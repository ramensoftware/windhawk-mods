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
or modifier+click (Ctrl/Alt/Shift+Click) on empty space in File Explorer. Supports 14 different
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
- **Open in VS Code** — Invoke the "Open in VS Code" entry from the folder's right-click background context menu (no hard-coded path; works if the verb is registered)
- **Open in Terminal** — Invoke the "Open in Terminal" / Windows Terminal entry from the context menu
- **Open in Cursor** — Invoke the "Open in Cursor" entry from the context menu
- **Open Context Menu Item** — Invoke any right-click background context menu entry by matching its text/verb (configured via "Context Menu Match" setting). Lets you open the folder in Git Bash, PowerShell 7, any editor, etc.
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
context menu programmatically and clicks the first entry that matches your input.

### Matching Rules

Matching is **case-insensitive** and ignores **spaces** and `&` accelerator markers:

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

### Tips

- **Any program** — any program that registered a right-click entry on the folder background works, regardless of install path
- **Multiple matches** — if more than one entry matches, the first one in menu order is used
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
    - openInCursor: Open in Cursor
    - openWithContextMenu: Open Context Menu Item
    - none: None
- doubleClickCustomHotkey: ""
  $name: Double Click Custom Hotkey
  $description: "Format: modifier keys + main key. Modifiers: Ctrl, Shift, Alt, Win (can combine multiple, e.g. Ctrl+Shift+N, Win+Shift+S). Main key: letter, F1-F24, Tab, Enter, Escape, arrows, Backspace, Delete, Home, End, PageUp, PageDown, Insert"
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
    - openInCursor: Open in Cursor
    - openWithContextMenu: Open Context Menu Item
    - none: None
- tripleClickCustomHotkey: ""
  $name: Triple Click Custom Hotkey
  $description: "Same format as Double Click Custom Hotkey. Supports multiple modifiers. Ex: Ctrl+W, Win+D, Ctrl+Shift+Esc"
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
    - openInCursor: Open in Cursor
    - openWithContextMenu: Open Context Menu Item
    - none: None
- middleClickCustomHotkey: ""
  $name: Middle Click Custom Hotkey
  $description: "Same format as Double Click Custom Hotkey. Supports multiple modifiers. Ex: Ctrl+V, Ctrl+T, Win+D, Ctrl+Shift+Esc"
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
    - openInCursor: Open in Cursor
    - openWithContextMenu: Open Context Menu Item
    - none: None
- doubleMiddleClickCustomHotkey: ""
  $name: Double Middle Click Custom Hotkey
  $description: "Same format as Double Click Custom Hotkey. Supports multiple modifiers. Ex: Ctrl+W, Alt+Tab, Win+E, Alt+Shift+F4"
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
    - openInCursor: Open in Cursor
    - openWithContextMenu: Open Context Menu Item
    - none: None
- ctrlClickCustomHotkey: ""
  $name: Ctrl+Click Custom Hotkey
  $description: "Same format as Double Click Custom Hotkey. Ex: Ctrl+N, Ctrl+Shift+E"
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
    - openInCursor: Open in Cursor
    - openWithContextMenu: Open Context Menu Item
    - none: None
- altClickCustomHotkey: ""
  $name: Alt+Click Custom Hotkey
  $description: "Same format as Double Click Custom Hotkey. Ex: Alt+F4, Alt+Tab"
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
    - openInCursor: Open in Cursor
    - openWithContextMenu: Open Context Menu Item
    - none: None
- shiftClickCustomHotkey: ""
  $name: Shift+Click Custom Hotkey
  $description: "Same format as Double Click Custom Hotkey. Ex: Shift+F10, Ctrl+Shift+N"
- contextMenuMatch: ""
  $name: Context Menu Match
  $description: "Used by the 'Open Context Menu Item' action. Text to match (case-insensitive substring) against the folder background right-click menu entries' display text or verb. Ex: VS Code, Terminal, Git Bash, PowerShell, Cursor. Any program that registered an 'Open in ...' entry works regardless of install path."
*/
// ==/WindhawkModSettings==

#include <initguid.h>
#include <windhawk_utils.h>
#include <windows.h>
#include <windowsx.h>
#include <shdeprecated.h>
#include <shlobj.h>
#include <shobjidl.h>

#include <commctrl.h>
#include <shlwapi.h>
#include <UIAutomationClient.h>
#include <UIAutomationCore.h>
#include <comutil.h>
#include <winrt/base.h>

#include <algorithm>
#include <mutex>
#include <string>
#include <vector>

using bstr_ptr = _bstr_t;

// ---- Global init/shutdown guard ----

static volatile LONG g_initialized = 0;

#define CHECK_INIT_OR_DEFER(hWnd, uMsg, wParam, lParam) \
    if (!g_initialized) return DefSubclassProc(hWnd, uMsg, wParam, lParam);
#define CHECK_INIT_OR_RETURN_VOID()  if (!g_initialized) return

// ---- Settings cache (RAII, thread-safe against settings-changed reload) ----

class StringSetting {
    PCWSTR m_str = nullptr;
public:
    void Load(PCWSTR name) {
        if (m_str) Wh_FreeStringSetting(m_str);
        m_str = Wh_GetStringSetting(name);
    }
    ~StringSetting() { if (m_str) Wh_FreeStringSetting(m_str); }
    PCWSTR Get() const { return m_str; }
    StringSetting(const StringSetting&) = delete;
    StringSetting& operator=(const StringSetting&) = delete;
    StringSetting() = default;
};

