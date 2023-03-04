// ==WindhawkMod==
// @id              disable-feedback-hub-hotkey
// @name            Disable Feedback Hub Hotkey
// @description     Disables the feedback hub (Win+F) hotkey in Windows
// @version         1.0.0
// @author          ItsProfessional
// @github          https://github.com/ItsProfessional
// @include         explorer.exe
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Disable Feedback Hub Hotkey
This mod disables the feedback hub (Win+F) hotkey in Windows.
*/
// ==/WindhawkModReadme==


BOOL(*pOriginalRegisterHotKey)(HWND hWnd, int id, UINT fsModifiers, UINT vk);
BOOL RegisterHotKeyHook(HWND hWnd, int id, UINT fsModifiers, UINT vk)
{
    if (fsModifiers == (MOD_WIN | MOD_NOREPEAT) && vk == 'F')
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
