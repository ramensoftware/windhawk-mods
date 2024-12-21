// ==WindhawkMod==
// @id              legacy-file-copy
// @name            Legacy File Copy
// @description     Restores the Windows 7 file copy dialog
// @version         1.0.0
// @author          rounk-ctrl
// @github          https://github.com/rounk-ctrl
// @include         explorer.exe
// @compilerOptions -lole32 -loleaut32 -lruntimeobject -ldbghelp
// @architecture    x86-64
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Legacy File Copy
This mod restores the Windows 7 file copy dialog.

The code is based on the implementation in [ExplorerPatcher](https://github.com/valinet/ExplorerPatcher).

![Screenshot](https://i.imgur.com/llxboeN.png)
*/
// ==/WindhawkModReadme==
#include <Windows.h>
#include <dbghelp.h>

typedef BOOL(*SHELL32_CanDisplayWin8CopyDialogFunc)();
BOOL(*SHELL32_CanDisplayWin8CopyDialogOrig)();
BOOL SHELL32_CanDisplayWin8CopyDialogHook()
{
    return FALSE;
}

BOOL Wh_ModInit(void)
{
    Wh_Log(L"Init");
    HANDLE hShell32 = LoadLibrary(L"ext-ms-win-shell-exports-internal-l1-1-0.dll");
    HMODULE hShell32M =  GetModuleHandle(L"ext-ms-win-shell-exports-internal-l1-1-0.dll");
    void* origFunc = (void*)GetProcAddress(hShell32M, "SHELL32_CanDisplayWin8CopyDialog");
    Wh_SetFunctionHook(origFunc, (void*)SHELL32_CanDisplayWin8CopyDialogHook, (void**)&SHELL32_CanDisplayWin8CopyDialogOrig);
    return TRUE;
}
