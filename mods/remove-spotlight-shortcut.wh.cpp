// ==WindhawkMod==
// @id              remove-spotlight-shortcut
// @name            Remove Windows Spotlight Desktop Shortcut
// @description     Removes the new Desktop Shortcut when you enable Windows Spotlight.
// @version         1.0
// @author          Erizur
// @github          https://github.com/Erizur
// @include         explorer.exe
// @compilerOptions -lcomctl32
// @architecture    x86-64
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Remove Windows Spotlight Desktop Shortcut
This mod removes the new Spotlight shortcut that appears on the desktop when you use Spotlight wallpaper mode.
Works on latest Windows 10 and LTSC 2021. Windows 11 has not been fully tested yet.

## Post-Install instructions
After installing the mod, you could either restart the explorer.exe process or refresh your desktop using the context menu.
This will make the icon not show up on your desktop anymore. Enjoy!
*/
// ==/WindhawkModReadme==

#include <minwindef.h>
#include <windhawk_utils.h>
#include <psapi.h>
#include <winnt.h>

struct DILT_WorkAreaChangeActivity;

/* Tell telemetry that the shortcut isn't enabled  (not required) */
typedef INT64 (* IconLayoutEngine_SetSpotlightIconInitialized)(void *);
IconLayoutEngine_SetSpotlightIconInitialized IconLayoutEngine_SetSpotlightIconInitialized_orig;
INT64 __fastcall IconLayoutEngine_SetSpotlightIconInitialized_hook(
    void *pThis
)
{
    return 0; //tamper!
}

typedef bool (* IconLayoutEngine_IsSpotlightIconInitialized)(void *);
IconLayoutEngine_IsSpotlightIconInitialized IconLayoutEngine_IsSpotlightIconInitialized_orig;
bool __fastcall IconLayoutEngine_IsSpotlightIconInitialized_hook(
    void *pThis
)
{
    return false;
}

typedef bool (* IconLayoutEngine_IsSpotlightIconPresent)(void *);
IconLayoutEngine_IsSpotlightIconPresent IconLayoutEngine_IsSpotlightIconPresent_orig;
bool __fastcall IconLayoutEngine_IsSpotlightIconPresent_hook(
    void *pThis
)
{
    return false;
}

/* Call to always remove the icon */
typedef INT64 (* _RemoveDesktopSpotlightIcon)(BOOL a1);
_RemoveDesktopSpotlightIcon _RemoveDesktopSpotlightIcon_orig;
INT64 __fastcall _RemoveDesktopSpotlightIcon_hook(
    BOOL a1
){
    return _RemoveDesktopSpotlightIcon_orig(a1);
}

typedef INT64 (* IconLayoutEngine_UpdateView)(void *, DILT_WorkAreaChangeActivity *);
IconLayoutEngine_UpdateView IconLayoutEngine_UpdateView_orig;
INT64 __fastcall IconLayoutEngine_UpdateView_hook(
    void *pThis,
    DILT_WorkAreaChangeActivity *a2
){
    INT64 val = IconLayoutEngine_UpdateView_orig(pThis, a2);
    _RemoveDesktopSpotlightIcon_orig(TRUE);
    return val;
}

typedef INT64 (* IconLayoutEngine_CheckIconPositionsAndUpdateSpotlightPositions)(void *);
IconLayoutEngine_CheckIconPositionsAndUpdateSpotlightPositions IconLayoutEngine_CheckIconPositionsAndUpdateSpotlightPositions_orig;
INT64 __fastcall IconLayoutEngine_CheckIconPositionsAndUpdateSpotlightPositions_hook(
    void *pThis
){
    INT64 val = IconLayoutEngine_CheckIconPositionsAndUpdateSpotlightPositions_orig(pThis);
    _RemoveDesktopSpotlightIcon_orig(TRUE);
    return val;
}

// shell32.dll
const WindhawkUtils::SYMBOL_HOOK hooks[] = {
    {
        {
            L"public: long __cdecl IconLayoutEngine::SetSpotlightIconInitialized(void)"
        },
        &IconLayoutEngine_SetSpotlightIconInitialized_orig,
        IconLayoutEngine_SetSpotlightIconInitialized_hook,
        false
    },
    {
        {
            L"private: bool __cdecl IconLayoutEngine::IsSpotlightIconInitialized(void)"
        },
        &IconLayoutEngine_IsSpotlightIconInitialized_orig,
        IconLayoutEngine_IsSpotlightIconInitialized_hook,
        false   
    },
    {
        {
            L"private: bool __cdecl IconLayoutEngine::IsSpotlightIconPresent(void)"
        },
        &IconLayoutEngine_IsSpotlightIconPresent_orig,
        IconLayoutEngine_IsSpotlightIconPresent_hook,
        false
    },
    {
        {
            L"long __cdecl _RemoveDesktopSpotlightIcon(bool)"
        },
        &_RemoveDesktopSpotlightIcon_orig,
        _RemoveDesktopSpotlightIcon_hook,
        false
    },
    {
        {
            L"public: long __cdecl IconLayoutEngine::UpdateView(class DesktopIconLayoutTelemetry::WorkAreaChangeActivity *,bool)"
        },
        &IconLayoutEngine_UpdateView_orig,
        IconLayoutEngine_UpdateView_hook,
        false
    },
    {
        {
            L"public: long __cdecl IconLayoutEngine::CheckIconPositionsAndUpdateSpotlightPositions(void)"
        },
        &IconLayoutEngine_CheckIconPositionsAndUpdateSpotlightPositions_orig,
        IconLayoutEngine_CheckIconPositionsAndUpdateSpotlightPositions_hook,
        false
    }
};

BOOL Wh_ModInit(void)
{
    HMODULE hShell32 = GetModuleHandleW(L"shell32.dll");
    if (!hShell32)
    {
        Wh_Log(L"Failed to load shell32.dll");
        return FALSE;
    }

    if (!WindhawkUtils::HookSymbols(
        hShell32,
        hooks,
        ARRAYSIZE(hooks)
    ))
    {
        Wh_Log(L"Failed to hook functions!");
        return FALSE;
    }
    return TRUE;
}