// ==WindhawkMod==
// @id              custom-shortcuts
// @name            Custom Shortcuts
// @description     Lightweight custom productivity shortcuts combining same-app window switching with Alt+`, custom application launchers, and virtual desktop navigation
// @version         1.0.0
// @author          Gustavo Rudiger
// @github          https://github.com/gustavotr
// @include         windhawk.exe
// @compilerOptions -lgdi32 -ldwmapi -lshlwapi -lole32 -luuid
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Custom Shortcuts

A lightweight, personal productivity mod for Windhawk that combines three focused features into a single, clean tool mod:

1. **Same-App Window Switcher**: Cycle through open windows of the currently active application with a clean, responsive HUD preview without altering or replacing your global Alt+Tab.
2. **Custom Application Launchers**: Launch configured terminal apps or commands (e.g. `cmd.exe`, `powershell -NoExit -Command "Set-Location $HOME"`, `wsl.exe`) directly via custom hotkeys (default `Win + Enter`).
3. **Virtual Desktop Navigation**: Direct switching to virtual desktops 1 through 9 using modifier + number keys.

This mod is tailored for users who want these specific daily conveniences combined in a single lightweight mod without needing the extensive configuration, overhead, or full feature sets of separate heavier mods.

## Acknowledgements & Base Projects

This mod was built for a custom workflow inspired by and referencing the following excellent Windhawk mods:

- **[Virtual Desktop Helper](https://windhawk.net/mods/virtual-desktop-helper)** by **u2x1**: Base implementation for undocumented Windows Virtual Desktop Manager COM interfaces (`IVirtualDesktopManagerInternal`) and per-build vtable structures.
- **[Simple Window Switcher](https://windhawk.net/mods/simple-window-switcher)** by **m417z**: Inspiration for standalone same-app window cycling with `` Alt + ` ``.
- **[Keyboard Shortcut Actions](https://windhawk.net/mods/keyboard-shortcut-actions)** by **m417z**: Concept for hotkey-based process launching.

## Configurable Keybindings

You can customize the key combination for every action in the mod settings according to your personal preference:

| Action | Default Combination | Description |
| :--- | :--- | :--- |
| **Same-App Window Switcher** | `` Alt + ` `` | Cycle through open windows of the currently active application with a clean HUD preview. |
| **Custom App Launchers** | `Win + Enter` | Launch any configured application or command (e.g. `cmd.exe`, `powershell -NoExit -Command "Set-Location $HOME"`, `wsl.exe`). |
| **Virtual Desktop Navigation** | `Alt + 1 ... 9` | Direct switching to virtual desktop 1 through 9 (disabled by default to avoid hotkey conflicts). |

*To disable any shortcut, simply clear its setting field.*

## Supported Key Formats

Key combinations are specified in the format `Modifier+Key` or `Modifier+Modifier+Key`.
- **Modifiers**: `Alt`, `Ctrl`, `Shift`, `Win`
- **Keys**: Letters (`A-Z`), Numbers (`0-9`), Function keys (`F1-F24`), Backtick (`` ` ``), `Tab`, `Space`, `Enter`, `Esc`, `Home`, `End`, `Delete`, Arrows, etc.
- **Examples**: `` Alt+` ``, `Win+T`, `Ctrl+Alt+T`, `Win+Q`, `Alt+Q`, `Win+W`, `Win+Enter`, `F11`.

## How the Same-App Switcher Works

1. Press your configured hotkey (default `` Alt + ` ``) to focus and list only windows of the active application.
2. A native Alt+Tab-style preview displays all matching windows with live DWM thumbnails, icons, and titles.
3. Keep the modifier (e.g. `Alt`) held down and tap the key (or `Shift + key`) to cycle forward and backward.
4. Use arrow keys (`Up`/`Down`/`Left`/`Right`) to browse. Press `Esc` to cancel.
5. Release the modifier (or press `Enter`) to switch to the selected window.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- shortcuts:
    - hotkeySameApp: "Alt+`"
      $name: Same-App Window Switcher
      $description: >-
        Key combination to cycle windows of the active application (e.g. Alt+`, Alt+Tab, Win+`). Leave empty to disable.
    - hotkeyCustomApp1: "Win+Enter"
      $name: Custom App 1 Shortcut
      $description: >-
        Key combination to launch Custom Application 1 (e.g. Win+Enter). Leave empty to disable.
    - hotkeyCustomApp2: ""
      $name: Custom App 2 Shortcut
      $description: >-
        Key combination to launch Custom Application 2. Leave empty to disable.
    - hotkeyCustomApp3: ""
      $name: Custom App 3 Shortcut
      $description: >-
        Key combination to launch Custom Application 3. Leave empty to disable.
- customAppOptions:
    - customAppPath1: "cmd.exe"
      $name: Custom App 1 Command
      $description: >-
        Application executable and optional arguments to launch (e.g. cmd.exe, powershell -NoExit -Command "Set-Location $HOME", wsl.exe).
    - customAppPath2: ""
      $name: Custom App 2 Command
      $description: >-
        Application executable and optional arguments to launch (e.g. powershell -NoExit -Command "Set-Location $HOME", wsl --cd ~).
    - customAppPath3: ""
      $name: Custom App 3 Command
      $description: Application executable and optional arguments to launch.
- hudOptions:
    - showHud: true
      $name: Show HUD preview
      $description: Show visual switcher preview popup when switching same-app windows.
    - hudTheme: system
      $name: HUD Color Theme
      $description: Visual theme for the switcher HUD.
      $options:
        - system: System
        - dark: Dark
        - light: Light
- virtualDesktops:
    - enableDirectSwitching: false
      $name: Enable Direct Switching
      $description: Switch directly to virtual desktop 1 through 9 using number keys. Disabled by default to avoid conflicts with Windows taskbar shortcuts (Win+1..9).
    - desktopModifier: "alt"
      $name: Virtual Desktop Modifier
      $description: >-
        Modifier key combination combined with number keys 1-9 (e.g. alt, win, ctrl+win). Defaults to alt.
    - windowsVersion: "auto"
      $name: Windows Version Compatibility
      $description: >-
        Windows version compatibility for internal desktop manager APIs. Defaults to auto.
      $options:
        - auto: Auto-detect
        - win10_old: Windows 10 (Build < 20348)
        - win10_20348: Windows 10 (Build 20348 - 21999)
        - win11_22000: Windows 11 (Build 22000 - 22482)
        - win11_22621: Windows 11 (Build 22621/22631/23H2)
        - win11_26100: Windows 11 (Build 26100+ / 24H2)
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <shellapi.h>
#include <shlwapi.h>
#include <dwmapi.h>
#include <objbase.h>
#include <objectarray.h>
#include <shobjidl.h>
#include <windhawk_utils.h>
#include <vector>
#include <string>
#include <algorithm>
#include <atomic>
#include <cwctype>

// Hotkey binding representation
struct HotkeyBinding {
    UINT modifiers = 0; // MOD_ALT, MOD_CONTROL, MOD_SHIFT, MOD_WIN
    UINT vk = 0;        // Virtual key code
    bool valid = false;
    std::wstring originalStr;

    bool Matches(UINT activeModifiers, UINT pressedVk) const {
        if (!valid) return false;
        UINT cleanActiveMods = activeModifiers & (MOD_ALT | MOD_CONTROL | MOD_SHIFT | MOD_WIN);
        UINT cleanTargetMods = modifiers & (MOD_ALT | MOD_CONTROL | MOD_SHIFT | MOD_WIN);
        return (cleanTargetMods == cleanActiveMods) && (vk == pressedVk);
    }
};

// Custom application shortcut entry
struct CustomAppShortcut {
    HotkeyBinding binding;
    std::wstring command;
};

// Mod settings structure
struct ModSettings {
    HotkeyBinding bindingSameApp;
    std::vector<CustomAppShortcut> customApps;
    bool showHud = true;
    std::wstring hudTheme = L"system";
    bool enableDirectSwitching = false;
    UINT desktopModifier = MOD_ALT;
    std::wstring windowsVersion = L"auto";
};

// Window Entry for same-app switcher
struct AppWindowEntry {
    HWND hWnd = NULL;
    std::wstring title;
    HICON hIcon = NULL;
    HTHUMBNAIL hThumbnail = NULL;
    RECT rcCard = {0};
    RECT rcThumb = {0};
};

// Layout metrics computed dynamically based on monitor work area and DPI
struct HudLayoutMetrics {
    int winCornerRadius = 16;
    int cardCornerRadius = 12;
    int iconSize = 18;
    int fontSize = 12;
    int thumbMargin = 8;
};

// Synchronization and globals
static SRWLOCK g_settingsLock = SRWLOCK_INIT;
static ModSettings g_settings;
static HudLayoutMetrics g_hudLayout;

static std::atomic<bool> g_hudVisible{false};
static std::atomic<UINT> g_hudTriggerModifier{MOD_ALT};
static std::atomic<UINT> g_desktopCount{0};

static HANDLE g_hWorkerReady = NULL;
static HANDLE g_hHookReady = NULL;
static HANDLE g_hWorkerThread = NULL;
static DWORD g_dwWorkerThreadId = 0;
static HANDLE g_hHookThread = NULL;
static DWORD g_dwHookThreadId = 0;
static HWND g_hHudWnd = NULL;
static HHOOK g_hKeyboardHook = NULL;

typedef VOID (WINAPI *SwitchToThisWindow_t)(HWND, BOOL);
static SwitchToThisWindow_t pfnSwitchToThisWindow = nullptr;

static WCHAR g_targetAppKey[MAX_PATH] = {0};
static WCHAR g_targetExeName[MAX_PATH] = {0};
static std::vector<AppWindowEntry> g_appWindows;
static int g_selectedIndex = 0;

// Internal custom window messages handled by Worker thread
#define TIMER_ALT_POLL             101
#define WM_USER_HUD_SHOW            (WM_USER + 201)
#define WM_USER_HUD_CYCLE           (WM_USER + 202)
#define WM_USER_HUD_COMMIT          (WM_USER + 203)
#define WM_USER_HUD_CANCEL          (WM_USER + 204)
#define WM_USER_LAUNCH_APP          (WM_USER + 205)
#define WM_USER_SWITCH_DESKTOP      (WM_USER + 206)
#define WM_USER_SETTINGS_CHANGED    (WM_USER + 207)

#define SAFE_RELEASE(p) do { if (p) { (p)->Release(); (p) = nullptr; } } while (0)

// Forward declarations
static bool ParseHotkeyString(const std::wstring& str, HotkeyBinding& out);
static UINT ParseModifierOnlyString(const std::wstring& str);
static void LoadModSettings();
static void ShowHud();
static void HideHud(bool commitSwitch);
static void CycleSelection(int direction);
static void RefreshAppWindows();
static void SwitchToAppWindow(HWND hTarget);
static void LaunchApplication(const std::wstring& commandLine);
static bool SwitchToDesktopNumber(int desktopNum);
static UINT UpdateVirtualDesktopCount();
static void CleanupVirtualDesktopAPI();
static DWORD WINAPI WorkerThreadProc(LPVOID lpParam);
static DWORD WINAPI HookThreadProc(LPVOID lpParam);
static LRESULT CALLBACK HudWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
static LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);

