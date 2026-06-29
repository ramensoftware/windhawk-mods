// ==WindhawkMod==
// @id              taskmgr-tab-slide-animation
// @name            Task Manager Tab Slide Animation
// @description     Smooth animated transition (crossfade or slide) when switching tabs (Processes, Performance, ...) in Task Manager.
// @version         0.4.1
// @author          Abdullah Masood
// @github          https://github.com/Abdullah-Masood-05
// @include         Taskmgr.exe
// @compilerOptions -ldwmapi -lgdi32 -lole32 -lmsimg32
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Task Manager Tab Slide Animation

**Crossfade:**

![Crossfade demo](https://raw.githubusercontent.com/Abdullah-Masood-05/my-windhawk-mods-media/main/taskmgr-tab-slide-animation-fade-v2.gif)

**Slide:**

![Slide demo](https://raw.githubusercontent.com/Abdullah-Masood-05/my-windhawk-mods-media/main/taskmgr-tab-slide-animation-slide-v2.gif)

The Windows 11 Task Manager switches between its tabs instantly. This mod adds a
smooth transition - both for the main left navigation (Processes, Performance,
App history, ...) and for the **Performance sub-pages** (CPU, Memory, Disk,
Network/Ethernet, Wi-Fi, GPU).

> Experimental. Task Manager is a WinUI 3 app, so there's no clean Win32
> "tab changed" signal - this works by heuristics and screen capture.

## Two animation styles (pick one in Settings)
- **Crossfade** *(default)* - the overlay crossfades from the old view to the new one.
- **Slide** - the old view slides out while the new view slides in (left/right, or
  up/down via a setting).

Both styles freeze the current view under a topmost, click-through overlay at
mouse-down, so Task Manager switches the tab *underneath* it with **no visual jump** -
the animation always starts from the old frame.

## How it works
- A low-level mouse hook (scoped to Task Manager) notices a click in a left-hand
  navigation area and, before the click switches the tab, captures the current view
  and shows it on a frozen overlay.
- **UI Automation** reads the clicked item's bounding rectangle - its right edge is
  where the content begins - so the animated region adapts automatically to the main
  nav (expanded or collapsed) *and* to the Performance sub-list.
- It captures the new view (via PrintWindow, so the overlay doesn't block it) and
  only animates if the content actually **changed** by more than a threshold, so
  clicking a process row, selecting, or live graph updates don't trigger a spurious
  animation.

## Settings
- **Animation type** - Crossfade or Slide.
- **Top inset** - height (px) of the top command bar to exclude.
- **Capture delay** - delay after the click before capturing the new view (ms).
- **Change threshold** - minimum % of the region that must change to animate.
- **Animation duration** and **slide vertically** (slide mode) to taste.

## Limitations
- Relies on UI Automation to locate the clicked item.
- The window should be visible/unobscured during the switch.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- animation_type: fade
  $name: Animation type
  $description: How the new tab appears.
  $options:
  - fade: Crossfade (no visual jump)
  - slide: Slide
- duration_ms: 250
  $name: Animation duration (ms)
  $description: How long the transition lasts. Clamped to 60-1500.
- top_inset: 48
  $name: Top inset (px)
  $description: Height of the top command bar to exclude from the animation.
- capture_delay_ms: 140
  $name: Capture delay (ms)
  $description: Delay after the click before capturing the new view. Raise if it grabs the old view.
- change_threshold: 2
  $name: Change threshold (%)
  $description: Minimum percent of the region that must change for the animation to play. Raise to reduce spurious animations.
- slide_vertical: false
  $name: Slide vertically (slide mode only)
  $description: Slide up/down instead of left/right. Only affects the Slide animation.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <dwmapi.h>
#include <uiautomation.h>
#include <atomic>
#include <mutex>
#include <wchar.h>

// Custom message posted from the worker thread to the hook thread asking it to
// destroy the overlay window (a window must be destroyed on its creating thread).
#define WM_FADE_DESTROY_WND (WM_APP + 1)

// Render the window's own DirectComposition/WinUI content (added in Win 8.1). Lets
// us capture the new view even though our topmost overlay covers it on screen.
#ifndef PW_RENDERFULLCONTENT
#define PW_RENDERFULLCONTENT 0x00000002
#endif

// CLSID_CUIAutomation / IID_IUIAutomation defined inline so we don't depend on the
// uuid import library being linked.
static const CLSID kCLSID_CUIAutomation =
    { 0xff48dba4, 0x60ef, 0x4201, { 0xaa, 0x87, 0x54, 0x10, 0x3e, 0xef, 0x59, 0x4e } };
static const IID kIID_IUIAutomation =
    { 0x30cbe57d, 0xd9d0, 0x452a, { 0xab, 0x13, 0x7a, 0xc5, 0xac, 0x48, 0x25, 0xee } };

// Animation styles.
enum { ANIM_FADE = 0, ANIM_SLIDE = 1 };

// -------------------------------------------------------------------------
// State
// -------------------------------------------------------------------------

// Data handed to the worker thread. The captured old frame lives on the overlay
// (g_overlay.frozenBmp), so the worker only needs the click point, overlay rect,
// the window (for PrintWindow capture) and which style to play.
struct TabAnimData {
    HWND  root;      // the Task Manager window (for PrintWindow capture)
    POINT clickPt;   // screen point of the nav click (for UI Automation)
    RECT  fullRect;  // screen rect the overlay covers
    RECT  winRect;   // window rect at mouse-down (to detect a move/resize mid-cycle)
    int   mode;      // ANIM_FADE or ANIM_SLIDE
};

// The single live overlay. Created on the hook thread at mouse-down, read by the
// worker during the animation, destroyed on the hook thread afterwards.
struct OverlayState {
    HWND    hwnd;       // topmost, click-through, layered window
    HBITMAP frozenBmp;  // the full old frame captured at mouse-down (overlay owns it)
    RECT    rect;       // screen rect of the overlay
    HWND    srcWnd;     // source window (to detect a move while the overlay is live)
    RECT    srcRect;    // source window rect when the overlay was created
};

std::atomic<int>  g_animType{ANIM_FADE};
std::atomic<int>  g_durationMs{250};
std::atomic<int>  g_topInset{48};
std::atomic<int>  g_captureDelay{140};
std::atomic<int>  g_changeThreshold{2};
std::atomic<bool> g_slideVertical{false};

std::atomic<bool> g_inProgress{false}; // an animation cycle (overlay live) is in flight

// Pairing state between mouse-down (create overlay) and mouse-up (start worker). The
// active style is captured into g_pendingMode at mouse-down so a mid-gesture settings
// change can't split a cycle across both code paths.
std::mutex g_mutex;
int        g_pendingMode  = ANIM_FADE;
HWND       g_pendingRoot  = NULL;
POINT      g_pendingClick = {0, 0};
RECT       g_pendingRect  = {0, 0, 0, 0};
RECT       g_pendingWin   = {0, 0, 0, 0};
bool       g_armed        = false;

// Overlay state, guarded by its own mutex (touched by hook + worker threads).
std::mutex   g_overlayMutex;
OverlayState g_overlay = { NULL, NULL, {0, 0, 0, 0}, NULL, {0, 0, 0, 0} };

// Mouse-hook thread.
HHOOK  g_mouseHook    = NULL;
HANDLE g_hookThread   = NULL;
DWORD  g_hookThreadId = 0;

void TmLoadSettings() {
    PCWSTR type = Wh_GetStringSetting(L"animation_type");
    int mode = ANIM_FADE;
    if (type) {
        if (wcscmp(type, L"slide") == 0) mode = ANIM_SLIDE;
        Wh_FreeStringSetting(type);
    }
    g_animType.store(mode, std::memory_order_relaxed);

    int ms = Wh_GetIntSetting(L"duration_ms");
    if (ms < 60)   ms = 60;
    if (ms > 1500) ms = 1500;
    g_durationMs.store(ms, std::memory_order_relaxed);

    int top = Wh_GetIntSetting(L"top_inset");
    if (top < 0) top = 0;
    g_topInset.store(top, std::memory_order_relaxed);

    int delay = Wh_GetIntSetting(L"capture_delay_ms");
    if (delay < 0)    delay = 0;
    if (delay > 1000) delay = 1000;
    g_captureDelay.store(delay, std::memory_order_relaxed);

    int thr = Wh_GetIntSetting(L"change_threshold");
    if (thr < 1)   thr = 1;
    if (thr > 100) thr = 100;
    g_changeThreshold.store(thr, std::memory_order_relaxed);

    g_slideVertical.store(Wh_GetIntSetting(L"slide_vertical") != 0, std::memory_order_relaxed);
}

// -------------------------------------------------------------------------
// Capture / geometry helpers
// -------------------------------------------------------------------------

// Grab a screen rectangle into a new DDB (caller owns / deletes it).
HBITMAP TmCaptureRect(const RECT& r) {
    int w = r.right - r.left;
    int h = r.bottom - r.top;
    if (w <= 0 || h <= 0) return NULL;

    HDC screen = GetDC(NULL);
    HDC mem = CreateCompatibleDC(screen);
    HBITMAP bmp = CreateCompatibleBitmap(screen, w, h);
    HBITMAP oldSel = (HBITMAP)SelectObject(mem, bmp);
    BitBlt(mem, 0, 0, w, h, screen, r.left, r.top, SRCCOPY);
    SelectObject(mem, oldSel);
    DeleteDC(mem);
    ReleaseDC(NULL, screen);
    return bmp;
}

// Capture a screen-space region of `root` by asking the window to render itself
// (PrintWindow + PW_RENDERFULLCONTENT). Unlike a screen BitBlt this is NOT blocked
// by our topmost overlay, so the worker can grab the freshly-switched view even
// while the overlay still covers that area on screen. Caller owns the result.
HBITMAP TmCaptureWindowRegion(HWND root, const RECT& screenRegion) {
    if (!root) return NULL;
    RECT wr;
    if (!GetWindowRect(root, &wr)) return NULL;
    int ww = wr.right - wr.left;
    int wh = wr.bottom - wr.top;
    int rw = screenRegion.right - screenRegion.left;
    int rh = screenRegion.bottom - screenRegion.top;
    if (ww <= 0 || wh <= 0 || rw <= 0 || rh <= 0) return NULL;

    HDC screen = GetDC(NULL);
    HDC winDC  = CreateCompatibleDC(screen);
    HBITMAP winBmp = CreateCompatibleBitmap(screen, ww, wh);
    HBITMAP oldWin = (HBITMAP)SelectObject(winDC, winBmp);

    // Render the whole window (incl. DirectComposition content) into winDC.
    BOOL ok = PrintWindow(root, winDC, PW_RENDERFULLCONTENT);

    HBITMAP dst = NULL;
    if (ok) {
        int rx = screenRegion.left - wr.left;   // region offset within the window
        int ry = screenRegion.top  - wr.top;
        HDC ddc = CreateCompatibleDC(screen);
        dst = CreateCompatibleBitmap(screen, rw, rh);
        HBITMAP oldDst = (HBITMAP)SelectObject(ddc, dst);
        BitBlt(ddc, 0, 0, rw, rh, winDC, rx, ry, SRCCOPY);
        SelectObject(ddc, oldDst);
        DeleteDC(ddc);
    }

    SelectObject(winDC, oldWin);
    DeleteObject(winBmp);
    DeleteDC(winDC);
    ReleaseDC(NULL, screen);
    return dst;
}

// True if a bitmap is essentially blank (almost entirely near-black) - the typical
// signature of PrintWindow failing on hardware-composited content. Used to fall
// back to a plain screen grab so the overlay never flashes a black box.
bool TmLooksBlank(HBITMAP bmp, int w, int h) {
    if (!bmp || w <= 0 || h <= 0) return true;

    BITMAPINFO bi;
    ZeroMemory(&bi, sizeof(bi));
    bi.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth       = w;
    bi.bmiHeader.biHeight      = -h;
    bi.bmiHeader.biPlanes      = 1;
    bi.bmiHeader.biBitCount    = 32;
    bi.bmiHeader.biCompression = BI_RGB;

    size_t n = (size_t)w * h * 4;
    BYTE* p = (BYTE*)HeapAlloc(GetProcessHeap(), 0, n);
    if (!p) return false;   // can't tell - assume usable

    bool blank = false;
    HDC dc = GetDC(NULL);
    int r = GetDIBits(dc, bmp, 0, h, p, &bi, DIB_RGB_COLORS);
    ReleaseDC(NULL, dc);
    if (r) {
        long sampled = 0, nonblack = 0;
        const int step = 8;
        for (int y = 0; y < h; y += step) {
            BYTE* row = p + (size_t)y * w * 4;
            for (int x = 0; x < w; x += step) {
                BYTE* c = row + (size_t)x * 4;
                if (c[0] > 16 || c[1] > 16 || c[2] > 16) nonblack++;
                sampled++;
            }
        }
        if (sampled > 0) blank = (nonblack * 100 / sampled) < 2;
    }
    HeapFree(GetProcessHeap(), 0, p);
    return blank;
}

// Copy a sub-region of a bitmap into a new bitmap.
HBITMAP TmCrop(HBITMAP src, int x, int y, int w, int h) {
    if (!src || w <= 0 || h <= 0) return NULL;
    HDC screen = GetDC(NULL);
    HDC sdc = CreateCompatibleDC(screen);
    HDC ddc = CreateCompatibleDC(screen);
    HBITMAP dst = CreateCompatibleBitmap(screen, w, h);
    HBITMAP os = (HBITMAP)SelectObject(sdc, src);
    HBITMAP od = (HBITMAP)SelectObject(ddc, dst);
    BitBlt(ddc, 0, 0, w, h, sdc, x, y, SRCCOPY);
    SelectObject(sdc, os);
    SelectObject(ddc, od);
    DeleteDC(sdc);
    DeleteDC(ddc);
    ReleaseDC(NULL, screen);
    return dst;
}

// Screen-space origin and size of the window's client area.
bool TmClientInfo(HWND root, POINT* originScreen, int* clientW, int* clientH) {
    if (!root) return false;
    RECT cr;
    if (!GetClientRect(root, &cr)) return false;
    POINT o = { cr.left, cr.top };
    ClientToScreen(root, &o);
    *originScreen = o;
    *clientW = cr.right - cr.left;
    *clientH = cr.bottom - cr.top;
    return true;
}

// Fraction (0..1) of sampled pixels that differ noticeably between two same-size
// bitmaps. Used to tell a real view change (high) from a selection highlight or a
// live graph tick (low). Returns 1.0 (treat as changed) if it can't read the bits.
double TmChangedFraction(HBITMAP a, HBITMAP b, int w, int h) {
    if (!a || !b || w <= 0 || h <= 0) return 1.0;

    BITMAPINFO bi;
    ZeroMemory(&bi, sizeof(bi));
    bi.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth       = w;
    bi.bmiHeader.biHeight      = -h;   // top-down
    bi.bmiHeader.biPlanes      = 1;
    bi.bmiHeader.biBitCount    = 32;
    bi.bmiHeader.biCompression = BI_RGB;

    size_t n = (size_t)w * h * 4;
    BYTE* pa = (BYTE*)HeapAlloc(GetProcessHeap(), 0, n);
    BYTE* pb = (BYTE*)HeapAlloc(GetProcessHeap(), 0, n);
    double frac = 1.0;
    if (pa && pb) {
        HDC dc = GetDC(NULL);
        int ra = GetDIBits(dc, a, 0, h, pa, &bi, DIB_RGB_COLORS);
        int rb = GetDIBits(dc, b, 0, h, pb, &bi, DIB_RGB_COLORS);
        ReleaseDC(NULL, dc);
        if (ra && rb) {
            long sampled = 0, diff = 0;
            const int step = 4;
            for (int y = 0; y < h; y += step) {
                BYTE* rowa = pa + (size_t)y * w * 4;
                BYTE* rowb = pb + (size_t)y * w * 4;
                for (int x = 0; x < w; x += step) {
                    BYTE* ca = rowa + (size_t)x * 4;
                    BYTE* cb = rowb + (size_t)x * 4;
                    int d0 = ca[0] - cb[0]; if (d0 < 0) d0 = -d0;
                    int d1 = ca[1] - cb[1]; if (d1 < 0) d1 = -d1;
                    int d2 = ca[2] - cb[2]; if (d2 < 0) d2 = -d2;
                    if (d0 > 24 || d1 > 24 || d2 > 24) diff++;
                    sampled++;
                }
            }
            if (sampled > 0) frac = (double)diff / (double)sampled;
        }
    }
    if (pa) HeapFree(GetProcessHeap(), 0, pa);
    if (pb) HeapFree(GetProcessHeap(), 0, pb);
    return frac;
}

// Using UI Automation, find the clicked navigation item and return the screen X of
// its right edge - i.e. where the content next to it begins. Walks a few ancestors
// so we land on the whole item/list, not an inner icon/label.
bool TmNavItemRight(IUIAutomation* uia, POINT pt, int clientLeft, int clientW,
                    int* contentLeftScreen) {
    if (!uia) return false;

    IUIAutomationElement* el = NULL;
    if (FAILED(uia->ElementFromPoint(pt, &el)) || !el) return false;

    IUIAutomationTreeWalker* walker = NULL;
    uia->get_ControlViewWalker(&walker);

    const int leftMax  = (int)(clientW * 0.55);
    const int rightMax = (int)(clientW * 0.70);

    int best = -1;
    IUIAutomationElement* cur = el;
    for (int i = 0; i < 6 && cur; ++i) {
        RECT r;
        if (SUCCEEDED(cur->get_CurrentBoundingRectangle(&r))) {
            int leftRel  = r.left  - clientLeft;
            int rightRel = r.right - clientLeft;
            int width    = rightRel - leftRel;
            if (leftRel >= -8 && leftRel <= leftMax &&
                width >= 24 && width <= 360 &&
                rightRel >= 24 && rightRel <= rightMax &&
                rightRel > best) {
                best = rightRel;
            }
        }
        IUIAutomationElement* parent = NULL;
        if (walker) walker->GetParentElement(cur, &parent);
        cur->Release();
        cur = parent;
    }
    if (cur)    cur->Release();
    if (walker) walker->Release();

    if (best < 0) return false;
    *contentLeftScreen = clientLeft + best;
    return true;
}

// True if the window has moved or resized away from `orig` (or vanished). Our overlay
// is pinned to fixed screen coordinates, so once the window moves the overlay would
// become a detached ghost - the animation aborts the moment this returns true.
static bool TmWindowMoved(HWND root, const RECT& orig) {
    RECT cur;
    if (!root || !GetWindowRect(root, &cur)) return true;
    return cur.left   != orig.left  || cur.top    != orig.top ||
           cur.right  != orig.right || cur.bottom != orig.bottom;
}

// -------------------------------------------------------------------------
// Overlay management
//   Create  / Destroy  -> hook thread only (owns the window)
//   Animate            -> worker thread (UpdateLayeredWindow is thread-agnostic)
// -------------------------------------------------------------------------

// Destroy the live overlay and free its bitmap. MUST run on the hook thread.
void TmDestroyOverlay() {
    std::lock_guard<std::mutex> lock(g_overlayMutex);
    if (g_overlay.hwnd) {
        DestroyWindow(g_overlay.hwnd);
        g_overlay.hwnd = NULL;
    }
    if (g_overlay.frozenBmp) {
        DeleteObject(g_overlay.frozenBmp);
        g_overlay.frozenBmp = NULL;
    }
    g_overlay.rect    = RECT{0, 0, 0, 0};
    g_overlay.srcWnd  = NULL;
    g_overlay.srcRect = RECT{0, 0, 0, 0};
}

// Create the overlay showing the frozen old frame. Takes ownership of `frozen`.
// MUST run on the hook thread. Returns true on success.
bool TmCreateOverlay(HBITMAP frozen, const RECT& full, HWND srcWnd, const RECT& srcRect) {
    int W = full.right - full.left;
    int H = full.bottom - full.top;
    if (!frozen || W <= 0 || H <= 0) return false;

    // Clear any stale overlay first (shouldn't normally exist).
    TmDestroyOverlay();

    HWND hwnd = CreateWindowExW(
        WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_TOPMOST | WS_EX_TRANSPARENT,
        L"STATIC", NULL, WS_POPUP,
        full.left, full.top, W, H,
        NULL, NULL, NULL, NULL);
    if (!hwnd) return false;

    // Paint the frozen frame onto the layered window.
    HDC screen = GetDC(NULL);
    HDC src = CreateCompatibleDC(screen);
    HBITMAP os = (HBITMAP)SelectObject(src, frozen);

    POINT ptDst = { full.left, full.top };
    SIZE  sz    = { W, H };
    POINT ptSrc = { 0, 0 };
    BLENDFUNCTION bf;
    bf.BlendOp             = AC_SRC_OVER;
    bf.BlendFlags          = 0;
    bf.SourceConstantAlpha = 255;
    bf.AlphaFormat         = 0;
    UpdateLayeredWindow(hwnd, screen, &ptDst, &sz, src, &ptSrc, 0, &bf, ULW_ALPHA);

    SelectObject(src, os);
    DeleteDC(src);
    ReleaseDC(NULL, screen);

    ShowWindow(hwnd, SW_SHOWNOACTIVATE);

    std::lock_guard<std::mutex> lock(g_overlayMutex);
    g_overlay.hwnd      = hwnd;
    g_overlay.frozenBmp = frozen;
    g_overlay.rect      = full;
    g_overlay.srcWnd    = srcWnd;
    g_overlay.srcRect   = srcRect;
    return true;
}

// Hide the overlay if its source window has moved/resized. Runs on the hook thread
// (called for every mouse event). The animation worker can be blocked inside a
// cross-process UI Automation call while the user drags the window - Task Manager's
// UI thread is busy in its modal move loop and can't answer - so the worker can't
// tear the overlay down itself until the drag ends. Hiding it here kills the ghost
// immediately, regardless of the worker. We only hide (not destroy) so we never free
// the frozen bitmap out from under a worker that might still be animating.
void TmHideOverlayIfMoved() {
    std::lock_guard<std::mutex> lock(g_overlayMutex);
    if (!g_overlay.hwnd) return;
    if (TmWindowMoved(g_overlay.srcWnd, g_overlay.srcRect)) {
        ShowWindow(g_overlay.hwnd, SW_HIDE);
    }
}

// Ask the hook thread to destroy the overlay once the worker is finished.
void TmAnimationDone() {
    if (g_hookThreadId) {
        PostThreadMessageW(g_hookThreadId, WM_FADE_DESTROY_WND, 0, 0);
    }
}

// Crossfade the EXISTING overlay: the frozen full frame stays as the base, and the
// new content (offsetX..offsetX+cw, 0..ch within the overlay) is blended in from
// alpha 0 -> 255 over the animation duration. Runs on the worker thread.
void TmFadeAnimate(HBITMAP newBmp, int offsetX, int cw, int ch, HWND root, const RECT& winRect) {
    HWND    hwnd;
    HBITMAP frozen;
    RECT    rect;
    {
        std::lock_guard<std::mutex> lock(g_overlayMutex);
        hwnd   = g_overlay.hwnd;
        frozen = g_overlay.frozenBmp;
        rect   = g_overlay.rect;
    }
    if (!hwnd || !frozen || !newBmp) return;

    int W = rect.right - rect.left;
    int H = rect.bottom - rect.top;
    if (W <= 0 || H <= 0) return;

    HDC screen   = GetDC(NULL);
    HDC frozenDC = CreateCompatibleDC(screen);
    HDC newDC    = CreateCompatibleDC(screen);
    HDC canvasDC = CreateCompatibleDC(screen);
    HBITMAP canvas  = CreateCompatibleBitmap(screen, W, H);
    HBITMAP oldSelF = (HBITMAP)SelectObject(frozenDC, frozen);
    HBITMAP oldSelN = (HBITMAP)SelectObject(newDC,    newBmp);
    HBITMAP oldSelC = (HBITMAP)SelectObject(canvasDC, canvas);

    const double total = (double)g_durationMs.load(std::memory_order_relaxed);

    LARGE_INTEGER freq, start, now;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&start);

    for (;;) {
        QueryPerformanceCounter(&now);
        double elapsed = (now.QuadPart - start.QuadPart) * 1000.0 / freq.QuadPart;
        BOOL last = (elapsed >= total);
        float p = last ? 1.0f : (float)(elapsed / total);
        float e = p * p * (3.0f - 2.0f * p);   // smoothstep ease in/out

        // Bail if the user moved/resized the window - a fixed-position overlay would
        // otherwise linger as a detached ghost during the move.
        if (TmWindowMoved(root, winRect)) break;

        // Base = frozen full frame. Then blend the new content region on top with
        // alpha tied to eased progress, so the nav stays frozen and only the
        // content crossfades.
        BitBlt(canvasDC, 0, 0, W, H, frozenDC, 0, 0, SRCCOPY);

        BLENDFUNCTION bfBlend;
        bfBlend.BlendOp             = AC_SRC_OVER;
        bfBlend.BlendFlags          = 0;
        bfBlend.SourceConstantAlpha = (BYTE)(e * 255.0f);
        bfBlend.AlphaFormat         = 0;
        AlphaBlend(canvasDC, offsetX, 0, cw, ch, newDC, 0, 0, cw, ch, bfBlend);

        POINT ptDst = { rect.left, rect.top };
        SIZE  sz    = { W, H };
        POINT ptSrc = { 0, 0 };
        BLENDFUNCTION bf;
        bf.BlendOp             = AC_SRC_OVER;
        bf.BlendFlags          = 0;
        bf.SourceConstantAlpha = 255;
        bf.AlphaFormat         = 0;
        UpdateLayeredWindow(hwnd, screen, &ptDst, &sz, canvasDC, &ptSrc, 0, &bf, ULW_ALPHA);

        if (last) break;
        DwmFlush();
    }

    SelectObject(canvasDC, oldSelC);
    SelectObject(newDC,    oldSelN);
    SelectObject(frozenDC, oldSelF);
    DeleteObject(canvas);
    DeleteDC(canvasDC);
    DeleteDC(newDC);
    DeleteDC(frozenDC);
    ReleaseDC(NULL, screen);
}

// Slide the content region of the EXISTING overlay: the frozen full frame is the
// base (keeps the left nav frozen), and within the content sub-rect the old content
// slides out while the new content slides in. Drawing is clipped to the content rect
// so the sliding bitmaps never spill over the nav. Runs on the worker thread.
void TmSlideAnimate(HBITMAP newContent, HBITMAP oldContent, int offsetX, int cw, int ch, HWND root, const RECT& winRect) {
    HWND    hwnd;
    HBITMAP frozen;
    RECT    rect;
    {
        std::lock_guard<std::mutex> lock(g_overlayMutex);
        hwnd   = g_overlay.hwnd;
        frozen = g_overlay.frozenBmp;
        rect   = g_overlay.rect;
    }
    if (!hwnd || !frozen || !newContent || !oldContent) return;

    int W = rect.right - rect.left;
    int H = rect.bottom - rect.top;
    if (W <= 0 || H <= 0) return;

    const bool vertical = g_slideVertical.load(std::memory_order_relaxed);
    const int  span     = vertical ? ch : cw;

    HDC screen   = GetDC(NULL);
    HDC frozenDC = CreateCompatibleDC(screen);
    HDC oldDC    = CreateCompatibleDC(screen);
    HDC newDC    = CreateCompatibleDC(screen);
    HDC canvasDC = CreateCompatibleDC(screen);
    HBITMAP canvas  = CreateCompatibleBitmap(screen, W, H);
    HBITMAP oldSelF = (HBITMAP)SelectObject(frozenDC, frozen);
    HBITMAP oldSelO = (HBITMAP)SelectObject(oldDC,    oldContent);
    HBITMAP oldSelN = (HBITMAP)SelectObject(newDC,    newContent);
    HBITMAP oldSelC = (HBITMAP)SelectObject(canvasDC, canvas);

    const double total = (double)g_durationMs.load(std::memory_order_relaxed);

    LARGE_INTEGER freq, start, now;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&start);

    for (;;) {
        QueryPerformanceCounter(&now);
        double elapsed = (now.QuadPart - start.QuadPart) * 1000.0 / freq.QuadPart;
        BOOL last = (elapsed >= total);
        float p = last ? 1.0f : (float)(elapsed / total);
        float e = p * p * (3.0f - 2.0f * p);   // smoothstep ease in/out

        // Bail if the user moved/resized the window (see TmFadeAnimate).
        if (TmWindowMoved(root, winRect)) break;

        int oldOff = (int)(-e * span);
        int newOff = (int)((1.0f - e) * span);
        int ox = vertical ? 0 : oldOff;
        int oy = vertical ? oldOff : 0;
        int nx = vertical ? 0 : newOff;
        int ny = vertical ? newOff : 0;

        // Base = frozen full frame (keeps the nav frozen). Then draw the sliding old
        // and new content, clipped to the content rect so nothing paints the nav.
        BitBlt(canvasDC, 0, 0, W, H, frozenDC, 0, 0, SRCCOPY);

        HRGN clip = CreateRectRgn(offsetX, 0, offsetX + cw, ch);
        SelectClipRgn(canvasDC, clip);
        BitBlt(canvasDC, offsetX + ox, oy, cw, ch, oldDC, 0, 0, SRCCOPY);
        BitBlt(canvasDC, offsetX + nx, ny, cw, ch, newDC, 0, 0, SRCCOPY);
        SelectClipRgn(canvasDC, NULL);
        DeleteObject(clip);

        POINT ptDst = { rect.left, rect.top };
        SIZE  sz    = { W, H };
        POINT ptSrc = { 0, 0 };
        BLENDFUNCTION bf;
        bf.BlendOp             = AC_SRC_OVER;
        bf.BlendFlags          = 0;
        bf.SourceConstantAlpha = 255;
        bf.AlphaFormat         = 0;
        UpdateLayeredWindow(hwnd, screen, &ptDst, &sz, canvasDC, &ptSrc, 0, &bf, ULW_ALPHA);

        if (last) break;
        DwmFlush();
    }

    SelectObject(canvasDC, oldSelC);
    SelectObject(newDC,    oldSelN);
    SelectObject(oldDC,    oldSelO);
    SelectObject(frozenDC, oldSelF);
    DeleteObject(canvas);
    DeleteDC(canvasDC);
    DeleteDC(newDC);
    DeleteDC(oldDC);
    DeleteDC(frozenDC);
    ReleaseDC(NULL, screen);
}

// -------------------------------------------------------------------------
// Worker: wait for the new view to render, locate the content edge via UI
// Automation, capture the new view, play the chosen animation on the existing
// overlay (only if the content actually changed), then ask the hook thread to
// destroy the overlay.
// -------------------------------------------------------------------------
DWORD WINAPI TmWorkerThread(LPVOID lpParam) {
    TabAnimData* d = (TabAnimData*)lpParam;

    Sleep(g_captureDelay.load(std::memory_order_relaxed));

    // If the user started moving/resizing the window during the capture delay, our
    // fixed-position overlay would show as a detached ghost - abort and destroy it.
    if (TmWindowMoved(d->root, d->winRect)) {
        TmAnimationDone();
        delete d;
        g_inProgress.store(false, std::memory_order_relaxed);
        return 0;
    }

    int clientW     = d->fullRect.right - d->fullRect.left;
    int contentLeft = d->fullRect.left;
    bool located    = false;

    HRESULT hrCo = CoInitializeEx(NULL, COINIT_MULTITHREADED);
    IUIAutomation* uia = NULL;
    if (SUCCEEDED(CoCreateInstance(kCLSID_CUIAutomation, NULL, CLSCTX_INPROC_SERVER,
                                   kIID_IUIAutomation, (void**)&uia)) && uia) {
        located = TmNavItemRight(uia, d->clickPt, d->fullRect.left, clientW, &contentLeft);
        uia->Release();
    }
    if (SUCCEEDED(hrCo)) CoUninitialize();

    if (located) {
        int offsetX = contentLeft - d->fullRect.left;
        RECT content = { contentLeft, d->fullRect.top, d->fullRect.right, d->fullRect.bottom };
        int cw = content.right  - content.left;
        int ch = content.bottom - content.top;

        if (offsetX >= 0 && cw >= 80 && ch >= 80) {
            // Crop the OLD content out of the overlay's frozen full frame.
            HBITMAP oldCrop = NULL;
            {
                std::lock_guard<std::mutex> lock(g_overlayMutex);
                if (g_overlay.frozenBmp) {
                    oldCrop = TmCrop(g_overlay.frozenBmp, offsetX, 0, cw, ch);
                }
            }

            if (oldCrop) {
                // Re-capture the new view until it has actually rendered. Some tabs
                // (Disk / Network / GPU) draw their first frame noticeably later than
                // CPU / Memory, so a single fixed delay can miss them. Poll until the
                // content visibly changes (or give up).
                const double readyFrac = 0.06;
                const double userThr   = g_changeThreshold.load(std::memory_order_relaxed) / 100.0;
                HBITMAP newBmp = NULL;
                double  frac   = 0.0;
                for (int attempt = 0; attempt < 6; ++attempt) {
                    // Capture from the window itself, not the screen - the overlay
                    // sits on top of `content`, so a screen BitBlt would just read
                    // back our own frozen old frame and the animation would never play.
                    HBITMAP cap = TmCaptureWindowRegion(d->root, content);
                    if (!cap) break;
                    if (newBmp) DeleteObject(newBmp);
                    newBmp = cap;
                    frac = TmChangedFraction(oldCrop, newBmp, cw, ch);
                    if (frac >= readyFrac) break;  // new view has rendered
                    Sleep(35);
                }

                if (newBmp && frac >= userThr && !TmWindowMoved(d->root, d->winRect)) {
                    if (d->mode == ANIM_SLIDE) {
                        TmSlideAnimate(newBmp, oldCrop, offsetX, cw, ch, d->root, d->winRect);
                    } else {
                        TmFadeAnimate(newBmp, offsetX, cw, ch, d->root, d->winRect);
                    }
                }
                if (newBmp) DeleteObject(newBmp);
                DeleteObject(oldCrop);
            }
        }
    }

    // Hand the overlay back to the hook thread for destruction.
    TmAnimationDone();

    delete d;
    g_inProgress.store(false, std::memory_order_relaxed);
    return 0;
}

// -------------------------------------------------------------------------
// Click detection (low-level mouse hook)
//   mouse-down -> capture old frame + create overlay immediately
//   mouse-up   -> launch the worker that animates and destroys it
// -------------------------------------------------------------------------
void TmOnClick(WPARAM msg, POINT pt) {
    // ------------------------------------------------------------------
    // FINISH: launch the worker.
    // ------------------------------------------------------------------
    if (msg == WM_LBUTTONUP) {
        int   mode;
        HWND  root;
        POINT click;
        RECT  full;
        RECT  win;
        {
            std::lock_guard<std::mutex> lock(g_mutex);
            if (!g_armed) return;
            g_armed = false;
            mode  = g_pendingMode;
            root  = g_pendingRoot;
            click = g_pendingClick;
            full  = g_pendingRect;
            win   = g_pendingWin;
        }

        // An overlay was created at mouse-down. If a previous cycle is somehow still
        // running, tear this overlay down and bail.
        if (g_inProgress.load(std::memory_order_relaxed)) {
            TmDestroyOverlay();
            return;
        }

        g_inProgress.store(true, std::memory_order_relaxed);

        TabAnimData* d = new TabAnimData();
        d->root     = root;
        d->clickPt  = click;
        d->fullRect = full;
        d->winRect  = win;
        d->mode     = mode;

        HANDLE h = CreateThread(NULL, 0, TmWorkerThread, d, 0, NULL);
        if (h) {
            CloseHandle(h);
        } else {
            delete d;
            TmDestroyOverlay();   // we're on the hook thread - safe
            g_inProgress.store(false, std::memory_order_relaxed);
        }
        return;
    }

    // ------------------------------------------------------------------
    // START: capture the current view and show the overlay immediately, before the
    // click changes the tab, if the click is plausibly in a left nav column.
    // ------------------------------------------------------------------
    if (msg != WM_LBUTTONDOWN) return;
    if (g_inProgress.load(std::memory_order_relaxed)) return;

    HWND under = WindowFromPoint(pt);
    HWND root  = under ? GetAncestor(under, GA_ROOT) : NULL;
    if (!root) return;

    DWORD pid = 0;
    GetWindowThreadProcessId(root, &pid);
    if (pid != GetCurrentProcessId()) return;

    POINT origin;
    int cw, ch;
    if (!TmClientInfo(root, &origin, &cw, &ch)) return;
    int relX = pt.x - origin.x;
    int relY = pt.y - origin.y;

    int topI = g_topInset.load(std::memory_order_relaxed);
    // Below the command bar, and in the left portion where the nav / sub-nav can be.
    if (relX < 0 || relY < topI || relX > (cw * 3) / 5 || relY >= ch) return;

    RECT full;
    full.left   = origin.x;
    full.top    = origin.y + topI;
    full.right  = origin.x + cw;
    full.bottom = origin.y + ch;
    if ((full.right - full.left) < 80 || (full.bottom - full.top) < 80) return;

    // Capture the old frame the same way the worker captures the new one
    // (PrintWindow), so the change-detection comparison is apples-to-apples. If
    // PrintWindow isn't usable here (null or a blank/black frame), fall back to a
    // plain screen grab for the displayed frame - the animation then simply no-ops
    // instead of showing a black overlay.
    int fullW = full.right - full.left;
    int fullH = full.bottom - full.top;
    HBITMAP oldBmp = TmCaptureWindowRegion(root, full);
    if (!oldBmp || TmLooksBlank(oldBmp, fullW, fullH)) {
        if (oldBmp) DeleteObject(oldBmp);
        oldBmp = TmCaptureRect(full);
    }
    if (!oldBmp) return;

    RECT winRect = {0, 0, 0, 0};
    GetWindowRect(root, &winRect);

    // Show the frozen old frame immediately so the tab switches invisibly. The
    // overlay takes ownership of oldBmp (freed in TmDestroyOverlay).
    if (!TmCreateOverlay(oldBmp, full, root, winRect)) {
        DeleteObject(oldBmp);
        return;
    }

    std::lock_guard<std::mutex> lock(g_mutex);
    g_pendingMode  = g_animType.load(std::memory_order_relaxed);
    g_pendingRoot  = root;
    g_pendingClick = pt;
    g_pendingRect  = full;
    g_pendingWin   = winRect;
    g_armed        = true;
}

LRESULT CALLBACK TmMouseProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        // Every mouse event (incl. moves during a title-bar drag): if a live overlay's
        // window has moved, hide it now so it doesn't linger as a detached ghost.
        TmHideOverlayIfMoved();

        if (wParam == WM_LBUTTONDOWN || wParam == WM_LBUTTONUP) {
            MSLLHOOKSTRUCT* ms = (MSLLHOOKSTRUCT*)lParam;
            TmOnClick(wParam, ms->pt);
        }
    }
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

