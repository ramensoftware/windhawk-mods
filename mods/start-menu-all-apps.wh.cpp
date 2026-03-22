// ==WindhawkMod==
// @id              start-menu-all-apps
// @name            Show all apps by default in start menu
// @description     When the Windows 11 start menu is opened, show all apps right away
// @version         1.0.5
// @author          m417z
// @github          https://github.com/m417z
// @twitter         https://twitter.com/m417z
// @homepage        https://m417z.com/
// @include         StartMenuExperienceHost.exe
// @architecture    x86-64
// ==/WindhawkMod==

// Source code is published under The GNU General Public License v3.0.
//
// For bug reports and feature requests, please open an issue here:
// https://github.com/ramensoftware/windhawk-mods/issues
//
// For pull requests, development takes place here:
// https://github.com/m417z/my-windhawk-mods

// ==WindhawkModReadme==
/*
# Show all apps by default in start menu

When the Windows 11 start menu is opened, show all apps right away without
having to click on the "All apps" button.

Before:

![Before screenshot](https://i.imgur.com/2ipCKJn.png)

After (when the start menu is opened):

![After screenshot](https://i.imgur.com/6UVVORa.png)
*/
// ==/WindhawkModReadme==

#include <windhawk_utils.h>

#include <atomic>

std::atomic<bool> g_startMenuDllLoaded;

bool g_inStartInnerFrameConstructor;
void* g_IDockedStartControllerOverrides_as_pThis;

using ShowAllApps_t = void(WINAPI*)(void* pThis);
ShowAllApps_t ShowAllApps_Original;

using HideAllApps_t = void(WINAPI*)(void* pThis);
HideAllApps_t HideAllApps_Original;
void WINAPI HideAllApps_Hook(void* pThis) {
    Wh_Log(L">");

    // HideAllApps is called when the start menu is closed to reset the state.
    // The call happens in:
    // winrt::StartMenu::implementation::StartInnerFrame::OnWindowVisibilityChanged
    // What we do is calling ShowAllApps instead.
    ShowAllApps_Original(pThis);
}

using StartInnerFrameConstructor_t = void*(WINAPI*)(void* pThis,
                                                    void* dockedStartController,
                                                    void* param2);
StartInnerFrameConstructor_t StartInnerFrameConstructor_Original;
void* WINAPI StartInnerFrameConstructor_Hook(void* pThis,
                                             void* dockedStartController,
                                             void* param2) {
    Wh_Log(L">");

    g_inStartInnerFrameConstructor = true;

    void* ret = StartInnerFrameConstructor_Original(
        pThis, dockedStartController, param2);

    g_inStartInnerFrameConstructor = false;

    void* controllerOverrides = g_IDockedStartControllerOverrides_as_pThis;
    if (!controllerOverrides) {
        // In older versions, IDockedStartControllerOverrides_as doesn't exist,
        // and the param is the right variable.
        controllerOverrides = dockedStartController;
    }

    // Show all apps on initialization. Prepares the start menu for the first
    // time it's opened.
    ShowAllApps_Original(controllerOverrides);

    return ret;
}

using IDockedStartControllerOverrides_as_t = void*(WINAPI*)(void* pThis,
                                                            void* param1);
IDockedStartControllerOverrides_as_t
    IDockedStartControllerOverrides_as_Original;
void* WINAPI IDockedStartControllerOverrides_as_Hook(void* pThis,
                                                     void* param1) {
    Wh_Log(L">");

    if (g_inStartInnerFrameConstructor &&
        !g_IDockedStartControllerOverrides_as_pThis) {
        g_IDockedStartControllerOverrides_as_pThis = pThis;
    }

    void* ret = IDockedStartControllerOverrides_as_Original(pThis, param1);

    return ret;
}

