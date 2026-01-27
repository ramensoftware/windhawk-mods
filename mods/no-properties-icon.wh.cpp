// ==WindhawkMod==
// @id              no-properties-icon
// @name            No Properties Icon
// @description     Removes the window icon from property sheets like previous Windows versions
// @version         1.0.2
// @author          xalejandro
// @github          https://github.com/tetawaves
// @include         *
// @compilerOptions -lcomctl32 -lversion
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# No Properties Icon
Removes the window icon from property sheets like previous Windows versions.
![Before / After](https://i.imgur.com/ehRAMCz.png)
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- showtaskbaricon: false
  $name: Show property sheet window in taskbar
  $description: Forces the window to show in the taskbar
- removeall: false
  $name: Remove window icon from all property sheets
  $description: Some property sheets explicitly set an icon, enable this if you want to remove the window icon from all property sheets
*/
// ==/WindhawkModSettings==

#include <prsht.h>
#include <windhawk_api.h>
#include <windhawk_utils.h>
#include <shlwapi.h>

#ifdef _WIN64
#   define STDCALL  __cdecl
#   define SSTDCALL L"__cdecl"
#else
#   define STDCALL  __stdcall
#   define SSTDCALL L"__stdcall"
#endif

struct
{
    bool showTaskbarIcon;
    bool removeAll;
} settings;

VS_FIXEDFILEINFO* GetModuleVersionInfo(HMODULE hModule, UINT *puPtrLen)
{
    void *pFixedFileInfo = nullptr;
    UINT uPtrLen = 0;

    HRSRC hResource =
        FindResourceW(hModule, MAKEINTRESOURCEW(VS_VERSION_INFO), RT_VERSION);
    if (hResource)
    {
        HGLOBAL hGlobal = LoadResource(hModule, hResource);
        if (hGlobal)
        {
            void *pData = LockResource(hGlobal);
            if (pData)
            {
                if (!VerQueryValueW(pData, L"\\", &pFixedFileInfo, &uPtrLen)
                || uPtrLen == 0)
                {
                    pFixedFileInfo = nullptr;
                    uPtrLen = 0;
                }
            }
        }
    }

    if (puPtrLen)
    {
        *puPtrLen = uPtrLen;
    }

    return (VS_FIXEDFILEINFO *)pFixedFileInfo;
}

/**
  * Loads comctl32.dll, version 6.0.
  * This uses an activation context that uses shell32.dll's manifest
  * to load 6.0, even in apps which don't have the proper manifest for
  * it.
  * From: https://github.com/ramensoftware/windhawk-mods/blob/main/mods/classic-list-group-fix.wh.cpp
  */
HMODULE LoadComCtlModule(void)
{
    HMODULE hShell32 = LoadLibraryW(L"shell32.dll");
    ACTCTXW actCtx = { sizeof(actCtx) };
    actCtx.dwFlags = ACTCTX_FLAG_RESOURCE_NAME_VALID | ACTCTX_FLAG_HMODULE_VALID;
    actCtx.lpResourceName = MAKEINTRESOURCEW(124);
    actCtx.hModule = hShell32;
    HANDLE hActCtx = CreateActCtxW(&actCtx);
    ULONG_PTR ulCookie;
    ActivateActCtx(hActCtx, &ulCookie);
    HMODULE hComCtl = LoadLibraryExW(L"comctl32.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    /**
      * Certain processes will ignore the activation context and load
      * comctl32.dll 5.82 anyway. If that occurs, just reject it.
      */
    VS_FIXEDFILEINFO *pVerInfo = GetModuleVersionInfo(hComCtl, nullptr);
    if (!pVerInfo || HIWORD(pVerInfo->dwFileVersionMS) < 6)
    {
        FreeLibrary(hComCtl);
        hComCtl = NULL;
    }
    DeactivateActCtx(0, ulCookie);
    ReleaseActCtx(hActCtx);
    FreeLibrary(hShell32);
    return hComCtl;
}

int (STDCALL *_SetPropertySheetIcon_orig)(IDataObject *, ITEMIDLIST_ABSOLUTE *, PROPSHEETHEADERW *);
int STDCALL _SetPropertySheetIcon_hook(IDataObject *, ITEMIDLIST_ABSOLUTE *, PROPSHEETHEADERW *)
{
    return 0;
}

INT_PTR (WINAPI *PropertySheetW_orig)(PROPSHEETHEADERW *);
INT_PTR WINAPI PropertySheetW_hook(PROPSHEETHEADERW *ppshw)
{
    // Some property sheets set an icon and show even on older Windows versions,
    // so we give a option to let property sheets have an icon if explicitly set
    if (settings.removeAll && ppshw->hIcon)
    {
        ppshw->hIcon = nullptr;
    }
    return PropertySheetW_orig(ppshw);
}

DLGPROC PropSheetDlgProc_orig;
INT_PTR CALLBACK PropSheetDlgProc_hook(
    HWND   hwnd,
    UINT   uMsg,
    WPARAM wParam,
    LPARAM lParam
)
{
    if (uMsg == WM_INITDIALOG || uMsg == WM_ACTIVATE)
    {
        PropSheetDlgProc_orig(hwnd, uMsg, wParam, lParam);

        LONG_PTR exStyle = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);
        exStyle = settings.showTaskbarIcon ? exStyle |= WS_EX_APPWINDOW : exStyle &= ~WS_EX_APPWINDOW;

        SetWindowLongPtrW(hwnd, GWL_EXSTYLE, exStyle);

        return 0;
    }
    return PropSheetDlgProc_orig(hwnd, uMsg, wParam, lParam);
}

WNDPROC WndProc_orig;
LRESULT CALLBACK WndProc_hook(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (uMsg == WM_ACTIVATEAPP)
        return DefWindowProcW(hwnd, uMsg, wParam, lParam);
    return WndProc_orig(hwnd, uMsg, wParam, lParam);
}

void LoadSettings(void)
{
    settings.showTaskbarIcon = Wh_GetIntSetting(L"showtaskbaricon");
    settings.removeAll = Wh_GetIntSetting(L"removeall");
}

BOOL Wh_ModInit(void)
{
    Wh_Log(L"Init");

    LoadSettings();

    WCHAR szModuleName[MAX_PATH];
    HMODULE hModule = GetModuleHandleW(nullptr);
    GetModuleFileNameW(hModule, szModuleName, MAX_PATH);

    HMODULE hShell32 = LoadLibraryExW(L"shell32.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!hShell32)
    {
        Wh_Log(L"Failed to load shell32.dll");
        return FALSE;
    }

    HMODULE hComCtl32 = LoadComCtlModule();
    if (!hComCtl32)
    {
        Wh_Log(L"Failed to load comctl32.dll");
        return FALSE;
    }

    const WindhawkUtils::SYMBOL_HOOK shell32DllHooks[]
    {
        {
            {
                L"int " SSTDCALL " _SetPropertySheetIcon(struct IDataObject *,struct _ITEMIDLIST_ABSOLUTE *,struct _PROPSHEETHEADERW_V2 *)"
            },
            &_SetPropertySheetIcon_orig,
            _SetPropertySheetIcon_hook,
            false
        },
    };

    WindhawkUtils::SYMBOL_HOOK comCtl32Dllhook = {
        {
            #ifdef _WIN64
            L"__int64 __cdecl PropSheetDlgProc(struct HWND__ *,unsigned int,unsigned __int64,__int64)"
            #else
            L"int __stdcall PropSheetDlgProc(struct HWND__ *,unsigned int,unsigned int,long)"
            #endif
        },
        &PropSheetDlgProc_orig,
        PropSheetDlgProc_hook,
        false
    };

    WindhawkUtils::SYMBOL_HOOK sysdmCplHook = {
        {
            #ifdef _WIN64
            L"__int64 __cdecl StubWndProc(struct HWND__ *,unsigned int,unsigned __int64,__int64)"
            #else
            L"long __stdcall StubWndProc(struct HWND__ *,unsigned int,unsigned int,long)"
            #endif
        },
        &WndProc_orig,
        WndProc_hook,
        false
    };

    WindhawkUtils::SYMBOL_HOOK rundll32ExeHook = 
    {
        {
            #ifdef _WIN64
            L"__int64 __cdecl WndProc(struct HWND__ *,unsigned int,unsigned __int64,__int64)"
            #else
            L"long __stdcall WndProc(struct HWND__ *,unsigned int,unsigned int,long)"
            #endif
        },
        &WndProc_orig,
        WndProc_hook,
        false
    };

    if (!WindhawkUtils::HookSymbols(hShell32, shell32DllHooks, 
                                    ARRAYSIZE(shell32DllHooks)))
    {
        Wh_Log(L"Failed to hook shell32.dll");
        return FALSE;
    }

    if (!WindhawkUtils::HookSymbols(hComCtl32, &comCtl32Dllhook, 1))
    {
        Wh_Log(L"Failed to hook comctl32.dll");
        return FALSE;
    }

    if (HINSTANCE hSysdm = GetModuleHandleW(L"sysdm.cpl"); hSysdm)
    {
        if (!WindhawkUtils::HookSymbols(hSysdm, &sysdmCplHook, 1)) 
        {
            Wh_Log(L"Failed to hook sysdm.cpl");
        }
    }

    if (StrStrI(szModuleName, L"\\rundll32.exe") != nullptr)
    {
        if (!WindhawkUtils::HookSymbols(hModule, &rundll32ExeHook, 1)) 
        {
            Wh_Log(L"Failed to hook rundll32.exe");
        }
    }

    Wh_SetFunctionHook((void*)GetProcAddress(hComCtl32, "PropertySheetW"), 
                       (void*)PropertySheetW_hook, (void**)&PropertySheetW_orig);

    return TRUE;
}

void Wh_ModSettingsChanged(void)
{
    LoadSettings();
}
