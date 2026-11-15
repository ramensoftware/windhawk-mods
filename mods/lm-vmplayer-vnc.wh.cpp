// ==WindhawkMod==
// @id           lm-vmplayer-vnc
// @name         Enable VMPlayer's VNC
// @description  Enable the builtin VNC in VMWare Player
// @version      1.0
// @author       Mark Jansen
// @github       https://github.com/learn-more
// @twitter      https://twitter.com/learn_more
// @include      vmplayer.exe
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Enable VMPlayer's VNC
By default, VMWare Player forcefully disabled the builtin VNC server.
This mod re-enables it.

Tested on VMWare Player 17.0.0 build-20800274
*/
// ==/WindhawkModReadme==


const char* g_szFind = " -s\"RemoteDisplay.vnc.enabled=FALSE\"";
const char* g_szReplace = " -s\"RemoteDisplay.vnc.enabled=TRUE\"";


char* (__cdecl* pUtilSafeStrdup0)(const char *str);
char* __cdecl UtilSafeStrdup0Hook(const char *str)
{
    if (str && !_stricmp(str, g_szFind))
    {
        Wh_Log(L"Enabling VNC(1)");
        str = g_szReplace;
    }

    return pUtilSafeStrdup0(str);
}

void* (__cdecl *pUnicode_GetAllocBytes)(const char *str, int a2);
void* __cdecl Unicode_GetAllocBytesHook(const char *str, int a2)
{
    if (str && !_stricmp(str, g_szFind))
    {
        Wh_Log(L"Enabling VNC(2)");
        str = g_szReplace;
    }

    return pUnicode_GetAllocBytes(str, a2);

}

BOOL Wh_ModInit()
{
    HMODULE mod = GetModuleHandleW(L"vmwarebase.DLL");
    Wh_Log(L"%p", mod);

    if (!mod)
        return FALSE;

    Wh_SetFunctionHook((PVOID)GetProcAddress(mod, "UtilSafeStrdup0"), (void*)UtilSafeStrdup0Hook, (void**)&pUtilSafeStrdup0);
    Wh_SetFunctionHook((PVOID)GetProcAddress(mod, "Unicode_GetAllocBytes"), (void*)Unicode_GetAllocBytesHook, (void**)&pUnicode_GetAllocBytes);

    return TRUE;
}

