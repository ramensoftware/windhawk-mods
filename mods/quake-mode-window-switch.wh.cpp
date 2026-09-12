// ==WindhawkMod==
// @id                quake-mode-window-switch
// @name              Quake mode window switch
// @description       Slide a chosen app down from off-screen with a hotkey, Quake-console style
// @version           2.0
// @author            Tal Koren
// @github            https://github.com/dewbjorn
// @include           windhawk.exe
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Quake mode window switch

Press a configurable hotkey (default **Win+`**) to slide a chosen app's
window down from off-screen into a docked strip at the top of the
screen, Quake-console style. Press again (or click another window) to
slide it back up out of view.

## Notes

- Runs as a standalone background process (a "tool mod"), not injected
  into `explorer.exe` - the hotkey works no matter how many `explorer.exe`
  processes are running, and a bug here can't take the shell down with it.
- Only acts on the configured process; if it isn't running, nothing happens.
- The window is not resized permanently, it's repositioned/resized only
  while parked in its docked or hidden slot; disabling the mod restores
  its original position, size and styles.
- If the configured hotkey is already registered by another app, this mod
  silently does nothing when pressed - there's no visible error, only a
  `RegisterHotKey failed` line in the log (logging is off by default).
  Windows Terminal in particular binds its own quake mode to Win+` by
  default, so pick a hotkey that isn't already spoken for.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- processName: WindowsTerminal.exe
  $name: Target process
  $description: >-
    Executable name (e.g. WindowsTerminal.exe) of the app to apply quake mode to.
- heightPercent: 40
  $name: Height percent
  $description: >-
    Height of the docked window as a percentage of the monitor's work area height.
- monitorMode: primary
  $name: Monitor
  $options:
  - primary: Primary monitor
  - cursor: Monitor under cursor
- hotkey: "Ctrl+Alt+`"
  $name: Hotkey
  $description: >-
    Key combo to toggle the window, e.g. "Ctrl+Alt+`" or "Ctrl+Shift+F12".
    Supported modifiers: Ctrl, Alt, Shift, Win. At least one modifier is
    required - a modifier-less key (e.g. a bare "`") would be captured
    globally and block typing that key in every other app. If another
    app already owns the combo, RegisterHotKey silently does nothing -
    enable logging and check for a "RegisterHotKey failed" line if the
    hotkey seems unresponsive.
- hideFromTaskbar: false
  $name: Hide from taskbar
  $description: >-
    Remove the window's taskbar button while this mod is active.
- hideTitleBar: false
  $name: Hide title bar
  $description: >-
    Remove the window's title bar while this mod is active.
- autoHideOnFocusLoss: true
  $name: Auto-hide on focus loss
  $description: >-
    Slide the window back out of view when another window gets focus,
    not just when the hotkey is pressed again.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <string>
#include <atomic>
#include <algorithm>
#include <cwctype>
#include <cstdlib>
#include <cmath>

static std::wstring g_processName;
static int g_heightPercent = 40;
static bool g_useCursorMonitor = false;
static UINT g_hotkeyModifiers = 0;
static UINT g_hotkeyVk = VK_OEM_3;
static bool g_hideFromTaskbar = false;
static bool g_hideTitleBar = false;
static bool g_autoHideOnFocusLoss = true;

static HWND g_styledHwnd = nullptr;
static LONG_PTR g_originalExStyle = 0;
static LONG_PTR g_originalStyle = 0;
static WINDOWPLACEMENT g_originalPlacement = {sizeof(WINDOWPLACEMENT)};

static HANDLE g_hThread = nullptr;
static HWND g_hMsgWnd = nullptr;
static std::atomic<bool> g_running{false};
static HWINEVENTHOOK g_foregroundHook = nullptr;

static HWND g_targetHwnd = nullptr;
static bool g_visible = false;
static HWND g_previousForegroundHwnd = nullptr;

static constexpr int kHotkeyId = 1;
static constexpr int kAnimationDurationMs = 120;
static constexpr int kAnimationFrameMs = 10;

#define WM_APP_SETTINGS_CHANGED (WM_APP + 1)

static const wchar_t kMsgWndClassName[] = L"QuakeModeWindowSwitch_MsgWnd";

static double EaseInOut(double t)
{
    return t * t * (3.0 - 2.0 * t);
}

static void AnimateWindowToRect(HWND hwnd, const RECT& from, const RECT& to, int durationMs)
{
    int steps = std::max(1, durationMs / kAnimationFrameMs);

    for (int i = 1; i <= steps; i++) {
        double t = static_cast<double>(i) / static_cast<double>(steps);
        double e = EaseInOut(t);

        int left = static_cast<int>(std::lround(from.left + (to.left - from.left) * e));
        int top = static_cast<int>(std::lround(from.top + (to.top - from.top) * e));
        int right = static_cast<int>(std::lround(from.right + (to.right - from.right) * e));
        int bottom = static_cast<int>(std::lround(from.bottom + (to.bottom - from.bottom) * e));

        // ASYNCWINDOWPOS keeps a hung target app from stalling this loop -
        // and with it, WhTool_ModUninit's join - indefinitely.
        SetWindowPos(
            hwnd,
            nullptr,
            left,
            top,
            right - left,
            bottom - top,
            SWP_NOZORDER | SWP_NOACTIVATE | SWP_ASYNCWINDOWPOS
        );

        Sleep(kAnimationFrameMs);
    }
}

// ---- Settings parsing ----

static std::wstring ToLower(std::wstring s)
{
    std::transform(s.begin(), s.end(), s.begin(), [](wchar_t c) { return std::towlower(c); });
    return s;
}

static UINT VkFromKeyName(const std::wstring& name)
{
    std::wstring lower = ToLower(name);

    if (lower.size() >= 2 && lower[0] == L'f') {
        int n = _wtoi(lower.c_str() + 1);
        if (n >= 1 && n <= 24) {
            return VK_F1 + (n - 1);
        }
    }

    if (lower == L"esc" || lower == L"escape") return VK_ESCAPE;
    if (lower == L"tab") return VK_TAB;
    if (lower == L"space") return VK_SPACE;
    if (lower == L"enter" || lower == L"return") return VK_RETURN;
    if (lower == L"`" || lower == L"tilde") return VK_OEM_3;

    if (name.size() == 1) {
        wchar_t c = towupper(name[0]);
        SHORT vk = VkKeyScanW(c);
        if (vk != -1) {
            return LOBYTE(vk);
        }
    }

    return 0;
}

static void ParseHotkeySetting(const std::wstring& spec, UINT& modifiersOut, UINT& vkOut)
{
    modifiersOut = 0;
    vkOut = 0;

    size_t start = 0;
    while (start <= spec.size()) {
        size_t plus = spec.find(L'+', start);
        std::wstring token = (plus == std::wstring::npos)
            ? spec.substr(start)
            : spec.substr(start, plus - start);

        std::wstring lower = ToLower(token);
        if (lower == L"ctrl" || lower == L"control") {
            modifiersOut |= MOD_CONTROL;
        } else if (lower == L"alt") {
            modifiersOut |= MOD_ALT;
        } else if (lower == L"shift") {
            modifiersOut |= MOD_SHIFT;
        } else if (lower == L"win" || lower == L"windows") {
            modifiersOut |= MOD_WIN;
        } else if (!token.empty()) {
            vkOut = VkFromKeyName(token);
        }

        if (plus == std::wstring::npos) {
            break;
        }
        start = plus + 1;
    }

    // A modifier-less hotkey registers the plain key globally, swallowing
    // it in every app (e.g. a bare "`" blocks typing a backtick anywhere).
    if (modifiersOut == 0) {
        vkOut = 0;
    }
}

static void LoadSettings()
{
    PCWSTR processName = Wh_GetStringSetting(L"processName");
    g_processName = ToLower(processName);
    Wh_FreeStringSetting(processName);

    g_heightPercent = Wh_GetIntSetting(L"heightPercent");
    if (g_heightPercent < 5 || g_heightPercent > 100) {
        g_heightPercent = 40;
    }

    PCWSTR monitorMode = Wh_GetStringSetting(L"monitorMode");
    g_useCursorMonitor = ToLower(monitorMode) == L"cursor";
    Wh_FreeStringSetting(monitorMode);

    PCWSTR hotkey = Wh_GetStringSetting(L"hotkey");
    UINT modifiers = 0;
    UINT vk = 0;
    ParseHotkeySetting(hotkey, modifiers, vk);
    Wh_FreeStringSetting(hotkey);

    if (vk == 0) {
        Wh_Log(L"Invalid or modifier-less hotkey setting, falling back to Ctrl+Alt+`");
        modifiers = MOD_CONTROL | MOD_ALT;
        vk = VK_OEM_3;
    }

    g_hotkeyModifiers = modifiers;
    g_hotkeyVk = vk;

    g_hideFromTaskbar = Wh_GetIntSetting(L"hideFromTaskbar") != 0;
    g_hideTitleBar = Wh_GetIntSetting(L"hideTitleBar") != 0;
    g_autoHideOnFocusLoss = Wh_GetIntSetting(L"autoHideOnFocusLoss") != 0;
}

// ---- Window/monitor lookup ----

static bool GetWindowProcessName(HWND hwnd, std::wstring& nameOut)
{
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    if (pid == 0) {
        return false;
    }

    HANDLE process = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!process) {
        return false;
    }

    wchar_t path[MAX_PATH];
    DWORD size = MAX_PATH;
    bool ok = QueryFullProcessImageNameW(process, 0, path, &size);
    CloseHandle(process);

    if (!ok) {
        return false;
    }

    std::wstring full(path, size);
    size_t slash = full.find_last_of(L"\\/");
    nameOut = ToLower(slash == std::wstring::npos ? full : full.substr(slash + 1));
    return true;
}

