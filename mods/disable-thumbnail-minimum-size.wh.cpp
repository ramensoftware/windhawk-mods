// ==WindhawkMod==
// @id              disable-thumbnail-minimum-size
// @name            Disable Thumbnail Minimum Size
// @description     Disables the cutoff size used to display thumbnails in File Explorer.
// @name:pt         Desativar tamanho mínimo de miniaturas
// @description:pt  Desativa o tamanho limite usado para exibir miniaturas no Explorador de Arquivos.
// @name:es         Desactivar el tamaño mínimo de miniaturas
// @description:es  Desactiva el tamaño de corte utilizado para mostrar miniaturas en el Explorador de Archivos.
// @name:ja         サムネイルの最小サイズを無効化
// @description:ja  エクスプローラーでサムネイルを表示する際に使用される最小サイズの制限を無効にします。
// @version         1.3
// @author          Leymonaide
// @github          https://github.com/Leymonaide
// @twitter         https://twitter.com/Leym0naide
// @homepage        https://leymonaide.github.io/
// @include         *
// @compilerOptions -lshlwapi -lpropsys -lole32 -lntdll
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Disable Thumbnail Minimum Size

This mod disables the cutoff size used to display thumbnails in File Explorer.

Images are cut off by default at 20 pixels, which means that views such as the details view, which use 16 pixel icons,
will not show thumbnails for any items. The motivation behind this design choice is the extreme loss in quality making
items nearly unrecognizable. However, it's good to have the option to display things as you would wish.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- keep_folder_cutoff: true
  $name: Keep the minimum size cutoff for folders
  $description: >
    This may be preferable as it keeps a visually consistent appearance, as generated folder thumbnails look rather
    different compared to the 16x16 icon.
  $name:pt: Manter o limite mínimo de tamanho para pastas
  $description:pt: >
    Isso pode ser preferível, pois mantém uma aparência visualmente consistente, já que as miniaturas geradas para
    pastas parecem bastante diferentes em comparação com o ícone 16x16.
  $name:es: Mantener el límite mínimo de tamaño para carpetas
  $description:es: >
    Esto puede ser preferible, ya que mantiene una apariencia visualmente coherente, dado que las miniaturas generadas
    para carpetas se ven bastante diferentes en comparación con el icono de 16x16.
  $name:ja: フォルダーの最小サイズ制限を維持する
  $description:ja: >
    生成されたフォルダーのサムネイルは 16x16 のアイコンと比べて見た目が大きく異なるため、視覚的な一貫性を保ちたい場合にはこちらの
    設定が適している場合があります。
*/
// ==/WindhawkModSettings==

#include <initguid.h>
#include <wrl.h>
#include <shlobj.h>
#include <windhawk_utils.h>
#include <shlwapi.h>
#include <propsys.h>
#include <propvarutil.h>

using Microsoft::WRL::ComPtr;

EXTERN_C NTSYSAPI NTSTATUS NTAPI RtlGetVersion(PRTL_OSVERSIONINFOW lpVersionInformation);

bool g_fKeepFolderCutoff = true;

inline bool IsDirectoryItemType(LPCWSTR szType)
{
    return 0 == StrCmpIW(szType, L"Directory")
        || 0 == StrCmpIW(szType, L"Folder");
}

unsigned int (__fastcall *GetThumbnailCutoffFromType_orig_shell32)(LPCWSTR szType) = nullptr;
unsigned int __fastcall GetThumbnailCutoffFromType_hook_shell32(LPCWSTR szType)
{
    if (g_fKeepFolderCutoff && IsDirectoryItemType(szType))
    {
        return GetThumbnailCutoffFromType_orig_shell32(szType);
    }

    return 0;
}

unsigned int (__fastcall *GetThumbnailCutoffFromType_orig_storage)(LPCWSTR szType) = nullptr;
unsigned int __fastcall GetThumbnailCutoffFromType_hook_storage(LPCWSTR szType)
{
    if (g_fKeepFolderCutoff && IsDirectoryItemType(szType))
    {
        return GetThumbnailCutoffFromType_orig_storage(szType);
    }

    return 0;
}

#define ThumbnailCutoffLog Wh_Log

DEFINE_PROPERTYKEY(PKEY_ItemType, 0x28636aa6, 0x953d, 0x11d2, 0xb5, 0xd6, 0x00, 0xc0, 0x4f, 0xd9, 0x18, 0xd0, 0x0b);

HRESULT (__thiscall *CImageManager___GetItem_orig)(void *pThis, void *pImageStore, REFIID riid, void **ppvOut) = nullptr;

