// ==WindhawkMod==
// @id              notification-bell-highlighter
// @name            Notification Bell Highlighter
// @description     Pulsing, bright overlay with a counter on the taskbar notification bell
// @version         0.3
// @author          mwalker212
// @github          https://github.com/mwalker212
// @include         explorer.exe
// @compilerOptions -lole32 -loleaut32 -luuid -lgdi32 -lshell32
// ==/WindhawkMod==
// ==WindhawkModReadme==
/*
# Notification Bell Highlighter
Makes the Windows notification bell more obvious when notifications are waiting.
Draws a pulsing, bright overlay with an optional counter over the taskbar notification bell when notifications are pending.

Settings for size, position, colour, pulse rate and counter visibility.

![Screenshot](https://raw.githubusercontent.com/mwalker212/windhawk-assets/refs/heads/main/bell-notification.gif)

Windows 11 only. Tested on 26H2 build 26300.9711.

Limitations:
Overlay rather than restyled icon
Primary taskbar only
English accessible name
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- sizePercent: 100
  $name: Overlay size (% of taskbar button height)
- color: "FF2D2D"
  $name: Colour (hex RRGGBB)
- pulseMs: 1100
  $name: Pulse period (milliseconds)
- showCount: true
  $name: Show notification count
- offsetX: 10
  $name: Horizontal offset (px)
- offsetY: 0
  $name: Vertical offset (px)
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <shellapi.h>
#include <uiautomation.h>
#include <cmath>
#include <string>

struct Settings {
    int sizePercent = 100;
    COLORREF color = RGB(255, 45, 45);
    int pulseMs = 1100;
    bool showCount = true;
    int offsetX = 10;
    int offsetY = 0;
} g;

static HANDLE g_thread = nullptr;
static HWND g_wnd = nullptr;
static volatile LONG g_relayout = 1;
static int g_count = 0;
static int g_curSize = 0;
static HDC g_memDC = nullptr;
static HBITMAP g_dib = nullptr;
static HGDIOBJ g_oldBmp = nullptr;
static int g_dibSize = 0, g_dibCount = -1;
static COLORREF g_dibColor = 0;
static bool g_dibShowCount = false;
static BYTE g_alpha = 255;

static void LoadSettings() {
    g.sizePercent = Wh_GetIntSetting(L"sizePercent");
    if (g.sizePercent < 50) g.sizePercent = 50;
    g.pulseMs = Wh_GetIntSetting(L"pulseMs");
    if (g.pulseMs < 200) g.pulseMs = 200;
    g.showCount = Wh_GetIntSetting(L"showCount") != 0;
    g.offsetX = Wh_GetIntSetting(L"offsetX");
    g.offsetY = Wh_GetIntSetting(L"offsetY");

    PCWSTR c = Wh_GetStringSetting(L"color");
    unsigned long v = wcstoul(c, nullptr, 16);
    Wh_FreeStringSetting(c);
    g.color = RGB((v >> 16) & 0xFF, (v >> 8) & 0xFF, v & 0xFF);
}

static int ParseCount(const wchar_t* name) {
    std::wstring s = name;
    for (auto& ch : s) ch = towlower(ch);
    size_t p = s.find(L"new notification");
    if (p == std::wstring::npos) return 0;      // "no new notifications" etc.
    size_t e = p;
    while (e > 0 && s[e - 1] == L' ') e--;
    size_t b = e;
    while (b > 0 && iswdigit(s[b - 1])) b--;
    if (b == e) return 0;
    return _wtoi(s.substr(b, e - b).c_str());
}

// Finds the notification-centre button in the taskbar via UI Automation.
static bool QueryBell(IUIAutomation* uia, RECT* rc, int* count) {
    HWND tray = FindWindowW(L"Shell_TrayWnd", nullptr);
    if (!tray || !IsWindowVisible(tray)) return false;

    IUIAutomationElement* root = nullptr;
    if (FAILED(uia->ElementFromHandle(tray, &root)) || !root) return false;

    VARIANT v;
    VariantInit(&v);
    v.vt = VT_BSTR;
    v.bstrVal = SysAllocString(L"Notification");
    IUIAutomationCondition* cond = nullptr;
        // 1 = IgnoreCase, 2 = MatchSubstring (numeric because older headers lack the enum name)
    HRESULT hr = uia->CreatePropertyConditionEx(
        UIA_NamePropertyId, v, (PropertyConditionFlags)(1 | 2), &cond);
    VariantClear(&v);

    bool found = false;
    if (SUCCEEDED(hr) && cond) {
        IUIAutomationElementArray* arr = nullptr;
        if (SUCCEEDED(root->FindAll(TreeScope_Descendants, cond, &arr)) && arr) {
            int n = 0;
            arr->get_Length(&n);
            for (int i = 0; i < n && !found; i++) {
                IUIAutomationElement* el = nullptr;
                if (FAILED(arr->GetElement(i, &el)) || !el) continue;
                BSTR name = nullptr;
                RECT r{};
                if (SUCCEEDED(el->get_CurrentName(&name)) && name &&
                    SUCCEEDED(el->get_CurrentBoundingRectangle(&r)) &&
                    r.right > r.left && r.bottom > r.top) {
                    static std::wstring lastName;
                    if (lastName != name) {
                        lastName = name;
                        Wh_Log(L"Bell element name: %s", name);
                    }
                    *rc = r;
                    *count = ParseCount(name);
                    found = true;
                }
                if (name) SysFreeString(name);
                el->Release();
            }
            arr->Release();
        }
        cond->Release();
    }
    root->Release();
    return found;
}

static void RenderOverlay(HWND hwnd, BYTE alpha) {
    int size = g_curSize;
    if (size <= 0) return;
    if (!g_memDC) g_memDC = CreateCompatibleDC(nullptr);

    if (!g_dib || size != g_dibSize || g_count != g_dibCount ||
        g.color != g_dibColor || g.showCount != g_dibShowCount) {
        if (g_dib) {
            SelectObject(g_memDC, g_oldBmp);
            DeleteObject(g_dib);
            g_dib = nullptr;
        }
        BITMAPINFO bi{};
        bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bi.bmiHeader.biWidth = size;
        bi.bmiHeader.biHeight = -size;          // top-down
        bi.bmiHeader.biPlanes = 1;
        bi.bmiHeader.biBitCount = 32;
        bi.bmiHeader.biCompression = BI_RGB;
        DWORD* bits = nullptr;
        g_dib = CreateDIBSection(g_memDC, &bi, DIB_RGB_COLORS, (void**)&bits, nullptr, 0);
        if (!g_dib || !bits) return;
        HGDIOBJ prev = SelectObject(g_memDC, g_dib);
        if (!g_oldBmp) g_oldBmp = prev;

        // Anti-aliased filled circle, premultiplied alpha
        double r = size / 2.0;
        int cr = GetRValue(g.color), cg = GetGValue(g.color), cb = GetBValue(g.color);
        for (int y = 0; y < size; y++) {
            for (int x = 0; x < size; x++) {
                double dx = x + 0.5 - r, dy = y + 0.5 - r;
                double cov = r - sqrt(dx * dx + dy * dy);
                if (cov < 0) cov = 0;
                if (cov > 1) cov = 1;
                int a = (int)(cov * 255 + 0.5);
                bits[y * size + x] = (a << 24) | ((cr * a / 255) << 16) |
                                     ((cg * a / 255) << 8) | (cb * a / 255);
            }
        }

        // Count text (GDI leaves the alpha channel alone; text sits inside the opaque area)
        if (g.showCount && g_count > 0) {
            wchar_t txt[16];
            if (g_count > 99) wcscpy(txt, L"99+");
            else wsprintfW(txt, L"%d", g_count);
            HFONT f = CreateFontW(-(size * 55 / 100), 0, 0, 0, FW_BOLD, 0, 0, 0,
                                  DEFAULT_CHARSET, 0, 0, ANTIALIASED_QUALITY, 0,
                                  L"Segoe UI");
            HGDIOBJ oldF = SelectObject(g_memDC, f);
            SetBkMode(g_memDC, TRANSPARENT);
            SetTextColor(g_memDC, RGB(255, 255, 255));
            RECT tr = {0, 0, size, size};
            DrawTextW(g_memDC, txt, -1, &tr, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
            SelectObject(g_memDC, oldF);
            DeleteObject(f);
        }
        GdiFlush();

        g_dibSize = size;
        g_dibCount = g_count;
        g_dibColor = g.color;
        g_dibShowCount = g.showCount;
    }

    SIZE sz = {size, size};
    POINT src = {0, 0};
    BLENDFUNCTION bf = {AC_SRC_OVER, 0, alpha, AC_SRC_ALPHA};
    UpdateLayeredWindow(hwnd, nullptr, nullptr, &sz, g_memDC, &src, 0, &bf, ULW_ALPHA);
}

static void Reposition(HWND hwnd, const RECT& rc, int count) {
    bool hide = count <= 0;
    QUERY_USER_NOTIFICATION_STATE qs;
    if (SUCCEEDED(SHQueryUserNotificationState(&qs)) &&
        (qs == QUNS_BUSY || qs == QUNS_RUNNING_D3D_FULL_SCREEN ||
         qs == QUNS_PRESENTATION_MODE)) {
        hide = true;   // don't draw over fullscreen apps
    }
    if (hide) {
        ShowWindow(hwnd, SW_HIDE);
        return;
    }

    int h = rc.bottom - rc.top;
    int size = h * g.sizePercent / 100;
    int x = rc.right - size + g.offsetX;
    int y = rc.top + (h - size) / 2 + g.offsetY;

    g_curSize = size;
    SetWindowPos(hwnd, HWND_TOPMOST, x, y, size, size,
                 SWP_NOACTIVATE | SWP_SHOWWINDOW);
    RenderOverlay(hwnd, g_alpha);
}

static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    static IUIAutomation* uia = nullptr;
    switch (msg) {
    case WM_CREATE:
        CoCreateInstance(CLSID_CUIAutomation, nullptr, CLSCTX_INPROC_SERVER,
                         IID_IUIAutomation, (void**)&uia);
        SetTimer(hwnd, 1, 33, nullptr);    // pulse animation
        SetTimer(hwnd, 2, 700, nullptr);   // poll bell state
        return 0;

    case WM_TIMER:
        if (wp == 1) {
            if (IsWindowVisible(hwnd)) {
                double t = (GetTickCount() % g.pulseMs) / (double)g.pulseMs;
                double k = 0.5 + 0.5 * sin(t * 6.2831853);
                g_alpha = (BYTE)(110 + 145 * k);
                RenderOverlay(hwnd, g_alpha);
            }
        } else if (wp == 2 && uia) {
            RECT rc{};
            int count = 0;
            if (QueryBell(uia, &rc, &count)) {
                g_count = count;
                Reposition(hwnd, rc, count);
            } else {
                ShowWindow(hwnd, SW_HIDE);
            }
        }
        return 0;

    case WM_ERASEBKGND:
        return 1;
    case WM_NCHITTEST:
        return HTTRANSPARENT;          // clicks fall through to the real bell
    case WM_CLOSE:
        DestroyWindow(hwnd);
        return 0;
    case WM_DESTROY:
        KillTimer(hwnd, 1);
        KillTimer(hwnd, 2);
        if (uia) { uia->Release(); uia = nullptr; }
        if (g_memDC) {
            if (g_oldBmp) SelectObject(g_memDC, g_oldBmp);
            if (g_dib) DeleteObject(g_dib);
            DeleteDC(g_memDC);
            g_memDC = nullptr; g_dib = nullptr; g_oldBmp = nullptr;
        }
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProcW(hwnd, msg, wp, lp);
}

static DWORD WINAPI OverlayThread(LPVOID) {
    CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);

    WNDCLASSW wc{};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = GetModuleHandleW(nullptr);
    wc.lpszClassName = L"WhBellHighlighter";
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    RegisterClassW(&wc);

    g_wnd = CreateWindowExW(
        WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE |
            WS_EX_TRANSPARENT,
        wc.lpszClassName, L"", WS_POPUP, 0, 0, 10, 10, nullptr, nullptr,
        wc.hInstance, nullptr);

    if (g_wnd) {
        MSG m;
        while (GetMessageW(&m, nullptr, 0, 0) > 0) {
            TranslateMessage(&m);
            DispatchMessageW(&m);
        }
    }

    g_wnd = nullptr;
    UnregisterClassW(wc.lpszClassName, wc.hInstance);
    CoUninitialize();
    return 0;
}

BOOL Wh_ModInit() {
    LoadSettings();
    g_thread = CreateThread(nullptr, 0, OverlayThread, nullptr, 0, nullptr);
    return g_thread != nullptr;
}

void Wh_ModSettingsChanged() {
    LoadSettings();
    InterlockedExchange(&g_relayout, 1);
}

void Wh_ModUninit() {
    if (g_thread) {
        if (g_wnd) PostMessageW(g_wnd, WM_CLOSE, 0, 0);
        WaitForSingleObject(g_thread, 3000);
        CloseHandle(g_thread);
        g_thread = nullptr;
    }
}