static bool IsCandidateWindow(HWND hwnd, bool requireVisible)
{
    if (!IsWindow(hwnd)) {
        return false;
    }

    if (requireVisible && !IsWindowVisible(hwnd)) {
        return false;
    }

    if (GetAncestor(hwnd, GA_ROOT) != hwnd) {
        return false;
    }

    if (GetWindow(hwnd, GW_OWNER) != nullptr) {
        return false;
    }

    if (!requireVisible) {
        // Only relevant for the orphaned-hidden fallback: skip a
        // zero-size window (e.g. an app's hidden helper window) rather
        // than mistaking it for the parked target.
        RECT rect;
        if (!GetWindowRect(hwnd, &rect) || rect.right <= rect.left || rect.bottom <= rect.top) {
            return false;
        }
    }

    return true;
}

// ---- Window styling ----

// Restores styles and the original WINDOWPLACEMENT (position, size, and
// minimized/maximized state) onto whichever window ApplyWindowStyles last
// captured. Also undoes the SW_HIDE used to park the window off-screen.
static void RestoreOriginalState()
{
    if (!g_styledHwnd) {
        return;
    }

    HWND hwnd = g_styledHwnd;
    g_styledHwnd = nullptr;

    if (!IsWindow(hwnd)) {
        return;
    }

    SetWindowLongPtrW(hwnd, GWL_EXSTYLE, g_originalExStyle);
    SetWindowLongPtrW(hwnd, GWL_STYLE, g_originalStyle);
    SetWindowPos(hwnd, nullptr, 0, 0, 0, 0,
        SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);

    if (g_hideFromTaskbar && IsWindowVisible(hwnd)) {
        ShowWindow(hwnd, SW_HIDE);
        ShowWindow(hwnd, SW_SHOWNA);
    }

    SetWindowPlacement(hwnd, &g_originalPlacement);
}

