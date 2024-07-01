// ==WindhawkMod==
// @id              notepad-remove-launch-new-app-banner
// @name            Notepad Remove Launch New App Banner
// @description     Removes the banner in classic notepad that tells you to launch the new app.
// @version         1.0.0
// @author          ItsProfessional
// @github          https://github.com/ItsProfessional
// @include         %SystemRoot%\System32\notepad.exe
// @include         %SystemRoot%\SysWOW64\notepad.exe
// @include         %SystemRoot%\notepad.exe
// ==/WindhawkMod==

// Source code is published under The GNU General Public License v3.0.

// ==WindhawkModReadme==
/*
# Notepad Remove Launch New App Banner
This mod removes the banner in classic notepad (which stills exists in the latest version of Windows 11 in `System32`, but can only be opened if the App Execution Alias is disabled) that tells you to launch the new app.

![Screenshot](https://i.imgur.com/8eFaQSb.png)
*/
// ==/WindhawkModReadme==

#include <windhawk_api.h>
#include <windhawk_utils.h>

#ifdef _WIN64
#define STDCALL  __cdecl
#define SSTDCALL L"__cdecl"
#else
#define STDCALL  __stdcall
#define SSTDCALL L"__stdcall"
#endif

typedef LONG (STDCALL *ActivateBanner_t)(bool);
ActivateBanner_t ActivateBanner_orig;
LONG STDCALL ActivateBanner_hook(bool a1) {
    return NULL;
}

BOOL Wh_ModInit(void)
{
    Wh_Log(L">");

    HMODULE hModule = GetModuleHandle(NULL);
    if (!hModule)
    {
        Wh_Log(L"Failed to load module");
        return FALSE;
    }

    WindhawkUtils::SYMBOL_HOOK hook = {
        {
            L"long " SSTDCALL " ActivateBanner(bool)"
        },
        &ActivateBanner_orig,
        ActivateBanner_hook,
        false
    };    

    if (!WindhawkUtils::HookSymbols(hModule, &hook, 1))
    {
        Wh_Log(L"Failed to hook ActivateBanner");
        return FALSE;
    }

    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L">");
}
