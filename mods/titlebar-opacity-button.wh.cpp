// ==WindhawkMod==
// @id              titlebar-opacity-button
// @name            Titlebar Opacity Button
// @description     Adds a small opacity button next to the minimize button of the active window, plus modifier+scroll to fade any window
// @version         1.0.0
// @author          JeremyWayne93
// @github          https://github.com/JeremyWayne93
// @include         windhawk.exe
// @compilerOptions -ldwmapi -lgdi32 -luser32 -lshell32 -ladvapi32
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Titlebar Opacity Button

Make any window see-through with one click.

A small button floats just left of the **minimize** button of the window you're
working in and follows it around. Every click makes the window more
see-through, and after the lowest level it snaps back to solid.

## Usage

- **Left-click the button**: 100% → 90% → 80% → ... → minimum → back to 100%
- **Right-click the button**: back to 100% (solid)
- **Alt + mouse wheel** over any window: wheel up = more solid, wheel down = more
  see-through
- **Alt + middle-click** over any window: back to 100%

A small percentage indicator shows up briefly after every change. The step size,
minimum opacity, button size, colors, glyph, position, modifier key and excluded
programs can all be changed in the mod settings, and changes apply instantly.

When the mod is disabled or unloaded, every window it made see-through is
restored to full opacity.

## How it works

- The button is a tiny always-on-top window that never takes focus. It is
  placed next to the window's real caption buttons, found by hit-testing the
  title bar (`WM_NCHITTEST`), so it also lines up in apps that draw their own
  title bar (Chrome, Edge, VS Code, Discord and other Electron apps). The
  result is cached per window size. If that fails it falls back to
  `DWMWA_CAPTION_BUTTON_BOUNDS`. Everything is scaled for the window's DPI.
- It is fully event driven (`SetWinEventHook` for foreground and focus
  changes, plus move/resize, minimize and hide events of the active window
  only), so it uses no CPU while nothing changes. There is no polling timer.
- The wheel shortcut uses a keyboard hook that only arms a mouse hook while the
  modifier key is held down, so mouse movement isn't intercepted the rest of the
  time. Both hooks run on their own thread that only forwards events, so typing
  and mouse input never wait on other programs. Over excluded programs, the
  taskbar and the desktop, modifier + wheel is passed through as usual.
- Opacity is applied with `WS_EX_LAYERED` + `SetLayeredWindowAttributes`, and
  `WS_EX_LAYERED` is removed again when a window goes back to 100%.
- The mod runs in its own dedicated `windhawk.exe` process (a "tool mod"), so it
  doesn't inject into Explorer or any other program.

## Limitations

- Windows of programs running as administrator (for example Task Manager,
  Windhawk's own window, or any app set to "Run as administrator") can't be
  changed, because Windows blocks a normal program from modifying them (UIPI).
  Windows that draw their own transparency (per-pixel layered or color key
  windows) are left alone too. By default these get no button and the wheel
  shortcut quietly ignores them; turn off "Hide on windows that can't be
  changed" to see a dimmed button and a short message instead.
- Some UWP/WinUI windows don't visibly react to opacity changes.
- Windows can't host a real button inside another program's title bar without
  injecting into every program, so the button is a separate window that sits
  next to the caption buttons. If an app doesn't report its caption buttons at
  all, the position uses a fallback offset from the right edge, which can be
  tuned with the horizontal offset setting.
- Maximized windows are supported. The button is hidden over fullscreen
  windows (no title bar, covering the whole monitor).
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- step: 10
  $name: Opacity step (%)
  $description: How much each button click or wheel notch changes the opacity (1-50)
- minOpacity: 20
  $name: Minimum opacity (%)
  $description: >-
    The most see-through a window can get (5-95). Clicking the button at this
    level wraps back to 100%.
- buttonEnabled: true
  $name: Show the titlebar button
- buttonWidth: 30
  $name: Button width
  $description: Width in pixels at 100% display scaling
- buttonHeight: 22
  $name: Button height
  $description: Height in pixels at 100% display scaling
- buttonOffset: 4
  $name: Horizontal offset
  $description: >-
    Gap in pixels (at 100% scaling) between the button and the minimize button.
    Negative values move it closer / over the caption buttons.
- buttonColor: "#2B2B2B"
  $name: Button background color
  $description: Hex color, for example #2B2B2B
- buttonText: "◐"
  $name: Button text / glyph
  $description: One or more characters shown on the button
- buttonTextColor: "#FFFFFF"
  $name: Button text color
  $description: Hex color, for example #FFFFFF
- buttonFontSize: 11
  $name: Button text size (pt)
- showIndicator: true
  $name: Show percentage indicator
  $description: Briefly show the new opacity after each change
- wheelEnabled: true
  $name: Enable modifier + mouse wheel
  $description: Hold the modifier and scroll over any window to change its opacity; modifier + middle-click resets it
- wheelModifier: alt
  $name: Wheel modifier key
  $options:
  - alt: Alt
  - ctrl: Ctrl
  - shift: Shift
  - win: Win
- hideOnUnchangeable: true
  $name: Hide on windows that can't be changed
  $description: >-
    Programs running as administrator (and windows that draw their own
    transparency) can't be made see-through. When on, they get no button and
    the wheel shortcut quietly ignores them. When off, the button is shown
    dimmed there and a short message explains why.
- excludedProcesses: [""]
  $name: Excluded programs
  $description: >-
    Process names (for example vlc.exe) that get no button and can't be
    changed with the wheel
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <windows.h>
#include <dwmapi.h>
#include <shellapi.h>

#include <algorithm>
#include <atomic>
#include <cwctype>
#include <string>
#include <unordered_map>
#include <vector>

////////////////////////////////////////////////////////////////////////////////
// Settings

enum class Modifier { Alt, Ctrl, Shift, Win };

struct Settings {
    int step = 10;
    int minOpacity = 20;
    bool buttonEnabled = true;
    int buttonWidth = 30;
    int buttonHeight = 22;
    int buttonOffset = 4;
    COLORREF buttonColor = RGB(0x2B, 0x2B, 0x2B);
    std::wstring buttonText = L"\u25D0";
    COLORREF buttonTextColor = RGB(0xFF, 0xFF, 0xFF);
    int buttonFontSize = 11;
    bool showIndicator = true;
    bool wheelEnabled = true;
    Modifier wheelModifier = Modifier::Alt;
    bool hideOnUnchangeable = true;
    std::vector<std::wstring> excludedProcesses;  // lowercase
};

static std::wstring Trim(std::wstring s) {
    auto notSpace = [](wchar_t c) { return !iswspace(c); };
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), notSpace));
    s.erase(std::find_if(s.rbegin(), s.rend(), notSpace).base(), s.end());
    return s;
}

static std::wstring ToLower(std::wstring s) {
    for (auto& c : s) {
        c = towlower(c);
    }
    return s;
}

static COLORREF ParseColor(const std::wstring& raw, COLORREF fallback) {
    std::wstring s = Trim(raw);
    if (!s.empty() && s[0] == L'#') {
        s.erase(0, 1);
    }
    if (s.size() == 3) {
        s = {s[0], s[0], s[1], s[1], s[2], s[2]};
    }
    if (s.size() != 6) {
        return fallback;
    }
    wchar_t* end = nullptr;
    unsigned long v = wcstoul(s.c_str(), &end, 16);
    if (!end || *end != L'\0') {
        return fallback;
    }
    return RGB((v >> 16) & 0xFF, (v >> 8) & 0xFF, v & 0xFF);
}

static Settings* LoadSettings() {
    Settings* s = new Settings();
    s->step = std::clamp(Wh_GetIntSetting(L"step"), 1, 50);
    s->minOpacity = std::clamp(Wh_GetIntSetting(L"minOpacity"), 5, 95);
    s->buttonEnabled = Wh_GetIntSetting(L"buttonEnabled") != 0;
    s->buttonWidth = std::clamp(Wh_GetIntSetting(L"buttonWidth"), 8, 200);
    s->buttonHeight = std::clamp(Wh_GetIntSetting(L"buttonHeight"), 8, 100);
    s->buttonOffset = std::clamp(Wh_GetIntSetting(L"buttonOffset"), -300, 300);
    s->buttonColor =
        ParseColor(WindhawkUtils::StringSetting::make(L"buttonColor").get(),
                   RGB(0x2B, 0x2B, 0x2B));
    std::wstring text = WindhawkUtils::StringSetting::make(L"buttonText").get();
    s->buttonText = text.empty() ? L"\u25D0" : text;
    s->buttonTextColor =
        ParseColor(WindhawkUtils::StringSetting::make(L"buttonTextColor").get(),
                   RGB(0xFF, 0xFF, 0xFF));
    s->buttonFontSize = std::clamp(Wh_GetIntSetting(L"buttonFontSize"), 5, 48);
    s->showIndicator = Wh_GetIntSetting(L"showIndicator") != 0;
    s->wheelEnabled = Wh_GetIntSetting(L"wheelEnabled") != 0;
    s->hideOnUnchangeable = Wh_GetIntSetting(L"hideOnUnchangeable") != 0;

    std::wstring mod =
        WindhawkUtils::StringSetting::make(L"wheelModifier").get();
    if (mod == L"ctrl") {
        s->wheelModifier = Modifier::Ctrl;
    } else if (mod == L"shift") {
        s->wheelModifier = Modifier::Shift;
    } else if (mod == L"win") {
        s->wheelModifier = Modifier::Win;
    } else {
        s->wheelModifier = Modifier::Alt;
    }

    for (int i = 0; i < 256; i++) {
        std::wstring name = ToLower(
            Trim(WindhawkUtils::StringSetting::make(L"excludedProcesses[%d]", i)
                     .get()));
        if (name.empty()) {
            break;
        }
        s->excludedProcesses.push_back(name);
    }
    return s;
}

