// ==WindhawkMod==
// @id              top-taskbar-auto-hide
// @name            Top Taskbar Auto-Hide
// @description     Smooth auto-hide Windows 11 top taskbar with full maximized window area
// @version         1.0
// @author          Atharv Phatak
// @github          https://github.com/atharvphatak
// @include         explorer.exe
// @architecture    x86-64
// ==/WindhawkMod==

// ==WindhawkModSettings==
/*
- edgeTriggerSize: 4
  $name: Top edge trigger
  $description: Pixels from top edge that reveal taskbar

- hideDelay: 300
  $name: Hide delay
  $description: Delay before hiding taskbar in milliseconds

- showDelay: 0
  $name: Show delay
  $description: Delay before showing taskbar in milliseconds

- animationDuration: 180
  $name: Animation duration
  $description: Taskbar slide animation duration in milliseconds
*/
// ==/WindhawkModSettings==

// ==WindhawkModReadme==
/*
# Top Taskbar Auto-Hide

Smoothly auto-hides Windows 11 taskbar when taskbar is positioned at top.

## Features

- Smooth slide animation
- Mouse-to-top reveal
- Full monitor work area
- Maximized windows use full screen
- Taskbar remains clickable when visible
- Configurable animation duration
- Configurable hide/show delay
- Configurable top-edge trigger

## Requirements

- Windows 11
- Windhawk
- Taskbar positioned at top of screen
*/
// ==/WindhawkModReadme==

#include <windhawk_utils.h>

#include <windows.h>

#include <atomic>


struct Settings {
    int edgeTriggerSize;
    int hideDelay;
    int showDelay;
    int animationDuration;
};

Settings g_settings;


std::atomic<bool> g_running{false};
std::atomic<bool> g_hidden{false};
std::atomic<bool> g_animating{false};

HWND g_taskbarWindow = nullptr;
HMONITOR g_taskbarMonitor = nullptr;
HANDLE g_workerThread = nullptr;


/*
 * ============================================================
 * SETTINGS
 * ============================================================
 */

void LoadSettings() {

    g_settings.edgeTriggerSize =
        Wh_GetIntSetting(L"edgeTriggerSize");

    g_settings.hideDelay =
        Wh_GetIntSetting(L"hideDelay");

    g_settings.showDelay =
        Wh_GetIntSetting(L"showDelay");

    g_settings.animationDuration =
        Wh_GetIntSetting(L"animationDuration");


    if (g_settings.edgeTriggerSize < 1)
        g_settings.edgeTriggerSize = 1;

    if (g_settings.edgeTriggerSize > 20)
        g_settings.edgeTriggerSize = 20;


    if (g_settings.hideDelay < 0)
        g_settings.hideDelay = 0;

    if (g_settings.hideDelay > 5000)
        g_settings.hideDelay = 5000;


    if (g_settings.showDelay < 0)
        g_settings.showDelay = 0;

    if (g_settings.showDelay > 1000)
        g_settings.showDelay = 1000;


    if (g_settings.animationDuration < 50)
        g_settings.animationDuration = 50;

    if (g_settings.animationDuration > 1000)
        g_settings.animationDuration = 1000;
}


/*
 * ============================================================
 * FIND TOP TASKBAR
 * ============================================================
 */

BOOL WINAPI FindTaskbarEnumProc(
    HWND hwnd,
    LPARAM lParam
) {

    DWORD processId = 0;

    if (
        !GetWindowThreadProcessId(
            hwnd,
            &processId
        )
    ) {
        return TRUE;
    }


    if (
        processId !=
        GetCurrentProcessId()
    ) {
        return TRUE;
    }


    wchar_t className[64]{};

    if (
        !GetClassNameW(
            hwnd,
            className,
            ARRAYSIZE(className)
        )
    ) {
        return TRUE;
    }


    if (
        _wcsicmp(
            className,
            L"Shell_TrayWnd"
        ) != 0
    ) {
        return TRUE;
    }


    RECT rc{};

    if (
        !GetWindowRect(
            hwnd,
            &rc
        )
    ) {
        return TRUE;
    }


    HMONITOR monitor =
        MonitorFromWindow(
            hwnd,
            MONITOR_DEFAULTTONEAREST
        );


    MONITORINFO mi{};

    mi.cbSize =
        sizeof(mi);


    if (
        !GetMonitorInfoW(
            monitor,
            &mi
        )
    ) {
        return TRUE;
    }


    if (
        abs(
            rc.top -
            mi.rcMonitor.top
        ) > 10
    ) {
        return TRUE;
    }


    *reinterpret_cast<HWND*>(
        lParam
    ) = hwnd;


    return FALSE;
}


