// ==WindhawkMod==
// @id              taskmgr-tab-slide-animation
// @name            Task Manager Tab Slide Animation
// @description     Smooth crossfade transition when switching tabs (Processes, Performance, ...) in Task Manager.
// @version         0.3.3
// @author          Abdullah Masood
// @github          https://github.com/Abdullah-Masood-05
// @include         Taskmgr.exe
// @compilerOptions -ldwmapi -lgdi32 -lole32 -lmsimg32
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Task Manager Tab Slide Animation

![Demo](https://raw.githubusercontent.com/Abdullah-Masood-05/my-windhawk-mods-media/main/taskmgr-tab-slide-animation.gif)

The Windows 11 Task Manager switches between its tabs instantly. This mod adds a
smooth **crossfade** transition - both for the main left navigation (Processes,
Performance, App history, ...) and for the **Performance sub-pages** (CPU, Memory,
Disk, Network/Ethernet, Wi-Fi, GPU).

> Experimental. Task Manager is a WinUI 3 app, so there's no clean Win32
> "tab changed" signal - this works by heuristics and screen capture.

## v2 architecture (no visual jump)
v1 created the overlay *after* Task Manager had already switched the tab, so you
could see a brief jump to the new view before the fade started. v2 fixes this:

1. On **mouse-down** in a nav area, the current view is captured and a topmost,
   click-through layered overlay showing that frozen frame is created **immediately**.
2. Task Manager switches the tab *underneath* the overlay - invisibly.
3. A worker thread waits for the new view to render, locates the content edge via
   **UI Automation**, captures the new view, and crossfades the overlay from the
   frozen old frame to the new frame.
4. The overlay is then destroyed **only on the hook thread** (via a posted message),
   because a window must be destroyed on the thread that created it.

Result: no visible jump - the old frame stays on screen until the fade plays.

## Settings
- **Top inset** - height (px) of the top command bar to exclude.
- **Capture delay** - delay after the click before capturing the new view (ms).
- **Change threshold** - minimum % of the region that must change to animate.
- **Animation duration** to taste.

## Notes / limitations
- Because the overlay is shown on every qualifying left-click, a click that does
  **not** change the view (e.g. selecting a process row) briefly shows the frozen
  frame for up to "capture delay" ms before the overlay disappears with no fade.
  Keep the capture delay modest to minimise this.
- Relies on UI Automation to locate the clicked item.
- The window should be visible/unobscured during the switch.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- duration_ms: 250
  $name: Animation duration (ms)
  $description: How long the crossfade lasts. Clamped to 60-1500.
- top_inset: 48
  $name: Top inset (px)
  $description: Height of the top command bar to exclude from the fade.
- capture_delay_ms: 140
  $name: Capture delay (ms)
  $description: Delay after the click before capturing the new view. Raise if it grabs the old view.
- change_threshold: 2
  $name: Change threshold (%)
  $description: Minimum percent of the region that must change for the fade to play. Raise to reduce spurious fades.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <dwmapi.h>
#include <uiautomation.h>
#include <atomic>
#include <mutex>

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

// -------------------------------------------------------------------------
// State
// -------------------------------------------------------------------------

// Data handed to the worker thread. In v2 the captured old frame lives on the
// overlay (g_overlay.frozenBmp), so the worker only needs the click point and the
// overlay's screen rect.
struct TabFadeData {
    HWND  root;      // the Task Manager window (for PrintWindow capture)
    POINT clickPt;   // screen point of the nav click (for UI Automation)
    RECT  fullRect;  // screen rect the overlay covers
};

// The single live overlay. Created on the hook thread at mouse-down, read by the
// worker during the fade, destroyed on the hook thread afterwards.
struct OverlayState {
    HWND    hwnd;       // topmost, click-through, layered window
    HBITMAP frozenBmp;  // the full old frame captured at mouse-down (overlay owns it)
    RECT    rect;       // screen rect of the overlay
};

std::atomic<int>  g_durationMs{250};
std::atomic<int>  g_topInset{48};
std::atomic<int>  g_captureDelay{140};
std::atomic<int>  g_changeThreshold{2};

std::atomic<bool> g_inProgress{false}; // a fade cycle (overlay live) is in flight

// Pairing state between mouse-down (create overlay) and mouse-up (start worker).
std::mutex g_mutex;
HWND       g_pendingRoot  = NULL;
POINT      g_pendingClick = {0, 0};
RECT       g_pendingRect  = {0, 0, 0, 0};
bool       g_armed        = false;

// Overlay state, guarded by its own mutex (touched by hook + worker threads).
std::mutex   g_overlayMutex;
OverlayState g_overlay = { NULL, NULL, {0, 0, 0, 0} };

// Mouse-hook thread.
HHOOK  g_mouseHook    = NULL;
HANDLE g_hookThread   = NULL;
DWORD  g_hookThreadId = 0;

void TmFadeLoadSettings() {
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
}

// -------------------------------------------------------------------------
// Capture / geometry helpers
// -------------------------------------------------------------------------

// Grab a screen rectangle into a new DDB (caller owns / deletes it).
HBITMAP TmFadeCaptureRect(const RECT& r) {
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
HBITMAP TmFadeCaptureWindowRegion(HWND root, const RECT& screenRegion) {
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
bool TmFadeLooksBlank(HBITMAP bmp, int w, int h) {
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
HBITMAP TmFadeCrop(HBITMAP src, int x, int y, int w, int h) {
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
bool TmFadeClientInfo(HWND root, POINT* originScreen, int* clientW, int* clientH) {
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
double TmFadeChangedFraction(HBITMAP a, HBITMAP b, int w, int h) {
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
bool TmFadeNavItemRight(IUIAutomation* uia, POINT pt, int clientLeft, int clientW,
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

// -------------------------------------------------------------------------
// Overlay management
//   Create  / Destroy  -> hook thread only (owns the window)
//   Animate            -> worker thread (UpdateLayeredWindow is thread-agnostic)
// -------------------------------------------------------------------------

// Destroy the live overlay and free its bitmap. MUST run on the hook thread.
void TmFadeDestroyOverlay() {
    std::lock_guard<std::mutex> lock(g_overlayMutex);
    if (g_overlay.hwnd) {
        DestroyWindow(g_overlay.hwnd);
        g_overlay.hwnd = NULL;
    }
    if (g_overlay.frozenBmp) {
        DeleteObject(g_overlay.frozenBmp);
        g_overlay.frozenBmp = NULL;
    }
    g_overlay.rect = RECT{0, 0, 0, 0};
}

// Create the overlay showing the frozen old frame. Takes ownership of `frozen`.
// MUST run on the hook thread. Returns true on success.
bool TmFadeCreateOverlay(HBITMAP frozen, const RECT& full) {
    int W = full.right - full.left;
    int H = full.bottom - full.top;
    if (!frozen || W <= 0 || H <= 0) return false;

    // Clear any stale overlay first (shouldn't normally exist).
    TmFadeDestroyOverlay();

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
    return true;
}

// Ask the hook thread to destroy the overlay once the worker is finished.
void TmFadeAnimationDone() {
    if (g_hookThreadId) {
        PostThreadMessageW(g_hookThreadId, WM_FADE_DESTROY_WND, 0, 0);
    }
}

// Crossfade the EXISTING overlay: the frozen full frame stays as the base, and the
// new content (offsetX..offsetX+cw, 0..ch within the overlay) is blended in from
// alpha 0 -> 255 over the animation duration. Runs on the worker thread.
void TmFadeAnimate(HBITMAP newBmp, int offsetX, int cw, int ch) {
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

// -------------------------------------------------------------------------
// Worker: wait for the new view to render, locate the content edge via UI
// Automation, capture the new view, crossfade the existing overlay (only if the
// content actually changed), then ask the hook thread to destroy the overlay.
// -------------------------------------------------------------------------
DWORD WINAPI TmFadeWorkerThread(LPVOID lpParam) {
    TabFadeData* d = (TabFadeData*)lpParam;

    Sleep(g_captureDelay.load(std::memory_order_relaxed));

    int clientW     = d->fullRect.right - d->fullRect.left;
    int contentLeft = d->fullRect.left;
    bool located    = false;

    HRESULT hrCo = CoInitializeEx(NULL, COINIT_MULTITHREADED);
    IUIAutomation* uia = NULL;
    if (SUCCEEDED(CoCreateInstance(kCLSID_CUIAutomation, NULL, CLSCTX_INPROC_SERVER,
                                   kIID_IUIAutomation, (void**)&uia)) && uia) {
        located = TmFadeNavItemRight(uia, d->clickPt, d->fullRect.left, clientW, &contentLeft);
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
                    oldCrop = TmFadeCrop(g_overlay.frozenBmp, offsetX, 0, cw, ch);
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
                    // back our own frozen old frame and the fade would never play.
                    HBITMAP cap = TmFadeCaptureWindowRegion(d->root, content);
                    if (!cap) break;
                    if (newBmp) DeleteObject(newBmp);
                    newBmp = cap;
                    frac = TmFadeChangedFraction(oldCrop, newBmp, cw, ch);
                    if (frac >= readyFrac) break;  // new view has rendered
                    Sleep(35);
                }

                if (newBmp && frac >= userThr) {
                    TmFadeAnimate(newBmp, offsetX, cw, ch);
                }
                if (newBmp) DeleteObject(newBmp);
                DeleteObject(oldCrop);
            }
        }
    }

    // Hand the overlay back to the hook thread for destruction.
    TmFadeAnimationDone();

    delete d;
    g_inProgress.store(false, std::memory_order_relaxed);
    return 0;
}

// -------------------------------------------------------------------------
// Click detection (low-level mouse hook)
//   mouse-down -> capture old frame + create overlay immediately
//   mouse-up   -> launch the worker that fades and destroys it
// -------------------------------------------------------------------------
void TmFadeOnClick(WPARAM msg, POINT pt) {
    // ------------------------------------------------------------------
    // FINISH: launch the worker.
    // ------------------------------------------------------------------
    if (msg == WM_LBUTTONUP) {
        HWND  root;
        POINT click;
        RECT  full;
        {
            std::lock_guard<std::mutex> lock(g_mutex);
            if (!g_armed) return;
            g_armed = false;
            root  = g_pendingRoot;
            click = g_pendingClick;
            full  = g_pendingRect;
        }

        // An overlay was created at mouse-down. If a previous cycle is somehow
        // still running, tear this overlay down and bail.
        if (g_inProgress.load(std::memory_order_relaxed)) {
            TmFadeDestroyOverlay();
            return;
        }

        g_inProgress.store(true, std::memory_order_relaxed);

        TabFadeData* d = new TabFadeData();
        d->root     = root;
        d->clickPt  = click;
        d->fullRect = full;

        HANDLE h = CreateThread(NULL, 0, TmFadeWorkerThread, d, 0, NULL);
        if (h) {
            CloseHandle(h);
        } else {
            delete d;
            TmFadeDestroyOverlay();   // we're on the hook thread - safe
            g_inProgress.store(false, std::memory_order_relaxed);
        }
        return;
    }

    // ------------------------------------------------------------------
    // START: capture the current view and show the overlay immediately, before
    // the click changes the tab, if the click is plausibly in a left nav column.
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
    if (!TmFadeClientInfo(root, &origin, &cw, &ch)) return;
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
    // plain screen grab for the displayed frame - the fade then simply no-ops
    // instead of showing a black overlay.
    int fullW = full.right - full.left;
    int fullH = full.bottom - full.top;
    HBITMAP oldBmp = TmFadeCaptureWindowRegion(root, full);
    if (!oldBmp || TmFadeLooksBlank(oldBmp, fullW, fullH)) {
        if (oldBmp) DeleteObject(oldBmp);
        oldBmp = TmFadeCaptureRect(full);
    }
    if (!oldBmp) return;

    // Show the frozen old frame immediately so the tab switches invisibly. The
    // overlay takes ownership of oldBmp (freed in TmFadeDestroyOverlay).
    if (!TmFadeCreateOverlay(oldBmp, full)) {
        DeleteObject(oldBmp);
        return;
    }

    std::lock_guard<std::mutex> lock(g_mutex);
    g_pendingRoot  = root;
    g_pendingClick = pt;
    g_pendingRect  = full;
    g_armed        = true;
}

LRESULT CALLBACK TmFadeMouseProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION && (wParam == WM_LBUTTONDOWN || wParam == WM_LBUTTONUP)) {
        MSLLHOOKSTRUCT* ms = (MSLLHOOKSTRUCT*)lParam;
        TmFadeOnClick(wParam, ms->pt);
    }
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

// Dedicated thread owning the hook and pumping messages (required for LL hooks).
// It also owns the overlay window, so it is the only place the overlay is destroyed.
DWORD WINAPI TmFadeHookThread(LPVOID lpParam) {
    g_mouseHook = SetWindowsHookExW(WH_MOUSE_LL, TmFadeMouseProc, GetModuleHandleW(NULL), 0);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0) > 0) {
        if (msg.message == WM_FADE_DESTROY_WND) {
            TmFadeDestroyOverlay();
            continue;
        }
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // Destroy any surviving overlay on its creating thread before we exit.
    TmFadeDestroyOverlay();

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
    TmFadeLoadSettings();
    g_hookThread = CreateThread(NULL, 0, TmFadeHookThread, NULL, 0, &g_hookThreadId);
    return TRUE;
}

void Wh_ModSettingsChanged() {
    TmFadeLoadSettings();
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
    TmFadeDestroyOverlay();
}