// Parse hotkey strings like "Alt+`", "Win+Enter", "Win+T", "ctrl+alt+84", "F11"
static bool ParseHotkeyString(const std::wstring& str, HotkeyBinding& out) {
    out = HotkeyBinding();
    out.originalStr = str;
    if (str.empty()) return false;

    std::vector<std::wstring> tokens;
    size_t start = 0;
    while (start < str.length()) {
        size_t plusPos = str.find(L'+', start);
        if (plusPos == std::wstring::npos) {
            tokens.push_back(str.substr(start));
            break;
        }
        tokens.push_back(str.substr(start, plusPos - start));
        start = plusPos + 1;
    }

    if (tokens.empty()) return false;

    UINT mods = 0;
    UINT vk = 0;

    for (size_t i = 0; i < tokens.size(); ++i) {
        std::wstring token = tokens[i];
        while (!token.empty() && (token.front() == L' ' || token.front() == L'\t')) token.erase(0, 1);
        while (!token.empty() && (token.back() == L' ' || token.back() == L'\t')) token.pop_back();
        if (token.empty()) continue;

        std::wstring upper = token;
        std::transform(upper.begin(), upper.end(), upper.begin(), ::towupper);

        if (upper == L"CTRL" || upper == L"CONTROL") {
            mods |= MOD_CONTROL;
        } else if (upper == L"ALT" || upper == L"MENU") {
            mods |= MOD_ALT;
        } else if (upper == L"SHIFT") {
            mods |= MOD_SHIFT;
        } else if (upper == L"WIN" || upper == L"WINDOWS" || upper == L"SUPER" || upper == L"META") {
            mods |= MOD_WIN;
        } else {
            bool allDigits = true;
            for (wchar_t c : upper) {
                if (!iswdigit(c)) { allDigits = false; break; }
            }

            if (allDigits && !upper.empty()) {
                vk = static_cast<UINT>(_wtoi(upper.c_str()));
            } else if (upper == L"`" || upper == L"~" || upper == L"BACKTICK" || upper == L"GRAVE" || upper == L"TILDE") {
                vk = VK_OEM_3;
            } else if (upper == L"TAB") {
                vk = VK_TAB;
            } else if (upper == L"ENTER" || upper == L"RETURN") {
                vk = VK_RETURN;
            } else if (upper == L"ESC" || upper == L"ESCAPE") {
                vk = VK_ESCAPE;
            } else if (upper == L"SPACE" || upper == L"SPACEBAR") {
                vk = VK_SPACE;
            } else if (upper == L"BACKSPACE") {
                vk = VK_BACK;
            } else if (upper == L"DELETE" || upper == L"DEL") {
                vk = VK_DELETE;
            } else if (upper == L"INSERT" || upper == L"INS") {
                vk = VK_INSERT;
            } else if (upper == L"HOME") {
                vk = VK_HOME;
            } else if (upper == L"END") {
                vk = VK_END;
            } else if (upper == L"PAGEUP" || upper == L"PGUP") {
                vk = VK_PRIOR;
            } else if (upper == L"PAGEDOWN" || upper == L"PGDN") {
                vk = VK_NEXT;
            } else if (upper == L"UP") {
                vk = VK_UP;
            } else if (upper == L"DOWN") {
                vk = VK_DOWN;
            } else if (upper == L"LEFT") {
                vk = VK_LEFT;
            } else if (upper == L"RIGHT") {
                vk = VK_RIGHT;
            } else if (upper.length() >= 2 && upper[0] == L'F' && iswdigit(upper[1])) {
                int fNum = _wtoi(upper.c_str() + 1);
                if (fNum >= 1 && fNum <= 24) {
                    vk = VK_F1 + (fNum - 1);
                }
            } else if (upper.length() == 1) {
                wchar_t c = upper[0];
                if ((c >= L'A' && c <= L'Z') || (c >= L'0' && c <= L'9')) {
                    vk = static_cast<UINT>(c);
                } else if (c == L'-' || c == L'_') {
                    vk = VK_OEM_MINUS;
                } else if (c == L'=' || c == L'+') {
                    vk = VK_OEM_PLUS;
                } else if (c == L'[' || c == L'{') {
                    vk = VK_OEM_4;
                } else if (c == L']' || c == L'}') {
                    vk = VK_OEM_6;
                } else if (c == L'\\' || c == L'|') {
                    vk = VK_OEM_5;
                } else if (c == L';' || c == L':') {
                    vk = VK_OEM_1;
                } else if (c == L'\'' || c == L'\"') {
                    vk = VK_OEM_7;
                } else if (c == L',' || c == L'<') {
                    vk = VK_OEM_COMMA;
                } else if (c == L'.' || c == L'>') {
                    vk = VK_OEM_PERIOD;
                } else if (c == L'/' || c == L'?') {
                    vk = VK_OEM_2;
                }
            }
        }
    }

    if (vk != 0) {
        out.modifiers = mods;
        out.vk = vk;
        out.valid = true;
        return true;
    }
    return false;
}

// Parse a modifier string like "win", "alt", "ctrl+win", "alt+shift"
static UINT ParseModifierOnlyString(const std::wstring& str) {
    if (str.empty()) return MOD_ALT;
    UINT mods = 0;
    size_t start = 0;
    while (start < str.length()) {
        size_t plusPos = str.find(L'+', start);
        std::wstring token = (plusPos == std::wstring::npos) ? str.substr(start) : str.substr(start, plusPos - start);
        while (!token.empty() && (token.front() == L' ' || token.front() == L'\t')) token.erase(0, 1);
        while (!token.empty() && (token.back() == L' ' || token.back() == L'\t')) token.pop_back();
        std::transform(token.begin(), token.end(), token.begin(), ::towupper);
        if (token == L"CTRL" || token == L"CONTROL") mods |= MOD_CONTROL;
        else if (token == L"ALT" || token == L"MENU") mods |= MOD_ALT;
        else if (token == L"SHIFT") mods |= MOD_SHIFT;
        else if (token == L"WIN" || token == L"WINDOWS" || token == L"SUPER" || token == L"META") mods |= MOD_WIN;
        if (plusPos == std::wstring::npos) break;
        start = plusPos + 1;
    }
    return mods ? mods : MOD_ALT;
}

// Helper: check if a window is validly visible on screen
static bool IsReallyVisible(HWND hWnd) {
    if (!IsWindow(hWnd)) return false;
    if (IsIconic(hWnd)) return true; // Minimized windows are valid switch targets
    if (!IsWindowVisible(hWnd)) return false;
    RECT r;
    GetWindowRect(hWnd, &r);
    return !IsRectEmpty(&r);
}

// Helper: check if an ancestor in the owner chain is a tool window
static bool IsOwnerToolWindow(HWND hWnd) {
    HWND own = GetWindow(hWnd, GW_OWNER);
    while (IsWindow(own)) {
        if (GetWindowLongPtrW(own, GWL_EXSTYLE) & WS_EX_TOOLWINDOW) return true;
        own = GetWindow(own, GW_OWNER);
    }
    return false;
}

// Check if a window is an Alt+Tab eligible top-level window
static bool IsSwitchableAppWindow(HWND hWnd) {
    if (!IsWindow(hWnd) || hWnd == g_hHudWnd) return false;
    if (!IsReallyVisible(hWnd)) return false;

    DWORD ex = (DWORD)GetWindowLongPtrW(hWnd, GWL_EXSTYLE);
    if (ex & WS_EX_TOOLWINDOW) return false;
    if ((ex & WS_EX_NOACTIVATE) && !(ex & WS_EX_APPWINDOW)) return false;

    HWND own = GetWindow(hWnd, GW_OWNER);
    if (!(ex & WS_EX_APPWINDOW) && IsWindow(own) && IsReallyVisible(own) &&
        !(GetWindowLongPtrW(own, GWL_EXSTYLE) & WS_EX_TOOLWINDOW) &&
        !IsOwnerToolWindow(own)) {
        return false;
    }

    if (IsOwnerToolWindow(hWnd)) return false;

    int cloaked = 0;
    if (SUCCEEDED(DwmGetWindowAttribute(hWnd, DWMWA_CLOAKED, &cloaked, sizeof(cloaked))) && cloaked) {
        return false;
    }

    WCHAR className[64] = {0};
    GetClassNameW(hWnd, className, ARRAYSIZE(className));
    if (_wcsicmp(className, L"Shell_TrayWnd") == 0 ||
        _wcsicmp(className, L"Shell_SecondaryTrayWnd") == 0 ||
        _wcsicmp(className, L"Progman") == 0 ||
        _wcsicmp(className, L"WorkerW") == 0 ||
        _wcsicmp(className, L"Windows.UI.Core.CoreWindow") == 0) {
        return false;
    }

    return true;
}

// Extract window icon safely with timeout
static HICON GetWindowAppIcon(HWND hWnd) {
    HICON hIcon = NULL;
    SendMessageTimeoutW(hWnd, WM_GETICON, ICON_SMALL2, 0, SMTO_ABORTIFHUNG, 25, (DWORD_PTR*)&hIcon);
    if (!hIcon) {
        SendMessageTimeoutW(hWnd, WM_GETICON, ICON_SMALL, 0, SMTO_ABORTIFHUNG, 25, (DWORD_PTR*)&hIcon);
    }
    if (!hIcon) {
        SendMessageTimeoutW(hWnd, WM_GETICON, ICON_BIG, 0, SMTO_ABORTIFHUNG, 25, (DWORD_PTR*)&hIcon);
    }
    if (!hIcon) {
        hIcon = (HICON)GetClassLongPtrW(hWnd, GCLP_HICONSM);
    }
    if (!hIcon) {
        hIcon = (HICON)GetClassLongPtrW(hWnd, GCLP_HICON);
    }
    if (!hIcon) {
        hIcon = LoadIconW(NULL, IDI_APPLICATION);
    }
    return hIcon;
}

// Callback to locate child CoreWindow inside UWP ApplicationFrameWindow
static BOOL CALLBACK FindCoreWindowProc(HWND hChild, LPARAM lp) {
    WCHAR cls[128] = {0};
    GetClassNameW(hChild, cls, ARRAYSIZE(cls));
    if (_wcsicmp(cls, L"Windows.UI.Core.CoreWindow") == 0) {
        *(HWND*)lp = hChild;
        return FALSE;
    }
    return TRUE;
}

// Identity key used to group windows of the same application
static bool GetWindowAppKey(HWND hWnd, WCHAR* outKey, size_t keyCch, WCHAR* outExeName, size_t exeCch) {
    if (outKey) outKey[0] = 0;
    if (outExeName) outExeName[0] = 0;
    if (!IsWindow(hWnd)) return false;

    DWORD pid = 0;
    WCHAR className[256] = {0};
    GetClassNameW(hWnd, className, ARRAYSIZE(className));

    if (_wcsicmp(className, L"ApplicationFrameWindow") == 0) {
        HWND hCore = NULL;
        EnumChildWindows(hWnd, FindCoreWindowProc, (LPARAM)&hCore);
        if (hCore) {
            GetWindowThreadProcessId(hCore, &pid);
        }
    }

    if (!pid) {
        GetWindowThreadProcessId(hWnd, &pid);
    }
    if (!pid) return false;

    // File Explorer special grouping for cabinet windows
    if (_wcsicmp(className, L"CabinetWClass") == 0 || _wcsicmp(className, L"ExploreWClass") == 0) {
        if (outKey) wcsncpy_s(outKey, keyCch, L"explorer.exe:cabinetwclass", _TRUNCATE);
        if (outExeName) wcsncpy_s(outExeName, exeCch, L"explorer.exe", _TRUNCATE);
        return true;
    }

    HANDLE hProc = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (hProc) {
        WCHAR exePath[MAX_PATH] = {0};
        DWORD size = MAX_PATH;
        if (QueryFullProcessImageNameW(hProc, 0, exePath, &size)) {
            LPCWSTR fName = PathFindFileNameW(exePath);
            if (outExeName && fName) {
                wcsncpy_s(outExeName, exeCch, fName, _TRUNCATE);
                for (size_t i = 0; outExeName[i]; ++i) outExeName[i] = (WCHAR)::towlower(outExeName[i]);
            }
            if (outKey) {
                for (DWORD i = 0; i < size; ++i) exePath[i] = (WCHAR)::towlower(exePath[i]);
                wcsncpy_s(outKey, keyCch, exePath, _TRUNCATE);
            }
        }
        CloseHandle(hProc);
    }

    if (outKey && !outKey[0]) {
        swprintf_s(outKey, keyCch, L"pid:%lu", pid);
    }
    return true;
}

