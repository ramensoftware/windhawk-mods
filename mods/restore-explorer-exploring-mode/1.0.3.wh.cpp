// ==WindhawkMod==
// @id              restore-explorer-exploring-mode
// @name            Restore Explorer "Exploring" Mode
// @description     Reintroduces File Explorer's "Exploring" mode from Windows XP and before
// @version         1.0.3
// @author          aubymori
// @github          https://github.com/aubymori
// @include         explorer.exe
// @compilerOptions -lshlwapi -lpropsys
// @architecture    x86-64
// @license         GPL-3.0
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Restore Explorer "Exploring" Mode
Before Windows 7, File Explorer acted different in many ways:

- Windows + E shortcut opens with the folder tree open by default.
- Directly invoking `explorer.exe` opens a different folder than the Windows + E shortcut.
- File Explorer windows have a different icon when the folder tree is open (Windows XP and before)
- The title of File Explorer windows is prefixed with "Exploring - " when it is opened via the explore
  verb (Windows + E shortcut, directly invoking `explorer.exe`)

This mod addresses all of these.

**Preview**:

![Preview](https://raw.githubusercontent.com/aubymori/images/refs/heads/main/restore-explorer-exploring-mode-preview.png)
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- exe_invoke_location: documents
  $name: explorer.exe invoke location
  $description: Location that should be opened when invoking "explorer.exe"
  $options:
  - unchanged: Unchanged
  - documents: Documents (Windows 2000, Me, XP, Vista)
  - sys_drive_root: Windows drive root (C:\) (Windows 95, 98, NT 4.0)
- explorer_icon: true
  $name: Explorer icon
  $description: Changes Explorer windows to have a generic Windows Explorer icon when the folder tree is open.
- exploring_text: Exploring
  $name: "\"Exploring\" text"
  $description:
    Text to use for the explore mode title (e.g. "Exploring - My Computer"). Leave blank 
    for the title to not be affected (Windows 2000/Me+).
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>
#include <initguid.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <propsys.h>

//
// Types
//

enum EXE_INVOKE_LOCATION
{
    EIL_UNCHANGED = 0,
    EIL_DOCUMENTS,
    EIL_SYSDRIVEROOT,
} g_exeInvokeLocation = EIL_UNCHANGED;

bool g_fExplorerIcon = false;

WindhawkUtils::StringSetting g_spszExploringText;

interface IBrowserEvents : IUnknown
{
    STDMETHOD(OnBrowserCreated)(HWND hwndBrowser, IDispatch *pdispBrowser, IShellItem *psi) PURE;
    STDMETHOD(OnNavigationCompleted)(HRESULT hrResult) PURE;
    STDMETHOD(OnBrowserClosed)() PURE;
    STDMETHOD(OnBrowserActive)() PURE;
    STDMETHOD(SetTitle)(LPCWSTR pszTitle) PURE;
    STDMETHOD(SetIcon)(HICON hIcon, BOOL fLarge) PURE;
    STDMETHOD(FrameTranslateAccelerator)(LPMSG pmsg, DWORD dwFlags) PURE;
};

DEFINE_GUID(SID_ShellBrowserPropStore, 0xA3B24A0A, 0x7B68, 0x448D, 0x99,0x79, 0xC7,0x00,0x05,0x9C,0x3A,0xD1);

//
// Util functions
//

BOOL (WINAPI *ShellExecuteExW_orig)(SHELLEXECUTEINFOW *);

HRESULT OpenFolderCSIDL(int csidl)
{
    SHELLEXECUTEINFOW shei = { 0 };
    shei.cbSize = sizeof(shei);
    shei.fMask  = SEE_MASK_IDLIST | SEE_MASK_INVOKEIDLIST;
    shei.nShow  = SW_SHOWNORMAL;
    shei.lpVerb = L"explore";

    shei.lpIDList = SHCloneSpecialIDList(NULL, csidl, FALSE);
    if (shei.lpIDList)
    {
        HRESULT hr = ShellExecuteExW_orig(&shei);
        ILFree((LPITEMIDLIST)shei.lpIDList);
        return hr;
    }
    return E_FAIL;
}

//
// Make Windows + E shortcut open in exploring mode
//
void (*_InvokeVerbOnParsingName_orig)(HWND, LPCWSTR, LPCWSTR);
void _InvokeVerbOnParsingName_hook(HWND hwnd, LPCWSTR pszUnused, LPCWSTR pszUnused2)
{
    OpenFolderCSIDL(CSIDL_DRIVES);
}

//
// Make calling "explorer.exe" open in exploring mode and optionally
// open Documents or drive root instead
//

BOOL WINAPI ShellExecuteExW_hook(
    SHELLEXECUTEINFOW *pExecInfo
)
{
    if (pExecInfo
    && pExecInfo->lpVerb
    && !wcscmp(pExecInfo->lpVerb, L"OpenNewWindow")
    && pExecInfo->lpFile
    && !wcscmp(pExecInfo->lpFile, L"shell:::{52205fd8-5dfb-447d-801a-d0b52f2e83e1}"))
    {
        switch (g_exeInvokeLocation)
        {
            case EIL_UNCHANGED:
                return ShellExecuteExW_orig(pExecInfo);
            case EIL_DOCUMENTS:
                return OpenFolderCSIDL(CSIDL_PERSONAL);
            case EIL_SYSDRIVEROOT:
            {
                WCHAR szSystemRoot[MAX_PATH];
                GetWindowsDirectoryW(szSystemRoot, ARRAYSIZE(szSystemRoot));

                while (*szSystemRoot && !PathIsRootW(szSystemRoot))
                {
                    PathRemoveFileSpecW(szSystemRoot);
                }

                SHELLEXECUTEINFOW shei = { 0 };
                shei.cbSize = sizeof(shei);
                shei.lpVerb = L"explore";
                shei.nShow  = SW_SHOWNORMAL;
                shei.lpFile = szSystemRoot;
                return ShellExecuteExW_orig(&shei);
            }
        }
    }
    
    return ShellExecuteExW_orig(pExecInfo);
}

//
// Show a generic Explorer icon when the tree view is visible
//

bool (*CShellBrowser__IsExplorerBandVisible)(void *pThis);

thread_local bool g_fInSetTitle = false;
thread_local bool g_fInExploreMode = false;

HRESULT (*CBrowserHost_SetTitle_orig)(void *pThis, LPCWSTR pszTitle);
HRESULT CBrowserHost_SetTitle_hook(void *pThis, LPCWSTR pszTitle)
{
    if (!g_fInSetTitle)
        return S_OK;

    if (g_spszExploringText[0] && g_fInExploreMode)
    {
        WCHAR szTitle[MAX_PATH];
        wcscpy_s(szTitle, g_spszExploringText.get());
        wcscat_s(szTitle, L" - ");
        wcscat_s(szTitle, pszTitle);
        return CBrowserHost_SetTitle_orig(pThis, szTitle);
    }
    return CBrowserHost_SetTitle_orig(pThis, pszTitle);
}

void (*CShellBrowser__SetTitle_orig)(void *);
void CShellBrowser__SetTitle_hook(void *pThis)
{
    g_fInSetTitle = true;

    IPropertyBag *pbp = nullptr;
    IServiceProvider *psp = (IServiceProvider *)((char *)pThis + 40);
    if (SUCCEEDED(psp->QueryService(SID_ShellBrowserPropStore, &pbp)))
    {
        BOOL fInExploreMode;
        if (SUCCEEDED(PSPropertyBag_ReadBOOL(pbp, L"ExpandInitialNav", &fInExploreMode)))
        {
            g_fInExploreMode = fInExploreMode;
        }
        pbp->Release();
    }

    CShellBrowser__SetTitle_orig(pThis);
    
    g_fInExploreMode = false;
    g_fInSetTitle = false;
}

void (*CShellBrowser__SetIcon_orig)(void *);
void CShellBrowser__SetIcon_hook(void *pThis)
{
    if (g_fExplorerIcon && CShellBrowser__IsExplorerBandVisible(pThis))
    {
        IBrowserEvents *pbe = *((IBrowserEvents **)pThis + 78);
        static HMODULE hShell32 = GetModuleHandleW(L"shell32.dll");

        #define IDI_STFLDRPROP 46
        HICON hIcon = (HICON)LoadImageW(
            hShell32, MAKEINTRESOURCEW(IDI_STFLDRPROP), IMAGE_ICON,
            GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON), 0);
        pbe->SetIcon(hIcon, FALSE);
        hIcon = (HICON)LoadImageW(
            hShell32, MAKEINTRESOURCEW(IDI_STFLDRPROP), IMAGE_ICON,
            GetSystemMetrics(SM_CXICON), GetSystemMetrics(SM_CYICON), 0);
        pbe->SetIcon(hIcon, TRUE);
    }
    else
    {
        CShellBrowser__SetIcon_orig(pThis);
    }
}