static std::mutex g_settingsMutex;
static StringSetting g_doubleClickAction;
static StringSetting g_tripleClickAction;
static StringSetting g_middleClickAction;
static StringSetting g_doubleMiddleClickAction;
static StringSetting g_ctrlClickAction;
static StringSetting g_altClickAction;
static StringSetting g_shiftClickAction;
static StringSetting g_doubleClickCustomCombo;
static StringSetting g_tripleClickCustomCombo;
static StringSetting g_middleClickCustomCombo;
static StringSetting g_doubleMiddleClickCustomCombo;
static StringSetting g_ctrlClickCustomCombo;
static StringSetting g_altClickCustomCombo;
static StringSetting g_shiftClickCustomCombo;
static StringSetting g_contextMenuMatch;

static void LoadSettings() {
    std::lock_guard<std::mutex> lock(g_settingsMutex);
    g_doubleClickAction.Load(L"doubleClickAction");
    g_tripleClickAction.Load(L"tripleClickAction");
    g_middleClickAction.Load(L"middleClickAction");
    g_doubleMiddleClickAction.Load(L"doubleMiddleClickAction");
    g_ctrlClickAction.Load(L"ctrlClickAction");
    g_altClickAction.Load(L"altClickAction");
    g_shiftClickAction.Load(L"shiftClickAction");
    g_doubleClickCustomCombo.Load(L"doubleClickCustomHotkey");
    g_tripleClickCustomCombo.Load(L"tripleClickCustomHotkey");
    g_middleClickCustomCombo.Load(L"middleClickCustomHotkey");
    g_doubleMiddleClickCustomCombo.Load(L"doubleMiddleClickCustomHotkey");
    g_ctrlClickCustomCombo.Load(L"ctrlClickCustomHotkey");
    g_altClickCustomCombo.Load(L"altClickCustomHotkey");
    g_shiftClickCustomCombo.Load(L"shiftClickCustomHotkey");
    g_contextMenuMatch.Load(L"contextMenuMatch");
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
};

static SettingsSnapshot CopySettings() {
    std::lock_guard<std::mutex> lock(g_settingsMutex);
    return {
        g_doubleClickAction.Get() ? g_doubleClickAction.Get() : L"",
        g_tripleClickAction.Get() ? g_tripleClickAction.Get() : L"",
        g_middleClickAction.Get() ? g_middleClickAction.Get() : L"",
        g_doubleMiddleClickAction.Get() ? g_doubleMiddleClickAction.Get() : L"",
        g_ctrlClickAction.Get() ? g_ctrlClickAction.Get() : L"",
        g_altClickAction.Get() ? g_altClickAction.Get() : L"",
        g_shiftClickAction.Get() ? g_shiftClickAction.Get() : L"",
        g_doubleClickCustomCombo.Get() ? g_doubleClickCustomCombo.Get() : L"",
        g_tripleClickCustomCombo.Get() ? g_tripleClickCustomCombo.Get() : L"",
        g_middleClickCustomCombo.Get() ? g_middleClickCustomCombo.Get() : L"",
        g_doubleMiddleClickCustomCombo.Get() ? g_doubleMiddleClickCustomCombo.Get() : L"",
        g_ctrlClickCustomCombo.Get() ? g_ctrlClickCustomCombo.Get() : L"",
        g_altClickCustomCombo.Get() ? g_altClickCustomCombo.Get() : L"",
        g_shiftClickCustomCombo.Get() ? g_shiftClickCustomCombo.Get() : L""
    };
}

static void SendParsedHotkey(const std::wstring& combo);

// Helper: execute custom hotkey if the selected action is "customHotkey"
static bool TryCustomHotkey(PCWSTR action, const std::wstring& combo) {
    if (wcscmp(action, L"customHotkey") != 0) return false;
    if (!combo.empty()) SendParsedHotkey(combo);
    return true;
}

// ---- Helper: Send key combination ----

static void SendKeyCombo(WORD vk1, WORD vk2, WORD vk3 = 0) {
    INPUT inputs[6] = {};
    int count = 0;
    auto Press = [&](WORD vk) {
        inputs[count].type = INPUT_KEYBOARD;
        inputs[count].ki.wVk = vk;
        count++;
    };
    auto Release = [&](WORD vk) {
        inputs[count].type = INPUT_KEYBOARD;
        inputs[count].ki.wVk = vk;
        inputs[count].ki.dwFlags = KEYEVENTF_KEYUP;
        count++;
    };
    Press(vk1);
    Press(vk2);
    if (vk3) Press(vk3);
    Release(vk2);
    if (vk3) Release(vk3);
    Release(vk1);
    SendInput(count, inputs, sizeof(INPUT));  // single atomic call
}

// ---- Custom hotkey parsing ----

static int StrICmp(const wchar_t* a, const wchar_t* b) {
    for (; *a && *b; a++, b++) {
        wchar_t ca = (a[0] >= L'a' && a[0] <= L'z') ? a[0] - L'a' + L'A' : a[0];
        wchar_t cb = (b[0] >= L'a' && b[0] <= L'z') ? b[0] - L'a' + L'A' : b[0];
        if (ca != cb) return ca - cb;
    }
    return *a - *b;
}

