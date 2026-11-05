// ==WindhawkMod==
// @id              taskbar-left-speed-meter
// @name            Taskbar Left Speed Meter
// @description     Displays real-time Internet download and upload speed on the bottom-left corner of the taskbar with full customization.
// @version         1.1.0
// @author          RAKIBUL ISLAM SANI
// @github          https://github.com/rakibulislamsani
// @include         explorer.exe
// @compilerOptions -liphlpapi -lgdi32 -luser32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Taskbar Left Speed Meter

A lightweight network traffic monitor docked on the bottom-left corner of the Windows taskbar (specifically tailored for centered taskbars in Windows 11).

### Features & Customization
- **Tailored Left-Corner Docking:** Seamlessly positions network rates on the bottom-left of the taskbar.
- **Deep Visual Styling:** Fully customizable via the Settings tab:
  - Custom arbitrary text color (HEX code support).
  - Custom font family (e.g. Segoe UI, Consolas, Arial, Tahoma).
  - Toggleable bold text styling.
  - Custom font size with automatic per-monitor DPI scaling.
  - Horizontal margin offset adjustments.
- **Auto-Hide Support:** Automatically hides when applications go fullscreen (videos, games) or when the taskbar is set to auto-hide.
- **Click-Through:** Transparent overlay that does not block mouse clicks to underlying taskbar elements.

### Author
Developed by **RAKIBUL ISLAM SANI**.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- textColor: "#00FF7F"
  $name: Text Color (HEX)
  $description: "HEX color code (e.g. #00FF7F for Neon Green, #00E5FF for Cyan, #FFFFFF for White, #000000 for Black)"
- fontName: "Segoe UI"
  $name: Font Name
  $description: "Font family name (e.g. Segoe UI, Consolas, Arial, Tahoma, Calibri)"
- isBold: true
  $name: Bold Text
  $description: "Enable bold styling for the speed text"
- fontSize: 13
  $name: Font Size
  $description: "Base text font size (Auto-scaled by monitor DPI. Default: 13)"
- posX: 20
  $name: Left Margin (X)
  $description: "Horizontal offset in pixels from the left corner (Default: 20)"
- hideOnFullscreen: true
  $name: Hide on Fullscreen / Taskbar Auto-Hide
  $description: "Automatically hide the speed meter when a window is fullscreen or the taskbar is hidden"
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <iphlpapi.h>
#include <stdlib.h>
#include <stdio.h>

#define TIMER_ID 1001
#define WM_REFRESH_SETTINGS (WM_APP + 1)
#define TRANSPARENT_COLOR RGB(255, 0, 255)

HWND hOverlayWnd = NULL;
HANDLE hThread = NULL;
DWORD dwThreadId = 0;
BOOL bRunning = TRUE;

COLORREF g_textColor = RGB(0, 255, 127);
wchar_t g_fontName[64] = L"Segoe UI";
BOOL g_isBold = TRUE;
int g_fontSize = 13;
int g_posX = 20;
BOOL g_hideOnFullscreen = TRUE;

DWORD prevInBytes = 0, prevOutBytes = 0;
wchar_t displayText[128] = L"▼ 0.0 KB/s\n▲ 0.0 KB/s";

// Retrieve the current module handle safely
HINSTANCE GetCurrentModuleHandle() {
    HINSTANCE hInst = NULL;
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                           GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                       (LPCWSTR)&GetCurrentModuleHandle, &hInst);
    return hInst;
}

// Get DPI for the given window with fallback
UINT GetTaskbarDpi(HWND hWnd) {
    HMODULE hUser32 = GetModuleHandle(L"user32.dll");
    if (hUser32) {
        typedef UINT(WINAPI * GetDpiForWindow_t)(HWND);
        GetDpiForWindow_t pGetDpiForWindow = (GetDpiForWindow_t)GetProcAddress(hUser32, "GetDpiForWindow");
        if (pGetDpiForWindow && hWnd) {
            return pGetDpiForWindow(hWnd);
        }
    }
    HDC hdc = GetDC(NULL);
    UINT dpi = GetDeviceCaps(hdc, LOGPIXELSY);
    ReleaseDC(NULL, hdc);
    return dpi ? dpi : 96;
}

COLORREF HexToColorRef(PCWSTR hexColor) {
    if (!hexColor || wcslen(hexColor) < 6) return RGB(0, 255, 127);
    const wchar_t* p = hexColor;
    if (*p == L'#') p++;
    unsigned int r = 0, g = 0, b = 0;
    if (swscanf(p, L"%02x%02x%02x", &r, &g, &b) == 3) {
        return RGB(r, g, b);
    }
    return RGB(0, 255, 127);
}

