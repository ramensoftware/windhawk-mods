// ==WindhawkMod==
// @id              word-local-autosave
// @name            Word Local AutoSave
// @description     Enables AutoSave functionality for local documents in Microsoft Word via direct Word saves
// @version         3.4
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

## Shortcut Safety (v3.4)

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

#include <windows.h>
#include <ocidl.h>
#include <oleauto.h>
#include <oleacc.h>
#include <new>
#include <utility>

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

// ============================================================================
// Global State
// ============================================================================

struct {
    int saveDelay;
    int minTimeBetweenSaves;
} g_settings;

TranslateMessage_t g_originalTranslateMessage = nullptr;
DWORD g_wordProcessId = 0;
DWORD g_ownerThreadId = 0;
UINT_PTR g_saveTimerId = 0;
UINT_PTR g_documentStateTimerId = 0;
ULONGLONG g_saveTimerDueTime = 0;
ULONGLONG g_documentStateTimerDueTime = 0;
ULONGLONG g_lastEditTime = 0;
ULONGLONG g_lastSaveTime = 0;
ULONGLONG g_lastEventConnectAttemptTime = 0;
ULONGLONG g_pendingSaveAsTime = 0;
BSTR g_observedDocumentPath = nullptr;
BSTR g_transitionFlushDocumentPath = nullptr;
BSTR g_lastSaveStatusMessage = nullptr;
BSTR g_lastDocumentStateStatusMessage = nullptr;
DWORD g_saveRetryDelayMs = MIN_RETRY_INTERVAL_MS;
DWORD g_documentStateRetryDelayMs = MIN_RETRY_INTERVAL_MS;
ULONGLONG g_lastSaveFailureLogTime = 0;
ULONGLONG g_lastDocumentStateFailureLogTime = 0;
ULONGLONG g_lastSaveStatusLogTime = 0;
ULONGLONG g_lastDocumentStateStatusLogTime = 0;
LONG_PTR g_connectedWordApplicationHwnd = 0;
IDispatch* g_connectedWordApplication = nullptr;
IConnectionPoint* g_wordApplicationEventConnectionPoint = nullptr;
IDispatch* g_wordApplicationEventSink = nullptr;
IDispatch* g_transitionFlushDocument = nullptr;
IUnknown* g_observedDocumentIdentity = nullptr;
IUnknown* g_pendingSaveAsDocumentIdentity = nullptr;
IUnknown* g_transitionFlushDocumentIdentity = nullptr;
DWORD g_wordApplicationEventCookie = 0;
WordEventDispIds g_wordEventDispIds;
HWND g_cachedWordRootWindow = nullptr;
HWND g_cachedNativeWordWindow = nullptr;
DWORD g_cachedWordUiThreadId = 0;
volatile LONG g_pendingSave = FALSE;
volatile LONG g_documentDirtyKnown = FALSE;
volatile LONG g_documentDirty = FALSE;
volatile LONG g_manualSavePending = FALSE;
volatile LONG g_expeditedSavePending = FALSE;
volatile LONG g_transitionFlushPending = FALSE;
volatile LONG g_imeComposing = FALSE;
volatile LONG g_automationBusyPending = FALSE;
volatile LONG g_pendingSaveAsMigration = FALSE;
volatile LONG g_wordEventsConnected = FALSE;
volatile LONG g_moduleActive = FALSE;

// ============================================================================
// Utility Helpers
// ============================================================================

bool IsQueueKeyDown(int vk) {
    return (GetKeyState(vk) & 0x8000) != 0;
}

bool IsAsyncKeyDown(int vk) {
    return (GetAsyncKeyState(vk) & 0x8000) != 0;
}

bool IsOwnerThread() {
    return g_ownerThreadId != 0 && GetCurrentThreadId() == g_ownerThreadId;
}

void ReleaseInterface(IUnknown** value) {
    if (value && *value) {
        (*value)->Release();
        *value = nullptr;
    }
}

bool ReplaceStoredUnknown(IUnknown** storage, IUnknown* value) {
    if (!storage) {
        return false;
    }

    if (*storage == value) {
        return true;
    }

    if (value) {
        value->AddRef();
    }

    ReleaseInterface(storage);
    *storage = value;
    return true;
}

bool ReplaceStoredDispatch(IDispatch** storage, IDispatch* value) {
    return ReplaceStoredUnknown(reinterpret_cast<IUnknown**>(storage),
                                reinterpret_cast<IUnknown*>(value));
}

bool AreSameDocumentPath(const wchar_t* left, const wchar_t* right);
bool ReplaceStoredBstr(BSTR* storage, const wchar_t* value);
void LogSaveStatus(const wchar_t* message);
void LogDocumentStateStatus(const wchar_t* message);
bool AreModifiersOrMouseButtonsHeld();
HRESULT GetComIdentity(IUnknown* unknown, IUnknown** result);
bool IsWindowInCurrentWordProcess(HWND hwnd);

