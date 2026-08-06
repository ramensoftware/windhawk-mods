// ==WindhawkMod==
// @id              fluid-zoom
// @name            Fluid-Zoom
// @description     Smooth fullscreen zoom for Windows using Alt + Scroll or touchpad pinch. Stay fully interactive while zoomed. Inspired by the macOS zoom experience.
// @version         1.0
// @author          Gaurav Nehra
// @github          https://github.com/gauravrocks009/Windhawk-Fluid-Zoom
// @include         explorer.exe
// @license MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Fluid-Zoom

A smooth, system-wide screen magnifier for Windows with touchpad pinch gestures and mouse wheel zoom. It brings macOS zoom capabilities to Windows.

![Fluid Zoom Demo](https://raw.githubusercontent.com/gauravrocks009/Fluid-Zoom/main/images/demo.gif)

## Features
- System-wide zooming across all Windows applications.
- Native touchpad pinch gesture support.
- Mouse wheel zoom support.
- Hardware-accelerated rendering using the Windows Magnification API.
- Cursor-centered zoom that follows your mouse.
- Fully interactive while zoomed (click, type, drag, scroll normally).

## Controls
| Action | Shortcut |
|----------|-----------|
| Zoom In | Hold Modifier Key + Pinch In / Mouse Wheel Up |
| Zoom Out | Hold Modifier Key + Pinch Out / Mouse Wheel Down |
| Reset Zoom | `Esc` |

## Settings Explained

**Modifier Key**
The keyboard key required to activate the zoom controls. Options include Alt or Shift.

**Zoom Speed (%)**
Controls how much the zoom level changes with each pinch gesture or mouse wheel step. Lower values provide finer control, while higher values zoom faster.

**Max Zoom Level**
Limits how far the screen can be magnified.

**Responsiveness**
Controls the animation speed of the zoom transitions. Lower values (e.g., 5) create longer, more cinematic transitions. Higher values (e.g., 100) shorten the animation duration, making zooming feel instantaneous. 
*Note: Values near 100 may cause higher CPU usage spikes.*

**Smooth Edges**
Uses Windows' built-in anti-aliasing to smooth jagged edges while zoomed.
- **0 (Disabled):** Sharper pixels and crisp text. Ideal for coding and reading.
- **1 (Enabled):** Smoother visuals. Better for images, videos, and presentations.

**Integer Zoom Only**
Restricts the zoom to whole-number magnification levels (e.g., 2x, 3x, 4x) for precise pixel scaling.

## Standalone Application
I have also converted this mod into a standalone application for Windows. It is extremely lightweight and uses less than 50MBs of RAM!
Here is the link for it: 
[Fluid-Zoom Standalone](https://github.com/gauravrocks009/Fluid-Zoom/)

![Fluid Zoom Settings](https://raw.githubusercontent.com/gauravrocks009/Fluid-Zoom/main/images/settings.png)

## Support the Project
If you like this mod, please star it on GitHub!

If you experience any issues, please open a ticket here: [GitHub Issues](https://github.com/gauravrocks009/Windhawk-Fluid-Zoom/issues)

Check out other amazing projects: [gauravrocks009 on GitHub](https://github.com/gauravrocks009)
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- modifierKey: alt
  $name: Modifier Key
  $description: Hold this key while scrolling to zoom
  $options:
  - alt: Alt
  - shift: Shift
- zoomStep: 15
  $name: Zoom Speed (%)
  $description: How much each scroll notch zooms (5 = subtle, 100 = dramatic)
- maxZoom: 20
  $name: Max Zoom Level
  $description: Maximum zoom multiplier (2 - 50)
- animSpeed: 45
  $name: Responsiveness
  $description: "Animation speed (5 = cinematic, 100 = instant). Warning: Values near 100 may cause higher CPU usage spikes."
- smoothEdges: 0
  $name: Smooth Edges (0 or 1)
  $description: "0 = Sharp/Pixelated (Best for coding). 1 = Soft/Blurry (Best for media). Must be exactly 0 or 1."
- integerSnap: 0
  $name: Integer Zoom Only
  $description: Lock to whole-number zoom levels (2x, 3x, 4x)
  $options:
  - 0: Off (smooth continuous zoom)
  - 1: On (snaps to integers)
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <math.h>

// ============================================================
//  Magnification API (dynamically loaded)
// ============================================================

typedef BOOL (WINAPI *FnMagInit)(void);
typedef BOOL (WINAPI *FnMagUninit)(void);
typedef BOOL (WINAPI *FnMagSetFST)(float, int, int);
typedef BOOL (WINAPI *FnMagSetSmoothing)(BOOL);

static HMODULE   g_hMagDll  = NULL;
static FnMagInit g_MagInit  = NULL;
static FnMagUninit g_MagUninit = NULL;
static FnMagSetFST g_MagSetFST = NULL;
static FnMagSetSmoothing g_MagSetSmoothing = NULL;
static bool g_bMagReady = false;

static bool LoadMagnificationAPI() {
    g_hMagDll = LoadLibraryW(L"Magnification.dll");
    if (!g_hMagDll) return false;
    g_MagInit    = (FnMagInit)   GetProcAddress(g_hMagDll, "MagInitialize");
    g_MagUninit  = (FnMagUninit) GetProcAddress(g_hMagDll, "MagUninitialize");
    g_MagSetFST  = (FnMagSetFST) GetProcAddress(g_hMagDll, "MagSetFullscreenTransform");
    g_MagSetSmoothing = (FnMagSetSmoothing) GetProcAddress(g_hMagDll, "MagSetFullscreenUseBitmapSmoothing");
    if (!g_MagInit || !g_MagUninit || !g_MagSetFST) {
        FreeLibrary(g_hMagDll); g_hMagDll = NULL; return false;
    }
    return true;
}

// ============================================================
//  Zoom state
// ============================================================

static float  g_zCur = 1.0f;       // Currently displayed zoom
static float  g_zTgt = 1.0f;       // Target zoom (lerping toward this)
static POINT  g_ptLast = {-1,-1};  // Last rendered cursor position
static float  g_zLast = 0.0f;      // Last rendered zoom level

#define ZEPS 0.003f

// ============================================================
//  Handles
// ============================================================

static HWND     g_hOverlay   = NULL;  // Invisible overlay for touchpad scroll capture
static HANDLE   g_hThread    = NULL;
static DWORD    g_dwThreadId = 0;
static UINT_PTR g_uAnimTimer = 0;     // Timer ID 1: zoom animation
static HHOOK    g_hKbdHook   = NULL;
static HHOOK    g_hMouseHook = NULL;

// ============================================================
//  Input state
// ============================================================

static volatile bool g_bModHeld       = false;  // Alt/Shift held?
static volatile bool g_bOverlayShown  = false;  // Is overlay currently visible?
static volatile bool g_bPinchCtrl     = false;  // Ctrl is held AND was INJECTED (= pinch gesture)
static volatile bool g_bInjecting     = false;  // We're injecting a click (avoid hook loop)

// ============================================================
//  Settings
// ============================================================

static int   g_iModVK   = VK_MENU;
static float g_fStep    = 1.15f;
static float g_fMaxZoom = 20.0f;
static float g_fLerp    = 0.45f;
static int   g_iSmooth  = 0;
static int   g_iSnap    = 0;

// ============================================================
//  Registry: bitmap smoothing for Magnification API
// ============================================================

static void SetBitmapSmoothing(int on) {
    HKEY hk = NULL;
    if (RegCreateKeyExW(HKEY_CURRENT_USER, L"Software\\Microsoft\\ScreenMagnifier",
        0, NULL, 0, KEY_SET_VALUE, NULL, &hk, NULL) == ERROR_SUCCESS) {
        DWORD v = on ? 1 : 0;
        RegSetValueExW(hk, L"UseBitmapSmoothing", 0, REG_DWORD, (BYTE*)&v, sizeof(v));
        RegCloseKey(hk);
    }
}

// ============================================================
//  Core zoom logic
// ============================================================

static void AnimTick(); // Forward declaration

static void ApplyZoomDelta(short delta) {
    float t = g_zTgt;
    if (g_iSnap) {
        t = (delta > 0) ? (floorf(t) + 1.0f) : (ceilf(t) - 1.0f);
    } else {
        float n = (float)delta / 120.0f;
        // Boost small touchpad deltas
        int ad = delta < 0 ? -delta : delta;
        if (ad > 0 && ad < 120) n *= 2.5f;
        t *= powf(g_fStep, n);
    }
    if (t < 1.0f)       t = 1.0f;
    if (t > g_fMaxZoom)  t = g_fMaxZoom;
    g_zTgt = t;

    // Start animation timer
    if (!g_uAnimTimer && g_hOverlay) {
        g_uAnimTimer = SetTimer(g_hOverlay, 1, 16, NULL);
        AnimTick(); // Force immediate update to eliminate 16ms lag
    }
}

static void AnimTick() {
    // Lerp
    float diff = g_zTgt - g_zCur;
    if (fabsf(diff) > ZEPS) g_zCur += diff * g_fLerp;
    else                    g_zCur = g_zTgt;

    // Fully unzoomed → stop
    if (g_zCur < 1.0f + ZEPS) {
        g_zCur = 1.0f; g_zTgt = 1.0f;
        if (g_bMagReady) g_MagSetFST(1.0f, 0, 0);
        if (g_uAnimTimer) { KillTimer(g_hOverlay, 1); g_uAnimTimer = 0; }
        g_ptLast.x = -1; g_zLast = 0;
        return;
    }

    POINT c; GetCursorPos(&c);

    // Skip redundant renders
    bool zc = fabsf(g_zCur - g_zLast) > 0.001f;
    int th = (int)(g_zCur * 2.5f); if (th < 2) th = 2;
    if (!zc && abs(c.x - g_ptLast.x) < th && abs(c.y - g_ptLast.y) < th) return;

    g_ptLast = c; g_zLast = g_zCur;

    float f = 1.0f - 1.0f / g_zCur;
    float ox = (float)c.x * f;
    float oy = (float)c.y * f;

    int vx = GetSystemMetrics(SM_XVIRTUALSCREEN);
    int vy = GetSystemMetrics(SM_YVIRTUALSCREEN);
    int vw = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    int vh = GetSystemMetrics(SM_CYVIRTUALSCREEN);

    float minX = (float)vx * f;
    float maxX = (float)(vx + vw) * f;
    float minY = (float)vy * f;
    float maxY = (float)(vy + vh) * f;

    if (ox < minX) ox = minX; if (ox > maxX) ox = maxX;
    if (oy < minY) oy = minY; if (oy > maxY) oy = maxY;

    if (g_bMagReady) g_MagSetFST(g_zCur, (int)ox, (int)oy);
}

// ============================================================
//  Overlay show/hide helpers
// ============================================================

static void ShowOverlay() {
    if (g_bOverlayShown || !g_hOverlay) return;
    g_bOverlayShown = true;
    SetLayeredWindowAttributes(g_hOverlay, 0, 1, LWA_ALPHA);
}

static void HideOverlay() {
    if (!g_bOverlayShown || !g_hOverlay) return;
    g_bOverlayShown = false;
    SetLayeredWindowAttributes(g_hOverlay, 0, 0, LWA_ALPHA);
}

// ============================================================
//  Low-Level Keyboard Hook
// ============================================================

LRESULT CALLBACK LLKbdProc(int nCode, WPARAM wp, LPARAM lp) {
    if (nCode == HC_ACTION) {
        KBDLLHOOKSTRUCT *k = (KBDLLHOOKSTRUCT*)lp;
        bool down = (wp == WM_KEYDOWN || wp == WM_SYSKEYDOWN);

        // ---- Track our modifier key (Alt/Shift) ----
        bool matchMod = false;
        if (g_iModVK == VK_MENU)
            matchMod = (k->vkCode == VK_MENU || k->vkCode == VK_LMENU || k->vkCode == VK_RMENU);
        else if (g_iModVK == VK_SHIFT)
            matchMod = (k->vkCode == VK_SHIFT || k->vkCode == VK_LSHIFT || k->vkCode == VK_RSHIFT);

        if (matchMod) {
            if (down && !g_bModHeld) {
                g_bModHeld = true;
                // Show the invisible overlay to capture touchpad scroll
                if (g_hOverlay) PostMessageW(g_hOverlay, WM_APP + 1, 1, 0);
            } else if (!down && g_bModHeld) {
                g_bModHeld = false;
                // Hide overlay — clicking works normally again
                if (g_hOverlay) PostMessageW(g_hOverlay, WM_APP + 1, 0, 0);
            }
        }

        // ---- Detect INJECTED Ctrl key (= touchpad pinch gesture) ----
        bool matchCtrl = (k->vkCode == VK_CONTROL || k->vkCode == VK_LCONTROL || k->vkCode == VK_RCONTROL);
        if (matchCtrl) {
            bool injected = (k->flags & LLKHF_INJECTED) != 0;
            if (down && injected) {
                g_bPinchCtrl = true;
            } else if (!down) {
                g_bPinchCtrl = false;
            }
        }

        // ---- Escape = instant zoom reset ----
        if (k->vkCode == VK_ESCAPE && down && g_zTgt > 1.0f + ZEPS) {
            g_zTgt = 1.0f;
            if (!g_uAnimTimer && g_hOverlay)
                g_uAnimTimer = SetTimer(g_hOverlay, 1, 16, NULL);
        }
    }
    return CallNextHookEx(NULL, nCode, wp, lp);
}

// ============================================================
//  Low-Level Mouse Hook
// ============================================================

LRESULT CALLBACK LLMouseProc(int nCode, WPARAM wp, LPARAM lp) {
    if (nCode == HC_ACTION) {
        // Skip our own injected events
        if (g_bInjecting) return CallNextHookEx(NULL, nCode, wp, lp);

        if (wp == WM_MOUSEWHEEL || wp == 0x020E) {
            MSLLHOOKSTRUCT *m = (MSLLHOOKSTRUCT*)lp;
            short delta = (short)HIWORD(m->mouseData);

            // Case 1: Modifier held + scroll → zoom
            if (g_bModHeld) {
                ApplyZoomDelta(delta);
                return 1; // Consume
            }

            // Case 2: Pinch-to-zoom (injected Ctrl + scroll)
            // Only intercept if Ctrl was INJECTED by the touchpad, not physically pressed
            if (g_bPinchCtrl) {
                ApplyZoomDelta(delta);
                return 1; // Consume
            }
        }

        // Keep panning timer alive while zoomed
        if (wp == WM_MOUSEMOVE && g_zCur > 1.0f + ZEPS) {
            if (!g_uAnimTimer && g_hOverlay)
                g_uAnimTimer = SetTimer(g_hOverlay, 1, 16, NULL);
        }
    }
    return CallNextHookEx(NULL, nCode, wp, lp);
}

// ============================================================
//  Overlay WndProc
// ============================================================

#define WM_APP_OVERLAY (WM_APP + 1)  // wParam: 1=show, 0=hide

LRESULT CALLBACK OverlayWndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {

    // ---- Show/Hide commands from keyboard hook ----
    if (msg == WM_APP_OVERLAY) {
        if (wp) ShowOverlay();
        else    HideOverlay();
        return 0;
    }

    // ---- Prevent activation ----
    if (msg == WM_MOUSEACTIVATE) return MA_NOACTIVATE;

    // ---- Screen resolution change ----
    if (msg == WM_DISPLAYCHANGE) {
        int x = GetSystemMetrics(SM_XVIRTUALSCREEN);
        int y = GetSystemMetrics(SM_YVIRTUALSCREEN);
        int w = GetSystemMetrics(SM_CXVIRTUALSCREEN);
        int h = GetSystemMetrics(SM_CYVIRTUALSCREEN);
        SetWindowPos(hwnd, NULL, x, y, w, h, SWP_NOZORDER | SWP_NOACTIVATE);
        return 0;
    }

    // ---- Touchpad scroll → zoom ----
    // This is the KEY handler. Touchpad two-finger scroll arrives here
    // because the overlay is visible and covers the screen while Alt is held.
    if (msg == WM_MOUSEWHEEL || msg == 0x020E) {
        short delta = GET_WHEEL_DELTA_WPARAM(wp);
        if (g_bModHeld) {
            ApplyZoomDelta(delta);
        } else {
            // Modifier released but we got a scroll (race condition).
            // Forward it to the app underneath.
            HideOverlay();
            g_bInjecting = true;
            INPUT in = {0};
            in.type = INPUT_MOUSE;
            in.mi.mouseData = (DWORD)GET_WHEEL_DELTA_WPARAM(wp);
            in.mi.dwFlags = (msg == WM_MOUSEWHEEL) ? MOUSEEVENTF_WHEEL : MOUSEEVENTF_HWHEEL;
            SendInput(1, &in, sizeof(INPUT));
            g_bInjecting = false;
        }
        return 0;
    }

    // ---- Click forwarding ----
    // When user clicks while overlay is visible (Alt held), we:
    // 1. Hide the overlay immediately
    // 2. Inject the click so it reaches the real app underneath
    // 3. Re-show overlay after a short delay if Alt is still held
    if (msg == WM_LBUTTONDOWN || msg == WM_RBUTTONDOWN || msg == WM_MBUTTONDOWN ||
        msg == WM_LBUTTONDBLCLK || msg == WM_RBUTTONDBLCLK) {
        HideOverlay();

        DWORD flag = 0;
        if (msg == WM_LBUTTONDOWN || msg == WM_LBUTTONDBLCLK) flag = MOUSEEVENTF_LEFTDOWN;
        else if (msg == WM_RBUTTONDOWN || msg == WM_RBUTTONDBLCLK) flag = MOUSEEVENTF_RIGHTDOWN;
        else flag = MOUSEEVENTF_MIDDLEDOWN;

        g_bInjecting = true;
        INPUT in = {0};
        in.type = INPUT_MOUSE;
        in.mi.dwFlags = flag;
        SendInput(1, &in, sizeof(INPUT));
        g_bInjecting = false;

        // Re-show overlay after 200ms if Alt still held
        SetTimer(hwnd, 2, 200, NULL);
        return 0;
    }

    if (msg == WM_LBUTTONUP || msg == WM_RBUTTONUP || msg == WM_MBUTTONUP) {
        HideOverlay();

        DWORD flag = 0;
        if (msg == WM_LBUTTONUP)   flag = MOUSEEVENTF_LEFTUP;
        else if (msg == WM_RBUTTONUP)  flag = MOUSEEVENTF_RIGHTUP;
        else flag = MOUSEEVENTF_MIDDLEUP;

        g_bInjecting = true;
        INPUT in = {0};
        in.type = INPUT_MOUSE;
        in.mi.dwFlags = flag;
        SendInput(1, &in, sizeof(INPUT));
        g_bInjecting = false;

        SetTimer(hwnd, 2, 200, NULL);
        return 0;
    }

    // ---- Animation timer ----
    if (msg == WM_TIMER) {
        if (wp == 1) { AnimTick(); return 0; }
        if (wp == 2) {
            KillTimer(hwnd, 2);
            // Re-show overlay if modifier still held
            if (g_bModHeld && !g_bOverlayShown) ShowOverlay();
            return 0;
        }
    }

    return DefWindowProcW(hwnd, msg, wp, lp);
}

// ============================================================
//  Background thread
// ============================================================

static DWORD WINAPI ZoomThread(LPVOID) {
    // 1. Load Magnification API
    if (!LoadMagnificationAPI() || !g_MagInit()) return 1;
    g_bMagReady = true;

    // Apply undocumented anti-aliasing filter if available
    if (g_MagSetSmoothing) {
        g_MagSetSmoothing(g_iSmooth ? TRUE : FALSE);
    }

    // Pre-warm the API to prevent first-zoom delay hitch
    g_MagSetFST(1.001f, 0, 0);
    g_MagSetFST(1.0f, 0, 0);

    // 2. Register window class
    WNDCLASSW wc = {0};
    wc.lpfnWndProc  = OverlayWndProc;
    wc.hInstance     = GetModuleHandle(NULL);
    wc.lpszClassName = L"ScreenZoom_Overlay_v21";
    wc.hCursor       = LoadCursorW(NULL, IDC_ARROW);
    RegisterClassW(&wc);

    // 3. Create the overlay window
    //    - WS_EX_LAYERED: allows SetLayeredWindowAttributes for transparency
    //    - WS_EX_TOPMOST: stays above all windows
    //    - WS_EX_TOOLWINDOW: doesn't appear in taskbar/Alt+Tab
    //    - WS_EX_NOACTIVATE: never steals keyboard focus
    int sx = GetSystemMetrics(SM_XVIRTUALSCREEN);
    int sy = GetSystemMetrics(SM_YVIRTUALSCREEN);
    int sw = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    int sh = GetSystemMetrics(SM_CYVIRTUALSCREEN);

    g_hOverlay = CreateWindowExW(
        WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
        wc.lpszClassName, NULL, WS_POPUP,
        sx, sy, sw, sh,   // Start full screen
        NULL, NULL, wc.hInstance, NULL);

    // 4. Start fully transparent (alpha=0), so mouse passes through
    SetLayeredWindowAttributes(g_hOverlay, 0, 0, LWA_ALPHA);
    ShowWindow(g_hOverlay, SW_SHOW);

    // 5. Install system-wide hooks
    HINSTANCE hInst = GetModuleHandle(NULL);
    g_hKbdHook   = SetWindowsHookExW(WH_KEYBOARD_LL, LLKbdProc,   hInst, 0);
    g_hMouseHook = SetWindowsHookExW(WH_MOUSE_LL,    LLMouseProc, hInst, 0);

    // 6. Message loop
    MSG msg;
    while (GetMessageW(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    // 7. Cleanup
    if (g_uAnimTimer) KillTimer(g_hOverlay, 1);
    if (g_hKbdHook)   UnhookWindowsHookEx(g_hKbdHook);
    if (g_hMouseHook) UnhookWindowsHookEx(g_hMouseHook);
    if (g_hOverlay)   DestroyWindow(g_hOverlay);
    UnregisterClassW(wc.lpszClassName, wc.hInstance);

    if (g_bMagReady) { g_MagSetFST(1.0f, 0, 0); g_MagUninit(); }
    if (g_hMagDll) FreeLibrary(g_hMagDll);

    g_hOverlay = NULL; g_hKbdHook = NULL; g_hMouseHook = NULL; g_bMagReady = false;
    return 0;
}

// ============================================================
//  Settings
// ============================================================

static void LoadSettings() {
    PCWSTR k = Wh_GetStringSetting(L"modifierKey");
    if (k) {
        if (!wcscmp(k, L"shift")) g_iModVK = VK_SHIFT;
        else                      g_iModVK = VK_MENU;
        Wh_FreeStringSetting(k);
    }
    int v;
    v = Wh_GetIntSetting(L"zoomStep");
    g_fStep = (v >= 5 && v <= 100) ? (1.0f + (float)v / 100.0f) : 1.15f;

    v = Wh_GetIntSetting(L"maxZoom");
    g_fMaxZoom = (v >= 2 && v <= 50) ? (float)v : 20.0f;

    v = Wh_GetIntSetting(L"animSpeed");
    g_fLerp = (v >= 5 && v <= 100) ? ((float)v / 100.0f) : 0.45f;

    g_iSmooth = Wh_GetIntSetting(L"smoothEdges");
    if (g_iSmooth != 0 && g_iSmooth != 1) g_iSmooth = 1;

    g_iSnap = Wh_GetIntSetting(L"integerSnap");
    if (g_iSnap != 0 && g_iSnap != 1) g_iSnap = 0;
}

// ============================================================
//  Windhawk entry points
// ============================================================

BOOL Wh_ModInit() {
    LoadSettings();
    SetBitmapSmoothing(g_iSmooth);
    g_hThread = CreateThread(NULL, 0, ZoomThread, NULL, 0, &g_dwThreadId);
    return g_hThread ? TRUE : FALSE;
}

void Wh_ModUninit() {
    if (g_dwThreadId) {
        PostThreadMessage(g_dwThreadId, WM_QUIT, 0, 0);
        if (WaitForSingleObject(g_hThread, 3000) == WAIT_TIMEOUT)
            TerminateThread(g_hThread, 0);
        CloseHandle(g_hThread);
        g_dwThreadId = 0; g_hThread = NULL;
    }
}

void Wh_ModSettingsChanged() {
    LoadSettings();
    SetBitmapSmoothing(g_iSmooth);
    Wh_ModUninit();
    g_zCur = 1.0f; g_zTgt = 1.0f;
    g_hThread = CreateThread(NULL, 0, ZoomThread, NULL, 0, &g_dwThreadId);
}