HRESULT (*CShellBrowser__OnCreate_orig)(void *, LPCREATESTRUCTW);
HRESULT CShellBrowser__OnCreate_hook(
    void            *pThis,
    LPCREATESTRUCTW  pcs
)
{
    HRESULT hr = CShellBrowser__OnCreate_orig(pThis, pcs);
    IPropertyBag *pbp = nullptr;
    IServiceProvider *psp = (IServiceProvider *)((char *)pThis + 40);

    // There's not a simple function we can hook to check when the tree view is toggled,
    // so instead we change the icon when certain properties are written to the shell browser's
    // property bag.
    if (SUCCEEDED(psp->QueryService(SID_ShellBrowserPropStore, &pbp)))
    {
        PSPropertyBag_WriteULONGLONG(pbp, L"WH_ShellBrowserPtr", (ULONGLONG)pThis);
        pbp->Release();
    }

    return hr;
}

HRESULT (*CMemPropStore_Write_orig)(IPropertyBag *, LPCOLESTR, VARIANT *);
HRESULT CMemPropStore_Write_hook(
    IPropertyBag *pThis,
    LPCOLESTR     pszPropName,
    VARIANT      *pVar
)
{
    HRESULT hr = CMemPropStore_Write_orig(pThis, pszPropName, pVar);
    if (!wcscmp(pszPropName, L"ProperTreeExpandoSizer_MaxxedOut")
    || !wcscmp(pszPropName, L"PageSpaceControlSizer_Visible"))
    {
        void *pShellBrowser = nullptr;
        if (SUCCEEDED(PSPropertyBag_ReadULONGLONG(pThis, L"WH_ShellBrowserPtr", (ULONGLONG *)&pShellBrowser)))
        {
            CShellBrowser__SetIcon_hook(pShellBrowser);
        }
    }
    return hr;
}

