// ==WindhawkMod==
// @id              word-local-autosave
// @name            Word Local AutoSave
// @description     Enables AutoSave functionality for local documents in Microsoft Word via direct Word saves
// @version         3.5
// @author          communism420
// @github          https://github.com/communism420
// @include         WINWORD.EXE
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -loleacc -luuid
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Word Local AutoSave

This mod enables automatic saving for locally stored Word documents, similar to
how AutoSave works with OneDrive files.

## How it works

The mod monitors keyboard input, document dirty-state changes, and natural
editing boundaries in Microsoft Word. It also listens to native Word
application events where possible. When you type, paste, format text, switch
away, rename via Save As, or make other editing changes, it schedules a save
after a short delay.

This build does **not** send `Ctrl+S`. It talks to Word directly through
automation and calls document save APIs, which removes the root cause of false
shortcut activations.

## Features

- Detects typing, backspace, delete, enter, punctuation, numpad, and clipboard operations
- Detects Ctrl+V, Ctrl+X, Ctrl+Y, Ctrl+Z, Ctrl+B, Ctrl+I, Ctrl+U, Ctrl+Enter
- Detects context-menu paste and non-keyboard formatting changes after Word marks the document dirty
- Configurable delay before saving
- Optional minimum interval between saves to prevent excessive disk writes
- Direct Word save calls with zero synthetic keyboard input
- Flushes pending changes earlier when you finish an action or leave the current document/window
- Waits for modal Word UI such as dialogs, menus, and IME composition before saving
- Migrates internal tracking after Save As / rename without losing document state
- Binds more precisely to the current Word instance and current document window
- Uses native Word application events for document transitions and manual saves when available
- Only saves when the active Word document window is focused

## Shortcut Safety (v3.5)

- No `SendInput`
- No synthetic `Ctrl` state
- No partial `Ctrl+...` races
- Save execution stays on one owner UI thread
- Pending input and held modifiers postpone auto-save instead of racing it

## Limitations

- Only works with documents that have already been saved at least once
- New unsaved documents are skipped to avoid opening "Save As"
- Operations that don't make Word mark the document as modified are ignored
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- saveDelay: 1000
  $name: Save Delay (ms)
  $description: Delay in milliseconds before auto-saving after a change is detected (minimum 100ms)
- minTimeBetweenSaves: 0
  $name: Minimum Time Between Saves (ms)
  $description: Minimum time between consecutive saves. Set to 0 to disable this limit.
*/
// ==/WindhawkModSettings==

// Local compiler checks can define WH_STANDALONE_COMPILE_CHECK to supply a
// minimal Windhawk API shim outside the Windhawk build environment.
#if defined(WH_STANDALONE_COMPILE_CHECK) && \
    !defined(_ALLOW_COMPILER_AND_STL_VERSION_MISMATCH)
#define _ALLOW_COMPILER_AND_STL_VERSION_MISMATCH
#endif

#include <windows.h>
#include <ocidl.h>
#include <oleauto.h>
#include <oleacc.h>
#include <new>
#include <utility>

#ifdef WH_STANDALONE_COMPILE_CHECK
inline void Wh_Log(const wchar_t*, ...) {
}

inline int Wh_GetIntSetting(const wchar_t*) {
    return 0;
}

inline BOOL Wh_SetFunctionHook(void*, void*, void**) {
    return TRUE;
}
#endif

// ============================================================================
// Constants
// ============================================================================

const int MIN_SAVE_DELAY_MS = 100;
const int MAX_SAVE_DELAY_MS = 60000;
const int MAX_MIN_TIME_BETWEEN_SAVES = 300000;
const DWORD MIN_RETRY_INTERVAL_MS = 50;
const DWORD INPUT_SETTLE_DELAY_MS = 25;
const DWORD ACTION_BURST_SETTLE_DELAY_MS = 250;
const DWORD AUTOMATION_RECOVERY_DELAY_MS = 125;
const DWORD DOCUMENT_STATE_ACTIVE_POLL_INTERVAL_MS = 350;
const DWORD DOCUMENT_STATE_IDLE_POLL_INTERVAL_MS = 1500;
const DWORD DOCUMENT_STATE_EVENT_IDLE_POLL_INTERVAL_MS = 5000;
const DWORD MAX_SAVE_RETRY_INTERVAL_MS = 1000;
const DWORD MAX_DOCUMENT_STATE_RETRY_INTERVAL_MS = 2000;
const DWORD FAILURE_LOG_INTERVAL_MS = 2000;
const DWORD STATUS_LOG_INTERVAL_MS = 3000;
const DWORD SAVE_AS_MIGRATION_TIMEOUT_MS = 15000;
const DWORD WORD_EVENT_RECONNECT_INTERVAL_MS = 2000;
const int DISPATCH_MEMBER_ID_CACHE_CAPACITY = 48;
const DWORD OBJID_NATIVEOM_VALUE = 0xFFFFFFF0u;

const int VK_KEY_0 = 0x30;
const int VK_KEY_9 = 0x39;
const int VK_KEY_A = 0x41;
const int VK_KEY_Z = 0x5A;

const IID kIIDNull = {};
const IID kIIDIDispatch = {
    0x00020400,
    0x0000,
    0x0000,
    {0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46}
};
const IID kDIIDWordApplicationEvents4 = {
    0x00020A01,
    0x0000,
    0x0000,
    {0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46}
};

// ============================================================================
// Function Types
// ============================================================================

typedef BOOL (WINAPI* TranslateMessage_t)(const MSG*);

struct WordEventDispIds {
    DISPID documentBeforeSave = DISPID_UNKNOWN;
    DISPID documentBeforeClose = DISPID_UNKNOWN;
    DISPID documentChange = DISPID_UNKNOWN;
    DISPID windowDeactivate = DISPID_UNKNOWN;

    void Reset() {
        documentBeforeSave = DISPID_UNKNOWN;
        documentBeforeClose = DISPID_UNKNOWN;
        documentChange = DISPID_UNKNOWN;
        windowDeactivate = DISPID_UNKNOWN;
    }
};

struct DispatchMemberIdCacheEntry {
    GUID typeGuid = GUID_NULL;
    bool hasTypeGuid = false;
    const wchar_t* name = nullptr;
    DISPID dispId = DISPID_UNKNOWN;
    ULONGLONG lastUseTime = 0;
};

template <typename T>
class ScopedComPtr {
public:
    ScopedComPtr() = default;

    ScopedComPtr(const ScopedComPtr&) = delete;
    ScopedComPtr& operator=(const ScopedComPtr&) = delete;

    ScopedComPtr(ScopedComPtr&& other) noexcept : m_ptr(other.Detach()) {
    }

    ScopedComPtr& operator=(ScopedComPtr&& other) noexcept {
        if (this != &other) {
            Reset(other.Detach());
        }

        return *this;
    }

    ~ScopedComPtr() {
        Reset();
    }

    T* Get() const {
        return m_ptr;
    }

    T** Put() {
        Reset();
        return &m_ptr;
    }

    void Reset(T* ptr = nullptr) {
        if (m_ptr) {
            m_ptr->Release();
        }

        m_ptr = ptr;
    }

    T* Detach() {
        T* ptr = m_ptr;
        m_ptr = nullptr;
        return ptr;
    }

    explicit operator bool() const {
        return m_ptr != nullptr;
    }

    T* operator->() const {
        return m_ptr;
    }

private:
    T* m_ptr = nullptr;
};

class ScopedBstr {
public:
    ScopedBstr() = default;

    ScopedBstr(const ScopedBstr&) = delete;
    ScopedBstr& operator=(const ScopedBstr&) = delete;

    ScopedBstr(ScopedBstr&& other) noexcept : m_value(other.Detach()) {
    }

    ScopedBstr& operator=(ScopedBstr&& other) noexcept {
        if (this != &other) {
            Reset(other.Detach());
        }

        return *this;
    }

    ~ScopedBstr() {
        Reset();
    }

    BSTR Get() const {
        return m_value;
    }

    BSTR* Put() {
        Reset();
        return &m_value;
    }

    void Reset(BSTR value = nullptr) {
        if (m_value) {
            SysFreeString(m_value);
        }

        m_value = value;
    }

    BSTR Detach() {
        BSTR value = m_value;
        m_value = nullptr;
        return value;
    }

    UINT Length() const {
        return m_value ? SysStringLen(m_value) : 0;
    }

    const wchar_t* CStr() const {
        return m_value ? m_value : L"";
    }

private:
    BSTR m_value = nullptr;
};

// ============================================================================
// Global State
// ============================================================================

struct {
    int saveDelay;
    int minTimeBetweenSaves;
} g_settings;

TranslateMessage_t g_originalTranslateMessage = nullptr;

struct RuntimeTimingState {
    UINT_PTR schedulerTimerId = 0;
    ULONGLONG schedulerTimerDueTime = 0;
    ULONGLONG saveTimerDueTime = 0;
    ULONGLONG documentStateTimerDueTime = 0;
    ULONGLONG lastEditTime = 0;
    ULONGLONG lastSaveTime = 0;
    ULONGLONG lastEventConnectAttemptTime = 0;
    ULONGLONG pendingSaveAsTime = 0;
    DWORD saveRetryDelayMs = MIN_RETRY_INTERVAL_MS;
    DWORD documentStateRetryDelayMs = MIN_RETRY_INTERVAL_MS;
};

struct RuntimeStatusState {
    ScopedBstr lastSaveStatusMessage;
    ScopedBstr lastDocumentStateStatusMessage;
    ULONGLONG lastSaveFailureLogTime = 0;
    ULONGLONG lastDocumentStateFailureLogTime = 0;
    ULONGLONG lastSaveStatusLogTime = 0;
    ULONGLONG lastDocumentStateStatusLogTime = 0;
};

struct RuntimeDocumentState {
    ScopedBstr observedDocumentPath;
    ScopedBstr transitionFlushDocumentPath;
    ScopedComPtr<IDispatch> transitionFlushDocument;
    ScopedComPtr<IUnknown> observedDocumentIdentity;
    ScopedComPtr<IUnknown> pendingSaveAsDocumentIdentity;
    ScopedComPtr<IUnknown> transitionFlushDocumentIdentity;
};

struct WordEventSession {
    WordEventSession() = default;
    WordEventSession(const WordEventSession&) = delete;
    WordEventSession& operator=(const WordEventSession&) = delete;
    WordEventSession(WordEventSession&&) = default;
    WordEventSession& operator=(WordEventSession&&) = default;

    LONG_PTR applicationHwnd = 0;
    ScopedComPtr<IDispatch> application;
    ScopedComPtr<IConnectionPoint> connectionPoint;
    ScopedComPtr<IDispatch> sink;
    DWORD cookie = 0;

    void Reset() {
        applicationHwnd = 0;
        application.Reset();
        connectionPoint.Reset();
        sink.Reset();
        cookie = 0;
    }

    bool IsConnected() const {
        return connectionPoint && cookie != 0;
    }
};

struct RuntimeEventState {
    WordEventSession session;
};

struct RuntimeUiCacheState {
    HWND cachedWordRootWindow = nullptr;
    HWND cachedNativeWordWindow = nullptr;
    DWORD cachedWordUiThreadId = 0;
};

struct RuntimeFlagState {
    volatile LONG pendingSave = FALSE;
    volatile LONG documentDirtyKnown = FALSE;
    volatile LONG documentDirty = FALSE;
    volatile LONG manualSavePending = FALSE;
    volatile LONG expeditedSavePending = FALSE;
    volatile LONG transitionFlushPending = FALSE;
    volatile LONG imeComposing = FALSE;
    volatile LONG automationBusyPending = FALSE;
    volatile LONG pendingSaveAsMigration = FALSE;
    volatile LONG wordEventsConnected = FALSE;
    volatile LONG moduleActive = FALSE;
};

struct RuntimeState {
    DWORD wordProcessId = 0;
    volatile LONG ownerThreadId = 0;
    RuntimeTimingState timing;
    RuntimeStatusState status;
    RuntimeDocumentState document;
    RuntimeEventState events;
    RuntimeUiCacheState ui;
    DispatchMemberIdCacheEntry
        dispatchMemberIdCache[DISPATCH_MEMBER_ID_CACHE_CAPACITY] = {};
    RuntimeFlagState flags;
};

RuntimeState g_runtime = {};

// ============================================================================
// Utility Helpers
// ============================================================================

bool LoadFlag(const volatile LONG& value) {
    return InterlockedCompareExchange(const_cast<volatile LONG*>(&value), TRUE, TRUE) == TRUE;
}

void StoreFlag(volatile LONG& value, bool enabled) {
    InterlockedExchange(&value, enabled ? TRUE : FALSE);
}

void SetFlag(volatile LONG& value) {
    StoreFlag(value, true);
}

void ClearFlag(volatile LONG& value) {
    StoreFlag(value, false);
}

DWORD LoadOwnerThreadId() {
    return static_cast<DWORD>(InterlockedCompareExchange(&g_runtime.ownerThreadId, 0, 0));
}

DWORD ExchangeOwnerThreadId(DWORD threadId) {
    return static_cast<DWORD>(InterlockedExchange(&g_runtime.ownerThreadId,
                                                  static_cast<LONG>(threadId)));
}

void ClearOwnerThreadId() {
    ExchangeOwnerThreadId(0);
}

bool IsQueueKeyDown(int vk) {
    return (GetKeyState(vk) & 0x8000) != 0;
}

bool IsAsyncKeyDown(int vk) {
    return (GetAsyncKeyState(vk) & 0x8000) != 0;
}

bool IsOwnerThread() {
    const DWORD ownerThreadId = LoadOwnerThreadId();
    return ownerThreadId != 0 && GetCurrentThreadId() == ownerThreadId;
}

void ClearDispatchMemberIdCache() {
    for (int index = 0; index < DISPATCH_MEMBER_ID_CACHE_CAPACITY; ++index) {
        g_runtime.dispatchMemberIdCache[index].typeGuid = GUID_NULL;
        g_runtime.dispatchMemberIdCache[index].hasTypeGuid = false;
        g_runtime.dispatchMemberIdCache[index].name = nullptr;
        g_runtime.dispatchMemberIdCache[index].dispId = DISPID_UNKNOWN;
        g_runtime.dispatchMemberIdCache[index].lastUseTime = 0;
    }
}

template <typename T>
bool ReplaceStoredComPtr(ScopedComPtr<T>* storage, T* value) {
    if (!storage) {
        return false;
    }

    if (storage->Get() == value) {
        return true;
    }

    if (value) {
        value->AddRef();
    }

    storage->Reset(value);
    return true;
}

bool AreSameDocumentPath(const wchar_t* left, const wchar_t* right);
bool ReplaceStoredBstr(BSTR* storage, const wchar_t* value);
bool ReplaceStoredBstr(ScopedBstr* storage, const wchar_t* value);
void LogSaveStatus(const wchar_t* message);
void LogDocumentStateStatus(const wchar_t* message);
bool AreModifiersOrMouseButtonsHeld();
HRESULT GetComIdentity(IUnknown* unknown, IUnknown** result);
HRESULT GetDispatchTypeGuid(IDispatch* dispatch, GUID* result);
bool IsWindowInCurrentWordProcess(HWND hwnd);
void InvalidateTransitionFlushDocumentCache();
struct RuntimeFlagSnapshot;
DWORD GetSteadyDocumentStatePollDelay(const RuntimeFlagSnapshot* flags = nullptr);

enum class MessageAutosaveRole {
    None,
    EditingInput,
    DocumentRefreshFallback,
    ActionBoundaryFallback,
    TransitionBoundaryFallback,
};

enum class RuntimeStatePhase {
    Idle,
    WaitingForOwnerThread,
    WaitingForWordWindow,
    WaitingForWordUi,
    WaitingForInputToSettle,
    WaitingForSaveDelay,
    WaitingForMinSaveInterval,
    ReadyToRefreshDocumentState,
    ReadyToSave,
};

enum class TickDecisionAction {
    None,
    RearmDocumentStateTimer,
    RearmSaveTimer,
    RefreshDocumentState,
    SaveDocument,
};

enum class ScheduledTaskKind {
    Save,
    DocumentState,
};

bool HasClassName(HWND hwnd, const wchar_t* className) {
    if (!hwnd || !className) {
        return false;
    }

    wchar_t actualClass[64] = {};
    if (!GetClassNameW(hwnd, actualClass, ARRAYSIZE(actualClass))) {
        return false;
    }

    return lstrcmpW(actualClass, className) == 0;
}

bool IsOwnerCandidateMessage(UINT message) {
    switch (message) {
        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
        case WM_CHAR:
        case WM_LBUTTONUP:
        case WM_RBUTTONUP:
        case WM_MBUTTONUP:
        case WM_XBUTTONUP:
        case WM_COMMAND:
        case WM_PASTE:
        case WM_CUT:
        case WM_UNDO:
        case WM_CONTEXTMENU:
            return true;
    }

    return false;
}

bool IsDocumentStateRefreshMessage(UINT message) {
    switch (message) {
        case WM_COMMAND:
        case WM_PASTE:
        case WM_CUT:
        case WM_UNDO:
            return true;
    }

    return false;
}

bool IsActionBoundaryMessage(const MSG* lpMsg) {
    if (!lpMsg) {
        return false;
    }

    switch (lpMsg->message) {
        case WM_LBUTTONUP:
        case WM_RBUTTONUP:
        case WM_MBUTTONUP:
        case WM_XBUTTONUP:
        case WM_MOUSEWHEEL:
        case WM_MOUSEHWHEEL:
        case WM_CONTEXTMENU:
        case WM_COMMAND:
        case WM_ACTIVATE:
        case WM_ACTIVATEAPP:
        case WM_KILLFOCUS:
        case WM_CLOSE:
        case WM_QUERYENDSESSION:
        case WM_ENDSESSION:
        case WM_SYSCOMMAND:
            return true;
    }

    return false;
}

bool IsTransitionFlushMessage(const MSG* lpMsg) {
    if (!lpMsg) {
        return false;
    }

    switch (lpMsg->message) {
        case WM_KILLFOCUS:
        case WM_CLOSE:
        case WM_QUERYENDSESSION:
            return true;

        case WM_ENDSESSION:
            return lpMsg->wParam != FALSE;

        case WM_ACTIVATEAPP:
            return lpMsg->wParam == FALSE;

        case WM_ACTIVATE:
            return LOWORD(lpMsg->wParam) == WA_INACTIVE;

        case WM_SYSCOMMAND: {
            const WPARAM command = lpMsg->wParam & 0xFFF0;
            return command == SC_CLOSE || command == SC_MINIMIZE;
        }
    }

    return false;
}

bool IsImeCompositionMessage(UINT message) {
    switch (message) {
        case WM_IME_STARTCOMPOSITION:
        case WM_IME_ENDCOMPOSITION:
            return true;
    }

    return false;
}

void UpdateImeCompositionState(const MSG* lpMsg) {
    if (!lpMsg || !IsImeCompositionMessage(lpMsg->message)) {
        return;
    }

    if (lpMsg->message == WM_IME_STARTCOMPOSITION) {
        SetFlag(g_runtime.flags.imeComposing);
    } else if (lpMsg->message == WM_IME_ENDCOMPOSITION) {
        ClearFlag(g_runtime.flags.imeComposing);
    }
}

bool ShouldInvalidateWordUiCacheForMessage(const MSG* lpMsg) {
    if (!lpMsg) {
        return false;
    }

    switch (lpMsg->message) {
        case WM_ACTIVATE:
        case WM_ACTIVATEAPP:
        case WM_KILLFOCUS:
        case WM_SETFOCUS:
        case WM_NCACTIVATE:
        case WM_DESTROY:
        case WM_CLOSE:
            return true;
    }

    return false;
}

bool IsRetryableAutomationFailure(HRESULT hr) {
    return hr == RPC_E_CALL_REJECTED || hr == RPC_E_SERVERCALL_RETRYLATER;
}

bool ShouldLogFailureNow(ULONGLONG* lastLogTime) {
    if (!lastLogTime) {
        return true;
    }

    const ULONGLONG now = GetTickCount64();
    const ULONGLONG previous = *lastLogTime;
    if (previous != 0 && now - previous < FAILURE_LOG_INTERVAL_MS) {
        return false;
    }

    *lastLogTime = now;
    return true;
}

bool ShouldLogStatusNow(ULONGLONG* lastLogTime) {
    if (!lastLogTime) {
        return true;
    }

    const ULONGLONG now = GetTickCount64();
    const ULONGLONG previous = *lastLogTime;
    if (previous != 0 && now - previous < STATUS_LOG_INTERVAL_MS) {
        return false;
    }

    *lastLogTime = now;
    return true;
}

bool ShouldLogStatusMessageNow(BSTR* lastMessage,
                               ULONGLONG* lastLogTime,
                               const wchar_t* message) {
    if (!message || !lastMessage || !lastLogTime) {
        return false;
    }

    const bool changed = !*lastMessage || lstrcmpW(*lastMessage, message) != 0;
    if (!changed) {
        return ShouldLogStatusNow(lastLogTime);
    }

    if (!ReplaceStoredBstr(lastMessage, message)) {
        return ShouldLogStatusNow(lastLogTime);
    }

    *lastLogTime = GetTickCount64();
    return true;
}

bool ShouldLogStatusMessageNow(ScopedBstr* lastMessage,
                               ULONGLONG* lastLogTime,
                               const wchar_t* message) {
    if (!message || !lastMessage || !lastLogTime) {
        return false;
    }

    const bool changed =
        !lastMessage->Get() || lstrcmpW(lastMessage->Get(), message) != 0;
    if (!changed) {
        return ShouldLogStatusNow(lastLogTime);
    }

    if (!ReplaceStoredBstr(lastMessage, message)) {
        return ShouldLogStatusNow(lastLogTime);
    }

    *lastLogTime = GetTickCount64();
    return true;
}

