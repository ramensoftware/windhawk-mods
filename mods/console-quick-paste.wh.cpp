// ==WindhawkMod==
// @id              console-quick-paste
// @name            Console Quick Paste
// @description     Injects a "Quick Commands" submenu into the right-click context menu of Windows PowerShell and Command Prompt.
// @version         1.0.0
// @author          Armaninyow
// @github          https://github.com/armaninyow
// @include         WindowsTerminal.exe
// @include         conhost.exe
// @include         OpenConsole.exe
// @compilerOptions -luser32 -lcomctl32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Console Quick Paste

![Console Quick Paste](https://raw.githubusercontent.com/armaninyow/Remove-Context-Menu-Items/refs/heads/main/CQP.png)

Brings a "Quick Commands" submenu to Windows Terminal, giving you one-click access to your most-used PowerShell and Command Prompt snippets. To open it, right-click the empty space on the tab bar.

## Features

- `Quick Commands Submenu`: Always pinned to the top of the context menu, separated from the built-in items by a divider.
- `Configurable Command List`: Define as many commands as you like. Each entry has:
  - `Label`: The display name shown in the submenu. Falls back to the command itself when left blank.
  - `Command`: The string that will be pasted into the console.
- `Auto-Enter`: When enabled, an `Enter` key is sent immediately after pasting so the command executes automatically.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- autoEnter: false
  $name: Auto-Enter
  $description: >-
    Automatically press Enter after pasting, executing the command immediately. Disabled by default.

- commands:
  - - label: "Sample PowerShell Command (Opens Edge InPrivate)"
      $name: Label (optional)
    - command: "Start-Process \"msedge.exe\" \"-inprivate https://youtu.be/dQw4w9WgXcQ\""
      $name: Command
  $name: Quick Commands
  $description: >-
    Commands shown in the Quick Commands submenu.

      Label: Optional display name shown in the menu. Leave blank to use the text value.

      Command: The string pasted into the console input when the item is clicked.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <string>
#include <vector>

// ─────────────────────────────────────────────────────────────────────────────
// Configuration
// ─────────────────────────────────────────────────────────────────────────────

struct CommandEntry {
    std::wstring text;   // actual string to paste
    std::wstring label;  // display name (may be empty → use text)
};

struct Settings {
    bool autoEnter = false;
    std::vector<CommandEntry> commands;
};

static Settings g_settings;

// ─────────────────────────────────────────────────────────────────────────────
// Menu ID helpers
// System menu predefined IDs are >= 0xF000, so we use IDs below that.
// We pick a block well below 0xF000 and well above 0 to avoid clashing with
// any standard SC_* values.
// ─────────────────────────────────────────────────────────────────────────────

static constexpr UINT QUICK_CMD_ID_BASE = 0x7000; // 28672
static constexpr UINT QUICK_CMD_ID_MAX  = 0x7FFF; // up to 4096 items

// Track the last system menu we injected into so we can recognise it in the
// TrackPopupMenu hook and avoid injecting twice.
static HMENU g_lastInjectedMenu = nullptr;

// ─────────────────────────────────────────────────────────────────────────────
// SendInput helpers
// ─────────────────────────────────────────────────────────────────────────────

static void SendUnicodeString(const std::wstring& text) {
    if (text.empty()) return;

    std::vector<INPUT> inputs;
    inputs.reserve(text.size() * 2);

    for (wchar_t ch : text) {
        INPUT in{};
        in.type           = INPUT_KEYBOARD;
        in.ki.wVk         = 0;
        in.ki.wScan       = ch;
        in.ki.dwFlags     = KEYEVENTF_UNICODE;
        inputs.push_back(in);

        in.ki.dwFlags = KEYEVENTF_UNICODE | KEYEVENTF_KEYUP;
        inputs.push_back(in);
    }

    SendInput(static_cast<UINT>(inputs.size()), inputs.data(), sizeof(INPUT));
}

static void SendEnterKey() {
    INPUT inputs[2]{};

    inputs[0].type       = INPUT_KEYBOARD;
    inputs[0].ki.wVk     = VK_RETURN;
    inputs[0].ki.dwFlags = 0;

    inputs[1].type       = INPUT_KEYBOARD;
    inputs[1].ki.wVk     = VK_RETURN;
    inputs[1].ki.dwFlags = KEYEVENTF_KEYUP;

    SendInput(2, inputs, sizeof(INPUT));
}

static void PasteCommand(const std::wstring& text) {
    // Small delay so the context menu can fully dismiss before we type.
    Sleep(50);
    SendUnicodeString(text);
    if (g_settings.autoEnter) {
        Sleep(20);
        SendEnterKey();
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// Build the Quick Commands submenu from current settings
// ─────────────────────────────────────────────────────────────────────────────

static HMENU BuildSubMenu() {
    HMENU hSub = CreatePopupMenu();
    if (!hSub) return nullptr;

    if (g_settings.commands.empty()) {
        // Show a greyed placeholder so the submenu isn't empty (empty submenus
        // can cause rendering glitches on some Windows versions).
        AppendMenuW(hSub, MF_STRING | MF_GRAYED, 0, L"(no commands configured)");
        return hSub;
    }

    UINT id = QUICK_CMD_ID_BASE;
    for (const auto& cmd : g_settings.commands) {
        const std::wstring& label = cmd.label.empty() ? cmd.text : cmd.label;
        AppendMenuW(hSub, MF_STRING, id, label.c_str());
        ++id;
        if (id > QUICK_CMD_ID_MAX) break; // guard against overflow
    }
    return hSub;
}

// ─────────────────────────────────────────────────────────────────────────────
// Prepend Quick Commands + separator to an existing popup menu
// ─────────────────────────────────────────────────────────────────────────────

static void PrependQuickCommandsMenu(HMENU hMenu) {
    HMENU hSub = BuildSubMenu();
    if (!hSub) return;

    // Insert separator at position 1 (after the submenu we're about to add).
    InsertMenuW(hMenu, 0, MF_BYPOSITION | MF_SEPARATOR, 0, nullptr);
    // Insert the "Quick Commands ►" submenu at position 0 (very top).
    InsertMenuW(hMenu, 0, MF_BYPOSITION | MF_POPUP | MF_STRING,
                reinterpret_cast<UINT_PTR>(hSub), L"Quick Commands");
}

// ─────────────────────────────────────────────────────────────────────────────
// Hooks for GetSystemMenu, TrackPopupMenu, and TrackPopupMenuEx
//
// The console tab right-click menu on Windows 11 is the Win32 system (title-bar)
// menu, not a shell popup.  The flow is:
//   1. conhost/OpenConsole calls GetSystemMenu(hwnd, FALSE) to get the menu handle.
//   2. It then calls TrackPopupMenu(Ex) to display it.
//   3. The selected SC_* command is posted as WM_SYSCOMMAND.
//
// We hook GetSystemMenu to inject our submenu the moment the menu handle is
// fetched, and hook TrackPopupMenu/Ex to intercept any selection that belongs
// to us before it reaches DefWindowProc.
// ─────────────────────────────────────────────────────────────────────────────

using GetSystemMenu_t = HMENU(WINAPI*)(HWND, BOOL);
using TrackPopupMenu_t = BOOL(WINAPI*)(HMENU, UINT, int, int, int, HWND, const RECT*);
using TrackPopupMenuEx_t = BOOL(WINAPI*)(HMENU, UINT, int, int, HWND, LPTPMPARAMS);

static GetSystemMenu_t    Original_GetSystemMenu    = nullptr;
static TrackPopupMenu_t   Original_TrackPopupMenu   = nullptr;
static TrackPopupMenuEx_t Original_TrackPopupMenuEx = nullptr;

// Returns true and populates `out` when `id` belongs to our submenu.
static bool ResolveSelection(UINT id, std::wstring& out) {
    if (id < QUICK_CMD_ID_BASE) return false;
    size_t idx = id - QUICK_CMD_ID_BASE;
    if (idx >= g_settings.commands.size()) return false;
    out = g_settings.commands[idx].text;
    return true;
}

static HMENU WINAPI Hook_GetSystemMenu(HWND hWnd, BOOL bRevert) {
    HMENU hMenu = Original_GetSystemMenu(hWnd, bRevert);

    if (hMenu && !bRevert && hMenu != g_lastInjectedMenu) {
        // Strip any previous injection (handles menu reuse across calls)
        // by removing items in our ID range from position 0 and 1.
        // Simpler: just check if item 0 is already ours.
        MENUITEMINFOW mii{};
        mii.cbSize = sizeof(mii);
        mii.fMask  = MIIM_ID;
        if (GetMenuItemInfoW(hMenu, 0, TRUE, &mii)) {
            if (mii.wID >= QUICK_CMD_ID_BASE && mii.wID <= QUICK_CMD_ID_MAX) {
                // Already injected — skip.
                return hMenu;
            }
        }
        PrependQuickCommandsMenu(hMenu);
        g_lastInjectedMenu = hMenu;
        Wh_Log(L"Quick Commands injected into system menu %p for HWND %p", hMenu, hWnd);
    }
    return hMenu;
}

static BOOL WINAPI Hook_TrackPopupMenu(
    HMENU hMenu, UINT uFlags, int x, int y,
    int nReserved, HWND hWnd, const RECT* prcRect)
{
    UINT flags = uFlags | TPM_RETURNCMD;
    BOOL result = Original_TrackPopupMenu(hMenu, flags, x, y, nReserved, hWnd, prcRect);

    std::wstring cmdText;
    if (result && ResolveSelection(static_cast<UINT>(result), cmdText)) {
        PasteCommand(cmdText);
        return (uFlags & TPM_RETURNCMD) ? 0 : FALSE;
    }

    // Re-dispatch non-quick-command selections as WM_SYSCOMMAND so the
    // window still responds to Minimize / Maximize / Close etc.
    if (result && hWnd && !(uFlags & TPM_RETURNCMD)) {
        PostMessageW(hWnd, WM_SYSCOMMAND, result & 0xFFF0, 0);
        return TRUE;
    }

    return (uFlags & TPM_RETURNCMD) ? result : (result ? TRUE : FALSE);
}

static BOOL WINAPI Hook_TrackPopupMenuEx(
    HMENU hMenu, UINT uFlags, int x, int y,
    HWND hWnd, LPTPMPARAMS lptpm)
{
    UINT flags = uFlags | TPM_RETURNCMD;
    BOOL result = Original_TrackPopupMenuEx(hMenu, flags, x, y, hWnd, lptpm);

    std::wstring cmdText;
    if (result && ResolveSelection(static_cast<UINT>(result), cmdText)) {
        PasteCommand(cmdText);
        return (uFlags & TPM_RETURNCMD) ? 0 : FALSE;
    }

    if (result && hWnd && !(uFlags & TPM_RETURNCMD)) {
        PostMessageW(hWnd, WM_SYSCOMMAND, result & 0xFFF0, 0);
        return TRUE;
    }

    return (uFlags & TPM_RETURNCMD) ? result : (result ? TRUE : FALSE);
}

// ─────────────────────────────────────────────────────────────────────────────
// Settings loading
// ─────────────────────────────────────────────────────────────────────────────

static void LoadSettings() {
    Settings s;
    s.autoEnter = Wh_GetIntSetting(L"autoEnter") != 0;

    // commands is a nested array: commands[N].label and commands[N].text
    for (int i = 0; ; ++i) {
        PCWSTR text = Wh_GetStringSetting(L"commands[%d].command", i);
        if (!text || text[0] == L'\0') {
            Wh_FreeStringSetting(text);
            break;
        }

        CommandEntry entry;
        entry.text = text;
        Wh_FreeStringSetting(text);

        PCWSTR label = Wh_GetStringSetting(L"commands[%d].label", i);
        if (label) {
            entry.label = label;
            Wh_FreeStringSetting(label);
        }

        s.commands.push_back(std::move(entry));
    }

    g_settings = std::move(s);
}

// ─────────────────────────────────────────────────────────────────────────────
// Windhawk entry points
// ─────────────────────────────────────────────────────────────────────────────

BOOL Wh_ModInit() {
    Wh_Log(L"Console Quick Paste: initialising");
    LoadSettings();

    // Hook GetSystemMenu — inject our submenu when the system menu is fetched
    if (!Wh_SetFunctionHook(
            reinterpret_cast<void*>(GetSystemMenu),
            reinterpret_cast<void*>(Hook_GetSystemMenu),
            reinterpret_cast<void**>(&Original_GetSystemMenu)))
    {
        Wh_Log(L"Failed to hook GetSystemMenu");
        return FALSE;
    }

    // Hook TrackPopupMenu — intercept our item selections
    if (!Wh_SetFunctionHook(
            reinterpret_cast<void*>(TrackPopupMenu),
            reinterpret_cast<void*>(Hook_TrackPopupMenu),
            reinterpret_cast<void**>(&Original_TrackPopupMenu)))
    {
        Wh_Log(L"Failed to hook TrackPopupMenu");
        return FALSE;
    }

    // Hook TrackPopupMenuEx — same, for the Ex variant
    if (!Wh_SetFunctionHook(
            reinterpret_cast<void*>(TrackPopupMenuEx),
            reinterpret_cast<void*>(Hook_TrackPopupMenuEx),
            reinterpret_cast<void**>(&Original_TrackPopupMenuEx)))
    {
        Wh_Log(L"Failed to hook TrackPopupMenuEx");
        return FALSE;
    }

    Wh_Log(L"Console Quick Paste: ready (%zu command(s) loaded, auto-enter=%s)",
           g_settings.commands.size(),
           g_settings.autoEnter ? L"on" : L"off");
    return TRUE;
}

void Wh_ModSettingsChanged() {
    Wh_Log(L"Console Quick Paste: settings changed, reloading");
    g_lastInjectedMenu = nullptr; // force re-injection on next GetSystemMenu call
    LoadSettings();
    Wh_Log(L"Console Quick Paste: %zu command(s), auto-enter=%s",
           g_settings.commands.size(),
           g_settings.autoEnter ? L"on" : L"off");
}

void Wh_ModUninit() {
    Wh_Log(L"Console Quick Paste: unloaded");
}