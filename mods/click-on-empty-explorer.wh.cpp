// ==WindhawkMod==
// @id              click-on-empty-explorer
// @name            Click on Empty Explorer
// @description     Configure double/triple click, middle click, double middle click, and modifier+click actions on empty space in File Explorer. Supports 17 actions including navigation, tabs, custom hotkeys, and invoking any right-click context menu entry.
// @version         2.7.0
// @author          LiHua81
// @github          https://github.com/LiHua81
// @include         explorer.exe
// @compilerOptions -lcomctl32 -loleaut32 -lole32 -lshlwapi
// @architecture    x86-64
// ==/WindhawkMod==

// Source code is published under The GNU General Public License v3.0.
//
// Based on "Explorer Double Click Up" by wrldspawn (GPL-3.0) and
// "Explorer Middle Click Duplicate Tab" by LiHua81 (GPL-3.0).
// UIAutomation usage from "Click on empty taskbar space" (GPL-3.0).
// FileCabinet_CreateViewWindow2 hook from "Classic Explorer Treeview".
// Going up a directory referenced from Open-Shell.

// ==WindhawkModReadme==
/*
# Click on Empty Explorer

Configure what happens when you double click, triple click, middle click, double middle click,
or modifier+click (Ctrl/Alt/Shift+Click) on empty space in File Explorer. Supports 17 different
actions.

## How it works

This mod intercepts mouse clicks on the blank area of File Explorer's file list (where
no file or folder is located) and performs the action you've configured.

- **Double left click** — Windows natively detects double clicks. No delay at all.
- **Triple left click** — When triple-click is configured, double-click actions are
  delayed by ~500ms (the system double-click time). If a third click arrives in that
  window, the double-click is cancelled and only the triple-click action fires. If no
  third click arrives, the double-click action fires when the timer expires.
  When triple-click is not configured, double-click fires instantly as before.
- **Modifier + Click** — Hold Ctrl, Alt, or Shift while left-clicking empty space to
  trigger a separate action (e.g. Ctrl+Click = Go Up, Alt+Click = Refresh). Each
  modifier has its own independent action setting.
- **Middle click** — If only a single-click action is set (double middle click is
  disabled), the action fires instantly with no delay, just like left click.
- **Double middle click** — Windows does not natively support double middle click, so
  the mod uses a timer-based detection. When you middle click, the mod waits for your
  system's double-click time (typically 500ms). If you middle click again within that
  window, it counts as a double click and fires the double-click action instead.
  - If both single and double middle click are set: single click is delayed by the
    double-click detection window.
  - If only double middle click is set: only double clicks trigger the action.

## Actions

- **Go Up** — Navigate to the parent folder
- **Go Back** — Navigate back in history
- **Go Forward** — Navigate forward in history
- **Refresh** — Refresh the current view
- **New Tab** — Open a new blank tab (Win11)
- **Duplicate Tab** — Open the current folder in a new tab (Win11)
- **Close Tab** — Close the current tab (Win11)
- **New Folder** — Create a new folder in the current view
- **Copy Path** — Copy the current folder path to clipboard
- **Paste** — Paste (Ctrl+V) into the current view
- **Custom Hotkey** — Send a custom key combination, configured per trigger (see below)
- **Go to Desktop** — Navigate to the Desktop
- **Go to Home** — Navigate to Quick Access / Home
- **Open in VS Code** — Invoke "Open with Code" from the context menu. Matches English "Code" as a loose substring, so entries like "Encode with HandBrake" may match first; if the wrong entry fires (or on non-English Windows where "Code" won't match), set that trigger's **Context Menu Match** override to a more specific text/verb, or use **Open Context Menu Item**
- **Open in Terminal** — Launch Windows Terminal (`wt.exe -d <folder>`) directly in the current folder. Works regardless of Windows language; requires Windows Terminal to be installed. If Windows Terminal is not installed, or the current location is a virtual folder (This PC, Recycle Bin), nothing happens and the reason is written to the Windhawk debug log.
- **Open Context Menu Item** — Invoke any right-click background context menu entry by matching its text/verb (configured via "Context Menu Match" setting). Use this for Cursor, Git Bash, PowerShell, or any program that registered an entry.
- **None** — Do nothing

## Custom Hotkey

When you choose "Custom Hotkey" for a trigger, a text field appears where you define the shortcut.

**Format:** `Modifier+Modifier+...+Key` — spaces around `+` are optional and auto-trimmed

- **Modifiers:** `Ctrl`, `Shift`, `Alt`, `Win` — can combine multiple, e.g. `Ctrl+Shift+N`, `Win+Shift+S`
- **Keys:** letters (A-Z), digits (0-9), function keys (F1-F24), named keys (Tab, Enter, Space, Backspace, Delete, Escape, Left, Right, Up, Down, Home, End, PageUp, PageDown, Insert)

**Examples:** `Ctrl+V`, `Ctrl+Shift+N`, `Win+E`, `Alt+F4`, `Ctrl+Shift+Esc`, `F5`, `Win+Shift+S`

Each trigger has its own independent custom hotkey field.

## Context Menu Match

When you choose **Open Context Menu Item** for a trigger, this text field tells the mod which
right-click background menu entry to invoke. The mod opens the folder's actual right-click
context menu programmatically and invokes the entry that matches your input.

### Matching Rules

Matching is **case-insensitive** and ignores **spaces** and `&` accelerator markers. An
**exact** match on the normalized text or verb takes priority over a substring match, and
top-level entries are checked before submenu entries, in menu order:

| Menu item text              | Verb              | You can type any of                      |
|-----------------------------|-------------------|------------------------------------------|
| `Open Git Ba&sh here`       | `git_shell`       | `gitbash`, `git`, `bash`, `shell`        |
| `Open with Code`            | `{1C6DF0C0...}`   | `code`, `openwithcode`                   |
| `Open in Terminal`          | `{9F156763...}`   | `terminal`, `openinterminal`             |
| `&ExtractAllFiles`          | `ExtractAllFiles` | `extractall`, `extract`                  |
| `Properties`                | `properties`      | `prop`, `properties`                     |
| `New` → `Folder`            | `NewFolder`       | `newfolder`, `folder`                    |

### How to Find the Right Text

If your match text doesn't work, check the debug log (enable in Windhawk editor) —
the mod will dump ALL available menu items with their normalized text:

```
No match for 'Git Bash' — dumping all context menu items:
CMENU[Open Git Ba&sh here] wID=92 verb=[git_shell]  → match: "opengitbashhere" or "git_shell"
```

The `→ match:` part shows the normalized form of each entry's text and verb.
**Type any part of either normalized string** to match that entry. For example,
`"gitbash"`, `"git"`, `"bash"`, `"shell"`, or `"git_shell"` would all match this entry.

### Per-trigger overrides

Each trigger also has its own **Context Menu Match** override next to its action
setting (e.g. *Double-click Context Menu Match*, *Middle-click Context Menu
Match*). Leave one empty to fall back to the global **Context Menu Match** above
— this is the recommended way to use several context-menu triggers with
different entries at once.

Matching is a loose substring test, so keep match text specific: a short string
like `code` also matches `Encode`, `Decode`, or `Open with Code Insiders`. If
the wrong entry fires, use a longer or more specific match, or the entry's verb.

### Tips

- **Any program** — any program that registered a right-click entry on the folder background works, regardless of install path
- **Multiple matches** — an exact match on the normalized text or verb wins over a substring match; within each kind, the first one in menu order is used
- **Non-English menus** — type any substring from the display text in your system language (e.g., Japanese, Chinese, Korean all work)

## Windows version support

Requires Windows 10 or later. Tab-related actions (New Tab, Duplicate Tab, Close Tab)
require Windows 11 for tabbed Explorer support.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- doubleClickAction: goUp
  $name: Double Click Action
  $description: What to do when double left clicking empty space. Instant when triple-click is disabled. When triple-click is enabled, delayed ~500ms and overridden if a third click arrives.
  $options:
    - goUp: Go Up
    - goBack: Go Back
    - goForward: Go Forward
    - refresh: Refresh
    - newTab: New Tab (Win11)
    - duplicateTab: Duplicate Tab (Win11)
    - closeTab: Close Tab (Win11)
    - newFolder: New Folder
    - copyPath: Copy Path
    - paste: Paste
    - customHotkey: Custom Hotkey
    - goToDesktop: Go to Desktop
    - goToHome: Go to Home
    - openInVSCode: Open in VS Code
    - openInTerminal: Open in Terminal
    - openWithContextMenu: Open Context Menu Item
    - none: None
- doubleClickCustomHotkey: ""
  $name: Double Click Custom Hotkey
  $description: "Format: modifier keys + main key. Modifiers: Ctrl, Shift, Alt, Win (can combine multiple, e.g. Ctrl+Shift+N, Win+Shift+S). Main key: letter, F1-F24, Tab, Enter, Escape, arrows, Backspace, Delete, Home, End, PageUp, PageDown, Insert"
- doubleClickContextMenuMatch: ""
  $name: Double Click Context Menu Match
  $description: "When Double Click Action is 'Open Context Menu Item', use this match text instead of the global 'Context Menu Match'. Leave empty to use the global setting."
- tripleClickAction: none
  $name: Triple Click Action
  $description: What to do when triple left clicking empty space. When enabled, double-click is delayed ~500ms; if a third click arrives, only the triple-click action fires (double-click is cancelled).
  $options:
    - goUp: Go Up
    - goBack: Go Back
    - goForward: Go Forward
    - refresh: Refresh
    - newTab: New Tab (Win11)
    - duplicateTab: Duplicate Tab (Win11)
    - closeTab: Close Tab (Win11)
    - newFolder: New Folder
    - copyPath: Copy Path
    - paste: Paste
    - customHotkey: Custom Hotkey
    - goToDesktop: Go to Desktop
    - goToHome: Go to Home
    - openInVSCode: Open in VS Code
    - openInTerminal: Open in Terminal
    - openWithContextMenu: Open Context Menu Item
    - none: None
- tripleClickCustomHotkey: ""
  $name: Triple Click Custom Hotkey
  $description: "Same format as Double Click Custom Hotkey. Supports multiple modifiers. Ex: Ctrl+W, Win+D, Ctrl+Shift+Esc"
- tripleClickContextMenuMatch: ""
  $name: Triple Click Context Menu Match
  $description: "Match text override for 'Open Context Menu Item' on this trigger. Leave empty to use global setting."
- middleClickAction: none
  $name: Middle Click Action
  $description: What to do when single middle clicking empty space. If only single click is set, fires instantly. If both single and double are set, single is delayed ~500ms to detect double clicks.
  $options:
    - goUp: Go Up
    - goBack: Go Back
    - goForward: Go Forward
    - refresh: Refresh
    - newTab: New Tab (Win11)
    - duplicateTab: Duplicate Tab (Win11)
    - closeTab: Close Tab (Win11)
    - newFolder: New Folder
    - copyPath: Copy Path
    - paste: Paste
    - customHotkey: Custom Hotkey
    - goToDesktop: Go to Desktop
    - goToHome: Go to Home
    - openInVSCode: Open in VS Code
    - openInTerminal: Open in Terminal
    - openWithContextMenu: Open Context Menu Item
    - none: None
- middleClickCustomHotkey: ""
  $name: Middle Click Custom Hotkey
  $description: "Same format as Double Click Custom Hotkey. Supports multiple modifiers. Ex: Ctrl+V, Ctrl+T, Win+D, Ctrl+Shift+Esc"
- middleClickContextMenuMatch: ""
  $name: Middle Click Context Menu Match
  $description: "Match text override for 'Open Context Menu Item' on this trigger. Leave empty to use global setting."
- doubleMiddleClickAction: none
  $name: Double Middle Click Action
  $description: What to do when double middle clicking empty space. Two middle clicks within ~500ms count as a double click. If only double is set, single middle clicks are ignored.
  $options:
    - goUp: Go Up
    - goBack: Go Back
    - goForward: Go Forward
    - refresh: Refresh
    - newTab: New Tab (Win11)
    - duplicateTab: Duplicate Tab (Win11)
    - closeTab: Close Tab (Win11)
    - newFolder: New Folder
    - copyPath: Copy Path
    - paste: Paste
    - customHotkey: Custom Hotkey
    - goToDesktop: Go to Desktop
    - goToHome: Go to Home
    - openInVSCode: Open in VS Code
    - openInTerminal: Open in Terminal
    - openWithContextMenu: Open Context Menu Item
    - none: None
- doubleMiddleClickCustomHotkey: ""
  $name: Double Middle Click Custom Hotkey
  $description: "Same format as Double Click Custom Hotkey. Supports multiple modifiers. Ex: Ctrl+W, Alt+Tab, Win+E, Alt+Shift+F4"
- doubleMiddleClickContextMenuMatch: ""
  $name: Double Middle Click Context Menu Match
  $description: "Match text override for 'Open Context Menu Item' on this trigger. Leave empty to use global setting."
- ctrlClickAction: none
  $name: Ctrl+Click Action
  $description: What to do when Ctrl+left clicking empty space. Hold Ctrl and single-click on empty area.
  $options:
    - goUp: Go Up
    - goBack: Go Back
    - goForward: Go Forward
    - refresh: Refresh
    - newTab: New Tab (Win11)
    - duplicateTab: Duplicate Tab (Win11)
    - closeTab: Close Tab (Win11)
    - newFolder: New Folder
    - copyPath: Copy Path
    - paste: Paste
    - customHotkey: Custom Hotkey
    - goToDesktop: Go to Desktop
    - goToHome: Go to Home
    - openInVSCode: Open in VS Code
    - openInTerminal: Open in Terminal
    - openWithContextMenu: Open Context Menu Item
    - none: None
- ctrlClickCustomHotkey: ""
  $name: Ctrl+Click Custom Hotkey
  $description: "Same format as Double Click Custom Hotkey. Ex: Ctrl+N, Ctrl+Shift+E"
- ctrlClickContextMenuMatch: ""
  $name: Ctrl+Click Context Menu Match
  $description: "Match text override for 'Open Context Menu Item' on this trigger. Leave empty to use global setting."
- altClickAction: none
  $name: Alt+Click Action
  $description: What to do when Alt+left clicking empty space. Hold Alt and single-click on empty area.
  $options:
    - goUp: Go Up
    - goBack: Go Back
    - goForward: Go Forward
    - refresh: Refresh
    - newTab: New Tab (Win11)
    - duplicateTab: Duplicate Tab (Win11)
    - closeTab: Close Tab (Win11)
    - newFolder: New Folder
    - copyPath: Copy Path
    - paste: Paste
    - customHotkey: Custom Hotkey
    - goToDesktop: Go to Desktop
    - goToHome: Go to Home
    - openInVSCode: Open in VS Code
    - openInTerminal: Open in Terminal
    - openWithContextMenu: Open Context Menu Item
    - none: None
- altClickCustomHotkey: ""
  $name: Alt+Click Custom Hotkey
  $description: "Same format as Double Click Custom Hotkey. Ex: Alt+F4, Alt+Tab"
- altClickContextMenuMatch: ""
  $name: Alt+Click Context Menu Match
  $description: "Match text override for 'Open Context Menu Item' on this trigger. Leave empty to use global setting."
- shiftClickAction: none
  $name: Shift+Click Action
  $description: What to do when Shift+left clicking empty space. Hold Shift and single-click on empty area.
  $options:
    - goUp: Go Up
    - goBack: Go Back
    - goForward: Go Forward
    - refresh: Refresh
    - newTab: New Tab (Win11)
    - duplicateTab: Duplicate Tab (Win11)
    - closeTab: Close Tab (Win11)
    - newFolder: New Folder
    - copyPath: Copy Path
    - paste: Paste
    - customHotkey: Custom Hotkey
    - goToDesktop: Go to Desktop
    - goToHome: Go to Home
    - openInVSCode: Open in VS Code
    - openInTerminal: Open in Terminal
    - openWithContextMenu: Open Context Menu Item
    - none: None
- shiftClickCustomHotkey: ""
  $name: Shift+Click Custom Hotkey
  $description: "Same format as Double Click Custom Hotkey. Ex: Shift+F10, Ctrl+Shift+N"
- shiftClickContextMenuMatch: ""
  $name: Shift+Click Context Menu Match
  $description: "Match text override for 'Open Context Menu Item' on this trigger. Leave empty to use global setting."
- contextMenuMatch: ""
  $name: Context Menu Match
  $description: "Used by the 'Open Context Menu Item' action. Text to match (case-insensitive substring) against the folder background right-click menu entries' display text or verb. Ex: VS Code, Terminal, Git Bash, PowerShell, Cursor. Any program that registered an 'Open in ...' entry works regardless of install path."
*/
// ==/WindhawkModSettings==

