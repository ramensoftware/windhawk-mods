// ==WindhawkMod==
// @id              taskbar-autohide-trigger-zone
// @name            Taskbar Auto-Hide Trigger Zone
// @description     Reveal and hold an auto-hidden taskbar from a wider configurable edge zone
// @version         1.0.0
// @author          bushbellbest
// @github          https://github.com/bushbellbest
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -lversion
// @license         GPL-3.0
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Taskbar Auto-Hide Trigger Zone

Expands the screen-edge zone that reveals an auto-hidden Windows 11 taskbar.
When the cursor enters the configured band near the taskbar edge, the mod:

1. requests that taskbar to unhide once;
2. keeps that taskbar visible while the cursor remains inside the wider zone;
3. returns control to Windows' normal auto-hide behavior when the cursor leaves.

The cursor never moves, and the virtual pointer state is sent only to the
matching taskbar. The mod does not replace Explorer's taskbar timer IDs.

## Requirements

- Windows 11 with taskbar auto-hide enabled.
- Windhawk 1.7.3 or newer.

This mod uses internal Explorer taskbar symbols. It is tested on Windows 11
build 26200.8655. A future Windows update can require a mod update.

## Recommended settings

- Start at **100 px**.
- Try **250 px** next.
- Use **500 px** only when you intentionally want a large reveal zone.
- Keep diagnostic logging disabled unless you are troubleshooting.

## Notes

- The setting applies independently to the taskbar on each monitor.
- The trigger band follows the taskbar edge: bottom, top, left, or right.

## Credits

The Windows 11 taskbar integration approach was informed by the Windhawk mod
`taskbar-auto-hide-keyboard-only` by m417z.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- triggerMarginPx: 100
  $name: Trigger margin, px
  $description: "Reveal and keep the auto-hidden taskbar visible while the pointer is this many pixels from its screen edge. Start with 100; then try 250 or 500."
- pollIntervalMs: 30
  $name: Cursor check interval, ms
  $description: "How often the cursor position is checked. The taskbar is notified only when the pointer enters or leaves the zone."
- logEnabled: false
  $name: Enable diagnostic logging
  $description: "Enable only for troubleshooting, then turn it off again."
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <windows.h>
#include <shellapi.h>

#include <atomic>
#include <cwchar>

struct Settings {
    int triggerMarginPx;
    int pollIntervalMs;
    bool logEnabled;
};

enum class TaskbarKind {
    Primary,
    Secondary,
};

struct TaskbarEntry {
    HWND hwnd;
    TaskbarKind kind;
    void* wndProcThis;
    void* unhideThis;
    void* viewCoordinator;
};

static constexpr int kMaxTaskbars = 16;
static constexpr UINT_PTR kPassThrough = 0x7FFFFFFF;

static const UINT g_captureThisMsg = RegisterWindowMessage(
    L"Windhawk.TaskbarAutoHideTriggerZone.v9.CaptureThis");
static const UINT g_zoneEnterMsg = RegisterWindowMessage(
    L"Windhawk.TaskbarAutoHideTriggerZone.v9.ZoneEnter");
static const UINT g_zoneLeaveMsg = RegisterWindowMessage(
    L"Windhawk.TaskbarAutoHideTriggerZone.v9.ZoneLeave");

static Settings g_settings{};
static SRWLOCK g_settingsLock = SRWLOCK_INIT;
static SRWLOCK g_taskbarsLock = SRWLOCK_INIT;
static TaskbarEntry g_taskbars[kMaxTaskbars]{};
static int g_taskbarCount = 0;

// Exactly one physical pointer can be inside one monitor's taskbar trigger
// zone at a time. nullptr means the physical pointer is outside every zone.
static std::atomic<HWND> g_virtualPointerTaskbar{nullptr};

static HANDLE g_stopEvent = nullptr;
static HANDLE g_workerThread = nullptr;

static void* TrayUI_vftable_ITrayComponentHost = nullptr;

