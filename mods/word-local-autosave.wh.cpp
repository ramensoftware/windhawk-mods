// ==WindhawkMod==
// @id              word-local-autosave
// @name            Word Local AutoSave
// @description     Enables AutoSave functionality for local documents in Microsoft Word via direct Word saves
// @version         3.2
// @author          communism420
// @github          https://github.com/communism420
// @include         WINWORD.EXE
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -loleacc
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Word Local AutoSave

This mod enables automatic saving for locally stored Word documents, similar to
how AutoSave works with OneDrive files.

## How it works

The mod monitors keyboard input and document dirty-state changes in Microsoft
Word. When you type, paste, format text, or make other editing changes, it
schedules a save after a short delay.

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
- Only saves when the active Word document window is focused

## Shortcut Safety (v3.2)

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
#include <oleauto.h>
#include <oleacc.h>

// ============================================================================
// Constants
// ============================================================================

const int MIN_SAVE_DELAY_MS = 100;
const int MAX_SAVE_DELAY_MS = 60000;
const int MAX_MIN_TIME_BETWEEN_SAVES = 300000;
const DWORD MIN_RETRY_INTERVAL_MS = 50;
const DWORD INPUT_SETTLE_DELAY_MS = 25;
const DWORD DOCUMENT_STATE_ACTIVE_POLL_INTERVAL_MS = 350;
const DWORD DOCUMENT_STATE_IDLE_POLL_INTERVAL_MS = 1500;
const DWORD MAX_SAVE_RETRY_INTERVAL_MS = 1000;
const DWORD MAX_DOCUMENT_STATE_RETRY_INTERVAL_MS = 2000;
const DWORD FAILURE_LOG_INTERVAL_MS = 2000;
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

// ============================================================================
// Function Types
// ============================================================================

typedef BOOL (WINAPI* TranslateMessage_t)(const MSG*);

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
ULONGLONG g_lastEditTime = 0;
ULONGLONG g_lastSaveTime = 0;
BSTR g_observedDocumentPath = nullptr;
DWORD g_saveRetryDelayMs = MIN_RETRY_INTERVAL_MS;
DWORD g_documentStateRetryDelayMs = MIN_RETRY_INTERVAL_MS;
ULONGLONG g_lastSaveFailureLogTime = 0;
ULONGLONG g_lastDocumentStateFailureLogTime = 0;
volatile LONG g_pendingSave = FALSE;
volatile LONG g_documentDirtyKnown = FALSE;
volatile LONG g_documentDirty = FALSE;
volatile LONG g_manualSavePending = FALSE;
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

bool AreSameDocumentPath(const wchar_t* left, const wchar_t* right) {
    const bool leftEmpty = !left || !*left;
    const bool rightEmpty = !right || !*right;
    if (leftEmpty || rightEmpty) {
        return leftEmpty == rightEmpty;
    }

    return lstrcmpiW(left, right) == 0;
}

bool SetObservedDocumentPath(const wchar_t* path) {
    if (AreSameDocumentPath(g_observedDocumentPath, path)) {
        return true;
    }

    BSTR newPath = nullptr;
    if (path && *path) {
        newPath = SysAllocString(path);
        if (!newPath) {
            return false;
        }
    }

    if (g_observedDocumentPath) {
        SysFreeString(g_observedDocumentPath);
    }

    g_observedDocumentPath = newPath;
    return true;
}

void ClearManualSavePending() {
    InterlockedExchange(&g_manualSavePending, FALSE);
}

void ResetObservedDocumentState();

void MarkObservedDocumentClean(const wchar_t* path) {
    if (!path || !*path) {
        ResetObservedDocumentState();
        return;
    }

    if (!SetObservedDocumentPath(path)) {
        ResetObservedDocumentState();
        return;
    }

    InterlockedExchange(&g_documentDirtyKnown, TRUE);
    InterlockedExchange(&g_documentDirty, FALSE);
}

void ResetObservedDocumentState() {
    if (g_observedDocumentPath) {
        SysFreeString(g_observedDocumentPath);
        g_observedDocumentPath = nullptr;
    }

    InterlockedExchange(&g_documentDirtyKnown, FALSE);
    InterlockedExchange(&g_documentDirty, FALSE);
}

