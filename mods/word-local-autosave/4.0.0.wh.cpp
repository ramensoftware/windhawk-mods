// ==WindhawkMod==
// @id              word-local-autosave
// @name            Word Local AutoSave
// @description     Enables AutoSave functionality for local documents in Microsoft Word via direct Word saves
// @version         4.0.0
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
- Keeps pending changes armed after direct save failures so transient file errors can be retried
- Only saves when the active Word document window is focused

## Shortcut Safety (v4.0.0)

- No `SendInput`
- No synthetic `Ctrl` state
- No partial `Ctrl+...` races
- Save execution stays on one owner UI thread
- Pending input and held modifiers postpone auto-save instead of racing it

## Performance and Reliability (v4.0.0)

- Uses Word events as the primary signal with an adaptive low-frequency watchdog
- Reuses cached Word application, document, metadata, and automation member IDs
- Avoids filesystem path resolution in the steady editing path
- Uses one owner-window timer with reason-aware bounded retry backoff

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
#include <atomic>
#include <limits>
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
const DWORD DOCUMENT_STATE_EVENT_IDLE_POLL_INTERVAL_MS = 30000;
const DWORD DOCUMENT_STATE_PENDING_WATCHDOG_MS = 1500;
const DWORD SAVE_AUTOMATION_RETRY_INITIAL_MS = 125;
const DWORD SAVE_AUTOMATION_RETRY_MAX_MS = 5000;
const DWORD SAVE_HARD_RETRY_INITIAL_MS = 2000;
const DWORD SAVE_HARD_RETRY_MAX_MS = 30000;
const DWORD SAVE_TARGET_RETRY_INITIAL_MS = 250;
const DWORD SAVE_TARGET_RETRY_MAX_MS = 5000;
const DWORD SAVE_DEFERRED_WATCHDOG_MS = 10000;
const DWORD SAVE_INACTIVE_EVENT_WATCHDOG_MS = 30000;
const DWORD SAVE_INACTIVE_FALLBACK_WATCHDOG_MS = 5000;
const DWORD SAVE_UI_INPUT_WATCHDOG_MS = 2000;
const DWORD DOCUMENT_BUSY_RETRY_INITIAL_MS = 125;
const DWORD DOCUMENT_BUSY_RETRY_MAX_MS = 5000;
const DWORD DOCUMENT_TARGET_RETRY_INITIAL_MS = 250;
const DWORD DOCUMENT_TARGET_RETRY_MAX_MS = 5000;
const DWORD DOCUMENT_HARD_RETRY_INITIAL_MS = 2000;
const DWORD DOCUMENT_HARD_RETRY_MAX_MS = 30000;
const DWORD SAVE_FAILURE_LOG_INTERVAL_MS = 30000;
const DWORD DOCUMENT_FAILURE_LOG_INTERVAL_MS = 10000;
const DWORD RETRY_REASON_CHANGE_LOG_INTERVAL_MS = 2000;
const DWORD RETRY_BUSY_LOG_INTERVAL_MS = 10000;
const DWORD RETRY_HARD_LOG_INTERVAL_MS = 30000;
const DWORD RETRY_WAIT_LOG_INTERVAL_MS = 60000;
const DWORD STATUS_LOG_INTERVAL_MS = 3000;
const DWORD WORD_EVENT_DISCONNECT_FAILURE_LOG_INTERVAL_MS = 10000;
const DWORD WORD_EVENT_SHUTDOWN_DISCONNECT_RETRY_DELAY_MS = 50;
const DWORD TEXT_INPUT_KEYDOWN_CHAR_SUPPRESSION_MS = 1000;
const DWORD COM_MESSAGE_FILTER_RETRY_DELAY_MS = 150;
const DWORD COM_MESSAGE_FILTER_CANCEL_AFTER_MS = 10000;
const DWORD COM_BACKGROUND_RETRY_DELAY_MS = 100;
const DWORD COM_BACKGROUND_RETRY_BUDGET_MS = 1000;
const DWORD SAVE_AS_MIGRATION_TIMEOUT_MS = 15000;
const DWORD WORD_EVENT_RECONNECT_INTERVAL_MS = 2000;
const DWORD WORD_EVENT_DISCONNECT_RETRY_INTERVAL_MS = 2000;
const int WORD_EVENT_SHUTDOWN_DISCONNECT_RETRY_ATTEMPTS = 6;
const int WORD_EVENT_PENDING_DISCONNECT_CAPACITY = 4;
const DWORD MAX_CANONICAL_PATH_CHARS = 32768;
const DWORD OBJID_NATIVEOM_VALUE = 0xFFFFFFF0u;
const UINT WORD_LOCAL_AUTOSAVE_CONTROL_MESSAGE = WM_APP + 0x4A3;
const UINT_PTR SCHEDULER_WINDOW_TIMER_ID = 0x574C;
const DWORD OWNER_SHUTDOWN_WAKE_INTERVAL_MS = 100;
const DWORD OWNER_SHUTDOWN_FAILSAFE_TIMEOUT_MS = 10000;

const IID kIIDNull = {};
const IID kIIDIDispatch = {
    0x00020400,
    0x0000,
    0x0000,
    {0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46}
};

#ifdef CONNECT_E_NOCONNECTION
const HRESULT kConnectENoConnection = CONNECT_E_NOCONNECTION;
#else
const HRESULT kConnectENoConnection = static_cast<HRESULT>(0x80040200L);
#endif
#ifdef CO_E_OBJNOTCONNECTED
const HRESULT kCoEObjectNotConnected = CO_E_OBJNOTCONNECTED;
#else
const HRESULT kCoEObjectNotConnected = static_cast<HRESULT>(0x800401FDL);
#endif
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
    DISPID windowActivate = DISPID_UNKNOWN;
    DISPID windowDeactivate = DISPID_UNKNOWN;

    void Reset() {
        documentBeforeSave = DISPID_UNKNOWN;
        documentBeforeClose = DISPID_UNKNOWN;
        documentChange = DISPID_UNKNOWN;
        windowActivate = DISPID_UNKNOWN;
        windowDeactivate = DISPID_UNKNOWN;
    }
};

enum class WordMember {
    ApplicationActiveWindow,
    ApplicationWindows,
    ApplicationHwnd,
    ApplicationActiveDocument,
    ApplicationActiveProtectedViewWindow,
    ApplicationDocuments,
    WindowHwnd,
    WindowDocument,
    WindowsCount,
    WindowsItem,
    DocumentsCount,
    DocumentsItem,
    DocumentApplication,
    DocumentSaved,
    DocumentReadOnly,
    DocumentPath,
    DocumentFullName,
    DocumentName,
    DocumentSave,
    Count,
};

struct WordMemberIdCacheEntry {
    DISPID dispId = DISPID_UNKNOWN;
    bool resolved = false;
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
        if (m_ptr == ptr) {
            return;
        }

        // Publish the replacement before Release. Releasing an STA proxy can
        // pump messages and re-enter the mod; no re-entrant observer may see
        // a pointer whose final Release is already in progress.
        T* previous = m_ptr;
        m_ptr = ptr;
        if (previous) {
            previous->Release();
        }
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
        if (m_value == value) {
            return;
        }

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

class ScopedHandle {
public:
    explicit ScopedHandle(HANDLE handle = INVALID_HANDLE_VALUE)
        : m_handle(handle) {
    }

    ScopedHandle(const ScopedHandle&) = delete;
    ScopedHandle& operator=(const ScopedHandle&) = delete;

    ScopedHandle(ScopedHandle&& other) noexcept : m_handle(other.Detach()) {
    }

    ScopedHandle& operator=(ScopedHandle&& other) noexcept {
        if (this != &other) {
            Reset(other.Detach());
        }

        return *this;
    }

    ~ScopedHandle() {
        Reset();
    }

    HANDLE Get() const {
        return m_handle;
    }

    bool IsValid() const {
        return IsValidHandle(m_handle);
    }

    void Reset(HANDLE handle = INVALID_HANDLE_VALUE) {
        if (m_handle == handle) {
            return;
        }

        if (IsValidHandle(m_handle)) {
            CloseHandle(m_handle);
        }

        m_handle = handle;
    }

    HANDLE Detach() {
        HANDLE handle = m_handle;
        m_handle = INVALID_HANDLE_VALUE;
        return handle;
    }

private:
    static bool IsValidHandle(HANDLE handle) {
        return handle && handle != INVALID_HANDLE_VALUE;
    }

    HANDLE m_handle = INVALID_HANDLE_VALUE;
};

// ============================================================================
// Global State
// ============================================================================

struct RuntimeSettings {
    int saveDelay;
    int minTimeBetweenSaves;
};

RuntimeSettings g_settings = {};

TranslateMessage_t g_originalTranslateMessage = nullptr;

class WordApplicationEventSink;

enum class AutomationFailureClass {
    None,
    Busy,
    Disconnected,
    Hard,
};

enum class SaveRetryReason {
    None,
    AutomationBusy,
    HardFailure,
    TargetUnavailable,
    DeferredProtectedView,
    InactiveWindow,
    UiPaused,
    InputBusy,
};

enum class DocumentRetryReason {
    None,
    Busy,
    Disconnected,
    Hard,
    InactiveWindow,
    UiPaused,
    InputBusy,
};

enum class SaveResumeSignal {
    None,
    WordActivated,
    DocumentChanged,
    UiReleased,
    ActionBoundary,
};

enum class ComRetryProfile {
    Background,
    CriticalClose,
    LegacyLifecycle,
};

struct RuntimeSaveRetryState {
    SaveRetryReason reason = SaveRetryReason::None;
    SaveRetryReason lastLoggedReason = SaveRetryReason::None;
    DWORD automationNextDelayMs = SAVE_AUTOMATION_RETRY_INITIAL_MS;
    DWORD hardNextDelayMs = SAVE_HARD_RETRY_INITIAL_MS;
    DWORD targetNextDelayMs = SAVE_TARGET_RETRY_INITIAL_MS;
    ULONGLONG lastReasonLogTime = 0;
};

struct RuntimeDocumentRetryState {
    DocumentRetryReason reason = DocumentRetryReason::None;
    DocumentRetryReason lastLoggedReason = DocumentRetryReason::None;
    DWORD busyNextDelayMs = DOCUMENT_BUSY_RETRY_INITIAL_MS;
    DWORD disconnectedNextDelayMs = DOCUMENT_TARGET_RETRY_INITIAL_MS;
    DWORD hardNextDelayMs = DOCUMENT_HARD_RETRY_INITIAL_MS;
    ULONGLONG lastReasonLogTime = 0;
};

struct RuntimeTimingState {
    UINT_PTR schedulerTimerId = 0;
    ULONGLONG schedulerTimerDueTime = 0;
    ULONGLONG saveTimerDueTime = 0;
    ULONGLONG documentStateTimerDueTime = 0;
    ULONGLONG eventDisconnectRetryTimerDueTime = 0;
    ULONGLONG lastEditTime = 0;
    ULONGLONG transitionFlushRequestTime = 0;
    ULONGLONG lastSaveTime = 0;
    ULONGLONG lastEventConnectAttemptTime = 0;
    ULONGLONG lastTextInputKeyDownTime = 0;
    ULONGLONG pendingSaveAsTime = 0;
};

struct RuntimeStatusState {
    ScopedBstr lastSaveStatusMessage;
    ScopedBstr lastDocumentStateStatusMessage;
    ULONGLONG lastSaveFailureLogTime = 0;
    ULONGLONG lastDocumentStateFailureLogTime = 0;
    ULONGLONG lastEventDisconnectFailureLogTime = 0;
    ULONGLONG lastSaveStatusLogTime = 0;
    ULONGLONG lastDocumentStateStatusLogTime = 0;
};

struct RuntimeDocumentState {
    ScopedBstr observedDocumentPath;
    ScopedBstr transitionFlushDocumentPath;
    ScopedComPtr<IDispatch> observedDocument;
    ScopedComPtr<IDispatch> transitionFlushDocument;
    ScopedComPtr<IUnknown> observedDocumentIdentity;
    ScopedComPtr<IUnknown> pendingSaveAsDocumentIdentity;
    ScopedComPtr<IUnknown> transitionFlushDocumentIdentity;
    unsigned int observedDocumentGeneration = 0;
    unsigned int transitionFlushGeneration = 0;
    unsigned int pendingSaveAsGeneration = 0;
};

struct RuntimeAutomationCacheState {
    ScopedComPtr<IDispatch> application;
    ScopedComPtr<IUnknown> applicationIdentity;
    LONG_PTR applicationHwnd = 0;
    ScopedComPtr<IDispatch> activeDocument;
    ScopedComPtr<IUnknown> activeDocumentIdentity;
    ScopedBstr activeDocumentPath;
    unsigned int applicationGeneration = 0;
    unsigned int documentGeneration = 0;
    unsigned int resetDepth = 0;
    unsigned int activeDocumentResetDepth = 0;
    bool activeDocumentStale = true;
    bool activeDocumentMetadataValid = false;
    bool activeDocumentReadOnly = false;
    bool activeDocumentHasPath = false;
};

struct WordEventSession {
    WordEventSession() = default;
    ~WordEventSession() {
        Reset();
    }

    WordEventSession(const WordEventSession&) = delete;
    WordEventSession& operator=(const WordEventSession&) = delete;
    WordEventSession(WordEventSession&& other) noexcept {
        MoveFrom(&other);
    }

    WordEventSession& operator=(WordEventSession&& other) noexcept {
        if (this != &other) {
            // Publish the replacement before releasing the previous session.
            // A COM Release can pump messages and re-enter event management.
            WordEventSession previous;
            previous.MoveFrom(this);
            MoveFrom(&other);
        }

        return *this;
    }

    LONG_PTR applicationHwnd = 0;
    ScopedComPtr<IDispatch> application;
    ScopedComPtr<IConnectionPoint> connectionPoint;
    ScopedComPtr<IDispatch> sink;
    WordApplicationEventSink* sinkControl = nullptr;
    DWORD cookie = 0;

    void Reset() {
        // Make the aggregate observably disconnected before any Release.
        applicationHwnd = 0;
        sinkControl = nullptr;
        cookie = 0;

        // Keep the sink alive while the application and connection point are
        // released, but detach every field before the first Release so a
        // re-entrant replacement can't be erased by the remainder of Reset.
        ScopedComPtr<IDispatch> previousSink;
        ScopedComPtr<IConnectionPoint> previousConnectionPoint;
        ScopedComPtr<IDispatch> previousApplication;
        previousSink.Reset(sink.Detach());
        previousConnectionPoint.Reset(connectionPoint.Detach());
        previousApplication.Reset(application.Detach());
    }

    bool IsConnected() const {
        return connectionPoint && cookie != 0;
    }

    bool IsEmpty() const {
        return applicationHwnd == 0 && !application && !connectionPoint &&
               !sink && !sinkControl && cookie == 0;
    }

private:
    void MoveFrom(WordEventSession* other) {
        if (!other) {
            return;
        }

        applicationHwnd = other->applicationHwnd;
        application = std::move(other->application);
        connectionPoint = std::move(other->connectionPoint);
        sink = std::move(other->sink);
        sinkControl = other->sinkControl;
        cookie = other->cookie;

        other->applicationHwnd = 0;
        other->sinkControl = nullptr;
        other->cookie = 0;
    }
};

struct RuntimeEventState {
    WordEventSession session;
    // A connection candidate lives in runtime-owned storage until commit or a
    // proven Unadvise. This prevents a failed rollback from dying with a stack
    // temporary when the deferred-disconnect queue is full.
    WordEventSession stagedSession;
    WordEventSession pendingDisconnectSessions[WORD_EVENT_PENDING_DISCONNECT_CAPACITY];
    WordEventDispIds cachedDispIds;
    bool cachedDispIdsValid = false;
    unsigned int connectionEpoch = 0;
    unsigned int connectionBuildDepth = 0;
    unsigned int primaryDisconnectDepth = 0;
    unsigned int pendingDisconnectRetryDepth = 0;
};

struct RuntimeUiCacheState {
    HWND cachedWordRootWindow = nullptr;
    HWND cachedNativeWordWindow = nullptr;
    DWORD cachedWordUiThreadId = 0;
};

struct RuntimeFlagState {
    std::atomic<LONG> pendingSave{FALSE};
    std::atomic<LONG> documentDirtyKnown{FALSE};
    std::atomic<LONG> documentDirty{FALSE};
    std::atomic<LONG> manualSavePending{FALSE};
    std::atomic<LONG> expeditedSavePending{FALSE};
    std::atomic<LONG> transitionFlushPending{FALSE};
    std::atomic<LONG> imeComposing{FALSE};
    std::atomic<LONG> automationBusyPending{FALSE};
    std::atomic<LONG> pendingSaveAsMigration{FALSE};
    std::atomic<LONG> wordEventsConnected{FALSE};
    std::atomic<LONG> wordEventDisconnectRetryPending{FALSE};
    std::atomic<LONG> autoSaveInProgress{FALSE};
    std::atomic<LONG> postTransitionRefreshPending{FALSE};
    std::atomic<LONG> suppressNextCharacterInput{FALSE};
    std::atomic<LONG> moduleActive{FALSE};
};

static_assert(std::atomic<LONG>::is_always_lock_free,
              "runtime flags require lock-free 32-bit atomics");

struct RuntimeState {
    DWORD wordProcessId = 0;
    volatile LONG ownerThreadId = 0;
    unsigned int ownerWorkDepth = 0;
    bool shutdownInProgress = false;
    RuntimeTimingState timing;
    RuntimeSaveRetryState saveRetry;
    RuntimeDocumentRetryState documentRetry;
    RuntimeStatusState status;
    RuntimeDocumentState document;
    RuntimeAutomationCacheState automation;
    RuntimeEventState events;
    RuntimeUiCacheState ui;
    WordMemberIdCacheEntry
        wordMemberIdCache[static_cast<size_t>(WordMember::Count)] = {};
    RuntimeFlagState flags;
};

RuntimeState g_runtime = {};
UINT g_startupBootstrapMessage = 0;

enum RuntimeControlRequest : LONG {
    RuntimeControlApplySettings = 1 << 0,
    RuntimeControlShutdown = 1 << 1,
    RuntimeControlRepairScheduler = 1 << 2,
};

enum RuntimeControlLifecycle : LONG {
    RuntimeControlAwaitingOwner = 0,
    RuntimeControlOwnerActive = 1,
    RuntimeControlStopRequested = 2,
    RuntimeControlStopped = 3,
};

enum RuntimeShutdownOutcome : LONG {
    RuntimeShutdownPending = 0,
    RuntimeShutdownCompleted = 1,
    RuntimeShutdownFallbackClaimed = 2,
};

struct RuntimeControlState {
    std::atomic<LONG> pendingRequests = 0;
    std::atomic<LONG> lifecycle = RuntimeControlAwaitingOwner;
    std::atomic<ULONG> settingsSequence = 0;
    std::atomic<ULONG> appliedSettingsSequence = 0;
    std::atomic<ULONGLONG> packedSettings = 0;
    std::atomic<HWND> messageWindow = nullptr;
    std::atomic<bool> ownerRuntimeLive = false;
    std::atomic<bool> shutdownRequested = false;
    std::atomic<bool> shutdownComplete = false;
    std::atomic<LONG> shutdownOutcome = RuntimeShutdownPending;
    std::atomic<bool> requiresPinnedShutdown = false;
    std::atomic<bool> modulePinnedForSafety = false;
    HANDLE shutdownCompleteEvent = nullptr;
};

RuntimeControlState g_control;

// ============================================================================
// Utility Helpers
// ============================================================================

bool LoadFlag(const std::atomic<LONG>& value) {
    return value.load(std::memory_order_acquire) != FALSE;
}

void StoreFlag(std::atomic<LONG>& value, bool enabled) {
    value.store(enabled ? TRUE : FALSE, std::memory_order_release);
}

void SetFlag(std::atomic<LONG>& value) {
    StoreFlag(value, true);
}

void ClearFlag(std::atomic<LONG>& value) {
    StoreFlag(value, false);
}

bool LoadInterlockedFlag(const volatile LONG& value) {
    return InterlockedCompareExchange(
               const_cast<volatile LONG*>(&value),
               FALSE,
               FALSE) != FALSE;
}

void SetInterlockedFlag(volatile LONG& value) {
    InterlockedExchange(&value, TRUE);
}

void ClearInterlockedFlag(volatile LONG& value) {
    InterlockedExchange(&value, FALSE);
}

class ScopedFlagSet {
public:
    explicit ScopedFlagSet(std::atomic<LONG>& flag) : m_flag(&flag) {
        SetFlag(*m_flag);
    }

    ScopedFlagSet(const ScopedFlagSet&) = delete;
    ScopedFlagSet& operator=(const ScopedFlagSet&) = delete;

    ~ScopedFlagSet() {
        if (m_flag) {
            ClearFlag(*m_flag);
        }
    }

private:
    std::atomic<LONG>* m_flag = nullptr;
};

class ScopedInterlockedCount {
public:
    explicit ScopedInterlockedCount(volatile LONG& value) : m_value(&value) {
        InterlockedIncrement(m_value);
    }

    ScopedInterlockedCount(const ScopedInterlockedCount&) = delete;
    ScopedInterlockedCount& operator=(const ScopedInterlockedCount&) = delete;

    ~ScopedInterlockedCount() {
        if (m_value) {
            InterlockedDecrement(m_value);
        }
    }

private:
    volatile LONG* m_value = nullptr;
};

class ScopedOwnerDepth {
public:
    explicit ScopedOwnerDepth(unsigned int& value) : m_value(&value) {
        ++*m_value;
    }

    ScopedOwnerDepth(const ScopedOwnerDepth&) = delete;
    ScopedOwnerDepth& operator=(const ScopedOwnerDepth&) = delete;

    ~ScopedOwnerDepth() {
        if (m_value && *m_value != 0) {
            --*m_value;
        }
    }

private:
    unsigned int* m_value = nullptr;
};

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

void ProcessOwnerControlRequests();
void SignalOwnerShutdownComplete();
void QueueOwnerControlRequest(LONG request);

class ScopedOwnerRuntimeWork {
public:
    ScopedOwnerRuntimeWork() : m_active(IsOwnerThread()) {
        if (m_active) {
            ++g_runtime.ownerWorkDepth;
        }
    }

    ScopedOwnerRuntimeWork(const ScopedOwnerRuntimeWork&) = delete;
    ScopedOwnerRuntimeWork& operator=(const ScopedOwnerRuntimeWork&) = delete;

    ~ScopedOwnerRuntimeWork() {
        if (!m_active) {
            return;
        }

        if (g_runtime.ownerWorkDepth > 0) {
            --g_runtime.ownerWorkDepth;
        }

        if (g_runtime.ownerWorkDepth == 0) {
            ProcessOwnerControlRequests();
        }
    }

private:
    bool m_active = false;
};

void ClearWordMemberIdCache() {
    for (WordMemberIdCacheEntry& entry : g_runtime.wordMemberIdCache) {
        entry.dispId = DISPID_UNKNOWN;
        entry.resolved = false;
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

enum class DocumentPathTextComparison {
    Equal,
    Different,
    RequiresResolution,
};

DocumentPathTextComparison CompareDocumentPathText(const wchar_t* left,
                                                   const wchar_t* right);
bool AreSameDocumentPathText(const wchar_t* left, const wchar_t* right);
bool AreSameResolvedDocumentPath(const wchar_t* left, const wchar_t* right);
bool ReplaceStoredBstr(BSTR* storage, const wchar_t* value);
bool ReplaceStoredBstr(ScopedBstr* storage, const wchar_t* value);
bool ReplaceStoredTextBstr(BSTR* storage, const wchar_t* value);
bool ReplaceStoredTextBstr(ScopedBstr* storage, const wchar_t* value);
void LogSaveStatus(const wchar_t* message);
void LogDocumentStateStatus(const wchar_t* message);
bool AreModifiersOrMouseButtonsHeld();
HRESULT GetComIdentity(IUnknown* unknown, IUnknown** result);
HRESULT GetDocumentIdentityAndPathState(IDispatch* document,
                                        BSTR* result,
                                        bool* hasSavedPath);
HRESULT GetWordDocumentFromActiveWindow(IDispatch** document);
bool IsWindowInCurrentWordProcess(HWND hwnd);
void InvalidateTransitionFlushDocumentCache();
bool IsWordEventTeardownInProgress();
struct RuntimeFlagSnapshot;
DWORD GetSteadyDocumentStatePollDelay(const RuntimeFlagSnapshot* flags = nullptr);

enum class MessageAutosaveRole {
    None,
    EditingInput,
    TextKeyDownInput,
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
    EventDisconnectRetry,
};

enum class ScheduledTaskScheduleMode {
    ArmEarlier,
    Reschedule,
};

bool IsScheduledTaskArmed(ScheduledTaskKind taskKind);

enum class RuntimeResetMode {
    Shutdown,
    Reload,
};

bool ShouldPreserveDeferredWordEventDisconnects(RuntimeResetMode resetMode) {
    switch (resetMode) {
        case RuntimeResetMode::Reload:
            return true;

        case RuntimeResetMode::Shutdown:
            return false;
    }

    return false;
}

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

enum class HookMessageRoute {
    Ignore,
    AdoptOwner,
    HandleOnOwner,
};

bool IsAutosaveRelevantMessage(UINT message) {
    switch (message) {
        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
        case WM_CHAR:
        case WM_LBUTTONUP:
        case WM_RBUTTONUP:
        case WM_MBUTTONUP:
        case WM_XBUTTONUP:
        case WM_MOUSEWHEEL:
        case WM_MOUSEHWHEEL:
        case WM_COMMAND:
        case WM_PASTE:
        case WM_CUT:
        case WM_UNDO:
        case WM_CONTEXTMENU:
        case WM_ACTIVATE:
        case WM_ACTIVATEAPP:
        case WM_KILLFOCUS:
        case WM_SETFOCUS:
        case WM_NCACTIVATE:
        case WM_DESTROY:
        case WM_CLOSE:
        case WM_QUERYENDSESSION:
        case WM_ENDSESSION:
        case WM_SYSCOMMAND:
        case WM_IME_STARTCOMPOSITION:
        case WM_IME_ENDCOMPOSITION:
        case WM_EXITMENULOOP:
        case WM_CANCELMODE:
            return true;
    }

    return false;
}

HookMessageRoute SelectHookMessageRoute(bool relevantMessage,
                                        bool controlRequestPending,
                                        DWORD ownerThreadId,
                                        DWORD currentThreadId) {
    if (!relevantMessage && !controlRequestPending) {
        return HookMessageRoute::Ignore;
    }

    if (ownerThreadId == 0) {
        return HookMessageRoute::AdoptOwner;
    }

    return ownerThreadId == currentThreadId
               ? HookMessageRoute::HandleOnOwner
               : HookMessageRoute::Ignore;
}

bool ShouldAttemptOwnerThreadAdoptionForState(
    UINT message,
    bool hasOwnerThread,
    bool messageWindowInCurrentWordProcess,
    bool foregroundWindowInCurrentWordProcess) {
    if (IsOwnerCandidateMessage(message)) {
        return true;
    }

    if (hasOwnerThread) {
        return false;
    }

    return messageWindowInCurrentWordProcess || foregroundWindowInCurrentWordProcess;
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

SaveResumeSignal ClassifySaveResumeSignal(const MSG* lpMsg) {
    if (!lpMsg) {
        return SaveResumeSignal::None;
    }

    switch (lpMsg->message) {
        case WM_ACTIVATE:
            return LOWORD(lpMsg->wParam) != WA_INACTIVE
                       ? SaveResumeSignal::WordActivated
                       : SaveResumeSignal::None;

        case WM_ACTIVATEAPP:
        case WM_NCACTIVATE:
            return lpMsg->wParam != FALSE
                       ? SaveResumeSignal::WordActivated
                       : SaveResumeSignal::None;

        case WM_SETFOCUS:
            return SaveResumeSignal::UiReleased;

        case WM_IME_ENDCOMPOSITION:
        case WM_EXITMENULOOP:
        case WM_CANCELMODE:
            return SaveResumeSignal::UiReleased;

        case WM_LBUTTONUP:
        case WM_RBUTTONUP:
        case WM_MBUTTONUP:
        case WM_XBUTTONUP:
        case WM_MOUSEWHEEL:
        case WM_MOUSEHWHEEL:
        case WM_COMMAND:
        case WM_PASTE:
        case WM_CUT:
        case WM_UNDO:
        case WM_CONTEXTMENU:
            return SaveResumeSignal::ActionBoundary;
    }

    return SaveResumeSignal::None;
}

bool ShouldResumeSaveForSignal(SaveRetryReason reason,
                               SaveResumeSignal signal) {
    if (reason == SaveRetryReason::None || signal == SaveResumeSignal::None) {
        return false;
    }

    switch (signal) {
        case SaveResumeSignal::WordActivated:
        case SaveResumeSignal::DocumentChanged:
            return true;

        case SaveResumeSignal::UiReleased:
        case SaveResumeSignal::ActionBoundary:
            return reason == SaveRetryReason::AutomationBusy ||
                   reason == SaveRetryReason::UiPaused ||
                   reason == SaveRetryReason::InputBusy;

        case SaveResumeSignal::None:
            return false;
    }

    return false;
}

bool ShouldResumeDocumentForSignal(DocumentRetryReason reason,
                                   SaveResumeSignal signal) {
    if (reason == DocumentRetryReason::None ||
        signal == SaveResumeSignal::None) {
        return false;
    }

    switch (signal) {
        case SaveResumeSignal::WordActivated:
        case SaveResumeSignal::DocumentChanged:
            return true;

        case SaveResumeSignal::UiReleased:
        case SaveResumeSignal::ActionBoundary:
            return reason == DocumentRetryReason::Busy ||
                   reason == DocumentRetryReason::UiPaused ||
                   reason == DocumentRetryReason::InputBusy;

        case SaveResumeSignal::None:
            return false;
    }

    return false;
}

bool IsDocumentFailureRetryReason(DocumentRetryReason reason) {
    return reason == DocumentRetryReason::Busy ||
           reason == DocumentRetryReason::Disconnected ||
           reason == DocumentRetryReason::Hard;
}

bool ShouldWakeSaveForOrdinaryFallback(SaveRetryReason reason,
                                       bool saveTimerArmed) {
    return !saveTimerArmed ||
           reason == SaveRetryReason::None ||
           ShouldResumeSaveForSignal(reason, SaveResumeSignal::ActionBoundary);
}

bool ShouldWakeDocumentForOrdinaryFallback(DocumentRetryReason reason,
                                           bool documentTimerArmed) {
    return !documentTimerArmed ||
           reason == DocumentRetryReason::None ||
           ShouldResumeDocumentForSignal(reason,
                                         SaveResumeSignal::ActionBoundary);
}

bool ShouldApplyMessageAutosaveRole(MessageAutosaveRole role,
                                    bool pendingAutosave,
                                    SaveRetryReason saveReason,
                                    bool saveTimerArmed,
                                    DocumentRetryReason documentReason,
                                    bool documentTimerArmed) {
    switch (role) {
        case MessageAutosaveRole::TextKeyDownInput:
        case MessageAutosaveRole::EditingInput:
        case MessageAutosaveRole::TransitionBoundaryFallback:
            return true;

        case MessageAutosaveRole::DocumentRefreshFallback:
            return ShouldWakeDocumentForOrdinaryFallback(documentReason,
                                                         documentTimerArmed);

        case MessageAutosaveRole::ActionBoundaryFallback:
            return pendingAutosave
                       ? ShouldWakeSaveForOrdinaryFallback(saveReason,
                                                           saveTimerArmed)
                       : ShouldWakeDocumentForOrdinaryFallback(
                             documentReason,
                             documentTimerArmed);

        case MessageAutosaveRole::None:
            return false;
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

bool IsTerminalAutomationObjectFailure(HRESULT hr) {
    return hr == kCoEObjectNotConnected ||
           hr == RPC_E_DISCONNECTED ||
           hr == RPC_E_SERVER_DIED ||
           hr == RPC_E_SERVER_DIED_DNE ||
           hr == HRESULT_FROM_WIN32(RPC_S_SERVER_UNAVAILABLE);
}

AutomationFailureClass ClassifyAutomationFailure(HRESULT hr) {
    if (IsRetryableAutomationFailure(hr)) {
        return AutomationFailureClass::Busy;
    }

    if (IsTerminalAutomationObjectFailure(hr)) {
        return AutomationFailureClass::Disconnected;
    }

    return FAILED(hr) ? AutomationFailureClass::Hard
                      : AutomationFailureClass::None;
}

bool ShouldLogFailureNow(ULONGLONG* lastLogTime,
                         DWORD intervalMs) {
    if (!lastLogTime) {
        return true;
    }

    const ULONGLONG now = GetTickCount64();
    const ULONGLONG previous = *lastLogTime;
    if (previous != 0 && now - previous < intervalMs) {
        return false;
    }

    *lastLogTime = now;
    return true;
}

bool ShouldLogRetryReasonNow(int lastReason,
                             int reason,
                             ULONGLONG lastLogTime,
                             ULONGLONG now,
                             DWORD reminderIntervalMs) {
    if (reason == 0) {
        return false;
    }

    if (lastLogTime == 0) {
        return true;
    }

    const DWORD intervalMs = lastReason == reason
                                 ? reminderIntervalMs
                                 : RETRY_REASON_CHANGE_LOG_INTERVAL_MS;
    return now - lastLogTime >= intervalMs;
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

    if (!ReplaceStoredTextBstr(lastMessage, message)) {
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

    if (!ReplaceStoredTextBstr(lastMessage, message)) {
        return ShouldLogStatusNow(lastLogTime);
    }

    *lastLogTime = GetTickCount64();
    return true;
}

DWORD AdvanceRetryDelayForPolicy(DWORD* retryDelayMs,
                                 DWORD initialDelayMs,
                                 DWORD maxRetryDelayMs) {
    if (!retryDelayMs) {
        return initialDelayMs;
    }

    DWORD delayMs = *retryDelayMs;
    if (delayMs < initialDelayMs) {
        delayMs = initialDelayMs;
    }
    if (delayMs > maxRetryDelayMs) {
        delayMs = maxRetryDelayMs;
    }

    DWORD nextDelayMs = delayMs;
    if (nextDelayMs < maxRetryDelayMs) {
        nextDelayMs = nextDelayMs > maxRetryDelayMs / 2
                          ? maxRetryDelayMs
                          : nextDelayMs * 2;
        if (nextDelayMs > maxRetryDelayMs) {
            nextDelayMs = maxRetryDelayMs;
        }
    }

    *retryDelayMs = nextDelayMs;
    return delayMs;
}

DWORD GetComRetryBudgetMs(ComRetryProfile profile) {
    return profile == ComRetryProfile::Background
               ? COM_BACKGROUND_RETRY_BUDGET_MS
               : COM_MESSAGE_FILTER_CANCEL_AFTER_MS;
}

DWORD SelectRejectedCallRetryDelay(ComRetryProfile profile,
                                   DWORD rejectType,
                                   DWORD elapsedMs,
                                   ULONGLONG now,
                                   ULONGLONG deadline) {
    if (rejectType != SERVERCALL_RETRYLATER ||
        elapsedMs >= GetComRetryBudgetMs(profile) ||
        now >= deadline) {
        return static_cast<DWORD>(-1);
    }

    return profile == ComRetryProfile::Background
               ? COM_BACKGROUND_RETRY_DELAY_MS
               : COM_MESSAGE_FILTER_RETRY_DELAY_MS;
}

bool ReplaceStoredBstr(BSTR* storage, const wchar_t* value) {
    if (!storage) {
        return false;
    }

    const bool currentEmpty = !*storage || !**storage;
    const bool valueEmpty = !value || !*value;
    if (currentEmpty && valueEmpty) {
        return true;
    }
    if (!currentEmpty && !valueEmpty &&
        AreSameDocumentPathText(*storage, value)) {
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
    if (currentEmpty && valueEmpty) {
        return true;
    }
    if (!currentEmpty && !valueEmpty &&
        AreSameDocumentPathText(storage->Get(), value)) {
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

bool ReplaceStoredTextBstr(BSTR* storage, const wchar_t* value) {
    if (!storage) {
        return false;
    }

    const bool currentEmpty = !*storage || !**storage;
    const bool valueEmpty = !value || !*value;
    if ((currentEmpty && valueEmpty) ||
        (!currentEmpty && !valueEmpty && lstrcmpW(*storage, value) == 0)) {
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

bool ReplaceStoredTextBstr(ScopedBstr* storage, const wchar_t* value) {
    if (!storage) {
        return false;
    }

    const bool currentEmpty = !storage->Get() || !*storage->Get();
    const bool valueEmpty = !value || !*value;
    if ((currentEmpty && valueEmpty) ||
        (!currentEmpty && !valueEmpty && lstrcmpW(storage->Get(), value) == 0)) {
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

bool IsAsciiDriveLetter(wchar_t value) {
    return (value >= L'A' && value <= L'Z') ||
           (value >= L'a' && value <= L'z');
}

bool HasWin32NamespacePrefix(const wchar_t* path, DWORD pathLength) {
    return path &&
           pathLength >= 4 &&
           path[0] == L'\\' &&
           path[1] == L'\\' &&
           path[2] == L'?' &&
           path[3] == L'\\';
}

bool StoreNormalizedFinalPath(const wchar_t* finalPath, DWORD finalPathLength, ScopedBstr* result) {
    if (!finalPath || finalPathLength == 0 || !result) {
        return false;
    }

    if (HasWin32NamespacePrefix(finalPath, finalPathLength) &&
        finalPathLength > 8 &&
        finalPath[4] == L'U' &&
        finalPath[5] == L'N' &&
        finalPath[6] == L'C' &&
        finalPath[7] == L'\\') {
        const UINT normalizedLength = static_cast<UINT>(finalPathLength - 6);
        BSTR normalizedPath = SysAllocStringLen(nullptr, normalizedLength);
        if (!normalizedPath) {
            return false;
        }

        normalizedPath[0] = L'\\';
        normalizedPath[1] = L'\\';
        CopyMemory(normalizedPath + 2,
                   finalPath + 8,
                   (normalizedLength - 2) * sizeof(wchar_t));
        normalizedPath[normalizedLength] = L'\0';
        result->Reset(normalizedPath);
        return true;
    }

    const wchar_t* pathStart = finalPath;
    DWORD pathLength = finalPathLength;
    if (HasWin32NamespacePrefix(finalPath, finalPathLength) &&
        finalPathLength > 6 &&
        IsAsciiDriveLetter(finalPath[4]) &&
        finalPath[5] == L':') {
        pathStart = finalPath + 4;
        pathLength = finalPathLength - 4;
    }

    BSTR normalizedPath = SysAllocStringLen(pathStart, static_cast<UINT>(pathLength));
    if (!normalizedPath) {
        return false;
    }

    result->Reset(normalizedPath);
    return true;
}

bool AreSameNormalizedDocumentPathText(const wchar_t* left,
                                       const wchar_t* right) {
    const bool leftEmpty = !left || !*left;
    const bool rightEmpty = !right || !*right;
    if (leftEmpty || rightEmpty) {
        return leftEmpty == rightEmpty;
    }

    auto isSlash = [](wchar_t value) {
        return value == L'\\' || value == L'/';
    };
    const int rawLeftLength = lstrlenW(left);
    const int rawRightLength = lstrlenW(right);
    const bool hasWin32Namespace =
        HasWin32NamespacePrefix(left, static_cast<DWORD>(rawLeftLength)) ||
        HasWin32NamespacePrefix(right, static_cast<DWORD>(rawRightLength));
    auto normalizedLength = [&isSlash, hasWin32Namespace](const wchar_t* value,
                                                         int rawLength) {
        if (hasWin32Namespace) {
            return rawLength;
        }

        int length = rawLength;
        while (length > 3 && isSlash(value[length - 1])) {
            --length;
        }

        return length;
    };

    const int leftLength = normalizedLength(left, rawLeftLength);
    const int rightLength = normalizedLength(right, rawRightLength);
    if (leftLength != rightLength) {
        return false;
    }

    if (CompareStringOrdinal(left,
                             leftLength,
                             right,
                             rightLength,
                             TRUE) == CSTR_EQUAL) {
        return true;
    }

    // The extended Win32 namespace deliberately disables normal Win32 path
    // parsing. In particular, don't equate '/' with '\\' or remove a trailing
    // separator there. A filesystem-backed comparison can still prove that two
    // such spellings refer to the same file at explicitly rare call sites.
    if (hasWin32Namespace) {
        return false;
    }

    for (int index = 0; index < leftLength; ++index) {
        const wchar_t leftChar = isSlash(left[index]) ? L'\\' : left[index];
        const wchar_t rightChar = isSlash(right[index]) ? L'\\' : right[index];
        if (leftChar == rightChar) {
            continue;
        }

        if (CompareStringOrdinal(&leftChar, 1, &rightChar, 1, TRUE) != CSTR_EQUAL) {
            return false;
        }
    }

    return true;
}

DocumentPathTextComparison CompareDocumentPathText(const wchar_t* left,
                                                   const wchar_t* right) {
    if (AreSameNormalizedDocumentPathText(left, right)) {
        return DocumentPathTextComparison::Equal;
    }

    const bool leftEmpty = !left || !*left;
    const bool rightEmpty = !right || !*right;
    if (leftEmpty || rightEmpty) {
        return DocumentPathTextComparison::Different;
    }

    return DocumentPathTextComparison::RequiresResolution;
}

bool AreSameDocumentPathText(const wchar_t* left, const wchar_t* right) {
    return CompareDocumentPathText(left, right) ==
           DocumentPathTextComparison::Equal;
}

class GrowablePathBuffer {
public:
    GrowablePathBuffer() = default;

    GrowablePathBuffer(const GrowablePathBuffer&) = delete;
    GrowablePathBuffer& operator=(const GrowablePathBuffer&) = delete;

    ~GrowablePathBuffer() {
        delete[] m_data;
    }

    bool EnsureCapacity(DWORD requiredCapacity) {
        if (requiredCapacity == 0 ||
            requiredCapacity > MAX_CANONICAL_PATH_CHARS) {
            return false;
        }

        if (m_capacity >= requiredCapacity) {
            return true;
        }

        DWORD newCapacity = m_capacity != 0 ? m_capacity : MAX_PATH;
        while (newCapacity < requiredCapacity) {
            if (newCapacity > MAX_CANONICAL_PATH_CHARS / 2) {
                newCapacity = MAX_CANONICAL_PATH_CHARS;
            } else {
                newCapacity *= 2;
            }
        }

        wchar_t* replacement = new (std::nothrow) wchar_t[newCapacity];
        if (!replacement) {
            return false;
        }

        delete[] m_data;
        m_data = replacement;
        m_capacity = newCapacity;
        return true;
    }

    wchar_t* Data() const {
        return m_data;
    }

    DWORD Capacity() const {
        return m_capacity;
    }

private:
    wchar_t* m_data = nullptr;
    DWORD m_capacity = 0;
};

DWORD GetNextPathBufferCapacity(DWORD returnedLength,
                                DWORD currentCapacity) {
    if (returnedLength > MAX_CANONICAL_PATH_CHARS ||
        currentCapacity > MAX_CANONICAL_PATH_CHARS) {
        return 0;
    }

    if (returnedLength > currentCapacity) {
        return returnedLength;
    }

    if (currentCapacity >= MAX_CANONICAL_PATH_CHARS) {
        return 0;
    }

    return currentCapacity + 1;
}

bool TryGetFullPathText(const wchar_t* path,
                        GrowablePathBuffer* buffer,
                        DWORD* pathLength) {
    if (!path || !*path || !buffer || !pathLength ||
        !buffer->EnsureCapacity(MAX_PATH)) {
        return false;
    }

    for (;;) {
        const DWORD length = GetFullPathNameW(path,
                                              buffer->Capacity(),
                                              buffer->Data(),
                                              nullptr);
        if (length == 0) {
            return false;
        }

        if (length < buffer->Capacity()) {
            *pathLength = length;
            return true;
        }

        const DWORD requiredCapacity =
            GetNextPathBufferCapacity(length, buffer->Capacity());
        if (!buffer->EnsureCapacity(requiredCapacity)) {
            return false;
        }
    }
}

bool TryGetFinalPathText(HANDLE file,
                         GrowablePathBuffer* buffer,
                         DWORD* pathLength) {
    if (!file || file == INVALID_HANDLE_VALUE || !buffer || !pathLength ||
        !buffer->EnsureCapacity(MAX_PATH)) {
        return false;
    }

    for (;;) {
        const DWORD length = GetFinalPathNameByHandleW(
            file,
            buffer->Data(),
            buffer->Capacity(),
            FILE_NAME_NORMALIZED | VOLUME_NAME_DOS);
        if (length == 0) {
            return false;
        }

        if (length < buffer->Capacity()) {
            *pathLength = length;
            return true;
        }

        const DWORD requiredCapacity =
            GetNextPathBufferCapacity(length, buffer->Capacity());
        if (!buffer->EnsureCapacity(requiredCapacity)) {
            return false;
        }
    }
}

bool TryGetResolvedDocumentPath(const wchar_t* path,
                                ScopedBstr* result,
                                GrowablePathBuffer* buffer) {
    if (!path || !*path || !result || !buffer) {
        return false;
    }

    result->Reset();

    DWORD fullPathLength = 0;
    if (!TryGetFullPathText(path, buffer, &fullPathLength)) {
        return false;
    }

    ScopedHandle file(CreateFileW(buffer->Data(),
                                  0,
                                  FILE_SHARE_READ | FILE_SHARE_WRITE |
                                      FILE_SHARE_DELETE,
                                  nullptr,
                                  OPEN_EXISTING,
                                  FILE_ATTRIBUTE_NORMAL |
                                      FILE_FLAG_BACKUP_SEMANTICS,
                                  nullptr));
    if (!file.IsValid()) {
        return false;
    }

    DWORD finalPathLength = 0;
    if (!TryGetFinalPathText(file.Get(), buffer, &finalPathLength)) {
        return false;
    }

    return StoreNormalizedFinalPath(buffer->Data(), finalPathLength, result);
}

bool AreSameResolvedDocumentPath(const wchar_t* left, const wchar_t* right) {
    const DocumentPathTextComparison comparison =
        CompareDocumentPathText(left, right);
    if (comparison == DocumentPathTextComparison::Equal) {
        return true;
    }

    if (comparison == DocumentPathTextComparison::Different) {
        return false;
    }

    GrowablePathBuffer buffer;
    ScopedBstr resolvedLeft;
    ScopedBstr resolvedRight;
    if (!TryGetResolvedDocumentPath(left, &resolvedLeft, &buffer) ||
        !TryGetResolvedDocumentPath(right, &resolvedRight, &buffer)) {
        return false;
    }

    return AreSameDocumentPathText(resolvedLeft.CStr(), resolvedRight.CStr());
}

struct DetachedDocumentBinding {
    // Declaration order keeps the identity alive until after document Release.
    ScopedComPtr<IUnknown> identity;
    ScopedComPtr<IDispatch> document;
};

void DetachDocumentBinding(ScopedComPtr<IDispatch>* document,
                           ScopedComPtr<IUnknown>* identity,
                           DetachedDocumentBinding* detached) {
    if (!document || !identity || !detached) {
        return;
    }

    detached->identity.Reset(identity->Detach());
    detached->document.Reset(document->Detach());
}

struct PreparedTransitionFlushTarget {
    ScopedBstr path;
    ScopedComPtr<IUnknown> identity;
    ScopedComPtr<IDispatch> document;
};

enum class TransitionFlushTargetPreparation {
    Ready,
    Failed,
    Superseded,
};

TransitionFlushTargetPreparation PrepareTransitionFlushTarget(
    const wchar_t* path,
    IDispatch* document,
    unsigned int expectedGeneration,
    PreparedTransitionFlushTarget* target) {
    if (!target || ((!path || !*path) && !document)) {
        return TransitionFlushTargetPreparation::Failed;
    }

    if (path && *path && !ReplaceStoredBstr(&target->path, path)) {
        return TransitionFlushTargetPreparation::Failed;
    }

    if (document) {
        ReplaceStoredComPtr(&target->document, document);
        const HRESULT hr =
            GetComIdentity(document, target->identity.Put());
        if (g_runtime.document.transitionFlushGeneration !=
            expectedGeneration) {
            return TransitionFlushTargetPreparation::Superseded;
        }

        if (FAILED(hr) || !target->identity) {
            // A path remains a valid recovery target even when the optional
            // strong COM binding couldn't be prepared.
            target->document.Reset();
            target->identity.Reset();
            if (target->path.Length() == 0) {
                return TransitionFlushTargetPreparation::Failed;
            }
        }
    }

    return g_runtime.document.transitionFlushGeneration == expectedGeneration
               ? TransitionFlushTargetPreparation::Ready
               : TransitionFlushTargetPreparation::Superseded;
}

void ClearManualSavePending() {
    ClearFlag(g_runtime.flags.manualSavePending);
}

void ClearExpeditedSavePending() {
    ClearFlag(g_runtime.flags.expeditedSavePending);
}

void ClearPostTransitionRefreshPending() {
    ClearFlag(g_runtime.flags.postTransitionRefreshPending);
}

enum class TransitionFlushClearMode {
    ClearPostTransitionRefresh,
    PreservePostTransitionRefresh,
};

void ClearTransitionFlushRequest(
    TransitionFlushClearMode clearMode =
        TransitionFlushClearMode::ClearPostTransitionRefresh) {
    const bool clearPostTransitionRefresh =
        clearMode == TransitionFlushClearMode::ClearPostTransitionRefresh;
    const bool hasTransitionState =
        LoadFlag(g_runtime.flags.transitionFlushPending) ||
        g_runtime.timing.transitionFlushRequestTime != 0 ||
        g_runtime.document.transitionFlushDocumentPath.Get() != nullptr ||
        g_runtime.document.transitionFlushDocument.Get() != nullptr ||
        g_runtime.document.transitionFlushDocumentIdentity.Get() != nullptr;
    if (!hasTransitionState &&
        (!clearPostTransitionRefresh ||
         !LoadFlag(g_runtime.flags.postTransitionRefreshPending))) {
        return;
    }

    // transitionFlushPending is the commit marker. Withdraw it before
    // changing payload and before any COM Release.
    ClearFlag(g_runtime.flags.transitionFlushPending);
    ++g_runtime.document.transitionFlushGeneration;
    g_runtime.timing.transitionFlushRequestTime = 0;
    if (clearPostTransitionRefresh) {
        ClearPostTransitionRefreshPending();
    }

    ScopedBstr previousPath;
    DetachedDocumentBinding previousBinding;
    previousPath.Reset(
        g_runtime.document.transitionFlushDocumentPath.Detach());
    DetachDocumentBinding(
        &g_runtime.document.transitionFlushDocument,
        &g_runtime.document.transitionFlushDocumentIdentity,
        &previousBinding);
}

bool HasPendingSaveWork() {
    return LoadFlag(g_runtime.flags.pendingSave) ||
           LoadFlag(g_runtime.flags.manualSavePending) ||
           LoadFlag(g_runtime.flags.documentDirty);
}

bool HasPendingAutosave() {
    return LoadFlag(g_runtime.flags.pendingSave);
}

void RefreshAutomationBusyPendingFromRetryState() {
    const bool automationBusy =
        g_runtime.saveRetry.reason == SaveRetryReason::AutomationBusy ||
        g_runtime.documentRetry.reason == DocumentRetryReason::Busy;
    StoreFlag(g_runtime.flags.automationBusyPending, automationBusy);
}

DWORD GetSaveRetryReasonLogInterval(SaveRetryReason reason) {
    switch (reason) {
        case SaveRetryReason::AutomationBusy:
            return RETRY_BUSY_LOG_INTERVAL_MS;

        case SaveRetryReason::HardFailure:
            return RETRY_HARD_LOG_INTERVAL_MS;

        case SaveRetryReason::TargetUnavailable:
        case SaveRetryReason::DeferredProtectedView:
        case SaveRetryReason::InactiveWindow:
        case SaveRetryReason::UiPaused:
        case SaveRetryReason::InputBusy:
            return RETRY_WAIT_LOG_INTERVAL_MS;

        case SaveRetryReason::None:
            return RETRY_WAIT_LOG_INTERVAL_MS;
    }

    return RETRY_WAIT_LOG_INTERVAL_MS;
}

const wchar_t* DescribeSaveRetryReason(SaveRetryReason reason) {
    switch (reason) {
        case SaveRetryReason::AutomationBusy:
            return L"Word automation is busy; pending auto-save target is retained";
        case SaveRetryReason::HardFailure:
            return L"auto-save failed; pending changes are retained for a slower retry";
        case SaveRetryReason::TargetUnavailable:
            return L"auto-save target is temporarily unavailable; pending target is retained";
        case SaveRetryReason::DeferredProtectedView:
            return L"auto-save is deferred while Word is in Protected View";
        case SaveRetryReason::InactiveWindow:
            return L"auto-save is waiting for a Word document window to become active";
        case SaveRetryReason::UiPaused:
            return L"auto-save is waiting for Word UI interaction to finish";
        case SaveRetryReason::InputBusy:
            return L"auto-save is waiting for input to settle";
        case SaveRetryReason::None:
            return L"";
    }

    return L"";
}

const wchar_t* DescribeDocumentRetryReason(DocumentRetryReason reason) {
    switch (reason) {
        case DocumentRetryReason::Busy:
            return L"Word automation is busy; document-state refresh will retry";
        case DocumentRetryReason::Disconnected:
            return L"Word automation target changed or disconnected; document-state refresh will recover";
        case DocumentRetryReason::Hard:
            return L"document-state refresh failed; using a slower retry";
        case DocumentRetryReason::InactiveWindow:
            return L"document-state refresh is waiting for a Word document window";
        case DocumentRetryReason::UiPaused:
            return L"document-state refresh is waiting for Word UI interaction to finish";
        case DocumentRetryReason::InputBusy:
            return L"document-state refresh is waiting for input to settle";
        case DocumentRetryReason::None:
            return L"";
    }

    return L"";
}

void LogSaveRetryReasonIfNeeded(SaveRetryReason reason) {
    RuntimeSaveRetryState* retry = &g_runtime.saveRetry;
    const ULONGLONG now = GetTickCount64();
    if (!ShouldLogRetryReasonNow(
            static_cast<int>(retry->lastLoggedReason),
            static_cast<int>(reason),
            retry->lastReasonLogTime,
            now,
            GetSaveRetryReasonLogInterval(reason))) {
        return;
    }

    retry->lastLoggedReason = reason;
    retry->lastReasonLogTime = now;
    Wh_Log(L"Auto-save retry: %ls", DescribeSaveRetryReason(reason));
}

void LogDocumentRetryReasonIfNeeded(DocumentRetryReason reason) {
    RuntimeDocumentRetryState* retry = &g_runtime.documentRetry;
    const ULONGLONG now = GetTickCount64();
    const DWORD reminderInterval =
        reason == DocumentRetryReason::Busy
            ? RETRY_BUSY_LOG_INTERVAL_MS
            : (reason == DocumentRetryReason::Hard
                   ? RETRY_HARD_LOG_INTERVAL_MS
                   : RETRY_WAIT_LOG_INTERVAL_MS);
    if (!ShouldLogRetryReasonNow(
            static_cast<int>(retry->lastLoggedReason),
            static_cast<int>(reason),
            retry->lastReasonLogTime,
            now,
            reminderInterval)) {
        return;
    }

    retry->lastLoggedReason = reason;
    retry->lastReasonLogTime = now;
    Wh_Log(L"Document state retry: %ls", DescribeDocumentRetryReason(reason));
}

void SetCurrentSaveRetryReason(SaveRetryReason reason) {
    g_runtime.saveRetry.reason = reason;
    RefreshAutomationBusyPendingFromRetryState();
    LogSaveRetryReasonIfNeeded(reason);
}

void SetCurrentDocumentRetryReason(DocumentRetryReason reason) {
    g_runtime.documentRetry.reason = reason;
    RefreshAutomationBusyPendingFromRetryState();
    LogDocumentRetryReasonIfNeeded(reason);
}

void ClearCurrentSaveRetryReason() {
    if (g_runtime.saveRetry.reason == SaveRetryReason::None) {
        return;
    }

    g_runtime.saveRetry.reason = SaveRetryReason::None;
    RefreshAutomationBusyPendingFromRetryState();
}

void ClearCurrentDocumentRetryReason() {
    if (g_runtime.documentRetry.reason == DocumentRetryReason::None) {
        return;
    }

    g_runtime.documentRetry.reason = DocumentRetryReason::None;
    RefreshAutomationBusyPendingFromRetryState();
}

void ResetRuntimeSaveRetryState(RuntimeSaveRetryState* retry) {
    if (retry) {
        *retry = RuntimeSaveRetryState{};
    }
}

void ResetRuntimeDocumentRetryState(RuntimeDocumentRetryState* retry) {
    if (retry) {
        *retry = RuntimeDocumentRetryState{};
    }
}

void ResetSaveRetryState() {
    ResetRuntimeSaveRetryState(&g_runtime.saveRetry);
    RefreshAutomationBusyPendingFromRetryState();
}

void ResetDocumentRetryState() {
    ResetRuntimeDocumentRetryState(&g_runtime.documentRetry);
    RefreshAutomationBusyPendingFromRetryState();
}

DWORD AdvanceSaveRetryDelay(SaveRetryReason reason) {
    RuntimeSaveRetryState* retry = &g_runtime.saveRetry;
    SetCurrentSaveRetryReason(reason);

    switch (reason) {
        case SaveRetryReason::AutomationBusy:
            return AdvanceRetryDelayForPolicy(
                &retry->automationNextDelayMs,
                SAVE_AUTOMATION_RETRY_INITIAL_MS,
                SAVE_AUTOMATION_RETRY_MAX_MS);

        case SaveRetryReason::HardFailure:
            return AdvanceRetryDelayForPolicy(&retry->hardNextDelayMs,
                                              SAVE_HARD_RETRY_INITIAL_MS,
                                              SAVE_HARD_RETRY_MAX_MS);

        case SaveRetryReason::TargetUnavailable:
            return AdvanceRetryDelayForPolicy(&retry->targetNextDelayMs,
                                              SAVE_TARGET_RETRY_INITIAL_MS,
                                              SAVE_TARGET_RETRY_MAX_MS);

        case SaveRetryReason::DeferredProtectedView:
            return SAVE_DEFERRED_WATCHDOG_MS;

        case SaveRetryReason::InactiveWindow:
            return SAVE_INACTIVE_FALLBACK_WATCHDOG_MS;

        case SaveRetryReason::UiPaused:
        case SaveRetryReason::InputBusy:
            return SAVE_UI_INPUT_WATCHDOG_MS;

        case SaveRetryReason::None:
            return MIN_RETRY_INTERVAL_MS;
    }

    return MIN_RETRY_INTERVAL_MS;
}

DWORD AdvanceDocumentRetryDelay(DocumentRetryReason reason) {
    RuntimeDocumentRetryState* retry = &g_runtime.documentRetry;
    SetCurrentDocumentRetryReason(reason);

    switch (reason) {
        case DocumentRetryReason::Busy:
            return AdvanceRetryDelayForPolicy(&retry->busyNextDelayMs,
                                              DOCUMENT_BUSY_RETRY_INITIAL_MS,
                                              DOCUMENT_BUSY_RETRY_MAX_MS);

        case DocumentRetryReason::Disconnected:
            return AdvanceRetryDelayForPolicy(
                &retry->disconnectedNextDelayMs,
                DOCUMENT_TARGET_RETRY_INITIAL_MS,
                DOCUMENT_TARGET_RETRY_MAX_MS);

        case DocumentRetryReason::Hard:
            return AdvanceRetryDelayForPolicy(&retry->hardNextDelayMs,
                                              DOCUMENT_HARD_RETRY_INITIAL_MS,
                                              DOCUMENT_HARD_RETRY_MAX_MS);

        case DocumentRetryReason::InactiveWindow:
            return SAVE_INACTIVE_FALLBACK_WATCHDOG_MS;

        case DocumentRetryReason::UiPaused:
        case DocumentRetryReason::InputBusy:
            return SAVE_UI_INPUT_WATCHDOG_MS;

        case DocumentRetryReason::None:
            return MIN_RETRY_INTERVAL_MS;
    }

    return MIN_RETRY_INTERVAL_MS;
}

void ClearPendingSaveAsMigration() {
    ClearFlag(g_runtime.flags.pendingSaveAsMigration);
    ++g_runtime.document.pendingSaveAsGeneration;
    g_runtime.timing.pendingSaveAsTime = 0;

    ScopedComPtr<IUnknown> previousIdentity;
    previousIdentity.Reset(
        g_runtime.document.pendingSaveAsDocumentIdentity.Detach());
}

bool HasPendingSaveAsMigration() {
    return LoadFlag(g_runtime.flags.pendingSaveAsMigration);
}

bool HasPendingWordEventDisconnectRetry() {
    return LoadFlag(g_runtime.flags.wordEventDisconnectRetryPending);
}

bool HasActiveDocumentStatePollWorkForState(bool pendingSaveWork,
                                            bool automationBusyPending,
                                            bool pendingSaveAsMigration) {
    return pendingSaveWork || automationBusyPending || pendingSaveAsMigration;
}

bool HasActiveDocumentStatePollWork() {
    return HasActiveDocumentStatePollWorkForState(
        HasPendingSaveWork(),
        LoadFlag(g_runtime.flags.automationBusyPending),
        HasPendingSaveAsMigration());
}

bool HasTransitionFlushTarget(const wchar_t* path, IDispatch* document) {
    return (path && *path) || document;
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

bool ShouldUseMessageDocumentRefreshFallback() {
    return true;
}

bool ShouldUseMessageBoundaryFallback(bool wordEventsConnected, bool transitionFlush) {
    return transitionFlush || !wordEventsConnected;
}

void ResetObservedDocumentState();

void ResetObservedDocumentState() {
    // documentDirtyKnown is the commit marker for the observed payload.
    ClearFlag(g_runtime.flags.documentDirtyKnown);
    ClearFlag(g_runtime.flags.documentDirty);
    ++g_runtime.document.observedDocumentGeneration;

    ScopedBstr previousPath;
    DetachedDocumentBinding previousBinding;
    previousPath.Reset(g_runtime.document.observedDocumentPath.Detach());
    DetachDocumentBinding(&g_runtime.document.observedDocument,
                          &g_runtime.document.observedDocumentIdentity,
                          &previousBinding);
}

void RequestTransitionFlush(const wchar_t* path,
                           const wchar_t* reason,
                           IDispatch* document = nullptr) {
    if (!HasTransitionFlushTarget(path, document)) {
        return;
    }

    const unsigned int expectedGeneration =
        g_runtime.document.transitionFlushGeneration;
    PreparedTransitionFlushTarget target;
    const TransitionFlushTargetPreparation preparation =
        PrepareTransitionFlushTarget(path,
                                     document,
                                     expectedGeneration,
                                     &target);
    if (preparation == TransitionFlushTargetPreparation::Superseded) {
        return;
    }

    if (preparation == TransitionFlushTargetPreparation::Failed) {
        ClearTransitionFlushRequest();
        return;
    }

    if (g_runtime.document.transitionFlushGeneration != expectedGeneration) {
        return;
    }

    const bool wasPending =
        LoadFlag(g_runtime.flags.transitionFlushPending);
    const ULONGLONG previousRequestTime =
        g_runtime.timing.transitionFlushRequestTime;

    // Withdraw the old commit marker, replace every payload field without a
    // COM call, then publish the new marker last.
    ClearFlag(g_runtime.flags.transitionFlushPending);
    ScopedBstr previousPath;
    DetachedDocumentBinding previousBinding;
    previousPath.Reset(
        g_runtime.document.transitionFlushDocumentPath.Detach());
    DetachDocumentBinding(
        &g_runtime.document.transitionFlushDocument,
        &g_runtime.document.transitionFlushDocumentIdentity,
        &previousBinding);

    g_runtime.document.transitionFlushDocumentPath.Reset(
        target.path.Detach());
    g_runtime.document.transitionFlushDocumentIdentity.Reset(
        target.identity.Detach());
    g_runtime.document.transitionFlushDocument.Reset(
        target.document.Detach());
    ++g_runtime.document.transitionFlushGeneration;

    ClearCurrentSaveRetryReason();
    g_runtime.timing.transitionFlushRequestTime =
        wasPending && previousRequestTime != 0
            ? previousRequestTime
            : GetTickCount64();
    SetFlag(g_runtime.flags.expeditedSavePending);
    SetFlag(g_runtime.flags.transitionFlushPending);
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
    bool saveTaskArmed = false;
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
    const DWORD ownerThreadId = LoadOwnerThreadId();
    return ownerThreadId != 0 && ownerThreadId == GetCurrentThreadId();
}

bool CanRunOwnerThreadRuntimeWork() {
    return IsOwnerThreadSchedulerContextValid() &&
           LoadFlag(g_runtime.flags.moduleActive);
}

bool ShouldTrackDocumentStateWhileInactive(bool pendingSaveWork,
                                           bool pendingSaveAsMigration,
                                           bool wordEventsConnected) {
    return pendingSaveWork ||
           pendingSaveAsMigration ||
           !wordEventsConnected;
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
    snapshot.saveTaskArmed = IsScheduledTaskArmed(ScheduledTaskKind::Save);
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

    snapshot.flags.documentDirtyKnown =
        LoadFlag(g_runtime.flags.documentDirtyKnown);
    snapshot.flags.documentDirty =
        LoadFlag(g_runtime.flags.documentDirty);
    snapshot.flags.pendingSaveAsMigration =
        LoadFlag(g_runtime.flags.pendingSaveAsMigration);
    snapshot.runtime.wordEventsConnected =
        LoadFlag(g_runtime.flags.wordEventsConnected);
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

ULONGLONG GetAutosaveDelayBaseTime(bool transitionFlushRequested) {
    if (transitionFlushRequested &&
        g_runtime.timing.transitionFlushRequestTime != 0) {
        return g_runtime.timing.transitionFlushRequestTime;
    }

    return g_runtime.timing.lastEditTime;
}

DWORD GetOwnerThreadSyncWatchdogDelay(bool criticalWork,
                                      bool wordEventsConnected) {
    if (criticalWork) {
        return DOCUMENT_STATE_PENDING_WATCHDOG_MS;
    }

    return wordEventsConnected ? SAVE_INACTIVE_EVENT_WATCHDOG_MS
                               : SAVE_INACTIVE_FALLBACK_WATCHDOG_MS;
}

TickDecision EvaluateDocumentStateTickDecision(const RuntimeTickSnapshot& snapshot) {
    const bool criticalObservation =
        snapshot.runtime.transitionFlushRequested ||
        snapshot.runtime.expeditedSaveRequested;
    if (!snapshot.runtime.ownerThreadSynchronized) {
        return MakeDocumentStateRearmDecision(
            RuntimeStatePhase::WaitingForOwnerThread,
            GetOwnerThreadSyncWatchdogDelay(
                criticalObservation,
                snapshot.runtime.wordEventsConnected));
    }

    const bool canYieldToArmedSave =
        snapshot.runtime.pendingSave &&
        snapshot.saveTaskArmed &&
        !snapshot.runtime.manualSavePending &&
        !snapshot.pendingSaveAsMigration;
    if (canYieldToArmedSave &&
        (snapshot.runtime.pauseReason != WordUiPauseReason::None ||
         !snapshot.runtime.activeWordDocumentWindow ||
         snapshot.runtime.inputBusy)) {
        return {RuntimeStatePhase::WaitingForSaveDelay, TickDecisionAction::None, 0};
    }

    if (snapshot.runtime.pauseReason != WordUiPauseReason::None) {
        return MakeDocumentStateRearmDecision(RuntimeStatePhase::WaitingForWordUi,
                                              criticalObservation
                                                  ? GetWordUiPauseDelay(
                                                        snapshot.runtime.pauseReason)
                                                  : SAVE_UI_INPUT_WATCHDOG_MS);
    }

    if (!snapshot.runtime.activeWordDocumentWindow) {
        if (ShouldTrackDocumentStateWhileInactive(snapshot.pendingSaveWork,
                                                  snapshot.pendingSaveAsMigration,
                                                  snapshot.runtime.wordEventsConnected)) {
            return MakeDocumentStateRearmDecision(RuntimeStatePhase::WaitingForWordWindow,
                                                  criticalObservation
                                                      ? snapshot.steadyPollDelay
                                                      : (snapshot.runtime.wordEventsConnected
                                                             ? SAVE_INACTIVE_EVENT_WATCHDOG_MS
                                                             : SAVE_INACTIVE_FALLBACK_WATCHDOG_MS));
        }

        return {RuntimeStatePhase::Idle, TickDecisionAction::None, 0};
    }

    if (snapshot.runtime.inputBusy) {
        return MakeDocumentStateRearmDecision(RuntimeStatePhase::WaitingForInputToSettle,
                                              criticalObservation
                                                  ? INPUT_SETTLE_DELAY_MS
                                                  : SAVE_UI_INPUT_WATCHDOG_MS);
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
        return MakeSaveRearmDecision(
            RuntimeStatePhase::WaitingForOwnerThread,
            GetOwnerThreadSyncWatchdogDelay(
                snapshot.runtime.transitionFlushRequested ||
                    snapshot.runtime.expeditedSaveRequested,
                snapshot.runtime.wordEventsConnected));
    }

    if (snapshot.runtime.pauseReason != WordUiPauseReason::None) {
        return MakeSaveRearmDecision(RuntimeStatePhase::WaitingForWordUi,
                                     snapshot.runtime.transitionFlushRequested
                                         ? GetWordUiPauseDelay(snapshot.runtime.pauseReason)
                                         : SAVE_UI_INPUT_WATCHDOG_MS);
    }

    if (!snapshot.runtime.transitionFlushRequested &&
        !snapshot.runtime.activeWordDocumentWindow) {
        return MakeSaveRearmDecision(RuntimeStatePhase::WaitingForWordWindow,
                                     snapshot.runtime.wordEventsConnected
                                         ? SAVE_INACTIVE_EVENT_WATCHDOG_MS
                                         : SAVE_INACTIVE_FALLBACK_WATCHDOG_MS);
    }

    const DWORD effectiveDelayMs =
        snapshot.runtime.expeditedSaveRequested || snapshot.runtime.transitionFlushRequested
            ? INPUT_SETTLE_DELAY_MS
            : static_cast<DWORD>(g_settings.saveDelay);
    const ULONGLONG earliestEditSaveTime =
        GetAutosaveDelayBaseTime(snapshot.runtime.transitionFlushRequested) +
        static_cast<ULONGLONG>(effectiveDelayMs);
    if (snapshot.now < earliestEditSaveTime) {
        return MakeSaveRearmDecision(RuntimeStatePhase::WaitingForSaveDelay,
                                     static_cast<DWORD>(earliestEditSaveTime - snapshot.now));
    }

    if (!snapshot.runtime.transitionFlushRequested &&
        g_settings.minTimeBetweenSaves > 0 &&
        g_runtime.timing.lastSaveTime > 0) {
        const ULONGLONG earliestAllowedSave =
            g_runtime.timing.lastSaveTime + static_cast<ULONGLONG>(g_settings.minTimeBetweenSaves);
        if (snapshot.now < earliestAllowedSave) {
            return MakeSaveRearmDecision(RuntimeStatePhase::WaitingForMinSaveInterval,
                                         static_cast<DWORD>(earliestAllowedSave - snapshot.now));
        }
    }

    if (snapshot.runtime.inputBusy) {
        return MakeSaveRearmDecision(RuntimeStatePhase::WaitingForInputToSettle,
                                     snapshot.runtime.transitionFlushRequested
                                         ? INPUT_SETTLE_DELAY_MS
                                         : SAVE_UI_INPUT_WATCHDOG_MS);
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
bool RescheduleSaveTimer(DWORD delayMs);
void ReconcileDocumentStateSchedule();
void CancelSchedulerTimer();
bool EnsureWordApplicationEventsConnected(bool forceReconnect = false);
bool DisconnectWordApplicationEvents(bool allowDeferredRetry = true);
void RetryPendingWordEventDisconnects();
bool ArmPendingWordEventDisconnectRetry();
void HandleSchedulerTimerMessage(UINT_PTR timerId);
struct ActiveDocumentSnapshot;
enum class SaveAttemptResult;

bool EnsureSchedulerMessageWindow() {
    if (!IsOwnerThread()) {
        return false;
    }

    HWND messageWindow = g_control.messageWindow.load(std::memory_order_acquire);
    if (messageWindow && IsWindow(messageWindow) &&
        GetWindowThreadProcessId(messageWindow, nullptr) == GetCurrentThreadId()) {
        return true;
    }

    if (messageWindow) {
        return false;
    }

    messageWindow = CreateWindowExW(0,
                                    L"STATIC",
                                    L"Windhawk.WordLocalAutoSave.Control",
                                    0,
                                    0,
                                    0,
                                    0,
                                    0,
                                    HWND_MESSAGE,
                                    nullptr,
                                    GetModuleHandleW(nullptr),
                                    nullptr);
    if (!messageWindow) {
        Wh_Log(L"Scheduler: failed to create the owner message window, error=%lu",
               GetLastError());
        return false;
    }

    g_control.messageWindow.store(messageWindow, std::memory_order_release);
    return true;
}

void DestroySchedulerMessageWindow() {
    HWND messageWindow =
        g_control.messageWindow.load(std::memory_order_acquire);
    if (!messageWindow) {
        return;
    }

    DWORD windowThreadId = GetWindowThreadProcessId(messageWindow, nullptr);
    if (windowThreadId != GetCurrentThreadId()) {
        Wh_Log(L"Scheduler: refusing to destroy the owner message window from another thread");
        return;
    }

    if (!g_control.messageWindow.compare_exchange_strong(
            messageWindow,
            nullptr,
            std::memory_order_acq_rel,
            std::memory_order_acquire)) {
        return;
    }

    DestroyWindow(messageWindow);
}

bool ShouldTryOwnerThreadAdoptionForMessage(const MSG* lpMsg) {
    if (!lpMsg) {
        return false;
    }

    const bool hasOwnerThread = LoadOwnerThreadId() != 0;
    const bool messageWindowInCurrentWordProcess =
        lpMsg->hwnd && IsWindowInCurrentWordProcess(lpMsg->hwnd);
    bool foregroundWindowInCurrentWordProcess = false;
    if (!messageWindowInCurrentWordProcess && !hasOwnerThread &&
        !IsOwnerCandidateMessage(lpMsg->message)) {
        HWND foregroundWindow = GetForegroundWindow();
        foregroundWindowInCurrentWordProcess =
            foregroundWindow && IsWindowInCurrentWordProcess(foregroundWindow);
    }

    return ShouldAttemptOwnerThreadAdoptionForState(
        lpMsg->message,
        hasOwnerThread,
        messageWindowInCurrentWordProcess,
        foregroundWindowInCurrentWordProcess);
}

void AdoptOwnerThreadIfNeeded(const MSG* lpMsg) {
    if (!lpMsg) {
        return;
    }

    const DWORD currentThreadId = GetCurrentThreadId();
    const DWORD previousOwnerThreadId = LoadOwnerThreadId();
    if (previousOwnerThreadId != 0) {
        // The runtime is intentionally bound to one STA owner for its complete
        // lifetime. Steady owner messages never need HWND/native discovery,
        // and a different UI thread cannot take over without explicit COM
        // marshaling.
        return;
    }

    if (!ShouldTryOwnerThreadAdoptionForMessage(lpMsg)) {
        return;
    }

    const DWORD preferredThreadId = ResolvePreferredOwnerThreadId(lpMsg);
    if (preferredThreadId == 0 || currentThreadId != preferredThreadId) {
        return;
    }

    LONG expectedLifecycle = RuntimeControlAwaitingOwner;
    if (!g_control.lifecycle.compare_exchange_strong(
            expectedLifecycle,
            RuntimeControlOwnerActive,
            std::memory_order_acq_rel,
            std::memory_order_acquire)) {
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

    CancelSchedulerTimer();
    g_runtime.timing.saveTimerDueTime = 0;
    g_runtime.timing.documentStateTimerDueTime = 0;
    ResetSaveRetryState();
    ResetDocumentRetryState();

    ExchangeOwnerThreadId(currentThreadId);
    ScopedOwnerRuntimeWork ownerWork;
    if (!EnsureSchedulerMessageWindow()) {
        ClearOwnerThreadId();
        LONG expectedLifecycle = RuntimeControlOwnerActive;
        if (!g_control.lifecycle.compare_exchange_strong(
                expectedLifecycle,
                RuntimeControlAwaitingOwner,
                std::memory_order_acq_rel,
                std::memory_order_acquire) &&
            expectedLifecycle == RuntimeControlStopRequested) {
            SignalOwnerShutdownComplete();
        }
        return;
    }

    g_control.ownerRuntimeLive.store(true, std::memory_order_release);
    ProcessOwnerControlRequests();
    if (g_control.shutdownRequested.load(std::memory_order_acquire)) {
        return;
    }
    if (!CanRunOwnerThreadRuntimeWork()) {
        return;
    }

    EnsureWordApplicationEventsConnected(true);
    ArmDocumentStateTimer(INPUT_SETTLE_DELAY_MS);
    if (LoadFlag(g_runtime.flags.pendingSave)) {
        ArmSaveTimer(INPUT_SETTLE_DELAY_MS);
    }
    if (HasPendingWordEventDisconnectRetry()) {
        ArmPendingWordEventDisconnectRetry();
    }
}

ULONGLONG* GetScheduledTaskDueTimeStorage(ScheduledTaskKind taskKind) {
    switch (taskKind) {
        case ScheduledTaskKind::Save:
            return &g_runtime.timing.saveTimerDueTime;

        case ScheduledTaskKind::DocumentState:
            return &g_runtime.timing.documentStateTimerDueTime;

        case ScheduledTaskKind::EventDisconnectRetry:
            return &g_runtime.timing.eventDisconnectRetryTimerDueTime;
    }

    return nullptr;
}

void SelectEarlierScheduledTaskDueTime(ULONGLONG* selectedDueTime,
                                       ULONGLONG candidateDueTime) {
    if (!selectedDueTime || candidateDueTime == 0) {
        return;
    }

    if (*selectedDueTime == 0 || candidateDueTime < *selectedDueTime) {
        *selectedDueTime = candidateDueTime;
    }
}

ULONGLONG GetNextScheduledTaskDueTime() {
    ULONGLONG nextDueTime = 0;
    SelectEarlierScheduledTaskDueTime(&nextDueTime,
                                      g_runtime.timing.saveTimerDueTime);
    SelectEarlierScheduledTaskDueTime(&nextDueTime,
                                      g_runtime.timing.documentStateTimerDueTime);
    SelectEarlierScheduledTaskDueTime(
        &nextDueTime,
        g_runtime.timing.eventDisconnectRetryTimerDueTime);

    return nextDueTime;
}

void CancelSchedulerTimer() {
    if (g_runtime.timing.schedulerTimerId != 0) {
        HWND messageWindow =
            g_control.messageWindow.load(std::memory_order_acquire);
        if (messageWindow) {
            KillTimer(messageWindow, SCHEDULER_WINDOW_TIMER_ID);
        }
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

    if (!EnsureSchedulerMessageWindow()) {
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
    HWND messageWindow =
        g_control.messageWindow.load(std::memory_order_acquire);
    if (!messageWindow ||
        SetTimer(messageWindow,
                 SCHEDULER_WINDOW_TIMER_ID,
                 delayMs,
                 nullptr) == 0) {
        Wh_Log(L"Scheduler: SetTimer failed, error=%lu", GetLastError());
        return false;
    }

    g_runtime.timing.schedulerTimerId = SCHEDULER_WINDOW_TIMER_ID;
    g_runtime.timing.schedulerTimerDueTime = nextDueTime;
    return true;
}

bool ShouldQueueSchedulerRepair(bool refreshSucceeded,
                                ULONGLONG nextDueTime,
                                bool shutdownRequested,
                                bool repairAlreadyPending) {
    return !refreshSucceeded &&
           nextDueTime != 0 &&
           !shutdownRequested &&
           !repairAlreadyPending;
}

bool RefreshSchedulerTimerWithRecovery() {
    const bool refreshSucceeded = RefreshSchedulerTimer();
    if (refreshSucceeded) {
        return true;
    }

    const LONG pendingRequests =
        g_control.pendingRequests.load(std::memory_order_acquire);
    if (ShouldQueueSchedulerRepair(
            refreshSucceeded,
            GetNextScheduledTaskDueTime(),
            g_control.shutdownRequested.load(std::memory_order_acquire),
            (pendingRequests & RuntimeControlRepairScheduler) != 0)) {
        QueueOwnerControlRequest(RuntimeControlRepairScheduler);
    }
    return false;
}

bool ShouldKeepExistingScheduledTaskDueTime(ULONGLONG existingDueTime,
                                            ULONGLONG candidateDueTime,
                                            ScheduledTaskScheduleMode scheduleMode) {
    return scheduleMode == ScheduledTaskScheduleMode::ArmEarlier &&
           existingDueTime != 0 &&
           existingDueTime <= candidateDueTime;
}

bool ScheduleTask(ScheduledTaskKind taskKind,
                  DWORD delayMs,
                  ScheduledTaskScheduleMode scheduleMode =
                      ScheduledTaskScheduleMode::ArmEarlier) {
    if (!IsOwnerThreadSchedulerContextValid()) {
        return false;
    }

    ULONGLONG* dueTimeStorage = GetScheduledTaskDueTimeStorage(taskKind);
    if (!dueTimeStorage) {
        return false;
    }

    const DWORD effectiveDelayMs = delayMs ? delayMs : 1;
    const ULONGLONG dueTime = GetTickCount64() + effectiveDelayMs;
    if (ShouldKeepExistingScheduledTaskDueTime(*dueTimeStorage,
                                               dueTime,
                                               scheduleMode)) {
        return RefreshSchedulerTimerWithRecovery();
    }

    *dueTimeStorage = dueTime;
    return RefreshSchedulerTimerWithRecovery();
}

void CancelScheduledTask(ScheduledTaskKind taskKind) {
    ULONGLONG* dueTimeStorage = GetScheduledTaskDueTimeStorage(taskKind);
    if (!dueTimeStorage) {
        return;
    }

    *dueTimeStorage = 0;
    RefreshSchedulerTimerWithRecovery();
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
SaveAttemptResult TrySaveActiveDocument(ActiveDocumentSnapshot* snapshot,
                                        const wchar_t* specificPath = nullptr,
                                        IDispatch* specificDocument = nullptr,
                                        ComRetryProfile retryProfile =
                                            ComRetryProfile::Background);
void HandleAutosaveAttemptResult(SaveAttemptResult result,
                                 const ActiveDocumentSnapshot* snapshot,
                                 const RuntimeTickSnapshot& tick);
void TryCriticalTransitionSaveNow(const wchar_t* path, IDispatch* document);

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

enum class DocumentMonitorScheduleKind {
    None,
    RepairSaveTask,
    ActiveRetry,
    EventWatchdog,
    FallbackPoll,
};

DocumentMonitorScheduleKind SelectDocumentMonitorSchedule(
    bool pendingSave,
    bool documentDirty,
    bool manualSavePending,
    bool automationBusyPending,
    bool pendingSaveAsMigration,
    bool saveTaskArmed,
    bool wordEventsConnected) {
    if (manualSavePending || pendingSaveAsMigration) {
        return DocumentMonitorScheduleKind::ActiveRetry;
    }

    if (pendingSave) {
        return saveTaskArmed ? DocumentMonitorScheduleKind::None
                             : DocumentMonitorScheduleKind::RepairSaveTask;
    }

    if (documentDirty) {
        return DocumentMonitorScheduleKind::RepairSaveTask;
    }

    if (automationBusyPending) {
        return DocumentMonitorScheduleKind::ActiveRetry;
    }

    return wordEventsConnected ? DocumentMonitorScheduleKind::EventWatchdog
                               : DocumentMonitorScheduleKind::FallbackPoll;
}

DWORD GetDocumentMonitorScheduleDelay(DocumentMonitorScheduleKind scheduleKind) {
    switch (scheduleKind) {
        case DocumentMonitorScheduleKind::ActiveRetry:
            return DOCUMENT_STATE_PENDING_WATCHDOG_MS;

        case DocumentMonitorScheduleKind::EventWatchdog:
            return DOCUMENT_STATE_EVENT_IDLE_POLL_INTERVAL_MS;

        case DocumentMonitorScheduleKind::FallbackPoll:
            return DOCUMENT_STATE_IDLE_POLL_INTERVAL_MS;

        case DocumentMonitorScheduleKind::None:
        case DocumentMonitorScheduleKind::RepairSaveTask:
            return 0;
    }

    return 0;
}

DWORD GetSteadyDocumentStatePollDelay(const RuntimeFlagSnapshot* flags) {
    const bool hasActivePollWork =
        flags ? HasActiveDocumentStatePollWorkForState(
                    flags->pendingSave ||
                        flags->documentDirty ||
                        flags->manualSavePending,
                    flags->automationBusyPending,
                    flags->pendingSaveAsMigration)
              : HasActiveDocumentStatePollWork();
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
    ClearCurrentSaveRetryReason();
    if (transitionFlush) {
        if (g_runtime.document.observedDocumentPath.Length() == 0 &&
            !g_runtime.document.observedDocument) {
            SetFlag(g_runtime.flags.expeditedSavePending);
            CancelSaveTimer();
            ArmDocumentStateTimer(INPUT_SETTLE_DELAY_MS);
            if (reason) {
                LogSaveStatus(L"waiting to identify the edited document before switching away");
            }
            return;
        }

        RequestTransitionFlush(g_runtime.document.observedDocumentPath.Get(),
                               reason,
                               g_runtime.document.observedDocument.Get());
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

void ResumePendingRuntimeWorkForSignal(SaveResumeSignal signal) {
    if (signal == SaveResumeSignal::None) {
        return;
    }

    if (HasPendingAutosave() &&
        ShouldResumeSaveForSignal(g_runtime.saveRetry.reason, signal)) {
        ClearCurrentSaveRetryReason();
        ArmSaveTimer(AUTOMATION_RECOVERY_DELAY_MS);
    }

    if (ShouldResumeDocumentForSignal(g_runtime.documentRetry.reason, signal)) {
        ClearCurrentDocumentRetryReason();
        ArmDocumentStateTimer(AUTOMATION_RECOVERY_DELAY_MS);
    }
}

class ScopedComMessageFilter final : public IMessageFilter {
public:
    explicit ScopedComMessageFilter(
        ComRetryProfile profile = ComRetryProfile::LegacyLifecycle)
        : m_profile(profile),
          m_deadline(GetTickCount64() + GetComRetryBudgetMs(profile)) {
        HRESULT hr = CoRegisterMessageFilter(this, &m_previousFilter);
        m_registered = SUCCEEDED(hr);
    }

    ScopedComMessageFilter(const ScopedComMessageFilter&) = delete;
    ScopedComMessageFilter& operator=(const ScopedComMessageFilter&) = delete;

    ~ScopedComMessageFilter() {
        if (m_registered) {
            IMessageFilter* currentFilter = nullptr;
            CoRegisterMessageFilter(m_previousFilter, &currentFilter);
            if (currentFilter) {
                currentFilter->Release();
            }
        }

        if (m_previousFilter) {
            m_previousFilter->Release();
        }
    }

    STDMETHODIMP QueryInterface(REFIID riid, void** object) override {
        if (!object) {
            return E_POINTER;
        }

        *object = nullptr;
        if (riid == IID_IUnknown || riid == IID_IMessageFilter) {
            *object = static_cast<IMessageFilter*>(this);
            AddRef();
            return S_OK;
        }

        return E_NOINTERFACE;
    }

    STDMETHODIMP_(ULONG) AddRef() override {
        return static_cast<ULONG>(InterlockedIncrement(&m_refCount));
    }

    STDMETHODIMP_(ULONG) Release() override {
        return static_cast<ULONG>(InterlockedDecrement(&m_refCount));
    }

    STDMETHODIMP_(DWORD) HandleInComingCall(DWORD,
                                            HTASK,
                                            DWORD,
                                            LPINTERFACEINFO) override {
        return SERVERCALL_ISHANDLED;
    }

    STDMETHODIMP_(DWORD) RetryRejectedCall(HTASK,
                                           DWORD elapsedMs,
                                           DWORD rejectType) override {
        const DWORD delayMs = SelectRejectedCallRetryDelay(
            m_profile,
            rejectType,
            elapsedMs,
            GetTickCount64(),
            m_deadline);
        if (delayMs != static_cast<DWORD>(-1)) {
            return delayMs;
        }

        return static_cast<DWORD>(-1);
    }

    STDMETHODIMP_(DWORD) MessagePending(HTASK, DWORD, DWORD) override {
        return PENDINGMSG_WAITDEFPROCESS;
    }

private:
    volatile LONG m_refCount = 1;
    IMessageFilter* m_previousFilter = nullptr;
    ComRetryProfile m_profile = ComRetryProfile::LegacyLifecycle;
    ULONGLONG m_deadline = 0;
    bool m_registered = false;
};

bool ArmDocumentStateTimer(DWORD delayMs) {
    return ScheduleTask(ScheduledTaskKind::DocumentState, delayMs);
}

void ScheduleSaveFromEdit() {
    ClearCurrentSaveRetryReason();
    bool transitionFlushPending =
        LoadFlag(g_runtime.flags.transitionFlushPending);
    if (!transitionFlushPending) {
        ClearExpeditedSavePending();
        ClearPostTransitionRefreshPending();
        g_runtime.timing.lastEditTime = GetTickCount64();
        ClearTransitionFlushRequest();

        // A retired COM target can re-enter and publish a newer transition.
        transitionFlushPending =
            LoadFlag(g_runtime.flags.transitionFlushPending);
    }

    if (transitionFlushPending) {
        SetFlag(g_runtime.flags.expeditedSavePending);
        SetFlag(g_runtime.flags.postTransitionRefreshPending);
    }

    SetFlag(g_runtime.flags.pendingSave);
    const bool documentDirtyKnown = LoadFlag(g_runtime.flags.documentDirtyKnown);
    const bool documentDirty = LoadFlag(g_runtime.flags.documentDirty);
    if (!documentDirtyKnown ||
        !documentDirty ||
        g_runtime.document.observedDocumentPath.Length() == 0 ||
        !g_runtime.document.observedDocument ||
        transitionFlushPending) {
        ArmDocumentStateTimer(INPUT_SETTLE_DELAY_MS);
    }
    RescheduleSaveTimer(transitionFlushPending
                            ? INPUT_SETTLE_DELAY_MS
                            : static_cast<DWORD>(g_settings.saveDelay));
}

void ReconcileDocumentStateSchedule() {
    if (!CanRunOwnerThreadRuntimeWork()) {
        return;
    }

    if (IsDocumentFailureRetryReason(g_runtime.documentRetry.reason) &&
        IsScheduledTaskArmed(ScheduledTaskKind::DocumentState)) {
        return;
    }

    const bool pendingSave = LoadFlag(g_runtime.flags.pendingSave);
    const DocumentMonitorScheduleKind scheduleKind =
        SelectDocumentMonitorSchedule(
            pendingSave,
            LoadFlag(g_runtime.flags.documentDirty),
            LoadFlag(g_runtime.flags.manualSavePending),
            LoadFlag(g_runtime.flags.automationBusyPending),
            LoadFlag(g_runtime.flags.pendingSaveAsMigration),
            IsScheduledTaskArmed(ScheduledTaskKind::Save),
            LoadFlag(g_runtime.flags.wordEventsConnected));

    switch (scheduleKind) {
        case DocumentMonitorScheduleKind::RepairSaveTask:
            if (pendingSave) {
                ArmSaveTimer(INPUT_SETTLE_DELAY_MS);
            } else {
                ScheduleSaveFromEdit();
            }
            return;

        case DocumentMonitorScheduleKind::ActiveRetry:
        case DocumentMonitorScheduleKind::EventWatchdog:
        case DocumentMonitorScheduleKind::FallbackPoll:
            ArmDocumentStateTimer(GetDocumentMonitorScheduleDelay(scheduleKind));
            return;

        case DocumentMonitorScheduleKind::None:
            return;
    }
}

void ClearPendingSave() {
    ClearFlag(g_runtime.flags.pendingSave);
    ClearExpeditedSavePending();
    ResetSaveRetryState();
}

bool ShouldPreserveTransitionFlushForManualSave(bool transitionFlushPending) {
    return transitionFlushPending;
}

void PrepareManualSaveObservation() {
    ClearCurrentSaveRetryReason();
    ClearCurrentDocumentRetryReason();
    bool preserveTransition =
        ShouldPreserveTransitionFlushForManualSave(
            LoadFlag(g_runtime.flags.transitionFlushPending));
    if (!preserveTransition) {
        ClearPostTransitionRefreshPending();
        CancelSaveTimer();
        ClearTransitionFlushRequest();
        preserveTransition =
            LoadFlag(g_runtime.flags.transitionFlushPending);
    }

    if (preserveTransition) {
        SetFlag(g_runtime.flags.expeditedSavePending);
        SetFlag(g_runtime.flags.postTransitionRefreshPending);
        ArmSaveTimer(INPUT_SETTLE_DELAY_MS);
    }

    SetFlag(g_runtime.flags.manualSavePending);
    ArmDocumentStateTimer(INPUT_SETTLE_DELAY_MS);
}

void HandleManualSave() {
    PrepareManualSaveObservation();
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

    BSTR DetachBstr() {
        if (m_value.vt != VT_BSTR) {
            return nullptr;
        }

        BSTR value = m_value.bstrVal;
        VariantInit(&m_value);
        return value;
    }

    IDispatch* DetachDispatch() {
        if (m_value.vt != VT_DISPATCH) {
            return nullptr;
        }

        IDispatch* value = m_value.pdispVal;
        VariantInit(&m_value);
        return value;
    }

private:
    VARIANT m_value = {};
};

const wchar_t* GetWordMemberName(WordMember member) {
    switch (member) {
        case WordMember::ApplicationActiveWindow:
            return L"ActiveWindow";
        case WordMember::ApplicationWindows:
            return L"Windows";
        case WordMember::ApplicationHwnd:
        case WordMember::WindowHwnd:
            return L"Hwnd";
        case WordMember::ApplicationActiveDocument:
            return L"ActiveDocument";
        case WordMember::ApplicationActiveProtectedViewWindow:
            return L"ActiveProtectedViewWindow";
        case WordMember::ApplicationDocuments:
            return L"Documents";
        case WordMember::WindowDocument:
            return L"Document";
        case WordMember::WindowsCount:
        case WordMember::DocumentsCount:
            return L"Count";
        case WordMember::WindowsItem:
        case WordMember::DocumentsItem:
            return L"Item";
        case WordMember::DocumentApplication:
            return L"Application";
        case WordMember::DocumentSaved:
            return L"Saved";
        case WordMember::DocumentReadOnly:
            return L"ReadOnly";
        case WordMember::DocumentPath:
            return L"Path";
        case WordMember::DocumentFullName:
            return L"FullName";
        case WordMember::DocumentName:
            return L"Name";
        case WordMember::DocumentSave:
            return L"Save";
        case WordMember::Count:
            break;
    }

    return nullptr;
}

WordMemberIdCacheEntry* GetWordMemberIdCacheEntry(WordMember member) {
    const size_t index = static_cast<size_t>(member);
    if (index >= static_cast<size_t>(WordMember::Count)) {
        return nullptr;
    }

    return &g_runtime.wordMemberIdCache[index];
}

HRESULT ResolveWordMemberId(IDispatch* dispatch,
                            WordMember member,
                            DISPID* dispatchId) {
    if (!dispatch || !dispatchId) {
        return E_POINTER;
    }

    WordMemberIdCacheEntry* entry = GetWordMemberIdCacheEntry(member);
    const wchar_t* memberName = GetWordMemberName(member);
    if (!entry || !memberName) {
        return E_INVALIDARG;
    }

    if (entry->resolved) {
        *dispatchId = entry->dispId;
        return S_OK;
    }

    LPOLESTR mutableName = const_cast<LPOLESTR>(memberName);
    DISPID resolvedId = DISPID_UNKNOWN;
    const HRESULT hr = dispatch->GetIDsOfNames(kIIDNull,
                                               &mutableName,
                                               1,
                                               LOCALE_USER_DEFAULT,
                                               &resolvedId);
    if (SUCCEEDED(hr)) {
        entry->dispId = resolvedId;
        entry->resolved = true;
        *dispatchId = resolvedId;
    }

    return hr;
}

void InvalidateWordMemberId(WordMember member) {
    WordMemberIdCacheEntry* entry = GetWordMemberIdCacheEntry(member);
    if (entry) {
        entry->dispId = DISPID_UNKNOWN;
        entry->resolved = false;
    }
}

HRESULT InvokeResolvedDispatch(IDispatch* dispatch,
                               DISPID dispatchId,
                               WORD flags,
                               VARIANT* result,
                               int argCount,
                               VARIANT* args) {
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

HRESULT InvokeWordMember(IDispatch* dispatch,
                         WORD flags,
                         WordMember member,
                         VARIANT* result = nullptr,
                         int argCount = 0,
                         VARIANT* args = nullptr) {
    if (!dispatch) {
        return E_POINTER;
    }

    if (argCount < 0) {
        return E_INVALIDARG;
    }

    for (int attempt = 0; attempt < 2; ++attempt) {
        DISPID dispatchId = DISPID_UNKNOWN;
        HRESULT hr = ResolveWordMemberId(dispatch, member, &dispatchId);
        if (FAILED(hr)) {
            return hr;
        }

        hr = InvokeResolvedDispatch(dispatch,
                                    dispatchId,
                                    flags,
                                    result,
                                    argCount,
                                    args);
        if (hr != DISP_E_MEMBERNOTFOUND) {
            return hr;
        }

        // A failed Invoke isn't required to leave pVarResult untouched. Drain
        // any value produced by the stale DISPID before invoking again, or a
        // BSTR/interface result can leak and the retry can receive a dirty
        // VARIANT.
        if (result) {
            const HRESULT clearHr = VariantClear(result);
            if (FAILED(clearHr)) {
                InvalidateWordMemberId(member);
                return clearHr;
            }
            VariantInit(result);
        }

        InvalidateWordMemberId(member);
        if (attempt != 0) {
            return hr;
        }
    }

    return DISP_E_MEMBERNOTFOUND;
}

HRESULT InvokeDispatchUncached(IDispatch* dispatch,
                               WORD flags,
                               const wchar_t* name,
                               VARIANT* result = nullptr,
                               int argCount = 0,
                               VARIANT* args = nullptr) {
    if (!dispatch || !name) {
        return E_POINTER;
    }

    if (argCount < 0) {
        return E_INVALIDARG;
    }

    LPOLESTR mutableName = const_cast<LPOLESTR>(name);
    DISPID dispatchId = DISPID_UNKNOWN;
    HRESULT hr = dispatch->GetIDsOfNames(kIIDNull,
                                         &mutableName,
                                         1,
                                         LOCALE_USER_DEFAULT,
                                         &dispatchId);
    if (FAILED(hr)) {
        return hr;
    }

    return InvokeResolvedDispatch(dispatch,
                                  dispatchId,
                                  flags,
                                  result,
                                  argCount,
                                  args);
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

HRESULT ExtractDispatchFromVariant(ScopedVariant* value, IDispatch** result) {
    if (!value || !result) {
        return E_POINTER;
    }

    if (value->Get()->vt == VT_DISPATCH && value->Get()->pdispVal) {
        *result = value->DetachDispatch();
        return S_OK;
    }

    if (value->Get()->vt == VT_UNKNOWN && value->Get()->punkVal) {
        return QueryInterfaceTyped(value->Get()->punkVal, result);
    }

    return DISP_E_TYPEMISMATCH;
}

HRESULT GetDispatchProperty(IDispatch* dispatch, WordMember member, IDispatch** result) {
    if (!result) {
        return E_POINTER;
    }

    *result = nullptr;

    ScopedVariant value;
    HRESULT hr = InvokeWordMember(dispatch,
                                  DISPATCH_PROPERTYGET,
                                  member,
                                  value.Get());
    if (FAILED(hr)) {
        return hr;
    }

    return ExtractDispatchFromVariant(&value, result);
}

HRESULT GetDispatchPropertyUncached(IDispatch* dispatch,
                                    const wchar_t* name,
                                    IDispatch** result) {
    if (!result) {
        return E_POINTER;
    }

    *result = nullptr;

    ScopedVariant value;
    HRESULT hr = InvokeDispatchUncached(dispatch,
                                        DISPATCH_PROPERTYGET,
                                        name,
                                        value.Get());
    if (FAILED(hr)) {
        return hr;
    }

    return ExtractDispatchFromVariant(&value, result);
}

HRESULT GetDispatchMethodObject(IDispatch* dispatch,
                                WordMember member,
                                IDispatch** result,
                                int argCount = 0,
                                VARIANT* args = nullptr) {
    if (!result) {
        return E_POINTER;
    }

    *result = nullptr;

    ScopedVariant value;
    HRESULT hr = InvokeWordMember(dispatch,
                                  DISPATCH_METHOD | DISPATCH_PROPERTYGET,
                                  member,
                                  value.Get(),
                                  argCount,
                                  args);
    if (FAILED(hr)) {
        return hr;
    }

    return ExtractDispatchFromVariant(&value, result);
}

HRESULT GetBoolProperty(IDispatch* dispatch, WordMember member, bool* result) {
    if (!result) {
        return E_POINTER;
    }

    *result = false;

    ScopedVariant value;
    ScopedVariant converted;
    HRESULT hr = InvokeWordMember(dispatch,
                                  DISPATCH_PROPERTYGET,
                                  member,
                                  value.Get());
    if (FAILED(hr)) {
        return hr;
    }

    if (value.Get()->vt == VT_BOOL) {
        *result = value.Get()->boolVal != VARIANT_FALSE;
        return S_OK;
    }

    hr = VariantChangeType(converted.Get(), value.Get(), 0, VT_BOOL);
    if (SUCCEEDED(hr)) {
        *result = converted.Get()->boolVal != VARIANT_FALSE;
    }

    return hr;
}

HRESULT GetBstrProperty(IDispatch* dispatch, WordMember member, BSTR* result) {
    if (!result) {
        return E_POINTER;
    }

    *result = nullptr;

    ScopedVariant value;
    ScopedVariant converted;
    HRESULT hr = InvokeWordMember(dispatch,
                                  DISPATCH_PROPERTYGET,
                                  member,
                                  value.Get());
    if (FAILED(hr)) {
        return hr;
    }

    if (value.Get()->vt == VT_BSTR) {
        *result = value.DetachBstr();
        return S_OK;
    }

    hr = VariantChangeType(converted.Get(), value.Get(), 0, VT_BSTR);
    if (SUCCEEDED(hr)) {
        *result = converted.DetachBstr();
    }

    return hr;
}

HRESULT GetIntProperty(IDispatch* dispatch, WordMember member, long* result) {
    if (!result) {
        return E_POINTER;
    }

    *result = 0;

    ScopedVariant value;
    ScopedVariant converted;
    HRESULT hr = InvokeWordMember(dispatch,
                                  DISPATCH_PROPERTYGET,
                                  member,
                                  value.Get());
    if (FAILED(hr)) {
        return hr;
    }

    if (value.Get()->vt == VT_I4) {
        *result = value.Get()->lVal;
        return S_OK;
    }

    hr = VariantChangeType(converted.Get(), value.Get(), 0, VT_I4);
    if (SUCCEEDED(hr)) {
        *result = converted.Get()->lVal;
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

bool AreSameComIdentity(IUnknown* left, IUnknown* right) {
    return left && right && left == right;
}

bool AreSameDispatchComIdentity(IDispatch* left, IDispatch* right) {
    if (!left || !right) {
        return false;
    }

    if (left == right) {
        return true;
    }

    ScopedComPtr<IUnknown> leftIdentity;
    ScopedComPtr<IUnknown> rightIdentity;
    if (FAILED(GetComIdentity(left, leftIdentity.Put())) ||
        FAILED(GetComIdentity(right, rightIdentity.Put()))) {
        return false;
    }

    return AreSameComIdentity(leftIdentity.Get(), rightIdentity.Get());
}

LONG_PTR NormalizeComWindowHandleValue(LONG_PTR hwndValue) {
    if (hwndValue == 0) {
        return 0;
    }

#ifdef _WIN64
    const ULONG_PTR rawValue = static_cast<ULONG_PTR>(hwndValue);
    const ULONG_PTR low32Value = static_cast<ULONG_PTR>(static_cast<DWORD>(rawValue));
    const ULONG_PTR high32Value = rawValue & 0xFFFFFFFF00000000ull;
    if (low32Value != 0 && high32Value == 0xFFFFFFFF00000000ull) {
        return static_cast<LONG_PTR>(low32Value);
    }
#endif

    return hwndValue;
}

HWND ResolveCurrentProcessWordRootWindowFromHwnd(HWND hwnd) {
    if (!hwnd || !IsWindow(hwnd)) {
        return nullptr;
    }

    if (IsUsableWordRootWindow(hwnd)) {
        return hwnd;
    }

    HWND rootWindow = GetAncestor(hwnd, GA_ROOT);
    if (!rootWindow) {
        rootWindow = hwnd;
    }

    return IsUsableWordRootWindow(rootWindow) ? rootWindow : nullptr;
}

HWND ResolveCurrentProcessWordRootWindowFromHandle(LONG_PTR hwndValue) {
    HWND rootWindow =
        ResolveCurrentProcessWordRootWindowFromHwnd(reinterpret_cast<HWND>(hwndValue));
    if (rootWindow) {
        return rootWindow;
    }

    const LONG_PTR normalizedHwndValue = NormalizeComWindowHandleValue(hwndValue);
    if (normalizedHwndValue == hwndValue) {
        return nullptr;
    }

    return ResolveCurrentProcessWordRootWindowFromHwnd(
        reinterpret_cast<HWND>(normalizedHwndValue));
}

bool IsUsableWordApplicationWindowHandle(LONG_PTR hwndValue) {
    return ResolveCurrentProcessWordRootWindowFromHandle(hwndValue) != nullptr;
}

HRESULT GetDispatchWindowHandle(IDispatch* dispatch,
                                WordMember hwndMember,
                                LONG_PTR* hwndValue);

bool TryResolveCurrentProcessWordWindowFromDispatch(IDispatch* dispatch,
                                                     WordMember hwndMember,
                                                     LONG_PTR* hwndValue) {
    if (!dispatch) {
        return false;
    }

    LONG_PTR rawHwndValue = 0;
    if (FAILED(GetDispatchWindowHandle(dispatch, hwndMember, &rawHwndValue))) {
        return false;
    }

    HWND rootWindow = ResolveCurrentProcessWordRootWindowFromHandle(rawHwndValue);
    if (!rootWindow) {
        return false;
    }

    if (hwndValue) {
        *hwndValue = reinterpret_cast<LONG_PTR>(rootWindow);
    }
    return true;
}

bool TryResolveCurrentProcessWordWindowFromApplicationWindows(IDispatch* application,
                                                             LONG_PTR* hwndValue) {
    if (!application) {
        return false;
    }

    ScopedComPtr<IDispatch> activeWindow;
    if (SUCCEEDED(GetDispatchProperty(application,
                                      WordMember::ApplicationActiveWindow,
                                      activeWindow.Put())) &&
        TryResolveCurrentProcessWordWindowFromDispatch(activeWindow.Get(),
                                                       WordMember::WindowHwnd,
                                                       hwndValue)) {
        return true;
    }

    ScopedComPtr<IDispatch> windows;
    HRESULT hr = GetDispatchProperty(application,
                                     WordMember::ApplicationWindows,
                                     windows.Put());
    if (FAILED(hr) || !windows) {
        return false;
    }

    long count = 0;
    hr = GetIntProperty(windows.Get(), WordMember::WindowsCount, &count);
    if (FAILED(hr)) {
        return false;
    }

    for (long index = 1; index <= count; ++index) {
        ScopedVariant indexArg;
        indexArg.Get()->vt = VT_I4;
        indexArg.Get()->lVal = index;

        ScopedComPtr<IDispatch> window;
        hr = GetDispatchMethodObject(windows.Get(),
                                     WordMember::WindowsItem,
                                     window.Put(),
                                     1,
                                     indexArg.Get());
        if (FAILED(hr) || !window) {
            continue;
        }

        if (TryResolveCurrentProcessWordWindowFromDispatch(window.Get(),
                                                           WordMember::WindowHwnd,
                                                           hwndValue)) {
            return true;
        }
    }

    return false;
}

HRESULT GetDispatchWindowHandle(IDispatch* dispatch,
                                WordMember hwndMember,
                                LONG_PTR* hwndValue) {
    if (!dispatch || !hwndValue) {
        return E_POINTER;
    }

    *hwndValue = 0;

    ScopedVariant value;
    HRESULT hr = InvokeWordMember(dispatch,
                                  DISPATCH_PROPERTYGET,
                                  hwndMember,
                                  value.Get());
    if (FAILED(hr)) {
        return hr;
    }

    if (value.Get()->vt == VT_I4) {
        *hwndValue = static_cast<LONG_PTR>(value.Get()->lVal);
    } else if (value.Get()->vt == VT_I8) {
        *hwndValue = static_cast<LONG_PTR>(value.Get()->llVal);
    } else {
        ScopedVariant convertedI8;
        hr = VariantChangeType(convertedI8.Get(), value.Get(), 0, VT_I8);
        if (SUCCEEDED(hr)) {
            *hwndValue = static_cast<LONG_PTR>(convertedI8.Get()->llVal);
        } else {
            // Use a fresh destination because a failed conversion is allowed
            // to leave its output VARIANT in an unspecified state.
            ScopedVariant convertedI4;
            hr = VariantChangeType(convertedI4.Get(), value.Get(), 0, VT_I4);
            if (FAILED(hr)) {
                return hr;
            }

            *hwndValue = static_cast<LONG_PTR>(convertedI4.Get()->lVal);
        }
    }

    if (*hwndValue == 0) {
        return E_FAIL;
    }

    return S_OK;
}

bool DoesApplicationBelongToCurrentProcess(IDispatch* application, LONG_PTR* hwndValue = nullptr) {
    LONG_PTR resolvedHwndValue = 0;
    if (TryResolveCurrentProcessWordWindowFromDispatch(application,
                                                       WordMember::ApplicationHwnd,
                                                       &resolvedHwndValue) ||
        TryResolveCurrentProcessWordWindowFromApplicationWindows(application, &resolvedHwndValue)) {
        if (hwndValue) {
            *hwndValue = resolvedHwndValue;
        }
        return true;
    }

    return false;
}

void ResetCachedActiveDocumentState() {
    ++g_runtime.automation.activeDocumentResetDepth;
    {
        // Make the cache unusable before releasing any COM proxy. Cache
        // publication is gated until every retired reference is released.
        ++g_runtime.automation.documentGeneration;
        g_runtime.automation.activeDocumentStale = true;
        g_runtime.automation.activeDocumentMetadataValid = false;
        g_runtime.automation.activeDocumentReadOnly = false;
        g_runtime.automation.activeDocumentHasPath = false;
        g_runtime.automation.activeDocumentPath.Reset();

        ScopedComPtr<IUnknown> previousIdentity;
        ScopedComPtr<IDispatch> previousDocument;
        previousIdentity.Reset(
            g_runtime.automation.activeDocumentIdentity.Detach());
        previousDocument.Reset(g_runtime.automation.activeDocument.Detach());
    }
    --g_runtime.automation.activeDocumentResetDepth;
}

void ResetAutomationCacheState() {
    ++g_runtime.automation.resetDepth;
    ++g_runtime.automation.applicationGeneration;
    {
        g_runtime.automation.applicationHwnd = 0;

        ScopedComPtr<IUnknown> previousApplicationIdentity;
        ScopedComPtr<IDispatch> previousApplication;
        previousApplicationIdentity.Reset(
            g_runtime.automation.applicationIdentity.Detach());
        previousApplication.Reset(g_runtime.automation.application.Detach());

        ResetCachedActiveDocumentState();
    }
    --g_runtime.automation.resetDepth;
}

HRESULT CacheWordApplication(IDispatch* application, LONG_PTR applicationHwnd) {
    if (!application || applicationHwnd == 0 ||
        !IsUsableWordApplicationWindowHandle(applicationHwnd)) {
        return E_INVALIDARG;
    }

    if (g_runtime.automation.resetDepth != 0) {
        return S_FALSE;
    }

    const unsigned int expectedGeneration =
        g_runtime.automation.applicationGeneration;
    ScopedComPtr<IDispatch> nextApplication;
    ReplaceStoredComPtr(&nextApplication, application);
    ScopedComPtr<IUnknown> identity;
    const HRESULT hr = GetComIdentity(application, identity.Put());
    if (g_runtime.automation.resetDepth != 0 ||
        g_runtime.automation.applicationGeneration != expectedGeneration) {
        return S_FALSE;
    }
    if (FAILED(hr) || !identity) {
        return FAILED(hr) ? hr : E_FAIL;
    }

    const bool sameApplication =
        AreSameComIdentity(identity.Get(),
                           g_runtime.automation.applicationIdentity.Get());
    if (!sameApplication) {
        // DISPIDs are stable for one Word automation type library, but a new
        // application identity can expose a different proxy/type version.
        ClearWordMemberIdCache();
    }

    // Hwnd is the application-cache commit marker. Withdraw it, publish the
    // complete replacement, then release retired proxies with no later write.
    g_runtime.automation.applicationHwnd = 0;
    ScopedComPtr<IUnknown> previousApplicationIdentity;
    ScopedComPtr<IDispatch> previousApplication;
    DetachedDocumentBinding previousActiveDocument;
    previousApplicationIdentity.Reset(
        g_runtime.automation.applicationIdentity.Detach());
    previousApplication.Reset(g_runtime.automation.application.Detach());

    if (!sameApplication) {
        ++g_runtime.automation.documentGeneration;
        g_runtime.automation.activeDocumentStale = true;
        g_runtime.automation.activeDocumentMetadataValid = false;
        g_runtime.automation.activeDocumentReadOnly = false;
        g_runtime.automation.activeDocumentHasPath = false;
        g_runtime.automation.activeDocumentPath.Reset();
        DetachDocumentBinding(
            &g_runtime.automation.activeDocument,
            &g_runtime.automation.activeDocumentIdentity,
            &previousActiveDocument);
    }

    g_runtime.automation.applicationIdentity.Reset(identity.Detach());
    g_runtime.automation.application.Reset(nextApplication.Detach());
    ++g_runtime.automation.applicationGeneration;
    g_runtime.automation.applicationHwnd = applicationHwnd;
    return S_OK;
}

HRESULT CopyCachedWordApplication(IDispatch** application) {
    if (!application) {
        return E_POINTER;
    }

    *application = nullptr;
    if (!g_runtime.automation.application) {
        return E_FAIL;
    }

    if (!g_runtime.automation.applicationIdentity ||
        !IsUsableWordApplicationWindowHandle(
            g_runtime.automation.applicationHwnd)) {
        ResetAutomationCacheState();
        return E_FAIL;
    }

    const unsigned int expectedGeneration =
        g_runtime.automation.applicationGeneration;
    IDispatch* cachedApplication =
        g_runtime.automation.application.Get();
    const LONG_PTR cachedHwnd = g_runtime.automation.applicationHwnd;
    cachedApplication->AddRef();
    if (g_runtime.automation.resetDepth != 0 ||
        g_runtime.automation.applicationGeneration != expectedGeneration ||
        g_runtime.automation.application.Get() != cachedApplication ||
        g_runtime.automation.applicationHwnd != cachedHwnd ||
        !g_runtime.automation.applicationIdentity) {
        cachedApplication->Release();
        return E_FAIL;
    }

    *application = cachedApplication;
    return S_OK;
}

HRESULT CacheActiveDocumentBinding(IDispatch* document) {
    if (!document) {
        ResetCachedActiveDocumentState();
        return S_FALSE;
    }

    if (g_runtime.automation.resetDepth != 0 ||
        g_runtime.automation.activeDocumentResetDepth != 0) {
        return S_FALSE;
    }

    const unsigned int expectedGeneration =
        g_runtime.automation.documentGeneration;
    ScopedComPtr<IDispatch> nextDocument;
    ReplaceStoredComPtr(&nextDocument, document);
    ScopedComPtr<IUnknown> identity;
    const HRESULT hr = GetComIdentity(document, identity.Put());
    if (g_runtime.automation.resetDepth != 0 ||
        g_runtime.automation.activeDocumentResetDepth != 0 ||
        g_runtime.automation.documentGeneration != expectedGeneration) {
        return S_FALSE;
    }
    if (FAILED(hr) || !identity) {
        // The caller is presenting the current document. Until its identity
        // can be established, the previous binding must not be reused.
        g_runtime.automation.activeDocumentStale = true;
        return FAILED(hr) ? hr : E_FAIL;
    }

    const bool sameDocument =
        AreSameComIdentity(identity.Get(),
                           g_runtime.automation.activeDocumentIdentity.Get());
    g_runtime.automation.activeDocumentStale = true;
    DetachedDocumentBinding previousDocument;
    DetachDocumentBinding(&g_runtime.automation.activeDocument,
                          &g_runtime.automation.activeDocumentIdentity,
                          &previousDocument);
    if (!sameDocument) {
        ++g_runtime.automation.documentGeneration;
        g_runtime.automation.activeDocumentMetadataValid = false;
        g_runtime.automation.activeDocumentReadOnly = false;
        g_runtime.automation.activeDocumentHasPath = false;
        g_runtime.automation.activeDocumentPath.Reset();
    }

    g_runtime.automation.activeDocumentIdentity.Reset(identity.Detach());
    g_runtime.automation.activeDocument.Reset(nextDocument.Detach());
    g_runtime.automation.activeDocumentStale = false;
    return S_OK;
}

HRESULT CopyCachedActiveDocument(IDispatch** document) {
    if (!document) {
        return E_POINTER;
    }

    *document = nullptr;
    if (g_runtime.automation.activeDocumentStale ||
        !g_runtime.automation.activeDocument ||
        !g_runtime.automation.activeDocumentIdentity) {
        return E_FAIL;
    }

    const unsigned int expectedGeneration =
        g_runtime.automation.documentGeneration;
    IDispatch* cachedDocument =
        g_runtime.automation.activeDocument.Get();
    cachedDocument->AddRef();
    if (g_runtime.automation.resetDepth != 0 ||
        g_runtime.automation.activeDocumentResetDepth != 0 ||
        g_runtime.automation.documentGeneration != expectedGeneration ||
        g_runtime.automation.activeDocumentStale ||
        g_runtime.automation.activeDocument.Get() != cachedDocument ||
        !g_runtime.automation.activeDocumentIdentity) {
        cachedDocument->Release();
        return E_FAIL;
    }

    *document = cachedDocument;
    return S_OK;
}

bool IsCachedActiveDocument(IDispatch* document) {
    if (!document || !g_runtime.automation.activeDocumentIdentity) {
        return false;
    }

    if (document == g_runtime.automation.activeDocument.Get()) {
        return true;
    }

    ScopedComPtr<IUnknown> identity;
    return SUCCEEDED(GetComIdentity(document, identity.Put())) &&
           identity &&
           AreSameComIdentity(identity.Get(),
                              g_runtime.automation.activeDocumentIdentity.Get());
}

void MarkCachedActiveDocumentStale() {
    g_runtime.automation.activeDocumentStale = true;
}

void InvalidateCachedActiveDocumentIfMatches(IDispatch* document) {
    if (!document || IsCachedActiveDocument(document)) {
        ResetCachedActiveDocumentState();
    }
}

void InvalidateCachedActiveDocumentMetadataIfMatches(IDispatch* document) {
    if (document && !IsCachedActiveDocument(document)) {
        return;
    }

    g_runtime.automation.activeDocumentMetadataValid = false;
    g_runtime.automation.activeDocumentReadOnly = false;
    g_runtime.automation.activeDocumentHasPath = false;
    g_runtime.automation.activeDocumentPath.Reset();
}

void InvalidateActiveDocumentCacheForEventCoverageGap() {
    // While the connection point is down, Word can switch documents or
    // complete Save As without delivering the invalidation events below.
    MarkCachedActiveDocumentStale();
    InvalidateCachedActiveDocumentMetadataIfMatches(nullptr);
}

bool IsWordEventSessionApplicationCacheValid(const WordEventSession& session,
                                              LONG_PTR* applicationHwnd = nullptr) {
    if (!session.application || session.applicationHwnd == 0 ||
        !IsUsableWordApplicationWindowHandle(session.applicationHwnd)) {
        return false;
    }

    if (applicationHwnd) {
        *applicationHwnd = session.applicationHwnd;
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

    const unsigned int expectedEpoch =
        g_runtime.events.connectionEpoch;
    IDispatch* cachedApplication = session.application.Get();
    const LONG_PTR cachedHwnd = session.applicationHwnd;
    cachedApplication->AddRef();
    if (g_runtime.events.connectionEpoch != expectedEpoch ||
        IsWordEventTeardownInProgress() ||
        session.application.Get() != cachedApplication ||
        session.applicationHwnd != cachedHwnd) {
        cachedApplication->Release();
        return E_FAIL;
    }

    *application = cachedApplication;
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

bool ShouldPropagateNativeDocumentProbeFailure(HRESULT hr) {
    return FAILED(hr) &&
           (IsRetryableAutomationFailure(hr) ||
            IsTerminalAutomationObjectFailure(hr));
}

bool IsExpectedNativeDocumentProbeMiss(HRESULT hr) {
    return hr == DISP_E_UNKNOWNNAME || hr == DISP_E_MEMBERNOTFOUND;
}

void RememberNativeDocumentProbeFailure(HRESULT hr,
                                        HRESULT* firstHardFailure) {
    if (!firstHardFailure || FAILED(*firstHardFailure) || !FAILED(hr) ||
        IsExpectedNativeDocumentProbeMiss(hr) ||
        ShouldPropagateNativeDocumentProbeFailure(hr)) {
        return;
    }

    *firstHardFailure = hr;
}

HRESULT SelectNativeDocumentProbeMissResult(HRESULT firstHardFailure) {
    return FAILED(firstHardFailure) ? firstHardFailure : S_FALSE;
}

HRESULT GetDocumentFromObjectChain(IDispatch* nativeObject, IDispatch** document) {
    if (!nativeObject || !document) {
        return E_POINTER;
    }

    *document = nullptr;

    ScopedComPtr<IDispatch> current;
    nativeObject->AddRef();
    current.Reset(nativeObject);
    HRESULT firstHardFailure = S_OK;

    for (int depth = 0; depth < 4 && current; ++depth) {
        HRESULT hr = GetDispatchPropertyUncached(current.Get(), L"Document", document);
        if (SUCCEEDED(hr) && *document) {
            return S_OK;
        }
        if (ShouldPropagateNativeDocumentProbeFailure(hr)) {
            return hr;
        }
        RememberNativeDocumentProbeFailure(hr, &firstHardFailure);

        hr = GetDispatchPropertyUncached(current.Get(), L"ActiveDocument", document);
        if (SUCCEEDED(hr) && *document) {
            return S_OK;
        }
        if (ShouldPropagateNativeDocumentProbeFailure(hr)) {
            return hr;
        }
        RememberNativeDocumentProbeFailure(hr, &firstHardFailure);

        ScopedComPtr<IDispatch> parent;
        hr = GetDispatchPropertyUncached(current.Get(), L"Parent", parent.Put());
        if (FAILED(hr) || !parent) {
            if (ShouldPropagateNativeDocumentProbeFailure(hr)) {
                return hr;
            }
            RememberNativeDocumentProbeFailure(hr, &firstHardFailure);
            break;
        }

        current = std::move(parent);
    }

    ScopedComPtr<IDispatch> application;
    HRESULT hr = GetDispatchPropertyUncached(nativeObject,
                                             L"Application",
                                             application.Put());
    if (FAILED(hr) || !application) {
        return FAILED(hr) ? hr : E_FAIL;
    }

    ScopedComPtr<IDispatch> activeWindow;
    hr = GetDispatchProperty(application.Get(),
                             WordMember::ApplicationActiveWindow,
                             activeWindow.Put());
    if (ShouldPropagateNativeDocumentProbeFailure(hr)) {
        return hr;
    }
    RememberNativeDocumentProbeFailure(hr, &firstHardFailure);
    if (SUCCEEDED(hr) && activeWindow) {
        hr = GetDispatchProperty(activeWindow.Get(),
                                 WordMember::WindowDocument,
                                 document);
        if (SUCCEEDED(hr) && *document) {
            return S_OK;
        }
        if (ShouldPropagateNativeDocumentProbeFailure(hr)) {
            return hr;
        }
        RememberNativeDocumentProbeFailure(hr, &firstHardFailure);
    }

    hr = GetDispatchProperty(application.Get(),
                             WordMember::ApplicationActiveDocument,
                             document);
    if (SUCCEEDED(hr) && *document) {
        return S_OK;
    }
    if (ShouldPropagateNativeDocumentProbeFailure(hr)) {
        return hr;
    }
    RememberNativeDocumentProbeFailure(hr, &firstHardFailure);
    return SelectNativeDocumentProbeMissResult(firstHardFailure);
}

HRESULT GetDocumentIdentityAndPathState(IDispatch* document,
                                        BSTR* result,
                                        bool* hasSavedPath) {
    if (!document || !result || !hasSavedPath) {
        return E_POINTER;
    }

    *result = nullptr;
    *hasSavedPath = false;

    ScopedBstr path;
    HRESULT hr = GetBstrProperty(document, WordMember::DocumentPath, path.Put());
    if (FAILED(hr)) {
        return hr;
    }

    if (path.Length() == 0) {
        *hasSavedPath = false;
        return S_FALSE;
    }

    ScopedBstr fullName;
    if (SUCCEEDED(GetBstrProperty(document,
                                  WordMember::DocumentFullName,
                                  fullName.Put())) &&
        fullName.Length() > 0) {
        *hasSavedPath = true;
        *result = fullName.Detach();
        return S_OK;
    }

    ScopedBstr name;
    hr = GetBstrProperty(document, WordMember::DocumentName, name.Put());
    if (FAILED(hr)) {
        return hr;
    }

    *hasSavedPath = true;

    if (name.Length() == 0) {
        *result = path.Detach();
        return S_OK;
    }

    const UINT pathLength = path.Length();
    const UINT nameLength = name.Length();
    const bool needsSlash = path.CStr()[pathLength - 1] != L'\\' && path.CStr()[pathLength - 1] != L'/';
    const UINT separatorLength = needsSlash ? 1u : 0u;
    const UINT maxLength = (std::numeric_limits<UINT>::max)();
    if (pathLength > maxLength - separatorLength ||
        nameLength > maxLength - pathLength - separatorLength) {
        return E_OUTOFMEMORY;
    }
    const UINT totalLength = pathLength + separatorLength + nameLength;

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
    DetachedDocumentBinding previousBinding;
    DetachDocumentBinding(
        &g_runtime.document.transitionFlushDocument,
        &g_runtime.document.transitionFlushDocumentIdentity,
        &previousBinding);
}

bool IsCurrentTransitionFlushDocumentBinding(unsigned int expectedGeneration,
                                             IDispatch* document,
                                             IUnknown* identity) {
    return LoadFlag(g_runtime.flags.transitionFlushPending) &&
           g_runtime.document.transitionFlushGeneration == expectedGeneration &&
           g_runtime.document.transitionFlushDocument.Get() == document &&
           g_runtime.document.transitionFlushDocumentIdentity.Get() == identity;
}

bool CopyValidTransitionFlushDocument(
    const wchar_t* expectedPath,
    ScopedComPtr<IDispatch>* result) {
    if (!result || !LoadFlag(g_runtime.flags.transitionFlushPending)) {
        return false;
    }

    const unsigned int expectedGeneration =
        g_runtime.document.transitionFlushGeneration;
    IDispatch* document =
        g_runtime.document.transitionFlushDocument.Get();
    IUnknown* storedIdentity =
        g_runtime.document.transitionFlushDocumentIdentity.Get();
    if (!document || !storedIdentity) {
        return false;
    }

    ScopedComPtr<IDispatch> documentRef;
    ScopedComPtr<IUnknown> storedIdentityRef;
    document->AddRef();
    documentRef.Reset(document);
    if (!IsCurrentTransitionFlushDocumentBinding(expectedGeneration,
                                                 document,
                                                 storedIdentity)) {
        return false;
    }

    // The first AddRef can re-enter and retire the identity. Confirm that the
    // pair is still current before touching the second raw pointer; once the
    // document is pinned, a later identity AddRef re-entry is harmless.
    storedIdentity->AddRef();
    storedIdentityRef.Reset(storedIdentity);
    if (!IsCurrentTransitionFlushDocumentBinding(expectedGeneration,
                                                 document,
                                                 storedIdentity)) {
        return false;
    }

    ScopedComPtr<IUnknown> resolvedIdentity;
    const HRESULT identityHr =
        GetComIdentity(documentRef.Get(), resolvedIdentity.Put());
    bool shouldInvalidate =
        IsTerminalAutomationObjectFailure(identityHr);
    bool valid = SUCCEEDED(identityHr) && resolvedIdentity &&
                 AreSameComIdentity(resolvedIdentity.Get(),
                                    storedIdentityRef.Get());
    if (SUCCEEDED(identityHr) && resolvedIdentity && !valid) {
        shouldInvalidate = true;
    }

    if (valid && expectedPath && *expectedPath) {
        ScopedBstr cachedPath;
        const HRESULT pathHr =
            GetDocumentIdentity(documentRef.Get(), cachedPath.Put());
        if (FAILED(pathHr)) {
            shouldInvalidate =
                IsTerminalAutomationObjectFailure(pathHr);
            valid = false;
        } else if (cachedPath.Length() == 0 ||
                   !AreSameResolvedDocumentPath(cachedPath.CStr(),
                                                expectedPath)) {
            shouldInvalidate = true;
            valid = false;
        }
    }

    const bool stillCurrent =
        IsCurrentTransitionFlushDocumentBinding(expectedGeneration,
                                                document,
                                                storedIdentity);
    if (!valid || !stillCurrent) {
        if (shouldInvalidate && stillCurrent) {
            InvalidateTransitionFlushDocumentCache();
        }
        return false;
    }

    result->Reset(documentRef.Detach());
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

    LONG_PTR applicationHwnd = 0;
    if (!DoesApplicationBelongToCurrentProcess(*application,
                                               &applicationHwnd)) {
        (*application)->Release();
        *application = nullptr;
        return HRESULT_FROM_WIN32(ERROR_NOT_FOUND);
    }

    CacheWordApplication(*application, applicationHwnd);
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

    hr = GetDispatchPropertyUncached(nativeObject.Get(), L"Application", application);
    if (FAILED(hr) || !*application) {
        ScopedComPtr<IDispatch> activeDocument;
        if (SUCCEEDED(GetDispatchPropertyUncached(nativeObject.Get(),
                                                  L"ActiveDocument",
                                                  activeDocument.Put()))) {
            hr = GetDispatchProperty(activeDocument.Get(),
                                     WordMember::DocumentApplication,
                                     application);
            if (FAILED(hr) || !*application) {
                nativeObject.Get()->AddRef();
                *application = nativeObject.Get();
                hr = S_OK;
            }
        }
    }

    if (FAILED(hr) || !*application) {
        return FAILED(hr) ? hr : E_FAIL;
    }

    LONG_PTR hwndValue = 0;
    if (!DoesApplicationBelongToCurrentProcess(*application, &hwndValue)) {
        HWND rootWindow = FindCurrentProcessWordRootWindow();
        if (!rootWindow) {
            (*application)->Release();
            *application = nullptr;
            return HRESULT_FROM_WIN32(ERROR_NOT_FOUND);
        }

        hwndValue = reinterpret_cast<LONG_PTR>(rootWindow);
    }

    CacheWordApplication(*application, hwndValue);
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

    CacheWordApplication(*application,
                         g_runtime.events.session.applicationHwnd);
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

HRESULT SelectWordApplicationResolutionFailure(bool hasRetryableFailure,
                                               HRESULT lastRetryableFailure,
                                               HRESULT lastFailure) {
    return hasRetryableFailure ? lastRetryableFailure : lastFailure;
}

HRESULT RememberFirstDocumentLookupHardFailure(HRESULT firstHardFailure,
                                               HRESULT candidateFailure) {
    if (FAILED(firstHardFailure) || !FAILED(candidateFailure)) {
        return firstHardFailure;
    }

    return candidateFailure;
}

bool ShouldReturnDocumentLookupFailureImmediately(HRESULT hr) {
    return IsRetryableAutomationFailure(hr) ||
           IsTerminalAutomationObjectFailure(hr);
}

HRESULT SelectDocumentPathLookupMissResult(HRESULT firstHardFailure) {
    return FAILED(firstHardFailure)
               ? firstHardFailure
               : HRESULT_FROM_WIN32(ERROR_NOT_FOUND);
}

HRESULT ResolveWordApplication(WordApplicationResolutionStrategy strategy,
                               IDispatch** application) {
    if (!application) {
        return E_POINTER;
    }

    *application = nullptr;

    if (SUCCEEDED(CopyCachedWordApplication(application)) && *application) {
        return S_OK;
    }

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

    bool hasRetryableFailure = false;
    HRESULT lastRetryableFailure = S_OK;
    HRESULT lastFailure = E_FAIL;
    for (WordApplicationSource source : sources) {
        HRESULT hr = TryResolveWordApplicationFromSource(source, application);
        if (SUCCEEDED(hr) && application && *application) {
            return hr;
        }

        if (IsRetryableAutomationFailure(hr)) {
            hasRetryableFailure = true;
            lastRetryableFailure = hr;
        } else if (FAILED(hr)) {
            lastFailure = hr;
        }
    }

    return SelectWordApplicationResolutionFailure(hasRetryableFailure,
                                                 lastRetryableFailure,
                                                 lastFailure);
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
    hr = GetDispatchProperty(application.Get(),
                             WordMember::ApplicationDocuments,
                             documents.Put());
    if (FAILED(hr) || !documents) {
        if (IsTerminalAutomationObjectFailure(hr)) {
            ResetAutomationCacheState();
            DisconnectWordApplicationEvents();
        }
        return FAILED(hr) ? hr : E_FAIL;
    }

    long count = 0;
    hr = GetIntProperty(documents.Get(), WordMember::DocumentsCount, &count);
    if (FAILED(hr)) {
        if (IsTerminalAutomationObjectFailure(hr)) {
            ResetAutomationCacheState();
            DisconnectWordApplicationEvents();
        }
        return hr;
    }

    GrowablePathBuffer pathBuffer;
    ScopedBstr resolvedTargetPath;
    bool targetResolutionAttempted = false;
    bool targetResolutionAvailable = false;
    HRESULT firstHardFailure = S_OK;
    for (long index = 1; index <= count; ++index) {
        ScopedVariant indexArg;
        indexArg.Get()->vt = VT_I4;
        indexArg.Get()->lVal = index;

        ScopedComPtr<IDispatch> candidate;
        hr = GetDispatchMethodObject(documents.Get(),
                                     WordMember::DocumentsItem,
                                     candidate.Put(),
                                     1,
                                     indexArg.Get());
        if (FAILED(hr)) {
            if (IsTerminalAutomationObjectFailure(hr)) {
                ResetAutomationCacheState();
                DisconnectWordApplicationEvents();
                return hr;
            }

            if (ShouldReturnDocumentLookupFailureImmediately(hr)) {
                return hr;
            }

            firstHardFailure =
                RememberFirstDocumentLookupHardFailure(firstHardFailure, hr);
            continue;
        }

        ScopedBstr candidateIdentity;
        hr = GetDocumentIdentity(candidate.Get(), candidateIdentity.Put());
        if (FAILED(hr) || candidateIdentity.Length() == 0) {
            if (IsTerminalAutomationObjectFailure(hr)) {
                ResetAutomationCacheState();
                DisconnectWordApplicationEvents();
                return hr;
            }

            if (FAILED(hr) &&
                ShouldReturnDocumentLookupFailureImmediately(hr)) {
                return hr;
            }

            if (FAILED(hr)) {
                firstHardFailure = RememberFirstDocumentLookupHardFailure(
                    firstHardFailure,
                    hr);
            }
            continue;
        }

        const DocumentPathTextComparison textComparison =
            CompareDocumentPathText(candidateIdentity.CStr(), path);
        if (textComparison == DocumentPathTextComparison::Equal) {
            *document = candidate.Detach();
            return S_OK;
        }

        if (textComparison != DocumentPathTextComparison::RequiresResolution) {
            continue;
        }

        if (!targetResolutionAttempted) {
            targetResolutionAttempted = true;
            targetResolutionAvailable =
                TryGetResolvedDocumentPath(path,
                                           &resolvedTargetPath,
                                           &pathBuffer);
        }

        if (!targetResolutionAvailable) {
            continue;
        }

        ScopedBstr resolvedCandidatePath;
        if (TryGetResolvedDocumentPath(candidateIdentity.CStr(),
                                       &resolvedCandidatePath,
                                       &pathBuffer) &&
            AreSameDocumentPathText(resolvedCandidatePath.CStr(),
                                    resolvedTargetPath.CStr())) {
            *document = candidate.Detach();
            return S_OK;
        }
    }

    return SelectDocumentPathLookupMissResult(firstHardFailure);
}

HRESULT GetWordDocumentFromActiveWindow(IDispatch** document) {
    if (!document) {
        return E_POINTER;
    }

    *document = nullptr;

    HWND viewWindow = FindNativeWordViewWindow();
    if (!viewWindow) {
        return S_FALSE;
    }

    ScopedComPtr<IDispatch> nativeObject;
    HRESULT hr = GetNativeWordObjectFromWindow(viewWindow, nativeObject.Put());
    if (FAILED(hr)) {
        return hr;
    }
    if (!nativeObject) {
        return S_FALSE;
    }

    hr = GetDocumentFromObjectChain(nativeObject.Get(), document);
    if (FAILED(hr)) {
        return hr;
    }
    if (!*document) {
        return S_FALSE;
    }

    return S_OK;
}

enum class NativeDocumentLookupOutcome {
    Ready,
    Missing,
    RetryLater,
};

NativeDocumentLookupOutcome ClassifyNativeDocumentLookupResult(
    HRESULT hr,
    bool hasDocument) {
    if (SUCCEEDED(hr) && hasDocument) {
        return NativeDocumentLookupOutcome::Ready;
    }

    if (hr == S_FALSE) {
        return NativeDocumentLookupOutcome::Missing;
    }

    return NativeDocumentLookupOutcome::RetryLater;
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

    if (g_runtime.events.cachedDispIdsValid) {
        *dispIds = g_runtime.events.cachedDispIds;
        return S_OK;
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
                               L"WindowActivate",
                               &dispIds->windowActivate);
    if (FAILED(hr)) {
        return hr;
    }

    hr = ResolveTypeInfoDispId(eventTypeInfo.Get(),
                               L"WindowDeactivate",
                               &dispIds->windowDeactivate);
    if (FAILED(hr)) {
        return hr;
    }

    g_runtime.events.cachedDispIds = *dispIds;
    g_runtime.events.cachedDispIdsValid = true;
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

    if (LoadFlag(g_runtime.flags.autoSaveInProgress) && !saveAsUi) {
        return;
    }

    PrepareManualSaveObservation();

    // SaveAsUI only reports whether Word displayed the Save As dialog. A
    // programmatic SaveAs/SaveAs2 can change FullName with SaveAsUI == false,
    // so every external save event must invalidate cached path metadata.
    if (document) {
        InvalidateCachedActiveDocumentMetadataIfMatches(document.Get());
    }

    if (!saveAsUi || !document) {
        return;
    }

    const unsigned int expectedGeneration =
        g_runtime.document.pendingSaveAsGeneration;
    ScopedComPtr<IUnknown> documentIdentity;
    GetComIdentity(document.Get(), documentIdentity.Put());
    if (g_runtime.document.pendingSaveAsGeneration != expectedGeneration) {
        return;
    }

    ClearFlag(g_runtime.flags.pendingSaveAsMigration);
    ScopedComPtr<IUnknown> previousIdentity;
    previousIdentity.Reset(
        g_runtime.document.pendingSaveAsDocumentIdentity.Detach());
    g_runtime.document.pendingSaveAsDocumentIdentity.Reset(
        documentIdentity.Detach());
    ++g_runtime.document.pendingSaveAsGeneration;
    g_runtime.timing.pendingSaveAsTime = GetTickCount64();
    SetFlag(g_runtime.flags.pendingSaveAsMigration);
    LogSaveStatus(L"tracking Save As to migrate the current document state");
}

void HandleWordDocumentBeforeCloseEvent(const DISPPARAMS* params) {
    MarkCachedActiveDocumentStale();
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
                               L"finishing pending changes before Word closes the current document",
                               document.Get());
    }

    if (HasPendingAutosave()) {
        TryCriticalTransitionSaveNow(documentPath.Length() > 0
                                         ? documentPath.CStr()
                                         : g_runtime.document.observedDocumentPath.Get(),
                                     document.Get());
    }

    if (HasPendingAutosave()) {
        g_runtime.timing.lastEditTime = 0;
        ArmSaveTimer(1);
    } else {
        ArmDocumentStateTimer(1);
    }
}

void HandleWordDocumentChangeEvent() {
    MarkCachedActiveDocumentStale();
    InvalidateCachedActiveDocumentMetadataIfMatches(nullptr);
    InvalidateWordUiWindowCache();
    ExpirePendingSaveAsMigrationIfNeeded();
    ResumePendingRuntimeWorkForSignal(SaveResumeSignal::DocumentChanged);
    ArmDocumentStateTimer(INPUT_SETTLE_DELAY_MS);
}

void HandleWordWindowActivateEvent(const DISPPARAMS* params) {
    ScopedComPtr<IDispatch> document;
    if (SUCCEEDED(CopyDispatchFromVariant(GetEventArgument(params, 0),
                                          document.Put())) &&
        document) {
        if (FAILED(CacheActiveDocumentBinding(document.Get()))) {
            MarkCachedActiveDocumentStale();
        }
    } else {
        MarkCachedActiveDocumentStale();
    }

    InvalidateWordUiWindowCache();
    ExpirePendingSaveAsMigrationIfNeeded();
    ResumePendingRuntimeWorkForSignal(SaveResumeSignal::WordActivated);
    ArmDocumentStateTimer(INPUT_SETTLE_DELAY_MS);
}

void HandleWordWindowDeactivateEvent(const DISPPARAMS* params) {
    MarkCachedActiveDocumentStale();
    InvalidateWordUiWindowCache();

    if (!HasPendingSaveWork()) {
        return;
    }

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
                               L"finishing pending changes before Word leaves the current document window",
                               document.Get());
    }

    if (HasPendingAutosave()) {
        g_runtime.timing.lastEditTime = 0;
        ArmSaveTimer(1);
    } else {
        ArmDocumentStateTimer(1);
    }
}

enum class WordApplicationEventKind {
    None,
    DocumentBeforeSave,
    DocumentBeforeClose,
    DocumentChange,
    WindowActivate,
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

    if (dispIdMember == dispIds.windowActivate) {
        return WordApplicationEventKind::WindowActivate;
    }

    if (dispIdMember == dispIds.windowDeactivate) {
        return WordApplicationEventKind::WindowDeactivate;
    }

    return WordApplicationEventKind::None;
}

void DispatchWordApplicationEvent(WordApplicationEventKind eventKind,
                                  const DISPPARAMS* params) {
    if (!LoadFlag(g_runtime.flags.moduleActive)) {
        return;
    }

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

        case WordApplicationEventKind::WindowActivate:
            HandleWordWindowActivateEvent(params);
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
        if (riid == IID_IUnknown ||
            riid == IID_IDispatch ||
            riid == kDIIDWordApplicationEvents4) {
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
        ScopedInterlockedCount activeInvoke(m_activeInvokeCount);
        if (!LoadInterlockedFlag(m_active)) {
            SetInterlockedFlag(m_invokeAfterDeactivateObserved);
            if (g_control.shutdownRequested.load(std::memory_order_acquire)) {
                g_control.requiresPinnedShutdown.store(true,
                                                       std::memory_order_release);
            }
            return S_OK;
        }

        if (!IsOwnerThread()) {
            SetInterlockedFlag(m_offOwnerInvokeObserved);
            g_control.requiresPinnedShutdown.store(true,
                                                   std::memory_order_release);
            return S_OK;
        }

        try {
            ScopedOwnerRuntimeWork ownerWork;
            DispatchWordApplicationEvent(
                ClassifyWordApplicationEvent(m_dispIds, dispIdMember),
                params);
        } catch (...) {
            return E_UNEXPECTED;
        }
        return S_OK;
    }

    void Deactivate() {
        ClearInterlockedFlag(m_active);
    }

    bool HasActiveInvokes() const {
        return InterlockedCompareExchange(
                   const_cast<volatile LONG*>(&m_activeInvokeCount),
                   0,
                   0) != 0;
    }

    bool RequiresPinnedRundown() const {
        return LoadInterlockedFlag(m_offOwnerInvokeObserved) ||
               LoadInterlockedFlag(m_invokeAfterDeactivateObserved);
    }

private:
    volatile LONG m_refCount = 1;
    volatile LONG m_active = TRUE;
    volatile LONG m_activeInvokeCount = 0;
    volatile LONG m_offOwnerInvokeObserved = FALSE;
    volatile LONG m_invokeAfterDeactivateObserved = FALSE;
    WordEventDispIds m_dispIds = {};
};

void DeactivateWordApplicationEventSink(WordEventSession* session) {
    if (session && session->sinkControl) {
        session->sinkControl->Deactivate();
    }
}

bool IsTerminalWordEventUnadviseFailure(HRESULT hr) {
    return hr == kConnectENoConnection ||
           hr == kCoEObjectNotConnected ||
           hr == RPC_E_DISCONNECTED ||
           hr == RPC_E_SERVER_DIED ||
           hr == RPC_E_SERVER_DIED_DNE;
}

bool DidWordEventUnadviseComplete(HRESULT hr) {
    return SUCCEEDED(hr) || IsTerminalWordEventUnadviseFailure(hr);
}

bool IsRetryableWordEventUnadviseFailure(HRESULT hr) {
    return IsRetryableAutomationFailure(hr);
}

bool ShouldRetryShutdownWordEventUnadvise(HRESULT hr,
                                          int attemptIndex,
                                          int maxAttempts) {
    return IsRetryableWordEventUnadviseFailure(hr) &&
           attemptIndex + 1 < maxAttempts;
}

bool ShouldLogWordEventDisconnectFailureNow() {
    const ULONGLONG now = GetTickCount64();
    const ULONGLONG previous =
        g_runtime.status.lastEventDisconnectFailureLogTime;
    if (previous != 0 &&
        now - previous < WORD_EVENT_DISCONNECT_FAILURE_LOG_INTERVAL_MS) {
        return false;
    }

    g_runtime.status.lastEventDisconnectFailureLogTime = now;
    return true;
}

void LogWordEventDisconnectFailure(const wchar_t* message, HRESULT hr) {
    if (ShouldLogWordEventDisconnectFailureNow()) {
        Wh_Log(L"Word events: %ls, hr=0x%08X", message, hr);
    }
}

void RefreshPendingWordEventDisconnectRetryFlag() {
    bool pending = g_runtime.events.stagedSession.IsConnected() ||
                   (!LoadFlag(g_runtime.flags.wordEventsConnected) &&
                    g_runtime.events.session.IsConnected());
    for (int index = 0; index < WORD_EVENT_PENDING_DISCONNECT_CAPACITY; ++index) {
        if (g_runtime.events.pendingDisconnectSessions[index].IsConnected()) {
            pending = true;
            break;
        }
    }

    StoreFlag(g_runtime.flags.wordEventDisconnectRetryPending, pending);
}

WordEventSession* FindFreePendingWordEventDisconnectSlot() {
    for (int index = 0; index < WORD_EVENT_PENDING_DISCONNECT_CAPACITY; ++index) {
        WordEventSession* session =
            &g_runtime.events.pendingDisconnectSessions[index];
        if (session->IsEmpty()) {
            return session;
        }
    }

    return nullptr;
}

HRESULT UnadviseWordEventSessionWithInitializedCom(WordEventSession* session) {
    if (!session || !session->IsConnected()) {
        return S_FALSE;
    }

    return session->connectionPoint->Unadvise(session->cookie);
}

HRESULT UnadviseWordEventSessionForShutdown(WordEventSession* session) {
    HRESULT hr = S_FALSE;
    for (int attemptIndex = 0;
         attemptIndex < WORD_EVENT_SHUTDOWN_DISCONNECT_RETRY_ATTEMPTS;
         ++attemptIndex) {
        hr = UnadviseWordEventSessionWithInitializedCom(session);
        if (DidWordEventUnadviseComplete(hr) ||
            !ShouldRetryShutdownWordEventUnadvise(
                hr,
                attemptIndex,
                WORD_EVENT_SHUTDOWN_DISCONNECT_RETRY_ATTEMPTS)) {
            return hr;
        }

        Sleep(WORD_EVENT_SHUTDOWN_DISCONNECT_RETRY_DELAY_MS);
    }

    return hr;
}

bool ArmPendingWordEventDisconnectRetry() {
    if (!HasPendingWordEventDisconnectRetry()) {
        return false;
    }

    return ScheduleTask(ScheduledTaskKind::EventDisconnectRetry,
                        WORD_EVENT_DISCONNECT_RETRY_INTERVAL_MS);
}

void RequestWordEventDisconnectRetryWakeup() {
    if (ArmPendingWordEventDisconnectRetry()) {
        return;
    }

    // The retry flag stays set and will be armed after owner-thread adoption.
}

bool IsWordEventTeardownInProgress() {
    return g_runtime.events.primaryDisconnectDepth != 0 ||
           g_runtime.events.pendingDisconnectRetryDepth != 0;
}

bool IsWordEventConnectionBuildInProgress() {
    return g_runtime.events.connectionBuildDepth != 0;
}

bool CanCommitWordEventConnection(unsigned int expectedEpoch) {
    return !IsWordEventTeardownInProgress() &&
           g_runtime.events.connectionEpoch == expectedEpoch;
}

void RetryPendingWordEventDisconnects() {
    if (!HasPendingWordEventDisconnectRetry() ||
        g_runtime.events.primaryDisconnectDepth != 0 ||
        g_runtime.events.pendingDisconnectRetryDepth != 0) {
        return;
    }

    ScopedOwnerDepth retryDepth(
        g_runtime.events.pendingDisconnectRetryDepth);

    ScopedComInit comInit;
    if (FAILED(comInit.GetResult())) {
        LogWordEventDisconnectFailure(
            L"COM initialization failed while retrying deferred event disconnect",
            comInit.GetResult());
        RequestWordEventDisconnectRetryWakeup();
        return;
    }

    ScopedComMessageFilter messageFilter;
    // Index -1 is the runtime-owned candidate whose commit/rollback may have
    // been interrupted after Advise. It must follow the same retry policy as
    // retired primary sessions.
    for (int index = -1;
         index < WORD_EVENT_PENDING_DISCONNECT_CAPACITY;
         ++index) {
        WordEventSession* session =
            index < 0
                ? &g_runtime.events.stagedSession
                : &g_runtime.events.pendingDisconnectSessions[index];
        if (!session->IsConnected()) {
            session->Reset();
            continue;
        }

        DeactivateWordApplicationEventSink(session);
        const HRESULT hr = UnadviseWordEventSessionWithInitializedCom(session);
        if (DidWordEventUnadviseComplete(hr)) {
            if (SUCCEEDED(hr)) {
                Wh_Log(L"Word events: completed deferred event disconnect");
            } else {
                LogWordEventDisconnectFailure(
                    L"deferred unadvise no longer has an active connection",
                    hr);
            }
            session->Reset();
            continue;
        }

        LogWordEventDisconnectFailure(L"deferred unadvise still failed", hr);
    }

    // A primary session is restored in place when the retirement queue is
    // full. Once the loop above frees a slot, retry it automatically rather
    // than waiting for an unrelated reconnect or shutdown.
    if (!LoadFlag(g_runtime.flags.wordEventsConnected) &&
        g_runtime.events.session.IsConnected()) {
        DisconnectWordApplicationEvents(true);
    }

    RefreshPendingWordEventDisconnectRetryFlag();
    if (HasPendingWordEventDisconnectRetry()) {
        RequestWordEventDisconnectRetryWakeup();
    }
}

bool QueueWordEventSessionDisconnectRetry(WordEventSession* session) {
    if (!session || !session->IsConnected()) {
        return false;
    }

    DeactivateWordApplicationEventSink(session);
    WordEventSession* slot = FindFreePendingWordEventDisconnectSlot();
    if (!slot) {
        Wh_Log(L"Word events: no free deferred disconnect slot");
        return false;
    }

    *slot = std::move(*session);
    SetFlag(g_runtime.flags.wordEventDisconnectRetryPending);
    RequestWordEventDisconnectRetryWakeup();
    return true;
}

bool DisconnectWordEventSession(WordEventSession* session, bool allowDeferredRetry) {
    if (!session) {
        return true;
    }

    const bool connected = session->IsConnected();
    if (connected) {
        DeactivateWordApplicationEventSink(session);
    }

    ScopedComInit comInit;
    if (SUCCEEDED(comInit.GetResult()) &&
        connected) {
        ScopedComMessageFilter messageFilter;
        const HRESULT hr = allowDeferredRetry
                               ? UnadviseWordEventSessionWithInitializedCom(session)
                               : UnadviseWordEventSessionForShutdown(session);
        if (DidWordEventUnadviseComplete(hr)) {
            if (FAILED(hr)) {
                LogWordEventDisconnectFailure(
                    L"unadvise found no active application event connection",
                    hr);
            }
            session->Reset();
            return true;
        }

        LogWordEventDisconnectFailure(
            allowDeferredRetry
                ? L"failed to unadvise application events"
                : L"shutdown unadvise failed after bounded retries",
            hr);
        if (allowDeferredRetry &&
            QueueWordEventSessionDisconnectRetry(session)) {
            return true;
        }
    } else if (connected) {
        LogWordEventDisconnectFailure(
            L"COM initialization failed while disconnecting",
            comInit.GetResult());
        if (allowDeferredRetry &&
            QueueWordEventSessionDisconnectRetry(session)) {
            return true;
        }
    }

    if (connected && session->IsConnected()) {
        // The caller owns persistent storage for this session. Never release a
        // still-advised sink merely because the bounded retirement queue is
        // full; retain it and stop new connection builds until Unadvise wins.
        SetFlag(g_runtime.flags.wordEventDisconnectRetryPending);
        if (allowDeferredRetry) {
            RequestWordEventDisconnectRetryWakeup();
        }
        return false;
    }

    session->Reset();
    return true;
}

bool DisconnectWordApplicationEvents(bool allowDeferredRetry) {
    if (g_runtime.events.primaryDisconnectDepth != 0) {
        return g_runtime.events.session.IsEmpty();
    }

    ScopedOwnerDepth disconnectDepth(
        g_runtime.events.primaryDisconnectDepth);
    ++g_runtime.events.connectionEpoch;
    ClearFlag(g_runtime.flags.wordEventsConnected);
    InvalidateActiveDocumentCacheForEventCoverageGap();

    // Remove the observable session before Deactivate/Unadvise/Release. A
    // nested disconnect sees an empty global and Ensure is gated by depth.
    WordEventSession disconnectedSession(
        std::move(g_runtime.events.session));
    const bool disconnected =
        DisconnectWordEventSession(&disconnectedSession, allowDeferredRetry);
    if (!disconnected && !disconnectedSession.IsEmpty()) {
        // primaryDisconnectDepth prevents a nested connection commit, so the
        // primary slot is still empty here. Restore the live session rather
        // than letting the stack aggregate release an advised sink.
        g_runtime.events.session = std::move(disconnectedSession);
    }
    return disconnected;
}

void DisconnectPendingWordEventDisconnectSessionsForShutdown() {
    if (IsWordEventTeardownInProgress()) {
        return;
    }

    ScopedOwnerDepth retryDepth(
        g_runtime.events.pendingDisconnectRetryDepth);
    bool complete = true;
    for (int index = -1;
         index < WORD_EVENT_PENDING_DISCONNECT_CAPACITY;
         ++index) {
        WordEventSession* session =
            index < 0
                ? &g_runtime.events.stagedSession
                : &g_runtime.events.pendingDisconnectSessions[index];
        if (!DisconnectWordEventSession(session, false)) {
            complete = false;
        }
    }

    if (complete) {
        ClearFlag(g_runtime.flags.wordEventDisconnectRetryPending);
    } else {
        g_control.requiresPinnedShutdown.store(true,
                                               std::memory_order_release);
        RefreshPendingWordEventDisconnectRetryFlag();
    }
}

bool TryDisconnectWordEventSessionForOwnerShutdown(WordEventSession* session) {
    if (!session) {
        return true;
    }

    DeactivateWordApplicationEventSink(session);

    if (!session->IsConnected()) {
        if (session->sinkControl &&
            session->sinkControl->RequiresPinnedRundown()) {
            g_control.requiresPinnedShutdown.store(true,
                                                   std::memory_order_release);
            return false;
        }
        if (session->sinkControl && session->sinkControl->HasActiveInvokes()) {
            return false;
        }
        session->Reset();
        return true;
    }

    ScopedComInit comInit;
    if (FAILED(comInit.GetResult())) {
        LogWordEventDisconnectFailure(
            L"COM initialization failed during owner-thread shutdown",
            comInit.GetResult());
        return false;
    }

    ScopedComMessageFilter messageFilter;
    const HRESULT hr =
        UnadviseWordEventSessionWithInitializedCom(session);
    if (!DidWordEventUnadviseComplete(hr)) {
        LogWordEventDisconnectFailure(
            L"owner-thread shutdown is waiting to unadvise application events",
            hr);
        return false;
    }

    if (FAILED(hr)) {
        LogWordEventDisconnectFailure(
            L"shutdown unadvise found no active application event connection",
            hr);
    }

    // Preserve the connection point and sink references while any already
    // entered Invoke drains, but never call Unadvise for this cookie twice.
    session->cookie = 0;

    if (session->sinkControl &&
        session->sinkControl->RequiresPinnedRundown()) {
        g_control.requiresPinnedShutdown.store(true,
                                               std::memory_order_release);
        return false;
    }

    if (session->sinkControl && session->sinkControl->HasActiveInvokes()) {
        return false;
    }

    session->Reset();
    return true;
}

bool TryDisconnectAllWordEventsForOwnerShutdown() {
    if (IsWordEventTeardownInProgress()) {
        return false;
    }

    ScopedOwnerDepth disconnectDepth(
        g_runtime.events.primaryDisconnectDepth);
    ++g_runtime.events.connectionEpoch;
    ClearFlag(g_runtime.flags.wordEventsConnected);
    InvalidateActiveDocumentCacheForEventCoverageGap();

    bool complete =
        TryDisconnectWordEventSessionForOwnerShutdown(
            &g_runtime.events.session);

    if (!TryDisconnectWordEventSessionForOwnerShutdown(
            &g_runtime.events.stagedSession)) {
        complete = false;
    }

    for (int index = 0;
         index < WORD_EVENT_PENDING_DISCONNECT_CAPACITY;
         ++index) {
        if (!TryDisconnectWordEventSessionForOwnerShutdown(
                &g_runtime.events.pendingDisconnectSessions[index])) {
            complete = false;
        }
    }

    if (complete) {
        ClearFlag(g_runtime.flags.wordEventDisconnectRetryPending);
    }
    return complete;
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
    session->sinkControl = sink;
    session->cookie = cookie;
    return S_OK;
}

HRESULT CommitWordEventSession(WordEventSession* session,
                               unsigned int expectedEpoch) {
    if (!session) {
        return E_POINTER;
    }

    if (!CanCommitWordEventConnection(expectedEpoch)) {
        ScopedOwnerDepth teardownDepth(
            g_runtime.events.pendingDisconnectRetryDepth);
        DisconnectWordEventSession(session, true);
        return E_ABORT;
    }

    CacheWordApplication(session->application.Get(), session->applicationHwnd);
    if (!CanCommitWordEventConnection(expectedEpoch)) {
        ScopedOwnerDepth teardownDepth(
            g_runtime.events.pendingDisconnectRetryDepth);
        DisconnectWordEventSession(session, true);
        return E_ABORT;
    }

    if (!DisconnectWordApplicationEvents()) {
        ScopedOwnerDepth teardownDepth(
            g_runtime.events.pendingDisconnectRetryDepth);
        DisconnectWordEventSession(session, true);
        return E_ABORT;
    }
    g_runtime.events.session = std::move(*session);
    SetFlag(g_runtime.flags.wordEventsConnected);
    return S_OK;
}

HRESULT ConnectWordApplicationEvents(IDispatch* application,
                                      unsigned int expectedEpoch) {
    WordEventSession* session = &g_runtime.events.stagedSession;
    if (!session->IsEmpty()) {
        ScopedOwnerDepth teardownDepth(
            g_runtime.events.pendingDisconnectRetryDepth);
        if (!DisconnectWordEventSession(session, true)) {
            return E_PENDING;
        }
    }

    // Keep one normal retirement slot available before Advise. If commit is
    // invalidated by re-entrancy, the staged session can then be retired
    // without dropping a live callback object.
    if (!FindFreePendingWordEventDisconnectSlot()) {
        SetFlag(g_runtime.flags.wordEventDisconnectRetryPending);
        RequestWordEventDisconnectRetryWakeup();
        return E_PENDING;
    }

    const HRESULT hr = BuildWordEventSession(application, session);
    if (FAILED(hr)) {
        return hr;
    }

    return CommitWordEventSession(session, expectedEpoch);
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
        // GetWordApplication can return the strong cached proxy before trying
        // the native/ROT sources. Evict a proxy that no longer validates so a
        // later reconnect can resolve the live application instead of looping
        // on the same dead object.
        ResetAutomationCacheState();
        return HRESULT_FROM_WIN32(ERROR_NOT_FOUND);
    }

    return S_OK;
}

WordEventConnectionResult EnsureWordEventSessionConnected(bool forceReconnect) {
    WordEventConnectionResult result = {};

    if (!IsOwnerThreadSchedulerContextValid()) {
        return result;
    }

    if (IsWordEventTeardownInProgress() ||
        IsWordEventConnectionBuildInProgress()) {
        return result;
    }

    ScopedOwnerDepth connectionBuildDepth(
        g_runtime.events.connectionBuildDepth);

    RetryPendingWordEventDisconnects();
    if (IsWordEventTeardownInProgress()) {
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

    if (IsWordEventTeardownInProgress()) {
        return result;
    }

    ScopedComInit comInit;
    if (FAILED(comInit.GetResult())) {
        return result;
    }

    ScopedComMessageFilter messageFilter(ComRetryProfile::Background);
    const unsigned int expectedEpoch =
        g_runtime.events.connectionEpoch;

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

    if (!CanCommitWordEventConnection(expectedEpoch)) {
        return result;
    }

    if (ShouldReuseWordEventConnectionForHwnd(forceReconnect,
                                              wordEventsConnected,
                                              g_runtime.events.session.applicationHwnd,
                                              applicationHwnd)) {
        result.outcome = WordEventConnectionOutcome::Reused;
        return result;
    }

    hr = ConnectWordApplicationEvents(application.Get(), expectedEpoch);
    if (FAILED(hr)) {
        if (hr == E_ABORT || hr == E_PENDING) {
            return result;
        }
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
        ShouldLogFailureNow(&g_runtime.status.lastDocumentStateFailureLogTime,
                            DOCUMENT_FAILURE_LOG_INTERVAL_MS)) {
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
    Failed,
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
    SpecificDocument,
};

enum class SnapshotClearedReason {
    None,
    NoDocument,
    ReadOnly,
    NoPath,
    MissingByPath,
    ProtectedView,
};

enum class SnapshotMetadataState {
    NotRequested,
    Cached,
    Refreshed,
    Unavailable,
};

struct ActiveDocumentSnapshot {
    ScopedComPtr<IDispatch> document;
    ScopedComPtr<IUnknown> identity;
    ScopedBstr path;
    SnapshotClearedReason clearedReason = SnapshotClearedReason::None;
    SnapshotMetadataState metadataState = SnapshotMetadataState::NotRequested;
    AutomationFailureClass failureClass = AutomationFailureClass::None;
    HRESULT failureHr = S_OK;
    bool readOnly = false;
    bool saved = true;
    bool hasPath = false;

    void Reset() {
        document.Reset();
        identity.Reset();
        path.Reset();
        clearedReason = SnapshotClearedReason::None;
        metadataState = SnapshotMetadataState::NotRequested;
        failureClass = AutomationFailureClass::None;
        failureHr = S_OK;
        readOnly = false;
        saved = true;
        hasPath = false;
    }
};

void RecordSnapshotFailure(ActiveDocumentSnapshot* snapshot, HRESULT hr) {
    if (!snapshot) {
        return;
    }

    const HRESULT effectiveHr = FAILED(hr) ? hr : E_FAIL;
    snapshot->failureHr = effectiveHr;
    snapshot->failureClass = ClassifyAutomationFailure(effectiveHr);
}

SaveRetryReason GetSaveRetryReasonForSnapshot(
    const ActiveDocumentSnapshot* snapshot) {
    const AutomationFailureClass failureClass =
        snapshot ? snapshot->failureClass : AutomationFailureClass::Hard;
    switch (failureClass) {
        case AutomationFailureClass::Busy:
            return SaveRetryReason::AutomationBusy;
        case AutomationFailureClass::Disconnected:
            return SaveRetryReason::TargetUnavailable;
        case AutomationFailureClass::Hard:
        case AutomationFailureClass::None:
            return SaveRetryReason::HardFailure;
    }

    return SaveRetryReason::HardFailure;
}

DocumentRetryReason GetDocumentRetryReasonForSnapshot(
    const ActiveDocumentSnapshot* snapshot) {
    const AutomationFailureClass failureClass =
        snapshot ? snapshot->failureClass : AutomationFailureClass::Hard;
    switch (failureClass) {
        case AutomationFailureClass::Busy:
            return DocumentRetryReason::Busy;
        case AutomationFailureClass::Disconnected:
            return DocumentRetryReason::Disconnected;
        case AutomationFailureClass::Hard:
        case AutomationFailureClass::None:
            return DocumentRetryReason::Hard;
    }

    return DocumentRetryReason::Hard;
}

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
    const DWORD intervalMs = context == SnapshotLogContext::Save
                                 ? SAVE_FAILURE_LOG_INTERVAL_MS
                                 : DOCUMENT_FAILURE_LOG_INTERVAL_MS;
    if (!ShouldLogFailureNow(lastLogTime, intervalMs)) {
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
    IDispatch* document = nullptr;
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

    const HRESULT hr = GetBoolProperty(snapshot->document.Get(),
                                       WordMember::DocumentSaved,
                                       &snapshot->saved);
    if (FAILED(hr)) {
        RecordSnapshotFailure(snapshot, hr);
        if (IsTerminalAutomationObjectFailure(hr)) {
            InvalidateCachedActiveDocumentIfMatches(snapshot->document.Get());
        }
        LogSnapshotFailure(context, L"failed to query Saved state", hr);
        return SnapshotQueryResult::RetryLater;
    }

    return SnapshotQueryResult::Ready;
}

SnapshotQueryResult CopyCachedActiveDocumentMetadata(
    ActiveDocumentSnapshot* snapshot) {
    if (!snapshot || !snapshot->document ||
        !AreWordEventsConnected() ||
        g_runtime.automation.activeDocumentStale ||
        !g_runtime.automation.activeDocumentMetadataValid ||
        !IsCachedActiveDocument(snapshot->document.Get()) ||
        !g_runtime.automation.activeDocumentIdentity) {
        return SnapshotQueryResult::RetryLater;
    }

    ReplaceStoredComPtr(
        &snapshot->identity,
        g_runtime.automation.activeDocumentIdentity.Get());
    snapshot->readOnly = g_runtime.automation.activeDocumentReadOnly;
    snapshot->hasPath = g_runtime.automation.activeDocumentHasPath;
    snapshot->metadataState = SnapshotMetadataState::Cached;

    if (snapshot->hasPath &&
        !ReplaceStoredTextBstr(&snapshot->path,
                               g_runtime.automation.activeDocumentPath.Get())) {
        snapshot->metadataState = SnapshotMetadataState::Unavailable;
        RecordSnapshotFailure(snapshot, E_OUTOFMEMORY);
        return SnapshotQueryResult::RetryLater;
    }

    if (snapshot->readOnly) {
        snapshot->clearedReason = SnapshotClearedReason::ReadOnly;
        return SnapshotQueryResult::Cleared;
    }

    if (!snapshot->hasPath) {
        snapshot->clearedReason = SnapshotClearedReason::NoPath;
        return SnapshotQueryResult::Cleared;
    }

    return SnapshotQueryResult::Ready;
}

void CacheActiveDocumentMetadata(const ActiveDocumentSnapshot* snapshot) {
    if (!snapshot || !snapshot->document || !snapshot->identity ||
        !IsCachedActiveDocument(snapshot->document.Get())) {
        return;
    }

    if (snapshot->hasPath &&
        !ReplaceStoredTextBstr(&g_runtime.automation.activeDocumentPath,
                               snapshot->path.Get())) {
        g_runtime.automation.activeDocumentMetadataValid = false;
        return;
    }

    if (!snapshot->hasPath) {
        g_runtime.automation.activeDocumentPath.Reset();
    }

    g_runtime.automation.activeDocumentReadOnly = snapshot->readOnly;
    g_runtime.automation.activeDocumentHasPath = snapshot->hasPath;
    g_runtime.automation.activeDocumentMetadataValid = true;
}

SnapshotQueryResult PopulateSnapshotMetadata(ActiveDocumentSnapshot* snapshot,
                                             SnapshotLogContext context) {
    if (!snapshot || !snapshot->document) {
        return SnapshotQueryResult::Cleared;
    }

    const SnapshotQueryResult cachedResult =
        CopyCachedActiveDocumentMetadata(snapshot);
    if (cachedResult != SnapshotQueryResult::RetryLater) {
        return cachedResult;
    }

    snapshot->failureClass = AutomationFailureClass::None;
    snapshot->failureHr = S_OK;
    snapshot->metadataState = SnapshotMetadataState::Unavailable;

    HRESULT hr = S_OK;
    if (snapshot->document.Get() ==
            g_runtime.automation.activeDocument.Get() &&
        g_runtime.automation.activeDocumentIdentity) {
        ReplaceStoredComPtr(
            &snapshot->identity,
            g_runtime.automation.activeDocumentIdentity.Get());
    } else {
        hr = GetComIdentity(snapshot->document.Get(), snapshot->identity.Put());
    }
    if (FAILED(hr) || !snapshot->identity) {
        const HRESULT failureHr = FAILED(hr) ? hr : E_FAIL;
        RecordSnapshotFailure(snapshot, failureHr);
        if (IsTerminalAutomationObjectFailure(hr)) {
            InvalidateCachedActiveDocumentIfMatches(snapshot->document.Get());
        }
        LogSnapshotFailure(context, L"failed to resolve document COM identity", hr);
        return SnapshotQueryResult::RetryLater;
    }

    hr = GetBoolProperty(snapshot->document.Get(),
                         WordMember::DocumentReadOnly,
                         &snapshot->readOnly);
    if (FAILED(hr)) {
        RecordSnapshotFailure(snapshot, hr);
        if (IsTerminalAutomationObjectFailure(hr)) {
            InvalidateCachedActiveDocumentIfMatches(snapshot->document.Get());
        }
        LogSnapshotFailure(context, L"failed to query ReadOnly", hr);
        return SnapshotQueryResult::RetryLater;
    }

    if (snapshot->readOnly) {
        snapshot->metadataState = SnapshotMetadataState::Refreshed;
        CacheActiveDocumentMetadata(snapshot);
        snapshot->clearedReason = SnapshotClearedReason::ReadOnly;
        return SnapshotQueryResult::Cleared;
    }

    hr = GetDocumentIdentityAndPathState(snapshot->document.Get(),
                                         snapshot->path.Put(),
                                         &snapshot->hasPath);
    if (FAILED(hr)) {
        RecordSnapshotFailure(snapshot, hr);
        if (IsTerminalAutomationObjectFailure(hr)) {
            InvalidateCachedActiveDocumentIfMatches(snapshot->document.Get());
        }
        LogSnapshotFailure(context, L"failed to query Path/Name identity", hr);
        return SnapshotQueryResult::RetryLater;
    }

    snapshot->metadataState = SnapshotMetadataState::Refreshed;
    CacheActiveDocumentMetadata(snapshot);

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

bool ShouldInvalidateTransitionFlushTargetForFailure(
    AutomationFailureClass failureClass) {
    return failureClass == AutomationFailureClass::Disconnected;
}

bool ShouldRetryInvalidCachedSnapshotDocument(
    bool usingCachedSpecificDocument,
    bool retriedAfterInvalidCachedDocument,
    AutomationFailureClass failureClass) {
    return usingCachedSpecificDocument &&
           !retriedAfterInvalidCachedDocument &&
           ShouldInvalidateTransitionFlushTargetForFailure(failureClass);
}

void PrepareInvalidCachedSnapshotDocumentRetry(
    ActiveDocumentSnapshot* snapshot,
    bool* retriedAfterInvalidCachedDocument) {
    if (snapshot) {
        snapshot->Reset();
    }

    InvalidateTransitionFlushDocumentCache();
    if (retriedAfterInvalidCachedDocument) {
        *retriedAfterInvalidCachedDocument = true;
    }
}

SnapshotQueryResult ResolveSpecificSnapshotDocument(const SnapshotRequest& request,
                                                    ActiveDocumentSnapshot* snapshot) {
    if (!snapshot) {
        return SnapshotQueryResult::Cleared;
    }

    const HRESULT hr = GetWordDocumentByPath(request.path, snapshot->document.Put());
    if (FAILED(hr) || !snapshot->document) {
        if (FAILED(hr) &&
            hr != HRESULT_FROM_WIN32(ERROR_NOT_FOUND)) {
            RecordSnapshotFailure(snapshot, FAILED(hr) ? hr : E_FAIL);
            return SnapshotQueryResult::RetryLater;
        }

        snapshot->clearedReason = SnapshotClearedReason::MissingByPath;
        return SnapshotQueryResult::Cleared;
    }

    return SnapshotQueryResult::Ready;
}

SnapshotQueryResult ResolveSpecificSnapshotDocumentObject(const SnapshotRequest& request,
                                                         ActiveDocumentSnapshot* snapshot) {
    if (!snapshot) {
        return SnapshotQueryResult::Cleared;
    }

    if (!request.document) {
        snapshot->clearedReason = SnapshotClearedReason::MissingByPath;
        return SnapshotQueryResult::Cleared;
    }

    request.document->AddRef();
    snapshot->document.Reset(request.document);
    return SnapshotQueryResult::Ready;
}

SnapshotQueryResult EnsureSnapshotApplication(ScopedComPtr<IDispatch>* application,
                                              ActiveDocumentSnapshot* snapshot,
                                              SnapshotLogContext context) {
    if (!application) {
        return SnapshotQueryResult::Cleared;
    }

    HRESULT hr = GetWordApplication(application->Put());
    if (FAILED(hr) || !application->Get()) {
        const HRESULT failureHr = FAILED(hr) ? hr : E_FAIL;
        RecordSnapshotFailure(snapshot, failureHr);
        if (IsRetryableAutomationFailure(hr)) {
            return SnapshotQueryResult::RetryLater;
        }

        LogSnapshotFailure(context, L"failed to get Word application", failureHr);
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
        GetDispatchProperty(application,
                            WordMember::ApplicationActiveDocument,
                            snapshot->document.Put());
    if (FAILED(hr) || !snapshot->document) {
        if (FAILED(hr)) {
            RecordSnapshotFailure(snapshot, hr);
            if (IsTerminalAutomationObjectFailure(hr)) {
                ResetAutomationCacheState();
                DisconnectWordApplicationEvents();
            }
            return SnapshotQueryResult::RetryLater;
        }
        return SnapshotQueryResult::Cleared;
    }

    const HRESULT cacheHr = CacheActiveDocumentBinding(snapshot->document.Get());
    if (FAILED(cacheHr)) {
        RecordSnapshotFailure(snapshot, cacheHr);
        snapshot->document.Reset();
        return SnapshotQueryResult::RetryLater;
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
        const HRESULT hr = GetDispatchProperty(
            application,
            WordMember::ApplicationActiveProtectedViewWindow,
            protectedViewWindow.Put());
        if (SUCCEEDED(hr) &&
            protectedViewWindow) {
            snapshot->clearedReason = SnapshotClearedReason::ProtectedView;
            return SnapshotQueryResult::Cleared;
        }

        if (FAILED(hr)) {
            RecordSnapshotFailure(snapshot, hr);
            if (IsTerminalAutomationObjectFailure(hr)) {
                ResetAutomationCacheState();
                DisconnectWordApplicationEvents();
            }
            return SnapshotQueryResult::RetryLater;
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

    switch (request.source) {
        case SnapshotSource::SpecificPath:
            return ResolveSpecificSnapshotDocument(request, snapshot);

        case SnapshotSource::SpecificDocument:
            return ResolveSpecificSnapshotDocumentObject(request, snapshot);

        case SnapshotSource::Active:
            break;
    }

    if (AreWordEventsConnected() &&
        SUCCEEDED(CopyCachedActiveDocument(snapshot->document.Put())) &&
        snapshot->document) {
        return SnapshotQueryResult::Ready;
    }

    ScopedComPtr<IDispatch> application;
    const SnapshotQueryResult applicationResult =
        EnsureSnapshotApplication(&application, snapshot, request.context);
    if (applicationResult != SnapshotQueryResult::Ready) {
        return applicationResult;
    }

    const SnapshotQueryResult activeDocumentResult =
        ResolveActiveSnapshotDocumentFromApplication(application.Get(), snapshot);
    if (activeDocumentResult != SnapshotQueryResult::Cleared) {
        return activeDocumentResult;
    }

    const HRESULT nativeDocumentHr =
        GetWordDocumentFromActiveWindow(snapshot->document.Put());
    switch (ClassifyNativeDocumentLookupResult(
        nativeDocumentHr,
        static_cast<bool>(snapshot->document))) {
        case NativeDocumentLookupOutcome::Ready: {
            const HRESULT cacheHr =
                CacheActiveDocumentBinding(snapshot->document.Get());
            if (FAILED(cacheHr)) {
                RecordSnapshotFailure(snapshot, cacheHr);
            }
            return SUCCEEDED(cacheHr) ? SnapshotQueryResult::Ready
                                      : SnapshotQueryResult::RetryLater;
        }

        case NativeDocumentLookupOutcome::Missing:
            CacheActiveDocumentBinding(nullptr);
            return FinalizeMissingActiveSnapshotDocument(application.Get(),
                                                          snapshot);

        case NativeDocumentLookupOutcome::RetryLater:
            RecordSnapshotFailure(snapshot, nativeDocumentHr);
            if (IsTerminalAutomationObjectFailure(nativeDocumentHr)) {
                ResetAutomationCacheState();
                DisconnectWordApplicationEvents();
            }
            return SnapshotQueryResult::RetryLater;
    }

    return SnapshotQueryResult::RetryLater;
}

SnapshotQueryResult ResolveSnapshotDocumentFromRequest(const SnapshotRequest& request,
                                                       ActiveDocumentSnapshot* snapshot) {
    if (!snapshot) {
        return SnapshotQueryResult::Cleared;
    }

    if (request.source == SnapshotSource::SpecificPath &&
        request.path &&
        *request.path) {
        ScopedComPtr<IDispatch> cachedDocument;
        if (CopyValidTransitionFlushDocument(request.path,
                                             &cachedDocument)) {
            snapshot->Reset();
            snapshot->document.Reset(cachedDocument.Detach());
            return SnapshotQueryResult::Ready;
        }
    }

    if (request.source == SnapshotSource::SpecificDocument &&
        request.document) {
        ScopedComPtr<IDispatch> cachedDocument;
        if (CopyValidTransitionFlushDocument(nullptr,
                                             &cachedDocument) &&
            AreSameDispatchComIdentity(request.document,
                                       cachedDocument.Get())) {
            snapshot->Reset();
            snapshot->document.Reset(cachedDocument.Detach());
            return SnapshotQueryResult::Ready;
        }
    }

    return ResolveSnapshotDocument(request, snapshot);
}

SnapshotQueryResult ExecuteSnapshotLoadPlan(const SnapshotLoadPlan& plan,
                                           ActiveDocumentSnapshot* snapshot) {
    if (!snapshot) {
        return SnapshotQueryResult::Cleared;
    }

    bool retriedAfterInvalidCachedDocument = false;
    for (;;) {
        const SnapshotQueryResult documentResult =
            ResolveSnapshotDocumentFromRequest(plan.request, snapshot);
        if (documentResult != SnapshotQueryResult::Ready) {
            return documentResult;
        }

        ScopedComPtr<IDispatch> cachedSpecificDocument;
        const bool usingCachedSpecificDocument =
            (plan.request.source == SnapshotSource::SpecificPath ||
             plan.request.source == SnapshotSource::SpecificDocument) &&
            snapshot->document &&
            CopyValidTransitionFlushDocument(nullptr,
                                             &cachedSpecificDocument) &&
            AreSameDispatchComIdentity(snapshot->document.Get(),
                                       cachedSpecificDocument.Get());
        const SnapshotQueryResult savedStateResult =
            PopulateSnapshotSavedState(snapshot, plan.request.context);
        if (savedStateResult != SnapshotQueryResult::Ready) {
            if (ShouldRetryInvalidCachedSnapshotDocument(
                    usingCachedSpecificDocument,
                    retriedAfterInvalidCachedDocument,
                    snapshot->failureClass)) {
                PrepareInvalidCachedSnapshotDocumentRetry(
                    snapshot,
                    &retriedAfterInvalidCachedDocument);
                continue;
            }

            if (usingCachedSpecificDocument &&
                ShouldInvalidateTransitionFlushTargetForFailure(
                    snapshot->failureClass)) {
                InvalidateTransitionFlushDocumentCache();
            }
            return savedStateResult;
        }

        if (!ShouldPopulateSnapshotMetadataForMode(snapshot->saved, plan.metadataMode)) {
            return SnapshotQueryResult::Ready;
        }

        const SnapshotQueryResult metadataResult =
            PopulateSnapshotMetadata(snapshot, plan.request.context);
        if (metadataResult != SnapshotQueryResult::Ready) {
            if (ShouldRetryInvalidCachedSnapshotDocument(
                    usingCachedSpecificDocument,
                    retriedAfterInvalidCachedDocument,
                    snapshot->failureClass)) {
                PrepareInvalidCachedSnapshotDocumentRetry(
                    snapshot,
                    &retriedAfterInvalidCachedDocument);
                continue;
            }

            if (usingCachedSpecificDocument &&
                ShouldInvalidateTransitionFlushTargetForFailure(
                    snapshot->failureClass)) {
                InvalidateTransitionFlushDocumentCache();
            }
            return metadataResult;
        }

        if (!usingCachedSpecificDocument ||
            plan.request.source == SnapshotSource::SpecificDocument ||
            (snapshot->hasPath &&
             AreSameResolvedDocumentPath(snapshot->path.CStr(),
                                         plan.request.path))) {
            return SnapshotQueryResult::Ready;
        }

        snapshot->Reset();
        if (!ShouldRetryInvalidCachedSnapshotDocument(
                usingCachedSpecificDocument,
                retriedAfterInvalidCachedDocument,
                AutomationFailureClass::Disconnected)) {
            return SnapshotQueryResult::Cleared;
        }

        PrepareInvalidCachedSnapshotDocumentRetry(
            snapshot,
            &retriedAfterInvalidCachedDocument);
    }
}

SnapshotLoadPlan MakeDocumentStateSnapshotLoadPlan(bool requireCleanSnapshotDetails) {
    SnapshotLoadPlan plan = {};
    plan.request.source = SnapshotSource::Active;
    plan.request.context = SnapshotLogContext::DocumentState;
    plan.metadataMode = requireCleanSnapshotDetails ? SnapshotMetadataMode::Always
                                                    : SnapshotMetadataMode::WhenDirty;
    return plan;
}

SnapshotLoadPlan MakeSaveSnapshotLoadPlan(const wchar_t* specificPath,
                                          IDispatch* specificDocument = nullptr) {
    SnapshotLoadPlan plan = {};
    if (specificDocument) {
        plan.request.source = SnapshotSource::SpecificDocument;
        plan.request.document = specificDocument;
    } else {
        plan.request.source =
            (specificPath && *specificPath) ? SnapshotSource::SpecificPath : SnapshotSource::Active;
    }
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

bool SetObservedDocumentFromSnapshot(const ActiveDocumentSnapshot* snapshot, bool dirty) {
    if (!snapshot || !snapshot->hasPath || !snapshot->path.Length() || !snapshot->identity) {
        ResetObservedDocumentState();
        return false;
    }

    const unsigned int expectedGeneration =
        g_runtime.document.observedDocumentGeneration;
    ScopedBstr nextPath;
    ScopedComPtr<IUnknown> nextIdentity;
    ScopedComPtr<IDispatch> nextDocument;
    if (!ReplaceStoredBstr(&nextPath, snapshot->path.CStr()) ||
        !ReplaceStoredComPtr(&nextIdentity, snapshot->identity.Get()) ||
        (dirty &&
         !ReplaceStoredComPtr(&nextDocument, snapshot->document.Get()))) {
        if (g_runtime.document.observedDocumentGeneration !=
            expectedGeneration) {
            return true;
        }
        ResetObservedDocumentState();
        return false;
    }

    // AddRef on an automation proxy is normally local, but a custom object can
    // still re-enter. Preserve any newer nested observation.
    if (g_runtime.document.observedDocumentGeneration !=
        expectedGeneration) {
        return true;
    }

    ClearFlag(g_runtime.flags.documentDirtyKnown);
    ScopedBstr previousPath;
    DetachedDocumentBinding previousBinding;
    previousPath.Reset(g_runtime.document.observedDocumentPath.Detach());
    DetachDocumentBinding(&g_runtime.document.observedDocument,
                          &g_runtime.document.observedDocumentIdentity,
                          &previousBinding);

    g_runtime.document.observedDocumentPath.Reset(nextPath.Detach());
    g_runtime.document.observedDocumentIdentity.Reset(nextIdentity.Detach());
    g_runtime.document.observedDocument.Reset(nextDocument.Detach());
    ++g_runtime.document.observedDocumentGeneration;
    StoreFlag(g_runtime.flags.documentDirty, dirty);
    SetFlag(g_runtime.flags.documentDirtyKnown);
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
        !AreSameDocumentPathText(g_runtime.document.observedDocumentPath.Get(),
                                 snapshot->path.CStr());
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

void FinalizeSaveAttemptState(
    const ActiveDocumentSnapshot* snapshot,
    bool clearPendingSave,
    bool clearTransitionRequest,
    TransitionFlushClearMode transitionClearMode);

bool TryMigrateObservedDocumentIdentity(
    const ActiveDocumentSnapshot* snapshot,
    bool clearPendingAutosaveOnClean,
    TransitionFlushClearMode transitionClearMode =
        TransitionFlushClearMode::ClearPostTransitionRefresh) {
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
        !AreSameDocumentPathText(g_runtime.document.observedDocumentPath.Get(),
                                 snapshot->path.CStr());
    if (!pathChanged) {
        if (saveAsIdentityMatch) {
            ClearPendingSaveAsMigration();
        }
        return false;
    }

    if (snapshot->saved) {
        g_runtime.timing.lastSaveTime = GetTickCount64();
        FinalizeSaveAttemptState(snapshot,
                                 clearPendingAutosaveOnClean,
                                 true,
                                 transitionClearMode);
    } else {
        const unsigned int expectedSaveAsGeneration =
            g_runtime.document.pendingSaveAsGeneration;
        const bool wasDirty = LoadFlag(g_runtime.flags.documentDirty);
        if (!SetObservedDocumentFromSnapshot(snapshot, wasDirty)) {
            return false;
        }

        if (g_runtime.document.pendingSaveAsGeneration ==
            expectedSaveAsGeneration) {
            ClearPendingSaveAsMigration();
        }
    }

    LogDocumentStateStatus(L"migrated the tracked document after Save As or rename");
    return true;
}

bool ShouldTransitionFlushObservedDocument(const ActiveDocumentSnapshot* snapshot) {
    return snapshot &&
           snapshot->hasPath &&
           g_runtime.document.observedDocumentPath.Length() != 0 &&
           !AreSameDocumentPathText(
               snapshot->path.CStr(),
               g_runtime.document.observedDocumentPath.Get()) &&
           HasPendingAutosave();
}

void BeginTransitionFlushForObservedDocument() {
    RequestTransitionFlush(g_runtime.document.observedDocumentPath.Get(),
                           L"finishing the previous document before switching to another one",
                           g_runtime.document.observedDocument.Get());
    ArmSaveTimer(INPUT_SETTLE_DELAY_MS);
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
                              bool clearTransitionRequest,
                              TransitionFlushClearMode transitionClearMode =
                                  TransitionFlushClearMode::ClearPostTransitionRefresh) {
    const unsigned int expectedObservedGeneration =
        g_runtime.document.observedDocumentGeneration;
    const unsigned int expectedTransitionGeneration =
        g_runtime.document.transitionFlushGeneration;
    const unsigned int expectedSaveAsGeneration =
        g_runtime.document.pendingSaveAsGeneration;

    ScopedBstr nextObservedPath;
    ScopedComPtr<IUnknown> nextObservedIdentity;
    bool publishObserved = snapshot && snapshot->hasPath &&
                           snapshot->path.Length() != 0 && snapshot->identity;
    if (publishObserved &&
        (!ReplaceStoredBstr(&nextObservedPath, snapshot->path.CStr()) ||
         !ReplaceStoredComPtr(&nextObservedIdentity,
                              snapshot->identity.Get()))) {
        publishObserved = false;
    }

    // If staging re-entered a newer state transition, preserve the nested
    // work rather than finalizing the older save attempt over it.
    if (g_runtime.document.observedDocumentGeneration !=
            expectedObservedGeneration ||
        g_runtime.document.pendingSaveAsGeneration !=
            expectedSaveAsGeneration ||
        (clearTransitionRequest &&
         g_runtime.document.transitionFlushGeneration !=
             expectedTransitionGeneration)) {
        return;
    }

    if (clearPendingSave) {
        ClearPendingSave();
    }
    ClearManualSavePending();

    ScopedComPtr<IUnknown> previousSaveAsIdentity;
    ClearFlag(g_runtime.flags.pendingSaveAsMigration);
    ++g_runtime.document.pendingSaveAsGeneration;
    g_runtime.timing.pendingSaveAsTime = 0;
    previousSaveAsIdentity.Reset(
        g_runtime.document.pendingSaveAsDocumentIdentity.Detach());

    ScopedBstr previousTransitionPath;
    DetachedDocumentBinding previousTransitionBinding;
    if (clearTransitionRequest) {
        ClearFlag(g_runtime.flags.transitionFlushPending);
        ++g_runtime.document.transitionFlushGeneration;
        g_runtime.timing.transitionFlushRequestTime = 0;
        if (transitionClearMode ==
            TransitionFlushClearMode::ClearPostTransitionRefresh) {
            ClearPostTransitionRefreshPending();
        }
        previousTransitionPath.Reset(
            g_runtime.document.transitionFlushDocumentPath.Detach());
        DetachDocumentBinding(
            &g_runtime.document.transitionFlushDocument,
            &g_runtime.document.transitionFlushDocumentIdentity,
            &previousTransitionBinding);
    }

    ScopedBstr previousObservedPath;
    DetachedDocumentBinding previousObservedBinding;
    ClearFlag(g_runtime.flags.documentDirtyKnown);
    ClearFlag(g_runtime.flags.documentDirty);
    ++g_runtime.document.observedDocumentGeneration;
    previousObservedPath.Reset(
        g_runtime.document.observedDocumentPath.Detach());
    DetachDocumentBinding(&g_runtime.document.observedDocument,
                          &g_runtime.document.observedDocumentIdentity,
                          &previousObservedBinding);

    if (publishObserved) {
        g_runtime.document.observedDocumentPath.Reset(
            nextObservedPath.Detach());
        g_runtime.document.observedDocumentIdentity.Reset(
            nextObservedIdentity.Detach());
        StoreFlag(g_runtime.flags.documentDirty, false);
        SetFlag(g_runtime.flags.documentDirtyKnown);
    }
}

TransitionFlushClearMode GetTransitionFlushClearModeForTick(
    const RuntimeTickSnapshot& tick) {
    return tick.runtime.transitionFlushRequested
               ? TransitionFlushClearMode::PreservePostTransitionRefresh
               : TransitionFlushClearMode::ClearPostTransitionRefresh;
}

void NoteDocumentStateRefreshReady() {
    ResetDocumentRetryState();
}

void NoteSaveOperationTime(ULONGLONG now) {
    g_runtime.timing.lastSaveTime = now;
}

void NoteSaveOperationReady() {
    ResetSaveRetryState();
}

bool ShouldResetSaveRetryAfterSuccessfulSnapshot(
    bool keepPendingForUnexpectedSnapshot) {
    return !keepPendingForUnexpectedSnapshot;
}

bool IsSnapshotExpectedForPendingAutosave(const ActiveDocumentSnapshot* snapshot,
                                          const RuntimeTickSnapshot& tick) {
    if (tick.runtime.transitionFlushRequested ||
        g_runtime.document.observedDocumentPath.Length() == 0) {
        return true;
    }

    if (!snapshot || !snapshot->hasPath) {
        return false;
    }

    return IsObservedDocumentSnapshot(snapshot) ||
           AreSameDocumentPathText(g_runtime.document.observedDocumentPath.Get(),
                                   snapshot->path.CStr());
}

bool KeepPendingAutosaveForUnexpectedSnapshot(const ActiveDocumentSnapshot* snapshot,
                                              const RuntimeTickSnapshot& tick) {
    if (IsSnapshotExpectedForPendingAutosave(snapshot, tick)) {
        return false;
    }

    LogSaveStatus(L"saved document did not match tracked pending document, keeping pending changes");
    if (g_runtime.document.observedDocumentPath.Length() != 0 ||
        g_runtime.document.observedDocument) {
        RequestTransitionFlush(g_runtime.document.observedDocumentPath.Get(),
                               L"retrying pending changes for the tracked document",
                               g_runtime.document.observedDocument.Get());
    }
    ArmSaveTimer(INPUT_SETTLE_DELAY_MS);
    return true;
}

void MaybeSchedulePostTransitionRefresh(const RuntimeTickSnapshot& tick) {
    if (!tick.runtime.transitionFlushRequested ||
        !LoadFlag(g_runtime.flags.postTransitionRefreshPending)) {
        return;
    }

    ClearPostTransitionRefreshPending();
    ArmDocumentStateTimer(INPUT_SETTLE_DELAY_MS);
}

enum class DocumentObservationApplyResult {
    None,
    RearmSteadyPoll,
    StartedTransitionFlush,
    ScheduledSave,
};

DocumentObservationApplyResult ApplyDirtyDocumentObservation(
    const ActiveDocumentSnapshot* snapshot,
    const RuntimeTickSnapshot& tick) {
    if (TryMigrateObservedDocumentIdentity(snapshot, false)) {
        return DocumentObservationApplyResult::RearmSteadyPoll;
    }

    if (ShouldTransitionFlushObservedDocument(snapshot)) {
        BeginTransitionFlushForObservedDocument();
        return DocumentObservationApplyResult::StartedTransitionFlush;
    }

    if (snapshot && snapshot->hasPath) {
        const bool newlyTrackedDirtyDocument = NoteObservedDocumentDirty(snapshot);
        if (tick.runtime.manualSavePending &&
            tick.runtime.pendingSave &&
            (IsObservedDocumentSnapshot(snapshot) ||
             AreSameDocumentPathText(
                 g_runtime.document.observedDocumentPath.Get(),
                 snapshot->path.CStr()))) {
            ClearManualSavePending();
            ClearCurrentSaveRetryReason();
            SetFlag(g_runtime.flags.expeditedSavePending);
            g_runtime.timing.lastEditTime = tick.now;
            LogSaveStatus(L"manual save did not finish, retrying pending auto-save");
            ArmSaveTimer(INPUT_SETTLE_DELAY_MS);
            return DocumentObservationApplyResult::ScheduledSave;
        }

        if (newlyTrackedDirtyDocument) {
            LogDocumentStateStatus(L"detected non-keyboard document change");
            ScheduleSaveFromEdit();
            return DocumentObservationApplyResult::ScheduledSave;
        }
    }

    return DocumentObservationApplyResult::None;
}

DocumentObservationApplyResult ApplyCleanDocumentObservation(
    const ActiveDocumentSnapshot* snapshot,
    const RuntimeTickSnapshot& tick) {
    if (snapshot &&
        snapshot->metadataState == SnapshotMetadataState::NotRequested) {
        return DocumentObservationApplyResult::None;
    }

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
            AreSameDocumentPathText(
                g_runtime.document.observedDocumentPath.Get(),
                snapshot->path.CStr());
        if (tick.runtime.pendingSave &&
            !sameObservedDocument &&
            g_runtime.document.observedDocumentPath.Length() == 0) {
            return DocumentObservationApplyResult::RearmSteadyPoll;
        }

        if (tick.runtime.manualSavePending) {
            ClearManualSavePending();
            g_runtime.timing.lastSaveTime = tick.now;
        }

        if (sameObservedDocument && tick.runtime.pendingSave) {
            ClearPendingSave();
        }

        SyncObservedDocumentCleanState(snapshot);
        if (sameObservedDocument && tick.runtime.pendingSave) {
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
    ActiveDocumentSnapshot localSnapshot;
    if (!snapshot) {
        snapshot = &localSnapshot;
    }
    snapshot->Reset();

    ScopedComInit comInit;
    if (FAILED(comInit.GetResult())) {
        RecordSnapshotFailure(snapshot, comInit.GetResult());
        LogSnapshotFailure(SnapshotLogContext::DocumentState,
                           L"CoInitializeEx failed",
                           comInit.GetResult());
        return DocumentDirtyState::RetryLater;
    }

    ScopedComMessageFilter messageFilter(ComRetryProfile::Background);

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
                                        const wchar_t* specificPath,
                                        IDispatch* specificDocument,
                                        ComRetryProfile retryProfile) {
    ActiveDocumentSnapshot localSnapshot;
    if (!snapshot) {
        snapshot = &localSnapshot;
    }
    snapshot->Reset();

    ScopedComInit comInit;
    if (FAILED(comInit.GetResult())) {
        RecordSnapshotFailure(snapshot, comInit.GetResult());
        LogSnapshotFailure(SnapshotLogContext::Save, L"CoInitializeEx failed", comInit.GetResult());
        return SaveAttemptResult::RetryLater;
    }

    ScopedComMessageFilter messageFilter(retryProfile);

    const SnapshotLoadPlan plan = MakeSaveSnapshotLoadPlan(specificPath, specificDocument);
    const SnapshotQueryResult snapshotResult = ExecuteSnapshotLoadPlan(plan, snapshot);
    const SaveAttemptResult snapshotInterpretation =
        InterpretSaveSnapshotQueryResult(specificPath, snapshot, snapshotResult);
    if (snapshotInterpretation != SaveAttemptResult::Saved) {
        return snapshotInterpretation;
    }

    if (snapshot->saved) {
        return SaveAttemptResult::AlreadyClean;
    }

    HRESULT hr = E_FAIL;
    {
        ScopedFlagSet autoSaveFlag(g_runtime.flags.autoSaveInProgress);
        hr = InvokeWordMember(snapshot->document.Get(),
                              DISPATCH_METHOD,
                              WordMember::DocumentSave);
    }
    if (SUCCEEDED(hr)) {
        return SaveAttemptResult::Saved;
    }

    if (IsRetryableAutomationFailure(hr)) {
        RecordSnapshotFailure(snapshot, hr);
        return SaveAttemptResult::RetryLater;
    }

    if (IsTerminalAutomationObjectFailure(hr)) {
        RecordSnapshotFailure(snapshot, hr);
        InvalidateCachedActiveDocumentIfMatches(snapshot->document.Get());
        if (ShouldInvalidateTransitionFlushTargetForFailure(
                snapshot->failureClass) &&
            snapshot->document.Get() ==
                g_runtime.document.transitionFlushDocument.Get()) {
            InvalidateTransitionFlushDocumentCache();
        }
        return SaveAttemptResult::RetryLater;
    }

    RecordSnapshotFailure(snapshot, hr);
    LogSnapshotFailure(SnapshotLogContext::Save, L"document save failed", hr);
    return SaveAttemptResult::Failed;
}

void HandleSchedulerTimerMessage(UINT_PTR timerId) {
    ScopedOwnerRuntimeWork ownerWork;

    if (!CanRunOwnerThreadRuntimeWork() ||
        timerId != SCHEDULER_WINDOW_TIMER_ID ||
        g_runtime.timing.schedulerTimerId != SCHEDULER_WINDOW_TIMER_ID) {
        return;
    }

    const ULONGLONG now = GetTickCount64();
    if (g_runtime.timing.schedulerTimerDueTime == 0 ||
        now < g_runtime.timing.schedulerTimerDueTime) {
        // KillTimer doesn't remove an already queued WM_TIMER. If an older
        // message arrives after the physical timer was moved, leave the new
        // timer intact and let its current deadline fire normally.
        return;
    }

    HWND messageWindow =
        g_control.messageWindow.load(std::memory_order_acquire);
    if (messageWindow) {
        KillTimer(messageWindow, SCHEDULER_WINDOW_TIMER_ID);
    }
    g_runtime.timing.schedulerTimerId = 0;
    g_runtime.timing.schedulerTimerDueTime = 0;

    const bool saveDue = IsScheduledTaskDue(ScheduledTaskKind::Save, now);
    if (saveDue &&
        CanRunOwnerThreadRuntimeWork()) {
        g_runtime.timing.saveTimerDueTime = 0;
        HandleAutosaveTick();
    }

    const ULONGLONG postSaveNow = GetTickCount64();
    if (CanRunOwnerThreadRuntimeWork() &&
        IsScheduledTaskDue(ScheduledTaskKind::EventDisconnectRetry, postSaveNow)) {
        g_runtime.timing.eventDisconnectRetryTimerDueTime = 0;
        RetryPendingWordEventDisconnects();
    }

    const ULONGLONG postEventDisconnectNow = GetTickCount64();
    if (CanRunOwnerThreadRuntimeWork() &&
        IsScheduledTaskDue(ScheduledTaskKind::DocumentState, postEventDisconnectNow)) {
        g_runtime.timing.documentStateTimerDueTime = 0;
        HandleDocumentStateTick();
    }

    if (CanRunOwnerThreadRuntimeWork()) {
        RefreshSchedulerTimerWithRecovery();
    }
}

bool ArmSaveTimer(DWORD delayMs) {
    return ScheduleTask(ScheduledTaskKind::Save, delayMs);
}

bool RescheduleSaveTimer(DWORD delayMs) {
    return ScheduleTask(ScheduledTaskKind::Save,
                        delayMs,
                        ScheduledTaskScheduleMode::Reschedule);
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

bool HandleDocumentObservationApplyOutcome(DocumentObservationApplyResult result) {
    switch (result) {
        case DocumentObservationApplyResult::RearmSteadyPoll:
            ReconcileDocumentStateSchedule();
            return true;

        case DocumentObservationApplyResult::StartedTransitionFlush:
        case DocumentObservationApplyResult::ScheduledSave:
            return true;

        case DocumentObservationApplyResult::None:
            return false;
    }

    return false;
}

bool HandleDocumentStateTickDecisionOutcome(const RuntimeTickSnapshot&,
                                            const TickDecision& decision) {
    if (decision.action == TickDecisionAction::None) {
        return true;
    }

    if (decision.action != TickDecisionAction::RearmDocumentStateTimer) {
        return false;
    }

    switch (decision.state) {
        case RuntimeStatePhase::WaitingForWordUi:
            SetCurrentDocumentRetryReason(DocumentRetryReason::UiPaused);
            break;

        case RuntimeStatePhase::WaitingForWordWindow:
            SetCurrentDocumentRetryReason(DocumentRetryReason::InactiveWindow);
            break;

        case RuntimeStatePhase::WaitingForInputToSettle:
            SetCurrentDocumentRetryReason(DocumentRetryReason::InputBusy);
            break;

        default:
            break;
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
                ApplyDirtyDocumentObservation(snapshot, tick));

        case DocumentDirtyState::Clean:
            NoteDocumentStateRefreshReady();
            return HandleDocumentObservationApplyOutcome(
                ApplyCleanDocumentObservation(snapshot, tick));

        case DocumentDirtyState::RetryLater:
            ArmDocumentStateTimer(AdvanceDocumentRetryDelay(
                GetDocumentRetryReasonForSnapshot(snapshot)));
            return true;
    }

    return false;
}

bool HandleAutosaveTickDecisionOutcome(const RuntimeTickSnapshot&,
                                       const TickDecision& decision) {
    if (decision.action == TickDecisionAction::None) {
        return true;
    }

    if (decision.action != TickDecisionAction::RearmSaveTimer) {
        return false;
    }

    switch (decision.state) {
        case RuntimeStatePhase::WaitingForWordUi:
            SetCurrentSaveRetryReason(SaveRetryReason::UiPaused);
            break;

        case RuntimeStatePhase::WaitingForWordWindow:
            SetCurrentSaveRetryReason(SaveRetryReason::InactiveWindow);
            break;

        case RuntimeStatePhase::WaitingForInputToSettle:
            SetCurrentSaveRetryReason(SaveRetryReason::InputBusy);
            break;

        default:
            break;
    }

    ArmSaveTimer(decision.delayMs);
    return true;
}

void HandleAutosaveAttemptResult(SaveAttemptResult result,
                                 const ActiveDocumentSnapshot* snapshot,
                                 const RuntimeTickSnapshot& tick) {
    switch (result) {
        case SaveAttemptResult::Saved:
            NoteSaveOperationTime(tick.now);
            if (snapshot->hasPath) {
                TryMigrateObservedDocumentIdentity(
                    snapshot,
                    false,
                    GetTransitionFlushClearModeForTick(tick));
            }
            if (!ShouldResetSaveRetryAfterSuccessfulSnapshot(
                    KeepPendingAutosaveForUnexpectedSnapshot(snapshot,
                                                             tick))) {
                return;
            }
            NoteSaveOperationReady();
            FinalizeSaveAttemptState(snapshot,
                                     true,
                                     true,
                                     GetTransitionFlushClearModeForTick(tick));
            MaybeSchedulePostTransitionRefresh(tick);
            ReconcileDocumentStateSchedule();
            if (tick.runtime.transitionFlushRequested) {
                LogSaveStatus(L"flushed pending changes for the previous document");
            } else {
                LogSaveStatus(L"pending changes were saved");
            }
            return;

        case SaveAttemptResult::AlreadyClean:
            NoteSaveOperationTime(tick.now);
            if (snapshot->hasPath &&
                TryMigrateObservedDocumentIdentity(
                    snapshot,
                    true,
                    GetTransitionFlushClearModeForTick(tick))) {
                NoteSaveOperationReady();
                MaybeSchedulePostTransitionRefresh(tick);
                ReconcileDocumentStateSchedule();
                return;
            }
            if (!ShouldResetSaveRetryAfterSuccessfulSnapshot(
                    KeepPendingAutosaveForUnexpectedSnapshot(snapshot,
                                                             tick))) {
                return;
            }
            NoteSaveOperationReady();
            FinalizeSaveAttemptState(snapshot,
                                     true,
                                     true,
                                     GetTransitionFlushClearModeForTick(tick));
            MaybeSchedulePostTransitionRefresh(tick);
            ReconcileDocumentStateSchedule();
            LogSaveStatus(tick.runtime.transitionFlushRequested
                              ? L"the previous document was already clean"
                              : L"pending changes were already saved");
            return;

        case SaveAttemptResult::Cleared:
            ResetSaveRetryState();
            if (snapshot->hasPath &&
                TryMigrateObservedDocumentIdentity(
                    snapshot,
                    true,
                    GetTransitionFlushClearModeForTick(tick))) {
                MaybeSchedulePostTransitionRefresh(tick);
                ReconcileDocumentStateSchedule();
                return;
            }
            if (snapshot->clearedReason != SnapshotClearedReason::None) {
                LogSaveStatus(DescribeSnapshotClearedReason(snapshot->clearedReason));
            }
            FinalizeSaveAttemptState(snapshot,
                                     true,
                                     true,
                                     GetTransitionFlushClearModeForTick(tick));
            MaybeSchedulePostTransitionRefresh(tick);
            ReconcileDocumentStateSchedule();
            return;

        case SaveAttemptResult::Deferred:
            ArmSaveTimer(
                AdvanceSaveRetryDelay(SaveRetryReason::DeferredProtectedView));
            return;

        case SaveAttemptResult::RetryLater:
            ArmSaveTimer(AdvanceSaveRetryDelay(
                GetSaveRetryReasonForSnapshot(snapshot)));
            return;

        case SaveAttemptResult::Failed:
            ArmSaveTimer(
                AdvanceSaveRetryDelay(SaveRetryReason::HardFailure));
            return;
    }
}

void TryCriticalTransitionSaveNow(const wchar_t* path, IDispatch* document) {
    if (!HasPendingAutosave()) {
        return;
    }

    bool copiedTarget = false;
    ScopedBstr pathCopy;
    if (path && *path) {
        copiedTarget = ReplaceStoredTextBstr(&pathCopy, path);
    }

    ScopedComPtr<IDispatch> documentRef;
    if (document) {
        copiedTarget = ReplaceStoredComPtr(&documentRef, document) || copiedTarget;
    }

    if (!copiedTarget) {
        ArmSaveTimer(
            AdvanceSaveRetryDelay(SaveRetryReason::TargetUnavailable));
        return;
    }

    ActiveDocumentSnapshot snapshot;
    RuntimeTickSnapshot tick = {};
    tick.now = GetTickCount64();
    tick.runtime.pendingSave = true;
    tick.runtime.transitionFlushRequested = true;

    HandleAutosaveAttemptResult(
        TrySaveActiveDocument(&snapshot,
                              pathCopy.Length() != 0 ? pathCopy.CStr() : nullptr,
                              documentRef.Get(),
                              ComRetryProfile::CriticalClose),
        &snapshot,
        tick);
}

bool ShouldUseObservedAutosaveTarget(bool transitionFlushRequested,
                                     bool hasObservedDocumentPath,
                                     bool hasObservedDocument) {
    return !transitionFlushRequested &&
           (hasObservedDocumentPath || hasObservedDocument);
}

bool CopyAutosaveTargetFromState(const ScopedBstr& sourcePath,
                                 IDispatch* sourceDocument,
                                 ScopedBstr* targetPath,
                                 ScopedComPtr<IDispatch>* targetDocument) {
    bool copiedTarget = false;

    if (sourcePath.Length() != 0) {
        copiedTarget = ReplaceStoredTextBstr(targetPath, sourcePath.Get());
    }

    if (sourceDocument) {
        copiedTarget = ReplaceStoredComPtr(targetDocument, sourceDocument) || copiedTarget;
    }

    return copiedTarget;
}

bool CopyAutosaveTargetForTick(const RuntimeTickSnapshot& tick,
                               ScopedBstr* targetPath,
                               ScopedComPtr<IDispatch>* targetDocument) {
    if (!targetPath || !targetDocument) {
        return false;
    }

    if (tick.runtime.transitionFlushRequested) {
        return CopyAutosaveTargetFromState(
            g_runtime.document.transitionFlushDocumentPath,
            g_runtime.document.transitionFlushDocument.Get(),
            targetPath,
            targetDocument);
    }

    if (!ShouldUseObservedAutosaveTarget(
            tick.runtime.transitionFlushRequested,
            g_runtime.document.observedDocumentPath.Length() != 0,
            static_cast<bool>(g_runtime.document.observedDocument))) {
        return true;
    }

    return CopyAutosaveTargetFromState(g_runtime.document.observedDocumentPath,
                                       g_runtime.document.observedDocument.Get(),
                                       targetPath,
                                       targetDocument);
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

    ReconcileDocumentStateSchedule();
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

    ClearCurrentSaveRetryReason();

    ActiveDocumentSnapshot snapshot;
    ScopedBstr targetPath;
    ScopedComPtr<IDispatch> targetDocument;
    if (!CopyAutosaveTargetForTick(tick, &targetPath, &targetDocument)) {
        ArmDocumentStateTimer(INPUT_SETTLE_DELAY_MS);
        ArmSaveTimer(
            AdvanceSaveRetryDelay(SaveRetryReason::TargetUnavailable));
        return;
    }

    HandleAutosaveAttemptResult(
        TrySaveActiveDocument(&snapshot,
                              targetPath.Length() != 0 ? targetPath.CStr() : nullptr,
                              targetDocument.Get()),
        &snapshot,
        tick);
}

// ============================================================================
// Input Detection
// ============================================================================

bool IsPlainTextInputVirtualKeyForState(WPARAM wParam,
                                        bool ctrlPressed,
                                        bool altPressed,
                                        bool winPressed) {
    if (ctrlPressed || altPressed || winPressed) {
        return false;
    }

    if ((wParam >= 'A' && wParam <= 'Z') ||
        (wParam >= '0' && wParam <= '9') ||
        (wParam >= VK_NUMPAD0 && wParam <= VK_NUMPAD9)) {
        return true;
    }

    switch (wParam) {
        case VK_SPACE:
        case VK_MULTIPLY:
        case VK_ADD:
        case VK_SUBTRACT:
        case VK_DECIMAL:
        case VK_DIVIDE:
        case VK_OEM_1:
        case VK_OEM_PLUS:
        case VK_OEM_COMMA:
        case VK_OEM_MINUS:
        case VK_OEM_PERIOD:
        case VK_OEM_2:
        case VK_OEM_3:
        case VK_OEM_4:
        case VK_OEM_5:
        case VK_OEM_6:
        case VK_OEM_7:
        case VK_OEM_8:
        case VK_OEM_102:
            return true;
    }

    return false;
}

enum class KeyDownAutosaveAction {
    None,
    ManualSave,
    EditingInput,
    TextInput,
};

bool IsPotentialAutosaveKeyDown(WPARAM wParam) {
    switch (wParam) {
        case VK_BACK:
        case VK_DELETE:
        case VK_RETURN:
        case VK_TAB:
            return true;
    }

    return IsPlainTextInputVirtualKeyForState(wParam, false, false, false);
}

KeyDownAutosaveAction ClassifyKeyDownAutosaveActionForState(
    WPARAM wParam,
    bool ctrlPressed,
    bool shiftPressed,
    bool altPressed,
    bool winPressed) {
    if (ctrlPressed && !shiftPressed && !altPressed && wParam == 'S') {
        return KeyDownAutosaveAction::ManualSave;
    }

    if (ctrlPressed && !altPressed) {
        if (wParam == 'B' || wParam == 'I' || wParam == 'U' ||
            wParam == 'V' || wParam == 'X' || wParam == 'Y' || wParam == 'Z' ||
            wParam == VK_RETURN) {
            return KeyDownAutosaveAction::EditingInput;
        }

        return KeyDownAutosaveAction::None;
    }

    if (altPressed) {
        return KeyDownAutosaveAction::None;
    }

    switch (wParam) {
        case VK_BACK:
        case VK_DELETE:
        case VK_RETURN:
        case VK_TAB:
            return KeyDownAutosaveAction::EditingInput;
    }

    return IsPlainTextInputVirtualKeyForState(wParam,
                                              ctrlPressed,
                                              altPressed,
                                              winPressed)
               ? KeyDownAutosaveAction::TextInput
               : KeyDownAutosaveAction::None;
}

KeyDownAutosaveAction ClassifyKeyDownAutosaveAction(WPARAM wParam) {
    if (!IsPotentialAutosaveKeyDown(wParam)) {
        return KeyDownAutosaveAction::None;
    }

    // Read each modifier at most once, and only read modifiers whose state can
    // affect this key. This avoids two full GetKeyState passes for ordinary
    // WM_KEYDOWN traffic.
    const bool ctrlPressed = IsQueueKeyDown(VK_CONTROL);
    const bool altPressed = IsQueueKeyDown(VK_MENU);
    const bool shiftPressed =
        ctrlPressed && !altPressed && wParam == 'S' &&
        IsQueueKeyDown(VK_SHIFT);

    bool winPressed = false;
    if (!ctrlPressed && !altPressed &&
        IsPlainTextInputVirtualKeyForState(wParam, false, false, false)) {
        winPressed = IsQueueKeyDown(VK_LWIN) || IsQueueKeyDown(VK_RWIN);
    }

    return ClassifyKeyDownAutosaveActionForState(wParam,
                                                 ctrlPressed,
                                                 shiftPressed,
                                                 altPressed,
                                                 winPressed);
}

bool ShouldSuppressCharacterInputAfterKeyDown(bool suppressionPending,
                                              ULONGLONG lastKeyDownTime,
                                              ULONGLONG now) {
    return suppressionPending &&
           lastKeyDownTime != 0 &&
           now >= lastKeyDownTime &&
           now - lastKeyDownTime <= TEXT_INPUT_KEYDOWN_CHAR_SUPPRESSION_MS;
}

void NoteTextInputKeyDownScheduled() {
    g_runtime.timing.lastTextInputKeyDownTime = GetTickCount64();
    SetFlag(g_runtime.flags.suppressNextCharacterInput);
}

bool ConsumeTextInputKeyDownCharacterSuppression() {
    const bool suppressionPending =
        LoadFlag(g_runtime.flags.suppressNextCharacterInput);
    if (!suppressionPending) {
        return false;
    }

    ClearFlag(g_runtime.flags.suppressNextCharacterInput);

    return ShouldSuppressCharacterInputAfterKeyDown(
        true,
        g_runtime.timing.lastTextInputKeyDownTime,
        GetTickCount64());
}

MessageAutosaveRole ClassifyMessageAutosaveRole(const MSG* lpMsg) {
    if (!lpMsg) {
        return MessageAutosaveRole::None;
    }

    if (lpMsg->message == WM_KEYDOWN) {
        switch (ClassifyKeyDownAutosaveAction(lpMsg->wParam)) {
            case KeyDownAutosaveAction::ManualSave:
                HandleManualSave();
                return MessageAutosaveRole::None;

            case KeyDownAutosaveAction::EditingInput:
                return MessageAutosaveRole::EditingInput;

            case KeyDownAutosaveAction::TextInput:
                return MessageAutosaveRole::TextKeyDownInput;

            case KeyDownAutosaveAction::None:
                break;
        }
    }

    if (lpMsg->message == WM_CHAR &&
        lpMsg->wParam >= 0x20 &&
        !ConsumeTextInputKeyDownCharacterSuppression()) {
        return MessageAutosaveRole::EditingInput;
    }

    if (IsDocumentStateRefreshMessage(lpMsg->message) &&
        ShouldUseMessageDocumentRefreshFallback()) {
        return MessageAutosaveRole::DocumentRefreshFallback;
    }

    if (!IsActionBoundaryMessage(lpMsg) || !HasPendingSaveWork()) {
        return MessageAutosaveRole::None;
    }

    const bool transitionFlush = IsTransitionFlushMessage(lpMsg);
    if (!transitionFlush &&
        !ShouldUseMessageBoundaryFallback(AreWordEventsConnected(), false)) {
        return MessageAutosaveRole::None;
    }

    return transitionFlush ? MessageAutosaveRole::TransitionBoundaryFallback
                           : MessageAutosaveRole::ActionBoundaryFallback;
}

void HandleClassifiedMessageAutosaveRole(const MSG* lpMsg) {
    const MessageAutosaveRole messageRole =
        ClassifyMessageAutosaveRole(lpMsg);
    if (messageRole == MessageAutosaveRole::None) {
        return;
    }

    if (messageRole == MessageAutosaveRole::DocumentRefreshFallback) {
        if (!ShouldApplyMessageAutosaveRole(
                messageRole,
                false,
                SaveRetryReason::None,
                false,
                g_runtime.documentRetry.reason,
                IsScheduledTaskArmed(ScheduledTaskKind::DocumentState))) {
            return;
        }
    } else if (messageRole == MessageAutosaveRole::ActionBoundaryFallback) {
        const bool pendingAutosave = HasPendingAutosave();
        const bool shouldApply =
            pendingAutosave
                ? ShouldApplyMessageAutosaveRole(
                      messageRole,
                      true,
                      g_runtime.saveRetry.reason,
                      IsScheduledTaskArmed(ScheduledTaskKind::Save),
                      DocumentRetryReason::None,
                      false)
                : ShouldApplyMessageAutosaveRole(
                      messageRole,
                      false,
                      SaveRetryReason::None,
                      false,
                      g_runtime.documentRetry.reason,
                      IsScheduledTaskArmed(ScheduledTaskKind::DocumentState));
        if (!shouldApply) {
            return;
        }
    }

    switch (messageRole) {
        case MessageAutosaveRole::TextKeyDownInput:
            NoteTextInputKeyDownScheduled();
            ScheduleSaveFromEdit();
            break;

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

UINT EnsureStartupBootstrapMessage() {
    if (g_startupBootstrapMessage != 0) {
        return g_startupBootstrapMessage;
    }

    g_startupBootstrapMessage =
        RegisterWindowMessageW(L"Windhawk.WordLocalAutoSave.StartupBootstrap");
    return g_startupBootstrapMessage;
}

bool PostStartupBootstrapMessageToWordUi() {
    UINT bootstrapMessage = EnsureStartupBootstrapMessage();
    if (bootstrapMessage == 0) {
        return false;
    }

    HWND rootWindow = FindCurrentProcessWordRootWindow();
    if (!rootWindow) {
        return false;
    }

    DWORD uiThreadId = GetWindowThreadProcessId(rootWindow, nullptr);
    if (uiThreadId == 0 || LoadOwnerThreadId() == uiThreadId) {
        return true;
    }

    return PostMessageW(rootWindow, bootstrapMessage, 0, 0) != FALSE;
}

// ============================================================================
// Hook
// ============================================================================

void HandleTranslateMessageAutosave(const MSG* lpMsg) {
    if (!lpMsg) {
        return;
    }

    const bool relevantMessage =
        IsAutosaveRelevantMessage(lpMsg->message) ||
        (g_startupBootstrapMessage != 0 &&
         lpMsg->message == g_startupBootstrapMessage);
    const bool controlRequestPending =
        g_control.pendingRequests.load(std::memory_order_acquire) != 0;
    if (!relevantMessage && !controlRequestPending) {
        return;
    }
    if (!controlRequestPending &&
        !LoadFlag(g_runtime.flags.moduleActive)) {
        return;
    }

    const DWORD ownerThreadId = LoadOwnerThreadId();
    const DWORD currentThreadId = GetCurrentThreadId();
    const HookMessageRoute route =
        SelectHookMessageRoute(relevantMessage,
                               controlRequestPending,
                               ownerThreadId,
                               currentThreadId);
    if (route == HookMessageRoute::Ignore) {
        return;
    }

    if (route == HookMessageRoute::AdoptOwner) {
        AdoptOwnerThreadIfNeeded(lpMsg);
    }
    if (!IsOwnerThreadSchedulerContextValid()) {
        return;
    }

    if (controlRequestPending) {
        ProcessOwnerControlRequests();
        if (!CanRunOwnerThreadRuntimeWork()) {
            return;
        }
    }

    if (!relevantMessage) {
        return;
    }

    ScopedOwnerRuntimeWork ownerWork;
    if (ShouldInvalidateWordUiCacheForMessage(lpMsg)) {
        InvalidateWordUiWindowCache();
    }

    UpdateImeCompositionState(lpMsg);
    ResumePendingRuntimeWorkForSignal(ClassifySaveResumeSignal(lpMsg));
    HandleClassifiedMessageAutosaveRole(lpMsg);
}

bool HandlePrivateRuntimeMessage(const MSG* lpMsg) {
    if (!lpMsg) {
        return false;
    }

    const bool isControlMessage =
        lpMsg->message == WORD_LOCAL_AUTOSAVE_CONTROL_MESSAGE;
    const bool isSchedulerMessage =
        lpMsg->message == WM_TIMER &&
        lpMsg->wParam == SCHEDULER_WINDOW_TIMER_ID;
    if (!isControlMessage && !isSchedulerMessage) {
        return false;
    }

    const HWND messageWindow =
        g_control.messageWindow.load(std::memory_order_acquire);
    if (!messageWindow || lpMsg->hwnd != messageWindow) {
        return false;
    }

    if (isControlMessage) {
        if (IsOwnerThread()) {
            ProcessOwnerControlRequests();
        }
        return true;
    }

    if (IsOwnerThread()) {
        HandleSchedulerTimerMessage(
            static_cast<UINT_PTR>(lpMsg->wParam));
    }
    return true;
}

bool HasPotentialHookRuntimeWork(const MSG* lpMsg) {
    if (lpMsg) {
        const bool privateMessageType =
            lpMsg->message == WORD_LOCAL_AUTOSAVE_CONTROL_MESSAGE ||
            (lpMsg->message == WM_TIMER &&
             lpMsg->wParam == SCHEDULER_WINDOW_TIMER_ID);
        const bool relevantMessage =
            IsAutosaveRelevantMessage(lpMsg->message) ||
            (g_startupBootstrapMessage != 0 &&
             lpMsg->message == g_startupBootstrapMessage);
        if (privateMessageType || relevantMessage) {
            return true;
        }
    }

    return g_control.pendingRequests.load(std::memory_order_acquire) != 0;
}

BOOL WINAPI TranslateMessage_Hook(const MSG* lpMsg) {
    if (!g_originalTranslateMessage) {
        return TRUE;
    }

    // The overwhelming majority of queue traffic isn't relevant to the mod.
    // Atomic reads and the message classifier don't touch LastError, so pass
    // it straight to the original API without any bookkeeping.
    if (!HasPotentialHookRuntimeWork(lpMsg)) {
        return g_originalTranslateMessage(lpMsg);
    }

    const DWORD entryLastError = GetLastError();

    if (!HandlePrivateRuntimeMessage(lpMsg)) {
        HandleTranslateMessageAutosave(lpMsg);
    }

    // Present the original API with the same thread error state it would have
    // seen without the hook. Returning it directly also preserves the error
    // state it leaves behind.
    SetLastError(entryLastError);
    return g_originalTranslateMessage(lpMsg);
}

// ============================================================================
// Windhawk Callbacks
// ============================================================================

#ifdef WH_STANDALONE_COMPILE_CHECK
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

class SelfTestDispatch final : public IDispatch {
public:
    STDMETHODIMP QueryInterface(REFIID riid, void** object) override {
        if (!object) {
            return E_POINTER;
        }

        *object = nullptr;
        if (riid == IID_IUnknown || riid == kIIDIDispatch) {
            *object = static_cast<IDispatch*>(this);
            AddRef();
            return S_OK;
        }

        return E_NOINTERFACE;
    }

    STDMETHODIMP_(ULONG) AddRef() override {
        return 2;
    }

    STDMETHODIMP_(ULONG) Release() override {
        return 1;
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

    STDMETHODIMP Invoke(DISPID,
                        REFIID,
                        LCID,
                        WORD,
                        DISPPARAMS*,
                        VARIANT*,
                        EXCEPINFO*,
                        UINT*) override {
        return E_NOTIMPL;
    }
};

using SelfTestCallback = void (*)(void* context);

class CountingDispatch final : public IDispatch {
public:
    CountingDispatch(const wchar_t* memberName, DISPID dispatchId)
        : m_memberName(memberName), m_dispatchId(dispatchId) {
    }

    void RebindAfterNextMemberNotFound(DISPID dispatchId) {
        m_rebindDispatchId = dispatchId;
        m_rebindPending = true;
    }

    void FailNextInvoke(HRESULT hr) {
        m_nextInvokeResult = hr;
        m_hasNextInvokeResult = true;
    }

    void FailIdentityQueries(bool fail = true) {
        m_failIdentityQueries = fail;
    }

    void PopulateResultBeforeNextMemberNotFound() {
        m_populateResultBeforeMemberNotFound = true;
    }

    void ReturnVariantBool(VARIANT_BOOL value) {
        m_resultType = VT_BOOL;
        m_resultBool = value;
    }

    void ReturnVariantLong(long value) {
        m_resultType = VT_I4;
        m_resultLong = value;
    }

    void ReturnVariantInt64(LONGLONG value) {
        m_resultType = VT_I8;
        m_resultInt64 = value;
    }

    void ReturnVariantInt16(short value) {
        m_resultType = VT_I2;
        m_resultInt16 = value;
    }

    void ReturnVariantBstr(const wchar_t* value) {
        m_resultType = VT_BSTR;
        m_resultText = value;
    }

    BSTR LastProducedBstr() const {
        return m_lastProducedBstr;
    }

    bool WasRetryResultEmpty() const {
        return m_retryResultWasEmpty;
    }

    void ObserveStorageDuringRelease(ScopedComPtr<IDispatch>* storage,
                                     bool* observedEmpty) {
        m_releaseObservedStorage = storage;
        m_releaseObservedEmpty = observedEmpty;
    }

    void RunOnceOnNextAddRef(SelfTestCallback callback, void* context) {
        m_addRefCallback = callback;
        m_addRefCallbackContext = context;
    }

    void RunOnceOnNextRelease(SelfTestCallback callback, void* context) {
        m_releaseCallback = callback;
        m_releaseCallbackContext = context;
    }

    int GetIdsCallCount() const {
        return m_getIdsCallCount;
    }

    int GetTypeInfoCallCount() const {
        return m_getTypeInfoCallCount;
    }

    int InvokeCallCount() const {
        return m_invokeCallCount;
    }

    DISPID LastInvokedDispId() const {
        return m_lastInvokedDispId;
    }

    STDMETHODIMP QueryInterface(REFIID riid, void** object) override {
        if (!object) {
            return E_POINTER;
        }

        *object = nullptr;
        if (m_failIdentityQueries && riid == IID_IUnknown) {
            return E_NOINTERFACE;
        }

        if (riid == IID_IUnknown || riid == kIIDIDispatch) {
            *object = static_cast<IDispatch*>(this);
            AddRef();
            return S_OK;
        }

        return E_NOINTERFACE;
    }

    STDMETHODIMP_(ULONG) AddRef() override {
        SelfTestCallback callback = m_addRefCallback;
        void* context = m_addRefCallbackContext;
        m_addRefCallback = nullptr;
        m_addRefCallbackContext = nullptr;
        if (callback) {
            callback(context);
        }
        return 2;
    }

    STDMETHODIMP_(ULONG) Release() override {
        if (m_releaseObservedStorage && m_releaseObservedEmpty) {
            *m_releaseObservedEmpty =
                m_releaseObservedStorage->Get() == nullptr;
        }
        SelfTestCallback callback = m_releaseCallback;
        void* context = m_releaseCallbackContext;
        m_releaseCallback = nullptr;
        m_releaseCallbackContext = nullptr;
        if (callback) {
            callback(context);
        }
        return 1;
    }

    STDMETHODIMP GetTypeInfoCount(UINT* pctinfo) override {
        if (pctinfo) {
            *pctinfo = 0;
        }
        return S_OK;
    }

    STDMETHODIMP GetTypeInfo(UINT, LCID, ITypeInfo**) override {
        ++m_getTypeInfoCallCount;
        return E_NOTIMPL;
    }

    STDMETHODIMP GetIDsOfNames(REFIID,
                                LPOLESTR* names,
                                UINT nameCount,
                                LCID,
                                DISPID* dispatchIds) override {
        ++m_getIdsCallCount;
        if (!names || nameCount != 1 || !names[0] || !dispatchIds ||
            !m_memberName || lstrcmpW(names[0], m_memberName) != 0) {
            return DISP_E_UNKNOWNNAME;
        }

        dispatchIds[0] = m_dispatchId;
        return S_OK;
    }

    STDMETHODIMP Invoke(DISPID dispatchId,
                        REFIID,
                        LCID,
                        WORD,
                        DISPPARAMS*,
                        VARIANT* result,
                        EXCEPINFO*,
                        UINT*) override {
        ++m_invokeCallCount;
        m_lastInvokedDispId = dispatchId;
        if (m_expectEmptyResultOnNextInvoke) {
            m_retryResultWasEmpty = !result || result->vt == VT_EMPTY;
            m_expectEmptyResultOnNextInvoke = false;
            if (!m_retryResultWasEmpty) {
                return E_UNEXPECTED;
            }
        }

        if (m_rebindPending) {
            m_rebindPending = false;
            m_dispatchId = m_rebindDispatchId;
            if (m_populateResultBeforeMemberNotFound && result) {
                VariantInit(result);
                result->vt = VT_BSTR;
                result->bstrVal = SysAllocString(L"stale retry result");
                if (!result->bstrVal) {
                    return E_OUTOFMEMORY;
                }
                m_expectEmptyResultOnNextInvoke = true;
                m_populateResultBeforeMemberNotFound = false;
            }
            return DISP_E_MEMBERNOTFOUND;
        }

        if (m_hasNextInvokeResult) {
            m_hasNextInvokeResult = false;
            return m_nextInvokeResult;
        }

        if (dispatchId != m_dispatchId) {
            return DISP_E_MEMBERNOTFOUND;
        }

        if (result) {
            VariantInit(result);
            result->vt = m_resultType;
            switch (m_resultType) {
                case VT_BOOL:
                    result->boolVal = m_resultBool;
                    break;

                case VT_I2:
                    result->iVal = m_resultInt16;
                    break;

                case VT_I4:
                    result->lVal = m_resultLong;
                    break;

                case VT_I8:
                    result->llVal = m_resultInt64;
                    break;

                case VT_BSTR:
                    result->bstrVal = m_resultText
                                          ? SysAllocString(m_resultText)
                                          : nullptr;
                    if (m_resultText && !result->bstrVal) {
                        result->vt = VT_EMPTY;
                        return E_OUTOFMEMORY;
                    }
                    m_lastProducedBstr = result->bstrVal;
                    break;

                default:
                    return E_UNEXPECTED;
            }
        }
        return S_OK;
    }

private:
    const wchar_t* m_memberName = nullptr;
    DISPID m_dispatchId = DISPID_UNKNOWN;
    DISPID m_rebindDispatchId = DISPID_UNKNOWN;
    DISPID m_lastInvokedDispId = DISPID_UNKNOWN;
    HRESULT m_nextInvokeResult = S_OK;
    VARTYPE m_resultType = VT_BOOL;
    VARIANT_BOOL m_resultBool = VARIANT_TRUE;
    short m_resultInt16 = 0;
    long m_resultLong = 0;
    LONGLONG m_resultInt64 = 0;
    const wchar_t* m_resultText = nullptr;
    BSTR m_lastProducedBstr = nullptr;
    int m_getIdsCallCount = 0;
    int m_getTypeInfoCallCount = 0;
    int m_invokeCallCount = 0;
    bool m_rebindPending = false;
    bool m_hasNextInvokeResult = false;
    bool m_failIdentityQueries = false;
    bool m_populateResultBeforeMemberNotFound = false;
    bool m_expectEmptyResultOnNextInvoke = false;
    bool m_retryResultWasEmpty = true;
    ScopedComPtr<IDispatch>* m_releaseObservedStorage = nullptr;
    bool* m_releaseObservedEmpty = nullptr;
    SelfTestCallback m_addRefCallback = nullptr;
    void* m_addRefCallbackContext = nullptr;
    SelfTestCallback m_releaseCallback = nullptr;
    void* m_releaseCallbackContext = nullptr;
};

class SelfTestConnectionPoint final : public IConnectionPoint {
public:
    void SetUnadviseResult(HRESULT result) {
        m_unadviseResult = result;
    }

    void RunOnceOnNextUnadvise(SelfTestCallback callback, void* context) {
        m_unadviseCallback = callback;
        m_unadviseCallbackContext = context;
    }

    int UnadviseCallCount() const {
        return m_unadviseCallCount;
    }

    STDMETHODIMP QueryInterface(REFIID riid, void** object) override {
        if (!object) {
            return E_POINTER;
        }

        *object = nullptr;
        if (riid == IID_IUnknown || riid == IID_IConnectionPoint) {
            *object = static_cast<IConnectionPoint*>(this);
            AddRef();
            return S_OK;
        }
        return E_NOINTERFACE;
    }

    STDMETHODIMP_(ULONG) AddRef() override {
        return 2;
    }

    STDMETHODIMP_(ULONG) Release() override {
        return 1;
    }

    STDMETHODIMP GetConnectionInterface(IID* iid) override {
        if (!iid) {
            return E_POINTER;
        }
        *iid = kDIIDWordApplicationEvents4;
        return S_OK;
    }

    STDMETHODIMP GetConnectionPointContainer(
        IConnectionPointContainer** container) override {
        if (container) {
            *container = nullptr;
        }
        return E_NOTIMPL;
    }

    STDMETHODIMP Advise(IUnknown*, DWORD*) override {
        return E_NOTIMPL;
    }

    STDMETHODIMP Unadvise(DWORD) override {
        ++m_unadviseCallCount;
        SelfTestCallback callback = m_unadviseCallback;
        void* context = m_unadviseCallbackContext;
        m_unadviseCallback = nullptr;
        m_unadviseCallbackContext = nullptr;
        if (callback) {
            callback(context);
        }
        return m_unadviseResult;
    }

    STDMETHODIMP EnumConnections(IEnumConnections** connections) override {
        if (connections) {
            *connections = nullptr;
        }
        return E_NOTIMPL;
    }

private:
    HRESULT m_unadviseResult = S_OK;
    int m_unadviseCallCount = 0;
    SelfTestCallback m_unadviseCallback = nullptr;
    void* m_unadviseCallbackContext = nullptr;
};

struct SessionResetReentryTestContext {
    WordEventSession* session = nullptr;
    CountingDispatch* replacementApplication = nullptr;
    bool observedFullyDetached = false;
};

void ReenterWordEventSessionReset(void* rawContext) {
    SessionResetReentryTestContext* context =
        static_cast<SessionResetReentryTestContext*>(rawContext);
    if (!context || !context->session || !context->replacementApplication) {
        return;
    }

    context->observedFullyDetached = context->session->IsEmpty();
    context->replacementApplication->AddRef();
    context->session->application.Reset(context->replacementApplication);
    context->session->applicationHwnd = 77;
}

struct EventDisconnectReentryTestContext {
    bool observedDisconnected = false;
    bool observedGlobalSessionEmpty = false;
    bool observedTeardownGuard = false;
};

void ReenterPrimaryWordEventDisconnect(void* rawContext) {
    EventDisconnectReentryTestContext* context =
        static_cast<EventDisconnectReentryTestContext*>(rawContext);
    if (context) {
        context->observedDisconnected =
            !LoadFlag(g_runtime.flags.wordEventsConnected);
        context->observedGlobalSessionEmpty =
            g_runtime.events.session.IsEmpty();
        context->observedTeardownGuard =
            IsWordEventTeardownInProgress();
    }
    DisconnectWordApplicationEvents(false);
}

void ReenterPendingWordEventDisconnectRetry(void*) {
    RetryPendingWordEventDisconnects();
}

void ReenterWordEventShutdown(void* rawContext) {
    bool* completed = static_cast<bool*>(rawContext);
    const bool result = TryDisconnectAllWordEventsForOwnerShutdown();
    if (completed) {
        *completed = result;
    }
}

struct TransitionReentryTestContext {
    const wchar_t* path = nullptr;
    IDispatch* document = nullptr;
};

void PublishNestedTransitionFlushTarget(void* rawContext) {
    TransitionReentryTestContext* context =
        static_cast<TransitionReentryTestContext*>(rawContext);
    if (context) {
        RequestTransitionFlush(context->path, nullptr, context->document);
    }
}

struct ObservedDocumentReentryTestContext {
    const ActiveDocumentSnapshot* snapshot = nullptr;
    bool dirty = false;
};

void PublishNestedObservedDocument(void* rawContext) {
    ObservedDocumentReentryTestContext* context =
        static_cast<ObservedDocumentReentryTestContext*>(rawContext);
    if (context) {
        SetObservedDocumentFromSnapshot(context->snapshot, context->dirty);
    }
}

struct ActiveDocumentBindingReentryTestContext {
    IDispatch* document = nullptr;
    HRESULT result = E_UNEXPECTED;
};

void PublishNestedActiveDocumentBinding(void* rawContext) {
    ActiveDocumentBindingReentryTestContext* context =
        static_cast<ActiveDocumentBindingReentryTestContext*>(rawContext);
    if (context) {
        context->result = CacheActiveDocumentBinding(context->document);
    }
}

const DWORD SELF_TEST_TRANSLATE_LAST_ERROR = 0x13572468u;
DWORD g_selfTestTranslateEntryLastError = 0;

BOOL WINAPI SelfTestOriginalTranslateMessage(const MSG*) {
    g_selfTestTranslateEntryLastError = GetLastError();
    SetLastError(SELF_TEST_TRANSLATE_LAST_ERROR);
    return FALSE;
}

bool RunDispatchAndAutomationCacheSelfTests() {
    bool success = true;
    ClearWordMemberIdCache();

    ScopedVariant detachedBstrVariant;
    BSTR detachSource = SysAllocString(L"detached BSTR");
    if (!detachSource) {
        ReportSelfTestFailure(&success, L"VARIANT BSTR detach allocation");
    } else {
        detachedBstrVariant.Get()->vt = VT_BSTR;
        detachedBstrVariant.Get()->bstrVal = detachSource;
        BSTR detachedBstr = detachedBstrVariant.DetachBstr();
        if (detachedBstr != detachSource ||
            detachedBstrVariant.Get()->vt != VT_EMPTY) {
            ReportSelfTestFailure(&success, L"VARIANT BSTR ownership detach");
        }
        SysFreeString(detachedBstr);
    }

    SelfTestDispatch detachDispatch;
    ScopedVariant detachedDispatchVariant;
    detachDispatch.AddRef();
    detachedDispatchVariant.Get()->vt = VT_DISPATCH;
    detachedDispatchVariant.Get()->pdispVal = &detachDispatch;
    IDispatch* detachedDispatch = nullptr;
    if (FAILED(ExtractDispatchFromVariant(&detachedDispatchVariant,
                                          &detachedDispatch)) ||
        detachedDispatch != &detachDispatch ||
        detachedDispatchVariant.Get()->vt != VT_EMPTY) {
        ReportSelfTestFailure(&success, L"VARIANT dispatch ownership detach");
    }
    if (detachedDispatch) {
        detachedDispatch->Release();
    }

    CountingDispatch directBoolDispatch(L"Saved", 91);
    directBoolDispatch.ReturnVariantBool(VARIANT_TRUE);
    bool directBool = false;
    if (FAILED(GetBoolProperty(&directBoolDispatch,
                               WordMember::DocumentSaved,
                               &directBool)) ||
        !directBool) {
        ReportSelfTestFailure(&success, L"direct VT_BOOL property path");
    }

    CountingDispatch directIntDispatch(L"Count", 92);
    directIntDispatch.ReturnVariantLong(4242);
    long directInt = 0;
    if (FAILED(GetIntProperty(&directIntDispatch,
                              WordMember::DocumentsCount,
                              &directInt)) ||
        directInt != 4242) {
        ReportSelfTestFailure(&success, L"direct VT_I4 property path");
    }

    CountingDispatch fallbackIntDispatch(L"Count", 93);
    fallbackIntDispatch.ReturnVariantInt16(123);
    long fallbackInt = 0;
    if (FAILED(GetIntProperty(&fallbackIntDispatch,
                              WordMember::WindowsCount,
                              &fallbackInt)) ||
        fallbackInt != 123) {
        ReportSelfTestFailure(&success, L"fallback integer property conversion");
    }

    CountingDispatch directBstrDispatch(L"Name", 94);
    directBstrDispatch.ReturnVariantBstr(L"direct BSTR");
    ScopedBstr directBstr;
    if (FAILED(GetBstrProperty(&directBstrDispatch,
                               WordMember::DocumentName,
                               directBstr.Put())) ||
        directBstr.Get() != directBstrDispatch.LastProducedBstr() ||
        lstrcmpW(directBstr.CStr(), L"direct BSTR") != 0) {
        ReportSelfTestFailure(&success, L"direct VT_BSTR ownership transfer");
    }

    CountingDispatch emptyBstrDispatch(L"Path", 95);
    emptyBstrDispatch.ReturnVariantBstr(nullptr);
    BSTR emptyBstr = reinterpret_cast<BSTR>(1);
    const HRESULT emptyBstrHr =
        GetBstrProperty(&emptyBstrDispatch,
                        WordMember::DocumentPath,
                        &emptyBstr);
    if (emptyBstrHr != S_OK || emptyBstr != nullptr) {
        ReportSelfTestFailure(&success, L"empty VT_BSTR property path");
    }

    CountingDispatch directHwndDispatch(L"Hwnd", 96);
    directHwndDispatch.ReturnVariantInt64(0x1234);
    LONG_PTR directHwnd = 0;
    if (FAILED(GetDispatchWindowHandle(&directHwndDispatch,
                                       WordMember::ApplicationHwnd,
                                       &directHwnd)) ||
        directHwnd != 0x1234) {
        ReportSelfTestFailure(&success, L"direct VT_I8 window handle path");
    }

    ClearWordMemberIdCache();

    CountingDispatch savedDispatch(L"Saved", 101);
    if (FAILED(InvokeWordMember(&savedDispatch,
                                DISPATCH_PROPERTYGET,
                                WordMember::DocumentSaved)) ||
        FAILED(InvokeWordMember(&savedDispatch,
                                DISPATCH_PROPERTYGET,
                                WordMember::DocumentSaved)) ||
        savedDispatch.GetIdsCallCount() != 1 ||
        savedDispatch.InvokeCallCount() != 2 ||
        savedDispatch.GetTypeInfoCallCount() != 0) {
        ReportSelfTestFailure(&success, L"typed DISPID steady-state cache");
    }

    CountingDispatch applicationHwndDispatch(L"Hwnd", 201);
    CountingDispatch windowHwndDispatch(L"Hwnd", 202);
    if (FAILED(InvokeWordMember(&applicationHwndDispatch,
                                DISPATCH_PROPERTYGET,
                                WordMember::ApplicationHwnd)) ||
        FAILED(InvokeWordMember(&windowHwndDispatch,
                                DISPATCH_PROPERTYGET,
                                WordMember::WindowHwnd)) ||
        applicationHwndDispatch.LastInvokedDispId() != 201 ||
        windowHwndDispatch.LastInvokedDispId() != 202 ||
        applicationHwndDispatch.GetIdsCallCount() != 1 ||
        windowHwndDispatch.GetIdsCallCount() != 1) {
        ReportSelfTestFailure(&success, L"typed DISPID object-kind separation");
    }

    CountingDispatch rebindingDispatch(L"ReadOnly", 301);
    rebindingDispatch.RebindAfterNextMemberNotFound(302);
    if (FAILED(InvokeWordMember(&rebindingDispatch,
                                DISPATCH_PROPERTYGET,
                                WordMember::DocumentReadOnly)) ||
        rebindingDispatch.GetIdsCallCount() != 2 ||
        rebindingDispatch.InvokeCallCount() != 2 ||
        rebindingDispatch.LastInvokedDispId() != 302) {
        ReportSelfTestFailure(&success, L"typed DISPID one-shot rebind");
    }

    CountingDispatch dirtyResultDispatch(L"Name", 311);
    dirtyResultDispatch.RebindAfterNextMemberNotFound(312);
    dirtyResultDispatch.PopulateResultBeforeNextMemberNotFound();
    ScopedVariant reboundResult;
    if (FAILED(InvokeWordMember(&dirtyResultDispatch,
                                DISPATCH_PROPERTYGET,
                                WordMember::DocumentName,
                                reboundResult.Get())) ||
        !dirtyResultDispatch.WasRetryResultEmpty() ||
        reboundResult.Get()->vt != VT_BOOL) {
        ReportSelfTestFailure(&success,
                              L"typed DISPID dirty-result retry cleanup");
    }

    CountingDispatch releaseDispatch(L"Saved", 321);
    ScopedComPtr<IDispatch> releaseObservedStorage;
    bool releaseObservedEmpty = false;
    releaseDispatch.ObserveStorageDuringRelease(&releaseObservedStorage,
                                                &releaseObservedEmpty);
    releaseObservedStorage.Reset(&releaseDispatch);
    releaseObservedStorage.Reset();
    if (!releaseObservedEmpty) {
        ReportSelfTestFailure(&success,
                              L"COM pointer re-entrant release publication");
    }

    CountingDispatch busyDispatch(L"Path", 401);
    busyDispatch.FailNextInvoke(RPC_E_CALL_REJECTED);
    if (InvokeWordMember(&busyDispatch,
                         DISPATCH_PROPERTYGET,
                         WordMember::DocumentPath) != RPC_E_CALL_REJECTED ||
        FAILED(InvokeWordMember(&busyDispatch,
                                DISPATCH_PROPERTYGET,
                                WordMember::DocumentPath)) ||
        busyDispatch.GetIdsCallCount() != 1 ||
        busyDispatch.InvokeCallCount() != 2) {
        ReportSelfTestFailure(&success, L"busy COM failure cache preservation");
    }

    if (IsTerminalAutomationObjectFailure(RPC_E_CALL_REJECTED) ||
        IsTerminalAutomationObjectFailure(RPC_E_SERVERCALL_RETRYLATER) ||
        !IsTerminalAutomationObjectFailure(kCoEObjectNotConnected) ||
        !IsTerminalAutomationObjectFailure(RPC_E_DISCONNECTED) ||
        !IsTerminalAutomationObjectFailure(RPC_E_SERVER_DIED)) {
        ReportSelfTestFailure(&success, L"COM failure invalidation classes");
    }

    ResetAutomationCacheState();
    SelfTestDispatch firstDocument;
    SelfTestDispatch secondDocument;
    if (FAILED(CacheActiveDocumentBinding(&firstDocument))) {
        ReportSelfTestFailure(&success, L"active document cache binding");
    } else {
        const unsigned int firstGeneration =
            g_runtime.automation.documentGeneration;
        g_runtime.automation.activeDocumentPath.Reset(
            SysAllocString(L"C:\\dummy.docx"));
        g_runtime.automation.activeDocumentMetadataValid =
            g_runtime.automation.activeDocumentPath.Get() != nullptr;
        g_runtime.automation.activeDocumentHasPath = true;

        if (FAILED(CacheActiveDocumentBinding(&firstDocument)) ||
            g_runtime.automation.documentGeneration != firstGeneration ||
            !g_runtime.automation.activeDocumentMetadataValid) {
            ReportSelfTestFailure(&success,
                                  L"same active document metadata preservation");
        }

        CountingDispatch invalidIdentityDocument(L"Saved", 501);
        invalidIdentityDocument.FailIdentityQueries();
        if (SUCCEEDED(CacheActiveDocumentBinding(&invalidIdentityDocument)) ||
            !g_runtime.automation.activeDocumentStale) {
            ReportSelfTestFailure(&success,
                                  L"failed active document binding invalidation");
        }

        if (FAILED(CacheActiveDocumentBinding(&firstDocument))) {
            ReportSelfTestFailure(&success,
                                  L"active document cache recovery after failed binding");
        }

        InvalidateActiveDocumentCacheForEventCoverageGap();
        if (!g_runtime.automation.activeDocumentStale ||
            g_runtime.automation.activeDocumentMetadataValid ||
            g_runtime.automation.activeDocumentPath.Get() != nullptr ||
            g_runtime.automation.activeDocumentHasPath) {
            ReportSelfTestFailure(&success,
                                  L"event coverage gap cache invalidation");
        }

        if (FAILED(CacheActiveDocumentBinding(&firstDocument))) {
            ReportSelfTestFailure(&success,
                                  L"active document cache recovery after event gap");
        }

        if (FAILED(CacheActiveDocumentBinding(&secondDocument)) ||
            g_runtime.automation.documentGeneration == firstGeneration ||
            g_runtime.automation.activeDocumentMetadataValid) {
            ReportSelfTestFailure(&success,
                                  L"active document identity invalidation");
        }

        MarkCachedActiveDocumentStale();
        ScopedComPtr<IDispatch> copiedDocument;
        if (SUCCEEDED(CopyCachedActiveDocument(copiedDocument.Put())) ||
            copiedDocument) {
            ReportSelfTestFailure(&success, L"stale active document rejection");
        }
    }

    ResetAutomationCacheState();

    CountingDispatch supersededDocument(L"Saved", 601);
    CountingDispatch nestedDocument(L"Saved", 602);
    ActiveDocumentBindingReentryTestContext bindingContext = {};
    bindingContext.document = &nestedDocument;
    supersededDocument.RunOnceOnNextAddRef(
        PublishNestedActiveDocumentBinding,
        &bindingContext);
    const HRESULT supersededBindingHr =
        CacheActiveDocumentBinding(&supersededDocument);
    if (bindingContext.result != S_OK || supersededBindingHr != S_FALSE ||
        g_runtime.automation.activeDocument.Get() != &nestedDocument ||
        g_runtime.automation.activeDocumentStale) {
        ReportSelfTestFailure(
            &success,
            L"re-entrant active document binding supersession");
    }

    ResetAutomationCacheState();
    CountingDispatch copiedDocumentBeforeReentry(L"Saved", 603);
    CountingDispatch copiedDocumentReplacement(L"Saved", 604);
    if (FAILED(CacheActiveDocumentBinding(&copiedDocumentBeforeReentry))) {
        ReportSelfTestFailure(&success,
                              L"active document copy re-entry setup");
    } else {
        ActiveDocumentBindingReentryTestContext copyContext = {};
        copyContext.document = &copiedDocumentReplacement;
        copiedDocumentBeforeReentry.RunOnceOnNextAddRef(
            PublishNestedActiveDocumentBinding,
            &copyContext);
        ScopedComPtr<IDispatch> copiedAfterReentry;
        const HRESULT copyHr =
            CopyCachedActiveDocument(copiedAfterReentry.Put());
        if (copyContext.result != S_OK || SUCCEEDED(copyHr) ||
            copiedAfterReentry ||
            g_runtime.automation.activeDocument.Get() !=
                &copiedDocumentReplacement ||
            g_runtime.automation.activeDocumentStale) {
            ReportSelfTestFailure(
                &success,
                L"re-entrant active document copy generation check");
        }
    }

    ResetAutomationCacheState();
    ClearTransitionFlushRequest();
    CountingDispatch supersededTransitionDocument(L"Saved", 605);
    CountingDispatch nestedTransitionDocument(L"Saved", 606);
    TransitionReentryTestContext transitionContext = {};
    transitionContext.path = L"C:\\nested-transition.docx";
    transitionContext.document = &nestedTransitionDocument;
    supersededTransitionDocument.RunOnceOnNextAddRef(
        PublishNestedTransitionFlushTarget,
        &transitionContext);
    RequestTransitionFlush(L"C:\\superseded-transition.docx",
                           nullptr,
                           &supersededTransitionDocument);
    if (!LoadFlag(g_runtime.flags.transitionFlushPending) ||
        g_runtime.document.transitionFlushDocument.Get() !=
            &nestedTransitionDocument ||
        lstrcmpW(g_runtime.document.transitionFlushDocumentPath.Get(),
                 transitionContext.path) != 0) {
        ReportSelfTestFailure(&success,
                              L"re-entrant transition target supersession");
    }

    ClearTransitionFlushRequest();
    RequestTransitionFlush(L"C:\\copied-transition.docx",
                           nullptr,
                           &supersededTransitionDocument);
    supersededTransitionDocument.RunOnceOnNextAddRef(
        PublishNestedTransitionFlushTarget,
        &transitionContext);
    ScopedComPtr<IDispatch> copiedTransitionDocument;
    if (CopyValidTransitionFlushDocument(nullptr,
                                         &copiedTransitionDocument) ||
        copiedTransitionDocument ||
        g_runtime.document.transitionFlushDocument.Get() !=
            &nestedTransitionDocument ||
        lstrcmpW(g_runtime.document.transitionFlushDocumentPath.Get(),
                 transitionContext.path) != 0) {
        ReportSelfTestFailure(
            &success,
            L"re-entrant transition target copy generation check");
    }
    ClearTransitionFlushRequest();

    CountingDispatch supersededObservedDocument(L"Saved", 607);
    CountingDispatch nestedObservedDocument(L"Saved", 608);
    ActiveDocumentSnapshot supersededObservedSnapshot;
    ActiveDocumentSnapshot nestedObservedSnapshot;
    const bool observedSetupReady =
        ReplaceStoredBstr(&supersededObservedSnapshot.path,
                          L"C:\\superseded-observed.docx") &&
        ReplaceStoredComPtr(&supersededObservedSnapshot.document,
                            static_cast<IDispatch*>(
                                &supersededObservedDocument)) &&
        SUCCEEDED(GetComIdentity(&supersededObservedDocument,
                                 supersededObservedSnapshot.identity.Put())) &&
        ReplaceStoredBstr(&nestedObservedSnapshot.path,
                          L"C:\\nested-observed.docx") &&
        ReplaceStoredComPtr(&nestedObservedSnapshot.document,
                            static_cast<IDispatch*>(
                                &nestedObservedDocument)) &&
        SUCCEEDED(GetComIdentity(&nestedObservedDocument,
                                 nestedObservedSnapshot.identity.Put()));
    supersededObservedSnapshot.hasPath = observedSetupReady;
    nestedObservedSnapshot.hasPath = observedSetupReady;
    if (!observedSetupReady) {
        ReportSelfTestFailure(&success,
                              L"observed document re-entry setup");
    } else {
        ObservedDocumentReentryTestContext observedContext = {};
        observedContext.snapshot = &nestedObservedSnapshot;
        observedContext.dirty = true;
        supersededObservedDocument.RunOnceOnNextAddRef(
            PublishNestedObservedDocument,
            &observedContext);
        if (!SetObservedDocumentFromSnapshot(&supersededObservedSnapshot,
                                             true) ||
            !LoadFlag(g_runtime.flags.documentDirtyKnown) ||
            !LoadFlag(g_runtime.flags.documentDirty) ||
            g_runtime.document.observedDocument.Get() !=
                &nestedObservedDocument ||
            lstrcmpW(g_runtime.document.observedDocumentPath.Get(),
                     nestedObservedSnapshot.path.CStr()) != 0) {
            ReportSelfTestFailure(
                &success,
                L"re-entrant observed document supersession");
        }
    }
    ResetObservedDocumentState();

    ClearWordMemberIdCache();
    return success;
}

bool RunHotPathAndMonitoringPolicySelfTests() {
    bool success = true;

    if (!IsPotentialAutosaveKeyDown('A') ||
        !IsPotentialAutosaveKeyDown(VK_BACK) ||
        IsPotentialAutosaveKeyDown(VK_F1) ||
        ClassifyKeyDownAutosaveActionForState(
            'A', false, false, false, false) !=
            KeyDownAutosaveAction::TextInput ||
        ClassifyKeyDownAutosaveActionForState(
            'A', false, true, false, false) !=
            KeyDownAutosaveAction::TextInput ||
        ClassifyKeyDownAutosaveActionForState(
            'A', false, false, false, true) !=
            KeyDownAutosaveAction::None ||
        ClassifyKeyDownAutosaveActionForState(
            VK_BACK, false, false, false, true) !=
            KeyDownAutosaveAction::EditingInput ||
        ClassifyKeyDownAutosaveActionForState(
            VK_BACK, true, false, false, false) !=
            KeyDownAutosaveAction::None ||
        ClassifyKeyDownAutosaveActionForState(
            'B', true, true, false, false) !=
            KeyDownAutosaveAction::EditingInput ||
        ClassifyKeyDownAutosaveActionForState(
            VK_RETURN, true, true, false, false) !=
            KeyDownAutosaveAction::EditingInput ||
        ClassifyKeyDownAutosaveActionForState(
            'S', true, false, false, false) !=
            KeyDownAutosaveAction::ManualSave ||
        ClassifyKeyDownAutosaveActionForState(
            'S', true, true, false, false) !=
            KeyDownAutosaveAction::None ||
        ClassifyKeyDownAutosaveActionForState(
            'B', true, false, true, false) !=
            KeyDownAutosaveAction::None) {
        ReportSelfTestFailure(&success, L"unified keydown shortcut classification");
    }

    GrowablePathBuffer smallPathBuffer;
    if (smallPathBuffer.EnsureCapacity(0) ||
        !smallPathBuffer.EnsureCapacity(1) ||
        smallPathBuffer.Capacity() < 1 ||
        !smallPathBuffer.EnsureCapacity(smallPathBuffer.Capacity()) ||
        GetNextPathBufferCapacity(0, 0) != 1 ||
        GetNextPathBufferCapacity(MAX_PATH, MAX_PATH) != MAX_PATH + 1 ||
        GetNextPathBufferCapacity(MAX_CANONICAL_PATH_CHARS,
                                  MAX_CANONICAL_PATH_CHARS) != 0 ||
        GetNextPathBufferCapacity(MAX_CANONICAL_PATH_CHARS + 1,
                                  MAX_CANONICAL_PATH_CHARS) != 0) {
        ReportSelfTestFailure(&success, L"growable path buffer boundaries");
    }

    const TranslateMessage_t previousTranslateMessage =
        g_originalTranslateMessage;
    const bool previousModuleActive =
        LoadFlag(g_runtime.flags.moduleActive);
    const LONG previousPendingRequests =
        g_control.pendingRequests.exchange(0, std::memory_order_acq_rel);
    ClearFlag(g_runtime.flags.moduleActive);

    g_originalTranslateMessage = nullptr;
    const DWORD missingOriginalLastError = 0x24681357u;
    SetLastError(missingOriginalLastError);
    const BOOL missingOriginalResult = TranslateMessage_Hook(nullptr);
    const DWORD missingOriginalObservedError = GetLastError();

    g_originalTranslateMessage = SelfTestOriginalTranslateMessage;
    MSG irrelevantMessage = {};
    irrelevantMessage.message = WM_PAINT;
    const bool irrelevantFastPathEligible =
        !HasPotentialHookRuntimeWork(&irrelevantMessage);
    const DWORD originalEntryLastError = 0x10203040u;
    g_selfTestTranslateEntryLastError = 0;
    SetLastError(originalEntryLastError);
    const BOOL originalResult = TranslateMessage_Hook(&irrelevantMessage);
    const DWORD originalObservedError = GetLastError();

    g_originalTranslateMessage = previousTranslateMessage;
    g_control.pendingRequests.store(previousPendingRequests,
                                    std::memory_order_release);
    StoreFlag(g_runtime.flags.moduleActive, previousModuleActive);
    if (!missingOriginalResult ||
        missingOriginalObservedError != missingOriginalLastError ||
        !irrelevantFastPathEligible ||
        originalResult != FALSE ||
        g_selfTestTranslateEntryLastError != originalEntryLastError ||
        originalObservedError != SELF_TEST_TRANSLATE_LAST_ERROR) {
        ReportSelfTestFailure(&success, L"TranslateMessage last-error preservation");
    }

    if (!IsAutosaveRelevantMessage(WM_KEYDOWN) ||
        !IsAutosaveRelevantMessage(WM_MOUSEWHEEL) ||
        !IsAutosaveRelevantMessage(WM_ACTIVATE) ||
        !IsAutosaveRelevantMessage(WM_IME_ENDCOMPOSITION) ||
        !IsAutosaveRelevantMessage(WM_EXITMENULOOP) ||
        !IsAutosaveRelevantMessage(WM_CANCELMODE) ||
        IsAutosaveRelevantMessage(WM_MOUSEMOVE) ||
        IsAutosaveRelevantMessage(WM_PAINT) ||
        IsAutosaveRelevantMessage(WM_TIMER)) {
        ReportSelfTestFailure(&success, L"hot-path message interest filter");
    }

    MSG activationMessage = {};
    activationMessage.message = WM_ACTIVATE;
    activationMessage.wParam = WA_ACTIVE;
    MSG deactivationMessage = activationMessage;
    deactivationMessage.wParam = WA_INACTIVE;
    MSG menuReleaseMessage = {};
    menuReleaseMessage.message = WM_EXITMENULOOP;
    MSG focusMessage = {};
    focusMessage.message = WM_SETFOCUS;
    MSG actionReleaseMessage = {};
    actionReleaseMessage.message = WM_LBUTTONUP;
    if (ClassifySaveResumeSignal(&activationMessage) !=
            SaveResumeSignal::WordActivated ||
        ClassifySaveResumeSignal(&deactivationMessage) !=
            SaveResumeSignal::None ||
        ClassifySaveResumeSignal(&menuReleaseMessage) !=
            SaveResumeSignal::UiReleased ||
        ClassifySaveResumeSignal(&focusMessage) !=
            SaveResumeSignal::UiReleased ||
        ClassifySaveResumeSignal(&actionReleaseMessage) !=
            SaveResumeSignal::ActionBoundary) {
        ReportSelfTestFailure(&success, L"retry resume signal classification");
    }

    if (ShouldResumeSaveForSignal(SaveRetryReason::None,
                                  SaveResumeSignal::WordActivated) ||
        !ShouldResumeSaveForSignal(SaveRetryReason::InactiveWindow,
                                   SaveResumeSignal::WordActivated) ||
        !ShouldResumeSaveForSignal(SaveRetryReason::HardFailure,
                                   SaveResumeSignal::DocumentChanged) ||
        !ShouldResumeSaveForSignal(SaveRetryReason::AutomationBusy,
                                   SaveResumeSignal::ActionBoundary) ||
        ShouldResumeSaveForSignal(SaveRetryReason::HardFailure,
                                  SaveResumeSignal::ActionBoundary) ||
        !ShouldResumeDocumentForSignal(DocumentRetryReason::Disconnected,
                                       SaveResumeSignal::WordActivated) ||
        !ShouldResumeDocumentForSignal(DocumentRetryReason::Busy,
                                       SaveResumeSignal::UiReleased) ||
        ShouldResumeDocumentForSignal(DocumentRetryReason::Hard,
                                      SaveResumeSignal::UiReleased)) {
        ReportSelfTestFailure(&success, L"reason-aware event resume policy");
    }

    if (ShouldWakeSaveForOrdinaryFallback(SaveRetryReason::HardFailure,
                                          true) ||
        ShouldWakeSaveForOrdinaryFallback(
            SaveRetryReason::TargetUnavailable,
            true) ||
        ShouldWakeSaveForOrdinaryFallback(
            SaveRetryReason::DeferredProtectedView,
            true) ||
        !ShouldWakeSaveForOrdinaryFallback(
            SaveRetryReason::AutomationBusy,
            true) ||
        !ShouldWakeSaveForOrdinaryFallback(SaveRetryReason::UiPaused,
                                           true) ||
        !ShouldWakeSaveForOrdinaryFallback(SaveRetryReason::InputBusy,
                                           true) ||
        !ShouldWakeSaveForOrdinaryFallback(SaveRetryReason::HardFailure,
                                           false)) {
        ReportSelfTestFailure(&success,
                              L"ordinary save fallback retry policy");
    }

    if (ShouldWakeDocumentForOrdinaryFallback(DocumentRetryReason::Hard,
                                              true) ||
        ShouldWakeDocumentForOrdinaryFallback(
            DocumentRetryReason::Disconnected,
            true) ||
        !ShouldWakeDocumentForOrdinaryFallback(DocumentRetryReason::Busy,
                                               true) ||
        !ShouldWakeDocumentForOrdinaryFallback(DocumentRetryReason::UiPaused,
                                               true) ||
        !ShouldWakeDocumentForOrdinaryFallback(DocumentRetryReason::InputBusy,
                                               true) ||
        !ShouldWakeDocumentForOrdinaryFallback(DocumentRetryReason::Hard,
                                               false)) {
        ReportSelfTestFailure(&success,
                              L"ordinary document fallback retry policy");
    }

    if (!ShouldApplyMessageAutosaveRole(
            MessageAutosaveRole::EditingInput,
            true,
            SaveRetryReason::HardFailure,
            true,
            DocumentRetryReason::Hard,
            true) ||
        !ShouldApplyMessageAutosaveRole(
            MessageAutosaveRole::TransitionBoundaryFallback,
            true,
            SaveRetryReason::HardFailure,
            true,
            DocumentRetryReason::Disconnected,
            true) ||
        ShouldApplyMessageAutosaveRole(
            MessageAutosaveRole::ActionBoundaryFallback,
            true,
            SaveRetryReason::HardFailure,
            true,
            DocumentRetryReason::Busy,
            true) ||
        ShouldApplyMessageAutosaveRole(
            MessageAutosaveRole::ActionBoundaryFallback,
            false,
            SaveRetryReason::None,
            false,
            DocumentRetryReason::Disconnected,
            true) ||
        !ShouldApplyMessageAutosaveRole(
            MessageAutosaveRole::ActionBoundaryFallback,
            false,
            SaveRetryReason::None,
            false,
            DocumentRetryReason::Busy,
            true) ||
        ShouldApplyMessageAutosaveRole(
            MessageAutosaveRole::DocumentRefreshFallback,
            false,
            SaveRetryReason::None,
            false,
            DocumentRetryReason::Hard,
            true) ||
        !ShouldApplyMessageAutosaveRole(
            MessageAutosaveRole::DocumentRefreshFallback,
            false,
            SaveRetryReason::None,
            false,
            DocumentRetryReason::Busy,
            true) ||
        ShouldApplyMessageAutosaveRole(MessageAutosaveRole::None,
                                       false,
                                       SaveRetryReason::None,
                                       false,
                                       DocumentRetryReason::None,
                                       false)) {
        ReportSelfTestFailure(&success,
                              L"message autosave fallback override policy");
    }

    if (SelectHookMessageRoute(false, false, 0, 42) !=
            HookMessageRoute::Ignore ||
        SelectHookMessageRoute(true, false, 0, 42) !=
            HookMessageRoute::AdoptOwner ||
        SelectHookMessageRoute(true, false, 42, 42) !=
            HookMessageRoute::HandleOnOwner ||
        SelectHookMessageRoute(false, true, 42, 42) !=
            HookMessageRoute::HandleOnOwner ||
        SelectHookMessageRoute(true, false, 43, 42) !=
            HookMessageRoute::Ignore) {
        ReportSelfTestFailure(&success, L"hot-path owner routing");
    }

    if (!ShouldQueueSchedulerRepair(false, 100, false, false) ||
        ShouldQueueSchedulerRepair(true, 100, false, false) ||
        ShouldQueueSchedulerRepair(false, 0, false, false) ||
        ShouldQueueSchedulerRepair(false, 100, true, false) ||
        ShouldQueueSchedulerRepair(false, 100, false, true)) {
        ReportSelfTestFailure(&success, L"bounded scheduler repair policy");
    }

    if (SelectDocumentMonitorSchedule(true,
                                      true,
                                      false,
                                      false,
                                      false,
                                      true,
                                      true) != DocumentMonitorScheduleKind::None ||
        SelectDocumentMonitorSchedule(true,
                                      true,
                                      false,
                                      false,
                                      false,
                                      false,
                                      true) !=
            DocumentMonitorScheduleKind::RepairSaveTask ||
        SelectDocumentMonitorSchedule(false,
                                      true,
                                      false,
                                      false,
                                      false,
                                      false,
                                      true) !=
            DocumentMonitorScheduleKind::RepairSaveTask ||
        SelectDocumentMonitorSchedule(false,
                                      false,
                                      true,
                                      false,
                                      false,
                                      false,
                                      true) != DocumentMonitorScheduleKind::ActiveRetry ||
        SelectDocumentMonitorSchedule(false,
                                      false,
                                      false,
                                      false,
                                      false,
                                      false,
                                      true) != DocumentMonitorScheduleKind::EventWatchdog ||
        SelectDocumentMonitorSchedule(false,
                                      false,
                                      false,
                                      false,
                                      false,
                                      false,
                                      false) != DocumentMonitorScheduleKind::FallbackPoll) {
        ReportSelfTestFailure(&success, L"one-shot document monitor selection");
    }

    if (GetDocumentMonitorScheduleDelay(DocumentMonitorScheduleKind::ActiveRetry) !=
            DOCUMENT_STATE_PENDING_WATCHDOG_MS ||
        GetDocumentMonitorScheduleDelay(DocumentMonitorScheduleKind::EventWatchdog) !=
            DOCUMENT_STATE_EVENT_IDLE_POLL_INTERVAL_MS ||
        GetDocumentMonitorScheduleDelay(DocumentMonitorScheduleKind::FallbackPoll) !=
            DOCUMENT_STATE_IDLE_POLL_INTERVAL_MS ||
        GetDocumentMonitorScheduleDelay(DocumentMonitorScheduleKind::None) != 0 ||
        GetDocumentMonitorScheduleDelay(DocumentMonitorScheduleKind::RepairSaveTask) != 0) {
        ReportSelfTestFailure(&success, L"document monitor delay selection");
    }

    if (!IsDocumentFailureRetryReason(DocumentRetryReason::Busy) ||
        !IsDocumentFailureRetryReason(DocumentRetryReason::Disconnected) ||
        !IsDocumentFailureRetryReason(DocumentRetryReason::Hard) ||
        IsDocumentFailureRetryReason(DocumentRetryReason::UiPaused) ||
        IsDocumentFailureRetryReason(DocumentRetryReason::None)) {
        ReportSelfTestFailure(&success, L"document failure retry classification");
    }

    return success;
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

    if (!AreSameDocumentPathText(L"C:\\Docs\\Test.docx",
                                 L"c:/docs/test.docx") ||
        !AreSameDocumentPathText(L"C:\\Docs\\Test.docx\\",
                                 L"c:/docs/test.docx") ||
        AreSameDocumentPathText(L"C:\\Docs\\One.docx",
                                L"C:\\Docs\\Two.docx") ||
        CompareDocumentPathText(L"C:\\Docs\\One.docx",
                                L"C:\\Docs\\Two.docx") !=
            DocumentPathTextComparison::RequiresResolution ||
        CompareDocumentPathText(nullptr, L"C:\\Docs\\Test.docx") !=
            DocumentPathTextComparison::Different ||
        CompareDocumentPathText(nullptr, L"") !=
            DocumentPathTextComparison::Equal) {
        ReportSelfTestFailure(&success, L"no-I/O path text comparison");
    }

    const wchar_t win32DrivePath[] = L"\\\\?\\C:\\Docs\\Test.docx";
    if (AreSameDocumentPathText(win32DrivePath, L"C:\\Docs\\Test.docx") ||
        CompareDocumentPathText(win32DrivePath,
                                L"C:\\Docs\\Test.docx") !=
            DocumentPathTextComparison::RequiresResolution ||
        AreSameDocumentPathText(L"\\\\?\\C:\\Docs/Test.docx",
                                win32DrivePath) ||
        !AreSameDocumentPathText(L"\\\\?\\C:\\DOCS\\TEST.DOCX",
                                 win32DrivePath)) {
        ReportSelfTestFailure(&success,
                              L"extended namespace lexical isolation");
    }

    ScopedBstr normalizedDrivePath;
    if (!StoreNormalizedFinalPath(win32DrivePath,
                                  static_cast<DWORD>(lstrlenW(win32DrivePath)),
                                  &normalizedDrivePath) ||
        lstrcmpW(normalizedDrivePath.CStr(), L"C:\\Docs\\Test.docx") != 0) {
        ReportSelfTestFailure(&success, L"Win32 drive path normalization");
    }

    const wchar_t win32UncPath[] = L"\\\\?\\UNC\\server\\share\\Test.docx";
    ScopedBstr normalizedUncPath;
    if (!StoreNormalizedFinalPath(win32UncPath,
                                  static_cast<DWORD>(lstrlenW(win32UncPath)),
                                  &normalizedUncPath) ||
        lstrcmpW(normalizedUncPath.CStr(), L"\\\\server\\share\\Test.docx") != 0) {
        ReportSelfTestFailure(&success, L"Win32 UNC path normalization");
    }

    const wchar_t win32VolumePath[] =
        L"\\\\?\\Volume{01234567-89AB-CDEF-0123-456789ABCDEF}\\Docs\\Test.docx";
    ScopedBstr normalizedVolumePath;
    if (!StoreNormalizedFinalPath(win32VolumePath,
                                  static_cast<DWORD>(lstrlenW(win32VolumePath)),
                                  &normalizedVolumePath) ||
        lstrcmpW(normalizedVolumePath.CStr(), win32VolumePath) != 0) {
        ReportSelfTestFailure(&success, L"Win32 volume path preservation");
    }

    const auto verifyBackoff = [&success](DWORD initialDelayMs,
                                          DWORD maxDelayMs,
                                          const DWORD* expectedDelays,
                                          size_t expectedCount,
                                          const wchar_t* testName) {
        DWORD nextDelayMs = initialDelayMs;
        for (size_t index = 0; index < expectedCount; ++index) {
            if (AdvanceRetryDelayForPolicy(&nextDelayMs,
                                           initialDelayMs,
                                           maxDelayMs) != expectedDelays[index]) {
                ReportSelfTestFailure(&success, testName);
                return;
            }
        }

        if (nextDelayMs != maxDelayMs) {
            ReportSelfTestFailure(&success, testName);
        }
    };

    const DWORD busyDelays[] = {125, 250, 500, 1000, 2000, 4000, 5000, 5000};
    const DWORD hardDelays[] = {2000, 4000, 8000, 16000, 30000, 30000};
    const DWORD targetDelays[] = {250, 500, 1000, 2000, 4000, 5000, 5000};
    verifyBackoff(SAVE_AUTOMATION_RETRY_INITIAL_MS,
                  SAVE_AUTOMATION_RETRY_MAX_MS,
                  busyDelays,
                  ARRAYSIZE(busyDelays),
                  L"busy retry backoff progression");
    verifyBackoff(SAVE_HARD_RETRY_INITIAL_MS,
                  SAVE_HARD_RETRY_MAX_MS,
                  hardDelays,
                  ARRAYSIZE(hardDelays),
                  L"hard retry backoff progression");
    verifyBackoff(SAVE_TARGET_RETRY_INITIAL_MS,
                  SAVE_TARGET_RETRY_MAX_MS,
                  targetDelays,
                  ARRAYSIZE(targetDelays),
                  L"target retry backoff progression");
    verifyBackoff(DOCUMENT_BUSY_RETRY_INITIAL_MS,
                  DOCUMENT_BUSY_RETRY_MAX_MS,
                  busyDelays,
                  ARRAYSIZE(busyDelays),
                  L"document busy retry backoff progression");
    verifyBackoff(DOCUMENT_HARD_RETRY_INITIAL_MS,
                  DOCUMENT_HARD_RETRY_MAX_MS,
                  hardDelays,
                  ARRAYSIZE(hardDelays),
                  L"document hard retry backoff progression");
    verifyBackoff(DOCUMENT_TARGET_RETRY_INITIAL_MS,
                  DOCUMENT_TARGET_RETRY_MAX_MS,
                  targetDelays,
                  ARRAYSIZE(targetDelays),
                  L"document target retry backoff progression");

    const DWORD canceledRetry = static_cast<DWORD>(-1);
    if (SelectRejectedCallRetryDelay(ComRetryProfile::Background,
                                     SERVERCALL_RETRYLATER,
                                     999,
                                     99,
                                     100) != COM_BACKGROUND_RETRY_DELAY_MS ||
        SelectRejectedCallRetryDelay(ComRetryProfile::Background,
                                     SERVERCALL_RETRYLATER,
                                     1000,
                                     99,
                                     100) != canceledRetry ||
        SelectRejectedCallRetryDelay(ComRetryProfile::Background,
                                     SERVERCALL_RETRYLATER,
                                     10,
                                     100,
                                     100) != canceledRetry ||
        SelectRejectedCallRetryDelay(ComRetryProfile::CriticalClose,
                                     SERVERCALL_RETRYLATER,
                                     9999,
                                     99,
                                     100) != COM_MESSAGE_FILTER_RETRY_DELAY_MS ||
        SelectRejectedCallRetryDelay(ComRetryProfile::LegacyLifecycle,
                                     SERVERCALL_RETRYLATER,
                                     10000,
                                     99,
                                     100) != canceledRetry ||
        SelectRejectedCallRetryDelay(ComRetryProfile::Background,
                                     SERVERCALL_REJECTED,
                                     10,
                                     99,
                                     100) != canceledRetry) {
        ReportSelfTestFailure(&success, L"COM retry profile deadline policy");
    }

    if (!ShouldLogRetryReasonNow(0, 1, 0, 100, 10000) ||
        ShouldLogRetryReasonNow(1, 1, 100, 1000, 10000) ||
        !ShouldLogRetryReasonNow(1, 1, 100, 10100, 10000) ||
        ShouldLogRetryReasonNow(1, 2, 100, 2000, 10000) ||
        !ShouldLogRetryReasonNow(1, 2, 100, 2100, 10000)) {
        ReportSelfTestFailure(&success, L"bounded retry reason logging");
    }

    BSTR selfResetText = SysAllocString(L"self-reset");
    ScopedBstr selfResetBstr;
    if (!selfResetText) {
        ReportSelfTestFailure(&success, L"BSTR self-reset allocation");
    } else {
        selfResetBstr.Reset(selfResetText);
        selfResetBstr.Reset(selfResetBstr.Get());
        if (lstrcmpW(selfResetBstr.CStr(), L"self-reset") != 0) {
            ReportSelfTestFailure(&success, L"BSTR self-reset guard");
        }
    }

    SelfTestDispatch selfResetDispatch;
    ScopedComPtr<IDispatch> selfResetDispatchPtr;
    selfResetDispatchPtr.Reset(&selfResetDispatch);
    selfResetDispatchPtr.Reset(selfResetDispatchPtr.Get());
    if (selfResetDispatchPtr.Get() != &selfResetDispatch) {
        ReportSelfTestFailure(&success, L"COM pointer self-reset guard");
    }

    if (!ShouldUseMessageDocumentRefreshFallback()) {
        ReportSelfTestFailure(&success, L"message refresh fallback gating");
    }

    if (!ShouldUseMessageBoundaryFallback(false, false) ||
        !ShouldUseMessageBoundaryFallback(true, true) ||
        ShouldUseMessageBoundaryFallback(true, false)) {
        ReportSelfTestFailure(&success, L"boundary fallback gating");
    }

    if (!IsPlainTextInputVirtualKeyForState('A', false, false, false) ||
        !IsPlainTextInputVirtualKeyForState('1', false, false, false) ||
        !IsPlainTextInputVirtualKeyForState(VK_OEM_PERIOD, false, false, false) ||
        IsPlainTextInputVirtualKeyForState('A', true, false, false) ||
        IsPlainTextInputVirtualKeyForState('A', false, true, false) ||
        IsPlainTextInputVirtualKeyForState(VK_BACK, false, false, false)) {
        ReportSelfTestFailure(&success, L"plain text keydown classification");
    }

    if (!ShouldSuppressCharacterInputAfterKeyDown(true, 100, 150) ||
        ShouldSuppressCharacterInputAfterKeyDown(true,
                                                 100,
                                                 100 + TEXT_INPUT_KEYDOWN_CHAR_SUPPRESSION_MS + 1) ||
        ShouldSuppressCharacterInputAfterKeyDown(false, 100, 150) ||
        ShouldSuppressCharacterInputAfterKeyDown(true, 0, 150)) {
        ReportSelfTestFailure(&success, L"text keydown character suppression");
    }

    if (NormalizeComWindowHandleValue(0) != 0 ||
        NormalizeComWindowHandleValue(0x1234) != 0x1234) {
        ReportSelfTestFailure(&success, L"COM window handle normalization");
    }
#ifdef _WIN64
    const LONG_PTR signExtendedHwnd =
        static_cast<LONG_PTR>(static_cast<ULONG_PTR>(0xFFFFFFFF87654321ull));
    const LONG_PTR zeroExtendedHwnd =
        static_cast<LONG_PTR>(static_cast<ULONG_PTR>(0x87654321ull));
    if (NormalizeComWindowHandleValue(signExtendedHwnd) != zeroExtendedHwnd) {
        ReportSelfTestFailure(&success, L"sign-extended COM window handle normalization");
    }
#endif

    if (!ShouldAttemptOwnerThreadAdoptionForState(WM_KEYDOWN, true, false, false) ||
        !ShouldAttemptOwnerThreadAdoptionForState(WM_TIMER, false, true, false) ||
        !ShouldAttemptOwnerThreadAdoptionForState(WM_TIMER, false, false, true) ||
        ShouldAttemptOwnerThreadAdoptionForState(WM_TIMER, true, true, true) ||
        ShouldAttemptOwnerThreadAdoptionForState(WM_TIMER, false, false, false)) {
        ReportSelfTestFailure(&success, L"startup owner-thread adoption policy");
    }

    if (!ShouldTrackDocumentStateWhileInactive(true, false, true) ||
        !ShouldTrackDocumentStateWhileInactive(false, true, true) ||
        !ShouldTrackDocumentStateWhileInactive(false, false, false) ||
        ShouldTrackDocumentStateWhileInactive(false, false, true)) {
        ReportSelfTestFailure(&success, L"inactive document-state tracking");
    }

    SelfTestDispatch dummyDispatch;
    IDispatch* dummyDocument = &dummyDispatch;
    if (!HasTransitionFlushTarget(L"C:\\dummy.docx", nullptr) ||
        !HasTransitionFlushTarget(nullptr, dummyDocument) ||
        HasTransitionFlushTarget(nullptr, nullptr) ||
        HasTransitionFlushTarget(L"", nullptr)) {
        ReportSelfTestFailure(&success, L"transition flush target gating");
    }

    if (!ShouldPreserveTransitionFlushForManualSave(true) ||
        ShouldPreserveTransitionFlushForManualSave(false)) {
        ReportSelfTestFailure(&success, L"manual save transition preservation");
    }

    const HRESULT missingWordFailure = HRESULT_FROM_WIN32(ERROR_NOT_FOUND);
    if (SelectWordApplicationResolutionFailure(false,
                                               RPC_E_SERVERCALL_RETRYLATER,
                                               missingWordFailure) != missingWordFailure ||
        SelectWordApplicationResolutionFailure(true,
                                               RPC_E_SERVERCALL_RETRYLATER,
                                               missingWordFailure) !=
            RPC_E_SERVERCALL_RETRYLATER) {
        ReportSelfTestFailure(&success, L"Word application failure selection");
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
    dispIds.windowActivate = 4;
    dispIds.windowDeactivate = 5;
    if (ClassifyWordApplicationEvent(dispIds, 1) !=
            WordApplicationEventKind::DocumentBeforeSave ||
        ClassifyWordApplicationEvent(dispIds, 2) !=
            WordApplicationEventKind::DocumentBeforeClose ||
        ClassifyWordApplicationEvent(dispIds, 3) !=
            WordApplicationEventKind::DocumentChange ||
        ClassifyWordApplicationEvent(dispIds, 4) !=
            WordApplicationEventKind::WindowActivate ||
        ClassifyWordApplicationEvent(dispIds, 5) !=
            WordApplicationEventKind::WindowDeactivate ||
        ClassifyWordApplicationEvent(dispIds, 99) !=
            WordApplicationEventKind::None) {
        ReportSelfTestFailure(&success, L"Word event dispatch classification");
    }

    if (!DidWordEventUnadviseComplete(S_OK) ||
        !DidWordEventUnadviseComplete(S_FALSE) ||
        !DidWordEventUnadviseComplete(kConnectENoConnection) ||
        !DidWordEventUnadviseComplete(kCoEObjectNotConnected) ||
        !DidWordEventUnadviseComplete(RPC_E_DISCONNECTED) ||
        !DidWordEventUnadviseComplete(RPC_E_SERVER_DIED) ||
        !DidWordEventUnadviseComplete(RPC_E_SERVER_DIED_DNE) ||
        DidWordEventUnadviseComplete(RPC_E_SERVERCALL_RETRYLATER) ||
        DidWordEventUnadviseComplete(E_FAIL)) {
        ReportSelfTestFailure(&success, L"Word event unadvise completion policy");
    }

    if (!ShouldRetryShutdownWordEventUnadvise(RPC_E_SERVERCALL_RETRYLATER,
                                              0,
                                              2) ||
        ShouldRetryShutdownWordEventUnadvise(RPC_E_SERVERCALL_RETRYLATER,
                                             1,
                                             2) ||
        ShouldRetryShutdownWordEventUnadvise(kConnectENoConnection,
                                             0,
                                             2) ||
        ShouldRetryShutdownWordEventUnadvise(E_FAIL,
                                             0,
                                             2)) {
        ReportSelfTestFailure(&success, L"shutdown unadvise retry policy");
    }

    if (!ShouldPreserveDeferredWordEventDisconnects(RuntimeResetMode::Reload) ||
        ShouldPreserveDeferredWordEventDisconnects(RuntimeResetMode::Shutdown)) {
        ReportSelfTestFailure(&success, L"Word event reset preservation policy");
    }

    g_runtime.events.session.Reset();
    g_runtime.events.stagedSession.Reset();
    for (int index = 0;
         index < WORD_EVENT_PENDING_DISCONNECT_CAPACITY;
         ++index) {
        g_runtime.events.pendingDisconnectSessions[index].Reset();
    }
    g_runtime.events.connectionBuildDepth = 0;
    ClearFlag(g_runtime.flags.wordEventsConnected);
    ClearFlag(g_runtime.flags.wordEventDisconnectRetryPending);

    const unsigned int policyEpoch = g_runtime.events.connectionEpoch;
    if (!CanCommitWordEventConnection(policyEpoch) ||
        CanCommitWordEventConnection(policyEpoch + 1)) {
        ReportSelfTestFailure(&success, L"Word event connection epoch policy");
    }
    {
        ScopedOwnerDepth buildDepth(
            g_runtime.events.connectionBuildDepth);
        if (!IsWordEventConnectionBuildInProgress()) {
            ReportSelfTestFailure(&success,
                                  L"Word event connection build guard");
        }
    }
    {
        ScopedOwnerDepth primaryDepth(
            g_runtime.events.primaryDisconnectDepth);
        if (CanCommitWordEventConnection(policyEpoch)) {
            ReportSelfTestFailure(&success,
                                  L"primary teardown commit exclusion");
        }
    }
    {
        ScopedOwnerDepth retryDepth(
            g_runtime.events.pendingDisconnectRetryDepth);
        if (CanCommitWordEventConnection(policyEpoch)) {
            ReportSelfTestFailure(&success,
                                  L"pending teardown commit exclusion");
        }
    }

    CountingDispatch resetApplication(L"Saved", 701);
    CountingDispatch resetSink(L"Saved", 702);
    CountingDispatch replacementApplication(L"Saved", 703);
    SelfTestConnectionPoint resetConnectionPoint;
    WordEventSession resetSession;
    ReplaceStoredComPtr(&resetSession.application,
                        static_cast<IDispatch*>(&resetApplication));
    ReplaceStoredComPtr(&resetSession.connectionPoint,
                        static_cast<IConnectionPoint*>(
                            &resetConnectionPoint));
    ReplaceStoredComPtr(&resetSession.sink,
                        static_cast<IDispatch*>(&resetSink));
    resetSession.applicationHwnd = 42;
    resetSession.cookie = 1;
    SessionResetReentryTestContext resetContext = {};
    resetContext.session = &resetSession;
    resetContext.replacementApplication = &replacementApplication;
    resetApplication.RunOnceOnNextRelease(ReenterWordEventSessionReset,
                                          &resetContext);
    resetSession.Reset();
    if (!resetContext.observedFullyDetached ||
        resetSession.application.Get() != &replacementApplication ||
        resetSession.applicationHwnd != 77 || resetSession.connectionPoint ||
        resetSession.sink || resetSession.sinkControl ||
        resetSession.cookie != 0) {
        ReportSelfTestFailure(&success,
                              L"re-entrant Word event session reset");
    }
    resetSession.Reset();

    SelfTestConnectionPoint primaryConnectionPoint;
    ReplaceStoredComPtr(&g_runtime.events.session.connectionPoint,
                        static_cast<IConnectionPoint*>(
                            &primaryConnectionPoint));
    g_runtime.events.session.cookie = 11;
    SetFlag(g_runtime.flags.wordEventsConnected);
    EventDisconnectReentryTestContext disconnectContext = {};
    primaryConnectionPoint.RunOnceOnNextUnadvise(
        ReenterPrimaryWordEventDisconnect,
        &disconnectContext);
    const unsigned int disconnectEpoch =
        g_runtime.events.connectionEpoch;
    DisconnectWordApplicationEvents(false);
    if (primaryConnectionPoint.UnadviseCallCount() != 1 ||
        !disconnectContext.observedDisconnected ||
        !disconnectContext.observedGlobalSessionEmpty ||
        !disconnectContext.observedTeardownGuard ||
        !g_runtime.events.session.IsEmpty() ||
        LoadFlag(g_runtime.flags.wordEventsConnected) ||
        g_runtime.events.primaryDisconnectDepth != 0 ||
        g_runtime.events.connectionEpoch != disconnectEpoch + 1) {
        ReportSelfTestFailure(&success,
                              L"recursive primary Word event disconnect");
    }

    WordEventSession* pendingSession =
        &g_runtime.events.pendingDisconnectSessions[0];
    SelfTestConnectionPoint retryConnectionPoint;
    ReplaceStoredComPtr(&pendingSession->connectionPoint,
                        static_cast<IConnectionPoint*>(
                            &retryConnectionPoint));
    pendingSession->cookie = 12;
    retryConnectionPoint.RunOnceOnNextUnadvise(
        ReenterPendingWordEventDisconnectRetry,
        nullptr);
    SetFlag(g_runtime.flags.wordEventDisconnectRetryPending);
    RetryPendingWordEventDisconnects();
    if (retryConnectionPoint.UnadviseCallCount() != 1 ||
        !pendingSession->IsEmpty() ||
        LoadFlag(g_runtime.flags.wordEventDisconnectRetryPending) ||
        g_runtime.events.pendingDisconnectRetryDepth != 0) {
        ReportSelfTestFailure(&success,
                              L"recursive deferred Word event disconnect");
    }

    SelfTestConnectionPoint shutdownConnectionPoint;
    ReplaceStoredComPtr(&pendingSession->connectionPoint,
                        static_cast<IConnectionPoint*>(
                            &shutdownConnectionPoint));
    pendingSession->cookie = 13;
    bool nestedShutdownCompleted = true;
    shutdownConnectionPoint.RunOnceOnNextUnadvise(
        ReenterWordEventShutdown,
        &nestedShutdownCompleted);
    SetFlag(g_runtime.flags.wordEventDisconnectRetryPending);
    RetryPendingWordEventDisconnects();
    if (nestedShutdownCompleted ||
        shutdownConnectionPoint.UnadviseCallCount() != 1 ||
        !pendingSession->IsEmpty() ||
        g_runtime.events.pendingDisconnectRetryDepth != 0) {
        ReportSelfTestFailure(
            &success,
            L"shutdown exclusion during deferred event disconnect");
    }

    CountingDispatch retainedPendingApplication(L"Saved", 704);
    ReplaceStoredComPtr(&pendingSession->application,
                        static_cast<IDispatch*>(
                            &retainedPendingApplication));
    if (FindFreePendingWordEventDisconnectSlot() == pendingSession) {
        ReportSelfTestFailure(&success,
                              L"retained pending event slot exclusion");
    }
    pendingSession->Reset();

    SelfTestConnectionPoint staleConnectionPoint;
    WordEventSession staleSession;
    ReplaceStoredComPtr(&staleSession.connectionPoint,
                        static_cast<IConnectionPoint*>(
                            &staleConnectionPoint));
    staleSession.cookie = 14;
    const unsigned int staleCommitEpoch =
        g_runtime.events.connectionEpoch;
    const HRESULT staleCommitHr =
        CommitWordEventSession(&staleSession, staleCommitEpoch + 1);
    if (staleCommitHr != E_ABORT ||
        staleConnectionPoint.UnadviseCallCount() != 1 ||
        !staleSession.IsEmpty() || !g_runtime.events.session.IsEmpty() ||
        g_runtime.events.connectionEpoch != staleCommitEpoch ||
        g_runtime.events.pendingDisconnectRetryDepth != 0) {
        ReportSelfTestFailure(&success,
                              L"stale Word event connection commit");
    }

    SelfTestConnectionPoint fullQueueConnectionPoints[
        WORD_EVENT_PENDING_DISCONNECT_CAPACITY];
    for (int index = 0;
         index < WORD_EVENT_PENDING_DISCONNECT_CAPACITY;
         ++index) {
        fullQueueConnectionPoints[index].SetUnadviseResult(E_FAIL);
        WordEventSession* fullSlot =
            &g_runtime.events.pendingDisconnectSessions[index];
        ReplaceStoredComPtr(
            &fullSlot->connectionPoint,
            static_cast<IConnectionPoint*>(
                &fullQueueConnectionPoints[index]));
        fullSlot->cookie = 100u + static_cast<DWORD>(index);
    }

    SelfTestConnectionPoint retainedPrimaryConnectionPoint;
    retainedPrimaryConnectionPoint.SetUnadviseResult(E_FAIL);
    ReplaceStoredComPtr(
        &g_runtime.events.session.connectionPoint,
        static_cast<IConnectionPoint*>(&retainedPrimaryConnectionPoint));
    g_runtime.events.session.cookie = 200;
    SetFlag(g_runtime.flags.wordEventsConnected);
    SetFlag(g_runtime.flags.wordEventDisconnectRetryPending);
    const bool fullQueueDisconnectCompleted =
        DisconnectWordApplicationEvents(true);
    if (fullQueueDisconnectCompleted ||
        retainedPrimaryConnectionPoint.UnadviseCallCount() != 1 ||
        !g_runtime.events.session.IsConnected() ||
        !g_runtime.events.stagedSession.IsEmpty() ||
        FindFreePendingWordEventDisconnectSlot() != nullptr ||
        LoadFlag(g_runtime.flags.wordEventsConnected) ||
        !LoadFlag(g_runtime.flags.wordEventDisconnectRetryPending)) {
        ReportSelfTestFailure(&success,
                              L"full event disconnect queue retention");
    }

    for (int index = 0;
         index < WORD_EVENT_PENDING_DISCONNECT_CAPACITY;
         ++index) {
        fullQueueConnectionPoints[index].SetUnadviseResult(S_OK);
    }
    retainedPrimaryConnectionPoint.SetUnadviseResult(S_OK);
    RetryPendingWordEventDisconnects();
    if (retainedPrimaryConnectionPoint.UnadviseCallCount() != 2 ||
        !g_runtime.events.session.IsEmpty() ||
        !g_runtime.events.stagedSession.IsEmpty() ||
        LoadFlag(g_runtime.flags.wordEventDisconnectRetryPending)) {
        ReportSelfTestFailure(&success,
                              L"retained event session cleanup");
    }

    ClearFlag(g_runtime.flags.wordEventsConnected);
    ClearFlag(g_runtime.flags.wordEventDisconnectRetryPending);

    return success;
}

bool RunSnapshotPolicySelfTests() {
    bool success = true;

    ActiveDocumentSnapshot metadataStateSnapshot;
    if (metadataStateSnapshot.metadataState !=
            SnapshotMetadataState::NotRequested ||
        metadataStateSnapshot.failureClass != AutomationFailureClass::None ||
        metadataStateSnapshot.hasPath) {
        ReportSelfTestFailure(&success,
                              L"snapshot metadata-not-requested state");
    }

    if (ClassifyAutomationFailure(RPC_E_CALL_REJECTED) !=
            AutomationFailureClass::Busy ||
        ClassifyAutomationFailure(kCoEObjectNotConnected) !=
            AutomationFailureClass::Disconnected ||
        ClassifyAutomationFailure(E_FAIL) != AutomationFailureClass::Hard ||
        ClassifyAutomationFailure(S_OK) != AutomationFailureClass::None) {
        ReportSelfTestFailure(&success, L"automation failure classification");
    }

    HRESULT firstDocumentLookupHardFailure =
        RememberFirstDocumentLookupHardFailure(S_OK, E_FAIL);
    firstDocumentLookupHardFailure = RememberFirstDocumentLookupHardFailure(
        firstDocumentLookupHardFailure,
        E_OUTOFMEMORY);
    if (firstDocumentLookupHardFailure != E_FAIL ||
        RememberFirstDocumentLookupHardFailure(S_OK, S_OK) != S_OK ||
        SelectDocumentPathLookupMissResult(firstDocumentLookupHardFailure) !=
            E_FAIL ||
        SelectDocumentPathLookupMissResult(S_OK) !=
            HRESULT_FROM_WIN32(ERROR_NOT_FOUND) ||
        !ShouldReturnDocumentLookupFailureImmediately(
            RPC_E_SERVERCALL_RETRYLATER) ||
        !ShouldReturnDocumentLookupFailureImmediately(
            kCoEObjectNotConnected) ||
        ShouldReturnDocumentLookupFailureImmediately(E_FAIL)) {
        ReportSelfTestFailure(&success,
                              L"document path lookup failure selection");
    }

    HRESULT firstNativeDocumentProbeHardFailure = S_OK;
    RememberNativeDocumentProbeFailure(DISP_E_UNKNOWNNAME,
                                       &firstNativeDocumentProbeHardFailure);
    RememberNativeDocumentProbeFailure(E_FAIL,
                                       &firstNativeDocumentProbeHardFailure);
    RememberNativeDocumentProbeFailure(E_OUTOFMEMORY,
                                       &firstNativeDocumentProbeHardFailure);
    RememberNativeDocumentProbeFailure(RPC_E_CALL_REJECTED,
                                       &firstNativeDocumentProbeHardFailure);

    HRESULT ignoredNativeDocumentProbeFailure = S_OK;
    RememberNativeDocumentProbeFailure(DISP_E_MEMBERNOTFOUND,
                                       &ignoredNativeDocumentProbeFailure);
    RememberNativeDocumentProbeFailure(kCoEObjectNotConnected,
                                       &ignoredNativeDocumentProbeFailure);

    if (ClassifyNativeDocumentLookupResult(S_OK, true) !=
            NativeDocumentLookupOutcome::Ready ||
        ClassifyNativeDocumentLookupResult(S_FALSE, false) !=
            NativeDocumentLookupOutcome::Missing ||
        ClassifyNativeDocumentLookupResult(S_OK, false) !=
            NativeDocumentLookupOutcome::RetryLater ||
        ClassifyNativeDocumentLookupResult(E_FAIL, false) !=
            NativeDocumentLookupOutcome::RetryLater ||
        ClassifyNativeDocumentLookupResult(RPC_E_CALL_REJECTED, false) !=
            NativeDocumentLookupOutcome::RetryLater ||
        ClassifyNativeDocumentLookupResult(kCoEObjectNotConnected, false) !=
            NativeDocumentLookupOutcome::RetryLater ||
        !ShouldPropagateNativeDocumentProbeFailure(
            RPC_E_SERVERCALL_RETRYLATER) ||
        !ShouldPropagateNativeDocumentProbeFailure(
            kCoEObjectNotConnected) ||
        ShouldPropagateNativeDocumentProbeFailure(E_FAIL) ||
        ShouldPropagateNativeDocumentProbeFailure(E_OUTOFMEMORY) ||
        ShouldPropagateNativeDocumentProbeFailure(DISP_E_UNKNOWNNAME) ||
        ShouldPropagateNativeDocumentProbeFailure(DISP_E_MEMBERNOTFOUND) ||
        !IsExpectedNativeDocumentProbeMiss(DISP_E_UNKNOWNNAME) ||
        !IsExpectedNativeDocumentProbeMiss(DISP_E_MEMBERNOTFOUND) ||
        IsExpectedNativeDocumentProbeMiss(E_FAIL) ||
        firstNativeDocumentProbeHardFailure != E_FAIL ||
        FAILED(ignoredNativeDocumentProbeFailure) ||
        SelectNativeDocumentProbeMissResult(
            firstNativeDocumentProbeHardFailure) != E_FAIL ||
        SelectNativeDocumentProbeMissResult(S_OK) != S_FALSE) {
        ReportSelfTestFailure(&success,
                              L"native active-document lookup policy");
    }

    ActiveDocumentSnapshot failureSnapshot;
    RecordSnapshotFailure(&failureSnapshot, RPC_E_SERVERCALL_RETRYLATER);
    if (failureSnapshot.failureClass != AutomationFailureClass::Busy ||
        failureSnapshot.failureHr != RPC_E_SERVERCALL_RETRYLATER ||
        GetSaveRetryReasonForSnapshot(&failureSnapshot) !=
            SaveRetryReason::AutomationBusy ||
        GetDocumentRetryReasonForSnapshot(&failureSnapshot) !=
            DocumentRetryReason::Busy) {
        ReportSelfTestFailure(&success, L"busy snapshot retry metadata");
    }

    RecordSnapshotFailure(&failureSnapshot, kCoEObjectNotConnected);
    if (GetSaveRetryReasonForSnapshot(&failureSnapshot) !=
            SaveRetryReason::TargetUnavailable ||
        GetDocumentRetryReasonForSnapshot(&failureSnapshot) !=
            DocumentRetryReason::Disconnected) {
        ReportSelfTestFailure(&success, L"disconnected snapshot retry metadata");
    }

    RecordSnapshotFailure(&failureSnapshot, E_OUTOFMEMORY);
    if (GetSaveRetryReasonForSnapshot(&failureSnapshot) !=
            SaveRetryReason::HardFailure ||
        GetDocumentRetryReasonForSnapshot(&failureSnapshot) !=
            DocumentRetryReason::Hard) {
        ReportSelfTestFailure(&success, L"hard snapshot retry metadata");
    }
    failureSnapshot.Reset();
    if (failureSnapshot.failureClass != AutomationFailureClass::None ||
        failureSnapshot.failureHr != S_OK) {
        ReportSelfTestFailure(&success, L"snapshot retry metadata reset");
    }

    if (!ShouldPopulateSnapshotMetadataForMode(false, SnapshotMetadataMode::WhenDirty) ||
        ShouldPopulateSnapshotMetadataForMode(true, SnapshotMetadataMode::WhenDirty) ||
        !ShouldPopulateSnapshotMetadataForMode(true, SnapshotMetadataMode::Always)) {
        ReportSelfTestFailure(&success, L"snapshot metadata mode gating");
    }

    if (!ShouldRetryInvalidCachedSnapshotDocument(
            true,
            false,
            AutomationFailureClass::Disconnected) ||
        ShouldRetryInvalidCachedSnapshotDocument(
            true,
            false,
            AutomationFailureClass::Busy) ||
        ShouldRetryInvalidCachedSnapshotDocument(
            true,
            false,
            AutomationFailureClass::Hard) ||
        ShouldRetryInvalidCachedSnapshotDocument(
            true,
            true,
            AutomationFailureClass::Disconnected) ||
        ShouldRetryInvalidCachedSnapshotDocument(
            false,
            false,
            AutomationFailureClass::Disconnected) ||
        !ShouldInvalidateTransitionFlushTargetForFailure(
            AutomationFailureClass::Disconnected) ||
        ShouldInvalidateTransitionFlushTargetForFailure(
            AutomationFailureClass::Busy) ||
        ShouldInvalidateTransitionFlushTargetForFailure(
            AutomationFailureClass::Hard)) {
        ReportSelfTestFailure(&success, L"invalid cached snapshot retry policy");
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

    SelfTestDispatch dummyDispatch;
    IDispatch* dummyDocument = &dummyDispatch;
    const SnapshotLoadPlan documentSavePlan =
        MakeSaveSnapshotLoadPlan(nullptr, dummyDocument);
    if (documentSavePlan.request.source != SnapshotSource::SpecificDocument ||
        documentSavePlan.request.document != dummyDocument) {
        ReportSelfTestFailure(&success, L"specific-document save snapshot load plan");
    }

    if (!AreSameDispatchComIdentity(dummyDocument, dummyDocument) ||
        AreSameDispatchComIdentity(dummyDocument, nullptr)) {
        ReportSelfTestFailure(&success, L"dispatch identity fast path");
    }

    if (!ShouldUseObservedAutosaveTarget(false, true, false) ||
        !ShouldUseObservedAutosaveTarget(false, false, true) ||
        ShouldUseObservedAutosaveTarget(false, false, false) ||
        ShouldUseObservedAutosaveTarget(true, true, true)) {
        ReportSelfTestFailure(&success, L"observed autosave target selection");
    }

    ScopedBstr sourceAutosavePath;
    ScopedBstr copiedAutosavePath;
    ScopedComPtr<IDispatch> copiedAutosaveDocument;
    if (!ReplaceStoredTextBstr(&sourceAutosavePath, L"C:\\dummy.docx") ||
        !CopyAutosaveTargetFromState(sourceAutosavePath,
                                     nullptr,
                                     &copiedAutosavePath,
                                     &copiedAutosaveDocument) ||
        !AreSameDocumentPathText(sourceAutosavePath.CStr(),
                                 copiedAutosavePath.CStr())) {
        ReportSelfTestFailure(&success, L"auto-save target copy");
    }

    RuntimeTickSnapshot normalAutosaveTick = {};
    RuntimeTickSnapshot transitionAutosaveTick = {};
    transitionAutosaveTick.runtime.transitionFlushRequested = true;
    if (GetTransitionFlushClearModeForTick(normalAutosaveTick) !=
            TransitionFlushClearMode::ClearPostTransitionRefresh ||
        GetTransitionFlushClearModeForTick(transitionAutosaveTick) !=
            TransitionFlushClearMode::PreservePostTransitionRefresh) {
        ReportSelfTestFailure(&success, L"transition clear mode selection");
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

    ScopedValueRestore<RuntimeSaveRetryState> saveRetryRestore(
        &g_runtime.saveRetry);
    ScopedValueRestore<ULONGLONG> lastSaveTimeRestore(
        &g_runtime.timing.lastSaveTime);
    g_runtime.saveRetry.reason = SaveRetryReason::HardFailure;
    g_runtime.saveRetry.hardNextDelayMs = SAVE_HARD_RETRY_MAX_MS;
    NoteSaveOperationTime(1234);
    if (g_runtime.timing.lastSaveTime != 1234 ||
        g_runtime.saveRetry.reason != SaveRetryReason::HardFailure ||
        g_runtime.saveRetry.hardNextDelayMs != SAVE_HARD_RETRY_MAX_MS ||
        ShouldResetSaveRetryAfterSuccessfulSnapshot(true) ||
        !ShouldResetSaveRetryAfterSuccessfulSnapshot(false)) {
        ReportSelfTestFailure(
            &success,
            L"unexpected successful snapshot retry preservation");
    }

    return success;
}

bool RunOwnerThreadAndSchedulerSelfTests() {
    bool success = true;

    std::atomic<LONG> atomicFlag{FALSE};
    if (LoadFlag(atomicFlag)) {
        ReportSelfTestFailure(&success, L"atomic runtime flag initial state");
    }
    SetFlag(atomicFlag);
    if (!LoadFlag(atomicFlag)) {
        ReportSelfTestFailure(&success, L"atomic runtime flag set/load");
    }
    {
        ScopedFlagSet scopedFlag(atomicFlag);
        if (!LoadFlag(atomicFlag)) {
            ReportSelfTestFailure(&success, L"atomic scoped flag publication");
        }
    }
    if (LoadFlag(atomicFlag)) {
        ReportSelfTestFailure(&success, L"atomic scoped flag clear");
    }
    StoreFlag(atomicFlag, true);
    ClearFlag(atomicFlag);
    if (LoadFlag(atomicFlag)) {
        ReportSelfTestFailure(&success, L"atomic runtime flag store/clear");
    }

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

    if (!HasActiveDocumentStatePollWorkForState(true, false, false) ||
        !HasActiveDocumentStatePollWorkForState(false, true, false) ||
        !HasActiveDocumentStatePollWorkForState(false, false, true) ||
        HasActiveDocumentStatePollWorkForState(false, false, false)) {
        ReportSelfTestFailure(&success, L"active document-state poll work policy");
    }

    if (!ShouldKeepExistingScheduledTaskDueTime(
            100,
            200,
            ScheduledTaskScheduleMode::ArmEarlier) ||
        ShouldKeepExistingScheduledTaskDueTime(
            100,
            200,
            ScheduledTaskScheduleMode::Reschedule) ||
        ShouldKeepExistingScheduledTaskDueTime(
            0,
            200,
            ScheduledTaskScheduleMode::ArmEarlier)) {
        ReportSelfTestFailure(&success, L"scheduled task update policy");
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

    WordEventSession sourceSession;
    sourceSession.applicationHwnd = 43;
    sourceSession.cookie = 18;
    WordEventSession movedSession(std::move(sourceSession));
    WordEventSession assignedSession;
    assignedSession = std::move(movedSession);
    if (assignedSession.applicationHwnd != 43 ||
        assignedSession.cookie != 18 ||
        sourceSession.applicationHwnd != 0 ||
        sourceSession.cookie != 0 ||
        movedSession.applicationHwnd != 0 ||
        movedSession.cookie != 0) {
        ReportSelfTestFailure(&success, L"word event session move reset");
    }

    RuntimeTimingState timingState = {};
    timingState.schedulerTimerId = 5;
    timingState.eventDisconnectRetryTimerDueTime = 6;
    timingState.lastEditTime = 7;
    timingState.transitionFlushRequestTime = 8;
    timingState.lastTextInputKeyDownTime = 9;
    ResetRuntimeTimingState(&timingState);
    if (timingState.schedulerTimerId != 0 ||
        timingState.eventDisconnectRetryTimerDueTime != 0 ||
        timingState.lastEditTime != 0 ||
        timingState.transitionFlushRequestTime != 0 ||
        timingState.lastTextInputKeyDownTime != 0) {
        ReportSelfTestFailure(&success, L"timing-state reset helper");
    }

    RuntimeSaveRetryState saveRetryState = {};
    saveRetryState.reason = SaveRetryReason::HardFailure;
    saveRetryState.hardNextDelayMs = SAVE_HARD_RETRY_MAX_MS;
    saveRetryState.lastReasonLogTime = 42;
    ResetRuntimeSaveRetryState(&saveRetryState);
    if (saveRetryState.reason != SaveRetryReason::None ||
        saveRetryState.automationNextDelayMs !=
            SAVE_AUTOMATION_RETRY_INITIAL_MS ||
        saveRetryState.hardNextDelayMs != SAVE_HARD_RETRY_INITIAL_MS ||
        saveRetryState.targetNextDelayMs != SAVE_TARGET_RETRY_INITIAL_MS ||
        saveRetryState.lastReasonLogTime != 0) {
        ReportSelfTestFailure(&success, L"save retry-state reset helper");
    }

    RuntimeDocumentRetryState documentRetryState = {};
    documentRetryState.reason = DocumentRetryReason::Disconnected;
    documentRetryState.disconnectedNextDelayMs =
        DOCUMENT_TARGET_RETRY_MAX_MS;
    documentRetryState.lastReasonLogTime = 42;
    ResetRuntimeDocumentRetryState(&documentRetryState);
    if (documentRetryState.reason != DocumentRetryReason::None ||
        documentRetryState.busyNextDelayMs != DOCUMENT_BUSY_RETRY_INITIAL_MS ||
        documentRetryState.disconnectedNextDelayMs !=
            DOCUMENT_TARGET_RETRY_INITIAL_MS ||
        documentRetryState.hardNextDelayMs != DOCUMENT_HARD_RETRY_INITIAL_MS ||
        documentRetryState.lastReasonLogTime != 0) {
        ReportSelfTestFailure(&success, L"document retry-state reset helper");
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
    ScopedValueRestore<ULONGLONG> eventDisconnectRetryDueTimeRestore(
        &g_runtime.timing.eventDisconnectRetryTimerDueTime);
    g_runtime.timing.schedulerTimerDueTime = 0;
    g_runtime.timing.saveTimerDueTime = 400;
    g_runtime.timing.documentStateTimerDueTime = 250;
    g_runtime.timing.eventDisconnectRetryTimerDueTime = 125;
    if (GetNextScheduledTaskDueTime() != 125) {
        ReportSelfTestFailure(&success, L"scheduler due-time selection");
    }

    return success;
}

bool RunTickDecisionSelfTests() {
    bool success = true;

    if (GetOwnerThreadSyncWatchdogDelay(true, true) !=
            DOCUMENT_STATE_PENDING_WATCHDOG_MS ||
        GetOwnerThreadSyncWatchdogDelay(false, true) !=
            SAVE_INACTIVE_EVENT_WATCHDOG_MS ||
        GetOwnerThreadSyncWatchdogDelay(false, false) !=
            SAVE_INACTIVE_FALLBACK_WATCHDOG_MS) {
        ReportSelfTestFailure(&success,
                              L"owner-thread synchronization watchdog policy");
    }

    RuntimeTickSnapshot ownerMismatchDocumentTick = {};
    ownerMismatchDocumentTick.runtime.wordEventsConnected = true;
    TickDecision ownerMismatchDocumentDecision =
        EvaluateDocumentStateTickDecision(ownerMismatchDocumentTick);
    ownerMismatchDocumentTick.runtime.expeditedSaveRequested = true;
    TickDecision criticalOwnerMismatchDocumentDecision =
        EvaluateDocumentStateTickDecision(ownerMismatchDocumentTick);
    if (ownerMismatchDocumentDecision.action !=
            TickDecisionAction::RearmDocumentStateTimer ||
        ownerMismatchDocumentDecision.state !=
            RuntimeStatePhase::WaitingForOwnerThread ||
        ownerMismatchDocumentDecision.delayMs !=
            SAVE_INACTIVE_EVENT_WATCHDOG_MS ||
        criticalOwnerMismatchDocumentDecision.action !=
            TickDecisionAction::RearmDocumentStateTimer ||
        criticalOwnerMismatchDocumentDecision.delayMs !=
            DOCUMENT_STATE_PENDING_WATCHDOG_MS) {
        ReportSelfTestFailure(&success,
                              L"owner-mismatch document-state rearm");
    }

    RuntimeTickSnapshot ownerMismatchSaveTick = {};
    ownerMismatchSaveTick.runtime.pendingSave = true;
    TickDecision ownerMismatchSaveDecision =
        EvaluateAutosaveTickDecision(ownerMismatchSaveTick);
    ownerMismatchSaveTick.runtime.transitionFlushRequested = true;
    TickDecision criticalOwnerMismatchSaveDecision =
        EvaluateAutosaveTickDecision(ownerMismatchSaveTick);
    if (ownerMismatchSaveDecision.action !=
            TickDecisionAction::RearmSaveTimer ||
        ownerMismatchSaveDecision.state !=
            RuntimeStatePhase::WaitingForOwnerThread ||
        ownerMismatchSaveDecision.delayMs !=
            SAVE_INACTIVE_FALLBACK_WATCHDOG_MS ||
        criticalOwnerMismatchSaveDecision.action !=
            TickDecisionAction::RearmSaveTimer ||
        criticalOwnerMismatchSaveDecision.delayMs !=
            DOCUMENT_STATE_PENDING_WATCHDOG_MS) {
        ReportSelfTestFailure(&success,
                              L"owner-mismatch save rearm");
    }

    RuntimeTickSnapshot pausedDocumentTick = {};
    pausedDocumentTick.runtime.ownerThreadSynchronized = true;
    pausedDocumentTick.runtime.pauseReason = WordUiPauseReason::MenuMode;
    pausedDocumentTick.steadyPollDelay = DOCUMENT_STATE_IDLE_POLL_INTERVAL_MS;
    TickDecision pausedDocumentDecision = EvaluateDocumentStateTickDecision(pausedDocumentTick);
    if (pausedDocumentDecision.action != TickDecisionAction::RearmDocumentStateTimer ||
        pausedDocumentDecision.state != RuntimeStatePhase::WaitingForWordUi ||
        pausedDocumentDecision.delayMs != SAVE_UI_INPUT_WATCHDOG_MS) {
        ReportSelfTestFailure(&success, L"paused document-state tick decision");
    }

    RuntimeTickSnapshot criticalPausedDocumentTick = pausedDocumentTick;
    criticalPausedDocumentTick.runtime.expeditedSaveRequested = true;
    TickDecision criticalPausedDocumentDecision =
        EvaluateDocumentStateTickDecision(criticalPausedDocumentTick);
    if (criticalPausedDocumentDecision.delayMs != INPUT_SETTLE_DELAY_MS) {
        ReportSelfTestFailure(&success,
                              L"critical paused document-state tick decision");
    }

    RuntimeTickSnapshot inactiveEventDocumentTick = {};
    inactiveEventDocumentTick.runtime.ownerThreadSynchronized = true;
    inactiveEventDocumentTick.runtime.wordEventsConnected = true;
    inactiveEventDocumentTick.pendingSaveWork = true;
    inactiveEventDocumentTick.steadyPollDelay =
        DOCUMENT_STATE_ACTIVE_POLL_INTERVAL_MS;
    TickDecision inactiveEventDocumentDecision =
        EvaluateDocumentStateTickDecision(inactiveEventDocumentTick);
    RuntimeTickSnapshot inactiveFallbackDocumentTick =
        inactiveEventDocumentTick;
    inactiveFallbackDocumentTick.runtime.wordEventsConnected = false;
    TickDecision inactiveFallbackDocumentDecision =
        EvaluateDocumentStateTickDecision(inactiveFallbackDocumentTick);
    RuntimeTickSnapshot criticalInactiveDocumentTick =
        inactiveEventDocumentTick;
    criticalInactiveDocumentTick.runtime.expeditedSaveRequested = true;
    TickDecision criticalInactiveDocumentDecision =
        EvaluateDocumentStateTickDecision(criticalInactiveDocumentTick);
    if (inactiveEventDocumentDecision.delayMs !=
            SAVE_INACTIVE_EVENT_WATCHDOG_MS ||
        inactiveFallbackDocumentDecision.delayMs !=
            SAVE_INACTIVE_FALLBACK_WATCHDOG_MS ||
        criticalInactiveDocumentDecision.delayMs !=
            DOCUMENT_STATE_ACTIVE_POLL_INTERVAL_MS) {
        ReportSelfTestFailure(&success,
                              L"inactive document-state watchdog policy");
    }

    RuntimeTickSnapshot inputBusyDocumentTick = {};
    inputBusyDocumentTick.runtime.ownerThreadSynchronized = true;
    inputBusyDocumentTick.runtime.activeWordDocumentWindow = true;
    inputBusyDocumentTick.runtime.inputBusy = true;
    TickDecision inputBusyDocumentDecision =
        EvaluateDocumentStateTickDecision(inputBusyDocumentTick);
    inputBusyDocumentTick.runtime.transitionFlushRequested = true;
    TickDecision criticalInputBusyDocumentDecision =
        EvaluateDocumentStateTickDecision(inputBusyDocumentTick);
    if (inputBusyDocumentDecision.delayMs != SAVE_UI_INPUT_WATCHDOG_MS ||
        criticalInputBusyDocumentDecision.delayMs != INPUT_SETTLE_DELAY_MS) {
        ReportSelfTestFailure(&success,
                              L"document-state input watchdog policy");
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

    RuntimeTickSnapshot pendingUnknownDocumentTick = {};
    pendingUnknownDocumentTick.runtime.ownerThreadSynchronized = true;
    pendingUnknownDocumentTick.runtime.activeWordDocumentWindow = true;
    pendingUnknownDocumentTick.runtime.pendingSave = true;
    pendingUnknownDocumentTick.flags.documentDirtyKnown = false;
    TickDecision pendingUnknownDocumentDecision =
        EvaluateDocumentStateTickDecision(pendingUnknownDocumentTick);
    if (pendingUnknownDocumentDecision.action != TickDecisionAction::RefreshDocumentState ||
        pendingUnknownDocumentDecision.state != RuntimeStatePhase::ReadyToRefreshDocumentState) {
        ReportSelfTestFailure(&success, L"pending unknown-document tick decision");
    }

    RuntimeTickSnapshot pendingArmedInactiveDocumentTick = {};
    pendingArmedInactiveDocumentTick.runtime.ownerThreadSynchronized = true;
    pendingArmedInactiveDocumentTick.runtime.pendingSave = true;
    pendingArmedInactiveDocumentTick.saveTaskArmed = true;
    TickDecision pendingArmedInactiveDocumentDecision =
        EvaluateDocumentStateTickDecision(pendingArmedInactiveDocumentTick);
    if (pendingArmedInactiveDocumentDecision.action != TickDecisionAction::None ||
        pendingArmedInactiveDocumentDecision.state !=
            RuntimeStatePhase::WaitingForSaveDelay) {
        ReportSelfTestFailure(&success,
                              L"pending save suppresses blocked document polling");
    }

    RuntimeTickSnapshot pendingArmedReadyDocumentTick =
        pendingArmedInactiveDocumentTick;
    pendingArmedReadyDocumentTick.runtime.activeWordDocumentWindow = true;
    pendingArmedReadyDocumentTick.flags.documentDirtyKnown = true;
    pendingArmedReadyDocumentTick.flags.documentDirty = true;
    TickDecision pendingArmedReadyDocumentDecision =
        EvaluateDocumentStateTickDecision(pendingArmedReadyDocumentTick);
    if (pendingArmedReadyDocumentDecision.action !=
            TickDecisionAction::RefreshDocumentState ||
        pendingArmedReadyDocumentDecision.state !=
            RuntimeStatePhase::ReadyToRefreshDocumentState) {
        ReportSelfTestFailure(&success,
                              L"explicit one-shot refresh for pending document");
    }

    ScopedValueRestore<int> saveDelayRestore(&g_settings.saveDelay);
    ScopedValueRestore<ULONGLONG> lastEditTimeRestore(&g_runtime.timing.lastEditTime);
    g_settings.saveDelay = 500;
    g_runtime.timing.lastEditTime = 1000;

    RuntimeTickSnapshot pendingDelaySaveTick = {};
    pendingDelaySaveTick.now = 1200;
    pendingDelaySaveTick.runtime.pendingSave = true;
    pendingDelaySaveTick.runtime.ownerThreadSynchronized = true;
    pendingDelaySaveTick.runtime.activeWordDocumentWindow = true;
    pendingDelaySaveTick.flags.documentDirtyKnown = false;
    TickDecision pendingDelaySaveDecision =
        EvaluateAutosaveTickDecision(pendingDelaySaveTick);
    if (pendingDelaySaveDecision.action != TickDecisionAction::RearmSaveTimer ||
        pendingDelaySaveDecision.state != RuntimeStatePhase::WaitingForSaveDelay ||
        pendingDelaySaveDecision.delayMs != 300) {
        ReportSelfTestFailure(&success, L"configured auto-save delay gating");
    }

    RuntimeTickSnapshot unknownSaveTick = pendingDelaySaveTick;
    unknownSaveTick.now = 1500;
    TickDecision unknownSaveDecision = EvaluateAutosaveTickDecision(unknownSaveTick);
    if (unknownSaveDecision.action != TickDecisionAction::SaveDocument ||
        unknownSaveDecision.state != RuntimeStatePhase::ReadyToSave) {
        ReportSelfTestFailure(&success, L"unknown-document auto-save readiness");
    }

    RuntimeTickSnapshot knownCleanSaveTick = unknownSaveTick;
    knownCleanSaveTick.flags.documentDirtyKnown = true;
    knownCleanSaveTick.flags.documentDirty = false;
    TickDecision knownCleanSaveDecision = EvaluateAutosaveTickDecision(knownCleanSaveTick);
    if (knownCleanSaveDecision.action != TickDecisionAction::SaveDocument ||
        knownCleanSaveDecision.state != RuntimeStatePhase::ReadyToSave) {
        ReportSelfTestFailure(&success, L"known-clean auto-save readiness");
    }

    RuntimeTickSnapshot pausedSaveTick = unknownSaveTick;
    pausedSaveTick.runtime.pauseReason = WordUiPauseReason::MenuMode;
    TickDecision pausedSaveDecision = EvaluateAutosaveTickDecision(pausedSaveTick);
    pausedSaveTick.runtime.transitionFlushRequested = true;
    TickDecision criticalPausedSaveDecision =
        EvaluateAutosaveTickDecision(pausedSaveTick);
    if (pausedSaveDecision.delayMs != SAVE_UI_INPUT_WATCHDOG_MS ||
        criticalPausedSaveDecision.delayMs != INPUT_SETTLE_DELAY_MS) {
        ReportSelfTestFailure(&success, L"auto-save UI watchdog policy");
    }

    RuntimeTickSnapshot inactiveSaveTick = unknownSaveTick;
    inactiveSaveTick.runtime.activeWordDocumentWindow = false;
    inactiveSaveTick.runtime.wordEventsConnected = true;
    TickDecision inactiveEventSaveDecision =
        EvaluateAutosaveTickDecision(inactiveSaveTick);
    inactiveSaveTick.runtime.wordEventsConnected = false;
    TickDecision inactiveFallbackSaveDecision =
        EvaluateAutosaveTickDecision(inactiveSaveTick);
    if (inactiveEventSaveDecision.delayMs != SAVE_INACTIVE_EVENT_WATCHDOG_MS ||
        inactiveFallbackSaveDecision.delayMs !=
            SAVE_INACTIVE_FALLBACK_WATCHDOG_MS) {
        ReportSelfTestFailure(&success, L"inactive auto-save watchdog policy");
    }

    RuntimeTickSnapshot inputBusySaveTick = unknownSaveTick;
    inputBusySaveTick.runtime.inputBusy = true;
    TickDecision inputBusySaveDecision =
        EvaluateAutosaveTickDecision(inputBusySaveTick);
    inputBusySaveTick.runtime.transitionFlushRequested = true;
    TickDecision criticalInputBusySaveDecision =
        EvaluateAutosaveTickDecision(inputBusySaveTick);
    if (inputBusySaveDecision.delayMs != SAVE_UI_INPUT_WATCHDOG_MS ||
        criticalInputBusySaveDecision.delayMs != INPUT_SETTLE_DELAY_MS) {
        ReportSelfTestFailure(&success, L"auto-save input watchdog policy");
    }

    ScopedValueRestore<int> minSaveIntervalRestore(&g_settings.minTimeBetweenSaves);
    ScopedValueRestore<ULONGLONG> lastSaveTimeRestore(&g_runtime.timing.lastSaveTime);
    ScopedValueRestore<ULONGLONG> transitionFlushRequestTimeRestore(
        &g_runtime.timing.transitionFlushRequestTime);
    g_settings.minTimeBetweenSaves = 5000;
    g_runtime.timing.lastSaveTime = 900;
    g_runtime.timing.lastEditTime = 990;
    g_runtime.timing.transitionFlushRequestTime = 100;

    RuntimeTickSnapshot readySaveTick = {};
    readySaveTick.now = 200;
    readySaveTick.runtime.pendingSave = true;
    readySaveTick.runtime.ownerThreadSynchronized = true;
    readySaveTick.runtime.activeWordDocumentWindow = true;
    readySaveTick.runtime.transitionFlushRequested = true;
    TickDecision readySaveDecision = EvaluateAutosaveTickDecision(readySaveTick);
    if (readySaveDecision.action != TickDecisionAction::SaveDocument ||
        readySaveDecision.state != RuntimeStatePhase::ReadyToSave) {
        ReportSelfTestFailure(&success, L"ready auto-save tick decision");
    }

    if (GetAutosaveDelayBaseTime(true) != 100 ||
        GetAutosaveDelayBaseTime(false) != 990) {
        ReportSelfTestFailure(&success, L"auto-save delay base time");
    }

    return success;
}

bool RunInternalSelfTests() {
    bool success = true;
    success = RunDispatchAndAutomationCacheSelfTests() && success;
    success = RunHotPathAndMonitoringPolicySelfTests() && success;
    success = RunTimingAndFallbackSelfTests() && success;
    success = RunWordEventPolicySelfTests() && success;
    success = RunSnapshotPolicySelfTests() && success;
    success = RunOwnerThreadAndSchedulerSelfTests() && success;
    success = RunTickDecisionSelfTests() && success;
    return success;
}
#endif

void ResetRuntimeTimingState(RuntimeTimingState* timing) {
    if (!timing) {
        return;
    }

    timing->schedulerTimerId = 0;
    timing->schedulerTimerDueTime = 0;
    timing->saveTimerDueTime = 0;
    timing->documentStateTimerDueTime = 0;
    timing->eventDisconnectRetryTimerDueTime = 0;
    timing->lastEditTime = 0;
    timing->transitionFlushRequestTime = 0;
    timing->lastSaveTime = 0;
    timing->lastEventConnectAttemptTime = 0;
    timing->lastTextInputKeyDownTime = 0;
    timing->pendingSaveAsTime = 0;
}

void ResetRuntimeStatusState(RuntimeStatusState* status) {
    if (!status) {
        return;
    }

    status->lastSaveFailureLogTime = 0;
    status->lastDocumentStateFailureLogTime = 0;
    status->lastEventDisconnectFailureLogTime = 0;
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

void ResetRuntimeState(RuntimeResetMode resetMode = RuntimeResetMode::Shutdown) {
    const bool preserveDeferredEventDisconnects =
        ShouldPreserveDeferredWordEventDisconnects(resetMode);
    CancelSchedulerTimer();
    ResetRuntimeTimingState(&g_runtime.timing);
    ResetSaveRetryState();
    ResetDocumentRetryState();
    DisconnectWordApplicationEvents(preserveDeferredEventDisconnects);
    if (preserveDeferredEventDisconnects) {
        RefreshPendingWordEventDisconnectRetryFlag();
    } else {
        DisconnectPendingWordEventDisconnectSessionsForShutdown();
    }
    ResetRuntimeStatusState(&g_runtime.status);
    ClearWordMemberIdCache();
    ResetAutomationCacheState();
    g_runtime.events.cachedDispIds.Reset();
    g_runtime.events.cachedDispIdsValid = false;
    ClearPendingSaveAsMigration();
    ClearPendingSave();
    ClearExpeditedSavePending();
    ClearPostTransitionRefreshPending();
    ClearTransitionFlushRequest();
    ClearManualSavePending();
    ClearFlag(g_runtime.flags.autoSaveInProgress);
    ClearFlag(g_runtime.flags.imeComposing);
    ClearFlag(g_runtime.flags.suppressNextCharacterInput);
    ResetObservedDocumentState();
    ResetRuntimeUiCacheState(&g_runtime.ui);
    DestroySchedulerMessageWindow();
    g_runtime.ownerWorkDepth = 0;
    ClearOwnerThreadId();
}

RuntimeSettings ReadValidatedSettings() {
    RuntimeSettings settings = {};
    settings.saveDelay = Wh_GetIntSetting(L"saveDelay");
    settings.minTimeBetweenSaves = Wh_GetIntSetting(L"minTimeBetweenSaves");

    if (settings.saveDelay < MIN_SAVE_DELAY_MS) {
        settings.saveDelay = MIN_SAVE_DELAY_MS;
    }
    if (settings.saveDelay > MAX_SAVE_DELAY_MS) {
        settings.saveDelay = MAX_SAVE_DELAY_MS;
    }
    if (settings.minTimeBetweenSaves < 0) {
        settings.minTimeBetweenSaves = 0;
    }
    if (settings.minTimeBetweenSaves > MAX_MIN_TIME_BETWEEN_SAVES) {
        settings.minTimeBetweenSaves = MAX_MIN_TIME_BETWEEN_SAVES;
    }

    return settings;
}

ULONGLONG PackRuntimeSettings(const RuntimeSettings& settings) {
    const ULONGLONG saveDelay =
        static_cast<ULONGLONG>(static_cast<DWORD>(settings.saveDelay));
    const ULONGLONG minSaveInterval =
        static_cast<ULONGLONG>(static_cast<DWORD>(settings.minTimeBetweenSaves));
    return saveDelay | (minSaveInterval << 32);
}

RuntimeSettings UnpackRuntimeSettings(ULONGLONG packedSettings) {
    RuntimeSettings settings = {};
    settings.saveDelay =
        static_cast<int>(static_cast<DWORD>(packedSettings & 0xFFFFFFFFull));
    settings.minTimeBetweenSaves = static_cast<int>(
        static_cast<DWORD>((packedSettings >> 32) & 0xFFFFFFFFull));
    return settings;
}

ULONG PublishRuntimeSettings(const RuntimeSettings& settings) {
    g_control.settingsSequence.fetch_add(1, std::memory_order_acq_rel);
    g_control.packedSettings.store(PackRuntimeSettings(settings),
                                   std::memory_order_relaxed);
    const ULONG stableSequence =
        g_control.settingsSequence.fetch_add(1, std::memory_order_release) + 1;
    g_control.pendingRequests.fetch_or(RuntimeControlApplySettings,
                                       std::memory_order_release);
    return stableSequence;
}

RuntimeSettings ReadPublishedRuntimeSettings(ULONG* stableSequence) {
    for (;;) {
        const ULONG before =
            g_control.settingsSequence.load(std::memory_order_acquire);
        if (before & 1) {
            SwitchToThread();
            continue;
        }

        const ULONGLONG packed =
            g_control.packedSettings.load(std::memory_order_relaxed);
        const ULONG after =
            g_control.settingsSequence.load(std::memory_order_acquire);
        if (before == after && !(after & 1)) {
            if (stableSequence) {
                *stableSequence = after;
            }
            return UnpackRuntimeSettings(packed);
        }
    }
}

void ApplyRuntimeSettings(const RuntimeSettings& settings) {
    g_settings = settings;
    Wh_Log(L"Settings: saveDelay=%d ms, minTimeBetweenSaves=%d ms",
           settings.saveDelay,
           settings.minTimeBetweenSaves);
}

void ApplyPublishedSettingsOnOwnerThread() {
    if (!IsOwnerThread() ||
        g_control.shutdownRequested.load(std::memory_order_acquire)) {
        return;
    }

    ULONG stableSequence = 0;
    const RuntimeSettings settings =
        ReadPublishedRuntimeSettings(&stableSequence);
    if (g_control.appliedSettingsSequence.load(std::memory_order_acquire) ==
        stableSequence) {
        return;
    }

    ApplyRuntimeSettings(settings);
    g_control.appliedSettingsSequence.store(stableSequence,
                                            std::memory_order_release);

    // Re-evaluate current work immediately. This preserves the tracked
    // document and Word event connection while correctly honoring both
    // shorter and longer delays.
    if (HasPendingAutosave()) {
        RescheduleSaveTimer(1);
    } else if (HasPendingSaveWork()) {
        ArmDocumentStateTimer(1);
    }
}

bool PostOwnerControlMessage() {
    HWND messageWindow =
        g_control.messageWindow.load(std::memory_order_acquire);
    return messageWindow &&
           IsWindow(messageWindow) &&
           PostMessageW(messageWindow,
                        WORD_LOCAL_AUTOSAVE_CONTROL_MESSAGE,
                        0,
                        0) != FALSE;
}

void QueueOwnerControlRequest(LONG request) {
    g_control.pendingRequests.fetch_or(request, std::memory_order_release);
    if (PostOwnerControlMessage()) {
        return;
    }

    if (!(request & RuntimeControlShutdown)) {
        // No owner exists yet. The already established bootstrap path will
        // adopt one; the pending request itself remains in the control plane.
        PostStartupBootstrapMessageToWordUi();
    }
}

void SignalOwnerShutdownComplete() {
    g_control.ownerRuntimeLive.store(false, std::memory_order_release);
    g_control.lifecycle.store(RuntimeControlStopped,
                              std::memory_order_release);

    LONG expectedOutcome = RuntimeShutdownPending;
    g_control.shutdownOutcome.compare_exchange_strong(
        expectedOutcome,
        RuntimeShutdownCompleted,
        std::memory_order_acq_rel,
        std::memory_order_acquire);

    HANDLE shutdownCompleteEvent = g_control.shutdownCompleteEvent;
    if (shutdownCompleteEvent) {
        SetEvent(shutdownCompleteEvent);
    }
    // Publish completion only after the owner has finished its final use of
    // the management-owned event handle.
    g_control.shutdownComplete.store(true, std::memory_order_release);
}

void ShutdownRuntimeOnOwnerThread() {
    if (!IsOwnerThread() ||
        g_runtime.ownerWorkDepth != 0 ||
        g_runtime.shutdownInProgress) {
        return;
    }

    g_runtime.shutdownInProgress = true;
    ClearFlag(g_runtime.flags.moduleActive);
    CancelSchedulerTimer();

    if (!TryDisconnectAllWordEventsForOwnerShutdown()) {
        // Keep every connection-point and sink reference in its owner STA.
        // The management thread will repost the pointer-free control wake;
        // only a completed/terminal Unadvise may advance to module unload.
        g_runtime.shutdownInProgress = false;
        g_control.pendingRequests.fetch_or(RuntimeControlShutdown,
                                           std::memory_order_release);
        return;
    }

    // ResetRuntimeState now releases event sessions and document COM state
    // before clearing the owner id, destroys the private window on this same
    // thread, and leaves no callback-based timer behind.
    ResetRuntimeState(RuntimeResetMode::Shutdown);
    SignalOwnerShutdownComplete();
}

void ProcessOwnerControlRequests() {
    if (!IsOwnerThread() ||
        g_runtime.ownerWorkDepth != 0 ||
        g_runtime.shutdownInProgress) {
        return;
    }

    const LONG requests =
        g_control.pendingRequests.exchange(0, std::memory_order_acq_rel);
    if (g_control.shutdownRequested.load(std::memory_order_acquire) ||
        (requests & RuntimeControlShutdown)) {
        ShutdownRuntimeOnOwnerThread();
        return;
    }

    if (requests & RuntimeControlApplySettings) {
        ApplyPublishedSettingsOnOwnerThread();
    }

    if ((requests & RuntimeControlRepairScheduler) &&
        !RefreshSchedulerTimer() &&
        GetNextScheduledTaskDueTime() != 0) {
        // Keep the repair request pending without posting an unbounded stream
        // of control messages if USER32 is temporarily unable to create a timer.
        // The next owner-side relevant message or control wake will retry it.
        g_control.pendingRequests.fetch_or(RuntimeControlRepairScheduler,
                                           std::memory_order_release);
    }
}

bool PinCurrentModuleForSafeShutdownFallback() {
    if (g_control.modulePinnedForSafety.load(std::memory_order_acquire)) {
        return true;
    }

    HMODULE module = nullptr;
    const auto moduleAddress =
        reinterpret_cast<LPCWSTR>(static_cast<const void*>(&g_control));
    if (!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                                GET_MODULE_HANDLE_EX_FLAG_PIN,
                            moduleAddress,
                            &module)) {
        Wh_Log(L"Shutdown: failed to pin the module for the safety fallback, error=%lu",
               GetLastError());
        return false;
    }

    g_control.modulePinnedForSafety.store(true, std::memory_order_release);
    Wh_Log(L"Shutdown: teardown could not be proven safe; the inactive module was pinned "
           L"until process exit to keep outstanding COM callbacks safe");
    return true;
}

void QuiesceWindowRuntimeForPinnedShutdown() {
    ClearFlag(g_runtime.flags.moduleActive);

    HWND messageWindow =
        g_control.messageWindow.load(std::memory_order_acquire);
    if (!messageWindow) {
        return;
    }

    // A window-associated timer can be cancelled by HWND/ID without touching
    // any COM state. WM_CLOSE is handled by the system STATIC window procedure
    // on its owner thread even after the TranslateMessage hook is removed.
    KillTimer(messageWindow, SCHEDULER_WINDOW_TIMER_ID);
    PostMessageW(messageWindow, WM_CLOSE, 0, 0);
}

bool TryClaimPinnedShutdownFallback() {
    LONG expectedOutcome = RuntimeShutdownPending;
    if (!g_control.shutdownOutcome.compare_exchange_strong(
            expectedOutcome,
            RuntimeShutdownFallbackClaimed,
            std::memory_order_acq_rel,
            std::memory_order_acquire)) {
        return expectedOutcome == RuntimeShutdownFallbackClaimed &&
               g_control.modulePinnedForSafety.load(std::memory_order_acquire);
    }

    QuiesceWindowRuntimeForPinnedShutdown();
    if (PinCurrentModuleForSafeShutdownFallback()) {
        return true;
    }

    if (g_control.shutdownComplete.load(std::memory_order_acquire)) {
        return false;
    }

    expectedOutcome = RuntimeShutdownFallbackClaimed;
    g_control.shutdownOutcome.compare_exchange_strong(
        expectedOutcome,
        RuntimeShutdownPending,
        std::memory_order_acq_rel,
        std::memory_order_acquire);
    return false;
}

bool RequestOwnerShutdownAndWait() {
    if (g_control.shutdownComplete.load(std::memory_order_acquire)) {
        return true;
    }

    g_control.shutdownRequested.store(true, std::memory_order_release);

    const LONG previousLifecycle =
        g_control.lifecycle.exchange(RuntimeControlStopRequested,
                                     std::memory_order_acq_rel);

    ClearFlag(g_runtime.flags.moduleActive);
    g_control.pendingRequests.fetch_or(RuntimeControlShutdown,
                                       std::memory_order_release);

    if (g_control.shutdownOutcome.load(std::memory_order_acquire) ==
            RuntimeShutdownFallbackClaimed &&
        g_control.modulePinnedForSafety.load(std::memory_order_acquire)) {
        return false;
    }

    if ((previousLifecycle == RuntimeControlAwaitingOwner ||
         previousLifecycle == RuntimeControlStopped) &&
        !g_control.ownerRuntimeLive.load(std::memory_order_acquire) &&
        LoadOwnerThreadId() == 0) {
        SignalOwnerShutdownComplete();
        return true;
    }

    HANDLE shutdownCompleteEvent = g_control.shutdownCompleteEvent;

    if (IsOwnerThread()) {
        ProcessOwnerControlRequests();
    } else {
        PostOwnerControlMessage();
    }

    const ULONGLONG waitStartTime = GetTickCount64();
    unsigned int timeoutCount = 0;
    for (;;) {
        const DWORD waitResult = shutdownCompleteEvent
                                     ? WaitForSingleObject(
                                           shutdownCompleteEvent,
                                           OWNER_SHUTDOWN_WAKE_INTERVAL_MS)
                                     : WAIT_TIMEOUT;
        if (waitResult == WAIT_OBJECT_0) {
            return true;
        }

        if (waitResult == WAIT_FAILED) {
            Wh_Log(L"Shutdown: waiting for the owner thread failed, error=%lu",
                   GetLastError());
        } else if (!shutdownCompleteEvent) {
            Sleep(OWNER_SHUTDOWN_WAKE_INTERVAL_MS);
        }

        // A modal Word loop may consume an earlier wake without calling
        // TranslateMessage. Reposting is idempotent and carries no pointer.
        PostOwnerControlMessage();
        ++timeoutCount;
        if (timeoutCount % 50 == 0) {
            Wh_Log(L"Shutdown: still waiting for owner-thread COM teardown");
        }

        const ULONGLONG now = GetTickCount64();
        const bool fallbackRequired =
            waitResult == WAIT_FAILED ||
            g_control.requiresPinnedShutdown.load(std::memory_order_acquire) ||
            (now >= waitStartTime &&
             now - waitStartTime >= OWNER_SHUTDOWN_FAILSAFE_TIMEOUT_MS);
        if (fallbackRequired) {
            if (g_control.shutdownComplete.load(std::memory_order_acquire) ||
                (shutdownCompleteEvent &&
                 WaitForSingleObject(shutdownCompleteEvent, 0) == WAIT_OBJECT_0)) {
                return true;
            }

            if (TryClaimPinnedShutdownFallback()) {
                return false;
            }
        }
    }
}

BOOL Wh_ModInit() {
    Wh_Log(L"Word Local AutoSave v4.0.0 initializing...");

    const bool residentPinned =
        g_control.modulePinnedForSafety.load(std::memory_order_acquire);
    if (residentPinned &&
        !g_control.shutdownComplete.load(std::memory_order_acquire)) {
        Wh_Log(L"ERROR: Refusing to reinitialize while the previous pinned runtime "
               L"is still active; restart Word first");
        return FALSE;
    }

    if (g_control.shutdownCompleteEvent) {
        CloseHandle(g_control.shutdownCompleteEvent);
        g_control.shutdownCompleteEvent = nullptr;
    }

    g_control.pendingRequests.store(0, std::memory_order_relaxed);
    g_control.lifecycle.store(RuntimeControlAwaitingOwner,
                              std::memory_order_relaxed);
    g_control.settingsSequence.store(0, std::memory_order_relaxed);
    g_control.appliedSettingsSequence.store(0, std::memory_order_relaxed);
    g_control.packedSettings.store(0, std::memory_order_relaxed);
    g_control.messageWindow.store(nullptr, std::memory_order_relaxed);
    g_control.ownerRuntimeLive.store(false, std::memory_order_relaxed);
    g_control.shutdownRequested.store(false, std::memory_order_relaxed);
    g_control.shutdownComplete.store(false, std::memory_order_relaxed);
    g_control.shutdownOutcome.store(RuntimeShutdownPending,
                                    std::memory_order_relaxed);
    g_control.requiresPinnedShutdown.store(false,
                                           std::memory_order_relaxed);

    g_runtime.wordProcessId = GetCurrentProcessId();
    ResetRuntimeState(RuntimeResetMode::Shutdown);
#ifdef WH_STANDALONE_COMPILE_CHECK
    const bool internalSelfTestsPassed = RunInternalSelfTests();
    ResetRuntimeState(RuntimeResetMode::Shutdown);
#endif
    g_runtime.shutdownInProgress = false;

    const RuntimeSettings initialSettings = ReadValidatedSettings();
    const ULONG initialSettingsSequence =
        PublishRuntimeSettings(initialSettings);
    ApplyRuntimeSettings(initialSettings);
    g_control.appliedSettingsSequence.store(initialSettingsSequence,
                                            std::memory_order_relaxed);
    g_control.pendingRequests.fetch_and(~RuntimeControlApplySettings,
                                        std::memory_order_relaxed);
#ifdef WH_STANDALONE_COMPILE_CHECK
    if (!internalSelfTestsPassed) {
        Wh_Log(L"WARNING: One or more internal self-tests failed");
    }
#endif

    ScopedHandle shutdownCompleteEvent(
        CreateEventW(nullptr, TRUE, FALSE, nullptr));
    if (!shutdownCompleteEvent.IsValid()) {
        Wh_Log(L"ERROR: Failed to create owner shutdown event, error=%lu",
               GetLastError());
        return FALSE;
    }

    if (EnsureStartupBootstrapMessage() == 0) {
        Wh_Log(L"ERROR: Failed to register startup bootstrap message");
        return FALSE;
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

    g_control.shutdownCompleteEvent = shutdownCompleteEvent.Detach();
    SetFlag(g_runtime.flags.moduleActive);
    PostStartupBootstrapMessageToWordUi();

    Wh_Log(L"Word Local AutoSave initialized");
    return TRUE;
}

void Wh_ModBeforeUninit() {
    Wh_Log(L"Word Local AutoSave preparing owner-thread teardown...");
    if (!RequestOwnerShutdownAndWait()) {
        Wh_Log(L"ERROR: Owner-thread teardown handshake failed");
    }
}

void Wh_ModUninit() {
    Wh_Log(L"Word Local AutoSave uninitializing...");

    ClearFlag(g_runtime.flags.moduleActive);
    if (!g_control.shutdownComplete.load(std::memory_order_acquire) &&
        !RequestOwnerShutdownAndWait()) {
        Wh_Log(L"ERROR: Owner-thread runtime was not fully torn down");
    }

    if (g_control.shutdownCompleteEvent &&
        (g_control.shutdownComplete.load(std::memory_order_acquire) ||
         !g_control.modulePinnedForSafety.load(std::memory_order_acquire))) {
        CloseHandle(g_control.shutdownCompleteEvent);
        g_control.shutdownCompleteEvent = nullptr;
    }

    Wh_Log(L"Word Local AutoSave uninitialized");
}

void Wh_ModSettingsChanged() {
    if (g_control.shutdownRequested.load(std::memory_order_acquire)) {
        return;
    }

    Wh_Log(L"Settings changed, publishing to the owner thread...");
    PublishRuntimeSettings(ReadValidatedSettings());
    QueueOwnerControlRequest(RuntimeControlApplySettings);
}
