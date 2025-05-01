// ==WindhawkMod==
// @id              taskbar-background-helper
// @name            Taskbar Background Helper
// @description     Sets the taskbar background for the transparent parts, always or only when there's a maximized window, designed to be used with Windows 11 Taskbar Styler
// @version         1.1
// @author          m417z
// @github          https://github.com/m417z
// @twitter         https://twitter.com/m417z
// @homepage        https://m417z.com/
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -ldwmapi -lole32 -loleaut32 -lruntimeobject
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
makes some styles only work if there's a single taskbar. This mod can be used as
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
    $name: Red
  - green: 127
    $name: Green
  - blue: 39
    $name: Blue
  - accentColor: false
    $name: Current theme accent color
    $description: If enabled, the color values above are ignored
  - transparency: 128
    $name: Transparency
  $name: Custom color
  $description: Values are between 0 and 255
- onlyWhenMaximized: true
  $name: Only when maximized
  $description: >-
    Only apply the style when there's a maximized window on the monitor
- excludedPrograms: [""]
  $name: Excluded programs
  $description: >-
    If the "Only when maximized" option is enabled, these programs will be
    ignored when maximized.

    Entries can be process names, paths or application IDs, for example:

    mspaint.exe

    C:\Windows\System32\notepad.exe

    Microsoft.WindowsCalculator_8wekyb3d8bbwe!App
- styleForDarkMode:
  - use: false
    $name: Use a separate background for dark mode
  - backgroundStyle: blur
    $name: Background style
    $options:
    - blur: Blur
    - acrylicBlur: Acrylic blur
    - color: Color
  - color:
    - red: 255
      $name: Red
    - green: 127
      $name: Green
    - blue: 39
      $name: Blue
    - accentColor: false
      $name: Current theme accent color
      $description: If enabled, the color values above are ignored
    - transparency: 128
      $name: Transparency
    $name: Custom color
    $description: Values are between 0 and 255
  $name: Dark mode
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <dwmapi.h>

#include <winrt/Windows.UI.ViewManagement.h>

#include <atomic>
#include <mutex>
#include <optional>
#include <string>
#include <unordered_set>

enum class BackgroundStyle {
    blur,
    acrylicBlur,
    color,
};

struct TaskbarStyle {
    BackgroundStyle backgroundStyle;
    COLORREF color;
    bool accentColor;
};

struct {
    TaskbarStyle style;
    bool onlyWhenMaximized;
    std::unordered_set<std::wstring> excludedPrograms;
    std::optional<TaskbarStyle> darkModeStyle;
} g_settings;

std::mutex g_winEventHookThreadMutex;
std::atomic<HANDLE> g_winEventHookThread;
std::unordered_set<HMONITOR> g_pendingMonitors;
UINT_PTR g_pendingMonitorsTimer;

#if __cplusplus < 202302L
// Missing in older MinGW headers.
DECLARE_HANDLE(CO_MTA_USAGE_COOKIE);
WINOLEAPI CoIncrementMTAUsage(CO_MTA_USAGE_COOKIE* pCookie);
WINOLEAPI CoDecrementMTAUsage(CO_MTA_USAGE_COOKIE Cookie);
#endif

// Missing in older MinGW headers.
#ifndef EVENT_OBJECT_CLOAKED
#define EVENT_OBJECT_CLOAKED 0x8017
#endif
#ifndef EVENT_OBJECT_UNCLOAKED
#define EVENT_OBJECT_UNCLOAKED 0x8018
#endif

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
    WCA_SYSTEMBACKDROP_TYPE = 30,
    WCA_SET_TAGGED_WINDOW_RECT = 31,
    WCA_CLEAR_TAGGED_WINDOW_RECT = 32,
    WCA_REMOTEAPP_POLICY = 33,
    WCA_HAS_ACCENT_POLICY = 34,
    WCA_REDIRECTIONBITMAP_FILL_COLOR = 35,
    WCA_REDIRECTIONBITMAP_ALPHA = 36,
    WCA_BORDER_MARGINS = 37,
    WCA_LAST = 38,
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

