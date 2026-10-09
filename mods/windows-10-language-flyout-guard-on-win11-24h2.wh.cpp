// ==WindhawkMod==
// @id              windows-10-language-flyout-guard-on-win11-24h2
// @name            Windows 10 language flyout guard and indicator colours
// @description     This mod hides the startup language flyout in the private Windows 10 shell on Windows 11 24H2+
// @version         1.0.0
// @author          babamohammed
// @github          https://github.com/babamohammed2022
// @architecture    x86-64
// @compilerOptions -ladvapi32 -lcomctl32 -lgdi32 -luser32
// @include         explorer.exe
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Windows 10 language flyout guard and indicator colours

This mod addresses two issues that can occur when the private Windows 10 shell is used by the Windows 10 taskbar mod on Windows 11 24H2 and later:

- **A language flyout that opens at startup.** For a limited time after the shell starts, the mod suppresses known language-switcher window classes in this Explorer process. It does not enumerate or hide windows owned by other processes. Clicking the language indicator manually is allowed through the guard.
- **The language indicator's orientation and appearance.** The optional colour feature uses one theme-aware painter for the indicator's client area and reads the full active keyboard-layout ID, so variants such as Italian (142) display as `ITA 142` rather than being collapsed to `ITA IT`. It handles paint itself rather than allowing Explorer's legacy grey fill to overwrite the composited background, and it repaints on hover, press, theme, and layout state changes.

### Before

