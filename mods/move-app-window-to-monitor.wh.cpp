// ==WindhawkMod==
// @id              move-app-window-to-monitor
// @name            Move Window to Monitor
// @description     Easily move windows between monitors using hotkeys or taskbar thumbnail/titlebar menu, with one-click rescue for windows lost on disconnected displays.
// @version         1.0.1
// @author          heartacker
// @github          https://github.com/heartacker
// @include         *
// @compilerOptions -luser32 -lshell32 -lgdi32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Move Window to Monitor (窗口换屏助手)

Easily move windows across multiple monitors, or rescue offscreen/lost windows when displays are disconnected.

针对多显示器切换、外接显示器断开/虚显（幽灵屏）、远程控制（向日葵等）场景打造的窗口跨屏调度与一键救援工具。

---

### Features / 核心功能

1. **Taskbar Preview & Window Titlebar Right-Click Menu Integration (任务栏预览与标题栏右键菜单集成)**:
   - Right-click on a taskbar thumbnail preview or right-click any window's titlebar:
     - `🖥️ Move to Monitor 1 (Primary)` / `🖥️ 移至显示器 1 (主屏幕)`
     - `🖥️ Move to Monitor 2` / `🖥️ 移至显示器 2`
     - `➡️ Move to Next Monitor` / `➡️ 移至下一个显示器`
     - `🔄 Rescue All Windows to Primary Monitor` / `🔄 召回所有窗口到主屏幕`
   - Works across all applications (Chrome, Edge, File Explorer, VS Code, and standard Win32 / Chromium apps).

2. **Global Hotkeys (全局快捷键)**:
   - `Win + Alt + 1`: Move active window to Monitor 1 (Primary) / 移至显示器 1
   - `Win + Alt + 2`: Move active window to Monitor 2 / 移至显示器 2
   - `Win + Alt + 3`: Move active window to Monitor 3 / 移至显示器 3
   - `Win + Alt + ← / →`: Cycle window to Previous / Next monitor / 移至上一/下一个显示器
   - `Win + Alt + R`: **Rescue All Windows** back to primary monitor / 一键召回所有窗口

3. **Settings (设置项)**:
   - Configurable hotkey modifiers: `Win + Alt`, `Win + Ctrl`, or `Win + Shift`.
   - Auto-rescue offscreen windows when display configuration changes.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- hotkeyModifier: win_alt
  $name: Hotkey Modifiers / 快捷键修饰键
  $description: Modifier combination used for moving windows and rescuing (e.g. Win+Alt+1, Win+Alt+R)
  $options:
    - win_alt: Win + Alt (e.g. Win+Alt+1, Win+Alt+R)
    - win_ctrl: Win + Ctrl (e.g. Win+Ctrl+1, Win+Ctrl+R)
    - win_shift: Win + Shift (e.g. Win+Shift+1, Win+Shift+R)
- autoRescueOnDisplayChange: false
  $name: Auto rescue on display change / 显示器变化时自动拯救窗口
  $description: Automatically pull offscreen windows back to the primary display when a monitor is disconnected or resolution changes.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <shellapi.h>
#include <string>
#include <vector>
#include <algorithm>
#include <mutex>

// Localization helper
static inline bool IsChineseLanguage() {
    LANGID langId = GetUserDefaultUILanguage();
    return PRIMARYLANGID(langId) == LANG_CHINESE;
}

// Settings
struct {
    std::wstring hotkeyModifier;
    bool autoRescueOnDisplayChange;
} g_settings;

// Monitor Structure
struct MonitorInfo {
    HMONITOR hMonitor;
    RECT rcMonitor;
    RECT rcWork;
    bool isPrimary;
    int index; // 1-based index
    std::wstring name;
};

// Hotkey IDs
enum {
    HOTKEY_ID_MON1 = 2001,
    HOTKEY_ID_MON2,
    HOTKEY_ID_MON3,
    HOTKEY_ID_MON4,
    HOTKEY_ID_PREV,
    HOTKEY_ID_NEXT,
    HOTKEY_ID_RESCUE,
    HOTKEY_ID_RESCUE_HOME
};

// Menu IDs
enum {
    // Custom Context Menu IDs
    IDM_CUSTOM_MOVE_BASE = 0xE700,
    IDM_CUSTOM_MOVE_NEXT = 0xE720,
    IDM_CUSTOM_RESCUE    = 0xE721,
};

