// ==WindhawkMod==
// @id              classic-tray-keyboard-layout-indicator
// @name            Classic Tray Keyboard Layout Indicator
// @description     Tray indicator for keyboard layout, similar to internat.exe from Windows 2000
// @version         6.1.0
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
On right click a menu with layout selection is displayed.

- **Left click** — cycle through installed layouts
- **Right click** — pick layout from list

The mod may go out on sync in elevated windows if you're using more than two layouts.

**Note:** This mod only works with the classic taskbar.
*/
// ==/WindhawkModReadme==

#include <windows.h>
#include <shellapi.h>
#include <vector>
#include <string>

#define WM_TRAYICON       (WM_USER + 1)
#define WM_MOD_QUIT       (WM_USER + 2)
#define WM_LAYOUT_CHANGED (WM_USER + 3)

#define TRAY_ICON_UID   7654
#define IDM_LAYOUT_BASE 1000
#define TIMER_SWITCH    1
#define TIMER_CHECK     2

static HWND            g_hwnd         = nullptr;
static HKL             g_currentHKL   = nullptr;
static HKL             g_targetHKL    = nullptr;
static DWORD           g_switchStart  = 0;
static HINSTANCE       g_hInstance    = nullptr;
static HANDLE          g_thread       = nullptr;
static HICON           g_hIcon        = nullptr;
static UINT            g_msgTaskbar   = 0;
static volatile bool   g_running      = true;
static bool            g_iconAdded    = false;
static DWORD           g_lastChange   = 0;

#define SWITCH_TIMEOUT 2000

bool IsTaskbarProcess() {
    HWND hTaskbar = FindWindowW(L"Shell_TrayWnd", nullptr);
    if (!hTaskbar) return false;
    DWORD pid = 0;
    GetWindowThreadProcessId(hTaskbar, &pid);
    return pid == GetCurrentProcessId();
}

bool IsModernTaskbar() {
    HWND hTaskbar = FindWindowW(L"Shell_TrayWnd", nullptr);
    if (!hTaskbar) return false;
    return FindWindowExW(hTaskbar, nullptr, L"Windows.UI.Core.CoreWindow", nullptr) != nullptr;
}

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
    if (fg) {
        DWORD tid = GetWindowThreadProcessId(fg, nullptr);
        if (tid) {
            HKL h = GetKeyboardLayout(tid);
            if (h) return h;
        }
    }
    HKL h = GetKeyboardLayout(0);
    if (h) return h;
    auto v = GetLayouts();
    if (!v.empty()) return v[0];
    return nullptr;
}

HKL GetNextLayout() {
    auto v = GetLayouts();
    if (v.empty()) return nullptr;
    for (size_t i = 0; i < v.size(); i++)
        if (v[i] == g_currentHKL) return v[(i + 1) % v.size()];
    return v[0];
}

void SimulateLayoutHotkey() {
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
    if (hotkey == 3) return;
    BYTE mod = (hotkey == 2) ? VK_CONTROL : VK_MENU;
    INPUT in[4] = {};
    in[0].type = in[1].type = in[2].type = in[3].type = INPUT_KEYBOARD;
    in[0].ki.wVk = mod;
    in[1].ki.wVk = VK_SHIFT;
    in[2].ki.wVk = VK_SHIFT; in[2].ki.dwFlags = KEYEVENTF_KEYUP;
    in[3].ki.wVk = mod;      in[3].ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(4, in, sizeof(INPUT));
}

bool IsSwitchingFromMenu() {
    if (!g_targetHKL) return false;
    if (GetTickCount() - g_switchStart > SWITCH_TIMEOUT) {
        g_targetHKL = nullptr;
        return false;
    }
    return true;
}

