// ==WindhawkMod==
// @id              window-centralizer
// @name            Window Centralizer
// @description     Previsibly centers new windows and dialogs to their parent or to the mouse cursor, with a configurable threshold and exclusion list.
// @version         1.0.0
// @author          3DDinosaur
// @github          https://github.com/3DDinosaur
// @include         *
// @compilerOptions -lcomctl32 -lgdi32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Window Centralizer

New dialogs and small windows open centered to the one they originated from, or the mouse cursor, instead of wherever the program decides.

The window is placed **before** it appears, so there is no jump, no flicker, and window opening animations play at the final position. Elevated programs are covered as well.

## Modes

- **Parent window**: centered on the window that owns the new one; otherwise on the active or topmost window of the same program (including another instance of it, as with File Explorer folder windows). Windows with none of these open centered on the primary monitor.
- **Mouse cursor**: centered on the mouse cursor.

In both modes the window is kept inside the monitor's work area.

## Which windows are moved

- Only windows with a title bar that are smaller than the **maximum window size** setting (a percentage of the monitor's work area). Large main windows keep their own position.
- Standard Windows dialogs (Save As, Run, properties pages…) are always centered, whatever their size.
- Maximized, minimized, tool, layered and untitled windows, splash screens, menus, tooltips and shell surfaces (taskbar, Start, search) are never moved.
- A window that is hidden and shown again (for example a Find dialog that is reopened) is centered again.

## Ignoring windows

Press **Win+Alt+C** while a window is active to stop centering it (press again to undo). The rule matches the program, the window class and the window title. A short notification confirms the change.

**Ignored windows** list accessible with **Win+Ctrl+Alt+C** shortcut, where entries can be removed. If its hotkey is empty or taken by another program, that window opens whenever the mod settings are saved.

Both hotkeys can be changed in the mod settings.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- mode: parent
  $name: Center on
  $options:
    - parent: Parent window
    - mouse: Mouse cursor
- maxAreaPercent: 10
  $name: Maximum window size (% of the monitor)
  $description: >-
    Windows larger than this share of the monitor's work area keep their own
    position. Standard Windows dialogs are centered regardless of size.
- ignoreHotkey: Win+Alt+C
  $name: Ignore hotkey
  $description: >-
    Add / remove the active window to the ignored list. Modifiers
    (Ctrl, Alt, Shift, Win) and one key, joined with +. Leave empty for no
    hotkey.
- listHotkey: Win+Ctrl+Alt+C
  $name: Ignored windows hotkey
  $description: >-
    Opens the list of ignored windows, same format as above. If empty, the
    list opens whenever these settings are saved.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <commctrl.h>
#include <stdio.h>
#include <wchar.h>
#include <wctype.h>

// ───────────── Settings ─────────────

enum Mode { MODE_PARENT, MODE_MOUSE };

struct Settings {
    Mode mode;
    int maxAreaPercent;
    wchar_t ignoreHotkey[64];
    wchar_t listHotkey[64];
};
Settings g_settings;

wchar_t g_processName[MAX_PATH];  // file name of the current process, for ignore rules

void CopySetting(const wchar_t* name, wchar_t* dst, size_t dstChars) {
    PCWSTR value = Wh_GetStringSetting(name);
    wcsncpy_s(dst, dstChars, value, _TRUNCATE);
    Wh_FreeStringSetting(value);
}

void LoadSettings() {
    PCWSTR mode = Wh_GetStringSetting(L"mode");
    g_settings.mode = wcscmp(mode, L"mouse") == 0 ? MODE_MOUSE : MODE_PARENT;
    Wh_FreeStringSetting(mode);
    int pct = Wh_GetIntSetting(L"maxAreaPercent");
    g_settings.maxAreaPercent = pct < 1 ? 1 : (pct > 100 ? 100 : pct);
    CopySetting(L"ignoreHotkey", g_settings.ignoreHotkey, ARRAYSIZE(g_settings.ignoreHotkey));
    CopySetting(L"listHotkey", g_settings.listHotkey, ARRAYSIZE(g_settings.listHotkey));
}

// ───────────── Ignore list ─────────────
//
// Stored in the mod's storage as one string, one rule per line: "process.exe|class|title".
// An empty title matches any title. "ignoredVersion" is bumped on every change so each
// process reloads its cached copy only when the list actually changed.

constexpr size_t kIgnoredChars = 32768;

struct IgnoreCache {
    SRWLOCK lock = SRWLOCK_INIT;
    int version = -1;
    wchar_t* text = nullptr;
};
IgnoreCache g_ignore;

bool ContainsNoCase(const wchar_t* haystack, const wchar_t* needle) {
    size_t n = wcslen(needle);
    if (!n)
        return true;
    for (; *haystack; haystack++)
        if (_wcsnicmp(haystack, needle, n) == 0)
            return true;
    return false;
}

/// True if the rule "process|class|title" matches the window. The title part is a case-insensitive substring.
bool RuleMatches(const wchar_t* rule, size_t ruleLen, const wchar_t* process, const wchar_t* cls, const wchar_t* title) {
    wchar_t buf[1024];
    if (ruleLen >= ARRAYSIZE(buf))
        return false;
    wmemcpy(buf, rule, ruleLen);
    buf[ruleLen] = L'\0';
    wchar_t* bar1 = wcschr(buf, L'|');
    if (!bar1)
        return false;
    wchar_t* bar2 = wcschr(bar1 + 1, L'|');
    if (!bar2)
        return false;
    *bar1 = *bar2 = L'\0';
    return _wcsicmp(buf, process) == 0 && _wcsicmp(bar1 + 1, cls) == 0 && ContainsNoCase(title, bar2 + 1);
}

bool IsIgnored(const wchar_t* cls, const wchar_t* title) {
    int version = Wh_GetIntValue(L"ignoredVersion", 0);
    AcquireSRWLockExclusive(&g_ignore.lock);
    if (version != g_ignore.version) {
        if (!g_ignore.text)
            g_ignore.text = (wchar_t*)HeapAlloc(GetProcessHeap(), 0, kIgnoredChars * sizeof(wchar_t));
        if (g_ignore.text) {
            g_ignore.text[0] = L'\0';
            Wh_GetStringValue(L"ignored", g_ignore.text, kIgnoredChars);
            g_ignore.version = version;
        }
    }
    bool ignored = false;
    for (const wchar_t* line = g_ignore.text; line && *line && !ignored;) {
        const wchar_t* end = wcschr(line, L'\n');
        size_t len = end ? (size_t)(end - line) : wcslen(line);
        ignored = len && RuleMatches(line, len, g_processName, cls, title);
        line = end ? end + 1 : nullptr;
    }
    ReleaseSRWLockExclusive(&g_ignore.lock);
    return ignored;
}

// ───────────── Which windows to center ─────────────

bool HasCaption(HWND hwnd) {
    return (GetWindowLongPtrW(hwnd, GWL_STYLE) & WS_CAPTION) == WS_CAPTION;
}

bool IsIgnoredClass(const wchar_t* cls) {
    static const wchar_t* const kParts[] = {
        L"Splash", L"Loading", L"MsoSplash", L"Ghost", L"NetUI", L"#32768", L"Menu", L"Shadow", L"Tooltip",
        L"NotifyIcon", L"Popup", L"Overlay", L"Dimmer", L"Shell_TrayWnd", L"TrayNotifyWnd", L"Taskbar",
        L"Progman", L"WorkerW", L"SearchHost", L"Windows.UI.Core.CoreWindow", L"WindowCentralizer_",
    };
    for (const wchar_t* part : kParts)
        if (ContainsNoCase(cls, part))
            return true;
    return false;
}

// InternalGetWindowText reads the cached title without sending WM_GETTEXT, which could deadlock
// inside a hook when the window belongs to another thread that is busy.
int TitleOf(HWND hwnd, wchar_t* buf, int chars) {
    return InternalGetWindowText(hwnd, buf, chars);
}

/// A window this mod may move: top level, title bar, not min/maximized, not tool/layered, titled, not ignored.
bool IsCandidate(HWND hwnd) {
    LONG_PTR style = GetWindowLongPtrW(hwnd, GWL_STYLE);
    if ((style & WS_CHILD) || (style & WS_CAPTION) != WS_CAPTION || (style & (WS_MINIMIZE | WS_MAXIMIZE)))
        return false;
    if (GetWindowLongPtrW(hwnd, GWL_EXSTYLE) & (WS_EX_LAYERED | WS_EX_TOOLWINDOW))
        return false;
    wchar_t cls[256], title[512];
    if (!GetClassNameW(hwnd, cls, ARRAYSIZE(cls)) || IsIgnoredClass(cls))
        return false;
    if (!TitleOf(hwnd, title, ARRAYSIZE(title)))
        return false;
    return !IsIgnored(cls, title);
}

// A valid parent has a title bar and a title. Proxy windows such as StubWindow32 have a
// title bar but no title, and must not be used as a reference.
bool IsValidParent(HWND parent, HWND child) {
    if (!parent || parent == child || !IsWindowVisible(parent) || IsIconic(parent) || !HasCaption(parent))
        return false;
    wchar_t title[8];
    return TitleOf(parent, title, ARRAYSIZE(title)) > 0;
}

POINT PrimaryCenter() {
    MONITORINFO mi = {};
    mi.cbSize = sizeof(mi);
    GetMonitorInfoW(MonitorFromPoint({0, 0}, MONITOR_DEFAULTTOPRIMARY), &mi);
    return {(mi.rcWork.left + mi.rcWork.right) / 2, (mi.rcWork.top + mi.rcWork.bottom) / 2};
}

struct TopWindowSearch {
    HWND exclude;
    HWND found;
};

BOOL CALLBACK FindTopOwnWindow(HWND hwnd, LPARAM param) {
    auto* search = (TopWindowSearch*)param;
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    if (pid != GetCurrentProcessId() || !IsValidParent(hwnd, search->exclude))
        return TRUE;
    wchar_t cls[256];
    if (GetClassNameW(hwnd, cls, ARRAYSIZE(cls)) && IsIgnoredClass(cls))
        return TRUE;
    search->found = hwnd;
    return FALSE;
}

bool SameExecutable(HWND hwnd) {
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    HANDLE proc = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!proc)
        return false;
    wchar_t path[MAX_PATH];
    DWORD size = MAX_PATH;
    bool same = false;
    if (QueryFullProcessImageNameW(proc, 0, path, &size)) {
        const wchar_t* slash = wcsrchr(path, L'\\');
        same = _wcsicmp(slash ? slash + 1 : path, g_processName) == 0;
    }
    CloseHandle(proc);
    return same;
}

