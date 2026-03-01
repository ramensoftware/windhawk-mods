// ==WindhawkMod==
// @id              native-brightness-v1
// @name            Brightness Shortcut Key (Always On Top)
// @description     Shortcut Key to Change the Brightness.Use CTRL + ALT + UP/DOWM to increase or decrease the brightness natively for the system.
// @version         1.0
// @author          Prash
// @include         explorer.exe
// @github	    https://github.com/prasmit2410
// @compilerOptions -lole32 -loleaut32 -lwbemuuid -lgdi32
// @include         mspaint.exe
// @license         MIT
// @include notepad.exe
// @include program-1.*.exe
// @include C:\programs\*.exe
// @include %SystemRoot%\explorer.exe
// @architecture x86
// @architecture x86-64
// ==/WindhawkMod==

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

// OSD Variables
HWND hOSD = NULL;
int displayBrightness = 0;
const wchar_t* OSD_CLASS_NAME = L"WhBrightnessOSD";

// ---------------------------------------------------------------------------
// 1. OSD (On Screen Display) Logic
// ---------------------------------------------------------------------------
LRESULT CALLBACK OSDWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        
        RECT rect;
        GetClientRect(hwnd, &rect);

        // --- Draw Background (Rounded Off-White) ---
        HBRUSH hBrush = CreateSolidBrush(RGB(245, 245, 245)); 
        HPEN hPen = (HPEN)GetStockObject(NULL_PEN);
        
        HGDIOBJ oldBrush = SelectObject(hdc, hBrush);
        HGDIOBJ oldPen = SelectObject(hdc, hPen);
        
        RoundRect(hdc, rect.left, rect.top, rect.right, rect.bottom, 24, 24);

        SelectObject(hdc, oldBrush);
        SelectObject(hdc, oldPen);
        DeleteObject(hBrush);
        
        // --- Text Settings ---
        SetBkMode(hdc, TRANSPARENT);
        SetTextColor(hdc, RGB(30, 30, 30));

        // --- Draw Icon (Left) ---
        HFONT hIconFont = CreateFont(32, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, 
                                 OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, 
                                 DEFAULT_PITCH | FF_SWISS, L"Segoe MDL2 Assets");
        SelectObject(hdc, hIconFont);
        
        RECT iconRect = rect;
        iconRect.right = 70;
        DrawText(hdc, L"\uE706", -1, &iconRect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
        DeleteObject(hIconFont);

        // --- Draw Percentage (Right) ---
        HFONT hTextFont = CreateFont(28, 0, 0, 0, FW_SEMIBOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, 
                                 OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, 
                                 DEFAULT_PITCH | FF_SWISS, L"Segoe UI");
        SelectObject(hdc, hTextFont);

        wchar_t text[16];
        swprintf(text, 16, L"%d%%", displayBrightness);
        
        RECT textRect = rect;
        textRect.left = 60;
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

    // Create window with TOPMOST flag initially
    hOSD = CreateWindowEx(
        WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_LAYERED, 
        OSD_CLASS_NAME, L"", 
        WS_POPUP, 
        0, 0, 180, 60, 
        NULL, NULL, wc.hInstance, NULL
    );

    if (hOSD) {
        HRGN hRgn = CreateRoundRectRgn(0, 0, 180, 60, 24, 24);
        SetWindowRgn(hOSD, hRgn, TRUE);
        SetLayeredWindowAttributes(hOSD, 0, 245, LWA_ALPHA);
    }
}

void ShowOSD(int level) {
    displayBrightness = level;
    
    if (hOSD) {
        int screenW = GetSystemMetrics(SM_CXSCREEN);
        int screenH = GetSystemMetrics(SM_CYSCREEN);
        int winW = 180;
        int winH = 60;
        int x = (screenW - winW) / 2;
        int y = screenH - 160; 

        // 1. Force visual update first
        InvalidateRect(hOSD, NULL, TRUE);

        // 2. AGGRESSIVE SHOW: Show, NoActivate, and Force Topmost Z-Order
        // We call SetWindowPos with HWND_TOPMOST every single time to ensure it jumps over other windows.
        SetWindowPos(hOSD, HWND_TOPMOST, x, y, winW, winH, 
            SWP_SHOWWINDOW | SWP_NOACTIVATE | SWP_NOOWNERZORDER);

        // 3. Reset the "Hide" timer
        SetTimer(hOSD, 1, 1500, NULL); 
    }
}

// ---------------------------------------------------------------------------
// 2. WMI Logic
// ---------------------------------------------------------------------------
void InitWMI() {
    HRESULT hres;
    hres = CoInitializeEx(0, COINIT_MULTITHREADED);
    hres = CoInitializeSecurity(NULL, -1, NULL, NULL, RPC_C_AUTHN_LEVEL_DEFAULT, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE, NULL);
    hres = CoCreateInstance(CLSID_WbemLocator, 0, CLSCTX_INPROC_SERVER, IID_IWbemLocator, (LPVOID *)&pLoc);
    if (FAILED(hres)) return;

    BSTR namespacePath = SysAllocString(L"ROOT\\WMI");
    hres = pLoc->ConnectServer(namespacePath, NULL, NULL, 0, NULL, 0, 0, &pSvc);
    SysFreeString(namespacePath);

    if (SUCCEEDED(hres)) {
        CoSetProxyBlanket(pSvc, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, NULL, RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE);
        wmiInitialized = true;
    }
}

