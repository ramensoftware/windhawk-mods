// ==WindhawkMod==
// @id              classic-frame-dpi-fix
// @name            Classic Frame DPI Fix
// @description     Draws the classic title bar and window frame at the right size in programs that were already running when the display scaling was changed
// @name:ru         Классическая рамка при смене масштаба
// @description:ru  Рисует классический заголовок и рамку окна нужного размера в программах, запущенных до смены масштаба экрана
// @version         1.0
// @author          appEW
// @github          https://github.com/appEW
// @include         *
// @exclude         dwm.exe
// @exclude         csrss.exe
// @exclude         smss.exe
// @exclude         services.exe
// @exclude         lsass.exe
// @exclude         fontdrvhost.exe
// @architecture    x86-64
// @compilerOptions -luser32 -lgdi32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Classic Frame DPI Fix

Fixes the classic window frame after the display scaling changes - for example
when a 100% monitor is swapped for a TV running at 150-225%. Programs that were
already running then show a tiny title bar with small buttons and an empty strip
under it; this mod draws their frame and title bar at the right size again.

> **Tested only on Windows 11 24H2 (build 26100).** It has not been tried on any other version of Windows and may not work there.

## What happens without the mod

The system DPI is fixed for a process when it starts and does not change until
you sign out. Windows sizes and hit-tests the non-client area by the DPI of the
window, but draws it by the DPI of the process. As long as the two agree nothing
shows. After a scaling change they no longer agree for every program that was
already running: the title bar is given, say, 47 pixels, but is drawn 19 pixels
high - with a small font, small buttons and an empty strip under it. Restarting
Explorer fixes only Explorer: every process that was already running keeps its
old system DPI.

The same mismatch appears on a second monitor with a different scale for
programs declared per-monitor aware.

## What the mod does

After the normal painting of the caption, the mod repaints the frame and the
title bar at the DPI of the window: the frame, the caption gradient, the icon,
the text and the buttons take their metrics and font from
`GetSystemMetricsForDpi` and `SystemParametersInfoForDpi`. The painting goes
through the same system calls Windows uses (`DrawCaptionTempW`,
`DrawFrameControl`), so the result matches a normal window pixel for pixel.

The mod does nothing when the system DPI of the process matches the DPI of the
window, so in a normal session without a scaling change it does not touch a
single window. It also leaves DPI-unaware programs alone: DWM stretches their
whole window, and their small caption is drawn correctly.

## What it does not do

The menu bar, the scroll bars and the highlight of a caption button while it is
being pressed stay at the scale of the process. A pressed button briefly lights
up in the wrong place; the caption is repainted once it is released.

---

## По-русски

Чинит классическую рамку окна после смены масштаба экрана — например когда
монитор со 100% заменяется телевизором со 150–225%.

## Что происходит без мода

Системный DPI закрепляется за процессом в момент его запуска и до перезахода в
учётную запись не меняется. Размер и хит-тест неклиентской области считает
ядро по DPI самого окна, а рисует неклиентскую область ядро по DPI процесса.
Пока оба значения совпадают, разницы не видно. После смены масштаба у всех уже
запущенных программ они расходятся: под заголовок отводится, скажем, 47
пикселей, а рисуется заголовок высотой 19 — с мелким шрифтом, мелкими кнопками
и пустой полосой под ним. Перезапуск проводника чинит только сам проводник:
каждый уже запущенный процесс остаётся со старым системным DPI.

Тот же расчёт расходится и на втором мониторе с другим масштабом, если
программа объявлена per-monitor aware.

## Что делает мод

После штатной отрисовки заголовка мод перерисовывает рамку и заголовок по DPI
окна: рамка, градиент заголовка, значок, текст и кнопки берут метрики и шрифт
из `GetSystemMetricsForDpi` и `SystemParametersInfoForDpi`. Отрисовка идёт теми
же системными вызовами, что и у ядра (`DrawCaptionTempW`, `DrawFrameControl`),
поэтому результат совпадает с обычным окном пиксель в пиксель.

Мод ничего не делает, если системный DPI процесса совпадает с DPI окна, то есть
в обычном сеансе без смены масштаба он не трогает ни одного окна. Не трогает он
и DPI-unaware программы: их окна целиком растягивает DWM, и мелкий заголовок у
них нарисован правильно.