/// The window to center on: the owner, else the program's active or topmost window, else the
/// foreground window of another instance of the same program (Explorer runs folder windows in
/// several processes). Null if none qualifies.
HWND FindParent(HWND hwnd) {
    HWND owner = GetWindow(hwnd, GW_OWNER);
    if (IsValidParent(owner, hwnd))
        return owner;

    HWND fg = GetForegroundWindow();
    DWORD fgPid = 0;
    GetWindowThreadProcessId(fg, &fgPid);
    if (fgPid == GetCurrentProcessId() && IsValidParent(fg, hwnd))
        return fg;

    // The shell opens some dialogs (folder Properties, for one) from a hidden StubWindow32 that is
    // also the foreground window at that moment; the program's topmost real window is the one the user clicked.
    TopWindowSearch search = {hwnd, nullptr};
    EnumWindows(FindTopOwnWindow, (LPARAM)&search);
    if (search.found)
        return search.found;

    if (fg && fgPid != GetCurrentProcessId() && IsValidParent(fg, hwnd) && SameExecutable(fg))
        return fg;
    return nullptr;
}

POINT ReferencePoint(HWND hwnd) {
    if (g_settings.mode == MODE_MOUSE) {
        POINT pt;
        GetCursorPos(&pt);
        return pt;
    }
    HWND parent = FindParent(hwnd);
    if (!parent)
        return PrimaryCenter();
    RECT rc;
    GetWindowRect(parent, &rc);
    return {(rc.left + rc.right) / 2, (rc.top + rc.bottom) / 2};
}

