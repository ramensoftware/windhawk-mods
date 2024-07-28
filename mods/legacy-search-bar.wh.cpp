// ==WindhawkMod==
// @id              legacy-search-bar
// @name            Legacy Search Bar
// @description     Restores the search box from before Windows 10, version 1909
// @version         1.0.0
// @author          aubymori
// @github          https://github.com/aubymori
// @include         *
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Legacy Search Bar
Since Windows 10, version 1909, the legacy search box, which was based on DirectUI,
was replaced by one that uses EdgeHTML. As well, when enabling the DirectUI one,
a bug occurs where the placeholder text disappars initially and on a resize. This mod
re-enables the DirectUI search box and fixes the bug with the placeholder.

**This mod requires Windhawk v1.4 or greater.**

**Before**:

![Before](https://raw.githubusercontent.com/aubymori/images/main/legacy-search-bar-before.png)

**After**:

![After](https://raw.githubusercontent.com/aubymori/images/main/legacy-search-bar-after.png)
*/
// ==/WindhawkModReadme==

#include <windhawk_utils.h>

#ifdef _WIN64
#   define THISCALL  __cdecl
#   define STHISCALL L"__cdecl"
#else
#   define THISCALL  __thiscall
#   define STHISCALL L"__thiscall"
#endif

/* Re-enable the DirectUI search box. */
bool (THISCALL *CUniversalSearchBand_IsModernSearchBoxEnabled_orig)(void *);
bool THISCALL CUniversalSearchBand_IsModernSearchBoxEnabled_hook(
    void *pThis
)
{
    return false;
}

/**
  * Fix the bug where the DirectUI search box's placeholder blanks out
  * initially and on a resize.
  *
  * For some ungodly reason, calling this function will clear the
  * DirectUI search box's placeholder text.
  *
  * This function was previously only called if the feature flag for
  * EdgeHTML search box ("SearchSuggestions") (18755234) was enabled.
  * However, in Vibranium's ExplorerFrame.dll, up until 19045.3754,
  * this feature flag is completely inlined. That is to say, its
  * enabled state is forced enabled. Any references to its
  * __private__IsEnabled function are completely gone, and any code
  * that may have been locked behind the feature being disabled goes
  * completely uncompiled.
  *
  * Even then, completely unmodded, Control Panel will use the DirectUI
  * search box, even with this feature flag enbled. As such, the bug
  * will be visible there too.
  */
long (THISCALL *CSearchEditBox_HideSuggestions_orig)(void *pThis);
long THISCALL CSearchEditBox_HideSuggestions_hook(
    void *pThis
)
{
    return 0;
}

BOOL Wh_ModInit(void)
{
    HMODULE hExplFrame = LoadLibraryW(L"ExplorerFrame.dll");
    if (!hExplFrame)
    {
        Wh_Log(L"Failed to load ExplorerFrame.dll");
        return FALSE;
    }

    WindhawkUtils::SYMBOL_HOOK hooks[] = {
        {
            {
                L"private: bool "
                STHISCALL
                L" CUniversalSearchBand::IsModernSearchBoxEnabled(void)"
            },
            &CUniversalSearchBand_IsModernSearchBoxEnabled_orig,
            CUniversalSearchBand_IsModernSearchBoxEnabled_hook,
            false
        },
        {
            {
                L"public: long "
                STHISCALL
                L" CSearchEditBox::HideSuggestions(void)"
            },
            &CSearchEditBox_HideSuggestions_orig,
            CSearchEditBox_HideSuggestions_hook,
            false
        }
    };

    if (!WindhawkUtils::HookSymbols(
        hExplFrame,
        hooks,
        ARRAYSIZE(hooks)
    ))
    {
        Wh_Log(L"Failed to hook one or more symbol functions in ExplorerFrame.dll");
        return FALSE;
    }

    return TRUE;
}