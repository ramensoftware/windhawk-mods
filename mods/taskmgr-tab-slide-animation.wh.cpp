// ==WindhawkMod==
// @id              taskmgr-tab-slide-animation
// @name            Task Manager Tab Slide Animation
// @description     Smooth slide transition when switching tabs (Processes, Performance, ...) in Task Manager.
// @version         0.2.0
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

The Windows 11 Task Manager switches between its left-hand tabs (Processes,
Performance, App history, Startup apps, Users, Details, Services) instantly. This
mod adds a smooth **slide** transition between them.

> Experimental. Task Manager is a WinUI 3 app, so there's no clean Win32
> "tab changed" signal - this works by heuristics and screen capture.

## How it works
- A low-level mouse hook (scoped to Task Manager) notices a click in the left
  navigation area and captures the content area from the screen (old tab).
- **The content's left edge is detected with UI Automation**: the right edge of
  the clicked navigation item is where the content begins. This adapts
  automatically whether the nav pane is **expanded** (wide, with labels) or
  **collapsed** (icons only) - no manual width setting needed.
- After a short delay it captures the new tab and slides old -> new in a layered
  overlay over the content area, then removes it to reveal the real content.

## Settings
- **Top inset** - height (px) of the top command bar to exclude from the slide.
- **Capture delay** - how long to wait after the click before grabbing the new
  tab (ms). Raise if it grabs the old tab.
- **Animation duration** and **slide vertically** to taste.

## Known limitations
- Relies on UI Automation to locate the content; if UIA can't identify the clicked
  navigation item, that switch just isn't animated.
- Clicking the *same* tab still plays a (harmless) slide of identical content.
- The Task Manager window should be visible/unobscured during the switch.
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
  $description: Delay after the click before capturing the new tab. Raise if it grabs the old tab.
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

// Using UI Automation, find the clicked navigation item and return the screen X of
// its right edge - i.e. where the content area begins. Walks a few ancestors in
// case ElementFromPoint lands on an inner icon/label. Returns false if the point
// isn't on a nav-item-like element (so content clicks don't trigger a slide).
bool TmSlideNavItemRight(IUIAutomation* uia, POINT pt, int clientLeft, int* contentLeftScreen) {
    if (!uia) return false;

    IUIAutomationElement* el = NULL;
    if (FAILED(uia->ElementFromPoint(pt, &el)) || !el) return false;

    IUIAutomationTreeWalker* walker = NULL;
    uia->get_ControlViewWalker(&walker);

    int best = -1;
    IUIAutomationElement* cur = el; // owns the ElementFromPoint reference
    for (int i = 0; i < 5 && cur; ++i) {
        RECT r;
        if (SUCCEEDED(cur->get_CurrentBoundingRectangle(&r))) {
            int leftRel = r.left - clientLeft;
            int rightRel = r.right - clientLeft;
            // Nav items hug the left edge and are at most ~pane-wide.
            if (leftRel < 48 && rightRel >= 32 && rightRel <= 520 && rightRel > best) {
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

    // Time-based progress, gated on the DWM compose cycle so each rendered frame
    // lands on exactly one displayed frame (same trick as the genie mod).
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

        int oldOff = (int)(-e * span);          // outgoing slides off one side
        int newOff = (int)((1.0f - e) * span);  // incoming slides in from the other

        int ox = vertical ? 0 : oldOff;
        int oy = vertical ? oldOff : 0;
        int nx = vertical ? 0 : newOff;
        int ny = vertical ? newOff : 0;

        // Old + new together cover the whole canvas, so no clear is needed.
        BitBlt(canvasDC, ox, oy, W, H, oldDC, 0, 0, SRCCOPY);
        BitBlt(canvasDC, nx, ny, W, H, newDC, 0, 0, SRCCOPY);

        POINT ptDst = { rect.left, rect.top };
        SIZE  sz    = { W, H };
        POINT ptSrc = { 0, 0 };
        BLENDFUNCTION bf;
        bf.BlendOp = AC_SRC_OVER;
        bf.BlendFlags = 0;
        bf.SourceConstantAlpha = 255;  // fully opaque - covers the real content
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

// Worker: wait for the new tab to render, locate the content edge via UI
// Automation, capture the new tab, and run the slide.
DWORD WINAPI TmSlideWorkerThread(LPVOID lpParam) {
    TabSlideData* d = (TabSlideData*)lpParam;

    Sleep(g_captureDelay.load(std::memory_order_relaxed));

    int contentLeft = d->fullRect.left;
    bool located = false;
    HRESULT hrCo = CoInitializeEx(NULL, COINIT_MULTITHREADED);
    IUIAutomation* uia = NULL;
    if (SUCCEEDED(CoCreateInstance(kCLSID_CUIAutomation, NULL, CLSCTX_INPROC_SERVER,
                                   kIID_IUIAutomation, (void**)&uia)) && uia) {
        located = TmSlideNavItemRight(uia, d->clickPt, d->fullRect.left, &contentLeft);
        uia->Release();
    }
    if (SUCCEEDED(hrCo)) CoUninitialize();

    if (located) {
        RECT content = { contentLeft, d->fullRect.top, d->fullRect.right, d->fullRect.bottom };
        int cw = content.right - content.left;
        int ch = content.bottom - content.top;
        if (cw >= 80 && ch >= 80) {
            HBITMAP oldC = TmSlideCrop(d->oldFull, contentLeft - d->fullRect.left, 0, cw, ch);
            HBITMAP newC = TmSlideCaptureRect(content);
            if (oldC && newC) {
                TmSlideRun(oldC, newC, content);
            }
            if (oldC) DeleteObject(oldC);
            if (newC) DeleteObject(newC);
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
        // Fire the slide for a previously-armed nav click.
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

    // msg == WM_LBUTTONDOWN: capture the current tab now (before the click switches
    // it) if the click is plausibly in the left navigation area. The actual nav /
    // content boundary is resolved later, on the worker thread, via UI Automation.
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
    // Below the command bar, and in the left portion where the nav can be.
    if (relX < 0 || relY < topI || relX > cw / 2 || relY >= ch) return;

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
