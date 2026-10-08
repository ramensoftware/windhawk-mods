// ==WindhawkMod==
// @id              hide-windows-11-clock-tooltip
// @name            Hide Windows 11 Clock Tooltip
// @description     Completely disables the Windows 11 taskbar clock hover tooltip while keeping the clock functional.
// @version         1.0.6
// @author          ViiT4liTy
// @github          https://github.com/ViiT4liTy
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -loleaut32 -lruntimeobject
// ==/WindhawkMod==
//
// Standalone Windows 11 taskbar clock tooltip suppression mod.
// The clock tooltip text is cleared and the Windows 11 clock element is made
// non-hit-testable, reproducing the Taskbar Styler setting:
//   Target: SystemTray.DateTimeIconContent
//   Style:  IsHitTestVisible=False
//
// No additional Windhawk mod is required.

#include <windhawk_utils.h>
#include <winstring.h>

#ifdef GetCurrentTime
#undef GetCurrentTime
#endif

#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Controls.h>

using namespace winrt::Windows::UI::Xaml;
namespace Controls = winrt::Windows::UI::Xaml::Controls;

using ClockToolTip_t =
    LPVOID(WINAPI*)(LPVOID, LPVOID, LPVOID, LPVOID, LPVOID);

ClockToolTip_t ClockSystemTrayIconDataModel_GetTimeToolTipString_Original;
ClockToolTip_t ClockSystemTrayIconDataModel2_GetTimeToolTipString_Original;
ClockToolTip_t ClockSystemTrayIconDataModel_GetTimeToolTipString2_Original;
ClockToolTip_t ClockSystemTrayIconDataModel2_GetTimeToolTipString2_Original;
ClockToolTip_t ClockSystemTrayIconDataModel_GetTimeToolTipString_2_Original;

using LoadLibraryExW_t = decltype(&LoadLibraryExW);
LoadLibraryExW_t LoadLibraryExW_Original;

using DateTimeIconContent_OnApplyTemplate_t = HRESULT(WINAPI*)(LPVOID);
DateTimeIconContent_OnApplyTemplate_t
    DateTimeIconContent_OnApplyTemplate_Original;

static void ClearReturnedHString(LPVOID ret) {
    if (!ret) {
        return;
    }

    HSTRING* result = reinterpret_cast<HSTRING*>(ret);

    if (*result) {
        WindowsDeleteString(*result);
        *result = nullptr;
    }

    WindowsCreateString(L"", 0, result);
}

using BadgeIconContent_get_ViewModel_t = HRESULT(WINAPI*)(LPVOID pThis, LPVOID pArgs);
BadgeIconContent_get_ViewModel_t BadgeIconContent_get_ViewModel_Original;

static void DisableClockHitTesting(LPVOID pThis) {
    if (!pThis) {
        return;
    }

    try {
        FrameworkElement clockElement{nullptr};

        winrt::check_hresult(
            reinterpret_cast<IUnknown*>(pThis)->QueryInterface(
                winrt::guid_of<FrameworkElement>(),
                winrt::put_abi(clockElement)));

        if (!clockElement) {
            return;
        }

        // Equivalent to Taskbar Styler:
        // Target: SystemTray.DateTimeIconContent
        // Style:  IsHitTestVisible=False
        clockElement.IsHitTestVisible(false);

        // Also remove any ToolTip attached directly to the clock element.
        // This prevents an empty tooltip host from remaining visible.
        Controls::ToolTipService::SetToolTip(
            clockElement,
            winrt::Windows::Foundation::IInspectable{nullptr});
    } catch (...) {
        // Do not interfere with the normal taskbar clock if XAML access fails.
    }
}

LPVOID WINAPI ClockSystemTrayIconDataModel_GetTimeToolTipString_Hook(
    LPVOID pThis, LPVOID p1, LPVOID p2, LPVOID p3, LPVOID p4) {
    LPVOID ret =
        ClockSystemTrayIconDataModel_GetTimeToolTipString_Original(
            pThis, p1, p2, p3, p4);
    ClearReturnedHString(ret);
    return ret;
}

LPVOID WINAPI ClockSystemTrayIconDataModel2_GetTimeToolTipString_Hook(
    LPVOID pThis, LPVOID p1, LPVOID p2, LPVOID p3, LPVOID p4) {
    LPVOID ret =
        ClockSystemTrayIconDataModel2_GetTimeToolTipString_Original(
            pThis, p1, p2, p3, p4);
    ClearReturnedHString(ret);
    return ret;
}

LPVOID WINAPI ClockSystemTrayIconDataModel_GetTimeToolTipString2_Hook(
    LPVOID pThis, LPVOID p1, LPVOID p2, LPVOID p3, LPVOID p4) {
    LPVOID ret =
        ClockSystemTrayIconDataModel_GetTimeToolTipString2_Original(
            pThis, p1, p2, p3, p4);
    ClearReturnedHString(ret);
    return ret;
}