## Чего мод не делает

Полоса меню, полосы прокрутки и подсветка нажатой кнопки заголовка во время
клика остаются в масштабе процесса. Кнопка при нажатии на мгновение
подсвечивается не на своём месте; после отпускания заголовок перерисовывается.

> **Проверено только на Windows 11 24H2 (сборка 26100).** На других версиях Windows мод не проверялся и может не работать.
*/
// ==/WindhawkModReadme==

#include <windhawk_api.h>
#include <windows.h>

#define WM_NCUAHDRAWCAPTION 0x00AE
#define WM_NCUAHDRAWFRAME   0x00AF

typedef UINT (WINAPI *P_GetDpiForWindow)(HWND);
typedef UINT (WINAPI *P_GetSystemDpiForProcess)(HANDLE);
typedef int  (WINAPI *P_GetSystemMetricsForDpi)(int, UINT);
typedef BOOL (WINAPI *P_SystemParametersInfoForDpi)(UINT, UINT, PVOID, UINT, UINT);
typedef BOOL (WINAPI *P_AdjustWindowRectExForDpi)(LPRECT, DWORD, BOOL, DWORD, UINT);
typedef HANDLE (WINAPI *P_GetDpiAwarenessContextForProcess)(HANDLE);
typedef int (WINAPI *P_GetAwarenessFromDpiAwarenessContext)(HANDLE);
typedef BOOL (WINAPI *P_DrawCaptionTempW)(HWND, HDC, const RECT*, HFONT, HICON, LPCWSTR, UINT);

static P_GetDpiForWindow pGetDpiForWindow;
static P_GetSystemDpiForProcess pGetSystemDpiForProcess;
static P_GetSystemMetricsForDpi pGetSystemMetricsForDpi;
static P_SystemParametersInfoForDpi pSystemParametersInfoForDpi;
static P_AdjustWindowRectExForDpi pAdjustWindowRectExForDpi;
static P_GetDpiAwarenessContextForProcess pGetDpiAwarenessContextForProcess;
static P_GetAwarenessFromDpiAwarenessContext pGetAwarenessFromDpiAwarenessContext;
static P_DrawCaptionTempW pDrawCaptionTempW;

// The system DPI this process was started with. The kernel paints the classic
// non-client area with it, but lays the area out with the DPI of the window.
static UINT g_processDpi;
static BOOL g_enabled;

static CRITICAL_SECTION g_fontLock;
static HFONT g_capFont, g_smCapFont;
static UINT g_capFontDpi, g_smCapFontDpi;

static HFONT CaptionFont(UINT dpi, BOOL small) {
    HFONT result = NULL;
    EnterCriticalSection(&g_fontLock);
    HFONT* slot = small ? &g_smCapFont : &g_capFont;
    UINT* slotDpi = small ? &g_smCapFontDpi : &g_capFontDpi;
    if (*slot && *slotDpi == dpi) {
        result = *slot;
    } else {
        NONCLIENTMETRICSW ncm = { sizeof(ncm) };
        if (pSystemParametersInfoForDpi &&
            pSystemParametersInfoForDpi(SPI_GETNONCLIENTMETRICS, sizeof(ncm), &ncm, 0, dpi)) {
            HFONT f = CreateFontIndirectW(small ? &ncm.lfSmCaptionFont : &ncm.lfCaptionFont);
            if (f) {
                if (*slot) DeleteObject(*slot);
                *slot = f;
                *slotDpi = dpi;
                result = f;
            }
        }
    }
    LeaveCriticalSection(&g_fontLock);
    return result;
}

// Non-client layout of a plain classic frame at `dpi`: border thickness and
// caption height, as the kernel reserves them.
static BOOL FrameMetrics(HWND hwnd, UINT dpi, int* pBorder, int* pCaptionH) {
    LONG style = GetWindowLongW(hwnd, GWL_STYLE);
    LONG exStyle = GetWindowLongW(hwnd, GWL_EXSTYLE);
    RECT adj = { 0, 0, 0, 0 };
    if (!pAdjustWindowRectExForDpi(&adj, style & ~(WS_HSCROLL | WS_VSCROLL), FALSE, exStyle, dpi))
        return FALSE;
    *pBorder = adj.bottom;
    *pCaptionH = -adj.top - adj.bottom;
    return *pBorder >= 0 && *pCaptionH > 0;
}