DWORD AdvanceRetryDelay(DWORD* retryDelayMs, DWORD maxRetryDelayMs) {
    if (!retryDelayMs) {
        return MIN_RETRY_INTERVAL_MS;
    }

    DWORD delayMs = *retryDelayMs;
    if (delayMs < MIN_RETRY_INTERVAL_MS) {
        delayMs = MIN_RETRY_INTERVAL_MS;
    }

    DWORD nextDelayMs = delayMs;
    if (nextDelayMs < maxRetryDelayMs) {
        if (nextDelayMs > maxRetryDelayMs / 2) {
            nextDelayMs = maxRetryDelayMs;
        } else {
            nextDelayMs *= 2;
        }
    }

    *retryDelayMs = nextDelayMs;
    return delayMs;
}

bool AreSameDocumentPath(const wchar_t* left, const wchar_t* right);

bool ReplaceStoredBstr(BSTR* storage, const wchar_t* value) {
    if (!storage) {
        return false;
    }

    const bool currentEmpty = !*storage || !**storage;
    const bool valueEmpty = !value || !*value;
    if ((!currentEmpty || !valueEmpty) && AreSameDocumentPath(*storage, value)) {
        return true;
    }

    BSTR replacement = nullptr;
    if (!valueEmpty) {
        replacement = SysAllocString(value);
        if (!replacement) {
            return false;
        }
    }

    if (*storage) {
        SysFreeString(*storage);
    }

    *storage = replacement;
    return true;
}

bool ReplaceStoredBstr(ScopedBstr* storage, const wchar_t* value) {
    if (!storage) {
        return false;
    }

    const bool currentEmpty = !storage->Get() || !*storage->Get();
    const bool valueEmpty = !value || !*value;
    if ((!currentEmpty || !valueEmpty) && AreSameDocumentPath(storage->Get(), value)) {
        return true;
    }

    BSTR replacement = nullptr;
    if (!valueEmpty) {
        replacement = SysAllocString(value);
        if (!replacement) {
            return false;
        }
    }

    storage->Reset(replacement);
    return true;
}

void ClearStoredStatusMessage(BSTR* storage, ULONGLONG* lastLogTime) {
    if (storage && *storage) {
        SysFreeString(*storage);
        *storage = nullptr;
    }

    if (lastLogTime) {
        *lastLogTime = 0;
    }
}

void ClearStoredStatusMessage(ScopedBstr* storage, ULONGLONG* lastLogTime) {
    if (storage) {
        storage->Reset();
    }

    if (lastLogTime) {
        *lastLogTime = 0;
    }
}

bool AreSameDocumentPath(const wchar_t* left, const wchar_t* right) {
    const bool leftEmpty = !left || !*left;
    const bool rightEmpty = !right || !*right;
    if (leftEmpty || rightEmpty) {
        return leftEmpty == rightEmpty;
    }

    return lstrcmpiW(left, right) == 0;
}

bool SetObservedDocumentPath(const wchar_t* path) {
    return ReplaceStoredBstr(&g_runtime.document.observedDocumentPath, path);
}

bool SetTransitionFlushDocumentPath(const wchar_t* path) {
    return ReplaceStoredBstr(&g_runtime.document.transitionFlushDocumentPath, path);
}

bool SetTransitionFlushDocument(IDispatch* document) {
    if (!ReplaceStoredComPtr(&g_runtime.document.transitionFlushDocument, document)) {
        return false;
    }

    if (!document) {
        g_runtime.document.transitionFlushDocumentIdentity.Reset();
        return true;
    }

    IUnknown* identity = nullptr;
    if (FAILED(GetComIdentity(document, &identity)) || !identity) {
        g_runtime.document.transitionFlushDocument.Reset();
        g_runtime.document.transitionFlushDocumentIdentity.Reset();
        return false;
    }

    const bool replaced =
        ReplaceStoredComPtr(&g_runtime.document.transitionFlushDocumentIdentity, identity);
    identity->Release();
    return replaced;
}

void ClearManualSavePending() {
    ClearFlag(g_runtime.flags.manualSavePending);
}

void ClearExpeditedSavePending() {
    ClearFlag(g_runtime.flags.expeditedSavePending);
}

void ClearTransitionFlushRequest() {
    ClearFlag(g_runtime.flags.transitionFlushPending);
    g_runtime.document.transitionFlushDocumentPath.Reset();

    InvalidateTransitionFlushDocumentCache();
}

bool HasPendingSaveWork() {
    return LoadFlag(g_runtime.flags.pendingSave) ||
           LoadFlag(g_runtime.flags.manualSavePending) ||
           LoadFlag(g_runtime.flags.documentDirty);
}

bool HasPendingAutosave() {
    return LoadFlag(g_runtime.flags.pendingSave);
}

void ClearAutomationBusyPending() {
    ClearFlag(g_runtime.flags.automationBusyPending);
}

void NoteAutomationBusy(const wchar_t* message) {
    SetFlag(g_runtime.flags.automationBusyPending);
    if (message) {
        LogSaveStatus(message);
    }
}

void ClearPendingSaveAsMigration() {
    ClearFlag(g_runtime.flags.pendingSaveAsMigration);
    g_runtime.timing.pendingSaveAsTime = 0;
    g_runtime.document.pendingSaveAsDocumentIdentity.Reset();
}

bool HasPendingSaveAsMigration() {
    return LoadFlag(g_runtime.flags.pendingSaveAsMigration);
}

bool HasActiveDocumentStatePollWork() {
    return HasPendingSaveWork() ||
           LoadFlag(g_runtime.flags.automationBusyPending) ||
           HasPendingSaveAsMigration();
}

bool AreWordEventsConnected() {
    return LoadFlag(g_runtime.flags.wordEventsConnected);
}

DWORD ComputeBoundaryCoalesceDelay(bool transitionFlush,
                                   ULONGLONG lastEditTime,
                                   ULONGLONG now) {
    if (transitionFlush) {
        return INPUT_SETTLE_DELAY_MS;
    }

    if (lastEditTime != 0 && now >= lastEditTime &&
        now - lastEditTime < ACTION_BURST_SETTLE_DELAY_MS) {
        return ACTION_BURST_SETTLE_DELAY_MS;
    }

    return INPUT_SETTLE_DELAY_MS;
}

bool ShouldUseMessageDocumentRefreshFallback(bool /*wordEventsConnected*/) {
    return true;
}

bool ShouldUseMessageBoundaryFallback(bool wordEventsConnected, bool transitionFlush) {
    return transitionFlush || !wordEventsConnected;
}

void ResetObservedDocumentState();

void ResetObservedDocumentState() {
    g_runtime.document.observedDocumentPath.Reset();
    g_runtime.document.observedDocumentIdentity.Reset();

    ClearFlag(g_runtime.flags.documentDirtyKnown);
    ClearFlag(g_runtime.flags.documentDirty);
}

void RequestTransitionFlush(const wchar_t* path,
                           const wchar_t* reason,
                           IDispatch* document = nullptr) {
    SetFlag(g_runtime.flags.transitionFlushPending);
    if (!SetTransitionFlushDocumentPath(path) ||
        (document && !SetTransitionFlushDocument(document))) {
        ClearTransitionFlushRequest();
        SetFlag(g_runtime.flags.transitionFlushPending);
        if (!SetTransitionFlushDocumentPath(path)) {
            return;
        }
    } else if (!document) {
        SetTransitionFlushDocument(nullptr);
    }

    SetFlag(g_runtime.flags.expeditedSavePending);
    if (reason) {
        LogSaveStatus(reason);
    }
}

void InvalidateWordUiWindowCache() {
    g_runtime.ui.cachedWordRootWindow = nullptr;
    g_runtime.ui.cachedNativeWordWindow = nullptr;
    g_runtime.ui.cachedWordUiThreadId = 0;
}

bool IsUsableWordRootWindow(HWND hwnd) {
    return hwnd &&
           IsWindow(hwnd) &&
           IsWindowInCurrentWordProcess(hwnd) &&
           HasClassName(hwnd, L"OpusApp");
}

bool IsUsableCachedNativeWordWindow(HWND hwnd, HWND rootWindow) {
    if (!hwnd || !IsWindow(hwnd) || !IsWindowInCurrentWordProcess(hwnd)) {
        return false;
    }

    if (rootWindow) {
        HWND nativeRootWindow = GetAncestor(hwnd, GA_ROOT);
        if (!nativeRootWindow) {
            nativeRootWindow = hwnd;
        }

        if (nativeRootWindow != rootWindow) {
            return false;
        }
    }

    return true;
}

void CacheWordUiWindowContext(HWND rootWindow, HWND nativeWordWindow, DWORD threadId = 0) {
    if (!IsUsableWordRootWindow(rootWindow)) {
        InvalidateWordUiWindowCache();
        return;
    }

    g_runtime.ui.cachedWordRootWindow = rootWindow;
    g_runtime.ui.cachedNativeWordWindow =
        IsUsableCachedNativeWordWindow(nativeWordWindow, rootWindow) ? nativeWordWindow : nullptr;
    if (g_runtime.ui.cachedNativeWordWindow) {
        g_runtime.ui.cachedWordUiThreadId = GetWindowThreadProcessId(g_runtime.ui.cachedNativeWordWindow, nullptr);
    } else {
        g_runtime.ui.cachedWordUiThreadId = threadId ? threadId : GetWindowThreadProcessId(rootWindow, nullptr);
    }
}

bool TryGetCachedWordUiThreadId(HWND rootWindow, DWORD* threadId) {
    if (!threadId) {
        return false;
    }

    *threadId = 0;
    if (!IsUsableWordRootWindow(rootWindow)) {
        return false;
    }

    if (!IsUsableWordRootWindow(g_runtime.ui.cachedWordRootWindow)) {
        InvalidateWordUiWindowCache();
        return false;
    }

    if (g_runtime.ui.cachedWordRootWindow != rootWindow || g_runtime.ui.cachedWordUiThreadId == 0) {
        return false;
    }

    if (g_runtime.ui.cachedNativeWordWindow &&
        !IsUsableCachedNativeWordWindow(g_runtime.ui.cachedNativeWordWindow, g_runtime.ui.cachedWordRootWindow)) {
        g_runtime.ui.cachedNativeWordWindow = nullptr;
        g_runtime.ui.cachedWordUiThreadId = GetWindowThreadProcessId(g_runtime.ui.cachedWordRootWindow, nullptr);
    }

    *threadId = g_runtime.ui.cachedWordUiThreadId;
    return true;
}

bool IsWindowInCurrentWordProcess(HWND hwnd) {
    if (!hwnd) {
        return false;
    }

    DWORD processId = 0;
    GetWindowThreadProcessId(hwnd, &processId);
    return processId == g_runtime.wordProcessId;
}

bool WindowHasNativeWordObject(HWND hwnd) {
    if (!hwnd) {
        return false;
    }

    IDispatch* nativeObject = nullptr;
    const HRESULT hr = AccessibleObjectFromWindow(
        hwnd,
        OBJID_NATIVEOM_VALUE,
        kIIDIDispatch,
        reinterpret_cast<void**>(&nativeObject));
    if (SUCCEEDED(hr) && nativeObject) {
        nativeObject->Release();
        return true;
    }

    return false;
}

struct NativeWordWindowSearch {
    HWND result = nullptr;
};

BOOL CALLBACK FindNativeWordObjectChildProc(HWND hwnd, LPARAM lParam) {
    NativeWordWindowSearch* search = reinterpret_cast<NativeWordWindowSearch*>(lParam);
    if (!search || search->result) {
        return FALSE;
    }

    if (WindowHasNativeWordObject(hwnd)) {
        search->result = hwnd;
        return FALSE;
    }

    return TRUE;
}

HWND FindNativeWordObjectWindowInRoot(HWND rootWindow) {
    if (!IsUsableWordRootWindow(rootWindow)) {
        return nullptr;
    }

    DWORD cachedThreadId = 0;
    if (TryGetCachedWordUiThreadId(rootWindow, &cachedThreadId) &&
        IsUsableCachedNativeWordWindow(g_runtime.ui.cachedNativeWordWindow, rootWindow)) {
        return g_runtime.ui.cachedNativeWordWindow;
    }

    const DWORD threadId = GetWindowThreadProcessId(rootWindow, nullptr);
    GUITHREADINFO guiThreadInfo = {};
    guiThreadInfo.cbSize = sizeof(guiThreadInfo);
    if (threadId && GetGUIThreadInfo(threadId, &guiThreadInfo)) {
        HWND candidates[] = {
            guiThreadInfo.hwndFocus,
            guiThreadInfo.hwndCaret,
            rootWindow,
        };

        for (HWND candidate : candidates) {
            if (!candidate) {
                continue;
            }

            HWND cursor = candidate;
            while (cursor) {
                if (WindowHasNativeWordObject(cursor)) {
                    CacheWordUiWindowContext(rootWindow,
                                             cursor,
                                             GetWindowThreadProcessId(cursor, nullptr));
                    return cursor;
                }

                if (cursor == rootWindow) {
                    break;
                }

                cursor = GetParent(cursor);
            }
        }
    }

    NativeWordWindowSearch search = {};
    EnumChildWindows(rootWindow,
                     FindNativeWordObjectChildProc,
                     reinterpret_cast<LPARAM>(&search));
    if (search.result) {
        CacheWordUiWindowContext(rootWindow,
                                 search.result,
                                 GetWindowThreadProcessId(search.result, nullptr));
    }
    return search.result;
}

struct WordRootWindowSearch {
    HWND preferredRoot = nullptr;
    HWND result = nullptr;
};

BOOL CALLBACK FindCurrentProcessWordRootWindowProc(HWND hwnd, LPARAM lParam) {
    WordRootWindowSearch* search = reinterpret_cast<WordRootWindowSearch*>(lParam);
    if (!search) {
        return FALSE;
    }

    if (!IsWindowInCurrentWordProcess(hwnd) || !HasClassName(hwnd, L"OpusApp")) {
        return TRUE;
    }

    if (hwnd == search->preferredRoot) {
        search->result = hwnd;
        return FALSE;
    }

    if (!search->result) {
        search->result = hwnd;
    }

    return TRUE;
}

HWND FindCurrentProcessWordRootWindow() {
    HWND preferredRoot = nullptr;
    HWND foregroundWindow = GetForegroundWindow();
    if (foregroundWindow && IsWindowInCurrentWordProcess(foregroundWindow)) {
        preferredRoot = GetAncestor(foregroundWindow, GA_ROOT);
        if (!preferredRoot) {
            preferredRoot = foregroundWindow;
        }

        if (IsUsableWordRootWindow(preferredRoot)) {
            return preferredRoot;
        }
    }

    if (IsUsableWordRootWindow(g_runtime.ui.cachedWordRootWindow)) {
        return g_runtime.ui.cachedWordRootWindow;
    }

    if (preferredRoot && !IsUsableWordRootWindow(preferredRoot)) {
        preferredRoot = nullptr;
    }

    WordRootWindowSearch search = {};
    search.preferredRoot = preferredRoot;
    EnumWindows(FindCurrentProcessWordRootWindowProc, reinterpret_cast<LPARAM>(&search));
    return search.result;
}

struct ProcessNativeWordWindowSearch {
    HWND result = nullptr;
};

BOOL CALLBACK FindCurrentProcessNativeWordWindowProc(HWND hwnd, LPARAM lParam) {
    ProcessNativeWordWindowSearch* search =
        reinterpret_cast<ProcessNativeWordWindowSearch*>(lParam);
    if (!search) {
        return FALSE;
    }

    if (!IsWindowInCurrentWordProcess(hwnd) || !HasClassName(hwnd, L"OpusApp")) {
        return TRUE;
    }

    HWND nativeWordWindow = FindNativeWordObjectWindowInRoot(hwnd);
    if (nativeWordWindow) {
        search->result = nativeWordWindow;
        return FALSE;
    }

    return TRUE;
}

HWND FindCurrentProcessNativeWordObjectWindow() {
    HWND preferredRoot = FindCurrentProcessWordRootWindow();
    if (preferredRoot) {
        if (IsUsableCachedNativeWordWindow(g_runtime.ui.cachedNativeWordWindow, preferredRoot)) {
            return g_runtime.ui.cachedNativeWordWindow;
        }

        HWND preferredNativeWindow = FindNativeWordObjectWindowInRoot(preferredRoot);
        if (preferredNativeWindow) {
            return preferredNativeWindow;
        }
    }

    ProcessNativeWordWindowSearch search = {};
    EnumWindows(FindCurrentProcessNativeWordWindowProc,
                reinterpret_cast<LPARAM>(&search));
    return search.result;
}

HWND FindActiveWordRootWindow() {
    HWND foregroundWindow = GetForegroundWindow();
    if (!foregroundWindow || !IsWindowInCurrentWordProcess(foregroundWindow)) {
        return nullptr;
    }

    HWND rootWindow = GetAncestor(foregroundWindow, GA_ROOT);
    if (!rootWindow) {
        rootWindow = foregroundWindow;
    }

    if (!IsUsableWordRootWindow(rootWindow)) {
        return nullptr;
    }

    return rootWindow;
}

HWND FindActiveWordNativeObjectWindow() {
    HWND rootWindow = FindActiveWordRootWindow();
    if (!rootWindow) {
        return nullptr;
    }

    if (IsUsableCachedNativeWordWindow(g_runtime.ui.cachedNativeWordWindow, rootWindow)) {
        return g_runtime.ui.cachedNativeWordWindow;
    }

    HWND foregroundWindow = GetForegroundWindow();
    if (foregroundWindow && IsWindowInCurrentWordProcess(foregroundWindow)) {
        HWND candidate = foregroundWindow;
        while (candidate) {
            if (WindowHasNativeWordObject(candidate)) {
                CacheWordUiWindowContext(rootWindow,
                                         candidate,
                                         GetWindowThreadProcessId(candidate, nullptr));
                return candidate;
            }

            if (candidate == rootWindow) {
                break;
            }

            candidate = GetParent(candidate);
        }
    }

    return FindNativeWordObjectWindowInRoot(rootWindow);
}

DWORD GetWordUiThreadIdForMessage(const MSG* lpMsg) {
    if (!lpMsg) {
        return 0;
    }

    HWND hwnd = lpMsg->hwnd;
    if (!hwnd || !IsWindowInCurrentWordProcess(hwnd)) {
        hwnd = GetForegroundWindow();
        if (!hwnd || !IsWindowInCurrentWordProcess(hwnd)) {
            return 0;
        }
    }

    HWND rootWindow = GetAncestor(hwnd, GA_ROOT);
    if (!rootWindow) {
        rootWindow = hwnd;
    }

    if (!IsUsableWordRootWindow(rootWindow)) {
        return 0;
    }

    DWORD cachedThreadId = 0;
    if (TryGetCachedWordUiThreadId(rootWindow, &cachedThreadId)) {
        return cachedThreadId;
    }

    HWND nativeWordWindow = FindNativeWordObjectWindowInRoot(rootWindow);
    if (nativeWordWindow) {
        return GetWindowThreadProcessId(nativeWordWindow, nullptr);
    }

    const DWORD threadId = GetWindowThreadProcessId(hwnd, nullptr);
    CacheWordUiWindowContext(rootWindow, nullptr, threadId);
    return threadId;
}

DWORD GetActiveWordUiThreadId() {
    HWND rootWindow = FindActiveWordRootWindow();
    if (!rootWindow) {
        return 0;
    }

    DWORD cachedThreadId = 0;
    if (TryGetCachedWordUiThreadId(rootWindow, &cachedThreadId)) {
        return cachedThreadId;
    }

    HWND nativeWordWindow = FindActiveWordNativeObjectWindow();
    if (nativeWordWindow) {
        return GetWindowThreadProcessId(nativeWordWindow, nullptr);
    }

    const DWORD threadId = GetWindowThreadProcessId(rootWindow, nullptr);
    CacheWordUiWindowContext(rootWindow, nullptr, threadId);
    return threadId;
}

DWORD ResolvePreferredOwnerThreadId(const MSG* lpMsg = nullptr) {
    DWORD preferredThreadId = GetWordUiThreadIdForMessage(lpMsg);
    if (preferredThreadId == 0) {
        preferredThreadId = GetActiveWordUiThreadId();
    }

    return preferredThreadId;
}

enum class WordUiPauseReason {
    None,
    ImeComposition,
    MenuMode,
    ModalUi,
};

const wchar_t* DescribeWordUiPauseReason(WordUiPauseReason reason) {
    switch (reason) {
        case WordUiPauseReason::ImeComposition:
            return L"IME composition is active";
        case WordUiPauseReason::MenuMode:
            return L"Word menu or system UI is active";
        case WordUiPauseReason::ModalUi:
            return L"Word modal UI is active";
        case WordUiPauseReason::None:
            break;
    }

    return L"Word UI is busy";
}

void LogSaveStatus(const wchar_t* message) {
    if (ShouldLogStatusMessageNow(&g_runtime.status.lastSaveStatusMessage,
                                  &g_runtime.status.lastSaveStatusLogTime,
                                  message)) {
        Wh_Log(L"Auto-save: %ls", message);
    }
}

void LogDocumentStateStatus(const wchar_t* message) {
    if (ShouldLogStatusMessageNow(&g_runtime.status.lastDocumentStateStatusMessage,
                                  &g_runtime.status.lastDocumentStateStatusLogTime,
                                  message)) {
        Wh_Log(L"Document state monitor: %ls", message);
    }
}

