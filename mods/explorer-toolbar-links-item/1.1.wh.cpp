// ==WindhawkMod==
// @id              explorer-toolbar-links-item
// @name            File Explorer Toolbar Links Item
// @description     Restores the ability to display the hidden "Links" toolbar in Windows 10 and 11.
// @version         1.1
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
This mod is based [on the work of AsumiLuna, which you can see on the WinClassic forums here.](https://winclassic.net/thread/2913/explorer-toolbar-funtional-modern-windows)

The links toolbar can be used to provide a custom set of quick links that the user can access across the top of
any File Explorer window.

![Preview image](https://raw.githubusercontent.com/kawapure/images/refs/heads/main/linksbar.png)

You can also enable it through the Ribbon UI via the "Options" dropdown in the "View" tab:

![Ribbon UI enablement position preview image](https://raw.githubusercontent.com/kawapure/images/refs/heads/main/linksbar_ribbon.png)

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
#include <winscard.h>
#include <string>
#include <shobjidl.h>
#include <initguid.h>

class CVersionHelper
{
public:
	// Data structure returned by GetVersionInfo().
	struct VersionStruct
	{
		bool isInitialized = false;
		DWORD dwMajorVersion = 0;
		DWORD dwMinorVersion = 0;
		DWORD dwBuildNumber = 0;
		DWORD dwPlatformId = 0;
	};

	typedef void (WINAPI *RtlGetVersion_t)(OSVERSIONINFOEXW *);

	// Gets the precise OS version.
	static const VersionStruct *GetVersionInfo()
	{
		static VersionStruct s_versionStruct = { 0 };

		// Skip if cached.
		if (!s_versionStruct.isInitialized)
		{
			HMODULE hMod = LoadLibraryW(L"ntdll.dll");

			if (hMod)
			{
				RtlGetVersion_t func = (RtlGetVersion_t)GetProcAddress(hMod, "RtlGetVersion");

				if (!func)
				{
					FreeLibrary(hMod);

					// TODO: error handling.
					return &s_versionStruct;
				}

				OSVERSIONINFOEXW osVersionInfo = { 0 };
				osVersionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEXW);

				func(&osVersionInfo);

				s_versionStruct.dwBuildNumber = osVersionInfo.dwBuildNumber;
				s_versionStruct.dwMajorVersion = osVersionInfo.dwMajorVersion;
				s_versionStruct.dwMinorVersion = osVersionInfo.dwMinorVersion;
				s_versionStruct.dwPlatformId = osVersionInfo.dwPlatformId;

				s_versionStruct.isInitialized = true;

				FreeLibrary(hMod);
			}
		}

		return &s_versionStruct;
	}

	// Specific version helpers.
	inline static bool IsWindows10OrGreater()
	{
		return GetVersionInfo()->dwMajorVersion >= 10 &&
			   GetVersionInfo()->dwBuildNumber >= 10240;
	}

    inline static bool IsWindows11OrGreater()
    {
        return GetVersionInfo()->dwMajorVersion >= 10 &&
               GetVersionInfo()->dwBuildNumber >= 22000;
    }
};

// Architecture calling convention declarations:
#ifdef _WIN64
#   define THISCALL  __cdecl
#   define STHISCALL L"__cdecl"

#   define STDCALL  __cdecl
#   define SSTDCALL L"__cdecl"
#else
#   define THISCALL  __thiscall
#   define STHISCALL L"__thiscall"

#   define STDCALL  __stdcall
#   define SSTDCALL L"__stdcall"
#endif

HMODULE g_hInstExplorerFrame = nullptr;
std::wstring g_spszLinksText;

#define MOD_CANCEL_INIT_AND_SHOW_USER_ERROR(msg) \
    Wh_Log(L"Failed to load mod: %s", msg);      \
    MessageBoxW(nullptr,                         \
            msg,                                 \
            kErrorTitle,                         \
            MB_OK                                \
        );                                       \
    return FALSE

#ifdef _WIN64
    #define FOR_64_32(for64, for32) for64
#else
    #define FOR_64_32(for64, for32) for32
#endif

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
        if (CVersionHelper::IsWindows11OrGreater())
        {
            return **(CInternetToolbar ***)((size_t *)pThis + 93);
        }
        else
        {
            return **(CInternetToolbar ***)((size_t *)pThis + 92);
        }
#else
        return **(CInternetToolbar ***)(((BYTE *)pThis + 404));
#endif
    }

    CInternetToolbar *&GetPtrInternetToolbar() { return GetPtrInternetToolbar(this); }
};

