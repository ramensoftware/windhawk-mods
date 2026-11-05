// ==WindhawkMod==
// @id              click-wallpaper-reveal-desktop
// @name            Click Wallpaper to Reveal Desktop
// @description     macOS-style: click the wallpaper to sweep your windows aside, click again to bring back only those windows
// @version         1.0.0
// @author          tetsu0s
// @github          https://github.com/tetsudaux
// @include         explorer.exe
// @license         MIT
// @compilerOptions -lcomctl32 -lole32 -luuid -ldwmapi -ladvapi32 -lshlwapi -luser32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Click Wallpaper to Reveal Desktop

**Your desktop, one click away. Your windows, exactly as you left them.**

![Click the wallpaper to reveal the desktop, click again to bring the windows back](https://raw.githubusercontent.com/tetsudaux/windhawk-mod-assets/main/click-wallpaper-reveal-desktop/demo.gif)

Click any empty spot on your wallpaper and every open window on the current
virtual desktop steps aside. Grab the file you need, drop something on the
desktop, then click the wallpaper again: **only the windows that were moved
come back**, in their original stacking order, maximized windows still
maximized, and focus right where it was.

The windows you minimized yourself are never touched. Inspired by the "Click
wallpaper to reveal desktop" behavior of macOS, built the Windows way.

## Why you'll like it

- **It remembers what it moved.** Restoring brings back exactly that set of
  windows, never everything you have minimized.
- **One session per virtual desktop.** Reveal the desktop on Desktop 1, switch
  to Desktop 2, come back later: your windows are still waiting.
- **You stay in control.** Restore one window yourself from the taskbar and it
  simply leaves the session. Open something new while the desktop is revealed
  and the next click sweeps it aside too, then brings everything back together.
- **A hotkey too.** `Win+Shift+D` by default. Map it to a touchpad gesture, a
  mouse button, a hot corner, or a click on the taskbar.
- **Clean-desktop mode.** Desktop icons show up only when the desktop is
  clear, and stay out of the way while you work. Turn it off in the settings.
- **Light on resources.** No polling loops: the mod reacts to clicks and
  system events, and sits idle the rest of the time.
- **Nothing gets lost.** Swept-aside windows are regular minimized windows, so
  they are always in the taskbar, even if you disable the mod.

## How to use it

| You do | What happens |
|---|---|
| Click empty wallpaper with windows open | They step aside and the desktop is yours |
| Click empty wallpaper again | Only those windows come back |
| Click empty wallpaper when the mod moved nothing | Nothing. Your own minimized windows stay put |
| Press the hotkey (`Win+Shift+D`) | Same as clicking the wallpaper, from anywhere |

Clicks on icons, drags (selection rectangles), double-clicks and the click
that just closes the Start menu or Search are ignored.

## Make it feel native

- **Touchpad:** Settings > Bluetooth & devices > Touchpad > Advanced gestures,
  then assign `Win+Shift+D` as a custom shortcut, for example to a three-finger
  swipe down.
- **Taskbar:** with the
  [Click on empty taskbar space](https://windhawk.net/mods/taskbar-empty-space-clicks)
  mod, use the "Virtual key press" action with `0x5B;0x10;0x44` to trigger the
  same reveal from the taskbar.

## Settings

- **Reveal on wallpaper click:** turn the click behavior on or off.
- **Hotkey:** modifiers `alt`, `ctrl`, `shift`, `win` joined with `+` and one
  key (`A-Z`, `0-9`, `F1-F24`). Leave it empty to disable it. If another app
  already owns the combination, the mod log says so.
- **Desktop icons:**
  - *Clean desktop* (default): icons are visible only when no window is open,
    as long as "Show desktop icons" is on in Windows.
  - *Clean desktop, always*: same, even if "Show desktop icons" is off.
  - *Don't change*: Windows handles the icons as usual.
- **Excluded programs:** executables that are never swept aside and don't
  count as open windows, for example launchers or widgets. Separate them with
  `;`, wildcards `*` and `?` work.

## Good to know

- Works on Windows 10 and Windows 11. Tested on Windows 11 build 26220.
- Only windows on the current virtual desktop are affected.
- If an app keeps coming back on its own, the mod log names it, so you can add
  it to *Excluded programs*.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- revealOnClick: true
  $name: Reveal on wallpaper click
  $description: Click empty wallpaper to sweep the open windows aside; click again to bring back only those windows.
- revealHotkey: win+shift+D
  $name: Hotkey
  $description: Modifiers alt, ctrl, shift, win joined with + and one key (A-Z, 0-9, F1-F24). Leave empty to disable.
- iconsMode: respectWindows
  $name: Desktop icons
  $options:
  - respectWindows: Clean desktop (icons only when no window is open, if "Show desktop icons" is on)
  - auto: Clean desktop, always (even if "Show desktop icons" is off)
  - off: Don't change
- excludeExe: microsoft.cmdpal.ui.exe;powertoys.powerlauncher.exe
  $name: Excluded programs
  $description: Executables that are never swept aside and don't count as open windows. Separate with ; (wildcards * and ? work).
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <dwmapi.h>
#include <shlwapi.h>
#include <shobjidl.h>
#include <strsafe.h>
#include <windhawk_utils.h>

#include <algorithm>
#include <atomic>
#include <string>
#include <unordered_map>
#include <vector>

#ifndef EVENT_OBJECT_CLOAKED
#define EVENT_OBJECT_CLOAKED   0x8017
#define EVENT_OBJECT_UNCLOAKED 0x8018
#endif
#ifndef MOD_NOREPEAT
#define MOD_NOREPEAT 0x4000
#endif

// ---------------------------------------------------------------------------
// Constants
// ---------------------------------------------------------------------------

static const UINT WM_APP_DESKTOP_CLICK = WM_APP + 1;
static const UINT WM_APP_STOP          = WM_APP + 2;

static const UINT_PTR TIMER_RECOMPUTE = 1;
static const UINT_PTR TIMER_WATCHDOG  = 2;
static const UINT_PTR TIMER_FINALIZE  = 3;
static const int      HOTKEY_REVEAL   = 1;

static const UINT      RECOMPUTE_DELAY_MS       = 60;    // coalesces bursts of events
static const UINT      WATCHDOG_MS              = 3000;  // finds the desktop again if Explorer recreates it
static const ULONGLONG FLYOUT_DISMISS_WINDOW_MS = 400;   // a click that only closed Start/Search
static const UINT      RESPONSIVE_PROBE_MS      = 150;
static const UINT      MAX_DEBOUNCE_MS          = 500;
static const ULONGLONG SELF_RESTORE_HINT_MS     = 1500;  // "came back on its own" threshold
static const ULONGLONG EXE_CACHE_TTL_MS         = 10000;
static const UINT      FINALIZE_TICK_MS         = 100;   // waits for slow restores...
static const int       FINALIZE_MAX_TICKS       = 15;    // ...for up to 1.5 s

#define WORKER_CLASS     L"ClickWallpaperRevealDesktop_Worker"
#define REG_EXPLORER_ADV L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced"

enum : DWORD_PTR { KIND_LISTVIEW = 1, KIND_DEFVIEW = 2 };
enum IconsMode { ICONS_OFF, ICONS_RESPECT_WINDOWS, ICONS_AUTO };

// ---------------------------------------------------------------------------
// Global state
// ---------------------------------------------------------------------------

struct {
    bool revealOnClick;
    UINT hotkeyMods;
    UINT hotkeyVk;
    IconsMode iconsMode;
    std::vector<std::wstring> excludeExe;
} g_settings;

static int g_moduleAnchor;  // its address identifies this DLL
static HMODULE g_hModule = nullptr;

static HANDLE g_hWorker = nullptr;
static HANDLE g_hWorkerReady = nullptr;
static std::atomic<HWND> g_workerWnd{nullptr};
static std::atomic<bool> g_stopping{false};

static std::atomic<HWND> g_defView{nullptr};
static std::atomic<HWND> g_listView{nullptr};

// A "session" is the set of windows this mod swept aside on one virtual desktop.
struct RevealEntry {
    HWND hwnd;
    DWORD pid;            // guards against HWND reuse
    ULONGLONG apartedAt;  // for the "came back on its own" diagnostic
};
struct RevealSession {
    std::vector<RevealEntry> windows;  // z-order: top to bottom
    HWND focus = nullptr;
};

struct ExeCacheEntry {
    std::wstring name;
    ULONGLONG at;
};

// Worker thread state (only the worker thread touches it).
static struct {
    IVirtualDesktopManager* vdm = nullptr;
    std::vector<HWINEVENTHOOK> eventHooks;
    bool hotkeyRegistered = false;

    bool iconsHidden = false;
    bool userHidesIcons = false;
    bool userHidesIconsKnown = false;
    bool taskViewActive = false;
    bool recomputePending = false;

    HWND lastAppForeground = nullptr;
    bool lastForegroundWasFlyout = false;
    ULONGLONG flyoutDismissedAt = 0;
    ULONGLONG lastToggleAt = 0;

    std::vector<RevealSession> sessions;
    std::unordered_map<DWORD, ExeCacheEntry> exeCache;

    // A restore in progress: z-order (top to bottom) and focus to re-apply once
    // every window is back, since some apps process the restore late.
    std::vector<HWND> pendingOrder;
    HWND pendingFocus = nullptr;
    int finalizeTicks = 0;
} W;

// Desktop thread state (only the subclass procedure touches it).
static bool g_clickArmed = false;
static POINT g_clickDownPt = {};

// ---------------------------------------------------------------------------
// Settings
// ---------------------------------------------------------------------------

static std::wstring Trim(const std::wstring& s) {
    size_t a = s.find_first_not_of(L" \t");
    if (a == std::wstring::npos) return L"";
    size_t b = s.find_last_not_of(L" \t");
    return s.substr(a, b - a + 1);
}

static std::wstring Lower(std::wstring s) {
    for (auto& c : s) c = (wchar_t)towlower(c);
    return s;
}

static std::vector<std::wstring> Split(const std::wstring& s, wchar_t sep) {
    std::vector<std::wstring> out;
    size_t start = 0;
    for (;;) {
        size_t pos = s.find(sep, start);
        out.push_back(s.substr(start, pos == std::wstring::npos ? std::wstring::npos : pos - start));
        if (pos == std::wstring::npos) break;
        start = pos + 1;
    }
    return out;
}

// "win+shift+D" -> MOD_WIN | MOD_SHIFT, 'D'.
static bool ParseHotkey(const std::wstring& text, UINT* mods, UINT* vk) {
    *mods = 0;
    *vk = 0;
    std::wstring s = Lower(Trim(text));
    if (s.empty()) return false;
    std::vector<std::wstring> parts = Split(s, L'+');
    for (size_t i = 0; i + 1 < parts.size(); i++) {
        std::wstring p = Trim(parts[i]);
        if (p == L"alt") *mods |= MOD_ALT;
        else if (p == L"ctrl" || p == L"control") *mods |= MOD_CONTROL;
        else if (p == L"shift") *mods |= MOD_SHIFT;
        else if (p == L"win") *mods |= MOD_WIN;
        else return false;
    }
    std::wstring key = Trim(parts.back());
    if (key.size() == 1 && key[0] >= L'a' && key[0] <= L'z') {
        *vk = 'A' + (key[0] - L'a');
    } else if (key.size() == 1 && key[0] >= L'0' && key[0] <= L'9') {
        *vk = key[0];
    } else if (key.size() >= 2 && key.size() <= 3 && key[0] == L'f') {
        int n = _wtoi(key.c_str() + 1);
        if (n < 1 || n > 24) return false;
        *vk = VK_F1 + (n - 1);
    } else {
        return false;
    }
    return true;
}

static void LoadSettings() {
    g_settings.revealOnClick = Wh_GetIntSetting(L"revealOnClick") != 0;

    PCWSTR hk = Wh_GetStringSetting(L"revealHotkey");
    std::wstring hotkey = hk ? hk : L"";
    Wh_FreeStringSetting(hk);
    if (!ParseHotkey(hotkey, &g_settings.hotkeyMods, &g_settings.hotkeyVk) &&
        !Trim(hotkey).empty()) {
        Wh_Log(L"[hotkey] invalid hotkey: '%s'", hotkey.c_str());
    }

    PCWSTR mode = Wh_GetStringSetting(L"iconsMode");
    std::wstring m = mode ? mode : L"";
    Wh_FreeStringSetting(mode);
    g_settings.iconsMode = m == L"auto"  ? ICONS_AUTO
                           : m == L"off" ? ICONS_OFF
                                         : ICONS_RESPECT_WINDOWS;

    PCWSTR ex = Wh_GetStringSetting(L"excludeExe");
    std::wstring exclude = ex ? ex : L"";
    Wh_FreeStringSetting(ex);
    g_settings.excludeExe.clear();
    for (auto& p : Split(exclude, L';')) {
        std::wstring t = Lower(Trim(p));
        if (!t.empty()) g_settings.excludeExe.push_back(t);
    }
}

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

static bool UserHidesDesktopIcons() {
    DWORD v = 0, sz = sizeof(v);
    return RegGetValueW(HKEY_CURRENT_USER, REG_EXPLORER_ADV, L"HideIcons", RRF_RT_REG_DWORD,
                        nullptr, &v, &sz) == ERROR_SUCCESS && v != 0;
}

static bool GetClass(HWND h, WCHAR (&cls)[64]) {
    return h && GetClassNameW(h, cls, ARRAYSIZE(cls)) > 0;
}

static bool IsCloaked(HWND h) {
    DWORD cloaked = 0;
    return SUCCEEDED(DwmGetWindowAttribute(h, DWMWA_CLOAKED, &cloaked, sizeof(cloaked))) && cloaked;
}

static DWORD WindowPid(HWND h) {
    DWORD pid = 0;
    GetWindowThreadProcessId(h, &pid);
    return pid;
}

static bool IsOwnProcess(HWND h) {
    return WindowPid(h) == GetCurrentProcessId();
}

static bool IsTopLevel(HWND h) {
    return GetAncestor(h, GA_PARENT) == GetDesktopWindow();
}

static bool IsShellChromeWindow(HWND h) {
    WCHAR cls[64];
    if (!GetClass(h, cls)) return false;
    return !wcscmp(cls, L"Progman") || !wcscmp(cls, L"WorkerW") ||
           !wcscmp(cls, L"Shell_TrayWnd") || !wcscmp(cls, L"Shell_SecondaryTrayWnd");
}

static bool IsDesktopBackground(HWND h) {
    WCHAR cls[64];
    if (!GetClass(h, cls)) return false;
    return !wcscmp(cls, L"Progman") || !wcscmp(cls, L"WorkerW");
}

static bool IsTaskViewWindow(HWND h) {
    WCHAR cls[64];
    if (!GetClass(h, cls)) return false;
    if (!wcscmp(cls, L"MultitaskingViewFrame")) return true;
    // Windows 11: Task View is an untitled XAML island owned by Explorer.
    return !wcscmp(cls, L"XamlExplorerHostIslandWindow") && IsOwnProcess(h) &&
           GetWindowTextLengthW(h) == 0;
}

// Start, Search, notification center, Task View, Alt+Tab.
static bool IsShellFlyout(HWND h) {
    WCHAR cls[64];
    if (!GetClass(h, cls)) return false;
    return !wcscmp(cls, L"Windows.UI.Core.CoreWindow") ||
           !wcscmp(cls, L"ActionCenterWindowClass") ||
           !wcscmp(cls, L"XamlExplorerHostIslandWindow") ||
           !wcscmp(cls, L"MultitaskingViewFrame");
}

// Same rules the taskbar uses to decide what counts as "a window".
static bool IsAppWindow(HWND h) {
    if (!IsWindowVisible(h)) return false;
    LONG_PTR ex = GetWindowLongPtrW(h, GWL_EXSTYLE);
    if (ex & (WS_EX_TOOLWINDOW | WS_EX_TRANSPARENT)) return false;  // palettes, click-through overlays
    if (!(ex & WS_EX_APPWINDOW)) {
        if (GetWindow(h, GW_OWNER)) return false;
        if (ex & WS_EX_NOACTIVATE) return false;
    }
    if (IsShellChromeWindow(h)) return false;
    if (IsCloaked(h)) return false;  // another virtual desktop, or a hidden UWP app
    return true;
}

static bool HasScreenArea(HWND h) {
    RECT r;
    if (!GetWindowRect(h, &r) || r.right - r.left <= 1 || r.bottom - r.top <= 1) return false;
    return MonitorFromRect(&r, MONITOR_DEFAULTTONULL) != nullptr;
}

static bool QueryExeName(DWORD pid, std::wstring* out) {
    HANDLE p = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!p) return false;
    WCHAR path[MAX_PATH];
    DWORD size = ARRAYSIZE(path);
    bool ok = QueryFullProcessImageNameW(p, 0, path, &size) != 0;
    CloseHandle(p);
    if (!ok) return false;
    *out = Lower(PathFindFileNameW(path));
    return true;
}

static std::wstring ExeNameCached(DWORD pid) {
    ULONGLONG now = GetTickCount64();
    auto it = W.exeCache.find(pid);
    if (it != W.exeCache.end() && now - it->second.at < EXE_CACHE_TTL_MS) return it->second.name;
    if (W.exeCache.size() > 256) W.exeCache.clear();
    std::wstring name;
    if (!QueryExeName(pid, &name)) name = L"?";
    W.exeCache[pid] = {name, now};
    return name;
}

static bool IsExcludedWindow(HWND h) {
    if (g_settings.excludeExe.empty()) return false;
    std::wstring exe = ExeNameCached(WindowPid(h));
    for (const auto& pattern : g_settings.excludeExe) {
        if (PathMatchSpecW(exe.c_str(), pattern.c_str())) return true;
    }
    return false;
}

static void DescribeWindow(HWND h, DWORD pid, WCHAR* out, size_t cch) {
    WCHAR cls[64];
    if (!GetClass(h, cls)) StringCchCopyW(cls, ARRAYSIZE(cls), L"?");
    std::wstring exe = ExeNameCached(pid);
    StringCchPrintfW(out, cch, L"%s [%s] hwnd=%p", exe.c_str(), cls, h);
}

static bool IsOnCurrentDesktop(HWND h) {
    if (!W.vdm) return true;
    BOOL on = TRUE;
    return FAILED(W.vdm->IsWindowOnCurrentVirtualDesktop(h, &on)) || on;
}

// Answers within 150 ms? Then it's safe to restore it synchronously.
static bool IsResponsive(HWND h) {
    DWORD_PTR result = 0;
    return SendMessageTimeoutW(h, WM_NULL, 0, 0, SMTO_ABORTIFHUNG | SMTO_BLOCK,
                               RESPONSIVE_PROBE_MS, &result) != 0;
}

// ---------------------------------------------------------------------------
// Visible windows on the current virtual desktop
// ---------------------------------------------------------------------------

struct CollectCtx {
    std::vector<RevealEntry>* out;  // null = only check whether one exists
    bool found;
};

static BOOL CALLBACK CollectVisibleProc(HWND h, LPARAM lp) {
    auto* c = reinterpret_cast<CollectCtx*>(lp);
    // A visible window on another virtual desktop is cloaked, so IsAppWindow
    // already skips it without a COM call.
    if (!IsAppWindow(h) || IsIconic(h) || !HasScreenArea(h)) return TRUE;
    if (IsExcludedWindow(h)) return TRUE;
    c->found = true;
    if (!c->out) return FALSE;
    c->out->push_back({h, WindowPid(h), 0});
    return TRUE;
}

static bool AnyVisibleAppWindow() {
    CollectCtx c{nullptr, false};
    EnumWindows(CollectVisibleProc, reinterpret_cast<LPARAM>(&c));
    return c.found;
}

// ---------------------------------------------------------------------------
// Sessions
// ---------------------------------------------------------------------------

static bool EntryAlive(const RevealEntry& e) {
    return IsWindow(e.hwnd) && WindowPid(e.hwnd) == e.pid;
}

static void DropEmptySessions() {
    W.sessions.erase(std::remove_if(W.sessions.begin(), W.sessions.end(),
                                    [](const RevealSession& s) { return s.windows.empty(); }),
                     W.sessions.end());
}

static const RevealEntry* FindInSessions(HWND h) {
    for (const auto& s : W.sessions)
        for (const auto& e : s.windows)
            if (e.hwnd == h) return &e;
    return nullptr;
}

static void ForgetWindow(HWND h) {
    bool touched = false;
    for (auto& s : W.sessions) {
        auto it = std::remove_if(s.windows.begin(), s.windows.end(),
                                 [h](const RevealEntry& e) { return e.hwnd == h; });
        if (it != s.windows.end()) {
            s.windows.erase(it, s.windows.end());
            touched = true;
        }
    }
    if (touched) DropEmptySessions();
}

static int SessionIndexForCurrentDesktop() {
    for (auto& s : W.sessions) {
        s.windows.erase(std::remove_if(s.windows.begin(), s.windows.end(),
                                       [](const RevealEntry& e) { return !EntryAlive(e); }),
                        s.windows.end());
    }
    DropEmptySessions();
    for (size_t i = 0; i < W.sessions.size(); i++) {
        for (const auto& e : W.sessions[i].windows) {
            if (IsOnCurrentDesktop(e.hwnd)) return (int)i;
        }
    }
    return -1;
}

static void RevealDesktop(std::vector<RevealEntry>& visible) {
    ULONGLONG now = GetTickCount64();
    HWND focus = nullptr;
    WCHAR desc[400];
    for (auto& e : visible) {
        e.apartedAt = now;
        if (e.hwnd == W.lastAppForeground) focus = e.hwnd;
        if (FindInSessions(e.hwnd)) {
            // Still in a session yet visible: it didn't stay minimized.
            DescribeWindow(e.hwnd, e.pid, desc, ARRAYSIZE(desc));
            Wh_Log(L"[diag] did not stay minimized: %s", desc);
        }
        ForgetWindow(e.hwnd);
    }
    if (!focus) focus = visible.front().hwnd;

    int idx = SessionIndexForCurrentDesktop();
    if (idx < 0) {
        W.sessions.emplace_back();
        idx = (int)W.sessions.size() - 1;
    }
    RevealSession& s = W.sessions[idx];
    // Newly swept windows go on top of the ones already in the session.
    s.windows.insert(s.windows.begin(), visible.begin(), visible.end());
    s.focus = focus;

    // Asynchronous and without activation: a hung app can't block us.
    for (const auto& e : visible) {
        ShowWindowAsync(e.hwnd, SW_SHOWMINNOACTIVE);
        DescribeWindow(e.hwnd, e.pid, desc, ARRAYSIZE(desc));
        Wh_Log(L"[reveal]   %s", desc);
    }
    Wh_Log(L"[reveal] swept=%d, in session=%d", (int)visible.size(), (int)s.windows.size());
}

static void RestoreSession(const RevealSession& s) {
    std::vector<HWND> list;
    list.reserve(s.windows.size());
    for (const auto& e : s.windows) {
        // Only what is still minimized and on this virtual desktop.
        if (EntryAlive(e) && IsIconic(e.hwnd) && IsOnCurrentDesktop(e.hwnd)) list.push_back(e.hwnd);
    }

    // Bottom to top: each SW_RESTORE brings its window to the front.
    // SW_RESTORE also keeps maximized windows maximized.
    for (auto it = list.rbegin(); it != list.rend(); ++it) {
        if (IsResponsive(*it)) ShowWindow(*it, SW_RESTORE);
        else ShowWindowAsync(*it, SW_RESTORE);
    }

    // Some apps handle the restore late and end up on top. Re-apply the
    // original z-order and focus once all of them are back.
    W.pendingOrder = list;
    W.pendingFocus = s.focus;
    W.finalizeTicks = 0;
    if (HWND w = g_workerWnd) SetTimer(w, TIMER_FINALIZE, FINALIZE_TICK_MS, nullptr);

    Wh_Log(L"[restore] restored=%d", (int)list.size());
}

static void CancelPendingRestore() {
    if (HWND w = g_workerWnd) KillTimer(w, TIMER_FINALIZE);
    W.pendingOrder.clear();
    W.pendingFocus = nullptr;
}

static void OnFinalizeTick() {
    bool allBack = true;
    for (HWND h : W.pendingOrder) {
        if (IsWindow(h) && IsIconic(h)) allBack = false;
    }
    if (!allBack && ++W.finalizeTicks < FINALIZE_MAX_TICKS) return;

    std::vector<HWND> order;
    for (HWND h : W.pendingOrder) {
        if (IsWindow(h) && !IsIconic(h)) order.push_back(h);
    }
    // Chain each window right below the one that was above it.
    const UINT flags = SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOOWNERZORDER;
    for (size_t i = 1; i < order.size(); i++) {
        UINT f = flags | (IsResponsive(order[i]) ? 0 : SWP_ASYNCWINDOWPOS);
        SetWindowPos(order[i], order[i - 1], 0, 0, 0, 0, f);
    }
    HWND focus = W.pendingFocus;
    if (focus && IsWindow(focus) && !IsIconic(focus)) {
        SetForegroundWindow(focus);
    } else if (!order.empty()) {
        SetForegroundWindow(order.front());
    }
    if (!allBack) Wh_Log(L"[restore] some windows were still minimized after 1.5 s");
    CancelPendingRestore();
}

// ---------------------------------------------------------------------------
// Desktop icons
// ---------------------------------------------------------------------------

static bool IconsManaged() {
    if (g_settings.iconsMode == ICONS_OFF) return false;
    if (g_settings.iconsMode == ICONS_RESPECT_WINDOWS && W.userHidesIcons) return false;
    return true;
}

static void SetIconsHidden(bool hide) {
    HWND lv = g_listView;
    if (!lv || !IsWindow(lv) || !IconsManaged()) return;
    // Compare with the real state: Explorer may show or hide the list on its
    // own (refresh, resolution change, desktop context menu).
    bool hiddenNow = !IsWindowVisible(lv);
    W.iconsHidden = hiddenNow;
    if (hide == hiddenNow) return;
    ShowWindowAsync(lv, hide ? SW_HIDE : SW_SHOWNA);
    W.iconsHidden = hide;
    Wh_Log(L"[icons] %s", hide ? L"hidden" : L"shown");
}

static void RecomputeIcons() {
    if (!IconsManaged() || W.taskViewActive) return;
    SetIconsHidden(AnyVisibleAppWindow());
}

static void ScheduleRecompute() {
    if (g_settings.iconsMode == ICONS_OFF || W.recomputePending) return;
    HWND w = g_workerWnd;
    if (!w) return;
    W.recomputePending = true;
    SetTimer(w, TIMER_RECOMPUTE, RECOMPUTE_DELAY_MS, nullptr);
}

static void RefreshUserIconsSetting() {
    if (g_settings.iconsMode == ICONS_OFF) return;
    bool hides = UserHidesDesktopIcons();
    if (W.userHidesIconsKnown && hides == W.userHidesIcons) return;
    W.userHidesIcons = hides;
    W.userHidesIconsKnown = true;
    if (hides && g_settings.iconsMode == ICONS_RESPECT_WINDOWS) {
        Wh_Log(L"[icons] 'Show desktop icons' is off in Windows; icons are left alone");
    }
    ScheduleRecompute();
}

// ---------------------------------------------------------------------------
// Reveal / restore (wallpaper click or hotkey)
// ---------------------------------------------------------------------------

static void ActivateDesktop() {
    // Like Win+D: focus goes to the desktop, not to a minimized window.
    if (HWND progman = FindWindowW(L"Progman", nullptr)) SetForegroundWindow(progman);
}

static void OnRevealToggle(bool fromHotkey) {
    if (W.taskViewActive) return;
    if (!fromHotkey && !g_settings.revealOnClick) return;

    ULONGLONG now = GetTickCount64();
    UINT debounce = GetDoubleClickTime();
    if (debounce > MAX_DEBOUNCE_MS) debounce = MAX_DEBOUNCE_MS;
    if (now - W.lastToggleAt < debounce) return;
    if (!fromHotkey && now - W.flyoutDismissedAt < FLYOUT_DISMISS_WINDOW_MS) return;  // closed Start
    W.lastToggleAt = now;
    CancelPendingRestore();

    std::vector<RevealEntry> visible;
    CollectCtx c{&visible, false};
    EnumWindows(CollectVisibleProc, reinterpret_cast<LPARAM>(&c));

    if (!visible.empty()) {
        RevealDesktop(visible);
        if (fromHotkey) ActivateDesktop();
    } else {
        int idx = SessionIndexForCurrentDesktop();
        if (idx >= 0) {
            RevealSession s = W.sessions[idx];
            W.sessions.erase(W.sessions.begin() + idx);
            RestoreSession(s);
        }
        // No session: nothing to do. Windows the user minimized stay minimized.
    }
    ScheduleRecompute();
}

static void FinishClick() {
    g_clickArmed = false;
    POINT up;
    GetCursorPos(&up);
    LONG dx = up.x - g_clickDownPt.x, dy = up.y - g_clickDownPt.y;
    if (dx < 0) dx = -dx;
    if (dy < 0) dy = -dy;
    if (dx > GetSystemMetrics(SM_CXDRAG) || dy > GetSystemMetrics(SM_CYDRAG)) return;  // a drag
    HWND w = g_workerWnd;
    if (w) PostMessageW(w, WM_APP_DESKTOP_CLICK, 0, 0);
}

// Runs on Explorer's desktop thread: it only detects the click and notifies.
static LRESULT CALLBACK DesktopSubclassProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp,
                                            DWORD_PTR kind) {
    switch (msg) {
        case WM_LBUTTONDOWN: {
            if (!g_settings.revealOnClick ||
                (wp & (MK_CONTROL | MK_SHIFT | MK_RBUTTON | MK_MBUTTON))) {
                break;
            }
            if (kind == KIND_LISTVIEW) {
                LVHITTESTINFO hti = {};
                hti.pt.x = GET_X_LPARAM(lp);
                hti.pt.y = GET_Y_LPARAM(lp);
                if (ListView_HitTest(hWnd, &hti) >= 0) break;  // clicked an icon
            }
            g_clickArmed = true;
            GetCursorPos(&g_clickDownPt);
            LRESULT r = DefSubclassProc(hWnd, msg, wp, lp);
            // The list view runs its own selection loop and swallows
            // WM_LBUTTONUP; if the button is already up, the click ended here.
            if (g_clickArmed && GetKeyState(VK_LBUTTON) >= 0) FinishClick();
            return r;
        }
        case WM_LBUTTONUP:
            if (g_clickArmed) {
                LRESULT r = DefSubclassProc(hWnd, msg, wp, lp);
                FinishClick();
                return r;
            }
            break;
    }
    return DefSubclassProc(hWnd, msg, wp, lp);
}

