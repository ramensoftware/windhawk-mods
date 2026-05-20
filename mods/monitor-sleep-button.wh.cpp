// ==WindhawkMod==
// @id              monitor-sleep-button
// @name            Monitor Sleep Button
// @description     Tray icon to turn off the monitor after a configurable countdown.
// @version         1.1.0
// @author          SilverAmd
// @github          https://github.com/SilverAmd
// @homepage        https://github.com/SilverAmd
// @include         windhawk.exe
// @compilerOptions -luser32 -lshell32 -lgdi32
// @license         MIT
// ==/WindhawkMod==


// ==WindhawkModReadme==
/*
# Monitor Sleep Button

Adds a small tray icon for quickly turning off the monitor after a configurable countdown.

## Features

- Tray icon for quick monitor power-off
- Left-click the tray icon to start the countdown
- Right-click the tray icon to open a context menu
- Configurable countdown duration
- Optional black screen mode for instant blackout without monitor standby
- Black screen mode can be closed with Esc or mouse click
- Optional configurable global hotkey
- Supports `Ctrl`, `Alt`, `Shift` and Windows key modifiers
- Default hotkey: `Ctrl + M`
- Hotkey can be enabled or disabled in the mod settings
- Tray menu can temporarily enable or disable the hotkey for the current session
- Countdown overlay before the monitor turns off
- Countdown can be cancelled from the tray menu
- Countdown can be cancelled with `Esc`
- Automatically restores the tray icon after taskbar/explorer restart
- Mouse wheel over the tray icon changes the countdown duration
- Tray icon displays the currently selected countdown duration
- Tray menu shortcut to open Windhawk

## Usage

- Left-click the tray icon to start the monitor sleep countdown.
- Right-click the tray icon to open the tray menu.
- Press the configured hotkey to start the countdown. The default is `Ctrl + M`.
- Press `Esc` while the countdown is running to cancel it.
- Move the mouse or press any key to wake the monitor again.
- Scroll the mouse wheel over the tray icon to temporarily change the countdown duration.

## Tray Menu

The tray icon context menu provides quick access to:

- Turn monitor off
- Cancel countdown
- Enable or disable the configured hotkey for the current session
- Open Windhawk

The countdown can also be cancelled by pressing `Esc` while it is running.

The tray menu hotkey toggle is temporary.  
To change the default hotkey behavior permanently, use the Windhawk mod settings.

## Settings

### Countdown

The countdown duration can be configured in the mod settings.

Available values:

- 1 second
- 2 seconds
- 3 seconds
- 5 seconds
- 10 seconds
- 15 seconds
- 30 seconds

### Action mode

The mod supports two action modes:

- Turn monitor off: Sends the Windows monitor power-off command.
- Show black screen: Shows a fullscreen black overlay while keeping the monitor active. Black screen mode is ideal when you want an instant blackout and instant wake-up without waiting for the monitor to leave standby. 



### Hotkey

The global hotkey can be enabled or disabled in the mod settings.

Default:

- `Ctrl + M`

`M` stands for monitor.

The `Esc` key can be used to cancel a running countdown.

### Hotkey conflicts

The hotkey supports `Ctrl`, `Alt`, `Shift` and the Windows key as modifiers.

Examples:

- `Ctrl + M`
- `Alt + M`
- `Ctrl + Shift + S`
- `Ctrl + Alt + S`
- `Ctrl + Win + J`

Some hotkey combinations may already be used by Windows or other applications, for example `Win + S` for Windows Search.

If a selected hotkey does not work, choose another key combination.

NirSoft HotKeysList can help you check which global hotkeys are already registered on your system:

https://www.nirsoft.net/utils/hot_keys_list.html

## Credits

Based on and inspired by an old PowerShell script:

```batch
@echo off
powershell -windowstyle hidden (Add-Type '[DllImport(\"user32.dll\")]^public static extern int PostMessage(int hWnd, int hMsg, int wParam, int lParam);' -Name a -Pas)::PostMessage(-1,0x0112,0xF170,2)
```

Additional development help and debugging guidance: ChatGPT by OpenAI.

## Notes

This mod does not shut down, suspend, hibernate, or lock the computer.  
It only sends the Windows monitor power-off command.

Useful as a quick replacement for desktop shortcuts, batch files, or context menu entries.

This is helpful when long-running downloads, renders, AI model installs, backups, or other background tasks are running and the monitor should be turned off without putting the PC to sleep.

Black screen mode does not save as much power as turning the monitor off, because the display remains active.
*/
// ==/WindhawkModReadme==