bool NoteObservedDocumentDirty(const wchar_t* path) {
    if (!path || !*path) {
        ResetObservedDocumentState();
        return false;
    }

    const bool pathChanged = !AreSameDocumentPath(g_observedDocumentPath, path);
    if (!SetObservedDocumentPath(path)) {
        ResetObservedDocumentState();
        return true;
    }

    const LONG wasKnown = InterlockedCompareExchange(&g_documentDirtyKnown, TRUE, TRUE);
    const LONG wasDirty = InterlockedExchange(&g_documentDirty, TRUE);
    InterlockedExchange(&g_documentDirtyKnown, TRUE);
    return wasKnown == FALSE || wasDirty == FALSE || pathChanged;
}

DWORD GetActiveWordUiThreadId() {
    HWND foregroundWindow = GetForegroundWindow();
    if (!foregroundWindow) {
        return 0;
    }

    DWORD foregroundProcessId = 0;
    GetWindowThreadProcessId(foregroundWindow, &foregroundProcessId);
    if (foregroundProcessId != g_wordProcessId) {
        return 0;
    }

    HWND rootWindow = GetAncestor(foregroundWindow, GA_ROOT);
    if (!rootWindow) {
        rootWindow = foregroundWindow;
    }

    if (!HasClassName(rootWindow, L"OpusApp")) {
        return 0;
    }

    return GetWindowThreadProcessId(rootWindow, nullptr);
}

bool IsActiveWordDocumentWindow() {
    return GetActiveWordUiThreadId() != 0;
}

bool IsCurrentThreadActiveWordUiThread() {
    const DWORD activeWordUiThreadId = GetActiveWordUiThreadId();
    return activeWordUiThreadId != 0 && activeWordUiThreadId == GetCurrentThreadId();
}

