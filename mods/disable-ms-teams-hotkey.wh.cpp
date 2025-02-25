// ==WindhawkMod==
// @id              disable-ms-teams-hotkey
// @name            Disable Microsoft Teams hotkeys used by Visual Studio
// @description     Disables Ctrl+Shift+Space that does something useful in Visual Studio and not in Teams.
// @version         1.0.0
// @author          Yves Goergen
// @github          https://github.com/ygoe
// @homepage        https://ygoe.de/
// @include         ms-teams.exe
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Disable Microsoft Teams hotkeys used by Visual Studio

Microsoft Teams occupies the keyboard shortcut Ctrl+Shift+Space to do whatever nonsense with it while sharing the screen.
This hotkey is also assigned in Visual Studio by default to show the parameters info of the method under the cursor.
That information is often very useful while coding, but it's not available in a team coding session when sharing the screen.
So this mod prevents Teams from ever occupying that hotkey so that it remains available to Visual Studio like it's supposed to.
*/
// ==/WindhawkModReadme==

BOOL(WINAPI *pOriginalRegisterHotKey)(HWND hWnd, int id, UINT fsModifiers, UINT vk);

BOOL WINAPI RegisterHotKeyHook(HWND hWnd, int id, UINT fsModifiers, UINT vk)
{
    if (fsModifiers == (MOD_CONTROL | MOD_SHIFT) && vk == VK_SPACE)
    {
        Wh_Log(L"Preventing Teams to register Ctrl+Shift+Space hotkey");
        SetLastError(ERROR_HOTKEY_ALREADY_REGISTERED);
        return FALSE;
    }

    return pOriginalRegisterHotKey(hWnd, id, fsModifiers, vk);
}

BOOL Wh_ModInit()
{
    Wh_Log(L"Initializing");
    HMODULE hUser32 = GetModuleHandle(L"user32.dll");

    void* origFunc = (void*)GetProcAddress(hUser32, "RegisterHotKey");
    Wh_SetFunctionHook(origFunc, (void*)RegisterHotKeyHook, (void**)&pOriginalRegisterHotKey);
    if (pOriginalRegisterHotKey)
        Wh_Log(L"Replaced RegisterHotKey function");

    return TRUE;
}
