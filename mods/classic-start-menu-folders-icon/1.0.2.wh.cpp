// ==WindhawkMod==
// @id              classic-start-menu-folders-icon
// @name            Classic Start Menu Folders Icon
// @description     Change the default icon of Start Menu folders like Windows XP and before
// @version         1.0.2
// @author          xalejandro
// @github          https://github.com/tetawaves
// @include         *
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Classic Start Menu Folders Icon
Changes the default icon of Start Menu folders that was used in Windows XP and before.

![Open-Shell Menu comparison showing the icon that was used in Windows XP and before](https://i.imgur.com/rufM7Yh.png)

**NOTE:** This mod doesn't change the icon of Windows 10/11 Start Menu.
[Open-Shell](https://github.com/Open-Shell/Open-Shell-Menu) was used for the
screenshot.
*/
// ==/WindhawkModReadme==

#include <shlobj.h>
#include <windhawk_api.h>
#include <windhawk_utils.h>

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

#define SIID_PROG 36

#ifdef _WIN64
#   define CFSFolder_CFSFolder(pThis) (CFSFolder*)((char*)pThis - 56)
#else
#   define CFSFolder_CFSFolder(pThis) (CFSFolder*)((char*)pThis - 28)
#endif

struct CFSFolder {};
struct ICachedPrivateProfile {};
struct IDFOLDER {};

UINT (THISCALL *CFSFolder__GetCSIDL)(CFSFolder *);
HRESULT (THISCALL *CFSFolder__GetDesktopIniForItem)(CFSFolder *, ITEMIDLIST_RELATIVE *, void *);
IDFOLDER * (THISCALL *CFSFolder__IsValidID)(CFSFolder *, ITEMIDLIST_RELATIVE *);
BOOL (STDCALL *CPrivateProfile_GetString)(ICachedPrivateProfile *, PCWSTR, PCWSTR, int, int, LPWSTR *);

BOOL CheckCSIDL(UINT csidl) 
{
    switch (csidl) 
    {
        case CSIDL_STARTMENU:
        case CSIDL_COMMON_STARTMENU:
        case CSIDL_PROGRAMS:
        case CSIDL_COMMON_PROGRAMS:
            return TRUE;
    }
    return FALSE;
}

HRESULT (THISCALL *CFSFolder__CreateFileFolderDefExtIcon_orig)(CFSFolder *, ITEMID_CHILD *, IDFOLDER *, IBindCtx *, GUID *, void **);
HRESULT THISCALL CFSFolder__CreateFileFolderDefExtIcon_hook(CFSFolder *pThis, ITEMID_CHILD *pItemId, IDFOLDER *pIDFOLDER, IBindCtx *pIBindCtx, GUID *guid, void **ppv)
{
    HRESULT hr = CFSFolder__CreateFileFolderDefExtIcon_orig(pThis, pItemId, pIDFOLDER, pIBindCtx, guid, ppv);

    if (!CheckCSIDL(CFSFolder__GetCSIDL(pThis)))
    {
        return hr;
    }

    ICachedPrivateProfile* pICachedPrivateProfile = nullptr;
    LPWSTR folderString[MAX_PATH + 4];
    if (SUCCEEDED(CFSFolder__GetDesktopIniForItem(pThis, pItemId, &pICachedPrivateProfile)) && pICachedPrivateProfile)
    {
        if (!CPrivateProfile_GetString(pICachedPrivateProfile, L".ShellClassInfo", L"IconResource", 0, 2, folderString) ||
            !CPrivateProfile_GetString(pICachedPrivateProfile, L".ShellClassInfo", L"IconFile", 0, 2, folderString))
        {
            return hr;
        }
    }

    IDefaultExtractIconInit* pdxi = nullptr;
    SHCreateDefaultExtractIcon(IID_PPV_ARGS(&pdxi));
    if (pdxi)
    {
        pdxi->SetNormalIcon(nullptr, SIID_PROG);
        pdxi->SetOpenIcon(nullptr, SIID_PROG);

        pdxi->QueryInterface((IID)*guid, ppv);       
        pdxi->Release();
    }

    return hr;
}

HRESULT (STDCALL *CFSFolder_GetIconOf_orig)(CFSFolder *, ITEMIDLIST_RELATIVE *, UINT, int *);
HRESULT STDCALL CFSFolder_GetIconOf_hook(CFSFolder *pThis, ITEMIDLIST_RELATIVE *pItemId, UINT flags, int *pImageIndex)
{
    HRESULT hr = CFSFolder_GetIconOf_orig(pThis, pItemId, flags, pImageIndex);
    
    CFSFolder* pFSFolder = CFSFolder_CFSFolder(pThis);
    IDFOLDER* pIdFolder = CFSFolder__IsValidID(pFSFolder, pItemId);

    if (!CheckCSIDL(CFSFolder__GetCSIDL(pFSFolder)) || !pIdFolder)
    {
        return hr;
    }

    ICachedPrivateProfile* pICachedPrivateProfile = nullptr;
    LPWSTR folderString[MAX_PATH + 4];

    if (SUCCEEDED(CFSFolder__GetDesktopIniForItem(pFSFolder, pItemId, &pICachedPrivateProfile)) && pICachedPrivateProfile)
    {
        if (!CPrivateProfile_GetString(pICachedPrivateProfile, L".ShellClassInfo", L"IconResource", 0, 2, folderString) ||
            !CPrivateProfile_GetString(pICachedPrivateProfile, L".ShellClassInfo", L"IconFile", 0, 2, folderString))
        {
            return hr;
        }
        *pImageIndex = SIID_PROG;
    }

    return hr;
}

BOOL Wh_ModInit() 
{
    Wh_Log(L"Init");

    // windows.storage.dll
    const WindhawkUtils::SYMBOL_HOOK windowsStorageHooks[]
    {
        {
            {
                #ifdef _WIN64
                L"protected: long __cdecl CFSFolder::_CreateFileFolderDefExtIcon(struct _ITEMID_CHILD const __unaligned *,struct IDFOLDER const __unaligned *,struct IBindCtx *,struct _GUID const &,void * *)"
                #else
                L"protected: long __thiscall CFSFolder::_CreateFileFolderDefExtIcon(struct _ITEMID_CHILD const *,struct IDFOLDER const *,struct IBindCtx *,struct _GUID const &,void * *)"
                #endif
            },
            &CFSFolder__CreateFileFolderDefExtIcon_orig,
            CFSFolder__CreateFileFolderDefExtIcon_hook,
            false
        },
        {
            {
                #ifdef _WIN64
                L"public: virtual long __cdecl CFSFolder::GetIconOf(struct _ITEMID_CHILD const __unaligned *,unsigned int,int *)"
                #else
                L"public: virtual long __stdcall CFSFolder::GetIconOf(struct _ITEMID_CHILD const *,unsigned int,int *)"
                #endif
            },
            &CFSFolder_GetIconOf_orig,
            CFSFolder_GetIconOf_hook,
            false
        },
        {
            {
                L"protected: unsigned int " STHISCALL " CFSFolder::_GetCSIDL(void)"
            },
            &CFSFolder__GetCSIDL,
            nullptr,
            false
        },
        {
            {
                #ifdef _WIN64
                L"protected: long __cdecl CFSFolder::_GetDesktopIniForItem(struct _ITEMIDLIST_RELATIVE const __unaligned *,struct ICachedPrivateProfile * *)"
                #else
                L"protected: long __thiscall CFSFolder::_GetDesktopIniForItem(struct _ITEMIDLIST_RELATIVE const *,struct ICachedPrivateProfile * *)"
                #endif
            },
            &CFSFolder__GetDesktopIniForItem,
            nullptr,
            false
        },
        {
            {
                #ifdef _WIN64
                L"protected: struct IDFOLDER const __unaligned * __cdecl CFSFolder::_IsValidID(struct _ITEMIDLIST_RELATIVE const __unaligned *)"
                #else
                L"protected: struct IDFOLDER const * __thiscall CFSFolder::_IsValidID(struct _ITEMIDLIST_RELATIVE const *)"
                #endif
            },
            &CFSFolder__IsValidID,
            nullptr,
            false
        },
        {
            {
                L"public: virtual long " SSTDCALL " CPrivateProfile::GetString(unsigned short const *,unsigned short const *,unsigned short const *,enum CACHEDPRIVATEPROFILEFLAGS,unsigned short * *)"
            },
            &CPrivateProfile_GetString,
            nullptr,
            false
        }
    };

    HMODULE hWindowsStorage = LoadLibraryExW(L"windows.storage.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!hWindowsStorage) 
    {
        Wh_Log(L"Failed to load windows.storage.dll");
        return FALSE;
    }

    if (!WindhawkUtils::HookSymbols(hWindowsStorage, windowsStorageHooks,
                                    ARRAYSIZE(windowsStorageHooks))) 
    {
        Wh_Log(L"Failed to hook windows.storage.dll");
        return FALSE;
    }

    return TRUE;
}
