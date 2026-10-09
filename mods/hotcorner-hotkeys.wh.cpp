// ==WindhawkMod==
// @id                   hotcorner-hotkeys
// @name                 HotCorner Hotkeys
// @description          Assign up to 9 distinct actions per hotkey, dispatched based on which screen zone the cursor is in when pressed (4 corners, 4 edges, or elsewhere).
// @version              3.56
// @author               webberlv
// @github               https://github.com/webberLV
// @license              GPL-3.0-only
// @include             explorer.exe
// @compilerOptions -lcomctl32 -loleaut32 -lole32 -lversion

// ==/WindhawkMod==

// Source code is published under The GNU General Public License v3.0.
//
// For pull requests, development takes place here:
// https://github.com/webberlv/windhawk-mods/tree/hotcorner-hotkeys

// ==WindhawkModReadme==
/*
# HotCorner Hotkeys

Assign actions to global hotkeys, with the action chosen by the cursor's
current screen zone when the hotkey is pressed. Each hotkey can have separate
actions for the four corners, four edges, and everywhere else.

## Cursor zones

Zones are detected from the **physical monitor edges**, not the taskbar or work
area. A zone becomes active when the cursor is within **12 pixels** of that
monitor edge.

When a hotkey is pressed, the mod first checks the exact zone under the cursor.
If nothing is configured there, it follows the fallback chain below until it
finds a configured action. If nothing matches, no action is run.

| Cursor zone  | Fallback order                            |
|--------------|-------------------------------------------|
| `TOP_LEFT`   | `TOP_LEFT` → `TOP` → `LEFT` → `ELSEWHERE` |
| `TOP_RIGHT`  | `TOP_RIGHT` → `TOP` → `RIGHT` → `ELSEWHERE` |
| `BOTTOM_LEFT` | `BOTTOM_LEFT` → `BOTTOM` → `LEFT` → `ELSEWHERE` |
| `BOTTOM_RIGHT` | `BOTTOM_RIGHT` → `BOTTOM` → `RIGHT` → `ELSEWHERE` |
| `TOP`        | `TOP` → `ELSEWHERE`                       |
| `BOTTOM`     | `BOTTOM` → `ELSEWHERE`                    |
| `LEFT`       | `LEFT` → `ELSEWHERE`                      |
| `RIGHT`      | `RIGHT` → `ELSEWHERE`                     |
| `ELSEWHERE`  | `ELSEWHERE`                               |

## Supported actions

1. **Show desktop**: Toggles desktop view using the taskbar's Show Desktop command.
2. **Ctrl+Alt+Tab**: Opens the Ctrl+Alt+Tab task switcher.
3. **Task Manager**: Opens Windows Task Manager.
4. **Mute system volume**: Toggles mute on all active render audio devices.
5. **Taskbar auto-hide**: Toggles the Windows taskbar auto-hide setting.
6. **Win+Tab**: Opens Task View.
7. **Hide desktop icons**: Toggles desktop icon visibility.
8. **Combine Taskbar buttons**: Toggles taskbar button combine mode between configured states. Uses Additional Arguments.
9. **Toggle Taskbar alignment**: Toggles taskbar alignment between left and center.
10. **Open Start menu**: Sends the Windows key to open Start.
11. **Virtual key press**: Sends one or more parsed virtual keys with `SendInput`. Uses Additional Arguments.
12. **Open application, path or URL**: Opens a target through `ShellExecuteEx`. Uses Additional Arguments.
13. **Clipboard Run**: Reads the current clipboard text, appends it to a configured alias or target string, and opens the result. Uses Additional Arguments.
14. **Open parsed title path in explorer, with fallback to exe**: Tries to extract a file path from the foreground window title and select it in Explorer; if that fails, it selects the foreground process executable instead.
15. **Opacity Up (active window)**: Raises the active window's opacity through fixed levels. If the window is already layered and reaches the top step, the layered style is removed to restore full opacity.
16. **Opacity Down (active window)**: Lowers the active window's opacity through fixed levels. If the window is not layered yet, it becomes layered and starts at 226.
17. **Toggle Always On Top (active window)**: Toggles the active window between normal and topmost.
18. **Force Kill Active Window**: Terminates the foreground window's process, except for the current `explorer.exe` host process.
19. **Media Play/Pause**: Sends the media play/pause key.
20. **Media Next Track**: Sends the media next-track key.
21. **Media Previous Track**: Sends the media previous-track key.

## Hotkey format

Use this format:

```text
Modifier+Modifier+Key
```

Rules:

- Use exactly one non-modifier key.
- Modifiers can appear in any order.
- Hotkey names are case-insensitive.

Supported modifiers:

- `Alt`
- `Ctrl`
- `Shift`
- `Win`
- `NoRepeat`

Common key names:

- Letters: `A` to `Z`
- Numbers: `0` to `9`
- Function keys: `F1` to `F24`
- Navigation: `Home`, `End`, `PageUp`, `PageDown`, `Insert`, `Delete`, `Left`, `Right`, `Up`, `Down`
- Common: `Enter`, `Tab`, `Space`, `Backspace`, `Escape` or `Esc`, `CapsLock`, `PrintScreen`, `Pause`
- Numpad: `Numpad0` to `Numpad9` or `Num0` to `Num9`, plus `NumLock`, `Add`, `Subtract`, `Multiply`, `Divide`, `Decimal`
- Media: `VolumeMute`, `VolumeUp`, `VolumeDown`, `MediaPlayPause`, `MediaNext`, `MediaPrev`, `MediaStop`
- Extra keys used mainly by **Virtual key press**: `LWin`, `RWin`, `LShift`, `RShift`, `LCtrl`, `RCtrl`, `LAlt`, `RAlt`
- Some additional `VK_*` names from [Virtual-Key Codes](https://learn.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes)
- Numeric virtual-key codes are also accepted, such as `0x70`

Examples:

```text
Ctrl+Alt+D
Ctrl+Shift+F1
Alt+Home
Win+Numpad1
NoRepeat+Ctrl+Z
```

## Additional Arguments

Only these actions use `Additional Arguments`:

- **Combine Taskbar buttons**
- **Virtual key press**
- **Open application, path or URL**
- **Clipboard Run**

All other actions ignore this field.

### Virtual key press

Use one or more semicolon-separated hotkey-style entries.

Each entry is parsed, then all parsed keys are flattened into one list and sent
together in a single `SendInput` batch. That means `Ctrl+C;Alt+Tab` does **not**
send Copy and then Alt+Tab as two separate shortcuts. It presses all parsed keys
together in one batch.

If any modifier key is still physically held when the action fires, sending is
deferred briefly until those modifier keys are released, with a timeout of about
500 ms.

Examples:

```text
Ctrl+C
Alt+F4
Ctrl+Shift+Esc
VolumeMute
LWin+Tab
```

### Open application, path or URL

This value is passed to `ShellExecuteEx`.

It can be:

- An executable
- A document, folder, or path
- A URL
- An executable plus arguments

The target is opened on the monitor where the cursor is currently located.

Examples:

```text
notepad.exe
C:\Windows\System32\cmd.exe
"C:\Program Files\Everything\Everything.exe"
explorer.exe C:\
https://example.com
```

To request elevation, prefix the value with `uac;`:

```text
uac;cmd.exe
uac;"C:\Tools\app.exe" --example
```

### Clipboard Run

This action reads the current clipboard text. If the clipboard is empty, nothing
happens.

`Additional Arguments` can be:

- A built-in alias
- A custom target string, typically a URL prefix such as `https://example.com/search?q=`
- The same value prefixed with `uac;`

The clipboard text is appended **as-is** to the configured target string, and
the result is opened through `ShellExecuteEx` on the monitor where the cursor is
currently located.

Built-in aliases:

| Alias       | Expands to                                    |
|-------------|-----------------------------------------------|
| `gpt`       | `https://chatgpt.com/?q=`                     |
| `yt`        | `https://www.youtube.com/results?search_query=` |
| `google`    | `https://www.google.com/search?q=`            |
| `copilot`   | `https://copilot.microsoft.com/?sendquery=1&q=` |
| `x`         | `https://twitter.com/search?q=`               |
| `reddit`    | `https://www.reddit.com/search/?q=`           |
| `translate` | `https://translate.google.com/?text=`         |

Examples:

```text
gpt
google
https://www.bing.com/search?q=
uac;https://example.com/search?q=
```

### Combine taskbar buttons

Use up to four semicolon-separated state values:

```text
primaryState1;primaryState2;secondaryState1;secondaryState2
```

Valid values:

```text
COMBINE_ALWAYS
COMBINE_WHEN_FULL
COMBINE_NEVER
```

The action toggles between the first two values for the primary taskbar. If the
third and fourth values are also provided, they control the same toggle for
secondary taskbars.

Toggle direction is tracked by an internal variable that is initialized from the
registry the first time the action runs. If you change the registry externally
between presses, the next toggle may be out of sync.

**Example:**

```text
COMBINE_ALWAYS;COMBINE_NEVER
```

## Notes

- Zone detection is based on the monitor under the cursor at the moment the hotkey fires.
- `Open parsed title path in explorer, with fallback to exe` checks the foreground window title from right to left, splitting on ` - `. It only accepts absolute paths that resolve to real files. If none are found, it falls back to the foreground process executable.
- `Opacity Up (active window)` uses these fixed levels when increasing: `56`, `85`, `113`, `141`, `170`, `198`, `226`. It only works on windows that are already layered.
- `Opacity Down (active window)` uses these fixed levels when decreasing: `226`, `198`, `170`, `141`, `113`, `85`, `56`, `0`. If the window is not layered yet, it is first made layered and set to `226`.
- `Toggle Always On Top (active window)` and `Force Kill Active Window` operate on the current foreground window.

## Credits

15 of the actions in this mod are based on
[Click on empty taskbar space](https://windhawk.net/mods/taskbar-empty-space-clicks)
by [m1lhaus](https://github.com/m1lhaus).

The global hotkey triggering mechanism is based on
[keyboard-shortcut-actions](https://windhawk.net/mods/keyboard-shortcut-actions)
by [m417z](https://github.com/m417z).

*/
// ==/WindhawkModReadme==


