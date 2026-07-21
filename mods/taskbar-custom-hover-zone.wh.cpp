// ==WindhawkMod==
// @id              taskbar-custom-hover-zone
// @name            Taskbar Custom Hover Zone
// @description     Only unhide the auto-hidden taskbar when the mouse enters a custom-defined region of the bottom edge.
// @version         1.1
// @author          RiverMountain Yoo
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -luser32
// ==/WindhawkMod==

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

// 상수 정의 (Windows 기본 작업 표시줄 Unhide 타이머 ID)
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

// 현재 마우스가 지정한 하단 중앙 영역 내부인지 검사
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

// --- Windows 10 / 구형 작업 표시줄 제어 (SetTimer 후킹) ---
using SetTimer_t = decltype(&SetTimer);
SetTimer_t SetTimer_Original;
UINT_PTR WINAPI SetTimer_Hook(HWND hWnd, UINT_PTR nIDEvent, UINT uElapse, TIMERPROC lpTimerFunc) {
    if (nIDEvent == kTrayUITimerUnhide) {
        if (!IsCursorInCustomZone()) {
            Wh_Log(L"[Win10] Cursor outside custom zone, blocking unhide timer");
            return 1; // 타이머 실행 차단
        }
    }
    return SetTimer_Original(hWnd, nIDEvent, uElapse, lpTimerFunc);
}

// --- Windows 11 신형 작업 표시줄 제어 (ViewCoordinator 후킹) ---
using ViewCoordinator_UpdateIsExpanded_t = void(WINAPI*)(void* pThis, HWND hMMTaskbarWnd, int reason);
ViewCoordinator_UpdateIsExpanded_t ViewCoordinator_UpdateIsExpanded_Original;

void WINAPI ViewCoordinator_UpdateIsExpanded_Hook(void* pThis, HWND hMMTaskbarWnd, int reason) {
    // 7: PointerOverChanged, 8: ScreenEdgeStrokePointerEntered (마우스 호버 관련 이벤트)
    if (reason == 7 || reason == 8) {
        if (!IsCursorInCustomZone()) {
            Wh_Log(L"[Win11] Cursor outside custom zone, blocking Taskbar expansion");
            return; // 작업 표시줄 확장(Unhide) 차단
        }
    }
    ViewCoordinator_UpdateIsExpanded_Original(pThis, hMMTaskbarWnd, reason);
}

BOOL Wh_ModInit() {
    Wh_Log(L"Taskbar Custom Hover Zone: ModInit");
    LoadSettings();

    // 1. Win32 SetTimer API 후킹 (Win10 및 기본 시스템 호버 차단)
    WindhawkUtils::SetFunctionHook(SetTimer, SetTimer_Hook, &SetTimer_Original);

    // 2. Windows 11 Taskbar.View.dll 심볼 후킹
    HMODULE taskbarViewModule = GetModuleHandle(L"Taskbar.View.dll");
    if (!taskbarViewModule) {
        taskbarViewModule = GetModuleHandle(L"ExplorerExtensions.dll");
    }

    if (taskbarViewModule) {
        WindhawkUtils::SYMBOL_HOOK symbolHooks[] = {
            {
                {LR"(public: void __cdecl winrt::Taskbar::implementation::ViewCoordinator::UpdateIsExpanded(unsigned __int64,enum TaskbarTipTest::TaskbarExpandCollapseReason))"},
                &ViewCoordinator_UpdateIsExpanded_Original,
                ViewCoordinator_UpdateIsExpanded_Hook,
            }
        };

        if (WindhawkUtils::HookSymbols(taskbarViewModule, symbolHooks, 1)) {
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
