// ==WindhawkMod==
// @id              taskbar-auto-hide-keyboard-only
// @name            Taskbar auto-hide fine tuning
// @description     Fine-tune taskbar auto-hide: keyboard-only unhide, prevent the taskbar from showing at all, hotkeys and mouse events to show or toggle visibility
// @version         2.2
// @author          m417z
// @github          https://github.com/m417z
// @twitter         https://twitter.com/m417z
// @homepage        https://m417z.com/
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lcomctl32 -ldwmapi -loleaut32 -lruntimeobject -lversion
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
# Taskbar auto-hide fine tuning

Fine-tune taskbar auto-hide with multiple modes and hotkeys. When taskbar
auto-hide is enabled, this mod gives you control over when and how the taskbar
appears.

## Auto-hide modes

- **Windows default**: Standard auto-hide behavior. Use this if you only want
  the hotkeys.
- **Keyboard or mouse click**: The taskbar only unhides via the keyboard or by
  clicking at the edge of the screen. Mouse hover no longer shows the taskbar.
- **Keyboard only**: The taskbar is completely hidden. It can only be shown via
  the keyboard.
- **Never**: The taskbar never shows for any reason (notifications, Win key,
  etc.). Only the mod's hotkeys can show it. The Start menu still opens but the
  taskbar stays hidden.

## Hotkeys

- **Show temporarily**: Briefly shows the taskbar. It hides again when you stop
  interacting with it.
- **Toggle always-show**: Toggles permanent taskbar visibility. Press again to
  return to auto-hide behavior.

On Windows 11, you can also toggle always-show via a mouse event (middle click
or double click) on the taskbar.

## Win key action

Optionally override what happens when the Win key is pressed:

- **Show taskbar**: Shows the taskbar without opening the Start menu.
- **Show taskbar, open Start menu if already shown**: Shows the taskbar on the
  first press. If the taskbar is already visible, opens the Start menu instead.
- **Toggle permanent taskbar visibility**: Toggles permanent visibility, same as
  the toggle always-show hotkey.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- mode: keyboardOnlyShowOnClick
  $name: Auto-hide mode
  $description: Refer to the mod description for details about each mode.
  $options:
  - windowsDefault: Windows default
  - keyboardOnlyShowOnClick: Keyboard or mouse click
  - keyboardOnlyFullyHide: Keyboard only
  - never: Never
- showTemporarilyHotkey: Alt+Escape
  $name: Show temporarily hotkey
  $description: >-
    Hotkey to temporarily show the taskbar (e.g. Alt+Escape or Ctrl+Alt+T). The
    taskbar hides again when you stop interacting with it.
- toggleAlwaysShowHotkey: Alt+`
  $name: Toggle always-show hotkey
  $description: >-
    Hotkey to toggle permanent taskbar visibility (e.g. Alt+` or Ctrl+Alt+Y).
    Press again to return to the configured auto-hide behavior.
- toggleAlwaysShowMouseEvent: disabled
  $name: Toggle always-show mouse event (Win11 only)
  $description: >-
    Mouse event on the taskbar to toggle permanent visibility, similar to the
    toggle always-show hotkey.
  $options:
  - disabled: Disabled
  - middleClick: Middle click
  - doubleClick: Double click
- showTemporarilyDurationMs: 1000
  $name: Show temporarily minimum duration (ms)
  $description: >-
    Minimum time in milliseconds the taskbar stays visible after being shown
    temporarily. Set to 0 for default duration.
- winKeyAction: defaultWindowsBehavior
  $name: Win key action
  $description: The action to perform when the Win key is pressed.
  $options:
  - defaultWindowsBehavior: Default Windows behavior
  - showTaskbar: Show taskbar
  - showTaskbarOpenStartIfShown: Show taskbar, open Start menu if already shown
  - togglePermanentVisibility: Toggle permanent taskbar visibility
- oldTaskbarOnWin11: false
  $name: Customize the old taskbar on Windows 11
  $description: >-
    Enable this option to customize the old taskbar on Windows 11 (if using
    ExplorerPatcher or a similar tool).
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <commctrl.h>
#include <dwmapi.h>
#include <psapi.h>

#undef GetCurrentTime

#include <winrt/Windows.UI.Input.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Input.h>

#include <algorithm>
#include <atomic>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

enum class AutoHideMode {
    WindowsDefault,
    KeyboardOnlyShowOnClick,
    KeyboardOnlyFullyHide,
    Never,
};

enum class ToggleAlwaysShowMouseEvent {
    Disabled,
    MiddleClick,
    DoubleClick,
};

enum class WinKeyAction {
    DefaultWindowsBehavior,
    ShowTaskbar,
    ShowTaskbarOpenStartIfShown,
    TogglePermanentVisibility,
};

struct {
    AutoHideMode mode;
    std::wstring showTemporarilyHotkey;
    std::wstring toggleAlwaysShowHotkey;
    ToggleAlwaysShowMouseEvent toggleAlwaysShowMouseEvent;
    int showTemporarilyDurationMs;
    WinKeyAction winKeyAction;
    bool oldTaskbarOnWin11;
} g_settings;

enum class WinVersion {
    Unsupported,
    Win10,
    Win11,
    Win11_24H2,
};

WinVersion g_winVersion;

std::atomic<bool> g_taskbarViewDllLoaded;
std::atomic<bool> g_initialized;
std::atomic<bool> g_explorerPatcherInitialized;

enum {
    kTrayUITimerHide = 2,
    kTrayUITimerUnhide = 3,
};

// Hotkey IDs.
enum {
    kHotkeyIdShowTemporarily = 1773615305,  // from epochconverter.com
    kHotkeyIdToggleAlwaysShow,
};

// Message dispatched to CTaskBand_v_WndProc_Hook for UI-thread operations.
static UINT g_uiThreadCallbackMsg =
    RegisterWindowMessage(L"Windhawk_uiThreadCallback_" WH_MOD_ID);

// Message to capture pThis in WndProc hooks after late load.
static UINT g_captureThisMsg =
    RegisterWindowMessage(L"Windhawk_captureThis_" WH_MOD_ID);

// Operations dispatched to the UI thread via g_uiThreadCallbackMsg.
enum {
    UI_REGISTER_HOTKEYS,
    UI_UNREGISTER_HOTKEYS,
    UI_APPLY_SETTINGS,
    UI_BEFORE_UNINIT,
    UI_SHOW_TASKBAR_TEMPORARILY,
    UI_SHOW_MAIN_TASKBAR_TEMPORARILY_IF_HIDDEN,
    UI_TOGGLE_ALWAYS_SHOW,
};

// Hotkey state.
bool g_showTempHotkeyRegistered;
bool g_toggleAlwaysHotkeyRegistered;

// Always-show state.
bool g_alwaysShowMode;

// Set during mod-triggered unhide to bypass Never mode blocking.
bool g_modTriggeredUnhide;

// Flyout snapping state for Never mode. Tracks the window and its original
// gap (in 96-DPI units) so we can restore position when leaving Never mode.
struct SnappedFlyout {
    HWND hwnd;
    int gapDip;  // original gap in DIP (96 DPI)
};
SnappedFlyout g_snappedStartMenu{};
SnappedFlyout g_snappedSearchMenu{};

// HWND -> TrayUI WndProc pThis.
std::unordered_map<HWND, void*> g_hwndToWndProcPThis;
// HWND -> CSecondaryTray WndProc pThis.
std::unordered_map<HWND, void*> g_hwndToSecondaryPThis;
// Win11: HWND -> ViewCoordinator pThis.
std::unordered_map<HWND, void*> g_hwndToViewCoordinator;

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

HWND GetTaskBandWnd() {
    HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();
    if (hTaskbarWnd) {
        return (HWND)GetProp(hTaskbarWnd, L"TaskbandHWND");
    }
    return nullptr;
}