// ==WindhawkModSettings==
/*
- HotkeyActions:
  - - Hotkey: ""
      $name: Keyboard Shortcut
      $description: >-
        Hotkey in format: Modifier+Key (e.g., Ctrl+Alt+D).
        Modifiers: Alt, Ctrl, Shift, Win.
        Keys: A-Z, 0-9, F1-F24, Enter, Space, Home, End, Insert, Delete, etc.
    - Regions:
      - - Region: ELSEWHERE
          $name: Region
          $options:
          - ELSEWHERE: ELSEWHERE
          - TOP_LEFT: Top-left corner
          - TOP: Top edge
          - TOP_RIGHT: Top-right corner
          - LEFT: Left edge
          - RIGHT: Right edge
          - BOTTOM_LEFT: Bottom-left corner
          - BOTTOM: Bottom edge
          - BOTTOM_RIGHT: Bottom-right corner
        - Action: ACTION_NOTHING
          $name: Action
          $description: Action to invoke when the hotkey is pressed in this region.
          $options:
          - ACTION_NOTHING: Nothing
          - ACTION_SHOW_DESKTOP: Show desktop
          - ACTION_ALT_TAB: Ctrl+Alt+Tab
          - ACTION_TASK_MANAGER: Task Manager
          - ACTION_MUTE: Mute system volume
          - ACTION_TASKBAR_AUTOHIDE: Taskbar auto-hide
          - ACTION_WIN_TAB: Win+Tab
          - ACTION_HIDE_ICONS: Hide desktop icons
          - ACTION_COMBINE_TASKBAR_BUTTONS: Combine Taskbar buttons
          - ACTION_TOGGLE_TASKBAR_ALIGNMENT: Toggle Taskbar alignment
          - ACTION_OPEN_START_MENU: Open Start menu
          - ACTION_SEND_KEYPRESS: Virtual key press
          - ACTION_START_PROCESS: Open application, path or URL
          - ACTION_COPYRUN: Clipboard Run
          - ACTION_SELECT_ACTIVE_IN_EXPLORER: Open parsed title path in explorer, with fallback to exe.
          - ACTION_OPACITY_UP_ACTIVE: Opacity Up (active window)
          - ACTION_OPACITY_DOWN_ACTIVE: Opacity Down (active window)
          - ACTION_TOGGLE_ALWAYSONTOP_ACTIVE: Toggle Always On Top (active window)
          - ACTION_FORCE_KILL_ACTIVE: Force Kill Active Window
          - ACTION_MEDIA_PLAY_PAUSE: Media Play/Pause
          - ACTION_MEDIA_NEXT: Media Next Track
          - ACTION_MEDIA_PREV: Media Previous Track
        - AdditionalArgs: ""
          $name: Additional Arguments
          $description: >-
            Optional. Only used by Virtual key press, Open application/path/URL,
            Clipboard Run, and Combine Taskbar Buttons.
            Examples: Ctrl+C • uac;cmd.exe • google; • COMBINE_ALWAYS;COMBINE_NEVER
            See the Details tab for full documentation.
  $name: Keyboard Shortcut Actions
*/
// ==/WindhawkModSettings==

#include <commctrl.h>
#include <endpointvolume.h>
#include <mmdeviceapi.h>
#include <shellapi.h>
#include <windef.h>
#include <windhawk_api.h>
#include <windhawk_utils.h>
#include <winerror.h>
#include <winrt/base.h>
#include <winuser.h>

#include <algorithm>
#include <cwctype>
#include <filesystem>
#include <functional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#if defined(__GNUC__) && __GNUC__ > 8
#define WINAPI_LAMBDA_RETURN(return_t) ->return_t WINAPI
#elif defined(__GNUC__)
#define WINAPI_LAMBDA_RETURN(return_t) WINAPI->return_t
#else
#define WINAPI_LAMBDA_RETURN(return_t) ->return_t
#endif

using winrt::com_ptr;

// =====================================================================

// #define ENABLE_LOG_DEBUG

#ifdef ENABLE_LOG_DEBUG
#define LOG_DEBUG(format, ...) Wh_Log(L"DEBUG: " format, ##__VA_ARGS__)
#else
#define LOG_DEBUG(format, ...)
#endif

// =====================================================================

#pragma region declarations

// Enum for taskbar buttons state
enum TaskBarButtonsState {
    COMBINE_ALWAYS = 0,
    COMBINE_WHEN_FULL,
    COMBINE_NEVER,
    COMBINE_INVALID
};

// Enum for hotkey actions

enum class HotkeyActionType {
    Nothing,
    ShowDesktop,
    AltTab,
    TaskManager,
    Mute,
    TaskbarAutohide,
    WinTab,
    HideIcons,
    CombineTaskbarButtons,
    ToggleTaskbarAlignment,
    OpenStartMenu,
    SendKeypress,
    StartProcess,
    CopyRun,
    SelectActiveInExplorer,
    OpacityUp,
    OpacityDown,
    ToggleAlwaysOnTop,
    ForceKillActive,
    MediaPlayPause,
    MediaNext,
    MediaPrev,
    Invalid,
};

enum class HotkeyRegionName {
    TopLeft,
    Top,
    TopRight,
    Left,
    Elsewhere,
    Right,
    BottomLeft,
    Bottom,
    BottomRight,
    Invalid,
};

struct HotkeyBinding {
    std::wstring hotkeyString;
    HotkeyRegionName region;
    HotkeyActionType actionType;
    std::wstring additionalArgs;
    std::function<void()> actionExecutor;
    UINT modifiers;
    UINT vk;
    int hotkeyId;
    bool ownsRegistration;
};

static struct {
    std::vector<HotkeyBinding> hotkeyActions;
} g_settings;

// Wrapper for COM API initialization (audio device enumeration only)
class AudioCOMAPI {
   public:
    AudioCOMAPI()
        : m_isInitialized(false),
          m_isCOMInitialized(false),
          m_pDeviceEnumerator(nullptr) {}

    bool Init() {
        if (!m_isCOMInitialized) {
            if (SUCCEEDED(CoInitializeEx(NULL, COINIT_APARTMENTTHREADED))) {
                m_isCOMInitialized = true;
            } else {
                Wh_Log(L"COM initialization failed");
                return false;
            }
        }

        if (!m_isInitialized && m_isCOMInitialized) {
            const GUID XIID_IMMDeviceEnumerator = {
                0xA95664D2,
                0x9614,
                0x4F35,
                {0xA7, 0x46, 0xDE, 0x8D, 0xB6, 0x36, 0x17, 0xE6}};
            const GUID XIID_MMDeviceEnumerator = {
                0xBCDE0395,
                0xE52F,
                0x467C,
                {0x8E, 0x3D, 0xC4, 0x57, 0x92, 0x91, 0x69, 0x2E}};

            if (FAILED(CoCreateInstance(XIID_MMDeviceEnumerator, NULL,
                                        CLSCTX_INPROC_SERVER,
                                        XIID_IMMDeviceEnumerator,
                                        m_pDeviceEnumerator.put_void())) ||
                !m_pDeviceEnumerator) {
                Wh_Log(L"Failed to create DeviceEnumerator COM instance");
                return false;
            }
            m_isInitialized = true;
        }

        return m_isInitialized;
    }

    void Uninit() {
        if (m_isInitialized) {
            m_pDeviceEnumerator = nullptr;
            m_isInitialized = false;
        }
        if (m_isCOMInitialized) {
            CoUninitialize();
            m_isCOMInitialized = false;
        }
    }

    bool IsInitialized() { return m_isInitialized; }

    const com_ptr<IMMDeviceEnumerator> GetDeviceEnumerator() {
        return m_pDeviceEnumerator;
    }

   private:
    bool m_isInitialized;
    bool m_isCOMInitialized;
    com_ptr<IMMDeviceEnumerator> m_pDeviceEnumerator;
};
static AudioCOMAPI g_audioCOM;

namespace stringtools {
std::wstring ltrim(const std::wstring& s) {
    std::wstring result = s;
    if (!result.empty()) {
        result.erase(result.begin(),
                     std::find_if(result.begin(), result.end(), [](wchar_t ch) {
                         return !std::iswspace(ch);
                     }));
    }
    return result;
}

std::wstring rtrim(const std::wstring& s) {
    std::wstring result = s;
    result.erase(std::find_if(result.rbegin(), result.rend(),
                              [](wchar_t ch) { return !std::iswspace(ch); })
                     .base(),
                 result.end());
    return result;
}

std::wstring trim(const std::wstring& s) {
    return rtrim(ltrim(s));
}

std::wstring trimDecorated(std::wstring s) {
    if (!s.empty() && s.front() == L'*') {
        s.erase(0, 1);
    }

    s = trim(s);

    if (s.size() >= 2 &&
        ((s.front() == L'"' && s.back() == L'"') ||
         (s.front() == L'\'' && s.back() == L'\''))) {
        s = s.substr(1, s.size() - 2);
    }

    return s;
}

std::wstring toLower(const std::wstring& s) {
    std::wstring result = s;
    std::transform(result.begin(), result.end(), result.begin(), ::towlower);
    return result;
}

bool startsWith(const std::wstring& s, const std::wstring& prefix) {
    if (s.length() < prefix.length()) {
        return false;
    }
    return std::equal(prefix.begin(), prefix.end(), s.begin());
}

}  // namespace stringtools

