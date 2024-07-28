// ==WindhawkMod==
// @id              eradicate-immersive-menus
// @name            Eradicate Immersive Menus
// @description     Gets rid of immersive menus system-wide
// @version         1.1.0
// @author          aubymori
// @github          https://github.com/aubymori
// @include         *
// @exclude         windhawk.exe
// @exclude         dwm.exe
// @exclude         ShellExperienceHost.exe
// @exclude         wlanext.exe
// @exclude         svchost.exe
// @exclude         SearchIndexer.exe
// @exclude         consent.exe
// @compilerOptions -luser32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Eradicate Immersive Menus
Gets rid of immersive menus system-wide. This mod should work on every Windows 10 version, from 1507
all the way up to 22H2. However, **this mod will only work on Windhawk v1.4 and greater.**

## Preview

*From left to right: Explorer, Taskbar, Explorer (classic), Taskbar (classic)*

**Before**:

![Before](https://raw.githubusercontent.com/aubymori/images/main/eradicate-immersive-menus-before.png)

**After**:
![After](https://raw.githubusercontent.com/aubymori/images/main/eradicate-immersive-menus-after.png)

# Common problems

## Freezing

This mod will take **a while** to load on the first run, and if you disable and then enable it while your system is all
already up and running with many software open.

## Stuck on "Initializing..."

If ever a certain process gets stuck on "Initializing...", and the "Mod tasks in progress" window does not go away,
restart Windhawk. If that does not work, simply add it to the exclusion list in the Advanced tab of this mod.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- explorer: true
  $name: Apply to Explorer
  $description: Applies to Explorer itself, including desktop, navigation pane, main Explorer view, open/save dialogs, and the main taskbar menu.
- nosettingsicon: true
  $name: No "Taskbar settings" icon
  $description: Removes the icon from the "Taskbar settings" entry in the taskbar menu
- sound: true
  $name: Apply to Volume tray icon
- network: true
  $name: Apply to Network tray icon
- defender: true
  $name: Apply to Windows Security
- narrator: true
  $name: Apply to Narrator
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

struct {
    BOOL explorer;
    /* Tray icons are always 64-bit */
#ifdef _WIN64
    BOOL sound;
    BOOL network;
    BOOL defender;
    BOOL nosettingsicon;
    BOOL narrator;
#endif
} settings;

/* ImmersiveContextMenuHelper::CanApplyOwnerDrawToMenu */
typedef bool (__fastcall *ICMH_CAODTM_t)(HMENU, HWND);

#define ORIG(MODULE) \
ICMH_CAODTM_t ICMH_CAODTM_orig_ ## MODULE = nullptr;

ORIG(shell32)
ORIG(ExplorerFrame)
/* Explorer and tray icons are always 64-bit */
#ifdef _WIN64
ORIG(explorer)
ORIG(twinui)
ORIG(twinui_pcshell)
ORIG(SndVolSSO)
ORIG(pnidui)
ORIG(SecurityHealthSSO)
ORIG(Narrator)
#endif

/** 
  * __fastcall is needed, or else stack corruption occurs
  * in x86-32 applications.
  */
bool __fastcall ICMH_CAODTM_hook(
    HMENU hMenu,
    HWND hWnd
)
{
    return false;
}

/* Prevent "Taskbar settings" icon from being set */
#ifdef _WIN64
using SetMenuItemInfoW_t = decltype(&SetMenuItemInfoW);
SetMenuItemInfoW_t SetMenuItemInfoW_orig;
BOOL SetMenuItemInfoW_hook(
    HMENU            hMenu,
    UINT             item,
    BOOL             fByPosition,
    LPCMENUITEMINFOW lpmii
)
{
    /* homestuck reference??? */
    if (settings.nosettingsicon && (!fByPosition && item == 413))
    {
        return TRUE;
    }

    return SetMenuItemInfoW_orig(
        hMenu,
        item,
        fByPosition,
        lpmii
    );
}
#endif

/**
  * Hooks ImmersiveContextMenuHelper::CanApplyOwnerDrawToMenu for a given DLL.
  * There are many DLLs that have this function, so each one needs to be accounted for.
  */
inline bool HookICMH_CAODTM(LPCWSTR lpDll, ICMH_CAODTM_t *pOrig)
{
    HMODULE hModule;

    /* If lpDll is NULL, patch the current process instead */
    if (lpDll == NULL)
    {
        hModule = GetModuleHandleW(NULL);
    }
    else
    {
        hModule = LoadLibraryW(lpDll);
    }

    if (!hModule)
    {
        Wh_Log(L"Failed to load %s", lpDll);
        return false;
    }

    WindhawkUtils::SYMBOL_HOOK hook = {
        {
            L"bool "
#ifdef _WIN64
            L"__cdecl"
#else
            L"__stdcall"
#endif
            L" ImmersiveContextMenuHelper::CanApplyOwnerDrawToMenu(struct HMENU__ *,struct HWND__ *)"
        },
        pOrig,
        ICMH_CAODTM_hook,
        true
    };

    bool bHookSuccess = WindhawkUtils::HookSymbols(
        hModule,
        &hook,
        1
    );

    if (bHookSuccess)
    {
        return true;
    }

    Wh_Log(
        L"Failed to hook ImmersiveContextMenuHelper::CanApplyOwnerDrawToMenu in %s.",
        lpDll ? lpDll : L"explorer.exe"
    );
    return false;
}

#define LoadIntSetting(NAME) \
settings.NAME = Wh_GetIntSetting(L ## #NAME)

void LoadSettings()
{
    LoadIntSetting(explorer);
    /* Tray icons are always 64-bit */
#ifdef _WIN64
    LoadIntSetting(sound);
    LoadIntSetting(network);
    LoadIntSetting(defender);
    LoadIntSetting(nosettingsicon);
    LoadIntSetting(narrator);
#endif
}

#define HOOK(MODULE) \
HookICMH_CAODTM(L ## #MODULE L".dll", &ICMH_CAODTM_orig_ ## MODULE)

#define HOOK_SAFE(MODULE, MODULE_SAFE) \
HookICMH_CAODTM(L ## MODULE L".dll", &ICMH_CAODTM_orig_ ## MODULE_SAFE)

#define HOOK_SELF(NAME) \
HookICMH_CAODTM(NULL, &ICMH_CAODTM_orig_ ## NAME)

BOOL Wh_ModInit()
{
    LoadSettings();

    /* Explorer is always 64-bit */
#ifdef _WIN64
    WCHAR szProcessPath[MAX_PATH];
    GetModuleFileNameW(NULL, szProcessPath, MAX_PATH);
    bool bIsExplorer = (0 == wcsicmp(szProcessPath, L"C:\\Windows\\explorer.exe"));

    if (settings.nosettingsicon && bIsExplorer)
    {
        Wh_SetFunctionHook(
            (void *)SetMenuItemInfoW,
            (void *)SetMenuItemInfoW_hook,
            (void **)&SetMenuItemInfoW_orig
        );
    }
#endif

    if (settings.explorer
#ifdef _WIN64
    && 0 != wcsicmp(szProcessPath, L"C:\\Windows\\System32\\SecurityHealthSystray.exe")
#endif
    )
    {
        HOOK(shell32);
        HOOK(ExplorerFrame);
        
        /* Explorer itself is always 64-bit */
#ifdef _WIN64
        if (bIsExplorer)
        {
            HOOK_SELF(explorer);
            HOOK(twinui);
            HOOK_SAFE("twinui.pcshell", twinui_pcshell);
        }
#endif
    }

    /* Tray icons are always 64-bit */
#ifdef _WIN64
    if (bIsExplorer)
    {
        if (settings.sound)
        {
            HOOK(SndVolSSO);
        }

        if (settings.network)
        {
            HOOK(pnidui);
        }
    }

    if (settings.defender
    &&  0 == wcsicmp(szProcessPath, L"C:\\Windows\\System32\\SecurityHealthSystray.exe"))
    {
        HOOK(SecurityHealthSSO);
    }

    if (settings.narrator
    &&  0 == wcsicmp(szProcessPath, L"C:\\Windows\\System32\\Narrator.exe"))
    {
        HOOK_SELF(Narrator);
    }
#endif

    return TRUE;
}

BOOL Wh_ModSettingsChanged(BOOL *bReload)
{
    *bReload = TRUE;
    return TRUE;
}