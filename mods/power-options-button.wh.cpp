// ==WindhawkMod==
// @id              power-options-button
// @name            Power Options Button
// @description     Tray icon to switch between Windows power plans with the mouse wheel.
// @version         1.0.0
// @author          SilverAmd
// @github          https://github.com/SilverAmd
// @license         MIT
// @include         windhawk.exe
// @compilerOptions -luser32 -lshell32 -lgdi32 -lpowrprof
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Power Options Button

Adds a tray icon for switching between Windows power plans.

## Preview

![Power Options Button preview](https://i.imgur.com/26A3Ilv.gif)

*Shows the tray icon, tooltip, right-click menu, and mouse-wheel switching between power plans.*

## Features

- Tray icon shows the currently active Windows power plan.
- Tooltip shows the active plan.
- Mouse wheel over the tray icon cycles between:
  - Power Saver
  - Balanced
  - High Performance
  - Ultimate Performance
- Right-click opens a context menu.
- Icon color changes depending on the active plan:
  - Green = Power Saver
  - Yellow = Balanced
  - Red = High Performance
  - Violet = Ultimate Performance
- If Ultimate Performance is not available, it is shown as disabled and can be created from the tray menu with administrator approval.
- Automatically restores the tray icon after Explorer/taskbar restart.

## Usage

- Scroll up over the tray icon: next power plan.
- Scroll down over the tray icon: previous power plan.
- Right-click the tray icon: choose a power plan manually.
- Left-click the tray icon: refresh current status.
- Use "Create Ultimate Performance..." from the tray menu to create the Ultimate Performance plan if it is not available yet.
*/
// ==/WindhawkModReadme==

// Important:
// After Windows startup, the low-level mouse hook can appear installed but not
// receive wheel events over the tray icon until it is refreshed.
// Refreshing the hook once on the first tray WM_MOUSEMOVE makes mouse-wheel
// switching work immediately after reboot.

#include <windows.h>
#include <shellapi.h>
#include <powrprof.h>
#include <strsafe.h>

#ifndef NIF_SHOWTIP
#define NIF_SHOWTIP 0x00000080
#endif

#ifndef NIN_SELECT
#define NIN_SELECT (WM_USER + 0)
#endif

#ifndef NIN_KEYSELECT
#define NIN_KEYSELECT (NIN_SELECT | 0x1)
#endif

#define WM_TRAYICON        (WM_USER + 1)
#define WM_TRAY_SCROLL     (WM_APP + 1)
#define WM_APP_EXIT        (WM_APP + 2)

#define ID_TRAYICON               24001
#define ID_MENU_POWER_SAVER       3001
#define ID_MENU_BALANCED          3002
#define ID_MENU_HIGH_PERFORMANCE  3003
#define ID_MENU_ULTIMATE          3004
#define ID_MENU_REFRESH           3005
#define ID_MENU_OPEN_POWER        3006
#define ID_MENU_OPEN_SETTINGS     3007
#define ID_MENU_OPEN_WINDHAWK     3008
#define ID_MENU_CREATE_ULTIMATE   3009

#define HIDDEN_WINDOW_CLASS L"PowerOptionsButtonHiddenWindow"

#define TIMER_TRAY_RECT_REFRESH 4001
#define TRAY_RECT_REFRESH_INTERVAL_MS 500
#define TRAY_RECT_REFRESH_MAX_ATTEMPTS 20
#define TIMER_POST_START_ICON_SYNC 4002
#define POST_START_ICON_SYNC_DELAY_MS 1000

static HWND g_hwnd = nullptr;
static HICON g_hIcon = nullptr;
static NOTIFYICONDATAW g_nid = {};
static UINT g_taskbarCreatedMessage = 0;

static HHOOK g_mouseHook = nullptr;
static RECT g_trayIconRect = {};
static DWORD g_lastScrollTime = 0;
static DWORD g_mouseHookInstalledAt = 0;
static DWORD g_lastMouseHookRefresh = 0;
static bool g_mouseHookRefreshedAfterFirstHover = false;
#define DEBUG_MOUSE_WHEEL 0

static DWORD g_lastWheelDebugLog = 0;
static DWORD g_lastTrayMouseMoveLog = 0;
static DWORD g_lastTrayMouseMoveTime = 0;

static HANDLE g_uiThread = nullptr;
static DWORD g_uiThreadId = 0;

static WCHAR g_windhawkPath[MAX_PATH] = {};

static const GUID GUID_POWER_SAVER =
    {0xa1841308, 0x3541, 0x4fab, {0xbc, 0x81, 0xf7, 0x15, 0x56, 0xf2, 0x0b, 0x4a}};

static const GUID GUID_BALANCED =
    {0x381b4222, 0xf694, 0x41f0, {0x96, 0x85, 0xff, 0x5b, 0xb2, 0x60, 0xdf, 0x2e}};

static const GUID GUID_HIGH_PERFORMANCE =
    {0x8c5e7fda, 0xe8bf, 0x4a96, {0x9a, 0x85, 0xa6, 0xe2, 0x3a, 0x8c, 0x63, 0x5c}};

static const GUID GUID_ULTIMATE_PERFORMANCE =
    {0xe9a42b02, 0xd5df, 0x448d, {0xaa, 0x00, 0x03, 0xf1, 0x47, 0x49, 0xeb, 0x61}};

struct PowerPlanInfo {
    const WCHAR* name;
    const WCHAR* shortName;
    const WCHAR* guidString;
    GUID guid;
    COLORREF color;
    int menuId;
};

static PowerPlanInfo g_plans[] = {
    {
        L"Power Saver",
        L"P",
        L"a1841308-3541-4fab-bc81-f71556f20b4a",
        GUID_POWER_SAVER,
        RGB(0, 190, 80),
        ID_MENU_POWER_SAVER
    },
    {
        L"Balanced",
        L"B",
        L"381b4222-f694-41f0-9685-ff5bb260df2e",
        GUID_BALANCED,
        RGB(255, 210, 0),
        ID_MENU_BALANCED
    },
    {
        L"High Performance",
        L"H",
        L"8c5e7fda-e8bf-4a96-9a85-a6e23a8c635c",
        GUID_HIGH_PERFORMANCE,
        RGB(230, 40, 40),
        ID_MENU_HIGH_PERFORMANCE
    },
    {
        L"Ultimate Performance",
        L"U",
        L"e9a42b02-d5df-448d-aa00-03f14749eb61",
        GUID_ULTIMATE_PERFORMANCE,
        RGB(160, 70, 255),
        ID_MENU_ULTIMATE
    },
};

static const int g_planCount = ARRAYSIZE(g_plans);
static int g_currentPlanIndex = 1;
static bool g_ultimateAvailable = false;
static GUID g_ultimateRuntimeGuid =
    {0xe9a42b02, 0xd5df, 0x448d, {0xaa, 0x00, 0x03, 0xf1, 0x47, 0x49, 0xeb, 0x61}};
static WCHAR g_ultimateRuntimeName[128] = L"Ultimate Performance";

static int g_trayRectRefreshAttempts = 0;

// ------------------------------------------------------------
// Helpers
// ------------------------------------------------------------

bool GuidEquals(const GUID& a, const GUID& b) {
    return IsEqualGUID(a, b) != FALSE;
}

bool ContainsTextInsensitive(PCWSTR text, PCWSTR search) {
    if (!text || !search || !search[0]) {
        return false;
    }

    size_t searchLen = wcslen(search);

    for (PCWSTR p = text; *p; p++) {
        if (_wcsnicmp(p, search, searchLen) == 0) {
            return true;
        }
    }

    return false;
}

void GuidToStringNoBraces(const GUID& guid, WCHAR* buffer, size_t bufferCount) {
    StringCchPrintfW(
        buffer,
        bufferCount,
        L"%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x",
        guid.Data1,
        guid.Data2,
        guid.Data3,
        guid.Data4[0],
        guid.Data4[1],
        guid.Data4[2],
        guid.Data4[3],
        guid.Data4[4],
        guid.Data4[5],
        guid.Data4[6],
        guid.Data4[7]
    );
}

bool ReadPowerSchemeFriendlyName(const GUID& guid, WCHAR* name, DWORD nameCount) {
    if (!name || nameCount == 0) {
        return false;
    }

    name[0] = L'\0';

    DWORD sizeBytes = nameCount * sizeof(WCHAR);

    DWORD result = PowerReadFriendlyName(
        nullptr,
        &guid,
        nullptr,
        nullptr,
        reinterpret_cast<UCHAR*>(name),
        &sizeBytes
    );

    return result == ERROR_SUCCESS && name[0];
}

bool IsEnumeratedPowerSchemePresent(const GUID& wantedGuid) {
    for (ULONG index = 0;; index++) {
        GUID schemeGuid = {};
        DWORD size = sizeof(schemeGuid);

        DWORD result = PowerEnumerate(
            nullptr,
            nullptr,
            nullptr,
            ACCESS_SCHEME,
            index,
            reinterpret_cast<UCHAR*>(&schemeGuid),
            &size
        );

        if (result == ERROR_NO_MORE_ITEMS) {
            break;
        }

        if (result != ERROR_SUCCESS) {
            Wh_Log(L"PowerEnumerate failed while checking schemes. Error: %u", result);
            break;
        }

        if (GuidEquals(schemeGuid, wantedGuid)) {
            return true;
        }
    }

    return false;
}

void RefreshUltimatePerformanceAvailability() {
    g_ultimateAvailable = false;
    g_ultimateRuntimeGuid = GUID_ULTIMATE_PERFORMANCE;
    StringCchCopyW(g_ultimateRuntimeName, ARRAYSIZE(g_ultimateRuntimeName), L"Ultimate Performance");

    for (ULONG index = 0;; index++) {
        GUID schemeGuid = {};
        DWORD size = sizeof(schemeGuid);

        DWORD result = PowerEnumerate(
            nullptr,
            nullptr,
            nullptr,
            ACCESS_SCHEME,
            index,
            reinterpret_cast<UCHAR*>(&schemeGuid),
            &size
        );

        if (result == ERROR_NO_MORE_ITEMS) {
            break;
        }

        if (result != ERROR_SUCCESS) {
            Wh_Log(L"PowerEnumerate failed while searching Ultimate Performance. Error: %u", result);
            break;
        }

        WCHAR friendlyName[128] = {};
        ReadPowerSchemeFriendlyName(schemeGuid, friendlyName, ARRAYSIZE(friendlyName));

        if (GuidEquals(schemeGuid, GUID_ULTIMATE_PERFORMANCE) ||
            ContainsTextInsensitive(friendlyName, L"Ultimate") ||
            ContainsTextInsensitive(friendlyName, L"Ultimativ")) {
            g_ultimateAvailable = true;
            g_ultimateRuntimeGuid = schemeGuid;

            if (friendlyName[0]) {
                StringCchCopyW(
                    g_ultimateRuntimeName,
                    ARRAYSIZE(g_ultimateRuntimeName),
                    friendlyName
                );
            }

            WCHAR guidText[64] = {};
            GuidToStringNoBraces(g_ultimateRuntimeGuid, guidText, ARRAYSIZE(guidText));

            Wh_Log(
                L"Ultimate Performance detected. Name: %s, GUID: %s",
                g_ultimateRuntimeName,
                guidText
            );

            return;
        }
    }

    Wh_Log(L"Ultimate Performance is not available as an active power scheme.");
}

GUID GetRuntimePlanGuid(int index) {
    if (index == 3 && g_ultimateAvailable) {
        return g_ultimateRuntimeGuid;
    }

    return g_plans[index].guid;
}

int FindPlanIndexByGuid(const GUID& guid);

bool IsPowerPlanAvailable(const GUID& guid) {
    int index = FindPlanIndexByGuid(guid);

    if (index < 0) {
        return false;
    }

    if (index == 3) {
        return g_ultimateAvailable;
    }

    return IsEnumeratedPowerSchemePresent(g_plans[index].guid);
}

bool SetPowerPlanWithPowercfg(int index) {
    if (index < 0 || index >= g_planCount) {
        return false;
    }

    GUID runtimeGuid = GetRuntimePlanGuid(index);

    WCHAR guidText[64] = {};
    GuidToStringNoBraces(runtimeGuid, guidText, ARRAYSIZE(guidText));

    WCHAR cmdLine[512] = {};
    StringCchPrintfW(
        cmdLine,
        ARRAYSIZE(cmdLine),
        L"powercfg.exe /setactive %s",
        guidText
    );

    STARTUPINFOW si = {};
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;

    PROCESS_INFORMATION pi = {};

    BOOL created = CreateProcessW(
        nullptr,
        cmdLine,
        nullptr,
        nullptr,
        FALSE,
        CREATE_NO_WINDOW,
        nullptr,
        nullptr,
        &si,
        &pi
    );

    if (!created) {
        Wh_Log(
            L"CreateProcessW failed for powercfg fallback: %s. Error: %u",
            g_plans[index].name,
            GetLastError()
        );
        return false;
    }

    WaitForSingleObject(pi.hProcess, 5000);

    DWORD exitCode = 1;
    GetExitCodeProcess(pi.hProcess, &exitCode);

    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);

    if (exitCode == 0) {
        Wh_Log(
            L"powercfg fallback changed active power plan to: %s",
            g_plans[index].name
        );
        return true;
    }

    Wh_Log(
        L"powercfg fallback failed for %s. Exit code: %u",
        g_plans[index].name,
        exitCode
    );

    return false;
}

