// ==WindhawkMod==
// @id             native-brightness
// @name           Native Brightness Shortcut (Always On Top)
// @description    Use CTRL + ALT + UP/DOWN to change brightness. Runs in a dedicated stable windhawk.exe process (Tool Mode).
// @version        1.1.0
// @author         Prash
// @github         https://github.com/prasmit2410
// @include        windhawk.exe
// @compilerOptions -lole32 -loleaut32 -lwbemuuid -lgdi32 -luser32 -lshell32
// @license        MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
This mod allows you to control laptop screen brightness using CTRL + ALT + UP and CTRL + ALT + DOWN.
It synchronizes with your system's actual brightness level on startup and displays a custom OSD.
Note: This works for built-in displays (Laptops/Tablets). External monitors may not support WMI brightness.

**Update Info**
The new update has resolved the previous issue of mod not working properly..
*/
// ==/WindhawkModReadme==

#include <windows.h>
#include <wbemidl.h>
#include <stdio.h>

// ---------------------------------------------------------------------------
// Globals
// ---------------------------------------------------------------------------
IWbemLocator *pLoc = NULL;
IWbemServices *pSvc = NULL;
bool g_wmiInitialized = false;
HHOOK g_hHook = NULL;
HWND g_hOSD = NULL;

int g_currentBrightness = 50; // Will be overwritten by Sync
const wchar_t* OSD_CLASS_NAME = L"WhBrightnessOSD_Tool";

HANDLE g_hookThread = NULL;
DWORD g_hookThreadId = 0;

// ---------------------------------------------------------------------------
// 1. Hardware Logic (WMI)
// ---------------------------------------------------------------------------

void InitWMI() {
    if (g_wmiInitialized) return;
    
    HRESULT hr = CoInitializeEx(0, COINIT_MULTITHREADED);
    hr = CoInitializeSecurity(NULL, -1, NULL, NULL, RPC_C_AUTHN_LEVEL_DEFAULT, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE, NULL);
    
    if (SUCCEEDED(CoCreateInstance(CLSID_WbemLocator, 0, CLSCTX_INPROC_SERVER, IID_IWbemLocator, (LPVOID *)&pLoc))) {
        BSTR ns = SysAllocString(L"ROOT\\WMI");
        if (SUCCEEDED(pLoc->ConnectServer(ns, NULL, NULL, 0, NULL, 0, 0, &pSvc))) {
            CoSetProxyBlanket(pSvc, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, NULL, RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE);
            g_wmiInitialized = true;
        }
        SysFreeString(ns);
    }
}

void SyncBrightnessWithSystem() {
    if (!g_wmiInitialized || !pSvc) return;

    IEnumWbemClassObject* pEnum = NULL;
    BSTR query = SysAllocString(L"SELECT CurrentBrightness FROM WmiMonitorBrightness");
    BSTR wql = SysAllocString(L"WQL");

    if (SUCCEEDED(pSvc->ExecQuery(wql, query, WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, NULL, &pEnum))) {
        IWbemClassObject *pObj = NULL;
        ULONG ret = 0;
        pEnum->Next(WBEM_INFINITE, 1, &pObj, &ret);
        if (ret && pObj) {
            VARIANT vtProp;
            VariantInit(&vtProp);
            if (SUCCEEDED(pObj->Get(L"CurrentBrightness", 0, &vtProp, 0, 0))) {
                g_currentBrightness = vtProp.bVal; 
                VariantClear(&vtProp);
            }
            pObj->Release();
        }
        pEnum->Release();
    }
    SysFreeString(query);
    SysFreeString(wql);
}

