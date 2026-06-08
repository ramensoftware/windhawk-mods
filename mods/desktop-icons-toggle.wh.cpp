// ==WindhawkMod==
// @id              desktop-icons-toggle
// @name            Desktop Icons Toggle
// @description     Taskbar button to toggle desktop icons visibility
// @version         7.5
// @author          ChamPoing
// @include         explorer.exe
// @compilerOptions -lgdi32 -ldwmapi
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*...*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*...*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <shellapi.h>
#include <dwmapi.h>
#include <strsafe.h>
#include <thread>

enum ZBID { ZBID_DEFAULT = 0, ZBID_IMMERSIVE_NOTIFICATION = 4 };
typedef HWND(WINAPI* pCreateWindowInBand)(DWORD, LPCWSTR, LPCWSTR, DWORD, int, int, int, int, HWND, HMENU, HINSTANCE, LPVOID, DWORD);

#define TOGGLE_MSG    0x7402
#define BTN_W         38
#define BTN_H         38
#define WM_REPOS      (WM_APP + 1)
#define WM_CLOSE      (WM_APP + 2)
#define WM_RELOAD     (WM_APP + 3)
#define HOTKEY_ID     100

#define DOT_H         3   
#define DOT_Y_OFFSET  3   
#define DOT_R         2   

static HWND          g_hwndBtn           = nullptr;
static bool          g_iconsVisible      = true;
static bool          g_hover             = false;
static const WCHAR* g_label             = L"\U0001F5A5";
static int           g_offsetX           = 0;
static int           g_offsetY           = 0;
static HWINEVENTHOOK g_taskbarHook       = nullptr;
static UINT          g_taskbarCreatedMsg = 0;
static ULONGLONG     g_lastClick         = 0;

static void RepositionButton();

static HWND FindShellDefView() {
    HWND hProgman = FindWindowW(L"Progman", nullptr);
    if (hProgman) {
        HWND hv = FindWindowExW(hProgman, nullptr, L"SHELLDLL_DefView", nullptr);
        if (hv) return hv;
    }
    HWND hWorker = nullptr;
    while ((hWorker = FindWindowExW(nullptr, hWorker, L"WorkerW", nullptr)) != nullptr) {
        HWND hv = FindWindowExW(hWorker, nullptr, L"SHELLDLL_DefView", nullptr);
        if (hv) return hv;
    }
    return nullptr;
}

static void ToggleDesktopIcons() {
    HWND hDefView = FindShellDefView();
    if (!hDefView) return;
    SendMessageW(hDefView, WM_COMMAND, TOGGLE_MSG, 0);
    g_iconsVisible = !g_iconsVisible;
    if (g_hwndBtn) InvalidateRect(g_hwndBtn, nullptr, TRUE);
}

