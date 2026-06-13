// ==WindhawkMod==
// @id              winver-custom-text
// @name            Winver Custom Text
// @description     Allows you to set the text displayed in winver.exe.
// @version         1.0.1
// @author          Thomas Shrader
// @github          https://github.com/ilikecoding-197
// @include         winver.exe
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
Allows you to set the text displayed in winver.exe.

If you dont know what winver is, its a file built in to Windows that simply displays info about your system:
![winver.exe](https://i.imgur.com/6XvNlWj.png)

This mod allows you to customize that.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- title: About Windows
  $name: Title
  $description: What displays in the title.
- firstline: Microsoft Windows
  $name: First line
  $description: What displays on the first line, after the Windows logo.
- otherstuff: ""
  $name: Other stuff
  $description: Info appearing after copy right and stuff.
*/
// ==/WindhawkModSettings==

// The source code of the mod starts here. This sample was inspired by the great
// article of Kyle Halladay, X64 Function Hooking by Example:
// http://kylehalladay.com/blog/2020/11/13/Hooking-By-Example.html
// If you're new to terms such as code injection and function hooking, the
// article is great to get started.

#include <shellapi.h>
#include <stdio.h>

struct {
    PCWSTR title;
    PCWSTR firstLine;
    PCWSTR otherstuff;
} Settings;

struct {
    WCHAR szApp[1024];
} ShellAboutArgs;

using ShellAboutW_t = decltype(&ShellAboutW);
ShellAboutW_t ShellAboutW_Orginal;
INT WINAPI ShellAboutW_Hook(
    HWND    hWnd,
    LPCWSTR szApp,
    LPCWSTR szOtherStuff,
    HICON   hIcon
) {
    Wh_Log(L"ShellAboutW(NULL, L\"%s\", L\"%s\", NULL);", szApp, szOtherStuff);
    return ShellAboutW_Orginal(hWnd, ShellAboutArgs.szApp, Settings.otherstuff, hIcon);
}

void LoadSettings() {
    Wh_Log(L"LoadSettings();");
    Settings.title = Wh_GetStringSetting(L"title");
    Settings.firstLine = Wh_GetStringSetting(L"firstline");
    Settings.otherstuff = Wh_GetStringSetting(L"otherstuff");

    swprintf((wchar_t *)ShellAboutArgs.szApp, L"%s#%s", Settings.title, Settings.firstLine);

    Wh_Log(L"title: %s", Settings.title);
    Wh_Log(L"firstline: %s\n", Settings.firstLine);
    Wh_Log(L"otherstuff: %s\n", Settings.otherstuff);
    Wh_Log(L"szApp: %s", ShellAboutArgs.szApp);
}

void FreeSettings() {
    Wh_Log(L"FreeSettings();");

    Wh_FreeStringSetting(Settings.title);
    Wh_FreeStringSetting(Settings.firstLine);
    Wh_FreeStringSetting(Settings.otherstuff);
}

BOOL Wh_ModInit() {
    Wh_Log(L"Wh_ModInit();");

    LoadSettings();

    HMODULE shell32Module = LoadLibrary(L"shell32.dll");
    ShellAboutW_t ShellAboutW =
        (ShellAboutW_t)GetProcAddress(shell32Module,
                                          "ShellAboutW");

    Wh_SetFunctionHook((void*)ShellAboutW,
                       (void*)ShellAboutW_Hook,
                       (void**)&ShellAboutW_Orginal);

    return TRUE;
}

// The mod is being unloaded, free all allocated resources.
void Wh_ModUninit() {
    Wh_Log(L"Wh_ModUninit();");

    FreeSettings();
}

// The mod setting were changed.
void Wh_ModSettingsChanged() {
    Wh_Log(L"Wh_ModSettingsChanged();");

    FreeSettings(); LoadSettings();
}
