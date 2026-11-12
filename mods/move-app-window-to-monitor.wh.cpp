// ==WindhawkMod==
// @id              move-app-window-to-monitor
// @name            Move Window to Monitor
// @name:zh-CN      窗口换屏助手
// @description     Add monitor-moving and rescue commands to window titlebar system menus.
// @description:zh-CN 在窗口标题栏系统菜单中添加跨显示器移动和窗口救援命令。
// @version         1.0.2
// @author          heartacker
// @github          https://github.com/heartacker
// @include         *
// @compilerOptions -luser32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Move Window to Monitor

Add commands to a window's titlebar system menu to move it to a monitor or rescue windows to the primary monitor. The menu entries are rebuilt every time the system menu opens, so the monitor list and current-monitor marker stay up to date and no entries remain after the menu closes.

## Features

- Right-click a window titlebar and choose **Move to Monitor N** or **Move to Next Monitor**.
- Choose **Rescue All Windows to Primary Monitor** to bring eligible windows back to the primary display.
- Menu labels follow the Windows display language.

This mod focuses on system-menu integration. For hotkeys, see [Move and Gather Windows](https://github.com/ramensoftware/windhawk-mods/blob/main/mods/gather-windows-to-monitor.wh.cpp) or [Move Window to Monitor](https://github.com/ramensoftware/windhawk-mods/blob/main/mods/move-window-to-monitor.wh.cpp). Windows also provides `Win+Shift+Left/Right` to move the active window between monitors.

# 窗口换屏助手

在窗口标题栏的系统菜单中添加跨显示器移动和救援命令。每次打开系统菜单时都会根据当前显示器重新生成菜单项，关闭菜单后会清理这些临时项目。

## 功能

- 右键点击窗口标题栏，选择 **移至显示器 N** 或 **移至下一个显示器**。
- 选择 **召回所有窗口到主显示器**，将符合条件的窗口移回主显示器。
- 菜单文字会根据 Windows 显示语言切换。

本 mod 专注于系统菜单集成。如需快捷键，请参阅上方链接的 **Move and Gather Windows** 或 **Move Window to Monitor**。Windows 也内置了 `Win+Shift+Left/Right` 跨显示器移动活动窗口的快捷键。
*/
// ==/WindhawkModReadme==

#include <windows.h>
#include <string>
#include <vector>
#include <algorithm>
#include <cwchar>

extern "C" IMAGE_DOS_HEADER __ImageBase;

static inline bool IsChineseLanguage() {
    LANGID langId = GetUserDefaultUILanguage();
    return PRIMARYLANGID(langId) == LANG_CHINESE;
}

struct MonitorInfo {
    HMONITOR hMonitor;
    RECT rcMonitor;
    RECT rcWork;
    bool isPrimary;
    int index;
    std::wstring name;
};

enum {
    IDM_CUSTOM_MOVE_BASE = 0xE700,
    IDM_CUSTOM_MOVE_NEXT = 0xE720,
    IDM_CUSTOM_RESCUE = 0xE721,
    IDM_CUSTOM_SEPARATOR = 0xE722,
};

std::vector<MonitorInfo> GetAllMonitors();
void MoveWindowToMonitor(HWND hWnd, const MonitorInfo& targetMon);
void RescueAllWindowsToPrimary();

static BOOL CALLBACK EnumMonitorsCallback(HMONITOR hMon, HDC, LPRECT, LPARAM lParam) {
    auto* monitors = reinterpret_cast<std::vector<MonitorInfo>*>(lParam);
    MONITORINFOEXW mi = {};
    mi.cbSize = sizeof(mi);
    if (GetMonitorInfoW(hMon, &mi)) {
        MonitorInfo info = {};
        info.hMonitor = hMon;
        info.rcMonitor = mi.rcMonitor;
        info.rcWork = mi.rcWork;
        info.isPrimary = (mi.dwFlags & MONITORINFOF_PRIMARY) != 0;
        info.name = mi.szDevice;
        monitors->push_back(info);
    }
    return TRUE;
}

std::vector<MonitorInfo> GetAllMonitors() {
    std::vector<MonitorInfo> monitors;
    EnumDisplayMonitors(nullptr, nullptr, EnumMonitorsCallback,
                        reinterpret_cast<LPARAM>(&monitors));
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

bool IsValidAppWindow(HWND hWnd) {
    if (!IsWindow(hWnd) || !IsWindowVisible(hWnd) || GetWindow(hWnd, GW_OWNER)) {
        return false;
    }
    if (GetWindowLongW(hWnd, GWL_EXSTYLE) & WS_EX_TOOLWINDOW) {
        return false;
    }
    if (GetWindowTextLengthW(hWnd) == 0) {
        return false;
    }

    WCHAR className[128];
    if (GetClassNameW(hWnd, className, ARRAYSIZE(className)) > 0 &&
        (wcscmp(className, L"Progman") == 0 ||
         wcscmp(className, L"WorkerW") == 0 ||
         wcscmp(className, L"Shell_TrayWnd") == 0 ||
         wcscmp(className, L"Shell_SecondaryTrayWnd") == 0 ||
         wcscmp(className, L"Windows.UI.Core.CoreWindow") == 0)) {
        return false;
    }
    return true;
}

int GetWindowMonitorIndex(HWND hWnd, const std::vector<MonitorInfo>& monitors) {
    HMONITOR hMon = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONULL);
    for (const auto& monitor : monitors) {
        if (monitor.hMonitor == hMon) {
            return monitor.index;
        }
    }
    return 0;
}

static bool IsForeignWindow(HWND hWnd) {
    return GetWindowThreadProcessId(hWnd, nullptr) != GetCurrentThreadId();
}

static void SetWindowPositionSafe(HWND hWnd, int x, int y, int width, int height) {
    UINT flags = SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED;
    if (IsForeignWindow(hWnd)) {
        flags |= SWP_ASYNCWINDOWPOS;
    }
    SetWindowPos(hWnd, nullptr, x, y, width, height, flags);
}

void MoveWindowToMonitor(HWND hWnd, const MonitorInfo& targetMon) {
    if (!IsWindow(hWnd) || IsHungAppWindow(hWnd)) {
        return;
    }

    HWND root = GetAncestor(hWnd, GA_ROOT);
    if (root && IsWindow(root)) {
        hWnd = root;
    }
    if (IsHungAppWindow(hWnd)) {
        return;
    }

    WINDOWPLACEMENT wp = {};
    wp.length = sizeof(wp);
    if (!GetWindowPlacement(hWnd, &wp)) {
        return;
    }

    const bool foreign = IsForeignWindow(hWnd);
    const bool wasMaximized = wp.showCmd == SW_SHOWMAXIMIZED;
    const bool wasMinimized = wp.showCmd == SW_SHOWMINIMIZED ||
                              wp.showCmd == SW_MINIMIZE ||
                              wp.showCmd == SW_SHOWMINNOACTIVE;

    HMONITOR currentMonitor = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
    MONITORINFO currentInfo = {};
    currentInfo.cbSize = sizeof(currentInfo);
    if (!GetMonitorInfoW(currentMonitor, &currentInfo)) {
        currentInfo.rcWork = targetMon.rcWork;
    }

    RECT sourceRect = wp.rcNormalPosition;
    if (!wasMinimized && !wasMaximized) {
        GetWindowRect(hWnd, &sourceRect);
    }
    const RECT& sourceWork = currentInfo.rcWork;
    const RECT& targetWork = targetMon.rcWork;
    int width = sourceRect.right - sourceRect.left;
    int height = sourceRect.bottom - sourceRect.top;
    const int targetWidth = targetWork.right - targetWork.left;
    const int targetHeight = targetWork.bottom - targetWork.top;
    if (width <= 0 || width > targetWidth) width = targetWidth * 4 / 5;
    if (height <= 0 || height > targetHeight) height = targetHeight * 4 / 5;

    const int sourceWidth = sourceWork.right - sourceWork.left;
    const int sourceHeight = sourceWork.bottom - sourceWork.top;
    double relX = 0.5;
    double relY = 0.5;
    if (sourceWidth > 0 && sourceHeight > 0) {
        relX = static_cast<double>(sourceRect.left - sourceWork.left) / sourceWidth;
        relY = static_cast<double>(sourceRect.top - sourceWork.top) / sourceHeight;
    }
    relX = (std::max)(0.0, (std::min)(0.8, relX));
    relY = (std::max)(0.0, (std::min)(0.8, relY));

    int x = targetWork.left + static_cast<int>(relX * (targetWidth - width));
    int y = targetWork.top + static_cast<int>(relY * (targetHeight - height));
    if (x + width > targetWork.right) x = targetWork.left + (targetWidth - width) / 2;
    if (y + height > targetWork.bottom) y = targetWork.top + (targetHeight - height) / 2;

    if (wasMinimized && !foreign) {
        wp.rcNormalPosition = { x, y, x + width, y + height };
        SetWindowPlacement(hWnd, &wp);
        return;
    }

    if (wasMinimized) {
        // SetWindowPlacement is synchronous across processes. Restore asynchronously,
        // then move the window using SWP_ASYNCWINDOWPOS instead.
        ShowWindowAsync(hWnd, SW_RESTORE);
    } else if (wasMaximized) {
        if (foreign) ShowWindowAsync(hWnd, SW_RESTORE);
        else ShowWindow(hWnd, SW_RESTORE);
    }

    SetWindowPositionSafe(hWnd, x, y, width, height);
    if (wasMaximized) {
        if (foreign) ShowWindowAsync(hWnd, SW_MAXIMIZE);
        else ShowWindow(hWnd, SW_MAXIMIZE);
    }
}

static BOOL CALLBACK EnumWindowsRescueCallback(HWND hWnd, LPARAM lParam) {
    if (IsValidAppWindow(hWnd)) {
        reinterpret_cast<std::vector<HWND>*>(lParam)->push_back(hWnd);
    }
    return TRUE;
}

void RescueAllWindowsToPrimary() {
    const auto monitors = GetAllMonitors();
    if (monitors.empty()) return;
    const MonitorInfo& primary = monitors.front();

    std::vector<HWND> windows;
    EnumWindows(EnumWindowsRescueCallback, reinterpret_cast<LPARAM>(&windows));

    int x = primary.rcWork.left + 50;
    int y = primary.rcWork.top + 50;
    const int offset = 25;
    const int maxWidth = primary.rcWork.right - primary.rcWork.left;
    const int maxHeight = primary.rcWork.bottom - primary.rcWork.top;

    for (HWND hWnd : windows) {
        if (!IsWindow(hWnd) || IsHungAppWindow(hWnd)) continue;
        HMONITOR hMon = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONULL);
        if (hMon == primary.hMonitor) continue;

        WINDOWPLACEMENT wp = {};
        wp.length = sizeof(wp);
        if (!GetWindowPlacement(hWnd, &wp)) continue;
        int width = wp.rcNormalPosition.right - wp.rcNormalPosition.left;
        int height = wp.rcNormalPosition.bottom - wp.rcNormalPosition.top;
        if (width <= 0 || width > maxWidth) width = maxWidth * 3 / 4;
        if (height <= 0 || height > maxHeight) height = maxHeight * 3 / 4;
        if (x + width > primary.rcWork.right) x = primary.rcWork.left + 50;
        if (y + height > primary.rcWork.bottom) y = primary.rcWork.top + 50;

        const bool foreign = IsForeignWindow(hWnd);
        const bool wasMinimized = wp.showCmd == SW_SHOWMINIMIZED ||
                                  wp.showCmd == SW_MINIMIZE ||
                                  wp.showCmd == SW_SHOWMINNOACTIVE;
        if (wasMinimized && !foreign) {
            wp.rcNormalPosition = { x, y, x + width, y + height };
            SetWindowPlacement(hWnd, &wp);
        } else {
            if (wasMinimized || wp.showCmd == SW_SHOWMAXIMIZED) {
                if (foreign) ShowWindowAsync(hWnd, SW_RESTORE);
                else ShowWindow(hWnd, SW_RESTORE);
            }
            SetWindowPositionSafe(hWnd, x, y, width, height);
        }
        x += offset;
        y += offset;
    }
}