LPVOID WINAPI ClockSystemTrayIconDataModel2_GetTimeToolTipString2_Hook(
    LPVOID pThis, LPVOID p1, LPVOID p2, LPVOID p3, LPVOID p4) {
    LPVOID ret =
        ClockSystemTrayIconDataModel2_GetTimeToolTipString2_Original(
            pThis, p1, p2, p3, p4);
    ClearReturnedHString(ret);
    return ret;
}

LPVOID WINAPI ClockSystemTrayIconDataModel_GetTimeToolTipString_2_Hook(
    LPVOID pThis, LPVOID p1, LPVOID p2, LPVOID p3, LPVOID p4) {
    LPVOID ret =
        ClockSystemTrayIconDataModel_GetTimeToolTipString_2_Original(
            pThis, p1, p2, p3, p4);
    ClearReturnedHString(ret);
    return ret;
}

HRESULT WINAPI BadgeIconContent_get_ViewModel_Hook(LPVOID pThis, LPVOID pArgs) {
    HRESULT ret = BadgeIconContent_get_ViewModel_Original(pThis, pArgs);

    // The working Taskbar Styler rule targets the actual
    // SystemTray.DateTimeIconContent object. Apply the same property to that
    // object after it has been created/loaded.
    try {
        winrt::Windows::Foundation::IInspectable obj{nullptr};
        winrt::check_hresult(
            reinterpret_cast<IUnknown*>(pThis)->QueryInterface(
                winrt::guid_of<winrt::Windows::Foundation::IInspectable>(),
                winrt::put_abi(obj)));

        if (obj && winrt::get_class_name(obj) == L"SystemTray.DateTimeIconContent") {
            auto clockElement = obj.as<FrameworkElement>();
            if (clockElement.IsLoaded()) {
                clockElement.IsHitTestVisible(false);
                Controls::ToolTipService::SetToolTip(
                    clockElement,
                    winrt::Windows::Foundation::IInspectable{nullptr});
            }
        }
    } catch (...) {
    }

    return ret;
}

HRESULT WINAPI DateTimeIconContent_OnApplyTemplate_Hook(LPVOID pThis) {
    HRESULT ret = DateTimeIconContent_OnApplyTemplate_Original(pThis);

    // OnApplyTemplate can run again when the taskbar recreates its XAML
    // template, so apply the setting every time the template is rebuilt.
    DisableClockHitTesting(pThis);

    return ret;
}

bool HookSystemTraySymbols(HMODULE module) {
    if (!module) {
        return false;
    }
    // SystemTray.dll, Taskbar.View.dll, ExplorerExtensions.dll
    WindhawkUtils::SYMBOL_HOOK systemTrayDllHooks[] = {
        {
            {LR"(private: struct winrt::hstring __cdecl winrt::SystemTray::implementation::ClockSystemTrayIconDataModel::GetTimeToolTipString(struct _SYSTEMTIME const &,struct _SYSTEMTIME const &,class SystemTrayTelemetry::ClockUpdate &))"},
            &ClockSystemTrayIconDataModel_GetTimeToolTipString_Original,
            ClockSystemTrayIconDataModel_GetTimeToolTipString_Hook,
            true,
        },
        {
            {LR"(private: struct winrt::hstring __cdecl winrt::SystemTray::implementation::ClockSystemTrayIconDataModel2::GetTimeToolTipString(struct _SYSTEMTIME const &,struct _SYSTEMTIME const &,class SystemTrayTelemetry::ClockUpdate &))"},
            &ClockSystemTrayIconDataModel2_GetTimeToolTipString_Original,
            ClockSystemTrayIconDataModel2_GetTimeToolTipString_Hook,
            true,
        },
        {
            {LR"(private: struct winrt::hstring __cdecl winrt::SystemTray::implementation::ClockSystemTrayIconDataModel::GetTimeToolTipString2(struct _SYSTEMTIME const &,struct _SYSTEMTIME const &,class SystemTrayTelemetry::ClockUpdate &))"},
            &ClockSystemTrayIconDataModel_GetTimeToolTipString2_Original,
            ClockSystemTrayIconDataModel_GetTimeToolTipString2_Hook,
            true,
        },
        {
            {LR"(private: struct winrt::hstring __cdecl winrt::SystemTray::implementation::ClockSystemTrayIconDataModel2::GetTimeToolTipString2(struct _SYSTEMTIME const &,struct _SYSTEMTIME const &,class SystemTrayTelemetry::ClockUpdate &))"},
            &ClockSystemTrayIconDataModel2_GetTimeToolTipString2_Original,
            ClockSystemTrayIconDataModel2_GetTimeToolTipString2_Hook,
            true,
        },
        {
            {LR"(private: struct winrt::hstring __cdecl winrt::SystemTray::implementation::ClockSystemTrayIconDataModel::GetTimeToolTipString(struct _SYSTEMTIME *,struct _TIME_DYNAMIC_ZONE_INFORMATION *,class SystemTrayTelemetry::ClockUpdate &))"},
            &ClockSystemTrayIconDataModel_GetTimeToolTipString_2_Original,
            ClockSystemTrayIconDataModel_GetTimeToolTipString_2_Hook,
            true,
        },
        {
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::SystemTray::implementation::BadgeIconContent,struct winrt::SystemTray::IBadgeIconContent>::get_ViewModel(void * *))"},
            &BadgeIconContent_get_ViewModel_Original,
            BadgeIconContent_get_ViewModel_Hook,
            true,
        },
        {
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::SystemTray::implementation::DateTimeIconContent,struct winrt::Windows::UI::Xaml::IFrameworkElementOverrides>::OnApplyTemplate(void))"},
            &DateTimeIconContent_OnApplyTemplate_Original,
            DateTimeIconContent_OnApplyTemplate_Hook,
            true,
        },
    };

    return WindhawkUtils::HookSymbols(
        module, systemTrayDllHooks, ARRAYSIZE(systemTrayDllHooks));
}