static HWND g_hTaskbarWnd = nullptr;
static DWORD g_dwTaskbarThreadId = 0;

// Base hotkey ID - each action gets base + index
static const int kHotkeyIdBase = 1775748064;  // from epochconverter.com

// Message for hotkey registration management
static UINT g_hotkeyRegisteredMsg =
    RegisterWindowMessage(L"Windhawk_hotkey_" WH_MOD_ID);

enum {
    HOTKEY_REGISTER,
    HOTKEY_UNREGISTER,
};

// Message to uninit COM from GUI thread
static const UINT g_uninitCOMMsg =
    RegisterWindowMessage(L"Windhawk_UnInit_COM_" WH_MOD_ID);

// Timer constants for keypress retry when modifier keys are pressed
static const int kMaxKeypressRetryCount = 50;  // ~500ms max wait
static const UINT kKeypressRetryIntervalMs = 10;

// Pending keypress storage for deferred send
static std::vector<int> g_pendingKeypressKeys;
static int g_pendingKeypressRetryCount = 0;
static UINT_PTR g_keypressTimerId = 0;

// Forward declarations
void LoadSettings();
bool FromStringHotKey(std::wstring_view hotkeyString,
                      UINT* modifiersOut,
                      UINT* vkOut);
void RegisterHotkeys(HWND hWnd);
void UnregisterHotkeys(HWND hWnd);
HotkeyRegionName TryParseRegionName(const std::wstring& raw);
HotkeyActionType TryParseActionType(const std::wstring& raw);
const wchar_t* RegionNameToString(HotkeyRegionName region);
const wchar_t* ActionTypeToString(HotkeyActionType actionType);
std::function<void()> ParseActionSetting(HotkeyActionType actionType,
                                         const std::wstring& args);

#pragma endregion  // declarations

// =====================================================================

#pragma region hotkey_parsing

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
        // Common keys (friendly names)
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
        // Modifier keys (for use in Virtual key press action)
        {L"LWIN", 0x5B},
        {L"RWIN", 0x5C},
        {L"LSHIFT", 0xA0},
        {L"RSHIFT", 0xA1},
        {L"LCTRL", 0xA2},
        {L"RCTRL", 0xA3},
        {L"LALT", 0xA4},
        {L"RALT", 0xA5},
        // VK_ prefixed versions (only keys without friendly aliases)
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

#pragma endregion  // hotkey_parsing

// =====================================================================


#pragma region hotkey_registration

void RegisterHotkeys(HWND hWnd) {
    for (auto& entry : g_settings.hotkeyActions) {
        entry.hotkeyId = 0;
        entry.ownsRegistration = false;
    }

    for (size_t i = 0; i < g_settings.hotkeyActions.size();) {
        auto& first = g_settings.hotkeyActions[i];
        const UINT modifiers = first.modifiers;
        const UINT vk = first.vk;

        size_t end = i + 1;
        while (end < g_settings.hotkeyActions.size() &&
               g_settings.hotkeyActions[end].modifiers == modifiers &&
               g_settings.hotkeyActions[end].vk == vk) {
            ++end;
        }

        int id = kHotkeyIdBase + static_cast<int>(i);

        if (RegisterHotKey(hWnd, id, modifiers, vk)) {
            first.hotkeyId = id;
            first.ownsRegistration = true;

            Wh_Log(L"Registered hotkey '%s' (id=%d)",
                   first.hotkeyString.c_str(), id);

            for (size_t j = i + 1; j < end; ++j) {
                g_settings.hotkeyActions[j].hotkeyId = id;
                g_settings.hotkeyActions[j].ownsRegistration = false;

                Wh_Log(L"Hotkey '%s' region %s: sharing id=%d",
                       g_settings.hotkeyActions[j].hotkeyString.c_str(),
                       RegionNameToString(g_settings.hotkeyActions[j].region),
                       id);
            }
        } else {
            Wh_Log(L"Hotkey '%s': failed to register (error=%lu)",
                   first.hotkeyString.c_str(), GetLastError());
       }

        i = end;
    }
}

void UnregisterHotkeys(HWND hWnd) {
    for (auto& entry : g_settings.hotkeyActions) {
        if (entry.ownsRegistration && entry.hotkeyId != 0) {
            if (!UnregisterHotKey(hWnd, entry.hotkeyId)) {
                Wh_Log(L"UnregisterHotKey(id=%d) failed (error=%lu)",
                       entry.hotkeyId, GetLastError());
            }
       }

        entry.ownsRegistration = false;
        entry.hotkeyId = 0;
    }
}
#pragma endregion  // hotkey_registration
// =====================================================================

#pragma region actions

// Splits semicolon-separated argument string into vector
std::vector<std::wstring> SplitArgs(const std::wstring& args,
                                    const wchar_t delimiter = L';') {
    std::vector<std::wstring> result;

    std::wstring args_ = stringtools::trim(args);
    if (args_.empty()) {
        return result;
    }

    size_t start = 0;
    size_t end = args_.find(delimiter);
    while (end != std::wstring::npos) {
        auto substring = stringtools::trim(args_.substr(start, end - start));
        if (!substring.empty()) {
            result.push_back(substring);
        }
        start = end + 1;
        end = args_.find(delimiter, start);
    }
    auto substring = stringtools::trim(args_.substr(start));
    if (!substring.empty()) {
        result.push_back(substring);
    }
    return result;
}

// Sends show desktop command to taskbar
void ShowDesktop() {
    if (g_hTaskbarWnd) {
        Wh_Log(L"Sending ShowDesktop message");
        SendMessage(g_hTaskbarWnd, WM_COMMAND, MAKELONG(407, 0), 0);
    } else {
        Wh_Log(L"Failed to show desktop - taskbar not found");
    }
}

// Returns current taskbar autohide state
bool GetTaskbarAutohideState() {
    if (g_hTaskbarWnd != NULL) {
        APPBARDATA msgData{};
        msgData.cbSize = sizeof(msgData);
        msgData.hWnd = g_hTaskbarWnd;
        LPARAM state = SHAppBarMessage(ABM_GETSTATE, &msgData);
        return state & ABS_AUTOHIDE;
    }
    return false;
}

// Sets taskbar autohide state
void SetTaskbarAutohide(bool enabled) {
    if (g_hTaskbarWnd != NULL) {
        APPBARDATA msgData{};
        msgData.cbSize = sizeof(msgData);
        msgData.hWnd = g_hTaskbarWnd;
        msgData.lParam = enabled ? ABS_AUTOHIDE : ABS_ALWAYSONTOP;
        SHAppBarMessage(ABM_SETSTATE, &msgData);
    }
}

// Toggles taskbar autohide state
void ToggleTaskbarAutohide() {
    if (g_hTaskbarWnd != NULL) {
        const bool isEnabled = GetTaskbarAutohideState();
        Wh_Log(L"Setting taskbar autohide to %s",
               !isEnabled ? L"enabled" : L"disabled");
        SetTaskbarAutohide(!isEnabled);
    } else {
        Wh_Log(L"Failed to toggle taskbar autohide - taskbar not found");
    }
}

std::tuple<TaskBarButtonsState,
           TaskBarButtonsState,
           TaskBarButtonsState,
           TaskBarButtonsState>
ParseTaskBarButtonsState(const std::wstring& args) {
    TaskBarButtonsState primaryState1 = COMBINE_INVALID;
    TaskBarButtonsState primaryState2 = COMBINE_INVALID;
    TaskBarButtonsState secondaryState1 = COMBINE_INVALID;
    TaskBarButtonsState secondaryState2 = COMBINE_INVALID;

    const auto argsSplit = SplitArgs(args);

    auto parseState = [](const std::wstring& arg) -> TaskBarButtonsState {
        if (arg == L"COMBINE_ALWAYS")
            return COMBINE_ALWAYS;
        if (arg == L"COMBINE_WHEN_FULL")
            return COMBINE_WHEN_FULL;
        if (arg == L"COMBINE_NEVER")
            return COMBINE_NEVER;
        return COMBINE_INVALID;
    };

    if (argsSplit.size() >= 1)
        primaryState1 = parseState(argsSplit[0]);
    if (argsSplit.size() >= 2)
        primaryState2 = parseState(argsSplit[1]);
    if (argsSplit.size() >= 3)
        secondaryState1 = parseState(argsSplit[2]);
    if (argsSplit.size() >= 4)
        secondaryState2 = parseState(argsSplit[3]);

    return std::make_tuple(primaryState1, primaryState2, secondaryState1,
                           secondaryState2);
}

DWORD GetCombineTaskbarButtons(const wchar_t* optionName) {
    HKEY hKey = NULL;
    DWORD dwValue = 0;
    DWORD dwBufferSize = sizeof(DWORD);
    if (RegOpenKeyEx(HKEY_CURRENT_USER,
                     TEXT("Software\\Microsoft\\Windows\\CurrentVersion\\Explor"
                          "er\\Advanced"),
                     0, KEY_QUERY_VALUE, &hKey) == ERROR_SUCCESS) {
        RegQueryValueEx(hKey, optionName, NULL, NULL, (LPBYTE)&dwValue,
                        &dwBufferSize);
        RegCloseKey(hKey);
    }
    return dwValue;
}