HWND FindTaskbarWindows(std::vector<HWND>* secondaryTaskbarWindows) {
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
            secondaryTaskbarWindows->push_back(hWnd);
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

void CloakWindow(HWND hWnd, BOOL cloak) {
    DwmSetWindowAttribute(hWnd, DWMWA_CLOAK, &cloak, sizeof(cloak));
}

void* QueryViaVtable(void* object, void* vtable) {
    void* ptr = object;
    while (*(void**)ptr != vtable) {
        ptr = (void**)ptr + 1;
    }
    return ptr;
}

bool FromStringHotKey(std::wstring_view hotkeyString,
                      UINT* modifiersOut,
                      UINT* vkOut) {
    static const std::unordered_map<std::wstring_view, UINT> modifiersMap = {
        {L"ALT", MOD_ALT},           {L"CTRL", MOD_CONTROL},
        {L"NOREPEAT", MOD_NOREPEAT}, {L"SHIFT", MOD_SHIFT},
        {L"WIN", MOD_WIN},
    };

    static const std::unordered_map<std::wstring_view, UINT> vkMap = {
        // Letters A-Z
        {L"A", 0x41},
        {L"B", 0x42},
        {L"C", 0x43},
        {L"D", 0x44},
        {L"E", 0x45},
        {L"F", 0x46},
        {L"G", 0x47},
        {L"H", 0x48},
        {L"I", 0x49},
        {L"J", 0x4A},
        {L"K", 0x4B},
        {L"L", 0x4C},
        {L"M", 0x4D},
        {L"N", 0x4E},
        {L"O", 0x4F},
        {L"P", 0x50},
        {L"Q", 0x51},
        {L"R", 0x52},
        {L"S", 0x53},
        {L"T", 0x54},
        {L"U", 0x55},
        {L"V", 0x56},
        {L"W", 0x57},
        {L"X", 0x58},
        {L"Y", 0x59},
        {L"Z", 0x5A},
        // Numbers 0-9
        {L"0", 0x30},
        {L"1", 0x31},
        {L"2", 0x32},
        {L"3", 0x33},
        {L"4", 0x34},
        {L"5", 0x35},
        {L"6", 0x36},
        {L"7", 0x37},
        {L"8", 0x38},
        {L"9", 0x39},
        // Function keys F1-F24
        {L"F1", 0x70},
        {L"F2", 0x71},
        {L"F3", 0x72},
        {L"F4", 0x73},
        {L"F5", 0x74},
        {L"F6", 0x75},
        {L"F7", 0x76},
        {L"F8", 0x77},
        {L"F9", 0x78},
        {L"F10", 0x79},
        {L"F11", 0x7A},
        {L"F12", 0x7B},
        {L"F13", 0x7C},
        {L"F14", 0x7D},
        {L"F15", 0x7E},
        {L"F16", 0x7F},
        {L"F17", 0x80},
        {L"F18", 0x81},
        {L"F19", 0x82},
        {L"F20", 0x83},
        {L"F21", 0x84},
        {L"F22", 0x85},
        {L"F23", 0x86},
        {L"F24", 0x87},
        // Common keys
        {L"BACKSPACE", 0x08},
        {L"TAB", 0x09},
        {L"ENTER", 0x0D},
        {L"RETURN", 0x0D},
        {L"PAUSE", 0x13},
        {L"CAPSLOCK", 0x14},
        {L"ESCAPE", 0x1B},
        {L"ESC", 0x1B},
        {L"SPACE", 0x20},
        {L"SPACEBAR", 0x20},
        {L"PAGEUP", 0x21},
        {L"PAGEDOWN", 0x22},
        {L"END", 0x23},
        {L"HOME", 0x24},
        {L"LEFT", 0x25},
        {L"UP", 0x26},
        {L"RIGHT", 0x27},
        {L"DOWN", 0x28},
        {L"PRINTSCREEN", 0x2C},
        {L"PRTSC", 0x2C},
        {L"INSERT", 0x2D},
        {L"INS", 0x2D},
        {L"DELETE", 0x2E},
        {L"DEL", 0x2E},
        {L"HELP", 0x2F},
        {L"SLEEP", 0x5F},
        {L"APPS", 0x5D},
        {L"MENU", 0x5D},
        // Numpad keys
        {L"NUMPAD0", 0x60},
        {L"NUMPAD1", 0x61},
        {L"NUMPAD2", 0x62},
        {L"NUMPAD3", 0x63},
        {L"NUMPAD4", 0x64},
        {L"NUMPAD5", 0x65},
        {L"NUMPAD6", 0x66},
        {L"NUMPAD7", 0x67},
        {L"NUMPAD8", 0x68},
        {L"NUMPAD9", 0x69},
        {L"NUM0", 0x60},
        {L"NUM1", 0x61},
        {L"NUM2", 0x62},
        {L"NUM3", 0x63},
        {L"NUM4", 0x64},
        {L"NUM5", 0x65},
        {L"NUM6", 0x66},
        {L"NUM7", 0x67},
        {L"NUM8", 0x68},
        {L"NUM9", 0x69},
        {L"MULTIPLY", 0x6A},
        {L"ADD", 0x6B},
        {L"SUBTRACT", 0x6D},
        {L"DECIMAL", 0x6E},
        {L"DIVIDE", 0x6F},
        {L"NUMLOCK", 0x90},
        {L"SCROLLLOCK", 0x91},
        // Media keys
        {L"VOLUMEMUTE", 0xAD},
        {L"VOLUMEDOWN", 0xAE},
        {L"VOLUMEUP", 0xAF},
        {L"MEDIANEXT", 0xB0},
        {L"MEDIAPREV", 0xB1},
        {L"MEDIASTOP", 0xB2},
        {L"MEDIAPLAYPAUSE", 0xB3},
        // Browser keys
        {L"BROWSERBACK", 0xA6},
        {L"BROWSERFORWARD", 0xA7},
        {L"BROWSERREFRESH", 0xA8},
        {L"BROWSERSTOP", 0xA9},
        {L"BROWSERSEARCH", 0xAA},
        {L"BROWSERFAVORITES", 0xAB},
        {L"BROWSERHOME", 0xAC},
        // Modifier keys (for use as the key itself)
        {L"LWIN", 0x5B},
        {L"RWIN", 0x5C},
        {L"LSHIFT", 0xA0},
        {L"RSHIFT", 0xA1},
        {L"LCTRL", 0xA2},
        {L"RCTRL", 0xA3},
        {L"LALT", 0xA4},
        {L"RALT", 0xA5},
        // VK_ prefixed versions
        {L"VK_LBUTTON", 0x01},
        {L"VK_RBUTTON", 0x02},
        {L"VK_CANCEL", 0x03},
        {L"VK_MBUTTON", 0x04},
        {L"VK_XBUTTON1", 0x05},
        {L"VK_XBUTTON2", 0x06},
        {L"VK_CLEAR", 0x0C},
        {L"VK_SHIFT", 0x10},
        {L"VK_CONTROL", 0x11},
        {L"VK_MENU", 0x12},
        {L"VK_KANA", 0x15},
        {L"VK_HANGUL", 0x15},
        {L"VK_IME_ON", 0x16},
        {L"VK_JUNJA", 0x17},
        {L"VK_FINAL", 0x18},
        {L"VK_HANJA", 0x19},
        {L"VK_KANJI", 0x19},
        {L"VK_IME_OFF", 0x1A},
        {L"VK_CONVERT", 0x1C},
        {L"VK_NONCONVERT", 0x1D},
        {L"VK_ACCEPT", 0x1E},
        {L"VK_MODECHANGE", 0x1F},
        {L"VK_SELECT", 0x29},
        {L"VK_PRINT", 0x2A},
        {L"VK_EXECUTE", 0x2B},
        {L"VK_SEPARATOR", 0x6C},
        {L"VK_LAUNCH_MAIL", 0xB4},
        {L"VK_LAUNCH_MEDIA_SELECT", 0xB5},
        {L"VK_LAUNCH_APP1", 0xB6},
        {L"VK_LAUNCH_APP2", 0xB7},
        {L"VK_OEM_1", 0xBA},
        {L"VK_OEM_PLUS", 0xBB},
        {L"VK_OEM_COMMA", 0xBC},
        {L"VK_OEM_MINUS", 0xBD},
        {L"VK_OEM_PERIOD", 0xBE},
        {L"VK_OEM_2", 0xBF},
        {L"VK_OEM_3", 0xC0},
        {L"VK_OEM_4", 0xDB},
        {L"VK_OEM_5", 0xDC},
        {L"VK_OEM_6", 0xDD},
        {L"VK_OEM_7", 0xDE},
        {L"VK_OEM_8", 0xDF},
        {L"VK_OEM_102", 0xE2},
        {L"VK_PROCESSKEY", 0xE5},
        {L"VK_PACKET", 0xE7},
        {L"VK_ATTN", 0xF6},
        {L"VK_CRSEL", 0xF7},
        {L"VK_EXSEL", 0xF8},
        {L"VK_EREOF", 0xF9},
        {L"VK_PLAY", 0xFA},
        {L"VK_ZOOM", 0xFB},
        {L"VK_NONAME", 0xFC},
        {L"VK_PA1", 0xFD},
        {L"VK_OEM_CLEAR", 0xFE},
        // US keyboard character aliases for OEM keys.
        {L";", 0xBA},   // VK_OEM_1
        {L":", 0xBA},   // VK_OEM_1
        {L"=", 0xBB},   // VK_OEM_PLUS
        {L"+", 0xBB},   // VK_OEM_PLUS
        {L",", 0xBC},   // VK_OEM_COMMA
        {L"<", 0xBC},   // VK_OEM_COMMA
        {L"-", 0xBD},   // VK_OEM_MINUS
        {L"_", 0xBD},   // VK_OEM_MINUS
        {L".", 0xBE},   // VK_OEM_PERIOD
        {L">", 0xBE},   // VK_OEM_PERIOD
        {L"/", 0xBF},   // VK_OEM_2
        {L"?", 0xBF},   // VK_OEM_2
        {L"`", 0xC0},   // VK_OEM_3
        {L"~", 0xC0},   // VK_OEM_3
        {L"[", 0xDB},   // VK_OEM_4
        {L"{", 0xDB},   // VK_OEM_4
        {L"\\", 0xDC},  // VK_OEM_5
        {L"|", 0xDC},   // VK_OEM_5
        {L"]", 0xDD},   // VK_OEM_6
        {L"}", 0xDD},   // VK_OEM_6
        {L"'", 0xDE},   // VK_OEM_7
        {L"\"", 0xDE},  // VK_OEM_7
    };

    auto splitStringView = [](std::wstring_view s, WCHAR delimiter) {
        size_t pos_start = 0, pos_end;
        std::wstring_view token;
        std::vector<std::wstring_view> res;

        while ((pos_end = s.find(delimiter, pos_start)) !=
               std::wstring_view::npos) {
            token = s.substr(pos_start, pos_end - pos_start);
            pos_start = pos_end + 1;
            res.push_back(token);
        }

        res.push_back(s.substr(pos_start));
        return res;
    };

    auto trimStringView = [](std::wstring_view s) {
        s.remove_prefix(std::min(s.find_first_not_of(L" \t\r\v\n"), s.size()));
        s.remove_suffix(std::min(
            s.size() - s.find_last_not_of(L" \t\r\v\n") - 1, s.size()));
        return s;
    };

    UINT modifiers = 0;
    UINT vk = 0;

    auto hotkeyParts = splitStringView(hotkeyString, '+');
    for (auto hotkeyPart : hotkeyParts) {
        hotkeyPart = trimStringView(hotkeyPart);
        std::wstring hotkeyPartUpper{hotkeyPart};
        std::transform(hotkeyPartUpper.begin(), hotkeyPartUpper.end(),
                       hotkeyPartUpper.begin(), ::toupper);

        if (auto it = modifiersMap.find(hotkeyPartUpper);
            it != modifiersMap.end()) {
            modifiers |= it->second;
            continue;
        }

        if (vk) {
            // Only one key is allowed.
            return false;
        }

        if (auto it = vkMap.find(hotkeyPartUpper); it != vkMap.end()) {
            vk = it->second;
            continue;
        }

        size_t pos;
        try {
            vk = std::stoi(hotkeyPartUpper, &pos, 0);
            if (hotkeyPartUpper[pos] != L'\0' || !vk) {
                return false;
            }
        } catch (const std::exception&) {
            return false;
        }
    }

    if (!vk) {
        return false;
    }

    *modifiersOut = modifiers;
    *vkOut = vk;
    return true;
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

    PCWSTR processFileName = wcsrchr(processPath, L'\\');
    if (!processFileName) {
        return std::wstring{};
    }

    processFileName++;
    return processFileName;
}

// Restore a snapped flyout back to its original position (gap from monitor
// bottom), e.g. when leaving Never mode or when the taskbar becomes visible.
void RestoreSnappedFlyout(SnappedFlyout& flyout) {
    if (!flyout.hwnd || !IsWindow(flyout.hwnd)) {
        flyout = {};
        return;
    }

    RECT rcWindow;
    if (!GetWindowRect(flyout.hwnd, &rcWindow)) {
        flyout = {};
        return;
    }

    HMONITOR monitor = MonitorFromWindow(flyout.hwnd, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi = {.cbSize = sizeof(MONITORINFO)};
    if (!GetMonitorInfo(monitor, &mi)) {
        flyout = {};
        return;
    }

    UINT dpi = GetDpiForWindow(flyout.hwnd);
    int gapPx = MulDiv(flyout.gapDip, dpi, 96);
    int targetBottom = mi.rcMonitor.bottom - gapPx;
    int dy = targetBottom - rcWindow.bottom;
    if (dy != 0) {
        Wh_Log(L"Restoring flyout %08X (dy=%d, gapDip=%d)",
               (DWORD)(DWORD_PTR)flyout.hwnd, dy, flyout.gapDip);
        SetWindowPos(flyout.hwnd, nullptr, rcWindow.left, rcWindow.top + dy, 0,
                     0, SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOSIZE);
    }

    flyout = {};
}

void RestoreAllSnappedFlyouts() {
    RestoreSnappedFlyout(g_snappedStartMenu);
    RestoreSnappedFlyout(g_snappedSearchMenu);
}

using DwmSetWindowAttribute_t = decltype(&DwmSetWindowAttribute);
DwmSetWindowAttribute_t DwmSetWindowAttribute_Original;
HRESULT WINAPI DwmSetWindowAttribute_Hook(HWND hwnd,
                                          DWORD dwAttribute,
                                          LPCVOID pvAttribute,
                                          DWORD cbAttribute) {
    auto original = [=]() {
        return DwmSetWindowAttribute_Original(hwnd, dwAttribute, pvAttribute,
                                              cbAttribute);
    };

    if (dwAttribute != DWMWA_CLOAK || cbAttribute != sizeof(BOOL)) {
        return original();
    }

    BOOL cloak = *(const BOOL*)pvAttribute;
    if (cloak) {
        return original();
    }

    // Only act in Never mode.
    if (g_settings.mode != AutoHideMode::Never) {
        return original();
    }

    // Only act if the taskbar is on the bottom edge.
    APPBARDATA appBarData = {
        .cbSize = sizeof(APPBARDATA),
    };
    if (SHAppBarMessage(ABM_GETTASKBARPOS, &appBarData) &&
        appBarData.uEdge != ABE_BOTTOM) {
        return original();
    }

    DWORD processId = 0;
    DWORD threadId = GetWindowThreadProcessId(hwnd, &processId);
    if (!processId || !threadId) {
        return original();
    }

    std::wstring processFileName = GetProcessFileName(processId);

    enum class FlyoutKind { None, StartMenu, SearchMenu };
    FlyoutKind kind = FlyoutKind::None;
    if (_wcsicmp(processFileName.c_str(), L"StartMenuExperienceHost.exe") ==
        0) {
        kind = FlyoutKind::StartMenu;
    } else if (_wcsicmp(processFileName.c_str(), L"SearchHost.exe") == 0) {
        kind = FlyoutKind::SearchMenu;
    }

    if (kind == FlyoutKind::None) {
        return original();
    }

    RECT rcWindow;
    if (!GetWindowRect(hwnd, &rcWindow)) {
        return original();
    }

    HMONITOR monitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi = {.cbSize = sizeof(MONITORINFO)};
    if (!GetMonitorInfo(monitor, &mi)) {
        return original();
    }

    SnappedFlyout& flyout = (kind == FlyoutKind::StartMenu)
                                ? g_snappedStartMenu
                                : g_snappedSearchMenu;

    bool taskbarShown = g_alwaysShowMode || g_modTriggeredUnhide;

    Wh_Log(L"Taskbar shown: %d", taskbarShown);

    if (taskbarShown) {
        // Taskbar is visible — the system positioned the flyout correctly
        // relative to the visible taskbar. Nothing to adjust.
    } else {
        // Taskbar is hidden — snap flyout to monitor bottom and save the
        // original gap (DPI-neutral) so we can restore it later.
        int gap = mi.rcMonitor.bottom - rcWindow.bottom;
        if (gap > 0) {
            UINT dpi = GetDpiForWindow(hwnd);
            int gapDip = MulDiv(gap, 96, dpi);
            flyout = {hwnd, gapDip};

            Wh_Log(
                L"Snapping flyout %08X to monitor bottom (gap=%d, "
                L"gapDip=%d)",
                (DWORD)(DWORD_PTR)hwnd, gap, gapDip);
            SetWindowPos(hwnd, nullptr, rcWindow.left, rcWindow.top + gap, 0, 0,
                         SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOSIZE);
        }
    }

    return original();
}

using SetTimer_t = decltype(&SetTimer);
SetTimer_t SetTimer_Original;
UINT_PTR WINAPI SetTimer_Hook(HWND hWnd,
                              UINT_PTR nIDEvent,
                              UINT uElapse,
                              TIMERPROC lpTimerFunc) {
    if (g_settings.mode != AutoHideMode::WindowsDefault &&
        nIDEvent == kTrayUITimerUnhide && IsTaskbarWindow(hWnd)) {
        Wh_Log(L"Blocking unhide timer (non-default mode)");
        return 1;
    }

    if (nIDEvent == kTrayUITimerHide && g_alwaysShowMode &&
        IsTaskbarWindow(hWnd)) {
        Wh_Log(L"Blocking hide timer (always-show mode)");
        return 1;
    }

    if (nIDEvent == kTrayUITimerHide && g_modTriggeredUnhide &&
        g_settings.showTemporarilyDurationMs > 0 && IsTaskbarWindow(hWnd)) {
        Wh_Log(L"Extending hide timer to %d ms",
               g_settings.showTemporarilyDurationMs);
        uElapse = g_settings.showTemporarilyDurationMs;
    }

    UINT_PTR ret = SetTimer_Original(hWnd, nIDEvent, uElapse, lpTimerFunc);

    return ret;
}

using TrayUI_SlideWindow_t = void(WINAPI*)(void* pThis,
                                           HWND hWnd,
                                           const RECT* rc,
                                           HMONITOR monitor,
                                           bool show,
                                           bool flag);
TrayUI_SlideWindow_t TrayUI_SlideWindow_Original;
void WINAPI TrayUI_SlideWindow_Hook(void* pThis,
                                    HWND hWnd,
                                    const RECT* rc,
                                    HMONITOR monitor,
                                    bool show,
                                    bool flag) {
    Wh_Log(L">");

    bool shouldCloak = g_settings.mode == AutoHideMode::KeyboardOnlyFullyHide ||
                       g_settings.mode == AutoHideMode::Never;

    if (show && shouldCloak) {
        CloakWindow(hWnd, FALSE);
    }

    TrayUI_SlideWindow_Original(pThis, hWnd, rc, monitor, show, flag);

    if (!show && shouldCloak) {
        CloakWindow(hWnd, TRUE);
    }

    // Clear mod-triggered flag when the old taskbar hides (equivalent of
    // ViewCoordinator's pointer-leave clearing for Win11).
    if (!show && g_modTriggeredUnhide) {
        g_modTriggeredUnhide = false;
    }
}

using TrayUI_GetAutoHideFlags_t = DWORD(WINAPI*)(void* pThis);
TrayUI_GetAutoHideFlags_t TrayUI_GetAutoHideFlags_Original;

// Vtable pointers for interface navigation.
void* TrayUI_vftable_ITrayComponentHost;
void* CSecondaryTray_vftable_ISecondaryTray;

// TrayUI::_Hide hook — block hide in always-show mode.
using TrayUI__Hide_t = void(WINAPI*)(void* pThis);
TrayUI__Hide_t TrayUI__Hide_Original;
void WINAPI TrayUI__Hide_Hook(void* pThis) {
    if (g_alwaysShowMode) {
        Wh_Log(L"Blocking hide (always-show mode)");
        return;
    }

    TrayUI__Hide_Original(pThis);
}

// CSecondaryTray::_AutoHide hook — block hide in always-show mode.
using CSecondaryTray__AutoHide_t = void(WINAPI*)(void* pThis, bool param1);
CSecondaryTray__AutoHide_t CSecondaryTray__AutoHide_Original;
void WINAPI CSecondaryTray__AutoHide_Hook(void* pThis, bool param1) {
    if (g_alwaysShowMode) {
        Wh_Log(L"Blocking auto-hide (always-show mode)");
        return;
    }

    CSecondaryTray__AutoHide_Original(pThis, param1);
}

// TrayUI::Unhide — block in Never mode (mod calls _Original to bypass).
using TrayUI_Unhide_t = void(WINAPI*)(void* pThis,
                                      int trayUnhideFlags,
                                      int unhideRequest);
TrayUI_Unhide_t TrayUI_Unhide_Original;
void WINAPI TrayUI_Unhide_Hook(void* pThis,
                               int trayUnhideFlags,
                               int unhideRequest) {
    if (g_settings.mode == AutoHideMode::Never && !g_alwaysShowMode) {
        Wh_Log(L"Blocking unhide (never mode)");
        return;
    }

    TrayUI_Unhide_Original(pThis, trayUnhideFlags, unhideRequest);
}

// CSecondaryTray::_Unhide — block in Never mode (mod calls _Original to
// bypass).
using CSecondaryTray__Unhide_t = void(WINAPI*)(void* pThis,
                                               int trayUnhideFlags,
                                               int unhideRequest);
CSecondaryTray__Unhide_t CSecondaryTray__Unhide_Original;
void WINAPI CSecondaryTray__Unhide_Hook(void* pThis,
                                        int trayUnhideFlags,
                                        int unhideRequest) {
    if (g_settings.mode == AutoHideMode::Never && !g_alwaysShowMode) {
        Wh_Log(L"Blocking unhide (never mode)");
        return;
    }

    CSecondaryTray__Unhide_Original(pThis, trayUnhideFlags, unhideRequest);
}

// TrayUI::WndProc hook — capture pThis-to-HWND mapping.
using TrayUI_WndProc_t = LRESULT(WINAPI*)(void* pThis,
                                          HWND hWnd,
                                          UINT Msg,
                                          WPARAM wParam,
                                          LPARAM lParam,
                                          bool* flag);
TrayUI_WndProc_t TrayUI_WndProc_Original;
LRESULT WINAPI TrayUI_WndProc_Hook(void* pThis,
                                   HWND hWnd,
                                   UINT Msg,
                                   WPARAM wParam,
                                   LPARAM lParam,
                                   bool* flag) {
    if (Msg == WM_NCCREATE || Msg == g_captureThisMsg) {
        g_hwndToWndProcPThis[hWnd] = pThis;
    } else if (Msg == WM_NCDESTROY) {
        g_hwndToWndProcPThis.erase(hWnd);
        g_hwndToViewCoordinator.erase(hWnd);
    }

    return TrayUI_WndProc_Original(pThis, hWnd, Msg, wParam, lParam, flag);
}

// CSecondaryTray::v_WndProc hook — capture pThis-to-HWND mapping.
using CSecondaryTray_v_WndProc_t = LRESULT(
    WINAPI*)(void* pThis, HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
CSecondaryTray_v_WndProc_t CSecondaryTray_v_WndProc_Original;
LRESULT WINAPI CSecondaryTray_v_WndProc_Hook(void* pThis,
                                             HWND hWnd,
                                             UINT Msg,
                                             WPARAM wParam,
                                             LPARAM lParam) {
    if (Msg == WM_NCCREATE || Msg == g_captureThisMsg) {
        g_hwndToSecondaryPThis[hWnd] = pThis;
    } else if (Msg == WM_NCDESTROY) {
        g_hwndToSecondaryPThis.erase(hWnd);
        g_hwndToViewCoordinator.erase(hWnd);
    }

    return CSecondaryTray_v_WndProc_Original(pThis, hWnd, Msg, wParam, lParam);
}

bool g_isPointerOverTaskbarFrame;

using ViewCoordinator_HandleIsPointerOverTaskbarFrameChanged_t =
    void(WINAPI*)(void* pThis,
                  HWND hMMTaskbarWnd,
                  bool isPointerOver,
                  int inputDeviceKind);
ViewCoordinator_HandleIsPointerOverTaskbarFrameChanged_t
    ViewCoordinator_HandleIsPointerOverTaskbarFrameChanged_Original;
void WINAPI ViewCoordinator_HandleIsPointerOverTaskbarFrameChanged_Hook(
    void* pThis,
    HWND hMMTaskbarWnd,
    bool isPointerOver,
    int inputDeviceKind) {
    Wh_Log(L"> isPointerOver=%d", isPointerOver);

    g_isPointerOverTaskbarFrame = isPointerOver;

    ViewCoordinator_HandleIsPointerOverTaskbarFrameChanged_Original(
        pThis, hMMTaskbarWnd, isPointerOver, inputDeviceKind);

    g_isPointerOverTaskbarFrame = false;

    // In Never mode, clear the mod-triggered flag when the pointer leaves
    // so ShouldTaskbarBeExpanded resumes blocking expansion.
    if (!isPointerOver && g_modTriggeredUnhide) {
        g_modTriggeredUnhide = false;
    }
}

// From ViewCoordinator::HandleIsPointerOverTaskbarFrameChanged.
constexpr int kReasonIsPointerOverTaskbarFrameChanged = 7;
// From TaskbarFrame::OnScreenEdgeStrokePointerEntered
constexpr int kReasonOnScreenEdgeStrokePointerEntered = 8;

using ViewCoordinator_ShouldTaskbarBeExpanded_t =
    bool(WINAPI*)(void* pThis, HWND hMMTaskbarWnd, bool expanded);
ViewCoordinator_ShouldTaskbarBeExpanded_t
    ViewCoordinator_ShouldTaskbarBeExpanded_Original;
bool WINAPI ViewCoordinator_ShouldTaskbarBeExpanded_Hook(void* pThis,
                                                         HWND hMMTaskbarWnd,
                                                         bool expanded) {
    g_hwndToViewCoordinator[hMMTaskbarWnd] = pThis;

    if (g_alwaysShowMode) {
        return true;
    }

    // In Never mode, block all expansion unless the mod triggered it.
    if (g_settings.mode == AutoHideMode::Never && !g_modTriggeredUnhide) {
        // Returning false here breaks the taskbar layout with the old auto-hide
        // implementation.
        // TODO: test this with the new implementation.
        // return false;
    }

    return ViewCoordinator_ShouldTaskbarBeExpanded_Original(
        pThis, hMMTaskbarWnd, expanded);
}

using ViewCoordinator_UpdateIsExpanded_t = void(WINAPI*)(void* pThis,
                                                         HWND hMMTaskbarWnd,
                                                         int reason);
ViewCoordinator_UpdateIsExpanded_t ViewCoordinator_UpdateIsExpanded_Original;
void WINAPI ViewCoordinator_UpdateIsExpanded_Hook(void* pThis,
                                                  HWND hMMTaskbarWnd,
                                                  int reason) {
    Wh_Log(L"> reason=%d", reason);

    if (g_settings.mode != AutoHideMode::WindowsDefault &&
        ((reason == kReasonIsPointerOverTaskbarFrameChanged &&
          g_isPointerOverTaskbarFrame) ||
         reason == kReasonOnScreenEdgeStrokePointerEntered)) {
        Wh_Log(L"Blocking mouse-triggered unhide");
        return;
    }

    ViewCoordinator_UpdateIsExpanded_Original(pThis, hMMTaskbarWnd, reason);
}

void UpdateViewCoordinatorIsExpanded(HWND hWnd) {
    if (ViewCoordinator_UpdateIsExpanded_Original) {
        auto it = g_hwndToViewCoordinator.find(hWnd);
        if (it != g_hwndToViewCoordinator.end()) {
            ViewCoordinator_UpdateIsExpanded_Original(
                it->second, hWnd, kReasonIsPointerOverTaskbarFrameChanged);
        }
    }
}

void ShowTaskbarTemporarily(bool mainTaskbar = false) {
    Wh_Log(L">");

    if (g_alwaysShowMode) {
        return;
    }

    HMONITOR monitor;
    if (mainTaskbar) {
        // Unhide primary monitor's taskbar.
        monitor = MonitorFromPoint({0, 0}, MONITOR_DEFAULTTOPRIMARY);
    } else {
        // Only unhide the taskbar on the monitor where the mouse cursor is.
        POINT pt;
        GetCursorPos(&pt);
        monitor = MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
    }

    // Restore any flyouts that were snapped to monitor bottom.
    RestoreAllSnappedFlyouts();

    // Set before both paths so DwmSetWindowAttribute_Hook and
    // ShouldTaskbarBeExpanded know the taskbar is shown by the mod.
    // Cleared by ViewCoordinator_HandleIsPointerOverTaskbarFrameChanged_Hook
    // (Win11) or TrayUI_SlideWindow_Hook when hiding (old taskbar).
    g_modTriggeredUnhide = true;

    // Old auto-hide path.
    for (auto& [hWnd, pThis] : g_hwndToWndProcPThis) {
        if ((HMONITOR)GetProp(hWnd, L"TaskbarMonitor") != monitor) {
            continue;
        }
        void* pThisHost =
            QueryViaVtable(pThis, TrayUI_vftable_ITrayComponentHost);
        TrayUI_Unhide_Original(pThisHost, 0, 0);
    }

    for (auto& [hWnd, pThis] : g_hwndToSecondaryPThis) {
        if ((HMONITOR)GetProp(hWnd, L"TaskbarMonitor") != monitor) {
            continue;
        }
        CSecondaryTray__Unhide_Original(pThis, 0, 0);
    }

    // Win11 ViewCoordinator path.
    if (ViewCoordinator_HandleIsPointerOverTaskbarFrameChanged_Original) {
        for (auto& [hWnd, pThis] : g_hwndToViewCoordinator) {
            if ((HMONITOR)GetProp(hWnd, L"TaskbarMonitor") != monitor) {
                continue;
            }
            ViewCoordinator_HandleIsPointerOverTaskbarFrameChanged_Original(
                pThis, hWnd, true, 0);
        }
    }
}

void CloakAllTaskbars(BOOL cloak);

void HideAllTaskbars() {
    std::vector<HWND> secondaryTaskbarWindows;
    HWND hTaskbarWnd = FindTaskbarWindows(&secondaryTaskbarWindows);
    if (hTaskbarWnd) {
        SetTimer(hTaskbarWnd, kTrayUITimerHide, 0, nullptr);
    }
    for (HWND hWnd : secondaryTaskbarWindows) {
        SetTimer(hWnd, kTrayUITimerHide, 0, nullptr);
    }

    for (auto& [hWnd, pThis] : g_hwndToViewCoordinator) {
        UpdateViewCoordinatorIsExpanded(hWnd);
    }
}

void ToggleAlwaysShow() {
    g_alwaysShowMode = !g_alwaysShowMode;
    Wh_Log(L"Always-show mode: %d", g_alwaysShowMode);

    std::vector<HWND> secondaryTaskbarWindows;
    HWND hTaskbarWnd = FindTaskbarWindows(&secondaryTaskbarWindows);

    if (g_alwaysShowMode) {
        // Restore any flyouts snapped to monitor bottom.
        RestoreAllSnappedFlyouts();

        // Kill any pending hide timers.
        if (hTaskbarWnd) {
            KillTimer(hTaskbarWnd, kTrayUITimerHide);
        }
        for (HWND hWnd : secondaryTaskbarWindows) {
            KillTimer(hWnd, kTrayUITimerHide);
        }

        // Uncloak if fully hidden.
        if (g_settings.mode == AutoHideMode::KeyboardOnlyFullyHide ||
            g_settings.mode == AutoHideMode::Never) {
            CloakAllTaskbars(FALSE);
        }

        // Old auto-hide path.
        for (auto& [hWnd, pThis] : g_hwndToWndProcPThis) {
            void* pThisHost =
                QueryViaVtable(pThis, TrayUI_vftable_ITrayComponentHost);
            TrayUI_Unhide_Original(pThisHost, 0, 0);
        }

        for (auto& [hWnd, pThis] : g_hwndToSecondaryPThis) {
            CSecondaryTray__Unhide_Original(pThis, 0, 0);
        }

        // Win11 ViewCoordinator path.
        for (auto& [hWnd, pThis] : g_hwndToViewCoordinator) {
            UpdateViewCoordinatorIsExpanded(hWnd);
        }
    } else {
        HideAllTaskbars();
    }
}

void RegisterHotkeys(HWND hWnd) {
    UINT modifiers, vk;

    if (!g_settings.showTemporarilyHotkey.empty()) {
        if (FromStringHotKey(g_settings.showTemporarilyHotkey, &modifiers,
                             &vk)) {
            if (RegisterHotKey(hWnd, kHotkeyIdShowTemporarily, modifiers, vk)) {
                g_showTempHotkeyRegistered = true;
                Wh_Log(L"Registered show-temporarily hotkey: %s",
                       g_settings.showTemporarilyHotkey.c_str());
            } else {
                Wh_Log(L"Failed to register show-temporarily hotkey: %s",
                       g_settings.showTemporarilyHotkey.c_str());
            }
        } else {
            Wh_Log(L"Failed to parse show-temporarily hotkey: %s",
                   g_settings.showTemporarilyHotkey.c_str());
        }
    }

    if (!g_settings.toggleAlwaysShowHotkey.empty()) {
        if (FromStringHotKey(g_settings.toggleAlwaysShowHotkey, &modifiers,
                             &vk)) {
            if (RegisterHotKey(hWnd, kHotkeyIdToggleAlwaysShow, modifiers,
                               vk)) {
                g_toggleAlwaysHotkeyRegistered = true;
                Wh_Log(L"Registered toggle-always-show hotkey: %s",
                       g_settings.toggleAlwaysShowHotkey.c_str());
            } else {
                Wh_Log(L"Failed to register toggle-always-show hotkey: %s",
                       g_settings.toggleAlwaysShowHotkey.c_str());
            }
        } else {
            Wh_Log(L"Failed to parse toggle-always-show hotkey: %s",
                   g_settings.toggleAlwaysShowHotkey.c_str());
        }
    }
}

void UnregisterHotkeys(HWND hWnd) {
    if (g_showTempHotkeyRegistered) {
        UnregisterHotKey(hWnd, kHotkeyIdShowTemporarily);
        g_showTempHotkeyRegistered = false;
    }

    if (g_toggleAlwaysHotkeyRegistered) {
        UnregisterHotKey(hWnd, kHotkeyIdToggleAlwaysShow);
        g_toggleAlwaysHotkeyRegistered = false;
    }
}

using XamlLauncher_ShowStartView_t =
    HRESULT(WINAPI*)(void* pThis,
                     int immersiveLauncherShowMethod,
                     int immersiveLauncherShowFlags);
XamlLauncher_ShowStartView_t XamlLauncher_ShowStartView_Original;
HRESULT WINAPI XamlLauncher_ShowStartView_Hook(void* pThis,
                                               int immersiveLauncherShowMethod,
                                               int immersiveLauncherShowFlags) {
    Wh_Log(L"> %d, %d", immersiveLauncherShowMethod,
           immersiveLauncherShowFlags);

    auto original = [=] {
        return XamlLauncher_ShowStartView_Original(
            pThis, immersiveLauncherShowMethod, immersiveLauncherShowFlags);
    };

    // If not keyboard hotkey.
    if (immersiveLauncherShowMethod != 1) {
        return original();
    }

    HWND hTaskBandWnd = GetTaskBandWnd();

    switch (g_settings.winKeyAction) {
        case WinKeyAction::ShowTaskbar:
            if (hTaskBandWnd) {
                PostMessage(hTaskBandWnd, g_uiThreadCallbackMsg,
                            UI_SHOW_TASKBAR_TEMPORARILY, 0);
            }
            return S_OK;

        case WinKeyAction::ShowTaskbarOpenStartIfShown:
            if (g_alwaysShowMode || g_modTriggeredUnhide) {
                break;
            }
            if (hTaskBandWnd) {
                bool shown =
                    SendMessage(hTaskBandWnd, g_uiThreadCallbackMsg,
                                UI_SHOW_MAIN_TASKBAR_TEMPORARILY_IF_HIDDEN, 0);
                if (shown) {
                    return S_OK;
                }
            }
            break;

        case WinKeyAction::TogglePermanentVisibility:
            if (hTaskBandWnd) {
                PostMessage(hTaskBandWnd, g_uiThreadCallbackMsg,
                            UI_TOGGLE_ALWAYS_SHOW, 0);
            }
            return S_OK;

        case WinKeyAction::DefaultWindowsBehavior:
        default:
            break;
    }

    return original();
}

void LoadSettings();

// Called on the UI thread via SendMessage(UI_APPLY_SETTINGS).
void ApplySettingsOnUIThread(HWND hWnd) {
    UnregisterHotkeys(hWnd);

    AutoHideMode prevMode = g_settings.mode;

    LoadSettings();

    // If always-show was active and all toggle methods were removed, disable
    // it.
    if (g_alwaysShowMode && g_settings.toggleAlwaysShowHotkey.empty() &&
        g_settings.toggleAlwaysShowMouseEvent ==
            ToggleAlwaysShowMouseEvent::Disabled) {
        g_alwaysShowMode = false;
        Wh_Log(L"Always-show disabled (toggle methods removed)");
        HideAllTaskbars();
    }

    bool wasCloaked = prevMode == AutoHideMode::KeyboardOnlyFullyHide ||
                      prevMode == AutoHideMode::Never;
    bool isCloaked = g_settings.mode == AutoHideMode::KeyboardOnlyFullyHide ||
                     g_settings.mode == AutoHideMode::Never;
    if (isCloaked != wasCloaked) {
        CloakAllTaskbars(isCloaked ? TRUE : FALSE);
    }

    // When leaving Never mode, restore any flyouts that were snapped to the
    // monitor bottom back to their natural position (with taskbar gap).
    if (prevMode == AutoHideMode::Never &&
        g_settings.mode != AutoHideMode::Never) {
        RestoreAllSnappedFlyouts();
    }

    RegisterHotkeys(hWnd);
}

using CTaskBand_v_WndProc_t = LRESULT(
    WINAPI*)(void* pThis, HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
CTaskBand_v_WndProc_t CTaskBand_v_WndProc_Original;
LRESULT WINAPI CTaskBand_v_WndProc_Hook(void* pThis,
                                        HWND hWnd,
                                        UINT Msg,
                                        WPARAM wParam,
                                        LPARAM lParam) {
    LRESULT result = 0;

    auto originalProc = [pThis](HWND hWnd, UINT Msg, WPARAM wParam,
                                LPARAM lParam) {
        return CTaskBand_v_WndProc_Original(pThis, hWnd, Msg, wParam, lParam);
    };

    switch (Msg) {
        case WM_HOTKEY:
            switch (wParam) {
                case kHotkeyIdShowTemporarily:
                    Wh_Log(L"Show-temporarily hotkey triggered");
                    ShowTaskbarTemporarily();
                    break;

                case kHotkeyIdToggleAlwaysShow:
                    Wh_Log(L"Toggle-always-show hotkey triggered");
                    ToggleAlwaysShow();
                    break;

                default:
                    result = originalProc(hWnd, Msg, wParam, lParam);
                    break;
            }
            break;

        case WM_CREATE:
            result = originalProc(hWnd, Msg, wParam, lParam);
            RegisterHotkeys(hWnd);
            break;

        case WM_DESTROY:
            UnregisterHotkeys(hWnd);
            result = originalProc(hWnd, Msg, wParam, lParam);
            break;

        default:
            if (Msg == g_uiThreadCallbackMsg) {
                switch (wParam) {
                    case UI_REGISTER_HOTKEYS:
                        RegisterHotkeys(hWnd);
                        break;
                    case UI_UNREGISTER_HOTKEYS:
                        UnregisterHotkeys(hWnd);
                        break;
                    case UI_APPLY_SETTINGS:
                        ApplySettingsOnUIThread(hWnd);
                        break;
                    case UI_BEFORE_UNINIT:
                        if (g_alwaysShowMode) {
                            g_alwaysShowMode = false;
                            HideAllTaskbars();
                        }
                        if (g_settings.mode == AutoHideMode::Never) {
                            RestoreAllSnappedFlyouts();
                        }
                        UnregisterHotkeys(hWnd);
                        break;
                    case UI_SHOW_TASKBAR_TEMPORARILY:
                        ShowTaskbarTemporarily();
                        break;
                    case UI_SHOW_MAIN_TASKBAR_TEMPORARILY_IF_HIDDEN: {
                        bool shown = false;
                        for (auto& [hWnd, pThis] : g_hwndToWndProcPThis) {
                            DWORD flags =
                                TrayUI_GetAutoHideFlags_Original(pThis);
                            Wh_Log(L"GetAutoHideFlags: %08X", flags);
                            if (!(flags & 0x02)) {
                                shown = true;
                                break;
                            }
                        }

                        if (!shown) {
                            ShowTaskbarTemporarily(/*mainTaskbar=*/true);
                            result = 1;
                        }
                        break;
                    }
                    case UI_TOGGLE_ALWAYS_SHOW:
                        ToggleAlwaysShow();
                        break;
                }
            } else {
                result = originalProc(hWnd, Msg, wParam, lParam);
            }
            break;
    }

    return result;
}

// Double-click state for mouse toggle.
DWORD g_lastPressTime;

using TaskbarFrame_OnPointerPressed_t = int(WINAPI*)(void* pThis, void* pArgs);
TaskbarFrame_OnPointerPressed_t TaskbarFrame_OnPointerPressed_Original;
int TaskbarFrame_OnPointerPressed_Hook(void* pThis, void* pArgs) {
    Wh_Log(L">");

    auto original = [=]() {
        return TaskbarFrame_OnPointerPressed_Original(pThis, pArgs);
    };

    if (g_settings.toggleAlwaysShowMouseEvent ==
        ToggleAlwaysShowMouseEvent::Disabled) {
        return original();
    }

    winrt::Windows::UI::Xaml::UIElement taskbarFrame = nullptr;
    ((IUnknown*)pThis)
        ->QueryInterface(winrt::guid_of<winrt::Windows::UI::Xaml::UIElement>(),
                         winrt::put_abi(taskbarFrame));

    if (!taskbarFrame) {
        return original();
    }

    if (winrt::get_class_name(taskbarFrame) != L"Taskbar.TaskbarFrame") {
        return original();
    }

    winrt::Windows::UI::Xaml::Input::PointerRoutedEventArgs args{nullptr};
    winrt::copy_from_abi(args, pArgs);
    if (!args) {
        return original();
    }

    auto properties = args.GetCurrentPoint(taskbarFrame).Properties();

    bool shouldToggle = false;

    if (g_settings.toggleAlwaysShowMouseEvent ==
        ToggleAlwaysShowMouseEvent::MiddleClick) {
        if (properties.IsMiddleButtonPressed()) {
            shouldToggle = true;
        }
    } else if (g_settings.toggleAlwaysShowMouseEvent ==
               ToggleAlwaysShowMouseEvent::DoubleClick) {
        DWORD now = GetTickCount();
        if (now - g_lastPressTime <= GetDoubleClickTime()) {
            g_lastPressTime = 0;
            shouldToggle = true;
        } else {
            g_lastPressTime = now;
        }
    }

    if (!shouldToggle) {
        return original();
    }

    ToggleAlwaysShow();

    args.Handled(true);
    return 0;
}

bool HookTaskbarViewDllSymbols(HMODULE module) {
    // Taskbar.View.dll, ExplorerExtensions.dll
    WindhawkUtils::SYMBOL_HOOK symbolHooks[] = {
        {
            {LR"(public: void __cdecl winrt::Taskbar::implementation::ViewCoordinator::HandleIsPointerOverTaskbarFrameChanged(unsigned __int64,bool,enum winrt::WindowsUdk::UI::Shell::InputDeviceKind))"},
            &ViewCoordinator_HandleIsPointerOverTaskbarFrameChanged_Original,
            ViewCoordinator_HandleIsPointerOverTaskbarFrameChanged_Hook,
            true,
        },
        {
            {LR"(public: bool __cdecl winrt::Taskbar::implementation::ViewCoordinator::ShouldTaskbarBeExpanded(unsigned __int64,bool))"},
            &ViewCoordinator_ShouldTaskbarBeExpanded_Original,
            ViewCoordinator_ShouldTaskbarBeExpanded_Hook,
            true,
        },
        {
            {LR"(public: void __cdecl winrt::Taskbar::implementation::ViewCoordinator::UpdateIsExpanded(unsigned __int64,enum TaskbarTipTest::TaskbarExpandCollapseReason))"},
            &ViewCoordinator_UpdateIsExpanded_Original,
            ViewCoordinator_UpdateIsExpanded_Hook,
            true,
        },
        {
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskbarFrame,struct winrt::Windows::UI::Xaml::Controls::IControlOverrides>::OnPointerPressed(void *))"},
            &TaskbarFrame_OnPointerPressed_Original,
            TaskbarFrame_OnPointerPressed_Hook,
            true,
        },
    };

    if (!HookSymbols(module, symbolHooks, ARRAYSIZE(symbolHooks))) {
        Wh_Log(L"HookSymbols failed");
        return false;
    }

    return true;
}

HMODULE GetTaskbarViewModuleHandle() {
    HMODULE module = GetModuleHandle(L"Taskbar.View.dll");
    if (!module) {
        module = GetModuleHandle(L"ExplorerExtensions.dll");
    }

    return module;
}

void HandleLoadedModuleIfTaskbarView(HMODULE module, LPCWSTR lpLibFileName) {
    if (g_winVersion >= WinVersion::Win11 && !g_taskbarViewDllLoaded &&
        GetTaskbarViewModuleHandle() == module &&
        !g_taskbarViewDllLoaded.exchange(true)) {
        Wh_Log(L"Loaded %s", lpLibFileName);

        if (HookTaskbarViewDllSymbols(module)) {
            Wh_ApplyHookOperations();
        }
    }
}

bool HookTaskbarSymbols() {
    HMODULE module;
    if (g_winVersion <= WinVersion::Win10) {
        module = GetModuleHandle(nullptr);
    } else {
        module = LoadLibraryEx(L"taskbar.dll", nullptr,
                               LOAD_LIBRARY_SEARCH_SYSTEM32);
        if (!module) {
            Wh_Log(L"Couldn't load taskbar.dll");
            return false;
        }
    }

    // Taskbar.dll, explorer.exe
    WindhawkUtils::SYMBOL_HOOK symbolHooks[] = {
        {
            {LR"(const TrayUI::`vftable'{for `ITrayComponentHost'})"},
            &TrayUI_vftable_ITrayComponentHost,
        },
        {
            {LR"(const CSecondaryTray::`vftable'{for `ISecondaryTray'})"},
            &CSecondaryTray_vftable_ISecondaryTray,
        },
        {
            {LR"(public: virtual void __cdecl TrayUI::SlideWindow(struct HWND__ *,struct tagRECT const *,struct HMONITOR__ *,bool,bool))"},
            &TrayUI_SlideWindow_Original,
            TrayUI_SlideWindow_Hook,
        },
        {
            {LR"(public: void __cdecl TrayUI::_Hide(void))"},
            &TrayUI__Hide_Original,
            TrayUI__Hide_Hook,
        },
        {
            {LR"(private: void __cdecl CSecondaryTray::_AutoHide(bool))"},
            &CSecondaryTray__AutoHide_Original,
            CSecondaryTray__AutoHide_Hook,
        },
        {
            {LR"(public: virtual void __cdecl TrayUI::Unhide(enum TrayCommon::TrayUnhideFlags,enum TrayCommon::UnhideRequest))"},
            &TrayUI_Unhide_Original,
            TrayUI_Unhide_Hook,
        },
        {
            {LR"(private: void __cdecl CSecondaryTray::_Unhide(enum TrayCommon::TrayUnhideFlags,enum TrayCommon::UnhideRequest))"},
            &CSecondaryTray__Unhide_Original,
            CSecondaryTray__Unhide_Hook,
        },
        {
            {LR"(public: virtual unsigned int __cdecl TrayUI::GetAutoHideFlags(void))"},
            &TrayUI_GetAutoHideFlags_Original,
        },
        {
            {LR"(public: virtual __int64 __cdecl TrayUI::WndProc(struct HWND__ *,unsigned int,unsigned __int64,__int64,bool *))"},
            &TrayUI_WndProc_Original,
            TrayUI_WndProc_Hook,
        },
        {
            {LR"(private: virtual __int64 __cdecl CSecondaryTray::v_WndProc(struct HWND__ *,unsigned int,unsigned __int64,__int64))"},
            &CSecondaryTray_v_WndProc_Original,
            CSecondaryTray_v_WndProc_Hook,
        },
        {
            {LR"(protected: virtual __int64 __cdecl CTaskBand::v_WndProc(struct HWND__ *,unsigned int,unsigned __int64,__int64))"},
            &CTaskBand_v_WndProc_Original,
            CTaskBand_v_WndProc_Hook,
        },
    };

    if (!HookSymbols(module, symbolHooks, ARRAYSIZE(symbolHooks))) {
        Wh_Log(L"HookSymbols failed");
        return false;
    }

    return true;
}

bool HookTwinuiPcshellSymbols() {
    HMODULE module = LoadLibraryEx(L"twinui.pcshell.dll", nullptr,
                                   LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!module) {
        Wh_Log(L"Couldn't load twinui.pcshell.dll");
        return false;
    }

    // twinui.pcshell.dll
    WindhawkUtils::SYMBOL_HOOK symbolHooks[] = {
        {
            {LR"(public: virtual long __cdecl XamlLauncher::ShowStartView(enum IMMERSIVELAUNCHERSHOWMETHOD,enum IMMERSIVELAUNCHERSHOWFLAGS))"},
            &XamlLauncher_ShowStartView_Original,
            XamlLauncher_ShowStartView_Hook,
        },
    };

    if (!HookSymbols(module, symbolHooks, ARRAYSIZE(symbolHooks))) {
        Wh_Log(L"HookSymbols failed");
        return false;
    }

    return true;
}

VS_FIXEDFILEINFO* GetModuleVersionInfo(HMODULE hModule, UINT* puPtrLen) {
    void* pFixedFileInfo = nullptr;
    UINT uPtrLen = 0;

    HRSRC hResource =
        FindResource(hModule, MAKEINTRESOURCE(VS_VERSION_INFO), RT_VERSION);
    if (hResource) {
        HGLOBAL hGlobal = LoadResource(hModule, hResource);
        if (hGlobal) {
            void* pData = LockResource(hGlobal);
            if (pData) {
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

WinVersion GetExplorerVersion() {
    VS_FIXEDFILEINFO* fixedFileInfo = GetModuleVersionInfo(nullptr, nullptr);
    if (!fixedFileInfo) {
        return WinVersion::Unsupported;
    }

    WORD major = HIWORD(fixedFileInfo->dwFileVersionMS);
    WORD minor = LOWORD(fixedFileInfo->dwFileVersionMS);
    WORD build = HIWORD(fixedFileInfo->dwFileVersionLS);
    WORD qfe = LOWORD(fixedFileInfo->dwFileVersionLS);

    Wh_Log(L"Version: %u.%u.%u.%u", major, minor, build, qfe);

    switch (major) {
        case 10:
            if (build < 22000) {
                return WinVersion::Win10;
            } else if (build < 26100) {
                return WinVersion::Win11;
            } else {
                return WinVersion::Win11_24H2;
            }
            break;
    }

    return WinVersion::Unsupported;
}

struct EXPLORER_PATCHER_HOOK {
    PCSTR symbol;
    void** pOriginalFunction;
    void* hookFunction = nullptr;
    bool optional = false;

    template <typename Prototype>
    EXPLORER_PATCHER_HOOK(
        PCSTR symbol,
        Prototype** originalFunction,
        std::type_identity_t<Prototype*> hookFunction = nullptr,
        bool optional = false)
        : symbol(symbol),
          pOriginalFunction(reinterpret_cast<void**>(originalFunction)),
          hookFunction(reinterpret_cast<void*>(hookFunction)),
          optional(optional) {}
};

bool HookExplorerPatcherSymbols(HMODULE explorerPatcherModule) {
    if (g_explorerPatcherInitialized.exchange(true)) {
        return true;
    }

    if (g_winVersion >= WinVersion::Win11) {
        g_winVersion = WinVersion::Win10;
    }

    EXPLORER_PATCHER_HOOK hooks[] = {
        {R"(??_7TrayUI@@6BITrayComponentHost@@@)",
         &TrayUI_vftable_ITrayComponentHost},
        {R"(??_7CSecondaryTray@@6BISecondaryTray@@@)",
         &CSecondaryTray_vftable_ISecondaryTray},
        {R"(?SlideWindow@TrayUI@@UEAAXPEAUHWND__@@PEBUtagRECT@@PEAUHMONITOR__@@_N3@Z)",
         &TrayUI_SlideWindow_Original, TrayUI_SlideWindow_Hook},
        {R"(?_Hide@TrayUI@@QEAAXXZ)", &TrayUI__Hide_Original,
         TrayUI__Hide_Hook},
        {R"(?_AutoHide@CSecondaryTray@@AEAAX_N@Z)",
         &CSecondaryTray__AutoHide_Original, CSecondaryTray__AutoHide_Hook},
        {R"(?Unhide@TrayUI@@UEAAXW4TrayUnhideFlags@TrayCommon@@W4UnhideRequest@3@@Z)",
         &TrayUI_Unhide_Original, TrayUI_Unhide_Hook},
        {R"(?_Unhide@CSecondaryTray@@AEAAXW4TrayUnhideFlags@TrayCommon@@W4UnhideRequest@3@@Z)",
         &CSecondaryTray__Unhide_Original, CSecondaryTray__Unhide_Hook},
        {R"(?GetAutoHideFlags@TrayUI@@UEAAIXZ)",
         &TrayUI_GetAutoHideFlags_Original},
        {R"(?WndProc@TrayUI@@UEAA_JPEAUHWND__@@I_K_JPEA_N@Z)",
         &TrayUI_WndProc_Original, TrayUI_WndProc_Hook},
        {R"(?v_WndProc@CSecondaryTray@@EEAA_JPEAUHWND__@@I_K_J@Z)",
         &CSecondaryTray_v_WndProc_Original, CSecondaryTray_v_WndProc_Hook,
         // Available in versions newer than 67.1.
         true},
        {R"(?v_WndProc@CTaskBand@@MEAA_JPEAUHWND__@@I_K_J@Z)",
         &CTaskBand_v_WndProc_Original, CTaskBand_v_WndProc_Hook},
    };

    bool succeeded = true;

    for (const auto& hook : hooks) {
        void* ptr = (void*)GetProcAddress(explorerPatcherModule, hook.symbol);
        if (!ptr) {
            Wh_Log(L"ExplorerPatcher symbol%s doesn't exist: %S",
                   hook.optional ? L" (optional)" : L"", hook.symbol);
            if (!hook.optional) {
                succeeded = false;
            }
            continue;
        }

        if (hook.hookFunction) {
            Wh_SetFunctionHook(ptr, hook.hookFunction, hook.pOriginalFunction);
        } else {
            *hook.pOriginalFunction = ptr;
        }
    }

    if (!succeeded) {
        Wh_Log(L"HookExplorerPatcherSymbols failed");
    } else if (g_initialized) {
        Wh_ApplyHookOperations();
    }

    return succeeded;
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

bool HandleLoadedExplorerPatcher() {
    HMODULE hMods[1024];
    DWORD cbNeeded;
    if (EnumProcessModules(GetCurrentProcess(), hMods, sizeof(hMods),
                           &cbNeeded)) {
        for (size_t i = 0; i < cbNeeded / sizeof(HMODULE); i++) {
            if (IsExplorerPatcherModule(hMods[i])) {
                return HookExplorerPatcherSymbols(hMods[i]);
            }
        }
    }

    return true;
}

void HandleLoadedModuleIfExplorerPatcher(HMODULE module) {
    if (module && !((ULONG_PTR)module & 3) && !g_explorerPatcherInitialized) {
        if (IsExplorerPatcherModule(module)) {
            HookExplorerPatcherSymbols(module);
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
        HandleLoadedModuleIfTaskbarView(module, lpLibFileName);
    }

    return module;
}

void LoadSettings() {
    PCWSTR mode = Wh_GetStringSetting(L"mode");
    if (wcscmp(mode, L"windowsDefault") == 0) {
        g_settings.mode = AutoHideMode::WindowsDefault;
    } else if (wcscmp(mode, L"keyboardOnlyFullyHide") == 0) {
        g_settings.mode = AutoHideMode::KeyboardOnlyFullyHide;
    } else if (wcscmp(mode, L"never") == 0) {
        g_settings.mode = AutoHideMode::Never;
    } else {
        g_settings.mode = AutoHideMode::KeyboardOnlyShowOnClick;
    }
    Wh_FreeStringSetting(mode);

    PCWSTR showTemp = Wh_GetStringSetting(L"showTemporarilyHotkey");
    g_settings.showTemporarilyHotkey = showTemp;
    Wh_FreeStringSetting(showTemp);

    PCWSTR toggleAlways = Wh_GetStringSetting(L"toggleAlwaysShowHotkey");
    g_settings.toggleAlwaysShowHotkey = toggleAlways;
    Wh_FreeStringSetting(toggleAlways);

    PCWSTR mouseEvent = Wh_GetStringSetting(L"toggleAlwaysShowMouseEvent");
    if (wcscmp(mouseEvent, L"middleClick") == 0) {
        g_settings.toggleAlwaysShowMouseEvent =
            ToggleAlwaysShowMouseEvent::MiddleClick;
    } else if (wcscmp(mouseEvent, L"doubleClick") == 0) {
        g_settings.toggleAlwaysShowMouseEvent =
            ToggleAlwaysShowMouseEvent::DoubleClick;
    } else {
        g_settings.toggleAlwaysShowMouseEvent =
            ToggleAlwaysShowMouseEvent::Disabled;
    }
    Wh_FreeStringSetting(mouseEvent);

    g_settings.showTemporarilyDurationMs =
        Wh_GetIntSetting(L"showTemporarilyDurationMs");

    PCWSTR winKeyAction = Wh_GetStringSetting(L"winKeyAction");
    if (wcscmp(winKeyAction, L"showTaskbar") == 0) {
        g_settings.winKeyAction = WinKeyAction::ShowTaskbar;
    } else if (wcscmp(winKeyAction, L"showTaskbarOpenStartIfShown") == 0) {
        g_settings.winKeyAction = WinKeyAction::ShowTaskbarOpenStartIfShown;
    } else if (wcscmp(winKeyAction, L"togglePermanentVisibility") == 0) {
        g_settings.winKeyAction = WinKeyAction::TogglePermanentVisibility;
    } else {
        g_settings.winKeyAction = WinKeyAction::DefaultWindowsBehavior;
    }
    Wh_FreeStringSetting(winKeyAction);

    g_settings.oldTaskbarOnWin11 = Wh_GetIntSetting(L"oldTaskbarOnWin11");
}

BOOL Wh_ModInit() {
    Wh_Log(L">");

    LoadSettings();

    g_winVersion = GetExplorerVersion();
    if (g_winVersion == WinVersion::Unsupported) {
        Wh_Log(L"Unsupported Windows version");
        return FALSE;
    }

    if (g_settings.oldTaskbarOnWin11) {
        bool hasWin10Taskbar = g_winVersion < WinVersion::Win11_24H2;

        if (g_winVersion >= WinVersion::Win11) {
            g_winVersion = WinVersion::Win10;
        }

        if (hasWin10Taskbar && !HookTaskbarSymbols()) {
            return FALSE;
        }
    } else {
        if (!HookTaskbarSymbols()) {
            return FALSE;
        }

        if (g_winVersion >= WinVersion::Win11) {
            if (HMODULE taskbarViewModule = GetTaskbarViewModuleHandle()) {
                g_taskbarViewDllLoaded = true;
                if (!HookTaskbarViewDllSymbols(taskbarViewModule)) {
                    return FALSE;
                }
            } else {
                Wh_Log(L"Taskbar view module not loaded yet");
            }
        }
    }

    if (!HandleLoadedExplorerPatcher()) {
        Wh_Log(L"HandleLoadedExplorerPatcher failed");
        return FALSE;
    }

    if (g_settings.winKeyAction != WinKeyAction::DefaultWindowsBehavior) {
        if (!HookTwinuiPcshellSymbols()) {
            return FALSE;
        }
    }

    HMODULE kernelBaseModule = GetModuleHandle(L"kernelbase.dll");
    auto pKernelBaseLoadLibraryExW = (decltype(&LoadLibraryExW))GetProcAddress(
        kernelBaseModule, "LoadLibraryExW");
    WindhawkUtils::SetFunctionHook(pKernelBaseLoadLibraryExW,
                                   LoadLibraryExW_Hook,
                                   &LoadLibraryExW_Original);

    WindhawkUtils::SetFunctionHook(SetTimer, SetTimer_Hook, &SetTimer_Original);

    WindhawkUtils::SetFunctionHook(DwmSetWindowAttribute,
                                   DwmSetWindowAttribute_Hook,
                                   &DwmSetWindowAttribute_Original);

    g_initialized = true;

    return TRUE;
}

void CloakAllTaskbars(BOOL cloak) {
    std::vector<HWND> secondaryTaskbarWindows;
    HWND taskbarWindow = FindTaskbarWindows(&secondaryTaskbarWindows);
    if (taskbarWindow) {
        CloakWindow(taskbarWindow, cloak);
    }
    for (HWND hWnd : secondaryTaskbarWindows) {
        CloakWindow(hWnd, cloak);
    }
}

void Wh_ModAfterInit() {
    Wh_Log(L">");

    if (g_winVersion >= WinVersion::Win11 && !g_taskbarViewDllLoaded) {
        if (HMODULE taskbarViewModule = GetTaskbarViewModuleHandle()) {
            if (!g_taskbarViewDllLoaded.exchange(true)) {
                Wh_Log(L"Got Taskbar.View.dll");

                if (HookTaskbarViewDllSymbols(taskbarViewModule)) {
                    Wh_ApplyHookOperations();
                }
            }
        }
    }

    // Try again in case there's a race between the previous attempt and the
    // LoadLibraryExW hook.
    if (!g_explorerPatcherInitialized) {
        HandleLoadedExplorerPatcher();
    }

    if (g_settings.mode == AutoHideMode::KeyboardOnlyFullyHide ||
        g_settings.mode == AutoHideMode::Never) {
        CloakAllTaskbars(TRUE);
    }

    // Send a message to each taskbar window to trigger the WndProc hooks
    // and populate pThis maps (WM_NCCREATE already fired before mod loaded).
    {
        std::vector<HWND> secondaryTaskbarWindows;
        HWND hTaskbarWnd = FindTaskbarWindows(&secondaryTaskbarWindows);
        if (hTaskbarWnd) {
            SendMessage(hTaskbarWnd, g_captureThisMsg, 0, 0);
        }
        for (HWND hWnd : secondaryTaskbarWindows) {
            SendMessage(hWnd, g_captureThisMsg, 0, 0);
        }
    }

    if (HWND hTaskBandWnd = GetTaskBandWnd()) {
        SendMessage(hTaskBandWnd, g_uiThreadCallbackMsg, UI_REGISTER_HOTKEYS,
                    0);
    }
}

void Wh_ModBeforeUninit() {
    Wh_Log(L">");

    if (HWND hTaskBandWnd = GetTaskBandWnd()) {
        SendMessage(hTaskBandWnd, g_uiThreadCallbackMsg, UI_BEFORE_UNINIT, 0);
    }
}

void Wh_ModUninit() {
    Wh_Log(L">");

    if (g_settings.mode == AutoHideMode::KeyboardOnlyFullyHide ||
        g_settings.mode == AutoHideMode::Never) {
        CloakAllTaskbars(FALSE);
    }

    g_hwndToWndProcPThis.clear();
    g_hwndToSecondaryPThis.clear();
    g_hwndToViewCoordinator.clear();
}

BOOL Wh_ModSettingsChanged(BOOL* bReload) {
    Wh_Log(L">");

    bool prevWinKeyActionCustomized =
        g_settings.winKeyAction != WinKeyAction::DefaultWindowsBehavior;
    bool prevOldTaskbarOnWin11 = g_settings.oldTaskbarOnWin11;

    // All UI work (unregister old hotkeys, load settings, cloak/uncloak,
    // register new hotkeys) runs on the UI thread.
    if (HWND hTaskBandWnd = GetTaskBandWnd()) {
        SendMessage(hTaskBandWnd, g_uiThreadCallbackMsg, UI_APPLY_SETTINGS, 0);
    } else {
        LoadSettings();
    }

    bool winKeyActionCustomized =
        g_settings.winKeyAction != WinKeyAction::DefaultWindowsBehavior;

    *bReload = g_settings.oldTaskbarOnWin11 != prevOldTaskbarOnWin11 ||
               prevWinKeyActionCustomized != winKeyActionCustomized;

    return TRUE;
}
