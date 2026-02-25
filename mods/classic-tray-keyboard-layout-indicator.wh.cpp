// ==WindhawkMod==
// @id              classic-tray-keyboard-layout-indicator
// @name            Classic Tray Keyboard Layout Indicator
// @description     Tray indicator for keyboard layout, similar to internat.exe from Windows 2000
// @version         1.0.0
// @author          Anixx
// @github          https://github.com/Anixx
// @include         explorer.exe
// @compilerOptions -lgdi32 -luser32 -lshell32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Keyboard Layout Tray Indicator

![Screenshot](https://i.imgur.com/L4i2WhE.png)

Displays current keyboard layout in the system tray as a two-letter code, 
in the style of Windows 2000. This is an alternative to the default indicator.

- **Left click** — cycle through installed layouts
- **Right click** — pick layout from list
*/
// ==/WindhawkModReadme==

#include <windows.h>
#include <shellapi.h>
#include <vector>
#include <string>

#define WM_TRAYICON     (WM_USER + 1)
#define WM_MOD_QUIT     (WM_USER + 2)
#define WM_REFRESH      (WM_USER + 3)

#define ID_TIMER        1
#define IDM_LAYOUT_BASE 1000

static const GUID g_iconGuid = 
    {0x8d5a8d5a, 0x1234, 0x5678, {0x9a, 0xbc, 0xde, 0xf0, 0x12, 0x34, 0x56, 0x78}};

static HWND            g_hwnd       = nullptr;
static HKL             g_currentHKL = nullptr;
static HINSTANCE       g_hInstance  = nullptr;
static HWINEVENTHOOK   g_eventHook  = nullptr;
static HANDLE          g_thread     = nullptr;
static NOTIFYICONDATAW g_nid        = {};
static UINT            g_msgTaskbar = 0;
static bool            g_iconAdded  = false;
static volatile bool   g_running    = true;
static int             g_checks     = 0;

// ─── Helpers ──────────────────────────────────

std::wstring GetLayoutCode(HKL hkl) {
    wchar_t buf[8] = {};
    LANGID lang = LOWORD((ULONG_PTR)hkl);
    if (GetLocaleInfoW(MAKELCID(lang, SORT_DEFAULT), LOCALE_SISO639LANGNAME, buf, 8)) {
        CharUpperW(buf);
        buf[2] = 0;
        return buf;
    }
    swprintf_s(buf, L"%02X", lang & 0xFF);
    return buf;
}

std::wstring GetLayoutName(HKL hkl) {
    DWORD id = (DWORD)(ULONG_PTR)hkl;
    wchar_t key[80], val[128] = {};
    swprintf_s(key, L"SYSTEM\\CurrentControlSet\\Control\\Keyboard Layouts\\%08X",
               HIWORD(id) ? id : LOWORD(id));
    DWORD sz = sizeof(val);
    HKEY hk;
    if (RegOpenKeyExW(HKEY_LOCAL_MACHINE, key, 0, KEY_READ, &hk) == ERROR_SUCCESS) {
        RegQueryValueExW(hk, L"Layout Text", 0, 0, (BYTE*)val, &sz);
        RegCloseKey(hk);
    }
    if (val[0]) return val;
    LANGID lang = LOWORD((ULONG_PTR)hkl);
    if (GetLocaleInfoW(MAKELCID(lang, SORT_DEFAULT), LOCALE_SLANGUAGE, val, 128))
        return val;
    return GetLayoutCode(hkl);
}

std::vector<HKL> GetLayouts() {
    int n = GetKeyboardLayoutList(0, nullptr);
    if (n <= 0) return {};
    std::vector<HKL> v(n);
    GetKeyboardLayoutList(n, v.data());
    return v;
}

HKL GetCurrentLayout() {
    HWND fg = GetForegroundWindow();
    return GetKeyboardLayout(fg ? GetWindowThreadProcessId(fg, nullptr) : 0);
}

HKL GetNextLayout() {
    auto v = GetLayouts();
    if (v.empty()) return nullptr;
    HKL cur = GetCurrentLayout();
    for (size_t i = 0; i < v.size(); i++)
        if (v[i] == cur) return v[(i + 1) % v.size()];
    return v[0];
}

// ─── Layout switching ─────────────────────────

void SwitchLayout() {
    HKEY hk;
    int hotkey = 1;
    if (RegOpenKeyExW(HKEY_CURRENT_USER, L"Keyboard Layout\\Toggle", 0, KEY_READ, &hk) == ERROR_SUCCESS) {
        wchar_t val[8] = {};
        DWORD sz = sizeof(val);
        if (RegQueryValueExW(hk, L"Language Hotkey", 0, 0, (BYTE*)val, &sz) == ERROR_SUCCESS ||
            RegQueryValueExW(hk, L"Hotkey", 0, 0, (BYTE*)val, &sz) == ERROR_SUCCESS) {
            if (val[0] == L'2') hotkey = 2;
            else if (val[0] == L'3') hotkey = 3;
        }
        RegCloseKey(hk);
    }
    
    if (hotkey == 3) {
        HWND fg = GetForegroundWindow();
        HKL next = GetNextLayout();
        if (fg && next) PostMessageW(fg, WM_INPUTLANGCHANGEREQUEST, 0, (LPARAM)next);
        return;
    }
    
    BYTE mod = (hotkey == 2) ? VK_CONTROL : VK_MENU;
    INPUT in[4] = {};
    in[0].type = in[1].type = in[2].type = in[3].type = INPUT_KEYBOARD;
    in[0].ki.wVk = mod;
    in[1].ki.wVk = VK_SHIFT;
    in[2].ki.wVk = VK_SHIFT; in[2].ki.dwFlags = KEYEVENTF_KEYUP;
    in[3].ki.wVk = mod;      in[3].ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(4, in, sizeof(INPUT));
}

void SwitchToLayout(HKL target) {
    auto v = GetLayouts();
    if (v.size() <= 1) return;
    HKL cur = GetCurrentLayout();
    if (cur == target) return;
    
    size_t ci = 0, ti = 0;
    for (size_t i = 0; i < v.size(); i++) {
        if (v[i] == cur) ci = i;
        if (v[i] == target) ti = i;
    }
    size_t n = (ti >= ci) ? (ti - ci) : (v.size() - ci + ti);
    for (size_t i = 0; i < n; i++) { SwitchLayout(); Sleep(30); }
}

// ─── Icon ─────────────────────────────────────

HICON CreateIcon(const std::wstring& text) {
    HDC hdcScr = GetDC(nullptr);
    HDC hdc = CreateCompatibleDC(hdcScr);
    
    BITMAPINFO bmi = {};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = 16;
    bmi.bmiHeader.biHeight = -16;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    
    HBITMAP hbm = CreateDIBSection(hdc, &bmi, DIB_RGB_COLORS, nullptr, nullptr, 0);
    HBITMAP hbmMask = CreateBitmap(16, 16, 1, 1, nullptr);
    HBITMAP hbmOld = (HBITMAP)SelectObject(hdc, hbm);
    
    RECT rc = {0, 0, 16, 16};
    HBRUSH br = CreateSolidBrush(GetSysColor(COLOR_HIGHLIGHT));
    FillRect(hdc, &rc, br);
    DeleteObject(br);
    
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, GetSysColor(COLOR_HIGHLIGHTTEXT));
    
    LOGFONTW lf = {};
    lf.lfHeight = 11;
    lf.lfWeight = FW_BOLD;
    wcscpy_s(lf.lfFaceName, L"Arial");
    HFONT hf = CreateFontIndirectW(&lf);
    HFONT hfOld = (HFONT)SelectObject(hdc, hf);
    
    DrawTextW(hdc, text.c_str(), -1, &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    
    SelectObject(hdc, hfOld);
    DeleteObject(hf);
    SelectObject(hdc, hbmOld);
    
    ICONINFO ii = {TRUE, 0, 0, hbmMask, hbm};
    HICON icon = CreateIconIndirect(&ii);
    
    DeleteObject(hbm);
    DeleteObject(hbmMask);
    DeleteDC(hdc);
    ReleaseDC(nullptr, hdcScr);
    
    return icon;
}

// ─── Tray ─────────────────────────────────────

void InitNid(HWND hwnd) {
    memset(&g_nid, 0, sizeof(g_nid));
    g_nid.cbSize = sizeof(g_nid);
    g_nid.hWnd = hwnd;
    g_nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP | NIF_GUID;
    g_nid.uCallbackMessage = WM_TRAYICON;
    g_nid.guidItem = g_iconGuid;
}

void RemoveIcon() {
    NOTIFYICONDATAW nid = {};
    nid.cbSize = sizeof(nid);
    nid.uFlags = NIF_GUID;
    nid.guidItem = g_iconGuid;
    Shell_NotifyIconW(NIM_DELETE, &nid);
    
    if (g_nid.hIcon) {
        DestroyIcon(g_nid.hIcon);
        g_nid.hIcon = nullptr;
    }
    g_iconAdded = false;
}

void UpdateIcon(HKL hkl) {
    if (!hkl || !g_hwnd) return;
    g_currentHKL = hkl;
    
    HICON hNew = CreateIcon(GetLayoutCode(hkl));
    HICON hOld = g_nid.hIcon;
    
    g_nid.hIcon = hNew;
    wcsncpy_s(g_nid.szTip, GetLayoutName(hkl).c_str(), _TRUNCATE);
    
    if (g_iconAdded && Shell_NotifyIconW(NIM_MODIFY, &g_nid)) {
        if (hOld) DestroyIcon(hOld);
        return;
    }
    
    Shell_NotifyIconW(NIM_DELETE, &g_nid);
    g_iconAdded = Shell_NotifyIconW(NIM_ADD, &g_nid) != FALSE;
    if (hOld) DestroyIcon(hOld);
}

void RefreshIcon() {
    if (!g_hwnd) return;
    HKL hkl = GetCurrentLayout();
    if (hkl != g_currentHKL || !g_iconAdded)
        UpdateIcon(hkl);
}

void StartChecks() {
    if (!g_hwnd) return;
    g_checks = 5;
    SetTimer(g_hwnd, ID_TIMER, 50, nullptr);
    PostMessageW(g_hwnd, WM_REFRESH, 0, 0);
}

// ─── Menu ─────────────────────────────────────

void ShowMenu() {
    HMENU hm = CreatePopupMenu();
    auto v = GetLayouts();
    HKL cur = GetCurrentLayout();
    for (size_t i = 0; i < v.size(); i++) {
        UINT flags = MF_STRING | ((v[i] == cur) ? MF_CHECKED : 0);
        AppendMenuW(hm, flags, IDM_LAYOUT_BASE + i, GetLayoutName(v[i]).c_str());
    }
    POINT pt;
    GetCursorPos(&pt);
    SetForegroundWindow(g_hwnd);
    TrackPopupMenu(hm, TPM_RIGHTBUTTON | TPM_BOTTOMALIGN, pt.x, pt.y, 0, g_hwnd, nullptr);
    DestroyMenu(hm);
}

// ─── Event hook callback ──────────────────────

void CALLBACK EventProc(HWINEVENTHOOK, DWORD, HWND, LONG, LONG, DWORD, DWORD) {
    StartChecks();
}

// ─── WndProc ──────────────────────────────────

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    if (msg == g_msgTaskbar) {
        g_iconAdded = false;
        g_currentHKL = nullptr;
        RefreshIcon();
        return 0;
    }
    
    switch (msg) {
    case WM_REFRESH:
        RefreshIcon();
        return 0;
        
    case WM_TIMER:
        if (wp == ID_TIMER) {
            RefreshIcon();
            if (--g_checks <= 0)
                KillTimer(hwnd, ID_TIMER);
        }
        return 0;
        
    case WM_TRAYICON:
        if (LOWORD(lp) == WM_LBUTTONUP) {
            UpdateIcon(GetNextLayout());
            SwitchLayout();
            StartChecks();
        } else if (LOWORD(lp) == WM_RBUTTONUP) {
            ShowMenu();
        }
        return 0;
        
    case WM_COMMAND:
        if (LOWORD(wp) >= IDM_LAYOUT_BASE) {
            auto v = GetLayouts();
            size_t i = LOWORD(wp) - IDM_LAYOUT_BASE;
            if (i < v.size()) {
                UpdateIcon(v[i]);
                SwitchToLayout(v[i]);
                StartChecks();
            }
        }
        return 0;
        
    case WM_MOD_QUIT:
        KillTimer(hwnd, ID_TIMER);
        RemoveIcon();
        PostQuitMessage(0);
        return 0;
    }
    
    return DefWindowProcW(hwnd, msg, wp, lp);
}

