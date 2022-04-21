// ==WindhawkMod==
// @id           lm-close-windbg-file-changed
// @name         Close WinDbg's file changed dialog
// @description  Automatically rejects (or accepts) the dialog from WinDbg telling that a file is changed
// @version      1.0
// @author       Mark Jansen
// @github       https://github.com/learn-more
// @twitter      https://twitter.com/learn_more
// @include      windbg.exe
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Close WinDbg's file changed dialog
This mod automatically dismisses the popup from WinDbg notifying about a file change.
When automating WinDbg, this dialog blocks progress.

![demonstration](https://i.imgur.com/8CLg83M.png)
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- Accept: false
  $description: Enable this to accept the dialog instead of rejecting it (reloading the file instead of keeping the old file)
*/
// ==/WindhawkModSettings==


#include <string>

static int g_Accept = 0;


typedef decltype(&MessageBoxW) MESSAGEBOXW;
MESSAGEBOXW pMessageBoxW;

INT WINAPI hkMessageBoxW(HWND hWnd, LPCWSTR lpText, LPCWSTR lpCaption, UINT uType)
{
    if (lpText)
    {
        std::wstring tmp = lpText;
        if (tmp.find(L"has changed since it was opened.") != std::wstring::npos &&
            tmp.find(L"Do you want to reload the file?") != std::wstring::npos)
        {
            Wh_Log(L"auto %s", g_Accept ? L"accepting" : L"rejecting");
            return g_Accept ? IDYES : IDNO;
        }
    }

    return pMessageBoxW(hWnd, lpText, lpCaption, uType);
}

// The mod is being initialized, load settings, hook functions, and do other
// initialization stuff if required.
BOOL Wh_ModInit(void)
{
    Wh_Log(L"Init");

    g_Accept = Wh_GetIntSetting(L"Accept");

    Wh_SetFunctionHook((void*)MessageBoxW, (void*)hkMessageBoxW, (void**)&pMessageBoxW);

    return TRUE;
}

// The mod is being unloaded, free all allocated resources.
void Wh_ModUninit(void)
{
    Wh_Log(L"Uninit");
}

// The mod setting were changed, reload them.
void Wh_ModSettingsChanged(void)
{
    Wh_Log(L"SettingsChanged");

    g_Accept = Wh_GetIntSetting(L"Accept");
}