HMODULE GetSystemTrayModuleHandle() {
    HMODULE module = GetModuleHandle(L"SystemTray.dll");

    if (!module) {
        module = GetModuleHandle(L"Taskbar.View.dll");
    }

    if (!module) {
        module = GetModuleHandle(L"ExplorerExtensions.dll");
    }

    return module;
}

void TryHookSystemTray() {
    HMODULE module = GetSystemTrayModuleHandle();

    if (module) {
        if (HookSystemTraySymbols(module)) {
            Wh_ApplyHookOperations();
            Wh_Log(L"Clock tooltip and hit-testing hooks installed");
        }
    }
}

HMODULE WINAPI LoadLibraryExW_Hook(
    LPCWSTR lpLibFileName, HANDLE hFile, DWORD dwFlags) {
    HMODULE module =
        LoadLibraryExW_Original(lpLibFileName, hFile, dwFlags);

    if (module && lpLibFileName) {
        PCWSTR fileName = wcsrchr(lpLibFileName, L'\\');
        fileName = fileName ? fileName + 1 : lpLibFileName;

        if (_wcsicmp(fileName, L"SystemTray.dll") == 0 ||
            _wcsicmp(fileName, L"Taskbar.View.dll") == 0 ||
            _wcsicmp(fileName, L"ExplorerExtensions.dll") == 0) {
            TryHookSystemTray();
        }
    }

    return module;
}

BOOL Wh_ModInit() {
    Wh_Log(L"Initializing Hide Windows 11 Clock Tooltip 1.0.6");

    TryHookSystemTray();

    HMODULE kernelBase = GetModuleHandle(L"kernelbase.dll");

    if (kernelBase) {
        auto pLoadLibraryExW =
            reinterpret_cast<LoadLibraryExW_t>(
                GetProcAddress(kernelBase, "LoadLibraryExW"));

        if (pLoadLibraryExW) {
            WindhawkUtils::SetFunctionHook(
                pLoadLibraryExW,
                LoadLibraryExW_Hook,
                &LoadLibraryExW_Original);
        }
    }

    return TRUE;
}

// ==WindhawkModReadme==
/*
# Hide Windows 11 Clock Tooltip

Disables the native tooltip that appears when hovering over the Windows 11 taskbar clock, including the empty or black tooltip popup, while keeping the clock visible and functional.

## Features

- Removes the native clock tooltip text and its visual popup.
- Keeps the taskbar clock visible and functional.
- Works independently without requiring Windows 11 Taskbar Styler or other mods.
- Does not change the date, time, or clock formatting.
- Can be used alongside Taskbar Clock Customization.

## Compatibility

Designed for Windows 11 (x86-64) and targets `explorer.exe`. Windows updates may change internal taskbar components and require updates to this mod.

## License

MIT

---

# Ocultar Tooltip do Relógio do Windows 11

Desativa o tooltip nativo que aparece ao passar o mouse sobre o relógio da barra de tarefas do Windows 11, incluindo a janela vazia ou preta, sem ocultar ou desativar o relógio.

## Funcionalidades

- Remove o texto do tooltip nativo e sua janela visual.
- Mantém o relógio da barra de tarefas visível e funcional.
- Funciona de forma independente, sem exigir o Windows 11 Taskbar Styler ou outros mods.
- Não altera a data, a hora ou a formatação do relógio.
- Pode ser usado junto com o Taskbar Clock Customization.

## Compatibilidade

Desenvolvido para Windows 11 (x86-64), com atuação no processo `explorer.exe`. Atualizações do Windows podem alterar componentes internos da barra de tarefas e exigir atualizações deste mod.

## Licença

MIT
*/
// ==/WindhawkModReadme==
