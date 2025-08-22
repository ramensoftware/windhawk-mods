// ==WindhawkMod==
// @id              classic-conhost
// @name            Classic Conhost [Deprecated]
// @description     Forces classic theme and optionally client edge on console windows
// @version         1.0.4
// @author          aubymori
// @github          https://github.com/aubymori
// @include         *
// @include         conhost.exe
// @compilerOptions -luser32 -ldwmapi -luxtheme
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# ⚠️ DEPRECATION NOTICE ⚠️
As Windhawk hooking is unable to hook into `conhost.exe` reliably, this mod has many
issues. For this reason, I forked the Windows Terminal's OpenConsole (which itself is
basically the exact same as Windows' built-in conhost), and made it include the features
from this mod. You can get my fork, ConhostEX, [here](https://github.com/aubymori/ConhostEX).
Since this mod is outdated and has a successor, it is deprecated, will not receive any future
updates, and I highly advise against using it.

# Classic Conhost
This mod will apply classic theme and optionally client edge to console windows.

# IMPORTANT: READ!
Windhawk and this mod need to inject into `conhost.exe` for this mod to work properly.
Please navigate to Windhawk's Settings, Advanced settings, More advanced settings, and
make sure that `conhost.exe` is in the Process inclusion list.

**Before**:

![Before](https://raw.githubusercontent.com/aubymori/images/main/classic-conhost-before.png)

**After**:

![After](https://raw.githubusercontent.com/aubymori/images/main/classic-conhost-after.png)
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- classic: true
  $name: Classic theme
  $description: Apply classic theme to console windows. Disable this if you use classic theme system-wide.
- clientedge: true
  $name: Client edge (Classic theme only)
  $description: Apply a client edge to console windows
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <processthreadsapi.h>
#include <uxtheme.h>
#include <dwmapi.h>

HANDLE g_hObserverThread;

struct {
    bool classic;
    bool clientedge;
} settings;

BOOL CALLBACK EnumWindowsProc(HWND hWnd, LPARAM lParam)
{
    WCHAR szClass[MAX_PATH];
    int   nStatus = GetClassNameW(hWnd, szClass, MAX_PATH);
    BOOL  bIsValidString = (((ULONG_PTR)szClass & ~(ULONG_PTR)0xffff) != 0);

    if (nStatus && bIsValidString && !wcscmp(szClass, L"ConsoleWindowClass"))
    {
        if (settings.classic)
        {
            /* Set window theme */
            SetWindowTheme(hWnd, L" ", L" ");
            SendMessage(hWnd, WM_THEMECHANGED, NULL, NULL);

            /* Disable DWM NC frames */
            DWORD dwPol = DWMNCRP_DISABLED;
            DwmSetWindowAttribute(
                hWnd,
                DWMWA_NCRENDERING_POLICY,
                &dwPol,
                sizeof(DWORD)
            );
        }

        if (settings.clientedge)
        {
            DWORD dwExStyle = GetWindowLongPtrW(hWnd, GWL_EXSTYLE);
            RECT  rcWnd;
            GetWindowRect(hWnd, &rcWnd);

            if ((dwExStyle & WS_EX_CLIENTEDGE) != WS_EX_CLIENTEDGE)
            {
                SetWindowLongPtrW(
                    hWnd,
                    GWL_EXSTYLE,
                    dwExStyle | WS_EX_CLIENTEDGE
                );

                /* Resize for client edge */
                SetWindowPos(
                    hWnd,
                    NULL,
                    0, 0,
                    rcWnd.right - rcWnd.left + 4,
                    rcWnd.bottom - rcWnd.top + 4,
                    SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE
                );
            }
        }
    }

    return TRUE;
}

DWORD CALLBACK ObserverThreadProc(HANDLE handle)
{
    HDESK curDesktop = GetThreadDesktop(GetCurrentThreadId());

    for (int i = 0; i < 2; i++)
    {
        EnumDesktopWindows(curDesktop, EnumWindowsProc, NULL);
        Sleep(1);
    }

    return 0;
}

void LoadSettings(void)
{
    settings.classic = Wh_GetIntSetting(L"classic");
    settings.clientedge = Wh_GetIntSetting(L"clientedge");
}

BOOL Wh_ModInit(void)
{
    LoadSettings();

    if (settings.classic)
    {
        WCHAR szPath[MAX_PATH], *pBackslash = NULL;
        if (GetModuleFileNameW(NULL, szPath, MAX_PATH))
        {
            LPWSTR szPathL = wcslwr(szPath);
            pBackslash = wcsrchr(szPathL, L'\\');
            if (pBackslash && !wcscmp(pBackslash, L"\\conhost.exe"))
            {
                SetThemeAppProperties(STAP_ALLOW_NONCLIENT);
            }
        }
    }

    g_hObserverThread = CreateThread(NULL, NULL, ObserverThreadProc, NULL, NULL, NULL);
    SetThreadPriority(g_hObserverThread, THREAD_PRIORITY_TIME_CRITICAL);

    return TRUE;
}

void Wh_ModSettingsChanged(void)
{
    LoadSettings();
}