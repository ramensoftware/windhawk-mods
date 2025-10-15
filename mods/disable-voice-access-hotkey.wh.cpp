// ==WindhawkMod==
// @id              disable-voice-access-hotkey
// @name            Disable Voice Access Hotkey
// @description     Disables the Voice Access hotkey in Windows
// @version         1.0.0
// @author          clemmyn23
// @github          https://github.com/clemmyn23
// @include         explorer.exe
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Disable Voice Access Hotkey
This mod disables the Voice Access (Win+Ctrl+S) hotkey in Windows.
*/
// ==/WindhawkModReadme==

#include <windhawk_utils.h>

BOOL WINAPI (*pOriginalRegisterHotKey)(HWND hWnd, int id, UINT fsModifiers, UINT vk);
BOOL WINAPI RegisterHotKeyHook(HWND  hWnd, int id, UINT fsModifiers, UINT vk)
{
    if (fsModifiers == (MOD_CONTROL | MOD_WIN | MOD_NOREPEAT) && vk == 'S')
    {
        SetLastError(ERROR_HOTKEY_ALREADY_REGISTERED);
        return FALSE;
    }

    return pOriginalRegisterHotKey(hWnd, id, fsModifiers, vk);
}

BOOL Wh_ModInit() {
    const HMODULE hUser32 = GetModuleHandle(L"user32.dll");
    const auto origFunc = (decltype(&RegisterHotKey))GetProcAddress(hUser32, "RegisterHotKey");

    WindhawkUtils::SetFunctionHook(origFunc, RegisterHotKeyHook, &pOriginalRegisterHotKey);

    return TRUE;
}
