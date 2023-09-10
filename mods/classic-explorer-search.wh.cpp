// ==WindhawkMod==
// @id              classic-explorer-search
// @name            Classic Explorer Search
// @description     Re-enables the classic search box in Explorer.
// @version         1.0.0
// @author          aubymori
// @github          https://github.com/aubymori
// @include         *
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Classic Explorer Search

Re-enables the classic search box in Explorer.

![Preview](https://raw.githubusercontent.com/aubymori/images/main/classic-explorer-search.png)
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- enable: false
  $name: Enable modern search
  $description: Enable the modern search box. Enabling this will force it on, even in places where it would not normally be (e.g. Control Panel).
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

bool bEnable;

typedef bool (* CUniversalSearchBand_IsModernSearchBoxEnabled_t)(void);
CUniversalSearchBand_IsModernSearchBoxEnabled_t CUniversalSearchBand_IsModernSearchBoxEnabled_orig;
bool CUniversalSearchBand_IsModernSearchBoxEnabled_hook(void)
{
    return bEnable;
}

void LoadSettings(void)
{
    bEnable = Wh_GetIntSetting(L"enable");
}

void Wh_ModSettingsChanged(void)
{
    LoadSettings();
}

BOOL Wh_ModInit(void)
{
    LoadSettings();

    HMODULE hExplorerFrame = LoadLibraryW(L"ExplorerFrame.dll");

    if (!hExplorerFrame)
    {
        Wh_Log(L"Failed to load ExplorerFrame.dll");
        return FALSE;
    }

    WindhawkUtils::SYMBOL_HOOK hook = {
        {
            L"private: bool "
            #ifdef _WIN64
            L"__cdecl"
            #else
            L"__thiscall"
            #endif
            L" CUniversalSearchBand::IsModernSearchBoxEnabled(void)"
        },
        (void **)&CUniversalSearchBand_IsModernSearchBoxEnabled_orig,
        (void *)CUniversalSearchBand_IsModernSearchBoxEnabled_hook,
        false
    };

    if (!HookSymbols(hExplorerFrame, &hook, 1))
    {
        Wh_Log(L"Failed to hook CUniversalSearchBand::IsModernSearchBoxEnabled");
        return FALSE;
    }

    return TRUE;
}