// ==WindhawkMod==
// @id              windows-10-language-flyout-guard-on-win11-24h2
// @name            Windows 10 language flyout guard and indicator colors
// @description     This mod hides the startup language flyout in the private Windows 10 shell on Windows 11 24H2+
// @version         1.0.0
// @author          babamohammed
// @github          https://github.com/babamohammed2022
// @architecture    x86-64
// @compilerOptions -ladvapi32 -lcomctl32 -lgdi32 -luser32
// @include         explorer.exe
// @exclude         %SystemRoot%\explorer.exe
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Windows 10 language flyout guard and indicator colors

This mod addresses two issues that can occur when the private Windows 10 shell is used by the Windows 10 taskbar mod on Windows 11 24H2 and later:

- **A language flyout that opens at startup.** For a limited time after the shell starts, the mod suppresses known language-switcher window classes in this Explorer process. It does not enumerate or hide windows owned by other processes. Clicking the language indicator manually is allowed through the guard.
- **The language indicator's orientation and appearance.** The optional color feature uses one theme-aware painter for the indicator's client area. It handles paint itself rather than allowing Explorer's legacy grey fill to overwrite the composited background, and it repaints on hover, press, theme, and layout state changes. If taskbar transparency is disabled or a classic/high-contrast theme is used, the transparent background might not blend correctly; turn this option off in that setup.

The guard lasts 20 seconds by default. Each intercepted flyout can extend it by 15 seconds, up to four times the configured duration. No window is closed or destroyed.

The mod targets non-SystemRoot `explorer.exe` instances and limits the flyout sweep to the current process. System files are not replaced.

## Screenshots

### Before

