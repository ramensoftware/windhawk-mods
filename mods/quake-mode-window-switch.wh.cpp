// ==WindhawkMod==
// @id                quake-mode-window-switch
// @name              Quake mode window switch
// @description       Slide a chosen app down from off-screen with a hotkey, Quake-console style
// @version           1.0
// @author            Tal Koren
// @github            https://github.com/dewbjorn
// @include           explorer.exe
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Quake mode window switch

Press a configurable hotkey (default **`~`**, the tilde/backtick key) to slide a chosen app's
window down from off-screen into a docked strip at the top of the
screen, Quake-console style. Press again (or click another window) to
slide it back up out of view.

## Notes

- Runs in `explorer.exe`, so one global hotkey handles everything.
- Only acts on the configured process; if it isn't running, nothing happens.
- The window is not resized permanently, it's repositioned/resized
  only while parked in its docked or hidden slot.
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
- hotkey: "`"
  $name: Hotkey
  $description: >-
    Key combo to toggle the window, e.g. "`" or "Ctrl+Shift+F12".
    Supported modifiers: Ctrl, Alt, Shift, Win.
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
#include <thread>
#include <atomic>
#include <algorithm>
#include <cwctype>
#include <cmath>

static std::wstring g_processName;
static int g_heightPercent = 40;
static bool g_useCursorMonitor = false;
static UINT g_hotkeyModifiers = 0;
static UINT g_hotkeyVk = VK_OEM_3;
static bool g_hideFromTaskbar = false;
static bool g_hideTitleBar = false;
static bool g_autoHideOnFocusLoss = true;
static LONG_PTR g_originalExStyle = 0;
static LONG_PTR g_originalStyle = 0;
static bool g_styleModified = false;

static std::thread g_hotkeyThread;
static std::atomic<bool> g_running = false;
static DWORD g_hotkeyThreadId = 0;
static HANDLE g_hotkeyThreadReady = nullptr;
static HWINEVENTHOOK g_foregroundHook = nullptr;

static HWND g_targetHwnd = nullptr;
static std::atomic<bool> g_visible = false;
static HWND g_previousForegroundHwnd = nullptr;

static constexpr int kHotkeyId = 1;
static constexpr int kAnimationDurationMs = 120;
static constexpr int kAnimationFrameMs = 10;

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

        SetWindowPos(
            hwnd,
            nullptr,
            left,
            top,
            right - left,
            bottom - top,
            SWP_NOZORDER | SWP_NOACTIVATE
        );

        std::this_thread::sleep_for(std::chrono::milliseconds(kAnimationFrameMs));
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
}

