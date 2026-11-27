// ==WindhawkMod==
// @id              fullscreen-in-a-window
// @name            Fullscreen in a Window
// @description     Run an app's own fullscreen inside a normal movable, resizable window. An outer frame is created and the app is told the screen is only that window's size, so its NATIVE fullscreen (or windowed) mode plays out inside the frame - the app's own window is never restyled.
// @version         1.2.1
// @author          thebdr
// @github          https://github.com/thebdr
// @homepage        https://github.com/thebdr/fullscreen-in-a-window
// @include         *
// @compilerOptions -lcomctl32 -lgdi32 -ldwmapi
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Fullscreen in a Window

Contain an app that goes into its own fullscreen (Chrome/Edge F11, media
players, VMware, games) inside an ordinary window you can move, resize and
minimize - **without changing the app itself**. Pick a window, choose
**Enclose**, and from then on the app believes that window is its whole
screen.

![A VMware guest's fullscreen contained in a movable Windows window](https://raw.githubusercontent.com/thebdr/fullscreen-in-a-window/main/screenshot.png)

## Which apps

Only the executables in the **Applications** setting are touched (plus
`explorer.exe`, which hosts the global hotkey). Every other process refuses the
mod at load time, so it stays out of programs you never enclose. After editing
the list, **disable and re-enable the mod** (or restart the app) so the change
reaches processes that were skipped before.

## How to enclose / release

- **Global hotkey: Ctrl+Alt+Shift+Space** - encloses (or releases) the
  foreground window. Works for apps with a custom title bar (Chrome/Edge).
- Or **right-click a standard title bar** (or press Alt+Space) and choose
  **"Enclose window"** / **"Release (un-enclose)"**.
- The container frame has its own **"Release (un-enclose)"** menu item, and
  closing the frame also releases (it never closes the app).

## How it works

1. A separate **container window** is created (normal resizable frame with a
   thin dark border all around the app).
2. The app is linked to it as an *owned* window - an invisible relationship
   that keeps them together without touching the app's border or styles - and
   is positioned inside the frame.
3. While enclosed, that process is told the screen is only as big as the
   container's inside (`GetSystemMetrics`, `GetMonitorInfo`,
   `EnumDisplaySettings`, `EnumDisplayMonitors`, `GetDeviceCaps`, work area),
   and real resolution changes are refused. As a safety net, any window of that
   process that still tries to cover the real monitor - via `SetWindowPos`,
   `MoveWindow`, `DeferWindowPos` or `SetWindowPlacement`, in one call or in
   split move/size steps - is placed into the container instead, so fullscreen
   ends up inside the frame even if the app read the screen size some other way
   (e.g. DXGI).
4. Release puts the app back exactly where it was and destroys the container.
   The app's own window is never modified, so nothing can get stuck.

## Notes

- Resize the container by its edges/corners (the dark strip around the app is
  draggable), move it by its title bar. The app follows. Maximizing the app
  itself maximizes it into the container.
- The container's title bar follows the Windows light/dark app theme, live.
- **Padding** is configurable per side globally (and per app): the top strip
  keeps edge-hover UI at the app's top (like VMware's fullscreen toolbar) away
  from the title bar; sides and bottom collapse automatically while maximized
  unless set explicitly per app.
- **Auto-hide title bar** (optional): the container has no title bar at all;
  hovering the top edge shows a small **floating bar** (like Remote Desktop's
  connection bar) with a drag grip and minimize / maximize / release buttons.
  Because it's an overlay, it never moves or resizes the app - not even when
  the container is maximized. The top strip also drags the window directly,
  and right-clicking it (or the bar) opens the menu (with Release). Resize
  borders keep working.
- **Edge resistance** (optional): a virtual border - while the enclosed app is
  in the foreground, the mouse pointer stops at the hosted APP's edge like at
  a real screen edge, and only crosses out after pressing against it for a
  configurable time. Makes edge-hover UI (VMware's toolbar, scrollbars, game
  edges) feel native; the frame strip beyond the stop point becomes reachable
  after the dwell. Alt-Tab or clicking elsewhere lifts it instantly.
- **One enclosure per process** (separate apps at the same time are fine -
  they are separate processes). Enclosing a second window of the same app is
  refused until the first is released.
- While a window is enclosed, that process sees a single display the size of
  the container. Other processes are unaffected.
- The container and the app are two taskbar entries; that's normal.
- **Maximized**, the side/bottom strips collapse to zero (nothing to resize)
  and only the top-padding strip remains, so the app gets nearly the whole
  screen - and an auto-hidden Windows taskbar stays reachable (the maximize
  rect leaves it a couple of pixels on its edge).
- **Per-app parameters**: each entry in the *Per-app parameters* setting can
  override the padding on each side, enable the virtual border per side, and
  set the container size - matched by executable name. Anything left at its
  sentinel (-1 / "use global" / 0) falls back to the global settings.
- **CLI companion** (optional, not part of this mod file): a small
  `enclose.exe` can launch a program containerized even if it is not in the
  Applications list, passing any of these parameters per launch. It works by
  leaving a one-shot request file for this mod at
  `%APPDATA%\fullscreen-in-a-window\cli-request.ini`.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- targetApps:
  - chrome.exe
  - msedge.exe
  - firefox.exe
  - vmware.exe
  $name: Applications
  $description: >-
    Executable names (no path) the mod is allowed to work in. All other
    processes refuse the mod at load. After changing the list, disable and
    re-enable the mod (or restart the app).
- padLeft: 4
  $name: Padding left
  $description: >-
    Frame strip left of the app, in pixels. Collapses to 0 while maximized.
- padTop: 16
  $name: Padding top
  $description: >-
    Strip between the title bar / frame top and the app, in pixels. Hover
    zones at the app's top edge (e.g. VMware's fullscreen toolbar) live here.
    Kept while maximized.
- padRight: 4
  $name: Padding right
  $description: >-
    Frame strip right of the app, in pixels. Collapses to 0 while maximized.
- padBottom: 4
  $name: Padding bottom
  $description: >-
    Frame strip below the app, in pixels. Collapses to 0 while maximized.
- borderLeft: true
  $name: Virtual border left
  $description: Edge resistance stops the cursor at the app's left edge.
- borderTop: true
  $name: Virtual border top
  $description: Edge resistance stops the cursor at the app's top edge.
- borderRight: true
  $name: Virtual border right
  $description: Edge resistance stops the cursor at the app's right edge.
- borderBottom: true
  $name: Virtual border bottom
  $description: Edge resistance stops the cursor at the app's bottom edge.
- width: 0
  $name: Container width
  $description: >-
    Inner width in pixels for a new container. 0 = wrap the app at its current
    size.
- height: 0
  $name: Container height
  $description: >-
    Inner height in pixels for a new container. 0 = wrap the app at its current
    size.
- apps:
  - - exe: ""
      $name: Executable name
      $description: e.g. vmware.exe. Empty entries are ignored.
    - padLeft: -1
      $name: Padding left
      $description: Pixels; -1 = use the global setting.
    - padTop: -1
      $name: Padding top
      $description: Pixels; -1 = use the global setting.
    - padRight: -1
      $name: Padding right
      $description: Pixels; -1 = use the global setting.
    - padBottom: -1
      $name: Padding bottom
      $description: Pixels; -1 = use the global setting.
    - borderLeft: global
      $name: Virtual border left
      $description: Edge resistance at the app's left edge.
      $options:
      - global: Use the global setting
      - on: On
      - off: Off
    - borderTop: global
      $name: Virtual border top
      $options:
      - global: Use the global setting
      - on: On
      - off: Off
    - borderRight: global
      $name: Virtual border right
      $options:
      - global: Use the global setting
      - on: On
      - off: Off
    - borderBottom: global
      $name: Virtual border bottom
      $options:
      - global: Use the global setting
      - on: On
      - off: Off
    - width: 0
      $name: Container width
      $description: Inner width in pixels; 0 = use the global setting.
    - height: 0
      $name: Container height
      $description: Inner height in pixels; 0 = use the global setting.
  $name: Per-app parameters
  $description: >-
    Optional overrides per application, matched by executable name. Every
    field mirrors a global setting above; values left at their sentinel
    (-1 / global / 0) fall back to it. An explicit per-app padding applies in
    every window state, including maximized.
- autoHideTitle: false
  $name: Auto-hide the container title bar
  $description: >-
    No title bar; hovering the top edge shows a small floating control bar
    (Remote Desktop style) with drag grip and minimize / maximize / release.
    Never moves or resizes the app, even maximized. The top strip also drags
    the window; right-click it for the menu.
- edgeHoldMs: 300
  $name: Edge resistance (ms)
  $description: >-
    Virtual border: while the enclosed app is in the foreground, the mouse
    pointer stops at the app's edge and only crosses out (into the frame and
    beyond) after pressing against it for this many milliseconds. 0 disables.
- spoofResolution: true
  $name: Override the resolution the app reads
  $description: >-
    While enclosed, report the container's inner size as the screen size to
    that process, refuse real resolution changes, and pull monitor-covering
    windows of that process into the container. This is what makes the app's
    own fullscreen fill the container.
- globalHotkey: true
  $name: Global hotkey (Ctrl+Alt+Shift+Space)
  $description: >-
    Enclose / release the foreground window from anywhere. Hosted by the
    explorer.exe instance of the mod.
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <dwmapi.h>
#include <shellapi.h>
#include <windowsx.h>

#include <atomic>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

constexpr UINT_PTR IDM_ENCLOSE = 0xB000;  // custom system-menu command id
constexpr WCHAR kContainerClass[] = L"WH_Enclose_Container";
constexpr WCHAR kHotkeyClass[] = L"WH_Enclose_Hotkey";
constexpr int kMargin = 4;  // frame strip around the app, also the resize grab zone

// Not in every mingw-w64 dwmapi.h vintage; the values are stable.
#ifndef DWMWA_USE_IMMERSIVE_DARK_MODE
#define DWMWA_USE_IMMERSIVE_DARK_MODE 20
#endif
#ifndef DWMWA_WINDOW_CORNER_PREFERENCE
#define DWMWA_WINDOW_CORNER_PREFERENCE 33
#endif

// The floating control bar shown in auto-hide mode (RDP connection-bar style).
constexpr WCHAR kBarClass[] = L"WH_Enclose_Bar";
constexpr int kBarW = 200;
constexpr int kBarH = 30;
constexpr int kBarBtnW = 42;  // three buttons, right-aligned; the rest drags

HINSTANCE g_hInst;
UINT g_toggleMsg;  // RegisterWindowMessage: "toggle enclose on this window"
bool g_isTarget;   // this process is in the Applications list
bool g_isExplorer; // this process hosts the global hotkey

// Teardown coordination. g_teardown: no NEW subclasses/enclosures once the mod
// starts shutting down. g_hooksRemoved: Windhawk removes the inline hooks
// BEFORE Wh_ModUninit runs, so from that point the saved *_Original trampoline
// pointers are dead - call the plain imports instead (safe then: with the
// hooks gone there is no self-interception).
std::atomic<bool> g_teardown{false};
std::atomic<bool> g_hooksRemoved{false};

// Set around the mod's OWN window placements so the safety net never
// redirects them.
thread_local bool t_selfOp = false;
// Suppress the CreateWindowExW hook for our own container creation.
thread_local bool t_creatingContainer = false;

// ---------------------------------------------------------------------------
// Settings
// ---------------------------------------------------------------------------

// One parameter set, used at two levels that mirror each other: the GLOBAL
// settings hold concrete values (pads >= 0, borders 0/1), and each per-app /
// per-CLI-launch record holds overrides where sentinels (-1 pads/borders,
// 0 sizes) mean "use the global". Resolution happens at USE time so the same
// record stays valid windowed and maximized.
struct AppParams {
    int padL = -1, padT = -1, padR = -1, padB = -1;
    int borderL = -1, borderT = -1, borderR = -1, borderB = -1;
    int width = 0, height = 0;
};

std::mutex g_settingsMutex;
std::vector<std::wstring> g_targetApps;
std::vector<std::pair<std::wstring, AppParams>> g_appParams;
AppParams g_globalParams;  // concrete values; under g_settingsMutex

AppParams GlobalParams() {
    std::lock_guard<std::mutex> lock(g_settingsMutex);
    return g_globalParams;
}
std::atomic<int> g_cfgEdgeHold{300};
std::atomic<bool> g_cfgAutoHide{false};
std::atomic<bool> g_cfgSpoof{true};
std::atomic<bool> g_cfgHotkey{true};