// ==WindhawkModSettings==
/*
- countdownSeconds: "3"
  $name: "Countdown seconds"
  $description: "Number of seconds to show before turning off the monitor. Mouse wheel changes over the tray icon are temporary for the current session. The default countdown duration is still controlled by the Windhawk mod settings."
  $options:
    - "1": "1 second"
    - "2": "2 seconds"
    - "3": "3 seconds"
    - "5": "5 seconds"
    - "10": "10 seconds"
    - "15": "15 seconds"
    - "30": "30 seconds"

- actionMode: "monitor_off"
  $name: Action mode
  $description: "Choose what happens after the countdown."
  $options:
    - "monitor_off": "Turn monitor off"
    - "black_screen": "Show black screen"

- hotkeyEnabled: true
  $name: Enable hotkey
  $description: "Enable global hotkey for starting the monitor sleep countdown. Default is true"

- hotkeyCtrl: true
  $name: Hotkey Ctrl

- hotkeyAlt: false
  $name: Hotkey Alt

- hotkeyShift: false
  $name: Hotkey Shift

- hotkeyWin: false
  $name: Hotkey Windows key

- hotkeyKey: "M"
  $name: Hotkey key
  $description: "Single key used for the hotkey. Default is M"
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <shellapi.h>
#include <stdlib.h>

#ifndef NIF_SHOWTIP
#define NIF_SHOWTIP 0x00000080
#endif

#define WM_TRAYICON (WM_USER + 1)

#define ID_TRAYICON 1001
#define ID_HOTKEY_MONITOR_OFF 1002
#define ID_HOTKEY_CANCEL_COUNTDOWN 1003
#define ID_HOTKEY_CLOSE_BLACK_SCREEN 1004

#define ID_MENU_TURN_OFF 3001
#define ID_MENU_CANCEL_COUNTDOWN 3002
#define ID_MENU_TOGGLE_HOTKEY 3003
#define ID_MENU_OPEN_WINDHAWK 3004

#define TIMER_COUNTDOWN 2001
#define COUNTDOWN_INTERVAL_MS 1000

#define WM_APP_EXIT (WM_APP + 1)
#define WM_APP_SETTINGS_CHANGED (WM_APP + 2)
#define WM_TRAY_SCROLL (WM_APP + 3)

#define HIDDEN_WINDOW_CLASS L"MonitorSleepButtonHiddenWindow"

#define COUNTDOWN_TRANSPARENT_COLOR RGB(1, 2, 3)


static HWND g_hwnd = nullptr;
static HWND g_countdownHwnd = nullptr;
static HWND g_blackScreenHwnd = nullptr;
static bool g_blackScreenActive = false;

static HICON g_hIcon = nullptr;
static NOTIFYICONDATAW g_nid = {};
static UINT g_taskbarCreatedMessage = 0;

static HHOOK g_mouseHook = nullptr;
static RECT g_trayIconRect = {};
static DWORD g_lastScrollTime = 0;

static bool g_countdownActive = false;
static int g_countdownSeconds = 3;
static int g_countdownValue = 3;
static const int g_countdownOptions[] = {1, 2, 3, 5, 10, 15, 30};
static const int g_countdownOptionCount = ARRAYSIZE(g_countdownOptions);
static bool g_hotkeyEnabled = true;
static bool g_hotkeyRegistered = false;
static UINT g_hotkeyModifiers = MOD_CONTROL;
static UINT g_hotkeyVk = 'M';
static WCHAR g_hotkeyName[64] = L"Ctrl+M";

enum ActionMode {
    ACTION_MONITOR_OFF,
    ACTION_BLACK_SCREEN,
};

static ActionMode g_actionMode = ACTION_MONITOR_OFF;

static HANDLE g_uiThread = nullptr;
static DWORD g_uiThreadId = 0;
static WCHAR g_windhawkPath[MAX_PATH] = {};

// ------------------------------------------------------------
// Monitor ausschalten
// ------------------------------------------------------------

void TurnMonitorOff() {
    Wh_Log(L"Turning monitor off...");

    // Entspricht praktisch:
    // PostMessage(-1, 0x0112, 0xF170, 2)
    PostMessageW(HWND_BROADCAST, WM_SYSCOMMAND, SC_MONITORPOWER, 2);
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

int ReadCountdownSecondsSetting() {
    PCWSTR countdownSecondsW = Wh_GetStringSetting(L"countdownSeconds");

    int countdownSeconds = _wtoi(countdownSecondsW);

    Wh_FreeStringSetting(countdownSecondsW);

    if (countdownSeconds < 1) {
        countdownSeconds = 1;
    }

    if (countdownSeconds > 30) {
        countdownSeconds = 30;
    }

    return countdownSeconds;
}

ActionMode ReadActionModeSetting() {
    PCWSTR actionMode = Wh_GetStringSetting(L"actionMode");

    ActionMode mode = ACTION_MONITOR_OFF;

    if (actionMode && wcscmp(actionMode, L"black_screen") == 0) {
        mode = ACTION_BLACK_SCREEN;
    }

    Wh_FreeStringSetting(actionMode);

    return mode;
}

int GetCountdownOptionIndex(int seconds) {
    for (int i = 0; i < g_countdownOptionCount; i++) {
        if (g_countdownOptions[i] == seconds) {
            return i;
        }
    }

    // Fallback to 3 seconds.
    for (int i = 0; i < g_countdownOptionCount; i++) {
        if (g_countdownOptions[i] == 3) {
            return i;
        }
    }

    return 0;
}

bool ReadHotkeyEnabledSetting() {
    return Wh_GetIntSetting(L"hotkeyEnabled") != 0;
}

UINT ReadHotkeyModifiersSetting() {
    UINT modifiers = 0;

    if (Wh_GetIntSetting(L"hotkeyCtrl")) {
        modifiers |= MOD_CONTROL;
    }

    if (Wh_GetIntSetting(L"hotkeyAlt")) {
        modifiers |= MOD_ALT;
    }

    if (Wh_GetIntSetting(L"hotkeyShift")) {
        modifiers |= MOD_SHIFT;
    }

    if (Wh_GetIntSetting(L"hotkeyWin")) {
        modifiers |= MOD_WIN;
    }

    // Safety fallback: don't allow a modifier-less global hotkey.
    if (modifiers == 0) {
        modifiers = MOD_CONTROL;
    }

    return modifiers;
}

UINT ReadHotkeyKeySetting() {
    PCWSTR hotkeyKey = Wh_GetStringSetting(L"hotkeyKey");

    UINT vk = 'M';

    if (hotkeyKey && hotkeyKey[0]) {
        WCHAR c = hotkeyKey[0];

        if (c >= L'a' && c <= L'z') {
            c = c - L'a' + L'A';
        }

        if ((c >= L'A' && c <= L'Z') || (c >= L'0' && c <= L'9')) {
            vk = (UINT)c;
        }
    }

    Wh_FreeStringSetting(hotkeyKey);

    return vk;
}

void UpdateHotkeyName() {
    g_hotkeyName[0] = L'\0';

    if (g_hotkeyModifiers & MOD_CONTROL) {
        wcscat_s(g_hotkeyName, L"Ctrl+");
    }

    if (g_hotkeyModifiers & MOD_ALT) {
        wcscat_s(g_hotkeyName, L"Alt+");
    }

    if (g_hotkeyModifiers & MOD_SHIFT) {
        wcscat_s(g_hotkeyName, L"Shift+");
    }

    if (g_hotkeyModifiers & MOD_WIN) {
        wcscat_s(g_hotkeyName, L"Win+");
    }

    WCHAR keyText[8] = {};
    keyText[0] = (WCHAR)g_hotkeyVk;
    keyText[1] = L'\0';

    wcscat_s(g_hotkeyName, keyText);
}

void UpdateTrayTooltip(bool countdownRunning) {
    if (countdownRunning) {
        wsprintfW(
            g_nid.szTip,
            L"Monitor Sleep Button - %ds countdown running, RMB: Menu, Esc cancels",
            g_countdownSeconds
        );
    } else {
    PCWSTR actionText =
        g_actionMode == ACTION_BLACK_SCREEN
            ? L"LMB: Black screen"
            : L"LMB: Off";

        wsprintfW(
            g_nid.szTip,
            L"Monitor Sleep Button - %ds, %s, RMB: Menu, %s",
            g_countdownSeconds,
            actionText,
            g_hotkeyName
        );
    }

    g_nid.uFlags = NIF_TIP | NIF_SHOWTIP; 
    Shell_NotifyIconW(NIM_MODIFY, &g_nid);
}

void LoadSettings() {
    g_countdownSeconds = ReadCountdownSecondsSetting();
    g_actionMode = ReadActionModeSetting();
    g_hotkeyEnabled = ReadHotkeyEnabledSetting();
    g_hotkeyModifiers = ReadHotkeyModifiersSetting();
    g_hotkeyVk = ReadHotkeyKeySetting();
    UpdateHotkeyName();
}

bool RegisterMonitorHotkey() {
    if (!g_hwnd || !g_hotkeyEnabled) {
        return false;
    }

    if (g_hotkeyRegistered) {
        UnregisterHotKey(g_hwnd, ID_HOTKEY_MONITOR_OFF);
        g_hotkeyRegistered = false;
    }

    if (!RegisterHotKey(g_hwnd,
                        ID_HOTKEY_MONITOR_OFF,
                        g_hotkeyModifiers,
                        g_hotkeyVk)) {
        Wh_Log(L"Failed to register hotkey %s. Error: %u",
               g_hotkeyName,
               GetLastError());
        return false;
    }

    g_hotkeyRegistered = true;
    Wh_Log(L"Registered hotkey %s.", g_hotkeyName);
    return true;
}

void UnregisterMonitorHotkey() {
    if (g_hwnd && g_hotkeyRegistered) {
        UnregisterHotKey(g_hwnd, ID_HOTKEY_MONITOR_OFF);
        g_hotkeyRegistered = false;
        Wh_Log(L"Unregistered hotkey %s.", g_hotkeyName);
    }
}

void UpdateTrayIcon();

void ApplyRuntimeSettings(int countdownSeconds,
                          ActionMode actionMode,
                          bool hotkeyEnabled,
                          UINT hotkeyModifiers,
                          UINT hotkeyVk) {
    UnregisterMonitorHotkey();

    g_countdownSeconds = countdownSeconds;
    g_actionMode = actionMode;
    g_hotkeyEnabled = hotkeyEnabled;
    g_hotkeyModifiers = hotkeyModifiers;
    g_hotkeyVk = hotkeyVk;
    UpdateHotkeyName();

    if (g_hwnd) {
        UpdateTrayIcon();
        UpdateTrayTooltip(g_countdownActive);
    }

    if (g_hotkeyEnabled) {
        RegisterMonitorHotkey();
    } else {
        Wh_Log(L"Hotkey disabled.");
    }

    Wh_Log(L"Runtime settings applied. Countdown seconds: %d, hotkey enabled: %d, hotkey: %s",
           g_countdownSeconds,
           g_hotkeyEnabled,
           g_hotkeyName);
}

// ------------------------------------------------------------
// Einfaches eigenes Tray-Icon erzeugen
// ------------------------------------------------------------

HICON CreateSimpleTrayIcon(int countdownSeconds) {
    const int size = 32;

    BITMAPINFO bi = {};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = size;
    bi.bmiHeader.biHeight = -size; // top-down bitmap
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

    // Bigger, simpler tray icon:
    // dark monitor-like background + blue border + large countdown number.
    HBRUSH bgBrush = CreateSolidBrush(RGB(20, 24, 28));
    HBRUSH oldBrush = (HBRUSH)SelectObject(iconHdc, bgBrush);

    HPEN borderPen = CreatePen(PS_SOLID, 2, RGB(0, 166, 255));
    HPEN oldPen = (HPEN)SelectObject(iconHdc, borderPen);

    RoundRect(iconHdc, 0, 1, 32, 31, 5, 5);

    SelectObject(iconHdc, oldPen);
    DeleteObject(borderPen);

    SelectObject(iconHdc, oldBrush);
    DeleteObject(bgBrush);

    SetBkMode(iconHdc, TRANSPARENT);

    int fontSize = 28;
    if (countdownSeconds >= 10) {
        fontSize = 22;
    }

    HFONT hFont = CreateFontW(
        fontSize,
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

    WCHAR text[8];
    wsprintfW(text, L"%d", countdownSeconds);

    RECT textRect = {-1, 0, 33, 31};

    // Strong shadow for readability.
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

    SetTextColor(iconHdc, RGB(255, 255, 255));
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

    // Fix alpha after GDI drawing.
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
// Tray-Icon hinzufügen / entfernen
// ------------------------------------------------------------
void RefreshTrayIconRect();
bool IsPointNearTrayIcon(POINT pt) {
    RECT rect = g_trayIconRect;

    // Tray icon rect can be slightly off because of DPI/taskbar scaling.
    // Expand it a bit so mouse wheel over the visible icon is still detected.
    InflateRect(&rect, 32, 48);

    return PtInRect(&rect, pt) != FALSE;
}
bool InstallMouseWheelHook();
void UninstallMouseWheelHook();

bool AddTrayIcon() {
    if (!g_hwnd) {
        return false;
    }

    if (!g_hIcon) {
        g_hIcon = CreateSimpleTrayIcon(g_countdownSeconds);
    }

    ZeroMemory(&g_nid, sizeof(g_nid));

    g_nid.cbSize = sizeof(g_nid);
    g_nid.hWnd = g_hwnd;
    g_nid.uID = ID_TRAYICON;
    g_nid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP | NIF_SHOWTIP;
    g_nid.uCallbackMessage = WM_TRAYICON;
    g_nid.hIcon = g_hIcon;

    PCWSTR actionText =
    g_actionMode == ACTION_BLACK_SCREEN
        ? L"LMB: Black screen"
        : L"LMB: Off";

    wsprintfW(
        g_nid.szTip,
        L"Monitor Sleep Button - %ds, %s, RMB: Menu, %s",
        g_countdownSeconds,
        actionText,
        g_hotkeyName
    );

    BOOL result = Shell_NotifyIconW(NIM_ADD, &g_nid);

    if (result) {
        NOTIFYICONDATAW nidVer = {};
        nidVer.cbSize = sizeof(nidVer);
        nidVer.hWnd = g_hwnd;
        nidVer.uID = ID_TRAYICON;
        nidVer.uVersion = NOTIFYICON_VERSION_4;
        Shell_NotifyIconW(NIM_SETVERSION, &nidVer);

        Wh_Log(L"Tray icon added.");
        RefreshTrayIconRect();
    } else {
        Wh_Log(L"Failed to add tray icon. Error: %u", GetLastError());
    }

    return result != FALSE;
}

void UpdateTrayIcon() {
    if (!g_hwnd) {
        return;
    }

    HICON newIcon = CreateSimpleTrayIcon(g_countdownSeconds);
    if (!newIcon) {
        return;
    }

    HICON oldIcon = g_hIcon;
    g_hIcon = newIcon;

    g_nid.uFlags = NIF_ICON;
    g_nid.hIcon = g_hIcon;
    Shell_NotifyIconW(NIM_MODIFY, &g_nid);
    RefreshTrayIconRect();

    if (oldIcon) {
        DestroyIcon(oldIcon);
    }
}

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

LRESULT CALLBACK LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION && wParam == WM_MOUSEWHEEL) {
        const MSLLHOOKSTRUCT* ms =
            reinterpret_cast<const MSLLHOOKSTRUCT*>(lParam);

        short delta = static_cast<short>(HIWORD(ms->mouseData));
        int direction = (delta > 0) ? 1 : -1;

        BOOL inside = IsPointNearTrayIcon(ms->pt);

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
        Wh_Log(L"Failed to register mouse wheel hook. Error: %u", GetLastError());
        return false;
    }

    Wh_Log(L"Mouse wheel hook registered.");
    return true;
}

void UninstallMouseWheelHook() {
    if (g_mouseHook) {
        UnhookWindowsHookEx(g_mouseHook);
        g_mouseHook = nullptr;
        Wh_Log(L"Mouse wheel hook unregistered.");
    }
}

void ChangeCountdownDurationByWheel(int direction) {
    if (g_countdownActive) {
        return;
    }

    int index = GetCountdownOptionIndex(g_countdownSeconds);

    if (direction > 0) {
        index++;
        if (index >= g_countdownOptionCount) {
            index = 0;
        }
    } else {
        index--;
        if (index < 0) {
            index = g_countdownOptionCount - 1;
        }
    }

    g_countdownSeconds = g_countdownOptions[index];

    UpdateTrayIcon();
    UpdateTrayTooltip(false);

    Wh_Log(L"Countdown duration changed by mouse wheel: %d seconds",
           g_countdownSeconds);
}

void RemoveTrayIcon() {
    if (g_hwnd) {
        Shell_NotifyIconW(NIM_DELETE, &g_nid);
    }

    if (g_hIcon) {
        DestroyIcon(g_hIcon);
        g_hIcon = nullptr;
    }
}

void DestroyCountdownWindow() {
    if (g_countdownHwnd) {
        DestroyWindow(g_countdownHwnd);
        g_countdownHwnd = nullptr;
    }
}

LRESULT CALLBACK BlackScreenWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_SETCURSOR:
            SetCursor(nullptr);
            return TRUE;

        case WM_MOUSEMOVE:
            SetCursor(nullptr);
            return 0;

        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);

            RECT rect;
            GetClientRect(hwnd, &rect);

            HBRUSH brush = CreateSolidBrush(RGB(0, 0, 0));
            FillRect(hdc, &rect, brush);
            DeleteObject(brush);

            EndPaint(hwnd, &ps);
            return 0;
        }

        case WM_KEYDOWN:
            if (wParam == VK_ESCAPE) {
                PostMessageW(g_hwnd, WM_HOTKEY, ID_HOTKEY_CLOSE_BLACK_SCREEN, 0);
                return 0;
            }
            break;

        case WM_LBUTTONDOWN:
        case WM_RBUTTONDOWN:
        case WM_MBUTTONDOWN:
            PostMessageW(g_hwnd, WM_HOTKEY, ID_HOTKEY_CLOSE_BLACK_SCREEN, 0);
            return 0;

        case WM_ERASEBKGND:
            return 1;
    }

    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

bool CreateBlackScreenWindow() {
    if (g_blackScreenHwnd) {
        return true;
    }

    HINSTANCE hInstance = GetModuleHandleW(nullptr);

    const wchar_t CLASS_NAME[] = L"MonitorSleepBlackScreenWindow";

    WNDCLASSW wc = {};
    wc.lpfnWndProc = BlackScreenWindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hCursor = nullptr;

    RegisterClassW(&wc);

    int x = GetSystemMetrics(SM_XVIRTUALSCREEN);
    int y = GetSystemMetrics(SM_YVIRTUALSCREEN);
    int width = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    int height = GetSystemMetrics(SM_CYVIRTUALSCREEN);

    g_blackScreenHwnd = CreateWindowExW(
        WS_EX_TOPMOST | WS_EX_TOOLWINDOW,
        CLASS_NAME,
        L"Monitor Sleep Black Screen",
        WS_POPUP,
        x,
        y,
        width,
        height,
        nullptr,
        nullptr,
        hInstance,
        nullptr
    );

    if (!g_blackScreenHwnd) {
        Wh_Log(L"Failed to create black screen window. Error: %u", GetLastError());
        return false;
    }

    ShowWindow(g_blackScreenHwnd, SW_SHOW);
    SetForegroundWindow(g_blackScreenHwnd);
    SetFocus(g_blackScreenHwnd);
    UpdateWindow(g_blackScreenHwnd);

    g_blackScreenActive = true;

    RegisterHotKey(g_hwnd, ID_HOTKEY_CLOSE_BLACK_SCREEN, 0, VK_ESCAPE);

    Wh_Log(L"Black screen shown.");
    return true;
}

void DestroyBlackScreenWindow() {
    if (g_blackScreenHwnd) {
        UnregisterHotKey(g_hwnd, ID_HOTKEY_CLOSE_BLACK_SCREEN);
        DestroyWindow(g_blackScreenHwnd);
        g_blackScreenHwnd = nullptr;
    }

    g_blackScreenActive = false;
    Wh_Log(L"Black screen closed.");
}

LRESULT CALLBACK CountdownWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);

            RECT rect;
            GetClientRect(hwnd, &rect);

            HBRUSH hBgBrush = CreateSolidBrush(COUNTDOWN_TRANSPARENT_COLOR);
            FillRect(hdc, &rect, hBgBrush);
            DeleteObject(hBgBrush);

            SetBkMode(hdc, TRANSPARENT);

            HFONT hFont = CreateFontW(
                260,
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

            HFONT oldFont = (HFONT)SelectObject(hdc, hFont);

            wchar_t text[16];
            wsprintfW(text, L"%d", g_countdownValue);

            // Schatten
            RECT shadowRect = rect;
            OffsetRect(&shadowRect, 6, 6);
            SetTextColor(hdc, RGB(0, 0, 0));
            DrawTextW(
                hdc,
                text,
                -1,
                &shadowRect,
                DT_CENTER | DT_VCENTER | DT_SINGLELINE
            );

            // Haupttext
            SetTextColor(hdc, RGB(255, 255, 255));
            DrawTextW(
                hdc,
                text,
                -1,
                &rect,
                DT_CENTER | DT_VCENTER | DT_SINGLELINE
            );

            SelectObject(hdc, oldFont);
            DeleteObject(hFont);

            EndPaint(hwnd, &ps);
            return 0;
        }

        case WM_ERASEBKGND:
            return 1;
    }

    return DefWindowProcW(hwnd, msg, wParam, lParam);
}


bool CreateCountdownWindow() {
    HINSTANCE hInstance = GetModuleHandleW(nullptr);

    const wchar_t CLASS_NAME[] = L"MonitorSleepCountdownWindow";

    WNDCLASSW wc = {};
    wc.lpfnWndProc = CountdownWindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);

    RegisterClassW(&wc);

    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);

    g_countdownHwnd = CreateWindowExW(
        WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW,
        CLASS_NAME,
        L"Monitor Sleep Countdown",
        WS_POPUP,
        0,
        0,
        screenWidth,
        screenHeight,
        nullptr,
        nullptr,
        hInstance,
        nullptr
    );

    if (!g_countdownHwnd) {
        Wh_Log(L"Failed to create countdown window. Error: %u", GetLastError());
        return false;
    }

    // Gesamtes Overlay leicht transparent
    SetLayeredWindowAttributes(
    g_countdownHwnd,
    COUNTDOWN_TRANSPARENT_COLOR,
    0,
    LWA_COLORKEY
    );

    ShowWindow(g_countdownHwnd, SW_SHOW);
    UpdateWindow(g_countdownHwnd);

    return true;
}


void StartCountdown(HWND hwnd) {
    if (g_blackScreenActive) {
        DestroyBlackScreenWindow();
        return;
    }

    if (g_countdownActive) {
        return;
    }

    g_countdownActive = true;
    g_countdownValue = g_countdownSeconds;

    Wh_Log(L"Starting monitor sleep countdown.");

    CreateCountdownWindow();

    UpdateTrayTooltip(true);

    SetTimer(hwnd, TIMER_COUNTDOWN, COUNTDOWN_INTERVAL_MS, nullptr);

    if (!RegisterHotKey(hwnd, ID_HOTKEY_CANCEL_COUNTDOWN, 0, VK_ESCAPE)) {
        Wh_Log(L"Failed to register ESC cancel hotkey. Error: %u", GetLastError());
    } else {
        Wh_Log(L"Registered ESC cancel hotkey.");
    }
}

void CancelCountdown(HWND hwnd) {
    if (!g_countdownActive) {
        return;
    }

    KillTimer(hwnd, TIMER_COUNTDOWN);
    UnregisterHotKey(hwnd, ID_HOTKEY_CANCEL_COUNTDOWN);

    g_countdownActive = false;

    DestroyCountdownWindow();

    UpdateTrayTooltip(false);

    Wh_Log(L"Countdown cancelled.");
}

void ShowTrayContextMenu(HWND hwnd) {
    HMENU menu = CreatePopupMenu();
    if (!menu) {
        return;
    }

    AppendMenuW(
        menu,
        MF_STRING,
        ID_MENU_TURN_OFF,
        g_actionMode == ACTION_BLACK_SCREEN
            ? L"Start countdown: Show black screen"
            : L"Start countdown: Turn monitor off"
    );

    if (g_countdownActive) {
        AppendMenuW(menu, MF_STRING, ID_MENU_CANCEL_COUNTDOWN, L"Cancel countdown\tEsc");
    } else {
        AppendMenuW(menu, MF_STRING | MF_GRAYED, ID_MENU_CANCEL_COUNTDOWN, L"Cancel countdown\tEsc");
    }

    AppendMenuW(menu, MF_SEPARATOR, 0, nullptr);

    WCHAR hotkeyMenuText[128];

    wsprintfW(
        hotkeyMenuText,
        g_hotkeyEnabled ? L"Disable %s for this session" : L"Enable %s for this session",
        g_hotkeyName
    );

    AppendMenuW(
        menu,
        MF_STRING,
        ID_MENU_TOGGLE_HOTKEY,
        hotkeyMenuText
    );

    AppendMenuW(menu, MF_SEPARATOR, 0, nullptr);
    AppendMenuW(menu, MF_STRING, ID_MENU_OPEN_WINDHAWK, L"Open Windhawk");

    POINT pt;
    GetCursorPos(&pt);

    SetForegroundWindow(hwnd);

    TrackPopupMenu(
        menu,
        TPM_RIGHTBUTTON | TPM_BOTTOMALIGN | TPM_LEFTALIGN,
        pt.x,
        pt.y,
        0,
        hwnd,
        nullptr
    );

    PostMessageW(hwnd, WM_NULL, 0, 0);

    DestroyMenu(menu);
}

void FinishCountdown(HWND hwnd) {
    KillTimer(hwnd, TIMER_COUNTDOWN);
    UnregisterHotKey(hwnd, ID_HOTKEY_CANCEL_COUNTDOWN);

    g_countdownActive = false;

    DestroyCountdownWindow();

    UpdateTrayTooltip(false);

    if (g_actionMode == ACTION_BLACK_SCREEN) {
        CreateBlackScreenWindow();
    } else {
        TurnMonitorOff();
    }
}

void WhTool_ModSettingsChanged() {
    if (g_hwnd) {
        PostMessageW(g_hwnd, WM_APP_SETTINGS_CHANGED, 0, 0);
    }
}

// ------------------------------------------------------------
// Fenster-Prozedur für Tray-Nachrichten
// ------------------------------------------------------------

LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {

    if (msg == WM_APP_EXIT) {
        Wh_Log(L"Exit message received.");

        KillTimer(hwnd, TIMER_COUNTDOWN);

        g_countdownActive = false;

        DestroyCountdownWindow();
        DestroyBlackScreenWindow();
        RemoveTrayIcon();

        UnregisterMonitorHotkey();
        UninstallMouseWheelHook();

        DestroyWindow(hwnd);
        g_hwnd = nullptr;

        PostQuitMessage(0);
        return 0;
    }

    if (msg == WM_APP_SETTINGS_CHANGED) {
        Wh_Log(L"Settings changed message received.");

        ApplyRuntimeSettings(
            ReadCountdownSecondsSetting(),
            ReadActionModeSetting(),
            ReadHotkeyEnabledSetting(),
            ReadHotkeyModifiersSetting(),
            ReadHotkeyKeySetting()
        );

        return 0;
    }

    if (msg == WM_DESTROY) {
        PostQuitMessage(0);
        return 0;
    }

    if (msg == WM_HOTKEY) {
        if (wParam == ID_HOTKEY_MONITOR_OFF) {
            StartCountdown(hwnd);
            return 0;
        }

        if (wParam == ID_HOTKEY_CANCEL_COUNTDOWN) {
            CancelCountdown(hwnd);
            return 0;
        }

        if (wParam == ID_HOTKEY_CLOSE_BLACK_SCREEN) {
            DestroyBlackScreenWindow();
            return 0;
        }

        return 0;
    }

    if (msg == WM_COMMAND) {
        switch (LOWORD(wParam)) {
            case ID_MENU_TURN_OFF:
                StartCountdown(hwnd);
                return 0;

            case ID_MENU_CANCEL_COUNTDOWN:
                CancelCountdown(hwnd);
                return 0;

            case ID_MENU_TOGGLE_HOTKEY:
                ApplyRuntimeSettings(
                    g_countdownSeconds,
                    g_actionMode,
                    !g_hotkeyEnabled,
                    g_hotkeyModifiers,
                    g_hotkeyVk
                );
                return 0;

            case ID_MENU_OPEN_WINDHAWK:
                OpenWindhawk();
                return 0;
                }

        return 0;
    }

    if (msg == WM_TRAY_SCROLL) {
        DWORD now = GetTickCount();

        if (now - g_lastScrollTime > 150) {
            g_lastScrollTime = now;

            int direction = static_cast<int>(wParam);
            ChangeCountdownDurationByWheel(direction);
        }

        return 0;
    }

    if (msg == WM_TRAYICON) {
        UINT event = LOWORD(lParam);

        switch (event) {
            case WM_MOUSEMOVE:
                RefreshTrayIconRect();
                return 0;

            case WM_LBUTTONUP:
                StartCountdown(hwnd);
                return 0;

            case WM_RBUTTONUP:
            case WM_CONTEXTMENU:
                ShowTrayContextMenu(hwnd);
                return 0;
        }

        return 0;
    }

    if (msg == WM_TIMER) {
        if (wParam == TIMER_COUNTDOWN) {
            g_countdownValue--;

            if (g_countdownValue <= 0) {
                FinishCountdown(hwnd);
                return 0;
            }

            if (g_countdownHwnd) {
                InvalidateRect(g_countdownHwnd, nullptr, TRUE);
                UpdateWindow(g_countdownHwnd);
            }

            return 0;
        }
    }

    if (g_taskbarCreatedMessage != 0 && msg == g_taskbarCreatedMessage) {
        Wh_Log(L"TaskbarCreated received. Recreating tray icon.");
        AddTrayIcon();
        return 0;
    }

    return DefWindowProcW(hwnd, msg, wParam, lParam);
}


// ------------------------------------------------------------
// Unsichtbares Fenster erstellen
// ------------------------------------------------------------

bool CreateHiddenWindow() {
    HINSTANCE hInstance = GetModuleHandleW(nullptr);

    WNDCLASSW wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = HIDDEN_WINDOW_CLASS;

    RegisterClassW(&wc);

    g_hwnd = CreateWindowExW(
        0,
        HIDDEN_WINDOW_CLASS,
        L"Monitor Sleep Button",
        WS_OVERLAPPED,
        0, 0, 0, 0,
        nullptr,
        nullptr,
        hInstance,
        nullptr
    );

    if (!g_hwnd) {
        Wh_Log(L"Failed to create hidden window. Error: %u", GetLastError());
        return false;
    }

    g_taskbarCreatedMessage = RegisterWindowMessageW(L"TaskbarCreated");

    RegisterMonitorHotkey();

    return true;
}


// ------------------------------------------------------------
// Tool-Mod Logik
// ------------------------------------------------------------

DWORD WINAPI UiThreadProc(LPVOID) {
    Wh_Log(L"UI thread started.");

    if (!CreateHiddenWindow()) {
        Wh_Log(L"Failed to create hidden window in UI thread.");
        return 1;
    }

    AddTrayIcon();
    RefreshTrayIconRect();
    InstallMouseWheelHook();

    MSG msg;
    while (GetMessageW(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    Wh_Log(L"UI thread message loop ended.");
    return 0;
}

BOOL WhTool_ModInit() {
    Wh_Log(L"Initializing Monitor Sleep Button...");

    GetModuleFileNameW(nullptr, g_windhawkPath, ARRAYSIZE(g_windhawkPath));

    LoadSettings();

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

    Wh_Log(L"Monitor Sleep Button initialized.");
    return TRUE;
}


void WhTool_ModUninit() {
    Wh_Log(L"Uninitializing Monitor Sleep Button...");

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

    Wh_Log(L"Monitor Sleep Button uninitialized.");
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
