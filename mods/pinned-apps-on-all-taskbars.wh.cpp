// ==WindhawkMod==
// @id              pinned-apps-on-all-taskbars
// @name            Pinned apps on all taskbars
// @description     Shows pinned apps on every monitor while keeping running apps on the taskbar where their window is open
// @version         2.1.0
// @author          Div
// @github          https://github.com/Divc09
// @include         explorer.exe
// @architecture    x86-64
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Pinned apps on all taskbars

Shows pinned taskbar apps on every monitor without duplicating running-window
buttons across monitors.

The mod lets Explorer use its complete native **All taskbars** path for pinned
launchers, including launching, ordering, pin/unpin updates, and the transition
back to a pinned button after the last window closes. It then keeps the native
per-monitor filter enabled for real window items. This produces the combination
that isn't offered by Windows Settings: pinned launchers on every taskbar, but
open windows only on the taskbar for their monitor.

## Setup

In **Settings > Personalization > Taskbar > Taskbar behaviors**, enable taskbars
on all displays and set **When using multiple displays, show my taskbar apps
on** to **Taskbar where window is open**.

Enabling and disabling the mod rebuilds the taskbar models automatically; an
Explorer restart isn't required.

## Compatibility

This mod targets the native Windows 11 taskbar on 64-bit Windows. Taskbar
replacements such as ExplorerPatcher are not supported.
*/
// ==/WindhawkModReadme==

#include <windhawk_utils.h>

namespace {

using GetMultiMonTaskbarMode_t = int(WINAPI*)();
GetMultiMonTaskbarMode_t GetMultiMonTaskbarMode_Original;

using TaskItemFilterFactory_CreateFilter_t = HRESULT(WINAPI*)(
    void* taskItemFilterFactory,
    bool isPrimaryTaskbar,
    HMONITOR monitor,
    REFIID riid,
    void** result);
TaskItemFilterFactory_CreateFilter_t TaskItemFilterFactory_CreateFilter_Original;

using TaskItemFilter_CreateInstance_t = HRESULT(WINAPI*)(
    HMONITOR monitor,
    unsigned int flags,
    void* trayComponentHost,
    REFIID riid,
    void** result);
TaskItemFilter_CreateInstance_t TaskItemFilter_CreateInstance_Original;

// In "All taskbars" mode, TaskItemFilterFactory normally discards the monitor
// before it creates the filter. Keep the monitor passed to CreateFilter so the
// CreateInstance hook can put it back. Thread-local storage is needed because
// Explorer can build taskbars concurrently on different UI threads.
thread_local unsigned int g_createFilterDepth;
thread_local HMONITOR g_createFilterMonitor;

int WINAPI GetMultiMonTaskbarMode_Hook() {
    // MultiMonTaskbarMode 0 is Windows' native "All taskbars" mode. Using the
    // native controller path is important: it creates real launchers and owns
    // their click, close, pin/unpin, and ordering lifecycle.
    constexpr int kAllTaskbars = 0;
    return kAllTaskbars;
}

HRESULT WINAPI TaskItemFilterFactory_CreateFilter_Hook(
    void* taskItemFilterFactory,
    bool isPrimaryTaskbar,
    HMONITOR monitor,
    REFIID riid,
    void** result) {
    HMONITOR previousMonitor = g_createFilterMonitor;
    g_createFilterMonitor = monitor;
    g_createFilterDepth++;

    HRESULT hr = TaskItemFilterFactory_CreateFilter_Original(
        taskItemFilterFactory, isPrimaryTaskbar, monitor, riid, result);

    g_createFilterDepth--;
    g_createFilterMonitor = previousMonitor;
    return hr;
}

HRESULT WINAPI TaskItemFilter_CreateInstance_Hook(HMONITOR monitor,
                                                   unsigned int flags,
                                                   void* trayComponentHost,
                                                   REFIID riid,
                                                   void** result) {
    // Native "All taskbars" mode normally clears the monitor-filter bit and
    // also replaces the taskbar's monitor with nullptr. Restore both pieces so
    // real windows remain local to their monitor, while clearing bit 1 so the
    // native pinned launchers remain eligible.
    constexpr unsigned int kFilterRealItemsByMonitor = 0x1;
    constexpr unsigned int kExcludePinnedLaunchers = 0x2;

    if (g_createFilterDepth != 0) {
        monitor = g_createFilterMonitor;
        flags = (flags | kFilterRealItemsByMonitor) & ~kExcludePinnedLaunchers;
    }

    return TaskItemFilter_CreateInstance_Original(
        monitor, flags, trayComponentHost, riid, result);
}

void RefreshTaskbarFilters() {
    // Notify every top-level taskbar, including Shell_SecondaryTrayWnd
    // instances. Notifying only Shell_TrayWnd refreshes the primary model but
    // can leave secondary taskbars using the filter created by the mod after
    // the mod is disabled. The send is synchronous, so the string remains
    // valid until all recipients have handled it.
    DWORD_PTR result;
    SendMessageTimeoutW(HWND_BROADCAST, WM_SETTINGCHANGE, 0,
                        reinterpret_cast<LPARAM>(L"TraySettings"),
                        SMTO_ABORTIFHUNG | SMTO_BLOCK, 2000, &result);
}

}  // namespace

BOOL Wh_ModInit() {
    Wh_Log(L"Initializing");

    HMODULE taskbarModule =
        LoadLibraryExW(L"taskbar.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!taskbarModule) {
        Wh_Log(L"Failed to load taskbar.dll");
        return FALSE;
    }

    WindhawkUtils::SYMBOL_HOOK hooks[] = {
        {
            {LR"(enum TrayCommon::MultiMonTaskbarMode __cdecl TrayCommon::GetMultiMonTaskbarMode(void))"},
            &GetMultiMonTaskbarMode_Original,
            GetMultiMonTaskbarMode_Hook,
        },
        {
            {LR"(public: virtual long __cdecl TaskItemFilterFactory::CreateFilter(bool,struct HMONITOR__ *,struct _GUID const &,void * *))"},
            &TaskItemFilterFactory_CreateFilter_Original,
            TaskItemFilterFactory_CreateFilter_Hook,
        },
        {
            {LR"(long __cdecl TaskItemFilter_CreateInstance(struct HMONITOR__ *,enum TaskItemFilterFlags,struct ITrayComponentHost *,struct _GUID const &,void * *))"},
            &TaskItemFilter_CreateInstance_Original,
            TaskItemFilter_CreateInstance_Hook,
        },
    };

    if (!WindhawkUtils::HookSymbols(taskbarModule, hooks,
                                    ARRAYSIZE(hooks))) {
        Wh_Log(L"Failed to hook taskbar symbols");
        return FALSE;
    }

    return TRUE;
}

void Wh_ModAfterInit() {
    RefreshTaskbarFilters();
}

void Wh_ModBeforeUninit() {
    // Hooks are still active here, so don't refresh until they are removed.
}

void Wh_ModUninit() {
    RefreshTaskbarFilters();
}
