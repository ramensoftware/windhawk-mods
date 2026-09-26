// ==WindhawkMod==
// @id              hide-taskbar-tooltips
// @name            Hide Taskbar Tooltips
// @description     Suppresses native Windows 11 XAML hover tooltips in Explorer (taskbar buttons, system tray icons, and clock).
// @version         1.0.6
// @author          gilnett
// @github          https://github.com/gilnett
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject
// @license         GPL-3.0
// ==/WindhawkMod==

// Source code is published under The GNU General Public License v3.0.

// ==WindhawkModReadme==
/*
# Hide Taskbar Tooltips

Suppresses native Windows 11 taskbar hover tooltips, including:
- Taskbar app icon tooltips and labels on hover
- System tray status icons tooltips (Network, Volume, Battery, Clock)

## Preview

**Before** (native Windows 11 tooltip on hover):  
![Before](https://i.imgur.com/dLAXEzl.png)

**After** (tooltip suppressed):  
![After](https://i.imgur.com/tISQj2F.png)

## Note

Restarting Explorer is recommended after installing or enabling the mod for changes to take full effect across all existing taskbar elements.

## Compatibility

- Only Windows 11 is supported.
*/
// ==/WindhawkModReadme==

#include <windhawk_api.h>
#include <windhawk_utils.h>

#include <windows.h>

#include <atomic>
#include <mutex>
#include <unordered_set>

#undef GetCurrentTime

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.Xaml.Controls.h>

static bool IsWindows11OrGreater() {
    HMODULE hNtdll = GetModuleHandleW(L"ntdll.dll");
    if (!hNtdll) {
        return false;
    }

    using fnRtlGetNtVersionNumbers =
        void(WINAPI*)(LPDWORD major, LPDWORD minor, LPDWORD build);
    auto RtlGetNtVersionNumbers =
        reinterpret_cast<fnRtlGetNtVersionNumbers>(
            GetProcAddress(hNtdll, "RtlGetNtVersionNumbers"));
    if (!RtlGetNtVersionNumbers) {
        return false;
    }

    DWORD major = 0, minor = 0, build = 0;
    RtlGetNtVersionNumbers(&major, &minor, &build);
    build &= ~0xF0000000;
    return (major > 10) || (major == 10 && build >= 22000);
}

static thread_local int t_tooltipScopeDepth = 0;

struct ToolTipScope {
    ToolTipScope() {
        t_tooltipScopeDepth++;
    }
    ~ToolTipScope() {
        t_tooltipScopeDepth--;
    }
};

static std::unordered_set<HWND> s_tooltipWindows;
static std::mutex s_tooltipWindowsMutex;
static std::atomic<bool> s_hasTrackedWindows{false};

static void RecordTooltipWindow(HWND hWnd) {
    if (!hWnd) {
        return;
    }
    std::lock_guard<std::mutex> lock(s_tooltipWindowsMutex);
    s_tooltipWindows.insert(hWnd);
    s_hasTrackedWindows.store(true, std::memory_order_relaxed);
}

static bool IsTooltipWindow(HWND hWnd) {
    if (!hWnd || !s_hasTrackedWindows.load(std::memory_order_relaxed)) {
        return false;
    }
    std::lock_guard<std::mutex> lock(s_tooltipWindowsMutex);
    return s_tooltipWindows.find(hWnd) != s_tooltipWindows.end();
}

using CreateWindowExW_t = decltype(&CreateWindowExW);
static CreateWindowExW_t CreateWindowExW_Original = nullptr;