bool SetCombineTaskbarButtons(const wchar_t* optionName, unsigned int option) {
    bool success = false;
    if (option <= 2) {
        HKEY hKey = NULL;
        if (RegOpenKeyEx(HKEY_CURRENT_USER,
                         TEXT("Software\\Microsoft\\Windows\\CurrentVersion\\Ex"
                              "plorer\\Advanced"),
                         0, KEY_SET_VALUE, &hKey) == ERROR_SUCCESS) {
            DWORD dwValue = option;
            if (RegSetValueEx(hKey, optionName, 0, REG_DWORD, (BYTE*)&dwValue,
                              sizeof(dwValue)) == ERROR_SUCCESS) {
                success = true;
            }
            RegCloseKey(hKey);
        }
    }
    return success;
}

void CombineTaskbarButtons(const TaskBarButtonsState primaryState1,
                           const TaskBarButtonsState primaryState2,
                           const TaskBarButtonsState secondaryState1,
                           const TaskBarButtonsState secondaryState2) {
    bool notify = false;
    if (primaryState1 != COMBINE_INVALID && primaryState2 != COMBINE_INVALID) {
        static bool zigzag =
            (GetCombineTaskbarButtons(L"TaskbarGlomLevel") == primaryState1);
        zigzag = !zigzag;
        notify |= SetCombineTaskbarButtons(
            L"TaskbarGlomLevel", zigzag ? primaryState1 : primaryState2);
    }
    if (secondaryState1 != COMBINE_INVALID &&
        secondaryState2 != COMBINE_INVALID) {
        static bool zigzag2 = (GetCombineTaskbarButtons(
                                   L"MMTaskbarGlomLevel") == secondaryState1);
        zigzag2 = !zigzag2;
        notify |= SetCombineTaskbarButtons(
            L"MMTaskbarGlomLevel", zigzag2 ? secondaryState1 : secondaryState2);
    }
    if (notify) {
        SendMessageTimeout(HWND_BROADCAST, WM_SETTINGCHANGE, 0,
                           (LPARAM)TEXT("TraySettings"), SMTO_ABORTIFHUNG, 100,
                           NULL);
    }
}

DWORD GetTaskbarAlignment() {
    HKEY hKey = NULL;
    DWORD dwValue = 1;  // Default to center
    DWORD dwBufferSize = sizeof(DWORD);

    if (RegOpenKeyEx(HKEY_CURRENT_USER,
                     TEXT("Software\\Microsoft\\Windows\\CurrentVersion\\Explor"
                          "er\\Advanced"),
                     0, KEY_QUERY_VALUE, &hKey) == ERROR_SUCCESS) {
        RegQueryValueEx(hKey, TEXT("TaskbarAl"), NULL, NULL, (LPBYTE)&dwValue,
                        &dwBufferSize);
        RegCloseKey(hKey);
    }
    return dwValue;
}

bool SetTaskbarAlignment(DWORD alignment) {
    HKEY hKey = NULL;
    bool success = false;

    if (RegOpenKeyEx(HKEY_CURRENT_USER,
                     TEXT("Software\\Microsoft\\Windows\\CurrentVersion\\Explor"
                          "er\\Advanced"),
                     0, KEY_SET_VALUE, &hKey) == ERROR_SUCCESS) {
        if (RegSetValueEx(hKey, TEXT("TaskbarAl"), 0, REG_DWORD,
                          (BYTE*)&alignment,
                          sizeof(alignment)) == ERROR_SUCCESS) {
            success = true;
        }
        RegCloseKey(hKey);
    }
    return success;
}

void ToggleTaskbarAlignment() {
    DWORD current = GetTaskbarAlignment();
    DWORD newAlign = (current == 0) ? 1 : 0;
    Wh_Log(L"Toggling taskbar alignment from %d to %d", current, newAlign);
    if (SetTaskbarAlignment(newAlign)) {
        SendMessageTimeout(HWND_BROADCAST, WM_SETTINGCHANGE, 0,
                           (LPARAM)TEXT("TraySettings"), SMTO_ABORTIFHUNG, 100,
                           NULL);
    }
}

// Finds desktop window for show/hide icons command
HWND FindDesktopWindow() {
    HWND hParentWnd = FindWindow(L"Progman", NULL);
    if (!hParentWnd) {
        return NULL;
    }

    HWND hChildWnd = FindWindowEx(hParentWnd, NULL, L"SHELLDLL_DefView", NULL);
    if (!hChildWnd) {
        DWORD dwThreadId = GetWindowThreadProcessId(hParentWnd, NULL);
        EnumThreadWindows(
            dwThreadId,
            [](HWND hWnd, LPARAM lParam) WINAPI_LAMBDA_RETURN(BOOL) {
                WCHAR szClassName[16];
                if (GetClassName(hWnd, szClassName, _countof(szClassName)) ==
                    0) {
                    return TRUE;
                }

                if (lstrcmp(szClassName, L"WorkerW") != 0) {
                    return TRUE;
                }

                HWND hChildWnd =
                    FindWindowEx(hWnd, NULL, L"SHELLDLL_DefView", NULL);
                if (!hChildWnd) {
                    return TRUE;
                }

                *(HWND*)lParam = hChildWnd;
                return FALSE;
            },
            (LPARAM)&hChildWnd);
    }

    return hChildWnd;
}

// Toggles desktop icon visibility
void HideIcons() {
    HWND hDesktopWnd = FindDesktopWindow();
    if (hDesktopWnd != NULL) {
        Wh_Log(L"Toggling desktop icons");
        PostMessage(hDesktopWnd, WM_COMMAND, 0x7402, 0);
    } else {
        Wh_Log(L"Failed to find desktop window");
    }
}

void OpenTaskManager() {
    Wh_Log(L"Opening Task Manager");

    WCHAR szWindowsDirectory[MAX_PATH];
    GetWindowsDirectory(szWindowsDirectory, ARRAYSIZE(szWindowsDirectory));
    std::wstring taskmgrPath = szWindowsDirectory;
    taskmgrPath += L"\\System32\\Taskmgr.exe";

    SHELLEXECUTEINFO sei = {sizeof(sei)};
    sei.lpVerb = L"open";
    sei.lpFile = taskmgrPath.c_str();
    sei.nShow = SW_SHOW;

    if (!ShellExecuteEx(&sei)) {
        DWORD error = GetLastError();
        if (error != ERROR_CANCELLED) {
            Wh_Log(L"Failed to start Task Manager, error: %d", error);
        }
    }
}

// Checks if any modifier keys are currently pressed
bool AreModifierKeysPressed() {
    return (GetAsyncKeyState(VK_CONTROL) & 0x8000) ||
           (GetAsyncKeyState(VK_MENU) & 0x8000) ||  // Alt
           (GetAsyncKeyState(VK_SHIFT) & 0x8000) ||
           (GetAsyncKeyState(VK_LWIN) & 0x8000) ||
           (GetAsyncKeyState(VK_RWIN) & 0x8000);
}

// Internal function to send virtual key sequence
void SendKeypressInternal(const std::vector<int>& keys) {
    if (keys.empty()) {
        return;
    }

    const int NUM_KEYS = static_cast<int>(keys.size());
    std::unique_ptr<INPUT[]> input(new INPUT[NUM_KEYS * 2]);

    for (int i = 0; i < NUM_KEYS; i++) {
        input[i].type = INPUT_KEYBOARD;
        input[i].ki.wScan = 0;
        input[i].ki.time = 0;
        input[i].ki.dwExtraInfo = 0;
        input[i].ki.wVk = static_cast<WORD>(keys[i]);
        input[i].ki.dwFlags = 0;  // KEYDOWN
    }

    for (int i = 0; i < NUM_KEYS; i++) {
        input[NUM_KEYS + i].type = INPUT_KEYBOARD;
        input[NUM_KEYS + i].ki.wScan = 0;
        input[NUM_KEYS + i].ki.time = 0;
        input[NUM_KEYS + i].ki.dwExtraInfo = 0;
        input[NUM_KEYS + i].ki.wVk = static_cast<WORD>(keys[i]);
        input[NUM_KEYS + i].ki.dwFlags = KEYEVENTF_KEYUP;
    }

    SendInput(NUM_KEYS * 2, input.get(), sizeof(input[0]));
}

// Timer callback to retry sending keypress when modifier keys are released
void CALLBACK KeypressRetryTimerProc(HWND, UINT, UINT_PTR timerId, DWORD) {
    if (!AreModifierKeysPressed()) {
        // Keys released, send the keypress
        KillTimer(nullptr, timerId);
        g_keypressTimerId = 0;
        Wh_Log(L"Modifier keys released, sending deferred keypress");
        SendKeypressInternal(g_pendingKeypressKeys);
        g_pendingKeypressKeys.clear();
    } else if (++g_pendingKeypressRetryCount >= kMaxKeypressRetryCount) {
        // Timeout - send anyway
        KillTimer(nullptr, timerId);
        g_keypressTimerId = 0;
        Wh_Log(L"Timeout waiting for modifier keys, sending anyway");
        SendKeypressInternal(g_pendingKeypressKeys);
        g_pendingKeypressKeys.clear();
    }
}

