// ==WindhawkMod==
// @id              scroll-window-opacity
// @name            Scroll Window Opacity
// @description     Hold a key combination and scroll the mouse wheel to change the opacity of the window under the cursor, with an optional title bar button
// @version         1.1.0
// @author          Sondre Myrmel, JeremyWayne93
// @github          https://github.com/Sondre234
// @twitter         https://x.com/ShaolinLoL
// @include         windhawk.exe
// @architecture    x86
// @architecture    x86-64
// @compilerOptions -ldwmapi -lgdi32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Scroll Window Opacity

Hold a configurable key combination (default: **Ctrl + Alt**) and scroll the mouse
wheel to adjust the transparency of the top-level window under the cursor.

- **Scroll up** - decrease opacity (more transparent)
- **Scroll down** - increase opacity (more opaque)

The opacity step, minimum opacity, and modifier keys are all configurable in the
settings panel.

Common shell windows such as the taskbar and desktop are excluded. For windows
that this mod makes layered, the original window style is restored when returned
to 100% opacity.

## Optional extras

These are all off by default, so the mod works exactly as before until you turn
them on in the settings. Each one can be turned on by itself.

- **Title bar button** - a small button sits just left of the minimize button of
  the active window and follows it. **Left-click** makes the window more
  transparent by the button step, landing on the minimum opacity once, and the
  next click goes back to 100%. **Right-click** goes straight back to 100%.
  Scrolling with the modifier key(s) over the button changes the window it
  belongs to. The button is placed by hit-testing the title bar, so it also
  lines up in apps that draw their own title bar (Chrome, Edge, VS Code,
  Electron apps). It's hidden on fullscreen windows, excluded programs and
  windows of programs running as administrator (which can't be changed).
- **Middle-click reset** - hold the modifier key(s) and middle-click a window to
  put it back to 100% opacity.
- **Percentage indicator** - briefly shows the new opacity next to the cursor
  (or under the button) after each change.
- **Excluded programs** - process names (for example `vlc.exe`) that the wheel,
  the button and middle-click leave alone.

The button and the indicator are small windows of the mod's own process. They
are event driven (`SetWinEventHook`), so nothing runs while nothing changes,
and the window events are only hooked while the button is enabled.

## Credits

The title bar button, middle-click reset, percentage indicator and excluded
programs were contributed by
[JeremyWayne93](https://github.com/JeremyWayne93), ported from his Titlebar
Opacity Button mod.

## Changelog

### 1.1.0
- Optional title bar button next to the minimize button (off by default).
- Optional modifier + middle-click to reset a window to 100% (off by default).
- Optional percentage indicator (off by default).
- Optional list of excluded programs (empty by default).
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- modifierKey:
  - value: ctrl+alt
    $name: Ctrl + Alt
  - value: ctrl+shift
    $name: Ctrl + Shift
  - value: alt+shift
    $name: Alt + Shift
  - value: ctrl
    $name: Ctrl only
  - value: alt
    $name: Alt only
  - value: shift
    $name: Shift only
  $name: Modifier Key(s)
  $description: Key(s) to hold while scrolling the mouse wheel to change opacity

- opacityStep: 1
  $name: Opacity Step (%)
  $description: How much opacity changes per wheel notch (1-50)

- minOpacity: 10
  $name: Minimum Opacity (%)
  $description: Lowest opacity a window can be set to (1-90)

- titlebarButton: false
  $name: Title Bar Button
  $description: Show a small opacity button next to the minimize button of the active window. Left-click makes the window more transparent, right-click resets it to 100%

- buttonStep: 10
  $name: Title Bar Button Step (%)
  $description: How much opacity changes per button click (1-50)

- buttonOffset: 4
  $name: Title Bar Button Offset
  $description: Gap in pixels (at 100% scaling) between the button and the minimize button. Negative values move it closer

- middleClickReset: false
  $name: Middle-Click Reset
  $description: Hold the modifier key(s) and middle-click a window to restore it to 100% opacity

- showIndicator: false
  $name: Show Percentage Indicator
  $description: Briefly show the new opacity after each change

- excludedPrograms: [""]
  $name: Excluded Programs
  $description: Process names (for example vlc.exe) whose windows are never changed
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <dwmapi.h>
#include <shellapi.h>

#include <algorithm>
#include <cstdlib>
#include <cwctype>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

struct Settings {
    bool needCtrl  = true;
    bool needAlt   = true;
    bool needShift = false;
    int  step      = 1;
    int  minPct    = 10;

    // Optional extras, all off by default.
    bool titlebarButton   = false;
    int  buttonStep       = 10;
    int  buttonOffset     = 4;
    bool middleClickReset = false;
    bool showIndicator    = false;
} g_settings;

static void SetExcludedPrograms(std::vector<std::wstring> list);

void LoadSettings() {
    PCWSTR raw = Wh_GetStringSetting(L"modifierKey");
    std::wstring key = (raw && raw[0] != L'\0') ? raw : L"ctrl+alt";
    Wh_FreeStringSetting(raw);

    g_settings.needCtrl  = key.find(L"ctrl")  != std::wstring::npos;
    g_settings.needAlt   = key.find(L"alt")   != std::wstring::npos;
    g_settings.needShift = key.find(L"shift") != std::wstring::npos;

    if (!g_settings.needCtrl && !g_settings.needAlt && !g_settings.needShift) {
        g_settings.needCtrl = true;
        g_settings.needAlt  = true;
    }

    int step = Wh_GetIntSetting(L"opacityStep");
    g_settings.step = (step >= 1 && step <= 50) ? step : 1;

    int minPct = Wh_GetIntSetting(L"minOpacity");
    g_settings.minPct = (minPct >= 1 && minPct <= 90) ? minPct : 10;

    g_settings.titlebarButton = Wh_GetIntSetting(L"titlebarButton") != 0;

    int buttonStep = Wh_GetIntSetting(L"buttonStep");
    g_settings.buttonStep = (buttonStep >= 1 && buttonStep <= 50) ? buttonStep : 10;

    int buttonOffset = Wh_GetIntSetting(L"buttonOffset");
    g_settings.buttonOffset = (buttonOffset >= -300 && buttonOffset <= 300) ? buttonOffset : 4;

    g_settings.middleClickReset = Wh_GetIntSetting(L"middleClickReset") != 0;
    g_settings.showIndicator    = Wh_GetIntSetting(L"showIndicator") != 0;

    std::vector<std::wstring> excluded;
    for (int i = 0; i < 256; i++) {
        PCWSTR rawName = Wh_GetStringSetting(L"excludedPrograms[%d]", i);
        std::wstring name = rawName ? rawName : L"";
        Wh_FreeStringSetting(rawName);

        size_t first = name.find_first_not_of(L" \t");
        size_t last  = name.find_last_not_of(L" \t");
        name = (first == std::wstring::npos) ? L"" : name.substr(first, last - first + 1);
        if (name.empty()) {
            break;
        }

        for (auto& c : name) c = towlower(c);
        excluded.push_back(name);
    }
    size_t excludedCount = excluded.size();
    SetExcludedPrograms(std::move(excluded));

    Wh_Log(L"Settings: ctrl=%d alt=%d shift=%d step=%d minPct=%d",
           g_settings.needCtrl,
           g_settings.needAlt,
           g_settings.needShift,
           g_settings.step,
           g_settings.minPct);

    Wh_Log(L"Extras: button=%d buttonStep=%d buttonOffset=%d middleClick=%d indicator=%d excluded=%d",
           g_settings.titlebarButton,
           g_settings.buttonStep,
           g_settings.buttonOffset,
           g_settings.middleClickReset,
           g_settings.showIndicator,
           (int)excludedCount);
}

struct WindowState {
    int  pct;
    bool hadLayered;
};

std::unordered_map<HWND, WindowState> g_states;
std::mutex g_mutex;

static bool IsModifierHeld() {
    if (g_settings.needCtrl  && !(GetAsyncKeyState(VK_CONTROL) & 0x8000)) return false;
    if (g_settings.needAlt   && !(GetAsyncKeyState(VK_MENU)    & 0x8000)) return false;
    if (g_settings.needShift && !(GetAsyncKeyState(VK_SHIFT)   & 0x8000)) return false;
    return true;
}

static bool IsSystemWindow(HWND hwnd) {
    WCHAR cls[256] = {};
    GetClassNameW(hwnd, cls, ARRAYSIZE(cls));

    return _wcsicmp(cls, L"Shell_TrayWnd") == 0 ||
           _wcsicmp(cls, L"Progman") == 0 ||
           _wcsicmp(cls, L"WorkerW") == 0 ||
           _wcsicmp(cls, L"DV2ControlHost") == 0 ||
           _wcsicmp(cls, L"Windows.UI.Core.CoreWindow") == 0;
}

////////////////////////////////////////////////////////////////////////////////
// Excluded programs and elevated windows
//
// Process facts are cached for a few seconds, so the mouse hook can decide
// cheaply whether a window is excluded. With an empty exclusion list (the
// default) the hook never looks at the process at all.

struct ProcInfo {
    bool      excluded = false;
    bool      elevated = false;  // Runs above our integrity level (UIPI).
    ULONGLONG tick     = 0;
};

const ULONGLONG kProcCacheTtl = 5000;

SRWLOCK g_procLock = SRWLOCK_INIT;
std::vector<std::wstring> g_excludedPrograms;     // lowercase, g_procLock
std::unordered_map<DWORD, ProcInfo> g_procCache;  // g_procLock
unsigned g_procGeneration = 0;                    // g_procLock

static void SetExcludedPrograms(std::vector<std::wstring> list) {
    AcquireSRWLockExclusive(&g_procLock);
    g_excludedPrograms = std::move(list);
    g_procCache.clear();
    g_procGeneration++;
    ReleaseSRWLockExclusive(&g_procLock);
}

static DWORD IntegrityLevelOfToken(HANDLE token) {
    DWORD size = 0;
    GetTokenInformation(token, TokenIntegrityLevel, nullptr, 0, &size);
    if (!size) {
        return 0;
    }

    std::vector<BYTE> buf(size);
    if (!GetTokenInformation(token, TokenIntegrityLevel, buf.data(), size, &size)) {
        return 0;
    }

    PSID sid = ((TOKEN_MANDATORY_LABEL*)buf.data())->Label.Sid;
    UCHAR count = *GetSidSubAuthorityCount(sid);
    return count ? *GetSidSubAuthority(sid, count - 1) : 0;
}

static DWORD OwnIntegrityLevel() {
    static DWORD level = [] {
        DWORD l = 0;
        HANDLE token;
        if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &token)) {
            l = IntegrityLevelOfToken(token);
            CloseHandle(token);
        }
        return l;
    }();
    return level;
}

