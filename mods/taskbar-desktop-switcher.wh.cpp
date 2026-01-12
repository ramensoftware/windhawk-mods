// ==WindhawkMod==
// @id              taskbar-desktop-switcher
// @name            Taskbar Desktop Switcher
// @description     Modern, customizable desktop buttons with hover effects
// @version         1.0
// @author          Frqme
// @github         https://github.com/Frqmelikescheese
// @include         explorer.exe
// @compilerOptions -luser32 -lgdi32 -ldwmapi
// ==/WindhawkMod==

// ==WindhawkModSettings==
/*
- desktopCount: 4
  $name: Number of Desktops
- xOffset: 80
  $name: Position X (Px from Left)
- yOffset: 0
  $name: Position Y (Px from Bottom)
- btnWidth: 36
  $name: Button Width
- btnHeight: 30
  $name: Button Height
- btnGap: 4
  $name: Gap Between Buttons
- cornerRadius: 6
  $name: Corner Roundness (0=Square)
- colorActive: 0xB400FF
  $name: Active Color (Hex)
- colorHover: 0x454545
  $name: Hover Color (Hex)
- colorNormal: 0x252525
  $name: Normal Color (Hex)
- colorText: 0xFFFFFF
  $name: Text Color (Hex)
- transparency: 255
  $name: Opacity (0-255)
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <dwmapi.h>

struct Settings {
    int count;
    int x, y;
    int w, h, gap, radius;
    int alpha;
    COLORREF cActive, cHover, cNormal, cText;
} g_cfg;

static HWND g_hOverlayWnd = NULL;
static HWND g_hTaskbar = NULL;
static BOOL g_bStop = FALSE;
static int g_currentDesktop = 1;
static int g_hoverIndex = -1;

COLORREF HexToRGB(int hex) {
    return RGB((hex >> 16) & 0xFF, (hex >> 8) & 0xFF, hex & 0xFF);
}

static void LoadSettings() {
    g_cfg.count = Wh_GetIntSetting(L"desktopCount");
    if(g_cfg.count < 1) g_cfg.count = 4;
    
    g_cfg.x = Wh_GetIntSetting(L"xOffset");
    g_cfg.y = Wh_GetIntSetting(L"yOffset");
    g_cfg.w = Wh_GetIntSetting(L"btnWidth");
    g_cfg.h = Wh_GetIntSetting(L"btnHeight");
    g_cfg.gap = Wh_GetIntSetting(L"btnGap");
    g_cfg.radius = Wh_GetIntSetting(L"cornerRadius");
    g_cfg.alpha = Wh_GetIntSetting(L"transparency");
    
    g_cfg.cActive = HexToRGB(Wh_GetIntSetting(L"colorActive"));
    g_cfg.cHover = HexToRGB(Wh_GetIntSetting(L"colorHover"));
    g_cfg.cNormal = HexToRGB(Wh_GetIntSetting(L"colorNormal"));
    g_cfg.cText = HexToRGB(Wh_GetIntSetting(L"colorText"));
}

static void SwitchToDesktop(int target) {
    if (target < 1 || target > g_cfg.count || target == g_currentDesktop) return;

    int diff = target - g_currentDesktop;
    WORD key = (diff > 0) ? VK_RIGHT : VK_LEFT;
    int steps = abs(diff);

    for (int i = 0; i < steps; i++) {
        keybd_event(VK_LWIN, 0, 0, 0);
        keybd_event(VK_CONTROL, 0, 0, 0);
        keybd_event(key, 0, 0, 0);
        keybd_event(key, 0, KEYEVENTF_KEYUP, 0);
        keybd_event(VK_CONTROL, 0, KEYEVENTF_KEYUP, 0);
        keybd_event(VK_LWIN, 0, KEYEVENTF_KEYUP, 0);
        Sleep(50);
    }
    g_currentDesktop = target;
    if(g_hOverlayWnd) InvalidateRect(g_hOverlayWnd, NULL, FALSE);
}

static void PaintSwitcher(HWND hWnd, HDC hdc) {
    RECT rc;
    GetClientRect(hWnd, &rc);

    HBRUSH bgBrush = CreateSolidBrush(RGB(32, 32, 32)); 
    FillRect(hdc, &rc, bgBrush);
    DeleteObject(bgBrush);

    HFONT font = CreateFont(15, 0, 0, 0, FW_SEMIBOLD, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Segoe UI");
    HFONT oldFont = (HFONT)SelectObject(hdc, font);
    
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, g_cfg.cText);

    for (int i = 1; i <= g_cfg.count; i++) {
        int x = g_cfg.gap + (i - 1) * (g_cfg.w + g_cfg.gap);
        int y = (rc.bottom - g_cfg.h) / 2;
        RECT btnRect = { x, y, x + g_cfg.w, y + g_cfg.h };

        COLORREF btnColor = g_cfg.cNormal;
        
        if (i == g_currentDesktop) {
            btnColor = g_cfg.cActive;
        } else if (i == g_hoverIndex) {
            btnColor = g_cfg.cHover;
        }

        HBRUSH brush = CreateSolidBrush(btnColor);
        HPEN pen = CreatePen(PS_SOLID, 1, btnColor);
        HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, brush);
        HPEN oldPen = (HPEN)SelectObject(hdc, pen);

        if (g_cfg.radius > 0) {
            RoundRect(hdc, btnRect.left, btnRect.top, btnRect.right, btnRect.bottom, g_cfg.radius, g_cfg.radius);
        } else {
            FillRect(hdc, &btnRect, brush);
        }

        SelectObject(hdc, oldBrush);
        SelectObject(hdc, oldPen);
        DeleteObject(brush);
        DeleteObject(pen);

        WCHAR buf[4];
        wsprintf(buf, L"%d", i);
        DrawText(hdc, buf, -1, &btnRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    }

    SelectObject(hdc, oldFont);
    DeleteObject(font);
}

LRESULT CALLBACK OverlayProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            
            RECT rc; GetClientRect(hWnd, &rc);
            HDC memDC = CreateCompatibleDC(hdc);
            HBITMAP memBM = CreateCompatibleBitmap(hdc, rc.right, rc.bottom);
            SelectObject(memDC, memBM);

            PaintSwitcher(hWnd, memDC);

            BitBlt(hdc, 0, 0, rc.right, rc.bottom, memDC, 0, 0, SRCCOPY);
            DeleteObject(memBM);
            DeleteDC(memDC);
            EndPaint(hWnd, &ps);
            return 0;
        }

        case WM_MOUSEMOVE: {
            int x = LOWORD(lParam);
            int idx = (x - g_cfg.gap) / (g_cfg.w + g_cfg.gap);
            int rem = (x - g_cfg.gap) % (g_cfg.w + g_cfg.gap);
            
            int newHover = -1;
            if (rem <= g_cfg.w && idx >= 0 && idx < g_cfg.count) {
                newHover = idx + 1;
            }

            if (newHover != g_hoverIndex) {
                g_hoverIndex = newHover;
                InvalidateRect(hWnd, NULL, FALSE);
                
                TRACKMOUSEEVENT tme;
                tme.cbSize = sizeof(TRACKMOUSEEVENT);
                tme.dwFlags = TME_LEAVE;
                tme.hwndTrack = hWnd;
                TrackMouseEvent(&tme);
            }
            return 0;
        }

        case WM_MOUSELEAVE:
            g_hoverIndex = -1;
            InvalidateRect(hWnd, NULL, FALSE);
            return 0;

        case WM_LBUTTONDOWN: {
            if (g_hoverIndex != -1) {
                SwitchToDesktop(g_hoverIndex);
            }
            return 0;
        }

        case WM_TIMER: {
            if (!g_hTaskbar || !IsWindow(g_hTaskbar)) {
                g_hTaskbar = FindWindow(L"Shell_TrayWnd", NULL);
            }

            if (g_hTaskbar) {
                RECT rcTask;
                GetWindowRect(g_hTaskbar, &rcTask);
                
                int myW = g_cfg.count * (g_cfg.w + g_cfg.gap) + g_cfg.gap;
                int myH = g_cfg.h + 4; 
                
                int x = rcTask.left + g_cfg.x;
                int y = rcTask.top + (rcTask.bottom - rcTask.top - myH) / 2 - g_cfg.y;

                SetWindowPos(hWnd, HWND_TOPMOST, x, y, myW, myH, 
                    SWP_NOACTIVATE | SWP_SHOWWINDOW | SWP_NOCOPYBITS);
            }
            break;
        }

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

DWORD WINAPI ThreadMain(LPVOID) {
    WNDCLASSEX wc = { sizeof(WNDCLASSEX) };
    wc.lpfnWndProc = OverlayProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = L"WhStyledSwitcherFinal";
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    RegisterClassEx(&wc);

    g_hOverlayWnd = CreateWindowEx(
        WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE | WS_EX_LAYERED,
        L"WhStyledSwitcherFinal", L"",
        WS_POPUP,
        0, 0, 100, 40,
        NULL, NULL, GetModuleHandle(NULL), NULL
    );

    if (g_hOverlayWnd) {
        SetLayeredWindowAttributes(g_hOverlayWnd, 0, (BYTE)g_cfg.alpha, LWA_ALPHA);
        SetTimer(g_hOverlayWnd, 1, 200, NULL);
        ShowWindow(g_hOverlayWnd, SW_SHOWNA);

        MSG msg;
        while (GetMessage(&msg, NULL, 0, 0) && !g_bStop) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
    return 0;
}

BOOL Wh_ModInit() {
    LoadSettings();
    g_bStop = FALSE;
    CreateThread(NULL, 0, ThreadMain, NULL, 0, NULL);
    return TRUE;
}

void Wh_ModUninit() {
    g_bStop = TRUE;
    if (g_hOverlayWnd) SendMessage(g_hOverlayWnd, WM_CLOSE, 0, 0);
}

void Wh_ModSettingsChanged() {
    LoadSettings();
    if(g_hOverlayWnd) {
        SetLayeredWindowAttributes(g_hOverlayWnd, 0, (BYTE)g_cfg.alpha, LWA_ALPHA);
        InvalidateRect(g_hOverlayWnd, NULL, TRUE);
    }
}