// Dedicated thread owning the hook and pumping messages (required for LL hooks).
// It also owns the overlay window, so it is the only place the overlay is destroyed.
DWORD WINAPI TmHookThread(LPVOID lpParam) {
    g_mouseHook = SetWindowsHookExW(WH_MOUSE_LL, TmMouseProc, GetModuleHandleW(NULL), 0);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0) > 0) {
        if (msg.message == WM_FADE_DESTROY_WND) {
            TmDestroyOverlay();
            continue;
        }
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // Destroy any surviving overlay on its creating thread before we exit.
    TmDestroyOverlay();

    if (g_mouseHook) {
        UnhookWindowsHookEx(g_mouseHook);
        g_mouseHook = NULL;
    }
    return 0;
}

// -------------------------------------------------------------------------
// Windhawk entry points
// -------------------------------------------------------------------------
BOOL Wh_ModInit() {
    TmLoadSettings();
    g_hookThread = CreateThread(NULL, 0, TmHookThread, NULL, 0, &g_hookThreadId);
    return TRUE;
}

void Wh_ModSettingsChanged() {
    TmLoadSettings();
}

void Wh_ModUninit() {
    if (g_hookThread) {
        if (g_hookThreadId) {
            PostThreadMessageW(g_hookThreadId, WM_QUIT, 0, 0);
        }
        WaitForSingleObject(g_hookThread, 2000);
        CloseHandle(g_hookThread);
        g_hookThread   = NULL;
        g_hookThreadId = 0;
    }

    // The hook thread destroys the overlay on exit; this frees anything left.
    TmDestroyOverlay();
}
