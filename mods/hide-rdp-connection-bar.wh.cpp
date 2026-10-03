// ==WindhawkMod==
// @id              hide-rdp-connection-bar
// @name            Hide RDP Connection Bar
// @description     Hides the Remote Desktop connection bar in fullscreen RDP sessions on Windows 11 and replaces it with a clean disconnect button. Shows hostname, fades when idle, supports a disconnect hotkey. Multi-monitor aware.
// @version         1.1.1
// @author          StarlightDaemon
// @github          https://github.com/StarlightDaemon
// @include         mstsc.exe
// @compilerOptions -lgdi32 -lshcore
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Hide RDP Connection Bar

Hides the floating Remote Desktop connection bar in fullscreen sessions on
Windows 11, where the native options to hide the bar may not persist reliably.

Optionally shows a clean disconnect button pinned to any corner of the screen:

- Displays the remote hostname above the disconnect label, updated live
- Full border outline for visibility, or top-accent-only — your choice
- Fades to near-invisible when idle, brightens on hover
- Configurable keyboard hotkey to disconnect without touching the mouse
- Follows the RDP window if moved to a different monitor
- DPI-aware — scales correctly on 4K and HiDPI displays

## Note

If the disconnect button does not appear after enabling it, close and reopen
the Remote Desktop connection. The button is created when the session starts;
it cannot appear for a session that is already running.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- hideBar: true
  $name: Hide connection bar
  $description: Hides the native RDP connection bar. Turn off to restore it.
- showButton: false
  $name: Show disconnect button
  $description: Shows a disconnect button on the screen edge. Works with or without Hide. If it does not appear, close and reopen the Remote Desktop connection.
- buttonPosition: top-right
  $name: Button position
  $description: Which corner of the RDP monitor to place the button.
  $options:
  - top-right: Top Right
  - top-left: Top Left
  - bottom-right: Bottom Right
  - bottom-left: Bottom Left
- offsetPreset: medium
  $name: Corner offset
  $description: How far to nudge the button away from the corner. Use Custom offset to override with an exact value.
  $options:
  - none: None (0 px)
  - small: Small (16 px)
  - medium: Medium (32 px)
  - large: Large (64 px)
  - xlarge: XL (96 px)
  - xxlarge: XXL (256 px)
  - xxxlarge: XXXL (512 px)
- offsetCustom: 0
  $name: Custom offset (pixels)
  $description: Exact pixel offset. Overrides Corner offset when non-zero.
- showBorder: true
  $name: Show full border
  $description: Draws a full outline around the button. Turn off for top-accent-only style.
- showHostname: true
  $name: Show hostname on button
  $description: Displays the remote host name above the disconnect label.
- fadeWhenIdle: false
  $name: Fade when idle
  $description: Fades the button to near-invisible after a few seconds of no hover. Brightens when you move the mouse over it.
- enableHotkey: false
  $name: Enable disconnect hotkey
  $description: Keyboard shortcut to disconnect without clicking the button.
- hotkeyModifier: ctrl-alt
  $name: Hotkey modifier keys
  $description: Modifier keys held for the hotkey. Only used when hotkey is enabled.
  $options:
  - ctrl-alt: Ctrl + Alt
  - ctrl-shift: Ctrl + Shift
  - alt-shift: Alt + Shift
- hotkeyKey: d
  $name: Hotkey key
  $description: Key pressed with the modifier. Only used when hotkey is enabled.
  $options:
  - d: D
  - q: Q
  - f4: F4
  - end: End
  - pause: Pause / Break
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <shellscalingapi.h>