#include <initguid.h>
#include <windhawk_utils.h>
#include <windows.h>
#include <shlobj.h>
#include <shobjidl.h>

#include <commctrl.h>
#include <shlwapi.h>
#include <UIAutomationClient.h>
#include <UIAutomationCore.h>
#include <comutil.h>
#include <winrt/base.h>

#include <atomic>
#include <cstdio>
#include <cstdlib>
#include <cwchar>
#include <cwctype>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

using bstr_ptr = _bstr_t;

// ---- Global init/shutdown guard ----

static std::atomic<bool> g_initialized = false;

#define CHECK_INIT_OR_DEFER(hWnd, uMsg, wParam, lParam) \
    if (!g_initialized) return DefSubclassProc(hWnd, uMsg, wParam, lParam);
#define CHECK_INIT_OR_RETURN_VOID()  if (!g_initialized) return

// ---- Settings cache (RAII, thread-safe against settings-changed reload) ----
// Uses WindhawkUtils::StringSetting; reloading is an assignment of a fresh
// make() result. Wh_GetStringSetting never returns NULL (it returns L"" when
// unset or on error), so .get() is always a valid pointer.

static std::mutex g_settingsMutex;
static WindhawkUtils::StringSetting g_doubleClickAction;
static WindhawkUtils::StringSetting g_tripleClickAction;
static WindhawkUtils::StringSetting g_middleClickAction;
static WindhawkUtils::StringSetting g_doubleMiddleClickAction;
static WindhawkUtils::StringSetting g_ctrlClickAction;
static WindhawkUtils::StringSetting g_altClickAction;
static WindhawkUtils::StringSetting g_shiftClickAction;
static WindhawkUtils::StringSetting g_doubleClickCustomCombo;
static WindhawkUtils::StringSetting g_tripleClickCustomCombo;
static WindhawkUtils::StringSetting g_middleClickCustomCombo;
static WindhawkUtils::StringSetting g_doubleMiddleClickCustomCombo;
static WindhawkUtils::StringSetting g_ctrlClickCustomCombo;
static WindhawkUtils::StringSetting g_altClickCustomCombo;
static WindhawkUtils::StringSetting g_shiftClickCustomCombo;
static WindhawkUtils::StringSetting g_doubleClickCtxMatch;
static WindhawkUtils::StringSetting g_tripleClickCtxMatch;
static WindhawkUtils::StringSetting g_middleClickCtxMatch;
static WindhawkUtils::StringSetting g_doubleMiddleClickCtxMatch;
static WindhawkUtils::StringSetting g_ctrlClickCtxMatch;
static WindhawkUtils::StringSetting g_altClickCtxMatch;
static WindhawkUtils::StringSetting g_shiftClickCtxMatch;
static WindhawkUtils::StringSetting g_contextMenuMatch;

static void LoadSettings() {
    std::lock_guard<std::mutex> lock(g_settingsMutex);
    g_doubleClickAction = WindhawkUtils::StringSetting::make(L"doubleClickAction");
    g_tripleClickAction = WindhawkUtils::StringSetting::make(L"tripleClickAction");
    g_middleClickAction = WindhawkUtils::StringSetting::make(L"middleClickAction");
    g_doubleMiddleClickAction = WindhawkUtils::StringSetting::make(L"doubleMiddleClickAction");
    g_ctrlClickAction = WindhawkUtils::StringSetting::make(L"ctrlClickAction");
    g_altClickAction = WindhawkUtils::StringSetting::make(L"altClickAction");
    g_shiftClickAction = WindhawkUtils::StringSetting::make(L"shiftClickAction");
    g_doubleClickCustomCombo = WindhawkUtils::StringSetting::make(L"doubleClickCustomHotkey");
    g_tripleClickCustomCombo = WindhawkUtils::StringSetting::make(L"tripleClickCustomHotkey");
    g_middleClickCustomCombo = WindhawkUtils::StringSetting::make(L"middleClickCustomHotkey");
    g_doubleMiddleClickCustomCombo = WindhawkUtils::StringSetting::make(L"doubleMiddleClickCustomHotkey");
    g_ctrlClickCustomCombo = WindhawkUtils::StringSetting::make(L"ctrlClickCustomHotkey");
    g_altClickCustomCombo = WindhawkUtils::StringSetting::make(L"altClickCustomHotkey");
    g_shiftClickCustomCombo = WindhawkUtils::StringSetting::make(L"shiftClickCustomHotkey");
    g_doubleClickCtxMatch = WindhawkUtils::StringSetting::make(L"doubleClickContextMenuMatch");
    g_tripleClickCtxMatch = WindhawkUtils::StringSetting::make(L"tripleClickContextMenuMatch");
    g_middleClickCtxMatch = WindhawkUtils::StringSetting::make(L"middleClickContextMenuMatch");
    g_doubleMiddleClickCtxMatch = WindhawkUtils::StringSetting::make(L"doubleMiddleClickContextMenuMatch");
    g_ctrlClickCtxMatch = WindhawkUtils::StringSetting::make(L"ctrlClickContextMenuMatch");
    g_altClickCtxMatch = WindhawkUtils::StringSetting::make(L"altClickContextMenuMatch");
    g_shiftClickCtxMatch = WindhawkUtils::StringSetting::make(L"shiftClickContextMenuMatch");
    g_contextMenuMatch = WindhawkUtils::StringSetting::make(L"contextMenuMatch");
}

