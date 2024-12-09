// ==WindhawkMod==
// @id              shadowplay-do-not-disable
// @name            Shadowplay anti-disable
// @description     Prevent Nvidia ShadowPlay from disabling itself
// @version         1.0
// @author          Temm
// @github          https://github.com/leumasme
// @include         nvcontainer.exe
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Shadowplay anti-disable
Nvidia ShadowPlay will normally turn off automatically/pause when detecting DRM content playing or when a Window requests to be hidden from screen captures.
This sucks, because ShadowPlay may just randomly turn off because a random window requests to be hidden from screen captures.
No more! This mod disables both of these checks.

Works with both the old "GeForce Experience" as well as the new "NVIDIA App"

![Screenshot](https://i.imgur.com/zdP0nhd.png)
*/
// ==/WindhawkModReadme==

#include <tlhelp32.h>
#include <windhawk_api.h>

BOOL WINAPI GetWindowDisplayAffinity_Hook(IN HWND hWnd, OUT DWORD *pwdAffinity) {
    // Drivers try to not record windows that have their Window Display Affinity set to anything other than NONE,
    // since those windows request to not be included on screen captures.
    // These windows are also hidden (transparent/absent) on normal screenshots and screensharing software, but the way the Nvidia
    // driver records, it would also capture these hidden windows - so it just refuses to record instead.
    // Block this by just returning NONE affinity.
    *pwdAffinity = WDA_NONE; 
    return TRUE;
}

BOOL WINAPI Module32FirstW(IN HANDLE hSnapshot, IN OUT LPMODULEENTRY32W lpme) {
    // Drivers try to detect DRM dll being loaded into browsers by creating a toolhelp module snapshot
    // and then iterating over the modules, starting with this function.
    // Block this by just not writing the module entry and returning false to indicate no further modules.
    return FALSE;
}


BOOL Wh_ModInit() {
    Wh_Log(L"Init");

    HMODULE hUser32 = GetModuleHandle(L"user32.dll");
    void* hookingGetWindowDisplayAffinity = (void*)GetProcAddress(hUser32, "GetWindowDisplayAffinity");
    Wh_SetFunctionHook(hookingGetWindowDisplayAffinity, (void*)GetWindowDisplayAffinity_Hook, NULL);

    HMODULE hKernel32 = GetModuleHandle(L"kernel32.dll");
    void* hookingModule32FirstW = (void*)GetProcAddress(hKernel32, "Module32FirstW");
    Wh_SetFunctionHook(hookingModule32FirstW, (void*)Module32FirstW, NULL);

    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L"Uninit");
}
