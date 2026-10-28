// ==WindhawkMod==
// @id              snap-commander
// @name            Snap Commander
// @description     Move the active window to screen halves and corners with keyboard shortcuts (inspired by Rectangle on macOS)
// @version         1.0.0
// @author          Asteski
// @github          https://github.com/Asteski
// @include         windhawk.exe
// @compilerOptions -luser32 -ldwmapi
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Snap Commander

Move the active window to specific screen areas using keyboard shortcuts, just like
the [Rectangle](https://rectangleapp.com/) app on macOS.

![snap-commander](https://raw.githubusercontent.com/Asteski/Windhawk-Mods/refs/heads/main/img/snap-commander/snap-commander.gif)

## Default shortcuts

| Action          | Shortcut    |
| --------------- | ----------- |
| Left half       | `Alt` + `Q` |
| Right half      | `Alt` + `W` |
| Top half        | `Alt` + `E` |
| Bottom half     | `Alt` + `R` |
| Center half     | `Alt` + `T` |
| Top-left        | `Alt` + `U` |
| Top-right       | `Alt` + `I` |
| Bottom-left     | `Alt` + `J` |
| Bottom-right    | `Alt` + `K` |
| Maximize        | `Alt` + `H` |
| Almost maximize | `Alt` + `Y` |
| Center          | `Alt` + `G` |
| Minimize        | `Alt` + `M` |
| Next display    | `Alt` + `]` |
| Previous display| `Alt` + `[` |
| Make smaller    | `Alt` + `-` |
| Make larger     | `Alt` + `+` |
| Restore         | `Alt` + `Z` |

*Maximize* fills the work area ignoring gaps; *almost maximize* fills it while keeping
the configured gaps. *Center* re-centers the active window without resizing it (unless a
center width/height is set). *Next/previous display* move the window to the adjacent
monitor (keeping its relative size and position, wrapping around). *Make smaller/larger*
shrink or grow the window about its center by the configured resize step. *Restore*
returns the window to where it was before its first snap.

## How it works

Each action is bound to a keyboard shortcut you type as text. The mod registers these as
global hotkeys (via `RegisterHotKey`), so when you press one, the focused window is
resized and moved to the matching region of the monitor it is on. The whole combination
is consumed cleanly, so it never reaches the focused application and leaves no stray
modifier behind (no menu-bar or Start-menu flicker).

## Shortcuts

Each action's shortcut is free text in the form `[modifier+]...key`, for example:

- `q` - just the `Q` key
- `ctrl+q` - `Ctrl` + `Q`
- `ctrl+shift+left` - `Ctrl` + `Shift` + `Left arrow`

Recognised modifier words are `alt`, `ctrl`, `shift`, and `win`; the key may be a letter
`a`-`z`, a digit `0`-`9`, an arrow `left`/`right`/`up`/`down`, or one of the punctuation
keys `[`, `]`, `-`. Tokens are separated by `+` and are case-insensitive. Because `+` is
the separator, the `=`/`+` key must be written as `plus` (or `=`).

The **Modifier key** setting adds a modifier (or two) on top of whatever you type for
every action. So with Modifier = `Alt` and Left half = `q`, the shortcut is `Alt+Q`. Set
Modifier = `None` to use only the modifiers written in each shortcut - letting you give
different actions different modifiers (e.g. `q`, `ctrl+w`, `ctrl+shift+e`).

The modifier combination must match exactly: an action bound to `Alt+Q` will *not* fire
on `Ctrl+Alt+Q`. Note that binding a bare key (e.g. `q` with Modifier = `None`) means
that key is claimed system-wide and can no longer be typed normally.

Each shortcut can have only one global owner, so a combination already claimed by another
app (or a reserved one such as `Win`+`L`) can't be registered. When that happens it is
noted in the mod's log and the shortcut is simply skipped.

## Other configuration

- **Gaps**: optional space (in pixels) applied to each side of the snapped window
  (top, bottom, left, right). All default to `0`, so windows sit flush by default.
- **Gap distribution**: controls how the gap is shared between adjacent windows.
  *Even spacing* (default) puts the full gap on sides touching a screen edge and half the
  gap on shared edges, so two windows placed side by side show the same gap between them
  as a single window shows against the screen edge - no doubling. *Full gap on every side*
  applies the whole gap to each side (adjacent windows then show a doubled gap). *Screen
  edges only* gaps just the sides that touch a screen edge, so adjacent windows meet flush.
- **Center size**: optional width/height (in pixels) for the Center action. Leave either
  empty to keep the window's current width or height.

## Auto-snap rules

Beyond keyboard shortcuts, you can have the mod snap certain apps automatically. Each
**Auto-snap rule** pairs one or more executable names with an action: when a window of a
matching process is shown, that action is applied to it. Rules also run against
already-open windows when you save settings, so adding a rule snaps matching windows that
are already on screen.

Executable names are case-insensitive and the `.exe` suffix is optional; list several in
one field separated by `,`, `;`, or `|` (e.g. `notepad; calc.exe`). The available
actions are the same region actions as the shortcuts (Maximize, halves, corners, etc.);
*Restore* is intentionally not offered, since a freshly opened window has nothing to
restore to.

Enable a rule's **Don't resize** toggle to move the window into the chosen region while
keeping its current size - the unresized window is anchored to the matching corner or
side of the region (e.g. *top-right* pins it to the region's top-right). This is ignored
by the Minimize action.

## ⚠ Important usage note ⚠

In order to use this mod with elevated processes, it can be configured to target
`dwm.exe`.

First, navigate to Windhawk's Settings > Advanced settings > More advanced
settings > Process inclusion list, and make sure that `dwm.exe` is in the list.

![Advanced settings screenshot](https://i.imgur.com/LRhREtJ.png)

Next, go to the mod's advanced tab, enable this option: "Ignore mod
inclusion/exclusion lists", then add `dwm.exe` to the custom process inclusion
list.

*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- modifier: alt
  $name: Modifier key
  $description: >-
    Added to every action's shortcut, on top of any modifiers typed into the action
    itself. Choose None to use only the modifiers written in each shortcut.
  $options:
    - none: None
    - alt: Alt
    - ctrl: Ctrl
    - shift: Shift
    - win: Win
    - alt_ctrl: Alt + Ctrl
    - alt_shift: Alt + Shift
    - ctrl_shift: Ctrl + Shift
    - win_alt: Win + Alt
    - win_ctrl: Win + Ctrl
    - win_shift: Win + Shift
- shortcuts:
    - keyLeftHalf: q
      $name: Left half
      $description: >-
        Shortcut text, e.g. "q", "ctrl+q", or "ctrl+shift+left". The Modifier key above is
        added on top. Leave empty to unbind. Same format for every action below.
    - keyRightHalf: w
      $name: Right half
    - keyTopHalf: e
      $name: Top half
    - keyBottomHalf: r
      $name: Bottom half
    - keyCenterHalf: t
      $name: Center half
      $description: A half-width strip in the middle of the screen (full height).
    - keyTopLeft: u
      $name: Top-left
    - keyTopRight: i
      $name: Top-right
    - keyBottomLeft: j
      $name: Bottom-left
    - keyBottomRight: k
      $name: Bottom-right
    - keyMaximize: h
      $name: Maximize
      $description: Fills the whole work area, ignoring any configured gaps.
    - keyAlmostMaximize: y
      $name: Almost-maximize
      $description: Fills the work area but keeps the configured gaps.
    - keyCenter: g
      $name: Center
      $description: Centers the window without changing its size (unless a size is set below).
    - keyMinimize: m
      $name: Minimize
      $description: Minimizes the active window.
    - keyNextDisplay: "]"
      $name: Next display
      $description: Moves the window to the next monitor.
    - keyPrevDisplay: "["
      $name: Previous display
      $description: Moves the window to the previous monitor.
    - keySmaller: "-"
      $name: Make smaller
      $description: Shrinks the window around its center.
    - keyLarger: plus
      $name: Make larger
      $description: >-
        Grows the window around its center. Because "+" is the shortcut separator,
        write this key as "plus" (or "="); it triggers on the unshifted "=" / "+" key.
    - keyRestore: z
      $name: Restore
      $description: Returns the window to where it was before the first snap.
  $name: Shortcuts
  $description: Keyboard shortcuts for each window action.
- dimensions:
    - gapTop: 0
      $name: Top gap (pixels)
      $description: Space left above a snapped window.
    - gapBottom: 0
      $name: Bottom gap (pixels)
      $description: Space left below a snapped window.
    - gapLeft: 0
      $name: Left gap (pixels)
      $description: Space left to the left of a snapped window.
    - gapRight: 0
      $name: Right gap (pixels)
      $description: Space left to the right of a snapped window.
    - gapMode: even
      $name: Gap distribution
      $description: >-
        How gaps are applied between adjacent windows. "Even spacing" halves the gap on
        shared edges so two windows side by side show the same gap as a single window
        against the screen edge (no doubling). "Full gap on every side" applies the whole
        gap to each window side, so adjacent windows show a doubled gap. "Screen edges
        only" gaps just the sides that touch a screen edge, so adjacent windows meet flush.
      $options:
        - even: Even spacing (no doubling)
        - full: Full gap on every side
        - screen: Screen edges only
    - centerWidth: ""
      $name: Center action width (pixels)
      $description: Width to use for the Center action. Leave empty to keep the window's current width.
    - centerHeight: ""
      $name: Center action height (pixels)
      $description: Height to use for the Center action. Leave empty to keep the window's current height.
    - resizeStep: 5
      $name: Resize step (%)
      $description: >-
        How much Make smaller / larger change the window each press, as a percentage
        of the monitor work area. Clamped to 1-50.
  $name: Dimensions
  $description: Gaps around snapped windows and sizes for the Center action.
- rules:
    - - executable_names: ""
        $name: Executable name(s)
        $description: >-
          One or more executable names. Separators: comma, semicolon, pipe.
          Case-insensitive; the ".exe" suffix is optional. Example:
          "notepad; calc.exe".
      - action: maximize
        $name: Action
        $options:
          - left-half: Left half
          - right-half: Right half
          - top-half: Top half
          - bottom-half: Bottom half
          - center-half: Center half
          - top-left: Top-left
          - top-right: Top-right
          - bottom-left: Bottom-left
          - bottom-right: Bottom-right
          - maximize: Maximize
          - almost-maximize: Almost maximize
          - center: Center
          - minimize: Minimize
      - no_resize: false
        $name: Don't resize
        $description: >-
          Move the window into the chosen region but keep its current size
          instead of resizing it to fill the region. Ignored by Minimize.
  $name: Auto-snap rules
  $description: >-
    Automatically apply an action to an app's windows when they open. Add one
    item per executable/action combination.
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>
#include <windows.h>
#include <dwmapi.h>

#include <algorithm>
#include <atomic>
#include <cstdlib>
#include <cwctype>
#include <deque>
#include <mutex>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

enum class Action {
    None,
    LeftHalf,
    RightHalf,
    TopHalf,
    BottomHalf,
    CenterHalf,
    TopLeft,
    TopRight,
    BottomLeft,
    BottomRight,
    Maximize,
    AlmostMaximize,
    Center,
    Minimize,
    NextDisplay,
    PreviousDisplay,
    MakeSmaller,
    MakeLarger,
    RestorePrevious,
};

constexpr UINT MOD_F_ALT = 0x1;
constexpr UINT MOD_F_CTRL = 0x2;
constexpr UINT MOD_F_SHIFT = 0x4;
constexpr UINT MOD_F_WIN = 0x8;

constexpr int ACTION_COUNT = 18;

// Posted to the hook thread after settings load, so it can re-register the hotkeys
// and install/remove the auto-snap WinEvent hook (both must run on that thread).
constexpr UINT WM_APP_SETTINGS_CHANGED = WM_APP + 1;

// Gaps (in pixels) left on each side of a snapped window.
static int g_gapTop = 0;
static int g_gapBottom = 0;
static int g_gapLeft = 0;
static int g_gapRight = 0;

// How the gap on a window side is sized depending on whether that side touches a
// screen edge (outer) or borders another tiled window (inner divider):
//   Even   - outer sides get the full gap, inner sides get half, so two adjacent
//            windows show the same total gap as a single window at the edge.
//   Full   - every side gets the full gap (adjacent windows show a doubled gap).
//   Screen - only sides touching a screen edge get a gap; inner sides get none.
enum class GapMode { Even, Full, Screen };
static GapMode g_gapMode = GapMode::Even;

// Target size for the Center action; 0 means keep the window's current size.
static int g_centerWidth = 0;
static int g_centerHeight = 0;

// Step (percentage of the monitor work area) by which Make smaller / larger
// grow or shrink the active window.
static int g_resizeStepPercent = 5;

// Per-action trigger key and required modifier mask; both indexes line up with
// g_actionByIndex below. A key of 0 means the action is unbound. The mask uses the
// MOD_F_* bits, which deliberately match RegisterHotKey's MOD_ALT/CONTROL/SHIFT/WIN
// values, so it can be passed straight to RegisterHotKey as the fsModifiers.
static UINT g_actionKeys[ACTION_COUNT] = {};
static UINT g_actionMods[ACTION_COUNT] = {};
static const Action g_actionByIndex[ACTION_COUNT] = {
    Action::LeftHalf,    Action::RightHalf,     Action::TopHalf,
    Action::BottomHalf,  Action::CenterHalf,    Action::TopLeft,
    Action::TopRight,    Action::BottomLeft,    Action::BottomRight,
    Action::Maximize,    Action::AlmostMaximize, Action::Center,
    Action::Minimize,    Action::NextDisplay,    Action::PreviousDisplay,
    Action::MakeSmaller, Action::MakeLarger,     Action::RestorePrevious,
};

// Setting names for each action's shortcut, lined up with g_actionByIndex. Used both
// to read the bindings and to name a binding in the log when it can't be registered.
static const PCWSTR g_keyNames[ACTION_COUNT] = {
    L"shortcuts.keyLeftHalf",       L"shortcuts.keyRightHalf",
    L"shortcuts.keyTopHalf",        L"shortcuts.keyBottomHalf",
    L"shortcuts.keyCenterHalf",     L"shortcuts.keyTopLeft",
    L"shortcuts.keyTopRight",       L"shortcuts.keyBottomLeft",
    L"shortcuts.keyBottomRight",    L"shortcuts.keyMaximize",
    L"shortcuts.keyAlmostMaximize", L"shortcuts.keyCenter",
    L"shortcuts.keyMinimize",       L"shortcuts.keyNextDisplay",
    L"shortcuts.keyPrevDisplay",    L"shortcuts.keySmaller",
    L"shortcuts.keyLarger",         L"shortcuts.keyRestore",
};

// Which hotkey ids are currently registered. Touched only on the hook thread.
static bool g_hotkeyRegistered[ACTION_COUNT] = {};

// Window placement captured before the first snap, so RestorePrevious can revert.
// Accessed only from the worker thread, so no synchronization is needed.
static std::unordered_map<HWND, WINDOWPLACEMENT> g_savedPlacements;

// Auto-snap rules: when a window of a matching executable is shown, the chosen
// action is applied to it automatically.
struct AppRule {
    std::vector<std::wstring> executables;
    Action action = Action::None;
    bool noResize = false;  // Move the window into the region but keep its size.
};
static std::vector<AppRule> g_appRules;
static std::mutex g_appRulesMutex;

// Windows already auto-snapped by a rule, so a single window isn't snapped
// repeatedly as it raises further show events.
static std::unordered_set<HWND> g_ruleHandledWindows;
static std::mutex g_ruleHandledMutex;

static HWINEVENTHOOK g_winEventHook = nullptr;
static HANDLE g_hookThread = nullptr;
static DWORD g_hookThreadId = 0;
static HANDLE g_hookReadyEvent = nullptr;

// Work queue drained by the worker thread; producers are the hotkey/window-event
// hooks, so it is guarded by a mutex. A "cleanup" item carries no action and just
// tells the worker (the sole owner of g_savedPlacements) to drop a destroyed window.
struct WorkItem {
    HWND hWnd;
    Action action;
    bool noResize;
    bool cleanup;
};
static std::deque<WorkItem> g_workQueue;
static std::mutex g_workQueueMutex;

static HANDLE g_workerThread = nullptr;
static HANDLE g_workEvent = nullptr;
static std::atomic<bool> g_quit{false};

static void EnqueueWork(HWND hWnd, Action action, bool noResize = false) {
    if (!hWnd || action == Action::None) {
        return;
    }
    {
        std::lock_guard<std::mutex> lock(g_workQueueMutex);
        g_workQueue.push_back({hWnd, action, noResize, false});
    }
    SetEvent(g_workEvent);
}

// Ask the worker thread to forget a window's saved placement once it's destroyed,
// so the map doesn't grow without bound and a recycled HWND can't inherit a stale
// placement.
static void EnqueueCleanup(HWND hWnd) {
    if (!hWnd) {
        return;
    }
    {
        std::lock_guard<std::mutex> lock(g_workQueueMutex);
        g_workQueue.push_back({hWnd, Action::None, false, true});
    }
    SetEvent(g_workEvent);
}

static std::wstring ToLower(std::wstring s) {
    for (auto& c : s) {
        c = (wchar_t)towlower(c);
    }
    return s;
}

static std::wstring Trim(const std::wstring& s) {
    size_t start = 0;
    while (start < s.size() && iswspace(s[start])) {
        start++;
    }
    size_t end = s.size();
    while (end > start && iswspace(s[end - 1])) {
        end--;
    }
    return s.substr(start, end - start);
}

static UINT GetVkFromKeyName(const std::wstring& name) {
    if (name == L"left") {
        return VK_LEFT;
    }
    if (name == L"right") {
        return VK_RIGHT;
    }
    if (name == L"up") {
        return VK_UP;
    }
    if (name == L"down") {
        return VK_DOWN;
    }
    // Punctuation keys, by symbol or by word. Note: the shortcut parser splits
    // on "+", so the plus key must be written as "plus" (or "=" / "equals").
    if (name == L"[" || name == L"lbracket" || name == L"leftbracket") {
        return VK_OEM_4;
    }
    if (name == L"]" || name == L"rbracket" || name == L"rightbracket") {
        return VK_OEM_6;
    }
    if (name == L"-" || name == L"minus") {
        return VK_OEM_MINUS;
    }
    if (name == L"+" || name == L"plus" || name == L"=" || name == L"equals") {
        return VK_OEM_PLUS;
    }
    if (name.size() == 1) {
        wchar_t c = name[0];
        if (c >= L'a' && c <= L'z') {
            return (UINT)(L'A' + (c - L'a'));
        }
        if (c >= L'0' && c <= L'9') {
            return (UINT)c;
        }
    }
    return 0;
}

static UINT ParseModifierMask(const std::wstring& value) {
    if (value == L"none") {
        return 0;
    }
    if (value == L"alt") {
        return MOD_F_ALT;
    }
    if (value == L"ctrl") {
        return MOD_F_CTRL;
    }
    if (value == L"shift") {
        return MOD_F_SHIFT;
    }
    if (value == L"win") {
        return MOD_F_WIN;
    }
    if (value == L"alt_ctrl") {
        return MOD_F_ALT | MOD_F_CTRL;
    }
    if (value == L"alt_shift") {
        return MOD_F_ALT | MOD_F_SHIFT;
    }
    if (value == L"ctrl_shift") {
        return MOD_F_CTRL | MOD_F_SHIFT;
    }
    if (value == L"win_alt") {
        return MOD_F_WIN | MOD_F_ALT;
    }
    if (value == L"win_ctrl") {
        return MOD_F_WIN | MOD_F_CTRL;
    }
    if (value == L"win_shift") {
        return MOD_F_WIN | MOD_F_SHIFT;
    }
    return MOD_F_ALT;
}

// Maps a single modifier word (alt/ctrl/shift/win and common aliases) to its bit.
// Returns true and sets *bit if the token is a recognised modifier.
static bool ParseModifierToken(const std::wstring& token, UINT* bit) {
    if (token == L"alt" || token == L"menu") {
        *bit = MOD_F_ALT;
        return true;
    }
    if (token == L"ctrl" || token == L"control") {
        *bit = MOD_F_CTRL;
        return true;
    }
    if (token == L"shift") {
        *bit = MOD_F_SHIFT;
        return true;
    }
    if (token == L"win" || token == L"super" || token == L"meta" ||
        token == L"cmd") {
        *bit = MOD_F_WIN;
        return true;
    }
    return false;
}

// Parses a "[mod+]...key" shortcut string into a key code and modifier mask.
// Returns true only if a key token was found; modifier-only strings are invalid.
static bool ParseShortcut(const std::wstring& text, UINT* outVk, UINT* outMods) {
    *outVk = 0;
    *outMods = 0;
    bool haveKey = false;

    size_t start = 0;
    while (start <= text.size()) {
        size_t pos = text.find(L'+', start);
        std::wstring token = ToLower(Trim(
            pos == std::wstring::npos ? text.substr(start)
                                      : text.substr(start, pos - start)));

        if (!token.empty()) {
            UINT bit = 0;
            if (ParseModifierToken(token, &bit)) {
                *outMods |= bit;
            } else {
                UINT vk = GetVkFromKeyName(token);
                if (vk != 0) {
                    *outVk = vk;
                    haveKey = true;
                }
            }
        }

        if (pos == std::wstring::npos) {
            break;
        }
        start = pos + 1;
    }

    return haveKey;
}

// File name portion of a path (handles both slash styles).
static std::wstring BaseName(const std::wstring& path) {
    size_t pos = path.find_last_of(L"\\/");
    return pos == std::wstring::npos ? path : path.substr(pos + 1);
}

// Reduces a user-entered executable reference to a bare lowercase file name,
// e.g. `"C:\Windows\notepad"` -> `notepad.exe`.
static std::wstring NormalizeExecutableName(std::wstring name) {
    name = Trim(name);
    if (name.size() >= 2 && name.front() == L'"' && name.back() == L'"') {
        name = name.substr(1, name.size() - 2);
    }
    if (name.empty()) {
        return name;
    }
    name = ToLower(Trim(BaseName(name)));
    if (!name.empty() && name.find(L'.') == std::wstring::npos) {
        name += L".exe";
    }
    return name;
}

// Splits a comma/semicolon/pipe-separated list of executable names, normalizing
// and de-duplicating each entry.
static std::vector<std::wstring> SplitExecutableList(const std::wstring& value) {
    std::vector<std::wstring> result;
    size_t start = 0;
    while (start <= value.size()) {
        size_t pos = value.find_first_of(L",;|", start);
        std::wstring part = pos == std::wstring::npos
                                ? value.substr(start)
                                : value.substr(start, pos - start);

        std::wstring normalized = NormalizeExecutableName(part);
        if (!normalized.empty() &&
            std::find(result.begin(), result.end(), normalized) ==
                result.end()) {
            result.push_back(normalized);
        }

        if (pos == std::wstring::npos) {
            break;
        }
        start = pos + 1;
    }
    return result;
}

static Action ParseActionName(const std::wstring& value) {
    std::wstring v = ToLower(Trim(value));
    if (v == L"left-half") return Action::LeftHalf;
    if (v == L"right-half") return Action::RightHalf;
    if (v == L"top-half") return Action::TopHalf;
    if (v == L"bottom-half") return Action::BottomHalf;
    if (v == L"center-half") return Action::CenterHalf;
    if (v == L"top-left") return Action::TopLeft;
    if (v == L"top-right") return Action::TopRight;
    if (v == L"bottom-left") return Action::BottomLeft;
    if (v == L"bottom-right") return Action::BottomRight;
    if (v == L"maximize") return Action::Maximize;
    if (v == L"almost-maximize") return Action::AlmostMaximize;
    if (v == L"center") return Action::Center;
    if (v == L"minimize") return Action::Minimize;
    return Action::None;
}

// Bare lowercase executable name of the process that owns the window.
static std::wstring GetWindowProcessExeName(HWND hWnd) {
    DWORD pid = 0;
    GetWindowThreadProcessId(hWnd, &pid);
    if (!pid) {
        return L"";
    }

    HANDLE process =
        OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!process) {
        return L"";
    }

    wchar_t path[MAX_PATH] = {};
    DWORD size = ARRAYSIZE(path);
    std::wstring result;
    if (QueryFullProcessImageNameW(process, 0, path, &size)) {
        result = ToLower(BaseName(std::wstring(path, size)));
    }
    CloseHandle(process);
    return result;
}

// Finds the action (and no-resize flag) for the first rule matching exeName.
// Returns Action::None if no rule matches.
static Action FindRuleAction(const std::wstring& exeName, bool* outNoResize) {
    *outNoResize = false;
    if (exeName.empty()) {
        return Action::None;
    }
    std::lock_guard<std::mutex> lock(g_appRulesMutex);
    for (const auto& rule : g_appRules) {
        for (const auto& exe : rule.executables) {
            if (exe == exeName) {
                *outNoResize = rule.noResize;
                return rule.action;
            }
        }
    }
    return Action::None;
}

static bool IsEligibleWindow(HWND hWnd) {
    if (!hWnd || !IsWindow(hWnd)) {
        return false;
    }

    if (GetAncestor(hWnd, GA_ROOT) != hWnd) {
        return false;
    }

    LONG_PTR exStyle = GetWindowLongPtrW(hWnd, GWL_EXSTYLE);
    if (exStyle & WS_EX_TOOLWINDOW) {
        return false;
    }

    LONG_PTR style = GetWindowLongPtrW(hWnd, GWL_STYLE);
    if (!(style & WS_CAPTION)) {
        return false;
    }

    return true;
}

// Position the window so its *visible* bounds match the given screen rectangle.
// GetWindowRect/SetWindowPos coordinates include the invisible drop-shadow border
// that DWM draws around a window, so measure it and expand the target accordingly.
static void ApplyVisibleRect(HWND hWnd, int x, int y, int w, int h) {
    RECT windowRect = {};
    RECT frameRect = {};
    int marginLeft = 0, marginTop = 0, marginRight = 0, marginBottom = 0;
    if (GetWindowRect(hWnd, &windowRect) &&
        SUCCEEDED(DwmGetWindowAttribute(hWnd, DWMWA_EXTENDED_FRAME_BOUNDS,
                                        &frameRect, sizeof(frameRect)))) {
        marginLeft = frameRect.left - windowRect.left;
        marginTop = frameRect.top - windowRect.top;
        marginRight = windowRect.right - frameRect.right;
        marginBottom = windowRect.bottom - frameRect.bottom;
    }

    SetWindowPos(hWnd, nullptr,
                 x - marginLeft,
                 y - marginTop,
                 w + marginLeft + marginRight,
                 h + marginTop + marginBottom,
                 SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE);
}

// Current visible bounds of the window (excludes the invisible drop-shadow border).
static void GetVisibleRect(HWND hWnd, RECT* rect) {
    if (FAILED(DwmGetWindowAttribute(hWnd, DWMWA_EXTENDED_FRAME_BOUNDS, rect,
                                     sizeof(*rect)))) {
        GetWindowRect(hWnd, rect);
    }
}

// Current visible size of the window (excludes the invisible drop-shadow border).
static void GetVisibleSize(HWND hWnd, int* width, int* height) {
    RECT rect = {};
    GetVisibleRect(hWnd, &rect);
    *width = rect.right - rect.left;
    *height = rect.bottom - rect.top;
}

static BOOL CALLBACK CollectMonitorsProc(HMONITOR hMonitor, HDC, LPRECT,
                                         LPARAM lParam) {
    auto* works = reinterpret_cast<std::vector<RECT>*>(lParam);
    MONITORINFO mi = {};
    mi.cbSize = sizeof(mi);
    if (GetMonitorInfoW(hMonitor, &mi)) {
        works->push_back(mi.rcWork);
    }
    return TRUE;
}

// Moves the window to the next/previous monitor (direction +1/-1, wrapping),
// keeping its size and position relative to the work area (scaled if the target
// monitor has a different work-area size). Preserves a maximized state.
static void MoveToAdjacentDisplay(HWND hWnd, int direction) {
    std::vector<RECT> works;
    EnumDisplayMonitors(nullptr, nullptr, CollectMonitorsProc,
                        reinterpret_cast<LPARAM>(&works));
    if (works.size() < 2) {
        return;  // Nothing to move to.
    }

    // Order monitors left-to-right, then top-to-bottom, for predictable cycling.
    std::sort(works.begin(), works.end(), [](const RECT& a, const RECT& b) {
        if (a.left != b.left) {
            return a.left < b.left;
        }
        return a.top < b.top;
    });

    MONITORINFO cur = {};
    cur.cbSize = sizeof(cur);
    if (!GetMonitorInfoW(MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST),
                         &cur)) {
        return;
    }

    int curIndex = -1;
    for (size_t i = 0; i < works.size(); i++) {
        if (EqualRect(&works[i], &cur.rcWork)) {
            curIndex = (int)i;
            break;
        }
    }
    if (curIndex < 0) {
        return;
    }

    const int n = (int)works.size();
    const RECT& src = works[curIndex];
    const RECT& dst = works[((curIndex + direction) % n + n) % n];

    const int srcW = src.right - src.left;
    const int srcH = src.bottom - src.top;
    const int dstW = dst.right - dst.left;
    const int dstH = dst.bottom - dst.top;
    if (srcW <= 0 || srcH <= 0 || dstW <= 0 || dstH <= 0) {
        return;
    }

    const bool wasZoomed = IsZoomed(hWnd) != FALSE;
    if (wasZoomed || IsIconic(hWnd)) {
        ShowWindow(hWnd, SW_RESTORE);
    }

    RECT vis = {};
    GetVisibleRect(hWnd, &vis);

    // Map the window rect from the source work area to the destination one.
    auto mapX = [&](LONG x) {
        return dst.left + (int)((double)(x - src.left) * dstW / srcW);
    };
    auto mapY = [&](LONG y) {
        return dst.top + (int)((double)(y - src.top) * dstH / srcH);
    };

    int x = mapX(vis.left);
    int y = mapY(vis.top);
    int w = (int)((double)(vis.right - vis.left) * dstW / srcW);
    int h = (int)((double)(vis.bottom - vis.top) * dstH / srcH);
    if (w < 1) w = 1;
    if (h < 1) h = 1;

    ApplyVisibleRect(hWnd, x, y, w, h);

    if (wasZoomed) {
        ShowWindow(hWnd, SW_MAXIMIZE);  // Re-maximize on the new monitor.
    }
}

// Grows or shrinks the window about its center by the configured step
// (direction +1 = larger, -1 = smaller), clamped to the monitor work area.
static void ResizeFromCenter(HWND hWnd, int direction) {
    if (IsIconic(hWnd) || IsZoomed(hWnd)) {
        ShowWindow(hWnd, SW_RESTORE);
    }

    RECT vis = {};
    GetVisibleRect(hWnd, &vis);
    const int w = vis.right - vis.left;
    const int h = vis.bottom - vis.top;
    const int cx = vis.left + w / 2;
    const int cy = vis.top + h / 2;

    MONITORINFO mi = {};
    mi.cbSize = sizeof(mi);
    if (!GetMonitorInfoW(MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST),
                         &mi)) {
        return;
    }
    const RECT& work = mi.rcWork;
    const int workW = work.right - work.left;
    const int workH = work.bottom - work.top;

    int stepX = workW * g_resizeStepPercent / 100;
    int stepY = workH * g_resizeStepPercent / 100;
    if (stepX < 1) stepX = 1;
    if (stepY < 1) stepY = 1;

    int newW = w + direction * stepX;
    int newH = h + direction * stepY;

    // Clamp between a small minimum and the full work area.
    const int minW = 200;
    const int minH = 150;
    newW = std::max(minW, std::min(newW, workW));
    newH = std::max(minH, std::min(newH, workH));

    int x = cx - newW / 2;
    int y = cy - newH / 2;
    if (x < work.left) x = work.left;
    if (y < work.top) y = work.top;
    if (x + newW > work.right) x = work.right - newW;
    if (y + newH > work.bottom) y = work.bottom - newH;

    ApplyVisibleRect(hWnd, x, y, newW, newH);
}

static void SavePlacementIfNew(HWND hWnd) {
    if (g_savedPlacements.find(hWnd) != g_savedPlacements.end()) {
        return;
    }
    WINDOWPLACEMENT wp = {};
    wp.length = sizeof(wp);
    if (GetWindowPlacement(hWnd, &wp)) {
        g_savedPlacements[hWnd] = wp;
    }
}

static void RestorePreviousPlacement(HWND hWnd) {
    auto it = g_savedPlacements.find(hWnd);
    if (it == g_savedPlacements.end()) {
        return;
    }
    SetWindowPlacement(hWnd, &it->second);
    g_savedPlacements.erase(it);
}

static void SnapWindow(HWND hWnd, Action action, bool noResize) {
    if (action == Action::None || !IsEligibleWindow(hWnd)) {
        return;
    }

    if (action == Action::RestorePrevious) {
        RestorePreviousPlacement(hWnd);
        return;
    }

    // Remember where the window was before its first snap so it can be restored.
    SavePlacementIfNew(hWnd);

    if (action == Action::Minimize) {
        ShowWindow(hWnd, SW_MINIMIZE);
        return;
    }

    // Maximize fills the work area; with "don't resize" it instead keeps its
    // size and is centered in the work area (handled by the cell logic below).
    if (action == Action::Maximize && !noResize) {
        ShowWindow(hWnd, SW_MAXIMIZE);
        return;
    }

    if (action == Action::NextDisplay || action == Action::PreviousDisplay) {
        MoveToAdjacentDisplay(hWnd, action == Action::NextDisplay ? 1 : -1);
        return;
    }

    if (action == Action::MakeSmaller || action == Action::MakeLarger) {
        ResizeFromCenter(hWnd, action == Action::MakeLarger ? 1 : -1);
        return;
    }

    if (IsIconic(hWnd) || IsZoomed(hWnd)) {
        ShowWindow(hWnd, SW_RESTORE);
    }

    HMONITOR monitor = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi = {};
    mi.cbSize = sizeof(mi);
    if (!GetMonitorInfoW(monitor, &mi)) {
        return;
    }

    const RECT& work = mi.rcWork;
    const int fullWidth = work.right - work.left;
    const int fullHeight = work.bottom - work.top;
    const int halfWidth = fullWidth / 2;
    const int midX = work.left + halfWidth;
    const int midY = work.top + fullHeight / 2;

    if (action == Action::Center) {
        int curW = 0, curH = 0;
        GetVisibleSize(hWnd, &curW, &curH);
        int w = (!noResize && g_centerWidth > 0) ? g_centerWidth : curW;
        int h = (!noResize && g_centerHeight > 0) ? g_centerHeight : curH;
        int x = work.left + (fullWidth - w) / 2;
        int y = work.top + (fullHeight - h) / 2;
        ApplyVisibleRect(hWnd, x, y, w, h);
        return;
    }

    // The region of the work area this action targets, before gaps.
    RECT cell = work;
    switch (action) {
        case Action::LeftHalf:
            cell = {work.left, work.top, midX, work.bottom};
            break;
        case Action::RightHalf:
            cell = {midX, work.top, work.right, work.bottom};
            break;
        case Action::TopHalf:
            cell = {work.left, work.top, work.right, midY};
            break;
        case Action::BottomHalf:
            cell = {work.left, midY, work.right, work.bottom};
            break;
        case Action::CenterHalf: {
            int left = work.left + (fullWidth - halfWidth) / 2;
            cell = {left, work.top, left + halfWidth, work.bottom};
            break;
        }
        case Action::TopLeft:
            cell = {work.left, work.top, midX, midY};
            break;
        case Action::TopRight:
            cell = {midX, work.top, work.right, midY};
            break;
        case Action::BottomLeft:
            cell = {work.left, midY, midX, work.bottom};
            break;
        case Action::BottomRight:
            cell = {midX, midY, work.right, work.bottom};
            break;
        case Action::AlmostMaximize:
        case Action::Maximize:  // Only reached with noResize; ignores gaps.
            cell = work;
            break;
        default:
            return;
    }

    // Inset each side by its gap. Maximize ignores gaps entirely. Inner sides
    // (those not coinciding with the work-area edge) are sized per the gap mode:
    // full gap (doubles between windows), half gap (even spacing), or none.
    if (action != Action::Maximize) {
        auto sideGap = [](bool atScreenEdge, int gap) -> int {
            if (atScreenEdge) {
                return gap;
            }
            switch (g_gapMode) {
                case GapMode::Even:
                    return gap / 2;
                case GapMode::Screen:
                    return 0;
                case GapMode::Full:
                default:
                    return gap;
            }
        };
        cell.left += sideGap(cell.left == work.left, g_gapLeft);
        cell.top += sideGap(cell.top == work.top, g_gapTop);
        cell.right -= sideGap(cell.right == work.right, g_gapRight);
        cell.bottom -= sideGap(cell.bottom == work.bottom, g_gapBottom);
    }
    if (cell.right <= cell.left || cell.bottom <= cell.top) {
        return;
    }

    if (!noResize) {
        ApplyVisibleRect(hWnd, cell.left, cell.top, cell.right - cell.left,
                         cell.bottom - cell.top);
        return;
    }

    // Keep the window's size and anchor it within the target cell. The anchor
    // edge follows the action (e.g. top-right -> top and right edges of the
    // cell), so the unresized window sits in the matching corner/side.
    int w = 0, h = 0;
    GetVisibleSize(hWnd, &w, &h);
    const int cellW = cell.right - cell.left;
    const int cellH = cell.bottom - cell.top;

    int ax = 0, ay = 0;  // -1 = left/top, 0 = center, 1 = right/bottom.
    switch (action) {
        case Action::LeftHalf:   ax = -1; ay = 0;  break;
        case Action::RightHalf:  ax = 1;  ay = 0;  break;
        case Action::TopHalf:    ax = 0;  ay = -1; break;
        case Action::BottomHalf: ax = 0;  ay = 1;  break;
        case Action::TopLeft:    ax = -1; ay = -1; break;
        case Action::TopRight:   ax = 1;  ay = -1; break;
        case Action::BottomLeft: ax = -1; ay = 1;  break;
        case Action::BottomRight:ax = 1;  ay = 1;  break;
        default:                 ax = 0;  ay = 0;  break;  // CenterHalf, (Almost)Maximize.
    }

    int x = ax < 0 ? cell.left
            : ax > 0 ? cell.right - w
                     : cell.left + (cellW - w) / 2;
    int y = ay < 0 ? cell.top
            : ay > 0 ? cell.bottom - h
                     : cell.top + (cellH - h) / 2;

    if (x < work.left) {
        x = work.left;
    }
    if (y < work.top) {
        y = work.top;
    }

    ApplyVisibleRect(hWnd, x, y, w, h);
}

// Drops every currently-registered hotkey. Must run on the hook thread (the thread
// that called RegisterHotKey owns the hotkeys and receives their WM_HOTKEY messages).
static void UnregisterHotkeys() {
    for (int i = 0; i < ACTION_COUNT; i++) {
        if (g_hotkeyRegistered[i]) {
            UnregisterHotKey(nullptr, i);
            g_hotkeyRegistered[i] = false;
        }
    }
}

// (Re)registers a thread-global hotkey per bound action, using the action index as the
// hotkey id. Consuming the whole combo avoids the stray-modifier side effects of a
// low-level keyboard hook. Must run on the hook thread.
static void RegisterHotkeys() {
    UnregisterHotkeys();
    for (int i = 0; i < ACTION_COUNT; i++) {
        if (g_actionKeys[i] == 0) {
            continue;
        }
        // MOD_NOREPEAT: one action per physical press, not once per auto-repeat.
        if (RegisterHotKey(nullptr, i, g_actionMods[i] | MOD_NOREPEAT,
                           g_actionKeys[i])) {
            g_hotkeyRegistered[i] = true;
        } else {
            Wh_Log(L"'%s' could not be registered (error %lu); the combination may "
                   L"be reserved or already claimed by another app",
                   g_keyNames[i], GetLastError());
        }
    }
}

// Applies the matching rule's action to a window the first time we see it shown.
static void TryApplyRuleToWindow(HWND hWnd) {
    if (!hWnd || !IsEligibleWindow(hWnd)) {
        return;
    }

    {
        std::lock_guard<std::mutex> lock(g_appRulesMutex);
        if (g_appRules.empty()) {
            return;
        }
    }

    {
        std::lock_guard<std::mutex> lock(g_ruleHandledMutex);
        if (g_ruleHandledWindows.count(hWnd)) {
            return;
        }
    }

    bool noResize = false;
    Action action = FindRuleAction(GetWindowProcessExeName(hWnd), &noResize);
    if (action == Action::None) {
        return;
    }

    {
        std::lock_guard<std::mutex> lock(g_ruleHandledMutex);
        if (!g_ruleHandledWindows.insert(hWnd).second) {
            return;  // Another event handled it first.
        }
    }

    EnqueueWork(hWnd, action, noResize);
}

void CALLBACK WinEventProc(HWINEVENTHOOK, DWORD event, HWND hWnd, LONG idObject,
                           LONG idChild, DWORD, DWORD) {
    if (idObject != OBJID_WINDOW || idChild != CHILDID_SELF || !hWnd) {
        return;
    }

    if (event == EVENT_OBJECT_DESTROY) {
        {
            std::lock_guard<std::mutex> lock(g_ruleHandledMutex);
            g_ruleHandledWindows.erase(hWnd);
        }
        EnqueueCleanup(hWnd);  // Drop any saved placement for this window.
        return;
    }

    // EVENT_OBJECT_SHOW.
    TryApplyRuleToWindow(hWnd);
}

// Installs the global show/destroy hook only while at least one auto-snap rule
// exists, and removes it otherwise. EVENT_OBJECT_SHOW fires for every window shown
// in every process, so the default (no-rules) configuration shouldn't pay for it.
// Must run on the hook thread (the thread that owns the WinEvent hook). The
// WINEVENT_SKIPOWNPROCESS flag keeps this tool process's own windows untouched.
static void UpdateWinEventHook() {
    bool wantHook;
    {
        std::lock_guard<std::mutex> lock(g_appRulesMutex);
        wantHook = !g_appRules.empty();
    }

    if (wantHook && !g_winEventHook) {
        g_winEventHook = SetWinEventHook(
            EVENT_OBJECT_DESTROY, EVENT_OBJECT_SHOW, nullptr, WinEventProc, 0, 0,
            WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS);
        if (!g_winEventHook) {
            Wh_Log(L"SetWinEventHook failed, error=%lu", GetLastError());
        }
    } else if (!wantHook && g_winEventHook) {
        UnhookWinEvent(g_winEventHook);
        g_winEventHook = nullptr;
    }
}

static BOOL CALLBACK EnumExistingWindowsProc(HWND hWnd, LPARAM) {
    if (IsWindowVisible(hWnd)) {
        TryApplyRuleToWindow(hWnd);
    }
    return TRUE;
}

// Snaps already-open windows that match a rule (on init and after a settings
// change), so adding a rule affects windows that are already on screen.
static void ApplyRulesToExistingWindows() {
    {
        std::lock_guard<std::mutex> lock(g_appRulesMutex);
        if (g_appRules.empty()) {
            return;
        }
    }
    EnumWindows(EnumExistingWindowsProc, 0);
}

static void LoadSettings() {
    // Wh_GetStringSetting never returns NULL (an unset value comes back as L""),
    // and WindhawkUtils::StringSetting frees the string for us on scope exit.
    UINT modifierMask =
        ParseModifierMask(WindhawkUtils::StringSetting::make(L"modifier").get());

    g_gapTop = Wh_GetIntSetting(L"dimensions.gapTop");
    g_gapBottom = Wh_GetIntSetting(L"dimensions.gapBottom");
    g_gapLeft = Wh_GetIntSetting(L"dimensions.gapLeft");
    g_gapRight = Wh_GetIntSetting(L"dimensions.gapRight");

    std::wstring gapModeValue =
        WindhawkUtils::StringSetting::make(L"dimensions.gapMode").get();
    if (gapModeValue == L"full") {
        g_gapMode = GapMode::Full;
    } else if (gapModeValue == L"screen") {
        g_gapMode = GapMode::Screen;
    } else {
        g_gapMode = GapMode::Even;
    }

    g_centerWidth =
        _wtoi(WindhawkUtils::StringSetting::make(L"dimensions.centerWidth").get());
    g_centerHeight =
        _wtoi(WindhawkUtils::StringSetting::make(L"dimensions.centerHeight").get());

    g_resizeStepPercent = Wh_GetIntSetting(L"dimensions.resizeStep");
    if (g_resizeStepPercent < 1) {
        g_resizeStepPercent = 1;
    }
    if (g_resizeStepPercent > 50) {
        g_resizeStepPercent = 50;
    }

    for (int i = 0; i < ACTION_COUNT; i++) {
        WindhawkUtils::StringSetting value =
            WindhawkUtils::StringSetting::make(g_keyNames[i]);

        UINT vk = 0, textMods = 0;
        if (ParseShortcut(value.get(), &vk, &textMods)) {
            g_actionKeys[i] = vk;
            // The global modifier is required on top of whatever the shortcut typed.
            g_actionMods[i] = modifierMask | textMods;
        } else {
            g_actionKeys[i] = 0;  // Unbound or invalid.
            g_actionMods[i] = 0;
        }
    }

    // De-duplicate bindings: if two actions resolve to the same key+modifier
    // combination, only the first one (in g_keyNames order) stays bound. Later
    // duplicates are disabled so a single keypress doesn't fire two actions.
    for (int i = 0; i < ACTION_COUNT; i++) {
        if (g_actionKeys[i] == 0) {
            continue;
        }
        for (int j = 0; j < i; j++) {
            if (g_actionKeys[j] == g_actionKeys[i] &&
                g_actionMods[j] == g_actionMods[i]) {
                Wh_Log(L"'%s' duplicates '%s' (same key+modifier); disabling '%s'",
                       g_keyNames[i], g_keyNames[j], g_keyNames[i]);
                g_actionKeys[i] = 0;
                g_actionMods[i] = 0;
                break;
            }
        }
    }

    // Auto-snap rules.
    std::vector<AppRule> newRules;
    for (int i = 0; i <= 255; i++) {
        wchar_t key[256] = {};

        swprintf_s(key, L"rules[%d].executable_names", i);
        std::wstring execNames = WindhawkUtils::StringSetting::make(key).get();

        swprintf_s(key, L"rules[%d].action", i);
        std::wstring actionValue = WindhawkUtils::StringSetting::make(key).get();

        swprintf_s(key, L"rules[%d].no_resize", i);
        bool noResize = Wh_GetIntSetting(key) != 0;

        if (execNames.empty() && actionValue.empty()) {
            // Tolerate a small gap of blank entries before giving up.
            if (i >= (int)newRules.size() + 2) {
                break;
            }
            continue;
        }

        AppRule rule;
        rule.executables = SplitExecutableList(execNames);
        rule.action = ParseActionName(actionValue);
        rule.noResize = noResize;
        if (!rule.executables.empty() && rule.action != Action::None) {
            newRules.push_back(std::move(rule));
        }
    }

    {
        std::lock_guard<std::mutex> lock(g_appRulesMutex);
        g_appRules = std::move(newRules);
    }
    {
        std::lock_guard<std::mutex> lock(g_ruleHandledMutex);
        g_ruleHandledWindows.clear();
    }
}

static DWORD WINAPI WorkerThreadProc(LPVOID) {
    for (;;) {
        WaitForSingleObject(g_workEvent, INFINITE);

        for (;;) {
            WorkItem item;
            {
                std::lock_guard<std::mutex> lock(g_workQueueMutex);
                if (g_workQueue.empty()) {
                    break;
                }
                item = g_workQueue.front();
                g_workQueue.pop_front();
            }
            if (item.cleanup) {
                g_savedPlacements.erase(item.hWnd);
            } else {
                SnapWindow(item.hWnd, item.action, item.noResize);
            }
        }

        if (g_quit.load()) {
            break;
        }
    }
    return 0;
}

static DWORD WINAPI HookThreadProc(LPVOID) {
    // Register the shortcut hotkeys on this thread; WM_HOTKEY (for a NULL window)
    // is posted to the queue of the thread that called RegisterHotKey. The auto-snap
    // WinEvent hook is installed here too, but only if any rules are configured.
    RegisterHotkeys();
    UpdateWinEventHook();

    SetEvent(g_hookReadyEvent);

    MSG msg;
    while (!g_quit.load()) {
        BOOL gm = GetMessageW(&msg, nullptr, 0, 0);
        if (gm == 0 || gm == -1) {
            break;
        }
        // WM_HOTKEY and our settings-changed signal are thread messages (no window),
        // so handle them here rather than via DispatchMessage.
        if (msg.hwnd == nullptr) {
            if (msg.message == WM_HOTKEY) {
                int idx = (int)msg.wParam;
                if (idx >= 0 && idx < ACTION_COUNT) {
                    EnqueueWork(GetForegroundWindow(), g_actionByIndex[idx]);
                }
                continue;
            }
            if (msg.message == WM_APP_SETTINGS_CHANGED) {
                RegisterHotkeys();
                UpdateWinEventHook();
                continue;
            }
        }
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    UnregisterHotkeys();
    if (g_winEventHook) {
        UnhookWinEvent(g_winEventHook);
        g_winEventHook = nullptr;
    }
    return 0;
}

BOOL WhTool_ModInit() {
    LoadSettings();

    g_quit = false;

    g_workEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    if (!g_workEvent) {
        Wh_Log(L"CreateEvent (work) failed");
        return FALSE;
    }

    g_workerThread = CreateThread(nullptr, 0, WorkerThreadProc, nullptr, 0, nullptr);
    if (!g_workerThread) {
        Wh_Log(L"CreateThread (worker) failed");
        CloseHandle(g_workEvent);
        g_workEvent = nullptr;
        return FALSE;
    }

    g_hookReadyEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_hookReadyEvent) {
        Wh_Log(L"CreateEvent (hook ready) failed");
        g_quit = true;
        SetEvent(g_workEvent);
        WaitForSingleObject(g_workerThread, 2000);
        CloseHandle(g_workerThread);
        g_workerThread = nullptr;
        CloseHandle(g_workEvent);
        g_workEvent = nullptr;
        return FALSE;
    }

    g_hookThread = CreateThread(nullptr, 0, HookThreadProc, nullptr, 0,
                               &g_hookThreadId);
    if (!g_hookThread) {
        Wh_Log(L"CreateThread (hook) failed");
        CloseHandle(g_hookReadyEvent);
        g_hookReadyEvent = nullptr;
        g_quit = true;
        SetEvent(g_workEvent);
        WaitForSingleObject(g_workerThread, 2000);
        CloseHandle(g_workerThread);
        g_workerThread = nullptr;
        CloseHandle(g_workEvent);
        g_workEvent = nullptr;
        return FALSE;
    }

    WaitForSingleObject(g_hookReadyEvent, 3000);
    CloseHandle(g_hookReadyEvent);
    g_hookReadyEvent = nullptr;

    ApplyRulesToExistingWindows();

    return TRUE;
}

void WhTool_ModSettingsChanged() {
    LoadSettings();
    // Reflect the new bindings/rules on the hook thread (it owns the hotkeys and the
    // WinEvent hook).
    if (g_hookThreadId) {
        PostThreadMessageW(g_hookThreadId, WM_APP_SETTINGS_CHANGED, 0, 0);
    }
    ApplyRulesToExistingWindows();
}

void WhTool_ModUninit() {
    g_quit = true;

    if (g_hookThreadId) {
        PostThreadMessageW(g_hookThreadId, WM_QUIT, 0, 0);
    }
    if (g_hookThread) {
        WaitForSingleObject(g_hookThread, 2000);
        CloseHandle(g_hookThread);
        g_hookThread = nullptr;
        g_hookThreadId = 0;
    }

    if (g_workEvent) {
        SetEvent(g_workEvent);
    }
    if (g_workerThread) {
        WaitForSingleObject(g_workerThread, 2000);
        CloseHandle(g_workerThread);
        g_workerThread = nullptr;
    }
    if (g_workEvent) {
        CloseHandle(g_workEvent);
        g_workEvent = nullptr;
    }

    if (g_winEventHook) {
        UnhookWinEvent(g_winEventHook);
        g_winEventHook = nullptr;
    }

    g_savedPlacements.clear();
    {
        std::lock_guard<std::mutex> lock(g_workQueueMutex);
        g_workQueue.clear();
    }
    {
        std::lock_guard<std::mutex> lock(g_ruleHandledMutex);
        g_ruleHandledWindows.clear();
    }
}

////////////////////////////////////////////////////////////////////////////////
// Windhawk tool mod implementation for mods which don't need to inject to other
// processes or hook other functions. Context:
// https://github.com/ramensoftware/windhawk/wiki/Mods-as-tools:-Running-mods-in-a-dedicated-process
//
// The mod will load and run in a dedicated windhawk.exe process.

bool g_isToolModProcessLauncher;
HANDLE g_toolModProcessMutex;

void WINAPI EntryPoint_Hook() {
    Wh_Log(L">");
    ExitThread(0);
}

BOOL Wh_ModInit() {
    DWORD sessionId;
    if (ProcessIdToSessionId(GetCurrentProcessId(), &sessionId) &&
        sessionId == 0) {
        return FALSE;
    }

    bool isExcluded = false;
    bool isToolModProcess = false;
    bool isCurrentToolModProcess = false;
    int argc;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLine(), &argc);
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

        IMAGE_DOS_HEADER* dosHeader =
            (IMAGE_DOS_HEADER*)GetModuleHandle(nullptr);
        IMAGE_NT_HEADERS* ntHeaders =
            (IMAGE_NT_HEADERS*)((BYTE*)dosHeader + dosHeader->e_lfanew);

        DWORD entryPointRVA = ntHeaders->OptionalHeader.AddressOfEntryPoint;
        void* entryPoint = (BYTE*)dosHeader + entryPointRVA;

        Wh_SetFunctionHook(entryPoint, (void*)EntryPoint_Hook, nullptr);
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

    using CreateProcessInternalW_t = BOOL(WINAPI*)(
        HANDLE hUserToken, LPCWSTR lpApplicationName, LPWSTR lpCommandLine,
        LPSECURITY_ATTRIBUTES lpProcessAttributes,
        LPSECURITY_ATTRIBUTES lpThreadAttributes, WINBOOL bInheritHandles,
        DWORD dwCreationFlags, LPVOID lpEnvironment, LPCWSTR lpCurrentDirectory,
        LPSTARTUPINFOW lpStartupInfo,
        LPPROCESS_INFORMATION lpProcessInformation,
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