// Sends virtual key sequence, deferring if modifier keys are pressed
void SendKeypress(const std::vector<int>& keys) {
    if (keys.empty()) {
        return;
    }

    // Cancel any pending keypress
    if (g_keypressTimerId) {
        KillTimer(nullptr, g_keypressTimerId);
        g_keypressTimerId = 0;
    }

    if (AreModifierKeysPressed()) {
        // Defer keypress - store and start thread timer
        g_pendingKeypressKeys = keys;
        g_pendingKeypressRetryCount = 0;
        g_keypressTimerId = SetTimer(nullptr, 0, kKeypressRetryIntervalMs,
                                     KeypressRetryTimerProc);
        Wh_Log(L"Modifier keys pressed, deferring keypress");
        return;
    }

    SendKeypressInternal(keys);
}

void SendCtrlAltTabKeypress() {
    Wh_Log(L"Sending Ctrl+Alt+Tab");
    SendKeypress({VK_LCONTROL, VK_LMENU, VK_TAB});
}

void SendWinTabKeypress() {
    Wh_Log(L"Sending Win+Tab");
    SendKeypress({VK_LWIN, VK_TAB});
}

void OpenStartMenu() {
    Wh_Log(L"Sending Win keypress for Start menu");
    SendKeypress({VK_LWIN});
}

std::vector<int> ParseVirtualKeypressSetting(const std::wstring& args) {
    std::vector<int> keys;

    const auto argsSplit = SplitArgs(args);
    for (const auto& arg : argsSplit) {
        UINT modifiers = 0;
        UINT vk = 0;
        if (FromStringHotKey(arg, &modifiers, &vk)) {
            // Add modifier keys first
            if (modifiers & MOD_CONTROL) {
                keys.push_back(VK_LCONTROL);
            }
            if (modifiers & MOD_ALT) {
                keys.push_back(VK_LMENU);
            }
            if (modifiers & MOD_SHIFT) {
                keys.push_back(VK_LSHIFT);
            }
            if (modifiers & MOD_WIN) {
                keys.push_back(VK_LWIN);
            }
            // Add the main key
            keys.push_back(static_cast<int>(vk));
        } else {
            Wh_Log(L"Failed to parse key: %s", arg.c_str());
        }
    }

    return keys;
}

void MediaPlayPause() {
    Wh_Log(L"Sending Media Play/Pause");
    SendKeypress({VK_MEDIA_PLAY_PAUSE});
}

void MediaNext() {
    Wh_Log(L"Sending Media Next Track");
    SendKeypress({VK_MEDIA_NEXT_TRACK});
}

void MediaPrev() {
    Wh_Log(L"Sending Media Previous Track");
    SendKeypress({VK_MEDIA_PREV_TRACK});
}

// Parses command line into executable path and parameters
std::tuple<std::wstring, std::wstring> ParseExecutableAndParameters(
    const std::wstring& command) {
    std::wstring executable = command;
    std::wstring parameters;

    if (stringtools::startsWith(command, L"\"") ||
        stringtools::startsWith(command, L"'")) {
        size_t closingQuotePos = command.find(command[0], 1);
        if (closingQuotePos != std::wstring::npos) {
            executable = command.substr(1, closingQuotePos - 1);
            if (command.length() > closingQuotePos + 1) {
                parameters = command.substr(closingQuotePos + 1);
            }
        }
    } else {
        std::vector<std::wstring> args = SplitArgs(command, L' ');
        if (args.size() > 1) {
            executable = L"";
            for (const auto& arg : args) {
                executable += arg;
                if (std::filesystem::path(executable)
                        .extension()
                        .wstring()
                        .length() > 1) {
                    break;
                } else {
                    executable += L" ";
                }
            }
            if (command.length() > executable.length()) {
                parameters = command.substr(executable.length());
            }
        }
    }
    return std::make_tuple(stringtools::trim(executable),
                           stringtools::trim(parameters));
}

void StartProcess(std::wstring command) {
    if (command.empty()) {
        return;
    }

    std::vector<std::wstring> uac_args = SplitArgs(command, L';');
    std::wstring verb = L"open";
    if (stringtools::toLower(uac_args[0]) == L"uac") {
        verb = L"runas";
        command = stringtools::ltrim(command.substr(uac_args[0].length() + 1));
    }

    const auto [executable, parameters] = ParseExecutableAndParameters(command);
    Wh_Log(L"Starting: %s %s", executable.c_str(), parameters.c_str());

    POINT cursorPos;
    GetCursorPos(&cursorPos);
    HMONITOR hMonitor = MonitorFromPoint(cursorPos, MONITOR_DEFAULTTONEAREST);

    SHELLEXECUTEINFO sei = {sizeof(sei)};
    sei.fMask = SEE_MASK_HMONITOR | SEE_MASK_NOASYNC | SEE_MASK_FLAG_NO_UI;
    sei.lpVerb = verb.c_str();
    sei.lpFile = executable.c_str();
    sei.lpParameters = parameters.empty() ? NULL : parameters.c_str();
    sei.nShow = SW_SHOWNORMAL;
    sei.hMonitor = hMonitor;

    if (!ShellExecuteEx(&sei)) {
        DWORD error = GetLastError();
        if (error != ERROR_CANCELLED) {
            Wh_Log(L"ShellExecuteEx failed, error: %d", error);
        }
    }
}

std::tuple<std::wstring, std::wstring> ParseCopyRunCommand(const std::wstring& command) {
    std::wstring verb = L"open";
    std::wstring arg = stringtools::trim(command);
    if (arg.empty()) {
        return {verb, L""};
    }

    std::vector<std::wstring> parts = SplitArgs(command, L';');
    if (stringtools::toLower(parts[0]) == L"uac") {
        verb = L"runas";
        arg = stringtools::ltrim(command.substr(parts[0].length() + 1));
    }

    arg = stringtools::trim(arg);
    if (arg.empty()) {
        return {verb, L""};
    }

    std::wstring lower = stringtools::toLower(arg);

    if (lower == L"gpt") return {verb, L"https://chatgpt.com/?q="};
    if (lower == L"yt") return {verb, L"https://www.youtube.com/results?search_query="};
    if (lower == L"reddit") return {verb, L"https://www.reddit.com/search/?q="};
    if (lower == L"google") return {verb, L"https://www.google.com/search?q="};
    if (lower == L"copilot") return {verb, L"https://copilot.microsoft.com/?sendquery=1&q="};
    if (lower == L"x") return {verb, L"https://twitter.com/search?q="};
    if (lower == L"translate") return {verb, L"https://translate.google.com/?text="};

    return {verb, arg};
}

void CopyRun(std::wstring command) {
    const auto [verb, base] = ParseCopyRunCommand(command);
    if (base.empty()) return;

    std::wstring clip;
    if (OpenClipboard(nullptr)) {
        if (HANDLE hData = GetClipboardData(CF_UNICODETEXT)) {
            if (const wchar_t* p =
                    static_cast<const wchar_t*>(GlobalLock(hData))) {
                clip = p;
                GlobalUnlock(hData);
            }
        }
        CloseClipboard();
    }
    if (clip.empty()) return;

    std::wstring url = base + clip;

    POINT pt{};
    GetCursorPos(&pt);
    HMONITOR mon = MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);

    SHELLEXECUTEINFOW sei{};
    sei.cbSize = sizeof(sei);
    sei.fMask = SEE_MASK_HMONITOR | SEE_MASK_NOASYNC | SEE_MASK_FLAG_NO_UI;
    sei.lpVerb = verb.c_str();  // ← same as StartProcess
    sei.lpFile = url.c_str();
    sei.nShow = SW_SHOWNORMAL;
    sei.hMonitor = mon;

    ShellExecuteExW(&sei);
}

bool GetForegroundWindowPathOrExe(std::wstring& outPath) {
    outPath.clear();

    HWND hWnd = GetForegroundWindow();
    if (!hWnd ||
        hWnd == g_hTaskbarWnd ||
        !IsWindow(hWnd) ||
        !IsWindowVisible(hWnd)) {
        return false;
    }

    auto isAbsolutePath = [](const std::wstring& s) {
        return
            (s.size() >= 3 &&
             iswalpha(s[0]) &&
             s[1] == L':' &&
             (s[2] == L'\\' || s[2] == L'/')) ||
            (s.rfind(L"\\\\", 0) == 0);
    };

    // 1) Try window title -> real file path
    int len = GetWindowTextLengthW(hWnd);
    if (len > 0) {
        std::wstring title(len + 1, L'\0');
        int copied = GetWindowTextW(hWnd, title.data(), len + 1);
        title.resize(copied > 0 ? copied : 0);

        std::wstring current = stringtools::trimDecorated(title);

        for (;;) {
            std::wstring candidate = stringtools::trimDecorated(current);

            if (isAbsolutePath(candidate)) {
                std::error_code ec;
                if (std::filesystem::is_regular_file(candidate, ec)) {
                    outPath = std::move(candidate);
                    return true;
                }
            }

            size_t sep = current.rfind(L" - ");
            if (sep == std::wstring::npos) {
                break;
            }

            current.resize(sep);
        }
    }

    // 2) Fallback -> process executable
    DWORD pid = 0;
    if (!GetWindowThreadProcessId(hWnd, &pid) || pid == 0) {
        return false;
    }

    HANDLE hProc = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!hProc) {
        return false;
    }

    std::wstring exe(32768, L'\0');
    DWORD size = static_cast<DWORD>(exe.size());

    if (QueryFullProcessImageNameW(hProc, 0, exe.data(), &size) && size > 0) {
        exe.resize(size);

        std::error_code ec;
        if (std::filesystem::is_regular_file(exe, ec)) {
            CloseHandle(hProc);
            outPath = std::move(exe);
            return true;
        }
    }

    CloseHandle(hProc);
    return false;
}

