// ==WindhawkMod==
// @id              start-menu-all-apps
// @name            Show all apps by default in start menu
// @description     When the Windows 11 start menu is opened, show all apps right away
// @version         1.0
// @author          m417z
// @github          https://github.com/m417z
// @twitter         https://twitter.com/m417z
// @homepage        https://m417z.com/
// @include         StartMenuExperienceHost.exe
// @architecture    x86-64
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Show all apps by default in start menu

When the Windows 11 start menu is opened, show all apps right away without
having to click on the "All apps" button.

Note: Might not work on Windhawk 0.9 or older due to a bug. If the mod doesn't
work, please update Windhawk.

Note: Might not work with a portable version of Windhawk. The reason is that the
StartMenuExperienceHost.exe process has limited access to files, and it might
not be able to load the mod.

Before:

![Before screenshot](https://i.imgur.com/2ipCKJn.png)

After (when the start menu is opened):

![After screenshot](https://i.imgur.com/6UVVORa.png)

Feedback and pull requests can be submitted
[here](https://github.com/m417z/my-windhawk-mods).
*/
// ==/WindhawkModReadme==

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

struct SYMBOLHOOKS {
    PCWSTR symbolName;
    void* hookFunction;
    void** pOriginalFunction;
};

BOOL Wh_ModInit(void)
{
    Wh_Log(L">");

    WH_FIND_SYMBOL symbol;
    HANDLE findSymbol;

    HMODULE module = GetModuleHandle(L"startmenu.dll");
    if (!module) {
        WCHAR szStartmenuDllPath[MAX_PATH];
        if (!GetWindowsDirectory(szStartmenuDllPath, ARRAYSIZE(szStartmenuDllPath))) {
            Wh_Log(L"GetWindowsDirectory failed");
            return FALSE;
        }

        wcscat_s(szStartmenuDllPath, LR"(\SystemApps\MicrosoftWindows.Client.CBS_cw5n1h2txyewy\StartMenu.dll)");
        module = LoadLibrary(szStartmenuDllPath);
        if (!module) {
            Wh_Log(L"LoadLibrary failed");
            return FALSE;
        }
    }

    SYMBOLHOOKS taskbarHooks[] = {
        {
            L"public: void __cdecl winrt::impl::consume_WindowsUdk_UI_StartScreen_Implementation_IDockedStartControllerOverrides<struct winrt::WindowsUdk::UI::StartScreen::Implementation::DockedStartController>::ShowAllApps(void)const __ptr64",
            NULL,
            (void**)&pOriginalShowAllApps
        },
        {
            L"public: void __cdecl winrt::impl::consume_WindowsUdk_UI_StartScreen_Implementation_IDockedStartControllerOverrides<struct winrt::WindowsUdk::UI::StartScreen::Implementation::DockedStartController>::HideAllApps(void)const __ptr64",
            (void*)HideAllAppsHook,
            (void**)&pOriginalHideAllApps
        },
        {
            L"public: __cdecl winrt::StartMenu::implementation::StartInnerFrame::StartInnerFrame(struct winrt::WindowsUdk::UI::StartScreen::Implementation::DockedStartController const & __ptr64,struct winrt::Windows::Foundation::IInspectable const & __ptr64) __ptr64",
            (void*)StartInnerFrameConstructorHook,
            (void**)&pOriginalStartInnerFrameConstructor
        }
    };

    findSymbol = Wh_FindFirstSymbol(module, NULL, &symbol);
    if (findSymbol) {
        do {
            for (size_t i = 0; i < ARRAYSIZE(taskbarHooks); i++) {
                if (!*taskbarHooks[i].pOriginalFunction && wcscmp(symbol.symbol, taskbarHooks[i].symbolName) == 0) {
                    if (taskbarHooks[i].hookFunction) {
                        Wh_SetFunctionHook(symbol.address, taskbarHooks[i].hookFunction, taskbarHooks[i].pOriginalFunction);
                        Wh_Log(L"Hooked %p (%s)", symbol.address, taskbarHooks[i].symbolName);
                    }
                    else {
                        *taskbarHooks[i].pOriginalFunction = symbol.address;
                        Wh_Log(L"Found %p (%s)", symbol.address, taskbarHooks[i].symbolName);
                    }
                    break;
                }
            }
        } while (Wh_FindNextSymbol(findSymbol, &symbol));

        Wh_FindCloseSymbol(findSymbol);
    }

    for (size_t i = 0; i < ARRAYSIZE(taskbarHooks); i++) {
        if (!*taskbarHooks[i].pOriginalFunction) {
            return FALSE;
        }
    }

    return TRUE;
}