static bool FindDesktop(HWND* defView, HWND* listView) {
    HWND dv = nullptr;
    HWND progman = FindWindowW(L"Progman", nullptr);
    if (progman) dv = FindWindowExW(progman, nullptr, L"SHELLDLL_DefView", nullptr);
    if (!dv) {
        HWND worker = nullptr;
        while ((worker = FindWindowExW(nullptr, worker, L"WorkerW", nullptr)) != nullptr) {
            dv = FindWindowExW(worker, nullptr, L"SHELLDLL_DefView", nullptr);
            if (dv) break;
        }
    }
    if (!dv || !IsOwnProcess(dv)) return false;
    HWND lv = FindWindowExW(dv, nullptr, L"SysListView32", nullptr);
    if (!lv) return false;
    *defView = dv;
    *listView = lv;
    return true;
}

static void EnsureDesktopHooked() {
    HWND lv = g_listView;
    if (lv && IsWindow(lv)) return;

    HWND dv = nullptr;
    if (!FindDesktop(&dv, &lv)) return;  // the desktop doesn't exist yet: the watchdog retries

    if (WindhawkUtils::SetWindowSubclassFromAnyThread(dv, DesktopSubclassProc, KIND_DEFVIEW)) {
        g_defView = dv;
    }
    if (WindhawkUtils::SetWindowSubclassFromAnyThread(lv, DesktopSubclassProc, KIND_LISTVIEW)) {
        g_listView = lv;
        W.iconsHidden = !IsWindowVisible(lv);
        ScheduleRecompute();
        Wh_Log(L"[desktop] defView=%p listView=%p", dv, lv);
    }
}