static HWND WINAPI CreateWindowExW_Hook(DWORD dwExStyle,
                                        LPCWSTR lpClassName,
                                        LPCWSTR lpWindowName,
                                        DWORD dwStyle,
                                        int X,
                                        int Y,
                                        int nWidth,
                                        int nHeight,
                                        HWND hWndParent,
                                        HMENU hMenu,
                                        HINSTANCE hInstance,
                                        LPVOID lpParam) {
    if (t_tooltipScopeDepth > 0) {
        Wh_Log(L"> CreateWindowExW in tooltip scope: class=%s, style=0x%X",
               lpClassName ? lpClassName : L"<null>", dwStyle);
        dwStyle &= ~WS_VISIBLE;
    }
    HWND hWnd = CreateWindowExW_Original(dwExStyle, lpClassName, lpWindowName,
                                         dwStyle, X, Y, nWidth, nHeight,
                                         hWndParent, hMenu, hInstance, lpParam);
    if (hWnd && t_tooltipScopeDepth > 0) {
        RecordTooltipWindow(hWnd);
    }
    return hWnd;
}

using ShowWindow_t = decltype(&ShowWindow);
static ShowWindow_t ShowWindow_Original = nullptr;

static BOOL WINAPI ShowWindow_Hook(HWND hWnd, int nCmdShow) {
    if (t_tooltipScopeDepth > 0 ||
        (s_hasTrackedWindows.load(std::memory_order_relaxed) &&
         IsTooltipWindow(hWnd))) {
        Wh_Log(L"> ShowWindow suppressed for tooltip window %p, cmd=%d", hWnd,
               nCmdShow);
        if (nCmdShow != SW_HIDE) {
            return FALSE;
        }
    }
    return ShowWindow_Original(hWnd, nCmdShow);
}

using SetWindowPos_t = decltype(&SetWindowPos);
static SetWindowPos_t SetWindowPos_Original = nullptr;

static BOOL WINAPI SetWindowPos_Hook(HWND hWnd,
                                     HWND hWndInsertAfter,
                                     int X,
                                     int Y,
                                     int cx,
                                     int cy,
                                     UINT uFlags) {
    if (t_tooltipScopeDepth > 0 ||
        (s_hasTrackedWindows.load(std::memory_order_relaxed) &&
         IsTooltipWindow(hWnd))) {
        if (t_tooltipScopeDepth > 0 && !IsTooltipWindow(hWnd)) {
            WCHAR className[64] = {0};
            GetClassNameW(hWnd, className, ARRAYSIZE(className));
            Wh_Log(L"> SetWindowPos in tooltip scope: %p (%s), flags=0x%X", hWnd,
                   className, uFlags);
            RecordTooltipWindow(hWnd);
        }
        if (uFlags & SWP_SHOWWINDOW) {
            Wh_Log(L"> SetWindowPos SWP_SHOWWINDOW stripped for %p", hWnd);
            uFlags &= ~SWP_SHOWWINDOW;
            uFlags |= SWP_HIDEWINDOW;
        }
    }
    return SetWindowPos_Original(hWnd, hWndInsertAfter, X, Y, cx, cy, uFlags);
}

// WinRT IToolTip ABI: put_IsOpen is at vtable index 9
using ToolTip_put_IsOpen_t = HRESULT(__stdcall*)(void* pThis, boolean value);
static ToolTip_put_IsOpen_t g_toolTipPutIsOpenOriginal = nullptr;

static HRESULT __stdcall ToolTip_put_IsOpen_Hook(void* pThis, boolean value) {
    if (value) {
        // Suppress opening any tooltip popup in the XAML framework with zero overhead
        return S_OK;
    }
    if (g_toolTipPutIsOpenOriginal) {
        return g_toolTipPutIsOpenOriginal(pThis, value);
    }
    return S_OK;
}