static const UINT WM_UPDATE_SETTINGS = WM_APP + 43;

static HANDLE s_hThread = nullptr;
static DWORD s_dwThreadId = 0;
static HWND s_hHelperWnd = nullptr;
static HWND s_hLastActiveAppWnd = nullptr;
static bool s_isExplorer = false;

// Forward Declarations
std::vector<MonitorInfo> GetAllMonitors();
void MoveWindowToMonitor(HWND hWnd, const MonitorInfo& targetMon);
void MoveForegroundWindowToMonitorIndex(int monitorIndex);
void MoveForegroundWindowToPrevOrNext(bool next);
void RescueAllWindowsToPrimary();
HWND GetTrackedTargetWindow();

bool CheckIsExplorer() {
    WCHAR processPath[MAX_PATH];
    if (GetModuleFileNameW(nullptr, processPath, ARRAYSIZE(processPath)) > 0) {
        PCWSTR processName = wcsrchr(processPath, L'\\');
        if (processName && _wcsicmp(processName + 1, L"explorer.exe") == 0) {
            return true;
        }
    }
    return false;
}

static BOOL CALLBACK EnumMonitorsCallback(HMONITOR hMon, HDC hdc, LPRECT lpRect, LPARAM lParam) {
    (void)hdc;
    (void)lpRect;
    auto* pList = reinterpret_cast<std::vector<MonitorInfo>*>(lParam);
    MONITORINFOEXW mi;
    mi.cbSize = sizeof(mi);
    if (GetMonitorInfoW(hMon, &mi)) {
        MonitorInfo info;
        info.hMonitor = hMon;
        info.rcMonitor = mi.rcMonitor;
        info.rcWork = mi.rcWork;
        info.isPrimary = (mi.dwFlags & MONITORINFOF_PRIMARY) != 0;
        info.index = 0;
        info.name = mi.szDevice;
        pList->push_back(info);
    }
    return TRUE;
}

// Helper to query all active monitors
std::vector<MonitorInfo> GetAllMonitors() {
    std::vector<MonitorInfo> monitors;
    EnumDisplayMonitors(nullptr, nullptr, EnumMonitorsCallback, reinterpret_cast<LPARAM>(&monitors));

    // Primary monitor is always index 1, then sort by left coordinate
    std::sort(monitors.begin(), monitors.end(), [](const MonitorInfo& a, const MonitorInfo& b) {
        if (a.isPrimary != b.isPrimary) {
            return a.isPrimary > b.isPrimary;
        }
        if (a.rcMonitor.left != b.rcMonitor.left) {
            return a.rcMonitor.left < b.rcMonitor.left;
        }
        return a.rcMonitor.top < b.rcMonitor.top;
    });

    for (size_t i = 0; i < monitors.size(); ++i) {
        monitors[i].index = static_cast<int>(i + 1);
    }
    return monitors;
}

// Check if a window is a valid standard application top-level window
bool IsValidAppWindow(HWND hWnd) {
    if (!IsWindow(hWnd) || !IsWindowVisible(hWnd)) return false;
    if (GetWindow(hWnd, GW_OWNER) != nullptr) return false;

    LONG exStyle = GetWindowLongW(hWnd, GWL_EXSTYLE);
    if (exStyle & WS_EX_TOOLWINDOW) return false;

    int len = GetWindowTextLengthW(hWnd);
    if (len == 0) return false;

    WCHAR className[128];
    if (GetClassNameW(hWnd, className, ARRAYSIZE(className)) > 0) {
        if (wcscmp(className, L"Progman") == 0 ||
            wcscmp(className, L"WorkerW") == 0 ||
            wcscmp(className, L"Shell_TrayWnd") == 0 ||
            wcscmp(className, L"Shell_SecondaryTrayWnd") == 0 ||
            wcscmp(className, L"Windows.UI.Core.CoreWindow") == 0) {
            return false;
        }
    }
    return true;
}

// Find the monitor that corresponds to a window
int GetWindowMonitorIndex(HWND hWnd, const std::vector<MonitorInfo>& monitors) {
    HMONITOR hMon = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONULL);
    if (!hMon) return 0; // Offscreen / invalid
    for (const auto& m : monitors) {
        if (m.hMonitor == hMon) {
            return m.index;
        }
    }
    return 0;
}

