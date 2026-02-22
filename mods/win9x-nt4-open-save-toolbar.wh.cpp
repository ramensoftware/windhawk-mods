// ==WindhawkMod==
// @id              win9x-nt4-open-save-toolbar
// @name            Windows 95/98/NT 4.0 Open/Save Toolbar
// @description     Modifies the classic open/save dialog to be more like Windows 98 and earlier
// @version         1.0.0
// @author          aubymori
// @github          https://github.com/aubymori
// @include         *
// @compilerOptions -lcomdlg32 -lcomctl32 -lpsapi
// @license         GPL-3.0
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Windows 95/98/NT 4.0 Open/Save Dialog
This mod replaces the classic open/save dialog's toolbar items with those from Windows
95, 98, or NT 4.0.

If you want Windows 98's Deskop item, you will need a copy of Windows 98's comdlg32.dll.
Once you have that, provide a path to it in the mod options.

You can disable the Places Bar through [other means](https://www.tenforums.com/tutorials/126103-enable-disable-places-bar-common-dialog-box-windows.html).

**NOTE**: This mod only applies to the *classic* open/save dialogs. To see these changes everywhere,
you will need [this mod](https://windhawk.net/mods/classic-file-picker-dialog).

**Before**:

![Before](https://raw.githubusercontent.com/aubymori/images/refs/heads/main/win9x-nt4-open-save-dialog/before.png)

**After**:

![After](https://raw.githubusercontent.com/aubymori/images/refs/heads/main/win9x-nt4-open-save-dialog/after.png)
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- comdlg_98_path: C:\path\to\comdlg32.dll
  $name: Windows 98 comdlg32.dll path
  $description: 
    Path to Windows 98's version of comdlg32.dll, necessary for the icon of the
    Desktop toolbar button. If an invalid path is provided, the desktop icon will
    be omitted.
*/
// ==/WindhawkModSettings==

#include <initguid.h>
#include <windhawk_utils.h>
#include <knownfolders.h>
#include <shlobj.h>
#include <psapi.h>

ULONGLONG g_uCommDlgBase  = 0;
ULONGLONG g_uCommDlgEnd   = 0;
HMODULE   g_hmodCommDlg98 = NULL;

//CDGetAppCompatFlags
#define CDACF_MATHCAD             0x00000001
#define CDACF_NT40TOOLBAR         0x00000002
#define CDACF_FILETITLE           0x00000004

DWORD (WINAPI *CDGetAppCompatFlags)();

#define FCIDM_BROWSERFIRST          0xa000
#define IDC_PARENT           (FCIDM_BROWSERFIRST + 1)
#define IDC_NEWFOLDER        (FCIDM_BROWSERFIRST + 2)
#define IDC_VIEWLIST         (FCIDM_BROWSERFIRST + 3)
#define IDC_VIEWDETAILS      (FCIDM_BROWSERFIRST + 4)
#define IDC_JUMPDESKTOP      (FCIDM_BROWSERFIRST + 9)

#define IDB_JUMPDESKTOP      800

// This is 12 in the original implementation, but has
// since been removed and shifted by other bitmaps.
#define VIEW_JUMPDESKTOP     12

TBBUTTON atbButtons95[] =
{
    { 0, 0, 0, BTNS_SEP, { 0, 0 }, 0, 0 },
    { VIEW_PARENTFOLDER, IDC_PARENT, TBSTATE_ENABLED, BTNS_BUTTON, { 0, 0 }, 0, -1 },
    { 0, 0, 0, BTNS_SEP, { 0, 0 }, 0, 0 },
    { VIEW_NEWFOLDER, IDC_NEWFOLDER, TBSTATE_ENABLED, BTNS_BUTTON, { 0, 0 }, 0, -1 },
    { 0, 0, 0, BTNS_SEP, { 0, 0 }, 0, 0 },
    { VIEW_LIST,    IDC_VIEWLIST,    TBSTATE_ENABLED | TBSTATE_CHECKED, BTNS_CHECKGROUP, { 0, 0 }, 0, -1 },
    { VIEW_DETAILS, IDC_VIEWDETAILS, TBSTATE_ENABLED,                   BTNS_CHECKGROUP, { 0, 0 }, 0, -1 }
};

TBBUTTON atbButtons98[] =
{
    { 0, 0, 0, BTNS_SEP, { 0, 0 }, 0, 0 },
    { VIEW_PARENTFOLDER, IDC_PARENT, TBSTATE_ENABLED, BTNS_BUTTON, { 0, 0 }, 0, -1 },
    { 0, 0, 0, BTNS_SEP, { 0, 0 }, 0, 0 },
    { VIEW_JUMPDESKTOP, IDC_JUMPDESKTOP, TBSTATE_ENABLED, BTNS_BUTTON, { 0, 0 }, 0, -1 },
    { 0, 0, 0, BTNS_SEP, { 0, 0 }, 0, 0 },
    { VIEW_NEWFOLDER, IDC_NEWFOLDER, TBSTATE_ENABLED, BTNS_BUTTON, { 0, 0 }, 0, -1 },
    { 0, 0, 0, BTNS_SEP, { 0, 0 }, 0, 0 },
    { VIEW_LIST,    IDC_VIEWLIST,    TBSTATE_ENABLED | TBSTATE_CHECKED, BTNS_CHECKGROUP, { 0, 0 }, 0, -1 },
    { VIEW_DETAILS, IDC_VIEWDETAILS, TBSTATE_ENABLED,                   BTNS_CHECKGROUP, { 0, 0 }, 0, -1 }
};

using CreateToolbarEx_t = decltype(&CreateToolbarEx);
CreateToolbarEx_t CreateToolbarEx_orig;
HWND WINAPI CreateToolbarEx_hook(
    HWND        hwnd,
    DWORD       ws,
    UINT        wID,
    int         nBitmaps,
    HINSTANCE   hBMInst,
    UINT_PTR    wBMID,
    LPCTBBUTTON lpButtons,
    int         iNumButtons,
    int         dxButton,
    int         dyButton,
    int         dxBitmap,
    int         dyBitmap,
    UINT        uStructSize)
{
    ULONGLONG ulRetAddr = (ULONGLONG)__builtin_return_address(0);
    if ((ulRetAddr >= g_uCommDlgBase) && (ulRetAddr < g_uCommDlgEnd)
    && !(CDGetAppCompatFlags() & CDACF_NT40TOOLBAR))
    {
        ws &= ~TBSTYLE_FLAT;
        
        if (g_hmodCommDlg98)
        {
            Wh_Log(L"Setting 98 buttons...");
            lpButtons = atbButtons98;
            iNumButtons = ARRAYSIZE(atbButtons98);

            HWND hwndToolbar = CreateToolbarEx_orig(
                hwnd,
                ws,
                wID,
                nBitmaps,
                hBMInst,
                wBMID,
                lpButtons,
                iNumButtons,
                dxButton,
                dyButton,
                dxBitmap,
                dyBitmap,
                uStructSize);
            if (hwndToolbar)
            {
                TBADDBITMAP tbab;
                HBITMAP hbmDesktop = (HBITMAP)LoadImageW(
                    g_hmodCommDlg98,
                    MAKEINTRESOURCEW(IDB_JUMPDESKTOP),
                    IMAGE_BITMAP,
                    16, 16,
                    LR_LOADMAP3DCOLORS);
                if (hbmDesktop)
                {
                    tbab.hInst = NULL;
                    tbab.nID = (UINT_PTR)hbmDesktop;
                }
                else
                {
                    tbab.hInst = g_hmodCommDlg98;
                    tbab.nID = IDB_JUMPDESKTOP;
                }
                SendMessageW(hwndToolbar, TB_ADDBITMAP, 1, (LPARAM)&tbab);
            }
            return hwndToolbar;
        }
        else
        {
            Wh_Log(L"Setting 95 buttons...");
            lpButtons = atbButtons95;
            iNumButtons = ARRAYSIZE(atbButtons95);
        }
    }
    return CreateToolbarEx_orig(
        hwnd,
        ws,
        wID,
        nBitmaps,
        hBMInst,
        wBMID,
        lpButtons,
        iNumButtons,
        dxButton,
        dyButton,
        dxBitmap,
        dyBitmap,
        uStructSize);
}

LRESULT CALLBACK DefViewSubclassProc(
    HWND      hwnd,
    UINT      uMsg,
    WPARAM    wParam,
    LPARAM    lParam,
    DWORD_PTR dwRefData
)
{
    switch (uMsg)
    {
        case WM_COMMAND:
        {
            UINT idCmd = LOWORD(wParam);
            if (idCmd >= 28747 && idCmd <= 28754)
            {
                Wh_Log(L"View command hit: %u", idCmd);
                HWND hwndToolbar = (HWND)dwRefData;
                SendMessageW(hwndToolbar, TB_CHECKBUTTON, IDC_VIEWLIST,    idCmd == 28753);
                SendMessageW(hwndToolbar, TB_CHECKBUTTON, IDC_VIEWDETAILS, idCmd == 28747);
            }
        }
        default:
            return DefSubclassProc(hwnd, uMsg, wParam, lParam);
    }
}

thread_local HWND g_hwndOpenSaveDlg = NULL;

void OnViewChange()
{
    HWND hwndDefView = FindWindowExW(g_hwndOpenSaveDlg, NULL, L"SHELLDLL_DefView", nullptr);
    if (hwndDefView)
    {
        HWND hwndToolbar = FindWindowExW(g_hwndOpenSaveDlg, NULL, TOOLBARCLASSNAMEW, nullptr);
        if (hwndToolbar)
        {
            WindhawkUtils::SetWindowSubclassFromAnyThread(
                hwndDefView, DefViewSubclassProc, (DWORD_PTR)hwndToolbar);
            
            HWND hwndListView = FindWindowExW(hwndDefView, NULL, WC_LISTVIEWW, nullptr);
            if (hwndListView)
            {
                Wh_Log(L"Setting initial view");
                int iView = ListView_GetView(hwndListView);
                SendMessageW(hwndToolbar, TB_CHECKBUTTON, IDC_VIEWLIST,    iView == LV_VIEW_LIST);
                SendMessageW(hwndToolbar, TB_CHECKBUTTON, IDC_VIEWDETAILS, iView == LV_VIEW_DETAILS);
            }
        }
    }
}

BOOL (__thiscall *CFileOpenSave__InitOpenSaveDialog_orig)(class CFileOpenSave *, HWND);
BOOL __thiscall CFileOpenSave__InitOpenSaveDialog_hook(
    class CFileOpenSave *pThis, 
    HWND                 hDlg)
{
    BOOL fResult = CFileOpenSave__InitOpenSaveDialog_orig(pThis, hDlg);
    if (fResult)
    {
        g_hwndOpenSaveDlg = hDlg;
        OnViewChange();
    }
    return fResult;
}

HRESULT (__stdcall *CFileOpenSave_OnNavigationComplete_orig)(class CFileOpenSave *, LPCITEMIDLIST);
HRESULT __stdcall CFileOpenSave_OnNavigationComplete_hook(
    class CFileOpenSave *pThis,
    LPCITEMIDLIST        pidlFolder)
{
    HRESULT hr = CFileOpenSave_OnNavigationComplete_orig(pThis, pidlFolder);
    if (SUCCEEDED(hr))
    {
        OnViewChange();
    }
    return hr;
}

HRESULT (__thiscall *CFileOpenSave__BrowseObject)(class CFileOpenSave *pThis, LPITEMIDLIST pidl, UINT wFlags);

LRESULT (__thiscall *CFileOpenSave__OnCommandMessage_orig)(class CFileOpenSave *, WPARAM, LPARAM);
LRESULT __thiscall CFileOpenSave__OnCommandMessage_hook(
    class CFileOpenSave *pThis,
    WPARAM               wParam,
    LPARAM               lParam)
{
    if (wParam == IDC_JUMPDESKTOP)
    {
        LPITEMIDLIST pidl;
        if (SUCCEEDED(SHGetKnownFolderIDList(FOLDERID_Desktop, 0, NULL, &pidl)))
        {
            CFileOpenSave__BrowseObject(pThis, pidl, SBSP_DEFBROWSER);
        }
    }
    return CFileOpenSave__OnCommandMessage_orig(pThis, wParam, lParam);
}

int (__fastcall *CDLoadStringEx_orig)(int, HINSTANCE, UINT, LPWSTR, int);
int __fastcall CDLoadStringEx_hook(
    int       cp,
    HINSTANCE hInstance,
    UINT      uID,
    LPWSTR    lpBuffer,
    int       nBufferMax
)
{
    if (uID == 710)
    {
        hInstance = g_hmodCommDlg98;
    }
    return CDLoadStringEx_orig(cp, hInstance, uID, lpBuffer, nBufferMax);
}

#ifdef _WIN64
    #define STDCALL_STR L"__cdecl"
    #define THISCALL_STR L"__cdecl"
#else
    #define STDCALL_STR L"__stdcall"
    #define THISCALL_STR L"__thiscall"
#endif

const WindhawkUtils::SYMBOL_HOOK comdlg32DllHooks[] = {
    {
        {
#ifdef _WIN64
            L"CDGetAppCompatFlags"
#else
            L"_CDGetAppCompatFlags@0"
#endif
        },
        &CDGetAppCompatFlags,
        nullptr,
        false
    },
    {
        {
            L"protected: int " THISCALL_STR L" CFileOpenSave::_InitOpenSaveDialog(struct HWND__ *)"
        },
        &CFileOpenSave__InitOpenSaveDialog_orig,
        CFileOpenSave__InitOpenSaveDialog_hook,
        false
    },
    {
        {
            L"public: virtual long " STDCALL_STR L" CFileOpenSave::OnNavigationComplete(struct _ITEMIDLIST_ABSOLUTE const *)"
        },
        &CFileOpenSave_OnNavigationComplete_orig,
        CFileOpenSave_OnNavigationComplete_hook,
        false
    },
    {
        {
            L"protected: long " THISCALL_STR L" CFileOpenSave::_BrowseObject(struct _ITEMIDLIST_ABSOLUTE const *,unsigned int)"
        },
        &CFileOpenSave__BrowseObject,
        nullptr,
        false
    },
    {
        {
#ifdef _WIN64
            L"protected: __int64 __cdecl CFileOpenSave::_OnCommandMessage(unsigned __int64,__int64)"
#else
            L"protected: long __thiscall CFileOpenSave::_OnCommandMessage(unsigned int,long)"
#endif
        },
        &CFileOpenSave__OnCommandMessage_orig,
        CFileOpenSave__OnCommandMessage_hook,
        false
    },
    {
        {
#ifdef _WIN64
            L"CDLoadStringEx"
#else
            L"_CDLoadStringEx@20"
#endif
        },
        &CDLoadStringEx_orig,
        CDLoadStringEx_hook,
        false
    },
};

void Wh_ModSettingsChanged(void)
{
    if (g_hmodCommDlg98)
    {
        FreeLibrary(g_hmodCommDlg98);
    }

    LPCWSTR pszPath = Wh_GetStringSetting(L"comdlg_98_path");
    if (pszPath[0])
    {
        g_hmodCommDlg98 = LoadLibraryExW(pszPath, NULL, LOAD_LIBRARY_AS_IMAGE_RESOURCE);
    }
    Wh_FreeStringSetting(pszPath);
}

BOOL Wh_ModInit(void)
{
    HMODULE hmodCommDlg = LoadLibraryExW(L"comdlg32.dll", NULL, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!hmodCommDlg)
    {
        Wh_Log(L"Failed to load comdlg32.dll");
        return FALSE;
    }

    if (!WindhawkUtils::HookSymbols(
        hmodCommDlg,
        comdlg32DllHooks,
        ARRAYSIZE(comdlg32DllHooks)))
    {
        Wh_Log(L"Failed to hook one or more symbol functions in comdlg32.dll");
        return FALSE;
    }

    MODULEINFO mi;
    GetModuleInformation(
        GetCurrentProcess(),
        hmodCommDlg,
        &mi,
        sizeof(mi));
    g_uCommDlgBase = (ULONGLONG)mi.lpBaseOfDll;
    g_uCommDlgEnd = g_uCommDlgBase + (ULONGLONG)mi.SizeOfImage;

    HMODULE hmodShlwapi = LoadLibraryExW(L"shlwapi.dll", NULL, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!hmodShlwapi)
    {
        Wh_Log(L"Failed to load shlwapi.dll");
        return FALSE;
    }

    void *pfnSHRestrictionLookup =
        (void *)GetProcAddress(hmodShlwapi, (LPCSTR)266);
    if (!pfnSHRestrictionLookup)
    {
        Wh_Log(L"Failed to get address of SHRestrictionLookup");
        return FALSE;
    }

    Wh_SetFunctionHook(
        (void *)CreateToolbarEx,
        (void *)CreateToolbarEx_hook,
        (void **)&CreateToolbarEx_orig
    );

    Wh_ModSettingsChanged();

    return TRUE;
}

void Wh_ModUninit(void)
{
    if (g_hmodCommDlg98)
    {
        FreeLibrary(g_hmodCommDlg98);
    }
}