// ==WindhawkMod==
// @id              msg-box-font-fix
// @name            Message Box Font Fix
// @description     Fixes the MessageBox font size in 1709+
// @version         1.0
// @author          aubymori
// @github          https://github.com/aubymori
// @include         *
// @compilerOptions -luser32 -lgdi32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Message Box Font Fix
Starting with Windows 10 1709, message boxes render their font size 1pt less than the
user-defined size.\* You cannot just set this size higher, as many applications still query
it, and will show up with bigger fonts.

This mod fixes that.

**Before:**

![Before](https://raw.githubusercontent.com/aubymori/images/main/message-box-font-fix-before.png)

**After:**

![After](https://raw.githubusercontent.com/aubymori/images/main/message-box-font-fix-after.png)

*\*Microsoft changed the way the font size was calculator for Per-Monitor V2 DPI awareness. It ALWAYS gets
1pt below the font size, even when on a higher DPI. This is because Microsoft decided to do some weird math
instead of just using `SystemParametersInfoW` like a normal person.*
*/
// ==/WindhawkModReadme==

#pragma region "WindhawkUtils"
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
#pragma endregion // "WindhawkUtils"

typedef HFONT (*GetMessageBoxFontForDpi_t)(UINT);
GetMessageBoxFontForDpi_t GetMessageBoxFontForDpi_orig;
HFONT GetMessageBoxFontForDpi_hook(
    UINT nDpi
)
{
    NONCLIENTMETRICSW ncm;
    ncm.cbSize = sizeof(NONCLIENTMETRICSW);

    SystemParametersInfoW(
        SPI_GETNONCLIENTMETRICS,
        sizeof(NONCLIENTMETRICSW),
        &ncm,
        0
    );

    return CreateFontIndirectW(&(ncm.lfMessageFont));
}

BOOL Wh_ModInit()
{
    Wh_Log(L"Initializing Message Box Font Fix");

    HMODULE hUser32 = LoadLibraryW(L"user32.dll");

    if (!hUser32)
    {
        MessageBoxW(
            NULL,
            L"Failed to load user32.dll. There is something seriously wrong with your Windows install or Windhawk.",
            L"Windhawk: Message Box Font Fix",
            MB_ICONERROR
        );
        return FALSE;
    }

    bool bHookSuccess = YukisCoffee::WindhawkUtils::hookWithSymbols(
        hUser32,
        {
            {
                L"struct HFONT__ * __cdecl GetMessageBoxFontForDpi(unsigned int)",
                (void *)GetMessageBoxFontForDpi_hook,
                (void **)&GetMessageBoxFontForDpi_orig
            }
        }
    );

    if (!bHookSuccess)
    {
        // Many applications will always fail to hook this.
        // Coincidentally, none of these ever use MessageBox.
        // At least for me. Anyways, silently fail to log,
        // we don't want to bombard the user with a million
        // error message boxes.
        Wh_Log(L"Failed to hook GetMessageBoxFontForDpi");
        return FALSE;
    }

    Wh_Log(L"Done initializing Message Box Font Fix");
    return TRUE;
}