// Get the window we should operate on
HWND GetTrackedTargetWindow() {
    HWND hWnd = GetForegroundWindow();
    if (hWnd && IsValidAppWindow(hWnd)) {
        return hWnd;
    }
    if (s_hLastActiveAppWnd && IsValidAppWindow(s_hLastActiveAppWnd)) {
        return s_hLastActiveAppWnd;
    }
    return nullptr;
}

// Move target window to destination monitor
void MoveWindowToMonitor(HWND hWnd, const MonitorInfo& targetMon) {
    if (!IsWindow(hWnd)) return;

    HWND hTopWnd = GetAncestor(hWnd, GA_ROOT);
    if (hTopWnd && IsWindow(hTopWnd)) {
        hWnd = hTopWnd;
    }

    WINDOWPLACEMENT wp = { sizeof(wp) };
    if (!GetWindowPlacement(hWnd, &wp)) return;

    bool wasMaximized = (wp.showCmd == SW_SHOWMAXIMIZED);
    bool wasMinimized = (wp.showCmd == SW_SHOWMINIMIZED);

    // Source monitor
    HMONITOR hCurMon = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
    MONITORINFO curMi = { sizeof(curMi) };
    if (!GetMonitorInfoW(hCurMon, &curMi)) {
        curMi.rcWork = targetMon.rcWork;
    }

    RECT rcSrcWork = curMi.rcWork;
    RECT rcDstWork = targetMon.rcWork;

    RECT rcNormal = wp.rcNormalPosition;
    int w = rcNormal.right - rcNormal.left;
    int h = rcNormal.bottom - rcNormal.top;

    int dstW = rcDstWork.right - rcDstWork.left;
    int dstH = rcDstWork.bottom - rcDstWork.top;

    if (w <= 0 || w > dstW) w = (dstW * 4) / 5;
    if (h <= 0 || h > dstH) h = (dstH * 4) / 5;

    int srcW = rcSrcWork.right - rcSrcWork.left;
    int srcH = rcSrcWork.bottom - rcSrcWork.top;

    double relX = 0.5;
    double relY = 0.5;
    if (srcW > 0 && srcH > 0) {
        relX = (double)(rcNormal.left - rcSrcWork.left) / (double)srcW;
        relY = (double)(rcNormal.top - rcSrcWork.top) / (double)srcH;
    }
    if (relX < 0.0) relX = 0.0;
    if (relX > 0.8) relX = 0.8;
    if (relY < 0.0) relY = 0.0;
    if (relY > 0.8) relY = 0.8;

    int newX = rcDstWork.left + static_cast<int>(relX * (dstW - w));
    int newY = rcDstWork.top + static_cast<int>(relY * (dstH - h));

    if (newX < rcDstWork.left || newX + w > rcDstWork.right) {
        newX = rcDstWork.left + (dstW - w) / 2;
    }
    if (newY < rcDstWork.top || newY + h > rcDstWork.bottom) {
        newY = rcDstWork.top + (dstH - h) / 2;
    }

    if (wasMaximized) {
        ShowWindow(hWnd, SW_RESTORE);
        SetWindowPos(hWnd, nullptr, newX, newY, w, h, SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);
        ShowWindow(hWnd, SW_MAXIMIZE);
    } else if (wasMinimized) {
        wp.rcNormalPosition = { newX, newY, newX + w, newY + h };
        wp.showCmd = SW_RESTORE;
        SetWindowPlacement(hWnd, &wp);
    } else {
        SetWindowPos(hWnd, nullptr, newX, newY, w, h, SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);
    }

    SetForegroundWindow(hWnd);
}

void MoveForegroundWindowToMonitorIndex(int monitorIndex) {
    HWND hWnd = GetTrackedTargetWindow();
    if (!hWnd) return;

    auto monitors = GetAllMonitors();
    for (const auto& mon : monitors) {
        if (mon.index == monitorIndex) {
            MoveWindowToMonitor(hWnd, mon);
            break;
        }
    }
}

void MoveForegroundWindowToPrevOrNext(bool next) {
    HWND hWnd = GetTrackedTargetWindow();
    if (!hWnd) return;

    auto monitors = GetAllMonitors();
    if (monitors.size() <= 1) return;

    int curIdx = GetWindowMonitorIndex(hWnd, monitors);
    int nextIdx = 1;
    if (curIdx == 0) {
        nextIdx = 1;
    } else {
        if (next) {
            nextIdx = (curIdx % static_cast<int>(monitors.size())) + 1;
        } else {
            nextIdx = curIdx - 1;
            if (nextIdx < 1) nextIdx = static_cast<int>(monitors.size());
        }
    }

    for (const auto& mon : monitors) {
        if (mon.index == nextIdx) {
            MoveWindowToMonitor(hWnd, mon);
            break;
        }
    }
}