namespace {

// ── Constants ─────────────────────────────────────────────────────────────

constexpr int  BTN_W           = 80;
constexpr int  BTN_H           = 56;
constexpr BYTE ALPHA_FULL      = 230;
constexpr BYTE ALPHA_FADED     = 35;
constexpr UINT FADE_DELAY_MS   = 4000;
constexpr int  FADE_TIMER_ID   = 42;
constexpr int  HOTKEY_ID       = 1;
constexpr auto BTN_CLASS       = L"WH_RdpDisconnectBtn";
constexpr UINT WM_CREATE_BTN   = WM_APP + 1;

// ── Settings ──────────────────────────────────────────────────────────────

bool g_hideBar        = true;
bool g_showButton     = false;
bool g_buttonOnRight  = true;
bool g_buttonAtBottom = false;
int  g_buttonOffset   = 32;
bool g_showBorder     = true;
bool g_showHostname   = true;
bool g_fadeWhenIdle   = false;
bool g_enableHotkey   = false;
bool g_hotkeyRegistered = false;
UINT g_hotkeyMod      = MOD_CONTROL | MOD_ALT;
UINT g_hotkeyVk       = 'D';

void LoadSettings() {
    g_hideBar      = Wh_GetIntSetting(L"hideBar")      != 0;
    g_showButton   = Wh_GetIntSetting(L"showButton")   != 0;
    g_showBorder   = Wh_GetIntSetting(L"showBorder")   != 0;
    g_showHostname = Wh_GetIntSetting(L"showHostname") != 0;
    g_fadeWhenIdle = Wh_GetIntSetting(L"fadeWhenIdle") != 0;
    g_enableHotkey = Wh_GetIntSetting(L"enableHotkey") != 0;

    // Position dropdown
    PCWSTR pos = Wh_GetStringSetting(L"buttonPosition");
    g_buttonOnRight  = lstrcmpW(pos, L"top-left")    != 0
                    && lstrcmpW(pos, L"bottom-left")  != 0;
    g_buttonAtBottom = lstrcmpW(pos, L"bottom-right") == 0
                    || lstrcmpW(pos, L"bottom-left")  == 0;
    Wh_FreeStringSetting(pos);

    // Offset — custom overrides preset when non-zero
    int custom = Wh_GetIntSetting(L"offsetCustom");
    if (custom > 0) {
        g_buttonOffset = custom;
    } else {
        PCWSTR preset = Wh_GetStringSetting(L"offsetPreset");
        if      (lstrcmpW(preset, L"none")     == 0) g_buttonOffset =   0;
        else if (lstrcmpW(preset, L"small")    == 0) g_buttonOffset =  16;
        else if (lstrcmpW(preset, L"large")    == 0) g_buttonOffset =  64;
        else if (lstrcmpW(preset, L"xlarge")   == 0) g_buttonOffset =  96;
        else if (lstrcmpW(preset, L"xxlarge")  == 0) g_buttonOffset = 256;
        else if (lstrcmpW(preset, L"xxxlarge") == 0) g_buttonOffset = 512;
        else                                          g_buttonOffset =  32;
        Wh_FreeStringSetting(preset);
    }

    // Hotkey modifier
    PCWSTR mod = Wh_GetStringSetting(L"hotkeyModifier");
    if      (lstrcmpW(mod, L"ctrl-shift") == 0) g_hotkeyMod = MOD_CONTROL | MOD_SHIFT;
    else if (lstrcmpW(mod, L"alt-shift")  == 0) g_hotkeyMod = MOD_ALT     | MOD_SHIFT;
    else                                         g_hotkeyMod = MOD_CONTROL | MOD_ALT;
    Wh_FreeStringSetting(mod);

    // Hotkey key
    PCWSTR key = Wh_GetStringSetting(L"hotkeyKey");
    if      (lstrcmpW(key, L"q")     == 0) g_hotkeyVk = 'Q';
    else if (lstrcmpW(key, L"f4")    == 0) g_hotkeyVk = VK_F4;
    else if (lstrcmpW(key, L"end")   == 0) g_hotkeyVk = VK_END;
    else if (lstrcmpW(key, L"pause") == 0) g_hotkeyVk = VK_PAUSE;
    else                                   g_hotkeyVk = 'D';
    Wh_FreeStringSetting(key);
}

// ── Shared state ──────────────────────────────────────────────────────────

CRITICAL_SECTION g_cs;
HWND             g_hBBar           = nullptr;
HWND             g_hRdpFrame       = nullptr;
WNDPROC          g_origBBarWndProc = nullptr;
HMONITOR         g_hLastMonitor    = nullptr;
wchar_t          g_hostname[256]   = {};

// ── Hook originals ────────────────────────────────────────────────────────

using CreateWindowExW_t  = decltype(&CreateWindowExW);
using ShowWindow_t       = decltype(&ShowWindow);
using SetWindowPos_t     = decltype(&SetWindowPos);
using SetWindowTextW_t   = decltype(&SetWindowTextW);

CreateWindowExW_t  pOrigCreateWindowExW  = nullptr;
ShowWindow_t       pOrigShowWindow       = nullptr;
SetWindowPos_t     pOrigSetWindowPos     = nullptr;
SetWindowTextW_t   pOrigSetWindowTextW   = nullptr;

// ── Monitor helper ────────────────────────────────────────────────────────

RECT GetMonitorRect(HWND hRef) {
    HMONITOR hMon = hRef && IsWindow(hRef)
        ? MonitorFromWindow(hRef, MONITOR_DEFAULTTONEAREST)
        : MonitorFromPoint({0, 0}, MONITOR_DEFAULTTOPRIMARY);
    MONITORINFO mi = { sizeof(mi) };
    if (hMon && GetMonitorInfoW(hMon, &mi))
        return mi.rcMonitor;
    return { 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN) };
}

