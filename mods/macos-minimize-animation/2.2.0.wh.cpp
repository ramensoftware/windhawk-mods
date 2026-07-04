// ==WindhawkMod==
// @id              macos-minimize-animation
// @name            MacOS Minimize Animation
// @description     Smooth macOS-style genie minimize and restore (open) animations for every window.
// @version         2.2.0
// @author          Abdullah Masood
// @github          https://github.com/Abdullah-Masood-05
// @include         *
// @compilerOptions -ldwmapi -lgdi32
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# MacOS Minimize Animation

![Demo](https://raw.githubusercontent.com/Abdullah-Masood-05/my-windhawk-mods-media/main/macos-minimize-animation.gif)

Brings the classic macOS **genie** effect to Windows. When you minimize a window
it warps and flows down into the taskbar; when you restore it, it flows back out.

Definitely not inspired from MacOS.

## See it in action
- Compile the mod with the button on the left or with Ctrl+B.
- Enable the mod and open any normal window (Explorer, a browser, Notepad...).
- Minimize it with the title bar `[-]` button or `Win`+`Down` and watch it warp
  and pour into the taskbar.
- Restore it from the taskbar and watch the genie play in reverse.
- Tweak the **Animation duration** in the settings to taste.

## Features
- **Real genie warp.** The window is sliced into horizontal strips that curve and
  funnel toward the taskbar (the bottom flows in first), instead of just shrinking.
- **Actually smooth.** Progress is driven by real elapsed time and every rendered
  frame is gated on the DWM compose cycle (`DwmFlush`), so each frame you render is
  exactly one frame you see - perfectly aligned with vsync at any duration you set.
- **Smoothstep easing** instead of a linear ramp, so it eases in and out.
- **Clean edges and fade** via a per-pixel-alpha layered surface.
- **Smart targeting.** The mod learns where each app's taskbar icon is (from where
  your cursor was when you minimized) and aims the genie at that spot next time.

## How it works
The mod hooks `ShowWindow`, `ShowWindowAsync`, `SetWindowPlacement`, `CloseWindow`,
`SetWindowPos` and `DefWindowProcW` to catch minimize / restore / first-show
requests - including apps with custom title bars (e.g. Zed) and Store/UWP apps,
whose minimize buttons bypass the classic paths. It snapshots the window, then on
a dedicated high-priority thread it
draws a transparent layered "ghost" window that warps the snapshot frame by frame
into the taskbar, leaving the real window to minimize behind it without the
system's own animation getting in the way.

## Settings
- **Animation duration** - how long the effect lasts (50-2000 ms).
- **Animate window restore (open)** - also play the reverse genie when restoring.
- **Animate app launch (experimental)** - also play the genie when an application
  window first opens. **Experimental and off by default**: it may briefly flash the
  window before animating, may not catch borderless apps, and can occasionally fire
  on splash / dialog windows. Leave it off and the mod behaves exactly like plain
  minimize / restore.
- **Multi-monitor support (experimental)** - play the genie on the monitor the
  window is on, targeting that monitor's taskbar edge, instead of always the
  primary monitor's. **Experimental and off by default.**

## Notes
- Works on all top-level windows; child / tiny / hidden windows are skipped.
- DWM transitions are temporarily disabled on the animated window and restored
  afterwards, so the system's own minimize/restore animation doesn't fight ours.
- By default the genie targets the primary monitor's taskbar edge; enable
  **Multi-monitor support** to play it on the monitor the window is on.

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
- launch_animation: false
  $name: Animate app launch (experimental)
  $description: >-
    Play a genie when an application window first opens. Experimental: may briefly
    flash the window before animating, may not catch borderless apps, and can
    occasionally fire on splash/dialog windows.
- multi_monitor: false
  $name: Multi-monitor support (experimental)
  $description: >-
    Play the genie on the monitor the window is on and target that monitor's
    taskbar edge, instead of always the primary monitor's.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <dwmapi.h>
#include <math.h>
#include <atomic>
#include <unordered_map>
#include <unordered_set>
#include <mutex>