![Before](https://raw.githubusercontent.com/babamohammed2022/babamohammed2022/main/before.png)

### After

![After](https://raw.githubusercontent.com/babamohammed2022/babamohammed2022/main/after.png)

The guard lasts 20 seconds by default. Each intercepted flyout can extend it by 15 seconds, up to four times the configured duration. No window is closed or destroyed.

**Startup reliability.** When Explorer starts, Windhawk loads the mod before the taskbar and the language indicator exist, and window-creation hooks only become active after `Wh_ModInit` returns. A background rescan therefore attaches to the indicator as soon as it appears (every 250 ms, for the first three minutes only, after which it stops), so the mod no longer needs to be toggled manually after logon.

The mod is loaded into `explorer.exe` processes but skips initialization in the system `%SystemRoot%\explorer.exe` (checked in code using the real image path, which Fake Explorer path does not alter), so it only acts in the private Windows 10 shell. The flyout sweep is limited to the current process. System files are not replaced.

**Overlap with "Fix language indicator in Win10 taskbar under Win11 24H2+".** With `LanguageIndicatorColours` enabled, this mod paints the language indicator itself and Explorer's native paint (which that mod hooks) no longer runs for it, so you do not need both for the indicator. Disable `LanguageIndicatorColours` if you prefer to keep using the other mod for the indicator.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- LanguageGuard: true
  $name: Hide the language flyout at logon
  $description: >-
    This setting suppresses known language flyouts in this Explorer process during
    startup. A click on the language indicator lets the flyout through temporarily.
- LanguageGuardSeconds: 20
  $name: Guard duration (seconds)
  $description: >-
    This setting controls how long the guard watches for the flyout. Each intercepted
    flyout extends it by 15 seconds, up to four times this duration.
- LanguageIndicatorColours: true
  $name: Windows 10 colours of the language indicator
  $description: >-
    This setting draws the active language and full keyboard-layout variant horizontally
    with a theme-appropriate colour (for example, ITA 142 instead of ITA IT). The
    transparent background relies on taskbar transparency composition.
- LanguageIndicatorRightClickOpensTaskbarMenu: false
  $name: Open the taskbar menu on indicator right-click
  $description: >-
    This setting is off by default. When enabled, right-clicking the language indicator
    opens the taskbar context menu instead of the indicator's native context menu.
- LogLanguageGuard: false
  $name: Log the windows of the logon
  $description: >-
    This setting logs visible window classes in this Explorer process once when the
    guard starts, and logs matching windows that it suppresses.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <commctrl.h>
#include <windhawk_api.h>
#include <windhawk_utils.h>
#include <atomic>
#include <string.h>
#include <wchar.h>

static std::atomic<bool> g_unloading{false};
static std::atomic<ULONGLONG> g_modStartTick{0};

static std::atomic<bool> g_langGuardEnabled{true};
static std::atomic<DWORD> g_langGuardMs{20000};
static std::atomic<bool> g_indicatorColours{true};
static std::atomic<bool> g_routeRightClickToTaskbar{false};
static std::atomic<bool> g_langCensusLog{false};

static void TrackIndicatorWindow(HWND hwnd);

// --- RAII helpers -----------------------------------------------------------
// Every scoped class owns one resource and releases it in its destructor, so
// error paths and early returns never leak. The classes are non-copyable to
// prevent accidental double-release.

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

// Owns a registry HKEY. put() closes any previously held key and returns the
// address of the slot so RegOpenKeyExW can write into it directly; this keeps
// call sites readable and closes on every exit path, including early returns
// inside the enum loop.
class ScopedRegKey {
public:
    ScopedRegKey() : m_key(nullptr) {}
    ~ScopedRegKey() { if (m_key) RegCloseKey(m_key); }
    ScopedRegKey(const ScopedRegKey&) = delete;
    ScopedRegKey& operator=(const ScopedRegKey&) = delete;
    HKEY* put() { reset(); return &m_key; }
    HKEY get() const { return m_key; }
    void reset() { if (m_key) { RegCloseKey(m_key); m_key = nullptr; } }
    bool valid() const { return m_key != nullptr; }
private:
    HKEY m_key;
};

// Owns a generic kernel HANDLE (event, thread, ...). Move-only so it can be
// returned from a factory while still guaranteeing release on every path.
class ScopedHandle {
public:
    ScopedHandle() : m_h(nullptr) {}
    explicit ScopedHandle(HANDLE h) : m_h(h) {}
    ~ScopedHandle() { if (m_h) CloseHandle(m_h); }
    ScopedHandle(const ScopedHandle&) = delete;
    ScopedHandle& operator=(const ScopedHandle&) = delete;
    ScopedHandle(ScopedHandle&& o) noexcept : m_h(o.m_h) { o.m_h = nullptr; }
    ScopedHandle& operator=(ScopedHandle&& o) noexcept {
        if (this != &o) { reset(); m_h = o.m_h; o.m_h = nullptr; }
        return *this;
    }
    HANDLE get() const { return m_h; }
    HANDLE* put() { reset(); return &m_h; }
    HANDLE release() { HANDLE h = m_h; m_h = nullptr; return h; }
    void reset(HANDLE h = nullptr) { if (m_h && m_h != h) CloseHandle(m_h); m_h = h; }
    bool valid() const { return m_h != nullptr; }
private:
    HANDLE m_h;
};

// --- Theme colours ----------------------------------------------------------
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
    return TaskbarUsesLightTheme() ? RGB(28, 28, 28) : RGB(255, 255, 255);
}

// --- Language flyout guard --------------------------------------------------
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
    if (!cls || (ULONG_PTR)cls <= 0xFFFF) return false;
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

// --- Language indicator tracking --------------------------------------------
static const DWORD_PTR kIndicatorSubclassRefData = 78;
static constexpr int kMaxIndicatorTargets = 32;
static SRWLOCK g_indicatorTargetsLock = SRWLOCK_INIT;
static std::atomic<int> g_indicatorCellLogs{0};
static std::atomic<int> g_indicatorInstallCount{0};

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

// InvalidateRect is safe to call from any thread: the invalidation is queued
// on the owning thread and coalesced into a single WM_PAINT. RedrawWindow with
// RDW_UPDATENOW would instead force a synchronous paint per call, which at
// mouse-move rate would run the registry scan below on Explorer's taskbar UI
// thread for every movement.
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

    if (installed) {
        Wh_Log(L"[lang] indicator %s 0x%p attached %llu ms after the mod init",
               cls, (void*)hwnd,
               (unsigned long long)(GetTickCount64() - g_modStartTick.load(std::memory_order_acquire)));
    }
    if (installed && g_indicatorColours.load(std::memory_order_relaxed)) {
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

static void ArmIndicatorSubclass() {
    EnumWindows(TaskbarEnumProc, 0);
}

static void RemoveIndicatorSubclasses() {
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
            RemovePropW(snapshot[i].wnd, L"WhLangIndicatorPainted");
        }
    }

    AcquireSRWLockExclusive(&g_indicatorTargetsLock);
    memset(g_indicatorTargets, 0, sizeof(g_indicatorTargets));
    g_indicatorTargetCount = 0;
    ReleaseSRWLockExclusive(&g_indicatorTargetsLock);
}

// --- Indicator text resolution ----------------------------------------------
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

// A layer that never reaches our painter (for example one drawn by the shell
// itself right after logon) must not be allowed to take over the text, or the
// indicator stays blank. Each layer that completes a paint stamps itself.
static const wchar_t kIndicatorPaintedProp[] = L"WhLangIndicatorPainted";

