// ==WindhawkMod==
// @id              console-quick-paste
// @name            Console Quick Paste
// @description     Injects a "Quick Commands" submenu into the right-click context menu of Windows PowerShell and Command Prompt.
// @version         1.1.0
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

![Console Quick Paste](https://raw.githubusercontent.com/armaninyow/Remove-Context-Menu-Items/refs/heads/main/Don't%20Touch/CQP.png)

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
#include <mutex>
#include <string>
#include <vector>

struct CommandEntry {
    std::wstring text;   // actual string to paste
    std::wstring label;  // display name (may be empty → use text)
};

struct Settings {
    bool autoEnter = false;
    std::vector<CommandEntry> commands;
};

static Settings g_settings;

static constexpr UINT QUICK_CMD_SUBMENU_ID = 0x6FFF; // ID of the "Quick Commands" submenu item itself
static constexpr UINT QUICK_CMD_ID_BASE = 0x7000; // 28672
static constexpr UINT QUICK_CMD_ID_MAX  = 0x7FFF; // up to 4096 items

static std::mutex g_injectedMutex;
static std::vector<HMENU> g_injectedMenus;

static void AddKey(std::vector<INPUT>& inputs, WORD vk, WORD scan, DWORD flags) {
    INPUT in{};
    in.type       = INPUT_KEYBOARD;
    in.ki.wVk     = vk;
    in.ki.wScan   = scan;
    in.ki.dwFlags = flags;
    inputs.push_back(in);
}

static void AddUnicodeChar(std::vector<INPUT>& inputs, wchar_t ch) {
    AddKey(inputs, 0, ch, KEYEVENTF_UNICODE);
    AddKey(inputs, 0, ch, KEYEVENTF_UNICODE | KEYEVENTF_KEYUP);
}

static bool KeyTypesChar(UINT vk, UINT scan, bool shift, bool capsOn,
                         wchar_t ch, HKL hkl) {
    BYTE keyState[256]{};
    if (shift) keyState[VK_SHIFT] = 0x80;
    if (capsOn) keyState[VK_CAPITAL] = 0x01;
    WCHAR buf[4];
    int n = ToUnicodeEx(vk, scan, keyState, buf, ARRAYSIZE(buf), 0x4, hkl);
    return n == 1 && buf[0] == ch;
}

static void SendString(const std::wstring& text) {
    if (text.empty()) return;

    HWND fg = GetForegroundWindow();
    DWORD tid = fg ? GetWindowThreadProcessId(fg, nullptr) : 0;
    HKL hkl = GetKeyboardLayout(tid);
    bool capsOn = (GetKeyState(VK_CAPITAL) & 1) != 0;

    WORD lang = PRIMARYLANGID(LOWORD(reinterpret_cast<UINT_PTR>(hkl)));
    bool imeLayout = lang == LANG_JAPANESE || lang == LANG_CHINESE || lang == LANG_KOREAN;

    WORD shiftScan = static_cast<WORD>(MapVirtualKeyExW(VK_SHIFT, MAPVK_VK_TO_VSC, hkl));

    std::vector<INPUT> inputs;
    inputs.reserve(text.size() * 4);

    for (wchar_t ch : text) {
        if (ch == L'\r') continue; // treat CRLF as a single Enter

        if (ch == L'\n') {
            WORD scan = static_cast<WORD>(MapVirtualKeyExW(VK_RETURN, MAPVK_VK_TO_VSC, hkl));
            AddKey(inputs, VK_RETURN, scan, 0);
            AddKey(inputs, VK_RETURN, scan, KEYEVENTF_KEYUP);
            continue;
        }

        SHORT res = VkKeyScanExW(ch, hkl);
        WORD vk = LOBYTE(res);
        UINT scan = MapVirtualKeyExW(vk, MAPVK_VK_TO_VSC, hkl);
        bool shift = HIBYTE(static_cast<WORD>(res)) & 1;

        bool useKey = !imeLayout && res != -1 &&
                      (HIBYTE(static_cast<WORD>(res)) & ~1) == 0 && scan != 0;

        if (useKey && !KeyTypesChar(vk, scan, shift, capsOn, ch, hkl)) {
            shift = !shift; // Caps Lock may invert Shift for this key
            useKey = KeyTypesChar(vk, scan, shift, capsOn, ch, hkl);
        }

        if (!useKey) {
            AddUnicodeChar(inputs, ch);
            continue;
        }

        if (shift) AddKey(inputs, VK_SHIFT, shiftScan, 0);
        AddKey(inputs, vk, static_cast<WORD>(scan), 0);
        AddKey(inputs, vk, static_cast<WORD>(scan), KEYEVENTF_KEYUP);
        if (shift) AddKey(inputs, VK_SHIFT, shiftScan, KEYEVENTF_KEYUP);
    }

    if (!inputs.empty()) {
        SendInput(static_cast<UINT>(inputs.size()), inputs.data(), sizeof(INPUT));
    }
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
    Sleep(50);
    SendString(text);
    if (g_settings.autoEnter) {
        Sleep(20);
        SendEnterKey();
    }
}

static HMENU BuildSubMenu() {
    HMENU hSub = CreatePopupMenu();
    if (!hSub) return nullptr;

    if (g_settings.commands.empty()) {
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

static int FindQuickCommandsItem(HMENU hMenu) {
    int count = GetMenuItemCount(hMenu);
    for (int i = 0; i < count; i++) {
        MENUITEMINFOW mii{};
        mii.cbSize = sizeof(mii);
        mii.fMask  = MIIM_ID;
        if (GetMenuItemInfoW(hMenu, i, TRUE, &mii) && mii.wID == QUICK_CMD_SUBMENU_ID) {
            return i;
        }
    }
    return -1;
}

static void RemoveQuickCommandsMenu(HMENU hMenu) {
    int idx;
    while ((idx = FindQuickCommandsItem(hMenu)) != -1) {
        DeleteMenu(hMenu, idx, MF_BYPOSITION);

        if (idx < GetMenuItemCount(hMenu)) {
            MENUITEMINFOW mii{};
            mii.cbSize = sizeof(mii);
            mii.fMask  = MIIM_FTYPE;
            if (GetMenuItemInfoW(hMenu, idx, TRUE, &mii) && (mii.fType & MFT_SEPARATOR)) {
                DeleteMenu(hMenu, idx, MF_BYPOSITION);
            }
        }
    }
}

static bool PrependQuickCommandsMenu(HMENU hMenu) {
    HMENU hSub = BuildSubMenu();
    if (!hSub) return false;

    MENUITEMINFOW mii{};
    mii.cbSize     = sizeof(mii);
    mii.fMask      = MIIM_ID | MIIM_SUBMENU | MIIM_STRING;
    mii.wID        = QUICK_CMD_SUBMENU_ID;
    mii.hSubMenu   = hSub;
    mii.dwTypeData = const_cast<LPWSTR>(L"Quick Commands");

    if (!InsertMenuItemW(hMenu, 0, TRUE, &mii)) {
        DestroyMenu(hSub);
        return false;
    }

    InsertMenuW(hMenu, 1, MF_BYPOSITION | MF_SEPARATOR, 0, nullptr);
    return true;
}

static void TrackInjectedMenu(HMENU hMenu) {
    std::lock_guard<std::mutex> lock(g_injectedMutex);
    for (HMENU h : g_injectedMenus) {
        if (h == hMenu) return;
    }
    g_injectedMenus.push_back(hMenu);
}

using GetSystemMenu_t = HMENU(WINAPI*)(HWND, BOOL);
using TrackPopupMenu_t = BOOL(WINAPI*)(HMENU, UINT, int, int, int, HWND, const RECT*);
using TrackPopupMenuEx_t = BOOL(WINAPI*)(HMENU, UINT, int, int, HWND, LPTPMPARAMS);

static GetSystemMenu_t    Original_GetSystemMenu    = nullptr;
static TrackPopupMenu_t   Original_TrackPopupMenu   = nullptr;
static TrackPopupMenuEx_t Original_TrackPopupMenuEx = nullptr;

static bool ResolveSelection(UINT id, std::wstring& out) {
    if (id < QUICK_CMD_ID_BASE) return false;
    size_t idx = id - QUICK_CMD_ID_BASE;
    if (idx >= g_settings.commands.size()) return false;
    out = g_settings.commands[idx].text;
    return true;
}

static HMENU WINAPI Hook_GetSystemMenu(HWND hWnd, BOOL bRevert) {
    HMENU hMenu = Original_GetSystemMenu(hWnd, bRevert);

    if (hMenu && !bRevert) {
        RemoveQuickCommandsMenu(hMenu);
        if (PrependQuickCommandsMenu(hMenu)) {
            TrackInjectedMenu(hMenu);
            Wh_Log(L"Quick Commands injected into system menu %p for HWND %p", hMenu, hWnd);
        }
    }
    return hMenu;
}

static BOOL WINAPI Hook_TrackPopupMenu(
    HMENU hMenu, UINT uFlags, int x, int y,
    int nReserved, HWND hWnd, const RECT* prcRect)
{
    if (FindQuickCommandsItem(hMenu) == -1) {
        return Original_TrackPopupMenu(hMenu, uFlags, x, y, nReserved, hWnd, prcRect);
    }

    UINT flags = uFlags | TPM_RETURNCMD;
    BOOL result = Original_TrackPopupMenu(hMenu, flags, x, y, nReserved, hWnd, prcRect);

    std::wstring cmdText;
    if (result && ResolveSelection(static_cast<UINT>(result), cmdText)) {
        PasteCommand(cmdText);
        return (uFlags & TPM_RETURNCMD) ? 0 : FALSE;
    }

    if (result && hWnd && !(uFlags & TPM_RETURNCMD)) {
        PostMessageW(hWnd, WM_SYSCOMMAND, result, 0);
        return TRUE;
    }

    return (uFlags & TPM_RETURNCMD) ? result : (result ? TRUE : FALSE);
}

static BOOL WINAPI Hook_TrackPopupMenuEx(
    HMENU hMenu, UINT uFlags, int x, int y,
    HWND hWnd, LPTPMPARAMS lptpm)
{
    if (FindQuickCommandsItem(hMenu) == -1) {
        return Original_TrackPopupMenuEx(hMenu, uFlags, x, y, hWnd, lptpm);
    }

    UINT flags = uFlags | TPM_RETURNCMD;
    BOOL result = Original_TrackPopupMenuEx(hMenu, flags, x, y, hWnd, lptpm);

    std::wstring cmdText;
    if (result && ResolveSelection(static_cast<UINT>(result), cmdText)) {
        PasteCommand(cmdText);
        return (uFlags & TPM_RETURNCMD) ? 0 : FALSE;
    }

    if (result && hWnd && !(uFlags & TPM_RETURNCMD)) {
        PostMessageW(hWnd, WM_SYSCOMMAND, result, 0);
        return TRUE;
    }

    return (uFlags & TPM_RETURNCMD) ? result : (result ? TRUE : FALSE);
}

static void LoadSettings() {
    Settings s;
    s.autoEnter = Wh_GetIntSetting(L"autoEnter") != 0;

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

BOOL Wh_ModInit() {
    Wh_Log(L"Console Quick Paste: initialising");
    LoadSettings();

    if (!Wh_SetFunctionHook(
            reinterpret_cast<void*>(GetSystemMenu),
            reinterpret_cast<void*>(Hook_GetSystemMenu),
            reinterpret_cast<void**>(&Original_GetSystemMenu)))
    {
        Wh_Log(L"Failed to hook GetSystemMenu");
        return FALSE;
    }

    if (!Wh_SetFunctionHook(
            reinterpret_cast<void*>(TrackPopupMenu),
            reinterpret_cast<void*>(Hook_TrackPopupMenu),
            reinterpret_cast<void**>(&Original_TrackPopupMenu)))
    {
        Wh_Log(L"Failed to hook TrackPopupMenu");
        return FALSE;
    }

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
    LoadSettings();
    Wh_Log(L"Console Quick Paste: %zu command(s), auto-enter=%s",
           g_settings.commands.size(),
           g_settings.autoEnter ? L"on" : L"off");
}

void Wh_ModUninit() {
    {
        std::lock_guard<std::mutex> lock(g_injectedMutex);
        for (HMENU hMenu : g_injectedMenus) {
            if (IsMenu(hMenu)) {
                RemoveQuickCommandsMenu(hMenu);
            }
        }
        g_injectedMenus.clear();
    }
    Wh_Log(L"Console Quick Paste: unloaded");
}
