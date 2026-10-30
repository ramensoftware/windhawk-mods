// ==WindhawkMod==
// @id              window-centralizer
// @name            Window Centralizer
// @description     Predictably centers new windows and dialogs to their parent or to the mouse cursor, with a configurable threshold and exclusion list.
// @version         1.0.0
// @author          3DDinosaur
// @github          https://github.com/3DDinosaur
// @include         *
// @include         windhawk.exe
// @compilerOptions -lgdi32 -lshell32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Window Centralizer

New dialogs and small windows open centered to the one they originated from, or the mouse cursor, instead of wherever the program decides.

The window is placed **before** it appears, so there is no jump, no flicker, and window opening animations play at the final position. Elevated programs are covered as well.

## Modes

- **Parent window**: centered on the window that owns the new one; otherwise on the active or topmost window of the same program (including another instance of it, as with File Explorer folder windows). Windows with none of these, such as the Run dialog, open centered on the primary monitor.
- **Mouse cursor**: centered on the mouse cursor.

In both modes the window is kept inside the monitor's work area.

## Which windows are moved

- Only windows with a title bar that are smaller than the **maximum window size** setting (a percentage of the monitor's work area). Large main windows keep their own position.
- Standard Windows dialogs (Save As, Run, properties pages…) are always centered, whatever their size.
- Maximized, minimized, tool, layered and untitled windows, splash screens, menus, tooltips and shell surfaces (taskbar, Start, search) are never moved.
- A window that is hidden and shown again (for example a Find dialog that is reopened) is centered again.

## Ignoring windows

Press **Win+Alt+C** while a window is active to stop centering that kind of window, and press it again on it to undo. A short notification confirms the change.

A rule covers the kind of window, not the document or page it shows: ignoring one Chrome window ignores every Chrome window, and ignoring the Properties of one folder ignores Properties for every folder. Standard dialogs, which all share one window class, are told apart by the kinds of controls they contain, so Find and Save As stay separate. Dialogs of the same program built from the same kinds of controls, such as simple message boxes, are treated as one kind.

The **Ignored windows** list opens with **Win+Ctrl+Alt+C**, and entries can be removed there. If its hotkey is empty or taken by another program, that window opens whenever the mod settings are saved.

Both hotkeys can be changed in the mod settings. They run in a small background process started by Windhawk.

## Compatibility

Don't use this mod together with [Center New Windows](https://github.com/ramensoftware/windhawk-mods/blob/main/mods/center-new-windows.wh.cpp). Both reposition windows as they are shown, so the same window would be moved twice, especially dialogs when that mod's "Always center dialogs" option is on.
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
#include <windhawk_utils.h>
#include <atomic>
#include <stdio.h>
#include <string.h>
#include <wchar.h>
#include <wctype.h>

extern "C" IMAGE_DOS_HEADER __ImageBase;

HINSTANCE ModInstance() {
    return (HINSTANCE)&__ImageBase;
}

// ───────────── Settings ─────────────

enum Mode { MODE_PARENT, MODE_MOUSE };

// Read by the hooks on application threads while Wh_ModSettingsChanged rewrites them.
std::atomic<int> g_mode{MODE_PARENT};
std::atomic<int> g_maxAreaPercent{10};

// Tool process only, read and written on the host thread.
wchar_t g_ignoreHotkey[64];
wchar_t g_listHotkey[64];

wchar_t g_processName[MAX_PATH];  // file name of the current process, for ignore rules

void CopySetting(const wchar_t* name, wchar_t* dst, size_t dstChars) {
    PCWSTR value = Wh_GetStringSetting(name);
    wcsncpy_s(dst, dstChars, value, _TRUNCATE);
    Wh_FreeStringSetting(value);
}

void LoadCenteringSettings() {
    PCWSTR mode = Wh_GetStringSetting(L"mode");
    g_mode = wcscmp(mode, L"mouse") == 0 ? MODE_MOUSE : MODE_PARENT;
    Wh_FreeStringSetting(mode);
    int pct = Wh_GetIntSetting(L"maxAreaPercent");
    g_maxAreaPercent = pct < 1 ? 1 : (pct > 100 ? 100 : pct);
}

void LoadHotkeySettings() {
    CopySetting(L"ignoreHotkey", g_ignoreHotkey, ARRAYSIZE(g_ignoreHotkey));
    CopySetting(L"listHotkey", g_listHotkey, ARRAYSIZE(g_listHotkey));
}

// ───────────── Window kind ─────────────

bool IsDialogClass(const wchar_t* cls) {
    return wcscmp(cls, L"#32770") == 0;
}

/// Strips the parts of a class name that change on every run: the GUID in WPF's "HwndWrapper[...]" and
/// the per-run suffix in WinForms' "WindowsForms10.<kind>.<suffix>".
void NormalizeClass(wchar_t* cls) {
    if (wcsncmp(cls, L"HwndWrapper[", 12) == 0) {
        cls[11] = L'\0';
        return;
    }
    if (wcsncmp(cls, L"WindowsForms10.", 15) == 0) {
        if (wchar_t* dot = wcschr(cls + 15, L'.'))
            *dot = L'\0';
    }
}

/// Identifies the kind of a standard dialog, whose class (#32770) is shared by all of them, from the
/// distinct classes of its direct child controls, which don't depend on language or on the file shown.
/// A sorted set rather than the sequence: property sheets add a child page the first time a tab is
/// opened, and that must not change the signature. Writes an 8-digit hash to sig.
void DialogSignature(HWND hwnd, wchar_t* sig, size_t sigChars) {
    constexpr int kMaxClasses = 32;
    wchar_t classes[kMaxClasses][64];
    int count = 0;
    HWND child = GetWindow(hwnd, GW_CHILD);
    for (int i = 0; child && i < 256; i++, child = GetWindow(child, GW_HWNDNEXT)) {
        wchar_t cls[64];
        if (!GetClassNameW(child, cls, ARRAYSIZE(cls)))
            continue;
        for (wchar_t* c = cls; *c; c++)
            *c = towlower(*c);
        int pos = 0;
        while (pos < count && wcscmp(classes[pos], cls) < 0)
            pos++;
        if ((pos < count && wcscmp(classes[pos], cls) == 0) || count == kMaxClasses)
            continue;
        memmove(classes[pos + 1], classes[pos], (count - pos) * sizeof(classes[0]));
        wcscpy_s(classes[pos], cls);
        count++;
    }
    UINT32 h = 2166136261u;
    for (int i = 0; i < count; i++) {
        for (const wchar_t* c = classes[i]; *c; c++) {
            h ^= (UINT32)*c;
            h *= 16777619u;
        }
        h ^= L',';
        h *= 16777619u;
    }
    swprintf_s(sig, sigChars, L"%08X", h);
}

// ───────────── Ignore list ─────────────
//
// Stored in the mod's storage as one string, one rule per line: "process.exe|class|signature|title".
// - signature: DialogSignature for standard dialogs, empty for any other window (the class is its kind).
// - title: the window's title when the rule was made, shown in the list only; never matched.
// "ignoredVersion" is bumped on every change so each process reloads its cached copy only when
// the list actually changed.

constexpr size_t kIgnoredChars = 32768;
constexpr wchar_t kRulesKey[] = L"rules";

struct IgnoreCache {
    SRWLOCK lock = SRWLOCK_INIT;
    int version = -1;
    wchar_t* text = nullptr;
};
IgnoreCache g_ignore;

/// The window a rule is tested against. The dialog signature is computed on first use only.
struct WindowKey {
    HWND hwnd;
    const wchar_t* process;
    const wchar_t* cls;
    wchar_t sig[16];
    bool sigReady;
};

const wchar_t* SignatureOf(WindowKey& key) {
    if (!key.sigReady) {
        DialogSignature(key.hwnd, key.sig, ARRAYSIZE(key.sig));
        key.sigReady = true;
    }
    return key.sig;
}

/// Splits a rule line into its four fields, in place. False if it is malformed.
bool SplitRule(wchar_t* buf, wchar_t** cls, wchar_t** sig, wchar_t** title) {
    wchar_t* bar1 = wcschr(buf, L'|');
    wchar_t* bar2 = bar1 ? wcschr(bar1 + 1, L'|') : nullptr;
    wchar_t* bar3 = bar2 ? wcschr(bar2 + 1, L'|') : nullptr;
    if (!bar3)
        return false;
    *bar1 = *bar2 = *bar3 = L'\0';
    *cls = bar1 + 1;
    *sig = bar2 + 1;
    *title = bar3 + 1;
    return true;
}

bool RuleMatches(const wchar_t* rule, size_t ruleLen, WindowKey& key) {
    wchar_t buf[1024];
    if (ruleLen >= ARRAYSIZE(buf))
        return false;
    wmemcpy(buf, rule, ruleLen);
    buf[ruleLen] = L'\0';
    wchar_t *cls, *sig, *title;
    if (!SplitRule(buf, &cls, &sig, &title))
        return false;
    if (_wcsicmp(buf, key.process) != 0 || _wcsicmp(cls, key.cls) != 0)
        return false;
    return !*sig || wcscmp(sig, SignatureOf(key)) == 0;
}

bool IsIgnored(HWND hwnd, const wchar_t* cls) {
    int version = Wh_GetIntValue(L"ignoredVersion", 0);
    AcquireSRWLockExclusive(&g_ignore.lock);
    if (version != g_ignore.version) {
        if (!g_ignore.text)
            g_ignore.text = (wchar_t*)HeapAlloc(GetProcessHeap(), 0, kIgnoredChars * sizeof(wchar_t));
        if (g_ignore.text) {
            g_ignore.text[0] = L'\0';
            Wh_GetStringValue(kRulesKey, g_ignore.text, kIgnoredChars);
            g_ignore.version = version;
        }
    }
    wchar_t kind[256];
    wcscpy_s(kind, cls);
    NormalizeClass(kind);
    WindowKey key = {hwnd, g_processName, kind, L"", false};
    bool ignored = false;
    for (const wchar_t* line = g_ignore.text; line && *line && !ignored;) {
        const wchar_t* end = wcschr(line, L'\n');
        size_t len = end ? (size_t)(end - line) : wcslen(line);
        ignored = len && RuleMatches(line, len, key);
        line = end ? end + 1 : nullptr;
    }
    ReleaseSRWLockExclusive(&g_ignore.lock);
    return ignored;
}

// ───────────── Which windows to center ─────────────

bool ContainsNoCase(const wchar_t* haystack, const wchar_t* needle) {
    size_t n = wcslen(needle);
    if (!n)
        return true;
    for (; *haystack; haystack++)
        if (_wcsnicmp(haystack, needle, n) == 0)
            return true;
    return false;
}

bool HasCaption(HWND hwnd) {
    return (GetWindowLongPtrW(hwnd, GWL_STYLE) & WS_CAPTION) == WS_CAPTION;
}

bool IsIgnoredClass(const wchar_t* cls) {
    static const wchar_t* const kParts[] = {
        L"Splash", L"Loading", L"MsoSplash", L"Ghost", L"NetUI", L"#32768", L"Menu", L"Shadow", L"Tooltip",
        L"NotifyIcon", L"Popup", L"Overlay", L"Dimmer", L"Shell_TrayWnd", L"TrayNotifyWnd", L"Taskbar",
        L"Progman", L"WorkerW", L"SearchHost", L"Windows.UI.Core.CoreWindow",
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

/// A window this mod may move: owned by this process, top level, title bar, not min/maximized,
/// not tool/layered, titled, not ignored.
bool IsCandidate(HWND hwnd) {
    // A program can show another process's window; the rules and parent search describe this process only.
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    if (pid != GetCurrentProcessId())
        return false;
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
    return !IsIgnored(hwnd, cls);
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
    // Only when this program is in the foreground: from another app or the desktop (Win+R), its topmost
    // window is unrelated to what the user did.
    wchar_t fgCls[256] = L"";
    bool fgIsShell = fg && GetClassNameW(fg, fgCls, ARRAYSIZE(fgCls)) && IsIgnoredClass(fgCls);
    if (fgPid == GetCurrentProcessId() && !fgIsShell) {
        TopWindowSearch search = {hwnd, nullptr};
        EnumWindows(FindTopOwnWindow, (LPARAM)&search);
        if (search.found)
            return search.found;
    }

    if (fg && fgPid != GetCurrentProcessId() && IsValidParent(fg, hwnd) && SameExecutable(fg))
        return fg;
    return nullptr;
}

POINT ReferencePoint(HWND hwnd) {
    if (g_mode == MODE_MOUSE) {
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
    bool isDialog = GetClassNameW(hwnd, cls, ARRAYSIZE(cls)) && IsDialogClass(cls);
    if (!isDialog && (long long)w * h * 100 > monitorArea * g_maxAreaPercent)
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
using CreateWindowExW_t = decltype(&CreateWindowExW);
CreateWindowExW_t CreateWindowExW_Original;
HWND WINAPI CreateWindowExW_Hook(DWORD exStyle, LPCWSTR cls, LPCWSTR name, DWORD style, int x, int y, int w, int h,
                                 HWND parent, HMENU menu, HINSTANCE inst, LPVOID param) {
    HWND hwnd = CreateWindowExW_Original(exStyle, cls, name, style, x, y, w, h, parent, menu, inst, param);
    if (hwnd && (style & WS_VISIBLE) && !(style & WS_CHILD))
        CenterNow(hwnd);
    return hwnd;
}

using CreateWindowExA_t = decltype(&CreateWindowExA);
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
// Runs only in the mod's dedicated windhawk.exe process (see "Mod callbacks").

HANDLE g_hostThread;
HANDLE g_hostStopEvent;
HWND g_hwndHost;     // hidden window: hotkeys and timers
HWND g_hwndList;     // "Ignored windows" window
HWND g_hwndToast;
bool g_listHotkeyActive;

constexpr wchar_t kHostClass[] = L"WindowCentralizer_Host";
constexpr wchar_t kListClass[] = L"WindowCentralizer_List";
constexpr wchar_t kToastClass[] = L"WindowCentralizer_Toast";

enum { WM_APP_RELOAD = WM_APP + 1 };
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
    Wh_GetStringValue(kRulesKey, g_hostList, kIgnoredChars);
}

void HostSaveList() {
    Wh_SetStringValue(kRulesKey, g_hostList);
    Wh_SetIntValue(L"ignoredVersion", Wh_GetIntValue(L"ignoredVersion", 0) + 1);
}

/// Line number of the first rule that matches the window, or -1.
int FindMatchingRule(WindowKey& key) {
    int index = 0;
    for (const wchar_t* line = g_hostList; *line; index++) {
        const wchar_t* end = wcschr(line, L'\n');
        size_t len = end ? (size_t)(end - line) : wcslen(line);
        if (len && RuleMatches(line, len, key))
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
    NormalizeClass(cls);
    TitleOf(hwnd, title, ARRAYSIZE(title));
    if (!process[0])
        return;
    for (wchar_t* p = title; *p; p++)
        if (*p == L'\n' || *p == L'\r')
            *p = L' ';

    // Toggle by what actually matches the window, so an existing rule is removed instead of duplicated.
    WindowKey key = {hwnd, process, cls, L"", false};
    HostLoadList();
    int index = FindMatchingRule(key);
    bool nowIgnored = index < 0;
    if (nowIgnored) {
        bool dialog = IsDialogClass(cls);
        wchar_t rule[1024];
        swprintf_s(rule, L"%s|%s|%s|%s", process, cls, dialog ? SignatureOf(key) : L"", dialog ? title : L"");
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
                                  L"", WS_POPUP, pt.x, pt.y, 1, 1, nullptr, nullptr, ModInstance(), nullptr);
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
            wchar_t *cls, *sig, *title;
            if (SplitRule(rule, &cls, &sig, &title))
                swprintf_s(label, *sig ? L"%s [%s] like \"%s\"" : L"%s [%s] (all windows)", rule, cls, title);
            else
                wcscpy_s(label, rule);
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

/// Shift+wheel over the list box scrolls it horizontally, which the stock list box doesn't do.
bool HandleListShiftWheel(const MSG& msg) {
    if (msg.message != WM_MOUSEWHEEL || !g_hwndList || msg.hwnd != ListCtl(ID_LIST) ||
        !(GET_KEYSTATE_WPARAM(msg.wParam) & MK_SHIFT))
        return false;
    WPARAM dir = GET_WHEEL_DELTA_WPARAM(msg.wParam) > 0 ? SB_LINELEFT : SB_LINERIGHT;
    for (int i = 0; i < 3; i++)
        SendMessageW(msg.hwnd, WM_HSCROLL, dir, 0);
    return true;
}

HFONT g_listFont;
constexpr int kListClientW = 420;

/// Applies the font and places the controls for dpi, sizing the hint to its wrapped text. Returns the client height needed.
int LayoutList(UINT dpi) {
    HFONT font = CreateUiFont(dpi, 0, false);
    const int ids[] = {ID_HINT, ID_LIST, ID_REMOVE, IDCANCEL};
    for (int id : ids)
        SendMessageW(ListCtl(id), WM_SETFONT, (WPARAM)font, TRUE);
    if (g_listFont)
        DeleteObject(g_listFont);
    g_listFont = font;
    auto s = [dpi](int v) { return MulDiv(v, (int)dpi, 96); };

    wchar_t hint[256] = L"";
    GetWindowTextW(ListCtl(ID_HINT), hint, ARRAYSIZE(hint));
    RECT tr = {0, 0, s(392), 0};
    HDC dc = GetDC(g_hwndList);
    HGDIOBJ old = SelectObject(dc, font);
    DrawTextW(dc, hint, -1, &tr, DT_CALCRECT | DT_WORDBREAK | DT_NOPREFIX);
    SelectObject(dc, old);
    ReleaseDC(g_hwndList, dc);

    int listY = s(12) + tr.bottom + s(8);
    int buttonY = listY + s(190) + s(12);
    SetWindowPos(ListCtl(ID_HINT), nullptr, s(14), s(12), s(392), tr.bottom, SWP_NOZORDER | SWP_NOACTIVATE);
    SetWindowPos(ListCtl(ID_LIST), nullptr, s(14), listY, s(392), s(190), SWP_NOZORDER | SWP_NOACTIVATE);
    SetWindowPos(ListCtl(ID_REMOVE), nullptr, s(14), buttonY, s(140), s(28), SWP_NOZORDER | SWP_NOACTIVATE);
    SetWindowPos(ListCtl(IDCANCEL), nullptr, s(314), buttonY, s(92), s(28), SWP_NOZORDER | SWP_NOACTIVATE);
    return buttonY + s(28) + s(18);
}

/// Sizes the window to its content. Position: the suggested rect if given, else centered on the cursor's
/// monitor when center is set, else unchanged.
void ResizeList(UINT dpi, const RECT* suggested, bool center) {
    RECT rc = {0, 0, MulDiv(kListClientW, (int)dpi, 96), LayoutList(dpi)};
    AdjustWindowRectExForDpi(&rc, (DWORD)GetWindowLongPtrW(g_hwndList, GWL_STYLE), FALSE,
                             (DWORD)GetWindowLongPtrW(g_hwndList, GWL_EXSTYLE), dpi);
    int w = rc.right - rc.left, h = rc.bottom - rc.top;
    RECT cur;
    GetWindowRect(g_hwndList, &cur);
    int x = cur.left, y = cur.top;
    if (suggested) {
        x = suggested->left;
        y = suggested->top;
    } else if (center) {
        POINT pt;
        GetCursorPos(&pt);
        MONITORINFO mi = {};
        mi.cbSize = sizeof(mi);
        GetMonitorInfoW(MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST), &mi);
        x = mi.rcWork.left + (mi.rcWork.right - mi.rcWork.left - w) / 2;
        y = mi.rcWork.top + (mi.rcWork.bottom - mi.rcWork.top - h) / 2;
    }
    SetWindowPos(g_hwndList, nullptr, x, y, w, h, SWP_NOZORDER | SWP_NOACTIVATE);
}

void UpdateHint() {
    wchar_t hint[160];
    if (g_ignoreHotkey[0])
        swprintf_s(hint, L"Windows that are not centered. Press %s on any window to add or remove it.",
                   g_ignoreHotkey);
    else
        wcscpy_s(hint, L"Windows that are not centered. Set an ignore hotkey in the mod settings to add more.");
    SetWindowTextW(ListCtl(ID_HINT), hint);
}

LRESULT CALLBACK ListProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_COMMAND:
            if (LOWORD(wParam) == ID_REMOVE) {
                HWND lb = ListCtl(ID_LIST);
                int count = (int)SendMessageW(lb, LB_GETSELCOUNT, 0, 0);
                int* items = count > 0 ? (int*)HeapAlloc(GetProcessHeap(), 0, count * sizeof(int)) : nullptr;
                if (items) {
                    count = (int)SendMessageW(lb, LB_GETSELITEMS, count, (LPARAM)items);
                    HostLoadList();
                    // Indices come in ascending order; removing from the end keeps the earlier ones valid.
                    for (int i = count - 1; i >= 0; i--)
                        RemoveRuleAt(items[i]);
                    HostSaveList();
                    int next = items[0];
                    HeapFree(GetProcessHeap(), 0, items);
                    FillListBox();
                    int total = (int)SendMessageW(lb, LB_GETCOUNT, 0, 0);
                    if (total > 0)
                        SendMessageW(lb, LB_SETSEL, TRUE, next < total ? next : total - 1);
                }
                return 0;
            }
            if (LOWORD(wParam) == IDCANCEL) {
                DestroyWindow(hwnd);
                return 0;
            }
            break;
        case WM_APP_RELOAD:
            UpdateHint();
            ResizeList(GetDpiForWindow(hwnd), nullptr, false);
            FillListBox();
            return 0;
        case WM_DPICHANGED:
            ResizeList(HIWORD(wParam), (const RECT*)lParam, false);
            FillListBox();
            return 0;
        case WM_CLOSE:
            DestroyWindow(hwnd);
            return 0;
        case WM_DESTROY:
            g_hwndList = nullptr;
            return 0;
        case WM_NCDESTROY:
            // The child controls are destroyed after the parent's WM_DESTROY and still hold the font until then.
            if (g_listFont)
                DeleteObject(g_listFont);
            g_listFont = nullptr;
            break;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

void OpenList() {
    if (g_hwndList) {
        ShowWindow(g_hwndList, SW_RESTORE);
        SetForegroundWindow(g_hwndList);
        return;
    }
    HINSTANCE inst = ModInstance();
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
    add(L"LISTBOX", L"",
        LBS_NOTIFY | LBS_EXTENDEDSEL | LBS_NOINTEGRALHEIGHT | WS_VSCROLL | WS_HSCROLL | WS_TABSTOP, ID_LIST,
        WS_EX_CLIENTEDGE);
    add(L"BUTTON", L"Remove selected", BS_PUSHBUTTON | WS_TABSTOP, ID_REMOVE, 0);
    add(L"BUTTON", L"Close", BS_DEFPUSHBUTTON | WS_TABSTOP, IDCANCEL, 0);
    UpdateHint();
    ResizeList(GetDpiForWindow(g_hwndList), nullptr, true);
    FillListBox();
    ShowWindow(g_hwndList, SW_SHOW);
    SetForegroundWindow(g_hwndList);
}

// ── Host window and thread ──

void ApplyHostSettings(bool fromSettingsChange) {
    LoadHotkeySettings();
    RegisterHotkeyText(HK_IGNORE, g_ignoreHotkey);
    g_listHotkeyActive = RegisterHotkeyText(HK_LIST, g_listHotkey);
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
        case WM_DESTROY:
            g_hwndHost = nullptr;
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

void RunHostLoop() {
    for (;;) {
        if (MsgWaitForMultipleObjects(1, &g_hostStopEvent, FALSE, INFINITE, QS_ALLINPUT) == WAIT_OBJECT_0)
            return;
        MSG msg;
        while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT)
                return;
            if (HandleListShiftWheel(msg))
                continue;
            if (g_hwndList && IsDialogMessageW(g_hwndList, &msg))
                continue;
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
    }
}

DWORD WINAPI HostThread(void*) {
    HINSTANCE inst = ModInstance();
    WNDCLASSEXW wc = {};
    wc.cbSize = sizeof(wc);
    wc.hInstance = inst;
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    wc.lpfnWndProc = HostProc;
    wc.lpszClassName = kHostClass;
    ATOM hostAtom = RegisterClassExW(&wc);
    wc.lpfnWndProc = ToastProc;
    wc.lpszClassName = kToastClass;
    ATOM toastAtom = RegisterClassExW(&wc);
    wc.lpfnWndProc = ListProc;
    wc.lpszClassName = kListClass;
    wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
    ATOM listAtom = RegisterClassExW(&wc);

    if (hostAtom && toastAtom && listAtom) {
        g_hwndHost = CreateWindowExW(0, kHostClass, L"", 0, 0, 0, 0, 0, HWND_MESSAGE, nullptr, inst, nullptr);
        if (g_hwndHost) {
            ApplyHostSettings(false);
            RunHostLoop();
        }
    } else {
        Wh_Log(L"RegisterClassExW failed: %u", GetLastError());
    }

    if (g_hwndHost) {
        UnregisterHotKey(g_hwndHost, HK_IGNORE);
        UnregisterHotKey(g_hwndHost, HK_LIST);
    }
    if (g_hwndList)
        DestroyWindow(g_hwndList);
    if (g_hwndToast)
        DestroyWindow(g_hwndToast);
    if (g_hwndHost)
        DestroyWindow(g_hwndHost);

    if (listAtom)
        UnregisterClassW(kListClass, inst);
    if (toastAtom)
        UnregisterClassW(kToastClass, inst);
    if (hostAtom)
        UnregisterClassW(kHostClass, inst);
    if (g_hostList)
        HeapFree(GetProcessHeap(), 0, g_hostList);
    g_hostList = nullptr;
    return 0;
}

BOOL WhTool_ModInit() {
    g_hostStopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_hostStopEvent)
        return FALSE;
    g_hostThread = CreateThread(nullptr, 0, HostThread, nullptr, 0, nullptr);
    return g_hostThread != nullptr;
}

void WhTool_ModSettingsChanged() {
    if (g_hwndHost)
        PostMessageW(g_hwndHost, WM_APP_RELOAD, 0, 0);
}

void WhTool_ModUninit() {
    SetEvent(g_hostStopEvent);
    // A system modal loop (menu open, window being dragged) doesn't watch the event; WM_QUIT ends it.
    PostThreadMessageW(GetThreadId(g_hostThread), WM_QUIT, 0, 0);
    WaitForSingleObject(g_hostThread, INFINITE);
    CloseHandle(g_hostThread);
    CloseHandle(g_hostStopEvent);
}

// ───────────── Mod callbacks ─────────────
//
// Each process is routed in the Wh_Mod* callbacks below: windhawk.exe and any process started with
// -tool-mod (Windhawk 2.0 starts this mod's tool process as windhawk-mod.exe) go to the tool-mod part,
// which launches or runs the tool process holding the hotkeys and the UI; every other process gets the
// centering hooks. The tool-mod part is the wiki snippet, unchanged except for its entry-point names.

////////////////////////////////////////////////////////////////////////////////
// Windhawk tool mod implementation for mods which don't need to inject to other
// processes or hook other functions. Context:
// https://github.com/ramensoftware/windhawk/wiki/Mods-as-tools:-Running-mods-in-a-dedicated-process
//
// The mod will load and run in a dedicated windhawk.exe process.
//
// Paste the code below as part of the mod code, and use these callbacks:
// * WhTool_ModInit
// * WhTool_ModSettingsChanged
// * WhTool_ModUninit
//
// Currently, other callbacks are not supported.

bool g_isToolModProcessLauncher;
HANDLE g_toolModProcessMutex;

void WINAPI EntryPoint_Hook() {
    Wh_Log(L">");
    ExitThread(0);
}

BOOL ToolMod_Init() {
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

void ToolMod_AfterInit() {
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

    WCHAR
    commandLine[MAX_PATH + 2 +
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

void ToolMod_SettingsChanged() {
    if (g_isToolModProcessLauncher) {
        return;
    }

    WhTool_ModSettingsChanged();
}

void ToolMod_Uninit() {
    if (g_isToolModProcessLauncher) {
        return;
    }

    WhTool_ModUninit();
    ExitProcess(0);
}

////////////////////////////////////////////////////////////////////////////////

bool g_inToolModPart;

bool HasToolModArg() {
    int argc;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);
    if (!argv)
        return false;
    bool found = false;
    for (int i = 1; i < argc && !found; i++)
        found = wcscmp(argv[i], L"-tool-mod") == 0;
    LocalFree(argv);
    return found;
}

BOOL Wh_ModInit() {
    wchar_t path[MAX_PATH];
    GetModuleFileNameW(nullptr, path, MAX_PATH);
    const wchar_t* slash = wcsrchr(path, L'\\');
    wcscpy_s(g_processName, slash ? slash + 1 : path);

    if (_wcsicmp(g_processName, L"windhawk.exe") == 0 || HasToolModArg()) {
        g_inToolModPart = true;
        return ToolMod_Init();
    }

    LoadCenteringSettings();
    if (HMODULE win32u = GetModuleHandleW(L"win32u.dll")) {
        if (auto p = (NtUserShowWindow_t)GetProcAddress(win32u, "NtUserShowWindow"))
            WindhawkUtils::SetFunctionHook(p, NtUserShowWindow_Hook, &NtUserShowWindow_Original);
        if (auto p = (NtUserSetWindowPos_t)GetProcAddress(win32u, "NtUserSetWindowPos"))
            WindhawkUtils::SetFunctionHook(p, NtUserSetWindowPos_Hook, &NtUserSetWindowPos_Original);
    }
    WindhawkUtils::SetFunctionHook(CreateWindowExW, CreateWindowExW_Hook, &CreateWindowExW_Original);
    WindhawkUtils::SetFunctionHook(CreateWindowExA, CreateWindowExA_Hook, &CreateWindowExA_Original);
    return TRUE;
}

void Wh_ModAfterInit() {
    if (g_inToolModPart)
        ToolMod_AfterInit();
}

void Wh_ModSettingsChanged() {
    if (g_inToolModPart)
        ToolMod_SettingsChanged();
    else
        LoadCenteringSettings();
}

void Wh_ModUninit() {
    if (g_inToolModPart) {
        ToolMod_Uninit();
        return;
    }
    if (g_ignore.text)
        HeapFree(GetProcessHeap(), 0, g_ignore.text);
    g_ignore.text = nullptr;
}