// https://stackoverflow.com/a/51336913
bool IsWindowsDarkModeEnabled() {
    constexpr WCHAR kSubKeyPath[] =
        L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize";

    DWORD value = 0;
    DWORD valueSize = sizeof(value);
    LONG result =
        RegGetValue(HKEY_CURRENT_USER, kSubKeyPath, L"AppsUseLightTheme",
                    RRF_RT_REG_DWORD, nullptr, &value, &valueSize);
    if (result != ERROR_SUCCESS) {
        return false;
    }

    return value == 0;
}

// https://devblogs.microsoft.com/oldnewthing/20200302-00/?p=103507
bool IsWindowCloaked(HWND hwnd) {
    BOOL isCloaked = FALSE;
    return SUCCEEDED(DwmGetWindowAttribute(hwnd, DWMWA_CLOAKED, &isCloaked,
                                           sizeof(isCloaked))) &&
           isCloaked;
}

BOOL SetTaskbarStyle(HWND hWnd) {
    TaskbarStyle& style =
        (g_settings.darkModeStyle && IsWindowsDarkModeEnabled())
            ? *g_settings.darkModeStyle
            : g_settings.style;

    ACCENT_STATE accentState;
    UINT accentFlags = 0;
    switch (style.backgroundStyle) {
        case BackgroundStyle::blur:
            accentState = ACCENT_ENABLE_BLURBEHIND;
            break;

        case BackgroundStyle::acrylicBlur:
            accentState = ACCENT_ENABLE_ACRYLICBLURBEHIND;
            break;

        case BackgroundStyle::color:
            accentState = ACCENT_ENABLE_TRANSPARENTGRADIENT;
            accentFlags = 0x13;
            break;
    }

    COLORREF color = style.color;
    if (style.accentColor) {
        try {
            const winrt::Windows::UI::ViewManagement::UISettings uiSettings;
            const auto accentColor{uiSettings.GetColorValue(
                winrt::Windows::UI::ViewManagement::UIColorType::Accent)};

            color = (COLORREF)((BYTE)accentColor.R |
                               ((WORD)((BYTE)accentColor.G) << 8) |
                               (((DWORD)(BYTE)accentColor.B) << 16) |
                               (color & 0xFF000000));
        } catch (winrt::hresult_error const& ex) {
            Wh_Log(L"Error %08X: %s", ex.code(), ex.message().c_str());
        }
    }

    ACCENTPOLICY policy = {accentState, accentFlags, color, 0};

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

bool IsTaskbarWindow(HWND hWnd) {
    WCHAR szClassName[32];
    if (!GetClassName(hWnd, szClassName, ARRAYSIZE(szClassName))) {
        return false;
    }

    return _wcsicmp(szClassName, L"Shell_TrayWnd") == 0 ||
           _wcsicmp(szClassName, L"Shell_SecondaryTrayWnd") == 0;
}

HWND FindTaskbarWindows(std::unordered_set<HWND>* secondaryTaskbarWindows) {
    secondaryTaskbarWindows->clear();

    HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();
    if (!hTaskbarWnd) {
        return nullptr;
    }

    DWORD taskbarThreadId = GetWindowThreadProcessId(hTaskbarWnd, nullptr);
    if (!taskbarThreadId) {
        return nullptr;
    }

    auto enumWindowsProc = [&secondaryTaskbarWindows](HWND hWnd) -> BOOL {
        WCHAR szClassName[32];
        if (GetClassName(hWnd, szClassName, ARRAYSIZE(szClassName)) == 0) {
            return TRUE;
        }

        if (_wcsicmp(szClassName, L"Shell_SecondaryTrayWnd") == 0) {
            secondaryTaskbarWindows->insert(hWnd);
        }

        return TRUE;
    };

    EnumThreadWindows(
        taskbarThreadId,
        [](HWND hWnd, LPARAM lParam) -> BOOL {
            auto& proc = *reinterpret_cast<decltype(enumWindowsProc)*>(lParam);
            return proc(hWnd);
        },
        reinterpret_cast<LPARAM>(&enumWindowsProc));

    return hTaskbarWnd;
}

// https://gist.github.com/m417z/451dfc2dad88d7ba88ed1814779a26b4
std::wstring GetWindowAppId(HWND hWnd) {
    // {c8900b66-a973-584b-8cae-355b7f55341b}
    constexpr winrt::guid CLSID_StartMenuCacheAndAppResolver{
        0x660b90c8,
        0x73a9,
        0x4b58,
        {0x8c, 0xae, 0x35, 0x5b, 0x7f, 0x55, 0x34, 0x1b}};

    // {de25675a-72de-44b4-9373-05170450c140}
    constexpr winrt::guid IID_IAppResolver_8{
        0xde25675a,
        0x72de,
        0x44b4,
        {0x93, 0x73, 0x05, 0x17, 0x04, 0x50, 0xc1, 0x40}};

    struct IAppResolver_8 : public IUnknown {
       public:
        virtual HRESULT STDMETHODCALLTYPE GetAppIDForShortcut() = 0;
        virtual HRESULT STDMETHODCALLTYPE GetAppIDForShortcutObject() = 0;
        virtual HRESULT STDMETHODCALLTYPE
        GetAppIDForWindow(HWND hWnd,
                          WCHAR** pszAppId,
                          void* pUnknown1,
                          void* pUnknown2,
                          void* pUnknown3) = 0;
        virtual HRESULT STDMETHODCALLTYPE
        GetAppIDForProcess(DWORD dwProcessId,
                           WCHAR** pszAppId,
                           void* pUnknown1,
                           void* pUnknown2,
                           void* pUnknown3) = 0;
    };

    HRESULT hr;
    std::wstring result;

    CO_MTA_USAGE_COOKIE cookie;
    bool mtaUsageIncreased = SUCCEEDED(CoIncrementMTAUsage(&cookie));

    winrt::com_ptr<IAppResolver_8> appResolver;
    hr = CoCreateInstance(CLSID_StartMenuCacheAndAppResolver, nullptr,
                          CLSCTX_INPROC_SERVER | CLSCTX_INPROC_HANDLER,
                          IID_IAppResolver_8, appResolver.put_void());
    if (SUCCEEDED(hr)) {
        WCHAR* pszAppId;
        hr = appResolver->GetAppIDForWindow(hWnd, &pszAppId, nullptr, nullptr,
                                            nullptr);
        if (SUCCEEDED(hr)) {
            result = pszAppId;
            CoTaskMemFree(pszAppId);
        }
    }

    appResolver = nullptr;

    if (mtaUsageIncreased) {
        CoDecrementMTAUsage(cookie);
    }

    return result;
}

bool IsWindowExcluded(HWND hWnd) {
    if (g_settings.excludedPrograms.empty()) {
        return false;
    }

    DWORD resolvedWindowProcessPathLen = 0;
    WCHAR resolvedWindowProcessPath[MAX_PATH];
    WCHAR resolvedWindowProcessPathUpper[MAX_PATH];

    DWORD dwProcessId = 0;
    if (GetWindowThreadProcessId(hWnd, &dwProcessId)) {
        HANDLE hProcess =
            OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, dwProcessId);
        if (hProcess) {
            DWORD dwSize = ARRAYSIZE(resolvedWindowProcessPath);
            if (QueryFullProcessImageName(hProcess, 0,
                                          resolvedWindowProcessPath, &dwSize)) {
                resolvedWindowProcessPathLen = dwSize;
            }

            CloseHandle(hProcess);
        }
    }

    if (resolvedWindowProcessPathLen > 0) {
        LCMapStringEx(LOCALE_NAME_USER_DEFAULT, LCMAP_UPPERCASE,
                      resolvedWindowProcessPath,
                      resolvedWindowProcessPathLen + 1,
                      resolvedWindowProcessPathUpper,
                      resolvedWindowProcessPathLen + 1, nullptr, nullptr, 0);
    } else {
        *resolvedWindowProcessPath = L'\0';
        *resolvedWindowProcessPathUpper = L'\0';
    }

    if (resolvedWindowProcessPathLen > 0 &&
        g_settings.excludedPrograms.contains(resolvedWindowProcessPathUpper)) {
        return true;
    }

    if (PCWSTR programFileNameUpper =
            wcsrchr(resolvedWindowProcessPathUpper, L'\\')) {
        programFileNameUpper++;
        if (*programFileNameUpper &&
            g_settings.excludedPrograms.contains(programFileNameUpper)) {
            return true;
        }
    }

    std::wstring appId = GetWindowAppId(hWnd);
    LCMapStringEx(LOCALE_NAME_USER_DEFAULT, LCMAP_UPPERCASE, appId.data(),
                  appId.length(), appId.data(), appId.length(), nullptr,
                  nullptr, 0);
    if (g_settings.excludedPrograms.contains(appId.c_str())) {
        return true;
    }

    return false;
}