int FindPlanIndexByGuid(const GUID& guid) {
    for (int i = 0; i < g_planCount; i++) {
        if (GuidEquals(guid, g_plans[i].guid)) {
            return i;
        }
    }

    if (g_ultimateAvailable && GuidEquals(guid, g_ultimateRuntimeGuid)) {
        return 3;
    }

    return -1;
}

int GetCurrentPowerPlanIndex() {
    GUID* activeGuid = nullptr;

    DWORD result = PowerGetActiveScheme(nullptr, &activeGuid);
    if (result != ERROR_SUCCESS || !activeGuid) {
        Wh_Log(L"PowerGetActiveScheme failed. Error: %u", result);
        return g_currentPlanIndex;
    }

    int index = FindPlanIndexByGuid(*activeGuid);

    LocalFree(activeGuid);

    if (index < 0) {
        Wh_Log(L"Active power plan is not one of the configured plans.");
        return g_currentPlanIndex;
    }

    return index;
}

bool SetPowerPlanByIndex(int index) {
    if (index < 0 || index >= g_planCount) {
        return false;
    }

    // Ultimate Performance is only a template on this system unless it was
    // created with: powercfg -duplicatescheme e9a42b02-d5df-448d-aa00-03f14749eb61
    // As long as it doesn't appear in "powercfg /list", don't try to activate it.
    if (index == 3 && !g_ultimateAvailable) {
        Wh_Log(L"Ultimate Performance is not available. Use the menu item to create it first.");
        return false;
    }

    if (!IsPowerPlanAvailable(g_plans[index].guid)) {
        Wh_Log(L"Power plan not available: %s", g_plans[index].name);
        return false;
    }

    GUID runtimeGuid = GetRuntimePlanGuid(index);
    DWORD result = PowerSetActiveScheme(nullptr, &runtimeGuid);

    if (result == ERROR_SUCCESS) {
        g_currentPlanIndex = index;

        Wh_Log(L"Active power plan changed to: %s", g_plans[index].name);

        return true;
    }

    Wh_Log(
        L"PowerSetActiveScheme failed for %s. Error: %u. Trying powercfg fallback.",
        g_plans[index].name,
        result
    );

    if (SetPowerPlanWithPowercfg(index)) {
        g_currentPlanIndex = GetCurrentPowerPlanIndex();
        return true;
    }

    return false;
}

