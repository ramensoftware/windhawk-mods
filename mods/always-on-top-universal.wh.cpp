// ==WindhawkMod==
// @id              always-on-top-universal
// @name            Always on Top (Universal)
// @description     Adds a customizable "Always on Top" option to window menus, plus a force-topmost process list.
// @version         1.0.0
// @author          gloobus1
// @github          https://github.com/glooobus1
// @include         *
// @exclude         windhawk.exe
// @exclude         svchost.exe
// @exclude         conhost.exe
// @exclude         git.exe
// @compilerOptions -luser32
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Always on Top (Universal)
Adds a toggle to the window title bar menu to keep the window on top.

## Template
The Menu template setting is used to build the menu item text.
- %enabled% is replaced with the Enabled prefix value **only when the window is currently topmost**. Otherwise it becomes an empty string.
- %text% is replaced with a static base text.

Examples:
- template "%enabled%On Top", prefix "✓ " -> "On Top" / "✓ On Top"
- template "%enabled%Always on top", prefix "[ON] " -> "Always on top" / "[ON] Always on top"

## Force topmost list
Processes listed in **Force topmost processes** (separated by `;`) will always have their
windows kept above all others, regardless of the menu toggle. Example:
`notepad.exe;calc.exe`
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- enabledPrefix: "✓ "
  $name: Enabled prefix
  $description: Text substituted for %enabled% when the window is topmost. Leave empty for no prefix.
- menuTemplate: "%enabled%On Top"
  $name: Menu template
  $description: Template for the menu item. Use %enabled% for the prefix and %text% for the base text.
- baseText: "On Top"
  $name: Base text
  $description: Text substituted for %text% in the template.
- forceTopmostList: ""
  $name: Force topmost processes
  $description: Semicolon-separated process names (e.g. notepad.exe;calc.exe). Their windows are always kept on top.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <string>
#include <algorithm>
#include <windhawk_utils.h>

// Window menu IDs must be below 0xF000 with the low 4 bits clear.
#define IDM_TOPMOST 0x8010

struct {
    PCWSTR enabledPrefix;
    PCWSTR menuTemplate;
    PCWSTR baseText;
    PCWSTR forceTopmostList;
} settings;

bool g_forceTopmostProcess = false;

using DefWindowProcW_t = decltype(&DefWindowProcW);
DefWindowProcW_t DefWindowProcW_Original;

using TrackPopupMenuEx_t = decltype(&TrackPopupMenuEx);
TrackPopupMenuEx_t TrackPopupMenuEx_Original;

static std::wstring ToLower(std::wstring s) {
    std::transform(s.begin(), s.end(), s.begin(), [](wchar_t c) {
        return (wchar_t)towlower(c);
    });
    return s;
}

static std::wstring Trim(const std::wstring& s) {
    size_t a = s.find_first_not_of(L" \t\r\n");
    if (a == std::wstring::npos) return L"";
    size_t b = s.find_last_not_of(L" \t\r\n");
    return s.substr(a, b - a + 1);
}

static std::wstring ReplaceAll(std::wstring str, const std::wstring& from, const std::wstring& to) {
    if (from.empty()) return str;
    size_t pos = 0;
    while ((pos = str.find(from, pos)) != std::wstring::npos) {
        str.replace(pos, from.length(), to);
        pos += to.length();
    }
    return str;
}

std::wstring BuildMenuText(bool isTopmost) {
    std::wstring tpl = (settings.menuTemplate && settings.menuTemplate[0]) ? settings.menuTemplate : L"%enabled%%text%";
    std::wstring prefix = (isTopmost && settings.enabledPrefix) ? settings.enabledPrefix : L"";
    std::wstring base = (settings.baseText && settings.baseText[0]) ? settings.baseText : L"On Top";

    std::wstring result = ReplaceAll(tpl, L"%enabled%", prefix);
    result = ReplaceAll(result, L"%text%", base);
    return result;
}

