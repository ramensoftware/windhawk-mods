// ==WindhawkMod==
// @id              hide-taskbar-tooltips
// @name            Hide Taskbar Tooltips
// @description     Suppresses native Windows 11 XAML hover tooltips in Explorer (taskbar buttons, system tray icons, and clock).
// @version         1.0.4
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
}

using TaskListButton_UpdateHover_t = void(__cdecl*)(void* pThis);
static TaskListButton_UpdateHover_t TaskListButton_UpdateHover_Original =
    nullptr;

static void __cdecl TaskListButton_UpdateHover_Hook(void* /*pThis*/) {
}

using OverflowToggleButton_UpdateHover_t = void(__cdecl*)(void* pThis);
static OverflowToggleButton_UpdateHover_t
    OverflowToggleButton_UpdateHover_Original = nullptr;

static void __cdecl OverflowToggleButton_UpdateHover_Hook(void* /*pThis*/) {
}

using TaskListButton_AddToolTipContent_t = void(__cdecl*)(void* pThis);
static TaskListButton_AddToolTipContent_t
    TaskListButton_AddToolTipContent_Original = nullptr;

static void __cdecl TaskListButton_AddToolTipContent_Hook(void* /*pThis*/) {
}

// Hover state hook in SystemTray.dll (zero-cost no-op)
using IconView_UpdateOuterToolTipPlacement_t = void(__cdecl*)(void* pThis,
                                                              bool isHover);
static IconView_UpdateOuterToolTipPlacement_t
    IconView_UpdateOuterToolTipPlacement_Original = nullptr;

static void __cdecl IconView_UpdateOuterToolTipPlacement_Hook(void* /*pThis*/,
                                                              bool /*isHover*/) {
}

// WinRT Property get_IsToolTipEnabled -> return false
using get_IsToolTipEnabled_t = int(__cdecl*)(void* pThis, bool* pValue);

static int __cdecl get_IsToolTipEnabled_Hook(void* /*pThis*/, bool* pValue) {
    if (pValue) {
        *pValue = false;
    }
    return 0;
}

