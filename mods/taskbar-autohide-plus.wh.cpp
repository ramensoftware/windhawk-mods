// ==WindhawkMod==
// @id              taskbar-autohide-plus
// @name            Taskbar Auto-Hide Plus
// @description     Auto-hide the taskbar with full control: modes, delay, hotkey, restore on unload
// @version         1.0.0
// @author          Areeexx
// @github          https://github.com/Areeexx
// @include         explorer.exe
// @compilerOptions -lshell32 -luser32
// @license         MIT
// ==/WindhawkMod==
 
// ==WindhawkModReadme==
/*
# Taskbar Auto-Hide Plus
 
Controls the built-in **"Automatically hide the taskbar"** feature
(the same one found in Settings → Personalization → Taskbar → Taskbar behaviors)
and adds flexible conditions on top of it.
 
## Modes
- **Always** - the taskbar is always hidden (it appears when the cursor touches the screen edge).
- **Only when the active window is maximized** - auto-hide turns on while the active window is maximized.
- **Only by hotkey** - does nothing until you press the toggle hotkey.
- **Off** - the mod does not hide the taskbar (the hotkey still works).
 
## Extra options
- Hide delay (ms).
- Don't hide while the cursor is over the taskbar.
- Toggle hotkey (Ctrl / Alt / Shift / Win + a key).
- Restore the original state when the mod is unloaded.
 
Note: the Windows auto-hide setting is shared by all monitors.
*/
// ==/WindhawkModReadme==
 
// ==WindhawkModSettings==
/*
- mode: always
  $name: Mode
  $description: When the taskbar should be auto-hidden
  $options:
  - always: Always
  - maximized: Only when the active window is maximized
  - hotkey: Only by hotkey
  - off: Off
- hideDelay: 0
  $name: Hide delay (ms)
  $description: How long the condition must hold before hiding (0 = immediately)
- pollInterval: 150
  $name: Check interval (ms)
  $description: Lower = more responsive, higher = less CPU usage (50-2000)
- waitForCursorLeave: true
  $name: Don't hide while the cursor is over the taskbar
- hotkeyEnabled: true
  $name: Enable toggle hotkey
- hotkeyCtrl: true
  $name: Hotkey - Ctrl
- hotkeyAlt: true
  $name: Hotkey - Alt
- hotkeyShift: false
  $name: Hotkey - Shift
- hotkeyWin: false
  $name: Hotkey - Win
- hotkeyKey: T
  $name: Hotkey - key
  $description: A single letter or digit
- restoreOnUnload: true
  $name: Restore original state on unload
*/
// ==/WindhawkModSettings==
 
#include <windows.h>
#include <shellapi.h>
#include <wchar.h>
 
enum Mode { MODE_ALWAYS, MODE_MAXIMIZED, MODE_HOTKEY, MODE_OFF };
 
struct Settings {
    Mode  mode;
    int   hideDelay;
    int   pollInterval;
    bool  waitForCursorLeave;
    bool  hotkeyEnabled;
    bool  ctrl, alt, shift, win;
    UINT  vk;
    bool  restoreOnUnload;
};
 
static Settings          g_settings;
static CRITICAL_SECTION  g_cs;
static HANDLE            g_stopEvent = nullptr;
static HANDLE            g_thread    = nullptr;
static bool              g_origAutoHide = false;
static bool              g_manualToggle = false;  // flipped by the hotkey
 
// ---------------------------------------------------------------- settings
 
static void LoadSettings() {
    Settings s{};
 
    PCWSTR m = Wh_GetStringSetting(L"mode");
    if (!wcscmp(m, L"maximized"))    s.mode = MODE_MAXIMIZED;
    else if (!wcscmp(m, L"hotkey"))  s.mode = MODE_HOTKEY;
    else if (!wcscmp(m, L"off"))     s.mode = MODE_OFF;
    else                             s.mode = MODE_ALWAYS;
    Wh_FreeStringSetting(m);
 
    s.hideDelay          = Wh_GetIntSetting(L"hideDelay");
    s.pollInterval       = Wh_GetIntSetting(L"pollInterval");
    s.waitForCursorLeave = Wh_GetIntSetting(L"waitForCursorLeave") != 0;
    s.hotkeyEnabled      = Wh_GetIntSetting(L"hotkeyEnabled") != 0;
    s.ctrl               = Wh_GetIntSetting(L"hotkeyCtrl") != 0;
    s.alt                = Wh_GetIntSetting(L"hotkeyAlt") != 0;
    s.shift              = Wh_GetIntSetting(L"hotkeyShift") != 0;
    s.win                = Wh_GetIntSetting(L"hotkeyWin") != 0;
    s.restoreOnUnload    = Wh_GetIntSetting(L"restoreOnUnload") != 0;
 
    if (s.hideDelay < 0)        s.hideDelay = 0;
    if (s.hideDelay > 60000)    s.hideDelay = 60000;
    if (s.pollInterval < 50)    s.pollInterval = 50;
    if (s.pollInterval > 2000)  s.pollInterval = 2000;
 
    PCWSTR k = Wh_GetStringSetting(L"hotkeyKey");
    WCHAR c = k[0] ? k[0] : L'T';
    if (c >= L'a' && c <= L'z') c -= 32;
    s.vk = c;  // for A-Z and 0-9 the virtual-key code equals the character
    Wh_FreeStringSetting(k);
 
    EnterCriticalSection(&g_cs);
    g_settings = s;
    LeaveCriticalSection(&g_cs);
}
 
static Settings GetSettings() {
    EnterCriticalSection(&g_cs);
    Settings s = g_settings;
    LeaveCriticalSection(&g_cs);
    return s;
}
 
