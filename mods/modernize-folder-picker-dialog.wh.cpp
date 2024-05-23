// ==WindhawkMod==
// @id              modernize-folder-picker-dialog
// @name            Modernize Folder Picker Dialog
// @description     Replaces the classic "Browse For Folder" dialog
// @version         1.0.0
// @author          aubymori
// @github          https://github.com/aubymori
// @include         *
// @compilerOptions -lole32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Modernize Folder Picker Dialog

This mod replaces the outdated, hard-to-use "Browse For Folder" dialog with a
modern styled folder picker dialog, which is easier to navigate.

**Before**:

![Before](https://raw.githubusercontent.com/aubymori/images/main/modernize-folder-picker-dialog-before.png)

**After**:
![After](https://raw.githubusercontent.com/aubymori/images/main/modernize-folder-picker-dialog-after.png)
*/
// ==/WindhawkModReadme==

#define RETURN_IF_FAILED(hr) if (FAILED(hr)) return SHBrowseForFolderW_orig(lpbi)

#define E_CANCELLED (HRESULT)0x800704C7

#include <initguid.h>
#include <shlobj.h>
#include <windhawk_utils.h>

using SHBrowseForFolderW_t = decltype(&SHBrowseForFolderW);
SHBrowseForFolderW_t SHBrowseForFolderW_orig = nullptr;
PIDLIST_ABSOLUTE WINAPI SHBrowseForFolderW_hook(
    LPBROWSEINFOW lpbi
)
{
    if (!lpbi) return NULL;

    /* Make the dialog */
    IFileOpenDialog *pDialog = nullptr;
    RETURN_IF_FAILED(CoCreateInstance(
        CLSID_FileOpenDialog,
        NULL,
        CLSCTX_INPROC_SERVER,
        IID_PPV_ARGS(&pDialog)
    ));

    /* Make it pick a folder instead of a file */
    RETURN_IF_FAILED(pDialog->SetOptions(
        FOS_PICKFOLDERS | FOS_PATHMUSTEXIST | FOS_FILEMUSTEXIST
    ));

    if (lpbi->lpszTitle && *lpbi->lpszTitle)
    {
        RETURN_IF_FAILED(pDialog->SetTitle(lpbi->lpszTitle));
    }

    /* Tell it where to start */
    if (lpbi->pidlRoot)
    {
        /* We have to convert from PIDLIST_ABSOLUTE -> IShellItem * */
        IShellItem *pRoot = nullptr;
        RETURN_IF_FAILED(SHCreateItemFromIDList(
            lpbi->pidlRoot,
            IID_PPV_ARGS(&pRoot)
        ));
        RETURN_IF_FAILED(pDialog->SetFolder(pRoot));
    }

    HRESULT hr = pDialog->Show(lpbi->hwndOwner);
    if (FAILED(hr))
    {
        /* For whatever reason, the dialog being cancelled counts as a fail */
        if (hr != E_CANCELLED)
        {
            return SHBrowseForFolderW_orig(lpbi);
        }
        else
        {
            return NULL;
        }
    }

    IShellItem *pResult = nullptr;
    RETURN_IF_FAILED(pDialog->GetResult(&pResult));

    LPWSTR pszPath = nullptr;
    RETURN_IF_FAILED(pResult->GetDisplayName(SIGDN_FILESYSPATH, &pszPath));

    /* Convert from wide character string to PIDLIST_ABSOLUTE */
    if (pszPath) return ILCreateFromPathW(pszPath);

    return NULL;//SHBrowseForFolderW_orig(lpbi);
}

BOOL Wh_ModInit(void)
{
    if (!WindhawkUtils::Wh_SetFunctionHookT(
        SHBrowseForFolderW,
        SHBrowseForFolderW_hook,
        &SHBrowseForFolderW_orig
    ))
    {
        Wh_Log(L"Failed to hook SHBrowseForFolderW");
        return FALSE;
    }

    return TRUE;
}