static bool SuppressToolTip(void* pToolTip) {
    if (!pToolTip) {
        return false;
    }

    void* pIToolTip = *(reinterpret_cast<void**>(pToolTip));
    if (!pIToolTip) {
        return false;
    }

    // Verify and obtain true IToolTip vtable via QueryInterface
    IUnknown* unk = reinterpret_cast<IUnknown*>(pIToolTip);
    winrt::com_ptr<winrt::Windows::UI::Xaml::Controls::IToolTip> spToolTip;
    if (FAILED(unk->QueryInterface(
            winrt::guid_of<winrt::Windows::UI::Xaml::Controls::IToolTip>(),
            spToolTip.put_void()))) {
        return false;
    }

    void** vtable = *reinterpret_cast<void***>(spToolTip.get());
    if (!vtable || !vtable[9]) {
        return false;
    }

    auto put_IsOpen = reinterpret_cast<ToolTip_put_IsOpen_t>(vtable[9]);

    // Install global hook on ToolTip::put_IsOpen exactly once from the first live instance
    static std::atomic<bool> s_hookInstalled{false};
    if (!s_hookInstalled.exchange(true)) {
        WindhawkUtils::SetFunctionHook(
            reinterpret_cast<void*>(put_IsOpen),
            reinterpret_cast<void*>(ToolTip_put_IsOpen_Hook),
            reinterpret_cast<void**>(&g_toolTipPutIsOpenOriginal));
        Wh_ApplyHookOperations();
        Wh_Log(L"> Successfully hooked ToolTip::put_IsOpen on first live instance");
    }

    // Immediately close the tooltip instance
    put_IsOpen(spToolTip.get(), FALSE);
    return true;
}

static BOOL CALLBACK EnumWindowsTaskbarProc(HWND hWnd, LPARAM lParam) {
    DWORD dwProcessId = 0;
    WCHAR className[32] = {0};
    if (GetWindowThreadProcessId(hWnd, &dwProcessId) &&
        dwProcessId == GetCurrentProcessId() &&
        GetClassNameW(hWnd, className, ARRAYSIZE(className)) &&
        _wcsicmp(className, L"Shell_TrayWnd") == 0) {
        *reinterpret_cast<HWND*>(lParam) = hWnd;
        return FALSE;
    }
    return TRUE;
}

static HWND FindCurrentProcessTaskbarWnd() {
    HWND hTaskbarWnd = nullptr;
    EnumWindows(EnumWindowsTaskbarProc, reinterpret_cast<LPARAM>(&hTaskbarWnd));
    return hTaskbarWnd;
}

using RunFromWindowThreadProc_t = void (*)(void* parameter);

struct RUN_FROM_WINDOW_THREAD_PARAM {
    RunFromWindowThreadProc_t proc;
    void* procParam;
};