// Is this a top-level classic frame whose caption the kernel paints too small?
static BOOL NeedsFix(HWND hwnd, UINT* pDpi) {
    if (!g_enabled) return FALSE;
    LONG style = GetWindowLongW(hwnd, GWL_STYLE);
    if ((style & WS_CHILD) || !(style & WS_CAPTION)) return FALSE;
    if (IsIconic(hwnd) || !IsWindowVisible(hwnd)) return FALSE;
    UINT dpi = pGetDpiForWindow(hwnd);
    if (!dpi || dpi == g_processDpi) return FALSE;

    int border, captionH;
    if (!FrameMetrics(hwnd, dpi, &border, &captionH)) return FALSE;

    // Plenty of programs keep WS_CAPTION but answer WM_NCCALCSIZE themselves so
    // that the client area covers the caption, then draw their own title bar
    // (every Chromium window does). Their window DC must not be painted into.
    // Only a window whose client area starts exactly where a plain classic
    // frame would put it is one the kernel drew and we may redraw.
    RECT wr, cr;
    POINT clientOrigin = { 0, 0 };
    if (!GetWindowRect(hwnd, &wr) || !GetClientRect(hwnd, &cr) ||
        !ClientToScreen(hwnd, &clientOrigin))
        return FALSE;
    if (clientOrigin.y - wr.top != border + captionH) return FALSE;
    if (clientOrigin.x - wr.left != border) return FALSE;

    *pDpi = dpi;
    return TRUE;
}

// Sizing border, in window coordinates.
static void PaintBorder(HDC hdc, int w, int h, int border) {
    RECT rcEdge = { 0, 0, w, h };
    if (border < 2) return;
    DrawEdge(hdc, &rcEdge, EDGE_RAISED, BF_RECT | BF_ADJUST);
    if (border > 2) {
        HBRUSH face = GetSysColorBrush(COLOR_3DFACE);
        RECT r;
        r = rcEdge; r.bottom = r.top + (border - 2); FillRect(hdc, &r, face);
        r = rcEdge; r.top = r.bottom - (border - 2); FillRect(hdc, &r, face);
        r = rcEdge; r.right = r.left + (border - 2); FillRect(hdc, &r, face);
        r = rcEdge; r.left = r.right - (border - 2); FillRect(hdc, &r, face);
    }
}

