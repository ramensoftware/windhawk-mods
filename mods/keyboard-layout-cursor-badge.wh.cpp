// ==WindhawkMod==
// @id              keyboard-layout-cursor-badge
// @name            Keyboard Layout Badge at Cursor
// @description     Shows the active keyboard layout beside the pointer after it pauses over supported text input areas.
// @version         5.0
// @author          divedooberman-dotcom
// @github          https://github.com/divedooberman-dotcom
// @license         MIT
// @include         windhawk.exe
// @compilerOptions -ld2d1 -ldwrite -lole32 -lgdi32 -lshell32 -lshcore
// ==/WindhawkMod==

// Authorship and provenance:
// This mod is based on Cursor Motion Blur by TheatriChris:
// https://github.com/chrisc44890/windhawk-mods/blob/main/mods/cursor-motion-blur.wh.cpp
//
// Except for the credited Cursor Motion Blur base, the keyboard-layout
// functionality and subsequent modifications were generated entirely by
// ChatGPT/Codex under the direction of divedooberman-dotcom.

// ==WindhawkModReadme==
/*
# Keyboard Layout Badge at Cursor

Displays a small rounded badge with the active keyboard language code
(EN, AR, RU, and others) beside the mouse pointer.

The badge is shown only when the pointer uses a supported text-input cursor.
By default, it waits until the pointer has been stationary for one second and
then appears after the keyboard layout changes. The badge hides while the
pointer is moving or a mouse button is held.

### Features

* **Keyboard layout indicator:** Displays the active input language as a
  two-letter uppercase code.
* **Text-input cursor detection:** Keeps the badge hidden outside supported
  text-input areas.
* **Configurable movement delay:** Controls how long the pointer must remain
  stationary before the badge can appear.
* **Windows shell support:** Keeps the overlay visible over Start and Search
  when the required Windows Z-band API is available.
* **DPI-aware rendering:** Scales the badge for different monitors.
* **Low-load rendering:** Uses a small Direct2D surface and adaptive refresh
  intervals to reduce CPU, GPU, and memory usage.
* **Mouse-button suppression:** Hides the badge while any mouse button is held.

### Modes

* **Flash on change (default):** when the pointer is over text input, the badge
  appears whenever the active layout changes, then fades.
* **Always show:** set *Always Show* to keep the badge next to the pointer while
  it is over text input. This composites the overlay continuously in that state,
  which uses more GPU than the flash mode.

### Compatibility

The mod recognizes the standard Windows I-beam pointer. Some applications use
custom document surfaces or custom text cursors and might not be detected.

### Credits and AI authorship

This mod is based on
[Cursor Motion Blur](https://github.com/chrisc44890/windhawk-mods/blob/main/mods/cursor-motion-blur.wh.cpp)
by TheatriChris (`chrisc44890`), published under the MIT license.

Except for the credited Cursor Motion Blur base, the keyboard-layout
functionality and subsequent modifications were generated entirely by
ChatGPT/Codex under the direction of `divedooberman-dotcom`.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- always_show: false
  $name: Always Show
  $description: Keep the badge visible while the pointer is over text input instead of only flashing on layout changes. Uses more GPU.
- movement_stop_delay_ms: 1000
  $name: Delay After Movement Stops (ms)
  $description: How long the pointer must remain stationary before the language badge can appear. Set to 0 for no delay.
- display_ms: 1200
  $name: Flash Duration (ms)
  $description: How long the badge stays after a layout change (ignored when Always Show is on).
- offset_x: 18
  $name: Offset X
  $description: Horizontal distance of the badge from the pointer, in pixels (at 100% scale).
- offset_y: 20
  $name: Offset Y
  $description: Vertical distance of the badge from the pointer, in pixels (at 100% scale).
- font_size: 15
  $name: Font Size
  $description: Text size of the language code, in pixels (at 100% scale).
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <d2d1.h>
#include <dwrite.h>
#include <shellscalingapi.h>
#include <math.h>
#include <wchar.h>

// windows.h #defines DrawText -> DrawTextW, which collides with the Direct2D
// ID2D1RenderTarget::DrawText method. Drop the macro so we can call the method.
#undef DrawText

// Global state
HWND g_overlayHwnd = NULL;
HANDLE g_threadHandle = NULL;
HHOOK g_mouseHook = NULL;
volatile LONG g_mouseButtonsDown = 0;
volatile LONG g_lastMouseMoveTick = 0;
bool g_overlayWindowVisible = false;
DWORD g_renderGeneration = 1;
constexpr UINT WM_APP_SETTINGS_CHANGED = WM_APP + 1;
constexpr UINT WM_APP_HIDE_FOR_MOUSE_PRESS = WM_APP + 2;

constexpr LONG MOUSE_BUTTON_LEFT   = 1 << 0;
constexpr LONG MOUSE_BUTTON_RIGHT  = 1 << 1;
constexpr LONG MOUSE_BUTTON_MIDDLE = 1 << 2;
constexpr LONG MOUSE_BUTTON_X1     = 1 << 3;
constexpr LONG MOUSE_BUTTON_X2     = 1 << 4;

// Undocumented but exported by user32. The official Windhawk mods use this
// API for windows that must participate in the immersive shell's Z-order.
using CreateWindowInBand_t = HWND(WINAPI*)(
    DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName,
    DWORD dwStyle, int x, int y, int width, int height, HWND parent,
    HMENU menu, HINSTANCE instance, LPVOID param, DWORD band);
constexpr DWORD ZBID_SYSTEM_TOOLS = 16;

// Direct2D / DirectWrite
ID2D1Factory* g_pD2DFactory = nullptr;
IDWriteFactory* g_pDWriteFactory = nullptr;
ID2D1DCRenderTarget* g_pDCRenderTarget = nullptr;
IDWriteTextFormat* g_pTextFormat = nullptr;
float g_textFormatScale = 0.0f;

ID2D1SolidColorBrush* g_pBadgeBg = nullptr;
ID2D1SolidColorBrush* g_pBadgeBorder = nullptr;
ID2D1SolidColorBrush* g_pBadgeText = nullptr;

// Cached backbuffer
HDC g_hdcMem = NULL;
HBITMAP g_hBitmap = NULL;
HBITMAP g_hOldBitmap = NULL;
int g_cachedSurfaceW = 0;
int g_cachedSurfaceH = 0;

// Settings cache
bool  g_alwaysShow = false;
int   g_movementStopDelayMs = 1000;
int   g_displayMs = 1200;
int   g_offsetX = 18;
int   g_offsetY = 20;
float g_fontSize = 15.0f;

void LoadSettings() {
    g_alwaysShow = Wh_GetIntSetting(L"always_show") != 0;
    g_movementStopDelayMs =
        Wh_GetIntSetting(L"movement_stop_delay_ms");
    g_displayMs  = Wh_GetIntSetting(L"display_ms");
    g_offsetX    = Wh_GetIntSetting(L"offset_x");
    g_offsetY    = Wh_GetIntSetting(L"offset_y");
    g_fontSize   = (float)Wh_GetIntSetting(L"font_size");

    if (g_movementStopDelayMs < 0)     g_movementStopDelayMs = 1000;
    if (g_movementStopDelayMs > 60000) g_movementStopDelayMs = 60000;
    if (g_displayMs < 200)              g_displayMs = 200;
    if (g_fontSize  < 8.0f)             g_fontSize  = 15.0f;
}

LRESULT CALLBACK OverlayWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == WM_APP_SETTINGS_CHANGED) {
        // DirectWrite text formats are immutable. Reload settings and release
        // the cached format on the overlay thread so the next render rebuilds
        // it with the new font size.
        LoadSettings();
        if (g_pTextFormat) {
            g_pTextFormat->Release();
            g_pTextFormat = nullptr;
        }
        g_textFormatScale = 0.0f;
        ++g_renderGeneration;
        return 0;
    }

    if (msg == WM_APP_HIDE_FOR_MOUSE_PRESS) {
        ShowWindow(hwnd, SW_HIDE);
        g_overlayWindowVisible = false;
        return 0;
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}

void ReleaseBadgeBrushes() {
    if (g_pBadgeText)   { g_pBadgeText->Release();   g_pBadgeText = nullptr; }
    if (g_pBadgeBorder) { g_pBadgeBorder->Release(); g_pBadgeBorder = nullptr; }
    if (g_pBadgeBg)     { g_pBadgeBg->Release();     g_pBadgeBg = nullptr; }
}

void CreateBadgeBrushes() {
    if (!g_pDCRenderTarget) return;
    // Windows 11-style dark chip: translucent dark fill, faint light border, white text.
    g_pDCRenderTarget->CreateSolidColorBrush(D2D1::ColorF(0.10f, 0.11f, 0.13f, 0.92f), &g_pBadgeBg);
    g_pDCRenderTarget->CreateSolidColorBrush(D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.18f), &g_pBadgeBorder);
    g_pDCRenderTarget->CreateSolidColorBrush(D2D1::ColorF(1.0f, 1.0f, 1.0f, 1.0f), &g_pBadgeText);
}

// Resolve the current foreground input language into a 2-letter uppercase code.
void GetCurrentLangCode(wchar_t* out, int outLen, HKL* outHkl) {
    HWND fg = GetForegroundWindow();
    DWORD tid = fg ? GetWindowThreadProcessId(fg, NULL) : GetCurrentThreadId();
    HKL hkl = GetKeyboardLayout(tid);
    if (outHkl) *outHkl = hkl;

    LANGID langId = LOWORD((DWORD_PTR)hkl);
    wchar_t iso[16] = L"";
    if (GetLocaleInfoW(MAKELCID(langId, SORT_DEFAULT), LOCALE_SISO639LANGNAME, iso, 16) > 0) {
        CharUpperBuffW(iso, (DWORD)wcslen(iso));
        lstrcpynW(out, iso, outLen);
    } else {
        lstrcpynW(out, L"?", outLen);
    }
}

bool IsTextInputCursor() {
    CURSORINFO ci = {};
    ci.cbSize = sizeof(ci);
    if (!GetCursorInfo(&ci)) return false;
    if (!(ci.flags & CURSOR_SHOWING)) return false;

    HCURSOR hBeamCursor = LoadCursorW(nullptr, IDC_IBEAM);
    return hBeamCursor && ci.hCursor == hBeamCursor;
}

LRESULT CALLBACK LowLevelMouseProc(int code, WPARAM wParam, LPARAM lParam) {
    if (code == HC_ACTION) {
        LONG downMask = 0;
        LONG upMask = 0;

        switch (wParam) {
            case WM_MOUSEMOVE:
                InterlockedExchange(&g_lastMouseMoveTick,
                                    static_cast<LONG>(GetTickCount()));
                break;
            case WM_LBUTTONDOWN: downMask = MOUSE_BUTTON_LEFT; break;
            case WM_LBUTTONUP:   upMask = MOUSE_BUTTON_LEFT; break;
            case WM_RBUTTONDOWN: downMask = MOUSE_BUTTON_RIGHT; break;
            case WM_RBUTTONUP:   upMask = MOUSE_BUTTON_RIGHT; break;
            case WM_MBUTTONDOWN: downMask = MOUSE_BUTTON_MIDDLE; break;
            case WM_MBUTTONUP:   upMask = MOUSE_BUTTON_MIDDLE; break;
            case WM_XBUTTONDOWN:
            case WM_XBUTTONUP: {
                const MSLLHOOKSTRUCT* mouse =
                    reinterpret_cast<const MSLLHOOKSTRUCT*>(lParam);
                LONG mask = HIWORD(mouse->mouseData) == XBUTTON1
                                ? MOUSE_BUTTON_X1
                                : MOUSE_BUTTON_X2;
                if (wParam == WM_XBUTTONDOWN) downMask = mask;
                else upMask = mask;
                break;
            }
        }

        if (downMask) {
            InterlockedOr(&g_mouseButtonsDown, downMask);
            if (g_overlayHwnd) {
                PostMessage(g_overlayHwnd, WM_APP_HIDE_FOR_MOUSE_PRESS, 0, 0);
            }
        }
        if (upMask) InterlockedAnd(&g_mouseButtonsDown, ~upMask);
    }

    return CallNextHookEx(g_mouseHook, code, wParam, lParam);
}

bool IsMouseButtonPressed() {
    return InterlockedCompareExchange(&g_mouseButtonsDown, 0, 0) != 0 ||
           (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0 ||
           (GetAsyncKeyState(VK_RBUTTON) & 0x8000) != 0 ||
           (GetAsyncKeyState(VK_MBUTTON) & 0x8000) != 0 ||
           (GetAsyncKeyState(VK_XBUTTON1) & 0x8000) != 0 ||
           (GetAsyncKeyState(VK_XBUTTON2) & 0x8000) != 0;
}

bool EnsureBadgeSurface(HDC hdcScreen, int width, int height) {
    if (g_hBitmap && g_hdcMem && g_cachedSurfaceW == width &&
        g_cachedSurfaceH == height) {
        return true;
    }

    ReleaseBadgeBrushes();
    if (g_pDCRenderTarget) {
        g_pDCRenderTarget->Release();
        g_pDCRenderTarget = nullptr;
    }

    if (g_hdcMem && g_hOldBitmap) {
        SelectObject(g_hdcMem, g_hOldBitmap);
        g_hOldBitmap = NULL;
    }
    if (g_hBitmap) {
        DeleteObject(g_hBitmap);
        g_hBitmap = NULL;
    }
    if (!g_hdcMem) {
        g_hdcMem = CreateCompatibleDC(hdcScreen);
    }
    if (!g_hdcMem) return false;

    BITMAPINFO bitmapInfo = {};
    bitmapInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bitmapInfo.bmiHeader.biWidth = width;
    bitmapInfo.bmiHeader.biHeight = -height;
    bitmapInfo.bmiHeader.biPlanes = 1;
    bitmapInfo.bmiHeader.biBitCount = 32;
    bitmapInfo.bmiHeader.biCompression = BI_RGB;

    void* pixels = nullptr;
    g_hBitmap = CreateDIBSection(hdcScreen, &bitmapInfo, DIB_RGB_COLORS,
                                 &pixels, NULL, 0);
    if (!g_hBitmap) return false;

    g_hOldBitmap = static_cast<HBITMAP>(SelectObject(g_hdcMem, g_hBitmap));
    g_cachedSurfaceW = width;
    g_cachedSurfaceH = height;
    return true;
}

VOID CALLBACK LangTimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime) {
    (void)uMsg;
    (void)idEvent;
    static DWORD lastPoll = 0;
    static HKL lastHkl = NULL;
    static HKL lastRenderedHkl = NULL;
    static bool haveLastHkl = false;
    static wchar_t langText[16] = L"";
    static DWORD flashStart = 0;
    static bool flashPending = false;
    static POINT lastRenderedCursor = { LONG_MIN, LONG_MIN };
    static DWORD lastRenderedGeneration = 0;
    static UINT timerInterval = 16;

    auto setTimerInterval = [&](UINT interval) {
        if (timerInterval != interval) {
            SetTimer(hwnd, 1, interval, LangTimerProc);
            timerInterval = interval;
        }
    };

    if (dwTime - lastPoll >= 150) {
        lastPoll = dwTime;
        HKL hkl = NULL;
        GetCurrentLangCode(langText, ARRAYSIZE(langText), &hkl);

        if (!haveLastHkl) {
            lastHkl = hkl;
            haveLastHkl = true;
        } else if (hkl != lastHkl) {
            lastHkl = hkl;
            if (!g_alwaysShow) {
                flashStart = 0;
                flashPending = true;
            }
        }
    }

    bool textInputMode = IsTextInputCursor();
    if (!textInputMode) {
        flashStart = 0;
        flashPending = false;
    }

    DWORD lastMoveTick = static_cast<DWORD>(
        InterlockedCompareExchange(&g_lastMouseMoveTick, 0, 0));
    bool movementDelayElapsed =
        g_movementStopDelayMs == 0 ||
        dwTime - lastMoveTick >= static_cast<DWORD>(g_movementStopDelayMs);

    // If movement interrupts a flash, defer it and give it a fresh full
    // display duration once the cursor has been stationary for the delay.
    if (!movementDelayElapsed && flashStart != 0) {
        flashStart = 0;
        flashPending = true;
    }

    bool canShowBadge = textInputMode && movementDelayElapsed &&
                        !IsMouseButtonPressed();
    if (canShowBadge && !g_alwaysShow && flashPending) {
        flashStart = dwTime ? dwTime : 1;
        flashPending = false;
    }
    bool flashActive = flashStart != 0 &&
                       dwTime - flashStart < static_cast<DWORD>(g_displayMs);
    bool flashing = canShowBadge && flashActive;
    bool render = canShowBadge && (g_alwaysShow || flashing) && langText[0];

    if (!flashActive) flashStart = 0;

    if (!render) {
        if (g_overlayWindowVisible) {
            ShowWindow(hwnd, SW_HIDE);
            g_overlayWindowVisible = false;
        }
        setTimerInterval(100);
        return;
    }

    POINT cursor;
    if (!GetCursorPos(&cursor)) {
        setTimerInterval(100);
        return;
    }

    bool movingRecently = dwTime - lastMoveTick < 120;
    UINT desiredInterval = movingRecently
                               ? 16
                               : (!g_alwaysShow && flashing ? 33 : 100);

    bool cursorMoved = cursor.x != lastRenderedCursor.x ||
                       cursor.y != lastRenderedCursor.y;
    bool needsRedraw = !g_overlayWindowVisible || cursorMoved ||
                       lastRenderedHkl != lastHkl ||
                       lastRenderedGeneration != g_renderGeneration ||
                       (!g_alwaysShow && flashing);
    if (!needsRedraw) {
        setTimerInterval(desiredInterval);
        return;
    }

    float opacity = 0.92f;
    if (!g_alwaysShow && flashing) {
        DWORD age = dwTime - flashStart;
        DWORD duration = static_cast<DWORD>(g_displayMs);
        DWORD fadeOut = duration < 300 ? duration : 300;
        opacity = 1.0f;
        if (age < 120) opacity = static_cast<float>(age) / 120.0f;
        if (age > duration - fadeOut) {
            float fade = static_cast<float>(duration - age) /
                         static_cast<float>(fadeOut);
            if (fade < opacity) opacity = fade;
        }
        if (opacity < 0.0f) opacity = 0.0f;
        if (opacity > 1.0f) opacity = 1.0f;
    }

    UINT dpiX = 96;
    UINT dpiY = 96;
    HMONITOR monitor = MonitorFromPoint(cursor, MONITOR_DEFAULTTONEAREST);
    GetDpiForMonitor(monitor, MDT_EFFECTIVE_DPI, &dpiX, &dpiY);
    float scale = dpiX / 96.0f;
    float badgeWidth = (g_fontSize * 2.0f + 20.0f) * scale;
    float badgeHeight = (g_fontSize + 14.0f) * scale;
    float radius = 8.0f * scale;
    float offsetX = g_offsetX * scale;
    float offsetY = g_offsetY * scale;
    float badgeLeft = static_cast<float>(cursor.x) + offsetX;
    float badgeTop = static_cast<float>(cursor.y) + offsetY;

    MONITORINFO monitorInfo = {};
    monitorInfo.cbSize = sizeof(monitorInfo);
    if (GetMonitorInfo(monitor, &monitorInfo)) {
        if (badgeLeft + badgeWidth > monitorInfo.rcMonitor.right)
            badgeLeft = static_cast<float>(cursor.x) - offsetX - badgeWidth;
        if (badgeTop + badgeHeight > monitorInfo.rcMonitor.bottom)
            badgeTop = static_cast<float>(cursor.y) - offsetY - badgeHeight;
        if (badgeLeft < monitorInfo.rcMonitor.left)
            badgeLeft = static_cast<float>(monitorInfo.rcMonitor.left);
        if (badgeTop < monitorInfo.rcMonitor.top)
            badgeTop = static_cast<float>(monitorInfo.rcMonitor.top);
    }

    constexpr int padding = 3;
    int windowX = static_cast<int>(floorf(badgeLeft)) - padding;
    int windowY = static_cast<int>(floorf(badgeTop)) - padding;
    int surfaceWidth = static_cast<int>(ceilf(badgeWidth)) + padding * 2 + 1;
    int surfaceHeight = static_cast<int>(ceilf(badgeHeight)) + padding * 2 + 1;

    HDC hdcScreen = GetDC(NULL);
    if (!hdcScreen || !EnsureBadgeSurface(hdcScreen, surfaceWidth, surfaceHeight)) {
        if (hdcScreen) ReleaseDC(NULL, hdcScreen);
        setTimerInterval(desiredInterval);
        return;
    }

    if (!g_pDCRenderTarget && g_pD2DFactory) {
        D2D1_RENDER_TARGET_PROPERTIES properties = D2D1::RenderTargetProperties(
            D2D1_RENDER_TARGET_TYPE_DEFAULT,
            D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM,
                              D2D1_ALPHA_MODE_PREMULTIPLIED),
            0, 0, D2D1_RENDER_TARGET_USAGE_NONE,
            D2D1_FEATURE_LEVEL_DEFAULT);
        g_pD2DFactory->CreateDCRenderTarget(&properties, &g_pDCRenderTarget);
        if (g_pDCRenderTarget) CreateBadgeBrushes();
    }

    bool updated = false;
    if (g_pDCRenderTarget) {
        RECT surfaceRect = { 0, 0, surfaceWidth, surfaceHeight };
        if (SUCCEEDED(g_pDCRenderTarget->BindDC(g_hdcMem, &surfaceRect))) {
            if (!g_pTextFormat || fabsf(scale - g_textFormatScale) > 0.01f) {
                if (g_pTextFormat) {
                    g_pTextFormat->Release();
                    g_pTextFormat = nullptr;
                }
                if (g_pDWriteFactory) {
                    g_pDWriteFactory->CreateTextFormat(
                        L"Segoe UI", NULL, DWRITE_FONT_WEIGHT_SEMI_BOLD,
                        DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL,
                        g_fontSize * scale, L"", &g_pTextFormat);
                    if (g_pTextFormat) {
                        g_pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
                        g_pTextFormat->SetParagraphAlignment(
                            DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
                    }
                    g_textFormatScale = scale;
                }
            }

            float drawLeft = padding + badgeLeft - floorf(badgeLeft);
            float drawTop = padding + badgeTop - floorf(badgeTop);
            D2D1_RECT_F textRect = D2D1::RectF(
                drawLeft, drawTop, drawLeft + badgeWidth,
                drawTop + badgeHeight);
            D2D1_ROUNDED_RECT roundedRect = { textRect, radius, radius };

            if (g_pBadgeBg) g_pBadgeBg->SetOpacity(opacity);
            if (g_pBadgeBorder) g_pBadgeBorder->SetOpacity(opacity);
            if (g_pBadgeText) g_pBadgeText->SetOpacity(opacity);

            g_pDCRenderTarget->BeginDraw();
            g_pDCRenderTarget->Clear(
                D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.0f));
            if (g_pBadgeBg)
                g_pDCRenderTarget->FillRoundedRectangle(roundedRect, g_pBadgeBg);
            if (g_pBadgeBorder)
                g_pDCRenderTarget->DrawRoundedRectangle(
                    roundedRect, g_pBadgeBorder, 1.0f * scale);
            if (g_pBadgeText && g_pTextFormat)
                g_pDCRenderTarget->DrawTextW(
                    langText, static_cast<UINT32>(wcslen(langText)),
                    g_pTextFormat, textRect, g_pBadgeText);

            HRESULT drawResult = g_pDCRenderTarget->EndDraw();
            if (drawResult == D2DERR_RECREATE_TARGET) {
                ReleaseBadgeBrushes();
                g_pDCRenderTarget->Release();
                g_pDCRenderTarget = nullptr;
            } else if (SUCCEEDED(drawResult)) {
                BLENDFUNCTION blend = {};
                blend.BlendOp = AC_SRC_OVER;
                blend.SourceConstantAlpha = 255;
                blend.AlphaFormat = AC_SRC_ALPHA;
                POINT windowPosition = { windowX, windowY };
                SIZE windowSize = { surfaceWidth, surfaceHeight };
                POINT sourcePosition = { 0, 0 };
                updated = UpdateLayeredWindow(
                    hwnd, hdcScreen, &windowPosition, &windowSize, g_hdcMem,
                    &sourcePosition, 0, &blend, ULW_ALPHA) != FALSE;
            }
        }
    }

    ReleaseDC(NULL, hdcScreen);

    if (updated) {
        if (!g_overlayWindowVisible) {
            ShowWindow(hwnd, SW_SHOWNA);
            g_overlayWindowVisible = true;
        }
        lastRenderedCursor = cursor;
        lastRenderedHkl = lastHkl;
        lastRenderedGeneration = g_renderGeneration;
    }

    setTimerInterval(desiredInterval);
}

DWORD WINAPI OverlayThreadProc(LPVOID lpParam) {
    (void)lpParam;
    CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

    D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &g_pD2DFactory);
    DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory),
                        reinterpret_cast<IUnknown**>(&g_pDWriteFactory));

    HINSTANCE hInstance = GetModuleHandle(NULL);
    const wchar_t CLASS_NAME[] = L"LangBadgeOverlayClass";

    WNDCLASS wc = { };
    wc.lpfnWndProc = OverlayWndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    RegisterClass(&wc);

    constexpr DWORD overlayExStyle =
        WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOPMOST |
        WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE;

    HMODULE user32Module = GetModuleHandleW(L"user32.dll");
    auto createWindowInBand = user32Module
                                  ? reinterpret_cast<CreateWindowInBand_t>(
                                        GetProcAddress(user32Module,
                                                       "CreateWindowInBand"))
                                  : nullptr;
    if (createWindowInBand) {
        SetLastError(ERROR_SUCCESS);
        g_overlayHwnd = createWindowInBand(
            overlayExStyle, CLASS_NAME, L"LangBadgeOverlay", WS_POPUP,
            0, 0, 1, 1, NULL, NULL, hInstance, NULL, ZBID_SYSTEM_TOOLS);
        if (!g_overlayHwnd) {
            Wh_Log(L"CreateWindowInBand(ZBID_SYSTEM_TOOLS) failed: %u",
                   GetLastError());
        }
    }

    if (!g_overlayHwnd) {
        g_overlayHwnd = CreateWindowExW(
            overlayExStyle, CLASS_NAME, L"LangBadgeOverlay", WS_POPUP,
            0, 0, 1, 1, NULL, NULL, hInstance, NULL);
    }

    if (!g_overlayHwnd) return 0;

    HMODULE hookModule = NULL;
    GetModuleHandleExW(
        GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
            GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
        reinterpret_cast<LPCWSTR>(&LowLevelMouseProc), &hookModule);
    g_mouseHook = SetWindowsHookExW(WH_MOUSE_LL, LowLevelMouseProc, hookModule, 0);
    if (!g_mouseHook) {
        Wh_Log(L"SetWindowsHookExW(WH_MOUSE_LL) failed: %u", GetLastError());
    }

    // Starts fast for initialization, then adapts between 16, 33, and 100 ms.
    SetTimer(g_overlayHwnd, 1, 16, LangTimerProc);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    if (g_mouseHook) {
        UnhookWindowsHookEx(g_mouseHook);
        g_mouseHook = NULL;
    }

    if (g_pTextFormat) { g_pTextFormat->Release(); g_pTextFormat = nullptr; }
    ReleaseBadgeBrushes();
    if (g_pDCRenderTarget) { g_pDCRenderTarget->Release(); g_pDCRenderTarget = nullptr; }
    if (g_pDWriteFactory) { g_pDWriteFactory->Release(); g_pDWriteFactory = nullptr; }
    if (g_pD2DFactory) { g_pD2DFactory->Release(); g_pD2DFactory = nullptr; }

    if (g_hdcMem && g_hOldBitmap) {
        SelectObject(g_hdcMem, g_hOldBitmap);
        g_hOldBitmap = NULL;
    }
    if (g_hBitmap) DeleteObject(g_hBitmap);
    if (g_hdcMem) DeleteDC(g_hdcMem);
    DestroyWindow(g_overlayHwnd);
    UnregisterClass(CLASS_NAME, hInstance);

    CoUninitialize();
    return 0;
}

BOOL WhTool_ModInit() {
    LoadSettings();
    g_threadHandle = CreateThread(NULL, 0, OverlayThreadProc, NULL, 0, NULL);
    return TRUE;
}

void WhTool_ModUninit() {
    if (g_overlayHwnd) {
        PostMessage(g_overlayHwnd, WM_QUIT, 0, 0);
    }
    if (g_threadHandle) {
        WaitForSingleObject(g_threadHandle, INFINITE);
        CloseHandle(g_threadHandle);
    }
}

void WhTool_ModSettingsChanged() {
    if (!g_overlayHwnd ||
        !PostMessage(g_overlayHwnd, WM_APP_SETTINGS_CHANGED, 0, 0)) {
        LoadSettings();
    }
}

////////////////////////////////////////////////////////////////////////////////
// Windhawk tool mod boilerplate (runs the mod in its own dedicated
// windhawk.exe process). See:
// https://github.com/ramensoftware/windhawk/wiki/Mods-as-tools:-Running-mods-in-a-dedicated-process

bool g_isToolModProcessLauncher;
HANDLE g_toolModProcessMutex;

void WINAPI EntryPoint_Hook() {
    Wh_Log(L">");
    ExitThread(0);
}

BOOL Wh_ModInit() {
    DWORD sessionId;
    if (ProcessIdToSessionId(GetCurrentProcessId(), &sessionId) &&
        sessionId == 0) {
        return FALSE;
    }

    bool isExcluded = false;
    bool isToolModProcess = false;
    bool isCurrentToolModProcess = false;
    int argc;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLine(), &argc);
    if (!argv) {
        Wh_Log(L"CommandLineToArgvW failed");
        return FALSE;
    }

    for (int i = 1; i < argc; i++) {
        if (wcscmp(argv[i], L"-service") == 0 ||
            wcscmp(argv[i], L"-service-start") == 0 ||
            wcscmp(argv[i], L"-service-stop") == 0) {
            isExcluded = true;
            break;
        }
    }

    for (int i = 1; i < argc - 1; i++) {
        if (wcscmp(argv[i], L"-tool-mod") == 0) {
            isToolModProcess = true;
            if (wcscmp(argv[i + 1], WH_MOD_ID) == 0) {
                isCurrentToolModProcess = true;
            }
            break;
        }
    }

    LocalFree(argv);

    if (isExcluded) {
        return FALSE;
    }

    if (isCurrentToolModProcess) {
        g_toolModProcessMutex =
            CreateMutex(nullptr, TRUE, L"windhawk-tool-mod_" WH_MOD_ID);
        if (!g_toolModProcessMutex) {
            Wh_Log(L"CreateMutex failed");
            ExitProcess(1);
        }

        if (GetLastError() == ERROR_ALREADY_EXISTS) {
            Wh_Log(L"Tool mod already running (%s)", WH_MOD_ID);
            ExitProcess(1);
        }

        if (!WhTool_ModInit()) {
            ExitProcess(1);
        }

        IMAGE_DOS_HEADER* dosHeader =
            (IMAGE_DOS_HEADER*)GetModuleHandle(nullptr);
        IMAGE_NT_HEADERS* ntHeaders =
            (IMAGE_NT_HEADERS*)((BYTE*)dosHeader + dosHeader->e_lfanew);

        DWORD entryPointRVA = ntHeaders->OptionalHeader.AddressOfEntryPoint;
        void* entryPoint = (BYTE*)dosHeader + entryPointRVA;

        Wh_SetFunctionHook(entryPoint, (void*)EntryPoint_Hook, nullptr);
        return TRUE;
    }

    if (isToolModProcess) {
        return FALSE;
    }

    g_isToolModProcessLauncher = true;
    return TRUE;
}

void Wh_ModAfterInit() {
    if (!g_isToolModProcessLauncher) {
        return;
    }

    WCHAR currentProcessPath[MAX_PATH];
    switch (GetModuleFileName(nullptr, currentProcessPath,
                              ARRAYSIZE(currentProcessPath))) {
        case 0:
        case ARRAYSIZE(currentProcessPath):
            Wh_Log(L"GetModuleFileName failed");
            return;
    }

    WCHAR
    commandLine[MAX_PATH + 2 +
                (sizeof(L" -tool-mod \"" WH_MOD_ID "\"") / sizeof(WCHAR)) - 1];
    swprintf(commandLine, L"\"%s\" -tool-mod \"%s\"", currentProcessPath,
             WH_MOD_ID);

    HMODULE kernelModule = GetModuleHandle(L"kernelbase.dll");
    if (!kernelModule) {
        kernelModule = GetModuleHandle(L"kernel32.dll");
        if (!kernelModule) {
            Wh_Log(L"No kernelbase.dll/kernel32.dll");
            return;
        }
    }

    using CreateProcessInternalW_t = BOOL(WINAPI*)(
        HANDLE hUserToken, LPCWSTR lpApplicationName, LPWSTR lpCommandLine,
        LPSECURITY_ATTRIBUTES lpProcessAttributes,
        LPSECURITY_ATTRIBUTES lpThreadAttributes, WINBOOL bInheritHandles,
        DWORD dwCreationFlags, LPVOID lpEnvironment, LPCWSTR lpCurrentDirectory,
        LPSTARTUPINFOW lpStartupInfo,
        LPPROCESS_INFORMATION lpProcessInformation,
        PHANDLE hRestrictedUserToken);
    CreateProcessInternalW_t pCreateProcessInternalW =
        (CreateProcessInternalW_t)GetProcAddress(kernelModule,
                                                 "CreateProcessInternalW");
    if (!pCreateProcessInternalW) {
        Wh_Log(L"No CreateProcessInternalW");
        return;
    }

    STARTUPINFO si = {};
    si.cb = sizeof(STARTUPINFO);
    si.dwFlags = STARTF_FORCEOFFFEEDBACK;
    PROCESS_INFORMATION pi;
    if (!pCreateProcessInternalW(nullptr, currentProcessPath, commandLine,
                                 nullptr, nullptr, FALSE, NORMAL_PRIORITY_CLASS,
                                 nullptr, nullptr, &si, &pi, nullptr)) {
        Wh_Log(L"CreateProcess failed");
        return;
    }

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}

void Wh_ModSettingsChanged() {
    if (g_isToolModProcessLauncher) {
        return;
    }

    WhTool_ModSettingsChanged();
}

void Wh_ModUninit() {
    if (g_isToolModProcessLauncher) {
        return;
    }

    WhTool_ModUninit();
    ExitProcess(0);
}
