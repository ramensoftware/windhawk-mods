// ==WindhawkMod==
// @id              change-tray-icons
// @name            Change Tray Icons
// @description     Change all tray icons for an application to a specific other icon
// @version         1.0.0
// @author          aubymori
// @github          https://github.com/aubymori
// @compilerOptions -lshell32 -lgdi32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Force Tray Icons
This mod will change all tray icons for an application to a specific other icon.

**Example**: Steam with its pre-2011 icon

![Example: *Steam with its pre-2011 icon](https://raw.githubusercontent.com/aubymori/images/refs/heads/main/change-tray-icons-example.png)

## How to use
1. Go to the "Advanced" tab of this mod
2. Find the "Custom process inclusion list"
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
  - - path: Discord.exe
      $name: Application name or path
    - icon: C:\Classic\mIRC.ico
      $name: ICO file path
*/
// ==/WindhawkModSettings==

#include <stddef.h>
#include <shellapi.h>

HICON g_hIcon = NULL;

using Shell_NotifyIconW_t = decltype(&Shell_NotifyIconW);
Shell_NotifyIconW_t Shell_NotifyIconW_orig;
BOOL WINAPI Shell_NotifyIconW_hook(
    DWORD dwMessage, PNOTIFYICONDATAW lpData
)
{
    if (g_hIcon
    && lpData
    && lpData->cbSize >= offsetof(NOTIFYICONDATAW, hIcon)
    && (lpData->uFlags & NIF_ICON))
    {
        lpData->hIcon = g_hIcon;
    }
    return Shell_NotifyIconW_orig(dwMessage, lpData);
}

void Wh_ModSettingsChanged(void)
{
    if (g_hIcon)
    {
        DestroyIcon(g_hIcon);
        g_hIcon = NULL;
    }

    WCHAR szAppPath[MAX_PATH];
    GetModuleFileNameW(GetModuleHandleW(NULL), szAppPath, MAX_PATH);

    for (int i = 0;; i++)
    {
        LPCWSTR pszPath = Wh_GetStringSetting(L"icons[%d].path", i);

        // Stop enumerating at an empty string
        if (!*pszPath)
        {
            Wh_FreeStringSetting(pszPath);
            break;
        }

        // Does the current application path or name match the spoof?
        WCHAR *pBackslash = wcsrchr(szAppPath, L'\\');
        if (0 == wcsicmp(szAppPath, pszPath)
        || (pBackslash && 0 == wcsicmp(pBackslash + 1, pszPath)))
        {
            int cxIcon, cyIcon;

            typedef UINT (WINAPI *GetSystemMetricsForDpi_t)(int nIndex, UINT dpi);
            static GetSystemMetricsForDpi_t pfnGetSystemMetricsForDpi
                 = (GetSystemMetricsForDpi_t)GetProcAddress(GetModuleHandleW(L"user32.dll"), "GetSystemMetricsForDpi");
            if (pfnGetSystemMetricsForDpi)
            {
                HDC hdc = GetDC(NULL);
                int dpi = GetDeviceCaps(hdc, LOGPIXELSX);
                ReleaseDC(NULL, hdc);

                cxIcon = pfnGetSystemMetricsForDpi(SM_CXSMICON, dpi);
                cyIcon = pfnGetSystemMetricsForDpi(SM_CYSMICON, dpi);
            }
            else
            {
                cxIcon = GetSystemMetrics(SM_CXSMICON);
                cyIcon = GetSystemMetrics(SM_CYSMICON);
            }

            LPCWSTR pszIconPath = Wh_GetStringSetting(L"icons[%d].icon", i);
            g_hIcon = (HICON)LoadImageW(
                NULL,
                pszIconPath,
                IMAGE_ICON,
                cxIcon, cyIcon,
                LR_LOADFROMFILE | LR_DEFAULTCOLOR);
            Wh_FreeStringSetting(pszIconPath);
        }
        Wh_FreeStringSetting(pszPath);
    }
}

BOOL Wh_ModInit(void)
{
    Wh_ModSettingsChanged();

    return Wh_SetFunctionHook(
        (void *)Shell_NotifyIconW,
        (void *)Shell_NotifyIconW_hook,
        (void **)&Shell_NotifyIconW_orig
    );
}

void Wh_ModUninit(void)
{
    if (g_hIcon)
        DestroyIcon(g_hIcon);
}