/// Computes the centered position for a window of size w×h. False if the window should keep its position.
bool ComputeTarget(HWND hwnd, int w, int h, int* x, int* y) {
    POINT c = ReferencePoint(hwnd);
    MONITORINFO mi = {};
    mi.cbSize = sizeof(mi);
    GetMonitorInfoW(MonitorFromPoint(c, MONITOR_DEFAULTTONEAREST), &mi);
    const RECT& wa = mi.rcWork;
    long long monitorArea = (long long)(wa.right - wa.left) * (wa.bottom - wa.top);
    wchar_t cls[16];
    bool isDialog = GetClassNameW(hwnd, cls, ARRAYSIZE(cls)) && wcscmp(cls, L"#32770") == 0;
    if (!isDialog && (long long)w * h * 100 > monitorArea * g_settings.maxAreaPercent)
        return false;
    int nx = c.x - w / 2, ny = c.y - h / 2;
    nx = nx + w > wa.right ? wa.right - w : nx;
    ny = ny + h > wa.bottom ? wa.bottom - h : ny;
    *x = nx < wa.left ? wa.left : nx;
    *y = ny < wa.top ? wa.top : ny;
    return true;
}

void CenterNow(HWND hwnd) {
    if (!IsCandidate(hwnd))
        return;
    RECT rc;
    GetWindowRect(hwnd, &rc);
    int x, y;
    if (!ComputeTarget(hwnd, rc.right - rc.left, rc.bottom - rc.top, &x, &y) || (x == rc.left && y == rc.top))
        return;
    SetWindowPos(hwnd, nullptr, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE);
}

bool IsShowCommand(int cmd) {
    return cmd == SW_SHOWNORMAL || cmd == SW_SHOWNOACTIVATE || cmd == SW_SHOW || cmd == SW_SHOWNA ||
           cmd == SW_SHOWDEFAULT;
}

// ───────────── Hooks ─────────────
//
// The win32u entry points are hooked rather than user32's, because user32 (dialog manager
// included) calls them directly; hooking only ShowWindow would miss most dialogs.

using NtUserShowWindow_t = BOOL(WINAPI*)(HWND, int);
NtUserShowWindow_t NtUserShowWindow_Original;
BOOL WINAPI NtUserShowWindow_Hook(HWND hwnd, int cmd) {
    if (hwnd && IsShowCommand(cmd) && !IsWindowVisible(hwnd))
        CenterNow(hwnd);
    return NtUserShowWindow_Original(hwnd, cmd);
}

using NtUserSetWindowPos_t = BOOL(WINAPI*)(HWND, HWND, int, int, int, int, UINT);
NtUserSetWindowPos_t NtUserSetWindowPos_Original;
BOOL WINAPI NtUserSetWindowPos_Hook(HWND hwnd, HWND after, int x, int y, int cx, int cy, UINT flags) {
    if (hwnd && (flags & SWP_SHOWWINDOW) && !IsWindowVisible(hwnd) && IsCandidate(hwnd)) {
        RECT rc;
        GetWindowRect(hwnd, &rc);
        int w = (flags & SWP_NOSIZE) ? rc.right - rc.left : cx;
        int h = (flags & SWP_NOSIZE) ? rc.bottom - rc.top : cy;
        int nx, ny;
        if (ComputeTarget(hwnd, w, h, &nx, &ny)) {
            x = nx;
            y = ny;
            flags &= ~SWP_NOMOVE;
        }
    }
    return NtUserSetWindowPos_Original(hwnd, after, x, y, cx, cy, flags);
}

// Windows created with WS_VISIBLE are shown inside the kernel, without any show call to hook;
// they are moved right after creation, before their first paint.
using CreateWindowExW_t = HWND(WINAPI*)(DWORD, LPCWSTR, LPCWSTR, DWORD, int, int, int, int, HWND, HMENU, HINSTANCE, LPVOID);
CreateWindowExW_t CreateWindowExW_Original;
HWND WINAPI CreateWindowExW_Hook(DWORD exStyle, LPCWSTR cls, LPCWSTR name, DWORD style, int x, int y, int w, int h,
                                 HWND parent, HMENU menu, HINSTANCE inst, LPVOID param) {
    HWND hwnd = CreateWindowExW_Original(exStyle, cls, name, style, x, y, w, h, parent, menu, inst, param);
    if (hwnd && (style & WS_VISIBLE) && !(style & WS_CHILD))
        CenterNow(hwnd);
    return hwnd;
}

