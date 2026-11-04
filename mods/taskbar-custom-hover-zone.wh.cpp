// ==WindhawkMod==
// @id              taskbar-custom-hover-zone
// @name            Taskbar Custom Hover Zone
// @description     Only unhide the auto-hidden taskbar when the mouse enters a custom-defined region of the bottom edge.
// @version         1.0
// @author          RiverMountain Yoo
// @github          https://github.com/ksryou3224926-jpg
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -luser32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Taskbar Custom Hover Zone

Restricts the active mouse trigger area for Windows **Taskbar Auto-Hide** to a customizable central region of the screen edge.

### Features
* **Customizable Trigger Zone:** Set the start and end percentages of the screen width allowed to trigger the taskbar.
* **Accidental Trigger Prevention:** Avoids taskbar pop-ups near corners while using scrollbars or full-screen apps.
* **Windows 10 & 11 Support:** Compatible with both legacy Win32 taskbar timers and modern Windows 11 XAML-based `ViewCoordinator`.
* **Multi-Monitor Support:** Automatically adapts to active monitor resolution bounds.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- zoneStartPercent: 30
  $name: Zone start (% of monitor width)
- zoneEndPercent: 70
  $name: Zone end (% of monitor width)
- edgeMarginPx: 4
  $name: Edge trigger margin (pixels)
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>
#include <windows.h>

struct {
    int zoneStartPercent;
    int zoneEndPercent;
    int edgeMarginPx;
} g_settings;

// Standard Windows Taskbar Unhide Timer ID
enum {
    kTrayUITimerUnhide = 3,
};

void LoadSettings() {
    g_settings.zoneStartPercent = Wh_GetIntSetting(L"zoneStartPercent");
    g_settings.zoneEndPercent = Wh_GetIntSetting(L"zoneEndPercent");
    g_settings.edgeMarginPx = Wh_GetIntSetting(L"edgeMarginPx");

    if (g_settings.zoneStartPercent < 0) g_settings.zoneStartPercent = 0;
    if (g_settings.zoneEndPercent > 100) g_settings.zoneEndPercent = 100;
    if (g_settings.zoneStartPercent >= g_settings.zoneEndPercent) {
        g_settings.zoneStartPercent = 30;
        g_settings.zoneEndPercent = 70;
    }
    if (g_settings.edgeMarginPx < 1) g_settings.edgeMarginPx = 1;
}

// Check if current mouse position is within custom trigger bounds
bool IsCursorInCustomZone() {
    POINT pt;
    if (!GetCursorPos(&pt)) return true;

    HMONITOR hMon = MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi{};
    mi.cbSize = sizeof(mi);
    if (!GetMonitorInfo(hMon, &mi)) return true;

    int screenLeft = mi.rcMonitor.left;
    int screenWidth = mi.rcMonitor.right - mi.rcMonitor.left;
    int screenBottom = mi.rcMonitor.bottom;

    int zoneLeft = screenLeft + (screenWidth * g_settings.zoneStartPercent) / 100;
    int zoneRight = screenLeft + (screenWidth * g_settings.zoneEndPercent) / 100;

    bool inX = (pt.x >= zoneLeft && pt.x <= zoneRight);
    bool inY = (pt.y >= screenBottom - g_settings.edgeMarginPx);

    return inX && inY;
}

// --- Windows 10 / Legacy Taskbar Control (SetTimer Hook) ---
using SetTimer_t = decltype(&SetTimer);
SetTimer_t SetTimer_Original;
UINT_PTR WINAPI SetTimer_Hook(HWND hWnd, UINT_PTR nIDEvent, UINT uElapse, TIMERPROC lpTimerFunc) {
    if (nIDEvent == kTrayUITimerUnhide) {
        if (!IsCursorInCustomZone()) {
            Wh_Log(L"[Win10] Cursor outside custom zone, blocking unhide timer");
            return 1;
        }
    }
    return SetTimer_Original(hWnd, nIDEvent, uElapse, lpTimerFunc);
}

// --- Windows 11 Modern Taskbar Control (ViewCoordinator Hook) ---
using ViewCoordinator_UpdateIsExpanded_t = void(WINAPI*)(void* pThis, HWND hMMTaskbarWnd, int reason);
ViewCoordinator_UpdateIsExpanded_t ViewCoordinator_UpdateIsExpanded_Original;

void WINAPI ViewCoordinator_UpdateIsExpanded_Hook(void* pThis, HWND hMMTaskbarWnd, int reason) {
    // 7: PointerOverChanged, 8: ScreenEdgeStrokePointerEntered
    if (reason == 7 || reason == 8) {
        if (!IsCursorInCustomZone()) {
            Wh_Log(L"[Win11] Cursor outside custom zone, blocking Taskbar expansion");
            return;
        }
    }
    ViewCoordinator_UpdateIsExpanded_Original(pThis, hMMTaskbarWnd, reason);
}

BOOL Wh_ModInit() {
    Wh_Log(L"Taskbar Custom Hover Zone: ModInit");
    LoadSettings();

    // 1. Hook Win32 SetTimer API
    WindhawkUtils::SetFunctionHook(SetTimer, SetTimer_Hook, &SetTimer_Original);

    // 2. Hook Windows 11 Taskbar.View.dll Symbols
    HMODULE taskbarViewModule = GetModuleHandle(L"Taskbar.View.dll");
    if (!taskbarViewModule) {
        taskbarViewModule = GetModuleHandle(L"ExplorerExtensions.dll");
    }

    if (taskbarViewModule) {
        // Taskbar.View.dll, ExplorerExtensions.dll
        WindhawkUtils::SYMBOL_HOOK taskbarViewDllHooks[] = {
            {
                {LR"(public: void __cdecl winrt::Taskbar::implementation::ViewCoordinator::UpdateIsExpanded(unsigned __int64,enum TaskbarTipTest::TaskbarExpandCollapseReason))"},
                &ViewCoordinator_UpdateIsExpanded_Original,
                ViewCoordinator_UpdateIsExpanded_Hook,
            }
        };

        if (WindhawkUtils::HookSymbols(taskbarViewModule, taskbarViewDllHooks, 1)) {
            Wh_Log(L"Successfully hooked Taskbar.View.dll ViewCoordinator");
        } else {
            Wh_Log(L"Failed to hook Taskbar.View.dll symbols");
        }
    }

    return TRUE;
}

void Wh_ModSettingsChanged() {
    LoadSettings();
}