HWND FindTopTaskbar() {

    HWND result = nullptr;


    EnumWindows(
        FindTaskbarEnumProc,
        reinterpret_cast<LPARAM>(
            &result
        )
    );


    return result;
}


/*
 * ============================================================
 * MONITOR
 * ============================================================
 */

HMONITOR GetTaskbarMonitor() {

    if (
        !IsWindow(
            g_taskbarWindow
        )
    ) {
        return nullptr;
    }


    return MonitorFromWindow(
        g_taskbarWindow,
        MONITOR_DEFAULTTONEAREST
    );
}


bool GetMonitorRect(
    HMONITOR monitor,
    RECT* rect
) {

    if (!monitor)
        return false;


    MONITORINFO mi{};

    mi.cbSize =
        sizeof(mi);


    if (
        !GetMonitorInfoW(
            monitor,
            &mi
        )
    ) {
        return false;
    }


    *rect =
        mi.rcMonitor;


    return true;
}


/*
 * ============================================================
 * CURSOR
 * ============================================================
 */

bool CursorAtTopEdge() {

    if (!g_taskbarMonitor)
        return false;


    POINT pt{};

    if (
        !GetCursorPos(
            &pt
        )
    ) {
        return false;
    }


    RECT monitor{};

    if (
        !GetMonitorRect(
            g_taskbarMonitor,
            &monitor
        )
    ) {
        return false;
    }


    return
        pt.y >=
            monitor.top &&

        pt.y <=
            monitor.top +
            g_settings.edgeTriggerSize;
}


bool CursorOverTaskbar() {

    if (
        !IsWindow(
            g_taskbarWindow
        )
    ) {
        return false;
    }


    POINT pt{};

    if (
        !GetCursorPos(
            &pt
        )
    ) {
        return false;
    }


    RECT rc{};

    if (
        !GetWindowRect(
            g_taskbarWindow,
            &rc
        )
    ) {
        return false;
    }


    return PtInRect(
        &rc,
        pt
    );
}


/*
 * ============================================================
 * EASING
 * ============================================================
 */

double EaseInOut(
    double t
) {

    if (t < 0.0)
        t = 0.0;

    if (t > 1.0)
        t = 1.0;


    return
        t * t *
        (3.0 - 2.0 * t);
}


/*
 * ============================================================
 * ANIMATION
 * ============================================================
 */

void AnimateTaskbar(
    int startY,
    int endY,
    int duration
) {

    if (
        !IsWindow(
            g_taskbarWindow
        )
    ) {
        return;
    }


    g_animating.store(
        true,
        std::memory_order_relaxed
    );


    ULONGLONG startTime =
        GetTickCount64();


    while (
        g_running.load(
            std::memory_order_relaxed
        )
    ) {

        ULONGLONG now =
            GetTickCount64();


        double t =
            static_cast<double>(
                now - startTime
            ) /
            static_cast<double>(
                duration
            );


        if (t >= 1.0)
            t = 1.0;


        double eased =
            EaseInOut(t);


        int currentY =
            startY +
            static_cast<int>(
                (endY - startY) *
                eased
            );


        RECT rc{};

        if (
            !GetWindowRect(
                g_taskbarWindow,
                &rc
            )
        ) {
            break;
        }


        SetWindowPos(
            g_taskbarWindow,
            HWND_TOP,
            rc.left,
            currentY,
            0,
            0,
            SWP_NOSIZE |
            SWP_NOACTIVATE |
            SWP_SHOWWINDOW
        );


        if (t >= 1.0)
            break;


        Sleep(8);
    }


    RECT rc{};

    if (
        GetWindowRect(
            g_taskbarWindow,
            &rc
        )
    ) {

        SetWindowPos(
            g_taskbarWindow,
            HWND_TOP,
            rc.left,
            endY,
            0,
            0,
            SWP_NOSIZE |
            SWP_NOACTIVATE |
            SWP_SHOWWINDOW
        );
    }


    g_animating.store(
        false,
        std::memory_order_relaxed
    );
}