![Before](https://raw.githubusercontent.com/babamohammed2022/babamohammed2022/main/before.png)

### After

![After](https://raw.githubusercontent.com/babamohammed2022/babamohammed2022/main/after.png)

## Settings

| Setting | What it does |
|---|---|
| `LanguageGuard` (default on) | suppress known startup language flyouts in this Explorer process |
| `LanguageGuardSeconds` (default 20) | initial guard duration; an intercepted flyout extends it by 15 seconds, up to four times this duration |
| `LanguageIndicatorColours` (default on) | redraw the indicator text horizontally with a theme-appropriate color |
| `LanguageIndicatorRightClickOpensTaskbarMenu` (default off) | route a right-click from the language indicator to the taskbar context menu instead of the indicator's native menu |
| `LogLanguageGuard` (default off) | log a one-time census of visible window classes in this Explorer process and each matching suppression |

The color option intentionally overlaps with [Fix language indicator in Win10 taskbar under Win11 24H2+](https://windhawk.net/mods/fix-legacy-taskbar-tray-input-indicator) by Anixx: both repaint the same visible indicator using different techniques. This mod retains the color feature alongside the flyout guard; **do not enable both indicator-repainting features at the same time**, since either painter can overwrite the other's result. If you only need the flyout guard, turn `LanguageIndicatorColours` off.

The color path is adapted from that existing mod's horizontal-text correction. The flyout guard is a separate feature.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- LanguageGuard: true
  $name: Hide the language flyout at logon
  $description: >-
    Suppresses known language flyouts in this Explorer process during startup. A click on
    the language indicator lets the flyout through temporarily.
- LanguageGuardSeconds: 20
  $name: Guard duration (seconds)
  $description: >-
    How long the guard watches for the flyout. Each intercepted flyout extends it by
    15 seconds, up to four times this duration.
- LanguageIndicatorColours: true
  $name: Windows 10 colors of the language indicator
  $description: >-
    Draws the language indicator text horizontally with a theme-appropriate color.
    The transparent background relies on taskbar transparency composition.
- LanguageIndicatorRightClickOpensTaskbarMenu: false
  $name: Open the taskbar menu on indicator right-click
  $description: >-
    Off by default. When enabled, right-clicking the language indicator opens the taskbar
    context menu instead of the indicator's native context menu.
- LogLanguageGuard: false
  $name: Log the windows of the logon
  $description: >-
    Logs visible window classes in this Explorer process once when the guard starts,
    and logs matching windows that it suppresses.
*/
// ==/WindhawkModSettings==
#include <windows.h>
#include <commctrl.h>
#include <windhawk_api.h>
#include <windhawk_utils.h>
#include <atomic>
#include <string.h>
#include <wchar.h>

// The private-shell target is selected by Windhawk's real process path filters
// above; no runtime GetModuleFileName check is used (Fake Explorer path hooks it).
static std::atomic<bool> g_unloading{false};

// Options of this module. These are read from hook and window-procedure threads.
static std::atomic<bool> g_langGuardEnabled{true};
static std::atomic<DWORD> g_langGuardMs{20000};
static std::atomic<bool> g_indicatorColours{true};
static std::atomic<bool> g_routeRightClickToTaskbar{false};
static std::atomic<bool> g_langCensusLog{false};

// With the color option enabled, the subclass owns the complete client paint:
// forwarding WM_PAINT would restore Explorer's legacy fill over our composition.
// Mouse/theme/layout changes invalidate the cell and use the same painter, so the
// appearance follows state transitions without a competing periodic repaint.

static void TrackIndicatorWindow(HWND hwnd);

class ScopedGdiObj {
public:
    explicit ScopedGdiObj(HGDIOBJ o = nullptr) : m_o(o) {}
    ~ScopedGdiObj() { if (m_o) DeleteObject(m_o); }
    ScopedGdiObj(const ScopedGdiObj&) = delete;
    ScopedGdiObj& operator=(const ScopedGdiObj&) = delete;
    HGDIOBJ get() const { return m_o; }
private:
    HGDIOBJ m_o;
};

// Restores the previously selected GDI object (SelectObject is sticky).
class ScopedSelectedObject {
public:
    ScopedSelectedObject(HDC hdc, HGDIOBJ obj) : m_hdc(hdc), m_old(nullptr) {
        if (obj) m_old = SelectObject(hdc, obj);
    }
    ~ScopedSelectedObject() { if (m_old && m_old != HGDI_ERROR) SelectObject(m_hdc, m_old); }
    ScopedSelectedObject(const ScopedSelectedObject&) = delete;
    ScopedSelectedObject& operator=(const ScopedSelectedObject&) = delete;
private:
    HDC m_hdc;
    HGDIOBJ m_old;
};

// GDI device contexts are reused by Explorer, including for hover redraws. Save
// and restore the whole state so our text color/background mode do not leak into
// the next native paint.
class ScopedSavedDc {
public:
    explicit ScopedSavedDc(HDC hdc) : m_hdc(hdc), m_saved(hdc ? SaveDC(hdc) : 0) {}
    ~ScopedSavedDc() { if (m_saved) RestoreDC(m_hdc, m_saved); }
    ScopedSavedDc(const ScopedSavedDc&) = delete;
    ScopedSavedDc& operator=(const ScopedSavedDc&) = delete;
    bool valid() const { return m_saved != 0; }
private:
    HDC m_hdc;
    int m_saved;
};

// Theme lookup is cached because the single-painter path can run on every state
// transition (paint, hover, press, layout change).
static SRWLOCK g_themeCacheLock = SRWLOCK_INIT;
static ULONGLONG g_themeCacheTick = 0;
static bool g_themeUsesLight = false;

static bool TaskbarUsesLightTheme() {
    AcquireSRWLockExclusive(&g_themeCacheLock);
    const ULONGLONG now = GetTickCount64();
    if (!g_themeCacheTick || now - g_themeCacheTick >= 1000) {
        DWORD value = 0, size = sizeof(value);
        if (RegGetValueW(HKEY_CURRENT_USER,
                         L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
                         L"SystemUsesLightTheme", RRF_RT_REG_DWORD, nullptr,
                         &value, &size) == ERROR_SUCCESS) {
            g_themeUsesLight = value != 0;
        } else {
            g_themeUsesLight = false;
        }
        g_themeCacheTick = now;
    }
    const bool light = g_themeUsesLight;
    ReleaseSRWLockExclusive(&g_themeCacheLock);
    return light;
}

static void ResetThemeCache() {
    AcquireSRWLockExclusive(&g_themeCacheLock);
    g_themeCacheTick = 0;
    ReleaseSRWLockExclusive(&g_themeCacheLock);
}

static COLORREF IndicatorTextColor() {
    // Avoid pure black: GDI black is treated as transparent by the composited
    // taskbar, so use an opaque near-black on light themes and white on dark ones.
    return TaskbarUsesLightTheme() ? RGB(28, 28, 28) : RGB(255, 255, 255);
}

// The language flyout is suppressed only inside this Explorer process. Exact
// class matching avoids hiding unrelated third-party windows with similar names.
static std::atomic<ULONGLONG> g_langGuardUntil{0};
static std::atomic<ULONGLONG> g_langGuardStart{0};
static std::atomic<ULONGLONG> g_langManualUntil{0};
static std::atomic<int> g_langCensus{0};
static std::atomic<int> g_langSuppressions{0};
static std::atomic<int> g_langLogs{0};
static std::atomic<int> g_langManualLogs{0};
static std::atomic<bool> g_langEndLogged{false};

static bool IsOwnProcessWindow(HWND hwnd) {
    if (!hwnd || !IsWindow(hwnd)) return false;
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    return pid == GetCurrentProcessId();
}

static bool IsLanguageFlyoutClass(const wchar_t* cls) {
    if (!cls || (ULONG_PTR)cls <= 0xFFFF) return false;  // class atom, not a name
    return _wcsicmp(cls, L"Shell_InputSwitchTopLevelWindow") == 0 ||
           _wcsicmp(cls, L"Shell_InputSwitchDismissOverlay") == 0 ||
           _wcsicmp(cls, L"Windhawk_Win78LanguageFlyout") == 0;
}

static bool LooksLikeLanguageFlyout(HWND hwnd) {
    if (!IsOwnProcessWindow(hwnd)) return false;
    wchar_t cls[128] = {};
    return GetClassNameW(hwnd, cls, _countof(cls)) && IsLanguageFlyoutClass(cls);
}

static bool LanguageGuardActive() {
    if (g_unloading.load(std::memory_order_acquire) ||
        !g_langGuardEnabled.load(std::memory_order_relaxed)) {
        return false;
    }
    const ULONGLONG until = g_langGuardUntil.load(std::memory_order_acquire);
    return until != 0 && GetTickCount64() < until;
}

// A suppression extends the guard by 15 seconds, with an absolute cap of four
// times the configured duration from the original guard start.
static void ExtendLanguageGuard() {
    const ULONGLONG start = g_langGuardStart.load(std::memory_order_acquire);
    const ULONGLONG now = GetTickCount64();
    const ULONGLONG cap = start +
        (ULONGLONG)g_langGuardMs.load(std::memory_order_relaxed) * 4;
    if (now >= cap || !g_langGuardEnabled.load(std::memory_order_relaxed) ||
        !g_langGuardUntil.load(std::memory_order_acquire)) return;
    ULONGLONG extended = now + 15000;
    if (extended > cap) extended = cap;

    ULONGLONG current = g_langGuardUntil.load(std::memory_order_acquire);
    while (current < extended &&
           !g_langGuardUntil.compare_exchange_weak(current, extended,
                                                    std::memory_order_acq_rel,
                                                    std::memory_order_acquire)) {
    }
}

static bool LanguageManualOpenActive() {
    const ULONGLONG until = g_langManualUntil.load(std::memory_order_acquire);
    return until != 0 && GetTickCount64() < until;
}

static void NoteManualLanguageOpen(const wchar_t* how) {
    if (!LanguageGuardActive()) return;
    g_langManualUntil.store(GetTickCount64() + 5000, std::memory_order_release);
    if (g_langManualLogs.fetch_add(1, std::memory_order_relaxed) < 4) {
        Wh_Log(L"[language] %s: the guard lets the flyout through for five seconds", how);
    }
}

typedef BOOL(WINAPI* ShowWindow_t)(HWND, int);
static ShowWindow_t ShowWindow_Original = nullptr;
typedef BOOL(WINAPI* ShowWindowAsync_t)(HWND, int);
static ShowWindowAsync_t ShowWindowAsync_Original = nullptr;
typedef BOOL(WINAPI* SetWindowPos_t)(HWND, HWND, int, int, int, int, UINT);
static SetWindowPos_t SetWindowPos_Original = nullptr;
typedef HWND(WINAPI* CreateWindowExW_t)(DWORD, LPCWSTR, LPCWSTR, DWORD, int, int, int, int,
                                        HWND, HMENU, HINSTANCE, LPVOID);
static CreateWindowExW_t CreateWindowExW_Original = nullptr;

static void LogLanguageEvent(const wchar_t* how, HWND hwnd) {
    const int logIndex = g_langLogs.fetch_add(1, std::memory_order_relaxed);
    if (logIndex >= 12) return;
    wchar_t cls[128] = {};
    GetClassNameW(hwnd, cls, _countof(cls));
    Wh_Log(L"[language] %s: class %s (pid %lu)", how, cls, GetCurrentProcessId());
}

// Hide asynchronously: even a stalled window thread cannot keep the guard worker
// from exiting during mod unload. No WM_CLOSE or cross-process window operation is used.
static void SuppressLanguageWindow(HWND hwnd, const wchar_t* how) {
    if (!LooksLikeLanguageFlyout(hwnd)) return;
    g_langSuppressions.fetch_add(1, std::memory_order_relaxed);
    LogLanguageEvent(how, hwnd);

    if (ShowWindowAsync_Original) {
        ShowWindowAsync_Original(hwnd, SW_HIDE);
    } else if (SetWindowPos_Original) {
        SetWindowPos_Original(hwnd, nullptr, 0, 0, 0, 0,
                              SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER |
                                  SWP_NOACTIVATE | SWP_HIDEWINDOW | SWP_ASYNCWINDOWPOS);
    }
    ExtendLanguageGuard();
}

static BOOL WINAPI ShowWindow_Hook(HWND hwnd, int cmd) {
    if (cmd != SW_HIDE && !LanguageManualOpenActive() && LanguageGuardActive() &&
        LooksLikeLanguageFlyout(hwnd)) {
        SuppressLanguageWindow(hwnd, L"show blocked (ShowWindow)");
        return TRUE;
    }
    return ShowWindow_Original(hwnd, cmd);
}

static BOOL WINAPI ShowWindowAsync_Hook(HWND hwnd, int cmd) {
    if (cmd != SW_HIDE && !LanguageManualOpenActive() && LanguageGuardActive() &&
        LooksLikeLanguageFlyout(hwnd)) {
        SuppressLanguageWindow(hwnd, L"show blocked (ShowWindowAsync)");
        return TRUE;
    }
    return ShowWindowAsync_Original(hwnd, cmd);
}

static BOOL WINAPI SetWindowPos_Hook(HWND hwnd, HWND after, int x, int y, int cx, int cy, UINT flags) {
    if ((flags & SWP_SHOWWINDOW) && !LanguageManualOpenActive() && LanguageGuardActive() &&
        LooksLikeLanguageFlyout(hwnd)) {
        SuppressLanguageWindow(hwnd, L"show blocked (SetWindowPos)");
        flags &= ~SWP_SHOWWINDOW;
        if (!flags) return TRUE;
    }
    return SetWindowPos_Original(hwnd, after, x, y, cx, cy, flags);
}

static bool ShouldTrackIndicatorWindows() {
    return !g_unloading.load(std::memory_order_acquire) &&
           (g_langGuardEnabled.load(std::memory_order_relaxed) ||
            g_indicatorColours.load(std::memory_order_relaxed) ||
            g_routeRightClickToTaskbar.load(std::memory_order_relaxed));
}

static HWND WINAPI CreateWindowExW_Hook(DWORD exStyle, LPCWSTR cls, LPCWSTR title, DWORD style,
                                        int x, int y, int w, int h, HWND parent, HMENU menu,
                                        HINSTANCE inst, LPVOID param) {
    const bool suppressAtCreation = (style & WS_VISIBLE) &&
                                    !LanguageManualOpenActive() && LanguageGuardActive() &&
                                    IsLanguageFlyoutClass(cls);
    if (suppressAtCreation) style &= ~WS_VISIBLE;

    HWND created = CreateWindowExW_Original(exStyle, cls, title, style, x, y, w, h,
                                            parent, menu, inst, param);
    if (!created || g_unloading.load(std::memory_order_acquire)) return created;

    if (ShouldTrackIndicatorWindows()) TrackIndicatorWindow(created);
    if (!LanguageManualOpenActive() && LanguageGuardActive() &&
        LooksLikeLanguageFlyout(created)) {
        SuppressLanguageWindow(created, L"window created during the guard");
    }
    return created;
}

static BOOL CALLBACK LangCensusProc(HWND hwnd, LPARAM logCensus) {
    if (!IsOwnProcessWindow(hwnd)) return TRUE;

    const bool visible = IsWindowVisible(hwnd) != 0;
    wchar_t cls[128] = {};
    if (!GetClassNameW(hwnd, cls, _countof(cls))) return TRUE;

    if (logCensus && visible && g_langCensusLog.load(std::memory_order_relaxed)) {
        const int censusIndex = g_langCensus.fetch_add(1, std::memory_order_relaxed);
        if (censusIndex < 40) {
            Wh_Log(L"[language] visible window class at guard start: %s", cls);
        }
    }

    if (visible && !LanguageManualOpenActive() && LanguageGuardActive() &&
        IsLanguageFlyoutClass(cls)) {
        SuppressLanguageWindow(hwnd, L"flyout visible during the guard");
    }
    return TRUE;
}

static void RunLanguageGuardCensus() {
    if (!LanguageGuardActive()) return;
    EnumWindows(LangCensusProc, 1);
}

// --- 10-bis) the language indicator: manual clicks and one authoritative paint --
// When colors are enabled, WM_PAINT/WM_PRINTCLIENT are fully handled by this
// subclass. Explorer's legacy paint is not forwarded, otherwise its grey fill can
// cover our theme-aware transparent composition, especially on non-classic themes.


static const DWORD_PTR kIndicatorSubclassRefData = 78;
static constexpr int kMaxIndicatorTargets = 32;
static SRWLOCK g_indicatorTargetsLock = SRWLOCK_INIT;
static std::atomic<int> g_indicatorCellLogs{0};
static std::atomic<int> g_indicatorInstallCount{0};

// IMEModeButton is deliberately excluded: it is the separate IME-mode control
// (A/あ, 中/英), not a language indicator layer.
static const wchar_t* const kIndicatorClasses[] = {
    L"TrayInputIndicatorWClass",
    L"InputIndicatorWClass",
    L"InputIndicatorButton",
};

struct IndicatorTarget {
    HWND wnd;
    bool subclassed;
    bool installing;
};
static IndicatorTarget g_indicatorTargets[kMaxIndicatorTargets] = {};
static int g_indicatorTargetCount = 0;

static LRESULT IndicatorSubclassProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam,
                                     DWORD_PTR refData);

static bool IsIndicatorClass(const wchar_t* cls) {
    if (!cls || !cls[0]) return false;
    for (const wchar_t* candidate : kIndicatorClasses) {
        if (_wcsicmp(cls, candidate) == 0) return true;
    }
    return false;
}

static int FindIndicatorTargetLocked(HWND hwnd) {
    for (int i = 0; i < g_indicatorTargetCount; i++) {
        if (g_indicatorTargets[i].wnd == hwnd) return i;
    }
    return -1;
}

static bool IsTrackedIndicator(HWND hwnd) {
    AcquireSRWLockShared(&g_indicatorTargetsLock);
    const bool tracked = FindIndicatorTargetLocked(hwnd) >= 0;
    ReleaseSRWLockShared(&g_indicatorTargetsLock);
    return tracked;
}

static int CopyIndicatorTargets(HWND* out, int capacity) {
    if (!out || capacity <= 0) return 0;
    AcquireSRWLockShared(&g_indicatorTargetsLock);
    const int count = g_indicatorTargetCount < capacity ? g_indicatorTargetCount : capacity;
    for (int i = 0; i < count; i++) out[i] = g_indicatorTargets[i].wnd;
    ReleaseSRWLockShared(&g_indicatorTargetsLock);
    return count;
}

// Invalidate only tracked indicator windows. Windows coalesces these requests
// into WM_PAINT; the subclass is the sole painter whenever the color option is on.
static void InvalidateIndicatorTargets() {
    if (g_unloading.load(std::memory_order_acquire)) return;
    HWND targets[kMaxIndicatorTargets] = {};
    const int count = CopyIndicatorTargets(targets, _countof(targets));
    for (int i = 0; i < count; i++) {
        if (IsOwnProcessWindow(targets[i]) && IsWindowVisible(targets[i])) {
            InvalidateRect(targets[i], nullptr, FALSE);
        }
    }
}

static void UntrackIndicatorWindow(HWND hwnd) {
    AcquireSRWLockExclusive(&g_indicatorTargetsLock);
    const int index = FindIndicatorTargetLocked(hwnd);
    if (index >= 0) {
        for (int i = index; i + 1 < g_indicatorTargetCount; i++) {
            g_indicatorTargets[i] = g_indicatorTargets[i + 1];
        }
        g_indicatorTargets[--g_indicatorTargetCount] = IndicatorTarget{};
    }
    ReleaseSRWLockExclusive(&g_indicatorTargetsLock);
}

static void TrackIndicatorWindow(HWND hwnd) {
    if (!ShouldTrackIndicatorWindows() || !IsOwnProcessWindow(hwnd)) return;

    wchar_t cls[64] = {};
    if (!GetClassNameW(hwnd, cls, _countof(cls)) || !IsIndicatorClass(cls)) return;

    bool shouldInstall = false;
    AcquireSRWLockExclusive(&g_indicatorTargetsLock);
    if (g_unloading.load(std::memory_order_acquire) || !ShouldTrackIndicatorWindows()) {
        ReleaseSRWLockExclusive(&g_indicatorTargetsLock);
        return;
    }
    int index = FindIndicatorTargetLocked(hwnd);
    if (index < 0) {
        if (g_indicatorTargetCount >= kMaxIndicatorTargets) {
            ReleaseSRWLockExclusive(&g_indicatorTargetsLock);
            return;
        }
        index = g_indicatorTargetCount++;
        g_indicatorTargets[index] = { hwnd, false, false };
    }
    if (!g_indicatorTargets[index].subclassed && !g_indicatorTargets[index].installing) {
        g_indicatorTargets[index].installing = true;
        g_indicatorInstallCount.fetch_add(1, std::memory_order_acq_rel);
        shouldInstall = true;
    }
    ReleaseSRWLockExclusive(&g_indicatorTargetsLock);

    if (!shouldInstall) return;
    const bool installed = WindhawkUtils::SetWindowSubclassFromAnyThread(
        hwnd, IndicatorSubclassProc, kIndicatorSubclassRefData);

    AcquireSRWLockExclusive(&g_indicatorTargetsLock);
    index = FindIndicatorTargetLocked(hwnd);
    if (index >= 0) {
        g_indicatorTargets[index].installing = false;
        g_indicatorTargets[index].subclassed = installed;
    }
    ReleaseSRWLockExclusive(&g_indicatorTargetsLock);
    g_indicatorInstallCount.fetch_sub(1, std::memory_order_acq_rel);

    if (installed && g_indicatorColours.load(std::memory_order_relaxed)) {
        // Ensure an already-created cell gets its first mod-owned paint even if
        // Explorer has no pending update region yet.
        InvalidateRect(hwnd, nullptr, FALSE);
    } else if (!installed && IsWindow(hwnd)) {
        Wh_Log(L"[lang] failed to subclass indicator window %s 0x%p", cls, (void*)hwnd);
    }
}

static BOOL CALLBACK IndicatorEnumProc(HWND hwnd, LPARAM) {
    TrackIndicatorWindow(hwnd);
    return TRUE;
}

static BOOL CALLBACK TaskbarEnumProc(HWND hwnd, LPARAM) {
    if (!IsOwnProcessWindow(hwnd)) return TRUE;
    wchar_t cls[64] = {};
    if (!GetClassNameW(hwnd, cls, _countof(cls))) return TRUE;
    if (_wcsicmp(cls, L"Shell_TrayWnd") == 0 ||
        _wcsicmp(cls, L"Shell_SecondaryTrayWnd") == 0) {
        EnumChildWindows(hwnd, IndicatorEnumProc, 0);
    }
    return TRUE;
}

// Discover current indicator windows at init/settings changes; later windows are
// caught by CreateWindowExW_Hook on their creating thread.
static void ArmIndicatorSubclass() {
    // TaskbarEnumProc walks both the primary and secondary taskbars and tracks
    // every indicator child, so a separate FindWindow-based lookup is unnecessary.
    EnumWindows(TaskbarEnumProc, 0);
}

static void RemoveIndicatorSubclasses() {
    // Wh_ModBeforeUninit prevents new installs. Wait for any installation that
    // already started so its subclass cannot outlive the cleanup snapshot.
    while (g_indicatorInstallCount.load(std::memory_order_acquire) != 0) Sleep(1);

    IndicatorTarget snapshot[kMaxIndicatorTargets] = {};
    int count = 0;
    AcquireSRWLockShared(&g_indicatorTargetsLock);
    count = g_indicatorTargetCount;
    for (int i = 0; i < count; i++) snapshot[i] = g_indicatorTargets[i];
    ReleaseSRWLockShared(&g_indicatorTargetsLock);

    for (int i = 0; i < count; i++) {
        if (snapshot[i].subclassed && IsOwnProcessWindow(snapshot[i].wnd)) {
            WindhawkUtils::RemoveWindowSubclassFromAnyThread(
                snapshot[i].wnd, IndicatorSubclassProc);
        }
    }

    AcquireSRWLockExclusive(&g_indicatorTargetsLock);
    memset(g_indicatorTargets, 0, sizeof(g_indicatorTargets));
    g_indicatorTargetCount = 0;
    ReleaseSRWLockExclusive(&g_indicatorTargetsLock);
}

class ScopedMemDc {
public:
    explicit ScopedMemDc(HDC src) : m_dc(CreateCompatibleDC(src)) {}
    ~ScopedMemDc() { if (m_dc) DeleteDC(m_dc); }
    ScopedMemDc(const ScopedMemDc&) = delete;
    ScopedMemDc& operator=(const ScopedMemDc&) = delete;
    HDC get() const { return m_dc; }
    bool valid() const { return m_dc != nullptr; }
private:
    HDC m_dc;
};

static HWND TopmostTrackedAt(POINT point) {
    HWND wnd = WindowFromPoint(point);
    for (int depth = 0; wnd && depth < 32; depth++) {
        if (IsTrackedIndicator(wnd)) return wnd;
        wnd = GetParent(wnd);
    }
    return nullptr;
}

static bool PrimaryIndicatorRect(RECT* out) {
    if (!out) return false;
    HWND targets[kMaxIndicatorTargets] = {};
    const int count = CopyIndicatorTargets(targets, _countof(targets));
    for (int pass = 0; pass < 2; pass++) {
        for (int i = 0; i < count; i++) {
            HWND wnd = targets[i];
            if (!wnd || !IsWindow(wnd) || !IsWindowVisible(wnd)) continue;
            if (pass == 0) {
                wchar_t cls[64] = {};
                if (!GetClassNameW(wnd, cls, _countof(cls))) continue;
                if (_wcsicmp(cls, L"TrayInputIndicatorWClass") != 0 &&
                    _wcsicmp(cls, L"InputIndicatorWClass") != 0) continue;
            }
            if (GetWindowRect(wnd, out)) return true;
        }
    }
    return false;
}

static bool ThisLayerShowsText(HWND hwnd) {
    RECT area = {};
    if (!PrimaryIndicatorRect(&area)) return true;
    POINT centre = { (area.left + area.right) / 2, (area.top + area.bottom) / 2 };
    HWND top = TopmostTrackedAt(centre);
    return !top || top == hwnd;
}

// Resolve the active layout at paint time instead of caching text from Explorer's
// internal GDI passes; that avoids retaining a partial hover string such as "ITA"
// without its matching country code.
static bool GetIndicatorText(HWND hwnd, wchar_t* text, size_t textCount,
                             wchar_t* text2, size_t text2Count) {
    if (!hwnd || !text || textCount < 4 || !text2 || text2Count < 4) return false;
    text[0] = text2[0] = 0;

    const DWORD threadId = GetWindowThreadProcessId(hwnd, nullptr);
    const HKL layout = GetKeyboardLayout(threadId);
    if (!layout) return false;
    const LANGID lang = (LANGID)LOWORD((UINT_PTR)layout);
    if (!GetLocaleInfoW(MAKELCID(lang, SORT_DEFAULT), LOCALE_SABBREVLANGNAME,
                        text, (int)textCount) || !text[0]) {
        return false;
    }
    GetLocaleInfoW(MAKELCID(lang, SORT_DEFAULT), LOCALE_SISO3166CTRYNAME,
                   text2, (int)text2Count);
    return true;
}

static void PaintIndicatorCell(HWND hwnd, const wchar_t* why, HDC targetDc) {
    if (g_unloading.load(std::memory_order_acquire) ||
        !g_indicatorColours.load(std::memory_order_relaxed) ||
        !hwnd || !targetDc || !IsWindow(hwnd) || !IsWindowVisible(hwnd) ||
        !IsOwnProcessWindow(hwnd) || !IsTrackedIndicator(hwnd)) {
        return;
    }

    wchar_t cls[64] = {};
    if (!GetClassNameW(hwnd, cls, _countof(cls)) || !IsIndicatorClass(cls)) return;

    RECT rc = {};
    if (!GetClientRect(hwnd, &rc)) return;
    const int width = rc.right - rc.left;
    const int height = rc.bottom - rc.top;
    if (width < 4 || height < 4 || width > 256 || height > 256) return;

    const bool showsText = ThisLayerShowsText(hwnd);
    wchar_t text[16] = {};
    wchar_t text2[16] = {};
    if (showsText && !GetIndicatorText(hwnd, text, _countof(text), text2, _countof(text2))) return;

    // Keep the caller's HDC state intact. WM_PAINT and WM_PRINTCLIENT can pass
    // Explorer-owned DCs that are reused for other taskbar content.
    ScopedSavedDc targetState(targetDc);
    if (!targetState.valid()) return;
    ScopedMemDc memoryDc(targetDc);
    if (!memoryDc.valid()) return;

    BITMAPINFO bitmapInfo = {};
    bitmapInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bitmapInfo.bmiHeader.biWidth = width;
    bitmapInfo.bmiHeader.biHeight = -height;
    bitmapInfo.bmiHeader.biPlanes = 1;
    bitmapInfo.bmiHeader.biBitCount = 32;
    bitmapInfo.bmiHeader.biCompression = BI_RGB;
    void* bits = nullptr;
    ScopedGdiObj bitmap((HGDIOBJ)CreateDIBSection(
        targetDc, &bitmapInfo, DIB_RGB_COLORS, &bits, nullptr, 0));
    if (!bitmap.get() || !bits) return;
    ScopedSelectedObject bitmapSelection(memoryDc.get(), bitmap.get());
    memset(bits, 0, (size_t)width * (size_t)height * 4);

    if (showsText) {
        LOGFONTW font = {};
        font.lfHeight = -((height * 28) / 100);
        font.lfWeight = FW_NORMAL;
        font.lfCharSet = DEFAULT_CHARSET;
        font.lfQuality = ANTIALIASED_QUALITY;
        font.lfPitchAndFamily = DEFAULT_PITCH | FF_SWISS;
        wcscpy_s(font.lfFaceName, LF_FACESIZE, L"Segoe UI");
        ScopedGdiObj fontObject((HGDIOBJ)CreateFontIndirectW(&font));
        if (fontObject.get()) {
            ScopedSelectedObject fontSelection(memoryDc.get(), fontObject.get());
            SetBkMode(memoryDc.get(), TRANSPARENT);
            SetTextColor(memoryDc.get(), RGB(255, 255, 255));
            TEXTMETRICW metrics = {};
            GetTextMetricsW(memoryDc.get(), &metrics);
            const int lineHeight = metrics.tmHeight > 0 ? metrics.tmHeight : 12;
            const int lineCount = text2[0] ? 2 : 1;
            const int y = rc.top + (height - lineHeight * lineCount) / 2;
            RECT first = { rc.left, y, rc.right, y + lineHeight };
            DrawTextW(memoryDc.get(), text, -1, &first,
                      DT_CENTER | DT_SINGLELINE | DT_NOPREFIX);
            if (lineCount == 2) {
                RECT second = { rc.left, y + lineHeight, rc.right, y + 2 * lineHeight };
                DrawTextW(memoryDc.get(), text2, -1, &second,
                          DT_CENTER | DT_SINGLELINE | DT_NOPREFIX);
            }
        }

        // Convert the white glyph coverage into premultiplied ARGB. The DIB's
        // zero-alpha pixels let the taskbar composition show through behind text.
        const COLORREF foreground = IndicatorTextColor();
        const unsigned fr = GetRValue(foreground);
        const unsigned fg = GetGValue(foreground);
        const unsigned fb = GetBValue(foreground);
        DWORD* pixels = (DWORD*)bits;
        const size_t pixelCount = (size_t)width * (size_t)height;
        for (size_t i = 0; i < pixelCount; i++) {
            const DWORD pixel = pixels[i];
            const unsigned r = (pixel >> 16) & 0xFF;
            const unsigned g = (pixel >> 8) & 0xFF;
            const unsigned b = pixel & 0xFF;
            unsigned alpha = r > g ? r : g;
            if (b > alpha) alpha = b;
            if (!alpha) {
                pixels[i] = 0;
                continue;
            }
            pixels[i] = ((DWORD)alpha << 24) |
                        ((DWORD)((fr * alpha + 127) / 255) << 16) |
                        ((DWORD)((fg * alpha + 127) / 255) << 8) |
                        (DWORD)((fb * alpha + 127) / 255);
        }
    }

    if (!g_indicatorColours.load(std::memory_order_relaxed) ||
        g_unloading.load(std::memory_order_acquire)) {
        return;
    }
    BitBlt(targetDc, 0, 0, width, height, memoryDc.get(), 0, 0, SRCCOPY);

    const int logIndex = g_indicatorCellLogs.fetch_add(1, std::memory_order_relaxed);
    if (logIndex < 8) {
        RECT windowRect = {};
        GetWindowRect(hwnd, &windowRect);
        Wh_Log(L"[lang] indicator repainted (%s): %s 0x%p %dx%d at (%d,%d), text %s",
               why, cls, (void*)hwnd, width, height, windowRect.left, windowRect.top,
               showsText ? text : L"(carried by topmost indicator layer)");
    }
}

static HWND GetIndicatorTaskbarWindow(HWND hwnd) {
    HWND tray = GetAncestor(hwnd, GA_ROOT);
    if (!tray || !IsOwnProcessWindow(tray)) return nullptr;
    wchar_t cls[64] = {};
    if (!GetClassNameW(tray, cls, _countof(cls))) return nullptr;
    if (_wcsicmp(cls, L"Shell_TrayWnd") != 0 &&
        _wcsicmp(cls, L"Shell_SecondaryTrayWnd") != 0) return nullptr;
    return tray;
}

static void RequestIndicatorRepaint() {
    if (g_indicatorColours.load(std::memory_order_relaxed) &&
        !g_unloading.load(std::memory_order_acquire)) {
        // Do not remove these state invalidations: a non-classic composited taskbar
        // otherwise keeps stale/legacy paint when hover, press, or layout changes.
        // Invalidate every layer because the topmost one can change on hover/press;
        // bErase=FALSE preserves the composed taskbar background.
        InvalidateIndicatorTargets();
    }
}

static void TrackIndicatorMouseLeave(HWND hwnd) {
    TRACKMOUSEEVENT tracking = {};
    tracking.cbSize = sizeof(tracking);
    tracking.dwFlags = TME_LEAVE;
    tracking.hwndTrack = hwnd;
    TrackMouseEvent(&tracking);
}

static LRESULT IndicatorSubclassProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam,
                                     DWORD_PTR refData) {
    (void)refData;
    if (msg == WM_NCDESTROY) {
        const LRESULT result = DefSubclassProc(hwnd, msg, wParam, lParam);
        UntrackIndicatorWindow(hwnd);
        return result;
    }
    if (g_unloading.load(std::memory_order_acquire)) {
        return DefSubclassProc(hwnd, msg, wParam, lParam);
    }

    const bool colorsEnabled = g_indicatorColours.load(std::memory_order_relaxed);
    bool repaintAfterDefault = false;
    switch (msg) {
        case WM_ERASEBKGND:
            if (colorsEnabled) return 1;
            break;

        case WM_PAINT:
            if (colorsEnabled) {
                // One authoritative paint: do not forward WM_PAINT to Explorer,
                // whose legacy themed fill would cover our composited rendering.
                // BeginPaint/EndPaint still validate the update region.
                PAINTSTRUCT paint = {};
                HDC hdc = BeginPaint(hwnd, &paint);
                if (hdc) PaintIndicatorCell(hwnd, L"paint", hdc);
                EndPaint(hwnd, &paint);
                return 0;
            }
            break;

        case WM_PRINTCLIENT:
            if (colorsEnabled) {
                HDC hdc = (HDC)wParam;
                if (hdc) PaintIndicatorCell(hwnd, L"printclient", hdc);
                return 0;
            }
            break;

        case WM_LBUTTONDOWN:
        case WM_LBUTTONUP:
            NoteManualLanguageOpen(L"click on the indicator");
            repaintAfterDefault = true;
            break;

        case WM_MBUTTONUP:
            NoteManualLanguageOpen(L"click on the indicator");
            repaintAfterDefault = true;
            break;

        case WM_RBUTTONDOWN: {
            if (g_routeRightClickToTaskbar.load(std::memory_order_relaxed)) {
                HWND tray = GetIndicatorTaskbarWindow(hwnd);
                if (tray) {
                    NoteManualLanguageOpen(L"right click on the indicator");
                    POINT point = {};
                    GetCursorPos(&point);
                    const LPARAM screenPoint = MAKELPARAM((SHORT)point.x, (SHORT)point.y);
                    PostMessageW(tray, WM_CONTEXTMENU, (WPARAM)tray, screenPoint);
                    RequestIndicatorRepaint();
                    return 0;
                }
            }
            repaintAfterDefault = true;
            break;  // default: keep the language indicator's native context menu
        }

        case WM_RBUTTONUP:
            if (g_routeRightClickToTaskbar.load(std::memory_order_relaxed) &&
                GetIndicatorTaskbarWindow(hwnd)) {
                RequestIndicatorRepaint();
                return 0;
            }
            repaintAfterDefault = true;
            break;

        case WM_MOUSEMOVE:
            TrackIndicatorMouseLeave(hwnd);
            repaintAfterDefault = true;
            break;

        case WM_MOUSELEAVE:
        case WM_MOUSEHOVER:
        case WM_MBUTTONDOWN:
        case WM_CAPTURECHANGED:
        case WM_CANCELMODE:
        case WM_ENABLE:
        case WM_SETFOCUS:
        case WM_KILLFOCUS:
        case WM_INPUTLANGCHANGE:
            repaintAfterDefault = true;
            break;

        case WM_THEMECHANGED:
        case WM_SYSCOLORCHANGE:
        case WM_SETTINGCHANGE:
            ResetThemeCache();
            repaintAfterDefault = true;
            break;
    }

    const LRESULT result = DefSubclassProc(hwnd, msg, wParam, lParam);
    if (repaintAfterDefault && colorsEnabled) RequestIndicatorRepaint();
    return result;
}

// The guard thread exists only while the guard is active. It sweeps this process's
// top-level windows, uses asynchronous hides, and exits as soon as the deadline passes.
static SRWLOCK g_languageThreadLock = SRWLOCK_INIT;
static HANDLE g_stopEvent = nullptr;
static HANDLE g_languageThread = nullptr;

static void InstallLanguageGuardHooks() {
    const bool hookedShow = Wh_SetFunctionHook((void*)ShowWindow, (void*)ShowWindow_Hook,
                                               (void**)&ShowWindow_Original);
    const bool hookedAsync = Wh_SetFunctionHook((void*)ShowWindowAsync, (void*)ShowWindowAsync_Hook,
                                                (void**)&ShowWindowAsync_Original);
    const bool hookedCreate = Wh_SetFunctionHook((void*)CreateWindowExW, (void*)CreateWindowExW_Hook,
                                                 (void**)&CreateWindowExW_Original);
    const bool hookedPos = Wh_SetFunctionHook((void*)SetWindowPos, (void*)SetWindowPos_Hook,
                                              (void**)&SetWindowPos_Original);
    Wh_Log(L"[language] guard hooks: ShowWindow %s, ShowWindowAsync %s, CreateWindowExW %s, SetWindowPos %s",
           hookedShow ? L"installed" : L"not installed",
           hookedAsync ? L"installed" : L"not installed",
           hookedCreate ? L"installed" : L"not installed",
           hookedPos ? L"installed" : L"not installed");
}

static void ArmLanguageGuard() {
    const ULONGLONG now = GetTickCount64();
    const DWORD duration = g_langGuardMs.load(std::memory_order_relaxed);
    g_langGuardStart.store(now, std::memory_order_release);
    g_langGuardUntil.store(now + duration, std::memory_order_release);
    g_langManualUntil.store(0, std::memory_order_release);
    g_langSuppressions.store(0, std::memory_order_relaxed);
    g_langLogs.store(0, std::memory_order_relaxed);
    g_langCensus.store(0, std::memory_order_relaxed);
    g_langManualLogs.store(0, std::memory_order_relaxed);
    g_langEndLogged.store(false, std::memory_order_release);
    Wh_Log(L"[language] guard armed for %u seconds", (unsigned)(duration / 1000));
}

static void DisarmLanguageGuard() {
    g_langGuardUntil.store(0, std::memory_order_release);
    Wh_Log(L"[language] guard off (the indicator colors stay on)");
}

static void LogLanguageGuardFinished() {
    if (!g_langEndLogged.exchange(true, std::memory_order_acq_rel)) {
        Wh_Log(L"[language] guard finished: %d interventions",
               g_langSuppressions.load(std::memory_order_relaxed));
    }
}

static DWORD WINAPI LanguageGuardThread(LPVOID) {
    bool firstCensus = true;
    for (;;) {
        if (g_unloading.load(std::memory_order_acquire) ||
            WaitForSingleObject(g_stopEvent, 0) == WAIT_OBJECT_0 ||
            !g_langGuardEnabled.load(std::memory_order_relaxed)) {
            break;
        }

        if (!LanguageGuardActive()) {
            if (g_langGuardUntil.load(std::memory_order_acquire)) {
                LogLanguageGuardFinished();
            }
            break;
        }

        // Only the language guard is polled here. Indicator paints are driven by
        // WM_PAINT and explicit state-change invalidations, never by a timer.
        EnumWindows(LangCensusProc, firstCensus ? 1 : 0);
        firstCensus = false;
        if (!LanguageGuardActive()) {
            if (g_langGuardEnabled.load(std::memory_order_relaxed) &&
                g_langGuardUntil.load(std::memory_order_acquire)) {
                LogLanguageGuardFinished();
            }
            break;
        }
        if (WaitForSingleObject(g_stopEvent, 500) == WAIT_OBJECT_0) break;
    }
    return 0;
}

static bool StartLanguageGuardThread() {
    AcquireSRWLockExclusive(&g_languageThreadLock);
    if (g_unloading.load(std::memory_order_acquire)) {
        ReleaseSRWLockExclusive(&g_languageThreadLock);
        return false;
    }

    if (g_languageThread) {
        const DWORD state = WaitForSingleObject(g_languageThread, 0);
        if (state == WAIT_TIMEOUT) {
            ReleaseSRWLockExclusive(&g_languageThreadLock);
            return true;
        }
        CloseHandle(g_languageThread);
        g_languageThread = nullptr;
    }

    if (!g_stopEvent) {
        g_stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
        if (!g_stopEvent) {
            ReleaseSRWLockExclusive(&g_languageThreadLock);
            Wh_Log(L"[language] failed to create the guard stop event (%lu)", GetLastError());
            return false;
        }
    } else {
        ResetEvent(g_stopEvent);
    }

    g_languageThread = CreateThread(nullptr, 0, LanguageGuardThread, nullptr, 0, nullptr);
    if (!g_languageThread) {
        Wh_Log(L"[language] failed to create the guard thread (%lu)", GetLastError());
    }
    const bool started = g_languageThread != nullptr;
    ReleaseSRWLockExclusive(&g_languageThreadLock);
    return started;
}

static void StopLanguageGuardThread() {
    AcquireSRWLockExclusive(&g_languageThreadLock);
    if (g_stopEvent) SetEvent(g_stopEvent);
    if (g_languageThread) {
        // The guard worker enumerates same-process windows and hides them through
        // asynchronous APIs, so it never waits on another window thread.
        WaitForSingleObject(g_languageThread, INFINITE);
        CloseHandle(g_languageThread);
        g_languageThread = nullptr;
    }
    if (g_stopEvent) {
        CloseHandle(g_stopEvent);
        g_stopEvent = nullptr;
    }
    ReleaseSRWLockExclusive(&g_languageThreadLock);
}

static void LoadLanguageSettings() {
    const bool guardEnabled = Wh_GetIntSetting(L"LanguageGuard") != 0;
    int seconds = Wh_GetIntSetting(L"LanguageGuardSeconds");
    if (seconds < 2) seconds = 2;
    if (seconds > 600) seconds = 600;

    g_langGuardEnabled.store(guardEnabled, std::memory_order_release);
    g_langGuardMs.store((DWORD)seconds * 1000, std::memory_order_release);
    g_indicatorColours.store(Wh_GetIntSetting(L"LanguageIndicatorColours") != 0,
                             std::memory_order_release);
    g_routeRightClickToTaskbar.store(
        Wh_GetIntSetting(L"LanguageIndicatorRightClickOpensTaskbarMenu") != 0,
        std::memory_order_release);
    g_langCensusLog.store(Wh_GetIntSetting(L"LogLanguageGuard") != 0,
                          std::memory_order_release);
}

static void LogCurrentSettings(const wchar_t* prefix) {
    Wh_Log(L"[lang] %s: guard=%s (%u s), indicator colors=%s, taskbar menu on right-click=%s, logon census=%s",
           prefix,
           g_langGuardEnabled.load(std::memory_order_relaxed) ? L"on" : L"off",
           (unsigned)(g_langGuardMs.load(std::memory_order_relaxed) / 1000),
           g_indicatorColours.load(std::memory_order_relaxed) ? L"on" : L"off",
           g_routeRightClickToTaskbar.load(std::memory_order_relaxed) ? L"on" : L"off",
           g_langCensusLog.load(std::memory_order_relaxed) ? L"on" : L"off");
}

BOOL Wh_ModInit() {
    g_unloading.store(false, std::memory_order_release);
    LoadLanguageSettings();
    Wh_Log(L"[lang] init: Windows 10 language flyout guard and indicator colors");
    LogCurrentSettings(L"settings");

    // Flyout interception is runtime-gated. Indicator color rendering is
    // handled exclusively by its WM_PAINT subclass; no competing GDI hooks exist.
    InstallLanguageGuardHooks();
    if (ShouldTrackIndicatorWindows()) ArmIndicatorSubclass();

    const bool guardEnabled = g_langGuardEnabled.load(std::memory_order_relaxed);
    if (guardEnabled) {
        ArmLanguageGuard();
        if (!StartLanguageGuardThread()) {
            Wh_Log(L"[language] guard thread unavailable; running the startup census synchronously");
            RunLanguageGuardCensus();
        }
    }
    return TRUE;
}

void Wh_ModBeforeUninit() {
    g_unloading.store(true, std::memory_order_release);
    // Synchronize with TrackIndicatorWindow's in-flight check before shutdown.
    AcquireSRWLockExclusive(&g_indicatorTargetsLock);
    ReleaseSRWLockExclusive(&g_indicatorTargetsLock);
    AcquireSRWLockExclusive(&g_languageThreadLock);
    if (g_stopEvent) SetEvent(g_stopEvent);
    ReleaseSRWLockExclusive(&g_languageThreadLock);
}

void Wh_ModUninit() {
    g_unloading.store(true, std::memory_order_release);
    StopLanguageGuardThread();
    // Remove every subclass before Windhawk unloads this image. Otherwise the next
    // message to an indicator could jump into unmapped code.
    RemoveIndicatorSubclasses();
    Wh_Log(L"[lang] unloaded");
}

void Wh_ModSettingsChanged() {
    const bool previousGuard = g_langGuardEnabled.load(std::memory_order_acquire);
    const DWORD previousDuration = g_langGuardMs.load(std::memory_order_acquire);
    const bool previousCensusLog = g_langCensusLog.load(std::memory_order_acquire);
    const bool previousColours = g_indicatorColours.load(std::memory_order_acquire);

    LoadLanguageSettings();
    const bool colorsEnabled = g_indicatorColours.load(std::memory_order_acquire);
    const bool guardEnabled = g_langGuardEnabled.load(std::memory_order_acquire);
    const DWORD duration = g_langGuardMs.load(std::memory_order_acquire);

    if (!guardEnabled) {
        DisarmLanguageGuard();
        StopLanguageGuardThread();
    } else if (!previousGuard ||
               (previousDuration != duration && LanguageGuardActive())) {
        ArmLanguageGuard();
        if (!StartLanguageGuardThread()) RunLanguageGuardCensus();
    } else if (!previousCensusLog &&
               g_langCensusLog.load(std::memory_order_relaxed) && LanguageGuardActive()) {
        RunLanguageGuardCensus();
    }

    if (ShouldTrackIndicatorWindows()) ArmIndicatorSubclass();
    if (previousColours != colorsEnabled) {
        // Switching the option changes who owns WM_PAINT. Invalidate once so
        // either our single painter or Explorer's native painter takes over.
        ResetThemeCache();
        InvalidateIndicatorTargets();
    }

    LogCurrentSettings(L"settings reloaded");
}