bool CreateUltimatePerformancePlan() {
    Wh_Log(L"Creating Ultimate Performance power scheme via powercfg -duplicatescheme...");

    SHELLEXECUTEINFOW sei = {};
    sei.cbSize = sizeof(sei);
    sei.fMask = SEE_MASK_NOCLOSEPROCESS;
    sei.lpVerb = L"runas";
    sei.lpFile = L"powercfg.exe";
    sei.lpParameters = L"-duplicatescheme e9a42b02-d5df-448d-aa00-03f14749eb61";
    sei.nShow = SW_HIDE;

    if (!ShellExecuteExW(&sei)) {
        DWORD error = GetLastError();

        if (error == ERROR_CANCELLED) {
            Wh_Log(L"User cancelled UAC prompt for creating Ultimate Performance.");
        } else {
            Wh_Log(L"Failed to start elevated powercfg duplicatescheme. Error: %u", error);
        }

        return false;
    }

    if (sei.hProcess) {
        WaitForSingleObject(sei.hProcess, 10000);

        DWORD exitCode = 1;
        GetExitCodeProcess(sei.hProcess, &exitCode);
        CloseHandle(sei.hProcess);

        if (exitCode != 0) {
            Wh_Log(L"powercfg -duplicatescheme failed. Exit code: %u", exitCode);
            return false;
        }
    }

    Sleep(500);

    RefreshUltimatePerformanceAvailability();

    if (g_ultimateAvailable) {
        Wh_Log(L"Ultimate Performance was created and detected successfully.");
        return true;
    }

    Wh_Log(L"Ultimate Performance creation finished, but the plan was not detected.");
    return false;
}

