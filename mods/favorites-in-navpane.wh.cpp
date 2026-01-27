// ==WindhawkMod==
// @id              favorites-in-navpane
// @name            Favorites in Navigation Pane
// @description     Replaces Windows 10 Quick Access with Favorites in Navigation Pane
// @version         1.0.3
// @author          xalejandro
// @github          https://github.com/tetawaves
// @include         *
// @compilerOptions -lcomctl32 -lshlwapi -lntdll -lole32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Favorites in Navigation Pane
Replaces Quick Access with Favorites in Navigation Pane.  
Restarting Explorer is recommended after installing the mod.

![Favorites in Navigation Pane showing in Windows 10](https://i.imgur.com/UzRUf2P.png)

**NOTE:** Only works on Windows 10.
*/
// ==/WindhawkModReadme==

#include <windhawk_api.h>
#include <windhawk_utils.h>
#include <Shlobj.h>
#include <shlwapi.h>
#include <initguid.h>
#include <ntdef.h>
#include <ntstatus.h>

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

typedef enum _KEY_INFORMATION_CLASS {
    KeyBasicInformation,
    KeyNodeInformation,
    KeyFullInformation,
    KeyNameInformation,
    KeyCachedInformation,
    KeyFlagsInformation,
    KeyVirtualizationInformation,
    KeyHandleTagsInformation,
    KeyTrustInformation,
    KeyLayerInformation,
    MaxKeyInfoClass
} KEY_INFORMATION_CLASS;

EXTERN_C NTSYSAPI NTSTATUS NTAPI NtQueryKey(
    IN HANDLE KeyHandle,
    IN KEY_INFORMATION_CLASS KeyInformationClass,
    OUT PVOID KeyInformation,
    IN ULONG Length,
    OUT PULONG ResultLength
);

DEFINE_GUID(IID_IShellItem, 0x43826d1e, 0xe718, 0x42ee, 0xbc,0x55, 0xa1,0xe2,0x61,0xc3,0x7b,0xfe);

typedef struct _DPA {
    int cpItems;
    PVOID *pArray;
    HANDLE hHeap;
    int cpCapacity;
    int cpGrow;
} DPA, *HDPA; 

bool EndsWith(const wchar_t *str, const wchar_t *suffix)
{
    if (!str || !suffix)
        return false;
    size_t lenstr = wcslen(str);
    size_t lensuffix = wcslen(suffix);
    if (lensuffix >  lenstr)
        return 0;
    
    return 0 == wcsnicmp(str + lenstr - lensuffix, suffix, lensuffix);
}

HRESULT WriteOrderRegistry(IStream *pStream)
{
    ULARGE_INTEGER streamSize;
    HRESULT hr = IStream_Size(pStream, &streamSize);    
    if (SUCCEEDED(hr))
    {
        LARGE_INTEGER zero;
        hr = pStream->Seek(zero, STREAM_SEEK_SET, nullptr);
        if (SUCCEEDED(hr))
        {
            BYTE *lpData = new BYTE[streamSize.QuadPart];
            hr = pStream->Read(lpData, streamSize.QuadPart, nullptr);
            if (SUCCEEDED(hr)) 
            {
                HKEY hKey;
                hr = HRESULT_FROM_WIN32(RegOpenKeyExW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Modules\\CommonPlaces\\", 0, KEY_SET_VALUE, &hKey));
                if (SUCCEEDED(hr))
                {
                    hr = HRESULT_FROM_WIN32(RegSetValueExW(hKey, L"Order", 0, REG_BINARY, lpData, streamSize.QuadPart));
                    RegCloseKey(hKey);
                }
            }
            delete[] lpData;
        }
    }

    return hr;
}

BOOL IsIShellItemFavorites(IShellItem *pIShellItem)
{
    BOOL isFavorites = FALSE;
    
    if (pIShellItem)
    {
        LPWSTR pszName;
        pIShellItem->GetDisplayName(SIGDN_PARENTRELATIVEPARSING, &pszName);
        if (pszName)
        {
            isFavorites = !wcscmp(L"::{323CA680-C24D-4099-B94D-446DD2D7249E}", pszName);
            CoTaskMemFree(pszName);
        }
    }

    return isFavorites;    
}

// https://github.com/ramensoftware/windhawk-mods/blob/main/mods/spoof-light-dark-theme.wh.cpp
using RegQueryValueExW_t = decltype(&RegQueryValueExW);
RegQueryValueExW_t RegQueryValueExW_orig;
LSTATUS WINAPI RegQueryValueExW_hook(
    HKEY    hKey,
    LPCWSTR lpValueName,
    LPDWORD lpReserved,
    LPDWORD lpType,
    LPBYTE  lpData,
    LPDWORD lpcbData
)
{
    LSTATUS lStatus = RegQueryValueExW_orig(hKey, lpValueName, lpReserved, lpType, lpData, lpcbData);
    if (ERROR_SUCCESS == lStatus && lpData && lpValueName && 0 == wcsicmp(lpValueName, L"Attributes"))
    {
        ULONG ulSize = 0;
        NTSTATUS status = NtQueryKey(hKey, KeyNameInformation, nullptr, 0, &ulSize);
        if (status == STATUS_BUFFER_TOO_SMALL)
        {
            ulSize += 2;
            LPWSTR lpBuffer = new WCHAR[ulSize / sizeof(WCHAR)];
            if (lpBuffer)
            {
                status = NtQueryKey(hKey, KeyNameInformation, lpBuffer, ulSize, &ulSize);
                if (status == STATUS_SUCCESS)
                {
                    LPCWSTR lpKeyName = lpBuffer + 2;
                    lpBuffer[ulSize / sizeof(WCHAR)] = L'\0';
                    if (EndsWith(lpKeyName, L"\\CLSID\\{323CA680-C24D-4099-B94D-446DD2D7249E}\\ShellFolder"))
                    {
                        // Fix duplicated favorites
                        *(LPDWORD)lpData = 0xA0900100;
                    }
                    if (EndsWith(lpKeyName, L"\\CLSID\\{22877A6D-37A1-461A-91B0-DBDA5AAEBC99}\\ShellFolder"))
                    {
                        // Fix recent folders pin
                        *(LPDWORD)lpData = 0x70010020;
                    }
                }
                delete[] lpBuffer;
            }
        }
    }

    return lStatus;
}

using SHCreateItemFromParsingName_t = decltype(&SHCreateItemFromParsingName);
SHCreateItemFromParsingName_t SHCreateItemFromParsingName_orig;
HRESULT WINAPI SHCreateItemFromParsingName_hook(PCWSTR pszPath, IBindCtx *pbc, const IID &riid, void **ppv)
{
    if (pszPath && !wcscmp(L"shell:::{679f85cb-0220-4080-b29b-5540cc05aab6}", pszPath) && pbc == NULL && riid == IID_IShellItem)
    {
        Wh_Log(L"%s", pszPath);
        pszPath = L"shell:::{323CA680-C24D-4099-B94D-446DD2D7249E}";
    }

    return SHCreateItemFromParsingName_orig(pszPath, pbc, riid, ppv);
}

HRESULT (STDCALL *CProperTreeHost_OnDragPosition_orig)(IShellItem **, IShellItem *, IShellItemArray *, int, int);
HRESULT STDCALL CProperTreeHost_OnDragPosition_hook(IShellItem **ppIShellItem, IShellItem *pIShellItem, IShellItemArray *pIShellItemArray, int a4, int a5)
{
    HRESULT hr = CProperTreeHost_OnDragPosition_orig(ppIShellItem, pIShellItem, pIShellItemArray, a4, a5);
    if (IsIShellItemFavorites(pIShellItem))
    {
        return 0;
    }

    return hr;
}

HRESULT (STDCALL *CProperTreeHost_OnDrop_orig)(void *, IShellItem *, IShellItemArray*, int);
HRESULT STDCALL CProperTreeHost_OnDrop_hook(void *pThis, IShellItem *pIShellItem, IShellItemArray* a3, int a4)
{
    HRESULT hr = CProperTreeHost_OnDrop_orig(pThis, pIShellItem, a3, a4);
    if (IsIShellItemFavorites(pIShellItem))
    {
        return 0;
    }

    return hr;
}

HRESULT (STDCALL *CProperTreeHost_OnDropPosition_orig)(void *, IShellItem *, IShellItemArray *, int, int);
HRESULT STDCALL CProperTreeHost_OnDropPosition_hook(void *pThis, IShellItem *pIShellItem, IShellItemArray *pIShellItemArray, int a4, int a5)
{
    HRESULT hr = CProperTreeHost_OnDropPosition_orig(pThis, pIShellItem, pIShellItemArray , a4, a5);
    if (IsIShellItemFavorites(pIShellItem))
    {
        return 0;
    }

    return hr;
}


HRESULT (STDCALL *CProperTreeHost_GetOrderStream_orig)(void *, IShellItem *, DWORD, IStream **);
HRESULT STDCALL CProperTreeHost_GetOrderStream_hook(void *pThis, IShellItem *pIShellItem, DWORD grfMode, IStream **ppStream)
{
    if (IsIShellItemFavorites(pIShellItem))
    {
        *ppStream = SHOpenRegStream2W(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Modules\\CommonPlaces\\", L"Order", grfMode);
        return 0;
    }

    return CProperTreeHost_GetOrderStream_orig(pThis, pIShellItem, grfMode, ppStream);
}

HRESULT (STDCALL *CProperTreeHost_PersistChildItemsOrder_orig)(void *, int, IShellItem *);
HRESULT STDCALL CProperTreeHost_PersistChildItemsOrder_hook(void *pThis, int a2, IShellItem *pIShellItem)
{
    HRESULT hr = CProperTreeHost_PersistChildItemsOrder_orig(pThis, a2, pIShellItem);
    if (IsIShellItemFavorites(pIShellItem))
    {
        return E_NOTIMPL;
    }

    return hr;
}

HRESULT (THISCALL *CNscTree_InsertRoot_orig)(void *pThis, ITEMIDLIST *, IShellItem *, unsigned long a3, unsigned long a4, IShellItemFilter *pIShellItemFilter);
HRESULT THISCALL CNscTree_InsertRoot_hook(void *pThis, ITEMIDLIST *pITEMIDLIST, IShellItem *pIShellItem, unsigned long a3, unsigned long a4, IShellItemFilter *pIShellItemFilter)
{
    if (IsIShellItemFavorites(pIShellItem))
    {
        a3 = 64;
        pIShellItemFilter = nullptr;
    }
            
    return CNscTree_InsertRoot_orig(pThis, pITEMIDLIST, pIShellItem, a3, a4, pIShellItemFilter);
}

HRESULT (__fastcall *OrderList_SaveToStream_orig)(IStream *, HDPA, IShellFolder *);
HRESULT __fastcall OrderList_SaveToStream_hook(IStream *pStream, HDPA hdpa, IShellFolder *pIShellFolder)
{
    // For some reason the first one is always empty
    if (!hdpa->pArray[0])
    {
        DPA_DeletePtr(hdpa, 0);
    }

    HRESULT hr = OrderList_SaveToStream_orig(pStream, hdpa, pIShellFolder);

    if (FAILED(WriteOrderRegistry(pStream)))
    {
        Wh_Log(L"Failed to write order list into registry");
    }

    return hr;
}

BOOL Wh_ModInit() {
    Wh_Log(L"Init");

    HMODULE hShell32 = LoadLibraryExW(L"shell32.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!hShell32) 
    {
        Wh_Log(L"Failed to load shell32.dll");
        return FALSE;
    }

    HMODULE hExplorerFrame = LoadLibraryExW(L"ExplorerFrame.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!hExplorerFrame) 
    {
        Wh_Log(L"Failed to load ExplorerFrame.dll");
        return FALSE;
    }

    const WindhawkUtils::SYMBOL_HOOK shell32DllHooks[]
    {
        {
            {
                L"public: virtual long " SSTDCALL " CProperTreeHost::OnDragPosition(struct IShellItem *,struct IShellItemArray *,int,int)"
            },
            &CProperTreeHost_OnDragPosition_orig,
            CProperTreeHost_OnDragPosition_hook,
            false
        },
        {
            {
                L"public: virtual long " SSTDCALL " CProperTreeHost::OnDrop(struct IShellItem *,struct IShellItemArray *,int,unsigned long,unsigned long *)"
            },
            &CProperTreeHost_OnDrop_orig,
            CProperTreeHost_OnDrop_hook,
            false
        },
        {
            {
                L"public: virtual long " SSTDCALL " CProperTreeHost::OnDropPosition(struct IShellItem *,struct IShellItemArray *,int,int)"
            },
            &CProperTreeHost_OnDropPosition_orig,
            CProperTreeHost_OnDropPosition_hook,
            false
        },
        {
            {
                L"public: virtual long " SSTDCALL " CProperTreeHost::GetOrderStream(struct IShellItem *,unsigned long,struct IStream * *)"
            },
            &CProperTreeHost_GetOrderStream_orig,
            CProperTreeHost_GetOrderStream_hook,
            false
        },
        {
            {
                L"public: virtual long " SSTDCALL " CProperTreeHost::PersistChildItemsOrder(int,struct IShellItem *)"
            },
            &CProperTreeHost_PersistChildItemsOrder_orig,
            CProperTreeHost_PersistChildItemsOrder_hook,
            false
        }
    };

    const WindhawkUtils::SYMBOL_HOOK explorerFrameDllHooks[]
    {
        {
            {
                #ifdef _WIN64
                L"private: long " STHISCALL " CNscTree::_InsertRoot(struct _TREEITEM *,struct IShellItem *,unsigned long,unsigned long,struct IShellItemFilter *)"
                #else
                L"public: virtual long __stdcall CNscTree::AppendRoot(struct IShellItem *,unsigned long,unsigned long,struct IShellItemFilter *)"
                #endif
            },
            &CNscTree_InsertRoot_orig,
            CNscTree_InsertRoot_hook,
            false
        },
        {
            {
                L"long " SSTDCALL " OrderList_SaveToStream(struct IStream *,struct _DPA *,struct IShellFolder *)"
            },
            &OrderList_SaveToStream_orig,
            OrderList_SaveToStream_hook,
            false
        },
    };


    if (!WindhawkUtils::HookSymbols(hShell32, shell32DllHooks,
                                    ARRAYSIZE(shell32DllHooks))) 
    {
        Wh_Log(L"Failed to hook shell32.dll");
        return FALSE;
    }

    if (!WindhawkUtils::HookSymbols(hExplorerFrame, explorerFrameDllHooks,
                                    ARRAYSIZE(explorerFrameDllHooks))) 
    {
        Wh_Log(L"Failed to hook ExplorerFrame.dll");
        return FALSE;
    }

    Wh_SetFunctionHook((void*)SHCreateItemFromParsingName, (void*)SHCreateItemFromParsingName_hook,
                       (void**)&SHCreateItemFromParsingName_orig);

    Wh_SetFunctionHook((void*)GetProcAddress(LoadLibrary(L"kernelbase.dll"), "RegQueryValueExW"), 
                       (void*)RegQueryValueExW_hook, (void**)&RegQueryValueExW_orig);

    return TRUE;
}