static WORD ParseHotkeyToken(const wchar_t* token) {
    if (!token || !token[0]) return 0;
    if (StrICmp(token, L"Ctrl") == 0 || StrICmp(token, L"Control") == 0) return VK_CONTROL;
    if (StrICmp(token, L"Shift") == 0)                         return VK_SHIFT;
    if (StrICmp(token, L"Alt") == 0 || StrICmp(token, L"Menu") == 0) return VK_MENU;
    if (StrICmp(token, L"Win") == 0 || StrICmp(token, L"Windows") == 0) return VK_LWIN;
    if (wcslen(token) == 1) {
        if (token[0] >= L'A' && token[0] <= L'Z') return (WORD)token[0];
        if (token[0] >= L'a' && token[0] <= L'z') return (WORD)(token[0] - L'a' + L'A');
        if (token[0] >= L'0' && token[0] <= L'9') return (WORD)token[0];
    }
    if (StrICmp(token, L"Tab") == 0)        return VK_TAB;
    if (StrICmp(token, L"Enter") == 0 || StrICmp(token, L"Return") == 0) return VK_RETURN;
    if (StrICmp(token, L"Space") == 0)      return VK_SPACE;
    if (StrICmp(token, L"Backspace") == 0)  return VK_BACK;
    if (StrICmp(token, L"Delete") == 0 || StrICmp(token, L"Del") == 0) return VK_DELETE;
    if (StrICmp(token, L"Escape") == 0 || StrICmp(token, L"Esc") == 0) return VK_ESCAPE;
    if (StrICmp(token, L"Left") == 0)       return VK_LEFT;
    if (StrICmp(token, L"Right") == 0)      return VK_RIGHT;
    if (StrICmp(token, L"Up") == 0)         return VK_UP;
    if (StrICmp(token, L"Down") == 0)       return VK_DOWN;
    if (StrICmp(token, L"Home") == 0)       return VK_HOME;
    if (StrICmp(token, L"End") == 0)        return VK_END;
    if (StrICmp(token, L"PageUp") == 0)     return VK_PRIOR;
    if (StrICmp(token, L"PageDown") == 0)   return VK_NEXT;
    if (StrICmp(token, L"Insert") == 0 || StrICmp(token, L"Ins") == 0) return VK_INSERT;
    // F1-F24
    if ((token[0] == L'F' || token[0] == L'f') && token[1]) {
        int n = (int)wcstol(token + 1, NULL, 10);
        if (n >= 1 && n <= 24) return VK_F1 + (n - 1);
    }
    return 0;
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
    if (keys.empty() || keys.size() > 4) return; // safety cap

    INPUT inputs[8] = {};
    int count = 0;
    // Press all
    for (size_t i = 0; i < keys.size(); i++) {
        inputs[count].type = INPUT_KEYBOARD;
        inputs[count].ki.wVk = keys[i];
        count++;
    }
    // Release in reverse
    for (int i = (int)keys.size() - 1; i >= 0; i--) {
        inputs[count].type = INPUT_KEYBOARD;
        inputs[count].ki.wVk = keys[i];
        inputs[count].ki.dwFlags = KEYEVENTF_KEYUP;
        count++;
    }
    SendInput(count, inputs, sizeof(INPUT));  // single atomic call
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
        out.push_back((wchar_t)towlower((wint_t)*p));
    }
    return out;
}

// Case-insensitive substring test after normalization.
// This lets "VS Code" match the verb "VSCode" and "Open with Code" match "Code", etc.
static bool StrContainsNorm(PCWSTR haystack, PCWSTR needle) {
    if (!haystack || !needle || !*needle) return false;
    std::wstring h = NormalizeForMatch(haystack);
    std::wstring n = NormalizeForMatch(needle);
    return StrStrIW(h.c_str(), n.c_str()) != NULL;
}

// Recursively walk the (possibly nested) context menu, invoking the first item whose
// display text or verb contains matchText. Returns true and invokes on success.
static bool EnumContextMenuMatch(HMENU hMenu, IContextMenu* pcm, IContextMenu2* pcm2,
                                 HWND hwnd, PCWSTR matchText, int idCmdFirst) {
    int count = GetMenuItemCount(hMenu);
    for (int i = 0; i < count; i++) {
        MENUITEMINFOW mii = { sizeof(mii) };
        mii.fMask = MIIM_ID | MIIM_SUBMENU;
        if (!GetMenuItemInfoW(hMenu, i, TRUE, &mii)) continue;

        // Cascading submenu — check before wID guards, since submenu items
        // carry an HMENU as wID (not a valid command ID in the context-menu range).
        if (mii.hSubMenu != NULL) {
            if (pcm2)
                pcm2->HandleMenuMsg(WM_INITMENUPOPUP, (WPARAM)mii.hSubMenu, MAKELPARAM(i, 0));
            if (EnumContextMenuMatch(mii.hSubMenu, pcm, pcm2, hwnd, matchText, idCmdFirst))
                return true;
            continue;
        }

        // Leaf item: skip separators and items outside the context-menu command range.
        if (mii.wID == 0) continue;                                     // separator
        if (mii.wID < (UINT)idCmdFirst || mii.wID > 0x7FFF) continue;  // outside range

        // Read verb + display text.
        UINT offset = mii.wID - idCmdFirst;

        CHAR verbA[MAX_PATH] = {};
        pcm->GetCommandString(offset, GCS_VERBA, NULL, verbA, MAX_PATH);
        wchar_t verb[MAX_PATH] = {};
        if (verbA[0])
            MultiByteToWideChar(CP_ACP, 0, verbA, -1, verb, MAX_PATH);

        wchar_t text[MAX_PATH] = {};
        MENUITEMINFOW miiT = { sizeof(miiT) };
        miiT.fMask = MIIM_STRING;
        miiT.dwTypeData = text;
        miiT.cch = MAX_PATH;
        GetMenuItemInfoW(hMenu, i, TRUE, &miiT);

        if (StrContainsNorm(text, matchText) || StrContainsNorm(verb, matchText)) {
            CMINVOKECOMMANDINFO ci = { sizeof(ci) };
            ci.hwnd = hwnd;
            ci.lpVerb = MAKEINTRESOURCEA(offset);
            ci.nShow = SW_SHOWNORMAL;
            if (SUCCEEDED(pcm->InvokeCommand(&ci)))
                return true;
        }
    }
    return false;
}