static BOOL CALLBACK EnumWindowsRescueCallback(HWND hWnd, LPARAM lParam) {
    if (!IsValidAppWindow(hWnd)) return TRUE;
    auto* pList = reinterpret_cast<std::vector<HWND>*>(lParam);
    pList->push_back(hWnd);
    return TRUE;
}

// Rescue all windows: move off-screen or secondary monitor windows to primary display
void RescueAllWindowsToPrimary() {
    auto monitors = GetAllMonitors();
    if (monitors.empty()) return;

    const MonitorInfo& primary = monitors[0]; // Primary is always index 1

    std::vector<HWND> toRescue;
    EnumWindows(EnumWindowsRescueCallback, reinterpret_cast<LPARAM>(&toRescue));

    int offset = 25;
    int curX = primary.rcWork.left + 50;
    int curY = primary.rcWork.top + 50;

    for (HWND hWnd : toRescue) {
        HMONITOR hMon = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONULL);
        // If window is completely outside any monitor OR not on primary monitor
        if (!hMon || hMon != primary.hMonitor) {
            WINDOWPLACEMENT wp = { sizeof(wp) };
            GetWindowPlacement(hWnd, &wp);

            int w = wp.rcNormalPosition.right - wp.rcNormalPosition.left;
            int h = wp.rcNormalPosition.bottom - wp.rcNormalPosition.top;
            int maxW = primary.rcWork.right - primary.rcWork.left;
            int maxH = primary.rcWork.bottom - primary.rcWork.top;

            if (w <= 0 || w > maxW) w = (maxW * 3) / 4;
            if (h <= 0 || h > maxH) h = (maxH * 3) / 4;

            if (curX + w > primary.rcWork.right) curX = primary.rcWork.left + 50;
            if (curY + h > primary.rcWork.bottom) curY = primary.rcWork.top + 50;

            if (wp.showCmd == SW_SHOWMAXIMIZED) {
                ShowWindow(hWnd, SW_RESTORE);
            }

            SetWindowPos(hWnd, nullptr, curX, curY, w, h, SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);

            curX += offset;
            curY += offset;
        }
    }
}

// Register or unregister global hotkeys (only inside explorer.exe)
void RegisterHotkeys(HWND hWnd) {
    for (int id = HOTKEY_ID_MON1; id <= HOTKEY_ID_RESCUE_HOME; ++id) {
        UnregisterHotKey(hWnd, id);
    }

    UINT mod = MOD_WIN | MOD_ALT;
    if (g_settings.hotkeyModifier == L"win_ctrl") {
        mod = MOD_WIN | MOD_CONTROL;
    } else if (g_settings.hotkeyModifier == L"win_shift") {
        mod = MOD_WIN | MOD_SHIFT;
    }

    // Win + Mod + 1..4
    RegisterHotKey(hWnd, HOTKEY_ID_MON1, mod | MOD_NOREPEAT, '1');
    RegisterHotKey(hWnd, HOTKEY_ID_MON2, mod | MOD_NOREPEAT, '2');
    RegisterHotKey(hWnd, HOTKEY_ID_MON3, mod | MOD_NOREPEAT, '3');
    RegisterHotKey(hWnd, HOTKEY_ID_MON4, mod | MOD_NOREPEAT, '4');

    // Win + Mod + Arrows
    RegisterHotKey(hWnd, HOTKEY_ID_PREV, mod | MOD_NOREPEAT, VK_LEFT);
    RegisterHotKey(hWnd, HOTKEY_ID_NEXT, mod | MOD_NOREPEAT, VK_RIGHT);

    // Win + Mod + R / Home (Rescue all)
    RegisterHotKey(hWnd, HOTKEY_ID_RESCUE, mod | MOD_NOREPEAT, 'R');
    RegisterHotKey(hWnd, HOTKEY_ID_RESCUE_HOME, mod | MOD_NOREPEAT, VK_HOME);
}

// ----------------------------------------------------------------------------------
// Taskbar Thumbnail / Window System Menu Hooking via TrackPopupMenuEx & TrackPopupMenu
// ----------------------------------------------------------------------------------

