// ==WindhawkMod==
// @id              taskbar-background-helper
// @name            Taskbar Background Helper
// @description     Sets the taskbar background for the transparent parts, always or only when there's a maximized window, designed to be used with Windows 11 Taskbar Styler
// @version         1.0
// @author          m417z
// @github          https://github.com/m417z
// @twitter         https://twitter.com/m417z
// @homepage        https://m417z.com/
// @include         explorer.exe
// @architecture    x86-64
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
# Taskbar Background Helper

Sets the taskbar background for the transparent parts, always or only when
there's a maximized window, designed to be used with [Windows 11 Taskbar
Styler](https://windhawk.net/mods/windows-11-taskbar-styler).

Also, Windows 11 Taskbar Styler has [a known
limitation](https://github.com/ramensoftware/windhawk-mods/issues/742) which
makes some styles only work if there's a single monitor. This mod can be used as
a workaround.

![Demonstration](https://i.imgur.com/lMp8OLp.gif)
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- backgroundStyle: blur
  $name: Background style
  $options:
  - blur: Blur
  - acrylicBlur: Acrylic blur
  - color: Color
- color:
  - red: 255
  - green: 127
  - blue: 39
  - transparency: 128
  $name: Custom color
  $description: Values are between 0 and 255
- onlyWhenMaximized: true
  $name: Only when maximized
  $description: >-
    Only apply the style when there's a maximized window on the monitor
*/
// ==/WindhawkModSettings==

#include <unordered_set>

enum class BackgroundStyle {
    blur,
    acrylicBlur,
    color,
};

struct {
    BackgroundStyle backgroundStyle;
    COLORREF color;
    bool onlyWhenMaximized;
} g_settings;

HANDLE g_winObjectLocationChangeThread;
std::unordered_set<HMONITOR> g_pendingMonitors;
UINT_PTR g_pendingMonitorsTimer;

enum WINDOWCOMPOSITIONATTRIB {
    WCA_UNDEFINED = 0,
    WCA_NCRENDERING_ENABLED = 1,
    WCA_NCRENDERING_POLICY = 2,
    WCA_TRANSITIONS_FORCEDISABLED = 3,
    WCA_ALLOW_NCPAINT = 4,
    WCA_CAPTION_BUTTON_BOUNDS = 5,
    WCA_NONCLIENT_RTL_LAYOUT = 6,
    WCA_FORCE_ICONIC_REPRESENTATION = 7,
    WCA_EXTENDED_FRAME_BOUNDS = 8,
    WCA_HAS_ICONIC_BITMAP = 9,
    WCA_THEME_ATTRIBUTES = 10,
    WCA_NCRENDERING_EXILED = 11,
    WCA_NCADORNMENTINFO = 12,
    WCA_EXCLUDED_FROM_LIVEPREVIEW = 13,
    WCA_VIDEO_OVERLAY_ACTIVE = 14,
    WCA_FORCE_ACTIVEWINDOW_APPEARANCE = 15,
    WCA_DISALLOW_PEEK = 16,
    WCA_CLOAK = 17,
    WCA_CLOAKED = 18,
    WCA_ACCENT_POLICY = 19,
    WCA_FREEZE_REPRESENTATION = 20,
    WCA_EVER_UNCLOAKED = 21,
    WCA_VISUAL_OWNER = 22,
    WCA_HOLOGRAPHIC = 23,
    WCA_EXCLUDED_FROM_DDA = 24,
    WCA_PASSIVEUPDATEMODE = 25,
    WCA_USEDARKMODECOLORS = 26,
    WCA_CORNER_STYLE = 27,
    WCA_PART_COLOR = 28,
    WCA_DISABLE_MOVESIZE_FEEDBACK = 29,
    WCA_LAST = 30
};

// Affects the rendering of the background of a window.
enum ACCENT_STATE {
    // Default value. Background is black.
    ACCENT_DISABLED = 0,
    // Background is GradientColor, alpha channel ignored.
    ACCENT_ENABLE_GRADIENT = 1,
    // Background is GradientColor.
    ACCENT_ENABLE_TRANSPARENTGRADIENT = 2,
    // Background is GradientColor, with blur effect.
    ACCENT_ENABLE_BLURBEHIND = 3,
    // Background is GradientColor, with acrylic blur effect.
    ACCENT_ENABLE_ACRYLICBLURBEHIND = 4,
    // Allows desktop apps to use Compositor.CreateHostBackdropBrush
    ACCENT_ENABLE_HOSTBACKDROP = 5,
    // Unknown. Seems to draw background fully transparent.
    ACCENT_INVALID_STATE = 6,
};

struct ACCENTPOLICY {
    ACCENT_STATE accentState;
    UINT accentFlags;
    COLORREF gradientColor;
    LONG animationId;
};

struct WINDOWCOMPOSITIONATTRIBDATA {
    WINDOWCOMPOSITIONATTRIB attrib;
    void* pvData;
    UINT cbData;
};

using SetWindowCompositionAttribute_t =
    BOOL(WINAPI*)(HWND hWnd, const WINDOWCOMPOSITIONATTRIBDATA* pAttrData);
SetWindowCompositionAttribute_t SetWindowCompositionAttribute_Original;

BOOL SetTaskbarStyle(HWND hWnd) {
    ACCENT_STATE accentState;
    switch (g_settings.backgroundStyle) {
        case BackgroundStyle::blur:
            accentState = ACCENT_ENABLE_BLURBEHIND;
            break;

        case BackgroundStyle::acrylicBlur:
            accentState = ACCENT_ENABLE_ACRYLICBLURBEHIND;
            break;

        case BackgroundStyle::color:
            accentState = ACCENT_ENABLE_TRANSPARENTGRADIENT;
            break;
    }

    ACCENTPOLICY policy = {accentState, 0, g_settings.color, 0};

    WINDOWCOMPOSITIONATTRIBDATA data = {WCA_ACCENT_POLICY, &policy,
                                        sizeof(policy)};
    return SetWindowCompositionAttribute_Original(hWnd, &data);
}

BOOL ResetTaskbarStyle(HWND hWnd) {
    // TrayUI::_OnThemeChanged
    // TrayUI::OnShellModeChanged
    ACCENTPOLICY policy = {ACCENT_ENABLE_TRANSPARENTGRADIENT, 0x13, 0, 0};
    WINDOWCOMPOSITIONATTRIBDATA data = {WCA_ACCENT_POLICY, &policy,
                                        sizeof(policy)};
    return SetWindowCompositionAttribute_Original(hWnd, &data);
}

HWND GetTaskbarForMonitor(HMONITOR monitor) {
    HWND hTaskbarWnd = FindWindow(L"Shell_TrayWnd", nullptr);
    if (!hTaskbarWnd) {
        return nullptr;
    }

    DWORD taskbarThreadId = 0;
    DWORD taskbarProcessId = 0;
    if (!(taskbarThreadId =
              GetWindowThreadProcessId(hTaskbarWnd, &taskbarProcessId)) ||
        taskbarProcessId != GetCurrentProcessId()) {
        return nullptr;
    }

    if (MonitorFromWindow(hTaskbarWnd, MONITOR_DEFAULTTONEAREST) == monitor) {
        return hTaskbarWnd;
    }

    HWND hResultWnd = nullptr;

    auto enumWindowsProc = [monitor, &hResultWnd](HWND hWnd) -> BOOL {
        WCHAR szClassName[32];
        if (GetClassName(hWnd, szClassName, ARRAYSIZE(szClassName)) == 0) {
            return TRUE;
        }

        if (_wcsicmp(szClassName, L"Shell_SecondaryTrayWnd") != 0) {
            return TRUE;
        }

        if (MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST) != monitor) {
            return TRUE;
        }

        hResultWnd = hWnd;
        return FALSE;
    };

    EnumThreadWindows(
        taskbarThreadId,
        [](HWND hWnd, LPARAM lParam) WINAPI -> BOOL {
            auto& proc = *reinterpret_cast<decltype(enumWindowsProc)*>(lParam);
            return proc(hWnd);
        },
        reinterpret_cast<LPARAM>(&enumWindowsProc));

    return hResultWnd;
}

bool DoesMonitorHaveMaximizedWindow(HMONITOR monitor) {
    bool hasMaximized = false;

    auto enumWindowsProc = [monitor, &hasMaximized](HWND hWnd) -> BOOL {
        if (MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST) != monitor) {
            return TRUE;
        }

        WINDOWPLACEMENT wp{
            .length = sizeof(WINDOWPLACEMENT),
        };
        if (!GetWindowPlacement(hWnd, &wp) || wp.showCmd != SW_SHOWMAXIMIZED) {
            return TRUE;
        }

        hasMaximized = true;
        return FALSE;
    };

    EnumWindows(
        [](HWND hWnd, LPARAM lParam) WINAPI -> BOOL {
            auto& proc = *reinterpret_cast<decltype(enumWindowsProc)*>(lParam);
            return proc(hWnd);
        },
        reinterpret_cast<LPARAM>(&enumWindowsProc));

    return hasMaximized;
}

void CALLBACK LocationChangeWinEventProc(HWINEVENTHOOK hWinEventHook,
                                         DWORD event,
                                         HWND hWnd,
                                         LONG idObject,
                                         LONG idChild,
                                         DWORD dwEventThread,
                                         DWORD dwmsEventTime) {
    if (idObject != OBJID_WINDOW) {
        return;
    }

    Wh_Log(L">");

    HMONITOR monitor = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
    g_pendingMonitors.insert(monitor);

    if (g_pendingMonitorsTimer) {
        return;
    }

    g_pendingMonitorsTimer =
        SetTimer(nullptr, 0, 200,
                 [](HWND hwnd,         // handle of window for timer messages
                    UINT uMsg,         // WM_TIMER message
                    UINT_PTR idEvent,  // timer identifier
                    DWORD dwTime       // current system time
                    ) WINAPI {
                     Wh_Log(L">");

                     KillTimer(nullptr, g_pendingMonitorsTimer);
                     g_pendingMonitorsTimer = 0;

                     for (HMONITOR monitor : g_pendingMonitors) {
                         HWND hMMTaskbarWnd = GetTaskbarForMonitor(monitor);
                         if (DoesMonitorHaveMaximizedWindow(monitor)) {
                             SetTaskbarStyle(hMMTaskbarWnd);
                         } else {
                             ResetTaskbarStyle(hMMTaskbarWnd);
                         }
                     }

                     g_pendingMonitors.clear();
                 });
}

BOOL AdjustTaskbarStyle(HWND hWnd) {
    if (g_settings.onlyWhenMaximized) {
        HMONITOR monitor = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
        if (!DoesMonitorHaveMaximizedWindow(monitor)) {
            return ResetTaskbarStyle(hWnd);
        }
    }

    return SetTaskbarStyle(hWnd);
}

BOOL WINAPI SetWindowCompositionAttribute_Hook(
    HWND hWnd,
    const WINDOWCOMPOSITIONATTRIBDATA* pAttrData) {
    auto original = [=]() {
        return SetWindowCompositionAttribute_Original(hWnd, pAttrData);
    };

    if (pAttrData->attrib != WCA_ACCENT_POLICY) {
        return original();
    }

    DWORD dwProcessId = 0;
    if (!GetWindowThreadProcessId(hWnd, &dwProcessId) ||
        dwProcessId != GetCurrentProcessId()) {
        return original();
    }

    WCHAR szClassName[32];
    if (GetClassName(hWnd, szClassName, ARRAYSIZE(szClassName)) == 0) {
        return original();
    }

    if (_wcsicmp(szClassName, L"Shell_TrayWnd") != 0 &&
        _wcsicmp(szClassName, L"Shell_SecondaryTrayWnd") != 0) {
        return original();
    }

    return AdjustTaskbarStyle(hWnd);
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

void AdjustAllTaskbarStyles() {
    std::unordered_set<HWND> secondaryTaskbarWindows;
    HWND hWnd = FindCurrentProcessTaskbarWindows(&secondaryTaskbarWindows);
    if (hWnd) {
        AdjustTaskbarStyle(hWnd);
    }

    for (HWND hSecondaryWnd : secondaryTaskbarWindows) {
        AdjustTaskbarStyle(hSecondaryWnd);
    }
}

void LoadSettings() {
    PCWSTR backgroundStyle = Wh_GetStringSetting(L"backgroundStyle");
    g_settings.backgroundStyle = BackgroundStyle::blur;
    if (wcscmp(backgroundStyle, L"acrylicBlur") == 0) {
        g_settings.backgroundStyle = BackgroundStyle::acrylicBlur;
    } else if (wcscmp(backgroundStyle, L"color") == 0) {
        g_settings.backgroundStyle = BackgroundStyle::color;
    }
    Wh_FreeStringSetting(backgroundStyle);

    int red = Wh_GetIntSetting(L"color.red");
    int green = Wh_GetIntSetting(L"color.green");
    int blue = Wh_GetIntSetting(L"color.blue");
    int transparency = Wh_GetIntSetting(L"color.transparency");

    g_settings.color = (COLORREF)((BYTE)red | ((WORD)((BYTE)green) << 8) |
                                  (((DWORD)(BYTE)blue) << 16) |
                                  (((DWORD)(BYTE)transparency) << 24));

    g_settings.onlyWhenMaximized = Wh_GetIntSetting(L"onlyWhenMaximized");
}

BOOL Wh_ModInit() {
    Wh_Log(L">");

    LoadSettings();

    HMODULE hUser32 = LoadLibrary(L"user32.dll");
    if (!hUser32) {
        Wh_Log(L"Error loading user32.dll");
        return FALSE;
    }

    SetWindowCompositionAttribute_t pSetWindowCompositionAttribute =
        (SetWindowCompositionAttribute_t)GetProcAddress(
            hUser32, "SetWindowCompositionAttribute");
    if (!pSetWindowCompositionAttribute) {
        Wh_Log(L"Error getting SetWindowCompositionAttribute");
        return FALSE;
    }

    Wh_SetFunctionHook((void*)pSetWindowCompositionAttribute,
                       (void*)SetWindowCompositionAttribute_Hook,
                       (void**)&SetWindowCompositionAttribute_Original);

    return TRUE;
}

void Wh_ModAfterInit() {
    WNDCLASS wndclass;
    if (GetClassInfo(GetModuleHandle(NULL), L"Shell_TrayWnd", &wndclass)) {
        AdjustAllTaskbarStyles();
    }

    if (g_settings.onlyWhenMaximized) {
        g_winObjectLocationChangeThread = CreateThread(
            nullptr, 0,
            [](LPVOID lpParameter) WINAPI -> DWORD {
                HWINEVENTHOOK winObjectLocationChangeEventHook =
                    SetWinEventHook(EVENT_OBJECT_LOCATIONCHANGE,
                                    EVENT_OBJECT_LOCATIONCHANGE, nullptr,
                                    LocationChangeWinEventProc, 0, 0,
                                    WINEVENT_OUTOFCONTEXT);
                if (!winObjectLocationChangeEventHook) {
                    Wh_Log(L"Error: SetWinEventHook");
                    return 0;
                }

                BOOL bRet;
                MSG msg;
                while ((bRet = GetMessage(&msg, NULL, 0, 0)) != 0) {
                    if (bRet == -1) {
                        msg.wParam = 0;
                        break;
                    }

                    if (msg.hwnd == NULL && msg.message == WM_APP) {
                        PostQuitMessage(0);
                        continue;
                    }

                    TranslateMessage(&msg);
                    DispatchMessage(&msg);
                }

                UnhookWinEvent(winObjectLocationChangeEventHook);
                return 0;
            },
            nullptr, 0, nullptr);
    }
}

void Wh_ModUninit() {
    Wh_Log(L">");

    if (g_winObjectLocationChangeThread) {
        PostThreadMessage(GetThreadId(g_winObjectLocationChangeThread), WM_APP,
                          0, 0);
        WaitForSingleObject(g_winObjectLocationChangeThread, INFINITE);
        CloseHandle(g_winObjectLocationChangeThread);
    }

    std::unordered_set<HWND> secondaryTaskbarWindows;
    HWND hWnd = FindCurrentProcessTaskbarWindows(&secondaryTaskbarWindows);
    if (hWnd) {
        ResetTaskbarStyle(hWnd);
    }

    for (HWND hSecondaryWnd : secondaryTaskbarWindows) {
        ResetTaskbarStyle(hSecondaryWnd);
    }
}

BOOL Wh_ModSettingsChanged(BOOL* bReload) {
    Wh_Log(L">");

    bool prevOnlyWhenMaximized = g_settings.onlyWhenMaximized;

    LoadSettings();

    *bReload = g_settings.onlyWhenMaximized != prevOnlyWhenMaximized;

    if (!*bReload) {
        AdjustAllTaskbarStyles();
    }

    return TRUE;
}
