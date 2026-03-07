// ==WindhawkMod==
// @id              native-brightness
// @name            Brightness Shortcut Key (Always On Top)
// @description     Shortcut Key to Change the Brightness. Use CTRL + ALT + UP/DOWN to increase or decrease the brightness natively for the system.
// @version         1.0
// @author          Prash
// @include         windhawk.exe
// @github          https://github.com/prasmit2410
// @compilerOptions -lole32 -loleaut32 -lwbemuuid -lgdi32
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Brightness Control with OSD

A lightweight tool to control your monitor's brightness using keyboard shortcuts, featuring a modern, rounded On-Screen Display (OSD).

### Key Features:
* **Stability:** Runs in its own dedicated process (`windhawk.exe`), keeping your Windows Explorer safe.
* **Modern UI:** A clean, centered OSD inspired by Windows 11 aesthetics.
* **Universal Shortcuts:** Uses global hooks to work from any app or the desktop.

### How to Use:
* **Ctrl + Alt + Up Arrow:** Increase brightness (+10%)
* **Ctrl + Alt + Down Arrow:** Decrease brightness (-10%)

*Note: Requires a monitor that supports WMI/DDC (standard on most laptops and modern displays).*
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
bool wmiInitialized = false;
HHOOK hHook = NULL;
DWORD hookThreadId = 0;
HWND hOSD = NULL;
int displayBrightness = 0;
const wchar_t* OSD_CLASS_NAME = L"WhBrightnessOSD";

// ---------------------------------------------------------------------------
// 1. OSD (On Screen Display) UI Logic
// ---------------------------------------------------------------------------
LRESULT CALLBACK OSDWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        RECT rect;
        GetClientRect(hwnd, &rect);

        // Background
        HBRUSH hBrush = CreateSolidBrush(RGB(245, 245, 245)); 
        HPEN hPen = (HPEN)GetStockObject(NULL_PEN);
        HGDIOBJ oldBrush = SelectObject(hdc, hBrush);
        HGDIOBJ oldPen = SelectObject(hdc, hPen);
        RoundRect(hdc, rect.left, rect.top, rect.right, rect.bottom, 24, 24);
        SelectObject(hdc, oldBrush); SelectObject(hdc, oldPen);
        DeleteObject(hBrush);

        SetBkMode(hdc, TRANSPARENT);
        SetTextColor(hdc, RGB(30, 30, 30));

        // Icon (Sun)
        HFONT hIconFont = CreateFont(32, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, 0, 0, DEFAULT_QUALITY, 0, L"Segoe MDL2 Assets");
        SelectObject(hdc, hIconFont);
        RECT iconRect = rect; iconRect.right = 70;
        DrawText(hdc, L"\uE706", -1, &iconRect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
        DeleteObject(hIconFont);

        // Percentage Text
        HFONT hTextFont = CreateFont(28, 0, 0, 0, FW_SEMIBOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, 0, 0, DEFAULT_QUALITY, 0, L"Segoe UI");
        SelectObject(hdc, hTextFont);
        wchar_t text[16]; swprintf(text, 16, L"%d%%", displayBrightness);
        RECT textRect = rect; textRect.left = 60;
        DrawText(hdc, text, -1, &textRect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
        DeleteObject(hTextFont);

        EndPaint(hwnd, &ps);
        return 0;
    }
    case WM_TIMER:
        ShowWindow(hwnd, SW_HIDE);
        KillTimer(hwnd, 1);
        return 0;
    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
}

void InitOSD() {
    WNDCLASS wc = { 0 };
    wc.lpfnWndProc = OSDWndProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = OSD_CLASS_NAME;
    RegisterClass(&wc);

    hOSD = CreateWindowEx(WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_LAYERED, OSD_CLASS_NAME, L"", WS_POPUP, 0, 0, 180, 60, NULL, NULL, wc.hInstance, NULL);
    if (hOSD) {
        HRGN hRgn = CreateRoundRectRgn(0, 0, 180, 60, 24, 24);
        SetWindowRgn(hOSD, hRgn, TRUE);
        SetLayeredWindowAttributes(hOSD, 0, 245, LWA_ALPHA);
    }
}

void ShowOSD(int level) {
    displayBrightness = level;
    if (hOSD) {
        int x = (GetSystemMetrics(SM_CXSCREEN) - 180) / 2;
        int y = GetSystemMetrics(SM_CYSCREEN) - 160; 
        InvalidateRect(hOSD, NULL, TRUE);
        SetWindowPos(hOSD, HWND_TOPMOST, x, y, 180, 60, SWP_SHOWWINDOW | SWP_NOACTIVATE);
        SetTimer(hOSD, 1, 1500, NULL); 
    }
}

