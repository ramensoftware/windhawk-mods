// ==WindhawkMod==
// @id              microdisk
// @name            MicroDisk
// @description     Mini Disk info tray icon showing free and total space on hard disk.
// @version         1.0.1
// @author          allelimo
// @github          https://github.com/
// @donateUrl       https://
// @include         windhawk.exe
// @compilerOptions -lpdh -lshell32 -lgdi32 -luser32 -lole32 -luuid -ladvapi32 -ldwmapi
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*

# MicroDisk

![Screenshot](https://i.imgur.com/V83qvSc.png)

A lightweight tray icon that shows (etc.).

## How to Use

1. *Left-click* the tray icon to open the popup showing:
   - Total CPU usage and the top CPU-consuming process
   - Total GPU usage and the top GPU-consuming process
   - Total RAM usage and the top RAM-consuming process
2. *Click anywhere outside* the popup (or press *Esc*) to close it
3. *Hover* the tray icon to see a live free/total summary tooltip
4. *Right-click* the tray icon for options

## Configuration

Right-click the tray icon to change the refresh rate (0.3s / 0.5s / 1s / 3s).

## Changelog

# 1.0.0
- Initial release.
- Left-click to see (etc.).
- Right-click for options. Update interval adjustable in Settings.
*/
// ==/WindhawkModReadme==

// allelimo
// ==WindhawkModSettings==
/*
- taskBarOnTop: false
  $name: Taskbar on top
  $description: >-
    Select if the taskbar is on top of the screen.
- diskLetter: "C:\\"
  $name: Disk 
  $description: Letter of the disk to be checked (please use the format "C:\")
- iconHidden: true 
  $name: Systray icon hidden
  $description: >- 
    Select if the systray icon is hidden from the taskbar.
*/
// ==/WindhawkModSettings==

#define NOMINMAX
#include <windows.h>
#include <shellapi.h>
#include <shobjidl.h>
#include <propkey.h>
#include <dwmapi.h>
#include <pdh.h>
#include <stdlib.h>
#include <stdio.h>
//allelimo
#include <fileapi.h>
#include <iostream>

// DWM window corner preference (Windows 11). Declared here so the mod builds
// against older SDK headers; the call silently no-ops on Windows 10.
#ifndef DWMWA_WINDOW_CORNER_PREFERENCE
#define DWMWA_WINDOW_CORNER_PREFERENCE 33
#endif
#ifndef DWMWCP_ROUND
#define DWMWCP_ROUND 2
#endif

#ifndef MIN
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#endif

// ─── Constants ────────────────────────────────────────────────────────────────

#define TRAY_ICON_ID        1
#define WM_TRAY_CALLBACK    (WM_USER + 1)
#define WM_TIMER_ID         1

// Base (96-DPI) popup geometry — scaled at runtime via Sc().
#define POPUP_WIDTH         170  // allelimo 360 
#define POPUP_HEIGHT        80   // allelimo 112 then 140  then 48 then 72
#define POPUP_ROWS          2    // allelimo 3

#define MENU_OPEN_WINDHAWK   9000
#define MENU_INTERVAL_300MS  9100
#define MENU_INTERVAL_500MS  9101
#define MENU_INTERVAL_1S     9102
#define MENU_INTERVAL_3S     9103

// Stable GUID that gives our tray icon a process-independent identity.
static const GUID MICRODISK_TRAY_GUID =
    {0x2C4E8A1B, 0x7D3F, 0x4A6E, {0x8B, 0x9C, 0x1D, 0x2E, 0x3F, 0x4A, 0x5B, 0x6C}}; //allelimo
    

// ─── Globals ──────────────────────────────────────────────────────────────────

static HANDLE              g_trayThread   = nullptr;
static HWND                g_trayHwnd     = nullptr;
static HWND                g_popupHwnd    = nullptr;
static ULONGLONG           g_lastPopupCloseTime = 0;
static HINSTANCE           g_hInstance    = nullptr;
static WCHAR               g_windhawkPath[MAX_PATH] = {};
static WCHAR               g_ddoresDllPath[MAX_PATH] = {};

static DWORD               g_updateMs     = 1000;
static int                 g_dpi          = 96;   // popup DPI, refreshed per show
static ULONGLONG           g_totalPhys    = 0;    // total physical RAM, bytes

static HICON               g_iconEnabled  = nullptr;
static HFONT               g_hPopupFont   = nullptr;
static int                 g_fontDpi      = 0;

static UINT                g_taskbarCreatedMsg = 0;
static WCHAR               g_lastTip[128] = {};

// allelimo
int                 g_freeSpace = 1;
int                 g_totalSpace = 1;
//double              g_ratio;

// ─── DPI Helpers ──────────────────────────────────────────────────────────────

static int Sc(int v) { return MulDiv(v, g_dpi, 96); }

// ─── Font Helpers ─────────────────────────────────────────────────────────────

static void EnsureFont() {
    if (g_hPopupFont && g_fontDpi == g_dpi) return;
    if (g_hPopupFont) { DeleteObject(g_hPopupFont); g_hPopupFont = nullptr; }
    g_hPopupFont = CreateFontW(-Sc(13), 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
        ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
    g_fontDpi = g_dpi;
}

// allelimo
struct {
    bool taskBarOnTop;
    LPCWSTR diskLetter;
    bool iconHidden;
} g_settings;

// allelimo
void GetDiskInfo() {

    ULARGE_INTEGER freeAvailable, totalBytes, totalFree;

    // Call GetDiskFreeSpaceExA for the selected drive
    if (GetDiskFreeSpaceExW(g_settings.diskLetter, &freeAvailable, &totalBytes, &totalFree)) {
        std::cout << "Total Capacity: " << totalBytes.QuadPart / (1024 * 1024 * 1024) << " GB\n";
        std::cout << "Free Space Available to Caller: " << freeAvailable.QuadPart / (1024 * 1024 * 1024) << " GB\n";
        std::cout << "Total Free Space: " << totalFree.QuadPart / (1024 * 1024 * 1024) << " GB\n";

         g_freeSpace = totalFree.QuadPart / (1024 * 1024 * 1024);
         g_totalSpace = totalBytes.QuadPart / (1024 * 1024 * 1024);

    } else {
        std::cerr << "Failed to retrieve disk space.\n";
    }
}

// ─── Data Refresh ─────────────────────────────────────────────────────────────

static void RefreshData() {
   
    //allelimo
    GetDiskInfo();

    // Live summary tooltip — refresh only when the displayed numbers change.
    if (g_trayHwnd) {
        WCHAR gbS[8], totS[8], tip[128];

        swprintf_s(gbS, L"%d Gb", g_freeSpace);
        swprintf_s(totS, L"%d Gb", g_totalSpace);
        
        swprintf_s(tip, L"Free: %s   Total: %s", gbS, totS);

        if (wcscmp(tip, g_lastTip) != 0) {
            wcscpy_s(g_lastTip, tip);
            NOTIFYICONDATAW nid = {sizeof(nid)};
            nid.hWnd     = (HWND)g_trayHwnd;
            nid.uID      = TRAY_ICON_ID;
            nid.uFlags   = NIF_TIP | NIF_GUID | NIF_SHOWTIP;
            nid.guidItem = MICRODISK_TRAY_GUID;
            lstrcpynW(nid.szTip, tip, 128);
            Shell_NotifyIconW(NIM_MODIFY, &nid);
        }
    }
}

// ─── Popup Window Procedure ───────────────────────────────────────────────────

static LRESULT CALLBACK PopupWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_ACTIVATE:
            if (LOWORD(wParam) == WA_INACTIVE) {
                static DWORD lastDeactivateTick = 0;
                DWORD now = GetTickCount();
                if (now - lastDeactivateTick > 200) {
                    lastDeactivateTick = now;
                    ShowWindow(hWnd, SW_HIDE);
                    g_lastPopupCloseTime = GetTickCount64();
                }
            }
            return 0;

        case WM_PAINT: {
            // Snapshot stats under the lock, then paint without holding it.
            int totals[POPUP_ROWS], topPcts[POPUP_ROWS];
            WCHAR topNames[POPUP_ROWS][64];

            // allelimo
            totals[0] = g_freeSpace; topPcts[0] = 0; 
            totals[1] = g_totalSpace; topPcts[1] = 0; 

            static const PCWSTR kLabels[POPUP_ROWS]    = { L"Free", L"Total" };
            PCWSTR kEmptyText[POPUP_ROWS] = { g_settings.diskLetter, g_settings.diskLetter };
            COLORREF kTotalColors[POPUP_ROWS] = { RGB(128, 255, 128), RGB(0, 200, 255) };
        
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);

            HBRUSH bgBrush = CreateSolidBrush(RGB(32, 32, 32));
            RECT rc;
            GetClientRect(hWnd, &rc);
            FillRect(hdc, &rc, bgBrush);
            DeleteObject(bgBrush);

            HPEN borderPen = CreatePen(PS_SOLID, 1, RGB(64, 64, 64));
            HPEN oldPen = (HPEN)SelectObject(hdc, borderPen);
            HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, GetStockObject(NULL_BRUSH));
            Rectangle(hdc, 0, 0, rc.right, rc.bottom);
            SelectObject(hdc, oldPen);
            SelectObject(hdc, oldBrush);
            DeleteObject(borderPen);

            SetBkMode(hdc, TRANSPARENT);
            EnsureFont();
            SelectObject(hdc, g_hPopupFont);

            for (int row = 0; row < POPUP_ROWS; row++) {
                int y = Sc(12) + row * Sc(32);
                int totalVal = totals[row];
                int topPctVal = topPcts[row];
                PCWSTR topName = topNames[row];

                SetTextColor(hdc, RGB(140, 140, 140));
                WCHAR labelBuf[16];
                swprintf_s(labelBuf, L"%s:", kLabels[row]);
                TextOutW(hdc, Sc(12), y, labelBuf, (int)wcslen(labelBuf));

//allelimo
                // if (row < POPUP_ROWS){
                //     SetTextColor(hdc, kTotalColors[row]);
                //     WCHAR totalBuf[16];
                //     if (totalVal < 0)
                //         swprintf_s(totalBuf, L".. Gb");
                //     else
                //         swprintf_s(totalBuf, L"%d Gb", totalVal);
                //     TextOutW(hdc, Sc(60), y, totalBuf, (int)wcslen(totalBuf));
                //  } else {
                //      SetTextColor(hdc, kTotalColors[row]);
                //      WCHAR totalBuf[16];
                //      if (totalVal < 0)
                //          swprintf_s(totalBuf, L".. Gb");
                //      else
                //          swprintf_s(totalBuf, L"%d Gb", totalVal);
                //      TextOutW(hdc, Sc(60), y, totalBuf, (int)wcslen(totalBuf));

                // }

                SetTextColor(hdc, kTotalColors[row]);
                WCHAR totalBuf[16];
                if (totalVal < 0)
                    swprintf_s(totalBuf, L".. Gb");
                else
                    swprintf_s(totalBuf, L"%d Gb", totalVal);
                TextOutW(hdc, Sc(60), y, totalBuf, (int)wcslen(totalBuf));
               

                SetTextColor(hdc, RGB(220, 220, 220));
                if (topName[0] != L'\0' && topPctVal > 0) {
                    WCHAR lineBuf[256];
                    swprintf_s(lineBuf, L"%s  %d%%", topName, topPctVal);
                    TextOutW(hdc, Sc(130), y, lineBuf, (int)wcslen(lineBuf));
                } else if (totalVal >= 0) {
                    TextOutW(hdc, Sc(130), y, kEmptyText[row], (int)wcslen(kEmptyText[row]));
                } else {
                    PCWSTR collecting = L"Collecting...";
                    TextOutW(hdc, Sc(130), y, collecting, (int)wcslen(collecting));
                }
            }

            EndPaint(hWnd, &ps);
            return 0;
        }

        case WM_NCHITTEST: {
            LRESULT hit = DefWindowProcW(hWnd, msg, wParam, lParam);
            if (hit == HTCLIENT) return HTCAPTION;
            return hit;
        }

        case WM_KEYDOWN:
            if (wParam == VK_ESCAPE) ShowWindow(hWnd, SW_HIDE);
            break;

        case WM_DESTROY:
            InterlockedExchangePointer((PVOID*)&g_popupHwnd, nullptr);
            break;
    }
    return DefWindowProcW(hWnd, msg, wParam, lParam);
}