enum class MessageAutosaveRole {
    None,
    EditingInput,
    DocumentRefreshFallback,
    ActionBoundaryFallback,
    TransitionBoundaryFallback,
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
        InterlockedExchange(&g_imeComposing, TRUE);
    } else if (lpMsg->message == WM_IME_ENDCOMPOSITION) {
        InterlockedExchange(&g_imeComposing, FALSE);
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

void ClearStoredStatusMessage(BSTR* storage, ULONGLONG* lastLogTime) {
    if (storage && *storage) {
        SysFreeString(*storage);
        *storage = nullptr;
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
    return ReplaceStoredBstr(&g_observedDocumentPath, path);
}

bool SetTransitionFlushDocumentPath(const wchar_t* path) {
    return ReplaceStoredBstr(&g_transitionFlushDocumentPath, path);
}

bool SetTransitionFlushDocument(IDispatch* document) {
    if (!ReplaceStoredDispatch(&g_transitionFlushDocument, document)) {
        return false;
    }

    if (!document) {
        ReleaseInterface(&g_transitionFlushDocumentIdentity);
        return true;
    }

    IUnknown* identity = nullptr;
    if (FAILED(GetComIdentity(document, &identity)) || !identity) {
        ReplaceStoredDispatch(&g_transitionFlushDocument, nullptr);
        ReleaseInterface(&g_transitionFlushDocumentIdentity);
        return false;
    }

    const bool replaced = ReplaceStoredUnknown(&g_transitionFlushDocumentIdentity, identity);
    identity->Release();
    return replaced;
}

void ClearManualSavePending() {
    InterlockedExchange(&g_manualSavePending, FALSE);
}

void ClearExpeditedSavePending() {
    InterlockedExchange(&g_expeditedSavePending, FALSE);
}

void ClearTransitionFlushRequest() {
    InterlockedExchange(&g_transitionFlushPending, FALSE);
    if (g_transitionFlushDocumentPath) {
        SysFreeString(g_transitionFlushDocumentPath);
        g_transitionFlushDocumentPath = nullptr;
    }

    ReplaceStoredDispatch(&g_transitionFlushDocument, nullptr);
    ReleaseInterface(&g_transitionFlushDocumentIdentity);
}

bool HasPendingSaveWork() {
    return InterlockedCompareExchange(&g_pendingSave, TRUE, TRUE) == TRUE ||
           InterlockedCompareExchange(&g_manualSavePending, TRUE, TRUE) == TRUE ||
           InterlockedCompareExchange(&g_documentDirty, TRUE, TRUE) == TRUE;
}

bool HasPendingAutosave() {
    return InterlockedCompareExchange(&g_pendingSave, TRUE, TRUE) == TRUE;
}

void ClearAutomationBusyPending() {
    InterlockedExchange(&g_automationBusyPending, FALSE);
}

void NoteAutomationBusy(const wchar_t* message) {
    InterlockedExchange(&g_automationBusyPending, TRUE);
    if (message) {
        LogSaveStatus(message);
    }
}

void ClearPendingSaveAsMigration() {
    InterlockedExchange(&g_pendingSaveAsMigration, FALSE);
    g_pendingSaveAsTime = 0;
    ReleaseInterface(&g_pendingSaveAsDocumentIdentity);
}

bool HasPendingSaveAsMigration() {
    return InterlockedCompareExchange(&g_pendingSaveAsMigration, TRUE, TRUE) == TRUE;
}

bool AreWordEventsConnected() {
    return InterlockedCompareExchange(&g_wordEventsConnected, TRUE, TRUE) == TRUE;
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
    if (g_observedDocumentPath) {
        SysFreeString(g_observedDocumentPath);
        g_observedDocumentPath = nullptr;
    }

    ReleaseInterface(&g_observedDocumentIdentity);

    InterlockedExchange(&g_documentDirtyKnown, FALSE);
    InterlockedExchange(&g_documentDirty, FALSE);
}

void RequestTransitionFlush(const wchar_t* path,
                           const wchar_t* reason,
                           IDispatch* document = nullptr) {
    InterlockedExchange(&g_transitionFlushPending, TRUE);
    if (!SetTransitionFlushDocumentPath(path) ||
        (document && !SetTransitionFlushDocument(document))) {
        ClearTransitionFlushRequest();
        InterlockedExchange(&g_transitionFlushPending, TRUE);
        if (!SetTransitionFlushDocumentPath(path)) {
            return;
        }
    } else if (!document) {
        SetTransitionFlushDocument(nullptr);
    }

    InterlockedExchange(&g_expeditedSavePending, TRUE);
    if (reason) {
        LogSaveStatus(reason);
    }
}

void InvalidateWordUiWindowCache() {
    g_cachedWordRootWindow = nullptr;
    g_cachedNativeWordWindow = nullptr;
    g_cachedWordUiThreadId = 0;
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

    g_cachedWordRootWindow = rootWindow;
    g_cachedNativeWordWindow =
        IsUsableCachedNativeWordWindow(nativeWordWindow, rootWindow) ? nativeWordWindow : nullptr;
    if (g_cachedNativeWordWindow) {
        g_cachedWordUiThreadId = GetWindowThreadProcessId(g_cachedNativeWordWindow, nullptr);
    } else {
        g_cachedWordUiThreadId = threadId ? threadId : GetWindowThreadProcessId(rootWindow, nullptr);
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

    if (!IsUsableWordRootWindow(g_cachedWordRootWindow)) {
        InvalidateWordUiWindowCache();
        return false;
    }

    if (g_cachedWordRootWindow != rootWindow || g_cachedWordUiThreadId == 0) {
        return false;
    }

    if (g_cachedNativeWordWindow &&
        !IsUsableCachedNativeWordWindow(g_cachedNativeWordWindow, g_cachedWordRootWindow)) {
        g_cachedNativeWordWindow = nullptr;
        g_cachedWordUiThreadId = GetWindowThreadProcessId(g_cachedWordRootWindow, nullptr);
    }

    *threadId = g_cachedWordUiThreadId;
    return true;
}

bool IsWindowInCurrentWordProcess(HWND hwnd) {
    if (!hwnd) {
        return false;
    }

    DWORD processId = 0;
    GetWindowThreadProcessId(hwnd, &processId);
    return processId == g_wordProcessId;
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
        IsUsableCachedNativeWordWindow(g_cachedNativeWordWindow, rootWindow)) {
        return g_cachedNativeWordWindow;
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

    if (IsUsableWordRootWindow(g_cachedWordRootWindow)) {
        return g_cachedWordRootWindow;
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
        if (IsUsableCachedNativeWordWindow(g_cachedNativeWordWindow, preferredRoot)) {
            return g_cachedNativeWordWindow;
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

    if (IsUsableCachedNativeWordWindow(g_cachedNativeWordWindow, rootWindow)) {
        return g_cachedNativeWordWindow;
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

bool IsActiveWordDocumentWindow() {
    return GetActiveWordUiThreadId() != 0;
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
    if (ShouldLogStatusMessageNow(&g_lastSaveStatusMessage, &g_lastSaveStatusLogTime, message)) {
        Wh_Log(L"Auto-save: %ls", message);
    }
}

void LogDocumentStateStatus(const wchar_t* message) {
    if (ShouldLogStatusMessageNow(&g_lastDocumentStateStatusMessage,
                                  &g_lastDocumentStateStatusLogTime,
                                  message)) {
        Wh_Log(L"Document state monitor: %ls", message);
    }
}

WordUiPauseReason GetWordUiPauseReason() {
    if (InterlockedCompareExchange(&g_imeComposing, TRUE, TRUE) == TRUE) {
        return WordUiPauseReason::ImeComposition;
    }

    if (g_ownerThreadId != 0) {
        GUITHREADINFO guiThreadInfo = {};
        guiThreadInfo.cbSize = sizeof(guiThreadInfo);
        if (GetGUIThreadInfo(g_ownerThreadId, &guiThreadInfo)) {
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
    if (foregroundProcessId != g_wordProcessId) {
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
    bool ownerThread = false;
    bool ownerThreadSynchronized = false;
    bool activeWordDocumentWindow = false;
    bool inputBusy = false;
    bool pendingSave = false;
    bool manualSavePending = false;
    bool transitionFlushRequested = false;
    bool expeditedSaveRequested = false;
    bool wordEventsConnected = false;
};

TickRuntimeObservation ObserveTickRuntime(const MSG* lpMsg = nullptr) {
    TickRuntimeObservation observation = {};
    observation.preferredOwnerThreadId = ResolvePreferredOwnerThreadId(lpMsg);
    observation.ownerThread = IsOwnerThread();
    observation.ownerThreadSynchronized =
        observation.ownerThread &&
        (observation.preferredOwnerThreadId == 0 ||
         g_ownerThreadId == observation.preferredOwnerThreadId);
    observation.activeWordDocumentWindow = observation.preferredOwnerThreadId != 0;
    observation.inputBusy = GetInputState() || AreModifiersOrMouseButtonsHeld();
    observation.pendingSave = HasPendingAutosave();
    observation.manualSavePending =
        InterlockedCompareExchange(&g_manualSavePending, TRUE, TRUE) == TRUE;
    observation.transitionFlushRequested =
        InterlockedCompareExchange(&g_transitionFlushPending, TRUE, TRUE) == TRUE;
    observation.expeditedSaveRequested =
        InterlockedCompareExchange(&g_expeditedSavePending, TRUE, TRUE) == TRUE;
    observation.wordEventsConnected = AreWordEventsConnected();
    observation.pauseReason = GetWordUiPauseReason();
    return observation;
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
                IsUsableCachedNativeWordWindow(g_cachedNativeWordWindow, rootWindow)
                    ? g_cachedNativeWordWindow
                    : nullptr;
            CacheWordUiWindowContext(rootWindow, nativeWordWindow, currentThreadId);
        }
    }

    const DWORD previousOwnerThreadId = InterlockedExchange(
        reinterpret_cast<volatile LONG*>(&g_ownerThreadId),
        static_cast<LONG>(currentThreadId));
    if (previousOwnerThreadId != currentThreadId) {
        g_saveTimerId = 0;
        g_documentStateTimerId = 0;
        g_saveTimerDueTime = 0;
        g_documentStateTimerDueTime = 0;
        g_saveRetryDelayMs = MIN_RETRY_INTERVAL_MS;
        g_documentStateRetryDelayMs = MIN_RETRY_INTERVAL_MS;
        EnsureWordApplicationEventsConnected(true);
        ArmDocumentStateTimer(INPUT_SETTLE_DELAY_MS);
        if (InterlockedCompareExchange(&g_pendingSave, TRUE, TRUE) == TRUE) {
            ArmSaveTimer(INPUT_SETTLE_DELAY_MS);
        }
    }
}

void CancelSaveTimer() {
    if (g_saveTimerId != 0) {
        KillTimer(nullptr, g_saveTimerId);
        g_saveTimerId = 0;
    }

    g_saveTimerDueTime = 0;
}

void CancelDocumentStateTimer() {
    if (g_documentStateTimerId != 0) {
        KillTimer(nullptr, g_documentStateTimerId);
        g_documentStateTimerId = 0;
    }

    g_documentStateTimerDueTime = 0;
}

void HandleAutosaveTick();
void HandleDocumentStateTick();
void CALLBACK DocumentStateTimerProc(HWND, UINT, UINT_PTR, DWORD);
void ExpirePendingSaveAsMigrationIfNeeded();

DWORD GetSteadyDocumentStatePollDelay() {
    if (InterlockedCompareExchange(&g_pendingSave, TRUE, TRUE) == TRUE ||
        InterlockedCompareExchange(&g_documentDirty, TRUE, TRUE) == TRUE ||
        InterlockedCompareExchange(&g_manualSavePending, TRUE, TRUE) == TRUE ||
        InterlockedCompareExchange(&g_automationBusyPending, TRUE, TRUE) == TRUE ||
        HasPendingSaveAsMigration()) {
        return DOCUMENT_STATE_ACTIVE_POLL_INTERVAL_MS;
    }

    if (AreWordEventsConnected()) {
        return DOCUMENT_STATE_EVENT_IDLE_POLL_INTERVAL_MS;
    }

    return DOCUMENT_STATE_IDLE_POLL_INTERVAL_MS;
}

DWORD GetBoundaryCoalesceDelay(bool transitionFlush) {
    return ComputeBoundaryCoalesceDelay(transitionFlush, g_lastEditTime, GetTickCount64());
}

void RequestPendingSaveExpedite(bool transitionFlush,
                                const wchar_t* reason,
                                DWORD delayMs = INPUT_SETTLE_DELAY_MS) {
    if (transitionFlush) {
        RequestTransitionFlush(g_observedDocumentPath, reason);
    } else {
        InterlockedExchange(&g_expeditedSavePending, TRUE);
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

void MaybeKickAutomationRecovery() {
    if (InterlockedCompareExchange(&g_automationBusyPending, TRUE, TRUE) == FALSE) {
        return;
    }

    if (GetInputState() || AreModifiersOrMouseButtonsHeld()) {
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

bool ArmDocumentStateTimer(DWORD delayMs) {
    if (!IsOwnerThread()) {
        return false;
    }

    const DWORD effectiveDelayMs = delayMs ? delayMs : 1;
    const ULONGLONG dueTime = GetTickCount64() + effectiveDelayMs;
    if (g_documentStateTimerId != 0 &&
        g_documentStateTimerDueTime != 0 &&
        g_documentStateTimerDueTime <= dueTime) {
        return true;
    }

    CancelDocumentStateTimer();
    g_documentStateTimerId =
        SetTimer(nullptr, 0, effectiveDelayMs, DocumentStateTimerProc);
    if (g_documentStateTimerId == 0) {
        Wh_Log(L"Document state monitor: SetTimer failed, error=%lu", GetLastError());
        return false;
    }

    g_documentStateTimerDueTime = dueTime;
    return true;
}

void ScheduleSaveFromEdit() {
    ClearTransitionFlushRequest();
    ClearExpeditedSavePending();
    ClearAutomationBusyPending();
    g_lastEditTime = GetTickCount64();
    InterlockedExchange(&g_pendingSave, TRUE);
    ArmSaveTimer(static_cast<DWORD>(g_settings.saveDelay));
}

void ClearPendingSave() {
    InterlockedExchange(&g_pendingSave, FALSE);
    ClearExpeditedSavePending();
}

void HandleManualSave() {
    ClearTransitionFlushRequest();
    InterlockedExchange(&g_manualSavePending, TRUE);
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
    HRESULT hr = dispatch->GetIDsOfNames(kIIDNull, &name, 1, LOCALE_USER_DEFAULT, &dispatchId);
    if (FAILED(hr)) {
        return hr;
    }

    DISPPARAMS params = {};
    params.cArgs = argCount;
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
        hr = value.Get()->punkVal->QueryInterface(IID_PPV_ARGS(result));
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
        return value.Get()->punkVal->QueryInterface(IID_PPV_ARGS(result));
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
    return unknown->QueryInterface(IID_PPV_ARGS(result));
}

bool AreSameComIdentity(IUnknown* left, IUnknown* right) {
    return left && right && left == right;
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

    DWORD processId = 0;
    GetWindowThreadProcessId(reinterpret_cast<HWND>(localHwndValue), &processId);
    if (processId != g_wordProcessId) {
        return false;
    }

    if (hwndValue) {
        *hwndValue = localHwndValue;
    }

    return true;
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

HRESULT GetDocumentIdentity(IDispatch* document, BSTR* result) {
    if (!document || !result) {
        return E_POINTER;
    }

    *result = nullptr;

    ScopedBstr fullName;
    HRESULT hr = GetBstrProperty(document, L"FullName", fullName.Put());
    if (SUCCEEDED(hr) && fullName.Length() > 0) {
        *result = fullName.Detach();
        return S_OK;
    }

    ScopedBstr path;
    hr = GetBstrProperty(document, L"Path", path.Put());
    if (FAILED(hr)) {
        return hr;
    }

    if (path.Length() == 0) {
        return S_FALSE;
    }

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

    hr = unknown->QueryInterface(IID_PPV_ARGS(application));
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

    if (!g_connectedWordApplication) {
        return E_FAIL;
    }

    if (!DoesApplicationBelongToCurrentProcess(g_connectedWordApplication)) {
        return E_FAIL;
    }

    g_connectedWordApplication->AddRef();
    *application = g_connectedWordApplication;
    return S_OK;
}

HRESULT GetWordApplication(IDispatch** application) {
    HRESULT hr = GetWordApplicationFromActiveWindow(application);
    if (SUCCEEDED(hr) && application && *application) {
        return hr;
    }

    hr = GetWordApplicationFromConnectedInstance(application);
    if (SUCCEEDED(hr) && application && *application) {
        return hr;
    }

    return GetWordApplicationFromRot(application);
}

HRESULT GetWordDocumentByPath(const wchar_t* path, IDispatch** document) {
    if (!path || !*path || !document) {
        return E_INVALIDARG;
    }

    *document = nullptr;

    ScopedComPtr<IDispatch> application;
    HRESULT hr = GetWordApplicationFromConnectedInstance(application.Put());
    if (FAILED(hr) || !application) {
        hr = GetWordApplication(application.Put());
    }
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
        return value->punkVal->QueryInterface(IID_PPV_ARGS(result));
    }

    if (value->vt == (VT_UNKNOWN | VT_BYREF) && value->ppunkVal && *value->ppunkVal) {
        return (*value->ppunkVal)->QueryInterface(IID_PPV_ARGS(result));
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
    InterlockedExchange(&g_manualSavePending, TRUE);
    CancelSaveTimer();
    ArmDocumentStateTimer(INPUT_SETTLE_DELAY_MS);

    if (!saveAsUi || !document) {
        return;
    }

    ScopedComPtr<IUnknown> documentIdentity;
    if (SUCCEEDED(GetComIdentity(document.Get(), documentIdentity.Put())) && documentIdentity) {
        ReplaceStoredUnknown(&g_pendingSaveAsDocumentIdentity, documentIdentity.Get());
    }

    g_pendingSaveAsTime = GetTickCount64();
    InterlockedExchange(&g_pendingSaveAsMigration, TRUE);
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
        RequestTransitionFlush(g_observedDocumentPath,
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
        RequestTransitionFlush(g_observedDocumentPath,
                               L"finishing pending changes before Word leaves the current document window");
    }

    if (HasPendingAutosave()) {
        ArmSaveTimer(INPUT_SETTLE_DELAY_MS);
    } else {
        ArmDocumentStateTimer(INPUT_SETTLE_DELAY_MS);
    }
}

class WordApplicationEventSink final : public IDispatch {
public:
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
        if (dispIdMember == g_wordEventDispIds.documentBeforeSave) {
            HandleWordDocumentBeforeSaveEvent(params);
        } else if (dispIdMember == g_wordEventDispIds.documentBeforeClose) {
            HandleWordDocumentBeforeCloseEvent(params);
        } else if (dispIdMember == g_wordEventDispIds.documentChange) {
            HandleWordDocumentChangeEvent();
        } else if (dispIdMember == g_wordEventDispIds.windowDeactivate) {
            HandleWordWindowDeactivateEvent(params);
        }

        return S_OK;
    }

private:
    volatile LONG m_refCount = 1;
};

void DisconnectWordApplicationEvents() {
    ScopedComInit comInit;
    if (SUCCEEDED(comInit.GetResult()) &&
        g_wordApplicationEventConnectionPoint &&
        g_wordApplicationEventCookie != 0) {
        g_wordApplicationEventConnectionPoint->Unadvise(g_wordApplicationEventCookie);
    }

    g_wordApplicationEventCookie = 0;
    if (g_wordApplicationEventConnectionPoint) {
        g_wordApplicationEventConnectionPoint->Release();
        g_wordApplicationEventConnectionPoint = nullptr;
    }

    if (g_wordApplicationEventSink) {
        g_wordApplicationEventSink->Release();
        g_wordApplicationEventSink = nullptr;
    }

    ReplaceStoredDispatch(&g_connectedWordApplication, nullptr);
    g_connectedWordApplicationHwnd = 0;
    g_wordEventDispIds.Reset();
    InterlockedExchange(&g_wordEventsConnected, FALSE);
}

HRESULT ConnectWordApplicationEvents(IDispatch* application) {
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
    hr = application->QueryInterface(IID_PPV_ARGS(connectionPointContainer.Put()));
    if (FAILED(hr) || !connectionPointContainer) {
        return FAILED(hr) ? hr : E_FAIL;
    }

    ScopedComPtr<IConnectionPoint> connectionPoint;
    hr = connectionPointContainer->FindConnectionPoint(kDIIDWordApplicationEvents4,
                                                       connectionPoint.Put());
    if (FAILED(hr) || !connectionPoint) {
        return FAILED(hr) ? hr : E_FAIL;
    }

    WordApplicationEventSink* sink = new (std::nothrow) WordApplicationEventSink();
    if (!sink) {
        return E_OUTOFMEMORY;
    }

    DWORD cookie = 0;
    hr = connectionPoint->Advise(static_cast<IUnknown*>(sink), &cookie);
    if (FAILED(hr) || cookie == 0) {
        sink->Release();
        return FAILED(hr) ? hr : E_FAIL;
    }

    DisconnectWordApplicationEvents();
    ReplaceStoredDispatch(&g_connectedWordApplication, application);
    g_connectedWordApplicationHwnd = applicationHwnd;
    g_wordApplicationEventConnectionPoint = connectionPoint.Detach();
    g_wordApplicationEventSink = sink;
    g_wordApplicationEventCookie = cookie;
    g_wordEventDispIds = dispIds;
    InterlockedExchange(&g_wordEventsConnected, TRUE);
    return S_OK;
}

bool EnsureWordApplicationEventsConnected(bool forceReconnect) {
    if (!IsOwnerThread()) {
        return false;
    }

    const ULONGLONG now = GetTickCount64();
    if (!forceReconnect &&
        g_lastEventConnectAttemptTime != 0 &&
        now - g_lastEventConnectAttemptTime < WORD_EVENT_RECONNECT_INTERVAL_MS &&
        InterlockedCompareExchange(&g_wordEventsConnected, TRUE, TRUE) == FALSE) {
        return false;
    }

    g_lastEventConnectAttemptTime = now;

    ScopedComInit comInit;
    if (FAILED(comInit.GetResult())) {
        return false;
    }

    ScopedComPtr<IDispatch> application;
    HRESULT hr = GetWordApplication(application.Put());
    if (FAILED(hr) || !application) {
        if (forceReconnect) {
            DisconnectWordApplicationEvents();
        }
        return false;
    }

    LONG_PTR applicationHwnd = 0;
    if (!DoesApplicationBelongToCurrentProcess(application.Get(), &applicationHwnd)) {
        return false;
    }

    if (!forceReconnect &&
        InterlockedCompareExchange(&g_wordEventsConnected, TRUE, TRUE) == TRUE &&
        g_connectedWordApplicationHwnd == applicationHwnd) {
        return true;
    }

    hr = ConnectWordApplicationEvents(application.Get());
    if (FAILED(hr)) {
        DisconnectWordApplicationEvents();
        if (ShouldLogFailureNow(&g_lastDocumentStateFailureLogTime)) {
            Wh_Log(L"Document state monitor: failed to connect Word application events, hr=0x%08X",
                   hr);
        }
        return false;
    }

    LogDocumentStateStatus(L"connected to native Word application events");
    return true;
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
                                 ? &g_lastSaveFailureLogTime
                                 : &g_lastDocumentStateFailureLogTime;
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

    ScopedBstr savedLocation;
    hr = GetBstrProperty(snapshot->document.Get(), L"Path", savedLocation.Put());
    if (FAILED(hr)) {
        LogSnapshotFailure(context, L"failed to query Path", hr);
        return SnapshotQueryResult::RetryLater;
    }

    snapshot->hasPath = savedLocation.Length() > 0;
    if (!snapshot->hasPath) {
        snapshot->clearedReason = SnapshotClearedReason::NoPath;
        return SnapshotQueryResult::Cleared;
    }

    hr = GetDocumentIdentity(snapshot->document.Get(), snapshot->path.Put());
    if (FAILED(hr) || snapshot->path.Length() == 0) {
        LogSnapshotFailure(context, L"failed to query FullName/Name identity", hr);
        return SnapshotQueryResult::RetryLater;
    }

    return SnapshotQueryResult::Ready;
}

SnapshotQueryResult PopulateSnapshotState(ActiveDocumentSnapshot* snapshot,
                                         SnapshotLogContext context) {
    const SnapshotQueryResult savedStateResult = PopulateSnapshotSavedState(snapshot, context);
    if (savedStateResult != SnapshotQueryResult::Ready) {
        return savedStateResult;
    }

    return PopulateSnapshotMetadata(snapshot, context);
}

SnapshotQueryResult ResolveSnapshotDocument(const SnapshotRequest& request,
                                           ActiveDocumentSnapshot* snapshot) {
    if (!snapshot) {
        return SnapshotQueryResult::Cleared;
    }

    snapshot->Reset();

    if (request.source == SnapshotSource::SpecificPath) {
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

    ScopedComPtr<IDispatch> application;
    HRESULT hr = GetWordDocumentFromActiveWindow(snapshot->document.Put());
    if ((FAILED(hr) || !snapshot->document) && !IsRetryableAutomationFailure(hr)) {
        hr = GetWordApplication(application.Put());
        if (FAILED(hr) || !application) {
            if (IsRetryableAutomationFailure(hr)) {
                return SnapshotQueryResult::RetryLater;
            }

            LogSnapshotFailure(request.context, L"failed to get Word application", hr);
            return SnapshotQueryResult::RetryLater;
        }

        hr = GetDispatchProperty(application.Get(), L"ActiveDocument", snapshot->document.Put());
    }

    if (FAILED(hr) || !snapshot->document) {
        if (IsRetryableAutomationFailure(hr)) {
            return SnapshotQueryResult::RetryLater;
        }

        if (!application) {
            hr = GetWordApplication(application.Put());
        }

        if (SUCCEEDED(hr) && application) {
            ScopedComPtr<IDispatch> protectedViewWindow;
            if (SUCCEEDED(GetDispatchProperty(application.Get(),
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

    return SnapshotQueryResult::Ready;
}

SnapshotQueryResult LoadActiveDocumentSnapshot(ActiveDocumentSnapshot* snapshot,
                                               SnapshotLogContext context) {
    const SnapshotRequest request = {SnapshotSource::Active, nullptr, context};
    const SnapshotQueryResult documentResult = ResolveSnapshotDocument(request, snapshot);
    if (documentResult != SnapshotQueryResult::Ready) {
        return documentResult;
    }

    return PopulateSnapshotState(snapshot, context);
}

SnapshotQueryResult LoadDocumentSnapshotFromDispatch(IDispatch* document,
                                                     ActiveDocumentSnapshot* snapshot,
                                                     SnapshotLogContext context) {
    if (!document || !snapshot) {
        return SnapshotQueryResult::Cleared;
    }

    snapshot->Reset();
    document->AddRef();
    snapshot->document.Reset(document);
    return PopulateSnapshotState(snapshot, context);
}

SnapshotQueryResult LoadDocumentSnapshotByPath(const wchar_t* path,
                                              ActiveDocumentSnapshot* snapshot,
                                              SnapshotLogContext context) {
    if (path &&
        *path &&
        g_transitionFlushDocument &&
        g_transitionFlushDocumentIdentity) {
        const SnapshotQueryResult cachedResult =
            LoadDocumentSnapshotFromDispatch(g_transitionFlushDocument, snapshot, context);
        if (cachedResult == SnapshotQueryResult::Ready) {
            if (snapshot->hasPath && AreSameDocumentPath(snapshot->path.CStr(), path)) {
                return cachedResult;
            }

            snapshot->Reset();
        } else if (cachedResult == SnapshotQueryResult::RetryLater) {
            return cachedResult;
        }
    }

    const SnapshotRequest request = {SnapshotSource::SpecificPath, path, context};
    const SnapshotQueryResult documentResult = ResolveSnapshotDocument(request, snapshot);
    if (documentResult != SnapshotQueryResult::Ready) {
        return documentResult;
    }

    return PopulateSnapshotState(snapshot, context);
}

void ExpirePendingSaveAsMigrationIfNeeded() {
    if (!HasPendingSaveAsMigration() || g_pendingSaveAsTime == 0) {
        return;
    }

    const ULONGLONG now = GetTickCount64();
    if (now - g_pendingSaveAsTime > SAVE_AS_MIGRATION_TIMEOUT_MS) {
        ClearPendingSaveAsMigration();
    }
}

bool SetObservedDocumentIdentity(IUnknown* identity) {
    return ReplaceStoredUnknown(&g_observedDocumentIdentity, identity);
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

    InterlockedExchange(&g_documentDirtyKnown, TRUE);
    InterlockedExchange(&g_documentDirty, dirty ? TRUE : FALSE);
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

    const bool pathChanged = !AreSameDocumentPath(g_observedDocumentPath, snapshot->path.CStr());
    const bool identityChanged = !AreSameComIdentity(g_observedDocumentIdentity, snapshot->identity.Get());
    const LONG wasKnown = InterlockedCompareExchange(&g_documentDirtyKnown, TRUE, TRUE);
    const LONG wasDirty = InterlockedCompareExchange(&g_documentDirty, TRUE, TRUE);
    if (!SetObservedDocumentFromSnapshot(snapshot, true)) {
        return true;
    }

    return wasKnown == FALSE || wasDirty == FALSE || pathChanged || identityChanged;
}

bool IsObservedDocumentSnapshot(const ActiveDocumentSnapshot* snapshot) {
    return snapshot && snapshot->identity && AreSameComIdentity(snapshot->identity.Get(),
                                                               g_observedDocumentIdentity);
}

bool IsPendingSaveAsSnapshot(const ActiveDocumentSnapshot* snapshot) {
    return snapshot && snapshot->identity &&
           AreSameComIdentity(snapshot->identity.Get(), g_pendingSaveAsDocumentIdentity);
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

    const bool pathChanged = !AreSameDocumentPath(g_observedDocumentPath, snapshot->path.CStr());
    if (!pathChanged) {
        if (saveAsIdentityMatch) {
            ClearPendingSaveAsMigration();
        }
        return false;
    }

    const bool wasDirty = InterlockedCompareExchange(&g_documentDirty, TRUE, TRUE) == TRUE;
    SetObservedDocumentFromSnapshot(snapshot, !snapshot->saved && wasDirty);

    if (snapshot->saved) {
        g_lastSaveTime = GetTickCount64();
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
           g_observedDocumentPath &&
           !AreSameDocumentPath(snapshot->path.CStr(), g_observedDocumentPath) &&
           HasPendingAutosave();
}

void BeginTransitionFlushForObservedDocument() {
    RequestTransitionFlush(g_observedDocumentPath,
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

    const SnapshotRequest request = {SnapshotSource::Active, nullptr, SnapshotLogContext::DocumentState};
    const SnapshotQueryResult documentResult = ResolveSnapshotDocument(request, snapshot);
    if (documentResult == SnapshotQueryResult::RetryLater) {
        return DocumentDirtyState::RetryLater;
    }

    if (documentResult == SnapshotQueryResult::Cleared) {
        return DocumentDirtyState::Clean;
    }

    const SnapshotQueryResult savedStateResult =
        PopulateSnapshotSavedState(snapshot, SnapshotLogContext::DocumentState);
    if (savedStateResult == SnapshotQueryResult::RetryLater) {
        return DocumentDirtyState::RetryLater;
    }

    if (savedStateResult == SnapshotQueryResult::Cleared) {
        return DocumentDirtyState::Clean;
    }

    if (!snapshot->saved || requireCleanSnapshotDetails) {
        switch (PopulateSnapshotMetadata(snapshot, SnapshotLogContext::DocumentState)) {
            case SnapshotQueryResult::Ready:
                break;

            case SnapshotQueryResult::Cleared:
                return DocumentDirtyState::Clean;

            case SnapshotQueryResult::RetryLater:
                return DocumentDirtyState::RetryLater;
        }
    }

    return snapshot->saved ? DocumentDirtyState::Clean : DocumentDirtyState::Dirty;
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

    const SnapshotQueryResult snapshotResult =
        specificPath && *specificPath
            ? LoadDocumentSnapshotByPath(specificPath, snapshot, SnapshotLogContext::Save)
            : LoadActiveDocumentSnapshot(snapshot, SnapshotLogContext::Save);

    switch (snapshotResult) {
        case SnapshotQueryResult::Cleared:
            if ((!specificPath || !*specificPath) &&
                snapshot->clearedReason == SnapshotClearedReason::ProtectedView) {
                return SaveAttemptResult::Deferred;
            }
            return SaveAttemptResult::Cleared;

        case SnapshotQueryResult::RetryLater:
            return SaveAttemptResult::RetryLater;

        case SnapshotQueryResult::Ready:
            break;
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

void CALLBACK SaveTimerProc(HWND, UINT, UINT_PTR idEvent, DWORD) {
    KillTimer(nullptr, idEvent);
    g_saveTimerDueTime = 0;

    if (InterlockedCompareExchange(&g_moduleActive, TRUE, TRUE) == FALSE) {
        return;
    }

    if (idEvent != g_saveTimerId) {
        return;
    }

    g_saveTimerId = 0;
    if (!IsOwnerThread()) {
        return;
    }

    HandleAutosaveTick();
}

void CALLBACK DocumentStateTimerProc(HWND, UINT, UINT_PTR idEvent, DWORD) {
    KillTimer(nullptr, idEvent);
    g_documentStateTimerDueTime = 0;

    if (InterlockedCompareExchange(&g_moduleActive, TRUE, TRUE) == FALSE) {
        return;
    }

    if (idEvent != g_documentStateTimerId) {
        return;
    }

    g_documentStateTimerId = 0;
    if (!IsOwnerThread()) {
        return;
    }

    HandleDocumentStateTick();
}

bool ArmSaveTimer(DWORD delayMs) {
    if (!IsOwnerThread()) {
        return false;
    }

    const DWORD effectiveDelayMs = delayMs ? delayMs : 1;
    const ULONGLONG dueTime = GetTickCount64() + effectiveDelayMs;
    if (g_saveTimerId != 0 &&
        g_saveTimerDueTime != 0 &&
        g_saveTimerDueTime <= dueTime) {
        return true;
    }

    CancelSaveTimer();
    g_saveTimerId = SetTimer(nullptr, 0, effectiveDelayMs, SaveTimerProc);
    if (g_saveTimerId == 0) {
        Wh_Log(L"Auto-save: SetTimer failed, error=%lu", GetLastError());
        return false;
    }

    g_saveTimerDueTime = dueTime;
    return true;
}

void HandleDocumentStateTick() {
    ExpirePendingSaveAsMigrationIfNeeded();
    const TickRuntimeObservation runtime = ObserveTickRuntime();
    if (!runtime.ownerThreadSynchronized) {
        return;
    }

    if (!runtime.wordEventsConnected && runtime.activeWordDocumentWindow) {
        EnsureWordApplicationEventsConnected();
    }

    if (runtime.pauseReason != WordUiPauseReason::None) {
        LogDocumentStateStatus(DescribeWordUiPauseReason(runtime.pauseReason));
        ArmDocumentStateTimer(GetWordUiPauseDelay(runtime.pauseReason));
        return;
    }

    MaybeKickAutomationRecovery();

    if (!runtime.activeWordDocumentWindow) {
        if (HasPendingSaveWork() ||
            HasPendingSaveAsMigration() ||
            !AreWordEventsConnected()) {
            ArmDocumentStateTimer(GetSteadyDocumentStatePollDelay());
        }
        return;
    }

    if (runtime.pendingSave &&
        !runtime.manualSavePending &&
        !HasPendingSaveAsMigration()) {
        ArmDocumentStateTimer(DOCUMENT_STATE_ACTIVE_POLL_INTERVAL_MS);
        return;
    }

    if (runtime.inputBusy) {
        ArmDocumentStateTimer(INPUT_SETTLE_DELAY_MS);
        return;
    }

    ActiveDocumentSnapshot snapshot;
    const bool requireCleanSnapshotDetails =
        runtime.manualSavePending ||
        runtime.pendingSave ||
        HasPendingSaveAsMigration() ||
        InterlockedCompareExchange(&g_documentDirtyKnown, TRUE, TRUE) == TRUE;
    switch (QueryActiveDocumentDirtyState(&snapshot, requireCleanSnapshotDetails)) {
        case DocumentDirtyState::Dirty:
            g_documentStateRetryDelayMs = MIN_RETRY_INTERVAL_MS;
            ClearAutomationBusyPending();
            if (TryMigrateObservedDocumentIdentity(&snapshot, false)) {
                ArmDocumentStateTimer(GetSteadyDocumentStatePollDelay());
                return;
            }

            if (ShouldTransitionFlushObservedDocument(&snapshot)) {
                BeginTransitionFlushForObservedDocument();
                return;
            }

            if (snapshot.hasPath && NoteObservedDocumentDirty(&snapshot)) {
                LogDocumentStateStatus(L"detected non-keyboard document change");
                ScheduleSaveFromEdit();
            }
            break;

        case DocumentDirtyState::Clean:
            g_documentStateRetryDelayMs = MIN_RETRY_INTERVAL_MS;
            ClearAutomationBusyPending();
            if (TryMigrateObservedDocumentIdentity(&snapshot, true)) {
                ArmDocumentStateTimer(GetSteadyDocumentStatePollDelay());
                return;
            }

            if (ShouldTransitionFlushObservedDocument(&snapshot)) {
                BeginTransitionFlushForObservedDocument();
                return;
            }

            if (snapshot.hasPath) {
                const bool sameObservedDocument =
                    IsObservedDocumentSnapshot(&snapshot) ||
                    AreSameDocumentPath(g_observedDocumentPath, snapshot.path.CStr());
                if (runtime.manualSavePending) {
                    ClearManualSavePending();
                    g_lastSaveTime = GetTickCount64();
                }

                SyncObservedDocumentCleanState(&snapshot);
                if (sameObservedDocument && runtime.pendingSave) {
                    ClearPendingSave();
                    LogSaveStatus(L"pending changes were already saved by Word");
                }
            } else {
                ClearManualSavePending();
                ResetObservedDocumentState();
            }
            break;

        case DocumentDirtyState::RetryLater:
            NoteAutomationBusy(L"Word automation is busy, waiting to retry document-state refresh");
            ArmDocumentStateTimer(
                AdvanceRetryDelay(&g_documentStateRetryDelayMs,
                                  MAX_DOCUMENT_STATE_RETRY_INTERVAL_MS));
            return;
    }

    ArmDocumentStateTimer(GetSteadyDocumentStatePollDelay());
}

void HandleAutosaveTick() {
    ExpirePendingSaveAsMigrationIfNeeded();
    const TickRuntimeObservation runtime = ObserveTickRuntime();
    if (!runtime.pendingSave || !runtime.ownerThreadSynchronized) {
        return;
    }

    if (runtime.pauseReason != WordUiPauseReason::None) {
        LogSaveStatus(DescribeWordUiPauseReason(runtime.pauseReason));
        ArmSaveTimer(GetWordUiPauseDelay(runtime.pauseReason));
        return;
    }

    const ULONGLONG now = GetTickCount64();

    if (!runtime.transitionFlushRequested && !runtime.activeWordDocumentWindow) {
        ArmSaveTimer(DOCUMENT_STATE_ACTIVE_POLL_INTERVAL_MS);
        return;
    }

    const DWORD effectiveDelayMs =
        runtime.expeditedSaveRequested || runtime.transitionFlushRequested
            ? INPUT_SETTLE_DELAY_MS
            : static_cast<DWORD>(g_settings.saveDelay);
    const ULONGLONG earliestEditSaveTime =
        g_lastEditTime + static_cast<ULONGLONG>(effectiveDelayMs);
    if (now < earliestEditSaveTime) {
        ArmSaveTimer(static_cast<DWORD>(earliestEditSaveTime - now));
        return;
    }

    if (g_settings.minTimeBetweenSaves > 0 && g_lastSaveTime > 0) {
        const ULONGLONG earliestAllowedSave =
            g_lastSaveTime + static_cast<ULONGLONG>(g_settings.minTimeBetweenSaves);
        if (now < earliestAllowedSave) {
            ArmSaveTimer(static_cast<DWORD>(earliestAllowedSave - now));
            return;
        }
    }

    if (runtime.inputBusy) {
        ArmSaveTimer(INPUT_SETTLE_DELAY_MS);
        return;
    }

    ActiveDocumentSnapshot snapshot;
    const wchar_t* transitionPath =
        g_transitionFlushDocumentPath && *g_transitionFlushDocumentPath
            ? g_transitionFlushDocumentPath
            : nullptr;
    switch (TrySaveActiveDocument(&snapshot, transitionPath)) {
        case SaveAttemptResult::Saved:
            g_saveRetryDelayMs = MIN_RETRY_INTERVAL_MS;
            ClearAutomationBusyPending();
            g_lastSaveTime = GetTickCount64();
            if (snapshot.hasPath) {
                TryMigrateObservedDocumentIdentity(&snapshot, false);
            }
            FinalizeSaveAttemptState(&snapshot, true, true);
            if (runtime.transitionFlushRequested) {
                LogSaveStatus(L"flushed pending changes for the previous document");
            }
            break;

        case SaveAttemptResult::AlreadyClean:
            g_saveRetryDelayMs = MIN_RETRY_INTERVAL_MS;
            ClearAutomationBusyPending();
            if (snapshot.hasPath && TryMigrateObservedDocumentIdentity(&snapshot, true)) {
                break;
            }
            g_lastSaveTime = GetTickCount64();
            FinalizeSaveAttemptState(&snapshot, true, true);
            LogSaveStatus(runtime.transitionFlushRequested
                              ? L"the previous document was already clean"
                              : L"pending changes were already saved");
            break;

        case SaveAttemptResult::Cleared:
            g_saveRetryDelayMs = MIN_RETRY_INTERVAL_MS;
            ClearAutomationBusyPending();
            if (snapshot.hasPath && TryMigrateObservedDocumentIdentity(&snapshot, true)) {
                break;
            }
            if (snapshot.clearedReason != SnapshotClearedReason::None) {
                LogSaveStatus(DescribeSnapshotClearedReason(snapshot.clearedReason));
            }
            FinalizeSaveAttemptState(&snapshot, true, true);
            break;

        case SaveAttemptResult::Deferred:
            LogSaveStatus(DescribeSnapshotClearedReason(snapshot.clearedReason));
            ArmSaveTimer(DOCUMENT_STATE_ACTIVE_POLL_INTERVAL_MS);
            break;

        case SaveAttemptResult::RetryLater:
            NoteAutomationBusy(L"Word automation is busy, waiting to retry auto-save");
            ArmSaveTimer(AdvanceRetryDelay(&g_saveRetryDelayMs, MAX_SAVE_RETRY_INTERVAL_MS));
            break;
    }
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

// ============================================================================
// Hook
// ============================================================================

BOOL WINAPI TranslateMessage_Hook(const MSG* lpMsg) {
    if (!g_originalTranslateMessage) {
        return TRUE;
    }

    if (lpMsg) {
        if (ShouldInvalidateWordUiCacheForMessage(lpMsg)) {
            InvalidateWordUiWindowCache();
        }

        UpdateImeCompositionState(lpMsg);
        AdoptOwnerThreadIfNeeded(lpMsg);
        if (IsOwnerThread()) {
            const bool wordEventsConnected = AreWordEventsConnected();
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

            if (g_documentStateTimerId == 0 &&
                (HasPendingSaveWork() ||
                 HasPendingSaveAsMigration() ||
                 InterlockedCompareExchange(&g_automationBusyPending, TRUE, TRUE) == TRUE)) {
                ArmDocumentStateTimer(GetSteadyDocumentStatePollDelay());
            }
        }
    }

    return g_originalTranslateMessage(lpMsg);
}

// ============================================================================
// Windhawk Callbacks
// ============================================================================

bool RunInternalSelfTests() {
    bool success = true;

    if (ComputeBoundaryCoalesceDelay(false, 100, 200) != ACTION_BURST_SETTLE_DELAY_MS) {
        Wh_Log(L"ERROR: Internal self-test failed for burst coalescing delay");
        success = false;
    }

    if (ComputeBoundaryCoalesceDelay(true, 100, 200) != INPUT_SETTLE_DELAY_MS) {
        Wh_Log(L"ERROR: Internal self-test failed for transition coalescing delay");
        success = false;
    }

    DWORD retryDelayMs = MIN_RETRY_INTERVAL_MS;
    const DWORD firstDelay = AdvanceRetryDelay(&retryDelayMs, MAX_SAVE_RETRY_INTERVAL_MS);
    if (firstDelay != MIN_RETRY_INTERVAL_MS || retryDelayMs != MIN_RETRY_INTERVAL_MS * 2) {
        Wh_Log(L"ERROR: Internal self-test failed for retry backoff progression");
        success = false;
    }

    if (!ShouldUseMessageDocumentRefreshFallback(false) ||
        !ShouldUseMessageDocumentRefreshFallback(true)) {
        Wh_Log(L"ERROR: Internal self-test failed for message refresh fallback gating");
        success = false;
    }

    if (!ShouldUseMessageBoundaryFallback(false, false) ||
        !ShouldUseMessageBoundaryFallback(true, true) ||
        ShouldUseMessageBoundaryFallback(true, false)) {
        Wh_Log(L"ERROR: Internal self-test failed for boundary fallback gating");
        success = false;
    }

    return success;
}

void ResetRuntimeState() {
    g_saveTimerDueTime = 0;
    g_documentStateTimerDueTime = 0;
    g_lastEditTime = 0;
    g_lastSaveTime = 0;
    g_saveRetryDelayMs = MIN_RETRY_INTERVAL_MS;
    g_documentStateRetryDelayMs = MIN_RETRY_INTERVAL_MS;
    g_lastEventConnectAttemptTime = 0;
    g_pendingSaveAsTime = 0;
    g_lastSaveFailureLogTime = 0;
    g_lastDocumentStateFailureLogTime = 0;
    g_lastSaveStatusLogTime = 0;
    g_lastDocumentStateStatusLogTime = 0;
    ClearStoredStatusMessage(&g_lastSaveStatusMessage, &g_lastSaveStatusLogTime);
    ClearStoredStatusMessage(&g_lastDocumentStateStatusMessage, &g_lastDocumentStateStatusLogTime);
    DisconnectWordApplicationEvents();
    ClearAutomationBusyPending();
    ClearPendingSaveAsMigration();
    ClearPendingSave();
    ClearTransitionFlushRequest();
    ClearManualSavePending();
    InterlockedExchange(&g_imeComposing, FALSE);
    ResetObservedDocumentState();
    CancelSaveTimer();
    CancelDocumentStateTimer();
    InvalidateWordUiWindowCache();
    g_ownerThreadId = 0;
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
    Wh_Log(L"Word Local AutoSave v3.4 initializing...");

    g_wordProcessId = GetCurrentProcessId();
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

    InterlockedExchange(&g_moduleActive, TRUE);

    Wh_Log(L"Word Local AutoSave initialized");
    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L"Word Local AutoSave uninitializing...");

    InterlockedExchange(&g_moduleActive, FALSE);
    ResetRuntimeState();

    Wh_Log(L"Word Local AutoSave uninitialized");
}

void Wh_ModSettingsChanged() {
    Wh_Log(L"Settings changed, reloading...");
    ResetRuntimeState();
    LoadSettings();
}
