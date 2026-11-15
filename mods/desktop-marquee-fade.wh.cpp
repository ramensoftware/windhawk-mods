// ==WindhawkMod==
// @id              desktop-marquee-fade
// @name            Desktop Marquee Fade
// @description     Adds a smooth fade-out animation to the desktop selection rectangle when you release the mouse button.
// @version         1.0.0
// @author          angel_lov
// @github          https://github.com/Twilight0Sparkle
// @include         explorer.exe
// @compilerOptions -luser32 -lgdi32
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*

# Desktop Marquee Fade

Adds a smooth fade animation to the desktop selection rectangle.
Instead of vanishing instantly when you release the mouse button, the
selection area dissolves gracefully — making the Windows desktop feel
more polished and modern.

![preview](https://i.imgur.com/oH4fh6O.gif)

---

## Key Features

* **Smooth Fade Effect** — The selection rectangle fades out instead of
  disappearing instantly.
* **Highly Customizable** — Adjust animation speed and duration via mod
  settings.
* **Smart Auto-Opacity** — Automatically calculates the starting opacity to
  match your speed and duration so the animation always looks natural.
* **Smart Interrupt** — If another window comes to the foreground while the
  animation is playing, the mod speeds it up 4x to stay out of your way.
* **Single Instance Mode** — Prevents overlapping animations; only one
  selection rectangle fades at a time.
* **Custom Fade Colors** — Override the fill and border colors used by the
  fade overlay. Only the fade overlay is affected — your system-wide
  selection colors are never touched.

## How to use

1. Click and drag on the desktop to draw a selection rectangle.
2. Release the mouse button — the rectangle fades out smoothly.

## Configuration

| Setting | Default | Description |
|---|---|---|
| Animation Speed (%) | 100 | Multiplier for how fast the animation runs (50–500). |
| Animation Duration (ms) | 300 | Total time the fade lasts (100–2000 ms). |
| Auto-calculate Opacity | Off | Scales starting opacity automatically to match speed/duration. |
| Manual Initial Opacity | 90 | Starting alpha (1–255) when auto-calculate is off. |
| Accelerate if Covered | On | Speeds up the fade 4× whenever another window is in the foreground. |
| Single Instance Only | Off | Destroys any previous fade when a new drag starts. |
| Use Custom Colors | Off | Overrides the fade overlay's fill and border colors with your custom hex values. The live drag rectangle is **not** affected. |
| Custom Fill Color | 8C8C8C | Hex color for the overlay fill. |
| Custom Border Color | B4B4B4 | Hex color for the overlay border. |

*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- speed: 100
  $name: Animation Speed (%)
  $description: Animation speed in percent (50-500). Default is 100.
- duration: 300
  $name: Animation Duration (ms)
  $description: Animation duration in milliseconds (100-2000). Default is 300.
- initial_alpha: 90
  $name: Manual Initial Opacity
  $description: Starting opacity (1-255). Only used if Auto-calculate Opacity is disabled. Default is 90.
- auto_calc_opacity: false
  $name: Auto-calculate Opacity
  $description: Automatically sets starting opacity based on speed and duration settings.
- accelerate_if_covered: true
  $name: Accelerate if covered
  $description: Speeds up the fade 4x whenever another window is in the foreground.
- single_instance: false
  $name: Single Instance Only
  $description: If ON, only one fading rectangle can be active at a time.
- use_custom_color: false
  $name: Use Custom Colors
  $description: Overrides the fade overlay's fill and border colors. The live drag rectangle keeps its normal system color.
- custom_fill_color: "8C8C8C"
  $name: Custom Fill Color (HEX)
  $description: Hex fill color for the fade overlay (e.g. 783838).
- custom_border_color: "B4B4B4"
  $name: Custom Border Color (HEX)
  $description: Hex border color for the fade overlay (e.g. FF8F8F).
*/
// ==/WindhawkModSettings==

#include <map>
#include <atomic>
#include <algorithm>
#include <cmath>
#define NOMINMAX
#include <windows.h>
#include <commctrl.h>
#include <windhawk_api.h>
#include <windhawk_utils.h>

struct AnimPayload {
    RECT rc;
};

std::atomic<int>  g_settingSpeed{100};
std::atomic<int>  g_settingDuration{300};
std::atomic<int>  g_settingInitialAlpha{90};
std::atomic<bool> g_settingAutoCalcAlpha{false};
std::atomic<bool> g_settingAccelerateIfCovered{true};
std::atomic<bool> g_settingSingleInstance{false};
std::atomic<bool> g_settingUseCustomColor{false};
std::atomic<DWORD> g_settingCustomFillColor{0x8C8C8C};
std::atomic<DWORD> g_settingCustomBorderColor{0xB4B4B4};

HWND   g_hManagerWnd = NULL;
HHOOK  g_hMouseHook  = NULL;
HANDLE g_hThread     = NULL;
DWORD  g_dwThreadId  = 0;

POINT g_dragStart = {0};
std::atomic<bool> g_isDragging{false};
std::atomic<bool> g_isStartPointValid{false};

HWND g_hCachedDesktopTopLevel  = NULL;
HWND g_hCachedSysListView32    = NULL;

#define TIMER_ID_FADE        1
#define WM_START_ANIMATION   (WM_USER + 1)
#define WM_PRECHECK_START    (WM_USER + 2)

// The animation border starts at this alpha rather than fully opaque.
// The live drag marquee is visually softer than a flat 255 border, so
// starting fully solid produced a visible "pop" at mouse-up. 190 keeps
// the handoff seamless.
constexpr int kAnimBorderStartAlpha = 190;

struct AnimState {
    float alpha;
    float decrement;
};
std::map<HWND, AnimState> g_animations;

// ---------------------------------------------------------------------------
// Color helpers
// ---------------------------------------------------------------------------

COLORREF GetTargetFillColor() {
    if (g_settingUseCustomColor.load()) {
        DWORD hex = g_settingCustomFillColor.load();
        return RGB((hex >> 16) & 0xFF, (hex >> 8) & 0xFF, hex & 0xFF);
    }
    return GetSysColor(COLOR_HIGHLIGHT);
}

COLORREF GetTargetBorderColor() {
    if (g_settingUseCustomColor.load()) {
        DWORD hex = g_settingCustomBorderColor.load();
        return RGB((hex >> 16) & 0xFF, (hex >> 8) & 0xFF, hex & 0xFF);
    }
    return GetSysColor(COLOR_HIGHLIGHT);
}

// ---------------------------------------------------------------------------
// Settings
// ---------------------------------------------------------------------------

int CalculateSmartAlpha(int speed, int duration) {
    // Perceptually what matters is how long the marquee actually stays visible:
    //   actualDuration = duration * 100 / speed
    // A short flash needs more initial punch; a long linger looks better dimmer.
    int actualDuration = (duration * 100) / speed;
    if (actualDuration < 16) actualDuration = 16;

    // Anchor: at the defaults (speed=100, duration=300) this returns ~90,
    // matching the manual default so toggling auto-calc changes nothing.
    const double kRefMs    = 300.0;
    const double kRefAlpha = 90.0;
    double scale      = std::sqrt(kRefMs / (double)actualDuration);
    double calculated = kRefAlpha * scale;
    return std::max(30, std::min(150, (int)lround(calculated)));
}

DWORD ParseHexColor(PCWSTR hexStr) {
    if (!hexStr) return 0;
    const wchar_t* p = hexStr;
    while (*p == L' ' || *p == L'\t') p++;
    if      (p[0] == L'0' && (p[1] == L'x' || p[1] == L'X')) p += 2;
    else if (p[0] == L'#') p += 1;
    return wcstoul(p, nullptr, 16);
}

DWORD ParseHexColorOrDefault(PCWSTR hexStr, DWORD defaultValue) {
    // Wh_GetStringSetting returns L"" (not NULL) for unset fields; an empty
    // string would silently parse to 0 (black) without this guard.
    if (!hexStr || hexStr[0] == L'\0') return defaultValue;
    return ParseHexColor(hexStr);
}

void LoadSettings() {
    g_settingSpeed.store(
        std::max(50, std::min(500, Wh_GetIntSetting(L"speed"))));
    g_settingDuration.store(
        std::max(100, std::min(2000, Wh_GetIntSetting(L"duration"))));
    g_settingInitialAlpha.store(
        std::max(1, std::min(255, Wh_GetIntSetting(L"initial_alpha"))));
    g_settingAutoCalcAlpha.store(
        Wh_GetIntSetting(L"auto_calc_opacity") != 0);
    g_settingAccelerateIfCovered.store(
        Wh_GetIntSetting(L"accelerate_if_covered") != 0);
    g_settingSingleInstance.store(
        Wh_GetIntSetting(L"single_instance") != 0);
    g_settingUseCustomColor.store(
        Wh_GetIntSetting(L"use_custom_color") != 0);

    // RAII: auto-frees the string on scope exit.
    WindhawkUtils::StringSetting fillStr =
        WindhawkUtils::StringSetting::make(L"custom_fill_color");
    g_settingCustomFillColor.store(
        ParseHexColorOrDefault(fillStr.get(), 0x8C8C8C));

    WindhawkUtils::StringSetting borderStr =
        WindhawkUtils::StringSetting::make(L"custom_border_color");
    g_settingCustomBorderColor.store(
        ParseHexColorOrDefault(borderStr.get(), 0xB4B4B4));
}

// ---------------------------------------------------------------------------
// Painting
// ---------------------------------------------------------------------------

void PaintMarqueeDIB(HWND hwnd, int width, int height,
                     int alphaBorder, int alphaFill, int borderThickness) {
    HDC hdcScreen = GetDC(NULL);
    HDC hdcMem    = CreateCompatibleDC(hdcScreen);

    BITMAPINFO bmi        = {0};
    bmi.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth       = width;
    bmi.bmiHeader.biHeight      = -height;  // top-down
    bmi.bmiHeader.biPlanes      = 1;
    bmi.bmiHeader.biBitCount    = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    void*   pBits     = nullptr;
    HBITMAP hBitmap   = CreateDIBSection(hdcMem, &bmi, DIB_RGB_COLORS,
                                         &pBits, NULL, 0);
    HBITMAP hOldBitmap = (HBITMAP)SelectObject(hdcMem, hBitmap);

    if (pBits) {
        COLORREF fillCol   = GetTargetFillColor();
        COLORREF borderCol = GetTargetBorderColor();

        BYTE fillR = GetRValue(fillCol),   fillG = GetGValue(fillCol),   fillB = GetBValue(fillCol);
        BYTE borR  = GetRValue(borderCol), borG  = GetGValue(borderCol), borB  = GetBValue(borderCol);

        // Clamp border thickness so a tiny rect never becomes all border.
        int bt    = borderThickness;
        int maxBt = std::max(1, std::min(width, height) / 2);
        bt = std::max(1, std::min(bt, maxBt));

        DWORD* pixels = (DWORD*)pBits;
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                bool isBorder = (x < bt || x >= width - bt ||
                                 y < bt || y >= height - bt);
                if (isBorder) {
                    BYTE r = (borR * alphaBorder) / 255;
                    BYTE g = (borG * alphaBorder) / 255;
                    BYTE b = (borB * alphaBorder) / 255;
                    pixels[y * width + x] =
                        ((DWORD)alphaBorder << 24) | (r << 16) | (g << 8) | b;
                } else {
                    BYTE r = (fillR * alphaFill) / 255;
                    BYTE g = (fillG * alphaFill) / 255;
                    BYTE b = (fillB * alphaFill) / 255;
                    pixels[y * width + x] =
                        ((DWORD)alphaFill << 24) | (r << 16) | (g << 8) | b;
                }
            }
        }

        POINT        ptSrc  = {0, 0};
        SIZE         szWnd  = {width, height};
        BLENDFUNCTION blend = {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
        UpdateLayeredWindow(hwnd, hdcScreen, NULL, &szWnd,
                            hdcMem, &ptSrc, 0, &blend, ULW_ALPHA);
    }

    SelectObject(hdcMem, hOldBitmap);
    DeleteObject(hBitmap);
    DeleteDC(hdcMem);
    ReleaseDC(NULL, hdcScreen);
}

