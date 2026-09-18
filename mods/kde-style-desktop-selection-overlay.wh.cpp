// ==WindhawkMod==
// @id             kde-style-desktop-selection-overlay
// @name           KDE Style Desktop Selection Overlay
// @description    Draws a custom KDE-inspired rounded selection box when drag-selecting on the Desktop
// @version        2.0
// @author         Xezjk
// @github         https://github.com/xezjk
// @include        explorer.exe
// @compilerOptions -lgdi32 -luser32 -lgdiplus
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# KDE Desktop Selection Overlay
Renders a modern, KDE Breeze-inspired rounded selection box when drag-selecting 
icons or empty space on the Windows Desktop.
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
#include <commctrl.h>
#include <gdiplus.h>
#include <algorithm>

using namespace Gdiplus;

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
HANDLE g_hMonitorThread = NULL;
volatile BOOL g_stopMonitorThread = FALSE;

void ClearOverlayBuffer(HWND hwnd);

void StopDragging() {
    if (g_isDragging) {
        g_isDragging = FALSE;
        if (g_hOverlayWnd) {
            ShowWindow(g_hOverlayWnd, SW_HIDE);
            ClearOverlayBuffer(g_hOverlayWnd);
        }
    }
}

DWORD WINAPI MouseMonitorThreadProc(LPVOID lpParam) {
    while (!g_stopMonitorThread) {
        if (g_isDragging) {
            if (!(GetAsyncKeyState(VK_LBUTTON) & 0x8000)) {
                StopDragging();
            }
        }
        Sleep(10);
    }
    return 0;
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

BOOL IsClickOnEmptyDesktopSpace(HWND hWndUnderMouse, POINT ptScreen) {
    WCHAR className[256];
    GetClassNameW(hWndUnderMouse, className, 256);

    HWND hListView = NULL;

    if (wcscmp(className, L"SysListView32") == 0) {
        hListView = hWndUnderMouse;
    } else if (wcscmp(className, L"WorkerW") == 0 || wcscmp(className, L"Progman") == 0) {
        HWND hShellDll = FindWindowExW(hWndUnderMouse, NULL, L"SHELLDLL_DefView", NULL);
        if (hShellDll) {
            hListView = FindWindowExW(hShellDll, NULL, L"SysListView32", NULL);
        }
    }

    if (!hListView) return FALSE;

    POINT ptClient = ptScreen;
    ScreenToClient(hListView, &ptClient);

    LVHITTESTINFO hitInfo = {0};
    hitInfo.pt = ptClient;

    int hitIndex = (int)SendMessageW(hListView, LVM_HITTEST, 0, (LPARAM)&hitInfo);

    if (hitIndex == -1 || (hitInfo.flags & LVHT_NOWHERE)) {
        return TRUE;
    }

    return FALSE;
}

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

void RedrawOverlay(HWND hwnd, RECT rc) {
    int selWidth = rc.right - rc.left;
    int selHeight = rc.bottom - rc.top;

    if (selWidth <= 2 || selHeight <= 2) {
        ShowWindow(hwnd, SW_HIDE);
        return;
    }

    int screenWidth = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYVIRTUALSCREEN);
    int screenLeft = GetSystemMetrics(SM_XVIRTUALSCREEN);
    int screenTop = GetSystemMetrics(SM_YVIRTUALSCREEN);

    if (settings.keepBehindWindows) {
        HWND hDesktop = GetDesktopWindowHandle();
        SetWindowPos(hwnd, hDesktop, screenLeft, screenTop, screenWidth, screenHeight, SWP_NOACTIVATE | SWP_SHOWWINDOW);
    } else {
        SetWindowPos(hwnd, HWND_TOPMOST, screenLeft, screenTop, screenWidth, screenHeight, SWP_NOACTIVATE | SWP_SHOWWINDOW);
    }

    HDC hdc = GetDC(hwnd);
    HDC hdcMem = CreateCompatibleDC(hdc);

    BITMAPINFO bmi = {0};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = screenWidth;
    bmi.bmiHeader.biHeight = -screenHeight; // Top-down DIB
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    void* pBits = NULL;
    HBITMAP hBitmap = CreateDIBSection(hdcMem, &bmi, DIB_RGB_COLORS, &pBits, NULL, 0);
    HBITMAP hOldBmp = (HBITMAP)SelectObject(hdcMem, hBitmap);

    Graphics graphics(hdcMem);
    graphics.SetSmoothingMode(SmoothingModeAntiAlias);
    graphics.Clear(Color(0, 0, 0, 0));

    REAL x = (REAL)(rc.left - screenLeft);
    REAL y = (REAL)(rc.top - screenTop);
    REAL penWidth = settings.borderThickness;
    REAL offset = penWidth / 2.0f;
    REAL w = (REAL)selWidth - penWidth;
    REAL h = (REAL)selHeight - penWidth;

    int radius = settings.cornerRadius;
    int diameter = radius * 2;
    if (diameter > w) diameter = (int)w;
    if (diameter > h) diameter = (int)h;

    GraphicsPath path;
    path.AddArc(x + offset, y + offset, (REAL)diameter, (REAL)diameter, 180.0f, 90.0f);
    path.AddArc(x + offset + w - diameter, y + offset, (REAL)diameter, (REAL)diameter, 270.0f, 90.0f);
    path.AddArc(x + offset + w - diameter, y + offset + h - diameter, (REAL)diameter, (REAL)diameter, 0.0f, 90.0f);
    path.AddArc(x + offset, y + offset + h - diameter, (REAL)diameter, (REAL)diameter, 90.0f, 90.0f);
    path.CloseFigure();

    // Costruzione nativa ARGB per GDI+ Bitmap 32bit DIB Section
    SolidBrush brush(Color(settings.fillA, settings.fillR, settings.fillG, settings.fillB));
    graphics.FillPath(&brush, &path);

    Pen pen(Color(settings.borderA, settings.borderR, settings.borderG, settings.borderB), penWidth);
    graphics.DrawPath(&pen, &path);

    BLENDFUNCTION blend = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
    POINT ptDst = { screenLeft, screenTop };
    SIZE size = { screenWidth, screenHeight };
    POINT ptSrc = { 0, 0 };

    UpdateLayeredWindow(hwnd, hdc, &ptDst, &size, hdcMem, &ptSrc, 0, &blend, ULW_ALPHA);

    SelectObject(hdcMem, hOldBmp);
    DeleteObject(hBitmap);
    DeleteDC(hdcMem);
    ReleaseDC(hwnd, hdc);
}

