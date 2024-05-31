// ==WindhawkMod==
// @id              regedit-disable-beep
// @name            RegEdit Disable Beep
// @description     Prevents RegEdit from beeping when an invalid key is entered to the navigation bar
// @version         1.0.0
// @author          ItsProfessional
// @github          https://github.com/ItsProfessional
// @include         regedit.exe
// @compilerOptions -lshlwapi
// ==/WindhawkMod==

// Source code is published under The GNU General Public License v3.0.

// ==WindhawkModReadme==
/*
# RegEdit Disable Beep
When an invalid key is entered into the navigation bar, RegEdit plays a beep sound. This mod disables this sound.
*/
// ==/WindhawkModReadme==

#include <Windows.h>
#include <windhawk_api.h>
#include <windhawk_utils.h>

#ifdef _WIN64
#define THISCALL  __cdecl
#define STHISCALL L"__cdecl"
#define STDCALL  __cdecl
#define SSTDCALL L"__cdecl"
#else
#define THISCALL  __thiscall
#define STHISCALL L"__thiscall"
#define STDCALL  __stdcall
#define SSTDCALL L"__stdcall"
#endif

bool g_flag;

typedef void (THISCALL *Navigate_t)(void*);
Navigate_t Navigate_orig;
void THISCALL Navigate_hook(void *pThis) {
    g_flag = true;
    Navigate_orig(pThis);
    g_flag = false;
}

using MessageBeep_t = decltype(&MessageBeep);
MessageBeep_t MessageBeep_orig;
WINBOOL STDCALL MessageBeep_hook(UINT uType) {
    if(g_flag) return 0;

    return MessageBeep_orig(uType);
}

BOOL Wh_ModInit(void)
{
    Wh_Log(L">");

    HMODULE hRegEdit = LoadLibraryW(L"regedit.exe");
    if (!hRegEdit)
    {
        Wh_Log(L"Failed to load regedit.exe");
        return FALSE;
    }

    WindhawkUtils::SYMBOL_HOOK hook = {
        {
            L"public: void " STHISCALL " RegEdit::Navigate(void)"
        },
        &Navigate_orig,
        Navigate_hook,
        false
    };    

    if (!WindhawkUtils::HookSymbols(hRegEdit, &hook, 1))
    {
        Wh_Log(L"Failed to hook RegEdit::ExpandKeyPath");
        return FALSE;
    }

    if (!Wh_SetFunctionHook((void*)MessageBeep, (void*)MessageBeep_hook, (void**)&MessageBeep_orig))
    {
        Wh_Log(L"Failed to hook MessageBeep");
        return FALSE; 
    }

    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L">");
}
