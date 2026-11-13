// ==WindhawkMod==
// @id              window-centralizer
// @name            Window Centralizer
// @description     Predictably centers new windows and dialogs to their parent, the screen or the mouse cursor, with configurable threshold and live exclusion list.
// @version         1.2.0
// @author          3DDinosaur
// @github          https://github.com/3DDinosaur
// @include         *
// @include         windhawk.exe
// @compilerOptions -lgdi32 -lshell32 -ldwmapi
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Window Centralizer

New dialogs and small windows open centered to where they came from, the mouse cursor, or the monitor, instead of randomly around the screen.

The window is placed **before** it appears, so there is no jump and no flicker. Elevated programs are covered as well.

With **Show windows after complete UI load** on, a centered window is also kept invisible for a moment while it draws its contents, so classic dialogs don't flash blank while loading. Its opening animation, from Windows or from an animation mod, may then be skipped; turn the option off to keep animations playing at the final position.

## Modes

- **Parent window / monitor**: centered on the window it came from, otherwise the active or topmost window of the same program. Orphan windows, such as the Run dialog or a program just launched, open centered on the monitor where the mouse cursor is. With **Center on the unrelated launcher window** on, a program started from another one opens centered on the window it was started from.
- **Parent monitor only**: centered on the monitor of the window it came from, or on the monitor with the mouse cursor when orphan. With the window size limit to target at 100%, every window opens at the center of the screen.
- **Mouse cursor**: centered on the mouse cursor.

In every mode the window is kept inside the monitor's work area.

## Which windows are moved