// ---------------------------------------------------------------------------
// Overlay window — one per active animation
// ---------------------------------------------------------------------------

LRESULT CALLBACK OverlayWndProc(HWND hwnd, UINT msg,
                                 WPARAM wParam, LPARAM lParam) {
    if (msg == WM_TIMER && wParam == TIMER_ID_FADE) {
        auto it = g_animations.find(hwnd);
        if (it != g_animations.end()) {
            AnimState& st = it->second;

            if (g_settingAccelerateIfCovered.load()) {
                HWND fg = GetForegroundWindow();
                if (fg) {
                    HWND hRoot = GetAncestor(fg, GA_ROOT);
                    wchar_t cls[64];
                    if (GetClassNameW(hRoot, cls, 64) &&
                        wcscmp(cls, L"Progman") != 0 &&
                        wcscmp(cls, L"WorkerW") != 0) {
                        float fastDec = 255.0f / 5.0f;
                        if (st.decrement < fastDec) st.decrement = fastDec;
                    }
                }
            }

            st.alpha -= st.decrement;
            if (st.alpha <= 0.0f) {
                KillTimer(hwnd, TIMER_ID_FADE);
                g_animations.erase(it);
                DestroyWindow(hwnd);
            } else {
                BLENDFUNCTION blend = {AC_SRC_OVER, 0, (BYTE)st.alpha, AC_SRC_ALPHA};
                UpdateLayeredWindow(hwnd, NULL, NULL, NULL, NULL, NULL, 0, &blend, ULW_ALPHA);
            }
        }
        return 0;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

// ---------------------------------------------------------------------------
// Desktop list-view discovery
// ---------------------------------------------------------------------------

HWND GetRealDesktopListView(HWND* outDesktopTopLevel) {
    HWND hProgman      = FindWindowW(L"Progman", L"Program Manager");
    HWND hDesktopDefView =
        FindWindowExW(hProgman, NULL, L"SHELLDLL_DefView", NULL);

    if (hDesktopDefView) {
        if (outDesktopTopLevel) *outDesktopTopLevel = hProgman;
    } else {
        HWND hWorkerW = NULL;
        while ((hWorkerW = FindWindowExW(NULL, hWorkerW,
                                        L"WorkerW", NULL)) != NULL) {
            hDesktopDefView =
                FindWindowExW(hWorkerW, NULL, L"SHELLDLL_DefView", NULL);
            if (hDesktopDefView) {
                if (outDesktopTopLevel) *outDesktopTopLevel = hWorkerW;
                break;
            }
        }
    }
    if (!hDesktopDefView) return NULL;
    return FindWindowExW(hDesktopDefView, NULL, L"SysListView32", L"FolderView");
}

// ---------------------------------------------------------------------------
// Manager window — receives posted messages from the hook thread
// ---------------------------------------------------------------------------

LRESULT CALLBACK ManagerWndProc(HWND hwnd, UINT msg,
                                 WPARAM wParam, LPARAM lParam) {
    if (msg == WM_PRECHECK_START) {
        POINT* ppt = (POINT*)lParam;

        g_hCachedSysListView32 =
            GetRealDesktopListView(&g_hCachedDesktopTopLevel);
        bool isValid = false;

        if (g_hCachedSysListView32 && g_hCachedDesktopTopLevel) {
            RECT rcDesktop;
            GetWindowRect(g_hCachedSysListView32, &rcDesktop);

            if (PtInRect(&rcDesktop, *ppt)) {
                // rcDesktop spans the whole screen regardless of covering
                // windows, so confirm the desktop is actually topmost here.
                HWND hwndAt     = WindowFromPoint(*ppt);
                HWND hRootAt    = hwndAt ? GetAncestor(hwndAt, GA_ROOT) : NULL;

                if (hRootAt == g_hCachedDesktopTopLevel) {
                    DWORD sysListViewPid = 0;
                    GetWindowThreadProcessId(g_hCachedSysListView32,
                                             &sysListViewPid);

                    // Only the explorer.exe that owns the desktop SysListView32
                    // should spawn an overlay. Any other instance must bail so
                    // it doesn't produce a duplicate animation for the same drag.
                    // In-process LVM_HITTEST via SendMessage is also only reliable
                    // from the owning process.
                    if (sysListViewPid != GetCurrentProcessId()) {
                        // Not our SysListView32 — skip entirely.
                        isValid = false;
                    } else {
                        LVHITTESTINFO lvhti;
                        ZeroMemory(&lvhti, sizeof(lvhti));
                        lvhti.pt = *ppt;
                        ScreenToClient(g_hCachedSysListView32, &lvhti.pt);
                        DWORD_PTR dwResult = 0;

                        bool isOnIcon = false;
                        if (SendMessageTimeoutW(g_hCachedSysListView32,
                                                LVM_HITTEST, 0, (LPARAM)&lvhti,
                                                SMTO_ABORTIFHUNG | SMTO_NORMAL,
                                                50, &dwResult)) {
                            isOnIcon = ((int)dwResult != -1);
                        }
                        isValid = !isOnIcon;
                    }
                }
            }
        }

        g_isStartPointValid.store(isValid);
        delete ppt;
        return 0;
    }

    if (msg == WM_START_ANIMATION) {
        AnimPayload* payload = (AnimPayload*)lParam;

        if (g_settingSingleInstance.load()) {
            for (auto const& pair : g_animations) {
                KillTimer(pair.first, TIMER_ID_FADE);
                DestroyWindow(pair.first);
            }
            g_animations.clear();
        }

        HWND hOverlay = CreateWindowExW(
            WS_EX_LAYERED | WS_EX_TRANSPARENT |
            WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
            L"FadeMarqueeOverlayClass", NULL, WS_POPUP,
            payload->rc.left, payload->rc.top,
            payload->rc.right  - payload->rc.left,
            payload->rc.bottom - payload->rc.top,
            NULL, NULL, GetModuleHandle(NULL), NULL);

        if (hOverlay) {
            int speed    = g_settingSpeed.load();
            int duration = g_settingDuration.load();
            int targetFillAlpha = g_settingAutoCalcAlpha.load()
                ? CalculateSmartAlpha(speed, duration)
                : g_settingInitialAlpha.load();

            int actualDuration = (duration * 100) / speed;
            if (actualDuration < 16) actualDuration = 16;

            // DPI-scale the border thickness for consistent appearance on
            // mixed-DPI multi-monitor setups.
            UINT dpi = GetDpiForWindow(hOverlay);
            if (dpi == 0) dpi = 96;
            int borderThickness = std::max(1, (int)lround((double)dpi / 96.0));

            int w = payload->rc.right  - payload->rc.left;
            int h = payload->rc.bottom - payload->rc.top;

            AnimState st;
            st.alpha      = 255.0f;
            st.decrement  = 255.0f / ((float)actualDuration / 16.0f);

            g_animations[hOverlay] = st;
            
            // Build the bitmap exactly ONCE
            PaintMarqueeDIB(hOverlay, w, h,
                            kAnimBorderStartAlpha, targetFillAlpha,
                            borderThickness);

            // Insert just above whatever window sits directly above the desktop
            // so the overlay is never drawn on top of real windows.
            HWND hAbove = GetWindow(g_hCachedDesktopTopLevel, GW_HWNDPREV);
            SetWindowPos(hOverlay,
                         hAbove ? hAbove : HWND_TOP,
                         0, 0, 0, 0,
                         SWP_NOMOVE | SWP_NOSIZE |
                         SWP_NOACTIVATE | SWP_SHOWWINDOW);

            SetTimer(hOverlay, TIMER_ID_FADE, 16, NULL);
        }

        delete payload;
        return 0;
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}

// ---------------------------------------------------------------------------
// Low-level mouse hook — runs in the worker thread's message loop
// ---------------------------------------------------------------------------

LRESULT CALLBACK MouseHookProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        MSLLHOOKSTRUCT* pMouse = (MSLLHOOKSTRUCT*)lParam;

        if (wParam == WM_LBUTTONDOWN) {
            g_dragStart = pMouse->pt;
            g_isDragging.store(true);
            g_isStartPointValid.store(false);

            // Asynchronous pre-check: post the click point to the manager
            // window for desktop-ownership and icon-hit validation.
            POINT* ppt = new POINT{pMouse->pt.x, pMouse->pt.y};
            PostMessageW(g_hManagerWnd, WM_PRECHECK_START, 0, (LPARAM)ppt);

        } else if (wParam == WM_LBUTTONUP && g_isDragging.load()) {
            g_isDragging.store(false);

            if (g_isStartPointValid.load()) {
                int w = abs((int)pMouse->pt.x - (int)g_dragStart.x);
                int h = abs((int)pMouse->pt.y - (int)g_dragStart.y);

                if (w > 10 && h > 10) {
                    int left = std::min((int)g_dragStart.x, (int)pMouse->pt.x);
                    int top  = std::min((int)g_dragStart.y, (int)pMouse->pt.y);

                    AnimPayload* payload = new AnimPayload;
                    payload->rc = {left, top, left + w, top + h};
                    PostMessageW(g_hManagerWnd, WM_START_ANIMATION,
                                 0, (LPARAM)payload);
                }
            }
        }
    }
    return CallNextHookEx(g_hMouseHook, nCode, wParam, lParam);
}

// ---------------------------------------------------------------------------
// Worker thread — owns the message loop, the hook, and the overlay windows
// ---------------------------------------------------------------------------

DWORD WINAPI WorkerThreadProc(LPVOID) {
    WNDCLASSW wcm = {0, ManagerWndProc, 0, 0, GetModuleHandle(NULL),
                     NULL, NULL, NULL, NULL, L"FadeMarqueeManagerClass"};
    RegisterClassW(&wcm);

    WNDCLASSW wco = {0, OverlayWndProc, 0, 0, GetModuleHandle(NULL),
                     NULL, NULL, NULL, NULL, L"FadeMarqueeOverlayClass"};
    RegisterClassW(&wco);

    g_hManagerWnd = CreateWindowExW(0, L"FadeMarqueeManagerClass", NULL, 0,
                                    0, 0, 0, 0, HWND_MESSAGE, NULL,
                                    GetModuleHandle(NULL), NULL);
    g_hMouseHook  = SetWindowsHookExW(WH_MOUSE_LL, MouseHookProc,
                                      GetModuleHandle(NULL), 0);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        DispatchMessage(&msg);
    }

    // Drain any unprocessed posted messages to free their heap payloads.
    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
        if (msg.message == WM_START_ANIMATION && msg.lParam)
            delete (AnimPayload*)msg.lParam;
        else if (msg.message == WM_PRECHECK_START && msg.lParam)
            delete (POINT*)msg.lParam;
    }

    // Tear down all live overlays.
    for (auto const& pair : g_animations) {
        KillTimer(pair.first, TIMER_ID_FADE);
        DestroyWindow(pair.first);
    }
    g_animations.clear();

    if (g_hMouseHook)  UnhookWindowsHookEx(g_hMouseHook);
    if (g_hManagerWnd) DestroyWindow(g_hManagerWnd);

    UnregisterClassW(L"FadeMarqueeManagerClass", GetModuleHandle(NULL));
    UnregisterClassW(L"FadeMarqueeOverlayClass", GetModuleHandle(NULL));
    return 0;
}