// Diagnostic: recursively dump all context-menu leaf items.
// Called when a match fails, so the user can see what verbs/text are available
// and adjust the "Context Menu Match" setting accordingly.
static void DumpContextMenuRecursive(HMENU hMenu, IContextMenu* pcm, IContextMenu2* pcm2,
                                     int idCmdFirst, int depth) {
    int count = GetMenuItemCount(hMenu);
    for (int i = 0; i < count; i++) {
        MENUITEMINFOW mii = { sizeof(mii) };
        mii.fMask = MIIM_ID | MIIM_SUBMENU;
        if (!GetMenuItemInfoW(hMenu, i, TRUE, &mii)) continue;

        wchar_t indent[64] = {};
        for (int d = 0; d < depth && d < 31; d++) indent[d] = L' ';
        indent[depth < 31 ? depth : 31] = 0;

        // Cascading submenu — check before wID guards.
        if (mii.hSubMenu != NULL) {
            wchar_t stext[MAX_PATH] = {};
            MENUITEMINFOW miiT = { sizeof(miiT) };
            miiT.fMask = MIIM_STRING;
            miiT.dwTypeData = stext;
            miiT.cch = MAX_PATH;
            GetMenuItemInfoW(hMenu, i, TRUE, &miiT);
            Wh_Log(L"CMENU%s[%s] (submenu) wID=%u", indent,
                   stext[0] ? stext : L"(no text)", mii.wID);
            if (pcm2)
                pcm2->HandleMenuMsg(WM_INITMENUPOPUP, (WPARAM)mii.hSubMenu, MAKELPARAM(i, 0));
            DumpContextMenuRecursive(mii.hSubMenu, pcm, pcm2, idCmdFirst, depth + 2);
            continue;
        }

        // Leaf item: skip separators and items outside the context-menu command range.
        if (mii.wID == 0) continue;
        if (mii.wID < (UINT)idCmdFirst || mii.wID > 0x7FFF) continue;

        UINT offset = mii.wID - idCmdFirst;
        CHAR verbA[MAX_PATH] = {};
        pcm->GetCommandString(offset, GCS_VERBA, NULL, verbA, MAX_PATH);
        wchar_t verb[MAX_PATH] = {};
        if (verbA[0])
            MultiByteToWideChar(CP_ACP, 0, verbA, -1, verb, MAX_PATH);
        wchar_t text[MAX_PATH] = {};
        MENUITEMINFOW miiT = { sizeof(miiT) };
        miiT.fMask = MIIM_STRING;
        miiT.dwTypeData = text;
        miiT.cch = MAX_PATH;
        GetMenuItemInfoW(hMenu, i, TRUE, &miiT);

        // Show normalized text so user knows exactly what substring to type
        std::wstring normText = NormalizeForMatch(text);
        std::wstring normVerb = NormalizeForMatch(verb);
        Wh_Log(L"CMENU%s[%s] wID=%u offset=%u verb=[%s]  → match: \"%s\" or \"%s\"", indent,
               text[0] ? text : L"(no text)", mii.wID, offset,
               verb[0] ? verb : L"(none)",
               normText.c_str(), normVerb[0] ? normVerb.c_str() : L"<no verb>");
    }
}

static bool InvokeFolderContextMenuVerb(PCWSTR folderPath, HWND hwnd, PCWSTR matchText) {
    if (!folderPath || !folderPath[0] || !matchText || !matchText[0]) return false;

    // SHParseDisplayName returns an absolute PIDL (relative to Desktop root).
    // We bind it directly on the desktop IShellFolder — no PIDL splitting needed.
    PIDLIST_ABSOLUTE pidl = nullptr;
    if (FAILED(SHParseDisplayName(folderPath, NULL, &pidl, 0, NULL)) || !pidl)
        return false;

    IShellFolder* psfDesktop = nullptr;
    if (FAILED(SHGetDesktopFolder(&psfDesktop))) {
        CoTaskMemFree(pidl);
        return false;
    }

    IShellFolder* psfFolder = nullptr;
    HRESULT hr = psfDesktop->BindToObject(pidl, NULL, IID_IShellFolder, (void**)&psfFolder);
    psfDesktop->Release();
    CoTaskMemFree(pidl);
    if (FAILED(hr) || !psfFolder) return false;

    // Background context menu for THIS folder (right-click empty space).
    IContextMenu* pcm = nullptr;
    hr = psfFolder->CreateViewObject(hwnd, IID_IContextMenu, (void**)&pcm);
    psfFolder->Release();
    if (FAILED(hr) || !pcm) return false;

    // IContextMenu2 is needed to populate cascading submenus (WM_INITMENUPOPUP).
    // QueryInterface for IContextMenu2 also succeeds when only IContextMenu3 exists.
    IContextMenu2* pcm2 = nullptr;
    pcm->QueryInterface(IID_IContextMenu2, (void**)&pcm2);

    bool found = false;
    HMENU hMenu = CreatePopupMenu();
    if (hMenu && SUCCEEDED(pcm->QueryContextMenu(hMenu, 0, 1, 0x7FFF, CMF_NORMAL))) {
        found = EnumContextMenuMatch(hMenu, pcm, pcm2, hwnd, matchText, 1);
        if (!found) {
            Wh_Log(L"No match for '%s' — dumping all context menu items:", matchText);
            DumpContextMenuRecursive(hMenu, pcm, pcm2, 1, 0);
        }
    }
    if (hMenu) DestroyMenu(hMenu);
    if (pcm2) pcm2->Release();
    pcm->Release();
    return found;
}

// ---- Duplicate Tab infrastructure ----

static thread_local wchar_t g_pendingNavPath[MAX_PATH] = {};
static thread_local winrt::com_ptr<IShellBrowser> g_pendingNavBrowser;
static thread_local HWND g_pendingNavHwnd = NULL;

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