void OpenWindhawk() {
    if (!g_windhawkPath[0]) {
        Wh_Log(L"Windhawk path is empty.");
        return;
    }

    SHELLEXECUTEINFOW sei = {};
    sei.cbSize = sizeof(sei);
    sei.lpFile = g_windhawkPath;
    sei.nShow = SW_SHOWNORMAL;

    if (!ShellExecuteExW(&sei)) {
        Wh_Log(L"Failed to open Windhawk. Error: %u", GetLastError());
    }
}

void OpenClassicPowerOptions() {
    ShellExecuteW(
        nullptr,
        L"open",
        L"control.exe",
        L"powercfg.cpl",
        nullptr,
        SW_SHOWNORMAL
    );
}

void OpenModernPowerSettings() {
    ShellExecuteW(
        nullptr,
        L"open",
        L"ms-settings:powersleep",
        nullptr,
        nullptr,
        SW_SHOWNORMAL
    );
}

// ------------------------------------------------------------
// Tray icon drawing
// ------------------------------------------------------------

HICON CreatePowerTrayIcon(int planIndex) {
    const int size = 32;

    if (planIndex < 0 || planIndex >= g_planCount) {
        planIndex = 1;
    }

    COLORREF color = g_plans[planIndex].color;
    const WCHAR* text = g_plans[planIndex].shortName;

    BITMAPINFO bi = {};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = size;
    bi.bmiHeader.biHeight = -size;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;

    DWORD* pixels = nullptr;

    HDC hdc = GetDC(nullptr);
    HBITMAP hbmColor = CreateDIBSection(
        hdc,
        &bi,
        DIB_RGB_COLORS,
        reinterpret_cast<void**>(&pixels),
        nullptr,
        0
    );
    ReleaseDC(nullptr, hdc);

    if (!hbmColor || !pixels) {
        return LoadIconW(nullptr, IDI_APPLICATION);
    }

    for (int i = 0; i < size * size; i++) {
        pixels[i] = 0x00000000;
    }

    HDC iconHdc = CreateCompatibleDC(nullptr);
    if (!iconHdc) {
        DeleteObject(hbmColor);
        return LoadIconW(nullptr, IDI_APPLICATION);
    }

    HBITMAP oldBitmap = (HBITMAP)SelectObject(iconHdc, hbmColor);

    HBRUSH bgBrush = CreateSolidBrush(RGB(18, 18, 18));
    HBRUSH oldBrush = (HBRUSH)SelectObject(iconHdc, bgBrush);

    HPEN borderPen = CreatePen(PS_SOLID, 3, color);
    HPEN oldPen = (HPEN)SelectObject(iconHdc, borderPen);

    RoundRect(iconHdc, 1, 1, 31, 31, 8, 8);

    SelectObject(iconHdc, oldPen);
    DeleteObject(borderPen);

    SelectObject(iconHdc, oldBrush);
    DeleteObject(bgBrush);

    SetBkMode(iconHdc, TRANSPARENT);

    HFONT hFont = CreateFontW(
        25,
        0,
        0,
        0,
        FW_HEAVY,
        FALSE,
        FALSE,
        FALSE,
        DEFAULT_CHARSET,
        OUT_DEFAULT_PRECIS,
        CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY,
        DEFAULT_PITCH | FF_SWISS,
        L"Segoe UI"
    );

    HFONT oldFont = (HFONT)SelectObject(iconHdc, hFont);

    RECT textRect = {0, 0, 32, 31};

    RECT shadowRect = textRect;
    OffsetRect(&shadowRect, 1, 1);

    SetTextColor(iconHdc, RGB(0, 0, 0));
    DrawTextW(
        iconHdc,
        text,
        -1,
        &shadowRect,
        DT_CENTER | DT_VCENTER | DT_SINGLELINE
    );

    SetTextColor(iconHdc, color);
    DrawTextW(
        iconHdc,
        text,
        -1,
        &textRect,
        DT_CENTER | DT_VCENTER | DT_SINGLELINE
    );

    SelectObject(iconHdc, oldFont);
    DeleteObject(hFont);

    SelectObject(iconHdc, oldBitmap);
    DeleteDC(iconHdc);

    for (int i = 0; i < size * size; i++) {
        if ((pixels[i] & 0x00FFFFFF) != 0) {
            pixels[i] |= 0xFF000000;
        }
    }

    HBITMAP hbmMask = CreateBitmap(size, size, 1, 1, nullptr);
    if (!hbmMask) {
        DeleteObject(hbmColor);
        return LoadIconW(nullptr, IDI_APPLICATION);
    }

    ICONINFO iconInfo = {};
    iconInfo.fIcon = TRUE;
    iconInfo.hbmColor = hbmColor;
    iconInfo.hbmMask = hbmMask;

    HICON hIcon = CreateIconIndirect(&iconInfo);

    DeleteObject(hbmColor);
    DeleteObject(hbmMask);

    if (!hIcon) {
        return LoadIconW(nullptr, IDI_APPLICATION);
    }

    return hIcon;
}

// ------------------------------------------------------------
// Tray icon handling
// ------------------------------------------------------------

void RefreshTrayIconRect() {
    if (!g_hwnd) {
        return;
    }

    NOTIFYICONIDENTIFIER nii = {};
    nii.cbSize = sizeof(nii);
    nii.hWnd = g_hwnd;
    nii.uID = ID_TRAYICON;

    HRESULT hr = Shell_NotifyIconGetRect(&nii, &g_trayIconRect);
    if (FAILED(hr)) {
        SetRectEmpty(&g_trayIconRect);
        Wh_Log(L"Shell_NotifyIconGetRect failed. HRESULT: 0x%08X", hr);
        return;
    }
}