RECT GetRdpMonitorRect() {
    EnterCriticalSection(&g_cs);
    HWND hRef = g_hRdpFrame ? g_hRdpFrame : g_hBBar;
    LeaveCriticalSection(&g_cs);
    return GetMonitorRect(hRef);
}

HMONITOR GetRdpMonitor() {
    EnterCriticalSection(&g_cs);
    HWND hRef = g_hRdpFrame ? g_hRdpFrame : g_hBBar;
    LeaveCriticalSection(&g_cs);
    return hRef && IsWindow(hRef)
        ? MonitorFromWindow(hRef, MONITOR_DEFAULTTONEAREST)
        : MonitorFromPoint({0, 0}, MONITOR_DEFAULTTOPRIMARY);
}

// ── Hostname ──────────────────────────────────────────────────────────────

void UpdateHostname() {
    EnterCriticalSection(&g_cs);
    HWND hFrame = g_hRdpFrame;
    LeaveCriticalSection(&g_cs);

    wchar_t title[512] = {};
    if (hFrame && IsWindow(hFrame))
        GetWindowTextW(hFrame, title, 512);

    // Strip " - Remote Desktop Connection" suffix
    wchar_t* sep = wcsstr(title, L" - ");
    if (sep) *sep = L'\0';

    EnterCriticalSection(&g_cs);
    wcsncpy_s(g_hostname, title[0] ? title : L"", _TRUNCATE);
    LeaveCriticalSection(&g_cs);
    Wh_Log(L"Hostname: %s", g_hostname);
}

// ── Disconnect ────────────────────────────────────────────────────────────

void DisconnectSession(HWND hRef) {
    HWND hFrame = g_hRdpFrame;
    if (!hFrame || !IsWindow(hFrame))
        hFrame = GetAncestor(hRef, GA_ROOT);
    if (!hFrame || !IsWindow(hFrame))
        hFrame = FindWindowW(L"TscShellContainerClass", nullptr);
    Wh_Log(L"Disconnect: WM_CLOSE → %p", hFrame);
    if (hFrame)
        PostMessageW(hFrame, WM_CLOSE, 0, 0);
}

// ── BBar subclass — cleanup only ─────────────────────────────────────────

LRESULT CALLBACK BBarSubclassProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    WNDPROC origProc;
    EnterCriticalSection(&g_cs);
    origProc = g_origBBarWndProc;
    LeaveCriticalSection(&g_cs);

    if (msg == WM_DESTROY) {
        if (origProc)
            SetWindowLongPtrW(hwnd, GWLP_WNDPROC,
                reinterpret_cast<LONG_PTR>(origProc));
        EnterCriticalSection(&g_cs);
        g_hBBar           = nullptr;
        g_hRdpFrame       = nullptr;
        g_origBBarWndProc = nullptr;
        LeaveCriticalSection(&g_cs);
    }

    return origProc
        ? CallWindowProcW(origProc, hwnd, msg, wParam, lParam)
        : DefWindowProcW(hwnd, msg, wParam, lParam);
}

// ── Disconnect button window ──────────────────────────────────────────────

HWND g_hBtn = nullptr;

