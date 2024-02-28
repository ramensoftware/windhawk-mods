// ==WindhawkMod==
// @id              classic-uwp-fix
// @name            Classic UWP Fix
// @description     Fix for UWPs breaking with Classic theme
// @version         0.3
// @author          Ingan121
// @github          https://github.com/Ingan121
// @homepage        https://www.ingan121.com/
// @include         ApplicationFrameHost.exe
// @include         explorer.exe
// @include         ShellAppRuntime.exe
// @include         CustomShellHost.exe
// @architecture    x86-64
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Classic UWP Fix
This mod fixes UWP apps getting stuck at the splash screen when using the Classic theme.

No more need to run ApplicationFrameHost before enabling Classic!

This works by hardcoding the splash fade animation values instead of fetching them from msstyles.
*/
// ==/WindhawkModReadme==

#include <windhawk_utils.h>

typedef INT64 (* WINAPI AnimationHelpers__LoadThemeTransform_t)(void *, UINT, INT64 *, int, INT64 *, struct TA_TIMINGFUNCTION *);
AnimationHelpers__LoadThemeTransform_t AnimationHelpers__LoadThemeTransform_orig;
INT64 WINAPI AnimationHelpers__LoadThemeTransform_hook(
    void *pThis,
    UINT a2,
    INT64 *out,
    int type,
    INT64 *a5,
    struct TA_TIMINGFUNCTION *a6
){
    if (type == 28) {
        out[0] = 2;
        out[1] = 0;
        out[2] = 2;
        out[3] = 1.0f;
        out[4] = 1;
        out[5] = 0;
        out[6] = 1.0f;
    } else {
        AnimationHelpers__LoadThemeTransform_orig(pThis, a2, out, type, a5, a6);
    }
    return 0;
}

// The mod is being initialized, load settings, hook functions, and do other
// initialization stuff if required.
BOOL Wh_ModInit() {
    Wh_Log(L"Init");

    HMODULE hAppFrameModule = LoadLibraryW(L"ApplicationFrame.dll");
    if(!hAppFrameModule) {
        Wh_Log(L"Failed to load ApplicationFrame.dll");
        return FALSE;
    }
    WindhawkUtils::SYMBOL_HOOK hooks[] = {
        {
            {
                L"long __cdecl AnimationHelpers::LoadThemeTransform(int,int,struct TA_TRANSFORM *,unsigned long,struct TA_TIMINGFUNCTION *,unsigned long)"
            },
            (void **)&AnimationHelpers__LoadThemeTransform_orig,
            (void *)AnimationHelpers__LoadThemeTransform_hook,
            false
        }
    };

    if (!WindhawkUtils::HookSymbols(
        hAppFrameModule,
        hooks,
        ARRAYSIZE(hooks)
    )) {
        Wh_Log(L"Failed to hook");
        return FALSE;
    }

    return TRUE;
}

// The mod is being unloaded, free all allocated resources.
void Wh_ModUninit() {
    Wh_Log(L"Uninit");
}
