// ==WindhawkMod==
// @id              taskbar-background-helper
// @name            Taskbar Background Helper
// @description     Sets the taskbar background for the transparent parts, always or only when there's a maximized window, designed to be used with Windows 11 Taskbar Styler
// @version         1.2
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
#include <unordered_map>
#include <unordered_set>
#include <vector>

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

// Aero Peek events (Show desktop preview).
// Reference:
// https://github.com/TranslucentTB/TranslucentTB/blob/9cfa9eeed5c264f33c8f005512a6649124a69845/Common/undoc/winuser.hpp
static constexpr DWORD EVENT_SYSTEM_PEEKSTART = 0x0021;
static constexpr DWORD EVENT_SYSTEM_PEEKEND = 0x0022;

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

// Private API for window band (z-order band).
// https://blog.adeltax.com/window-z-order-in-windows-10/
using GetWindowBand_t = BOOL(WINAPI*)(HWND hWnd, PDWORD pdwBand);
GetWindowBand_t pGetWindowBand;

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

class MonitorState {
   public:
    struct MonitorChange {
        HMONITOR monitor;
        bool hasMaximized;
    };

    // Update window state, returns list of monitors whose state changed
    std::vector<MonitorChange> UpdateWindowState(HWND hWnd,
                                                 bool isActive,
                                                 HMONITOR monitor) {
        std::lock_guard<std::mutex> lock(m_mutex);
        return UpdateWindowStateInternal(hWnd, isActive, monitor);
    }

    // Remove window entirely (on destruction)
    std::vector<MonitorChange> RemoveWindow(HWND hWnd) {
        std::lock_guard<std::mutex> lock(m_mutex);
        return UpdateWindowStateInternal(hWnd, false, nullptr);
    }

    bool HasMaximizedWindow(HMONITOR monitor) const {
        std::lock_guard<std::mutex> lock(m_mutex);
        auto it = m_maximizedWindowsPerMonitor.find(monitor);
        return it != m_maximizedWindowsPerMonitor.end() && !it->second.empty();
    }

    void Clear() {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_maximizedWindowsPerMonitor.clear();
        m_windowMonitor.clear();
    }

    // For atomic repopulation - clears state, updates cached values, and
    // returns a lock guard. Use AddWindowWhileLocked() to add windows while
    // holding the lock.
    [[nodiscard]] std::unique_lock<std::mutex> BeginRepopulate(
        DWORD dwTaskbarThreadId,
        HWND hShellWindow) {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_maximizedWindowsPerMonitor.clear();
        m_windowMonitor.clear();
        m_taskbarThreadId = dwTaskbarThreadId;
        m_shellWindow = hShellWindow;
        return lock;
    }

    // Add a window while holding the lock from BeginRepopulate()
    void AddWindowWhileLocked(HWND hWnd, HMONITOR monitor) {
        m_windowMonitor[hWnd] = monitor;
        m_maximizedWindowsPerMonitor[monitor].insert(hWnd);
    }

    // Get cached values for event processing (single lock acquisition)
    void GetCachedState(DWORD* pdwTaskbarThreadId, HWND* phShellWindow) const {
        std::lock_guard<std::mutex> lock(m_mutex);
        *pdwTaskbarThreadId = m_taskbarThreadId;
        *phShellWindow = m_shellWindow;
    }

   private:
    std::vector<MonitorChange> UpdateWindowStateInternal(HWND hWnd,
                                                         bool isActive,
                                                         HMONITOR monitor) {
        std::vector<MonitorChange> changes;

        auto windowIt = m_windowMonitor.find(hWnd);
        HMONITOR oldMonitor =
            (windowIt != m_windowMonitor.end()) ? windowIt->second : nullptr;

        bool wasTracked = (oldMonitor != nullptr);
        bool sameMonitor = (oldMonitor == monitor);

        if (isActive) {
            // Window should be tracked
            if (!wasTracked) {
                // New window to track
                m_windowMonitor[hWnd] = monitor;
                auto& newSet = m_maximizedWindowsPerMonitor[monitor];
                newSet.insert(hWnd);
                if (newSet.size() == 1) {
                    changes.push_back({monitor, true});
                }
            } else if (!sameMonitor) {
                // Window moved to different monitor
                auto& oldSet = m_maximizedWindowsPerMonitor[oldMonitor];
                oldSet.erase(hWnd);
                if (oldSet.empty()) {
                    changes.push_back({oldMonitor, false});
                }

                m_windowMonitor[hWnd] = monitor;
                auto& newSet = m_maximizedWindowsPerMonitor[monitor];
                newSet.insert(hWnd);
                if (newSet.size() == 1) {
                    changes.push_back({monitor, true});
                }
            }
            // else: same monitor, still active - no change
        } else {
            // Window should not be tracked
            if (wasTracked) {
                auto& oldSet = m_maximizedWindowsPerMonitor[oldMonitor];
                oldSet.erase(hWnd);
                m_windowMonitor.erase(hWnd);
                if (oldSet.empty()) {
                    changes.push_back({oldMonitor, false});
                }
            }
        }

        return changes;
    }