void ChangeBrightness(int offset) {
    if (!wmiInitialized || !pSvc) return;

    IEnumWbemClassObject* pEnumerator = NULL;
    BSTR lang = SysAllocString(L"WQL");
    BSTR query = SysAllocString(L"SELECT * FROM WmiMonitorBrightness"); 
    HRESULT hres = pSvc->ExecQuery(lang, query, WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, NULL, &pEnumerator);
    SysFreeString(lang); SysFreeString(query);

    if (FAILED(hres)) return;

    IWbemClassObject *pclsObj = NULL;
    ULONG uReturn = 0;
    int currentBrightness = 0;

    if (pEnumerator) {
        pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn);
        if (uReturn != 0) {
            VARIANT vtProp; VariantInit(&vtProp);
            pclsObj->Get(L"CurrentBrightness", 0, &vtProp, 0, 0);
            currentBrightness = vtProp.uintVal;
            VariantClear(&vtProp);
            pclsObj->Release();
        }
        pEnumerator->Release();
    }

    int newBrightness = currentBrightness + offset;
    if (newBrightness > 100) newBrightness = 100;
    if (newBrightness < 0) newBrightness = 0;
    
    // UPDATE VISUALS IMMEDIATELY
    ShowOSD(newBrightness);

    if (newBrightness == currentBrightness) return;

    BSTR methodQueryStr = SysAllocString(L"SELECT * FROM WmiMonitorBrightnessMethods");
    BSTR langWQL = SysAllocString(L"WQL");
    hres = pSvc->ExecQuery(langWQL, methodQueryStr, WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, NULL, &pEnumerator);
    SysFreeString(methodQueryStr); SysFreeString(langWQL);

    if (pEnumerator) {
        pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn);
        if (uReturn != 0) {
            VARIANT vtPath; VariantInit(&vtPath);
            pclsObj->Get(L"__PATH", 0, &vtPath, 0, 0);
            
            BSTR className = SysAllocString(L"WmiMonitorBrightnessMethods");
            BSTR methodName = SysAllocString(L"WmiSetBrightness");

            IWbemClassObject* pClass = NULL;
            IWbemClassObject* pInParamsDefinition = NULL;
            IWbemClassObject* pClassInstance = NULL;
            
            pSvc->GetObject(className, 0, NULL, &pClass, NULL);
            pClass->GetMethod(methodName, 0, &pInParamsDefinition, NULL);
            pInParamsDefinition->SpawnInstance(0, &pClassInstance);

            VARIANT var1; VariantInit(&var1); var1.vt = VT_I4; var1.lVal = 0; 
            pClassInstance->Put(L"Timeout", 0, &var1, 0);

            VARIANT var2; VariantInit(&var2); var2.vt = VT_UI1; var2.bVal = (BYTE)newBrightness; 
            pClassInstance->Put(L"Brightness", 0, &var2, 0);

            pSvc->ExecMethod(vtPath.bstrVal, methodName, 0, NULL, pClassInstance, NULL, NULL);

            SysFreeString(className); SysFreeString(methodName);
            VariantClear(&vtPath);
            pClass->Release(); pInParamsDefinition->Release(); pClassInstance->Release(); pclsObj->Release();
        }
        pEnumerator->Release();
    }
}

// ---------------------------------------------------------------------------
// 3. Hooks & Threads
// ---------------------------------------------------------------------------
LRESULT CALLBACK KeyboardHook(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION && (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN)) {
        KBDLLHOOKSTRUCT* p = (KBDLLHOOKSTRUCT*)lParam;
        bool ctrl = GetAsyncKeyState(VK_CONTROL) & 0x8000;
        bool alt = GetAsyncKeyState(VK_MENU) & 0x8000;

        if (ctrl && alt) {
            if (p->vkCode == VK_UP) {
                ChangeBrightness(10);
                return 1;
            }
            if (p->vkCode == VK_DOWN) {
                ChangeBrightness(-10);
                return 1;
            }
        }
    }
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

DWORD WINAPI ThreadMain(LPVOID) {
    InitWMI();
    InitOSD();
    
    hookThreadId = GetCurrentThreadId();
    hHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardHook, NULL, 0);
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    
    if (hHook) UnhookWindowsHookEx(hHook);
    if (pSvc) pSvc->Release();
    if (pLoc) pLoc->Release();
    CoUninitialize();
    return 0;
}

BOOL Wh_ModInit() {
    CreateThread(NULL, 0, ThreadMain, NULL, 0, NULL);
    return TRUE;
}

void Wh_ModUninit() {
    if (hookThreadId) PostThreadMessage(hookThreadId, WM_QUIT, 0, 0);
}