void SelectActiveInExplorer() {
    std::wstring path;
    if (!GetForegroundWindowPathOrExe(path)) {
        Wh_Log(L"SelectActiveInExplorer: could not resolve path");
        return;
    }

    Wh_Log(L"SelectActiveInExplorer: %s", path.c_str());

    std::wstring explorerArgs = L"/select,\"" + path + L"\"";

    POINT cursorPos;
    GetCursorPos(&cursorPos);
    HMONITOR hMonitor = MonitorFromPoint(cursorPos, MONITOR_DEFAULTTONEAREST);

    SHELLEXECUTEINFO sei = { sizeof(sei) };
    sei.fMask = SEE_MASK_HMONITOR | SEE_MASK_NOASYNC | SEE_MASK_FLAG_NO_UI;
    sei.lpVerb = L"open";
    sei.lpFile = L"explorer.exe";
    sei.lpParameters = explorerArgs.c_str();
    sei.nShow = SW_SHOWNORMAL;
    sei.hMonitor = hMonitor;

    if (!ShellExecuteEx(&sei)) {
        DWORD err = GetLastError();
        if (err != ERROR_CANCELLED) {
            Wh_Log(L"SelectActiveInExplorer: launch failed (%lu)", err);
        }
    }
}

void OpacityUp() {
    HWND hWnd = GetForegroundWindow();
    if (!hWnd ||
        hWnd == g_hTaskbarWnd ||
        !IsWindow(hWnd) ||
        !IsWindowVisible(hWnd)) {
        return;
    }

    LONG_PTR ex = GetWindowLongPtrW(hWnd, GWL_EXSTYLE);
    const bool isLayered = (ex & WS_EX_LAYERED) != 0;

    auto resetOpacity = [&]() {
        Wh_Log(L"OpacityUp: reset to full opacity");
        SetWindowLongPtrW(hWnd, GWL_EXSTYLE, ex & ~WS_EX_LAYERED);
    };

    static const BYTE kLevels[] = {0, 56, 85, 113, 141, 170, 198, 226};

    if (!isLayered) {
        // Already at full opacity, nothing to increase.
        return;
    }

    BYTE cur = 255;
    DWORD flags = 0;
    if (!GetLayeredWindowAttributes(hWnd, nullptr, &cur, &flags) ||
        !(flags & LWA_ALPHA)) {
        return;
    }

    if (cur >= 226) {
        resetOpacity();
        return;
    }

    for (BYTE level : kLevels) {
        if (cur < level) {
            SetLayeredWindowAttributes(hWnd, 0, level, LWA_ALPHA);
            return;
        }
    }

    resetOpacity();
}

void OpacityDown() {
    HWND hWnd = GetForegroundWindow();
    if (!hWnd ||
        hWnd == g_hTaskbarWnd ||
        !IsWindow(hWnd) ||
        !IsWindowVisible(hWnd)) {
        return;
    }

    LONG_PTR ex = GetWindowLongPtrW(hWnd, GWL_EXSTYLE);
    const bool isLayered = (ex & WS_EX_LAYERED) != 0;

    static const BYTE kLevels[] = {226, 198, 170, 141, 113, 85, 56, 0};

    if (!isLayered) {
        Wh_Log(L"OpacityDown: starting at 226");
        SetWindowLongPtrW(hWnd, GWL_EXSTYLE, ex | WS_EX_LAYERED);
        SetLayeredWindowAttributes(hWnd, 0, 226, LWA_ALPHA);
        return;
    }

    BYTE cur = 255;
    DWORD flags = 0;
    if (!GetLayeredWindowAttributes(hWnd, nullptr, &cur, &flags) ||
        !(flags & LWA_ALPHA)) {
        return;
    }

    if (cur > 226) {
        SetLayeredWindowAttributes(hWnd, 0, 226, LWA_ALPHA);
        return;
    }

    for (BYTE level : kLevels) {
        if (cur > level) {
            SetLayeredWindowAttributes(hWnd, 0, level, LWA_ALPHA);
            return;
        }
    }

    SetLayeredWindowAttributes(hWnd, 0, 0, LWA_ALPHA);
}

// Toggles always-on-top state of foreground window
void ToggleAlwaysOnTop() {
    HWND hWnd = GetForegroundWindow();
    if (!hWnd ||
        hWnd == g_hTaskbarWnd ||
        !IsWindow(hWnd) ||
        !IsWindowVisible(hWnd)) {
        return;
    }

    bool topmost = (GetWindowLongPtrW(hWnd, GWL_EXSTYLE) & WS_EX_TOPMOST) != 0;

    SetWindowPos(hWnd,
                 topmost ? HWND_NOTOPMOST : HWND_TOPMOST,
                 0, 0, 0, 0,
                SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
    Wh_Log(L"ToggleAlwaysOnTop: %s -> %s",
           topmost ? L"topmost" : L"normal",
           topmost ? L"normal" : L"topmost");
}

void ForceKillActiveWindow() {
    HWND hWnd = GetForegroundWindow();
    if (!hWnd ||
        hWnd == g_hTaskbarWnd ||
        !IsWindow(hWnd) ||
        !IsWindowVisible(hWnd)) {
        return;
    }

    DWORD pid = 0;
    if (!GetWindowThreadProcessId(hWnd, &pid) ||
        pid == 0 ||
        pid == GetCurrentProcessId()) {
        return;
    }

    HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, pid);
    if (hProcess) {
        TerminateProcess(hProcess, 1);
        CloseHandle(hProcess);
        Wh_Log(L"ForceKill: terminated PID %lu", pid);
    } else {
        Wh_Log(L"ForceKill: OpenProcess failed for PID %lu (UAC?)", pid);
    }
}

// Returns mute state of default audio device
BOOL IsAudioMuted(com_ptr<IMMDeviceEnumerator> pDeviceEnumerator) {
    const GUID XIID_IAudioEndpointVolume = {
        0x5CDF2C82,
        0x841E,
        0x4546,
        {0x97, 0x22, 0x0C, 0xF7, 0x40, 0x78, 0x22, 0x9A}};

    BOOL isMuted = FALSE;
    com_ptr<IMMDevice> defaultAudioDevice;
    if (SUCCEEDED(pDeviceEnumerator->GetDefaultAudioEndpoint(
            eRender, eConsole, defaultAudioDevice.put()))) {
        com_ptr<IAudioEndpointVolume> endpointVolume;
        if (SUCCEEDED(defaultAudioDevice->Activate(
                XIID_IAudioEndpointVolume, CLSCTX_INPROC_SERVER, NULL,
                endpointVolume.put_void()))) {
            endpointVolume->GetMute(&isMuted);
        }
    }
    return isMuted;
}

// Toggles mute state for all active audio devices
void ToggleVolMuted() {
    if (!g_audioCOM.IsInitialized()) {
        if (!g_audioCOM.Init()) {
            Wh_Log(L"Failed to initialize audio COM");
            return;
        }
    }

    auto pDeviceEnumerator = g_audioCOM.GetDeviceEnumerator();
    if (!pDeviceEnumerator) {
        Wh_Log(L"Failed to get device enumerator");
        return;
    }

    Wh_Log(L"Toggling volume mute");

    const GUID XIID_IAudioEndpointVolume = {
        0x5CDF2C82,
        0x841E,
        0x4546,
        {0x97, 0x22, 0x0C, 0xF7, 0x40, 0x78, 0x22, 0x9A}};

    const BOOL isMuted = IsAudioMuted(pDeviceEnumerator);

    com_ptr<IMMDeviceCollection> pDeviceCollection;
    if (FAILED(pDeviceEnumerator->EnumAudioEndpoints(
            eRender, DEVICE_STATE_ACTIVE, pDeviceCollection.put()))) {
        return;
    }

    UINT deviceCount = 0;
    if (FAILED(pDeviceCollection->GetCount(&deviceCount))) {
        return;
    }

    for (UINT i = 0; i < deviceCount; i++) {
        com_ptr<IMMDevice> pDevice;
        if (SUCCEEDED(pDeviceCollection->Item(i, pDevice.put()))) {
            com_ptr<IAudioEndpointVolume> endpointVolume;
            if (SUCCEEDED(pDevice->Activate(XIID_IAudioEndpointVolume,
                                            CLSCTX_INPROC_SERVER, NULL,
                                            endpointVolume.put_void()))) {
                endpointVolume->SetMute(!isMuted, NULL);
            }
        }
    }
}

HotkeyRegionName TryParseRegionName(const std::wstring& raw) {
    static const std::unordered_map<std::wstring, HotkeyRegionName> regionMap = 
        {
            {L"top_left", HotkeyRegionName::TopLeft},
            {L"top", HotkeyRegionName::Top},
            {L"top_right", HotkeyRegionName::TopRight},
            {L"left", HotkeyRegionName::Left},
            {L"elsewhere", HotkeyRegionName::Elsewhere},
            {L"right", HotkeyRegionName::Right},
            {L"bottom_left", HotkeyRegionName::BottomLeft},
            {L"bottom", HotkeyRegionName::Bottom},
            {L"bottom_right", HotkeyRegionName::BottomRight},
        };

        auto normalized = stringtools::toLower(stringtools::trim(raw));
        auto it = regionMap.find(normalized);
        if (it != regionMap.end()) {
            return it->second;
        }

        return HotkeyRegionName::Invalid;
    }

