// ==WindhawkMod==
// @id              lm-ppee-wow64
// @name            Fix PPEE Wow64 filesystem redirection
// @description     Disables Wow64 filesystem redirection when loading a file in PPEE
// @version         1.0
// @author          Mark Jansen
// @github          https://github.com/learn-more
// @twitter         https://twitter.com/learn_more
// @include         ppee.exe
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Fix Wow64 filesystem redirection for PPEE

When loading a file from `C:\Windows\System32`, PPEE would automatically be redirected to `C:\Windows\SysWOW64`.

This mod disables that redirection, so that the correct file is loaded.

## Before:
![before](https://i.imgur.com/pJx8oPy.png)

## After:
![after](https://i.imgur.com/DBmkYJe.png)


*/
// ==/WindhawkModReadme==

template<typename ProtoType>
BOOL Wh_SetFunctionHookT(ProtoType targetFunction, ProtoType hookFunction, ProtoType* originalFunction)
{
    return Wh_SetFunctionHook((void*)targetFunction, (void*)hookFunction, (void**)originalFunction);
}

static PBYTE g_PPEE_Address;

using CreateFileW_t = decltype(&CreateFileW);
CreateFileW_t CreateFileW_Original;
HANDLE __stdcall CreateFileW_Hook(LPCWSTR lpFileName,
                                  DWORD dwDesiredAccess,
                                  DWORD dwShareMode,
                                  LPSECURITY_ATTRIBUTES lpSecurityAttributes,
                                  DWORD dwCreationDisposition,
                                  DWORD dwFlagsAndAttributes,
                                  HANDLE hTemplateFile) {
    Wh_Log(L"%s", lpFileName);

    PVOID Cookie = NULL;
    BOOL fRestore = Wow64DisableWow64FsRedirection(&Cookie);

    HANDLE hFile = CreateFileW_Original(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);

    if (fRestore)
        Wow64RevertWow64FsRedirection(Cookie);
    
    return hFile;
}

BOOL Wh_ModInit()
{
    Wh_Log(L"Init " WH_MOD_ID L" version " WH_MOD_VERSION);
    g_PPEE_Address = (PBYTE)GetModuleHandleW(NULL);
    Wh_SetFunctionHookT(CreateFileW, CreateFileW_Hook, &CreateFileW_Original);
    return TRUE;
}