// Bring target window to foreground reliably
static void SwitchToAppWindow(HWND hTarget) {
    if (!IsWindow(hTarget)) return;

    HWND hForeground = GetForegroundWindow();
    if (hForeground == hTarget) return;

    HWND hPopup = GetLastActivePopup(hTarget);
    HWND hWndToActivate = (IsWindow(hPopup) && IsWindowVisible(hPopup)) ? hPopup : hTarget;

    if (IsIconic(hWndToActivate)) {
        ShowWindow(hWndToActivate, SW_RESTORE);
    } else {
        ShowWindow(hWndToActivate, SW_SHOW);
    }

    DWORD curThreadId = GetCurrentThreadId();
    DWORD foreThreadId = hForeground ? GetWindowThreadProcessId(hForeground, NULL) : 0;
    DWORD targetThreadId = GetWindowThreadProcessId(hWndToActivate, NULL);

    if (foreThreadId && foreThreadId != curThreadId) {
        AttachThreadInput(curThreadId, foreThreadId, TRUE);
    }
    if (targetThreadId && targetThreadId != curThreadId) {
        AttachThreadInput(curThreadId, targetThreadId, TRUE);
    }

    BringWindowToTop(hWndToActivate);
    BOOL ok = SetForegroundWindow(hWndToActivate);
    if (!ok && pfnSwitchToThisWindow) {
        pfnSwitchToThisWindow(hWndToActivate, TRUE);
    }

    if (foreThreadId && foreThreadId != curThreadId) {
        AttachThreadInput(curThreadId, foreThreadId, FALSE);
    }
    if (targetThreadId && targetThreadId != curThreadId) {
        AttachThreadInput(curThreadId, targetThreadId, FALSE);
    }
}

// Window enumeration callback for matching current application
static BOOL CALLBACK EnumWindowsProc(HWND hWnd, LPARAM lParam) {
    if (hWnd == g_hHudWnd) return TRUE;

    if (!IsSwitchableAppWindow(hWnd)) {
        return TRUE;
    }

    WCHAR key[MAX_PATH] = {0};
    WCHAR exeName[MAX_PATH] = {0};
    if (!GetWindowAppKey(hWnd, key, ARRAYSIZE(key), exeName, ARRAYSIZE(exeName))) {
        return TRUE;
    }

    bool matches = (_wcsicmp(key, g_targetAppKey) == 0) ||
                   (g_targetExeName[0] && exeName[0] && _wcsicmp(exeName, g_targetExeName) == 0);

    if (matches) {
        AppWindowEntry entry;
        entry.hWnd = hWnd;

        WCHAR title[256] = {0};
        GetWindowTextW(hWnd, title, ARRAYSIZE(title));
        if (!title[0]) {
            LPCWSTR name = exeName[0] ? exeName : PathFindFileNameW(key);
            wcsncpy_s(title, (name && *name) ? name : L"Window", _TRUNCATE);
        }
        entry.title = title;
        entry.hIcon = GetWindowAppIcon(hWnd);
        g_appWindows.push_back(entry);
    }
    return TRUE;
}

static void RefreshAppWindows() {
    g_appWindows.clear();
    HWND hForeground = GetForegroundWindow();
    if (!hForeground || hForeground == g_hHudWnd) return;

    if (!GetWindowAppKey(hForeground, g_targetAppKey, ARRAYSIZE(g_targetAppKey),
                         g_targetExeName, ARRAYSIZE(g_targetExeName))) {
        return;
    }

    EnumWindows(EnumWindowsProc, 0);

    // Ensure the currently active window is placed first
    for (size_t i = 0; i < g_appWindows.size(); ++i) {
        if (g_appWindows[i].hWnd == hForeground) {
            if (i != 0) {
                AppWindowEntry active = g_appWindows[i];
                g_appWindows.erase(g_appWindows.begin() + i);
                g_appWindows.insert(g_appWindows.begin(), active);
            }
            break;
        }
    }
}

static void CycleSelection(int direction) {
    if (g_appWindows.empty()) return;
    int count = static_cast<int>(g_appWindows.size());
    g_selectedIndex = (g_selectedIndex + direction + count) % count;
    Wh_Log(L"Cycling selection: %d of %d", g_selectedIndex + 1, count);
    if (g_hHudWnd) {
        InvalidateRect(g_hHudWnd, NULL, TRUE);
    }
}