    mutable std::mutex m_mutex;

    // Track which windows are "active" (maximized/fullscreen) per monitor
    std::unordered_map<HMONITOR, std::unordered_set<HWND>>
        m_maximizedWindowsPerMonitor;

    // Track each window's current monitor (for handling monitor changes)
    std::unordered_map<HWND, HMONITOR> m_windowMonitor;

    // Cached values to avoid expensive lookups in event processing
    DWORD m_taskbarThreadId = 0;
    HWND m_shellWindow = nullptr;
};

MonitorState g_monitorState;

// Encapsulates special view modes (Peek, Multitasking View) that should
// temporarily reset taskbar style to non-maximized.
class SpecialViewModeState {
   public:
    enum class Mode { None, Peek, MultitaskingView };

    // Returns true if any special view mode is active.
    bool IsActive() const { return m_mode != Mode::None; }

    // Sets the special view mode. Returns true if mode changed.
    bool SetMode(Mode mode) {
        Mode expected = Mode::None;
        return m_mode.compare_exchange_strong(expected, mode);
    }

    // Clears a specific mode (only if it's the current mode).
    // Returns true if mode was cleared.
    bool ClearMode(Mode mode) {
        Mode expected = mode;
        return m_mode.compare_exchange_strong(expected, Mode::None);
    }

    void Reset() { m_mode = Mode::None; }

   private:
    std::atomic<Mode> m_mode{Mode::None};
};

SpecialViewModeState g_specialViewMode;