void SetHardwareBrightness(int level) {
    if (!g_wmiInitialized || !pSvc) return;
    
    BSTR className = SysAllocString(L"WmiMonitorBrightnessMethods");
    BSTR methodName = SysAllocString(L"WmiSetBrightness");
    IEnumWbemClassObject* pEnum = NULL;

    if (SUCCEEDED(pSvc->CreateInstanceEnum(className, 0, NULL, &pEnum))) {
        IWbemClassObject *pInst = NULL; ULONG ret = 0;
        pEnum->Next(WBEM_INFINITE, 1, &pInst, &ret);
        if (ret && pInst) {
            VARIANT vtPath; VariantInit(&vtPath);
            pInst->Get(L"__PATH", 0, &vtPath, 0, 0);
            
            IWbemClassObject* pClass = NULL;
            if (SUCCEEDED(pSvc->GetObject(className, 0, NULL, &pClass, NULL))) {
                IWbemClassObject* pInParamsDef = NULL;
                pClass->GetMethod(methodName, 0, &pInParamsDef, NULL);
                
                IWbemClassObject* pInParamsInst = NULL;
                pInParamsDef->SpawnInstance(0, &pInParamsInst);
                
                VARIANT vTimeout; vTimeout.vt = VT_I4; vTimeout.lVal = 0;
                pInParamsInst->Put(L"Timeout", 0, &vTimeout, 0);
                
                VARIANT vBright; vBright.vt = VT_UI1; vBright.bVal = (BYTE)level;
                pInParamsInst->Put(L"Brightness", 0, &vBright, 0);
                
                pSvc->ExecMethod(vtPath.bstrVal, methodName, 0, NULL, pInParamsInst, NULL, NULL);
                
                pInParamsInst->Release(); pInParamsDef->Release(); pClass->Release();
            }
            VariantClear(&vtPath); pInst->Release();
        }
        pEnum->Release();
    }
    SysFreeString(className); SysFreeString(methodName);
}

// ---------------------------------------------------------------------------
// 2. UI Logic
// ---------------------------------------------------------------------------

LRESULT CALLBACK OSDWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        RECT rect; GetClientRect(hwnd, &rect);
        
        // Background
        HBRUSH hBrush = CreateSolidBrush(RGB(25, 25, 25));
        FillRect(hdc, &rect, hBrush);
        DeleteObject(hBrush);
        
        SetBkMode(hdc, TRANSPARENT);
        SetTextColor(hdc, RGB(255, 255, 255));
        
        HFONT hFont = CreateFont(22, 0, 0, 0, FW_SEMIBOLD, 0, 0, 0, 0, 0, 0, 0, 0, L"Segoe UI");
        SelectObject(hdc, hFont);
        
        wchar_t buf[32]; swprintf(buf, 32, L"Brightness: %d%%", g_currentBrightness);
        DrawText(hdc, buf, -1, &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        
        DeleteObject(hFont);
        EndPaint(hwnd, &ps); return 0;
    }
    case WM_TIMER: ShowWindow(hwnd, SW_HIDE); KillTimer(hwnd, 1); return 0;
    default: return DefWindowProc(hwnd, msg, wParam, lParam);
    }
}

LRESULT CALLBACK KeyboardHook(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION && (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN)) {
        KBDLLHOOKSTRUCT* p = (KBDLLHOOKSTRUCT*)lParam;
        bool ctrl = (GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0;
        bool alt = (GetAsyncKeyState(VK_MENU) & 0x8000) != 0;
        
        if (ctrl && alt) {
            int step = 0;
            if (p->vkCode == VK_UP) step = 10;
            if (p->vkCode == VK_DOWN) step = -10;
            
            if (step != 0) {
                g_currentBrightness += step;
                if (g_currentBrightness > 100) g_currentBrightness = 100;
                if (g_currentBrightness < 0) g_currentBrightness = 0;
                
                SetHardwareBrightness(g_currentBrightness);
                InvalidateRect(g_hOSD, NULL, TRUE);
                ShowWindow(g_hOSD, SW_SHOWNOACTIVATE);
                SetTimer(g_hOSD, 1, 1500, NULL);
                return 1;
            }
        }
    }
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

// ---------------------------------------------------------------------------
// 3. Thread Management
// ---------------------------------------------------------------------------

DWORD WINAPI HookThreadProc(LPVOID lpParam) {
    InitWMI();
    SyncBrightnessWithSystem(); // Real hardware sync on startup

    WNDCLASS wc = { 0 };
    wc.lpfnWndProc = OSDWndProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = OSD_CLASS_NAME;
    RegisterClass(&wc);

    int w = 240, h = 60;
    int x = (GetSystemMetrics(SM_CXSCREEN) - w) / 2;
    int y = GetSystemMetrics(SM_CYSCREEN) - 180;
    g_hOSD = CreateWindowEx(WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_LAYERED, OSD_CLASS_NAME, L"", WS_POPUP, x, y, w, h, NULL, NULL, wc.hInstance, NULL);
    SetLayeredWindowAttributes(g_hOSD, 0, 230, LWA_ALPHA);

    g_hHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardHook, GetModuleHandle(NULL), 0);
    
    HANDLE hEvent = (HANDLE)lpParam;
    if (hEvent) SetEvent(hEvent);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    if (g_hHook) UnhookWindowsHookEx(g_hHook);
    if (g_hOSD) DestroyWindow(g_hOSD);
    if (pSvc) { pSvc->Release(); pSvc = NULL; }
    if (pLoc) { pLoc->Release(); pLoc = NULL; }
    CoUninitialize();

    return 0;
}

