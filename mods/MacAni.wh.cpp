// ==WindhawkMod==
// @id              macani
// @name            MacAni
// @description     macOS-style window motion for Windows: genie and scale minimize, pop launch, scale-and-fade close, slow-motion with Shift/Ctrl, and a GPU genie engine.
// @version        1.0.0
// @author          Charles-28X
// @github          https://github.com/Charles-28X
// @include         *
// @compilerOptions -ldwmapi -lgdi32 -ld2d1 -lole32 -loleaut32 -luuid -lshell32
// @license         MIT
// ==/WindhawkMod==

// ============================================================================
//  Author profile : https://github.com/Charles-28X
//  Related repo   : https://github.com/Charles-28X/macos-window-animations
// ============================================================================

// ==WindhawkModReadme==
/*
# macOS Genie & Pop Animations

The macOS window motion you miss on Windows: the **genie** that pours a window into its taskbar button, the **pop** when an app opens, and the quiet **scale-and-fade** when it closes. Every effect has its own switch, so you can use all of it or just the part you like.

## What you get

**Minimize and restore**
- **Genie**: the real macOS "lamp" warp. The window necks down and flows into the exact taskbar button of that window, found through UI Automation and AppUserModelID matching. If you click the button itself, the mod follows your cursor.
- **Scale**: the second macOS minimize style, where the window shrinks straight onto its button.
- **Dock depth**: set how far the genie tip dives into the taskbar.

**Open and close**
- **Pop launch**: windows scale up and fade in when an app opens. Dialogs get a springier pop, like macOS alert panels.
- **Scale-and-fade close**: a slight shrink and fade out. You can switch either one to a genie instead (closing flows to the top, bottom, left or right edge of the screen).
- **Tray apps**: optionally animate apps that hide instead of closing.

**Polish**
- A soft shadow under windows while they pop or scale, so they look like real windows instead of flat images.
- Windows 11 rounded corners are baked into the snapshot, so they warp with the window.

## Feel

- **Slow motion**: hold **Shift** (much slower), **Ctrl** (slow), or **Ctrl+Shift** (slowest) while minimizing or restoring, just like on macOS.
- **Easing**: cubic-bezier presets (macOS, ease in-out, ease out, ease in, linear), chosen separately for minimize and restore. Pop can use a spring with a small overshoot.
- **Smooth timing**: progress follows real elapsed time, and frames are paced at twice the display refresh rate.

## Pick an engine

| Engine | Best for | Notes |
|---|---|---|
| **Modern** (default) | Most systems | Direct2D mesh warp. Tile count sets the curve smoothness. |
| **GPU** (experimental) | High-refresh displays | The same warp on the graphics card via Direct3D 11 and DirectComposition. Vsync-locked, anti-aliased, mip-mapped. Uses more memory while animating, and falls back to Modern automatically if anything is missing. |
| **Classic** | Low-end machines | Lightweight strip-based GDI warp. |

## Works with your setup

- **Multi-monitor**: the genie flows into the taskbar of the monitor the window is on, or the primary monitor's taskbar if that screen has none.
- **Auto-hide taskbar**: the taskbar is briefly revealed so the genie has something to land on, then slides away.
- **System settings**: respects Windows' "Animation effects" toggle and skips remote desktop sessions.

## Light on resources

- Direct2D is created only when the first animation plays, so processes that never animate a window pay nothing.
- Snapshots of minimized windows live in a size-capped LRU cache (default 64 MB). Set it to 0 to re-capture on every restore.
- Worker threads are joined on unload, so disabling the mod is safe.

## Tips

- If an app misbehaves (for example `WindowsTerminal.exe`), add it to **Excluded programs**.
- Use **Excluded programs** instead of Windhawk's own process list. Windhawk's list only stops the mod from loading into a process, but a window's animation is often triggered from another one, such as Explorer. This setting matches the process that owns the window.
- Want a slower, more dramatic genie? Raise the duration to 700-900 ms, or just hold Shift.
- For the full Dock feel, pair it with a dock-style taskbar mod such as *Taskbar Dock Animation*.

## Good to know

- Translucent (Mica / acrylic) windows are captured as a static image, so the live blur can't animate. Transparent regions stay transparent.
- On some secondary monitors Windows won't hand over the backdrop, and the mod falls back to an opaque backdrop for the animation.
- A few apps hide or restore windows in unusual ways. If you see a glitch, exclude the app and open an issue with its name.

## Credits

Built on the Direct2D genie engine, UI Automation taskbar targeting and auto-hide handling by **Potassiumuncher**, and on **Abdullah Masood**'s *MacOS Minimize Animation* mod. Everything here is MIT licensed.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- duration_ms: 450
  $name: Genie duration (ms)
  $description: How long the genie minimize / restore lasts. Clamped to 50-2000.
- minimize_easing: macos
  $name: Minimize easing
  $description: Timing curve of the genie when a window goes into the taskbar.
  $options:
  - macos: macOS (slow start, fast middle, soft landing)
  - ease_in_out: Ease in-out (symmetrical)
  - ease_out: Ease out (fast start, long settle)
  - ease_in: Ease in (slow start, fast finish)
  - linear: Linear
- restore_easing: macos
  $name: Restore easing
  $description: Timing curve of the genie when a window comes back out of the taskbar.
  $options:
  - macos: macOS (slow start, fast middle, soft landing)
  - ease_in_out: Ease in-out (symmetrical)
  - ease_out: Ease out (fast start, long settle)
  - ease_in: Ease in (slow start, fast finish)
  - linear: Linear
- minimize_effect: genie
  $name: Minimize effect
  $description: Genie pours the window into the taskbar button. Scale shrinks it straight onto the button (macOS "Scale effect").
  $options:
  - genie: Genie
  - scale: Scale
- slow_motion_shift: true
  $name: Slow-motion with Shift / Ctrl
  $description: Hold Shift (slower) or Ctrl (slow) while minimizing or restoring - Ctrl+Shift is the slowest. Just like macOS.
- slow_motion_factor: 5
  $name: Slow-motion factor (Shift)
  $description: How many times slower the animation plays with Shift held. Ctrl uses about half, Ctrl+Shift double. Clamped to 2-12.
- open_animation: true
  $name: Animate window restore (open)
  $description: Play the reverse genie when a window is restored from the taskbar.
- launch_animation: true
  $name: Animate app launch
  $description: Animate an application window when it first opens.
- launch_style: pop
  $name: Launch style
  $description: Pop is the macOS look (scale up + fade in). Genie flows the window out of the taskbar.
  $options:
  - pop: Pop (scale + fade, like macOS)
  - genie: Genie (from the taskbar)
- close_animation: true
  $name: Animate window close
  $description: Animate a window when it closes.
- close_style: pop
  $name: Close style
  $description: Pop is the macOS look (slight shrink + fade out). Genie flows the window to a screen edge.
  $options:
  - pop: Pop (scale + fade, like macOS)
  - genie: Genie (to a screen edge)
- close_direction: up
  $name: Close genie direction
  $description: Only used when the Close style is Genie. Where a closing window flows to on its own screen.
  $options:
  - up: Up (top center)
  - down: Down (bottom center)
  - left: Left (center)
  - right: Right (center)
- hide_as_close: false
  $name: Animate apps that hide to the tray on close
  $description: Some apps hide instead of closing. Treat that as a close animation.
- pop_open_duration_ms: 240
  $name: Pop open duration (ms)
  $description: Duration of the launch pop. Clamped to 60-1500.
- pop_close_duration_ms: 180
  $name: Pop close duration (ms)
  $description: Duration of the close pop. Clamped to 60-1500.
- pop_scale: 92
  $name: Pop start/end scale (%)
  $description: Size the window starts from when opening and shrinks to when closing. Clamped to 50-100.
- pop_easing: ease_out
  $name: Pop easing
  $description: Timing curve of the launch pop. Spring adds a tiny overshoot.
  $options:
  - ease_out: Ease out (soft settle)
  - spring: Spring (slight overshoot)
  - ease_in_out: Ease in-out
  - macos: macOS curve
- alert_pop: true
  $name: Springy pop for dialogs
  $description: Dialog / alert windows pop in with a spring overshoot, like macOS alert panels.
- window_shadow: true
  $name: Soft window shadow during pop / scale
  $description: Draw a soft shadow under the animated window so it matches real windows instead of looking flat.
- round_corners: true
  $name: Round window corners (Windows 11)
  $description: Bake Windows 11 style rounded corners into the animated snapshot so they warp with the window.
- dock_depth_percent: 30
  $name: Genie dock depth (%)
  $description: How far the genie tip dives into the taskbar, as a percentage of the taskbar height. Clamped to 0-90.
- respect_reduce_motion: true
  $name: Respect Windows animation settings
  $description: Skip all animations when "Animation effects" is off in Windows, and inside remote desktop sessions.
- snapshot_cache_mb: 64
  $name: Restore snapshot cache (MB)
  $description: Memory budget for remembered minimized-window snapshots (least recently used are dropped first). 0 disables the cache and re-captures on restore. Clamped to 0-512.
- multi_monitor: true
  $name: Multi-monitor support
  $description: Flow into the taskbar of the monitor the window is on (falls back to the primary monitor's taskbar when that screen has none).
- unhide_taskbar: true
  $name: Unhide taskbar (for taskbar auto-hide)
  $description: Briefly reveal the taskbar during minimize if it is set to auto-hide.
- unhide_duration_ms: 450
  $name: Taskbar unhide duration (ms)
  $description: How long to keep the taskbar revealed before minimizing.
- engine: modern
  $name: Genie engine
  $description: Modern is the Direct2D mesh warp. GPU renders the exact same warp on the graphics card with vsync-locked frames and anti-aliased edges (uses more memory while animating). Classic is the strip-based GDI warp.
  $options:
  - modern: Modern genie (Direct2D engine)
  - gpu: GPU genie (Direct3D 11, experimental - falls back to Modern)
  - classic: Classic genie (original)
- tile_count: 35
  $name: Genie tile count (modern engine)
  $description: Mesh resolution of the modern genie (higher = smoother curve, more CPU).
- excluded_programs: [""]
  $name: Excluded programs
  $description: Programs whose windows should never animate (e.g. WindowsTerminal.exe).
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <dwmapi.h>
#include <d2d1.h>
#include <math.h>
#include <atomic>
#include <unordered_map>
#include <unordered_set>
#include <mutex>
#include <vector>
#include <string>
#include <algorithm>
#include <cwctype>
#include <uiautomation.h>
#include <shellapi.h>
#include <objbase.h>
#include <oleauto.h>
#include <propsys.h>
#include <d3d11.h>
#include <dxgi1_2.h>
#include <dcomp.h>
#include <d3dcommon.h>
#include <propidl.h>

// Fallback definitions for MinGW compatibility
#ifndef DWMWA_EXTENDED_FRAME_BOUNDS
#define DWMWA_EXTENDED_FRAME_BOUNDS 9
#endif
#ifndef DWMWA_CLOAK
#define DWMWA_CLOAK 13
#endif
#ifndef DWMWA_CLOAKED
#define DWMWA_CLOAKED 14
#endif
#ifndef DWMWA_TRANSITIONS_FORCEDISABLED
#define DWMWA_TRANSITIONS_FORCEDISABLED 3
#endif
#ifndef PW_RENDERFULLCONTENT
#define PW_RENDERFULLCONTENT 2
#endif
#ifndef CREATE_WAITABLE_TIMER_HIGH_RESOLUTION
#define CREATE_WAITABLE_TIMER_HIGH_RESOLUTION 0x00000002
#endif
#ifndef PROCESS_QUERY_LIMITED_INFORMATION
#define PROCESS_QUERY_LIMITED_INFORMATION 0x1000
#endif
#ifndef UIA_ButtonControlTypeId
#define UIA_ButtonControlTypeId 50000
#endif
#ifndef UIA_ListItemControlTypeId
#define UIA_ListItemControlTypeId 50007
#endif
#ifndef UIA_ControlTypePropertyId
#define UIA_ControlTypePropertyId 30003
#endif
#ifndef TreeScope_Descendants
#define TreeScope_Descendants 4
#endif
#ifndef UIA_NamePropertyId
#define UIA_NamePropertyId 30005
#endif
#ifndef UIA_AutomationIdPropertyId
#define UIA_AutomationIdPropertyId 30011
#endif
#ifndef UIA_BoundingRectanglePropertyId
#define UIA_BoundingRectanglePropertyId 30001
#endif
#ifndef SPI_GETCLIENTAREAANIMATION
#define SPI_GETCLIENTAREAANIMATION 0x1042
#endif
#ifndef DWMWA_WINDOW_CORNER_PREFERENCE
#define DWMWA_WINDOW_CORNER_PREFERENCE 33
#endif

// Explicit GUIDs to avoid __uuidof MinGW compile errors
static const GUID CLSID_CUIAutomation8_Guid = {0xe22ad333, 0xb25f, 0x460c, {0x83, 0xd0, 0x05, 0x81, 0x10, 0x73, 0x95, 0xc9}};
static const GUID CLSID_CUIAutomation_Guid = {0xff48dba4, 0x60ef, 0x4201, {0xaa, 0x87, 0x54, 0x10, 0x3e, 0xef, 0x59, 0x4e}};
static const GUID IID_IUIAutomation_Guid = {0x30cbe57d, 0xd9d0, 0x452a, {0xab, 0x13, 0x7a, 0xc5, 0xac, 0x48, 0x25, 0xee}};
static const GUID IID_ID2D1Factory_Guid = {0x06152247, 0x6f50, 0x465a, {0x92, 0x45, 0x11, 0x8b, 0xfd, 0x3b, 0x60, 0x07}};

#define PI 3.14159265f
#define TILE_COUNT_MIN 8
#define TILE_COUNT_MAX 96
#define TILE_COUNT_DEFAULT 35
#define COMPAT_TASKBAR_HEIGHT 70
#define COMPAT_ICON_SIZE      33
#define MACGENIE_ASYNC_HIDE (WM_APP + 0x4A1)

struct Geometry { float x, y, width, height; };

typedef LRESULT (WINAPI *DefWindowProcW_t)(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
DefWindowProcW_t DefWindowProcW_Original;

typedef BOOL (WINAPI *DestroyWindow_t)(HWND hWnd);
DestroyWindow_t DestroyWindow_Original;

typedef BOOL (WINAPI *ShowWindow_t)(HWND hWnd, int nCmdShow);
ShowWindow_t ShowWindow_Original;

typedef BOOL (WINAPI *ShowWindowAsync_t)(HWND hWnd, int nCmdShow);
ShowWindowAsync_t ShowWindowAsync_Original;

typedef BOOL (WINAPI *SetWindowPlacement_t)(HWND hWnd, const WINDOWPLACEMENT* lpwndpl);
SetWindowPlacement_t SetWindowPlacement_Original;

typedef BOOL (WINAPI *CloseWindow_t)(HWND hWnd);
CloseWindow_t CloseWindow_Original;

typedef BOOL (WINAPI *SetWindowPos_t)(HWND hWnd, HWND hWndInsertAfter, int X, int Y,
                                      int cx, int cy, UINT uFlags);
SetWindowPos_t SetWindowPos_Original;

ID2D1Factory* g_d2dFactory = nullptr;
static std::mutex g_D2DMutex;

// The factory is created on first use, not at load: this mod is injected into every
// process, and most of them never animate a window, so they never pay for Direct2D.
static bool MacGenieEnsureD2D() {
    std::lock_guard<std::mutex> lock(g_D2DMutex);
    if (g_d2dFactory) return true;
    ID2D1Factory* f = nullptr;
    if (SUCCEEDED(D2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED, IID_ID2D1Factory_Guid,
                                    reinterpret_cast<void**>(&f))) && f) {
        g_d2dFactory = f;
    }
    return g_d2dFactory != nullptr;
}

struct MacGenieAnimData {
    HWND hRealWnd;
    HBITMAP hBitmap;
    void* pBits;
    RECT targetRect;
    HMONITOR hMon;
    int width;
    int height;
    int targetDockX;
    BOOL isRising;
    LONG_PTR originalExStyle;
    BOOL hiddenByCloak;
    HANDLE hFirstFrameShown;
    int durationMs;
    BOOL requestedUnhide;
    HWND hNextApp;
    int  unhideDurationMs;
    BOOL deferredMinimize;
    BOOL isClosing;
    UINT closeMsg;
    int  style;          // 0 = genie (mesh warp), 1 = pop (scale + fade), 2 = scale-to-dock
    float popScale;      // pop style: start/end scale (0.5 - 1.0)
    int  popEasing;      // easing preset for the pop's scale
    float cornerRadius;  // real window corner radius in px (0 = square)
    float dpiScale;      // window DPI / 96
    BOOL shadow;         // draw a synthesized soft shadow under the ghost
};

struct MacGenieLaunchData {
    HWND     hWnd;
    LONG_PTR originalExStyle;
    BOOL     hiddenByCloak;
};

struct SnapCache { HBITMAP hBmp; void* pBits; int w; int h; ULONGLONG tick; };

std::unordered_map<HWND, SnapCache> g_SnapshotCache;
std::unordered_map<HWND, int> g_IconPositions;
std::unordered_map<std::wstring, int> g_ProcessIconPositions;
std::unordered_set<HWND> g_LaunchSeen;
std::unordered_set<HWND> g_AnimActive;
std::mutex g_CacheMutex;

std::atomic<int> g_durationMs{450};
std::atomic<bool> g_openAnimation{true};
std::atomic<bool> g_launchAnimation{true};
std::atomic<bool> g_multiMonitor{true};
std::atomic<int>  g_unhideDurationMs{450};
std::atomic<bool> g_unhideEnabled{true};
std::atomic<bool> g_classicEngine{false};
std::atomic<bool> g_gpuEngine{false};
std::atomic<bool> g_gpuBroken{false};   // GPU init failed once in this process: stop retrying
std::atomic<int> g_tileCount{TILE_COUNT_DEFAULT};
std::atomic<bool> g_closeAnimation{true};
std::atomic<bool> g_hideAsClose{false};
std::atomic<int>  g_closeDirection{2};
std::atomic<int>  g_easeMinimize{0};      // index into kMacGenieEasings
std::atomic<int>  g_easeRestore{0};
std::atomic<bool> g_slowMoShift{true};
std::atomic<int>  g_slowMoFactor{5};
std::atomic<int>  g_launchStyle{1};       // 0 = genie, 1 = pop
std::atomic<int>  g_closeStyle{1};        // 0 = genie, 1 = pop
std::atomic<int>  g_popOpenMs{240};
std::atomic<int>  g_popCloseMs{180};
std::atomic<int>  g_popScalePct{92};
std::atomic<bool> g_roundCorners{true};
std::atomic<int>  g_dockDepthPct{30};
std::atomic<int>  g_minimizeEffect{0};    // 0 = genie, 2 = scale (style ids)
std::atomic<int>  g_popEasing{2};
std::atomic<bool> g_alertPop{true};
std::atomic<bool> g_windowShadow{true};
std::atomic<bool> g_respectReduceMotion{true};
std::atomic<int>  g_cacheBudgetMb{64};
std::vector<std::wstring> g_ExcludedPrograms;

std::atomic<bool> g_unloading{false};
std::mutex g_WorkerThreadsMutex;
std::vector<HANDLE> g_WorkerThreads;

static bool MacGenieSpawnWorker(LPTHREAD_START_ROUTINE proc, void* param) {
    std::lock_guard<std::mutex> lock(g_WorkerThreadsMutex);
    if (g_unloading.load(std::memory_order_relaxed)) return false;
    for (size_t i = g_WorkerThreads.size(); i-- > 0;) {
        if (WaitForSingleObject(g_WorkerThreads[i], 0) == WAIT_OBJECT_0) {
            CloseHandle(g_WorkerThreads[i]);
            g_WorkerThreads.erase(g_WorkerThreads.begin() + i);
        }
    }
    HANDLE hThread = CreateThread(NULL, 0, proc, param, 0, NULL);
    if (!hThread) return false;
    g_WorkerThreads.push_back(hThread);
    return true;
}

// Cubic-bezier easing presets: x1, y1, x2, y2 (CSS-style control points).
struct MacGenieBezier { float x1, y1, x2, y2; };
static const MacGenieBezier kMacGenieEasings[] = {
    { 0.52f, 0.00f, 0.30f, 1.00f },   // 0 macos        - slow start, fast middle, soft landing
    { 0.65f, 0.00f, 0.35f, 1.00f },   // 1 ease_in_out
    { 0.22f, 1.00f, 0.36f, 1.00f },   // 2 ease_out
    { 0.55f, 0.00f, 0.85f, 0.40f },   // 3 ease_in
    { 0.00f, 0.00f, 1.00f, 1.00f },   // 4 linear
    { 0.30f, 1.80f, 0.50f, 1.00f },   // 5 spring (overshoots 1.0, pop only)
};

static int MacGenieParseEasing(PCWSTR v) {
    if (!wcscmp(v, L"ease_in_out")) return 1;
    if (!wcscmp(v, L"ease_out")) return 2;
    if (!wcscmp(v, L"ease_in")) return 3;
    if (!wcscmp(v, L"linear")) return 4;
    if (!wcscmp(v, L"spring")) return 5;
    return 0;
}

// Evaluate y(x) of a CSS cubic-bezier (Newton-Raphson with a bisection fallback).
static float MacGenieEase(int preset, float x) {
    if (x <= 0.0f) return 0.0f;
    if (x >= 1.0f) return 1.0f;
    if (preset < 0 || preset > 5) preset = 0;
    const MacGenieBezier& b = kMacGenieEasings[preset];
    const float cx = 3.0f * b.x1, bx = 3.0f * (b.x2 - b.x1) - cx, ax = 1.0f - cx - bx;
    const float cy = 3.0f * b.y1, by = 3.0f * (b.y2 - b.y1) - cy, ay = 1.0f - cy - by;
    float t = x;
    for (int i = 0; i < 8; ++i) {
        float err = ((ax * t + bx) * t + cx) * t - x;
        if (fabsf(err) < 1e-5f) return ((ay * t + by) * t + cy) * t;
        float d = (3.0f * ax * t + 2.0f * bx) * t + cx;
        if (fabsf(d) < 1e-6f) break;
        t -= err / d;
    }
    float lo = 0.0f, hi = 1.0f;
    t = x;
    for (int i = 0; i < 24; ++i) {
        float v = ((ax * t + bx) * t + cx) * t;
        if (fabsf(v - x) < 1e-5f) break;
        if (v < x) lo = t; else hi = t;
        t = (lo + hi) * 0.5f;
    }
    return ((ay * t + by) * t + cy) * t;
}

void MacGenieLoadSettings() {
    int ms = Wh_GetIntSetting(L"duration_ms");
    if (ms < 50) ms = 50;
    if (ms > 2000) ms = 2000;
    g_durationMs.store(ms, std::memory_order_relaxed);
    g_openAnimation.store(Wh_GetIntSetting(L"open_animation") != 0, std::memory_order_relaxed);
    g_launchAnimation.store(Wh_GetIntSetting(L"launch_animation") != 0, std::memory_order_relaxed);
    g_multiMonitor.store(Wh_GetIntSetting(L"multi_monitor") != 0, std::memory_order_relaxed);

    int unhide_ms = Wh_GetIntSetting(L"unhide_duration_ms");
    if (unhide_ms < 0) unhide_ms = 0;
    if (unhide_ms > 5000) unhide_ms = 5000;
    g_unhideDurationMs.store(unhide_ms, std::memory_order_relaxed);
    g_unhideEnabled.store(Wh_GetIntSetting(L"unhide_taskbar") != 0, std::memory_order_relaxed);

    PCWSTR engine = Wh_GetStringSetting(L"engine");
    g_classicEngine.store(wcscmp(engine, L"classic") == 0, std::memory_order_relaxed);
    g_gpuEngine.store(wcscmp(engine, L"gpu") == 0, std::memory_order_relaxed);
    Wh_FreeStringSetting(engine);

    int tiles = Wh_GetIntSetting(L"tile_count");
    if (tiles < TILE_COUNT_MIN) tiles = TILE_COUNT_MIN;
    if (tiles > TILE_COUNT_MAX) tiles = TILE_COUNT_MAX;
    g_tileCount.store(tiles, std::memory_order_relaxed);

    g_closeAnimation.store(Wh_GetIntSetting(L"close_animation") != 0, std::memory_order_relaxed);
    g_hideAsClose.store(Wh_GetIntSetting(L"hide_as_close") != 0, std::memory_order_relaxed);
    PCWSTR direction = Wh_GetStringSetting(L"close_direction");
    g_closeDirection.store(wcscmp(direction, L"left") == 0  ? 0 :
                           wcscmp(direction, L"right") == 0 ? 1 :
                           wcscmp(direction, L"down") == 0  ? 3 : 2,
                           std::memory_order_relaxed);
    Wh_FreeStringSetting(direction);

    PCWSTR easeMin = Wh_GetStringSetting(L"minimize_easing");
    g_easeMinimize.store(MacGenieParseEasing(easeMin), std::memory_order_relaxed);
    Wh_FreeStringSetting(easeMin);
    PCWSTR easeRes = Wh_GetStringSetting(L"restore_easing");
    g_easeRestore.store(MacGenieParseEasing(easeRes), std::memory_order_relaxed);
    Wh_FreeStringSetting(easeRes);

    g_slowMoShift.store(Wh_GetIntSetting(L"slow_motion_shift") != 0, std::memory_order_relaxed);
    int slowFactor = Wh_GetIntSetting(L"slow_motion_factor");
    if (slowFactor < 2) slowFactor = 2;
    if (slowFactor > 12) slowFactor = 12;
    g_slowMoFactor.store(slowFactor, std::memory_order_relaxed);

    PCWSTR launchStyle = Wh_GetStringSetting(L"launch_style");
    g_launchStyle.store(wcscmp(launchStyle, L"genie") == 0 ? 0 : 1, std::memory_order_relaxed);
    Wh_FreeStringSetting(launchStyle);
    PCWSTR closeStyle = Wh_GetStringSetting(L"close_style");
    g_closeStyle.store(wcscmp(closeStyle, L"genie") == 0 ? 0 : 1, std::memory_order_relaxed);
    Wh_FreeStringSetting(closeStyle);

    int popOpen = Wh_GetIntSetting(L"pop_open_duration_ms");
    if (popOpen < 60) popOpen = 60;
    if (popOpen > 1500) popOpen = 1500;
    g_popOpenMs.store(popOpen, std::memory_order_relaxed);
    int popClose = Wh_GetIntSetting(L"pop_close_duration_ms");
    if (popClose < 60) popClose = 60;
    if (popClose > 1500) popClose = 1500;
    g_popCloseMs.store(popClose, std::memory_order_relaxed);
    int popScale = Wh_GetIntSetting(L"pop_scale");
    if (popScale < 50) popScale = 50;
    if (popScale > 100) popScale = 100;
    g_popScalePct.store(popScale, std::memory_order_relaxed);

    g_roundCorners.store(Wh_GetIntSetting(L"round_corners") != 0, std::memory_order_relaxed);
    int depth = Wh_GetIntSetting(L"dock_depth_percent");
    if (depth < 0) depth = 0;
    if (depth > 90) depth = 90;
    g_dockDepthPct.store(depth, std::memory_order_relaxed);

    PCWSTR effect = Wh_GetStringSetting(L"minimize_effect");
    g_minimizeEffect.store(wcscmp(effect, L"scale") == 0 ? 2 : 0, std::memory_order_relaxed);
    Wh_FreeStringSetting(effect);
    PCWSTR popEase = Wh_GetStringSetting(L"pop_easing");
    g_popEasing.store(MacGenieParseEasing(popEase), std::memory_order_relaxed);
    Wh_FreeStringSetting(popEase);
    g_alertPop.store(Wh_GetIntSetting(L"alert_pop") != 0, std::memory_order_relaxed);
    g_windowShadow.store(Wh_GetIntSetting(L"window_shadow") != 0, std::memory_order_relaxed);
    g_respectReduceMotion.store(Wh_GetIntSetting(L"respect_reduce_motion") != 0, std::memory_order_relaxed);
    int cacheMb = Wh_GetIntSetting(L"snapshot_cache_mb");
    if (cacheMb < 0) cacheMb = 0;
    if (cacheMb > 512) cacheMb = 512;
    g_cacheBudgetMb.store(cacheMb, std::memory_order_relaxed);

    std::vector<std::wstring> excluded;
    for (int i = 0;; i++) {
        PCWSTR prog = Wh_GetStringSetting(L"excluded_programs[%d]", i);
        bool has = *prog;
        if (has) {
            std::wstring s = prog;
            std::transform(s.begin(), s.end(), s.begin(), ::towlower);
            excluded.push_back(std::move(s));
        }
        Wh_FreeStringSetting(prog);
        if (!has) break;
    }
    {
        std::lock_guard<std::mutex> lock(g_CacheMutex);
        g_ExcludedPrograms.swap(excluded);
    }
}

bool MacGenieIsExcluded(HWND hWnd) {
    {
        std::lock_guard<std::mutex> lock(g_CacheMutex);
        if (g_ExcludedPrograms.empty()) return false;
    }
    DWORD pid = 0;
    GetWindowThreadProcessId(hWnd, &pid);
    if (!pid) return false;
    HANDLE hProc = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!hProc) return false;
    WCHAR path[MAX_PATH];
    DWORD len = MAX_PATH;
    bool ok = QueryFullProcessImageNameW(hProc, 0, path, &len) != 0;
    CloseHandle(hProc);
    if (!ok) return false;

    std::wstring full = path;
    std::transform(full.begin(), full.end(), full.begin(), ::towlower);
    size_t slash = full.find_last_of(L"\\/");
    std::wstring base = (slash == std::wstring::npos) ? full : full.substr(slash + 1);
    std::wstring baseNoExt = base;
    if (baseNoExt.size() > 4 && baseNoExt.compare(baseNoExt.size() - 4, 4, L".exe") == 0) {
        baseNoExt.resize(baseNoExt.size() - 4);
    }

    std::lock_guard<std::mutex> lock(g_CacheMutex);
    for (const std::wstring& e : g_ExcludedPrograms) {
        if (e == full || e == base || e == baseNoExt) return true;
    }
    return false;
}

// Windows "Animation effects" off, or a remote desktop session: do not animate.
static bool MacGenieMotionAllowed() {
    if (!g_respectReduceMotion.load(std::memory_order_relaxed)) return true;
    if (GetSystemMetrics(SM_REMOTESESSION)) return false;
    BOOL on = TRUE;
    if (SystemParametersInfoW(SPI_GETCLIENTAREAANIMATION, 0, &on, 0) && !on) return false;
    return true;
}

void MacGenieSetDwmTransitions(HWND hWnd, BOOL enable) {
    BOOL disable = !enable;
    DwmSetWindowAttribute(hWnd, DWMWA_TRANSITIONS_FORCEDISABLED, &disable, sizeof(disable));
}

void MacGenieSetCloak(HWND hWnd, BOOL cloak) {
    DwmSetWindowAttribute(hWnd, DWMWA_CLOAK, &cloak, sizeof(cloak));
}

static void MacGenieUndoRisingHide(HWND hWnd, LONG_PTR originalExStyle, BOOL cloakHidden) {
    if (cloakHidden) {
        MacGenieSetCloak(hWnd, FALSE);
    } else {
        SetLayeredWindowAttributes(hWnd, 0, 255, LWA_ALPHA);
        if (!(originalExStyle & WS_EX_LAYERED)) {
            SetWindowLongPtrW(hWnd, GWL_EXSTYLE, originalExStyle);
        }
    }
    MacGenieSetDwmTransitions(hWnd, TRUE);
}

static bool MacGenieShouldAnimate(HWND hWnd) {
    if (!hWnd) return false;
    LONG_PTR style = GetWindowLongPtrW(hWnd, GWL_STYLE);
    if (style & WS_CHILD) return false;

    RECT r;
    if (IsIconic(hWnd)) {
        WINDOWPLACEMENT wp;
        wp.length = sizeof(wp);
        if (!GetWindowPlacement(hWnd, &wp)) return false;
        r = wp.rcNormalPosition;
    } else if (!GetWindowRect(hWnd, &r)) {
        return false;
    }
    if ((r.right - r.left) < 40 || (r.bottom - r.top) < 40) return false;
    return true;
}

static bool MacGenieIsLaunchWindow(HWND hWnd) {
    if (!hWnd) return false;
    if (GetAncestor(hWnd, GA_ROOT) != hWnd) return false;
    LONG_PTR style = GetWindowLongPtrW(hWnd, GWL_STYLE);
    if (!(style & WS_CAPTION)) return false;
    LONG_PTR exStyle = GetWindowLongPtrW(hWnd, GWL_EXSTYLE);
    if (exStyle & WS_EX_TOOLWINDOW) return false;
    return MacGenieShouldAnimate(hWnd);
}

static ID2D1PathGeometry* CreateQuadGeo(
    ID2D1Factory* factory,
    D2D1_POINT_2F p0, D2D1_POINT_2F p1,
    D2D1_POINT_2F p2, D2D1_POINT_2F p3)
{
    ID2D1PathGeometry* geo = nullptr;
    factory->CreatePathGeometry(&geo);
    if (!geo) return nullptr;
    ID2D1GeometrySink* sink = nullptr;
    geo->Open(&sink);
    sink->BeginFigure(p0, D2D1_FIGURE_BEGIN_FILLED);
    sink->AddLine(p1);
    sink->AddLine(p2);
    sink->AddLine(p3);
    sink->EndFigure(D2D1_FIGURE_END_CLOSED);
    sink->Close();
    sink->Release();
    return geo;
}

static D2D1_POINT_2F BloatPoint(D2D1_POINT_2F p, D2D1_POINT_2F c, float amount = 0.5f) {
    float dx = p.x - c.x; float dy = p.y - c.y;
    float len = sqrtf(dx*dx + dy*dy);
    if (len < 0.001f) return p;
    return D2D1::Point2F(p.x + (dx/len)*amount, p.y + (dy/len)*amount);
}

HWND FindTaskbarForMonitor(HMONITOR hMon) {
    HWND hMainTray = FindWindowW(L"Shell_TrayWnd", NULL);
    HMONITOR mainMon = MonitorFromWindow(hMainTray, MONITOR_DEFAULTTOPRIMARY);
    if (hMon == mainMon || !hMon) return hMainTray;

    HWND hSecTray = NULL;
    while ((hSecTray = FindWindowExW(NULL, hSecTray, L"Shell_SecondaryTrayWnd", NULL)) != NULL) {
        if (MonitorFromWindow(hSecTray, MONITOR_DEFAULTTONULL) == hMon) {
            return hSecTray;
        }
    }
    return hMainTray;
}

// The taskbar that sits on exactly this monitor, or NULL (e.g. "show taskbar on all
// displays" is off, so a secondary monitor has none).
static HWND FindTaskbarOnMonitorExact(HMONITOR hMon) {
    if (!hMon) return NULL;
    HWND hMain = FindWindowW(L"Shell_TrayWnd", NULL);
    if (hMain && MonitorFromWindow(hMain, MONITOR_DEFAULTTOPRIMARY) == hMon) return hMain;
    HWND hSec = NULL;
    while ((hSec = FindWindowExW(NULL, hSec, L"Shell_SecondaryTrayWnd", NULL)) != NULL) {
        if (MonitorFromWindow(hSec, MONITOR_DEFAULTTONULL) == hMon) return hSec;
    }
    return NULL;
}

// Monitor whose taskbar the window flows into: its own monitor when that monitor has a
// taskbar, otherwise the primary one (never a taskbar that lives on a different screen
// than the animation canvas would assume).
static HMONITOR MacGenieResolveDockMonitor(HWND hWnd) {
    HMONITOR primary = MonitorFromPoint(POINT{0, 0}, MONITOR_DEFAULTTOPRIMARY);
    if (!g_multiMonitor.load(std::memory_order_relaxed)) return primary;
    HMONITOR winMon = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
    return FindTaskbarOnMonitorExact(winMon) ? winMon : primary;
}

// ---------------------------------------------------------------------------
// Taskbar button lookup (UI Automation)
//
// The old implementation made one cross-process call per property per button
// (dozens of round trips) on the UI thread of the app being minimized, which is
// what delayed the start of the genie. The enumeration now fetches Name,
// AutomationId and BoundingRectangle for every button in ONE cached request, and
// the result is reused for a short while, so back-to-back animations are instant.
// ---------------------------------------------------------------------------
struct MacGenieTaskbarButton {
    std::wstring nameLower;
    std::wstring idLower;     // AutomationId, e.g. "appid: chrome"
    RECT rect;
};

struct MacGenieButtonCache {
    ULONGLONG tick = 0;
    std::vector<MacGenieTaskbarButton> buttons;
};

static std::mutex g_UiaMutex;
static std::unordered_map<HWND, MacGenieButtonCache> g_ButtonCache;
static const ULONGLONG kButtonCacheTtlMs = 600;

static std::wstring MacGenieLower(const wchar_t* text) {
    std::wstring r = text ? text : L"";
    std::transform(r.begin(), r.end(), r.begin(), ::towlower);
    return r;
}

// Must be called on a COM-initialized thread.
static bool MacGenieEnumerateTaskbarButtons(HWND hTray, std::vector<MacGenieTaskbarButton>& out) {
    IUIAutomation* pAutomation = nullptr;
    HRESULT hr = CoCreateInstance(CLSID_CUIAutomation8_Guid, NULL, CLSCTX_INPROC_SERVER,
                                  IID_IUIAutomation_Guid, (void**)&pAutomation);
    if (FAILED(hr)) {
        hr = CoCreateInstance(CLSID_CUIAutomation_Guid, NULL, CLSCTX_INPROC_SERVER,
                              IID_IUIAutomation_Guid, (void**)&pAutomation);
    }
    if (FAILED(hr) || !pAutomation) return false;

    bool ok = false;
    IUIAutomationElement* pTray = nullptr;
    IUIAutomationCacheRequest* pCache = nullptr;
    IUIAutomationCondition* pButtonCond = nullptr;
    IUIAutomationCondition* pListCond = nullptr;
    IUIAutomationCondition* pOrCond = nullptr;
    IUIAutomationElementArray* pArray = nullptr;

    if (SUCCEEDED(pAutomation->ElementFromHandle(hTray, &pTray)) && pTray &&
        SUCCEEDED(pAutomation->CreateCacheRequest(&pCache)) && pCache) {
        pCache->AddProperty((PROPERTYID)UIA_NamePropertyId);
        pCache->AddProperty((PROPERTYID)UIA_AutomationIdPropertyId);
        pCache->AddProperty((PROPERTYID)UIA_BoundingRectanglePropertyId);
        pCache->put_AutomationElementMode(AutomationElementMode_None);

        VARIANT varBtn; VariantInit(&varBtn); varBtn.vt = VT_I4; varBtn.lVal = (int)UIA_ButtonControlTypeId;
        pAutomation->CreatePropertyCondition((PROPERTYID)UIA_ControlTypePropertyId, varBtn, &pButtonCond);
        VARIANT varList; VariantInit(&varList); varList.vt = VT_I4; varList.lVal = (int)UIA_ListItemControlTypeId;
        pAutomation->CreatePropertyCondition((PROPERTYID)UIA_ControlTypePropertyId, varList, &pListCond);
        if (pButtonCond && pListCond) pAutomation->CreateOrCondition(pButtonCond, pListCond, &pOrCond);

        if (pOrCond &&
            SUCCEEDED(pTray->FindAllBuildCache((TreeScope)TreeScope_Descendants, pOrCond, pCache, &pArray)) &&
            pArray) {
            int length = 0;
            pArray->get_Length(&length);
            for (int i = 0; i < length; i++) {
                IUIAutomationElement* pItem = nullptr;
                if (FAILED(pArray->GetElement(i, &pItem)) || !pItem) continue;
                MacGenieTaskbarButton btn;
                btn.rect = {0, 0, 0, 0};
                BSTR name = nullptr;
                BSTR id = nullptr;
                if (SUCCEEDED(pItem->get_CachedName(&name)) && name) btn.nameLower = MacGenieLower(name);
                if (SUCCEEDED(pItem->get_CachedAutomationId(&id)) && id) btn.idLower = MacGenieLower(id);
                if (name) SysFreeString(name);
                if (id) SysFreeString(id);
                RECT r;
                if (SUCCEEDED(pItem->get_CachedBoundingRectangle(&r)) && r.right > r.left) {
                    btn.rect = r;
                    out.push_back(std::move(btn));
                }
                pItem->Release();
            }
            ok = true;
        }
    }
    if (pArray) pArray->Release();
    if (pOrCond) pOrCond->Release();
    if (pListCond) pListCond->Release();
    if (pButtonCond) pButtonCond->Release();
    if (pCache) pCache->Release();
    if (pTray) pTray->Release();
    pAutomation->Release();
    return ok;
}

static bool MacGenieGetTaskbarButtons(HWND hTray, std::vector<MacGenieTaskbarButton>& out) {
    {
        std::lock_guard<std::mutex> lock(g_UiaMutex);
        auto it = g_ButtonCache.find(hTray);
        if (it != g_ButtonCache.end() && GetTickCount64() - it->second.tick < kButtonCacheTtlMs) {
            out = it->second.buttons;
            return true;
        }
    }
    std::vector<MacGenieTaskbarButton> fresh;
    if (!MacGenieEnumerateTaskbarButtons(hTray, fresh)) return false;
    {
        std::lock_guard<std::mutex> lock(g_UiaMutex);
        MacGenieButtonCache& c = g_ButtonCache[hTray];
        c.tick = GetTickCount64();
        c.buttons = fresh;
    }
    out = std::move(fresh);
    return true;
}

// The window's explicit AppUserModelID (what the taskbar groups by), lowercase.
// Empty when the app did not set one. Must be called on a COM-initialized thread.
static std::wstring MacGenieGetWindowAumidLower(HWND hWnd) {
    static const PROPERTYKEY kAppUserModelId = {
        {0x9F4C2855, 0x9F79, 0x4B39, {0xA8, 0xD0, 0xE1, 0xD4, 0x2D, 0xE1, 0xD5, 0xF3}}, 5};
    std::wstring result;
    IPropertyStore* ps = nullptr;
    if (SUCCEEDED(SHGetPropertyStoreForWindow(hWnd, IID_IPropertyStore, (void**)&ps)) && ps) {
        PROPVARIANT pv;
        PropVariantInit(&pv);
        if (SUCCEEDED(ps->GetValue(kAppUserModelId, &pv)) && pv.vt == VT_LPWSTR && pv.pwszVal) {
            result = MacGenieLower(pv.pwszVal);
        }
        PropVariantClear(&pv);
        ps->Release();
    }
    return result;
}

// Returns the horizontal centre of the taskbar button that belongs to hWndApp
// (falls back to fallbackX, then to the per-window / per-process learned caches).
int GetTaskbarButtonX(HWND hWndApp, int fallbackX, HMONITOR hMon) {
    int targetX = fallbackX;
    bool uiaFound = false;

    std::wstring procNameLower = L"";
    std::wstring exeFileLower = L"";
    DWORD ownerPid = 0;
    GetWindowThreadProcessId(hWndApp, &ownerPid);
    if (ownerPid) {
        HANDLE hProc = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, ownerPid);
        if (hProc) {
            WCHAR exePath[MAX_PATH] = {0};
            DWORD exePathLen = MAX_PATH;
            if (QueryFullProcessImageNameW(hProc, 0, exePath, &exePathLen)) {
                WCHAR* name = wcsrchr(exePath, L'\\');
                if (name) {
                    exeFileLower = MacGenieLower(name + 1);
                    procNameLower = exeFileLower;
                    size_t dotPos = procNameLower.find(L'.');
                    if (dotPos != std::wstring::npos) procNameLower = procNameLower.substr(0, dotPos);
                }
            }
            CloseHandle(hProc);
        }
    }

    std::wstring processKey = procNameLower;
    if (!processKey.empty() && hMon) {
        processKey += L"_" + std::to_wstring(reinterpret_cast<size_t>(hMon));
    }

    HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
    bool coInit = (hr == S_OK || hr == S_FALSE);

    if (hr == S_OK || hr == S_FALSE || hr == RPC_E_CHANGED_MODE) {
        HWND hTray = FindTaskbarForMonitor(hMon);
        std::vector<MacGenieTaskbarButton> buttons;
        if (hTray && MacGenieGetTaskbarButtons(hTray, buttons) && !buttons.empty()) {
            WCHAR titleW[512] = {0};
            GetWindowTextW(hWndApp, titleW, 512);
            std::wstring titleLower = MacGenieLower(titleW);
            std::wstring aumidLower = MacGenieGetWindowAumidLower(hWndApp);

            std::wstring procHintLower = procNameLower;
            if (procNameLower == L"chrome") procHintLower = L"google chrome";
            else if (procNameLower == L"msedge") procHintLower = L"microsoft edge";

            MONITORINFO mi = {0};
            mi.cbSize = sizeof(MONITORINFO);
            GetMonitorInfoW(hMon, &mi);
            const int monRight = mi.rcMonitor.right;

            // Window title split into words once (>= 4 chars) for the weak match.
            std::vector<std::wstring> titleWords;
            {
                std::wstring cur;
                for (wchar_t c : titleLower) {
                    if (iswalnum(c)) cur += c;
                    else { if (cur.length() >= 4) titleWords.push_back(cur); cur.clear(); }
                }
                if (cur.length() >= 4) titleWords.push_back(cur);
            }

            int bestScore = 0;
            for (const MacGenieTaskbarButton& btn : buttons) {
                if (btn.rect.left >= monRight - 50) continue;     // tray / clock area
                const std::wstring& nm = btn.nameLower;
                const std::wstring& id = btn.idLower;
                if (nm.empty() && id.empty()) continue;

                int score = 0;
                // Exact application identity: the taskbar's AutomationId carries the AppUserModelID.
                if (!aumidLower.empty() && id.find(aumidLower) != std::wstring::npos) score += 1500;
                else if (!exeFileLower.empty() && id.find(exeFileLower) != std::wstring::npos) score += 700;

                if (!nm.empty()) {
                    if (!titleLower.empty()) {
                        if (titleLower == nm) score += 1000;
                        if (titleLower.find(nm) != std::wstring::npos) score += 500;
                        if (nm.find(titleLower) != std::wstring::npos) score += 500;
                    }
                    if (!procNameLower.empty() && nm.find(procNameLower) != std::wstring::npos) score += 400;
                    if (!procHintLower.empty() && procHintLower != procNameLower &&
                        nm.find(procHintLower) != std::wstring::npos) score += 900;
                    for (const std::wstring& w : titleWords) {
                        if (nm.find(w) != std::wstring::npos) score += 50;
                    }
                    if (nm.find(L"start") != std::wstring::npos) score -= 500;
                    if (nm.find(L"search") != std::wstring::npos) score -= 500;
                    if (nm.find(L"task view") != std::wstring::npos) score -= 500;
                    if (nm.find(L"widgets") != std::wstring::npos) score -= 500;
                }

                if (score > bestScore) {
                    bestScore = score;
                    targetX = btn.rect.left + (btn.rect.right - btn.rect.left) / 2;
                }
            }
            // A match made of nothing but a stray title word is not trustworthy.
            if (bestScore >= 100) uiaFound = true; else targetX = fallbackX;
        }
        if (coInit) CoUninitialize();
    }

    std::lock_guard<std::mutex> lock(g_CacheMutex);
    if (uiaFound) {
        g_IconPositions[hWndApp] = targetX;
        if (!processKey.empty()) {
            if (g_ProcessIconPositions.size() > 256) g_ProcessIconPositions.clear();
            g_ProcessIconPositions[processKey] = targetX;
        }
    } else if (g_IconPositions.count(hWndApp)) {
        targetX = g_IconPositions[hWndApp];
    } else if (!processKey.empty() && g_ProcessIconPositions.count(processKey)) {
        targetX = g_ProcessIconPositions[processKey];
    }

    return targetX;
}

static void MacGenieFinishClose(MacGenieAnimData* data) {
    HWND hWnd = data->hRealWnd;
    if (!IsWindow(hWnd)) return;
    if (g_unloading.load(std::memory_order_relaxed)) {
        MacGenieSetCloak(hWnd, FALSE);
        MacGenieSetDwmTransitions(hWnd, TRUE);
        RemovePropW(hWnd, L"GenieCloseAnimating");
        RemovePropW(hWnd, L"GenieCloseBypass");
        if (data->closeMsg == MACGENIE_ASYNC_HIDE) {
            ShowWindowAsync_Original(hWnd, SW_HIDE);
        } else if (data->closeMsg == WM_SYSCOMMAND) {
            PostMessageW(hWnd, WM_SYSCOMMAND, SC_CLOSE, 0);
        } else if (data->closeMsg == WM_CLOSE) {
            PostMessageW(hWnd, WM_CLOSE, 0, 0);
        }
        return;
    }
    if (data->closeMsg == WM_DESTROY) return;

    SetPropW(hWnd, L"GenieCloseHandled", (HANDLE)1);
    int waitSteps;
    if (data->closeMsg == MACGENIE_ASYNC_HIDE) {
        ShowWindowAsync_Original(hWnd, SW_HIDE);
        waitSteps = 50;
    } else {
        SetPropW(hWnd, L"GenieCloseBypass", (HANDLE)1);
        if (data->closeMsg == WM_SYSCOMMAND) {
            PostMessageW(hWnd, WM_SYSCOMMAND, SC_CLOSE, 0);
        } else {
            PostMessageW(hWnd, WM_CLOSE, 0, 0);
        }
        waitSteps = 200;
    }
    for (int i = 0; i < waitSteps && IsWindow(hWnd) && IsWindowVisible(hWnd) &&
                    !g_unloading.load(std::memory_order_relaxed); ++i) {
        Sleep(10);
    }
    if (IsWindow(hWnd)) {
        MacGenieSetCloak(hWnd, FALSE);
        RemovePropW(hWnd, L"GenieCloseAnimating");
        MacGenieSetDwmTransitions(hWnd, TRUE);
        if (IsWindowVisible(hWnd)) {
            RemovePropW(hWnd, L"GenieCloseHandled");
            RemovePropW(hWnd, L"GenieCloseBypass");
        }
    }
}

// Auto-hide taskbar path: after the animation, wait out the unhide time, perform the
// real (deferred) minimize, uncloak and hand focus back so the taskbar slides away.
static void MacGenieRunDeferredMinimize(MacGenieAnimData* data) {
    if (data->deferredMinimize) {
        int unhideMs = data->unhideDurationMs;
        int animMs = data->durationMs;
        if (data->requestedUnhide && unhideMs > animMs) {
            int remaining = unhideMs - animMs;
            while (remaining > 0 && !g_unloading.load(std::memory_order_relaxed)) {
                int chunk = remaining < 20 ? remaining : 20;
                Sleep(chunk);
                remaining -= chunk;
            }
        }

        if (!g_unloading.load(std::memory_order_relaxed) && IsWindow(data->hRealWnd)) {
            SetPropW(data->hRealWnd, L"GenieBypass", (HANDLE)1);
            SendMessageTimeoutW(data->hRealWnd, WM_SYSCOMMAND, SC_MINIMIZE, 0, SMTO_NORMAL, 1000, NULL);
            RemovePropW(data->hRealWnd, L"GenieBypass");
        }
        if (IsWindow(data->hRealWnd)) {
            MacGenieSetCloak(data->hRealWnd, FALSE);
            MacGenieSetDwmTransitions(data->hRealWnd, TRUE);
        }
        if (data->requestedUnhide && !g_unloading.load(std::memory_order_relaxed)) {
            if (data->hNextApp && IsWindow(data->hNextApp) && IsWindowVisible(data->hNextApp)) {
                SetForegroundWindow(data->hNextApp);
            } else {
                SetForegroundWindow(FindWindowW(L"Progman", NULL));
            }
        }
    }
}

static void CalculateLampVertexMacOS(float tx, float ty, float p, const Geometry& w, const Geometry& i, float *outX, float *outY) {
    float split = 0.3f;
    float k = (p <= split) ? (p / split) : 1.0f;
    float j = (p > split) ? ((p - split) / (1.0f - split)) : 0.0f;

    float expandHeight = (i.y - w.y - w.height);
    float fullHeight = (i.y - w.y) - (expandHeight * (1.0f - k));
    float height = fullHeight - (j * fullHeight);

    float y = ty * height;
    float x = tx * (i.width) + tx * (w.width - i.width) * (1.0f - j) * (1.0f - ty) + tx * (w.width - i.width) * (1.0f - k) * ty;

    float offsetX = (i.x - w.x) * (y / (fullHeight + 0.1f)) * k + (i.x - w.x) * j;
    float offsetY = i.y - w.y - height - (expandHeight * (1.0f - k));

    float effectX = sinf(((height - y) / fullHeight) * 2.0f * PI + PI) * (w.x + w.width * tx - (i.x + i.width * tx)) / 7.0f * k;

    *outX = w.x + x + offsetX + effectX;
    *outY = w.y + y + offsetY;
}

static void CalculateLampVertex(float tx, float ty, float p, const Geometry& w,
                                const Geometry& i, float* outX, float* outY) {
    if (i.y + i.height * 0.5f >= w.y + w.height * 0.5f) {
        CalculateLampVertexMacOS(tx, ty, p, w, i, outX, outY);
        return;
    }
    Geometry wm = { w.x, -(w.y + w.height), w.width, w.height };
    Geometry im = { i.x, -(i.y + i.height), i.width, i.height };
    float yM;
    CalculateLampVertexMacOS(tx, 1.0f - ty, p, wm, im, outX, &yM);
    *outY = -yM;
}

// Horizontal genie (dock on the left / right edge): the same lamp curve evaluated
// with the X and Y axes swapped, so the window funnels sideways instead of down.
static void CalculateLampVertexHorizontal(float tx, float ty, float p, const Geometry& w,
                                          const Geometry& i, float* outX, float* outY) {
    Geometry wT = { w.y, w.x, w.height, w.width };
    Geometry iT = { i.y, i.x, i.height, i.width };
    float ox, oy;
    CalculateLampVertex(ty, tx, p, wT, iT, &ox, &oy);
    *outX = oy;
    *outY = ox;
}

static RECT GetGenieMonitorUnion(const RECT& winRect, HMONITOR hDockMon) {
    RECT winMon;
    HMONITOR hWinMon = MonitorFromRect(&winRect, MONITOR_DEFAULTTONEAREST);
    MONITORINFO wmi = {0};
    wmi.cbSize = sizeof(wmi);
    if (hWinMon && GetMonitorInfoW(hWinMon, &wmi)) {
        winMon = wmi.rcMonitor;
    } else {
        winMon = winRect;
    }

    RECT dockMon;
    MONITORINFO dmi = {0};
    dmi.cbSize = sizeof(dmi);
    if (hDockMon && GetMonitorInfoW(hDockMon, &dmi)) {
        dockMon = dmi.rcMonitor;
    } else {
        dockMon = winMon;
    }

    RECT u;
    u.left   = (winMon.left   < dockMon.left)   ? winMon.left   : dockMon.left;
    u.top    = (winMon.top    < dockMon.top)    ? winMon.top    : dockMon.top;
    u.right  = (winMon.right  > dockMon.right)  ? winMon.right  : dockMon.right;
    u.bottom = (winMon.bottom > dockMon.bottom) ? winMon.bottom : dockMon.bottom;
    return u;
}

static void ClampGenieCanvas(int& left, int& top, int& right, int& bottom,
                             const RECT& winRect, int dockX, const RECT& monUnion) {
    if (left   < monUnion.left)   left   = monUnion.left;
    if (right  > monUnion.right)  right  = monUnion.right;
    if (top    < monUnion.top)    top    = monUnion.top;
    if (bottom > monUnion.bottom) bottom = monUnion.bottom;

    if (left  > winRect.left)  left  = winRect.left;
    if (right < winRect.right) right = winRect.right;
    if (top   > winRect.top)   top   = winRect.top;
    if (bottom < winRect.bottom) bottom = winRect.bottom;
    if (left  > dockX - 16)    left  = dockX - 16;
    if (right < dockX + 16)    right = dockX + 16;
}

// Everything both genie renderers need to know before the first frame: window and dock
// rectangles, orientation, mesh resolution and the (tight) canvas the ghost covers.
struct GenieLayout {
    Geometry w, i;
    bool horizontal;
    int xTiles, yTiles;
    int vLeft, vTop, vWidth, vHeight;
};

static void MacGenieComputeGenieLayout(const MacGenieAnimData* data, GenieLayout& L) {
    Geometry wGeom = { (float)data->targetRect.left, (float)data->targetRect.top, (float)data->width, (float)data->height };

    MONITORINFO mi = {0};
    mi.cbSize = sizeof(MONITORINFO);
    HMONITOR hMon = data->hMon;
    GetMonitorInfoW(hMon, &mi);

    UINT dpiX = 96, dpiY = 96;
    HMODULE hShcore = LoadLibraryExW(L"Shcore.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (hShcore) {
        typedef HRESULT (WINAPI *GetDpiForMonitor_t)(HMONITOR, int, UINT*, UINT*);
        auto pGetDpiForMonitor = (GetDpiForMonitor_t)GetProcAddress(hShcore, "GetDpiForMonitor");
        if (pGetDpiForMonitor) pGetDpiForMonitor(hMon, 0, &dpiX, &dpiY);
        FreeLibrary(hShcore);
    }
    float dpiScale = dpiY / 96.0f;
    float scaledIconSize = COMPAT_ICON_SIZE * dpiScale;

    // Where the genie tip ends. It dives into the taskbar by "dock depth" percent of
    // the bar height (macOS flows into the dock icon, it does not stop at the edge).
    const float dockDepth = g_dockDepthPct.load(std::memory_order_relaxed) / 100.0f;
    float taskbarH = 48.0f * dpiScale;                         // Windows 11 default, used if unmeasurable
    bool barOnTop = false;
    float barEdge = (float)mi.rcMonitor.bottom - taskbarH;     // edge of the bar facing the screen centre

    HWND hTrayGeom = FindTaskbarForMonitor(hMon);
    if (hTrayGeom) {
        RECT tr;
        if (GetWindowRect(hTrayGeom, &tr)) {
            const int th = tr.bottom - tr.top;
            const int tw = tr.right - tr.left;
            if (th > 0 && tw > th) {                            // horizontal bar
                barOnTop = (tr.top + tr.bottom) < (mi.rcMonitor.top + mi.rcMonitor.bottom);
                const bool parked = barOnTop ? (tr.bottom <= mi.rcMonitor.top + 8)
                                             : (tr.top >= mi.rcMonitor.bottom - 8);
                if (!parked) {                                  // auto-hidden bars are parked off-screen
                    taskbarH = (float)th;
                    barEdge = barOnTop ? (float)tr.bottom : (float)tr.top;
                } else {
                    barEdge = barOnTop ? (float)mi.rcMonitor.top + taskbarH
                                       : (float)mi.rcMonitor.bottom - taskbarH;
                }
            }
        }
    }

    Geometry iGeom = {
        (float)data->targetDockX - 11.0f,
        0.0f,
        22.0f,
        scaledIconSize
    };
    if (barOnTop) iGeom.y = (barEdge - taskbarH * dockDepth) - iGeom.height;
    else          iGeom.y = barEdge + taskbarH * dockDepth;

    bool horizontalGenie = false;
    if (data->isClosing) {
        const int direction = g_closeDirection.load(std::memory_order_relaxed);
        iGeom.height = 22.0f;
        if (direction == 0) {
            horizontalGenie = true;
            iGeom.x = (float)mi.rcMonitor.left;
            iGeom.y = (float)(mi.rcMonitor.top + mi.rcMonitor.bottom) * 0.5f - 11.0f;
        } else if (direction == 1) {
            horizontalGenie = true;
            iGeom.x = (float)(mi.rcMonitor.right - 22);
            iGeom.y = (float)(mi.rcMonitor.top + mi.rcMonitor.bottom) * 0.5f - 11.0f;
        } else if (direction == 3) {
            iGeom.x = (float)(mi.rcMonitor.left + mi.rcMonitor.right) * 0.5f - 11.0f;
            iGeom.y = (float)mi.rcMonitor.bottom - iGeom.height;
        } else {
            iGeom.x = (float)(mi.rcMonitor.left + mi.rcMonitor.right) * 0.5f - 11.0f;
            iGeom.y = (float)mi.rcMonitor.top;
        }
    }

    int xTiles = g_tileCount.load(std::memory_order_relaxed);
    int yTiles = xTiles;

    const int origLeftB = data->targetRect.left;
    const int wB = data->width;
    const int dockXB = data->isClosing ? (int)(iGeom.x + iGeom.width * 0.5f) : data->targetDockX;

    const float iLeftF = iGeom.x;
    const float iWidthF = iGeom.width;
    const float sway = fmaxf(fabsf((float)origLeftB - iLeftF), fabsf((float)origLeftB + wB - iLeftF - iWidthF)) / 7.0f;
    int padX = (int)((wB / 2 > sway ? wB / 2 : sway) + 1);

    int boundLeft   = ((origLeftB < dockXB) ? origLeftB : dockXB) - padX;
    int boundRight  = (((origLeftB + wB) > dockXB) ? (origLeftB + wB) : dockXB) + padX;
    int boundTop    = (int)fminf(wGeom.y, iGeom.y);
    int boundBottom = (int)fmaxf(wGeom.y + wGeom.height, iGeom.y + iGeom.height);
    if (horizontalGenie) {
        // The lamp's sideways sway now happens along Y.
        const float swayY = fmaxf(fabsf(wGeom.y - iGeom.y),
                                  fabsf(wGeom.y + wGeom.height - iGeom.y - iGeom.height)) / 7.0f;
        const int padY = (int)((wGeom.height / 2 > swayY ? wGeom.height / 2 : swayY) + 1);
        boundTop    -= padY;
        boundBottom += padY;
    }

    RECT monUnion = GetGenieMonitorUnion(data->targetRect, data->hMon);
    ClampGenieCanvas(boundLeft, boundTop, boundRight, boundBottom, data->targetRect, dockXB, monUnion);

    int vLeft = boundLeft;
    int vTop = boundTop;
    int vWidth = boundRight - boundLeft;
    int vHeight = (boundBottom - boundTop) - 1;
    if (vWidth < 1) vWidth = 1;
    if (vHeight < 1) vHeight = 1;

    L.w = wGeom;
    L.i = iGeom;
    L.horizontal = horizontalGenie;
    L.xTiles = xTiles;
    L.yTiles = yTiles;
    L.vLeft = vLeft;
    L.vTop = vTop;
    L.vWidth = vWidth;
    L.vHeight = vHeight;
}

DWORD WINAPI MacGenieAnimThread(LPVOID lpParam) {
    MacGenieAnimData* data = (MacGenieAnimData*)lpParam;
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST);

    GenieLayout L;
    MacGenieComputeGenieLayout(data, L);
    Geometry wGeom = L.w;
    Geometry iGeom = L.i;
    const bool horizontalGenie = L.horizontal;
    int xTiles = L.xTiles;
    int yTiles = L.yTiles;
    int vLeft = L.vLeft;
    int vTop = L.vTop;
    int vWidth = L.vWidth;
    int vHeight = L.vHeight;

    HDC hScreenDC = GetDC(NULL);
    HDC hMemDC = CreateCompatibleDC(hScreenDC);

    BITMAPINFO bmi = {{0}};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = vWidth;
    bmi.bmiHeader.biHeight = -vHeight;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    void* pTargetBits = nullptr;
    HBITMAP hTargetBmp = CreateDIBSection(hScreenDC, &bmi, DIB_RGB_COLORS, &pTargetBits, NULL, 0);
    HBITMAP hOldBmp = (HBITMAP)SelectObject(hMemDC, hTargetBmp);

    HWND hGhost = CreateWindowExW(
        WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_NOACTIVATE,
        L"STATIC", NULL, WS_POPUP,
        vLeft, vTop, vWidth, vHeight,
        NULL, NULL, NULL, NULL);

    ID2D1DCRenderTarget* rt = nullptr;
    ID2D1Bitmap* snapshotBmp = nullptr;
    ID2D1BitmapBrush* bmpBrush = nullptr;

    if (g_d2dFactory) {
        D2D1_RENDER_TARGET_PROPERTIES rtProps = D2D1::RenderTargetProperties(
            D2D1_RENDER_TARGET_TYPE_DEFAULT,
            D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED),
            0, 0, D2D1_RENDER_TARGET_USAGE_GDI_COMPATIBLE, D2D1_FEATURE_LEVEL_DEFAULT
        );
        g_d2dFactory->CreateDCRenderTarget(&rtProps, &rt);
        if (rt) {
            rt->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE);
            D2D1_BITMAP_PROPERTIES bmpProps = D2D1::BitmapProperties(
                D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)
            );
            rt->CreateBitmap(D2D1::SizeU(data->width, data->height), data->pBits, data->width * 4, bmpProps, &snapshotBmp);
            if (snapshotBmp) {
                D2D1_BITMAP_BRUSH_PROPERTIES brushProps = D2D1::BitmapBrushProperties(
                    D2D1_EXTEND_MODE_CLAMP, D2D1_EXTEND_MODE_CLAMP,
                    D2D1_BITMAP_INTERPOLATION_MODE_LINEAR
                );
                rt->CreateBitmapBrush(snapshotBmp, &brushProps, nullptr, &bmpBrush);
            }
        }
    }
    bool d2dOk = (rt && bmpBrush);
    if (snapshotBmp) {
        // Direct2D keeps its own copy of the pixels; drop the GDI one right away
        // (halves the snapshot's memory for the whole animation).
        DeleteObject(data->hBitmap);
        data->hBitmap = NULL;
        data->pBits = nullptr;
    }

    const double animDur = (double)data->durationMs;
    LARGE_INTEGER qpcFreq, qpcStart, qpcNow;
    QueryPerformanceFrequency(&qpcFreq);
    QueryPerformanceCounter(&qpcStart);

    std::vector<std::vector<D2D1_POINT_2F>> grid(yTiles + 1, std::vector<D2D1_POINT_2F>(xTiles + 1));

    BOOL firstFrame = TRUE;

    double frameIntervalMs = 1000.0 / 240.0;
    DWM_TIMING_INFO ti = { sizeof(ti) };
    if (SUCCEEDED(DwmGetCompositionTimingInfo(NULL, &ti)) && ti.rateRefresh.uiDenominator) {
        double hz = (double)ti.rateRefresh.uiNumerator / ti.rateRefresh.uiDenominator;
        frameIntervalMs = 1000.0 / (hz * 2.0);
    }
    HANDLE hFrameTimer = CreateWaitableTimerExW(
        nullptr, nullptr, CREATE_WAITABLE_TIMER_HIGH_RESOLUTION,
        TIMER_MODIFY_STATE | SYNCHRONIZE);
    if (!hFrameTimer) {
        hFrameTimer = CreateWaitableTimerExW(nullptr, nullptr, 0, TIMER_MODIFY_STATE | SYNCHRONIZE);
    }

    if (d2dOk) {
        for (;;) {
            QueryPerformanceCounter(&qpcNow);
            const LARGE_INTEGER qpcFrameStart = qpcNow;
            double elapsedMs = (qpcNow.QuadPart - qpcStart.QuadPart) * 1000.0 / qpcFreq.QuadPart;
            BOOL lastFrame = (elapsedMs >= animDur);

            float raw_p = (float)fmin(elapsedMs / animDur, 1.0);
            
            // Timing curve from the settings (cubic-bezier presets). Minimize and
            // restore/launch have separate curves; restore plays the lamp in reverse.
            float eased_p = MacGenieEase(data->isRising ? g_easeRestore.load(std::memory_order_relaxed)
                                                        : g_easeMinimize.load(std::memory_order_relaxed),
                                         raw_p);

            float t = data->isRising ? (1.0f - eased_p) : eased_p;

            RECT bindRect = { 0, 0, vWidth, vHeight };
            rt->BindDC(hMemDC, &bindRect);
            rt->BeginDraw();
            rt->Clear(D2D1::ColorF(0, 0, 0, 0));
            rt->SetAntialiasMode(D2D1_ANTIALIAS_MODE_ALIASED);

            for (int y = 0; y <= yTiles; y++) {
                for (int x = 0; x <= xTiles; x++) {
                    float tx = (float)x / xTiles, ty = (float)y / yTiles;
                    float px, py;
                    if (horizontalGenie) CalculateLampVertexHorizontal(tx, ty, t, wGeom, iGeom, &px, &py);
                    else                 CalculateLampVertex(tx, ty, t, wGeom, iGeom, &px, &py);
                    grid[y][x] = D2D1::Point2F(px - vLeft, py - vTop);
                }
            }

            bool degenerate = false;
            {
                float left   = grid[0][0].x, right  = grid[0][0].x;
                float top    = grid[0][0].y, bottom = grid[0][0].y;
                for (int gy = 0; gy <= yTiles; gy++) {
                    for (int gx = 0; gx <= xTiles; gx++) {
                        left   = fminf(left,   grid[gy][gx].x);
                        right  = fmaxf(right,  grid[gy][gx].x);
                        top    = fminf(top,    grid[gy][gx].y);
                        bottom = fmaxf(bottom, grid[gy][gx].y);
                    }
                }
                degenerate = (right - left) < 2.0f || (bottom - top) < 2.0f;
            }

            // Corners are already rounded inside the snapshot (alpha), so no per-frame
            // mask layer is needed - cheaper and the corners warp with the window.
            if (!degenerate) {
                for (int y = 0; y < yTiles; y++) {
                    for (int x = 0; x < xTiles; x++) {
                        D2D1_POINT_2F p1 = grid[y][x];
                        D2D1_POINT_2F p2 = grid[y][x+1];
                        D2D1_POINT_2F p3 = grid[y+1][x];
                        D2D1_POINT_2F p4 = grid[y+1][x+1];

                        D2D1_POINT_2F c = D2D1::Point2F((p1.x+p2.x+p3.x+p4.x)/4.0f, (p1.y+p2.y+p3.y+p4.y)/4.0f);

                        float quadW = fmaxf(fabsf(p2.x - p1.x), fabsf(p4.x - p3.x));
                        float quadH = fmaxf(fabsf(p3.y - p1.y), fabsf(p4.y - p2.y));
                        float bloatAmt = fminf(quadW, quadH) * 0.05f;
                        bloatAmt = fmaxf(0.20f, fminf(0.40f, bloatAmt));

                        ID2D1PathGeometry* quadGeo = CreateQuadGeo(g_d2dFactory,
                            BloatPoint(p1, c, bloatAmt), BloatPoint(p2, c, bloatAmt),
                            BloatPoint(p4, c, bloatAmt), BloatPoint(p3, c, bloatAmt));

                        if (quadGeo) {
                            float sx = ((float)x / xTiles) * wGeom.width;
                            float sy = ((float)y / yTiles) * wGeom.height;
                            float sw = wGeom.width / xTiles;
                            float sh = wGeom.height / yTiles;

                            float m11 = (p2.x - p1.x) / sw;
                            float m12 = (p2.y - p1.y) / sw;
                            float m21 = (p3.x - p1.x) / sh;
                            float m22 = (p3.y - p1.y) / sh;
                            float m31 = p1.x - sx * m11 - sy * m21;
                            float m32 = p1.y - sx * m12 - sy * m22;

                            bmpBrush->SetTransform(D2D1::Matrix3x2F(m11, m12, m21, m22, m31, m32));
                            rt->FillGeometry(quadGeo, bmpBrush);
                            quadGeo->Release();
                        }
                    }
                }
            }
            rt->EndDraw();

            if (!degenerate) {
                POINT ptDst = { vLeft, vTop };
                SIZE sz = { vWidth, vHeight };
                POINT ptSrc = { 0, 0 };
                BLENDFUNCTION bf = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
                UpdateLayeredWindow(hGhost, hScreenDC, &ptDst, &sz, hMemDC, &ptSrc, 0, &bf, ULW_ALPHA);

                if (firstFrame) {
                    ShowWindow(hGhost, SW_SHOWNOACTIVATE);
                }
            }

            if (lastFrame) break;
            if (g_unloading.load(std::memory_order_relaxed)) break;

            if (firstFrame && !degenerate) {
                DwmFlush();
                firstFrame = FALSE;
                if (data->hFirstFrameShown) SetEvent(data->hFirstFrameShown);
            }

            if (hFrameTimer) {
                LARGE_INTEGER qpcAfter;
                QueryPerformanceCounter(&qpcAfter);
                double spentMs = (qpcAfter.QuadPart - qpcFrameStart.QuadPart) * 1000.0 / qpcFreq.QuadPart;
                double waitMs = frameIntervalMs - spentMs;
                if (waitMs > 0.1) {
                    LARGE_INTEGER due;
                    due.QuadPart = -(LONGLONG)(waitMs * 10000.0);
                    SetWaitableTimer(hFrameTimer, &due, 0, nullptr, nullptr, FALSE);
                    WaitForSingleObject(hFrameTimer, 100);
                } else {
                    SwitchToThread();
                }
            } else {
                Sleep(1);
            }
        }
    }

    if (hFrameTimer) { CloseHandle(hFrameTimer); hFrameTimer = nullptr; }

    if (data->isRising) {
        if (data->hiddenByCloak) {
            MacGenieSetCloak(data->hRealWnd, FALSE);
        } else {
            SetLayeredWindowAttributes(data->hRealWnd, 0, 255, LWA_ALPHA);
            if (!(data->originalExStyle & WS_EX_LAYERED)) {
                SetWindowLongPtrW(data->hRealWnd, GWL_EXSTYLE, data->originalExStyle);
            }
        }
    }

    if (!data->deferredMinimize && !data->isClosing) {
        MacGenieSetDwmTransitions(data->hRealWnd, TRUE);
    }
    if (data->isClosing) MacGenieFinishClose(data);

    if (bmpBrush) { bmpBrush->Release(); bmpBrush = nullptr; }
    if (snapshotBmp) { snapshotBmp->Release(); snapshotBmp = nullptr; }
    if (rt) { rt->Release(); rt = nullptr; }

    SelectObject(hMemDC, hOldBmp);
    DeleteObject(hTargetBmp);
    DeleteObject(data->hBitmap);
    DeleteDC(hMemDC);
    ReleaseDC(NULL, hScreenDC);
    DestroyWindow(hGhost);

    if (data->hFirstFrameShown) {
        SetEvent(data->hFirstFrameShown);
        CloseHandle(data->hFirstFrameShown);
    }

    MacGenieRunDeferredMinimize(data);

    {
        std::lock_guard<std::mutex> lock(g_CacheMutex);
        g_AnimActive.erase(data->hRealWnd);
    }
    delete data;
    return 0;
}

// ---------------------------------------------------------------------------
// GPU genie engine (Direct3D 11 + DirectComposition)
//
// The lamp curve runs in the vertex shader on a static mesh, the window snapshot is a
// mip-mapped texture sampled anisotropically (no shimmer when the neck gets thin), the
// result is anti-aliased with 4x MSAA and presented through a premultiplied-alpha flip
// swap chain on a DirectComposition visual, locked to the display's vsync. The CPU only
// updates three float4 per frame. Any failure while setting up falls back to the Direct2D
// engine before a single pixel has been shown.
// ---------------------------------------------------------------------------
static const char kGenieHlsl[] = R"HLSL(
cbuffer CB : register(b0) {
    float4 gW;   // window rect in canvas px: x, y, width, height
    float4 gI;   // dock rect in canvas px
    float4 gP;   // x = progress, y = 1 for the horizontal (sideways) genie, zw = canvas size
};
Texture2D tex : register(t0);
SamplerState smp : register(s0);

struct VSIn  { float2 uv : TEXCOORD0; };
struct VSOut { float4 pos : SV_Position; float2 uv : TEXCOORD0; };

float2 lampMac(float tx, float ty, float p, float4 w, float4 i) {
    float split = 0.3;
    float k = (p <= split) ? (p / split) : 1.0;
    float j = (p > split) ? ((p - split) / (1.0 - split)) : 0.0;
    float expandHeight = i.y - w.y - w.w;
    float fullHeight = (i.y - w.y) - expandHeight * (1.0 - k);
    float height = fullHeight - j * fullHeight;
    float y = ty * height;
    float x = tx * i.z + tx * (w.z - i.z) * (1.0 - j) * (1.0 - ty) + tx * (w.z - i.z) * (1.0 - k) * ty;
    float offsetX = (i.x - w.x) * (y / (fullHeight + 0.1)) * k + (i.x - w.x) * j;
    float offsetY = i.y - w.y - height - expandHeight * (1.0 - k);
    float effectX = sin(((height - y) / fullHeight) * 2.0 * 3.14159265 + 3.14159265) *
                    (w.x + w.z * tx - (i.x + i.z * tx)) / 7.0 * k;
    return float2(w.x + x + offsetX + effectX, w.y + y + offsetY);
}

float2 lamp(float tx, float ty, float p, float4 w, float4 i) {
    if (i.y + i.w * 0.5 >= w.y + w.w * 0.5) return lampMac(tx, ty, p, w, i);
    float4 wm = float4(w.x, -(w.y + w.w), w.z, w.w);
    float4 im = float4(i.x, -(i.y + i.w), i.z, i.w);
    float2 r = lampMac(tx, 1.0 - ty, p, wm, im);
    return float2(r.x, -r.y);
}

VSOut VSMain(VSIn v) {
    float2 pos;
    if (gP.y > 0.5) {
        float4 wT = float4(gW.y, gW.x, gW.w, gW.z);
        float4 iT = float4(gI.y, gI.x, gI.w, gI.z);
        float2 r = lamp(v.uv.y, v.uv.x, gP.x, wT, iT);
        pos = float2(r.y, r.x);
    } else {
        pos = lamp(v.uv.x, v.uv.y, gP.x, gW, gI);
    }
    VSOut o;
    o.pos = float4(pos.x / gP.z * 2.0 - 1.0, 1.0 - pos.y / gP.w * 2.0, 0.0, 1.0);
    o.uv = v.uv;
    return o;
}

float4 PSMain(VSOut i) : SV_Target {
    return tex.Sample(smp, i.uv);   // snapshot is premultiplied, so is the swap chain
}
)HLSL";

static const GUID kIID_IDCompositionDevice = {0xC37EA93A, 0xE7AA, 0x450D, {0xB1, 0x6F, 0x97, 0x46, 0xCB, 0x04, 0x07, 0xF3}};

struct MacGenieGpuCB { float w[4]; float i[4]; float p[4]; };

static std::mutex g_GpuMutex;
static ID3DBlob* g_gpuVsBlob = nullptr;
static ID3DBlob* g_gpuPsBlob = nullptr;

template <class T> static void MacGenieRelease(T*& p) { if (p) { p->Release(); p = nullptr; } }

// Compile the shaders once per process (bytecode is a few KB); devices are per animation.
static bool MacGenieGpuGetShaders(ID3DBlob** vs, ID3DBlob** ps) {
    std::lock_guard<std::mutex> lock(g_GpuMutex);
    if (!g_gpuVsBlob || !g_gpuPsBlob) {
        typedef HRESULT (WINAPI *D3DCompile_t)(LPCVOID, SIZE_T, LPCSTR, const D3D_SHADER_MACRO*, ID3DInclude*,
                                               LPCSTR, LPCSTR, UINT, UINT, ID3DBlob**, ID3DBlob**);
        HMODULE hc = LoadLibraryExW(L"d3dcompiler_47.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
        D3DCompile_t compile = hc ? (D3DCompile_t)(void*)GetProcAddress(hc, "D3DCompile") : nullptr;
        if (!compile) return false;
        ID3DBlob* v = nullptr; ID3DBlob* p = nullptr; ID3DBlob* err = nullptr;
        const UINT flags = (1u << 15);   // D3DCOMPILE_OPTIMIZATION_LEVEL3
        HRESULT h1 = compile(kGenieHlsl, sizeof(kGenieHlsl) - 1, "genie", nullptr, nullptr, "VSMain", "vs_4_0", flags, 0, &v, &err);
        MacGenieRelease(err);
        HRESULT h2 = compile(kGenieHlsl, sizeof(kGenieHlsl) - 1, "genie", nullptr, nullptr, "PSMain", "ps_4_0", flags, 0, &p, &err);
        MacGenieRelease(err);
        if (FAILED(h1) || FAILED(h2) || !v || !p) { MacGenieRelease(v); MacGenieRelease(p); return false; }
        g_gpuVsBlob = v;
        g_gpuPsBlob = p;
    }
    *vs = g_gpuVsBlob;
    *ps = g_gpuPsBlob;
    return true;
}

struct MacGenieGpu {
    HWND hGhost = nullptr;
    ID3D11Device* dev = nullptr;
    ID3D11DeviceContext* ctx = nullptr;
    IDXGIDevice* dxgiDev = nullptr;
    IDXGISwapChain1* sc = nullptr;
    IDCompositionDevice* dcDev = nullptr;
    IDCompositionTarget* dcTarget = nullptr;
    IDCompositionVisual* dcVisual = nullptr;
    ID3D11Texture2D* snapTex = nullptr;
    ID3D11ShaderResourceView* snapSrv = nullptr;
    ID3D11VertexShader* vs = nullptr;
    ID3D11PixelShader* ps = nullptr;
    ID3D11InputLayout* layout = nullptr;
    ID3D11Buffer* vb = nullptr;
    ID3D11Buffer* ib = nullptr;
    ID3D11Buffer* cb = nullptr;
    ID3D11SamplerState* sampler = nullptr;
    ID3D11RasterizerState* raster = nullptr;
    ID3D11Texture2D* backBuf = nullptr;
    ID3D11RenderTargetView* backRtv = nullptr;
    ID3D11Texture2D* msaaTex = nullptr;
    ID3D11RenderTargetView* msaaRtv = nullptr;
    UINT indexCount = 0;

    void Release() {
        if (ctx) { ctx->ClearState(); ctx->Flush(); }
        MacGenieRelease(msaaRtv); MacGenieRelease(msaaTex);
        MacGenieRelease(backRtv); MacGenieRelease(backBuf);
        MacGenieRelease(raster); MacGenieRelease(sampler);
        MacGenieRelease(cb); MacGenieRelease(ib); MacGenieRelease(vb);
        MacGenieRelease(layout); MacGenieRelease(ps); MacGenieRelease(vs);
        MacGenieRelease(snapSrv); MacGenieRelease(snapTex);
        MacGenieRelease(dcVisual); MacGenieRelease(dcTarget); MacGenieRelease(dcDev);
        MacGenieRelease(sc); MacGenieRelease(dxgiDev);
        MacGenieRelease(ctx); MacGenieRelease(dev);
        if (hGhost) { DestroyWindow(hGhost); hGhost = nullptr; }
    }
};

static bool MacGenieGpuInit(const MacGenieAnimData* data, const GenieLayout& L, MacGenieGpu& g) {
    const int W = data->width, H = data->height;
    if (W < 1 || H < 1 || W > 16000 || H > 16000 || L.vWidth > 16000 || L.vHeight > 16000) return false;

    ID3DBlob* vsBlob = nullptr; ID3DBlob* psBlob = nullptr;
    if (!MacGenieGpuGetShaders(&vsBlob, &psBlob)) return false;

    typedef HRESULT (WINAPI *D3D11CreateDevice_t)(IDXGIAdapter*, D3D_DRIVER_TYPE, HMODULE, UINT,
        const D3D_FEATURE_LEVEL*, UINT, UINT, ID3D11Device**, D3D_FEATURE_LEVEL*, ID3D11DeviceContext**);
    typedef HRESULT (WINAPI *DCompositionCreateDevice_t)(IDXGIDevice*, REFIID, void**);
    HMODULE hD3D = LoadLibraryExW(L"d3d11.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    HMODULE hDComp = LoadLibraryExW(L"dcomp.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!hD3D || !hDComp) return false;
    auto pCreateDevice = (D3D11CreateDevice_t)(void*)GetProcAddress(hD3D, "D3D11CreateDevice");
    auto pDCompCreate = (DCompositionCreateDevice_t)(void*)GetProcAddress(hDComp, "DCompositionCreateDevice");
    if (!pCreateDevice || !pDCompCreate) return false;

    const D3D_FEATURE_LEVEL levels[] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_1, D3D_FEATURE_LEVEL_10_0 };
    D3D_FEATURE_LEVEL got;
    if (FAILED(pCreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, D3D11_CREATE_DEVICE_BGRA_SUPPORT,
                             levels, ARRAYSIZE(levels), D3D11_SDK_VERSION, &g.dev, &got, &g.ctx)) || !g.dev || !g.ctx) return false;
    if (FAILED(g.dev->QueryInterface(IID_IDXGIDevice, (void**)&g.dxgiDev)) || !g.dxgiDev) return false;

    IDXGIAdapter* adapter = nullptr;
    IDXGIFactory2* factory = nullptr;
    bool haveFactory = SUCCEEDED(g.dxgiDev->GetAdapter(&adapter)) && adapter &&
                       SUCCEEDED(adapter->GetParent(IID_IDXGIFactory2, (void**)&factory)) && factory;
    MacGenieRelease(adapter);
    if (!haveFactory) return false;

    // Ghost window: no GDI surface at all, composed purely by DirectComposition.
    g.hGhost = CreateWindowExW(
        WS_EX_NOREDIRECTIONBITMAP | WS_EX_TOOLWINDOW | WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_NOACTIVATE,
        L"STATIC", NULL, WS_POPUP, L.vLeft, L.vTop, L.vWidth, L.vHeight, NULL, NULL, NULL, NULL);
    if (!g.hGhost) { MacGenieRelease(factory); return false; }

    DXGI_SWAP_CHAIN_DESC1 sd = {};
    sd.Width = L.vWidth;
    sd.Height = L.vHeight;
    sd.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    sd.SampleDesc.Count = 1;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.BufferCount = 2;
    sd.Scaling = DXGI_SCALING_STRETCH;
    sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
    sd.AlphaMode = DXGI_ALPHA_MODE_PREMULTIPLIED;
    HRESULT hr = factory->CreateSwapChainForComposition(g.dev, &sd, nullptr, &g.sc);
    MacGenieRelease(factory);
    if (FAILED(hr) || !g.sc) return false;

    if (FAILED(pDCompCreate(g.dxgiDev, kIID_IDCompositionDevice, (void**)&g.dcDev)) || !g.dcDev) return false;
    if (FAILED(g.dcDev->CreateTargetForHwnd(g.hGhost, TRUE, &g.dcTarget)) || !g.dcTarget) return false;
    if (FAILED(g.dcDev->CreateVisual(&g.dcVisual)) || !g.dcVisual) return false;
    if (FAILED(g.dcVisual->SetContent(g.sc)) || FAILED(g.dcTarget->SetRoot(g.dcVisual)) ||
        FAILED(g.dcDev->Commit())) return false;

    // Snapshot texture with a full mip chain.
    D3D11_TEXTURE2D_DESC td = {};
    td.Width = W; td.Height = H; td.MipLevels = 0; td.ArraySize = 1;
    td.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    td.SampleDesc.Count = 1;
    td.Usage = D3D11_USAGE_DEFAULT;
    td.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
    td.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;
    if (FAILED(g.dev->CreateTexture2D(&td, nullptr, &g.snapTex)) || !g.snapTex) return false;
    g.ctx->UpdateSubresource(g.snapTex, 0, nullptr, data->pBits, W * 4, 0);
    if (FAILED(g.dev->CreateShaderResourceView(g.snapTex, nullptr, &g.snapSrv)) || !g.snapSrv) return false;
    g.ctx->GenerateMips(g.snapSrv);

    if (FAILED(g.dev->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &g.vs)) ||
        FAILED(g.dev->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &g.ps))) return false;
    const D3D11_INPUT_ELEMENT_DESC il[] = {
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 } };
    if (FAILED(g.dev->CreateInputLayout(il, 1, vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &g.layout))) return false;

    // Static (n+1)^2 grid of (u,v); the vertex shader does all the warping.
    int n = g_tileCount.load(std::memory_order_relaxed);
    if (n < 64) n = 64;
    if (n > 128) n = 128;
    std::vector<float> verts((size_t)(n + 1) * (n + 1) * 2);
    for (int y = 0; y <= n; ++y)
        for (int x = 0; x <= n; ++x) {
            verts[((size_t)y * (n + 1) + x) * 2 + 0] = (float)x / n;
            verts[((size_t)y * (n + 1) + x) * 2 + 1] = (float)y / n;
        }
    std::vector<unsigned short> idx((size_t)n * n * 6);
    size_t k = 0;
    for (int y = 0; y < n; ++y)
        for (int x = 0; x < n; ++x) {
            unsigned short a = (unsigned short)(y * (n + 1) + x), b = (unsigned short)(a + 1);
            unsigned short c = (unsigned short)(a + (n + 1)), d = (unsigned short)(c + 1);
            idx[k++] = a; idx[k++] = b; idx[k++] = c;
            idx[k++] = b; idx[k++] = d; idx[k++] = c;
        }
    g.indexCount = (UINT)idx.size();
    D3D11_BUFFER_DESC bd = {};
    D3D11_SUBRESOURCE_DATA sr = {};
    bd.ByteWidth = (UINT)(verts.size() * sizeof(float)); bd.Usage = D3D11_USAGE_IMMUTABLE; bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    sr.pSysMem = verts.data();
    if (FAILED(g.dev->CreateBuffer(&bd, &sr, &g.vb))) return false;
    bd.ByteWidth = (UINT)(idx.size() * sizeof(unsigned short)); bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    sr.pSysMem = idx.data();
    if (FAILED(g.dev->CreateBuffer(&bd, &sr, &g.ib))) return false;
    bd.ByteWidth = sizeof(MacGenieGpuCB); bd.Usage = D3D11_USAGE_DYNAMIC; bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    if (FAILED(g.dev->CreateBuffer(&bd, nullptr, &g.cb))) return false;

    D3D11_SAMPLER_DESC smd = {};
    smd.Filter = D3D11_FILTER_ANISOTROPIC;
    smd.AddressU = smd.AddressV = smd.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    smd.MaxAnisotropy = 8;
    smd.MaxLOD = D3D11_FLOAT32_MAX;
    smd.ComparisonFunc = D3D11_COMPARISON_NEVER;
    if (FAILED(g.dev->CreateSamplerState(&smd, &g.sampler))) return false;

    // 4x MSAA target when available (the flip swap chain itself cannot be multisampled).
    UINT quality = 0;
    bool msaa = SUCCEEDED(g.dev->CheckMultisampleQualityLevels(DXGI_FORMAT_B8G8R8A8_UNORM, 4, &quality)) && quality > 0;
    if (msaa) {
        D3D11_TEXTURE2D_DESC md = {};
        md.Width = L.vWidth; md.Height = L.vHeight; md.MipLevels = 1; md.ArraySize = 1;
        md.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
        md.SampleDesc.Count = 4; md.SampleDesc.Quality = 0;
        md.Usage = D3D11_USAGE_DEFAULT; md.BindFlags = D3D11_BIND_RENDER_TARGET;
        if (FAILED(g.dev->CreateTexture2D(&md, nullptr, &g.msaaTex)) || !g.msaaTex ||
            FAILED(g.dev->CreateRenderTargetView(g.msaaTex, nullptr, &g.msaaRtv))) {
            MacGenieRelease(g.msaaRtv); MacGenieRelease(g.msaaTex); msaa = false;
        }
    }
    D3D11_RASTERIZER_DESC rd = {};
    rd.FillMode = D3D11_FILL_SOLID; rd.CullMode = D3D11_CULL_NONE; rd.DepthClipEnable = TRUE;
    rd.MultisampleEnable = msaa ? TRUE : FALSE;
    if (FAILED(g.dev->CreateRasterizerState(&rd, &g.raster))) return false;

    if (FAILED(g.sc->GetBuffer(0, IID_ID3D11Texture2D, (void**)&g.backBuf)) || !g.backBuf ||
        FAILED(g.dev->CreateRenderTargetView(g.backBuf, nullptr, &g.backRtv))) return false;
    return true;
}

DWORD WINAPI MacGenieAnimThreadGpu(LPVOID lpParam) {
    MacGenieAnimData* data = (MacGenieAnimData*)lpParam;

    GenieLayout L;
    MacGenieComputeGenieLayout(data, L);

    MacGenieGpu g;
    if (g_gpuBroken.load(std::memory_order_relaxed) || !MacGenieGpuInit(data, L, g)) {
        g.Release();
        g_gpuBroken.store(true, std::memory_order_relaxed);
        return MacGenieAnimThread(lpParam);          // Direct2D engine takes over, nothing was shown yet
    }
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST);

    // The texture owns a copy of the pixels now.
    DeleteObject(data->hBitmap);
    data->hBitmap = NULL;
    data->pBits = nullptr;

    const double animDur = (double)data->durationMs;
    LARGE_INTEGER qpcFreq, qpcStart, qpcNow;
    QueryPerformanceFrequency(&qpcFreq);
    QueryPerformanceCounter(&qpcStart);

    ID3D11RenderTargetView* target = g.msaaRtv ? g.msaaRtv : g.backRtv;
    const float clear[4] = { 0, 0, 0, 0 };
    const UINT stride = sizeof(float) * 2, offset = 0;
    D3D11_VIEWPORT vp = { 0.0f, 0.0f, (float)L.vWidth, (float)L.vHeight, 0.0f, 1.0f };
    BOOL firstFrame = TRUE;

    for (;;) {
        QueryPerformanceCounter(&qpcNow);
        double elapsedMs = (qpcNow.QuadPart - qpcStart.QuadPart) * 1000.0 / qpcFreq.QuadPart;
        const BOOL lastFrame = (elapsedMs >= animDur);
        const float raw_p = (float)fmin(elapsedMs / animDur, 1.0);
        const float e = MacGenieEase(data->isRising ? g_easeRestore.load(std::memory_order_relaxed)
                                                    : g_easeMinimize.load(std::memory_order_relaxed), raw_p);
        const float t = data->isRising ? (1.0f - e) : e;

        D3D11_MAPPED_SUBRESOURCE m;
        if (SUCCEEDED(g.ctx->Map(g.cb, 0, D3D11_MAP_WRITE_DISCARD, 0, &m))) {
            MacGenieGpuCB* c = (MacGenieGpuCB*)m.pData;
            c->w[0] = L.w.x - L.vLeft; c->w[1] = L.w.y - L.vTop; c->w[2] = L.w.width; c->w[3] = L.w.height;
            c->i[0] = L.i.x - L.vLeft; c->i[1] = L.i.y - L.vTop; c->i[2] = L.i.width; c->i[3] = L.i.height;
            c->p[0] = t; c->p[1] = L.horizontal ? 1.0f : 0.0f; c->p[2] = (float)L.vWidth; c->p[3] = (float)L.vHeight;
            g.ctx->Unmap(g.cb, 0);
        }

        g.ctx->OMSetRenderTargets(1, &target, nullptr);
        g.ctx->ClearRenderTargetView(target, clear);
        g.ctx->RSSetViewports(1, &vp);
        g.ctx->RSSetState(g.raster);
        g.ctx->IASetInputLayout(g.layout);
        g.ctx->IASetVertexBuffers(0, 1, &g.vb, &stride, &offset);
        g.ctx->IASetIndexBuffer(g.ib, DXGI_FORMAT_R16_UINT, 0);
        g.ctx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        g.ctx->VSSetShader(g.vs, nullptr, 0);
        g.ctx->VSSetConstantBuffers(0, 1, &g.cb);
        g.ctx->PSSetShader(g.ps, nullptr, 0);
        g.ctx->PSSetShaderResources(0, 1, &g.snapSrv);
        g.ctx->PSSetSamplers(0, 1, &g.sampler);
        g.ctx->DrawIndexed(g.indexCount, 0, 0);
        if (g.msaaTex) g.ctx->ResolveSubresource(g.backBuf, 0, g.msaaTex, 0, DXGI_FORMAT_B8G8R8A8_UNORM);
        g.sc->Present(1, 0);                       // vsync-locked pacing

        if (firstFrame) {
            ShowWindow(g.hGhost, SW_SHOWNOACTIVATE);
            DwmFlush();
            firstFrame = FALSE;
            if (data->hFirstFrameShown) SetEvent(data->hFirstFrameShown);
        }
        if (lastFrame) break;
        if (g_unloading.load(std::memory_order_relaxed)) break;
    }

    if (data->isRising) {
        if (data->hiddenByCloak) {
            MacGenieSetCloak(data->hRealWnd, FALSE);
        } else {
            SetLayeredWindowAttributes(data->hRealWnd, 0, 255, LWA_ALPHA);
            if (!(data->originalExStyle & WS_EX_LAYERED)) {
                SetWindowLongPtrW(data->hRealWnd, GWL_EXSTYLE, data->originalExStyle);
            }
        }
    }
    if (!data->deferredMinimize && !data->isClosing) MacGenieSetDwmTransitions(data->hRealWnd, TRUE);
    if (data->isClosing) MacGenieFinishClose(data);

    g.Release();                                   // also destroys the ghost window

    if (data->hFirstFrameShown) {
        SetEvent(data->hFirstFrameShown);
        CloseHandle(data->hFirstFrameShown);
    }
    MacGenieRunDeferredMinimize(data);
    {
        std::lock_guard<std::mutex> lock(g_CacheMutex);
        g_AnimActive.erase(data->hRealWnd);
    }
    delete data;
    return 0;
}

DWORD WINAPI MacGenieAnimThreadClassic(LPVOID lpParam) {
    MacGenieAnimData* data = (MacGenieAnimData*)lpParam;
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST);

    const int W = data->width;
    const int H = data->height;
    RECT mon;
    MONITORINFO mmi; mmi.cbSize = sizeof(mmi);
    if (data->hMon && GetMonitorInfoW(data->hMon, &mmi)) {
        mon = mmi.rcMonitor;
    } else {
        mon.left = 0; mon.top = 0;
        mon.right = GetSystemMetrics(SM_CXSCREEN);
        mon.bottom = GetSystemMetrics(SM_CYSCREEN);
    }

    const int origLeft = data->targetRect.left;
    const int origTop  = data->targetRect.top;
    const float origCenterX = (float)origLeft + W * 0.5f;

    int dockX = data->targetDockX;
    if (dockX < mon.left) dockX = mon.left;
    if (dockX > mon.right) dockX = mon.right;
    float dockY = (float)mon.bottom;
    if (HWND hTray = FindTaskbarForMonitor(data->hMon)) {
        RECT tr;
        if (GetWindowRect(hTray, &tr) && tr.bottom > tr.top &&
            (tr.top + tr.bottom) < (mon.top + mon.bottom)) {
            dockY = (float)mon.top;
        }
    }
    if (data->isClosing) {
        const int direction = g_closeDirection.load(std::memory_order_relaxed);
        if (direction == 0) {
            dockX = mon.left;
            dockY = (float)(mon.top + mon.bottom) * 0.5f;
        } else if (direction == 1) {
            dockX = mon.right;
            dockY = (float)(mon.top + mon.bottom) * 0.5f;
        } else if (direction == 3) {
            dockX = (mon.left + mon.right) / 2;
            dockY = (float)mon.bottom;
        } else {
            dockX = (mon.left + mon.right) / 2;
            dockY = (float)mon.top;
        }
    }
    const float dockXf = (float)dockX;
    const float origBottomF = (float)(origTop + H);
    const bool dockAbove = dockY < ((float)origTop + origBottomF) * 0.5f;
    if (dockAbove) { if (dockY > (float)origTop) dockY = (float)origTop; }
    else           { if (dockY < origBottomF)    dockY = origBottomF; }
    float neckW = W * 0.03f;
    if (neckW < 12.0f) neckW = 12.0f;
    if (neckW > 60.0f) neckW = 60.0f;

    int boundLeft   = (origLeft < dockX ? origLeft : dockX) - W / 2;
    int boundRight  = ((origLeft + W) > dockX ? (origLeft + W) : dockX) + W / 2;
    int boundTop    = std::min<int>(origTop, (int)dockY);
    int boundBottom = std::max<int>(origTop + H, (int)dockY);
    RECT monUnion = GetGenieMonitorUnion(data->targetRect, data->hMon);
    ClampGenieCanvas(boundLeft, boundTop, boundRight, boundBottom, data->targetRect, dockX, monUnion);
    int boundW = boundRight - boundLeft;
    int boundH = boundBottom - boundTop;
    if (boundW < 1) boundW = 1;
    if (boundH < 1) boundH = 1;

    HWND hGhost = CreateWindowExW(
        WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_TOPMOST | WS_EX_TRANSPARENT,
        L"STATIC", NULL, WS_POPUP,
        boundLeft, boundTop, boundW, boundH,
        NULL, NULL, NULL, NULL);

    HDC hScreenDC = GetDC(NULL);

    HDC hSrcDC = CreateCompatibleDC(hScreenDC);
    HBITMAP hOldSrc = (HBITMAP)SelectObject(hSrcDC, data->hBitmap);

    BITMAPINFO sbmi;
    ZeroMemory(&sbmi, sizeof(sbmi));
    sbmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    sbmi.bmiHeader.biWidth = W;
    sbmi.bmiHeader.biHeight = -H;
    sbmi.bmiHeader.biPlanes = 1;
    sbmi.bmiHeader.biBitCount = 32;
    sbmi.bmiHeader.biCompression = BI_RGB;
    BYTE* srcBits = NULL;
    HBITMAP hSrcDib = CreateDIBSection(hScreenDC, &sbmi, DIB_RGB_COLORS, (void**)&srcBits, NULL, 0);
    HDC hSrcDibDC = CreateCompatibleDC(hScreenDC);
    HBITMAP hOldSrcDib = (HBITMAP)SelectObject(hSrcDibDC, hSrcDib);
    BitBlt(hSrcDibDC, 0, 0, W, H, hSrcDC, 0, 0, SRCCOPY);
    GdiFlush();
    const int srcStride = W * 4;

    BITMAPINFO bmi;
    ZeroMemory(&bmi, sizeof(bmi));
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = boundW;
    bmi.bmiHeader.biHeight = -boundH;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;
    BYTE* pBits = NULL;
    HBITMAP hCanvas = CreateDIBSection(hScreenDC, &bmi, DIB_RGB_COLORS, (void**)&pBits, NULL, 0);
    HDC hCanvasDC = CreateCompatibleDC(hScreenDC);
    HBITMAP hOldCanvas = (HBITMAP)SelectObject(hCanvasDC, hCanvas);
    const int canvasStride = boundW * 4;

    const float SPREAD = 0.65f;
    const size_t canvasBytes = (size_t)boundW * 4 * boundH;

    const double totalMs = (double)data->durationMs;

    auto morphAt = [&](float v, float tt) -> float {
        float m = tt * (1.0f + SPREAD) - (1.0f - v) * SPREAD;
        if (m < 0.0f) m = 0.0f;
        if (m > 1.0f) m = 1.0f;
        return m * m * (3.0f - 2.0f * m);
    };

    float* yb = new float[H + 1];

    LARGE_INTEGER qpcFreq, qpcStart, qpcNow;
    QueryPerformanceFrequency(&qpcFreq);
    QueryPerformanceCounter(&qpcStart);

    double frameIntervalMs = 1000.0 / 240.0;
    DWM_TIMING_INFO ti = { sizeof(ti) };
    if (SUCCEEDED(DwmGetCompositionTimingInfo(NULL, &ti)) && ti.rateRefresh.uiDenominator) {
        double hz = (double)ti.rateRefresh.uiNumerator / ti.rateRefresh.uiDenominator;
        frameIntervalMs = 1000.0 / (hz * 2.0);
    }
    HANDLE hFrameTimer = CreateWaitableTimerExW(
        nullptr, nullptr, CREATE_WAITABLE_TIMER_HIGH_RESOLUTION,
        TIMER_MODIFY_STATE | SYNCHRONIZE);
    if (!hFrameTimer) {
        hFrameTimer = CreateWaitableTimerExW(nullptr, nullptr, 0, TIMER_MODIFY_STATE | SYNCHRONIZE);
    }

    BOOL firstFrame = TRUE;
    for (;;) {
        QueryPerformanceCounter(&qpcNow);
        const LARGE_INTEGER qpcFrameStart = qpcNow;
        double elapsedMs = (qpcNow.QuadPart - qpcStart.QuadPart) * 1000.0 / qpcFreq.QuadPart;
        BOOL lastFrame = (elapsedMs >= totalMs);
        float progress = lastFrame ? 1.0f : (float)(elapsedMs / totalMs);

        float eased_progress = MacGenieEase(data->isRising ? g_easeRestore.load(std::memory_order_relaxed)
                                                           : g_easeMinimize.load(std::memory_order_relaxed),
                                            progress);

        float tt = data->isRising ? (1.0f - eased_progress) : eased_progress;

        float fade = 1.0f;
        if (tt > 0.8f) fade = (1.0f - tt) / 0.2f;
        if (fade < 0.0f) fade = 0.0f;
        if (fade > 1.0f) fade = 1.0f;

        memset(pBits, 0, canvasBytes);

        for (int k = 0; k <= H; ++k) {
            float v = (float)k / (float)H;
            float e = morphAt(dockAbove ? (1.0f - v) : v, tt);
            float idY = (float)origTop + (float)H * v;
            yb[k] = idY + (dockY - idY) * e;
        }

        int kSeg = 0;
        for (int yC = 0; yC < boundH; ++yC) {
            float screenY = (float)(yC + boundTop) + 0.5f;
            if (screenY < yb[0] || screenY >= yb[H]) continue;

            while (kSeg < H - 1 && yb[kSeg + 1] <= screenY) kSeg++;
            float segH = yb[kSeg + 1] - yb[kSeg];
            float frac = segH > 1e-4f ? (screenY - yb[kSeg]) / segH : 0.0f;
            float v = ((float)kSeg + frac) / (float)H;

            float em = morphAt(dockAbove ? (1.0f - v) : v, tt);
            float width = (float)W + (neckW - (float)W) * em;
            if (width < 1.0f) width = 1.0f;
            float cx = origCenterX + (dockXf - origCenterX) * em;
            float leftCanvas = (cx - width * 0.5f) - (float)boundLeft;

            int srcRow = (int)(v * (float)H);
            if (srcRow < 0) srcRow = 0;
            if (srcRow > H - 1) srcRow = H - 1;
            const BYTE* srcRowPtr = srcBits + (size_t)srcRow * srcStride;
            BYTE* dstRowPtr = pBits + (size_t)yC * canvasStride;

            int xStart = (int)leftCanvas;
            int xEnd   = (int)(leftCanvas + width) + 1;
            if (xStart < 0) xStart = 0;
            if (xEnd > boundW) xEnd = boundW;

            float invW = 1.0f / width;
            for (int xC = xStart; xC < xEnd; ++xC) {
                float u = ((float)xC + 0.5f - leftCanvas) * invW;
                if (u < 0.0f || u >= 1.0f) continue;
                int srcX = (int)(u * (float)W);
                if (srcX < 0) srcX = 0;
                if (srcX > W - 1) srcX = W - 1;
                const BYTE* sp = srcRowPtr + (size_t)srcX * 4;
                BYTE* dp = dstRowPtr + (size_t)xC * 4;
                dp[0] = sp[0];
                dp[1] = sp[1];
                dp[2] = sp[2];
                dp[3] = sp[3];
            }
        }

        POINT ptDst = { boundLeft, boundTop };
        SIZE  sz    = { boundW, boundH };
        POINT ptSrc = { 0, 0 };
        BLENDFUNCTION bf;
        bf.BlendOp = AC_SRC_OVER;
        bf.BlendFlags = 0;
        bf.SourceConstantAlpha = (BYTE)(255.0f * fade);
        bf.AlphaFormat = AC_SRC_ALPHA;
        UpdateLayeredWindow(hGhost, hScreenDC, &ptDst, &sz, hCanvasDC, &ptSrc, 0, &bf, ULW_ALPHA);

        if (firstFrame) {
            ShowWindow(hGhost, SW_SHOWNOACTIVATE);
        }

        if (lastFrame) break;
        if (g_unloading.load(std::memory_order_relaxed)) break;

        if (firstFrame) {
            DwmFlush();
            firstFrame = FALSE;
            if (data->hFirstFrameShown) SetEvent(data->hFirstFrameShown);
        }

        if (hFrameTimer) {
            LARGE_INTEGER qpcAfter;
            QueryPerformanceCounter(&qpcAfter);
            double spentMs = (qpcAfter.QuadPart - qpcFrameStart.QuadPart) * 1000.0 / qpcFreq.QuadPart;
            double waitMs = frameIntervalMs - spentMs;
            if (waitMs > 0.1) {
                LARGE_INTEGER due;
                due.QuadPart = -(LONGLONG)(waitMs * 10000.0);
                SetWaitableTimer(hFrameTimer, &due, 0, nullptr, nullptr, FALSE);
                WaitForSingleObject(hFrameTimer, 100);
            } else {
                SwitchToThread();
            }
        } else {
            Sleep(1);
        }
    }

    if (hFrameTimer) { CloseHandle(hFrameTimer); hFrameTimer = nullptr; }

    if (data->isRising) {
        if (data->hiddenByCloak) {
            MacGenieSetCloak(data->hRealWnd, FALSE);
        } else {
            SetLayeredWindowAttributes(data->hRealWnd, 0, 255, LWA_ALPHA);
            if (!(data->originalExStyle & WS_EX_LAYERED)) {
                SetWindowLongPtrW(data->hRealWnd, GWL_EXSTYLE, data->originalExStyle);
            }
        }
    }

    if (!data->deferredMinimize && !data->isClosing) {
        MacGenieSetDwmTransitions(data->hRealWnd, TRUE);
    }
    if (data->isClosing) MacGenieFinishClose(data);

    delete[] yb;
    SelectObject(hCanvasDC, hOldCanvas);
    SelectObject(hSrcDibDC, hOldSrcDib);
    SelectObject(hSrcDC, hOldSrc);
    DeleteObject(hCanvas);
    DeleteObject(hSrcDib);
    DeleteObject(data->hBitmap);
    DeleteDC(hCanvasDC);
    DeleteDC(hSrcDibDC);
    DeleteDC(hSrcDC);
    ReleaseDC(NULL, hScreenDC);
    DestroyWindow(hGhost);

    if (data->hFirstFrameShown) {
        SetEvent(data->hFirstFrameShown);
        CloseHandle(data->hFirstFrameShown);
    }

    MacGenieRunDeferredMinimize(data);

    {
        std::lock_guard<std::mutex> lock(g_CacheMutex);
        g_AnimActive.erase(data->hRealWnd);
    }
    delete data;
    return 0;
}

// ---------------------------------------------------------------------------
// Shared helpers: frame pacing, Windows 11 corner rounding
// ---------------------------------------------------------------------------
static double MacGenieFrameIntervalMs() {
    double frameIntervalMs = 1000.0 / 240.0;
    DWM_TIMING_INFO ti = { sizeof(ti) };
    if (SUCCEEDED(DwmGetCompositionTimingInfo(NULL, &ti)) && ti.rateRefresh.uiDenominator) {
        double hz = (double)ti.rateRefresh.uiNumerator / ti.rateRefresh.uiDenominator;
        if (hz > 1.0) frameIntervalMs = 1000.0 / (hz * 2.0);
    }
    return frameIntervalMs;
}

static HANDLE MacGenieCreateFrameTimer() {
    HANDLE h = CreateWaitableTimerExW(nullptr, nullptr, CREATE_WAITABLE_TIMER_HIGH_RESOLUTION,
                                      TIMER_MODIFY_STATE | SYNCHRONIZE);
    if (!h) h = CreateWaitableTimerExW(nullptr, nullptr, 0, TIMER_MODIFY_STATE | SYNCHRONIZE);
    return h;
}

static void MacGenieWaitNextFrame(HANDLE hTimer, double intervalMs,
                                  const LARGE_INTEGER& frameStart, const LARGE_INTEGER& freq) {
    if (!hTimer) { Sleep(1); return; }
    LARGE_INTEGER now;
    QueryPerformanceCounter(&now);
    double spentMs = (now.QuadPart - frameStart.QuadPart) * 1000.0 / freq.QuadPart;
    double waitMs = intervalMs - spentMs;
    if (waitMs > 0.1) {
        LARGE_INTEGER due;
        due.QuadPart = -(LONGLONG)(waitMs * 10000.0);
        SetWaitableTimer(hTimer, &due, 0, nullptr, nullptr, FALSE);
        WaitForSingleObject(hTimer, 100);
    } else {
        SwitchToThread();
    }
}

static bool MacGenieIsWindows11() {
    static const bool isWin11 = [] {
        typedef LONG (WINAPI *RtlGetVersion_t)(OSVERSIONINFOW*);
        HMODULE hNt = GetModuleHandleW(L"ntdll.dll");
        RtlGetVersion_t fn = hNt ? (RtlGetVersion_t)(void*)GetProcAddress(hNt, "RtlGetVersion") : nullptr;
        OSVERSIONINFOW vi = {};
        vi.dwOSVersionInfoSize = sizeof(vi);
        return fn && fn(&vi) == 0 && vi.dwBuildNumber >= 22000;
    }();
    return isWin11;
}

static UINT MacGenieWindowDpi(HWND hWnd) {
    typedef UINT (WINAPI *GetDpiForWindow_t)(HWND);
    static GetDpiForWindow_t fn = [] {
        HMODULE h = GetModuleHandleW(L"user32.dll");
        return h ? (GetDpiForWindow_t)(void*)GetProcAddress(h, "GetDpiForWindow") : nullptr;
    }();
    UINT dpi = fn ? fn(hWnd) : 0;
    return dpi ? dpi : 96;
}

static void MacGenieScalePixel(DWORD& px, float coverage) {
    // Premultiplied BGRA: scaling every channel keeps the pixel valid.
    DWORD a = (DWORD)(((px >> 24) & 0xFF) * coverage + 0.5f);
    DWORD r = (DWORD)(((px >> 16) & 0xFF) * coverage + 0.5f);
    DWORD g = (DWORD)(((px >> 8) & 0xFF) * coverage + 0.5f);
    DWORD b = (DWORD)((px & 0xFF) * coverage + 0.5f);
    px = (a << 24) | (r << 16) | (g << 8) | b;
}

// Real corner radius of the window in pixels (0 = square). Windows 11 rounds the
// corners at composition time; Windows 10, maximized and full-screen windows are square.
static float MacGenieCornerRadius(HWND hWnd, int w, int h) {
    if (!MacGenieIsWindows11() || IsZoomed(hWnd) || w < 64 || h < 64) return 0.0f;
    int pref = 0;   // DWMWCP_DEFAULT
    DwmGetWindowAttribute(hWnd, DWMWA_WINDOW_CORNER_PREFERENCE, &pref, sizeof(pref));
    if (pref == 1) return 0.0f;                             // DWMWCP_DONOTROUND
    float radius = (pref == 3 ? 4.0f : 8.0f) * (float)MacGenieWindowDpi(hWnd) / 96.0f;

    MONITORINFO mi = {};
    mi.cbSize = sizeof(mi);
    RECT wr;
    if (GetWindowRect(hWnd, &wr) && GetMonitorInfoW(MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST), &mi)) {
        if (wr.left <= mi.rcMonitor.left && wr.top <= mi.rcMonitor.top &&
            wr.right >= mi.rcMonitor.right && wr.bottom >= mi.rcMonitor.bottom) return 0.0f;
    }
    return radius;
}

// PrintWindow hands back square corners, so bake the rounding into the snapshot's alpha
// and it travels with the animation.
static void MacGenieRoundSnapshotCorners(DWORD* px, int w, int h, float radius) {
    if (!g_roundCorners.load(std::memory_order_relaxed) || radius <= 0.0f) return;
    if (((px[0] >> 24) & 0xFF) < 250) return;   // DWM already gave us transparent corners
    const int ri = (int)ceilf(radius);
    if (w < ri * 2 || h < ri * 2) return;
    for (int cy = 0; cy < ri; ++cy) {
        for (int cx = 0; cx < ri; ++cx) {
            float dx = radius - (cx + 0.5f);
            float dy = radius - (cy + 0.5f);
            float cov = radius - sqrtf(dx * dx + dy * dy) + 0.5f;
            if (cov >= 1.0f) continue;
            if (cov < 0.0f) cov = 0.0f;
            MacGenieScalePixel(px[cy * w + cx], cov);
            MacGenieScalePixel(px[cy * w + (w - 1 - cx)], cov);
            MacGenieScalePixel(px[(h - 1 - cy) * w + cx], cov);
            MacGenieScalePixel(px[(h - 1 - cy) * w + (w - 1 - cx)], cov);
        }
    }
}

// ---------------------------------------------------------------------------
// Dock geometry helpers (shared by the scale-to-dock effect)
// ---------------------------------------------------------------------------
static float MacGenieMonitorDpiScale(HMONITOR hMon) {
    UINT dpiX = 96, dpiY = 96;
    HMODULE hShcore = LoadLibraryExW(L"Shcore.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (hShcore) {
        typedef HRESULT (WINAPI *GetDpiForMonitor_t)(HMONITOR, int, UINT*, UINT*);
        auto fn = (GetDpiForMonitor_t)(void*)GetProcAddress(hShcore, "GetDpiForMonitor");
        if (fn && hMon) fn(hMon, 0, &dpiX, &dpiY);
        FreeLibrary(hShcore);
    }
    return dpiY ? dpiY / 96.0f : 1.0f;
}

// Y coordinate the dock effects aim at: inside the taskbar, "dock depth" percent past its edge.
static float MacGenieDockTipY(HMONITOR hMon, float dpiScale) {
    MONITORINFO mi = {};
    mi.cbSize = sizeof(mi);
    if (!hMon || !GetMonitorInfoW(hMon, &mi)) {
        mi.rcMonitor.left = 0; mi.rcMonitor.top = 0;
        mi.rcMonitor.right = GetSystemMetrics(SM_CXSCREEN);
        mi.rcMonitor.bottom = GetSystemMetrics(SM_CYSCREEN);
    }
    const float depth = g_dockDepthPct.load(std::memory_order_relaxed) / 100.0f;
    float taskbarH = 48.0f * dpiScale;
    bool barOnTop = false;
    float barEdge = (float)mi.rcMonitor.bottom - taskbarH;
    HWND hTray = FindTaskbarForMonitor(hMon);
    RECT tr;
    if (hTray && GetWindowRect(hTray, &tr)) {
        const int th = tr.bottom - tr.top, tw = tr.right - tr.left;
        if (th > 0 && tw > th) {
            barOnTop = (tr.top + tr.bottom) < (mi.rcMonitor.top + mi.rcMonitor.bottom);
            const bool parked = barOnTop ? (tr.bottom <= mi.rcMonitor.top + 8)
                                         : (tr.top >= mi.rcMonitor.bottom - 8);
            if (!parked) { taskbarH = (float)th; barEdge = barOnTop ? (float)tr.bottom : (float)tr.top; }
            else barEdge = barOnTop ? (float)mi.rcMonitor.top + taskbarH : (float)mi.rcMonitor.bottom - taskbarH;
        }
    }
    return barOnTop ? barEdge - taskbarH * depth : barEdge + taskbarH * depth;
}

// Soft drop shadow made of stacked translucent rounded rectangles (no blur effect needed).
// Real windows have a DWM shadow that the snapshot lacks; without this the shadow would
// pop in / out at the first / last frame.
static void MacGenieDrawShadow(ID2D1RenderTarget* rt, ID2D1SolidColorBrush* brush,
                               const D2D1_RECT_F& r, float radius, float dpiScale, float opacity) {
    const int layers = 10;
    const float extent = 26.0f * dpiScale;
    const float dy = 9.0f * dpiScale;
    brush->SetOpacity(0.034f * opacity);
    for (int i = layers; i >= 1; --i) {
        float g = extent * (float)i / (float)layers;
        D2D1_ROUNDED_RECT rr = D2D1::RoundedRect(
            D2D1::RectF(r.left - g, r.top - g + dy, r.right + g, r.bottom + g + dy),
            radius + g, radius + g);
        rt->FillRoundedRectangle(rr, brush);
    }
}

// ---------------------------------------------------------------------------
// POP / SCALE engine (one transform + opacity per frame, cheap and exact)
//   style 1 = pop:   the real macOS open / close - window scales slightly and fades
//   style 2 = scale: the macOS "Scale effect" minimize - window shrinks onto its taskbar button
// ---------------------------------------------------------------------------
DWORD WINAPI MacGeniePopThread(LPVOID lpParam) {
    MacGenieAnimData* data = (MacGenieAnimData*)lpParam;
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST);

    const bool toDock = (data->style == 2);
    const int W = data->width;
    const int H = data->height;
    const float ds = data->dpiScale > 0.0f ? data->dpiScale : 1.0f;
    const float minScale = data->popScale;
    const float maxScale = toDock ? 1.0f : fmaxf(1.0f, minScale + (1.0f - minScale) * 1.3f);
    const int shadowPad = data->shadow ? (int)ceilf(36.0f * ds) : 2;

    // Canvas: the window rect (+ the dock target for the scale effect) plus margins.
    RECT area = data->targetRect;
    float dockCx = 0.0f, dockCy = 0.0f;
    if (toDock) {
        dockCx = (float)data->targetDockX;
        dockCy = MacGenieDockTipY(data->hMon, ds);
        const int r = (int)(24.0f * ds);
        RECT dockRc = { (int)dockCx - r, (int)dockCy - r, (int)dockCx + r, (int)dockCy + r };
        UnionRect(&area, &area, &dockRc);
    }
    const int padX = (int)ceilf(W * (maxScale - 1.0f) * 0.5f) + shadowPad;
    const int padY = (int)ceilf(H * (maxScale - 1.0f) * 0.5f) + shadowPad;
    const int vLeft = area.left - padX;
    const int vTop = area.top - padY;
    const int vWidth = (area.right - area.left) + padX * 2;
    const int vHeight = (area.bottom - area.top) + padY * 2;

    HDC hScreenDC = GetDC(NULL);
    HDC hMemDC = CreateCompatibleDC(hScreenDC);

    BITMAPINFO bmi = {};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = vWidth;
    bmi.bmiHeader.biHeight = -vHeight;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;
    void* pTargetBits = nullptr;
    HBITMAP hTargetBmp = CreateDIBSection(hScreenDC, &bmi, DIB_RGB_COLORS, &pTargetBits, NULL, 0);
    HBITMAP hOldBmp = (HBITMAP)SelectObject(hMemDC, hTargetBmp);

    HWND hGhost = CreateWindowExW(
        WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_NOACTIVATE,
        L"STATIC", NULL, WS_POPUP,
        vLeft, vTop, vWidth, vHeight,
        NULL, NULL, NULL, NULL);

    ID2D1DCRenderTarget* rt = nullptr;
    ID2D1Bitmap* snapshotBmp = nullptr;
    ID2D1SolidColorBrush* shadowBrush = nullptr;
    if (g_d2dFactory) {
        D2D1_RENDER_TARGET_PROPERTIES rtProps = D2D1::RenderTargetProperties(
            D2D1_RENDER_TARGET_TYPE_DEFAULT,
            D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED),
            0, 0, D2D1_RENDER_TARGET_USAGE_GDI_COMPATIBLE, D2D1_FEATURE_LEVEL_DEFAULT);
        g_d2dFactory->CreateDCRenderTarget(&rtProps, &rt);
        if (rt) {
            D2D1_BITMAP_PROPERTIES bmpProps = D2D1::BitmapProperties(
                D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED));
            rt->CreateBitmap(D2D1::SizeU(W, H), data->pBits, W * 4, bmpProps, &snapshotBmp);
            if (data->shadow) rt->CreateSolidColorBrush(D2D1::ColorF(0, 0, 0, 1.0f), &shadowBrush);
        }
    }
    if (snapshotBmp) {
        // Direct2D owns a copy now; release the GDI one early to halve the memory.
        DeleteObject(data->hBitmap);
        data->hBitmap = NULL;
        data->pBits = nullptr;
    }
    const bool d2dOk = (rt && snapshotBmp && hGhost);

    const double animDur = (double)data->durationMs;
    LARGE_INTEGER qpcFreq, qpcStart, qpcNow;
    QueryPerformanceFrequency(&qpcFreq);
    QueryPerformanceCounter(&qpcStart);
    const double frameIntervalMs = MacGenieFrameIntervalMs();
    HANDLE hFrameTimer = MacGenieCreateFrameTimer();

    const float winL = (float)(data->targetRect.left - vLeft);
    const float winT = (float)(data->targetRect.top - vTop);
    const D2D1_RECT_F dest = D2D1::RectF(winL, winT, winL + W, winT + H);
    const D2D1_POINT_2F c0 = D2D1::Point2F(winL + W * 0.5f, winT + H * 0.5f);
    const D2D1_POINT_2F cd = D2D1::Point2F(dockCx - vLeft, dockCy - vTop);
    const float endScale = fmaxf(0.03f, (44.0f * ds) / (float)(W > H ? W : H));
    BOOL firstFrame = TRUE;

    if (d2dOk) {
        for (;;) {
            QueryPerformanceCounter(&qpcNow);
            const LARGE_INTEGER qpcFrameStart = qpcNow;
            double elapsedMs = (qpcNow.QuadPart - qpcStart.QuadPart) * 1000.0 / qpcFreq.QuadPart;
            const BOOL lastFrame = (elapsedMs >= animDur);
            const float raw = (float)fmin(elapsedMs / animDur, 1.0);

            float scale, alpha, dx = 0.0f, dy = 0.0f;
            if (toDock) {
                float e = MacGenieEase(data->isRising ? g_easeRestore.load(std::memory_order_relaxed)
                                                      : g_easeMinimize.load(std::memory_order_relaxed), raw);
                float t = data->isRising ? (1.0f - e) : e;          // 0 = at the window, 1 = on the button
                if (t < 0.0f) t = 0.0f;
                if (t > 1.0f) t = 1.0f;
                scale = 1.0f + (endScale - 1.0f) * t;
                dx = (cd.x - c0.x) * t;
                dy = (cd.y - c0.y) * t;
                float f = (t - 0.75f) / 0.25f;                      // fade only while arriving
                if (f < 0.0f) f = 0.0f;
                if (f > 1.0f) f = 1.0f;
                alpha = 1.0f - f * f * (3.0f - 2.0f * f);
            } else if (data->isClosing) {
                // Shrink quickly, fade with a gentle in-out so the window lingers a moment.
                scale = 1.0f - (1.0f - minScale) * MacGenieEase(2, raw);
                alpha = 1.0f - MacGenieEase(1, raw);
            } else {
                // Open: settle out of the smaller size; opacity arrives a bit faster than size.
                scale = minScale + (1.0f - minScale) * MacGenieEase(data->popEasing, raw);
                alpha = MacGenieEase(2, fminf(raw * 1.6f, 1.0f));
            }
            if (alpha < 0.0f) alpha = 0.0f;
            if (alpha > 1.0f) alpha = 1.0f;

            RECT bindRect = { 0, 0, vWidth, vHeight };
            rt->BindDC(hMemDC, &bindRect);
            rt->BeginDraw();
            rt->Clear(D2D1::ColorF(0, 0, 0, 0));
            rt->SetTransform(D2D1::Matrix3x2F::Scale(scale, scale, c0) * D2D1::Matrix3x2F::Translation(dx, dy));
            if (shadowBrush) MacGenieDrawShadow(rt, shadowBrush, dest, data->cornerRadius, ds, alpha);
            rt->DrawBitmap(snapshotBmp, dest, alpha, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
            rt->SetTransform(D2D1::Matrix3x2F::Identity());
            rt->EndDraw();

            POINT ptDst = { vLeft, vTop };
            SIZE sz = { vWidth, vHeight };
            POINT ptSrc = { 0, 0 };
            BLENDFUNCTION bf = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
            UpdateLayeredWindow(hGhost, hScreenDC, &ptDst, &sz, hMemDC, &ptSrc, 0, &bf, ULW_ALPHA);
            if (firstFrame) ShowWindow(hGhost, SW_SHOWNOACTIVATE);

            if (lastFrame) break;
            if (g_unloading.load(std::memory_order_relaxed)) break;

            if (firstFrame) {
                DwmFlush();
                firstFrame = FALSE;
                if (data->hFirstFrameShown) SetEvent(data->hFirstFrameShown);
            }
            MacGenieWaitNextFrame(hFrameTimer, frameIntervalMs, qpcFrameStart, qpcFreq);
        }
    }

    if (hFrameTimer) { CloseHandle(hFrameTimer); hFrameTimer = nullptr; }

    if (data->isRising) {
        if (data->hiddenByCloak) {
            MacGenieSetCloak(data->hRealWnd, FALSE);
        } else {
            SetLayeredWindowAttributes(data->hRealWnd, 0, 255, LWA_ALPHA);
            if (!(data->originalExStyle & WS_EX_LAYERED)) {
                SetWindowLongPtrW(data->hRealWnd, GWL_EXSTYLE, data->originalExStyle);
            }
        }
    }
    if (!data->deferredMinimize && !data->isClosing) MacGenieSetDwmTransitions(data->hRealWnd, TRUE);
    if (data->isClosing) MacGenieFinishClose(data);

    if (shadowBrush) { shadowBrush->Release(); shadowBrush = nullptr; }
    if (snapshotBmp) { snapshotBmp->Release(); snapshotBmp = nullptr; }
    if (rt) { rt->Release(); rt = nullptr; }
    SelectObject(hMemDC, hOldBmp);
    DeleteObject(hTargetBmp);
    if (data->hBitmap) DeleteObject(data->hBitmap);
    DeleteDC(hMemDC);
    ReleaseDC(NULL, hScreenDC);
    if (hGhost) DestroyWindow(hGhost);

    if (data->hFirstFrameShown) {
        SetEvent(data->hFirstFrameShown);
        CloseHandle(data->hFirstFrameShown);
    }
    MacGenieRunDeferredMinimize(data);
    {
        std::lock_guard<std::mutex> lock(g_CacheMutex);
        g_AnimActive.erase(data->hRealWnd);
    }
    delete data;
    return 0;
}

bool StartMacGenieAnim(HWND hWnd, BOOL rising, LONG_PTR originalExStyle,
                       BOOL cloakHidden = FALSE, BOOL deferredMinimize = FALSE,
                       BOOL requestedUnhide = FALSE, HWND hNextApp = NULL,
                       BOOL isClosing = FALSE, UINT closeMsg = WM_DESTROY,
                       BOOL isLaunch = FALSE) {
    {
        std::lock_guard<std::mutex> lock(g_CacheMutex);
        for (auto it = g_SnapshotCache.begin(); it != g_SnapshotCache.end();) {
            if (!IsWindow(it->first)) {
                DeleteObject(it->second.hBmp);
                it = g_SnapshotCache.erase(it);
            } else {
                ++it;
            }
        }
    }

    if (MacGenieIsExcluded(hWnd) || !MacGenieMotionAllowed()) {
        if (rising) MacGenieUndoRisingHide(hWnd, originalExStyle, cloakHidden);
        else MacGenieSetDwmTransitions(hWnd, TRUE);
        return false;
    }

    RECT winRect;
    if (!GetWindowRect(hWnd, &winRect)) {
        if (rising) MacGenieUndoRisingHide(hWnd, originalExStyle, cloakHidden);
        else MacGenieSetDwmTransitions(hWnd, TRUE);
        return false;
    }
    RECT rect = winRect, extRect;
    if (SUCCEEDED(DwmGetWindowAttribute(hWnd, DWMWA_EXTENDED_FRAME_BOUNDS, &extRect, sizeof(extRect)))) {
        rect = extRect;
    }
    int w = rect.right - rect.left;
    int h = rect.bottom - rect.top;
    int offsetX = rect.left - winRect.left;
    int offsetY = rect.top - winRect.top;
    int rawW = winRect.right - winRect.left;
    int rawH = winRect.bottom - winRect.top;

    if (w <= 0 || h <= 0 || rawW <= 0 || rawH <= 0) {
        if (rising) MacGenieUndoRisingHide(hWnd, originalExStyle, cloakHidden);
        else MacGenieSetDwmTransitions(hWnd, TRUE);
        return false;
    }

    bool blocked = g_unloading.load(std::memory_order_relaxed);
    if (!blocked) {
        std::lock_guard<std::mutex> lock(g_CacheMutex);
        blocked = !g_AnimActive.insert(hWnd).second;
        if (!blocked) g_LaunchSeen.insert(hWnd);
    }
    if (blocked) {
        if (rising) {
            MacGenieUndoRisingHide(hWnd, originalExStyle, cloakHidden);
        } else {
            bool owned;
            {
                std::lock_guard<std::mutex> lock(g_CacheMutex);
                owned = g_AnimActive.count(hWnd) != 0;
            }
            if (!owned) MacGenieSetDwmTransitions(hWnd, TRUE);
        }
        return false;
    }

    // 0 = genie (mesh warp), 1 = pop (scale + fade), 2 = scale-to-dock (macOS Scale effect).
    int style = 0;
    if (isClosing) style = g_closeStyle.load(std::memory_order_relaxed);
    else if (isLaunch) style = g_launchStyle.load(std::memory_order_relaxed);
    else style = g_minimizeEffect.load(std::memory_order_relaxed);   // genie (0) or scale (2)

    // Dialogs / alerts get their own springier pop, like NSWindow's alert-panel behaviour.
    bool isAlert = false;
    if (isLaunch && g_alertPop.load(std::memory_order_relaxed) && style == 1) {
        WCHAR cls[64] = {0};
        GetClassNameW(hWnd, cls, ARRAYSIZE(cls));
        isAlert = GetWindow(hWnd, GW_OWNER) != NULL || _wcsicmp(cls, L"#32770") == 0;
    }

    HMONITOR hMon = isClosing ? MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST)
                              : MacGenieResolveDockMonitor(hWnd);
    MONITORINFO mi;
    mi.cbSize = sizeof(mi);
    if (!GetMonitorInfoW(hMon, &mi)) {
        mi.rcMonitor.left = 0; mi.rcMonitor.top = 0;
        mi.rcMonitor.right = GetSystemMetrics(SM_CXSCREEN);
        mi.rcMonitor.bottom = GetSystemMetrics(SM_CYSCREEN);
    }
    int monWidth = mi.rcMonitor.right - mi.rcMonitor.left;

    int learnedTargetX = mi.rcMonitor.left + monWidth / 2;
    if (!isClosing && style != 1) {
        DWORD alignVal = 1, dataSize = sizeof(alignVal);
        RegGetValueW(HKEY_CURRENT_USER,
                     L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced",
                     L"TaskbarAl", RRF_RT_REG_DWORD, NULL, &alignVal, &dataSize);
        learnedTargetX = (alignVal == 0) ? (mi.rcMonitor.left + 160)
                                         : (mi.rcMonitor.left + monWidth / 2);

        POINT pt;
        GetCursorPos(&pt);
        RECT workArea;
        MONITORINFO cursorMi;
        cursorMi.cbSize = sizeof(cursorMi);
        if (GetMonitorInfoW(MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST), &cursorMi)) {
            workArea = cursorMi.rcWork;
        } else {
            SystemParametersInfoW(SPI_GETWORKAREA, 0, &workArea, 0);
        }

        // Cursor on the taskbar of THIS animation's monitor = the user clicked the button itself.
        if (!PtInRect(&workArea, pt) && MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST) == hMon) {
            learnedTargetX = pt.x;
            std::lock_guard<std::mutex> lock(g_CacheMutex);
            g_IconPositions[hWnd] = learnedTargetX;
        } else {
            learnedTargetX = GetTaskbarButtonX(hWnd, learnedTargetX, hMon);
        }
    }

    MacGenieAnimData* data = new MacGenieAnimData();
    data->hRealWnd = hWnd;
    data->targetRect = rect;
    data->hMon = hMon;
    data->width = w;
    data->height = h;
    data->isRising = rising;
    data->targetDockX = learnedTargetX;
    data->originalExStyle = originalExStyle;
    data->hiddenByCloak = cloakHidden;
    data->hFirstFrameShown = NULL;
    int durationMs = (style == 1)
        ? (isClosing ? g_popCloseMs.load(std::memory_order_relaxed)
                     : g_popOpenMs.load(std::memory_order_relaxed))
        : g_durationMs.load(std::memory_order_relaxed);
    // macOS: Shift = much slower, Ctrl = slow, Ctrl+Shift = slowest (genie and scale).
    if (style != 1 && g_slowMoShift.load(std::memory_order_relaxed)) {
        const bool shift = (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0;
        const bool ctrl = (GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0;
        const int f = g_slowMoFactor.load(std::memory_order_relaxed);
        int mult = 1;
        if (shift && ctrl) mult = f * 2;
        else if (shift) mult = f;
        else if (ctrl) mult = (f + 1) / 2 < 2 ? 2 : (f + 1) / 2;
        durationMs *= mult;
        if (durationMs > 20000) durationMs = 20000;
    }
    data->durationMs = durationMs;
    data->style = style;
    data->popScale = g_popScalePct.load(std::memory_order_relaxed) / 100.0f;
    data->popEasing = g_popEasing.load(std::memory_order_relaxed);
    if (isAlert) {
        data->popEasing = 5;                                   // spring
        if (data->popScale > 0.88f) data->popScale = 0.88f;    // a bit more travel so the overshoot reads
    }
    data->dpiScale = (float)MacGenieWindowDpi(hWnd) / 96.0f;
    data->shadow = (style != 0) && g_windowShadow.load(std::memory_order_relaxed);
    data->cornerRadius = MacGenieCornerRadius(hWnd, w, h);
    data->requestedUnhide = requestedUnhide;
    data->hNextApp = hNextApp;
    data->unhideDurationMs = g_unhideDurationMs.load(std::memory_order_relaxed);
    data->deferredMinimize = deferredMinimize;
    data->isClosing = isClosing;
    data->closeMsg = closeMsg;
    data->pBits = nullptr;
    data->hBitmap = NULL;

    HDC hScreenDC = GetDC(NULL);

    BITMAPINFO bmi = {{0}};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = w;
    bmi.bmiHeader.biHeight = -h;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;
    data->hBitmap = CreateDIBSection(hScreenDC, &bmi, DIB_RGB_COLORS, &data->pBits, NULL, 0);

    if (!data->hBitmap || !data->pBits) {
        ReleaseDC(NULL, hScreenDC);
        if (rising) MacGenieUndoRisingHide(hWnd, originalExStyle, cloakHidden);
        else MacGenieSetDwmTransitions(hWnd, TRUE);
        if (data->hBitmap) DeleteObject(data->hBitmap);
        delete data;
        std::lock_guard<std::mutex> lock(g_CacheMutex);
        g_AnimActive.erase(hWnd);
        return false;
    }

    auto CopySnapshot = [&](void* srcBits) {
        DWORD* src = (DWORD*)srcBits;
        DWORD* dst = (DWORD*)data->pBits;
        int inBounds = 0, zeroAlpha = 0;
        for (int y = 0; y < h; y++) {
            for (int x = 0; x < w; x++) {
                int sx = x + offsetX;
                int sy = y + offsetY;
                if (sx >= 0 && sx < rawW && sy >= 0 && sy < rawH) {
                    DWORD p = src[sy * rawW + sx];
                    BYTE a = (BYTE)((p >> 24) & 0xFF);
                    inBounds++;
                    if (a == 0) zeroAlpha++;
                    dst[y * w + x] = (a == 0) ? 0 : p;
                } else {
                    dst[y * w + x] = 0;
                }
            }
        }
        if (inBounds > 0 && zeroAlpha * 2 > inBounds) {
            for (int y = 0; y < h; y++) {
                for (int x = 0; x < w; x++) {
                    int sx = x + offsetX;
                    int sy = y + offsetY;
                    if (sx >= 0 && sx < rawW && sy >= 0 && sy < rawH) {
                        DWORD p = src[sy * rawW + sx];
                        if (((p >> 24) & 0xFF) == 0) {
                            dst[y * w + x] = 0xFF000000u | (p & 0x00FFFFFFu);
                        }
                    }
                }
            }
        }
    };

    auto CaptureNow = [&]() -> bool {
        HDC hTempDC = CreateCompatibleDC(hScreenDC);
        BITMAPINFO bmiTemp = bmi;
        bmiTemp.bmiHeader.biWidth = rawW;
        bmiTemp.bmiHeader.biHeight = -rawH;
        void* pTempBits = nullptr;
        HBITMAP hTempBmp = CreateDIBSection(hScreenDC, &bmiTemp, DIB_RGB_COLORS, &pTempBits, NULL, 0);
        if (!hTempBmp || !pTempBits) {
            if (hTempBmp) DeleteObject(hTempBmp);
            DeleteDC(hTempDC);
            return false;
        }
        HBITMAP hOldTempBmp = (HBITMAP)SelectObject(hTempDC, hTempBmp);
        BOOL screenGrab = isClosing && GetForegroundWindow() == hWnd &&
                          BitBlt(hTempDC, 0, 0, rawW, rawH, hScreenDC,
                                 winRect.left, winRect.top, SRCCOPY);
        if (!screenGrab) PrintWindow(hWnd, hTempDC, PW_RENDERFULLCONTENT);
        GdiFlush();
        CopySnapshot(pTempBits);
        data->cornerRadius = MacGenieCornerRadius(hWnd, w, h);
        MacGenieRoundSnapshotCorners((DWORD*)data->pBits, w, h, data->cornerRadius);
        SelectObject(hTempDC, hOldTempBmp);
        DeleteObject(hTempBmp);
        DeleteDC(hTempDC);
        return true;
    };

    if (rising) {
        BOOL fromCache = FALSE;
        {
            std::lock_guard<std::mutex> lock(g_CacheMutex);
            auto it = g_SnapshotCache.find(hWnd);
            if (it != g_SnapshotCache.end()) {
                SnapCache& c = it->second;
                if (c.w == w && c.h == h) {
                    memcpy(data->pBits, c.pBits, (size_t)w * h * 4);
                    fromCache = TRUE;
                }
                DeleteObject(c.hBmp);
                g_SnapshotCache.erase(it);
            }
        }
        if (!fromCache) CaptureNow();
    } else {
        CaptureNow();
        std::lock_guard<std::mutex> lock(g_CacheMutex);
        auto it = g_SnapshotCache.find(hWnd);
        if (it != g_SnapshotCache.end()) {
            DeleteObject(it->second.hBmp);
            g_SnapshotCache.erase(it);
        }
        // Remember the snapshot for the restore animation, within a memory budget:
        // least recently used entries are dropped first, and a window that alone
        // exceeds the budget is simply re-captured on restore.
        const size_t bytes = (size_t)w * h * 4;
        const size_t budget = (size_t)g_cacheBudgetMb.load(std::memory_order_relaxed) * 1024u * 1024u;
        void* pCacheBits = nullptr;
        HBITMAP hCacheBmp = NULL;
        if (!isClosing && !g_unloading.load(std::memory_order_relaxed) && bytes <= budget) {
            for (;;) {
                size_t total = bytes;
                auto oldest = g_SnapshotCache.end();
                for (auto c = g_SnapshotCache.begin(); c != g_SnapshotCache.end(); ++c) {
                    total += (size_t)c->second.w * c->second.h * 4;
                    if (oldest == g_SnapshotCache.end() || c->second.tick < oldest->second.tick) oldest = c;
                }
                if (total <= budget || oldest == g_SnapshotCache.end()) break;
                DeleteObject(oldest->second.hBmp);
                g_SnapshotCache.erase(oldest);
            }
            hCacheBmp = CreateDIBSection(hScreenDC, &bmi, DIB_RGB_COLORS, &pCacheBits, NULL, 0);
        }
        if (hCacheBmp && pCacheBits) {
            memcpy(pCacheBits, data->pBits, bytes);
            g_SnapshotCache[hWnd] = { hCacheBmp, pCacheBits, w, h, GetTickCount64() };
        } else if (hCacheBmp) {
            DeleteObject(hCacheBmp);
        }
    }

    ReleaseDC(NULL, hScreenDC);

    HANDLE hFirstShown = NULL;
    if (!rising) {
        hFirstShown = CreateEventW(NULL, TRUE, FALSE, NULL);
        if (hFirstShown &&
            !DuplicateHandle(GetCurrentProcess(), hFirstShown, GetCurrentProcess(),
                             &data->hFirstFrameShown, 0, FALSE, DUPLICATE_SAME_ACCESS)) {
            data->hFirstFrameShown = NULL;
        }
    }
    bool waitForFirstFrame = (data->hFirstFrameShown != NULL);

    if (style != 0 || !g_classicEngine.load(std::memory_order_relaxed)) MacGenieEnsureD2D();
    LPTHREAD_START_ROUTINE renderProc;
    if (style != 0)                                        renderProc = MacGeniePopThread;
    else if (g_classicEngine.load(std::memory_order_relaxed)) renderProc = MacGenieAnimThreadClassic;
    else if (g_gpuEngine.load(std::memory_order_relaxed) && !g_gpuBroken.load(std::memory_order_relaxed))
                                                           renderProc = MacGenieAnimThreadGpu;
    else                                                   renderProc = MacGenieAnimThread;
    if (MacGenieSpawnWorker(renderProc, data)) {
        if (hFirstShown) {
            if (waitForFirstFrame) {
                WaitForSingleObject(hFirstShown, 200);
            }
            CloseHandle(hFirstShown);
        }
        return true;
    }

    if (rising) {
        MacGenieUndoRisingHide(hWnd, data->originalExStyle, data->hiddenByCloak);
    } else {
        MacGenieSetDwmTransitions(hWnd, TRUE);
    }
    if (hFirstShown) CloseHandle(hFirstShown);
    if (data->hFirstFrameShown) CloseHandle(data->hFirstFrameShown);
    DeleteObject(data->hBitmap);
    delete data;
    {
        std::lock_guard<std::mutex> lock(g_CacheMutex);
        g_AnimActive.erase(hWnd);
    }
    return false;
}

DWORD WINAPI MacGenieLaunchThread(LPVOID lpParam);

struct MacGenieGeckoSearch { HWND self; DWORD pid; bool found; };

static BOOL CALLBACK MacGenieFindOtherGeckoWindow(HWND w, LPARAM lp) {
    MacGenieGeckoSearch* s = (MacGenieGeckoSearch*)lp;
    if (w == s->self || (!IsWindowVisible(w) && !IsIconic(w)) ||
        GetAncestor(w, GA_ROOT) != w) {
        return TRUE;
    }
    DWORD pid = 0;
    GetWindowThreadProcessId(w, &pid);
    WCHAR cls[64];
    if (pid == s->pid && GetClassNameW(w, cls, ARRAYSIZE(cls)) &&
        _wcsicmp(cls, L"MozillaWindowClass") == 0) {
        s->found = true;
        return FALSE;
    }
    return TRUE;
}

static bool MacGenieIsFirstGeckoWindow(HWND hWnd) {
    WCHAR cls[64];
    if (!GetClassNameW(hWnd, cls, ARRAYSIZE(cls)) ||
        _wcsicmp(cls, L"MozillaWindowClass") != 0) {
        return false;
    }
    DWORD pid = 0;
    GetWindowThreadProcessId(hWnd, &pid);
    MacGenieGeckoSearch search = { hWnd, pid, false };
    EnumWindows(MacGenieFindOtherGeckoWindow, (LPARAM)&search);
    return !search.found;
}

static bool MacGenieLaunchPrepare(HWND hWnd, int nCmdShow, LONG_PTR* origExOut,
                                  BOOL* cloakedOut) {
    if (g_unloading.load(std::memory_order_relaxed)) return false;
    if (!g_launchAnimation.load(std::memory_order_relaxed)) return false;
    if (nCmdShow != SW_SHOW && nCmdShow != SW_SHOWNORMAL &&
        nCmdShow != SW_SHOWDEFAULT && nCmdShow != SW_SHOWMAXIMIZED) return false;
    if (IsWindowVisible(hWnd) || IsIconic(hWnd)) return false;
    if (!MacGenieIsLaunchWindow(hWnd)) return false;
    {
        std::lock_guard<std::mutex> lock(g_CacheMutex);
        if (!g_LaunchSeen.insert(hWnd).second) return false;
    }
    if (MacGenieIsExcluded(hWnd)) return false;
    LONG_PTR exStyle = GetWindowLongPtrW(hWnd, GWL_EXSTYLE);
    *origExOut = exStyle;
    MacGenieSetDwmTransitions(hWnd, FALSE);

    UINT cloaked = 0;
    bool appCloaked =
        SUCCEEDED(DwmGetWindowAttribute(hWnd, DWMWA_CLOAKED, &cloaked, sizeof(cloaked))) &&
        (cloaked & DWM_CLOAKED_APP);
    BOOL cloak = TRUE;
    if (!appCloaked && !MacGenieIsFirstGeckoWindow(hWnd) &&
        SUCCEEDED(DwmSetWindowAttribute(hWnd, DWMWA_CLOAK, &cloak, sizeof(cloak)))) {
        *cloakedOut = TRUE;
        return true;
    }
    SetWindowLongPtrW(hWnd, GWL_EXSTYLE, exStyle | WS_EX_LAYERED);
    SetLayeredWindowAttributes(hWnd, 0, 0, LWA_ALPHA);
    *cloakedOut = FALSE;
    return true;
}

static void MacGenieLaunchCommit(HWND hWnd, LONG_PTR originalExStyle, BOOL cloaked) {
    MacGenieLaunchData* ld = new MacGenieLaunchData();
    ld->hWnd = hWnd;
    ld->originalExStyle = originalExStyle;
    ld->hiddenByCloak = cloaked;
    if (!MacGenieSpawnWorker(MacGenieLaunchThread, ld)) {
        delete ld;
        MacGenieUndoRisingHide(hWnd, originalExStyle, cloaked);
    }
}

static bool MacGenieIsCloaked(HWND hWnd) {
    UINT cloaked = 0;
    return SUCCEEDED(DwmGetWindowAttribute(hWnd, DWMWA_CLOAKED, &cloaked, sizeof(cloaked))) &&
           cloaked;
}

static void MacGenieClearCloseHandled(HWND hWnd) {
    if (GetPropW(hWnd, L"GenieCloseHandled")) RemovePropW(hWnd, L"GenieCloseHandled");
}

static bool MacGenieMarkCloseAndCloak(HWND hWnd) {
    std::lock_guard<std::mutex> lock(g_WorkerThreadsMutex);
    if (g_unloading.load(std::memory_order_relaxed)) return false;
    SetPropW(hWnd, L"GenieCloseHandled", (HANDLE)1);
    MacGenieSetCloak(hWnd, TRUE);
    return true;
}

static void MacGenieWaitCloseAnim(HWND hWnd, DWORD timeoutMs) {
    const DWORD start = GetTickCount();
    while (IsWindow(hWnd) && !g_unloading.load(std::memory_order_relaxed)) {
        {
            std::lock_guard<std::mutex> lock(g_CacheMutex);
            if (!g_AnimActive.count(hWnd)) break;
        }
        const DWORD elapsed = GetTickCount() - start;
        if (elapsed >= timeoutMs) break;
        DWORD slice = timeoutMs - elapsed;
        if (slice > 10) slice = 10;
        if (MsgWaitForMultipleObjectsEx(0, nullptr, slice, QS_SENDMESSAGE,
                                        MWMO_INPUTAVAILABLE) == WAIT_OBJECT_0) {
            MSG msg;
            PeekMessageW(&msg, nullptr, 0, 0, PM_NOREMOVE | PM_QS_SENDMESSAGE);
        }
    }
}

struct MacGenieOtherWindowSearch { HWND self; DWORD pid; bool found; };

static BOOL CALLBACK MacGenieFindOtherAppWindow(HWND w, LPARAM lp) {
    MacGenieOtherWindowSearch* s = (MacGenieOtherWindowSearch*)lp;
    if (w == s->self || !IsWindowVisible(w) || GetAncestor(w, GA_ROOT) != w ||
        GetWindow(w, GW_OWNER) ||
        (GetWindowLongPtrW(w, GWL_EXSTYLE) & WS_EX_TOOLWINDOW)) {
        return TRUE;
    }
    DWORD pid = 0;
    GetWindowThreadProcessId(w, &pid);
    if (pid == s->pid && !MacGenieIsCloaked(w)) {
        s->found = true;
        return FALSE;
    }
    return TRUE;
}

static bool MacGenieHasOtherAppWindow(HWND hWnd) {
    MacGenieOtherWindowSearch search = { hWnd, GetCurrentProcessId(), false };
    EnumWindows(MacGenieFindOtherAppWindow, (LPARAM)&search);
    return search.found;
}

static bool MacGenieStartClose(HWND hWnd, UINT closeMsg) {
    MacGenieSetDwmTransitions(hWnd, FALSE);
    SetPropW(hWnd, L"GenieCloseAnimating", (HANDLE)1);
    if (StartMacGenieAnim(hWnd, FALSE, GetWindowLongPtrW(hWnd, GWL_EXSTYLE),
                          FALSE, FALSE, FALSE, NULL, TRUE, closeMsg)) {
        if (MacGenieMarkCloseAndCloak(hWnd)) return true;
        RemovePropW(hWnd, L"GenieCloseAnimating");
        return false;
    }
    RemovePropW(hWnd, L"GenieCloseAnimating");
    MacGenieSetDwmTransitions(hWnd, TRUE);
    return false;
}

static bool MacGenieIsPopupClass(const WCHAR* cls) {
    static const WCHAR* const kClasses[] = {
        L"Xaml_WindowedPopupClass", L"Popup", L"Overlay", L"ToolTip",
        L"CoreWindow", L"ApplicationFrameWindow", L"XamlExplorerHostIslandWindow",
        L"Windows.Internal.Shell.TabProxyWindow",
    };
    for (const WCHAR* c : kClasses) {
        if (_wcsicmp(cls, c) == 0) return true;
    }
    return false;
}

static bool MacGenieIsExplorerProcess() {
    static const bool isExplorer = [] {
        WCHAR path[MAX_PATH];
        DWORD len = GetModuleFileNameW(NULL, path, MAX_PATH);
        if (!len || len >= MAX_PATH) return false;
        const WCHAR* name = wcsrchr(path, L'\\');
        return _wcsicmp(name ? name + 1 : path, L"explorer.exe") == 0;
    }();
    return isExplorer;
}

static bool MacGenieIsBlockedShellWindow(const WCHAR* cls) {
    return MacGenieIsExplorerProcess() &&
           _wcsicmp(cls, L"CabinetWClass") != 0 && _wcsicmp(cls, L"ExploreWClass") != 0;
}

static bool MacGenieIsCloseTarget(HWND hWnd) {
    if (!g_closeAnimation.load(std::memory_order_relaxed)) return false;
    if (!hWnd || !IsWindowVisible(hWnd) || IsIconic(hWnd) ||
        GetAncestor(hWnd, GA_ROOT) != hWnd ||
        GetWindowThreadProcessId(hWnd, nullptr) != GetCurrentThreadId()) {
        return false;
    }
    LONG_PTR style = GetWindowLongPtrW(hWnd, GWL_STYLE);
    LONG_PTR exStyle = GetWindowLongPtrW(hWnd, GWL_EXSTYLE);
    if ((style & WS_CHILD) || (exStyle & WS_EX_TOOLWINDOW) || GetWindow(hWnd, GW_OWNER)) {
        return false;
    }
    WCHAR cls[128];
    if (!GetClassNameW(hWnd, cls, ARRAYSIZE(cls)) || MacGenieIsPopupClass(cls) ||
        MacGenieIsBlockedShellWindow(cls)) {
        return false;
    }
    if (MacGenieIsCloaked(hWnd)) return false;
    RECT r;
    return GetWindowRect(hWnd, &r) && r.right - r.left >= 300 && r.bottom - r.top >= 300;
}

static bool MacGenieIsCloseMessageClass(HWND hWnd) {
    WCHAR cls[128];
    if (!hWnd || !GetClassNameW(hWnd, cls, ARRAYSIZE(cls))) return false;
    return _wcsicmp(cls, L"ConsoleWindowClass") == 0 ||
           _wcsicmp(cls, L"CASCADIA_HOSTING_WINDOW_CLASS") == 0 ||
           _wcsicmp(cls, L"TaskManagerWindow") == 0 ||
           _wcsicmp(cls, L"WinUIDesktopWin32WindowClass") == 0;
}

static bool MacGenieShouldTreatHideAsClose(HWND hWnd) {
    if (!g_closeAnimation.load(std::memory_order_relaxed) ||
        !g_hideAsClose.load(std::memory_order_relaxed)) {
        return false;
    }
    if (!IsWindow(hWnd) || !IsWindowVisible(hWnd) || IsIconic(hWnd) ||
        GetAncestor(hWnd, GA_ROOT) != hWnd) {
        return false;
    }
    DWORD pid = 0;
    GetWindowThreadProcessId(hWnd, &pid);
    if (pid != GetCurrentProcessId()) return false;
    LONG_PTR style = GetWindowLongPtrW(hWnd, GWL_STYLE);
    LONG_PTR exStyle = GetWindowLongPtrW(hWnd, GWL_EXSTYLE);
    if ((style & WS_CHILD) || (exStyle & WS_EX_TOOLWINDOW) ||
        GetWindow(hWnd, GW_OWNER) || MacGenieIsCloseMessageClass(hWnd)) {
        return false;
    }
    WCHAR cls[128];
    if (!GetClassNameW(hWnd, cls, ARRAYSIZE(cls)) || MacGenieIsPopupClass(cls) ||
        MacGenieIsBlockedShellWindow(cls)) {
        return false;
    }
    if (MacGenieIsCloaked(hWnd)) return false;
    RECT r;
    return GetWindowRect(hWnd, &r) && r.right - r.left >= 300 && r.bottom - r.top >= 300;
}

static bool MacGenieCloseBlocked(HWND hWnd) {
    return GetPropW(hWnd, L"GenieCloseBypass") || GetPropW(hWnd, L"GenieCloseHandled") ||
           GetPropW(hWnd, L"GenieCloseAnimating") ||
           g_unloading.load(std::memory_order_relaxed);
}

static bool MacGenieAnimateBeforeNativeHide(HWND hWnd) {
    if (!MacGenieShouldTreatHideAsClose(hWnd) || MacGenieCloseBlocked(hWnd)) return false;
    if (!MacGenieStartClose(hWnd, WM_DESTROY)) return false;
    MacGenieWaitCloseAnim(hWnd, 3500);
    return true;
}

static void MacGenieFinishNativeHide(HWND hWnd, bool animated) {
    if (!animated || !IsWindow(hWnd)) return;
    MacGenieSetCloak(hWnd, FALSE);
    RemovePropW(hWnd, L"GenieCloseAnimating");
    MacGenieSetDwmTransitions(hWnd, TRUE);
}

// HWND values are recycled by Windows. Many apps never forward WM_DESTROY to
// DefWindowProc, so also forget per-window state when DestroyWindow succeeds,
// otherwise a recycled handle would inherit "already launched" / a stale dock X.
static void MacGenieForgetWindow(HWND hWnd) {
    std::lock_guard<std::mutex> lock(g_CacheMutex);
    auto it = g_SnapshotCache.find(hWnd);
    if (it != g_SnapshotCache.end()) {
        DeleteObject(it->second.hBmp);
        g_SnapshotCache.erase(it);
    }
    g_IconPositions.erase(hWnd);
    g_LaunchSeen.erase(hWnd);
}

BOOL WINAPI DestroyWindow_Hook(HWND hWnd) {
    bool animated = false;
    if (MacGenieIsCloseTarget(hWnd) && !MacGenieCloseBlocked(hWnd)) {
        animated = MacGenieStartClose(hWnd, WM_DESTROY);
        if (animated && !MacGenieHasOtherAppWindow(hWnd)) MacGenieWaitCloseAnim(hWnd, 4000);
    }
    BOOL result = DestroyWindow_Original(hWnd);
    if (result) MacGenieForgetWindow(hWnd);
    if (!result && animated && IsWindow(hWnd)) {
        RemovePropW(hWnd, L"GenieCloseHandled");
        RemovePropW(hWnd, L"GenieCloseAnimating");
        MacGenieSetCloak(hWnd, FALSE);
        MacGenieSetDwmTransitions(hWnd, TRUE);
    }
    return result;
}

BOOL WINAPI ShowWindow_Hook(HWND hWnd, int nCmdShow) {
    if (nCmdShow == SW_HIDE) {
        bool animated = MacGenieAnimateBeforeNativeHide(hWnd);
        BOOL res = ShowWindow_Original(hWnd, nCmdShow);
        MacGenieFinishNativeHide(hWnd, animated);
        return res;
    }
    MacGenieClearCloseHandled(hWnd);

    if (nCmdShow == SW_MINIMIZE || nCmdShow == SW_SHOWMINIMIZED || nCmdShow == SW_SHOWMINNOACTIVE) {
        if (MacGenieShouldAnimate(hWnd)) {
            MacGenieSetDwmTransitions(hWnd, FALSE);
            StartMacGenieAnim(hWnd, FALSE, GetWindowLongPtrW(hWnd, GWL_EXSTYLE));
        }
        return ShowWindow_Original(hWnd, nCmdShow);
    }

    if ((nCmdShow == SW_RESTORE || nCmdShow == SW_SHOWNORMAL) && IsIconic(hWnd)) {
        if (g_openAnimation.load(std::memory_order_relaxed) && MacGenieShouldAnimate(hWnd)) {
            MacGenieSetDwmTransitions(hWnd, FALSE);
            MacGenieSetCloak(hWnd, TRUE);
            BOOL res = ShowWindow_Original(hWnd, nCmdShow);
            StartMacGenieAnim(hWnd, TRUE, GetWindowLongPtrW(hWnd, GWL_EXSTYLE), TRUE);
            return res;
        }
        return ShowWindow_Original(hWnd, nCmdShow);
    }

    LONG_PTR launchOrigEx;
    BOOL launchCloaked;
    if (MacGenieLaunchPrepare(hWnd, nCmdShow, &launchOrigEx, &launchCloaked)) {
        BOOL res = ShowWindow_Original(hWnd, nCmdShow);
        MacGenieLaunchCommit(hWnd, launchOrigEx, launchCloaked);
        return res;
    }

    return ShowWindow_Original(hWnd, nCmdShow);
}

LRESULT WINAPI DefWindowProcW_Hook(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) {
    if (Msg == WM_SHOWWINDOW && wParam && !IsIconic(hWnd)) {
        MacGenieClearCloseHandled(hWnd);
    }

    bool closeMessage = Msg == WM_CLOSE ||
                        (Msg == WM_SYSCOMMAND && (wParam & 0xFFF0) == SC_CLOSE);
    if (closeMessage && GetPropW(hWnd, L"GenieCloseBypass")) {
        if (Msg == WM_CLOSE) RemovePropW(hWnd, L"GenieCloseBypass");
        return DefWindowProcW_Original(hWnd, Msg, wParam, lParam);
    }
    if (closeMessage && IsWindowVisible(hWnd) && GetPropW(hWnd, L"GenieCloseAnimating")) {
        return 0;
    }
    if (closeMessage && g_closeAnimation.load(std::memory_order_relaxed) &&
        MacGenieIsCloseMessageClass(hWnd) &&
        IsWindowVisible(hWnd) && !IsIconic(hWnd) && GetAncestor(hWnd, GA_ROOT) == hWnd &&
        !MacGenieIsCloaked(hWnd) && !MacGenieCloseBlocked(hWnd)) {
        if (MacGenieStartClose(hWnd, Msg)) return 0;
    }

    if (Msg == WM_DESTROY) {
        RemovePropW(hWnd, L"GenieCloseHandled");
        RemovePropW(hWnd, L"GenieCloseAnimating");
        RemovePropW(hWnd, L"GenieCloseBypass");
        MacGenieForgetWindow(hWnd);
    }

    if (Msg == WM_SYSCOMMAND) {
        UINT cmd = wParam & 0xFFF0;
        if (cmd == SC_MINIMIZE) {
            if (GetPropW(hWnd, L"GenieBypass")) {
                return DefWindowProcW_Original(hWnd, Msg, wParam, lParam);
            }
            if (MacGenieShouldAnimate(hWnd)) {
                BOOL requestedUnhide = FALSE;
                HWND hNext = NULL;
                if (g_unhideEnabled.load(std::memory_order_relaxed)) {
                    HWND hTray = FindTaskbarForMonitor(MacGenieResolveDockMonitor(hWnd));
                    if (hTray) {
                        APPBARDATA abd = { sizeof(APPBARDATA) };
                        abd.hWnd = hTray;
                        UINT uState = (UINT)SHAppBarMessage(ABM_GETSTATE, &abd);
                        if (uState & ABS_AUTOHIDE) {
                            requestedUnhide = TRUE;
                            HWND hwndIter = GetWindow(hWnd, GW_HWNDNEXT);
                            while (hwndIter) {
                                if (IsWindowVisible(hwndIter) && !IsIconic(hwndIter) &&
                                    GetAncestor(hwndIter, GA_ROOT) == hwndIter &&
                                    GetWindowTextLengthW(hwndIter) > 0 &&
                                    hwndIter != hWnd && hwndIter != hTray) {
                                    DWORD exStyle = GetWindowLongPtrW(hwndIter, GWL_EXSTYLE);
                                    if (!(exStyle & WS_EX_TOOLWINDOW)) { hNext = hwndIter; break; }
                                }
                                hwndIter = GetWindow(hwndIter, GW_HWNDNEXT);
                            }
                            SetForegroundWindow(hTray);
                        }
                    }
                }

                MacGenieSetDwmTransitions(hWnd, FALSE);
                if (requestedUnhide) {
                    bool ok = StartMacGenieAnim(hWnd, FALSE, GetWindowLongPtrW(hWnd, GWL_EXSTYLE),
                                                FALSE, TRUE, TRUE, hNext);
                    if (ok) {
                        MacGenieSetCloak(hWnd, TRUE);
                        return 0;
                    }
                    return DefWindowProcW_Original(hWnd, Msg, wParam, lParam);
                }
                StartMacGenieAnim(hWnd, FALSE, GetWindowLongPtrW(hWnd, GWL_EXSTYLE));
            }
            return DefWindowProcW_Original(hWnd, Msg, wParam, lParam);
        }
        else if (cmd == SC_RESTORE && IsIconic(hWnd) &&
                 g_openAnimation.load(std::memory_order_relaxed) &&
                 MacGenieShouldAnimate(hWnd)) {
            MacGenieSetDwmTransitions(hWnd, FALSE);
            MacGenieSetCloak(hWnd, TRUE);
            LRESULT res = DefWindowProcW_Original(hWnd, Msg, wParam, lParam);
            StartMacGenieAnim(hWnd, TRUE, GetWindowLongPtrW(hWnd, GWL_EXSTYLE), TRUE);
            return res;
        }
    }
    return DefWindowProcW_Original(hWnd, Msg, wParam, lParam);
}

static void MacGenieTryMinimizeAnim(HWND hWnd) {
    if (!IsWindowVisible(hWnd) || IsIconic(hWnd)) return;
    if (!MacGenieShouldAnimate(hWnd)) return;
    MacGenieSetDwmTransitions(hWnd, FALSE);
    StartMacGenieAnim(hWnd, FALSE, GetWindowLongPtrW(hWnd, GWL_EXSTYLE));
}

BOOL WINAPI ShowWindowAsync_Hook(HWND hWnd, int nCmdShow) {
    if (nCmdShow == SW_HIDE) {
        if (MacGenieShouldTreatHideAsClose(hWnd) && !MacGenieCloseBlocked(hWnd) &&
            MacGenieStartClose(hWnd, MACGENIE_ASYNC_HIDE)) {
            return TRUE;
        }
        return ShowWindowAsync_Original(hWnd, nCmdShow);
    }
    MacGenieClearCloseHandled(hWnd);

    if (nCmdShow == SW_MINIMIZE || nCmdShow == SW_SHOWMINIMIZED ||
        nCmdShow == SW_SHOWMINNOACTIVE) {
        MacGenieTryMinimizeAnim(hWnd);
    } else {
        LONG_PTR launchOrigEx;
        BOOL launchCloaked;
        if (MacGenieLaunchPrepare(hWnd, nCmdShow, &launchOrigEx, &launchCloaked)) {
            BOOL res = ShowWindowAsync_Original(hWnd, nCmdShow);
            MacGenieLaunchCommit(hWnd, launchOrigEx, launchCloaked);
            return res;
        }
    }
    return ShowWindowAsync_Original(hWnd, nCmdShow);
}

BOOL WINAPI SetWindowPlacement_Hook(HWND hWnd, const WINDOWPLACEMENT* lpwndpl) {
    if (lpwndpl && lpwndpl->showCmd == SW_HIDE &&
        !(lpwndpl->flags & WPF_ASYNCWINDOWPLACEMENT)) {
        bool animated = MacGenieAnimateBeforeNativeHide(hWnd);
        BOOL res = SetWindowPlacement_Original(hWnd, lpwndpl);
        MacGenieFinishNativeHide(hWnd, animated);
        return res;
    }
    if (lpwndpl && lpwndpl->showCmd != SW_HIDE && lpwndpl->showCmd != SW_MINIMIZE &&
        lpwndpl->showCmd != SW_SHOWMINIMIZED && lpwndpl->showCmd != SW_SHOWMINNOACTIVE) {
        MacGenieClearCloseHandled(hWnd);
    }
    if (lpwndpl && (lpwndpl->showCmd == SW_MINIMIZE ||
                    lpwndpl->showCmd == SW_SHOWMINIMIZED ||
                    lpwndpl->showCmd == SW_SHOWMINNOACTIVE)) {
        MacGenieTryMinimizeAnim(hWnd);
    }
    return SetWindowPlacement_Original(hWnd, lpwndpl);
}

BOOL WINAPI CloseWindow_Hook(HWND hWnd) {
    MacGenieTryMinimizeAnim(hWnd);
    return CloseWindow_Original(hWnd);
}

BOOL WINAPI SetWindowPos_Hook(HWND hWnd, HWND hWndInsertAfter, int X, int Y,
                              int cx, int cy, UINT uFlags) {
    if ((uFlags & SWP_HIDEWINDOW) && !(uFlags & SWP_ASYNCWINDOWPOS)) {
        bool animated = MacGenieAnimateBeforeNativeHide(hWnd);
        BOOL res = SetWindowPos_Original(hWnd, hWndInsertAfter, X, Y, cx, cy, uFlags);
        MacGenieFinishNativeHide(hWnd, animated);
        return res;
    }
    if (uFlags & SWP_SHOWWINDOW) {
        MacGenieClearCloseHandled(hWnd);
        LONG_PTR launchOrigEx;
        BOOL launchCloaked;
        if (MacGenieLaunchPrepare(hWnd, SW_SHOW, &launchOrigEx, &launchCloaked)) {
            BOOL res = SetWindowPos_Original(hWnd, hWndInsertAfter, X, Y, cx, cy, uFlags);
            MacGenieLaunchCommit(hWnd, launchOrigEx, launchCloaked);
            return res;
        }
    }
    return SetWindowPos_Original(hWnd, hWndInsertAfter, X, Y, cx, cy, uFlags);
}

DWORD WINAPI MacGenieLaunchThread(LPVOID lpParam) {
    MacGenieLaunchData* ld = (MacGenieLaunchData*)lpParam;
    HWND hWnd = ld->hWnd;
    LONG_PTR originalExStyle = ld->originalExStyle;
    BOOL hiddenByCloak = ld->hiddenByCloak;
    delete ld;

    UINT ownCloak = hiddenByCloak ? (UINT)DWM_CLOAKED_APP : 0;
    Sleep(60);
    for (int i = 0; i < 30; ++i) {
        if (!IsWindow(hWnd) || g_unloading.load(std::memory_order_relaxed)) break;
        UINT cloaked = 0;
        if (FAILED(DwmGetWindowAttribute(hWnd, DWMWA_CLOAKED, &cloaked, sizeof(cloaked))) ||
            !(cloaked & ~ownCloak)) {
            break;
        }
        Sleep(50);
    }

    if (g_unloading.load(std::memory_order_relaxed) ||
        !IsWindow(hWnd) || IsIconic(hWnd) || !IsWindowVisible(hWnd)) {
        if (IsWindow(hWnd)) {
            MacGenieUndoRisingHide(hWnd, originalExStyle, hiddenByCloak);
        }
        return 0;
    }

    StartMacGenieAnim(hWnd, TRUE, originalExStyle, hiddenByCloak, FALSE, FALSE, NULL, FALSE, WM_DESTROY, TRUE);
    return 0;
}

BOOL Wh_ModInit() {
    MacGenieLoadSettings();

    Wh_SetFunctionHook((void*)DefWindowProcW, (void*)DefWindowProcW_Hook, (void**)&DefWindowProcW_Original);
    Wh_SetFunctionHook((void*)DestroyWindow, (void*)DestroyWindow_Hook, (void**)&DestroyWindow_Original);
    Wh_SetFunctionHook((void*)ShowWindow, (void*)ShowWindow_Hook, (void**)&ShowWindow_Original);
    Wh_SetFunctionHook((void*)ShowWindowAsync, (void*)ShowWindowAsync_Hook, (void**)&ShowWindowAsync_Original);
    Wh_SetFunctionHook((void*)SetWindowPlacement, (void*)SetWindowPlacement_Hook, (void**)&SetWindowPlacement_Original);
    Wh_SetFunctionHook((void*)CloseWindow, (void*)CloseWindow_Hook, (void**)&CloseWindow_Original);
    Wh_SetFunctionHook((void*)SetWindowPos, (void*)SetWindowPos_Hook, (void**)&SetWindowPos_Original);
    return TRUE;
}

BOOL Wh_ModSettingsChanged(BOOL* bReload) {
    *bReload = FALSE;
    MacGenieLoadSettings();
    return TRUE;
}

void Wh_ModBeforeUninit() {
    std::lock_guard<std::mutex> lock(g_WorkerThreadsMutex);
    g_unloading.store(true, std::memory_order_relaxed);
}

void Wh_ModUninit() {
    {
        std::lock_guard<std::mutex> lock(g_WorkerThreadsMutex);
        g_unloading.store(true, std::memory_order_relaxed);
    }
    std::vector<HANDLE> threads;
    {
        std::lock_guard<std::mutex> lock(g_WorkerThreadsMutex);
        threads.swap(g_WorkerThreads);
    }
    for (HANDLE h : threads) {
        WaitForSingleObject(h, INFINITE);
        CloseHandle(h);
    }

    {
        std::lock_guard<std::mutex> gpuLock(g_GpuMutex);
        MacGenieRelease(g_gpuVsBlob);
        MacGenieRelease(g_gpuPsBlob);
    }
    {
        std::lock_guard<std::mutex> d2dLock(g_D2DMutex);
        if (g_d2dFactory) {
            g_d2dFactory->Release();
            g_d2dFactory = nullptr;
        }
    }
    {
        std::lock_guard<std::mutex> uiaLock(g_UiaMutex);
        g_ButtonCache.clear();
    }
    std::lock_guard<std::mutex> lock(g_CacheMutex);
    for (auto& pair : g_SnapshotCache) {
        DeleteObject(pair.second.hBmp);
    }
    g_SnapshotCache.clear();
    g_IconPositions.clear();
    g_ProcessIconPositions.clear();
    g_LaunchSeen.clear();
}