static get_IsToolTipEnabled_t get_IsToolTipEnabled_LaunchList_Orig = nullptr;
static get_IsToolTipEnabled_t get_IsToolTipEnabled_TaskWindow_Orig = nullptr;
static get_IsToolTipEnabled_t get_IsToolTipEnabled_TaskGroup_Orig = nullptr;
static get_IsToolTipEnabled_t get_IsToolTipEnabled_Overflow_Orig = nullptr;
static get_IsToolTipEnabled_t get_IsToolTipEnabled_Recommended_Orig = nullptr;

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
                LR"(?PositionTaskbarTooltip@TaskbarLocationHelpers@@YAXAEBUToolTip@Controls@Xaml@UI@Windows@winrt@@AEBUFrameworkElement@4567@W4TaskbarLocation@Shell@5WindowsUdk@7@_N3@Z)",
                LR"(void __cdecl TaskbarLocationHelpers::PositionTaskbarTooltip(struct winrt::Windows::UI::Xaml::Controls::ToolTip const &,struct winrt::Windows::UI::Xaml::FrameworkElement const &,enum winrt::WindowsUdk::UI::Shell::TaskbarLocation,bool,bool))",
                LR"(void __cdecl TaskbarLocationHelpers::PositionTaskbarTooltip(struct winrt::Windows::UI::Xaml::Controls::ToolTip const &,struct winrt::Windows::UI::Xaml::FrameworkElement const &,enum winrt::WindowsUdk::Shell::TaskbarLocation,bool,bool))",
            },
            &PositionTaskbarTooltip_Original,
            PositionTaskbarTooltip_Hook,
            true,
        },
        {
            {
                LR"(?ApplyTaskbarTooltipPlacement@TaskbarLocationHelpers@@YAXAEBUToolTip@Controls@Xaml@UI@Windows@winrt@@W4TaskbarLocation@Shell@5WindowsUdk@7@USize@Foundation@67@2@Z)",
                LR"(void __cdecl TaskbarLocationHelpers::ApplyTaskbarTooltipPlacement(struct winrt::Windows::UI::Xaml::Controls::ToolTip const &,enum winrt::WindowsUdk::UI::Shell::TaskbarLocation,struct winrt::Windows::Foundation::Size,struct winrt::Windows::Foundation::Size))",
                LR"(void __cdecl TaskbarLocationHelpers::ApplyTaskbarTooltipPlacement(struct winrt::Windows::UI::Xaml::Controls::ToolTip const &,enum winrt::WindowsUdk::Shell::TaskbarLocation,struct winrt::Windows::Foundation::Size,struct winrt::Windows::Foundation::Size))",
            },
            &ApplyTaskbarTooltipPlacement_Tray_Original,
            ApplyTaskbarTooltipPlacement_Tray_Hook,
            true,
        },
        {
            {
                LR"(?UpdateOuterToolTipPlacement@IconView@implementation@SystemTray@winrt@@AEAAX_N@Z)",
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
                LR"(?UpdateToolTipPlacementForHoverState@ExperienceToggleButton@implementation@Taskbar@winrt@@QEAAX_N@Z)",
                LR"(public: void __cdecl winrt::Taskbar::implementation::ExperienceToggleButton::UpdateToolTipPlacementForHoverState(bool))",
            },
            &ExperienceToggleButton_UpdateHover_Original,
            ExperienceToggleButton_UpdateHover_Hook,
            true,
        },
        {
            {
                LR"(?UpdateToolTipPlacementForHoverState@TaskListButton@implementation@Taskbar@winrt@@QEAAXXZ)",
                LR"(public: void __cdecl winrt::Taskbar::implementation::TaskListButton::UpdateToolTipPlacementForHoverState(void))",
            },
            &TaskListButton_UpdateHover_Original,
            TaskListButton_UpdateHover_Hook,
            true,
        },
        {
            {
                LR"(?UpdateToolTipPlacementForHoverState@OverflowToggleButton@implementation@Taskbar@winrt@@QEAAXXZ)",
                LR"(public: void __cdecl winrt::Taskbar::implementation::OverflowToggleButton::UpdateToolTipPlacementForHoverState(void))",
            },
            &OverflowToggleButton_UpdateHover_Original,
            OverflowToggleButton_UpdateHover_Hook,
            true,
        },
        {
            {
                LR"(?AddToolTipContent@TaskListButton@implementation@Taskbar@winrt@@QEAAXXZ)",
                LR"(public: void __cdecl winrt::Taskbar::implementation::TaskListButton::AddToolTipContent(void))",
            },
            &TaskListButton_AddToolTipContent_Original,
            TaskListButton_AddToolTipContent_Hook,
            true,
        },
        {
            {
                LR"(?get_IsToolTipEnabled@?$produce@ULaunchListItemViewModel@implementation@Taskbar@winrt@@UILaunchListItemViewModel@34@@impl@winrt@@UEAAHPEA_N@Z)",
            },
            &get_IsToolTipEnabled_LaunchList_Orig,
            get_IsToolTipEnabled_Hook,
            true,
        },
        {
            {
                LR"(?get_IsToolTipEnabled@?$produce@UTaskListWindowViewModel@implementation@Taskbar@winrt@@UITaskbarAppItemViewModel@34@@impl@winrt@@UEAAHPEA_N@Z)",
            },
            &get_IsToolTipEnabled_TaskWindow_Orig,
            get_IsToolTipEnabled_Hook,
            true,
        },
        {
            {
                LR"(?get_IsToolTipEnabled@?$produce@UTaskListGroupViewModel@implementation@Taskbar@winrt@@UITaskbarAppItemViewModel@34@@impl@winrt@@UEAAHPEA_N@Z)",
            },
            &get_IsToolTipEnabled_TaskGroup_Orig,
            get_IsToolTipEnabled_Hook,
            true,
        },
        {
            {
                LR"(?get_IsToolTipEnabled@?$produce@UOverflowItemViewModel@implementation@Taskbar@winrt@@UIOverflowItemViewModel@34@@impl@winrt@@UEAAHPEA_N@Z)",
            },
            &get_IsToolTipEnabled_Overflow_Orig,
            get_IsToolTipEnabled_Hook,
            true,
        },
        {
            {
                LR"(?get_IsToolTipEnabled@?$produce@URecommendedItemViewModel@implementation@Taskbar@winrt@@UITaskbarAppItemViewModel@34@@impl@winrt@@UEAAHPEA_N@Z)",
            },
            &get_IsToolTipEnabled_Recommended_Orig,
            get_IsToolTipEnabled_Hook,
            true,
        },
        {
            {
                LR"(?IsToolTipEnabled@LaunchListItemViewModel@implementation@Taskbar@winrt@@UEBA_NXZ)",
                LR"(public: virtual bool __cdecl winrt::Taskbar::implementation::LaunchListItemViewModel::IsToolTipEnabled(void)const)",
            },
            &IsToolTipEnabled_LaunchList_Orig,
            IsToolTipEnabled_Hook,
            true,
        },
        {
            {
                LR"(?IsToolTipEnabled@AugmentedEntryPointViewModel@implementation@Taskbar@winrt@@UEBA_NXZ)",
                LR"(public: virtual bool __cdecl winrt::Taskbar::implementation::AugmentedEntryPointViewModel::IsToolTipEnabled(void)const)",
            },
            &IsToolTipEnabled_Augmented_Orig,
            IsToolTipEnabled_Hook,
            true,
        },
        {
            {
                LR"(?IsToolTipEnabled@RecommendedItemViewModel@implementation@Taskbar@winrt@@QEBA_NXZ)",
                LR"(public: bool __cdecl winrt::Taskbar::implementation::RecommendedItemViewModel::IsToolTipEnabled(void)const)",
            },
            &IsToolTipEnabled_Recommended_Orig,
            IsToolTipEnabled_Hook,
            true,
        },
        {
            {
                LR"(?ApplyTaskbarTooltipPlacement@TaskbarLocationHelpers@@YAXAEBUToolTip@Controls@Xaml@UI@Windows@winrt@@W4TaskbarLocation@Shell@5WindowsUdk@7@USize@Foundation@67@2_N3@Z)",
                LR"(void __cdecl TaskbarLocationHelpers::ApplyTaskbarTooltipPlacement(struct winrt::Windows::UI::Xaml::Controls::ToolTip const &,enum winrt::WindowsUdk::UI::Shell::TaskbarLocation,struct winrt::Windows::Foundation::Size,struct winrt::Windows::Foundation::Size,bool,bool))",
                LR"(void __cdecl TaskbarLocationHelpers::ApplyTaskbarTooltipPlacement(struct winrt::Windows::UI::Xaml::Controls::ToolTip const &,enum winrt::WindowsUdk::Shell::TaskbarLocation,struct winrt::Windows::Foundation::Size,struct winrt::Windows::Foundation::Size,bool,bool))",
            },
            &ApplyTaskbarTooltipPlacement6_Original,
            ApplyTaskbarTooltipPlacement6_Hook,
            true,
        },
        {
            {
                LR"(?ApplyTaskbarTooltipPlacement@TaskbarLocationHelpers@@YAXAEBUToolTip@Controls@Xaml@UI@Windows@winrt@@W4TaskbarLocation@Shell@5WindowsUdk@7@USize@Foundation@67@2@Z)",
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
    Wh_Log(L"> Initializing Hide Taskbar Tooltips mod v1.0.3");

    if (!IsWindows11OrGreater()) {
        Wh_Log(L"Hide Taskbar Tooltips: Only Windows 11 is supported");
        return FALSE;
    }

    bool hookedAny = false;

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
}