void ApplyTopmost(HWND hWnd, bool topmost) {
    HWND insertAfter = topmost ? HWND_TOPMOST : HWND_NOTOPMOST;
    SetWindowPos(hWnd, insertAfter, 0, 0, 0, 0,
                 SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
}

void ToggleTopmost(HWND hWnd) {
    LONG_PTR exStyle = GetWindowLongPtrW(hWnd, GWL_EXSTYLE);
    bool isTopmost = (exStyle & WS_EX_TOPMOST) != 0;
    ApplyTopmost(hWnd, !isTopmost);
}

// Returns true only if this call added the item.
bool AddMenuItem(HWND hWnd, HMENU hMenu) {
    if (GetMenuState(hMenu, IDM_TOPMOST, MF_BYCOMMAND) != (UINT)-1) {
        return false;  // already there: ours, or the app's own
    }

    LONG_PTR exStyle = GetWindowLongPtrW(hWnd, GWL_EXSTYLE);
    bool isTopmost = (exStyle & WS_EX_TOPMOST) != 0;

    std::wstring text = BuildMenuText(isTopmost);
    UINT flags = MF_STRING | (isTopmost ? MF_CHECKED : MF_UNCHECKED);

    AppendMenuW(hMenu, MF_SEPARATOR, 0, NULL);
    AppendMenuW(hMenu, flags, IDM_TOPMOST, text.c_str());
    return true;
}

void RemoveMenuItem(HMENU hMenu) {
    int count = GetMenuItemCount(hMenu);
    for (int i = count - 1; i >= 0; i--) {
        if (GetMenuItemID(hMenu, i) == IDM_TOPMOST) {
            DeleteMenu(hMenu, i, MF_BYPOSITION);
            // Remove the separator we added (it will be at position i-1)
            if (i > 0 && (GetMenuState(hMenu, i - 1, MF_BYPOSITION) & MF_SEPARATOR)) {
                DeleteMenu(hMenu, i - 1, MF_BYPOSITION);
            }
            break;
        }
    }
}

void LoadSettings() {
    settings.enabledPrefix = Wh_GetStringSetting(L"enabledPrefix");
    settings.menuTemplate  = Wh_GetStringSetting(L"menuTemplate");
    settings.baseText      = Wh_GetStringSetting(L"baseText");
    settings.forceTopmostList = Wh_GetStringSetting(L"forceTopmostList");

    wchar_t path[MAX_PATH] = {};
    std::wstring currentProcessName;
    if (GetModuleFileNameW(nullptr, path, MAX_PATH)) {
        std::wstring full = path;
        size_t slash = full.find_last_of(L"\\/");
        currentProcessName = (slash == std::wstring::npos) ? full : full.substr(slash + 1);
        currentProcessName = ToLower(currentProcessName);
    }

    g_forceTopmostProcess = false;
    if (settings.forceTopmostList && currentProcessName.length() > 0) {
        std::wstring str = settings.forceTopmostList;
        size_t pos = 0;
        while (pos <= str.size()) {
            size_t next = str.find(L';', pos);
            std::wstring token = (next == std::wstring::npos) ? str.substr(pos) : str.substr(pos, next - pos);
            token = ToLower(Trim(token));
            if (!token.empty() && token == currentProcessName) {
                g_forceTopmostProcess = true;
                break;
            }
            if (next == std::wstring::npos) break;
            pos = next + 1;
        }
    }
}

void FreeSettings() {
    Wh_FreeStringSetting(settings.enabledPrefix);
    Wh_FreeStringSetting(settings.menuTemplate);
    Wh_FreeStringSetting(settings.baseText);
    Wh_FreeStringSetting(settings.forceTopmostList);
}

void EnforceForceTopmost(HWND hWnd) {
    if (!hWnd || !IsWindow(hWnd)) return;
    if (GetWindowLongPtrW(hWnd, GWL_STYLE) & WS_CHILD) return;

    LONG_PTR exStyle = GetWindowLongPtrW(hWnd, GWL_EXSTYLE);
    if (!(exStyle & WS_EX_TOPMOST)) {
        ApplyTopmost(hWnd, true);
    }
}

LRESULT WINAPI DefWindowProcW_Hook(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) {
    if (Msg == WM_INITMENUPOPUP && HIWORD(lParam) == MF_SYSMENU) {
        AddMenuItem(hWnd, (HMENU)wParam);
    } 
    else if (Msg == WM_UNINITMENUPOPUP && HIWORD(lParam) == MF_SYSMENU) {
        RemoveMenuItem((HMENU)wParam);
    }
    else if (Msg == WM_SYSCOMMAND) {
        if ((wParam & 0xFFF0) == IDM_TOPMOST) {
            ToggleTopmost(hWnd);
            return 0;
        }
    }

    if (g_forceTopmostProcess) {
        static thread_local bool inSetWindowPos = false;
        if (!inSetWindowPos && (Msg == WM_WINDOWPOSCHANGED || Msg == WM_ACTIVATE || Msg == WM_SHOWWINDOW)) {
            inSetWindowPos = true;
            EnforceForceTopmost(hWnd);
            inSetWindowPos = false;
        }
    }

    return DefWindowProcW_Original(hWnd, Msg, wParam, lParam);
}

bool IsWindowMenu(HMENU hMenu) {
    return GetMenuState(hMenu, SC_MINIMIZE, MF_BYCOMMAND) != -1 ||
           GetMenuState(hMenu, SC_MAXIMIZE, MF_BYCOMMAND) != -1;
}

BOOL WINAPI TrackPopupMenuEx_Hook(HMENU hMenu, UINT uFlags, int x, int y, HWND hWnd, LPTPMPARAMS lptpm) {
    bool added = false;
    
    if ((uFlags & TPM_RETURNCMD) && hWnd && IsWindow(hWnd) && IsWindowMenu(hMenu)) {
        added = AddMenuItem(hWnd, hMenu);
    }

    BOOL result = TrackPopupMenuEx_Original(hMenu, uFlags, x, y, hWnd, lptpm);

    if (added) {
        RemoveMenuItem(hMenu);
        if (result == IDM_TOPMOST) {
            ToggleTopmost(hWnd);
            return 0;
        }
    }

    return result;
}

BOOL Wh_ModInit() {
    Wh_Log(L"Init Always on Top Universal v%s", WH_MOD_VERSION);
    LoadSettings();

    WindhawkUtils::SetFunctionHook(DefWindowProcW, DefWindowProcW_Hook, &DefWindowProcW_Original);
    WindhawkUtils::SetFunctionHook(TrackPopupMenuEx, TrackPopupMenuEx_Hook, &TrackPopupMenuEx_Original);

    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L"Uninit");
    FreeSettings();
}

BOOL Wh_ModSettingsChanged(BOOL* bReload) {
    *bReload = TRUE;
    return TRUE;
}
