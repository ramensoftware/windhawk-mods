// ==WindhawkMod==
// @id              taskmgr-tab-slide-animation
// @name            Task Manager Tab Slide Animation
// @description     Smooth slide transition when switching tabs (Processes, Performance, ...) in Task Manager.
// @version         0.3.2
// @author          Abdullah Masood
// @github          https://github.com/Abdullah-Masood-05
// @include         Taskmgr.exe
// @compilerOptions -ldwmapi -lgdi32 -lole32
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Task Manager Tab Slide Animation

![Demo](https://raw.githubusercontent.com/Abdullah-Masood-05/my-windhawk-mods-media/main/taskmgr-tab-slide-animation.gif)

The Windows 11 Task Manager switches between its tabs instantly. This mod adds a
smooth **slide** transition - both for the main left navigation (Processes,
Performance, App history, ...) and for the **Performance sub-pages** (CPU, Memory,
Disk, Network/Ethernet, Wi-Fi, GPU).

> Experimental. Task Manager is a WinUI 3 app, so there's no clean Win32
> "tab changed" signal - this works by heuristics and screen capture.

## How it works
- A low-level mouse hook (scoped to Task Manager) notices a click in a left-hand
  navigation area and screen-captures the content next to it (old view).
- **UI Automation** reads the clicked item's bounding rectangle - its right edge is
  where the content begins - so the slide region adapts automatically to the main
  nav (expanded or collapsed) *and* to the Performance sub-list.
- After a short delay it captures the new view. It only plays the slide if the
  content actually **changed** by more than a threshold, so clicking a process row,
  selecting, or live graph updates don't trigger a spurious slide.

## Settings
- **Top inset** - height (px) of the top command bar to exclude.
- **Capture delay** - delay after the click before capturing the new view (ms).
- **Change threshold** - minimum % of the region that must change to animate.
- **Animation duration** and **slide vertically** to taste.

## Limitations
- Relies on UI Automation to locate the clicked item.
- The window should be visible/unobscured during the switch.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- duration_ms: 250
  $name: Animation duration (ms)
  $description: How long the slide lasts. Clamped to 60-1500.
- top_inset: 48
  $name: Top inset (px)
  $description: Height of the top command bar to exclude from the slide.
- capture_delay_ms: 140
  $name: Capture delay (ms)
  $description: Delay after the click before capturing the new view. Raise if it grabs the old view.
- change_threshold: 2
  $name: Change threshold (%)
  $description: Minimum percent of the region that must change for the slide to play. Raise to reduce spurious slides.
- slide_vertical: false
  $name: Slide vertically
  $description: Slide up/down instead of left/right.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <dwmapi.h>
#include <uiautomation.h>
#include <atomic>
#include <mutex>

// CLSID_CUIAutomation / IID_IUIAutomation defined inline so we don't depend on the
// uuid import library being linked.
static const CLSID kCLSID_CUIAutomation =
    { 0xff48dba4, 0x60ef, 0x4201, { 0xaa, 0x87, 0x54, 0x10, 0x3e, 0xef, 0x59, 0x4e } };
static const IID kIID_IUIAutomation =
    { 0x30cbe57d, 0xd9d0, 0x452a, { 0xab, 0x13, 0x7a, 0xc5, 0xac, 0x48, 0x25, 0xee } };

// -------------------------------------------------------------------------
// State
// -------------------------------------------------------------------------
struct TabSlideData {
    HBITMAP oldFull;   // full client area (below top inset) captured before the switch
    POINT   clickPt;   // screen point of the nav click (for UI Automation)
    RECT    fullRect;  // screen rect that oldFull was captured from
};

std::atomic<int>  g_durationMs{250};
std::atomic<int>  g_topInset{48};
std::atomic<int>  g_captureDelay{140};
std::atomic<int>  g_changeThreshold{2};
std::atomic<bool> g_slideVertical{false};

std::atomic<bool> g_inProgress{false}; // an animation is currently running

// Pairing state between mouse-down (capture old) and mouse-up (start slide).
std::mutex g_mutex;
HBITMAP    g_pendingOld = NULL;
POINT      g_pendingClick = {0, 0};
RECT       g_pendingRect = {0, 0, 0, 0};
bool       g_armed = false;

// Mouse-hook thread.
HHOOK  g_mouseHook = NULL;
HANDLE g_hookThread = NULL;
DWORD  g_hookThreadId = 0;

void TmSlideLoadSettings() {
    int ms = Wh_GetIntSetting(L"duration_ms");
    if (ms < 60) ms = 60;
    if (ms > 1500) ms = 1500;
    g_durationMs.store(ms, std::memory_order_relaxed);

    int top = Wh_GetIntSetting(L"top_inset");
    if (top < 0) top = 0;
    g_topInset.store(top, std::memory_order_relaxed);

    int delay = Wh_GetIntSetting(L"capture_delay_ms");
    if (delay < 0) delay = 0;
    if (delay > 1000) delay = 1000;
    g_captureDelay.store(delay, std::memory_order_relaxed);

    int thr = Wh_GetIntSetting(L"change_threshold");
    if (thr < 1) thr = 1;
    if (thr > 100) thr = 100;
    g_changeThreshold.store(thr, std::memory_order_relaxed);

    g_slideVertical.store(Wh_GetIntSetting(L"slide_vertical") != 0, std::memory_order_relaxed);
}

// -------------------------------------------------------------------------
// Capture / geometry helpers
// -------------------------------------------------------------------------

// Grab a screen rectangle into a new DDB (caller owns / deletes it).
HBITMAP TmSlideCaptureRect(const RECT& r) {
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

// Copy a sub-region of a bitmap into a new bitmap.
HBITMAP TmSlideCrop(HBITMAP src, int x, int y, int w, int h) {
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
bool TmSlideClientInfo(HWND root, POINT* originScreen, int* clientW, int* clientH) {
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
double TmSlideChangedFraction(HBITMAP a, HBITMAP b, int w, int h) {
    if (!a || !b || w <= 0 || h <= 0) return 1.0;

    BITMAPINFO bi;
    ZeroMemory(&bi, sizeof(bi));
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = w;
    bi.bmiHeader.biHeight = -h;        // top-down
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
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

// Using UI Automation, find the clicked navigation item (main nav OR a second-level
// list like the Performance sub-pages) and return the screen X of its right edge -
// i.e. where the content next to it begins. Walks a few ancestors so we land on the
// whole item/list, not an inner icon/label.
bool TmSlideNavItemRight(IUIAutomation* uia, POINT pt, int clientLeft, int clientW,
                         int* contentLeftScreen) {
    if (!uia) return false;

    IUIAutomationElement* el = NULL;
    if (FAILED(uia->ElementFromPoint(pt, &el)) || !el) return false;

    IUIAutomationTreeWalker* walker = NULL;
    uia->get_ControlViewWalker(&walker);

    const int leftMax = (int)(clientW * 0.55);
    const int rightMax = (int)(clientW * 0.70);

    int best = -1;
    IUIAutomationElement* cur = el; // owns the ElementFromPoint reference
    for (int i = 0; i < 6 && cur; ++i) {
        RECT r;
        if (SUCCEEDED(cur->get_CurrentBoundingRectangle(&r))) {
            int leftRel = r.left - clientLeft;
            int rightRel = r.right - clientLeft;
            int width = rightRel - leftRel;
            // A nav / sub-nav item or column: starts at the left or in a left column,
            // is much narrower than the content, and ends in the left portion.
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
    if (cur) cur->Release();
    if (walker) walker->Release();

    if (best < 0) return false;
    *contentLeftScreen = clientLeft + best;
    return true;
}

// -------------------------------------------------------------------------
// The slide animation
// -------------------------------------------------------------------------
void TmSlideRun(HBITMAP oldBmp, HBITMAP newBmp, const RECT& rect) {
    int W = rect.right - rect.left;
    int H = rect.bottom - rect.top;
    if (W <= 0 || H <= 0) return;

    const bool vertical = g_slideVertical.load(std::memory_order_relaxed);
    const int span = vertical ? H : W;

    // Topmost, click-through layered overlay sitting exactly over the content area.
    HWND overlay = CreateWindowExW(
        WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_TOPMOST | WS_EX_TRANSPARENT,
        L"STATIC", NULL, WS_POPUP,
        rect.left, rect.top, W, H,
        NULL, NULL, NULL, NULL);
    if (!overlay) return;

    HDC screen = GetDC(NULL);
    HDC oldDC = CreateCompatibleDC(screen);
    HDC newDC = CreateCompatibleDC(screen);
    HDC canvasDC = CreateCompatibleDC(screen);
    HBITMAP canvas = CreateCompatibleBitmap(screen, W, H);
    HBITMAP oldSelO = (HBITMAP)SelectObject(oldDC, oldBmp);
    HBITMAP oldSelN = (HBITMAP)SelectObject(newDC, newBmp);
    HBITMAP oldSelC = (HBITMAP)SelectObject(canvasDC, canvas);

    const double total = (double)g_durationMs.load(std::memory_order_relaxed);

    LARGE_INTEGER freq, start, now;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&start);

    BOOL first = TRUE;
    for (;;) {
        QueryPerformanceCounter(&now);
        double elapsed = (now.QuadPart - start.QuadPart) * 1000.0 / freq.QuadPart;
        BOOL last = (elapsed >= total);
        float p = last ? 1.0f : (float)(elapsed / total);
        float e = p * p * (3.0f - 2.0f * p);   // smoothstep ease in/out

        int oldOff = (int)(-e * span);
        int newOff = (int)((1.0f - e) * span);

        int ox = vertical ? 0 : oldOff;
        int oy = vertical ? oldOff : 0;
        int nx = vertical ? 0 : newOff;
        int ny = vertical ? newOff : 0;

        BitBlt(canvasDC, ox, oy, W, H, oldDC, 0, 0, SRCCOPY);
        BitBlt(canvasDC, nx, ny, W, H, newDC, 0, 0, SRCCOPY);

        POINT ptDst = { rect.left, rect.top };
        SIZE  sz    = { W, H };
        POINT ptSrc = { 0, 0 };
        BLENDFUNCTION bf;
        bf.BlendOp = AC_SRC_OVER;
        bf.BlendFlags = 0;
        bf.SourceConstantAlpha = 255;
        bf.AlphaFormat = 0;
        UpdateLayeredWindow(overlay, screen, &ptDst, &sz, canvasDC, &ptSrc, 0, &bf, ULW_ALPHA);

        if (first) {
            ShowWindow(overlay, SW_SHOWNOACTIVATE);
            first = FALSE;
        }

        if (last) break;
        DwmFlush();
    }

    SelectObject(canvasDC, oldSelC);
    SelectObject(newDC, oldSelN);
    SelectObject(oldDC, oldSelO);
    DeleteObject(canvas);
    DeleteDC(canvasDC);
    DeleteDC(newDC);
    DeleteDC(oldDC);
    ReleaseDC(NULL, screen);
    DestroyWindow(overlay);
}

// Worker: wait for the new view to render, locate the content edge via UI
// Automation, capture the new view, and slide - but only if it actually changed.
DWORD WINAPI TmSlideWorkerThread(LPVOID lpParam) {
    TabSlideData* d = (TabSlideData*)lpParam;

    Sleep(g_captureDelay.load(std::memory_order_relaxed));

    int clientW = d->fullRect.right - d->fullRect.left;
    int contentLeft = d->fullRect.left;
    bool located = false;
    HRESULT hrCo = CoInitializeEx(NULL, COINIT_MULTITHREADED);
    IUIAutomation* uia = NULL;
    if (SUCCEEDED(CoCreateInstance(kCLSID_CUIAutomation, NULL, CLSCTX_INPROC_SERVER,
                                   kIID_IUIAutomation, (void**)&uia)) && uia) {
        located = TmSlideNavItemRight(uia, d->clickPt, d->fullRect.left, clientW, &contentLeft);
        uia->Release();
    }
    if (SUCCEEDED(hrCo)) CoUninitialize();

    if (located) {
        RECT content = { contentLeft, d->fullRect.top, d->fullRect.right, d->fullRect.bottom };
        int cw = content.right - content.left;
        int ch = content.bottom - content.top;
        if (cw >= 80 && ch >= 80) {
            HBITMAP oldC = TmSlideCrop(d->oldFull, contentLeft - d->fullRect.left, 0, cw, ch);
            if (oldC) {
                // Re-capture the new view until it has actually rendered. Some tabs
                // (Disk / Network / GPU) draw their first frame noticeably later than
                // CPU / Memory, so a single fixed delay misses them. We poll until the
                // content visibly changes (or give up), which makes the effect robust
                // to a small capture delay.
                const double readyFrac = 0.06;
                const double userThr = g_changeThreshold.load(std::memory_order_relaxed) / 100.0;
                HBITMAP newC = NULL;
                double frac = 0.0;
                for (int attempt = 0; attempt < 6; ++attempt) {
                    HBITMAP cap = TmSlideCaptureRect(content);
                    if (!cap) break;
                    if (newC) DeleteObject(newC);
                    newC = cap;
                    frac = TmSlideChangedFraction(oldC, newC, cw, ch);
                    if (frac >= readyFrac) break;  // new view has rendered
                    Sleep(35);
                }
                if (newC && frac >= userThr) {
                    TmSlideRun(oldC, newC, content);
                }
                if (newC) DeleteObject(newC);
                DeleteObject(oldC);
            }
        }
    }

    DeleteObject(d->oldFull);
    delete d;
    g_inProgress.store(false, std::memory_order_relaxed);
    return 0;
}

// -------------------------------------------------------------------------
// Click detection (low-level mouse hook)
// -------------------------------------------------------------------------
void TmSlideOnClick(WPARAM msg, POINT pt) {
    if (msg == WM_LBUTTONUP) {
        std::lock_guard<std::mutex> lock(g_mutex);
        if (!g_armed) return;
        g_armed = false;
        if (!g_pendingOld) return;
        if (g_inProgress.load(std::memory_order_relaxed)) {
            DeleteObject(g_pendingOld);
            g_pendingOld = NULL;
            return;
        }

        g_inProgress.store(true, std::memory_order_relaxed);
        TabSlideData* d = new TabSlideData();
        d->oldFull = g_pendingOld;
        d->clickPt = g_pendingClick;
        d->fullRect = g_pendingRect;
        g_pendingOld = NULL;

        HANDLE h = CreateThread(NULL, 0, TmSlideWorkerThread, d, 0, NULL);
        if (h) {
            CloseHandle(h);
        } else {
            DeleteObject(d->oldFull);
            delete d;
            g_inProgress.store(false, std::memory_order_relaxed);
        }
        return;
    }

    // msg == WM_LBUTTONDOWN: capture the current view now (before the click changes
    // it) if the click is plausibly in a left navigation column. The nav/content
    // boundary and whether anything actually changed are resolved on the worker.
    if (g_inProgress.load(std::memory_order_relaxed)) return;

    HWND under = WindowFromPoint(pt);
    HWND root = under ? GetAncestor(under, GA_ROOT) : NULL;
    if (!root) return;

    DWORD pid = 0;
    GetWindowThreadProcessId(root, &pid);
    if (pid != GetCurrentProcessId()) return;

    POINT origin;
    int cw, ch;
    if (!TmSlideClientInfo(root, &origin, &cw, &ch)) return;
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

    HBITMAP old = TmSlideCaptureRect(full);
    std::lock_guard<std::mutex> lock(g_mutex);
    if (g_pendingOld) DeleteObject(g_pendingOld);
    g_pendingOld = old;
    g_pendingClick = pt;
    g_pendingRect = full;
    g_armed = (old != NULL);
}

LRESULT CALLBACK TmSlideMouseProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION && (wParam == WM_LBUTTONDOWN || wParam == WM_LBUTTONUP)) {
        MSLLHOOKSTRUCT* ms = (MSLLHOOKSTRUCT*)lParam;
        TmSlideOnClick(wParam, ms->pt);
    }
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

// Dedicated thread owning the hook and pumping messages (required for LL hooks).
DWORD WINAPI TmSlideHookThread(LPVOID lpParam) {
    g_mouseHook = SetWindowsHookExW(WH_MOUSE_LL, TmSlideMouseProc, GetModuleHandleW(NULL), 0);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

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
    TmSlideLoadSettings();
    g_hookThread = CreateThread(NULL, 0, TmSlideHookThread, NULL, 0, &g_hookThreadId);
    return TRUE;
}

void Wh_ModSettingsChanged() {
    TmSlideLoadSettings();
}

void Wh_ModUninit() {
    if (g_hookThread) {
        if (g_hookThreadId) {
            PostThreadMessageW(g_hookThreadId, WM_QUIT, 0, 0);
        }
        WaitForSingleObject(g_hookThread, 2000);
        CloseHandle(g_hookThread);
        g_hookThread = NULL;
        g_hookThreadId = 0;
    }

    std::lock_guard<std::mutex> lock(g_mutex);
    if (g_pendingOld) {
        DeleteObject(g_pendingOld);
        g_pendingOld = NULL;
    }
}