WordUiPauseReason GetWordUiPauseReason() {
    if (LoadFlag(g_runtime.flags.imeComposing)) {
        return WordUiPauseReason::ImeComposition;
    }

    const DWORD ownerThreadId = LoadOwnerThreadId();
    if (ownerThreadId != 0) {
        GUITHREADINFO guiThreadInfo = {};
        guiThreadInfo.cbSize = sizeof(guiThreadInfo);
        if (GetGUIThreadInfo(ownerThreadId, &guiThreadInfo)) {
            if (guiThreadInfo.flags &
                (GUI_INMOVESIZE | GUI_INMENUMODE | GUI_POPUPMENUMODE | GUI_SYSTEMMENUMODE)) {
                return WordUiPauseReason::MenuMode;
            }
        }
    }

    HWND foregroundWindow = GetForegroundWindow();
    if (!foregroundWindow) {
        return WordUiPauseReason::None;
    }

    DWORD foregroundProcessId = 0;
    GetWindowThreadProcessId(foregroundWindow, &foregroundProcessId);
    if (foregroundProcessId != g_runtime.wordProcessId) {
        return WordUiPauseReason::None;
    }

    HWND rootWindow = GetAncestor(foregroundWindow, GA_ROOT);
    if (!rootWindow) {
        rootWindow = foregroundWindow;
    }

    if (!HasClassName(rootWindow, L"OpusApp")) {
        return WordUiPauseReason::ModalUi;
    }

    return WordUiPauseReason::None;
}

DWORD GetWordUiPauseDelay(WordUiPauseReason reason) {
    switch (reason) {
        case WordUiPauseReason::ImeComposition:
        case WordUiPauseReason::MenuMode:
            return INPUT_SETTLE_DELAY_MS;

        case WordUiPauseReason::ModalUi:
            return DOCUMENT_STATE_ACTIVE_POLL_INTERVAL_MS;

        case WordUiPauseReason::None:
            break;
    }

    return DOCUMENT_STATE_ACTIVE_POLL_INTERVAL_MS;
}

struct TickRuntimeObservation {
    DWORD preferredOwnerThreadId = 0;
    WordUiPauseReason pauseReason = WordUiPauseReason::None;
    bool ownerThreadSynchronized = false;
    bool activeWordDocumentWindow = false;
    bool inputBusy = false;
    bool pendingSave = false;
    bool manualSavePending = false;
    bool transitionFlushRequested = false;
    bool expeditedSaveRequested = false;
    bool wordEventsConnected = false;
};

struct RuntimeFlagSnapshot {
    bool pendingSave = false;
    bool documentDirtyKnown = false;
    bool documentDirty = false;
    bool manualSavePending = false;
    bool automationBusyPending = false;
    bool pendingSaveAsMigration = false;
    bool wordEventsConnected = false;
};

struct RuntimeTickSnapshot {
    TickRuntimeObservation runtime;
    RuntimeFlagSnapshot flags;
    ULONGLONG now = 0;
    DWORD steadyPollDelay = DOCUMENT_STATE_IDLE_POLL_INTERVAL_MS;
    bool pendingSaveWork = false;
    bool pendingSaveAsMigration = false;
    bool documentDirtyKnown = false;
};

struct TickDecision {
    RuntimeStatePhase state = RuntimeStatePhase::Idle;
    TickDecisionAction action = TickDecisionAction::None;
    DWORD delayMs = 0;
};

bool IsOwnerThreadStillPreferred(DWORD preferredOwnerThreadId) {
    return IsOwnerThread() &&
           (preferredOwnerThreadId == 0 || preferredOwnerThreadId == GetCurrentThreadId());
}

bool IsOwnerThreadSchedulerContextValid() {
    return LoadOwnerThreadId() != 0 && IsOwnerThread();
}

bool CanRunOwnerThreadRuntimeWork() {
    return IsOwnerThreadSchedulerContextValid() &&
           LoadFlag(g_runtime.flags.moduleActive);
}

bool ShouldTrackDocumentStateWhileInactive(bool pendingSaveWork,
                                           bool pendingSaveAsMigration,
                                           bool wordEventsConnected) {
    return pendingSaveWork || pendingSaveAsMigration || !wordEventsConnected;
}

bool ShouldRequireCleanSnapshotDetails(bool manualSavePending,
                                       bool pendingSave,
                                       bool pendingSaveAsMigration,
                                       bool documentDirtyKnown) {
    return manualSavePending || pendingSave || pendingSaveAsMigration || documentDirtyKnown;
}

void CaptureTickWindowAndInputObservation(TickRuntimeObservation* runtime,
                                          const MSG* lpMsg = nullptr) {
    if (!runtime) {
        return;
    }

    runtime->preferredOwnerThreadId = ResolvePreferredOwnerThreadId(lpMsg);
    runtime->ownerThreadSynchronized =
        IsOwnerThreadStillPreferred(runtime->preferredOwnerThreadId);
    runtime->activeWordDocumentWindow =
        runtime->preferredOwnerThreadId != 0;
    runtime->inputBusy = GetInputState() || AreModifiersOrMouseButtonsHeld();
    runtime->transitionFlushRequested =
        LoadFlag(g_runtime.flags.transitionFlushPending);
    runtime->expeditedSaveRequested =
        LoadFlag(g_runtime.flags.expeditedSavePending);
    runtime->pauseReason = GetWordUiPauseReason();
}

RuntimeTickSnapshot CaptureDocumentStateTickSnapshot(const MSG* lpMsg = nullptr) {
    RuntimeTickSnapshot snapshot = {};
    snapshot.now = GetTickCount64();
    CaptureTickWindowAndInputObservation(&snapshot.runtime, lpMsg);
    snapshot.runtime.pendingSave =
        LoadFlag(g_runtime.flags.pendingSave);
    snapshot.runtime.manualSavePending =
        LoadFlag(g_runtime.flags.manualSavePending);
    snapshot.runtime.wordEventsConnected =
        LoadFlag(g_runtime.flags.wordEventsConnected);

    snapshot.flags.pendingSave = snapshot.runtime.pendingSave;
    snapshot.flags.documentDirtyKnown =
        LoadFlag(g_runtime.flags.documentDirtyKnown);
    snapshot.flags.documentDirty =
        LoadFlag(g_runtime.flags.documentDirty);
    snapshot.flags.manualSavePending = snapshot.runtime.manualSavePending;
    snapshot.flags.automationBusyPending =
        LoadFlag(g_runtime.flags.automationBusyPending);
    snapshot.flags.pendingSaveAsMigration =
        LoadFlag(g_runtime.flags.pendingSaveAsMigration);
    snapshot.flags.wordEventsConnected = snapshot.runtime.wordEventsConnected;

    snapshot.steadyPollDelay = GetSteadyDocumentStatePollDelay(&snapshot.flags);
    snapshot.pendingSaveWork = snapshot.flags.pendingSave ||
                               snapshot.flags.manualSavePending ||
                               snapshot.flags.documentDirty;
    snapshot.pendingSaveAsMigration = snapshot.flags.pendingSaveAsMigration;
    snapshot.documentDirtyKnown = snapshot.flags.documentDirtyKnown;
    return snapshot;
}

RuntimeTickSnapshot CaptureAutosaveTickSnapshot(const MSG* lpMsg = nullptr) {
    RuntimeTickSnapshot snapshot = {};
    snapshot.now = GetTickCount64();
    snapshot.runtime.pendingSave =
        LoadFlag(g_runtime.flags.pendingSave);
    if (!snapshot.runtime.pendingSave) {
        return snapshot;
    }

    CaptureTickWindowAndInputObservation(&snapshot.runtime, lpMsg);
    return snapshot;
}

TickDecision MakeDocumentStateRearmDecision(RuntimeStatePhase state, DWORD delayMs) {
    TickDecision decision = {};
    decision.state = state;
    decision.action = TickDecisionAction::RearmDocumentStateTimer;
    decision.delayMs = delayMs;
    return decision;
}

TickDecision MakeSaveRearmDecision(RuntimeStatePhase state, DWORD delayMs) {
    TickDecision decision = {};
    decision.state = state;
    decision.action = TickDecisionAction::RearmSaveTimer;
    decision.delayMs = delayMs;
    return decision;
}

TickDecision EvaluateDocumentStateTickDecision(const RuntimeTickSnapshot& snapshot) {
    if (!snapshot.runtime.ownerThreadSynchronized) {
        return {RuntimeStatePhase::WaitingForOwnerThread, TickDecisionAction::None, 0};
    }

    if (snapshot.runtime.pauseReason != WordUiPauseReason::None) {
        return MakeDocumentStateRearmDecision(RuntimeStatePhase::WaitingForWordUi,
                                              GetWordUiPauseDelay(snapshot.runtime.pauseReason));
    }

    if (!snapshot.runtime.activeWordDocumentWindow) {
        if (ShouldTrackDocumentStateWhileInactive(snapshot.pendingSaveWork,
                                                  snapshot.pendingSaveAsMigration,
                                                  snapshot.runtime.wordEventsConnected)) {
            return MakeDocumentStateRearmDecision(RuntimeStatePhase::WaitingForWordWindow,
                                                  snapshot.steadyPollDelay);
        }

        return {RuntimeStatePhase::Idle, TickDecisionAction::None, 0};
    }

    if (snapshot.runtime.pendingSave &&
        !snapshot.runtime.manualSavePending &&
        !snapshot.pendingSaveAsMigration) {
        return MakeDocumentStateRearmDecision(RuntimeStatePhase::WaitingForSaveDelay,
                                              DOCUMENT_STATE_ACTIVE_POLL_INTERVAL_MS);
    }

    if (snapshot.runtime.inputBusy) {
        return MakeDocumentStateRearmDecision(RuntimeStatePhase::WaitingForInputToSettle,
                                              INPUT_SETTLE_DELAY_MS);
    }

    return {RuntimeStatePhase::ReadyToRefreshDocumentState,
            TickDecisionAction::RefreshDocumentState,
            0};
}

TickDecision EvaluateAutosaveTickDecision(const RuntimeTickSnapshot& snapshot) {
    if (!snapshot.runtime.pendingSave) {
        return {RuntimeStatePhase::Idle, TickDecisionAction::None, 0};
    }

    if (!snapshot.runtime.ownerThreadSynchronized) {
        return {RuntimeStatePhase::WaitingForOwnerThread, TickDecisionAction::None, 0};
    }

    if (snapshot.runtime.pauseReason != WordUiPauseReason::None) {
        return MakeSaveRearmDecision(RuntimeStatePhase::WaitingForWordUi,
                                     GetWordUiPauseDelay(snapshot.runtime.pauseReason));
    }

    if (!snapshot.runtime.transitionFlushRequested &&
        !snapshot.runtime.activeWordDocumentWindow) {
        return MakeSaveRearmDecision(RuntimeStatePhase::WaitingForWordWindow,
                                     DOCUMENT_STATE_ACTIVE_POLL_INTERVAL_MS);
    }

    const DWORD effectiveDelayMs =
        snapshot.runtime.expeditedSaveRequested || snapshot.runtime.transitionFlushRequested
            ? INPUT_SETTLE_DELAY_MS
            : static_cast<DWORD>(g_settings.saveDelay);
    const ULONGLONG earliestEditSaveTime =
        g_runtime.timing.lastEditTime + static_cast<ULONGLONG>(effectiveDelayMs);
    if (snapshot.now < earliestEditSaveTime) {
        return MakeSaveRearmDecision(RuntimeStatePhase::WaitingForSaveDelay,
                                     static_cast<DWORD>(earliestEditSaveTime - snapshot.now));
    }

    if (g_settings.minTimeBetweenSaves > 0 && g_runtime.timing.lastSaveTime > 0) {
        const ULONGLONG earliestAllowedSave =
            g_runtime.timing.lastSaveTime + static_cast<ULONGLONG>(g_settings.minTimeBetweenSaves);
        if (snapshot.now < earliestAllowedSave) {
            return MakeSaveRearmDecision(RuntimeStatePhase::WaitingForMinSaveInterval,
                                         static_cast<DWORD>(earliestAllowedSave - snapshot.now));
        }
    }

    if (snapshot.runtime.inputBusy) {
        return MakeSaveRearmDecision(RuntimeStatePhase::WaitingForInputToSettle,
                                     INPUT_SETTLE_DELAY_MS);
    }

    return {RuntimeStatePhase::ReadyToSave, TickDecisionAction::SaveDocument, 0};
}

bool AreModifiersOrMouseButtonsHeld() {
    return IsAsyncKeyDown(VK_SHIFT) ||
           IsAsyncKeyDown(VK_CONTROL) ||
           IsAsyncKeyDown(VK_MENU) ||
           IsAsyncKeyDown(VK_LWIN) ||
           IsAsyncKeyDown(VK_RWIN) ||
           IsAsyncKeyDown(VK_LBUTTON) ||
           IsAsyncKeyDown(VK_RBUTTON) ||
           IsAsyncKeyDown(VK_MBUTTON) ||
           IsAsyncKeyDown(VK_XBUTTON1) ||
           IsAsyncKeyDown(VK_XBUTTON2);
}

HWND FindNativeWordViewWindow() {
    return FindCurrentProcessNativeWordObjectWindow();
}

bool ArmDocumentStateTimer(DWORD delayMs);
bool ArmSaveTimer(DWORD delayMs);
bool EnsureWordApplicationEventsConnected(bool forceReconnect = false);
void DisconnectWordApplicationEvents();
void CALLBACK SchedulerTimerProc(HWND, UINT, UINT_PTR, DWORD);

void AdoptOwnerThreadIfNeeded(const MSG* lpMsg) {
    if (!lpMsg || !IsOwnerCandidateMessage(lpMsg->message)) {
        return;
    }

    const DWORD currentThreadId = GetCurrentThreadId();
    const DWORD preferredThreadId = ResolvePreferredOwnerThreadId(lpMsg);
    if (preferredThreadId == 0 || currentThreadId != preferredThreadId) {
        return;
    }

    if (lpMsg->hwnd && IsWindowInCurrentWordProcess(lpMsg->hwnd)) {
        HWND rootWindow = GetAncestor(lpMsg->hwnd, GA_ROOT);
        if (!rootWindow) {
            rootWindow = lpMsg->hwnd;
        }

        if (IsUsableWordRootWindow(rootWindow)) {
            HWND nativeWordWindow =
                IsUsableCachedNativeWordWindow(g_runtime.ui.cachedNativeWordWindow, rootWindow)
                    ? g_runtime.ui.cachedNativeWordWindow
                    : nullptr;
            CacheWordUiWindowContext(rootWindow, nativeWordWindow, currentThreadId);
        }
    }

    const DWORD previousOwnerThreadId = ExchangeOwnerThreadId(currentThreadId);
    if (previousOwnerThreadId != currentThreadId) {
        g_runtime.timing.schedulerTimerId = 0;
        g_runtime.timing.schedulerTimerDueTime = 0;
        g_runtime.timing.saveTimerDueTime = 0;
        g_runtime.timing.documentStateTimerDueTime = 0;
        g_runtime.timing.saveRetryDelayMs = MIN_RETRY_INTERVAL_MS;
        g_runtime.timing.documentStateRetryDelayMs = MIN_RETRY_INTERVAL_MS;
        EnsureWordApplicationEventsConnected(true);
        ArmDocumentStateTimer(INPUT_SETTLE_DELAY_MS);
        if (LoadFlag(g_runtime.flags.pendingSave)) {
            ArmSaveTimer(INPUT_SETTLE_DELAY_MS);
        }
    }
}

ULONGLONG* GetScheduledTaskDueTimeStorage(ScheduledTaskKind taskKind) {
    switch (taskKind) {
        case ScheduledTaskKind::Save:
            return &g_runtime.timing.saveTimerDueTime;

        case ScheduledTaskKind::DocumentState:
            return &g_runtime.timing.documentStateTimerDueTime;
    }

    return nullptr;
}

ULONGLONG GetNextScheduledTaskDueTime() {
    ULONGLONG nextDueTime = 0;
    if (g_runtime.timing.saveTimerDueTime != 0) {
        nextDueTime = g_runtime.timing.saveTimerDueTime;
    }

    if (g_runtime.timing.documentStateTimerDueTime != 0 &&
        (nextDueTime == 0 || g_runtime.timing.documentStateTimerDueTime < nextDueTime)) {
        nextDueTime = g_runtime.timing.documentStateTimerDueTime;
    }

    return nextDueTime;
}

void CancelSchedulerTimer() {
    if (g_runtime.timing.schedulerTimerId != 0) {
        KillTimer(nullptr, g_runtime.timing.schedulerTimerId);
        g_runtime.timing.schedulerTimerId = 0;
    }

    g_runtime.timing.schedulerTimerDueTime = 0;
}

bool RefreshSchedulerTimer() {
    const ULONGLONG nextDueTime = GetNextScheduledTaskDueTime();
    if (nextDueTime == 0) {
        CancelSchedulerTimer();
        return true;
    }

    if (!IsOwnerThreadSchedulerContextValid()) {
        return false;
    }

    if (g_runtime.timing.schedulerTimerId != 0 &&
        g_runtime.timing.schedulerTimerDueTime != 0 &&
        g_runtime.timing.schedulerTimerDueTime <= nextDueTime) {
        return true;
    }

    CancelSchedulerTimer();

    const ULONGLONG now = GetTickCount64();
    const DWORD delayMs =
        nextDueTime <= now ? 1 : static_cast<DWORD>(nextDueTime - now);
    g_runtime.timing.schedulerTimerId = SetTimer(nullptr, 0, delayMs, SchedulerTimerProc);
    if (g_runtime.timing.schedulerTimerId == 0) {
        Wh_Log(L"Scheduler: SetTimer failed, error=%lu", GetLastError());
        return false;
    }

    g_runtime.timing.schedulerTimerDueTime = nextDueTime;
    return true;
}

bool ScheduleTask(ScheduledTaskKind taskKind, DWORD delayMs) {
    if (!IsOwnerThreadSchedulerContextValid()) {
        return false;
    }

    ULONGLONG* dueTimeStorage = GetScheduledTaskDueTimeStorage(taskKind);
    if (!dueTimeStorage) {
        return false;
    }

    const DWORD effectiveDelayMs = delayMs ? delayMs : 1;
    const ULONGLONG dueTime = GetTickCount64() + effectiveDelayMs;
    if (*dueTimeStorage != 0 && *dueTimeStorage <= dueTime) {
        return RefreshSchedulerTimer();
    }

    *dueTimeStorage = dueTime;
    return RefreshSchedulerTimer();
}

void CancelScheduledTask(ScheduledTaskKind taskKind) {
    ULONGLONG* dueTimeStorage = GetScheduledTaskDueTimeStorage(taskKind);
    if (!dueTimeStorage) {
        return;
    }

    *dueTimeStorage = 0;
    RefreshSchedulerTimer();
}

bool IsScheduledTaskArmed(ScheduledTaskKind taskKind) {
    ULONGLONG* dueTimeStorage = GetScheduledTaskDueTimeStorage(taskKind);
    return dueTimeStorage && *dueTimeStorage != 0;
}

bool IsScheduledTaskDue(ScheduledTaskKind taskKind, ULONGLONG now) {
    ULONGLONG* dueTimeStorage = GetScheduledTaskDueTimeStorage(taskKind);
    return dueTimeStorage && *dueTimeStorage != 0 && *dueTimeStorage <= now;
}

void CancelSaveTimer() {
    CancelScheduledTask(ScheduledTaskKind::Save);
}

void CancelDocumentStateTimer() {
    CancelScheduledTask(ScheduledTaskKind::DocumentState);
}

void HandleAutosaveTick();
void HandleDocumentStateTick();
void ExpirePendingSaveAsMigrationIfNeeded();

DWORD ComputeSteadyDocumentStatePollDelay(bool hasActivePollWork,
                                          bool wordEventsConnected) {
    if (hasActivePollWork) {
        return DOCUMENT_STATE_ACTIVE_POLL_INTERVAL_MS;
    }

    if (wordEventsConnected) {
        return DOCUMENT_STATE_EVENT_IDLE_POLL_INTERVAL_MS;
    }

    return DOCUMENT_STATE_IDLE_POLL_INTERVAL_MS;
}

DWORD GetSteadyDocumentStatePollDelay(const RuntimeFlagSnapshot* flags) {
    const bool hasActivePollWork =
        flags ? (flags->pendingSave ||
                 flags->documentDirty ||
                 flags->manualSavePending ||
                 flags->automationBusyPending ||
                 flags->pendingSaveAsMigration)
              : (HasPendingSaveWork() ||
                 LoadFlag(g_runtime.flags.automationBusyPending) ||
                 HasPendingSaveAsMigration());
    const bool wordEventsConnected =
        flags ? flags->wordEventsConnected : AreWordEventsConnected();

    return ComputeSteadyDocumentStatePollDelay(hasActivePollWork, wordEventsConnected);
}

DWORD GetBoundaryCoalesceDelay(bool transitionFlush) {
    return ComputeBoundaryCoalesceDelay(transitionFlush, g_runtime.timing.lastEditTime, GetTickCount64());
}

void RequestPendingSaveExpedite(bool transitionFlush,
                                const wchar_t* reason,
                                DWORD delayMs = INPUT_SETTLE_DELAY_MS) {
    if (transitionFlush) {
        RequestTransitionFlush(g_runtime.document.observedDocumentPath.Get(), reason);
    } else {
        SetFlag(g_runtime.flags.expeditedSavePending);
        if (reason) {
            LogSaveStatus(reason);
        }
    }

    if (HasPendingAutosave()) {
        ArmSaveTimer(delayMs);
    } else if (HasPendingSaveWork()) {
        ArmDocumentStateTimer(delayMs);
    }
}