static bool LayerHasPainted(HWND wnd) {
    return GetPropW(wnd, kIndicatorPaintedProp) != nullptr;
}

static bool ThisLayerShowsText(HWND hwnd) {
    RECT area = {};
    if (!PrimaryIndicatorRect(&area)) return true;
    POINT centre = { (area.left + area.right) / 2, (area.top + area.bottom) / 2 };
    HWND top = TopmostTrackedAt(centre);
    if (!top || top == hwnd) return true;
    return !LayerHasPainted(top);
}

static SRWLOCK g_layoutVariantCacheLock = SRWLOCK_INIT;
static wchar_t g_cachedLayoutId[KL_NAMELENGTH] = {};
static wchar_t g_cachedLayoutVariant[16] = {};
static bool g_layoutVariantCacheValid = false;

static bool ReadLayoutVariant(const wchar_t* layoutId, wchar_t* variant,
                              size_t variantCount) {
    static const wchar_t kLayoutsKey[] =
        L"SYSTEM\\CurrentControlSet\\Control\\Keyboard Layouts\\";
    wchar_t subkey[_countof(kLayoutsKey) + KL_NAMELENGTH] = {};
    wcscpy_s(subkey, _countof(subkey), kLayoutsKey);
    wcscat_s(subkey, _countof(subkey), layoutId);

    wchar_t layoutText[128] = {};
    DWORD bytes = sizeof(layoutText);
    if (RegGetValueW(HKEY_LOCAL_MACHINE, subkey, L"Layout Text",
                     RRF_RT_REG_SZ, nullptr, layoutText, &bytes) != ERROR_SUCCESS) {
        return false;
    }

    const wchar_t* open = wcsrchr(layoutText, L'(');
    const wchar_t* close = open ? wcschr(open + 1, L')') : nullptr;
    if (!open || !close) return false;

    const wchar_t* start = open + 1;
    while (start < close && (*start == L' ' || *start == L'\t')) start++;
    while (close > start && (close[-1] == L' ' || close[-1] == L'\t')) close--;
    const size_t length = (size_t)(close - start);
    if (!length || length >= variantCount) return false;

    wmemcpy(variant, start, length);
    variant[length] = 0;
    return true;
}

static bool GetCachedLayoutVariant(const wchar_t* layoutId, wchar_t* variant,
                                   size_t variantCount) {
    if (!layoutId || !variant || variantCount < 2) return false;

    AcquireSRWLockShared(&g_layoutVariantCacheLock);
    if (g_layoutVariantCacheValid && wcscmp(g_cachedLayoutId, layoutId) == 0) {
        const bool found = g_cachedLayoutVariant[0] != 0;
        if (found && wcscpy_s(variant, variantCount, g_cachedLayoutVariant) != 0) {
            ReleaseSRWLockShared(&g_layoutVariantCacheLock);
            return false;
        }
        if (!found) variant[0] = 0;
        ReleaseSRWLockShared(&g_layoutVariantCacheLock);
        return found;
    }
    ReleaseSRWLockShared(&g_layoutVariantCacheLock);

    wchar_t resolved[16] = {};
    const bool found = ReadLayoutVariant(layoutId, resolved, _countof(resolved));
    AcquireSRWLockExclusive(&g_layoutVariantCacheLock);
    wcscpy_s(g_cachedLayoutId, _countof(g_cachedLayoutId), layoutId);
    wcscpy_s(g_cachedLayoutVariant, _countof(g_cachedLayoutVariant),
             found ? resolved : L"");
    g_layoutVariantCacheValid = true;
    if (found && wcscpy_s(variant, variantCount, resolved) != 0) {
        ReleaseSRWLockExclusive(&g_layoutVariantCacheLock);
        return false;
    }
    if (!found) variant[0] = 0;
    ReleaseSRWLockExclusive(&g_layoutVariantCacheLock);
    return found;
}

// Some keyboard layouts carry a hardware "Layout Id" in the HKL's high word
// (top nibble == 0xF) rather than a KLID prefix. Those must be resolved by
// scanning HKLM\SYSTEM\CurrentControlSet\Control\Keyboard Layouts for a
// subkey whose "Layout Id" value matches devId & 0x0FFF; the subkey name is
// the real KLID. Returns false when no match is found.
//
// Both the root and the per-subkey HKEYs are owned by ScopedRegKey, so every
// exit path (early return, break, exception) closes them without leaks.
//
// The full scan is expensive (a couple of hundred subkeys on a typical
// install), and it would otherwise run on every paint, including every mouse
// move over the indicator. The result is therefore cached per Layout Id in a
// small lock-protected table, so the scan runs once per distinct layout.
static SRWLOCK g_klidCacheLock = SRWLOCK_INIT;
static constexpr int kKlidCacheSize = 16;
struct KlidCacheEntry {
    WORD layoutId;
    wchar_t klid[KL_NAMELENGTH];
};
static KlidCacheEntry g_klidCache[kKlidCacheSize] = {};
static int g_klidCacheCount = 0;