// Captures the window's original styles/placement the first time it's
// touched and applies the configured taskbar/title-bar tweaks. Tracked
// by g_styledHwnd, independent of g_targetHwnd, so a stale capture never
// gets applied to (or restored onto) the wrong window.
static void ApplyWindowStyles(HWND hwnd)
{
    if (g_styledHwnd == hwnd) {
        return;
    }

    if (g_styledHwnd) {
        RestoreOriginalState();
    }

    g_originalExStyle = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);
    g_originalStyle = GetWindowLongPtrW(hwnd, GWL_STYLE);
    g_originalPlacement = WINDOWPLACEMENT{sizeof(WINDOWPLACEMENT)};
    GetWindowPlacement(hwnd, &g_originalPlacement);
    g_styledHwnd = hwnd;

    LONG_PTR exStyle = g_originalExStyle;
    if (g_hideFromTaskbar) {
        exStyle |= WS_EX_TOOLWINDOW;
        exStyle &= ~WS_EX_APPWINDOW;
    }

    LONG_PTR style = g_originalStyle;
    if (g_hideTitleBar) {
        style &= ~WS_CAPTION;
    }

    if (exStyle == g_originalExStyle && style == g_originalStyle) {
        return;
    }

    SetWindowLongPtrW(hwnd, GWL_EXSTYLE, exStyle);
    SetWindowLongPtrW(hwnd, GWL_STYLE, style);
    SetWindowPos(hwnd, nullptr, 0, 0, 0, 0,
        SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);

    // Explorer's taskbar only re-reads WS_EX_TOOLWINDOW/APPWINDOW on a
    // hide/show transition, not on a live style change.
    if (g_hideFromTaskbar && IsWindowVisible(hwnd)) {
        ShowWindow(hwnd, SW_HIDE);
        ShowWindow(hwnd, SW_SHOWNA);
    }
}