// Read settings under lock, deep-copy strings so they outlive the lock
struct SettingsSnapshot {
    std::wstring doubleClick;
    std::wstring tripleClick;
    std::wstring middleClick;
    std::wstring doubleMiddleClick;
    std::wstring ctrlClick;
    std::wstring altClick;
    std::wstring shiftClick;
    std::wstring doubleClickCombo;
    std::wstring tripleClickCombo;
    std::wstring middleClickCombo;
    std::wstring doubleMiddleClickCombo;
    std::wstring ctrlClickCombo;
    std::wstring altClickCombo;
    std::wstring shiftClickCombo;
    std::wstring doubleClickCtxMatch;
    std::wstring tripleClickCtxMatch;
    std::wstring middleClickCtxMatch;
    std::wstring doubleMiddleClickCtxMatch;
    std::wstring ctrlClickCtxMatch;
    std::wstring altClickCtxMatch;
    std::wstring shiftClickCtxMatch;
};

static SettingsSnapshot CopySettings() {
    std::lock_guard<std::mutex> lock(g_settingsMutex);
    return {
        g_doubleClickAction.get(),
        g_tripleClickAction.get(),
        g_middleClickAction.get(),
        g_doubleMiddleClickAction.get(),
        g_ctrlClickAction.get(),
        g_altClickAction.get(),
        g_shiftClickAction.get(),
        g_doubleClickCustomCombo.get(),
        g_tripleClickCustomCombo.get(),
        g_middleClickCustomCombo.get(),
        g_doubleMiddleClickCustomCombo.get(),
        g_ctrlClickCustomCombo.get(),
        g_altClickCustomCombo.get(),
        g_shiftClickCustomCombo.get(),
        g_doubleClickCtxMatch.get(),
        g_tripleClickCtxMatch.get(),
        g_middleClickCtxMatch.get(),
        g_doubleMiddleClickCtxMatch.get(),
        g_ctrlClickCtxMatch.get(),
        g_altClickCtxMatch.get(),
        g_shiftClickCtxMatch.get(),
    };
}

static void SendParsedHotkey(const std::wstring& combo);

// Helper: execute custom hotkey if the selected action is "customHotkey"
static bool TryCustomHotkey(PCWSTR action, const std::wstring& combo) {
    if (wcscmp(action, L"customHotkey") != 0) return false;
    if (!combo.empty()) SendParsedHotkey(combo);
    return true;
}

// ---- Custom hotkey parsing ----

static WORD ParseHotkeyToken(const wchar_t* token) {
    if (!token || !token[0]) return 0;
    if (_wcsicmp(token, L"Ctrl") == 0 || _wcsicmp(token, L"Control") == 0) return VK_CONTROL;
    if (_wcsicmp(token, L"Shift") == 0)                         return VK_SHIFT;
    if (_wcsicmp(token, L"Alt") == 0 || _wcsicmp(token, L"Menu") == 0) return VK_MENU;
    if (_wcsicmp(token, L"Win") == 0 || _wcsicmp(token, L"Windows") == 0) return VK_LWIN;
    if (wcslen(token) == 1) {
        if (token[0] >= L'A' && token[0] <= L'Z') return (WORD)token[0];
        if (token[0] >= L'a' && token[0] <= L'z') return (WORD)(token[0] - L'a' + L'A');
        if (token[0] >= L'0' && token[0] <= L'9') return (WORD)token[0];
    }
    if (_wcsicmp(token, L"Tab") == 0)        return VK_TAB;
    if (_wcsicmp(token, L"Enter") == 0 || _wcsicmp(token, L"Return") == 0) return VK_RETURN;
    if (_wcsicmp(token, L"Space") == 0)      return VK_SPACE;
    if (_wcsicmp(token, L"Backspace") == 0)  return VK_BACK;
    if (_wcsicmp(token, L"Delete") == 0 || _wcsicmp(token, L"Del") == 0) return VK_DELETE;
    if (_wcsicmp(token, L"Escape") == 0 || _wcsicmp(token, L"Esc") == 0) return VK_ESCAPE;
    if (_wcsicmp(token, L"Left") == 0)       return VK_LEFT;
    if (_wcsicmp(token, L"Right") == 0)      return VK_RIGHT;
    if (_wcsicmp(token, L"Up") == 0)         return VK_UP;
    if (_wcsicmp(token, L"Down") == 0)       return VK_DOWN;
    if (_wcsicmp(token, L"Home") == 0)       return VK_HOME;
    if (_wcsicmp(token, L"End") == 0)        return VK_END;
    if (_wcsicmp(token, L"PageUp") == 0)     return VK_PRIOR;
    if (_wcsicmp(token, L"PageDown") == 0)   return VK_NEXT;
    if (_wcsicmp(token, L"Insert") == 0 || _wcsicmp(token, L"Ins") == 0) return VK_INSERT;
    // F1-F24
    if ((token[0] == L'F' || token[0] == L'f') && token[1]) {
        int n = (int)wcstol(token + 1, NULL, 10);
        if (n >= 1 && n <= 24) return VK_F1 + (n - 1);
    }
    return 0;
}

// Release held side-modifiers, build combo, restore — all in one SendInput.
// Factored out so both SendKeyCombo and SendParsedHotkey use it.
static void InjectKeys(std::vector<INPUT>& in) {
    static constexpr WORD kSideMods[] = {VK_LCONTROL, VK_RCONTROL, VK_LMENU,
                                         VK_RMENU, VK_LSHIFT, VK_RSHIFT};
    std::vector<WORD> held;
    for (WORD vk : kSideMods)
        if (GetKeyState(vk) & 0x8000) held.push_back(vk);

    std::vector<INPUT> batch;
    auto Key = [&](WORD vk, DWORD flags) {
        batch.push_back(INPUT{INPUT_KEYBOARD, {.ki = {.wVk = vk, .dwFlags = flags}}});
    };
    for (WORD vk : held) Key(vk, KEYEVENTF_KEYUP);
    batch.insert(batch.end(), in.begin(), in.end());
    for (WORD vk : held) Key(vk, 0);

    SendInput((UINT)batch.size(), batch.data(), sizeof(INPUT));
}

static void SendKeyCombo(WORD vk1, WORD vk2, WORD vk3 = 0) {
    std::vector<INPUT> in;
    auto Key = [&](WORD vk, DWORD flags) {
        in.push_back(INPUT{INPUT_KEYBOARD, {.ki = {.wVk = vk, .dwFlags = flags}}});
    };
    Key(vk1, 0); Key(vk2, 0); if (vk3) Key(vk3, 0);
    if (vk3) Key(vk3, KEYEVENTF_KEYUP);
    Key(vk2, KEYEVENTF_KEYUP); Key(vk1, KEYEVENTF_KEYUP);
    InjectKeys(in);
}

static void SendParsedHotkey(const std::wstring& combo) {
    auto trim = [](std::wstring s) {
        size_t a = s.find_first_not_of(L" \t");
        if (a == std::wstring::npos) return std::wstring();
        size_t b = s.find_last_not_of(L" \t");
        return s.substr(a, b - a + 1);
    };
    std::vector<WORD> keys;
    size_t start = 0;
    while (start < combo.length()) {
        size_t end = combo.find(L'+', start);
        std::wstring token = trim((end == std::wstring::npos)
            ? combo.substr(start) : combo.substr(start, end - start));
        if (token.empty()) { if (end == std::wstring::npos) break; start = end + 1; continue; }
        WORD vk = ParseHotkeyToken(token.c_str());
        if (vk) keys.push_back(vk);
        if (end == std::wstring::npos) break;
        start = end + 1;
    }
    if (keys.empty() || keys.size() > 4) return;

    std::vector<INPUT> in;
    auto Key = [&](WORD vk, DWORD flags) {
        in.push_back(INPUT{INPUT_KEYBOARD, {.ki = {.wVk = vk, .dwFlags = flags}}});
    };
    for (size_t i = 0; i < keys.size(); i++) Key(keys[i], 0);
    for (int i = (int)keys.size() - 1; i >= 0; i--) Key(keys[i], KEYEVENTF_KEYUP);
    InjectKeys(in);
}

// ---- Invoke a folder background context menu entry (borrow the right-click menu) ----
// Reuses exactly what Explorer shows when you right-click empty space in a folder, so any
// program that registered an "Open in ..." (or similar) verb works without hard-coded paths.
// matchText is matched (case-insensitive, space-insensitive) against both the menu display
// text and the underlying verb (e.g. "VS Code" matches verb "VSCode" or text "Open with Code").
//
// SAFETY NOTE: Called from Explorer UI thread subclass proc — COM must already be initialized
// (Explorer does that internally). All PIDL/interface clean-up is manual and exception-safe
// (no C++ exceptions; early-return via if-guards). No window subclassing is used: cascading
// submenus are populated by calling IContextMenu2::HandleMenuMsg(WM_INITMENUPOPUP, ...) directly.

// Normalize: lowercase + drop whitespace + drop & accelerator markers.
// "Open Git Ba&sh here" → "opengitbashhere", so user typing "gitbash" matches.
// "通过 Code 打开" → "通过code打开", so user typing "code" or "通过code" matches.
static std::wstring NormalizeForMatch(PCWSTR s) {
    std::wstring out;
    if (!s) return out;
    out.reserve(wcslen(s) + 1);
    for (const wchar_t* p = s; *p; ++p) {
        if (iswspace((wint_t)*p) || *p == L'&') continue;
        out.push_back(*p);
    }
    // Unicode-aware, locale-independent lowercasing so exact matches also work
    // for Cyrillic/Greek/accented text (towlower only folds ASCII in the "C"
    // locale, which would silently break the case-insensitive promise).
    if (!out.empty())
        CharLowerBuffW(&out[0], (DWORD)out.size());
    return out;
}

// Case-insensitive substring test after normalization.
// This lets "VS Code" match the verb "VSCode" and "Open with Code" match "Code", etc.
static bool StrContainsNorm(PCWSTR haystack, PCWSTR needle) {
    if (!haystack || !needle || !*needle) return false;
    std::wstring h = NormalizeForMatch(haystack);
    std::wstring n = NormalizeForMatch(needle);
    // A match string that normalizes to empty (e.g. a stray space, "&", or a tab)
    // would otherwise make StrStrIW succeed for the first entry it walks — don't.
    if (n.empty()) return false;
    return StrStrIW(h.c_str(), n.c_str()) != NULL;
}