typedef LRESULT (WINAPI *DefWindowProcW_t)(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
DefWindowProcW_t DefWindowProcW_Original;

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

struct MacGenieAnimData {
    HWND hRealWnd;
    HBITMAP hBitmap;      // snapshot of the window (a DDB at original size)
    RECT targetRect;      // the window's on-screen rect when the anim started
    RECT monRect;         // monitor the genie plays on, in virtual-screen coords
                          // (the window's monitor when multi-monitor support is
                          // enabled, else the primary)
    int width;
    int height;
    int targetDockX;      // dynamically learned taskbar icon X position
    BOOL isRising;        // FALSE = minimize (flow in), TRUE = restore (flow out)
    LONG_PTR originalExStyle;
    BOOL hiddenByCloak;   // rising only: TRUE = hidden via DWM cloak (restore path),
                          // FALSE = via WS_EX_LAYERED + alpha 0 (launch path)
    HANDLE hFirstFrameShown; // falling only: event signaled once the ghost's first
                             // frame is composed on screen (thread owns this handle);
                             // NULL when unused (rising)
};

// Handed to MacGenieLaunchThread: the launch worker needs the window's TRUE
// pre-hide extended style (captured at the hook site, before WS_EX_LAYERED was
// added) so the layered bit can be removed again after the reveal.
struct MacGenieLaunchData {
    HWND     hWnd;
    LONG_PTR originalExStyle;
};

// --- THE VAULTS ---
std::unordered_map<HWND, HBITMAP> g_SnapshotCache;
std::unordered_map<HWND, int> g_IconPositions; // Remembers where icons live
std::unordered_set<HWND> g_LaunchSeen;         // windows we've already shown/animated once
std::unordered_set<HWND> g_AnimActive;         // windows with a genie currently in flight
std::mutex g_CacheMutex;

// --- SETTINGS ---
std::atomic<int> g_durationMs{450};
std::atomic<bool> g_openAnimation{true};
std::atomic<bool> g_launchAnimation{false};
std::atomic<bool> g_multiMonitor{false};

// --- UNLOAD COORDINATION ---
// Windhawk unmaps the mod DLL right after uninit, so any worker thread still
// running mod code at that point would crash its host process. Workers register
// here and abort promptly once g_unloading is set; Wh_ModBeforeUninit waits for
// the count to drain before the DLL goes away.
std::atomic<bool> g_unloading{false};
std::atomic<int>  g_workerCount{0};

void MacGenieLoadSettings() {
    int ms = Wh_GetIntSetting(L"duration_ms");
    if (ms < 50) ms = 50;
    if (ms > 2000) ms = 2000;
    g_durationMs.store(ms, std::memory_order_relaxed);
    g_openAnimation.store(Wh_GetIntSetting(L"open_animation") != 0, std::memory_order_relaxed);
    g_launchAnimation.store(Wh_GetIntSetting(L"launch_animation") != 0, std::memory_order_relaxed);
    g_multiMonitor.store(Wh_GetIntSetting(L"multi_monitor") != 0, std::memory_order_relaxed);
}

void MacGenieSetDwmTransitions(HWND hWnd, BOOL enable) {
    BOOL disable = !enable;
    DwmSetWindowAttribute(hWnd, DWMWA_TRANSITIONS_FORCEDISABLED, &disable, sizeof(disable));
}

// Hide / show a window at the DWM level. A cloaked window is simply not rendered
// at all - frame included - while staying "visible" to Win32 and painting normally
// underneath, so a restore can happen invisibly under the genie. The previous
// approach (WS_EX_LAYERED + alpha 0) required DWM to rebuild the window's
// redirection surface each time the style was freshly added, and until that rebuild
// landed the bare window frame was composed for a few frames - a visible flash on
// every restore. Cloaking has no such lag and leaves no state on the window.
void MacGenieSetCloak(HWND hWnd, BOOL cloak) {
    DwmSetWindowAttribute(hWnd, DWMWA_CLOAK, &cloak, sizeof(cloak));
}

// Undo a rising caller's pre-animation hide (used whenever the animation can't run
// or is refused): uncloak or un-hide depending on how the window was hidden, and
// re-enable its DWM transitions. Without this the window would stay invisible.
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
// only fires on real top-level windows (and we never disable DWM transitions on
// something we won't actually animate).
static bool MacGenieShouldAnimate(HWND hWnd) {
    if (!hWnd) return false;
    LONG_PTR style = GetWindowLongPtrW(hWnd, GWL_STYLE);
    if (style & WS_CHILD) return false;

    // When the window is minimized its on-screen rect is the tiny, off-screen
    // icon rect, so measure the *restored* size from the window placement instead.
    // Without this the restore animation was wrongly skipped and the window came
    // back with the default system animation.
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

// Is this a window we should treat as an app "launch"? A real, top-level window
// with a title bar (skips child windows, tool windows, and borderless popups /
// menus / tooltips / splash screens, which would otherwise animate spuriously).
static bool MacGenieIsLaunchWindow(HWND hWnd) {
    if (!hWnd) return false;
    if (GetAncestor(hWnd, GA_ROOT) != hWnd) return false;
    LONG_PTR style = GetWindowLongPtrW(hWnd, GWL_STYLE);
    if (!(style & WS_CAPTION)) return false;
    LONG_PTR exStyle = GetWindowLongPtrW(hWnd, GWL_EXSTYLE);
    if (exStyle & WS_EX_TOOLWINDOW) return false;
    return MacGenieShouldAnimate(hWnd);  // top-level + reasonable size
}

// -------------------------------------------------------------------------
// Genie Animation Thread
// -------------------------------------------------------------------------
DWORD WINAPI MacGenieAnimThread(LPVOID lpParam) {
    MacGenieAnimData* data = (MacGenieAnimData*)lpParam;

    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST);

    const int W = data->width;
    const int H = data->height;
    // The monitor the genie plays on, in virtual-screen coordinates (left/top can
    // be negative on secondary monitors). With multi-monitor support disabled the
    // caller fills this with the primary screen rect, which reproduces the old
    // SM_CXSCREEN/SM_CYSCREEN math exactly.
    const RECT mon = data->monRect;

    const int origLeft = data->targetRect.left;
    const int origTop  = data->targetRect.top;
    const float origCenterX = (float)origLeft + W * 0.5f;

    // Where the genie funnels to: the learned taskbar icon X, at the very bottom
    // of the monitor the genie plays on.
    int dockX = data->targetDockX;
    if (dockX < mon.left) dockX = mon.left;
    if (dockX > mon.right) dockX = mon.right;
    const float dockXf = (float)dockX;
    const float dockY  = (float)mon.bottom;      // flow into the taskbar edge
    float neckW = W * 0.03f;
    if (neckW < 12.0f) neckW = 12.0f;
    if (neckW > 60.0f) neckW = 60.0f;

    // Bounding box the funnel can occupy. Content never rises above the window
    // top and reaches the dock at the bottom; horizontally it spans the union of
    // the window and the dock target (with a margin for wide intermediate strips).
    int boundLeft   = (origLeft < dockX ? origLeft : dockX) - W / 2;
    int boundRight  = ((origLeft + W) > dockX ? (origLeft + W) : dockX) + W / 2;
    int boundTop    = origTop;
    int boundBottom = mon.bottom;
    if (boundLeft < mon.left) boundLeft = mon.left;
    if (boundRight > mon.right) boundRight = mon.right;
    if (boundTop < mon.top) boundTop = mon.top;
    int boundW = boundRight - boundLeft;
    int boundH = boundBottom - boundTop;
    if (boundW < 1) boundW = 1;
    if (boundH < 1) boundH = 1;

    // The layered "ghost" window we animate on. Created hidden so it isn't briefly
    // shown as an uninitialized layered window; the first UpdateLayeredWindow
    // configures it, then we make it visible.
    HWND hGhost = CreateWindowExW(
        WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_TOPMOST | WS_EX_TRANSPARENT,
        L"STATIC", NULL, WS_POPUP,
        boundLeft, boundTop, boundW, boundH,
        NULL, NULL, NULL, NULL);

    HDC hScreenDC = GetDC(NULL);

    // Source pixels as a readable 32-bit top-down DIB. We copy the captured
    // snapshot (a DDB) into it once, then sample it directly per scanline below.
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
    GdiFlush();                          // ensure srcBits is populated before we read it
    const int srcStride = W * 4;

    // Canvas is a 32-bit top-down DIB so we get genuine per-pixel alpha: opaque
    // where the funnel is, transparent everywhere else. We composite into its bits
    // directly (no GDI StretchBlt), one output scanline at a time.
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

    // The window is warped per output scanline (not in coarse strips), so every
    // row gets its own width and horizontal offset and the funnel's edges come out
    // smooth instead of stair-stepped. SPREAD makes lower rows lead the morph,
    // which gives the genie its bottom-first "neck".
    const float SPREAD = 0.65f;
    const size_t canvasBytes = (size_t)boundW * 4 * boundH;

    const double totalMs = (double)g_durationMs.load(std::memory_order_relaxed);

    // Per-position eased morph at vertical fraction v (0 = window top, 1 = bottom).
    // smoothstep(m) eases in/out; the SPREAD term makes lower rows lead.
    auto morphAt = [&](float v, float tt) -> float {
        float m = tt * (1.0f + SPREAD) - (1.0f - v) * SPREAD;
        if (m < 0.0f) m = 0.0f;
        if (m > 1.0f) m = 1.0f;
        return m * m * (3.0f - 2.0f * m);
    };

    // Screen Y of each source row's top edge for the current morph time. Refilled
    // every frame; monotonic in k so we can invert it with a moving cursor.
    float* yb = new float[H + 1];

    // Time-based progress synced to the DWM compose cycle via DwmFlush. Driving by
    // real elapsed time and gating each frame on the next compose cycle means
    // every frame we render is exactly one frame the user sees, aligned with vsync.
    // If a frame runs long we simply skip ahead in time rather than slowing down.
    LARGE_INTEGER qpcFreq, qpcStart, qpcNow;
    QueryPerformanceFrequency(&qpcFreq);
    QueryPerformanceCounter(&qpcStart);

    BOOL firstFrame = TRUE;
    for (;;) {
        QueryPerformanceCounter(&qpcNow);
        double elapsedMs = (qpcNow.QuadPart - qpcStart.QuadPart) * 1000.0 / qpcFreq.QuadPart;
        BOOL lastFrame = (elapsedMs >= totalMs);
        float progress = lastFrame ? 1.0f : (float)(elapsedMs / totalMs);

        // tt: morph time. 0 = full window at its place, 1 = collapsed at the dock.
        // Minimize runs 0->1, restore runs 1->0 so the same warp plays in reverse.
        float tt = data->isRising ? (1.0f - progress) : progress;

        // Gentle fade so the window dissolves cleanly into / out of the dock.
        float fade = 1.0f;
        if (tt > 0.8f) fade = (1.0f - tt) / 0.2f;
        if (fade < 0.0f) fade = 0.0f;
        if (fade > 1.0f) fade = 1.0f;

        // Clear the canvas to fully transparent, then warp the snapshot into it
        // one output scanline at a time.
        memset(pBits, 0, canvasBytes);

        // Vertical map for this frame: where each source row lands on screen.
        for (int k = 0; k <= H; ++k) {
            float v = (float)k / (float)H;
            float e = morphAt(v, tt);
            float idY = (float)origTop + (float)H * v;
            yb[k] = idY + (dockY - idY) * e;
        }

        int kSeg = 0;
        for (int yC = 0; yC < boundH; ++yC) {
            float screenY = (float)(yC + boundTop) + 0.5f;
            if (screenY < yb[0] || screenY >= yb[H]) continue;

            // Invert the vertical map: which source row does this output row show?
            while (kSeg < H - 1 && yb[kSeg + 1] <= screenY) kSeg++;
            float segH = yb[kSeg + 1] - yb[kSeg];
            float frac = segH > 1e-4f ? (screenY - yb[kSeg]) / segH : 0.0f;
            float v = ((float)kSeg + frac) / (float)H;

            // This row's width and horizontal position from the eased morph. Because
            // it's recomputed for every output row, the left/right edges move at most
            // ~1px per row - so the silhouette is a smooth curve, not a staircase.
            float em = morphAt(v, tt);
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
                float u = ((float)xC + 0.5f - leftCanvas) * invW;  // 0..1 across the row
                if (u < 0.0f || u >= 1.0f) continue;
                int srcX = (int)(u * (float)W);
                if (srcX < 0) srcX = 0;
                if (srcX > W - 1) srcX = W - 1;
                const BYTE* sp = srcRowPtr + (size_t)srcX * 4;
                BYTE* dp = dstRowPtr + (size_t)xC * 4;
                dp[0] = sp[0];
                dp[1] = sp[1];
                dp[2] = sp[2];
                dp[3] = 255;          // opaque; the constant alpha below applies the fade
            }
        }

        // Single atomic commit: per-pixel alpha funnel + global fade, in one
        // composition pass. Per-pixel alpha is 255 (so premultiplied RGB == RGB),
        // and SourceConstantAlpha scales the whole thing for the dissolve.
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

        // Mod is being unloaded: cut the animation short. The cleanup below still
        // reveals the real window (rising) and re-enables its DWM transitions.
        if (g_unloading.load(std::memory_order_relaxed)) break;

        // Block until the next DWM compose cycle - the vsync sync point.
        DwmFlush();

        if (firstFrame) {
            firstFrame = FALSE;
            // The first frame has now been composed on screen. For a minimize, the
            // hook is holding the REAL minimize back until this moment - otherwise
            // the window would vanish a few frames before the genie appears (a
            // visible gap, especially on slow or power-throttled CPUs).
            if (data->hFirstFrameShown) SetEvent(data->hFirstFrameShown);
        }
    }

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

    // Re-enable DWM transitions on the real window now that our animation is done.
    // Without this, transitions stay force-disabled permanently for any window the
    // mod has ever touched - so future system minimize/maximize/show animations
    // (or another tool's transitions) would never play.
    MacGenieSetDwmTransitions(data->hRealWnd, TRUE);

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
    // Unblock the waiting minimize hook in case we bailed out before the first
    // frame was ever composed, then release the event (this thread owns it).
    if (data->hFirstFrameShown) {
        SetEvent(data->hFirstFrameShown);
        CloseHandle(data->hFirstFrameShown);
    }

    {
        std::lock_guard<std::mutex> lock(g_CacheMutex);
        g_AnimActive.erase(data->hRealWnd);
    }
    delete data;
    g_workerCount.fetch_sub(1, std::memory_order_release);
    return 0;
}