// ---------------------------------------------------------------------------
// System events
// ---------------------------------------------------------------------------

static void OnForeground(HWND h) {
    W.taskViewActive = IsTaskViewWindow(h);
    bool flyout = IsShellFlyout(h);
    if (!flyout && W.lastForegroundWasFlyout && IsDesktopBackground(h)) {
        W.flyoutDismissedAt = GetTickCount64();
    }
    W.lastForegroundWasFlyout = flyout;
    if (h && !flyout && IsAppWindow(h)) W.lastAppForeground = h;
    ScheduleRecompute();
}

static void OnMinimizeEnd(HWND h) {
    if (const RevealEntry* e = FindInSessions(h)) {
        // Our own restores remove the session first, so the user or the app
        // itself brought this window back.
        ULONGLONG elapsed = GetTickCount64() - e->apartedAt;
        WCHAR desc[400];
        DescribeWindow(h, e->pid, desc, ARRAYSIZE(desc));
        if (elapsed < SELF_RESTORE_HINT_MS) {
            Wh_Log(L"[diag] came back on its own after %u ms (exclude it?): %s",
                   (unsigned)elapsed, desc);
        } else {
            Wh_Log(L"[session] restored by the user, leaves the session: %s", desc);
        }
        ForgetWindow(h);
    }
}