LRESULT CALLBACK BtnWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {

    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        RECT rc;
        GetClientRect(hwnd, &rc);

        HMONITOR hMon = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
        UINT dpiX = 96, dpiY = 96;
        if (FAILED(GetDpiForMonitor(hMon, MDT_EFFECTIVE_DPI, &dpiX, &dpiY))) {
            dpiX = 96; dpiY = 96;
        }

        auto ScaleX = [dpiX](int v) { return MulDiv(v, dpiX, 96); };
        auto ScaleY = [dpiY](int v) { return MulDiv(v, dpiY, 96); };

        // Background
        HBRUSH hbrBg = CreateSolidBrush(RGB(24, 24, 24));
        FillRect(hdc, &rc, hbrBg);
        DeleteObject(hbrBg);

        // Blue accent / border
        HBRUSH hbrAccent = CreateSolidBrush(RGB(0, 120, 212));
        RECT accent = { rc.left, rc.top, rc.right, rc.top + ScaleY(3) };
        FillRect(hdc, &accent, hbrAccent);
        if (g_showBorder) {
            RECT left   = { rc.left,           rc.top, rc.left  + ScaleX(2), rc.bottom };
            RECT right  = { rc.right - ScaleX(2), rc.top, rc.right,          rc.bottom };
            RECT bottom = { rc.left, rc.bottom - ScaleY(2), rc.right,        rc.bottom };
            FillRect(hdc, &left,   hbrAccent);
            FillRect(hdc, &right,  hbrAccent);
            FillRect(hdc, &bottom, hbrAccent);
        }
        DeleteObject(hbrAccent);

        SetBkMode(hdc, TRANSPARENT);

        bool hotkeyConflict = g_enableHotkey && !g_hotkeyRegistered;
        PCWSTR disconnectLabel = hotkeyConflict ? L"✕  Hotkey Failed" : L"✕  Disconnect";

        wchar_t hostname[256];
        EnterCriticalSection(&g_cs);
        wcsncpy_s(hostname, g_hostname, _TRUNCATE);
        LeaveCriticalSection(&g_cs);

        if (g_showHostname && hostname[0]) {
            // Hostname — small, dimmed, top half
            SetTextColor(hdc, RGB(140, 140, 140));
            HFONT hSmall = CreateFontW(
                ScaleY(11), 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Segoe UI");
            HFONT hOld = (HFONT)SelectObject(hdc, hSmall);
            RECT rHost = { rc.left + ScaleX(4), rc.top + ScaleY(5), rc.right - ScaleX(4), rc.top + ScaleY(26) };
            DrawTextW(hdc, hostname, -1, &rHost,
                DT_CENTER | DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS);
            SelectObject(hdc, hOld);
            DeleteObject(hSmall);

            // Disconnect — normal, bottom half
            SetTextColor(hdc, hotkeyConflict ? RGB(255, 100, 100) : RGB(235, 235, 235));
            HFONT hMain = CreateFontW(
                ScaleY(14), 0, 0, 0, hotkeyConflict ? FW_BOLD : FW_NORMAL, FALSE, FALSE, FALSE,
                DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Segoe UI");
            hOld = (HFONT)SelectObject(hdc, hMain);
            RECT rDisc = { rc.left, rc.top + ScaleY(26), rc.right, rc.bottom };
            DrawTextW(hdc, disconnectLabel, -1, &rDisc,
                DT_CENTER | DT_SINGLELINE | DT_VCENTER);
            SelectObject(hdc, hOld);
            DeleteObject(hMain);
        } else {
            // Disconnect only — vertically centered
            SetTextColor(hdc, hotkeyConflict ? RGB(255, 100, 100) : RGB(235, 235, 235));
            HFONT hMain = CreateFontW(
                ScaleY(14), 0, 0, 0, hotkeyConflict ? FW_BOLD : FW_NORMAL, FALSE, FALSE, FALSE,
                DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Segoe UI");
            HFONT hOld = (HFONT)SelectObject(hdc, hMain);
            RECT rDisc = { rc.left, rc.top + ScaleY(3), rc.right, rc.bottom };
            DrawTextW(hdc, disconnectLabel, -1, &rDisc,
                DT_CENTER | DT_SINGLELINE | DT_VCENTER);
            SelectObject(hdc, hOld);
            DeleteObject(hMain);
        }

        EndPaint(hwnd, &ps);
        return 0;
    }

    case WM_LBUTTONDOWN:
        DisconnectSession(g_hBBar ? g_hBBar : hwnd);
        return 0;

    case WM_HOTKEY:
        if (wParam == HOTKEY_ID)
            DisconnectSession(g_hBBar ? g_hBBar : hwnd);
        return 0;

    case WM_MOUSEMOVE:
        if (g_fadeWhenIdle) {
            KillTimer(hwnd, FADE_TIMER_ID);
            SetLayeredWindowAttributes(hwnd, 0, ALPHA_FULL, LWA_ALPHA);
            TRACKMOUSEEVENT tme = { sizeof(tme), TME_LEAVE, hwnd, 0 };
            TrackMouseEvent(&tme);
        }
        break;

    case WM_MOUSELEAVE:
        if (g_fadeWhenIdle)
            SetTimer(hwnd, FADE_TIMER_ID, FADE_DELAY_MS, nullptr);
        return 0;

    case WM_TIMER:
        if (wParam == FADE_TIMER_ID) {
            KillTimer(hwnd, FADE_TIMER_ID);
            SetLayeredWindowAttributes(hwnd, 0, ALPHA_FADED, LWA_ALPHA);
        }
        return 0;

    case WM_SETCURSOR:
        SetCursor(LoadCursorW(nullptr, IDC_HAND));
        return TRUE;

    case WM_DESTROY:
        KillTimer(hwnd, FADE_TIMER_ID);
        if (g_enableHotkey)
            UnregisterHotKey(hwnd, HOTKEY_ID);
        g_hBtn = nullptr;
        return 0;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

void CreateOrRepositionButton() {
    HMONITOR hMon = GetRdpMonitor();
    MONITORINFO mi = { sizeof(mi) };
    RECT mon = (hMon && GetMonitorInfoW(hMon, &mi)) ? mi.rcMonitor : 
               RECT{ 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN) };

    UINT dpiX = 96, dpiY = 96;
    if (hMon && FAILED(GetDpiForMonitor(hMon, MDT_EFFECTIVE_DPI, &dpiX, &dpiY))) {
        dpiX = 96; dpiY = 96;
    }

    int scaledW = MulDiv(BTN_W, dpiX, 96);
    int scaledH = MulDiv(BTN_H, dpiY, 96);
    int scaledOffset = MulDiv(g_buttonOffset, dpiX, 96);

    int btnX = g_buttonOnRight  ? (mon.right - scaledW)                : mon.left;
    int btnY = g_buttonAtBottom ? (mon.bottom - scaledH - scaledOffset): (mon.top + scaledOffset);

    Wh_Log(L"Button: x=%d y=%d w=%d h=%d (monitor %d,%d-%d,%d)",
        btnX, btnY, scaledW, scaledH,
        mon.left, mon.top, mon.right, mon.bottom);

    if (g_hBtn && IsWindow(g_hBtn)) {
        pOrigSetWindowPos(g_hBtn, HWND_TOPMOST,
            btnX, btnY, scaledW, scaledH,
            SWP_NOACTIVATE | SWP_SHOWWINDOW);
        
        HRGN hRgn = CreateRoundRectRgn(0, 0, scaledW + 1, scaledH + 1, MulDiv(8, dpiX, 96), MulDiv(8, dpiY, 96));
        SetWindowRgn(g_hBtn, hRgn, FALSE);
        return;
    }

    g_hBtn = CreateWindowExW(
        WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_LAYERED,
        BTN_CLASS, L"",
        WS_POPUP,
        btnX, btnY, scaledW, scaledH,
        nullptr, nullptr, GetModuleHandleW(nullptr), nullptr);

    if (!g_hBtn) {
        Wh_Log(L"Button CreateWindowExW FAILED GLE=%d", GetLastError());
        return;
    }

    // Rounded corners
    HRGN hRgn = CreateRoundRectRgn(0, 0, scaledW + 1, scaledH + 1, MulDiv(8, dpiX, 96), MulDiv(8, dpiY, 96));
    SetWindowRgn(g_hBtn, hRgn, FALSE);

    // Start faded if idle-fade is on, otherwise full opacity
    SetLayeredWindowAttributes(g_hBtn, 0,
        g_fadeWhenIdle ? ALPHA_FADED : ALPHA_FULL, LWA_ALPHA);

    pOrigShowWindow(g_hBtn, SW_SHOWNOACTIVATE);
    pOrigSetWindowPos(g_hBtn, HWND_TOPMOST,
        btnX, btnY, scaledW, scaledH,
        SWP_NOACTIVATE | SWP_SHOWWINDOW);

    if (g_enableHotkey) {
        if (RegisterHotKey(g_hBtn, HOTKEY_ID, g_hotkeyMod, g_hotkeyVk)) {
            Wh_Log(L"Hotkey registered mod=0x%x vk=0x%x", g_hotkeyMod, g_hotkeyVk);
            g_hotkeyRegistered = true;
        } else {
            Wh_Log(L"Hotkey registration FAILED GLE=%d", GetLastError());
            g_hotkeyRegistered = false;
        }
    }

    UpdateHostname();
    InvalidateRect(g_hBtn, nullptr, TRUE);

    Wh_Log(L"Button created HWND=%p at (%d,%d)", g_hBtn, btnX, btnY);
}

// ── Helper thread ─────────────────────────────────────────────────────────

HANDLE g_hHelperThread  = nullptr;
DWORD  g_helperThreadId = 0;

DWORD WINAPI HelperThread(LPVOID) {
    WNDCLASSEXW wc   = { sizeof(wc) };
    wc.lpfnWndProc   = BtnWndProc;
    wc.hInstance     = GetModuleHandleW(nullptr);
    wc.lpszClassName = BTN_CLASS;
    wc.hCursor       = LoadCursorW(nullptr, IDC_HAND);
    RegisterClassExW(&wc);

    EnterCriticalSection(&g_cs);
    bool bbarReady = (g_hBBar != nullptr);
    LeaveCriticalSection(&g_cs);
    if (bbarReady)
        CreateOrRepositionButton();

    MSG msg;
    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        if (msg.message == WM_CREATE_BTN) {
            CreateOrRepositionButton();
        } else {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
    }

    if (g_hBtn && IsWindow(g_hBtn))
        DestroyWindow(g_hBtn);
    UnregisterClassW(BTN_CLASS, GetModuleHandleW(nullptr));
    return 0;
}

void StartHelperThread() {
    if (g_hHelperThread) return;
    g_hHelperThread = CreateThread(
        nullptr, 0, HelperThread, nullptr, 0, &g_helperThreadId);
}

void StopHelperThread() {
    if (g_helperThreadId)
        PostThreadMessageW(g_helperThreadId, WM_QUIT, 0, 0);
    if (g_hHelperThread) {
        WaitForSingleObject(g_hHelperThread, 3000);
        CloseHandle(g_hHelperThread);
        g_hHelperThread  = nullptr;
        g_helperThreadId = 0;
    }
}

// ── Hooks ─────────────────────────────────────────────────────────────────

bool IsBBarClass(LPCWSTR lpClassName) {
    if (!lpClassName) return false;
    if (reinterpret_cast<ULONG_PTR>(lpClassName) <= 0xFFFF) return false;
    return lstrcmpW(lpClassName, L"BBarWindowClass") == 0;
}

HWND WINAPI CreateWindowExW_Hook(
    DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName,
    DWORD dwStyle, int X, int Y, int nWidth, int nHeight,
    HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam)
{
    HWND hwnd = pOrigCreateWindowExW(
        dwExStyle, lpClassName, lpWindowName,
        dwStyle, X, Y, nWidth, nHeight,
        hWndParent, hMenu, hInstance, lpParam);

    if (hwnd && (g_hideBar || g_showButton) && IsBBarClass(lpClassName)) {
        HWND hFrame = hWndParent ? GetAncestor(hWndParent, GA_ROOT) : nullptr;
        RECT mon    = GetMonitorRect(hFrame ? hFrame : hwnd);
        Wh_Log(L"BBar detected HWND=%p frame=%p monitor=%d,%d-%d,%d",
            hwnd, hFrame, mon.left, mon.top, mon.right, mon.bottom);

        EnterCriticalSection(&g_cs);
        g_hBBar     = hwnd;
        g_hRdpFrame = hFrame;
        LeaveCriticalSection(&g_cs);

        g_origBBarWndProc = reinterpret_cast<WNDPROC>(
            SetWindowLongPtrW(hwnd, GWLP_WNDPROC,
                reinterpret_cast<LONG_PTR>(BBarSubclassProc)));

        if (g_hideBar)
            pOrigShowWindow(hwnd, SW_HIDE);

        if (g_showButton && g_helperThreadId)
            PostThreadMessageW(g_helperThreadId, WM_CREATE_BTN, 0, 0);
    }

    return hwnd;
}

BOOL WINAPI ShowWindow_Hook(HWND hWnd, int nCmdShow) {
    if (g_hideBar && hWnd && hWnd == g_hBBar) {
        Wh_Log(L"ShowWindow: suppressing nCmdShow=%d on BBar", nCmdShow);
        return pOrigShowWindow(hWnd, SW_HIDE);
    }
    return pOrigShowWindow(hWnd, nCmdShow);
}

BOOL WINAPI SetWindowPos_Hook(
    HWND hWnd, HWND hWndInsertAfter,
    int X, int Y, int cx, int cy, UINT uFlags)
{
    if (g_hideBar && hWnd && hWnd == g_hBBar) {
        if (uFlags & SWP_SHOWWINDOW) {
            Wh_Log(L"SetWindowPos: stripping SWP_SHOWWINDOW from BBar");
            uFlags = (uFlags & ~SWP_SHOWWINDOW) | SWP_HIDEWINDOW;
        }
    }

    BOOL result = pOrigSetWindowPos(hWnd, hWndInsertAfter, X, Y, cx, cy, uFlags);

    if (g_showButton && hWnd && hWnd == g_hRdpFrame && !(uFlags & SWP_NOMOVE)) {
        HMONITOR hMon = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
        if (hMon && hMon != g_hLastMonitor) {
            g_hLastMonitor = hMon;
            Wh_Log(L"RDP frame changed monitor — repositioning button");
            if (g_helperThreadId)
                PostThreadMessageW(g_helperThreadId, WM_CREATE_BTN, 0, 0);
        }
    }

    return result;
}

BOOL WINAPI SetWindowTextW_Hook(HWND hWnd, LPCWSTR lpString) {
    BOOL result = pOrigSetWindowTextW(hWnd, lpString);

    EnterCriticalSection(&g_cs);
    bool isFrame = (hWnd == g_hRdpFrame);
    LeaveCriticalSection(&g_cs);

    if (isFrame && g_showButton && g_showHostname) {
        UpdateHostname();
        if (g_hBtn && IsWindow(g_hBtn))
            InvalidateRect(g_hBtn, nullptr, TRUE);
    }
    return result;
}

} // namespace

BOOL Wh_ModInit() {
    InitializeCriticalSection(&g_cs);
    LoadSettings();

    Wh_SetFunctionHook(
        reinterpret_cast<void*>(CreateWindowExW),
        reinterpret_cast<void*>(CreateWindowExW_Hook),
        reinterpret_cast<void**>(&pOrigCreateWindowExW));

    Wh_SetFunctionHook(
        reinterpret_cast<void*>(ShowWindow),
        reinterpret_cast<void*>(ShowWindow_Hook),
        reinterpret_cast<void**>(&pOrigShowWindow));

    Wh_SetFunctionHook(
        reinterpret_cast<void*>(SetWindowPos),
        reinterpret_cast<void*>(SetWindowPos_Hook),
        reinterpret_cast<void**>(&pOrigSetWindowPos));

    Wh_SetFunctionHook(
        reinterpret_cast<void*>(SetWindowTextW),
        reinterpret_cast<void*>(SetWindowTextW_Hook),
        reinterpret_cast<void**>(&pOrigSetWindowTextW));

    if (g_showButton)
        StartHelperThread();

    Wh_Log(L"Hide RDP Connection Bar v1.1.1 initialized — "
           L"hide=%d button=%d hotkey=%d fade=%d hostname=%d",
           (int)g_hideBar, (int)g_showButton,
           (int)g_enableHotkey, (int)g_fadeWhenIdle, (int)g_showHostname);
    return TRUE;
}

void Wh_ModSettingsChanged() {
    bool prevButton = g_showButton;
    LoadSettings();

    if (prevButton || g_showButton) {
        StopHelperThread();
        if (g_showButton)
            StartHelperThread();
    }

    EnterCriticalSection(&g_cs);
    HWND hBBar = g_hBBar;
    LeaveCriticalSection(&g_cs);

    if (hBBar && IsWindow(hBBar)) {
        pOrigShowWindow(hBBar, SW_HIDE);
        if (g_showButton && g_helperThreadId)
            PostThreadMessageW(g_helperThreadId, WM_CREATE_BTN, 0, 0);
    }

    Wh_Log(L"Settings reloaded — hide=%d button=%d hotkey=%d fade=%d hostname=%d",
           (int)g_hideBar, (int)g_showButton,
           (int)g_enableHotkey, (int)g_fadeWhenIdle, (int)g_showHostname);
}

void Wh_ModUninit() {
    StopHelperThread();

    EnterCriticalSection(&g_cs);
    HWND    hBBar    = g_hBBar;
    WNDPROC origProc = g_origBBarWndProc;
    LeaveCriticalSection(&g_cs);

    if (hBBar && origProc && IsWindow(hBBar))
        SetWindowLongPtrW(hBBar, GWLP_WNDPROC,
            reinterpret_cast<LONG_PTR>(origProc));

    DeleteCriticalSection(&g_cs);
}
