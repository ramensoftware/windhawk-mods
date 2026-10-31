// ==WindhawkMod==
// @id              explorer-open-folder-icon
// @name            Explorer Open Folder Icon
// @description     Restores the open folder icon in Explorer's icon and tree view
// @version         1.0.0
// @author          aubymori
// @github          https://github.com/aubymori
// @include         explorer.exe
// @compilerOptions -lcomctl32
// @architecture    x86-64
// @license         GPL-3.0
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Explorer Open Folder Icon
In Windows XP and before, File Explorer showed an open folder icon in the title bar and
for selected folders in the Folders tree. This mod restores that behavior to Windows 10.

**Before**:

![Before](https://raw.githubusercontent.com/aubymori/images/refs/heads/main/explorer-open-folder-icon/before.png)

**After**:

![After](https://raw.githubusercontent.com/aubymori/images/refs/heads/main/explorer-open-folder-icon/after.png)
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- replace_downloads: true
  $name: Replace Downloads Icon
  $description: Also replaces the Downloads folder's icon with an open folder icon.
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>
#include <shlobj.h>
#include <commctrl.h>
#include <commoncontrols.h>

bool g_fReplaceDownloads = false;

/* Replace the window icon with the open folder. */
int (*CShellBrowser__GetIconIndex_orig)(class CShellBrowser *);
int CShellBrowser__GetIconIndex_hook(class CShellBrowser *pThis)
{
    int index = CShellBrowser__GetIconIndex_orig(pThis);
    if (index == 3 || (g_fReplaceDownloads && index == 243))
    {
        return 4;
    }
    return index;
}

/* Replaces the selected icon on the namespace tree with the open folder icon.
   We need to actually ensure the icon is there first. */
void SetOpenFolderIcon(HWND hwndTree, LPTVITEMW ptvi)
{
    if (ptvi->mask & TVIF_IMAGE && (ptvi->iImage == 3 || (g_fReplaceDownloads && ptvi->iImage == 243)))
    {
        HIMAGELIST himl = TreeView_GetImageList(hwndTree, TVSIL_NORMAL);
        if (himl)
        {
            IImageList2 *pil2;
            if (SUCCEEDED(HIMAGELIST_QueryInterface(himl, IID_PPV_ARGS(&pil2))))
            {
                if (SUCCEEDED(pil2->ForceImagePresent(4, ILFIP_ALWAYS)))
                {
                    ptvi->mask |= TVIF_SELECTEDIMAGE;
                    ptvi->iSelectedImage = 4;
                }
                pil2->Release();
            }
        } 
    }
}

void (*CNscTree__OnGetDisplayInfo_orig)(class CNscTree *, LPNMTVDISPINFOEXW);
void CNscTree__OnGetDisplayInfo_hook(class CNscTree *pThis, LPNMTVDISPINFOEXW pnm)
{
    CNscTree__OnGetDisplayInfo_orig(pThis, pnm);
    SetOpenFolderIcon(pnm->hdr.hwndFrom, (LPTVITEMW)&pnm->item);
}

SUBCLASSPROC CNscTree_s_SubClassTreeWndProc_orig;
LRESULT CALLBACK CNscTree_s_SubClassTreeWndProc_hook(
    HWND      hwnd, 
    UINT      uMsg,
    WPARAM    wParam,
    LPARAM    lParam,
    UINT_PTR  uIdSubclass,
    DWORD_PTR dwRefData
)
{
    switch (uMsg)
    {
        case TVM_SETITEMW:
            SetOpenFolderIcon(hwnd, (LPTVITEMW)lParam);
            [[fallthrough]];
        default:
            return CNscTree_s_SubClassTreeWndProc_orig(hwnd, uMsg, wParam, lParam, uIdSubclass, dwRefData);
    }
}

const WindhawkUtils::SYMBOL_HOOK explorerFrameDllHooks[] = {
    {
        {
            L"private: int __cdecl CShellBrowser::_GetIconIndex(void)"
        },
        &CShellBrowser__GetIconIndex_orig,
        CShellBrowser__GetIconIndex_hook,
        false
    },
    {
        {
            L"private: void __cdecl CNscTree::_OnGetDisplayInfo(struct tagTVDISPINFOEXW *)"
        },
        &CNscTree__OnGetDisplayInfo_orig,
        CNscTree__OnGetDisplayInfo_hook,
        false
    },
    {
        {
            L"private: static __int64 __cdecl CNscTree::s_SubClassTreeWndProc(struct HWND__ *,unsigned int,unsigned __int64,__int64,unsigned __int64,unsigned __int64)"
        },
        &CNscTree_s_SubClassTreeWndProc_orig,
        CNscTree_s_SubClassTreeWndProc_hook,
        false
    }
};

void Wh_ModSettingsChanged(void)
{
    g_fReplaceDownloads = Wh_GetIntSetting(L"replace_downloads");
}

BOOL Wh_ModInit(void)
{
    Wh_ModSettingsChanged();

    HMODULE hExplorerFrame = LoadLibraryExW(L"ExplorerFrame.dll", NULL , LOAD_LIBRARY_SEARCH_SYSTEM32);
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

    return TRUE;
}