// Recursively walk the (possibly nested) context menu, invoking the first item whose
// display text or verb contains matchText. Returns true and invokes on success.
// If dumpLines is non-null, collects diagnostic lines during the walk so the caller
// doesn't need a second walk on failure (avoids re-running shell extensions).
static bool EnumContextMenuMatch(HMENU hMenu, IContextMenu* pcm, IContextMenu2* pcm2,
                                 HWND hwnd, PCWSTR matchText, int idCmdFirst,
                                 std::vector<std::wstring>* dumpLines = nullptr,
                                 int depth = 0) {
    std::wstring normMatch = NormalizeForMatch(matchText);
    if (normMatch.empty()) return false;

    int count = GetMenuItemCount(hMenu);

    // Collect this level's leaf items and submenus (in menu order) and build the
    // diagnostic dump. Matching runs afterwards in three passes so that:
    //   1) an exact (normalized text/verb) hit beats a substring hit — typing the
    //      full entry name is immune to "Encode"/"Decode"-style misfires;
    //   2) top-level entries are tested before nested ones, as the README promises
    //      ("first one in menu order"), which also avoids expanding submenus when a
    //      top-level entry already matches.
    struct LeafItem { UINT offset; std::wstring text, verb, normText, normVerb; };
    struct SubmenuItem { int index; HMENU hSubMenu; };
    std::vector<LeafItem> leaves;
    std::vector<SubmenuItem> submenus;

    for (int i = 0; i < count; i++) {
        MENUITEMINFOW mii = { sizeof(mii) };
        mii.fMask = MIIM_ID | MIIM_SUBMENU;
        if (!GetMenuItemInfoW(hMenu, i, TRUE, &mii)) continue;

        if (mii.hSubMenu != NULL) {
            if (dumpLines) {
                wchar_t stext[MAX_PATH] = {};
                MENUITEMINFOW miiT = { sizeof(miiT), MIIM_STRING };
                miiT.dwTypeData = stext; miiT.cch = MAX_PATH;
                GetMenuItemInfoW(hMenu, i, TRUE, &miiT);
                wchar_t buf[512];
                _snwprintf_s(buf, _countof(buf), _TRUNCATE,
                    L"CMENU%s[%s] (submenu) wID=%u",
                    std::wstring(depth, L' ').c_str(),
                    stext[0] ? stext : L"(no text)", mii.wID);
                dumpLines->push_back(buf);
            }
            submenus.push_back({ i, mii.hSubMenu });
            continue;
        }

        // Leaf item: skip separators and items outside the context-menu command range.
        if (mii.wID == 0) continue;
        if (mii.wID < (UINT)idCmdFirst || mii.wID > 0x7FFF) continue;

        UINT offset = mii.wID - idCmdFirst;

        wchar_t verb[MAX_PATH] = {};
        // Prefer the wide form; fall back to ANSI + CP_ACP only if the verb
        // handler doesn't support GCS_VERBW. This avoids the wide-into-ANSI
        // round trip that corrupts verbs containing non-ANSI characters.
        if (FAILED(pcm->GetCommandString(offset, GCS_VERBW, NULL, (CHAR*)verb, MAX_PATH))) {
            CHAR verbA[MAX_PATH] = {};
            if (SUCCEEDED(pcm->GetCommandString(offset, GCS_VERBA, NULL, verbA, MAX_PATH)))
                MultiByteToWideChar(CP_ACP, 0, verbA, -1, verb, MAX_PATH);
        }

        wchar_t text[MAX_PATH] = {};
        MENUITEMINFOW miiT = { sizeof(miiT), MIIM_STRING };
        miiT.dwTypeData = text; miiT.cch = MAX_PATH;
        GetMenuItemInfoW(hMenu, i, TRUE, &miiT);

        leaves.push_back({ offset, text, verb,
                           NormalizeForMatch(text), NormalizeForMatch(verb) });

        if (dumpLines) {
            wchar_t buf[640];
            _snwprintf_s(buf, _countof(buf), _TRUNCATE,
                L"CMENU%s[%s] wID=%u offset=%u verb=[%s]  → match: \"%s\" or \"%s\"",
                std::wstring(depth, L' ').c_str(),
                text[0] ? text : L"(no text)", mii.wID, offset,
                verb[0] ? verb : L"(none)",
                leaves.back().normText.c_str(),
                leaves.back().normVerb[0] ? leaves.back().normVerb.c_str() : L"<no verb>");
            dumpLines->push_back(buf);
        }
    }

    auto invokeLeaf = [&](const LeafItem& it) -> bool {
        CMINVOKECOMMANDINFO ci = { sizeof(ci) };
        ci.hwnd = hwnd;
        ci.lpVerb = MAKEINTRESOURCEA(it.offset);
        ci.nShow = SW_SHOWNORMAL;
        return SUCCEEDED(pcm->InvokeCommand(&ci));
    };

    // Pass 1: exact match on the normalized text or verb.
    for (const auto& it : leaves) {
        if (it.normText == normMatch || it.normVerb == normMatch) {
            if (invokeLeaf(it)) return true;
        }
    }
    // Pass 2: substring match.
    for (const auto& it : leaves) {
        if (StrContainsNorm(it.text.c_str(), matchText) ||
            StrContainsNorm(it.verb.c_str(), matchText)) {
            if (invokeLeaf(it)) return true;
        }
    }
    // Pass 3: descend into submenus in menu order.
    for (const auto& sm : submenus) {
        if (pcm2)
            pcm2->HandleMenuMsg(WM_INITMENUPOPUP, (WPARAM)sm.hSubMenu, MAKELPARAM(sm.index, 0));
        if (EnumContextMenuMatch(sm.hSubMenu, pcm, pcm2, hwnd, matchText, idCmdFirst,
                                 dumpLines, depth + 2))
            return true;
    }
    return false;
}

// Get the background context menu directly from the shell view.
// Works for virtual folders (This PC, Libraries, Recycle Bin, search results)
// where SHGetPathFromIDListW returns FALSE.
static bool InvokeFolderContextMenuFromBrowser(IShellBrowser* browser, HWND hwnd, PCWSTR matchText) {
    if (!browser || !matchText || !matchText[0]) return false;

    IShellView* psv = nullptr;
    if (FAILED(browser->QueryActiveShellView(&psv)) || !psv) return false;

    IContextMenu* pcm = nullptr;
    HRESULT hr = psv->GetItemObject(SVGIO_BACKGROUND, IID_IContextMenu, (void**)&pcm);
    psv->Release();
    if (FAILED(hr) || !pcm) return false;

    IContextMenu2* pcm2 = nullptr;
    pcm->QueryInterface(IID_IContextMenu2, (void**)&pcm2);

    bool found = false;
    HMENU hMenu = CreatePopupMenu();
    if (hMenu && SUCCEEDED(pcm->QueryContextMenu(hMenu, 0, 1, 0x7FFF, CMF_NORMAL))) {
        std::vector<std::wstring> dumpLines;
        found = EnumContextMenuMatch(hMenu, pcm, pcm2, hwnd, matchText, 1, &dumpLines);
        if (!found) {
            Wh_Log(L"No match for '%s' — dumping all context menu items:", matchText);
            for (auto& line : dumpLines) Wh_Log(L"%s", line.c_str());
        }
    }
    if (hMenu) DestroyMenu(hMenu);
    if (pcm2) pcm2->Release();
    pcm->Release();
    return found;
}

// Legacy path-based fallback — only used by brand-specific actions (OpenInVSCode etc.)
// which are kept for backward compatibility. The browser-based version above is preferred.
// ---- Duplicate Tab infrastructure ----

static thread_local wchar_t g_pendingNavPath[MAX_PATH] = {};
// Raw pointer with explicit AddRef/Release (instead of winrt::com_ptr) so the
// thread-local has a trivial destructor — a com_ptr would register a thread-exit
// destructor living in the mod image. Released on the window's own thread via
// ReleasePendingNavForThread (g_msgTeardown / WM_NCDESTROY), never from
// Wh_ModUninit's arbitrary thread.
static thread_local IShellBrowser* g_pendingNavBrowser = nullptr;

// Release the per-thread pending-nav browser and clear the pending path.
// Must run on the thread that stored them (the Explorer UI thread).
static void ReleasePendingNavForThread() {
    if (g_pendingNavBrowser) {
        g_pendingNavBrowser->Release();
        g_pendingNavBrowser = nullptr;
    }
    g_pendingNavPath[0] = L'\0';
}

static VOID CALLBACK NavigateNewTabProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);
static VOID CALLBACK MidClickTimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);
static VOID CALLBACK DblClickTimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);

// ---- Middle-click double-click detection (timer-based) ----
// Single-click fires immediately if only single is configured.
// If both single and double are configured, single is delayed by
// GetDoubleClickTime() (~500ms) to detect double clicks.

static thread_local HWND g_midClickPendingHwnd = NULL;
static thread_local UINT_PTR g_midClickTimerId = 0;

// Pending double-click timer — used when triple-click is configured.
// When triple-click is enabled, double-click is delayed by GetDoubleClickTime()
// so a third click can arrive and override it with the triple-click action.
static thread_local HWND g_pendingDblClickHwnd = NULL;
static thread_local UINT_PTR g_pendingDblClickTimerId = 0;
static thread_local std::wstring g_pendingDblClickAction;
static thread_local std::wstring g_pendingDblClickCombo;
// Per-trigger context-menu match for the delayed double-click path. Kept separate
// from g_pendingDblClickAction so the two never alias each other.
static thread_local std::wstring g_pendingDblClickCtxMatch;

// Private message: dequeues action dispatch from mouse handlers to avoid
// blocking on COM activation inside WM_LBUTTONDOWN/WM_MBUTTONDOWN.
// It carries NO pointer in wParam — the action data lives in a thread-local
// queue (g_pendingActions) owned by the same thread that posts and dispatches
// it, so a message from another process or a stale post can never make
// explorer.exe free an arbitrary address.
static UINT g_msgDoAction = 0;
// Teardown message: handled on the window's own thread to kill pending timers
// (timers must be killed on their creating thread) and release per-thread COM.
static UINT g_msgTeardown = 0;

// Carries an action plus an optional context-menu match string across the
// posted-message boundary. Using an owned object (instead of a hand-packed
// wchar buffer) avoids the out-of-bounds read the previous layout had when
// no match text was present.
struct PendingAction {
    HWND target;        // the window this action was posted for
    std::wstring action;
    std::wstring match;
};

// Per-thread FIFO backing the deferred-action dispatch. The producer (mouse
// handlers) and the consumer (the g_msgDoAction handler) always run on the
// same Explorer UI thread, so a simple vector is sufficient and we never pass
// an owning pointer through a globally-visible window message. Cleared by the
// DLL's thread_local teardown on mod unload, so a disable/enable cycle cannot
// leak or double-free an allocation from a previous instance.
static thread_local std::vector<PendingAction> g_pendingActions;

static bool FindShellTabAndDoAction(HWND hWnd, PCWSTR action, PCWSTR match = nullptr);

// Post an action to be handled asynchronously — avoids synchronously activating
// context-menu handlers inside the mouse-down handler.
static void PostDoAction(HWND hWnd, PCWSTR action, PCWSTR match = nullptr) {
    if (!g_msgDoAction || !action || !*action) return;
    g_pendingActions.push_back({ hWnd, action, (match && *match) ? match : L"" });
    if (!PostMessage(hWnd, g_msgDoAction, 0, 0))
        g_pendingActions.pop_back();
}

static VOID CALLBACK MidClickTimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime) {
    KillTimer(hwnd, idEvent);
    g_midClickTimerId = 0;
    if (g_midClickPendingHwnd && IsWindow(g_midClickPendingHwnd) && g_initialized) {
        SettingsSnapshot s = CopySettings();
        if (wcscmp(s.middleClick.c_str(), L"none") != 0) {
            if (!TryCustomHotkey(s.middleClick.c_str(), s.middleClickCombo))
                FindShellTabAndDoAction(g_midClickPendingHwnd, s.middleClick.c_str(),
                                        s.middleClickCtxMatch.c_str());
        }
    }
    g_midClickPendingHwnd = NULL;
}