using TrackPopupMenuEx_t = BOOL(WINAPI*)(HMENU, UINT, int, int, HWND, LPTPMPARAMS);
static TrackPopupMenuEx_t TrackPopupMenuEx_Original = nullptr;

using TrackPopupMenu_t = BOOL(WINAPI*)(HMENU, UINT, int, int, int, HWND, const RECT*);
static TrackPopupMenu_t TrackPopupMenu_Original = nullptr;

static BOOL ProcessTrackPopupMenu(
    HMENU hMenu,
    UINT uFlags,
    int x,
    int y,
    HWND hWnd,
    LPTPMPARAMS lptpm,
    BOOL isEx,
    int nReserved,
    const RECT* prcRect,
    BOOL* pHandled,
    BOOL* pResult
) {
    if (!hMenu) {
        *pHandled = FALSE;
        return FALSE;
    }

    // Check if this menu is a window system/thumbnail menu (contains SC_CLOSE, SC_RESTORE, or SC_MINIMIZE)
    bool isSystemOrThumbMenu = (GetMenuState(hMenu, SC_CLOSE, MF_BYCOMMAND) != (UINT)-1) ||
                               (GetMenuState(hMenu, SC_RESTORE, MF_BYCOMMAND) != (UINT)-1) ||
                               (GetMenuState(hMenu, SC_MINIMIZE, MF_BYCOMMAND) != (UINT)-1) ||
                               (GetMenuState(hMenu, SC_MAXIMIZE, MF_BYCOMMAND) != (UINT)-1);

    if (!isSystemOrThumbMenu) {
        *pHandled = FALSE;
        return FALSE;
    }

    HWND hTargetWnd = hWnd;
    if (hTargetWnd && IsWindow(hTargetWnd)) {
        HWND hRoot = GetAncestor(hTargetWnd, GA_ROOT);
        if (hRoot && IsWindow(hRoot)) {
            hTargetWnd = hRoot;
        }
    } else {
        hTargetWnd = GetForegroundWindow();
    }

    Wh_Log(L"ProcessTrackPopupMenu detected system/thumbnail menu for hWnd=%p, targetWnd=%p", hWnd, hTargetWnd);

    // If not already appended, add our menu items
    if (GetMenuState(hMenu, IDM_CUSTOM_MOVE_NEXT, MF_BYCOMMAND) == (UINT)-1) {
        AppendMenuW(hMenu, MF_SEPARATOR, 0, nullptr);

        auto monitors = GetAllMonitors();
        int curMon = hTargetWnd ? GetWindowMonitorIndex(hTargetWnd, monitors) : 0;

        bool isZh = IsChineseLanguage();
        for (const auto& mon : monitors) {
            WCHAR text[64];
            if (isZh) {
                if (mon.isPrimary) {
                    swprintf_s(text, L"🖥️ 移至显示器 %d (主屏)%s", mon.index, (mon.index == curMon ? L" [当前]" : L""));
                } else {
                    swprintf_s(text, L"🖥️ 移至显示器 %d%s", mon.index, (mon.index == curMon ? L" [当前]" : L""));
                }
            } else {
                if (mon.isPrimary) {
                    swprintf_s(text, L"🖥️ Move to Monitor %d (Primary)%s", mon.index, (mon.index == curMon ? L" [Current]" : L""));
                } else {
                    swprintf_s(text, L"🖥️ Move to Monitor %d%s", mon.index, (mon.index == curMon ? L" [Current]" : L""));
                }
            }
            AppendMenuW(hMenu, MF_STRING, IDM_CUSTOM_MOVE_BASE + mon.index, text);
        }
        AppendMenuW(hMenu, MF_STRING, IDM_CUSTOM_MOVE_NEXT, isZh ? L"➡️ 移至下一个显示器" : L"➡️ Move to Next Monitor");
        AppendMenuW(hMenu, MF_STRING, IDM_CUSTOM_RESCUE, isZh ? L"🔄 召回所有窗口到主屏幕" : L"🔄 Rescue All Windows to Primary Monitor");
    }

    // Force TPM_RETURNCMD so we can catch our own custom command IDs
    BOOL callerWantedReturnCmd = (uFlags & TPM_RETURNCMD) != 0;
    UINT invokeFlags = uFlags | TPM_RETURNCMD;

    int selectedCmd = 0;
    if (isEx) {
        selectedCmd = TrackPopupMenuEx_Original(hMenu, invokeFlags, x, y, hWnd, lptpm);
    } else {
        selectedCmd = TrackPopupMenu_Original(hMenu, invokeFlags, x, y, nReserved, hWnd, prcRect);
    }

    Wh_Log(L"ProcessTrackPopupMenu selectedCmd=0x%X", selectedCmd);

    *pHandled = TRUE;

    if (selectedCmd == 0) {
        *pResult = callerWantedReturnCmd ? 0 : TRUE;
        return TRUE;
    }

    // Check if user selected one of our custom items
    if (selectedCmd >= IDM_CUSTOM_MOVE_BASE + 1 && selectedCmd <= IDM_CUSTOM_MOVE_BASE + 10) {
        int targetMon = selectedCmd - IDM_CUSTOM_MOVE_BASE;
        auto monitors = GetAllMonitors();
        for (const auto& mon : monitors) {
            if (mon.index == targetMon) {
                if (hTargetWnd) MoveWindowToMonitor(hTargetWnd, mon);
                break;
            }
        }
        *pResult = callerWantedReturnCmd ? 0 : TRUE;
        return TRUE;
    } else if (selectedCmd == IDM_CUSTOM_MOVE_NEXT) {
        auto monitors = GetAllMonitors();
        if (!monitors.empty() && hTargetWnd) {
            int curIdx = GetWindowMonitorIndex(hTargetWnd, monitors);
            int nextIdx = (curIdx % static_cast<int>(monitors.size())) + 1;
            for (const auto& mon : monitors) {
                if (mon.index == nextIdx) {
                    MoveWindowToMonitor(hTargetWnd, mon);
                    break;
                }
            }
        }
        *pResult = callerWantedReturnCmd ? 0 : TRUE;
        return TRUE;
    } else if (selectedCmd == IDM_CUSTOM_RESCUE) {
        RescueAllWindowsToPrimary();
        *pResult = callerWantedReturnCmd ? 0 : TRUE;
        return TRUE;
    }

    // Standard Win32 system command (SC_CLOSE, SC_RESTORE, etc.)
    if (callerWantedReturnCmd) {
        *pResult = selectedCmd;
    } else {
        if (hWnd && IsWindow(hWnd)) {
            PostMessageW(hWnd, WM_SYSCOMMAND, selectedCmd, 0);
        }
        *pResult = TRUE;
    }

    return TRUE;
}