/*
 * ============================================================
 * CTray::RecomputeWorkArea
 * ============================================================
 */

using CTray_RecomputeWorkArea_t =
    int(WINAPI*)(
        void* pThis,
        HMONITOR monitor,
        RECT* rect
    );

CTray_RecomputeWorkArea_t
    CTray_RecomputeWorkArea_Original =
        nullptr;


int WINAPI CTray_RecomputeWorkArea_Hook(
    void* pThis,
    HMONITOR monitor,
    RECT* rect
) {

    int result =
        CTray_RecomputeWorkArea_Original(
            pThis,
            monitor,
            rect
        );


    if (
        monitor ==
            g_taskbarMonitor &&
        rect != nullptr
    ) {

        RECT monitorRect{};

        if (
            GetMonitorRect(
                monitor,
                &monitorRect
            )
        ) {

            *rect =
                monitorRect;
        }
    }


    return result;
}


/*
 * ============================================================
 * CTray::AppBarSetAutoHideBar
 * ============================================================
 */

using CTray_AppBarSetAutoHideBar_t =
    BOOL(WINAPI*)(
        void* pThis,
        HWND hwnd,
        BOOL autoHide,
        UINT edge,
        HMONITOR monitor
    );

CTray_AppBarSetAutoHideBar_t
    CTray_AppBarSetAutoHideBar_Original =
        nullptr;


BOOL WINAPI CTray_AppBarSetAutoHideBar_Hook(
    void* pThis,
    HWND hwnd,
    BOOL autoHide,
    UINT edge,
    HMONITOR monitor
) {

    if (
        g_hidden.load(
            std::memory_order_relaxed
        ) &&
        monitor ==
            g_taskbarMonitor
    ) {

        autoHide =
            TRUE;

        edge =
            ABE_TOP;
    }


    return
        CTray_AppBarSetAutoHideBar_Original(
            pThis,
            hwnd,
            autoHide,
            edge,
            monitor
        );
}


/*
 * ============================================================
 * CTray::_OnAppBarMessage
 * ============================================================
 */

using CTray_OnAppBarMessage_t =
    UINT_PTR(WINAPI*)(
        void* pThis,
        COPYDATASTRUCT* cds
    );

CTray_OnAppBarMessage_t
    CTray_OnAppBarMessage_Original =
        nullptr;


UINT_PTR WINAPI CTray_OnAppBarMessage_Hook(
    void* pThis,
    COPYDATASTRUCT* cds
) {

    return
        CTray_OnAppBarMessage_Original(
            pThis,
            cds
        );
}


/*
 * ============================================================
 * TrayUI::GetAutoHideFlags
 * ============================================================
 */

using TrayUI_GetAutoHideFlags_t =
    DWORD(WINAPI*)(
        void* pThis
    );

TrayUI_GetAutoHideFlags_t
    TrayUI_GetAutoHideFlags_Original =
        nullptr;


DWORD WINAPI TrayUI_GetAutoHideFlags_Hook(
    void* pThis
) {

    if (
        g_hidden.load(
            std::memory_order_relaxed
        )
    ) {

        return ABS_AUTOHIDE;
    }


    return
        TrayUI_GetAutoHideFlags_Original(
            pThis
        );
}


/*
 * ============================================================
 * TrayUI::_Hide
 * ============================================================
 */

using TrayUI_Hide_t =
    void(WINAPI*)(
        void* pThis
    );

TrayUI_Hide_t
    TrayUI_Hide_Original =
        nullptr;


void WINAPI TrayUI_Hide_Hook(
    void* pThis
) {

    if (
        g_hidden.load(
            std::memory_order_relaxed
        )
    ) {
        return;
    }


    TrayUI_Hide_Original(
        pThis
    );
}


/*
 * ============================================================
 * CSecondaryTray
 * ============================================================
 */