void MaybeKickAutomationRecovery(bool inputBusy) {
    if (!LoadFlag(g_runtime.flags.automationBusyPending)) {
        return;
    }

    if (inputBusy) {
        return;
    }

    ClearAutomationBusyPending();
    if (HasPendingAutosave()) {
        LogSaveStatus(L"Word is responsive again, retrying pending changes");
        ArmSaveTimer(AUTOMATION_RECOVERY_DELAY_MS);
    } else if (HasPendingSaveWork()) {
        LogDocumentStateStatus(L"Word is responsive again, refreshing document state");
        ArmDocumentStateTimer(AUTOMATION_RECOVERY_DELAY_MS);
    }
}

void MaybeKickAutomationRecovery() {
    MaybeKickAutomationRecovery(GetInputState() || AreModifiersOrMouseButtonsHeld());
}

bool ArmDocumentStateTimer(DWORD delayMs) {
    return ScheduleTask(ScheduledTaskKind::DocumentState, delayMs);
}

void ScheduleSaveFromEdit() {
    ClearTransitionFlushRequest();
    ClearExpeditedSavePending();
    ClearAutomationBusyPending();
    g_runtime.timing.lastEditTime = GetTickCount64();
    SetFlag(g_runtime.flags.pendingSave);
    ArmSaveTimer(static_cast<DWORD>(g_settings.saveDelay));
}

void ClearPendingSave() {
    ClearFlag(g_runtime.flags.pendingSave);
    ClearExpeditedSavePending();
}

void HandleManualSave() {
    ClearTransitionFlushRequest();
    SetFlag(g_runtime.flags.manualSavePending);
    CancelSaveTimer();
    ArmDocumentStateTimer(INPUT_SETTLE_DELAY_MS);
}

// ============================================================================
// COM Helpers
// ============================================================================

class ScopedComInit {
public:
    ScopedComInit() {
        m_hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
        if (m_hr == RPC_E_CHANGED_MODE) {
            m_hr = S_OK;
            m_shouldUninitialize = false;
            return;
        }

        m_shouldUninitialize = SUCCEEDED(m_hr);
    }

    ~ScopedComInit() {
        if (m_shouldUninitialize) {
            CoUninitialize();
        }
    }

    HRESULT GetResult() const {
        return m_hr;
    }

private:
    HRESULT m_hr = E_FAIL;
    bool m_shouldUninitialize = false;
};

class ScopedVariant {
public:
    ScopedVariant() {
        VariantInit(&m_value);
    }

    ScopedVariant(const ScopedVariant&) = delete;
    ScopedVariant& operator=(const ScopedVariant&) = delete;

    ~ScopedVariant() {
        VariantClear(&m_value);
    }

    VARIANT* Get() {
        return &m_value;
    }

    const VARIANT* Get() const {
        return &m_value;
    }

private:
    VARIANT m_value = {};
};

bool TryGetCachedDispatchMemberId(IDispatch* dispatch,
                                  const wchar_t* name,
                                  DISPID* dispatchId) {
    if (!dispatch || !name || !dispatchId) {
        return false;
    }

    GUID typeGuid = GUID_NULL;
    if (FAILED(GetDispatchTypeGuid(dispatch, &typeGuid)) ||
        IsEqualGUID(typeGuid, GUID_NULL)) {
        return false;
    }

    const ULONGLONG now = GetTickCount64();
    for (int index = 0; index < DISPATCH_MEMBER_ID_CACHE_CAPACITY; ++index) {
        DispatchMemberIdCacheEntry* entry = &g_runtime.dispatchMemberIdCache[index];
        if (!entry->hasTypeGuid ||
            !IsEqualGUID(entry->typeGuid, typeGuid) ||
            !entry->name) {
            continue;
        }

        if (entry->name == name || lstrcmpW(entry->name, name) == 0) {
            *dispatchId = entry->dispId;
            entry->lastUseTime = now;
            return true;
        }
    }

    return false;
}

void StoreCachedDispatchMemberId(IDispatch* dispatch,
                                 const wchar_t* name,
                                 DISPID dispatchId) {
    if (!dispatch || !name || dispatchId == DISPID_UNKNOWN) {
        return;
    }

    GUID typeGuid = GUID_NULL;
    if (FAILED(GetDispatchTypeGuid(dispatch, &typeGuid)) ||
        IsEqualGUID(typeGuid, GUID_NULL)) {
        return;
    }

    const ULONGLONG now = GetTickCount64();
    DispatchMemberIdCacheEntry* selectedEntry = nullptr;
    DispatchMemberIdCacheEntry* oldestEntry = nullptr;

    for (int index = 0; index < DISPATCH_MEMBER_ID_CACHE_CAPACITY; ++index) {
        DispatchMemberIdCacheEntry* entry = &g_runtime.dispatchMemberIdCache[index];
        if (entry->hasTypeGuid &&
            IsEqualGUID(entry->typeGuid, typeGuid) &&
            entry->name &&
            (entry->name == name || lstrcmpW(entry->name, name) == 0)) {
            selectedEntry = entry;
            break;
        }

        if (!selectedEntry &&
            (!entry->hasTypeGuid || entry->lastUseTime == 0)) {
            selectedEntry = entry;
        }

        if (!oldestEntry || entry->lastUseTime < oldestEntry->lastUseTime) {
            oldestEntry = entry;
        }
    }

    if (!selectedEntry) {
        selectedEntry = oldestEntry;
    }

    if (!selectedEntry) {
        return;
    }

    selectedEntry->typeGuid = typeGuid;
    selectedEntry->hasTypeGuid = true;
    selectedEntry->name = name;
    selectedEntry->dispId = dispatchId;
    selectedEntry->lastUseTime = now;
}

HRESULT InvokeDispatch(IDispatch* dispatch,
                       WORD flags,
                       LPOLESTR name,
                       VARIANT* result = nullptr,
                       int argCount = 0,
                       VARIANT* args = nullptr) {
    if (!dispatch) {
        return E_POINTER;
    }

    DISPID dispatchId = DISPID_UNKNOWN;
    HRESULT hr = S_OK;
    if (!TryGetCachedDispatchMemberId(dispatch, name, &dispatchId)) {
        hr = dispatch->GetIDsOfNames(kIIDNull, &name, 1, LOCALE_USER_DEFAULT, &dispatchId);
        if (SUCCEEDED(hr)) {
            StoreCachedDispatchMemberId(dispatch, name, dispatchId);
        }
    }

    if (FAILED(hr)) {
        return hr;
    }

    if (argCount < 0) {
        return E_INVALIDARG;
    }

    DISPPARAMS params = {};
    params.cArgs = static_cast<UINT>(argCount);
    params.rgvarg = args;

    DISPID namedArg = DISPID_PROPERTYPUT;
    if (flags & DISPATCH_PROPERTYPUT) {
        params.cNamedArgs = 1;
        params.rgdispidNamedArgs = &namedArg;
    }

    return dispatch->Invoke(dispatchId,
                            kIIDNull,
                            LOCALE_USER_DEFAULT,
                            flags,
                            &params,
                            result,
                            nullptr,
                            nullptr);
}

#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wlanguage-extension-token"
#endif
template <typename T>
HRESULT QueryInterfaceTyped(IUnknown* unknown, T** result) {
    if (!unknown || !result) {
        return E_POINTER;
    }

    *result = nullptr;
    return unknown->QueryInterface(__uuidof(T),
                                   reinterpret_cast<void**>(result));
}
#if defined(__clang__)
#pragma clang diagnostic pop
#endif

HRESULT GetDispatchProperty(IDispatch* dispatch, const wchar_t* name, IDispatch** result) {
    if (!result) {
        return E_POINTER;
    }

    *result = nullptr;

    ScopedVariant value;
    HRESULT hr = InvokeDispatch(dispatch,
                                DISPATCH_PROPERTYGET,
                                const_cast<LPOLESTR>(name),
                                value.Get());
    if (FAILED(hr)) {
        return hr;
    }

    if (value.Get()->vt == VT_DISPATCH && value.Get()->pdispVal) {
        *result = value.Get()->pdispVal;
        value.Get()->pdispVal = nullptr;
        return S_OK;
    }

    if (value.Get()->vt == VT_UNKNOWN && value.Get()->punkVal) {
        hr = QueryInterfaceTyped(value.Get()->punkVal, result);
        return hr;
    }

    return DISP_E_TYPEMISMATCH;
}

HRESULT GetDispatchMethodObject(IDispatch* dispatch,
                                const wchar_t* name,
                                IDispatch** result,
                                int argCount = 0,
                                VARIANT* args = nullptr) {
    if (!result) {
        return E_POINTER;
    }

    *result = nullptr;

    ScopedVariant value;
    HRESULT hr = InvokeDispatch(dispatch,
                                DISPATCH_METHOD | DISPATCH_PROPERTYGET,
                                const_cast<LPOLESTR>(name),
                                value.Get(),
                                argCount,
                                args);
    if (FAILED(hr)) {
        return hr;
    }

    if (value.Get()->vt == VT_DISPATCH && value.Get()->pdispVal) {
        *result = value.Get()->pdispVal;
        value.Get()->pdispVal = nullptr;
        return S_OK;
    }

    if (value.Get()->vt == VT_UNKNOWN && value.Get()->punkVal) {
        return QueryInterfaceTyped(value.Get()->punkVal, result);
    }

    return DISP_E_TYPEMISMATCH;
}

HRESULT GetBoolProperty(IDispatch* dispatch, const wchar_t* name, bool* result) {
    if (!result) {
        return E_POINTER;
    }

    *result = false;

    ScopedVariant value;
    ScopedVariant converted;
    HRESULT hr = InvokeDispatch(dispatch,
                                DISPATCH_PROPERTYGET,
                                const_cast<LPOLESTR>(name),
                                value.Get());
    if (SUCCEEDED(hr)) {
        hr = VariantChangeType(converted.Get(), value.Get(), 0, VT_BOOL);
        if (SUCCEEDED(hr)) {
            *result = converted.Get()->boolVal != VARIANT_FALSE;
        }
    }

    return hr;
}

HRESULT GetBstrProperty(IDispatch* dispatch, const wchar_t* name, BSTR* result) {
    if (!result) {
        return E_POINTER;
    }

    *result = nullptr;

    ScopedVariant value;
    ScopedVariant converted;
    HRESULT hr = InvokeDispatch(dispatch,
                                DISPATCH_PROPERTYGET,
                                const_cast<LPOLESTR>(name),
                                value.Get());
    if (SUCCEEDED(hr)) {
        hr = VariantChangeType(converted.Get(), value.Get(), 0, VT_BSTR);
        if (SUCCEEDED(hr) && converted.Get()->bstrVal) {
            *result = SysAllocString(converted.Get()->bstrVal);
            hr = *result ? S_OK : E_OUTOFMEMORY;
        }
    }

    return hr;
}

HRESULT GetIntProperty(IDispatch* dispatch, const wchar_t* name, long* result) {
    if (!result) {
        return E_POINTER;
    }

    *result = 0;

    ScopedVariant value;
    ScopedVariant converted;
    HRESULT hr = InvokeDispatch(dispatch,
                                DISPATCH_PROPERTYGET,
                                const_cast<LPOLESTR>(name),
                                value.Get());
    if (SUCCEEDED(hr)) {
        hr = VariantChangeType(converted.Get(), value.Get(), 0, VT_I4);
        if (SUCCEEDED(hr)) {
            *result = converted.Get()->lVal;
        }
    }

    return hr;
}

HRESULT GetComIdentity(IUnknown* unknown, IUnknown** result) {
    if (!unknown || !result) {
        return E_POINTER;
    }

    *result = nullptr;
    return QueryInterfaceTyped(unknown, result);
}

HRESULT GetDispatchTypeGuid(IDispatch* dispatch, GUID* result) {
    if (!dispatch || !result) {
        return E_POINTER;
    }

    *result = GUID_NULL;

    ITypeInfo* typeInfo = nullptr;
    HRESULT hr = dispatch->GetTypeInfo(0, LOCALE_USER_DEFAULT, &typeInfo);
    if (FAILED(hr) || !typeInfo) {
        return FAILED(hr) ? hr : E_FAIL;
    }

    TYPEATTR* typeAttr = nullptr;
    hr = typeInfo->GetTypeAttr(&typeAttr);
    if (SUCCEEDED(hr) && typeAttr) {
        *result = typeAttr->guid;
        typeInfo->ReleaseTypeAttr(typeAttr);
    } else if (SUCCEEDED(hr)) {
        hr = E_FAIL;
    }

    typeInfo->Release();
    return hr;
}

bool AreSameComIdentity(IUnknown* left, IUnknown* right) {
    return left && right && left == right;
}

bool IsUsableWordApplicationWindowHandle(LONG_PTR hwndValue) {
    HWND hwnd = reinterpret_cast<HWND>(hwndValue);
    return hwnd &&
           IsWindow(hwnd) &&
           IsWindowInCurrentWordProcess(hwnd) &&
           HasClassName(hwnd, L"OpusApp");
}

HRESULT GetApplicationWindowHandle(IDispatch* application, LONG_PTR* hwndValue) {
    if (!application || !hwndValue) {
        return E_POINTER;
    }

    *hwndValue = 0;

    long hwndLong = 0;
    HRESULT hr = GetIntProperty(application, L"Hwnd", &hwndLong);
    if (FAILED(hr)) {
        return hr;
    }

    if (hwndLong == 0) {
        return E_FAIL;
    }

    *hwndValue = static_cast<LONG_PTR>(hwndLong);
    return S_OK;
}

bool DoesApplicationBelongToCurrentProcess(IDispatch* application, LONG_PTR* hwndValue = nullptr) {
    LONG_PTR localHwndValue = 0;
    if (FAILED(GetApplicationWindowHandle(application, &localHwndValue))) {
        return false;
    }

    if (!IsUsableWordApplicationWindowHandle(localHwndValue)) {
        return false;
    }

    if (hwndValue) {
        *hwndValue = localHwndValue;
    }

    return true;
}

bool IsWordEventSessionApplicationCacheValid(const WordEventSession& session,
                                             LONG_PTR* applicationHwnd = nullptr) {
    if (!session.application) {
        return false;
    }

    LONG_PTR localApplicationHwnd = 0;
    if (!DoesApplicationBelongToCurrentProcess(session.application.Get(),
                                               &localApplicationHwnd)) {
        return false;
    }

    if (session.applicationHwnd != 0 &&
        session.applicationHwnd != localApplicationHwnd) {
        return false;
    }

    if (applicationHwnd) {
        *applicationHwnd = localApplicationHwnd;
    }

    return true;
}

HRESULT CopyWordEventSessionApplication(const WordEventSession& session, IDispatch** application) {
    if (!application) {
        return E_POINTER;
    }

    *application = nullptr;

    if (!IsWordEventSessionApplicationCacheValid(session)) {
        return E_FAIL;
    }

    session.application.Get()->AddRef();
    *application = session.application.Get();
    return S_OK;
}

HRESULT GetNativeWordObjectFromWindow(HWND hwnd, IDispatch** nativeObject) {
    if (!hwnd || !nativeObject) {
        return E_INVALIDARG;
    }

    *nativeObject = nullptr;
    return AccessibleObjectFromWindow(hwnd,
                                      OBJID_NATIVEOM_VALUE,
                                      kIIDIDispatch,
                                      reinterpret_cast<void**>(nativeObject));
}

HRESULT GetDocumentFromObjectChain(IDispatch* nativeObject, IDispatch** document) {
    if (!nativeObject || !document) {
        return E_POINTER;
    }

    *document = nullptr;

    ScopedComPtr<IDispatch> current;
    nativeObject->AddRef();
    current.Reset(nativeObject);

    for (int depth = 0; depth < 4 && current; ++depth) {
        HRESULT hr = GetDispatchProperty(current.Get(), L"Document", document);
        if (SUCCEEDED(hr) && *document) {
            return S_OK;
        }

        hr = GetDispatchProperty(current.Get(), L"ActiveDocument", document);
        if (SUCCEEDED(hr) && *document) {
            return S_OK;
        }

        ScopedComPtr<IDispatch> parent;
        hr = GetDispatchProperty(current.Get(), L"Parent", parent.Put());
        if (FAILED(hr) || !parent) {
            break;
        }

        current = std::move(parent);
    }

    ScopedComPtr<IDispatch> application;
    HRESULT hr = GetDispatchProperty(nativeObject, L"Application", application.Put());
    if (FAILED(hr) || !application) {
        return FAILED(hr) ? hr : E_FAIL;
    }

    ScopedComPtr<IDispatch> activeWindow;
    hr = GetDispatchProperty(application.Get(), L"ActiveWindow", activeWindow.Put());
    if (SUCCEEDED(hr) && activeWindow) {
        hr = GetDispatchProperty(activeWindow.Get(), L"Document", document);
        if (SUCCEEDED(hr) && *document) {
            return S_OK;
        }
    }

    return GetDispatchProperty(application.Get(), L"ActiveDocument", document);
}

HRESULT GetDocumentIdentityAndPathState(IDispatch* document,
                                        BSTR* result,
                                        bool* hasSavedPath) {
    if (!document || !result || !hasSavedPath) {
        return E_POINTER;
    }

    *result = nullptr;
    *hasSavedPath = false;

    ScopedBstr fullName;
    HRESULT hr = GetBstrProperty(document, L"FullName", fullName.Put());
    if (SUCCEEDED(hr) && fullName.Length() > 0) {
        *hasSavedPath = true;
        *result = fullName.Detach();
        return S_OK;
    }

    ScopedBstr path;
    hr = GetBstrProperty(document, L"Path", path.Put());
    if (FAILED(hr)) {
        return hr;
    }

    if (path.Length() == 0) {
        *hasSavedPath = false;
        return S_FALSE;
    }

    *hasSavedPath = true;

    ScopedBstr name;
    hr = GetBstrProperty(document, L"Name", name.Put());
    if (FAILED(hr)) {
        return hr;
    }

    if (name.Length() == 0) {
        *result = path.Detach();
        return S_OK;
    }

    const UINT pathLength = path.Length();
    const UINT nameLength = name.Length();
    const bool needsSlash = path.CStr()[pathLength - 1] != L'\\' && path.CStr()[pathLength - 1] != L'/';
    const UINT totalLength = pathLength + (needsSlash ? 1u : 0u) + nameLength;

    BSTR identity = SysAllocStringLen(nullptr, totalLength);
    if (!identity) {
        return E_OUTOFMEMORY;
    }

    CopyMemory(identity, path.CStr(), pathLength * sizeof(wchar_t));
    UINT offset = pathLength;
    if (needsSlash) {
        identity[offset++] = L'\\';
    }

    CopyMemory(identity + offset, name.CStr(), nameLength * sizeof(wchar_t));
    identity[totalLength] = L'\0';

    *result = identity;
    return S_OK;
}

HRESULT GetDocumentIdentity(IDispatch* document, BSTR* result) {
    bool hasSavedPath = false;
    return GetDocumentIdentityAndPathState(document, result, &hasSavedPath);
}

void InvalidateTransitionFlushDocumentCache() {
    g_runtime.document.transitionFlushDocument.Reset();
    g_runtime.document.transitionFlushDocumentIdentity.Reset();
}

bool IsTransitionFlushDocumentCacheValid(const wchar_t* expectedPath = nullptr) {
    if (!g_runtime.document.transitionFlushDocument || !g_runtime.document.transitionFlushDocumentIdentity) {
        return false;
    }

    IUnknown* identity = nullptr;
    if (FAILED(GetComIdentity(g_runtime.document.transitionFlushDocument.Get(), &identity)) || !identity) {
        InvalidateTransitionFlushDocumentCache();
        return false;
    }

    const bool sameIdentity =
        AreSameComIdentity(identity, g_runtime.document.transitionFlushDocumentIdentity.Get());
    identity->Release();
    if (!sameIdentity) {
        InvalidateTransitionFlushDocumentCache();
        return false;
    }

    if (expectedPath && *expectedPath) {
        ScopedBstr cachedPath;
        const HRESULT hr =
            GetDocumentIdentity(g_runtime.document.transitionFlushDocument.Get(), cachedPath.Put());
        if (FAILED(hr) ||
            cachedPath.Length() == 0 ||
            !AreSameDocumentPath(cachedPath.CStr(), expectedPath)) {
            InvalidateTransitionFlushDocumentCache();
            return false;
        }
    }

    return true;
}

HRESULT GetWordApplicationFromRot(IDispatch** application) {
    if (!application) {
        return E_POINTER;
    }

    *application = nullptr;

    CLSID wordClsid;
    HRESULT hr = CLSIDFromProgID(L"Word.Application", &wordClsid);
    if (FAILED(hr)) {
        return hr;
    }

    ScopedComPtr<IUnknown> unknown;
    hr = GetActiveObject(wordClsid, nullptr, unknown.Put());
    if (FAILED(hr) || !unknown) {
        return hr;
    }

    hr = QueryInterfaceTyped(unknown.Get(), application);
    if (FAILED(hr) || !*application) {
        return hr;
    }

    if (!DoesApplicationBelongToCurrentProcess(*application)) {
        (*application)->Release();
        *application = nullptr;
        return HRESULT_FROM_WIN32(ERROR_NOT_FOUND);
    }

    return S_OK;
}