using CreateWindowExA_t = HWND(WINAPI*)(DWORD, LPCSTR, LPCSTR, DWORD, int, int, int, int, HWND, HMENU, HINSTANCE, LPVOID);
CreateWindowExA_t CreateWindowExA_Original;
HWND WINAPI CreateWindowExA_Hook(DWORD exStyle, LPCSTR cls, LPCSTR name, DWORD style, int x, int y, int w, int h,
                                 HWND parent, HMENU menu, HINSTANCE inst, LPVOID param) {
    HWND hwnd = CreateWindowExA_Original(exStyle, cls, name, style, x, y, w, h, parent, menu, inst, param);
    if (hwnd && (style & WS_VISIBLE) && !(style & WS_CHILD))
        CenterNow(hwnd);
    return hwnd;
}

// ───────────── Host: hotkeys, notification, ignored windows list ─────────────
//
// Runs in a single explorer.exe process (the first one to take the mutex).

HANDLE g_hostMutex;
HANDLE g_hostThread;
HWND g_hwndHost;     // hidden window: hotkeys and timers
HWND g_hwndList;     // "Ignored windows" window
HWND g_hwndToast;
bool g_listHotkeyActive;

constexpr wchar_t kHostClass[] = L"WindowCentralizer_Host";
constexpr wchar_t kListClass[] = L"WindowCentralizer_List";
constexpr wchar_t kToastClass[] = L"WindowCentralizer_Toast";

enum { WM_APP_RELOAD = WM_APP + 1, WM_APP_EXIT };
enum { HK_IGNORE = 1, HK_LIST = 2 };
enum { TIMER_TOAST = 1 };
enum { ID_LIST = 100, ID_REMOVE, ID_HINT };

wchar_t g_toastTitle[64];
wchar_t g_toastDetail[512];

UINT KeyFromName(const wchar_t* name) {
    size_t len = wcslen(name);
    if (len == 1) {
        wchar_t c = towupper(name[0]);
        if ((c >= L'A' && c <= L'Z') || (c >= L'0' && c <= L'9'))
            return c;
        SHORT scan = VkKeyScanW(name[0]);
        return scan == -1 ? 0 : LOBYTE(scan);
    }
    if ((name[0] == L'F' || name[0] == L'f') && len <= 3) {
        int n = _wtoi(name + 1);
        if (n >= 1 && n <= 24)
            return VK_F1 + n - 1;
    }
    static const struct { const wchar_t* name; UINT vk; } kNames[] = {
        {L"Space", VK_SPACE}, {L"Enter", VK_RETURN}, {L"Tab", VK_TAB}, {L"Esc", VK_ESCAPE},
        {L"Escape", VK_ESCAPE}, {L"Insert", VK_INSERT}, {L"Delete", VK_DELETE}, {L"Home", VK_HOME},
        {L"End", VK_END}, {L"PageUp", VK_PRIOR}, {L"PageDown", VK_NEXT}, {L"Up", VK_UP},
        {L"Down", VK_DOWN}, {L"Left", VK_LEFT}, {L"Right", VK_RIGHT}, {L"Pause", VK_PAUSE},
    };
    for (const auto& k : kNames)
        if (_wcsicmp(name, k.name) == 0)
            return k.vk;
    return 0;
}

/// Parses "Ctrl+Alt+K" into MOD_* modifiers and a virtual key. False if empty or invalid.
bool ParseHotkey(const wchar_t* text, UINT* mods, UINT* vk) {
    *mods = 0;
    *vk = 0;
    wchar_t buf[64];
    wcsncpy_s(buf, text, _TRUNCATE);
    wchar_t* ctx = nullptr;
    for (wchar_t* tok = wcstok_s(buf, L"+", &ctx); tok; tok = wcstok_s(nullptr, L"+", &ctx)) {
        while (iswspace(*tok))
            tok++;
        wchar_t* end = tok + wcslen(tok);
        while (end > tok && iswspace(end[-1]))
            *--end = L'\0';
        if (!*tok)
            continue;
        if (_wcsicmp(tok, L"Ctrl") == 0 || _wcsicmp(tok, L"Control") == 0)
            *mods |= MOD_CONTROL;
        else if (_wcsicmp(tok, L"Alt") == 0)
            *mods |= MOD_ALT;
        else if (_wcsicmp(tok, L"Shift") == 0)
            *mods |= MOD_SHIFT;
        else if (_wcsicmp(tok, L"Win") == 0 || _wcsicmp(tok, L"Windows") == 0)
            *mods |= MOD_WIN;
        else if (*vk || !(*vk = KeyFromName(tok)))
            return false;
    }
    return *vk != 0;
}

bool RegisterHotkeyText(int id, const wchar_t* text) {
    UnregisterHotKey(g_hwndHost, id);
    if (!text[0])
        return false;
    UINT mods, vk;
    if (!ParseHotkey(text, &mods, &vk)) {
        Wh_Log(L"Invalid hotkey: %s", text);
        return false;
    }
    if (!RegisterHotKey(g_hwndHost, id, mods | MOD_NOREPEAT, vk)) {
        Wh_Log(L"Hotkey already in use: %s", text);
        return false;
    }
    return true;
}

// ── Ignore list editing (host only) ──

