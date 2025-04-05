// ==WindhawkMod==
// @id              explorer-toolbar-links-item
// @name            File Explorer Toolbar Links Item
// @description     Restores the ability to display the hidden "Links" toolbar in Windows 10 and 11.
// @version         1.0
// @author          Isabella Lulamoon (kawapure)
// @github          https://github.com/kawapure
// @twitter         https://twitter.com/kawaipure
// @homepage        https://kawapure.github.io
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -luser32 -lole32 -luuid
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# File Explorer Toolbar Links Item

This mod restores the ability to display the "Links" toolbar, hidden since Windows Vista, in Windows 10 and 11.

The links toolbar can be used to provide a custom set of quick links that the user can access across the top of
any File Explorer window.

![Preview image](https://raw.githubusercontent.com/kawapure/images/refs/heads/main/linksbar.png)

## What's the difference between this and some registry patch?

This restores the actual hidden Links toolbar with all of its special handlers. **It functions perfectly**, just like
it did in Windows 98, 2000, and XP. For a technical explanation, [please see this thread on WinClassic.](https://winclassic.net/thread/2913/explorer-toolbar-funtional-modern-windows)

Adding the CLSID for the toolbar will install it as an external toolbar, which has a few bugs. You cannot reorder
items, or drag items into it, and its vertical height was uncapped so it was prone to looking visually broken.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- custom_localized_link_string: ""
  $name: Custom localized "Link" string
  $name:en-GB: Custom localised "Link" string
  $description: >
    This allows you to override the "Link" string. Use the "&" character to specify
    a keyboard shortcut for the text.
*/
// ==/WindhawkModSettings==

// We only inject into explorer.exe because toolbars don't load in other processes.
// Fortunately, this greatly simplifies the work we need to do.

#include <processthreadsapi.h>
#include <windows.h>
#include <windowsx.h>
#include <windhawk_api.h>
#include <windhawk_utils.h>
#include <exdisp.h>
#include <string>

HMODULE g_hInstExplorerFrame = nullptr;
std::wstring g_spszLinksText;

#define MOD_CANCEL_INIT_AND_SHOW_USER_ERROR(msg) \
    MessageBoxW(nullptr,                         \
            msg,                                 \
            kErrorTitle,                         \
            MB_OK                                \
        );                                       \
    return FALSE

#define ARRAY_SIZE_ARGS(arr) arr, ARRAYSIZE(arr)

constexpr LPCWSTR kErrorTitle = L"Windhawk :: BrowseUI Links Item mod";

#define VBF_TOOLS 0x01
#define VBF_ADDRESS 0x02
#define VBF_LINKS 0x04
#define VBF_BRAND 0x08
#define VBF_MENU 0x10

#define IDM_ITBAR 0x108

// Menu item ID for the separator:
#define FCIDM_VIEWCONTEXTMENUSEP 0xA208

// Menu item ID for the Links button:
#define FCIDM_VIEWLINKS 0xA206

// String resource ID for the "&Links" text in ExplorerFrame.dll:
#define IDS_LINKS 0x3352

void LoadSettings()
{
    WindhawkUtils::StringSetting spszCustomLocalizedLinkText =
        WindhawkUtils::StringSetting::make(L"custom_localized_link_string");

    if (wcslen(spszCustomLocalizedLinkText.get()) > 0)
    {
        g_spszLinksText = spszCustomLocalizedLinkText;
    }
    else
    {
        // Load the links text from ExplorerFrame.
        WCHAR szBuffer[MAX_PATH];
        LoadStringW(g_hInstExplorerFrame, IDS_LINKS, ARRAY_SIZE_ARGS(szBuffer));
        g_spszLinksText = szBuffer;
    }

    // Ultimate fallback text:
    if (g_spszLinksText.empty())
    {
        Wh_Log(L"Failed to find links text in ExplorerFrame.dll, and the user did not specify "
               L"a custom string. Falling back to ultimate fallback text.");
        g_spszLinksText = L"&Links";
    }
}

class CInternetToolbar : public IUnknown
{
public:
    // Offset can be found in CInternetToolbar::_ShowLinks
    static DWORD &GetVisibleBands(CInternetToolbar *pThis)
    {
#ifdef _WIN64
        return *((DWORD *)pThis + 61);
#else
        return *((DWORD *)pThis + 34);
#endif
    }

    DWORD &GetVisibleBands() { return GetVisibleBands(this); }

    // Offset can be found in CInternetToolbar::_OnCommand
    static IWebBrowser2 *&GetPtrIE(CInternetToolbar *pThis)
    {
#ifdef _WIN64
        return *((IWebBrowser2 **)((size_t *)pThis + 32));
#else
        return *((IWebBrowser2 **)((DWORD *)pThis + 36));
#endif
    }

    IWebBrowser2 *&GetPtrIE() { return GetPtrIE(this); }
};

class CShellBrowser
{
public:
    // Offset can be found in CShellBrowser::_OnViewMenuPopup
    static CInternetToolbar *&GetPtrInternetToolbar(CShellBrowser *pThis)
    {
#if _WIN64
        return **(CInternetToolbar ***)((size_t *)pThis + 92);
#else
        return **(CInternetToolbar ***)(((BYTE *)pThis + 404));
#endif
    }

    CInternetToolbar *&GetPtrInternetToolbar() { return GetPtrInternetToolbar(this); }
};

// LoadMenuW hook useful globals:
DWORD g_dwInterceptMenuCreationThreadId = 0;
CInternetToolbar *g_pInternetToolbar = nullptr;

using LoadMenuW_t = decltype(&LoadMenuW);
LoadMenuW_t LoadMenuW_orig;
HMENU WINAPI LoadMenuW_hook(HINSTANCE hInstance, LPCWSTR lpMenuName)
{
    HMENU hMenu = LoadMenuW_orig(hInstance, lpMenuName);

    if (g_dwInterceptMenuCreationThreadId == GetCurrentThreadId() && lpMenuName == MAKEINTRESOURCEW(IDM_ITBAR))
    {
        InsertMenuW(hMenu, FCIDM_VIEWCONTEXTMENUSEP, MF_BYCOMMAND, FCIDM_VIEWLINKS, g_spszLinksText.c_str());
        
        if (g_pInternetToolbar)
        {
            CheckMenuItem(hMenu, FCIDM_VIEWLINKS, 
                g_pInternetToolbar->GetVisibleBands() & VBF_LINKS 
                    ? MF_CHECKED 
                    : MF_UNCHECKED
            );
        }
        else
        {
            Wh_Log(L"WARNING: We're being called for the purpose of adding the menu item to an "
                   L"Internet Toolbar, but g_pInternetToolbar is nullptr.");
        }
    }

    return hMenu;
}

void (*CInternetToolbar___ShowVisible)(CInternetToolbar *pThis, DWORD dwNewVisibleItemFlags, BOOL fNotifyShellBrowser);

void (*CInternetToolbar___ShowContextMenu_orig)(CInternetToolbar *pThis, HWND hWnd, LPARAM lParam, RECT *prc);
void CInternetToolbar___ShowContextMenu_hook(CInternetToolbar *pThis, HWND hWnd, LPARAM lParam, RECT *prc)
{
    g_dwInterceptMenuCreationThreadId = GetCurrentThreadId();
    g_pInternetToolbar = pThis;

    CInternetToolbar___ShowContextMenu_orig(pThis, hWnd, lParam, prc);

    g_dwInterceptMenuCreationThreadId = 0;
    g_pInternetToolbar = nullptr;
}

void (*CInternetToolbar___OnCommand_orig)(CInternetToolbar *pThis, WPARAM wParam, LPARAM lParam);
void CInternetToolbar___OnCommand_hook(CInternetToolbar *pThis, WPARAM wParam, LPARAM lParam)
{
    UINT idCmd = GET_WM_COMMAND_ID(wParam, lParam);

    // Run the original first since it does some work that we wanna do and won't really
    // step over us since we're requesting a command ID it doesn't even handle anymore.
    CInternetToolbar___OnCommand_orig(pThis, wParam, lParam);

    if (idCmd == FCIDM_VIEWLINKS)
    {
        // For some reason, it will only immediately refresh if VBF_BRAND is passed here too.
        pThis->GetVisibleBands() ^= VBF_LINKS | VBF_BRAND;

        if (!(pThis->GetVisibleBands() & ~VBF_BRAND))
        {
            pThis->GetPtrIE()->put_ToolBar(FALSE);
        }
        
        CInternetToolbar___ShowVisible(pThis, pThis->GetVisibleBands(), TRUE);
    }
}

void (*CShellBrowser___OnViewMenuPopup_orig)(CShellBrowser *pThis, HMENU hMenu);
void CShellBrowser___OnViewMenuPopup_hook(CShellBrowser *pThis, HMENU hMenu)
{
    g_dwInterceptMenuCreationThreadId = GetCurrentThreadId();

    // There is a constant offset of 128 here. Without looking into it, I presume this is
    // the base class CBaseBar, since calling QueryInterface will not even get the proper
    // base pointer of the class, but I haven't completely looked into it.
    g_pInternetToolbar = (CInternetToolbar *)((BYTE *)pThis->GetPtrInternetToolbar() - 128);

    CShellBrowser___OnViewMenuPopup_orig(pThis, hMenu);

    g_dwInterceptMenuCreationThreadId = 0;
    g_pInternetToolbar = nullptr;
}

HRESULT (*CShellBrowser___OnCommand_orig)(CShellBrowser *pThis, WPARAM wParam, LPARAM lParam);
HRESULT CShellBrowser___OnCommand_hook(CShellBrowser *pThis, WPARAM wParam, LPARAM lParam)
{
    if (wParam == FCIDM_VIEWLINKS)
    {
        CInternetToolbar *pInternetToolbar = (CInternetToolbar *)((BYTE *)pThis->GetPtrInternetToolbar() - 128);

        CInternetToolbar___OnCommand_hook(pInternetToolbar, FCIDM_VIEWLINKS, 0);
        return S_OK;
    }

    return CShellBrowser___OnCommand_orig(pThis, wParam, lParam);
}

// ExplorerFrame.dll
WindhawkUtils::SYMBOL_HOOK c_rgHooksExplorerFrame[] = {
    {
        // CInternetToolbar::_ShowContextMenu creates the Internet Toolbar context menu for when
        // the user right clicks on a toolbar (i.e. the menu bar). 
        {
            L"protected: void __cdecl CInternetToolbar::_ShowContextMenu(struct HWND__ *,__int64,struct tagRECT const *)"
        },
        &CInternetToolbar___ShowContextMenu_orig,
        CInternetToolbar___ShowContextMenu_hook
    },
    {
        {
            L"protected: void __cdecl CInternetToolbar::_ShowVisible(unsigned long,int)"
        },
        &CInternetToolbar___ShowVisible
    },
    {
        {
            L"protected: void __cdecl CInternetToolbar::_OnCommand(unsigned __int64,__int64)"
        },
        &CInternetToolbar___OnCommand_orig,
        CInternetToolbar___OnCommand_hook
    },
    {
        // CShellBrowser::_OnViewMenuPopup creates the Internet Toolbar context menu for when
        // the user opens the toolbars menu from 
        {
            L"private: void __cdecl CShellBrowser::_OnViewMenuPopup(struct HMENU__ *)"
        },
        &CShellBrowser___OnViewMenuPopup_orig,
        CShellBrowser___OnViewMenuPopup_hook
    },
    {
        {
            L"private: __int64 __cdecl CShellBrowser::_OnCommand(unsigned __int64,__int64)"
        },
        &CShellBrowser___OnCommand_orig,
        CShellBrowser___OnCommand_hook
    },
};

// The mod is being initialized, load settings, hook functions, and do other
// initialization stuff if required.
BOOL Wh_ModInit()
{
    Wh_Log(L"Init");

    HMODULE hmExplorerFrame = GetModuleHandleW(L"ExplorerFrame.dll");

    if (!hmExplorerFrame)
    {
        MOD_CANCEL_INIT_AND_SHOW_USER_ERROR(L"Failed to find the handle to ExplorerFrame.dll.");
    }

    g_hInstExplorerFrame = hmExplorerFrame;

    // Precondition checks are done, so proceed with initialising the mod now:
    LoadSettings();

    if (!WindhawkUtils::SetFunctionHook(
        LoadMenuW,
        LoadMenuW_hook,
        &LoadMenuW_orig
    ))
    {
        MOD_CANCEL_INIT_AND_SHOW_USER_ERROR(L"Failed to install hooks for LoadMenuW.");
    }

    if (!WindhawkUtils::HookSymbols(hmExplorerFrame, ARRAY_SIZE_ARGS(c_rgHooksExplorerFrame)))
    {
        MOD_CANCEL_INIT_AND_SHOW_USER_ERROR(L"Failed to install hooks for ExplorerFrame.dll.");
    }

    return TRUE;
}

// The mod is being unloaded, free all allocated resources.
void Wh_ModUninit()
{
    Wh_Log(L"Uninit");
}

BOOL Wh_ModSettingsChanged()
{
    LoadSettings();
    return TRUE;
}
