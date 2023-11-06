// ==WindhawkMod==
// @id              start-menu-all-apps
// @name            Show all apps by default in start menu
// @description     When the Windows 11 start menu is opened, show all apps right away
// @version         1.0.2
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

**Note:** Requires Windhawk 1.3 or newer.

**Note:** Might not work with a portable version of Windhawk. The reason is that
the StartMenuExperienceHost.exe process has limited access to files, and it
might not be able to load the mod.

Before:

![Before screenshot](https://i.imgur.com/2ipCKJn.png)

After (when the start menu is opened):

![After screenshot](https://i.imgur.com/6UVVORa.png)
*/
// ==/WindhawkModReadme==

#include <windhawk_utils.h>

typedef void (WINAPI *ShowAllApps_t)(
    LPVOID pThis
);

typedef void (WINAPI *HideAllApps_t)(
    LPVOID pThis
);

typedef LPVOID (WINAPI *StartInnerFrameConstructor_t)(
    LPVOID pThis,
    LPVOID, // struct winrt::WindowsUdk::UI::StartScreen::Implementation::DockedStartController const &
    LPVOID // struct winrt::Windows::Foundation::IInspectable const &
);

ShowAllApps_t pOriginalShowAllApps;
HideAllApps_t pOriginalHideAllApps;
StartInnerFrameConstructor_t pOriginalStartInnerFrameConstructor;

void WINAPI HideAllAppsHook(
    LPVOID pThis
)
{
    Wh_Log(L">");

    // HideAllApps is called when the start menu is closed to reset the state.
    // The call happens in:
    // winrt::StartMenu::implementation::StartInnerFrame::OnWindowVisibilityChanged
    // What we do is calling ShowAllApps instead.
    pOriginalShowAllApps(pThis);
}

LPVOID WINAPI StartInnerFrameConstructorHook(
    LPVOID pThis,
    LPVOID dockedStartController,
    LPVOID param2
)
{
    Wh_Log(L">");

    LPVOID ret = pOriginalStartInnerFrameConstructor(pThis, dockedStartController, param2);

    // Show all apps on initialization. Prepares the start menu for the first time it's opened.
    pOriginalShowAllApps(dockedStartController);

    return ret;
}

BOOL Wh_ModInit(void)
{
    Wh_Log(L">");

    WCHAR szWindowsDirectory[MAX_PATH];
    if (!GetWindowsDirectory(szWindowsDirectory, ARRAYSIZE(szWindowsDirectory))) {
        Wh_Log(L"GetWindowsDirectory failed");
        return FALSE;
    }

    HMODULE module;

    // Try the path for Windows 11 version 22H2.
    WCHAR szStartmenuDllPath[MAX_PATH];
    wcscpy_s(szStartmenuDllPath, szWindowsDirectory);
    wcscat_s(szStartmenuDllPath, LR"(\SystemApps\MicrosoftWindows.Client.Core_cw5n1h2txyewy\StartMenu.dll)");
    if (GetFileAttributes(szStartmenuDllPath) != INVALID_FILE_ATTRIBUTES) {
        // Try to load dependency DLLs. At process start, if they're not loaded,
        // loading the start menu DLL fails.
        WCHAR szRuntimeDllPath[MAX_PATH];

        wcscpy_s(szRuntimeDllPath, szWindowsDirectory);
        wcscat_s(szRuntimeDllPath, LR"(\SystemApps\MicrosoftWindows.Client.CBS_cw5n1h2txyewy\vcruntime140_app.dll)");
        LoadLibrary(szRuntimeDllPath);

        wcscpy_s(szRuntimeDllPath, szWindowsDirectory);
        wcscat_s(szRuntimeDllPath, LR"(\SystemApps\MicrosoftWindows.Client.CBS_cw5n1h2txyewy\vcruntime140_1_app.dll)");
        LoadLibrary(szRuntimeDllPath);

        wcscpy_s(szRuntimeDllPath, szWindowsDirectory);
        wcscat_s(szRuntimeDllPath, LR"(\SystemApps\MicrosoftWindows.Client.CBS_cw5n1h2txyewy\msvcp140_app.dll)");
        LoadLibrary(szRuntimeDllPath);

        module = LoadLibrary(szStartmenuDllPath);
    }
    else {
        // Try the path for Windows 11 before version 22H2.
        wcscpy_s(szStartmenuDllPath, szWindowsDirectory);
        wcscat_s(szStartmenuDllPath, LR"(\SystemApps\MicrosoftWindows.Client.CBS_cw5n1h2txyewy\StartMenu.dll)");
        module = LoadLibrary(szStartmenuDllPath);
    }

    if (!module) {
        Wh_Log(L"LoadLibrary failed");
        return FALSE;
    }

    WindhawkUtils::SYMBOL_HOOK taskbarHooks[] = {
        {
            {
                LR"(public: void __cdecl winrt::impl::consume_WindowsUdk_UI_StartScreen_Implementation_IDockedStartControllerOverrides<struct winrt::WindowsUdk::UI::StartScreen::Implementation::DockedStartController>::ShowAllApps(void)const )",
                LR"(public: __cdecl winrt::impl::consume_WindowsUdk_UI_StartScreen_Implementation_IDockedStartControllerOverrides<struct winrt::WindowsUdk::UI::StartScreen::Implementation::DockedStartController>::ShowAllApps(void)const )",
            },
            (void**)&pOriginalShowAllApps,
        },
        {
            {
                LR"(public: void __cdecl winrt::impl::consume_WindowsUdk_UI_StartScreen_Implementation_IDockedStartControllerOverrides<struct winrt::WindowsUdk::UI::StartScreen::Implementation::DockedStartController>::HideAllApps(void)const )",
                LR"(public: __cdecl winrt::impl::consume_WindowsUdk_UI_StartScreen_Implementation_IDockedStartControllerOverrides<struct winrt::WindowsUdk::UI::StartScreen::Implementation::DockedStartController>::HideAllApps(void)const )",
            },
            (void**)&pOriginalHideAllApps,
            (void*)HideAllAppsHook,
        },
        {
            {
                LR"(public: __cdecl winrt::StartMenu::implementation::StartInnerFrame::StartInnerFrame(struct winrt::WindowsUdk::UI::StartScreen::Implementation::DockedStartController const &,struct winrt::Windows::Foundation::IInspectable const &))",
            },
            (void**)&pOriginalStartInnerFrameConstructor,
            (void*)StartInnerFrameConstructorHook,
        }
    };

    if (!HookSymbols(module, taskbarHooks, ARRAYSIZE(taskbarHooks))) {
        return FALSE;
    }

    return TRUE;
}