bool IsCurrentThreadActiveOwner() {
    const DWORD activeWordUiThreadId = GetActiveWordUiThreadId();
    return activeWordUiThreadId != 0 &&
           g_ownerThreadId == activeWordUiThreadId &&
           GetCurrentThreadId() == activeWordUiThreadId;
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

BOOL CALLBACK FindWordViewWindowProc(HWND hwnd, LPARAM lParam) {
    HWND* result = reinterpret_cast<HWND*>(lParam);
    if (!result || *result) {
        return FALSE;
    }

    if (HasClassName(hwnd, L"_WwG")) {
        *result = hwnd;
        return FALSE;
    }

    return TRUE;
}

HWND FindNativeWordViewWindow() {
    HWND foregroundWindow = GetForegroundWindow();
    if (!foregroundWindow) {
        return nullptr;
    }

    HWND rootWindow = GetAncestor(foregroundWindow, GA_ROOT);
    if (!rootWindow) {
        rootWindow = foregroundWindow;
    }

    DWORD threadId = GetWindowThreadProcessId(rootWindow, nullptr);
    GUITHREADINFO guiThreadInfo = {};
    guiThreadInfo.cbSize = sizeof(guiThreadInfo);

    if (threadId && GetGUIThreadInfo(threadId, &guiThreadInfo)) {
        HWND candidates[] = {
            guiThreadInfo.hwndFocus,
            guiThreadInfo.hwndCaret,
            foregroundWindow,
            rootWindow,
        };

        for (HWND candidate : candidates) {
            while (candidate) {
                if (HasClassName(candidate, L"_WwG")) {
                    return candidate;
                }

                if (candidate == rootWindow) {
                    break;
                }

                candidate = GetParent(candidate);
            }
        }
    }

    HWND result = nullptr;
    EnumChildWindows(rootWindow, FindWordViewWindowProc, reinterpret_cast<LPARAM>(&result));
    return result;
}

bool ArmDocumentStateTimer(DWORD delayMs);
bool ArmSaveTimer(DWORD delayMs);

void AdoptOwnerThreadIfNeeded(const MSG* lpMsg) {
    if (!lpMsg || !IsOwnerCandidateMessage(lpMsg->message)) {
        return;
    }

    if (!IsCurrentThreadActiveWordUiThread()) {
        return;
    }

    const DWORD currentThreadId = GetCurrentThreadId();
    const DWORD previousOwnerThreadId = InterlockedExchange(
        reinterpret_cast<volatile LONG*>(&g_ownerThreadId),
        static_cast<LONG>(currentThreadId));
    if (previousOwnerThreadId != currentThreadId) {
        g_saveRetryDelayMs = MIN_RETRY_INTERVAL_MS;
        g_documentStateRetryDelayMs = MIN_RETRY_INTERVAL_MS;
        ResetObservedDocumentState();
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
}

void CancelDocumentStateTimer() {
    if (g_documentStateTimerId != 0) {
        KillTimer(nullptr, g_documentStateTimerId);
        g_documentStateTimerId = 0;
    }
}

void HandleAutosaveTick();
void HandleDocumentStateTick();
void CALLBACK DocumentStateTimerProc(HWND, UINT, UINT_PTR, DWORD);

DWORD GetSteadyDocumentStatePollDelay() {
    if (InterlockedCompareExchange(&g_pendingSave, TRUE, TRUE) == TRUE ||
        InterlockedCompareExchange(&g_documentDirty, TRUE, TRUE) == TRUE ||
        InterlockedCompareExchange(&g_manualSavePending, TRUE, TRUE) == TRUE) {
        return DOCUMENT_STATE_ACTIVE_POLL_INTERVAL_MS;
    }

    return DOCUMENT_STATE_IDLE_POLL_INTERVAL_MS;
}

bool ArmDocumentStateTimer(DWORD delayMs) {
    if (!IsOwnerThread()) {
        return false;
    }

    CancelDocumentStateTimer();
    g_documentStateTimerId =
        SetTimer(nullptr, 0, delayMs ? delayMs : 1, DocumentStateTimerProc);
    if (g_documentStateTimerId == 0) {
        Wh_Log(L"Document state monitor: SetTimer failed, error=%lu", GetLastError());
        return false;
    }

    return true;
}

void ScheduleSaveFromEdit() {
    g_lastEditTime = GetTickCount64();
    InterlockedExchange(&g_pendingSave, TRUE);
    ArmSaveTimer(static_cast<DWORD>(g_settings.saveDelay));
}

void ClearPendingSave() {
    InterlockedExchange(&g_pendingSave, FALSE);
}

void HandleManualSave() {
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

    return unknown->QueryInterface(IID_PPV_ARGS(application));
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
    HRESULT hr = AccessibleObjectFromWindow(
        viewWindow,
        OBJID_NATIVEOM_VALUE,
        kIIDIDispatch,
        reinterpret_cast<void**>(nativeObject.Put()));
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

    return hr;
}

HRESULT GetWordApplication(IDispatch** application) {
    HRESULT hr = GetWordApplicationFromActiveWindow(application);
    if (SUCCEEDED(hr) && application && *application) {
        return hr;
    }

    return GetWordApplicationFromRot(application);
}

// ============================================================================
// Save Logic
// ============================================================================

enum class SaveAttemptResult {
    Saved,
    AlreadyClean,
    Cleared,
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

struct ActiveDocumentSnapshot {
    ScopedComPtr<IDispatch> document;
    ScopedBstr path;
    bool readOnly = false;
    bool saved = true;
    bool hasPath = false;

    void Reset() {
        document.Reset();
        path.Reset();
        readOnly = false;
        saved = true;
        hasPath = false;
    }
};

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

SnapshotQueryResult LoadActiveDocumentSnapshot(ActiveDocumentSnapshot* snapshot,
                                               SnapshotLogContext context) {
    if (!snapshot) {
        return SnapshotQueryResult::Cleared;
    }

    snapshot->Reset();

    ScopedComPtr<IDispatch> application;
    HRESULT hr = GetWordApplication(application.Put());
    if (FAILED(hr) || !application) {
        if (IsRetryableAutomationFailure(hr)) {
            return SnapshotQueryResult::RetryLater;
        }

        LogSnapshotFailure(context, L"failed to get Word application", hr);
        return SnapshotQueryResult::RetryLater;
    }

    hr = GetDispatchProperty(application.Get(), L"ActiveDocument", snapshot->document.Put());
    if (FAILED(hr) || !snapshot->document) {
        if (IsRetryableAutomationFailure(hr)) {
            return SnapshotQueryResult::RetryLater;
        }

        return SnapshotQueryResult::Cleared;
    }

    hr = GetBoolProperty(snapshot->document.Get(), L"ReadOnly", &snapshot->readOnly);
    if (FAILED(hr)) {
        LogSnapshotFailure(context, L"failed to query ReadOnly", hr);
        return SnapshotQueryResult::RetryLater;
    }

    if (snapshot->readOnly) {
        return SnapshotQueryResult::Cleared;
    }

    hr = GetBstrProperty(snapshot->document.Get(), L"Path", snapshot->path.Put());
    if (FAILED(hr)) {
        LogSnapshotFailure(context, L"failed to query Path", hr);
        return SnapshotQueryResult::RetryLater;
    }

    snapshot->hasPath = snapshot->path.Length() > 0;
    if (!snapshot->hasPath) {
        return SnapshotQueryResult::Cleared;
    }

    hr = GetBoolProperty(snapshot->document.Get(), L"Saved", &snapshot->saved);
    if (FAILED(hr)) {
        LogSnapshotFailure(context, L"failed to query Saved state", hr);
        return SnapshotQueryResult::RetryLater;
    }

    return SnapshotQueryResult::Ready;
}

DocumentDirtyState QueryActiveDocumentDirtyState(ActiveDocumentSnapshot* snapshot) {
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

    switch (LoadActiveDocumentSnapshot(snapshot, SnapshotLogContext::DocumentState)) {
        case SnapshotQueryResult::Ready:
            return snapshot->saved ? DocumentDirtyState::Clean : DocumentDirtyState::Dirty;

        case SnapshotQueryResult::Cleared:
            return DocumentDirtyState::Clean;

        case SnapshotQueryResult::RetryLater:
            return DocumentDirtyState::RetryLater;
    }

    return DocumentDirtyState::RetryLater;
}

SaveAttemptResult TrySaveActiveDocument(ActiveDocumentSnapshot* snapshot) {
    ScopedComInit comInit;
    if (FAILED(comInit.GetResult())) {
        LogSnapshotFailure(SnapshotLogContext::Save, L"CoInitializeEx failed", comInit.GetResult());
        return SaveAttemptResult::RetryLater;
    }

    ActiveDocumentSnapshot localSnapshot;
    if (!snapshot) {
        snapshot = &localSnapshot;
    }

    switch (LoadActiveDocumentSnapshot(snapshot, SnapshotLogContext::Save)) {
        case SnapshotQueryResult::Cleared:
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

    CancelSaveTimer();
    g_saveTimerId = SetTimer(nullptr, 0, delayMs ? delayMs : 1, SaveTimerProc);
    if (g_saveTimerId == 0) {
        Wh_Log(L"Auto-save: SetTimer failed, error=%lu", GetLastError());
        return false;
    }

    return true;
}

void HandleDocumentStateTick() {
    if (!IsOwnerThread()) {
        return;
    }

    const DWORD activeWordUiThreadId = GetActiveWordUiThreadId();
    if (activeWordUiThreadId != 0 && activeWordUiThreadId != g_ownerThreadId) {
        return;
    }

    if (!IsActiveWordDocumentWindow()) {
        ArmDocumentStateTimer(GetSteadyDocumentStatePollDelay());
        return;
    }

    if (InterlockedCompareExchange(&g_pendingSave, TRUE, TRUE) == TRUE) {
        ArmDocumentStateTimer(DOCUMENT_STATE_ACTIVE_POLL_INTERVAL_MS);
        return;
    }

    if (GetInputState() || AreModifiersOrMouseButtonsHeld()) {
        ArmDocumentStateTimer(INPUT_SETTLE_DELAY_MS);
        return;
    }

    ActiveDocumentSnapshot snapshot;
    switch (QueryActiveDocumentDirtyState(&snapshot)) {
        case DocumentDirtyState::Dirty:
            g_documentStateRetryDelayMs = MIN_RETRY_INTERVAL_MS;
            if (snapshot.hasPath && NoteObservedDocumentDirty(snapshot.path.CStr())) {
                Wh_Log(L"Document state monitor: detected non-keyboard document change");
                ScheduleSaveFromEdit();
            }
            break;

        case DocumentDirtyState::Clean:
            g_documentStateRetryDelayMs = MIN_RETRY_INTERVAL_MS;
            if (snapshot.hasPath) {
                if (InterlockedExchange(&g_manualSavePending, FALSE) == TRUE) {
                    g_lastSaveTime = GetTickCount64();
                }

                MarkObservedDocumentClean(snapshot.path.CStr());
            } else {
                ClearManualSavePending();
                ResetObservedDocumentState();
            }
            break;

        case DocumentDirtyState::RetryLater:
            ArmDocumentStateTimer(
                AdvanceRetryDelay(&g_documentStateRetryDelayMs,
                                  MAX_DOCUMENT_STATE_RETRY_INTERVAL_MS));
            return;
    }

    ArmDocumentStateTimer(GetSteadyDocumentStatePollDelay());
}

void HandleAutosaveTick() {
    if (InterlockedCompareExchange(&g_pendingSave, TRUE, TRUE) == FALSE) {
        return;
    }

    if (!IsOwnerThread()) {
        return;
    }

    const DWORD activeWordUiThreadId = GetActiveWordUiThreadId();
    if (activeWordUiThreadId != 0 && activeWordUiThreadId != g_ownerThreadId) {
        return;
    }

    const ULONGLONG now = GetTickCount64();

    if (!IsActiveWordDocumentWindow()) {
        ArmSaveTimer(DOCUMENT_STATE_ACTIVE_POLL_INTERVAL_MS);
        return;
    }

    const ULONGLONG earliestEditSaveTime =
        g_lastEditTime + static_cast<ULONGLONG>(g_settings.saveDelay);
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

    if (GetInputState() || AreModifiersOrMouseButtonsHeld()) {
        ArmSaveTimer(INPUT_SETTLE_DELAY_MS);
        return;
    }

    ActiveDocumentSnapshot snapshot;
    switch (TrySaveActiveDocument(&snapshot)) {
        case SaveAttemptResult::Saved:
            g_saveRetryDelayMs = MIN_RETRY_INTERVAL_MS;
            g_lastSaveTime = GetTickCount64();
            ClearManualSavePending();
            if (snapshot.hasPath) {
                MarkObservedDocumentClean(snapshot.path.CStr());
            } else {
                ResetObservedDocumentState();
            }
            ClearPendingSave();
            Wh_Log(L"Auto-save: document saved directly");
            break;

        case SaveAttemptResult::AlreadyClean:
            g_saveRetryDelayMs = MIN_RETRY_INTERVAL_MS;
            g_lastSaveTime = GetTickCount64();
            ClearManualSavePending();
            if (snapshot.hasPath) {
                MarkObservedDocumentClean(snapshot.path.CStr());
            } else {
                ResetObservedDocumentState();
            }
            ClearPendingSave();
            break;

        case SaveAttemptResult::Cleared:
            g_saveRetryDelayMs = MIN_RETRY_INTERVAL_MS;
            ClearManualSavePending();
            if (snapshot.hasPath) {
                MarkObservedDocumentClean(snapshot.path.CStr());
            } else {
                ResetObservedDocumentState();
            }
            ClearPendingSave();
            break;

        case SaveAttemptResult::RetryLater:
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

// ============================================================================
// Hook
// ============================================================================

BOOL WINAPI TranslateMessage_Hook(const MSG* lpMsg) {
    if (!g_originalTranslateMessage) {
        return TRUE;
    }

    if (lpMsg) {
        AdoptOwnerThreadIfNeeded(lpMsg);

        if (IsCurrentThreadActiveOwner()) {
            if (g_documentStateTimerId == 0) {
                ArmDocumentStateTimer(GetSteadyDocumentStatePollDelay());
            }

            if (lpMsg->message == WM_KEYDOWN && IsEditingKey(lpMsg->wParam)) {
                ScheduleSaveFromEdit();
            } else if (lpMsg->message == WM_CHAR && lpMsg->wParam >= 0x20) {
                ScheduleSaveFromEdit();
            } else if (IsDocumentStateRefreshMessage(lpMsg->message)) {
                ArmDocumentStateTimer(INPUT_SETTLE_DELAY_MS);
            }
        }
    }

    return g_originalTranslateMessage(lpMsg);
}

// ============================================================================
// Windhawk Callbacks
// ============================================================================

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
    Wh_Log(L"Word Local AutoSave v3.2 initializing...");

    g_wordProcessId = GetCurrentProcessId();
    g_saveRetryDelayMs = MIN_RETRY_INTERVAL_MS;
    g_documentStateRetryDelayMs = MIN_RETRY_INTERVAL_MS;
    g_lastSaveFailureLogTime = 0;
    g_lastDocumentStateFailureLogTime = 0;
    g_ownerThreadId = 0;
    ClearManualSavePending();
    ResetObservedDocumentState();
    LoadSettings();

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
    ClearManualSavePending();
    ResetObservedDocumentState();
    ClearPendingSave();
    CancelSaveTimer();
    CancelDocumentStateTimer();
    g_ownerThreadId = 0;

    Wh_Log(L"Word Local AutoSave uninitialized");
}

void Wh_ModSettingsChanged() {
    Wh_Log(L"Settings changed, reloading...");
    g_saveRetryDelayMs = MIN_RETRY_INTERVAL_MS;
    g_documentStateRetryDelayMs = MIN_RETRY_INTERVAL_MS;
    g_lastSaveFailureLogTime = 0;
    g_lastDocumentStateFailureLogTime = 0;
    ClearManualSavePending();
    ResetObservedDocumentState();
    ClearPendingSave();
    CancelSaveTimer();
    CancelDocumentStateTimer();
    g_ownerThreadId = 0;
    LoadSettings();
}