using TrackPopupMenuEx_t = BOOL(WINAPI*)(HMENU, UINT, int, int, HWND, LPTPMPARAMS);
static TrackPopupMenuEx_t TrackPopupMenuEx_Original = nullptr;
using TrackPopupMenu_t = BOOL(WINAPI*)(HMENU, UINT, int, int, int, HWND, const RECT*);
static TrackPopupMenu_t TrackPopupMenu_Original = nullptr;

static void RemoveOwnMenuItems(HMENU hMenu) {
    for (int i = 1; i <= 10; ++i) {
        DeleteMenu(hMenu, IDM_CUSTOM_MOVE_BASE + i, MF_BYCOMMAND);
    }
    DeleteMenu(hMenu, IDM_CUSTOM_MOVE_NEXT, MF_BYCOMMAND);
    DeleteMenu(hMenu, IDM_CUSTOM_RESCUE, MF_BYCOMMAND);
    DeleteMenu(hMenu, IDM_CUSTOM_SEPARATOR, MF_BYCOMMAND);
}

static bool AppendOwnMenuItems(HMENU hMenu, HWND hTargetWnd,
                               const std::vector<MonitorInfo>& monitors) {
    // Avoid colliding with an existing system-menu command.
    for (int i = 1; i <= 10; ++i) {
        if (GetMenuState(hMenu, IDM_CUSTOM_MOVE_BASE + i, MF_BYCOMMAND) != (UINT)-1) {
            return false;
        }
    }
    if (GetMenuState(hMenu, IDM_CUSTOM_MOVE_NEXT, MF_BYCOMMAND) != (UINT)-1 ||
        GetMenuState(hMenu, IDM_CUSTOM_RESCUE, MF_BYCOMMAND) != (UINT)-1 ||
        GetMenuState(hMenu, IDM_CUSTOM_SEPARATOR, MF_BYCOMMAND) != (UINT)-1) {
        return false;
    }

    if (!AppendMenuW(hMenu, MF_SEPARATOR, IDM_CUSTOM_SEPARATOR, nullptr)) return false;
    const int currentMonitor = GetWindowMonitorIndex(hTargetWnd, monitors);
    const bool chinese = IsChineseLanguage();
    for (const auto& monitor : monitors) {
        WCHAR label[128];
        if (chinese) {
            swprintf_s(label, ARRAYSIZE(label), L"🖥️ 移至显示器 %d%s%s",
                       monitor.index, monitor.isPrimary ? L" (主屏)" : L"",
                       monitor.index == currentMonitor ? L" [当前]" : L"");
        } else {
            swprintf_s(label, ARRAYSIZE(label), L"🖥️ Move to Monitor %d%s%s",
                       monitor.index, monitor.isPrimary ? L" (Primary)" : L"",
                       monitor.index == currentMonitor ? L" [Current]" : L"");
        }
        if (!AppendMenuW(hMenu, MF_STRING, IDM_CUSTOM_MOVE_BASE + monitor.index, label)) {
            RemoveOwnMenuItems(hMenu);
            return false;
        }
    }
    if (!AppendMenuW(hMenu, MF_STRING, IDM_CUSTOM_MOVE_NEXT,
                     chinese ? L"➡️ 移至下一个显示器" : L"➡️ Move to Next Monitor") ||
        !AppendMenuW(hMenu, MF_STRING, IDM_CUSTOM_RESCUE,
                     chinese ? L"🔄 召回所有窗口到主屏幕" : L"🔄 Rescue All Windows to Primary Monitor")) {
        RemoveOwnMenuItems(hMenu);
        return false;
    }
    return true;
}

