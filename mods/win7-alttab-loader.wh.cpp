// ==WindhawkMod==
// @id              win7-alttab-loader
// @name            Windows 7 Alt+Tab Loader
// @description     Loads Windows 7 Alt+Tab on Windows 10.
// @version         2.0.0
// @author          aubymori
// @github          https://github.com/aubymori
// @include         explorer.exe
// @compilerOptions -ldwmapi -lcomctl32 -lgdi32
// @architecture    x86-64
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Windows 7 Alt+Tab Loader
This mod allows the Windows 7 Alt+Tab UI to work on Windows 10.

# ⚠ IMPORTANT: PREREQUISITES! ⚠
- You will need a copy of `AltTab.dll` from Windows 7 (x64). Once you have this, drop it into `%SystemRoot%\System32`.
- You will also need an msstyles theme with a proper `AltTab` class, or else it will not render properly (Windows 3.x System font, weird looking selection, transparent background on basic theme)
  - [Here is a Windows 7 theme with proper classes.](https://www.deviantart.com/vaporvance/art/Aero10-for-Windows-10-1903-22H2-909711949)
  - You can make one with Windows Style Builder, which allows you to add classes to theme, or you can base on one that already does, and edit using
  msstyleEditor (recommended).
- Once you have all these and install this mod, you will need to restart `explorer.exe` for the Windows 7 Alt+Tab UI to load.
  - You will also need to restart `explorer.exe` to use the mod.

![DWM (with thumbnails)](https://raw.githubusercontent.com/aubymori/images/main/win7-alt-tab-dwm.png)
![Basic (no thumbnails)](https://raw.githubusercontent.com/aubymori/images/main/win7-alt-tab-basic.png)

*Original Windhawk mod by ephemeralViolette, original implementation in ExplorerPatcher by valinet.*
*/
// ==/WindhawkModReadme==

#include <windhawk_utils.h>
#include <initguid.h>
#include <docobj.h>
#include <psapi.h>
#include <dwmapi.h>
#include <shlobj.h>
#include <shobjidl.h>
#include <propkey.h>
#include <shlguid.h>

HMODULE g_hAltTab = NULL;

DEFINE_GUID(CLSID_AltTabSSO, 0xA1607060, 0x5D4C, 0x467A, 0xB7,0x11, 0x2B,0x59,0xA6,0xF2,0x59,0x57);
IOleCommandTarget *g_pAltTabSSO = nullptr;

typedef BOOL (WINAPI *IsShellWindow_t)(HWND);
IsShellWindow_t IsShellFrameWindow = nullptr;
IsShellWindow_t IsShellManagedWindow = nullptr;

typedef HWND(WINAPI *GhostWindowFromHungWindow_t)(HWND);
GhostWindowFromHungWindow_t GhostWindowFromHungWindow = nullptr;

/* Prevent background UWP windows from showing up */
BOOL (WINAPI *IsWindowEnabled_orig)(HWND);
BOOL WINAPI IsWindowEnabled_hook(HWND hWnd)
{
	if (!IsWindowEnabled_orig(hWnd))
		return FALSE;

	BOOL bCloaked;
	DwmGetWindowAttribute(hWnd, DWMWA_CLOAKED, &bCloaked, sizeof(BOOL));
	if (bCloaked)
		return FALSE;

    if (IsShellFrameWindow(hWnd) && !GhostWindowFromHungWindow(hWnd))
        return TRUE;

    if (IsShellManagedWindow(hWnd) && GetPropW(hWnd, L"Microsoft.Windows.ShellManagedWindowAsNormalWindow") == NULL)
        return FALSE;

	return TRUE;
}

void removechar(wchar_t *str, wchar_t c)
{
    wchar_t *src, *dst;
    for (src = dst = str; *src != L'\0'; src++)
    {
        *dst = *src;
        if (*dst != c) dst++;
    }
    *dst = L'\0';
}

/* Load strings without the MUI */
int (WINAPI *LoadStringW_orig)(HINSTANCE, UINT, LPWSTR, int);
int WINAPI LoadStringW_hook(HINSTANCE hInstance, UINT uId, LPWSTR lpBuffer, int cchBufferMax)
{
    if (hInstance == g_hAltTab)
    {
        switch (uId)
        {
            case 0x3E8:
                swprintf_s(lpBuffer, cchBufferMax, L"AltTab");
                return 6;
            case 0x3EA:
            {
                HMODULE hExplorerFrame = GetModuleHandleW(L"ExplorerFrame.dll");
                if (hExplorerFrame)
                {
                    WCHAR szDesktop[MAX_PATH];
                    LoadStringW_orig(hExplorerFrame, 13140, szDesktop, MAX_PATH);
                    removechar(szDesktop, L'&');
                    wcscpy_s(lpBuffer, cchBufferMax, szDesktop);
                    return wcslen(lpBuffer);
                }
                else
                {
                    swprintf_s(lpBuffer, cchBufferMax, L"Desktop");
                    return 7;
                }
            }
        }
    }
    return LoadStringW_orig(hInstance, uId, lpBuffer, cchBufferMax);
}

/* I don't know what this does but it's in the original so I'll keep it. */
BOOL (WINAPI *PostMessageW_orig)(HWND, UINT, WPARAM, LPARAM);
BOOL WINAPI PostMessageW_hook(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (hWnd == FindWindowW(L"Shell_TrayWnd", NULL) && uMsg == 0x5B7 && wParam == 0 && lParam == 0)
    {
        return PostMessageW_orig(hWnd, WM_COMMAND, 407, 0);
    }
    return PostMessageW_orig(hWnd, uMsg, wParam, lParam);
}

/* The signature of this function changed in Windows 10. */
HRESULT (WINAPI *DwmpActivateLivePreview_orig)(BOOL, HWND, HWND, UINT, UINT_PTR);
HRESULT WINAPI DwmpActivateLivePreview_hook(
    BOOL fActivate,
    HWND hPeekWnd,
    HWND hTopmostWnd,
    UINT uPeekType
)
{
    return DwmpActivateLivePreview_orig(
        fActivate, hPeekWnd, hTopmostWnd, uPeekType, 0
    );
}

DEFINE_GUID(GUID_AppUserModelIdProperty, 0x9F4C2855, 0x9F79, 0x4B39, 0xA8,0xD0, 0xE1,0xD4,0x2D,0xE1,0xD5,0xF3);

/**
  * Implementation taken from Simple Window Switcher.
  * https://github.com/valinet/sws/blob/971a3f20262c065c3b001c1781b6d75f9083680e/SimpleWindowSwitcher/sws_IconPainter.c#L253
  */
HICON GetUWPIcon(HWND hWnd)
{
    HICON result = NULL;
    HRESULT hr = S_OK;
    IShellItemImageFactory *psiif = nullptr;
    SIIGBF flags = SIIGBF_RESIZETOFIT | SIIGBF_ICONBACKGROUND;

    IPropertyStore *pps = nullptr;
    hr = SHGetPropertyStoreForWindow(
        hWnd, IID_PPV_ARGS(&pps)
    );
    if (SUCCEEDED(hr))
    {
        PROPERTYKEY pKey;
        pKey.fmtid = GUID_AppUserModelIdProperty;
        pKey.pid = 5;
        PROPVARIANT prop;
        ZeroMemory(&prop, sizeof(PROPVARIANT));
        pps->GetValue(pKey, &prop);
        pps->Release();
        if (prop.bstrVal)
        {
            SHCreateItemInKnownFolder(
                FOLDERID_AppsFolder,
                KF_FLAG_DONT_VERIFY,
                prop.bstrVal,
                IID_PPV_ARGS(&psiif)
            );
            if (psiif)
            {
                int szIcon = GetSystemMetrics(SM_CXICON);

                SIZE size;
                size.cx = szIcon;
                size.cy = szIcon;
                HBITMAP hBitmap;
                hr = psiif->GetImage(
                    size,
                    flags,
                    &hBitmap
                );
                if (SUCCEEDED(hr))
                {
                    // Easiest way to get an HICON from an HBITMAP
                    // I have turned the Internet upside down and was unable to find this
                    // Only a convoluted example using GDI+
                    // This is from the disassembly of StartIsBack/StartAllBack
                    HIMAGELIST hImageList = ImageList_Create(size.cx, size.cy, ILC_COLOR32, 1, 0);
                    if (ImageList_Add(hImageList, hBitmap, NULL) != -1)
                    {
                        result = ImageList_GetIcon(hImageList, 0, 0);
                        ImageList_Destroy(hImageList);

                        DeleteObject(hBitmap);
                        psiif->Release();
                    }
                    DeleteObject(hBitmap);
                }
                psiif->Release();
            }
        }
    }
    return result;
}

/* Support UWP icons */
SENDASYNCPROC CWindowInfo__GetIconProc_orig;
void WINAPI CWindowInfo__GetIconProc_hook(
    HWND     hWnd,
    UINT     uMsg,
    UINT_PTR upParam,
    LRESULT  lResult
)
{
    if (NULL == lResult && IsShellFrameWindow(hWnd))
    {
        lResult = (LRESULT)GetUWPIcon(hWnd);
    }
    CWindowInfo__GetIconProc_orig(hWnd, uMsg, upParam, lResult);
}

const WindhawkUtils::SYMBOL_HOOK altTabDllHooks[] = {
    {
        {
            L"private: static void __cdecl CWindowInfo::_GetIconProc(struct HWND__ *,unsigned int,unsigned __int64,__int64)"
        },
        &CWindowInfo__GetIconProc_orig,
        CWindowInfo__GetIconProc_hook,
        false
    }
};

BOOL Wh_ModInit(void)
{
    g_hAltTab = LoadLibraryW(L"AltTab.dll");
    if (!g_hAltTab)
    {
        Wh_Log(L"Failed to load AltTab.dll");
        return FALSE;
    }

    HMODULE hUser32 = GetModuleHandleW(L"user32.dll");
    IsShellFrameWindow = (IsShellWindow_t)GetProcAddress(hUser32, (LPCSTR)2573);
	IsShellManagedWindow = (IsShellWindow_t)GetProcAddress(hUser32, (LPCSTR)2574);
	GhostWindowFromHungWindow = (GhostWindowFromHungWindow_t)GetProcAddress(hUser32, "GhostWindowFromHungWindow");

    Wh_SetFunctionHook(
        (void *)IsWindowEnabled,
        (void *)IsWindowEnabled_hook,
        (void **)&IsWindowEnabled_orig
    );
    Wh_SetFunctionHook(
        (void *)LoadStringW,
        (void *)LoadStringW_hook,
        (void **)&LoadStringW_orig
    );
    Wh_SetFunctionHook(
        (void *)PostMessageW,
        (void *)PostMessageW_hook,
        (void **)&PostMessageW_orig
    );

    HMODULE hDwmapi = LoadLibraryW(L"dwmapi.dll");
    if (!hDwmapi)
    {
        Wh_Log(L"Failed to load dwmapi.dll");
        return FALSE;
    }

    void *DwmpActivateLivePreview = (void *)GetProcAddress(hDwmapi, (LPCSTR)113);
    if (!DwmpActivateLivePreview)
    {
        Wh_Log(L"Failed to find DwmpActivateLivePreview in dwmapi.dll");
        return FALSE;
    }

    Wh_SetFunctionHook(
        DwmpActivateLivePreview,
        (void *)DwmpActivateLivePreview_hook,
        (void **)&DwmpActivateLivePreview_orig
    );

    using DllGetClassObject_t = decltype(&DllGetClassObject);
    DllGetClassObject_t pDllGetClassObject = (DllGetClassObject_t)GetProcAddress(
        g_hAltTab, "DllGetClassObject"
    );
    if (!pDllGetClassObject)
    {
        Wh_Log(L"Failed to find DllGetClassObject in AltTab.dll");
        return FALSE;
    }

    IClassFactory *pFactory = nullptr;
    bool bSucceeded = false;
    if (SUCCEEDED(pDllGetClassObject(CLSID_AltTabSSO, IID_PPV_ARGS(&pFactory))) && pFactory)
    {
        if (SUCCEEDED(pFactory->CreateInstance(nullptr, IID_PPV_ARGS(&g_pAltTabSSO)) && g_pAltTabSSO))
        {
            if (SUCCEEDED(g_pAltTabSSO->Exec(&CGID_ShellServiceObject, 2, 0, NULL, NULL)))
            {
                Wh_Log(L"Using Windows 7 Alt+Tab");
                bSucceeded = true;
            }
            else
            {
                Wh_Log(L"Failed at Exec");
            }
        }
        else
        {
            Wh_Log(L"Failed at CreateInstance");
        }

        pFactory->Release();
    }
    else
    {
        Wh_Log(L"Failed at DllGetClassObject");
    }

    if (!bSucceeded || !WindhawkUtils::HookSymbols(
        g_hAltTab,
        altTabDllHooks,
        ARRAYSIZE(altTabDllHooks)
    ))
    {
        return FALSE;
    }
    return TRUE;
}

void Wh_ModUninit(void)
{
    if (g_pAltTabSSO)
        g_pAltTabSSO->Release();
}