// ==WindhawkMod==
// @id              shadowplay-single-folder
// @name            ShadowPlay Save Folder Override
// @description     Forces Nvidia ShadowPlay to save all clips into a single folder
// @version         1.1
// @author          yuma-dev
// @github          https://github.com/yuma-dev
// @include         nvcontainer.exe
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# ShadowPlay Save Folder Override
This hooks into Nvidia ShadowPlay's file creation to redirect all saved clips
into a single folder (e.g., C:\Users\USER\Videos), instead of separate folders for each game or source.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- savePath: "C:\\"
  $name: Save path
  $description: Where all ShadowPlay clips should be saved.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <shlobj.h> // For SHGetKnownFolderPath
#include <algorithm>
#include <string>

wchar_t g_savePath[MAX_PATH] = L"C:\\";

void LoadSettings() {
    PCWSTR settingValue = Wh_GetStringSetting(L"savePath");
    if (settingValue && *settingValue) { 
        wcsncpy_s(g_savePath, MAX_PATH, settingValue, _TRUNCATE);
        Wh_Log(L"Successfully retrieved setting 'savePath': %s", g_savePath);
    } else {
        Wh_Log(L"Warning: Wh_GetStringSetting returned null or empty for 'savePath'. Using default or previous value: %s", g_savePath);
    }

    Wh_FreeStringSetting(settingValue);
}

bool endsWith(const std::wstring& str, const std::wstring& suffix) {
    if (suffix.size() > str.size()) return false;
    return std::equal(suffix.rbegin(), suffix.rend(), str.rbegin());
}

using CreateFileW_t = decltype(&CreateFileW);
CreateFileW_t CreateFileW_Original;

HANDLE WINAPI CreateFileW_Hook(
    LPCWSTR lpFileName,
    DWORD dwDesiredAccess,
    DWORD dwShareMode,
    LPSECURITY_ATTRIBUTES lpSecurityAttributes,
    DWORD dwCreationDisposition,
    DWORD dwFlagsAndAttributes,
    HANDLE hTemplateFile
) {
    if (lpFileName && endsWith(lpFileName, L".DVR.mp4") && wcsstr(lpFileName, L"\\Videos\\NVIDIA\\")) {
        Wh_Log(L"Intercepted clip save: %s", lpFileName);

        const wchar_t* pFileName = wcsrchr(lpFileName, L'\\');
        if (pFileName && *(pFileName + 1) != 0) {
            static wchar_t redirectedPath[MAX_PATH];
            swprintf(redirectedPath, MAX_PATH, L"%s%s", g_savePath, pFileName);
            Wh_Log(L"Redirecting to: %s", redirectedPath);
            lpFileName = redirectedPath;
        }
    }

    return CreateFileW_Original(
        lpFileName,
        dwDesiredAccess,
        dwShareMode,
        lpSecurityAttributes,
        dwCreationDisposition,
        dwFlagsAndAttributes,
        hTemplateFile
    );
}

BOOL Wh_ModInit() {
    Wh_Log(L"Init");
    LoadSettings();
    void* target = (void*)GetProcAddress(GetModuleHandle(L"kernel32.dll"), "CreateFileW");
    return Wh_SetFunctionHook(target, (void*)CreateFileW_Hook, (void**)&CreateFileW_Original);
}

void Wh_ModUninit() {
    Wh_Log(L"Uninit");
}

void Wh_ModSettingsChanged() {
    Wh_Log(L"Settings changed");
    LoadSettings();
}
