// ==WindhawkMod==
// @id              numpad-disabler
// @name            Numpad Disabler
// @description     Selectively disables Numpad key detection for a specific application. Prevents background tools or game trainers from intercepting Numpad inputs.
// @version         1.0.0
// @author          osmanonurkoc
// @github          https://github.com/osmanonurkoc
// @include         *
// @compilerOptions -luser32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Numpad Disabler
This mod selectively "deafens" a specific application so it cannot detect Numpad key presses. 

### 🎯 Common Use Cases
* **Gaming:** Prevents game trainers, overlays, or macro tools in the background from activating when you use the Numpad inside your game.
* **Productivity:** Stops specific background applications from hijacking or reacting to your Numpad inputs while you work.

### 🛡️ Performance & Stability Guarantee
To ensure zero system crashes and zero performance overhead, this mod utilizes an ultra-fast filtering mechanism. If a starting process does not match your specified target application, the mod instantly unloads itself. It will not hook or interfere with your web browsers, Windows Explorer, or your main games.

### ⚙️ Configuration Guide
1. Go to the **Settings** tab of this mod.
2. Enter the exact executable name of the application you want to block in the "Target Application Name" field (e.g., `trainer.exe` or `macro_tool.exe`).
3. **Important:** Because the mod automatically unloads from non-target apps, you must **restart the target application** after modifying the settings for the block to take effect.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- target_app: ""
  $name: "Target Application Name"
  $description: "Enter the exact executable name of the application to block (e.g., trainer.exe)."
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <string>
#include <algorithm>

bool g_isTargetAppBlocked = false;

// ---------------------------------------------------------------------------
// Settings and Initialization
// ---------------------------------------------------------------------------

// Reads settings from the Windhawk UI and checks if the current process is the designated target
void LoadSettings() {
    g_isTargetAppBlocked = false;

    LPCWSTR targetAppSetting = Wh_GetStringSetting(L"target_app");
    if (!targetAppSetting) return;

    std::wstring targetApp = targetAppSetting;
    Wh_FreeStringSetting(targetAppSetting);

    // Sanitize the input by trimming spaces and quotes securely
    size_t first = targetApp.find_first_not_of(L" \t\"'");
    if (first == std::wstring::npos) {
        targetApp.clear();
    } else {
        size_t last = targetApp.find_last_not_of(L" \t\"'");
        targetApp = targetApp.substr(first, (last - first + 1));
    }

    if (targetApp.empty()) return;

    std::transform(targetApp.begin(), targetApp.end(), targetApp.begin(), ::towlower);

    // Retrieve the executable name of the current process
    WCHAR szAppPath[MAX_PATH];
    GetModuleFileNameW(NULL, szAppPath, MAX_PATH);
    WCHAR *pExeName = wcsrchr(szAppPath, L'\\');
    if (pExeName) {
        pExeName++; 
    } else {
        pExeName = szAppPath;
    }

    std::wstring currentExeName = pExeName;
    std::transform(currentExeName.begin(), currentExeName.end(), currentExeName.begin(), ::towlower);

    // Flag the process if it matches the user's setting
    if (currentExeName == targetApp) {
        g_isTargetAppBlocked = true;
    }
}

// ---------------------------------------------------------------------------
// Core Logic & API Hooks
// ---------------------------------------------------------------------------

// Validates whether the queried virtual key code corresponds to a Numpad key
bool is_numpad_vk(int vKey) {
    // VK_NUMPAD0 (0x60) through VK_DIVIDE (0x6F) covers the entire standard Numpad range
    return (vKey >= VK_NUMPAD0 && vKey <= VK_DIVIDE);
}

// Hook for GetAsyncKeyState (Primary method utilized by input overlays and trainers)
typedef SHORT (WINAPI *GetAsyncKeyState_t)(int vKey);
GetAsyncKeyState_t GetAsyncKeyState_Original;
SHORT WINAPI GetAsyncKeyState_Hook(int vKey) {
    if (g_isTargetAppBlocked && is_numpad_vk(vKey)) {
        return 0; // Deceive the target application by indicating the key is NOT pressed
    }
    return GetAsyncKeyState_Original(vKey);
}

// Hook for GetKeyState
typedef SHORT (WINAPI *GetKeyState_t)(int vKey);
GetKeyState_t GetKeyState_Original;
SHORT WINAPI GetKeyState_Hook(int vKey) {
    if (g_isTargetAppBlocked && is_numpad_vk(vKey)) {
        return 0; // Deceive the target application
    }
    return GetKeyState_Original(vKey);
}

// Hook for RegisterHotKey (Secondary method utilized by background listeners)
typedef BOOL (WINAPI *RegisterHotKey_t)(HWND hWnd, int id, UINT fsModifiers, UINT vk);
RegisterHotKey_t RegisterHotKey_Original;
BOOL WINAPI RegisterHotKey_Hook(HWND hWnd, int id, UINT fsModifiers, UINT vk) {
    if (g_isTargetAppBlocked && is_numpad_vk(vk)) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE; // Actively prevent the target application from registering a Numpad hotkey
    }
    return RegisterHotKey_Original(hWnd, id, fsModifiers, vk);
}

// ---------------------------------------------------------------------------
// Mod Lifecycle
// ---------------------------------------------------------------------------

BOOL Wh_ModInit() {
    LoadSettings();
    
    // Strict inclusion filter: If the current process is not the designated target, 
    // instantly abort the initialization to prevent unnecessary hooking across the system.
    if (!g_isTargetAppBlocked) {
        return FALSE; 
    }

    // Deploy hooks solely within the verified target process
    Wh_SetFunctionHook((void*)GetAsyncKeyState, (void*)GetAsyncKeyState_Hook, (void**)&GetAsyncKeyState_Original);
    Wh_SetFunctionHook((void*)GetKeyState, (void*)GetKeyState_Hook, (void**)&GetKeyState_Original);
    Wh_SetFunctionHook((void*)RegisterHotKey, (void*)RegisterHotKey_Hook, (void**)&RegisterHotKey_Original);
    
    return TRUE;
}

void Wh_ModSettingsChanged() {
    LoadSettings();
}