// -------------------------------------------------------------------------
// Core Setup Engine & Smart Tracking Logic
// -------------------------------------------------------------------------
// `originalExStyle` is the window's TRUE extended style, captured by the caller
// BEFORE any WS_EX_LAYERED was added for the hide. Rising callers hide the window
// first, so re-reading GetWindowLongPtrW here would always see the layered bit and
// the cleanup that removes it could never fire - the style would leak permanently.
// Falling callers never touch layering and just pass the current style.
// `cloakHidden` says how a rising caller hid the window: TRUE = DWM cloak (restore),
// FALSE = WS_EX_LAYERED + alpha 0 (launch) - the reveal must match the hide.
void StartMacGenieAnim(HWND hWnd, BOOL rising, LONG_PTR originalExStyle, BOOL cloakHidden = FALSE) {
    RECT rect;
    GetWindowRect(hWnd, &rect);
    int w = rect.right - rect.left;
    int h = rect.bottom - rect.top;

    if (w <= 0 || h <= 0) {
        // Can't animate - but a rising caller has already hidden the window, so
        // never bail without undoing that.
        if (rising) MacGenieUndoRisingHide(hWnd, originalExStyle, cloakHidden);
        return;
    }

    // One genie per window at a time: with several minimize entry points hooked
    // (ShowWindow, ShowWindowAsync, SetWindowPlacement, WM_SYSCOMMAND, ...), a single
    // gesture could otherwise start two overlapping ghosts. Any window we animate
    // also counts as "seen", so the launch path won't fire on it later (e.g. restore).
    // New animations are also refused while the mod is unloading.
    bool blocked = g_unloading.load(std::memory_order_relaxed);
    if (!blocked) {
        std::lock_guard<std::mutex> lock(g_CacheMutex);
        blocked = !g_AnimActive.insert(hWnd).second;
        if (!blocked) g_LaunchSeen.insert(hWnd);
    }
    if (blocked) {
        if (rising) {
            // Rising callers hide the window BEFORE calling us and rely on the
            // animation thread to reveal it at the end. If we bail here - e.g. the
            // user restores a window whose minimize genie is still in flight - that
            // reveal never comes, so undo the hide now or the restored window stays
            // permanently invisible.
            MacGenieUndoRisingHide(hWnd, originalExStyle, cloakHidden);
        } else {
            // Falling: callers disabled DWM transitions before calling us. Re-enable
            // them only if no in-flight genie owns this window (a running worker
            // re-enables them itself when it finishes) - otherwise a minimize gesture
            // landing during unload would leave them force-disabled on the window.
            bool owned;
            {
                std::lock_guard<std::mutex> lock(g_CacheMutex);
                owned = g_AnimActive.count(hWnd) != 0;
            }
            if (!owned) MacGenieSetDwmTransitions(hWnd, TRUE);
        }
        return;
    }

    // The monitor the genie plays on. Multi-monitor support (experimental) uses
    // the monitor the window is on, so the genie flows into that monitor's
    // taskbar edge; otherwise (or if the monitor can't be queried) it's the
    // primary screen rect, which reproduces the old behavior exactly.
    bool multiMon = g_multiMonitor.load(std::memory_order_relaxed);
    MONITORINFO mi;
    mi.cbSize = sizeof(mi);
    if (!multiMon ||
        !GetMonitorInfoW(MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST), &mi)) {
        mi.rcMonitor.left   = 0;
        mi.rcMonitor.top    = 0;
        mi.rcMonitor.right  = GetSystemMetrics(SM_CXSCREEN);
        mi.rcMonitor.bottom = GetSystemMetrics(SM_CYSCREEN);
    }

    // --- SMART ICON TRACKING ---
    POINT pt;
    GetCursorPos(&pt);
    RECT workArea;
    if (multiMon) {
        // "Is the cursor over a taskbar?" = outside the work area of the monitor
        // the CURSOR is on. The primary-only SPI_GETWORKAREA test below would
        // match every cursor position on a secondary monitor, so a plain
        // title-bar minimize there would mislearn the cursor X as the icon spot.
        MONITORINFO cursorMi;
        cursorMi.cbSize = sizeof(cursorMi);
        if (GetMonitorInfoW(MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST), &cursorMi)) {
            workArea = cursorMi.rcWork;
        } else {
            SystemParametersInfoW(SPI_GETWORKAREA, 0, &workArea, 0);
        }
    } else {
        SystemParametersInfoW(SPI_GETWORKAREA, 0, &workArea, 0);
    }

    int learnedTargetX = (mi.rcMonitor.left + mi.rcMonitor.right) / 2; // Default to center

    // If the mouse is outside the desktop area (e.g. hovering over the taskbar)
    if (!PtInRect(&workArea, pt)) {
        learnedTargetX = pt.x; // Steal the mouse X coordinate!
        std::lock_guard<std::mutex> lock(g_CacheMutex);
        g_IconPositions[hWnd] = learnedTargetX; // Save it to the vault
    } else {
        // Mouse is on the desktop (clicking the [-] title bar button)
        // Check the vault to see if we remember where this app's icon is!
        std::lock_guard<std::mutex> lock(g_CacheMutex);
        if (g_IconPositions.count(hWnd)) {
            learnedTargetX = g_IconPositions[hWnd];
        }
    }

    MacGenieAnimData* data = new MacGenieAnimData();
    data->hRealWnd = hWnd;
    data->targetRect = rect;
    data->monRect = mi.rcMonitor;
    data->width = w;
    data->height = h;
    data->isRising = rising;
    data->targetDockX = learnedTargetX; // Assign the learned coordinate
    data->originalExStyle = originalExStyle;
    data->hiddenByCloak = cloakHidden;

    HDC hScreenDC = GetDC(NULL);
    HDC hMemDC = CreateCompatibleDC(hScreenDC);
    data->hBitmap = CreateCompatibleBitmap(hScreenDC, w, h);
    HBITMAP hOldBmp = (HBITMAP)SelectObject(hMemDC, data->hBitmap);

    if (rising) {
        BOOL fromCache = FALSE;
        {
            std::lock_guard<std::mutex> lock(g_CacheMutex);
            if (g_SnapshotCache.count(hWnd)) {
                HDC hCacheDC = CreateCompatibleDC(hScreenDC);
                HBITMAP hOldCacheBmp = (HBITMAP)SelectObject(hCacheDC, g_SnapshotCache[hWnd]);
                BitBlt(hMemDC, 0, 0, w, h, hCacheDC, 0, 0, SRCCOPY);
                SelectObject(hCacheDC, hOldCacheBmp);
                DeleteDC(hCacheDC);

                DeleteObject(g_SnapshotCache[hWnd]);
                g_SnapshotCache.erase(hWnd);
                fromCache = TRUE;
            }
        }
        if (!fromCache) {
            PrintWindow(hWnd, hMemDC, PW_CLIENTONLY | 0x00000002);
        }
    } else {
        BitBlt(hMemDC, 0, 0, w, h, hScreenDC, rect.left, rect.top, SRCCOPY);

        std::lock_guard<std::mutex> lock(g_CacheMutex);
        if (g_SnapshotCache.count(hWnd)) {
            DeleteObject(g_SnapshotCache[hWnd]);
        }
        g_SnapshotCache[hWnd] = CreateCompatibleBitmap(hScreenDC, w, h);
        HDC hCacheDC = CreateCompatibleDC(hScreenDC);
        HBITMAP hOldCacheBmp = (HBITMAP)SelectObject(hCacheDC, g_SnapshotCache[hWnd]);
        BitBlt(hCacheDC, 0, 0, w, h, hMemDC, 0, 0, SRCCOPY);
        SelectObject(hCacheDC, hOldCacheBmp);
        DeleteDC(hCacheDC);
    }

    SelectObject(hMemDC, hOldBmp);
    DeleteDC(hMemDC);
    ReleaseDC(NULL, hScreenDC);

    // For a minimize the caller performs the REAL minimize right after we return,
    // so hold it back until the ghost's first frame is composed - otherwise the
    // window vanishes a few frames before the genie appears (a visible gap,
    // especially on slow / power-throttled CPUs). Two handles to one event: the
    // animation thread owns its duplicate (signals + closes it), we own and wait on
    // the original, so neither side can ever touch a freed handle.
    HANDLE hFirstShown = NULL;
    data->hFirstFrameShown = NULL;
    if (!rising) {
        hFirstShown = CreateEventW(NULL, TRUE, FALSE, NULL);
        if (hFirstShown &&
            !DuplicateHandle(GetCurrentProcess(), hFirstShown, GetCurrentProcess(),
                             &data->hFirstFrameShown, 0, FALSE, DUPLICATE_SAME_ACCESS)) {
            data->hFirstFrameShown = NULL;   // no sync available - fall back to no wait
        }
    }

    // Decide before spawning: once the thread runs, `data` belongs to it and may
    // already be freed by the time we get here again.
    bool waitForFirstFrame = (data->hFirstFrameShown != NULL);

    g_workerCount.fetch_add(1, std::memory_order_relaxed);
    HANDLE hThread = CreateThread(NULL, 0, MacGenieAnimThread, data, 0, NULL);
    if (hThread) {
        CloseHandle(hThread);
        if (hFirstShown) {
            if (waitForFirstFrame) {
                WaitForSingleObject(hFirstShown, 200);   // capped: worst case = old behavior
            }
            CloseHandle(hFirstShown);
        }
    } else {
        g_workerCount.fetch_sub(1, std::memory_order_release);
        // Thread couldn't start - undo our state so we don't leave the window
        // invisible or with transitions permanently disabled.
        if (rising) {
            MacGenieUndoRisingHide(hWnd, data->originalExStyle, data->hiddenByCloak);
        } else {
            MacGenieSetDwmTransitions(hWnd, TRUE);
        }
        if (hFirstShown) CloseHandle(hFirstShown);
        if (data->hFirstFrameShown) CloseHandle(data->hFirstFrameShown);
        DeleteObject(data->hBitmap);
        delete data;
        std::lock_guard<std::mutex> lock(g_CacheMutex);
        g_AnimActive.erase(hWnd);
    }
}

