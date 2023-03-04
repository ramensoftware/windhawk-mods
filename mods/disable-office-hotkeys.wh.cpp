// ==WindhawkMod==
// @id              disable-office-hotkeys
// @name            Disable Office Hotkeys
// @description     Disables the office hotkeys (Ctrl+Shift+Alt+Win combinations) in Windows
// @version         1.0.0
// @author          ItsProfessional
// @github          https://github.com/ItsProfessional
// @include         explorer.exe
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Disable Office Hotkeys
This mod disables the office hotkeys (Ctrl+Shift+Alt+Win combinations) in Windows.
*/
// ==/WindhawkModReadme==

const UINT office_hotkeys[10] = { 0x57, 0x54, 0x59, 0x4F, 0x50, 0x44, 0x4C, 0x58, 0x4E, 0x20 }; // W, T, Y, O, P, D, L, X, N, SPACEBAR

BOOL(*pOriginalRegisterHotKey)(HWND hWnd, int id, UINT fsModifiers, UINT vk);
BOOL RegisterHotKeyHook(HWND hWnd, int id, UINT fsModifiers, UINT vk)
{
    if (fsModifiers == (MOD_ALT | MOD_CONTROL | MOD_SHIFT | MOD_WIN | MOD_NOREPEAT) && (
        !vk
        || vk == office_hotkeys[0]
        || vk == office_hotkeys[1]
        || vk == office_hotkeys[2]
        || vk == office_hotkeys[3]
        || vk == office_hotkeys[4]
        || vk == office_hotkeys[5]
        || vk == office_hotkeys[6]
        || vk == office_hotkeys[7]
        || vk == office_hotkeys[8]
        || vk == office_hotkeys[9]
    ))
    {
        SetLastError(ERROR_HOTKEY_ALREADY_REGISTERED);
        return FALSE;
    }

    return pOriginalRegisterHotKey(hWnd, id, fsModifiers, vk);
}


BOOL Wh_ModInit() {
    HMODULE hUser32 = GetModuleHandle(L"user32.dll");

    void* origFunc = (void*)GetProcAddress(hUser32, "RegisterHotKey");
    Wh_SetFunctionHook(origFunc, (void*)RegisterHotKeyHook, (void**)&pOriginalRegisterHotKey);

    return TRUE;
}