BOOL WINAPI TrackPopupMenuEx_Hook(
    HMENU hMenu,
    UINT uFlags,
    int x,
    int y,
    HWND hWnd,
    LPTPMPARAMS lptpm
) {
    BOOL handled = FALSE;
    BOOL result = FALSE;
    if (ProcessTrackPopupMenu(hMenu, uFlags, x, y, hWnd, lptpm, TRUE, 0, nullptr, &handled, &result)) {
        return result;
    }
    return TrackPopupMenuEx_Original(hMenu, uFlags, x, y, hWnd, lptpm);
}

BOOL WINAPI TrackPopupMenu_Hook(
    HMENU hMenu,
    UINT uFlags,
    int x,
    int y,
    int nReserved,
    HWND hWnd,
    const RECT* prcRect
) {
    BOOL handled = FALSE;
    BOOL result = FALSE;
    if (ProcessTrackPopupMenu(hMenu, uFlags, x, y, hWnd, nullptr, FALSE, nReserved, prcRect, &handled, &result)) {
        return result;
    }
    return TrackPopupMenu_Original(hMenu, uFlags, x, y, nReserved, hWnd, prcRect);
}

// ----------------------------------------------------------------------------------

// Background Window Procedure (Runs only in explorer.exe)
LRESULT CALLBACK HelperWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_HOTKEY: {
            int id = static_cast<int>(wParam);
            if (id >= HOTKEY_ID_MON1 && id <= HOTKEY_ID_MON4) {
                MoveForegroundWindowToMonitorIndex(id - HOTKEY_ID_MON1 + 1);
            } else if (id == HOTKEY_ID_PREV) {
                MoveForegroundWindowToPrevOrNext(false);
            } else if (id == HOTKEY_ID_NEXT) {
                MoveForegroundWindowToPrevOrNext(true);
            } else if (id == HOTKEY_ID_RESCUE || id == HOTKEY_ID_RESCUE_HOME) {
                RescueAllWindowsToPrimary();
            }
            return 0;
        }

        case WM_DISPLAYCHANGE: {
            if (g_settings.autoRescueOnDisplayChange) {
                RescueAllWindowsToPrimary();
            }
            return 0;
        }

        case WM_UPDATE_SETTINGS: {
            RegisterHotkeys(hWnd);
            return 0;
        }

        case WM_TIMER: {
            HWND hFore = GetForegroundWindow();
            if (hFore && IsValidAppWindow(hFore)) {
                s_hLastActiveAppWnd = hFore;
            }
            return 0;
        }

        case WM_DESTROY: {
            KillTimer(hWnd, 1);
            PostQuitMessage(0);
            return 0;
        }
    }
    return DefWindowProcW(hWnd, msg, wParam, lParam);
}

