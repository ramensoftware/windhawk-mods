// ==WindhawkMod==
// @id              extension-change-no-warning
// @name            Turn off change file extension warning
// @description     When a file is renamed and its extension is changed, a confirmation warning appears, this mod turns it off
// @version         1.0
// @author          m417z
// @github          https://github.com/m417z
// @twitter         https://twitter.com/m417z
// @homepage        https://m417z.com/
// @include         explorer.exe
// ==/WindhawkMod==

// Source code is published under The GNU General Public License v3.0.
//
// For bug reports and feature requests, please open an issue here:
// https://github.com/ramensoftware/windhawk-mods/issues
//
// For pull requests, development takes place here:
// https://github.com/m417z/my-windhawk-mods

// ==WindhawkModReadme==
/*
# Turn off change file extension warning

When a file is renamed and its extension is changed, a confirmation warning
appears, this mod turns it off.

![Screenshot](https://i.imgur.com/ZV47UCC.png)
*/
// ==/WindhawkModReadme==

// Forwarding arguments of varadic functions isn't supported in C/C++:
// https://stackoverflow.com/q/3530771
//
// Therefore, instead of using `decltype(&ShellMessageBoxW)`, only declare the
// first arguments (must be at least 4 as they're passed in registers in x86-64)
// and use the musttail attribute to ensure that a tail call is used, which in
// turn ensures that the rest of the arguments are passed along in the stack.
using ShellMessageBoxW_t = int(__cdecl*)(HINSTANCE hAppInst,
                                         HWND hWnd,
                                         LPCWSTR lpcText,
                                         LPCWSTR lpcTitle,
                                         UINT fuStyle);
ShellMessageBoxW_t ShellMessageBoxW_Original;
int __cdecl ShellMessageBoxW_Hook(HINSTANCE hAppInst,
                                  HWND hWnd,
                                  LPCWSTR lpcText,
                                  LPCWSTR lpcTitle,
                                  UINT fuStyle) {
    Wh_Log(L">");

    if (hAppInst && lpcText == MAKEINTRESOURCE(4112) &&
        lpcTitle == MAKEINTRESOURCE(4148) &&
        fuStyle == (MB_ICONEXCLAMATION | MB_YESNO) &&
        hAppInst == GetModuleHandle(L"shell32.dll")) {
        return IDYES;
    }

    [[clang::musttail]] return ShellMessageBoxW_Original(
        hAppInst, hWnd, lpcText, lpcTitle, fuStyle);
}

BOOL Wh_ModInit() {
    Wh_Log(L">");

    Wh_SetFunctionHook((void*)ShellMessageBoxW, (void*)ShellMessageBoxW_Hook,
                       (void**)&ShellMessageBoxW_Original);

    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L">");
}