HRESULT GetWordApplicationFromActiveWindow(IDispatch** application) {
    if (!application) {
        return E_POINTER;
    }

    *application = nullptr;

    HWND viewWindow = FindNativeWordViewWindow();
    if (!viewWindow) {
        return E_FAIL;
    }

    ScopedComPtr<IDispatch> nativeObject;
    HRESULT hr = GetNativeWordObjectFromWindow(viewWindow, nativeObject.Put());
    if (FAILED(hr) || !nativeObject) {
        return hr;
    }

    hr = GetDispatchProperty(nativeObject.Get(), L"Application", application);
    if (FAILED(hr) || !*application) {
        ScopedComPtr<IDispatch> activeDocument;
        if (SUCCEEDED(GetDispatchProperty(nativeObject.Get(),
                                          L"ActiveDocument",
                                          activeDocument.Put()))) {
            nativeObject.Get()->AddRef();
            *application = nativeObject.Get();
            hr = S_OK;
        }
    }

    if (FAILED(hr) || !*application) {
        return FAILED(hr) ? hr : E_FAIL;
    }

    LONG_PTR hwndValue = 0;
    if (!DoesApplicationBelongToCurrentProcess(*application, &hwndValue)) {
        (*application)->Release();
        *application = nullptr;
        return HRESULT_FROM_WIN32(ERROR_NOT_FOUND);
    }

    return S_OK;
}

HRESULT GetWordApplicationFromConnectedInstance(IDispatch** application) {
    if (!application) {
        return E_POINTER;
    }

    *application = nullptr;

    if (FAILED(CopyWordEventSessionApplication(g_runtime.events.session, application))) {
        DisconnectWordApplicationEvents();
        return E_FAIL;
    }
    return S_OK;
}

enum class WordApplicationSource {
    ActiveWindow,
    ConnectedInstance,
    RunningObjectTable,
};

enum class WordApplicationResolutionStrategy {
    PreferActiveWindow,
    PreferConnectedInstance,
};

HRESULT TryResolveWordApplicationFromSource(WordApplicationSource source,
                                            IDispatch** application) {
    switch (source) {
        case WordApplicationSource::ActiveWindow:
            return GetWordApplicationFromActiveWindow(application);

        case WordApplicationSource::ConnectedInstance:
            return GetWordApplicationFromConnectedInstance(application);

        case WordApplicationSource::RunningObjectTable:
            return GetWordApplicationFromRot(application);
    }

    return E_FAIL;
}

HRESULT ResolveWordApplication(WordApplicationResolutionStrategy strategy,
                               IDispatch** application) {
    if (!application) {
        return E_POINTER;
    }

    *application = nullptr;

    WordApplicationSource sources[3] = {};
    if (strategy == WordApplicationResolutionStrategy::PreferConnectedInstance) {
        sources[0] = WordApplicationSource::ConnectedInstance;
        sources[1] = WordApplicationSource::ActiveWindow;
        sources[2] = WordApplicationSource::RunningObjectTable;
    } else {
        sources[0] = WordApplicationSource::ActiveWindow;
        sources[1] = WordApplicationSource::ConnectedInstance;
        sources[2] = WordApplicationSource::RunningObjectTable;
    }

    HRESULT lastRetryableFailure = E_FAIL;
    HRESULT lastFailure = E_FAIL;
    for (WordApplicationSource source : sources) {
        HRESULT hr = TryResolveWordApplicationFromSource(source, application);
        if (SUCCEEDED(hr) && application && *application) {
            return hr;
        }

        if (IsRetryableAutomationFailure(hr)) {
            lastRetryableFailure = hr;
        } else if (FAILED(hr)) {
            lastFailure = hr;
        }
    }

    return FAILED(lastRetryableFailure) ? lastRetryableFailure : lastFailure;
}

HRESULT GetWordApplication(IDispatch** application) {
    return ResolveWordApplication(WordApplicationResolutionStrategy::PreferActiveWindow,
                                  application);
}

HRESULT GetWordDocumentByPath(const wchar_t* path, IDispatch** document) {
    if (!path || !*path || !document) {
        return E_INVALIDARG;
    }

    *document = nullptr;

    ScopedComPtr<IDispatch> application;
    HRESULT hr = ResolveWordApplication(
        WordApplicationResolutionStrategy::PreferConnectedInstance,
        application.Put());
    if (FAILED(hr) || !application) {
        return hr;
    }

    ScopedComPtr<IDispatch> documents;
    hr = GetDispatchProperty(application.Get(), L"Documents", documents.Put());
    if (FAILED(hr) || !documents) {
        return FAILED(hr) ? hr : E_FAIL;
    }

    long count = 0;
    hr = GetIntProperty(documents.Get(), L"Count", &count);
    if (FAILED(hr)) {
        return hr;
    }

    for (long index = 1; index <= count; ++index) {
        ScopedVariant indexArg;
        indexArg.Get()->vt = VT_I4;
        indexArg.Get()->lVal = index;

        ScopedComPtr<IDispatch> candidate;
        hr = GetDispatchMethodObject(documents.Get(), L"Item", candidate.Put(), 1, indexArg.Get());
        if (FAILED(hr)) {
            if (IsRetryableAutomationFailure(hr)) {
                return hr;
            }

            continue;
        }

        ScopedBstr candidateIdentity;
        hr = GetDocumentIdentity(candidate.Get(), candidateIdentity.Put());
        if (FAILED(hr) || candidateIdentity.Length() == 0) {
            if (FAILED(hr) && IsRetryableAutomationFailure(hr)) {
                return hr;
            }

            continue;
        }

        if (AreSameDocumentPath(candidateIdentity.CStr(), path)) {
            *document = candidate.Detach();
            return S_OK;
        }
    }

    return HRESULT_FROM_WIN32(ERROR_NOT_FOUND);
}

HRESULT GetWordDocumentFromActiveWindow(IDispatch** document) {
    if (!document) {
        return E_POINTER;
    }

    *document = nullptr;

    HWND viewWindow = FindNativeWordViewWindow();
    if (!viewWindow) {
        return E_FAIL;
    }

    ScopedComPtr<IDispatch> nativeObject;
    HRESULT hr = GetNativeWordObjectFromWindow(viewWindow, nativeObject.Put());
    if (FAILED(hr) || !nativeObject) {
        return hr;
    }

    hr = GetDocumentFromObjectChain(nativeObject.Get(), document);
    if (FAILED(hr) || !*document) {
        return FAILED(hr) ? hr : E_FAIL;
    }

    return S_OK;
}

HRESULT ResolveTypeInfoDispId(ITypeInfo* typeInfo, const wchar_t* name, DISPID* dispId) {
    if (!typeInfo || !name || !dispId) {
        return E_POINTER;
    }

    *dispId = DISPID_UNKNOWN;
    LPOLESTR names[] = {const_cast<LPOLESTR>(name)};
    MEMBERID memberId = DISPID_UNKNOWN;
    HRESULT hr = typeInfo->GetIDsOfNames(names, 1, &memberId);
    if (SUCCEEDED(hr)) {
        *dispId = memberId;
    }

    return hr;
}

HRESULT ResolveWordEventDispIds(IDispatch* application, WordEventDispIds* dispIds) {
    if (!application || !dispIds) {
        return E_POINTER;
    }

    dispIds->Reset();

    ScopedComPtr<ITypeInfo> applicationTypeInfo;
    HRESULT hr = application->GetTypeInfo(0, LOCALE_USER_DEFAULT, applicationTypeInfo.Put());
    if (FAILED(hr) || !applicationTypeInfo) {
        return FAILED(hr) ? hr : E_FAIL;
    }

    ScopedComPtr<ITypeLib> typeLib;
    UINT typeInfoIndex = 0;
    hr = applicationTypeInfo->GetContainingTypeLib(typeLib.Put(), &typeInfoIndex);
    if (FAILED(hr) || !typeLib) {
        return FAILED(hr) ? hr : E_FAIL;
    }

    ScopedComPtr<ITypeInfo> eventTypeInfo;
    hr = typeLib->GetTypeInfoOfGuid(kDIIDWordApplicationEvents4, eventTypeInfo.Put());
    if (FAILED(hr) || !eventTypeInfo) {
        return FAILED(hr) ? hr : E_FAIL;
    }

    hr = ResolveTypeInfoDispId(eventTypeInfo.Get(),
                               L"DocumentBeforeSave",
                               &dispIds->documentBeforeSave);
    if (FAILED(hr)) {
        return hr;
    }

    hr = ResolveTypeInfoDispId(eventTypeInfo.Get(),
                               L"DocumentBeforeClose",
                               &dispIds->documentBeforeClose);
    if (FAILED(hr)) {
        return hr;
    }

    hr = ResolveTypeInfoDispId(eventTypeInfo.Get(), L"DocumentChange", &dispIds->documentChange);
    if (FAILED(hr)) {
        return hr;
    }

    hr = ResolveTypeInfoDispId(eventTypeInfo.Get(),
                               L"WindowDeactivate",
                               &dispIds->windowDeactivate);
    if (FAILED(hr)) {
        return hr;
    }

    return S_OK;
}

VARIANT* GetEventArgument(const DISPPARAMS* params, UINT logicalIndex) {
    if (!params || logicalIndex >= params->cArgs) {
        return nullptr;
    }

    return &params->rgvarg[params->cArgs - 1 - logicalIndex];
}

HRESULT CopyDispatchFromVariant(VARIANT* value, IDispatch** result) {
    if (!result) {
        return E_POINTER;
    }

    *result = nullptr;
    if (!value) {
        return E_INVALIDARG;
    }

    if (value->vt == VT_DISPATCH && value->pdispVal) {
        value->pdispVal->AddRef();
        *result = value->pdispVal;
        return S_OK;
    }

    if (value->vt == (VT_DISPATCH | VT_BYREF) && value->ppdispVal && *value->ppdispVal) {
        (*value->ppdispVal)->AddRef();
        *result = *value->ppdispVal;
        return S_OK;
    }

    if (value->vt == VT_UNKNOWN && value->punkVal) {
        return QueryInterfaceTyped(value->punkVal, result);
    }

    if (value->vt == (VT_UNKNOWN | VT_BYREF) && value->ppunkVal && *value->ppunkVal) {
        return QueryInterfaceTyped(*value->ppunkVal, result);
    }

    return DISP_E_TYPEMISMATCH;
}

bool GetBoolFromVariant(VARIANT* value, bool* result) {
    if (!value || !result) {
        return false;
    }

    *result = false;

    const VARTYPE vt = value->vt;
    if (vt == VT_BOOL) {
        *result = value->boolVal != VARIANT_FALSE;
        return true;
    }

    if (vt == (VT_BOOL | VT_BYREF) && value->pboolVal) {
        *result = *value->pboolVal != VARIANT_FALSE;
        return true;
    }

    if (vt == (VT_VARIANT | VT_BYREF) && value->pvarVal) {
        return GetBoolFromVariant(value->pvarVal, result);
    }

    ScopedVariant converted;
    if (SUCCEEDED(VariantChangeType(converted.Get(), value, 0, VT_BOOL))) {
        *result = converted.Get()->boolVal != VARIANT_FALSE;
        return true;
    }

    return false;
}

void HandleWordDocumentBeforeSaveEvent(const DISPPARAMS* params) {
    ScopedComPtr<IDispatch> document;
    CopyDispatchFromVariant(GetEventArgument(params, 0), document.Put());
    bool saveAsUi = false;
    GetBoolFromVariant(GetEventArgument(params, 1), &saveAsUi);

    ClearTransitionFlushRequest();
    SetFlag(g_runtime.flags.manualSavePending);
    CancelSaveTimer();
    ArmDocumentStateTimer(INPUT_SETTLE_DELAY_MS);

    if (!saveAsUi || !document) {
        return;
    }

    ScopedComPtr<IUnknown> documentIdentity;
    if (SUCCEEDED(GetComIdentity(document.Get(), documentIdentity.Put())) && documentIdentity) {
        ReplaceStoredComPtr(&g_runtime.document.pendingSaveAsDocumentIdentity,
                            documentIdentity.Get());
    }

    g_runtime.timing.pendingSaveAsTime = GetTickCount64();
    SetFlag(g_runtime.flags.pendingSaveAsMigration);
    LogSaveStatus(L"tracking Save As to migrate the current document state");
}

void HandleWordDocumentBeforeCloseEvent(const DISPPARAMS* params) {
    if (!HasPendingSaveWork()) {
        return;
    }

    InvalidateWordUiWindowCache();

    ScopedComPtr<IDispatch> document;
    CopyDispatchFromVariant(GetEventArgument(params, 0), document.Put());
    ScopedBstr documentPath;
    if (document &&
        SUCCEEDED(GetDocumentIdentity(document.Get(), documentPath.Put())) &&
        documentPath.Length() > 0) {
        RequestTransitionFlush(documentPath.CStr(),
                               L"finishing pending changes before Word closes the current document",
                               document.Get());
    } else {
        RequestTransitionFlush(g_runtime.document.observedDocumentPath.Get(),
                               L"finishing pending changes before Word closes the current document");
    }

    if (HasPendingAutosave()) {
        ArmSaveTimer(INPUT_SETTLE_DELAY_MS);
    } else {
        ArmDocumentStateTimer(INPUT_SETTLE_DELAY_MS);
    }
}

void HandleWordDocumentChangeEvent() {
    InvalidateWordUiWindowCache();
    ExpirePendingSaveAsMigrationIfNeeded();
    MaybeKickAutomationRecovery();
    ArmDocumentStateTimer(INPUT_SETTLE_DELAY_MS);
}

void HandleWordWindowDeactivateEvent(const DISPPARAMS* params) {
    if (!HasPendingSaveWork()) {
        return;
    }

    InvalidateWordUiWindowCache();

    ScopedComPtr<IDispatch> document;
    CopyDispatchFromVariant(GetEventArgument(params, 0), document.Put());
    ScopedBstr documentPath;
    if (document &&
        SUCCEEDED(GetDocumentIdentity(document.Get(), documentPath.Put())) &&
        documentPath.Length() > 0) {
        RequestTransitionFlush(documentPath.CStr(),
                               L"finishing pending changes before Word leaves the current document window",
                               document.Get());
    } else {
        RequestTransitionFlush(g_runtime.document.observedDocumentPath.Get(),
                               L"finishing pending changes before Word leaves the current document window");
    }

    if (HasPendingAutosave()) {
        ArmSaveTimer(INPUT_SETTLE_DELAY_MS);
    } else {
        ArmDocumentStateTimer(INPUT_SETTLE_DELAY_MS);
    }
}

enum class WordApplicationEventKind {
    None,
    DocumentBeforeSave,
    DocumentBeforeClose,
    DocumentChange,
    WindowDeactivate,
};

WordApplicationEventKind ClassifyWordApplicationEvent(const WordEventDispIds& dispIds,
                                                      DISPID dispIdMember) {
    if (dispIdMember == dispIds.documentBeforeSave) {
        return WordApplicationEventKind::DocumentBeforeSave;
    }

    if (dispIdMember == dispIds.documentBeforeClose) {
        return WordApplicationEventKind::DocumentBeforeClose;
    }

    if (dispIdMember == dispIds.documentChange) {
        return WordApplicationEventKind::DocumentChange;
    }

    if (dispIdMember == dispIds.windowDeactivate) {
        return WordApplicationEventKind::WindowDeactivate;
    }

    return WordApplicationEventKind::None;
}

void DispatchWordApplicationEvent(WordApplicationEventKind eventKind,
                                  const DISPPARAMS* params) {
    switch (eventKind) {
        case WordApplicationEventKind::DocumentBeforeSave:
            HandleWordDocumentBeforeSaveEvent(params);
            return;

        case WordApplicationEventKind::DocumentBeforeClose:
            HandleWordDocumentBeforeCloseEvent(params);
            return;

        case WordApplicationEventKind::DocumentChange:
            HandleWordDocumentChangeEvent();
            return;

        case WordApplicationEventKind::WindowDeactivate:
            HandleWordWindowDeactivateEvent(params);
            return;

        case WordApplicationEventKind::None:
            return;
    }
}

class WordApplicationEventSink final : public IDispatch {
public:
    explicit WordApplicationEventSink(const WordEventDispIds& dispIds)
        : m_dispIds(dispIds) {
    }

    ~WordApplicationEventSink() = default;

    STDMETHODIMP QueryInterface(REFIID riid, void** object) override {
        if (!object) {
            return E_POINTER;
        }

        *object = nullptr;
        if (riid == IID_IUnknown || riid == IID_IDispatch) {
            *object = static_cast<IDispatch*>(this);
            AddRef();
            return S_OK;
        }

        return E_NOINTERFACE;
    }

    STDMETHODIMP_(ULONG) AddRef() override {
        return static_cast<ULONG>(InterlockedIncrement(&m_refCount));
    }

    STDMETHODIMP_(ULONG) Release() override {
        const ULONG refCount = static_cast<ULONG>(InterlockedDecrement(&m_refCount));
        if (refCount == 0) {
            delete this;
        }

        return refCount;
    }

    STDMETHODIMP GetTypeInfoCount(UINT* pctinfo) override {
        if (pctinfo) {
            *pctinfo = 0;
        }

        return S_OK;
    }

    STDMETHODIMP GetTypeInfo(UINT, LCID, ITypeInfo**) override {
        return E_NOTIMPL;
    }

    STDMETHODIMP GetIDsOfNames(REFIID, LPOLESTR*, UINT, LCID, DISPID*) override {
        return E_NOTIMPL;
    }

    STDMETHODIMP Invoke(DISPID dispIdMember,
                        REFIID,
                        LCID,
                        WORD,
                        DISPPARAMS* params,
                        VARIANT*,
                        EXCEPINFO*,
                        UINT*) override {
        DispatchWordApplicationEvent(ClassifyWordApplicationEvent(m_dispIds, dispIdMember),
                                     params);
        return S_OK;
    }

private:
    volatile LONG m_refCount = 1;
    WordEventDispIds m_dispIds = {};
};

void DisconnectWordEventSession(WordEventSession* session) {
    if (!session) {
        return;
    }

    ScopedComInit comInit;
    if (SUCCEEDED(comInit.GetResult()) &&
        session->IsConnected()) {
        session->connectionPoint->Unadvise(session->cookie);
    }

    session->Reset();
}

void DisconnectWordApplicationEvents() {
    DisconnectWordEventSession(&g_runtime.events.session);
    ClearFlag(g_runtime.flags.wordEventsConnected);
}

HRESULT BuildWordEventSession(IDispatch* application, WordEventSession* session) {
    if (!session) {
        return E_POINTER;
    }

    session->Reset();

    if (!application) {
        return E_POINTER;
    }

    LONG_PTR applicationHwnd = 0;
    if (!DoesApplicationBelongToCurrentProcess(application, &applicationHwnd)) {
        return HRESULT_FROM_WIN32(ERROR_NOT_FOUND);
    }

    WordEventDispIds dispIds;
    HRESULT hr = ResolveWordEventDispIds(application, &dispIds);
    if (FAILED(hr)) {
        return hr;
    }

    ScopedComPtr<IConnectionPointContainer> connectionPointContainer;
    hr = QueryInterfaceTyped(application, connectionPointContainer.Put());
    if (FAILED(hr) || !connectionPointContainer) {
        return FAILED(hr) ? hr : E_FAIL;
    }

    ScopedComPtr<IConnectionPoint> connectionPoint;
    hr = connectionPointContainer->FindConnectionPoint(kDIIDWordApplicationEvents4,
                                                       connectionPoint.Put());
    if (FAILED(hr) || !connectionPoint) {
        return FAILED(hr) ? hr : E_FAIL;
    }

    WordApplicationEventSink* sink = new (std::nothrow) WordApplicationEventSink(dispIds);
    if (!sink) {
        return E_OUTOFMEMORY;
    }

    DWORD cookie = 0;
    hr = connectionPoint->Advise(static_cast<IUnknown*>(sink), &cookie);
    if (FAILED(hr) || cookie == 0) {
        sink->Release();
        return FAILED(hr) ? hr : E_FAIL;
    }

    ReplaceStoredComPtr(&session->application, application);
    session->applicationHwnd = applicationHwnd;
    session->connectionPoint.Reset(connectionPoint.Detach());
    session->sink.Reset(sink);
    session->cookie = cookie;
    return S_OK;
}

void CommitWordEventSession(WordEventSession* session) {
    if (!session) {
        return;
    }

    DisconnectWordApplicationEvents();
    g_runtime.events.session = std::move(*session);
    SetFlag(g_runtime.flags.wordEventsConnected);
}

HRESULT ConnectWordApplicationEvents(IDispatch* application) {
    WordEventSession session;
    const HRESULT hr = BuildWordEventSession(application, &session);
    if (FAILED(hr)) {
        return hr;
    }

    CommitWordEventSession(&session);
    return S_OK;
}

bool CanReuseConnectedWordEventState(bool forceReconnect,
                                     bool wordEventsConnected,
                                     bool cacheValid) {
    return !forceReconnect && wordEventsConnected && cacheValid;
}

bool ShouldThrottleWordEventReconnect(bool forceReconnect,
                                      bool wordEventsConnected,
                                      ULONGLONG lastAttemptTime,
                                      ULONGLONG now) {
    return !forceReconnect &&
           !wordEventsConnected &&
           lastAttemptTime != 0 &&
           now - lastAttemptTime < WORD_EVENT_RECONNECT_INTERVAL_MS;
}

bool ShouldReuseWordEventConnectionForHwnd(bool forceReconnect,
                                           bool wordEventsConnected,
                                           LONG_PTR connectedApplicationHwnd,
                                           LONG_PTR resolvedApplicationHwnd) {
    return !forceReconnect &&
           wordEventsConnected &&
           connectedApplicationHwnd != 0 &&
           connectedApplicationHwnd == resolvedApplicationHwnd;
}

enum class WordEventConnectionOutcome {
    Reused,
    Connected,
    Unavailable,
    Failed,
};

struct WordEventConnectionResult {
    WordEventConnectionOutcome outcome = WordEventConnectionOutcome::Unavailable;
    HRESULT hr = S_OK;
};

