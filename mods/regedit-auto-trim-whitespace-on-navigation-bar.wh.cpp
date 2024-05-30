// ==WindhawkMod==
// @id              regedit-auto-trim-whitespace-on-navigation-bar
// @name            RegEdit Auto Trim Whitespace on Navigation Bar
// @description     Automatically trims whitespace on the navigation bar
// @version         1.0.0
// @author          ItsProfessional
// @github          https://github.com/ItsProfessional
// @include         regedit.exe
// @compilerOptions -lshlwapi
// ==/WindhawkMod==

// Source code is published under The GNU General Public License v3.0.

// ==WindhawkModReadme==
/*
# RegEdit Auto Trim Whitespace on Navigation Bar
This mod automatically trims whitespace on RegEdit's navigation bar.

If there is leading whitespace in the key entered in the navigation bar, then RegEdit doesn't navigate to the key and instead plays a beep sound.

This causes frustration when pasting a key into the navigation bar which has leading whitespace in it. This mod fixes that by automatically trimming the whitespace.
*/
// ==/WindhawkModReadme==

#include <Windows.h>
#include <shlwapi.h>
#include <windhawk_api.h>
#include <windhawk_utils.h>

#ifdef _WIN64
#define THISCALL  __cdecl
#define STHISCALL L"__cdecl"
#else
#define THISCALL  __thiscall
#define STHISCALL L"__thiscall"
#endif

typedef BOOL (THISCALL *ExpandKeyPath_t)(void*, LPWSTR, bool);
ExpandKeyPath_t ExpandKeyPath_orig;
BOOL THISCALL ExpandKeyPath_hook(void* pThis, LPWSTR pszKeyPath, bool param3) {
    StrTrimW(pszKeyPath, L" ");
    return ExpandKeyPath_orig(pThis, pszKeyPath, param3);
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
            L"private: int " STHISCALL " RegEdit::ExpandKeyPath(unsigned short const *,bool)"
        },
        &ExpandKeyPath_orig,
        ExpandKeyPath_hook,
        false
    };    

    if (!WindhawkUtils::HookSymbols(hRegEdit, &hook, 1))
    {
        Wh_Log(L"Failed to hook RegEdit::ExpandKeyPath");
        return FALSE;
    }

    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L">");
}
