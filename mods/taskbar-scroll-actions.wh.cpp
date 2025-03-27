// ==WindhawkMod==
// @id              taskbar-extra-scroll-actions
// @name            Taskbar Scroll Actions
// @description     Assign actions for scrolling over the taskbar, including virtual desktop switching, monitor brightness control, and SDR brightness control on HDR displays
// @version         1.1.0
// @author          nairodorian(aka claude3.7*) + m417z
// @github          https://github.com/m417z https://github.com/NairoDorian
// @homepage        https://m417z.com/ https://nairodorian.com/
// @include         explorer.exe
// @compilerOptions -lcomctl32 -lole32 -loleaut32 -lversion -ldxgi -ldwmapi
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

This mod allows you to assign actions to mouse wheel scrolling over the taskbar, including:
- Switching between virtual desktops
- Adjusting monitor brightness
- Adjusting SDR content brightness on HDR displays
- Master brightness control (intelligently adjusts the appropriate brightness based on display type)

## Features

### Virtual Desktop Switching
Scroll up or down on the taskbar to switch between virtual desktops.

### Monitor Brightness Control
Scroll up or down on the taskbar to increase or decrease monitor brightness using the standard Windows brightness control.

### SDR Brightness Control on HDR Displays
For HDR displays, scroll up or down on the taskbar to adjust the SDR content brightness. This is especially useful when:
- You have an HDR display and find SDR content too dark or too bright
- The standard brightness controls don't work when HDR is enabled
- You switch between SDR and HDR content frequently

### Master Brightness Control (Intelligent)
The master mode intelligently detects which display's taskbar you're scrolling on and automatically applies the appropriate brightness adjustment:
- If scrolling on an HDR display's taskbar, it adjusts the SDR brightness for that specific monitor
- If scrolling on a non-HDR display's taskbar, it adjusts the standard monitor brightness

This intelligent approach is perfect for mixed display setups where you have both HDR and non-HDR monitors connected to your system.

## Settings

- **Scroll Action**: Choose between virtual desktop switching, monitor brightness control, SDR brightness control, or master brightness control.
- **Scroll Area**: Define where scrolling should trigger the action (entire taskbar, notification area only, or taskbar without notification area).
- **Scroll Step**: How many steps to change per scroll action.
- **Reverse Scrolling Direction**: Reverse the direction of the scrolling action.
- **Old Taskbar on Windows 11**: Enable if you're using the old Windows 10 taskbar on Windows 11 (e.g., with Explorer Patcher).
- **Remember SDR to HDR Brightness**: When enabled, the mod will remember your SDR brightness setting between sessions.
- **Maximum SDR Brightness**: Set the maximum value for SDR brightness (default max is 6.0).

## Notes for SDR Brightness on HDR Displays
- The SDR brightness range is from 1.0 to the maximum value you set (default 6.0)
- Each scroll wheel tick changes the brightness by 0.1 (or more if you increase the scroll step)
- This setting affects the same brightness level you can adjust in Windows 11 Settings > System > Display > HDR > SDR brightness balance
- The master brightness control mode will intelligently detect whether you're scrolling on an HDR or non-HDR display's taskbar
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- scrollAction: virtualDesktopSwitch
  $name: Scroll action
  $description: The action to perform when scrolling over the taskbar
  $options:
  - virtualDesktopSwitch: Switch virtual desktop
  - brightnessChange: Change monitor brightness
  - sdrToHdrBrightnessChange: Change SDR brightness on HDR displays
  - masterBrightnessChange: Master brightness control (monitor + SDR/HDR brightness)
- scrollArea: taskbar
  $name: Scroll area
  $description: The area on which scrolling triggers the action
  $options:
  - taskbar: The entire taskbar
  - notificationArea: The notification area (system tray and clock)
  - taskbarWithoutNotificationArea: The taskbar without the notification area
- scrollStep: 1
  $name: Scroll step
  $description: How many steps to change per scroll action
- storeCurrentSdrToHdrBrightness: true
  $name: Remember SDR to HDR brightness
  $description: Remember the last SDR to HDR brightness value between sessions