void StartTrayRectRefreshTimer() {
    if (!g_hwnd) {
        return;
    }

    g_trayRectRefreshAttempts = 0;

    SetTimer(
        g_hwnd,
        TIMER_TRAY_RECT_REFRESH,
        TRAY_RECT_REFRESH_INTERVAL_MS,
        nullptr
    );

    Wh_Log(L"Tray icon rect refresh timer started.");
}

void UpdateTrayTooltip() {
    if (!g_hwnd) {
        return;
    }

    g_currentPlanIndex = GetCurrentPowerPlanIndex();

    WCHAR tip[128] = {};
    StringCchPrintfW(
        tip,
        ARRAYSIZE(tip),
        L"Power Options Button\nActive: %s\nMouse wheel: switch\nRight-click: menu",
        g_plans[g_currentPlanIndex].name
    );

    StringCchCopyW(g_nid.szTip, ARRAYSIZE(g_nid.szTip), tip);

    g_nid.uFlags = NIF_TIP | NIF_SHOWTIP;
    Shell_NotifyIconW(NIM_MODIFY, &g_nid);
}

void UpdateTrayIcon() {
    if (!g_hwnd) {
        return;
    }

    g_currentPlanIndex = GetCurrentPowerPlanIndex();

    HICON newIcon = CreatePowerTrayIcon(g_currentPlanIndex);
    if (!newIcon) {
        return;
    }

    HICON oldIcon = g_hIcon;
    g_hIcon = newIcon;

    g_nid.uFlags = NIF_ICON;
    g_nid.hIcon = g_hIcon;
    Shell_NotifyIconW(NIM_MODIFY, &g_nid);

    UpdateTrayTooltip();
    RefreshTrayIconRect();

    if (oldIcon) {
        DestroyIcon(oldIcon);
    }
}

bool AddTrayIcon() {
    if (!g_hwnd) {
        return false;
    }

    g_currentPlanIndex = GetCurrentPowerPlanIndex();

    if (!g_hIcon) {
        g_hIcon = CreatePowerTrayIcon(g_currentPlanIndex);
    }

    ZeroMemory(&g_nid, sizeof(g_nid));

    g_nid.cbSize = sizeof(g_nid);
    g_nid.hWnd = g_hwnd;
    g_nid.uID = ID_TRAYICON;
    g_nid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP | NIF_SHOWTIP;
    g_nid.uCallbackMessage = WM_TRAYICON;
    g_nid.hIcon = g_hIcon;

    StringCchPrintfW(
        g_nid.szTip,
        ARRAYSIZE(g_nid.szTip),
        L"Power Options Button\nActive: %s\nMouse wheel: switch\nRight-click: menu",
        g_plans[g_currentPlanIndex].name
    );

    BOOL result = Shell_NotifyIconW(NIM_ADD, &g_nid);

    if (result) {
        NOTIFYICONDATAW nidVer = {};
        nidVer.cbSize = sizeof(nidVer);
        nidVer.hWnd = g_hwnd;
        nidVer.uID = ID_TRAYICON;
        nidVer.uVersion = NOTIFYICON_VERSION_4;
        Shell_NotifyIconW(NIM_SETVERSION, &nidVer);

        Wh_Log(L"Power Options Button tray icon added.");
        g_mouseHookRefreshedAfterFirstHover = false;
        RefreshTrayIconRect();
        StartTrayRectRefreshTimer();

        SetTimer(
            g_hwnd,
            TIMER_POST_START_ICON_SYNC,
            POST_START_ICON_SYNC_DELAY_MS,
            nullptr
        );

        Wh_Log(L"Post-start tray icon sync timer started.");
    } else {
        Wh_Log(L"Failed to add tray icon. Error: %u", GetLastError());
    }

    return result != FALSE;
}

void RemoveTrayIcon() {
    if (!g_hwnd) {
        return;
    }

    NOTIFYICONDATAW nid = {};
    nid.cbSize = sizeof(nid);
    nid.hWnd = g_hwnd;
    nid.uID = ID_TRAYICON;

    Shell_NotifyIconW(NIM_DELETE, &nid);

    if (g_hIcon) {
        DestroyIcon(g_hIcon);
        g_hIcon = nullptr;
    }

    Wh_Log(L"Power Options Button tray icon removed.");
}

// ------------------------------------------------------------
// Mouse wheel over tray icon
// ------------------------------------------------------------

bool IsPointNearTrayIcon(POINT pt) {
    // The tray can be rearranged after Windows/Explorer startup.
    // Always refresh the icon rect before testing mouse wheel position.
    RefreshTrayIconRect();

    RECT rect = g_trayIconRect;

    if (IsRectEmpty(&rect)) {
        return false;
    }

    // Small tolerance for DPI/scaling/border edge cases.
    InflateRect(&rect, 4, 4);

    return PtInRect(&rect, pt) != FALSE;
}

