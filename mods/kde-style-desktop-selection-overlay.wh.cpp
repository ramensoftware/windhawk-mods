// ==WindhawkMod==
// @id           kde-style-desktop-selection-overlay
// @name         KDE Style Desktop Selection Overlay
// @description  Draws a custom KDE-inspired rounded selection box when drag-selecting on the Desktop
// @version      2.5
// @author       Xezjk
// @github       https://github.com/xezjk
// @include      explorer.exe
// @compilerOptions -lgdi32 -luser32 -lgdiplus -lcomctl32 -lshlwapi
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# KDE Desktop Selection Overlay
Renders a modern, KDE Breeze-inspired rounded selection box when drag-selecting 
icons or empty space on the Windows Desktop.

![KDE Desktop Selection Overlay](https://raw.githubusercontent.com/xezjk/kde-style-desktop-selection-overlay/main/assets/preview.png)

### Features & Settings:
- **Corner Radius**: Adjust the curvature of the selection box corners.
- **Fill & Border Colors**: Customize RGB and Alpha values for fill and outline.
- **Border Thickness**: Set custom outline thickness (value divided by 10 px).
- **Render Layer**: Choose whether to render behind or above existing windows.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- cornerRadius: 4
  $name: Corner Radius (px)
  $description: Sets the curvature radius of the selection box corners.
- fillR: 61
  $name: Fill Red (0-255)
- fillG: 174
  $name: Fill Green (0-255)
- fillB: 233
  $name: Fill Blue (0-255)
- fillA: 80
  $name: Fill Alpha (0-255)
- borderR: 61
  $name: Border Red (0-255)
- borderG: 174
  $name: Border Green (0-255)
- borderB: 233
  $name: Border Blue (0-255)
- borderA: 220
  $name: Border Alpha (0-255)
- borderThickness: 15
  $name: Border Thickness (x10 px)
- keepBehindWindows: true
  $name: Render Behind Windows
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <gdiplus.h>
#include <algorithm>

using namespace Gdiplus;

// Custom Window Messages
#define WM_USER_START_DRAG  (WM_USER + 1)
#define WM_USER_UPDATE_DRAG (WM_USER + 2)
#define WM_USER_STOP_DRAG   (WM_USER + 3)

#define TIMER_DRAG_CHECK 101

struct {
    int cornerRadius;
    BYTE fillR, fillG, fillB, fillA;
    BYTE borderR, borderG, borderB, borderA;
    float borderThickness;
    BOOL keepBehindWindows;
} settings;

ULONG_PTR g_gdiplusToken;
HHOOK g_hMouseHook = NULL;
HWND g_hOverlayWnd = NULL;
POINT g_ptStart = {0, 0};
volatile BOOL g_isDragging = FALSE;
HANDLE g_hThread = NULL;
DWORD g_dwThreadId = 0;
HANDLE g_hThreadReadyEvent = NULL;

HWND g_hCachedListView = NULL;

HDC g_hdcMem = NULL;
HBITMAP g_hBitmap = NULL;
HBITMAP g_hOldBmp = NULL;
void* g_pBits = NULL;
int g_cachedBufWidth = 0;
int g_cachedBufHeight = 0;

void SetNativeTranslucentSelection(BOOL enable) {
    HKEY hKey;
    if (RegOpenKeyExW(HKEY_CURRENT_USER, 
                      L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced", 
                      0, KEY_SET_VALUE, &hKey) == ERROR_SUCCESS) {
        DWORD value = enable ? 1 : 0;
        RegSetValueExW(hKey, L"ListviewAlphaSelect", 0, REG_DWORD, (BYTE*)&value, sizeof(value));
        RegCloseKey(hKey);

        DWORD_PTR result;
        SendMessageTimeoutW(HWND_BROADCAST, WM_SETTINGCHANGE, 0, 0, SMTO_ABORTIFHUNG, 100, &result);
    }
}

HWND GetDesktopWindowHandle() {
    HWND hProgman = FindWindowW(L"Progman", NULL);
    HWND hDesktopWnd = hProgman;
    HWND hWorkerW = NULL;

    do {
        hWorkerW = FindWindowExW(NULL, hWorkerW, L"WorkerW", NULL);
        if (hWorkerW) {
            HWND hSHELLDLL = FindWindowExW(hWorkerW, NULL, L"SHELLDLL_DefView", NULL);
            if (hSHELLDLL) {
                hDesktopWnd = hWorkerW;
                break;
            }
        }
    } while (hWorkerW);

    return hDesktopWnd;
}

HWND GetDesktopListViewHandle() {
    HWND hDesktop = GetDesktopWindowHandle();
    if (!hDesktop) return NULL;

    HWND hShellDll = FindWindowExW(hDesktop, NULL, L"SHELLDLL_DefView", NULL);
    if (!hShellDll) {
        HWND hProgman = FindWindowW(L"Progman", NULL);
        hShellDll = FindWindowExW(hProgman, NULL, L"SHELLDLL_DefView", NULL);
    }

    if (hShellDll) {
        return FindWindowExW(hShellDll, NULL, L"SysListView32", NULL);
    }
    return NULL;
}

void FreeRenderTarget() {
    if (g_hdcMem) {
        if (g_hOldBmp) SelectObject(g_hdcMem, g_hOldBmp);
        if (g_hBitmap) DeleteObject(g_hBitmap);
        DeleteDC(g_hdcMem);
        g_hdcMem = NULL;
        g_hBitmap = NULL;
        g_hOldBmp = NULL;
        g_pBits = NULL;
        g_cachedBufWidth = 0;
        g_cachedBufHeight = 0;
    }
}

void EnsureRenderTarget(HDC hdcRef, int width, int height) {
    if (g_hdcMem && width == g_cachedBufWidth && height == g_cachedBufHeight) {
        return;
    }

    FreeRenderTarget();

    g_hdcMem = CreateCompatibleDC(hdcRef);

    BITMAPINFO bmi = {0};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = width;
    bmi.bmiHeader.biHeight = -height;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    g_hBitmap = CreateDIBSection(g_hdcMem, &bmi, DIB_RGB_COLORS, &g_pBits, NULL, 0);
    g_hOldBmp = (HBITMAP)SelectObject(g_hdcMem, g_hBitmap);
    g_cachedBufWidth = width;
    g_cachedBufHeight = height;
}

BOOL IsClickOnEmptyDesktopSpace(POINT ptScreen) {
    if (!g_hCachedListView || !IsWindow(g_hCachedListView)) {
        g_hCachedListView = GetDesktopListViewHandle();
    }

    if (!g_hCachedListView) return FALSE;

    POINT ptClient = ptScreen;
    ScreenToClient(g_hCachedListView, &ptClient);

    LVHITTESTINFO hitInfo = {0};
    hitInfo.pt = ptClient;

    int hitIndex = (int)SendMessageW(g_hCachedListView, LVM_HITTEST, 0, (LPARAM)&hitInfo);
    return (hitIndex == -1 || (hitInfo.flags & LVHT_NOWHERE));
}

void StopDraggingInternal(HWND hwnd) {
    if (g_isDragging) {
        g_isDragging = FALSE;
        KillTimer(hwnd, TIMER_DRAG_CHECK);
        ShowWindow(hwnd, SW_HIDE);
    }
}

void RedrawOverlay(HWND hwnd, RECT rc) {
    int selWidth = rc.right - rc.left;
    int selHeight = rc.bottom - rc.top;

    if (selWidth <= 2 || selHeight <= 2) {
        ShowWindow(hwnd, SW_HIDE);
        return;
    }

    float penWidth = settings.borderThickness;
    int margin = (int)ceilf(penWidth);
    
    int w = selWidth + (margin * 2);
    int h = selHeight + (margin * 2);

    int posX = rc.left - margin;
    int posY = rc.top - margin;

    if (settings.keepBehindWindows) {
        HWND hDesktop = GetDesktopWindowHandle();
        SetWindowPos(hwnd, hDesktop, posX, posY, w, h, SWP_NOACTIVATE | SWP_SHOWWINDOW);
    } else {
        SetWindowPos(hwnd, HWND_TOPMOST, posX, posY, w, h, SWP_NOACTIVATE | SWP_SHOWWINDOW);
    }

    HDC hdc = GetDC(hwnd);
    EnsureRenderTarget(hdc, w, h);

    Bitmap bitmap(w, h, w * 4, PixelFormat32bppPARGB, (BYTE*)g_pBits);
    Graphics graphics(&bitmap);
    graphics.SetSmoothingMode(SmoothingModeAntiAlias);
    graphics.Clear(Color(0, 0, 0, 0));

    REAL x = (REAL)margin;
    REAL y = (REAL)margin;
    REAL offset = penWidth / 2.0f;
    REAL drawW = (REAL)selWidth - penWidth;
    REAL drawH = (REAL)selHeight - penWidth;

    int radius = settings.cornerRadius;
    int diameter = radius * 2;
    if (diameter > drawW) diameter = (int)drawW;
    if (diameter > drawH) diameter = (int)drawH;

    GraphicsPath path;
    if (diameter > 0) {
        path.AddArc(x + offset, y + offset, (REAL)diameter, (REAL)diameter, 180.0f, 90.0f);
        path.AddArc(x + offset + drawW - diameter, y + offset, (REAL)diameter, (REAL)diameter, 270.0f, 90.0f);
        path.AddArc(x + offset + drawW - diameter, y + offset + drawH - diameter, (REAL)diameter, (REAL)diameter, 0.0f, 90.0f);
        path.AddArc(x + offset, y + offset + drawH - diameter, (REAL)diameter, (REAL)diameter, 90.0f, 90.0f);
        path.CloseFigure();
    } else {
        path.AddRectangle(RectF(x + offset, y + offset, drawW, drawH));
    }

    SolidBrush brush(Color(settings.fillA, settings.fillR, settings.fillG, settings.fillB));
    graphics.FillPath(&brush, &path);

    Pen pen(Color(settings.borderA, settings.borderR, settings.borderG, settings.borderB), penWidth);
    graphics.DrawPath(&pen, &path);

    BLENDFUNCTION blend = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
    POINT ptDst = { posX, posY };
    SIZE size = { w, h };
    POINT ptSrc = { 0, 0 };

    UpdateLayeredWindow(hwnd, hdc, &ptDst, &size, g_hdcMem, &ptSrc, 0, &blend, ULW_ALPHA);
    ReleaseDC(hwnd, hdc);
}

LRESULT CALLBACK OverlayWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_USER_START_DRAG: {
        POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
        if (IsClickOnEmptyDesktopSpace(pt)) {
            g_ptStart = pt;
            g_isDragging = TRUE;
            SetTimer(hwnd, TIMER_DRAG_CHECK, 15, NULL);
        }
        return 0;
    }
    case WM_USER_UPDATE_DRAG: {
        if (g_isDragging) {
            POINT ptCurrent = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
            RECT rc;
            rc.left = (std::min)(g_ptStart.x, ptCurrent.x);
            rc.top = (std::min)(g_ptStart.y, ptCurrent.y);
            rc.right = (std::max)(g_ptStart.x, ptCurrent.x);
            rc.bottom = (std::max)(g_ptStart.y, ptCurrent.y);

            RedrawOverlay(hwnd, rc);
        }
        return 0;
    }
    case WM_USER_STOP_DRAG: {
        StopDraggingInternal(hwnd);
        return 0;
    }
    case WM_TIMER: {
        if (wParam == TIMER_DRAG_CHECK) {
            if (!(GetAsyncKeyState(VK_LBUTTON) & 0x8000)) {
                StopDraggingInternal(hwnd);
            }
        }
        return 0;
    }
    case WM_DESTROY: {
        StopDraggingInternal(hwnd);
        FreeRenderTarget();
        PostQuitMessage(0);
        return 0;
    }
    }
    return DefWindowProcW(hwnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK MouseProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode >= 0 && g_hOverlayWnd) {
        MSLLHOOKSTRUCT* pMouse = (MSLLHOOKSTRUCT*)lParam;

        if (wParam == WM_LBUTTONDOWN) {
            PostMessageW(g_hOverlayWnd, WM_USER_START_DRAG, 0, MAKELPARAM(pMouse->pt.x, pMouse->pt.y));
        }
        else if (wParam == WM_MOUSEMOVE && g_isDragging) {
            PostMessageW(g_hOverlayWnd, WM_USER_UPDATE_DRAG, 0, MAKELPARAM(pMouse->pt.x, pMouse->pt.y));
        }
        else if (wParam == WM_LBUTTONUP && g_isDragging) {
            PostMessageW(g_hOverlayWnd, WM_USER_STOP_DRAG, 0, 0);
        }
    }
    return CallNextHookEx(g_hMouseHook, nCode, wParam, lParam);
}