static void CALLBACK WinEventProc(HWINEVENTHOOK, DWORD ev, HWND h, LONG idObject, LONG idChild,
                                  DWORD, DWORD) {
    if (g_stopping || !h || idObject != OBJID_WINDOW || idChild != CHILDID_SELF) return;

    switch (ev) {
        case EVENT_SYSTEM_FOREGROUND:
            OnForeground(h);
            break;

        case EVENT_SYSTEM_MINIMIZEEND:
            OnMinimizeEnd(h);
            if (!W.iconsHidden) ScheduleRecompute();
            break;

        case EVENT_OBJECT_DESTROY:
            ForgetWindow(h);
            if (W.iconsHidden) ScheduleRecompute();
            break;

        // May make a window appear: only relevant while the icons are shown.
        case EVENT_OBJECT_SHOW:
        case EVENT_OBJECT_UNCLOAKED:
            if (!W.iconsHidden && IsTopLevel(h)) ScheduleRecompute();
            break;

        // May leave the desktop clear: only relevant while the icons are hidden.
        case EVENT_OBJECT_HIDE:
        case EVENT_OBJECT_CLOAKED:
        case EVENT_SYSTEM_MINIMIZESTART:
            if (W.iconsHidden && IsTopLevel(h)) ScheduleRecompute();
            break;
    }
}