bool HookStartMenuDllSymbols(HMODULE module) {
    WindhawkUtils::SYMBOL_HOOK startMenuDllHooks[] = {
        {
            {
                // First seen in StartMenu.dll version 2124.33803.0.0.
                LR"(public: __cdecl winrt::impl::consume_WindowsUdk_UI_StartScreen_Implementation_IDockedStartControllerOverrides<struct winrt::WindowsUdk::UI::StartScreen::Implementation::IDockedStartControllerOverrides>::ShowAllApps(void)const )",
                // Older symbol.
                LR"(public: __cdecl winrt::impl::consume_WindowsUdk_UI_StartScreen_Implementation_IDockedStartControllerOverrides<struct winrt::WindowsUdk::UI::StartScreen::Implementation::DockedStartController>::ShowAllApps(void)const )",
                // Even older symbol.
                LR"(public: void __cdecl winrt::impl::consume_WindowsUdk_UI_StartScreen_Implementation_IDockedStartControllerOverrides<struct winrt::WindowsUdk::UI::StartScreen::Implementation::DockedStartController>::ShowAllApps(void)const )",
            },
            &ShowAllApps_Original,
        },
        {
            {
                // First seen in StartMenu.dll version 2124.33803.0.0.
                LR"(public: __cdecl winrt::impl::consume_WindowsUdk_UI_StartScreen_Implementation_IDockedStartControllerOverrides<struct winrt::WindowsUdk::UI::StartScreen::Implementation::IDockedStartControllerOverrides>::HideAllApps(void)const )",
                // Older symbol.
                LR"(public: __cdecl winrt::impl::consume_WindowsUdk_UI_StartScreen_Implementation_IDockedStartControllerOverrides<struct winrt::WindowsUdk::UI::StartScreen::Implementation::DockedStartController>::HideAllApps(void)const )",
                // Even older symbol.
                LR"(public: void __cdecl winrt::impl::consume_WindowsUdk_UI_StartScreen_Implementation_IDockedStartControllerOverrides<struct winrt::WindowsUdk::UI::StartScreen::Implementation::DockedStartController>::HideAllApps(void)const )",
            },
            &HideAllApps_Original,
            HideAllApps_Hook,
        },
        {
            {LR"(public: __cdecl winrt::StartMenu::implementation::StartInnerFrame::StartInnerFrame(struct winrt::WindowsUdk::UI::StartScreen::Implementation::DockedStartController const &,struct winrt::Windows::Foundation::IInspectable const &))"},
            &StartInnerFrameConstructor_Original,
            StartInnerFrameConstructor_Hook,
        },
        {
            {LR"(struct winrt::WindowsUdk::UI::StartScreen::Implementation::IDockedStartControllerOverrides __cdecl winrt::impl::as<struct winrt::WindowsUdk::UI::StartScreen::Implementation::IDockedStartControllerOverrides,struct winrt::impl::abi<struct winrt::Windows::Foundation::IUnknown,void>::type,0>(struct winrt::impl::abi<struct winrt::Windows::Foundation::IUnknown,void>::type *))"},
            &IDockedStartControllerOverrides_as_Original,
            IDockedStartControllerOverrides_as_Hook,
            true,  // Added in KB5055627.
        },
    };

    if (!HookSymbols(module, startMenuDllHooks, ARRAYSIZE(startMenuDllHooks))) {
        Wh_Log(L"HookSymbols failed");
        return false;
    }

    return true;
}

HMODULE GetStartMenuModuleHandle() {
    return GetModuleHandle(L"StartMenu.dll");
}

void HandleLoadedModuleIfStartMenu(HMODULE module, LPCWSTR lpLibFileName) {
    if (!g_startMenuDllLoaded && GetStartMenuModuleHandle() == module &&
        !g_startMenuDllLoaded.exchange(true)) {
        Wh_Log(L"Loaded %s", lpLibFileName);

        if (HookStartMenuDllSymbols(module)) {
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
        HandleLoadedModuleIfStartMenu(module, lpLibFileName);
    }

    return module;
}

BOOL Wh_ModInit() {
    Wh_Log(L">");

    if (HMODULE startMenuModule = GetStartMenuModuleHandle()) {
        g_startMenuDllLoaded = true;
        if (!HookStartMenuDllSymbols(startMenuModule)) {
            return FALSE;
        }
    } else {
        Wh_Log(L"StartMenu.dll not loaded yet");
    }

    HMODULE kernelBaseModule = GetModuleHandle(L"kernelbase.dll");
    auto pKernelBaseLoadLibraryExW = (decltype(&LoadLibraryExW))GetProcAddress(
        kernelBaseModule, "LoadLibraryExW");
    WindhawkUtils::Wh_SetFunctionHookT(pKernelBaseLoadLibraryExW,
                                       LoadLibraryExW_Hook,
                                       &LoadLibraryExW_Original);

    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L">");

    if (!g_startMenuDllLoaded) {
        if (HMODULE startMenuModule = GetStartMenuModuleHandle()) {
            if (!g_startMenuDllLoaded.exchange(true)) {
                Wh_Log(L"Got StartMenu.dll");

                if (HookStartMenuDllSymbols(startMenuModule)) {
                    Wh_ApplyHookOperations();
                }
            }
        }
    }
}

void Wh_ModUninit() {
    Wh_Log(L">");
}
