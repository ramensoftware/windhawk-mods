// ==WindhawkMod==
// @id              passkey-popup-blocker
// @name            "Sign in with a passkey" popup blocker
// @description     Blocks automatic Windows Security "Sign in with a passkey" popup in browser unless you explicitly click "Sign in".
// @version         2.1.0
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
## ⫸ Description
This mod blocks the annoying automatic Windows Security "Sign in with a passkey" popup that appears immediately when loading a login page where you have previously saved passkey for it, even if you didn't click login by passkey. It uses a **Smart Sensor** to distinguish between automated "Sign in by a passkey" and real user intent so it can appear.

‎‎‎‎‎‎‎‎ㅤ
## ⫸ Photos
### Before
![Automatic Passkeys images](https://raw.githubusercontent.com/mode0192/windhawkimages/main/images/image1_small.jpg)


‎‎‎‎‎‎‎‎ㅤ
## ⫸ Features
* **Smart Blocking**: Silently blocks passkey prompts automatically appearing on page load.
* **Intent Detection**: Instantly allows the popup if you have **Clicked** (also supports touch screen) or pressed **Enter/Space** on login by passkey in the last 0.5 seconds.
* **Strict Page Logic**: Prevents clicks on a previous page (referrals) from triggering popups on the new page.
* **Robust Input Hook**: Mod uses a reliable sensor (`PeekMessage`) instead of ('DispatchMessage') * fixes issues where the mod would stop working (blocking all passkeys) after closing and reopening the browser.
* **Zero Latency**: Runs natively in the browser process.
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
* **No Keylogging**: The mod **does not** record what you type. It only checks if an interaction occurred (Yes/No) and updates a timestamp.
* **No Data Transmission**: The mod runs entirely locally. No data is sent to any server.
* **Safety**: Passwords and usernames are never touched or inspected by this code.

‎‎‎‎‎‎‎‎ㅤ
## 📄 Credits
* **Author**: [mode0192](https://github.com/mode0192)
* **License**: CC BY-NC-SA 4.0 (Free for personal use, no commercial sale)

‎‎‎‎‎‎‎‎ㅤ
## 📜 Changelog
**2.1.0** (Jan 15, 2026)
* Fixed: Critical bug where the mod blocked all passkeys (even manual clicks) after closing and reopening the browser.
* Improved: Fixed compatibility conflicts with other mods (e.g., Edge Tab Manager blockers).
* Tech: Switched from `DispatchMessage` to `PeekMessage` to correctly handle browser process restarts and background threads.

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
typedef BOOL (WINAPI *PeekMessageW_t)(LPMSG lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax, UINT wRemoveMsg);
typedef BOOL (WINAPI *GetMessageW_t)(LPMSG lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax);
typedef HRESULT (WINAPI *WebAuthNAuthenticatorGetAssertion_t)(
    HWND hWnd, 
    LPCWSTR pwszRpId, 
    void* pClientData, 
    void* pAssertionOptions, 
    void** ppAssertion
);

PeekMessageW_t pOriginalPeekMessageW;
GetMessageW_t pOriginalGetMessageW;
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
// Core Logic: Check Input
// -----------------------------------------------------------------------------
void CheckInputMessage(CONST MSG *lpMsg) {
    if (!lpMsg) return;

    UINT msg = lpMsg->message;
    bool isInteraction = false;

    // A. Mouse Clicks (Button Release)
    if (msg == WM_LBUTTONUP || msg == WM_RBUTTONUP || msg == WM_NCLBUTTONUP) {
        isInteraction = true;
    }
    // B. Keyboard (Strict - Enter/Space)
    else if (msg == WM_KEYUP) {
        if (lpMsg->wParam == VK_RETURN || lpMsg->wParam == VK_SPACE) {
            isInteraction = true;
        }
    }
    // C. Touch Screens (Added Feature)
    else if (msg == WM_POINTERUP) {
        isInteraction = true;
    }

    if (isInteraction) {
        g_LastInteractionTime = GetTickCount();
    }
}

// -----------------------------------------------------------------------------
// Hook 1: The Robust Sensor (PeekMessageW)
// -----------------------------------------------------------------------------
// Changed to PeekMessageW from DispatchMessage to fix issues with browser restarts
BOOL WINAPI DetourPeekMessageW(LPMSG lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax, UINT wRemoveMsg)
{
    // Run original first to get the message
    BOOL result = pOriginalPeekMessageW(lpMsg, hWnd, wMsgFilterMin, wMsgFilterMax, wRemoveMsg);

    // If we got a message, check it
    if (result && lpMsg) {
        CheckInputMessage(lpMsg);
    }

    return result;
}

// -----------------------------------------------------------------------------
// Hook 2: The Backup Sensor (GetMessageW)
// -----------------------------------------------------------------------------
// Some loops use GetMessage instead of PeekMessage. We hook both to be safe.
BOOL WINAPI DetourGetMessageW(LPMSG lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax)
{
    BOOL result = pOriginalGetMessageW(lpMsg, hWnd, wMsgFilterMin, wMsgFilterMax);

    if (result != -1 && result != 0 && lpMsg) {
        CheckInputMessage(lpMsg);
    }

    return result;
}

// -----------------------------------------------------------------------------
// Hook 3: The Gatekeeper (WebAuthn)
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

    // Check strict intent (Click/Enter within last 500ms (default))
    if (timeSinceInteraction < (DWORD)g_AllowedLatencyMs) {
        return pOriginalGetAssertion(hWnd, pwszRpId, pClientData, pAssertionOptions, ppAssertion);
    }
    
    // Otherwise Block
    return 0x80090011; 
}

// -----------------------------------------------------------------------------
// Initialization
// -----------------------------------------------------------------------------
BOOL Wh_ModInit()
{
    LoadSettings();

    // 1. Hook WebAuthn
    HMODULE hWebAuthn = LoadLibraryEx(L"webauthn.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (hWebAuthn) {
        void* pFunc = (void*)GetProcAddress(hWebAuthn, "WebAuthNAuthenticatorGetAssertion");
        if (pFunc) {
            Wh_SetFunctionHook(pFunc, (void*)DetourGetAssertion, (void**)&pOriginalGetAssertion);
        }
    }

    // 2. Hook Input Sources (User32)
    HMODULE hUser32 = GetModuleHandle(L"user32.dll");
    if (hUser32) {
        // Hook PeekMessageW (Most common loop method)
        void* pFuncPeek = (void*)GetProcAddress(hUser32, "PeekMessageW");
        if (pFuncPeek) {
             Wh_SetFunctionHook(pFuncPeek, (void*)DetourPeekMessageW, (void**)&pOriginalPeekMessageW);
        }

        // Hook GetMessageW (Alternative loop method)
        void* pFuncGet = (void*)GetProcAddress(hUser32, "GetMessageW");
        if (pFuncGet) {
             Wh_SetFunctionHook(pFuncGet, (void*)DetourGetMessageW, (void**)&pOriginalGetMessageW);
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