// ─── Show/Hide Popup ──────────────────────────────────────────────────────────

static void ShowPopup(HWND hTrayWnd) {
    if (!g_popupHwnd) {
        WNDCLASSW wc = {0};
        wc.lpfnWndProc = PopupWndProc;
        wc.hInstance = g_hInstance;
        wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
        wc.lpszClassName = L"MicroDiskPopupClass";
        wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
        if (!RegisterClassW(&wc) && GetLastError() != ERROR_CLASS_ALREADY_EXISTS) {
            Wh_Log(L"Popup RegisterClassW failed (%u)", GetLastError());
            return;
        }

        g_popupHwnd = CreateWindowExW(
            WS_EX_TOOLWINDOW | WS_EX_TOPMOST,
            wc.lpszClassName, L"MicroDisk",
            WS_POPUP | WS_BORDER,
            0, 0, Sc(POPUP_WIDTH), Sc(POPUP_HEIGHT),
            nullptr, nullptr, g_hInstance, nullptr);
        if (!g_popupHwnd) {
            UnregisterClassW(L"MicroDiskPopupClass", g_hInstance);
            return;
        }
    }

    if (!g_popupHwnd) return;

    // Refresh DPI for the monitor the icon lives on, then rebuild the font.
    UINT dpi = GetDpiForWindow(g_popupHwnd);
    g_dpi = dpi ? (int)dpi : 96;
    EnsureFont();

    NOTIFYICONIDENTIFIER nii = {sizeof(nii)};
    nii.hWnd = nullptr;
    nii.uID = 0;
    nii.guidItem = MICRODISK_TRAY_GUID;  // icon is registered with NIF_GUID
    RECT iconRect;
    int w = Sc(POPUP_WIDTH), h = Sc(POPUP_HEIGHT);
    int x, y;

    if (SUCCEEDED(Shell_NotifyIconGetRect(&nii, &iconRect))) {
        x = iconRect.left - w + (iconRect.right - iconRect.left) / 2;
        y = iconRect.top - h - Sc(4);
    } else {
        POINT pt;
        GetCursorPos(&pt);
        x = pt.x - w / 2;
        y = pt.y - h - Sc(4);
    }


// allelimo
    if (g_settings.taskBarOnTop){
        if(g_settings.iconHidden){
            SetWindowPos(g_popupHwnd, HWND_TOPMOST, x, y + 70, w, h, SWP_SHOWWINDOW);  // 100 per originale, per disk info vediamo 50. 70 ok da tray, 100 da taskbar
        }
        else{
            SetWindowPos(g_popupHwnd, HWND_TOPMOST, x, y + 100, w, h, SWP_SHOWWINDOW);  // 100 per originale, per disk info vediamo 50. 70 ok da tray, 100 da taskbar        
        }
    }
    else {
        SetWindowPos(g_popupHwnd, HWND_TOPMOST, x, y, w, h, SWP_SHOWWINDOW);
    }
   

    // Rounded corners on Windows 11 (silently ignored on Windows 10).
    int corner = DWMWCP_ROUND;
    DwmSetWindowAttribute(g_popupHwnd, DWMWA_WINDOW_CORNER_PREFERENCE,
        &corner, sizeof(corner));

    // Take foreground activation so a click anywhere outside the popup fires
    // WM_ACTIVATE/WA_INACTIVE and auto-hides it. SetFocus alone does not cross
    // the foreground boundary from the tray thread, so the popup would never
    // become "active" and would only close after being clicked first. This
    // mirrors AudioSwap's VolumePopup::Show.
    SetForegroundWindow(g_popupHwnd);
}

