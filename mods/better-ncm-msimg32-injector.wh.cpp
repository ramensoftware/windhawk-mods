// ==WindhawkMod==
// @id              better-ncm-msimg32-injector
// @name            BetterNCM direct runtime loader
// @description     Load a configured BetterNCM DLL directly into every cloudmusic.exe process.
// @version         0.2.0
// @author          Jurangren
// @github          https://github.com/Jurangren
// @include         cloudmusic.exe
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# BetterNCM direct runtime loader

This Windhawk mod is limited to `cloudmusic.exe` by the `@include` metadata.

It expands and loads the configured BetterNCM DLL directly with `LoadLibraryW`.
It does not copy the DLL, rename it to `msimg32.dll`, or attempt to replace an
already-loaded `msimg32.dll`. By the time `Wh_ModInit` runs, CloudMusic may
already have bound its `msimg32.dll` imports; this mod only performs a direct
runtime load of the configured DLL.

Suggested test setting:

- `sourceDllPath`: `C:\\Program Files (x86)\\NetEase\\CloudMusic\\BetterNCMII86.dll`

A successful log means Windows mapped the configured DLL and ran its ordinary
process-attach initialization. It does not prove that any undocumented
BetterNCM export was called.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- sourceDllPath: "C:\\Program Files (x86)\\NetEase\\CloudMusic\\BetterNCMII86.dll"
  $name: BetterNCM DLL path
  $description: Full path to the BetterNCM DLL to load. Environment variables such as %TEMP% are supported.
*/
// ==/WindhawkModSettings==

#include <windhawk_api.h>

#include <windows.h>

#include <string>

struct InjectSettings {
    std::wstring sourceDllPath;
};

static InjectSettings g_settings;
static HMODULE g_loadedModule = nullptr;

static std::wstring ReadStringSetting(const wchar_t* name) {
    PCWSTR value = Wh_GetStringSetting(name);
    std::wstring result = value ? value : L"";
    Wh_FreeStringSetting(value);
    return result;
}

static void LoadSettings() {
    g_settings.sourceDllPath = ReadStringSetting(L"sourceDllPath");
}

static std::wstring ExpandEnvironmentStringsToString(const std::wstring& input) {
    if (input.empty()) {
        return {};
    }

    DWORD requiredChars = ExpandEnvironmentStringsW(input.c_str(), nullptr, 0);
    if (requiredChars == 0) {
        Wh_Log(L"ExpandEnvironmentStringsW failed for '%s', error=%lu", input.c_str(), GetLastError());
        return input;
    }

    std::wstring output(requiredChars, L'\0');
    DWORD writtenChars = ExpandEnvironmentStringsW(input.c_str(), output.data(), requiredChars);
    if (writtenChars == 0 || writtenChars > requiredChars) {
        Wh_Log(L"ExpandEnvironmentStringsW write failed for '%s', error=%lu", input.c_str(), GetLastError());
        return input;
    }

    if (!output.empty() && output.back() == L'\0') {
        output.pop_back();
    }

    return output;
}

static PCWSTR FindFileName(PCWSTR path) {
    if (!path) {
        return L"";
    }

    PCWSTR fileName = path;
    for (PCWSTR cursor = path; *cursor; cursor++) {
        if (*cursor == L'\\' || *cursor == L'/') {
            fileName = cursor + 1;
        }
    }

    return fileName;
}

static bool IsCloudMusicProcess() {
    wchar_t exePath[MAX_PATH] = {};
    DWORD chars = GetModuleFileNameW(nullptr, exePath, ARRAYSIZE(exePath));
    if (chars == 0 || chars >= ARRAYSIZE(exePath)) {
        Wh_Log(L"GetModuleFileNameW for current process failed, error=%lu", GetLastError());
        return false;
    }

    return _wcsicmp(FindFileName(exePath), L"cloudmusic.exe") == 0;
}

static bool ValidateSourceDllPath(const std::wstring& sourcePath) {
    DWORD attributes = GetFileAttributesW(sourcePath.c_str());
    if (attributes == INVALID_FILE_ATTRIBUTES) {
        Wh_Log(L"Configured BetterNCM DLL doesn't exist or isn't accessible: '%s', error=%lu", sourcePath.c_str(), GetLastError());
        return false;
    }

    if (attributes & FILE_ATTRIBUTE_DIRECTORY) {
        Wh_Log(L"Configured BetterNCM DLL path is a directory, not a DLL: '%s'", sourcePath.c_str());
        return false;
    }

    return true;
}

static void LogModulePath(PCWSTR description, HMODULE module) {
    wchar_t modulePath[MAX_PATH] = {};
    DWORD chars = GetModuleFileNameW(module, modulePath, ARRAYSIZE(modulePath));
    if (chars == 0 || chars >= ARRAYSIZE(modulePath)) {
        Wh_Log(L"%s is loaded at 0x%p; GetModuleFileNameW failed, error=%lu", description, module, GetLastError());
        return;
    }

    Wh_Log(L"%s is loaded at 0x%p from '%s'", description, module, modulePath);
}

static void LogExistingMsimg32Module() {
    HMODULE module = GetModuleHandleW(L"msimg32.dll");
    if (module) {
        LogModulePath(L"Existing msimg32.dll (informational; direct BetterNCM load will continue)", module);
    } else {
        Wh_Log(L"msimg32.dll is not loaded yet (informational; direct BetterNCM load will continue)");
    }
}

static void LoadBetterNcmDirectly() {
    if (!IsCloudMusicProcess()) {
        Wh_Log(L"Current process is not cloudmusic.exe; skipping");
        return;
    }

    if (g_loadedModule) {
        LogModulePath(L"Configured BetterNCM DLL was already loaded by this mod", g_loadedModule);
        return;
    }

    std::wstring sourcePath = ExpandEnvironmentStringsToString(g_settings.sourceDllPath);
    if (sourcePath.empty()) {
        Wh_Log(L"sourceDllPath is empty; skipping");
        return;
    }

    if (!ValidateSourceDllPath(sourcePath)) {
        return;
    }

    LogExistingMsimg32Module();
    Wh_Log(L"Loading configured BetterNCM DLL directly: '%s'", sourcePath.c_str());

    g_loadedModule = LoadLibraryW(sourcePath.c_str());
    if (!g_loadedModule) {
        Wh_Log(L"LoadLibraryW('%s') failed, error=%lu", sourcePath.c_str(), GetLastError());
        return;
    }

    LogModulePath(L"Configured BetterNCM DLL loaded directly", g_loadedModule);
}

BOOL Wh_ModInit() {
    Wh_Log(L"Init direct BetterNCM runtime loader");
    LoadSettings();
    LoadBetterNcmDirectly();
    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L"AfterInit");
}

void Wh_ModBeforeUninit() {
    Wh_Log(L"BeforeUninit; leaving the direct-loaded BetterNCM DLL mapped for process lifetime");
}

void Wh_ModSettingsChanged() {
    Wh_Log(L"SettingsChanged; the new sourceDllPath takes effect in newly started cloudmusic.exe processes");
    LoadSettings();
}
