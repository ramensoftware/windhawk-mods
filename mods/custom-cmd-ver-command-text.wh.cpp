// ==WindhawkMod==
// @id              custom-cmd-ver-command-text
// @name            Custom CMD "ver" Command Text
// @description     A simple mod that lets you customize the command prompt "ver" command text safely when typed at the prompt
// @version         1.0
// @author          RandomGuy5747
// @github          https://github.com/RandomGuy5747
// @include         cmd.exe
// @compilerOptions -std=c++17
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Custom CMD "ver" Command Text
A simple mod that lets you customize the command prompt "ver" command text.

You can customize it in the settings tab. This mod specifically targets the `ver` command typed directly at the active prompt, leaving scripts and internal operations unaffected.

Preview:

![preview1](https://githubusercontent.com)
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
#include <windhawk_utils.h>
#include <utility>
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
static bool g_promptPrinted = false;

// Hook für die interne PrintPrompt-Funktion von cmd.exe
void (*PrintPrompt_orig)();
void PrintPrompt_hook() {
    PrintPrompt_orig();
    g_promptPrinted = true;
}

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

    if (!result || !lpBuffer || !lpNumberOfCharsRead || *lpNumberOfCharsRead == 0) {
        return result;
    }

    DWORD consoleMode;
    if (!GetConsoleMode(hConsoleInput, &consoleMode)) {
        return result;
    }

    WCHAR* input = static_cast<WCHAR*>(lpBuffer);
    DWORD count = *lpNumberOfCharsRead;

    // 1. Nur agieren, wenn die Eingabe tatsächlich mit Enter (CRLF) abgeschlossen wurde
    if (count < 2 || input[count - 2] != L'\r' || input[count - 1] != L'\n') {
        return result;
    }

    // 2. Sicherstellen, dass die Eingabe direkt nach dem Prompt geschah (verhindert set /p und More?)
    if (!std::exchange(g_promptPrinted, false)) {
        return result;
    }

    DWORD end = count - 2;
    DWORD start = 0;

    while (start < end && (input[start] == L' ' || input[start] == L'\t')) {
        ++start;
    }

    while (end > start && (input[end - 1] == L' ' || input[end - 1] == L'\t')) {
        --end;
    }

    // Prüfen auf den Befehl "ver"
    if (end - start != 3 || _wcsnicmp(input + start, L"ver", 3) != 0) {
        return result;
    }

    PCWSTR setting = Wh_GetStringSetting(L"custom_text");
    std::wstring customText = setting ? setting : L"Microsoft Windows [Version 6.1.7601]";

    if (setting) {
        Wh_FreeStringSetting(setting);
    }

    // Text sicher ausgeben
    std::wstring output = L"\r\n" + customText + L"\r\n";
    DWORD written;
    WriteConsoleW(GetStdHandle(STD_OUTPUT_HANDLE), output.c_str(),
                  static_cast<DWORD>(output.size()), &written, nullptr);

    // No-Op rem Befehl unterschieben
    constexpr WCHAR kNoop[] = L"rem\r\n";
    if (nNumberOfCharsToRead >= (sizeof(kNoop) / sizeof(WCHAR)) - 1) {
        memcpy(lpBuffer, kNoop, sizeof(kNoop) - sizeof(WCHAR));
        *lpNumberOfCharsRead = (sizeof(kNoop) / sizeof(WCHAR)) - 1;
    }

    Wh_Log(L"Replaced ver using the custom text setting securely at the prompt.");

    return result;
}

BOOL Wh_ModInit() {
    HMODULE hCmd = GetModuleHandleW(nullptr);

    // Symbole von cmd.exe für PrintPrompt hooken
    const WindhawkUtils::SYMBOL_HOOK cmdExeHooks[] = {
        {
            {
    #ifdef _WIN64
                L"void __cdecl PrintPrompt(void)",
    #else
                L"void __stdcall PrintPrompt(void)",
    #endif
            },
            (void**)&PrintPrompt_orig,
            (void*)PrintPrompt_hook,
        },
    };

    if (!WindhawkUtils::HookSymbols(hCmd, cmdExeHooks, ARRAYSIZE(cmdExeHooks))) {
        Wh_Log(L"Failed to hook PrintPrompt symbols. Make sure symbols are available.");
        return FALSE;
    }

    // Standard ReadConsoleW Hook einrichten
    HMODULE kernelBase = GetModuleHandleW(L"KernelBase.dll");
    if (!kernelBase) {
        Wh_Log(L"KernelBase.dll not found.");
        return FALSE;
    }

    void* target = reinterpret_cast<void*>(GetProcAddress(kernelBase, "ReadConsoleW"));
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

    Wh_Log(L"Custom CMD ver mod initialized successfully with symbols.");
    return TRUE;
}
// ==WindhawkModEnd==
