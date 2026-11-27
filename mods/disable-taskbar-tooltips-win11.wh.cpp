// ==WindhawkMod==
// @id              disable-taskbar-tooltips-win11
// @name            Disable Taskbar Tooltips (Win11)
// @description     Hides the hover tooltips of Windows 11 taskbar buttons (apps, Start, Search, and more), and optionally the system tray
// @version         1.2
// @author          enrkc
// @github          https://github.com/enrkc
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Disable Taskbar Tooltips (Win11)

Hides the tooltip that pops up when you hover over a button on the Windows 11
taskbar, such as "File Explorer" over the File Explorer icon, "Start" over the
Start button. System tray tooltips can optionally be hidden too.

## What's covered

- Pinned and running app buttons
- Start
- Search
- Task View
- Widgets
- Other buttons on the taskbar itself
- Optionally, the system tray: network, volume, battery, clock, app tray
  icons, and the "Show hidden icons" (^) chevron button

System tray tooltips are kept by default. Turn on the *Also hide system tray
tooltips* setting to hide them too.

Tooltips anywhere else in Explorer are not affected.

## How it works

Taskbar tooltips are standard XAML automatic tooltips. When the pointer or
keyboard focus enters an element that has a tooltip, the XAML ToolTipService
schedules it to open. The mod hooks that single entry point,
`DirectUI::ToolTipService::OnOwnerEnterInternal` in `Windows.UI.Xaml.dll`.
If the element (or the nearest identifiable parent) is a `Taskbar.*` XAML
class, or a `SystemTray.*` class while the tray setting is on (this includes
the `SystemTray.ChevronIconView` "Show hidden icons" button), the tooltip is
simply not scheduled. Everything else is passed through unchanged.

The mod makes no persistent changes: no registry edits and no modifications
to the taskbar's XAML tree. Disabling the mod removes the hook and tooltips
return immediately.

## Compatibility