static bool IsOurCommand(UINT command) {
    return (command >= IDM_CUSTOM_MOVE_BASE + 1 && command <= IDM_CUSTOM_MOVE_BASE + 10) ||
           command == IDM_CUSTOM_MOVE_NEXT || command == IDM_CUSTOM_RESCUE;
}

static BOOL ProcessTrackPopupMenu(HMENU hMenu, UINT flags, int x, int y, int nReserved,
                                  HWND hWnd, LPTPMPARAMS params, bool isEx) {
    if (!hMenu) return FALSE;

    // First do a cheap check; GetSystemMenu can create a menu copy for the window.
    const bool hasSystemCommand =
        GetMenuState(hMenu, SC_CLOSE, MF_BYCOMMAND) != (UINT)-1 ||
        GetMenuState(hMenu, SC_RESTORE, MF_BYCOMMAND) != (UINT)-1 ||
        GetMenuState(hMenu, SC_MINIMIZE, MF_BYCOMMAND) != (UINT)-1 ||
        GetMenuState(hMenu, SC_MAXIMIZE, MF_BYCOMMAND) != (UINT)-1;
    if (!hasSystemCommand || !hWnd || !IsWindow(hWnd)) return FALSE;

    HWND targetWnd = GetAncestor(hWnd, GA_ROOT);
    if (!targetWnd) targetWnd = hWnd;
    if (hMenu != GetSystemMenu(targetWnd, FALSE)) return FALSE;

    const auto monitors = GetAllMonitors();
    if (monitors.empty() || !AppendOwnMenuItems(hMenu, targetWnd, monitors)) return FALSE;

    const bool callerWantsReturn = (flags & TPM_RETURNCMD) != 0;
    const UINT invokeFlags = flags | TPM_RETURNCMD;
    UINT selected = 0;
    if (isEx) {
        selected = TrackPopupMenuEx_Original(hMenu, invokeFlags, x, y, hWnd, params);
    } else {
        selected = TrackPopupMenu_Original(hMenu, invokeFlags, x, y, nReserved, hWnd, nullptr);
    }

    // System menus belong to the window and persist after the popup closes. Always
    // remove our temporary commands, including on cancellation or normal commands.
    RemoveOwnMenuItems(hMenu);

    if (selected == 0) {
        return callerWantsReturn ? 0 : TRUE;
    }
    if (IsOurCommand(selected)) {
        if (selected >= IDM_CUSTOM_MOVE_BASE + 1 &&
            selected <= IDM_CUSTOM_MOVE_BASE + static_cast<UINT>(monitors.size())) {
            const int index = static_cast<int>(selected - IDM_CUSTOM_MOVE_BASE);
            for (const auto& monitor : monitors) {
                if (monitor.index == index) {
                    MoveWindowToMonitor(targetWnd, monitor);
                    break;
                }
            }
        } else if (selected == IDM_CUSTOM_MOVE_NEXT && monitors.size() > 1) {
            const int current = GetWindowMonitorIndex(targetWnd, monitors);
            const int next = current == 0 ? 1 : current % static_cast<int>(monitors.size()) + 1;
            for (const auto& monitor : monitors) {
                if (monitor.index == next) {
                    MoveWindowToMonitor(targetWnd, monitor);
                    break;
                }
            }
        } else if (selected == IDM_CUSTOM_RESCUE) {
            RescueAllWindowsToPrimary();
        }
        return callerWantsReturn ? 0 : TRUE;
    }

    if (callerWantsReturn) {
        return selected;
    }
    if (!(flags & TPM_NONOTIFY)) {
        const DWORD messagePos = GetMessagePos();
        PostMessageW(hWnd, WM_SYSCOMMAND, selected, messagePos);
    }
    return TRUE;
}