// -------------------------------------------------------------------------
// Hooks
// -------------------------------------------------------------------------
DWORD WINAPI MacGenieLaunchThread(LPVOID lpParam);   // defined below

// Arm the launch animation for a window about to be shown for the first time: check
// eligibility, kill its DWM transition and hide it (alpha 0) *before* the real show
// call, so the system's own open animation never appears. On success writes the
// window's TRUE pre-hide extended style to *origExOut (needed to remove the layered
// bit again later) - the caller must perform its original show call, then
// MacGenieLaunchCommit(hWnd, origEx).
static bool MacGenieLaunchPrepare(HWND hWnd, int nCmdShow, LONG_PTR* origExOut) {
    if (g_unloading.load(std::memory_order_relaxed)) return false;   // no new workers past the drain
    if (!g_launchAnimation.load(std::memory_order_relaxed)) return false;
    if (nCmdShow != SW_SHOW && nCmdShow != SW_SHOWNORMAL &&
        nCmdShow != SW_SHOWDEFAULT && nCmdShow != SW_SHOWMAXIMIZED) return false;
    if (IsWindowVisible(hWnd) || IsIconic(hWnd)) return false;
    if (!MacGenieIsLaunchWindow(hWnd)) return false;
    {
        std::lock_guard<std::mutex> lock(g_CacheMutex);
        if (!g_LaunchSeen.insert(hWnd).second) return false;   // only the first show
    }
    MacGenieSetDwmTransitions(hWnd, FALSE);
    LONG_PTR exStyle = GetWindowLongPtrW(hWnd, GWL_EXSTYLE);
    *origExOut = exStyle;
    SetWindowLongPtrW(hWnd, GWL_EXSTYLE, exStyle | WS_EX_LAYERED);
    SetLayeredWindowAttributes(hWnd, 0, 0, LWA_ALPHA);
    return true;
}