struct FindWindowContext {
    HWND found;
    bool requireVisible;
};

static BOOL CALLBACK FindTargetWindowProc(HWND hwnd, LPARAM lParam)
{
    FindWindowContext& ctx = *reinterpret_cast<FindWindowContext*>(lParam);

    if (!IsCandidateWindow(hwnd, ctx.requireVisible)) {
        return TRUE;
    }

    std::wstring processName;
    if (!GetWindowProcessName(hwnd, processName) || processName != g_processName) {
        return TRUE;
    }

    ctx.found = hwnd;
    return FALSE;
}

static HWND FindTargetWindow()
{
    if (g_targetHwnd && IsWindow(g_targetHwnd)) {
        std::wstring processName;
        if (GetWindowProcessName(g_targetHwnd, processName) && processName == g_processName) {
            ApplyWindowStyles(g_targetHwnd);
            return g_targetHwnd;
        }
    }

    FindWindowContext ctx{nullptr, /* requireVisible */ true};
    EnumWindows(FindTargetWindowProc, reinterpret_cast<LPARAM>(&ctx));

    if (!ctx.found) {
        // Nothing visible matched - the tool process may have ended
        // abnormally (crash, killed) while the window was parked SW_HIDE'd,
        // leaving it invisible with no taskbar button and no Alt+Tab entry.
        // Fall back to a non-visible match so it isn't stranded forever.
        ctx.requireVisible = false;
        EnumWindows(FindTargetWindowProc, reinterpret_cast<LPARAM>(&ctx));
    }

    HWND found = ctx.found;

    if (found != g_targetHwnd) {
        // The previous target is gone or no longer matches; don't carry
        // over its shown/hidden state onto an unrelated window.
        g_visible = false;
    }

    g_targetHwnd = found;
    if (found) {
        ApplyWindowStyles(found);
    }
    return found;
}

static HMONITOR PickMonitor()
{
    if (g_useCursorMonitor) {
        POINT pt;
        if (GetCursorPos(&pt)) {
            return MonitorFromPoint(pt, MONITOR_DEFAULTTOPRIMARY);
        }
    }

    POINT origin = {0, 0};
    return MonitorFromPoint(origin, MONITOR_DEFAULTTOPRIMARY);
}

static void GetDockedAndHiddenRects(RECT& docked, RECT& hidden)
{
    HMONITOR monitor = PickMonitor();

    MONITORINFO mi = {};
    mi.cbSize = sizeof(mi);
    GetMonitorInfoW(monitor, &mi);

    const RECT& work = mi.rcWork;
    int width = work.right - work.left;
    int workHeight = work.bottom - work.top;
    int height = std::max(1, static_cast<int>(workHeight * (g_heightPercent / 100.0)));

    docked.left = work.left;
    docked.top = work.top;
    docked.right = work.left + width;
    docked.bottom = work.top + height;

    hidden = docked;
    hidden.top = work.top - height;
    hidden.bottom = work.top;
}

// ---- Show/hide ----

// Plain SetForegroundWindow silently no-ops (just flashes the taskbar
// icon) unless the calling thread "owns" the current input focus.
// Briefly attaching our input queue to the current foreground thread's
// satisfies that check regardless of timing.
static void ForceSetForegroundWindow(HWND hwnd)
{
    if (!hwnd) {
        return;
    }

    HWND foreground = GetForegroundWindow();
    DWORD foregroundThreadId = foreground ? GetWindowThreadProcessId(foreground, nullptr) : 0;
    DWORD currentThreadId = GetCurrentThreadId();

    bool attached = false;
    if (foregroundThreadId != 0 && foregroundThreadId != currentThreadId) {
        attached = AttachThreadInput(currentThreadId, foregroundThreadId, TRUE) != 0;
    }

    BringWindowToTop(hwnd);
    SetForegroundWindow(hwnd);

    if (attached) {
        AttachThreadInput(currentThreadId, foregroundThreadId, FALSE);
    }
}