- maxSdrBrightness: 6.0
  $name: Maximum SDR brightness
  $description: Maximum value for SDR brightness on HDR displays (default is 6.0, range 1.0-10.0)
- reverseScrollingDirection: false
  $name: Reverse scrolling direction
  $description: Reverse the direction of the scrolling action
- oldTaskbarOnWin11: false
  $name: Old taskbar on Windows 11
  $description: Enable if you're using the old Windows 10 taskbar on Windows 11 (Explorer Patcher)
*/
// ==/WindhawkModSettings==

#include <initguid.h>

#include <combaseapi.h>
#include <commctrl.h>
#include <comutil.h>
#include <wbemcli.h>
#include <windowsx.h>
#include <dxgi1_6.h> // For checking HDR status

#include <unordered_set>
#include <unordered_map>

enum class ScrollAction {
    virtualDesktopSwitch,
    brightnessChange,
    sdrToHdrBrightnessChange, // New action for SDR brightness adjustment
    masterBrightnessChange,   // Combined brightness adjustment (regular + SDR)
};

enum class ScrollArea {
    taskbar,
    notificationArea,
    taskbarWithoutNotificationArea,
};

// Typedefs for SDR to HDR brightness functionality
typedef void (WINAPI *DwmpSDRToHDRBoostPtr_t)(HMONITOR hMonitor, double brightness);

// Global variables for the SDR to HDR brightness functionality
HMODULE g_hDwmApiDll = NULL;
DwmpSDRToHDRBoostPtr_t g_pDwmpSDRToHDRBoost = NULL;
double g_currentSdrToHdrBrightness = 1.0; // Default value

struct {
    ScrollAction scrollAction;
    ScrollArea scrollArea;
    int scrollStep;
    bool reverseScrollingDirection;
    bool oldTaskbarOnWin11;
    bool storeCurrentSdrToHdrBrightness; // New setting for remembering SDR brightness
    double maxSdrBrightness; // Maximum SDR brightness setting
} g_settings;

// Add HDR detection and monitor state tracking
typedef HRESULT (WINAPI *PFN_CreateDXGIFactory1)(REFIID riid, void **ppFactory);

// Cache for HDR monitor status to avoid frequent checks
struct MonitorState {
    bool isHdr;
    DWORD lastCheckTime;
};

std::unordered_map<HMONITOR, MonitorState> g_monitorHdrCache;

// Define the callback structure for SDR brightness
struct SDRBrightnessCallbackData {
    DwmpSDRToHDRBoostPtr_t pFunc;
    double brightness;
};

// Function to check if a monitor has HDR enabled
bool IsMonitorHdrEnabled(HMONITOR hMonitor) {
    // Check cache first to avoid frequent checks
    auto it = g_monitorHdrCache.find(hMonitor);
    if (it != g_monitorHdrCache.end()) {
        // Only recheck after 5 seconds to avoid performance impact
        if (GetTickCount() - it->second.lastCheckTime < 5000) {
            return it->second.isHdr;
        }
    }

    // Default to non-HDR if detection fails
    bool isHdr = false;
    
    HMODULE hDXGI = LoadLibrary(L"dxgi.dll");
    if (hDXGI) {
        PFN_CreateDXGIFactory1 createDXGIFactory1 = 
            (PFN_CreateDXGIFactory1)GetProcAddress(hDXGI, "CreateDXGIFactory1");
        
        if (createDXGIFactory1) {
            IDXGIFactory1* pFactory = nullptr;
            HRESULT hr = createDXGIFactory1(__uuidof(IDXGIFactory1), (void**)&pFactory);
            
            if (SUCCEEDED(hr) && pFactory) {
                IDXGIAdapter1* pAdapter = nullptr;
                for (UINT i = 0; SUCCEEDED(pFactory->EnumAdapters1(i, &pAdapter)); i++) {
                    IDXGIOutput* pOutput = nullptr;
                    for (UINT j = 0; SUCCEEDED(pAdapter->EnumOutputs(j, &pOutput)); j++) {
                        DXGI_OUTPUT_DESC desc;
                        if (SUCCEEDED(pOutput->GetDesc(&desc)) && desc.Monitor == hMonitor) {
                            IDXGIOutput6* pOutput6 = nullptr;
                            if (SUCCEEDED(pOutput->QueryInterface(__uuidof(IDXGIOutput6), (void**)&pOutput6))) {
                                DXGI_OUTPUT_DESC1 desc1;
                                if (SUCCEEDED(pOutput6->GetDesc1(&desc1))) {
                                    isHdr = desc1.ColorSpace == DXGI_COLOR_SPACE_RGB_FULL_G2084_NONE_P2020;
                                }
                                pOutput6->Release();
                            }
                        }
                        pOutput->Release();
                    }
                    pAdapter->Release();
                }
                pFactory->Release();
            }
        }
        FreeLibrary(hDXGI);
    }
    
    // Cache the result
    g_monitorHdrCache[hMonitor] = { isHdr, GetTickCount() };
    return isHdr;
}

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