// Second half of the launch sequence: spawn the reveal worker (or roll back so the
// window isn't left invisible, layered, or with transitions disabled).
static void MacGenieLaunchCommit(HWND hWnd, LONG_PTR originalExStyle) {
    MacGenieLaunchData* ld = new MacGenieLaunchData();
    ld->hWnd = hWnd;
    ld->originalExStyle = originalExStyle;
    g_workerCount.fetch_add(1, std::memory_order_relaxed);
    HANDLE h = CreateThread(NULL, 0, MacGenieLaunchThread, ld, 0, NULL);
    if (h) {
        CloseHandle(h);
    } else {
        g_workerCount.fetch_sub(1, std::memory_order_release);
        delete ld;
        SetLayeredWindowAttributes(hWnd, 0, 255, LWA_ALPHA);
        if (!(originalExStyle & WS_EX_LAYERED)) {
            SetWindowLongPtrW(hWnd, GWL_EXSTYLE, originalExStyle);
        }
        MacGenieSetDwmTransitions(hWnd, TRUE);
    }
}

BOOL WINAPI ShowWindow_Hook(HWND hWnd, int nCmdShow) {
    if (nCmdShow == SW_MINIMIZE || nCmdShow == SW_SHOWMINIMIZED || nCmdShow == SW_SHOWMINNOACTIVE) {
        if (MacGenieShouldAnimate(hWnd)) {
            MacGenieSetDwmTransitions(hWnd, FALSE);
            StartMacGenieAnim(hWnd, FALSE, GetWindowLongPtrW(hWnd, GWL_EXSTYLE));
        }
        return ShowWindow_Original(hWnd, nCmdShow);
    }

    // Restore from the taskbar / minimized state.
    if ((nCmdShow == SW_RESTORE || nCmdShow == SW_SHOWNORMAL) && IsIconic(hWnd)) {
        if (g_openAnimation.load(std::memory_order_relaxed) && MacGenieShouldAnimate(hWnd)) {
            MacGenieSetDwmTransitions(hWnd, FALSE);
            // Cloak instead of WS_EX_LAYERED + alpha 0: the restore happens fully
            // invisibly (frame included), with no layered-surface rebuild flash, and
            // the window's styles are never touched.
            MacGenieSetCloak(hWnd, TRUE);
            BOOL res = ShowWindow_Original(hWnd, nCmdShow);
            StartMacGenieAnim(hWnd, TRUE, GetWindowLongPtrW(hWnd, GWL_EXSTYLE), TRUE);
            return res;
        }
        return ShowWindow_Original(hWnd, nCmdShow);
    }

    // App launch: a brand-new top-level window shown for the first time. We hide it
    // and kill its DWM transition *before* the real ShowWindow, so the system's own
    // open animation never appears - then the genie plays it in. (Windows created
    // already-visible just open normally, which is better than double-animating.)
    LONG_PTR launchOrigEx;
    if (MacGenieLaunchPrepare(hWnd, nCmdShow, &launchOrigEx)) {
        BOOL res = ShowWindow_Original(hWnd, nCmdShow);
        MacGenieLaunchCommit(hWnd, launchOrigEx);
        return res;
    }

    return ShowWindow_Original(hWnd, nCmdShow);
}

