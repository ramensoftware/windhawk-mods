// ==WindhawkMod==
// @id              explorer-fast-navigation
// @name            Explorer Fast Navigation
// @description     Improves File Explorer navigation latency and eliminates redraw flicker
// @version         1.1.0
// @author          gilnett
// @github          https://github.com/gilnett
// @donateUrl       https://buymeacoffee.com/gilnett
// @license         GPL-3.0
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lcomctl32 -lshell32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Explorer Fast Navigation

Makes folder navigation faster and less visually disruptive in File Explorer by removing intermediate redraw flicker
and deferring toolbar updates until folder contents are loaded.

## How it works

In Windows 10 and Windows 11, switching folders triggers intermediate window clearing ("Working on it..." placeholder)
and multiple synchronous redraw events from the Ribbon and XAML command bar before files are enumerated.

This mod optimizes the navigation pipeline:
- **Redraw suppression**: Suppresses intermediate window clears and synchronous redraw flushes during folder transitions.
- **Deferred toolbar updates**: Defers Ribbon and XAML command bar state notifications while files are being enumerated,
  flushing them once file enumeration completes (`DISPID_FILELISTENUMDONE`) or batch processing settles.
- **UI freeze reduction**: Prevents the UI thread from locking up during folder switches and column grouping changes,
  lowering CPU repaint spikes.
- **Safety fallback**: Incorporates a configurable safety ceiling to ensure pending UI updates always flush even on
  slow network drives or non-responsive storage.

## Observed results

Real-world behavior varies depending on the storage medium and system load:
- **High-speed NVMe drives**: Windows virtualizes viewports quickly, but the mod noticeably removes the transient white
  flicker, suppresses UI repaint churn, and keeps window headers responsive.
- **Mechanical hard drives and laptops**: On rotational HDDs or resource-constrained mobile CPUs where disk seek times
  and background file enumeration can cause visible stalls, the mod eliminates the intermediate "Working on it..."
  freeze and keeps the Explorer interface smooth during heavy folder browsing and column grouping operations.

## Settings

- **Defer immediate navigation paints**: Defer immediate activation and layout paints while Explorer switches views.
- **Defer classic ribbon updates**: Delay classic ribbon refreshes during navigation.
- **Defer XAML command bar updates**: Delay modern command bar refreshes during navigation.
- **Defer navigation bar updates**: Delay navigation state updates in classic and modern navigation bars.
- **Accelerate grouped folder batching**: Shorten Explorer's internal category grouping timer delay on grouped views
  for faster folder presentation.
- **Safety fallback ceiling (ms)**: Maximum time (in ms) to wait before forcing a deferred UI update to flush if
  navigation stalls or on slow storage.

## Compatibility

- Windows 11 (21H2, 22H2, 23H2, 24H2)
- Windows 10 (1809 and later)
- Architectures: 64-bit (x86-64, ARM64)

## Support

