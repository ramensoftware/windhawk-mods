// ==WindhawkMod==
// @id              taskbar-scroll-actions
// @name            Taskbar Scroll Actions
// @description     Assign actions for scrolling over the taskbar, including virtual desktop switching and monitor brightness control
// @version         1.1
// @author          m417z
// @github          https://github.com/m417z
// @twitter         https://twitter.com/m417z
// @homepage        https://m417z.com/
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lcomctl32 -lgdi32 -lole32 -loleaut32 -lversion
// ==/WindhawkMod==

// Source code is published under The GNU General Public License v3.0.
//
// For bug reports and feature requests, please open an issue here:
// https://github.com/ramensoftware/windhawk-mods/issues
//
// For pull requests, development takes place here:
// https://github.com/m417z/my-windhawk-mods

// ==WindhawkModReadme==
/*
# Taskbar Scroll Actions

Assign actions for scrolling over the taskbar, including virtual desktop
switching and monitor brightness control.

Currently, the following actions are supported:

* Switch virtual desktop
* Change monitor brightness

Also check out the following related mods:

* Taskbar Volume Control
* Cycle taskbar buttons with mouse wheel

**Note:** Some laptop touchpads might not support scrolling over the taskbar. A
workaround is to use the "pinch to zoom" gesture. For details, check out [a
relevant
issue](https://tweaker.userecho.com/topics/826-scroll-on-trackpadtouchpad-doesnt-trigger-mouse-wheel-options).
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- scrollAction: virtualDesktopSwitch
  $name: Scroll action
  $options:
  - virtualDesktopSwitch: Switch virtual desktop
  - brightnessChange: Change monitor brightness
  - micVolumeChange: Change microphone volume
- scrollArea: taskbar
  $name: Scroll area
  $options:
  - taskbar: The taskbar
  - notificationArea: The tray area
  - taskbarWithoutNotificationArea: The taskbar without the tray area
- scrollStep: 1
  $name: Scroll step
  $description: >-
    Allows to configure the change that will occur with each notch of mouse
    wheel movement.
- throttleMs: 0
  $name: Throttle time (milliseconds)
  $description: >-
    Prevents new actions from being triggered for this amount of time after the
    last one. Set to 0 to disable throttling. Useful for preventing a single
    scroll wheel 'flick' from switching multiple desktops.
- reverseScrollingDirection: false
  $name: Reverse scrolling direction
- oldTaskbarOnWin11: false
  $name: Customize the old taskbar on Windows 11
  $description: >-
    Enable this option to customize the old taskbar on Windows 11 (if using
    ExplorerPatcher or a similar tool).
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <initguid.h>

#include <combaseapi.h>
#include <commctrl.h>
#include <comutil.h>
#include <endpointvolume.h>
#include <mmdeviceapi.h>
#include <psapi.h>
#include <wbemcli.h>
#include <windowsx.h>

#include <unordered_set>

enum class ScrollAction {
    virtualDesktopSwitch,
    brightnessChange,
    micVolumeChange,
};

enum class ScrollArea {
    taskbar,
    notificationArea,
    taskbarWithoutNotificationArea,
};

struct {
    ScrollAction scrollAction;
    ScrollArea scrollArea;
    int scrollStep;
    int throttleMs;
    bool reverseScrollingDirection;
    bool oldTaskbarOnWin11;
} g_settings;

bool g_initialized = false;
bool g_inputSiteProcHooked = false;
std::unordered_set<HWND> g_secondaryTaskbarWindows;

enum {
    WIN_VERSION_UNSUPPORTED = 0,
    WIN_VERSION_7,
    WIN_VERSION_8,
    WIN_VERSION_81,
    WIN_VERSION_811,
    WIN_VERSION_10_T1,        // 1507
    WIN_VERSION_10_T2,        // 1511
    WIN_VERSION_10_R1,        // 1607
    WIN_VERSION_10_R2,        // 1703
    WIN_VERSION_10_R3,        // 1709
    WIN_VERSION_10_R4,        // 1803
    WIN_VERSION_10_R5,        // 1809
    WIN_VERSION_10_19H1,      // 1903, 1909
    WIN_VERSION_10_20H1,      // 2004, 20H2, 21H1, 21H2
    WIN_VERSION_SERVER_2022,  // Server 2022
    WIN_VERSION_11_21H2,
    WIN_VERSION_11_22H2,
};

#ifndef WM_POINTERWHEEL
#define WM_POINTERWHEEL 0x024E
#endif

int g_nWinVersion;
int g_nExplorerVersion;
HWND g_hTaskbarWnd;
DWORD g_dwTaskbarThreadId;

#pragma region functions

UINT GetDpiForWindowWithFallback(HWND hWnd) {
    using GetDpiForWindow_t = UINT(WINAPI*)(HWND hwnd);
    static GetDpiForWindow_t pGetDpiForWindow = []() {
        HMODULE hUser32 = GetModuleHandle(L"user32.dll");
        if (hUser32) {
            return (GetDpiForWindow_t)GetProcAddress(hUser32,
                                                     "GetDpiForWindow");
        }

        return (GetDpiForWindow_t) nullptr;
    }();

    int iDpi = 96;
    if (pGetDpiForWindow) {
        iDpi = pGetDpiForWindow(hWnd);
    } else {
        HDC hdc = GetDC(NULL);
        if (hdc) {
            iDpi = GetDeviceCaps(hdc, LOGPIXELSX);
            ReleaseDC(NULL, hdc);
        }
    }

    return iDpi;
}

bool IsTaskbarWindow(HWND hWnd) {
    WCHAR szClassName[32];
    if (!GetClassName(hWnd, szClassName, ARRAYSIZE(szClassName))) {
        return false;
    }

    return _wcsicmp(szClassName, L"Shell_TrayWnd") == 0 ||
           _wcsicmp(szClassName, L"Shell_SecondaryTrayWnd") == 0;
}

HWND FindCurrentProcessTaskbarWnd() {
    HWND hTaskbarWnd = nullptr;

    EnumWindows(
        [](HWND hWnd, LPARAM lParam) -> BOOL {
            DWORD dwProcessId;
            WCHAR className[32];
            if (GetWindowThreadProcessId(hWnd, &dwProcessId) &&
                dwProcessId == GetCurrentProcessId() &&
                GetClassName(hWnd, className, ARRAYSIZE(className)) &&
                _wcsicmp(className, L"Shell_TrayWnd") == 0) {
                *reinterpret_cast<HWND*>(lParam) = hWnd;
                return FALSE;
            }
            return TRUE;
        },
        reinterpret_cast<LPARAM>(&hTaskbarWnd));

    return hTaskbarWnd;
}

bool GetNotificationAreaRect(HWND hMMTaskbarWnd, RECT* rcResult) {
    if (hMMTaskbarWnd == g_hTaskbarWnd) {
        HWND hTrayNotifyWnd =
            FindWindowEx(hMMTaskbarWnd, NULL, L"TrayNotifyWnd", NULL);
        if (hTrayNotifyWnd && GetWindowRect(hTrayNotifyWnd, rcResult) &&
            !IsRectEmpty(rcResult)) {
            return true;
        }

        // When attaching an external monitor, it was observed that the rect can
        // be empty. Use fallback in this case.
    } else if (g_nExplorerVersion >= WIN_VERSION_11_21H2) {
        RECT rcTaskbar;
        if (GetWindowRect(hMMTaskbarWnd, &rcTaskbar)) {
            HWND hBridgeWnd = FindWindowEx(
                hMMTaskbarWnd, NULL,
                L"Windows.UI.Composition.DesktopWindowContentBridge", NULL);
            while (hBridgeWnd) {
                RECT rcBridge;
                if (!GetWindowRect(hBridgeWnd, &rcBridge)) {
                    break;
                }

                if (!EqualRect(&rcBridge, &rcTaskbar)) {
                    if (IsRectEmpty(&rcBridge)) {
                        break;
                    }

                    CopyRect(rcResult, &rcBridge);
                    return true;
                }

                hBridgeWnd = FindWindowEx(
                    hMMTaskbarWnd, hBridgeWnd,
                    L"Windows.UI.Composition.DesktopWindowContentBridge", NULL);
            }
        }

        // On newer Win11 versions, the clock on secondary taskbars is difficult
        // to detect without either UI Automation or UWP UI APIs. Use fallback.
    } else if (g_nExplorerVersion >= WIN_VERSION_10_R1) {
        HWND hClockButtonWnd =
            FindWindowEx(hMMTaskbarWnd, NULL, L"ClockButton", NULL);
        if (hClockButtonWnd && GetWindowRect(hClockButtonWnd, rcResult) &&
            !IsRectEmpty(rcResult)) {
            return true;
        }
    } else {
        // In older Windows versions, there's no clock on the secondary taskbar.
        SetRectEmpty(rcResult);
        return true;
    }

    RECT rcTaskbar;
    if (!GetWindowRect(hMMTaskbarWnd, &rcTaskbar)) {
        return false;
    }

    // Just consider the last pixels as a fallback, not accurate, but better
    // than nothing.
    int lastPixels = MulDiv(50, GetDpiForWindowWithFallback(hMMTaskbarWnd), 96);
    CopyRect(rcResult, &rcTaskbar);
    if (rcResult->right - rcResult->left > lastPixels) {
        if (GetWindowLong(hMMTaskbarWnd, GWL_EXSTYLE) & WS_EX_LAYOUTRTL) {
            rcResult->right = rcResult->left + lastPixels;
        } else {
            rcResult->left = rcResult->right - lastPixels;
        }
    }

    return true;
}

bool IsPointInsideScrollArea(HWND hMMTaskbarWnd, POINT pt) {
    switch (g_settings.scrollArea) {
        case ScrollArea::taskbar: {
            RECT rc;
            return GetWindowRect(hMMTaskbarWnd, &rc) && PtInRect(&rc, pt);
        }

        case ScrollArea::notificationArea: {
            RECT rc;
            return GetNotificationAreaRect(hMMTaskbarWnd, &rc) &&
                   PtInRect(&rc, pt);
        }

        case ScrollArea::taskbarWithoutNotificationArea: {
            RECT rc;
            return GetWindowRect(hMMTaskbarWnd, &rc) && PtInRect(&rc, pt) &&
                   (!GetNotificationAreaRect(hMMTaskbarWnd, &rc) ||
                    !PtInRect(&rc, pt));
        }
    }

    return false;
}

VS_FIXEDFILEINFO* GetModuleVersionInfo(HMODULE hModule, UINT* puPtrLen) {
    HRSRC hResource;
    HGLOBAL hGlobal;
    void* pData;
    void* pFixedFileInfo;
    UINT uPtrLen;

    pFixedFileInfo = NULL;
    uPtrLen = 0;

    hResource =
        FindResource(hModule, MAKEINTRESOURCE(VS_VERSION_INFO), RT_VERSION);
    if (hResource != NULL) {
        hGlobal = LoadResource(hModule, hResource);
        if (hGlobal != NULL) {
            pData = LockResource(hGlobal);
            if (pData != NULL) {
                if (!VerQueryValue(pData, L"\\", &pFixedFileInfo, &uPtrLen) ||
                    uPtrLen == 0) {
                    pFixedFileInfo = NULL;
                    uPtrLen = 0;
                }
            }
        }
    }

    if (puPtrLen)
        *puPtrLen = uPtrLen;

    return (VS_FIXEDFILEINFO*)pFixedFileInfo;
}

BOOL WindowsVersionInit() {
    g_nWinVersion = WIN_VERSION_UNSUPPORTED;

    VS_FIXEDFILEINFO* pFixedFileInfo = GetModuleVersionInfo(NULL, NULL);
    if (!pFixedFileInfo)
        return FALSE;

    WORD nMajor = HIWORD(pFixedFileInfo->dwFileVersionMS);
    WORD nMinor = LOWORD(pFixedFileInfo->dwFileVersionMS);
    WORD nBuild = HIWORD(pFixedFileInfo->dwFileVersionLS);
    WORD nQFE = LOWORD(pFixedFileInfo->dwFileVersionLS);

    switch (nMajor) {
        case 6:
            switch (nMinor) {
                case 1:
                    g_nWinVersion = WIN_VERSION_7;
                    break;

                case 2:
                    g_nWinVersion = WIN_VERSION_8;
                    break;

                case 3:
                    if (nQFE < 17000)
                        g_nWinVersion = WIN_VERSION_81;
                    else
                        g_nWinVersion = WIN_VERSION_811;
                    break;

                case 4:
                    g_nWinVersion = WIN_VERSION_10_T1;
                    break;
            }
            break;

        case 10:
            if (nBuild <= 10240)
                g_nWinVersion = WIN_VERSION_10_T1;
            else if (nBuild <= 10586)
                g_nWinVersion = WIN_VERSION_10_T2;
            else if (nBuild <= 14393)
                g_nWinVersion = WIN_VERSION_10_R1;
            else if (nBuild <= 15063)
                g_nWinVersion = WIN_VERSION_10_R2;
            else if (nBuild <= 16299)
                g_nWinVersion = WIN_VERSION_10_R3;
            else if (nBuild <= 17134)
                g_nWinVersion = WIN_VERSION_10_R4;
            else if (nBuild <= 17763)
                g_nWinVersion = WIN_VERSION_10_R5;
            else if (nBuild <= 18362)
                g_nWinVersion = WIN_VERSION_10_19H1;
            else if (nBuild <= 19041)
                g_nWinVersion = WIN_VERSION_10_20H1;
            else if (nBuild <= 20348)
                g_nWinVersion = WIN_VERSION_SERVER_2022;
            else if (nBuild <= 22000)
                g_nWinVersion = WIN_VERSION_11_21H2;
            else
                g_nWinVersion = WIN_VERSION_11_22H2;
            break;
    }

    if (g_nWinVersion == WIN_VERSION_UNSUPPORTED)
        return FALSE;

    return TRUE;
}

#pragma endregion  // functions

#pragma region brightness

// Reference:
// https://github.com/stefankueng/tools/blob/e7cd50c6ac3a50f6dac84c6aace519349164155e/Misc/AAClr/src/Utils.cpp

int GetBrightness() {
    int ret = -1;

    IWbemLocator* pLocator = NULL;
    IWbemServices* pNamespace = 0;
    IEnumWbemClassObject* pEnum = NULL;
    HRESULT hr = S_OK;

    BSTR path = SysAllocString(L"root\\wmi");
    BSTR ClassPath = SysAllocString(L"WmiMonitorBrightness");
    BSTR bstrQuery = SysAllocString(L"Select * from WmiMonitorBrightness");

    if (!path || !ClassPath) {
        goto cleanup;
    }

    // Initialize COM and connect up to CIMOM

    hr = CoInitialize(0);
    if (FAILED(hr)) {
        goto cleanup;
    }

    //  NOTE:
    //  When using asynchronous WMI API's remotely in an environment where the
    //  "Local System" account has no network identity (such as non-Kerberos
    //  domains), the authentication level of RPC_C_AUTHN_LEVEL_NONE is needed.
    //  However, lowering the authentication level to RPC_C_AUTHN_LEVEL_NONE
    //  makes your application less secure. It is wise to
    // use semi-synchronous API's for accessing WMI data and events instead of
    // the asynchronous ones.

    hr = CoInitializeSecurity(
        NULL, -1, NULL, NULL, RPC_C_AUTHN_LEVEL_PKT_PRIVACY,
        RPC_C_IMP_LEVEL_IMPERSONATE, NULL,
        EOAC_SECURE_REFS,  // change to EOAC_NONE if you change dwAuthnLevel to
                           // RPC_C_AUTHN_LEVEL_NONE
        NULL);

    hr = CoCreateInstance(CLSID_WbemLocator, 0, CLSCTX_INPROC_SERVER,
                          IID_IWbemLocator, (LPVOID*)&pLocator);
    if (FAILED(hr)) {
        goto cleanup;
    }
    hr = pLocator->ConnectServer(path, NULL, NULL, NULL, 0, NULL, NULL,
                                 &pNamespace);
    if (hr != WBEM_S_NO_ERROR) {
        goto cleanup;
    }

    hr = CoSetProxyBlanket(pNamespace, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE,
                           NULL, RPC_C_AUTHN_LEVEL_PKT,
                           RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE);

    if (hr != WBEM_S_NO_ERROR) {
        goto cleanup;
    }

    hr = pNamespace->ExecQuery(
        _bstr_t(L"WQL"),               // Query Language
        bstrQuery,                     // Query to Execute
        WBEM_FLAG_RETURN_IMMEDIATELY,  // Make a semi-synchronous call
        NULL,                          // Context
        &pEnum                         // Enumeration Interface
    );

    if (hr != WBEM_S_NO_ERROR) {
        goto cleanup;
    }

    hr = WBEM_S_NO_ERROR;

    while (WBEM_S_NO_ERROR == hr) {
        ULONG ulReturned;
        IWbemClassObject* pObj;

        // Get the Next Object from the collection
        hr = pEnum->Next(WBEM_INFINITE,  // Timeout
                         1,              // No of objects requested
                         &pObj,          // Returned Object
                         &ulReturned     // No of object returned
        );

        if (hr != WBEM_S_NO_ERROR) {
            goto cleanup;
        }

        VARIANT var1;
        hr = pObj->Get(_bstr_t(L"CurrentBrightness"), 0, &var1, NULL, NULL);

        ret = V_UI1(&var1);

        VariantClear(&var1);
        if (hr != WBEM_S_NO_ERROR) {
            goto cleanup;
        }
    }

    // Free up resources
cleanup:

    SysFreeString(path);
    SysFreeString(ClassPath);
    SysFreeString(bstrQuery);

    if (pLocator)
        pLocator->Release();
    if (pNamespace)
        pNamespace->Release();

    CoUninitialize();

    return ret;
}

bool SetBrightness(int val) {
    bool bRet = true;

    IWbemLocator* pLocator = NULL;
    IWbemServices* pNamespace = 0;
    IWbemClassObject* pClass = NULL;
    IWbemClassObject* pInClass = NULL;
    IWbemClassObject* pInInst = NULL;
    IEnumWbemClassObject* pEnum = NULL;
    HRESULT hr = S_OK;

    BSTR path = SysAllocString(L"root\\wmi");
    BSTR ClassPath = SysAllocString(L"WmiMonitorBrightnessMethods");
    BSTR MethodName = SysAllocString(L"WmiSetBrightness");
    BSTR ArgName0 = SysAllocString(L"Timeout");
    BSTR ArgName1 = SysAllocString(L"Brightness");
    BSTR bstrQuery =
        SysAllocString(L"Select * from WmiMonitorBrightnessMethods");

    if (!path || !ClassPath || !MethodName || !ArgName0) {
        bRet = false;
        goto cleanup;
    }

    // Initialize COM and connect up to CIMOM

    hr = CoInitialize(0);
    if (FAILED(hr)) {
        bRet = false;
        goto cleanup;
    }

    //  NOTE:
    //  When using asynchronous WMI API's remotely in an environment where the
    //  "Local System" account has no network identity (such as non-Kerberos
    //  domains), the authentication level of RPC_C_AUTHN_LEVEL_NONE is needed.
    //  However, lowering the authentication level to RPC_C_AUTHN_LEVEL_NONE
    //  makes your application less secure. It is wise to
    // use semi-synchronous API's for accessing WMI data and events instead of
    // the asynchronous ones.

    hr = CoInitializeSecurity(
        NULL, -1, NULL, NULL, RPC_C_AUTHN_LEVEL_PKT_PRIVACY,
        RPC_C_IMP_LEVEL_IMPERSONATE, NULL,
        EOAC_SECURE_REFS,  // change to EOAC_NONE if you change dwAuthnLevel to
                           // RPC_C_AUTHN_LEVEL_NONE
        NULL);

    hr = CoCreateInstance(CLSID_WbemLocator, 0, CLSCTX_INPROC_SERVER,
                          IID_IWbemLocator, (LPVOID*)&pLocator);
    if (FAILED(hr)) {
        bRet = false;
        goto cleanup;
    }
    hr = pLocator->ConnectServer(path, NULL, NULL, NULL, 0, NULL, NULL,
                                 &pNamespace);
    if (hr != WBEM_S_NO_ERROR) {
        bRet = false;
        goto cleanup;
    }

    hr = CoSetProxyBlanket(pNamespace, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE,
                           NULL, RPC_C_AUTHN_LEVEL_PKT,
                           RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE);

    if (hr != WBEM_S_NO_ERROR) {
        bRet = false;
        goto cleanup;
    }

    hr = pNamespace->ExecQuery(
        _bstr_t(L"WQL"),               // Query Language
        bstrQuery,                     // Query to Execute
        WBEM_FLAG_RETURN_IMMEDIATELY,  // Make a semi-synchronous call
        NULL,                          // Context
        &pEnum                         // Enumeration Interface
    );

    if (hr != WBEM_S_NO_ERROR) {
        bRet = false;
        goto cleanup;
    }

    hr = WBEM_S_NO_ERROR;

    while (WBEM_S_NO_ERROR == hr) {
        ULONG ulReturned;
        IWbemClassObject* pObj;

        // Get the Next Object from the collection
        hr = pEnum->Next(WBEM_INFINITE,  // Timeout
                         1,              // No of objects requested
                         &pObj,          // Returned Object
                         &ulReturned     // No of object returned
        );

        if (hr != WBEM_S_NO_ERROR) {
            bRet = false;
            goto cleanup;
        }

        // Get the class object
        hr = pNamespace->GetObject(ClassPath, 0, NULL, &pClass, NULL);
        if (hr != WBEM_S_NO_ERROR) {
            bRet = false;
            goto cleanup;
        }

        // Get the input argument and set the property
        hr = pClass->GetMethod(MethodName, 0, &pInClass, NULL);
        if (hr != WBEM_S_NO_ERROR) {
            bRet = false;
            goto cleanup;
        }

        hr = pInClass->SpawnInstance(0, &pInInst);
        if (hr != WBEM_S_NO_ERROR) {
            bRet = false;
            goto cleanup;
        }

        VARIANT var1;
        VariantInit(&var1);

        V_VT(&var1) = VT_BSTR;
        V_BSTR(&var1) = SysAllocString(L"0");
        hr = pInInst->Put(ArgName0, 0, &var1, CIM_UINT32);  // CIM_UINT64

        // var1.vt = VT_I4;
        // var1.ullVal = 0;
        //    hr = pInInst->Put(ArgName0, 0, &var1, 0);
        VariantClear(&var1);
        if (hr != WBEM_S_NO_ERROR) {
            bRet = false;
            goto cleanup;
        }

        VARIANT var;
        VariantInit(&var);

        V_VT(&var) = VT_BSTR;
        WCHAR buf[10] = {0};
        swprintf_s(buf, _countof(buf), L"%d", val);
        V_BSTR(&var) = SysAllocString(buf);
        hr = pInInst->Put(ArgName1, 0, &var, CIM_UINT8);

        // var.vt=VT_UI1;
        // var.uiVal = 100;
        // hr = pInInst->Put(ArgName1, 0, &var, 0);
        VariantClear(&var);
        if (hr != WBEM_S_NO_ERROR) {
            bRet = false;
            goto cleanup;
        }
        // Call the method

        VARIANT pathVariable;
        VariantInit(&pathVariable);

        hr = pObj->Get(_bstr_t(L"__PATH"), 0, &pathVariable, NULL, NULL);
        if (hr != WBEM_S_NO_ERROR)
            goto cleanup;

        hr = pNamespace->ExecMethod(pathVariable.bstrVal, MethodName, 0, NULL,
                                    pInInst, NULL, NULL);
        VariantClear(&pathVariable);
        if (hr != WBEM_S_NO_ERROR) {
            bRet = false;
            goto cleanup;
        }
    }

    // Free up resources
cleanup:

    SysFreeString(path);
    SysFreeString(ClassPath);
    SysFreeString(MethodName);
    SysFreeString(ArgName0);
    SysFreeString(ArgName1);
    SysFreeString(bstrQuery);

    if (pClass)
        pClass->Release();
    if (pInInst)
        pInInst->Release();
    if (pInClass)
        pInClass->Release();
    if (pLocator)
        pLocator->Release();
    if (pNamespace)
        pNamespace->Release();

    CoUninitialize();

    return bRet;
}

#pragma endregion  // brightness

// Use a keyboard simulation and not IVirtualDesktopManagerInternal, since the
// latter switches desktop without an animation. See:
// https://github.com/Ciantic/VirtualDesktopAccessor/issues/29
bool SwitchDesktopViaKeyboardShortcut(int clicks) {
    if (GetKeyState(VK_CONTROL) < 0 || GetKeyState(VK_MENU) < 0 ||
        GetKeyState(VK_SHIFT) < 0 || GetKeyState(VK_PRIOR) < 0 ||
        GetKeyState(VK_NEXT) < 0) {
        return false;
    }

    // To allow to switch if the foreground window is of an elevated process.
    HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();
    if (hTaskbarWnd) {
        SetForegroundWindow(hTaskbarWnd);
    }

    WORD key = VK_LEFT;
    if (clicks < 0) {
        clicks = -clicks;
        key = VK_RIGHT;
    }

    INPUT* input = new INPUT[clicks * 2 + 4];
    for (int i = 0; i < clicks * 2 + 4; i++) {
        input[i].type = INPUT_KEYBOARD;
        input[i].ki.wScan = 0;
        input[i].ki.time = 0;
        input[i].ki.dwExtraInfo = 0;
    }

    input[0].ki.wVk = VK_LWIN;
    input[0].ki.dwFlags = 0;
    input[1].ki.wVk = VK_LCONTROL;
    input[1].ki.dwFlags = 0;

    for (int i = 0; i < clicks; i++) {
        input[2 + i * 2].ki.wVk = key;
        input[2 + i * 2].ki.dwFlags = 0;
        input[2 + i * 2 + 1].ki.wVk = key;
        input[2 + i * 2 + 1].ki.dwFlags = KEYEVENTF_KEYUP;
    }

    input[2 + clicks * 2].ki.wVk = VK_LCONTROL;
    input[2 + clicks * 2].ki.dwFlags = KEYEVENTF_KEYUP;
    input[2 + clicks * 2 + 1].ki.wVk = VK_LWIN;
    input[2 + clicks * 2 + 1].ki.dwFlags = KEYEVENTF_KEYUP;

    SendInput(clicks * 2 + 4, input, sizeof(input[0]));

    delete[] input;

    return true;
}

#pragma region microphone_volume

const static GUID XIID_IMMDeviceEnumerator = {
    0xA95664D2,
    0x9614,
    0x4F35,
    {0xA7, 0x46, 0xDE, 0x8D, 0xB6, 0x36, 0x17, 0xE6}};
const static GUID XIID_MMDeviceEnumerator = {
    0xBCDE0395,
    0xE52F,
    0x467C,
    {0x8E, 0x3D, 0xC4, 0x57, 0x92, 0x91, 0x69, 0x2E}};
const static GUID XIID_IAudioEndpointVolume = {
    0x5CDF2C82,
    0x841E,
    0x4546,
    {0x97, 0x22, 0x0C, 0xF7, 0x40, 0x78, 0x22, 0x9A}};

bool g_bMicVolInitialized;
IMMDeviceEnumerator* g_pDeviceEnumerator;

void MicVolInit() {
    HRESULT hr = CoCreateInstance(
        XIID_MMDeviceEnumerator, NULL, CLSCTX_INPROC_SERVER,
        XIID_IMMDeviceEnumerator, (LPVOID*)&g_pDeviceEnumerator);
    if (FAILED(hr))
        g_pDeviceEnumerator = NULL;
}

void MicVolUninit() {
    if (g_pDeviceEnumerator) {
        g_pDeviceEnumerator->Release();
        g_pDeviceEnumerator = NULL;
    }
}

BOOL AddMicMasterVolumeLevelScalar(float fMasterVolumeAdd) {
    IMMDevice* defaultDevice = NULL;
    IAudioEndpointVolume* endpointVolume = NULL;
    HRESULT hr;
    float fMasterVolume;
    BOOL bSuccess = FALSE;

    if (!g_bMicVolInitialized) {
        MicVolInit();
        g_bMicVolInitialized = true;
    }

    if (g_pDeviceEnumerator) {
        hr = g_pDeviceEnumerator->GetDefaultAudioEndpoint(eCapture, eConsole,
                                                          &defaultDevice);
        if (SUCCEEDED(hr)) {
            hr = defaultDevice->Activate(XIID_IAudioEndpointVolume,
                                         CLSCTX_INPROC_SERVER, NULL,
                                         (LPVOID*)&endpointVolume);
            if (SUCCEEDED(hr)) {
                if (SUCCEEDED(endpointVolume->GetMasterVolumeLevelScalar(
                        &fMasterVolume))) {
                    fMasterVolume += fMasterVolumeAdd;

                    if (fMasterVolume < 0.0)
                        fMasterVolume = 0.0;
                    else if (fMasterVolume > 1.0)
                        fMasterVolume = 1.0;

                    if (SUCCEEDED(endpointVolume->SetMasterVolumeLevelScalar(
                            fMasterVolume, NULL))) {
                        bSuccess = TRUE;
                    }
                }

                endpointVolume->Release();
            }

            defaultDevice->Release();
        }
    }

    return bSuccess;
}

#pragma endregion  // microphone_volume

DWORD g_lastScrollTime;
int g_lastScrollDeltaRemainder;
DWORD g_lastActionTime;

void InvokeScrollAction(WPARAM wParam, LPARAM lMousePosParam) {
    int delta = GET_WHEEL_DELTA_WPARAM(wParam) * g_settings.scrollStep;

    if (g_settings.reverseScrollingDirection) {
        delta = -delta;
    }

    if (GetTickCount() - g_lastScrollTime < 1000 * 5) {
        delta += g_lastScrollDeltaRemainder;
    }

    int clicks = delta / WHEEL_DELTA;
    Wh_Log(L"%d clicks (delta=%d)", clicks, delta);

    if (clicks != 0 && g_settings.throttleMs > 0) {
        if (GetTickCount() - g_lastActionTime < (DWORD)g_settings.throttleMs) {
            // It's too soon, ignore this scroll event.
            clicks = 0;

            // Reset reminder too.
            delta = 0;
        } else if (clicks < -1 || clicks > 1) {
            // Throttle to a single action at a time.
            clicks = clicks > 0 ? 1 : -1;

            // Reset reminder if going too fast.
            delta = 0;
        }
    }

    if (clicks != 0) {
        switch (g_settings.scrollAction) {
            case ScrollAction::virtualDesktopSwitch:
                SwitchDesktopViaKeyboardShortcut(clicks);
                break;

            case ScrollAction::brightnessChange: {
                int brightness = GetBrightness();
                if (brightness != -1) {
                    Wh_Log(L"Changing brightness from %d to %d", brightness,
                           brightness + clicks);
                    SetBrightness(brightness + clicks);
                } else {
                    Wh_Log(L"Error getting current brightness");
                }
                break;
            }

            case ScrollAction::micVolumeChange:
                if (AddMicMasterVolumeLevelScalar(clicks * 0.01f)) {
                    Wh_Log(L"Changed microphone volume by %d%%", clicks);
                } else {
                    Wh_Log(L"Error changing microphone volume");
                }
                break;
        }

        g_lastActionTime = GetTickCount();
    }

    g_lastScrollTime = GetTickCount();
    g_lastScrollDeltaRemainder = delta % WHEEL_DELTA;
}

////////////////////////////////////////////////////////////

// wParam - TRUE to subclass, FALSE to unsubclass
// lParam - subclass data
UINT g_subclassRegisteredMsg = RegisterWindowMessage(
    L"Windhawk_SetWindowSubclassFromAnyThread_" WH_MOD_ID);

BOOL SetWindowSubclassFromAnyThread(HWND hWnd,
                                    SUBCLASSPROC pfnSubclass,
                                    UINT_PTR uIdSubclass,
                                    DWORD_PTR dwRefData) {
    struct SET_WINDOW_SUBCLASS_FROM_ANY_THREAD_PARAM {
        SUBCLASSPROC pfnSubclass;
        UINT_PTR uIdSubclass;
        DWORD_PTR dwRefData;
        BOOL result;
    };

    DWORD dwThreadId = GetWindowThreadProcessId(hWnd, nullptr);
    if (dwThreadId == 0) {
        return FALSE;
    }

    if (dwThreadId == GetCurrentThreadId()) {
        return SetWindowSubclass(hWnd, pfnSubclass, uIdSubclass, dwRefData);
    }

    HHOOK hook = SetWindowsHookEx(
        WH_CALLWNDPROC,
        [](int nCode, WPARAM wParam, LPARAM lParam) -> LRESULT {
            if (nCode == HC_ACTION) {
                const CWPSTRUCT* cwp = (const CWPSTRUCT*)lParam;
                if (cwp->message == g_subclassRegisteredMsg && cwp->wParam) {
                    SET_WINDOW_SUBCLASS_FROM_ANY_THREAD_PARAM* param =
                        (SET_WINDOW_SUBCLASS_FROM_ANY_THREAD_PARAM*)cwp->lParam;
                    param->result =
                        SetWindowSubclass(cwp->hwnd, param->pfnSubclass,
                                          param->uIdSubclass, param->dwRefData);
                }
            }

            return CallNextHookEx(nullptr, nCode, wParam, lParam);
        },
        nullptr, dwThreadId);
    if (!hook) {
        return FALSE;
    }

    SET_WINDOW_SUBCLASS_FROM_ANY_THREAD_PARAM param;
    param.pfnSubclass = pfnSubclass;
    param.uIdSubclass = uIdSubclass;
    param.dwRefData = dwRefData;
    param.result = FALSE;
    SendMessage(hWnd, g_subclassRegisteredMsg, TRUE, (LPARAM)&param);

    UnhookWindowsHookEx(hook);

    return param.result;
}

bool OnMouseWheel(HWND hWnd, WPARAM wParam, LPARAM lParam) {
    if (GetCapture()) {
        return false;
    }

    POINT pt;
    pt.x = GET_X_LPARAM(lParam);
    pt.y = GET_Y_LPARAM(lParam);

    if (!IsPointInsideScrollArea(hWnd, pt)) {
        return false;
    }

    // Allows to steal focus.
    INPUT input;
    ZeroMemory(&input, sizeof(INPUT));
    SendInput(1, &input, sizeof(INPUT));

    InvokeScrollAction(wParam, lParam);

    return true;
}

LRESULT CALLBACK TaskbarWindowSubclassProc(_In_ HWND hWnd,
                                           _In_ UINT uMsg,
                                           _In_ WPARAM wParam,
                                           _In_ LPARAM lParam,
                                           _In_ UINT_PTR uIdSubclass,
                                           _In_ DWORD_PTR dwRefData) {
    if (uMsg == WM_NCDESTROY || (uMsg == g_subclassRegisteredMsg && !wParam)) {
        RemoveWindowSubclass(hWnd, TaskbarWindowSubclassProc, 0);
    }

    LRESULT result = 0;

    switch (uMsg) {
        case WM_MOUSEWHEEL:
            if (g_nExplorerVersion < WIN_VERSION_11_21H2 &&
                OnMouseWheel(hWnd, wParam, lParam)) {
                result = 0;
            } else {
                result = DefSubclassProc(hWnd, uMsg, wParam, lParam);
            }
            break;

        case WM_NCDESTROY:
            result = DefSubclassProc(hWnd, uMsg, wParam, lParam);

            if (hWnd != g_hTaskbarWnd) {
                g_secondaryTaskbarWindows.erase(hWnd);
            }
            break;

        default:
            result = DefSubclassProc(hWnd, uMsg, wParam, lParam);
            break;
    }

    return result;
}

WNDPROC InputSiteWindowProc_Original;
LRESULT CALLBACK InputSiteWindowProc_Hook(HWND hWnd,
                                          UINT uMsg,
                                          WPARAM wParam,
                                          LPARAM lParam) {
    switch (uMsg) {
        case WM_POINTERWHEEL:
            if (HWND hRootWnd = GetAncestor(hWnd, GA_ROOT);
                IsTaskbarWindow(hRootWnd) &&
                OnMouseWheel(hRootWnd, wParam, lParam)) {
                return 0;
            }
            break;
    }

    return InputSiteWindowProc_Original(hWnd, uMsg, wParam, lParam);
}

void SubclassTaskbarWindow(HWND hWnd) {
    SetWindowSubclassFromAnyThread(hWnd, TaskbarWindowSubclassProc, 0, 0);
}

void UnsubclassTaskbarWindow(HWND hWnd) {
    SendMessage(hWnd, g_subclassRegisteredMsg, FALSE, 0);
}

void HandleIdentifiedInputSiteWindow(HWND hWnd) {
    if (!g_dwTaskbarThreadId ||
        GetWindowThreadProcessId(hWnd, nullptr) != g_dwTaskbarThreadId) {
        return;
    }

    HWND hParentWnd = GetParent(hWnd);
    WCHAR szClassName[64];
    if (!hParentWnd ||
        !GetClassName(hParentWnd, szClassName, ARRAYSIZE(szClassName)) ||
        _wcsicmp(szClassName,
                 L"Windows.UI.Composition.DesktopWindowContentBridge") != 0) {
        return;
    }

    hParentWnd = GetParent(hParentWnd);
    if (!hParentWnd || !IsTaskbarWindow(hParentWnd)) {
        return;
    }

    // At first, I tried to subclass the window instead of hooking its wndproc,
    // but the inputsite.dll code checks that the value wasn't changed, and
    // crashes otherwise.
    auto wndProc = (WNDPROC)GetWindowLongPtr(hWnd, GWLP_WNDPROC);
    WindhawkUtils::Wh_SetFunctionHookT(wndProc, InputSiteWindowProc_Hook,
                                       &InputSiteWindowProc_Original);

    if (g_initialized) {
        Wh_ApplyHookOperations();
    }

    Wh_Log(L"Hooked InputSite wndproc %p", wndProc);
    g_inputSiteProcHooked = true;
}

void HandleIdentifiedTaskbarWindow(HWND hWnd) {
    g_hTaskbarWnd = hWnd;
    g_dwTaskbarThreadId = GetWindowThreadProcessId(hWnd, nullptr);
    SubclassTaskbarWindow(hWnd);
    for (HWND hSecondaryWnd : g_secondaryTaskbarWindows) {
        SubclassTaskbarWindow(hSecondaryWnd);
    }

    if (g_nExplorerVersion >= WIN_VERSION_11_21H2 && !g_inputSiteProcHooked) {
        HWND hXamlIslandWnd = FindWindowEx(
            hWnd, nullptr, L"Windows.UI.Composition.DesktopWindowContentBridge",
            nullptr);
        if (hXamlIslandWnd) {
            HWND hInputSiteWnd = FindWindowEx(
                hXamlIslandWnd, nullptr,
                L"Windows.UI.Input.InputSite.WindowClass", nullptr);
            if (hInputSiteWnd) {
                HandleIdentifiedInputSiteWindow(hInputSiteWnd);
            }
        }
    }
}

void HandleIdentifiedSecondaryTaskbarWindow(HWND hWnd) {
    if (!g_dwTaskbarThreadId ||
        GetWindowThreadProcessId(hWnd, nullptr) != g_dwTaskbarThreadId) {
        return;
    }

    g_secondaryTaskbarWindows.insert(hWnd);
    SubclassTaskbarWindow(hWnd);

    if (g_nExplorerVersion >= WIN_VERSION_11_21H2 && !g_inputSiteProcHooked) {
        HWND hXamlIslandWnd = FindWindowEx(
            hWnd, nullptr, L"Windows.UI.Composition.DesktopWindowContentBridge",
            nullptr);
        if (hXamlIslandWnd) {
            HWND hInputSiteWnd = FindWindowEx(
                hXamlIslandWnd, nullptr,
                L"Windows.UI.Input.InputSite.WindowClass", nullptr);
            if (hInputSiteWnd) {
                HandleIdentifiedInputSiteWindow(hInputSiteWnd);
            }
        }
    }
}

HWND FindCurrentProcessTaskbarWindows(
    std::unordered_set<HWND>* secondaryTaskbarWindows) {
    struct ENUM_WINDOWS_PARAM {
        HWND* hWnd;
        std::unordered_set<HWND>* secondaryTaskbarWindows;
    };

    HWND hWnd = nullptr;
    ENUM_WINDOWS_PARAM param = {&hWnd, secondaryTaskbarWindows};
    EnumWindows(
        [](HWND hWnd, LPARAM lParam) -> BOOL {
            ENUM_WINDOWS_PARAM& param = *(ENUM_WINDOWS_PARAM*)lParam;

            DWORD dwProcessId = 0;
            if (!GetWindowThreadProcessId(hWnd, &dwProcessId) ||
                dwProcessId != GetCurrentProcessId())
                return TRUE;

            WCHAR szClassName[32];
            if (GetClassName(hWnd, szClassName, ARRAYSIZE(szClassName)) == 0)
                return TRUE;

            if (_wcsicmp(szClassName, L"Shell_TrayWnd") == 0) {
                *param.hWnd = hWnd;
            } else if (_wcsicmp(szClassName, L"Shell_SecondaryTrayWnd") == 0) {
                param.secondaryTaskbarWindows->insert(hWnd);
            }

            return TRUE;
        },
        (LPARAM)&param);

    return hWnd;
}

using CreateWindowExW_t = decltype(&CreateWindowExW);
CreateWindowExW_t CreateWindowExW_Original;
HWND WINAPI CreateWindowExW_Hook(DWORD dwExStyle,
                                 LPCWSTR lpClassName,
                                 LPCWSTR lpWindowName,
                                 DWORD dwStyle,
                                 int X,
                                 int Y,
                                 int nWidth,
                                 int nHeight,
                                 HWND hWndParent,
                                 HMENU hMenu,
                                 HINSTANCE hInstance,
                                 LPVOID lpParam) {
    HWND hWnd = CreateWindowExW_Original(dwExStyle, lpClassName, lpWindowName,
                                         dwStyle, X, Y, nWidth, nHeight,
                                         hWndParent, hMenu, hInstance, lpParam);
    if (!hWnd)
        return hWnd;

    BOOL bTextualClassName = ((ULONG_PTR)lpClassName & ~(ULONG_PTR)0xffff) != 0;

    if (bTextualClassName && _wcsicmp(lpClassName, L"Shell_TrayWnd") == 0) {
        Wh_Log(L"Taskbar window created: %08X", (DWORD)(ULONG_PTR)hWnd);
        HandleIdentifiedTaskbarWindow(hWnd);
    } else if (bTextualClassName &&
               _wcsicmp(lpClassName, L"Shell_SecondaryTrayWnd") == 0) {
        Wh_Log(L"Secondary taskbar window created: %08X",
               (DWORD)(ULONG_PTR)hWnd);
        HandleIdentifiedSecondaryTaskbarWindow(hWnd);
    }

    return hWnd;
}

using CreateWindowInBand_t = HWND(WINAPI*)(DWORD dwExStyle,
                                           LPCWSTR lpClassName,
                                           LPCWSTR lpWindowName,
                                           DWORD dwStyle,
                                           int X,
                                           int Y,
                                           int nWidth,
                                           int nHeight,
                                           HWND hWndParent,
                                           HMENU hMenu,
                                           HINSTANCE hInstance,
                                           LPVOID lpParam,
                                           DWORD dwBand);
CreateWindowInBand_t CreateWindowInBand_Original;
HWND WINAPI CreateWindowInBand_Hook(DWORD dwExStyle,
                                    LPCWSTR lpClassName,
                                    LPCWSTR lpWindowName,
                                    DWORD dwStyle,
                                    int X,
                                    int Y,
                                    int nWidth,
                                    int nHeight,
                                    HWND hWndParent,
                                    HMENU hMenu,
                                    HINSTANCE hInstance,
                                    LPVOID lpParam,
                                    DWORD dwBand) {
    HWND hWnd = CreateWindowInBand_Original(
        dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight,
        hWndParent, hMenu, hInstance, lpParam, dwBand);
    if (!hWnd)
        return hWnd;

    BOOL bTextualClassName = ((ULONG_PTR)lpClassName & ~(ULONG_PTR)0xffff) != 0;

    if (bTextualClassName &&
        _wcsicmp(lpClassName, L"Windows.UI.Input.InputSite.WindowClass") == 0) {
        Wh_Log(L"InputSite window created: %08X", (DWORD)(ULONG_PTR)hWnd);
        if (g_nExplorerVersion >= WIN_VERSION_11_21H2 &&
            !g_inputSiteProcHooked) {
            HandleIdentifiedInputSiteWindow(hWnd);
        }
    }

    return hWnd;
}

void LoadSettings() {
    PCWSTR scrollAction = Wh_GetStringSetting(L"scrollAction");
    g_settings.scrollAction = ScrollAction::virtualDesktopSwitch;
    if (wcscmp(scrollAction, L"brightnessChange") == 0) {
        g_settings.scrollAction = ScrollAction::brightnessChange;
    } else if (wcscmp(scrollAction, L"micVolumeChange") == 0) {
        g_settings.scrollAction = ScrollAction::micVolumeChange;
    }
    Wh_FreeStringSetting(scrollAction);

    PCWSTR scrollArea = Wh_GetStringSetting(L"scrollArea");
    g_settings.scrollArea = ScrollArea::taskbar;
    if (wcscmp(scrollArea, L"notificationArea") == 0) {
        g_settings.scrollArea = ScrollArea::notificationArea;
    } else if (wcscmp(scrollArea, L"taskbarWithoutNotificationArea") == 0) {
        g_settings.scrollArea = ScrollArea::taskbarWithoutNotificationArea;
    }
    Wh_FreeStringSetting(scrollArea);

    g_settings.scrollStep = Wh_GetIntSetting(L"scrollStep");
    g_settings.throttleMs = Wh_GetIntSetting(L"throttleMs");
    g_settings.reverseScrollingDirection =
        Wh_GetIntSetting(L"reverseScrollingDirection");
    g_settings.oldTaskbarOnWin11 = Wh_GetIntSetting(L"oldTaskbarOnWin11");
}

bool IsExplorerPatcherModule(HMODULE module) {
    WCHAR moduleFilePath[MAX_PATH];
    switch (
        GetModuleFileName(module, moduleFilePath, ARRAYSIZE(moduleFilePath))) {
        case 0:
        case ARRAYSIZE(moduleFilePath):
            return false;
    }

    PCWSTR moduleFileName = wcsrchr(moduleFilePath, L'\\');
    if (!moduleFileName) {
        return false;
    }

    moduleFileName++;

    if (_wcsnicmp(L"ep_taskbar.", moduleFileName, sizeof("ep_taskbar.") - 1) ==
        0) {
        Wh_Log(L"ExplorerPatcher taskbar module: %s", moduleFileName);
        return true;
    }

    return false;
}

void HandleLoadedExplorerPatcher() {
    HMODULE hMods[1024];
    DWORD cbNeeded;
    if (EnumProcessModules(GetCurrentProcess(), hMods, sizeof(hMods),
                           &cbNeeded)) {
        for (size_t i = 0; i < cbNeeded / sizeof(HMODULE); i++) {
            if (IsExplorerPatcherModule(hMods[i])) {
                if (g_nExplorerVersion >= WIN_VERSION_11_21H2) {
                    g_nExplorerVersion = WIN_VERSION_10_20H1;
                }
                break;
            }
        }
    }
}

void HandleLoadedModuleIfExplorerPatcher(HMODULE module) {
    if (module && !((ULONG_PTR)module & 3)) {
        if (IsExplorerPatcherModule(module)) {
            if (g_nExplorerVersion >= WIN_VERSION_11_21H2) {
                g_nExplorerVersion = WIN_VERSION_10_20H1;
            }
        }
    }
}

using LoadLibraryExW_t = decltype(&LoadLibraryExW);
LoadLibraryExW_t LoadLibraryExW_Original;
HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR lpLibFileName,
                                   HANDLE hFile,
                                   DWORD dwFlags) {
    HMODULE module = LoadLibraryExW_Original(lpLibFileName, hFile, dwFlags);
    if (module) {
        HandleLoadedModuleIfExplorerPatcher(module);
    }

    return module;
}

BOOL Wh_ModInit() {
    Wh_Log(L">");

    LoadSettings();

    if (!WindowsVersionInit()) {
        Wh_Log(L"Unsupported Windows version");
        return FALSE;
    }

    g_nExplorerVersion = g_nWinVersion;
    if (g_nExplorerVersion >= WIN_VERSION_11_21H2 &&
        g_settings.oldTaskbarOnWin11) {
        g_nExplorerVersion = WIN_VERSION_10_20H1;
    }

    WindhawkUtils::Wh_SetFunctionHookT(CreateWindowExW, CreateWindowExW_Hook,
                                       &CreateWindowExW_Original);

    HMODULE user32Module =
        LoadLibraryEx(L"user32.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (user32Module) {
        auto pCreateWindowInBand = (CreateWindowInBand_t)GetProcAddress(
            user32Module, "CreateWindowInBand");
        if (pCreateWindowInBand) {
            WindhawkUtils::Wh_SetFunctionHookT(pCreateWindowInBand,
                                               CreateWindowInBand_Hook,
                                               &CreateWindowInBand_Original);
        }
    }

    HandleLoadedExplorerPatcher();

    HMODULE kernelBaseModule = GetModuleHandle(L"kernelbase.dll");
    auto pKernelBaseLoadLibraryExW = (decltype(&LoadLibraryExW))GetProcAddress(
        kernelBaseModule, "LoadLibraryExW");
    WindhawkUtils::Wh_SetFunctionHookT(pKernelBaseLoadLibraryExW,
                                       LoadLibraryExW_Hook,
                                       &LoadLibraryExW_Original);

    g_initialized = true;

    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L">");

    // Try again in case there's a race between the previous attempt and the
    // LoadLibraryExW hook.
    HandleLoadedExplorerPatcher();

    WNDCLASS wndclass;
    if (GetClassInfo(GetModuleHandle(NULL), L"Shell_TrayWnd", &wndclass)) {
        HWND hWnd =
            FindCurrentProcessTaskbarWindows(&g_secondaryTaskbarWindows);
        if (hWnd) {
            HandleIdentifiedTaskbarWindow(hWnd);
        }
    }
}

void Wh_ModUninit() {
    Wh_Log(L">");

    if (g_hTaskbarWnd) {
        UnsubclassTaskbarWindow(g_hTaskbarWnd);

        for (HWND hSecondaryWnd : g_secondaryTaskbarWindows) {
            UnsubclassTaskbarWindow(hSecondaryWnd);
        }
    }

    MicVolUninit();
}

BOOL Wh_ModSettingsChanged(BOOL* bReload) {
    Wh_Log(L">");

    bool prevOldTaskbarOnWin11 = g_settings.oldTaskbarOnWin11;

    LoadSettings();

    *bReload = g_settings.oldTaskbarOnWin11 != prevOldTaskbarOnWin11;

    return TRUE;
}