// Private message: dequeues action dispatch from mouse handlers to avoid
// blocking on COM activation inside WM_LBUTTONDOWN/WM_MBUTTONDOWN.
// wParam = (WPARAM)strdup(actionString), lParam = (LPARAM)hWnd
static UINT g_msgDoAction = 0;

static bool FindShellTabAndDoAction(HWND hWnd, PCWSTR action);

// Post an action to be handled asynchronously — avoids synchronously activating
// context-menu handlers inside the mouse-down handler.
static void PostDoAction(HWND hWnd, PCWSTR action) {
    if (!g_msgDoAction || !action || !*action) return;
    size_t len = wcslen(action) + 1;
    wchar_t* s = (wchar_t*)HeapAlloc(GetProcessHeap(), 0, len * sizeof(wchar_t));
    if (!s) return;
    wcscpy_s(s, len, action);
    if (!PostMessage(hWnd, g_msgDoAction, (WPARAM)s, (LPARAM)hWnd))
        HeapFree(GetProcessHeap(), 0, s);
}

static VOID CALLBACK MidClickTimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime) {
    KillTimer(hwnd, idEvent);
    g_midClickTimerId = 0;
    if (g_midClickPendingHwnd && IsWindow(g_midClickPendingHwnd) && g_initialized) {
        SettingsSnapshot s = CopySettings();
        if (wcscmp(s.middleClick.c_str(), L"none") != 0) {
            if (!TryCustomHotkey(s.middleClick.c_str(), s.middleClickCombo))
                FindShellTabAndDoAction(g_midClickPendingHwnd, s.middleClick.c_str());
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
                FindShellTabAndDoAction(g_pendingDblClickHwnd, g_pendingDblClickAction.c_str());
        }
    }
    g_pendingDblClickHwnd = NULL;
    g_pendingDblClickAction.clear();
    g_pendingDblClickCombo.clear();
}

static void CancelPendingDblClick() {
    if (g_pendingDblClickTimerId && g_pendingDblClickHwnd && IsWindow(g_pendingDblClickHwnd)) {
        KillTimer(g_pendingDblClickHwnd, g_pendingDblClickTimerId);
    }
    g_pendingDblClickTimerId = 0;
    g_pendingDblClickHwnd = NULL;
    g_pendingDblClickAction.clear();
    g_pendingDblClickCombo.clear();
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

    ExplorerWrapper(HWND shellTab, IShellBrowser* hShellBrowser, HWND timerHwnd = NULL) {
        hShellTab = shellTab;
        hBrowser.copy_from(hShellBrowser);
        m_timerHwnd = timerHwnd;
    }

    // Return a ref-counted copy of the browser for thread-safe access
    winrt::com_ptr<IShellBrowser> GetBrowser() const { return hBrowser; }

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
        g_pendingNavBrowser = nullptr;
        g_pendingNavHwnd = m_timerHwnd;
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

    // ---- External program launchers (via the folder background context menu) ----
    // These borrow the right-click-on-empty-space menu, so they adapt to whatever
    // program registered the corresponding verb (path-independent).

    void OpenInVSCode() {
        wchar_t path[MAX_PATH] = {};
        if (!GetCurrentFolderPath(path, MAX_PATH)) return;
        // "Code" matches verb "VSCode" and display "Open with Code" / "通过 Code 打开"
        if (!InvokeFolderContextMenuVerb(path, hShellTab, L"Code"))
            Wh_Log(L"OpenInVSCode: no matching context menu entry found");
    }

    void OpenInTerminal() {
        wchar_t path[MAX_PATH] = {};
        if (!GetCurrentFolderPath(path, MAX_PATH)) return;
        if (!InvokeFolderContextMenuVerb(path, hShellTab, L"Terminal"))
            Wh_Log(L"OpenInTerminal: no matching context menu entry found");
    }

    void OpenInCursor() {
        wchar_t path[MAX_PATH] = {};
        if (!GetCurrentFolderPath(path, MAX_PATH)) return;
        if (!InvokeFolderContextMenuVerb(path, hShellTab, L"Cursor"))
            Wh_Log(L"OpenInCursor: no matching context menu entry found");
    }

    void OpenWithContextMenu() {
        // Thread-safe read: copy the match string under the settings lock so
        // a concurrent Wh_ModSettingsChanged() cannot free it mid-use (UAF).
        std::wstring match;
        {
            std::lock_guard<std::mutex> lock(g_settingsMutex);
            PCWSTR s = g_contextMenuMatch.Get();
            if (s) match = s;
        }
        if (match.empty()) {
            Wh_Log(L"OpenWithContextMenu: no match text configured (Context Menu Match setting)");
            return;
        }
        wchar_t path[MAX_PATH] = {};
        if (!GetCurrentFolderPath(path, MAX_PATH)) return;
        if (!InvokeFolderContextMenuVerb(path, hShellTab, match.c_str()))
            Wh_Log(L"OpenWithContextMenu: no context menu entry matching '%s'", match.c_str());
    }

    // ---- Dispatch ----

    void DoAction(PCWSTR action) {
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
        else if (wcscmp(action, L"openInVSCode") == 0)  OpenInVSCode();
        else if (wcscmp(action, L"openInTerminal") == 0) OpenInTerminal();
        else if (wcscmp(action, L"openInCursor") == 0)   OpenInCursor();
        else if (wcscmp(action, L"openWithContextMenu") == 0) OpenWithContextMenu();
        // "none" or unknown — do nothing
    }
};

// ---- Globals ----

// Track subclassed windows so Wh_ModUninit can remove subclasses
// for windows created both during and after init.
struct SubclassEntry { HWND hWnd; bool isListView; };
static std::vector<SubclassEntry> g_subclassed;
static std::mutex g_subclassMutex;