// Caption bar of hwnd, drawn into hdc in window coordinates with the metrics of
// `dpi` instead of the ones the process was started with.
static void PaintCaption(HWND hwnd, HDC hdc, UINT dpi, BOOL active, int w, int border, int captionH) {
    LONG style = GetWindowLongW(hwnd, GWL_STYLE);
    LONG exStyle = GetWindowLongW(hwnd, GWL_EXSTYLE);
    BOOL small = (exStyle & WS_EX_TOOLWINDOW) != 0;
    RECT rcCap = { border, border, w - border, border + captionH };
    if (rcCap.right <= rcCap.left) return;

    BOOL gradient = FALSE;
    SystemParametersInfoW(SPI_GETGRADIENTCAPTIONS, 0, &gradient, 0);

    UINT flags = DC_ICON | DC_TEXT;
    if (active) flags |= DC_ACTIVE;
    if (gradient) flags |= DC_GRADIENT;
    if (small) flags |= DC_SMALLCAP;

    // The caption gradient stops at the leftmost button; the strip behind the
    // buttons is flat. Splitting the caption there is what makes the gradient
    // land on the same colours as a window drawn by the kernel itself.
    BOOL hasButtons = (style & WS_SYSMENU) != 0;
    BOOL hasMinMax = hasButtons && !small && (style & (WS_MINIMIZEBOX | WS_MAXIMIZEBOX)) != 0;
    int bw = hasButtons ? pGetSystemMetricsForDpi(small ? SM_CXSMSIZE : SM_CXSIZE, dpi) - 2 : 0;
    RECT rcText = rcCap;
    if (hasButtons && bw > 0) {
        rcText.right = rcCap.right - 2 - bw;
        if (hasMinMax) rcText.right -= 2 + bw + bw;
        if (rcText.right < rcText.left) rcText.right = rcText.left;
        RECT rcStrip = { rcText.right, rcCap.top, rcCap.right, rcCap.bottom };
        int flat = gradient ? (active ? COLOR_GRADIENTACTIVECAPTION : COLOR_GRADIENTINACTIVECAPTION)
                            : (active ? COLOR_ACTIVECAPTION : COLOR_INACTIVECAPTION);
        FillRect(hdc, &rcStrip, GetSysColorBrush(flat));
    }

    WCHAR title[512];
    title[0] = 0;
    InternalGetWindowText(hwnd, title, ARRAYSIZE(title) - 1);

    if (rcText.right > rcText.left)
        pDrawCaptionTempW(hwnd, hdc, &rcText, CaptionFont(dpi, small), NULL, title, flags);

    if (hasButtons && bw > 0) {
        RECT rb = { rcCap.right - 2 - bw, rcCap.top + 2, rcCap.right - 2, rcCap.bottom - 2 };
        DrawFrameControl(hdc, &rb, DFC_CAPTION, DFCS_CAPTIONCLOSE);
        if (hasMinMax) {
            OffsetRect(&rb, -(bw + 2), 0);
            DrawFrameControl(hdc, &rb, DFC_CAPTION,
                ((style & WS_MAXIMIZE) ? DFCS_CAPTIONRESTORE : DFCS_CAPTIONMAX) |
                ((style & WS_MAXIMIZEBOX) ? 0 : DFCS_INACTIVE));
            OffsetRect(&rb, -bw, 0);
            DrawFrameControl(hdc, &rb, DFC_CAPTION,
                DFCS_CAPTIONMIN | ((style & WS_MINIMIZEBOX) ? 0 : DFCS_INACTIVE));
        }
    }
}

// The kernel has already painted its undersized caption; draw over it. The
// caption goes through a buffer so the window does not visibly flash between
// the two sizes; the border is thin enough to draw straight onto the window.
static void RepaintFrame(HWND hwnd, UINT dpi, BOOL active) {
    RECT wr;
    if (!GetWindowRect(hwnd, &wr)) return;
    int w = wr.right - wr.left, h = wr.bottom - wr.top;
    if (w <= 0 || h <= 0) return;

    int border, captionH;
    if (!FrameMetrics(hwnd, dpi, &border, &captionH)) return;

    HDC hdc = GetWindowDC(hwnd);
    if (!hdc) return;

    PaintBorder(hdc, w, h, border);

    int stripH = border + captionH;
    HDC mem = CreateCompatibleDC(hdc);
    HBITMAP bmp = mem ? CreateCompatibleBitmap(hdc, w, stripH) : NULL;
    if (mem && bmp) {
        HGDIOBJ old = SelectObject(mem, bmp);
        BitBlt(mem, 0, 0, w, stripH, hdc, 0, 0, SRCCOPY);
        PaintCaption(hwnd, mem, dpi, active, w, border, captionH);
        BitBlt(hdc, 0, 0, w, stripH, mem, 0, 0, SRCCOPY);
        SelectObject(mem, old);
    } else {
        PaintCaption(hwnd, hdc, dpi, active, w, border, captionH);
    }
    if (bmp) DeleteObject(bmp);
    if (mem) DeleteDC(mem);
    ReleaseDC(hwnd, hdc);
}

using DefWindowProc_t = LRESULT (WINAPI*)(HWND, UINT, WPARAM, LPARAM);
DefWindowProc_t DefWindowProcW_orig;
DefWindowProc_t DefWindowProcA_orig;

static LRESULT AfterDefWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LRESULT res) {
    int active = -1;
    switch (msg) {
    case WM_NCACTIVATE:
        active = wParam ? 1 : 0;
        break;
    case WM_NCPAINT:
    case WM_NCUAHDRAWCAPTION:
    case WM_NCUAHDRAWFRAME:
    case WM_SETTEXT:
    case WM_SETICON:
    case WM_NCLBUTTONDOWN:   // after the button tracking loop has finished
    case WM_NCLBUTTONUP:
        break;
    default:
        return res;
    }

    UINT dpi;
    if (NeedsFix(hwnd, &dpi))
        RepaintFrame(hwnd, dpi, active >= 0 ? active : (GetForegroundWindow() == hwnd));
    return res;
}