wchar_t* g_hostList;  // kIgnoredChars buffer with the current list

void HostLoadList() {
    if (!g_hostList)
        g_hostList = (wchar_t*)HeapAlloc(GetProcessHeap(), 0, kIgnoredChars * sizeof(wchar_t));
    g_hostList[0] = L'\0';
    Wh_GetStringValue(L"ignored", g_hostList, kIgnoredChars);
}

void HostSaveList() {
    Wh_SetStringValue(L"ignored", g_hostList);
    Wh_SetIntValue(L"ignoredVersion", Wh_GetIntValue(L"ignoredVersion", 0) + 1);
}

/// Line number of rule in the list, or -1.
int FindRule(const wchar_t* rule) {
    size_t ruleLen = wcslen(rule);
    int index = 0;
    for (const wchar_t* line = g_hostList; *line; index++) {
        const wchar_t* end = wcschr(line, L'\n');
        size_t len = end ? (size_t)(end - line) : wcslen(line);
        if (len == ruleLen && wcsncmp(line, rule, len) == 0)
            return index;
        if (!end)
            break;
        line = end + 1;
    }
    return -1;
}

void RemoveRuleAt(int index) {
    wchar_t* line = g_hostList;
    for (int i = 0; i < index && line; i++) {
        wchar_t* end = wcschr(line, L'\n');
        line = end ? end + 1 : nullptr;
    }
    if (!line || !*line)
        return;
    wchar_t* end = wcschr(line, L'\n');
    if (end)
        wmemmove(line, end + 1, wcslen(end + 1) + 1);
    else if (line > g_hostList)
        line[-1] = L'\0';  // last line: drop the separator before it too
    else
        line[0] = L'\0';
}

bool AddRule(const wchar_t* rule) {
    size_t used = wcslen(g_hostList), len = wcslen(rule);
    if (used + len + 2 >= kIgnoredChars)
        return false;
    if (used)
        g_hostList[used++] = L'\n';
    wcscpy_s(g_hostList + used, kIgnoredChars - used, rule);
    return true;
}

void ProcessNameOf(HWND hwnd, wchar_t* name, DWORD chars) {
    name[0] = L'\0';
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    HANDLE proc = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!proc)
        return;
    wchar_t path[MAX_PATH];
    DWORD size = MAX_PATH;
    if (QueryFullProcessImageNameW(proc, 0, path, &size)) {
        const wchar_t* slash = wcsrchr(path, L'\\');
        wcsncpy_s(name, chars, slash ? slash + 1 : path, _TRUNCATE);
    }
    CloseHandle(proc);
}

// ── Notification ──

void ShowToast(const wchar_t* title, const wchar_t* detail);

void ToggleIgnoreActive() {
    HWND hwnd = GetForegroundWindow();
    if (!hwnd || hwnd == g_hwndList || hwnd == g_hwndToast)
        return;
    wchar_t process[MAX_PATH], cls[256], title[512];
    ProcessNameOf(hwnd, process, ARRAYSIZE(process));
    GetClassNameW(hwnd, cls, ARRAYSIZE(cls));
    GetWindowTextW(hwnd, title, ARRAYSIZE(title));
    if (!process[0])
        return;
    for (wchar_t* p = title; *p; p++)
        if (*p == L'\n' || *p == L'\r')
            *p = L' ';
    wchar_t rule[1024];
    swprintf_s(rule, L"%s|%s|%s", process, cls, title);

    HostLoadList();
    int index = FindRule(rule);
    bool nowIgnored = index < 0;
    if (nowIgnored) {
        if (!AddRule(rule))
            return;
    } else {
        RemoveRuleAt(index);
    }
    HostSaveList();

    wchar_t detail[512];
    swprintf_s(detail, L"%s [%s]", process, cls);
    ShowToast(nowIgnored ? L"Ignored by Window Centralizer" : L"Centered by Window Centralizer", detail);
    if (g_hwndList)
        PostMessageW(g_hwndList, WM_APP_RELOAD, 0, 0);
}

HFONT CreateUiFont(UINT dpi, int pointsDelta, bool bold) {
    NONCLIENTMETRICSW ncm = {};
    ncm.cbSize = sizeof(ncm);
    SystemParametersInfoForDpi(SPI_GETNONCLIENTMETRICS, sizeof(ncm), &ncm, 0, dpi);
    LOGFONTW lf = ncm.lfMessageFont;
    lf.lfHeight = lf.lfHeight < 0 ? lf.lfHeight - MulDiv(pointsDelta, (int)dpi, 72) : lf.lfHeight;
    if (bold)
        lf.lfWeight = FW_SEMIBOLD;
    return CreateFontIndirectW(&lf);
}