//
// Init stuff
//

const WindhawkUtils::SYMBOL_HOOK explorerExeHooks[] = {
    {
        {
            L"void __cdecl _InvokeVerbOnParsingName(struct HWND__ *,unsigned short const *,unsigned short const *)"
        },
        &_InvokeVerbOnParsingName_orig,
        _InvokeVerbOnParsingName_hook,
        false
    },
};

const WindhawkUtils::SYMBOL_HOOK explorerFrameDllHooks[] = {
    {
        {
            L"private: int __cdecl CShellBrowser::_IsExplorerBandVisible(void)"
        },
        &CShellBrowser__IsExplorerBandVisible,
        nullptr,
        false
    },
    {
        {
            L"public: virtual long __cdecl CBrowserHost::SetTitle(unsigned short const *)"
        },
        &CBrowserHost_SetTitle_orig,
        CBrowserHost_SetTitle_hook,
        false
    },
    {
        {
            L"private: void __cdecl CShellBrowser::_SetTitle(void)"
        },
        &CShellBrowser__SetTitle_orig,
        CShellBrowser__SetTitle_hook,
        false
    },
    {
        {
            L"private: void __cdecl CShellBrowser::_SetIcon(void)"
        },
        &CShellBrowser__SetIcon_orig,
        CShellBrowser__SetIcon_hook,
        false
    },
    {
        {
            L"private: long __cdecl CShellBrowser::_OnCreate(struct tagCREATESTRUCTW *)"
        },
        &CShellBrowser__OnCreate_orig,
        CShellBrowser__OnCreate_hook,
        false
    },
};

const WindhawkUtils::SYMBOL_HOOK propsysDllHooks[] = {
    {
        {
            L"public: virtual long __cdecl CMemPropStore::Write(unsigned short const *,struct tagVARIANT *)"
        },
        &CMemPropStore_Write_orig,
        CMemPropStore_Write_hook,
        false
    },
};

void Wh_ModSettingsChanged(void)
{
    g_spszExploringText = WindhawkUtils::StringSetting::make(L"exploring_text");
    g_fExplorerIcon = Wh_GetIntSetting(L"explorer_icon");
}

BOOL Wh_ModInit(void)
{
    Wh_ModSettingsChanged();

    // We can safely only retrieve this in init, since it only matters for
    // when explorer.exe's WinMain runs.
    LPCWSTR pszInvokeLocation = Wh_GetStringSetting(L"exe_invoke_location");
    if (!wcscmp(pszInvokeLocation, L"documents"))
    {
        g_exeInvokeLocation = EIL_DOCUMENTS;
    }
    else if (!wcscmp(pszInvokeLocation, L"sys_drive_root"))
    {
        g_exeInvokeLocation = EIL_SYSDRIVEROOT;
    }
    Wh_FreeStringSetting(pszInvokeLocation);

    if (!WindhawkUtils::HookSymbols(
        GetModuleHandleW(NULL),
        explorerExeHooks,
        ARRAYSIZE(explorerExeHooks)
    ))
    {
        Wh_Log(L"Failed to hook one or more symbol functions in explorer.exe");
        return FALSE;
    }

    HMODULE hExplorerFrame = LoadLibraryExW(L"ExplorerFrame.dll", NULL, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!hExplorerFrame)
    {
        Wh_Log(L"Failed to load ExplorerFrame.dll");
        return FALSE;
    }

    if (!WindhawkUtils::HookSymbols(
        hExplorerFrame,
        explorerFrameDllHooks,
        ARRAYSIZE(explorerFrameDllHooks)
    ))
    {
        Wh_Log(L"Failed to hook one or more symbol functions in ExplorerFrame.dll");
        return FALSE;
    }

    HMODULE hPropsys = LoadLibraryExW(L"propsys.dll", NULL, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!hPropsys)
    {
        Wh_Log(L"Failed to load propsys.dll");
        return FALSE;
    }

    if (!WindhawkUtils::HookSymbols(
        hPropsys,
        propsysDllHooks,
        ARRAYSIZE(propsysDllHooks)
    ))
    {
        Wh_Log(L"Failed to hook one or more symbol functions in propsys.dll");
        return FALSE;
    }

    if (!Wh_SetFunctionHook(
        (void *)ShellExecuteExW,
        (void *)ShellExecuteExW_hook,
        (void **)&ShellExecuteExW_orig
    ))
    {
        Wh_Log(L"Failed to hook ShellExecuteExW");
        return FALSE;
    }

    return TRUE;
}