void LoadSettings() {
    PCWSTR colorStr = Wh_GetStringSetting(L"textColor");
    if (colorStr) {
        g_textColor = HexToColorRef(colorStr);
        Wh_FreeStringSetting(colorStr);
    }

    PCWSTR fontStr = Wh_GetStringSetting(L"fontName");
    if (fontStr && wcslen(fontStr) > 0) {
        wcsncpy_s(g_fontName, 64, fontStr, _TRUNCATE);
        Wh_FreeStringSetting(fontStr);
    } else {
        wcsncpy_s(g_fontName, 64, L"Segoe UI", _TRUNCATE);
    }

    g_isBold = Wh_GetIntSetting(L"isBold") != 0;

    g_fontSize = Wh_GetIntSetting(L"fontSize");
    if (g_fontSize <= 0) g_fontSize = 13;

    g_posX = Wh_GetIntSetting(L"posX");
    if (g_posX < 0) g_posX = 20;

    g_hideOnFullscreen = Wh_GetIntSetting(L"hideOnFullscreen") != 0;
}

void FetchNetworkTraffic(DWORD *inBytes, DWORD *outBytes) {
    *inBytes = 0;
    *outBytes = 0;
    ULONG size = 0;

    if (GetIfTable(NULL, &size, FALSE) == ERROR_INSUFFICIENT_BUFFER) {
        PMIB_IFTABLE table = (PMIB_IFTABLE)malloc(size);
        if (table) {
            if (GetIfTable(table, &size, FALSE) == NO_ERROR) {
                for (DWORD i = 0; i < table->dwNumEntries; i++) {
                    if (table->table[i].dwType != MIB_IF_TYPE_LOOPBACK) {
                        *inBytes += table->table[i].dwInOctets;
                        *outBytes += table->table[i].dwOutOctets;
                    }
                }
            }
            free(table);
        }
    }
}

BOOL ShouldHideMeter(HWND hTaskbar) {
    if (!g_hideOnFullscreen) return FALSE;

    if (!hTaskbar || !IsWindowVisible(hTaskbar)) return TRUE;

    RECT rcTaskbar;
    GetWindowRect(hTaskbar, &rcTaskbar);
    HMONITOR hMon = MonitorFromWindow(hTaskbar, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi = { sizeof(mi) };
    if (GetMonitorInfo(hMon, &mi)) {
        if (rcTaskbar.top >= mi.rcMonitor.bottom - 4 || (rcTaskbar.bottom - rcTaskbar.top) <= 5) {
            return TRUE;
        }
    }

    HWND hFore = GetForegroundWindow();
    if (hFore && hFore != hTaskbar && hFore != GetDesktopWindow()) {
        HMONITOR hForeMon = MonitorFromWindow(hFore, MONITOR_DEFAULTTONEAREST);
        if (hForeMon == hMon) {
            RECT rcFore;
            GetWindowRect(hFore, &rcFore);
            if (EqualRect(&rcFore, &mi.rcMonitor)) {
                wchar_t className[256];
                GetClassName(hFore, className, 256);
                if (wcscmp(className, L"Progman") != 0 && wcscmp(className, L"WorkerW") != 0) {
                    return TRUE;
                }
            }
        }
    }

    return FALSE;
}

void UpdatePosition(HWND hTaskbar) {
    if (!hTaskbar || !hOverlayWnd) return;

    RECT rcTaskbar;
    GetWindowRect(hTaskbar, &rcTaskbar);

    UINT dpi = GetTaskbarDpi(hTaskbar);
    int width = MulDiv(140, dpi, 96);
    int height = MulDiv(40, dpi, 96);
    int posXScaled = MulDiv(g_posX, dpi, 96);

    int x = rcTaskbar.left + posXScaled;
    int y = rcTaskbar.top + ((rcTaskbar.bottom - rcTaskbar.top - height) / 2);

    SetWindowPos(hOverlayWnd, HWND_TOPMOST, x, y, width, height, SWP_NOACTIVATE | SWP_SHOWWINDOW);
}

void RefreshSpeed() {
    // Verify that the taskbar window exists and belongs to the current shell process
    HWND hTaskbar = FindWindow(L"Shell_TrayWnd", NULL);
    DWORD taskbarPid = 0;
    if (hTaskbar) {
        GetWindowThreadProcessId(hTaskbar, &taskbarPid);
    }

    if (!hTaskbar || taskbarPid != GetCurrentProcessId()) {
        if (IsWindowVisible(hOverlayWnd)) {
            ShowWindow(hOverlayWnd, SW_HIDE);
        }
        return;
    }

    DWORD curIn = 0, curOut = 0;
    FetchNetworkTraffic(&curIn, &curOut);

    double downSpeed = (double)(DWORD)(curIn - prevInBytes) / 1024.0;
    double upSpeed = (double)(DWORD)(curOut - prevOutBytes) / 1024.0;

    prevInBytes = curIn;
    prevOutBytes = curOut;

    if (ShouldHideMeter(hTaskbar)) {
        if (IsWindowVisible(hOverlayWnd)) {
            ShowWindow(hOverlayWnd, SW_HIDE);
        }
        return;
    } else {
        if (!IsWindowVisible(hOverlayWnd)) {
            ShowWindow(hOverlayWnd, SW_SHOWNOACTIVATE);
        }
    }

    wchar_t downStr[64], upStr[64];
    if (downSpeed >= 1024.0)
        swprintf(downStr, 64, L"▼ %.2f MB/s", downSpeed / 1024.0);
    else
        swprintf(downStr, 64, L"▼ %.1f KB/s", downSpeed);

    if (upSpeed >= 1024.0)
        swprintf(upStr, 64, L"▲ %.2f MB/s", upSpeed / 1024.0);
    else
        swprintf(upStr, 64, L"▲ %.1f KB/s", upSpeed);

    swprintf(displayText, 128, L"%s\n%s", downStr, upStr);

    UpdatePosition(hTaskbar);
    InvalidateRect(hOverlayWnd, NULL, TRUE);
}

LRESULT CALLBACK OverlayProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_TIMER:
            if (wParam == TIMER_ID) {
                RefreshSpeed();
            }
            return 0;

        case WM_REFRESH_SETTINGS:
            LoadSettings();
            RefreshSpeed();
            return 0;

        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);

            RECT rc;
            GetClientRect(hWnd, &rc);

            HBRUSH hTransBrush = CreateSolidBrush(TRANSPARENT_COLOR);
            FillRect(hdc, &rc, hTransBrush);
            DeleteObject(hTransBrush);

            SetBkMode(hdc, TRANSPARENT);
            SetTextColor(hdc, g_textColor);

            HWND hTaskbar = FindWindow(L"Shell_TrayWnd", NULL);
            UINT dpi = GetTaskbarDpi(hTaskbar);
            int scaledFontSize = MulDiv(g_fontSize, dpi, 96);

            int weight = g_isBold ? FW_BOLD : FW_NORMAL;
            HFONT hFont = CreateFont(scaledFontSize, 0, 0, 0, weight, FALSE, FALSE, FALSE,
                                     DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                                     NONANTIALIASED_QUALITY, DEFAULT_PITCH, g_fontName);
            HFONT oldFont = (HFONT)SelectObject(hdc, hFont);

            DrawText(hdc, displayText, -1, &rc, DT_LEFT | DT_TOP);

            SelectObject(hdc, oldFont);
            DeleteObject(hFont);
            EndPaint(hWnd, &ps);
            return 0;
        }

        case WM_NCHITTEST:
            return HTTRANSPARENT;

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;

        default:
            return DefWindowProc(hWnd, msg, wParam, lParam);
    }
}