LRESULT WINAPI DefWindowProcW_hook(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    return AfterDefWindowProc(hwnd, msg, wParam, DefWindowProcW_orig(hwnd, msg, wParam, lParam));
}

LRESULT WINAPI DefWindowProcA_hook(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    return AfterDefWindowProc(hwnd, msg, wParam, DefWindowProcA_orig(hwnd, msg, wParam, lParam));
}

static BOOL CALLBACK RedrawTopLevel(HWND hwnd, LPARAM) {
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    if (pid == GetCurrentProcessId() && IsWindowVisible(hwnd))
        RedrawWindow(hwnd, NULL, NULL, RDW_FRAME | RDW_INVALIDATE);
    return TRUE;
}

BOOL Wh_ModInit() {
    InitializeCriticalSection(&g_fontLock);

    HMODULE user32 = GetModuleHandleW(L"user32.dll");
    pGetDpiForWindow = (P_GetDpiForWindow)GetProcAddress(user32, "GetDpiForWindow");
    pGetSystemDpiForProcess = (P_GetSystemDpiForProcess)GetProcAddress(user32, "GetSystemDpiForProcess");
    pGetSystemMetricsForDpi = (P_GetSystemMetricsForDpi)GetProcAddress(user32, "GetSystemMetricsForDpi");
    pSystemParametersInfoForDpi = (P_SystemParametersInfoForDpi)GetProcAddress(user32, "SystemParametersInfoForDpi");
    pAdjustWindowRectExForDpi = (P_AdjustWindowRectExForDpi)GetProcAddress(user32, "AdjustWindowRectExForDpi");
    pGetDpiAwarenessContextForProcess = (P_GetDpiAwarenessContextForProcess)GetProcAddress(user32, "GetDpiAwarenessContextForProcess");
    pGetAwarenessFromDpiAwarenessContext = (P_GetAwarenessFromDpiAwarenessContext)GetProcAddress(user32, "GetAwarenessFromDpiAwarenessContext");
    pDrawCaptionTempW = (P_DrawCaptionTempW)GetProcAddress(user32, "DrawCaptionTempW");

    if (!pGetDpiForWindow || !pGetSystemDpiForProcess || !pGetSystemMetricsForDpi ||
        !pSystemParametersInfoForDpi || !pAdjustWindowRectExForDpi ||
        !pGetDpiAwarenessContextForProcess || !pGetAwarenessFromDpiAwarenessContext ||
        !pDrawCaptionTempW) {
        Wh_Log(L"a DPI entry point is missing, nothing to do");
        return TRUE;
    }

    // A DPI-unaware process is stretched by DWM as a whole, so its small
    // caption is already the right size once scaled. Leave it alone.
    int awareness = pGetAwarenessFromDpiAwarenessContext(
        pGetDpiAwarenessContextForProcess(GetCurrentProcess()));
    g_processDpi = pGetSystemDpiForProcess(GetCurrentProcess());
    g_enabled = awareness != DPI_AWARENESS_UNAWARE && g_processDpi != 0;
    Wh_Log(L"process DPI %u, awareness %d, enabled %d", g_processDpi, awareness, g_enabled);

    Wh_SetFunctionHook((void*)GetProcAddress(user32, "DefWindowProcW"),
                       (void*)DefWindowProcW_hook, (void**)&DefWindowProcW_orig);
    Wh_SetFunctionHook((void*)GetProcAddress(user32, "DefWindowProcA"),
                       (void*)DefWindowProcA_hook, (void**)&DefWindowProcA_orig);
    return TRUE;
}

void Wh_ModAfterInit() {
    if (g_enabled) EnumWindows(RedrawTopLevel, 0);
}

void Wh_ModUninit() {
    g_enabled = FALSE;
    EnumWindows(RedrawTopLevel, 0);
    if (g_capFont) DeleteObject(g_capFont);
    if (g_smCapFont) DeleteObject(g_smCapFont);
    DeleteCriticalSection(&g_fontLock);
}