using CSecondaryTray_GetMonitor_t =
    HMONITOR(WINAPI*)(
        void* pThis
    );

CSecondaryTray_GetMonitor_t
    CSecondaryTray_GetMonitor_Original =
        nullptr;


using CSecondaryTray_LoadSettings_t =
    HRESULT(WINAPI*)(
        void* pThis
    );

CSecondaryTray_LoadSettings_t
    CSecondaryTray_LoadSettings_Original =
        nullptr;


HRESULT WINAPI CSecondaryTray_LoadSettings_Hook(
    void* pThis
) {

    return
        CSecondaryTray_LoadSettings_Original(
            pThis
        );
}


using CSecondaryTray_CheckSize_t =
    void(WINAPI*)(
        void* pThis,
        int param1
    );

CSecondaryTray_CheckSize_t
    CSecondaryTray_CheckSize_Original =
        nullptr;


void WINAPI CSecondaryTray_CheckSize_Hook(
    void* pThis,
    int param1
) {

    CSecondaryTray_CheckSize_Original(
        pThis,
        param1
    );
}


/*
 * ============================================================
 * TaskbarHost
 * ============================================================
 */

using TaskbarHost_Start_t =
    void(WINAPI*)(
        void* pThis
    );

TaskbarHost_Start_t
    TaskbarHost_Start_Original =
        nullptr;


void WINAPI TaskbarHost_Start_Hook(
    void* pThis
) {

    TaskbarHost_Start_Original(
        pThis
    );
}


using TaskbarHost_Start_System_t =
    void(WINAPI*)(
        void* pThis
    );

TaskbarHost_Start_System_t
    TaskbarHost_Start_System_Original =
        nullptr;


void WINAPI TaskbarHost_Start_System_Hook(
    void* pThis
) {

    TaskbarHost_Start_System_Original(
        pThis
    );
}


/*
 * ============================================================
 * ViewCoordinator::ShouldTaskbarBeExpanded
 * ============================================================
 */

using ViewCoordinator_ShouldTaskbarBeExpanded_t =
    bool(WINAPI*)(
        void* pThis,
        HWND hMMTaskbarWnd,
        bool expanded
    );

ViewCoordinator_ShouldTaskbarBeExpanded_t
    ViewCoordinator_ShouldTaskbarBeExpanded_Original =
        nullptr;


bool WINAPI ViewCoordinator_ShouldTaskbarBeExpanded_Hook(
    void* pThis,
    HWND hMMTaskbarWnd,
    bool expanded
) {

    if (
        g_hidden.load(
            std::memory_order_relaxed
        ) &&
        hMMTaskbarWnd ==
            g_taskbarWindow
    ) {

        return false;
    }


    return
        ViewCoordinator_ShouldTaskbarBeExpanded_Original(
            pThis,
            hMMTaskbarWnd,
            expanded
        );
}


/*
 * ============================================================
 * Taskbar.View.dll
 * ============================================================
 */

HMODULE GetTaskbarViewModule() {

    HMODULE module =
        GetModuleHandleW(
            L"Taskbar.View.dll"
        );


    if (!module) {

        module =
            GetModuleHandleW(
                L"ExplorerExtensions.dll"
            );
    }


    return module;
}


bool HookTaskbarViewSymbols(
    HMODULE module
) {

    WindhawkUtils::SYMBOL_HOOK taskbar_view_dll_hooks[] = {

        {
            {
                LR"(public: bool __cdecl winrt::Taskbar::implementation::ViewCoordinator::ShouldTaskbarBeExpanded(unsigned __int64,bool))"
            },

            &ViewCoordinator_ShouldTaskbarBeExpanded_Original,

            ViewCoordinator_ShouldTaskbarBeExpanded_Hook,

            true
        }
    };


    if (
        !WindhawkUtils::HookSymbols(
            module,
            taskbar_view_dll_hooks,
            ARRAYSIZE(taskbar_view_dll_hooks)
        )
    ) {

        Wh_Log(
            L"Taskbar.View.dll HookSymbols failed"
        );

        return false;
    }


    Wh_Log(
        L"Taskbar.View.dll hook installed"
    );


    return true;
}


