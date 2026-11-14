// ==WindhawkMod==
// @id              copy-queue
// @name            Copy Queue
// @description     Queues Explorer copy operations while keeping them visible in the normal copy dialog
// @version         1.0
// @author          Nerdworld
// @github          https://github.com/nerdworldDE
// @homepage        https://nerdworld.de/
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -luuid
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Sequential Explorer Copy Queue
Copy queues Windows Explorer copy operations and runs them one after another instead of in parallel. The next copy operation starts automatically when the previous one has finished.

## Screenshot
![Copy dialog](https://i.imgur.com/7Jmifwt.png)

 */
// ==/WindhawkModReadme==

#include <windows.h>
#include <objbase.h>
#include <shobjidl.h>

#include <algorithm>
#include <atomic>
#include <cstdint>
#include <deque>
#include <unordered_set>

namespace {

// IFileOperation vtable indices, including the three IUnknown methods.
constexpr size_t kCopyItemIndex = 16;
constexpr size_t kCopyItemsIndex = 17;
constexpr size_t kPerformOperationsIndex = 21;

using CopyItem_t = HRESULT(STDMETHODCALLTYPE*)(
    IFileOperation* self,
    IShellItem* source,
    IShellItem* destinationFolder,
    LPCWSTR copyName,
    IFileOperationProgressSink* progressSink);

using CopyItems_t = HRESULT(STDMETHODCALLTYPE*)(
    IFileOperation* self,
    IUnknown* items,
    IShellItem* destinationFolder);

using PerformOperations_t = HRESULT(STDMETHODCALLTYPE*)(IFileOperation* self);

CopyItem_t g_copyItemOriginal = nullptr;
CopyItems_t g_copyItemsOriginal = nullptr;
PerformOperations_t g_performOperationsOriginal = nullptr;

SRWLOCK g_stateLock = SRWLOCK_INIT;
CONDITION_VARIABLE g_queueChanged = CONDITION_VARIABLE_INIT;

// IFileOperation objects on which CopyItem or CopyItems succeeded.
std::unordered_set<IFileOperation*> g_copyOperations;

HANDLE g_crossProcessMutex = nullptr;
HANDLE g_shutdownEvent = nullptr;

std::atomic_bool g_unloading = false;
thread_local bool g_insidePerformHook = false;

uint64_t g_nextOperationId = 1;

enum class TurnState {
    Waiting,
    AcquiringCrossProcessMutex,
    Ready,
    Aborted,
};

struct QueueEntry {
    uint64_t id = 0;
    TurnState state = TurnState::Waiting;
    bool queued = false;
    bool ownsCrossProcessMutex = false;
};

std::deque<QueueEntry*> g_queue;

struct HookTargets {
    HRESULT result = E_FAIL;
    void* copyItem = nullptr;
    void* copyItems = nullptr;
    void* performOperations = nullptr;
};

DWORD WINAPI ResolveHookTargetsThread(void* parameter) {
    auto* targets = static_cast<HookTargets*>(parameter);

    HRESULT initializeResult = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    if (FAILED(initializeResult)) {
        targets->result = initializeResult;
        return 0;
    }

    IFileOperation* fileOperation = nullptr;
    HRESULT result = CoCreateInstance(
        CLSID_FileOperation,
        nullptr,
        CLSCTX_INPROC_SERVER,
        IID_IFileOperation,
        reinterpret_cast<void**>(&fileOperation));

    if (SUCCEEDED(result) && fileOperation) {
        void** vtable = *reinterpret_cast<void***>(fileOperation);
        targets->copyItem = vtable[kCopyItemIndex];
        targets->copyItems = vtable[kCopyItemsIndex];
        targets->performOperations = vtable[kPerformOperationsIndex];
        fileOperation->Release();
    }

    targets->result = result;
    CoUninitialize();
    return 0;
}

bool ResolveHookTargets(HookTargets* targets) {
    HANDLE thread = CreateThread(
        nullptr, 0, ResolveHookTargetsThread, targets, 0, nullptr);
    if (!thread) {
        targets->result = HRESULT_FROM_WIN32(GetLastError());
        return false;
    }

    WaitForSingleObject(thread, INFINITE);
    CloseHandle(thread);

    return SUCCEEDED(targets->result) &&
           targets->copyItem &&
           targets->copyItems &&
           targets->performOperations;
}

void MarkAsCopyOperation(IFileOperation* operation) {
    AcquireSRWLockExclusive(&g_stateLock);
    g_copyOperations.insert(operation);
    ReleaseSRWLockExclusive(&g_stateLock);
}

bool ConsumeCopyOperationMark(IFileOperation* operation) {
    AcquireSRWLockExclusive(&g_stateLock);
    bool isCopy = g_copyOperations.erase(operation) != 0;
    ReleaseSRWLockExclusive(&g_stateLock);
    return isCopy;
}

void RegisterQueueEntry(QueueEntry* entry) {
    AcquireSRWLockExclusive(&g_stateLock);

    entry->id = g_nextOperationId++;
    entry->state = TurnState::Waiting;
    entry->queued = true;
    entry->ownsCrossProcessMutex = false;
    g_queue.push_back(entry);

    Wh_Log(
        L"Copy operation registered: id=%llu, position=%llu",
        static_cast<unsigned long long>(entry->id),
        static_cast<unsigned long long>(g_queue.size()));

    ReleaseSRWLockExclusive(&g_stateLock);
}

bool AcquireCrossProcessTurn(QueueEntry* entry) {
    HANDLE handles[] = {g_shutdownEvent, g_crossProcessMutex};
    DWORD waitResult = WaitForMultipleObjects(2, handles, FALSE, INFINITE);

    if (waitResult == WAIT_OBJECT_0 + 1 ||
        waitResult == WAIT_ABANDONED_0 + 1) {
        entry->ownsCrossProcessMutex = true;
        return true;
    }

    return false;
}

bool WaitForQueueTurn(QueueEntry* entry) {
    bool acquireCrossProcessMutex = false;

    for (;;) {
        AcquireSRWLockExclusive(&g_stateLock);

        if (g_unloading.load(std::memory_order_acquire) || !entry->queued) {
            entry->state = TurnState::Aborted;
            ReleaseSRWLockExclusive(&g_stateLock);
            return false;
        }

        if (entry->state == TurnState::Ready) {
            ReleaseSRWLockExclusive(&g_stateLock);
            return true;
        }

        if (entry->state == TurnState::Aborted) {
            ReleaseSRWLockExclusive(&g_stateLock);
            return false;
        }

        const bool isFirst = !g_queue.empty() && g_queue.front() == entry;

        if (isFirst && entry->state == TurnState::Waiting) {
            entry->state = TurnState::AcquiringCrossProcessMutex;
            acquireCrossProcessMutex = true;
            ReleaseSRWLockExclusive(&g_stateLock);
            break;
        }

        SleepConditionVariableSRW(
            &g_queueChanged,
            &g_stateLock,
            INFINITE,
            0);
        ReleaseSRWLockExclusive(&g_stateLock);
    }

    const bool acquired = acquireCrossProcessMutex &&
                          AcquireCrossProcessTurn(entry);

    AcquireSRWLockExclusive(&g_stateLock);

    if (acquired &&
        !g_unloading.load(std::memory_order_acquire) &&
        entry->queued) {
        entry->state = TurnState::Ready;
        Wh_Log(
            L"Copy operation released for execution: id=%llu",
            static_cast<unsigned long long>(entry->id));
    } else {
        entry->state = TurnState::Aborted;
    }

    WakeAllConditionVariable(&g_queueChanged);
    const bool ready = entry->state == TurnState::Ready;
    ReleaseSRWLockExclusive(&g_stateLock);

    return ready;
}

void CompleteQueueEntry(QueueEntry* entry) {
    // Release the process-wide lock before advancing the local queue. This
    // prevents the next local entry from racing against the old owner.
    if (entry->ownsCrossProcessMutex && g_crossProcessMutex) {
        ReleaseMutex(g_crossProcessMutex);
        entry->ownsCrossProcessMutex = false;
    }

    AcquireSRWLockExclusive(&g_stateLock);

    auto iterator = std::find(g_queue.begin(), g_queue.end(), entry);
    if (iterator != g_queue.end()) {
        g_queue.erase(iterator);
    }

    entry->queued = false;
    if (entry->state != TurnState::Ready) {
        entry->state = TurnState::Aborted;
    }

    WakeAllConditionVariable(&g_queueChanged);
    ReleaseSRWLockExclusive(&g_stateLock);
}

class QueueProgressSink final : public IFileOperationProgressSink {
public:
    explicit QueueProgressSink(QueueEntry* entry) : entry_(entry) {}

    HRESULT STDMETHODCALLTYPE QueryInterface(
        REFIID riid,
        void** object) override {
        if (!object) {
            return E_POINTER;
        }

        *object = nullptr;

        if (IsEqualIID(riid, IID_IUnknown) ||
            IsEqualIID(riid, __uuidof(IFileOperationProgressSink))) {
            *object = static_cast<IFileOperationProgressSink*>(this);
            AddRef();
            return S_OK;
        }

        return E_NOINTERFACE;
    }

    ULONG STDMETHODCALLTYPE AddRef() override {
        return static_cast<ULONG>(InterlockedIncrement(&referenceCount_));
    }

    ULONG STDMETHODCALLTYPE Release() override {
        LONG count = InterlockedDecrement(&referenceCount_);
        if (count == 0) {
            delete this;
        }
        return static_cast<ULONG>(count);
    }

    HRESULT STDMETHODCALLTYPE StartOperations() override {
        Wh_Log(
            L"Explorer started visual copy operation: id=%llu",
            static_cast<unsigned long long>(entry_->id));
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE FinishOperations(HRESULT result) override {
        Wh_Log(
            L"Explorer finished visual copy operation: id=%llu, result=0x%08X",
            static_cast<unsigned long long>(entry_->id),
            static_cast<unsigned int>(result));
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE PreRenameItem(
        DWORD,
        IShellItem*,
        LPCWSTR) override {
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE PostRenameItem(
        DWORD,
        IShellItem*,
        LPCWSTR,
        HRESULT,
        IShellItem*) override {
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE PreMoveItem(
        DWORD,
        IShellItem*,
        IShellItem*,
        LPCWSTR) override {
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE PostMoveItem(
        DWORD,
        IShellItem*,
        IShellItem*,
        LPCWSTR,
        HRESULT,
        IShellItem*) override {
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE PreCopyItem(
        DWORD,
        IShellItem*,
        IShellItem*,
        LPCWSTR) override {
        return WaitBeforeTransfer();
    }

    HRESULT STDMETHODCALLTYPE PostCopyItem(
        DWORD,
        IShellItem*,
        IShellItem*,
        LPCWSTR,
        HRESULT,
        IShellItem*) override {
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE PreDeleteItem(
        DWORD,
        IShellItem*) override {
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE PostDeleteItem(
        DWORD,
        IShellItem*,
        HRESULT,
        IShellItem*) override {
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE PreNewItem(
        DWORD,
        IShellItem*,
        LPCWSTR) override {
        // Folder copies can create destination directories before the first
        // PreCopyItem callback, so this callback must also acquire the turn.
        return WaitBeforeTransfer();
    }

    HRESULT STDMETHODCALLTYPE PostNewItem(
        DWORD,
        IShellItem*,
        LPCWSTR,
        LPCWSTR,
        DWORD,
        HRESULT,
        IShellItem*) override {
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE UpdateProgress(UINT, UINT) override {
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE ResetTimer() override {
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE PauseTimer() override {
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE ResumeTimer() override {
        return S_OK;
    }

private:
    HRESULT WaitBeforeTransfer() {
        return WaitForQueueTurn(entry_)
                   ? S_OK
                   : HRESULT_FROM_WIN32(ERROR_CANCELLED);
    }

    volatile LONG referenceCount_ = 1;
    QueueEntry* entry_;
};

HRESULT STDMETHODCALLTYPE CopyItemHook(
    IFileOperation* self,
    IShellItem* source,
    IShellItem* destinationFolder,
    LPCWSTR copyName,
    IFileOperationProgressSink* progressSink) {
    HRESULT result = g_copyItemOriginal(
        self,
        source,
        destinationFolder,
        copyName,
        progressSink);

    if (SUCCEEDED(result)) {
        MarkAsCopyOperation(self);
    }

    return result;
}

HRESULT STDMETHODCALLTYPE CopyItemsHook(
    IFileOperation* self,
    IUnknown* items,
    IShellItem* destinationFolder) {
    HRESULT result = g_copyItemsOriginal(self, items, destinationFolder);

    if (SUCCEEDED(result)) {
        MarkAsCopyOperation(self);
    }

    return result;
}

HRESULT STDMETHODCALLTYPE PerformOperationsHook(IFileOperation* self) {
    if (g_insidePerformHook ||
        g_unloading.load(std::memory_order_acquire) ||
        !ConsumeCopyOperationMark(self)) {
        return g_performOperationsOriginal(self);
    }

    QueueEntry entry;
    RegisterQueueEntry(&entry);

    auto* progressSink = new QueueProgressSink(&entry);
    DWORD adviseCookie = 0;
    HRESULT adviseResult = self->Advise(progressSink, &adviseCookie);

    if (FAILED(adviseResult)) {
        Wh_Log(
            L"IFileOperation::Advise failed for id=%llu: 0x%08X; "
            L"falling back to non-visual waiting",
            static_cast<unsigned long long>(entry.id),
            static_cast<unsigned int>(adviseResult));

        // Preserve serialization even if the progress sink cannot be attached.
        WaitForQueueTurn(&entry);
    } else {
        Wh_Log(
            L"Progress sink attached: id=%llu, cookie=%u",
            static_cast<unsigned long long>(entry.id),
            adviseCookie);
    }

    // The crucial difference from version 0.1.0: the original operation is
    // entered immediately. Waiting happens from the progress sink callbacks.
    g_insidePerformHook = true;
    HRESULT result = g_performOperationsOriginal(self);
    g_insidePerformHook = false;

    if (SUCCEEDED(adviseResult)) {
        HRESULT unadviseResult = self->Unadvise(adviseCookie);
        if (FAILED(unadviseResult)) {
            Wh_Log(
                L"IFileOperation::Unadvise failed for id=%llu: 0x%08X",
                static_cast<unsigned long long>(entry.id),
                static_cast<unsigned int>(unadviseResult));
        }
    }

    progressSink->Release();

    Wh_Log(
        L"PerformOperations returned: id=%llu, result=0x%08X",
        static_cast<unsigned long long>(entry.id),
        static_cast<unsigned int>(result));

    CompleteQueueEntry(&entry);
    return result;
}

}  // namespace

BOOL Wh_ModInit() {
    Wh_Log(L"Initializing Copy Queue 0.2.0");

    g_shutdownEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_crossProcessMutex = CreateMutexW(
        nullptr,
        FALSE,
        L"Local\\Windhawk.SequentialExplorerCopy.v2");

    if (!g_shutdownEvent || !g_crossProcessMutex) {
        Wh_Log(
            L"Failed to create synchronization objects: %u",
            GetLastError());
        return FALSE;
    }

    HookTargets targets;
    if (!ResolveHookTargets(&targets)) {
        Wh_Log(
            L"Failed to resolve IFileOperation methods: 0x%08X",
            static_cast<unsigned int>(targets.result));
        return FALSE;
    }

    if (!Wh_SetFunctionHook(
            targets.copyItem,
            reinterpret_cast<void*>(CopyItemHook),
            reinterpret_cast<void**>(&g_copyItemOriginal)) ||
        !Wh_SetFunctionHook(
            targets.copyItems,
            reinterpret_cast<void*>(CopyItemsHook),
            reinterpret_cast<void**>(&g_copyItemsOriginal)) ||
        !Wh_SetFunctionHook(
            targets.performOperations,
            reinterpret_cast<void*>(PerformOperationsHook),
            reinterpret_cast<void**>(&g_performOperationsOriginal))) {
        Wh_Log(L"Failed to register one or more hooks");
        return FALSE;
    }

    Wh_Log(L"IFileOperation hooks registered for version 0.2.0");
    return TRUE;
}

void Wh_ModBeforeUninit() {
    g_unloading.store(true, std::memory_order_release);

    if (g_shutdownEvent) {
        SetEvent(g_shutdownEvent);
    }

    WakeAllConditionVariable(&g_queueChanged);
}

void Wh_ModUninit() {
    // Active PerformOperations calls can still be returning while the mod is
    // being disabled. The process will reclaim these handles at exit.
    Wh_Log(L"Copy Queue unloaded");
}
