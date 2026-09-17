// ==WindhawkMod==
// @id             kde-style-desktop-selection-overlay
// @name           KDE Style Desktop Selection Overlay
// @description    Draws a custom KDE-inspired rounded selection box when drag-selecting on the Desktop
// @version        1.0
// @author         Xezjk
// @github         xezjk
// @include        explorer.exe
// @compilerOptions -lgdi32 -luser32 -lgdiplus
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# KDE Desktop Selection Overlay
Renders a modern, KDE Breeze-inspired rounded selection box when drag-selecting 
icons or empty space on the Windows Desktop.
Automatically hides the default Windows translucent selection box while active.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- cornerRadius: 4
  $name: Corner Radius (px)
  $description: Sets the curvature radius of the selection box corners.
- keepBehindWindows: true
  $name: Render Behind Windows
  $description: If enabled, the selection box stays behind other active windows instead of rendering on top.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <gdiplus.h>
#include <algorithm>

using namespace Gdiplus;

struct {
    int cornerRadius;
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

// Instantly stops dragging and hides the selection box
void StopDragging() {
    if (g_isDragging) {
        g_isDragging = FALSE;
        if (g_hOverlayWnd) {
            ShowWindow(g_hOverlayWnd, SW_HIDE);
            ClearOverlayBuffer(g_hOverlayWnd);
        }
    }
}

// Dedicated thread for hardware-level mouse state monitoring (100 Hz)
DWORD WINAPI MouseMonitorThreadProc(LPVOID lpParam) {
    while (!g_stopMonitorThread) {
        if (g_isDragging) {
            // Check if mouse left button is released globally at the system level
            if (!(GetAsyncKeyState(VK_LBUTTON) & 0x8000)) {
                StopDragging();
            }
        }
        Sleep(10);
    }
    return 0;
}

// Finds the appropriate Desktop window handle to position the overlay directly above it
HWND GetDesktopWindowHandle() {
    HWND hProgman = FindWindowW(L"Progman", NULL);
    HWND hDesktopWnd = hProgman;

    // Search for active WorkerW if animated wallpaper engines or Windows created one
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

// Enables or disables the native Windows translucent selection rectangle via Registry
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

    // Z-Order management based on user settings
    if (settings.keepBehindWindows) {
        HWND hDesktop = GetDesktopWindowHandle();
        // Position overlay directly above Desktop window (behind all other active applications)
        SetWindowPos(hwnd, hDesktop, screenLeft, screenTop, screenWidth, screenHeight, SWP_NOACTIVATE | SWP_SHOWWINDOW);
    } else {
        // Position overlay in the foreground on top of all windows
        SetWindowPos(hwnd, HWND_TOPMOST, screenLeft, screenTop, screenWidth, screenHeight, SWP_NOACTIVATE | SWP_SHOWWINDOW);
    }

    HDC hdc = GetDC(hwnd);
    HDC hdcMem = CreateCompatibleDC(hdc);
    HBITMAP hBitmap = CreateCompatibleBitmap(hdc, screenWidth, screenHeight);
    HBITMAP hOldBmp = (HBITMAP)SelectObject(hdcMem, hBitmap);

    Graphics graphics(hdcMem);
    graphics.SetSmoothingMode(SmoothingModeAntiAlias);
    graphics.Clear(Color(0, 0, 0, 0));

    REAL x = (REAL)(rc.left - screenLeft);
    REAL y = (REAL)(rc.top - screenTop);
    REAL penWidth = 1.5f;
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

    SolidBrush brush(Color(80, 61, 174, 233));
    graphics.FillPath(&brush, &path);

    Pen pen(Color(220, 61, 174, 233), penWidth);
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
            WCHAR className[256];
            GetClassNameW(hWndUnderMouse, className, 256);

            if (wcscmp(className, L"SysListView32") == 0 || wcscmp(className, L"WorkerW") == 0 || wcscmp(className, L"Progman") == 0) {
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
    if (settings.cornerRadius <= 0) settings.cornerRadius = 10;

    settings.keepBehindWindows = Wh_GetIntSetting(L"keepBehindWindows");
}

BOOL Wh_ModInit() {
    Wh_Log(L"Init KDE Desktop Overlay Mod v2.6");
    LoadSettings();

    SetNativeTranslucentSelection(FALSE);

    GdiplusStartupInput gdiplusStartupInput;
    GdiplusStartup(&g_gdiplusToken, &gdiplusStartupInput, NULL);

    g_hThread = CreateThread(NULL, 0, HookThreadProc, NULL, 0, &g_dwThreadId);
    
    // Start independent thread for hardware-level mouse polling
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