If you find this mod useful, you can support its development and maintenance:
- [Support on Buy Me a Coffee](https://buymeacoffee.com/gilnett)

## Credits

- Based on the original implementation by **Vivy** ([@VivyVCCS](https://x.com/VivyVCCS) / [enginelesscc](https://github.com/enginelesscc)).
- Modernized, refactored, and maintained by **gilnett**.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- navigationPaint: true
  $name: Defer immediate navigation paints
  $description: Defer immediate activation and layout paints while Explorer switches views. Native loading and redraw
    protection remain active.
- classicRibbon: true
  $name: Defer classic ribbon updates
  $description: Delay classic ribbon refreshes during navigation.
- xamlCommandBar: true
  $name: Defer XAML command bar updates
  $description: Delay modern command bar refreshes during navigation. Navigation bar updates have a separate switch.
- navigationBar: true
  $name: Defer navigation bar updates
  $description: Delay navigation state updates in both the classic and modern navigation bars.
- groupedFolderBatching: true
  $name: Accelerate grouped folder batching
  $description: Shorten Explorer's internal grouping timer interval on grouped views while preserving standard
    non-grouped behavior.
- safetyCeilingMs: 2000
  $name: Safety fallback ceiling (ms)
  $description: Maximum time in milliseconds before forcing pending UI updates to flush if navigation stalls or on slow network drives.
*/
// ==/WindhawkModSettings==

#include <windows.h>

#include <oaidl.h>
#include <shdispid.h>
#include <shlobj.h>
#include <windhawk_utils.h>
#include <wrl/client.h>

#include <algorithm>
#include <array>
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstddef>
#include <deque>
#include <memory>
#include <mutex>
#include <vector>

// Shared types.

struct Settings {
    bool navigationPaint = true;
    bool classicRibbon = true;
    bool xamlCommandBar = true;
    bool navigationBar = true;
    bool groupedFolderBatching = true;
    int safetyCeilingMs = 2000;
};

// Synchronize window work and mod unload. Never hold mutex across window/COM calls:
// they can reenter the mod on an owning UI thread.
class WindowLifetime {
    std::mutex mutex;
    std::condition_variable changed;
    std::atomic<bool> stopping{false};
    std::atomic<size_t> operations{0};
    std::atomic<size_t> callbacks{0};
    std::vector<HWND> windows;

  public:
    class Operation {
        WindowLifetime& owner;
        bool admitted = false;

      public:
        explicit Operation(WindowLifetime& owner) : owner(owner) {
            if (owner.stopping.load(std::memory_order_seq_cst)) {
                return;
            }
            owner.operations.fetch_add(1, std::memory_order_seq_cst);
            if (owner.stopping.load(std::memory_order_seq_cst)) {
                if (owner.operations.fetch_sub(1, std::memory_order_seq_cst) == 1) {
                    std::lock_guard lock(owner.mutex);
                    owner.changed.notify_all();
                }
                return;
            }
            admitted = true;
        }

        Operation(const Operation&) = delete;
        Operation& operator=(const Operation&) = delete;

        ~Operation() {
            if (admitted) {
                if (owner.operations.fetch_sub(1, std::memory_order_seq_cst) == 1 &&
                    owner.stopping.load(std::memory_order_seq_cst)) {
                    std::lock_guard lock(owner.mutex);
                    owner.changed.notify_all();
                }
            }
        }

        explicit operator bool() const {
            return admitted;
        }
    };

    class Callback {
        WindowLifetime& owner;

      public:
        explicit Callback(WindowLifetime& owner) : owner(owner) {
            owner.callbacks.fetch_add(1, std::memory_order_seq_cst);
        }

        Callback(const Callback&) = delete;
        Callback& operator=(const Callback&) = delete;

        ~Callback() {
            if (owner.callbacks.fetch_sub(1, std::memory_order_seq_cst) == 1 &&
                owner.stopping.load(std::memory_order_seq_cst)) {
                std::lock_guard lock(owner.mutex);
                owner.changed.notify_all();
            }
        }
    };

    void Add(HWND hwnd) {
        std::lock_guard lock(mutex);
        windows.push_back(hwnd);
    }

    void Remove(HWND hwnd) {
        std::lock_guard lock(mutex);
        std::erase(windows, hwnd);
        changed.notify_all();
    }

    void Stop() {
        std::lock_guard lock(mutex);
        stopping.store(true, std::memory_order_seq_cst);
    }

    void CloseAndWait() {
        std::vector<HWND> copy;
        {
            std::unique_lock lock(mutex);
            changed.wait(lock, [this] {
                return operations.load(std::memory_order_seq_cst) == 0;
            });
            copy = windows;
        }
        for (HWND hwnd : copy) {
            PostMessageW(hwnd, WM_CLOSE, 0, 0);
        }
        std::unique_lock lock(mutex);
        const ULONGLONG startTick = GetTickCount64();
        while (!changed.wait_for(lock, std::chrono::seconds(2), [this] {
            std::erase_if(windows, [](HWND hwnd) { return !IsWindow(hwnd); });
            return windows.empty() && callbacks.load(std::memory_order_seq_cst) == 0;
        })) {
            const ULONGLONG elapsed = GetTickCount64() - startTick;
            Wh_Log(L"[LIFETIME] Waiting for %zu queue window(s) and %zu callback(s) (elapsed=%llu s)",
                   windows.size(), callbacks.load(std::memory_order_relaxed), elapsed / 1000);
            if (elapsed >= 4000) {
                for (HWND h : windows) {
                    if (IsWindow(h)) {
                        PostMessageW(h, WM_CLOSE, 0, 0);
                    }
                }
            }
        }
    }
};

using Callback = void (*)(void*);

struct Action { // GMA_ACTION ABI; checked before reading callback/parameters.
    DWORD size{};
    float startDelay{};
    float duration{};
    float repeatDelay{};
    int repeatCount{};
    int timeSlice{};
    Callback callback{};
    void* context{};
};

#if defined(_WIN64)
static_assert(sizeof(Action) == 40 && offsetof(Action, callback) == 24);
#endif

using CreateAction = uintptr_t (*)(const Action*);
using DeleteAction = BOOL(WINAPI*)(void*);

using RedrawFrame = void(__fastcall*)(void*);
using RenderSizer = HRESULT(__fastcall*)(void*);

struct PaintScope {
    unsigned& depth;

    explicit PaintScope(unsigned& d) : depth(d) {
        ++depth;
    }

    PaintScope(const PaintScope&) = delete;
    PaintScope& operator=(const PaintScope&) = delete;

    ~PaintScope() {
        --depth;
    }
};

// File-wide state and original function pointers.

static Settings settings;

static Callback batchCallback;
static RedrawFrame redrawFrameOriginal;
static RenderSizer renderSizerOriginal;
static thread_local unsigned redrawFrameDepth;
static thread_local unsigned renderSizerDepth;

static CreateAction createActionOriginal;
static DeleteAction deleteActionOriginal;
static bool trackBatchActions;
static decltype(&UpdateWindow) updateWindowOriginal;
static decltype(&RedrawWindow) redrawWindowOriginal;
static decltype(&SetWindowPos) setWindowPosOriginal;
static decltype(&LoadLibraryExW) loadLibraryExWOriginal;

static HMODULE frame;
static HMODULE shell;
static HMODULE duser;

// Shared helpers.

static void LoadSettings() {
    settings.navigationPaint = Wh_GetIntSetting(L"navigationPaint") != 0;
    settings.classicRibbon = Wh_GetIntSetting(L"classicRibbon") != 0;
    settings.xamlCommandBar = Wh_GetIntSetting(L"xamlCommandBar") != 0;
    settings.navigationBar = Wh_GetIntSetting(L"navigationBar") != 0;
    settings.groupedFolderBatching = Wh_GetIntSetting(L"groupedFolderBatching") != 0;
    settings.safetyCeilingMs =
        std::clamp(Wh_GetIntSetting(L"safetyCeilingMs"), 200, 10000);
}

static constexpr GUID kNullIid{};
static constexpr UINT kMsgMetadataResult = WM_APP + 1;

template <size_t Capacity>
class BoundedPtrSet {
  public:
    [[nodiscard]] bool contains(const void* ptr) const noexcept {
        if (!ptr) {
            return false;
        }
        for (size_t i = 0; i < m_count; ++i) {
            if (m_entries[i] == ptr) {
                return true;
            }
        }
        return false;
    }

    void insert(void* ptr) noexcept {
        if (!ptr) {
            return;
        }
        for (size_t i = 0; i < m_count; ++i) {
            if (m_entries[i] == ptr) {
                if (i + 1 < m_count) {
                    std::rotate(m_entries.begin() + static_cast<ptrdiff_t>(i),
                                m_entries.begin() + static_cast<ptrdiff_t>(i + 1),
                                m_entries.begin() + static_cast<ptrdiff_t>(m_count));
                }
                return;
            }
        }
        if (m_count == Capacity) {
            std::shift_left(m_entries.begin(), m_entries.end(), 1);
            --m_count;
        }
        m_entries[m_count++] = ptr;
    }

    bool erase(const void* ptr) noexcept {
        if (!ptr) {
            return false;
        }
        for (size_t i = 0; i < m_count; ++i) {
            if (m_entries[i] == ptr) {
                for (size_t j = i; j + 1 < m_count; ++j) {
                    m_entries[j] = m_entries[j + 1];
                }
                m_entries[--m_count] = nullptr;
                return true;
            }
        }
        return false;
    }

    void clear() noexcept {
        m_entries.fill(nullptr);
        m_count = 0;
    }

    [[nodiscard]] bool empty() const noexcept {
        return m_count == 0;
    }

  private:
    std::array<void*, Capacity> m_entries{};
    size_t m_count = 0;
};

static Microsoft::WRL::ComPtr<IUnknown> CanonicalUnknown(IUnknown* obj) noexcept {
    Microsoft::WRL::ComPtr<IUnknown> canonical;
    if (!obj) {
        return canonical;
    }
    if (FAILED(obj->QueryInterface(IID_PPV_ARGS(&canonical))) || !canonical) {
        canonical = obj;
    }
    return canonical;
}

static bool CanDeferInvoke(REFIID iid, WORD flags, const DISPPARAMS* args) {
    return IsEqualGUID(iid, kNullIid) && flags == DISPATCH_METHOD && args &&
           args->cArgs == 0 && args->cNamedArgs == 0;
}

static BOOL IsShellDesktopOrTrayWindow(HWND hwnd) {
    if (!hwnd) {
        return FALSE;
    }
    wchar_t name[64];
    if (GetClassNameW(hwnd, name, ARRAYSIZE(name))) {
        if (wcscmp(name, L"Progman") == 0 || wcscmp(name, L"WorkerW") == 0 ||
            wcscmp(name, L"Shell_TrayWnd") == 0 ||
            wcscmp(name, L"Shell_SecondaryTrayWnd") == 0) {
            return TRUE;
        }
    }
    return FALSE;
}

// If a thread hosts multiple CabinetWClass windows (e.g. multi-window or dual-pane
// scenarios hosted on the same thread), returning nullptr intentionally disables deferral
// for this thread. The deferral queue is thread-local and subclasses a single root window;
// sharing it across multiple windows would cause cross-talk and state synchronization hazards.
static BOOL CALLBACK FindBrowserWindowProc(HWND hwnd, LPARAM param) {
    wchar_t name[64];
    if (!GetClassNameW(hwnd, name, ARRAYSIZE(name)) ||
        wcscmp(name, L"CabinetWClass") != 0) {
        return TRUE;
    }
    auto& found = *reinterpret_cast<HWND*>(param);
    if (found) {
        static thread_local bool loggedMultiWindow = false;
        if (!loggedMultiWindow) {
            loggedMultiWindow = true;
            Wh_Log(L"[TAB] Multiple CabinetWClass windows on thread %lu; deferral disabled",
                   GetCurrentThreadId());
        }
        found = nullptr;
        return FALSE;
    }
    found = hwnd;
    return TRUE;
}

static HWND FindThreadBrowserWindow() {
    HWND browser = nullptr;
    EnumThreadWindows(GetCurrentThreadId(), FindBrowserWindowProc,
                      reinterpret_cast<LPARAM>(&browser));
    return browser;
}

// Cross-namespace notifications used by FileTransition and MetadataWork.
namespace RibbonWork {
static bool DeferInvoke(void*, DISPID, REFIID, LCID, WORD, const DISPPARAMS*);
static void NavigationStarted();
static void ViewReady();
static bool Pending();
static HWND EnsureQueueWindow();
extern thread_local ULONGLONG started;
} // namespace RibbonWork

namespace XamlWork {
static void ClearThreadConnections() noexcept;
} // namespace XamlWork

namespace FileTransition {
static void ApplyMetadataResult(void* browserKey, ULONG generation, bool isSlowStorage);
static void ClearThreadTabs() noexcept;
} // namespace FileTransition

// Offload slow non-UI storage/cloud metadata checks to the Windows NT Threadpool.
namespace MetadataWork {
struct Result {
    void* browserKey = nullptr;
    ULONG generation = 0;
    bool isSlowStorage = false;
    bool isCloudPlaceholder = false;
    bool isUnc = false;
    bool isRemoteDrive = false;
};

struct PidlDeleter {
    void operator()(ITEMIDLIST_ABSOLUTE* pidl) const noexcept {
        if (pidl) {
            ILFree(pidl);
        }
    }
};
using UniquePidl = std::unique_ptr<ITEMIDLIST_ABSOLUTE, PidlDeleter>;

struct WorkJob {
    void* browserKey = nullptr;
    ULONG generation = 0;
    UniquePidl pidl{};
    HWND uiHwnd = nullptr;
};

using CfGetSyncRootInfoByPathFn = HRESULT(WINAPI*)(LPCWSTR filePath, DWORD infoClass,
                                                   PVOID infoBuffer, DWORD infoBufferLength,
                                                   DWORD* returnedLength);

static constexpr size_t kMaxPendingJobs = 32;

static TP_CALLBACK_ENVIRON tpEnv{};
static PTP_CLEANUP_GROUP cleanupGroup = nullptr;
static PTP_WORK sharedWork = nullptr;
static HMODULE cldapiModule = nullptr;
static CfGetSyncRootInfoByPathFn cfGetSyncRootInfoByPath = nullptr;

static std::mutex jobsMutex;
static std::deque<std::unique_ptr<WorkJob>> pendingJobs;
static std::atomic<bool> unloading{false};
static std::atomic<long> activeSubmitters{0};

struct SubmitterGuard {
    bool admitted = false;

    SubmitterGuard() noexcept {
        if (unloading.load(std::memory_order_seq_cst)) {
            return;
        }
        activeSubmitters.fetch_add(1, std::memory_order_seq_cst);
        if (unloading.load(std::memory_order_seq_cst)) {
            activeSubmitters.fetch_sub(1, std::memory_order_seq_cst);
            return;
        }
        admitted = true;
    }

    SubmitterGuard(const SubmitterGuard&) = delete;
    SubmitterGuard& operator=(const SubmitterGuard&) = delete;

    ~SubmitterGuard() {
        if (admitted) {
            activeSubmitters.fetch_sub(1, std::memory_order_seq_cst);
        }
    }

    explicit operator bool() const noexcept {
        return admitted;
    }
};

static bool IsUnloading() noexcept {
    return unloading.load(std::memory_order_acquire);
}

static bool IsCloudProviderAttributeOrProp(const wchar_t* fsPath) {
    if (fsPath && fsPath[0] != L'\0') {
        const DWORD attrs = GetFileAttributesW(fsPath);
        if (attrs != INVALID_FILE_ATTRIBUTES &&
            (attrs & (FILE_ATTRIBUTE_RECALL_ON_OPEN | FILE_ATTRIBUTE_RECALL_ON_DATA_ACCESS |
                      FILE_ATTRIBUTE_OFFLINE)) != 0) {
            return true;
        }
        if (cfGetSyncRootInfoByPath) {
            std::array<BYTE, 512> syncRootBuf{};
            DWORD returnedLen = 0;
            if (SUCCEEDED(cfGetSyncRootInfoByPath(fsPath, 0, syncRootBuf.data(),
                                                  static_cast<DWORD>(syncRootBuf.size()),
                                                  &returnedLen))) {
                return true;
            }
        }
    }
    return false;
}

static void InspectStorageMetadata(const WorkJob& job, Result& out) {
    out.browserKey = job.browserKey;
    out.generation = job.generation;
    if (!job.pidl) {
        return;
    }

    wchar_t rawPath[MAX_PATH]{};
    if (SHGetPathFromIDListW(job.pidl.get(), rawPath) && rawPath[0] != L'\0') {
        out.isUnc = (wcsncmp(rawPath, L"\\\\", 2) == 0);
        if (!out.isUnc && rawPath[0] != L'\0' && rawPath[1] == L':' && rawPath[2] == L'\\') {
            wchar_t root[4]{rawPath[0], L':', L'\\', L'\0'};
            const UINT driveType = GetDriveTypeW(root);
            out.isRemoteDrive = (driveType == DRIVE_REMOTE || driveType == DRIVE_CDROM);
        }
        if (!out.isUnc && !out.isRemoteDrive) {
            out.isCloudPlaceholder = IsCloudProviderAttributeOrProp(rawPath);
        }
    }
    out.isSlowStorage = out.isUnc || out.isRemoteDrive || out.isCloudPlaceholder;
}

static VOID CALLBACK WorkerCallback(PTP_CALLBACK_INSTANCE, PVOID, PTP_WORK) {
    std::unique_ptr<WorkJob> job;
    {
        std::lock_guard lock(jobsMutex);
        if (!pendingJobs.empty()) {
            job = std::move(pendingJobs.front());
            pendingJobs.pop_front();
        }
    }
    if (!job || unloading.load(std::memory_order_acquire)) {
        return;
    }

    auto res = std::make_unique<Result>();
    InspectStorageMetadata(*job, *res);

    if (unloading.load(std::memory_order_acquire)) {
        return;
    }

    if (job->uiHwnd && IsWindow(job->uiHwnd)) {
        Result* rawResult = res.release();
        if (unloading.load(std::memory_order_acquire) ||
            !PostMessageW(job->uiHwnd, kMsgMetadataResult, 0,
                          reinterpret_cast<LPARAM>(rawResult))) {
            delete rawResult;
        }
    }
}

static void EnqueueForNavigation(void* browserKey, ULONG generation, PCIDLIST_ABSOLUTE pidl,
                                 HWND uiHwnd) {
    if (unloading.load(std::memory_order_acquire) || !pidl || !browserKey || !uiHwnd) {
        return;
    }

    UniquePidl cloned(ILClone(pidl));
    if (!cloned) {
        return;
    }

    auto job = std::make_unique<WorkJob>();
    job->browserKey = browserKey;
    job->generation = generation;
    job->pidl = std::move(cloned);
    job->uiHwnd = uiHwnd;

    SubmitterGuard submitter;
    if (!submitter || !sharedWork) {
        return;
    }
    {
        std::lock_guard lock(jobsMutex);
        if (unloading.load(std::memory_order_acquire)) {
            return;
        }
        if (pendingJobs.size() >= kMaxPendingJobs) {
            pendingJobs.pop_front();
        }
        pendingJobs.push_back(std::move(job));
    }
    SubmitThreadpoolWork(sharedWork);
}

static bool Initialize() {
    unloading.store(false, std::memory_order_release);
    activeSubmitters.store(0, std::memory_order_release);

    cldapiModule = LoadLibraryExW(L"cldapi.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (cldapiModule) {
        cfGetSyncRootInfoByPath = reinterpret_cast<CfGetSyncRootInfoByPathFn>(
            GetProcAddress(cldapiModule, "CfGetSyncRootInfoByPath"));
    }

    InitializeThreadpoolEnvironment(&tpEnv);
    cleanupGroup = CreateThreadpoolCleanupGroup();
    if (!cleanupGroup) {
        DestroyThreadpoolEnvironment(&tpEnv);
        return false;
    }
    SetThreadpoolCallbackCleanupGroup(&tpEnv, cleanupGroup, nullptr);
    sharedWork = CreateThreadpoolWork(WorkerCallback, nullptr, &tpEnv);
    if (!sharedWork) {
        CloseThreadpoolCleanupGroupMembers(cleanupGroup, TRUE, nullptr);
        CloseThreadpoolCleanupGroup(cleanupGroup);
        cleanupGroup = nullptr;
        DestroyThreadpoolEnvironment(&tpEnv);
        return false;
    }
    return true;
}

static void BeginUnload() {
    unloading.store(true, std::memory_order_seq_cst);
    while (activeSubmitters.load(std::memory_order_seq_cst) != 0) {
        SwitchToThread();
    }
    {
        std::lock_guard lock(jobsMutex);
        pendingJobs.clear();
    }
    if (cleanupGroup) {
        CloseThreadpoolCleanupGroupMembers(cleanupGroup, TRUE, nullptr);
        CloseThreadpoolCleanupGroup(cleanupGroup);
        cleanupGroup = nullptr;
        sharedWork = nullptr;
        DestroyThreadpoolEnvironment(&tpEnv);
    }
}

static void Uninit() {
    cfGetSyncRootInfoByPath = nullptr;
    if (cldapiModule) {
        FreeLibrary(cldapiModule);
        cldapiModule = nullptr;
    }
}
} // namespace MetadataWork

// Track view completion for deferred UI work without taking over file painting.
namespace FileTransition {
static constexpr UINT kAdaptiveCheckMs = 45;
static constexpr UINT kBatchQuiescenceMs = 85;
static constexpr UINT kSlowBatchQuiescenceMs = 150;
static constexpr size_t kMaxTabs = 32;

struct State {
    void* browser{};
    void* itemsView{};
    void* batchAction{};
    ULONGLONG lastActivityTick = 0;
    ULONGLONG navStartTick = 0;
    ULONG generation = 0;
    size_t batchCount = 0;
    size_t lastCheckedBatchCount = 0;
    bool inUse = false;
    bool pending = false;
    bool enumerated = false;
    bool isGroupedView = false;
    bool isSlowStorage = false;
    bool telemetryReported = false;
};

class TabTable {
  public:
    State* FindByBrowser(const void* browser) noexcept {
        if (!browser) {
            return nullptr;
        }
        for (auto& entry : m_entries) {
            if (entry.inUse && entry.browser == browser) {
                return &entry;
            }
        }
        return nullptr;
    }

    State* FindByItemsView(const void* itemsView) noexcept {
        if (!itemsView) {
            return nullptr;
        }
        for (auto& entry : m_entries) {
            if (entry.inUse && entry.itemsView == itemsView) {
                return &entry;
            }
        }
        return nullptr;
    }

    State* FindOrAllocateByBrowser(void* browser) noexcept {
        if (State* existing = FindByBrowser(browser)) {
            return existing;
        }
        State* slot = AllocateSlot();
        slot->browser = browser;
        return slot;
    }

    State* FindOrAllocateByItemsView(void* itemsView, void* currentBrowser) noexcept {
        if (currentBrowser) {
            if (State* byBrowser = FindByBrowser(currentBrowser)) {
                if (itemsView) {
                    for (auto& entry : m_entries) {
                        if (&entry != byBrowser && entry.inUse && entry.itemsView == itemsView) {
                            entry.itemsView = nullptr;
                        }
                    }
                }
                byBrowser->itemsView = itemsView;
                return byBrowser;
            }
        }
        if (State* existing = FindByItemsView(itemsView);
            existing && (!currentBrowser || !existing->browser || existing->browser == currentBrowser)) {
            if (currentBrowser && !existing->browser) {
                existing->browser = currentBrowser;
            }
            return existing;
        }
        State* slot = AllocateSlot();
        if (itemsView) {
            for (auto& entry : m_entries) {
                if (&entry != slot && entry.inUse && entry.itemsView == itemsView) {
                    entry.itemsView = nullptr;
                }
            }
        }
        slot->browser = currentBrowser;
        slot->itemsView = itemsView;
        return slot;
    }

    void EraseByBrowser(const void* browser) noexcept {
        if (State* entry = FindByBrowser(browser)) {
            ResetEntry(*entry);
        }
    }

    void EraseByItemsView(const void* itemsView) noexcept {
        if (State* entry = FindByItemsView(itemsView)) {
            ResetEntry(*entry);
        }
    }

    void clear() noexcept {
        for (auto& entry : m_entries) {
            ResetEntry(entry);
        }
    }

  private:
    static void ResetEntry(State& entry) noexcept {
        entry = {};
    }

    State* AllocateSlot() noexcept {
        for (auto& entry : m_entries) {
            if (!entry.inUse) {
                ResetEntry(entry);
                entry.inUse = true;
                return &entry;
            }
        }
        State* victim = &m_entries[0];
        for (size_t i = 1; i < kMaxTabs; ++i) {
            State& cand = m_entries[i];
            const bool victimIdle = !victim->pending || victim->enumerated;
            const bool candIdle = !cand.pending || cand.enumerated;
            if ((candIdle && !victimIdle) ||
                (candIdle == victimIdle && cand.lastActivityTick < victim->lastActivityTick)) {
                victim = &cand;
            }
        }
        ResetEntry(*victim);
        victim->inUse = true;
        return victim;
    }

    std::array<State, kMaxTabs> m_entries{};
};

struct BrowserScope {
    void*& slot;
    void* previous;

    BrowserScope(void*& s, void* current) noexcept : slot(s), previous(s) {
        slot = current;
    }
    BrowserScope(const BrowserScope&) = delete;
    BrowserScope& operator=(const BrowserScope&) = delete;
    ~BrowserScope() {
        slot = previous;
    }
};

using ResetRoot = HRESULT(__fastcall*)(void*, void*, void*);
using Navigate = HRESULT(__fastcall*)(void*, PCIDLIST_ABSOLUTE, ULONG, ULONG);
using ConnectionCreated = HRESULT(__fastcall*)(void*, IShellItem*, IUnknown*, ULONG, ULONG,
                                               IUnknown*, IUnknown*, REFGUID);
using EnsureBatching = HRESULT(__fastcall*)(void*);
using BatchTimer = void(__fastcall*)(void*, void*);
using DeleteBatchTimer = bool(__fastcall*)(void*);
using Invoke = HRESULT(__fastcall*)(void*, DISPID, REFIID, LCID, WORD, DISPPARAMS*, VARIANT*,
                                    EXCEPINFO*, UINT*);

static thread_local TabTable tabs;
static thread_local State* active = nullptr;
static thread_local void* currentBrowser = nullptr;
static thread_local void* processingBatch = nullptr;
static thread_local unsigned navigationDepth = 0;
static thread_local ULONG nextTabGeneration = 0;
static bool completionTracking = true;

static ULONG NextGeneration() noexcept {
    ULONG next = ++nextTabGeneration;
    if (next == 0) {
        next = ++nextTabGeneration;
    }
    return next;
}

static ResetRoot resetOriginal;
static Navigate navigateOriginal;
static ConnectionCreated connectionOriginal;
static EnsureBatching ensureOriginal;
static BatchTimer batchOriginal;
static DeleteBatchTimer deleteOriginal;
static Invoke invokeOriginal;

static bool IsActiveViewGrouped() noexcept {
    return active != nullptr && active->inUse && active->isGroupedView;
}

static void OnGroupItem() {
    if (!active || !active->inUse) {
        return;
    }
    active->isGroupedView = true;
}

static void ApplyMetadataResult(void* browserKey, ULONG generation, bool isSlowStorage) {
    State* tab = tabs.FindByBrowser(browserKey);
    if (tab && tab->generation == generation) {
        tab->isSlowStorage = isSlowStorage;
    }
}

static void ClearThreadTabs() noexcept {
    tabs.clear();
    active = nullptr;
}

static void ReportTelemetryIfNeeded(const wchar_t* reason, ULONGLONG fallbackStartTick) {
    State* target = active;
    if (target && target->inUse) {
        if (target->telemetryReported) {
            return;
        }
        const ULONGLONG startTick = target->navStartTick ? target->navStartTick : fallbackStartTick;
        if (!startTick) {
            return;
        }
        target->telemetryReported = true;
        const ULONGLONG now = GetTickCount64();
        const ULONGLONG navMs = now >= startTick ? (now - startTick) : 0;
        Wh_Log(L"[TELEMETRY] Transition complete (navMs=%llu, batchCount=%zu, grouped=%d, "
               L"flushReason=%s)",
               navMs, target->batchCount, target->isGroupedView ? 1 : 0,
               reason ? reason : L"unknown");
    } else if (fallbackStartTick) {
        static thread_local ULONGLONG lastReportedFallbackTick = 0;
        if (lastReportedFallbackTick == fallbackStartTick) {
            return;
        }
        lastReportedFallbackTick = fallbackStartTick;
        const ULONGLONG now = GetTickCount64();
        const ULONGLONG navMs = now >= fallbackStartTick ? (now - fallbackStartTick) : 0;
        Wh_Log(L"[TELEMETRY] Transition complete (navMs=%llu, batchCount=%zu, grouped=%d, "
               L"flushReason=%s)",
               navMs, static_cast<size_t>(0), 0, reason ? reason : L"unknown");
    }
}

static bool DeferNavigationPaint() {
    if (!settings.navigationPaint || !navigationDepth) {
        return false;
    }
    if (RibbonWork::started &&
        (GetTickCount64() - RibbonWork::started >=
         static_cast<ULONGLONG>(settings.safetyCeilingMs))) {
        return false;
    }
    return true;
}

static bool IsReady() {
    return completionTracking && active && active->inUse && active->itemsView &&
           active->enumerated && !active->pending && !navigationDepth && !processingBatch;
}

static bool HasActiveWork() {
    if (navigationDepth > 0 || processingBatch != nullptr) {
        return true;
    }
    if (!active || !active->inUse || active->enumerated) {
        return false;
    }
    if (active->batchCount > active->lastCheckedBatchCount) {
        active->lastCheckedBatchCount = active->batchCount;
        return true;
    }
    const UINT quiescenceMs =
        active->isSlowStorage ? kSlowBatchQuiescenceMs : kBatchQuiescenceMs;
    if (GetTickCount64() - active->lastActivityTick <= quiescenceMs) {
        return true;
    }
    active->pending = false;
    return false;
}

static void NotifyReady() {
    if (IsReady()) {
        RibbonWork::ViewReady();
    }
}

static void EnumerationDone() {
    if (active && active->inUse && active->itemsView) {
        active->enumerated = true;
        active->pending = false;
        NotifyReady();
    }
}

static State* BeginForBrowser(void* browser) {
    State* tab = tabs.FindOrAllocateByBrowser(browser);
    const ULONGLONG now = GetTickCount64();
    const ULONG nextGen = NextGeneration();
    *tab = State{};
    tab->inUse = true;
    tab->browser = browser;
    tab->lastActivityTick = now;
    tab->navStartTick = now;
    tab->generation = nextGen;
    active = tab;
    return tab;
}

static State* BeginForItemsView(void* itemsView) {
    State* tab = tabs.FindOrAllocateByItemsView(itemsView, currentBrowser);
    const ULONGLONG now = GetTickCount64();
    void* savedBrowser = tab->browser;
    const bool keepStart =
        (currentBrowser != nullptr && tab->browser == currentBrowser && tab->navStartTick != 0);
    const ULONG nextGen = (keepStart && tab->generation != 0) ? tab->generation : NextGeneration();
    const ULONGLONG savedNavStart = keepStart ? tab->navStartTick : now;
    const bool savedSlow = keepStart ? tab->isSlowStorage : false;
    const bool savedGrouped = keepStart ? tab->isGroupedView : false;
    *tab = State{};
    tab->inUse = true;
    tab->browser = savedBrowser;
    tab->itemsView = itemsView;
    tab->lastActivityTick = now;
    tab->navStartTick = savedNavStart;
    tab->generation = nextGen;
    tab->isSlowStorage = savedSlow;
    tab->isGroupedView = savedGrouped;
    active = tab;
    return tab;
}

static HRESULT __fastcall NavigateHook(void* self, PCIDLIST_ABSOLUTE pidl, ULONG flags,
                                       ULONG extra) {
    RibbonWork::NavigationStarted();
    State* tab = BeginForBrowser(self);
    const ULONG generation = tab->generation;
    if (pidl) {
        const HWND uiHwnd = RibbonWork::EnsureQueueWindow();
        if (uiHwnd) {
            MetadataWork::EnqueueForNavigation(self, generation, pidl, uiHwnd);
        }
    }
    HRESULT result;
    {
        BrowserScope browserScope(currentBrowser, self);
        PaintScope scope(navigationDepth);
        result = navigateOriginal(self, pidl, flags, extra);
    }
    if (FAILED(result)) {
        tabs.EraseByBrowser(self);
        if (active == tab) {
            active = nullptr;
        }
    } else {
        if (tab->inUse && tab->generation == generation) {
            active = tab;
        }
        NotifyReady();
    }
    return result;
}

static HRESULT __fastcall ConnectionHook(void* self, IShellItem* item, IUnknown* connection,
                                         ULONG flags, ULONG extra, IUnknown* first,
                                         IUnknown* second, REFGUID id) {
    State* tab = tabs.FindOrAllocateByBrowser(self);
    tab->lastActivityTick = GetTickCount64();
    active = tab;
    HRESULT result;
    {
        BrowserScope browserScope(currentBrowser, self);
        PaintScope scope(navigationDepth);
        result = connectionOriginal(self, item, connection, flags, extra, first, second, id);
    }
    if (tab->inUse) {
        tab->lastActivityTick = GetTickCount64();
        active = tab;
    }
    NotifyReady();
    return result;
}

static HRESULT __fastcall ResetHook(void* self, void* item, void* collection) {
    if (!RibbonWork::Pending()) {
        RibbonWork::NavigationStarted();
    }
    State* tab = BeginForItemsView(self);
    const ULONG generation = tab->generation;
    HRESULT result;
    {
        PaintScope scope(navigationDepth);
        result = resetOriginal(self, item, collection);
    }
    if (tab->inUse && tab->generation == generation) {
        if (FAILED(result)) {
            tabs.EraseByItemsView(self);
            if (active == tab) {
                active = nullptr;
            }
        } else {
            tab->lastActivityTick = GetTickCount64();
            active = tab;
            NotifyReady();
        }
    }
    return result;
}

static HRESULT __fastcall EnsureHook(void* self) {
    State* tab = tabs.FindByItemsView(self);
    const ULONG generation = tab ? tab->generation : 0;
    HRESULT result = ensureOriginal(self);
    if (SUCCEEDED(result) && tab && tab->inUse && tab->itemsView == self &&
        tab->generation == generation) {
        tab->lastActivityTick = GetTickCount64();
        if (!tab->pending) {
            tab->pending = true;
            ++tab->batchCount;
        }
        active = tab;
    }
    return result;
}

static bool __fastcall DeleteHook(void* self) {
    State* tab = tabs.FindByItemsView(self);
    const ULONG generation = tab ? tab->generation : 0;
    bool result = deleteOriginal(self);
    if (result && tab && tab->inUse && tab->itemsView == self &&
        tab->generation == generation) {
        tab->pending = false;
        active = tab;
        NotifyReady();
    }
    return result;
}

static void __fastcall BatchHook(void* self, void* info) {
    void* previous = processingBatch;
    processingBatch = self;
    State* tab = tabs.FindByItemsView(self);
    if (tab && tab->inUse) {
        ++tab->batchCount;
        tab->lastActivityTick = GetTickCount64();
        active = tab;
    }
    batchOriginal(self, info);
    if (tab && tab->inUse) {
        tab->lastActivityTick = GetTickCount64();
        active = tab;
    }
    processingBatch = previous;
    NotifyReady();
}

static HRESULT __fastcall InvokeHook(void* self, DISPID id, REFIID iid, LCID locale, WORD flags,
                                     DISPPARAMS* args, VARIANT* result, EXCEPINFO* exception,
                                     UINT* argError) {
    HRESULT hr;
    if (RibbonWork::DeferInvoke(self, id, iid, locale, flags, args)) {
        if (result) {
            *result = {};
        }
        hr = S_OK;
    } else {
        hr = invokeOriginal(self, id, iid, locale, flags, args, result, exception, argError);
    }
    if (id == DISPID_FILELISTENUMDONE) {
        EnumerationDone();
    }
    return hr;
}

} // namespace FileTransition

// Run ribbon state refreshes on the owning UI thread after native view painting.
namespace RibbonWork {
enum class TaskKind : unsigned char {
    ClassicInvoke,
    ClassicNavigated,
    ClassicNavState,
    XamlNavigated,
    XamlInvalidated,
    XamlNavState,
    XamlInvalidateCommands,
    XamlInvoke,
};

struct Task {
    using Invoker = void (*)(const Task&);

    Microsoft::WRL::ComPtr<IUnknown> object{};
    void* target = nullptr;
    Invoker run = nullptr;
    ULONG param = 0;
    LCID locale = 0;
    WORD flags = 0;
    TaskKind kind{};

    void Reset() noexcept {
        object.Reset();
        target = nullptr;
        run = nullptr;
        param = 0;
        locale = 0;
        flags = 0;
        kind = {};
    }
};

class TaskQueue {
  public:
    static constexpr size_t kCapacity = 64;

    [[nodiscard]] bool empty() const noexcept {
        return m_count == 0;
    }

    [[nodiscard]] size_t size() const noexcept {
        return m_count;
    }

    [[nodiscard]] Task& operator[](size_t index) noexcept {
        return m_buffer[(m_head + index) & (kCapacity - 1)];
    }

    [[nodiscard]] const Task& operator[](size_t index) const noexcept {
        return m_buffer[(m_head + index) & (kCapacity - 1)];
    }

    bool Push(Task&& task) noexcept {
        if (m_count >= kCapacity) {
            return false;
        }
        size_t tail = (m_head + m_count) & (kCapacity - 1);
        m_buffer[tail] = std::move(task);
        task.Reset();
        ++m_count;
        return true;
    }

    Task Pop() noexcept {
        if (m_count == 0) {
            return Task{};
        }
        Task task = std::move(m_buffer[m_head]);
        m_buffer[m_head].Reset();
        m_head = (m_head + 1) & (kCapacity - 1);
        --m_count;
        if (m_count == 0) {
            m_head = 0;
        }
        return task;
    }

    void DropTarget(void* target) noexcept {
        if (!target || m_count == 0) {
            return;
        }
        size_t writeIdx = 0;
        for (size_t readIdx = 0; readIdx < m_count; ++readIdx) {
            size_t actualIdx = (m_head + readIdx) & (kCapacity - 1);
            if (m_buffer[actualIdx].object.Get() == target ||
                m_buffer[actualIdx].target == target) {
                m_buffer[actualIdx].Reset();
            } else {
                if (writeIdx != readIdx) {
                    size_t destIdx = (m_head + writeIdx) & (kCapacity - 1);
                    m_buffer[destIdx] = std::move(m_buffer[actualIdx]);
                    m_buffer[actualIdx].Reset();
                }
                ++writeIdx;
            }
        }
        m_count = writeIdx;
        if (m_count == 0) {
            m_head = 0;
        }
    }

  private:
    std::array<Task, kCapacity> m_buffer{};
    size_t m_head = 0;
    size_t m_count = 0;
};

struct Queue {
    HWND hwnd{};
    HWND root{};
    HHOOK inputHook{};
    TaskQueue tasks{};
    bool timerActive = false;
    bool draining = false;
    bool closing = false;
};

using Notification = HRESULT(__fastcall*)(void*);
using Destroy = HRESULT(__fastcall*)(void*, BOOL);
using NavState = HRESULT(__fastcall*)(void*, ULONG);

static constexpr wchar_t ClassName[] = L"Windhawk.ExplorerFastNavigation.RibbonWork";

static thread_local Queue* queue;
static thread_local BoundedPtrSet<32> connectedTargets;
thread_local ULONGLONG started = 0;
static thread_local bool ready;
static WindowLifetime lifetime;
static HINSTANCE instance;
static bool registered;

static Notification navigatedOriginal;
static Notification changedOriginal;
static Destroy destroyOriginal;
static NavState navStateOriginal;

static void Flush(const wchar_t* reason = L"unknown") {
    WindowLifetime::Callback callback(lifetime);
    if (started) {
        FileTransition::ReportTelemetryIfNeeded(reason, started);
    }
    Queue* q = queue;
    if (!q || q->draining) {
        return;
    }
    q->draining = true;
    if (q->timerActive) {
        KillTimer(q->hwnd, 1);
        q->timerActive = false;
    }
    while (!q->tasks.empty()) {
        Task task = q->tasks.Pop();
        if (!q->closing && task.run) {
            task.run(task);
        }
    }
    q->draining = false;
    if (q->closing) {
        DestroyWindow(q->hwnd);
    }
}

static void DropTasksForTarget(void* target) {
    if (!queue || !target) {
        return;
    }
    if (!queue->tasks.empty()) {
        auto canonical = CanonicalUnknown(reinterpret_cast<IUnknown*>(target));
        if (canonical && canonical.Get() != target) {
            queue->tasks.DropTarget(canonical.Get());
        }
        queue->tasks.DropTarget(target);
    }
    if (queue->tasks.empty() && queue->timerActive) {
        KillTimer(queue->hwnd, 1);
        queue->timerActive = false;
    }
}

static LRESULT CALLBACK InputHook(int code, WPARAM wp, LPARAM lp) {
    WindowLifetime::Callback callback(lifetime);
    if (code >= 0 && wp == PM_REMOVE && queue && !queue->tasks.empty()) {
        const auto msg = reinterpret_cast<const MSG*>(lp);
        if (msg->message == WM_KEYDOWN || msg->message == WM_SYSKEYDOWN ||
            msg->message == WM_LBUTTONDOWN || msg->message == WM_RBUTTONDOWN ||
            msg->message == WM_MBUTTONDOWN || msg->message == WM_NCLBUTTONDOWN) {
            ready = true;
            Flush(L"input");
        }
    }
    return CallNextHookEx(nullptr, code, wp, lp);
}

static LRESULT CALLBACK OwnerSubclass(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp, DWORD_PTR data) {
    WindowLifetime::Callback callback(lifetime);
    if (msg == WM_DESTROY) {
        auto q = reinterpret_cast<Queue*>(data);
        q->closing = true;
        Flush(L"teardown");
        connectedTargets.clear();
        XamlWork::ClearThreadConnections();
        FileTransition::ClearThreadTabs();
    }
    return DefSubclassProc(hwnd, msg, wp, lp);
}

static void DrainQueuedMetadataMessages(HWND hwnd) noexcept {
    MSG queuedMsg{};
    while (PeekMessageW(&queuedMsg, hwnd, kMsgMetadataResult, kMsgMetadataResult, PM_REMOVE)) {
        auto* rawRes = reinterpret_cast<MetadataWork::Result*>(queuedMsg.lParam);
        delete rawRes;
    }
}

static LRESULT CALLBACK Proc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    WindowLifetime::Callback callback(lifetime);
    if (msg == WM_APP) {
        if (ready && !FileTransition::navigationDepth && !FileTransition::processingBatch) {
            Flush(L"enumDone");
        }
        return 0;
    }
    if (msg == kMsgMetadataResult) {
        std::unique_ptr<MetadataWork::Result> owned(reinterpret_cast<MetadataWork::Result*>(lp));
        if (!MetadataWork::IsUnloading() && owned) {
            FileTransition::ApplyMetadataResult(owned->browserKey, owned->generation,
                                                owned->isSlowStorage);
        }
        return 0;
    }
    if (msg == WM_TIMER && wp == 1) {
        const bool viewSettled =
            ready && !FileTransition::navigationDepth && !FileTransition::processingBatch;
        const bool ceilingReached =
            GetTickCount64() - started >= static_cast<ULONGLONG>(settings.safetyCeilingMs);
        if (viewSettled || !FileTransition::HasActiveWork() || ceilingReached) {
            const wchar_t* reason =
                viewSettled ? L"enumDone" : (ceilingReached ? L"ceiling" : L"quiescence");
            ready = true;
            Flush(reason);
        }
        return 0;
    }
    if (msg == WM_CLOSE) {
        auto q = reinterpret_cast<Queue*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
        if (!q) {
            DrainQueuedMetadataMessages(hwnd);
            DestroyWindow(hwnd);
            return 0;
        }
        if (!q->draining) {
            Flush(L"teardown");
        }
        q->closing = true;
        Flush(L"teardown");
        return 0;
    }
    if (msg == WM_NCDESTROY) {
        DrainQueuedMetadataMessages(hwnd);
        connectedTargets.clear();
        XamlWork::ClearThreadConnections();
        FileTransition::ClearThreadTabs();
        auto q = reinterpret_cast<Queue*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
        SetWindowLongPtrW(hwnd, GWLP_USERDATA, 0);
        if (q) {
            if (q->inputHook) {
                UnhookWindowsHookEx(q->inputHook);
            }
            if (q->root) {
                WindhawkUtils::RemoveWindowSubclassFromAnyThread(q->root, OwnerSubclass);
            }
            if (queue == q) {
                queue = nullptr;
            }
            delete q;
        }
        lifetime.Remove(hwnd);
    }
    return DefWindowProcW(hwnd, msg, wp, lp);
}

static HWND EnsureQueueWindow() {
    WindowLifetime::Operation operation(lifetime);
    if (!operation || !registered) {
        return nullptr;
    }
    if (!queue) {
        HWND root = FindThreadBrowserWindow();
        if (!root) {
            return nullptr;
        }
        auto q = std::make_unique<Queue>();
        q->hwnd = CreateWindowExW(0, ClassName, L"", 0, 0, 0, 0, 0, HWND_MESSAGE, nullptr,
                                  instance, nullptr);
        if (!q->hwnd) {
            return nullptr;
        }
        SetWindowLongPtrW(q->hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(q.get()));
        q->root = root;
        q->inputHook = SetWindowsHookExW(WH_GETMESSAGE, InputHook, nullptr, GetCurrentThreadId());
        if (!q->inputHook || !WindhawkUtils::SetWindowSubclassFromAnyThread(
                                 root, OwnerSubclass, reinterpret_cast<DWORD_PTR>(q.get()))) {
            DestroyWindow(q->hwnd);
            return nullptr;
        }
        lifetime.Add(q->hwnd);
        queue = q.release();
    }
    return queue->closing ? nullptr : queue->hwnd;
}

static bool Enqueue(IUnknown* object, TaskKind kind, ULONG param, Task::Invoker run,
                    LCID locale = 0, WORD flags = 0) {
    WindowLifetime::Operation operation(lifetime);
    if (!operation || (queue && queue->draining) || !run) {
        return false;
    }
    if (!EnsureQueueWindow()) {
        if (queue && queue->closing) {
            Flush(L"teardown");
        }
        return false;
    }
    Microsoft::WRL::ComPtr<IUnknown> canonical = CanonicalUnknown(object);
    for (size_t i = 0; i < queue->tasks.size(); ++i) {
        Task& existing = queue->tasks[i];
        if ((existing.object.Get() == canonical.Get() || existing.target == object) &&
            existing.kind == kind) {
            if (kind == TaskKind::ClassicNavState || kind == TaskKind::XamlNavState) {
                existing.param = param;
                existing.run = run;
                return true;
            }
            if (existing.param == param) {
                return true;
            }
        }
    }
    if (queue->tasks.size() >= TaskQueue::kCapacity) {
        Flush(L"capacity");
        return false;
    }
    if (!queue->timerActive) {
        if (!SetTimer(queue->hwnd, 1, FileTransition::kAdaptiveCheckMs, nullptr)) {
            Flush(L"teardown");
            return false;
        }
        queue->timerActive = true;
    }
    Task task{};
    task.object = std::move(canonical);
    task.target = object;
    task.run = run;
    task.kind = kind;
    task.param = param;
    task.locale = locale;
    task.flags = flags;
    queue->tasks.Push(std::move(task));
    return true;
}

static void NavigationStarted() {
    Flush(L"preempt");
    started = GetTickCount64();
    ready = false;
}

static void ViewReady() {
    ready = true;
    if (!FileTransition::navigationDepth && !FileTransition::processingBatch) {
        Flush(L"enumDone");
    } else if (queue && queue->hwnd) {
        PostMessageW(queue->hwnd, WM_APP, 0, 0);
    }
}

static bool Pending() {
    return started && !ready &&
           (GetTickCount64() - started < static_cast<ULONGLONG>(settings.safetyCeilingMs));
}

static void RunClassicInvoke(const Task& task) {
    DISPPARAMS empty{};
    FileTransition::invokeOriginal(task.target, static_cast<DISPID>(task.param),
                                   kNullIid, task.locale, task.flags,
                                   &empty, nullptr, nullptr, nullptr);
}

static void RunClassicNavigated(const Task& task) {
    navigatedOriginal(task.target);
}

static void RunClassicNavState(const Task& task) {
    navStateOriginal(task.target, task.param);
}

// DISPID_FILELISTENUMDONE is deferred alongside other UI updates so that
// the Ribbon is notified only after the view settles, while
// EnumerationDone() uses the event internally as the trigger to flush.
static bool DeferInvoke(void* self, DISPID id, REFIID iid, LCID locale, WORD flags,
                        const DISPPARAMS* args) {
    if (!destroyOriginal || !settings.classicRibbon ||
        (!FileTransition::navigationDepth && !Pending())) {
        return false;
    }
    if (id != DISPID_SELECTIONCHANGED && id != DISPID_FILELISTENUMDONE &&
        id != DISPID_VIEWMODECHANGED && id != DISPID_CONTENTSCHANGED &&
        id != DISPID_COLUMNSCHANGED && id != DISPID_ICONSIZECHANGED &&
        id != DISPID_SELECTEDITEMCHANGED) {
        return false;
    }
    if (!CanDeferInvoke(iid, flags, args)) {
        return false;
    }
    return Enqueue(reinterpret_cast<IUnknown*>(self), TaskKind::ClassicInvoke,
                   static_cast<ULONG>(id), RunClassicInvoke, locale, flags);
}

static HRESULT __fastcall NavigatedHook(void* self) {
    if (destroyOriginal && settings.classicRibbon && Pending()) {
        auto canonical = CanonicalUnknown(reinterpret_cast<IUnknown*>(self));
        if (connectedTargets.contains(canonical.Get()) &&
            Enqueue(reinterpret_cast<IUnknown*>(self), TaskKind::ClassicNavigated, 0,
                    RunClassicNavigated)) {
            return S_OK;
        }
    }
    return navigatedOriginal(self);
}

static HRESULT __fastcall ChangedHook(void* self) {
    if (!Pending()) {
        Flush(L"quiescence");
    }
    HRESULT hr = changedOriginal(self);
    auto canonical = CanonicalUnknown(reinterpret_cast<IUnknown*>(self));
    if (SUCCEEDED(hr)) {
        connectedTargets.insert(canonical.Get());
    } else {
        connectedTargets.erase(canonical.Get());
    }
    return hr;
}

static HRESULT __fastcall DestroyHook(void* self, BOOL final) {
    if (queue) {
        if (!queue->draining) {
            Flush(L"teardown");
        } else {
            DropTasksForTarget(self);
        }
    }
    if (!connectedTargets.empty()) {
        auto canonical = CanonicalUnknown(reinterpret_cast<IUnknown*>(self));
        connectedTargets.erase(canonical.Get());
        connectedTargets.erase(self);
    }
    return destroyOriginal(self, final);
}

static HRESULT __fastcall NavStateHook(void* self, ULONG flags) {
    if (settings.navigationBar && Pending() &&
        Enqueue(reinterpret_cast<IUnknown*>(self), TaskKind::ClassicNavState, flags,
                RunClassicNavState)) {
        return S_OK;
    }
    return navStateOriginal(self, flags);
}

static bool Initialize() {
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                           GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                       reinterpret_cast<LPCWSTR>(Proc), &instance);
    WNDCLASSW cls{};
    cls.lpfnWndProc = Proc;
    cls.hInstance = instance;
    cls.lpszClassName = ClassName;
    registered = RegisterClassW(&cls) != 0;
    return registered;
}

static void BeforeUninit() {
    lifetime.Stop();
    lifetime.CloseAndWait();
}

static void Uninit() {
    if (registered) {
        if (!UnregisterClassW(ClassName, instance)) {
            Wh_Log(L"[CORE] Failed to unregister window class: %lu", GetLastError());
        }
        registered = false;
    }
}

} // namespace RibbonWork

// The modern command surface has its own event sink and navigation-band implementation.
namespace XamlWork {
using Notification = HRESULT(__fastcall*)(void*);
using StateChange = HRESULT(__fastcall*)(void*, ULONG);
using Invoke = FileTransition::Invoke;

static constexpr ULONG NAVSTATE_LOCATION = 0x4;

static std::atomic<bool> enabled{false};
static std::atomic<bool> hooksInstalled{false};
static thread_local BoundedPtrSet<32> connectedTargets;
static Notification navigatedOriginal;
static Notification changedOriginal;
static Notification invalidatedOriginal;
static Notification cleanupOriginal;
static StateChange stateOriginal;
static StateChange invalidateOriginal;
static Invoke invokeOriginal;

static void ClearThreadConnections() noexcept {
    connectedTargets.clear();
}

static bool Pending() {
    return enabled.load(std::memory_order_relaxed) && RibbonWork::Pending();
}

static HRESULT Run(Notification original, void* self, const wchar_t* label) {
    HRESULT hr = original(self);
    if (FAILED(hr)) {
        Wh_Log(L"[XAML] Deferred refresh failed (%s, hr=0x%08X)", label, hr);
    }
    return hr;
}

static HRESULT __fastcall ChangedHook(void* self) {
    HRESULT hr = changedOriginal(self);
    auto canonical = CanonicalUnknown(reinterpret_cast<IUnknown*>(self));
    if (SUCCEEDED(hr)) {
        connectedTargets.insert(canonical.Get());
    } else {
        connectedTargets.erase(canonical.Get());
    }
    return hr;
}

static void RunXamlNavigated(const RibbonWork::Task& task) {
    Run(navigatedOriginal, task.target, L"xaml_navigated");
}

static void RunXamlInvalidated(const RibbonWork::Task& task) {
    Run(invalidatedOriginal, task.target, L"xaml_invalidated");
}

static void RunXamlNavState(const RibbonWork::Task& task) {
    HRESULT hr = stateOriginal(task.target, task.param);
    if (FAILED(hr)) {
        Wh_Log(L"[XAML] Deferred location failed (hr=0x%08X)", hr);
    }
}

static void RunXamlInvalidateCommands(const RibbonWork::Task& task) {
    invalidateOriginal(task.target, task.param);
}

static void RunXamlInvoke(const RibbonWork::Task& task) {
    DISPPARAMS empty{};
    invokeOriginal(task.target, static_cast<DISPID>(task.param),
                   kNullIid, task.locale, task.flags,
                   &empty, nullptr, nullptr, nullptr);
}

static HRESULT __fastcall NavigatedHook(void* self) {
    if (settings.xamlCommandBar && Pending()) {
        auto canonical = CanonicalUnknown(reinterpret_cast<IUnknown*>(self));
        if (connectedTargets.contains(canonical.Get()) &&
            RibbonWork::Enqueue(reinterpret_cast<IUnknown*>(self),
                                RibbonWork::TaskKind::XamlNavigated, 0,
                                RunXamlNavigated)) {
            return S_OK;
        }
    }
    return navigatedOriginal(self);
}

static HRESULT __fastcall InvalidatedHook(void* self) {
    if (settings.xamlCommandBar && Pending() &&
        RibbonWork::Enqueue(reinterpret_cast<IUnknown*>(self),
                            RibbonWork::TaskKind::XamlInvalidated, 0,
                            RunXamlInvalidated)) {
        return S_OK;
    }
    return invalidatedOriginal(self);
}

static HRESULT __fastcall StateHook(void* self, ULONG flags) {
    if (settings.navigationBar && (flags & NAVSTATE_LOCATION) && Pending() &&
        RibbonWork::Enqueue(reinterpret_cast<IUnknown*>(self),
                            RibbonWork::TaskKind::XamlNavState, flags,
                            RunXamlNavState)) {
        return S_OK;
    }
    return stateOriginal(self, flags);
}

static HRESULT __fastcall InvalidateHook(void* self, ULONG commands) {
    if (settings.xamlCommandBar && Pending() &&
        RibbonWork::Enqueue(reinterpret_cast<IUnknown*>(self),
                            RibbonWork::TaskKind::XamlInvalidateCommands,
                            commands, RunXamlInvalidateCommands)) {
        return S_OK;
    }
    return invalidateOriginal(self, commands);
}

static HRESULT __fastcall InvokeHook(void* self, DISPID id, REFIID iid, LCID locale, WORD flags,
                                     DISPPARAMS* args, VARIANT* result, EXCEPINFO* exception,
                                     UINT* argError) {
    // DISPID_FILELISTENUMDONE is deferred alongside other UI updates so that
    // the XAML command bar is notified only after the view settles, while
    // EnumerationDone() uses the event internally as the trigger to flush.
    if (settings.xamlCommandBar && (FileTransition::navigationDepth || Pending())) {
        bool invalidation = id == DISPID_SELECTIONCHANGED || id == DISPID_FILELISTENUMDONE ||
                            id == DISPID_VIEWMODECHANGED || id == DISPID_CONTENTSCHANGED ||
                            id == DISPID_VIEWPAINTDONE || id == DISPID_COLUMNSCHANGED ||
                            id == DISPID_ICONSIZECHANGED || id == DISPID_SELECTEDITEMCHANGED;
        if (invalidation && CanDeferInvoke(iid, flags, args) &&
            RibbonWork::Enqueue(reinterpret_cast<IUnknown*>(self),
                                RibbonWork::TaskKind::XamlInvoke,
                                static_cast<ULONG>(id), RunXamlInvoke, locale,
                                flags)) {
            if (result) {
                *result = {};
            }
            if (id == DISPID_FILELISTENUMDONE) {
                FileTransition::EnumerationDone();
            }
            return S_OK;
        }
    }

    HRESULT hr = invokeOriginal(self, id, iid, locale, flags, args, result, exception, argError);
    if (id == DISPID_FILELISTENUMDONE) {
        FileTransition::EnumerationDone();
    }
    return hr;
}

static HRESULT __fastcall CleanupHook(void* self) {
    if (RibbonWork::queue) {
        if (!RibbonWork::queue->draining) {
            RibbonWork::Flush(L"teardown");
        } else {
            RibbonWork::DropTasksForTarget(self);
        }
    }
    if (!connectedTargets.empty()) {
        auto canonical = CanonicalUnknown(reinterpret_cast<IUnknown*>(self));
        connectedTargets.erase(canonical.Get());
        connectedTargets.erase(self);
    }
    return cleanupOriginal(self);
}

static void HookWindowsUiFileExplorerIfLoaded(HMODULE mod) {
    if (!settings.xamlCommandBar && !settings.navigationBar) {
        return;
    }
    if (!mod) {
        mod = GetModuleHandleW(L"Windows.UI.FileExplorer.dll");
    }
    if (!mod) {
        return;
    }
    if (hooksInstalled.exchange(true)) {
        return;
    }

    // Windows.UI.FileExplorer.dll
    WindhawkUtils::SYMBOL_HOOK windowsUiFileExplorerHooks[] = {
        {
            {LR"(public: virtual long __cdecl CommandBarViewAdapter::OnBrowserNavigated(void))"},
            reinterpret_cast<void**>(&navigatedOriginal),
            reinterpret_cast<void*>(NavigatedHook),
            true,
        },
        {
            {LR"(public: virtual long __cdecl CommandBarViewAdapter::OnShellViewChanged(void))"},
            reinterpret_cast<void**>(&changedOriginal),
            reinterpret_cast<void*>(ChangedHook),
            true,
        },
        {
            {LR"(public: virtual long __cdecl CommandBarViewAdapter::OnCommandStateInvalidated(void))"},
            reinterpret_cast<void**>(&invalidatedOriginal),
            reinterpret_cast<void*>(InvalidatedHook),
            true,
        },
        {
            {LR"(public: virtual long __cdecl CommandBarViewAdapter::SetNavigationState(unsigned long))"},
            reinterpret_cast<void**>(&stateOriginal),
            reinterpret_cast<void*>(StateHook),
            true,
        },
        {
            {LR"(public: virtual long __cdecl CommandBarViewAdapter::InvalidateCommands(enum COMMAND_SET))"},
            reinterpret_cast<void**>(&invalidateOriginal),
            reinterpret_cast<void*>(InvalidateHook),
            true,
        },
        {
            {LR"(public: virtual long __cdecl CommandBarViewAdapter::Invoke(long,struct _GUID const &,unsigned long,unsigned short,struct tagDISPPARAMS *,struct tagVARIANT *,struct tagEXCEPINFO *,unsigned int *))"},
            reinterpret_cast<void**>(&invokeOriginal),
            reinterpret_cast<void*>(InvokeHook),
            true,
        },
        {
            {LR"(public: long __cdecl CommandBarViewAdapter::Cleanup(void))"},
            reinterpret_cast<void**>(&cleanupOriginal),
            reinterpret_cast<void*>(CleanupHook),
            true,
        },
    };

    bool hooked = WindhawkUtils::HookSymbols(mod, windowsUiFileExplorerHooks,
                                             ARRAYSIZE(windowsUiFileExplorerHooks));
    if (!hooked) {
        hooksInstalled.store(false, std::memory_order_release);
        return;
    }

    bool isEnabled = cleanupOriginal && ((navigatedOriginal && changedOriginal) ||
                                         invalidatedOriginal || stateOriginal ||
                                         invalidateOriginal || invokeOriginal);
    enabled.store(isEnabled, std::memory_order_release);
    Wh_Log(L"[XAML] Modern XAML deferral (enabled=%d)", isEnabled ? 1 : 0);
}

static void Uninit() {
    enabled.store(false, std::memory_order_release);
    hooksInstalled.store(false, std::memory_order_release);
}

} // namespace XamlWork

// Grouped folders use a separate category timer before ItemsView batching.
namespace GroupingWork {
using GroupItem = void(__fastcall*)(void*, const void*, int);

static HMODULE storage = nullptr;
static GroupItem groupItemOriginal = nullptr;
static decltype(&SetTimer) setTimerOriginal = nullptr;
static thread_local unsigned depth = 0;

static bool IsActiveOnCurrentThread() noexcept {
    return depth > 0;
}

static void __fastcall GroupItemHook(void* self, const void* key, int flags) {
    FileTransition::OnGroupItem();
    PaintScope scope(depth);
    groupItemOriginal(self, key, flags);
}

static UINT_PTR WINAPI SetTimerHook(HWND hwnd, UINT_PTR id, UINT elapsed, TIMERPROC callback) {
    if (!IsActiveOnCurrentThread()) {
        return setTimerOriginal(hwnd, id, elapsed, callback);
    }
    if (settings.groupedFolderBatching && FileTransition::IsActiveViewGrouped() &&
        hwnd != nullptr && id == 7 && elapsed == 50 && callback == nullptr) {
        elapsed = 10;
    }
    return setTimerOriginal(hwnd, id, elapsed, callback);
}

static void Initialize() {
    if (!settings.groupedFolderBatching) {
        return;
    }
    storage = LoadLibraryExW(L"windows.storage.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!storage) {
        Wh_Log(L"[GROUP] Failed to load windows.storage.dll");
        return;
    }
    // windows.storage.dll
    WindhawkUtils::SYMBOL_HOOK windowsStorageHooks[] = {
        {
            {LR"(private: void __cdecl CDefCollection::_GroupItem(struct tagITEMKEY const *,int))"},
            reinterpret_cast<void**>(&groupItemOriginal),
            reinterpret_cast<void*>(GroupItemHook),
            true,
        },
    };
    WindhawkUtils::HookSymbols(storage, windowsStorageHooks, ARRAYSIZE(windowsStorageHooks));
    bool timerHooked = false;
    if (groupItemOriginal != nullptr) {
        timerHooked = WindhawkUtils::SetFunctionHook(SetTimer, SetTimerHook, &setTimerOriginal);
    }
    Wh_Log(L"[GROUP] Grouped folder batching (groupItem=%d, setTimer=%d)",
           groupItemOriginal != nullptr ? 1 : 0, timerHooked ? 1 : 0);
}

static void Uninit() {
    if (storage) {
        FreeLibrary(storage);
        storage = nullptr;
    }
}
} // namespace GroupingWork

// Native paint and batching hooks.

static void __fastcall RedrawFrameHook(void* self) {
    PaintScope scope(redrawFrameDepth);
    redrawFrameOriginal(self);
}

static HRESULT __fastcall RenderSizerHook(void* self) {
    PaintScope scope(renderSizerDepth);
    return renderSizerOriginal(self);
}

static BOOL WINAPI SetWindowPosHook(HWND hwnd, HWND after, int x, int y, int cx, int cy,
                                    UINT flags) {
    if (!FileTransition::navigationDepth || !settings.navigationPaint || (flags & SWP_NOREDRAW)) {
        return setWindowPosOriginal(hwnd, after, x, y, cx, cy, flags);
    }
    if (GetWindowThreadProcessId(hwnd, nullptr) != GetCurrentThreadId()) {
        return setWindowPosOriginal(hwnd, after, x, y, cx, cy, flags);
    }
    if (!(GetWindowLongPtrW(hwnd, GWL_STYLE) & WS_CHILD)) {
        return setWindowPosOriginal(hwnd, after, x, y, cx, cy, flags);
    }
    if (IsShellDesktopOrTrayWindow(hwnd)) {
        return setWindowPosOriginal(hwnd, after, x, y, cx, cy, flags);
    }
    if (!FileTransition::DeferNavigationPaint()) {
        return setWindowPosOriginal(hwnd, after, x, y, cx, cy, flags);
    }

    HWND parent = GetParent(hwnd);
    RECT beforeRect{};
    bool hasBefore = false;
    if (parent && GetWindowRect(hwnd, &beforeRect)) {
        MapWindowPoints(HWND_DESKTOP, parent, reinterpret_cast<LPPOINT>(&beforeRect), 2);
        hasBefore = true;
    }

    BOOL result = setWindowPosOriginal(hwnd, after, x, y, cx, cy, flags | SWP_NOREDRAW);

    if (result && hasBefore && parent) {
        RECT afterRect{};
        if (GetWindowRect(hwnd, &afterRect)) {
            MapWindowPoints(HWND_DESKTOP, parent, reinterpret_cast<LPPOINT>(&afterRect), 2);
            RECT dirty{};
            UnionRect(&dirty, &beforeRect, &afterRect);
            redrawWindowOriginal(parent, &dirty, nullptr, RDW_INVALIDATE | RDW_ERASE | RDW_ALLCHILDREN);
        } else {
            redrawWindowOriginal(parent, &beforeRect, nullptr, RDW_INVALIDATE | RDW_ERASE | RDW_ALLCHILDREN);
        }
    }

    return result;
}

static BOOL WINAPI UpdateWindowHook(HWND hwnd) {
    if (!redrawFrameDepth || !settings.navigationPaint) {
        return updateWindowOriginal(hwnd);
    }
    return FileTransition::DeferNavigationPaint() ? TRUE : updateWindowOriginal(hwnd);
}

static BOOL WINAPI RedrawWindowHook(HWND hwnd, const RECT* rect, HRGN region, UINT flags) {
    if (!renderSizerDepth || !settings.navigationPaint) {
        return redrawWindowOriginal(hwnd, rect, region, flags);
    }
    if (FileTransition::DeferNavigationPaint()) {
        flags &= ~static_cast<UINT>(RDW_UPDATENOW | RDW_ERASENOW);
    }
    return redrawWindowOriginal(hwnd, rect, region, flags);
}

static HMODULE WINAPI LoadLibraryExWHook(LPCWSTR lpLibFileName, HANDLE hFile, DWORD dwFlags) {
    HMODULE hModule = loadLibraryExWOriginal(lpLibFileName, hFile, dwFlags);
    if (hModule && !XamlWork::hooksInstalled.load(std::memory_order_relaxed)) {
        if (GetModuleHandleW(L"Windows.UI.FileExplorer.dll") == hModule) {
            XamlWork::HookWindowsUiFileExplorerIfLoaded(hModule);
            Wh_ApplyHookOperations();
        }
    }
    return hModule;
}

static uintptr_t CreateActionHook(const Action* action) {
    bool batch = action && action->size == sizeof(Action) && batchCallback &&
                 action->callback == batchCallback;
    auto state = FileTransition::active;
    bool observe = trackBatchActions && batch && state && state->itemsView == action->context;
    ULONG generation = observe ? state->generation : 0;
    uintptr_t handleVal = createActionOriginal(action);
    void* handle = reinterpret_cast<void*>(handleVal);
    if (observe && handle && FileTransition::active == state &&
        state->generation == generation) {
        state->batchAction = handle;
        if (!state->pending) {
            state->pending = true;
            ++state->batchCount;
        }
    }
    return handleVal;
}

static BOOL WINAPI DeleteActionHook(void* handle) {
    auto state = FileTransition::active;
    bool observe = state && handle && state->batchAction == handle;
    ULONG generation = observe ? state->generation : 0;
    BOOL result = deleteActionOriginal(handle);
    if (observe && result && FileTransition::active == state &&
        state->generation == generation && state->batchAction == handle) {
        state->batchAction = nullptr;
        state->pending = false;
        state->enumerated = true;
        FileTransition::NotifyReady();
    }
    return result;
}

// Module initialization and cleanup.

static void ReleaseModules() {
    MetadataWork::BeginUnload();
    MetadataWork::Uninit();
    GroupingWork::Uninit();
    XamlWork::Uninit();
    if (duser) {
        FreeLibrary(duser);
        duser = nullptr;
    }
    if (shell) {
        FreeLibrary(shell);
        shell = nullptr;
    }
    if (frame) {
        FreeLibrary(frame);
        frame = nullptr;
    }
}

static BOOL InitializeMod() {
    LoadSettings();

    frame = LoadLibraryExW(L"ExplorerFrame.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    shell = LoadLibraryExW(L"shell32.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);

    if (frame) {
        // ExplorerFrame.dll
        WindhawkUtils::SYMBOL_HOOK explorerFrameHooks[] = {
            {
                {LR"(private: static void __cdecl UIItemsView::s_BatchTimerCallback(struct GMA_ACTIONINFO *))"},
                reinterpret_cast<void**>(&batchCallback),
                nullptr,
                true,
            },
            {
                {LR"(private: long __cdecl CShellBrowser::_NavigateToPidl(struct _ITEMIDLIST_ABSOLUTE const *,unsigned long,unsigned long))"},
                reinterpret_cast<void**>(&FileTransition::navigateOriginal),
                reinterpret_cast<void*>(FileTransition::NavigateHook),
                true,
            },
            {
                {LR"(private: long __cdecl CShellBrowser::_OnConnectionCreated(struct IShellItem *,struct IUnknown *,unsigned long,unsigned long,struct IUnknown *,struct IUnknown *,struct _GUID const &))"},
                reinterpret_cast<void**>(&FileTransition::connectionOriginal),
                reinterpret_cast<void*>(FileTransition::ConnectionHook),
                true,
            },
            {
                {LR"(private: long __cdecl UIItemsView::_ResetRoot(struct IItem *,struct IItemCollection *))"},
                reinterpret_cast<void**>(&FileTransition::resetOriginal),
                reinterpret_cast<void*>(FileTransition::ResetHook),
                true,
            },
            {
                {LR"(private: long __cdecl UIItemsView::_EnsureBatching(void))"},
                reinterpret_cast<void**>(&FileTransition::ensureOriginal),
                reinterpret_cast<void*>(FileTransition::EnsureHook),
                true,
            },
            {
                {LR"(private: void __cdecl UIItemsView::_OnBatchTimer(struct GMA_ACTIONINFO *))"},
                reinterpret_cast<void**>(&FileTransition::batchOriginal),
                reinterpret_cast<void*>(FileTransition::BatchHook),
                true,
            },
            {
                {LR"(private: bool __cdecl UIItemsView::DeleteBatchTimer(void))"},
                reinterpret_cast<void**>(&FileTransition::deleteOriginal),
                reinterpret_cast<void*>(FileTransition::DeleteHook),
                true,
            },
            {
                {LR"(public: virtual long __cdecl CExplorerRibbon::Invoke(long,struct _GUID const &,unsigned long,unsigned short,struct tagDISPPARAMS *,struct tagVARIANT *,struct tagEXCEPINFO *,unsigned int *))"},
                reinterpret_cast<void**>(&FileTransition::invokeOriginal),
                reinterpret_cast<void*>(FileTransition::InvokeHook),
                true,
            },
            {
                {LR"(public: virtual long __cdecl CExplorerRibbon::OnBrowserNavigated(void))"},
                reinterpret_cast<void**>(&RibbonWork::navigatedOriginal),
                reinterpret_cast<void*>(RibbonWork::NavigatedHook),
                true,
            },
            {
                {LR"(public: virtual long __cdecl CExplorerRibbon::OnShellViewChanged(void))"},
                reinterpret_cast<void**>(&RibbonWork::changedOriginal),
                reinterpret_cast<void*>(RibbonWork::ChangedHook),
                true,
            },
            {
                {LR"(public: virtual long __cdecl CExplorerRibbon::DestroyRibbonUI(int))"},
                reinterpret_cast<void**>(&RibbonWork::destroyOriginal),
                reinterpret_cast<void*>(RibbonWork::DestroyHook),
                true,
            },
            {
                {LR"(public: virtual long __cdecl CNavBar::SetNavigationState(unsigned long))"},
                reinterpret_cast<void**>(&RibbonWork::navStateOriginal),
                reinterpret_cast<void*>(RibbonWork::NavStateHook),
                true,
            },
        };
        WindhawkUtils::HookSymbols(frame, explorerFrameHooks,
                                   ARRAYSIZE(explorerFrameHooks));
    }

    if (shell) {
        // shell32.dll
        WindhawkUtils::SYMBOL_HOOK shell32Hooks[] = {
            {
                {LR"(private: void __cdecl CDUIViewFrame::_RedrawFrame(void))"},
                reinterpret_cast<void**>(&redrawFrameOriginal),
                reinterpret_cast<void*>(RedrawFrameHook),
                true,
            },
            {
                {LR"(private: long __cdecl CDUISizerElement::_Render(void))"},
                reinterpret_cast<void**>(&renderSizerOriginal),
                reinterpret_cast<void*>(RenderSizerHook),
                true,
            },
        };
        WindhawkUtils::HookSymbols(shell, shell32Hooks, ARRAYSIZE(shell32Hooks));
    }

    if (settings.xamlCommandBar || settings.navigationBar) {
        HMODULE kernelbase = GetModuleHandleW(L"kernelbase.dll");
        if (kernelbase) {
            auto pfnLoadLibraryExW = reinterpret_cast<decltype(&LoadLibraryExW)>(
                GetProcAddress(kernelbase, "LoadLibraryExW"));
            if (pfnLoadLibraryExW) {
                WindhawkUtils::SetFunctionHook(pfnLoadLibraryExW, LoadLibraryExWHook,
                                               &loadLibraryExWOriginal);
            }
        }
        XamlWork::HookWindowsUiFileExplorerIfLoaded(nullptr);
    }

    if (!FileTransition::navigateOriginal && !FileTransition::resetOriginal &&
        !RibbonWork::navigatedOriginal && !RibbonWork::navStateOriginal &&
        !XamlWork::enabled.load(std::memory_order_relaxed) &&
        !((settings.xamlCommandBar || settings.navigationBar) && loadLibraryExWOriginal)) {
        Wh_Log(L"[CORE] No usable hooks on this build");
        return FALSE;
    }

    bool paint = false;
    bool layoutPaint = false;
    bool positionPaint = false;
    if (settings.navigationPaint) {
        paint = WindhawkUtils::SetFunctionHook(UpdateWindow, UpdateWindowHook,
                                               &updateWindowOriginal);
        layoutPaint = WindhawkUtils::SetFunctionHook(RedrawWindow, RedrawWindowHook,
                                                     &redrawWindowOriginal);
        positionPaint = WindhawkUtils::SetFunctionHook(SetWindowPos, SetWindowPosHook,
                                                       &setWindowPosOriginal);
    }

    if (!RibbonWork::Initialize()) {
        Wh_Log(L"[CORE] Failed to initialize RibbonWork window class");
        return FALSE;
    }

    if (!MetadataWork::Initialize()) {
        Wh_Log(L"[POOL] Metadata threadpool unavailable; continuing without async probe");
    }

    GroupingWork::Initialize();

    bool nativeBatchTracking =
        FileTransition::ensureOriginal && FileTransition::deleteOriginal;
    bool batches = nativeBatchTracking;
    if (!nativeBatchTracking && batchCallback) {
        duser = LoadLibraryExW(L"DUser.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
        if (duser) {
            createActionOriginal =
                reinterpret_cast<CreateAction>(GetProcAddress(duser, "CreateAction"));
            deleteActionOriginal =
                reinterpret_cast<DeleteAction>(GetProcAddress(duser, "DeleteHandle"));
            if (createActionOriginal && deleteActionOriginal &&
                WindhawkUtils::SetFunctionHook(createActionOriginal, CreateActionHook,
                                               &createActionOriginal) &&
                WindhawkUtils::SetFunctionHook(deleteActionOriginal, DeleteActionHook,
                                               &deleteActionOriginal)) {
                trackBatchActions = true;
                batches = true;
            }
        }
    }

    FileTransition::completionTracking =
        FileTransition::batchOriginal && (nativeBatchTracking || trackBatchActions);

    Wh_Log(L"[CORE] Initialization complete (batching=%d, activationPaint=%d, layoutPaint=%d, positionPaint=%d)",
           batches, paint, layoutPaint, positionPaint);

    return TRUE;
}

// Windhawk lifecycle entry points.

BOOL Wh_ModInit() {
    if (InitializeMod()) {
        return TRUE;
    }
    RibbonWork::Uninit();
    ReleaseModules();
    return FALSE;
}

void Wh_ModAfterInit() {
    if (settings.xamlCommandBar || settings.navigationBar) {
        XamlWork::HookWindowsUiFileExplorerIfLoaded(nullptr);
        Wh_ApplyHookOperations();
    }
}

BOOL Wh_ModSettingsChanged(BOOL* reload) {
    *reload = TRUE;
    return TRUE;
}

void Wh_ModBeforeUninit() {
    MetadataWork::BeginUnload();
    RibbonWork::BeforeUninit();
}

void Wh_ModUninit() {
    RibbonWork::Uninit();
    ReleaseModules();
}