LRESULT CALLBACK ToastProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC dc = BeginPaint(hwnd, &ps);
            UINT dpi = GetDpiForWindow(hwnd);
            RECT rc;
            GetClientRect(hwnd, &rc);
            HBRUSH bg = CreateSolidBrush(RGB(0x20, 0x20, 0x20));
            FillRect(dc, &rc, bg);
            DeleteObject(bg);
            SetBkMode(dc, TRANSPARENT);
            HFONT titleFont = CreateUiFont(dpi, 2, true);
            HFONT detailFont = CreateUiFont(dpi, 1, false);
            int pad = MulDiv(16, (int)dpi, 96);
            RECT t = {pad, MulDiv(10, (int)dpi, 96), rc.right - pad, MulDiv(32, (int)dpi, 96)};
            HGDIOBJ old = SelectObject(dc, titleFont);
            SetTextColor(dc, RGB(0xFF, 0xFF, 0xFF));
            DrawTextW(dc, g_toastTitle, -1, &t, DT_SINGLELINE | DT_END_ELLIPSIS | DT_NOPREFIX);
            RECT d = {pad, MulDiv(33, (int)dpi, 96), rc.right - pad, rc.bottom};
            SelectObject(dc, detailFont);
            SetTextColor(dc, RGB(0x90, 0x90, 0x90));
            DrawTextW(dc, g_toastDetail, -1, &d, DT_SINGLELINE | DT_END_ELLIPSIS | DT_NOPREFIX);
            SelectObject(dc, old);
            DeleteObject(titleFont);
            DeleteObject(detailFont);
            EndPaint(hwnd, &ps);
            return 0;
        }
        case WM_TIMER:
            DestroyWindow(hwnd);
            return 0;
        case WM_NCHITTEST:
            return HTTRANSPARENT;
        case WM_DESTROY:
            g_hwndToast = nullptr;
            return 0;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

void ShowToast(const wchar_t* title, const wchar_t* detail) {
    wcsncpy_s(g_toastTitle, title, _TRUNCATE);
    wcsncpy_s(g_toastDetail, detail, _TRUNCATE);
    if (g_hwndToast)
        DestroyWindow(g_hwndToast);
    POINT pt;
    GetCursorPos(&pt);
    HMONITOR mon = MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi = {};
    mi.cbSize = sizeof(mi);
    GetMonitorInfoW(mon, &mi);
    g_hwndToast = CreateWindowExW(WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE | WS_EX_TRANSPARENT, kToastClass,
                                  L"", WS_POPUP, pt.x, pt.y, 1, 1, nullptr, nullptr, GetModuleHandleW(nullptr), nullptr);
    if (!g_hwndToast)
        return;
    UINT dpi = GetDpiForWindow(g_hwndToast);
    int w = MulDiv(372, (int)dpi, 96), h = MulDiv(58, (int)dpi, 96), off = MulDiv(16, (int)dpi, 96);
    int x = pt.x + off, y = pt.y + off;
    if (x + w > mi.rcWork.right)
        x = mi.rcWork.right - w;
    if (y + h > mi.rcWork.bottom)
        y = mi.rcWork.bottom - h;
    SetWindowPos(g_hwndToast, HWND_TOPMOST, x, y, w, h, SWP_NOACTIVATE | SWP_SHOWWINDOW);
    SetTimer(g_hwndToast, TIMER_TOAST, 3000, nullptr);
}

// ── Ignored windows list ──

HWND ListCtl(int id) { return GetDlgItem(g_hwndList, id); }

void FillListBox() {
    HWND lb = ListCtl(ID_LIST);
    SendMessageW(lb, LB_RESETCONTENT, 0, 0);
    HostLoadList();
    HDC dc = GetDC(lb);
    HGDIOBJ old = SelectObject(dc, (HFONT)SendMessageW(lb, WM_GETFONT, 0, 0));
    int widest = 0;
    for (const wchar_t* line = g_hostList; *line;) {
        const wchar_t* end = wcschr(line, L'\n');
        size_t len = end ? (size_t)(end - line) : wcslen(line);
        wchar_t rule[1024], label[1100];
        if (len < ARRAYSIZE(rule)) {
            wmemcpy(rule, line, len);
            rule[len] = L'\0';
            wchar_t* bar1 = wcschr(rule, L'|');
            wchar_t* bar2 = bar1 ? wcschr(bar1 + 1, L'|') : nullptr;
            if (bar2) {
                *bar1 = *bar2 = L'\0';
                swprintf_s(label, bar2[1] ? L"%s [%s] %s" : L"%s [%s]", rule, bar1 + 1, bar2 + 1);
            } else {
                wcscpy_s(label, rule);
            }
            SendMessageW(lb, LB_ADDSTRING, 0, (LPARAM)label);
            SIZE sz;
            if (GetTextExtentPoint32W(dc, label, (int)wcslen(label), &sz) && sz.cx > widest)
                widest = sz.cx;
        }
        if (!end)
            break;
        line = end + 1;
    }
    SelectObject(dc, old);
    ReleaseDC(lb, dc);
    // Without a horizontal extent the list box shows the scroll bar but cannot scroll.
    SendMessageW(lb, LB_SETHORIZONTALEXTENT, widest + MulDiv(16, (int)GetDpiForWindow(lb), 96), 0);
}

LRESULT CALLBACK ListBoxSubclass(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam, UINT_PTR, DWORD_PTR) {
    if (msg == WM_MOUSEWHEEL && (GET_KEYSTATE_WPARAM(wParam) & MK_SHIFT)) {
        WPARAM dir = GET_WHEEL_DELTA_WPARAM(wParam) > 0 ? SB_LINELEFT : SB_LINERIGHT;
        for (int i = 0; i < 3; i++)
            SendMessageW(hwnd, WM_HSCROLL, dir, 0);
        return 0;
    }
    if (msg == WM_NCDESTROY)
        RemoveWindowSubclass(hwnd, ListBoxSubclass, 0);
    return DefSubclassProc(hwnd, msg, wParam, lParam);
}

HFONT g_listFont;
constexpr int kListClientW = 420, kListClientH = 300;