// Called with g_procLock held (shared).
static ProcInfo QueryProcInfo(DWORD pid) {
    ProcInfo info;
    HANDLE process = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!process) {
        info.elevated = true;  // Can't even query it, so can't change it.
        return info;
    }

    if (!g_excludedPrograms.empty()) {
        WCHAR path[MAX_PATH];
        DWORD size = ARRAYSIZE(path);
        if (QueryFullProcessImageNameW(process, 0, path, &size)) {
            PCWSTR name = wcsrchr(path, L'\\');
            std::wstring lower = name ? name + 1 : path;
            for (auto& c : lower) c = towlower(c);

            for (const auto& e : g_excludedPrograms) {
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

static ProcInfo GetProcInfo(DWORD pid) {
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

static bool IsWindowExcluded(HWND hwnd) {
    AcquireSRWLockShared(&g_procLock);
    bool noList = g_excludedPrograms.empty();
    ReleaseSRWLockShared(&g_procLock);
    if (noList) {
        return false;
    }

    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    return pid && pid != GetCurrentProcessId() && GetProcInfo(pid).excluded;
}

static bool IsWindowElevated(HWND hwnd) {
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    if (!pid || pid == GetCurrentProcessId()) {
        return false;
    }
    return GetProcInfo(pid).elevated;
}

////////////////////////////////////////////////////////////////////////////////
// Windows owned by this mod (title bar button, indicator)

HWND g_ctlWnd = nullptr;
HWND g_btnWnd = nullptr;
HWND g_tipWnd = nullptr;

const wchar_t kControlClass[]   = L"WindhawkScrollOpacityCtl_" WH_MOD_ID;
const wchar_t kButtonClass[]    = L"WindhawkScrollOpacityBtn_" WH_MOD_ID;
const wchar_t kIndicatorClass[] = L"WindhawkScrollOpacityTip_" WH_MOD_ID;

// The button stores the window it belongs to in GWLP_USERDATA, so scrolling
// over it changes that window. Other windows of this process are left alone.
static HWND MapOwnWindow(HWND root) {
    DWORD pid = 0;
    GetWindowThreadProcessId(root, &pid);
    if (pid != GetCurrentProcessId()) {
        return root;
    }

    if (root == g_btnWnd) {
        return (HWND)GetWindowLongPtrW(root, GWLP_USERDATA);
    }

    return nullptr;
}

static HWND GetRootWindowAt(POINT pt) {
    HWND hwnd = WindowFromPoint(pt);
    if (!hwnd) {
        return nullptr;
    }

    HWND root = GetAncestor(hwnd, GA_ROOT);
    return MapOwnWindow(root ? root : hwnd);
}

static bool AdjustOpacity(HWND hwnd, int delta) {
    if (!hwnd || !IsWindow(hwnd) || IsSystemWindow(hwnd) || !IsWindowVisible(hwnd) ||
        IsWindowExcluded(hwnd)) {
        return false;
    }

    std::lock_guard<std::mutex> lock(g_mutex);

    LONG_PTR exStyle = GetWindowLongPtr(hwnd, GWL_EXSTYLE);

    auto it = g_states.find(hwnd);
    if (it == g_states.end()) {
        bool hadLayered = (exStyle & WS_EX_LAYERED) != 0;
        int curPct = 100;

        if (hadLayered) {
            BYTE alpha = 255;
            DWORD flags = 0;
            if (GetLayeredWindowAttributes(hwnd, nullptr, &alpha, &flags) &&
                (flags & LWA_ALPHA)) {
                curPct = (int)((alpha * 100) / 255);
                if (curPct < 1) curPct = 1;
                if (curPct > 100) curPct = 100;
            }
        }

        g_states[hwnd] = {curPct, hadLayered};
        it = g_states.find(hwnd);
    }

    int oldPct = it->second.pct;
    int newPct = oldPct + delta;

    if (newPct < g_settings.minPct) newPct = g_settings.minPct;
    if (newPct > 100) newPct = 100;

    if (newPct == oldPct) {
        return false;
    }

    if (newPct >= 100 && !it->second.hadLayered) {
        SetLastError(ERROR_SUCCESS);
        LONG_PTR result = SetWindowLongPtr(hwnd, GWL_EXSTYLE, exStyle & ~WS_EX_LAYERED);
        if (result == 0 && GetLastError() != ERROR_SUCCESS) {
            Wh_Log(L"SetWindowLongPtr(remove layered) failed for 0x%p: %u",
                   (void*)hwnd, GetLastError());
            return false;
        }

        g_states.erase(it);
        Wh_Log(L"Restored window 0x%p to full opacity", (void*)hwnd);
        return true;
    }

    BYTE alpha = (BYTE)((newPct * 255) / 100);

    if (!(exStyle & WS_EX_LAYERED)) {
        SetLastError(ERROR_SUCCESS);
        LONG_PTR result = SetWindowLongPtr(hwnd, GWL_EXSTYLE, exStyle | WS_EX_LAYERED);
        if (result == 0 && GetLastError() != ERROR_SUCCESS) {
            Wh_Log(L"SetWindowLongPtr(add layered) failed for 0x%p: %u",
                   (void*)hwnd, GetLastError());
            return false;
        }
    }

    if (!SetLayeredWindowAttributes(hwnd, 0, alpha, LWA_ALPHA)) {
        Wh_Log(L"SetLayeredWindowAttributes failed for 0x%p: %u",
               (void*)hwnd, GetLastError());
        return false;
    }

    it->second.pct = newPct;
    Wh_Log(L"Window 0x%p opacity -> %d%%", (void*)hwnd, newPct);
    return true;
}

static void RestoreAllWindows() {
    std::lock_guard<std::mutex> lock(g_mutex);

    for (auto& [hwnd, state] : g_states) {
        if (!IsWindow(hwnd)) continue;

        LONG_PTR exStyle = GetWindowLongPtr(hwnd, GWL_EXSTYLE);

        if (state.hadLayered) {
            if (!SetLayeredWindowAttributes(hwnd, 0, 255, LWA_ALPHA)) {
                Wh_Log(L"Restore alpha failed for 0x%p: %u",
                       (void*)hwnd, GetLastError());
            }
        } else {
            SetLastError(ERROR_SUCCESS);
            LONG_PTR result = SetWindowLongPtr(hwnd, GWL_EXSTYLE, exStyle & ~WS_EX_LAYERED);
            if (result == 0 && GetLastError() != ERROR_SUCCESS) {
                Wh_Log(L"Restore style failed for 0x%p: %u",
                       (void*)hwnd, GetLastError());
            }
        }
    }

    g_states.clear();
}

////////////////////////////////////////////////////////////////////////////////
// Opacity helpers used by the optional extras

// Current opacity in percent, as AdjustOpacity sees it.
static int GetOpacityPct(HWND hwnd) {
    std::lock_guard<std::mutex> lock(g_mutex);

    auto it = g_states.find(hwnd);
    if (it != g_states.end()) {
        return it->second.pct;
    }

    int pct = 100;
    if (GetWindowLongPtr(hwnd, GWL_EXSTYLE) & WS_EX_LAYERED) {
        BYTE alpha = 255;
        DWORD flags = 0;
        if (GetLayeredWindowAttributes(hwnd, nullptr, &alpha, &flags) &&
            (flags & LWA_ALPHA)) {
            pct = (int)((alpha * 100) / 255);
            if (pct < 1) pct = 1;
        }
    }
    return pct;
}

static bool SetOpacityPct(HWND hwnd, int pct) {
    int cur = GetOpacityPct(hwnd);
    if (pct == cur) {
        return true;
    }
    return AdjustOpacity(hwnd, pct - cur);
}

static int DpiOf(HWND hwnd) {
    UINT dpi = hwnd ? GetDpiForWindow(hwnd) : 0;
    return dpi ? (int)dpi : 96;
}

static int Scale(int v, int dpi) {
    return MulDiv(v, dpi, 96);
}

static bool IsCloaked(HWND hwnd) {
    DWORD cloaked = 0;
    return SUCCEEDED(DwmGetWindowAttribute(hwnd, DWMWA_CLOAKED, &cloaked,
                                           sizeof(cloaked))) && cloaked;
}

// Messages for the control window (the extras' UI thread).
enum : UINT {
    WM_APP_SETTINGS = WM_APP + 1,
    WM_APP_QUIT,
    WM_APP_SHOW_PCT,  // wParam = HWND, lParam = percent
};

const UINT_PTR kIndicatorTimer         = 1;
const UINT_PTR kForegroundRecheckTimer = 2;

////////////////////////////////////////////////////////////////////////////////
// Percentage indicator (option "showIndicator")

HFONT        g_tipFont    = nullptr;
int          g_tipFontDpi = 0;
std::wstring g_tipText;

static void ShowIndicator(const std::wstring& text, POINT anchor, int dpi) {
    if (!g_settings.showIndicator || !g_tipWnd) {
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
    if (GetMonitorInfoW(MonitorFromPoint(anchor, MONITOR_DEFAULTTONEAREST), &mi)) {
        x = std::clamp(x, (int)mi.rcWork.left, (int)mi.rcWork.right - w);
        y = std::clamp(y, (int)mi.rcWork.top, (int)mi.rcWork.bottom - h);
    }

    SetWindowPos(g_tipWnd, HWND_TOPMOST, x, y, w, h, SWP_NOACTIVATE | SWP_SHOWWINDOW);
    InvalidateRect(g_tipWnd, nullptr, TRUE);
    SetTimer(g_tipWnd, kIndicatorTimer, 800, nullptr);
}

static POINT CursorAnchor(int dpi) {
    POINT pt{};
    GetCursorPos(&pt);
    return POINT{pt.x + Scale(16, dpi), pt.y + Scale(16, dpi)};
}

static std::wstring PctText(int pct) {
    return std::to_wstring(pct) + L"%";
}

static LRESULT CALLBACK IndicatorWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
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
// Title bar button (option "titlebarButton")
//
// Windows can't host a real button inside another program's title bar without
// injecting into every program, so the button is a tiny always-on-top window
// of this process that never takes focus. It's placed next to the active
// window's real caption buttons, found by hit-testing the title bar
// (WM_NCHITTEST), so it also lines up in apps that draw their own title bar
// (Chrome, Edge, VS Code, Electron apps). It's fully event driven, there is no
// polling timer.

const int kButtonWidth    = 30;
const int kButtonHeight   = 22;
const int kButtonFontSize = 11;
const COLORREF kButtonColor     = RGB(0x2B, 0x2B, 0x2B);
const COLORREF kButtonTextColor = RGB(0xFF, 0xFF, 0xFF);

HWINEVENTHOOK g_fgHook             = nullptr;
HWINEVENTHOOK g_minHook            = nullptr;
HWINEVENTHOOK g_focusHook          = nullptr;
HWINEVENTHOOK g_captureHook        = nullptr;
HWINEVENTHOOK g_targetLocHook      = nullptr;
HWINEVENTHOOK g_targetShowHideHook = nullptr;
HWINEVENTHOOK g_targetCloakHook    = nullptr;
HWINEVENTHOOK g_targetMoveSizeHook = nullptr;
DWORD g_targetHookThread = 0;
bool  g_inMoveSize       = false;

HWND  g_target         = nullptr;
bool  g_targetElevated = false;
RECT  g_btnRect{};
bool  g_btnVisible     = false;
bool  g_btnHover       = false;
HFONT g_btnFont        = nullptr;
int   g_btnFontDpi     = 0;

// Whether the button should be shown for this window.
static bool IsEligibleForButton(HWND hwnd) {
    if (!hwnd || !IsWindow(hwnd) || !IsWindowVisible(hwnd) || IsIconic(hwnd)) {
        return false;
    }

    LONG_PTR style = GetWindowLongPtrW(hwnd, GWL_STYLE);
    if (!(style & WS_MINIMIZEBOX) || (style & WS_MINIMIZE)) {
        return false;
    }

    if (IsSystemWindow(hwnd) || IsCloaked(hwnd)) {
        return false;
    }

    if ((style & WS_CAPTION) != WS_CAPTION) {
        // No caption: skip it if it fills its monitor (fullscreen).
        RECT rc;
        MONITORINFO mi{};
        mi.cbSize = sizeof(mi);
        HMONITOR mon = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
        if (GetWindowRect(hwnd, &rc) && GetMonitorInfoW(mon, &mi) &&
            rc.left <= mi.rcMonitor.left && rc.top <= mi.rcMonitor.top &&
            rc.right >= mi.rcMonitor.right && rc.bottom >= mi.rcMonitor.bottom) {
            return false;
        }
    }

    return true;
}

static void HideButton() {
    if (g_btnVisible) {
        ShowWindow(g_btnWnd, SW_HIDE);
        g_btnVisible = false;
        g_btnHover = false;
    }
    SetRectEmpty(&g_btnRect);
}

// Caption button position found by hit-testing, cached per window and size so
// moving a window doesn't probe again.
struct CaptionCache {
    HWND hwnd          = nullptr;
    int  width         = 0;
    int  height        = 0;
    int  dpi           = 0;
    bool found         = false;
    int  fromRight     = 0;  // frame's right edge to the leftmost caption button
    int  top           = 0;  // caption buttons' top, relative to the frame
    int  buttonsHeight = 0;
} g_captionCache;

static void ResetCaptionCache() {
    g_captionCache = CaptionCache{};
}

static bool IsCaptionButtonHit(LRESULT r) {
    return r == HTMINBUTTON || r == HTMAXBUTTON || r == HTCLOSE || r == HTHELP;
}

static bool NcHitTest(HWND hwnd, int x, int y, LRESULT* result) {
    DWORD_PTR res = 0;
    // SMTO_BLOCK so the probe can't be re-entered by WinEvent callbacks. The
    // mouse hook runs on its own thread, so this never delays input.
    if (!SendMessageTimeoutW(hwnd, WM_NCHITTEST, 0, MAKELPARAM((short)x, (short)y),
                             SMTO_ABORTIFHUNG | SMTO_BLOCK, 50, &res)) {
        return false;
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
                if (gap > Scale(12, dpi)) break;
            } else if (dx > Scale(80, dpi)) {
                break;
            }
        }

        if (!any) {
            continue;
        }

        // Exact left edge.
        for (int x = leftmost - 1; x > leftmost - step; x--) {
            LRESULT r;
            if (!NcHitTest(hwnd, x, y, &r) || !IsCaptionButtonHit(r)) break;
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
            if (!NcHitTest(hwnd, xv, f.top + yy, &r) || !IsCaptionButtonHit(r)) break;
            t = yy;
        }
        int vstep = std::max(1, Scale(2, dpi));
        for (int yy = yo + vstep; yy < Scale(80, dpi); yy += vstep) {
            LRESULT r;
            if (!NcHitTest(hwnd, xv, f.top + yy, &r) || !IsCaptionButtonHit(r)) break;
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
    bool stale = c.hwnd != hwnd || c.width != w || c.height != h || c.dpi != dpi;

    // While the window is being resized, keep the offsets found before (they
    // are relative to the frame's top right corner). It's probed again when
    // the resize ends.
    if (stale && g_inMoveSize && c.hwnd == hwnd && c.dpi == dpi) {
        stale = false;
    }

    if (stale) {
        int pFromRight = 0, pTop = 0, pHeight = 0;
        bool found = ProbeCaptionButtons(hwnd, f, dpi, &pFromRight, &pTop, &pHeight);
        c = CaptionCache{hwnd, w, h, dpi, found, pFromRight, pTop, pHeight};
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
    if (!g_settings.titlebarButton || g_targetElevated || !IsEligibleForButton(h) ||
        IsWindowExcluded(h)) {
        HideButton();
        return;
    }

    RECT wr;
    if (!GetWindowRect(h, &wr)) {
        HideButton();
        return;
    }

    int dpi = DpiOf(h);
    int bw = Scale(kButtonWidth, dpi);
    int bh = Scale(kButtonHeight, dpi);
    int capLeft = 0;
    int y = 0;
    bool haveCaption = false;

    // Visible frame (without the invisible resize borders).
    RECT frame;
    if (FAILED(DwmGetWindowAttribute(h, DWMWA_EXTENDED_FRAME_BOUNDS, &frame, sizeof(frame)))) {
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
        SUCCEEDED(DwmGetWindowAttribute(h, DWMWA_CAPTION_BUTTON_BOUNDS, &cb, sizeof(cb)))) {
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

    int x = capLeft - bw - Scale(g_settings.buttonOffset, dpi);

    // Never above the visible top of the window or off the monitor.
    y = std::max(y, (int)frame.top);
    MONITORINFO mi{};
    mi.cbSize = sizeof(mi);
    if (GetMonitorInfoW(MonitorFromWindow(h, MONITOR_DEFAULTTONEAREST), &mi)) {
        y = std::max(y, (int)mi.rcMonitor.top);
        x = std::clamp(x, (int)mi.rcMonitor.left,
                       std::max((int)mi.rcMonitor.left, (int)mi.rcMonitor.right - bw));
    }

    if (!g_btnFont || g_btnFontDpi != dpi) {
        if (g_btnFont) {
            DeleteObject(g_btnFont);
        }
        g_btnFont = CreateFontW(-MulDiv(kButtonFontSize, dpi, 72), 0, 0, 0, FW_NORMAL,
                                FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
                                CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH,
                                L"Segoe UI Symbol");
        g_btnFontDpi = dpi;
        InvalidateRect(g_btnWnd, nullptr, TRUE);
    }

    SetWindowLongPtrW(g_btnWnd, GWLP_USERDATA, (LONG_PTR)h);

    RECT newRect{x, y, x + bw, y + bh};
    if (!g_btnVisible || !EqualRect(&newRect, &g_btnRect)) {
        SetWindowPos(g_btnWnd, HWND_TOPMOST, x, y, bw, bh, SWP_NOACTIVATE | SWP_SHOWWINDOW);
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

// Left-click: one button step more transparent, landing on the minimum once,
// then back to 100%.
static void ButtonCycle() {
    HWND h = g_target;
    if (!h || !IsWindow(h)) {
        return;
    }

    int cur = GetOpacityPct(h);
    int step = g_settings.buttonStep;
    int next = 100 - ((100 - cur) / step + 1) * step;
    if (next < g_settings.minPct) {
        next = cur > g_settings.minPct ? g_settings.minPct : 100;
    }

    bool ok = SetOpacityPct(h, next);
    ShowIndicator(ok ? PctText(next) : std::wstring(L"Can't change this window"),
                  ButtonIndicatorAnchor(), DpiOf(h));
}

// Right-click: back to 100%.
static void ButtonReset() {
    HWND h = g_target;
    if (!h || !IsWindow(h)) {
        return;
    }

    bool ok = SetOpacityPct(h, 100);
    ShowIndicator(ok ? PctText(100) : std::wstring(L"Can't change this window"),
                  ButtonIndicatorAnchor(), DpiOf(h));
}

static COLORREF Lighten(COLORREF c, int amount) {
    auto mix = [amount](int v) { return v + (255 - v) * amount / 100; };
    return RGB(mix(GetRValue(c)), mix(GetGValue(c)), mix(GetBValue(c)));
}

static LRESULT CALLBACK ButtonWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
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
        case WM_LBUTTONUP:
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
            HBRUSH bg = CreateSolidBrush(g_btnHover ? Lighten(kButtonColor, 18) : kButtonColor);
            FillRect(dc, &rc, bg);
            DeleteObject(bg);
            SetBkMode(dc, TRANSPARENT);
            SetTextColor(dc, kButtonTextColor);
            HGDIOBJ old = g_btnFont ? SelectObject(dc, g_btnFont) : nullptr;
            DrawTextW(dc, L"\u25D0", 1, &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
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
// Window events for the button (only hooked while the button is enabled)

static void CALLBACK WinEventProc(HWINEVENTHOOK hook, DWORD event, HWND hwnd,
                                  LONG idObject, LONG idChild, DWORD thread, DWORD time);

static void Unhook(HWINEVENTHOOK& hook) {
    if (hook) {
        UnhookWinEvent(hook);
        hook = nullptr;
    }
}

static void UnhookTargetEvents() {
    Unhook(g_targetLocHook);
    Unhook(g_targetShowHideHook);
    Unhook(g_targetCloakHook);
    Unhook(g_targetMoveSizeHook);
    g_targetHookThread = 0;
    g_inMoveSize = false;
}

static void SetTarget(HWND hwnd) {
    if (hwnd && (hwnd == g_btnWnd || hwnd == g_tipWnd || hwnd == g_ctlWnd)) {
        return;
    }

    DWORD pid = 0;
    DWORD tid = hwnd ? GetWindowThreadProcessId(hwnd, &pid) : 0;

    // Probe the title bar again whenever a window is activated, apps may have
    // finished drawing it by now.
    ResetCaptionCache();

    if (hwnd != g_target) {
        g_target = hwnd;
        g_inMoveSize = false;
        g_targetElevated = hwnd && IsWindowElevated(hwnd);
    }

    // Only listen to move/resize/hide events of the active window's thread,
    // so we don't get flooded by events from every program.
    if (tid != g_targetHookThread) {
        UnhookTargetEvents();
        if (tid) {
            g_targetLocHook = SetWinEventHook(EVENT_OBJECT_LOCATIONCHANGE, EVENT_OBJECT_LOCATIONCHANGE,
                                              nullptr, WinEventProc, pid, tid, WINEVENT_OUTOFCONTEXT);
            g_targetShowHideHook = SetWinEventHook(EVENT_OBJECT_DESTROY, EVENT_OBJECT_HIDE,
                                                   nullptr, WinEventProc, pid, tid, WINEVENT_OUTOFCONTEXT);
            // Windows 11 apps often become active while still cloaked.
            g_targetCloakHook = SetWinEventHook(EVENT_OBJECT_CLOAKED, EVENT_OBJECT_UNCLOAKED,
                                                nullptr, WinEventProc, pid, tid, WINEVENT_OUTOFCONTEXT);
            g_targetMoveSizeHook = SetWinEventHook(EVENT_SYSTEM_MOVESIZESTART, EVENT_SYSTEM_MOVESIZEEND,
                                                   nullptr, WinEventProc, pid, tid, WINEVENT_OUTOFCONTEXT);
            g_targetHookThread = tid;
        }
    }

    UpdateButton();
}

// Popups (menus etc.) of an app: keep the button on the app's main window.
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
                                  LONG idObject, LONG idChild, DWORD thread, DWORD time) {
    bool self = idObject == OBJID_WINDOW && idChild == CHILDID_SELF;

    switch (event) {
        case EVENT_SYSTEM_FOREGROUND:
            SetTarget(ResolveTarget(hwnd));
            // With focus stealing prevention Windows sometimes reports a
            // window that didn't end up in front. Check again a moment later.
            SetTimer(g_ctlWnd, kForegroundRecheckTimer, 300, nullptr);
            break;

        case EVENT_SYSTEM_CAPTURESTART:
        case EVENT_OBJECT_FOCUS:
            // EVENT_SYSTEM_FOREGROUND isn't always sent, focus changes and
            // mouse clicks catch the rest.
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
                UpdateButton();
            }
            break;

        case EVENT_OBJECT_LOCATIONCHANGE:
        case EVENT_OBJECT_SHOW:
        case EVENT_OBJECT_CLOAKED:
        case EVENT_OBJECT_UNCLOAKED:
            if (hwnd == g_target && self) {
                UpdateButton();
            }
            break;

        case EVENT_OBJECT_HIDE:
            if (hwnd == g_target && self) {
                HWND fg = GetForegroundWindow();
                if (fg && fg != hwnd) {
                    SetTarget(fg);
                } else {
                    UpdateButton();
                }
            }
            break;

        case EVENT_OBJECT_DESTROY:
            if (hwnd == g_target && self) {
                g_target = nullptr;
                HideButton();
                UnhookTargetEvents();
                HWND fg = GetForegroundWindow();
                if (fg && fg != hwnd) {
                    SetTarget(fg);
                }
            }
            break;
    }
}

static void UpdateButtonHooks() {
    if (!g_settings.titlebarButton) {
        Unhook(g_fgHook);
        Unhook(g_minHook);
        Unhook(g_focusHook);
        Unhook(g_captureHook);
        UnhookTargetEvents();
        g_target = nullptr;
        HideButton();
        return;
    }

    const DWORD flags = WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS;
    if (!g_fgHook) {
        g_fgHook = SetWinEventHook(EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND,
                                   nullptr, WinEventProc, 0, 0, flags);
    }
    if (!g_minHook) {
        g_minHook = SetWinEventHook(EVENT_SYSTEM_MINIMIZESTART, EVENT_SYSTEM_MINIMIZEEND,
                                    nullptr, WinEventProc, 0, 0, flags);
    }
    if (!g_focusHook) {
        g_focusHook = SetWinEventHook(EVENT_OBJECT_FOCUS, EVENT_OBJECT_FOCUS,
                                      nullptr, WinEventProc, 0, 0, flags);
    }
    if (!g_captureHook) {
        g_captureHook = SetWinEventHook(EVENT_SYSTEM_CAPTURESTART, EVENT_SYSTEM_CAPTURESTART,
                                        nullptr, WinEventProc, 0, 0, flags);
    }

    // Re-evaluate the current window (exclusions may have changed).
    HWND target = g_target;
    g_target = nullptr;
    UnhookTargetEvents();
    SetTarget(target ? target : ResolveTarget(GetForegroundWindow()));
    ForceButtonRefresh();
}

////////////////////////////////////////////////////////////////////////////////
// UI thread for the extras (button and indicator)

HANDLE g_uiThread     = nullptr;
HANDLE g_uiReadyEvent = nullptr;
bool   g_uiInitOk     = false;

static void UiShutdown() {
    Unhook(g_fgHook);
    Unhook(g_minHook);
    Unhook(g_focusHook);
    Unhook(g_captureHook);
    UnhookTargetEvents();

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

static LRESULT CALLBACK ControlWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_APP_SETTINGS:
            UpdateButtonHooks();
            return 0;
        case WM_APP_SHOW_PCT: {
            HWND target = (HWND)wParam;
            int dpi = DpiOf(target);
            ShowIndicator(PctText((int)lParam), CursorAnchor(dpi), dpi);
            return 0;
        }
        case WM_TIMER:
            if (wParam == kForegroundRecheckTimer) {
                KillTimer(hwnd, kForegroundRecheckTimer);
                if (g_settings.titlebarButton) {
                    RecheckForeground();
                }
                return 0;
            }
            break;
        case WM_DISPLAYCHANGE:
        case WM_SETTINGCHANGE:
            if (g_settings.titlebarButton) {
                ForceButtonRefresh();
            }
            return 0;
        case WM_APP_QUIT:
            UiShutdown();
            DestroyWindow(hwnd);
            return 0;
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

static bool CreateUiWindows() {
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

    // A top-level (not message-only) control window, so it gets
    // WM_DISPLAYCHANGE / WM_SETTINGCHANGE broadcasts. It's never shown.
    HWND ctl = CreateWindowExW(WS_EX_TOOLWINDOW, kControlClass, L"", WS_POPUP,
                               0, 0, 0, 0, nullptr, nullptr, inst, nullptr);
    if (!ctl) {
        return false;
    }

    DWORD noActivate = WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE;
    g_btnWnd = CreateWindowExW(noActivate, kButtonClass, L"Opacity", WS_POPUP,
                               0, 0, 0, 0, nullptr, nullptr, inst, nullptr);
    g_tipWnd = CreateWindowExW(noActivate | WS_EX_LAYERED | WS_EX_TRANSPARENT,
                               kIndicatorClass, L"", WS_POPUP, 0, 0, 0, 0,
                               nullptr, nullptr, inst, nullptr);
    if (!g_btnWnd || !g_tipWnd) {
        DestroyWindow(ctl);
        return false;
    }

    SetLayeredWindowAttributes(g_tipWnd, 0, 235, LWA_ALPHA);

    BOOL disable = TRUE;
    DWM_WINDOW_CORNER_PREFERENCE corner = DWMWCP_ROUNDSMALL;
    for (HWND w : {g_btnWnd, g_tipWnd}) {
        DwmSetWindowAttribute(w, DWMWA_TRANSITIONS_FORCEDISABLED, &disable, sizeof(disable));
        DwmSetWindowAttribute(w, DWMWA_WINDOW_CORNER_PREFERENCE, &corner, sizeof(corner));
    }

    g_ctlWnd = ctl;
    return true;
}

DWORD WINAPI UiThreadProc(LPVOID) {
    g_uiInitOk = CreateUiWindows();
    if (g_uiInitOk) {
        UpdateButtonHooks();
    } else {
        Wh_Log(L"Failed to create the button windows: %u", GetLastError());
        UiShutdown();
    }

    SetEvent(g_uiReadyEvent);
    if (!g_uiInitOk) {
        return 1;
    }

    MSG msg;
    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    return 0;
}

static void StartUiThread() {
    g_uiReadyEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_uiThread = CreateThread(nullptr, 0, UiThreadProc, nullptr, 0, nullptr);
    if (!g_uiThread) {
        Wh_Log(L"CreateThread(UI) failed: %u", GetLastError());
    } else {
        WaitForSingleObject(g_uiReadyEvent, 5000);
    }
    CloseHandle(g_uiReadyEvent);
    g_uiReadyEvent = nullptr;
}

static void StopUiThread() {
    if (!g_uiThread) {
        return;
    }

    if (g_ctlWnd) {
        PostMessageW(g_ctlWnd, WM_APP_QUIT, 0, 0);
    }
    WaitForSingleObject(g_uiThread, 5000);
    CloseHandle(g_uiThread);
    g_uiThread = nullptr;
    g_ctlWnd = nullptr;
}

// Called from the mouse hook thread after a modifier + wheel or middle-click
// change. The indicator is drawn on the UI thread, so input never waits.
static void NotifyOpacityChanged(HWND hwnd) {
    if (g_settings.showIndicator && g_ctlWnd) {
        PostMessageW(g_ctlWnd, WM_APP_SHOW_PCT, (WPARAM)hwnd, GetOpacityPct(hwnd));
    }
}

HHOOK  g_mouseHook      = nullptr;
HANDLE g_hookThread     = nullptr;
DWORD  g_hookThreadId   = 0;
int    g_wheelRemainder = 0;
bool   g_swallowMiddleUp = false;

LRESULT CALLBACK LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION && wParam == WM_MOUSEWHEEL && IsModifierHeld()) {
        auto* ms = reinterpret_cast<MSLLHOOKSTRUCT*>(lParam);

        g_wheelRemainder += GET_WHEEL_DELTA_WPARAM(ms->mouseData);

        int notches = g_wheelRemainder / WHEEL_DELTA;
        g_wheelRemainder %= WHEEL_DELTA;

        if (notches != 0) {
            int delta = (notches > 0 ? -1 : 1) * g_settings.step * std::abs(notches);

            HWND target = GetRootWindowAt(ms->pt);
            if (AdjustOpacity(target, delta)) {
                NotifyOpacityChanged(target);
                return 1;
            }
        }
    }

    // Optional: modifier + middle-click resets the window to 100%.
    if (nCode == HC_ACTION && wParam == WM_MBUTTONUP && g_swallowMiddleUp) {
        g_swallowMiddleUp = false;
        return 1;
    }

    if (nCode == HC_ACTION && wParam == WM_MBUTTONDOWN && g_settings.middleClickReset &&
        IsModifierHeld()) {
        auto* ms = reinterpret_cast<MSLLHOOKSTRUCT*>(lParam);

        HWND target = GetRootWindowAt(ms->pt);
        if (AdjustOpacity(target, 100)) {
            NotifyOpacityChanged(target);
            g_swallowMiddleUp = true;
            return 1;
        }
    }

    return CallNextHookEx(g_mouseHook, nCode, wParam, lParam);
}

DWORD WINAPI HookThreadProc(LPVOID) {
    MSG msg;
    PeekMessageW(&msg, nullptr, WM_USER, WM_USER, PM_NOREMOVE);

    g_mouseHook = SetWindowsHookExW(WH_MOUSE_LL, LowLevelMouseProc, nullptr, 0);
    if (!g_mouseHook) {
        Wh_Log(L"SetWindowsHookEx failed: %u", GetLastError());
        return 1;
    }

    Wh_Log(L"Low-level mouse hook installed");

    while (GetMessageW(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    UnhookWindowsHookEx(g_mouseHook);
    g_mouseHook = nullptr;
    Wh_Log(L"Low-level mouse hook removed");
    return 0;
}

BOOL WhTool_ModInit() {
    Wh_Log(L"Scroll Window Opacity: init");
    LoadSettings();

    // Title bar button and percentage indicator. If this fails, the extras
    // are just not available and the wheel still works.
    StartUiThread();

    g_hookThread = CreateThread(nullptr, 0, HookThreadProc, nullptr, 0, &g_hookThreadId);
    if (!g_hookThread) {
        Wh_Log(L"CreateThread failed: %u", GetLastError());
        return FALSE;
    }

    return TRUE;
}

void WhTool_ModSettingsChanged() {
    Wh_Log(L"Scroll Window Opacity: settings changed");
    LoadSettings();

    if (g_ctlWnd) {
        PostMessageW(g_ctlWnd, WM_APP_SETTINGS, 0, 0);
    }
}

void WhTool_ModUninit() {
    Wh_Log(L"Scroll Window Opacity: uninit");

    if (g_hookThreadId) {
        PostThreadMessageW(g_hookThreadId, WM_QUIT, 0, 0);
    }

    if (g_hookThread) {
        WaitForSingleObject(g_hookThread, 5000);
        CloseHandle(g_hookThread);
        g_hookThread = nullptr;
        g_hookThreadId = 0;
    }

    StopUiThread();

    RestoreAllWindows();
}

bool g_isToolModProcessLauncher;
HANDLE g_toolModProcessMutex;

void WINAPI EntryPoint_Hook() {
    Wh_Log(L">");
    ExitThread(0);
}

BOOL Wh_ModInit() {
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

    WCHAR commandLine[MAX_PATH + 2 +
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
