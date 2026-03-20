// ==WindhawkMod==
// @id              word-local-autosave
// @name            Word Local AutoSave
// @description     Enables AutoSave functionality for local documents in Microsoft Word via direct Word saves
// @version         3.0
// @author          communism420
// @github          https://github.com/communism420
// @include         WINWORD.EXE
// @architecture    x86-64
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Word Local AutoSave

This mod enables automatic saving for locally stored Word documents, similar to
how AutoSave works with OneDrive files.

## How it works

The mod monitors keyboard input in Microsoft Word. When you type, delete, paste,
or make other text-editing changes, it schedules a save after a short delay.

This build does **not** send `Ctrl+S`. It talks to Word directly through
automation and calls document save APIs, which removes the root cause of false
shortcut activations.

## Features

- Detects typing, backspace, delete, enter, punctuation, numpad, and clipboard operations
- Detects Ctrl+V, Ctrl+X, Ctrl+Z, Ctrl+Y, Ctrl+Enter (page break)
- Configurable delay before saving
- Optional minimum interval between saves to prevent excessive disk writes
- Direct Word save calls with zero synthetic keyboard input
- Only saves when the active Word document window is focused

## Shortcut Safety (v3.0)

- No `SendInput`
- No synthetic `Ctrl` state
- No partial `Ctrl+...` races
- Save execution stays on one owner UI thread
- Pending input and held modifiers postpone auto-save instead of racing it

## Limitations

- Mouse operations (click, drag & drop, context menu paste) are not detected
- Only works with documents that have already been saved at least once
- New unsaved documents are skipped to avoid opening "Save As"
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

// ============================================================================
// Constants
// ============================================================================

const int MIN_SAVE_DELAY_MS = 100;
const int MAX_SAVE_DELAY_MS = 60000;
const int MAX_MIN_TIME_BETWEEN_SAVES = 300000;
const DWORD RETRY_INTERVAL_MS = 50;
const DWORD INPUT_SETTLE_DELAY_MS = 25;
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
// Runtime Imports
// ============================================================================

typedef BOOL (WINAPI* TranslateMessage_t)(const MSG*);
typedef void (WINAPI* VariantInit_t)(VARIANTARG*);
typedef HRESULT (WINAPI* VariantClear_t)(VARIANTARG*);
typedef HRESULT (WINAPI* VariantChangeType_t)(VARIANTARG*, const VARIANTARG*, USHORT, VARTYPE);
typedef BSTR (WINAPI* SysAllocString_t)(const OLECHAR*);
typedef void (WINAPI* SysFreeString_t)(BSTR);
typedef UINT (WINAPI* SysStringLen_t)(BSTR);
typedef HRESULT (WINAPI* CLSIDFromProgID_t)(LPCOLESTR, LPCLSID);
typedef HRESULT (WINAPI* GetActiveObject_t)(REFCLSID, void*, IUnknown**);
typedef HRESULT (WINAPI* CoInitializeEx_t)(LPVOID, DWORD);
typedef void (WINAPI* CoUninitialize_t)(void);
typedef HRESULT (STDAPICALLTYPE* AccessibleObjectFromWindow_t)(HWND, DWORD, REFIID, void**);

struct RuntimeImports {
    VariantInit_t VariantInit = nullptr;
    VariantClear_t VariantClear = nullptr;
    VariantChangeType_t VariantChangeType = nullptr;
    SysAllocString_t SysAllocString = nullptr;
    SysFreeString_t SysFreeString = nullptr;
    SysStringLen_t SysStringLen = nullptr;
    CLSIDFromProgID_t CLSIDFromProgID = nullptr;
    GetActiveObject_t GetActiveObject = nullptr;
    CoInitializeEx_t CoInitializeEx = nullptr;
    CoUninitialize_t CoUninitialize = nullptr;
    AccessibleObjectFromWindow_t AccessibleObjectFromWindow = nullptr;
} g_runtime;

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
ULONGLONG g_lastEditTime = 0;
ULONGLONG g_lastSaveTime = 0;
volatile LONG g_pendingSave = FALSE;
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