static void OnWatchdog() {
    if (!W.vdm) {  // the virtual desktop service may not be ready while Explorer starts
        CoCreateInstance(CLSID_VirtualDesktopManager, nullptr, CLSCTX_INPROC_SERVER,
                         IID_IVirtualDesktopManager, (void**)&W.vdm);
    }
    EnsureDesktopHooked();
    RefreshUserIconsSetting();
}

// ---------------------------------------------------------------------------
// Worker thread
// ---------------------------------------------------------------------------

static LRESULT CALLBACK WorkerWndProc(HWND h, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
        case WM_APP_DESKTOP_CLICK:
            OnRevealToggle(false);
            return 0;
        case WM_HOTKEY:
            if (wp == HOTKEY_REVEAL) OnRevealToggle(true);
            return 0;
        case WM_TIMER:
            if (wp == TIMER_RECOMPUTE) {
                KillTimer(h, TIMER_RECOMPUTE);
                W.recomputePending = false;
                RecomputeIcons();
            } else if (wp == TIMER_WATCHDOG) {
                OnWatchdog();
            } else if (wp == TIMER_FINALIZE) {
                OnFinalizeTick();
            }
            return 0;
        case WM_APP_STOP:
            g_workerWnd = nullptr;
            if (W.hotkeyRegistered) {
                UnregisterHotKey(h, HOTKEY_REVEAL);
                W.hotkeyRegistered = false;
            }
            DestroyWindow(h);
            return 0;
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProcW(h, msg, wp, lp);
}