BOOL WINAPI TrackPopupMenuEx_Hook(HMENU hMenu, UINT flags, int x, int y,
                                  HWND hWnd, LPTPMPARAMS params) {
    if (ProcessTrackPopupMenu(hMenu, flags, x, y, 0, hWnd, params, true)) return TRUE;
    return TrackPopupMenuEx_Original(hMenu, flags, x, y, hWnd, params);
}

BOOL WINAPI TrackPopupMenu_Hook(HMENU hMenu, UINT flags, int x, int y, int nReserved,
                                HWND hWnd, const RECT* rect) {
    if (ProcessTrackPopupMenu(hMenu, flags, x, y, nReserved, hWnd, nullptr, false)) return TRUE;
    return TrackPopupMenu_Original(hMenu, flags, x, y, nReserved, hWnd, rect);
}

BOOL Wh_ModInit() {
    HMODULE user32 = GetModuleHandleW(L"user32.dll");
    if (!user32) return FALSE;
    void* trackPopupMenuEx = reinterpret_cast<void*>(GetProcAddress(user32, "TrackPopupMenuEx"));
    void* trackPopupMenu = reinterpret_cast<void*>(GetProcAddress(user32, "TrackPopupMenu"));
    if (!trackPopupMenuEx || !trackPopupMenu) {
        Wh_Log(L"Failed to locate TrackPopupMenu functions");
        return FALSE;
    }
    Wh_SetFunctionHook(trackPopupMenuEx, reinterpret_cast<void*>(TrackPopupMenuEx_Hook),
                       reinterpret_cast<void**>(&TrackPopupMenuEx_Original));
    Wh_SetFunctionHook(trackPopupMenu, reinterpret_cast<void*>(TrackPopupMenu_Hook),
                       reinterpret_cast<void**>(&TrackPopupMenu_Original));
    return TRUE;
}

void Wh_ModUninit() {
}