static bool FindKlidByLayoutId(WORD layoutId, wchar_t* out, size_t outCount) {
    if (!out || outCount < 2 || layoutId == 0) return false;
    out[0] = 0;

    AcquireSRWLockShared(&g_klidCacheLock);
    for (int i = 0; i < g_klidCacheCount; i++) {
        if (g_klidCache[i].layoutId == layoutId) {
            const bool found = g_klidCache[i].klid[0] != 0;
            if (found) wcsncpy_s(out, outCount, g_klidCache[i].klid, _TRUNCATE);
            ReleaseSRWLockShared(&g_klidCacheLock);
            return found;
        }
    }
    ReleaseSRWLockShared(&g_klidCacheLock);

    wchar_t resolved[KL_NAMELENGTH] = {};
    bool found = false;
    {
        ScopedRegKey hRoot;
        if (RegOpenKeyExW(HKEY_LOCAL_MACHINE,
                          L"SYSTEM\\CurrentControlSet\\Control\\Keyboard Layouts",
                          0, KEY_READ, hRoot.put()) == ERROR_SUCCESS) {
            for (DWORD i = 0; ; ++i) {
                wchar_t subKey[256] = {};
                DWORD subLen = _countof(subKey);
                FILETIME ft{};
                if (RegEnumKeyExW(hRoot.get(), i, subKey, &subLen, nullptr, nullptr, nullptr, &ft) != ERROR_SUCCESS) {
                    break;
                }

                ScopedRegKey hSub;
                if (RegOpenKeyExW(hRoot.get(), subKey, 0, KEY_READ, hSub.put()) == ERROR_SUCCESS) {
                    wchar_t idBuf[16] = {};
                    DWORD cb = sizeof(idBuf);
                    if (RegQueryValueExW(hSub.get(), L"Layout Id", nullptr, nullptr,
                                         reinterpret_cast<LPBYTE>(idBuf), &cb) == ERROR_SUCCESS && idBuf[0]) {
                        wchar_t* end = nullptr;
                        unsigned long parsed = wcstoul(idBuf, &end, 16);
                        if (end != idBuf && (parsed & 0xFFFF) == layoutId) {
                            wcsncpy_s(resolved, _countof(resolved), subKey, _TRUNCATE);
                            found = true;
                            break;
                        }
                    }
                }
            }
        }
    }

    AcquireSRWLockExclusive(&g_klidCacheLock);
    if (g_klidCacheCount < kKlidCacheSize) {
        g_klidCache[g_klidCacheCount].layoutId = layoutId;
        wcsncpy_s(g_klidCache[g_klidCacheCount].klid, KL_NAMELENGTH,
                  found ? resolved : L"", _TRUNCATE);
        g_klidCacheCount++;
    }
    ReleaseSRWLockExclusive(&g_klidCacheLock);

    if (found) wcsncpy_s(out, outCount, resolved, _TRUNCATE);
    return found;
}

