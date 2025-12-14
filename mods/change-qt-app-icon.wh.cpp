// ==WindhawkMod==
// @id              change-qt-app-icon
// @name            Change Qt App Icon
// @description     Change the icon of all windows in a Qt app
// @version         1.0.0
// @author          aubymori
// @github          https://github.com/aubymori
// @compilerOptions -lpsapi
// @license         GPLv3
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Change Qt App Icon
This mod allows you to change the window icon of all windows for a Qt
application. Due to the way Qt stores resources, it is difficult to
differentiate icons, and as such, you may only set one icon to be
used at all times for an application.

**Example**: *IDA 9.2 with its pre-6.0 icon*

![Example: IDA 9.2 with its pre-6.0 icon](https://raw.githubusercontent.com/aubymori/images/refs/heads/main/change-qt-app-icon-example.png)

## How to use
1. Go to the "Advanced" tab of this mod.
2. Find the "Custom process inclusion list" option.
3. Add the name of the EXE you want to disable tray icons for.
   - If you don't know the name of the EXE, you can find it using 
     Task Manager.
4. Click Save.
5. Configure the mod as you want it to be.
6. If the program is open already, restart it.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- icons:
  - - path: ida.exe
      $name: Application name or path
    - icon: C:\Classic\IDA.ico
      $name: ICO file path
*/
// ==/WindhawkModSettings==

#include <psapi.h>

HICON g_hIconBig   = NULL;
HICON g_hIconSmall = NULL;

LRESULT (WINAPI *SendMessageW_orig)(HWND, UINT, WPARAM, LPARAM);
LRESULT WINAPI SendMessageW_hook(
    HWND   hwnd,
    UINT   uMsg,
    WPARAM wParam,
    LPARAM lParam
)
{
    if ((g_hIconBig || g_hIconSmall) && uMsg == WM_SETICON)
    {
        /* Store the beginning and end of qwindows.dll in memory. */
        static ULONG_PTR s_uQWindowsBase = 0;
        static ULONG_PTR s_uQWindowsEnd  = 0;
        if (!s_uQWindowsBase)
        {
            HMODULE hQWindows = GetModuleHandleW(L"qwindows.dll");
            if (hQWindows)
            {
                MODULEINFO mi;
                GetModuleInformation(
                    GetCurrentProcess(), hQWindows, &mi, sizeof(mi));
                s_uQWindowsBase = (ULONG_PTR)mi.lpBaseOfDll;
                s_uQWindowsEnd  = (ULONG_PTR)mi.lpBaseOfDll + mi.SizeOfImage;
                FreeLibrary(hQWindows);
            }
        }

        if (s_uQWindowsBase)
        {
            /* Only change the icon if the call is coming from qwindows.dll.
               That way, we don't mess up things like the native open/save dialogs. */
            ULONG_PTR uRetAddr = (ULONG_PTR)__builtin_return_address(0);
            if (uRetAddr >= s_uQWindowsBase && uRetAddr <= s_uQWindowsEnd)
            {
                DestroyIcon((HICON)lParam);
                lParam = (wParam == ICON_BIG) ? (LPARAM)g_hIconBig : (LPARAM)g_hIconSmall;
            }
        }
    }
    return SendMessageW_orig(hwnd, uMsg, wParam, lParam);
}

void Wh_ModSettingsChanged(void)
{
    if (g_hIconBig)
    {
        DestroyIcon(g_hIconBig);
        g_hIconBig = NULL;
    }

    if (g_hIconSmall)
    {
        DestroyIcon(g_hIconSmall);
        g_hIconSmall = NULL;
    }

    WCHAR szAppPath[MAX_PATH];
    GetModuleFileNameW(GetModuleHandleW(NULL), szAppPath, MAX_PATH);

    for (int i = 0;; i++)
    {
        LPCWSTR pszPath = Wh_GetStringSetting(L"icons[%d].path", i);

        // Stop enumerating at an empty string
        if (!pszPath[0])
        {
            Wh_FreeStringSetting(pszPath);
            break;
        }

        // Does the current application path or name match the icon definition?
        WCHAR *pBackslash = wcsrchr(szAppPath, L'\\');
        if (0 == wcsicmp(szAppPath, pszPath)
        || (pBackslash && 0 == wcsicmp(pBackslash + 1, pszPath)))
        {
            LPCWSTR pszIconPath = Wh_GetStringSetting(L"icons[%d].icon", i);
            g_hIconBig = (HICON)LoadImageW(
                NULL,
                pszIconPath,
                IMAGE_ICON,
                GetSystemMetrics(SM_CXICON),
                GetSystemMetrics(SM_CYICON),
                LR_LOADFROMFILE | LR_DEFAULTCOLOR);
            g_hIconSmall = (HICON)LoadImageW(
                NULL,
                pszIconPath,
                IMAGE_ICON,
                GetSystemMetrics(SM_CXSMICON),
                GetSystemMetrics(SM_CYSMICON),
                LR_LOADFROMFILE | LR_DEFAULTCOLOR);
            Wh_Log(L"App:  '%s'", szAppPath);
            Wh_Log(L"Icon: '%s'", pszIconPath);
            Wh_FreeStringSetting(pszIconPath);
        }
        Wh_FreeStringSetting(pszPath);
    }
}

BOOL Wh_ModInit(void)
{
    Wh_ModSettingsChanged();

    Wh_SetFunctionHook(
        (void *)SendMessageW,
        (void *)SendMessageW_hook,
        (void **)&SendMessageW_orig
    );
    return TRUE;
}