// ---------------------------------------------------------- taskbar control
 
static HWND GetTaskbar() {
    return FindWindowW(L"Shell_TrayWnd", nullptr);
}
 
static bool GetAutoHide() {
    APPBARDATA abd{};
    abd.cbSize = sizeof(abd);
    abd.hWnd   = GetTaskbar();
    UINT st = (UINT)SHAppBarMessage(ABM_GETSTATE, &abd);
    return (st & ABS_AUTOHIDE) != 0;
}
 
static void SetAutoHide(bool enable) {
    APPBARDATA abd{};
    abd.cbSize = sizeof(abd);
    abd.hWnd   = GetTaskbar();
    UINT st = (UINT)SHAppBarMessage(ABM_GETSTATE, &abd);
    st = enable ? (st | ABS_AUTOHIDE) : (st & ~(UINT)ABS_AUTOHIDE);
    abd.lParam = (LPARAM)st;
    SHAppBarMessage(ABM_SETSTATE, &abd);
}
 
static bool CursorOverTaskbar() {
    APPBARDATA abd{};
    abd.cbSize = sizeof(abd);
    abd.hWnd   = GetTaskbar();
    if (!SHAppBarMessage(ABM_GETTASKBARPOS, &abd)) return false;
    RECT r = abd.rc;
    InflateRect(&r, 4, 4);
    POINT p;
    GetCursorPos(&p);
    return PtInRect(&r, p) != FALSE;
}
 
static bool ActiveWindowMaximized() {
    HWND fg = GetForegroundWindow();
    if (!fg || !IsWindowVisible(fg) || IsIconic(fg)) return false;
 
    WCHAR cls[64] = L"";
    GetClassNameW(fg, cls, ARRAYSIZE(cls));
    if (!wcscmp(cls, L"Shell_TrayWnd") || !wcscmp(cls, L"Shell_SecondaryTrayWnd") ||
        !wcscmp(cls, L"Progman")       || !wcscmp(cls, L"WorkerW"))
        return false;
 
    return IsZoomed(fg) != FALSE;
}
 
// ----------------------------------------------------------------- hotkey
 
static bool IsDown(int vk) { return (GetAsyncKeyState(vk) & 0x8000) != 0; }
 
static bool HotkeyPressed(const Settings& s) {
    if (!s.hotkeyEnabled) return false;
    if (s.ctrl  != IsDown(VK_CONTROL)) return false;
    if (s.alt   != IsDown(VK_MENU))    return false;
    if (s.shift != IsDown(VK_SHIFT))   return false;
    if (s.win   != (IsDown(VK_LWIN) || IsDown(VK_RWIN))) return false;
    return IsDown((int)s.vk);
}
 
// ------------------------------------------------------------ worker thread
 
static bool DesiredAutoHide(const Settings& s) {
    bool base = false;
    switch (s.mode) {
        case MODE_ALWAYS:    base = true; break;
        case MODE_MAXIMIZED: base = ActiveWindowMaximized(); break;
        case MODE_HOTKEY:    base = false; break;
        case MODE_OFF:       base = false; break;
    }
    return g_manualToggle ? !base : base;
}
 
static DWORD WINAPI WorkerThread(LPVOID) {
    bool      hotkeyWasDown = false;
    bool      pending       = false;
    ULONGLONG pendingSince  = GetTickCount64();
 
    for (;;) {
        Settings s = GetSettings();
        if (WaitForSingleObject(g_stopEvent, (DWORD)s.pollInterval) != WAIT_TIMEOUT)
            break;
 
        // Hotkey toggle (fires on key press, not while held)
        bool down = HotkeyPressed(s);
        if (down && !hotkeyWasDown) {
            g_manualToggle = !g_manualToggle;
            Wh_Log(L"Hotkey toggle -> %d", (int)g_manualToggle);
        }
        hotkeyWasDown = down;
 
        bool want = DesiredAutoHide(s);
        ULONGLONG now = GetTickCount64();
 
        if (want != pending) {
            pending = want;
            pendingSince = now;
        }
 
        bool current = GetAutoHide();
        if (want == current) continue;
 
        if (want) {
            // Turning auto-hide on: wait for the delay and (optionally) for the cursor to leave
            if (now - pendingSince < (ULONGLONG)s.hideDelay) continue;
            if (s.waitForCursorLeave && CursorOverTaskbar()) continue;
            SetAutoHide(true);
        } else {
            SetAutoHide(false);
        }
    }
    return 0;
}
 
// ------------------------------------------------------------------ Windhawk
 
BOOL Wh_ModInit() {
    Wh_Log(L"Init");
    InitializeCriticalSection(&g_cs);
    LoadSettings();
 
    g_origAutoHide = GetAutoHide();
    g_manualToggle = false;
 
    g_stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_thread    = CreateThread(nullptr, 0, WorkerThread, nullptr, 0, nullptr);
    return g_stopEvent && g_thread;
}
 
void Wh_ModSettingsChanged() {
    Wh_Log(L"Settings changed");
    LoadSettings();
    g_manualToggle = false;
}
 
void Wh_ModUninit() {
    Wh_Log(L"Uninit");
    if (g_stopEvent) SetEvent(g_stopEvent);
    if (g_thread) {
        WaitForSingleObject(g_thread, 5000);
        CloseHandle(g_thread);
        g_thread = nullptr;
    }
    if (g_stopEvent) {
        CloseHandle(g_stopEvent);
        g_stopEvent = nullptr;
    }
 
    if (GetSettings().restoreOnUnload) {
        SetAutoHide(g_origAutoHide);
    }
    DeleteCriticalSection(&g_cs);
}
 