void ClearOverlayBuffer(HWND hwnd) {
    HDC hdc = GetDC(hwnd);
    HDC hdcMem = CreateCompatibleDC(hdc);
    HBITMAP hBitmap = CreateCompatibleBitmap(hdc, 1, 1);
    HBITMAP hOldBmp = (HBITMAP)SelectObject(hdcMem, hBitmap);

    Graphics graphics(hdcMem);
    graphics.Clear(Color(0, 0, 0, 0));

    BLENDFUNCTION blend = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
    POINT ptDst = { 0, 0 };
    SIZE size = { 1, 1 };
    POINT ptSrc = { 0, 0 };

    UpdateLayeredWindow(hwnd, hdc, &ptDst, &size, hdcMem, &ptSrc, 0, &blend, ULW_ALPHA);

    SelectObject(hdcMem, hOldBmp);
    DeleteObject(hBitmap);
    DeleteDC(hdcMem);
    ReleaseDC(hwnd, hdc);
}

LRESULT CALLBACK MouseProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode >= 0) {
        MSLLHOOKSTRUCT* pMouse = (MSLLHOOKSTRUCT*)lParam;

        if (wParam == WM_LBUTTONDOWN) {
            HWND hWndUnderMouse = WindowFromPoint(pMouse->pt);
            
            if (IsClickOnEmptyDesktopSpace(hWndUnderMouse, pMouse->pt)) {
                g_ptStart = pMouse->pt;
                g_isDragging = TRUE;

                ShowWindow(g_hOverlayWnd, SW_HIDE);
                ClearOverlayBuffer(g_hOverlayWnd);
            }
        }
        else if (wParam == WM_MOUSEMOVE && g_isDragging) {
            if (!(GetAsyncKeyState(VK_LBUTTON) & 0x8000)) {
                StopDragging();
            } else {
                RECT rc;
                rc.left = (std::min)(g_ptStart.x, pMouse->pt.x);
                rc.top = (std::min)(g_ptStart.y, pMouse->pt.y);
                rc.right = (std::max)(g_ptStart.x, pMouse->pt.x);
                rc.bottom = (std::max)(g_ptStart.y, pMouse->pt.y);

                RedrawOverlay(g_hOverlayWnd, rc);
            }
        }
        else if (wParam == WM_LBUTTONUP && g_isDragging) {
            StopDragging();
        }
    }
    return CallNextHookEx(g_hMouseHook, nCode, wParam, lParam);
}

DWORD WINAPI HookThreadProc(LPVOID lpParam) {
    WNDCLASSW wc = {0};
    wc.lpfnWndProc = DefWindowProcW;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = L"WindhawkKDESelectionOverlay";
    RegisterClassW(&wc);

    g_hOverlayWnd = CreateWindowExW(
        WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW,
        L"WindhawkKDESelectionOverlay", L"",
        WS_POPUP, 0, 0, 0, 0, NULL, NULL, GetModuleHandle(NULL), NULL
    );

    g_hMouseHook = SetWindowsHookEx(WH_MOUSE_LL, MouseProc, GetModuleHandle(NULL), 0);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    if (g_hMouseHook) UnhookWindowsHookEx(g_hMouseHook);
    if (g_hOverlayWnd) DestroyWindow(g_hOverlayWnd);

    return 0;
}

void LoadSettings() {
    settings.cornerRadius = Wh_GetIntSetting(L"cornerRadius");
    if (settings.cornerRadius <= 0) settings.cornerRadius = 4;

    // Lettura valori R, G, B, A separati (evita problemi di parsing esadecimale)
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
    Wh_Log(L"Init KDE Desktop Overlay Mod v2.0.2");
    LoadSettings();

    SetNativeTranslucentSelection(FALSE);

    GdiplusStartupInput gdiplusStartupInput;
    GdiplusStartup(&g_gdiplusToken, &gdiplusStartupInput, NULL);

    g_hThread = CreateThread(NULL, 0, HookThreadProc, NULL, 0, &g_dwThreadId);
    
    g_stopMonitorThread = FALSE;
    g_hMonitorThread = CreateThread(NULL, 0, MouseMonitorThreadProc, NULL, 0, NULL);

    return TRUE;
}

void Wh_ModUninit() {
    SetNativeTranslucentSelection(TRUE);

    g_stopMonitorThread = TRUE;
    if (g_hMonitorThread) {
        WaitForSingleObject(g_hMonitorThread, 1000);
        CloseHandle(g_hMonitorThread);
    }

    if (g_dwThreadId) {
        PostThreadMessage(g_dwThreadId, WM_QUIT, 0, 0);
        WaitForSingleObject(g_hThread, 1000);
        CloseHandle(g_hThread);
    }
    GdiplusShutdown(g_gdiplusToken);
}

void Wh_ModSettingsChanged() {
    LoadSettings();
}