static void CancelPendingMidClick() {
    if (g_midClickTimerId && g_midClickPendingHwnd && IsWindow(g_midClickPendingHwnd)) {
        KillTimer(g_midClickPendingHwnd, g_midClickTimerId);
    }
    g_midClickTimerId = 0;
    g_midClickPendingHwnd = NULL;
}

static VOID CALLBACK DblClickTimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime) {
    KillTimer(hwnd, idEvent);
    CHECK_INIT_OR_RETURN_VOID();
    g_pendingDblClickTimerId = 0;
    if (g_pendingDblClickHwnd && IsWindow(g_pendingDblClickHwnd)) {
        if (!g_pendingDblClickAction.empty()) {
            if (!TryCustomHotkey(g_pendingDblClickAction.c_str(), g_pendingDblClickCombo))
                FindShellTabAndDoAction(g_pendingDblClickHwnd, g_pendingDblClickAction.c_str(),
                                        g_pendingDblClickCtxMatch.c_str());
        }
    }
    g_pendingDblClickHwnd = NULL;
    g_pendingDblClickAction.clear();
    g_pendingDblClickCombo.clear();
    g_pendingDblClickCtxMatch.clear();
}

static void CancelPendingDblClick() {
    if (g_pendingDblClickTimerId && g_pendingDblClickHwnd && IsWindow(g_pendingDblClickHwnd)) {
        KillTimer(g_pendingDblClickHwnd, g_pendingDblClickTimerId);
    }
    g_pendingDblClickTimerId = 0;
    g_pendingDblClickHwnd = NULL;
    g_pendingDblClickAction.clear();
    g_pendingDblClickCombo.clear();
    g_pendingDblClickCtxMatch.clear();
}

// ---- ExplorerWrapper ----

class ExplorerWrapper {
    winrt::com_ptr<IShellBrowser> hBrowser;

    // Helper: get current folder path via IShellView chain
    bool GetCurrentFolderPath(wchar_t* outPath, size_t outLen) {
        IShellView* psv = nullptr;
        if (FAILED(hBrowser->QueryActiveShellView(&psv)) || !psv)
            return false;
        IFolderView* pfv = nullptr;
        if (FAILED(psv->QueryInterface(IID_PPV_ARGS(&pfv)))) {
            psv->Release();
            return false;
        }
        IPersistIDList* pidlList = nullptr;
        if (FAILED(pfv->GetFolder(IID_PPV_ARGS(&pidlList)))) {
            pfv->Release();
            psv->Release();
            return false;
        }
        PIDLIST_ABSOLUTE pidl = nullptr;
        bool ok = SUCCEEDED(pidlList->GetIDList(&pidl)) && pidl
                  && SHGetPathFromIDListW(pidl, outPath) && outPath[0];
        if (pidl) CoTaskMemFree(pidl);
        pidlList->Release();
        pfv->Release();
        psv->Release();
        return ok;
    }

public:
    HWND hShellTab = NULL;
    HWND m_timerHwnd = NULL;  // subclassed HWND for timer messages

    ExplorerWrapper(HWND shellTab, IShellBrowser* hShellBrowser, HWND timerHwnd) {
        hShellTab = shellTab;
        hBrowser.copy_from(hShellBrowser);
        m_timerHwnd = timerHwnd;
    }

    // ---- Navigation ----

    void GoUp() {
        hBrowser->BrowseObject(NULL, SBSP_SAMEBROWSER | SBSP_PARENT);
    }

    void GoBack() {
        hBrowser->BrowseObject(NULL, SBSP_SAMEBROWSER | SBSP_NAVIGATEBACK);
    }

    void GoForward() {
        hBrowser->BrowseObject(NULL, SBSP_SAMEBROWSER | SBSP_NAVIGATEFORWARD);
    }

    void GoToDesktop() {
        PIDLIST_ABSOLUTE pidl = nullptr;
        if (SUCCEEDED(SHGetFolderLocation(NULL, CSIDL_DESKTOP, NULL, 0, &pidl)) && pidl) {
            hBrowser->BrowseObject(pidl, SBSP_SAMEBROWSER | SBSP_ABSOLUTE);
            CoTaskMemFree(pidl);
        }
    }

    void GoToHome() {
        PIDLIST_ABSOLUTE pidl = nullptr;
        if (SUCCEEDED(SHParseDisplayName(
                L"shell:::{679f85cb-0220-4080-b29b-5540cc05aab6}",
                NULL, &pidl, 0, NULL)) && pidl) {
            hBrowser->BrowseObject(pidl, SBSP_SAMEBROWSER | SBSP_ABSOLUTE);
            CoTaskMemFree(pidl);
        }
    }

    // ---- View ----

    void Refresh() {
        IShellView* psv = nullptr;
        if (SUCCEEDED(hBrowser->QueryActiveShellView(&psv)) && psv) {
            psv->Refresh();
            psv->Release();
        }
    }

    // ---- Tab operations ----

    void NewTab() {
        SendKeyCombo(VK_CONTROL, 'T');
    }

    void DuplicateTab() {
        wchar_t path[MAX_PATH] = {};
        if (!GetCurrentFolderPath(path, MAX_PATH)) return;
        wcsncpy(g_pendingNavPath, path, MAX_PATH - 1);
        g_pendingNavPath[MAX_PATH - 1] = L'\0';
        if (g_pendingNavBrowser) {  // release a previously pending browser, if any
            g_pendingNavBrowser->Release();
            g_pendingNavBrowser = nullptr;
        }
        SendKeyCombo(VK_CONTROL, 'T');
        SetTimer(m_timerHwnd, 0x4D43, 500, nullptr);
    }

    void CloseTab() {
        SendKeyCombo(VK_CONTROL, 'W');
    }

    // ---- File operations ----

    void NewFolder() {
        SendKeyCombo(VK_CONTROL, VK_SHIFT, 'N');
    }

    void Paste() {
        SendKeyCombo(VK_CONTROL, 'V');
    }

    void CopyPath() {
        wchar_t path[MAX_PATH] = {};
        if (!GetCurrentFolderPath(path, MAX_PATH)) return;
        size_t len = wcslen(path) + 1;
        HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, len * sizeof(wchar_t));
        if (!hMem) return;
        wchar_t* pMem = (wchar_t*)GlobalLock(hMem);
        if (!pMem) { GlobalFree(hMem); return; }
        wcsncpy(pMem, path, len);
        GlobalUnlock(hMem);
        if (OpenClipboard(NULL)) {
            EmptyClipboard();
            if (!SetClipboardData(CF_UNICODETEXT, hMem))
                GlobalFree(hMem);
            CloseClipboard();
        } else {
            GlobalFree(hMem);
        }
    }

    // ---- External program launchers (via browser — works for virtual folders too) ----

    void OpenInVSCode(PCWSTR match) {
        // "Code" is a loose substring match — entries like "Encode with HandBrake"
        // also contain "code" and may be invoked instead. Route through the generic
        // path so a per-trigger Context Menu Match can override the default; see the
        // README note under "Open in VS Code".
        OpenWithContextMenu(match && *match ? match : L"Code");
    }

    void OpenInTerminal() {
        wchar_t path[MAX_PATH] = {};
        if (!GetCurrentFolderPath(path, MAX_PATH)) {
            Wh_Log(L"OpenInTerminal: no real folder path (virtual folder?), nothing to open");
            return;
        }
        // Launch Windows Terminal directly in the current folder. Avoids the fragile
        // context-menu text/verb matching (which breaks on non-English Windows / updates).
        // A drive root (C:\) ends with a backslash; double it so the closing quote
        // isn't escaped by command-line parsing ("C:\" -> "C:\\").
        std::wstring dir = path;
        if (!dir.empty() && dir.back() == L'\\')
            dir.push_back(L'\\');
        std::wstring cmdLine = L"wt.exe -d \"" + dir + L"\"";
        STARTUPINFOW si = { sizeof(si) };
        PROCESS_INFORMATION pi = {};
        if (CreateProcessW(NULL, cmdLine.data(), NULL, NULL, FALSE, 0, NULL, path, &si, &pi)) {
            CloseHandle(pi.hThread);
            CloseHandle(pi.hProcess);
        } else {
            Wh_Log(L"OpenInTerminal: failed to launch wt.exe (err=%lu)", GetLastError());
        }
    }

    void OpenWithContextMenu(PCWSTR match = nullptr) {
        // match is passed explicitly from the dispatch path (see DoAction). If no
        // per-trigger match was supplied, fall back to the global setting.
        std::wstring m;
        if (match && *match) m = match;
        if (m.empty()) {
            std::lock_guard<std::mutex> lock(g_settingsMutex);
            m = g_contextMenuMatch.get();
        }
        if (m.empty()) {
            Wh_Log(L"OpenWithContextMenu: no match text configured (Context Menu Match setting)");
            return;
        }
        // Use browser directly — works for virtual folders too (This PC, Libraries, etc.)
        // Own any verb UI (Properties, extension dialogs) with the top-level Explorer
        // frame rather than the ShellTabWindowClass child, so the dialog can't end up
        // behind the frame.
        HWND hwndOwner = GetAncestor(hShellTab, GA_ROOT);
        if (!hwndOwner) hwndOwner = hShellTab;
        if (!InvokeFolderContextMenuFromBrowser(hBrowser.get(), hwndOwner, m.c_str()))
            Wh_Log(L"OpenWithContextMenu: no context menu entry matching '%s'", m.c_str());
    }

    // ---- Dispatch ----

    void DoAction(PCWSTR action, PCWSTR match = nullptr) {
        if (!action || !hBrowser || !IsWindow(hShellTab)) return;

        if (wcscmp(action, L"goUp") == 0)            GoUp();
        else if (wcscmp(action, L"goBack") == 0)     GoBack();
        else if (wcscmp(action, L"goForward") == 0)  GoForward();
        else if (wcscmp(action, L"goToDesktop") == 0) GoToDesktop();
        else if (wcscmp(action, L"goToHome") == 0)    GoToHome();
        else if (wcscmp(action, L"refresh") == 0)     Refresh();
        else if (wcscmp(action, L"newTab") == 0)      NewTab();
        else if (wcscmp(action, L"duplicateTab") == 0) DuplicateTab();
        else if (wcscmp(action, L"closeTab") == 0)    CloseTab();
        else if (wcscmp(action, L"newFolder") == 0)   NewFolder();
        else if (wcscmp(action, L"copyPath") == 0)    CopyPath();
        else if (wcscmp(action, L"paste") == 0)       Paste();
        else if (wcscmp(action, L"openInVSCode") == 0)  OpenInVSCode(match);
        else if (wcscmp(action, L"openInTerminal") == 0) OpenInTerminal();
        else if (wcscmp(action, L"openWithContextMenu") == 0) OpenWithContextMenu(match);
        // "none" or unknown — do nothing
    }
};

// ---- Globals ----

// Track subclassed windows so Wh_ModUninit can remove subclasses
// for windows created both during and after init.
struct SubclassEntry { HWND hWnd; bool isListView; };
static std::vector<SubclassEntry> g_subclassed;
static std::mutex g_subclassMutex;