void LoadSettings() {
    std::vector<std::wstring> apps;
    for (int i = 0;; i++) {
        PCWSTR value = Wh_GetStringSetting(L"targetApps[%d]", i);
        bool empty = !value || !value[0];
        if (!empty) {
            apps.push_back(value);
        }
        Wh_FreeStringSetting(value);
        if (empty) {
            break;
        }
    }
    // Per-app border dropdowns are string-valued: global / on / off.
    auto triState = [](PCWSTR fmt, int i) {
        PCWSTR v = Wh_GetStringSetting(fmt, i);
        int r = -1;
        if (v && !_wcsicmp(v, L"on")) r = 1;
        else if (v && !_wcsicmp(v, L"off")) r = 0;
        Wh_FreeStringSetting(v);
        return r;
    };

    std::vector<std::pair<std::wstring, AppParams>> perApp;
    for (int i = 0;; i++) {
        PCWSTR exe = Wh_GetStringSetting(L"apps[%d].exe", i);
        bool empty = !exe || !exe[0];
        if (!empty) {
            AppParams p;
            p.padL = Wh_GetIntSetting(L"apps[%d].padLeft", i);
            p.padT = Wh_GetIntSetting(L"apps[%d].padTop", i);
            p.padR = Wh_GetIntSetting(L"apps[%d].padRight", i);
            p.padB = Wh_GetIntSetting(L"apps[%d].padBottom", i);
            p.borderL = triState(L"apps[%d].borderLeft", i);
            p.borderT = triState(L"apps[%d].borderTop", i);
            p.borderR = triState(L"apps[%d].borderRight", i);
            p.borderB = triState(L"apps[%d].borderBottom", i);
            p.width = Wh_GetIntSetting(L"apps[%d].width", i);
            p.height = Wh_GetIntSetting(L"apps[%d].height", i);
            perApp.push_back({exe, p});
        }
        Wh_FreeStringSetting(exe);
        if (empty) {
            break;
        }
    }

    // The mirrored global section: concrete values, clamped sane.
    auto clampPad = [](int v) { return v < 0 ? 0 : (v > 200 ? 200 : v); };
    AppParams globals;
    globals.padL = clampPad(Wh_GetIntSetting(L"padLeft"));
    globals.padT = clampPad(Wh_GetIntSetting(L"padTop"));
    globals.padR = clampPad(Wh_GetIntSetting(L"padRight"));
    globals.padB = clampPad(Wh_GetIntSetting(L"padBottom"));
    globals.borderL = Wh_GetIntSetting(L"borderLeft") != 0 ? 1 : 0;
    globals.borderT = Wh_GetIntSetting(L"borderTop") != 0 ? 1 : 0;
    globals.borderR = Wh_GetIntSetting(L"borderRight") != 0 ? 1 : 0;
    globals.borderB = Wh_GetIntSetting(L"borderBottom") != 0 ? 1 : 0;
    globals.width = Wh_GetIntSetting(L"width");
    globals.height = Wh_GetIntSetting(L"height");

    {
        std::lock_guard<std::mutex> lock(g_settingsMutex);
        g_targetApps = std::move(apps);
        g_appParams = std::move(perApp);
        g_globalParams = globals;
    }
    g_cfgAutoHide = Wh_GetIntSetting(L"autoHideTitle") != 0;
    int holdMs = Wh_GetIntSetting(L"edgeHoldMs");
    g_cfgEdgeHold = holdMs < 0 ? 0 : (holdMs > 5000 ? 5000 : holdMs);
    g_cfgSpoof = Wh_GetIntSetting(L"spoofResolution") != 0;
    g_cfgHotkey = Wh_GetIntSetting(L"globalHotkey") != 0;
}

std::wstring ProcessBaseName() {
    WCHAR path[MAX_PATH];
    if (!GetModuleFileNameW(nullptr, path, ARRAYSIZE(path))) {
        return L"";
    }
    PCWSTR name = wcsrchr(path, L'\\');
    return name ? name + 1 : path;
}

bool ProcessIsTarget() {
    std::wstring self = ProcessBaseName();
    std::lock_guard<std::mutex> lock(g_settingsMutex);
    for (const auto& app : g_targetApps) {
        if (_wcsicmp(app.c_str(), self.c_str()) == 0) {
            return true;
        }
    }
    return false;
}

// The settings entry for this process's executable, or defaults.
AppParams ResolveParams() {
    std::wstring self = ProcessBaseName();
    std::lock_guard<std::mutex> lock(g_settingsMutex);
    for (const auto& [exe, p] : g_appParams) {
        if (_wcsicmp(exe.c_str(), self.c_str()) == 0) {
            return p;
        }
    }
    return AppParams{};
}

// ---------------------------------------------------------------------------
// CLI one-shot request: enclose.exe writes a flat key=value file, launches
// the target, and this reads + consumes it at mod init. A fresh matching
// request admits a process that is NOT in the Applications list and asks for
// its first suitable window to be auto-enclosed with the given parameters.
// ---------------------------------------------------------------------------

bool g_cliAdmitted = false;         // this process was let in by the CLI
AppParams g_cliParams;              // per-app params with CLI overlays applied
std::atomic<bool> g_autoEnclosePending{false};

bool ReadAndConsumeCliRequest(const std::wstring& selfExe, AppParams& inout) {
    WCHAR path[MAX_PATH];
    if (!ExpandEnvironmentStringsW(
            L"%APPDATA%\\fullscreen-in-a-window\\cli-request.ini", path,
            ARRAYSIZE(path))) {
        return false;
    }
    HANDLE f = CreateFileW(path, GENERIC_READ, FILE_SHARE_READ, nullptr,
                           OPEN_EXISTING, 0, nullptr);
    if (f == INVALID_HANDLE_VALUE) {
        return false;
    }
    char buf[2048];
    DWORD got = 0;
    BOOL ok = ReadFile(f, buf, sizeof(buf) - 1, &got, nullptr);
    CloseHandle(f);
    if (!ok || !got) {
        return false;
    }
    buf[got] = 0;

    // Parse the flat key=value lines (ASCII; exe names are matched
    // case-insensitively after a widening copy).
    std::wstring exe;
    long long stamp = 0;
    AppParams p = inout;
    char* ctx = nullptr;
    for (char* line = strtok_s(buf, "\r\n", &ctx); line;
         line = strtok_s(nullptr, "\r\n", &ctx)) {
        char* eq = strchr(line, '=');
        if (!eq) {
            continue;
        }
        *eq = 0;
        const char* key = line;
        const char* val = eq + 1;
        if (!strcmp(key, "exe")) {
            for (const char* c = val; *c; c++) {
                exe.push_back((wchar_t)(unsigned char)*c);
            }
        } else if (!strcmp(key, "stamp")) {
            stamp = _atoi64(val);
        } else if (!strcmp(key, "width")) {
            p.width = atoi(val);
        } else if (!strcmp(key, "height")) {
            p.height = atoi(val);
        } else if (!strcmp(key, "padLeft")) {
            p.padL = atoi(val);
        } else if (!strcmp(key, "padTop")) {
            p.padT = atoi(val);
        } else if (!strcmp(key, "padRight")) {
            p.padR = atoi(val);
        } else if (!strcmp(key, "padBottom")) {
            p.padB = atoi(val);
        } else if (!strcmp(key, "borderLeft")) {
            p.borderL = atoi(val) != 0;
        } else if (!strcmp(key, "borderTop")) {
            p.borderT = atoi(val) != 0;
        } else if (!strcmp(key, "borderRight")) {
            p.borderR = atoi(val) != 0;
        } else if (!strcmp(key, "borderBottom")) {
            p.borderB = atoi(val) != 0;
        }
    }

    if (_wcsicmp(exe.c_str(), selfExe.c_str()) != 0) {
        return false;
    }
    __int64 now = _time64(nullptr);
    if (stamp <= 0 || now - stamp > 60) {
        DeleteFileW(path);  // stale leftover - clean it up
        return false;
    }

    DeleteFileW(path);  // one-shot: consumed
    inout = p;
    return true;
}

// ---------------------------------------------------------------------------
// Enclosed-window state
// ---------------------------------------------------------------------------

struct Enclosed {
    HWND container;       // our outer window
    RECT savedRect;       // the app's original window rect, to restore
    bool savedZoomed;     // was the app maximized before enclosing
    LONG_PTR savedOwner;  // the app's original GWLP_HWNDPARENT (owner)
    RECT inner;           // app area inside the container (screen) = fake screen
    HMONITOR monitor;     // real monitor the container sits on
    AppParams params;     // per-app / per-launch parameter set
    bool fromCli;         // params carry CLI overrides: don't overwrite them
                          // when the settings change
};

std::mutex g_mutex;
std::unordered_map<HWND, Enclosed> g_byApp;       // app hwnd -> state
std::unordered_map<HWND, HWND> g_containerToApp;  // container hwnd -> app hwnd
HWND g_activeApp = nullptr;                       // answers process-wide queries

// Lock-free fast path for the hot query hooks: untouched processes and idle
// periods pay one atomic load, no mutex.
std::atomic<bool> g_anyEnclosed{false};

// Originals saved by the hooks; our own math goes through the Real* wrappers
// below so it neither reads our own spoofed values nor calls a dead
// trampoline after hook removal.
using GetMonitorInfoW_t = decltype(&GetMonitorInfoW);
GetMonitorInfoW_t GetMonitorInfoW_Original;
using GetMonitorInfoA_t = decltype(&GetMonitorInfoA);
GetMonitorInfoA_t GetMonitorInfoA_Original;
using GetSystemMetrics_t = decltype(&GetSystemMetrics);
GetSystemMetrics_t GetSystemMetrics_Original;
using SetWindowPos_t = decltype(&SetWindowPos);
SetWindowPos_t SetWindowPos_Original;
using GetDeviceCaps_t = decltype(&GetDeviceCaps);
GetDeviceCaps_t GetDeviceCaps_Original;

int RealGetSystemMetrics(int nIndex) {
    if (GetSystemMetrics_Original && !g_hooksRemoved.load()) {
        return GetSystemMetrics_Original(nIndex);
    }
    return GetSystemMetrics(nIndex);
}

BOOL RealGetMonitorInfoW(HMONITOR mon, LPMONITORINFO lpmi) {
    if (GetMonitorInfoW_Original && !g_hooksRemoved.load()) {
        return GetMonitorInfoW_Original(mon, lpmi);
    }
    return GetMonitorInfoW(mon, lpmi);
}

// The mod's own window placements: exempt from the net (t_selfOp) and valid
// both while hooked (trampoline) and during teardown (plain import).
BOOL SelfSetWindowPos(HWND hWnd, HWND after, int x, int y, int cx, int cy,
                      UINT flags) {
    t_selfOp = true;
    BOOL r;
    if (SetWindowPos_Original && !g_hooksRemoved.load()) {
        r = SetWindowPos_Original(hWnd, after, x, y, cx, cy, flags);
    } else {
        r = SetWindowPos(hWnd, after, x, y, cx, cy, flags);
    }
    t_selfOp = false;
    return r;
}

void SelfShowWindow(HWND hWnd, int cmd) {
    t_selfOp = true;
    ShowWindow(hWnd, cmd);
    t_selfOp = false;
}

bool SpoofActive() {
    return g_anyEnclosed.load() && g_cfgSpoof.load();
}

// The fake screen rect (the active container's inner area).
bool ActiveFakeRect(RECT& out) {
    if (!g_anyEnclosed.load()) {
        return false;
    }
    std::lock_guard<std::mutex> lock(g_mutex);
    if (g_byApp.empty()) {
        return false;
    }
    auto it = g_activeApp ? g_byApp.find(g_activeApp) : g_byApp.end();
    if (it == g_byApp.end()) {
        it = g_byApp.begin();
    }
    out = it->second.inner;
    return (out.right - out.left) > 0 && (out.bottom - out.top) > 0;
}

bool ActiveFakeSize(int& w, int& h) {
    RECT rc;
    if (!ActiveFakeRect(rc)) {
        return false;
    }
    w = rc.right - rc.left;
    h = rc.bottom - rc.top;
    return true;
}

bool ActiveFakeMonitor(HMONITOR& mon, RECT& rc) {
    if (!g_anyEnclosed.load()) {
        return false;
    }
    std::lock_guard<std::mutex> lock(g_mutex);
    if (g_byApp.empty()) {
        return false;
    }
    auto it = g_activeApp ? g_byApp.find(g_activeApp) : g_byApp.end();
    if (it == g_byApp.end()) {
        it = g_byApp.begin();
    }
    mon = it->second.monitor;
    rc = it->second.inner;
    return mon != nullptr;
}

bool EnclosedInnerForMonitor(HMONITOR mon, RECT& out) {
    if (!g_anyEnclosed.load()) {
        return false;
    }
    std::lock_guard<std::mutex> lock(g_mutex);
    for (const auto& [app, info] : g_byApp) {
        if (info.monitor == mon) {
            out = info.inner;
            return true;
        }
    }
    return false;
}

bool IsEnclosedApp(HWND hWnd) {
    if (!g_anyEnclosed.load()) {
        return false;
    }
    std::lock_guard<std::mutex> lock(g_mutex);
    return g_byApp.find(hWnd) != g_byApp.end();
}

bool OwnInnerRect(HWND hWnd, RECT& out) {
    if (!g_anyEnclosed.load()) {
        return false;
    }
    std::lock_guard<std::mutex> lock(g_mutex);
    auto it = g_byApp.find(hWnd);
    if (it == g_byApp.end()) {
        return false;
    }
    out = it->second.inner;
    return true;
}

bool IsContainer(HWND hWnd) {
    WCHAR cls[64];
    return GetClassNameW(hWnd, cls, ARRAYSIZE(cls)) &&
           _wcsicmp(cls, kContainerClass) == 0;
}

// ---------------------------------------------------------------------------
// Positioning
// ---------------------------------------------------------------------------

// Container inner (client) rect in screen coordinates.
RECT ContainerClientScreenRect(HWND container) {
    RECT rc{};
    GetClientRect(container, &rc);
    POINT tl{0, 0};
    ClientToScreen(container, &tl);
    return RECT{tl.x, tl.y, tl.x + rc.right, tl.y + rc.bottom};
}