void LayoutList(UINT dpi) {
    HFONT font = CreateUiFont(dpi, 0, false);
    const int ids[] = {ID_HINT, ID_LIST, ID_REMOVE, IDCANCEL};
    for (int id : ids)
        SendMessageW(ListCtl(id), WM_SETFONT, (WPARAM)font, TRUE);
    if (g_listFont)
        DeleteObject(g_listFont);
    g_listFont = font;
    auto s = [dpi](int v) { return MulDiv(v, (int)dpi, 96); };
    SetWindowPos(ListCtl(ID_HINT), nullptr, s(14), s(12), s(392), s(36), SWP_NOZORDER | SWP_NOACTIVATE);
    SetWindowPos(ListCtl(ID_LIST), nullptr, s(14), s(52), s(392), s(190), SWP_NOZORDER | SWP_NOACTIVATE);
    SetWindowPos(ListCtl(ID_REMOVE), nullptr, s(14), s(254), s(140), s(28), SWP_NOZORDER | SWP_NOACTIVATE);
    SetWindowPos(ListCtl(IDCANCEL), nullptr, s(314), s(254), s(92), s(28), SWP_NOZORDER | SWP_NOACTIVATE);
}

void ResizeList(UINT dpi, const RECT* suggested) {
    RECT rc = {0, 0, MulDiv(kListClientW, (int)dpi, 96), MulDiv(kListClientH, (int)dpi, 96)};
    AdjustWindowRectExForDpi(&rc, (DWORD)GetWindowLongPtrW(g_hwndList, GWL_STYLE), FALSE,
                             (DWORD)GetWindowLongPtrW(g_hwndList, GWL_EXSTYLE), dpi);
    int w = rc.right - rc.left, h = rc.bottom - rc.top, x, y;
    if (suggested) {
        x = suggested->left;
        y = suggested->top;
    } else {
        POINT pt;
        GetCursorPos(&pt);
        MONITORINFO mi = {};
        mi.cbSize = sizeof(mi);
        GetMonitorInfoW(MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST), &mi);
        x = mi.rcWork.left + (mi.rcWork.right - mi.rcWork.left - w) / 2;
        y = mi.rcWork.top + (mi.rcWork.bottom - mi.rcWork.top - h) / 2;
    }
    SetWindowPos(g_hwndList, nullptr, x, y, w, h, SWP_NOZORDER | SWP_NOACTIVATE);
    LayoutList(dpi);
}

void UpdateHint() {
    wchar_t hint[160];
    if (g_settings.ignoreHotkey[0])
        swprintf_s(hint, L"Windows that are not centered. Press %s on any window to add or remove it.",
                   g_settings.ignoreHotkey);
    else
        wcscpy_s(hint, L"Windows that are not centered. Set an ignore hotkey in the mod settings to add more.");
    SetWindowTextW(ListCtl(ID_HINT), hint);
}

LRESULT CALLBACK ListProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_COMMAND:
            if (LOWORD(wParam) == ID_REMOVE) {
                int sel = (int)SendMessageW(ListCtl(ID_LIST), LB_GETCURSEL, 0, 0);
                if (sel >= 0) {
                    HostLoadList();
                    RemoveRuleAt(sel);
                    HostSaveList();
                    FillListBox();
                    int count = (int)SendMessageW(ListCtl(ID_LIST), LB_GETCOUNT, 0, 0);
                    SendMessageW(ListCtl(ID_LIST), LB_SETCURSEL, sel < count ? sel : count - 1, 0);
                }
                return 0;
            }
            if (LOWORD(wParam) == IDCANCEL) {
                DestroyWindow(hwnd);
                return 0;
            }
            break;
        case WM_APP_RELOAD:
            FillListBox();
            UpdateHint();
            return 0;
        case WM_DPICHANGED:
            ResizeList(HIWORD(wParam), (const RECT*)lParam);
            FillListBox();
            return 0;
        case WM_CLOSE:
            DestroyWindow(hwnd);
            return 0;
        case WM_DESTROY:
            g_hwndList = nullptr;
            if (g_listFont)
                DeleteObject(g_listFont);
            g_listFont = nullptr;
            return 0;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

void OpenList() {
    if (g_hwndList) {
        ShowWindow(g_hwndList, SW_RESTORE);
        SetForegroundWindow(g_hwndList);
        return;
    }
    HINSTANCE inst = GetModuleHandleW(nullptr);
    g_hwndList = CreateWindowExW(WS_EX_CONTROLPARENT | WS_EX_DLGMODALFRAME, kListClass,
                                 L"Window Centralizer - Ignored windows", WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
                                 CW_USEDEFAULT, CW_USEDEFAULT, 100, 100, nullptr, nullptr, inst, nullptr);
    if (!g_hwndList)
        return;
    auto add = [inst](const wchar_t* cls, const wchar_t* text, DWORD style, int id, DWORD ex) {
        return CreateWindowExW(ex, cls, text, WS_CHILD | WS_VISIBLE | style, 0, 0, 0, 0, g_hwndList,
                               (HMENU)(INT_PTR)id, inst, nullptr);
    };
    add(L"STATIC", L"", SS_LEFT, ID_HINT, 0);
    HWND lb = add(L"LISTBOX", L"", LBS_NOTIFY | LBS_NOINTEGRALHEIGHT | WS_VSCROLL | WS_HSCROLL | WS_TABSTOP, ID_LIST,
                  WS_EX_CLIENTEDGE);
    SetWindowSubclass(lb, ListBoxSubclass, 0, 0);
    add(L"BUTTON", L"Remove selected", BS_PUSHBUTTON | WS_TABSTOP, ID_REMOVE, 0);
    add(L"BUTTON", L"Close", BS_DEFPUSHBUTTON | WS_TABSTOP, IDCANCEL, 0);
    ResizeList(GetDpiForWindow(g_hwndList), nullptr);
    FillListBox();
    UpdateHint();
    ShowWindow(g_hwndList, SW_SHOW);
    SetForegroundWindow(g_hwndList);
}

