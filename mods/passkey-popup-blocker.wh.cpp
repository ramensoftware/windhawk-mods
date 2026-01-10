// ==WindhawkMod==
// @id              passkey-popup-blocker
// @name            "Sign in with a passkey" popup blocker 
// @description     Blocks automatic Windows Security "Sign in with a passkey" popup in browser when you open login pages, even if you didn't click login by passkey. while manual click on "Login by passkey" button will work.
// @version         2.0.0
// @author          mode0192
// @github          https://github.com/mode0192
// @include         msedge.exe
// @include         chrome.exe
// @include         brave.exe
// @include         opera.exe
// @include         opera_gx.exe
// @include         vivaldi.exe
// @include         firefox.exe
// @include         waterfox.exe
// @include         librewolf.exe
// @include         thorium.exe
// @include         chromium.exe
// @license         CC-BY-NC-SA-4.0
// @compilerOptions -luser32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Block "Sign in with a passkey" Popup
## ⫸ Description:
This mod blocks the annoying automatic Windows Security "Sign in with a passkey" popup that appears immediately when loading a login page where you have previously saved passkey for it, even if you didn't click login by passkey. It uses a **Smart Sensor** to distinguish between automated "Sign in by a passkey" and real user intent so it can appears.

‎‎‎‎‎‎‎‎ㅤ
## ⫸ Photos
### Before
![Automatic Passkeys images](https://raw.githubusercontent.com/mode0192/windhawkimages/main/images/image1_small.jpg)


‎‎‎‎‎‎‎‎ㅤ
## ⫸ Features
* **Smart Blocking**: Silently blocks passkey prompts automatically appears on page load.
* **Intent Detection**: Instantly allows the popup if you have **Clicked** (also supports touch screen) or pressed **Enter/Space** on login by passkey in the last 0.5 seconds.
* **Strict Page Logic**: Prevents clicks on a previous page (referrals) from triggering popups on the new page.
* **Zero Latency**: Runs natively in the browser process (`DispatchMessageW` hook).
* **Privacy First**: Does not log keystrokes or transmit data.

‎‎‎‎‎‎‎‎ㅤ
## ⫸ How to Use
No configuration is needed. It works out of the box.

| Action | Result |
| :--- | :--- |
| **Open Login Page** | The automatic popup is **Blocked** 🛑. |
| **Click "Sign in by passkey"** | The popup is **Allowed** ✅. |
| **Hit Enter on "Sign in by passkey"** | The popup is **Allowed** ✅. |
| **Click Link -> New Page Loads** | The automatic popup is **Blocked** 🛑 (Interaction time expired during page load). |

‎‎‎‎‎‎‎‎ㅤ
## ⫸ Advanced Settings
You can now customize the strictness of the blocker in the **Settings** tab.
* **Interaction Timeout**: Default is 500ms. *Only change this if the mod is blocking legitimate clicks for you.*

‎‎‎‎‎‎‎‎ㅤ
## ⫸ Supported Browsers
The mod works with almost all Chromium-based and Gecko-based browsers:
* **Chrome**
* **Edge**
* **Firefox** (and forks like Waterfox, Librewolf)
* **Brave**
* **Opera** / **Opera GX**
* **Vivaldi**
* **Thorium**

‎‎‎‎‎‎‎‎ㅤ
## 🔒 Privacy & Security
Transparency is critical for mods that handle input.
* **No Keylogging**: The mod **does not** record what you type. It only listens for specific "submission" keys (`Enter`, `Space`) and generic Mouse Clicks (`WM_LBUTTONUP`) to update a simple timestamp.
* **No Data Transmission**: The mod runs entirely locally. No data is sent to any server.
* **Safety**: Passwords and usernames are never touched or inspected by this code.

‎‎‎‎‎‎‎‎ㅤ
## 📄 Credits
* **Author**: [mode0192](https://github.com/mode0192)
* **License**: CC BY-NC-SA 4.0 (Free for personal use, no commercial sale)

‎‎‎‎‎‎‎‎ㅤ
## 📜 Changelog
**2.0.0** (Jan 10, 2026)
* Fixed: Referrals to login page triggering the passkey popup
* Added: Setting to change the Interaction timeout

**1.0.0** (Jan 9, 2026)
* Initial release
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- timeout: 500
  $name: Interaction Timeout (ms)
  $description: "ADVANCED: The time window (in milliseconds) after a click where the popup is allowed. Default is 500ms. WARNING: Do not change this unless the mod is blocking valid clicks."
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <windhawk_utils.h>

// -----------------------------------------------------------------------------
// Global State
// -----------------------------------------------------------------------------
static DWORD g_LastInteractionTime = 0;
static int g_AllowedLatencyMs = 500; 

// -----------------------------------------------------------------------------
// Definitions
// -----------------------------------------------------------------------------
typedef LRESULT (WINAPI *DispatchMessageW_t)(CONST MSG *lpMsg);
typedef HRESULT (WINAPI *WebAuthNAuthenticatorGetAssertion_t)(
    HWND hWnd, 
    LPCWSTR pwszRpId, 
    void* pClientData, 
    void* pAssertionOptions, 
    void** ppAssertion
);

DispatchMessageW_t pOriginalDispatchMessageW;
WebAuthNAuthenticatorGetAssertion_t pOriginalGetAssertion;

// -----------------------------------------------------------------------------
// Helper: Load Settings
// -----------------------------------------------------------------------------
void LoadSettings()
{
    g_AllowedLatencyMs = Wh_GetIntSetting(L"timeout");
    if (g_AllowedLatencyMs <= 0) g_AllowedLatencyMs = 500;
}

// -----------------------------------------------------------------------------
// Hook 1: The Reliable Sensor (DispatchMessageW)
// -----------------------------------------------------------------------------
// This monitors the internal message loop.
// to see what the browser is actually processing.
LRESULT WINAPI DetourDispatchMessageW(CONST MSG *lpMsg)
{
    // A. Validation: Ensure message exists
    if (lpMsg) {
        
        UINT msg = lpMsg->message;
        bool isInteraction = false;

        // B. Mouse Clicks (Button Release)
        // We look for LBUTTONUP (Left Click Release) as this triggers buttons.
        if (msg == WM_LBUTTONUP || msg == WM_RBUTTONUP || msg == WM_NCLBUTTONUP) {
            isInteraction = true;
        }
        
        // C. Keyboard (Strict)
        // Only count keys that actually submit forms (Enter or Space).
        // Ignoring others saves resources and improves privacy.
        else if (msg == WM_KEYUP) {
            if (lpMsg->wParam == VK_RETURN || lpMsg->wParam == VK_SPACE) {
                isInteraction = true;
            }
        }

        // D. Touch Screens (Added Feature)
        // If you use a laptop touch screen, this ensures it works too.
        else if (msg == WM_POINTERUP) {
            isInteraction = true;
        }

        // E. Update Timestamp
        if (isInteraction) {
            g_LastInteractionTime = GetTickCount();
        }
    }

    // Always run the original function so the browser works normal!
    return pOriginalDispatchMessageW(lpMsg);
}

// -----------------------------------------------------------------------------
// Hook 2: The Gatekeeper (WebAuthn)
// -----------------------------------------------------------------------------
HRESULT WINAPI DetourGetAssertion(
    HWND hWnd, 
    LPCWSTR pwszRpId, 
    void* pClientData, 
    void* pAssertionOptions, 
    void** ppAssertion
)
{
    DWORD currentTime = GetTickCount();
    DWORD timeSinceInteraction = currentTime - g_LastInteractionTime;

    // --- CHECK ---
    // If user clicked < [Configured Time] seconds ago -> ALLOW
    if (timeSinceInteraction < (DWORD)g_AllowedLatencyMs) {
        return pOriginalGetAssertion(hWnd, pwszRpId, pClientData, pAssertionOptions, ppAssertion);
    }
    
    // Else -> BLOCK (Return Not Found)
    return 0x80090011; 
}

// -----------------------------------------------------------------------------
// Initialization
// -----------------------------------------------------------------------------
BOOL Wh_ModInit()
{
    // 0. Load Settings
    LoadSettings();

    // 1. Hook WebAuthn (The Popup)
    HMODULE hWebAuthn = LoadLibraryEx(L"webauthn.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!hWebAuthn) return FALSE;

    void* pFuncAddrAssertion = (void*)GetProcAddress(hWebAuthn, "WebAuthNAuthenticatorGetAssertion");
    if (pFuncAddrAssertion) {
        Wh_SetFunctionHook(
            pFuncAddrAssertion,
            (void*)DetourGetAssertion,
            (void**)&pOriginalGetAssertion
        );
    } else {
        return FALSE; 
    }

    // 2. Hook User32 (The Input Sensor)
    HMODULE hUser32 = GetModuleHandle(L"user32.dll");
    if (hUser32) {
        // We go back to DispatchMessageW because it PROVED to be more reliable.
        void* pFuncDispatch = (void*)GetProcAddress(hUser32, "DispatchMessageW");
        if (pFuncDispatch) {
             Wh_SetFunctionHook(
                pFuncDispatch,
                (void*)DetourDispatchMessageW,
                (void**)&pOriginalDispatchMessageW
            );
        }
    }

    return TRUE;
}

// -----------------------------------------------------------------------------
// Events
// -----------------------------------------------------------------------------
void Wh_ModSettingsChanged()
{
    LoadSettings();
}