static void HideTargetWindow(bool restoreFocus)
{
    if (!g_visible || !g_targetHwnd || !IsWindow(g_targetHwnd)) {
        return;
    }

    RECT docked, hidden;
    GetDockedAndHiddenRects(docked, hidden);

    RECT current = {};
    GetWindowRect(g_targetHwnd, &current);

    AnimateWindowToRect(g_targetHwnd, current, hidden, kAnimationDurationMs);

    // The hidden slot is only offscreen on a single-monitor, horizontally
    // arranged setup - on a vertically stacked layout it's another
    // monitor's screen. Actually hiding it also drops it from Alt+Tab
    // and stops it from rendering while parked.
    ShowWindow(g_targetHwnd, SW_HIDE);
    g_visible = false;

    // Only restore focus on an explicit hotkey collapse. On an
    // auto-hide (focus already moved to another window), the new
    // foreground window is already what the user wants - stealing
    // it back would fight the click that triggered the hide.
    if (restoreFocus && g_previousForegroundHwnd && IsWindow(g_previousForegroundHwnd)) {
        ForceSetForegroundWindow(g_previousForegroundHwnd);
    }
    g_previousForegroundHwnd = nullptr;
}

static void ShowTargetWindow(HWND hwnd)
{
    HWND currentForeground = GetForegroundWindow();
    g_previousForegroundHwnd = (currentForeground && currentForeground != hwnd) ? currentForeground : nullptr;

    RECT docked, hidden;
    GetDockedAndHiddenRects(docked, hidden);

    // IsWindowVisible is TRUE for a minimized window, and SW_SHOWNA below
    // shows it in its current (still iconic) state - so a minimized target
    // would otherwise never reappear and every hotkey press would silently
    // flip g_visible without anything visible happening.
    if (IsIconic(hwnd)) {
        ShowWindow(hwnd, SW_RESTORE);
    }

    SetWindowPos(
        hwnd,
        HWND_TOP,
        hidden.left,
        hidden.top,
        hidden.right - hidden.left,
        hidden.bottom - hidden.top,
        SWP_NOACTIVATE
    );

    ShowWindow(hwnd, SW_SHOWNA);
    ForceSetForegroundWindow(hwnd);

    AnimateWindowToRect(hwnd, hidden, docked, kAnimationDurationMs);

    g_visible = true;
}

static void ToggleTargetWindow()
{
    HWND hwnd = FindTargetWindow();
    if (!hwnd) {
        return;
    }

    if (g_visible) {
        HideTargetWindow(/* restoreFocus */ true);
    } else {
        ShowTargetWindow(hwnd);
    }
}

static void CALLBACK OnForegroundChanged(
    HWINEVENTHOOK, DWORD event, HWND hwnd, LONG idObject, LONG idChild, DWORD, DWORD)
{
    if (event != EVENT_SYSTEM_FOREGROUND || idObject != OBJID_WINDOW || idChild != CHILDID_SELF) {
        return;
    }

    if (!g_autoHideOnFocusLoss || !g_visible || !g_targetHwnd) {
        return;
    }

    // GA_ROOTOWNER (not GA_ROOT) walks the owner chain, so an owned
    // dialog/popup from the target app (settings, file picker, find bar)
    // still resolves back to the target and doesn't trigger auto-hide.
    HWND root = GetAncestor(hwnd, GA_ROOTOWNER);
    if (root == g_targetHwnd) {
        return;
    }

    HideTargetWindow(/* restoreFocus */ false);
}

// ---- Hotkey thread ----

static LRESULT CALLBACK HotkeyWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (msg == WM_HOTKEY && wParam == kHotkeyId) {
        ToggleTargetWindow();
        return 0;
    }

    if (msg == WM_APP_SETTINGS_CHANGED) {
        // (Un)RegisterHotKey must run on the thread that owns the window.
        UnregisterHotKey(hwnd, kHotkeyId);

        // Settings like taskbar/title-bar styling or the height percent
        // only affect a window when it's (re-)captured, so drop whatever
        // we're currently tracking and undo styling done under the old
        // settings; the next toggle re-applies everything fresh.
        RestoreOriginalState();
        g_targetHwnd = nullptr;
        g_visible = false;

        LoadSettings();

        if (!RegisterHotKey(hwnd, kHotkeyId, g_hotkeyModifiers | MOD_NOREPEAT, g_hotkeyVk)) {
            Wh_Log(L"RegisterHotKey failed: %u", GetLastError());
        } else {
            Wh_Log(L"Quake mode hotkey re-registered");
        }
        return 0;
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}