// Record a subclassed window, skipping duplicates (e.g. a view that was
// recreated after a view-mode switch, or a window already picked up by both
// the CreateWindowExW hook and the Wh_ModAfterInit enumeration).
static void TrackSubclassedWindow(HWND hWnd, bool isListView) {
    std::lock_guard<std::mutex> lk(g_subclassMutex);
    for (auto& e : g_subclassed)
        if (e.hWnd == hWnd) return;
    g_subclassed.push_back({ hWnd, isListView });
}

// Per-thread UIAutomation — each Explorer window runs on its own STA thread.
// Stored thread_local and released on the window's own thread (see
// ReleaseUIAutomationForThread / g_msgTeardown): a COM object has thread
// affinity, so it must not be released from Wh_ModUninit's arbitrary thread,
// and Releasing during DLL_PROCESS_DETACH is unsafe too.
static thread_local IUIAutomation* g_pUIAutomation = nullptr;

static IUIAutomation* GetUIAutomation() {
    if (!g_pUIAutomation) {
        CoCreateInstance(CLSID_CUIAutomation, NULL, CLSCTX_INPROC_SERVER,
                         __uuidof(IUIAutomation), (void**)&g_pUIAutomation);
    }
    return g_pUIAutomation;
}

// Release the per-thread UIAutomation. Must be called on the thread that
// created it (the window's own thread), never from Wh_ModUninit.
static void ReleaseUIAutomationForThread() {
    if (g_pUIAutomation) {
        g_pUIAutomation->Release();
        g_pUIAutomation = nullptr;
    }
}

// ---- NavigateNewTabProc (timer callback for duplicate tab) ----

static VOID CALLBACK NavigateNewTabProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime) {
    KillTimer(hwnd, 0x4D43);
    CHECK_INIT_OR_RETURN_VOID();

    if (!g_pendingNavPath[0] || !g_pendingNavBrowser) {
        ReleasePendingNavForThread();
        return;
    }

    PIDLIST_ABSOLUTE pidl = nullptr;
    if (SUCCEEDED(SHParseDisplayName(g_pendingNavPath, NULL, &pidl, 0, NULL)) && pidl) {
        g_pendingNavBrowser->BrowseObject(pidl, SBSP_SAMEBROWSER | SBSP_ABSOLUTE);
        CoTaskMemFree(pidl);
    }

    ReleasePendingNavForThread();
}

// ---- Helper: find ExplorerWrapper by shellTab HWND and run action ----

static bool FindShellTabAndDoAction(HWND hWnd, PCWSTR action, PCWSTR match) {
    if (!hWnd || !IsWindow(hWnd) || !action) {
        Wh_Log(L"FindShellTabAndDoAction: invalid args hWnd=%p action=%s", hWnd, action ? action : L"null");
        return false;
    }
    int limit = 16;
    HWND parent = GetParent(hWnd);
    while (parent && limit-- > 0) {
        wchar_t className[256];
        if (!GetClassName(parent, className, 256)) break;
        if (wcscmp(className, L"ShellTabWindowClass") == 0) {
            HWND shellTab = parent;
            // Query the browser on-demand instead of caching in g_Wrappers
            // (WM_USER+7 returns a borrowed pointer, so copy_from to AddRef)
            winrt::com_ptr<IShellBrowser> browser;
            browser.copy_from(reinterpret_cast<IShellBrowser*>(
                (void*)SendMessage(shellTab, WM_USER + 7, 0, 0)));
            if (browser) {
                ExplorerWrapper tmp(shellTab, browser.get(), hWnd);
                tmp.DoAction(action, match);
                return true;
            }
            Wh_Log(L"FindShellTabAndDoAction: no browser for shellTab=%p, action=%s", shellTab, action);
            break;
        }
        parent = GetParent(parent);
    }
    Wh_Log(L"FindShellTabAndDoAction: ShellTabWindowClass not found from hWnd=%p, action=%s", hWnd, action);
    return false;
}

// ---- SysListView32 subclass ----

