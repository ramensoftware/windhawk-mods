// ==WindhawkMod==
// @id              amd-radeon-software-window-fix
// @name            AMD Radeon Software Window Fix
// @description     Forces the position, size, and maximized state of Radeon Software (RadeonSoftware.exe) every time its window is shown from the system tray. Optionally forces the startup page.
// @version         1.0
// @author          St0RM53
// @github          https://github.com/St0RM53
// @include         RadeonSoftware.exe
// @compilerOptions -luser32 -lcomctl32
// @license         GPL-3.0-or-later
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# AMD Radeon Software Window Fix

AMD's Radeon Software runs persistently in the system tray. When you double-click
the tray icon to open it, the app reads its size from the registry and then centers
itself — ignoring any position you prefer, and doing so *after* any `SetWindowPos`
call, making simple hooks ineffective.

The behavior is especially frustrating on multi-monitor setups with extended desktop
and has been most likely present since the Adrenalin edition release from 2018. Since
AMD devs are unwilling to even acknowledge their incompetence or even attempt to fix
it, i've used some of my knowledge and a few AI coding tools to give a working solution.

This mod subclasses the main window to intercept `WM_WINDOWPOSCHANGING` — a message
fired *before* any move or resize takes effect — and overrides the position and size
there, which is the only intercept point that reliably wins against Qt's internal
layout logic.

Tested on version 26.3.1 (which introduced a metrics overlay bug), but should work on 
any of the recent (2025-2026) versions that use the Qt 6.x.x framework.

## Settings

Open the mod's **Settings** tab to configure:

| Setting | Description |
|---|---|
| **Left (X)** | Horizontal position of the window's left edge, in pixels |
| **Top (Y)** | Vertical position of the window's top edge, in pixels |
| **Width** | Width of the window in pixels, minimum 1280 (used when not maximized) |
| **Height** | Height of the window in pixels, minimum 600 (used when not maximized) |
| **Maximized** | When enabled, the window will always open maximized |
| **[Experimental] Force Startup Page** | Forces the app to reopen on the last visited page instead of always defaulting to the Dashboard |

> **Note:** The startup page feature sets `ResumeLastPage=true` in the registry each time the window is shown. The app itself tracks and writes the last visited page — this mod simply ensures that value is always honoured.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- x: 0
  $name: Left (X position)
  $description: Horizontal screen coordinate for the left edge of the window (pixels)
- y: 0
  $name: Top (Y position)
  $description: Vertical screen coordinate for the top edge of the window (pixels)
- width: 1400
  $name: Width
  $description: Window width in pixels, minimum 1280 (ignored when Maximized is enabled)
- height: 1000
  $name: Height
  $description: Window height in pixels, minimum 600 (ignored when Maximized is enabled)
- maximized: false
  $name: Maximized
  $description: Always open the window maximized (overrides position and size)
- forceStartupPage: false
  $name: "[Experimental] Force Startup Page"
  $description: "Forces ResumeLastPage=true in HKCU\\SOFTWARE\\AMD\\CN every time the window is shown, so the app reopens on the last page you visited rather than always defaulting to the Dashboard"
*/
// ==/WindhawkModSettings==

#include <windhawk_api.h>
#include <windhawk_utils.h>
#include <windows.h>
#include <commctrl.h>
#include <algorithm>

// ---------------------------------------------------------------------------
// Settings
// ---------------------------------------------------------------------------
static struct Settings {
    int  x, y, width, height;
    bool maximized;
    bool forceStartupPage;
} g_settings;

static void LoadSettings() {
    g_settings.x               = Wh_GetIntSetting(L"x",         0);
    g_settings.y               = Wh_GetIntSetting(L"y",         0);
    g_settings.width           = std::max(1280, Wh_GetIntSetting(L"width",  1400));
    g_settings.height          = std::max(600,  Wh_GetIntSetting(L"height", 1000));
    g_settings.maximized        = Wh_GetIntSetting(L"maximized",        0) != 0;
    g_settings.forceStartupPage = Wh_GetIntSetting(L"forceStartupPage",  0) != 0;
}

