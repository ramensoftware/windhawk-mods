// ==WindhawkMod==
// @id              no-icon-messagebox-sound
// @name            No icon MessageBox Sound Restore
// @description     This mod restores the sound effect that plays when a MessageBox without an icon pops up.
// @version         1.0.1
// @author          Jevil7452
// @github          https://github.com/Jevil7452
// @include         *
// ==/WindhawkMod==
// ==WindhawkModReadme==
/*
This mod restores the sound that plays whenever a MessageBox that does not have an icon set pops up.

This replicates behavior from Windows Vista and below.

NOTE: If you use "Message Box Fix" mod with Vista and below style, this is not needed as the mod will cause the sound to play in this scenario.

This mod is for users of older Windows versions (like Windows 7), where the Message Box Fix mod does not work, or for users who just want to restore the sound without changing anything else.
*/
// ==/WindhawkModReadme==

#include <windows.h>

typedef int (WINAPI *MessageBoxW_t)(HWND, LPCWSTR, LPCWSTR, UINT);
typedef int (WINAPI *MessageBoxA_t)(HWND, LPCSTR, LPCSTR, UINT);

static MessageBoxW_t pMessageBoxW;
static MessageBoxA_t pMessageBoxA;

static BOOL HasNoIcon(UINT uType)
{
    UINT iconBits = uType & (MB_ICONHAND | MB_ICONQUESTION | MB_ICONEXCLAMATION | MB_ICONASTERISK);
    return iconBits == 0;
}

int WINAPI MessageBoxW_Hook(HWND hWnd, LPCWSTR lpText, LPCWSTR lpCaption, UINT uType)
{
    if (HasNoIcon(uType))
        MessageBeep(0);
    return pMessageBoxW(hWnd, lpText, lpCaption, uType);
}

int WINAPI MessageBoxA_Hook(HWND hWnd, LPCSTR lpText, LPCSTR lpCaption, UINT uType)
{
    if (HasNoIcon(uType))
        MessageBeep(0);
    return pMessageBoxA(hWnd, lpText, lpCaption, uType);
}

BOOL Wh_ModInit()
{
    Wh_SetFunctionHook((void*)MessageBoxW, (void*)MessageBoxW_Hook, (void**)&pMessageBoxW);
    Wh_SetFunctionHook((void*)MessageBoxA, (void*)MessageBoxA_Hook, (void**)&pMessageBoxA);
    return TRUE;

}