LRESULT CALLBACK SysListViewSubclass(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam,
                                      DWORD_PTR dwRefData) {
    // Teardown on the window's own thread, before the init guard so it still
    // runs while Wh_ModUninit is tearing down (g_initialized is already 0 then).
    // Kills pending timers (must be killed on their creating thread) and
    // releases the per-thread UIAutomation.
    if (g_msgTeardown && uMsg == g_msgTeardown) {
        KillTimer(hWnd, 0x4D43);
        KillTimer(hWnd, 0x4D44);
        KillTimer(hWnd, 0x4D45);
        ReleaseUIAutomationForThread();
        ReleasePendingNavForThread();
        return 0;
    }

    // Remove from subclass tracking on destroy and release per-thread COM.
    // Handled before the init guard so it runs even during teardown.
    if (uMsg == WM_NCDESTROY) {
        {
            std::lock_guard<std::mutex> lk(g_subclassMutex);
            std::erase_if(g_subclassed, [hWnd](const SubclassEntry& e) { return e.hWnd == hWnd; });
        }
        ReleaseUIAutomationForThread();
        ReleasePendingNavForThread();
        return DefSubclassProc(hWnd, uMsg, wParam, lParam);
    }

    // Deferred action dispatch (posted from mouse handlers to avoid blocking).
    // Handled before the init guard so a posted action that arrives after the
    // mod was disabled is still drained (the per-thread queue below owns it).
    // The dispatch itself is gated on g_initialized.
    if (g_msgDoAction && uMsg == g_msgDoAction) {
        // Drop stale entries whose target window is gone (e.g. the tab was
        // closed before its posted message was dispatched) so they can't be
        // mis-dispatched to a different window on the same thread.
        while (!g_pendingActions.empty() &&
               !IsWindow(g_pendingActions.front().target))
            g_pendingActions.erase(g_pendingActions.begin());
        if (!g_pendingActions.empty() && g_pendingActions.front().target == hWnd) {
            PendingAction a = std::move(g_pendingActions.front());
            g_pendingActions.erase(g_pendingActions.begin());
            if (g_initialized && !a.action.empty())
                FindShellTabAndDoAction(hWnd, a.action.c_str(), a.match.c_str());
        }
        return 0;
    }

    CHECK_INIT_OR_DEFER(hWnd, uMsg, wParam, lParam);

    // WM_TIMER: handles deferred middle-click, double-click, and duplicate-tab timers.
    // Uses nullptr callback (TIMERPROC lives in mod image, unsafe across unload).
    if (uMsg == WM_TIMER) {
        switch (wParam) {
        case 0x4D43: NavigateNewTabProc(hWnd, uMsg, wParam, 0); return 0;
        case 0x4D44: MidClickTimerProc(hWnd, uMsg, wParam, 0); return 0;
        case 0x4D45: DblClickTimerProc(hWnd, uMsg, wParam, 0); return 0;
        }
        return DefSubclassProc(hWnd, uMsg, wParam, lParam);  // let Explorer handle its own timers
    }

    // Fast path: skip settings copy for messages we don't handle
    if (uMsg != WM_LBUTTONDOWN && uMsg != WM_LBUTTONDBLCLK && uMsg != WM_MBUTTONDOWN)
        return DefSubclassProc(hWnd, uMsg, wParam, lParam);

    SettingsSnapshot s = CopySettings();

    if (uMsg == WM_LBUTTONDOWN) {
        bool ctrlOn   = (wcscmp(s.ctrlClick.c_str(), L"none") != 0);
        bool altOn    = (wcscmp(s.altClick.c_str(), L"none") != 0);
        bool shiftOn  = (wcscmp(s.shiftClick.c_str(), L"none") != 0);
        bool tripleOn = (wcscmp(s.tripleClick.c_str(), L"none") != 0);

        if (!ctrlOn && !altOn && !shiftOn && !tripleOn)
            return DefSubclassProc(hWnd, uMsg, wParam, lParam);

        POINT mousePos;
        GetCursorPos(&mousePos);
        ScreenToClient(hWnd, &mousePos);
        LVHITTESTINFO ht = {};
        ht.flags = LVHT_NOWHERE;
        ht.pt = mousePos;
        if (ListView_SubItemHitTest(hWnd, &ht) != -1)
            return DefSubclassProc(hWnd, uMsg, wParam, lParam); // clicked on an item

        // Triple-click override: a pending double-click exists on this HWND;
        // cancel the double-click and fire triple-click instead.
        if (tripleOn && g_pendingDblClickHwnd == hWnd && g_pendingDblClickTimerId != 0) {
            CancelPendingDblClick();
            if (!TryCustomHotkey(s.tripleClick.c_str(), s.tripleClickCombo))
                PostDoAction(hWnd, s.tripleClick.c_str(), s.tripleClickCtxMatch.c_str());
            return DefSubclassProc(hWnd, uMsg, wParam, lParam);
        }

        // Modifier+click checks
        bool ctrlDown  = (GetKeyState(VK_CONTROL) & 0x8000) != 0;
        bool altDown   = (GetKeyState(VK_MENU) & 0x8000) != 0;
        bool shiftDown = (GetKeyState(VK_SHIFT) & 0x8000) != 0;

        if (ctrlOn && ctrlDown) {
            if (!TryCustomHotkey(s.ctrlClick.c_str(), s.ctrlClickCombo))
                PostDoAction(hWnd, s.ctrlClick.c_str(), s.ctrlClickCtxMatch.c_str());
        } else if (altOn && altDown) {
            if (!TryCustomHotkey(s.altClick.c_str(), s.altClickCombo))
                PostDoAction(hWnd, s.altClick.c_str(), s.altClickCtxMatch.c_str());
        } else if (shiftOn && shiftDown) {
            if (!TryCustomHotkey(s.shiftClick.c_str(), s.shiftClickCombo))
                PostDoAction(hWnd, s.shiftClick.c_str(), s.shiftClickCtxMatch.c_str());
        }

    } else if (uMsg == WM_LBUTTONDBLCLK) {
        bool dblOn    = (wcscmp(s.doubleClick.c_str(), L"none") != 0);
        bool tripleOn = (wcscmp(s.tripleClick.c_str(), L"none") != 0);
        if (!dblOn && !tripleOn)
            return DefSubclassProc(hWnd, uMsg, wParam, lParam);

        POINT mousePos;
        GetCursorPos(&mousePos);
        ScreenToClient(hWnd, &mousePos);
        LVHITTESTINFO ht = {};
        ht.flags = LVHT_NOWHERE;
        ht.pt = mousePos;
        if (ListView_SubItemHitTest(hWnd, &ht) != -1)
            return DefSubclassProc(hWnd, uMsg, wParam, lParam); // clicked on an item

        if (tripleOn) {
            // Delay double-click to wait for possible third click that overrides it.
            // Store empty action when dbl=none — DblClickTimerProc already guards on .empty().
            CancelPendingDblClick();
            g_pendingDblClickHwnd = hWnd;
            g_pendingDblClickAction = dblOn ? s.doubleClick : L"";
            g_pendingDblClickCombo = s.doubleClickCombo;
            g_pendingDblClickCtxMatch = dblOn ? s.doubleClickCtxMatch : L"";
            g_pendingDblClickTimerId = SetTimer(hWnd, 0x4D45,
                GetDoubleClickTime(), nullptr);
        } else {
            // Instant double-click (no triple-click configured)
            if (!TryCustomHotkey(s.doubleClick.c_str(), s.doubleClickCombo))
                PostDoAction(hWnd, s.doubleClick.c_str(), s.doubleClickCtxMatch.c_str());
        }

    } else if (uMsg == WM_MBUTTONDOWN) {
        POINT mousePos;
        GetCursorPos(&mousePos);
        ScreenToClient(hWnd, &mousePos);
        LVHITTESTINFO ht = {};
        ht.flags = LVHT_NOWHERE;
        ht.pt = mousePos;
        if (ListView_SubItemHitTest(hWnd, &ht) != -1)
            return DefSubclassProc(hWnd, uMsg, wParam, lParam);

        bool singleOn = wcscmp(s.middleClick.c_str(), L"none") != 0;
        bool doubleOn = wcscmp(s.doubleMiddleClick.c_str(), L"none") != 0;
        if (!singleOn && !doubleOn)
            return DefSubclassProc(hWnd, uMsg, wParam, lParam);

        if (singleOn && !doubleOn) {
            if (!TryCustomHotkey(s.middleClick.c_str(), s.middleClickCombo))
                PostDoAction(hWnd, s.middleClick.c_str(), s.middleClickCtxMatch.c_str());
            return DefSubclassProc(hWnd, uMsg, wParam, lParam);
        }

        bool isDouble = (g_midClickTimerId != 0 && g_midClickPendingHwnd == hWnd && IsWindow(hWnd));

        if (isDouble) {
            CancelPendingMidClick();
            if (!TryCustomHotkey(s.doubleMiddleClick.c_str(), s.doubleMiddleClickCombo))
                PostDoAction(hWnd, s.doubleMiddleClick.c_str(), s.doubleMiddleClickCtxMatch.c_str());
        } else {
            CancelPendingMidClick();
            g_midClickPendingHwnd = hWnd;
            g_midClickTimerId = SetTimer(hWnd, 0x4D44,
                GetDoubleClickTime(), nullptr);
        }
    }

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

// ---- DirectUI subclass ----

struct ClickHelper {
    DWORD time = 0;
    std::wstring className;
    HWND hWnd = NULL;
};

static thread_local ClickHelper g_lastClick;

LRESULT CALLBACK DUISubclass(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam,
                              DWORD_PTR dwRefData) {
    // Teardown on the window's own thread, before the init guard so it still
    // runs while Wh_ModUninit is tearing down (g_initialized is already 0 then).
    // Kills pending timers (must be killed on their creating thread) and
    // releases the per-thread UIAutomation.
    if (g_msgTeardown && uMsg == g_msgTeardown) {
        KillTimer(hWnd, 0x4D43);
        KillTimer(hWnd, 0x4D44);
        KillTimer(hWnd, 0x4D45);
        ReleaseUIAutomationForThread();
        ReleasePendingNavForThread();
        return 0;
    }

    // Remove from subclass tracking on destroy and release per-thread COM.
    // Handled before the init guard so it runs even during teardown.
    if (uMsg == WM_NCDESTROY) {
        {
            std::lock_guard<std::mutex> lk(g_subclassMutex);
            std::erase_if(g_subclassed, [hWnd](const SubclassEntry& e) { return e.hWnd == hWnd; });
        }
        ReleaseUIAutomationForThread();
        ReleasePendingNavForThread();
        return DefSubclassProc(hWnd, uMsg, wParam, lParam);
    }

    // Deferred action dispatch (posted from mouse handlers to avoid blocking).
    // Handled before the init guard so a posted action that arrives after the
    // mod was disabled is still drained (the per-thread queue below owns it).
    // The dispatch itself is gated on g_initialized.
    if (g_msgDoAction && uMsg == g_msgDoAction) {
        // Drop stale entries whose target window is gone (e.g. the tab was
        // closed before its posted message was dispatched) so they can't be
        // mis-dispatched to a different window on the same thread.
        while (!g_pendingActions.empty() &&
               !IsWindow(g_pendingActions.front().target))
            g_pendingActions.erase(g_pendingActions.begin());
        if (!g_pendingActions.empty() && g_pendingActions.front().target == hWnd) {
            PendingAction a = std::move(g_pendingActions.front());
            g_pendingActions.erase(g_pendingActions.begin());
            if (g_initialized && !a.action.empty())
                FindShellTabAndDoAction(hWnd, a.action.c_str(), a.match.c_str());
        }
        return 0;
    }

    CHECK_INIT_OR_DEFER(hWnd, uMsg, wParam, lParam);

    // WM_TIMER: handles deferred middle-click, double-click, and duplicate-tab timers.
    if (uMsg == WM_TIMER) {
        switch (wParam) {
        case 0x4D43: NavigateNewTabProc(hWnd, uMsg, wParam, 0); return 0;
        case 0x4D44: MidClickTimerProc(hWnd, uMsg, wParam, 0); return 0;
        case 0x4D45: DblClickTimerProc(hWnd, uMsg, wParam, 0); return 0;
        }
        return DefSubclassProc(hWnd, uMsg, wParam, lParam);  // let Explorer handle its own timers
    }

    if (uMsg != WM_PARENTNOTIFY)
        return DefSubclassProc(hWnd, uMsg, wParam, lParam);

    // Fast path: only handle mouse button events
    if (LOWORD(wParam) != WM_LBUTTONDOWN && LOWORD(wParam) != WM_MBUTTONDOWN)
        return DefSubclassProc(hWnd, uMsg, wParam, lParam);

    SettingsSnapshot s = CopySettings();

    // Middle click — timer-based double-click detection
    if (LOWORD(wParam) == WM_MBUTTONDOWN) {
        bool singleOn = wcscmp(s.middleClick.c_str(), L"none") != 0;
        bool doubleOn = wcscmp(s.doubleMiddleClick.c_str(), L"none") != 0;
        if (!singleOn && !doubleOn)
            return DefSubclassProc(hWnd, uMsg, wParam, lParam);

        // Create the per-thread UIAutomation object lazily, only once an action is
        // configured and we actually need to hit-test the click point.
        auto pUIA = GetUIAutomation();
        if (!pUIA)
            return DefSubclassProc(hWnd, uMsg, wParam, lParam);

        POINT mousePos;
        GetCursorPos(&mousePos);
        winrt::com_ptr<IUIAutomationElement> pElement = NULL;
        if (SUCCEEDED(pUIA->ElementFromPoint(mousePos, pElement.put())) && pElement) {
            bstr_ptr clsName;
            if (SUCCEEDED(pElement->get_CurrentClassName(clsName.GetAddress()))) {
                wchar_t* cn = clsName.GetBSTR();
                if (cn && (wcscmp(cn, L"UIGroupItem") == 0 || wcscmp(cn, L"UIItemsView") == 0)) {

                    if (singleOn && !doubleOn) {
                        if (!TryCustomHotkey(s.middleClick.c_str(), s.middleClickCombo))
                            PostDoAction(hWnd, s.middleClick.c_str(), s.middleClickCtxMatch.c_str());
                        return DefSubclassProc(hWnd, uMsg, wParam, lParam);
                    }

                    bool isDouble = (g_midClickTimerId != 0 && g_midClickPendingHwnd == hWnd);

                    if (isDouble) {
                        CancelPendingMidClick();
                        if (!TryCustomHotkey(s.doubleMiddleClick.c_str(), s.doubleMiddleClickCombo))
                            PostDoAction(hWnd, s.doubleMiddleClick.c_str(), s.doubleMiddleClickCtxMatch.c_str());
                    } else {
                        CancelPendingMidClick();
                        g_midClickPendingHwnd = hWnd;
                        g_midClickTimerId = SetTimer(hWnd, 0x4D44,
                            GetDoubleClickTime(), nullptr);
                    }
                }
            }
        }
        return DefSubclassProc(hWnd, uMsg, wParam, lParam);
    }

    // Left click — double-click, triple-click, and modifier+click detection
    if (LOWORD(wParam) == WM_LBUTTONDOWN) {
        bool dblOn    = (wcscmp(s.doubleClick.c_str(), L"none") != 0);
        bool tripleOn = (wcscmp(s.tripleClick.c_str(), L"none") != 0);
        bool ctrlOn   = (wcscmp(s.ctrlClick.c_str(), L"none") != 0);
        bool altOn    = (wcscmp(s.altClick.c_str(), L"none") != 0);
        bool shiftOn  = (wcscmp(s.shiftClick.c_str(), L"none") != 0);

        if (!dblOn && !tripleOn && !ctrlOn && !altOn && !shiftOn)
            return DefSubclassProc(hWnd, uMsg, wParam, lParam);

        // Create the per-thread UIAutomation object lazily, only once an action is
        // configured and we actually need to hit-test the click point.
        auto pUIA = GetUIAutomation();
        if (!pUIA)
            return DefSubclassProc(hWnd, uMsg, wParam, lParam);

        POINT mousePos;
        GetCursorPos(&mousePos);
        winrt::com_ptr<IUIAutomationElement> pElement = NULL;
        if (FAILED(pUIA->ElementFromPoint(mousePos, pElement.put())) || !pElement)
            return DefSubclassProc(hWnd, uMsg, wParam, lParam);

        bstr_ptr clsName;
        if (FAILED(pElement->get_CurrentClassName(clsName.GetAddress())))
            return DefSubclassProc(hWnd, uMsg, wParam, lParam);

        wchar_t* cn = clsName.GetBSTR();
        if (!cn || (wcscmp(cn, L"UIGroupItem") != 0 && wcscmp(cn, L"UIItemsView") != 0))
            return DefSubclassProc(hWnd, uMsg, wParam, lParam); // clicked on an item

        // ========== On empty space ==========

        // 1. Triple-click override: a pending double-click exists on this HWND;
        //    cancel the double-click and fire triple-click instead.
        if (tripleOn && g_pendingDblClickHwnd == hWnd && g_pendingDblClickTimerId != 0) {
            CancelPendingDblClick();
            if (!TryCustomHotkey(s.tripleClick.c_str(), s.tripleClickCombo))
                PostDoAction(hWnd, s.tripleClick.c_str(), s.tripleClickCtxMatch.c_str());
            return DefSubclassProc(hWnd, uMsg, wParam, lParam);
        }

        // 2. Double-click detection
        DWORD now = GetTickCount();

        DWORD delta = now - g_lastClick.time;
        if (hWnd == g_lastClick.hWnd &&
            ((wcscmp(cn, L"UIGroupItem") == 0 &&
              g_lastClick.className == L"UIGroupItem") ||
             (wcscmp(cn, L"UIItemsView") == 0 &&
              g_lastClick.className == L"UIItemsView")) &&
            delta <= (DWORD)GetDoubleClickTime()) {
            // This is a double-click
            if (tripleOn) {
                // Delay to wait for possible third click.
                // Store empty action when dbl=none (DblClickTimerProc guards on .empty()).
                CancelPendingDblClick();
                g_pendingDblClickHwnd = hWnd;
                g_pendingDblClickAction = dblOn ? s.doubleClick : L"";
                g_pendingDblClickCombo = s.doubleClickCombo;
                g_pendingDblClickCtxMatch = dblOn ? s.doubleClickCtxMatch : L"";
                g_pendingDblClickTimerId = SetTimer(hWnd, 0x4D45,
                    GetDoubleClickTime(), nullptr);
            } else if (dblOn) {
                // Instant double-click (no triple-click configured)
                if (!TryCustomHotkey(s.doubleClick.c_str(), s.doubleClickCombo))
                    PostDoAction(hWnd, s.doubleClick.c_str(), s.doubleClickCtxMatch.c_str());
            }
            g_lastClick.time = 0;   // prevent next click from being another double-click
        } else {
            // Single click — check modifier+click combos
            bool ctrlDown  = (GetKeyState(VK_CONTROL) & 0x8000) != 0;
            bool altDown   = (GetKeyState(VK_MENU) & 0x8000) != 0;
            bool shiftDown = (GetKeyState(VK_SHIFT) & 0x8000) != 0;

            if (ctrlOn && ctrlDown) {
                if (!TryCustomHotkey(s.ctrlClick.c_str(), s.ctrlClickCombo))
                    PostDoAction(hWnd, s.ctrlClick.c_str(), s.ctrlClickCtxMatch.c_str());
            } else if (altOn && altDown) {
                if (!TryCustomHotkey(s.altClick.c_str(), s.altClickCombo))
                    PostDoAction(hWnd, s.altClick.c_str(), s.altClickCtxMatch.c_str());
            } else if (shiftOn && shiftDown) {
                if (!TryCustomHotkey(s.shiftClick.c_str(), s.shiftClickCombo))
                    PostDoAction(hWnd, s.shiftClick.c_str(), s.shiftClickCtxMatch.c_str());
            }

            g_lastClick.time = now;
        }

        g_lastClick.hWnd = hWnd;
        g_lastClick.className = cn;
    }

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

// ---- CreateWindowExW hook ----

using CreateWindowExW_t = decltype(&CreateWindowExW);
CreateWindowExW_t CreateWindowExW_original;

HWND WINAPI CreateWindowExW_hook(DWORD dwExStyle, LPCWSTR lpClassName,
                                  LPCWSTR lpWindowName, DWORD dwStyle,
                                  int X, int Y, int nWidth, int nHeight,
                                  HWND hWndParent, HMENU hMenu,
                                  HINSTANCE hInstance, LPVOID lpParam) {
    HWND hWnd = CreateWindowExW_original(dwExStyle, lpClassName, lpWindowName,
                                          dwStyle, X, Y, nWidth, nHeight,
                                          hWndParent, hMenu, hInstance, lpParam);
    if (!hWnd || !g_initialized) return hWnd;

    wchar_t className[256];
    if (!GetClassName(hWnd, className, 256)) return hWnd;
    if (wcscmp(className, L"SysListView32") != 0 &&
        wcscmp(className, L"DirectUIHWND") != 0) return hWnd;

    HWND shellTab = NULL, defView = NULL;
    HWND p = GetParent(hWnd);
    int limit = 16;
    while (p && limit-- > 0) {
        wchar_t pc[256];
        if (!GetClassName(p, pc, 256)) break;
        if (wcscmp(pc, L"SHELLDLL_DefView") == 0) defView = p;
        if (wcscmp(pc, L"ShellTabWindowClass") == 0) { shellTab = p; break; }
        p = GetParent(p);
    }
    if (!shellTab || !defView || !IsWindow(shellTab)) return hWnd;

    if (wcscmp(className, L"SysListView32") == 0) {
        if (WindhawkUtils::SetWindowSubclassFromAnyThread(hWnd, SysListViewSubclass, 0))
            TrackSubclassedWindow(hWnd, true);
    } else {
        if (IsWindow(defView) &&
            WindhawkUtils::SetWindowSubclassFromAnyThread(defView, DUISubclass, 0))
            TrackSubclassedWindow(defView, false);
    }
    return hWnd;
}

// ---- FileCabinet_CreateViewWindow2 hook ----

typedef HRESULT (*__cdecl FileCabinet_CreateViewWindow2_t)(
    IShellBrowser*, void*, IShellView*, IShellView*, void*, HWND*);
FileCabinet_CreateViewWindow2_t FileCabinet_CreateViewWindow2Original;
// Loaded in Wh_ModInit to install the symbol hook, kept loaded for the hook's
// lifetime, freed in Wh_ModUninit to balance the reference.
static HMODULE g_hExplorerFrame = nullptr;

HRESULT __cdecl FileCabinet_CreateViewWindow2Hook(
    IShellBrowser* pBrowser, void* var1, IShellView* psv1,
    IShellView* psv2, void* var2, HWND* hWnd) {
    HRESULT hRes = FileCabinet_CreateViewWindow2Original(pBrowser, var1, psv1, psv2, var2, hWnd);
    if (!g_initialized || FAILED(hRes) || !hWnd || !*hWnd) return hRes;

    HWND shellTab = GetParent(*hWnd);
    if (shellTab && IsWindow(shellTab)) {
        if (g_pendingNavPath[0] && !g_pendingNavBrowser && pBrowser) {
            g_pendingNavBrowser = pBrowser;
            g_pendingNavBrowser->AddRef();
        }
    }
    return hRes;
}

// ---- Enumeration for already-open Explorer windows ----

BOOL CALLBACK InitEnumChildWindowsProc(HWND hWnd, LPARAM lParam) {
    DWORD pid = 0;
    GetWindowThreadProcessId(hWnd, &pid);
    if (pid == GetCurrentProcessId()) {
        wchar_t className[256];
        if (!GetClassName(hWnd, className, 256)) return TRUE;
        if (wcscmp(className, L"SHELLDLL_DefView") == 0) {
            HWND lv = FindWindowEx(hWnd, NULL, L"SysListView32", NULL);
            HWND dui = FindWindowEx(hWnd, NULL, L"DirectUIHWND", NULL);
            if (lv) {
                if (WindhawkUtils::SetWindowSubclassFromAnyThread(lv, SysListViewSubclass, 0)) {
                    Wh_Log(L"SysListView32 Subclassed %p", lv);
                    TrackSubclassedWindow(lv, true);
                }
            } else if (dui) {
                if (WindhawkUtils::SetWindowSubclassFromAnyThread(hWnd, DUISubclass, 0)) {
                    Wh_Log(L"DirectUIHWND Subclassed %p", hWnd);
                    TrackSubclassedWindow(hWnd, false);
                }
            }
            return FALSE;
        }
    }
    return TRUE;
}

BOOL CALLBACK InitEnumWindowsProc(HWND hWnd, LPARAM lParam) {
    DWORD pid = 0;
    GetWindowThreadProcessId(hWnd, &pid);
    if (pid == GetCurrentProcessId()) {
        wchar_t className[256];
        if (!GetClassName(hWnd, className, 256)) return TRUE;
        if (wcscmp(className, L"CabinetWClass") == 0) {
            for (HWND shellTab = FindWindowEx(hWnd, NULL, L"ShellTabWindowClass", NULL);
                 shellTab;
                 shellTab = FindWindowEx(hWnd, shellTab, L"ShellTabWindowClass", NULL)) {
                EnumChildWindows(shellTab, InitEnumChildWindowsProc, (LPARAM)shellTab);
            }
        }
    }
    return TRUE;
}

// ---- Windhawk lifecycle ----

BOOL Wh_ModInit() {
    Wh_Log(L"Click on Empty Explorer Init");

    g_msgDoAction = RegisterWindowMessage(L"ClickOnEmptyExplorer_DoAction_" WH_MOD_ID);
    g_msgTeardown = RegisterWindowMessage(L"ClickOnEmptyExplorer_Teardown_" WH_MOD_ID);
    LoadSettings();

    g_hExplorerFrame = LoadLibraryExW(L"explorerframe.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!g_hExplorerFrame) {
        Wh_Log(L"Failed to load explorerframe.dll");
        return FALSE;
    }

    WindhawkUtils::SYMBOL_HOOK explorerframe_dll_hooks[] = {
        {{
            L"long __cdecl FileCabinet_CreateViewWindow2(struct IShellBrowser *,struct tagFolderSetDataBase *,struct IShellView *,struct IShellView *,struct tagRECT *,struct HWND__ * *)"
        },
        (void**)&FileCabinet_CreateViewWindow2Original,
        (void*)FileCabinet_CreateViewWindow2Hook,
        FALSE}
    };
    if (!WindhawkUtils::HookSymbols(g_hExplorerFrame, explorerframe_dll_hooks, ARRAYSIZE(explorerframe_dll_hooks))) {
        // FileCabinet_CreateViewWindow2 is only used to capture the new tab's
        // IShellBrowser for the Duplicate Tab action. If the symbol can't be
        // resolved (new Windows build, symbol server down), don't disable the
        // whole mod — only Duplicate Tab should degrade. The hook simply isn't
        // installed, so FileCabinet_CreateViewWindow2Original stays null and is
        // never called.
        Wh_Log(L"Failed to hook ExplorerFrame.dll — Duplicate Tab will be unavailable, other actions still work");
        FreeLibrary(g_hExplorerFrame);
        g_hExplorerFrame = nullptr;
    }
    // On success, keep explorerframe.dll loaded so the installed hook stays valid;
    // the reference is released in Wh_ModUninit.

    WindhawkUtils::SetFunctionHook(CreateWindowExW, CreateWindowExW_hook, &CreateWindowExW_original);

    g_initialized.store(true);
    return TRUE;
}

void Wh_ModAfterInit() {
    EnumWindows(InitEnumWindowsProc, 0);
}

void Wh_ModSettingsChanged() {
    LoadSettings();
}

void Wh_ModUninit() {
    // Block subclass callbacks and hook code before cleanup
    g_initialized.store(false);

    // g_pendingNav* are thread_local and owned by the Explorer UI threads; they
    // are released on those threads via the g_msgTeardown / WM_NCDESTROY paths
    // below, not here (this runs on Windhawk's arbitrary thread).

    // Collect subclassed HWNDs under lock, then teardown + remove subclasses
    // outside the lock. Timers use nullptr callback now (WM_TIMER handled in
    // the subclass proc), so no mod-image callback can fire after unload.
    // Send the teardown message so KillTimer + per-thread COM release happen on
    // the window's own thread (a timer must be killed on its creating thread,
    // and the UIAutomation COM object has thread affinity).
    std::vector<SubclassEntry> toRemove;
    {
        std::lock_guard<std::mutex> lk(g_subclassMutex);
        std::swap(toRemove, g_subclassed);
    }
    for (auto& e : toRemove) {
        if (e.hWnd && IsWindow(e.hWnd)) {
            if (g_msgTeardown) SendMessage(e.hWnd, g_msgTeardown, 0, 0);
            WindhawkUtils::RemoveWindowSubclassFromAnyThread(
                e.hWnd, e.isListView ? SysListViewSubclass : DUISubclass);
        }
    }

    // Balance the LoadLibraryExW reference taken in Wh_ModInit. explorerframe.dll
    // stays loaded regardless (Explorer itself holds a reference), so this only
    // releases the mod's own reference — the hooks are gone by now.
    if (g_hExplorerFrame) {
        FreeLibrary(g_hExplorerFrame);
        g_hExplorerFrame = nullptr;
    }
}