static LRESULT CALLBACK CallWndProcHook(int nCode, WPARAM wParam, LPARAM lParam) {
    static const UINT runFromWindowThreadRegisteredMsg =
        RegisterWindowMessageW(L"Windhawk_RunFromWindowThread_hide_taskbar_tooltips");

    if (nCode == HC_ACTION) {
        const auto* cwp = reinterpret_cast<const CWPSTRUCT*>(lParam);
        if (cwp->message == runFromWindowThreadRegisteredMsg) {
            auto* param =
                reinterpret_cast<RUN_FROM_WINDOW_THREAD_PARAM*>(cwp->lParam);
            if (param && param->proc) {
                param->proc(param->procParam);
            }
        }
    }

    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

static bool RunFromWindowThread(HWND hWnd,
                                RunFromWindowThreadProc_t proc,
                                void* procParam) {
    static const UINT runFromWindowThreadRegisteredMsg =
        RegisterWindowMessageW(L"Windhawk_RunFromWindowThread_hide_taskbar_tooltips");

    DWORD dwThreadId = GetWindowThreadProcessId(hWnd, nullptr);
    if (dwThreadId == 0) {
        return false;
    }

    if (dwThreadId == GetCurrentThreadId()) {
        proc(procParam);
        return true;
    }

    HHOOK hook = SetWindowsHookExW(
        WH_CALLWNDPROC,
        CallWndProcHook,
        nullptr, dwThreadId);
    if (!hook) {
        return false;
    }

    RUN_FROM_WINDOW_THREAD_PARAM param;
    param.proc = proc;
    param.procParam = procParam;
    DWORD_PTR dwResult = 0;
    SendMessageTimeoutW(hWnd, runFromWindowThreadRegisteredMsg, 0,
                        reinterpret_cast<LPARAM>(&param),
                        SMTO_ABORTIFHUNG | SMTO_NORMAL, 3000, &dwResult);

    UnhookWindowsHookEx(hook);
    return true;
}

static void EagerHookToolTipOnTaskbarThread(void* /*procParam*/) {
    try {
        winrt::Windows::UI::Xaml::Controls::ToolTip dummyToolTip;
        void* pUnk = winrt::get_abi(dummyToolTip);
        if (pUnk) {
            void** vtable = *reinterpret_cast<void***>(pUnk);
            if (vtable && vtable[9]) {
                auto put_IsOpen = reinterpret_cast<ToolTip_put_IsOpen_t>(vtable[9]);
                static std::atomic<bool> s_eagerHookInstalled{false};
                if (!s_eagerHookInstalled.exchange(true)) {
                    WindhawkUtils::SetFunctionHook(
                        reinterpret_cast<void*>(put_IsOpen),
                        reinterpret_cast<void*>(ToolTip_put_IsOpen_Hook),
                        reinterpret_cast<void**>(&g_toolTipPutIsOpenOriginal));
                    Wh_ApplyHookOperations();
                    Wh_Log(L"> Successfully hooked ToolTip::put_IsOpen eagerly on Taskbar UI thread!");
                }
            }
        }
    } catch (...) {
        Wh_Log(L"> Eager ToolTip hook on UI thread caught exception");
    }
}

static void InitEagerToolTipHook() {
    HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();
    if (hTaskbarWnd) {
        Wh_Log(L"> Dispatching eager ToolTip hook to Taskbar window %p", hTaskbarWnd);
        RunFromWindowThread(hTaskbarWnd, EagerHookToolTipOnTaskbarThread, nullptr);
    } else {
        Wh_Log(L"> Taskbar window not found yet for eager ToolTip hook");
    }
}

// SystemTray.dll: TaskbarLocationHelpers::PositionTaskbarTooltip
using PositionTaskbarTooltip_t = void(__cdecl*)(void* pToolTip,
                                                void* pTarget,
                                                int location,
                                                bool b1,
                                                bool b2);
static PositionTaskbarTooltip_t PositionTaskbarTooltip_Original = nullptr;

static void __cdecl PositionTaskbarTooltip_Hook(void* pToolTip,
                                                void* pTarget,
                                                int location,
                                                bool b1,
                                                bool b2) {
    ToolTipScope scope;
    if (!SuppressToolTip(pToolTip) && PositionTaskbarTooltip_Original) {
        PositionTaskbarTooltip_Original(pToolTip, pTarget, location, b1, b2);
    }
}

// SystemTray.dll & Taskbar.View.dll: TaskbarLocationHelpers::ApplyTaskbarTooltipPlacement (4 params)
using ApplyTaskbarTooltipPlacement_t =
    void(__cdecl*)(void* pToolTip,
                   int location,
                   winrt::Windows::Foundation::Size size1,
                   winrt::Windows::Foundation::Size size2);

static ApplyTaskbarTooltipPlacement_t
    ApplyTaskbarTooltipPlacement_Tray_Original = nullptr;

static void __cdecl ApplyTaskbarTooltipPlacement_Tray_Hook(
    void* pToolTip,
    int location,
    winrt::Windows::Foundation::Size size1,
    winrt::Windows::Foundation::Size size2) {
    ToolTipScope scope;
    if (!SuppressToolTip(pToolTip) &&
        ApplyTaskbarTooltipPlacement_Tray_Original) {
        ApplyTaskbarTooltipPlacement_Tray_Original(pToolTip, location, size1,
                                                   size2);
    }
}

static ApplyTaskbarTooltipPlacement_t
    ApplyTaskbarTooltipPlacement_View_Original = nullptr;

static void __cdecl ApplyTaskbarTooltipPlacement_View_Hook(
    void* pToolTip,
    int location,
    winrt::Windows::Foundation::Size size1,
    winrt::Windows::Foundation::Size size2) {
    ToolTipScope scope;
    if (!SuppressToolTip(pToolTip) &&
        ApplyTaskbarTooltipPlacement_View_Original) {
        ApplyTaskbarTooltipPlacement_View_Original(pToolTip, location, size1,
                                                   size2);
    }
}

// Taskbar.View.dll: TaskbarLocationHelpers::ApplyTaskbarTooltipPlacement (6 params)
using ApplyTaskbarTooltipPlacement6_t =
    void(__cdecl*)(void* pToolTip,
                   int location,
                   winrt::Windows::Foundation::Size size1,
                   winrt::Windows::Foundation::Size size2,
                   bool b1,
                   bool b2);
static ApplyTaskbarTooltipPlacement6_t ApplyTaskbarTooltipPlacement6_Original =
    nullptr;

static void __cdecl ApplyTaskbarTooltipPlacement6_Hook(
    void* pToolTip,
    int location,
    winrt::Windows::Foundation::Size size1,
    winrt::Windows::Foundation::Size size2,
    bool b1,
    bool b2) {
    ToolTipScope scope;
    if (!SuppressToolTip(pToolTip) && ApplyTaskbarTooltipPlacement6_Original) {
        ApplyTaskbarTooltipPlacement6_Original(pToolTip, location, size1, size2,
                                               b1, b2);
    }
}

// Hover state hooks in Taskbar.View.dll (zero-cost no-ops)
using ExperienceToggleButton_UpdateHover_t = void(__cdecl*)(void* pThis,
                                                            bool isHover);
static ExperienceToggleButton_UpdateHover_t
    ExperienceToggleButton_UpdateHover_Original = nullptr;

static void __cdecl ExperienceToggleButton_UpdateHover_Hook(void* /*pThis*/,
                                                            bool /*isHover*/) {
    ToolTipScope scope;
}

using TaskListButton_UpdateHover_t = void(__cdecl*)(void* pThis);
static TaskListButton_UpdateHover_t TaskListButton_UpdateHover_Original =
    nullptr;

static void __cdecl TaskListButton_UpdateHover_Hook(void* /*pThis*/) {
    ToolTipScope scope;
}

using OverflowToggleButton_UpdateHover_t = void(__cdecl*)(void* pThis);
static OverflowToggleButton_UpdateHover_t
    OverflowToggleButton_UpdateHover_Original = nullptr;

static void __cdecl OverflowToggleButton_UpdateHover_Hook(void* /*pThis*/) {
    ToolTipScope scope;
}

using TaskListButton_AddToolTipContent_t = void(__cdecl*)(void* pThis);
static TaskListButton_AddToolTipContent_t
    TaskListButton_AddToolTipContent_Original = nullptr;

static void __cdecl TaskListButton_AddToolTipContent_Hook(void* /*pThis*/) {
    ToolTipScope scope;
}

using TaskItemThumbnailView_UpdateToolTip_t = void(__cdecl*)(void* pThis);
static TaskItemThumbnailView_UpdateToolTip_t
    TaskItemThumbnailView_UpdateToolTip_Original = nullptr;

static void __cdecl TaskItemThumbnailView_UpdateToolTip_Hook(void* /*pThis*/) {
    ToolTipScope scope;
}

// Hover state hook in SystemTray.dll (zero-cost no-op)
using IconView_UpdateOuterToolTipPlacement_t = void(__cdecl*)(void* pThis,
                                                              bool isHover);
static IconView_UpdateOuterToolTipPlacement_t
    IconView_UpdateOuterToolTipPlacement_Original = nullptr;

static void __cdecl IconView_UpdateOuterToolTipPlacement_Hook(void* /*pThis*/,
                                                              bool /*isHover*/) {
    ToolTipScope scope;
}

// Method IsToolTipEnabled -> return false
using IsToolTipEnabled_t = bool(__cdecl*)(void* pThis);

static bool __cdecl IsToolTipEnabled_Hook(void* /*pThis*/) {
    return false;
}

static IsToolTipEnabled_t IsToolTipEnabled_LaunchList_Orig = nullptr;
static IsToolTipEnabled_t IsToolTipEnabled_Augmented_Orig = nullptr;
static IsToolTipEnabled_t IsToolTipEnabled_Recommended_Orig = nullptr;

static std::atomic<bool> g_systemTrayHooked{false};
static std::atomic<bool> g_taskbarViewHooked{false};

static bool HookSystemTraySymbols(HMODULE module) {
    Wh_Log(L"> Hooking SystemTray symbols on %p", module);
    // SystemTray.dll
    WindhawkUtils::SYMBOL_HOOK systemTrayHooks[] = {
        {
            {
                LR"(void __cdecl TaskbarLocationHelpers::PositionTaskbarTooltip(struct winrt::Windows::UI::Xaml::Controls::ToolTip const &,struct winrt::Windows::UI::Xaml::FrameworkElement const &,enum winrt::WindowsUdk::UI::Shell::TaskbarLocation,bool,bool))",
                LR"(void __cdecl TaskbarLocationHelpers::PositionTaskbarTooltip(struct winrt::Windows::UI::Xaml::Controls::ToolTip const &,struct winrt::Windows::UI::Xaml::FrameworkElement const &,enum winrt::WindowsUdk::Shell::TaskbarLocation,bool,bool))",
            },
            &PositionTaskbarTooltip_Original,
            PositionTaskbarTooltip_Hook,
            true,
        },
        {
            {
                LR"(void __cdecl TaskbarLocationHelpers::ApplyTaskbarTooltipPlacement(struct winrt::Windows::UI::Xaml::Controls::ToolTip const &,enum winrt::WindowsUdk::UI::Shell::TaskbarLocation,struct winrt::Windows::Foundation::Size,struct winrt::Windows::Foundation::Size))",
                LR"(void __cdecl TaskbarLocationHelpers::ApplyTaskbarTooltipPlacement(struct winrt::Windows::UI::Xaml::Controls::ToolTip const &,enum winrt::WindowsUdk::Shell::TaskbarLocation,struct winrt::Windows::Foundation::Size,struct winrt::Windows::Foundation::Size))",
            },
            &ApplyTaskbarTooltipPlacement_Tray_Original,
            ApplyTaskbarTooltipPlacement_Tray_Hook,
            true,
        },
        {
            {
                LR"(private: void __cdecl winrt::SystemTray::implementation::IconView::UpdateOuterToolTipPlacement(bool))",
            },
            &IconView_UpdateOuterToolTipPlacement_Original,
            IconView_UpdateOuterToolTipPlacement_Hook,
            true,
        },
    };

    bool res = WindhawkUtils::HookSymbols(module, systemTrayHooks,
                                          ARRAYSIZE(systemTrayHooks));
    Wh_Log(L"> HookSystemTraySymbols result: %d", res ? 1 : 0);
    return res;
}

static bool HookTaskbarViewSymbols(HMODULE module) {
    Wh_Log(L"> Hooking Taskbar.View symbols on %p", module);
    // Taskbar.View.dll
    WindhawkUtils::SYMBOL_HOOK taskbarViewHooks[] = {
        {
            {
                LR"(public: void __cdecl winrt::Taskbar::implementation::ExperienceToggleButton::UpdateToolTipPlacementForHoverState(bool))",
            },
            &ExperienceToggleButton_UpdateHover_Original,
            ExperienceToggleButton_UpdateHover_Hook,
            true,
        },
        {
            {
                LR"(public: void __cdecl winrt::Taskbar::implementation::TaskListButton::UpdateToolTipPlacementForHoverState(void))",
            },
            &TaskListButton_UpdateHover_Original,
            TaskListButton_UpdateHover_Hook,
            true,
        },
        {
            {
                LR"(public: void __cdecl winrt::Taskbar::implementation::OverflowToggleButton::UpdateToolTipPlacementForHoverState(void))",
            },
            &OverflowToggleButton_UpdateHover_Original,
            OverflowToggleButton_UpdateHover_Hook,
            true,
        },
        {
            {
                LR"(public: void __cdecl winrt::Taskbar::implementation::TaskListButton::AddToolTipContent(void))",
            },
            &TaskListButton_AddToolTipContent_Original,
            TaskListButton_AddToolTipContent_Hook,
            true,
        },
        {
            {
                LR"(private: void __cdecl winrt::Taskbar::implementation::TaskItemThumbnailView::UpdateToolTip(void))",
            },
            &TaskItemThumbnailView_UpdateToolTip_Original,
            TaskItemThumbnailView_UpdateToolTip_Hook,
            true,
        },
        {
            {
                LR"(public: virtual bool __cdecl winrt::Taskbar::implementation::LaunchListItemViewModel::IsToolTipEnabled(void)const)",
            },
            &IsToolTipEnabled_LaunchList_Orig,
            IsToolTipEnabled_Hook,
            true,
        },
        {
            {
                LR"(public: virtual bool __cdecl winrt::Taskbar::implementation::AugmentedEntryPointViewModel::IsToolTipEnabled(void)const)",
            },
            &IsToolTipEnabled_Augmented_Orig,
            IsToolTipEnabled_Hook,
            true,
        },
        {
            {
                LR"(public: bool __cdecl winrt::Taskbar::implementation::RecommendedItemViewModel::IsToolTipEnabled(void)const)",
            },
            &IsToolTipEnabled_Recommended_Orig,
            IsToolTipEnabled_Hook,
            true,
        },
        {
            {
                LR"(void __cdecl TaskbarLocationHelpers::ApplyTaskbarTooltipPlacement(struct winrt::Windows::UI::Xaml::Controls::ToolTip const &,enum winrt::WindowsUdk::UI::Shell::TaskbarLocation,struct winrt::Windows::Foundation::Size,struct winrt::Windows::Foundation::Size,bool,bool))",
                LR"(void __cdecl TaskbarLocationHelpers::ApplyTaskbarTooltipPlacement(struct winrt::Windows::UI::Xaml::Controls::ToolTip const &,enum winrt::WindowsUdk::Shell::TaskbarLocation,struct winrt::Windows::Foundation::Size,struct winrt::Windows::Foundation::Size,bool,bool))",
            },
            &ApplyTaskbarTooltipPlacement6_Original,
            ApplyTaskbarTooltipPlacement6_Hook,
            true,
        },
        {
            {
                LR"(void __cdecl TaskbarLocationHelpers::ApplyTaskbarTooltipPlacement(struct winrt::Windows::UI::Xaml::Controls::ToolTip const &,enum winrt::WindowsUdk::UI::Shell::TaskbarLocation,struct winrt::Windows::Foundation::Size,struct winrt::Windows::Foundation::Size))",
                LR"(void __cdecl TaskbarLocationHelpers::ApplyTaskbarTooltipPlacement(struct winrt::Windows::UI::Xaml::Controls::ToolTip const &,enum winrt::WindowsUdk::Shell::TaskbarLocation,struct winrt::Windows::Foundation::Size,struct winrt::Windows::Foundation::Size))",
            },
            &ApplyTaskbarTooltipPlacement_View_Original,
            ApplyTaskbarTooltipPlacement_View_Hook,
            true,
        },
    };

    bool res = WindhawkUtils::HookSymbols(module, taskbarViewHooks,
                                          ARRAYSIZE(taskbarViewHooks));
    Wh_Log(L"> HookTaskbarViewSymbols result: %d", res ? 1 : 0);
    return res;
}

static void HandleLoadedModule(HMODULE module) {
    if (!module) {
        return;
    }

    if (!g_systemTrayHooked && GetModuleHandleW(L"SystemTray.dll") == module) {
        if (!g_systemTrayHooked.exchange(true)) {
            Wh_Log(L"Loaded SystemTray.dll dynamically");
            HookSystemTraySymbols(module);
            Wh_ApplyHookOperations();
        }
    } else if (!g_taskbarViewHooked &&
               GetModuleHandleW(L"Taskbar.View.dll") == module) {
        if (!g_taskbarViewHooked.exchange(true)) {
            Wh_Log(L"Loaded Taskbar.View.dll dynamically");
            HookTaskbarViewSymbols(module);
            Wh_ApplyHookOperations();
        }
    }
}

using LoadLibraryExW_t = decltype(&LoadLibraryExW);
static LoadLibraryExW_t LoadLibraryExW_Original = nullptr;

static HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR lpLibFileName,
                                          HANDLE hFile,
                                          DWORD dwFlags) {
    HMODULE module = LoadLibraryExW_Original(lpLibFileName, hFile, dwFlags);
    if (module) {
        HandleLoadedModule(module);
    }
    return module;
}