LRESULT CALLBACK LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION && wParam == WM_MOUSEWHEEL) {
        const MSLLHOOKSTRUCT* ms =
            reinterpret_cast<const MSLLHOOKSTRUCT*>(lParam);

        short delta = static_cast<short>(HIWORD(ms->mouseData));
        int direction = (delta > 0) ? 1 : -1;

        DWORD now = GetTickCount();

        RECT rectBefore = g_trayIconRect;

        BOOL inside = IsPointNearTrayIcon(ms->pt);

        RECT rectAfter = g_trayIconRect;

        bool acceptedByMouseMoveFallback = false;

        // Fallback:
        // If the tray icon sent WM_MOUSEMOVE recently, we know the cursor was
        // over our tray icon even if Shell_NotifyIconGetRect is stale/wrong.
        if (!inside &&
            IsRectEmpty(&rectAfter) &&
            g_lastTrayMouseMoveTime != 0 &&
            now - g_lastTrayMouseMoveTime < 500) {
            inside = TRUE;
            acceptedByMouseMoveFallback = true;
        }

#if DEBUG_MOUSE_WHEEL
        if (inside || now - g_lastWheelDebugLog > 250) {
            g_lastWheelDebugLog = now;

            Wh_Log(
                L"MouseWheel hook: pt=(%ld,%ld), delta=%d, direction=%d, "
                L"inside=%d, fallback=%d, hwnd=0x%p, "
                L"rectBefore empty=%d [%ld,%ld,%ld,%ld], "
                L"rectAfter empty=%d [%ld,%ld,%ld,%ld], "
                L"lastTrayMouseMoveAge=%lu",
                ms->pt.x,
                ms->pt.y,
                delta,
                direction,
                inside,
                acceptedByMouseMoveFallback,
                g_hwnd,
                IsRectEmpty(&rectBefore),
                rectBefore.left,
                rectBefore.top,
                rectBefore.right,
                rectBefore.bottom,
                IsRectEmpty(&rectAfter),
                rectAfter.left,
                rectAfter.top,
                rectAfter.right,
                rectAfter.bottom,
                g_lastTrayMouseMoveTime ? now - g_lastTrayMouseMoveTime : 999999
            );
        }
#endif

        if (inside && g_hwnd) {
            PostMessageW(
                g_hwnd,
                WM_TRAY_SCROLL,
                static_cast<WPARAM>(direction),
                0
            );

            return 1;
        }
    }

    return CallNextHookEx(g_mouseHook, nCode, wParam, lParam);
}

bool InstallMouseWheelHook() {
    if (g_mouseHook) {
        return true;
    }

    g_mouseHook = SetWindowsHookExW(
        WH_MOUSE_LL,
        LowLevelMouseProc,
        GetModuleHandleW(nullptr),
        0
    );

    if (!g_mouseHook) {
        Wh_Log(L"Failed to install mouse wheel hook. Error: %u", GetLastError());
        return false;
    }

    g_mouseHookInstalledAt = GetTickCount();

    Wh_Log(
        L"Mouse wheel hook installed. HHOOK=0x%p, thread=%lu, hwnd=0x%p",
        g_mouseHook,
        GetCurrentThreadId(),
        g_hwnd
    );

    return true;
}

void UninstallMouseWheelHook() {
    if (g_mouseHook) {
        UnhookWindowsHookEx(g_mouseHook);
        g_mouseHook = nullptr;
        Wh_Log(L"Mouse wheel hook uninstalled.");
    }
}

bool RefreshMouseWheelHook(PCWSTR reason) {
    DWORD now = GetTickCount();

    // Avoid repeated hook reinstall spam.
    if (g_lastMouseHookRefresh && now - g_lastMouseHookRefresh < 1000) {
        Wh_Log(
            L"Mouse wheel hook refresh skipped. Reason: %s, last refresh age=%lu ms",
            reason,
            now - g_lastMouseHookRefresh
        );

        return g_mouseHook != nullptr;
    }

    g_lastMouseHookRefresh = now;

    Wh_Log(
        L"Refreshing mouse wheel hook. Reason: %s, old HHOOK=0x%p",
        reason,
        g_mouseHook
    );

    UninstallMouseWheelHook();

    bool result = InstallMouseWheelHook();

    Wh_Log(
        L"Mouse wheel hook refresh completed. Reason: %s, success=%d, new HHOOK=0x%p",
        reason,
        result,
        g_mouseHook
    );

    return result;
}

void CyclePowerPlan(int direction) {
    g_currentPlanIndex = GetCurrentPowerPlanIndex();

    int nextIndex = g_currentPlanIndex;

    for (int attempt = 0; attempt < g_planCount; attempt++) {
        nextIndex += direction;

        if (nextIndex >= g_planCount) {
            nextIndex = 0;
        } else if (nextIndex < 0) {
            nextIndex = g_planCount - 1;
        }

        if (IsPowerPlanAvailable(g_plans[nextIndex].guid)) {
            break;
        }
    }

    if (SetPowerPlanByIndex(nextIndex)) {
        UpdateTrayIcon();
    }
}

// ------------------------------------------------------------
// Context menu
// ------------------------------------------------------------

void ShowContextMenu() {
    if (!g_hwnd) {
        return;
    }

    g_currentPlanIndex = GetCurrentPowerPlanIndex();
    RefreshUltimatePerformanceAvailability();

    HMENU menu = CreatePopupMenu();
    if (!menu) {
        return;
    }

    for (int i = 0; i < g_planCount; i++) {
        UINT flags = MF_STRING;

        if (!IsPowerPlanAvailable(g_plans[i].guid)) {
            flags |= MF_GRAYED;
        }

        if (i == g_currentPlanIndex) {
            flags |= MF_CHECKED;
        }

        PCWSTR menuText = g_plans[i].name;

        if (i == 3) {
            menuText = L"Ultimate Performance";
        }

        AppendMenuW(
            menu,
            flags,
            g_plans[i].menuId,
            menuText
        );
    }

    if (!g_ultimateAvailable) {
        AppendMenuW(menu, MF_STRING, ID_MENU_CREATE_ULTIMATE, L"Create Ultimate Performance...");
    }

    AppendMenuW(menu, MF_SEPARATOR, 0, nullptr);
    AppendMenuW(menu, MF_STRING, ID_MENU_REFRESH, L"Refresh");
    AppendMenuW(menu, MF_SEPARATOR, 0, nullptr);
    AppendMenuW(menu, MF_STRING, ID_MENU_OPEN_POWER, L"Open classic Power Options");
    AppendMenuW(menu, MF_STRING, ID_MENU_OPEN_SETTINGS, L"Open Windows power settings");
    AppendMenuW(menu, MF_STRING, ID_MENU_OPEN_WINDHAWK, L"Open Windhawk");

    POINT pt;
    GetCursorPos(&pt);

    SetForegroundWindow(g_hwnd);

    TrackPopupMenu(
        menu,
        TPM_RIGHTBUTTON,
        pt.x,
        pt.y,
        0,
        g_hwnd,
        nullptr
    );

    DestroyMenu(menu);
}

