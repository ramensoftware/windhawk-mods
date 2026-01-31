// ==WindhawkMod==
// @id              block-win-v
// @name            Block Win+V / 阻止 Win+V
// @description     Blocks the Win+V keyboard shortcut (Clipboard History) / 阻止 Windows 的 Win+V 键盘快捷键（剪贴板历史记录）
// @version         1.0.0
// @author          Fenig
// @github          https://github.com/JiangFengning
// @include         explorer.exe
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Block Win+V / 阻止 Win+V

This mod blocks the Win+V keyboard shortcut (Clipboard History) in Windows.

此模块阻止 Windows 中的 Win+V 键盘快捷键（剪贴板历史记录）。
*/
// ==/WindhawkModReadme==

BOOL(*pOriginalRegisterHotKey)(HWND hWnd, int id, UINT fsModifiers, UINT vk);
BOOL RegisterHotKeyHook(HWND hWnd, int id, UINT fsModifiers, UINT vk)
{
    if (fsModifiers == (MOD_WIN | MOD_NOREPEAT) && vk == 'V')
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