// Lazily initialized on first use (Explorer UI thread has COM already).
// Intentionally leaked (raw pointer) to avoid Release() during DLL_PROCESS_DETACH.
static IUIAutomation* GetUIAutomation() {
    static IUIAutomation* s_pUIAutomation = []{
        IUIAutomation* p = nullptr;
        CoCreateInstance(CLSID_CUIAutomation, NULL, CLSCTX_INPROC_SERVER,
                         __uuidof(IUIAutomation), (void**)&p);
        return p;
    }();
    return s_pUIAutomation;
}

// ---- NavigateNewTabProc (timer callback for duplicate tab) ----

static VOID CALLBACK NavigateNewTabProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime) {
    KillTimer(hwnd, 0x4D43);
    CHECK_INIT_OR_RETURN_VOID();

    if (!g_pendingNavPath[0] || !g_pendingNavBrowser) {
        g_pendingNavPath[0] = L'\0';
        g_pendingNavBrowser = nullptr;
        g_pendingNavHwnd = NULL;
        return;
    }

    PIDLIST_ABSOLUTE pidl = nullptr;
    if (SUCCEEDED(SHParseDisplayName(g_pendingNavPath, NULL, &pidl, 0, NULL)) && pidl) {
        g_pendingNavBrowser->BrowseObject(pidl, SBSP_SAMEBROWSER | SBSP_ABSOLUTE);
        CoTaskMemFree(pidl);
    }

    g_pendingNavPath[0] = L'\0';
    g_pendingNavBrowser = nullptr;
    g_pendingNavHwnd = NULL;
}

// ---- Helper: find ExplorerWrapper by shellTab HWND and run action ----