// ---------------------------------------------------------------------------
// 2. Hardware/WMI Logic (Full 0-100% Range Fix)
// ---------------------------------------------------------------------------
void InitWMI() {
    CoInitializeEx(0, COINIT_MULTITHREADED);
    CoInitializeSecurity(NULL, -1, NULL, NULL, RPC_C_AUTHN_LEVEL_DEFAULT, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE, NULL);
    if (SUCCEEDED(CoCreateInstance(CLSID_WbemLocator, 0, CLSCTX_INPROC_SERVER, IID_IWbemLocator, (LPVOID *)&pLoc))) {
        BSTR ns = SysAllocString(L"ROOT\\WMI");
        if (SUCCEEDED(pLoc->ConnectServer(ns, NULL, NULL, 0, NULL, 0, 0, &pSvc))) {
            CoSetProxyBlanket(pSvc, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, NULL, RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE);
            wmiInitialized = true;
        }
        SysFreeString(ns);
    }
}

void ChangeBrightness(int offset) {
    if (!wmiInitialized || !pSvc) return;

    IEnumWbemClassObject* pEnum = NULL;
    BSTR query = SysAllocString(L"SELECT * FROM WmiMonitorBrightness");
    if (SUCCEEDED(pSvc->ExecQuery(SysAllocString(L"WQL"), query, 48, NULL, &pEnum))) {
        IWbemClassObject *pObj = NULL; ULONG ret = 0;
        pEnum->Next(WBEM_INFINITE, 1, &pObj, &ret);
        if (ret) {
            VARIANT v; VariantInit(&v);
            pObj->Get(L"CurrentBrightness", 0, &v, 0, 0);
            int currentB = (v.vt == VT_UI1) ? v.bVal : v.uintVal;
            
            int newB = currentB + offset;
            if (newB > 100) newB = 100; if (newB < 0) newB = 0;
            
            ShowOSD(newB); // Update UI
            VariantClear(&v); pObj->Release();

            // Apply to hardware
            IEnumWbemClassObject* pEnumMethods = NULL;
            BSTR methodQuery = SysAllocString(L"SELECT * FROM WmiMonitorBrightnessMethods");
            if (SUCCEEDED(pSvc->ExecQuery(SysAllocString(L"WQL"), methodQuery, 48, NULL, &pEnumMethods))) {
                IWbemClassObject *pMethodObj = NULL; ULONG retM = 0;
                pEnumMethods->Next(WBEM_INFINITE, 1, &pMethodObj, &retM);
                if (retM) {
                    VARIANT vtPath; VariantInit(&vtPath);
                    pMethodObj->Get(L"__PATH", 0, &vtPath, 0, 0);
                    
                    IWbemClassObject* pInDef = NULL; IWbemClassObject* pInInst = NULL;
                    pSvc->GetObject(SysAllocString(L"WmiMonitorBrightnessMethods"), 0, NULL, &pInDef, NULL);
                    pInDef->GetMethod(SysAllocString(L"WmiSetBrightness"), 0, &pInDef, NULL);
                    pInDef->SpawnInstance(0, &pInInst);

                    VARIANT vTimeout; vTimeout.vt = VT_I4; vTimeout.lVal = 0;
                    pInInst->Put(L"Timeout", 0, &vTimeout, 0);
                    VARIANT vBright; vBright.vt = VT_UI1; vBright.bVal = (BYTE)newB;
                    pInInst->Put(L"Brightness", 0, &vBright, 0);

                    pSvc->ExecMethod(vtPath.bstrVal, SysAllocString(L"WmiSetBrightness"), 0, NULL, pInInst, NULL, NULL);
                    
                    VariantClear(&vtPath); pMethodObj->Release();
                }
                pEnumMethods->Release();
            }
            SysFreeString(methodQuery);
        }
        pEnum->Release();
    }
    SysFreeString(query);
}

// ---------------------------------------------------------------------------
// 3. Threading & Hooks
// ---------------------------------------------------------------------------
LRESULT CALLBACK KeyboardHook(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION && (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN)) {
        KBDLLHOOKSTRUCT* p = (KBDLLHOOKSTRUCT*)lParam;
        if ((GetAsyncKeyState(VK_CONTROL) & 0x8000) && (GetAsyncKeyState(VK_MENU) & 0x8000)) {
            if (p->vkCode == VK_UP) { ChangeBrightness(10); return 1; }
            if (p->vkCode == VK_DOWN) { ChangeBrightness(-10); return 1; }
        }
    }
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