static void UpdateButtonLayered(HWND hwnd) {
    RECT rc;
    GetClientRect(hwnd, &rc);
    int w = rc.right, h = rc.bottom;
    if (w <= 0 || h <= 0) return;

    int dotW = !g_iconsVisible ? (g_hover ? 28 : 26) : (g_hover ? 18 : 8);
    BYTE dotAlpha = !g_iconsVisible ? (g_hover ? 255 : 220) : (g_hover ? 190 : 130);

    BITMAPINFO bmi = {};
    bmi.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth       = w;
    bmi.bmiHeader.biHeight      = -h;
    bmi.bmiHeader.biPlanes      = 1;
    bmi.bmiHeader.biBitCount    = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    void* pBits = nullptr;
    HDC hdcScreen = GetDC(nullptr);
    HDC hdcMem    = CreateCompatibleDC(hdcScreen);
    HBITMAP hBmp  = CreateDIBSection(hdcScreen, &bmi, DIB_RGB_COLORS, &pBits, nullptr, 0);
    HBITMAP hOld  = (HBITMAP)SelectObject(hdcMem, hBmp);

    memset(pBits, 0, w * h * 4);

    // Text rendering
    {
        LOGFONTW lf = {};
        lf.lfHeight  = 20;
        lf.lfCharSet = DEFAULT_CHARSET;
        lf.lfQuality = CLEARTYPE_QUALITY;
        StringCchCopyW(lf.lfFaceName, LF_FACESIZE, L"Segoe UI Emoji");
        HFONT hFont    = CreateFontIndirectW(&lf);
        HFONT hOldFont = (HFONT)SelectObject(hdcMem, hFont);

        SetBkMode(hdcMem, TRANSPARENT);
        SetTextColor(hdcMem, RGB(255, 255, 255));

        RECT txtRc = { 0, g_offsetY, w, h - DOT_H - DOT_Y_OFFSET - 2 + g_offsetY };
        DrawTextW(hdcMem, g_label, -1, &txtRc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

        SelectObject(hdcMem, hOldFont);
        DeleteObject(hFont);
    }

    // Indicator rendering
    {
        int dotX1 = w / 2 - dotW / 2;
        int dotX2 = w / 2 + dotW / 2;
        int dotY1 = h - DOT_Y_OFFSET - DOT_H;
        int dotY2 = h - DOT_Y_OFFSET;

        HBRUSH hbr = CreateSolidBrush(RGB(255, 255, 255));
        HPEN   hpn = CreatePen(PS_SOLID, 0, RGB(255, 255, 255));
        HBRUSH ob  = (HBRUSH)SelectObject(hdcMem, hbr);
        HPEN   op  = (HPEN)SelectObject(hdcMem, hpn);
        RoundRect(hdcMem, dotX1, dotY1, dotX2, dotY2, DOT_R, DOT_R);
        SelectObject(hdcMem, ob);
        SelectObject(hdcMem, op);
        DeleteObject(hbr); DeleteObject(hpn);
    }

    // Alpha blend calculations
    {
        DWORD* px = (DWORD*)pBits;
        int dotY1 = h - DOT_Y_OFFSET - DOT_H;
        int dotX1 = w / 2 - dotW / 2;
        int dotX2 = w / 2 + dotW / 2;

        for (int i = 0; i < w * h; i++) {
            DWORD pxv = px[i];
            BYTE r = (pxv >> 16) & 0xFF;
            BYTE g2= (pxv >>  8) & 0xFF;
            BYTE b = (pxv >>  0) & 0xFF;

            if (r == 0 && g2 == 0 && b == 0) {
                px[i] = 0;
                continue;
            }

            int row = i / w;
            int col = i % w;

            bool inDot = (row >= dotY1) && (col >= dotX1) && (col < dotX2);
            BYTE a = inDot ? dotAlpha : (BYTE)(g_hover ? 200 : 160);
            
            BYTE pr = (BYTE)((r * a) / 255);
            BYTE pg = (BYTE)((g2 * a) / 255);
            BYTE pb = (BYTE)((b * a) / 255);
            px[i] = ((DWORD)a << 24) | ((DWORD)pr << 16) | ((DWORD)pg << 8) | pb;
        }
    }

    POINT ptSrc = {0, 0};
    POINT ptDst;
    RECT  wrect;
    GetWindowRect(hwnd, &wrect);
    ptDst.x = wrect.left;
    ptDst.y = wrect.top;
    SIZE sz  = { w, h };
    BLENDFUNCTION blend = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
    UpdateLayeredWindow(hwnd, hdcScreen, &ptDst, &sz, hdcMem, &ptSrc, 0, &blend, ULW_ALPHA);

    SelectObject(hdcMem, hOld);
    DeleteObject(hBmp);
    DeleteDC(hdcMem);
    ReleaseDC(nullptr, hdcScreen);
}

static bool IsFullscreenWindow() {
    HWND hwnd = GetForegroundWindow();
    if (!hwnd || hwnd == g_hwndBtn) return false;

    RECT winRect;
    if (!GetWindowRect(hwnd, &winRect)) return false;

    HMONITOR hMon = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi = {};
    mi.cbSize = sizeof(mi);
    if (!GetMonitorInfoW(hMon, &mi)) return false;

    RECT mon = mi.rcMonitor;
    return winRect.left   <= mon.left  &&
           winRect.top    <= mon.top   &&
           winRect.right  >= mon.right &&
           winRect.bottom >= mon.bottom;
}

static void RepositionButton() {
    if (!g_hwndBtn) return;
    if (IsFullscreenWindow()) {
        ShowWindow(g_hwndBtn, SW_HIDE);
        return;
    }

    HWND hTaskbar = FindWindowW(L"Shell_TrayWnd", nullptr);
    if (!hTaskbar) return;

    RECT rc;
    GetWindowRect(hTaskbar, &rc);
    int barW = rc.right - rc.left;
    int barH = rc.bottom - rc.top;

    // Positionné par défaut à droite avant l'horloge
    int x = rc.left + barW - BTN_W - 220;
    x += g_offsetX;
    int y = rc.top + (barH - BTN_H) / 2;

    SetWindowPos(g_hwndBtn, HWND_TOPMOST, x, y, BTN_W, BTN_H, SWP_NOACTIVATE | SWP_SHOWWINDOW);
    UpdateButtonLayered(g_hwndBtn);
}

static void CALLBACK TaskbarEventProc(HWINEVENTHOOK, DWORD, HWND hwnd, LONG, LONG, DWORD, DWORD) {
    WCHAR cls[64] = {};
    GetClassNameW(hwnd, cls, ARRAYSIZE(cls));
    if (wcscmp(cls, L"Shell_TrayWnd") == 0 && g_hwndBtn) {
        PostMessageW(g_hwndBtn, WM_REPOS, 0, 0);
    }
}

static void RegisterTaskbarHook() {
    HWND hTaskbar = FindWindowW(L"Shell_TrayWnd", nullptr);
    if (!hTaskbar) return;
    DWORD pid = 0, tid = GetWindowThreadProcessId(hTaskbar, &pid);
    if (!tid) return;
    g_taskbarHook = SetWinEventHook(EVENT_OBJECT_LOCATIONCHANGE, EVENT_OBJECT_LOCATIONCHANGE, nullptr, TaskbarEventProc, pid, tid, WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS);
}

static LRESULT CALLBACK BtnWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == g_taskbarCreatedMsg) {
        if (g_taskbarHook) { UnhookWinEvent(g_taskbarHook); g_taskbarHook = nullptr; }
        RegisterTaskbarHook();
        RepositionButton();
        return 0;
    }
    switch (msg) {
        case WM_CREATE:
            RegisterHotKey(hwnd, HOTKEY_ID, MOD_CONTROL | MOD_ALT, 'D');
            SetTimer(hwnd, 1, 500, nullptr);
            return 0;

        case WM_TIMER:
            RepositionButton();
            return 0;

        case WM_HOTKEY:
            if (wParam == HOTKEY_ID) {
                ToggleDesktopIcons();
                UpdateButtonLayered(hwnd);
            }
            return 0;

        case WM_MOUSEMOVE:
            if (!g_hover) {
                g_hover = true;
                UpdateButtonLayered(hwnd);
                TRACKMOUSEEVENT tme = { sizeof(tme), TME_LEAVE, hwnd, 0 };
                TrackMouseEvent(&tme);
            }
            return 0;

        case WM_MOUSELEAVE:
            g_hover = false;
            UpdateButtonLayered(hwnd);
            return 0;

        case WM_ERASEBKGND:
            return 1;

        case WM_PAINT: {
            PAINTSTRUCT ps;
            BeginPaint(hwnd, &ps);
            EndPaint(hwnd, &ps);
            return 0;
        }

        case WM_LBUTTONUP: {
            ULONGLONG now = GetTickCount64();
            if (now - g_lastClick < 300) {
                ShellExecuteW(nullptr, L"open", L"explorer.exe", nullptr, nullptr, SW_SHOWNORMAL);
            } else {
                ToggleDesktopIcons();
                UpdateButtonLayered(hwnd);
            }
            g_lastClick = now;
            return 0;
        }

        case WM_RBUTTONUP: {
            POINT pt;
            GetCursorPos(&pt);
            HMENU hMenu = CreatePopupMenu();
            AppendMenuW(hMenu, MF_STRING, 1, g_iconsVisible ? L"Hide Desktop Icons" : L"Show Desktop Icons");
            AppendMenuW(hMenu, MF_SEPARATOR, 0, nullptr);
            AppendMenuW(hMenu, MF_STRING, 2, L"Open Desktop Folder");
            AppendMenuW(hMenu, MF_STRING, 3, L"Open Documents Folder");
            AppendMenuW(hMenu, MF_STRING, 4, L"Refresh Desktop");
            AppendMenuW(hMenu, MF_SEPARATOR, 0, nullptr);
            AppendMenuW(hMenu, MF_STRING, 5, L"About");
            
            SetForegroundWindow(hwnd);
            int cmd = (int)TrackPopupMenu(hMenu, TPM_RETURNCMD | TPM_RIGHTBUTTON, pt.x, pt.y, 0, hwnd, nullptr);
            DestroyMenu(hMenu);
            
            switch (cmd) {
                case 1: ToggleDesktopIcons(); UpdateButtonLayered(hwnd); break;
                case 2: ShellExecuteW(nullptr, L"open", L"shell:desktop",  nullptr, nullptr, SW_SHOWNORMAL); break;
                case 3: ShellExecuteW(nullptr, L"open", L"shell:personal", nullptr, nullptr, SW_SHOWNORMAL); break;
                case 4: { HWND hv = FindShellDefView(); if (hv) SendMessageW(hv, WM_COMMAND, 41504, 0); break; }
                case 5: MessageBoxW(hwnd, L"Desktop Icons Toggle v7.5\nBy ChamPoing", L"About", MB_OK | MB_ICONINFORMATION); break;
            }
            return 0;
        }

        case WM_SETCURSOR:
            SetCursor(LoadCursorW(nullptr, IDC_HAND));
            return TRUE;

        case WM_REPOS:
            RepositionButton();
            return 0;

        case WM_RELOAD:
            RepositionButton();
            UpdateButtonLayered(hwnd);
            return 0;

        case WM_CLOSE:
            KillTimer(hwnd, 1);
            if (g_taskbarHook) { UnhookWinEvent(g_taskbarHook); g_taskbarHook = nullptr; }
            DestroyWindow(hwnd);
            return 0;

        case WM_DESTROY:
            UnregisterHotKey(hwnd, HOTKEY_ID);
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

static void LoadSettings() {
    g_offsetX = Wh_GetIntSetting(L"offsetX");
    g_offsetY = Wh_GetIntSetting(L"offsetY");
}

static void BtnThread() {
    Sleep(800);
    g_taskbarCreatedMsg = RegisterWindowMessageW(L"TaskbarCreated");

    HINSTANCE hInst = GetModuleHandleW(nullptr);
    WNDCLASSEXW wc  = {};
    wc.cbSize        = sizeof(wc);
    wc.lpfnWndProc   = BtnWndProc;
    wc.hInstance     = hInst;
    wc.lpszClassName = L"WhDeskToggle75";
    wc.hCursor       = LoadCursorW(nullptr, IDC_HAND);
    RegisterClassExW(&wc);

    HMODULE hUser = GetModuleHandleW(L"user32.dll");
    auto CreateWinInBand = hUser ? (pCreateWindowInBand)GetProcAddress(hUser, "CreateWindowInBand") : nullptr;

    DWORD exStyle = WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_TOPMOST | WS_EX_NOACTIVATE;
    DWORD style   = WS_POPUP | WS_VISIBLE;

    if (CreateWinInBand) {
        g_hwndBtn = CreateWinInBand(exStyle, L"WhDeskToggle75", nullptr, style, 0, 0, BTN_W, BTN_H, nullptr, nullptr, hInst, nullptr, ZBID_IMMERSIVE_NOTIFICATION);
    }
    if (!g_hwndBtn) {
        g_hwndBtn = CreateWindowExW(exStyle, L"WhDeskToggle75", nullptr, style, 0, 0, BTN_W, BTN_H, nullptr, nullptr, hInst, nullptr);
    }
    if (!g_hwndBtn) return;

    UpdateButtonLayered(g_hwndBtn);
    RegisterTaskbarHook();
    RepositionButton();

    MSG msg;
    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    UnregisterClassW(L"WhDeskToggle75", hInst);
}

static std::thread* g_thread = nullptr;

BOOL Wh_ModInit() {
    LoadSettings();
    g_thread = new std::thread(BtnThread);
    return TRUE;
}

void Wh_ModUninit() {
    if (g_hwndBtn) SendMessageW(g_hwndBtn, WM_CLOSE, 0, 0);
    if (g_thread) {
        if (g_thread->joinable()) g_thread->join();
        delete g_thread;
        g_thread = nullptr;
    }
    g_hwndBtn = nullptr;
}

void Wh_ModSettingsChanged() {
    LoadSettings();
    if (g_hwndBtn) PostMessageW(g_hwndBtn, WM_RELOAD, 0, 0);
}