// Parameter set of the enclosure a container belongs to (defaults if none).
AppParams ParamsForContainer(HWND container) {
    std::lock_guard<std::mutex> lock(g_mutex);
    auto it = g_containerToApp.find(container);
    if (it != g_containerToApp.end()) {
        auto ai = g_byApp.find(it->second);
        if (ai != g_byApp.end()) {
            return ai->second.params;
        }
    }
    return AppParams{};
}

// Effective per-side pads: an explicit per-app value (>= 0) wins in every
// window state; otherwise the mirrored global values apply - with sides and
// bottom collapsing to 0 while maximized (nothing to resize there), the top
// keeping its value (hover zones live in it).
void EffectivePads(const AppParams& p, bool zoomed, int& l, int& t, int& r,
                   int& b) {
    AppParams g = GlobalParams();
    l = p.padL >= 0 ? p.padL : (zoomed ? 0 : g.padL);
    t = p.padT >= 0 ? p.padT : g.padT;
    r = p.padR >= 0 ? p.padR : (zoomed ? 0 : g.padR);
    b = p.padB >= 0 ? p.padB : (zoomed ? 0 : g.padB);
}

// Effective per-side virtual border: per-app tri-state over the global.
void EffectiveBorders(const AppParams& p, bool& l, bool& t, bool& r, bool& b) {
    AppParams g = GlobalParams();
    l = (p.borderL >= 0 ? p.borderL : g.borderL) != 0;
    t = (p.borderT >= 0 ? p.borderT : g.borderT) != 0;
    r = (p.borderR >= 0 ? p.borderR : g.borderR) != 0;
    b = (p.borderB >= 0 ? p.borderB : g.borderB) != 0;
}

int EffTopPad(HWND container) {
    AppParams p = ParamsForContainer(container);
    int l, t, r, b;
    EffectivePads(p, IsZoomed(container), l, t, r, b);
    return t;
}

// The app area inside a container: the client rect inset by the effective
// per-side pads. The top pad keeps hover zones at the app's top edge (VMware's
// fullscreen toolbar) away from the title bar / frame.
RECT ContainerInnerRect(HWND container) {
    RECT rc = ContainerClientScreenRect(container);
    AppParams p = ParamsForContainer(container);
    int l, t, r, b;
    EffectivePads(p, IsZoomed(container), l, t, r, b);
    rc.left += l;
    rc.top += t;
    rc.right -= r;
    rc.bottom -= b;
    if (rc.right < rc.left + 50) rc.right = rc.left + 50;
    if (rc.bottom < rc.top + 50) rc.bottom = rc.top + 50;
    return rc;
}

// ---------------------------------------------------------------------------
// Auto-hide title bar. The reveal/hide NEVER moves or resizes the app: the
// container's outer frame grows upward for the title bar and shrinks back,
// keeping the client (and so the app and the fake screen) pinned in place.
// ---------------------------------------------------------------------------

constexpr UINT_PTR kAutoHideTimer = 1;
constexpr UINT_PTR kEdgeTimer = 2;
std::atomic<bool> g_captionShown{true};  // one container per process

HWND AppForContainer(HWND container);                  // fwd
void FitAppToContainer(HWND app, HWND container);      // fwd

// True while a caption toggle is in flight: the container's WM_SIZE/WM_MOVE
// handlers must not refit the app against the intermediate frame geometry.
std::atomic<bool> g_inCaptionToggle{false};

// Toggle the caption while keeping the CLIENT area pinned: recalc the frame on
// the unchanged outer rect, MEASURE where the client actually landed, then
// shift the outer by exactly that delta. Measured, not predicted -
// AdjustWindowRectEx does not always match the real DWM metrics, and even one
// stray pixel would ripple into the app as a resolution change. At the end the
// client (and so the fake screen and the app) is byte-identical, so no refit
// happens at all.
void SetCaptionShown(HWND container, bool show) {
    DWORD style = (DWORD)GetWindowLongPtrW(container, GWL_STYLE);
    bool cur = (style & WS_CAPTION) == WS_CAPTION;
    if (cur == show) {
        g_captionShown = show;
        return;
    }
    RECT clientBefore = ContainerClientScreenRect(container);
    RECT wr;
    if (!GetWindowRect(container, &wr)) {
        return;
    }

    g_inCaptionToggle = true;
    SetWindowLongPtrW(container, GWL_STYLE,
                      show ? (style | WS_CAPTION)
                           : (style & ~(DWORD)WS_CAPTION));
    g_captionShown = show;

    if (IsZoomed(container)) {
        // A maximized frame is pinned to the work area - just recalc it; the
        // client legitimately changes here, so refit once afterwards.
        SelfSetWindowPos(container, nullptr, 0, 0, 0, 0,
                         SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER |
                             SWP_NOACTIVATE | SWP_FRAMECHANGED);
        g_inCaptionToggle = false;
        if (HWND app = AppForContainer(container)) {
            FitAppToContainer(app, container);
        }
        return;
    }

    // Pass 1: apply the new frame metrics on the unchanged outer rect.
    SelfSetWindowPos(container, nullptr, 0, 0, 0, 0,
                     SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE |
                         SWP_FRAMECHANGED);
    // Pass 2: shift/grow the outer by however far the client actually moved,
    // returning it exactly to where it was.
    RECT clientAfter = ContainerClientScreenRect(container);
    int dL = clientAfter.left - clientBefore.left;
    int dT = clientAfter.top - clientBefore.top;
    int dR = clientAfter.right - clientBefore.right;
    int dB = clientAfter.bottom - clientBefore.bottom;
    SelfSetWindowPos(container, nullptr, wr.left - dL, wr.top - dT,
                     (wr.right - dR) - (wr.left - dL),
                     (wr.bottom - dB) - (wr.top - dT),
                     SWP_NOZORDER | SWP_NOACTIVATE);
    g_inCaptionToggle = false;
}

// The hover band that keeps a revealed title bar out: from the frame's top
// down through the padding strip.
RECT RevealBand(HWND container) {
    RECT wr{};
    GetWindowRect(container, &wr);
    RECT cl = ContainerClientScreenRect(container);
    wr.bottom = cl.top + EffTopPad(container);
    return wr;
}

bool SystemUsesLightTheme() {
    DWORD light = 1;
    DWORD size = sizeof(light);
    RegGetValueW(HKEY_CURRENT_USER,
                 L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
                 L"AppsUseLightTheme", RRF_RT_REG_DWORD, nullptr, &light, &size);
    return light != 0;
}

// Match the container's title bar to the Windows app theme (dark/light).
void ApplyContainerTheme(HWND container) {
    BOOL dark = !SystemUsesLightTheme();
    DwmSetWindowAttribute(container, DWMWA_USE_IMMERSIVE_DARK_MODE, &dark,
                          sizeof(dark));
}

// Tell every top-level window of this process that "the display" changed, so
// fullscreen surfaces re-place themselves (their placement then goes through
// the safety net and lands in the container).
void NotifyDisplayChange() {
    int w = RealGetSystemMetrics(SM_CXSCREEN);
    int h = RealGetSystemMetrics(SM_CYSCREEN);
    int fw, fh;
    if (SpoofActive() && ActiveFakeSize(fw, fh)) {
        w = fw;
        h = fh;
    }
    struct Ctx {
        int w, h;
    } ctx{w, h};
    EnumWindows(
        [](HWND hWnd, LPARAM lParam) WINAPI -> BOOL {
            DWORD pid = 0;
            GetWindowThreadProcessId(hWnd, &pid);
            if (pid == GetCurrentProcessId() && !IsContainer(hWnd)) {
                auto* c = reinterpret_cast<Ctx*>(lParam);
                PostMessageW(hWnd, WM_DISPLAYCHANGE, 32,
                             MAKELPARAM(c->w, c->h));
            }
            return TRUE;
        },
        reinterpret_cast<LPARAM>(&ctx));
}

void PositionBarIfVisible(HWND container);  // fwd

// Put the app into the container's inner area and refresh the fake screen.
// The new fake rect is published BEFORE the app is moved: the move dispatches
// WM_SIZE into the app synchronously, and whatever the app asks during that
// dispatch must already see the new geometry.
void FitAppToContainer(HWND app, HWND container) {
    if (IsIconic(container)) {
        return;  // never fit against the (-32000,-32000) iconic rect
    }
    RECT inner = ContainerInnerRect(container);
    HMONITOR mon = MonitorFromWindow(container, MONITOR_DEFAULTTONEAREST);
    {
        std::lock_guard<std::mutex> lock(g_mutex);
        auto it = g_byApp.find(app);
        if (it == g_byApp.end()) {
            return;
        }
        it->second.inner = inner;
        it->second.monitor = mon;
    }
    SelfSetWindowPos(app, nullptr, inner.left, inner.top,
                     inner.right - inner.left, inner.bottom - inner.top,
                     SWP_NOZORDER | SWP_NOACTIVATE);
    PositionBarIfVisible(container);  // the floating bar stays glued
}

// ---------------------------------------------------------------------------
// The container window
// ---------------------------------------------------------------------------

void ReleaseByApp(HWND app);  // fwd

HWND AppForContainer(HWND container) {
    std::lock_guard<std::mutex> lock(g_mutex);
    auto it = g_containerToApp.find(container);
    return it != g_containerToApp.end() ? it->second : nullptr;
}

void ReleaseByContainer(HWND container) {
    if (HWND app = AppForContainer(container)) {
        ReleaseByApp(app);
    }
}

// True while the (single) container is in an interactive move/size loop, so
// per-tick WM_SIZE handling skips the display-change broadcast.
std::atomic<bool> g_inSizeMove{false};

// ---------------------------------------------------------------------------
// Edge resistance ("virtual borders"): while the enclosed app is in the
// foreground and the cursor is inside the APP's area, clip the cursor to that
// area so it stops at the app's edge like at a real screen edge - that edge
// is where the app's own slam-to-edge UI lives, and the stop point accounts
// for the frame padding beyond it. Pressing against the boundary for
// edgeHoldMs releases it (freeing the frame strip and the rest of the
// desktop); it re-arms once the cursor comes back well inside. All state
// lives on the container's thread.
// ---------------------------------------------------------------------------

bool g_clipActive = false;
bool g_edgeReleased = false;
DWORD g_edgePressStart = 0;

void EdgeResistanceOff() {
    if (g_clipActive) {
        ClipCursor(nullptr);
        g_clipActive = false;
    }
    g_edgeReleased = false;
    g_edgePressStart = 0;
}

void EdgeResistanceTick(HWND container) {
    int hold = g_cfgEdgeHold.load();
    HWND app = AppForContainer(container);
    POINT pt;
    RECT inner;
    if (!app || hold <= 0 || g_inSizeMove.load() || g_inCaptionToggle.load() ||
        IsIconic(container) || !GetCursorPos(&pt) ||
        !OwnInnerRect(app, inner)) {
        EdgeResistanceOff();
        return;
    }
    AppParams p = ParamsForContainer(container);
    bool bL, bT, bR, bB;
    EffectiveBorders(p, bL, bT, bR, bB);
    if (!bL && !bT && !bR && !bB) {
        EdgeResistanceOff();  // all sides open for this app
        return;
    }
    HWND fg = GetForegroundWindow();
    bool ours = fg == app || fg == container ||
                (fg && GetAncestor(fg, GA_ROOTOWNER) == container);
    if (!ours) {
        EdgeResistanceOff();
        return;
    }

    if (g_edgeReleased) {
        // Recently let out: wait for the cursor to come back well inside
        // before arming again, so it can actually leave.
        RECT rearm = inner;
        InflateRect(&rearm, -24, -24);
        if (PtInRect(&rearm, pt)) {
            g_edgeReleased = false;
        }
        return;
    }

    if (!g_clipActive) {
        if (PtInRect(&inner, pt)) {
            g_clipActive = true;
            g_edgePressStart = 0;
        }
        return;
    }

    // The cursor stops only at ENABLED sides: the clip rect follows the app
    // area on those and opens to the virtual screen on the rest.
    RECT clip = inner;
    if (!bL) clip.left = RealGetSystemMetrics(SM_XVIRTUALSCREEN);
    if (!bT) clip.top = RealGetSystemMetrics(SM_YVIRTUALSCREEN);
    if (!bR) {
        clip.right = RealGetSystemMetrics(SM_XVIRTUALSCREEN) +
                     RealGetSystemMetrics(SM_CXVIRTUALSCREEN);
    }
    if (!bB) {
        clip.bottom = RealGetSystemMetrics(SM_YVIRTUALSCREEN) +
                      RealGetSystemMetrics(SM_CYVIRTUALSCREEN);
    }

    // Left the app area through an open side (or the container moved away):
    // stand down until the cursor comes back inside.
    if (!PtInRect(&inner, pt)) {
        ClipCursor(nullptr);
        g_clipActive = false;
        g_edgePressStart = 0;
        return;
    }

    // Re-assert only when the clip drifted - a constant re-clip can interfere
    // with apps that manage the cursor themselves.
    RECT cur;
    if (!GetClipCursor(&cur) || !EqualRect(&cur, &clip)) {
        ClipCursor(&clip);
    }
    const int e = 2;
    bool atEdge = (bL && pt.x <= inner.left + e) ||
                  (bR && pt.x >= inner.right - 1 - e) ||
                  (bT && pt.y <= inner.top + e) ||
                  (bB && pt.y >= inner.bottom - 1 - e);
    if (!atEdge) {
        g_edgePressStart = 0;
        return;
    }
    DWORD now = GetTickCount();
    if (!g_edgePressStart) {
        g_edgePressStart = now;
    } else if ((int)(now - g_edgePressStart) >= hold) {
        ClipCursor(nullptr);
        g_clipActive = false;
        g_edgeReleased = true;
        g_edgePressStart = 0;
    }
}