using TrayUI_Unhide_t = void(WINAPI*)(void* pThis,
                                      int trayUnhideFlags,
                                      int unhideRequest);
static TrayUI_Unhide_t TrayUI_Unhide_Original = nullptr;

using CSecondaryTray__Unhide_t = void(WINAPI*)(void* pThis,
                                               int trayUnhideFlags,
                                               int unhideRequest);
static CSecondaryTray__Unhide_t CSecondaryTray__Unhide_Original = nullptr;

// These are the native collapse paths used by taskbar.dll. We only block the
// matching path while the cursor remains in that taskbar's expanded trigger
// zone. This is intentionally narrower than a global always-show mode.
using TrayUI__Hide_t = void(WINAPI*)(void* pThis);
static TrayUI__Hide_t TrayUI__Hide_Original = nullptr;

using CSecondaryTray__AutoHide_t = void(WINAPI*)(void* pThis, bool param1);
static CSecondaryTray__AutoHide_t CSecondaryTray__AutoHide_Original = nullptr;

using TrayUI_WndProc_t = LRESULT(WINAPI*)(void* pThis,
                                          HWND hWnd,
                                          UINT Msg,
                                          WPARAM wParam,
                                          LPARAM lParam,
                                          bool* flag);
static TrayUI_WndProc_t TrayUI_WndProc_Original = nullptr;

