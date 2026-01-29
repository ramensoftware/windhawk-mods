// ==WindhawkMod==
// @id              global-suffix-remover
// @name            Global Suffix Remover
// @description     Removes the suffix (everything after the last " - ") from window titles globally.
// @version         1.0
// @author          thewerthon
// @github          https://github.com/thewerthon
// @include         *
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Global Suffix Remover
Removes the suffix from window titles for all applications. \
It targets the last " - " separator and strips everything following it.

Example: \
"GitHub - Google Chrome" -> "GitHub" \
"Olympus - Visual Studio Code" -> "Olympus" \
"Downloads - File Explorer" -> "Downloads"
*/
// ==/WindhawkModReadme==

#include <string>

// --- Logic 1: Generic Apps (SetWindowTextW) ---
using SetWindowTextW_t = decltype(&SetWindowTextW);
SetWindowTextW_t SetWindowTextW_Original;

BOOL WINAPI SetWindowTextW_Hook(HWND hWnd, LPCWSTR lpString) {
    if (lpString) {
        std::wstring title(lpString);
        size_t lastSeparator = title.rfind(L" - ");
        if (lastSeparator != std::wstring::npos) {
            title = title.substr(0, lastSeparator);
            return SetWindowTextW_Original(hWnd, title.c_str());
        }
    }
    return SetWindowTextW_Original(hWnd, lpString);
}

// --- Logic 2: File Explorer (FindResourceExW) ---
using FindResourceExW_t = decltype(&FindResourceExW);
FindResourceExW_t FindResourceExW_Original;

HRSRC WINAPI FindResourceExW_Hook(HMODULE hModule, LPCWSTR lpType, LPCWSTR lpName, WORD wLanguage) {
    if (hModule && lpType == RT_STRING && lpName == MAKEINTRESOURCE(2195) &&
        hModule == GetModuleHandle(L"explorerframe.dll")) {
        SetLastError(ERROR_RESOURCE_NAME_NOT_FOUND);
        return nullptr;
    }
    return FindResourceExW_Original(hModule, lpType, lpName, wLanguage);
}

BOOL Wh_ModInit() {
    Wh_Log(L">");

    // Hook 1: SetWindowTextW (User32)
    HMODULE user32Module = GetModuleHandle(L"user32.dll");
    if (user32Module) {
        Wh_SetFunctionHook((void*)GetProcAddress(user32Module, "SetWindowTextW"),
                           (void*)SetWindowTextW_Hook,
                           (void**)&SetWindowTextW_Original);
    }

    // Hook 2: FindResourceExW (Kernel32/KernelBase)
    // Only necessary if explorerframe.dll is loaded or likely to be loaded, 
    // but safe to hook globally as the hook function checks the module handle.
    HMODULE kernelBaseModule = GetModuleHandle(L"kernelbase.dll");
    HMODULE kernel32Module = GetModuleHandle(L"kernel32.dll");

    void* targetFunction = (void*)GetProcAddress(kernelBaseModule, "FindResourceExW");
    if (!targetFunction) {
        targetFunction = (void*)GetProcAddress(kernel32Module, "FindResourceExW");
    }

    if (targetFunction) {
        Wh_SetFunctionHook(targetFunction, (void*)FindResourceExW_Hook, (void**)&FindResourceExW_Original);
    }

    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L">");
}