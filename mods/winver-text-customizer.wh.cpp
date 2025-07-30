// ==WindhawkMod==
// @id              winver-text-customizer
// @name            Winver Text Customizer
// @description     Customize the text sent to ShellAbout by winver.exe
// @version         1.0.0
// @author          aubymori
// @github          https://github.com/aubymori
// @include         winver.exe
// @compilerOptions -lshell32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Winver Text Customizer
This mod customizes the text sent to the `ShellAbout` API by `winver.exe`. This is useful
if you want to have a fake "evaluation copy" text, since that is only displayed by winver.exe
and not other instances of `ShellAbout`.

**Preview**:

![Preview](https://raw.githubusercontent.com/aubymori/images/refs/heads/main/winver-text-customizer.png)
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- app: Windows
  $name: App
  $description: "The szApp parameter, which will show up in the titlebar. If the text contains a #
    separator, the first part will replace the whole title text, and the second part will replace the
    first line of text."
- other_stuff: ""
  $name: Other stuff
  $description: The szOtherStuff parameter, which will show up in the middle of the dialog.
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

WindhawkUtils::StringSetting g_spszApp, g_spszOtherStuff;

using ShellAboutW_t = decltype(&ShellAboutW);
ShellAboutW_t ShellAboutW_orig;
INT WINAPI ShellAboutW_hook(
    HWND    hWnd,
    LPCWSTR szApp,
    LPCWSTR szOtherStuff,
    HICON   hIcon
)
{
    return ShellAboutW_orig(
        hWnd, g_spszApp.get(), g_spszOtherStuff.get(), hIcon
    );
}

BOOL Wh_ModInit(void)
{
    // We have no need for a Wh_ModSettingsChanged callback, the dialog is only displayed
    // once by winver.exe.
    g_spszApp        = WindhawkUtils::StringSetting::make(L"app");
    g_spszOtherStuff = WindhawkUtils::StringSetting::make(L"other_stuff");

    if (!Wh_SetFunctionHook(
        (void *)ShellAboutW,
        (void *)ShellAboutW_hook,
        (void **)&ShellAboutW_orig
    ))
    {
        Wh_Log(L"Failed to hook ShellAboutW");
        return FALSE;
    }

    return TRUE;
}