// ==WindhawkMod==
// @id              macos-minimize-animation
// @name            MacOS Minimize Animation
// @description     Smooth macOS-style genie minimize, restore (open) and optional close animations for every window.
// @version         3.2.0
// @author          Abdullah Masood
// @github          https://github.com/Abdullah-Masood-05
// @include         *
// @compilerOptions -ldwmapi -lgdi32 -ld2d1 -lole32 -loleaut32 -luuid -lshell32
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# MacOS Minimize Animation

Brings the classic macOS **genie** effect to Windows. When you minimize a window
it warps and flows down into the taskbar; when you restore it, it flows back out.
Apps opening and windows closing get a genie too.

Definitely not inspired from MacOS.

## Animation style
There are two genie styles, selectable in the settings (**Animation style**):

- **Classic** (default) - this mod's original strip-based genie: light and fast:

![Classic genie demo](https://raw.githubusercontent.com/Abdullah-Masood-05/my-windhawk-mods-media/main/macos-minimize-animation.gif)

- **Modern** - the Direct2D genie engine contributed by **Potassiumuncher**:
  a smooth mesh warp that necks the window down into the taskbar:

![Modern genie demo](https://raw.githubusercontent.com/Potassiumuncher/MacOS-Animation-for-windows/99a9a78e9a06c49b282cc8e337854840a9f7fa73/Desktop2026.07.02-19.32.49.05-ezgif.com-video-to-gif-converter.gif)

## Close animation
When a window closes, it necks down and flows toward the top, bottom, left or right
edge of its own screen (**Close direction**). Contributed by **MrCrustyToast**:

![Close genie demo](https://i.imgur.com/Vzpi1re.gif)

## Credits
The **Modern** genie style - the Direct2D genie rendering engine (a configurable
mesh warp driven by the macOS "lamp" curve), the UI Automation taskbar-button
targeting, and the taskbar auto-hide handling - was contributed by
**Potassiumuncher** - <https://github.com/Potassiumuncher>. As of v3.0.0 the
default animation is his engine, integrated into this mod's hardening (multi-hook
capture, flash-free cloak restore, safe unload, first-frame sync). The fixes in
v3.1.1 are his as well (from v1.5 of his engine): the translucent-window capture
fix that stops acrylic / Mica windows going grey, the window-only capture that
keeps the taskbar out of the animation, the smoother mask / mesh rendering, the
frame pacing (since reshaped into a high-resolution 2x-refresh timer in v3.1.2),
and the tile-count setting. The modern-genie demo GIF above is also his, from his
own [MacOS-Animation-for-windows](https://github.com/Potassiumuncher/MacOS-Animation-for-windows)
repo. Huge thanks for building this, sharing the recording, and generously handing
it all over - this mod wouldn't look nearly this good without him. The **Classic**
style is the mod's original renderer.

The optional **close animation** (v3.2.0) was contributed by **MrCrustyToast** -
<https://github.com/MrCrustyToast> - from his MacOS Genie Closing Animation mod: the
close triggers, the close-direction targets and the hide-to-tray handling. His close
triggers are in turn based on ReDrag's Windows Animations mod. Thanks!

The Firefox-family first-window check in the app-launch animation (v3.1.7) is
ported from **ReDrag**'s [Windows Animations](https://github.com/ramensoftware/windhawk-mods/blob/main/mods/windows-animations.wh.cpp)
mod, which found that hiding a Gecko browser's first window with a DWM cloak can
make it open a second window. Thanks!

## Known issues
- On multi-monitor setups (especially the secondary display), the genie can
  briefly slide sideways / to the left for a frame or two at the very start of a
  minimize or restore before it flows toward the taskbar. The animation still
  completes correctly; this is a known issue being worked on. If you spot anything
  else, please report it on the mod's GitHub issue thread.
- If an app reopens (or restores a saved session) at coordinates left over from a
  monitor that's no longer connected (e.g. an HDMI display you've since unplugged),
  the window can appear off-screen or the genie can play oddly / land on the wrong
  spot. This is a Windows/app placement quirk outside the mod's control, not
  something the mod can reliably detect and fix.
- Translucent (acrylic / Mica) windows are captured much more faithfully as of
  v3.1.1 - transparent regions now stay transparent instead of turning grey - but
  some apps may still show a flat or slightly-off backdrop during the genie,
  because the live blur effect itself can't be carried into a static snapshot.
- On NON-primary monitors specifically, Windows often refuses to hand over the
  Mica / acrylic backdrop at all (the capture comes back fully transparent
  there). The mod detects this and falls back to an opaque dark backdrop for the
  animation instead of letting the window background vanish - so translucent
  windows on a secondary display animate with the older dark look. A proper fix
  needs deeper investigation.
- A repeating / looping animation has been reported on Zen Browser
  (Firefox-based). It's under investigation; if it hits you, add the browser to
  the mod's **Excluded programs** setting as a workaround and please share
  details on the GitHub issue thread.

## See it in action
- Compile the mod with the button on the left or with Ctrl+B.
- Enable the mod and open any normal window (Explorer, a browser, Notepad...).
- Minimize it with the title bar `[-]` button or `Win`+`Down` and watch it warp
  and pour into the taskbar.
- Restore it from the taskbar and watch the genie play in reverse.
- Close it and watch it flow to the edge of the screen; open an app and watch it
  flow out.
- Tweak the **Animation duration** in the settings to taste, and switch the
  **Animation style** between the classic and modern (Potassiumuncher) genie.

## Features
- **Real genie warp.** The window necks down and funnels into the taskbar icon
  instead of just shrinking - row by row in the classic style, or as a Direct2D
  mesh following the macOS lamp curve in the modern style.
- **Actually smooth.** Progress is driven by real elapsed time, and frames are
  paced at twice the display's refresh rate (via a high-resolution timer), so the
  compositor always has a fresh frame ready at each vsync. No system animation
  competes with it, so it stays smooth at any duration you set. In v3.1.3 the
  modern genie's render canvas shrank from the full virtual desktop to a tight
  bounding box around the window and dock target, which removed a large per-frame
  `UpdateLayeredWindow` cost that made the modern style stutter on AMD GPUs
  (reported on a 6900 XT and a Vega 8 iGPU; the classic style was already using
  the tight canvas).
- **Smoothstep easing** instead of a linear ramp, so it eases in and out.
- **Accurate targeting.** The mod locates the app's actual taskbar button via UI
  Automation and aims the genie at it (with a per-process fallback cache), instead
  of guessing from the cursor.
- **Pixel-aligned capture.** The window is measured by its DWM extended frame
  bounds (not the legacy window rect), so keyboard / AutoHotkey minimizes are no
  longer spatially shifted.

## How it works
The mod hooks `ShowWindow`, `ShowWindowAsync`, `SetWindowPlacement`, `CloseWindow`,
`SetWindowPos` and `DefWindowProcW` to catch minimize / restore / first-show
requests - including apps with custom title bars (e.g. Zed) and Store/UWP apps,
whose minimize buttons bypass the classic paths - and `DestroyWindow` (plus
`WM_CLOSE` and hides) for the close animation. It snapshots the window, then on
a dedicated high-priority thread it draws a transparent layered "ghost" window that
warps the snapshot frame by frame into the taskbar, leaving the real window to
minimize behind it without the system's own animation getting in the way.

## Settings
- **Animation duration** - how long the effect lasts (50-2000 ms).
- **Animate window restore (open)** - also play the reverse genie when restoring.
- **Animate app launch (experimental)** - also play the genie when an application
  window first opens. **Experimental, on by default**: it may not catch borderless
  apps, and can occasionally fire on splash / dialog windows. Since v3.1.7 the
  window is hidden with a DWM cloak until the genie is done, so GPU-rendered apps
  (Tauri, Slint, Electron...) no longer flash before it.
- **Animate window close (experimental)** - also play a genie when a window
  closes: the window necks down and flows toward the top, bottom, left or right
  edge of its screen (**Close direction**). **Experimental, on by default.** It uses
  the same duration, style and tile count as minimize. Contributed by
  MrCrustyToast.
- **Animate apps that hide to the tray on close** - some apps (Discord, Steam,
  Telegram...) hide their window instead of closing it; with this on (and close
  animation on) that hide plays the close genie too. Off by default.
- **Multi-monitor support (experimental)** - play the genie on the monitor the
  window is on, targeting that monitor's taskbar edge, instead of always the
  primary monitor's. **Experimental and off by default.** (The close genie always
  plays on the window's own monitor.)
- **Unhide taskbar (for taskbar auto-hide)** - if your taskbar is set to
  auto-hide, briefly reveal it during the minimize so the genie has a visible dock
  to flow into, then let it slide back once the animation finishes.
- **Taskbar unhide duration** - how long (ms) to keep the taskbar revealed before
  performing the deferred minimize on the auto-hide path.
- **Animation style** - Classic (this mod's original renderer, default) or
  Modern (Potassiumuncher's Direct2D engine).
- **Animation tile count** - mesh resolution of the modern genie (8-96, default
  35). Higher is smoother but costs more per frame. Contributed by Potassiumuncher.
- **Excluded programs** - windows owned by these processes never animate. Use
  this rather than Windhawk's per-mod process exclusion: that list only stops the
  mod from loading INTO a process, but a window's animation is often driven from
  a different process (Explorer's taskbar, the shell), so the app would still
  animate. Example: for Windows Terminal's Quake mode, exclude
  `WindowsTerminal.exe` (the window's owner) - excluding `powershell.exe` has no
  effect, since the terminal window doesn't belong to PowerShell.

## Notes
- Works on all top-level windows; child / tiny / hidden windows are skipped.
- The close animation catches windows that are destroyed on their own UI thread,
  plus `WM_CLOSE` for console windows, Windows Terminal, Task Manager and WinUI 3
  apps. Apps that ask "save changes?" before closing may show the window again
  after the genie if you cancel; WinUI 3 apps can ask a second time.
- DWM transitions are temporarily disabled on the animated window and restored
  afterwards, so the system's own minimize/restore animation doesn't fight ours.
- Minimize snapshots are captured from the window itself, so the taskbar and other
  windows never bleed into the animation (even for maximized / fullscreen apps).
  Since v3.1.1 the capture preserves per-pixel transparency (Potassiumuncher's
  fix), so translucent windows keep their see-through regions during the genie
  instead of flattening to grey.

# Getting started
Check out the documentation
[here](https://github.com/ramensoftware/windhawk/wiki/Creating-a-new-mod).
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- duration_ms: 450
  $name: Animation duration (ms)
  $description: How long the genie animation lasts. Clamped to 50-2000.
- open_animation: true
  $name: Animate window restore (open)
  $description: Play the reverse genie animation when a window is restored from the taskbar.
- launch_animation: true
  $name: Animate app launch (experimental)
  $description: >-
    Play a genie when an application window first opens. Experimental: may not
    catch borderless apps, and can occasionally fire on splash/dialog windows.
- close_animation: true
  $name: Animate window close (experimental)
  $description: >-
    Play a genie when a window closes. Experimental. Uses the same duration,
    animation style and tile count as minimize. Contributed by MrCrustyToast.
- close_direction: up
  $name: Close direction
  $description: >-
    Where a closing window flows to on its own screen: the center of the top or
    bottom edge, or the center of the left / right edge.
  $options:
  - up: Up (top center)
  - down: Down (bottom center)
  - left: Left (center)
  - right: Right (center)
- hide_as_close: false
  $name: Animate apps that hide to the tray on close
  $description: >-
    Some apps (Discord, Steam, Telegram...) hide their window instead of closing
    it. With this on (and window close animation on), that hide plays the close
    genie too.
- multi_monitor: false
  $name: Multi-monitor support (experimental)
  $description: >-
    Play the genie on the monitor the window is on and target that monitor's
    taskbar edge, instead of always the primary monitor's.
- unhide_taskbar: true
  $name: Unhide taskbar (for taskbar auto-hide)
  $description: >-
    If the taskbar is set to auto-hide, briefly reveal it during the minimize so
    the genie has a visible dock to flow into, then let it slide back afterwards.
- unhide_duration_ms: 450
  $name: Taskbar unhide duration (ms)
  $description: >-
    How long to keep the taskbar revealed before performing the deferred minimize
    on the auto-hide path. Clamped to 0-5000.
- engine: classic
  $name: Animation style
  $description: >-
    Modern is the Direct2D genie engine contributed by Potassiumuncher - a smooth
    mesh warp that necks the window down into the taskbar. Classic is this mod's
    original strip-based genie (lighter, a slightly different look). Pick whichever
    you prefer; some people asked to keep the classic one.
  $options:
  - modern: Modern genie (Direct2D engine by Potassiumuncher)
  - classic: Classic genie (original)
- tile_count: 35
  $name: Animation tile count (modern style)
  $description: >-
    Mesh resolution for the modern genie warp (higher = smoother curve, but more
    expensive to render each frame). 24-32 is cheap/fast, 35 is a good middle
    ground, 64+ is highest quality. Clamped to 8-96. Contributed by
    Potassiumuncher.
- excluded_programs: [""]
  $name: Excluded programs
  $description: >-
    Programs whose windows should never animate (one per entry, e.g.
    WindowsTerminal.exe - with or without .exe). Use this instead of Windhawk's
    process exclusion list: that list only stops the mod from loading INTO a
    process, but animations for a window are often driven from a different
    process (Explorer's taskbar, the shell), so excluded apps would still
    animate. This setting matches the process that OWNS the window, wherever the
    animation is triggered from.
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

#ifndef DWMWA_EXTENDED_FRAME_BOUNDS
#define DWMWA_EXTENDED_FRAME_BOUNDS 9
#endif

#ifndef PW_RENDERFULLCONTENT
#define PW_RENDERFULLCONTENT 2
#endif

// High-resolution waitable timer flag (Win10 1803+). The kernel rejects the
// flag on 1709 and earlier (and Win7/8.1), so CreateWaitableTimerExW is retried
// without it; the pacer then just ticks coarser. A plain Sleep(1) floor keeps
// the loop from ever degrading to a busy spin.
#ifndef CREATE_WAITABLE_TIMER_HIGH_RESOLUTION
#define CREATE_WAITABLE_TIMER_HIGH_RESOLUTION 0x00000002
#endif

// Ported from Potassiumuncher's genie engine (github.com/Potassiumuncher).
#define PI 3.14159265f
// Mesh tile-count bounds for the modern engine (Potassiumuncher's v1.5 update:
// resolution is now a user setting instead of a fixed 20x20 grid).
#define TILE_COUNT_MIN 8
#define TILE_COUNT_MAX 96
#define TILE_COUNT_DEFAULT 35
#define COMPAT_TASKBAR_HEIGHT 70
#define COMPAT_ICON_SIZE      33
// Close-animation completion marker for a held ShowWindowAsync(SW_HIDE).
#define MACGENIE_ASYNC_HIDE (WM_APP + 0x4A1)

// Ported from Potassiumuncher's genie engine (github.com/Potassiumuncher).
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

// Shared Direct2D factory (multi-threaded: several worker threads may render at
// once). Ported from Potassiumuncher's genie engine.
ID2D1Factory* g_d2dFactory = nullptr;

struct MacGenieAnimData {
    HWND hRealWnd;
    HBITMAP hBitmap;      // 32-bit top-down premultiplied DIB snapshot (w x h)
    void* pBits;          // its bits (fed straight to D2D CreateBitmap)
    RECT targetRect;      // the window's DWM extended-frame rect when anim started
    HMONITOR hMon;        // monitor the genie targets (window's when multi-monitor
                          // support is on, else the primary)
    int width;
    int height;
    int targetDockX;      // taskbar button X located via UI Automation
    BOOL isRising;        // FALSE = minimize (flow in), TRUE = restore (flow out)
    LONG_PTR originalExStyle;
    BOOL hiddenByCloak;   // rising only: TRUE = hidden via DWM cloak (restore and
                          // launch paths), FALSE = via WS_EX_LAYERED + alpha 0
                          // (launch of a Firefox-family first window)
    HANDLE hFirstFrameShown; // falling only: event signaled once the ghost's first
                             // frame is composed; NULL when unused
    int durationMs;
    // Auto-hide "unhide" (deferred minimize) - Potassiumuncher's engine. Only used
    // on the DefWindowProcW SC_MINIMIZE path when the taskbar is set to auto-hide.
    BOOL requestedUnhide;
    HWND hNextApp;
    int  unhideDurationMs;
    BOOL deferredMinimize;
    // Close animation (MrCrustyToast). Falling only: the window flows to the
    // close-direction edge instead of the taskbar, and the teardown finishes the
    // close the hook held back (see closeMsg).
    BOOL isClosing;
    // How the teardown completes the close: WM_DESTROY = nothing (the hook lets
    // the held DestroyWindow / hide through once we're done), WM_CLOSE /
    // WM_SYSCOMMAND = repost that close, MACGENIE_ASYNC_HIDE = ShowWindowAsync(SW_HIDE).
    UINT closeMsg;
};

// Handed to MacGenieLaunchThread: the window, its TRUE pre-hide extended style
// (captured at the hook site, before any WS_EX_LAYERED was added) and how the hook
// hid it, so the worker can undo exactly that hide.
struct MacGenieLaunchData {
    HWND     hWnd;
    LONG_PTR originalExStyle;
    BOOL     hiddenByCloak;   // TRUE = DWM cloak, FALSE = WS_EX_LAYERED + alpha 0
};

// Cached snapshot for the restore animation. The DIB section owns pBits, so
// DeleteObject(hBmp) frees both. Snapshot model from Potassiumuncher's engine.
struct SnapCache { HBITMAP hBmp; void* pBits; int w; int h; };

// --- THE VAULTS ---
std::unordered_map<HWND, SnapCache> g_SnapshotCache;
std::unordered_map<HWND, int> g_IconPositions;            // per-window learned icon X
std::unordered_map<std::wstring, int> g_ProcessIconPositions; // per-process fallback
std::unordered_set<HWND> g_LaunchSeen;   // windows we've already shown/animated once
std::unordered_set<HWND> g_AnimActive;   // windows with a genie currently in flight
std::mutex g_CacheMutex;

// --- SETTINGS ---
std::atomic<int> g_durationMs{450};
std::atomic<bool> g_openAnimation{true};
std::atomic<bool> g_launchAnimation{true};
std::atomic<bool> g_multiMonitor{false};
std::atomic<int>  g_unhideDurationMs{450};
std::atomic<bool> g_unhideEnabled{true};
// false = modern Direct2D genie (Potassiumuncher's engine), true = the original
// v2.2.0 strip-based GDI genie ("classic", the default style).
std::atomic<bool> g_classicEngine{true};
// Mesh resolution of the modern engine (Potassiumuncher's tile-count setting).
std::atomic<int> g_tileCount{TILE_COUNT_DEFAULT};
// Close animation (MrCrustyToast): on by default; hide-as-close off by default.
std::atomic<bool> g_closeAnimation{true};
std::atomic<bool> g_hideAsClose{false};
std::atomic<int>  g_closeDirection{2};   // 0 = left, 1 = right, 2 = up, 3 = down
// Lowercased entries of the "Excluded programs" setting. Guarded by g_CacheMutex
// (written only from LoadSettings, read once per animation start).
std::vector<std::wstring> g_ExcludedPrograms;

// --- UNLOAD COORDINATION ---
// Windhawk unmaps the mod DLL right after Wh_ModUninit returns, so a worker thread
// still running mod code at that point (even just its function epilogue) would
// crash its host process. Every worker is started through MacGenieSpawnWorker,
// which keeps its handle and refuses new workers once g_unloading is set (checked
// under the same lock). Wh_ModBeforeUninit sets the flag, the workers notice it
// and wind down, and Wh_ModUninit - which runs after the hooks are disabled, so no
// new worker can appear - joins every handle before the DLL goes away. The join
// doesn't pump messages (that could re-enter hooks), and no worker sends a message
// to the unloading thread once g_unloading is set.
std::atomic<bool> g_unloading{false};
std::mutex g_WorkerThreadsMutex;
std::vector<HANDLE> g_WorkerThreads;

// Returns false (nothing started) when unloading or CreateThread fails; callers
// then undo their hide exactly as on any other failed start.
static bool MacGenieSpawnWorker(LPTHREAD_START_ROUTINE proc, void* param) {
    std::lock_guard<std::mutex> lock(g_WorkerThreadsMutex);
    if (g_unloading.load(std::memory_order_relaxed)) return false;
    // Drop handles of workers that already finished.
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
    // Wh_GetStringSetting never returns NULL (it returns L"" on error / unset),
    // so only the string comparison matters.
    g_classicEngine.store(wcscmp(engine, L"classic") == 0, std::memory_order_relaxed);
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

    // "Excluded programs" list: lowercase each entry once here so the per-anim
    // check is a straight comparison.
    std::vector<std::wstring> excluded;
    for (int i = 0;; i++) {
        PCWSTR prog = Wh_GetStringSetting(L"excluded_programs[%d]", i);
        bool has = *prog;   // L"" on unset, so *prog alone signals presence
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

// TRUE if the process that OWNS hWnd is on the "Excluded programs" list. This is
// checked at the single animation choke point (StartMacGenieAnim), NOT via
// Windhawk's own exclusion list: that list only prevents the mod from being
// LOADED into a process, but this mod's hooks animate a window from whichever
// process manipulates it (Explorer's taskbar click, the shell, the app itself),
// so an app excluded there would still animate. Matching the window's owning
// process here respects the exclusion no matter where the trigger came from.
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

void MacGenieSetDwmTransitions(HWND hWnd, BOOL enable) {
    BOOL disable = !enable;
    DwmSetWindowAttribute(hWnd, DWMWA_TRANSITIONS_FORCEDISABLED, &disable, sizeof(disable));
}

// Hide / show a window at the DWM level. A cloaked window is simply not rendered
// (frame included) while staying "visible" to Win32 and painting normally
// underneath, so a restore can happen invisibly under the genie with no
// layered-surface rebuild flash. Kept from MINE in preference to HIS's
// WS_EX_LAYERED + alpha 0 restore hide, which flashes the bare frame.
void MacGenieSetCloak(HWND hWnd, BOOL cloak) {
    DwmSetWindowAttribute(hWnd, DWMWA_CLOAK, &cloak, sizeof(cloak));
}

// Undo a rising caller's pre-animation hide (used whenever the animation can't run
// or is refused): uncloak or un-hide depending on how the window was hidden, and
// re-enable its DWM transitions.
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

// Should we animate this window at all? Skip child / tiny windows so the effect
// only fires on real top-level windows. The size check stays on the placement /
// window rect; the animation geometry (below) uses the DWM extended frame bounds.
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

// A real, top-level window with a title bar (skips child windows, tool windows,
// and borderless popups / menus / tooltips / splash screens).
static bool MacGenieIsLaunchWindow(HWND hWnd) {
    if (!hWnd) return false;
    if (GetAncestor(hWnd, GA_ROOT) != hWnd) return false;
    LONG_PTR style = GetWindowLongPtrW(hWnd, GWL_STYLE);
    if (!(style & WS_CAPTION)) return false;
    LONG_PTR exStyle = GetWindowLongPtrW(hWnd, GWL_EXSTYLE);
    if (exStyle & WS_EX_TOOLWINDOW) return false;
    return MacGenieShouldAnimate(hWnd);
}

// -------------------------------------------------------------------------
// Direct2D genie renderer helpers - ported verbatim from Potassiumuncher's
// genie engine (github.com/Potassiumuncher).
// -------------------------------------------------------------------------

// Ported from Potassiumuncher's genie engine (github.com/Potassiumuncher).
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

// Ported from Potassiumuncher's genie engine (github.com/Potassiumuncher).
static D2D1_POINT_2F BloatPoint(D2D1_POINT_2F p, D2D1_POINT_2F c, float amount = 0.5f) {
    float dx = p.x - c.x; float dy = p.y - c.y;
    float len = sqrtf(dx*dx + dy*dy);
    if (len < 0.001f) return p;
    return D2D1::Point2F(p.x + (dx/len)*amount, p.y + (dy/len)*amount);
}

// Ported from Potassiumuncher's genie engine (github.com/Potassiumuncher).
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

// Ported from Potassiumuncher's genie engine (github.com/Potassiumuncher).
// Locates the app's taskbar button via UI Automation and returns its center X
// (falls back to fallbackX, then to the per-window / per-process learned cache).
// Runs on the hook (app UI) thread: CoInitializeEx(APARTMENTTHREADED) matches the
// typical GUI-thread apartment (S_FALSE) or reports RPC_E_CHANGED_MODE on an MTA
// thread; either way CoUninitialize is balanced only when we actually initialized.
int GetTaskbarButtonX(HWND hWndApp, int fallbackX, HMONITOR hMon) {
    int targetX = fallbackX;
    bool uiaFound = false;

    std::wstring procNameLower = L"";
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
                    procNameLower = (name + 1);
                    size_t dotPos = procNameLower.find(L'.');
                    if (dotPos != std::wstring::npos) procNameLower = procNameLower.substr(0, dotPos);
                    std::transform(procNameLower.begin(), procNameLower.end(), procNameLower.begin(), ::towlower);
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
        IUIAutomation* pAutomation = nullptr;
        HRESULT hrUia = CoCreateInstance(__uuidof(CUIAutomation8), NULL, CLSCTX_INPROC_SERVER, __uuidof(IUIAutomation), (void**)&pAutomation);
        if (FAILED(hrUia)) {
            hrUia = CoCreateInstance(__uuidof(CUIAutomation), NULL, CLSCTX_INPROC_SERVER, __uuidof(IUIAutomation), (void**)&pAutomation);
        }

        if (SUCCEEDED(hrUia) && pAutomation) {
            HWND hTray = FindTaskbarForMonitor(hMon);
            if (hTray) {
                IUIAutomationElement* pTrayElement = nullptr;
                if (SUCCEEDED(pAutomation->ElementFromHandle(hTray, &pTrayElement)) && pTrayElement) {

                    WCHAR titleW[512] = {0};
                    GetWindowTextW(hWndApp, titleW, 512);
                    std::wstring titleLower = titleW;
                    std::transform(titleLower.begin(), titleLower.end(), titleLower.begin(), ::towlower);

                    std::wstring procHintLower = procNameLower;
                    if (procNameLower == L"chrome") procHintLower = L"google chrome";
                    else if (procNameLower == L"msedge") procHintLower = L"microsoft edge";
                    else if (procNameLower == L"firefox") procHintLower = L"firefox";
                    else if (procNameLower == L"brave") procHintLower = L"brave";
                    else if (procNameLower == L"opera") procHintLower = L"opera";
                    else if (procNameLower == L"vivaldi") procHintLower = L"vivaldi";

                    IUIAutomationCondition* pButtonCond = nullptr;
                    IUIAutomationCondition* pListItemCond = nullptr;
                    IUIAutomationCondition* pOrCond = nullptr;

                    VARIANT varBtn; varBtn.vt = VT_I4; varBtn.lVal = UIA_ButtonControlTypeId;
                    pAutomation->CreatePropertyCondition(UIA_ControlTypePropertyId, varBtn, &pButtonCond);

                    VARIANT varList; varList.vt = VT_I4; varList.lVal = UIA_ListItemControlTypeId;
                    pAutomation->CreatePropertyCondition(UIA_ControlTypePropertyId, varList, &pListItemCond);

                    if (pButtonCond && pListItemCond) {
                        pAutomation->CreateOrCondition(pButtonCond, pListItemCond, &pOrCond);
                    }

                    IUIAutomationElementArray* pArray = nullptr;
                    if (pOrCond && SUCCEEDED(pTrayElement->FindAll(TreeScope_Descendants, pOrCond, &pArray)) && pArray) {
                        int length = 0;
                        pArray->get_Length(&length);

                        MONITORINFO mi = {0};
                        mi.cbSize = sizeof(MONITORINFO);
                        GetMonitorInfoW(hMon, &mi);
                        int monRight = mi.rcMonitor.right;

                        int bestScore = 0;

                        for (int i = 0; i < length; i++) {
                            IUIAutomationElement* pItem = nullptr;
                            if (SUCCEEDED(pArray->GetElement(i, &pItem)) && pItem) {
                                BSTR name;
                                if (SUCCEEDED(pItem->get_CurrentName(&name)) && name) {
                                    std::wstring uiaNameLower = name;
                                    std::transform(uiaNameLower.begin(), uiaNameLower.end(), uiaNameLower.begin(), ::towlower);

                                    if (!uiaNameLower.empty()) {
                                        int score = 0;

                                        if (titleLower == uiaNameLower) score += 1000;
                                        if (!titleLower.empty() && titleLower.find(uiaNameLower) != std::wstring::npos) score += 500;
                                        if (!uiaNameLower.empty() && uiaNameLower.find(titleLower) != std::wstring::npos) score += 500;

                                        if (!procNameLower.empty() && uiaNameLower.find(procNameLower) != std::wstring::npos) score += 400;
                                        if (!procHintLower.empty() && procHintLower != procNameLower &&
                                            uiaNameLower.find(procHintLower) != std::wstring::npos) score += 900;

                                        std::wstring currentWord;
                                        for (wchar_t c : titleLower) {
                                            if (iswalnum(c)) {
                                                currentWord += c;
                                            } else {
                                                if (currentWord.length() >= 4 && uiaNameLower.find(currentWord) != std::wstring::npos) score += 50;
                                                currentWord.clear();
                                            }
                                        }
                                        if (currentWord.length() >= 4 && uiaNameLower.find(currentWord) != std::wstring::npos) score += 50;

                                        if (uiaNameLower.find(L"start") != std::wstring::npos) score -= 500;
                                        if (uiaNameLower.find(L"search") != std::wstring::npos) score -= 500;
                                        if (uiaNameLower.find(L"task view") != std::wstring::npos) score -= 500;
                                        if (uiaNameLower.find(L"widgets") != std::wstring::npos) score -= 500;

                                        if (score > bestScore) {
                                            RECT bRect;
                                            if (SUCCEEDED(pItem->get_CurrentBoundingRectangle(&bRect))) {
                                                if (bRect.right > bRect.left && bRect.left < monRight - 50) {
                                                    bestScore = score;
                                                    targetX = bRect.left + (bRect.right - bRect.left) / 2;
                                                    uiaFound = true;
                                                }
                                            }
                                        }
                                    }
                                    SysFreeString(name);
                                }
                                pItem->Release();
                            }
                        }
                        pArray->Release();
                    }
                    if (pButtonCond) pButtonCond->Release();
                    if (pListItemCond) pListItemCond->Release();
                    if (pOrCond) pOrCond->Release();
                    pTrayElement->Release();
                }
            }
            pAutomation->Release();
        }
        if (coInit) CoUninitialize();
    }

    std::lock_guard<std::mutex> lock(g_CacheMutex);
    if (uiaFound) {
        g_IconPositions[hWndApp] = targetX;
        if (!processKey.empty()) {
            g_ProcessIconPositions[processKey] = targetX;
        }
    } else if (g_IconPositions.count(hWndApp)) {
        targetX = g_IconPositions[hWndApp];
    } else if (!processKey.empty() && g_ProcessIconPositions.count(processKey)) {
        targetX = g_ProcessIconPositions[processKey];
    }

    return targetX;
}

// Close animation teardown, run by both render threads once the genie is done
// (MrCrustyToast). The hook that started a close keeps the real window cloaked;
// how the close is completed depends on what the hook held back:
//  - WM_DESTROY: nothing here - the DestroyWindow / hide hook is waiting for this
//    thread to finish and then lets the app's call through itself;
//  - MACGENIE_ASYNC_HIDE: perform the app's ShowWindowAsync(SW_HIDE) now;
//  - WM_CLOSE / WM_SYSCOMMAND: repost the swallowed close with a bypass marker.
// The window is uncloaked once it is gone or hidden (or after a bounded wait, e.g.
// when the app showed a "save changes?" prompt and the close was cancelled).
// While unloading, the held close is still completed - posted plainly, since the
// hooks are going away and will no longer intercept it - and the cloak undone.
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
        // Still visible: the app cancelled the close (e.g. "close all tabs?" ->
        // No). Re-arm so the next close of this window animates again.
        if (IsWindowVisible(hWnd)) {
            RemovePropW(hWnd, L"GenieCloseHandled");
            RemovePropW(hWnd, L"GenieCloseBypass");
        }
    }
}

// Ported from Potassiumuncher's genie engine (github.com/Potassiumuncher).
// The macOS "lamp" curve: maps a mesh vertex (tx,ty in 0..1) at progress p to its
// warped screen position, given the window rect w and the taskbar icon rect i.
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

// The lamp curve assumes the target is BELOW the window. For a target above it (a
// close toward the top edge, a top taskbar, a side edge with the window in the
// lower half) evaluate it in vertically mirrored space and mirror the result back;
// otherwise the window sweeps past itself and the snapshot plays upside down.
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

// Shared by both engines: union of the monitors involved (the window's own
// monitor and the monitor the genie funnels to). With Multi-monitor support off
// (the default) data->hMon is the primary monitor, so a window sitting on a
// secondary display still needs its own monitor in the union or the canvas would
// collapse onto the primary and clip the first half of the animation.
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

// Shared by both engines: clamp a proposed canvas rect to the union of the
// monitors involved, then re-expand it so it can never clip the window rect or
// the dock target column away. The monitor clamp only trims the box to the
// screens actually used; it must never cut geometry the animation draws.
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
    if (left  > dockX - 16)    left  = dockX - 16;   // iGeom is dockX ± 11
    if (right < dockX + 16)    right = dockX + 16;
}

// -------------------------------------------------------------------------
// Genie Animation Thread
//
// MINE's loop and lifecycle - joined worker, g_unloading early-out, the
// first-frame sync event, g_AnimActive teardown - with HIS's Direct2D mesh
// renderer as the per-frame draw call. Frames are paced by a high-resolution
// waitable timer at 2x the display's refresh rate (never a busy loop), so DWM
// always has a fresh frame to composite at each vsync.
// -------------------------------------------------------------------------
DWORD WINAPI MacGenieAnimThread(LPVOID lpParam) {
    MacGenieAnimData* data = (MacGenieAnimData*)lpParam;

    // Matches the in-repo standalone genie mod (genie-minimize-animation.wh.cpp):
    // HIGHEST, not TIME_CRITICAL. A TIME_CRITICAL thread that never yields would
    // preempt the host app's UI thread for the whole animation on slow or 2-core
    // machines; the pacer below keeps the frame rate up without that.
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST);

    // --- Target geometry (Potassiumuncher's engine), using the monitor picked by
    // the multi-monitor setting (data->hMon), so "off" keeps the primary-monitor
    // behavior and "on" targets the window's monitor taskbar edge. ---
    Geometry wGeom = { (float)data->targetRect.left, (float)data->targetRect.top, (float)data->width, (float)data->height };

    MONITORINFO mi = {0};
    mi.cbSize = sizeof(MONITORINFO);
    HMONITOR hMon = data->hMon;
    GetMonitorInfoW(hMon, &mi);

    UINT dpiX = 96, dpiY = 96;
    // System32-only search: Shcore.dll is not a KnownDLL, and under @include *
    // this code runs inside every process - a bare-name LoadLibrary would search
    // the host .exe's own directory first, letting a planted Shcore.dll next to
    // any portable app execute in that process (windhawk-mods #2063 pattern).
    HMODULE hShcore = LoadLibraryExW(L"Shcore.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (hShcore) {
        typedef HRESULT (WINAPI *GetDpiForMonitor_t)(HMONITOR, int, UINT*, UINT*);
        auto pGetDpiForMonitor = (GetDpiForMonitor_t)GetProcAddress(hShcore, "GetDpiForMonitor");
        if (pGetDpiForMonitor) pGetDpiForMonitor(hMon, 0, &dpiX, &dpiY);
        FreeLibrary(hShcore);
    }
    float dpiScale = dpiY / 96.0f;
    float scaledTaskbarHeight = COMPAT_TASKBAR_HEIGHT * dpiScale;
    float scaledIconSize = COMPAT_ICON_SIZE * dpiScale;

    float iGeomTaskbarTop = (float)mi.rcMonitor.bottom - scaledTaskbarHeight;

    HWND hTrayGeom = FindTaskbarForMonitor(hMon);
    if (hTrayGeom) {
        RECT tr;
        if (GetWindowRect(hTrayGeom, &tr)) {
            int th = tr.bottom - tr.top;
            if (th > 0) iGeomTaskbarTop = (float)tr.top;
        }
    }

    Geometry iGeom = {
        (float)data->targetDockX - 11.0f,
        iGeomTaskbarTop,
        22.0f,
        scaledIconSize
    };
    // Close animation: flow to the middle of the chosen screen edge instead of
    // the taskbar button (MrCrustyToast's close targets).
    if (data->isClosing) {
        const int direction = g_closeDirection.load(std::memory_order_relaxed);
        iGeom.height = 22.0f;
        if (direction == 0) {
            iGeom.x = (float)mi.rcMonitor.left;
            iGeom.y = (float)(mi.rcMonitor.top + mi.rcMonitor.bottom) * 0.5f;
        } else if (direction == 1) {
            iGeom.x = (float)(mi.rcMonitor.right - 22);
            iGeom.y = (float)(mi.rcMonitor.top + mi.rcMonitor.bottom) * 0.5f;
        } else if (direction == 3) {
            iGeom.x = (float)(mi.rcMonitor.left + mi.rcMonitor.right) * 0.5f - 11.0f;
            iGeom.y = (float)mi.rcMonitor.bottom - iGeom.height;
        } else {
            iGeom.x = (float)(mi.rcMonitor.left + mi.rcMonitor.right) * 0.5f - 11.0f;
            iGeom.y = (float)mi.rcMonitor.top;
        }
    }

    // Mesh resolution from the tile-count setting (Potassiumuncher's v1.5).
    int xTiles = g_tileCount.load(std::memory_order_relaxed);
    int yTiles = xTiles;

    // Tight bounding-box canvas: the genie mesh is written in screen-space
    // coordinates and only ever occupies the region between the window rect and
    // the dock target. Rendering over the full virtual desktop wasted CPU on
    // every frame - UpdateLayeredWindow of a whole-screen layered window is slow,
    // most visibly on AMD drivers. A ~W/2 pad alone can't bound the mesh's sway
    // term in CalculateLampVertexMacOS (its amplitude scales with the
    // window-to-dock distance, not the window width), so the horizontal pad is
    // the exact sway bound: max(|w.x-i.x|, |w.right-i.right|)/7.
    const int origLeftB = data->targetRect.left;
    const int wB = data->width;
    const int dockXB = data->isClosing ? (int)(iGeom.x + iGeom.width * 0.5f)
                                       : data->targetDockX;

    const float iLeftF = iGeom.x;
    const float iWidthF = iGeom.width;
    const float sway = fmaxf(fabsf((float)origLeftB - iLeftF),
                             fabsf((float)origLeftB + wB - iLeftF - iWidthF)) / 7.0f;
    int padX = (int)((wB / 2 > sway ? wB / 2 : sway) + 1);

    // iGeom is known here, so a top/side taskbar (iGeom.y above/beside the
    // window) is covered: the box spans min..max of the window and the icon rect,
    // with no band below the taskbar top ever wasted.
    int boundLeft   = ((origLeftB < dockXB) ? origLeftB : dockXB) - padX;
    int boundRight  = (((origLeftB + wB) > dockXB) ? (origLeftB + wB) : dockXB) + padX;
    int boundTop    = (int)fminf(wGeom.y, iGeom.y);
    int boundBottom = (int)fmaxf(wGeom.y + wGeom.height, iGeom.y + iGeom.height);

    // Trim the box to the union of the monitors actually involved, but never
    // clip the window rect or the dock target column away.
    RECT monUnion = GetGenieMonitorUnion(data->targetRect, data->hMon);
    ClampGenieCanvas(boundLeft, boundTop, boundRight, boundBottom,
                     data->targetRect, dockXB, monUnion);

    int vLeft = boundLeft;
    int vTop = boundTop;
    int vWidth = boundRight - boundLeft;
    int vHeight = (boundBottom - boundTop) - 1;
    if (vWidth < 1) vWidth = 1;
    if (vHeight < 1) vHeight = 1;

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

    // Full-screen layered ghost. Created hidden (plain STATIC class, torn down by
    // this thread) so we keep MINE's in-thread ghost lifecycle instead of HIS's
    // registered window class + g_activeGhosts vector.
    HWND hGhost = CreateWindowExW(
        WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_NOACTIVATE,
        L"STATIC", NULL, WS_POPUP,
        vLeft, vTop, vWidth, vHeight,
        NULL, NULL, NULL, NULL);

    // --- Direct2D setup (Potassiumuncher's engine) ---
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
            // Potassiumuncher's v1.5: text AA fixed once at creation (the geometry
            // AA mode is set per frame in the draw loop below).
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

    const double animDur = (double)data->durationMs;
    LARGE_INTEGER qpcFreq, qpcStart, qpcNow;
    QueryPerformanceFrequency(&qpcFreq);
    QueryPerformanceCounter(&qpcStart);

    std::vector<std::vector<D2D1_POINT_2F>> grid(yTiles + 1, std::vector<D2D1_POINT_2F>(xTiles + 1));

    ID2D1PathGeometry* cachedOutlineGeo = nullptr;
    float cachedLeft = -1e9f, cachedRight = -1e9f, cachedTop = -1e9f, cachedBottom = -1e9f;

    BOOL firstFrame = TRUE;

    // Frame pacing, 2x the display's refresh rate (or 240 fps if we can't read
    // it): the pacer is a rate LIMITER, not a vsync gate. Dropping the old
    // per-frame DwmFlush removed the render_time + vsync serialization that cut
    // delivered frames in half on fast displays; this timer caps the loop below
    // a busy spin while still handing DWM a fresh frame at every compose tick.
    double frameIntervalMs = 1000.0 / 240.0;   // fallback
    DWM_TIMING_INFO ti = { sizeof(ti) };
    if (SUCCEEDED(DwmGetCompositionTimingInfo(NULL, &ti)) && ti.rateRefresh.uiDenominator) {
        double hz = (double)ti.rateRefresh.uiNumerator / ti.rateRefresh.uiDenominator;
        frameIntervalMs = 1000.0 / (hz * 2.0);
    }
    HANDLE hFrameTimer = CreateWaitableTimerExW(
        nullptr, nullptr, CREATE_WAITABLE_TIMER_HIGH_RESOLUTION,
        TIMER_MODIFY_STATE | SYNCHRONIZE);
    if (!hFrameTimer) {
        // Pre-1803: the high-resolution flag isn't recognized. A plain waitable
        // timer paces coarser, but it still isn't a spin.
        hFrameTimer = CreateWaitableTimerExW(nullptr, nullptr, 0,
                                             TIMER_MODIFY_STATE | SYNCHRONIZE);
    }

    if (d2dOk) {
        for (;;) {
            QueryPerformanceCounter(&qpcNow);
            const LARGE_INTEGER qpcFrameStart = qpcNow;
            double elapsedMs = (qpcNow.QuadPart - qpcStart.QuadPart) * 1000.0 / qpcFreq.QuadPart;
            BOOL lastFrame = (elapsedMs >= animDur);

            float raw_p = (float)fmin(elapsedMs / animDur, 1.0);
            float eased_p = raw_p * raw_p * (3.0f - 2.0f * raw_p);
            // Minimize runs 0->1, restore runs 1->0 so the same warp plays in reverse.
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
                    // Warp this mesh vertex. CalculateLampVertexMacOS returns SCREEN
                    // (virtual-desktop) coordinates - wGeom / iGeom are both
                    // screen-space (targetRect, targetDockX, taskbar-top Y are all
                    // virtual-screen). The ONLY conversion to the ghost's
                    // bounding-box canvas is the (px - vLeft, py - vTop)
                    // subtraction below - vLeft/vTop are negative on monitors left of
                    // / above the primary. Every mesh coordinate passes through here
                    // exactly once; nothing downstream re-offsets.
                    CalculateLampVertex(tx, ty, t, wGeom, iGeom, &px, &py);
                    grid[y][x] = D2D1::Point2F(px - vLeft, py - vTop);
                }
            }

            bool degenerate = false;
            ID2D1PathGeometry* outlineGeo = nullptr;
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

                // Belt-and-suspenders guard: if the warped mesh has collapsed to a
                // near-zero-width/height box, don't present it - that is what would
                // flash as a single horizontal line for one frame (seen at the tail
                // of the collapse, and at the very start of a restore where the mesh
                // emerges from the dock). Skipping these frames removes the flash
                // regardless of monitor / arrangement.
                degenerate = (right - left) < 2.0f || (bottom - top) < 2.0f;

                bool needRebuild = (cachedOutlineGeo == nullptr)
                    || fabsf(left   - cachedLeft)   > 0.5f
                    || fabsf(right  - cachedRight)  > 0.5f
                    || fabsf(top    - cachedTop)    > 0.5f
                    || fabsf(bottom - cachedBottom) > 0.5f;

                if (needRebuild) {
                    if (cachedOutlineGeo) { cachedOutlineGeo->Release(); cachedOutlineGeo = nullptr; }
                    cachedLeft = left; cachedRight = right; cachedTop = top; cachedBottom = bottom;

                    float w = right - left;
                    float h = bottom - top;
                    float r = fminf(12.0f, fminf(w, h) / 2.0f);   // Potassiumuncher's v1.5 radius
                    const float K = 0.5523f;
                    float L = left, T = top, R = right, B = bottom;

                    g_d2dFactory->CreatePathGeometry(&cachedOutlineGeo);
                    if (cachedOutlineGeo) {
                        ID2D1GeometrySink* sink = nullptr;
                        cachedOutlineGeo->Open(&sink);
                        sink->BeginFigure(D2D1::Point2F(L + r, T), D2D1_FIGURE_BEGIN_FILLED);
                        sink->AddLine(D2D1::Point2F(R - r, T));
                        { D2D1_BEZIER_SEGMENT s;
                          s.point1 = D2D1::Point2F(R - r + K*r, T);
                          s.point2 = D2D1::Point2F(R, T + r - K*r);
                          s.point3 = D2D1::Point2F(R, T + r);
                          sink->AddBezier(s); }
                        sink->AddLine(D2D1::Point2F(R, B - r));
                        { D2D1_BEZIER_SEGMENT s;
                          s.point1 = D2D1::Point2F(R, B - r + K*r);
                          s.point2 = D2D1::Point2F(R - r + K*r, B);
                          s.point3 = D2D1::Point2F(R - r, B);
                          sink->AddBezier(s); }
                        sink->AddLine(D2D1::Point2F(L + r, B));
                        { D2D1_BEZIER_SEGMENT s;
                          s.point1 = D2D1::Point2F(L + r - K*r, B);
                          s.point2 = D2D1::Point2F(L, B - r + K*r);
                          s.point3 = D2D1::Point2F(L, B - r);
                          sink->AddBezier(s); }
                        sink->AddLine(D2D1::Point2F(L, T + r));
                        { D2D1_BEZIER_SEGMENT s;
                          s.point1 = D2D1::Point2F(L, T + r - K*r);
                          s.point2 = D2D1::Point2F(L + r - K*r, T);
                          s.point3 = D2D1::Point2F(L + r, T);
                          sink->AddBezier(s); }
                        sink->EndFigure(D2D1_FIGURE_END_CLOSED);
                        sink->Close();
                        sink->Release();
                    }
                }
                outlineGeo = cachedOutlineGeo;
            }

            if (outlineGeo) {
                D2D1_LAYER_PARAMETERS layerParams = D2D1::LayerParameters();
                layerParams.geometricMask = outlineGeo;
                // Per-primitive AA on the outline mask (Potassiumuncher's v1.5):
                // smooth silhouette edges without per-tile seam artifacts.
                layerParams.maskAntialiasMode = D2D1_ANTIALIAS_MODE_PER_PRIMITIVE;
                // nullptr layer: D2D manages its own layer pool, avoiding a fresh
                // ID2D1Layer create/release on every frame.
                rt->PushLayer(&layerParams, nullptr);

                for (int y = 0; y < yTiles; y++) {
                    for (int x = 0; x < xTiles; x++) {
                        D2D1_POINT_2F p1 = grid[y][x];
                        D2D1_POINT_2F p2 = grid[y][x+1];
                        D2D1_POINT_2F p3 = grid[y+1][x];
                        D2D1_POINT_2F p4 = grid[y+1][x+1];

                        D2D1_POINT_2F c = D2D1::Point2F((p1.x+p2.x+p3.x+p4.x)/4.0f, (p1.y+p2.y+p3.y+p4.y)/4.0f);

                        float quadW = fmaxf(fabsf(p2.x - p1.x), fabsf(p4.x - p3.x));
                        float quadH = fmaxf(fabsf(p3.y - p1.y), fabsf(p4.y - p2.y));
                        // Potassiumuncher's v1.5 bloat: slightly larger overlap so
                        // tile seams stay closed at higher tile counts.
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
                rt->PopLayer();
            }
            rt->EndDraw();

            // Only present / reveal the ghost on a non-degenerate frame, so a
            // collapsed mesh never flashes as a line. The ghost stays hidden until
            // the first real frame (restore) and simply holds its last good frame if
            // the tail collapses (minimize) - both invisible to the eye.
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

            // Mod is being unloaded: cut the animation short. The teardown below
            // still reveals the real window (rising) and re-enables its DWM
            // transitions.
            if (g_unloading.load(std::memory_order_relaxed)) break;

            // The first real frame has been presented on the ghost. For a
            // minimize the hook is holding the REAL minimize (or, on the
            // auto-hide path, the cloak-hide) back until this moment - otherwise
            // the window would vanish a few frames before the genie appears.
            // One DwmFlush here guarantees it is composed on screen before the
            // handoff; every subsequent frame is capped only by the pacer below.
            if (firstFrame && !degenerate) {
                DwmFlush();
                firstFrame = FALSE;
                if (data->hFirstFrameShown) SetEvent(data->hFirstFrameShown);
            }

            // Rate-limit to ~2x refresh so we never busy-spin. The deadline is
            // measured from the START of the frame, so the period is
            // max(render, interval), not render + interval (a render faster than
            // the interval just sleeps the difference; a slower one runs as-is).
            // Capped wait keeps the g_unloading check responsive. When a frame
            // overruns its budget, SwitchToThread still yields the quantum.
            if (hFrameTimer) {
                LARGE_INTEGER qpcAfter;
                QueryPerformanceCounter(&qpcAfter);
                double spentMs = (qpcAfter.QuadPart - qpcFrameStart.QuadPart) * 1000.0 / qpcFreq.QuadPart;
                double waitMs = frameIntervalMs - spentMs;
                if (waitMs > 0.1) {
                    LARGE_INTEGER due;
                    due.QuadPart = -(LONGLONG)(waitMs * 10000.0);   // 100ns units
                    SetWaitableTimer(hFrameTimer, &due, 0, nullptr, nullptr, FALSE);
                    WaitForSingleObject(hFrameTimer, 100);
                } else {
                    SwitchToThread();   // overran the budget - yield the quantum anyway
                }
            } else {
                Sleep(1);   // last resort: never spin
            }
        }
    }

    if (hFrameTimer) { CloseHandle(hFrameTimer); hFrameTimer = nullptr; }
    if (cachedOutlineGeo) { cachedOutlineGeo->Release(); cachedOutlineGeo = nullptr; }

    // -------------------- COMMON TEARDOWN --------------------
    if (data->isRising) {
        // Reveal the real window the same way it was hidden.
        if (data->hiddenByCloak) {
            MacGenieSetCloak(data->hRealWnd, FALSE);
        } else {
            SetLayeredWindowAttributes(data->hRealWnd, 0, 255, LWA_ALPHA);
            if (!(data->originalExStyle & WS_EX_LAYERED)) {
                SetWindowLongPtrW(data->hRealWnd, GWL_EXSTYLE, data->originalExStyle);
            }
        }
    }

    // For the auto-hide deferred path we keep DWM transitions disabled through the
    // real (deferred) minimize just below, so the system's own minimize animation
    // never plays; a close finishes its own window below; every other path
    // re-enables them now.
    if (!data->deferredMinimize && !data->isClosing) {
        MacGenieSetDwmTransitions(data->hRealWnd, TRUE);
    }
    if (data->isClosing) MacGenieFinishClose(data);

    if (bmpBrush) { bmpBrush->Release(); bmpBrush = nullptr; }
    if (snapshotBmp) { snapshotBmp->Release(); snapshotBmp = nullptr; }
    if (rt) { rt->Release(); rt = nullptr; }

    SelectObject(hMemDC, hOldBmp);
    DeleteObject(hTargetBmp);
    DeleteObject(data->hBitmap);   // frees data->pBits (DIB section)
    DeleteDC(hMemDC);
    ReleaseDC(NULL, hScreenDC);
    DestroyWindow(hGhost);

    // Unblock the waiting minimize hook (in case we bailed before the first frame
    // was ever composed), then release the event (this thread owns it).
    if (data->hFirstFrameShown) {
        SetEvent(data->hFirstFrameShown);
        CloseHandle(data->hFirstFrameShown);
    }

    // Auto-hide "unhide" completion - Potassiumuncher's engine, reconciled with
    // MINE's cloak hide. The caller cloaked the real window after our first frame
    // (flash-free, unlike HIS's WS_EX_LAYERED+alpha hide) and deferred the real
    // minimize to us so the revealed taskbar didn't slide away mid-animation.
    if (data->deferredMinimize) {
        int unhideMs = data->unhideDurationMs;
        int animMs = data->durationMs;
        if (data->requestedUnhide && unhideMs > animMs) {
            // Chunked sleep so an unload (which joins this thread) never waits out
            // the up-to-~5 s unhide delay.
            int remaining = unhideMs - animMs;
            while (remaining > 0 && !g_unloading.load(std::memory_order_relaxed)) {
                int chunk = remaining < 20 ? remaining : 20;
                Sleep(chunk);
                remaining -= chunk;
            }
        }

        // Perform the delayed minimize with transitions still disabled (instant, no
        // system animation). GenieBypass stops our own DefWindowProcW hook from
        // re-triggering the genie. Hardened with IsWindow guards (HIS's unhide path
        // could race a closed window / silent SetForegroundWindow failure).
        if (!g_unloading.load(std::memory_order_relaxed) && IsWindow(data->hRealWnd)) {
            SetPropW(data->hRealWnd, L"GenieBypass", (HANDLE)1);
            SendMessageTimeoutW(data->hRealWnd, WM_SYSCOMMAND, SC_MINIMIZE, 0, SMTO_NORMAL, 1000, NULL);
            RemovePropW(data->hRealWnd, L"GenieBypass");
        }
        // Always uncloak so we never leave an invisible, non-minimized window, then
        // re-enable transitions for future system animations.
        if (IsWindow(data->hRealWnd)) {
            MacGenieSetCloak(data->hRealWnd, FALSE);
            MacGenieSetDwmTransitions(data->hRealWnd, TRUE);
        }
        // Shift focus so the taskbar slides back down natively.
        if (data->requestedUnhide && !g_unloading.load(std::memory_order_relaxed)) {
            if (data->hNextApp && IsWindow(data->hNextApp) && IsWindowVisible(data->hNextApp)) {
                SetForegroundWindow(data->hNextApp);
            } else {
                SetForegroundWindow(FindWindowW(L"Progman", NULL));
            }
        }
    }

    {
        std::lock_guard<std::mutex> lock(g_CacheMutex);
        g_AnimActive.erase(data->hRealWnd);
    }
    delete data;
    return 0;
}

// -------------------------------------------------------------------------
// CLASSIC engine: the mod's original (v2.2.0) per-output-scanline GDI strip warp,
// kept as an opt-in alternative to Potassiumuncher's Direct2D engine (some users
// preferred it). It consumes the SAME MacGenieAnimData contract the modern engine
// does - snapshot bitmap, extended-frame rect, dock target, first-frame event - and
// runs the SAME teardown (reveal, DWM transitions, auto-hide deferred minimize,
// unload join), so it drops in behind the identical setup/capture path; only the
// per-frame rendering differs. Selected by the "Animation style" setting.
// -------------------------------------------------------------------------
DWORD WINAPI MacGenieAnimThreadClassic(LPVOID lpParam) {
    MacGenieAnimData* data = (MacGenieAnimData*)lpParam;

    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST);

    const int W = data->width;
    const int H = data->height;
    // The monitor the genie plays on, in virtual-screen coords (from the shared
    // setup: the primary when multi-monitor support is off, else the window's).
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

    // Where the genie funnels to: the learned taskbar icon X. The dock Y is taken
    // from the real per-monitor taskbar window (the same signal the modern engine
    // uses), so a taskbar pinned to the top of the screen is honored too -
    // previously the classic style always assumed the taskbar was at the bottom of
    // the monitor. A single global shell query would get the wrong edge on setups
    // where each monitor's taskbar can be on a different edge.
    int dockX = data->targetDockX;
    if (dockX < mon.left) dockX = mon.left;
    if (dockX > mon.right) dockX = mon.right;
    float dockY = (float)mon.bottom;
    if (HWND hTray = FindTaskbarForMonitor(data->hMon)) {
        RECT tr;
        if (GetWindowRect(hTray, &tr) && tr.bottom > tr.top &&
            (tr.top + tr.bottom) < (mon.top + mon.bottom)) {   // taskbar in the upper half
            dockY = (float)mon.top;
        }
    }
    // Close animation: the middle of the chosen screen edge (MrCrustyToast).
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
    // Which end leads the morph. Taken from the window's mid-line so a dock that
    // lands exactly on the window edge still picks a sane direction, and kept
    // outside the window's own vertical span so the yb[] map below stays
    // monotonic (a hard requirement of the scanline walk).
    const float origBottomF = (float)(origTop + H);
    const bool dockAbove = dockY < ((float)origTop + origBottomF) * 0.5f;
    if (dockAbove) { if (dockY > (float)origTop) dockY = (float)origTop; }
    else           { if (dockY < origBottomF)    dockY = origBottomF; }
    float neckW = W * 0.03f;
    if (neckW < 12.0f) neckW = 12.0f;
    if (neckW > 60.0f) neckW = 60.0f;

    // Bounding box the funnel can occupy. Both the window's own monitor and the
    // monitor it funnels to are unioned; the box is trimmed to them but never
    // allowed to clip the window rect or the dock column away (shared helper -
    // the same clipping bug the modern engine had, when multi-monitor support is
    // off and the window sits on a secondary display).
    int boundLeft   = (origLeft < dockX ? origLeft : dockX) - W / 2;
    int boundRight  = ((origLeft + W) > dockX ? (origLeft + W) : dockX) + W / 2;
    // Span from the dock edge up/down to the window so a top taskbar (dock above
    // the window) is inside the canvas instead of below the box.
    int boundTop    = std::min<int>(origTop, (int)dockY);
    int boundBottom = std::max<int>(origTop + H, (int)dockY);
    RECT monUnion = GetGenieMonitorUnion(data->targetRect, data->hMon);
    ClampGenieCanvas(boundLeft, boundTop, boundRight, boundBottom,
                     data->targetRect, dockX, monUnion);
    int boundW = boundRight - boundLeft;
    int boundH = boundBottom - boundTop;
    if (boundW < 1) boundW = 1;
    if (boundH < 1) boundH = 1;

    // Layered "ghost" window (plain STATIC class - no custom WndProc, so no clash
    // with the modern engine's ghost class). Created hidden, shown after frame 1.
    HWND hGhost = CreateWindowExW(
        WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_TOPMOST | WS_EX_TRANSPARENT,
        L"STATIC", NULL, WS_POPUP,
        boundLeft, boundTop, boundW, boundH,
        NULL, NULL, NULL, NULL);

    HDC hScreenDC = GetDC(NULL);

    // Source pixels as a readable 32-bit top-down DIB.
    HDC hSrcDC = CreateCompatibleDC(hScreenDC);
    HBITMAP hOldSrc = (HBITMAP)SelectObject(hSrcDC, data->hBitmap);

    BITMAPINFO sbmi;
    ZeroMemory(&sbmi, sizeof(sbmi));
    sbmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    sbmi.bmiHeader.biWidth = W;
    sbmi.bmiHeader.biHeight = -H;       // negative height = top-down
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

    // Canvas: 32-bit top-down DIB for genuine per-pixel alpha.
    BITMAPINFO bmi;
    ZeroMemory(&bmi, sizeof(bmi));
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = boundW;
    bmi.bmiHeader.biHeight = -boundH;   // negative height = top-down
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;
    BYTE* pBits = NULL;
    HBITMAP hCanvas = CreateDIBSection(hScreenDC, &bmi, DIB_RGB_COLORS, (void**)&pBits, NULL, 0);
    HDC hCanvasDC = CreateCompatibleDC(hScreenDC);
    HBITMAP hOldCanvas = (HBITMAP)SelectObject(hCanvasDC, hCanvas);
    const int canvasStride = boundW * 4;

    const float SPREAD = 0.65f;   // rows nearest the dock lead the morph (the
                                  // spread v is flipped when the dock is above)
    const size_t canvasBytes = (size_t)boundW * 4 * boundH;

    const double totalMs = (double)data->durationMs;

    auto morphAt = [&](float v, float tt) -> float {
        float m = tt * (1.0f + SPREAD) - (1.0f - v) * SPREAD;
        if (m < 0.0f) m = 0.0f;
        if (m > 1.0f) m = 1.0f;
        return m * m * (3.0f - 2.0f * m);
    };

    float* yb = new float[H + 1];

    // Time-based progress; frames paced at 2x refresh below (no per-frame
    // DwmFlush, same as the modern engine, so the classic warp hits the same
    // half-rate cliff only if its per-frame work ever exceeds the interval).
    LARGE_INTEGER qpcFreq, qpcStart, qpcNow;
    QueryPerformanceFrequency(&qpcFreq);
    QueryPerformanceCounter(&qpcStart);

    // Frame pacing, 2x the display's refresh rate (fallback 240 fps).
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
        // Pre-1803: the high-resolution flag isn't recognized. A plain waitable
        // timer paces coarser, but it still isn't a spin.
        hFrameTimer = CreateWaitableTimerExW(nullptr, nullptr, 0,
                                             TIMER_MODIFY_STATE | SYNCHRONIZE);
    }

    BOOL firstFrame = TRUE;
    for (;;) {
        QueryPerformanceCounter(&qpcNow);
        const LARGE_INTEGER qpcFrameStart = qpcNow;
        double elapsedMs = (qpcNow.QuadPart - qpcStart.QuadPart) * 1000.0 / qpcFreq.QuadPart;
        BOOL lastFrame = (elapsedMs >= totalMs);
        float progress = lastFrame ? 1.0f : (float)(elapsedMs / totalMs);

        // 0 = full window at its place, 1 = collapsed at the dock; restore reverses.
        float tt = data->isRising ? (1.0f - progress) : progress;

        float fade = 1.0f;
        if (tt > 0.8f) fade = (1.0f - tt) / 0.2f;
        if (fade < 0.0f) fade = 0.0f;
        if (fade > 1.0f) fade = 1.0f;

        memset(pBits, 0, canvasBytes);

        // Vertical map: where each source row lands on screen this frame. The rows
        // nearest the dock lead the morph (bottom-first neck when the dock is
        // below the window, top-first when it's above), so the spread v is
        // flipped accordingly.
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
                // Snapshots carry premultiplied per-pixel alpha since the v1.5
                // capture fix (translucent regions have a<255); propagate it so
                // acrylic areas stay see-through in the classic style too. The
                // constant alpha below still applies the global fade on top.
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

        // First frame is on screen; release the minimize hook it was holding.
        // One DwmFlush here matches the modern engine's handoff sync; subsequent
        // frames are paced by the timer below instead of a per-frame DwmFlush.
        if (firstFrame) {
            DwmFlush();
            firstFrame = FALSE;
            if (data->hFirstFrameShown) SetEvent(data->hFirstFrameShown);
        }

        // Rate-limit to ~2x refresh so we never busy-spin. The deadline is
        // measured from the START of the frame, so the period is
        // max(render, interval), not render + interval (a render faster than
        // the interval just sleeps the difference; a slower one runs as-is).
        // Capped wait keeps the g_unloading check responsive. When a frame
        // overruns its budget, SwitchToThread still yields the quantum.
        if (hFrameTimer) {
            LARGE_INTEGER qpcAfter;
            QueryPerformanceCounter(&qpcAfter);
            double spentMs = (qpcAfter.QuadPart - qpcFrameStart.QuadPart) * 1000.0 / qpcFreq.QuadPart;
            double waitMs = frameIntervalMs - spentMs;
            if (waitMs > 0.1) {
                LARGE_INTEGER due;
                due.QuadPart = -(LONGLONG)(waitMs * 10000.0);   // 100ns units
                SetWaitableTimer(hFrameTimer, &due, 0, nullptr, nullptr, FALSE);
                WaitForSingleObject(hFrameTimer, 100);
            } else {
                SwitchToThread();   // overran the budget - yield the quantum anyway
            }
        } else {
            Sleep(1);   // last resort: never spin
        }
    }

    if (hFrameTimer) { CloseHandle(hFrameTimer); hFrameTimer = nullptr; }

    // -------------------- COMMON TEARDOWN (identical to the modern engine) -----
    if (data->isRising) {
        // Reveal the real window the same way it was hidden.
        if (data->hiddenByCloak) {
            MacGenieSetCloak(data->hRealWnd, FALSE);
        } else {
            SetLayeredWindowAttributes(data->hRealWnd, 0, 255, LWA_ALPHA);
            if (!(data->originalExStyle & WS_EX_LAYERED)) {
                SetWindowLongPtrW(data->hRealWnd, GWL_EXSTYLE, data->originalExStyle);
            }
        }
    }

    // The auto-hide deferred path keeps transitions disabled through the real
    // (deferred) minimize below; a close finishes its own window below; every
    // other path re-enables them now.
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

    // Auto-hide "unhide" completion (Potassiumuncher's engine) - same as the modern
    // thread, so the auto-hide feature behaves identically under the classic style.
    if (data->deferredMinimize) {
        int unhideMs = data->unhideDurationMs;
        int animMs = data->durationMs;
        if (data->requestedUnhide && unhideMs > animMs) {
            // Chunked sleep so an unload (which joins this thread) never waits out
            // the up-to-~5 s unhide delay.
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

    {
        std::lock_guard<std::mutex> lock(g_CacheMutex);
        g_AnimActive.erase(data->hRealWnd);
    }
    delete data;
    return 0;
}

// -------------------------------------------------------------------------
// Core Setup Engine & Smart Tracking Logic
// -------------------------------------------------------------------------
// Returns TRUE if the worker thread was spawned (the animation is now in flight).
// `originalExStyle` is the window's TRUE extended style, captured by the caller
// BEFORE any WS_EX_LAYERED was added for the hide (rising callers hide first).
// `cloakHidden` says how a rising caller hid the window: TRUE = DWM cloak (restore
// and launch), FALSE = WS_EX_LAYERED + alpha 0 (launch of a Firefox-family first
// window). The deferred* args drive the auto-hide
// "unhide" path (falling only): the caller cloaks the window after our first frame
// and we perform the real minimize at teardown. `isClosing` / `closeMsg` drive the
// close animation (falling only; see MacGenieFinishClose).
bool StartMacGenieAnim(HWND hWnd, BOOL rising, LONG_PTR originalExStyle,
                       BOOL cloakHidden = FALSE, BOOL deferredMinimize = FALSE,
                       BOOL requestedUnhide = FALSE, HWND hNextApp = NULL,
                       BOOL isClosing = FALSE, UINT closeMsg = WM_DESTROY) {
    // Bound the snapshot cache: windows closed while minimized (the common
    // WM_DESTROY + PostQuitMessage path never reaches DefWindowProcW, where the
    // usual cleanup lives) would otherwise leave their full DIB behind until
    // unload. Cheap sweep at the single animation entry point.
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

    // "Excluded programs" gate - single choke point for every animation path
    // (all six minimize/restore hooks AND the launch thread funnel through here).
    // Bail exactly like the other early-outs so hook-side state is restored: a
    // rising caller gets its hide undone, a falling caller gets DWM transitions
    // back so the DEFAULT system animation plays for the excluded app.
    if (MacGenieIsExcluded(hWnd)) {
        if (rising) MacGenieUndoRisingHide(hWnd, originalExStyle, cloakHidden);
        else MacGenieSetDwmTransitions(hWnd, TRUE);
        return false;
    }

    // Animation geometry via DWM extended frame bounds (Goal D / bug #4670):
    // GetWindowRect returns the legacy frame (invisible resize borders / shadow),
    // which is offset a few px from where the pixels actually are on composited
    // windows. Snapshot AND warp both use the extended-frame rect, so keyboard /
    // AutoHotkey minimizes are pixel-aligned. Scheme from Potassiumuncher's engine.
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

    // One genie per window at a time (MINE). Any window we animate also counts as
    // "seen", so the launch path won't fire on it later. Refused while unloading.
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

    // Monitor the genie targets. Multi-monitor support (experimental) targets the
    // window's monitor; otherwise the primary, which reproduces the old behavior.
    // A close always flows to an edge of the window's own monitor.
    bool multiMon = isClosing || g_multiMonitor.load(std::memory_order_relaxed);
    HMONITOR hMon = multiMon
        ? MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST)
        : MonitorFromPoint(POINT{0, 0}, MONITOR_DEFAULTTOPRIMARY);
    MONITORINFO mi;
    mi.cbSize = sizeof(mi);
    if (!GetMonitorInfoW(hMon, &mi)) {
        mi.rcMonitor.left = 0; mi.rcMonitor.top = 0;
        mi.rcMonitor.right = GetSystemMetrics(SM_CXSCREEN);
        mi.rcMonitor.bottom = GetSystemMetrics(SM_CYSCREEN);
    }
    int monWidth = mi.rcMonitor.right - mi.rcMonitor.left;

    // Default dock target before UIA (Potassiumuncher's engine): left-aligned
    // taskbars start near the monitor's left, centered ones at its middle.
    // A close doesn't target the taskbar (the render thread picks the edge from the
    // close-direction setting), so it skips the registry, cursor and UIA lookups.
    int learnedTargetX = mi.rcMonitor.left + monWidth / 2;
    if (!isClosing) {
        DWORD alignVal = 1, dataSize = sizeof(alignVal);
        RegGetValueW(HKEY_CURRENT_USER,
                     L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced",
                     L"TaskbarAl", RRF_RT_REG_DWORD, NULL, &alignVal, &dataSize);
        learnedTargetX = (alignVal == 0) ? (mi.rcMonitor.left + 160)
                                         : (mi.rcMonitor.left + monWidth / 2);

        // --- Per-window targeting ---
        // UI Automation (GetTaskbarButtonX) matches taskbar buttons by title / process
        // name, which can't reliably tell apart several windows of the SAME process
        // (e.g. multiple Brave profiles): they tie on the process-name score and it
        // resolves them all to the first-opened window's button. So MINE's cursor +
        // per-window signal takes priority; UIA is only a fallback for a window we've
        // never seen minimized from the taskbar (frame 0 of the warp is the identity,
        // so targetDockX is only needed for later frames).
        //
        // 1. Cursor over the taskbar = the user clicked THIS window's own taskbar
        //    button, so its X is exactly the icon - the only fully reliable per-window
        //    signal when windows share a process. Learn it, keyed by HWND (never shared
        //    across sibling windows).
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

        if (!PtInRect(&workArea, pt)) {
            learnedTargetX = pt.x;                        // steal the click X
            std::lock_guard<std::mutex> lock(g_CacheMutex);
            g_IconPositions[hWnd] = learnedTargetX;       // remember it for this window
        } else {
            // 2. Title-bar / keyboard minimize: locate the taskbar button via UI
            //    Automation EVERY time (Potassiumuncher's targeting). Reusing a
            //    per-window cache here made the button-minimize land past the icon when
            //    the cached spot was stale; running UIA fresh lands it on the icon.
            //    GetTaskbarButtonX still falls back to the per-window / per-process cache
            //    (including a position learned from an earlier taskbar click) internally
            //    if UIA can't match, so nothing is lost.
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
    data->durationMs = g_durationMs.load(std::memory_order_relaxed);
    data->requestedUnhide = requestedUnhide;
    data->hNextApp = hNextApp;
    data->unhideDurationMs = g_unhideDurationMs.load(std::memory_order_relaxed);
    data->deferredMinimize = deferredMinimize;
    data->isClosing = isClosing;
    data->closeMsg = closeMsg;
    data->pBits = nullptr;
    data->hBitmap = NULL;

    HDC hScreenDC = GetDC(NULL);

    // Premultiplied 32-bit top-down DIB - D2D's CreateBitmap reads pBits directly.
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

    // Copy a rawW x rawH PrintWindow capture into the w x h snapshot at the
    // extended-frame offset. Potassiumuncher's v1.5 grey-screen fix: PrintWindow
    // (PW_RENDERFULLCONTENT) returns pixels that are ALREADY premultiplied, so pass
    // them through untouched and keep a==0 pixels fully TRANSPARENT. The old code
    // forced every pixel opaque, which flattened acrylic / Mica backdrop regions
    // (alpha 0 or partial) into a solid grey - the "grey top" on translucent
    // windows. With alpha preserved, those regions stay see-through during the
    // genie and the desktop shows through them, matching the window's real look.
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
        // Fallback: on NON-PRIMARY monitors DWM often returns the entire Mica /
        // acrylic backdrop region as a==0 - with alpha preserved the whole window
        // background would simply VANISH during the genie (reported on Explorer on
        // a secondary display). If most of the window came back fully transparent,
        // the alpha channel clearly isn't describing real translucency, so force
        // those pixels opaque - restoring the pre-3.1 dark-backdrop look for this
        // case, which beats a see-through hole. Windows where transparency is
        // sparse (rounded corners, small blur regions) keep the faithful alpha.
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

    // Window-only capture: grab the window into a rawW x rawH DIB via PrintWindow,
    // then CopySnapshot into the extended-frame snapshot. PrintWindow renders only
    // the window's own content, so the taskbar / other windows can never bleed into
    // the animation (Potassiumuncher's taskbar fix) - and it also works on a
    // cloaked window (the rising-no-cache path), where a screen grab would not.
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
        // A closing foreground window is grabbed from the screen instead
        // (MrCrustyToast): at close time Chromium's PrintWindow can return only
        // its title-bar band. It is on top, so the grab is the window itself.
        BOOL screenGrab = isClosing && GetForegroundWindow() == hWnd &&
                          BitBlt(hTempDC, 0, 0, rawW, rawH, hScreenDC,
                                 winRect.left, winRect.top, SRCCOPY);
        if (!screenGrab) PrintWindow(hWnd, hTempDC, PW_RENDERFULLCONTENT);
        GdiFlush();
        CopySnapshot(pTempBits);
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
        // No cached minimize snapshot (e.g. window was already minimized before the
        // mod loaded): snapshot the freshly-restored (cloaked) window directly.
        if (!fromCache) CaptureNow();
    } else {
        // Falling: capture the WINDOW ONLY via PrintWindow (Potassiumuncher's
        // method), NOT a screen BitBlt. A screen grab of the window rect also pulls
        // in whatever is behind / around the window - most visibly the taskbar on
        // maximized / fullscreen apps, which then warps inside the genie. PrintWindow
        // renders just the window's own content, so the taskbar (a separate window)
        // can never bleed in. Tradeoff: DWM acrylic/translucency backdrops aren't
        // part of the window's own render, so translucent windows animate opaque - a
        // documented limitation, far less jarring than the taskbar appearing.
        CaptureNow();
        // Cache a copy keyed by HWND for the later restore animation (not for a
        // closing window - there's no restore after a close).
        std::lock_guard<std::mutex> lock(g_CacheMutex);
        auto it = g_SnapshotCache.find(hWnd);
        if (it != g_SnapshotCache.end()) {
            DeleteObject(it->second.hBmp);
            g_SnapshotCache.erase(it);
        }
        void* pCacheBits = nullptr;
        // Not while unloading: Wh_ModUninit may already have cleared the cache, and
        // nothing would ever free a bitmap added after that.
        HBITMAP hCacheBmp = (isClosing || g_unloading.load(std::memory_order_relaxed)) ? NULL :
            CreateDIBSection(hScreenDC, &bmi, DIB_RGB_COLORS, &pCacheBits, NULL, 0);
        if (hCacheBmp && pCacheBits) {
            memcpy(pCacheBits, data->pBits, (size_t)w * h * 4);
            g_SnapshotCache[hWnd] = { hCacheBmp, pCacheBits, w, h };
        } else if (hCacheBmp) {
            DeleteObject(hCacheBmp);
        }
    }

    ReleaseDC(NULL, hScreenDC);

    // For a minimize (normal OR deferred) hold the caller back until the ghost's
    // first frame is composed - otherwise the window (or, on the deferred path, the
    // cloak-hide) would happen a few frames before the genie appears (a visible
    // gap, especially on power-throttled CPUs). Two handles to one event: the
    // thread owns its duplicate, we own and wait on the original.
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

    // Same setup and capture for both styles; only the render thread differs.
    LPTHREAD_START_ROUTINE renderProc =
        g_classicEngine.load(std::memory_order_relaxed) ? MacGenieAnimThreadClassic
                                                        : MacGenieAnimThread;
    if (MacGenieSpawnWorker(renderProc, data)) {
        if (hFirstShown) {
            if (waitForFirstFrame) {
                WaitForSingleObject(hFirstShown, 200);   // capped: worst case = old behavior
            }
            CloseHandle(hFirstShown);
        }
        return true;
    }

    // Thread couldn't start (or the mod is unloading) - undo our state so the window
    // isn't left invisible or with transitions permanently disabled.
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

// -------------------------------------------------------------------------
// Hooks
// -------------------------------------------------------------------------
DWORD WINAPI MacGenieLaunchThread(LPVOID lpParam);   // defined below

// Firefox-family browsers (Firefox, Zen, Floorp... all MozillaWindowClass): is
// hWnd the process's first browser window? See MacGenieLaunchPrepare.
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
    // The hooks run in whichever process makes the call, so match the window's
    // owner, not the calling process.
    DWORD pid = 0;
    GetWindowThreadProcessId(hWnd, &pid);
    MacGenieGeckoSearch search = { hWnd, pid, false };
    EnumWindows(MacGenieFindOtherGeckoWindow, (LPARAM)&search);
    return !search.found;
}

// Arm the launch animation: check eligibility, kill DWM transition and hide the
// window *before* the real show call, so neither the window nor the system's open
// animation appears before the genie. Writes the TRUE pre-hide extended style to
// *origExOut and which hide was used to *cloakedOut.
//
// The hide is a DWM cloak, not WS_EX_LAYERED + alpha 0. The layered hide failed on
// winit / tao windows (Slint, Tauri): those frameworks rewrite GWL_EXSTYLE from
// their own flags right after the show (winit's WindowFlags::apply_diff) and on
// every later flag change (tao), which silently drops WS_EX_LAYERED, so the window
// appeared and the genie played over it. Windows also strips WS_EX_LAYERED from
// flip-model swap-chain and DirectComposition windows (WebView2, Skia/wgpu). A
// cloak is a DWM-level hide that no style rewrite touches, and PrintWindow still
// captures the cloaked window's content for the snapshot.
//
// The old layered hide is kept (exactly the v3.1.6 behavior) where a cloak isn't
// appropriate:
//  - the first browser window of a Firefox-family process: a cloak installed
//    before its first show can cross Gecko's first-window / session-restore
//    boundary and make it open a second real window (found and worked around the
//    same way by ReDrag's Windows Animations mod);
//  - a window the app already cloaked itself (WPF / WinUI white-flash workarounds
//    cloak until their first frame): our uncloak would reveal it early;
//  - a window DWM won't let us cloak.
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
    // Excluded programs show untouched (StartMacGenieAnim would refuse them anyway,
    // but only after the hide had already delayed the window). Checked after the
    // seen-set so re-shows of a seen window skip the process lookup.
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

// Second half of the launch sequence: spawn the reveal worker (or roll back).
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

// -------------------------------------------------------------------------
// Close animation (experimental) - contributed by MrCrustyToast, from his MacOS
// Genie Closing Animation mod; its triggers are based on ReDrag's Windows
// Animations. Every path: start a falling genie (StartMacGenieAnim waits for its
// first frame), cloak the real window behind the ghost, then finish the close the
// hook held back (MacGenieFinishClose / the hook itself). Window props:
//  - GenieCloseAnimating: a close genie is in flight for this window;
//  - GenieCloseHandled: this close already played - a later hide / destroy of the
//    same close must not replay it (cleared on the next real show);
//  - GenieCloseBypass: our own reposted close, let it through untouched.
// -------------------------------------------------------------------------
static bool MacGenieIsCloaked(HWND hWnd) {
    UINT cloaked = 0;
    return SUCCEEDED(DwmGetWindowAttribute(hWnd, DWMWA_CLOAKED, &cloaked, sizeof(cloaked))) &&
           cloaked;
}

static void MacGenieClearCloseHandled(HWND hWnd) {
    if (GetPropW(hWnd, L"GenieCloseHandled")) RemovePropW(hWnd, L"GenieCloseHandled");
}

// Cloak the real window behind the close ghost. Under the worker lock so an unload
// can't slip between the check and the cloak (the unload path uncloaks).
static bool MacGenieMarkCloseAndCloak(HWND hWnd) {
    std::lock_guard<std::mutex> lock(g_WorkerThreadsMutex);
    if (g_unloading.load(std::memory_order_relaxed)) return false;
    SetPropW(hWnd, L"GenieCloseHandled", (HANDLE)1);
    MacGenieSetCloak(hWnd, TRUE);
    return true;
}

// Wait (bounded) for the close genie of hWnd to finish. Runs on the app's UI
// thread, which is holding the app's DestroyWindow / hide call back meanwhile. The
// render thread never sends this thread a message, so this can't deadlock; and
// while waiting, nonqueued SENT messages are still handled (other threads that
// SendMessage to this thread's windows - the app's workers, the taskbar,
// accessibility tools, broadcasts - aren't stuck behind the genie), while posted
// input stays queued. Same wait as ReDrag's Windows Animations. A sent message
// may destroy the window meanwhile; the callers check IsWindow afterwards.
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

// Does the process have another visible app window besides hWnd? Then closing
// hWnd doesn't end the process, so DestroyWindow needn't be held for the whole
// genie (the ghost is a snapshot on its own thread) - and holding it would freeze
// the app's other windows on the same UI thread (Chromium, Edge...).
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

// Start a close genie for hWnd. On failure everything is undone and false returned.
static bool MacGenieStartClose(HWND hWnd, UINT closeMsg) {
    MacGenieSetDwmTransitions(hWnd, FALSE);
    SetPropW(hWnd, L"GenieCloseAnimating", (HANDLE)1);
    if (StartMacGenieAnim(hWnd, FALSE, GetWindowLongPtrW(hWnd, GWL_EXSTYLE),
                          FALSE, FALSE, FALSE, NULL, TRUE, closeMsg)) {
        if (MacGenieMarkCloseAndCloak(hWnd)) return true;
        // Unloading: the genie still plays, the window just isn't hidden behind it.
        RemovePropW(hWnd, L"GenieCloseAnimating");
        return false;
    }
    RemovePropW(hWnd, L"GenieCloseAnimating");
    MacGenieSetDwmTransitions(hWnd, TRUE);
    return false;
}

// Popup / overlay / shell-surface classes that are never an app's main window.
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

// Inside explorer.exe only File Explorer windows count as app windows: the shell's
// own full-monitor surfaces (around lock / power transitions, the desktop, flyouts)
// look like large unowned windows and would otherwise play a whole-screen genie and
// hold a shell thread for it. Same rule as ReDrag's Windows Animations.
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

// A window being destroyed by DestroyWindow on its own thread that is an app's main
// window: visible, on this desktop, a root window with no owner, not a tool window
// or popup / shell surface, at least 300x300 (the Windows Animations rule - smaller
// unowned windows are mostly splash screens, toasts and overlays).
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
    // (The popup list includes Explorer's small taskbar tab proxy for browser
    // windows, which would otherwise play a second, thin genie.)
    WCHAR cls[128];
    if (!GetClassNameW(hWnd, cls, ARRAYSIZE(cls)) || MacGenieIsPopupClass(cls) ||
        MacGenieIsBlockedShellWindow(cls)) {
        return false;
    }
    // Cloaked = not on screen here (another virtual desktop, or already hidden).
    if (MacGenieIsCloaked(hWnd)) return false;
    RECT r;
    return GetWindowRect(hWnd, &r) && r.right - r.left >= 300 && r.bottom - r.top >= 300;
}

// Classes whose close is caught at WM_CLOSE / SC_CLOSE in DefWindowProcW (they
// don't reach DestroyWindow on their own thread with their content intact).
static bool MacGenieIsCloseMessageClass(HWND hWnd) {
    WCHAR cls[128];
    if (!hWnd || !GetClassNameW(hWnd, cls, ARRAYSIZE(cls))) return false;
    return _wcsicmp(cls, L"ConsoleWindowClass") == 0 ||
           _wcsicmp(cls, L"CASCADIA_HOSTING_WINDOW_CLASS") == 0 ||
           _wcsicmp(cls, L"TaskManagerWindow") == 0 ||
           _wcsicmp(cls, L"WinUIDesktopWin32WindowClass") == 0;
}

// "Hide to the tray" treated as a close: a visible, real app main window of this
// process (>= 300x300), not a popup / shell / close-message class.
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

// Synchronous hide (ShowWindow / SetWindowPlacement / SetWindowPos): play the
// genie first, then the caller performs the hide and MacGenieFinishNativeHide
// uncloaks the (now hidden) window.
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
    // GenieCloseHandled stays until the next real show: some apps hide first and
    // destroy later, and that destroy must not replay the close.
    MacGenieSetDwmTransitions(hWnd, TRUE);
}

BOOL WINAPI DestroyWindow_Hook(HWND hWnd) {
    // Chromium, Firefox and Electron windows destroy themselves directly after
    // their own close handling, without WM_CLOSE reaching DefWindowProcW. Animate
    // at this confirmed-destruction point, then let the destroy through. The
    // destroy is held for the whole genie only for the process's last window (so
    // the process doesn't exit under the ghost); otherwise only for its first frame.
    bool animated = false;
    if (MacGenieIsCloseTarget(hWnd) && !MacGenieCloseBlocked(hWnd)) {
        animated = MacGenieStartClose(hWnd, WM_DESTROY);
        if (animated && !MacGenieHasOtherAppWindow(hWnd)) MacGenieWaitCloseAnim(hWnd, 4000);
    }
    BOOL result = DestroyWindow_Original(hWnd);
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
            // Cloak (frame-level hide, no layered-surface rebuild flash) - MINE.
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
        MacGenieClearCloseHandled(hWnd);   // shown again: a later close animates
    }

    // Close animation: WM_CLOSE / SC_CLOSE for the close-message classes. The close
    // is swallowed while the genie plays and reposted (with a bypass marker) by the
    // render thread's teardown.
    bool closeMessage = Msg == WM_CLOSE ||
                        (Msg == WM_SYSCOMMAND && (wParam & 0xFFF0) == SC_CLOSE);
    if (closeMessage && GetPropW(hWnd, L"GenieCloseBypass")) {
        // SC_CLOSE's default processing generates WM_CLOSE synchronously: carry the
        // bypass through that handoff and consume it at WM_CLOSE.
        if (Msg == WM_CLOSE) RemovePropW(hWnd, L"GenieCloseBypass");
        return DefWindowProcW_Original(hWnd, Msg, wParam, lParam);
    }
    if (closeMessage && IsWindowVisible(hWnd) && GetPropW(hWnd, L"GenieCloseAnimating")) {
        return 0;   // a second close click while the close genie is playing
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
        std::lock_guard<std::mutex> lock(g_CacheMutex);
        auto it = g_SnapshotCache.find(hWnd);
        if (it != g_SnapshotCache.end()) {
            DeleteObject(it->second.hBmp);
            g_SnapshotCache.erase(it);
        }
        g_IconPositions.erase(hWnd);
        g_LaunchSeen.erase(hWnd);
    }

    if (Msg == WM_SYSCOMMAND) {
        UINT cmd = wParam & 0xFFF0;
        if (cmd == SC_MINIMIZE) {
            // Our own deferred minimize (auto-hide path) re-enters here via
            // SendMessageTimeoutW - let it pass straight through, no recursion.
            if (GetPropW(hWnd, L"GenieBypass")) {
                return DefWindowProcW_Original(hWnd, Msg, wParam, lParam);
            }
            if (MacGenieShouldAnimate(hWnd)) {
                // Auto-hide taskbar? Reveal it, defer the real minimize until the
                // animation finishes so it doesn't slide away mid-genie.
                // (Potassiumuncher's engine, reconciled with MINE's cloak hide.)
                BOOL requestedUnhide = FALSE;
                HWND hNext = NULL;
                if (g_unhideEnabled.load(std::memory_order_relaxed)) {
                    HWND hTray = FindTaskbarForMonitor(MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST));
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
                    // Deferred path: animate, then (in the worker) cloak-hide is
                    // applied by us here after the first frame, and the worker does
                    // the real minimize + focus juggle at the end.
                    bool ok = StartMacGenieAnim(hWnd, FALSE, GetWindowLongPtrW(hWnd, GWL_EXSTYLE),
                                                FALSE, TRUE, TRUE, hNext);
                    if (ok) {
                        // First frame is up (StartMacGenieAnim waited): hide the real
                        // window with a flash-free cloak; worker minimizes it later.
                        MacGenieSetCloak(hWnd, TRUE);
                        return 0;
                    }
                    // Spawn failed: fall back to a normal immediate minimize.
                    return DefWindowProcW_Original(hWnd, Msg, wParam, lParam);
                }
                // Normal minimize (MINE): first-frame gate, then real minimize.
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

// Custom-title-bar apps (Zed/GPUI, some Electron) minimize via ShowWindowAsync,
// SetWindowPlacement, or CloseWindow. Shared minimize kick-off for those paths (no
// unhide handling - that lives on the SC_MINIMIZE path only).
static void MacGenieTryMinimizeAnim(HWND hWnd) {
    if (!IsWindowVisible(hWnd) || IsIconic(hWnd)) return;
    if (!MacGenieShouldAnimate(hWnd)) return;
    MacGenieSetDwmTransitions(hWnd, FALSE);
    StartMacGenieAnim(hWnd, FALSE, GetWindowLongPtrW(hWnd, GWL_EXSTYLE));
}

BOOL WINAPI ShowWindowAsync_Hook(HWND hWnd, int nCmdShow) {
    if (nCmdShow == SW_HIDE) {
        // Async hide treated as a close: the hook can't hold the caller for the
        // whole genie, so the hide is performed by the render thread's teardown.
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

// Store / UWP apps show their first window via SetWindowPos + SWP_SHOWWINDOW.
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

// -------------------------------------------------------------------------
// App-launch animation (experimental)
// -------------------------------------------------------------------------
DWORD WINAPI MacGenieLaunchThread(LPVOID lpParam) {
    MacGenieLaunchData* ld = (MacGenieLaunchData*)lpParam;
    HWND hWnd = ld->hWnd;
    LONG_PTR originalExStyle = ld->originalExStyle;
    BOOL hiddenByCloak = ld->hiddenByCloak;
    delete ld;

    // Let the freshly-shown (still hidden) window paint; UWP apps stay DWM-cloaked
    // by the shell behind their splash for a while - wait (bounded) for that cloak
    // to lift. When we cloaked the window ourselves, our cloak reports as
    // DWM_CLOAKED_APP, so that bit is masked out here.
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

    // Rising genie; reveals the real window at teardown the same way it was hidden.
    StartMacGenieAnim(hWnd, TRUE, originalExStyle, hiddenByCloak);
    return 0;
}

BOOL Wh_ModInit() {
    MacGenieLoadSettings();

    // Direct2D factory for the genie renderer (Potassiumuncher's engine). Released
    // in Wh_ModUninit. If it fails, workers skip the animation gracefully.
    HRESULT hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED,
                                   __uuidof(ID2D1Factory),
                                   reinterpret_cast<void**>(&g_d2dFactory));
    if (FAILED(hr)) g_d2dFactory = nullptr;

    Wh_SetFunctionHook((void*)DefWindowProcW, (void*)DefWindowProcW_Hook, (void**)&DefWindowProcW_Original);
    Wh_SetFunctionHook((void*)DestroyWindow, (void*)DestroyWindow_Hook, (void**)&DestroyWindow_Original);
    Wh_SetFunctionHook((void*)ShowWindow, (void*)ShowWindow_Hook, (void**)&ShowWindow_Original);
    Wh_SetFunctionHook((void*)ShowWindowAsync, (void*)ShowWindowAsync_Hook, (void**)&ShowWindowAsync_Original);
    Wh_SetFunctionHook((void*)SetWindowPlacement, (void*)SetWindowPlacement_Hook, (void**)&SetWindowPlacement_Original);
    Wh_SetFunctionHook((void*)CloseWindow, (void*)CloseWindow_Hook, (void**)&CloseWindow_Original);
    Wh_SetFunctionHook((void*)SetWindowPos, (void*)SetWindowPos_Hook, (void**)&SetWindowPos_Original);
    return TRUE;
}

void Wh_ModSettingsChanged() {
    MacGenieLoadSettings();
}

// Signal workers to wind down (the animation loop checks the flag every frame, the
// launch worker every 50 ms) and stop new ones from starting. They're joined in
// Wh_ModUninit, once the hooks are gone (see UNLOAD COORDINATION).
void Wh_ModBeforeUninit() {
    std::lock_guard<std::mutex> lock(g_WorkerThreadsMutex);
    g_unloading.store(true, std::memory_order_relaxed);
}

void Wh_ModUninit() {
    {
        // Normally already set by Wh_ModBeforeUninit; cheap insurance.
        std::lock_guard<std::mutex> lock(g_WorkerThreadsMutex);
        g_unloading.store(true, std::memory_order_relaxed);
    }
    // Join every worker before releasing anything they use (D2D factory, caches).
    // Not under g_CacheMutex: workers take it during their teardown.
    std::vector<HANDLE> threads;
    {
        std::lock_guard<std::mutex> lock(g_WorkerThreadsMutex);
        threads.swap(g_WorkerThreads);
    }
    for (HANDLE h : threads) {
        WaitForSingleObject(h, INFINITE);
        CloseHandle(h);
    }

    if (g_d2dFactory) {
        g_d2dFactory->Release();
        g_d2dFactory = nullptr;
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