// Parses action string to enum
HotkeyActionType TryParseActionType(const std::wstring& raw) {
    static const std::unordered_map<std::wstring, HotkeyActionType> actionMap =
        {
            {L"action_nothing", HotkeyActionType::Nothing},
            {L"action_show_desktop", HotkeyActionType::ShowDesktop},
            {L"action_alt_tab", HotkeyActionType::AltTab},
            {L"action_task_manager", HotkeyActionType::TaskManager},
            {L"action_mute", HotkeyActionType::Mute},
            {L"action_taskbar_autohide", HotkeyActionType::TaskbarAutohide},
            {L"action_win_tab", HotkeyActionType::WinTab},
            {L"action_hide_icons", HotkeyActionType::HideIcons},
            {L"action_combine_taskbar_buttons",
             HotkeyActionType::CombineTaskbarButtons},
            {L"action_toggle_taskbar_alignment",
             HotkeyActionType::ToggleTaskbarAlignment},
            {L"action_open_start_menu", HotkeyActionType::OpenStartMenu},
            {L"action_send_keypress", HotkeyActionType::SendKeypress},
            {L"action_start_process", HotkeyActionType::StartProcess},
            {L"action_copyrun", HotkeyActionType::CopyRun},
            {L"action_select_active_in_explorer",
             HotkeyActionType::SelectActiveInExplorer},
            {L"action_opacity_up_active", HotkeyActionType::OpacityUp},
            {L"action_opacity_down_active", HotkeyActionType::OpacityDown},
            {L"action_toggle_alwaysontop_active",
             HotkeyActionType::ToggleAlwaysOnTop},
            {L"action_force_kill_active", HotkeyActionType::ForceKillActive},
            {L"action_media_play_pause", HotkeyActionType::MediaPlayPause},
            {L"action_media_next", HotkeyActionType::MediaNext},
            {L"action_media_prev", HotkeyActionType::MediaPrev},
        };

    auto normalized = stringtools::toLower(stringtools::trim(raw));
    auto it = actionMap.find(normalized);
    if (it != actionMap.end()) {
        return it->second;
    }

    return HotkeyActionType::Invalid;
}

const wchar_t* RegionNameToString(HotkeyRegionName region) {
    switch (region) {
        case HotkeyRegionName::TopLeft:
            return L"TOP_LEFT";
        case HotkeyRegionName::Top:
            return L"TOP";
        case HotkeyRegionName::TopRight:
            return L"TOP_RIGHT";
        case HotkeyRegionName::Left:
            return L"LEFT";
        case HotkeyRegionName::Elsewhere:
            return L"ELSEWHERE";
        case HotkeyRegionName::Right:
            return L"RIGHT";
        case HotkeyRegionName::BottomLeft:
            return L"BOTTOM_LEFT";
        case HotkeyRegionName::Bottom:
            return L"BOTTOM";
        case HotkeyRegionName::BottomRight:
            return L"BOTTOM_RIGHT";
        case HotkeyRegionName::Invalid:
            return L"Invalid";
    }
   return L"Unknown";
}

// Converts action enum to string for logging
const wchar_t* ActionTypeToString(HotkeyActionType actionType) {
    switch (actionType) {
        case HotkeyActionType::Nothing:
            return L"Nothing";
        case HotkeyActionType::ShowDesktop:
            return L"ShowDesktop";
        case HotkeyActionType::AltTab:
            return L"AltTab";
        case HotkeyActionType::TaskManager:
            return L"TaskManager";
        case HotkeyActionType::Mute:
            return L"Mute";
        case HotkeyActionType::TaskbarAutohide:
            return L"TaskbarAutohide";
        case HotkeyActionType::WinTab:
            return L"WinTab";
        case HotkeyActionType::HideIcons:
            return L"HideIcons";
        case HotkeyActionType::CombineTaskbarButtons:
            return L"CombineTaskbarButtons";
        case HotkeyActionType::ToggleTaskbarAlignment:
            return L"ToggleTaskbarAlignment";
        case HotkeyActionType::OpenStartMenu:
            return L"OpenStartMenu";
        case HotkeyActionType::SendKeypress:
            return L"SendKeypress";
        case HotkeyActionType::StartProcess:
            return L"StartProcess";
        case HotkeyActionType::CopyRun:
            return L"CopyRun";
        case HotkeyActionType::SelectActiveInExplorer:
            return L"SelectActiveInExplorer";
        case HotkeyActionType::OpacityUp:
            return L"OpacityUp";
        case HotkeyActionType::OpacityDown:
            return L"OpacityDown";
        case HotkeyActionType::ToggleAlwaysOnTop:
            return L"ToggleAlwaysOnTop";
        case HotkeyActionType::ForceKillActive:
            return L"ForceKillActive";
        case HotkeyActionType::MediaPlayPause:
            return L"MediaPlayPause";
        case HotkeyActionType::MediaNext:
            return L"MediaNext";
        case HotkeyActionType::MediaPrev:
            return L"MediaPrev";
        case HotkeyActionType::Invalid:
            return L"Invalid";
    }
   return L"Unknown";
}

// Creates action executor from action type and arguments
std::function<void()> ParseActionSetting(HotkeyActionType actionType,
                                         const std::wstring& args) {
    switch (actionType) {
        case HotkeyActionType::Nothing:
            return []() {};
        case HotkeyActionType::ShowDesktop:
            return []() { ShowDesktop(); };
        case HotkeyActionType::AltTab:
            return []() { SendCtrlAltTabKeypress(); };
        case HotkeyActionType::TaskManager:
            return []() { OpenTaskManager(); };
        case HotkeyActionType::Mute:
            return []() { ToggleVolMuted(); };
        case HotkeyActionType::TaskbarAutohide:
            return []() { ToggleTaskbarAutohide(); };
        case HotkeyActionType::WinTab:
            return []() { SendWinTabKeypress(); };
        case HotkeyActionType::HideIcons:
            return []() { HideIcons(); };
        case HotkeyActionType::CombineTaskbarButtons: {
            const auto [s1, s2, s3, s4] = ParseTaskBarButtonsState(args);
            return
                [s1, s2, s3, s4]() { CombineTaskbarButtons(s1, s2, s3, s4); };
       }
        case HotkeyActionType::ToggleTaskbarAlignment:
            return []() { ToggleTaskbarAlignment(); };
        case HotkeyActionType::OpenStartMenu:
            return []() { OpenStartMenu(); };
        case HotkeyActionType::SendKeypress: {
            const auto keyCodes = ParseVirtualKeypressSetting(args);
            return [keyCodes]() {
                Wh_Log(L"Sending %zu keypresses", keyCodes.size());
                SendKeypress(keyCodes);
            };
        }
        case HotkeyActionType::StartProcess: {
            std::wstring cmd = stringtools::trim(args);
            return [cmd]() { StartProcess(cmd); };
        }
        case HotkeyActionType::CopyRun: {
            std::wstring cmd = stringtools::trim(args);
            return [cmd]() { CopyRun(cmd); };
        }
        case HotkeyActionType::SelectActiveInExplorer:
            return []() { SelectActiveInExplorer(); };
        case HotkeyActionType::OpacityUp:
            return []() { OpacityUp(); };
        case HotkeyActionType::OpacityDown:
            return []() { OpacityDown(); };
        case HotkeyActionType::ToggleAlwaysOnTop:
            return []() { ToggleAlwaysOnTop(); };
        case HotkeyActionType::ForceKillActive:
            return []() { ForceKillActiveWindow(); };
        case HotkeyActionType::MediaPlayPause:
            return []() { MediaPlayPause(); };
        case HotkeyActionType::MediaNext:
            return []() { MediaNext(); };
        case HotkeyActionType::MediaPrev:
            return []() { MediaPrev(); };
        case HotkeyActionType::Invalid:
            return []() {};
    }
    return []() {};
}
#pragma endregion  // actions

// =====================================================================

#pragma region taskbar_subclass