////////////////////////////////////////////////////////////////////////////////
// State (owned by the worker thread unless noted)

enum : UINT {
    WM_APP_SETTINGS = WM_APP + 1,  // lParam = Settings*
    WM_APP_QUIT,
    WM_APP_WHEEL,   // wParam = delta, lParam = point
    WM_APP_MCLICK,  // lParam = point
    WM_APP_MASK_KEY,
};

// Thread messages for the hook thread.
enum : UINT {
    WM_HK_ARM_MOUSE = WM_APP + 0x40,
    WM_HK_DISARM_MOUSE,    // wParam = 1 if the modifier was released
    WM_HK_APPLY_SETTINGS,  // wParam = 1 if the modifier changed
};

static const ULONG_PTR kInjectedMarker = 0x54424F50;  // "TOBP"
static const UINT_PTR kIndicatorTimer = 1;
static const UINT_PTR kForegroundRecheckTimer = 2;
static const wchar_t kControlClass[] = L"WindhawkTitlebarOpacityCtl_" WH_MOD_ID;
static const wchar_t kButtonClass[] = L"WindhawkTitlebarOpacityBtn_" WH_MOD_ID;
static const wchar_t kIndicatorClass[] =
    L"WindhawkTitlebarOpacityTip_" WH_MOD_ID;

static Settings* g_settings;
static HANDLE g_thread;
static HANDLE g_readyEvent;
static bool g_initOk;
static HWND g_ctlWnd;
static HWND g_btnWnd;
static HWND g_tipWnd;

static HWINEVENTHOOK g_fgHook;
static HWINEVENTHOOK g_minHook;
static HWINEVENTHOOK g_focusHook;
static HWINEVENTHOOK g_captureHook;
static HWINEVENTHOOK g_targetLocHook;
static HWINEVENTHOOK g_targetShowHideHook;
static HWINEVENTHOOK g_targetCloakHook;
static HWINEVENTHOOK g_targetMoveSizeHook;
static DWORD g_targetHookThread;
static bool g_inMoveSize;

// The low-level keyboard and mouse hooks live on their own thread, which does
// nothing but pump messages, so input never waits for the worker thread
// (which talks to other, possibly busy, programs). The hook procs only read
// these atomic copies of the settings and post messages.
static HANDLE g_hookThread;
static std::atomic<DWORD> g_hookThreadId;
static std::atomic<int> g_hkModifier{(int)Modifier::Alt};
static std::atomic<bool> g_hkWheelEnabled{true};
static std::atomic<bool> g_hkHideUnchangeable{true};

// Owned by the hook thread.
static HHOOK g_kbHook;
static HHOOK g_mouseHook;
static bool g_swallowMiddleUp;
static bool g_maskSent;

static HWND g_target;
static bool g_targetExcluded;
static bool g_targetChangeable = true;
static RECT g_btnRect;
static bool g_btnVisible;
static bool g_btnHover;
static HFONT g_btnFont;
static int g_btnFontDpi;
static HFONT g_tipFont;
static int g_tipFontDpi;
static std::wstring g_tipText;
static int g_wheelAccum;
static HWND g_wheelTarget;

// Windows we changed, with their state before we touched them.
struct ModifiedWindow {
    bool hadLayered;  // Whether it already had WS_EX_LAYERED.
    BYTE origAlpha;   // Its own alpha (255 unless it was already layered).
};
static std::unordered_map<HWND, ModifiedWindow> g_modified;

////////////////////////////////////////////////////////////////////////////////
// Helpers

static int DpiOf(HWND hwnd) {
    UINT dpi = hwnd ? GetDpiForWindow(hwnd) : 0;
    return dpi ? (int)dpi : 96;
}

static int Scale(int v, int dpi) {
    return MulDiv(v, dpi, 96);
}

static bool IsShellClass(HWND hwnd) {
    WCHAR cls[64];
    if (!GetClassNameW(hwnd, cls, ARRAYSIZE(cls))) {
        return true;
    }
    static const wchar_t* kExcluded[] = {
        L"Progman",
        L"WorkerW",
        L"Shell_TrayWnd",
        L"Shell_SecondaryTrayWnd",
        L"RainmeterMeterWindow",
        L"Windows.UI.Core.CoreWindow",
        L"XamlExplorerHostIslandWindow",
        L"TopLevelWindowForOverflowXamlIsland",
    };
    for (auto* c : kExcluded) {
        if (_wcsicmp(cls, c) == 0) {
            return true;
        }
    }
    return false;
}

static bool IsCloaked(HWND hwnd) {
    DWORD cloaked = 0;
    return SUCCEEDED(DwmGetWindowAttribute(hwnd, DWMWA_CLOAKED, &cloaked,
                                           sizeof(cloaked))) &&
           cloaked;
}

// Whether the button should be shown for this window.
static bool IsEligibleForButton(HWND hwnd) {
    if (!hwnd || !IsWindow(hwnd) || !IsWindowVisible(hwnd) || IsIconic(hwnd)) {
        return false;
    }
    LONG_PTR style = GetWindowLongPtrW(hwnd, GWL_STYLE);
    if (!(style & WS_MINIMIZEBOX) || (style & WS_MINIMIZE)) {
        return false;
    }
    if (IsShellClass(hwnd) || IsCloaked(hwnd)) {
        return false;
    }
    if ((style & WS_CAPTION) != WS_CAPTION) {
        // No caption: skip if it fills its monitor (fullscreen).
        RECT rc;
        MONITORINFO mi{};
        mi.cbSize = sizeof(mi);
        HMONITOR mon = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
        if (GetWindowRect(hwnd, &rc) && GetMonitorInfoW(mon, &mi) &&
            rc.left <= mi.rcMonitor.left && rc.top <= mi.rcMonitor.top &&
            rc.right >= mi.rcMonitor.right &&
            rc.bottom >= mi.rcMonitor.bottom) {
            return false;
        }
    }
    return true;
}

////////////////////////////////////////////////////////////////////////////////
// Opacity

// Returns false if the window's transparency can't be controlled by us.
static bool GetOpacityAlpha(HWND hwnd, BYTE* alpha) {
    LONG_PTR ex = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);
    if (!(ex & WS_EX_LAYERED)) {
        *alpha = 255;
        return true;
    }
    COLORREF key;
    BYTE a = 255;
    DWORD flags = 0;
    if (!GetLayeredWindowAttributes(hwnd, &key, &a, &flags)) {
        // Uses UpdateLayeredWindow (per-pixel alpha), don't touch.
        return false;
    }
    if (flags & LWA_COLORKEY) {
        return false;
    }
    *alpha = (flags & LWA_ALPHA) ? a : 255;
    return true;
}

static DWORD IntegrityLevelOfToken(HANDLE token) {
    DWORD size = 0;
    GetTokenInformation(token, TokenIntegrityLevel, nullptr, 0, &size);
    if (!size) {
        return 0;
    }
    std::vector<BYTE> buf(size);
    if (!GetTokenInformation(token, TokenIntegrityLevel, buf.data(), size,
                             &size)) {
        return 0;
    }
    PSID sid = ((TOKEN_MANDATORY_LABEL*)buf.data())->Label.Sid;
    UCHAR count = *GetSidSubAuthorityCount(sid);
    return count ? *GetSidSubAuthority(sid, count - 1) : 0;
}

static DWORD OwnIntegrityLevel() {
    static DWORD level = [] {
        HANDLE token;
        DWORD l = 0;
        if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &token)) {
            l = IntegrityLevelOfToken(token);
            CloseHandle(token);
        }
        return l;
    }();
    return level;
}

