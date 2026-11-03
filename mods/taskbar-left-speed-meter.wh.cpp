// ==WindhawkMod==
// @id              taskbar-left-speed-meter
// @name            Taskbar Left Speed Meter
// @description     Displays real-time Internet download and upload speed on the bottom-left corner of the taskbar.
// @version         1.0.0
// @author          RAKIBUL ISLAM SANI
// @github          https://github.com/rakibulislam
// @include         explorer.exe
// @compilerOptions -liphlpapi -lgdi32 -luser32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Taskbar Left Speed Meter

A lightweight network traffic monitor that docks seamlessly on the bottom-left corner of the Windows taskbar.

### Features
- Real-time download and upload speed monitoring.
- Crisp, fringe-free text rendering compatible with both dark and light themes.
- Fully customizable text color (HEX code), font family, font size, and horizontal position via the Settings tab.
- Click-through transparent overlay that does not interfere with taskbar clicks.

### Author
Developed by **RAKIBUL ISLAM SANI**.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- textColor: "#000000"
  $name: Text Color (HEX)
  $description: "HEX color code (e.g. #000000 for Black, #FFFFFF for White, #00FF7F for Neon Green, #00E5FF for Cyan)"
- fontName: "Segoe UI"
  $name: Font Name
  $description: "Font family name (e.g. Segoe UI, Consolas, Arial, Tahoma, Calibri)"
- isBold: true
  $name: Bold Text
  $description: "Enable bold styling for the speed text"
- fontSize: 13
  $name: Font Size
  $description: "Text font size (Default: 13)"
- posX: 20
  $name: Left Margin (X)
  $description: "Horizontal offset in pixels from the left corner (Default: 20)"
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <iphlpapi.h>
#include <stdlib.h>
#include <stdio.h>

#define TIMER_ID 1001
#define TRANSPARENT_COLOR RGB(255, 0, 255)

HWND hOverlayWnd = NULL;
HANDLE hThread = NULL;
DWORD dwThreadId = 0;
BOOL bRunning = TRUE;

COLORREF g_textColor = RGB(0, 0, 0);
wchar_t g_fontName[64] = L"Segoe UI";
BOOL g_isBold = TRUE;
int g_fontSize = 13;
int g_posX = 20;

DWORD prevInBytes = 0, prevOutBytes = 0;
wchar_t displayText[128] = L"▼ 0.0 KB/s\n▲ 0.0 KB/s";

COLORREF HexToColorRef(PCWSTR hexColor) {
    if (!hexColor || wcslen(hexColor) < 6) return RGB(0, 0, 0);
    const wchar_t* p = hexColor;
    if (*p == L'#') p++;
    unsigned int r = 0, g = 0, b = 0;
    if (swscanf(p, L"%02x%02x%02x", &r, &g, &b) == 3) {
        return RGB(r, g, b);
    }
    return RGB(0, 0, 0);
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
    }

    g_isBold = Wh_GetIntSetting(L"isBold") != 0;

    g_fontSize = Wh_GetIntSetting(L"fontSize");
    if (g_fontSize <= 0) g_fontSize = 13;

    g_posX = Wh_GetIntSetting(L"posX");
    if (g_posX < 0) g_posX = 20;
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

void UpdatePosition() {
    HWND hTaskbar = FindWindow(L"Shell_TrayWnd", NULL);
    if (!hTaskbar || !hOverlayWnd) return;

    RECT rcTaskbar;
    GetWindowRect(hTaskbar, &rcTaskbar);

    int width = 130;
    int height = 38;
    int x = rcTaskbar.left + g_posX;
    int y = rcTaskbar.top + ((rcTaskbar.bottom - rcTaskbar.top - height) / 2);

    SetWindowPos(hOverlayWnd, HWND_TOPMOST, x, y, width, height, SWP_NOACTIVATE | SWP_SHOWWINDOW);
}

void RefreshSpeed() {
    DWORD curIn = 0, curOut = 0;
    FetchNetworkTraffic(&curIn, &curOut);

    double downSpeed = (double)(DWORD)(curIn - prevInBytes) / 1024.0;
    double upSpeed = (double)(DWORD)(curOut - prevOutBytes) / 1024.0;

    prevInBytes = curIn;
    prevOutBytes = curOut;

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

    UpdatePosition();
    InvalidateRect(hOverlayWnd, NULL, TRUE);
}

LRESULT CALLBACK OverlayProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_TIMER:
            if (wParam == TIMER_ID) {
                RefreshSpeed();
            }
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

            int weight = g_isBold ? FW_BOLD : FW_NORMAL;
            HFONT hFont = CreateFont(g_fontSize, 0, 0, 0, weight, FALSE, FALSE, FALSE,
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
    HINSTANCE hInstance = GetModuleHandle(NULL);

    WNDCLASS wc = { 0 };
    wc.lpfnWndProc = OverlayProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"WindhawkTaskbarLeftMeterWindow";
    RegisterClass(&wc);

    hOverlayWnd = CreateWindowEx(
        WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_NOACTIVATE,
        L"WindhawkTaskbarLeftMeterWindow", L"",
        WS_POPUP, 0, 0, 130, 38,
        NULL, NULL, hInstance, NULL
    );

    if (hOverlayWnd) {
        SetLayeredWindowAttributes(hOverlayWnd, TRANSPARENT_COLOR, 0, LWA_COLORKEY);
        ShowWindow(hOverlayWnd, SW_SHOWNOACTIVATE);
        UpdateWindow(hOverlayWnd);

        FetchNetworkTraffic(&prevInBytes, &prevOutBytes);
        UpdatePosition();

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

    return 0;
}

BOOL Wh_ModInit() {
    LoadSettings();
    bRunning = TRUE;
    hThread = CreateThread(NULL, 0, OverlayThreadProc, NULL, 0, &dwThreadId);
    return (hThread != NULL);
}

void Wh_ModSettingsChanged() {
    LoadSettings();
    if (hOverlayWnd) {
        UpdatePosition();
        InvalidateRect(hOverlayWnd, NULL, TRUE);
    }
}

void Wh_ModUninit() {
    bRunning = FALSE;
    if (dwThreadId) {
        PostThreadMessage(dwThreadId, WM_QUIT, 0, 0);
    }
    if (hThread) {
        WaitForSingleObject(hThread, 2000);
        CloseHandle(hThread);
        hThread = NULL;
    }
}