// ---------------------------------------------------------------------------
// The floating control bar (auto-hide mode). A small owner-drawn overlay pill
// - drag grip plus minimize / maximize / release - owned by the app so it
// floats above it. Being an overlay, showing or hiding it NEVER touches the
// container's frame, so the app's geometry (and the fake resolution) is
// untouchable in every state, maximized included.
// ---------------------------------------------------------------------------

std::atomic<HWND> g_bar{nullptr};
int g_barHot = -1;  // hovered button index; bar-thread only

HWND ContainerForBar(HWND bar) {
    HWND app = GetWindow(bar, GW_OWNER);
    if (!app) {
        return nullptr;
    }
    std::lock_guard<std::mutex> lock(g_mutex);
    auto it = g_byApp.find(app);
    return it != g_byApp.end() ? it->second.container : nullptr;
}

RECT BarButtonRect(const RECT& client, int i) {
    int left = client.right - (3 - i) * kBarBtnW;
    return RECT{left, 0, left + kBarBtnW, client.bottom};
}

int BarHitButton(HWND bar, int x, int y) {
    RECT rc;
    GetClientRect(bar, &rc);
    for (int i = 0; i < 3; i++) {
        RECT br = BarButtonRect(rc, i);
        POINT pt{x, y};
        if (PtInRect(&br, pt)) {
            return i;
        }
    }
    return -1;
}

LRESULT CALLBACK BarWndProc(HWND hWnd, UINT uMsg, WPARAM wParam,
                            LPARAM lParam) {
    switch (uMsg) {
        case WM_ERASEBKGND:
            return 1;

        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC dc = BeginPaint(hWnd, &ps);
            RECT rc;
            GetClientRect(hWnd, &rc);
            bool lightTheme = SystemUsesLightTheme();
            COLORREF bg = lightTheme ? RGB(243, 243, 243) : RGB(32, 32, 32);
            COLORREF fg = lightTheme ? RGB(30, 30, 30) : RGB(228, 228, 228);
            HBRUSH bb = CreateSolidBrush(bg);
            FillRect(dc, &rc, bb);
            DeleteObject(bb);

            HFONT font = CreateFontW(-12, 0, 0, 0, FW_NORMAL, FALSE, FALSE,
                                     FALSE, DEFAULT_CHARSET, 0, 0,
                                     CLEARTYPE_QUALITY, 0,
                                     L"Segoe MDL2 Assets");
            HFONT oldFont = (HFONT)SelectObject(dc, font);
            SetBkMode(dc, TRANSPARENT);

            HWND cont = ContainerForBar(hWnd);
            PCWSTR glyphs[3] = {
                L"",  // minimize
                (cont && IsZoomed(cont)) ? L"" : L"",  // restore/max
                L"",  // close glyph = release (never closes the app)
            };
            for (int i = 0; i < 3; i++) {
                RECT br = BarButtonRect(rc, i);
                bool hot = i == g_barHot;
                if (hot) {
                    COLORREF hbg = i == 2 ? RGB(196, 43, 28)
                                          : (lightTheme ? RGB(222, 222, 222)
                                                        : RGB(64, 64, 64));
                    HBRUSH hb = CreateSolidBrush(hbg);
                    FillRect(dc, &br, hb);
                    DeleteObject(hb);
                }
                SetTextColor(dc, i == 2 && hot ? RGB(255, 255, 255) : fg);
                DrawTextW(dc, glyphs[i], -1, &br,
                          DT_CENTER | DT_VCENTER | DT_SINGLELINE);
            }
            // Grip glyph on the draggable left part.
            SetTextColor(dc, fg);
            RECT grip{0, 0, rc.right - 3 * kBarBtnW, rc.bottom};
            DrawTextW(dc, L"", -1, &grip,
                      DT_CENTER | DT_VCENTER | DT_SINGLELINE);

            SelectObject(dc, oldFont);
            DeleteObject(font);
            EndPaint(hWnd, &ps);
            return 0;
        }

        case WM_MOUSEMOVE: {
            int hot = BarHitButton(hWnd, GET_X_LPARAM(lParam),
                                   GET_Y_LPARAM(lParam));
            if (hot != g_barHot) {
                g_barHot = hot;
                InvalidateRect(hWnd, nullptr, FALSE);
            }
            TRACKMOUSEEVENT tme{sizeof(tme), TME_LEAVE, hWnd, 0};
            TrackMouseEvent(&tme);
            break;
        }

        case WM_MOUSELEAVE:
            if (g_barHot != -1) {
                g_barHot = -1;
                InvalidateRect(hWnd, nullptr, FALSE);
            }
            break;

        case WM_LBUTTONDOWN:
            if (BarHitButton(hWnd, GET_X_LPARAM(lParam),
                             GET_Y_LPARAM(lParam)) == -1) {
                // The grip: hand the drag to the container.
                if (HWND cont = ContainerForBar(hWnd)) {
                    ReleaseCapture();
                    SendMessageW(cont, WM_NCLBUTTONDOWN, HTCAPTION, 0);
                }
            }
            break;

        case WM_LBUTTONUP: {
            int b = BarHitButton(hWnd, GET_X_LPARAM(lParam),
                                 GET_Y_LPARAM(lParam));
            HWND cont = ContainerForBar(hWnd);
            if (cont) {
                if (b == 0) {
                    PostMessageW(cont, WM_SYSCOMMAND, SC_MINIMIZE, 0);
                } else if (b == 1) {
                    PostMessageW(cont, WM_SYSCOMMAND,
                                 IsZoomed(cont) ? SC_RESTORE : SC_MAXIMIZE, 0);
                    InvalidateRect(hWnd, nullptr, FALSE);
                } else if (b == 2 && g_toggleMsg) {
                    PostMessageW(cont, g_toggleMsg, 0, 0);
                }
            }
            break;
        }

        case WM_RBUTTONUP:
            if (HWND cont = ContainerForBar(hWnd)) {
                HMENU menu = GetSystemMenu(cont, FALSE);
                POINT pt;
                GetCursorPos(&pt);
                int cmd = TrackPopupMenu(menu, TPM_RETURNCMD | TPM_RIGHTBUTTON,
                                         pt.x, pt.y, 0, hWnd, nullptr);
                if (cmd) {
                    SendMessageW(cont, WM_SYSCOMMAND, (WPARAM)cmd, 0);
                }
            }
            break;

        case WM_NCDESTROY:
            g_bar = nullptr;
            break;
    }
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

bool EnsureBarClass() {
    static bool registered = false;
    if (registered) {
        return true;
    }
    WNDCLASSEXW wc{};
    wc.cbSize = sizeof(wc);
    wc.lpfnWndProc = BarWndProc;
    wc.hInstance = g_hInst;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.lpszClassName = kBarClass;
    if (!RegisterClassExW(&wc)) {
        if (GetLastError() != ERROR_CLASS_ALREADY_EXISTS) {
            return false;
        }
        UnregisterClassW(kBarClass, g_hInst);
        if (!RegisterClassExW(&wc)) {
            return false;
        }
    }
    registered = true;
    return true;
}

void PositionBar(HWND container, HWND bar) {
    RECT cl = ContainerClientScreenRect(container);
    int x = cl.left + ((cl.right - cl.left) - kBarW) / 2;
    int y = cl.top + 2;
    SelfSetWindowPos(bar, nullptr, x, y, kBarW, kBarH,
                     SWP_NOZORDER | SWP_NOACTIVATE);
}

void PositionBarIfVisible(HWND container) {
    HWND bar = g_bar.load();
    if (bar && IsWindow(bar) && IsWindowVisible(bar)) {
        PositionBar(container, bar);
    }
}

void ShowBar(HWND container) {
    HWND app = AppForContainer(container);
    if (!app || !EnsureBarClass()) {
        return;
    }
    HWND bar = g_bar.load();
    if (!bar || !IsWindow(bar)) {
        // Owned by the APP so it floats above it; never activates.
        bar = CreateWindowExW(WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE, kBarClass,
                              L"", WS_POPUP, 0, 0, kBarW, kBarH, app, nullptr,
                              g_hInst, nullptr);
        if (!bar) {
            return;
        }
        int corner = 3;  // DWMWCP_ROUNDSMALL
        DwmSetWindowAttribute(bar, DWMWA_WINDOW_CORNER_PREFERENCE, &corner,
                              sizeof(corner));
        g_bar = bar;
    }
    PositionBar(container, bar);
    if (!IsWindowVisible(bar)) {
        t_selfOp = true;
        ShowWindow(bar, SW_SHOWNOACTIVATE);
        t_selfOp = false;
    }
    InvalidateRect(bar, nullptr, TRUE);
}

void HideBar() {
    HWND bar = g_bar.load();
    if (bar && IsWindow(bar) && IsWindowVisible(bar)) {
        ShowWindow(bar, SW_HIDE);
    }
}