// ------------------------------------------------------------
// Window procedure
// ------------------------------------------------------------

LRESULT CALLBACK HiddenWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == g_taskbarCreatedMessage) {
        Wh_Log(L"TaskbarCreated received. Re-adding tray icon.");
        AddTrayIcon();
        UpdateTrayIcon();
        return 0;
    }

    switch (msg) {
    case WM_TIMER:
        if (wParam == TIMER_TRAY_RECT_REFRESH) {
            RefreshTrayIconRect();

            g_trayRectRefreshAttempts++;

            if (!IsRectEmpty(&g_trayIconRect)) {
                KillTimer(hwnd, TIMER_TRAY_RECT_REFRESH);

                Wh_Log(
                    L"Tray icon rect detected after %d attempt(s): left=%ld top=%ld right=%ld bottom=%ld",
                    g_trayRectRefreshAttempts,
                    g_trayIconRect.left,
                    g_trayIconRect.top,
                    g_trayIconRect.right,
                    g_trayIconRect.bottom
                );

                return 0;
            }

            if (g_trayRectRefreshAttempts >= TRAY_RECT_REFRESH_MAX_ATTEMPTS) {
                KillTimer(hwnd, TIMER_TRAY_RECT_REFRESH);

                Wh_Log(L"Tray icon rect refresh timer stopped. Tray rect still empty.");

                return 0;
            }

            return 0;
        }

            if (wParam == TIMER_POST_START_ICON_SYNC) {
                KillTimer(hwnd, TIMER_POST_START_ICON_SYNC);

                Wh_Log(L"Post-start tray icon sync running.");

                UpdateTrayIcon();
                RefreshTrayIconRect();
                RefreshMouseWheelHook(L"post-start tray icon sync");

                Wh_Log(L"Post-start tray icon sync completed.");

                return 0;
            }

        break;

    case WM_CREATE:
        g_hwnd = hwnd;
        Wh_Log(L"Hidden window created. HWND: 0x%p", hwnd);
        return 0;

    case WM_DESTROY:
        KillTimer(hwnd, TIMER_TRAY_RECT_REFRESH);
        KillTimer(hwnd, TIMER_POST_START_ICON_SYNC);
        RemoveTrayIcon();
        UninstallMouseWheelHook();
        PostQuitMessage(0);
        return 0;

    case WM_TRAYICON: {
        UINT event = LOWORD(lParam);

        if (event == WM_RBUTTONUP || event == WM_CONTEXTMENU) {
            ShowContextMenu();
            return 0;
        }

        if (event == WM_LBUTTONUP || event == NIN_SELECT || event == NIN_KEYSELECT) {
            UpdateTrayIcon();
            return 0;
        }

        if (event == WM_MOUSEMOVE) {
            DWORD now = GetTickCount();

            g_lastTrayMouseMoveTime = now;

            RefreshTrayIconRect();

        #if DEBUG_MOUSE_WHEEL
            if (now - g_lastTrayMouseMoveLog > 500) {
                g_lastTrayMouseMoveLog = now;

                Wh_Log(
                    L"Tray icon WM_MOUSEMOVE: rect empty=%d [%ld,%ld,%ld,%ld]",
                    IsRectEmpty(&g_trayIconRect),
                    g_trayIconRect.left,
                    g_trayIconRect.top,
                    g_trayIconRect.right,
                    g_trayIconRect.bottom
                );
            }
        #endif

            if (!g_mouseHookRefreshedAfterFirstHover) {
                g_mouseHookRefreshedAfterFirstHover = true;
                RefreshMouseWheelHook(L"first tray icon hover");
            }

            return 0;
        }

        return 0;
    }

    case WM_TRAY_SCROLL: {
        DWORD now = GetTickCount();
        DWORD diff = now - g_lastScrollTime;

        int direction = static_cast<int>(wParam);

        #if DEBUG_MOUSE_WHEEL
            Wh_Log(
                L"WM_TRAY_SCROLL received: direction=%d, diff=%lu ms",
                direction,
                diff
            );
        #endif

            if (diff > 150) {
                g_lastScrollTime = now;
                CyclePowerPlan(direction);
            } else {
        #if DEBUG_MOUSE_WHEEL
                Wh_Log(L"WM_TRAY_SCROLL ignored by debounce.");
        #endif
            }

            return 0;
        }

    case WM_COMMAND: {
        int id = LOWORD(wParam);

        for (int i = 0; i < g_planCount; i++) {
            if (id == g_plans[i].menuId) {
                if (SetPowerPlanByIndex(i)) {
                    UpdateTrayIcon();
                }
                return 0;
            }
        }

        switch (id) {
        case ID_MENU_CREATE_ULTIMATE:
            if (CreateUltimatePerformancePlan()) {
                UpdateTrayIcon();
            }
            return 0;

        case ID_MENU_REFRESH:
            Wh_Log(L"Manual refresh requested from tray menu.");
            RefreshUltimatePerformanceAvailability();
            UpdateTrayIcon();

            {
                PCWSTR currentName = L"Unknown";

                if (g_currentPlanIndex >= 0 && g_currentPlanIndex < g_planCount) {
                    currentName = g_plans[g_currentPlanIndex].name;
                }

                Wh_Log(
                    L"Manual refresh completed. Current plan: %s",
                    currentName
                );
            }

            return 0;

        case ID_MENU_OPEN_POWER:
            OpenClassicPowerOptions();
            return 0;

        case ID_MENU_OPEN_SETTINGS:
            OpenModernPowerSettings();
            return 0;

        case ID_MENU_OPEN_WINDHAWK:
            OpenWindhawk();
            return 0;
        }

        return 0;
    }

    case WM_APP_EXIT:
        DestroyWindow(hwnd);
        return 0;
    }

    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

