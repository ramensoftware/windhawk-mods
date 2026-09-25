// ==WindhawkMod==
// @id              snipping-tool-escape-quit
// @name            Snipping Tool Escape to Quit
// @description     Instantly close the Windows 10 Snipping Tool by pressing the Escape key, improving workflow speed.
// @version         1.0.0.4
// @author          TheShadyRainbow4
// @github          https://github.com/TheShadyRainbow4
// @homepage        https://github.com/TheShadyRainbow4
// @include         snippingtool.exe
// @compilerOptions -luser32
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Snipping Tool Escape to Quit
This mod dramatically improves the quality of life for heavy users of the classic Windows 10 IoT Enterprise LTSC Snipping Tool (`snippingtool.exe`).

By default, the Snipping Tool might cancel a current snip when you press Escape, but it forces you to manually click the "X" to close the application entirely. This mod alters that behavior. When enabled, hitting the Escape key will immediately and cleanly exit the Snipping Tool. 

### Getting started
- Compile and enable the mod.
- Open the Snipping Tool from your Start menu.
- Press the **Escape (Esc)** key and observe that the application completely closes. 

## Best if paired with my other mode snipping tool multi instance!
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
# Here you can define settings, in YAML format, that the mod users will be able to configure.
- enableEscapeToQuit: true
  $name: Enable Escape to Quit
  $description: When enabled, pressing the Escape key will forcefully close the Snipping Tool application.
*/
// ==/WindhawkModSettings==

#include <windows.h>

// Define the settings structure to hold our mod configuration
struct {
    bool enableEscapeToQuit;
} settings;

// Define the hook for TranslateMessage to intercept keystrokes before they are processed
using TranslateMessage_t = decltype(&TranslateMessage);
TranslateMessage_t TranslateMessage_Original;

BOOL WINAPI TranslateMessage_Hook(const MSG *lpMsg) {
    // Check if the current message is a key press (WM_KEYDOWN) and specifically the Escape key (VK_ESCAPE)
    if (lpMsg && lpMsg->message == WM_KEYDOWN && lpMsg->wParam == VK_ESCAPE) {
        
        // Verify if the user has the setting enabled in the Windhawk interface
        if (settings.enableEscapeToQuit) {
            Wh_Log(L"Escape key detected. Terminating Snipping Tool.");
            
            // Immediately cleanly terminate the process
            ExitProcess(0);
            
            // Return TRUE to indicate the message was handled
            return TRUE;
        }
    }

    // For all other keys or messages, call the original function so the app behaves normally
    return TranslateMessage_Original(lpMsg);
}

// Function to pull the configuration from the Windhawk UI
void LoadSettings() {
    settings.enableEscapeToQuit = Wh_GetIntSetting(L"enableEscapeToQuit");
}

// The mod is being initialized, load settings, hook functions, and do other initialization stuff if required.
BOOL Wh_ModInit() {
    Wh_Log(L"Init Snipping Tool Escape to Quit");

    // Load initial settings when the mod starts
    LoadSettings();

    // Load user32.dll to find the TranslateMessage function memory address
    HMODULE user32Module = LoadLibrary(L"user32.dll");
    if (user32Module) {
        TranslateMessage_t TranslateMessage = (TranslateMessage_t)GetProcAddress(user32Module, "TranslateMessage");
        
        // Apply the hook to intercept the messages
        Wh_SetFunctionHook((void*)TranslateMessage, (void*)TranslateMessage_Hook, (void**)&TranslateMessage_Original);
    }

    return TRUE;
}

// The mod is being unloaded, free all allocated resources.
void Wh_ModUninit() {
    Wh_Log(L"Uninit Snipping Tool Escape to Quit");
}

// The mod setting were changed, reload them.
void Wh_ModSettingsChanged() {
    Wh_Log(L"SettingsChanged");

    // Reload settings dynamically to apply changes instantly without restarting the target program
    LoadSettings();
}