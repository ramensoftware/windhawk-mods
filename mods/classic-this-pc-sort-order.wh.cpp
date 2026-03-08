// ==WindhawkMod==
// @id              classic-this-pc-sort-order
// @name            Classic This PC Item Sorting
// @description     Makes the sort order in "This PC" folder like in Windows XP and before
// @version         1.0
// @author          xalejandro
// @github          https://github.com/tetawaves
// @include         *
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Classic This PC Item Sorting
Makes the sort order in "This PC" folder like in Windows XP and before.  
Restarting Explorer is recommended after installing the mod.

**Before**

![Before](https://i.imgur.com/sRULNeY.png)

**After**

![After](https://i.imgur.com/f5wskMi.png)

*/
// ==/WindhawkModReadme==


// ==WindhawkModSettings==
/*
- addcpltype: category
  $name: Add Control Panel Shortcut
  $description: Determines what type of Control Panel shortcut on This PC will be added. Requires a process restart for the setting to take effect.
  $options:
    - none: None
    - category: Category shortcut
    - classic: All Items shortcut
- hidecplfrompc: true
  $name: Hide Control Panel from This PC folder view
  $description: Hides Control Panel shortcut on the folder view and keeps it in the navigation pane. Requires a process restart for the setting to take effect.
*/
// ==/WindhawkModSettings==

#include <shlobj.h>
#include <initguid.h>
#include <propkey.h>
#include <windhawk_api.h>
#include <windhawk_utils.h>

#ifdef _WIN64
#   define STDCALL  __cdecl
#   define SSTDCALL L"__cdecl"
#else
#   define STDCALL  __stdcall
#   define SSTDCALL L"__stdcall"
#endif

#define REGSTR_PATH_EXPLORER_COMPUTER_NAMESPACE L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\MyComputer\\NameSpace"
#define CONTROLS_SORT_INDEX 30

DEFINE_GUID(CLSID_ControlPanel, 0x26EE0668, 0xA00A, 0x44D7, 0x93, 0x71, 0xBE, 0xB0, 0x64, 0xC9, 0x86, 0x83);
DEFINE_GUID(CLSID_ControlPanelClassic, 0x21EC2020, 0x3AEA, 0x1069, 0xA2, 0xDD, 0x08, 0x00, 0x2B, 0x30, 0x30, 0x9D);
DEFINE_GUID(IID_IShellItem2, 0x7e9fb0d3, 0x919f, 0x4307, 0xab,0x2e, 0x9b,0x18,0x60,0x31,0x0c,0x93);

typedef enum
{
    ADDCPL_NONE = 0,
    ADDCPL_CATEGORY,
    ADDCPL_CLASSIC
} ADDCPLTYPE;

struct
{
    ADDCPLTYPE fAddCPL;
    bool fHideCPLFromThisPC;
} g_settings;

typedef struct
{
    const CLSID * pclsid;
    UINT    uNameID;
    LPCTSTR pszIconFile;
    int     iDefIcon;
    BYTE    bOrder;
    DWORD   dwAttributes;
    LPCTSTR pszCPL;
} REQREGITEM;

typedef struct
{
    LPCTSTR pszAllow;
    RESTRICTIONS restAllow;
    LPCTSTR pszDisallow;
    RESTRICTIONS restDisallow;
} REGITEMSPOLICY;

typedef struct
{
    LPCTSTR             pszRegKey;
    REGITEMSPOLICY*     pPolicy;
    TCHAR               cRegItem;
    BYTE                bFlags;
    int                 iCmp;
    DWORD               rgfRegItems;
    int                 iReqItems;
    REQREGITEM const *  pReqItems;
    DWORD               dwSortAttrib;
    LPCTSTR             pszMachine;
    DWORD               cbPadding;
    BYTE                bFlagsLegacy;
} REGITEMSINFO;

REQREGITEM g_asDrivesReqItems[] =
{
    { &CLSID_ControlPanel, 0x1041, L"shell32.dll", -137, CONTROLS_SORT_INDEX, SFGAO_FOLDER | SFGAO_HASSUBFOLDER, NULL},
};

bool g_fAddControlPanelIcon = false;
bool g_fHideControlPanelIconFromDriveView = false;

HRESULT (STDCALL *CRegFolder_Initialize_orig)(void *, REGITEMSINFO *);
HRESULT STDCALL CRegFolder_Initialize_hook(void *pThis, REGITEMSINFO *prif)
{
    if (!wcscmp(REGSTR_PATH_EXPLORER_COMPUTER_NAMESPACE, prif->pszRegKey))
    {
        prif->iCmp = -1;
        if (g_settings.fAddCPL != ADDCPL_NONE)
        {
            g_asDrivesReqItems->pclsid = g_settings.fAddCPL == ADDCPL_CLASSIC ? &CLSID_ControlPanelClassic : &CLSID_ControlPanel;
            prif->iReqItems = ARRAYSIZE(g_asDrivesReqItems);
            prif->pReqItems = g_asDrivesReqItems;
        }
    }

    return CRegFolder_Initialize_orig(pThis, prif);
}

HRESULT (STDCALL *CDrivesViewCallback_ShouldShow_orig)(void *, IShellFolder *, LPCITEMIDLIST, LPCITEMIDLIST);
HRESULT STDCALL CDrivesViewCallback_ShouldShow_hook(void *pThis, IShellFolder *psf, LPCITEMIDLIST pidlFolder, LPCITEMIDLIST pidlItem)
{
    HRESULT hr = CDrivesViewCallback_ShouldShow_orig(pThis, psf, pidlFolder, pidlItem);

    if (hr != S_FALSE && g_settings.fHideCPLFromThisPC)
    {
        IShellItem2 *psi2 = NULL;
        if (SUCCEEDED(SHCreateItemWithParent(nullptr, psf, pidlItem, IID_PPV_ARGS(&psi2))))
        {
            CLSID clsid;
            if (SUCCEEDED(psi2->GetCLSID(PKEY_NamespaceCLSID, &clsid)))
            {
                if (IsEqualCLSID(clsid, *g_asDrivesReqItems->pclsid))
                {
                    hr = S_FALSE;
                }
            }
            if (psi2)
                psi2->Release();
        }     
    }

    return hr;
}

void LoadSettings(void)
{
    PCWSTR szCplType = Wh_GetStringSetting(L"addcpltype"); 

    if (!wcscmp(szCplType, L"category"))
    {
        g_settings.fAddCPL = ADDCPL_CATEGORY;
    }
    else if (!wcscmp(szCplType, L"classic"))
    {
        g_settings.fAddCPL = ADDCPL_CLASSIC;
    }
    else
    {
        g_settings.fAddCPL = ADDCPL_NONE;
    }

    Wh_FreeStringSetting(szCplType);
    g_settings.fHideCPLFromThisPC = Wh_GetIntSetting(L"hidecplfrompc");
}

BOOL Wh_ModInit() 
{
    Wh_Log(L"Init");
    LoadSettings();

    HMODULE hWindowsStorage = LoadLibraryExW(L"windows.storage.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!hWindowsStorage) 
    {
        Wh_Log(L"Failed to load windows.storage.dll");
        return FALSE;
    }

    HMODULE hShell32 = LoadLibraryExW(L"shell32.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!hShell32) 
    {
        Wh_Log(L"Failed to load shell32.dll");
        return FALSE;
    }

    // windows.storage.dll
    WindhawkUtils::SYMBOL_HOOK windowsStorageHook
        {
            {
                L"public: virtual long " SSTDCALL " CRegFolder::Initialize(struct REGITEMSINFO const *)"
            },
            &CRegFolder_Initialize_orig,
            CRegFolder_Initialize_hook,
            false
        };


    WindhawkUtils::SYMBOL_HOOK shell32DllHook
    {
        {
            #ifdef _WIN64
            L"public: virtual long __cdecl CDrivesViewCallback::ShouldShow(struct IShellFolder *,struct _ITEMIDLIST_ABSOLUTE const *,struct _ITEMID_CHILD const __unaligned *)"
            #else
            L"public: virtual long __stdcall CDrivesViewCallback::ShouldShow(struct IShellFolder *,struct _ITEMIDLIST_ABSOLUTE const *,struct _ITEMID_CHILD const *)"
            #endif
        },
        &CDrivesViewCallback_ShouldShow_orig,
        CDrivesViewCallback_ShouldShow_hook,
        false
    };

    if (!WindhawkUtils::HookSymbols(hWindowsStorage, &windowsStorageHook, 1)) 
    {
        Wh_Log(L"Failed to hook windows.storage.dll");
        return FALSE;
    }

    if (!WindhawkUtils::HookSymbols(hShell32, &shell32DllHook, 1)) 
    {
        Wh_Log(L"Failed to hook shell32.dll");
        return FALSE;
    }

    return TRUE;
}

void Wh_ModSettingsChanged(void)
{
    LoadSettings();
}
