// ==WindhawkMod==
// @id              always-on-top-universal
// @name            Always on Top (Universal)
// @description     Adds a customizable "Always on Top" option to window menus, plus a force-topmost process list.
// @version         6.3
// @author          You
// @github          https://github.com/glooobus1
// @include         *
// @exclude         windhawk.exe;svchost.exe;conhost.exe;git.exe
// @compilerOptions -luser32 -lpsapi
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
`explorer.exe;cmd.exe`

Note: for `explorer.exe` only the taskbar / desktop windows (Shell_TrayWnd, Progman, WorkerW)
are forced on top, not regular folder windows.
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
- forceTopmostList: "explorer.exe;cmd.exe"
  $name: Force topmost processes
  $description: Semicolon-separated process names (e.g. explorer.exe;cmd.exe). Their windows are always kept on top.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <psapi.h>
#include <string>
#include <vector>
#include <algorithm>

#define IDM_TOPMOST 0x8001

struct {
    PCWSTR enabledPrefix;
    PCWSTR menuTemplate;
    PCWSTR baseText;
    PCWSTR forceTopmostList;
} settings;

std::vector<std::wstring> g_forceTopmost;

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

static void ParseForceList(PCWSTR list) {
    g_forceTopmost.clear();
    if (!list) return;
    std::wstring str = list;
    size_t pos = 0;
    while (pos <= str.size()) {
        size_t next = str.find(L';', pos);
        std::wstring token = (next == std::wstring::npos)
                                 ? str.substr(pos)
                                 : str.substr(pos, next - pos);
        token = Trim(token);
        if (!token.empty()) {
            g_forceTopmost.push_back(ToLower(token));
        }
        if (next == std::wstring::npos) break;
        pos = next + 1;
    }
}

static std::wstring GetProcessName(HWND hWnd) {
    DWORD pid = 0;
    GetWindowThreadProcessId(hWnd, &pid);
    if (!pid) return L"";

    HANDLE hProc = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!hProc) return L"";

    wchar_t path[MAX_PATH] = {};
    DWORD size = MAX_PATH;
    std::wstring result;
    if (QueryFullProcessImageNameW(hProc, 0, path, &size)) {
        std::wstring full = path;
        size_t slash = full.find_last_of(L"\\/");
        result = (slash == std::wstring::npos) ? full : full.substr(slash + 1);
    }
    CloseHandle(hProc);
    return ToLower(result);
}

// Для explorer.exe форсим только панель задач / рабочий стол,
// чтобы не поднимать все окна папок.
static bool IsShellSurfaceWindow(HWND hWnd) {
    wchar_t cls[64] = {};
    if (!GetClassNameW(hWnd, cls, 64)) return false;
    std::wstring c = cls;
    return c == L"Shell_TrayWnd" ||
           c == L"Shell_SecondaryTrayWnd" ||
           c == L"Progman" ||
           c == L"WorkerW";
}

static bool IsForceTopmost(HWND hWnd) {
    if (g_forceTopmost.empty()) return false;

    std::wstring name = GetProcessName(hWnd);
    if (name.empty()) return false;

    for (const auto& p : g_forceTopmost) {
        if (name == p) {
            // Особая логика для explorer.exe: только shell-поверхности.
            if (name == L"explorer.exe") {
                return IsShellSurfaceWindow(hWnd);
            }
            return true;
        }
    }
    return false;
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
    std::wstring tpl = settings.menuTemplate ? settings.menuTemplate : L"%enabled%On Top";
    std::wstring prefix = (isTopmost && settings.enabledPrefix) ? settings.enabledPrefix : L"";
    std::wstring base = settings.baseText ? settings.baseText : L"On Top";

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
    DrawMenuBar(hWnd);
}

// Гарантирует, что окна из force-списка остаются поверх.
void EnforceForceTopmost(HWND hWnd) {
    if (!hWnd || !IsWindow(hWnd)) return;
    if (!IsForceTopmost(hWnd)) return;

    LONG_PTR exStyle = GetWindowLongPtrW(hWnd, GWL_EXSTYLE);
    if (!(exStyle & WS_EX_TOPMOST)) {
        ApplyTopmost(hWnd, true);
    }
}

void LoadSettings() {
    settings.enabledPrefix = Wh_GetStringSetting(L"enabledPrefix");
    settings.menuTemplate  = Wh_GetStringSetting(L"menuTemplate");
    settings.baseText      = Wh_GetStringSetting(L"baseText");
    settings.forceTopmostList = Wh_GetStringSetting(L"forceTopmostList");
    ParseForceList(settings.forceTopmostList);
}

