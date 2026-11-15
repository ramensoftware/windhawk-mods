// ==WindhawkMod==
// @id              touhou-kbd-binds
// @name            Touhou Keyboard Binds
// @description     Allows you to change keyboard binds in Touhou
// @version         1.0.0
// @author          aubymori
// @github          https://github.com/aubymori
// @include         東方紅魔郷.exe
// @include         “Œ•ûg–‚‹½.exe
// @include         th07.exe
// @include         th08.exe
// @include         th09.exe
// @include         th10.exe
// @include         th11.exe
// @include         th12.exe
// @include         th13.exe
// @include         th14.exe
// @include         th15.exe
// @include         th16.exe
// @include         th17.exe
// @architecture    x86
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Touhou Keyboard Binds
From Touhou 6 to Touhou 17, keyboard bindings were unable to be changed within
the game. This mod lets you set your own bindings.

## ⚠️ Important usage note ⚠️
Windhawk may not inject into the game due to it being in a common game directory.
If this occurs, add the game's executable to the process inclusion list in the advanced 
settings.

![Advanced settings screenshot](https://i.imgur.com/LRhREtJ.png)

## Key format
The key options can either be a single character, or one of the following keywords:

None, Space, Up, Down, Left, Right, LCTRL, RCTRL, LControl, RControl, LAlt, RAlt, LShift,
RShift, Tab, CapsLock, F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12, Numpad0, Numpad1,
Numpad2, Numpad3, Numpad4, Numpad5, Numpad6, Numpad7, Numpad8, Numpad9
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- up: "Up"
  $name: Up
- down: "Down"
  $name: Down
- left: "Left"
  $name: Left
- right: "Right"
  $name: Right
- focus: "LShift"
  $name: Focus/Slow down replay
- speed: "LCtrl"
  $name: Speed up dialogue/replay
- shoot: "Z"
  $name: Shoot/Confirm
- bomb: "X"
  $name: Bomb/Back
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>
#include <dinput.h>

BYTE g_vkUp    = VK_UP;
BYTE g_vkDown  = VK_DOWN;
BYTE g_vkLeft  = VK_LEFT;
BYTE g_vkRight = VK_RIGHT;
BYTE g_vkFocus = VK_LSHIFT;
BYTE g_vkSpeed = VK_LCONTROL;
BYTE g_vkShoot = 'Z';
BYTE g_vkBomb  = 'X';

BYTE g_dikUp    = DIK_UP;
BYTE g_dikDown  = DIK_DOWN;
BYTE g_dikLeft  = DIK_LEFT;
BYTE g_dikRight = DIK_RIGHT;
BYTE g_dikFocus = DIK_LSHIFT;
BYTE g_dikSpeed = DIK_LCONTROL;
BYTE g_dikShoot = DIK_Z;
BYTE g_dikBomb  = DIK_X;

#define SET_KEY_STATE(i, v) if (v) lpKeyState[i] = lpKeyState[v]

BOOL (WINAPI *GetKeyboardState_orig)(PBYTE);
BOOL WINAPI GetKeyboardState_hook(PBYTE lpKeyState)
{
    if (!GetKeyboardState_orig(lpKeyState))
        return FALSE;

    SET_KEY_STATE(VK_UP,      g_vkUp);
    SET_KEY_STATE(VK_DOWN,    g_vkDown);
    SET_KEY_STATE(VK_LEFT,    g_vkLeft);
    SET_KEY_STATE(VK_RIGHT,   g_vkRight);
    SET_KEY_STATE(VK_SHIFT,   g_vkFocus);
    SET_KEY_STATE(VK_CONTROL, g_vkSpeed);
    SET_KEY_STATE('Z',        g_vkShoot);
    SET_KEY_STATE('X',        g_vkBomb);

    // Prevent alternate keys from applying alongside
    // our custom binds
    const BYTE c_rgAlternateKeys[] = {
        VK_NUMPAD1,
        VK_NUMPAD2,
        VK_NUMPAD3,
        VK_NUMPAD4,
        VK_NUMPAD6,
        VK_NUMPAD7,
        VK_NUMPAD8,
        VK_NUMPAD9,
    };
    for (BYTE bKey : c_rgAlternateKeys)
    {
        lpKeyState[bKey] = 0;
    }
    return TRUE;
}

//_CKbd_GetDeviceState@8
STDMETHODIMP (*CKbd_GetDeviceState_orig)(struct CKbd *, LPVOID);
STDMETHODIMP CKbd_GetDeviceState_hook(
    struct CKbd *pThis,
    LPVOID lpvData
)
{
    HRESULT hr = CKbd_GetDeviceState_orig(pThis, lpvData);
    if (SUCCEEDED(hr))
    {
        PBYTE lpKeyState = (PBYTE)lpvData;
        SET_KEY_STATE(DIK_UP,       g_dikUp);
        SET_KEY_STATE(DIK_DOWN,     g_dikDown);
        SET_KEY_STATE(DIK_LEFT,     g_dikLeft);
        SET_KEY_STATE(DIK_RIGHT,    g_dikRight);
        SET_KEY_STATE(DIK_LSHIFT,   g_dikFocus);
        SET_KEY_STATE(DIK_LCONTROL, g_dikSpeed);
        SET_KEY_STATE(DIK_Z,        g_dikShoot);
        SET_KEY_STATE(DIK_X,        g_dikBomb);

        // Prevent alternate keys from applying alongside
        // our custom binds
        const BYTE c_rgAlternateKeys[] = {
            DIK_RSHIFT,
            DIK_RCONTROL,
            DIK_NUMPAD1,
            DIK_NUMPAD2,
            DIK_NUMPAD3,
            DIK_NUMPAD4,
            DIK_NUMPAD6,
            DIK_NUMPAD7,
            DIK_NUMPAD8,
            DIK_NUMPAD9,
        };
        for (BYTE bKey : c_rgAlternateKeys)
        {
            lpKeyState[bKey] = 0;
        }
    }
    return hr;
}

const struct
{
    LPCWSTR pszName;
    BYTE vk;
    BYTE dik;
} c_rgNameToKeyMap[] = {
    { L"None", 0, 0 },
    { L"Space", VK_SPACE, DIK_SPACE },
    { L"Up", VK_UP, DIK_UP },
    { L"Down", VK_DOWN, DIK_DOWN },
    { L"Left", VK_LEFT, DIK_LEFT },
    { L"Right", VK_RIGHT, DIK_RIGHT },
    { L"LCTRL", VK_LCONTROL, DIK_LCONTROL },
    { L"RCTRL", VK_RCONTROL, DIK_RCONTROL },
    { L"LControl", VK_LCONTROL, DIK_LCONTROL },
    { L"RControl", VK_RCONTROL, DIK_RCONTROL },
    { L"LAlt", VK_LMENU, DIK_LMENU },
    { L"RAlt", VK_RMENU, DIK_RMENU },
    { L"LShift", VK_LSHIFT, DIK_LSHIFT },
    { L"RShift", VK_RSHIFT, DIK_RSHIFT },
    { L"Tab", VK_TAB, DIK_TAB },
    { L"CapsLock", VK_CAPITAL, DIK_CAPITAL },
    { L"F1", VK_F1, DIK_F1 },
    { L"F2", VK_F2, DIK_F2 },
    { L"F3", VK_F3, DIK_F3 },
    { L"F4", VK_F4, DIK_F4 },
    { L"F5", VK_F5, DIK_F5 },
    { L"F6", VK_F6, DIK_F6 },
    { L"F7", VK_F7, DIK_F7 },
    { L"F8", VK_F8, DIK_F8 },
    { L"F9", VK_F9, DIK_F9 },
    { L"F10", VK_F10, DIK_F10 },
    { L"F11", VK_F11, DIK_F11 },
    { L"F12", VK_F12, DIK_F12 },
    { L"Numpad0", VK_NUMPAD0, DIK_NUMPAD0 },
    { L"Numpad1", VK_NUMPAD1, DIK_NUMPAD1 },
    { L"Numpad2", VK_NUMPAD2, DIK_NUMPAD2 },
    { L"Numpad3", VK_NUMPAD3, DIK_NUMPAD3 },
    { L"Numpad4", VK_NUMPAD4, DIK_NUMPAD4 },
    { L"Numpad5", VK_NUMPAD5, DIK_NUMPAD5 },
    { L"Numpad6", VK_NUMPAD6, DIK_NUMPAD6 },
    { L"Numpad7", VK_NUMPAD7, DIK_NUMPAD7 },
    { L"Numpad8", VK_NUMPAD8, DIK_NUMPAD8 },
    { L"Numpad9", VK_NUMPAD9, DIK_NUMPAD9 },
};

bool LoadKey(LPCWSTR pszName, LPBYTE pbvk, LPBYTE pbdik)
{
    WindhawkUtils::StringSetting spszKey = WindhawkUtils::StringSetting::make(pszName);
    if (wcslen(spszKey.get()) == 1)
    {
        BYTE bKey = spszKey.get()[0];
        if (bKey >= 'a' && bKey <= 'z')
            bKey -= 'a' - 'A';
        *pbvk = bKey;

        HKL hkl = GetKeyboardLayout(0);
        *pbdik = MapVirtualKeyExW(bKey, MAPVK_VK_TO_VSC, hkl);
        return true;
    }
    else
    {
        for (const auto &mapping : c_rgNameToKeyMap)
        {
            if (0 == wcsicmp(spszKey.get(), mapping.pszName))
            {
                *pbvk = mapping.vk;
                *pbdik = mapping.dik;
                return true;
            }
        }
    }

    WCHAR szMessage[MAX_PATH];
    swprintf_s(szMessage, L"Invalid key name: '%s'", spszKey.get());
    MessageBoxW(NULL, szMessage, L"Windhawk: Touhou Keyboard Binds", MB_ICONERROR);
    return false;
}

bool LoadSettings(void)
{
    return LoadKey(L"up", &g_vkUp, &g_dikUp)
    && LoadKey(L"down", &g_vkDown, &g_dikDown)
    && LoadKey(L"left", &g_vkLeft, &g_dikLeft)
    && LoadKey(L"right", &g_vkRight, &g_dikRight)
    && LoadKey(L"focus", &g_vkFocus, &g_dikFocus)
    && LoadKey(L"speed", &g_vkSpeed, &g_dikSpeed)
    && LoadKey(L"shoot", &g_vkShoot, &g_dikShoot)
    && LoadKey(L"bomb", &g_vkBomb, &g_dikBomb);
}

const WindhawkUtils::SYMBOL_HOOK dinput8DllHooks[] = {
    {
        {
            L"_CKbd_GetDeviceState@8"
        },
        &CKbd_GetDeviceState_orig,
        CKbd_GetDeviceState_hook,
        false
    }
};

BOOL Wh_ModInit(void)
{
    if (!LoadSettings())
        return FALSE;
    
    HMODULE hDinput8 = LoadLibraryW(L"dinput8.dll");
    if (!hDinput8)
    {
        Wh_Log(L"Failed to load dinput8.dll");
        return FALSE;
    }

    if (!WindhawkUtils::HookSymbols(
        hDinput8,
        dinput8DllHooks,
        ARRAYSIZE(dinput8DllHooks)
    ))
    {
        Wh_Log(L"Failed to hook CKbd_GetDeviceState in dinput8.dll");
        return FALSE;
    }

    Wh_SetFunctionHook(
        (void *)GetKeyboardState,
        (void *)GetKeyboardState_hook,
        (void **)&GetKeyboardState_orig
    );
    return TRUE;
}

BOOL Wh_ModSettingsChanged(BOOL *pfReload)
{
    *pfReload = FALSE;
    return LoadSettings();
}