HICON CreateLayoutIcon(const std::wstring& text) {
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

NOTIFYICONDATAW MakeNid() {
    NOTIFYICONDATAW nid = {};
    nid.cbSize = sizeof(nid);
    nid.hWnd = g_hwnd;
    nid.uID = TRAY_ICON_UID;
    return nid;
}

void DeleteTrayIcon() {
    NOTIFYICONDATAW nid = MakeNid();
    Shell_NotifyIconW(NIM_DELETE, &nid);
    g_iconAdded = false;
}

void SetTrayIcon(HKL hkl) {
    if (!hkl || !g_hwnd) return;
    if (hkl == g_currentHKL && g_iconAdded) return;
    
    HICON hNew = CreateLayoutIcon(GetLayoutCode(hkl));
    NOTIFYICONDATAW nid = MakeNid();
    nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    nid.uCallbackMessage = WM_TRAYICON;
    nid.hIcon = hNew;
    wcsncpy_s(nid.szTip, GetLayoutName(hkl).c_str(), _TRUNCATE);
    
    if (g_iconAdded) {
        Shell_NotifyIconW(NIM_MODIFY, &nid);
    } else {
        Shell_NotifyIconW(NIM_DELETE, &nid);
        if (Shell_NotifyIconW(NIM_ADD, &nid))
            g_iconAdded = true;
    }
    
    if (g_hIcon) DestroyIcon(g_hIcon);
    g_hIcon = hNew;
    g_currentHKL = hkl;
}

void ShowMenu() {
    HMENU hm = CreatePopupMenu();
    auto v = GetLayouts();
    for (size_t i = 0; i < v.size(); i++) {
        UINT flags = MF_STRING | ((v[i] == g_currentHKL) ? MF_CHECKED : 0);
        AppendMenuW(hm, flags, IDM_LAYOUT_BASE + i, GetLayoutName(v[i]).c_str());
    }
    POINT pt;
    GetCursorPos(&pt);
    SetForegroundWindow(g_hwnd);
    TrackPopupMenu(hm, TPM_RIGHTBUTTON | TPM_BOTTOMALIGN, pt.x, pt.y, 0, g_hwnd, nullptr);
    DestroyMenu(hm);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    if (msg == g_msgTaskbar && g_msgTaskbar != 0) {
        g_iconAdded = false;
        g_currentHKL = nullptr;
        SetTimer(hwnd, TIMER_CHECK, 1000, nullptr);
        return 0;
    }

    switch (msg) {
    case WM_TIMER:
        if (wp == TIMER_CHECK) {
            KillTimer(hwnd, TIMER_CHECK);
            if (IsModernTaskbar()) {
                PostMessageW(hwnd, WM_MOD_QUIT, 0, 0);
            } else {
                SetTrayIcon(GetCurrentLayout());
            }
        }
        else if (wp == TIMER_SWITCH) {
            KillTimer(hwnd, TIMER_SWITCH);
            if (IsSwitchingFromMenu() && g_currentHKL != g_targetHKL) {
                SimulateLayoutHotkey();
            } else {
                g_targetHKL = nullptr;
            }
        }
        return 0;
    case WM_LAYOUT_CHANGED: {
        HKL hkl = (HKL)lp;
        if (hkl) {
            SetTrayIcon(hkl);
            if (IsSwitchingFromMenu() && g_currentHKL != g_targetHKL) {
                SetTimer(hwnd, TIMER_SWITCH, 80, nullptr);
            } else {
                g_targetHKL = nullptr;
            }
        }
        return 0;
    }
    case WM_TRAYICON:
        if (LOWORD(lp) == WM_LBUTTONUP) {
            g_targetHKL = nullptr;
            SimulateLayoutHotkey();
        }
        else if (LOWORD(lp) == WM_RBUTTONUP) {
            ShowMenu();
        }
        return 0;
    case WM_COMMAND:
        if (LOWORD(wp) >= IDM_LAYOUT_BASE) {
            auto v = GetLayouts();
            size_t idx = LOWORD(wp) - IDM_LAYOUT_BASE;
            if (idx < v.size()) {
                HKL target = v[idx];
                if (target != g_currentHKL) {
                    g_targetHKL = target;
                    g_switchStart = GetTickCount();
                    SimulateLayoutHotkey();
                }
            }
        }
        return 0;
    case WM_MOD_QUIT:
        KillTimer(hwnd, TIMER_SWITCH);
        KillTimer(hwnd, TIMER_CHECK);
        DeleteTrayIcon();
        if (g_hIcon) { DestroyIcon(g_hIcon); g_hIcon = nullptr; }
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProcW(hwnd, msg, wp, lp);
}

DWORD WINAPI TrayThread(LPVOID) {
    // Ждём появления таскбара
    for (int i = 0; i < 60 && g_running; i++) {
        if (FindWindowW(L"Shell_TrayWnd", nullptr)) break;
        Sleep(500);
    }
    if (!g_running) return 0;
    
    // Проверяем, что это наш процесс
    if (!IsTaskbarProcess()) return 0;
    
    WNDCLASSEXW wc = {sizeof(wc)};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = g_hInstance;
    wc.lpszClassName = L"KbdLayoutTray_WH";
    RegisterClassExW(&wc);
    
    g_msgTaskbar = RegisterWindowMessageW(L"TaskbarCreated");
    
    g_hwnd = CreateWindowExW(WS_EX_TOOLWINDOW, L"KbdLayoutTray_WH", nullptr, WS_POPUP,
                              0, 0, 0, 0, nullptr, nullptr, g_hInstance, nullptr);
    if (!g_hwnd) return 1;
    
    if (g_msgTaskbar)
        ChangeWindowMessageFilterEx(g_hwnd, g_msgTaskbar, MSGFLT_ALLOW, nullptr);
    
    if (IsModernTaskbar()) {
        DestroyWindow(g_hwnd);
        UnregisterClassW(L"KbdLayoutTray_WH", g_hInstance);
        g_hwnd = nullptr;
        return 0;
    }
    
    SetTrayIcon(GetCurrentLayout());
    
    MSG msg;
    while (g_running && GetMessageW(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    
    DestroyWindow(g_hwnd);
    UnregisterClassW(L"KbdLayoutTray_WH", g_hInstance);
    g_hwnd = nullptr;
    return 0;
}

using InvalidateRect_t = BOOL(WINAPI*)(HWND, const RECT*, BOOL);
InvalidateRect_t InvalidateRect_Original;

BOOL WINAPI InvalidateRect_Hook(HWND hWnd, const RECT* lpRect, BOOL bErase) {
    if (hWnd && g_hwnd) {
        wchar_t cls[64] = {};
        GetClassNameW(hWnd, cls, 64);
        if (wcscmp(cls, L"TrayInputIndicatorWClass") == 0) {
            DWORD now = GetTickCount();
            if (now - g_lastChange > 50) {
                g_lastChange = now;
                HKL hkl = GetCurrentLayout();
                if (hkl && hkl != g_currentHKL) {
                    PostMessageW(g_hwnd, WM_LAYOUT_CHANGED, 0, (LPARAM)hkl);
                } else {
                    HKL next = GetNextLayout();
                    if (next && next != g_currentHKL) {
                        PostMessageW(g_hwnd, WM_LAYOUT_CHANGED, 0, (LPARAM)next);
                    }
                }
            }
        }
    }
    return InvalidateRect_Original(hWnd, lpRect, bErase);
}

BOOL Wh_ModInit() {
    g_hInstance = GetModuleHandleW(nullptr);
    g_running = true;
    g_iconAdded = false;
    g_currentHKL = nullptr;
    g_targetHKL = nullptr;
    g_switchStart = 0;
    g_hIcon = nullptr;
    g_lastChange = 0;

    Wh_SetFunctionHook((void*)InvalidateRect, (void*)InvalidateRect_Hook, (void**)&InvalidateRect_Original);

    g_thread = CreateThread(nullptr, 0, TrayThread, nullptr, 0, nullptr);
    return TRUE;
}

void Wh_ModUninit() {
    g_running = false;
    if (g_hwnd) PostMessageW(g_hwnd, WM_MOD_QUIT, 0, 0);
    if (g_thread) {
        WaitForSingleObject(g_thread, 2000);
        CloseHandle(g_thread);
        g_thread = nullptr;
    }
}