static DWORD WINAPI WorkerThreadProc(LPVOID) {
    // An STA with a message loop: out-of-context WinEvents, RegisterHotKey and
    // COM all need one.
    HRESULT hrCom = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    CoCreateInstance(CLSID_VirtualDesktopManager, nullptr, CLSCTX_INPROC_SERVER,
                     IID_IVirtualDesktopManager, (void**)&W.vdm);

    WNDCLASSEXW wc = {};
    wc.cbSize = sizeof(wc);
    wc.lpfnWndProc = WorkerWndProc;
    wc.hInstance = g_hModule;
    wc.lpszClassName = WORKER_CLASS;
    ATOM atom = RegisterClassExW(&wc);
    HWND wnd = atom ? CreateWindowExW(0, WORKER_CLASS, L"", 0, 0, 0, 0, 0, HWND_MESSAGE, nullptr,
                                      g_hModule, nullptr)
                    : nullptr;
    if (!wnd) {
        Wh_Log(L"[worker] couldn't create the message window (%u)", GetLastError());
        SetEvent(g_hWorkerReady);
        if (atom) UnregisterClassW(WORKER_CLASS, g_hModule);
        if (W.vdm) W.vdm->Release();
        if (SUCCEEDED(hrCom)) CoUninitialize();
        return 1;
    }
    g_workerWnd = wnd;

    // Small, specific event ranges. No WINEVENT_SKIPOWNPROCESS: File Explorer
    // windows and Task View belong to explorer.exe and matter here.
    const DWORD flags = WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNTHREAD;
    const DWORD ranges[][2] = {
        {EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND},
        {EVENT_SYSTEM_MINIMIZESTART, EVENT_SYSTEM_MINIMIZEEND},
        {EVENT_OBJECT_DESTROY, EVENT_OBJECT_HIDE},
        {EVENT_OBJECT_CLOAKED, EVENT_OBJECT_UNCLOAKED},
    };
    for (const auto& r : ranges) {
        HWINEVENTHOOK hk = SetWinEventHook(r[0], r[1], nullptr, WinEventProc, 0, 0, flags);
        if (hk) W.eventHooks.push_back(hk);
    }

    if (g_settings.hotkeyVk) {
        W.hotkeyRegistered = RegisterHotKey(wnd, HOTKEY_REVEAL,
                                            g_settings.hotkeyMods | MOD_NOREPEAT,
                                            g_settings.hotkeyVk) != 0;
        if (W.hotkeyRegistered) {
            Wh_Log(L"[hotkey] registered");
        } else {
            Wh_Log(L"[hotkey] couldn't register (%u): another app or Windows already uses it",
                   GetLastError());
        }
    }

    // From here on Wh_ModUninit may stop the thread. What follows can wait on
    // the desktop thread, so it runs after the signal.
    SetEvent(g_hWorkerReady);

    RefreshUserIconsSetting();
    EnsureDesktopHooked();
    OnForeground(GetForegroundWindow());
    SetTimer(wnd, TIMER_WATCHDOG, WATCHDOG_MS, nullptr);

    MSG m;
    while (GetMessageW(&m, nullptr, 0, 0) > 0) DispatchMessageW(&m);

    for (HWINEVENTHOOK hk : W.eventHooks) UnhookWinEvent(hk);
    W.eventHooks.clear();
    if (W.vdm) {
        W.vdm->Release();
        W.vdm = nullptr;
    }
    UnregisterClassW(WORKER_CLASS, g_hModule);
    if (SUCCEEDED(hrCom)) CoUninitialize();
    return 0;
}