bool LoadRuntimeImports() {
    if (g_runtime.VariantInit &&
        g_runtime.VariantClear &&
        g_runtime.VariantChangeType &&
        g_runtime.SysAllocString &&
        g_runtime.SysFreeString &&
        g_runtime.SysStringLen &&
        g_runtime.CLSIDFromProgID &&
        g_runtime.GetActiveObject &&
        g_runtime.CoInitializeEx &&
        g_runtime.CoUninitialize &&
        g_runtime.AccessibleObjectFromWindow) {
        return true;
    }

    HMODULE ole32 = GetModuleHandleW(L"ole32.dll");
    if (!ole32) {
        ole32 = LoadLibraryW(L"ole32.dll");
    }

    HMODULE oleaut32 = GetModuleHandleW(L"oleaut32.dll");
    if (!oleaut32) {
        oleaut32 = LoadLibraryW(L"oleaut32.dll");
    }

    HMODULE oleacc = GetModuleHandleW(L"oleacc.dll");
    if (!oleacc) {
        oleacc = LoadLibraryW(L"oleacc.dll");
    }

    if (!ole32 || !oleaut32 || !oleacc) {
        Wh_Log(L"ERROR: Failed to load OLE runtime modules");
        return false;
    }

    g_runtime.VariantInit =
        reinterpret_cast<VariantInit_t>(GetProcAddress(oleaut32, "VariantInit"));
    g_runtime.VariantClear =
        reinterpret_cast<VariantClear_t>(GetProcAddress(oleaut32, "VariantClear"));
    g_runtime.VariantChangeType =
        reinterpret_cast<VariantChangeType_t>(GetProcAddress(oleaut32, "VariantChangeType"));
    g_runtime.SysAllocString =
        reinterpret_cast<SysAllocString_t>(GetProcAddress(oleaut32, "SysAllocString"));
    g_runtime.SysFreeString =
        reinterpret_cast<SysFreeString_t>(GetProcAddress(oleaut32, "SysFreeString"));
    g_runtime.SysStringLen =
        reinterpret_cast<SysStringLen_t>(GetProcAddress(oleaut32, "SysStringLen"));
    g_runtime.CLSIDFromProgID =
        reinterpret_cast<CLSIDFromProgID_t>(GetProcAddress(ole32, "CLSIDFromProgID"));
    g_runtime.GetActiveObject =
        reinterpret_cast<GetActiveObject_t>(GetProcAddress(oleaut32, "GetActiveObject"));
    g_runtime.CoInitializeEx =
        reinterpret_cast<CoInitializeEx_t>(GetProcAddress(ole32, "CoInitializeEx"));
    g_runtime.CoUninitialize =
        reinterpret_cast<CoUninitialize_t>(GetProcAddress(ole32, "CoUninitialize"));
    g_runtime.AccessibleObjectFromWindow =
        reinterpret_cast<AccessibleObjectFromWindow_t>(
            GetProcAddress(oleacc, "AccessibleObjectFromWindow"));

    if (!g_runtime.VariantInit ||
        !g_runtime.VariantClear ||
        !g_runtime.VariantChangeType ||
        !g_runtime.SysAllocString ||
        !g_runtime.SysFreeString ||
        !g_runtime.SysStringLen ||
        !g_runtime.CLSIDFromProgID ||
        !g_runtime.GetActiveObject ||
        !g_runtime.CoInitializeEx ||
        !g_runtime.CoUninitialize ||
        !g_runtime.AccessibleObjectFromWindow) {
        Wh_Log(L"ERROR: Failed to resolve required OLE runtime functions");
        return false;
    }

    return true;
}