BOOL SetTaskbarStyle(HWND hWnd) {
    Wh_Log(L">");

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
    Wh_Log(L">");

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

HWND GetTaskbarForMonitor(HMONITOR monitor) {
    HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();
    if (!hTaskbarWnd) {
        return nullptr;
    }

    HMONITOR taskbarMonitor = (HMONITOR)GetProp(hTaskbarWnd, L"TaskbarMonitor");
    if (taskbarMonitor == monitor) {
        return hTaskbarWnd;
    }

    DWORD taskbarThreadId = GetWindowThreadProcessId(hTaskbarWnd, nullptr);
    if (!taskbarThreadId) {
        return nullptr;
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

        HMONITOR taskbarMonitor = (HMONITOR)GetProp(hWnd, L"TaskbarMonitor");
        if (taskbarMonitor != monitor) {
            return TRUE;
        }

        hResultWnd = hWnd;
        return FALSE;
    };

    EnumThreadWindows(
        taskbarThreadId,
        [](HWND hWnd, LPARAM lParam) -> BOOL {
            auto& proc = *reinterpret_cast<decltype(enumWindowsProc)*>(lParam);
            return proc(hWnd);
        },
        reinterpret_cast<LPARAM>(&enumWindowsProc));

    return hResultWnd;
}

void UpdateTaskbarStyleForMonitor(HMONITOR monitor, bool hasMaximized) {
    HWND hMMTaskbarWnd = GetTaskbarForMonitor(monitor);
    if (!hMMTaskbarWnd) {
        return;
    }

    if (hasMaximized) {
        SetTaskbarStyle(hMMTaskbarWnd);
    } else {
        ResetTaskbarStyle(hMMTaskbarWnd);
    }
}

BOOL ApplyTaskbarStyleForWindow(HWND hWnd) {
    if (!g_settings.onlyWhenMaximized) {
        return SetTaskbarStyle(hWnd);
    }

    HMONITOR monitor = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
    if (g_specialViewMode.IsActive() ||
        !g_monitorState.HasMaximizedWindow(monitor)) {
        return ResetTaskbarStyle(hWnd);
    }

    return SetTaskbarStyle(hWnd);
}

void EnsureMonitoringThreadStarted();

// Updates all taskbars based on current special view mode and maximized state.
void UpdateAllTaskbarStyles() {
    std::unordered_set<HWND> secondaryTaskbarWindows;
    HWND hTaskbarWnd = FindTaskbarWindows(&secondaryTaskbarWindows);
    if (!hTaskbarWnd) {
        return;
    }

    EnsureMonitoringThreadStarted();
    ApplyTaskbarStyleForWindow(hTaskbarWnd);

    for (HWND hSecondaryWnd : secondaryTaskbarWindows) {
        ApplyTaskbarStyleForWindow(hSecondaryWnd);
    }
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

std::wstring GetProcessFileName(DWORD dwProcessId) {
    HANDLE hProcess =
        OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, dwProcessId);
    if (!hProcess) {
        return std::wstring{};
    }

    WCHAR processPath[MAX_PATH];

    DWORD dwSize = ARRAYSIZE(processPath);
    if (!QueryFullProcessImageName(hProcess, 0, processPath, &dwSize)) {
        CloseHandle(hProcess);
        return std::wstring{};
    }

    CloseHandle(hProcess);

    PCWSTR processFileNameUpper = wcsrchr(processPath, L'\\');
    if (!processFileNameUpper) {
        return std::wstring{};
    }

    processFileNameUpper++;
    return processFileNameUpper;
}

std::wstring GetWindowLogInfo(HWND hWnd) {
    DWORD dwProcessId = 0;
    GetWindowThreadProcessId(hWnd, &dwProcessId);
    std::wstring processName = GetProcessFileName(dwProcessId);

    WCHAR className[256];
    if (!GetClassName(hWnd, className, ARRAYSIZE(className))) {
        wcscpy_s(className, L"<unknown>");
    }

    WCHAR windowName[256];
    if (!GetWindowText(hWnd, windowName, ARRAYSIZE(windowName))) {
        windowName[0] = L'\0';
    }

    LONG style = GetWindowLong(hWnd, GWL_STYLE);
    LONG exStyle = GetWindowLong(hWnd, GWL_EXSTYLE);

    RECT rect{};
    GetWindowRect(hWnd, &rect);

    WCHAR buffer[1024];
    swprintf_s(buffer,
               L"window %08X: PID=%u, process=%s, class=%s, name=%s, "
               L"style=0x%08X, exStyle=0x%08X, rect={%d,%d,%d,%d}",
               (DWORD)(DWORD_PTR)hWnd, dwProcessId, processName.c_str(),
               className, windowName, style, exStyle, rect.left, rect.top,
               rect.right, rect.bottom);
    return buffer;
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

// Detects Win+Tab / Task View window.
// Uses ZBID_IMMERSIVE_APPCHROME band, MultitaskingView thread description, and
// taskbar process.
bool IsMultitaskingViewWindow(HWND hWnd) {
    // Must be in the current process (explorer.exe).
    DWORD dwProcessId = 0;
    DWORD dwThreadId = GetWindowThreadProcessId(hWnd, &dwProcessId);
    if (!dwThreadId || dwProcessId != GetCurrentProcessId()) {
        return false;
    }

    // Check window band - must be ZBID_IMMERSIVE_APPCHROME (5).
    if (pGetWindowBand) {
        DWORD band = 0;
        if (!pGetWindowBand(hWnd, &band) || band != 5) {
            return false;
        }
    }

    // Check thread description for "MultitaskingView".
    HANDLE hThread =
        OpenThread(THREAD_QUERY_LIMITED_INFORMATION, FALSE, dwThreadId);
    if (!hThread) {
        return false;
    }

    bool isMultitaskingView = false;
    PWSTR description = nullptr;
    if (SUCCEEDED(GetThreadDescription(hThread, &description)) && description) {
        isMultitaskingView = wcscmp(description, L"MultitaskingView") == 0;
        LocalFree(description);
    }

    CloseHandle(hThread);
    return isMultitaskingView;
}

bool IsWindowActiveCandidate(HWND hWnd,
                             DWORD dwTaskbarThreadId,
                             HWND hShellWindow) {
    DWORD dwProcessId = 0;
    if (GetWindowThreadProcessId(hWnd, &dwProcessId) == dwTaskbarThreadId) {
        return false;
    }

    if (!IsWindowVisible(hWnd) || IsWindowCloaked(hWnd) || IsIconic(hWnd) ||
        (GetWindowLong(hWnd, GWL_EXSTYLE) & WS_EX_NOACTIVATE)) {
        return false;
    }

    if (hWnd == hShellWindow || GetProp(hWnd, L"DesktopWindow")) {
        return false;
    }

    // Exclude DWM aero peek window.
    WCHAR className[256];
    if (GetClassName(hWnd, className, ARRAYSIZE(className)) &&
        _wcsicmp(className, L"LivePreview") == 0) {
        return false;
    }

    // Check this after the other checks, as it's the most expensive one.
    if (IsWindowExcluded(hWnd)) {
        return false;
    }

    return true;
}

bool IsWindowMaximizedOrFullscreen(HWND hWnd, HMONITOR monitor) {
    WINDOWPLACEMENT wp{
        .length = sizeof(WINDOWPLACEMENT),
    };
    if (GetWindowPlacement(hWnd, &wp) && wp.showCmd == SW_SHOWMAXIMIZED) {
        Wh_Log(L"Maximized %s for monitor %p", GetWindowLogInfo(hWnd).c_str(),
               monitor);
        return true;
    }

    MONITORINFO monitorInfo{
        .cbSize = sizeof(monitorInfo),
    };
    GetMonitorInfo(monitor, &monitorInfo);

    RECT windowRect{};
    DwmGetWindowAttribute(hWnd, DWMWA_EXTENDED_FRAME_BOUNDS, &windowRect,
                          sizeof(windowRect));

    if (EqualRect(&windowRect, &monitorInfo.rcMonitor)) {
        // Spans across the whole monitor, e.g. Win+Tab view.
        Wh_Log(L"Fullscreen %s for monitor %p", GetWindowLogInfo(hWnd).c_str(),
               monitor);
        return true;
    }

    return false;
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

    auto logEvent = [event, hWnd] {
        Wh_Log(
            L"Event %s for %s",
            [](DWORD event) -> PCWSTR {
                switch (event) {
                    case EVENT_OBJECT_CREATE:
                        return L"OBJECT_CREATE";
                    case EVENT_OBJECT_DESTROY:
                        return L"OBJECT_DESTROY";
                    case EVENT_OBJECT_SHOW:
                        return L"OBJECT_SHOW";
                    case EVENT_OBJECT_HIDE:
                        return L"OBJECT_HIDE";
                    case EVENT_OBJECT_LOCATIONCHANGE:
                        return L"OBJECT_LOCATIONCHANGE";
                    case EVENT_OBJECT_CLOAKED:
                        return L"OBJECT_CLOAKED";
                    case EVENT_OBJECT_UNCLOAKED:
                        return L"OBJECT_UNCLOAKED";
                    default:
                        return L"UNKNOWN";
                }
            }(event),
            GetWindowLogInfo(hWnd).c_str());
    };

    // Check for Multitasking View (Win+Tab) window state changes.
    if (IsMultitaskingViewWindow(hWnd)) {
        bool entering = event == EVENT_OBJECT_SHOW ||
                        event == EVENT_OBJECT_UNCLOAKED ||
                        event == EVENT_OBJECT_CREATE;
        bool leaving = event == EVENT_OBJECT_HIDE ||
                       event == EVENT_OBJECT_CLOAKED ||
                       event == EVENT_OBJECT_DESTROY;

        if (entering) {
            Wh_Log(L"MultitaskingView entering");
            if (g_specialViewMode.SetMode(
                    SpecialViewModeState::Mode::MultitaskingView)) {
                UpdateAllTaskbarStyles();
            }
        } else if (leaving) {
            Wh_Log(L"MultitaskingView leaving");
            if (g_specialViewMode.ClearMode(
                    SpecialViewModeState::Mode::MultitaskingView)) {
                UpdateAllTaskbarStyles();
            }
        }

        return;
    }

    std::vector<MonitorState::MonitorChange> changes;

    if (event == EVENT_OBJECT_DESTROY) {
        logEvent();

        // Window is being destroyed, remove it from tracking
        changes = g_monitorState.RemoveWindow(hWnd);
    } else if (event == EVENT_OBJECT_HIDE || event == EVENT_OBJECT_CLOAKED) {
        logEvent();

        // Window is being hidden or cloaked, mark it as inactive
        HMONITOR monitor = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);

        changes = g_monitorState.UpdateWindowState(hWnd, false, monitor);
    } else {
        // Use cached taskbar thread ID and shell window for candidate check
        DWORD dwTaskbarThreadId;
        HWND hShellWindow;
        g_monitorState.GetCachedState(&dwTaskbarThreadId, &hShellWindow);

        HMONITOR monitor = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);

        // Check if window is an active candidate (visible, not cloaked, etc.)
        bool isCandidate =
            IsWindowActiveCandidate(hWnd, dwTaskbarThreadId, hShellWindow);

        if (isCandidate) {
            logEvent();
        }

        // Check if window is maximized or fullscreen
        bool isActive =
            isCandidate && IsWindowMaximizedOrFullscreen(hWnd, monitor);

        changes = g_monitorState.UpdateWindowState(hWnd, isActive, monitor);
    }

    // Update taskbar style for each monitor that changed
    // (but not if in special view mode - handlers will restore when done)
    if (!g_specialViewMode.IsActive()) {
        for (const auto& change : changes) {
            Wh_Log(L"Monitor %p state changed to %s", change.monitor,
                   change.hasMaximized ? L"maximized" : L"not maximized");
            UpdateTaskbarStyleForMonitor(change.monitor, change.hasMaximized);
        }
    }
}