LRESULT WINAPI DefWindowProcW_Hook(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) {
    if (Msg == WM_DESTROY) {
        std::lock_guard<std::mutex> lock(g_CacheMutex);
        if (g_SnapshotCache.count(hWnd)) {
            DeleteObject(g_SnapshotCache[hWnd]);
            g_SnapshotCache.erase(hWnd);
        }
        // Forget the icon position when the app closes
        if (g_IconPositions.count(hWnd)) {
            g_IconPositions.erase(hWnd);
        }
        g_LaunchSeen.erase(hWnd);
    }

    if (Msg == WM_SYSCOMMAND) {
        UINT cmd = wParam & 0xFFF0;
        if (cmd == SC_MINIMIZE) {
            if (MacGenieShouldAnimate(hWnd)) {
                MacGenieSetDwmTransitions(hWnd, FALSE);
                StartMacGenieAnim(hWnd, FALSE, GetWindowLongPtrW(hWnd, GWL_EXSTYLE));
            }
            return DefWindowProcW_Original(hWnd, Msg, wParam, lParam);
        }
        else if (cmd == SC_RESTORE && IsIconic(hWnd) &&
                 g_openAnimation.load(std::memory_order_relaxed) &&
                 MacGenieShouldAnimate(hWnd)) {
            MacGenieSetDwmTransitions(hWnd, FALSE);
            // Cloak instead of WS_EX_LAYERED + alpha 0 - see the ShowWindow hook.
            MacGenieSetCloak(hWnd, TRUE);
            LRESULT res = DefWindowProcW_Original(hWnd, Msg, wParam, lParam);
            StartMacGenieAnim(hWnd, TRUE, GetWindowLongPtrW(hWnd, GWL_EXSTYLE), TRUE);
            return res;
        }
    }
    return DefWindowProcW_Original(hWnd, Msg, wParam, lParam);
}