HRESULT ResolveWordApplicationForEventConnection(ScopedComPtr<IDispatch>* application,
                                                 LONG_PTR* applicationHwnd) {
    if (!application || !applicationHwnd) {
        return E_POINTER;
    }

    *applicationHwnd = 0;

    HRESULT hr = GetWordApplication(application->Put());
    if (FAILED(hr) || !application->Get()) {
        return FAILED(hr) ? hr : E_FAIL;
    }

    if (!DoesApplicationBelongToCurrentProcess(application->Get(), applicationHwnd)) {
        return HRESULT_FROM_WIN32(ERROR_NOT_FOUND);
    }

    return S_OK;
}

WordEventConnectionResult EnsureWordEventSessionConnected(bool forceReconnect) {
    WordEventConnectionResult result = {};

    if (!IsOwnerThreadSchedulerContextValid()) {
        return result;
    }

    const bool wordEventsConnected =
        LoadFlag(g_runtime.flags.wordEventsConnected);
    LONG_PTR cachedApplicationHwnd = 0;
    const bool cacheValid =
        wordEventsConnected &&
        IsWordEventSessionApplicationCacheValid(g_runtime.events.session,
                                                &cachedApplicationHwnd);
    if (CanReuseConnectedWordEventState(forceReconnect,
                                        wordEventsConnected,
                                        cacheValid)) {
        g_runtime.events.session.applicationHwnd = cachedApplicationHwnd;
        result.outcome = WordEventConnectionOutcome::Reused;
        return result;
    }

    const ULONGLONG now = GetTickCount64();
    if (ShouldThrottleWordEventReconnect(forceReconnect,
                                         wordEventsConnected,
                                         g_runtime.timing.lastEventConnectAttemptTime,
                                         now)) {
        return result;
    }

    g_runtime.timing.lastEventConnectAttemptTime = now;

    if (!forceReconnect && wordEventsConnected && !cacheValid) {
        DisconnectWordApplicationEvents();
    }

    ScopedComInit comInit;
    if (FAILED(comInit.GetResult())) {
        return result;
    }

    ScopedComPtr<IDispatch> application;
    LONG_PTR applicationHwnd = 0;
    HRESULT hr =
        ResolveWordApplicationForEventConnection(&application, &applicationHwnd);
    if (FAILED(hr) || !application) {
        if (forceReconnect) {
            DisconnectWordApplicationEvents();
        }
        result.hr = FAILED(hr) ? hr : E_FAIL;
        return result;
    }

    if (ShouldReuseWordEventConnectionForHwnd(forceReconnect,
                                              wordEventsConnected,
                                              g_runtime.events.session.applicationHwnd,
                                              applicationHwnd)) {
        result.outcome = WordEventConnectionOutcome::Reused;
        return result;
    }

    hr = ConnectWordApplicationEvents(application.Get());
    if (FAILED(hr)) {
        DisconnectWordApplicationEvents();
        result.outcome = WordEventConnectionOutcome::Failed;
        result.hr = hr;
        return result;
    }

    result.outcome = WordEventConnectionOutcome::Connected;
    return result;
}

bool EnsureWordApplicationEventsConnected(bool forceReconnect) {
    const WordEventConnectionResult result =
        EnsureWordEventSessionConnected(forceReconnect);
    if (result.outcome == WordEventConnectionOutcome::Connected) {
        LogDocumentStateStatus(L"connected to native Word application events");
        return true;
    }

    if (result.outcome == WordEventConnectionOutcome::Reused) {
        return true;
    }

    if (result.outcome == WordEventConnectionOutcome::Failed &&
        ShouldLogFailureNow(&g_runtime.status.lastDocumentStateFailureLogTime)) {
        Wh_Log(L"Document state monitor: failed to connect Word application events, hr=0x%08X",
               result.hr);
    }

    return false;
}

// ============================================================================
// Save Logic
// ============================================================================

enum class SaveAttemptResult {
    Saved,
    AlreadyClean,
    Cleared,
    Deferred,
    RetryLater,
};

enum class DocumentDirtyState {
    Clean,
    Dirty,
    RetryLater,
};

enum class SnapshotLogContext {
    Save,
    DocumentState,
};

enum class SnapshotQueryResult {
    Ready,
    Cleared,
    RetryLater,
};

enum class SnapshotSource {
    Active,
    SpecificPath,
};

enum class SnapshotClearedReason {
    None,
    NoDocument,
    ReadOnly,
    NoPath,
    MissingByPath,
    ProtectedView,
};

struct ActiveDocumentSnapshot {
    ScopedComPtr<IDispatch> document;
    ScopedComPtr<IUnknown> identity;
    ScopedBstr path;
    SnapshotClearedReason clearedReason = SnapshotClearedReason::None;
    bool readOnly = false;
    bool saved = true;
    bool hasPath = false;

    void Reset() {
        document.Reset();
        identity.Reset();
        path.Reset();
        clearedReason = SnapshotClearedReason::None;
        readOnly = false;
        saved = true;
        hasPath = false;
    }
};

const wchar_t* DescribeSnapshotClearedReason(SnapshotClearedReason reason) {
    switch (reason) {
        case SnapshotClearedReason::NoDocument:
            return L"no active document is available";
        case SnapshotClearedReason::ReadOnly:
            return L"skipping save for a read-only document";
        case SnapshotClearedReason::NoPath:
            return L"skipping save for a document that has not been saved yet";
        case SnapshotClearedReason::MissingByPath:
            return L"the previously edited document is no longer available";
        case SnapshotClearedReason::ProtectedView:
            return L"skipping save while Word is in Protected View";
        case SnapshotClearedReason::None:
            break;
    }

    return L"no document is available for saving";
}

void LogSnapshotFailure(SnapshotLogContext context, const wchar_t* message, HRESULT hr) {
    ULONGLONG* lastLogTime = context == SnapshotLogContext::Save
                                 ? &g_runtime.status.lastSaveFailureLogTime
                                 : &g_runtime.status.lastDocumentStateFailureLogTime;
    if (!ShouldLogFailureNow(lastLogTime)) {
        return;
    }

    if (context == SnapshotLogContext::Save) {
        Wh_Log(L"Auto-save: %ls, hr=0x%08X", message, hr);
    } else {
        Wh_Log(L"Document state monitor: %ls, hr=0x%08X", message, hr);
    }
}

struct SnapshotRequest {
    SnapshotSource source = SnapshotSource::Active;
    const wchar_t* path = nullptr;
    SnapshotLogContext context = SnapshotLogContext::Save;
};

enum class SnapshotMetadataMode {
    WhenDirty,
    Always,
};

struct SnapshotLoadPlan {
    SnapshotRequest request = {};
    SnapshotMetadataMode metadataMode = SnapshotMetadataMode::Always;
};

SnapshotQueryResult PopulateSnapshotSavedState(ActiveDocumentSnapshot* snapshot,
                                              SnapshotLogContext context) {
    if (!snapshot || !snapshot->document) {
        return SnapshotQueryResult::Cleared;
    }

    const HRESULT hr = GetBoolProperty(snapshot->document.Get(), L"Saved", &snapshot->saved);
    if (FAILED(hr)) {
        LogSnapshotFailure(context, L"failed to query Saved state", hr);
        return SnapshotQueryResult::RetryLater;
    }

    return SnapshotQueryResult::Ready;
}

SnapshotQueryResult PopulateSnapshotMetadata(ActiveDocumentSnapshot* snapshot,
                                            SnapshotLogContext context) {
    if (!snapshot || !snapshot->document) {
        return SnapshotQueryResult::Cleared;
    }

    HRESULT hr = GetComIdentity(snapshot->document.Get(), snapshot->identity.Put());
    if (FAILED(hr) || !snapshot->identity) {
        LogSnapshotFailure(context, L"failed to resolve document COM identity", hr);
        return SnapshotQueryResult::RetryLater;
    }

    hr = GetBoolProperty(snapshot->document.Get(), L"ReadOnly", &snapshot->readOnly);
    if (FAILED(hr)) {
        LogSnapshotFailure(context, L"failed to query ReadOnly", hr);
        return SnapshotQueryResult::RetryLater;
    }

    if (snapshot->readOnly) {
        snapshot->clearedReason = SnapshotClearedReason::ReadOnly;
        return SnapshotQueryResult::Cleared;
    }

    hr = GetDocumentIdentityAndPathState(snapshot->document.Get(),
                                         snapshot->path.Put(),
                                         &snapshot->hasPath);
    if (FAILED(hr)) {
        LogSnapshotFailure(context, L"failed to query FullName/Name identity", hr);
        return SnapshotQueryResult::RetryLater;
    }

    if (!snapshot->hasPath || hr == S_FALSE) {
        snapshot->clearedReason = SnapshotClearedReason::NoPath;
        return SnapshotQueryResult::Cleared;
    }

    return SnapshotQueryResult::Ready;
}

bool ShouldPopulateSnapshotMetadataForMode(bool saved, SnapshotMetadataMode metadataMode) {
    switch (metadataMode) {
        case SnapshotMetadataMode::WhenDirty:
            return !saved;

        case SnapshotMetadataMode::Always:
            return true;
    }

    return true;
}

SnapshotQueryResult ResolveSpecificSnapshotDocument(const SnapshotRequest& request,
                                                    ActiveDocumentSnapshot* snapshot) {
    if (!snapshot) {
        return SnapshotQueryResult::Cleared;
    }

    const HRESULT hr = GetWordDocumentByPath(request.path, snapshot->document.Put());
    if (FAILED(hr) || !snapshot->document) {
        if (IsRetryableAutomationFailure(hr)) {
            return SnapshotQueryResult::RetryLater;
        }

        snapshot->clearedReason = SnapshotClearedReason::MissingByPath;
        return SnapshotQueryResult::Cleared;
    }

    return SnapshotQueryResult::Ready;
}

SnapshotQueryResult EnsureSnapshotApplication(ScopedComPtr<IDispatch>* application,
                                              SnapshotLogContext context) {
    if (!application) {
        return SnapshotQueryResult::Cleared;
    }

    HRESULT hr = GetWordApplication(application->Put());
    if (FAILED(hr) || !application->Get()) {
        if (IsRetryableAutomationFailure(hr)) {
            return SnapshotQueryResult::RetryLater;
        }

        LogSnapshotFailure(context, L"failed to get Word application", hr);
        return SnapshotQueryResult::RetryLater;
    }

    return SnapshotQueryResult::Ready;
}

SnapshotQueryResult ResolveActiveSnapshotDocumentFromApplication(IDispatch* application,
                                                                ActiveDocumentSnapshot* snapshot) {
    if (!application || !snapshot) {
        return SnapshotQueryResult::Cleared;
    }

    const HRESULT hr =
        GetDispatchProperty(application, L"ActiveDocument", snapshot->document.Put());
    if (FAILED(hr) || !snapshot->document) {
        return IsRetryableAutomationFailure(hr) ? SnapshotQueryResult::RetryLater
                                                : SnapshotQueryResult::Cleared;
    }

    return SnapshotQueryResult::Ready;
}

SnapshotQueryResult FinalizeMissingActiveSnapshotDocument(IDispatch* application,
                                                          ActiveDocumentSnapshot* snapshot) {
    if (!snapshot) {
        return SnapshotQueryResult::Cleared;
    }

    if (application) {
        ScopedComPtr<IDispatch> protectedViewWindow;
        if (SUCCEEDED(GetDispatchProperty(application,
                                          L"ActiveProtectedViewWindow",
                                          protectedViewWindow.Put())) &&
            protectedViewWindow) {
            snapshot->clearedReason = SnapshotClearedReason::ProtectedView;
            return SnapshotQueryResult::Cleared;
        }
    }

    snapshot->clearedReason = SnapshotClearedReason::NoDocument;
    return SnapshotQueryResult::Cleared;
}

SnapshotQueryResult ResolveSnapshotDocument(const SnapshotRequest& request,
                                           ActiveDocumentSnapshot* snapshot) {
    if (!snapshot) {
        return SnapshotQueryResult::Cleared;
    }

    snapshot->Reset();

    if (request.source == SnapshotSource::SpecificPath) {
        return ResolveSpecificSnapshotDocument(request, snapshot);
    }

    ScopedComPtr<IDispatch> application;
    HRESULT hr = GetWordDocumentFromActiveWindow(snapshot->document.Put());
    if (SUCCEEDED(hr) && snapshot->document) {
        return SnapshotQueryResult::Ready;
    }

    if (IsRetryableAutomationFailure(hr)) {
        return SnapshotQueryResult::RetryLater;
    }

    const SnapshotQueryResult applicationResult =
        EnsureSnapshotApplication(&application, request.context);
    if (applicationResult != SnapshotQueryResult::Ready) {
        return applicationResult;
    }

    const SnapshotQueryResult activeDocumentResult =
        ResolveActiveSnapshotDocumentFromApplication(application.Get(), snapshot);
    if (activeDocumentResult != SnapshotQueryResult::Cleared) {
        return activeDocumentResult;
    }

    return FinalizeMissingActiveSnapshotDocument(application.Get(), snapshot);
}

SnapshotQueryResult ResolveSnapshotDocumentFromRequest(const SnapshotRequest& request,
                                                       ActiveDocumentSnapshot* snapshot) {
    if (!snapshot) {
        return SnapshotQueryResult::Cleared;
    }

    if (request.source == SnapshotSource::SpecificPath &&
        request.path &&
        *request.path &&
        IsTransitionFlushDocumentCacheValid(request.path)) {
        snapshot->Reset();
        g_runtime.document.transitionFlushDocument.Get()->AddRef();
        snapshot->document.Reset(g_runtime.document.transitionFlushDocument.Get());
        return SnapshotQueryResult::Ready;
    }

    return ResolveSnapshotDocument(request, snapshot);
}

SnapshotQueryResult ExecuteSnapshotLoadPlan(const SnapshotLoadPlan& plan,
                                           ActiveDocumentSnapshot* snapshot) {
    const SnapshotQueryResult documentResult =
        ResolveSnapshotDocumentFromRequest(plan.request, snapshot);
    if (documentResult != SnapshotQueryResult::Ready) {
        return documentResult;
    }

    const bool usingCachedSpecificDocument =
        plan.request.source == SnapshotSource::SpecificPath &&
        plan.request.path &&
        *plan.request.path &&
        snapshot &&
        snapshot->document &&
        g_runtime.document.transitionFlushDocument &&
        snapshot->document.Get() == g_runtime.document.transitionFlushDocument.Get();
    const SnapshotQueryResult savedStateResult =
        PopulateSnapshotSavedState(snapshot, plan.request.context);
    if (savedStateResult != SnapshotQueryResult::Ready) {
        if (usingCachedSpecificDocument) {
            InvalidateTransitionFlushDocumentCache();
        }

        return savedStateResult;
    }

    if (!ShouldPopulateSnapshotMetadataForMode(snapshot->saved, plan.metadataMode)) {
        return SnapshotQueryResult::Ready;
    }

    const SnapshotQueryResult metadataResult =
        PopulateSnapshotMetadata(snapshot, plan.request.context);
    if (metadataResult != SnapshotQueryResult::Ready &&
        usingCachedSpecificDocument) {
        InvalidateTransitionFlushDocumentCache();
    }

    if (metadataResult == SnapshotQueryResult::Ready &&
        usingCachedSpecificDocument &&
        (!snapshot->hasPath ||
         !AreSameDocumentPath(snapshot->path.CStr(), plan.request.path))) {
        snapshot->Reset();
        InvalidateTransitionFlushDocumentCache();
        return ExecuteSnapshotLoadPlan(plan, snapshot);
    }

    return metadataResult;
}

SnapshotLoadPlan MakeDocumentStateSnapshotLoadPlan(bool requireCleanSnapshotDetails) {
    SnapshotLoadPlan plan = {};
    plan.request.source = SnapshotSource::Active;
    plan.request.context = SnapshotLogContext::DocumentState;
    plan.metadataMode = requireCleanSnapshotDetails ? SnapshotMetadataMode::Always
                                                    : SnapshotMetadataMode::WhenDirty;
    return plan;
}

SnapshotLoadPlan MakeSaveSnapshotLoadPlan(const wchar_t* specificPath) {
    SnapshotLoadPlan plan = {};
    plan.request.source =
        (specificPath && *specificPath) ? SnapshotSource::SpecificPath : SnapshotSource::Active;
    plan.request.path = specificPath;
    plan.request.context = SnapshotLogContext::Save;
    plan.metadataMode = SnapshotMetadataMode::Always;
    return plan;
}

void ExpirePendingSaveAsMigrationIfNeeded() {
    if (!HasPendingSaveAsMigration() || g_runtime.timing.pendingSaveAsTime == 0) {
        return;
    }

    const ULONGLONG now = GetTickCount64();
    if (now - g_runtime.timing.pendingSaveAsTime > SAVE_AS_MIGRATION_TIMEOUT_MS) {
        ClearPendingSaveAsMigration();
    }
}

bool SetObservedDocumentIdentity(IUnknown* identity) {
    return ReplaceStoredComPtr(&g_runtime.document.observedDocumentIdentity, identity);
}

bool SetObservedDocumentFromSnapshot(const ActiveDocumentSnapshot* snapshot, bool dirty) {
    if (!snapshot || !snapshot->hasPath || !snapshot->path.Length() || !snapshot->identity) {
        ResetObservedDocumentState();
        return false;
    }

    if (!SetObservedDocumentPath(snapshot->path.CStr()) ||
        !SetObservedDocumentIdentity(snapshot->identity.Get())) {
        ResetObservedDocumentState();
        return false;
    }

    SetFlag(g_runtime.flags.documentDirtyKnown);
    StoreFlag(g_runtime.flags.documentDirty, dirty);
    return true;
}

void MarkObservedDocumentClean(const ActiveDocumentSnapshot* snapshot) {
    SetObservedDocumentFromSnapshot(snapshot, false);
}

bool NoteObservedDocumentDirty(const ActiveDocumentSnapshot* snapshot) {
    if (!snapshot || !snapshot->hasPath || !snapshot->path.Length() || !snapshot->identity) {
        ResetObservedDocumentState();
        return false;
    }

    const bool pathChanged =
        !AreSameDocumentPath(g_runtime.document.observedDocumentPath.Get(), snapshot->path.CStr());
    const bool identityChanged =
        !AreSameComIdentity(g_runtime.document.observedDocumentIdentity.Get(),
                            snapshot->identity.Get());
    const bool wasKnown = LoadFlag(g_runtime.flags.documentDirtyKnown);
    const bool wasDirty = LoadFlag(g_runtime.flags.documentDirty);
    if (!SetObservedDocumentFromSnapshot(snapshot, true)) {
        return true;
    }

    return !wasKnown || !wasDirty || pathChanged || identityChanged;
}

bool IsObservedDocumentSnapshot(const ActiveDocumentSnapshot* snapshot) {
    return snapshot && snapshot->identity && AreSameComIdentity(snapshot->identity.Get(),
                                                               g_runtime.document.observedDocumentIdentity.Get());
}

bool IsPendingSaveAsSnapshot(const ActiveDocumentSnapshot* snapshot) {
    return snapshot && snapshot->identity &&
           AreSameComIdentity(snapshot->identity.Get(),
                              g_runtime.document.pendingSaveAsDocumentIdentity.Get());
}

bool TryMigrateObservedDocumentIdentity(const ActiveDocumentSnapshot* snapshot,
                                        bool clearPendingAutosaveOnClean) {
    if (!snapshot || !snapshot->hasPath || !snapshot->path.Length() || !snapshot->identity) {
        return false;
    }

    ExpirePendingSaveAsMigrationIfNeeded();

    const bool observedIdentityMatch = IsObservedDocumentSnapshot(snapshot);
    const bool saveAsIdentityMatch = IsPendingSaveAsSnapshot(snapshot);
    if (!observedIdentityMatch && !saveAsIdentityMatch) {
        return false;
    }

    const bool pathChanged =
        !AreSameDocumentPath(g_runtime.document.observedDocumentPath.Get(), snapshot->path.CStr());
    if (!pathChanged) {
        if (saveAsIdentityMatch) {
            ClearPendingSaveAsMigration();
        }
        return false;
    }

    const bool wasDirty = LoadFlag(g_runtime.flags.documentDirty);
    SetObservedDocumentFromSnapshot(snapshot, !snapshot->saved && wasDirty);

    if (snapshot->saved) {
        g_runtime.timing.lastSaveTime = GetTickCount64();
        ClearManualSavePending();
        ClearTransitionFlushRequest();
        if (clearPendingAutosaveOnClean) {
            ClearPendingSave();
        }
    }

    ClearPendingSaveAsMigration();
    LogDocumentStateStatus(L"migrated the tracked document after Save As or rename");
    return true;
}

bool ShouldTransitionFlushObservedDocument(const ActiveDocumentSnapshot* snapshot) {
    return snapshot &&
           snapshot->hasPath &&
           g_runtime.document.observedDocumentPath.Length() != 0 &&
           !AreSameDocumentPath(snapshot->path.CStr(), g_runtime.document.observedDocumentPath.Get()) &&
           HasPendingAutosave();
}

void BeginTransitionFlushForObservedDocument() {
    RequestTransitionFlush(g_runtime.document.observedDocumentPath.Get(),
                           L"finishing the previous document before switching to another one");
    ArmSaveTimer(INPUT_SETTLE_DELAY_MS);
    ArmDocumentStateTimer(DOCUMENT_STATE_ACTIVE_POLL_INTERVAL_MS);
}

void SyncObservedDocumentCleanState(const ActiveDocumentSnapshot* snapshot) {
    if (snapshot && snapshot->hasPath) {
        MarkObservedDocumentClean(snapshot);
    } else {
        ResetObservedDocumentState();
    }
}

