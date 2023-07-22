// ==WindhawkMod==
// @id              dwm-ghost-mods
// @name            DWM Ghost Mods
// @description     Allows you to use basic or classic theme with your DWM ghost windows and more!
// @version         1.0
// @author          ephemeralViolette
// @github          https://github.com/ephemeralViolette
// @include         dwm.exe
// @compilerOptions -ldwmapi
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# DWM Ghost Mods

This mod injects into DWM and allows you to modify the behaviour of DWM ghost (not responding) windows. For
example, you can restore non-DWM window frames, making this useful for blending in non-responsive windows
with the basic or classic theme.

Currently, you can:
- Disable the DWM-rendered window frames on ghost windows.
- Modify or disable the frost effect introduced with Windows Vista, which lightens the ghost window after
  sending inputs to it a few times.
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
FrostPolicyType  g_prefFrostPolicy;

//==============================================================================================================================
// Setup, external things:
//
#include <dwmapi.h>

#include <initializer_list>
namespace YukisCoffee::WindhawkUtils
{
    struct SymbolHooks
    {
        PCWSTR symbolName;
        void *hookFunction;
        void **pOriginalFunction;
    };

    bool hookWithSymbols(HMODULE module, std::initializer_list<SymbolHooks> hooks, PCWSTR server = NULL)
    {
        WH_FIND_SYMBOL symbol;
        HANDLE findSymbol;

        if (!module)
        {
            Wh_Log(L"Loaded invalid module");
            return false;
        }

        // These functions are terribly named, which is why I wrote this wrapper
        // in the first place.
        findSymbol = Wh_FindFirstSymbol(module, server, &symbol);

        if (findSymbol)
        {
            do
            {
                for (SymbolHooks hook : hooks)
                {
                    // If the symbol is unknown, then learn it.
                    if (!*hook.pOriginalFunction && 0 == wcscmp(symbol.symbol, hook.symbolName))
                    {
                        if (hook.hookFunction)
                        {
                            // This is unsafe if you make any typo at all.
                            Wh_SetFunctionHook(
                                symbol.address,
                                hook.hookFunction,
                                hook.pOriginalFunction
                            );

                            Wh_Log(
                                L"Installed hook for symbol %s at %p.", 
                                hook.symbolName,
                                symbol.address
                            );
                        }
                        else
                        {
                            *hook.pOriginalFunction = symbol.address;

                            Wh_Log(
                                L"Found symbol %s for %p.", 
                                hook.symbolName,
                                symbol.address
                            );
                        }
                    }
                }
            }
            while (Wh_FindNextSymbol(findSymbol, &symbol));

            Wh_FindCloseSymbol(findSymbol);
        }
        else
        {
            Wh_Log(L"Unable to find symbols for module.");
            return false;
        }

        // If a requested symbol is not found at all, then error as such.
        for (SymbolHooks hook : hooks)
        {
            if (!*hook.pOriginalFunction)
            {
                Wh_Log(
                    L"Original function for symbol hook %s not found.",
                    hook.symbolName
                );

                return false;
            }
        }

        return true;
    }
}

//==============================================================================================================================
// Main implementation:
//

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

typedef HWND (*CGhostWindow__CreateGhostWindow_t)(void *);
CGhostWindow__CreateGhostWindow_t CGhostWindow__CreateGhostWindow_orig;
HWND CGhostWindow__CreateGhostWindow_Hook(void *pThis)
{
    HWND hWnd = CGhostWindow__CreateGhostWindow_orig(pThis);

    if (g_prefDisableDwmFrames)
    {
        DWMNCRENDERINGPOLICY policy = DWMNCRP_DISABLED;
        DwmSetWindowAttribute(hWnd, DWMWA_NCRENDERING_POLICY, &policy, sizeof(BOOL));
    }

    return hWnd;
}

typedef __int64 (*CGhostWindow__s_GhostWndProc_t)(HWND, int, __int64, __int64);
CGhostWindow__s_GhostWndProc_t CGhostWindow__s_GhostWndProc_orig;
__int64 CGhostWindow__s_GhostWndProc_Hook(HWND hWnd, int a, __int64 b, __int64 c)
{
    __int64 result = CGhostWindow__s_GhostWndProc_orig(hWnd, a, b, c);

    if (g_prefDisableDwmFrames)
    {
        DWMNCRENDERINGPOLICY policy = DWMNCRP_DISABLED;
        DwmSetWindowAttribute(hWnd, DWMWA_NCRENDERING_POLICY, &policy, sizeof(BOOL));
    }

    return result;
}
/*
 * DwmUpdateThumbnailProperties: Handles the painting of the thumbnail of the source window within
 *                               the ghost, as well as the frost effect.
 */
typedef BOOL (*DwmUpdateThumbnailProperties_t)(HTHUMBNAIL, DWM_THUMBNAIL_PROPERTIES *);
DwmUpdateThumbnailProperties_t DwmUpdateThumbnailProperties_orig;
BOOL DwmUpdateThumbnailProperties_Hook(HTHUMBNAIL hThumbnail, DWM_THUMBNAIL_PROPERTIES *pProperties)
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
}

// The mod is being initialized, load settings, hook functions, and do other
// initialization stuff if required.
BOOL Wh_ModInit()
{
    #ifdef _WIN64
    const int offset_same_teb_flags = 0x17EE;
    #define NT_CURRENT_TEB_T __int64
    #else
    const int offset_same_teb_flags = 0x0FCA;
    #define NT_CURRENT_TEB_T long
    #endif

    // 0x0400 = InitialThread
    bool already_running = (!*(USHORT *)(((NT_CURRENT_TEB_T *)NtCurrentTeb()) + offset_same_teb_flags) & 0x0400);

    // restart loop prevention
    if (already_running)
        return FALSE;

    Wh_Log(L"Init " WH_MOD_ID L" version " WH_MOD_VERSION);

    LoadSettings();

    HMODULE dwmghost = LoadLibrary(L"dwmghost.dll");
    HMODULE dwmapi = LoadLibrary(L"dwmapi.dll");

    if (!dwmghost && !dwmapi)
        return FALSE;

    YukisCoffee::WindhawkUtils::hookWithSymbols(dwmghost, {
        {
            L"public: int __cdecl CGhostWindow::CreateGhostWindow(void)",
            (void *)CGhostWindow__CreateGhostWindow_Hook,
            (void **)&CGhostWindow__CreateGhostWindow_orig
        },
        {
            L"public: static __int64 __cdecl CGhostWindow::s_GhostWndProc(struct HWND__ *,unsigned int,unsigned __int64,__int64)",
            (void *)CGhostWindow__s_GhostWndProc_Hook,
            (void **)&CGhostWindow__s_GhostWndProc_orig
        }
    });

    Wh_SetFunctionHook(
        (void *)GetProcAddress(dwmapi, "DwmUpdateThumbnailProperties"),
        (void *)DwmUpdateThumbnailProperties_Hook,
        (void **)&DwmUpdateThumbnailProperties_orig
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
