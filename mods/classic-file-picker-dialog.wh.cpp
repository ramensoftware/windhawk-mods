// ==WindhawkMod==
// @id              classic-file-picker-dialog
// @name            Classic File Picker dialog
// @description     Redirect the Windows Vista+ file picker to the Windows XP one
// @version         1.1.1
// @author 	        Anixx
// @github          https://github.com/Anixx
// @include         *
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
This mod replaces the default file picker (open/save dialog) with the legacy one,
which was used in Windows XP and Windws 2000.

For 3D border in the dialog, install the mod "ClientEdge Everywhere"

The mod is based on the idea by xdmg01.

![File Picker](https://i.imgur.com/g2NVokJ.png)

*/
// ==/WindhawkModReadme==

#include <windhawk_utils.h>

#ifdef _WIN64
#define CALCON __cdecl
#define SCALCON L" __cdecl "
#else
#define CALCON __thiscall
#define SCALCON L" __stdcall "
#endif

typedef int (CALCON *IsCommonItemDialogAvailable)(void);
IsCommonItemDialogAvailable IsCommonItemDialogAvailable_orig;

int CALCON IsCommonItemDialogAvailable_hook(void) {return 0;}

typedef long (CALCON *CFileOpenSave__s_CreateInstance)(int, GUID*, void**);
CFileOpenSave__s_CreateInstance CFileOpenSave__s_CreateInstance_orig;

long CALCON CFileOpenSave__s_CreateInstance_hook(int param_1, GUID *param_2, void **param_3) {
    param_1 = (param_1 > 1) ? param_1 -= 2 : param_1;
    return CFileOpenSave__s_CreateInstance_orig(param_1, param_2, param_3);
}

BOOL Wh_ModInit() {
    HMODULE module = LoadLibrary(L"comdlg32.dll");

    WindhawkUtils::SYMBOL_HOOK comdlg32DllHooks[] = {
        {
            {L"int" SCALCON L"IsCommonItemDialogAvailable(void)"},
            (void**)&IsCommonItemDialogAvailable_orig,
            (void*)IsCommonItemDialogAvailable_hook,
        },
        {
            {L"public: static long" SCALCON L"CFileOpenSave::s_CreateInstance(enum DIALOGINITFLAGS,struct _GUID const &,void * *)"},
            (void**)&CFileOpenSave__s_CreateInstance_orig,
            (void*)CFileOpenSave__s_CreateInstance_hook,
        }
    };

    WindhawkUtils::HookSymbols(module, comdlg32DllHooks, ARRAYSIZE(comdlg32DllHooks));

    return TRUE;
}