void FinalizeSaveAttemptState(const ActiveDocumentSnapshot* snapshot,
                              bool clearPendingSave,
                              bool clearTransitionRequest) {
    if (clearTransitionRequest) {
        ClearTransitionFlushRequest();
    }

    ClearManualSavePending();
    ClearPendingSaveAsMigration();
    SyncObservedDocumentCleanState(snapshot);
    if (clearPendingSave) {
        ClearPendingSave();
    }
}

void NoteDocumentStateRefreshReady() {
    g_runtime.timing.documentStateRetryDelayMs = MIN_RETRY_INTERVAL_MS;
    ClearAutomationBusyPending();
}

void NoteSaveOperationReady(ULONGLONG now) {
    g_runtime.timing.saveRetryDelayMs = MIN_RETRY_INTERVAL_MS;
    ClearAutomationBusyPending();
    g_runtime.timing.lastSaveTime = now;
}

enum class DocumentObservationApplyResult {
    None,
    RearmSteadyPoll,
    StartedTransitionFlush,
    ScheduledSave,
};

DocumentObservationApplyResult ApplyDirtyDocumentObservation(
    const ActiveDocumentSnapshot* snapshot) {
    if (TryMigrateObservedDocumentIdentity(snapshot, false)) {
        return DocumentObservationApplyResult::RearmSteadyPoll;
    }

    if (ShouldTransitionFlushObservedDocument(snapshot)) {
        BeginTransitionFlushForObservedDocument();
        return DocumentObservationApplyResult::StartedTransitionFlush;
    }

    if (snapshot && snapshot->hasPath && NoteObservedDocumentDirty(snapshot)) {
        LogDocumentStateStatus(L"detected non-keyboard document change");
        ScheduleSaveFromEdit();
        return DocumentObservationApplyResult::ScheduledSave;
    }

    return DocumentObservationApplyResult::None;
}

DocumentObservationApplyResult ApplyCleanDocumentObservation(
    const ActiveDocumentSnapshot* snapshot,
    const RuntimeTickSnapshot& tick) {
    if (TryMigrateObservedDocumentIdentity(snapshot, true)) {
        return DocumentObservationApplyResult::RearmSteadyPoll;
    }

    if (ShouldTransitionFlushObservedDocument(snapshot)) {
        BeginTransitionFlushForObservedDocument();
        return DocumentObservationApplyResult::StartedTransitionFlush;
    }

    if (snapshot && snapshot->hasPath) {
        const bool sameObservedDocument =
            IsObservedDocumentSnapshot(snapshot) ||
            AreSameDocumentPath(g_runtime.document.observedDocumentPath.Get(),
                                snapshot->path.CStr());
        if (tick.runtime.manualSavePending) {
            ClearManualSavePending();
            g_runtime.timing.lastSaveTime = tick.now;
        }

        SyncObservedDocumentCleanState(snapshot);
        if (sameObservedDocument && tick.runtime.pendingSave) {
            ClearPendingSave();
            LogSaveStatus(L"pending changes were already saved by Word");
        }
    } else {
        ClearManualSavePending();
        ResetObservedDocumentState();
    }

    return DocumentObservationApplyResult::None;
}

DocumentDirtyState InterpretDocumentStateSnapshotQueryResult(
    const ActiveDocumentSnapshot* snapshot,
    SnapshotQueryResult snapshotResult) {
    switch (snapshotResult) {
        case SnapshotQueryResult::Ready:
            return snapshot && !snapshot->saved ? DocumentDirtyState::Dirty
                                                : DocumentDirtyState::Clean;

        case SnapshotQueryResult::Cleared:
            return DocumentDirtyState::Clean;

        case SnapshotQueryResult::RetryLater:
            return DocumentDirtyState::RetryLater;
    }

    return DocumentDirtyState::RetryLater;
}

DocumentDirtyState QueryActiveDocumentDirtyState(ActiveDocumentSnapshot* snapshot,
                                                 bool requireCleanSnapshotDetails = false) {
    ScopedComInit comInit;
    if (FAILED(comInit.GetResult())) {
        LogSnapshotFailure(SnapshotLogContext::DocumentState,
                           L"CoInitializeEx failed",
                           comInit.GetResult());
        return DocumentDirtyState::RetryLater;
    }

    ActiveDocumentSnapshot localSnapshot;
    if (!snapshot) {
        snapshot = &localSnapshot;
    }

    const SnapshotLoadPlan plan =
        MakeDocumentStateSnapshotLoadPlan(requireCleanSnapshotDetails);
    return InterpretDocumentStateSnapshotQueryResult(
        snapshot,
        ExecuteSnapshotLoadPlan(plan, snapshot));
}

SaveAttemptResult InterpretSaveSnapshotQueryResult(const wchar_t* specificPath,
                                                   const ActiveDocumentSnapshot* snapshot,
                                                   SnapshotQueryResult snapshotResult) {
    switch (snapshotResult) {
        case SnapshotQueryResult::Cleared:
            if ((!specificPath || !*specificPath) &&
                snapshot &&
                snapshot->clearedReason == SnapshotClearedReason::ProtectedView) {
                return SaveAttemptResult::Deferred;
            }
            return SaveAttemptResult::Cleared;

        case SnapshotQueryResult::RetryLater:
            return SaveAttemptResult::RetryLater;

        case SnapshotQueryResult::Ready:
            return SaveAttemptResult::Saved;
    }

    return SaveAttemptResult::Cleared;
}

SaveAttemptResult TrySaveActiveDocument(ActiveDocumentSnapshot* snapshot,
                                        const wchar_t* specificPath = nullptr) {
    ScopedComInit comInit;
    if (FAILED(comInit.GetResult())) {
        LogSnapshotFailure(SnapshotLogContext::Save, L"CoInitializeEx failed", comInit.GetResult());
        return SaveAttemptResult::RetryLater;
    }

    ActiveDocumentSnapshot localSnapshot;
    if (!snapshot) {
        snapshot = &localSnapshot;
    }

    const SnapshotLoadPlan plan = MakeSaveSnapshotLoadPlan(specificPath);
    const SnapshotQueryResult snapshotResult = ExecuteSnapshotLoadPlan(plan, snapshot);
    const SaveAttemptResult snapshotInterpretation =
        InterpretSaveSnapshotQueryResult(specificPath, snapshot, snapshotResult);
    if (snapshotInterpretation != SaveAttemptResult::Saved) {
        return snapshotInterpretation;
    }

    if (snapshot->saved) {
        return SaveAttemptResult::AlreadyClean;
    }

    const HRESULT hr = InvokeDispatch(snapshot->document.Get(),
                                      DISPATCH_METHOD,
                                      const_cast<LPOLESTR>(L"Save"));
    if (SUCCEEDED(hr)) {
        return SaveAttemptResult::Saved;
    }

    if (IsRetryableAutomationFailure(hr)) {
        return SaveAttemptResult::RetryLater;
    }

    LogSnapshotFailure(SnapshotLogContext::Save, L"document save failed", hr);
    return SaveAttemptResult::Cleared;
}

void CALLBACK SchedulerTimerProc(HWND, UINT, UINT_PTR idEvent, DWORD) {
    KillTimer(nullptr, idEvent);
    g_runtime.timing.schedulerTimerId = 0;
    g_runtime.timing.schedulerTimerDueTime = 0;

    if (!CanRunOwnerThreadRuntimeWork()) {
        return;
    }

    const ULONGLONG now = GetTickCount64();
    const bool documentStateDue = IsScheduledTaskDue(ScheduledTaskKind::DocumentState, now);
    const bool saveDue = IsScheduledTaskDue(ScheduledTaskKind::Save, now);

    if (documentStateDue) {
        g_runtime.timing.documentStateTimerDueTime = 0;
        HandleDocumentStateTick();
    }

    const ULONGLONG postDocumentStateNow = GetTickCount64();
    if (saveDue &&
        CanRunOwnerThreadRuntimeWork() &&
        IsScheduledTaskDue(ScheduledTaskKind::Save, postDocumentStateNow)) {
        g_runtime.timing.saveTimerDueTime = 0;
        HandleAutosaveTick();
    }

    if (CanRunOwnerThreadRuntimeWork()) {
        RefreshSchedulerTimer();
    }
}

bool ArmSaveTimer(DWORD delayMs) {
    return ScheduleTask(ScheduledTaskKind::Save, delayMs);
}

void RefreshTickWordEventState(RuntimeTickSnapshot* tick) {
    if (!tick) {
        return;
    }

    if (!tick->runtime.wordEventsConnected &&
        tick->runtime.activeWordDocumentWindow &&
        EnsureWordApplicationEventsConnected()) {
        tick->runtime.wordEventsConnected = true;
        tick->flags.wordEventsConnected = true;
        tick->steadyPollDelay = GetSteadyDocumentStatePollDelay(&tick->flags);
    }
}

bool HandleDocumentObservationApplyOutcome(DocumentObservationApplyResult result,
                                           DWORD steadyPollDelay) {
    switch (result) {
        case DocumentObservationApplyResult::RearmSteadyPoll:
            ArmDocumentStateTimer(steadyPollDelay);
            return true;

        case DocumentObservationApplyResult::StartedTransitionFlush:
        case DocumentObservationApplyResult::ScheduledSave:
            return true;

        case DocumentObservationApplyResult::None:
            return false;
    }

    return false;
}

bool HandleDocumentStateTickDecisionOutcome(const RuntimeTickSnapshot& tick,
                                            const TickDecision& decision) {
    if (decision.action == TickDecisionAction::None) {
        return true;
    }

    if (decision.action != TickDecisionAction::RearmDocumentStateTimer) {
        return false;
    }

    if (tick.runtime.pauseReason != WordUiPauseReason::None) {
        LogDocumentStateStatus(DescribeWordUiPauseReason(tick.runtime.pauseReason));
    }

    ArmDocumentStateTimer(decision.delayMs);
    return true;
}

bool HandleDocumentStateRefreshResult(DocumentDirtyState dirtyState,
                                      ActiveDocumentSnapshot* snapshot,
                                      const RuntimeTickSnapshot& tick) {
    switch (dirtyState) {
        case DocumentDirtyState::Dirty:
            NoteDocumentStateRefreshReady();
            return HandleDocumentObservationApplyOutcome(
                ApplyDirtyDocumentObservation(snapshot),
                tick.steadyPollDelay);

        case DocumentDirtyState::Clean:
            NoteDocumentStateRefreshReady();
            return HandleDocumentObservationApplyOutcome(
                ApplyCleanDocumentObservation(snapshot, tick),
                tick.steadyPollDelay);

        case DocumentDirtyState::RetryLater:
            NoteAutomationBusy(L"Word automation is busy, waiting to retry document-state refresh");
            ArmDocumentStateTimer(
                AdvanceRetryDelay(&g_runtime.timing.documentStateRetryDelayMs,
                                  MAX_DOCUMENT_STATE_RETRY_INTERVAL_MS));
            return true;
    }

    return false;
}

bool HandleAutosaveTickDecisionOutcome(const RuntimeTickSnapshot& tick,
                                       const TickDecision& decision) {
    if (decision.action == TickDecisionAction::None) {
        return true;
    }

    if (decision.action != TickDecisionAction::RearmSaveTimer) {
        return false;
    }

    if (tick.runtime.pauseReason != WordUiPauseReason::None) {
        LogSaveStatus(DescribeWordUiPauseReason(tick.runtime.pauseReason));
    }

    ArmSaveTimer(decision.delayMs);
    return true;
}

void HandleAutosaveAttemptResult(SaveAttemptResult result,
                                 const ActiveDocumentSnapshot* snapshot,
                                 const RuntimeTickSnapshot& tick) {
    switch (result) {
        case SaveAttemptResult::Saved:
            NoteSaveOperationReady(tick.now);
            if (snapshot->hasPath) {
                TryMigrateObservedDocumentIdentity(snapshot, false);
            }
            FinalizeSaveAttemptState(snapshot, true, true);
            if (tick.runtime.transitionFlushRequested) {
                LogSaveStatus(L"flushed pending changes for the previous document");
            } else {
                LogSaveStatus(L"pending changes were saved");
            }
            return;

        case SaveAttemptResult::AlreadyClean:
            NoteSaveOperationReady(tick.now);
            if (snapshot->hasPath && TryMigrateObservedDocumentIdentity(snapshot, true)) {
                return;
            }
            FinalizeSaveAttemptState(snapshot, true, true);
            LogSaveStatus(tick.runtime.transitionFlushRequested
                              ? L"the previous document was already clean"
                              : L"pending changes were already saved");
            return;

        case SaveAttemptResult::Cleared:
            g_runtime.timing.saveRetryDelayMs = MIN_RETRY_INTERVAL_MS;
            ClearAutomationBusyPending();
            if (snapshot->hasPath && TryMigrateObservedDocumentIdentity(snapshot, true)) {
                return;
            }
            if (snapshot->clearedReason != SnapshotClearedReason::None) {
                LogSaveStatus(DescribeSnapshotClearedReason(snapshot->clearedReason));
            }
            FinalizeSaveAttemptState(snapshot, true, true);
            return;

        case SaveAttemptResult::Deferred:
            LogSaveStatus(DescribeSnapshotClearedReason(snapshot->clearedReason));
            ArmSaveTimer(DOCUMENT_STATE_ACTIVE_POLL_INTERVAL_MS);
            return;

        case SaveAttemptResult::RetryLater:
            NoteAutomationBusy(L"Word automation is busy, waiting to retry auto-save");
            ArmSaveTimer(
                AdvanceRetryDelay(&g_runtime.timing.saveRetryDelayMs,
                                  MAX_SAVE_RETRY_INTERVAL_MS));
            return;
    }
}

void HandleDocumentStateTick() {
    ExpirePendingSaveAsMigrationIfNeeded();
    RuntimeTickSnapshot tick = CaptureDocumentStateTickSnapshot();
    RefreshTickWordEventState(&tick);

    const TickDecision decision = EvaluateDocumentStateTickDecision(tick);
    if (HandleDocumentStateTickDecisionOutcome(tick, decision)) {
        return;
    }

    if (decision.action != TickDecisionAction::RefreshDocumentState) {
        return;
    }

    MaybeKickAutomationRecovery(tick.runtime.inputBusy);

    ActiveDocumentSnapshot snapshot;
    const bool requireCleanSnapshotDetails =
        ShouldRequireCleanSnapshotDetails(tick.runtime.manualSavePending,
                                          tick.runtime.pendingSave,
                                          tick.pendingSaveAsMigration,
                                          tick.documentDirtyKnown);
    if (HandleDocumentStateRefreshResult(
            QueryActiveDocumentDirtyState(&snapshot, requireCleanSnapshotDetails),
            &snapshot,
            tick)) {
        return;
    }

    ArmDocumentStateTimer(tick.steadyPollDelay);
}

void HandleAutosaveTick() {
    ExpirePendingSaveAsMigrationIfNeeded();
    const RuntimeTickSnapshot tick = CaptureAutosaveTickSnapshot();
    const TickDecision decision = EvaluateAutosaveTickDecision(tick);
    if (HandleAutosaveTickDecisionOutcome(tick, decision)) {
        return;
    }

    if (decision.action != TickDecisionAction::SaveDocument) {
        return;
    }

    ActiveDocumentSnapshot snapshot;
    const wchar_t* transitionPath =
        g_runtime.document.transitionFlushDocumentPath.Length() != 0
            ? g_runtime.document.transitionFlushDocumentPath.Get()
            : nullptr;
    HandleAutosaveAttemptResult(TrySaveActiveDocument(&snapshot, transitionPath), &snapshot, tick);
}

// ============================================================================
// Input Detection
// ============================================================================

bool IsEditingKey(WPARAM wParam) {
    const bool ctrlPressed = IsQueueKeyDown(VK_CONTROL);
    const bool shiftPressed = IsQueueKeyDown(VK_SHIFT);
    const bool altPressed = IsQueueKeyDown(VK_MENU);

    if (ctrlPressed && !shiftPressed && !altPressed && wParam == 'S') {
        HandleManualSave();
        return false;
    }

    if (ctrlPressed && !altPressed) {
        if (wParam == 'B' || wParam == 'I' || wParam == 'U' ||
            wParam == 'V' || wParam == 'X' || wParam == 'Y' || wParam == 'Z') {
            return true;
        }

        if (wParam == VK_RETURN) {
            return true;
        }

        return false;
    }

    if (altPressed) {
        return false;
    }

    if (wParam >= VK_KEY_A && wParam <= VK_KEY_Z) return true;
    if (wParam >= VK_KEY_0 && wParam <= VK_KEY_9) return true;
    if (wParam == VK_SPACE) return true;

    switch (wParam) {
        case VK_BACK:
        case VK_DELETE:
        case VK_RETURN:
        case VK_TAB:
            return true;
    }

    if (wParam >= VK_NUMPAD0 && wParam <= VK_NUMPAD9) return true;
    if (wParam == VK_MULTIPLY || wParam == VK_ADD ||
        wParam == VK_SUBTRACT || wParam == VK_DECIMAL || wParam == VK_DIVIDE) {
        return true;
    }

    switch (wParam) {
        case VK_OEM_1:
        case VK_OEM_2:
        case VK_OEM_3:
        case VK_OEM_4:
        case VK_OEM_5:
        case VK_OEM_6:
        case VK_OEM_7:
        case VK_OEM_PLUS:
        case VK_OEM_COMMA:
        case VK_OEM_MINUS:
        case VK_OEM_PERIOD:
        case VK_OEM_102:
            return true;
    }

    return false;
}

MessageAutosaveRole ClassifyMessageAutosaveRole(const MSG* lpMsg, bool wordEventsConnected) {
    if (!lpMsg) {
        return MessageAutosaveRole::None;
    }

    if (lpMsg->message == WM_KEYDOWN && IsEditingKey(lpMsg->wParam)) {
        return MessageAutosaveRole::EditingInput;
    }

    if (lpMsg->message == WM_CHAR && lpMsg->wParam >= 0x20) {
        return MessageAutosaveRole::EditingInput;
    }

    if (IsDocumentStateRefreshMessage(lpMsg->message) &&
        ShouldUseMessageDocumentRefreshFallback(wordEventsConnected)) {
        return MessageAutosaveRole::DocumentRefreshFallback;
    }

    if (!IsActionBoundaryMessage(lpMsg) || !HasPendingSaveWork()) {
        return MessageAutosaveRole::None;
    }

    const bool transitionFlush = IsTransitionFlushMessage(lpMsg);
    if (!ShouldUseMessageBoundaryFallback(wordEventsConnected, transitionFlush)) {
        return MessageAutosaveRole::None;
    }

    return transitionFlush ? MessageAutosaveRole::TransitionBoundaryFallback
                           : MessageAutosaveRole::ActionBoundaryFallback;
}

void HandleClassifiedMessageAutosaveRole(const MSG* lpMsg, bool wordEventsConnected) {
    const MessageAutosaveRole messageRole =
        ClassifyMessageAutosaveRole(lpMsg, wordEventsConnected);
    switch (messageRole) {
        case MessageAutosaveRole::EditingInput:
            ScheduleSaveFromEdit();
            break;

        case MessageAutosaveRole::DocumentRefreshFallback:
            ArmDocumentStateTimer(INPUT_SETTLE_DELAY_MS);
            break;

        case MessageAutosaveRole::ActionBoundaryFallback:
        case MessageAutosaveRole::TransitionBoundaryFallback: {
            const bool transitionFlush =
                messageRole == MessageAutosaveRole::TransitionBoundaryFallback;
            RequestPendingSaveExpedite(
                transitionFlush,
                transitionFlush
                    ? L"finishing pending changes before Word leaves the current document/window"
                    : L"finishing pending changes at the end of the current action",
                GetBoundaryCoalesceDelay(transitionFlush));
            break;
        }

        case MessageAutosaveRole::None:
            break;
    }
}

void MaybeArmDocumentStateMonitorFromMessage() {
    const bool hasActivePollWork = HasActiveDocumentStatePollWork();
    if (!IsScheduledTaskArmed(ScheduledTaskKind::DocumentState) &&
        hasActivePollWork) {
        ArmDocumentStateTimer(
            ComputeSteadyDocumentStatePollDelay(hasActivePollWork,
                                                AreWordEventsConnected()));
    }
}

// ============================================================================
// Hook
// ============================================================================

void HandleTranslateMessageAutosave(const MSG* lpMsg) {
    if (!lpMsg) {
        return;
    }

    if (ShouldInvalidateWordUiCacheForMessage(lpMsg)) {
        InvalidateWordUiWindowCache();
    }

    UpdateImeCompositionState(lpMsg);
    AdoptOwnerThreadIfNeeded(lpMsg);
    if (!IsOwnerThreadSchedulerContextValid()) {
        return;
    }

    const bool wordEventsConnected = AreWordEventsConnected();
    HandleClassifiedMessageAutosaveRole(lpMsg, wordEventsConnected);
    MaybeArmDocumentStateMonitorFromMessage();
}

BOOL WINAPI TranslateMessage_Hook(const MSG* lpMsg) {
    if (!g_originalTranslateMessage) {
        return TRUE;
    }

    HandleTranslateMessageAutosave(lpMsg);

    return g_originalTranslateMessage(lpMsg);
}

// ============================================================================
// Windhawk Callbacks
// ============================================================================

template <typename T>
class ScopedValueRestore {
public:
    explicit ScopedValueRestore(T* target) : m_target(target) {
        if (m_target) {
            m_value = *m_target;
        }
    }

    ~ScopedValueRestore() {
        if (m_target) {
            *m_target = m_value;
        }
    }

private:
    T* m_target = nullptr;
    T m_value = {};
};

class ScopedOwnerThreadRestore {
public:
    ScopedOwnerThreadRestore() : m_ownerThreadId(LoadOwnerThreadId()) {
    }