// ---------------------------------------------------------------------------
// Registry helper — sets ResumeLastPage="true" in HKCU\SOFTWARE\AMD\CN
// ---------------------------------------------------------------------------
static void WriteResumeLastPage() {
    HKEY hKey = nullptr;
    LSTATUS status = RegCreateKeyExW(
        HKEY_CURRENT_USER, L"SOFTWARE\\AMD\\CN",
        0, nullptr, 0, KEY_SET_VALUE, nullptr, &hKey, nullptr);

    if (status != ERROR_SUCCESS) {
        Wh_Log(L"WriteResumeLastPage: RegCreateKeyEx failed (%ld)", status);
        return;
    }

    static const wchar_t valTrue[] = L"true";
    status = RegSetValueExW(hKey, L"ResumeLastPage", 0, REG_SZ,
        reinterpret_cast<const BYTE*>(valTrue),
        static_cast<DWORD>(sizeof(valTrue)));

    if (status != ERROR_SUCCESS)
        Wh_Log(L"WriteResumeLastPage: failed to write value (%ld)", status);
    else
        Wh_Log(L"WriteResumeLastPage: set ResumeLastPage=\"true\"");

    RegCloseKey(hKey);
}

// ---------------------------------------------------------------------------
// Registered message
// ---------------------------------------------------------------------------
// Using RegisterWindowMessage avoids colliding with Qt's own WM_USER-range
// messages, which it uses internally for cross-thread signalling.
static UINT g_msgApplyMaximize = 0;

// Saved handle of the subclassed window, used to remove the subclass on mod unload.
static HWND g_subclassedHwnd = nullptr;

