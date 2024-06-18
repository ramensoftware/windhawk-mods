// ==WindhawkMod==
// @id              change-explorer-default-location
// @name            Change Explorer Default Location
// @description     Allows you to change what shell location or directory Explorer starts in.
// @version         1.0.0
// @author          aubymori
// @github          https://github.com/aubymori
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lshell32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Change Explorer Default Location
This mod allows you to change the shell location or directory where Explorer starts.
The directory can be either a regular path, or a shell location. A list of CLSIDs
for shell locations in Windows 10 can be found
[here](https://www.tenforums.com/tutorials/3123-clsid-key-guid-shortcuts-list-windows-10-a.html).

Shell locations must be prefixed with `shell:::` (e.g. `shell:::{031E4825-7B94-4dc3-B131-E946B44C8DD5}`).
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- location: "shell:::{031E4825-7B94-4dc3-B131-E946B44C8DD5}"
  $name: Location
  $description: The directory or shell location Explorer should start in.
*/
// ==/WindhawkModSettings==

#include <initguid.h>
#include <shobjidl.h>
#include <windhawk_utils.h>

WindhawkUtils::StringSetting g_location;

HRESULT (*COpenDefaultLocationCommand__GetNavTarget_orig)(void *, IShellItem **);
HRESULT COpenDefaultLocationCommand__GetNavTarget_hook(
    void        *pThis,
    IShellItem **ppsi
)
{
    return SHCreateItemFromParsingName(
        g_location.get(),
        nullptr,
        IID_IShellItem,
        (void **)ppsi
    );
}

void LoadSettings(void)
{
    g_location = WindhawkUtils::StringSetting::make(L"location");
}

const WindhawkUtils::SYMBOL_HOOK hook = {
    {
        L"private: long __cdecl COpenDefaultLocationCommand::_GetNavTarget(struct IShellItem * *)"
    },
    &COpenDefaultLocationCommand__GetNavTarget_orig,
    COpenDefaultLocationCommand__GetNavTarget_hook,
    false
};

BOOL Wh_ModInit(void)
{
    LoadSettings();

    HMODULE hShell32 = LoadLibraryW(L"shell32.dll");
    if (!hShell32)
    {
        Wh_Log(L"Failed to load shell32.dll");
        return FALSE;
    }

    if (!WindhawkUtils::HookSymbols(
        hShell32,
        &hook,
        1
    ))
    {
        Wh_Log(L"Failed to hook COpenDefaultLocationCommand::_GetNavTarget");
        return FALSE;
    }

    return TRUE;
}

void Wh_ModSettingsChanged(void)
{
    LoadSettings();
}