static DWORD WINAPI HotkeyThreadProc(LPVOID)
{
    // Without this, GetMonitorInfoW/GetCursorPos coordinates come back
    // DPI-virtualized on any monitor whose scaling differs from the
    // system DPI, so the docked strip lands at the wrong position/size
    // on a mixed-DPI multi-monitor setup (exactly what monitorMode:
    // cursor is for).
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

    WNDCLASSEXW wc = {sizeof(wc)};
    wc.lpfnWndProc = HotkeyWndProc;
    wc.hInstance = GetModuleHandle(nullptr);
    wc.lpszClassName = kMsgWndClassName;
    RegisterClassExW(&wc);

    HWND hwnd = CreateWindowExW(0, kMsgWndClassName, nullptr, 0, 0, 0, 0, 0,
        HWND_MESSAGE, nullptr, GetModuleHandle(nullptr), nullptr);
    if (!hwnd) {
        Wh_Log(L"Failed to create message window");
        return 0;
    }

    g_hMsgWnd = hwnd;

    if (!RegisterHotKey(hwnd, kHotkeyId, g_hotkeyModifiers | MOD_NOREPEAT, g_hotkeyVk)) {
        Wh_Log(L"RegisterHotKey failed: %u", GetLastError());
    } else {
        Wh_Log(L"Quake mode hotkey registered");
    }

    // Keep the message loop running even if registration failed, so a
    // later settings change (fixing the hotkey) can still register it,
    // and so the thread ID/window stay valid for a clean WM_QUIT shutdown.
    g_foregroundHook = SetWinEventHook(
        EVENT_SYSTEM_FOREGROUND,
        EVENT_SYSTEM_FOREGROUND,
        nullptr,
        OnForegroundChanged,
        0,
        0,
        WINEVENT_OUTOFCONTEXT
    );

    MSG msg;
    while (g_running.load() && GetMessageW(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    if (g_foregroundHook) {
        UnhookWinEvent(g_foregroundHook);
        g_foregroundHook = nullptr;
    }

    UnregisterHotKey(hwnd, kHotkeyId);
    g_hMsgWnd = nullptr;
    DestroyWindow(hwnd);
    UnregisterClassW(kMsgWndClassName, GetModuleHandle(nullptr));

    // Must run here, not in WhTool_ModUninit: window-coordinate APIs are
    // virtualized per the calling thread's DPI awareness, and this thread
    // is the one set to PER_MONITOR_AWARE_V2 (matching the capture in
    // ApplyWindowStyles). Restoring from a thread on the process default
    // awareness would re-scale the captured coordinates on a non-100%
    // display.
    RestoreOriginalState();

    Wh_Log(L"Quake mode hotkey unregistered");
    return 0;
}

// ---- Tool mod callbacks ----

BOOL WhTool_ModInit()
{
    Wh_Log(L"Init");

    LoadSettings();

    g_running = true;
    g_hThread = CreateThread(nullptr, 0, HotkeyThreadProc, nullptr, 0, nullptr);
    return g_hThread != nullptr;
}

void WhTool_ModSettingsChanged()
{
    Wh_Log(L"Settings changed");

    if (g_hMsgWnd) {
        PostMessageW(g_hMsgWnd, WM_APP_SETTINGS_CHANGED, 0, 0);
    }
}

void WhTool_ModUninit()
{
    Wh_Log(L"Uninit");

    // The hotkey thread restores styles/placement itself once its message
    // loop exits, under its own DPI awareness context - see the end of
    // HotkeyThreadProc.
    g_running = false;
    if (g_hMsgWnd) {
        PostMessageW(g_hMsgWnd, WM_QUIT, 0, 0);
    }
    if (g_hThread) {
        // Bounded: the toggle/restore path does synchronous cross-process
        // work (SetWindowPos, SetForegroundWindow, AttachThreadInput) that
        // a hung target app can stall indefinitely, and Wh_ModUninit calls
        // ExitProcess(0) right after this returns regardless.
        WaitForSingleObject(g_hThread, 3000);
        CloseHandle(g_hThread);
        g_hThread = nullptr;
    }
}

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

BOOL Wh_ModInit() {
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

void Wh_ModAfterInit() {
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

void Wh_ModSettingsChanged() {
    if (g_isToolModProcessLauncher) {
        return;
    }

    WhTool_ModSettingsChanged();
}

void Wh_ModUninit() {
    if (g_isToolModProcessLauncher) {
        return;
    }

    WhTool_ModUninit();
    ExitProcess(0);
}