DWORD WINAPI ThreadMain(LPVOID) {
    InitWMI(); InitOSD();
    hookThreadId = GetCurrentThreadId();
    hHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardHook, NULL, 0);
    MSG msg; while (GetMessage(&msg, NULL, 0, 0)) { TranslateMessage(&msg); DispatchMessage(&msg); }
    if (hHook) UnhookWindowsHookEx(hHook);
    if (pSvc) pSvc->Release(); if (pLoc) pLoc->Release();
    CoUninitialize(); return 0;
}

// ---------------------------------------------------------------------------
// 4. Windhawk Tool Callbacks
// ---------------------------------------------------------------------------
BOOL WhTool_ModInit() {
    CreateThread(NULL, 0, ThreadMain, NULL, 0, NULL);
    return TRUE;
}

void WhTool_ModUninit() {
    if (hookThreadId) PostThreadMessage(hookThreadId, WM_QUIT, 0, 0);
}

void WhTool_ModSettingsChanged() {}

// ---------------------------------------------------------------------------
// 5. OFFICIAL WINDHAWK TOOL BOILERPLATE (DO NOT EDIT)
// ---------------------------------------------------------------------------
bool g_isToolModProcessLauncher;
HANDLE g_toolModProcessMutex;

void WINAPI EntryPoint_Hook() { ExitThread(0); }

BOOL Wh_ModInit() {
    bool isService = false, isToolModProcess = false, isCurrentToolModProcess = false;
    int argc; LPWSTR* argv = CommandLineToArgvW(GetCommandLine(), &argc);
    if (!argv) return FALSE;
    for (int i = 1; i < argc; i++) { if (wcscmp(argv[i], L"-service") == 0) isService = true; }
    for (int i = 1; i < argc - 1; i++) {
        if (wcscmp(argv[i], L"-tool-mod") == 0) {
            isToolModProcess = true;
            if (wcscmp(argv[i + 1], WH_MOD_ID) == 0) isCurrentToolModProcess = true;
        }
    }
    LocalFree(argv);
    if (isService) return FALSE;
    if (isCurrentToolModProcess) {
        g_toolModProcessMutex = CreateMutex(nullptr, TRUE, L"windhawk-tool-mod_" WH_MOD_ID);
        if (!g_toolModProcessMutex || GetLastError() == ERROR_ALREADY_EXISTS) ExitProcess(1);
        if (!WhTool_ModInit()) ExitProcess(1);
        IMAGE_DOS_HEADER* dos = (IMAGE_DOS_HEADER*)GetModuleHandle(nullptr);
        IMAGE_NT_HEADERS* nt = (IMAGE_NT_HEADERS*)((BYTE*)dos + dos->e_lfanew);
        Wh_SetFunctionHook((BYTE*)dos + nt->OptionalHeader.AddressOfEntryPoint, (void*)EntryPoint_Hook, nullptr);
        return TRUE;
    }
    if (isToolModProcess) return FALSE;
    g_isToolModProcessLauncher = true;
    return TRUE;
}

void Wh_ModAfterInit() {
    if (!g_isToolModProcessLauncher) return;
    WCHAR path[MAX_PATH]; GetModuleFileName(nullptr, path, MAX_PATH);
    WCHAR cmd[MAX_PATH + 100]; swprintf_s(cmd, L"\"%s\" -tool-mod \"%s\"", path, WH_MOD_ID);
    HMODULE k = GetModuleHandle(L"kernelbase.dll"); if (!k) k = GetModuleHandle(L"kernel32.dll");
    typedef BOOL(WINAPI* CPIW)(HANDLE, LPCWSTR, LPWSTR, LPSECURITY_ATTRIBUTES, LPSECURITY_ATTRIBUTES, BOOL, DWORD, LPVOID, LPCWSTR, LPSTARTUPINFOW, LPPROCESS_INFORMATION, PHANDLE);
    CPIW p = (CPIW)GetProcAddress(k, "CreateProcessInternalW");
    STARTUPINFO si{ .cb = sizeof(si), .dwFlags = STARTF_FORCEOFFFEEDBACK }; PROCESS_INFORMATION pi;
    if (p(nullptr, path, cmd, nullptr, nullptr, FALSE, NORMAL_PRIORITY_CLASS, nullptr, nullptr, &si, &pi, nullptr)) {
        CloseHandle(pi.hProcess); CloseHandle(pi.hThread);
    }
}

void Wh_ModSettingsChanged() { if (!g_isToolModProcessLauncher) WhTool_ModSettingsChanged(); }
void Wh_ModUninit() { if (!g_isToolModProcessLauncher) { WhTool_ModUninit(); ExitProcess(0); } }

