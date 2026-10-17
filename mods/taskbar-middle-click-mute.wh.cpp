// ==WindhawkMod==
// @id              taskbar-middle-click-mute
// @name            Taskbar Middle Click Mute
// @description     Toggle the system volume mute state by middle-clicking the taskbar
// @version         1.0.0
// @author          venasataa
// @github          https://github.com/venasataa
// @homepage        https://venasataa.github.io
// @include         explorer.exe
// @compilerOptions -lcomctl32 -ldwmapi -lgdi32 -lole32 -loleaut32 -loleacc -lshell32 -lversion
// ==/WindhawkMod==

// Source code is published under The GNU General Public License v3.0.

// ==WindhawkModReadme==
/*
Toggle mute/unmute by pressing the mouse wheel (middle click) on the taskbar.

Middle click modes:

1. Empty taskbar area only
   Only blank taskbar space toggles mute.

2. Volume icon only
   Only the volume icon toggles mute.
   On Windows 11, this uses the SystemTray volume icon hook when available.

3. Entire taskbar without application buttons
   The whole taskbar toggles mute, except application buttons and, for
   compatibility, the whole classic application button band.

4. Entire taskbar
   The whole taskbar toggles mute, while application buttons are still
   excluded on a best-effort basis using accessibility and toolbar hit-testing.

Windows version setting:

Automatic:
   Detect the running Windows version automatically.

Windows 10:
   Force Windows 10 taskbar compatibility behavior.

Windows 11:
   Force Windows 11 taskbar compatibility behavior.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
# The taskbar area where a middle click (mouse wheel click) toggles mute:
- clickArea: taskbarWithoutAppButtons
  $options:
  - emptyArea: Empty taskbar area only
  - volumeIcon: Volume icon only
  - taskbarWithoutAppButtons: Entire taskbar without application buttons
  - entireTaskbar: Entire taskbar
# Windows version used for taskbar compatibility behavior:
- windowsVersion: automatic
  $options:
  - automatic: Automatic (detect the running Windows version)
  - windows10: Windows 10 taskbar behavior
  - windows11: Windows 11 taskbar behavior
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <dwmapi.h>
#include <endpointvolume.h>
#include <mmdeviceapi.h>
#include <objbase.h>
#include <shellapi.h>
#include <oleacc.h>
#include <oleauto.h>
#include <psapi.h>
#include <windhawk_utils.h>

#include <atomic>

#ifndef ROLE_SYSTEM_PUSHBUTTON
#define ROLE_SYSTEM_PUSHBUTTON 0x2b
#endif

#ifndef ROLE_SYSTEM_LINK
#define ROLE_SYSTEM_LINK 0x1e
#endif

#ifndef ROLE_SYSTEM_SPLITBUTTON
#define ROLE_SYSTEM_SPLITBUTTON 0x38
#endif

#ifndef ROLE_SYSTEM_BUTTONMENU
#define ROLE_SYSTEM_BUTTONMENU 0x3a
#endif

#ifndef ROLE_SYSTEM_MENUITEM
#define ROLE_SYSTEM_MENUITEM 0x0c
#endif

enum class ClickArea {
    emptyArea,
    volumeIcon,
    taskbarWithoutAppButtons,
    entireTaskbar,
};

enum class WindowsVersionOverride {
    automatic,
    windows10,
    windows11,
};

struct {
    ClickArea clickArea;
    WindowsVersionOverride windowsVersion;
} g_settings;

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

int g_nWinVersion;
int g_nExplorerVersion;

std::atomic<bool> g_systemTrayModuleHooked;
HANDLE g_workerThread = nullptr;

bool g_suppressMiddleUp = false;
DWORD g_lastMiddleToggleTime = 0;

////////////////////////////////////////////////////////////
// Basic helpers.

VS_FIXEDFILEINFO* GetModuleVersionInfo(HMODULE hModule, UINT* puPtrLen) {
    HRSRC hResource;
    HGLOBAL hGlobal;
    void* pData;
    void* pFixedFileInfo;
    UINT uPtrLen;

    pFixedFileInfo = nullptr;
    uPtrLen = 0;

    hResource =
        FindResource(hModule, MAKEINTRESOURCE(VS_VERSION_INFO), RT_VERSION);
    if (hResource != nullptr) {
        hGlobal = LoadResource(hModule, hResource);
        if (hGlobal != nullptr) {
            pData = LockResource(hGlobal);
            if (pData != nullptr) {
                if (!VerQueryValue(pData, L"\\", &pFixedFileInfo, &uPtrLen) ||
                    uPtrLen == 0) {
                    pFixedFileInfo = nullptr;
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

    VS_FIXEDFILEINFO* pFixedFileInfo = GetModuleVersionInfo(nullptr, nullptr);
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
        HDC hdc = GetDC(nullptr);
        if (hdc) {
            iDpi = GetDeviceCaps(hdc, LOGPIXELSX);
            ReleaseDC(nullptr, hdc);
        }
    }

    return iDpi;
}

bool IsTaskbarWindow(HWND hWnd) {
    if (!hWnd) {
        return false;
    }

    WCHAR szClassName[32];
    if (!GetClassName(hWnd, szClassName, ARRAYSIZE(szClassName))) {
        return false;
    }

    return _wcsicmp(szClassName, L"Shell_TrayWnd") == 0 ||
           _wcsicmp(szClassName, L"Shell_SecondaryTrayWnd") == 0;
}

bool IsPrimaryTaskbar(HWND hWnd) {
    if (!hWnd) {
        return false;
    }

    WCHAR szClassName[32];
    if (!GetClassName(hWnd, szClassName, ARRAYSIZE(szClassName))) {
        return false;
    }

    return _wcsicmp(szClassName, L"Shell_TrayWnd") == 0;
}

////////////////////////////////////////////////////////////
// Volume functions.

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

BOOL ToggleVolMuted() {
    HRESULT hrCom = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    bool needCoUninitialize = SUCCEEDED(hrCom);

    BOOL bSuccess = FALSE;
    IMMDeviceEnumerator* pDeviceEnumerator = nullptr;
    IMMDevice* pDefaultDevice = nullptr;
    IAudioEndpointVolume* pEndpointVolume = nullptr;

    HRESULT hr = CoCreateInstance(XIID_MMDeviceEnumerator, nullptr,
                                  CLSCTX_INPROC_SERVER,
                                  XIID_IMMDeviceEnumerator,
                                  (LPVOID*)&pDeviceEnumerator);
    if (SUCCEEDED(hr)) {
        hr = pDeviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole,
                                                        &pDefaultDevice);
        if (SUCCEEDED(hr)) {
            hr = pDefaultDevice->Activate(XIID_IAudioEndpointVolume,
                                          CLSCTX_INPROC_SERVER, nullptr,
                                          (LPVOID*)&pEndpointVolume);
            if (SUCCEEDED(hr)) {
                BOOL bMuted = FALSE;
                if (SUCCEEDED(pEndpointVolume->GetMute(&bMuted))) {
                    if (SUCCEEDED(pEndpointVolume->SetMute(!bMuted, nullptr))) {
                        bSuccess = TRUE;
                    }
                }

                pEndpointVolume->Release();
            }

            pDefaultDevice->Release();
        }

        pDeviceEnumerator->Release();
    }

    if (needCoUninitialize) {
        CoUninitialize();
    }

    return bSuccess;
}

////////////////////////////////////////////////////////////
// Taskbar geometry.

bool GetNotificationAreaRect(HWND hMMTaskbarWnd, RECT* rcResult) {
    if (!hMMTaskbarWnd || !rcResult) {
        return false;
    }

    if (IsPrimaryTaskbar(hMMTaskbarWnd)) {
        HWND hTrayNotifyWnd =
            FindWindowEx(hMMTaskbarWnd, nullptr, L"TrayNotifyWnd", nullptr);
        if (hTrayNotifyWnd && GetWindowRect(hTrayNotifyWnd, rcResult) &&
            !IsRectEmpty(rcResult)) {
            return true;
        }
    } else if (g_nExplorerVersion >= WIN_VERSION_11_21H2) {
        RECT rcTaskbar;
        if (GetWindowRect(hMMTaskbarWnd, &rcTaskbar)) {
            HWND hBridgeWnd = FindWindowEx(
                hMMTaskbarWnd, nullptr,
                L"Windows.UI.Composition.DesktopWindowContentBridge", nullptr);

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
                    L"Windows.UI.Composition.DesktopWindowContentBridge",
                    nullptr);
            }
        }
    } else if (g_nExplorerVersion >= WIN_VERSION_10_R1) {
        HWND hClockButtonWnd =
            FindWindowEx(hMMTaskbarWnd, nullptr, L"ClockButton", nullptr);
        if (hClockButtonWnd && GetWindowRect(hClockButtonWnd, rcResult) &&
            !IsRectEmpty(rcResult)) {
            return true;
        }
    } else {
        // In older Windows versions, there is no clock on secondary taskbars.
        SetRectEmpty(rcResult);
        return true;
    }

    RECT rcTaskbar;
    if (!GetWindowRect(hMMTaskbarWnd, &rcTaskbar)) {
        return false;
    }

    // Fallback: treat the last pixels of the taskbar as the notification area.
    int lastPixels =
        MulDiv(50, GetDpiForWindowWithFallback(hMMTaskbarWnd), 96);

    CopyRect(rcResult, &rcTaskbar);
    if (rcResult->right - rcResult->left > lastPixels) {
        if (GetWindowLongPtr(hMMTaskbarWnd, GWL_EXSTYLE) & WS_EX_LAYOUTRTL) {
            rcResult->right = rcResult->left + lastPixels;
        } else {
            rcResult->left = rcResult->right - lastPixels;
        }
    }

    return true;
}

bool IsPointInsideNotificationArea(HWND hTaskbarWnd, POINT pt) {
    RECT rc;
    return GetNotificationAreaRect(hTaskbarWnd, &rc) && PtInRect(&rc, pt);
}

////////////////////////////////////////////////////////////
// Volume icon hit test.

// Volume notification icon GUID used also by Taskbar Volume Control.
const GUID GUID_VolumeNotificationIcon = {
    0x7820AE73,
    0x23E3,
    0x4229,
    {0x82, 0xC1, 0xE4, 0x1C, 0xB6, 0x7D, 0x5B, 0x9C}};

bool IsPointOverVolumeIcon(POINT pt) {
    NOTIFYICONIDENTIFIER notifyIconIdentifier = {};
    notifyIconIdentifier.cbSize = sizeof(notifyIconIdentifier);
    notifyIconIdentifier.guidItem = GUID_VolumeNotificationIcon;

    RECT rc;
    if (Shell_NotifyIconGetRect(&notifyIconIdentifier, &rc) == S_OK &&
        PtInRect(&rc, pt)) {
        return true;
    }

    return false;
}

////////////////////////////////////////////////////////////
// Accessibility and toolbar hit tests.

bool IsPointOverAccessibleButtonOrLink(POINT pt) {
    IAccessible* pAccessible = nullptr;
    VARIANT varChild;
    VariantInit(&varChild);

    HRESULT hr = AccessibleObjectFromPoint(pt, &pAccessible, &varChild);
    if (FAILED(hr) || !pAccessible) {
        if (pAccessible) {
            pAccessible->Release();
        }

        VariantClear(&varChild);
        return false;
    }

    if (varChild.vt != VT_I4) {
        VariantClear(&varChild);
        varChild.vt = VT_I4;
        varChild.lVal = CHILDID_SELF;
    }

    bool result = false;
    VARIANT varRole;
    VariantInit(&varRole);

    if (SUCCEEDED(pAccessible->get_accRole(varChild, &varRole)) &&
        varRole.vt == VT_I4) {
        long role = varRole.lVal;

        if (role == ROLE_SYSTEM_PUSHBUTTON || role == ROLE_SYSTEM_LINK ||
            role == ROLE_SYSTEM_SPLITBUTTON || role == ROLE_SYSTEM_BUTTONMENU ||
            role == ROLE_SYSTEM_MENUITEM) {
            result = true;
        }
    }

    VariantClear(&varRole);
    pAccessible->Release();
    VariantClear(&varChild);

    return result;
}

struct AppBandEnumData {
    POINT pt;
    bool found;
};

BOOL CALLBACK EnumAppBandProc(HWND hWnd, LPARAM lParam) {
    AppBandEnumData* data = (AppBandEnumData*)lParam;

    WCHAR szClassName[64];
    if (GetClassName(hWnd, szClassName, ARRAYSIZE(szClassName))) {
        if (_wcsicmp(szClassName, L"MSTaskSwWClass") == 0 ||
            _wcsicmp(szClassName, L"MSTaskListWClass") == 0 ||
            _wcsicmp(szClassName, L"MSTaskListWnd") == 0 ||
            _wcsicmp(szClassName, L"TaskListWnd") == 0) {
            RECT rc;
            if (GetWindowRect(hWnd, &rc) && PtInRect(&rc, data->pt)) {
                data->found = true;
                return FALSE;
            }
        }
    }

    return TRUE;
}

bool IsPointInsideApplicationBand(HWND hTaskbarWnd, POINT pt) {
    AppBandEnumData data = {pt, false};
    EnumChildWindows(hTaskbarWnd, EnumAppBandProc, (LPARAM)&data);
    return data.found;
}

struct FindTaskListData {
    HWND found;
};

BOOL CALLBACK EnumFindTaskListProc(HWND hWnd, LPARAM lParam) {
    FindTaskListData* data = (FindTaskListData*)lParam;

    WCHAR szClassName[64];
    if (GetClassName(hWnd, szClassName, ARRAYSIZE(szClassName))) {
        if (_wcsicmp(szClassName, L"MSTaskListWClass") == 0 ||
            _wcsicmp(szClassName, L"MSTaskListWnd") == 0 ||
            _wcsicmp(szClassName, L"TaskListWnd") == 0) {
            data->found = hWnd;
            return FALSE;
        }
    }

    return TRUE;
}

HWND FindTaskListWindow(HWND hTaskbarWnd) {
    FindTaskListData data = {nullptr};
    EnumChildWindows(hTaskbarWnd, EnumFindTaskListProc, (LPARAM)&data);
    return data.found;
}

bool IsPointOverToolbarButton(HWND hToolbarWnd, POINT ptScreen) {
    RECT rc;
    if (!GetWindowRect(hToolbarWnd, &rc) || !PtInRect(&rc, ptScreen)) {
        return false;
    }

    int count = (int)SendMessage(hToolbarWnd, TB_BUTTONCOUNT, 0, 0);
    if (count <= 0) {
        return false;
    }

    POINT ptClient = ptScreen;
    if (!ScreenToClient(hToolbarWnd, &ptClient)) {
        return false;
    }

    for (int i = 0; i < count; i++) {
        RECT rcItem;
        if (SendMessage(hToolbarWnd, TB_GETITEMRECT, i, (LPARAM)&rcItem)) {
            if (PtInRect(&rcItem, ptClient)) {
                return true;
            }
        }
    }

    return false;
}

HWND GetTaskbarRootFromPoint(POINT pt) {
    HWND hWnd = WindowFromPoint(pt);
    if (!hWnd) {
        return nullptr;
    }

    HWND hRootWnd = GetAncestor(hWnd, GA_ROOT);
    if (hRootWnd && IsTaskbarWindow(hRootWnd)) {
        return hRootWnd;
    }

    return nullptr;
}

bool IsPointOverApplicationButton(HWND hTaskbarWnd, POINT pt) {
    // Tray buttons are not application buttons.
    if (IsPointInsideNotificationArea(hTaskbarWnd, pt)) {
        return false;
    }

    // The volume icon is not an application button.
    if (IsPointOverVolumeIcon(pt)) {
        return false;
    }

    // For classic taskbars, try an accurate toolbar hit test first.
    HWND hTaskListWnd = FindTaskListWindow(hTaskbarWnd);
    if (hTaskListWnd) {
        RECT rcTaskList;
        if (GetWindowRect(hTaskListWnd, &rcTaskList) &&
            PtInRect(&rcTaskList, pt)) {
            int count = (int)SendMessage(hTaskListWnd, TB_BUTTONCOUNT, 0, 0);
            if (count > 0) {
                return IsPointOverToolbarButton(hTaskListWnd, pt);
            }
        }
    }

    // Fallback: any accessible button/link outside the notification area is
    // treated as an application or system taskbar button.
    return IsPointOverAccessibleButtonOrLink(pt);
}

////////////////////////////////////////////////////////////
// Decision logic.

bool ShouldToggleAtPoint(POINT pt) {
    HWND hTaskbarWnd = GetTaskbarRootFromPoint(pt);
    if (!hTaskbarWnd) {
        return false;
    }

    RECT rcTaskbar;
    if (!GetWindowRect(hTaskbarWnd, &rcTaskbar) ||
        !PtInRect(&rcTaskbar, pt)) {
        return false;
    }

    bool volumeIcon = IsPointOverVolumeIcon(pt);

    if (g_settings.clickArea == ClickArea::volumeIcon) {
        // If the Windows 11 SystemTray hook is active, let it handle the
        // volume icon click.
        if (g_systemTrayModuleHooked) {
            return false;
        }

        return volumeIcon;
    }

    bool notifyArea = IsPointInsideNotificationArea(hTaskbarWnd, pt);
    bool appBand = IsPointInsideApplicationBand(hTaskbarWnd, pt);
    bool appButton = IsPointOverApplicationButton(hTaskbarWnd, pt);
    bool accessibleInteractive = IsPointOverAccessibleButtonOrLink(pt);
    bool interactive = appButton || accessibleInteractive;

    switch (g_settings.clickArea) {
        case ClickArea::emptyArea:
            // Only blank taskbar area, no buttons and no tray area.
            return !interactive && !notifyArea;

        case ClickArea::taskbarWithoutAppButtons:
            // Strict mode: exclude actual app buttons and the classic app band.
            return !appButton && !appBand;

        case ClickArea::entireTaskbar:
            // Entire taskbar, but still exclude actual application buttons
            // on a best-effort basis.
            return !appButton;

        default:
            return false;
    }
}

////////////////////////////////////////////////////////////
// Low-level mouse hook.

LRESULT CALLBACK LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode != HC_ACTION) {
        return CallNextHookEx(nullptr, nCode, wParam, lParam);
    }

    // If the Windows 11 volume icon hook is active and the mode is volume icon
    // only, the low-level hook does not need to do anything.
    if (g_settings.clickArea == ClickArea::volumeIcon &&
        g_systemTrayModuleHooked) {
        return CallNextHookEx(nullptr, nCode, wParam, lParam);
    }

    MSLLHOOKSTRUCT* pMouseStruct = (MSLLHOOKSTRUCT*)lParam;

    switch (wParam) {
        case WM_MBUTTONDOWN:
        case WM_NCMBUTTONDOWN:
            if (ShouldToggleAtPoint(pMouseStruct->pt)) {
                ToggleVolMuted();
                g_suppressMiddleUp = true;
                g_lastMiddleToggleTime = GetTickCount();
                return 1;
            }

            g_suppressMiddleUp = false;
            break;

        case WM_MBUTTONUP:
        case WM_NCMBUTTONUP:
            if (g_suppressMiddleUp) {
                g_suppressMiddleUp = false;
                return 1;
            }
            break;

        case WM_MBUTTONDBLCLK:
        case WM_NCMBUTTONDBLCLK:
            if (g_lastMiddleToggleTime != 0 &&
                GetTickCount() - g_lastMiddleToggleTime < 1000) {
                // Suppress the second click of a double click.
                g_suppressMiddleUp = true;
                return 1;
            }
            break;
    }

    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

DWORD WINAPI WorkerThread(LPVOID lpParameter) {
    HANDLE hReadyEvent = (HANDLE)lpParameter;

    HRESULT hrCom = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    bool comInitialized = SUCCEEDED(hrCom);

    HHOOK mouseHook =
        SetWindowsHookEx(WH_MOUSE_LL, LowLevelMouseProc, nullptr, 0);

    SetEvent(hReadyEvent);

    if (!mouseHook) {
        Wh_Log(L"SetWindowsHookEx failed: %u", GetLastError());
    }

    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0) > 0) {
        if (msg.hwnd == nullptr && msg.message == WM_APP) {
            PostQuitMessage(0);
            continue;
        }

        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    if (mouseHook) {
        UnhookWindowsHookEx(mouseHook);
    }

    if (comInitialized) {
        CoUninitialize();
    }

    return 0;
}

void WorkerThreadInit() {
    if (g_workerThread) {
        return;
    }

    HANDLE hReadyEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
    if (!hReadyEvent) {
        return;
    }

    g_workerThread =
        CreateThread(nullptr, 0, WorkerThread, hReadyEvent, 0, nullptr);
    if (g_workerThread) {
        WaitForSingleObject(hReadyEvent, INFINITE);
    }

    CloseHandle(hReadyEvent);
}

void WorkerThreadUninit() {
    if (!g_workerThread) {
        return;
    }

    PostThreadMessage(GetThreadId(g_workerThread), WM_APP, 0, 0);
    WaitForSingleObject(g_workerThread, INFINITE);
    CloseHandle(g_workerThread);
    g_workerThread = nullptr;
}

////////////////////////////////////////////////////////////
// Windows 11 SystemTray volume icon hook.

using VolumeSystemTrayIconDataModel_OnIconClicked_t =
    int(WINAPI*)(void* pThis, void* param1);

VolumeSystemTrayIconDataModel_OnIconClicked_t
    VolumeSystemTrayIconDataModel_OnIconClicked_Original;

int WINAPI VolumeSystemTrayIconDataModel_OnIconClicked_Hook(void* pThis,
                                                            void* param1) {
    if (g_settings.clickArea == ClickArea::volumeIcon &&
        GetKeyState(VK_MBUTTON) < 0) {
        Wh_Log(L"Middle click on Windows 11 volume icon");
        ToggleVolMuted();
        return 0;  // S_OK
    }

    return VolumeSystemTrayIconDataModel_OnIconClicked_Original(pThis, param1);
}

bool HookSystemTraySymbols(HMODULE module) {
    // In SystemTray.dll, the implementation function was inlined into the
    // produce<> COM thunk, so hook that.
    bool isSystemTrayDll = module == GetModuleHandle(L"SystemTray.dll");

    // SystemTray.dll, Taskbar.View.dll
    WindhawkUtils::SYMBOL_HOOK symbolHooks[] = {
        {
            {isSystemTrayDll
                 ? LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::SystemTray::implementation::VolumeSystemTrayIconDataModel,struct winrt::SystemTray::IIconDataModel>::OnIconClicked(void *))"
                 : LR"(public: void __cdecl winrt::SystemTray::implementation::VolumeSystemTrayIconDataModel::OnIconClicked(struct winrt::SystemTray::IconClickedEventArgs const &))"},
            &VolumeSystemTrayIconDataModel_OnIconClicked_Original,
            VolumeSystemTrayIconDataModel_OnIconClicked_Hook,
            true,
        },
    };

    if (!HookSymbols(module, symbolHooks, ARRAYSIZE(symbolHooks))) {
        Wh_Log(L"HookSymbols failed");
        return false;
    }

    return true;
}

HMODULE GetSystemTrayModuleHandle() {
    HMODULE module = GetModuleHandle(L"SystemTray.dll");
    if (!module) {
        module = GetModuleHandle(L"Taskbar.View.dll");
        if (module) {
            // Starting with Taskbar.View.dll 2604.8002.200.6000, the
            // SystemTray types moved out of Taskbar.View.dll into
            // SystemTray.dll, so do not hook Taskbar.View.dll at this version
            // and above.
            VS_FIXEDFILEINFO* fixedFileInfo =
                GetModuleVersionInfo(module, nullptr);

            WORD moduleMajor =
                fixedFileInfo ? HIWORD(fixedFileInfo->dwFileVersionMS) : 0;

            if (!moduleMajor || moduleMajor >= 2604) {
                Wh_Log(L"Skipping Taskbar.View.dll version %d", moduleMajor);
                module = nullptr;
            }
        }
    }

    if (!module) {
        module = GetModuleHandle(L"ExplorerExtensions.dll");
    }

    return module;
}

bool ShouldHookSystemTraySymbols() {
    return g_nExplorerVersion >= WIN_VERSION_11_22H2 &&
           g_settings.clickArea == ClickArea::volumeIcon;
}

void HandleLoadedModuleIfSystemTray(HMODULE module, LPCWSTR lpLibFileName) {
    if (ShouldHookSystemTraySymbols() && !g_systemTrayModuleHooked &&
        GetSystemTrayModuleHandle() == module) {
        if (!g_systemTrayModuleHooked.exchange(true)) {
            Wh_Log(L"Loaded %s", lpLibFileName);

            if (HookSystemTraySymbols(module)) {
                Wh_ApplyHookOperations();
            } else {
                g_systemTrayModuleHooked = false;
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
        HandleLoadedModuleIfSystemTray(module, lpLibFileName);
    }

    return module;
}

////////////////////////////////////////////////////////////
// Settings.

void LoadSettings() {
    PCWSTR clickArea = Wh_GetStringSetting(L"clickArea");

    g_settings.clickArea = ClickArea::taskbarWithoutAppButtons;

    if (wcscmp(clickArea, L"emptyArea") == 0) {
        g_settings.clickArea = ClickArea::emptyArea;
    } else if (wcscmp(clickArea, L"volumeIcon") == 0) {
        g_settings.clickArea = ClickArea::volumeIcon;
    } else if (wcscmp(clickArea, L"taskbarWithoutAppButtons") == 0) {
        g_settings.clickArea = ClickArea::taskbarWithoutAppButtons;
    } else if (wcscmp(clickArea, L"entireTaskbar") == 0) {
        g_settings.clickArea = ClickArea::entireTaskbar;
    }

    Wh_FreeStringSetting(clickArea);

    PCWSTR windowsVersion = Wh_GetStringSetting(L"windowsVersion");

    g_settings.windowsVersion = WindowsVersionOverride::automatic;

    if (wcscmp(windowsVersion, L"windows10") == 0) {
        g_settings.windowsVersion = WindowsVersionOverride::windows10;
    } else if (wcscmp(windowsVersion, L"windows11") == 0) {
        g_settings.windowsVersion = WindowsVersionOverride::windows11;
    }

    Wh_FreeStringSetting(windowsVersion);
}

void ApplyWindowsVersionOverride() {
    switch (g_settings.windowsVersion) {
        case WindowsVersionOverride::windows10:
            g_nExplorerVersion = WIN_VERSION_10_20H1;
            break;

        case WindowsVersionOverride::windows11:
            g_nExplorerVersion = WIN_VERSION_11_22H2;
            break;

        default:
            g_nExplorerVersion = g_nWinVersion;
            break;
    }
}

////////////////////////////////////////////////////////////
// Windhawk mod entry points.

BOOL Wh_ModInit() {
    Wh_Log(L">");

    LoadSettings();

    if (!WindowsVersionInit()) {
        Wh_Log(L"Unsupported Windows version");
        return FALSE;
    }

    ApplyWindowsVersionOverride();

    if (ShouldHookSystemTraySymbols()) {
        if (HMODULE systemTrayModule = GetSystemTrayModuleHandle()) {
            if (!g_systemTrayModuleHooked.exchange(true)) {
                if (!HookSystemTraySymbols(systemTrayModule)) {
                    g_systemTrayModuleHooked = false;
                }
            }
        } else {
            Wh_Log(L"System tray module not loaded yet");
        }

        HMODULE kernelBaseModule = GetModuleHandle(L"kernelbase.dll");
        auto pLoadLibraryExW = (decltype(&LoadLibraryExW))GetProcAddress(
            kernelBaseModule, "LoadLibraryExW");

        WindhawkUtils::SetFunctionHook(pLoadLibraryExW, LoadLibraryExW_Hook,
                                       &LoadLibraryExW_Original);
    }

    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L">");

    if (ShouldHookSystemTraySymbols() && !g_systemTrayModuleHooked) {
        if (HMODULE systemTrayModule = GetSystemTrayModuleHandle()) {
            if (!g_systemTrayModuleHooked.exchange(true)) {
                if (HookSystemTraySymbols(systemTrayModule)) {
                    Wh_ApplyHookOperations();
                } else {
                    g_systemTrayModuleHooked = false;
                }
            }
        }
    }

    WorkerThreadInit();
}

void Wh_ModUninit() {
    Wh_Log(L">");

    WorkerThreadUninit();
}

BOOL Wh_ModSettingsChanged(BOOL* bReload) {
    Wh_Log(L">");

    // The simplest and safest behavior is to reload the mod when settings
    // change, because hook requirements depend on the selected click area and
    // Windows version override.
    *bReload = TRUE;

    return TRUE;
}