void CALLBACK PeekEventProc(HWINEVENTHOOK hWinEventHook,
                            DWORD event,
                            HWND hWnd,
                            LONG idObject,
                            LONG idChild,
                            DWORD dwEventThread,
                            DWORD dwmsEventTime) {
    bool entering = (event == EVENT_SYSTEM_PEEKSTART);
    Wh_Log(L"Peek %s", entering ? L"start" : L"end");

    if (entering) {
        if (g_specialViewMode.SetMode(SpecialViewModeState::Mode::Peek)) {
            UpdateAllTaskbarStyles();
        }
    } else {
        if (g_specialViewMode.ClearMode(SpecialViewModeState::Mode::Peek)) {
            UpdateAllTaskbarStyles();
        }
    }
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

    HWINEVENTHOOK winPeekEventHook =
        SetWinEventHook(EVENT_SYSTEM_PEEKSTART, EVENT_SYSTEM_PEEKEND, nullptr,
                        PeekEventProc, 0, 0, WINEVENT_OUTOFCONTEXT);
    if (!winPeekEventHook) {
        Wh_Log(L"Error: SetWinEventHook for PEEK");
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

    if (winPeekEventHook) {
        UnhookWinEvent(winPeekEventHook);
    }

    g_specialViewMode.Reset();
    g_monitorState.Clear();

    return 0;
}

void PopulateMonitorState() {
    HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();
    DWORD dwTaskbarThreadId =
        hTaskbarWnd ? GetWindowThreadProcessId(hTaskbarWnd, nullptr) : 0;
    HWND hShellWindow = GetShellWindow();

    // Hold the lock for the entire repopulation to ensure atomicity.
    // Also caches taskbar thread ID and shell window for event processing.
    auto lock = g_monitorState.BeginRepopulate(dwTaskbarThreadId, hShellWindow);

    auto enumWindowsProc = [&](HWND hWnd) -> BOOL {
        if (!IsWindowActiveCandidate(hWnd, dwTaskbarThreadId, hShellWindow)) {
            return TRUE;
        }

        HMONITOR monitor = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
        if (IsWindowMaximizedOrFullscreen(hWnd, monitor)) {
            Wh_Log(L"Initial scan: tracking %s",
                   GetWindowLogInfo(hWnd).c_str());
            g_monitorState.AddWindowWhileLocked(hWnd, monitor);
        }

        return TRUE;
    };

    EnumWindows(
        [](HWND hWnd, LPARAM lParam) -> BOOL {
            auto& proc = *reinterpret_cast<decltype(enumWindowsProc)*>(lParam);
            return proc(hWnd);
        },
        reinterpret_cast<LPARAM>(&enumWindowsProc));
}

void EnsureMonitoringThreadStarted() {
    if (!g_settings.onlyWhenMaximized || g_winEventHookThread) {
        return;
    }

    std::lock_guard<std::mutex> guard(g_winEventHookThreadMutex);
    if (!g_winEventHookThread) {
        PopulateMonitorState();
        g_winEventHookThread =
            CreateThread(nullptr, 0, WinEventHookThread, nullptr, 0, nullptr);
    }
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

    if (!IsTaskbarWindow(hWnd)) {
        return original();
    }

    EnsureMonitoringThreadStarted();
    return ApplyTaskbarStyleForWindow(hWnd);
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

    HMODULE hUser32 =
        LoadLibraryEx(L"user32.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
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

    pGetWindowBand = (GetWindowBand_t)GetProcAddress(hUser32, "GetWindowBand");

    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L">");

    WNDCLASS wndclass;
    if (GetClassInfo(GetModuleHandle(nullptr), L"Shell_TrayWnd", &wndclass)) {
        UpdateAllTaskbarStyles();
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

    {
        std::lock_guard<std::mutex> guard(g_winEventHookThreadMutex);

        if (g_winEventHookThread) {
            PostThreadMessage(GetThreadId(g_winEventHookThread), WM_APP, 0, 0);
            WaitForSingleObject(g_winEventHookThread, INFINITE);
            CloseHandle(g_winEventHookThread);
            g_winEventHookThread = nullptr;
        }
    }

    UpdateAllTaskbarStyles();
}
