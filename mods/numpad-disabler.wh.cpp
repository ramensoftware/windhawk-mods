// ==WindhawkMod==
// @id              numpad-disabler
// @name            Numpad Disabler
// @description     Selectively disables Numpad key detection for a specific application. Prevents background tools or game trainers from intercepting Numpad inputs.
// @version         1.0.0
// @author          osmanonurkoc
// @github          https://github.com/osmanonurkoc
// @compilerOptions -luser32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
 # * Numpad Disabler
 This mod selectively "deafens" a specific application so it cannot detect Numpad key presses.

 ### 🎯 Common Use Cases
 * **Gaming:** Prevents game trainers, overlays, or macro tools in the background from activating when you use the Numpad inside your game.
 * **Productivity:** Stops specific background applications from hijacking or reacting to your Numpad inputs while you work.

 ### 🛡️ Performance & Stability Guarantee
 By relying exclusively on Windhawk's native process filtering, this mod has zero performance overhead. It will not hook or interfere with your web browsers, Windows Explorer, or your main games unless you explicitly target them.

 ### ⚙️ Configuration Guide
 The mod targets nothing by default. To choose which applications to block:
 1. Open the mod in Windhawk and go to the **Advanced** tab.
 2. Add the exact executable name(s) of the application(s) you want to block (e.g., `trainer.exe` or `macro_tool.exe`) to the **Custom process inclusion list**.
 3. Click Save. The mod takes effect immediately.
 */
// ==/WindhawkModReadme==

#include <windows.h>
#include <windhawk_utils.h>

// ---------------------------------------------------------------------------
// Core Logic & API Hooks
// ---------------------------------------------------------------------------

// Validates whether the queried virtual key code corresponds to a Numpad key
bool is_numpad_vk(int vKey) {
    // VK_NUMPAD0 (0x60) through VK_DIVIDE (0x6F) covers the entire standard Numpad range
    return (vKey >= VK_NUMPAD0 && vKey <= VK_DIVIDE);
}

// Hook for GetAsyncKeyState (Primary method utilized by input overlays and trainers)
using GetAsyncKeyState_t = decltype(&GetAsyncKeyState);
GetAsyncKeyState_t GetAsyncKeyState_Original;
SHORT WINAPI GetAsyncKeyState_Hook(int vKey) {
    if (is_numpad_vk(vKey)) {
        return 0; // Deceive the target application by indicating the key is NOT pressed
    }
    return GetAsyncKeyState_Original(vKey);
}

// Hook for GetKeyState
using GetKeyState_t = decltype(&GetKeyState);
GetKeyState_t GetKeyState_Original;
SHORT WINAPI GetKeyState_Hook(int vKey) {
    if (is_numpad_vk(vKey)) {
        return 0; // Deceive the target application
    }
    return GetKeyState_Original(vKey);
}

// Hook for RegisterHotKey (Secondary method utilized by background listeners)
using RegisterHotKey_t = decltype(&RegisterHotKey);
RegisterHotKey_t RegisterHotKey_Original;
BOOL WINAPI RegisterHotKey_Hook(HWND hWnd, int id, UINT fsModifiers, UINT vk) {
    if (is_numpad_vk(vk)) {
        // Natural failure code prevents the application from crashing
        SetLastError(ERROR_HOTKEY_ALREADY_REGISTERED);
        return FALSE;
    }
    return RegisterHotKey_Original(hWnd, id, fsModifiers, vk);
}

// ---------------------------------------------------------------------------
// Mod Lifecycle
// ---------------------------------------------------------------------------

BOOL Wh_ModInit() {
    // Deploy type-checked hooks solely within the verified target process
    WindhawkUtils::SetFunctionHook(GetAsyncKeyState, GetAsyncKeyState_Hook, &GetAsyncKeyState_Original);
    WindhawkUtils::SetFunctionHook(GetKeyState, GetKeyState_Hook, &GetKeyState_Original);
    WindhawkUtils::SetFunctionHook(RegisterHotKey, RegisterHotKey_Hook, &RegisterHotKey_Original);

    return TRUE;
}
