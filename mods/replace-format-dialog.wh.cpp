// ==WindhawkMod==
// @id              replace-format-dialog
// @name            Replace Shell Format Dialog
// @description     Replaces the Shell Disk Format Dialog with a custom app of your choosing.
// @version         1.0
// @author          FireBlade
// @github          https://github.com/FireBlade211
// @include         *
// @compilerOptions -lshell32
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Replace Shell Format Dialog
A simple mod that allows you to replace the File Explorer disk format dialog with a custom one.

## Note
This will replace the format dialog in all apps that use the Shell Format dialog, not just File Explorer. You can manually
set the exclusion list in this mod's **Advanced** tab.

## More details about the Format Arguments option
If you enable the **Format Arguments** option in the mod settings, the mod will automatically replace the first instance of %s in your
argument string with the drive letter of the drive the app is attempting to format. This drive letter does not include the `:` suffix
(e.g, it will say `C` instead of `C:`).

*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
# Here you can define settings, in YAML format, that the mod users will be able
# to configure. Metadata values such as $name and $description are optional.
# Check out the documentation for more information:
# https://github.com/ramensoftware/windhawk/wiki/Creating-a-new-mod#settings
- path: ""
  $name: Path
  $description: The path to the app to launch instead of the format dialog.
- args: ""
  $name: Arguments
  $description: Arguments to launch the app with.
- formatArgs: false
  $name: Format Arguments
  $description: If enabled, automatically replace the first instance of %s in the arguments with the drive letter. More info in the README.
*/
// ==/WindhawkModSettings==

#include <shlobj.h>
#include <wchar.h>
#include <string>
#include <stddef.h>

struct {
    PCWSTR path;
    PCWSTR args;
    bool formatArgs;
} settings;


using SHFormatDrive_t = decltype(&SHFormatDrive);
SHFormatDrive_t SHFormatDrive_Original;

void LoadSettings() {
    settings.path = Wh_GetStringSetting(L"path");
    settings.args = Wh_GetStringSetting(L"args");
    settings.formatArgs = Wh_GetIntSetting(L"formatArgs");
}

void FreeSettings() {
    if (settings.path)
    {
        Wh_FreeStringSetting(settings.path);
        settings.path = NULL;
    }

    if (settings.args)
    {
        Wh_FreeStringSetting(settings.args);
        settings.args = NULL;
    }
}

DWORD WINAPI SHFormatDrive_Hook(HWND hwnd, UINT drive, UINT fmtID, UINT options)
{
    if (*settings.path == '\0' || drive > 25)
        return SHFormatDrive_Original(hwnd, drive, fmtID, options);

    std::wstring args = settings.args;

    if (settings.formatArgs) {
        size_t pos = args.find(L"%s");
        if (pos != std::wstring::npos) {
            WCHAR letter[2] = {(WCHAR)(L'A' + drive), L'\0'};
            args.replace(pos, 2, letter);
        }
    }

    if ((INT_PTR)ShellExecuteW(hwnd, NULL, settings.path, args.c_str(), NULL, SW_SHOW) <= 32)
        return SHFormatDrive_Original(hwnd, drive, fmtID, options);

    return SHFMT_CANCEL;
}

// The mod is being initialized, load settings, hook functions, and do other
// initialization stuff if required.
BOOL Wh_ModInit() {
    Wh_Log(L"Init");

    LoadSettings();

    HMODULE hShell32 = LoadLibraryExW(L"shell32.dll", NULL, LOAD_LIBRARY_SEARCH_SYSTEM32);
    SHFormatDrive_t pfnSHFormatDrive =
        (SHFormatDrive_t)GetProcAddress(hShell32, "SHFormatDrive");

    Wh_SetFunctionHook((void*)pfnSHFormatDrive,
                       (void*)SHFormatDrive_Hook,
                       (void**)&SHFormatDrive_Original);

    return TRUE;
}

// The mod is being unloaded, free all allocated resources.
void Wh_ModUninit() {
    Wh_Log(L"Uninit");
    FreeSettings();
}

// The mod setting were changed, reload them.
void Wh_ModSettingsChanged() {
    Wh_Log(L"SettingsChanged");

    FreeSettings();
    LoadSettings();
}