bool DoesMonitorHaveMaximizedWindow(HMONITOR monitor, HWND hMMTaskbarWnd) {
    bool hasMaximizedWindow = false;

    MONITORINFO monitorInfo{
        .cbSize = sizeof(monitorInfo),
    };
    GetMonitorInfo(monitor, &monitorInfo);

    HWND hShellWindow = GetShellWindow();

    DWORD dwTaskbarThreadId = GetWindowThreadProcessId(hMMTaskbarWnd, nullptr);

    auto enumWindowsProc = [&](HWND hWnd) -> BOOL {
        if (GetWindowThreadProcessId(hWnd, nullptr) == dwTaskbarThreadId) {
            return TRUE;
        }

        if (MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST) != monitor) {
            return TRUE;
        }

        if (!IsWindowVisible(hWnd) || IsWindowCloaked(hWnd) || IsIconic(hWnd) ||
            (GetWindowLong(hWnd, GWL_EXSTYLE) & WS_EX_NOACTIVATE)) {
            return TRUE;
        }

        if (hWnd == hShellWindow || GetProp(hWnd, L"DesktopWindow")) {
            return TRUE;
        }

        // Check this after the other checks, as it's the most expensive one.
        if (IsWindowExcluded(hWnd)) {
            return TRUE;
        }

        WINDOWPLACEMENT wp{
            .length = sizeof(WINDOWPLACEMENT),
        };
        if (GetWindowPlacement(hWnd, &wp) && wp.showCmd == SW_SHOWMAXIMIZED) {
            hasMaximizedWindow = true;
            return FALSE;
        }

        RECT windowRect{};
        DwmGetWindowAttribute(hWnd, DWMWA_EXTENDED_FRAME_BOUNDS, &windowRect,
                              sizeof(windowRect));

        if (EqualRect(&windowRect, &monitorInfo.rcMonitor)) {
            // Spans across the whole monitor, e.g. Win+Tab view.
            hasMaximizedWindow = true;
            return FALSE;
        }

        return TRUE;
    };

    EnumWindows(
        [](HWND hWnd, LPARAM lParam) -> BOOL {
            auto& proc = *reinterpret_cast<decltype(enumWindowsProc)*>(lParam);
            return proc(hWnd);
        },
        reinterpret_cast<LPARAM>(&enumWindowsProc));

    return hasMaximizedWindow;
}