void FreeSettings() {
    Wh_FreeStringSetting(settings.enabledPrefix);
    Wh_FreeStringSetting(settings.menuTemplate);
    Wh_FreeStringSetting(settings.baseText);
    Wh_FreeStringSetting(settings.forceTopmostList);
}

void CleanUpMenu(HMENU hMenu) {
    int count = GetMenuItemCount(hMenu);
    for (int i = count - 1; i >= 0; i--) {
        UINT id = GetMenuItemID(hMenu, i);
        if (id == IDM_TOPMOST) {
            DeleteMenu(hMenu, i, MF_BYPOSITION);
        }
        else if (id == 0 && (GetMenuState(hMenu, i, MF_BYPOSITION) & MF_SEPARATOR)) {
            DeleteMenu(hMenu, i, MF_BYPOSITION);
        }
    }
}

void ModifyMenu(HWND hWnd, HMENU hMenu) {
    CleanUpMenu(hMenu);

    LONG_PTR exStyle = GetWindowLongPtrW(hWnd, GWL_EXSTYLE);
    bool isTopmost = (exStyle & WS_EX_TOPMOST) != 0;

    std::wstring text = BuildMenuText(isTopmost);
    UINT flags = MF_STRING | (isTopmost ? MF_CHECKED : MF_UNCHECKED);

    AppendMenuW(hMenu, MF_SEPARATOR, 0, NULL);
    AppendMenuW(hMenu, flags, IDM_TOPMOST, text.c_str());
}

LRESULT WINAPI DefWindowProcW_Hook(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) {
    if (Msg == WM_INITMENUPOPUP) {
        HMENU hMenu = (HMENU)wParam;
        if (HIWORD(lParam) == 0) {
            HMENU hSysMenu = GetSystemMenu(hWnd, FALSE);
            if (hMenu == hSysMenu) {
                ModifyMenu(hWnd, hMenu);
            }
        }
    }

    if (Msg == WM_SYSCOMMAND) {
        if ((wParam & 0xFFF0) == IDM_TOPMOST) {
            ToggleTopmost(hWnd);
            return 0;
        }
    }

    // Не даём окну из force-списка потерять TOPMOST.
    if (Msg == WM_WINDOWPOSCHANGED || Msg == WM_ACTIVATE || Msg == WM_SHOWWINDOW) {
        EnforceForceTopmost(hWnd);
    }

    return DefWindowProcW_Original(hWnd, Msg, wParam, lParam);
}

BOOL WINAPI TrackPopupMenuEx_Hook(HMENU hMenu, UINT uFlags, int x, int y, HWND hWnd, LPTPMPARAMS lptpm) {
    bool isSystemMenu = false;

    if (hWnd && IsWindow(hWnd)) {
        if (GetMenuState(hMenu, SC_MINIMIZE, MF_BYCOMMAND) != -1 ||
            GetMenuState(hMenu, SC_MAXIMIZE, MF_BYCOMMAND) != -1) {
            isSystemMenu = true;
            ModifyMenu(hWnd, hMenu);
        }
    }

    if (isSystemMenu) {
        uFlags |= TPM_RETURNCMD;
    }

    BOOL result = TrackPopupMenuEx_Original(hMenu, uFlags, x, y, hWnd, lptpm);

    if (isSystemMenu) {
        if (result == IDM_TOPMOST) {
            ToggleTopmost(hWnd);
            return FALSE;
        }
        else if (result != 0) {
            PostMessageW(hWnd, WM_SYSCOMMAND, result, 0);
            return FALSE;
        }
        return FALSE;
    }

    return result;
}

BOOL Wh_ModInit() {
    Wh_Log(L"Init Always on Top Universal v6.3");
    LoadSettings();

    Wh_SetFunctionHook((void*)DefWindowProcW, (void*)DefWindowProcW_Hook, (void**)&DefWindowProcW_Original);
    Wh_SetFunctionHook((void*)TrackPopupMenuEx, (void*)TrackPopupMenuEx_Hook, (void**)&TrackPopupMenuEx_Original);

    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L"Uninit");
    FreeSettings();
}

void Wh_ModSettingsChanged() {
    Wh_Log(L"SettingsChanged");
    FreeSettings();
    LoadSettings();
}
