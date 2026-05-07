// ==WindhawkMod==
// @id              autoscroll-win32
// @name            AutoScroll Win32
// @name:pt         Rolagem Automatica Win32
// @name:es         Desplazamiento Automatico Win32
// @description     Enables browser-style middle-mouse autoscroll in Win32 applications. Compatible with Smooth Scroll Win32.
// @description:pt  Ativa rolagem automatica com o botao do meio (estilo navegador) em aplicativos Win32. Compativel com Smooth Scroll Win32.
// @description:es  Activa el desplazamiento automatico con el boton central del raton (estilo navegador) en aplicaciones Win32. Compatible con Smooth Scroll Win32.
// @version         1.0.0
// @author          crazyboyybs
// @github          https://github.com/crazyboyybs
// @include         explorer.exe
// @compilerOptions -luser32 -lgdi32 -ld2d1 -ladvapi32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*

![Preview](https://raw.githubusercontent.com/crazyboyybs/assets/refs/heads/main/auto%20scroll%20preview.gif)

---

## AutoScroll Win32

Simulates browser-style autoscroll (middle mouse button) in Win32 applications
such as File Explorer, with a WinUI-style visual indicator at the scroll origin.

### How to use

1. **Hold** the **middle mouse button** anywhere in a scrollable area.
2. Move the cursor **up or down** (and optionally sideways) from the origin
   point -- the farther from the origin, the faster the scroll.
3. **Release** the middle button to stop.

### Compatibility with Smooth Scroll Win32

Fully compatible. AutoScroll Win32 posts standard WM_MOUSEWHEEL and
WM_MOUSEHWHEEL messages, which Smooth Scroll Win32 intercepts and animates
exactly like real mouse-wheel events.

### Adding more applications

By default the mod targets only explorer.exe. To enable it in other
applications, add their executable names in the **Advanced** tab of the mod
settings in Windhawk under "Include processes".

---

## AutoScroll Win32 (Portugues)

Simula a rolagem automatica com o botao do meio do mouse (estilo navegador) em
aplicativos Win32 como o Explorador de Arquivos, com um indicador visual
estilo WinUI no ponto de origem da rolagem.

### Como usar

1. **Segure** o **botao do meio do mouse** em qualquer area rolavel.
2. Mova o cursor **para cima ou para baixo** (e opcionalmente para os lados)
   a partir do ponto de origem -- quanto mais longe, mais rapido rola.
3. **Solte** o botao do meio para parar.

### Compatibilidade com Smooth Scroll Win32

Totalmente compativel. O AutoScroll Win32 envia mensagens padrao WM_MOUSEWHEEL
e WM_MOUSEHWHEEL, que o Smooth Scroll Win32 intercepta e anima exatamente como
eventos reais da roda do mouse.

### Adicionar mais aplicativos

Por padrao, o mod afeta apenas o explorer.exe. Para ativa-lo em outros
aplicativos, adicione os nomes dos executaveis na aba **Advanced** das
configuraces do mod no Windhawk em "Include processes".

---

## AutoScroll Win32 (Espanol)

Simula el desplazamiento automatico con el boton central del raton (estilo
navegador) en aplicaciones Win32 como el Explorador de archivos, con un
indicador visual estilo WinUI en el punto de origen del desplazamiento.

### Como usar

1. **Mantena presionado** el **boton central del raton** en cualquier area
   desplazable.
2. Mueve el cursor **hacia arriba o hacia abajo** (y opcionalmente hacia los
   lados) desde el punto de origen -- cuanto mas lejos, mas rapido se desplaza.
3. **Suelta** el boton central para detener.

### Compatibilidad con Smooth Scroll Win32

Totalmente compatible. AutoScroll Win32 envia mensajes estandar WM_MOUSEWHEEL
y WM_MOUSEHWHEEL, que Smooth Scroll Win32 intercepta y anima exactamente como
eventos reales de la rueda del raton.

### Agregar mas aplicaciones

De forma predeterminada, el mod solo afecta a explorer.exe. Para habilitarlo
en otras aplicaciones, agrega los nombres de los ejecutables en la pestana **Advanced**
de la configuracion del mod en Windhawk bajo "Include processes".
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- deadzone: 8
  $name: Dead Zone (px)
  $name:pt: Zona Morta (px)
  $name:es: Zona Muerta (px)
  $description: Distance in pixels from the origin point within which no scrolling occurs.
  $description:pt: Distancia em pixels a partir do ponto de origem dentro da qual a rolagem nao ocorre.
  $description:es: Distancia en pixeles desde el origen dentro de la cual no ocurre desplazamiento.
- speed: 150
  $name: Max Speed Distance (px)
  $name:pt: Distancia de Velocidade Maxima (px)
  $name:es: Distancia de Velocidad Maxima (px)
  $description: Pixel distance from the origin at which maximum scroll speed is reached.
  $description:pt: Distancia em pixels para atingir a velocidade maxima de rolagem.
  $description:es: Distancia en pixeles desde el origen para alcanzar la velocidad maxima.
- horizontal: true
  $name: Horizontal Scroll
  $name:pt: Rolagem Horizontal
  $name:es: Desplazamiento Horizontal
  $description: Enables horizontal scrolling when the cursor is moved sideways.
  $description:pt: Ativa rolagem horizontal ao mover o cursor para os lados.
  $description:es: Activa el desplazamiento horizontal al mover el cursor hacia los lados.
- speedMult: 30
  $name: Max Speed
  $name:pt: Velocidade Maxima
  $name:es: Velocidad Maxima
  $description: Maximum speed in tenths of a wheel notch per frame (30 = 3.0 notches/frame).
  $description:pt: Velocidade maxima em decimos de entalhe por quadro (30 = 3,0 entalhes/quadro).
  $description:es: Velocidad maxima en decimas de muesca por fotograma (30 = 3,0 muescas/fotograma).
- showIndicator: true
  $name: Show Visual Indicator
  $name:pt: Mostrar Indicador Visual
  $name:es: Mostrar Indicador Visual
  $description: Shows the autoscroll indicator circle at the scroll origin.
  $description:pt: Mostra o circulo de indicador de rolagem na origem da rolagem.
  $description:es: Muestra el circulo indicador de desplazamiento en el origen del gesto.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <windowsx.h>
#include <d2d1.h>

#include <algorithm>
#include <atomic>
#include <cstdlib>
#include <thread>

// ===========================================================================
// Settings
// ===========================================================================
static std::atomic<int>  g_deadzonePx{8};
static std::atomic<int>  g_speedPx{150};
static std::atomic<bool> g_horzEnabled{true};
static std::atomic<int>  g_speedMult{30};
static std::atomic<bool> g_showIndicator{true};

// ===========================================================================
// Autoscroll state
// ===========================================================================
static std::atomic<HWND> g_targetHwnd{nullptr};
static std::atomic<int>  g_originX{0};
static std::atomic<int>  g_originY{0};

// Custom messages driving the indicator window from the scroll thread.
// Declared early because ScrollThreadProc and state transitions use them.
static const UINT WM_IND_SHOW   = WM_USER + 1;
static const UINT WM_IND_HIDE   = WM_USER + 2;
static const UINT WM_IND_UPDATE = WM_USER + 3;

// Scroll thread
static std::atomic<bool> g_threadRunning{false};
static std::thread       g_scrollThread;

// Indicator thread
static std::atomic<HWND> g_indicatorHwnd{nullptr};
static std::thread       g_indicatorThread;

// Event signaled by StartAutoscroll to wake the scroll thread from idle.
// Auto-reset: consumed by WaitForSingleObject automatically.
static HANDLE g_scrollEvent = nullptr;

// Set before posting WM_IND_UPDATE; cleared by the handler.
// Prevents the scroll thread from flooding the indicator queue, which would
// delay WM_IND_SHOW/HIDE. Uses relaxed ordering: a missed update just means
// one extra frame of stale arrow directions -- not a correctness issue.
static std::atomic<bool> g_indUpdatePending{false};

// Cached display refresh interval (ms) for the target window's monitor.
// Updated once per gesture in StartAutoscroll -- avoids 3 syscalls per frame.
static std::atomic<DWORD> g_frameMs{16};

// ===========================================================================
// Scroll thread
// ===========================================================================
static void ScrollThreadProc() {
    // Outer loop: sleep until a gesture starts or the mod is unloading.
    while (g_threadRunning.load(std::memory_order_relaxed)) {
        WaitForSingleObject(g_scrollEvent, INFINITE);
        if (!g_threadRunning.load(std::memory_order_relaxed)) break;

        // Inner loop: active while a gesture is ongoing.
        while (g_threadRunning.load(std::memory_order_relaxed)) {
            HWND hwnd = g_targetHwnd.load(std::memory_order_acquire);
            if (!hwnd) break;  // gesture ended

            if (!IsWindow(hwnd)) {
                HWND expected = hwnd;
                g_targetHwnd.compare_exchange_strong(
                    expected, nullptr,
                    std::memory_order_release,
                    std::memory_order_relaxed);
                break;
            }

            DWORD frameMs = g_frameMs.load(std::memory_order_relaxed);

            POINT cursor{};
            GetCursorPos(&cursor);

            int dy = cursor.y - g_originY.load(std::memory_order_relaxed);
            int dx = cursor.x - g_originX.load(std::memory_order_relaxed);

            int   deadzone   = g_deadzonePx.load(std::memory_order_relaxed);
            int   speedRange = std::max(1,
                g_speedPx.load(std::memory_order_relaxed) - deadzone);
            float maxNotches = g_speedMult.load(std::memory_order_relaxed) / 10.0f;

            // Vertical scroll
            {
                int absDy = std::abs(dy);
                if (absDy > deadzone) {
                    float t     = std::min(1.0f, (float)(absDy - deadzone) / speedRange);
                    int   delta = (int)(t * t * t * maxNotches * WHEEL_DELTA);
                    if (delta > 0) {
                        if (dy > 0) delta = -delta;
                        PostMessageW(hwnd, WM_MOUSEWHEEL,
                            MAKEWPARAM(0, (SHORT)delta),
                            MAKELPARAM(cursor.x, cursor.y));
                    }
                }
            }

            // Horizontal scroll
            if (g_horzEnabled.load(std::memory_order_relaxed)) {
                int absDx = std::abs(dx);
                if (absDx > deadzone) {
                    float t     = std::min(1.0f, (float)(absDx - deadzone) / speedRange);
                    int   delta = (int)(t * t * t * maxNotches * WHEEL_DELTA);
                    if (delta > 0) {
                        if (dx > 0) delta = -delta;
                        PostMessageW(hwnd, WM_MOUSEHWHEEL,
                            MAKEWPARAM(0, (SHORT)delta),
                            MAKELPARAM(cursor.x, cursor.y));
                    }
                }
            }

            // Wake indicator to refresh arrow directions.
            // Only post if no WM_IND_UPDATE is already queued (atomic flag),
            // so the indicator queue never accumulates and WM_IND_SHOW/HIDE
            // are always processed promptly.
            HWND ind = g_indicatorHwnd.load(std::memory_order_relaxed);
            if (ind && !g_indUpdatePending.exchange(true, std::memory_order_relaxed)) {
                PostMessageW(ind, WM_IND_UPDATE, 0, 0);
            }

            Sleep(frameMs);
        }  // inner loop
    }  // outer loop
}

// ===========================================================================
// Autoscroll state transitions
// ===========================================================================

static DWORD CalcFrameMs(HWND hwnd) {
    HMONITOR hmon = hwnd
        ? MonitorFromWindow(hwnd, MONITOR_DEFAULTTOPRIMARY)
        : MonitorFromPoint({0, 0}, MONITOR_DEFAULTTOPRIMARY);
    MONITORINFOEXW mi{};
    mi.cbSize = sizeof(mi);
    if (!GetMonitorInfoW(hmon, &mi)) return 16;
    DEVMODEW dm{};
    dm.dmSize = sizeof(dm);
    if (!EnumDisplaySettingsW(mi.szDevice, ENUM_CURRENT_SETTINGS, &dm)
        || dm.dmDisplayFrequency == 0) return 16;
    return std::max((DWORD)1, 1000u / dm.dmDisplayFrequency);
}

static void StartAutoscroll(HWND hwnd, POINT screenOrigin) {
    g_frameMs.store(CalcFrameMs(hwnd), std::memory_order_relaxed);
    g_originX.store(screenOrigin.x, std::memory_order_relaxed);
    g_originY.store(screenOrigin.y, std::memory_order_relaxed);
    g_targetHwnd.store(hwnd, std::memory_order_release);
    if (g_scrollEvent) SetEvent(g_scrollEvent);  // wake scroll thread
    HWND ind = g_indicatorHwnd.load(std::memory_order_acquire);
    if (ind) PostMessageW(ind, WM_IND_SHOW, 0, 0);
}

static void StopAutoscroll() {
    g_targetHwnd.store(nullptr, std::memory_order_release);
    HWND ind = g_indicatorHwnd.load(std::memory_order_acquire);
    if (ind) PostMessageW(ind, WM_IND_HIDE, 0, 0);
}

// ===========================================================================
// Message handler -- shared across all four Get/Peek/Dispatch hooks.
//
// Returns true if the message was consumed and must be suppressed.
// When called from a GetMessage/PeekMessage hook the caller nullifies the
// message; when called from a DispatchMessage hook the caller returns 0.
// The WM_NULL produced by the former prevents double-processing in the latter.
// ===========================================================================
static bool HandleMsg(const MSG* pMsg) {
    if (!pMsg || pMsg->message == WM_NULL) return false;

    const bool isActive = (g_targetHwnd.load(std::memory_order_acquire) != nullptr);

    switch (pMsg->message) {

    // Middle button DOWN: always start; swallow so the app does not see it.
    case WM_MBUTTONDOWN: {
        POINT pt = { GET_X_LPARAM(pMsg->lParam),
                     GET_Y_LPARAM(pMsg->lParam) };
        ClientToScreen(pMsg->hwnd, &pt);
        StartAutoscroll(pMsg->hwnd, pt);
        return true;
    }

    // Middle button UP: stop; swallow to match the DOWN we consumed.
    case WM_MBUTTONUP:
        if (isActive) StopAutoscroll();
        return true;

    // Other buttons while active: cancel but let the click through.
    case WM_LBUTTONDOWN:
    case WM_RBUTTONDOWN:
    case WM_XBUTTONDOWN:
        if (isActive) StopAutoscroll();
        break;

    // Escape: cancel and swallow.
    case WM_KEYDOWN:
        if (isActive && pMsg->wParam == VK_ESCAPE) {
            StopAutoscroll();
            return true;
        }
        break;

    // App deactivation: cancel.
    case WM_ACTIVATEAPP:
        if (isActive && !pMsg->wParam) StopAutoscroll();
        break;

    default:
        break;
    }
    return false;
}

// ===========================================================================
// GetMessageW / GetMessageA hooks
//
// First point of access for any message in a standard Win32 message loop.
// Covering these catches apps that handle messages inline without calling
// DispatchMessage (rare but valid Win32 pattern).
// We nullify consumed messages so the paired DispatchMessage hook is a no-op.
// ===========================================================================
// ===========================================================================
// PeekMessageW / PeekMessageA hooks
//
// Covers PeekMessage-driven loops (game-style, DirectX, WPF/WinForms hosts).
// GetMessageW/A is intentionally NOT hooked: it is a blocking call, so any
// thread suspended inside our hook cannot exit during mod unload, causing
// explorer.exe to show a persistent unloading state in Windhawk.
// Standard GetMessage loops are still covered because every message removed
// from the queue via GetMessage is subsequently dispatched via DispatchMessage,
// which is hooked below.
// Only act when PM_REMOVE is set -- PM_NOREMOVE is a non-destructive peek
// and the message will be seen again.
// ===========================================================================
using PeekMessageW_t = BOOL(WINAPI*)(LPMSG, HWND, UINT, UINT, UINT);
static PeekMessageW_t g_origPeekMessageW = nullptr;

BOOL WINAPI PeekMessageW_Hook(
    LPMSG lpMsg, HWND hWnd, UINT wMin, UINT wMax, UINT wRemoveMsg)
{
    BOOL ret = g_origPeekMessageW(lpMsg, hWnd, wMin, wMax, wRemoveMsg);
    if (ret && lpMsg && (wRemoveMsg & PM_REMOVE) && HandleMsg(lpMsg))
        lpMsg->message = WM_NULL;
    return ret;
}

using PeekMessageA_t = BOOL(WINAPI*)(LPMSG, HWND, UINT, UINT, UINT);
static PeekMessageA_t g_origPeekMessageA = nullptr;

BOOL WINAPI PeekMessageA_Hook(
    LPMSG lpMsg, HWND hWnd, UINT wMin, UINT wMax, UINT wRemoveMsg)
{
    BOOL ret = g_origPeekMessageA(lpMsg, hWnd, wMin, wMax, wRemoveMsg);
    if (ret && lpMsg && (wRemoveMsg & PM_REMOVE) && HandleMsg(lpMsg))
        lpMsg->message = WM_NULL;
    return ret;
}

// ===========================================================================
// DispatchMessageW / DispatchMessageA hooks
//
// Primary interception point for standard GetMessage-based loops:
// GetMessage is not hooked (it blocks, causing persistent unload state in
// Windhawk when targeting explorer.exe), so WM_MBUTTONDOWN is caught here
// after the app dequeues it. Also catches PeekMessage loops where the
// PeekMessage hook has already nullified the message to WM_NULL, in which
// case HandleMsg returns false immediately -- no double-processing.
// ===========================================================================
using DispatchMessageW_t = LRESULT(WINAPI*)(const MSG*);
static DispatchMessageW_t g_origDispatchMessageW = nullptr;

LRESULT WINAPI DispatchMessageW_Hook(const MSG* pMsg) {
    if (!pMsg) return 0;
    if (HandleMsg(pMsg)) return 0;
    return g_origDispatchMessageW(pMsg);
}

using DispatchMessageA_t = LRESULT(WINAPI*)(const MSG*);
static DispatchMessageA_t g_origDispatchMessageA = nullptr;

LRESULT WINAPI DispatchMessageA_Hook(const MSG* pMsg) {
    if (!pMsg) return 0;
    if (HandleMsg(pMsg)) return 0;
    return g_origDispatchMessageA(pMsg);
}


// ===========================================================================
// Indicator
// ===========================================================================

static const int DIR_UP    = 1;
static const int DIR_DOWN  = 2;
static const int DIR_LEFT  = 4;
static const int DIR_RIGHT = 8;

static bool ReadDarkMode() {
    DWORD value  = 1;
    DWORD cbData = sizeof(value);
    RegGetValueW(
        HKEY_CURRENT_USER,
        L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
        L"AppsUseLightTheme",
        RRF_RT_REG_DWORD, nullptr, &value, &cbData);
    return value == 0;
}

static int GetActiveDirs() {
    POINT cursor{};
    GetCursorPos(&cursor);
    int dy  = cursor.y - g_originY.load(std::memory_order_relaxed);
    int dx  = cursor.x - g_originX.load(std::memory_order_relaxed);
    int dz  = g_deadzonePx.load(std::memory_order_relaxed);
    int dir = 0;
    if (dy < -dz) dir |= DIR_UP;
    if (dy >  dz) dir |= DIR_DOWN;
    if (dx < -dz) dir |= DIR_LEFT;
    if (dx >  dz) dir |= DIR_RIGHT;
    return dir;
}

struct IndicatorResources {
    ID2D1Factory*         pFactory     = nullptr;
    ID2D1DCRenderTarget*  pRT          = nullptr;
    HDC                   hdcMem       = nullptr;
    HBITMAP               hBitmap      = nullptr;
    void*                 pvBits       = nullptr;  // pixel data of hBitmap (main DIB)
    HDC                   hdcBase      = nullptr;  // DC for base layer
    HBITMAP               hBitmapBase  = nullptr;
    void*                 pvBase       = nullptr;  // pixel data of hBitmapBase (static cache)
    int                   sizePhys     = 0;

    ID2D1PathGeometry*    pArrows[4]   = {};
    ID2D1StrokeStyle*     pArrowStroke = nullptr;
    float                 strokeWidth  = 2.0f;

    ID2D1SolidColorBrush* pShadowBrush = nullptr;
    ID2D1SolidColorBrush* pBgBrush     = nullptr;
    ID2D1SolidColorBrush* pBorderBrush = nullptr;
    ID2D1SolidColorBrush* pDotBrush    = nullptr;
    ID2D1SolidColorBrush* pArrowBrush  = nullptr;

    void ReleaseBrushes() {
        auto rel = [](ID2D1SolidColorBrush*& p) {
            if (p) { p->Release(); p = nullptr; }
        };
        rel(pShadowBrush);
        rel(pBgBrush); rel(pBorderBrush); rel(pDotBrush); rel(pArrowBrush);
    }

    void Release() {
        ReleaseBrushes();
        for (auto& p : pArrows) { if (p) { p->Release(); p = nullptr; } }
        if (pArrowStroke)  { pArrowStroke->Release();  pArrowStroke = nullptr; }
        if (pRT)           { pRT->Release();           pRT          = nullptr; }
        if (pFactory)      { pFactory->Release();      pFactory     = nullptr; }
        // Delete DCs before bitmaps: a bitmap selected into a DC cannot
        // be deleted (GDI silently returns FALSE and leaks it). Deleting
        // the DC first automatically deselects the bitmap.
        if (hdcBase)       { DeleteDC(hdcBase);         hdcBase     = nullptr; }
        if (hBitmapBase)   { DeleteObject(hBitmapBase); hBitmapBase = nullptr; }
        if (hdcMem)        { DeleteDC(hdcMem);          hdcMem      = nullptr; }
        if (hBitmap)       { DeleteObject(hBitmap);     hBitmap     = nullptr; }
        pvBits = pvBase = nullptr;
    }
};

static HRESULT MakeChevron(
    ID2D1Factory* pF,
    D2D1_POINT_2F tip, D2D1_POINT_2F b1, D2D1_POINT_2F b2,
    ID2D1PathGeometry** pp)
{
    HRESULT hr = pF->CreatePathGeometry(pp);
    if (FAILED(hr)) return hr;

    ID2D1GeometrySink* pSink = nullptr;
    hr = (*pp)->Open(&pSink);
    if (FAILED(hr)) { (*pp)->Release(); *pp = nullptr; return hr; }

    pSink->BeginFigure(b1, D2D1_FIGURE_BEGIN_HOLLOW);
    pSink->AddLine(tip);
    pSink->AddLine(b2);
    pSink->EndFigure(D2D1_FIGURE_END_OPEN);
    pSink->Close();
    pSink->Release();
    return S_OK;
}

static HRESULT InitIndicatorResources(IndicatorResources& res, UINT dpi) {
    if (dpi == 0) dpi = GetDpiForSystem();
    if (dpi == 0) dpi = 96;
    int sz       = MulDiv(44, (int)dpi, 96);
    res.sizePhys = sz;

    HRESULT hr = D2D1CreateFactory(
        D2D1_FACTORY_TYPE_SINGLE_THREADED, &res.pFactory);
    if (FAILED(hr)) return hr;

    res.hdcMem = CreateCompatibleDC(nullptr);
    if (!res.hdcMem) return E_FAIL;

    BITMAPINFO bmi{};
    bmi.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth       = sz;
    bmi.bmiHeader.biHeight      = -sz;
    bmi.bmiHeader.biPlanes      = 1;
    bmi.bmiHeader.biBitCount    = 32;
    bmi.bmiHeader.biCompression = BI_RGB;
    void* pvBits = nullptr;
    res.hBitmap = CreateDIBSection(
        res.hdcMem, &bmi, DIB_RGB_COLORS, &pvBits, nullptr, 0);
    if (!res.hBitmap) return E_FAIL;
    res.pvBits = pvBits;
    SelectObject(res.hdcMem, res.hBitmap);

    // Base layer DIB: same layout as the main DIB; holds the pre-rendered
    // static content (shadow + background + border + dot).
    res.hdcBase = CreateCompatibleDC(nullptr);
    if (!res.hdcBase) return E_FAIL;
    void* pvBase = nullptr;
    res.hBitmapBase = CreateDIBSection(
        res.hdcBase, &bmi, DIB_RGB_COLORS, &pvBase, nullptr, 0);
    if (!res.hBitmapBase) return E_FAIL;
    res.pvBase = pvBase;
    SelectObject(res.hdcBase, res.hBitmapBase);

    D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties(
        D2D1_RENDER_TARGET_TYPE_DEFAULT,
        D2D1::PixelFormat(
            DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED));
    hr = res.pFactory->CreateDCRenderTarget(&props, &res.pRT);
    if (FAILED(hr)) return hr;

    RECT rc = {0, 0, sz, sz};
    hr = res.pRT->BindDC(res.hdcMem, &rc);
    if (FAILED(hr)) return hr;

    {
        D2D1_STROKE_STYLE_PROPERTIES sp{};
        sp.startCap   = D2D1_CAP_STYLE_ROUND;
        sp.endCap     = D2D1_CAP_STYLE_ROUND;
        sp.dashCap    = D2D1_CAP_STYLE_ROUND;
        sp.lineJoin   = D2D1_LINE_JOIN_ROUND;
        sp.miterLimit = 10.0f;
        sp.dashStyle  = D2D1_DASH_STYLE_SOLID;
        sp.dashOffset = 0.0f;
        hr = res.pFactory->CreateStrokeStyle(&sp, nullptr, 0, &res.pArrowStroke);
        if (FAILED(hr)) return hr;
    }
    res.strokeWidth = sz * 0.048f;

    float cx   = sz * 0.5f;
    float cy   = sz * 0.5f;
    float r    = sz * 0.5f - 1.0f;
    float off  = r * 0.58f;
    float arm  = r * 0.15f;
    float half = r * 0.13f;

    hr = MakeChevron(res.pFactory,
        {cx, cy - off}, {cx - half, cy - off + arm}, {cx + half, cy - off + arm},
        &res.pArrows[0]);
    if (FAILED(hr)) return hr;

    hr = MakeChevron(res.pFactory,
        {cx, cy + off}, {cx - half, cy + off - arm}, {cx + half, cy + off - arm},
        &res.pArrows[1]);
    if (FAILED(hr)) return hr;

    hr = MakeChevron(res.pFactory,
        {cx - off, cy}, {cx - off + arm, cy - half}, {cx - off + arm, cy + half},
        &res.pArrows[2]);
    if (FAILED(hr)) return hr;

    hr = MakeChevron(res.pFactory,
        {cx + off, cy}, {cx + off - arm, cy - half}, {cx + off - arm, cy + half},
        &res.pArrows[3]);
    if (FAILED(hr)) return hr;

    return S_OK;
}

static HRESULT CreateBrushes(IndicatorResources& res, bool darkMode) {
    res.ReleaseBrushes();

    D2D1_COLOR_F shadow = D2D1::ColorF(0, 0, 0, 1.0f);
    D2D1_COLOR_F bg  = darkMode
        ? D2D1::ColorF(0x20/255.0f, 0x20/255.0f, 0x20/255.0f, 1.0f)
        : D2D1::ColorF(0xF3/255.0f, 0xF3/255.0f, 0xF3/255.0f, 1.0f);
    D2D1_COLOR_F brd = darkMode
        ? D2D1::ColorF(1.0f, 1.0f, 1.0f, 1.0f)
        : D2D1::ColorF(0.0f, 0.0f, 0.0f, 1.0f);
    D2D1_COLOR_F fg  = darkMode
        ? D2D1::ColorF(1.0f, 1.0f, 1.0f, 1.0f)
        : D2D1::ColorF(0.10f, 0.10f, 0.10f, 1.0f);

    HRESULT hr;
    hr = res.pRT->CreateSolidColorBrush(shadow, &res.pShadowBrush); if (FAILED(hr)) return hr;
    hr = res.pRT->CreateSolidColorBrush(bg,     &res.pBgBrush);     if (FAILED(hr)) return hr;
    hr = res.pRT->CreateSolidColorBrush(brd,    &res.pBorderBrush); if (FAILED(hr)) return hr;
    hr = res.pRT->CreateSolidColorBrush(fg,     &res.pDotBrush);    if (FAILED(hr)) return hr;
    hr = res.pRT->CreateSolidColorBrush(fg,     &res.pArrowBrush);  if (FAILED(hr)) return hr;
    return S_OK;
}

// Render the static parts (shadow, background, border, center dot) into
// the base DIB.  Called once per gesture start and on dark mode change.
// After this, the RT is re-bound to hdcMem for the per-frame hot path.
static void RenderBaseLayer(IndicatorResources& res, bool darkMode) {
    if (!res.pRT || !res.hdcBase) return;

    float sz = (float)res.sizePhys;
    float cx = sz * 0.5f;
    float cy = sz * 0.5f;
    float r  = sz * 0.5f - 1.0f;

    RECT rc = {0, 0, res.sizePhys, res.sizePhys};
    res.pRT->BindDC(res.hdcBase, &rc);

    res.pRT->BeginDraw();
    res.pRT->Clear(D2D1::ColorF(0, 0, 0, 0));

    // Shadow (opacity values are fixed -- no SetOpacity needed per frame)
    {
        const int   kLayers    = 5;
        const float kSpread    = r * 0.08f;
        const float kOffsetY   = r * 0.06f;
        const float kBaseAlpha = 0.10f;
        for (int i = kLayers; i >= 1; --i) {
            float t       = (float)i / kLayers;
            float expand  = kSpread * (1.0f - t);
            float opacity = kBaseAlpha * t * t;
            res.pShadowBrush->SetOpacity(opacity);
            res.pRT->FillEllipse(
                D2D1::Ellipse(D2D1::Point2F(cx, cy + kOffsetY), r + expand, r + expand),
                res.pShadowBrush);
        }
    }

    res.pBgBrush->SetOpacity(0.90f);
    res.pRT->FillEllipse(
        D2D1::Ellipse(D2D1::Point2F(cx, cy), r, r), res.pBgBrush);

    float borderOpacity = darkMode ? 0.10f : 0.07f;
    res.pBorderBrush->SetOpacity(borderOpacity);
    res.pRT->DrawEllipse(
        D2D1::Ellipse(D2D1::Point2F(cx, cy), r, r), res.pBorderBrush, 1.0f);

    res.pDotBrush->SetOpacity(0.80f);
    res.pRT->FillEllipse(
        D2D1::Ellipse(D2D1::Point2F(cx, cy), 2.5f, 2.5f), res.pDotBrush);

    HRESULT hrBase = res.pRT->EndDraw();
    if (FAILED(hrBase))
        Wh_Log(L"autoscroll indicator: RenderBaseLayer EndDraw failed (0x%08X)", hrBase);

    // Re-bind to the main DC so the hot path is ready.
    res.pRT->BindDC(res.hdcMem, &rc);
}

// Per-frame render: restore base layer via memcpy, then draw only the
// chevron arrows whose opacity changed.  11 draw calls -> 4 draw calls.
static void RenderIndicator(IndicatorResources& res, int activeDirs) {
    if (!res.pRT) return;

    // Restore the pre-rendered static base layer (shadow + bg + border + dot)
    // via memcpy, then open D2D only to draw the 4 chevron arrows on top.
    int bytes = res.sizePhys * res.sizePhys * 4;
    if (res.pvBits && res.pvBase)
        memcpy(res.pvBits, res.pvBase, (size_t)bytes);

    res.pRT->BeginDraw();
    // No Clear(): base layer already in place via memcpy.

    // Chevron arrows only (4 draw calls instead of 11)
    const int dirFlags[4] = { DIR_UP, DIR_DOWN, DIR_LEFT, DIR_RIGHT };
    for (int i = 0; i < 4; ++i) {
        if (!res.pArrows[i]) continue;
        bool active = (activeDirs & dirFlags[i]) != 0;
        res.pArrowBrush->SetOpacity(active ? 0.90f : 0.35f);
        res.pRT->DrawGeometry(
            res.pArrows[i], res.pArrowBrush, res.strokeWidth, res.pArrowStroke);
    }

    HRESULT hrDraw = res.pRT->EndDraw();
    if (FAILED(hrDraw))
        Wh_Log(L"autoscroll indicator: EndDraw failed (0x%08X)", hrDraw);
}

// Returns false if UpdateLayeredWindow failed and the window should be
// re-shown to recover visibility.
static bool PresentIndicator(HWND hwnd, IndicatorResources& res) {
    int sz      = res.sizePhys;
    int originX = g_originX.load(std::memory_order_relaxed);
    int originY = g_originY.load(std::memory_order_relaxed);

    POINT ptDst = { originX - sz / 2, originY - sz / 2 };
    POINT ptSrc = { 0, 0 };
    SIZE  szWnd = { sz, sz };
    static BLENDFUNCTION blend = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
    BOOL ok = UpdateLayeredWindow(
        hwnd, nullptr, &ptDst, &szWnd,
        res.hdcMem, &ptSrc, 0, &blend, ULW_ALPHA);
    if (!ok)
        Wh_Log(L"autoscroll indicator: UpdateLayeredWindow failed (%lu)", GetLastError());
    return ok != FALSE;
}

// ---------------------------------------------------------------------------
// Indicator window
// ---------------------------------------------------------------------------
struct IndicatorState {
    IndicatorResources res;
    UINT               currentDpi   = 0;
    bool               darkMode     = false;
    int                lastDirs     = -1;  // last rendered activeDirs; -1 forces first render
};

static LRESULT CALLBACK IndicatorWndProc(
    HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
    switch (msg) {
    case WM_NCCREATE: {
        auto* cs = reinterpret_cast<CREATESTRUCTW*>(lp);
        SetWindowLongPtrW(hwnd, GWLP_USERDATA,
            reinterpret_cast<LONG_PTR>(cs->lpCreateParams));
        return TRUE;
    }

    case WM_IND_SHOW: {
        auto* st = reinterpret_cast<IndicatorState*>(
            GetWindowLongPtrW(hwnd, GWLP_USERDATA));
        if (!st) break;

        // Per-monitor DPI: detect DPI of the window that received the gesture.
        {
            HWND target = g_targetHwnd.load(std::memory_order_relaxed);
            UINT newDpi = target ? GetDpiForWindow(target) : GetDpiForSystem();
            if (newDpi == 0) newDpi = 96;
            if (newDpi != st->currentDpi) {
                st->res.Release();
                if (SUCCEEDED(InitIndicatorResources(st->res, newDpi))) {
                    st->currentDpi = newDpi;
                    // Resize window to new physical size.
                    SetWindowPos(hwnd, nullptr, 0, 0,
                        st->res.sizePhys, st->res.sizePhys,
                        SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE);
                } else {
                    Wh_Log(L"autoscroll indicator: DPI resource recreation failed");
                    break;
                }
                // Force brush and base layer recreation: clear pBgBrush so
                // the dm != st->darkMode || !pBgBrush check below triggers.
                st->res.ReleaseBrushes();
            }
        }

        bool dm = ReadDarkMode();
        if (dm != st->darkMode || !st->res.pBgBrush) {
            st->darkMode = dm;
            if (FAILED(CreateBrushes(st->res, dm))) {
                Wh_Log(L"autoscroll indicator: CreateBrushes failed");
                break;
            }
            RenderBaseLayer(st->res, dm);
        } else if (!st->res.pvBase) {
            RenderBaseLayer(st->res, st->darkMode);
        }

        st->lastDirs = -1;  // force first frame to always render

        int sz = st->res.sizePhys;
        int ox = g_originX.load(std::memory_order_relaxed);
        int oy = g_originY.load(std::memory_order_relaxed);

        // Assert TOPMOST and position in one call before making visible.
        SetWindowPos(hwnd, HWND_TOPMOST,
            ox - sz / 2, oy - sz / 2, 0, 0,
            SWP_NOSIZE | SWP_NOACTIVATE | SWP_SHOWWINDOW);

        RenderIndicator(st->res, 0);
        PresentIndicator(hwnd, st->res);
        st->lastDirs = 0;
        break;
    }

    case WM_IND_UPDATE: {
        // Clear the pending flag before rendering so the scroll thread can
        // queue the next update immediately after this one completes.
        g_indUpdatePending.store(false, std::memory_order_relaxed);

        auto* st = reinterpret_cast<IndicatorState*>(
            GetWindowLongPtrW(hwnd, GWLP_USERDATA));
        if (!st) break;

        int activeDirs = GetActiveDirs();
        if (activeDirs == st->lastDirs) break;  // nothing changed, skip render
        st->lastDirs = activeDirs;

        RenderIndicator(st->res, activeDirs);

        // If UpdateLayeredWindow fails (e.g. window was temporarily hidden
        // by the system), re-assert visibility and topmost.
        if (!PresentIndicator(hwnd, st->res)) {
            int sz = st->res.sizePhys;
            int ox = g_originX.load(std::memory_order_relaxed);
            int oy = g_originY.load(std::memory_order_relaxed);
            SetWindowPos(hwnd, HWND_TOPMOST,
                ox - sz / 2, oy - sz / 2, 0, 0,
                SWP_NOSIZE | SWP_NOACTIVATE | SWP_SHOWWINDOW);
            PresentIndicator(hwnd, st->res);
        }
        break;
    }

    case WM_IND_HIDE:
        g_indUpdatePending.store(false, std::memory_order_relaxed);
        ShowWindow(hwnd, SW_HIDE);
        break;

    case WM_CLOSE:
        DestroyWindow(hwnd);
        return 0;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProcW(hwnd, msg, wp, lp);
}

// Returns the handle of the mod's own DLL (used for window class
// registration so the lpfnWndProc is tied to the correct module).
static HMODULE GetCurrentModuleHandle() {
    HMODULE mod = nullptr;
    GetModuleHandleExW(
        GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
        GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
        L"", &mod);
    return mod;
}

static void IndicatorThreadProc() {
    static const wchar_t kClass[] = L"WH_AutoscrollIndicator";

    HMODULE hMod = GetCurrentModuleHandle();
    if (!hMod) {
        Wh_Log(L"autoscroll indicator: GetCurrentModuleHandle failed");
        return;
    }

    WNDCLASSEXW wc{};
    wc.cbSize        = sizeof(wc);
    wc.lpfnWndProc   = IndicatorWndProc;
    wc.hInstance     = hMod;
    wc.lpszClassName = kClass;
    // Do NOT fall back on ERROR_CLASS_ALREADY_EXISTS: a class with the
    // same name from a previously freed mod instance would have a stale
    // lpfnWndProc pointing to unmapped memory, causing crashes.
    if (!RegisterClassExW(&wc)) {
        Wh_Log(L"autoscroll indicator: RegisterClassExW failed (%lu)", GetLastError());
        return;
    }

    IndicatorState state;

    HRESULT hr = InitIndicatorResources(state.res, GetDpiForSystem());
    if (FAILED(hr)) {
        Wh_Log(L"autoscroll indicator: InitIndicatorResources failed (0x%08X)", hr);
        state.res.Release();
        UnregisterClassW(kClass, hMod);
        return;
    }
    state.currentDpi = state.res.sizePhys > 0
        ? (UINT)MulDiv(state.res.sizePhys, 96, 44) : GetDpiForSystem();

    HWND hwnd = CreateWindowExW(
        WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_TRANSPARENT |
        WS_EX_NOACTIVATE | WS_EX_TOOLWINDOW,
        kClass, nullptr, WS_POPUP,
        0, 0, state.res.sizePhys, state.res.sizePhys,
        nullptr, nullptr, hMod, &state);

    if (!hwnd) {
        Wh_Log(L"autoscroll indicator: CreateWindowExW failed (%lu)", GetLastError());
        state.res.Release();
        UnregisterClassW(kClass, hMod);
        return;
    }

    g_indicatorHwnd.store(hwnd, std::memory_order_release);

    MSG msg{};
    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    g_indicatorHwnd.store(nullptr, std::memory_order_release);
    state.res.Release();
    UnregisterClassW(kClass, hMod);
}

// ===========================================================================
// Settings
// ===========================================================================
static void LoadSettings() {
    int deadzone      = Wh_GetIntSetting(L"deadzone");
    int speed         = Wh_GetIntSetting(L"speed");
    int horz          = Wh_GetIntSetting(L"horizontal");
    int speedMult     = Wh_GetIntSetting(L"speedMult");
    int showIndicator = Wh_GetIntSetting(L"showIndicator");

    deadzone  = std::max(0,  std::min(64,  deadzone));
    speed     = std::max(deadzone + 1, std::min(500, speed));
    speedMult = std::max(1,  std::min(100, speedMult));

    g_deadzonePx   .store(deadzone,          std::memory_order_relaxed);
    g_speedPx      .store(speed,             std::memory_order_relaxed);
    g_horzEnabled  .store(horz != 0,          std::memory_order_relaxed);
    g_speedMult    .store(speedMult,         std::memory_order_relaxed);
    g_showIndicator.store(showIndicator != 0, std::memory_order_relaxed);
}

// ===========================================================================
// Windhawk lifecycle
// ===========================================================================

// Helper: hook a function, log on failure, optionally treat as non-fatal.
static bool HookFn(void* target, void* hook, void** orig,
                   const wchar_t* name, bool fatal = false) {
    if (Wh_SetFunctionHook(target, hook, orig)) return true;
    Wh_Log(L"autoscroll: failed to hook %s", name);
    if (!fatal) *orig = nullptr;
    return !fatal;
}

BOOL Wh_ModInit() {
    LoadSettings();

    // PeekMessage hooks (PeekMessage-driven loops)
    HookFn((void*)PeekMessageW, (void*)PeekMessageW_Hook,
           (void**)&g_origPeekMessageW, L"PeekMessageW");
    HookFn((void*)PeekMessageA, (void*)PeekMessageA_Hook,
           (void**)&g_origPeekMessageA, L"PeekMessageA");

    // DispatchMessage hooks (primary interception point -- fatal if unavailable)
    if (!HookFn((void*)DispatchMessageW, (void*)DispatchMessageW_Hook,
                (void**)&g_origDispatchMessageW, L"DispatchMessageW", true))
        return FALSE;
    HookFn((void*)DispatchMessageA, (void*)DispatchMessageA_Hook,
           (void**)&g_origDispatchMessageA, L"DispatchMessageA");

    return TRUE;
}

void Wh_ModAfterInit() {
    g_scrollEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);  // auto-reset
    if (!g_scrollEvent) {
        // Without the event the scroll thread would busy-spin on a null handle.
        Wh_Log(L"autoscroll: CreateEventW failed (%lu) -- scroll thread not started",
               GetLastError());
        return;
    }
    g_threadRunning.store(true, std::memory_order_relaxed);
    g_scrollThread = std::thread(ScrollThreadProc);
    if (g_showIndicator.load(std::memory_order_relaxed))
        g_indicatorThread = std::thread(IndicatorThreadProc);
}

void Wh_ModSettingsChanged() {
    StopAutoscroll();
    LoadSettings();

    bool show = g_showIndicator.load(std::memory_order_relaxed);
    HWND ind  = g_indicatorHwnd.load(std::memory_order_acquire);

    if (!show && ind) {
        // Indicator is running but should be off: close and join the thread.
        // WM_CLOSE → DestroyWindow → PostQuitMessage exits GetMessage quickly.
        PostMessageW(ind, WM_CLOSE, 0, 0);
        if (g_indicatorThread.joinable())
            g_indicatorThread.join();
    } else if (show && !ind) {
        // Indicator should be on but thread is not running: start it.
        // Join the previous (already-exited) thread handle before replacing.
        if (g_indicatorThread.joinable())
            g_indicatorThread.join();
        g_indicatorThread = std::thread(IndicatorThreadProc);
    }
}

void Wh_ModBeforeUninit() {
    StopAutoscroll();
    HWND ind = g_indicatorHwnd.load(std::memory_order_acquire);
    if (ind) PostMessageW(ind, WM_CLOSE, 0, 0);

}

void Wh_ModUninit() {
    g_threadRunning.store(false, std::memory_order_relaxed);
    if (g_scrollEvent) SetEvent(g_scrollEvent);  // unblock idle scroll thread
    if (g_scrollThread.joinable())    g_scrollThread.join();
    if (g_scrollEvent) { CloseHandle(g_scrollEvent); g_scrollEvent = nullptr; }
    if (g_indicatorThread.joinable()) g_indicatorThread.join();

    g_origPeekMessageW     = nullptr;
    g_origPeekMessageA     = nullptr;
    g_origDispatchMessageW = nullptr;
    g_origDispatchMessageA = nullptr;
}