// Resolve the active layout at paint time. Reads the keyboard layout from the
// foreground thread, because that is the thread that actually receives the
// layout change; the taskbar indicator's own thread does not necessarily
// reflect the active layout when another app owns the focus. The KLID is
// derived directly from the HKL (with FindKlidByLayoutId for hardware Layout
// Ids), not from GetKeyboardLayoutNameW, which would only read the calling
// thread's layout.
static bool GetIndicatorText(HWND hwnd, wchar_t* text, size_t textCount,
                             wchar_t* text2, size_t text2Count) {
    if (!hwnd || !text || textCount < 4 || !text2 || text2Count < 4) return false;
    text[0] = text2[0] = 0;

    DWORD threadId = 0;
    HWND hFore = GetForegroundWindow();
    if (hFore) {
        threadId = GetWindowThreadProcessId(hFore, nullptr);
    }
    if (!threadId) {
        threadId = GetWindowThreadProcessId(hwnd, nullptr);
    }
    if (!threadId) return false;

    const HKL layout = GetKeyboardLayout(threadId);
    if (!layout) return false;
    const LANGID lang = (LANGID)LOWORD((UINT_PTR)layout);
    if (!GetLocaleInfoW(MAKELCID(lang, SORT_DEFAULT), LOCALE_SABBREVLANGNAME,
                        text, (int)textCount) || !text[0]) {
        return false;
    }

    const WORD dev = HIWORD((UINT_PTR)layout);
    const WORD langLow = LOWORD((UINT_PTR)layout);
    wchar_t layoutId[KL_NAMELENGTH] = {};
    if ((dev & 0xF000) == 0xF000) {
        if (!FindKlidByLayoutId((WORD)(dev & 0x0FFF), layoutId, _countof(layoutId))) {
            swprintf_s(layoutId, L"%08X", (UINT)langLow);
        }
    } else if (dev == 0 || dev == langLow) {
        swprintf_s(layoutId, L"%08X", (UINT)langLow);
    } else {
        swprintf_s(layoutId, L"%04X%04X", (UINT)dev, (UINT)langLow);
    }

    if (!GetCachedLayoutVariant(layoutId, text2, text2Count)) {
        GetLocaleInfoW(MAKELCID(lang, SORT_DEFAULT), LOCALE_SISO3166CTRYNAME,
                       text2, (int)text2Count);
    }

    static std::atomic<int> s_log{0};
    if (s_log.fetch_add(1, std::memory_order_relaxed) < 60) {
        Wh_Log(L"[lang-paint] GetIndicatorText: tid=0x%X hkl=0x%p klid=%s text=%s text2=%s",
               (unsigned)threadId, (void*)layout, layoutId, text, text2);
    }
    return true;
}

// --- Painting ---------------------------------------------------------------
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
    if (showsText) SetPropW(hwnd, kIndicatorPaintedProp, (HANDLE)1);

    const int logIndex = g_indicatorCellLogs.fetch_add(1, std::memory_order_relaxed);
    if (logIndex < 40) {
        RECT windowRect = {};
        GetWindowRect(hwnd, &windowRect);
        Wh_Log(L"[lang] indicator repainted (%s): %s 0x%p %dx%d at (%d,%d), text %s %s",
               why, cls, (void*)hwnd, width, height, windowRect.left, windowRect.top,
               showsText ? text : L"(carried by topmost indicator layer)",
               showsText ? text2 : L"");
    }
}

// --- Indicator subclass -----------------------------------------------------
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
        InvalidateIndicatorTargets();
    }
}

// InvalidateRect is thread-safe and makes the owning thread repaint, so no
// private message is needed. WM_APP is documented as reserved for the owning
// application (Explorer, in this case) and must not be posted to its windows
// from another module. A RegisterWindowMessageW value would be the correct
// fallback if a custom notification were ever required.
static void PostForceRepaintToIndicators() {
    if (g_unloading.load(std::memory_order_acquire) ||
        !g_indicatorColours.load(std::memory_order_relaxed)) return;
    HWND targets[kMaxIndicatorTargets] = {};
    const int count = CopyIndicatorTargets(targets, _countof(targets));
    for (int i = 0; i < count; i++) {
        if (IsOwnProcessWindow(targets[i])) {
            InvalidateRect(targets[i], nullptr, FALSE);
        }
    }
}

typedef HKL(WINAPI* ActivateKeyboardLayout_t)(HKL, UINT);
static ActivateKeyboardLayout_t ActivateKeyboardLayout_Original = nullptr;

typedef HKL(WINAPI* LoadKeyboardLayoutW_t)(LPCWSTR, UINT);
static LoadKeyboardLayoutW_t LoadKeyboardLayoutW_Original = nullptr;

static HKL WINAPI ActivateKeyboardLayout_Hook(HKL layout, UINT flags) {
    const HKL previous = GetKeyboardLayout(GetCurrentThreadId());
    const HKL result = ActivateKeyboardLayout_Original(layout, flags);
    Wh_Log(L"[lang-hook] ActivateKeyboardLayout: requested=0x%p flags=0x%X prev=0x%p -> result=0x%p",
           (void*)layout, flags, (void*)previous, (void*)result);
    PostForceRepaintToIndicators();
    return result;
}