void CALLBACK WinEventProc(HWINEVENTHOOK hWinEventHook,
                           DWORD event,
                           HWND hWnd,
                           LONG idObject,
                           LONG idChild,
                           DWORD dwEventThread,
                           DWORD dwmsEventTime) {
    if (idObject != OBJID_WINDOW ||
        (GetWindowLong(hWnd, GWL_STYLE) & WS_CHILD) || IsTaskbarWindow(hWnd)) {
        return;
    }

    HWND hParentWnd = GetAncestor(hWnd, GA_PARENT);
    if (hParentWnd && hParentWnd != GetDesktopWindow()) {
        return;
    }

    Wh_Log(L"> %08X", (DWORD)(ULONG_PTR)hWnd);

    HMONITOR monitor = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
    g_pendingMonitors.insert(monitor);

    if (g_pendingMonitorsTimer) {
        return;
    }

    g_pendingMonitorsTimer = SetTimer(
        nullptr, 0, 200,
        [](HWND hwnd,         // handle of window for timer messages
           UINT uMsg,         // WM_TIMER message
           UINT_PTR idEvent,  // timer identifier
           DWORD dwTime       // current system time
        ) {
            Wh_Log(L">");

            KillTimer(nullptr, g_pendingMonitorsTimer);
            g_pendingMonitorsTimer = 0;

            std::unordered_set<HWND> secondaryTaskbarWindows;
            HWND hTaskbarWnd = FindTaskbarWindows(&secondaryTaskbarWindows);

            for (HMONITOR monitor : g_pendingMonitors) {
                HWND hMMTaskbarWnd = nullptr;

                if (hTaskbarWnd &&
                    MonitorFromWindow(hTaskbarWnd, MONITOR_DEFAULTTONEAREST) ==
                        monitor) {
                    hMMTaskbarWnd = hTaskbarWnd;
                } else {
                    for (HWND hSecondaryTaskbarWnd : secondaryTaskbarWindows) {
                        if (MonitorFromWindow(hSecondaryTaskbarWnd,
                                              MONITOR_DEFAULTTONEAREST) ==
                            monitor) {
                            hMMTaskbarWnd = hSecondaryTaskbarWnd;
                            break;
                        }
                    }
                }

                if (!hMMTaskbarWnd) {
                    continue;
                }

                if (DoesMonitorHaveMaximizedWindow(monitor, hMMTaskbarWnd)) {
                    SetTaskbarStyle(hMMTaskbarWnd);
                } else {
                    ResetTaskbarStyle(hMMTaskbarWnd);
                }
            }

            g_pendingMonitors.clear();
        });
}