    ~ScopedOwnerThreadRestore() {
        ExchangeOwnerThreadId(m_ownerThreadId);
    }

private:
    DWORD m_ownerThreadId = 0;
};

void ReportSelfTestFailure(bool* success, const wchar_t* area) {
    Wh_Log(L"ERROR: Internal self-test failed for %ls", area);
    if (success) {
        *success = false;
    }
}

void ResetRuntimeTimingState(RuntimeTimingState* timing);
void ResetRuntimeStatusState(RuntimeStatusState* status);
void ResetRuntimeUiCacheState(RuntimeUiCacheState* ui);

bool RunTimingAndFallbackSelfTests() {
    bool success = true;

    if (ComputeBoundaryCoalesceDelay(false, 100, 200) != ACTION_BURST_SETTLE_DELAY_MS) {
        ReportSelfTestFailure(&success, L"burst coalescing delay");
    }

    if (ComputeBoundaryCoalesceDelay(true, 100, 200) != INPUT_SETTLE_DELAY_MS) {
        ReportSelfTestFailure(&success, L"transition coalescing delay");
    }

    DWORD retryDelayMs = MIN_RETRY_INTERVAL_MS;
    const DWORD firstDelay = AdvanceRetryDelay(&retryDelayMs, MAX_SAVE_RETRY_INTERVAL_MS);
    if (firstDelay != MIN_RETRY_INTERVAL_MS || retryDelayMs != MIN_RETRY_INTERVAL_MS * 2) {
        ReportSelfTestFailure(&success, L"retry backoff progression");
    }

    if (!ShouldUseMessageDocumentRefreshFallback(false) ||
        !ShouldUseMessageDocumentRefreshFallback(true)) {
        ReportSelfTestFailure(&success, L"message refresh fallback gating");
    }

    if (!ShouldUseMessageBoundaryFallback(false, false) ||
        !ShouldUseMessageBoundaryFallback(true, true) ||
        ShouldUseMessageBoundaryFallback(true, false)) {
        ReportSelfTestFailure(&success, L"boundary fallback gating");
    }

    if (!ShouldTrackDocumentStateWhileInactive(true, false, true) ||
        !ShouldTrackDocumentStateWhileInactive(false, true, true) ||
        !ShouldTrackDocumentStateWhileInactive(false, false, false) ||
        ShouldTrackDocumentStateWhileInactive(false, false, true)) {
        ReportSelfTestFailure(&success, L"inactive document-state tracking");
    }

    if (!ShouldRequireCleanSnapshotDetails(true, false, false, false) ||
        !ShouldRequireCleanSnapshotDetails(false, true, false, false) ||
        !ShouldRequireCleanSnapshotDetails(false, false, true, false) ||
        !ShouldRequireCleanSnapshotDetails(false, false, false, true) ||
        ShouldRequireCleanSnapshotDetails(false, false, false, false)) {
        ReportSelfTestFailure(&success, L"clean snapshot detail gating");
    }

    return success;
}

bool RunWordEventPolicySelfTests() {
    bool success = true;
    const ULONGLONG reconnectBaseTime = 100;
    const ULONGLONG reconnectInsideWindow =
        reconnectBaseTime + WORD_EVENT_RECONNECT_INTERVAL_MS - 1;
    const ULONGLONG reconnectOutsideWindow =
        reconnectBaseTime + WORD_EVENT_RECONNECT_INTERVAL_MS;

    if (!CanReuseConnectedWordEventState(false, true, true) ||
        CanReuseConnectedWordEventState(true, true, true) ||
        CanReuseConnectedWordEventState(false, false, true) ||
        CanReuseConnectedWordEventState(false, true, false)) {
        ReportSelfTestFailure(&success, L"Word event reuse gating");
    }

    if (!ShouldThrottleWordEventReconnect(false,
                                          false,
                                          reconnectBaseTime,
                                          reconnectInsideWindow) ||
        ShouldThrottleWordEventReconnect(false,
                                         false,
                                         0,
                                         reconnectInsideWindow) ||
        ShouldThrottleWordEventReconnect(false,
                                         true,
                                         reconnectBaseTime,
                                         reconnectInsideWindow) ||
        ShouldThrottleWordEventReconnect(true,
                                         false,
                                         reconnectBaseTime,
                                         reconnectInsideWindow) ||
        ShouldThrottleWordEventReconnect(false,
                                         false,
                                         reconnectBaseTime,
                                         reconnectOutsideWindow)) {
        ReportSelfTestFailure(&success, L"Word event reconnect throttling");
    }

    if (!ShouldReuseWordEventConnectionForHwnd(false, true, 42, 42) ||
        ShouldReuseWordEventConnectionForHwnd(true, true, 42, 42) ||
        ShouldReuseWordEventConnectionForHwnd(false, false, 42, 42) ||
        ShouldReuseWordEventConnectionForHwnd(false, true, 0, 42) ||
        ShouldReuseWordEventConnectionForHwnd(false, true, 42, 43)) {
        ReportSelfTestFailure(&success, L"event HWND reuse gating");
    }

    WordEventDispIds dispIds = {};
    dispIds.documentBeforeSave = 1;
    dispIds.documentBeforeClose = 2;
    dispIds.documentChange = 3;
    dispIds.windowDeactivate = 4;
    if (ClassifyWordApplicationEvent(dispIds, 1) !=
            WordApplicationEventKind::DocumentBeforeSave ||
        ClassifyWordApplicationEvent(dispIds, 2) !=
            WordApplicationEventKind::DocumentBeforeClose ||
        ClassifyWordApplicationEvent(dispIds, 3) !=
            WordApplicationEventKind::DocumentChange ||
        ClassifyWordApplicationEvent(dispIds, 4) !=
            WordApplicationEventKind::WindowDeactivate ||
        ClassifyWordApplicationEvent(dispIds, 99) !=
            WordApplicationEventKind::None) {
        ReportSelfTestFailure(&success, L"Word event dispatch classification");
    }

    return success;
}

bool RunSnapshotPolicySelfTests() {
    bool success = true;

    if (!ShouldPopulateSnapshotMetadataForMode(false, SnapshotMetadataMode::WhenDirty) ||
        ShouldPopulateSnapshotMetadataForMode(true, SnapshotMetadataMode::WhenDirty) ||
        !ShouldPopulateSnapshotMetadataForMode(true, SnapshotMetadataMode::Always)) {
        ReportSelfTestFailure(&success, L"snapshot metadata mode gating");
    }

    const SnapshotLoadPlan documentDirtyPlan =
        MakeDocumentStateSnapshotLoadPlan(false);
    if (documentDirtyPlan.request.source != SnapshotSource::Active ||
        documentDirtyPlan.request.context != SnapshotLogContext::DocumentState ||
        documentDirtyPlan.metadataMode != SnapshotMetadataMode::WhenDirty) {
        ReportSelfTestFailure(&success, L"document-state snapshot load plan");
    }

    const SnapshotLoadPlan documentFullPlan =
        MakeDocumentStateSnapshotLoadPlan(true);
    if (documentFullPlan.metadataMode != SnapshotMetadataMode::Always) {
        ReportSelfTestFailure(&success, L"document-state clean-detail plan");
    }

    const SnapshotLoadPlan activeSavePlan = MakeSaveSnapshotLoadPlan(nullptr);
    if (activeSavePlan.request.source != SnapshotSource::Active ||
        activeSavePlan.request.context != SnapshotLogContext::Save ||
        activeSavePlan.metadataMode != SnapshotMetadataMode::Always) {
        ReportSelfTestFailure(&success, L"active save snapshot load plan");
    }

    const SnapshotLoadPlan specificSavePlan =
        MakeSaveSnapshotLoadPlan(L"C:\\dummy.docx");
    if (specificSavePlan.request.source != SnapshotSource::SpecificPath ||
        specificSavePlan.request.path == nullptr) {
        ReportSelfTestFailure(&success, L"specific-path save snapshot load plan");
    }

    ActiveDocumentSnapshot cleanSnapshot = {};
    cleanSnapshot.saved = true;
    ActiveDocumentSnapshot dirtySnapshot = {};
    dirtySnapshot.saved = false;
    if (InterpretDocumentStateSnapshotQueryResult(&cleanSnapshot,
                                                  SnapshotQueryResult::Ready) !=
            DocumentDirtyState::Clean ||
        InterpretDocumentStateSnapshotQueryResult(&dirtySnapshot,
                                                  SnapshotQueryResult::Ready) !=
            DocumentDirtyState::Dirty ||
        InterpretDocumentStateSnapshotQueryResult(&cleanSnapshot,
                                                  SnapshotQueryResult::Cleared) !=
            DocumentDirtyState::Clean ||
        InterpretDocumentStateSnapshotQueryResult(&cleanSnapshot,
                                                  SnapshotQueryResult::RetryLater) !=
            DocumentDirtyState::RetryLater) {
        ReportSelfTestFailure(&success, L"document-state snapshot interpretation");
    }

    return success;
}

bool RunOwnerThreadAndSchedulerSelfTests() {
    bool success = true;

    ScopedOwnerThreadRestore ownerThreadRestore;
    ClearOwnerThreadId();
    if (LoadOwnerThreadId() != 0) {
        ReportSelfTestFailure(&success, L"owner-thread clear helper");
    }

    const DWORD currentThreadId = GetCurrentThreadId();
    if (ExchangeOwnerThreadId(currentThreadId) != 0 ||
        LoadOwnerThreadId() != currentThreadId) {
        ReportSelfTestFailure(&success, L"owner-thread exchange helper");
    }

    if (!IsOwnerThreadSchedulerContextValid()) {
        ReportSelfTestFailure(&success, L"owner-thread scheduler guard");
    }

    ClearOwnerThreadId();
    if (IsOwnerThreadSchedulerContextValid()) {
        ReportSelfTestFailure(&success, L"owner-thread scheduler reset");
    }

    RuntimeFlagSnapshot idleFlags = {};
    if (GetSteadyDocumentStatePollDelay(&idleFlags) !=
        DOCUMENT_STATE_IDLE_POLL_INTERVAL_MS) {
        ReportSelfTestFailure(&success, L"steady poll idle delay");
    }

    RuntimeFlagSnapshot eventIdleFlags = {};
    eventIdleFlags.wordEventsConnected = true;
    if (GetSteadyDocumentStatePollDelay(&eventIdleFlags) !=
        DOCUMENT_STATE_EVENT_IDLE_POLL_INTERVAL_MS) {
        ReportSelfTestFailure(&success, L"steady poll event-idle delay");
    }

    RuntimeFlagSnapshot activeFlags = {};
    activeFlags.pendingSave = true;
    activeFlags.wordEventsConnected = true;
    if (GetSteadyDocumentStatePollDelay(&activeFlags) !=
        DOCUMENT_STATE_ACTIVE_POLL_INTERVAL_MS) {
        ReportSelfTestFailure(&success, L"steady poll active delay");
    }

    WordEventSession session;
    session.applicationHwnd = 42;
    session.cookie = 17;
    if (session.IsConnected()) {
        ReportSelfTestFailure(&success, L"word event session disconnected state");
    }

    session.Reset();
    if (session.applicationHwnd != 0 ||
        session.cookie != 0) {
        ReportSelfTestFailure(&success, L"word event session reset");
    }

    RuntimeTimingState timingState = {};
    timingState.schedulerTimerId = 5;
    timingState.lastEditTime = 7;
    timingState.saveRetryDelayMs = 123;
    timingState.documentStateRetryDelayMs = 456;
    ResetRuntimeTimingState(&timingState);
    if (timingState.schedulerTimerId != 0 ||
        timingState.lastEditTime != 0 ||
        timingState.saveRetryDelayMs != MIN_RETRY_INTERVAL_MS ||
        timingState.documentStateRetryDelayMs != MIN_RETRY_INTERVAL_MS) {
        ReportSelfTestFailure(&success, L"timing-state reset helper");
    }

    RuntimeUiCacheState uiCacheState = {};
    uiCacheState.cachedWordRootWindow = reinterpret_cast<HWND>(1);
    uiCacheState.cachedNativeWordWindow = reinterpret_cast<HWND>(2);
    uiCacheState.cachedWordUiThreadId = 3;
    ResetRuntimeUiCacheState(&uiCacheState);
    if (uiCacheState.cachedWordRootWindow != nullptr ||
        uiCacheState.cachedNativeWordWindow != nullptr ||
        uiCacheState.cachedWordUiThreadId != 0) {
        ReportSelfTestFailure(&success, L"ui-cache reset helper");
    }

    ActiveDocumentSnapshot protectedViewSnapshot = {};
    protectedViewSnapshot.clearedReason = SnapshotClearedReason::ProtectedView;
    if (InterpretSaveSnapshotQueryResult(nullptr,
                                         &protectedViewSnapshot,
                                         SnapshotQueryResult::Cleared) !=
            SaveAttemptResult::Deferred ||
        InterpretSaveSnapshotQueryResult(L"C:\\dummy.docx",
                                         &protectedViewSnapshot,
                                         SnapshotQueryResult::Cleared) !=
            SaveAttemptResult::Cleared ||
        InterpretSaveSnapshotQueryResult(nullptr,
                                         &protectedViewSnapshot,
                                         SnapshotQueryResult::RetryLater) !=
            SaveAttemptResult::RetryLater) {
        ReportSelfTestFailure(&success, L"save snapshot interpretation");
    }

    ScopedValueRestore<ULONGLONG> schedulerDueTimeRestore(
        &g_runtime.timing.schedulerTimerDueTime);
    ScopedValueRestore<ULONGLONG> saveDueTimeRestore(&g_runtime.timing.saveTimerDueTime);
    ScopedValueRestore<ULONGLONG> documentStateDueTimeRestore(
        &g_runtime.timing.documentStateTimerDueTime);
    g_runtime.timing.schedulerTimerDueTime = 0;
    g_runtime.timing.saveTimerDueTime = 400;
    g_runtime.timing.documentStateTimerDueTime = 250;
    if (GetNextScheduledTaskDueTime() != 250) {
        ReportSelfTestFailure(&success, L"scheduler due-time selection");
    }

    return success;
}

bool RunTickDecisionSelfTests() {
    bool success = true;

    RuntimeTickSnapshot pausedDocumentTick = {};
    pausedDocumentTick.runtime.ownerThreadSynchronized = true;
    pausedDocumentTick.runtime.pauseReason = WordUiPauseReason::MenuMode;
    pausedDocumentTick.steadyPollDelay = DOCUMENT_STATE_IDLE_POLL_INTERVAL_MS;
    TickDecision pausedDocumentDecision = EvaluateDocumentStateTickDecision(pausedDocumentTick);
    if (pausedDocumentDecision.action != TickDecisionAction::RearmDocumentStateTimer ||
        pausedDocumentDecision.state != RuntimeStatePhase::WaitingForWordUi ||
        pausedDocumentDecision.delayMs != INPUT_SETTLE_DELAY_MS) {
        ReportSelfTestFailure(&success, L"paused document-state tick decision");
    }

    RuntimeTickSnapshot readyDocumentTick = {};
    readyDocumentTick.runtime.ownerThreadSynchronized = true;
    readyDocumentTick.runtime.activeWordDocumentWindow = true;
    readyDocumentTick.steadyPollDelay = DOCUMENT_STATE_IDLE_POLL_INTERVAL_MS;
    TickDecision readyDocumentDecision = EvaluateDocumentStateTickDecision(readyDocumentTick);
    if (readyDocumentDecision.action != TickDecisionAction::RefreshDocumentState ||
        readyDocumentDecision.state != RuntimeStatePhase::ReadyToRefreshDocumentState) {
        ReportSelfTestFailure(&success, L"ready document-state tick decision");
    }

    RuntimeTickSnapshot readySaveTick = {};
    readySaveTick.now = 1000;
    readySaveTick.runtime.pendingSave = true;
    readySaveTick.runtime.ownerThreadSynchronized = true;
    readySaveTick.runtime.activeWordDocumentWindow = true;
    readySaveTick.runtime.transitionFlushRequested = true;
    TickDecision readySaveDecision = EvaluateAutosaveTickDecision(readySaveTick);
    if (readySaveDecision.action != TickDecisionAction::SaveDocument ||
        readySaveDecision.state != RuntimeStatePhase::ReadyToSave) {
        ReportSelfTestFailure(&success, L"ready auto-save tick decision");
    }

    return success;
}

bool RunInternalSelfTests() {
    bool success = true;
    success = RunTimingAndFallbackSelfTests() && success;
    success = RunWordEventPolicySelfTests() && success;
    success = RunSnapshotPolicySelfTests() && success;
    success = RunOwnerThreadAndSchedulerSelfTests() && success;
    success = RunTickDecisionSelfTests() && success;
    return success;
}

void ResetRuntimeTimingState(RuntimeTimingState* timing) {
    if (!timing) {
        return;
    }

    timing->schedulerTimerId = 0;
    timing->schedulerTimerDueTime = 0;
    timing->saveTimerDueTime = 0;
    timing->documentStateTimerDueTime = 0;
    timing->lastEditTime = 0;
    timing->lastSaveTime = 0;
    timing->lastEventConnectAttemptTime = 0;
    timing->pendingSaveAsTime = 0;
    timing->saveRetryDelayMs = MIN_RETRY_INTERVAL_MS;
    timing->documentStateRetryDelayMs = MIN_RETRY_INTERVAL_MS;
}

void ResetRuntimeStatusState(RuntimeStatusState* status) {
    if (!status) {
        return;
    }

    status->lastSaveFailureLogTime = 0;
    status->lastDocumentStateFailureLogTime = 0;
    status->lastSaveStatusLogTime = 0;
    status->lastDocumentStateStatusLogTime = 0;
    ClearStoredStatusMessage(&status->lastSaveStatusMessage,
                             &status->lastSaveStatusLogTime);
    ClearStoredStatusMessage(&status->lastDocumentStateStatusMessage,
                             &status->lastDocumentStateStatusLogTime);
}

void ResetRuntimeUiCacheState(RuntimeUiCacheState* ui) {
    if (!ui) {
        return;
    }

    ui->cachedWordRootWindow = nullptr;
    ui->cachedNativeWordWindow = nullptr;
    ui->cachedWordUiThreadId = 0;
}

void ResetRuntimeState() {
    CancelSchedulerTimer();
    ResetRuntimeTimingState(&g_runtime.timing);
    ResetRuntimeStatusState(&g_runtime.status);
    ClearDispatchMemberIdCache();
    DisconnectWordApplicationEvents();
    ClearAutomationBusyPending();
    ClearPendingSaveAsMigration();
    ClearPendingSave();
    ClearExpeditedSavePending();
    ClearTransitionFlushRequest();
    ClearManualSavePending();
    ClearFlag(g_runtime.flags.imeComposing);
    ResetObservedDocumentState();
    ResetRuntimeUiCacheState(&g_runtime.ui);
    ClearOwnerThreadId();
}

void LoadSettings() {
    g_settings.saveDelay = Wh_GetIntSetting(L"saveDelay");
    g_settings.minTimeBetweenSaves = Wh_GetIntSetting(L"minTimeBetweenSaves");

    if (g_settings.saveDelay < MIN_SAVE_DELAY_MS) {
        g_settings.saveDelay = MIN_SAVE_DELAY_MS;
    }
    if (g_settings.saveDelay > MAX_SAVE_DELAY_MS) {
        g_settings.saveDelay = MAX_SAVE_DELAY_MS;
    }
    if (g_settings.minTimeBetweenSaves < 0) {
        g_settings.minTimeBetweenSaves = 0;
    }
    if (g_settings.minTimeBetweenSaves > MAX_MIN_TIME_BETWEEN_SAVES) {
        g_settings.minTimeBetweenSaves = MAX_MIN_TIME_BETWEEN_SAVES;
    }

    Wh_Log(L"Settings: saveDelay=%d ms, minTimeBetweenSaves=%d ms",
           g_settings.saveDelay, g_settings.minTimeBetweenSaves);
}

BOOL Wh_ModInit() {
    Wh_Log(L"Word Local AutoSave v3.5 initializing...");

    g_runtime.wordProcessId = GetCurrentProcessId();
    ResetRuntimeState();
    LoadSettings();
    if (!RunInternalSelfTests()) {
        Wh_Log(L"WARNING: One or more internal self-tests failed");
    }

    HMODULE user32 = GetModuleHandleW(L"user32.dll");
    if (!user32) {
        Wh_Log(L"ERROR: Failed to get user32.dll handle");
        return FALSE;
    }

    void* translateMessageAddr = reinterpret_cast<void*>(GetProcAddress(user32, "TranslateMessage"));
    if (!translateMessageAddr) {
        Wh_Log(L"ERROR: Failed to get TranslateMessage address");
        return FALSE;
    }

    if (!Wh_SetFunctionHook(translateMessageAddr,
                            reinterpret_cast<void*>(TranslateMessage_Hook),
                            reinterpret_cast<void**>(&g_originalTranslateMessage))) {
        Wh_Log(L"ERROR: Failed to hook TranslateMessage");
        return FALSE;
    }

    if (!g_originalTranslateMessage) {
        Wh_Log(L"ERROR: Original TranslateMessage pointer is null");
        return FALSE;
    }

    SetFlag(g_runtime.flags.moduleActive);

    Wh_Log(L"Word Local AutoSave initialized");
    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L"Word Local AutoSave uninitializing...");

    ClearFlag(g_runtime.flags.moduleActive);
    ResetRuntimeState();

    Wh_Log(L"Word Local AutoSave uninitialized");
}

void Wh_ModSettingsChanged() {
    Wh_Log(L"Settings changed, reloading...");
    ResetRuntimeState();
    LoadSettings();
}