// Per-process facts, used by the worker and by the mouse hook (to decide
// cheaply whether to swallow an event). Entries expire after a few seconds so
// a reused process ID doesn't keep stale facts.
struct ProcInfo {
    bool excluded = false;
    bool elevated = false;  // Runs above our integrity level (UIPI).
    ULONGLONG tick = 0;
};
static const ULONGLONG kProcCacheTtl = 5000;
static SRWLOCK g_procLock = SRWLOCK_INIT;
// Guarded by g_procLock.
static std::vector<std::wstring> g_procExcludedList;  // lowercase
static std::unordered_map<DWORD, ProcInfo> g_procCache;
static unsigned g_procGeneration;

static void SetExcludedProcesses(const std::vector<std::wstring>& list) {
    AcquireSRWLockExclusive(&g_procLock);
    g_procExcludedList = list;
    g_procCache.clear();
    g_procGeneration++;
    ReleaseSRWLockExclusive(&g_procLock);
}

// Called with g_procLock held (shared).
static ProcInfo QueryProcInfo(DWORD pid) {
    ProcInfo info;
    HANDLE process =
        OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!process) {
        info.elevated = true;  // Can't even query it, so can't change it.
        return info;
    }
    if (!g_procExcludedList.empty()) {
        WCHAR path[MAX_PATH];
        DWORD size = ARRAYSIZE(path);
        if (QueryFullProcessImageNameW(process, 0, path, &size)) {
            PCWSTR name = wcsrchr(path, L'\\');
            name = name ? name + 1 : path;
            std::wstring lower = ToLower(name);
            for (const auto& e : g_procExcludedList) {
                if (lower == e) {
                    info.excluded = true;
                    break;
                }
            }
        }
    }
    HANDLE token;
    if (OpenProcessToken(process, TOKEN_QUERY, &token)) {
        DWORD own = OwnIntegrityLevel();
        if (own && IntegrityLevelOfToken(token) > own) {
            info.elevated = true;
        }
        CloseHandle(token);
    } else if (GetLastError() == ERROR_ACCESS_DENIED) {
        // A normal process can't open the token of an elevated one.
        info.elevated = true;
    }
    CloseHandle(process);
    return info;
}

// Thread safe. Doesn't send messages to other threads.
static ProcInfo GetProcInfo(DWORD pid) {
    if (pid == GetCurrentProcessId()) {
        ProcInfo own;
        own.excluded = true;
        return own;
    }
    ULONGLONG now = GetTickCount64();
    AcquireSRWLockShared(&g_procLock);
    auto it = g_procCache.find(pid);
    if (it != g_procCache.end() && now - it->second.tick < kProcCacheTtl) {
        ProcInfo cached = it->second;
        ReleaseSRWLockShared(&g_procLock);
        return cached;
    }
    unsigned generation = g_procGeneration;
    ProcInfo info = QueryProcInfo(pid);
    ReleaseSRWLockShared(&g_procLock);

    info.tick = now;
    AcquireSRWLockExclusive(&g_procLock);
    if (generation == g_procGeneration) {
        if (g_procCache.size() > 128) {
            g_procCache.clear();
        }
        g_procCache[pid] = info;
    }
    ReleaseSRWLockExclusive(&g_procLock);
    return info;
}

static bool IsProcessExcluded(DWORD pid) {
    return GetProcInfo(pid).excluded;
}

static bool IsWindowExcluded(HWND hwnd) {
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    return IsProcessExcluded(pid);
}

// Whether we can change the window's opacity at all. Windows of programs
// running at a higher integrity level (as administrator) are protected by
// UIPI, and per-pixel layered windows draw their own transparency.
static bool CanChangeWindow(HWND hwnd) {
    BYTE alpha;
    if (!hwnd || !GetOpacityAlpha(hwnd, &alpha)) {
        return false;
    }
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    if (!pid || pid == GetCurrentProcessId()) {
        return pid != 0;
    }
    return !GetProcInfo(pid).elevated;
}

static int AlphaToPercent(BYTE alpha) {
    return (alpha * 100 + 127) / 255;
}

static void Redraw(HWND hwnd) {
    RedrawWindow(hwnd, nullptr, nullptr,
                 RDW_ERASE | RDW_INVALIDATE | RDW_FRAME | RDW_ALLCHILDREN);
}

static void PruneModified() {
    for (auto it = g_modified.begin(); it != g_modified.end();) {
        if (!IsWindow(it->first)) {
            it = g_modified.erase(it);
        } else {
            ++it;
        }
    }
}

// Sets the opacity in percent. Returns false on failure.
static bool SetOpacityPercent(HWND hwnd, int pct) {
    BYTE cur;
    // Changing the style sends messages to the window's thread and has no
    // timeout, so leave windows that aren't responding alone.
    if (!IsWindow(hwnd) || IsHungAppWindow(hwnd) ||
        !GetOpacityAlpha(hwnd, &cur)) {
        return false;
    }

    LONG_PTR ex = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);
    bool hadLayered = (ex & WS_EX_LAYERED) != 0;
    auto it = g_modified.find(hwnd);

    if (pct >= 100) {
        if (!hadLayered) {
            if (it != g_modified.end()) {
                g_modified.erase(it);
            }
            return true;
        }
        if (!SetLayeredWindowAttributes(hwnd, 0, 255, LWA_ALPHA)) {
            return false;
        }
        if (it != g_modified.end()) {
            if (!it->second.hadLayered) {
                SetWindowLongPtrW(hwnd, GWL_EXSTYLE, ex & ~WS_EX_LAYERED);
                Redraw(hwnd);
                g_modified.erase(it);
            } else if (it->second.origAlpha == 255) {
                g_modified.erase(it);
            }
            // Otherwise keep tracking it, so unloading the mod gives the
            // window its own alpha back.
        }
        return true;
    }

    BYTE alpha = (BYTE)std::clamp((pct * 255 + 50) / 100, 1, 255);

    if (!hadLayered) {
        SetLastError(0);
        SetWindowLongPtrW(hwnd, GWL_EXSTYLE, ex | WS_EX_LAYERED);
        if (!(GetWindowLongPtrW(hwnd, GWL_EXSTYLE) & WS_EX_LAYERED)) {
            return false;  // Most likely an elevated window (UIPI).
        }
    }

    if (!SetLayeredWindowAttributes(hwnd, 0, alpha, LWA_ALPHA)) {
        if (!hadLayered) {
            SetWindowLongPtrW(hwnd, GWL_EXSTYLE, ex);
        }
        return false;
    }

    if (it == g_modified.end()) {
        PruneModified();
        // Remember whether the style was there before and the window's own
        // alpha, so both can be restored.
        g_modified[hwnd] = ModifiedWindow{hadLayered, cur};
    }
    return true;
}

static void RestoreWindow(HWND hwnd, const ModifiedWindow& m) {
    if (!IsWindow(hwnd)) {
        return;
    }
    SetLayeredWindowAttributes(hwnd, 0, m.origAlpha, LWA_ALPHA);
    if (!m.hadLayered) {
        if (IsHungAppWindow(hwnd)) {
            // Removing the style would block until the app responds. It's
            // fully opaque again already, only the style stays.
            Wh_Log(L"Window %p isn't responding, leaving WS_EX_LAYERED", hwnd);
            return;
        }
        LONG_PTR ex = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);
        SetWindowLongPtrW(hwnd, GWL_EXSTYLE, ex & ~WS_EX_LAYERED);
        Redraw(hwnd);
    }
}

static void RestoreAll() {
    for (const auto& [hwnd, m] : g_modified) {
        RestoreWindow(hwnd, m);
    }
    g_modified.clear();
}

// After a settings change: windows of programs that are excluded now get no
// button and ignore the wheel, so give them their original opacity back.
static void RestoreExcluded() {
    for (auto it = g_modified.begin(); it != g_modified.end();) {
        if (!IsWindow(it->first)) {
            it = g_modified.erase(it);
        } else if (IsWindowExcluded(it->first)) {
            RestoreWindow(it->first, it->second);
            it = g_modified.erase(it);
        } else {
            ++it;
        }
    }
}

// Current level expressed as a number of steps below 100%.
static int CurrentStepIndex(int pct) {
    int step = g_settings->step;
    return std::max(0, (100 - pct + step / 2) / step);
}

////////////////////////////////////////////////////////////////////////////////
// Indicator