- Requires the new Windows 11 (XAML) taskbar. It does nothing on the classic
  Windows 10 taskbar or ExplorerPatcher; for those, use the *Disable tooltips
  on hover* option of the
  [Disable Taskbar Thumbnails](https://windhawk.net/mods/taskbar-thumbnails)
  mod.
- Tested on Windows 11 25H2 (build 26200) with `Windows.UI.Xaml.dll`
  10.0.26100.8972.
- If a Windows update renames the hooked function, the mod logs a message and
  does nothing; the taskbar keeps working normally.

## First load

On first load, Windhawk downloads the debug symbols (PDB) for
`Windows.UI.Xaml.dll`, about 350 MB. The download happens again whenever a
Windows update changes that DLL. It can take a few minutes, and the result is
cached, so later loads are instant. If the mod loads while Explorer is
starting, the taskbar may appear only after the download finishes on that
first start.

## Comparison with Hide Taskbar Tooltips

[Hide Taskbar Tooltips](https://windhawk.net/mods/hide-taskbar-tooltips)
covers a broader set of shell tooltips using several hooks across
`Taskbar.View.dll`, `SystemTray.dll`, the ToolTip `IsOpen` setter, and the
window creation and positioning APIs. This mod takes a narrower approach: a
single XAML hook, filtered to taskbar and system tray elements, with the
tray part controlled by a setting.

## Credits

Idea, testing, and a lot of complaining about tooltips by enrkc, who
cheerfully admits he couldn't code his way out of a paper bag. Every line of
code was written by Grok Bot.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- hideTrayTooltips: false
  $name: Also hide system tray tooltips
  $description: Network, volume, battery, clock, tray icon and "Show hidden icons" tooltips
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#undef GetCurrentTime

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Media.h>

#include <atomic>
#include <string_view>

namespace {

std::atomic<bool> g_hideTrayTooltips;

std::atomic<bool> g_xamlModuleHooked;

enum class TooltipOwnerKind {
    Other,
    Taskbar,
    SystemTray,
};

// Walks up the visual tree from the tooltip owner and classifies it by the
// first Taskbar.* or SystemTray.* XAML class found.
TooltipOwnerKind ClassifyTooltipOwner(void* ownerAbi) {
    winrt::Windows::Foundation::IInspectable owner;
    winrt::copy_from_abi(owner, ownerAbi);

    auto element = owner.try_as<winrt::Windows::UI::Xaml::DependencyObject>();
    for (int depth = 0; element && depth < 32; depth++) {
        winrt::hstring className = winrt::get_class_name(element);
        std::wstring_view name = className;
        if (name.starts_with(L"SystemTray.")) {
            return TooltipOwnerKind::SystemTray;
        }
        if (name.starts_with(L"Taskbar.")) {
            return TooltipOwnerKind::Taskbar;
        }

        element =
            winrt::Windows::UI::Xaml::Media::VisualTreeHelper::GetParent(
                element);
    }

    return TooltipOwnerKind::Other;
}

// static HRESULT DirectUI::ToolTipService::OnOwnerEnterInternal(
//     IInspectable* pOwner,
//     IInspectable* pSource,
//     AutomaticToolTipInputMode mode)
using ToolTipService_OnOwnerEnterInternal_t = HRESULT(WINAPI*)(void* pOwner,
                                                               void* pSource,
                                                               int mode);
ToolTipService_OnOwnerEnterInternal_t
    ToolTipService_OnOwnerEnterInternal_Original;

// static HRESULT DirectUI::ToolTipService::CancelAutomaticToolTip()
using ToolTipService_CancelAutomaticToolTip_t = HRESULT(WINAPI*)();
ToolTipService_CancelAutomaticToolTip_t ToolTipService_CancelAutomaticToolTip;
HRESULT WINAPI ToolTipService_OnOwnerEnterInternal_Hook(void* pOwner,
                                                        void* pSource,
                                                        int mode) {
    if (pOwner) {
        TooltipOwnerKind kind = TooltipOwnerKind::Other;
        try {
            kind = ClassifyTooltipOwner(pOwner);
        } catch (...) {
            kind = TooltipOwnerKind::Other;
        }

        if (kind == TooltipOwnerKind::Taskbar ||
            (kind == TooltipOwnerKind::SystemTray &&
             g_hideTrayTooltips)) {
            // Don't schedule the automatic tooltip for this element. Like
            // when the pointer leaves an element, close a tooltip that's
            // still open for another element, or cancel one that's pending.
            // The original function would close it before scheduling its own.
            if (ToolTipService_CancelAutomaticToolTip) {
                ToolTipService_CancelAutomaticToolTip();
            }
            return S_OK;
        }
    }

    return ToolTipService_OnOwnerEnterInternal_Original(pOwner, pSource, mode);
}

bool HookXamlModuleSymbols(HMODULE module) {
    // Windows.UI.Xaml.dll
    WindhawkUtils::SYMBOL_HOOK xamlHooks[] = {
        {
            {LR"(private: static long __cdecl DirectUI::ToolTipService::OnOwnerEnterInternal(struct IInspectable *,struct IInspectable *,enum DirectUI::AutomaticToolTipInputMode))"},
            &ToolTipService_OnOwnerEnterInternal_Original,
            ToolTipService_OnOwnerEnterInternal_Hook,
        },
        {
            {LR"(public: static long __cdecl DirectUI::ToolTipService::CancelAutomaticToolTip(void))"},
            &ToolTipService_CancelAutomaticToolTip,
            nullptr,
            true,
        },
    };

    if (!WindhawkUtils::HookSymbols(module, xamlHooks, ARRAYSIZE(xamlHooks))) {
        Wh_Log(L"ToolTipService::OnOwnerEnterInternal not found, this Windows "
               L"version isn't supported");
        return false;
    }

    return true;
}

void HandleLoadedModuleIfXaml(HMODULE module) {
    if (!g_xamlModuleHooked &&
        GetModuleHandleW(L"Windows.UI.Xaml.dll") == module &&
        !g_xamlModuleHooked.exchange(true)) {
        Wh_Log(L"Windows.UI.Xaml.dll loaded");
        if (HookXamlModuleSymbols(module)) {
            Wh_ApplyHookOperations();
        }
    }
}

using LoadLibraryExW_t = decltype(&LoadLibraryExW);
LoadLibraryExW_t LoadLibraryExW_Original;
HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR lpLibFileName,
                                   HANDLE hFile,
                                   DWORD dwFlags) {
    HMODULE module = LoadLibraryExW_Original(lpLibFileName, hFile, dwFlags);
    if (module) {
        HandleLoadedModuleIfXaml(module);
    }

    return module;
}

void LoadSettings() {
    g_hideTrayTooltips = Wh_GetIntSetting(L"hideTrayTooltips");
}

}  // namespace

BOOL Wh_ModInit() {
    Wh_Log(L">");

    LoadSettings();

    if (HMODULE xamlModule = GetModuleHandleW(L"Windows.UI.Xaml.dll")) {
        g_xamlModuleHooked = true;
        // If the symbol isn't found, stay loaded but inactive.
        HookXamlModuleSymbols(xamlModule);
    } else {
        Wh_Log(L"Windows.UI.Xaml.dll not loaded yet");

        HMODULE kernelBaseModule = GetModuleHandleW(L"kernelbase.dll");
        auto pKernelBaseLoadLibraryExW =
            kernelBaseModule
                ? (LoadLibraryExW_t)GetProcAddress(kernelBaseModule,
                                                   "LoadLibraryExW")
                : nullptr;
        if (pKernelBaseLoadLibraryExW) {
            WindhawkUtils::SetFunctionHook(pKernelBaseLoadLibraryExW,
                                           LoadLibraryExW_Hook,
                                           &LoadLibraryExW_Original);
        } else {
            Wh_Log(L"Couldn't find LoadLibraryExW");
        }
    }

    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L">");

    // In case Windows.UI.Xaml.dll was loaded between Wh_ModInit and now.
    if (!g_xamlModuleHooked) {
        if (HMODULE xamlModule = GetModuleHandleW(L"Windows.UI.Xaml.dll")) {
            if (!g_xamlModuleHooked.exchange(true)) {
                if (HookXamlModuleSymbols(xamlModule)) {
                    Wh_ApplyHookOperations();
                }
            }
        }
    }
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");

    LoadSettings();
}

void Wh_ModUninit() {
    Wh_Log(L">");
}