/*
 * ============================================================
 * SHOW TASKBAR
 * ============================================================
 */

void ShowTaskbar() {

    if (
        !IsWindow(
            g_taskbarWindow
        )
    ) {
        return;
    }


    if (
        !g_hidden.load(
            std::memory_order_relaxed
        )
    ) {
        return;
    }


    if (
        g_animating.load(
            std::memory_order_relaxed
        )
    ) {
        return;
    }


    RECT taskbarRect{};

    if (
        !GetWindowRect(
            g_taskbarWindow,
            &taskbarRect
        )
    ) {
        return;
    }


    RECT monitorRect{};

    if (
        !GetMonitorRect(
            g_taskbarMonitor,
            &monitorRect
        )
    ) {
        return;
    }


    int height =
        taskbarRect.bottom -
        taskbarRect.top;


    int finalY =
        monitorRect.top;


    int startY =
        finalY -
        height;


    g_hidden.store(
        false,
        std::memory_order_relaxed
    );


    SetWindowPos(
        g_taskbarWindow,
        HWND_TOP,
        taskbarRect.left,
        startY,
        0,
        0,
        SWP_NOSIZE |
        SWP_NOACTIVATE |
        SWP_SHOWWINDOW
    );


    AnimateTaskbar(
        startY,
        finalY,
        g_settings.animationDuration
    );


    Wh_Log(
        L"Taskbar shown smoothly"
    );
}


/*
 * ============================================================
 * HIDE TASKBAR
 * ============================================================
 */

void HideTaskbar() {

    if (
        !IsWindow(
            g_taskbarWindow
        )
    ) {
        return;
    }


    if (
        g_hidden.load(
            std::memory_order_relaxed
        )
    ) {
        return;
    }


    if (
        g_animating.load(
            std::memory_order_relaxed
        )
    ) {
        return;
    }


    if (
        CursorOverTaskbar()
    ) {
        return;
    }


    RECT taskbarRect{};

    if (
        !GetWindowRect(
            g_taskbarWindow,
            &taskbarRect
        )
    ) {
        return;
    }


    RECT monitorRect{};

    if (
        !GetMonitorRect(
            g_taskbarMonitor,
            &monitorRect
        )
    ) {
        return;
    }


    int height =
        taskbarRect.bottom -
        taskbarRect.top;


    int startY =
        monitorRect.top;


    int endY =
        monitorRect.top -
        height;


    g_hidden.store(
        true,
        std::memory_order_relaxed
    );


    AnimateTaskbar(
        startY,
        endY,
        g_settings.animationDuration
    );


    ShowWindowAsync(
        g_taskbarWindow,
        SW_HIDE
    );


    Wh_Log(
        L"Taskbar hidden smoothly"
    );
}


/*
 * ============================================================
 * WORKER
 * ============================================================
 */