using CSecondaryTray_v_WndProc_t = LRESULT(
    WINAPI*)(void* pThis, HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
static CSecondaryTray_v_WndProc_t CSecondaryTray_v_WndProc_Original = nullptr;

using ViewCoordinator_HandleIsPointerOverTaskbarFrameChanged_t =
    void(WINAPI*)(void* pThis,
                  HWND hMMTaskbarWnd,
                  bool isPointerOver,
                  int inputDeviceKind);
static ViewCoordinator_HandleIsPointerOverTaskbarFrameChanged_t
    ViewCoordinator_HandleIsPointerOverTaskbarFrameChanged_Original = nullptr;

using ViewCoordinator_ShouldTaskbarBeExpanded_t =
    bool(WINAPI*)(void* pThis, HWND hMMTaskbarWnd, bool expanded);
static ViewCoordinator_ShouldTaskbarBeExpanded_t
    ViewCoordinator_ShouldTaskbarBeExpanded_Original = nullptr;

static int ClampInt(int value, int minValue, int maxValue) {
    if (value < minValue) {
        return minValue;
    }
    if (value > maxValue) {
        return maxValue;
    }
    return value;
}

static void Log(PCWSTR text, HWND hwnd = nullptr) {
    AcquireSRWLockShared(&g_settingsLock);
    const bool enabled = g_settings.logEnabled;
    ReleaseSRWLockShared(&g_settingsLock);

    if (enabled) {
        Wh_Log(L"%s: %p", text, hwnd);
    }
}

static Settings GetSettingsSnapshot() {
    AcquireSRWLockShared(&g_settingsLock);
    const Settings settings = g_settings;
    ReleaseSRWLockShared(&g_settingsLock);
    return settings;
}

static void LoadSettings() {
    Settings settings{};
    settings.triggerMarginPx =
        ClampInt(Wh_GetIntSetting(L"triggerMarginPx"), 1, 4096);
    settings.pollIntervalMs =
        ClampInt(Wh_GetIntSetting(L"pollIntervalMs"), 15, 500);
    settings.logEnabled = Wh_GetIntSetting(L"logEnabled") != 0;

    AcquireSRWLockExclusive(&g_settingsLock);
    g_settings = settings;
    ReleaseSRWLockExclusive(&g_settingsLock);
}

static bool IsTaskbarWindow(HWND hwnd) {
    WCHAR className[64]{};
    if (!GetClassName(hwnd, className, ARRAYSIZE(className))) {
        return false;
    }

    return _wcsicmp(className, L"Shell_TrayWnd") == 0 ||
           _wcsicmp(className, L"Shell_SecondaryTrayWnd") == 0;
}

static void* QueryViaVtableBounded(void* object, void* vtable) {
    if (!object || !vtable) {
        return nullptr;
    }

    MEMORY_BASIC_INFORMATION mbi{};
    if (!VirtualQuery(object, &mbi, sizeof(mbi))) {
        return nullptr;
    }

    BYTE* ptr = static_cast<BYTE*>(object);
    const BYTE* regionBegin = static_cast<const BYTE*>(mbi.BaseAddress);
    const BYTE* regionEnd = regionBegin + mbi.RegionSize;

    for (int i = 0; i < 256; ++i, ptr += sizeof(void*)) {
        if (ptr < regionBegin || ptr + sizeof(void*) > regionEnd) {
            break;
        }

        if (*reinterpret_cast<void**>(ptr) == vtable) {
            return ptr;
        }
    }

    return nullptr;
}

static bool GetTaskbarEntry(HWND hwnd, TaskbarEntry* entryOut) {
    bool found = false;

    AcquireSRWLockShared(&g_taskbarsLock);
    for (int i = 0; i < g_taskbarCount; ++i) {
        if (g_taskbars[i].hwnd == hwnd) {
            *entryOut = g_taskbars[i];
            found = true;
            break;
        }
    }
    ReleaseSRWLockShared(&g_taskbarsLock);

    return found;
}

static int GetTaskbarSnapshot(TaskbarEntry* entriesOut) {
    AcquireSRWLockShared(&g_taskbarsLock);
    int count = g_taskbarCount;
    if (count > kMaxTaskbars) {
        count = kMaxTaskbars;
    }
    for (int i = 0; i < count; ++i) {
        entriesOut[i] = g_taskbars[i];
    }
    ReleaseSRWLockShared(&g_taskbarsLock);
    return count;
}

static void RegisterTaskbar(HWND hwnd, TaskbarKind kind, void* wndProcThis) {
    if (!IsTaskbarWindow(hwnd) || !wndProcThis) {
        return;
    }

    void* unhideThis = wndProcThis;
    if (kind == TaskbarKind::Primary) {
        unhideThis = QueryViaVtableBounded(
            wndProcThis, TrayUI_vftable_ITrayComponentHost);
    }

    AcquireSRWLockExclusive(&g_taskbarsLock);

    for (int i = 0; i < g_taskbarCount; ++i) {
        if (g_taskbars[i].hwnd == hwnd) {
            g_taskbars[i].kind = kind;
            g_taskbars[i].wndProcThis = wndProcThis;
            if (unhideThis) {
                g_taskbars[i].unhideThis = unhideThis;
            }
            ReleaseSRWLockExclusive(&g_taskbarsLock);
            return;
        }
    }

    if (g_taskbarCount < kMaxTaskbars) {
        g_taskbars[g_taskbarCount++] = {
            hwnd,
            kind,
            wndProcThis,
            unhideThis,
            nullptr,
        };
    }

    ReleaseSRWLockExclusive(&g_taskbarsLock);
    Log(unhideThis ? L"Registered taskbar" :
                     L"Registered taskbar without Unhide context",
        hwnd);
}

static void RemoveTaskbar(HWND hwnd) {
    AcquireSRWLockExclusive(&g_taskbarsLock);
    for (int i = 0; i < g_taskbarCount; ++i) {
        if (g_taskbars[i].hwnd == hwnd) {
            g_taskbars[i] = g_taskbars[g_taskbarCount - 1];
            --g_taskbarCount;
            break;
        }
    }
    ReleaseSRWLockExclusive(&g_taskbarsLock);

    HWND active = g_virtualPointerTaskbar.load(std::memory_order_acquire);
    if (active == hwnd) {
        g_virtualPointerTaskbar.store(nullptr, std::memory_order_release);
    }
}

static void CacheViewCoordinator(HWND hwnd, void* viewCoordinator) {
    if (!hwnd || !viewCoordinator) {
        return;
    }

    bool newlyCaptured = false;

    AcquireSRWLockExclusive(&g_taskbarsLock);
    for (int i = 0; i < g_taskbarCount; ++i) {
        if (g_taskbars[i].hwnd == hwnd) {
            newlyCaptured = g_taskbars[i].viewCoordinator == nullptr;
            g_taskbars[i].viewCoordinator = viewCoordinator;
            break;
        }
    }
    ReleaseSRWLockExclusive(&g_taskbarsLock);

    if (newlyCaptured) {
        Log(L"Captured ViewCoordinator", hwnd);
    }
}

static bool IsVirtualPointerOver(HWND hwnd) {
    return g_virtualPointerTaskbar.load(std::memory_order_acquire) == hwnd;
}

static bool IsVirtualPointerOverKind(TaskbarKind kind) {
    const HWND active =
        g_virtualPointerTaskbar.load(std::memory_order_acquire);
    if (!active) {
        return false;
    }

    TaskbarEntry entry{};
    return GetTaskbarEntry(active, &entry) && entry.kind == kind;
}

static int AbsDiff(int a, int b) {
    return a >= b ? a - b : b - a;
}

static UINT GuessTaskbarEdge(HWND hwnd, const MONITORINFO& monitorInfo) {
    RECT rc{};
    if (!GetWindowRect(hwnd, &rc)) {
        return ABE_BOTTOM;
    }

    const int width = rc.right - rc.left;
    const int height = rc.bottom - rc.top;
    const int leftDistance = AbsDiff(rc.left, monitorInfo.rcMonitor.left);
    const int topDistance = AbsDiff(rc.top, monitorInfo.rcMonitor.top);
    const int rightDistance =
        AbsDiff(monitorInfo.rcMonitor.right, rc.right);
    const int bottomDistance =
        AbsDiff(monitorInfo.rcMonitor.bottom, rc.bottom);

    if (height > width) {
        return leftDistance <= rightDistance ? ABE_LEFT : ABE_RIGHT;
    }

    return topDistance <= bottomDistance ? ABE_TOP : ABE_BOTTOM;
}

static bool CursorIsInTriggerZone(HWND taskbarWnd,
                                  const POINT& cursor,
                                  int triggerMarginPx) {
    const HMONITOR taskbarMonitor =
        MonitorFromWindow(taskbarWnd, MONITOR_DEFAULTTONEAREST);
    const HMONITOR cursorMonitor =
        MonitorFromPoint(cursor, MONITOR_DEFAULTTONULL);
    if (!taskbarMonitor || taskbarMonitor != cursorMonitor) {
        return false;
    }

    MONITORINFO monitorInfo{};
    monitorInfo.cbSize = sizeof(monitorInfo);
    if (!GetMonitorInfo(taskbarMonitor, &monitorInfo)) {
        return false;
    }

    const RECT& rc = monitorInfo.rcMonitor;
    const UINT edge = GuessTaskbarEdge(taskbarWnd, monitorInfo);
    const int monitorWidth = rc.right - rc.left;
    const int monitorHeight = rc.bottom - rc.top;

    switch (edge) {
        case ABE_TOP: {
            const int margin = ClampInt(triggerMarginPx, 1, monitorHeight);
            return cursor.x >= rc.left && cursor.x < rc.right &&
                   cursor.y >= rc.top && cursor.y < rc.top + margin;
        }
        case ABE_LEFT: {
            const int margin = ClampInt(triggerMarginPx, 1, monitorWidth);
            return cursor.x >= rc.left && cursor.x < rc.left + margin &&
                   cursor.y >= rc.top && cursor.y < rc.bottom;
        }
        case ABE_RIGHT: {
            const int margin = ClampInt(triggerMarginPx, 1, monitorWidth);
            return cursor.x >= rc.right - margin && cursor.x < rc.right &&
                   cursor.y >= rc.top && cursor.y < rc.bottom;
        }
        case ABE_BOTTOM:
        default: {
            const int margin = ClampInt(triggerMarginPx, 1, monitorHeight);
            return cursor.x >= rc.left && cursor.x < rc.right &&
                   cursor.y >= rc.bottom - margin && cursor.y < rc.bottom;
        }
    }
}

static HWND FindTriggerTaskbar(const POINT& cursor, int triggerMarginPx) {
    TaskbarEntry taskbars[kMaxTaskbars]{};
    const int count = GetTaskbarSnapshot(taskbars);

    for (int i = 0; i < count; ++i) {
        if (IsWindow(taskbars[i].hwnd) &&
            CursorIsInTriggerZone(taskbars[i].hwnd, cursor,
                                  triggerMarginPx)) {
            return taskbars[i].hwnd;
        }
    }

    return nullptr;
}

// This worker never calls Explorer's taskbar internals. It performs only the
// inexpensive coordinate comparison and posts a transition message to the UI
// thread that owns the target taskbar window.
static DWORD WINAPI ZoneWorkerThread(LPVOID) {
    HWND previousTaskbar = nullptr;

    while (true) {
        const Settings settings = GetSettingsSnapshot();
        if (WaitForSingleObject(g_stopEvent, settings.pollIntervalMs) !=
            WAIT_TIMEOUT) {
            break;
        }

        POINT cursor{};
        if (!GetCursorPos(&cursor)) {
            continue;
        }

        const HWND currentTaskbar =
            FindTriggerTaskbar(cursor, settings.triggerMarginPx);
        if (currentTaskbar == previousTaskbar) {
            continue;
        }

        // Publish the logical pointer state before the UI thread receives the
        // transition. If the pointer left a taskbar, clear it first so the
        // synthetic false event is not suppressed by the hook below.
        g_virtualPointerTaskbar.store(currentTaskbar,
                                      std::memory_order_release);

        if (previousTaskbar && IsWindow(previousTaskbar)) {
            PostMessage(previousTaskbar, g_zoneLeaveMsg, 0, 0);
            Log(L"Worker posted zone leave", previousTaskbar);
        }

        if (currentTaskbar && IsWindow(currentTaskbar)) {
            PostMessage(currentTaskbar, g_zoneEnterMsg, 0, 0);
            Log(L"Worker posted zone enter", currentTaskbar);
        }

        previousTaskbar = currentTaskbar;
    }

    return 0;
}

// The main taskbar and secondary taskbars use different native collapse
// methods. Blocking them only while their own wide trigger zone is active
// prevents the immediate re-hide seen in v9.1, without touching Explorer's
// timer IDs or forcing the taskbar to remain visible outside the zone.
static void WINAPI TrayUI__Hide_Hook(void* pThis) {
    if (IsVirtualPointerOverKind(TaskbarKind::Primary)) {
        Log(L"Blocked native TrayUI::_Hide while zone is active",
            g_virtualPointerTaskbar.load(std::memory_order_acquire));
        return;
    }

    TrayUI__Hide_Original(pThis);
}

static void WINAPI CSecondaryTray__AutoHide_Hook(void* pThis, bool param1) {
    if (IsVirtualPointerOverKind(TaskbarKind::Secondary)) {
        Log(L"Blocked native CSecondaryTray::_AutoHide while zone is active",
            g_virtualPointerTaskbar.load(std::memory_order_acquire));
        return;
    }

    CSecondaryTray__AutoHide_Original(pThis, param1);
}

static void NativeUnhideFallback(const TaskbarEntry& taskbar) {
    if (!taskbar.unhideThis) {
        Log(L"Unhide fallback unavailable", taskbar.hwnd);
        return;
    }

    if (taskbar.kind == TaskbarKind::Primary) {
        if (TrayUI_Unhide_Original) {
            TrayUI_Unhide_Original(taskbar.unhideThis, 0, 0);
            Log(L"Called native TrayUI::Unhide fallback", taskbar.hwnd);
        }
    } else if (CSecondaryTray__Unhide_Original) {
        CSecondaryTray__Unhide_Original(taskbar.unhideThis, 0, 0);
        Log(L"Called native CSecondaryTray::_Unhide fallback", taskbar.hwnd);
    }
}

// Called only from the taskbar's own UI thread through the WndProc hook.
//
// Important: on the current Windows 11 taskbar, the ViewCoordinator
// pointer-over state alone does not necessarily begin the reveal animation.
// The native TrayUI/CSecondaryTray Unhide call is still the actual reveal
// request. The official Windhawk taskbar auto-hide mod performs both steps:
// native Unhide first, then the ViewCoordinator pointer-over transition.
static void SetTaskbarVirtualPointerState(HWND hwnd, bool pointerOver) {
    TaskbarEntry taskbar{};
    if (!GetTaskbarEntry(hwnd, &taskbar)) {
        return;
    }

    if (!pointerOver && IsVirtualPointerOver(hwnd)) {
        // A stale leave message arrived after the worker re-entered the same
        // zone. Do not cancel the still-active virtual pointer state.
        Log(L"Ignored stale zone leave", hwnd);
        return;
    }

    if (pointerOver) {
        // This must happen on every outside -> inside transition, even after
        // ViewCoordinator was already captured. It starts the taskbar's own
        // normal reveal path; it is not a repeating call while the cursor
        // remains in the wide trigger zone.
        NativeUnhideFallback(taskbar);

        // NativeUnhide can synchronously populate viewCoordinator through the
        // ShouldTaskbarBeExpanded hook. Refresh our snapshot before sending the
        // virtual pointer-enter state.
        if (!GetTaskbarEntry(hwnd, &taskbar)) {
            return;
        }
    }

    if (ViewCoordinator_HandleIsPointerOverTaskbarFrameChanged_Original &&
        taskbar.viewCoordinator) {
        ViewCoordinator_HandleIsPointerOverTaskbarFrameChanged_Original(
            taskbar.viewCoordinator, hwnd, pointerOver, 0);
        Log(pointerOver ? L"Sent virtual pointer-over true" :
                          L"Sent virtual pointer-over false",
            hwnd);
    } else if (pointerOver) {
        Log(L"ViewCoordinator not captured after native Unhide", hwnd);
    } else {
        Log(L"Virtual leave deferred; ViewCoordinator not captured", hwnd);
    }
}

static LRESULT HandleTaskbarWndProc(HWND hwnd,
                                    TaskbarKind kind,
                                    void* pThis,
                                    UINT msg) {
    if (msg == WM_NCCREATE || msg == g_captureThisMsg) {
        RegisterTaskbar(hwnd, kind, pThis);
    }

    if (msg == g_zoneEnterMsg) {
        if (IsVirtualPointerOver(hwnd)) {
            SetTaskbarVirtualPointerState(hwnd, true);
        } else {
            Log(L"Ignored stale zone enter", hwnd);
        }
        return 0;
    }

    if (msg == g_zoneLeaveMsg) {
        SetTaskbarVirtualPointerState(hwnd, false);
        return 0;
    }

    if (msg == WM_NCDESTROY) {
        RemoveTaskbar(hwnd);
    }

    return kPassThrough;
}

static LRESULT WINAPI TrayUI_WndProc_Hook(void* pThis,
                                          HWND hWnd,
                                          UINT Msg,
                                          WPARAM wParam,
                                          LPARAM lParam,
                                          bool* flag) {
    const LRESULT result = HandleTaskbarWndProc(
        hWnd, TaskbarKind::Primary, pThis, Msg);
    if (result != kPassThrough) {
        return result;
    }

    return TrayUI_WndProc_Original(pThis, hWnd, Msg, wParam, lParam, flag);
}

static LRESULT WINAPI CSecondaryTray_v_WndProc_Hook(void* pThis,
                                                    HWND hWnd,
                                                    UINT Msg,
                                                    WPARAM wParam,
                                                    LPARAM lParam) {
    const LRESULT result = HandleTaskbarWndProc(
        hWnd, TaskbarKind::Secondary, pThis, Msg);
    if (result != kPassThrough) {
        return result;
    }

    return CSecondaryTray_v_WndProc_Original(
        pThis, hWnd, Msg, wParam, lParam);
}

static void WINAPI ViewCoordinator_HandleIsPointerOverTaskbarFrameChanged_Hook(
    void* pThis,
    HWND hMMTaskbarWnd,
    bool isPointerOver,
    int inputDeviceKind) {
    CacheViewCoordinator(hMMTaskbarWnd, pThis);

    // Keep the Windows 11 pointer-over state logically true while the cursor
    // is inside the expanded trigger zone. The separate taskbar.dll hide hooks
    // below prevent any already-scheduled legacy collapse from completing.
    if (!isPointerOver && IsVirtualPointerOver(hMMTaskbarWnd)) {
        Log(L"Suppressed native pointer-leave while zone is active",
            hMMTaskbarWnd);
        ViewCoordinator_HandleIsPointerOverTaskbarFrameChanged_Original(
            pThis, hMMTaskbarWnd, true, inputDeviceKind);
        return;
    }

    ViewCoordinator_HandleIsPointerOverTaskbarFrameChanged_Original(
        pThis, hMMTaskbarWnd, isPointerOver, inputDeviceKind);
}

static bool WINAPI ViewCoordinator_ShouldTaskbarBeExpanded_Hook(
    void* pThis,
    HWND hMMTaskbarWnd,
    bool expanded) {
    CacheViewCoordinator(hMMTaskbarWnd, pThis);

    // First-entry fallback: NativeUnhide can reach this method before the
    // coordinator is cached. Returning true here gives the taskbar the same
    // expand decision it would receive for a genuine pointer-over condition.
    if (IsVirtualPointerOver(hMMTaskbarWnd)) {
        return true;
    }

    return ViewCoordinator_ShouldTaskbarBeExpanded_Original(
        pThis, hMMTaskbarWnd, expanded);
}

static bool HookTaskbarSymbols() {
    HMODULE module = GetModuleHandle(L"taskbar.dll");
    if (!module) {
        module = LoadLibraryEx(L"taskbar.dll", nullptr,
                               LOAD_LIBRARY_SEARCH_SYSTEM32);
    }
    if (!module) {
        Wh_Log(L"Could not load taskbar.dll");
        return false;
    }

    WindhawkUtils::SYMBOL_HOOK symbolHooks[] = {
        {
            {LR"(const TrayUI::`vftable'{for `ITrayComponentHost'})"},
            &TrayUI_vftable_ITrayComponentHost,
        },
        {
            {LR"(public: virtual void __cdecl TrayUI::Unhide(enum TrayCommon::TrayUnhideFlags,enum TrayCommon::UnhideRequest))"},
            &TrayUI_Unhide_Original,
        },
        {
            {LR"(private: void __cdecl CSecondaryTray::_Unhide(enum TrayCommon::TrayUnhideFlags,enum TrayCommon::UnhideRequest))"},
            &CSecondaryTray__Unhide_Original,
            nullptr,
            true,
        },
        {
            {LR"(public: void __cdecl TrayUI::_Hide(void))"},
            &TrayUI__Hide_Original,
            TrayUI__Hide_Hook,
        },
        {
            {LR"(private: void __cdecl CSecondaryTray::_AutoHide(bool))"},
            &CSecondaryTray__AutoHide_Original,
            CSecondaryTray__AutoHide_Hook,
            true,
        },
        {
            {LR"(public: virtual __int64 __cdecl TrayUI::WndProc(struct HWND__ *,unsigned int,unsigned __int64,__int64,bool *))"},
            &TrayUI_WndProc_Original,
            TrayUI_WndProc_Hook,
        },
        {
            {LR"(private: virtual __int64 __cdecl CSecondaryTray::v_WndProc(struct HWND__ *,unsigned int,unsigned __int64,__int64))"},
            &CSecondaryTray_v_WndProc_Original,
            CSecondaryTray_v_WndProc_Hook,
            true,
        },
    };

    if (!HookSymbols(module, symbolHooks, ARRAYSIZE(symbolHooks))) {
        Wh_Log(L"HookSymbols failed for taskbar.dll");
        return false;
    }

    Wh_Log(L"Taskbar symbols hooked");
    return true;
}

static bool HookTaskbarViewSymbols() {
    HMODULE module = GetModuleHandle(L"Taskbar.View.dll");
    if (!module) {
        module = GetModuleHandle(L"ExplorerExtensions.dll");
    }
    if (!module) {
        Wh_Log(L"Taskbar view module is not loaded");
        return false;
    }

    WindhawkUtils::SYMBOL_HOOK symbolHooks[] = {
        {
            {LR"(public: void __cdecl winrt::Taskbar::implementation::ViewCoordinator::HandleIsPointerOverTaskbarFrameChanged(unsigned __int64,bool,enum winrt::WindowsUdk::UI::Shell::InputDeviceKind))"},
            &ViewCoordinator_HandleIsPointerOverTaskbarFrameChanged_Original,
            ViewCoordinator_HandleIsPointerOverTaskbarFrameChanged_Hook,
        },
        {
            {LR"(public: bool __cdecl winrt::Taskbar::implementation::ViewCoordinator::ShouldTaskbarBeExpanded(unsigned __int64,bool))"},
            &ViewCoordinator_ShouldTaskbarBeExpanded_Original,
            ViewCoordinator_ShouldTaskbarBeExpanded_Hook,
        },
    };

    if (!HookSymbols(module, symbolHooks, ARRAYSIZE(symbolHooks))) {
        Wh_Log(L"HookSymbols failed for Taskbar.View.dll");
        return false;
    }

    Wh_Log(L"Taskbar view pointer-state symbols hooked");
    return true;
}

static BOOL CALLBACK CaptureExistingTaskbarsProc(HWND hwnd, LPARAM) {
    DWORD processId = 0;
    GetWindowThreadProcessId(hwnd, &processId);
    if (processId == GetCurrentProcessId() && IsTaskbarWindow(hwnd)) {
        SendMessage(hwnd, g_captureThisMsg, 0, 0);
    }
    return TRUE;
}

static void SendLeaveToActiveTaskbar() {
    const HWND active = g_virtualPointerTaskbar.exchange(
        nullptr, std::memory_order_acq_rel);
    if (active && IsWindow(active)) {
        PostMessage(active, g_zoneLeaveMsg, 0, 0);
    }
}

BOOL Wh_ModInit() {
    LoadSettings();

    if (!HookTaskbarSymbols()) {
        return FALSE;
    }

    if (!HookTaskbarViewSymbols()) {
        return FALSE;
    }

    g_stopEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
    if (!g_stopEvent) {
        return FALSE;
    }

    g_workerThread = CreateThread(nullptr, 0, ZoneWorkerThread, nullptr, 0,
                                  nullptr);
    if (!g_workerThread) {
        CloseHandle(g_stopEvent);
        g_stopEvent = nullptr;
        return FALSE;
    }

    Wh_Log(L"Taskbar Auto-Hide Trigger Zone loaded");
    return TRUE;
}

void Wh_ModAfterInit() {
    EnumWindows(CaptureExistingTaskbarsProc, 0);
    Wh_Log(L"Captured existing taskbar windows");
}

void Wh_ModBeforeUninit() {
    SendLeaveToActiveTaskbar();

    if (g_stopEvent) {
        SetEvent(g_stopEvent);
    }

    if (g_workerThread) {
        WaitForSingleObject(g_workerThread, 2000);
        CloseHandle(g_workerThread);
        g_workerThread = nullptr;
    }
}

void Wh_ModUninit() {
    if (g_stopEvent) {
        CloseHandle(g_stopEvent);
        g_stopEvent = nullptr;
    }

    AcquireSRWLockExclusive(&g_taskbarsLock);
    g_taskbarCount = 0;
    ReleaseSRWLockExclusive(&g_taskbarsLock);
}

void Wh_ModSettingsChanged() {
    LoadSettings();
}