// Function to initialize the SDR to HDR brightness adjustment
bool InitializeSdrToHdrBrightness() {
    if (g_pDwmpSDRToHDRBoost)
        return true;

    if (!g_hDwmApiDll) {
        g_hDwmApiDll = LoadLibrary(L"dwmapi.dll");
        if (!g_hDwmApiDll) {
            Wh_Log(L"Failed to load dwmapi.dll");
            return false;
        }
    }

    // The function we need is at ordinal 171 (based on the ScreenBrightnessSetter)
    FARPROC pFunc = GetProcAddress(g_hDwmApiDll, (LPCSTR)171);
    if (!pFunc) {
        Wh_Log(L"Failed to get DwmpSDRToHDRBoost function");
        return false;
    }

    g_pDwmpSDRToHDRBoost = (DwmpSDRToHDRBoostPtr_t)pFunc;
    
    // Load saved brightness if enabled
    if (g_settings.storeCurrentSdrToHdrBrightness) {
        g_currentSdrToHdrBrightness = Wh_GetIntValue(L"CurrentSdrToHdrBrightness", 10) / 10.0;
        if (g_currentSdrToHdrBrightness < 1.0 || g_currentSdrToHdrBrightness > 1000.0) {
            g_currentSdrToHdrBrightness = 1.0;
        }
    }
    
    return true;
}

// Callback function for EnumDisplayMonitors with the correct stdcall convention
BOOL CALLBACK MonitorEnumProcForSDRBrightness(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData) {
    SDRBrightnessCallbackData* pData = reinterpret_cast<SDRBrightnessCallbackData*>(dwData);
    pData->pFunc(hMonitor, pData->brightness);
    return TRUE;
}

// Function to adjust the SDR to HDR brightness
bool AdjustSdrToHdrBrightness(int clicks, HMONITOR targetMonitor = NULL) {
    if (!InitializeSdrToHdrBrightness())
        return false;

    // The range for SDR to HDR brightness is typically 1.0 to max (configurable)
    const double minBrightness = 1.0;
    const double maxBrightness = g_settings.maxSdrBrightness;
    const double step = 0.1; // Each scroll wheel click changes by 0.1
    
    // Calculate new brightness
    g_currentSdrToHdrBrightness += clicks * step;
    
    // Clamp to min/max
    if (g_currentSdrToHdrBrightness < minBrightness)
        g_currentSdrToHdrBrightness = minBrightness;
    if (g_currentSdrToHdrBrightness > maxBrightness)
        g_currentSdrToHdrBrightness = maxBrightness;
    
    Wh_Log(L"Setting SDR to HDR brightness to %.1f", g_currentSdrToHdrBrightness);
    
    // Save the current brightness if enabled
    if (g_settings.storeCurrentSdrToHdrBrightness) {
        Wh_SetIntValue(L"CurrentSdrToHdrBrightness", (int)(g_currentSdrToHdrBrightness * 10));
    }
    
    // Create a struct to pass data to the callback
    SDRBrightnessCallbackData data = { g_pDwmpSDRToHDRBoost, g_currentSdrToHdrBrightness };
    
    if (targetMonitor) {
        // Apply to the specific monitor only
        g_pDwmpSDRToHDRBoost(targetMonitor, g_currentSdrToHdrBrightness);
    } else {
        // Apply to all monitors
        EnumDisplayMonitors(NULL, NULL, MonitorEnumProcForSDRBrightness, reinterpret_cast<LPARAM>(&data));
    }
    
    return true;
}