static void LoadSettings()
{
    PCWSTR processName = Wh_GetStringSetting(L"processName");
    g_processName = ToLower(processName ? processName : L"");
    Wh_FreeStringSetting(processName);

    g_heightPercent = Wh_GetIntSetting(L"heightPercent");
    if (g_heightPercent < 5 || g_heightPercent > 100) {
        g_heightPercent = 40;
    }

    PCWSTR monitorMode = Wh_GetStringSetting(L"monitorMode");
    g_useCursorMonitor = monitorMode && ToLower(monitorMode) == L"cursor";
    Wh_FreeStringSetting(monitorMode);

    PCWSTR hotkey = Wh_GetStringSetting(L"hotkey");
    UINT modifiers = 0;
    UINT vk = 0;
    ParseHotkeySetting(hotkey ? hotkey : L"`", modifiers, vk);
    Wh_FreeStringSetting(hotkey);

    if (vk == 0) {
        Wh_Log(L"Invalid hotkey setting, falling back to `");
        modifiers = 0;
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

static bool IsCandidateWindow(HWND hwnd)
{
    if (!IsWindow(hwnd) || !IsWindowVisible(hwnd)) {
        return false;
    }

    if (GetAncestor(hwnd, GA_ROOT) != hwnd) {
        return false;
    }

    if (GetWindow(hwnd, GW_OWNER) != nullptr) {
        return false;
    }

    return true;
}

// ---- Window styling ----

static void ApplyWindowStyles(HWND hwnd)
{
    if (g_styleModified && g_targetHwnd == hwnd) {
        return;
    }

    g_originalExStyle = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);
    g_originalStyle = GetWindowLongPtrW(hwnd, GWL_STYLE);

    LONG_PTR exStyle = g_originalExStyle;
    if (g_hideFromTaskbar) {
        exStyle |= WS_EX_TOOLWINDOW;
        exStyle &= ~WS_EX_APPWINDOW;
    }

    LONG_PTR style = g_originalStyle;
    if (g_hideTitleBar) {
        style &= ~WS_CAPTION;
    }

    g_styleModified = true;

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

static void RestoreWindowStyles()
{
    if (!g_styleModified || !g_targetHwnd || !IsWindow(g_targetHwnd)) {
        g_styleModified = false;
        return;
    }

    SetWindowLongPtrW(g_targetHwnd, GWL_EXSTYLE, g_originalExStyle);
    SetWindowLongPtrW(g_targetHwnd, GWL_STYLE, g_originalStyle);
    SetWindowPos(g_targetHwnd, nullptr, 0, 0, 0, 0,
        SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);

    if (g_hideFromTaskbar && IsWindowVisible(g_targetHwnd)) {
        ShowWindow(g_targetHwnd, SW_HIDE);
        ShowWindow(g_targetHwnd, SW_SHOWNA);
    }

    g_styleModified = false;
}

static BOOL CALLBACK FindTargetWindowProc(HWND hwnd, LPARAM lParam)
{
    if (!IsCandidateWindow(hwnd)) {
        return TRUE;
    }

    std::wstring processName;
    if (!GetWindowProcessName(hwnd, processName) || processName != g_processName) {
        return TRUE;
    }

    *reinterpret_cast<HWND*>(lParam) = hwnd;
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

    HWND found = nullptr;
    EnumWindows(FindTargetWindowProc, reinterpret_cast<LPARAM>(&found));
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

static void GetDockedAndHiddenRects(HWND hwnd, RECT& docked, RECT& hidden)
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
    if (!g_visible.load() || !g_targetHwnd || !IsWindow(g_targetHwnd)) {
        return;
    }

    RECT docked, hidden;
    GetDockedAndHiddenRects(g_targetHwnd, docked, hidden);

    RECT current = {};
    GetWindowRect(g_targetHwnd, &current);

    AnimateWindowToRect(g_targetHwnd, current, hidden, kAnimationDurationMs);
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
    GetDockedAndHiddenRects(hwnd, docked, hidden);

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

    if (g_visible.load()) {
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

    if (!g_autoHideOnFocusLoss || !g_visible.load() || !g_targetHwnd) {
        return;
    }

    HWND root = GetAncestor(hwnd, GA_ROOT);
    if (root == g_targetHwnd) {
        return;
    }

    HideTargetWindow(/* restoreFocus */ false);
}

static void HotkeyThreadProc()
{
    g_hotkeyThreadId = GetCurrentThreadId();

    bool registered = RegisterHotKey(nullptr, kHotkeyId, g_hotkeyModifiers, g_hotkeyVk);
    if (!registered) {
        Wh_Log(L"RegisterHotKey failed: %u", GetLastError());
    } else {
        Wh_Log(L"Quake mode hotkey registered");
        g_foregroundHook = SetWinEventHook(
            EVENT_SYSTEM_FOREGROUND,
            EVENT_SYSTEM_FOREGROUND,
            nullptr,
            OnForegroundChanged,
            0,
            0,
            WINEVENT_OUTOFCONTEXT
        );
    }

    SetEvent(g_hotkeyThreadReady);

    if (!registered) {
        return;
    }

    MSG msg;
    while (g_running.load() && GetMessageW(&msg, nullptr, 0, 0) > 0) {
        if (msg.message == WM_HOTKEY && msg.wParam == kHotkeyId) {
            ToggleTargetWindow();
        }
    }

    if (g_foregroundHook) {
        UnhookWinEvent(g_foregroundHook);
        g_foregroundHook = nullptr;
    }

    UnregisterHotKey(nullptr, kHotkeyId);
    Wh_Log(L"Quake mode hotkey unregistered");
}

BOOL Wh_ModInit()
{
    Wh_Log(L"Init");

    LoadSettings();

    g_hotkeyThreadReady = CreateEventW(nullptr, TRUE, FALSE, nullptr);

    g_running = true;
    g_hotkeyThread = std::thread(HotkeyThreadProc);

    WaitForSingleObject(g_hotkeyThreadReady, INFINITE);
    CloseHandle(g_hotkeyThreadReady);
    g_hotkeyThreadReady = nullptr;

    return TRUE;
}

void Wh_ModUninit()
{
    Wh_Log(L"Uninit");

    RestoreWindowStyles();

    g_running = false;

    if (g_hotkeyThreadId != 0) {
        PostThreadMessageW(g_hotkeyThreadId, WM_QUIT, 0, 0);
    }

    if (g_hotkeyThread.joinable()) {
        g_hotkeyThread.join();
    }
}

BOOL Wh_ModSettingsChanged(BOOL* bReload)
{
    Wh_Log(L"Settings changed");

    // Several settings (hotkey, taskbar/title-bar styling) only take
    // effect from Wh_ModInit, so always request a full reload rather
    // than trying to hot-apply a subset.
    *bReload = TRUE;
    return TRUE;
}