// ─── System Theme + Context Menu ─────────────────────────────────────────────

static bool IsSystemDarkMode() {
    DWORD value = 1, size = sizeof(value);
    RegGetValueW(HKEY_CURRENT_USER,
        L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
        L"AppsUseLightTheme", RRF_RT_REG_DWORD, nullptr, &value, &size);
    return value == 0;
}

static void ApplyContextMenuTheme(HWND hWnd, bool dark) {
    HMODULE ux = GetModuleHandleW(L"uxtheme.dll");
    if (!ux) return;
    using Fn135 = int(WINAPI*)(int);
    using Fn133 = bool(WINAPI*)(HWND, bool);
    using Fn136 = void(WINAPI*)();
    if (auto f = (Fn135)GetProcAddress(ux, MAKEINTRESOURCEA(135))) f(dark ? 2 : 0);
    if (auto f = (Fn133)GetProcAddress(ux, MAKEINTRESOURCEA(133))) f(hWnd, dark);
    if (auto f = (Fn136)GetProcAddress(ux, MAKEINTRESOURCEA(136))) f();
}

static void SaveIntervalMs(DWORD ms) {
    Wh_SetIntValue(L"UpdateIntervalMs", (int)ms);
}

static DWORD LoadIntervalMs() {
    DWORD ms = (DWORD)Wh_GetIntValue(L"UpdateIntervalMs", 120000);
    if (ms != 30000 && ms != 60000 && ms != 120000 && ms != 300000) ms = 120000;
    return ms;
}