// Helper: query system dark mode preference
static bool IsSystemDarkMode() {
    AcquireSRWLockShared(&g_settingsLock);
    std::wstring theme = g_settings.hudTheme;
    ReleaseSRWLockShared(&g_settingsLock);

    if (theme == L"light") return false;
    if (theme == L"dark") return true;

    // "system" setting or default: query registry
    DWORD lightTheme = 1;
    DWORD sz = sizeof(lightTheme);
    HKEY hKey;
    if (RegOpenKeyExW(HKEY_CURRENT_USER,
                      L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
                      0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        RegQueryValueExW(hKey, L"AppsUseLightTheme", NULL, NULL, (LPBYTE)&lightTheme, &sz);
        RegCloseKey(hKey);
    }
    return lightTheme == 0;
}

// Helper: query system accent color from registry
static COLORREF GetSystemAccentColor(bool isDark) {
    DWORD accent = 0;
    DWORD sz = sizeof(accent);
    HKEY hKey;
    if (RegOpenKeyExW(HKEY_CURRENT_USER,
                      L"Software\\Microsoft\\Windows\\DWM",
                      0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        RegQueryValueExW(hKey, L"AccentColor", NULL, NULL, (LPBYTE)&accent, &sz);
        RegCloseKey(hKey);
    }
    if (accent != 0) {
        BYTE r = accent & 0xFF;
        BYTE g = (accent >> 8) & 0xFF;
        BYTE b = (accent >> 16) & 0xFF;
        return RGB(r, g, b);
    }
    return isDark ? RGB(0, 120, 215) : RGB(0, 102, 204);
}

// Helper: query monitor DPI for accurate display scaling
static UINT GetMonitorDpi(HMONITOR hMon) {
    UINT dpiX = 96, dpiY = 96;
    HMODULE hShcore = GetModuleHandleW(L"shcore.dll");
    if (!hShcore) hShcore = LoadLibraryW(L"shcore.dll");
    if (hShcore) {
        typedef HRESULT(WINAPI *GetDpiForMonitor_t)(HMONITOR, int, UINT*, UINT*);
        GetDpiForMonitor_t pfnGetDpiForMonitor = (GetDpiForMonitor_t)GetProcAddress(hShcore, "GetDpiForMonitor");
        if (pfnGetDpiForMonitor) {
            if (SUCCEEDED(pfnGetDpiForMonitor(hMon, 0 /* MDT_EFFECTIVE_DPI */, &dpiX, &dpiY))) {
                return dpiY ? dpiY : 96;
            }
        }
    }
    return 96;
}

static inline int ScaleDpi(int val, UINT dpi) {
    return MulDiv(val, dpi, 96);
}

static void ShowHud() {
    RefreshAppWindows();
    Wh_Log(L"Found %d window(s) for app key '%s' (%s)",
           (int)g_appWindows.size(), g_targetAppKey, g_targetExeName);

    if (g_appWindows.size() <= 1) {
        Wh_Log(L"Only 1 window found for this app, nothing to cycle.");
        return;
    }

    g_selectedIndex = 1; // Default to the other / next window of the app
    g_hudVisible.store(true);

    if (g_hHudWnd) {
        SetTimer(g_hHudWnd, TIMER_ALT_POLL, 25, NULL);
    }

    bool showHudSetting = true;
    AcquireSRWLockShared(&g_settingsLock);
    showHudSetting = g_settings.showHud;
    ReleaseSRWLockShared(&g_settingsLock);

    if (!showHudSetting || !g_hHudWnd) {
        return;
    }

    int itemCount = static_cast<int>(g_appWindows.size());
    if (itemCount <= 0) return;

    HWND hForeground = GetForegroundWindow();
    HMONITOR hMon = MonitorFromWindow(hForeground ? hForeground : GetDesktopWindow(), MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi = { sizeof(mi) };
    GetMonitorInfoW(hMon, &mi);

    UINT dpi = GetMonitorDpi(hMon);

    int screenW = mi.rcWork.right - mi.rcWork.left;
    int screenH = mi.rcWork.bottom - mi.rcWork.top;
    if (screenW <= 0) screenW = 1920;
    if (screenH <= 0) screenH = 1080;

    int cardHeight = MulDiv(screenH, 70, 100);
    cardHeight = std::max(ScaleDpi(160, dpi), std::min(MulDiv(screenH, 85, 100), cardHeight));

    int footerHeight = MulDiv(cardHeight, 18, 100);
    footerHeight = std::max(ScaleDpi(28, dpi), std::min(ScaleDpi(54, dpi), footerHeight));

    int thumbMargin = MulDiv(cardHeight, 4, 100);
    thumbMargin = std::max(ScaleDpi(12, dpi), std::min(ScaleDpi(28, dpi), thumbMargin));

    int thumbH = cardHeight - footerHeight - (thumbMargin * 2);
    if (thumbH < ScaleDpi(60, dpi)) thumbH = ScaleDpi(60, dpi);
    int thumbW = MulDiv(thumbH, screenW, screenH);
    if (thumbW < ScaleDpi(80, dpi)) thumbW = ScaleDpi(80, dpi);

    int cardWidth = thumbW + (thumbMargin * 2);

    int cardGap = MulDiv(screenW, 2, 100);
    cardGap = std::max(ScaleDpi(24, dpi), std::min(ScaleDpi(52, dpi), cardGap));

    int padX = MulDiv(screenW, 22, 1000);
    padX = std::max(ScaleDpi(28, dpi), std::min(ScaleDpi(60, dpi), padX));

    int padY = MulDiv(screenH, 25, 1000);
    padY = std::max(ScaleDpi(24, dpi), std::min(ScaleDpi(54, dpi), padY));

    int maxW = MulDiv(screenW, 92, 100);
    int totalWidth = padX * 2 + (itemCount * cardWidth) + ((itemCount - 1) * cardGap);

    if (totalWidth > maxW) {
        int availableForCards = maxW - (padX * 2) - ((itemCount - 1) * cardGap);
        int maxCardW = std::max(ScaleDpi(120, dpi), availableForCards / itemCount);
        if (maxCardW < cardWidth) {
            double scale = static_cast<double>(maxCardW) / static_cast<double>(cardWidth);
            cardWidth = maxCardW;
            cardHeight = std::max(ScaleDpi(130, dpi), static_cast<int>(cardHeight * scale));
            footerHeight = std::max(ScaleDpi(24, dpi), std::min(ScaleDpi(54, dpi), MulDiv(cardHeight, 18, 100)));
            thumbMargin = std::max(ScaleDpi(8, dpi), std::min(ScaleDpi(24, dpi), MulDiv(cardHeight, 4, 100)));
        }
        totalWidth = padX * 2 + (itemCount * cardWidth) + ((itemCount - 1) * cardGap);
    }

    int hudWidth = totalWidth;
    int hudHeight = padY * 2 + cardHeight;

    // Update DPI-scaled metrics for WM_PAINT
    g_hudLayout.winCornerRadius = std::max(ScaleDpi(16, dpi), std::min(ScaleDpi(30, dpi), MulDiv(cardHeight, 8, 100)));
    g_hudLayout.cardCornerRadius = std::max(ScaleDpi(12, dpi), std::min(ScaleDpi(22, dpi), MulDiv(cardHeight, 6, 100)));
    g_hudLayout.iconSize = std::max(ScaleDpi(16, dpi), std::min(ScaleDpi(32, dpi), MulDiv(footerHeight, 55, 100)));
    g_hudLayout.fontSize = std::max(ScaleDpi(11, dpi), std::min(ScaleDpi(18, dpi), MulDiv(footerHeight, 38, 100)));
    g_hudLayout.thumbMargin = thumbMargin;

    int posX = mi.rcWork.left + (screenW - hudWidth) / 2;
    int posY = mi.rcWork.top + (screenH - hudHeight) / 2;

    SetWindowPos(g_hHudWnd, HWND_TOPMOST, posX, posY, hudWidth, hudHeight,
                 SWP_SHOWWINDOW | SWP_NOACTIVATE);

    HRGN hRgn = CreateRoundRectRgn(0, 0, hudWidth + 1, hudHeight + 1,
                                   g_hudLayout.winCornerRadius, g_hudLayout.winCornerRadius);
    SetWindowRgn(g_hHudWnd, hRgn, TRUE);
    DeleteObject(hRgn);

    BOOL darkMode = IsSystemDarkMode();
    DwmSetWindowAttribute(g_hHudWnd, 20 /* DWMWA_USE_IMMERSIVE_DARK_MODE */, &darkMode, sizeof(darkMode));
    DWORD cornerPref = 2; // DWMWCP_ROUND
    DwmSetWindowAttribute(g_hHudWnd, 33 /* DWMWA_WINDOW_CORNER_PREFERENCE */, &cornerPref, sizeof(cornerPref));

    for (int i = 0; i < itemCount; ++i) {
        int left = padX + i * (cardWidth + cardGap);
        int top = padY;
        g_appWindows[i].rcCard = { left, top, left + cardWidth, top + cardHeight };

        g_appWindows[i].rcThumb = {
            left + thumbMargin,
            top + thumbMargin,
            left + cardWidth - thumbMargin,
            top + cardHeight - footerHeight - thumbMargin
        };

        if (IsWindow(g_appWindows[i].hWnd)) {
            if (!g_appWindows[i].hThumbnail) {
                HTHUMBNAIL hT = NULL;
                if (SUCCEEDED(DwmRegisterThumbnail(g_hHudWnd, g_appWindows[i].hWnd, &hT))) {
                    g_appWindows[i].hThumbnail = hT;
                }
            }

            if (g_appWindows[i].hThumbnail) {
                DWM_THUMBNAIL_PROPERTIES props = {};
                props.dwFlags = DWM_TNP_RECTDESTINATION | DWM_TNP_VISIBLE | DWM_TNP_OPACITY;
                props.rcDestination = g_appWindows[i].rcThumb;
                props.fVisible = TRUE;
                props.opacity = 255;
                DwmUpdateThumbnailProperties(g_appWindows[i].hThumbnail, &props);
            }
        }
    }

    InvalidateRect(g_hHudWnd, NULL, TRUE);
}

static void HideHud(bool commitSwitch) {
    if (!g_hudVisible.load()) return;

    g_hudVisible.store(false);
    if (g_hHudWnd) {
        KillTimer(g_hHudWnd, TIMER_ALT_POLL);
        ShowWindow(g_hHudWnd, SW_HIDE);
    }

    for (auto& entry : g_appWindows) {
        if (entry.hThumbnail) {
            DwmUnregisterThumbnail(entry.hThumbnail);
            entry.hThumbnail = NULL;
        }
    }

    if (commitSwitch && !g_appWindows.empty() && g_selectedIndex >= 0 &&
        g_selectedIndex < static_cast<int>(g_appWindows.size())) {
        HWND target = g_appWindows[g_selectedIndex].hWnd;
        Wh_Log(L"Switching to window: %p (\"%s\")",
               target, g_appWindows[g_selectedIndex].title.c_str());
        SwitchToAppWindow(target);
    } else {
        Wh_Log(L"Switch cancelled.");
    }
    g_appWindows.clear();
}

// Drawing HUD window
static LRESULT CALLBACK HudWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        RECT clientRect;
        GetClientRect(hWnd, &clientRect);

        HDC memDC = CreateCompatibleDC(hdc);
        HBITMAP memBmp = CreateCompatibleBitmap(hdc, clientRect.right, clientRect.bottom);
        HBITMAP oldBmp = (HBITMAP)SelectObject(memDC, memBmp);

        bool isDark = IsSystemDarkMode();
        COLORREF accentColor = GetSystemAccentColor(isDark);

        COLORREF bgColor = isDark ? RGB(26, 26, 28) : RGB(242, 242, 245);
        COLORREF cardBgColor = isDark ? RGB(36, 36, 40) : RGB(255, 255, 255);
        COLORREF cardBorderColor = isDark ? RGB(52, 52, 56) : RGB(220, 220, 225);

        COLORREF selCardBgColor = isDark ? RGB(50, 52, 58) : RGB(232, 238, 248);
        COLORREF selCardBorderColor = accentColor;

        COLORREF textColor = isDark ? RGB(235, 235, 235) : RGB(25, 25, 25);
        COLORREF thumbPlaceholderColor = isDark ? RGB(20, 20, 22) : RGB(230, 230, 234);

        HBRUSH bgBrush = CreateSolidBrush(bgColor);
        FillRect(memDC, &clientRect, bgBrush);
        DeleteObject(bgBrush);

        HPEN winBorderPen = CreatePen(PS_SOLID, 1, isDark ? RGB(55, 55, 60) : RGB(205, 205, 210));
        HPEN oldWinPen = (HPEN)SelectObject(memDC, winBorderPen);
        SelectObject(memDC, GetStockObject(NULL_BRUSH));
        RoundRect(memDC, clientRect.left, clientRect.top, clientRect.right, clientRect.bottom,
                  g_hudLayout.winCornerRadius, g_hudLayout.winCornerRadius);
        SelectObject(memDC, oldWinPen);
        DeleteObject(winBorderPen);

        HFONT hFontItem = CreateFontW(-g_hudLayout.fontSize, 0, 0, 0, FW_MEDIUM, FALSE, FALSE, FALSE,
                                      DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                                      CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
        HFONT oldFont = (HFONT)SelectObject(memDC, hFontItem);

        for (int i = 0; i < static_cast<int>(g_appWindows.size()); ++i) {
            const auto& entry = g_appWindows[i];
            bool isSelected = (i == g_selectedIndex);

            COLORREF cBg = isSelected ? selCardBgColor : cardBgColor;
            COLORREF cBorder = isSelected ? selCardBorderColor : cardBorderColor;
            int borderWidth = isSelected ? 2 : 1;

            HBRUSH cardBrush = CreateSolidBrush(cBg);
            HPEN cardPen = CreatePen(PS_SOLID, borderWidth, cBorder);
            HBRUSH oldB = (HBRUSH)SelectObject(memDC, cardBrush);
            HPEN oldP = (HPEN)SelectObject(memDC, cardPen);

            RoundRect(memDC, entry.rcCard.left, entry.rcCard.top,
                      entry.rcCard.right, entry.rcCard.bottom,
                      g_hudLayout.cardCornerRadius, g_hudLayout.cardCornerRadius);

            SelectObject(memDC, oldB);
            SelectObject(memDC, oldP);
            DeleteObject(cardBrush);
            DeleteObject(cardPen);

            HBRUSH thumbPlaceholderBrush = CreateSolidBrush(thumbPlaceholderColor);
            FillRect(memDC, &entry.rcThumb, thumbPlaceholderBrush);
            DeleteObject(thumbPlaceholderBrush);

            int iconSize = g_hudLayout.iconSize;
            int footerTop = entry.rcThumb.bottom;
            int footerH = entry.rcCard.bottom - footerTop;
            int iconX = entry.rcCard.left + g_hudLayout.thumbMargin;
            int iconY = footerTop + (footerH - iconSize) / 2;

            if (entry.hIcon) {
                DrawIconEx(memDC, iconX, iconY, entry.hIcon, iconSize, iconSize, 0, NULL, DI_NORMAL);
            }

            RECT textRect = {
                iconX + iconSize + 8,
                footerTop,
                entry.rcCard.right - g_hudLayout.thumbMargin,
                entry.rcCard.bottom
            };

            SetTextColor(memDC, textColor);
            SetBkMode(memDC, TRANSPARENT);
            DrawTextW(memDC, entry.title.c_str(), -1, &textRect,
                      DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS);
        }

        SelectObject(memDC, oldFont);
        DeleteObject(hFontItem);

        BitBlt(hdc, 0, 0, clientRect.right, clientRect.bottom, memDC, 0, 0, SRCCOPY);
        SelectObject(memDC, oldBmp);
        DeleteObject(memBmp);
        DeleteDC(memDC);

        EndPaint(hWnd, &ps);
        return 0;
    }
    case WM_TIMER:
        if (wParam == TIMER_ALT_POLL) {
            if (g_hudVisible.load()) {
                UINT triggerMod = g_hudTriggerModifier.load();
                bool modifierStillDown = false;
                if ((triggerMod & MOD_ALT) && ((GetAsyncKeyState(VK_MENU) & 0x8000) != 0)) {
                    modifierStillDown = true;
                } else if ((triggerMod & MOD_CONTROL) && ((GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0)) {
                    modifierStillDown = true;
                } else if ((triggerMod & MOD_WIN) && (((GetAsyncKeyState(VK_LWIN) & 0x8000) != 0) || ((GetAsyncKeyState(VK_RWIN) & 0x8000) != 0))) {
                    modifierStillDown = true;
                }

                if (!modifierStillDown) {
                    KillTimer(hWnd, TIMER_ALT_POLL);
                    Wh_Log(L"Modifier physically released (timer poll), committing switch!");
                    HideHud(true);
                }
            } else {
                KillTimer(hWnd, TIMER_ALT_POLL);
            }
            return 0;
        }
        return DefWindowProcW(hWnd, uMsg, wParam, lParam);
    case WM_LBUTTONDOWN: {
        POINT pt = { (LONG)LOWORD(lParam), (LONG)HIWORD(lParam) };
        for (size_t i = 0; i < g_appWindows.size(); ++i) {
            if (PtInRect(&g_appWindows[i].rcCard, pt)) {
                g_selectedIndex = static_cast<int>(i);
                HideHud(true);
                break;
            }
        }
        return 0;
    }
    case WM_USER_HUD_SHOW:
        ShowHud();
        return 0;
    case WM_USER_HUD_CYCLE:
        CycleSelection((int)wParam);
        return 0;
    case WM_USER_HUD_COMMIT:
        HideHud(true);
        return 0;
    case WM_USER_HUD_CANCEL:
        HideHud(false);
        return 0;
    case WM_USER_LAUNCH_APP: {
        size_t appIdx = (size_t)wParam;
        std::wstring cmd;
        AcquireSRWLockShared(&g_settingsLock);
        if (appIdx < g_settings.customApps.size()) {
            cmd = g_settings.customApps[appIdx].command;
        }
        ReleaseSRWLockShared(&g_settingsLock);
        if (!cmd.empty()) {
            LaunchApplication(cmd);
        }
        return 0;
    }
    case WM_USER_SWITCH_DESKTOP: {
        int desktopNum = static_cast<int>(wParam);
        SwitchToDesktopNumber(desktopNum);
        return 0;
    }
    case WM_USER_SETTINGS_CHANGED: {
        CleanupVirtualDesktopAPI();
        LoadModSettings();
        UpdateVirtualDesktopCount();
        InvalidateRect(hWnd, NULL, TRUE);
        return 0;
    }
    case WM_ERASEBKGND:
        return 1;
    default:
        return DefWindowProcW(hWnd, uMsg, wParam, lParam);
    }

    return 0;
}

// Check WOW64 execution environment (Windhawk 1.7.x runs 32-bit windhawk.exe)
static bool IsCurrentProcessWow64() {
    BOOL isWow64 = FALSE;
    typedef BOOL(WINAPI *LPFN_ISWOW64PROCESS)(HANDLE, PBOOL);
    LPFN_ISWOW64PROCESS fnIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress(
        GetModuleHandleW(L"kernel32.dll"), "IsWow64Process");
    if (fnIsWow64Process) {
        fnIsWow64Process(GetCurrentProcess(), &isWow64);
    }
    return isWow64 != FALSE;
}

// Retrieve user home directory for terminal launch working directory
static std::wstring GetUserHomeDirectory() {
    WCHAR userProfile[MAX_PATH] = {0};
    if (GetEnvironmentVariableW(L"USERPROFILE", userProfile, ARRAYSIZE(userProfile))) {
        return userProfile;
    }
    return L"";
}

// Launch application or command line cleanly
static void LaunchApplication(const std::wstring& commandLine) {
    if (commandLine.empty()) return;

    std::wstring execPath = commandLine;
    std::wstring execArgs;

    while (!execPath.empty() && (execPath.front() == L' ' || execPath.front() == L'\t')) execPath.erase(0, 1);
    while (!execPath.empty() && (execPath.back() == L' ' || execPath.back() == L'\t')) execPath.pop_back();

    if (execPath.find(L' ') != std::wstring::npos) {
        if (GetFileAttributesW(execPath.c_str()) == INVALID_FILE_ATTRIBUTES) {
            if (execPath.front() == L'\"') {
                size_t closeQuote = execPath.find(L'\"', 1);
                if (closeQuote != std::wstring::npos) {
                    std::wstring candidateExec = execPath.substr(1, closeQuote - 1);
                    std::wstring candidateArgs = execPath.substr(closeQuote + 1);
                    while (!candidateArgs.empty() && (candidateArgs.front() == L' ' || candidateArgs.front() == L'\t')) candidateArgs.erase(0, 1);
                    execPath = candidateExec;
                    execArgs = candidateArgs;
                }
            } else {
                size_t firstSpace = execPath.find(L' ');
                if (firstSpace != std::wstring::npos) {
                    std::wstring candidateExec = execPath.substr(0, firstSpace);
                    std::wstring candidateArgs = execPath.substr(firstSpace + 1);
                    while (!candidateArgs.empty() && (candidateArgs.front() == L' ' || candidateArgs.front() == L'\t')) candidateArgs.erase(0, 1);
                    execPath = candidateExec;
                    execArgs = candidateArgs;
                }
            }
        }
    }

    // Resolve system executables under WOW64 (32-bit windhawk.exe in Windhawk 1.7.x)
    bool isWow64 = IsCurrentProcessWow64();
    if (isWow64) {
        WCHAR winDir[MAX_PATH] = {0};
        if (GetWindowsDirectoryW(winDir, ARRAYSIZE(winDir))) {
            std::wstring sysnative = std::wstring(winDir) + L"\\Sysnative\\";
            std::wstring candidate = sysnative + execPath;
            if (GetFileAttributesW(candidate.c_str()) != INVALID_FILE_ATTRIBUTES) {
                execPath = candidate;
            } else if (candidate.length() < 4 || _wcsicmp(candidate.substr(candidate.length() - 4).c_str(), L".exe") != 0) {
                std::wstring candidateExe = candidate + L".exe";
                if (GetFileAttributesW(candidateExe.c_str()) != INVALID_FILE_ATTRIBUTES) {
                    execPath = candidateExe;
                }
            }
        }
    }

    std::wstring workingDir = GetUserHomeDirectory();
    LPCWSTR pDir = workingDir.empty() ? nullptr : workingDir.c_str();
    LPCWSTR pArgs = execArgs.empty() ? nullptr : execArgs.c_str();

    Wh_Log(L"Launching: '%s', args: '%s', dir: '%s'",
           execPath.c_str(), execArgs.c_str(), workingDir.c_str());

    HINSTANCE hRes = ShellExecuteW(NULL, L"open", execPath.c_str(), pArgs, pDir, SW_SHOWNORMAL);

    if ((INT_PTR)hRes <= 32) {
        if (_wcsicmp(execPath.c_str(), L"wt.exe") == 0 || _wcsicmp(execPath.c_str(), L"wt") == 0) {
            WCHAR localAppData[MAX_PATH] = {0};
            if (GetEnvironmentVariableW(L"LOCALAPPDATA", localAppData, ARRAYSIZE(localAppData))) {
                std::wstring fullWt = std::wstring(localAppData) + L"\\Microsoft\\WindowsApps\\wt.exe";
                ShellExecuteW(NULL, L"open", fullWt.c_str(), pArgs, pDir, SW_SHOWNORMAL);
            }
        } else if (_wcsicmp(execPath.c_str(), L"wsl.exe") == 0 || _wcsicmp(execPath.c_str(), L"wsl") == 0) {
            WCHAR winDir[MAX_PATH] = {0};
            if (GetWindowsDirectoryW(winDir, ARRAYSIZE(winDir))) {
                std::wstring fullWsl = std::wstring(winDir) + (isWow64 ? L"\\Sysnative\\wsl.exe" : L"\\System32\\wsl.exe");
                ShellExecuteW(NULL, L"open", fullWsl.c_str(), pArgs, pDir, SW_SHOWNORMAL);
            }
        }
    }
}

//=============================================================================
// Virtual Desktop API Implementation (Windows 10 & 11)
// Derived from Virtual Desktop Helper by u2x1
//=============================================================================

static const CLSID CLSID_ImmersiveShell_Val = {
    0xC2F03A33, 0x21F5, 0x47FA, {0xB4, 0xBB, 0x15, 0x63, 0x62, 0xA2, 0xF2, 0x39}};

static const CLSID CLSID_VirtualDesktopManagerInternal_Val = {
    0xC5E0CDCA, 0x7B6E, 0x41B2, {0x9F, 0xC4, 0xD9, 0x39, 0x75, 0xCC, 0x46, 0x7B}};

static const CLSID CLSID_VirtualDesktopManager_Val = {
    0xAA509086, 0x5CA9, 0x4C25, {0x8F, 0x95, 0x58, 0x9D, 0x3C, 0x07, 0xB4, 0x8A}};

static const IID IID_IVirtualDesktopManager_Val = {
    0xA5CD92DA, 0xDAC9, 0x4701, {0x83, 0x1E, 0x69, 0x52, 0x50, 0xDA, 0x0E, 0x58}};

struct VersionIIDs {
    IID managerInternal;
    IID virtualDesktop;
    bool usesHMonitor;
};

static const VersionIIDs g_versionIIDs[] = {
    // [0] Windows 10 (Build < 20348)
    {{0xF31574D6, 0xB682, 0x4CDC, {0xBD, 0x56, 0x18, 0x27, 0x86, 0x0A, 0xBE, 0xC6}},
     {0xFF72FFDD, 0xBE7E, 0x43FC, {0x9C, 0x03, 0xAD, 0x81, 0x68, 0x1E, 0x88, 0xE4}},
     false},

    // [1] Windows 10 (Build 20348 - 21999)
    {{0x094AFE11, 0x44F2, 0x4BA0, {0x97, 0x6F, 0x29, 0xA9, 0x7E, 0x26, 0x3E, 0xE0}},
     {0x62FDF88B, 0x11CA, 0x4AFB, {0x8B, 0xD8, 0x22, 0x96, 0xDF, 0xAE, 0x49, 0xE2}},
     true},

    // [2] Windows 11 (Build 22000 - 22482)
    {{0xB2F925B9, 0x5A0F, 0x4D2E, {0x9F, 0x4D, 0x2B, 0x15, 0x07, 0x59, 0x3C, 0x10}},
     {0x536D3495, 0xB208, 0x4CC9, {0xAE, 0x26, 0xDE, 0x81, 0x11, 0x27, 0x5B, 0xF8}},
     true},

    // [3] Windows 11 (Build 22621/22631/23H2)
    {{0xA3175F2D, 0x239C, 0x4BD2, {0x8A, 0xA0, 0xEE, 0xBA, 0x8B, 0x0B, 0x13, 0x8E}},
     {0x3F07F4BE, 0xB107, 0x441A, {0xAF, 0x0F, 0x39, 0xD8, 0x25, 0x29, 0x07, 0x2C}},
     false},

    // [4] Windows 11 (Build 26100+ / 24H2)
    {{0x53F5CA0B, 0x158F, 0x4124, {0x90, 0x0C, 0x05, 0x71, 0x58, 0x06, 0x0B, 0x27}},
     {0x3F07F4BE, 0xB107, 0x441A, {0xAF, 0x0F, 0x39, 0xD8, 0x25, 0x29, 0x07, 0x2C}},
     false},
};

static int g_windowsVersionIndex = 4;

struct IVirtualDesktop : public IUnknown {
    virtual HRESULT STDMETHODCALLTYPE IsViewVisible(IUnknown*, BOOL*) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetId(GUID*) = 0;
};

struct IVirtualDesktopManagerInternal : public IUnknown {};

static IServiceProvider* g_pServiceProvider = nullptr;
static IVirtualDesktopManagerInternal* g_pDesktopManagerInternal = nullptr;
static IVirtualDesktopManager* g_pDesktopManager = nullptr;
static bool g_bVDInitialized = false;

template <typename T>
static T GetVTableFunction(void* pInterface, int index) {
    return reinterpret_cast<T>((*reinterpret_cast<void***>(pInterface))[index]);
}

static const int VTABLE_GET_CURRENT_DESKTOP = 6;
static const int VTABLE_GET_DESKTOPS = 7;
static const int VTABLE_SWITCH_DESKTOP = 9;

static inline bool UsesHMonitorParameter() {
    return g_versionIIDs[g_windowsVersionIndex].usesHMonitor;
}

static void CleanupVirtualDesktopAPI() {
    SAFE_RELEASE(g_pDesktopManager);
    SAFE_RELEASE(g_pDesktopManagerInternal);
    SAFE_RELEASE(g_pServiceProvider);
    g_bVDInitialized = false;
}

static bool TryQueryDesktopManager(int versionIndex) {
    if (!g_pServiceProvider) return false;
    SAFE_RELEASE(g_pDesktopManagerInternal);

    HRESULT hr = g_pServiceProvider->QueryService(
        CLSID_VirtualDesktopManagerInternal_Val,
        g_versionIIDs[versionIndex].managerInternal,
        (void**)&g_pDesktopManagerInternal);
    return SUCCEEDED(hr) && g_pDesktopManagerInternal;
}

static bool InitializeVirtualDesktopAPI() {
    if (g_bVDInitialized && g_pDesktopManagerInternal) return true;

    HRESULT hr = CoCreateInstance(CLSID_ImmersiveShell_Val, nullptr, CLSCTX_LOCAL_SERVER, IID_IServiceProvider,
                                  (void**)&g_pServiceProvider);
    if (FAILED(hr) || !g_pServiceProvider) {
        Wh_Log(L"Failed to create ImmersiveShell: 0x%08X", hr);
        return false;
    }

    bool matched = false;
    AcquireSRWLockShared(&g_settingsLock);
    std::wstring verPref = g_settings.windowsVersion;
    ReleaseSRWLockShared(&g_settingsLock);

    if (verPref == L"auto") {
        if (TryQueryDesktopManager(g_windowsVersionIndex)) {
            matched = true;
        } else {
            for (int i = 4; i >= 0; --i) {
                if (i != g_windowsVersionIndex && TryQueryDesktopManager(i)) {
                    g_windowsVersionIndex = i;
                    matched = true;
                    Wh_Log(L"Auto-detected working VirtualDesktopManagerInternal interface at index %d", i);
                    break;
                }
            }
        }
    } else {
        matched = TryQueryDesktopManager(g_windowsVersionIndex);
    }

    if (!matched || !g_pDesktopManagerInternal) {
        Wh_Log(L"Failed to get VirtualDesktopManagerInternal");
        CleanupVirtualDesktopAPI();
        return false;
    }

    hr = CoCreateInstance(CLSID_VirtualDesktopManager_Val, nullptr, CLSCTX_INPROC_SERVER, IID_IVirtualDesktopManager_Val,
                          (void**)&g_pDesktopManager);
    if (FAILED(hr)) {
        Wh_Log(L"Failed to create VirtualDesktopManager: 0x%08X", hr);
    }

    g_bVDInitialized = true;
    Wh_Log(L"Virtual Desktop API initialized successfully (Version index: %d)", g_windowsVersionIndex);
    return true;
}

static bool ReinitializeVirtualDesktopAPI() {
    CleanupVirtualDesktopAPI();
    return InitializeVirtualDesktopAPI();
}

template <typename TResult>
static HRESULT CallManagerInternal(int vtableIndex, TResult* outResult) {
    if (!g_pDesktopManagerInternal && !InitializeVirtualDesktopAPI()) return E_FAIL;
    if (UsesHMonitorParameter()) {
        auto pfn = GetVTableFunction<HRESULT(STDMETHODCALLTYPE*)(void*, HMONITOR, TResult*)>(
            g_pDesktopManagerInternal, vtableIndex);
        HRESULT hr = pfn(g_pDesktopManagerInternal, nullptr, outResult);
        if (FAILED(hr) && ReinitializeVirtualDesktopAPI()) {
            pfn = GetVTableFunction<HRESULT(STDMETHODCALLTYPE*)(void*, HMONITOR, TResult*)>(
                g_pDesktopManagerInternal, vtableIndex);
            hr = pfn(g_pDesktopManagerInternal, nullptr, outResult);
        }
        return hr;
    } else {
        auto pfn = GetVTableFunction<HRESULT(STDMETHODCALLTYPE*)(void*, TResult*)>(
            g_pDesktopManagerInternal, vtableIndex);
        HRESULT hr = pfn(g_pDesktopManagerInternal, outResult);
        if (FAILED(hr) && ReinitializeVirtualDesktopAPI()) {
            pfn = GetVTableFunction<HRESULT(STDMETHODCALLTYPE*)(void*, TResult*)>(
                g_pDesktopManagerInternal, vtableIndex);
            hr = pfn(g_pDesktopManagerInternal, outResult);
        }
        return hr;
    }
}

template <typename TArg>
static HRESULT CallManagerInternalWithArg(int vtableIndex, TArg arg) {
    if (!g_pDesktopManagerInternal && !InitializeVirtualDesktopAPI()) return E_FAIL;
    if (UsesHMonitorParameter()) {
        auto pfn = GetVTableFunction<HRESULT(STDMETHODCALLTYPE*)(void*, HMONITOR, TArg)>(
            g_pDesktopManagerInternal, vtableIndex);
        HRESULT hr = pfn(g_pDesktopManagerInternal, nullptr, arg);
        if (FAILED(hr) && ReinitializeVirtualDesktopAPI()) {
            pfn = GetVTableFunction<HRESULT(STDMETHODCALLTYPE*)(void*, HMONITOR, TArg)>(
                g_pDesktopManagerInternal, vtableIndex);
            hr = pfn(g_pDesktopManagerInternal, nullptr, arg);
        }
        return hr;
    } else {
        auto pfn = GetVTableFunction<HRESULT(STDMETHODCALLTYPE*)(void*, TArg)>(
            g_pDesktopManagerInternal, vtableIndex);
        HRESULT hr = pfn(g_pDesktopManagerInternal, arg);
        if (FAILED(hr) && ReinitializeVirtualDesktopAPI()) {
            pfn = GetVTableFunction<HRESULT(STDMETHODCALLTYPE*)(void*, TArg)>(
                g_pDesktopManagerInternal, vtableIndex);
            hr = pfn(g_pDesktopManagerInternal, arg);
        }
        return hr;
    }
}

static IObjectArray* GetDesktops() {
    if (!g_pDesktopManagerInternal) return nullptr;
    IObjectArray* desktops = nullptr;
    HRESULT hr = CallManagerInternal(VTABLE_GET_DESKTOPS, &desktops);
    if (FAILED(hr)) {
        Wh_Log(L"GetDesktops failed: 0x%08X", hr);
        return nullptr;
    }
    return desktops;
}

static bool GetCurrentDesktopId(GUID* outGuid) {
    if (!g_pDesktopManagerInternal) return false;
    IVirtualDesktop* desktop = nullptr;
    HRESULT hr = CallManagerInternal(VTABLE_GET_CURRENT_DESKTOP, &desktop);
    if (FAILED(hr) || !desktop) {
        Wh_Log(L"GetCurrentDesktop failed: 0x%08X", hr);
        return false;
    }
    hr = desktop->GetId(outGuid);
    desktop->Release();
    return SUCCEEDED(hr);
}

static int GetCurrentDesktopIndex() {
    GUID currentId = {};
    if (!GetCurrentDesktopId(&currentId)) return -1;
    IObjectArray* desktops = GetDesktops();
    if (!desktops) return -1;
    UINT count = 0;
    desktops->GetCount(&count);
    int foundIdx = -1;
    for (UINT i = 0; i < count; ++i) {
        IVirtualDesktop* desktop = nullptr;
        if (SUCCEEDED(desktops->GetAt(i, g_versionIIDs[g_windowsVersionIndex].virtualDesktop, (void**)&desktop)) && desktop) {
            GUID guid = {};
            bool match = SUCCEEDED(desktop->GetId(&guid)) && IsEqualGUID(guid, currentId);
            desktop->Release();
            if (match) {
                foundIdx = static_cast<int>(i);
                break;
            }
        }
    }
    desktops->Release();
    return foundIdx;
}

static bool SwitchToDesktop(IVirtualDesktop* desktop) {
    if (!g_pDesktopManagerInternal || !desktop) return false;
    HRESULT hr = CallManagerInternalWithArg(VTABLE_SWITCH_DESKTOP, desktop);
    if (FAILED(hr)) {
        Wh_Log(L"SwitchToDesktop COM call failed: 0x%08X", hr);
        return false;
    }
    return true;
}

static bool IsEligibleFocusWindow(HWND hwnd) {
    if (!hwnd || !IsWindow(hwnd)) return false;
    LONG_PTR style = GetWindowLongPtr(hwnd, GWL_STYLE);
    if (!(style & WS_VISIBLE) || (style & WS_CHILD)) return false;
    if (GetWindowLongPtr(hwnd, GWL_EXSTYLE) & WS_EX_TOOLWINDOW) return false;
    return GetAncestor(hwnd, GA_ROOTOWNER) == hwnd;
}

static HWND FindTopWindowOnDesktop(const GUID& desktopId) {
    if (!g_pDesktopManager) return nullptr;
    struct EnumContext {
        GUID targetId;
        HWND resultHwnd;
    } context = { desktopId, nullptr };

    EnumWindows([](HWND hwnd, LPARAM lParam) WINAPI -> BOOL {
        auto* ctx = reinterpret_cast<EnumContext*>(lParam);
        if (!IsEligibleFocusWindow(hwnd)) return TRUE;

        GUID winDesktopId = {};
        if (SUCCEEDED(g_pDesktopManager->GetWindowDesktopId(hwnd, &winDesktopId))) {
            if (IsEqualGUID(winDesktopId, ctx->targetId)) {
                ctx->resultHwnd = hwnd;
                return FALSE;
            }
        }
        return TRUE;
    }, reinterpret_cast<LPARAM>(&context));

    return context.resultHwnd;
}

static void FocusWindow(HWND hwnd) {
    if (!hwnd || !IsWindow(hwnd)) return;
    if (IsIconic(hwnd)) ShowWindow(hwnd, SW_RESTORE);
    SetForegroundWindow(hwnd);
}

typedef LONG(NTAPI* RtlGetVersionPtr)(PRTL_OSVERSIONINFOW);

static int DetectWindowsVersionIndex() {
    HMODULE hNtdll = GetModuleHandleW(L"ntdll.dll");
    if (hNtdll) {
        RtlGetVersionPtr pRtlGetVersion = (RtlGetVersionPtr)GetProcAddress(hNtdll, "RtlGetVersion");
        if (pRtlGetVersion) {
            RTL_OSVERSIONINFOW rovi = { sizeof(rovi) };
            if (pRtlGetVersion(&rovi) == 0) {
                DWORD build = rovi.dwBuildNumber;
                Wh_Log(L"Windows Build detected: %lu", build);
                if (build >= 26100) return 4;
                if (build >= 22621) return 3;
                if (build >= 22000) return 2;
                if (build >= 20348) return 1;
                return 0;
            }
        }
    }
    return 4;
}

static UINT UpdateVirtualDesktopCount() {
    bool enabled = false;
    AcquireSRWLockShared(&g_settingsLock);
    enabled = g_settings.enableDirectSwitching;
    ReleaseSRWLockShared(&g_settingsLock);

    if (!enabled) {
        g_desktopCount.store(0);
        return 0;
    }

    if (!InitializeVirtualDesktopAPI()) {
        g_desktopCount.store(0);
        return 0;
    }

    IObjectArray* desktops = GetDesktops();
    if (!desktops) {
        g_desktopCount.store(0);
        return 0;
    }

    UINT count = 0;
    desktops->GetCount(&count);
    desktops->Release();
    g_desktopCount.store(count);
    Wh_Log(L"Virtual desktop count updated: %u", count);
    return count;
}

static bool SwitchToDesktopNumber(int desktopNum) {
    if (desktopNum < 1 || desktopNum > 9) return false;
    if (!InitializeVirtualDesktopAPI()) {
        Wh_Log(L"SwitchToDesktopNumber: Failed to initialize Virtual Desktop API");
        return false;
    }

    IObjectArray* desktops = GetDesktops();
    if (!desktops) {
        Wh_Log(L"SwitchToDesktopNumber: Failed to get virtual desktops");
        return false;
    }

    UINT count = 0;
    desktops->GetCount(&count);
    g_desktopCount.store(count);

    if ((UINT)(desktopNum - 1) >= count) {
        Wh_Log(L"Desktop %d requested, but only %u virtual desktops exist", desktopNum, count);
        desktops->Release();
        return false;
    }

    IVirtualDesktop* targetDesktop = nullptr;
    HRESULT hr = desktops->GetAt(desktopNum - 1, g_versionIIDs[g_windowsVersionIndex].virtualDesktop, (void**)&targetDesktop);
    desktops->Release();

    if (FAILED(hr) || !targetDesktop) {
        Wh_Log(L"Failed to get desktop %d: 0x%08X", desktopNum, hr);
        return false;
    }

    GUID currentId = {};
    GUID targetId = {};
    if (GetCurrentDesktopId(&currentId) && SUCCEEDED(targetDesktop->GetId(&targetId))) {
        if (IsEqualGUID(currentId, targetId)) {
            Wh_Log(L"Already on Desktop %d", desktopNum);
            targetDesktop->Release();
            return true;
        }
    }

    HWND windowToFocus = nullptr;
    if (SUCCEEDED(targetDesktop->GetId(&targetId))) {
        windowToFocus = FindTopWindowOnDesktop(targetId);
    }

    bool success = SwitchToDesktop(targetDesktop);
    targetDesktop->Release();

    if (success) {
        Wh_Log(L"Successfully switched to Virtual Desktop %d", desktopNum);
        if (windowToFocus) {
            FocusWindow(windowToFocus);
        }
    } else {
        Wh_Log(L"SwitchToDesktop failed for Desktop %d, trying keyboard fallback...", desktopNum);
        int curIdx = GetCurrentDesktopIndex();
        if (curIdx >= 0) {
            int targetIdx = desktopNum - 1;
            int diff = targetIdx - curIdx;
            if (diff != 0) {
                WORD vkDir = (diff > 0) ? VK_RIGHT : VK_LEFT;
                int steps = abs(diff);
                Wh_Log(L"Keyboard fallback: %d steps (%s)",
                       steps, (vkDir == VK_RIGHT) ? L"RIGHT" : L"LEFT");
                for (int i = 0; i < steps; ++i) {
                    keybd_event(VK_CONTROL, 0, 0, 0);
                    keybd_event(VK_LWIN, 0, 0, 0);
                    keybd_event(vkDir, 0, 0, 0);
                    keybd_event(vkDir, 0, KEYEVENTF_KEYUP, 0);
                    keybd_event(VK_LWIN, 0, KEYEVENTF_KEYUP, 0);
                    keybd_event(VK_CONTROL, 0, KEYEVENTF_KEYUP, 0);
                    Sleep(30);
                }
                success = true;
            }
        }
    }

    return success;
}

// Low-level keyboard hook callback: minimal, non-blocking input interception
static LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode != HC_ACTION) {
        return CallNextHookEx(g_hKeyboardHook, nCode, wParam, lParam);
    }

    KBDLLHOOKSTRUCT* pKey = (KBDLLHOOKSTRUCT*)lParam;
    bool isKeyDown = (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN);
    bool isKeyUp = (wParam == WM_KEYUP || wParam == WM_SYSKEYUP);

    UINT activeModifiers = 0;
    if ((GetAsyncKeyState(VK_MENU) & 0x8000) != 0) activeModifiers |= MOD_ALT;
    if ((GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0) activeModifiers |= MOD_CONTROL;
    if ((GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0) activeModifiers |= MOD_SHIFT;
    if (((GetAsyncKeyState(VK_LWIN) & 0x8000) != 0) || ((GetAsyncKeyState(VK_RWIN) & 0x8000) != 0)) activeModifiers |= MOD_WIN;

    // Track modifier release to commit HUD selection without swallowing key-up
    if (isKeyUp && g_hudVisible.load()) {
        UINT triggerMod = g_hudTriggerModifier.load();
        bool modifierReleased = false;
        if ((triggerMod & MOD_ALT) && (pKey->vkCode == VK_LMENU || pKey->vkCode == VK_RMENU || pKey->vkCode == VK_MENU)) {
            modifierReleased = true;
        } else if ((triggerMod & MOD_CONTROL) && (pKey->vkCode == VK_LCONTROL || pKey->vkCode == VK_RCONTROL || pKey->vkCode == VK_CONTROL)) {
            modifierReleased = true;
        } else if ((triggerMod & MOD_WIN) && (pKey->vkCode == VK_LWIN || pKey->vkCode == VK_RWIN)) {
            modifierReleased = true;
        }

        if (modifierReleased) {
            Wh_Log(L"Modifier released, posting commit switch.");
            if (g_hHudWnd) {
                PostMessageW(g_hHudWnd, WM_USER_HUD_COMMIT, 0, 0);
            }
            return CallNextHookEx(g_hKeyboardHook, nCode, wParam, lParam);
        }
    }

    // While HUD is visible, handle navigation and cancellation
    if (g_hudVisible.load() && isKeyDown) {
        if (pKey->vkCode == VK_ESCAPE) {
            Wh_Log(L"ESC pressed, cancelling switch.");
            if (g_hHudWnd) PostMessageW(g_hHudWnd, WM_USER_HUD_CANCEL, 0, 0);
            return 1;
        }
        if (pKey->vkCode == VK_RETURN || pKey->vkCode == VK_SPACE) {
            Wh_Log(L"Enter/Space pressed, committing switch.");
            if (g_hHudWnd) PostMessageW(g_hHudWnd, WM_USER_HUD_COMMIT, 0, 0);
            return 1;
        }
        if (pKey->vkCode == VK_UP || pKey->vkCode == VK_LEFT) {
            if (g_hHudWnd) PostMessageW(g_hHudWnd, WM_USER_HUD_CYCLE, (WPARAM)-1, 0);
            return 1;
        }
        if (pKey->vkCode == VK_DOWN || pKey->vkCode == VK_RIGHT) {
            if (g_hHudWnd) PostMessageW(g_hHudWnd, WM_USER_HUD_CYCLE, (WPARAM)1, 0);
            return 1;
        }
        if (pKey->vkCode == VK_TAB) {
            bool isShift = (activeModifiers & MOD_SHIFT) != 0;
            if (g_hHudWnd) PostMessageW(g_hHudWnd, WM_USER_HUD_CYCLE, (WPARAM)(isShift ? -1 : 1), 0);
            return 1;
        }
    }

    AcquireSRWLockShared(&g_settingsLock);

    // 1. Same-App Window Switcher
    if (isKeyDown && g_settings.bindingSameApp.valid) {
        UINT modsWithoutShift = activeModifiers & ~MOD_SHIFT;
        UINT targetModsWithoutShift = g_settings.bindingSameApp.modifiers & ~MOD_SHIFT;

        if (pKey->vkCode == g_settings.bindingSameApp.vk && modsWithoutShift == targetModsWithoutShift) {
            Wh_Log(L"Triggered Same-App Switcher! (%s)", g_settings.bindingSameApp.originalStr.c_str());
            keybd_event(0xFF, 0, KEYEVENTF_KEYUP, 0); // Mask held modifier to avoid menu focus

            UINT triggerMod = g_settings.bindingSameApp.modifiers ? g_settings.bindingSameApp.modifiers : MOD_ALT;
            g_hudTriggerModifier.store(triggerMod);

            if (!g_hudVisible.load()) {
                if (g_hHudWnd) PostMessageW(g_hHudWnd, WM_USER_HUD_SHOW, 0, 0);
            } else {
                bool isShift = (activeModifiers & MOD_SHIFT) != 0;
                if (g_hHudWnd) PostMessageW(g_hHudWnd, WM_USER_HUD_CYCLE, (WPARAM)(isShift ? -1 : 1), 0);
            }
            ReleaseSRWLockShared(&g_settingsLock);
            return 1;
        }
    }

    // 2. Custom Application Launchers
    if (isKeyDown) {
        for (size_t i = 0; i < g_settings.customApps.size(); ++i) {
            if (g_settings.customApps[i].binding.Matches(activeModifiers, pKey->vkCode)) {
                Wh_Log(L"Triggered Custom App Launcher! '%s' -> '%s'",
                       g_settings.customApps[i].binding.originalStr.c_str(),
                       g_settings.customApps[i].command.c_str());
                if (activeModifiers & MOD_WIN) {
                    keybd_event(0xFF, 0, KEYEVENTF_KEYUP, 0); // Suppress Start Menu
                }
                if (g_hHudWnd) {
                    PostMessageW(g_hHudWnd, WM_USER_LAUNCH_APP, (WPARAM)i, 0);
                }
                ReleaseSRWLockShared(&g_settingsLock);
                return 1;
            }
        }
    }

    // 3. Virtual Desktop Direct Switching: <modifier> + 1..9
    if (g_settings.enableDirectSwitching && (pKey->vkCode >= '1' && pKey->vkCode <= '9')) {
        UINT cleanMods = activeModifiers & (MOD_ALT | MOD_CONTROL | MOD_SHIFT | MOD_WIN);
        if (cleanMods == g_settings.desktopModifier) {
            int targetDesktop = pKey->vkCode - '0';
            UINT maxDesktops = g_desktopCount.load();

            // Only swallow if desktop exists or if desktop count is uninitialized
            if (maxDesktops == 0 || (UINT)targetDesktop <= maxDesktops) {
                if (isKeyDown) {
                    Wh_Log(L"Triggered Virtual Desktop switch: Desktop %d", targetDesktop);
                    if (activeModifiers & MOD_WIN) {
                        keybd_event(0xFF, 0, KEYEVENTF_KEYUP, 0); // Suppress Start Menu
                    }
                    if (g_hHudWnd) {
                        PostMessageW(g_hHudWnd, WM_USER_SWITCH_DESKTOP, (WPARAM)targetDesktop, 0);
                    }
                }
                ReleaseSRWLockShared(&g_settingsLock);
                return 1;
            }
        }
    }

    ReleaseSRWLockShared(&g_settingsLock);
    return CallNextHookEx(g_hKeyboardHook, nCode, wParam, lParam);
}

// Dedicated background hook thread running WH_KEYBOARD_LL message pump
static DWORD WINAPI HookThreadProc(LPVOID lpParam) {
    Wh_Log(L"Hook thread started (ID: %lu)", GetCurrentThreadId());

    MSG msg;
    PeekMessageW(&msg, nullptr, WM_USER, WM_USER, PM_NOREMOVE);

    g_hKeyboardHook = SetWindowsHookExW(
        WH_KEYBOARD_LL,
        LowLevelKeyboardProc,
        GetModuleHandle(NULL),
        0
    );

    if (!g_hKeyboardHook) {
        g_hKeyboardHook = SetWindowsHookExW(
            WH_KEYBOARD_LL,
            LowLevelKeyboardProc,
            NULL,
            0
        );
    }

    if (!g_hKeyboardHook) {
        Wh_Log(L"ERROR: Failed to install WH_KEYBOARD_LL hook (Error %lu)", GetLastError());
        return 1;
    }

    Wh_Log(L"SUCCESS: WH_KEYBOARD_LL hook installed and listening to keyboard events!");
    SetEvent(g_hHookReady);

    while (GetMessageW(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    Wh_Log(L"Hook thread message loop terminating...");

    if (g_hKeyboardHook) {
        UnhookWindowsHookEx(g_hKeyboardHook);
        g_hKeyboardHook = NULL;
    }

    return 0;
}

// Dedicated worker thread owning HUD window, COM objects, ShellExecute
static DWORD WINAPI WorkerThreadProc(LPVOID lpParam) {
    Wh_Log(L"Worker thread started (ID: %lu)", GetCurrentThreadId());

    MSG msg;
    PeekMessageW(&msg, nullptr, WM_USER, WM_USER, PM_NOREMOVE);

    HRESULT hrCo = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
    if (FAILED(hrCo)) {
        Wh_Log(L"CoInitializeEx returned: 0x%08X", hrCo);
    }

    WNDCLASSEXW wc = { sizeof(wc) };
    wc.lpfnWndProc = HudWndProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = L"WindhawkCustomShortcutsHud";
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);

    if (!RegisterClassExW(&wc) && GetLastError() != ERROR_CLASS_ALREADY_EXISTS) {
        Wh_Log(L"ERROR: RegisterClassExW failed (Error %lu)", GetLastError());
        CoUninitialize();
        return 1;
    }

    g_hHudWnd = CreateWindowExW(
        WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE | WS_EX_LAYERED,
        wc.lpszClassName,
        L"CustomShortcutsHUD",
        WS_POPUP,
        0, 0, 460, 200,
        NULL, NULL, wc.hInstance, NULL
    );

    if (!g_hHudWnd) {
        Wh_Log(L"ERROR: CreateWindowExW failed (Error %lu)", GetLastError());
        UnregisterClassW(wc.lpszClassName, wc.hInstance);
        CoUninitialize();
        return 1;
    }

    SetLayeredWindowAttributes(g_hHudWnd, 0, 246, LWA_ALPHA);

    LoadModSettings();
    UpdateVirtualDesktopCount();

    SetEvent(g_hWorkerReady);

    while (GetMessageW(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    Wh_Log(L"Worker thread message loop terminating...");

    if (g_hHudWnd) {
        DestroyWindow(g_hHudWnd);
        g_hHudWnd = NULL;
    }

    UnregisterClassW(L"WindhawkCustomShortcutsHud", GetModuleHandle(NULL));
    CleanupVirtualDesktopAPI();
    CoUninitialize();
    return 0;
}

// Load and parse mod settings cleanly without memory leaks
static void LoadModSettings() {
    AcquireSRWLockExclusive(&g_settingsLock);

    WindhawkUtils::StringSetting sameAppStr =
        WindhawkUtils::StringSetting::make(L"shortcuts.hotkeySameApp");
    ParseHotkeyString(sameAppStr.get(), g_settings.bindingSameApp);

    g_settings.showHud = Wh_GetIntSetting(L"hudOptions.showHud") != 0;

    WindhawkUtils::StringSetting theme =
        WindhawkUtils::StringSetting::make(L"hudOptions.hudTheme");
    g_settings.hudTheme = *theme.get() ? theme.get() : L"system";

    g_settings.enableDirectSwitching = Wh_GetIntSetting(L"virtualDesktops.enableDirectSwitching") != 0;

    WindhawkUtils::StringSetting modStr =
        WindhawkUtils::StringSetting::make(L"virtualDesktops.desktopModifier");
    if (*modStr.get()) {
        g_settings.desktopModifier = ParseModifierOnlyString(modStr.get());
    } else {
        g_settings.desktopModifier = MOD_ALT;
    }

    WindhawkUtils::StringSetting verStr =
        WindhawkUtils::StringSetting::make(L"virtualDesktops.windowsVersion");
    g_settings.windowsVersion = *verStr.get() ? verStr.get() : L"auto";
    if (g_settings.windowsVersion == L"win10_old") {
        g_windowsVersionIndex = 0;
    } else if (g_settings.windowsVersion == L"win10_20348") {
        g_windowsVersionIndex = 1;
    } else if (g_settings.windowsVersion == L"win11_22000") {
        g_windowsVersionIndex = 2;
    } else if (g_settings.windowsVersion == L"win11_22621") {
        g_windowsVersionIndex = 3;
    } else if (g_settings.windowsVersion == L"win11_26100") {
        g_windowsVersionIndex = 4;
    } else {
        g_windowsVersionIndex = DetectWindowsVersionIndex();
    }

    // Load custom application shortcuts
    g_settings.customApps.clear();
    for (int i = 1; i <= 3; ++i) {
        auto hotkeyStr = WindhawkUtils::StringSetting::make(L"shortcuts.hotkeyCustomApp%d", i);
        auto pathStr = WindhawkUtils::StringSetting::make(L"customAppOptions.customAppPath%d", i);

        if (*hotkeyStr.get() && *pathStr.get()) {
            CustomAppShortcut app;
            ParseHotkeyString(hotkeyStr.get(), app.binding);
            app.command = pathStr.get();

            if (app.binding.valid && !app.command.empty()) {
                g_settings.customApps.push_back(app);
                Wh_Log(L"Custom App %d: '%s' -> '%s'",
                       i, app.binding.originalStr.c_str(), app.command.c_str());
            }
        }
    }

    ReleaseSRWLockExclusive(&g_settingsLock);

    Wh_Log(L"Settings loaded:");
    Wh_Log(L"  Same-App: '%s' (valid=%d, vk=0x%02X, mods=0x%X)",
           g_settings.bindingSameApp.originalStr.c_str(), g_settings.bindingSameApp.valid,
           g_settings.bindingSameApp.vk, g_settings.bindingSameApp.modifiers);
    Wh_Log(L"  Virtual Desktops: enabled=%d, mod=0x%X, versionIndex=%d",
           g_settings.enableDirectSwitching, g_settings.desktopModifier, g_windowsVersionIndex);
}

// Tool mod initialization (runs in dedicated windhawk.exe tool process)
BOOL WhTool_ModInit() {
    Wh_Log(L"WhTool_ModInit called");

    HMODULE hUser32 = GetModuleHandleW(L"user32.dll");
    if (hUser32) {
        pfnSwitchToThisWindow = (SwitchToThisWindow_t)GetProcAddress(hUser32, "SwitchToThisWindow");
    }

    g_hWorkerReady = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_hWorkerReady) {
        Wh_Log(L"ERROR: CreateEvent for worker failed (%lu)", GetLastError());
        return FALSE;
    }

    g_hHookReady = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_hHookReady) {
        Wh_Log(L"ERROR: CreateEvent for hook failed (%lu)", GetLastError());
        CloseHandle(g_hWorkerReady);
        g_hWorkerReady = NULL;
        return FALSE;
    }

    g_hWorkerThread = CreateThread(nullptr, 0, WorkerThreadProc, nullptr, 0, &g_dwWorkerThreadId);
    if (!g_hWorkerThread) {
        Wh_Log(L"ERROR: CreateThread for worker failed (%lu)", GetLastError());
        CloseHandle(g_hWorkerReady);
        CloseHandle(g_hHookReady);
        g_hWorkerReady = NULL;
        g_hHookReady = NULL;
        return FALSE;
    }

    HANDLE workerInitHandles[] = {g_hWorkerReady, g_hWorkerThread};
    DWORD workerWaitRes = WaitForMultipleObjects(2, workerInitHandles, FALSE, INFINITE);
    if (workerWaitRes != WAIT_OBJECT_0) {
        Wh_Log(L"ERROR: Worker thread exited before readiness");
        return FALSE;
    }

    g_hHookThread = CreateThread(nullptr, 0, HookThreadProc, nullptr, 0, &g_dwHookThreadId);
    if (!g_hHookThread) {
        Wh_Log(L"ERROR: CreateThread for hook failed (%lu)", GetLastError());
        return FALSE;
    }

    HANDLE hookInitHandles[] = {g_hHookReady, g_hHookThread};
    DWORD hookWaitRes = WaitForMultipleObjects(2, hookInitHandles, FALSE, INFINITE);
    if (hookWaitRes != WAIT_OBJECT_0) {
        Wh_Log(L"ERROR: Hook thread exited before readiness");
        return FALSE;
    }

    Wh_Log(L"Custom Shortcuts tool mod initialized successfully.");
    return TRUE;
}

// Tool mod termination
void WhTool_ModUninit() {
    Wh_Log(L"WhTool_ModUninit called");

    if (g_dwHookThreadId) {
        if (g_hHookReady && g_hHookThread) {
            HANDLE handles[] = {g_hHookReady, g_hHookThread};
            WaitForMultipleObjects(2, handles, FALSE, INFINITE);
        }
        PostThreadMessageW(g_dwHookThreadId, WM_QUIT, 0, 0);
        if (g_hHookThread) {
            WaitForSingleObject(g_hHookThread, INFINITE);
            CloseHandle(g_hHookThread);
            g_hHookThread = NULL;
        }
        g_dwHookThreadId = 0;
    }

    if (g_dwWorkerThreadId) {
        if (g_hWorkerReady && g_hWorkerThread) {
            HANDLE handles[] = {g_hWorkerReady, g_hWorkerThread};
            WaitForMultipleObjects(2, handles, FALSE, INFINITE);
        }
        if (g_hHudWnd) {
            PostMessageW(g_hHudWnd, WM_QUIT, 0, 0);
        } else {
            PostThreadMessageW(g_dwWorkerThreadId, WM_QUIT, 0, 0);
        }
        if (g_hWorkerThread) {
            WaitForSingleObject(g_hWorkerThread, INFINITE);
            CloseHandle(g_hWorkerThread);
            g_hWorkerThread = NULL;
        }
        g_dwWorkerThreadId = 0;
    }

    if (g_hHookReady) {
        CloseHandle(g_hHookReady);
        g_hHookReady = NULL;
    }
    if (g_hWorkerReady) {
        CloseHandle(g_hWorkerReady);
        g_hWorkerReady = NULL;
    }

    Wh_Log(L"Cleanup complete.");
}

// Tool mod settings changed notification
void WhTool_ModSettingsChanged() {
    Wh_Log(L"WhTool_ModSettingsChanged called");
    if (g_hHudWnd) {
        PostMessageW(g_hHudWnd, WM_USER_SETTINGS_CHANGED, 0, 0);
    }
}

////////////////////////////////////////////////////////////////////////////////
// Windhawk tool mod implementation for mods which don't need to inject to other
// processes or hook other functions. Context:
// https://github.com/ramensoftware/windhawk/wiki/Mods-as-tools:-Running-mods-in-a-dedicated-process
//
// The mod will load and run in a dedicated windhawk.exe process.
//
// Paste the code below as part of the mod code, and use these callbacks:
// * WhTool_ModInit
// * WhTool_ModSettingsChanged
// * WhTool_ModUninit
//
// Currently, other callbacks are not supported.

bool g_isToolModProcessLauncher;
HANDLE g_toolModProcessMutex;

void WINAPI EntryPoint_Hook() {
    Wh_Log(L">");
    ExitThread(0);
}

BOOL Wh_ModInit() {
    bool isExcluded = false;
    bool isToolModProcess = false;
    bool isCurrentToolModProcess = false;
    int argc;
    LPWSTR *argv = CommandLineToArgvW(GetCommandLine(), &argc);
    if (!argv) {
        Wh_Log(L"CommandLineToArgvW failed");
        return FALSE;
    }

    for (int i = 1; i < argc; i++) {
        if (wcscmp(argv[i], L"-service") == 0 ||
            wcscmp(argv[i], L"-service-start") == 0 ||
            wcscmp(argv[i], L"-service-stop") == 0) {
            isExcluded = true;
            break;
        }
    }

    for (int i = 1; i < argc - 1; i++) {
        if (wcscmp(argv[i], L"-tool-mod") == 0) {
            isToolModProcess = true;
            if (wcscmp(argv[i + 1], WH_MOD_ID) == 0) {
                isCurrentToolModProcess = true;
            }
            break;
        }
    }

    LocalFree(argv);

    if (isExcluded) {
        return FALSE;
    }

    if (isCurrentToolModProcess) {
        g_toolModProcessMutex =
            CreateMutex(nullptr, TRUE, L"windhawk-tool-mod_" WH_MOD_ID);
        if (!g_toolModProcessMutex) {
            Wh_Log(L"CreateMutex failed");
            ExitProcess(1);
        }

        if (GetLastError() == ERROR_ALREADY_EXISTS) {
            Wh_Log(L"Tool mod already running (%s)", WH_MOD_ID);
            ExitProcess(1);
        }

        if (!WhTool_ModInit()) {
            ExitProcess(1);
        }

        IMAGE_DOS_HEADER *dosHeader = (IMAGE_DOS_HEADER *)GetModuleHandle(nullptr);
        IMAGE_NT_HEADERS *ntHeaders =
            (IMAGE_NT_HEADERS *)((BYTE *)dosHeader + dosHeader->e_lfanew);

        DWORD entryPointRVA = ntHeaders->OptionalHeader.AddressOfEntryPoint;
        void *entryPoint = (BYTE *)dosHeader + entryPointRVA;

        Wh_SetFunctionHook(entryPoint, (void *)EntryPoint_Hook, nullptr);
        return TRUE;
    }

    if (isToolModProcess) {
        return FALSE;
    }

    g_isToolModProcessLauncher = true;
    return TRUE;
}

void Wh_ModAfterInit() {
    if (!g_isToolModProcessLauncher) {
        return;
    }

    WCHAR currentProcessPath[MAX_PATH];
    switch (GetModuleFileName(nullptr, currentProcessPath,
                              ARRAYSIZE(currentProcessPath))) {
    case 0:
    case ARRAYSIZE(currentProcessPath):
        Wh_Log(L"GetModuleFileName failed");
        return;
    }

    WCHAR commandLine[MAX_PATH + 2 +
                      (sizeof(L" -tool-mod \"" WH_MOD_ID "\"") / sizeof(WCHAR)) - 1];
    swprintf_s(commandLine, L"\"%s\" -tool-mod \"%s\"", currentProcessPath,
               WH_MOD_ID);

    HMODULE kernelModule = GetModuleHandle(L"kernelbase.dll");
    if (!kernelModule) {
        kernelModule = GetModuleHandle(L"kernel32.dll");
        if (!kernelModule) {
            Wh_Log(L"No kernelbase.dll/kernel32.dll");
            return;
        }
    }

    using CreateProcessInternalW_t = BOOL(WINAPI *)(
        HANDLE hUserToken, LPCWSTR lpApplicationName, LPWSTR lpCommandLine,
        LPSECURITY_ATTRIBUTES lpProcessAttributes,
        LPSECURITY_ATTRIBUTES lpThreadAttributes, WINBOOL bInheritHandles,
        DWORD dwCreationFlags, LPVOID lpEnvironment, LPCWSTR lpCurrentDirectory,
        LPSTARTUPINFOW lpStartupInfo, LPPROCESS_INFORMATION lpProcessInformation,
        PHANDLE hRestrictedUserToken);
    CreateProcessInternalW_t pCreateProcessInternalW =
        (CreateProcessInternalW_t)GetProcAddress(kernelModule,
                                                 "CreateProcessInternalW");
    if (!pCreateProcessInternalW) {
        Wh_Log(L"No CreateProcessInternalW");
        return;
    }

    STARTUPINFO si{
        .cb = sizeof(STARTUPINFO),
        .dwFlags = STARTF_FORCEOFFFEEDBACK,
    };
    PROCESS_INFORMATION pi;
    if (!pCreateProcessInternalW(nullptr, currentProcessPath, commandLine,
                                 nullptr, nullptr, FALSE, NORMAL_PRIORITY_CLASS,
                                 nullptr, nullptr, &si, &pi, nullptr)) {
        Wh_Log(L"CreateProcess failed");
        return;
    }

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}

void Wh_ModSettingsChanged() {
    if (g_isToolModProcessLauncher) {
        return;
    }

    WhTool_ModSettingsChanged();
}

void Wh_ModUninit() {
    if (g_isToolModProcessLauncher) {
        return;
    }

    WhTool_ModUninit();
    ExitProcess(0);
}