static void ShowIndicator(const std::wstring& text, POINT anchor, int dpi) {
    if (!g_settings->showIndicator || !g_tipWnd) {
        return;
    }
    g_tipText = text;

    if (!g_tipFont || g_tipFontDpi != dpi) {
        if (g_tipFont) {
            DeleteObject(g_tipFont);
        }
        g_tipFont = CreateFontW(-MulDiv(10, dpi, 72), 0, 0, 0, FW_SEMIBOLD,
                                FALSE, FALSE, FALSE, DEFAULT_CHARSET,
                                OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                                CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Segoe UI");
        g_tipFontDpi = dpi;
    }

    SIZE sz{};
    HDC dc = GetDC(g_tipWnd);
    HGDIOBJ old = SelectObject(dc, g_tipFont);
    GetTextExtentPoint32W(dc, g_tipText.c_str(), (int)g_tipText.size(), &sz);
    SelectObject(dc, old);
    ReleaseDC(g_tipWnd, dc);

    int w = sz.cx + Scale(16, dpi);
    int h = sz.cy + Scale(8, dpi);
    int x = anchor.x;
    int y = anchor.y;

    // Keep it on the monitor.
    MONITORINFO mi{};
    mi.cbSize = sizeof(mi);
    if (GetMonitorInfoW(MonitorFromPoint(anchor, MONITOR_DEFAULTTONEAREST),
                        &mi)) {
        x = std::clamp(x, (int)mi.rcWork.left, (int)mi.rcWork.right - w);
        y = std::clamp(y, (int)mi.rcWork.top, (int)mi.rcWork.bottom - h);
    }

    SetWindowPos(g_tipWnd, HWND_TOPMOST, x, y, w, h,
                 SWP_NOACTIVATE | SWP_SHOWWINDOW);
    InvalidateRect(g_tipWnd, nullptr, TRUE);
    SetTimer(g_tipWnd, kIndicatorTimer, 800, nullptr);
}

static void ShowResult(bool ok, int pct, POINT anchor, int dpi) {
    ShowIndicator(ok ? std::to_wstring(pct) + L"%"
                     : std::wstring(L"Can't change this window"),
                  anchor, dpi);
}

static LRESULT CALLBACK IndicatorWndProc(HWND hwnd, UINT msg, WPARAM wParam,
                                         LPARAM lParam) {
    switch (msg) {
        case WM_TIMER:
            if (wParam == kIndicatorTimer) {
                KillTimer(hwnd, kIndicatorTimer);
                ShowWindow(hwnd, SW_HIDE);
            }
            return 0;
        case WM_NCHITTEST:
            return HTTRANSPARENT;
        case WM_MOUSEACTIVATE:
            return MA_NOACTIVATE;
        case WM_ERASEBKGND:
            return 1;
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC dc = BeginPaint(hwnd, &ps);
            RECT rc;
            GetClientRect(hwnd, &rc);
            HBRUSH bg = CreateSolidBrush(RGB(0x20, 0x20, 0x20));
            FillRect(dc, &rc, bg);
            DeleteObject(bg);
            SetBkMode(dc, TRANSPARENT);
            SetTextColor(dc, RGB(0xFF, 0xFF, 0xFF));
            HGDIOBJ old = SelectObject(dc, g_tipFont);
            DrawTextW(dc, g_tipText.c_str(), (int)g_tipText.size(), &rc,
                      DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
            SelectObject(dc, old);
            EndPaint(hwnd, &ps);
            return 0;
        }
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

////////////////////////////////////////////////////////////////////////////////
// Button

static void HideButton() {
    if (g_btnVisible) {
        ShowWindow(g_btnWnd, SW_HIDE);
        g_btnVisible = false;
        g_btnHover = false;
    }
    SetRectEmpty(&g_btnRect);
}

// Real caption button position, found by hit-testing the title bar. Apps
// that draw their own title bar (Chromium, Electron, VS Code, ...) report
// hidden or wrong DWM caption button bounds, but they answer WM_NCHITTEST
// with HTMINBUTTON/HTMAXBUTTON/HTCLOSE for their custom buttons (Windows 11
// needs that for snap layouts). The result is cached per window and size, so
// moving a window doesn't probe again.
struct CaptionCache {
    HWND hwnd = nullptr;
    int width = 0;
    int height = 0;
    int dpi = 0;
    bool found = false;
    int fromRight = 0;  // distance from the frame's right edge to the
                        // leftmost caption button
    int top = 0;        // caption buttons' top, relative to the frame
    int buttonsHeight = 0;
};
static CaptionCache g_captionCache;

static void ResetCaptionCache() {
    g_captionCache = CaptionCache{};
}

static bool IsCaptionButtonHit(LRESULT r) {
    return r == HTMINBUTTON || r == HTMAXBUTTON || r == HTCLOSE ||
           r == HTHELP;
}

static bool NcHitTest(HWND hwnd, int x, int y, LRESULT* result) {
    DWORD_PTR res = 0;
    if (!SendMessageTimeoutW(hwnd, WM_NCHITTEST, 0,
                             MAKELPARAM((short)x, (short)y), SMTO_ABORTIFHUNG,
                             50, &res)) {
        return false;  // Hung, timed out, or blocked (e.g. elevated window).
    }
    *result = (LRESULT)res;
    return true;
}

static bool ProbeCaptionButtons(HWND hwnd, const RECT& f, int dpi,
                                int* fromRight, int* top, int* height) {
    int width = f.right - f.left;
    if (width < Scale(160, dpi)) {
        return false;
    }
    int step = std::max(2, Scale(4, dpi));
    int maxScan = std::min(width / 2, Scale(420, dpi));
    // A few rows from the top, title bars differ in height. Rows without a
    // button near the right edge stop early, so this stays cheap.
    for (int row = 0; row < 7; row++) {
        int yo = Scale(12 + row * 8, dpi);
        int y = f.top + yo;
        int leftmost = 0;
        bool any = false;
        int gap = 0;
        for (int dx = Scale(2, dpi); dx <= maxScan; dx += step) {
            int x = f.right - dx;
            LRESULT r;
            if (!NcHitTest(hwnd, x, y, &r)) {
                return false;
            }
            if (IsCaptionButtonHit(r)) {
                leftmost = x;
                any = true;
                gap = 0;
            } else if (any) {
                gap += step;
                if (gap > Scale(12, dpi)) {
                    break;
                }
            } else if (dx > Scale(80, dpi)) {
                break;  // Nothing near the right edge on this row.
            }
        }
        if (!any) {
            continue;
        }
        // Exact left edge.
        for (int x = leftmost - 1; x > leftmost - step; x--) {
            LRESULT r;
            if (!NcHitTest(hwnd, x, y, &r) || !IsCaptionButtonHit(r)) {
                break;
            }
            leftmost = x;
        }
        int span = f.right - leftmost;
        if (span < Scale(24, dpi) || span > Scale(420, dpi)) {
            continue;
        }
        // Vertical extent of the buttons.
        int xv = leftmost + std::min(Scale(6, dpi), span / 2);
        int t = yo;
        int b = yo;
        for (int yy = yo - 1; yy >= 0; yy--) {
            LRESULT r;
            if (!NcHitTest(hwnd, xv, f.top + yy, &r) || !IsCaptionButtonHit(r)) {
                break;
            }
            t = yy;
        }
        int vstep = std::max(1, Scale(2, dpi));
        for (int yy = yo + vstep; yy < Scale(80, dpi); yy += vstep) {
            LRESULT r;
            if (!NcHitTest(hwnd, xv, f.top + yy, &r) || !IsCaptionButtonHit(r)) {
                break;
            }
            b = yy;
        }
        *fromRight = span;
        *top = t;
        *height = b - t + 1;
        return true;
    }
    return false;
}

static bool GetCaptionButtons(HWND hwnd, const RECT& f, int dpi,
                              int* fromRight, int* top, int* height) {
    CaptionCache& c = g_captionCache;
    int w = f.right - f.left;
    int h = f.bottom - f.top;
    bool stale =
        c.hwnd != hwnd || c.width != w || c.height != h || c.dpi != dpi;
    // While the window is being resized, keep the offsets found before (they
    // are relative to the frame's top right corner) instead of probing on
    // every location change. It's probed again when the resize ends.
    if (stale && g_inMoveSize && c.hwnd == hwnd && c.dpi == dpi) {
        stale = false;
    }
    if (stale) {
        c.hwnd = hwnd;
        c.width = w;
        c.height = h;
        c.dpi = dpi;
        c.found = ProbeCaptionButtons(hwnd, f, dpi, &c.fromRight, &c.top,
                                      &c.buttonsHeight);
    }
    if (!c.found) {
        return false;
    }
    *fromRight = c.fromRight;
    *top = c.top;
    *height = c.buttonsHeight;
    return true;
}

static void UpdateButton() {
    if (!g_btnWnd) {
        return;
    }
    HWND h = g_target;
    if (!g_settings->buttonEnabled || g_targetExcluded ||
        (!g_targetChangeable && g_settings->hideOnUnchangeable) ||
        !IsEligibleForButton(h)) {
        HideButton();
        return;
    }

    RECT wr;
    if (!GetWindowRect(h, &wr)) {
        HideButton();
        return;
    }

    int dpi = DpiOf(h);
    int bw = Scale(g_settings->buttonWidth, dpi);
    int bh = Scale(g_settings->buttonHeight, dpi);

    int capLeft = 0;
    int y = 0;
    bool haveCaption = false;

    // Visible frame (without the invisible resize borders).
    RECT frame;
    if (FAILED(DwmGetWindowAttribute(h, DWMWA_EXTENDED_FRAME_BOUNDS, &frame,
                                     sizeof(frame)))) {
        frame = wr;
    }
    int fromRight, capTop, capHeight;
    if (GetCaptionButtons(h, frame, dpi, &fromRight, &capTop, &capHeight)) {
        capLeft = frame.right - fromRight;
        y = frame.top + capTop + std::max(0, (capHeight - bh) / 2);
        haveCaption = true;
    }

    RECT cb{};
    if (!haveCaption &&
        SUCCEEDED(DwmGetWindowAttribute(h, DWMWA_CAPTION_BUTTON_BOUNDS, &cb,
                                        sizeof(cb)))) {
        int cw = cb.right - cb.left;
        if (cw > Scale(40, dpi) && cw < Scale(400, dpi)) {
            capLeft = wr.left + cb.left;
            // Maximized windows extend past the monitor by their resize
            // border, center within the visible part of the caption.
            int capTopAbs = std::max((int)(wr.top + cb.top), (int)frame.top);
            int capBottomAbs = wr.top + cb.bottom;
            y = capTopAbs + std::max(0, (capBottomAbs - capTopAbs - bh) / 2);
            haveCaption = true;
        }
    }
    if (!haveCaption) {
        capLeft = frame.right - Scale(138, dpi);
        y = frame.top + Scale(4, dpi);
    }
    int x = capLeft - bw - Scale(g_settings->buttonOffset, dpi);

    // Never put it above the visible top of the window or off the monitor
    // (maximized windows start above the monitor's top edge).
    y = std::max(y, (int)frame.top);
    MONITORINFO mi{};
    mi.cbSize = sizeof(mi);
    if (GetMonitorInfoW(MonitorFromWindow(h, MONITOR_DEFAULTTONEAREST), &mi)) {
        y = std::max(y, (int)mi.rcMonitor.top);
        x = std::clamp(x, (int)mi.rcMonitor.left,
                       std::max((int)mi.rcMonitor.left,
                                (int)mi.rcMonitor.right - bw));
    }

    if (!g_btnFont || g_btnFontDpi != dpi) {
        if (g_btnFont) {
            DeleteObject(g_btnFont);
        }
        g_btnFont = CreateFontW(
            -MulDiv(g_settings->buttonFontSize, dpi, 72), 0, 0, 0, FW_NORMAL,
            FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
            CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH,
            L"Segoe UI Symbol");
        g_btnFontDpi = dpi;
        InvalidateRect(g_btnWnd, nullptr, TRUE);
    }

    RECT newRect{x, y, x + bw, y + bh};
    if (!g_btnVisible || !EqualRect(&newRect, &g_btnRect)) {
        SetWindowPos(g_btnWnd, HWND_TOPMOST, x, y, bw, bh,
                     SWP_NOACTIVATE | SWP_SHOWWINDOW);
        g_btnRect = newRect;
        g_btnVisible = true;
    }
}

static void ForceButtonRefresh() {
    ResetCaptionCache();
    if (g_btnFont) {
        DeleteObject(g_btnFont);
        g_btnFont = nullptr;
    }
    g_btnVisible = g_btnVisible && IsWindowVisible(g_btnWnd);
    SetRectEmpty(&g_btnRect);
    UpdateButton();
    InvalidateRect(g_btnWnd, nullptr, TRUE);
}

static POINT ButtonIndicatorAnchor() {
    return POINT{g_btnRect.left, g_btnRect.bottom + Scale(4, g_btnFontDpi)};
}

static void ButtonCycle() {
    HWND h = g_target;
    if (!h || !IsWindow(h)) {
        return;
    }
    BYTE alpha;
    int dpi = DpiOf(h);
    if (!CanChangeWindow(h) || !GetOpacityAlpha(h, &alpha)) {
        ShowResult(false, 0, ButtonIndicatorAnchor(), dpi);
        return;
    }
    int cur = AlphaToPercent(alpha);
    int next = 100 - (CurrentStepIndex(cur) + 1) * g_settings->step;
    if (next < g_settings->minOpacity) {
        // Land on the minimum once before wrapping back to 100%.
        next = cur > g_settings->minOpacity ? g_settings->minOpacity : 100;
    }
    bool ok = SetOpacityPercent(h, next);
    ShowResult(ok, next, ButtonIndicatorAnchor(), dpi);
}

static void ButtonReset() {
    HWND h = g_target;
    if (!h || !IsWindow(h)) {
        return;
    }
    bool ok = CanChangeWindow(h) && SetOpacityPercent(h, 100);
    ShowResult(ok, 100, ButtonIndicatorAnchor(), DpiOf(h));
}

static COLORREF Lighten(COLORREF c, int amount) {
    auto mix = [amount](int v) { return v + (255 - v) * amount / 100; };
    return RGB(mix(GetRValue(c)), mix(GetGValue(c)), mix(GetBValue(c)));
}

static LRESULT CALLBACK ButtonWndProc(HWND hwnd, UINT msg, WPARAM wParam,
                                      LPARAM lParam) {
    switch (msg) {
        case WM_MOUSEACTIVATE:
            return MA_NOACTIVATE;
        case WM_MOUSEMOVE:
            if (!g_btnHover) {
                g_btnHover = true;
                TRACKMOUSEEVENT tme{sizeof(tme), TME_LEAVE, hwnd, 0};
                TrackMouseEvent(&tme);
                InvalidateRect(hwnd, nullptr, FALSE);
            }
            return 0;
        case WM_MOUSELEAVE:
            g_btnHover = false;
            InvalidateRect(hwnd, nullptr, FALSE);
            return 0;
        case WM_LBUTTONDOWN:
            SetCapture(hwnd);
            return 0;
        case WM_LBUTTONUP: {
            if (GetCapture() == hwnd) {
                ReleaseCapture();
                POINT pt{(short)LOWORD(lParam), (short)HIWORD(lParam)};
                RECT rc;
                GetClientRect(hwnd, &rc);
                if (PtInRect(&rc, pt)) {
                    ButtonCycle();
                }
            }
            return 0;
        }
        case WM_RBUTTONUP:
            ButtonReset();
            return 0;
        case WM_CONTEXTMENU:
            return 0;
        case WM_ERASEBKGND:
            return 1;
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC dc = BeginPaint(hwnd, &ps);
            RECT rc;
            GetClientRect(hwnd, &rc);
            COLORREF bgColor = g_btnHover ? Lighten(g_settings->buttonColor, 18)
                                          : g_settings->buttonColor;
            HBRUSH bg = CreateSolidBrush(bgColor);
            FillRect(dc, &rc, bg);
            DeleteObject(bg);
            SetBkMode(dc, TRANSPARENT);
            COLORREF fg = g_settings->buttonTextColor;
            if (!g_targetChangeable) {
                // Dimmed: this window can't be changed.
                auto mix = [](int a, int b) { return (a * 35 + b * 65) / 100; };
                fg = RGB(mix(GetRValue(fg), GetRValue(bgColor)),
                         mix(GetGValue(fg), GetGValue(bgColor)),
                         mix(GetBValue(fg), GetBValue(bgColor)));
            }
            SetTextColor(dc, fg);
            HGDIOBJ old = g_btnFont ? SelectObject(dc, g_btnFont) : nullptr;
            const std::wstring& text = g_settings->buttonText;
            DrawTextW(dc, text.c_str(), (int)text.size(), &rc,
                      DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
            if (old) {
                SelectObject(dc, old);
            }
            EndPaint(hwnd, &ps);
            return 0;
        }
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

////////////////////////////////////////////////////////////////////////////////
// Window events (event driven, no polling)

static void CALLBACK WinEventProc(HWINEVENTHOOK hook, DWORD event, HWND hwnd,
                                  LONG idObject, LONG idChild, DWORD thread,
                                  DWORD time);

static void UnhookTargetEvents() {
    if (g_targetLocHook) {
        UnhookWinEvent(g_targetLocHook);
        g_targetLocHook = nullptr;
    }
    if (g_targetShowHideHook) {
        UnhookWinEvent(g_targetShowHideHook);
        g_targetShowHideHook = nullptr;
    }
    if (g_targetCloakHook) {
        UnhookWinEvent(g_targetCloakHook);
        g_targetCloakHook = nullptr;
    }
    if (g_targetMoveSizeHook) {
        UnhookWinEvent(g_targetMoveSizeHook);
        g_targetMoveSizeHook = nullptr;
    }
    g_targetHookThread = 0;
    g_inMoveSize = false;
}

static void SetTarget(HWND hwnd) {
    if (hwnd && (hwnd == g_btnWnd || hwnd == g_tipWnd || hwnd == g_ctlWnd)) {
        return;
    }

    DWORD pid = 0;
    DWORD tid = hwnd ? GetWindowThreadProcessId(hwnd, &pid) : 0;

    // Probe the title bar again whenever a window is activated, apps may
    // have finished drawing it by now.
    ResetCaptionCache();

    if (hwnd != g_target) {
        g_target = hwnd;
        g_inMoveSize = false;
        g_targetExcluded = hwnd ? IsProcessExcluded(pid) : true;
        bool changeable = hwnd && CanChangeWindow(hwnd);
        if (changeable != g_targetChangeable) {
            g_targetChangeable = changeable;
            if (g_btnWnd) {
                InvalidateRect(g_btnWnd, nullptr, TRUE);
            }
        }
    }

    // Only listen to move/resize/hide events of the active window's thread,
    // so we don't get flooded by events from every program.
    if (tid != g_targetHookThread) {
        UnhookTargetEvents();
        if (tid && !g_targetExcluded) {
            g_targetLocHook = SetWinEventHook(
                EVENT_OBJECT_LOCATIONCHANGE, EVENT_OBJECT_LOCATIONCHANGE,
                nullptr, WinEventProc, pid, tid, WINEVENT_OUTOFCONTEXT);
            g_targetShowHideHook = SetWinEventHook(
                EVENT_OBJECT_DESTROY, EVENT_OBJECT_HIDE, nullptr, WinEventProc,
                pid, tid, WINEVENT_OUTOFCONTEXT);
            // Windows 11 apps often become active while still cloaked and
            // uncloak a moment later.
            g_targetCloakHook = SetWinEventHook(
                EVENT_OBJECT_CLOAKED, EVENT_OBJECT_UNCLOAKED, nullptr,
                WinEventProc, pid, tid, WINEVENT_OUTOFCONTEXT);
            // Resize drags: keep the caption offset until the drag ends.
            g_targetMoveSizeHook = SetWinEventHook(
                EVENT_SYSTEM_MOVESIZESTART, EVENT_SYSTEM_MOVESIZEEND, nullptr,
                WinEventProc, pid, tid, WINEVENT_OUTOFCONTEXT);
            g_targetHookThread = tid;
        }
    }

    UpdateButton();
}

// Popups (menus, access key tips) of an app: keep the button on the app's
// main window.
static HWND ResolveTarget(HWND hwnd) {
    if (hwnd && !IsEligibleForButton(hwnd)) {
        HWND owner = GetAncestor(hwnd, GA_ROOTOWNER);
        if (owner && owner != hwnd && IsEligibleForButton(owner)) {
            return owner;
        }
    }
    return hwnd;
}

static void RecheckForeground() {
    HWND fg = ResolveTarget(GetForegroundWindow());
    if (fg && fg != g_target) {
        SetTarget(fg);
    }
}

static void CALLBACK WinEventProc(HWINEVENTHOOK hook, DWORD event, HWND hwnd,
                                  LONG idObject, LONG idChild, DWORD thread,
                                  DWORD time) {
    switch (event) {
        case EVENT_SYSTEM_FOREGROUND:
            SetTarget(ResolveTarget(hwnd));
            // The event is delivered asynchronously, and with focus stealing
            // prevention Windows sometimes reports a window that didn't
            // actually end up in front. Check once more a moment later.
            SetTimer(g_ctlWnd, kForegroundRecheckTimer, 300, nullptr);
            break;

        case EVENT_SYSTEM_CAPTURESTART:
        case EVENT_OBJECT_FOCUS:
            // Windows doesn't always send EVENT_SYSTEM_FOREGROUND, e.g. when a
            // program that was launched in the background (and already got
            // the event back then) is activated later. Focus changes and
            // mouse clicks (capture start) catch it.
            if (GetForegroundWindow() != g_target) {
                RecheckForeground();
            }
            break;

        case EVENT_SYSTEM_MINIMIZESTART:
        case EVENT_SYSTEM_MINIMIZEEND:
            if (hwnd == g_target) {
                UpdateButton();
            }
            break;

        case EVENT_SYSTEM_MOVESIZESTART:
            if (hwnd == g_target) {
                g_inMoveSize = true;
            }
            break;

        case EVENT_SYSTEM_MOVESIZEEND:
            if (hwnd == g_target) {
                g_inMoveSize = false;
                UpdateButton();  // Probes again if the size changed.
            }
            break;

        case EVENT_OBJECT_LOCATIONCHANGE:
        case EVENT_OBJECT_SHOW:
        case EVENT_OBJECT_CLOAKED:
        case EVENT_OBJECT_UNCLOAKED:
            if (hwnd == g_target && idObject == OBJID_WINDOW &&
                idChild == CHILDID_SELF) {
                UpdateButton();
            }
            break;

        case EVENT_OBJECT_HIDE:
            if (hwnd == g_target && idObject == OBJID_WINDOW &&
                idChild == CHILDID_SELF) {
                // If a temporary window was the target, go back to whatever
                // is active now.
                HWND fg = GetForegroundWindow();
                if (fg && fg != hwnd) {
                    SetTarget(fg);
                } else {
                    UpdateButton();
                }
            }
            break;

        case EVENT_OBJECT_DESTROY:
            if (hwnd == g_target && idObject == OBJID_WINDOW &&
                idChild == CHILDID_SELF) {
                g_target = nullptr;
                HideButton();
                UnhookTargetEvents();
                HWND fg = GetForegroundWindow();
                if (fg && fg != hwnd) {
                    SetTarget(fg);
                }
            }
            if (idObject == OBJID_WINDOW && idChild == CHILDID_SELF) {
                g_modified.erase(hwnd);
            }
            break;
    }
}

////////////////////////////////////////////////////////////////////////////////
// Modifier + wheel / middle click
//
// Everything up to SendMaskKey runs on the hook thread.

static Modifier HookModifier() {
    return (Modifier)g_hkModifier.load(std::memory_order_relaxed);
}

static void PostToHookThread(UINT msg, WPARAM wParam = 0) {
    DWORD id = g_hookThreadId.load();
    if (id) {
        PostThreadMessageW(id, msg, wParam, 0);
    }
}

static bool IsModifierVk(DWORD vk) {
    switch (HookModifier()) {
        case Modifier::Alt:
            return vk == VK_MENU || vk == VK_LMENU || vk == VK_RMENU;
        case Modifier::Ctrl:
            return vk == VK_CONTROL || vk == VK_LCONTROL || vk == VK_RCONTROL;
        case Modifier::Shift:
            return vk == VK_SHIFT || vk == VK_LSHIFT || vk == VK_RSHIFT;
        case Modifier::Win:
            return vk == VK_LWIN || vk == VK_RWIN;
    }
    return false;
}

static bool IsModifierDown() {
    auto down = [](int vk) { return (GetAsyncKeyState(vk) & 0x8000) != 0; };
    switch (HookModifier()) {
        case Modifier::Alt:
            return down(VK_MENU);
        case Modifier::Ctrl:
            return down(VK_CONTROL);
        case Modifier::Shift:
            return down(VK_SHIFT);
        case Modifier::Win:
            return down(VK_LWIN) || down(VK_RWIN);
    }
    return false;
}

static void RequestMaskKey() {
    // Releasing Alt/Win after a swallowed wheel would otherwise open the
    // app's menu bar or the Start menu.
    Modifier mod = HookModifier();
    if (!g_maskSent && (mod == Modifier::Alt || mod == Modifier::Win)) {
        g_maskSent = true;
        PostMessageW(g_ctlWnd, WM_APP_MASK_KEY, 0, 0);
    }
}

// Whether the worker will act on a wheel / middle click at this point. If not,
// the event is passed on, so excluded programs, the taskbar, the desktop and
// windows that can't be changed still get modifier + wheel. Only uses calls
// that don't send messages to other threads, so it stays cheap.
static bool WillHandleAt(POINT pt) {
    HWND h = WindowFromPoint(pt);
    if (!h) {
        return false;
    }
    HWND root = GetAncestor(h, GA_ROOT);
    if (!root || root == g_tipWnd) {
        return false;
    }
    if (root == g_btnWnd) {
        return true;
    }
    if (IsShellClass(root)) {
        return false;
    }
    DWORD pid = 0;
    GetWindowThreadProcessId(root, &pid);
    if (!pid) {
        return false;
    }
    ProcInfo info = GetProcInfo(pid);
    if (info.excluded) {
        return false;
    }
    if (g_hkHideUnchangeable.load(std::memory_order_relaxed)) {
        BYTE alpha;
        if (info.elevated || !GetOpacityAlpha(root, &alpha)) {
            return false;
        }
    }
    return true;
}

static LRESULT CALLBACK MouseHookProc(int code, WPARAM wParam, LPARAM lParam) {
    if (code == HC_ACTION) {
        auto* m = (MSLLHOOKSTRUCT*)lParam;
        LPARAM pt = MAKELPARAM((short)m->pt.x, (short)m->pt.y);
        switch (wParam) {
            case WM_MOUSEWHEEL:
                if (!IsModifierDown()) {
                    PostToHookThread(WM_HK_DISARM_MOUSE);
                    break;
                }
                if (WillHandleAt(m->pt)) {
                    PostMessageW(g_ctlWnd, WM_APP_WHEEL,
                                 (WPARAM)(short)HIWORD(m->mouseData), pt);
                    RequestMaskKey();
                    return 1;
                }
                break;
            case WM_MBUTTONDOWN:
                if (IsModifierDown() && WillHandleAt(m->pt)) {
                    g_swallowMiddleUp = true;
                    PostMessageW(g_ctlWnd, WM_APP_MCLICK, 0, pt);
                    RequestMaskKey();
                    return 1;
                }
                break;
            case WM_MBUTTONUP:
                if (g_swallowMiddleUp) {
                    g_swallowMiddleUp = false;
                    if (!IsModifierDown()) {
                        PostToHookThread(WM_HK_DISARM_MOUSE);
                    }
                    return 1;
                }
                break;
        }
    }
    return CallNextHookEx(nullptr, code, wParam, lParam);
}

static LRESULT CALLBACK KeyboardHookProc(int code, WPARAM wParam,
                                         LPARAM lParam) {
    if (code == HC_ACTION) {
        auto* k = (KBDLLHOOKSTRUCT*)lParam;
        if (k->dwExtraInfo != kInjectedMarker && IsModifierVk(k->vkCode)) {
            bool down = wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN;
            if (down) {
                if (!g_mouseHook) {
                    PostToHookThread(WM_HK_ARM_MOUSE);
                }
            } else {
                // wParam = 1: the modifier was released. The async key state
                // isn't updated yet while the hook runs, so don't rely on it.
                PostToHookThread(WM_HK_DISARM_MOUSE, 1);
            }
        }
    }
    return CallNextHookEx(nullptr, code, wParam, lParam);
}

static void DisarmMouseHook(bool force, bool released = false) {
    if (released) {
        // The mask key is needed once per modifier hold.
        g_maskSent = false;
    }
    if (!g_mouseHook) {
        return;
    }
    if (!force && (g_swallowMiddleUp || (!released && IsModifierDown()))) {
        return;
    }
    UnhookWindowsHookEx(g_mouseHook);
    g_mouseHook = nullptr;
    g_maskSent = false;
    g_swallowMiddleUp = false;
}

static void ArmMouseHook() {
    if (g_mouseHook || !g_hkWheelEnabled.load()) {
        return;
    }
    g_maskSent = false;
    g_mouseHook = SetWindowsHookExW(WH_MOUSE_LL, MouseHookProc,
                                    GetModuleHandleW(nullptr), 0);
}

static void ApplyHookSettings(bool modifierChanged) {
    if (modifierChanged) {
        DisarmMouseHook(true);
    }
    if (g_hkWheelEnabled.load()) {
        if (!g_kbHook) {
            g_kbHook = SetWindowsHookExW(WH_KEYBOARD_LL, KeyboardHookProc,
                                         GetModuleHandleW(nullptr), 0);
            if (!g_kbHook) {
                Wh_Log(L"Keyboard hook failed: %u", GetLastError());
            }
        }
    } else {
        DisarmMouseHook(true);
        if (g_kbHook) {
            UnhookWindowsHookEx(g_kbHook);
            g_kbHook = nullptr;
        }
    }
}

// Only pumps messages: low-level hook procs are called while it waits in
// GetMessage, and they never wait on anything themselves.
static DWORD WINAPI HookThreadProc(LPVOID param) {
    MSG msg;
    // Create the message queue before anyone posts to it.
    PeekMessageW(&msg, nullptr, WM_USER, WM_USER, PM_NOREMOVE);
    g_hookThreadId = GetCurrentThreadId();
    ApplyHookSettings(false);
    SetEvent((HANDLE)param);

    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        if (msg.hwnd) {
            DispatchMessageW(&msg);
            continue;
        }
        switch (msg.message) {
            case WM_HK_ARM_MOUSE:
                // Posted on modifier key down. Don't check the async key
                // state here, it may not be updated yet.
                ArmMouseHook();
                break;
            case WM_HK_DISARM_MOUSE:
                DisarmMouseHook(false, msg.wParam != 0);
                break;
            case WM_HK_APPLY_SETTINGS:
                ApplyHookSettings(msg.wParam != 0);
                break;
        }
    }

    DisarmMouseHook(true);
    if (g_kbHook) {
        UnhookWindowsHookEx(g_kbHook);
        g_kbHook = nullptr;
    }
    return 0;
}

// Runs on the worker (or before any thread starts).
static void PublishHookSettings(const Settings* s, bool modifierChanged) {
    SetExcludedProcesses(s->excludedProcesses);
    g_hkModifier = (int)s->wheelModifier;
    g_hkWheelEnabled = s->wheelEnabled;
    g_hkHideUnchangeable = s->hideOnUnchangeable;
    PostToHookThread(WM_HK_APPLY_SETTINGS, modifierChanged ? 1 : 0);
}

////////////////////////////////////////////////////////////////////////////////
// Worker side of modifier + wheel / middle click

static void SendMaskKey() {
    INPUT in[2]{};
    for (auto& i : in) {
        i.type = INPUT_KEYBOARD;
        i.ki.wVk = 0xE8;  // Unassigned virtual key.
        i.ki.dwExtraInfo = kInjectedMarker;
    }
    in[1].ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(2, in, sizeof(INPUT));
}

static HWND WindowForPoint(POINT pt) {
    HWND h = WindowFromPoint(pt);
    if (!h) {
        return nullptr;
    }
    HWND root = GetAncestor(h, GA_ROOT);
    if (!root) {
        return nullptr;
    }
    if (root == g_btnWnd) {
        return g_target;
    }
    if (root == g_tipWnd || IsShellClass(root) || IsWindowExcluded(root)) {
        return nullptr;
    }
    return root;
}

static void HandleWheel(int delta, POINT pt) {
    HWND h = WindowForPoint(pt);
    if (!h) {
        return;
    }
    if (h != g_wheelTarget) {
        g_wheelTarget = h;
        g_wheelAccum = 0;
    }
    g_wheelAccum += delta;
    int notches = g_wheelAccum / WHEEL_DELTA;
    if (notches == 0) {
        return;
    }
    g_wheelAccum -= notches * WHEEL_DELTA;

    int dpi = DpiOf(h);
    POINT anchor{pt.x + Scale(16, dpi), pt.y + Scale(16, dpi)};
    BYTE alpha;
    if (!CanChangeWindow(h) || !GetOpacityAlpha(h, &alpha)) {
        if (g_settings->hideOnUnchangeable) {
            return;
        }
        ShowResult(false, 0, anchor, dpi);
        return;
    }
    int step = g_settings->step;
    int pct = 100 - (CurrentStepIndex(AlphaToPercent(alpha)) - notches) * step;
    pct = std::clamp(pct, g_settings->minOpacity, 100);
    bool ok = SetOpacityPercent(h, pct);
    ShowResult(ok, pct, anchor, dpi);
}

static void HandleMiddleClick(POINT pt) {
    HWND h = WindowForPoint(pt);
    if (!h) {
        return;
    }
    int dpi = DpiOf(h);
    if (!CanChangeWindow(h) && g_settings->hideOnUnchangeable) {
        return;
    }
    bool ok = SetOpacityPercent(h, 100);
    ShowResult(ok, 100, POINT{pt.x + Scale(16, dpi), pt.y + Scale(16, dpi)},
               dpi);
}

////////////////////////////////////////////////////////////////////////////////
// Control window and worker thread

static void Shutdown() {
    UnhookTargetEvents();
    if (g_fgHook) {
        UnhookWinEvent(g_fgHook);
        g_fgHook = nullptr;
    }
    if (g_minHook) {
        UnhookWinEvent(g_minHook);
        g_minHook = nullptr;
    }
    if (g_focusHook) {
        UnhookWinEvent(g_focusHook);
        g_focusHook = nullptr;
    }
    if (g_captureHook) {
        UnhookWinEvent(g_captureHook);
        g_captureHook = nullptr;
    }
    RestoreAll();
    if (g_btnWnd) {
        DestroyWindow(g_btnWnd);
        g_btnWnd = nullptr;
    }
    if (g_tipWnd) {
        DestroyWindow(g_tipWnd);
        g_tipWnd = nullptr;
    }
    if (g_btnFont) {
        DeleteObject(g_btnFont);
        g_btnFont = nullptr;
    }
    if (g_tipFont) {
        DeleteObject(g_tipFont);
        g_tipFont = nullptr;
    }
}

static LRESULT CALLBACK ControlWndProc(HWND hwnd, UINT msg, WPARAM wParam,
                                       LPARAM lParam) {
    switch (msg) {
        case WM_APP_SETTINGS: {
            Settings* s = (Settings*)lParam;
            Modifier oldModifier = g_settings->wheelModifier;
            delete g_settings;
            g_settings = s;
            PublishHookSettings(g_settings,
                                oldModifier != g_settings->wheelModifier);
            RestoreExcluded();
            // Re-evaluate the exclusion list for the current window.
            HWND target = g_target;
            g_target = nullptr;
            g_targetHookThread = 0;
            UnhookTargetEvents();
            SetTarget(target ? target : GetForegroundWindow());
            ForceButtonRefresh();
            return 0;
        }
        case WM_APP_WHEEL:
            HandleWheel((int)(short)wParam,
                        POINT{(short)LOWORD(lParam), (short)HIWORD(lParam)});
            return 0;
        case WM_APP_MCLICK:
            HandleMiddleClick(
                POINT{(short)LOWORD(lParam), (short)HIWORD(lParam)});
            return 0;
        case WM_APP_MASK_KEY:
            SendMaskKey();
            return 0;
        case WM_TIMER:
            if (wParam == kForegroundRecheckTimer) {
                KillTimer(hwnd, kForegroundRecheckTimer);
                RecheckForeground();
                return 0;
            }
            break;
        case WM_DISPLAYCHANGE:
        case WM_SETTINGCHANGE:
            ForceButtonRefresh();
            return 0;
        case WM_APP_QUIT:
            Shutdown();
            DestroyWindow(hwnd);
            return 0;
        case WM_DESTROY:
            g_ctlWnd = nullptr;
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

static bool CreateWindows() {
    HINSTANCE inst = GetModuleHandleW(nullptr);

    WNDCLASSEXW wc{};
    wc.cbSize = sizeof(wc);
    wc.hInstance = inst;
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);

    wc.lpfnWndProc = ControlWndProc;
    wc.lpszClassName = kControlClass;
    RegisterClassExW(&wc);

    wc.lpfnWndProc = ButtonWndProc;
    wc.lpszClassName = kButtonClass;
    RegisterClassExW(&wc);

    wc.lpfnWndProc = IndicatorWndProc;
    wc.lpszClassName = kIndicatorClass;
    RegisterClassExW(&wc);

    // A top-level (not message-only) control window so it receives
    // WM_DISPLAYCHANGE / WM_SETTINGCHANGE broadcasts. It's never shown.
    g_ctlWnd = CreateWindowExW(WS_EX_TOOLWINDOW, kControlClass, L"", WS_POPUP,
                               0, 0, 0, 0, nullptr, nullptr, inst, nullptr);
    if (!g_ctlWnd) {
        return false;
    }

    DWORD noActivate = WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE;
    g_btnWnd = CreateWindowExW(noActivate, kButtonClass, L"Opacity", WS_POPUP,
                               0, 0, 0, 0, nullptr, nullptr, inst, nullptr);
    g_tipWnd = CreateWindowExW(noActivate | WS_EX_LAYERED | WS_EX_TRANSPARENT,
                               kIndicatorClass, L"", WS_POPUP, 0, 0, 0, 0,
                               nullptr, nullptr, inst, nullptr);
    if (!g_btnWnd || !g_tipWnd) {
        return false;
    }
    SetLayeredWindowAttributes(g_tipWnd, 0, 235, LWA_ALPHA);

    BOOL disable = TRUE;
    DWM_WINDOW_CORNER_PREFERENCE corner = DWMWCP_ROUNDSMALL;  // Windows 11
    for (HWND w : {g_btnWnd, g_tipWnd}) {
        DwmSetWindowAttribute(w, DWMWA_TRANSITIONS_FORCEDISABLED, &disable,
                              sizeof(disable));
        DwmSetWindowAttribute(w, DWMWA_WINDOW_CORNER_PREFERENCE, &corner,
                              sizeof(corner));
    }
    return true;
}

static DWORD WINAPI ThreadProc(LPVOID) {
    g_initOk = CreateWindows();
    if (g_initOk) {
        g_fgHook = SetWinEventHook(EVENT_SYSTEM_FOREGROUND,
                                   EVENT_SYSTEM_FOREGROUND, nullptr,
                                   WinEventProc, 0, 0,
                                   WINEVENT_OUTOFCONTEXT |
                                       WINEVENT_SKIPOWNPROCESS);
        g_minHook = SetWinEventHook(EVENT_SYSTEM_MINIMIZESTART,
                                    EVENT_SYSTEM_MINIMIZEEND, nullptr,
                                    WinEventProc, 0, 0,
                                    WINEVENT_OUTOFCONTEXT |
                                        WINEVENT_SKIPOWNPROCESS);
        g_focusHook = SetWinEventHook(EVENT_OBJECT_FOCUS, EVENT_OBJECT_FOCUS,
                                      nullptr, WinEventProc, 0, 0,
                                      WINEVENT_OUTOFCONTEXT |
                                          WINEVENT_SKIPOWNPROCESS);
        g_captureHook = SetWinEventHook(EVENT_SYSTEM_CAPTURESTART,
                                        EVENT_SYSTEM_CAPTURESTART, nullptr,
                                        WinEventProc, 0, 0,
                                        WINEVENT_OUTOFCONTEXT |
                                            WINEVENT_SKIPOWNPROCESS);
        SetTarget(GetForegroundWindow());
    } else {
        Wh_Log(L"Failed to create windows: %u", GetLastError());
        Shutdown();
        if (g_ctlWnd) {
            DestroyWindow(g_ctlWnd);
            g_ctlWnd = nullptr;
        }
    }
    SetEvent(g_readyEvent);
    if (!g_initOk) {
        return 1;
    }

    MSG msg;
    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    return 0;
}

////////////////////////////////////////////////////////////////////////////////
// Mod callbacks (run in the dedicated tool process)

BOOL WhTool_ModInit() {
    Wh_Log(L">");
    g_settings = LoadSettings();
    PublishHookSettings(g_settings, false);
    g_readyEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_readyEvent) {
        return FALSE;
    }
    g_thread = CreateThread(nullptr, 0, ThreadProc, nullptr, 0, nullptr);
    if (!g_thread) {
        return FALSE;
    }
    WaitForSingleObject(g_readyEvent, INFINITE);
    CloseHandle(g_readyEvent);
    g_readyEvent = nullptr;
    if (!g_initOk) {
        WaitForSingleObject(g_thread, INFINITE);
        CloseHandle(g_thread);
        g_thread = nullptr;
        return FALSE;
    }

    // Started after the worker created its windows, stopped before they're
    // destroyed.
    HANDLE hookReady = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (hookReady) {
        g_hookThread =
            CreateThread(nullptr, 0, HookThreadProc, hookReady, 0, nullptr);
        if (g_hookThread) {
            WaitForSingleObject(hookReady, INFINITE);
        }
        CloseHandle(hookReady);
    }
    if (!g_hookThread) {
        Wh_Log(L"Failed to start the hook thread, wheel shortcut disabled");
    }
    return TRUE;
}

void WhTool_ModSettingsChanged() {
    Wh_Log(L">");
    Settings* s = LoadSettings();
    if (!g_ctlWnd || !PostMessageW(g_ctlWnd, WM_APP_SETTINGS, 0, (LPARAM)s)) {
        delete s;
    }
}

void WhTool_ModUninit() {
    Wh_Log(L">");
    if (g_hookThread) {
        PostThreadMessageW(g_hookThreadId, WM_QUIT, 0, 0);
        if (WaitForSingleObject(g_hookThread, 5000) != WAIT_OBJECT_0) {
            Wh_Log(L"Hook thread didn't exit in time");
        }
        CloseHandle(g_hookThread);
        g_hookThread = nullptr;
        g_hookThreadId = 0;
    }
    if (g_thread) {
        if (g_ctlWnd) {
            PostMessageW(g_ctlWnd, WM_APP_QUIT, 0, 0);
        }
        // Give it time to restore the opacity of every window it changed.
        if (WaitForSingleObject(g_thread, 5000) != WAIT_OBJECT_0) {
            Wh_Log(L"Worker thread didn't exit in time");
        }
        CloseHandle(g_thread);
        g_thread = nullptr;
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