DWORD WINAPI HookThreadProc(LPVOID lpParam) {
    HMODULE hMod = GetModuleHandle(NULL);

    MSG msgPeek;
    PeekMessageW(&msgPeek, NULL, WM_USER, WM_USER, PM_NOREMOVE);

    WNDCLASSW wc = {0};
    wc.lpfnWndProc = OverlayWndProc;
    wc.hInstance = hMod;
    wc.lpszClassName = L"WindhawkKDESelectionOverlay";
    
    if (!RegisterClassW(&wc) && GetLastError() != ERROR_CLASS_ALREADY_EXISTS) {
        SetEvent(g_hThreadReadyEvent);
        return 0;
    }

    g_hOverlayWnd = CreateWindowExW(
        WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW,
        L"WindhawkKDESelectionOverlay", L"",
        WS_POPUP, 0, 0, 0, 0, NULL, NULL, hMod, NULL
    );

    g_hMouseHook = SetWindowsHookExW(WH_MOUSE_LL, MouseProc, hMod, 0);

    SetEvent(g_hThreadReadyEvent);

    MSG msg;
    while (GetMessageW(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    if (g_hMouseHook) {
        UnhookWindowsHookEx(g_hMouseHook);
        g_hMouseHook = NULL;
    }

    if (g_hOverlayWnd) {
        DestroyWindow(g_hOverlayWnd);
        g_hOverlayWnd = NULL;
    }

    UnregisterClassW(L"WindhawkKDESelectionOverlay", hMod);
    return 0;
}

void LoadSettings() {
    settings.cornerRadius = Wh_GetIntSetting(L"cornerRadius");
    if (settings.cornerRadius < 0) settings.cornerRadius = 0;

    settings.fillR = (BYTE)Wh_GetIntSetting(L"fillR");
    settings.fillG = (BYTE)Wh_GetIntSetting(L"fillG");
    settings.fillB = (BYTE)Wh_GetIntSetting(L"fillB");
    settings.fillA = (BYTE)Wh_GetIntSetting(L"fillA");

    settings.borderR = (BYTE)Wh_GetIntSetting(L"borderR");
    settings.borderG = (BYTE)Wh_GetIntSetting(L"borderG");
    settings.borderB = (BYTE)Wh_GetIntSetting(L"borderB");
    settings.borderA = (BYTE)Wh_GetIntSetting(L"borderA");

    int rawThickness = Wh_GetIntSetting(L"borderThickness");
    settings.borderThickness = (rawThickness > 0) ? ((float)rawThickness / 10.0f) : 1.5f;

    settings.keepBehindWindows = Wh_GetIntSetting(L"keepBehindWindows");
}

BOOL Wh_ModInit() {
    // Process Scoping Guard: Ensure mod only runs inside the main shell Explorer process
    HWND hShellWnd = GetShellWindow();
    DWORD dwShellProcessId = 0;
    if (hShellWnd) {
        GetWindowThreadProcessId(hShellWnd, &dwShellProcessId);
    }

    if (dwShellProcessId == 0 || dwShellProcessId != GetCurrentProcessId()) {
        return FALSE;
    }

    Wh_Log(L"Init KDE Desktop Overlay Mod v2.1.1");
    LoadSettings();

    // Disable native translucent marquee via registry setting
    SetNativeTranslucentSelection(FALSE);

    GdiplusStartupInput gdiplusStartupInput;
    GdiplusStartup(&g_gdiplusToken, &gdiplusStartupInput, NULL);

    g_hThreadReadyEvent = CreateEventW(NULL, TRUE, FALSE, NULL);

    g_hThread = CreateThread(NULL, 0, HookThreadProc, NULL, 0, &g_dwThreadId);
    if (g_hThread) {
        WaitForSingleObject(g_hThreadReadyEvent, INFINITE);
    }
    
    CloseHandle(g_hThreadReadyEvent);
    g_hThreadReadyEvent = NULL;

    return TRUE;
}

void Wh_ModUninit() {
    // Re-enable native selection rectangle on unload
    SetNativeTranslucentSelection(TRUE);

    if (g_dwThreadId) {
        while (!PostThreadMessageW(g_dwThreadId, WM_QUIT, 0, 0)) {
            Sleep(10);
        }
        
        WaitForSingleObject(g_hThread, INFINITE);
        CloseHandle(g_hThread);
        g_hThread = NULL;
        g_dwThreadId = 0;
    }

    GdiplusShutdown(g_gdiplusToken);
}

void Wh_ModSettingsChanged() {
    LoadSettings();
}