// ── Host window and thread ──

void ApplyHostSettings(bool fromSettingsChange) {
    RegisterHotkeyText(HK_IGNORE, g_settings.ignoreHotkey);
    g_listHotkeyActive = RegisterHotkeyText(HK_LIST, g_settings.listHotkey);
    if (g_hwndList)
        PostMessageW(g_hwndList, WM_APP_RELOAD, 0, 0);
    if (fromSettingsChange && !g_listHotkeyActive)
        OpenList();
}

LRESULT CALLBACK HostProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_HOTKEY:
            if (wParam == HK_IGNORE)
                ToggleIgnoreActive();
            else if (wParam == HK_LIST)
                OpenList();
            return 0;
        case WM_APP_RELOAD:
            ApplyHostSettings(true);
            return 0;
        case WM_APP_EXIT:
            UnregisterHotKey(hwnd, HK_IGNORE);
            UnregisterHotKey(hwnd, HK_LIST);
            if (g_hwndList)
                DestroyWindow(g_hwndList);
            if (g_hwndToast)
                DestroyWindow(g_hwndToast);
            DestroyWindow(hwnd);
            return 0;
        case WM_DESTROY:
            g_hwndHost = nullptr;
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

DWORD WINAPI HostThread(void*) {
    HINSTANCE inst = GetModuleHandleW(nullptr);
    WNDCLASSEXW wc = {};
    wc.cbSize = sizeof(wc);
    wc.hInstance = inst;
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    wc.lpfnWndProc = HostProc;
    wc.lpszClassName = kHostClass;
    RegisterClassExW(&wc);
    wc.lpfnWndProc = ToastProc;
    wc.lpszClassName = kToastClass;
    RegisterClassExW(&wc);
    wc.lpfnWndProc = ListProc;
    wc.lpszClassName = kListClass;
    wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
    RegisterClassExW(&wc);

    g_hwndHost = CreateWindowExW(0, kHostClass, L"", 0, 0, 0, 0, 0, HWND_MESSAGE, nullptr, inst, nullptr);
    if (g_hwndHost) {
        ApplyHostSettings(false);
        MSG msg;
        while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
            if (g_hwndList && IsDialogMessageW(g_hwndList, &msg))
                continue;
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
    }

    UnregisterClassW(kListClass, inst);
    UnregisterClassW(kToastClass, inst);
    UnregisterClassW(kHostClass, inst);
    if (g_hostList)
        HeapFree(GetProcessHeap(), 0, g_hostList);
    g_hostList = nullptr;
    return 0;
}

// ───────────── Mod callbacks ─────────────

template <typename T>
void HookExport(const wchar_t* module, const char* name, void* hook, T* original) {
    HMODULE mod = GetModuleHandleW(module);
    void* target = mod ? (void*)GetProcAddress(mod, name) : nullptr;
    if (target)
        Wh_SetFunctionHook(target, hook, (void**)original);
    else
        Wh_Log(L"Not found: %S", name);
}

BOOL Wh_ModInit() {
    LoadSettings();

    wchar_t path[MAX_PATH];
    GetModuleFileNameW(nullptr, path, MAX_PATH);
    const wchar_t* slash = wcsrchr(path, L'\\');
    wcscpy_s(g_processName, slash ? slash + 1 : path);

    HookExport(L"win32u.dll", "NtUserShowWindow", (void*)NtUserShowWindow_Hook, &NtUserShowWindow_Original);
    HookExport(L"win32u.dll", "NtUserSetWindowPos", (void*)NtUserSetWindowPos_Hook, &NtUserSetWindowPos_Original);
    HookExport(L"user32.dll", "CreateWindowExW", (void*)CreateWindowExW_Hook, &CreateWindowExW_Original);
    HookExport(L"user32.dll", "CreateWindowExA", (void*)CreateWindowExA_Hook, &CreateWindowExA_Original);
    return TRUE;
}

void Wh_ModAfterInit() {
    if (_wcsicmp(g_processName, L"explorer.exe") != 0)
        return;
    g_hostMutex = CreateMutexW(nullptr, FALSE, L"Local\\windhawk-window-centralizer-host");
    if (g_hostMutex && GetLastError() == ERROR_ALREADY_EXISTS) {
        CloseHandle(g_hostMutex);
        g_hostMutex = nullptr;
    }
    if (g_hostMutex)
        g_hostThread = CreateThread(nullptr, 0, HostThread, nullptr, 0, nullptr);
}

void Wh_ModSettingsChanged() {
    LoadSettings();
    if (g_hwndHost)
        PostMessageW(g_hwndHost, WM_APP_RELOAD, 0, 0);
}

void Wh_ModUninit() {
    if (g_hostThread) {
        if (g_hwndHost)
            PostMessageW(g_hwndHost, WM_APP_EXIT, 0, 0);
        WaitForSingleObject(g_hostThread, 5000);
        CloseHandle(g_hostThread);
        g_hostThread = nullptr;
    }
    if (g_hostMutex) {
        CloseHandle(g_hostMutex);
        g_hostMutex = nullptr;
    }
    if (g_ignore.text)
        HeapFree(GetProcessHeap(), 0, g_ignore.text);
    g_ignore.text = nullptr;
}