// Worker thread for handling background messages and hotkeys
DWORD WINAPI WorkerThreadProc(LPVOID) {
    WNDCLASSEXW wc = { sizeof(wc) };
    wc.lpfnWndProc = HelperWndProc;
    wc.hInstance = GetModuleHandle(nullptr);
    wc.lpszClassName = L"MoveWindowToMonitorHelperClass";
    RegisterClassExW(&wc);

    s_hHelperWnd = CreateWindowExW(
        0,
        wc.lpszClassName,
        L"MoveWindowToMonitorHelper",
        WS_OVERLAPPED,
        0, 0, 0, 0,
        HWND_MESSAGE,
        nullptr,
        wc.hInstance,
        nullptr
    );

    if (!s_hHelperWnd) return 1;

    RegisterHotkeys(s_hHelperWnd);

    // Timer every 300ms to poll active application window
    SetTimer(s_hHelperWnd, 1, 300, nullptr);

    MSG msg;
    while (GetMessageW(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    return 0;
}

void LoadSettings() {
    PCWSTR modifier = Wh_GetStringSetting(L"hotkeyModifier");
    g_settings.hotkeyModifier = modifier ? modifier : L"win_alt";
    Wh_FreeStringSetting(modifier);

    g_settings.autoRescueOnDisplayChange = Wh_GetIntSetting(L"autoRescueOnDisplayChange") != 0;
}

BOOL Wh_ModInit() {
    s_isExplorer = CheckIsExplorer();
    Wh_Log(L"Move Window to Monitor initializing (isExplorer=%d)", s_isExplorer);

    LoadSettings();

    // Hook TrackPopupMenuEx & TrackPopupMenu in ALL processes so Chrome/Edge/Explorer system menus are all covered!
    HMODULE hUser32 = GetModuleHandleW(L"user32.dll");
    if (hUser32) {
        void* pTrackPopupMenuEx = (void*)GetProcAddress(hUser32, "TrackPopupMenuEx");
        if (pTrackPopupMenuEx) {
            Wh_SetFunctionHook(pTrackPopupMenuEx, (void*)TrackPopupMenuEx_Hook, (void**)&TrackPopupMenuEx_Original);
        }
        void* pTrackPopupMenu = (void*)GetProcAddress(hUser32, "TrackPopupMenu");
        if (pTrackPopupMenu) {
            Wh_SetFunctionHook(pTrackPopupMenu, (void*)TrackPopupMenu_Hook, (void**)&TrackPopupMenu_Original);
        }
    }

    // Only explorer.exe hosts the global hotkeys and helper thread
    if (s_isExplorer) {
        s_hThread = CreateThread(nullptr, 0, WorkerThreadProc, nullptr, 0, &s_dwThreadId);
        if (!s_hThread) {
            Wh_Log(L"Failed to create worker thread in explorer");
        }
    }

    Wh_Log(L"Move Window to Monitor initialized successfully");
    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L"Move Window to Monitor uninitializing");

    if (s_isExplorer) {
        if (s_hHelperWnd) {
            PostMessageW(s_hHelperWnd, WM_CLOSE, 0, 0);
        }

        if (s_hThread) {
            WaitForSingleObject(s_hThread, 3000);
            CloseHandle(s_hThread);
            s_hThread = nullptr;
        }
    }
}

void Wh_ModSettingsChanged() {
    Wh_Log(L"Move Window to Monitor settings changed");

    LoadSettings();

    if (s_isExplorer && s_hHelperWnd) {
        PostMessageW(s_hHelperWnd, WM_UPDATE_SETTINGS, 0, 0);
    }
}