static bool FindShellTabAndDoAction(HWND hWnd, PCWSTR action) {
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
                tmp.DoAction(action);
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
    CHECK_INIT_OR_DEFER(hWnd, uMsg, wParam, lParam);

    // Remove from subclass tracking on destroy (even during teardown)
    if (uMsg == WM_NCDESTROY) {
        std::lock_guard<std::mutex> lk(g_subclassMutex);
        std::erase_if(g_subclassed, [hWnd](const SubclassEntry& e) { return e.hWnd == hWnd; });
        return DefSubclassProc(hWnd, uMsg, wParam, lParam);
    }

    // Deferred action dispatch (posted from mouse handlers to avoid blocking)
    if (g_msgDoAction && uMsg == g_msgDoAction) {
        PCWSTR action = (PCWSTR)wParam;
        HWND target = (HWND)lParam;
        if (action && *action && target)
            FindShellTabAndDoAction(target, action);
        HeapFree(GetProcessHeap(), 0, (void*)wParam);
        return 0;
    }

    // WM_TIMER: handles deferred middle-click, double-click, and duplicate-tab timers.
    // Uses nullptr callback (TIMERPROC lives in mod image, unsafe across unload).
    if (uMsg == WM_TIMER) {
        switch (wParam) {
        case 0x4D43: NavigateNewTabProc(hWnd, uMsg, wParam, dwRefData); break;
        case 0x4D44: MidClickTimerProc(hWnd, uMsg, wParam, dwRefData); break;
        case 0x4D45: DblClickTimerProc(hWnd, uMsg, wParam, dwRefData); break;
        }
        return 0;
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
                PostDoAction(hWnd, s.tripleClick.c_str());
            return DefSubclassProc(hWnd, uMsg, wParam, lParam);
        }

        // Modifier+click checks
        bool ctrlDown  = (GetKeyState(VK_CONTROL) & 0x8000) != 0;
        bool altDown   = (GetKeyState(VK_MENU) & 0x8000) != 0;
        bool shiftDown = (GetKeyState(VK_SHIFT) & 0x8000) != 0;

        if (ctrlOn && ctrlDown) {
            if (!TryCustomHotkey(s.ctrlClick.c_str(), s.ctrlClickCombo))
                PostDoAction(hWnd, s.ctrlClick.c_str());
        } else if (altOn && altDown) {
            if (!TryCustomHotkey(s.altClick.c_str(), s.altClickCombo))
                PostDoAction(hWnd, s.altClick.c_str());
        } else if (shiftOn && shiftDown) {
            if (!TryCustomHotkey(s.shiftClick.c_str(), s.shiftClickCombo))
                PostDoAction(hWnd, s.shiftClick.c_str());
        }

    } else if (uMsg == WM_LBUTTONDBLCLK) {
        if (wcscmp(s.doubleClick.c_str(), L"none") == 0)
            return DefSubclassProc(hWnd, uMsg, wParam, lParam);

        POINT mousePos;
        GetCursorPos(&mousePos);
        ScreenToClient(hWnd, &mousePos);
        LVHITTESTINFO ht = {};
        ht.flags = LVHT_NOWHERE;
        ht.pt = mousePos;
        if (ListView_SubItemHitTest(hWnd, &ht) != -1)
            return DefSubclassProc(hWnd, uMsg, wParam, lParam); // clicked on an item

        bool tripleOn = (wcscmp(s.tripleClick.c_str(), L"none") != 0);

        if (tripleOn) {
            // Delay double-click to wait for possible third click that overrides it
            CancelPendingDblClick();
            g_pendingDblClickHwnd = hWnd;
            g_pendingDblClickAction = s.doubleClick;
            g_pendingDblClickCombo = s.doubleClickCombo;
            g_pendingDblClickTimerId = SetTimer(hWnd, 0x4D45,
                GetDoubleClickTime(), nullptr);
        } else {
            // Instant double-click (no triple-click configured)
            if (!TryCustomHotkey(s.doubleClick.c_str(), s.doubleClickCombo))
                PostDoAction(hWnd, s.doubleClick.c_str());
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
                PostDoAction(hWnd, s.middleClick.c_str());
            return DefSubclassProc(hWnd, uMsg, wParam, lParam);
        }

        bool isDouble = (g_midClickTimerId != 0 && g_midClickPendingHwnd == hWnd && IsWindow(hWnd));

        if (isDouble) {
            CancelPendingMidClick();
            if (!TryCustomHotkey(s.doubleMiddleClick.c_str(), s.doubleMiddleClickCombo))
                PostDoAction(hWnd, s.doubleMiddleClick.c_str());
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

static thread_local ClickHelper g_currentClick;
static thread_local ClickHelper g_lastClick;

LRESULT CALLBACK DUISubclass(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam,
                              DWORD_PTR dwRefData) {
    CHECK_INIT_OR_DEFER(hWnd, uMsg, wParam, lParam);

    // Remove from subclass tracking on destroy (even during teardown)
    if (uMsg == WM_NCDESTROY) {
        std::lock_guard<std::mutex> lk(g_subclassMutex);
        std::erase_if(g_subclassed, [hWnd](const SubclassEntry& e) { return e.hWnd == hWnd; });
        return DefSubclassProc(hWnd, uMsg, wParam, lParam);
    }

    // Deferred action dispatch (posted from mouse handlers to avoid blocking)
    if (g_msgDoAction && uMsg == g_msgDoAction) {
        PCWSTR action = (PCWSTR)wParam;
        HWND target = (HWND)lParam;
        if (action && *action && target)
            FindShellTabAndDoAction(target, action);
        HeapFree(GetProcessHeap(), 0, (void*)wParam);
        return 0;
    }

    // WM_TIMER: handles deferred middle-click, double-click, and duplicate-tab timers.
    if (uMsg == WM_TIMER) {
        switch (wParam) {
        case 0x4D43: NavigateNewTabProc(hWnd, uMsg, wParam, dwRefData); break;
        case 0x4D44: MidClickTimerProc(hWnd, uMsg, wParam, dwRefData); break;
        case 0x4D45: DblClickTimerProc(hWnd, uMsg, wParam, dwRefData); break;
        }
        return 0;
    }

    if (uMsg != WM_PARENTNOTIFY)
        return DefSubclassProc(hWnd, uMsg, wParam, lParam);

    // Fast path: only handle mouse button events
    if (wParam != WM_LBUTTONDOWN && wParam != WM_MBUTTONDOWN)
        return DefSubclassProc(hWnd, uMsg, wParam, lParam);

    auto pUIA = GetUIAutomation();
    if (!pUIA)
        return DefSubclassProc(hWnd, uMsg, wParam, lParam);

    SettingsSnapshot s = CopySettings();

    // Middle click — timer-based double-click detection
    if (wParam == WM_MBUTTONDOWN) {
        bool singleOn = wcscmp(s.middleClick.c_str(), L"none") != 0;
        bool doubleOn = wcscmp(s.doubleMiddleClick.c_str(), L"none") != 0;
        if (!singleOn && !doubleOn)
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
                            PostDoAction(hWnd, s.middleClick.c_str());
                        return DefSubclassProc(hWnd, uMsg, wParam, lParam);
                    }

                    bool isDouble = (g_midClickTimerId != 0 && g_midClickPendingHwnd == hWnd);

                    if (isDouble) {
                        CancelPendingMidClick();
                        if (!TryCustomHotkey(s.doubleMiddleClick.c_str(), s.doubleMiddleClickCombo))
                            PostDoAction(hWnd, s.doubleMiddleClick.c_str());
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
    if (wParam == WM_LBUTTONDOWN) {
        bool dblOn    = (wcscmp(s.doubleClick.c_str(), L"none") != 0);
        bool tripleOn = (wcscmp(s.tripleClick.c_str(), L"none") != 0);
        bool ctrlOn   = (wcscmp(s.ctrlClick.c_str(), L"none") != 0);
        bool altOn    = (wcscmp(s.altClick.c_str(), L"none") != 0);
        bool shiftOn  = (wcscmp(s.shiftClick.c_str(), L"none") != 0);

        if (!dblOn && !tripleOn && !ctrlOn && !altOn && !shiftOn)
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
                PostDoAction(hWnd, s.tripleClick.c_str());
            return DefSubclassProc(hWnd, uMsg, wParam, lParam);
        }

        // 2. Double-click detection
        DWORD now = GetTickCount();
        g_currentClick.time = now;
        g_currentClick.hWnd = hWnd;
        g_currentClick.className = cn;

        DWORD delta = now - g_lastClick.time;
        if (g_currentClick.hWnd == g_lastClick.hWnd &&
            ((wcscmp(cn, L"UIGroupItem") == 0 &&
              g_lastClick.className == L"UIGroupItem") ||
             (wcscmp(cn, L"UIItemsView") == 0 &&
              g_lastClick.className == L"UIItemsView")) &&
            delta <= (DWORD)GetDoubleClickTime()) {
            // This is a double-click
            if (dblOn) {
                if (tripleOn) {
                    // Delay double-click to wait for possible third click
                    CancelPendingDblClick();
                    g_pendingDblClickHwnd = hWnd;
                    g_pendingDblClickAction = s.doubleClick;
                    g_pendingDblClickCombo = s.doubleClickCombo;
                    g_pendingDblClickTimerId = SetTimer(hWnd, 0x4D45,
                        GetDoubleClickTime(), nullptr);
                } else {
                    // Instant double-click (no triple-click configured)
                    if (!TryCustomHotkey(s.doubleClick.c_str(), s.doubleClickCombo))
                        PostDoAction(hWnd, s.doubleClick.c_str());
                }
            }
            g_lastClick.time = 0;   // prevent next click from being another double-click
        } else {
            // Single click — check modifier+click combos
            bool ctrlDown  = (GetKeyState(VK_CONTROL) & 0x8000) != 0;
            bool altDown   = (GetKeyState(VK_MENU) & 0x8000) != 0;
            bool shiftDown = (GetKeyState(VK_SHIFT) & 0x8000) != 0;

            if (ctrlOn && ctrlDown) {
                if (!TryCustomHotkey(s.ctrlClick.c_str(), s.ctrlClickCombo))
                    PostDoAction(hWnd, s.ctrlClick.c_str());
            } else if (altOn && altDown) {
                if (!TryCustomHotkey(s.altClick.c_str(), s.altClickCombo))
                    PostDoAction(hWnd, s.altClick.c_str());
            } else if (shiftOn && shiftDown) {
                if (!TryCustomHotkey(s.shiftClick.c_str(), s.shiftClickCombo))
                    PostDoAction(hWnd, s.shiftClick.c_str());
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
        WindhawkUtils::SetWindowSubclassFromAnyThread(hWnd, SysListViewSubclass, 0);
        { std::lock_guard<std::mutex> lk(g_subclassMutex);
          g_subclassed.push_back({ hWnd, true });
        }
    } else {
        if (IsWindow(defView)) {
            WindhawkUtils::SetWindowSubclassFromAnyThread(defView, DUISubclass, 0);
            { std::lock_guard<std::mutex> lk(g_subclassMutex);
              g_subclassed.push_back({ defView, false });
            }
        }
    }
    return hWnd;
}

// ---- FileCabinet_CreateViewWindow2 hook ----

typedef HRESULT (*__cdecl FileCabinet_CreateViewWindow2_t)(
    IShellBrowser*, void*, IShellView*, IShellView*, void*, HWND*);
FileCabinet_CreateViewWindow2_t FileCabinet_CreateViewWindow2Original;

HRESULT __cdecl FileCabinet_CreateViewWindow2Hook(
    IShellBrowser* pBrowser, void* var1, IShellView* psv1,
    IShellView* psv2, void* var2, HWND* hWnd) {
    HRESULT hRes = FileCabinet_CreateViewWindow2Original(pBrowser, var1, psv1, psv2, var2, hWnd);
    if (!g_initialized || FAILED(hRes) || !hWnd || !*hWnd) return hRes;

    HWND shellTab = GetParent(*hWnd);
    if (shellTab && IsWindow(shellTab)) {
        if (g_pendingNavPath[0] && !g_pendingNavBrowser) {
            g_pendingNavBrowser.copy_from(pBrowser);
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
        GetClassName(hWnd, className, 256);
        if (wcscmp(className, L"SHELLDLL_DefView") == 0) {
            HWND lv = FindWindowEx(hWnd, NULL, L"SysListView32", NULL);
            HWND dui = FindWindowEx(hWnd, NULL, L"DirectUIHWND", NULL);
            if (lv) {
                if (WindhawkUtils::SetWindowSubclassFromAnyThread(lv, SysListViewSubclass, 0)) {
                    Wh_Log(L"SysListView32 Subclassed %p", lv);
                    std::lock_guard<std::mutex> slk(g_subclassMutex);
                    g_subclassed.push_back({ lv, true });
                }
            } else if (dui) {
                if (WindhawkUtils::SetWindowSubclassFromAnyThread(hWnd, DUISubclass, 0)) {
                    Wh_Log(L"DirectUIHWND Subclassed %p", hWnd);
                    std::lock_guard<std::mutex> slk(g_subclassMutex);
                    g_subclassed.push_back({ hWnd, false });
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
        GetClassName(hWnd, className, 256);
        if (wcscmp(className, L"CabinetWClass") == 0) {
            HWND shellTab = FindWindowEx(hWnd, NULL, L"ShellTabWindowClass", NULL);
            if (shellTab != NULL)
                EnumChildWindows(shellTab, InitEnumChildWindowsProc, (LPARAM)shellTab);
        }
    }
    return TRUE;
}

// ---- Windhawk lifecycle ----

BOOL Wh_ModInit() {
    Wh_Log(L"Click on Empty Explorer Init");

    g_msgDoAction = RegisterWindowMessage(L"ClickOnEmptyExplorer_DoAction");
    LoadSettings();

    HMODULE hExplorerFrame = LoadLibraryExW(L"explorerframe.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!hExplorerFrame) {
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
    if (!WindhawkUtils::HookSymbols(hExplorerFrame, explorerframe_dll_hooks, ARRAYSIZE(explorerframe_dll_hooks))) {
        Wh_Log(L"Failed to hook ExplorerFrame.dll");
        return FALSE;
    }

    WindhawkUtils::SetFunctionHook((void*)CreateWindowExW, (void*)CreateWindowExW_hook, (void**)&CreateWindowExW_original);

    InterlockedExchange(&g_initialized, 1);
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
    InterlockedExchange(&g_initialized, 0);

    g_pendingNavHwnd = NULL;
    g_pendingNavBrowser = nullptr;
    g_pendingNavPath[0] = L'\0';

    // Collect subclassed HWNDs under lock, kill timers + remove subclasses
    // outside the lock. Timers use nullptr callback now (WM_TIMER handled in
    // subclass proc), so KillTimer here is clean — no mod-image callback fire.
    std::vector<SubclassEntry> toRemove;
    {
        std::lock_guard<std::mutex> lk(g_subclassMutex);
        std::swap(toRemove, g_subclassed);
    }
    for (auto& e : toRemove) {
        if (e.hWnd && IsWindow(e.hWnd)) {
            KillTimer(e.hWnd, 0x4D43);
            KillTimer(e.hWnd, 0x4D44);
            KillTimer(e.hWnd, 0x4D45);
            WindhawkUtils::RemoveWindowSubclassFromAnyThread(
                e.hWnd, e.isListView ? SysListViewSubclass : DUISubclass);
        }
    }
}
