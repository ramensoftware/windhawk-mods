// ==WindhawkMod==
// @id              file-explorer-remove-suffixes
// @name            Remove File Explorer Suffixes
// @description     Windows appends a " - File Explorer" suffix for each folder on the taskbar, this mod gets rid of these redundant suffixes
// @version         1.0
// @author          m417z
// @github          https://github.com/m417z
// @twitter         https://twitter.com/m417z
// @homepage        https://m417z.com/
// @include         explorer.exe
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Remove File Explorer Suffixes

Windows appends a " - File Explorer" suffix for each folder on the taskbar, this
mod gets rid of these redundant suffixes.

![Before](https://i.imgur.com/ErUN0YU.png) \
_Before_

![After](https://i.imgur.com/tblTr3Q.png) \
_After_
*/
// ==/WindhawkModReadme==

using FindResourceExW_t = decltype(&FindResourceExW);
FindResourceExW_t FindResourceExW_Original;
HRSRC WINAPI FindResourceExW_Hook(HMODULE hModule,
                                  LPCWSTR lpType,
                                  LPCWSTR lpName,
                                  WORD wLanguage) {
    if (hModule && lpType == RT_STRING && lpName == MAKEINTRESOURCE(2195) &&
        hModule == GetModuleHandle(L"explorerframe.dll")) {
        Wh_Log(L">");
        SetLastError(ERROR_RESOURCE_NAME_NOT_FOUND);
        return nullptr;
    }

    return FindResourceExW_Original(hModule, lpType, lpName, wLanguage);
}

BOOL Wh_ModInit() {
    Wh_Log(L">");

    HMODULE kernelBaseModule = GetModuleHandle(L"kernelbase.dll");
    HMODULE kernel32Module = GetModuleHandle(L"kernel32.dll");

    auto setKernelFunctionHook = [kernelBaseModule, kernel32Module](
                                     PCSTR targetName, void* hookFunction,
                                     void** originalFunction) {
        void* targetFunction =
            (void*)GetProcAddress(kernelBaseModule, targetName);
        if (!targetFunction) {
            targetFunction = (void*)GetProcAddress(kernel32Module, targetName);
            if (!targetFunction) {
                return FALSE;
            }
        }

        return Wh_SetFunctionHook(targetFunction, hookFunction,
                                  originalFunction);
    };

    setKernelFunctionHook("FindResourceExW", (void*)FindResourceExW_Hook,
                          (void**)&FindResourceExW_Original);

    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L">");
}