// ─── Tray Window Procedure ────────────────────────────────────────────────────

static LRESULT CALLBACK TrayWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_CREATE:
            RefreshData();
            SetTimer(hWnd, WM_TIMER_ID, g_updateMs, nullptr);
            return 0;

        case WM_TIMER:
            if (wParam == WM_TIMER_ID) {
                RefreshData();
            }
            return 0;

        case WM_TRAY_CALLBACK:
            switch (LOWORD(lParam)) {
                case WM_LBUTTONUP:
                    if (g_popupHwnd && IsWindowVisible(g_popupHwnd)) {
                        ShowWindow(g_popupHwnd, SW_HIDE);
                        g_lastPopupCloseTime = GetTickCount64();
                    } else {
                        if (GetTickCount64() - g_lastPopupCloseTime > 200) {
                            ShowPopup(hWnd);
                        }
                    }
                    break;

                case WM_RBUTTONUP: {
                    HMENU hMenu = CreatePopupMenu();

                    //allelimo 
                    WCHAR statusText[128];
                    int freedisk = g_freeSpace;
                    int totaldisk = g_totalSpace;

                     if (freedisk >= 0 && totaldisk >= 0)
                         swprintf_s(statusText, L"Free: %d%%  Total: %d%%", freedisk, totaldisk);
                     else
                         lstrcpyW(statusText, L"Collecting...");
                     AppendMenuW(hMenu, MF_STRING | MF_GRAYED, 0, statusText);
                     AppendMenuW(hMenu, MF_SEPARATOR, 0, nullptr);

                    UINT chk300 = (g_updateMs == 30000)  ? (MF_STRING | MF_CHECKED) : MF_STRING;
                    UINT chk500 = (g_updateMs == 60000)  ? (MF_STRING | MF_CHECKED) : MF_STRING;
                    UINT chk1s  = (g_updateMs == 120000) ? (MF_STRING | MF_CHECKED) : MF_STRING;
                    UINT chk3s  = (g_updateMs == 300000) ? (MF_STRING | MF_CHECKED) : MF_STRING;
                    AppendMenuW(hMenu, chk300, MENU_INTERVAL_300MS, L"Refresh: 30s");
                    AppendMenuW(hMenu, chk500, MENU_INTERVAL_500MS, L"Refresh: 60s");
                    AppendMenuW(hMenu, chk1s,  MENU_INTERVAL_1S,    L"Refresh: 2m");
                    AppendMenuW(hMenu, chk3s,  MENU_INTERVAL_3S,    L"Refresh: 5m");
                    AppendMenuW(hMenu, MF_SEPARATOR, 0, nullptr);
                    AppendMenuW(hMenu, MF_STRING, MENU_OPEN_WINDHAWK, L"Open Windhawk");

                    POINT pt;
                    GetCursorPos(&pt);
                    bool dark = IsSystemDarkMode();
                    ApplyContextMenuTheme(hWnd, dark);
                    SetForegroundWindow(hWnd);
                    int cmd = TrackPopupMenu(hMenu, TPM_RETURNCMD | TPM_RIGHTBUTTON,  // allelimo
                        pt.x, pt.y, 0, hWnd, nullptr);
                    PostMessageW(hWnd, WM_NULL, 0, 0);
                    DestroyMenu(hMenu);

                    DWORD newMs = 0;
                    if      (cmd == MENU_INTERVAL_300MS) newMs = 30000;  //300
                    else if (cmd == MENU_INTERVAL_500MS) newMs = 60000;  //500
                    else if (cmd == MENU_INTERVAL_1S)    newMs = 120000; //1000
                    else if (cmd == MENU_INTERVAL_3S)    newMs = 300000; //3000
                    else if (cmd == MENU_OPEN_WINDHAWK) {
                        SHELLEXECUTEINFOW sei = {sizeof(sei)};
                        sei.lpFile = g_windhawkPath;
                        sei.nShow  = SW_SHOWNORMAL;
                        ShellExecuteExW(&sei);
                    }

                    if (newMs > 0 && newMs != g_updateMs) {
                        g_updateMs = newMs;
                        KillTimer(hWnd, WM_TIMER_ID);
                        SetTimer(hWnd, WM_TIMER_ID, newMs, nullptr);
                        SaveIntervalMs(newMs);
                    }
                    break;
                }
            }
            return 0;

        case WM_CLOSE:
            // Destroy popup on the tray thread (its owner) before destroying the tray window
            if (g_popupHwnd) { DestroyWindow(g_popupHwnd); g_popupHwnd = nullptr; }
            DestroyWindow(hWnd);
            return 0;

        case WM_DESTROY:
            KillTimer(hWnd, WM_TIMER_ID);
            {
                NOTIFYICONDATAW nid = {sizeof(nid)};
                nid.hWnd     = hWnd;
                nid.uID      = TRAY_ICON_ID;
                nid.uFlags   = NIF_GUID;
                nid.guidItem = MICRODISK_TRAY_GUID;
                Shell_NotifyIconW(NIM_DELETE, &nid);
            }
            PostQuitMessage(0);
            return 0;
    }

    // Re-add tray icon after Explorer restarts
    if (msg == g_taskbarCreatedMsg && g_taskbarCreatedMsg != 0) {
        g_lastTip[0] = L'\0';  // force the tooltip to be re-pushed on next refresh
        NOTIFYICONDATAW nid = {sizeof(nid)};
        nid.hWnd            = hWnd;
        nid.uID             = TRAY_ICON_ID;
        nid.uFlags          = NIF_ICON | NIF_MESSAGE | NIF_TIP | NIF_GUID | NIF_SHOWTIP;
        nid.guidItem        = MICRODISK_TRAY_GUID;
        nid.uCallbackMessage = WM_TRAY_CALLBACK;
        lstrcpynW(nid.szTip, L"MicroDisk", 128);
        if (!g_iconEnabled)
            ExtractIconExW(g_ddoresDllPath, 28, nullptr, &g_iconEnabled, 1);
        nid.hIcon = g_iconEnabled ? g_iconEnabled : LoadIconW(nullptr, IDI_APPLICATION);
        if (Shell_NotifyIconW(NIM_ADD, &nid)) {
            NOTIFYICONDATAW nidVer = {sizeof(nidVer)};
            nidVer.hWnd     = hWnd;
            nidVer.uID      = TRAY_ICON_ID;
            nidVer.uFlags   = NIF_GUID;
            nidVer.guidItem = MICRODISK_TRAY_GUID;
            nidVer.uVersion = NOTIFYICON_VERSION_4;
            Shell_NotifyIconW(NIM_SETVERSION, &nidVer);
        }
        return 0;
    }

    return DefWindowProcW(hWnd, msg, wParam, lParam);
}