LRESULT CALLBACK TaskbarWindowSubclassProc(_In_ HWND hWnd,
                                           _In_ UINT uMsg,
                                           _In_ WPARAM wParam,
                                           _In_ LPARAM lParam,
                                           _In_ DWORD_PTR dwRefData) {
    switch (uMsg) {
        case WM_HOTKEY: {
            int hotkeyId = static_cast<int>(wParam);

            auto hotkeyIt = std::find_if(
                g_settings.hotkeyActions.begin(),
                g_settings.hotkeyActions.end(),
                [&](const HotkeyBinding& e) {
                    return e.hotkeyId == hotkeyId;
                });
            if (hotkeyIt == g_settings.hotkeyActions.end()) {
                break;
            }

            auto tryFire = [&](HotkeyRegionName r) -> bool {
                auto it = std::find_if(
                    g_settings.hotkeyActions.begin(),
                    g_settings.hotkeyActions.end(),
                    [&](const HotkeyBinding& e) {
                        return e.hotkeyId == hotkeyId && e.region == r;
                    });
                if (it == g_settings.hotkeyActions.end()) return false;
                Wh_Log(L"Hotkey '%s' fired in region %s, executing %s",
                       it->hotkeyString.c_str(),
                       RegionNameToString(r),
                       ActionTypeToString(it->actionType));
                it->actionExecutor();
                return true;
            };

            constexpr LONG kEdgeMargin = 12;
            POINT cursorPos{};
            GetCursorPos(&cursorPos);
            HMONITOR hMonitor = MonitorFromPoint(cursorPos, MONITOR_DEFAULTTONEAREST);
            MONITORINFO monitorInfo{};
            monitorInfo.cbSize = sizeof(monitorInfo);
            GetMonitorInfo(hMonitor, &monitorInfo);
            const auto& r = monitorInfo.rcMonitor;
            const bool atTop    = cursorPos.y - r.top    < kEdgeMargin;
            const bool atBottom = r.bottom - cursorPos.y <= kEdgeMargin;
            const bool atLeft   = cursorPos.x - r.left   < kEdgeMargin;
            const bool atRight  = r.right - cursorPos.x  <= kEdgeMargin;

            bool fired = false;
            if (atTop && atLeft) {
                fired = tryFire(HotkeyRegionName::TopLeft)
                     || tryFire(HotkeyRegionName::Top)
                     || tryFire(HotkeyRegionName::Left);
            } else if (atTop && atRight) {
                fired = tryFire(HotkeyRegionName::TopRight)
                     || tryFire(HotkeyRegionName::Top)
                     || tryFire(HotkeyRegionName::Right);
            } else if (atBottom && atLeft) {
                fired = tryFire(HotkeyRegionName::BottomLeft)
                     || tryFire(HotkeyRegionName::Bottom)
                     || tryFire(HotkeyRegionName::Left);
            } else if (atBottom && atRight) {
                fired = tryFire(HotkeyRegionName::BottomRight)
                     || tryFire(HotkeyRegionName::Bottom)
                     || tryFire(HotkeyRegionName::Right);
            } else if (atTop)    { fired = tryFire(HotkeyRegionName::Top); }
            else if (atBottom)   { fired = tryFire(HotkeyRegionName::Bottom); }
            else if (atLeft)     { fired = tryFire(HotkeyRegionName::Left); }
            else if (atRight)    { fired = tryFire(HotkeyRegionName::Right); }
            if (!fired) tryFire(HotkeyRegionName::Elsewhere);
            return 0;
        }

        default:
            if (uMsg == g_hotkeyRegisteredMsg) {
                switch (wParam) {
                    case HOTKEY_REGISTER:
                        RegisterHotkeys(hWnd);
                        break;
                    case HOTKEY_UNREGISTER:
                        UnregisterHotkeys(hWnd);
                        break;
                }
                return 0;
            }

            if (uMsg == g_uninitCOMMsg) {
                g_audioCOM.Uninit();
                return 0;
            }
            break;
    }

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

void HandleIdentifiedTaskbarWindow(HWND hWnd) {
    g_hTaskbarWnd = hWnd;
    g_dwTaskbarThreadId = GetWindowThreadProcessId(hWnd, nullptr);
    if (WindhawkUtils::SetWindowSubclassFromAnyThread(
            hWnd, TaskbarWindowSubclassProc, 0)) {
        Wh_Log(L"Taskbar window %p subclassed", hWnd);
    } else {
        Wh_Log(L"Failed to subclass taskbar window %p", hWnd);
    }
}

#pragma endregion  // taskbar_subclass

// =====================================================================

#pragma region window_hooks

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
        Wh_Log(L"Shell_TrayWnd created: %p", hWnd);
        HandleIdentifiedTaskbarWindow(hWnd);
        RegisterHotkeys(hWnd);
    }

    return hWnd;
}

// Finds main taskbar window in current process
HWND FindCurrentProcessTaskbarWindow() {
    HWND hWnd = nullptr;

    EnumWindows(
        [](HWND hWnd, LPARAM lParam) WINAPI_LAMBDA_RETURN(BOOL) {
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
                *(HWND*)lParam = hWnd;
                return FALSE;
            }

            return TRUE;
        },
        (LPARAM)&hWnd);

    return hWnd;
}

#pragma endregion  // window_hooks

// =====================================================================

#pragma region settings

void LoadSettings() {
    using WindhawkUtils::StringSetting;

    g_settings.hotkeyActions.clear();

    std::unordered_map<uint64_t, int> seenCombos;

    for (int i = 0; i < 50; i++) {
        auto hotkeyStr = stringtools::trim(std::wstring(
            StringSetting::make(L"HotkeyActions[%d].Hotkey", i).get()));

        if (hotkeyStr.empty()) {
            break;
        }

        UINT modifiers = 0, vk = 0;
        if (!FromStringHotKey(hotkeyStr, &modifiers, &vk)) {
            Wh_Log(L"Hotkey[%d] '%s': failed to parse, skipping group",
                   i, hotkeyStr.c_str());
            continue;
        }

        uint64_t comboKey = (static_cast<uint64_t>(modifiers) << 32) | vk;
        auto existing = seenCombos.find(comboKey);
        if (existing != seenCombos.end()) {
            Wh_Log(L"Hotkey[%d] '%s': duplicate combo (first at group %d), skipping group",
                   i, hotkeyStr.c_str(), existing->second);
            continue;
        }
        seenCombos[comboKey] = i;

        size_t startIdx = g_settings.hotkeyActions.size();

        for (int j = 0; j < 10; j++) {
            auto regionStr = stringtools::trim(std::wstring(
                StringSetting::make(L"HotkeyActions[%d].Regions[%d].Region", i, j)
                    .get()));
            auto actionStrRaw = stringtools::trim(std::wstring(
                StringSetting::make(L"HotkeyActions[%d].Regions[%d].Action", i, j)
                    .get()));

            if (regionStr.empty() && actionStrRaw.empty()) {
                break;
            }

            auto regionName = TryParseRegionName(regionStr);
            if (regionName == HotkeyRegionName::Invalid) {
                Wh_Log(L"Hotkey[%d] Region[%d]: unknown region '%s', skipping row",
                       i, j, regionStr.c_str());
                continue;
            }

            auto it = std::find_if(
                g_settings.hotkeyActions.begin() + startIdx,
                g_settings.hotkeyActions.end(),
                [&](const HotkeyBinding& e) {
                    return e.region == regionName;
                });

            if (it != g_settings.hotkeyActions.end()) {
                Wh_Log(L"Hotkey[%d] Region[%d]: duplicate region '%s', first row wins, skipping",
                       i, j, RegionNameToString(regionName));
                continue;
            }
            auto actionType = TryParseActionType(actionStrRaw);
            if (actionType == HotkeyActionType::Invalid) {
                Wh_Log(L"Hotkey[%d] Region[%d]: unknown action '%s', skipping row",
                       i, j, actionStrRaw.c_str());
                continue;
            }

            if (actionType == HotkeyActionType::Nothing) {
                continue;
            }

            auto argsStr = stringtools::trim(std::wstring(
                StringSetting::make(
                    L"HotkeyActions[%d].Regions[%d].AdditionalArgs", i, j)
                    .get()));

            HotkeyBinding entry;
            entry.hotkeyString = hotkeyStr;
            entry.region = regionName;
            entry.actionType = actionType;
            entry.additionalArgs = argsStr;
            entry.actionExecutor = ParseActionSetting(actionType, argsStr);
            entry.modifiers = modifiers;
            entry.vk = vk;
            entry.hotkeyId = 0;
            entry.ownsRegistration = false;

            g_settings.hotkeyActions.push_back(std::move(entry));
        }

        if (g_settings.hotkeyActions.size() == startIdx) {
            Wh_Log(L"Hotkey[%d] '%s': no non-Nothing region rows configured, skipping",
                   i, hotkeyStr.c_str());
            continue;
        }

        Wh_Log(L"Loaded hotkey[%d]: '%s' with %zu region(s)", i,
               hotkeyStr.c_str(), g_settings.hotkeyActions.size() - startIdx);
    }
}
#pragma endregion  // settings

// =====================================================================

#pragma region windhawk_exports

BOOL Wh_ModInit() {
    Wh_Log(L">");

    LoadSettings();

    if (!Wh_SetFunctionHook((void*)CreateWindowExW, (void*)CreateWindowExW_Hook,
                            (void**)&CreateWindowExW_Original)) {
        Wh_Log(L"Failed to hook CreateWindowExW");
        return FALSE;
    }

    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L">");

    WNDCLASS wndclass;
    if (GetClassInfo(GetModuleHandle(NULL), L"Shell_TrayWnd", &wndclass)) {
        HWND hWnd = FindCurrentProcessTaskbarWindow();
        if (hWnd) {
            HandleIdentifiedTaskbarWindow(hWnd);
        }
    }

    if (g_hTaskbarWnd) {
        SendMessage(g_hTaskbarWnd, g_hotkeyRegisteredMsg, HOTKEY_REGISTER, 0);
    }
}

void Wh_ModBeforeUninit() {
    Wh_Log(L">");

    if (g_hTaskbarWnd) {
        SendMessage(g_hTaskbarWnd, g_hotkeyRegisteredMsg, HOTKEY_UNREGISTER, 0);
    }
}

void Wh_ModUninit() {
    Wh_Log(L">");

    if (g_hTaskbarWnd) {
        SendMessage(g_hTaskbarWnd, g_uninitCOMMsg, 0, 0);
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(
            g_hTaskbarWnd, TaskbarWindowSubclassProc);
    }
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");

    // Unregister old hotkeys before loading new settings
    if (g_hTaskbarWnd) {
        SendMessage(g_hTaskbarWnd, g_hotkeyRegisteredMsg, HOTKEY_UNREGISTER, 0);
    }

    LoadSettings();

    if (g_hTaskbarWnd) {
        SendMessage(g_hTaskbarWnd, g_hotkeyRegisteredMsg, HOTKEY_REGISTER, 0);
    }
}

#pragma endregion  // windhawk_exports