// Some apps with custom title bars (e.g. Zed / GPUI, some Electron-style apps) don't
// minimize through ShowWindow or WM_SYSCOMMAND at all - their own minimize button goes
// through ShowWindowAsync, SetWindowPlacement, or CloseWindow (which minimizes, despite
// its name). Shared guard + kick-off for those paths. No restore handling here: restore
// from the taskbar already animates via SC_RESTORE, and the async delivery of these
// APIs would race the restore's hide-then-reveal logic.
static void MacGenieTryMinimizeAnim(HWND hWnd) {
    // Skip hidden / already-minimized windows: apps restoring a saved session may set a
    // minimized placement on a still-hidden window at startup, and we'd snapshot
    // whatever desktop pixels happen to be under its rect.
    if (!IsWindowVisible(hWnd) || IsIconic(hWnd)) return;
    if (!MacGenieShouldAnimate(hWnd)) return;
    MacGenieSetDwmTransitions(hWnd, FALSE);
    StartMacGenieAnim(hWnd, FALSE, GetWindowLongPtrW(hWnd, GWL_EXSTYLE));
}

BOOL WINAPI ShowWindowAsync_Hook(HWND hWnd, int nCmdShow) {
    if (nCmdShow == SW_MINIMIZE || nCmdShow == SW_SHOWMINIMIZED ||
        nCmdShow == SW_SHOWMINNOACTIVE) {
        MacGenieTryMinimizeAnim(hWnd);
    }
    // Launch parity with ShowWindow: some apps show their first window async.
    else {
        LONG_PTR launchOrigEx;
        if (MacGenieLaunchPrepare(hWnd, nCmdShow, &launchOrigEx)) {
            BOOL res = ShowWindowAsync_Original(hWnd, nCmdShow);
            MacGenieLaunchCommit(hWnd, launchOrigEx);
            return res;
        }
    }
    return ShowWindowAsync_Original(hWnd, nCmdShow);
}