// ---------------------------------------------------------------------------
// Windhawk entry points
// ---------------------------------------------------------------------------

BOOL Wh_ModInit() {
    // Avoid installing the global hook in instances launched as /factory or -Embedding
    PCWSTR cmdLine = GetCommandLineW();
    if (cmdLine && (wcsstr(cmdLine, L"/factory") || wcsstr(cmdLine, L"-Embedding"))) {
        return FALSE;
    }

    // Ensure we only install the hook on the shell explorer instance
    HWND hShell = GetShellWindow();
    if (hShell) {
        DWORD shellPid = 0;
        GetWindowThreadProcessId(hShell, &shellPid);
        if (shellPid != 0 && shellPid != GetCurrentProcessId()) {
            return FALSE;
        }
    }

    LoadSettings();
    g_hThread = CreateThread(NULL, 0, WorkerThreadProc, NULL, 0, &g_dwThreadId);
    return g_hThread != NULL;
}

void Wh_ModSettingsChanged() {
    LoadSettings();
}

void Wh_ModUninit() {
    if (g_dwThreadId)
        PostThreadMessageW(g_dwThreadId, WM_QUIT, 0, 0);
    if (g_hThread) {
        // Wait unconditionally: the teardown is quick, and a timed-out wait
        // would unload the DLL while the worker thread is still executing
        // WndProc code that lives in it — a guaranteed crash.
        WaitForSingleObject(g_hThread, INFINITE);
        CloseHandle(g_hThread);
    }
}
