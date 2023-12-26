// ==WindhawkMod==
// @id              dwm-ghost-mods
// @name            DWM Ghost Mods
// @description     Allows you to use basic or classic theme with your DWM ghost windows and more!
// @version         1.2
// @author          ephemeralViolette
// @github          https://github.com/ephemeralViolette
// @include         dwm.exe
// @compilerOptions -ldwmapi -luxtheme
// @architecture    x86-64
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# DWM Ghost Mods

This mod injects into DWM and allows you to modify the behaviour of DWM ghost (not responding) windows. For
example, you can restore non-DWM window frames, making this useful for blending in non-responsive windows
with the basic or classic theme.

![Preview image](https://raw.githubusercontent.com/ephemeralViolette/images/main/dwmghostmods.png)

**This mod should be used with a tool such as BasicThemer or ClassicThemeTray.** It enhances the behaviour of
these tools, and will not work properly with standard DWM window frames.

Currently, you can:
- Disable the DWM-rendered window frames on ghost windows.
- Modify or disable the frost effect introduced with Windows Vista, which lightens the ghost window after
  sending inputs to it a few times.

# ⚠ Important usage note ⚠

In order to use this mod, you must enable Windhawk to inject into system processes in its advanced settings.
If you do not do this, it will silently fail to inject. **Changing the Windhawk advanced settings will also
affect any other mod you have installed, and may cause instability as any other mod that injects into all
processes will now inject into system processes too.**

This mod will not work on portable versions of Windhawk because DWM is a protected process and can only be
modified by a system account. Since the portable version of Windhawk only runs as administrator under your
own user account, it will not have the privilege required to inject into DWM. (You may not be so lucky with
forcing the portable version to run as `NT AUTHORITY\SYSTEM` either, as this didn't work in my testing.)

# Tested compatible Windows versions

The mod is tested on the following versions of Windows and is confirmed to work as intended.

- ✅ Windows 8.1 (build 9600)
- ✅ Windows 10 version 1607 (build 14393)
- ✅ Windows 10 version 1809 (build 17763)
- ✅ Windows 10 version 20H2 (build 19042)
- ✅ Windows 10 version 21H1 (build 19043)
- ✅ Windows 10 version 21H2 (build 19044)
- ✅ Windows 11 version 22H2 (build 22621)

# Easy testing

You can use [BadApp](https://www.ntwind.com/freeware/badapp.html) to easily test a hung process.

# Common problems

## Failed to download symbols:

Sometimes, Windhawk may fail to download the symbols for DWM. This doesn't necessarily mean that your version
of Windows is incompatible, but it means that DWM cannot connect to the internet (which is how Windhawk
attempts to download symbols).

You can work around this issue by manually downloading the symbols and dropping them in the Windhawk symbols
folder. In order to do this, use a tool such as [PDB Downloader](https://github.com/rajkumar-rangaraj/PDB-Downloader/releases)
and download the symbols for `%SystemRoot%\System32\dwmghost.dll`.

The standard (non-portable) version of Windhawk stores symbols in `C:\ProgramData\Windhawk\Engine\Symbols`.

If you encounter this error, a message box will be displayed warning you of the failure.

# Known issues

- On Windows 10 and Windows 11, if the source window has DWM frames (you aren't using BasicThemer or anything like 
  that), the thumbnail of the source window inside the ghost window will be overly-cropped, leaving white borders 
  behind. This is because of the 1 pixel visual borders used for DWM top-level windows since Windows 10.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
# g_prefDisableDwmFrames
- disableDwmFrames: true
  $name: Disable DWM frames (use Basic or Classic theme)
  $description: >-
    Tools like BasicThemer will often not affect ghost windows since they are not reported to their
    observers. This will force the basic appearance for that window.
# g_prefUseClassicTheme
- useClassicTheme: false
  $name: Use classic theme
  $description: >-
    Enables the classic theme in DWM child windows (such as ghosts). This will technically affect any
    other framed window spawned by DWM, but I think ghosts are the only ones that do that.


    If this is not set, then the window will use the UXTheme frames, even if the classic theme is
    otherwise set (i.e. with ClassicThemeTray). This is because the theme is (and must be) kept open
    in DWM or else it will crash.
# g_prefFrostPolicy
- frostPolicy: basic
  $name: Frost behaviour
  $description: >-
    Modifies the "frost" effect when sending inputs to an unresponsive window.
  $options:
  - disabled: Disabled (like XP)
  - basic: No fade (like Vista/7 Basic)
  - enabled: Enabled with fade (default DWM effect)
*/
// ==/WindhawkModSettings==

enum class FrostPolicyType
{
    DISABLED,
    BASIC,
    ENABLED
};

//==============================================================================================================================
// Global properties:
//
bool             g_prefDisableDwmFrames;
bool             g_prefUseClassicTheme;
FrostPolicyType  g_prefFrostPolicy;

//==============================================================================================================================
// Setup, external things:
//
#include <windhawk_utils.h>
#include <VersionHelpers.h>
#include <dwmapi.h>
#include <uxtheme.h>

//==============================================================================================================================
// Main implementation:
//

/*
 * DisplayErrorMessage: Open and activate a message box that informs the user of an issue with
 *                      the mod.
 */
void DisplayErrorMessage(LPCWSTR text)
{
    MessageBoxW(
        NULL,
        text,
        L"Windhawk : DWM Ghost Mods",
        MB_OK | MB_ICONERROR
    );
}

typedef struct
{
    int topBorderHeight;
    int leftRightBottomBorderSize;
} NCWindowSizes;

/*
 * GetNCWindowSizes: Get the dimensions of the non-client area as rendered on non-DWM windows.
 */
HRESULT GetNCWindowSizes(NCWindowSizes *out)
{
    RECT base;
    base.left = 0;
    base.top = 0;
    base.right = 500;
    base.bottom = 500;

    AdjustWindowRectEx(&base, WS_CAPTION, FALSE, NULL);

    int leftRightBorderWidth = ((base.right - base.left) - 500) / 2;
    int topBorderHeight = (base.bottom - base.top) - 500 - leftRightBorderWidth;

    out->topBorderHeight = topBorderHeight;
    out->leftRightBottomBorderSize = leftRightBorderWidth;

    return S_OK;
}

/*
 * ApplyDesiredWindowTheme: Apply basic frames or the classic theme to a specified DWM child window. 
 */
void ApplyDesiredWindowTheme(HWND hWnd)
{
    if (g_prefUseClassicTheme)
    {
        SetThemeAppProperties(STAP_ALLOW_CONTROLS | STAP_ALLOW_WEBCONTENT);
    }
    else
    {
        SetThemeAppProperties(STAP_ALLOW_NONCLIENT | STAP_ALLOW_CONTROLS | STAP_ALLOW_WEBCONTENT);
    }

    if (g_prefDisableDwmFrames)
    {
        DWMNCRENDERINGPOLICY policy = DWMNCRP_DISABLED;
        DwmSetWindowAttribute(hWnd, DWMWA_NCRENDERING_POLICY, &policy, sizeof(BOOL));
    }
}

typedef HWND (*CGhostWindow__CreateGhostWindow_t)(void *);
CGhostWindow__CreateGhostWindow_t CGhostWindow__CreateGhostWindow_orig;
HWND CGhostWindow__CreateGhostWindow_hook(void *pThis)
{
    HWND hWnd = CGhostWindow__CreateGhostWindow_orig(pThis);

    ApplyDesiredWindowTheme(hWnd);

    return hWnd;
}

typedef __int64 (*CGhostWindow__s_GhostWndProc_t)(HWND, int, __int64, __int64);
CGhostWindow__s_GhostWndProc_t CGhostWindow__s_GhostWndProc_orig;
__int64 CGhostWindow__s_GhostWndProc_hook(HWND hWnd, int a, __int64 b, __int64 c)
{
    __int64 result = CGhostWindow__s_GhostWndProc_orig(hWnd, a, b, c);

    ApplyDesiredWindowTheme(hWnd);

    return result;
}
/*
 * DwmUpdateThumbnailProperties: Handles the painting of the thumbnail of the source window within
 *                               the ghost, as well as the frost effect.
 */
typedef BOOL (*DwmUpdateThumbnailProperties_t)(HTHUMBNAIL, DWM_THUMBNAIL_PROPERTIES *);
DwmUpdateThumbnailProperties_t DwmUpdateThumbnailProperties_orig;
BOOL DwmUpdateThumbnailProperties_hook(HTHUMBNAIL hThumbnail, DWM_THUMBNAIL_PROPERTIES *pProperties)
{
    // If DWM frames are used, then the position of the thumbnail needs to be moved or it will
    // render at { 0, 0 } inside the window frames.
    if (g_prefDisableDwmFrames)
    {
        SIZE thumbSrcSize;
        DwmQueryThumbnailSourceSize(hThumbnail, &thumbSrcSize);

        NCWindowSizes ncWindowSizes;
        GetNCWindowSizes(&ncWindowSizes);

        pProperties->dwFlags |= DWM_TNP_RECTDESTINATION | DWM_TNP_RECTSOURCE;

        // Calculate source positions
        pProperties->rcSource.top = ncWindowSizes.topBorderHeight;
        pProperties->rcSource.left = ncWindowSizes.leftRightBottomBorderSize;
        pProperties->rcSource.right = thumbSrcSize.cx - ncWindowSizes.leftRightBottomBorderSize;
        pProperties->rcSource.bottom = thumbSrcSize.cy - ncWindowSizes.leftRightBottomBorderSize;

        // We reuse the same values that we used to crop the source to translate the destination.
        // No additional work is required here.
        pProperties->rcDestination = pProperties->rcSource;
    }

    if (g_prefFrostPolicy == FrostPolicyType::BASIC)
    {
        if (pProperties->opacity < 255)
        {
            pProperties->opacity = 128;
        }
    }
    else if (g_prefFrostPolicy == FrostPolicyType::DISABLED)
    {
        if (pProperties->opacity < 255)
        {
            pProperties->opacity = 255;
        }
    }

    return DwmUpdateThumbnailProperties_orig(hThumbnail, pProperties);
}

void LoadSettings()
{
    g_prefDisableDwmFrames = Wh_GetIntSetting(L"disableDwmFrames");
    g_prefUseClassicTheme = Wh_GetIntSetting(L"useClassicTheme");

    PCWSTR pszFrostPolicy = Wh_GetStringSetting(L"frostPolicy");
    
    if (lstrcmpW(pszFrostPolicy, L"enabled") == 0)
    {
        g_prefFrostPolicy = FrostPolicyType::ENABLED;
    }
    else if (lstrcmpW(pszFrostPolicy, L"basic") == 0)
    {
        g_prefFrostPolicy = FrostPolicyType::BASIC;
    }
    else
    {
        g_prefFrostPolicy = FrostPolicyType::DISABLED;
    }

    Wh_FreeStringSetting(pszFrostPolicy);

    // Validate:
    if (g_prefUseClassicTheme && !g_prefDisableDwmFrames)
    {
        DisplayErrorMessage(
            L"Loaded invalid settings. In order to enable the classic theme on dialogs, "
            L"you must also enable the \"Disable DWM frames\" setting. Please enable that "
            L"option and try again."
        );

        g_prefUseClassicTheme = FALSE;
    }
}

const WindhawkUtils::SYMBOL_HOOK hooks[] = {
    {
        {
            L"public: int __cdecl CGhostWindow::CreateGhostWindow(void)"
        },
        &CGhostWindow__CreateGhostWindow_orig,
        CGhostWindow__CreateGhostWindow_hook,
        false
    },
    {
        {
            L"public: static __int64 __cdecl CGhostWindow::s_GhostWndProc(struct HWND__ *,unsigned int,unsigned __int64,__int64)"
        },
        &CGhostWindow__s_GhostWndProc_orig,
        CGhostWindow__s_GhostWndProc_hook,
        false
    }
};

// The mod is being initialized, load settings, hook functions, and do other
// initialization stuff if required.
BOOL Wh_ModInit()
{
    Wh_Log(L"Init " WH_MOD_ID L" version " WH_MOD_VERSION);

    LoadSettings();

    HMODULE dwmghost;
    HMODULE dwmapi = LoadLibraryW(L"dwmapi.dll");

    // Prior to Windows 10, ghosting is implemented directly into dwm.exe.
    if (IsWindows10OrGreater())
    {
        dwmghost = LoadLibraryW(L"dwmghost.dll");
    }
    else
    {
        dwmghost = GetModuleHandleW(NULL);
    }

    if (!dwmghost && !dwmapi)
    {
        DisplayErrorMessage(
            L"Failed to load both dwmghost.dll and dwmapi.dll. This should never happen."
        );

        return FALSE;
    }
    else if (!dwmghost)
    {
        DisplayErrorMessage(
            L"Failed to load dwmghost.dll. This most likely means that you are using an "
            L"early beta version of Windows 10, which is unsupported."
        );

        return FALSE;
    }
    else if (!dwmapi)
    {
        DisplayErrorMessage(
            L"Failed to load dwmapi.dll."
        );

        return FALSE;
    }

    bool hookSuccess = WindhawkUtils::HookSymbols(dwmghost, hooks, ARRAYSIZE(hooks));

    if (!hookSuccess)
    {
        DisplayErrorMessage(
            L"Failed to download symbols. \r\n\r\n"
            L"This is most likely an issue with Windhawk rather than your version of Windows. "
            L"Please see the \"Details\" page of the mod for more details."
        );

        return FALSE;
    }

    WindhawkUtils::Wh_SetFunctionHookT(
        (DwmUpdateThumbnailProperties_t)GetProcAddress(dwmapi, "DwmUpdateThumbnailProperties"),
        DwmUpdateThumbnailProperties_hook,
        &DwmUpdateThumbnailProperties_orig
    );

    return TRUE;
}

// The mod is being unloaded, free all allocated resources.
void Wh_ModUninit()
{
    Wh_Log(L"Uninit");
}

// The mod setting were changed, reload them.
void Wh_ModSettingsChanged()
{
    Wh_Log(L"SettingsChanged");
    LoadSettings();
}