DWORD WINAPI WorkerThread(
    LPVOID
) {

    LoadSettings();


    ULONGLONG lastTaskbarScan =
        0;

    ULONGLONG outsideSince =
        GetTickCount64();

    ULONGLONG edgeSince =
        0;


    while (
        g_running.load(
            std::memory_order_relaxed
        )
    ) {

        ULONGLONG now =
            GetTickCount64();


        if (
            now -
            lastTaskbarScan >=
            1000
        ) {

            HWND taskbar =
                FindTopTaskbar();


            if (
                taskbar !=
                g_taskbarWindow
            ) {

                g_taskbarWindow =
                    taskbar;


                g_taskbarMonitor =
                    GetTaskbarMonitor();


                g_hidden.store(
                    false,
                    std::memory_order_relaxed
                );


                Wh_Log(
                    L"Found top taskbar: %p",
                    taskbar
                );
            }


            lastTaskbarScan =
                now;
        }


        if (
            !IsWindow(
                g_taskbarWindow
            )
        ) {

            Sleep(50);

            continue;
        }


        if (
            g_animating.load(
                std::memory_order_relaxed
            )
        ) {

            Sleep(8);

            continue;
        }


        bool atTop =
            CursorAtTopEdge();


        bool overTaskbar =
            CursorOverTaskbar();


        if (
            g_hidden.load(
                std::memory_order_relaxed
            )
        ) {

            if (atTop) {

                if (
                    edgeSince ==
                    0
                ) {

                    edgeSince =
                        now;
                }


                if (
                    now -
                    edgeSince >=
                    (ULONGLONG)
                    g_settings.showDelay
                ) {

                    ShowTaskbar();

                    edgeSince =
                        0;
                }

            }
            else {

                edgeSince =
                    0;
            }
        }
        else {

            if (
                atTop ||
                overTaskbar
            ) {

                outsideSince =
                    now;

            }
            else {

                if (
                    now -
                    outsideSince >=
                    (ULONGLONG)
                    g_settings.hideDelay
                ) {

                    HideTaskbar();

                    outsideSince =
                        now;
                }
            }
        }


        Sleep(10);
    }


    g_hidden.store(
        false,
        std::memory_order_relaxed
    );


    if (
        IsWindow(
            g_taskbarWindow
        )
    ) {

        RECT monitor{};

        if (
            GetMonitorRect(
                g_taskbarMonitor,
                &monitor
            )
        ) {

            RECT rc{};

            if (
                GetWindowRect(
                    g_taskbarWindow,
                    &rc
                )
            ) {

                SetWindowPos(
                    g_taskbarWindow,
                    HWND_TOP,
                    rc.left,
                    monitor.top,
                    0,
                    0,
                    SWP_NOSIZE |
                    SWP_NOACTIVATE |
                    SWP_SHOWWINDOW
                );
            }
        }


        ShowWindowAsync(
            g_taskbarWindow,
            SW_SHOWNOACTIVATE
        );
    }


    return 0;
}


/*
 * ============================================================
 * INIT
 * ============================================================
 */

BOOL Wh_ModInit() {

    Wh_Log(
        L"Top Taskbar Auto-Hide v1.0 init"
    );


    LoadSettings();


    HMODULE taskbarDll =
        LoadLibraryExW(
            L"taskbar.dll",
            nullptr,
            LOAD_LIBRARY_SEARCH_SYSTEM32
        );


    if (!taskbarDll) {

        Wh_Log(
            L"Couldn't load taskbar.dll"
        );

        return FALSE;
    }


    WindhawkUtils::SYMBOL_HOOK taskbar_dll_hooks[] = {

        {
            {
                LR"(public: virtual struct HMONITOR__ * __cdecl CSecondaryTray::GetMonitor(void))"
            },

            &CSecondaryTray_GetMonitor_Original
        },


        {
            {
                LR"(public: virtual unsigned int __cdecl TrayUI::GetAutoHideFlags(void))"
            },

            &TrayUI_GetAutoHideFlags_Original,

            TrayUI_GetAutoHideFlags_Hook
        },


        {
            {
                LR"(public: void __cdecl TrayUI::_Hide(void))"
            },

            &TrayUI_Hide_Original,

            TrayUI_Hide_Hook
        },


        {
            {
                LR"(private: long __cdecl CSecondaryTray::_LoadSettings(void))"
            },

            &CSecondaryTray_LoadSettings_Original,

            CSecondaryTray_LoadSettings_Hook
        },


        {
            {
                LR"(public: virtual void __cdecl CSecondaryTray::CheckSize(int))"
            },

            &CSecondaryTray_CheckSize_Original,

            CSecondaryTray_CheckSize_Hook
        },


        {
            {
                LR"(public: void __cdecl TaskbarHost::Start_System(void))"
            },

            &TaskbarHost_Start_System_Original,

            TaskbarHost_Start_System_Hook,

            true
        },


        {
            {
                LR"(public: void __cdecl TaskbarHost::Start(void))"
            },

            &TaskbarHost_Start_Original,

            TaskbarHost_Start_Hook,

            true
        }
    };


    if (
        !WindhawkUtils::HookSymbols(
            taskbarDll,
            taskbar_dll_hooks,
            ARRAYSIZE(taskbar_dll_hooks)
        )
    ) {

        Wh_Log(
            L"taskbar.dll HookSymbols failed"
        );

        return FALSE;
    }


    Wh_Log(
        L"taskbar.dll hooks installed"
    );


    HMODULE explorer =
        GetModuleHandleW(
            nullptr
        );


    WindhawkUtils::SYMBOL_HOOK explorer_exe_hooks[] = {

        {
            {
                LR"(private: int __cdecl CTray::RecomputeWorkArea(struct HMONITOR__ *,struct tagRECT *))"
            },

            &CTray_RecomputeWorkArea_Original,

            CTray_RecomputeWorkArea_Hook
        },


        {
            {
                LR"(public: virtual int __cdecl CTray::AppBarSetAutoHideBar(struct HWND__ *,int,unsigned int,struct HMONITOR__ *))"
            },

            &CTray_AppBarSetAutoHideBar_Original,

            CTray_AppBarSetAutoHideBar_Hook
        },


        {
            {
                LR"(protected: __int64 __cdecl CTray::_OnAppBarMessage(struct tagCOPYDATASTRUCT *))"
            },

            &CTray_OnAppBarMessage_Original,

            CTray_OnAppBarMessage_Hook
        }
    };


    if (
        !WindhawkUtils::HookSymbols(
            explorer,
            explorer_exe_hooks,
            ARRAYSIZE(explorer_exe_hooks)
        )
    ) {

        Wh_Log(
            L"explorer.exe HookSymbols failed"
        );

        return FALSE;
    }


    Wh_Log(
        L"explorer.exe hooks installed"
    );


    HMODULE taskbarView =
        GetTaskbarViewModule();


    if (
        taskbarView
    ) {

        if (
            !HookTaskbarViewSymbols(
                taskbarView
            )
        ) {

            return FALSE;
        }
    }


    g_taskbarWindow =
        FindTopTaskbar();


    if (
        g_taskbarWindow
    ) {

        g_taskbarMonitor =
            GetTaskbarMonitor();


        Wh_Log(
            L"Found top taskbar: %p",
            g_taskbarWindow
        );
    }
    else {

        Wh_Log(
            L"No top taskbar found"
        );
    }


    g_running.store(
        true,
        std::memory_order_relaxed
    );


    g_workerThread =
        CreateThread(
            nullptr,
            0,
            WorkerThread,
            nullptr,
            0,
            nullptr
        );


    if (
        !g_workerThread
    ) {

        g_running.store(
            false,
            std::memory_order_relaxed
        );

        return FALSE;
    }


    return TRUE;
}