BOOL Wh_ModInit() {
    Wh_Log(L"> Initializing Hide Taskbar Tooltips mod v1.0.6");

    if (!IsWindows11OrGreater()) {
        Wh_Log(L"Hide Taskbar Tooltips: Only Windows 11 is supported");
        return FALSE;
    }

    bool hookedAny = false;

    HMODULE user32Module = GetModuleHandleW(L"user32.dll");
    if (user32Module) {
        auto pCreateWindowExW = reinterpret_cast<decltype(&CreateWindowExW)>(
            GetProcAddress(user32Module, "CreateWindowExW"));
        if (pCreateWindowExW) {
            WindhawkUtils::SetFunctionHook(pCreateWindowExW,
                                           CreateWindowExW_Hook,
                                           &CreateWindowExW_Original);
        }

        auto pShowWindow = reinterpret_cast<decltype(&ShowWindow)>(
            GetProcAddress(user32Module, "ShowWindow"));
        if (pShowWindow) {
            WindhawkUtils::SetFunctionHook(pShowWindow, ShowWindow_Hook,
                                           &ShowWindow_Original);
        }

        auto pSetWindowPos = reinterpret_cast<decltype(&SetWindowPos)>(
            GetProcAddress(user32Module, "SetWindowPos"));
        if (pSetWindowPos) {
            WindhawkUtils::SetFunctionHook(pSetWindowPos, SetWindowPos_Hook,
                                           &SetWindowPos_Original);
        }
    }

    InitEagerToolTipHook();

    if (HMODULE systemTrayModule = GetModuleHandleW(L"SystemTray.dll")) {
        g_systemTrayHooked = true;
        if (HookSystemTraySymbols(systemTrayModule)) {
            hookedAny = true;
        }
    }

    if (HMODULE taskbarViewModule = GetModuleHandleW(L"Taskbar.View.dll")) {
        g_taskbarViewHooked = true;
        if (HookTaskbarViewSymbols(taskbarViewModule)) {
            hookedAny = true;
        }
    }

    bool waitingForModules = false;
    if (!g_systemTrayHooked || !g_taskbarViewHooked) {
        HMODULE kernelBaseModule = GetModuleHandleW(L"kernelbase.dll");
        if (kernelBaseModule) {
            auto pKernelBaseLoadLibraryExW =
                reinterpret_cast<decltype(&LoadLibraryExW)>(
                    GetProcAddress(kernelBaseModule, "LoadLibraryExW"));
            if (pKernelBaseLoadLibraryExW) {
                WindhawkUtils::SetFunctionHook(pKernelBaseLoadLibraryExW,
                                               LoadLibraryExW_Hook,
                                               &LoadLibraryExW_Original);
                waitingForModules = true;
            }
        }
    }

    return hookedAny || waitingForModules;
}

void Wh_ModUninit() {
    Wh_Log(L"> Uninitializing Hide Taskbar Tooltips mod");
    std::lock_guard<std::mutex> lock(s_tooltipWindowsMutex);
    s_tooltipWindows.clear();
    s_hasTrackedWindows.store(false, std::memory_order_relaxed);
}