class CLegacyBarsGalleryItem : public IObjectWithSite
{
public:
    // Offset can be found in CLegacyBarsGalleryItem::CLegacyBarsGalleryItem
    static DWORD &GetMenuId(CLegacyBarsGalleryItem *pThis)
    {
#if _WIN64
        return *((DWORD *)pThis + 24);
#else
        return *((DWORD *)pThis + 19);
#endif
    }

    DWORD &GetMenuId() { return GetMenuId(this); }
};

// LoadMenuW hook useful globals:
DWORD g_dwInterceptMenuCreationThreadId = 0;
CInternetToolbar *g_pInternetToolbar = nullptr;

using LoadMenuW_t = decltype(&LoadMenuW);
LoadMenuW_t LoadMenuW_orig;
HMENU WINAPI LoadMenuW_hook(HINSTANCE hInstance, LPCWSTR lpMenuName)
{
    Wh_Log(L"Entered method.");
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

void (THISCALL *CInternetToolbar___ShowVisible)(CInternetToolbar *pThis, DWORD dwNewVisibleItemFlags, BOOL fNotifyShellBrowser);

void (THISCALL *CInternetToolbar___ShowContextMenu_orig)(CInternetToolbar *pThis, HWND hWnd, LPARAM lParam, RECT *prc);
void THISCALL CInternetToolbar___ShowContextMenu_hook(CInternetToolbar *pThis, HWND hWnd, LPARAM lParam, RECT *prc)
{
    Wh_Log(L"Entered method.");
    g_dwInterceptMenuCreationThreadId = GetCurrentThreadId();
    g_pInternetToolbar = pThis;

    CInternetToolbar___ShowContextMenu_orig(pThis, hWnd, lParam, prc);

    g_dwInterceptMenuCreationThreadId = 0;
    g_pInternetToolbar = nullptr;
}

void (THISCALL *CInternetToolbar___OnCommand_orig)(CInternetToolbar *pThis, WPARAM wParam, LPARAM lParam);
void THISCALL CInternetToolbar___OnCommand_hook(CInternetToolbar *pThis, WPARAM wParam, LPARAM lParam)
{
    Wh_Log(L"Entered method.");
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

bool g_fLieAboutToolbarPopulation = false;
BOOL (STDCALL *PopulateItbarToolbarBands_orig)(HMENU hMenu, IUnknown *pUnk);
BOOL STDCALL PopulateItbarToolbarBands_hook(HMENU hMenu, IUnknown *pUnk)
{
    Wh_Log(L"Entered method.");
    if (g_fLieAboutToolbarPopulation)
    {
        PopulateItbarToolbarBands_orig(hMenu, pUnk);
        return TRUE;
    }

    return PopulateItbarToolbarBands_orig(hMenu, pUnk);
}

void (THISCALL *CShellBrowser___OnViewMenuPopup_orig)(CShellBrowser *pThis, HMENU hMenu);
void THISCALL CShellBrowser___OnViewMenuPopup_hook(CShellBrowser *pThis, HMENU hMenu)
{
    Wh_Log(L"Entered method.");
    g_dwInterceptMenuCreationThreadId = GetCurrentThreadId();

    // There is a constant offset of 128 here. Without looking into it, I presume this is
    // the base class CBaseBar, since calling QueryInterface will not even get the proper
    // base pointer of the class, but I haven't completely looked into it.
    g_pInternetToolbar = (CInternetToolbar *)((BYTE *)pThis->GetPtrInternetToolbar() - 128);

    // Make PopulateItbarToolbarBands lie about its result so that the toolbars menu isn't
    // removed from the result; we want to keep the menu when the Links toolbar is available.
    g_fLieAboutToolbarPopulation = true;

    CShellBrowser___OnViewMenuPopup_orig(pThis, hMenu);

    g_fLieAboutToolbarPopulation = false;
    g_dwInterceptMenuCreationThreadId = 0;
    g_pInternetToolbar = nullptr;
}

void (THISCALL *CShellBrowser___InvalidateRibbonCommandSet)(CShellBrowser *pThis, int eCommandSet);

HRESULT (THISCALL *CShellBrowser___OnCommand_orig)(CShellBrowser *pThis, WPARAM wParam, LPARAM lParam);
HRESULT THISCALL CShellBrowser___OnCommand_hook(CShellBrowser *pThis, WPARAM wParam, LPARAM lParam)
{
    Wh_Log(L"Entered method.");
    if (wParam == FCIDM_VIEWLINKS)
    {
        CInternetToolbar *pInternetToolbar = (CInternetToolbar *)((BYTE *)pThis->GetPtrInternetToolbar() - 128);

        CInternetToolbar___OnCommand_hook(pInternetToolbar, FCIDM_VIEWLINKS, 0);

        // NOTE: This does not update the selected item if the user then goes on to manage the
        // visible toolbars through any other means (i.e. using the Internet Toolbar context
        // menu). This is not my mistake. Microsoft messed this up.
        CShellBrowser___InvalidateRibbonCommandSet(pThis, 7);
        return S_OK;
    }

    return CShellBrowser___OnCommand_orig(pThis, wParam, lParam);
}

HRESULT (THISCALL *CShellBrowser__GetLegacyBarsMenu_orig)(CShellBrowser *pThis, HMENU *phMenu);
HRESULT THISCALL CShellBrowser__GetLegacyBarsMenu_hook(CShellBrowser *pThis, HMENU *phMenu)
{
    Wh_Log(L"Entered method.");
    HRESULT hr = CShellBrowser__GetLegacyBarsMenu_orig(pThis, phMenu);

    // Adjust vftable:
    CShellBrowser *pThisAdj = (CShellBrowser *)((size_t *)pThis - 36);

    if (SUCCEEDED(hr))
    {
        InsertMenuW(*phMenu, 0, MF_BYPOSITION, FCIDM_VIEWLINKS, g_spszLinksText.c_str());

        CInternetToolbar *pInternetToolbar = (CInternetToolbar *)((BYTE *)pThisAdj->GetPtrInternetToolbar() - 128);

        CheckMenuItem(*phMenu, FCIDM_VIEWLINKS, 
            pInternetToolbar->GetVisibleBands() & VBF_LINKS 
                ? MF_CHECKED 
                : MF_UNCHECKED
        );
    }

    return hr;
}

// ExplorerFrame.dll
WindhawkUtils::SYMBOL_HOOK c_rgHooksExplorerFrame[] = {
    {
        // CInternetToolbar::_ShowContextMenu creates the Internet Toolbar context menu for when
        // the user right clicks on a toolbar (i.e. the menu bar).
        {
            FOR_64_32(
                L"protected: void __cdecl CInternetToolbar::_ShowContextMenu(struct HWND__ *,__int64,struct tagRECT const *)",
                L"protected: void __thiscall CInternetToolbar::_ShowContextMenu(struct HWND__ *,long,struct tagRECT const *)"
            )
        },
        &CInternetToolbar___ShowContextMenu_orig,
        CInternetToolbar___ShowContextMenu_hook
    },
    {
        {
            FOR_64_32(
                L"protected: void __cdecl CInternetToolbar::_ShowVisible(unsigned long,int)",
                L"protected: void __thiscall CInternetToolbar::_ShowVisible(unsigned long,int)"
            )
        },
        &CInternetToolbar___ShowVisible
    },
    {
        {
            FOR_64_32(
                L"protected: void __cdecl CInternetToolbar::_OnCommand(unsigned __int64,__int64)",
                L"protected: void __thiscall CInternetToolbar::_OnCommand(unsigned int,long)"
            )
        },
        &CInternetToolbar___OnCommand_orig,
        CInternetToolbar___OnCommand_hook
    },
    {
        // CShellBrowser::_OnViewMenuPopup creates the Internet Toolbar context menu for when
        // the user opens the toolbars menu from 
        {
            FOR_64_32(
                L"private: void __cdecl CShellBrowser::_OnViewMenuPopup(struct HMENU__ *)",
                L"private: void __thiscall CShellBrowser::_OnViewMenuPopup(struct HMENU__ *)"
            )
        },
        &CShellBrowser___OnViewMenuPopup_orig,
        CShellBrowser___OnViewMenuPopup_hook
    },
    {
        {
            FOR_64_32(
                L"private: __int64 __cdecl CShellBrowser::_OnCommand(unsigned __int64,__int64)",
                L"private: long __thiscall CShellBrowser::_OnCommand(unsigned int,long)"
            )
        },
        &CShellBrowser___OnCommand_orig,
        CShellBrowser___OnCommand_hook
    },
    {
        // Response is a boolean indicating if any toolbars are installed.
        // We control this response in some cases to make the toolbars menu always display.
        {
            FOR_64_32(
                L"int __cdecl PopulateItbarToolbarBands(struct HMENU__ *,struct IUnknown *)",
                L"int __stdcall PopulateItbarToolbarBands(struct HMENU__ *,struct IUnknown *)"
            )
        },
        &PopulateItbarToolbarBands_orig,
        PopulateItbarToolbarBands_hook
    },
    {
        // Used to get the menu for Ribbon UI.
        {
            FOR_64_32(
                L"public: virtual long __cdecl CShellBrowser::GetLegacyBarsMenu(struct HMENU__ * *)",
                L"public: virtual long __stdcall CShellBrowser::GetLegacyBarsMenu(struct HMENU__ * *)"
            )
        },
        &CShellBrowser__GetLegacyBarsMenu_orig,
        CShellBrowser__GetLegacyBarsMenu_hook
    },
    {
        // Used to update the menu item states for Ribbon UI.
        {
            FOR_64_32(
                L"private: void __cdecl CShellBrowser::_InvalidateRibbonCommandSet(enum COMMAND_SET)",
                L"private: void __thiscall CShellBrowser::_InvalidateRibbonCommandSet(enum COMMAND_SET)"
            )
        },
        &CShellBrowser___InvalidateRibbonCommandSet
    },
};

// The mod is being initialized, load settings, hook functions, and do other
// initialization stuff if required.
BOOL Wh_ModInit()
{
    Wh_Log(L"Init");

    g_hInstExplorerFrame = LoadLibraryW(L"ExplorerFrame.dll");

    if (!g_hInstExplorerFrame)
    {
        MOD_CANCEL_INIT_AND_SHOW_USER_ERROR(L"Failed to find the handle to ExplorerFrame.dll.");
    }

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

    if (!WindhawkUtils::HookSymbols(g_hInstExplorerFrame, ARRAY_SIZE_ARGS(c_rgHooksExplorerFrame)))
    {
        MOD_CANCEL_INIT_AND_SHOW_USER_ERROR(L"Failed to install hooks for ExplorerFrame.dll.");
    }

    return TRUE;
}

// The mod is being unloaded, free all allocated resources.
void Wh_ModUninit()
{
    Wh_Log(L"Uninit");
    FreeLibrary(g_hInstExplorerFrame);
}

BOOL Wh_ModSettingsChanged()
{
    LoadSettings();
    return TRUE;
}
