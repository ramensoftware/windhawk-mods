// ==WindhawkMod==
// @id              taskbar-scroll-actions
// @name            Taskbar Scroll Actions
// @description     Assign actions for scrolling over the taskbar, including virtual desktop switching, brightness control, and microphone volume control
// @version         1.3
// @author          m417z
// @github          https://github.com/m417z
// @twitter         https://twitter.com/m417z
// @homepage        https://m417z.com/
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lcomctl32 -ldxva2 -lgdi32 -lole32 -loleaut32 -lruntimeobject -lversion
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
switching, brightness control, and microphone volume control.

Currently, the following actions are supported:

* Switch virtual desktop
* Change monitor brightness
* Change microphone volume

Brightness control works with both external monitors (via DDC/CI) and laptop
internal displays (via WMI). For external monitors, DDC/CI must be enabled in
the monitor's OSD settings.

By default, the brightness is changed with the native Windows brightness API,
the one used by the Windows Settings app, and a native Windows indicator is
shown. For external monitors, this requires native external monitor brightness
support, which is only available in some Windows 11 builds. If the native method
isn't available, the brightness is changed directly via WMI or DDC/CI, without
an indicator. The method can be changed in the mod settings.

**Note:** Some laptop touchpads might not support scrolling over the taskbar. A
workaround is to use the "pinch to zoom" gesture. For details, check out [a
relevant
issue](https://tweaker.userecho.com/topics/826-scroll-on-trackpadtouchpad-doesnt-trigger-mouse-wheel-options).

## Related mods

* For system-wide volume control, check out the [Taskbar Volume
  Control](https://windhawk.net/mods/taskbar-volume-control) mod.
* For per-app volume control, check out the [Taskbar Volume Control
  Per-App](https://windhawk.net/mods/taskbar-volume-control-per-app) mod.
* For cycling between taskbar buttons with the mouse wheel while hovering over
  the taskbar, check out the [Cycle taskbar buttons with mouse
  wheel](https://windhawk.net/mods/taskbar-wheel-cycle) mod.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- ScrollActions:
  - - scrollAction: virtualDesktopSwitch
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
      - none: None (only additional scroll regions)
    - additionalScrollRegions: ""
      $name: Additional scroll regions
      $description: >-
        A comma-separated list of additional regions along the taskbar where
        scrolling will be active. Each region is a range like
        "100-200" (pixels) or "20%-50%" (percentage of taskbar length).
    - scrollStep: 1
      $name: Scroll step
      $description: >-
        Allows to configure the change that will occur with each notch of mouse
        wheel movement.
    - throttleMs: 0
      $name: Throttle time (milliseconds)
      $description: >-
        Prevents new actions from being triggered for this amount of time after
        the last one. Set to 0 to disable throttling. Useful for preventing a
        single scroll wheel 'flick' from switching multiple desktops.
    - reverseScrollingDirection: false
      $name: Reverse scrolling direction
  $name: Scroll actions
  $description: >-
    Define one or more scroll actions for different regions of the taskbar.
- brightnessMethodWmi: native
  $name: Laptop display brightness method (WMI)
  $description: >-
    The native method uses the Windows brightness API and shows the Windows
    brightness indicator. The custom method uses WMI directly and shows no
    indicator. If the native method isn't available, the custom method is used.
  $options:
  - native: Native (with indicator)
  - custom: Custom (no indicator)
- brightnessMethodDdcCi: native
  $name: External monitor brightness method (DDC/CI)
  $description: >-
    The native method uses the Windows external monitor brightness API, which is
    only available in some Windows 11 builds, and shows a Windows text
    indicator. The custom method uses DDC/CI directly and shows no indicator. If
    the native method isn't available, the custom method is used.
  $options:
  - native: Native (with indicator)
  - custom: Custom (no indicator)
- oldTaskbarOnWin11: false
  $name: Customize the old taskbar on Windows 11
  $description: >-
    Enable this option to customize the old taskbar on Windows 11 (if using
    ExplorerPatcher or a similar tool).
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <initguid.h>

#include <asyncinfo.h>
#include <combaseapi.h>
#include <commctrl.h>
#include <comutil.h>
#include <endpointvolume.h>
#include <highlevelmonitorconfigurationapi.h>
#include <mmdeviceapi.h>
#include <physicalmonitorenumerationapi.h>
#include <psapi.h>
#include <roapi.h>
#include <servprov.h>
#include <wbemcli.h>
#include <windowsx.h>

#include <algorithm>
#include <atomic>
#include <climits>
#include <mutex>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_set>
#include <vector>

#include <winrt/base.h>

enum class ScrollAction {
    virtualDesktopSwitch,
    brightnessChange,
    micVolumeChange,
};

enum class ScrollArea {
    taskbar,
    notificationArea,
    taskbarWithoutNotificationArea,
    none,
};

struct Region {
    bool isPercentage;
    int start;
    int end;
};

struct ScrollActionEntry {
    ScrollAction scrollAction;
    ScrollArea scrollArea;
    std::vector<Region> additionalScrollRegions;
    int scrollStep;
    int throttleMs;
    bool reverseScrollingDirection;
};

enum class BrightnessMethod {
    native,
    custom,
};

struct {
    std::vector<ScrollActionEntry> scrollActions;
    BrightnessMethod brightnessMethodWmi;
    BrightnessMethod brightnessMethodDdcCi;
    bool oldTaskbarOnWin11;
} g_settings;

std::atomic<bool> g_initialized;
bool g_inputSiteProcHooked;
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

bool IsPointInsideTaskbarScrollArea(HWND hMMTaskbarWnd,
                                    POINT pt,
                                    ScrollArea scrollArea) {
    switch (scrollArea) {
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

        case ScrollArea::none:
            return false;
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

    if (puPtrLen) {
        *puPtrLen = uPtrLen;
    }

    return (VS_FIXEDFILEINFO*)pFixedFileInfo;
}

BOOL WindowsVersionInit() {
    g_nWinVersion = WIN_VERSION_UNSUPPORTED;

    VS_FIXEDFILEINFO* pFixedFileInfo = GetModuleVersionInfo(NULL, NULL);
    if (!pFixedFileInfo) {
        return FALSE;
    }

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
                    if (nQFE < 17000) {
                        g_nWinVersion = WIN_VERSION_81;
                    } else {
                        g_nWinVersion = WIN_VERSION_811;
                    }
                    break;

                case 4:
                    g_nWinVersion = WIN_VERSION_10_T1;
                    break;
            }
            break;

        case 10:
            if (nBuild <= 10240) {
                g_nWinVersion = WIN_VERSION_10_T1;
            } else if (nBuild <= 10586) {
                g_nWinVersion = WIN_VERSION_10_T2;
            } else if (nBuild <= 14393) {
                g_nWinVersion = WIN_VERSION_10_R1;
            } else if (nBuild <= 15063) {
                g_nWinVersion = WIN_VERSION_10_R2;
            } else if (nBuild <= 16299) {
                g_nWinVersion = WIN_VERSION_10_R3;
            } else if (nBuild <= 17134) {
                g_nWinVersion = WIN_VERSION_10_R4;
            } else if (nBuild <= 17763) {
                g_nWinVersion = WIN_VERSION_10_R5;
            } else if (nBuild <= 18362) {
                g_nWinVersion = WIN_VERSION_10_19H1;
            } else if (nBuild <= 19041) {
                g_nWinVersion = WIN_VERSION_10_20H1;
            } else if (nBuild <= 20348) {
                g_nWinVersion = WIN_VERSION_SERVER_2022;
            } else if (nBuild <= 22000) {
                g_nWinVersion = WIN_VERSION_11_21H2;
            } else {
                g_nWinVersion = WIN_VERSION_11_22H2;
            }
            break;
    }

    if (g_nWinVersion == WIN_VERSION_UNSUPPORTED) {
        return FALSE;
    }

    return TRUE;
}

#pragma endregion  // functions

#pragma region regions

// https://stackoverflow.com/a/54364173
std::wstring_view TrimStringView(std::wstring_view s) {
    s.remove_prefix(std::min(s.find_first_not_of(L" \t\r\v\n"), s.size()));
    s.remove_suffix(
        std::min(s.size() - s.find_last_not_of(L" \t\r\v\n") - 1, s.size()));
    return s;
}

// https://stackoverflow.com/a/46931770
std::vector<std::wstring_view> SplitStringView(std::wstring_view s,
                                               std::wstring_view delimiter) {
    size_t pos_start = 0, pos_end, delim_len = delimiter.length();
    std::wstring_view token;
    std::vector<std::wstring_view> res;

    while ((pos_end = s.find(delimiter, pos_start)) !=
           std::wstring_view::npos) {
        token = s.substr(pos_start, pos_end - pos_start);
        pos_start = pos_end + delim_len;
        res.push_back(token);
    }

    res.push_back(s.substr(pos_start));
    return res;
}

bool SvToInt(std::wstring_view s, int* result) {
    if (s.empty()) {
        return false;
    }

    int value = 0;
    for (WCHAR c : s) {
        if (c < L'0' || c > L'9') {
            return false;
        }
        int digit = c - L'0';
        if (value > (INT_MAX - digit) / 10) {
            return false;
        }
        value = value * 10 + digit;
    }

    *result = value;
    return true;
}

std::optional<Region> ParseRegion(std::wstring_view regionStr) {
    auto parts = SplitStringView(regionStr, L"-");
    if (parts.size() != 2) {
        Wh_Log(L"Invalid region (expected start-end): %.*s",
               (int)regionStr.size(), regionStr.data());
        return std::nullopt;
    }

    auto startStr = TrimStringView(parts[0]);
    auto endStr = TrimStringView(parts[1]);

    bool startIsPercentage = !startStr.empty() && startStr.back() == L'%';
    bool endIsPercentage = !endStr.empty() && endStr.back() == L'%';
    if (startIsPercentage != endIsPercentage) {
        Wh_Log(L"Invalid region (mixed percent and pixel): %.*s",
               (int)regionStr.size(), regionStr.data());
        return std::nullopt;
    }

    bool isPercentage = startIsPercentage;
    if (isPercentage) {
        startStr.remove_suffix(1);
        endStr.remove_suffix(1);
    }

    int start;
    int end;
    if (!SvToInt(startStr, &start) || !SvToInt(endStr, &end)) {
        Wh_Log(L"Invalid region (non-numeric values): %.*s",
               (int)regionStr.size(), regionStr.data());
        return std::nullopt;
    }

    if (start >= end) {
        Wh_Log(L"Invalid region (start must be less than end): %.*s",
               (int)regionStr.size(), regionStr.data());
        return std::nullopt;
    }

    return Region{isPercentage, start, end};
}

bool IsPointInsideAdditionalRegion(HWND hMMTaskbarWnd,
                                   POINT pt,
                                   const std::vector<Region>& regions) {
    if (regions.empty()) {
        return false;
    }

    RECT rc;
    if (!GetWindowRect(hMMTaskbarWnd, &rc) || !PtInRect(&rc, pt)) {
        return false;
    }

    bool isHorizontal = (rc.right - rc.left) >= (rc.bottom - rc.top);
    int taskbarLength;
    int cursorOffset;
    if (isHorizontal) {
        taskbarLength = rc.right - rc.left;
        if (GetWindowLong(hMMTaskbarWnd, GWL_EXSTYLE) & WS_EX_LAYOUTRTL) {
            cursorOffset = rc.right - 1 - pt.x;
        } else {
            cursorOffset = pt.x - rc.left;
        }
    } else {
        taskbarLength = rc.bottom - rc.top;
        cursorOffset = pt.y - rc.top;
    }

    UINT dpi = GetDpiForWindowWithFallback(hMMTaskbarWnd);

    for (const auto& region : regions) {
        int start, end;
        if (region.isPercentage) {
            start = MulDiv(taskbarLength, region.start, 100);
            end = MulDiv(taskbarLength, region.end, 100);
        } else {
            start = MulDiv(region.start, dpi, 96);
            end = MulDiv(region.end, dpi, 96);
        }

        if (cursorOffset >= start && cursorOffset <= end) {
            return true;
        }
    }

    return false;
}

bool IsPointInsideEntryScrollArea(HWND hMMTaskbarWnd,
                                  POINT pt,
                                  const ScrollActionEntry& entry) {
    return IsPointInsideTaskbarScrollArea(hMMTaskbarWnd, pt,
                                          entry.scrollArea) ||
           IsPointInsideAdditionalRegion(hMMTaskbarWnd, pt,
                                         entry.additionalScrollRegions);
}

#pragma endregion  // regions

#pragma region brightness

// Reference:
// https://github.com/stefankueng/tools/blob/e7cd50c6ac3a50f6dac84c6aace519349164155e/Misc/AAClr/src/Utils.cpp

int GetBrightnessWmi() {
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
        VariantInit(&var1);
        hr = pObj->Get(_bstr_t(L"CurrentBrightness"), 0, &var1, NULL, NULL);
        pObj->Release();
        if (hr != WBEM_S_NO_ERROR) {
            goto cleanup;
        }

        ret = V_UI1(&var1);

        VariantClear(&var1);
    }

    // Free up resources
cleanup:

    SysFreeString(path);
    SysFreeString(ClassPath);
    SysFreeString(bstrQuery);

    if (pEnum) {
        pEnum->Release();
    }
    if (pLocator) {
        pLocator->Release();
    }
    if (pNamespace) {
        pNamespace->Release();
    }

    return ret;
}

bool SetBrightnessWmi(int val) {
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
    WCHAR buf[10];
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

        if (hr == WBEM_S_FALSE) {
            break;
        }

        if (hr != WBEM_S_NO_ERROR) {
            bRet = false;
            goto cleanup;
        }

        // Call the method

        VARIANT pathVariable;
        VariantInit(&pathVariable);

        hr = pObj->Get(_bstr_t(L"__PATH"), 0, &pathVariable, NULL, NULL);
        pObj->Release();
        if (hr != WBEM_S_NO_ERROR) {
            bRet = false;
            goto cleanup;
        }

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

    if (pEnum) {
        pEnum->Release();
    }
    if (pClass) {
        pClass->Release();
    }
    if (pInInst) {
        pInInst->Release();
    }
    if (pInClass) {
        pInClass->Release();
    }
    if (pLocator) {
        pLocator->Release();
    }
    if (pNamespace) {
        pNamespace->Release();
    }

    return bRet;
}

// DDC/CI brightness control (for external monitors).
// VCP code 0x10 = Luminance (Brightness) per MCCS standard.

bool AdjustBrightnessDdcCi(HMONITOR hMonitor, int delta) {
    DWORD numPhysical = 0;
    if (!GetNumberOfPhysicalMonitorsFromHMONITOR(hMonitor, &numPhysical) ||
        numPhysical == 0) {
        return false;
    }

    std::vector<PHYSICAL_MONITOR> physical(numPhysical);
    if (!GetPhysicalMonitorsFromHMONITOR(hMonitor, numPhysical,
                                         physical.data())) {
        return false;
    }

    bool anySuccess = false;

    for (DWORD i = 0; i < numPhysical; i++) {
        DWORD dwMin = 0;
        DWORD dwCurrent = 0;
        DWORD dwMax = 0;
        if (!GetMonitorBrightness(physical[i].hPhysicalMonitor, &dwMin,
                                  &dwCurrent, &dwMax)) {
            continue;
        }

        DWORD newVal = dwCurrent;
        if (delta > 0) {
            newVal = std::min(dwCurrent + (DWORD)delta, dwMax);
        } else if (delta < 0) {
            DWORD sub = (DWORD)(-delta);
            newVal = sub <= dwCurrent - dwMin ? dwCurrent - sub : dwMin;
        }

        Wh_Log(L"DDC/CI: %s brightness %lu -> %lu (min %lu, max %lu)",
               physical[i].szPhysicalMonitorDescription, dwCurrent, newVal,
               dwMin, dwMax);

        if (SetMonitorBrightness(physical[i].hPhysicalMonitor, newVal)) {
            anySuccess = true;
        }
    }

    DestroyPhysicalMonitors(numPhysical, physical.data());
    return anySuccess;
}

void AdjustBrightnessWmi(int delta) {
    int brightness = GetBrightnessWmi();
    if (brightness >= 0) {
        int newBrightness = std::clamp(brightness + delta, 0, 100);
        Wh_Log(L"WMI: Changing brightness from %d to %d", brightness,
               newBrightness);
        if (!SetBrightnessWmi(newBrightness)) {
            Wh_Log(L"Error setting brightness via WMI");
        }
    } else {
        Wh_Log(L"Error getting brightness via WMI");
    }
}

// The native method uses undocumented Windows interfaces, as found in Windows
// 11 build 26300. If any of them is missing or doesn't match, the native method
// fails and the custom method is used instead.

// The per-display brightness API used by the Settings app. It supports external
// monitors only if the ExternalBrightness Windows feature is enabled.
constexpr WCHAR kDisplayEnhancementManagementDll[] =
    L"Windows.Internal.Graphics.Display.DisplayEnhancementManagement.dll";
constexpr WCHAR kDisplayEnhancementManagementClass[] =
    L"Windows.Internal.Graphics.Display.DisplayEnhancementManagement."
    L"DisplayEnhancementManagement";

const static GUID XIID_IDisplayEnhancementManagementStatics = {
    0x22771028,
    0x1658,
    0x4E5E,
    {0xAB, 0x77, 0x30, 0x36, 0x91, 0xA8, 0x6F, 0xDA}};
const static GUID XIID_IDisplayEnhancementManagement = {
    0xCDA29A3E,
    0x9E7E,
    0x4B86,
    {0x8F, 0x5F, 0x36, 0x8A, 0xA0, 0x71, 0x00, 0x08}};
// IAsyncOperation<DisplayEnhancementManagement>.
const static GUID XIID_IAsyncOperationDisplayEnhancementManagement = {
    0xBD9CB39A,
    0x6F09,
    0x5209,
    {0xA6, 0x56, 0x13, 0x06, 0xF0, 0xDD, 0xF5, 0xC9}};

MIDL_INTERFACE("BD9CB39A-6F09-5209-A656-1306F0DDF5C9")
IAsyncOperationDisplayEnhancementManagement : public IInspectable {
   public:
    virtual HRESULT STDMETHODCALLTYPE put_Completed(IUnknown * handler) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Completed(IUnknown * *handler) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetResults(IInspectable * *results) = 0;
};

// Resolved from symbols, and called only on objects whose vtables point to
// them.
struct {
    HRESULT(STDMETHODCALLTYPE* FromIdAsync)(IInspectable* pThis,
                                            HSTRING id,
                                            IInspectable** operation);
    HRESULT(STDMETHODCALLTYPE* get_IsBrightnessCapable)(IInspectable* pThis,
                                                        BYTE* value);
    HRESULT(STDMETHODCALLTYPE* get_UserBrightnessSettingPercent)(
        IInspectable* pThis,
        ULONG* value);
    HRESULT(STDMETHODCALLTYPE* put_UserBrightnessSettingPercent)(
        IInspectable* pThis,
        ULONG value);
} g_displayEnhancementManagement;

// Vtable indices, IInspectable methods included.
constexpr int kFromIdAsyncIndex = 6;
constexpr int kGetIsBrightnessCapableIndex = 34;
constexpr int kGetUserBrightnessSettingPercentIndex = 46;
constexpr int kPutUserBrightnessSettingPercentIndex = 47;

// The shell component which shows the volume, brightness and other hardware
// indicators.
const static GUID XCLSID_ImmersiveShell = {
    0xC2F03A33,
    0x21F5,
    0x47FA,
    {0xB4, 0xBB, 0x15, 0x63, 0x62, 0xA2, 0xF2, 0x39}};
const static GUID XIID_IAudioFlyoutController = {
    0x41F9D2FB,
    0x7834,
    0x4AB6,
    {0x8B, 0x1B, 0x73, 0xE7, 0x40, 0x64, 0xB4, 0x65}};
const static GUID XIID_IServiceProvider = {
    0x6D5140C1,
    0x7436,
    0x11CE,
    {0x80, 0x34, 0x00, 0xAA, 0x00, 0x60, 0x09, 0xFA}};

MIDL_INTERFACE("41F9D2FB-7834-4AB6-8B1B-73E74064B465")
IAudioFlyoutController : public IUnknown {
   public:
    virtual HRESULT STDMETHODCALLTYPE Show(int type, DWORD arg) = 0;
    virtual HRESULT STDMETHODCALLTYPE ShowWithCustomMessage(int type, DWORD arg,
                                                            PCWSTR message) = 0;
};

// Shows the system brightness level.
constexpr int kFlyoutTypeBrightness = 3;
// The microphone mute indicator. With an argument of 0, it shows only the
// custom message, without an icon.
constexpr int kFlyoutTypeMicrophoneMuted = 7;

// The shell hook code that the system sends for brightness key presses.
constexpr WPARAM kShellHookBrightness = 0x37;

bool g_nativeBrightnessInitAttempted;
bool g_nativeBrightnessInitialized;
HMODULE g_displayEnhancementManagementModule;

// Brightness changes are slow, so they're made by a worker thread. Requests
// that queue up while it's busy are merged.
struct BrightnessRequest {
    HMONITOR hMonitor;
    BrightnessMethod methodWmi;
    BrightnessMethod methodDdcCi;
    int delta;
};

HANDLE g_brightnessThread;
HANDLE g_brightnessRequestEvent;
HANDLE g_brightnessStopEvent;
std::mutex g_brightnessRequestsMutex;
std::vector<BrightnessRequest> g_brightnessRequests;

bool InitNativeBrightness() {
    // The indicator types above are those of the Windows 11 22H2+ indicators.
    WCHAR hardwareConfirmatorPath[MAX_PATH];
    UINT len = GetSystemDirectory(hardwareConfirmatorPath,
                                  ARRAYSIZE(hardwareConfirmatorPath));
    if (!len || len >= ARRAYSIZE(hardwareConfirmatorPath) ||
        wcscat_s(hardwareConfirmatorPath,
                 L"\\Windows.Internal.HardwareConfirmator.dll") != 0 ||
        GetFileAttributes(hardwareConfirmatorPath) == INVALID_FILE_ATTRIBUTES) {
        Wh_Log(L"Native brightness indicators aren't available");
        return false;
    }

    HMODULE module = LoadLibraryEx(kDisplayEnhancementManagementDll, nullptr,
                                   LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!module) {
        Wh_Log(L"Native brightness API isn't available");
        return false;
    }

    g_displayEnhancementManagementModule = module;

    // Windows.Internal.Graphics.Display.DisplayEnhancementManagement.dll
    WindhawkUtils::SYMBOL_HOOK symbolHooks[] = {
        {
            {LR"(public: virtual long __cdecl Windows::Internal::Graphics::Display::DisplayEnhancementManagement::DisplayEnhancementManagementStatics::FromIdAsync(struct HSTRING__ *,struct Windows::Foundation::IAsyncOperation<class Windows::Internal::Graphics::Display::DisplayEnhancementManagement::DisplayEnhancementManagement *> * *))"},
            &g_displayEnhancementManagement.FromIdAsync,
        },
        {
            {LR"(public: virtual long __cdecl Windows::Internal::Graphics::Display::DisplayEnhancementManagement::DisplayEnhancementManagement::get_IsBrightnessCapable(unsigned char *))"},
            &g_displayEnhancementManagement.get_IsBrightnessCapable,
        },
        {
            {LR"(public: virtual long __cdecl Windows::Internal::Graphics::Display::DisplayEnhancementManagement::DisplayEnhancementManagement::get_UserBrightnessSettingPercent(unsigned long *))"},
            &g_displayEnhancementManagement.get_UserBrightnessSettingPercent,
        },
        {
            {LR"(public: virtual long __cdecl Windows::Internal::Graphics::Display::DisplayEnhancementManagement::DisplayEnhancementManagement::put_UserBrightnessSettingPercent(unsigned long))"},
            &g_displayEnhancementManagement.put_UserBrightnessSettingPercent,
        },
    };

    if (!HookSymbols(module, symbolHooks, ARRAYSIZE(symbolHooks))) {
        Wh_Log(L"Native brightness API symbols weren't found");
        return false;
    }

    return true;
}

bool IsVtableEntry(IUnknown* object, int index, void* function) {
    return (*reinterpret_cast<void***>(object))[index] == function;
}

winrt::com_ptr<IInspectable> GetAsyncOperationResults(IInspectable* operation) {
    winrt::com_ptr<IAsyncInfo> asyncInfo;
    if (FAILED(
            operation->QueryInterface(IID_IAsyncInfo, asyncInfo.put_void()))) {
        return nullptr;
    }

    AsyncStatus status = AsyncStatus::Error;
    DWORD startTime = GetTickCount();
    while (SUCCEEDED(asyncInfo->get_Status(&status)) &&
           status == AsyncStatus::Started) {
        if (GetTickCount() - startTime > 5000 ||
            WaitForSingleObject(g_brightnessStopEvent, 10) != WAIT_TIMEOUT) {
            asyncInfo->Cancel();
            return nullptr;
        }
    }

    if (status != AsyncStatus::Completed) {
        return nullptr;
    }

    winrt::com_ptr<IAsyncOperationDisplayEnhancementManagement> typedOperation;
    winrt::com_ptr<IInspectable> results;
    if (FAILED(operation->QueryInterface(
            XIID_IAsyncOperationDisplayEnhancementManagement,
            typedOperation.put_void())) ||
        FAILED(typedOperation->GetResults(results.put()))) {
        return nullptr;
    }

    return results;
}

winrt::com_ptr<IInspectable> GetDisplayEnhancementManagement(
    PCWSTR monitorDevicePath) {
    const auto& functions = g_displayEnhancementManagement;

    winrt::hstring className{kDisplayEnhancementManagementClass};
    winrt::com_ptr<IInspectable> statics;
    HRESULT hr = RoGetActivationFactory(
        static_cast<HSTRING>(winrt::get_abi(className)),
        XIID_IDisplayEnhancementManagementStatics, statics.put_void());
    if (FAILED(hr)) {
        Wh_Log(L"RoGetActivationFactory failed: %08X", hr);
        return nullptr;
    }

    if (!IsVtableEntry(statics.get(), kFromIdAsyncIndex,
                       reinterpret_cast<void*>(functions.FromIdAsync))) {
        Wh_Log(L"Native brightness API mismatch");
        return nullptr;
    }

    winrt::hstring id{monitorDevicePath};
    winrt::com_ptr<IInspectable> operation;
    hr = functions.FromIdAsync(statics.get(),
                               static_cast<HSTRING>(winrt::get_abi(id)),
                               operation.put());
    if (FAILED(hr)) {
        Wh_Log(L"FromIdAsync failed: %08X", hr);
        return nullptr;
    }

    winrt::com_ptr<IInspectable> results =
        GetAsyncOperationResults(operation.get());
    if (!results) {
        Wh_Log(L"FromIdAsync didn't complete for %s", monitorDevicePath);
        return nullptr;
    }

    winrt::com_ptr<IInspectable> displayEnhancementManagement;
    if (FAILED(
            results->QueryInterface(XIID_IDisplayEnhancementManagement,
                                    displayEnhancementManagement.put_void()))) {
        Wh_Log(L"Native brightness API mismatch");
        return nullptr;
    }

    IInspectable* object = displayEnhancementManagement.get();
    if (!IsVtableEntry(
            object, kGetIsBrightnessCapableIndex,
            reinterpret_cast<void*>(functions.get_IsBrightnessCapable)) ||
        !IsVtableEntry(object, kGetUserBrightnessSettingPercentIndex,
                       reinterpret_cast<void*>(
                           functions.get_UserBrightnessSettingPercent)) ||
        !IsVtableEntry(object, kPutUserBrightnessSettingPercentIndex,
                       reinterpret_cast<void*>(
                           functions.put_UserBrightnessSettingPercent))) {
        Wh_Log(L"Native brightness API mismatch");
        return nullptr;
    }

    return displayEnhancementManagement;
}

winrt::com_ptr<IAudioFlyoutController> GetAudioFlyoutController() {
    winrt::com_ptr<IServiceProvider> serviceProvider;
    HRESULT hr =
        CoCreateInstance(XCLSID_ImmersiveShell, nullptr, CLSCTX_LOCAL_SERVER,
                         XIID_IServiceProvider, serviceProvider.put_void());
    if (FAILED(hr)) {
        Wh_Log(L"CoCreateInstance failed: %08X", hr);
        return nullptr;
    }

    winrt::com_ptr<IAudioFlyoutController> flyoutController;
    hr = serviceProvider->QueryService(XIID_IAudioFlyoutController,
                                       XIID_IAudioFlyoutController,
                                       flyoutController.put_void());
    if (FAILED(hr)) {
        Wh_Log(L"QueryService failed: %08X", hr);
        return nullptr;
    }

    return flyoutController;
}

HWND FindTaskSwWindow() {
    HWND hTaskbarWnd = g_hTaskbarWnd;
    if (!hTaskbarWnd) {
        return nullptr;
    }

    HWND hReBarWindow32 =
        FindWindowEx(hTaskbarWnd, nullptr, L"ReBarWindow32", nullptr);
    if (!hReBarWindow32) {
        return nullptr;
    }

    return FindWindowEx(hReBarWindow32, nullptr, L"MSTaskSwWClass", nullptr);
}

struct DisplayTarget {
    std::wstring monitorDevicePath;
    std::wstring monitorFriendlyName;
    bool internal;
};

// Returns the active display targets of the given monitor, or of all monitors
// if it's null.
std::vector<DisplayTarget> GetDisplayTargets(HMONITOR hMonitor) {
    std::vector<DisplayTarget> targets;

    MONITORINFOEX monitorInfo{};
    monitorInfo.cbSize = sizeof(monitorInfo);
    if (hMonitor && !GetMonitorInfo(hMonitor, &monitorInfo)) {
        return targets;
    }

    std::vector<DISPLAYCONFIG_PATH_INFO> paths;
    std::vector<DISPLAYCONFIG_MODE_INFO> modes;
    LONG result;
    do {
        UINT32 pathCount;
        UINT32 modeCount;
        result = GetDisplayConfigBufferSizes(QDC_ONLY_ACTIVE_PATHS, &pathCount,
                                             &modeCount);
        if (result != ERROR_SUCCESS) {
            return targets;
        }

        paths.resize(pathCount);
        modes.resize(modeCount);
        result =
            QueryDisplayConfig(QDC_ONLY_ACTIVE_PATHS, &pathCount, paths.data(),
                               &modeCount, modes.data(), nullptr);
        paths.resize(pathCount);
    } while (result == ERROR_INSUFFICIENT_BUFFER);

    if (result != ERROR_SUCCESS) {
        return targets;
    }

    for (const auto& path : paths) {
        if (hMonitor) {
            DISPLAYCONFIG_SOURCE_DEVICE_NAME sourceName{};
            sourceName.header.type = DISPLAYCONFIG_DEVICE_INFO_GET_SOURCE_NAME;
            sourceName.header.size = sizeof(sourceName);
            sourceName.header.adapterId = path.sourceInfo.adapterId;
            sourceName.header.id = path.sourceInfo.id;
            if (DisplayConfigGetDeviceInfo(&sourceName.header) !=
                    ERROR_SUCCESS ||
                _wcsicmp(sourceName.viewGdiDeviceName, monitorInfo.szDevice) !=
                    0) {
                continue;
            }
        }

        DISPLAYCONFIG_TARGET_DEVICE_NAME targetName{};
        targetName.header.type = DISPLAYCONFIG_DEVICE_INFO_GET_TARGET_NAME;
        targetName.header.size = sizeof(targetName);
        targetName.header.adapterId = path.targetInfo.adapterId;
        targetName.header.id = path.targetInfo.id;
        if (DisplayConfigGetDeviceInfo(&targetName.header) != ERROR_SUCCESS ||
            !*targetName.monitorDevicePath) {
            continue;
        }

        auto tech = targetName.outputTechnology;
        targets.push_back({
            .monitorDevicePath = targetName.monitorDevicePath,
            .monitorFriendlyName = targetName.monitorFriendlyDeviceName,
            .internal =
                tech == DISPLAYCONFIG_OUTPUT_TECHNOLOGY_INTERNAL ||
                tech == DISPLAYCONFIG_OUTPUT_TECHNOLOGY_DISPLAYPORT_EMBEDDED ||
                tech == DISPLAYCONFIG_OUTPUT_TECHNOLOGY_UDI_EMBEDDED,
        });
    }

    return targets;
}

// For internal displays, adjusts all of them, like the WMI method does. For
// external monitors, adjusts the ones of the given monitor.
bool AdjustBrightnessNative(bool internal, HMONITOR hMonitor, int delta) {
    if (!g_nativeBrightnessInitialized) {
        return false;
    }

    std::vector<DisplayTarget> targets;
    for (auto& target : GetDisplayTargets(internal ? nullptr : hMonitor)) {
        if (target.internal == internal) {
            targets.push_back(std::move(target));
        }
    }

    if (targets.empty()) {
        return false;
    }

    // Make sure that an indicator can be shown before changing the brightness.
    HWND hTaskSwWnd = internal ? FindTaskSwWindow() : nullptr;
    winrt::com_ptr<IAudioFlyoutController> flyoutController;
    if (!hTaskSwWnd) {
        flyoutController = GetAudioFlyoutController();
        if (!flyoutController) {
            return false;
        }
    }

    const auto& functions = g_displayEnhancementManagement;
    std::wstring message;
    bool adjusted = false;

    for (const auto& target : targets) {
        winrt::com_ptr<IInspectable> displayEnhancementManagement =
            GetDisplayEnhancementManagement(target.monitorDevicePath.c_str());
        if (!displayEnhancementManagement) {
            continue;
        }

        IInspectable* object = displayEnhancementManagement.get();

        BYTE capable;
        ULONG current;
        if (FAILED(functions.get_IsBrightnessCapable(object, &capable)) ||
            !capable ||
            FAILED(
                functions.get_UserBrightnessSettingPercent(object, &current)) ||
            current > 100) {
            Wh_Log(L"Native: %s isn't brightness capable",
                   target.monitorDevicePath.c_str());
            continue;
        }

        ULONG newValue = std::clamp((int)current + delta, 0, 100);

        Wh_Log(L"Native: %s brightness %lu -> %lu",
               target.monitorDevicePath.c_str(), current, newValue);

        if (newValue != current &&
            FAILED(
                functions.put_UserBrightnessSettingPercent(object, newValue))) {
            Wh_Log(L"Native: Error setting brightness");
            continue;
        }

        adjusted = true;

        if (!message.empty()) {
            message += L", ";
        }

        if (target.monitorFriendlyName.empty()) {
            message += L"Brightness";
        } else {
            message += target.monitorFriendlyName;
            message += L" brightness";
        }

        message += L": ";
        message += std::to_wstring(newValue);
        message += L"%";
    }

    if (!adjusted) {
        return false;
    }

    if (!internal) {
        flyoutController->ShowWithCustomMessage(kFlyoutTypeMicrophoneMuted, 0,
                                                message.c_str());
    } else if (hTaskSwWnd) {
        // Same as a brightness key press.
        static const UINT shellHookMsg = RegisterWindowMessage(L"SHELLHOOK");
        PostMessage(hTaskSwWnd, shellHookMsg, kShellHookBrightness, 0);
    } else {
        flyoutController->Show(kFlyoutTypeBrightness, 0);
    }

    return true;
}

bool AdjustBrightnessExternal(HMONITOR hMonitor,
                              int delta,
                              BrightnessMethod method) {
    if (method == BrightnessMethod::native &&
        AdjustBrightnessNative(/*internal=*/false, hMonitor, delta)) {
        return true;
    }

    return AdjustBrightnessDdcCi(hMonitor, delta);
}

void AdjustBrightnessInternal(int delta, BrightnessMethod method) {
    if (method == BrightnessMethod::native &&
        AdjustBrightnessNative(/*internal=*/true, nullptr, delta)) {
        return;
    }

    AdjustBrightnessWmi(delta);
}

void AdjustBrightness(const BrightnessRequest& request) {
    if (!request.hMonitor ||
        !AdjustBrightnessExternal(request.hMonitor, request.delta,
                                  request.methodDdcCi)) {
        AdjustBrightnessInternal(request.delta, request.methodWmi);
    }
}

DWORD WINAPI BrightnessThread(LPVOID) {
    HRESULT hrCoInit = CoInitializeEx(nullptr, COINIT_MULTITHREADED);

    HANDLE events[] = {g_brightnessStopEvent, g_brightnessRequestEvent};
    while (WaitForMultipleObjects(ARRAYSIZE(events), events, FALSE, INFINITE) ==
           WAIT_OBJECT_0 + 1) {
        std::vector<BrightnessRequest> requests;
        {
            std::lock_guard<std::mutex> lock(g_brightnessRequestsMutex);
            requests.swap(g_brightnessRequests);
        }

        for (const auto& request : requests) {
            if (WaitForSingleObject(g_brightnessStopEvent, 0) != WAIT_TIMEOUT) {
                break;
            }

            AdjustBrightness(request);
        }
    }

    if (SUCCEEDED(hrCoInit)) {
        CoUninitialize();
    }

    return 0;
}

void BrightnessThreadInit() {
    g_brightnessRequestEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    g_brightnessStopEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
    if (g_brightnessRequestEvent && g_brightnessStopEvent) {
        g_brightnessThread =
            CreateThread(nullptr, 0, BrightnessThread, nullptr, 0, nullptr);
    }

    if (!g_brightnessThread) {
        Wh_Log(L"Failed to create the brightness thread");
    }
}

void BrightnessThreadUninit() {
    if (g_brightnessThread) {
        SetEvent(g_brightnessStopEvent);
        WaitForSingleObject(g_brightnessThread, INFINITE);
        CloseHandle(g_brightnessThread);
        g_brightnessThread = nullptr;
    }

    if (g_brightnessRequestEvent) {
        CloseHandle(g_brightnessRequestEvent);
        g_brightnessRequestEvent = nullptr;
    }

    if (g_brightnessStopEvent) {
        CloseHandle(g_brightnessStopEvent);
        g_brightnessStopEvent = nullptr;
    }
}

void RequestBrightnessChange(const BrightnessRequest& request) {
    if (!g_brightnessThread) {
        return;
    }

    {
        std::lock_guard<std::mutex> lock(g_brightnessRequestsMutex);
        auto it = std::find_if(
            g_brightnessRequests.begin(), g_brightnessRequests.end(),
            [&request](const BrightnessRequest& pending) {
                return pending.hMonitor == request.hMonitor &&
                       pending.methodWmi == request.methodWmi &&
                       pending.methodDdcCi == request.methodDdcCi;
            });
        if (it != g_brightnessRequests.end()) {
            it->delta += request.delta;
        } else {
            g_brightnessRequests.push_back(request);
        }
    }

    SetEvent(g_brightnessRequestEvent);
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
    if (FAILED(hr)) {
        g_pDeviceEnumerator = NULL;
    }
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

                    if (fMasterVolume < 0.0) {
                        fMasterVolume = 0.0;
                    } else if (fMasterVolume > 1.0) {
                        fMasterVolume = 1.0;
                    }

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
int g_lastScrollActionIndex = -1;

void InvokeScrollAction(HWND hWnd,
                        WPARAM wParam,
                        LPARAM lMousePosParam,
                        int entryIndex) {
    const auto& entry = g_settings.scrollActions[entryIndex];

    if (entryIndex != g_lastScrollActionIndex) {
        g_lastScrollTime = 0;
        g_lastScrollDeltaRemainder = 0;
        g_lastActionTime = 0;
        g_lastScrollActionIndex = entryIndex;
    }

    int delta = GET_WHEEL_DELTA_WPARAM(wParam) * entry.scrollStep;

    if (entry.reverseScrollingDirection) {
        delta = -delta;
    }

    if (GetTickCount() - g_lastScrollTime < 1000 * 5) {
        delta += g_lastScrollDeltaRemainder;
    }

    int clicks = delta / WHEEL_DELTA;
    Wh_Log(L"%d clicks (delta=%d)", clicks, delta);

    if (clicks != 0 && entry.throttleMs > 0) {
        if (GetTickCount() - g_lastActionTime < (DWORD)entry.throttleMs) {
            // It's too soon, ignore this scroll event.
            clicks = 0;

            // Reset remainder too.
            delta = 0;
        } else if (clicks < -1 || clicks > 1) {
            // Throttle to a single action at a time.
            clicks = clicks > 0 ? 1 : -1;

            // Reset remainder if going too fast.
            delta = 0;
        }
    }

    if (clicks != 0) {
        switch (entry.scrollAction) {
            case ScrollAction::virtualDesktopSwitch:
                SwitchDesktopViaKeyboardShortcut(clicks);
                break;

            case ScrollAction::brightnessChange:
                RequestBrightnessChange({
                    .hMonitor =
                        MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST),
                    .methodWmi = g_settings.brightnessMethodWmi,
                    .methodDdcCi = g_settings.brightnessMethodDdcCi,
                    .delta = clicks,
                });
                break;

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

bool OnMouseWheel(HWND hWnd, WPARAM wParam, LPARAM lParam) {
    if (GetCapture()) {
        return false;
    }

    POINT pt;
    pt.x = GET_X_LPARAM(lParam);
    pt.y = GET_Y_LPARAM(lParam);

    for (int i = 0; i < (int)g_settings.scrollActions.size(); i++) {
        if (IsPointInsideEntryScrollArea(hWnd, pt,
                                         g_settings.scrollActions[i])) {
            // Allows to steal focus.
            INPUT input;
            ZeroMemory(&input, sizeof(INPUT));
            SendInput(1, &input, sizeof(INPUT));

            InvokeScrollAction(hWnd, wParam, lParam, i);
            return true;
        }
    }

    return false;
}

LRESULT CALLBACK TaskbarWindowSubclassProc(_In_ HWND hWnd,
                                           _In_ UINT uMsg,
                                           _In_ WPARAM wParam,
                                           _In_ LPARAM lParam,
                                           _In_ DWORD_PTR dwRefData) {
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
    WindhawkUtils::SetWindowSubclassFromAnyThread(hWnd,
                                                  TaskbarWindowSubclassProc, 0);
}

void UnsubclassTaskbarWindow(HWND hWnd) {
    WindhawkUtils::RemoveWindowSubclassFromAnyThread(hWnd,
                                                     TaskbarWindowSubclassProc);
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
    WindhawkUtils::SetFunctionHook(wndProc, InputSiteWindowProc_Hook,
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
                dwProcessId != GetCurrentProcessId()) {
                return TRUE;
            }

            WCHAR szClassName[32];
            if (GetClassName(hWnd, szClassName, ARRAYSIZE(szClassName)) == 0) {
                return TRUE;
            }

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
    if (!hWnd) {
        return hWnd;
    }

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
    if (!hWnd) {
        return hWnd;
    }

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
    g_settings.scrollActions.clear();

    for (int i = 0;; i++) {
        PCWSTR scrollAction =
            Wh_GetStringSetting(L"ScrollActions[%d].scrollAction", i);
        PCWSTR scrollArea =
            Wh_GetStringSetting(L"ScrollActions[%d].scrollArea", i);
        bool hasEntry = *scrollAction || *scrollArea;

        if (!hasEntry) {
            Wh_FreeStringSetting(scrollAction);
            Wh_FreeStringSetting(scrollArea);
            break;
        }

        ScrollActionEntry entry{};

        entry.scrollAction = ScrollAction::virtualDesktopSwitch;
        if (wcscmp(scrollAction, L"brightnessChange") == 0) {
            entry.scrollAction = ScrollAction::brightnessChange;
        } else if (wcscmp(scrollAction, L"micVolumeChange") == 0) {
            entry.scrollAction = ScrollAction::micVolumeChange;
        }
        Wh_FreeStringSetting(scrollAction);

        entry.scrollArea = ScrollArea::taskbar;
        if (wcscmp(scrollArea, L"notificationArea") == 0) {
            entry.scrollArea = ScrollArea::notificationArea;
        } else if (wcscmp(scrollArea, L"taskbarWithoutNotificationArea") == 0) {
            entry.scrollArea = ScrollArea::taskbarWithoutNotificationArea;
        } else if (wcscmp(scrollArea, L"none") == 0) {
            entry.scrollArea = ScrollArea::none;
        }
        Wh_FreeStringSetting(scrollArea);

        PCWSTR additionalScrollRegions = Wh_GetStringSetting(
            L"ScrollActions[%d].additionalScrollRegions", i);
        for (auto regionStr : SplitStringView(additionalScrollRegions, L",")) {
            regionStr = TrimStringView(regionStr);
            if (regionStr.empty()) {
                continue;
            }
            if (auto region = ParseRegion(regionStr)) {
                entry.additionalScrollRegions.push_back(*region);
            }
        }
        Wh_FreeStringSetting(additionalScrollRegions);

        entry.scrollStep =
            std::max(1, Wh_GetIntSetting(L"ScrollActions[%d].scrollStep", i));
        entry.throttleMs = Wh_GetIntSetting(L"ScrollActions[%d].throttleMs", i);
        entry.reverseScrollingDirection =
            Wh_GetIntSetting(L"ScrollActions[%d].reverseScrollingDirection", i);

        g_settings.scrollActions.push_back(std::move(entry));
    }

    PCWSTR brightnessMethodWmi = Wh_GetStringSetting(L"brightnessMethodWmi");
    g_settings.brightnessMethodWmi = wcscmp(brightnessMethodWmi, L"custom") == 0
                                         ? BrightnessMethod::custom
                                         : BrightnessMethod::native;
    Wh_FreeStringSetting(brightnessMethodWmi);

    PCWSTR brightnessMethodDdcCi =
        Wh_GetStringSetting(L"brightnessMethodDdcCi");
    g_settings.brightnessMethodDdcCi =
        wcscmp(brightnessMethodDdcCi, L"custom") == 0
            ? BrightnessMethod::custom
            : BrightnessMethod::native;
    Wh_FreeStringSetting(brightnessMethodDdcCi);

    g_settings.oldTaskbarOnWin11 = Wh_GetIntSetting(L"oldTaskbarOnWin11");
}

bool IsNativeBrightnessNeeded() {
    if (g_settings.brightnessMethodWmi != BrightnessMethod::native &&
        g_settings.brightnessMethodDdcCi != BrightnessMethod::native) {
        return false;
    }

    return std::any_of(g_settings.scrollActions.begin(),
                       g_settings.scrollActions.end(), [](const auto& entry) {
                           return entry.scrollAction ==
                                  ScrollAction::brightnessChange;
                       });
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

    if (IsNativeBrightnessNeeded()) {
        g_nativeBrightnessInitAttempted = true;
        g_nativeBrightnessInitialized = InitNativeBrightness();
    }

    BrightnessThreadInit();

    WindhawkUtils::SetFunctionHook(CreateWindowExW, CreateWindowExW_Hook,
                                   &CreateWindowExW_Original);

    HMODULE user32Module =
        LoadLibraryEx(L"user32.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (user32Module) {
        auto pCreateWindowInBand = (CreateWindowInBand_t)GetProcAddress(
            user32Module, "CreateWindowInBand");
        if (pCreateWindowInBand) {
            WindhawkUtils::SetFunctionHook(pCreateWindowInBand,
                                           CreateWindowInBand_Hook,
                                           &CreateWindowInBand_Original);
        }
    }

    HandleLoadedExplorerPatcher();

    HMODULE kernelBaseModule = GetModuleHandle(L"kernelbase.dll");
    auto pKernelBaseLoadLibraryExW = (decltype(&LoadLibraryExW))GetProcAddress(
        kernelBaseModule, "LoadLibraryExW");
    WindhawkUtils::SetFunctionHook(pKernelBaseLoadLibraryExW,
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

    BrightnessThreadUninit();

    if (g_displayEnhancementManagementModule) {
        FreeLibrary(g_displayEnhancementManagementModule);
        g_displayEnhancementManagementModule = nullptr;
    }

    MicVolUninit();
}

BOOL Wh_ModSettingsChanged(BOOL* bReload) {
    Wh_Log(L">");

    bool prevOldTaskbarOnWin11 = g_settings.oldTaskbarOnWin11;

    LoadSettings();

    // Native brightness is only initialized on init.
    *bReload = g_settings.oldTaskbarOnWin11 != prevOldTaskbarOnWin11 ||
               (IsNativeBrightnessNeeded() && !g_nativeBrightnessInitAttempted);

    return TRUE;
}