- Only windows with a title bar that are smaller than the **window size limit to target** setting (a percentage of the monitor's work area). Large main windows keep their assigned position.
- Standard Windows dialogs (Save As, Run, properties pages…) are always centered, whatever their size.
- Windows of UWP apps such as Calculator and Clock are centered the first time they get focus, so they can be seen jumping after appearing.
- Maximized, minimized, tool, layered and untitled windows, splash screens, menus, tooltips and shell surfaces (taskbar, Start, search) are never moved. Windows that an animation mod fades in are still centered.
- A window that is hidden and shown again (for example a Find dialog that is reopened) is centered again.
- Some programs move their window again right after showing it (Firefox). The mod takes care of that. Initial moves by other tools (FancyZones), and the user's own dragging, are left alone.

## Ignoring windows

Press **Win+Alt+I** while a window is active to stop centering that kind of window. Press it again to undo. A short notification confirms the option.

It covers the kind of window, not the document or page it shows: ignoring one Chrome window ignores every Chrome window, and ignoring the Properties of a folder ignores Properties for every folder. Standard dialogs, which all share one window class, are told apart by the kinds of controls they contain, so Find and Save As stay separate. Dialogs of the same program built from the same kinds of controls, such as simple message boxes, are treated as one kind. UWP apps are told apart by the app's own program, so each one can be ignored separately.

The **Ignored windows** list opens with **Win+Ctrl+Alt+I**, and entries can be reviewed and removed there. If its hotkey is empty or taken by another program, that window opens whenever the mod settings are saved.

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
    - parent: Parent window / monitor
    - monitor: Parent monitor only
    - mouse: Mouse cursor
- centerOnLauncher: false
  $name: Center on the unrelated launcher window
  $description: A program started from another one (context menu, file opened from a folder, etc.) is centered on the window it was started from, instead of the monitor.
  $showIf: {mode: parent}
- maxAreaPercent: 10
  $name: Window size limit to target — % of the monitor area
  $description: Windows larger than this keep their assigned position. Standard dialogs are centered regardless, but can be set to be ignored.
- revealWhenDrawn: true
  $name: Show windows after complete UI load
  $description: A centered window stays invisible until its contents finish drawing (half a second at most), avoiding the native flicker. Windows drawn with DirectX (browsers, WPF, WinUI) can't be detected and gain little. Opening animations may be skipped; turn off to keep them.
- ignoreHotkey: Win+Alt+I
  $name: Ignore hotkey
  $description: Add/remove the active window to the list. Modifiers (Ctrl, Alt, Shift, Win) and one key, joined with + (Win+Alt+I). Leave empty for no hotkey.
- listHotkey: Win+Ctrl+Alt+I
  $name: Ignored list hotkey
  $description: Opens the list of ignored windows, same format as above. If empty, the list opens whenever these settings are saved.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <dwmapi.h>
#include <tlhelp32.h>
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

enum Mode { MODE_PARENT, MODE_MONITOR, MODE_MOUSE };

// Read by the hooks on application threads while Wh_ModSettingsChanged rewrites them.
std::atomic<int> g_mode{MODE_PARENT};
std::atomic<int> g_maxAreaPercent{10};
std::atomic<bool> g_revealWhenDrawn{true};
std::atomic<bool> g_centerOnLauncher{false};

// Tool process only, read and written on the host thread.
wchar_t g_ignoreHotkey[64];
wchar_t g_listHotkey[64];

wchar_t g_processName[MAX_PATH];  // file name of the current process, for ignore rules

void CopySetting(const wchar_t* name, wchar_t* dst, size_t dstChars) {
    auto value = WindhawkUtils::StringSetting::make(name);
    wcsncpy_s(dst, dstChars, value.get(), _TRUNCATE);
}

void LoadCenteringSettings() {
    auto mode = WindhawkUtils::StringSetting::make(L"mode");
    g_mode = wcscmp(mode.get(), L"mouse") == 0     ? MODE_MOUSE
             : wcscmp(mode.get(), L"monitor") == 0 ? MODE_MONITOR
                                                   : MODE_PARENT;
    int pct = Wh_GetIntSetting(L"maxAreaPercent");
    g_maxAreaPercent = pct < 1 ? 1 : (pct > 100 ? 100 : pct);
    g_revealWhenDrawn = Wh_GetIntSetting(L"revealWhenDrawn") != 0;
    g_centerOnLauncher = Wh_GetIntSetting(L"centerOnLauncher") != 0;
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
// Each rule is "process.exe|class|signature|title".
// - signature: DialogSignature for standard dialogs, empty for any other window (the class is its kind).
// - title: the window's title when the rule was made, shown in the list only; never matched.
// Stored as "ruleCount" plus one value per rule ("rule0", "rule1"…), since portable Windhawk keeps
// storage in an INI file, which can't hold line breaks. In memory, the rules are one string, one per line.
// "ignoredVersion" is bumped on every change so each process reloads its cached copy only when
// the list actually changed.

constexpr size_t kIgnoredChars = 32768;
constexpr size_t kRuleChars = 1024;
constexpr wchar_t kLegacyRulesKey[] = L"rules";  // 1.0–1.1: all rules in one value, joined with \n

void RuleValueName(int index, wchar_t* name, size_t chars) {
    swprintf_s(name, chars, L"rule%d", index);
}

/// Reads the stored rules into buf, one per line. Falls back to the legacy single value if the list
/// hasn't been converted yet.
void ReadRules(wchar_t* buf, size_t chars) {
    buf[0] = L'\0';
    int count = Wh_GetIntValue(L"ruleCount", -1);
    if (count < 0) {
        Wh_GetStringValue(kLegacyRulesKey, buf, chars);
        return;
    }
    size_t used = 0;
    for (int i = 0; i < count; i++) {
        wchar_t name[16], rule[kRuleChars];
        RuleValueName(i, name, ARRAYSIZE(name));
        rule[0] = L'\0';
        Wh_GetStringValue(name, rule, ARRAYSIZE(rule));
        size_t len = wcslen(rule);
        if (!len || used + len + 2 >= chars)
            continue;
        if (used)
            buf[used++] = L'\n';
        wmemcpy(buf + used, rule, len + 1);
        used += len;
    }
}

/// Stores the rules in text (one per line), deletes values left over from a longer list, and bumps
/// "ignoredVersion".
void WriteRules(const wchar_t* text) {
    int oldCount = Wh_GetIntValue(L"ruleCount", 0);
    int count = 0;
    for (const wchar_t* line = text; *line;) {
        const wchar_t* end = wcschr(line, L'\n');
        size_t len = end ? (size_t)(end - line) : wcslen(line);
        if (len && len < kRuleChars) {
            wchar_t name[16], rule[kRuleChars];
            wmemcpy(rule, line, len);
            rule[len] = L'\0';
            RuleValueName(count++, name, ARRAYSIZE(name));
            Wh_SetStringValue(name, rule);
        }
        if (!end)
            break;
        line = end + 1;
    }
    Wh_SetIntValue(L"ruleCount", count);
    for (int i = count; i < oldCount; i++) {
        wchar_t name[16];
        RuleValueName(i, name, ARRAYSIZE(name));
        Wh_DeleteValue(name);
    }
    Wh_SetIntValue(L"ignoredVersion", Wh_GetIntValue(L"ignoredVersion", 0) + 1);
}

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
    wchar_t buf[kRuleChars];
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

/// Whether a rule matches the window, as a window of the given process (process name, class, dialog signature).
bool IsIgnoredAs(const wchar_t* process, HWND hwnd, const wchar_t* cls) {
    int version = Wh_GetIntValue(L"ignoredVersion", 0);
    AcquireSRWLockExclusive(&g_ignore.lock);
    if (version != g_ignore.version) {
        if (!g_ignore.text)
            g_ignore.text = (wchar_t*)HeapAlloc(GetProcessHeap(), 0, kIgnoredChars * sizeof(wchar_t));
        if (g_ignore.text) {
            ReadRules(g_ignore.text, kIgnoredChars);
            g_ignore.version = version;
        }
    }
    wchar_t kind[256];
    wcscpy_s(kind, cls);
    NormalizeClass(kind);
    WindowKey key = {hwnd, process, kind, L"", false};
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

bool IsIgnored(HWND hwnd, const wchar_t* cls) {
    return IsIgnoredAs(g_processName, hwnd, cls);
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
    // Windows' own classes, matched exactly: as substrings, "Ghost" (the hung-window ghost) also caught
    // Blender's GHOST_WindowClass.
    static const wchar_t* const kExact[] = {
        L"Ghost", L"#32768", L"Shell_TrayWnd", L"TrayNotifyWnd", L"Progman", L"WorkerW",
        L"Windows.UI.Core.CoreWindow",
        L"ApplicationFrameWindow",  // UWP frames: centered by the tool process, see "UWP app windows"
    };
    for (const wchar_t* name : kExact)
        if (_wcsicmp(cls, name) == 0)
            return true;
    static const wchar_t* const kParts[] = {
        L"Splash", L"Loading", L"NetUI", L"Menu", L"Shadow", L"Tooltip", L"NotifyIcon", L"Popup",
        L"Overlay", L"Dimmer", L"Taskbar", L"SearchHost",
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

// Animation mods (such as Windhawk's Windows Animations) make an ordinary window layered with alpha 0
// right before it is shown, then fade it in; it must still be centered.
bool IsHiddenForFadeIn(HWND hwnd) {
    COLORREF key = 0;
    BYTE alpha = 255;
    DWORD flags = 0;
    return GetLayeredWindowAttributes(hwnd, &key, &alpha, &flags) && (flags & LWA_ALPHA) && alpha == 0;
}

/// A window this mod may move: owned by this process, top level, title bar, not min/maximized,
/// not tool/layered (unless being faded in), titled, not ignored.
bool IsCandidate(HWND hwnd) {
    // A program can show another process's window; the rules and parent search describe this process only.
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    if (pid != GetCurrentProcessId())
        return false;
    LONG_PTR style = GetWindowLongPtrW(hwnd, GWL_STYLE);
    if ((style & WS_CHILD) || (style & WS_CAPTION) != WS_CAPTION || (style & (WS_MINIMIZE | WS_MAXIMIZE)))
        return false;
    LONG_PTR exStyle = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);
    if ((exStyle & WS_EX_TOOLWINDOW) || ((exStyle & WS_EX_LAYERED) && !IsHiddenForFadeIn(hwnd)))
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

POINT WorkAreaCenter(HMONITOR mon) {
    MONITORINFO mi = {};
    mi.cbSize = sizeof(mi);
    GetMonitorInfoW(mon, &mi);
    return {(mi.rcWork.left + mi.rcWork.right) / 2, (mi.rcWork.top + mi.rcWork.bottom) / 2};
}

/// Center of the monitor with the mouse cursor, where the user last was.
POINT CursorMonitorCenter() {
    POINT pt;
    GetCursorPos(&pt);
    return WorkAreaCenter(MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST));
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

/// File name of a process's executable, empty if it can't be read.
void ExeNameOfPid(DWORD pid, wchar_t* name, DWORD chars) {
    name[0] = L'\0';
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

bool SameExecutable(HWND hwnd) {
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    wchar_t name[MAX_PATH];
    ExeNameOfPid(pid, name, MAX_PATH);
    return *name && _wcsicmp(name, g_processName) == 0;
}

wchar_t g_launcherName[MAX_PATH];  // executable of LauncherPid(), set with it
HWND g_launchForeground;           // foreground window when this process started, set in Wh_ModInit

/// The process that started this one, or 0 if unknown or no longer running. Looked up on first use.
DWORD LauncherPid() {
    static std::atomic<DWORD> cached{0xFFFFFFFF};  // never a real PID: those are multiples of 4
    DWORD pid = cached;
    if (pid != 0xFFFFFFFF)
        return pid;
    DWORD self = GetCurrentProcessId(), parent = 0;
    HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snap != INVALID_HANDLE_VALUE) {
        PROCESSENTRY32W pe = {};
        pe.dwSize = sizeof(pe);
        for (BOOL ok = Process32FirstW(snap, &pe); ok; ok = Process32NextW(snap, &pe)) {
            if (pe.th32ProcessID == self) {
                parent = pe.th32ParentProcessID;
                break;
            }
        }
        CloseHandle(snap);
    }
    // A PID is reused once its process exits; a process started after this one isn't its launcher.
    pid = 0;
    if (HANDLE proc = parent ? OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, parent) : nullptr) {
        FILETIME parentStart, selfStart, unused1, unused2, unused3;
        if (GetProcessTimes(proc, &parentStart, &unused1, &unused2, &unused3) &&
            GetProcessTimes(GetCurrentProcess(), &selfStart, &unused1, &unused2, &unused3) &&
            CompareFileTime(&parentStart, &selfStart) < 0)
            pid = parent;
        CloseHandle(proc);
    }
    if (pid)
        ExeNameOfPid(pid, g_launcherName, MAX_PATH);
    cached = pid;
    return pid;
}

/// The window to center on: the owner, else the program's active or topmost window, else the
/// foreground window of another instance of the same program (Explorer runs folder windows in
/// several processes), else, if enabled, the foreground window of the program that started this one.
/// Null if none qualifies.
HWND FindParent(HWND hwnd) {
    HWND owner = GetWindow(hwnd, GW_OWNER);
    if (IsValidParent(owner, hwnd))
        return owner;

    // ApplicationFrameHost.exe hosts the windows of unrelated UWP apps; another one of its windows
    // is never what the user launched this one from.
    if (_wcsicmp(g_processName, L"ApplicationFrameHost.exe") == 0)
        return nullptr;

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

    // A launched program takes the foreground before its first window is shown, so the window the user
    // acted on is the one in the foreground when this process started. Explorer can run folder windows in
    // a different process from the one that launched the program, so any window of the launcher's executable counts.
    if (g_centerOnLauncher && g_mode == MODE_PARENT && IsValidParent(g_launchForeground, hwnd)) {
        DWORD launcher = LauncherPid();
        DWORD startFgPid = 0;
        GetWindowThreadProcessId(g_launchForeground, &startFgPid);
        wchar_t startFgCls[256] = L"", startFgName[MAX_PATH];
        bool startFgIsShell =
            GetClassNameW(g_launchForeground, startFgCls, ARRAYSIZE(startFgCls)) && IsIgnoredClass(startFgCls);
        ExeNameOfPid(startFgPid, startFgName, MAX_PATH);
        if (launcher && startFgPid != GetCurrentProcessId() && !startFgIsShell &&
            (startFgPid == launcher || (*startFgName && _wcsicmp(startFgName, g_launcherName) == 0)))
            return g_launchForeground;
    }
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
        return CursorMonitorCenter();
    if (g_mode == MODE_MONITOR)
        return WorkAreaCenter(MonitorFromWindow(parent, MONITOR_DEFAULTTONEAREST));
    RECT rc;
    GetWindowRect(parent, &rc);
    return {(rc.left + rc.right) / 2, (rc.top + rc.bottom) / 2};
}

/// Top-left that centers a w×h window on c, kept inside the work area of c's monitor. False if the
/// window is over the size threshold and isn't a standard dialog.
bool PlaceAround(HWND hwnd, POINT c, int w, int h, int* x, int* y) {
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

/// Computes the centered position for a window about to take the proposed rect, and the point it is
/// centered on. False if the window should keep its position.
bool ComputeTarget(HWND hwnd, const RECT& proposed, int* x, int* y, POINT* center = nullptr) {
    POINT c = ReferencePoint(hwnd);
    if (!PlaceAround(hwnd, c, proposed.right - proposed.left, proposed.bottom - proposed.top, x, y))
        return false;
    if (center)
        *center = c;
    return true;
}

// ───────────── Programs that move their window back ─────────────
//
// Some programs (Firefox restoring its session, for one) move a window again shortly after it is shown,
// undoing the centering. For kKeepCenteredMs after centering, moves made by the window's own process
// are redirected to the centered point. Other programs' moves are left alone, and so is the user's
// dragging, which happens inside the kernel and doesn't reach the SetWindowPos hook.

constexpr wchar_t kCenterXProp[] = L"WindowCentralizer.CenterX";
constexpr wchar_t kCenterYProp[] = L"WindowCentralizer.CenterY";
constexpr wchar_t kCenterTickProp[] = L"WindowCentralizer.CenterTick";
constexpr DWORD kKeepCenteredMs = 1500;

void RememberCenter(HWND hwnd, POINT c) {
    SetPropW(hwnd, kCenterXProp, (HANDLE)(INT_PTR)c.x);
    SetPropW(hwnd, kCenterYProp, (HANDLE)(INT_PTR)c.y);
    SetPropW(hwnd, kCenterTickProp, (HANDLE)(ULONG_PTR)(GetTickCount() | 1));  // never 0, which means unset
}

void ForgetCenter(HWND hwnd) {
    RemovePropW(hwnd, kCenterXProp);
    RemovePropW(hwnd, kCenterYProp);
    RemovePropW(hwnd, kCenterTickProp);
}

/// The point the window was centered on, if that was less than kKeepCenteredMs ago.
bool RecentCenter(HWND hwnd, POINT* c) {
    DWORD tick = (DWORD)(ULONG_PTR)GetPropW(hwnd, kCenterTickProp);
    if (!tick)
        return false;
    if (GetTickCount() - tick >= kKeepCenteredMs) {
        ForgetCenter(hwnd);
        return false;
    }
    c->x = (LONG)(INT_PTR)GetPropW(hwnd, kCenterXProp);
    c->y = (LONG)(INT_PTR)GetPropW(hwnd, kCenterYProp);
    return true;
}

/// Centers the window if it is a candidate under the size threshold. True if it was centered.
bool CenterNow(HWND hwnd) {
    if (!IsCandidate(hwnd))
        return false;
    RECT rc;
    GetWindowRect(hwnd, &rc);
    int x, y;
    POINT c;
    if (!ComputeTarget(hwnd, rc, &x, &y, &c))
        return false;
    if (x != rc.left || y != rc.top)
        SetWindowPos(hwnd, nullptr, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE);
    RememberCenter(hwnd, c);
    return true;
}

// ───────────── Showing windows already drawn ─────────────
//
// A centered window is cloaked (DWM leaves it off the screen, while it is still shown, active and
// receiving input) until it has painted, so it doesn't flash a blank frame while its contents load.
// It is revealed once it and its children have no pending paint for two checks in a row, after at
// least kRevealMinMs, and at the latest after kRevealMaxMs.

constexpr DWORD kRevealMinMs = 80;
constexpr DWORD kRevealMaxMs = 500;
constexpr DWORD kRevealCheckMs = 15;

struct PendingReveal {
    HWND hwnd;
    DWORD start;
    int quietChecks;
};

// All guarded by g_revealLock. The timer runs on a thread pool thread, so no callback is left on
// application threads when the mod unloads.
SRWLOCK g_revealLock = SRWLOCK_INIT;
PendingReveal g_pendingReveal[16];
int g_pendingRevealCount;
PTP_TIMER g_revealTimer;

bool SetCloak(HWND hwnd, BOOL cloak) {
    return SUCCEEDED(DwmSetWindowAttribute(hwnd, DWMWA_CLOAK, &cloak, sizeof(cloak)));
}

// Whoever cloaked a window first (the app itself, or a mod such as Mobile Open Animation) uncloaks it;
// this mod only ever lifts its own cloak.
bool IsCloaked(HWND hwnd) {
    DWORD cloaked = 0;
    return SUCCEEDED(DwmGetWindowAttribute(hwnd, DWMWA_CLOAKED, &cloaked, sizeof(cloaked))) && cloaked;
}

BOOL CALLBACK FindPendingPaint(HWND child, LPARAM found) {
    if (IsWindowVisible(child) && GetUpdateRect(child, nullptr, FALSE)) {
        *(bool*)found = true;
        return FALSE;
    }
    return TRUE;
}

bool HasPendingPaint(HWND hwnd) {
    if (GetUpdateRect(hwnd, nullptr, FALSE))
        return true;
    bool found = false;
    EnumChildWindows(hwnd, FindPendingPaint, (LPARAM)&found);
    return found;
}

// Uncloaking happens outside g_revealLock: it fires EVENT_OBJECT_UNCLOAKED, and an in-context WinEvent
// hook that shows a window from it would reenter HideUntilDrawn on the same thread and deadlock.
void CALLBACK RevealCheck(PTP_CALLBACK_INSTANCE, PVOID, PTP_TIMER timer) {
    HWND reveal[ARRAYSIZE(g_pendingReveal)];
    int revealCount = 0;
    AcquireSRWLockExclusive(&g_revealLock);
    DWORD now = GetTickCount();
    int kept = 0;
    for (int i = 0; i < g_pendingRevealCount; i++) {
        PendingReveal p = g_pendingReveal[i];
        if (!IsWindow(p.hwnd))
            continue;
        DWORD elapsed = now - p.start;
        p.quietChecks = IsWindowVisible(p.hwnd) && !HasPendingPaint(p.hwnd) ? p.quietChecks + 1 : 0;
        if ((elapsed >= kRevealMinMs && p.quietChecks >= 2) || elapsed >= kRevealMaxMs) {
            reveal[revealCount++] = p.hwnd;
            continue;
        }
        g_pendingReveal[kept++] = p;
    }
    g_pendingRevealCount = kept;
    if (!kept)
        SetThreadpoolTimer(timer, nullptr, 0, 0);
    ReleaseSRWLockExclusive(&g_revealLock);
    for (int i = 0; i < revealCount; i++)
        SetCloak(reveal[i], FALSE);
}

/// Cloaks a window that was just centered, until RevealCheck finds it drawn.
void HideUntilDrawn(HWND hwnd) {
    if (!g_revealWhenDrawn)
        return;
    AcquireSRWLockExclusive(&g_revealLock);
    bool listed = false;
    for (int i = 0; i < g_pendingRevealCount; i++)
        listed = listed || g_pendingReveal[i].hwnd == hwnd;
    if (!listed && g_revealTimer && g_pendingRevealCount < (int)ARRAYSIZE(g_pendingReveal) && !IsCloaked(hwnd) &&
        SetCloak(hwnd, TRUE)) {
        g_pendingReveal[g_pendingRevealCount++] = {hwnd, GetTickCount(), 0};
        if (g_pendingRevealCount == 1) {
            ULARGE_INTEGER due;
            due.QuadPart = (ULONGLONG)(-(LONGLONG)kRevealCheckMs * 10000);  // negative: relative time
            FILETIME ft = {due.LowPart, due.HighPart};
            SetThreadpoolTimer(g_revealTimer, &ft, kRevealCheckMs, 0);
        }
    }
    ReleaseSRWLockExclusive(&g_revealLock);
}

void StartRevealing() {
    g_revealTimer = CreateThreadpoolTimer(RevealCheck, nullptr, nullptr);
}

/// Stops the timer and reveals every window still cloaked.
void StopRevealing() {
    AcquireSRWLockExclusive(&g_revealLock);
    PTP_TIMER timer = g_revealTimer;
    g_revealTimer = nullptr;
    ReleaseSRWLockExclusive(&g_revealLock);
    if (timer) {
        SetThreadpoolTimer(timer, nullptr, 0, 0);
        WaitForThreadpoolTimerCallbacks(timer, TRUE);
        CloseThreadpoolTimer(timer);
    }
    HWND reveal[ARRAYSIZE(g_pendingReveal)];
    AcquireSRWLockExclusive(&g_revealLock);
    int revealCount = g_pendingRevealCount;
    for (int i = 0; i < revealCount; i++)
        reveal[i] = g_pendingReveal[i].hwnd;
    g_pendingRevealCount = 0;
    ReleaseSRWLockExclusive(&g_revealLock);
    for (int i = 0; i < revealCount; i++)
        if (IsWindow(reveal[i]))
            SetCloak(reveal[i], FALSE);
}

bool IsUwpFrame(HWND hwnd) {
    wchar_t cls[32];
    return GetClassNameW(hwnd, cls, ARRAYSIZE(cls)) && wcscmp(cls, L"ApplicationFrameWindow") == 0;
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
    if (hwnd && IsShowCommand(cmd) && !IsWindowVisible(hwnd) && CenterNow(hwnd))
        HideUntilDrawn(hwnd);
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
        int px = (flags & SWP_NOMOVE) ? rc.left : x;
        int py = (flags & SWP_NOMOVE) ? rc.top : y;
        RECT proposed = {px, py, px + w, py + h};
        int nx, ny;
        POINT c;
        if (ComputeTarget(hwnd, proposed, &nx, &ny, &c)) {
            x = nx;
            y = ny;
            flags &= ~SWP_NOMOVE;
            RememberCenter(hwnd, c);
            HideUntilDrawn(hwnd);
        }
    } else if (hwnd && !(flags & SWP_NOMOVE) && IsWindowVisible(hwnd) && !IsIconic(hwnd) && !IsZoomed(hwnd)) {
        DWORD pid = 0;
        GetWindowThreadProcessId(hwnd, &pid);
        POINT c;
        if (pid == GetCurrentProcessId() && RecentCenter(hwnd, &c)) {
            RECT rc;
            GetWindowRect(hwnd, &rc);
            int w = (flags & SWP_NOSIZE) ? rc.right - rc.left : cx;
            int h = (flags & SWP_NOSIZE) ? rc.bottom - rc.top : cy;
            int nx, ny;
            if (PlaceAround(hwnd, c, w, h, &nx, &ny)) {
                x = nx;
                y = ny;
            }
        }
    }
    return NtUserSetWindowPos_Original(hwnd, after, x, y, cx, cy, flags);
}

// rcNormalPosition is in workspace coordinates: relative to the work area of the window's monitor,
// which differs from the screen when the taskbar is on the left or top.
POINT WorkspaceOffset(const RECT& rc) {
    MONITORINFO mi = {};
    mi.cbSize = sizeof(mi);
    GetMonitorInfoW(MonitorFromRect(&rc, MONITOR_DEFAULTTONEAREST), &mi);
    return {mi.rcWork.left - mi.rcMonitor.left, mi.rcWork.top - mi.rcMonitor.top};
}

// Programs that restore a saved position (7-Zip's file manager) show their window with
// SetWindowPlacement, which shows it inside the kernel without a show call to hook.
using SetWindowPlacement_t = decltype(&SetWindowPlacement);
SetWindowPlacement_t SetWindowPlacement_Original;
BOOL WINAPI SetWindowPlacement_Hook(HWND hwnd, const WINDOWPLACEMENT* wp) {
    if (hwnd && wp && IsShowCommand(wp->showCmd) && !IsWindowVisible(hwnd) && IsCandidate(hwnd)) {
        WINDOWPLACEMENT centered = *wp;
        RECT& rc = centered.rcNormalPosition;
        int w = rc.right - rc.left, h = rc.bottom - rc.top;
        POINT off = WorkspaceOffset(rc);
        RECT proposed = {rc.left + off.x, rc.top + off.y, rc.left + off.x + w, rc.top + off.y + h};
        int x, y;
        POINT c;
        if (ComputeTarget(hwnd, proposed, &x, &y, &c)) {
            RECT target = {x, y, x + w, y + h};
            POINT back = WorkspaceOffset(target);
            rc = {x - back.x, y - back.y, x - back.x + w, y - back.y + h};
            RememberCenter(hwnd, c);
            HideUntilDrawn(hwnd);
            return SetWindowPlacement_Original(hwnd, &centered);
        }
    }
    return SetWindowPlacement_Original(hwnd, wp);
}

// Windows created with WS_VISIBLE are shown inside the kernel, without any show call to hook;
// they are moved right after creation, before their first paint.
using CreateWindowExW_t = decltype(&CreateWindowExW);
CreateWindowExW_t CreateWindowExW_Original;
HWND WINAPI CreateWindowExW_Hook(DWORD exStyle, LPCWSTR cls, LPCWSTR name, DWORD style, int x, int y, int w, int h,
                                 HWND parent, HMENU menu, HINSTANCE inst, LPVOID param) {
    HWND hwnd = CreateWindowExW_Original(exStyle, cls, name, style, x, y, w, h, parent, menu, inst, param);
    if (hwnd && (style & WS_VISIBLE) && !(style & WS_CHILD) && CenterNow(hwnd))
        HideUntilDrawn(hwnd);
    return hwnd;
}

using CreateWindowExA_t = decltype(&CreateWindowExA);
CreateWindowExA_t CreateWindowExA_Original;
HWND WINAPI CreateWindowExA_Hook(DWORD exStyle, LPCSTR cls, LPCSTR name, DWORD style, int x, int y, int w, int h,
                                 HWND parent, HMENU menu, HINSTANCE inst, LPVOID param) {
    HWND hwnd = CreateWindowExA_Original(exStyle, cls, name, style, x, y, w, h, parent, menu, inst, param);
    if (hwnd && (style & WS_VISIBLE) && !(style & WS_CHILD) && CenterNow(hwnd))
        HideUntilDrawn(hwnd);
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
    ReadRules(g_hostList, kIgnoredChars);
}

void HostSaveList() {
    WriteRules(g_hostList);
}

/// Converts a list stored by 1.0–1.1 in the single "rules" value to one value per rule.
void ConvertLegacyRules() {
    if (Wh_GetIntValue(L"ruleCount", -1) >= 0)
        return;
    HostLoadList();
    HostSaveList();
    Wh_DeleteValue(kLegacyRulesKey);
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

// ── UWP app windows ──
//
// Every UWP app (Calculator, Clock…) lives in an ApplicationFrameWindow of ApplicationFrameHost.exe, so
// process and class can't tell the apps apart. The app's own process, which owns the CoreWindow inside
// the frame, can, but that window only exists once the app has started, after the frame is shown. So
// UWP frames are centered from this process, the first time they get focus, and may move visibly.

HWINEVENTHOOK g_foregroundHook;
HWND g_uwpSeen[64];  // frames already handled: centered once, or open before the tool started
struct UwpRetry {
    HWND frame;
    int tries;
};
UwpRetry g_uwpRetries[8];
enum { TIMER_UWP_RETRY = 2 };

struct CoreWindowSearch {
    HWND found;
};

BOOL CALLBACK FindCoreWindow(HWND hwnd, LPARAM param) {
    wchar_t cls[64];
    if (GetClassNameW(hwnd, cls, ARRAYSIZE(cls)) && wcscmp(cls, L"Windows.UI.Core.CoreWindow") == 0) {
        ((CoreWindowSearch*)param)->found = hwnd;
        return FALSE;
    }
    return TRUE;
}

/// File name of the app hosted in a UWP frame, such as CalculatorApp.exe. False if the app hasn't attached yet.
bool UwpAppProcess(HWND frame, wchar_t* name, DWORD chars) {
    CoreWindowSearch search = {nullptr};
    EnumChildWindows(frame, FindCoreWindow, (LPARAM)&search);
    if (!search.found)
        return false;
    ProcessNameOf(search.found, name, chars);
    return name[0] != L'\0';
}

bool UwpSeen(HWND frame) {
    for (HWND h : g_uwpSeen)
        if (h == frame)
            return true;
    return false;
}

void MarkUwpSeen(HWND frame) {
    static int next;
    for (HWND& h : g_uwpSeen) {
        if (!h || !IsWindow(h)) {
            h = frame;
            return;
        }
    }
    g_uwpSeen[next++ % ARRAYSIZE(g_uwpSeen)] = frame;
}

/// Centers a UWP frame once. False if it isn't ready yet (placeholder size, or the app not attached).
bool TryCenterUwpFrame(HWND frame) {
    if (!IsWindow(frame))
        return true;
    if (IsIconic(frame) || IsZoomed(frame)) {
        MarkUwpSeen(frame);
        return true;
    }
    RECT rc;
    GetWindowRect(frame, &rc);
    int w = rc.right - rc.left, h = rc.bottom - rc.top;
    // The frame is shown at a placeholder size (about 178×39) before it gets the app's.
    if (h < 100)
        return false;
    wchar_t app[MAX_PATH];
    if (!UwpAppProcess(frame, app, ARRAYSIZE(app)))
        return false;
    MarkUwpSeen(frame);
    if (IsIgnoredAs(app, frame, L"ApplicationFrameWindow"))
        return true;
    POINT c;
    if (g_mode == MODE_MOUSE)
        GetCursorPos(&c);
    else
        c = CursorMonitorCenter();
    int x, y;
    if (PlaceAround(frame, c, w, h, &x, &y) && (x != rc.left || y != rc.top))
        SetWindowPos(frame, nullptr, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE);
    return true;
}

void QueueUwpRetry(HWND frame) {
    UwpRetry* slot = nullptr;
    for (UwpRetry& r : g_uwpRetries) {
        if (r.frame == frame)
            return;
        if (!r.frame && !slot)
            slot = &r;
    }
    if (!slot)
        return;
    *slot = {frame, 0};
    SetTimer(g_hwndHost, TIMER_UWP_RETRY, 100, nullptr);
}

void RunUwpRetries() {
    bool pending = false;
    for (UwpRetry& r : g_uwpRetries) {
        if (!r.frame)
            continue;
        if (TryCenterUwpFrame(r.frame) || ++r.tries >= 20)
            r = {};
        else
            pending = true;
    }
    if (!pending)
        KillTimer(g_hwndHost, TIMER_UWP_RETRY);
}

void CALLBACK ForegroundProc(HWINEVENTHOOK, DWORD, HWND hwnd, LONG idObject, LONG, DWORD, DWORD) {
    if (idObject != OBJID_WINDOW || !hwnd || !IsUwpFrame(hwnd) || UwpSeen(hwnd))
        return;
    if (!TryCenterUwpFrame(hwnd))
        QueueUwpRetry(hwnd);
}

BOOL CALLBACK MarkOpenUwpFrame(HWND hwnd, LPARAM) {
    if (IsUwpFrame(hwnd))
        MarkUwpSeen(hwnd);
    return TRUE;
}

void StartUwpCentering() {
    // Apps already open when the tool starts keep their place.
    EnumWindows(MarkOpenUwpFrame, 0);
    g_foregroundHook = SetWinEventHook(EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND, nullptr, ForegroundProc, 0, 0,
                                       WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS);
}

void StopUwpCentering() {
    if (g_foregroundHook)
        UnhookWinEvent(g_foregroundHook);
    g_foregroundHook = nullptr;
}

// ── Notification ──

void ShowToast(HWND target, const wchar_t* title, const wchar_t* detail);

void ToggleIgnoreActive() {
    HWND hwnd = GetForegroundWindow();
    if (!hwnd || hwnd == g_hwndList || hwnd == g_hwndToast)
        return;
    wchar_t process[MAX_PATH], cls[256], title[512];
    ProcessNameOf(hwnd, process, ARRAYSIZE(process));
    GetClassNameW(hwnd, cls, ARRAYSIZE(cls));
    // A UWP frame's rule names the hosted app, so each app can be ignored on its own.
    if (IsUwpFrame(hwnd))
        UwpAppProcess(hwnd, process, ARRAYSIZE(process));
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
        wchar_t rule[kRuleChars];
        swprintf_s(rule, L"%s|%s|%s|%s", process, cls, dialog ? SignatureOf(key) : L"", dialog ? title : L"");
        if (!AddRule(rule))
            return;
    } else {
        RemoveRuleAt(index);
    }
    HostSaveList();

    wchar_t detail[512];
    swprintf_s(detail, L"%s [%s]", process, cls);
    ShowToast(hwnd, nowIgnored ? L"Ignored by Window Centralizer" : L"Centered by Window Centralizer", detail);
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

/// Shows the notification just below the title bar of target, centered on it; at the cursor if target is gone.
void ShowToast(HWND target, const wchar_t* title, const wchar_t* detail) {
    wcsncpy_s(g_toastTitle, title, _TRUNCATE);
    wcsncpy_s(g_toastDetail, detail, _TRUNCATE);
    if (g_hwndToast)
        DestroyWindow(g_hwndToast);
    RECT rc;
    bool onWindow = target && IsWindow(target) && !IsIconic(target) && GetWindowRect(target, &rc);
    POINT anchor;
    if (onWindow)
        anchor = {(rc.left + rc.right) / 2, rc.top};
    else
        GetCursorPos(&anchor);
    MONITORINFO mi = {};
    mi.cbSize = sizeof(mi);
    GetMonitorInfoW(MonitorFromPoint(anchor, MONITOR_DEFAULTTONEAREST), &mi);
    // Created at the anchor first, so GetDpiForWindow reports the DPI of the monitor it will appear on.
    g_hwndToast = CreateWindowExW(WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE | WS_EX_TRANSPARENT, kToastClass,
                                  L"", WS_POPUP, anchor.x, anchor.y, 1, 1, nullptr, nullptr, ModInstance(), nullptr);
    if (!g_hwndToast)
        return;
    UINT dpi = GetDpiForWindow(g_hwndToast);
    int w = MulDiv(372, (int)dpi, 96), h = MulDiv(65, (int)dpi, 96), off = MulDiv(16, (int)dpi, 96);
    int x, y;
    if (onWindow) {
        int titleBar = GetSystemMetricsForDpi(SM_CYCAPTION, dpi) + GetSystemMetricsForDpi(SM_CYFRAME, dpi) +
                       GetSystemMetricsForDpi(SM_CXPADDEDBORDER, dpi);
        x = anchor.x - w / 2;
        y = anchor.y + titleBar + off / 2;
    } else {
        x = anchor.x + off;
        y = anchor.y + off;
    }
    x = x + w > mi.rcWork.right ? mi.rcWork.right - w : x;
    y = y + h > mi.rcWork.bottom ? mi.rcWork.bottom - h : y;
    x = x < mi.rcWork.left ? mi.rcWork.left : x;
    y = y < mi.rcWork.top ? mi.rcWork.top : y;
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
    LoadCenteringSettings();  // the mode and size threshold drive UWP centering here
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
        case WM_TIMER:
            if (wParam == TIMER_UWP_RETRY)
                RunUwpRetries();
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
            ConvertLegacyRules();
            ApplyHostSettings(false);
            StartUwpCentering();
            RunHostLoop();
            StopUwpCentering();
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

    g_launchForeground = GetForegroundWindow();
    LoadCenteringSettings();
    StartRevealing();
    if (HMODULE win32u = GetModuleHandleW(L"win32u.dll")) {
        if (auto p = (NtUserShowWindow_t)GetProcAddress(win32u, "NtUserShowWindow"))
            WindhawkUtils::SetFunctionHook(p, NtUserShowWindow_Hook, &NtUserShowWindow_Original);
        if (auto p = (NtUserSetWindowPos_t)GetProcAddress(win32u, "NtUserSetWindowPos"))
            WindhawkUtils::SetFunctionHook(p, NtUserSetWindowPos_Hook, &NtUserSetWindowPos_Original);
    }
    WindhawkUtils::SetFunctionHook(CreateWindowExW, CreateWindowExW_Hook, &CreateWindowExW_Original);
    WindhawkUtils::SetFunctionHook(CreateWindowExA, CreateWindowExA_Hook, &CreateWindowExA_Original);
    WindhawkUtils::SetFunctionHook(SetWindowPlacement, SetWindowPlacement_Hook, &SetWindowPlacement_Original);
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
    StopRevealing();
    if (g_ignore.text)
        HeapFree(GetProcessHeap(), 0, g_ignore.text);
    g_ignore.text = nullptr;
}