#pragma region functions

bool IsTaskbarWindow(HWND hWnd) {
    WCHAR szClassName[32];
    if (!GetClassName(hWnd, szClassName, ARRAYSIZE(szClassName))) {
        return false;
    }

    return _wcsicmp(szClassName, L"Shell_TrayWnd") == 0 ||
           _wcsicmp(szClassName, L"Shell_SecondaryTrayWnd") == 0;
}

bool GetNotificationAreaRect(HWND hMMTaskbarWnd, RECT* rcResult) {
    if (hMMTaskbarWnd == g_hTaskbarWnd) {
        HWND hTrayNotifyWnd =
            FindWindowEx(hMMTaskbarWnd, NULL, L"TrayNotifyWnd", NULL);
        if (!hTrayNotifyWnd) {
            return false;
        }

        return GetWindowRect(hTrayNotifyWnd, rcResult);
    }

    if (g_nExplorerVersion >= WIN_VERSION_11_21H2) {
        RECT rcTaskbar;
        if (!GetWindowRect(hMMTaskbarWnd, &rcTaskbar)) {
            return false;
        }

        HWND hBridgeWnd = FindWindowEx(
            hMMTaskbarWnd, NULL,
            L"Windows.UI.Composition.DesktopWindowContentBridge", NULL);
        while (hBridgeWnd) {
            RECT rcBridge;
            if (!GetWindowRect(hBridgeWnd, &rcBridge)) {
                break;
            }

            if (rcBridge.left != rcTaskbar.left ||
                rcBridge.top != rcTaskbar.top ||
                rcBridge.right != rcTaskbar.right ||
                rcBridge.bottom != rcTaskbar.bottom) {
                CopyRect(rcResult, &rcBridge);
                return true;
            }

            hBridgeWnd = FindWindowEx(
                hMMTaskbarWnd, hBridgeWnd,
                L"Windows.UI.Composition.DesktopWindowContentBridge", NULL);
        }

        // On newer Win11 versions, the clock on secondary taskbars is difficult
        // to detect without either UI Automation or UWP UI APIs. Just consider
        // the last pixels, not accurate, but better than nothing.
        CopyRect(rcResult, &rcTaskbar);
        if (rcResult->right - rcResult->left > 50) {
            if (GetWindowLong(hMMTaskbarWnd, GWL_EXSTYLE) & WS_EX_LAYOUTRTL) {
                rcResult->right = rcResult->left + 50;
            } else {
                rcResult->left = rcResult->right - 50;
            }
        }

        return true;
    }

    if (g_nExplorerVersion >= WIN_VERSION_10_R1) {
        HWND hClockButtonWnd =
            FindWindowEx(hMMTaskbarWnd, NULL, L"ClockButton", NULL);
        if (!hClockButtonWnd) {
            return false;
        }

        return GetWindowRect(hClockButtonWnd, rcResult);
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
    HWND hTaskbarWnd = FindWindow(L"Shell_TrayWnd", nullptr);
    if (hTaskbarWnd) {
        SetForegroundWindow(hTaskbarWnd);
    }

    WORD key = VK_LEFT;
    if (clicks < 0) {
        clicks = -clicks;
        key = VK_RIGHT;
    }

    INPUT* input = new INPUT[clicks * 2 + 4];
    for (size_t i = 0; i < static_cast<size_t>(clicks) * 2 + 4; i++) {
        input[i].type = INPUT_KEYBOARD;
        input[i].ki.wScan = 0;
        input[i].ki.time = 0;
        input[i].ki.dwExtraInfo = 0;
    }

    input[0].ki.wVk = VK_LWIN;
    input[0].ki.dwFlags = 0;
    input[1].ki.wVk = VK_LCONTROL;
    input[1].ki.dwFlags = 0;

    for (size_t i = 0; i < static_cast<size_t>(clicks); i++) {
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

DWORD g_lastScrollTime;
int g_lastScrollDeltaRemainder;

// Updated to handle monitor-specific brightness adjustment
void InvokeScrollAction(WPARAM wParam, LPARAM lMousePosParam, HMONITOR hMonitor = NULL) {
    int delta = GET_WHEEL_DELTA_WPARAM(wParam) * g_settings.scrollStep;

    if (g_settings.reverseScrollingDirection) {
        delta = -delta;
    }

    if (GetTickCount() - g_lastScrollTime < 1000 * 5) {
        delta += g_lastScrollDeltaRemainder;
    }

    int clicks = delta / WHEEL_DELTA;
    Wh_Log(L"%d clicks (delta=%d)", clicks, delta);

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
            
            case ScrollAction::sdrToHdrBrightnessChange: {
                AdjustSdrToHdrBrightness(clicks, hMonitor);
                break;
            }
            
            case ScrollAction::masterBrightnessChange: {
                bool isHdrMonitor = false;
                
                // Check if the monitor we're scrolling on has HDR enabled
                if (hMonitor) {
                    isHdrMonitor = IsMonitorHdrEnabled(hMonitor);
                }
                
                Wh_Log(L"Master brightness mode for monitor %p (HDR: %s)", 
                       hMonitor, isHdrMonitor ? L"Yes" : L"No");
                
                if (isHdrMonitor) {
                    // For HDR monitors, adjust SDR brightness for this specific monitor
                    AdjustSdrToHdrBrightness(clicks, hMonitor);
                } else {
                    // For non-HDR monitors, adjust standard brightness
                    int brightness = GetBrightness();
                    if (brightness != -1) {
                        Wh_Log(L"Master mode (non-HDR): Changing monitor brightness from %d to %d", 
                               brightness, brightness + clicks);
                        SetBrightness(brightness + clicks);
                    } else {
                        Wh_Log(L"Error getting current brightness");
                    }
                }
                break;
            }
        }
    }

    g_lastScrollTime = GetTickCount();
    g_lastScrollDeltaRemainder = delta % WHEEL_DELTA;
}

////////////////////////////////////////////////////////////

// wParam - TRUE to subclass, FALSE to unsubclass
// lParam - subclass data
UINT g_subclassRegisteredMsg = RegisterWindowMessage(
    L"Windhawk_SetWindowSubclassFromAnyThread_taskbar-scroll-actions");

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
        [](int nCode, WPARAM wParam, LPARAM lParam) WINAPI -> LRESULT {
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

    // Get the monitor for the taskbar we're scrolling on
    HMONITOR hMonitor = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
    
    // Store the monitor for use in InvokeScrollAction
    thread_local static HMONITOR lastScrolledMonitor = NULL;
    lastScrolledMonitor = hMonitor;

    InvokeScrollAction(wParam, lParam, hMonitor);

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
            if (g_nExplorerVersion < WIN_VERSION_11_21H2) {
                HMONITOR hMonitor = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
                if (OnMouseWheel(hWnd, wParam, lParam)) {
                    result = 0;
                } else {
                    result = DefSubclassProc(hWnd, uMsg, wParam, lParam);
                }
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
                IsTaskbarWindow(hRootWnd)) {
                HMONITOR hMonitor = MonitorFromWindow(hRootWnd, MONITOR_DEFAULTTONEAREST);
                if (OnMouseWheel(hRootWnd, wParam, lParam)) {
                    return 0;
                }
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
    void* wndProc = (void*)GetWindowLongPtr(hWnd, GWLP_WNDPROC);
    Wh_SetFunctionHook(wndProc, (void*)InputSiteWindowProc_Hook,
                       (void**)&InputSiteWindowProc_Original);

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
        [](HWND hWnd, LPARAM lParam) WINAPI -> BOOL {
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

// Updated to handle the new action and settings
void LoadSettings() {
    PCWSTR scrollAction = Wh_GetStringSetting(L"scrollAction");
    g_settings.scrollAction = ScrollAction::virtualDesktopSwitch;
    if (wcscmp(scrollAction, L"brightnessChange") == 0) {
        g_settings.scrollAction = ScrollAction::brightnessChange;
    } else if (wcscmp(scrollAction, L"sdrToHdrBrightnessChange") == 0) {
        g_settings.scrollAction = ScrollAction::sdrToHdrBrightnessChange;
    } else if (wcscmp(scrollAction, L"masterBrightnessChange") == 0) {
        g_settings.scrollAction = ScrollAction::masterBrightnessChange;
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
    g_settings.reverseScrollingDirection = Wh_GetIntSetting(L"reverseScrollingDirection");
    g_settings.oldTaskbarOnWin11 = Wh_GetIntSetting(L"oldTaskbarOnWin11");
    g_settings.storeCurrentSdrToHdrBrightness = Wh_GetIntSetting(L"storeCurrentSdrToHdrBrightness");
    
    // Load max SDR brightness (default 6.0, clamped between 1.0 and 10.0)
    double maxBrightness = Wh_GetIntSetting(L"maxSdrBrightness");
    if (maxBrightness < 1.0) maxBrightness = 6.0; // Use default if invalid
    g_settings.maxSdrBrightness = maxBrightness < 1.0 ? 1.0 : (maxBrightness > 10000.0 ? 10.0 : maxBrightness);
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

    Wh_SetFunctionHook((void*)CreateWindowExW, (void*)CreateWindowExW_Hook,
                       (void**)&CreateWindowExW_Original);

    HMODULE user32Module = LoadLibrary(L"user32.dll");
    if (user32Module) {
        void* pCreateWindowInBand =
            (void*)GetProcAddress(user32Module, "CreateWindowInBand");
        if (pCreateWindowInBand) {
            Wh_SetFunctionHook(pCreateWindowInBand,
                               (void*)CreateWindowInBand_Hook,
                               (void**)&CreateWindowInBand_Original);
        }
    }

    WNDCLASS wndclass;
    if (GetClassInfo(GetModuleHandle(NULL), L"Shell_TrayWnd", &wndclass)) {
        HWND hWnd =
            FindCurrentProcessTaskbarWindows(&g_secondaryTaskbarWindows);
        if (hWnd) {
            HandleIdentifiedTaskbarWindow(hWnd);
        }
    }

    g_initialized = true;

    return TRUE;
}

// Added save for SDR brightness when mod is unloaded
void Wh_ModBeforeUninit() {
    if (g_settings.storeCurrentSdrToHdrBrightness) {
        Wh_SetIntValue(L"CurrentSdrToHdrBrightness", (int)(g_currentSdrToHdrBrightness * 10));
    }
}

// Updated to clean up resources for SDR brightness
void Wh_ModUninit() {
    Wh_Log(L">");

    if (g_hTaskbarWnd) {
        UnsubclassTaskbarWindow(g_hTaskbarWnd);

        for (HWND hSecondaryWnd : g_secondaryTaskbarWindows) {
            UnsubclassTaskbarWindow(hSecondaryWnd);
        }
    }

    if (g_hDwmApiDll) {
        FreeLibrary(g_hDwmApiDll);
        g_hDwmApiDll = NULL;
        g_pDwmpSDRToHDRBoost = NULL;
    }
}

BOOL Wh_ModSettingsChanged(BOOL* bReload) {
    Wh_Log(L">");

    bool prevOldTaskbarOnWin11 = g_settings.oldTaskbarOnWin11;
    ScrollAction prevScrollAction = g_settings.scrollAction;

    LoadSettings();

    // If we're switching to/from SDR to HDR brightness, or changing taskbar style 
    // on Win11, we need to reload the mod
    *bReload = g_settings.oldTaskbarOnWin11 != prevOldTaskbarOnWin11 ||
             (prevScrollAction != g_settings.scrollAction && 
              (prevScrollAction == ScrollAction::sdrToHdrBrightnessChange || 
               g_settings.scrollAction == ScrollAction::sdrToHdrBrightnessChange));

    return TRUE;
}