/*
 * ============================================================
 * AFTER INIT
 * ============================================================
 */

void Wh_ModAfterInit() {

    Wh_Log(
        L"Top Taskbar Auto-Hide v1.0 initialized"
    );
}


/*
 * ============================================================
 * UNINIT
 * ============================================================
 */

void Wh_ModUninit() {

    Wh_Log(
        L"Top Taskbar Auto-Hide v1.0 uninit"
    );


    g_running.store(
        false,
        std::memory_order_relaxed
    );


    if (
        g_workerThread
    ) {

        WaitForSingleObject(
            g_workerThread,
            3000
        );


        CloseHandle(
            g_workerThread
        );


        g_workerThread =
            nullptr;
    }


    g_hidden.store(
        false,
        std::memory_order_relaxed
    );


    g_animating.store(
        false,
        std::memory_order_relaxed
    );


    if (
        IsWindow(
            g_taskbarWindow
        )
    ) {

        RECT monitor{};

        if (
            GetMonitorRect(
                g_taskbarMonitor,
                &monitor
            )
        ) {

            RECT rc{};

            if (
                GetWindowRect(
                    g_taskbarWindow,
                    &rc
                )
            ) {

                SetWindowPos(
                    g_taskbarWindow,
                    HWND_TOP,
                    rc.left,
                    monitor.top,
                    0,
                    0,
                    SWP_NOSIZE |
                    SWP_NOACTIVATE |
                    SWP_SHOWWINDOW
                );
            }
        }


        ShowWindowAsync(
            g_taskbarWindow,
            SW_SHOWNOACTIVATE
        );
    }


    g_taskbarWindow =
        nullptr;

    g_taskbarMonitor =
        nullptr;
}


/*
 * ============================================================
 * SETTINGS CHANGED
 * ============================================================
 */

void Wh_ModSettingsChanged() {

    LoadSettings();
}