// ------------------------------------------------------------
// UI thread
// ------------------------------------------------------------

DWORD WINAPI UiThreadProc(LPVOID) {
    g_taskbarCreatedMessage = RegisterWindowMessageW(L"TaskbarCreated");

    WNDCLASSW wc = {};
    wc.lpfnWndProc = HiddenWndProc;
    wc.hInstance = GetModuleHandleW(nullptr);
    wc.lpszClassName = HIDDEN_WINDOW_CLASS;

    RegisterClassW(&wc);

    HINSTANCE hInstance = GetModuleHandleW(nullptr);

    HWND hwnd = CreateWindowExW(
        0,
        HIDDEN_WINDOW_CLASS,
        L"Power Options Button",
        WS_OVERLAPPED,
        0,
        0,
        0,
        0,
        nullptr,
        nullptr,
        hInstance,
        nullptr
    );

    if (!hwnd) {
        Wh_Log(L"Failed to create hidden window. Error: %u", GetLastError());
        return 0;
    }

    g_hwnd = hwnd;

    if (AddTrayIcon()) {
        RefreshTrayIconRect();
        InstallMouseWheelHook();
    } else {
        Wh_Log(L"AddTrayIcon returned false after CreateWindowExW. Mouse hook not installed.");
    }

    MSG msg;
    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    g_hwnd = nullptr;

    return 0;
}

// ------------------------------------------------------------
// Windhawk lifecycle
// ------------------------------------------------------------

BOOL WhTool_ModInit() {
    Wh_Log(L"Initializing Power Options Button...");

    GetModuleFileNameW(nullptr, g_windhawkPath, ARRAYSIZE(g_windhawkPath));

    RefreshUltimatePerformanceAvailability();
    g_currentPlanIndex = GetCurrentPowerPlanIndex();

    g_uiThread = CreateThread(
        nullptr,
        0,
        UiThreadProc,
        nullptr,
        0,
        &g_uiThreadId
    );

    if (!g_uiThread) {
        Wh_Log(L"Failed to create UI thread. Error: %u", GetLastError());
        return FALSE;
    }

    Wh_Log(L"Power Options Button initialized.");
    return TRUE;
}

void WhTool_ModUninit() {
    Wh_Log(L"Uninitializing Power Options Button...");

    if (g_hwnd) {
        PostMessageW(g_hwnd, WM_APP_EXIT, 0, 0);
    } else if (g_uiThreadId) {
        PostThreadMessageW(g_uiThreadId, WM_QUIT, 0, 0);
    }

    if (g_uiThread) {
        WaitForSingleObject(g_uiThread, 3000);
        CloseHandle(g_uiThread);
        g_uiThread = nullptr;
    }

    g_uiThreadId = 0;

    if (g_hIcon) {
        DestroyIcon(g_hIcon);
        g_hIcon = nullptr;
    }

    Wh_Log(L"Power Options Button uninitialized.");
}

void WhTool_ModSettingsChanged() {
    // No Windhawk settings yet.
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
    LPWSTR* argv = CommandLineToArgvW(GetCommandLine(), &argc);
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
            CreateMutex(nullptr, TRUE, L"windhawk-tool-mod_" WH_MOD_ID);

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
            (IMAGE_DOS_HEADER*)GetModuleHandle(nullptr);

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
    switch (GetModuleFileName(nullptr, currentProcessPath,
                              ARRAYSIZE(currentProcessPath))) {
        case 0:
        case ARRAYSIZE(currentProcessPath):
            Wh_Log(L"GetModuleFileName failed");
            return;
    }

    WCHAR commandLine[
        MAX_PATH + 2 +
        (sizeof(L" -tool-mod \"" WH_MOD_ID "\"") / sizeof(WCHAR)) - 1
    ];

    wsprintfW(
        commandLine,
        L"\"%s\" -tool-mod \"%s\"",
        currentProcessPath,
        WH_MOD_ID
    );

    HMODULE kernelModule = GetModuleHandle(L"kernelbase.dll");
    if (!kernelModule) {
        kernelModule = GetModuleHandle(L"kernel32.dll");
        if (!kernelModule) {
            Wh_Log(L"No kernelbase.dll/kernel32.dll");
            return;
        }
    }

    using CreateProcessInternalW_t = BOOL(WINAPI*)(
        HANDLE hUserToken,
        LPCWSTR lpApplicationName,
        LPWSTR lpCommandLine,
        LPSECURITY_ATTRIBUTES lpProcessAttributes,
        LPSECURITY_ATTRIBUTES lpThreadAttributes,
        WINBOOL bInheritHandles,
        DWORD dwCreationFlags,
        LPVOID lpEnvironment,
        LPCWSTR lpCurrentDirectory,
        LPSTARTUPINFOW lpStartupInfo,
        LPPROCESS_INFORMATION lpProcessInformation,
        PHANDLE hRestrictedUserToken
    );

    CreateProcessInternalW_t pCreateProcessInternalW =
        (CreateProcessInternalW_t)GetProcAddress(
            kernelModule,
            "CreateProcessInternalW"
        );

    if (!pCreateProcessInternalW) {
        Wh_Log(L"No CreateProcessInternalW");
        return;
    }

    STARTUPINFO si{
        .cb = sizeof(STARTUPINFO),
        .dwFlags = STARTF_FORCEOFFFEEDBACK,
    };

    PROCESS_INFORMATION pi;

    if (!pCreateProcessInternalW(
            nullptr,
            currentProcessPath,
            commandLine,
            nullptr,
            nullptr,
            FALSE,
            NORMAL_PRIORITY_CLASS,
            nullptr,
            nullptr,
            &si,
            &pi,
            nullptr)) {
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