// ---------------------------------------------------------------------------
// 4. Windhawk Tool Callbacks
// ---------------------------------------------------------------------------

BOOL WhTool_ModInit() {
    HANDLE hThreadReadyEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    g_hookThread = CreateThread(NULL, 0, HookThreadProc, hThreadReadyEvent, 0, &g_hookThreadId);
    
    if (g_hookThread) {
        WaitForSingleObject(hThreadReadyEvent, 3000);
        CloseHandle(hThreadReadyEvent);
        return TRUE;
    }
    return FALSE;
}

void WhTool_ModUninit() {
    if (g_hookThreadId) {
        PostThreadMessage(g_hookThreadId, WM_QUIT, 0, 0);
        WaitForSingleObject(g_hookThread, 1000);
        CloseHandle(g_hookThread);
        g_hookThread = NULL;
        g_hookThreadId = 0;
    }
}

void WhTool_ModSettingsChanged() {}

// ---------------------------------------------------------------------------
// 5. Official Windhawk Launcher Logic
// ---------------------------------------------------------------------------
bool g_isToolModProcessLauncher;
HANDLE g_toolModProcessMutex;

void WINAPI EntryPoint_Hook() {
    ExitThread(0);
}

BOOL Wh_ModInit() {
    bool isExcluded = false;
    bool isToolModProcess = false;
    bool isCurrentToolModProcess = false;
    int argc;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLine(), &argc);
    if (!argv) return FALSE;

    for (int i = 1; i < argc; i++) {
        if (wcscmp(argv[i], L"-service") == 0 || wcscmp(argv[i], L"-service-start") == 0 || wcscmp(argv[i], L"-service-stop") == 0) {
            isExcluded = true;
            break;
        }
    }

    for (int i = 1; i < argc - 1; i++) {
        if (wcscmp(argv[i], L"-tool-mod") == 0) {
            isToolModProcess = true;
            if (wcscmp(argv[i + 1], WH_MOD_ID) == 0) isCurrentToolModProcess = true;
            break;
        }
    }

    LocalFree(argv);
    if (isExcluded) return FALSE;

    if (isCurrentToolModProcess) {
        g_toolModProcessMutex = CreateMutex(nullptr, TRUE, L"windhawk-tool-mod_" WH_MOD_ID);
        if (!g_toolModProcessMutex || GetLastError() == ERROR_ALREADY_EXISTS) ExitProcess(1);
        if (!WhTool_ModInit()) ExitProcess(1);

        IMAGE_DOS_HEADER* dosHeader = (IMAGE_DOS_HEADER*)GetModuleHandle(nullptr);
        IMAGE_NT_HEADERS* ntHeaders = (IMAGE_NT_HEADERS*)((BYTE*)dosHeader + dosHeader->e_lfanew);
        void* entryPoint = (BYTE*)dosHeader + ntHeaders->OptionalHeader.AddressOfEntryPoint;

        Wh_SetFunctionHook(entryPoint, (void*)EntryPoint_Hook, nullptr);
        return TRUE;
    }

    if (isToolModProcess) return FALSE;

    g_isToolModProcessLauncher = true;
    return TRUE;
}

void Wh_ModAfterInit() {
    if (!g_isToolModProcessLauncher) return;

    WCHAR currentProcessPath[MAX_PATH];
    GetModuleFileName(nullptr, currentProcessPath, MAX_PATH);

    WCHAR commandLine[MAX_PATH + 100];
    swprintf_s(commandLine, L"\"%s\" -tool-mod \"%s\"", currentProcessPath, WH_MOD_ID);

    STARTUPINFO si = { sizeof(si) };
    si.dwFlags = STARTF_FORCEOFFFEEDBACK;
    PROCESS_INFORMATION pi;
    
    // Using standard CreateProcess for reliability in Tool Mode
    if (CreateProcess(currentProcessPath, commandLine, nullptr, nullptr, FALSE, NORMAL_PRIORITY_CLASS, nullptr, nullptr, &si, &pi)) {
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }
}

void Wh_ModSettingsChanged() {
    if (!g_isToolModProcessLauncher) WhTool_ModSettingsChanged();
}

void Wh_ModUninit() {
    if (g_isToolModProcessLauncher) return;
    WhTool_ModUninit();
    ExitProcess(0);
}