LRESULT CALLBACK ContainerWndProc(HWND hWnd, UINT uMsg, WPARAM wParam,
                                  LPARAM lParam) {
    if (g_toggleMsg && uMsg == g_toggleMsg) {
        ReleaseByContainer(hWnd);
        return 0;
    }

    switch (uMsg) {
        case WM_NCHITTEST: {
            // The app covers the client area up to the frame strip, so the
            // strip itself must act as the resize border.
            LRESULT ht = DefWindowProc(hWnd, uMsg, wParam, lParam);
            if (ht == HTCLIENT) {
                POINT pt{GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
                RECT cl = ContainerClientScreenRect(hWnd);
                if (IsZoomed(hWnd)) {
                    // No resize handles on a maximized frame; the top strip
                    // drags (restore-drag) and hosts the bar reveal.
                    if (g_cfgAutoHide.load() &&
                        pt.y < cl.top + EffTopPad(hWnd)) {
                        return HTCAPTION;
                    }
                    return ht;
                }
                // The whole configurable strip must resize, not just the
                // first few pixels of it - a wider pad would otherwise leave
                // a dead band. The top zone stays slim: the strip below it is
                // the drag handle.
                int spL, spT, spR, spB;
                EffectivePads(ParamsForContainer(hWnd), false, spL, spT, spR,
                              spB);
                const int m = kMargin + 2;
                bool l = pt.x < cl.left + (spL > m ? spL : m);
                bool r = pt.x >= cl.right - (spR > m ? spR : m);
                bool t = pt.y < cl.top + m;
                bool b = pt.y >= cl.bottom - (spB > m ? spB : m);
                if (t && l) return HTTOPLEFT;
                if (t && r) return HTTOPRIGHT;
                if (b && l) return HTBOTTOMLEFT;
                if (b && r) return HTBOTTOMRIGHT;
                if (l) return HTLEFT;
                if (r) return HTRIGHT;
                if (t) return HTTOP;
                if (b) return HTBOTTOM;
                // Auto-hide mode: the padding strip doubles as the drag
                // handle (and right-click menu source) while the title bar
                // is tucked away.
                if (g_cfgAutoHide.load() &&
                    pt.y < cl.top + EffTopPad(hWnd)) {
                    return HTCAPTION;
                }
            }
            return ht;
        }

        case WM_NCMOUSEMOVE:
            // Hovering the top strip reveals the floating control bar.
            if (g_cfgAutoHide.load() && !g_captionShown.load() &&
                (wParam == HTCAPTION || wParam == HTTOP ||
                 wParam == HTTOPLEFT || wParam == HTTOPRIGHT)) {
                ShowBar(hWnd);
                SetTimer(hWnd, kAutoHideTimer, 300, nullptr);
            }
            break;

        case WM_TIMER:
            if (wParam == kEdgeTimer) {
                EdgeResistanceTick(hWnd);
                return 0;
            }
            if (wParam == kAutoHideTimer) {
                if (!g_cfgAutoHide.load()) {
                    KillTimer(hWnd, kAutoHideTimer);  // setting switched off
                    HideBar();
                    return 0;
                }
                if (IsIconic(hWnd) || g_inSizeMove.load()) {
                    return 0;  // don't fiddle mid-drag or while minimized
                }
                GUITHREADINFO gti{};
                gti.cbSize = sizeof(gti);
                if (GetGUIThreadInfo(GetCurrentThreadId(), &gti) &&
                    (gti.flags & (GUI_INMENUMODE | GUI_SYSTEMMENUMODE))) {
                    return 0;  // a menu is up (ours, probably) - keep it
                }
                POINT pt;
                GetCursorPos(&pt);
                RECT band = RevealBand(hWnd);
                bool onBar = false;
                HWND bar = g_bar.load();
                if (bar && IsWindow(bar) && IsWindowVisible(bar)) {
                    RECT br;
                    if (GetWindowRect(bar, &br)) {
                        onBar = PtInRect(&br, pt);
                    }
                }
                HWND fg = GetForegroundWindow();
                HWND app = AppForContainer(hWnd);
                bool ours = fg == app || fg == hWnd ||
                            (fg && GetAncestor(fg, GA_ROOTOWNER) == hWnd);
                if (!ours || (!PtInRect(&band, pt) && !onBar)) {
                    KillTimer(hWnd, kAutoHideTimer);
                    HideBar();
                }
                return 0;
            }
            break;

        case WM_GETMINMAXINFO: {
            // A caption-less window maximizes over the taskbar by default;
            // pin the maximize rect to the monitor's WORK area - through the
            // REAL query, since GetMonitorInfo is spoofed in this process.
            LRESULT r = DefWindowProc(hWnd, uMsg, wParam, lParam);
            HMONITOR mon = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
            MONITORINFO mi{};
            mi.cbSize = sizeof(mi);
            if (RealGetMonitorInfoW(mon, &mi)) {
                auto* mmi = reinterpret_cast<MINMAXINFO*>(lParam);
                mmi->ptMaxPosition.x = mi.rcWork.left - mi.rcMonitor.left;
                mmi->ptMaxPosition.y = mi.rcWork.top - mi.rcMonitor.top;
                mmi->ptMaxSize.x = mi.rcWork.right - mi.rcWork.left;
                mmi->ptMaxSize.y = mi.rcWork.bottom - mi.rcWork.top;

                // With an AUTO-HIDDEN taskbar the work area equals the whole
                // monitor, and a window covering every pixel is treated as
                // fullscreen by the shell, which then refuses to reveal the
                // taskbar on hover. Leave it a sliver on its edge.
                APPBARDATA abd{};
                abd.cbSize = sizeof(abd);
                if (SHAppBarMessage(ABM_GETSTATE, &abd) & ABS_AUTOHIDE) {
                    const UINT edges[4] = {ABE_BOTTOM, ABE_TOP, ABE_LEFT,
                                           ABE_RIGHT};
                    for (UINT edge : edges) {
                        APPBARDATA q{};
                        q.cbSize = sizeof(q);
                        q.uEdge = edge;
                        q.rc = mi.rcMonitor;
                        if (!SHAppBarMessage(ABM_GETAUTOHIDEBAREX, &q)) {
                            continue;
                        }
                        const int sliver = 2;
                        switch (edge) {
                            case ABE_BOTTOM:
                                mmi->ptMaxSize.y -= sliver;
                                break;
                            case ABE_TOP:
                                mmi->ptMaxPosition.y += sliver;
                                mmi->ptMaxSize.y -= sliver;
                                break;
                            case ABE_LEFT:
                                mmi->ptMaxPosition.x += sliver;
                                mmi->ptMaxSize.x -= sliver;
                                break;
                            case ABE_RIGHT:
                                mmi->ptMaxSize.x -= sliver;
                                break;
                        }
                        break;
                    }
                }
            }
            return r;
        }

        case WM_ENTERSIZEMOVE:
            g_inSizeMove = true;
            break;

        case WM_SIZE:
            if (wParam == SIZE_MINIMIZED || g_inCaptionToggle.load()) {
                break;  // minimize: owner carries the app; toggle: transient
            }
            if (HWND app = AppForContainer(hWnd)) {
                FitAppToContainer(app, hWnd);
                // Maximize / restore arrive as a single WM_SIZE with no
                // size-move loop, so broadcast the new geometry here.
                if (!g_inSizeMove.load() &&
                    (wParam == SIZE_MAXIMIZED || wParam == SIZE_RESTORED)) {
                    NotifyDisplayChange();
                }
            }
            break;

        case WM_MOVE:
            if (!IsIconic(hWnd) && !g_inCaptionToggle.load()) {
                if (HWND app = AppForContainer(hWnd)) {
                    FitAppToContainer(app, hWnd);
                }
            }
            break;

        case WM_EXITSIZEMOVE:
            g_inSizeMove = false;
            if (HWND app = AppForContainer(hWnd)) {
                FitAppToContainer(app, hWnd);
                NotifyDisplayChange();
                SetForegroundWindow(app);
            }
            break;

        case WM_SETTINGCHANGE:
            // Windows light/dark app theme switched: restyle the title bar
            // and repaint the floating bar.
            if (lParam &&
                _wcsicmp((PCWSTR)lParam, L"ImmersiveColorSet") == 0) {
                ApplyContainerTheme(hWnd);
                if (HWND bar = g_bar.load()) {
                    if (IsWindow(bar)) {
                        InvalidateRect(bar, nullptr, TRUE);
                    }
                }
            }
            break;

        case WM_SYSCOMMAND:
            if ((wParam & 0xFFF0) == IDM_ENCLOSE) {
                ReleaseByContainer(hWnd);
                return 0;
            }
            break;

        case WM_CLOSE:
            // Closing the frame releases the app (never closes the app).
            ReleaseByContainer(hWnd);
            return 0;
    }
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

bool EnsureContainerClass() {
    static bool registered = false;
    if (registered) {
        return true;
    }
    WNDCLASSEXW wc{};
    wc.cbSize = sizeof(wc);
    wc.lpfnWndProc = ContainerWndProc;
    wc.hInstance = g_hInst;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wc.lpszClassName = kContainerClass;
    if (!RegisterClassExW(&wc)) {
        if (GetLastError() != ERROR_CLASS_ALREADY_EXISTS) {
            return false;
        }
        // A stale registration from a previous load would carry a dead
        // wndproc - replace it, never reuse it.
        UnregisterClassW(kContainerClass, g_hInst);
        if (!RegisterClassExW(&wc)) {
            return false;
        }
    }
    registered = true;
    return true;
}

// ---------------------------------------------------------------------------
// Enclose / release
// ---------------------------------------------------------------------------

bool IsEnclosableTopLevel(HWND hWnd) {
    if (!IsWindow(hWnd) || GetAncestor(hWnd, GA_ROOT) != hWnd) {
        return false;
    }
    LONG_PTR style = GetWindowLongPtrW(hWnd, GWL_STYLE);
    LONG_PTR exStyle = GetWindowLongPtrW(hWnd, GWL_EXSTYLE);
    if ((style & WS_CHILD) || (exStyle & WS_EX_TOOLWINDOW)) {
        return false;
    }
    RECT rc;
    if (!GetWindowRect(hWnd, &rc)) {
        return false;
    }
    return (rc.right - rc.left) >= 200 && (rc.bottom - rc.top) >= 150;
}

void EncloseWindow(HWND app) {
    if (g_teardown.load() || IsContainer(app) || IsEnclosedApp(app) ||
        !IsEnclosableTopLevel(app)) {
        return;
    }

    // One enclosure per process: the fake screen is process-wide, so a second
    // one cannot be answered faithfully.
    {
        std::lock_guard<std::mutex> lock(g_mutex);
        if (!g_byApp.empty()) {
            Wh_Log(L"Refusing second enclosure in this process");
            MessageBeep(MB_ICONWARNING);
            return;
        }
    }

    if (!EnsureContainerClass()) {
        Wh_Log(L"Container class registration failed");
        return;
    }

    Enclosed info{};
    info.savedZoomed = IsZoomed(app);
    if (info.savedZoomed || IsIconic(app)) {
        SelfShowWindow(app, SW_RESTORE);
    }
    if (!GetWindowRect(app, &info.savedRect)) {
        return;
    }
    info.savedOwner = GetWindowLongPtrW(app, GWLP_HWNDPARENT);

    HMONITOR mon = MonitorFromWindow(app, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi{};
    mi.cbSize = sizeof(mi);
    RECT work{0, 0, RealGetSystemMetrics(SM_CXSCREEN),
              RealGetSystemMetrics(SM_CYSCREEN)};
    if (RealGetMonitorInfoW(mon, &mi)) {
        work = mi.rcWork;
    }

    // Per-app parameters (settings entry for this exe), with the one-shot CLI
    // overlay when this process was launched by enclose.exe.
    info.params = g_cliAdmitted ? g_cliParams : ResolveParams();
    info.fromCli = g_cliAdmitted;

    // Desired inner size: per-app, else global, else wrap the app as-is.
    AppParams globals = GlobalParams();
    int innerW = info.params.width > 0 ? info.params.width : globals.width;
    int innerH = info.params.height > 0 ? info.params.height : globals.height;
    if (innerW <= 0 || innerH <= 0) {
        innerW = info.savedRect.right - info.savedRect.left;
        innerH = info.savedRect.bottom - info.savedRect.top;
    }
    // The clamp must account for the (now configurable) pads, or the
    // container's outer rect can be born larger than the work area.
    int padL, padT, padR, padB;
    EffectivePads(info.params, false, padL, padT, padR, padB);
    int maxW = (work.right - work.left) * 95 / 100 - (padL + padR);
    int maxH = (work.bottom - work.top) * 90 / 100 - (padT + padB);
    if (maxW < 50) maxW = 50;
    if (maxH < 50) maxH = 50;
    if (innerW > maxW) innerW = maxW;
    if (innerH > maxH) innerH = maxH;

    // Outer rect that yields client = inner + effective pads, anchored where
    // the app already is (minimal jump). With auto-hide on, the container
    // starts without its title bar (hover the strip to reveal it).
    DWORD contStyle = WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS;
    bool autoHide = g_cfgAutoHide.load();
    if (autoHide) {
        contStyle &= ~(DWORD)WS_CAPTION;
    }
    g_captionShown = !autoHide;
    const DWORD contExStyle = WS_EX_APPWINDOW;
    RECT outer{info.savedRect.left - padL, info.savedRect.top - padT,
               info.savedRect.left + innerW + padR,
               info.savedRect.top + innerH + padB};
    AdjustWindowRectEx(&outer, contStyle, FALSE, contExStyle);
    int outerW = outer.right - outer.left;
    int outerH = outer.bottom - outer.top;
    int x = outer.left;
    int y = outer.top;
    if (x + outerW > work.right) x = work.right - outerW;
    if (y + outerH > work.bottom) y = work.bottom - outerH;
    if (x < work.left) x = work.left;
    if (y < work.top) y = work.top;

    WCHAR title[256];
    GetWindowTextW(app, title, ARRAYSIZE(title));

    t_creatingContainer = true;
    HWND container = CreateWindowExW(contExStyle, kContainerClass, title,
                                     contStyle, x, y, outerW, outerH, nullptr,
                                     nullptr, g_hInst, nullptr);
    t_creatingContainer = false;
    if (!container) {
        return;
    }

    // CreateWindowExW force-adds WS_CAPTION to any top-level non-popup
    // window, overriding the style we asked for - in auto-hide mode the
    // born-with caption must be stripped explicitly, before first show.
    if (autoHide) {
        LONG_PTR st = GetWindowLongPtrW(container, GWL_STYLE);
        if (st & WS_CAPTION) {
            SetWindowLongPtrW(container, GWL_STYLE,
                              st & ~(LONG_PTR)WS_CAPTION);
            SelfSetWindowPos(container, nullptr, 0, 0, 0, 0,
                             SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER |
                                 SWP_NOACTIVATE | SWP_FRAMECHANGED);
        }
    }

    ApplyContainerTheme(container);  // dark/light title bar, before first show

    // Square corners: Win11's default rounding leaves gaps around the
    // rectangular app inside.
    int corner = 1;  // DWMWCP_DONOTROUND
    DwmSetWindowAttribute(container, DWMWA_WINDOW_CORNER_PREFERENCE, &corner,
                          sizeof(corner));

    if (HICON ic = (HICON)SendMessageW(app, WM_GETICON, ICON_BIG, 0)) {
        SendMessageW(container, WM_SETICON, ICON_BIG, (LPARAM)ic);
    }

    if (HMENU sysMenu = GetSystemMenu(container, FALSE)) {
        InsertMenuW(sysMenu, (UINT)-1, MF_BYPOSITION | MF_SEPARATOR, 0, nullptr);
        InsertMenuW(sysMenu, (UINT)-1, MF_BYPOSITION | MF_STRING, IDM_ENCLOSE,
                    L"Release (un-enclose)");
    }

    // Own the app with the container: an invisible link that keeps z-order and
    // minimize together WITHOUT touching the app's border or styles.
    SetWindowLongPtrW(app, GWLP_HWNDPARENT, (LONG_PTR)container);

    {
        std::lock_guard<std::mutex> lock(g_mutex);
        info.container = container;
        info.monitor = mon;
        g_byApp[app] = info;
        g_containerToApp[container] = app;
        g_activeApp = app;
        g_anyEnclosed = true;
    }

    SelfShowWindow(container, SW_SHOWNOACTIVATE);
    FitAppToContainer(app, container);
    // The container was created at the top of the z-order - above the app it
    // now owns. Raise the app back over it, or the frame's dark client covers
    // the app until the next activation ("black interior until clicked").
    // Also drop any topmost state the app carried in - contained surfaces
    // must never sit above the taskbar.
    if (GetWindowLongPtrW(app, GWL_EXSTYLE) & WS_EX_TOPMOST) {
        SelfSetWindowPos(app, HWND_NOTOPMOST, 0, 0, 0, 0,
                         SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
    }
    SelfSetWindowPos(app, HWND_TOP, 0, 0, 0, 0,
                     SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
    SetForegroundWindow(app);
    NotifyDisplayChange();
    SetTimer(container, kEdgeTimer, 50, nullptr);  // edge-resistance poll

    Wh_Log(L"Enclosed app %p in container %p", app, container);
}

void ReleaseByApp(HWND app) {
    Enclosed info;
    {
        std::lock_guard<std::mutex> lock(g_mutex);
        auto it = g_byApp.find(app);
        if (it == g_byApp.end()) {
            return;
        }
        info = it->second;
        g_byApp.erase(it);
        g_containerToApp.erase(info.container);
        if (g_activeApp == app) {
            g_activeApp = g_byApp.empty() ? nullptr : g_byApp.begin()->first;
        }
        g_anyEnclosed = !g_byApp.empty();
    }

    if (IsWindow(app)) {
        SetWindowLongPtrW(app, GWLP_HWNDPARENT, info.savedOwner);
        SelfSetWindowPos(app, nullptr, info.savedRect.left, info.savedRect.top,
                         info.savedRect.right - info.savedRect.left,
                         info.savedRect.bottom - info.savedRect.top,
                         SWP_NOZORDER | SWP_NOACTIVATE);
        if (info.savedZoomed) {
            SelfShowWindow(app, SW_MAXIMIZE);
        }
    }

    HWND bar = g_bar.exchange(nullptr);
    if (bar && IsWindow(bar)) {
        DestroyWindow(bar);
    }
    if (IsWindow(info.container)) {
        DestroyWindow(info.container);
    }

    EdgeResistanceOff();  // never leave a cursor clip behind
    NotifyDisplayChange();
    Wh_Log(L"Released app %p", app);
}

void ToggleEnclose(HWND hWnd) {
    if (IsContainer(hWnd)) {
        ReleaseByContainer(hWnd);
    } else if (IsEnclosedApp(hWnd)) {
        ReleaseByApp(hWnd);
    } else {
        EncloseWindow(hWnd);
    }
}

// ---------------------------------------------------------------------------
// App-window system-menu item + message handling (subclass)
// ---------------------------------------------------------------------------

void EnsureMenuItem(HWND hWnd) {
    if (!(GetWindowLongPtrW(hWnd, GWL_STYLE) & WS_SYSMENU)) {
        return;
    }
    HMENU hMenu = GetSystemMenu(hWnd, FALSE);
    if (!hMenu) {
        return;
    }

    bool enclosed = IsEnclosedApp(hWnd);
    PCWSTR text = enclosed ? L"Release (un-enclose)" : L"Enclose window";

    MENUITEMINFOW mii{};
    mii.cbSize = sizeof(mii);
    mii.fMask = MIIM_ID | MIIM_STATE | MIIM_STRING;
    mii.wID = IDM_ENCLOSE;
    mii.fState = enclosed ? MFS_CHECKED : MFS_UNCHECKED;
    mii.dwTypeData = const_cast<LPWSTR>(text);

    if (GetMenuState(hMenu, IDM_ENCLOSE, MF_BYCOMMAND) == (UINT)-1) {
        InsertMenuW(hMenu, (UINT)-1, MF_BYPOSITION | MF_SEPARATOR, 0, nullptr);
        InsertMenuItemW(hMenu, (UINT)-1, TRUE, &mii);
    } else {
        SetMenuItemInfoW(hMenu, IDM_ENCLOSE, FALSE, &mii);
    }
}

LRESULT CALLBACK AppSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam,
                                 LPARAM lParam, DWORD_PTR) {
    if (g_toggleMsg && uMsg == g_toggleMsg) {
        ToggleEnclose(hWnd);
        return 0;
    }

    switch (uMsg) {
        case WM_INITMENUPOPUP:
            if (HIWORD(lParam)) {  // the window (system) menu
                EnsureMenuItem(hWnd);
            }
            break;

        case WM_SYSCOMMAND:
            if ((wParam & 0xFFF0) == IDM_ENCLOSE) {
                ToggleEnclose(hWnd);
                return 0;
            }
            break;

        case WM_GETMINMAXINFO:
            // Maximizing the enclosed app must land inside the container: the
            // kernel-side maximize geometry ignores our work-area spoof, so
            // supply it here. ptMaxPosition is monitor-relative.
            if (IsEnclosedApp(hWnd)) {
                LRESULT res = DefSubclassProc(hWnd, uMsg, wParam, lParam);
                RECT inner;
                if (OwnInnerRect(hWnd, inner)) {
                    HMONITOR mon =
                        MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
                    MONITORINFO mi{};
                    mi.cbSize = sizeof(mi);
                    if (RealGetMonitorInfoW(mon, &mi)) {
                        auto* mmi = reinterpret_cast<MINMAXINFO*>(lParam);
                        mmi->ptMaxSize.x = inner.right - inner.left;
                        mmi->ptMaxSize.y = inner.bottom - inner.top;
                        mmi->ptMaxPosition.x = inner.left - mi.rcMonitor.left;
                        mmi->ptMaxPosition.y = inner.top - mi.rcMonitor.top;
                    }
                }
                return res;
            }
            break;

        case WM_NCDESTROY:
            if (IsEnclosedApp(hWnd)) {
                HWND container = nullptr;
                {
                    std::lock_guard<std::mutex> lock(g_mutex);
                    auto it = g_byApp.find(hWnd);
                    if (it != g_byApp.end()) {
                        container = it->second.container;
                        g_containerToApp.erase(container);
                        g_byApp.erase(it);
                        if (g_activeApp == hWnd) {
                            g_activeApp =
                                g_byApp.empty() ? nullptr : g_byApp.begin()->first;
                        }
                        g_anyEnclosed = !g_byApp.empty();
                    }
                }
                if (IsWindow(container)) {
                    DestroyWindow(container);
                }
                EdgeResistanceOff();
            }
            break;
    }
    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

// In-flight settle threads; Wh_ModBeforeUninit drains this to 0 before the
// DLL can unload (the borderless-fullscreen.wh.cpp pattern).
std::atomic<int> g_pendingSettle{0};

void SubclassIfCandidate(HWND hWnd);  // fwd

// The largest currently-visible enclosable top-level window of THIS process -
// so we grab the real main window, not a splash or a hidden helper.
BOOL CALLBACK FindBestWindowProc(HWND hWnd, LPARAM lParam) {
    DWORD pid = 0;
    GetWindowThreadProcessId(hWnd, &pid);
    if (pid != GetCurrentProcessId() || IsContainer(hWnd)) {
        return TRUE;
    }
    if (!IsWindowVisible(hWnd) || !IsEnclosableTopLevel(hWnd)) {
        return TRUE;
    }
    RECT rc;
    if (!GetWindowRect(hWnd, &rc)) {
        return TRUE;
    }
    long area = (long)(rc.right - rc.left) * (rc.bottom - rc.top);
    auto* best = reinterpret_cast<std::pair<HWND, long>*>(lParam);
    if (area > best->second) {
        best->second = area;
        best->first = hWnd;
    }
    return TRUE;
}

// CLI auto-enclose: a process admitted by enclose.exe polls for its main
// window (handling splash-first startups and windows shown after a delay),
// then routes it through the normal toggle path. Runs once per CLI process.
DWORD WINAPI AutoEncloseThread(LPVOID) {
    for (int i = 0; i < 16 && !g_teardown.load() && g_autoEnclosePending.load();
         i++) {
        Sleep(400);
        if (g_teardown.load()) {
            break;
        }
        std::pair<HWND, long> best{nullptr, 0};
        EnumWindows(FindBestWindowProc, reinterpret_cast<LPARAM>(&best));
        if (best.first) {
            g_autoEnclosePending = false;
            SubclassIfCandidate(best.first);  // so it hears the toggle message
            if (g_toggleMsg) {
                PostMessageW(best.first, g_toggleMsg, 0, 0);
            }
            break;
        }
    }
    g_pendingSettle--;
    return 0;
}

void SubclassIfCandidate(HWND hWnd) {
    if (g_teardown.load() || GetAncestor(hWnd, GA_ROOT) != hWnd) {
        return;
    }
    LONG_PTR style = GetWindowLongPtrW(hWnd, GWL_STYLE);
    LONG_PTR exStyle = GetWindowLongPtrW(hWnd, GWL_EXSTYLE);
    if ((style & WS_CHILD) || (exStyle & WS_EX_TOOLWINDOW) || IsContainer(hWnd)) {
        return;
    }
    WindhawkUtils::SetWindowSubclassFromAnyThread(hWnd, AppSubclassProc, 0);
}

// ---------------------------------------------------------------------------
// Catch new and existing windows
// ---------------------------------------------------------------------------

using CreateWindowExW_t = decltype(&CreateWindowExW);
CreateWindowExW_t CreateWindowExW_Original;
HWND WINAPI CreateWindowExW_Hook(DWORD dwExStyle, LPCWSTR lpClassName,
                                 LPCWSTR lpWindowName, DWORD dwStyle, int X,
                                 int Y, int nWidth, int nHeight, HWND hWndParent,
                                 HMENU hMenu, HINSTANCE hInstance, PVOID lpParam) {
    HWND hWnd = CreateWindowExW_Original(dwExStyle, lpClassName, lpWindowName,
                                         dwStyle, X, Y, nWidth, nHeight,
                                         hWndParent, hMenu, hInstance, lpParam);
    if (hWnd && !t_creatingContainer && !(dwStyle & WS_CHILD) &&
        !(dwExStyle & WS_EX_TOOLWINDOW) && !hWndParent) {
        SubclassIfCandidate(hWnd);
    }
    return hWnd;
}

BOOL CALLBACK EnumExistingProc(HWND hWnd, LPARAM) {
    DWORD pid = 0;
    GetWindowThreadProcessId(hWnd, &pid);
    if (pid == GetCurrentProcessId()) {
        SubclassIfCandidate(hWnd);
    }
    return TRUE;
}

// ---------------------------------------------------------------------------
// Resolution / monitor spoof (inert until a window is enclosed)
// ---------------------------------------------------------------------------

int WINAPI GetSystemMetrics_Hook(int nIndex) {
    if (SpoofActive()) {
        RECT rc;
        switch (nIndex) {
            case SM_CXSCREEN:
            case SM_CXFULLSCREEN:
            case SM_CXMAXIMIZED:
            case SM_CXVIRTUALSCREEN:
                if (ActiveFakeRect(rc)) return rc.right - rc.left;
                break;
            case SM_CYSCREEN:
            case SM_CYFULLSCREEN:
            case SM_CYMAXIMIZED:
            case SM_CYVIRTUALSCREEN:
                if (ActiveFakeRect(rc)) return rc.bottom - rc.top;
                break;
            case SM_XVIRTUALSCREEN:
                if (ActiveFakeRect(rc)) return rc.left;
                break;
            case SM_YVIRTUALSCREEN:
                if (ActiveFakeRect(rc)) return rc.top;
                break;
            case SM_CMONITORS:
                if (g_anyEnclosed.load()) return 1;
                break;
        }
    }
    return GetSystemMetrics_Original(nIndex);
}

BOOL WINAPI GetMonitorInfoW_Hook(HMONITOR hMonitor, LPMONITORINFO lpmi) {
    BOOL ok = GetMonitorInfoW_Original(hMonitor, lpmi);
    if (ok && lpmi && SpoofActive()) {
        RECT inner;
        if (EnclosedInnerForMonitor(hMonitor, inner)) {
            lpmi->rcMonitor = inner;
            lpmi->rcWork = inner;
        }
    }
    return ok;
}

BOOL WINAPI GetMonitorInfoA_Hook(HMONITOR hMonitor, LPMONITORINFO lpmi) {
    BOOL ok = GetMonitorInfoA_Original(hMonitor, lpmi);
    if (ok && lpmi && SpoofActive()) {
        RECT inner;
        if (EnclosedInnerForMonitor(hMonitor, inner)) {
            lpmi->rcMonitor = inner;
            lpmi->rcWork = inner;
        }
    }
    return ok;
}

using EnumDisplayMonitors_t = decltype(&EnumDisplayMonitors);
EnumDisplayMonitors_t EnumDisplayMonitors_Original;
BOOL WINAPI EnumDisplayMonitors_Hook(HDC hdc, LPCRECT lprcClip,
                                     MONITORENUMPROC lpfnEnum, LPARAM dwData) {
    if (!hdc && !lprcClip && lpfnEnum && SpoofActive()) {
        HMONITOR mon;
        RECT rc;
        if (ActiveFakeMonitor(mon, rc)) {
            lpfnEnum(mon, nullptr, &rc, dwData);  // one display, enclosure-sized
            return TRUE;
        }
    }
    return EnumDisplayMonitors_Original(hdc, lprcClip, lpfnEnum, dwData);
}

using SystemParametersInfoW_t = decltype(&SystemParametersInfoW);
SystemParametersInfoW_t SystemParametersInfoW_Original;
BOOL WINAPI SystemParametersInfoW_Hook(UINT uiAction, UINT uiParam,
                                       PVOID pvParam, UINT fWinIni) {
    BOOL ok = SystemParametersInfoW_Original(uiAction, uiParam, pvParam, fWinIni);
    if (ok && uiAction == SPI_GETWORKAREA && pvParam && SpoofActive()) {
        RECT rc;
        if (ActiveFakeRect(rc)) {
            *(RECT*)pvParam = rc;
        }
    }
    return ok;
}

using SystemParametersInfoA_t = decltype(&SystemParametersInfoA);
SystemParametersInfoA_t SystemParametersInfoA_Original;
BOOL WINAPI SystemParametersInfoA_Hook(UINT uiAction, UINT uiParam,
                                       PVOID pvParam, UINT fWinIni) {
    BOOL ok = SystemParametersInfoA_Original(uiAction, uiParam, pvParam, fWinIni);
    if (ok && uiAction == SPI_GETWORKAREA && pvParam && SpoofActive()) {
        RECT rc;
        if (ActiveFakeRect(rc)) {
            *(RECT*)pvParam = rc;
        }
    }
    return ok;
}

int WINAPI GetDeviceCaps_Hook(HDC hdc, int index) {
    if (SpoofActive() &&
        (index == HORZRES || index == VERTRES || index == DESKTOPHORZRES ||
         index == DESKTOPVERTRES) &&
        hdc && GetObjectType(hdc) == OBJ_DC &&
        GetDeviceCaps_Original(hdc, TECHNOLOGY) == DT_RASDISPLAY) {
        RECT rc;
        if (ActiveFakeRect(rc)) {
            if (index == HORZRES || index == DESKTOPHORZRES) {
                return rc.right - rc.left;
            }
            return rc.bottom - rc.top;
        }
    }
    return GetDeviceCaps_Original(hdc, index);
}

using EnumDisplaySettingsW_t = decltype(&EnumDisplaySettingsW);
EnumDisplaySettingsW_t EnumDisplaySettingsW_Original;
BOOL WINAPI EnumDisplaySettingsW_Hook(LPCWSTR lpszDeviceName, DWORD iModeNum,
                                      DEVMODEW* lpDevMode) {
    BOOL ok = EnumDisplaySettingsW_Original(lpszDeviceName, iModeNum, lpDevMode);
    if (ok && lpDevMode && iModeNum == ENUM_CURRENT_SETTINGS && SpoofActive()) {
        RECT rc;
        if (ActiveFakeRect(rc)) {
            lpDevMode->dmPelsWidth = (DWORD)(rc.right - rc.left);
            lpDevMode->dmPelsHeight = (DWORD)(rc.bottom - rc.top);
            lpDevMode->dmFields |= DM_PELSWIDTH | DM_PELSHEIGHT;
        }
    }
    return ok;
}

using EnumDisplaySettingsA_t = decltype(&EnumDisplaySettingsA);
EnumDisplaySettingsA_t EnumDisplaySettingsA_Original;
BOOL WINAPI EnumDisplaySettingsA_Hook(LPCSTR lpszDeviceName, DWORD iModeNum,
                                      DEVMODEA* lpDevMode) {
    BOOL ok = EnumDisplaySettingsA_Original(lpszDeviceName, iModeNum, lpDevMode);
    if (ok && lpDevMode && iModeNum == ENUM_CURRENT_SETTINGS && SpoofActive()) {
        RECT rc;
        if (ActiveFakeRect(rc)) {
            lpDevMode->dmPelsWidth = (DWORD)(rc.right - rc.left);
            lpDevMode->dmPelsHeight = (DWORD)(rc.bottom - rc.top);
            lpDevMode->dmFields |= DM_PELSWIDTH | DM_PELSHEIGHT;
        }
    }
    return ok;
}

using ChangeDisplaySettingsExW_t = decltype(&ChangeDisplaySettingsExW);
ChangeDisplaySettingsExW_t ChangeDisplaySettingsExW_Original;
LONG WINAPI ChangeDisplaySettingsExW_Hook(LPCWSTR lpszDeviceName,
                                          DEVMODEW* lpDevMode, HWND hwnd,
                                          DWORD dwflags, LPVOID lParam) {
    if (SpoofActive() && lpDevMode) {
        return DISP_CHANGE_SUCCESSFUL;  // never let it change the real desktop
    }
    return ChangeDisplaySettingsExW_Original(lpszDeviceName, lpDevMode, hwnd,
                                             dwflags, lParam);
}

using ChangeDisplaySettingsExA_t = decltype(&ChangeDisplaySettingsExA);
ChangeDisplaySettingsExA_t ChangeDisplaySettingsExA_Original;
LONG WINAPI ChangeDisplaySettingsExA_Hook(LPCSTR lpszDeviceName,
                                          DEVMODEA* lpDevMode, HWND hwnd,
                                          DWORD dwflags, LPVOID lParam) {
    if (SpoofActive() && lpDevMode) {
        return DISP_CHANGE_SUCCESSFUL;
    }
    return ChangeDisplaySettingsExA_Original(lpszDeviceName, lpDevMode, hwnd,
                                             dwflags, lParam);
}

// ---------------------------------------------------------------------------
// The safety net: monitor-covering placements land in the container.
//
// Whatever API told the app how big the screen is (including ones we cannot
// hook, like DXGI), the moment it positions a window over the real monitor -
// or at a monitor origin with the spoofed screen size - we place it into the
// enclosure instead. Covers SetWindowPos, MoveWindow, DeferWindowPos and
// SetWindowPlacement, including split move-only / size-only calls.
// ---------------------------------------------------------------------------

bool RedirectCoveringRect(HWND hWnd, int& x, int& y, int& cx, int& cy) {
    if (t_selfOp || !SpoofActive()) {
        return false;
    }
    if (GetAncestor(hWnd, GA_ROOT) != hWnd || IsContainer(hWnd)) {
        return false;
    }
    LONG_PTR style = GetWindowLongPtrW(hWnd, GWL_STYLE);
    if (style & (WS_CHILD | WS_MAXIMIZE)) {
        return false;  // a maximize is not a fullscreen takeover
    }

    RECT fake;
    if (!ActiveFakeRect(fake)) {
        return false;
    }
    // Already aimed at the enclosure? Nothing to do.
    if (x == fake.left && y == fake.top) {
        return false;
    }

    RECT target{x, y, x + cx, y + cy};
    HMONITOR mon = MonitorFromRect(&target, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi{};
    mi.cbSize = sizeof(mi);
    if (!mon || !RealGetMonitorInfoW(mon, &mi)) {
        return false;
    }
    const int slack = 2;

    // Trigger 1: the rect essentially covers the real monitor (the app read
    // the true screen size somewhere we cannot spoof).
    bool covering = target.left <= mi.rcMonitor.left + slack &&
                    target.top <= mi.rcMonitor.top + slack &&
                    target.right >= mi.rcMonitor.right - slack &&
                    target.bottom >= mi.rcMonitor.bottom - slack;

    // Trigger 2: the rect is the spoofed screen SIZE placed at a monitor
    // origin - the classic `SetWindowPos(0, 0, SM_CXSCREEN, SM_CYSCREEN)`
    // fullscreen idiom, which believes the screen starts at (0,0).
    int fw = fake.right - fake.left;
    int fh = fake.bottom - fake.top;
    bool originIdiom = !covering && std::abs(cx - fw) <= 2 * slack &&
                       std::abs(cy - fh) <= 2 * slack &&
                       std::abs(x - mi.rcMonitor.left) <= slack &&
                       std::abs(y - mi.rcMonitor.top) <= slack;

    if (!covering && !originIdiom) {
        return false;
    }

    x = fake.left;
    y = fake.top;
    cx = fw;
    cy = fh;
    Wh_Log(L"Redirected %s placement of %p into the enclosure",
           covering ? L"monitor-covering" : L"origin-idiom", hWnd);
    return true;
}

// Compose the effective rect of a placement call (filling in components the
// flags freeze from the current window rect), run the net, and if it
// redirects, force the full corrected rect to apply.
bool NetOnPlacement(HWND hWnd, int& x, int& y, int& cx, int& cy, UINT* uFlags) {
    if (t_selfOp || !SpoofActive()) {
        return false;
    }
    int ex = x, ey = y, ecx = cx, ecy = cy;
    if (uFlags && (*uFlags & (SWP_NOSIZE | SWP_NOMOVE))) {
        if ((*uFlags & (SWP_NOSIZE | SWP_NOMOVE)) == (SWP_NOSIZE | SWP_NOMOVE)) {
            return false;  // pure z-order/frame call, no geometry
        }
        RECT cur;
        if (!GetWindowRect(hWnd, &cur)) {
            return false;
        }
        if (*uFlags & SWP_NOMOVE) {
            ex = cur.left;
            ey = cur.top;
        }
        if (*uFlags & SWP_NOSIZE) {
            ecx = cur.right - cur.left;
            ecy = cur.bottom - cur.top;
        }
    }
    if (!RedirectCoveringRect(hWnd, ex, ey, ecx, ecy)) {
        return false;
    }
    x = ex;
    y = ey;
    cx = ecx;
    cy = ecy;
    if (uFlags) {
        *uFlags &= ~(SWP_NOSIZE | SWP_NOMOVE);
    }
    return true;
}

// A contained surface must never sit in the topmost band: it would cover the
// auto-hidden taskbar's reveal (seen as "taskbar never shows" or "shows under
// the window"). Demote topmost assertions by the enclosed app or by any
// window covering most of the enclosure - but leave small popups (menus,
// tooltips, IMEs) alone.
bool ShouldDemoteTopmost(HWND hWnd) {
    if (t_selfOp || !SpoofActive()) {
        return false;
    }
    if (GetAncestor(hWnd, GA_ROOT) != hWnd || IsContainer(hWnd)) {
        return false;
    }
    if (IsEnclosedApp(hWnd)) {
        return true;
    }
    if (GetWindowLongPtrW(hWnd, GWL_EXSTYLE) & WS_EX_TOOLWINDOW) {
        return false;
    }
    RECT fake, wr, ix;
    if (!ActiveFakeRect(fake) || !GetWindowRect(hWnd, &wr) ||
        !IntersectRect(&ix, &fake, &wr)) {
        return false;
    }
    long cover = (long)(ix.right - ix.left) * (ix.bottom - ix.top);
    long total = (long)(fake.right - fake.left) * (fake.bottom - fake.top);
    return total > 0 && cover * 10 >= total * 8;  // covers >= 80% of it
}

BOOL WINAPI SetWindowPos_Hook(HWND hWnd, HWND hWndInsertAfter, int X, int Y,
                              int cx, int cy, UINT uFlags) {
    bool redirected = NetOnPlacement(hWnd, X, Y, cx, cy, &uFlags);
    if (hWndInsertAfter == HWND_TOPMOST && !(uFlags & SWP_NOZORDER) &&
        ShouldDemoteTopmost(hWnd)) {
        hWndInsertAfter = HWND_NOTOPMOST;
        Wh_Log(L"Demoted topmost assertion of %p", hWnd);
    }
    if (redirected &&
        (GetWindowLongPtrW(hWnd, GWL_EXSTYLE) & WS_EX_TOPMOST)) {
        // Pulled into the enclosure while already topmost: drop that too.
        hWndInsertAfter = HWND_NOTOPMOST;
        uFlags &= ~(UINT)SWP_NOZORDER;
    }
    return SetWindowPos_Original(hWnd, hWndInsertAfter, X, Y, cx, cy, uFlags);
}

using MoveWindow_t = decltype(&MoveWindow);
MoveWindow_t MoveWindow_Original;
BOOL WINAPI MoveWindow_Hook(HWND hWnd, int X, int Y, int nWidth, int nHeight,
                            BOOL bRepaint) {
    NetOnPlacement(hWnd, X, Y, nWidth, nHeight, nullptr);
    return MoveWindow_Original(hWnd, X, Y, nWidth, nHeight, bRepaint);
}

using DeferWindowPos_t = decltype(&DeferWindowPos);
DeferWindowPos_t DeferWindowPos_Original;
HDWP WINAPI DeferWindowPos_Hook(HDWP hWinPosInfo, HWND hWnd,
                                HWND hWndInsertAfter, int x, int y, int cx,
                                int cy, UINT uFlags) {
    NetOnPlacement(hWnd, x, y, cx, cy, &uFlags);
    if (hWndInsertAfter == HWND_TOPMOST && !(uFlags & SWP_NOZORDER) &&
        ShouldDemoteTopmost(hWnd)) {
        hWndInsertAfter = HWND_NOTOPMOST;
    }
    return DeferWindowPos_Original(hWinPosInfo, hWnd, hWndInsertAfter, x, y, cx,
                                   cy, uFlags);
}

using SetWindowPlacement_t = decltype(&SetWindowPlacement);
SetWindowPlacement_t SetWindowPlacement_Original;
BOOL WINAPI SetWindowPlacement_Hook(HWND hWnd,
                                    const WINDOWPLACEMENT* lpwndpl) {
    if (!t_selfOp && SpoofActive() && lpwndpl &&
        lpwndpl->length >= sizeof(WINDOWPLACEMENT)) {
        const RECT& r = lpwndpl->rcNormalPosition;
        int x = r.left, y = r.top;
        int cx = r.right - r.left, cy = r.bottom - r.top;
        if (RedirectCoveringRect(hWnd, x, y, cx, cy)) {
            WINDOWPLACEMENT wp = *lpwndpl;
            wp.rcNormalPosition = RECT{x, y, x + cx, y + cy};
            return SetWindowPlacement_Original(hWnd, &wp);
        }
    }
    return SetWindowPlacement_Original(hWnd, lpwndpl);
}

// ---------------------------------------------------------------------------
// Global hotkey (hosted by the explorer.exe instance)
// ---------------------------------------------------------------------------

HANDLE g_hotkeyThread;
DWORD g_hotkeyThreadId;
HANDLE g_hotkeyReady;  // signaled once the thread's message queue exists

LRESULT CALLBACK HotkeyWndProc(HWND hWnd, UINT uMsg, WPARAM wParam,
                               LPARAM lParam) {
    if (uMsg == WM_HOTKEY) {
        HWND fg = GetForegroundWindow();
        if (fg) {
            PostMessageW(GetAncestor(fg, GA_ROOT), g_toggleMsg, 0, 0);
        }
        return 0;
    }
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

DWORD WINAPI HotkeyThreadProc(LPVOID) {
    // Force queue creation, then signal readiness so shutdown can trust
    // PostThreadMessage(WM_QUIT) to be deliverable.
    MSG msg;
    PeekMessageW(&msg, nullptr, 0, 0, PM_NOREMOVE);
    if (g_hotkeyReady) {
        SetEvent(g_hotkeyReady);
    }

    WNDCLASSEXW wc{};
    wc.cbSize = sizeof(wc);
    wc.lpfnWndProc = HotkeyWndProc;
    wc.hInstance = g_hInst;
    wc.lpszClassName = kHotkeyClass;
    if (!RegisterClassExW(&wc) &&
        GetLastError() != ERROR_CLASS_ALREADY_EXISTS) {
        return 0;
    }

    HWND hWnd = CreateWindowExW(0, kHotkeyClass, L"", 0, 0, 0, 0, 0,
                                HWND_MESSAGE, nullptr, g_hInst, nullptr);
    if (!hWnd) {
        UnregisterClassW(kHotkeyClass, g_hInst);
        return 0;
    }

    if (!RegisterHotKey(hWnd, 1,
                        MOD_CONTROL | MOD_ALT | MOD_SHIFT | MOD_NOREPEAT,
                        VK_SPACE)) {
        Wh_Log(L"RegisterHotKey failed (%u) - is the combo taken?",
               GetLastError());
    }

    while (GetMessage(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    UnregisterHotKey(hWnd, 1);
    DestroyWindow(hWnd);
    UnregisterClassW(kHotkeyClass, g_hInst);
    return 0;
}

void StartHotkeyThread() {
    if (g_hotkeyThread) {
        return;
    }
    g_hotkeyReady = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_hotkeyThread = CreateThread(nullptr, 0, HotkeyThreadProc, nullptr, 0,
                                  &g_hotkeyThreadId);
    if (!g_hotkeyThread && g_hotkeyReady) {
        CloseHandle(g_hotkeyReady);
        g_hotkeyReady = nullptr;
    }
}

void StopHotkeyThread() {
    if (!g_hotkeyThread) {
        return;
    }
    // The thread must never outlive the DLL: wait for its queue, deliver
    // WM_QUIT for certain, then wait for exit without a timeout (the loop
    // only ever blocks in GetMessage, so this terminates promptly).
    if (g_hotkeyReady) {
        WaitForSingleObject(g_hotkeyReady, 5000);
    }
    while (!PostThreadMessageW(g_hotkeyThreadId, WM_QUIT, 0, 0)) {
        if (WaitForSingleObject(g_hotkeyThread, 50) == WAIT_OBJECT_0) {
            break;  // already exited on its own
        }
    }
    WaitForSingleObject(g_hotkeyThread, INFINITE);
    CloseHandle(g_hotkeyThread);
    g_hotkeyThread = nullptr;
    g_hotkeyThreadId = 0;
    if (g_hotkeyReady) {
        CloseHandle(g_hotkeyReady);
        g_hotkeyReady = nullptr;
    }
}

// ---------------------------------------------------------------------------
// Mod entry points
// ---------------------------------------------------------------------------

BOOL Wh_ModInit() {
    LoadSettings();

    std::wstring self = ProcessBaseName();
    g_isExplorer = _wcsicmp(self.c_str(), L"explorer.exe") == 0;
    g_isTarget = ProcessIsTarget();

    if (!g_isTarget) {
        // A fresh one-shot request from enclose.exe admits a process that is
        // not in the Applications list, with per-launch parameters.
        AppParams p = ResolveParams();
        if (ReadAndConsumeCliRequest(self, p)) {
            g_cliAdmitted = true;
            g_cliParams = p;
            g_autoEnclosePending = true;
            g_isTarget = true;  // full functionality in this process
            Wh_Log(L"CLI-admitted %s", self.c_str());
        }
    }
    if (!g_isTarget && !g_isExplorer) {
        return FALSE;  // stay out of everything not on the list
    }

    Wh_Log(L"Init in %s (target=%d, hotkey-host=%d)", self.c_str(), g_isTarget,
           g_isExplorer);

    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                           GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                       (LPCWSTR)&g_toggleMsg, &g_hInst);
    g_toggleMsg = RegisterWindowMessageW(L"Windhawk_Enclose_Toggle_v1");
    if (!g_toggleMsg) {
        return FALSE;  // 0 would collide with WM_NULL liveness probes
    }

    if (g_isTarget) {
        Wh_SetFunctionHook((void*)CreateWindowExW, (void*)CreateWindowExW_Hook,
                           (void**)&CreateWindowExW_Original);
        Wh_SetFunctionHook((void*)GetSystemMetrics, (void*)GetSystemMetrics_Hook,
                           (void**)&GetSystemMetrics_Original);
        Wh_SetFunctionHook((void*)GetMonitorInfoW, (void*)GetMonitorInfoW_Hook,
                           (void**)&GetMonitorInfoW_Original);
        Wh_SetFunctionHook((void*)GetMonitorInfoA, (void*)GetMonitorInfoA_Hook,
                           (void**)&GetMonitorInfoA_Original);
        Wh_SetFunctionHook((void*)EnumDisplayMonitors,
                           (void*)EnumDisplayMonitors_Hook,
                           (void**)&EnumDisplayMonitors_Original);
        Wh_SetFunctionHook((void*)SystemParametersInfoW,
                           (void*)SystemParametersInfoW_Hook,
                           (void**)&SystemParametersInfoW_Original);
        Wh_SetFunctionHook((void*)SystemParametersInfoA,
                           (void*)SystemParametersInfoA_Hook,
                           (void**)&SystemParametersInfoA_Original);
        Wh_SetFunctionHook((void*)GetDeviceCaps, (void*)GetDeviceCaps_Hook,
                           (void**)&GetDeviceCaps_Original);
        Wh_SetFunctionHook((void*)EnumDisplaySettingsW,
                           (void*)EnumDisplaySettingsW_Hook,
                           (void**)&EnumDisplaySettingsW_Original);
        Wh_SetFunctionHook((void*)EnumDisplaySettingsA,
                           (void*)EnumDisplaySettingsA_Hook,
                           (void**)&EnumDisplaySettingsA_Original);
        Wh_SetFunctionHook((void*)ChangeDisplaySettingsExW,
                           (void*)ChangeDisplaySettingsExW_Hook,
                           (void**)&ChangeDisplaySettingsExW_Original);
        Wh_SetFunctionHook((void*)ChangeDisplaySettingsExA,
                           (void*)ChangeDisplaySettingsExA_Hook,
                           (void**)&ChangeDisplaySettingsExA_Original);
        Wh_SetFunctionHook((void*)SetWindowPos, (void*)SetWindowPos_Hook,
                           (void**)&SetWindowPos_Original);
        Wh_SetFunctionHook((void*)MoveWindow, (void*)MoveWindow_Hook,
                           (void**)&MoveWindow_Original);
        Wh_SetFunctionHook((void*)DeferWindowPos, (void*)DeferWindowPos_Hook,
                           (void**)&DeferWindowPos_Original);
        Wh_SetFunctionHook((void*)SetWindowPlacement,
                           (void*)SetWindowPlacement_Hook,
                           (void**)&SetWindowPlacement_Original);
    }

    return TRUE;
}

void Wh_ModAfterInit() {
    if (g_isTarget) {
        EnumWindows(EnumExistingProc, 0);
    }
    if (g_autoEnclosePending.load()) {
        // CLI-admitted: start the finder that will enclose the main window
        // once it appears (drained at teardown via g_pendingSettle).
        g_pendingSettle++;
        HANDLE t = CreateThread(nullptr, 0, AutoEncloseThread, nullptr, 0,
                                nullptr);
        if (t) {
            CloseHandle(t);
        } else {
            g_pendingSettle--;
        }
    }
    if (g_isExplorer && g_cfgHotkey.load()) {
        StartHotkeyThread();
    }
}

void Wh_ModSettingsChanged() {
    LoadSettings();
    if (g_isExplorer) {
        if (g_cfgHotkey.load()) {
            StartHotkeyThread();
        } else {
            StopHotkeyThread();
        }
    }
    if (g_isTarget) {
        // Apply changed parameters / auto-hide to live enclosures. CLI-
        // parameterized enclosures keep their per-launch values.
        std::vector<std::pair<HWND, HWND>> pairs;
        AppParams fresh = ResolveParams();
        {
            std::lock_guard<std::mutex> lock(g_mutex);
            for (auto& [app, info] : g_byApp) {
                if (!info.fromCli) {
                    info.params = fresh;
                }
                pairs.push_back({app, info.container});
            }
        }
        bool autoHide = g_cfgAutoHide.load();
        for (const auto& [app, container] : pairs) {
            if (IsWindow(app) && IsWindow(container)) {
                if (!autoHide) {
                    KillTimer(container, kAutoHideTimer);
                    HideBar();
                    SetCaptionShown(container, true);
                } else if (g_captionShown.load()) {
                    SetCaptionShown(container, false);
                }
                FitAppToContainer(app, container);
            }
        }
    }
}

// Runs BEFORE Windhawk removes the hooks: everything that needs the live
// trampolines (releasing enclosures) must happen here, not in Wh_ModUninit.
void Wh_ModBeforeUninit() {
    g_teardown = true;

    // Let any in-flight auto-enclose settle thread notice teardown and exit
    // before the DLL unloads (max ~2s; the thread sleeps 800ms then returns).
    for (int i = 0; i < 40 && g_pendingSettle.load() > 0; i++) {
        Sleep(50);
    }

    StopHotkeyThread();

    if (!g_isTarget) {
        return;
    }

    // Release every enclosure from its own thread (DestroyWindow rule), with
    // escalating patience.
    for (int attempt = 0; attempt < 3; attempt++) {
        std::vector<HWND> containers;
        {
            std::lock_guard<std::mutex> lock(g_mutex);
            for (const auto& [container, app] : g_containerToApp) {
                containers.push_back(container);
            }
        }
        if (containers.empty()) {
            return;
        }
        for (HWND container : containers) {
            SendMessageTimeoutW(container, g_toggleMsg, 0, 0, SMTO_ABORTIFHUNG,
                                2000 * (attempt + 1), nullptr);
        }
    }

    // Survivors mean a hung app thread. We cannot destroy their windows
    // cross-thread, but we CAN make them safe: restore the app, sever the
    // bookkeeping, point the container at DefWindowProc (a stable user32
    // address) and hide it - so nothing references this DLL once it unloads.
    std::vector<std::pair<HWND, Enclosed>> leftovers;
    {
        std::lock_guard<std::mutex> lock(g_mutex);
        for (const auto& [app, info] : g_byApp) {
            leftovers.push_back({app, info});
        }
        g_byApp.clear();
        g_containerToApp.clear();
        g_activeApp = nullptr;
        g_anyEnclosed = false;
    }
    for (const auto& [app, info] : leftovers) {
        Wh_Log(L"Forced cleanup of enclosure %p (app thread unresponsive)",
               info.container);
        if (IsWindow(app)) {
            SetWindowLongPtrW(app, GWLP_HWNDPARENT, info.savedOwner);
            SelfSetWindowPos(app, nullptr, info.savedRect.left,
                             info.savedRect.top,
                             info.savedRect.right - info.savedRect.left,
                             info.savedRect.bottom - info.savedRect.top,
                             SWP_NOZORDER | SWP_NOACTIVATE);
        }
        if (IsWindow(info.container)) {
            SetWindowLongPtrW(info.container, GWLP_WNDPROC,
                              (LONG_PTR)DefWindowProcW);
            SelfShowWindow(info.container, SW_HIDE);
        }
    }
    HWND bar = g_bar.exchange(nullptr);
    if (bar && IsWindow(bar)) {
        SetWindowLongPtrW(bar, GWLP_WNDPROC, (LONG_PTR)DefWindowProcW);
        SelfShowWindow(bar, SW_HIDE);
    }

    ClipCursor(nullptr);  // never unload with a cursor clip in place
}

// Runs AFTER the hooks are removed: only plain imports from here on.
void Wh_ModUninit() {
    g_hooksRemoved = true;
    Wh_Log(L"Uninit");

    if (!g_isTarget) {
        return;
    }

    // Drop our menu items / subclasses from this process's windows.
    std::vector<HWND> topLevel;
    EnumWindows(
        [](HWND hWnd, LPARAM lParam) WINAPI -> BOOL {
            DWORD pid = 0;
            GetWindowThreadProcessId(hWnd, &pid);
            if (pid == GetCurrentProcessId()) {
                reinterpret_cast<std::vector<HWND>*>(lParam)->push_back(hWnd);
            }
            return TRUE;
        },
        reinterpret_cast<LPARAM>(&topLevel));
    for (HWND hWnd : topLevel) {
        HMENU hMenu = GetSystemMenu(hWnd, FALSE);
        if (hMenu && GetMenuState(hMenu, IDM_ENCLOSE, MF_BYCOMMAND) != (UINT)-1) {
            DeleteMenu(hMenu, IDM_ENCLOSE, MF_BYCOMMAND);
        }
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(hWnd, AppSubclassProc);
    }

    // Classes registered by a DLL are NOT auto-unregistered at unload; a stale
    // registration would carry a dangling wndproc into the next load.
    UnregisterClassW(kContainerClass, g_hInst);
    UnregisterClassW(kBarClass, g_hInst);
}