DWORD WINAPI WinEventHookThread(LPVOID lpThreadParameter) {
    HWINEVENTHOOK winObjectEventHook1 =
        SetWinEventHook(EVENT_OBJECT_CREATE, EVENT_OBJECT_HIDE, nullptr,
                        WinEventProc, 0, 0, WINEVENT_OUTOFCONTEXT);
    if (!winObjectEventHook1) {
        Wh_Log(L"Error: SetWinEventHook");
    }

    HWINEVENTHOOK winObjectEventHook2 = SetWinEventHook(
        EVENT_OBJECT_LOCATIONCHANGE, EVENT_OBJECT_LOCATIONCHANGE, nullptr,
        WinEventProc, 0, 0, WINEVENT_OUTOFCONTEXT);
    if (!winObjectEventHook2) {
        Wh_Log(L"Error: SetWinEventHook");
    }

    HWINEVENTHOOK winObjectEventHook3 =
        SetWinEventHook(EVENT_OBJECT_CLOAKED, EVENT_OBJECT_UNCLOAKED, nullptr,
                        WinEventProc, 0, 0, WINEVENT_OUTOFCONTEXT);
    if (!winObjectEventHook3) {
        Wh_Log(L"Error: SetWinEventHook");
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

    if (winObjectEventHook1) {
        UnhookWinEvent(winObjectEventHook1);
    }

    if (winObjectEventHook2) {
        UnhookWinEvent(winObjectEventHook2);
    }

    if (winObjectEventHook3) {
        UnhookWinEvent(winObjectEventHook3);
    }

    return 0;
}

BOOL AdjustTaskbarStyle(HWND hWnd) {
    if (g_settings.onlyWhenMaximized) {
        if (!g_winEventHookThread) {
            std::lock_guard<std::mutex> guard(g_winEventHookThreadMutex);

            if (!g_winEventHookThread) {
                g_winEventHookThread = CreateThread(
                    nullptr, 0, WinEventHookThread, nullptr, 0, nullptr);
            }
        }

        HMONITOR monitor = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
        if (!DoesMonitorHaveMaximizedWindow(monitor, hWnd)) {
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

void AdjustAllTaskbarStyles() {
    std::unordered_set<HWND> secondaryTaskbarWindows;
    HWND hWnd = FindTaskbarWindows(&secondaryTaskbarWindows);
    if (hWnd) {
        AdjustTaskbarStyle(hWnd);
    }

    for (HWND hSecondaryWnd : secondaryTaskbarWindows) {
        AdjustTaskbarStyle(hSecondaryWnd);
    }
}

void LoadSettings() {
    PCWSTR backgroundStyle = Wh_GetStringSetting(L"backgroundStyle");
    g_settings.style.backgroundStyle = BackgroundStyle::blur;
    if (wcscmp(backgroundStyle, L"acrylicBlur") == 0) {
        g_settings.style.backgroundStyle = BackgroundStyle::acrylicBlur;
    } else if (wcscmp(backgroundStyle, L"color") == 0) {
        g_settings.style.backgroundStyle = BackgroundStyle::color;
    }
    Wh_FreeStringSetting(backgroundStyle);

    int red = Wh_GetIntSetting(L"color.red");
    int green = Wh_GetIntSetting(L"color.green");
    int blue = Wh_GetIntSetting(L"color.blue");
    bool accentColor = Wh_GetIntSetting(L"color.accentColor");
    int transparency = Wh_GetIntSetting(L"color.transparency");

    g_settings.style.color = (COLORREF)((BYTE)red | ((WORD)((BYTE)green) << 8) |
                                        (((DWORD)(BYTE)blue) << 16) |
                                        (((DWORD)(BYTE)transparency) << 24));
    g_settings.style.accentColor = accentColor;

    g_settings.onlyWhenMaximized = Wh_GetIntSetting(L"onlyWhenMaximized");

    g_settings.excludedPrograms.clear();

    for (int i = 0;; i++) {
        PCWSTR program = Wh_GetStringSetting(L"excludedPrograms[%d]", i);

        bool hasProgram = *program;
        if (hasProgram) {
            std::wstring programUpper = program;
            LCMapStringEx(
                LOCALE_NAME_USER_DEFAULT, LCMAP_UPPERCASE, &programUpper[0],
                static_cast<int>(programUpper.length()), &programUpper[0],
                static_cast<int>(programUpper.length()), nullptr, nullptr, 0);

            g_settings.excludedPrograms.insert(std::move(programUpper));
        }

        Wh_FreeStringSetting(program);

        if (!hasProgram) {
            break;
        }
    }

    if (Wh_GetIntSetting(L"styleForDarkMode.use")) {
        TaskbarStyle style;

        PCWSTR backgroundStyle =
            Wh_GetStringSetting(L"styleForDarkMode.backgroundStyle");
        style.backgroundStyle = BackgroundStyle::blur;
        if (wcscmp(backgroundStyle, L"acrylicBlur") == 0) {
            style.backgroundStyle = BackgroundStyle::acrylicBlur;
        } else if (wcscmp(backgroundStyle, L"color") == 0) {
            style.backgroundStyle = BackgroundStyle::color;
        }
        Wh_FreeStringSetting(backgroundStyle);

        int red = Wh_GetIntSetting(L"styleForDarkMode.color.red");
        int green = Wh_GetIntSetting(L"styleForDarkMode.color.green");
        int blue = Wh_GetIntSetting(L"styleForDarkMode.color.blue");
        int transparency =
            Wh_GetIntSetting(L"styleForDarkMode.color.transparency");

        style.color = (COLORREF)((BYTE)red | ((WORD)((BYTE)green) << 8) |
                                 (((DWORD)(BYTE)blue) << 16) |
                                 (((DWORD)(BYTE)transparency) << 24));

        g_settings.darkModeStyle = std::move(style);
    } else {
        g_settings.darkModeStyle.reset();
    }
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

    WindhawkUtils::Wh_SetFunctionHookT(pSetWindowCompositionAttribute,
                                       SetWindowCompositionAttribute_Hook,
                                       &SetWindowCompositionAttribute_Original);

    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L">");

    WNDCLASS wndclass;
    if (GetClassInfo(GetModuleHandle(nullptr), L"Shell_TrayWnd", &wndclass)) {
        AdjustAllTaskbarStyles();
    }
}

void Wh_ModUninit() {
    Wh_Log(L">");

    if (g_winEventHookThread) {
        PostThreadMessage(GetThreadId(g_winEventHookThread), WM_APP, 0, 0);
        WaitForSingleObject(g_winEventHookThread, INFINITE);
        CloseHandle(g_winEventHookThread);
        g_winEventHookThread = nullptr;
    }

    std::unordered_set<HWND> secondaryTaskbarWindows;
    HWND hWnd = FindTaskbarWindows(&secondaryTaskbarWindows);
    if (hWnd) {
        ResetTaskbarStyle(hWnd);
    }

    for (HWND hSecondaryWnd : secondaryTaskbarWindows) {
        ResetTaskbarStyle(hSecondaryWnd);
    }
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");

    LoadSettings();

    if (!g_settings.onlyWhenMaximized) {
        std::lock_guard<std::mutex> guard(g_winEventHookThreadMutex);

        if (g_winEventHookThread) {
            PostThreadMessage(GetThreadId(g_winEventHookThread), WM_APP, 0, 0);
            WaitForSingleObject(g_winEventHookThread, INFINITE);
            CloseHandle(g_winEventHookThread);
            g_winEventHookThread = nullptr;
        }
    }

    AdjustAllTaskbarStyles();
}