bool IsActiveWordDocumentWindow() {
    HWND foregroundWindow = GetForegroundWindow();
    if (!foregroundWindow) {
        return false;
    }

    DWORD foregroundProcessId = 0;
    GetWindowThreadProcessId(foregroundWindow, &foregroundProcessId);
    if (foregroundProcessId != g_wordProcessId) {
        return false;
    }

    HWND rootWindow = GetAncestor(foregroundWindow, GA_ROOT);
    if (!rootWindow) {
        rootWindow = foregroundWindow;
    }

    return HasClassName(rootWindow, L"OpusApp");
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

void AdoptOwnerThreadIfNeeded(const MSG* lpMsg) {
    if (!lpMsg || g_ownerThreadId != 0) {
        return;
    }

    switch (lpMsg->message) {
        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
        case WM_CHAR:
            break;
        default:
            return;
    }

    if (!IsActiveWordDocumentWindow()) {
        return;
    }

    InterlockedCompareExchange(
        reinterpret_cast<volatile LONG*>(&g_ownerThreadId),
        static_cast<LONG>(GetCurrentThreadId()),
        0);
}

void CancelSaveTimer() {
    if (g_saveTimerId != 0) {
        KillTimer(nullptr, g_saveTimerId);
        g_saveTimerId = 0;
    }
}

bool ArmSaveTimer(DWORD delayMs);
void HandleAutosaveTick();

void ScheduleSaveFromEdit() {
    g_lastEditTime = GetTickCount64();
    InterlockedExchange(&g_pendingSave, TRUE);
    ArmSaveTimer(static_cast<DWORD>(g_settings.saveDelay));
}

void ClearPendingSave() {
    InterlockedExchange(&g_pendingSave, FALSE);
}

void HandleManualSave() {
    g_lastSaveTime = GetTickCount64();
    ClearPendingSave();
    CancelSaveTimer();
}

// ============================================================================
// COM Helpers
// ============================================================================

class ScopedComInit {
public:
    ScopedComInit() {
        m_hr = g_runtime.CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
        if (m_hr == RPC_E_CHANGED_MODE) {
            m_hr = S_OK;
            m_shouldUninitialize = false;
            return;
        }

        m_shouldUninitialize = SUCCEEDED(m_hr);
    }

    ~ScopedComInit() {
        if (m_shouldUninitialize) {
            g_runtime.CoUninitialize();
        }
    }

    HRESULT GetResult() const {
        return m_hr;
    }

private:
    HRESULT m_hr = E_FAIL;
    bool m_shouldUninitialize = false;
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

    VARIANT value;
    g_runtime.VariantInit(&value);

    HRESULT hr = InvokeDispatch(dispatch,
                                DISPATCH_PROPERTYGET,
                                const_cast<LPOLESTR>(name),
                                &value);
    if (FAILED(hr)) {
        g_runtime.VariantClear(&value);
        return hr;
    }

    if (value.vt == VT_DISPATCH && value.pdispVal) {
        *result = value.pdispVal;
        value.pdispVal = nullptr;
        g_runtime.VariantClear(&value);
        return S_OK;
    }

    if (value.vt == VT_UNKNOWN && value.punkVal) {
        hr = value.punkVal->QueryInterface(IID_PPV_ARGS(result));
        g_runtime.VariantClear(&value);
        return hr;
    }

    g_runtime.VariantClear(&value);
    return DISP_E_TYPEMISMATCH;
}

HRESULT GetBoolProperty(IDispatch* dispatch, const wchar_t* name, bool* result) {
    if (!result) {
        return E_POINTER;
    }

    *result = false;

    VARIANT value;
    VARIANT converted;
    g_runtime.VariantInit(&value);
    g_runtime.VariantInit(&converted);

    HRESULT hr = InvokeDispatch(dispatch,
                                DISPATCH_PROPERTYGET,
                                const_cast<LPOLESTR>(name),
                                &value);
    if (SUCCEEDED(hr)) {
        hr = g_runtime.VariantChangeType(&converted, &value, 0, VT_BOOL);
        if (SUCCEEDED(hr)) {
            *result = converted.boolVal != VARIANT_FALSE;
        }
    }

    g_runtime.VariantClear(&converted);
    g_runtime.VariantClear(&value);
    return hr;
}

HRESULT GetBstrProperty(IDispatch* dispatch, const wchar_t* name, BSTR* result) {
    if (!result) {
        return E_POINTER;
    }

    *result = nullptr;

    VARIANT value;
    VARIANT converted;
    g_runtime.VariantInit(&value);
    g_runtime.VariantInit(&converted);

    HRESULT hr = InvokeDispatch(dispatch,
                                DISPATCH_PROPERTYGET,
                                const_cast<LPOLESTR>(name),
                                &value);
    if (SUCCEEDED(hr)) {
        hr = g_runtime.VariantChangeType(&converted, &value, 0, VT_BSTR);
        if (SUCCEEDED(hr) && converted.bstrVal) {
            *result = g_runtime.SysAllocString(converted.bstrVal);
            hr = *result ? S_OK : E_OUTOFMEMORY;
        }
    }

    g_runtime.VariantClear(&converted);
    g_runtime.VariantClear(&value);
    return hr;
}

HRESULT GetWordApplicationFromRot(IDispatch** application) {
    if (!application) {
        return E_POINTER;
    }

    *application = nullptr;

    CLSID wordClsid;
    HRESULT hr = g_runtime.CLSIDFromProgID(L"Word.Application", &wordClsid);
    if (FAILED(hr)) {
        return hr;
    }

    IUnknown* unknown = nullptr;
    hr = g_runtime.GetActiveObject(wordClsid, nullptr, &unknown);
    if (FAILED(hr) || !unknown) {
        return hr;
    }

    hr = unknown->QueryInterface(IID_PPV_ARGS(application));
    unknown->Release();
    return hr;
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

    IDispatch* nativeObject = nullptr;
    HRESULT hr = g_runtime.AccessibleObjectFromWindow(
        viewWindow,
        OBJID_NATIVEOM_VALUE,
        kIIDIDispatch,
        reinterpret_cast<void**>(&nativeObject));
    if (FAILED(hr) || !nativeObject) {
        return hr;
    }

    hr = GetDispatchProperty(nativeObject, L"Application", application);
    if (FAILED(hr) || !*application) {
        IDispatch* activeDocument = nullptr;
        if (SUCCEEDED(GetDispatchProperty(nativeObject, L"ActiveDocument", &activeDocument))) {
            activeDocument->Release();
            nativeObject->AddRef();
            *application = nativeObject;
            hr = S_OK;
        }
    }

    nativeObject->Release();
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
    Cleared,
    RetryLater,
};

SaveAttemptResult TrySaveActiveDocument() {
    if (!LoadRuntimeImports()) {
        return SaveAttemptResult::RetryLater;
    }

    ScopedComInit comInit;
    if (FAILED(comInit.GetResult())) {
        Wh_Log(L"Auto-save: CoInitializeEx failed, hr=0x%08X", comInit.GetResult());
        return SaveAttemptResult::RetryLater;
    }

    IDispatch* application = nullptr;
    HRESULT hr = GetWordApplication(&application);
    if (FAILED(hr) || !application) {
        if (hr == RPC_E_CALL_REJECTED || hr == RPC_E_SERVERCALL_RETRYLATER) {
            return SaveAttemptResult::RetryLater;
        }

        Wh_Log(L"Auto-save: failed to get Word application, hr=0x%08X", hr);
        return SaveAttemptResult::RetryLater;
    }

    IDispatch* document = nullptr;
    hr = GetDispatchProperty(application, L"ActiveDocument", &document);
    application->Release();
    if (FAILED(hr) || !document) {
        if (hr == RPC_E_CALL_REJECTED || hr == RPC_E_SERVERCALL_RETRYLATER) {
            return SaveAttemptResult::RetryLater;
        }

        return SaveAttemptResult::Cleared;
    }

    bool readOnly = false;
    hr = GetBoolProperty(document, L"ReadOnly", &readOnly);
    if (FAILED(hr)) {
        document->Release();
        Wh_Log(L"Auto-save: failed to query ReadOnly, hr=0x%08X", hr);
        return SaveAttemptResult::RetryLater;
    }

    if (readOnly) {
        document->Release();
        return SaveAttemptResult::Cleared;
    }

    BSTR path = nullptr;
    hr = GetBstrProperty(document, L"Path", &path);
    if (FAILED(hr)) {
        document->Release();
        Wh_Log(L"Auto-save: failed to query Path, hr=0x%08X", hr);
        return SaveAttemptResult::RetryLater;
    }

    const bool hasPath = path && g_runtime.SysStringLen(path) > 0;
    if (path) {
        g_runtime.SysFreeString(path);
    }

    if (!hasPath) {
        document->Release();
        return SaveAttemptResult::Cleared;
    }

    bool saved = true;
    hr = GetBoolProperty(document, L"Saved", &saved);
    if (FAILED(hr)) {
        document->Release();
        Wh_Log(L"Auto-save: failed to query Saved state, hr=0x%08X", hr);
        return SaveAttemptResult::RetryLater;
    }

    if (saved) {
        document->Release();
        return SaveAttemptResult::Cleared;
    }

    hr = InvokeDispatch(document, DISPATCH_METHOD, const_cast<LPOLESTR>(L"Save"));
    document->Release();

    if (SUCCEEDED(hr)) {
        return SaveAttemptResult::Saved;
    }

    if (hr == RPC_E_CALL_REJECTED || hr == RPC_E_SERVERCALL_RETRYLATER) {
        return SaveAttemptResult::RetryLater;
    }

    Wh_Log(L"Auto-save: document save failed, hr=0x%08X", hr);
    return SaveAttemptResult::Cleared;
}

void CALLBACK SaveTimerProc(HWND, UINT, UINT_PTR idEvent, DWORD) {
    if (InterlockedCompareExchange(&g_moduleActive, TRUE, TRUE) == FALSE) {
        return;
    }

    if (idEvent != g_saveTimerId) {
        return;
    }

    g_saveTimerId = 0;
    HandleAutosaveTick();
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

void HandleAutosaveTick() {
    if (InterlockedCompareExchange(&g_pendingSave, TRUE, TRUE) == FALSE) {
        return;
    }

    const ULONGLONG now = GetTickCount64();

    if (!IsActiveWordDocumentWindow()) {
        ArmSaveTimer(RETRY_INTERVAL_MS);
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

    switch (TrySaveActiveDocument()) {
        case SaveAttemptResult::Saved:
            g_lastSaveTime = GetTickCount64();
            ClearPendingSave();
            Wh_Log(L"Auto-save: document saved directly");
            break;

        case SaveAttemptResult::Cleared:
            ClearPendingSave();
            break;

        case SaveAttemptResult::RetryLater:
            ArmSaveTimer(RETRY_INTERVAL_MS);
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
        if (wParam == 'V' || wParam == 'X' || wParam == 'Y' || wParam == 'Z') {
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

        if (IsOwnerThread()) {
            if (lpMsg->message == WM_KEYDOWN && IsEditingKey(lpMsg->wParam)) {
                ScheduleSaveFromEdit();
            } else if (lpMsg->message == WM_CHAR && lpMsg->wParam >= 0x20) {
                ScheduleSaveFromEdit();
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
    Wh_Log(L"Word Local AutoSave v3.0 initializing...");

    g_wordProcessId = GetCurrentProcessId();
    LoadSettings();

    if (!LoadRuntimeImports()) {
        Wh_Log(L"ERROR: Failed to initialize required runtime imports");
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

    InterlockedExchange(&g_moduleActive, TRUE);

    Wh_Log(L"Word Local AutoSave initialized");
    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L"Word Local AutoSave uninitializing...");

    InterlockedExchange(&g_moduleActive, FALSE);
    ClearPendingSave();
    CancelSaveTimer();

    Wh_Log(L"Word Local AutoSave uninitialized");
}

void Wh_ModSettingsChanged() {
    Wh_Log(L"Settings changed, reloading...");
    ClearPendingSave();
    CancelSaveTimer();
    LoadSettings();
}