// Used for some locations, such as the downloads folder.
unsigned int (__thiscall *CImageManager___GetThumbnailCutoff_orig)(void *pThis, void *pImageStore) = nullptr;
unsigned int __thiscall CImageManager___GetThumbnailCutoff_hook(void *pThis, void *pImageStore)
{
    bool fIsExcluded = false;

    if (g_fKeepFolderCutoff)
    {
        ComPtr<IShellItem2> pShellItem;
        if (SUCCEEDED(CImageManager___GetItem_orig(pThis, pImageStore, IID_PPV_ARGS(&pShellItem))))
        {
            PROPVARIANT pvar;
            if (SUCCEEDED(pShellItem->GetProperty(PKEY_ItemType, &pvar)))
            {
                LPCWSTR pszItemType = nullptr;
                if (nullptr != (pszItemType = PropVariantToStringWithDefault(pvar, nullptr)))
                {
                    ThumbnailCutoffLog(L"Item type: %s", pszItemType);
                    if (IsDirectoryItemType(pszItemType))
                    {
                        fIsExcluded = true;
                        ThumbnailCutoffLog(L"Found a folder type, which will be excluded.");
                    }
                }
                else
                {
                    ThumbnailCutoffLog(L"No string exists for the item type property.");
                }
                PropVariantClear(&pvar);
            }
            else
            {
                ThumbnailCutoffLog(L"Failed to get PKEY_ItemType");
            }
        }
    }

    if (fIsExcluded)
    {
        // If the type is excluded, then we'll call the original function to
        // query the proper cutoff size.
        return CImageManager___GetThumbnailCutoff_orig(pThis, pImageStore);
    }
    else
    {
        // Otherwise we'll return 0 which disables the cutoff size.
        return 0;
    }
}

// shell32.dll
WindhawkUtils::SYMBOL_HOOK shell32Hooks[] =
{
    {
        {
#ifdef _WIN64
            L"unsigned int __cdecl GetThumbnailCutoffFromType(unsigned short const *)",
#else
            L"unsigned int __stdcall GetThumbnailCutoffFromType(unsigned short const *)",
#endif
        },
        &GetThumbnailCutoffFromType_orig_shell32,
        GetThumbnailCutoffFromType_hook_shell32,
    },
};

// windows.storage.dll 
WindhawkUtils::SYMBOL_HOOK windowsStorageHooks[] =
{
    {
        {
#ifdef _WIN64
            L"unsigned int __cdecl GetThumbnailCutoffFromType(unsigned short const *)",
#else
            L"unsigned int __stdcall GetThumbnailCutoffFromType(unsigned short const *)",
#endif
        },
        &GetThumbnailCutoffFromType_orig_storage,
        GetThumbnailCutoffFromType_hook_storage,
    },
    {
        {
#ifdef _WIN64
            L"private: unsigned int __cdecl CImageManager::_GetThumbnailCutoff(struct IItemImageStore *)",
#else
            L"private: unsigned int __thiscall CImageManager::_GetThumbnailCutoff(struct IItemImageStore *)",
#endif
        },
        &CImageManager___GetThumbnailCutoff_orig,
        CImageManager___GetThumbnailCutoff_hook,
    },
    {
        {
#ifdef _WIN64
            L"private: long __cdecl CImageManager::_GetItem(struct IItemImageStore *,struct _GUID const &,void * *)",
#else
            L"private: long __thiscall CImageManager::_GetItem(struct IItemImageStore *,struct _GUID const &,void * *)",
#endif
        },
        &CImageManager___GetItem_orig,
    },
};

void LoadSettings()
{
    g_fKeepFolderCutoff = Wh_GetIntSetting(L"keep_folder_cutoff") ? true : false;
}

// The mod is being initialized, load settings, hook functions, and do other
// initialization stuff if required.
BOOL Wh_ModInit() {
    Wh_Log(L"Init");

    LoadSettings();

    RTL_OSVERSIONINFOW osvi;
    RtlGetVersion(&osvi);

    HMODULE shell32 = LoadLibraryExW(L"shell32.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    HMODULE windowsStorageDll = LoadLibraryExW(L"windows.storage.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);

    if (!shell32)
    {
        Wh_Log(L"Failed to load shell32.dll.");
        return FALSE;
    }

    // The duplicate functions were removed from shell32 in Windows 11, so we
    // will only attempt to hook them on Windows 10.
    if (osvi.dwBuildNumber < 26100)
    {
        if (!WindhawkUtils::HookSymbols(shell32, shell32Hooks, ARRAYSIZE(shell32Hooks)))
        {
            // This failure is non-fatal as the functionality is mostly
            // contained in windows.storage.dll. shell32 is hooked for complete
            // coverage, but it's not too worrying if the hooks failed.
            Wh_Log(L"Nonfatal: Failed to hook symbols in shell32.dll.");
        }
    }

    if (!WindhawkUtils::HookSymbols(windowsStorageDll, windowsStorageHooks, ARRAYSIZE(windowsStorageHooks)))
    {
        Wh_Log(L"Failed to hook symbols in windows.storage.dll.");
        return FALSE;
    }

    return TRUE;
}

// The mod is being unloaded, free all allocated resources.
void Wh_ModUninit() {
    Wh_Log(L"Uninit");
}

void Wh_ModSettingsChanged()
{
    LoadSettings();
}
