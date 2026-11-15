// ==WindhawkMod==
// @id              lm-regedit-multi-instance
// @name            Multi-instance regedit
// @description     Allow multiple instances of regedit
// @version         1.0
// @author          Mark Jansen
// @github          https://github.com/learn-more
// @twitter         https://twitter.com/learn_more
// @include         regedit.exe
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Allow multiple instances of regedit
By default, regedit does not allow multiple instances running at the same time.
This mod fixes that.
*/
// ==/WindhawkModReadme==

using FindWindowW_t = decltype(&FindWindowW);
FindWindowW_t FindWindowW_Original;
HWND WINAPI FindWindowW_Hook(LPCWSTR lpClassName, LPCWSTR lpWindowName)
{
    Wh_Log(L"lpClassName=%s, lpWindowName=%s", lpClassName, lpWindowName);

    if (lpClassName && !_wcsicmp(lpClassName, L"RegEdit_RegEdit"))
    {
        Wh_Log(L"Returning NULL");
        return NULL;
    }

    return FindWindowW_Original(lpClassName, lpWindowName);
}

template<typename ProtoType>
BOOL Wh_SetFunctionHookT(ProtoType targetFunction, ProtoType hookFunction, ProtoType* originalFunction)
{
    return Wh_SetFunctionHook((void*)targetFunction, (void*)hookFunction, (void**)originalFunction);
}

BOOL Wh_ModInit()
{
    Wh_Log(L"Init " WH_MOD_ID L" version " WH_MOD_VERSION);

    Wh_SetFunctionHookT(FindWindowW, FindWindowW_Hook, &FindWindowW_Original);

    return TRUE;
}