DWORD WINAPI OverlayThreadProc(LPVOID lpParam) {
    HINSTANCE hInst = GetCurrentModuleHandle();

    WNDCLASS wc = { 0 };
    wc.lpfnWndProc = OverlayProc;
    wc.hInstance = hInst;
    wc.lpszClassName = L"WindhawkTaskbarLeftMeterWindow";
    
    if (!RegisterClass(&wc)) {
        Wh_Log(L"RegisterClass failed: %u", GetLastError());
        return 0;
    }

    HWND hTaskbar = FindWindow(L"Shell_TrayWnd", NULL);
    UINT dpi = GetTaskbarDpi(hTaskbar);
    int width = MulDiv(140, dpi, 96);
    int height = MulDiv(40, dpi, 96);

    hOverlayWnd = CreateWindowEx(
        WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_NOACTIVATE,
        wc.lpszClassName, L"",
        WS_POPUP, 0, 0, width, height,
        NULL, NULL, hInst, NULL
    );

    if (hOverlayWnd) {
        SetLayeredWindowAttributes(hOverlayWnd, TRANSPARENT_COLOR, 0, LWA_COLORKEY);
        FetchNetworkTraffic(&prevInBytes, &prevOutBytes);

        if (hTaskbar) {
            UpdatePosition(hTaskbar);
            ShowWindow(hOverlayWnd, SW_SHOWNOACTIVATE);
        }

        SetTimer(hOverlayWnd, TIMER_ID, 1000, NULL);
    }

    MSG msg;
    while (bRunning && GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    if (hOverlayWnd) {
        KillTimer(hOverlayWnd, TIMER_ID);
        DestroyWindow(hOverlayWnd);
        hOverlayWnd = NULL;
    }

    UnregisterClass(wc.lpszClassName, hInst);
    return 0;
}

BOOL Wh_ModInit() {
    LoadSettings();
    bRunning = TRUE;
    hThread = CreateThread(NULL, 0, OverlayThreadProc, NULL, 0, &dwThreadId);
    return (hThread != NULL);
}

void Wh_ModSettingsChanged() {
    if (hOverlayWnd) {
        PostMessage(hOverlayWnd, WM_REFRESH_SETTINGS, 0, 0);
    }
}

void Wh_ModUninit() {
    bRunning = FALSE;
    if (dwThreadId) {
        PostThreadMessage(dwThreadId, WM_QUIT, 0, 0);
    }
    if (hThread) {
        WaitForSingleObject(hThread, INFINITE);
        CloseHandle(hThread);
        hThread = NULL;
    }
}