// ─── Thread ───────────────────────────────────

DWORD WINAPI TrayThread(LPVOID) {
    WNDCLASSEXW wc = {sizeof(wc)};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = g_hInstance;
    wc.lpszClassName = L"KbdLayoutTray";
    RegisterClassExW(&wc);
    
    g_msgTaskbar = RegisterWindowMessageW(L"TaskbarCreated");
    
    g_hwnd = CreateWindowExW(WS_EX_TOOLWINDOW, L"KbdLayoutTray", nullptr, WS_POPUP,
                              0, 0, 0, 0, nullptr, nullptr, g_hInstance, nullptr);
    if (!g_hwnd) return 1;
    
    InitNid(g_hwnd);
    
    RemoveIcon();
    
    g_eventHook = SetWinEventHook(
        EVENT_SYSTEM_FOREGROUND, EVENT_OBJECT_FOCUS,
        nullptr, EventProc, 0, 0,
        WINEVENT_OUTOFCONTEXT);
    
    RefreshIcon();
    
    MSG msg;
    while (g_running && GetMessageW(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    
    if (g_eventHook) {
        UnhookWinEvent(g_eventHook);
        g_eventHook = nullptr;
    }
    
    DestroyWindow(g_hwnd);
    UnregisterClassW(L"KbdLayoutTray", g_hInstance);
    g_hwnd = nullptr;
    return 0;
}

// ─── Windhawk ─────────────────────────────────

BOOL Wh_ModInit() {
    g_hInstance = GetModuleHandleW(nullptr);
    g_running = true;
    
    NOTIFYICONDATAW nid = {};
    nid.cbSize = sizeof(nid);
    nid.uFlags = NIF_GUID;
    nid.guidItem = g_iconGuid;
    Shell_NotifyIconW(NIM_DELETE, &nid);
    
    g_thread = CreateThread(nullptr, 0, TrayThread, nullptr, 0, nullptr);
    return TRUE;
}

void Wh_ModUninit() {
    g_running = false;
    
    if (g_hwnd) {
        PostMessageW(g_hwnd, WM_MOD_QUIT, 0, 0);
    }
    
    if (g_thread) {
        WaitForSingleObject(g_thread, 5000);
        CloseHandle(g_thread);
        g_thread = nullptr;
    }
    
    NOTIFYICONDATAW nid = {};
    nid.cbSize = sizeof(nid);
    nid.uFlags = NIF_GUID;
    nid.guidItem = g_iconGuid;
    Shell_NotifyIconW(NIM_DELETE, &nid);
}