BOOL WINAPI SetWindowPlacement_Hook(HWND hWnd, const WINDOWPLACEMENT* lpwndpl) {
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

// Store / UWP-style apps (e.g. the modern Media Player) don't show their first window
// through ShowWindow at all - the frame comes up via SetWindowPos + SWP_SHOWWINDOW
// (and stays DWM-cloaked until the app has rendered; the launch worker waits that
// out). Catch that path for the launch animation too. MacGenieLaunchPrepare bails
// unless the window is hidden, top-level and launch-eligible, so the hot common case
// costs one flag test.
BOOL WINAPI SetWindowPos_Hook(HWND hWnd, HWND hWndInsertAfter, int X, int Y,
                              int cx, int cy, UINT uFlags) {
    if (uFlags & SWP_SHOWWINDOW) {
        LONG_PTR launchOrigEx;
        if (MacGenieLaunchPrepare(hWnd, SW_SHOW, &launchOrigEx)) {
            BOOL res = SetWindowPos_Original(hWnd, hWndInsertAfter, X, Y, cx, cy, uFlags);
            MacGenieLaunchCommit(hWnd, launchOrigEx);
            return res;
        }
    }
    return SetWindowPos_Original(hWnd, hWndInsertAfter, X, Y, cx, cy, uFlags);
}

// -------------------------------------------------------------------------
// App-launch animation (experimental)
// -------------------------------------------------------------------------

// Worker: the window has been shown invisibly (alpha 0); give it a moment to paint,
// then snapshot it and play the rising genie. Runs off the ShowWindow hook so the
// app's own thread isn't blocked while we wait.
DWORD WINAPI MacGenieLaunchThread(LPVOID lpParam) {
    MacGenieLaunchData* ld = (MacGenieLaunchData*)lpParam;
    HWND hWnd = ld->hWnd;
    LONG_PTR originalExStyle = ld->originalExStyle;
    delete ld;

    // Let the freshly-shown (still hidden) window paint its content first, otherwise
    // the genie would warp a blank frame. Store / UWP-style apps additionally stay
    // DWM-cloaked behind their splash screen for a while - wait (bounded) for the
    // cloak to lift, or PrintWindow would hand us a black rectangle.
    Sleep(60);
    for (int i = 0; i < 30; ++i) {
        if (!IsWindow(hWnd) || g_unloading.load(std::memory_order_relaxed)) break;
        UINT cloaked = 0;
        if (FAILED(DwmGetWindowAttribute(hWnd, DWMWA_CLOAKED, &cloaked, sizeof(cloaked))) ||
            !cloaked) {
            break;
        }
        Sleep(50);
    }

    if (g_unloading.load(std::memory_order_relaxed) ||
        !IsWindow(hWnd) || IsIconic(hWnd) || !IsWindowVisible(hWnd)) {
        // It went away / got minimized while we waited (or the mod is unloading) -
        // make sure we didn't leave it hidden, layered, or with transitions disabled.
        if (IsWindow(hWnd)) {
            SetLayeredWindowAttributes(hWnd, 0, 255, LWA_ALPHA);
            if (!(originalExStyle & WS_EX_LAYERED)) {
                SetWindowLongPtrW(hWnd, GWL_EXSTYLE, originalExStyle);
            }
            MacGenieSetDwmTransitions(hWnd, TRUE);
        }
        g_workerCount.fetch_sub(1, std::memory_order_release);
        return 0;
    }

    // If the one-genie guard blocks this (duplicate / unload race), it restores the
    // window's visibility and style itself, so no reveal is ever lost.
    StartMacGenieAnim(hWnd, TRUE, originalExStyle); // rising genie; reveals the real window
    g_workerCount.fetch_sub(1, std::memory_order_release);
    return 0;
}

BOOL Wh_ModInit() {
    MacGenieLoadSettings();
    Wh_SetFunctionHook((void*)DefWindowProcW, (void*)DefWindowProcW_Hook, (void**)&DefWindowProcW_Original);
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

// Windhawk unmaps the mod DLL right after Wh_ModUninit returns, so any worker
// thread still running mod code at that point would crash its host process (and
// with @include * that could be any process mid-animation). Signal workers to
// abort and wait for them to drain: the animation loop checks the flag every
// frame (~16 ms) and the launch worker every 50 ms, so this normally returns in
// well under 100 ms; the cap only guards against a hung DWM.
void Wh_ModBeforeUninit() {
    g_unloading.store(true, std::memory_order_relaxed);
    for (int i = 0; i < 300 && g_workerCount.load(std::memory_order_acquire) > 0; ++i) {
        Sleep(10);
    }
}

void Wh_ModUninit() {
    std::lock_guard<std::mutex> lock(g_CacheMutex);
    for (auto& pair : g_SnapshotCache) {
        DeleteObject(pair.second);
    }
    g_SnapshotCache.clear();
    g_IconPositions.clear();
    g_LaunchSeen.clear();
}