static HKL WINAPI LoadKeyboardLayoutW_Hook(LPCWSTR id, UINT flags) {
    const HKL result = LoadKeyboardLayoutW_Original(id, flags);
    Wh_Log(L"[lang-hook] LoadKeyboardLayoutW: id=%s flags=0x%X -> result=0x%p",
           id ? id : L"(null)", flags, (void*)result);
    if (!(flags & KLF_NOTELLSHELL)) {
        PostForceRepaintToIndicators();
    }
    return result;
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

    const bool coloursEnabled = g_indicatorColours.load(std::memory_order_relaxed);
    bool repaintAfterDefault = false;
    switch (msg) {
        case WM_ERASEBKGND:
            if (coloursEnabled) return 1;
            break;

        case WM_PAINT:
            if (coloursEnabled) {
                PAINTSTRUCT paint = {};
                HDC hdc = BeginPaint(hwnd, &paint);
                if (hdc) PaintIndicatorCell(hwnd, L"paint", hdc);
                EndPaint(hwnd, &paint);
                return 0;
            }
            break;

        case WM_PRINTCLIENT:
            if (coloursEnabled) {
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
            break;
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

        case WM_INPUTLANGCHANGE:
            if (coloursEnabled) {
                InvalidateRect(hwnd, nullptr, FALSE);
                UpdateWindow(hwnd);
                return 0;
            }
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
    if (repaintAfterDefault && coloursEnabled) RequestIndicatorRepaint();
    return result;
}

// --- Layout polling thread ---------------------------------------------------
// The real change of layout happens on the foreground thread. The taskbar
// indicator's own thread does not necessarily reflect it, so polling the
// indicator thread alone can miss the change. Poll the foreground thread's
// HKL instead; also poll the indicator thread as a fallback when there is no
// foreground window.
static HANDLE g_layoutPollThread = nullptr;
static HANDLE g_layoutPollStopEvent = nullptr;

static DWORD WINAPI LayoutPollThread(LPVOID) {
    DWORD lastFgHkl = 0;
    DWORD lastIndHkl = 0;
    {
        HWND hFore = GetForegroundWindow();
        if (hFore) {
            DWORD tid = GetWindowThreadProcessId(hFore, nullptr);
            if (tid) lastFgHkl = (DWORD)(UINT_PTR)GetKeyboardLayout(tid);
        }
        HWND targets[kMaxIndicatorTargets] = {};
        const int count = CopyIndicatorTargets(targets, _countof(targets));
        if (count > 0) {
            DWORD tid = GetWindowThreadProcessId(targets[0], nullptr);
            if (tid) lastIndHkl = (DWORD)(UINT_PTR)GetKeyboardLayout(tid);
        }
    }

    for (;;) {
        if (WaitForSingleObject(g_layoutPollStopEvent, 100) != WAIT_TIMEOUT) break;
        if (g_unloading.load(std::memory_order_acquire)) break;
        if (!g_indicatorColours.load(std::memory_order_relaxed)) continue;

        DWORD fgHkl = 0;
        HWND hFore = GetForegroundWindow();
        if (hFore) {
            DWORD tid = GetWindowThreadProcessId(hFore, nullptr);
            if (tid) fgHkl = (DWORD)(UINT_PTR)GetKeyboardLayout(tid);
        }

        DWORD indHkl = 0;
        HWND targets[kMaxIndicatorTargets] = {};
        const int count = CopyIndicatorTargets(targets, _countof(targets));
        if (count > 0) {
            DWORD tid = GetWindowThreadProcessId(targets[0], nullptr);
            if (tid) indHkl = (DWORD)(UINT_PTR)GetKeyboardLayout(tid);
        }

        if (fgHkl == lastFgHkl && indHkl == lastIndHkl) continue;
        Wh_Log(L"[lang-poll] layout change: fg=0x%08X ind=0x%08X",
               (unsigned)fgHkl, (unsigned)indHkl);
        lastFgHkl = fgHkl;
        lastIndHkl = indHkl;
        PostForceRepaintToIndicators();
    }
    return 0;
}

static bool StartLayoutPollThread() {
    if (g_layoutPollThread) return true;
    if (!g_layoutPollStopEvent) {
        g_layoutPollStopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
        if (!g_layoutPollStopEvent) return false;
    } else {
        ResetEvent(g_layoutPollStopEvent);
    }
    g_layoutPollThread = CreateThread(nullptr, 0, LayoutPollThread, nullptr, 0, nullptr);
    return g_layoutPollThread != nullptr;
}

static void StopLayoutPollThread() {
    // Take ownership of the handles under RAII so the destructor closes them
    // even if a future edit adds an early return between the waits.
    ScopedHandle threadHandle(g_layoutPollThread);
    ScopedHandle stopEvent(g_layoutPollStopEvent);
    g_layoutPollThread = nullptr;
    g_layoutPollStopEvent = nullptr;

    if (stopEvent.valid()) SetEvent(stopEvent.get());
    if (threadHandle.valid()) WaitForSingleObject(threadHandle.get(), INFINITE);
}

// --- Language guard thread --------------------------------------------------
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
    const bool hookedActivate = Wh_SetFunctionHook((void*)ActivateKeyboardLayout,
                                                   (void*)ActivateKeyboardLayout_Hook,
                                                   (void**)&ActivateKeyboardLayout_Original);
    const bool hookedLoad = Wh_SetFunctionHook((void*)LoadKeyboardLayoutW,
                                               (void*)LoadKeyboardLayoutW_Hook,
                                               (void**)&LoadKeyboardLayoutW_Original);
    Wh_Log(L"[language] guard hooks: ShowWindow %s, ShowWindowAsync %s, CreateWindowExW %s, SetWindowPos %s, ActivateKeyboardLayout %s, LoadKeyboardLayoutW %s",
           hookedShow ? L"installed" : L"not installed",
           hookedAsync ? L"installed" : L"not installed",
           hookedCreate ? L"installed" : L"not installed",
           hookedPos ? L"installed" : L"not installed",
           hookedActivate ? L"installed" : L"not installed",
           hookedLoad ? L"installed" : L"not installed");
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
    Wh_Log(L"[language] guard off (the indicator colours stay on)");
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
    // Take ownership under RAII before waiting, so the handles are released
    // exactly once on every exit path even if a future edit adds an early
    // return inside the locked section.
    ScopedHandle threadHandle(g_languageThread);
    ScopedHandle stopEvent(g_stopEvent);
    g_languageThread = nullptr;
    g_stopEvent = nullptr;

    if (stopEvent.valid()) SetEvent(stopEvent.get());
    if (threadHandle.valid()) WaitForSingleObject(threadHandle.get(), INFINITE);
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
    Wh_Log(L"[lang] %s: guard=%s (%u s), indicator colours=%s, taskbar menu on right-click=%s, logon census=%s",
           prefix,
           g_langGuardEnabled.load(std::memory_order_relaxed) ? L"on" : L"off",
           (unsigned)(g_langGuardMs.load(std::memory_order_relaxed) / 1000),
           g_indicatorColours.load(std::memory_order_relaxed) ? L"on" : L"off",
           g_routeRightClickToTaskbar.load(std::memory_order_relaxed) ? L"on" : L"off",
           g_langCensusLog.load(std::memory_order_relaxed) ? L"on" : L"off");
}

// --- Startup rescan ---------------------------------------------------------
// At Explorer startup the engine loads the mod before the taskbar and the
// language indicator exist, and hooks set in Wh_ModInit become active only after
// it returns. A one-shot EnumWindows in Wh_ModInit therefore finds nothing, and
// windows created in the gap are missed by the CreateWindowExW hook. This thread
// repeats the scan for the first three minutes, then exits. TrackIndicatorWindow is
// idempotent, so repeated scans never subclass a window twice.
static HANDLE g_rescanThread = nullptr;
static HANDLE g_rescanStopEvent = nullptr;
static SRWLOCK g_rescanLock = SRWLOCK_INIT;

static DWORD WINAPI IndicatorRescanThread(LPVOID) {
    const ULONGLONG start = GetTickCount64();
    // The rescan only covers the logon window; it ends after three minutes.
    while (GetTickCount64() - start < 180000) {
        if (WaitForSingleObject(g_rescanStopEvent, 250) != WAIT_TIMEOUT) break;
        if (g_unloading.load(std::memory_order_acquire)) break;
        if (ShouldTrackIndicatorWindows()) ArmIndicatorSubclass();
    }
    return 0;
}

static bool StartIndicatorRescanThread() {
    AcquireSRWLockExclusive(&g_rescanLock);
    bool ok = true;
    if (g_unloading.load(std::memory_order_acquire)) {
        ok = false;
    } else if (!g_rescanThread) {
        if (!g_rescanStopEvent) {
            g_rescanStopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
        } else {
            ResetEvent(g_rescanStopEvent);
        }
        if (g_rescanStopEvent) {
            g_rescanThread = CreateThread(nullptr, 0, IndicatorRescanThread, nullptr, 0, nullptr);
        }
        ok = g_rescanThread != nullptr;
        if (!ok) Wh_Log(L"[lang] failed to start the startup rescan thread (%lu)", GetLastError());
    }
    ReleaseSRWLockExclusive(&g_rescanLock);
    return ok;
}

static void StopIndicatorRescanThread() {
    AcquireSRWLockExclusive(&g_rescanLock);
    ScopedHandle threadHandle(g_rescanThread);
    ScopedHandle stopEvent(g_rescanStopEvent);
    g_rescanThread = nullptr;
    g_rescanStopEvent = nullptr;
    if (stopEvent.valid()) SetEvent(stopEvent.get());
    if (threadHandle.valid()) WaitForSingleObject(threadHandle.get(), INFINITE);
    ReleaseSRWLockExclusive(&g_rescanLock);
}

// --- Windhawk entry points --------------------------------------------------
// True when this process is the system %SystemRoot%\explorer.exe. Uses the real
// image path (QueryFullProcessImageNameW), which Fake Explorer path does not hook.
static bool IsSystemExplorer() {
    WCHAR systemPath[MAX_PATH];
    UINT len = GetWindowsDirectoryW(systemPath, ARRAYSIZE(systemPath));
    WCHAR path[MAX_PATH];
    DWORD size = ARRAYSIZE(path);
    if (!len || len >= ARRAYSIZE(systemPath) ||
        wcscat_s(systemPath, L"\\explorer.exe") != 0 ||
        !QueryFullProcessImageNameW(GetCurrentProcess(), 0, path, &size)) {
        return false;
    }
    return _wcsicmp(path, systemPath) == 0;
}

BOOL Wh_ModInit() {
    if (IsSystemExplorer()) {
        Wh_Log(L"[lang] system explorer.exe detected; not loading");
        return FALSE;
    }
    g_unloading.store(false, std::memory_order_release);
    g_modStartTick.store(GetTickCount64(), std::memory_order_release);
    LoadLanguageSettings();
    Wh_Log(L"[lang] init: Windows 10 language flyout guard and indicator colours");
    LogCurrentSettings(L"settings");

    InstallLanguageGuardHooks();
    if (ShouldTrackIndicatorWindows()) {
        ArmIndicatorSubclass();
        StartIndicatorRescanThread();
    }
    if (g_indicatorColours.load(std::memory_order_relaxed)) {
        StartLayoutPollThread();
    }

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

// Called by the engine after the hooks from Wh_ModInit are active, so windows
// created in the gap since Wh_ModInit are picked up here.
void Wh_ModAfterInit() {
    if (ShouldTrackIndicatorWindows()) ArmIndicatorSubclass();
    if (g_langGuardEnabled.load(std::memory_order_relaxed)) RunLanguageGuardCensus();
}

void Wh_ModBeforeUninit() {
    g_unloading.store(true, std::memory_order_release);
    AcquireSRWLockExclusive(&g_indicatorTargetsLock);
    ReleaseSRWLockExclusive(&g_indicatorTargetsLock);
    AcquireSRWLockExclusive(&g_languageThreadLock);
    if (g_stopEvent) SetEvent(g_stopEvent);
    ReleaseSRWLockExclusive(&g_languageThreadLock);
    if (g_layoutPollStopEvent) SetEvent(g_layoutPollStopEvent);
    AcquireSRWLockExclusive(&g_rescanLock);
    if (g_rescanStopEvent) SetEvent(g_rescanStopEvent);
    ReleaseSRWLockExclusive(&g_rescanLock);
}

void Wh_ModUninit() {
    g_unloading.store(true, std::memory_order_release);
    StopIndicatorRescanThread();
    StopLayoutPollThread();
    StopLanguageGuardThread();
    RemoveIndicatorSubclasses();
    Wh_Log(L"[lang] unloaded");
}

void Wh_ModSettingsChanged() {
    const bool previousGuard = g_langGuardEnabled.load(std::memory_order_acquire);
    const DWORD previousDuration = g_langGuardMs.load(std::memory_order_acquire);
    const bool previousCensusLog = g_langCensusLog.load(std::memory_order_acquire);
    const bool previousColours = g_indicatorColours.load(std::memory_order_acquire);

    LoadLanguageSettings();
    const bool coloursEnabled = g_indicatorColours.load(std::memory_order_acquire);
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

    if (ShouldTrackIndicatorWindows()) {
        ArmIndicatorSubclass();
        StartIndicatorRescanThread();
    }

    if (coloursEnabled) {
        StartLayoutPollThread();
    } else {
        StopLayoutPollThread();
    }

    if (previousColours != coloursEnabled) {
        ResetThemeCache();
        InvalidateIndicatorTargets();
    }

    LogCurrentSettings(L"settings reloaded");
}