// ─── Tray Thread ──────────────────────────────────────────────────────────────

static DWORD WINAPI TrayThreadProc(LPVOID) {
    // Per-monitor DPI awareness so the popup renders crisply on scaled displays.
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

    HRESULT hrCo = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    if (FAILED(hrCo) && hrCo != RPC_E_CHANGED_MODE) {
        Wh_Log(L"TrayThread: CoInitializeEx failed (0x%X)", hrCo);
        return 1;
    }

    g_taskbarCreatedMsg = RegisterWindowMessageW(L"TaskbarCreated");

    WNDCLASSW wc = {0};
    wc.lpfnWndProc = TrayWndProc;
    wc.hInstance = g_hInstance;
    wc.lpszClassName = L"MicroDiskTrayClass";
    if (!RegisterClassW(&wc) && GetLastError() != ERROR_CLASS_ALREADY_EXISTS) {
        Wh_Log(L"Tray RegisterClassW failed (%u)", GetLastError());
        if (SUCCEEDED(hrCo)) CoUninitialize();
        return 1;
    }

    InterlockedExchangePointer((PVOID*)&g_trayHwnd, CreateWindowExW(
        WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
        wc.lpszClassName, L"MicroDisk",
        WS_POPUP,
        0, 0, 1, 1, nullptr, nullptr, g_hInstance, nullptr));
    if (!g_trayHwnd) {
        if (SUCCEEDED(hrCo)) CoUninitialize();
        return 1;
    }

    // Unique AUMID so the OS doesn't group this icon with Windhawk's main window
    IPropertyStore* pps = nullptr;
    if (SUCCEEDED(SHGetPropertyStoreForWindow(g_trayHwnd, IID_PPV_ARGS(&pps)))) {
        PROPVARIANT var;
        PropVariantInit(&var);
        var.vt      = VT_LPWSTR;
        var.pwszVal = (LPWSTR)CoTaskMemAlloc(MAX_PATH * sizeof(WCHAR));
        if (var.pwszVal) {
            lstrcpyW(var.pwszVal, L"allelimo.MicroDisk");
            pps->SetValue(PKEY_AppUserModel_ID, var);
            CoTaskMemFree(var.pwszVal);
        }
        pps->Commit();
        pps->Release();
    }

    NOTIFYICONDATAW nid = {sizeof(nid)};
    nid.hWnd            = g_trayHwnd;
    nid.uID             = TRAY_ICON_ID;
    nid.uFlags          = NIF_ICON | NIF_MESSAGE | NIF_TIP | NIF_GUID | NIF_SHOWTIP;
    nid.guidItem        = MICRODISK_TRAY_GUID;
    nid.uCallbackMessage = WM_TRAY_CALLBACK;
    lstrcpynW(nid.szTip, L"MicroDisk", 128);
    nid.hIcon = g_iconEnabled ? g_iconEnabled : LoadIconW(nullptr, IDI_APPLICATION);
    if (Shell_NotifyIconW(NIM_ADD, &nid)) {
        NOTIFYICONDATAW nidVer = {sizeof(nidVer)};
        nidVer.hWnd     = g_trayHwnd;
        nidVer.uID      = TRAY_ICON_ID;
        nidVer.uFlags   = NIF_GUID;
        nidVer.guidItem = MICRODISK_TRAY_GUID;
        nidVer.uVersion = NOTIFYICON_VERSION_4;
        Shell_NotifyIconW(NIM_SETVERSION, &nidVer);
    }

    MSG msg;
    while (GetMessageW(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    InterlockedExchangePointer((PVOID*)&g_trayHwnd, nullptr);
    if (SUCCEEDED(hrCo)) CoUninitialize();
    return 0;
}

// ─── Windhawk Callbacks ───────────────────────────────────────────────────────

// allelimo
void LoadSettings() {
    g_settings.taskBarOnTop = Wh_GetIntSetting(L"taskBarOnTop");
    g_settings.diskLetter = Wh_GetStringSetting(L"diskLetter");
    g_settings.iconHidden = Wh_GetIntSetting(L"iconHidden");
}

BOOL WhTool_ModInit() {
    Wh_Log(L"MicroDisk Init");

    // allelimo
    LoadSettings();
    GetDiskInfo();

    g_updateMs = LoadIntervalMs();

    MEMORYSTATUSEX mem = {sizeof(mem)};
    if (GlobalMemoryStatusEx(&mem)) g_totalPhys = mem.ullTotalPhys;

    g_hInstance = GetModuleHandleW(nullptr);
    switch (GetModuleFileNameW(g_hInstance, g_windhawkPath, ARRAYSIZE(g_windhawkPath))) {
        case 0:
        case ARRAYSIZE(g_windhawkPath):
            Wh_Log(L"GetModuleFileNameW failed");
            break;
    }

    // Full path for ddores.dll — ExtractIconExW handles the .mun redirect on Win11
    UINT sysLen = GetSystemDirectoryW(g_ddoresDllPath, MAX_PATH);
    if (sysLen > 0 && sysLen < MAX_PATH - 12)
        lstrcatW(g_ddoresDllPath, L"\\ddores.dll");
    else
        lstrcpyW(g_ddoresDllPath, L"ddores.dll");

    ExtractIconExW(g_ddoresDllPath, 75, nullptr, &g_iconEnabled, 1);  //allelimo icon 75 invece di 28
    
    g_trayThread = CreateThread(nullptr, 0, TrayThreadProc, nullptr, 0, nullptr);
    return TRUE;
}

void WhTool_ModSettingsChanged() {
    // allelimo
    LoadSettings();
}

void WhTool_ModUninit() {
    Wh_Log(L"MicroDisk Mod Uninit");

    // WM_CLOSE handler on the tray thread destroys g_popupHwnd before the tray
    // window itself — no cross-thread DestroyWindow needed here
    HWND hwndClose = (HWND)InterlockedCompareExchangePointer((PVOID*)&g_trayHwnd, nullptr, nullptr);
    if (hwndClose && IsWindow(hwndClose)) PostMessageW(hwndClose, WM_CLOSE, 0, 0);
    if (g_trayThread) {
        WaitForSingleObject(g_trayThread, 3000);
        CloseHandle(g_trayThread);
        g_trayThread = nullptr;
    }

    // Unregister popup class after the tray thread has fully exited so a
    // subsequent mod reload can re-register it cleanly
    UnregisterClassW(L"MicroDiskPopupClass", g_hInstance);
    UnregisterClassW(L"MicroDiskTrayClass", g_hInstance);

    if (g_iconEnabled) { DestroyIcon(g_iconEnabled); g_iconEnabled = nullptr; }
    if (g_hPopupFont) { DeleteObject(g_hPopupFont); g_hPopupFont = nullptr; }
    //if (g_gpuQuery) { PdhCloseQuery(g_gpuQuery); g_gpuQuery = nullptr; g_gpuCounter = nullptr; }
}

////////////////////////////////////////////////////////////////////////////////
// Windhawk tool mod implementation for mods which don't need to inject to other
// processes or hook other functions. Context:
// https://github.com/ramensoftware/windhawk/wiki/Mods-as-tools:-Running-mods-in-a-dedicated-process
//
// The mod will load and run in a dedicated windhawk.exe process.
//
// Paste the code below as part of the mod code, and use these callbacks:
// * WhTool_ModInit
// * WhTool_ModSettingsChanged
// * WhTool_ModUninit
//
// Currently, other callbacks are not supported.

bool g_isToolModProcessLauncher;
HANDLE g_toolModProcessMutex;

void WINAPI EntryPoint_Hook() {
    Wh_Log(L">");
    ExitThread(0);
}

BOOL Wh_ModInit() {
    DWORD sessionId;
    if (ProcessIdToSessionId(GetCurrentProcessId(), &sessionId) &&
        sessionId == 0) {
        return FALSE;
    }

    bool isExcluded = false;
    bool isToolModProcess = false;
    bool isCurrentToolModProcess = false;
    int argc;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);
    if (!argv) {
        Wh_Log(L"CommandLineToArgvW failed");
        return FALSE;
    }

    for (int i = 1; i < argc; i++) {
        if (wcscmp(argv[i], L"-service") == 0 ||
            wcscmp(argv[i], L"-service-start") == 0 ||
            wcscmp(argv[i], L"-service-stop") == 0) {
            isExcluded = true;
            break;
        }
    }

    for (int i = 1; i < argc - 1; i++) {
        if (wcscmp(argv[i], L"-tool-mod") == 0) {
            isToolModProcess = true;
            if (wcscmp(argv[i + 1], WH_MOD_ID) == 0) {
                isCurrentToolModProcess = true;
            }
            break;
        }
    }

    LocalFree(argv);

    if (isExcluded) {
        return FALSE;
    }

    if (isCurrentToolModProcess) {
        g_toolModProcessMutex =
            CreateMutexW(nullptr, TRUE, L"windhawk-tool-mod_" WH_MOD_ID);
        if (!g_toolModProcessMutex) {
            Wh_Log(L"CreateMutex failed");
            ExitProcess(1);
        }

        if (GetLastError() == ERROR_ALREADY_EXISTS) {
            Wh_Log(L"Tool mod already running (%s)", WH_MOD_ID);
            ExitProcess(1);
        }

        if (!WhTool_ModInit()) {
            ExitProcess(1);
        }

        IMAGE_DOS_HEADER* dosHeader =
            (IMAGE_DOS_HEADER*)GetModuleHandleW(nullptr);
        IMAGE_NT_HEADERS* ntHeaders =
            (IMAGE_NT_HEADERS*)((BYTE*)dosHeader + dosHeader->e_lfanew);

        DWORD entryPointRVA = ntHeaders->OptionalHeader.AddressOfEntryPoint;
        void* entryPoint = (BYTE*)dosHeader + entryPointRVA;

        Wh_SetFunctionHook(entryPoint, (void*)EntryPoint_Hook, nullptr);
        return TRUE;
    }

    if (isToolModProcess) {
        return FALSE;
    }

    g_isToolModProcessLauncher = true;
    return TRUE;
}

void Wh_ModAfterInit() {
    if (!g_isToolModProcessLauncher) {
        return;
    }

    WCHAR currentProcessPath[MAX_PATH];
    switch (GetModuleFileNameW(nullptr, currentProcessPath,
                               ARRAYSIZE(currentProcessPath))) {
        case 0:
        case ARRAYSIZE(currentProcessPath):
            Wh_Log(L"GetModuleFileName failed");
            return;
    }

    WCHAR
    commandLine[MAX_PATH + 2 +
                (sizeof(L" -tool-mod \"" WH_MOD_ID "\"") / sizeof(WCHAR)) - 1];
    swprintf_s(commandLine, L"\"%s\" -tool-mod \"%s\"", currentProcessPath,
               WH_MOD_ID);

    HMODULE kernelModule = GetModuleHandleW(L"kernelbase.dll");
    if (!kernelModule) {
        kernelModule = GetModuleHandleW(L"kernel32.dll");
        if (!kernelModule) {
            Wh_Log(L"No kernelbase.dll/kernel32.dll");
            return;
        }
    }

    using CreateProcessInternalW_t = BOOL(WINAPI*)(
        HANDLE hUserToken, LPCWSTR lpApplicationName, LPWSTR lpCommandLine,
        LPSECURITY_ATTRIBUTES lpProcessAttributes,
        LPSECURITY_ATTRIBUTES lpThreadAttributes, WINBOOL bInheritHandles,
        DWORD dwCreationFlags, LPVOID lpEnvironment, LPCWSTR lpCurrentDirectory,
        LPSTARTUPINFOW lpStartupInfo,
        LPPROCESS_INFORMATION lpProcessInformation,
        PHANDLE hRestrictedUserToken);
    CreateProcessInternalW_t pCreateProcessInternalW =
        (CreateProcessInternalW_t)GetProcAddress(kernelModule,
                                                 "CreateProcessInternalW");
    if (!pCreateProcessInternalW) {
        Wh_Log(L"No CreateProcessInternalW");
        return;
    }

    STARTUPINFOW si{
        .cb = sizeof(STARTUPINFOW),
        .dwFlags = STARTF_FORCEOFFFEEDBACK,
    };
    PROCESS_INFORMATION pi;
    if (!pCreateProcessInternalW(nullptr, currentProcessPath, commandLine,
                                 nullptr, nullptr, FALSE, NORMAL_PRIORITY_CLASS,
                                 nullptr, nullptr, &si, &pi, nullptr)) {
        Wh_Log(L"CreateProcess failed");
        return;
    }

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}

void Wh_ModSettingsChanged() {
    if (g_isToolModProcessLauncher) {
        return;
    }

    WhTool_ModSettingsChanged();
}

void Wh_ModUninit() {
    if (g_isToolModProcessLauncher) {
        return;
    }

    WhTool_ModUninit();
    ExitProcess(0);
}