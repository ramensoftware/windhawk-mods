// ==WindhawkMod==
// @id              middle-click-minimize
// @name            Middle Click Minimize
// @description     Minimizes the window when middle-clicking (scroll wheel) on the title bar.
// @version         1.1.2
// @author          Ar4ujo_009
// @github          https://github.com/Ar4ujo009
// @include         *
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Middle Click Minimize
This mod allows you to minimize windows by middle-clicking (using the scroll wheel) directly on the title bar.

## Features
* **Universal Support:** Works seamlessly across most native Windows applications and modern frameworks that report the HTCAPTION hit test.
* **Configurable Blacklist:** Specify executable names (e.g., `steam.exe`, `vivaldi.exe`) in the mod settings to prevent injection in incompatible or custom-drawn applications.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- blacklist: "windhawk.exe"
  $name: "Blacklisted Applications"
  $description: "Comma-separated list of executable names to ignore. (Example: steam.exe, vivaldi.exe)"
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <windhawk_api.h>
#include <string>

bool IsProcessBlacklisted() {
    PCWSTR blacklistStr = Wh_GetStringSetting(L"blacklist");
    if (!blacklistStr) return false;

    std::wstring list = blacklistStr;
    Wh_FreeStringSetting(blacklistStr);

    WCHAR exePath[MAX_PATH];
    GetModuleFileNameW(NULL, exePath, MAX_PATH);
    WCHAR* exeName = wcsrchr(exePath, L'\\');
    exeName = exeName ? (exeName + 1) : exePath;

    size_t pos = 0;
    std::wstring token;
    std::wstring delimiter = L",";
    std::wstring s = list + L",";

    while ((pos = s.find(delimiter)) != std::wstring::npos) {
        token = s.substr(0, pos);
        token.erase(0, token.find_first_not_of(L" \t\r\n"));
        token.erase(token.find_last_not_of(L" \t\r\n") + 1);
        if (!token.empty() && _wcsicmp(exeName, token.c_str()) == 0) {
            return true;
        }
        s.erase(0, pos + delimiter.length());
    }
    return false;
}

typedef LRESULT(WINAPI *DispatchMessageW_t)(const MSG *lpMsg);
DispatchMessageW_t DispatchMessageW_Original;

LRESULT WINAPI DispatchMessageW_Hook(const MSG *lpMsg) {
    if (lpMsg && lpMsg->message == WM_NCMBUTTONDOWN && lpMsg->wParam == HTCAPTION) {
        HWND hRoot = GetAncestor(lpMsg->hwnd, GA_ROOT);
        PostMessage(hRoot, WM_SYSCOMMAND, SC_MINIMIZE, 0);
        return 0; 
    }
    return DispatchMessageW_Original(lpMsg);
}

typedef LRESULT(WINAPI *DispatchMessageA_t)(const MSG *lpMsg);
DispatchMessageA_t DispatchMessageA_Original;

LRESULT WINAPI DispatchMessageA_Hook(const MSG *lpMsg) {
    if (lpMsg && lpMsg->message == WM_NCMBUTTONDOWN && lpMsg->wParam == HTCAPTION) {
        HWND hRoot = GetAncestor(lpMsg->hwnd, GA_ROOT);
        PostMessage(hRoot, WM_SYSCOMMAND, SC_MINIMIZE, 0);
        return 0;
    }
    return DispatchMessageA_Original(lpMsg);
}

BOOL Wh_ModInit() {
    if (IsProcessBlacklisted()) {
        Wh_Log(L"Process is blacklisted. Middle Click Minimize ignored.");
        return FALSE;
    }

    HMODULE hUser32 = GetModuleHandle(L"user32.dll");
    if (!hUser32) return FALSE;

    FARPROC pDispatchMessageW = GetProcAddress(hUser32, "DispatchMessageW");
    if (pDispatchMessageW) {
        Wh_SetFunctionHook((void*)pDispatchMessageW, (void*)DispatchMessageW_Hook, (void**)&DispatchMessageW_Original);
    }

    FARPROC pDispatchMessageA = GetProcAddress(hUser32, "DispatchMessageA");
    if (pDispatchMessageA) {
        Wh_SetFunctionHook((void*)pDispatchMessageA, (void*)DispatchMessageA_Hook, (void**)&DispatchMessageA_Original);
    }

    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L"Middle Click Minimize unloaded.");
}
