// ==WindhawkMod==
// @id              custom-cmd-ver-command-text
// @name            Custom CMD "ver" Command Text
// @description     A simple mod that lets you customize the command prompt "ver" command text
// @version         1.0.0
// @author          RandomGuy5747
// @github          https://github.com/RandomGuy5747
// @include         cmd.exe
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Custom CMD "ver" Command Text
A simple mod that lets you customize the command prompt "ver" command text.

You can customize it in the settings tab.

Preview:

![preview1](https://raw.githubusercontent.com/RandomGuy5747/windhawk-preview/refs/heads/main/preview.png)
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- custom_text: "Microsoft Windows [Version 6.1.7601]"
  $name: The text to show
  $description: Here you can customize the text to show.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <windhawk_api.h>
#include <cstring>
#include <string>

using ReadConsoleW_t = BOOL (WINAPI*)(
    HANDLE,
    LPVOID,
    DWORD,
    LPDWORD,
    PCONSOLE_READCONSOLE_CONTROL
);

static ReadConsoleW_t ReadConsoleW_Original = nullptr;

static BOOL WINAPI ReadConsoleW_Hook(
    HANDLE hConsoleInput,
    LPVOID lpBuffer,
    DWORD nNumberOfCharsToRead,
    LPDWORD lpNumberOfCharsRead,
    PCONSOLE_READCONSOLE_CONTROL pInputControl
) {
    BOOL result = ReadConsoleW_Original(
        hConsoleInput,
        lpBuffer,
        nNumberOfCharsToRead,
        lpNumberOfCharsRead,
        pInputControl
    );

    if (!result || !lpBuffer || !lpNumberOfCharsRead ||
        *lpNumberOfCharsRead == 0) {
        return result;
    }

    DWORD consoleMode;
    if (!GetConsoleMode(hConsoleInput, &consoleMode)) {
        return result;
    }

    WCHAR* input = static_cast<WCHAR*>(lpBuffer);
    DWORD count = *lpNumberOfCharsRead;
    DWORD end = count;

    while (end > 0 &&
           (input[end - 1] == L'\r' || input[end - 1] == L'\n')) {
        --end;
    }

    DWORD start = 0;

    while (start < end &&
           (input[start] == L' ' || input[start] == L'\t')) {
        ++start;
    }

    while (end > start &&
           (input[end - 1] == L' ' || input[end - 1] == L'\t')) {
        --end;
    }

    if (end - start != 3 ||
        _wcsnicmp(input + start, L"ver", 3) != 0) {
        return result;
    }

    PCWSTR setting = Wh_GetStringSetting(L"custom_text");

    std::wstring customText =
        setting ? setting : L"Microsoft Windows [Version 6.1.7601]";

    if (setting) {
        Wh_FreeStringSetting(setting);
    }

    std::wstring replacement = L"echo.&echo " + customText + L"\r\n";

    if (replacement.size() > nNumberOfCharsToRead) {
        Wh_Log(L"Custom text is too long for the console input buffer.");
        return result;
    }

    memcpy(
        lpBuffer,
        replacement.c_str(),
        replacement.size() * sizeof(WCHAR)
    );

    *lpNumberOfCharsRead = static_cast<DWORD>(replacement.size());

    Wh_Log(L"Replaced ver using the custom text setting.");

    return result;
}

BOOL Wh_ModInit() {
    HMODULE kernelBase = GetModuleHandleW(L"KernelBase.dll");

    if (!kernelBase) {
        Wh_Log(L"KernelBase.dll not found.");
        return FALSE;
    }

    void* target = reinterpret_cast<void*>(
        GetProcAddress(kernelBase, "ReadConsoleW")
    );

    if (!target) {
        Wh_Log(L"ReadConsoleW not found.");
        return FALSE;
    }

    if (!Wh_SetFunctionHook(
            target,
            reinterpret_cast<void*>(ReadConsoleW_Hook),
            reinterpret_cast<void**>(&ReadConsoleW_Original))) {
        Wh_Log(L"Failed to hook ReadConsoleW.");
        return FALSE;
    }

    Wh_Log(L"Custom CMD ver mod initialized.");
    return TRUE;
}
// ==WindhawkModEnd==