// ---------------------------------------------------------------------------
// Mod lifecycle
// ---------------------------------------------------------------------------

BOOL Wh_ModInit() {
    Wh_Log(L"Init");
    LoadSettings();

    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                           GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                       reinterpret_cast<LPCWSTR>(&g_moduleAnchor), &g_hModule);

    g_stopping = false;
    g_hWorkerReady = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_hWorkerReady) return FALSE;

    // No waiting here: when Explorer is starting, Wh_ModInit runs before the
    // process starts executing and the desktop doesn't exist yet.
    g_hWorker = CreateThread(nullptr, 0, WorkerThreadProc, nullptr, 0, nullptr);
    if (!g_hWorker) {
        Wh_Log(L"Couldn't create the worker thread (%u)", GetLastError());
        CloseHandle(g_hWorkerReady);
        g_hWorkerReady = nullptr;
        return FALSE;
    }
    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L"Uninit");
    g_stopping = true;

    if (g_hWorker) {
        // The worker always signals "ready" (even on failure) before its loop.
        HANDLE waits[] = {g_hWorkerReady, g_hWorker};
        WaitForMultipleObjects(2, waits, FALSE, INFINITE);
        if (HWND w = g_workerWnd) PostMessageW(w, WM_APP_STOP, 0, 0);
        WaitForSingleObject(g_hWorker, INFINITE);
        CloseHandle(g_hWorker);
        g_hWorker = nullptr;
    }

    HWND lv = g_listView.exchange(nullptr);
    HWND dv = g_defView.exchange(nullptr);
    if (lv && IsWindow(lv)) {
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(lv, DesktopSubclassProc);
        // Leave the icons the way Windows wants them ("Show desktop icons").
        if (g_settings.iconsMode != ICONS_OFF) {
            bool wantVisible = !UserHidesDesktopIcons();
            if (wantVisible != (IsWindowVisible(lv) != FALSE)) {
                ShowWindowAsync(lv, wantVisible ? SW_SHOWNA : SW_HIDE);
            }
        }
    }
    if (dv && IsWindow(dv)) {
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(dv, DesktopSubclassProc);
    }

    if (g_hWorkerReady) {
        CloseHandle(g_hWorkerReady);
        g_hWorkerReady = nullptr;
    }
}

BOOL Wh_ModSettingsChanged(BOOL* bReload) {
    *bReload = TRUE;  // clean reload: thread, hooks, hotkey and subclasses are rebuilt
    return TRUE;
}