// ---------------------------------------------------------------------------
// Subclass procedure
// ---------------------------------------------------------------------------
static LRESULT CALLBACK SubclassProc(
    HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam,
    DWORD_PTR /*dwRefData*/)
{
    if (uMsg == WM_WINDOWPOSCHANGING) {
        WINDOWPOS* wp = reinterpret_cast<WINDOWPOS*>(lParam);

        if (g_settings.maximized) {
            // Don't interfere — we handle positioning in the ApplyMaximize message before
            // calling SW_MAXIMIZE, and we must not block the maximize itself.
        } else if (!IsIconic(hWnd)) {
            wp->x   = g_settings.x;
            wp->y   = g_settings.y;
            wp->cx  = g_settings.width;
            wp->cy  = g_settings.height;
            wp->flags &= ~(SWP_NOMOVE | SWP_NOSIZE);

            Wh_Log(L"WM_WINDOWPOSCHANGING: forced to (%d,%d %dx%d)",
                   wp->x, wp->y, wp->cx, wp->cy);
        }
    }
    else if (uMsg == WM_SHOWWINDOW && wParam == TRUE) {
        if (g_settings.forceStartupPage) {
            WriteResumeLastPage();
        }
        if (g_settings.maximized) {
            PostMessage(hWnd, g_msgApplyMaximize, 0, 0);
            Wh_Log(L"WM_SHOWWINDOW: posted maximize request");
        }
    }
    else if (uMsg != 0 && uMsg == g_msgApplyMaximize) {
        // Move onto the correct monitor first, then maximize.
        // SW_MAXIMIZE always maximizes on the monitor the window currently
        // occupies, so we nudge it to the target monitor beforehand.
        POINT pt = { g_settings.x, g_settings.y };
        HMONITOR hMon = MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
        MONITORINFO mi = {};
        mi.cbSize = sizeof(mi);
        if (GetMonitorInfo(hMon, &mi)) {
            SetWindowPos(hWnd, nullptr,
                mi.rcWork.left, mi.rcWork.top, 0, 0,
                SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
            Wh_Log(L"ApplyMaximize: moved to monitor work area (%ld,%ld) before maximize",
                   mi.rcWork.left, mi.rcWork.top);
        }
        ShowWindow(hWnd, SW_MAXIMIZE);
        Wh_Log(L"ApplyMaximize: applied SW_MAXIMIZE");
    }
    else if (uMsg == WM_NCDESTROY) {
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(hWnd, SubclassProc);
        g_subclassedHwnd = nullptr;
        Wh_Log(L"WM_NCDESTROY: subclass removed");
    }

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

// ---------------------------------------------------------------------------
// Hook: SetWindowPos — used only to detect the window and install the subclass
// ---------------------------------------------------------------------------
using SetWindowPos_t = decltype(&SetWindowPos);
static SetWindowPos_t pOriginalSetWindowPos = nullptr;

static BOOL WINAPI SetWindowPos_Hook(
    HWND hWnd, HWND hWndInsertAfter,
    int X, int Y, int cx, int cy, UINT uFlags)
{
    DWORD pid = 0;
    GetWindowThreadProcessId(hWnd, &pid);

    if (pid == GetCurrentProcessId()) {
        wchar_t className[64] = {};
        GetClassNameW(hWnd, className, _countof(className));

        if (wcscmp(className, L"Qt683QWindowIcon") == 0) {
            if (g_subclassedHwnd != hWnd) {
                if (WindhawkUtils::SetWindowSubclassFromAnyThread(hWnd, SubclassProc, 0)) {
                    g_subclassedHwnd = hWnd;
                    Wh_Log(L"SetWindowPos_Hook: subclass installed on hwnd=%p", hWnd);
                } else {
                    Wh_Log(L"SetWindowPos_Hook: failed to install subclass (error=%lu)", GetLastError());
                }
            }
        }
    }

    return pOriginalSetWindowPos(hWnd, hWndInsertAfter, X, Y, cx, cy, uFlags);
}

// ---------------------------------------------------------------------------
// Windhawk entry points
// ---------------------------------------------------------------------------
BOOL Wh_ModInit() {
    Wh_Log(L"AMD Radeon Software Window Fix v1.0: initialising");

    g_msgApplyMaximize = RegisterWindowMessageW(L"WindhawkAMDRadeonSoftwareApplyMaximize");
    if (!g_msgApplyMaximize) {
        Wh_Log(L"RegisterWindowMessage failed (%lu)", GetLastError());
        return FALSE;
    }

    LoadSettings();

    HMODULE hUser32 = GetModuleHandleW(L"user32.dll");
    if (!hUser32) {
        Wh_Log(L"Failed to get user32.dll");
        return FALSE;
    }

    void* pSetWindowPos = (void*)GetProcAddress(hUser32, "SetWindowPos");
    if (!pSetWindowPos) {
        Wh_Log(L"Failed to resolve SetWindowPos");
        return FALSE;
    }

    if (!Wh_SetFunctionHook(pSetWindowPos, (void*)SetWindowPos_Hook, (void**)&pOriginalSetWindowPos)) {
        Wh_Log(L"Failed to hook SetWindowPos");
        return FALSE;
    }

    Wh_Log(L"Hook installed. pos=(%d,%d) size=%dx%d maximized=%s forceStartupPage=%s",
           g_settings.x, g_settings.y,
           g_settings.width, g_settings.height,
           g_settings.maximized        ? L"yes" : L"no",
           g_settings.forceStartupPage ? L"yes" : L"no");

    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L"AMD Radeon Software Window Fix: uninitialising");
    if (g_subclassedHwnd) {
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(g_subclassedHwnd, SubclassProc);
        g_subclassedHwnd = nullptr;
        Wh_Log(L"Wh_ModUninit: subclass removed");
    }
}

void Wh_ModSettingsChanged() {
    Wh_Log(L"Settings changed — reloading");
    LoadSettings();
}
