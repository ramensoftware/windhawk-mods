// ==WindhawkMod==
// @id              passkey-popup-blocker
// @name            "Sign in with a passkey" popup blocker
// @description     Blocks automatic Windows Security "Sign in with a passkey" popup in browser. Only allows them after a recent click/tap or Enter/Space.
// @version         3.0.0
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
// @compilerOptions -luser32 -ladvapi32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Block "Sign in with a passkey" Popup
## ⫸ Description
This mod blocks the automatic **Windows Security passkey** popup that may appear immediately when opening a login page (because a passkey exists for that site), even if you didn’t choose “Sign in with passkey”.

The popup is only allowed after **recent user intent**:
- a **click/tap** (mouse or touch), or
- pressing **Enter** / **Space**
within a short time window (default **500ms**).

‎‎‎‎‎‎‎‎ㅤ
## ⫸ Photos
### Before
![Automatic Passkeys images](https://raw.githubusercontent.com/mode0192/windhawkimages/main/images/image1_small.jpg)

‎‎‎‎‎‎‎‎ㅤ
## ⫸ Features
* **Smart Blocking**: Silently blocks passkey prompts that appear automatically on page load.
* **Intent Detection**: Allows the popup only after a recent **Click / Tap / Enter / Space**.
* **Multi-Process Reliable (Chromium browsers)**: Syncs intent across Chrome/Edge/Brave/etc. processes (tabs / sandboxed processes) using **shared memory**, so clicks in one process are visible to the WebAuthn call in another.
* **Robust Input Hooks**: Hooks both `PeekMessageW` and `GetMessageW` so it works across different message loops.
* **Accurate Timing**: Uses the message timestamp when available, with safe fallback to `GetTickCount()`.
* **Configurable Block Behavior**: Choose which WebAuthn error code is returned when blocking (recommended default behaves like “user cancelled”).
* **Zero Latency**: Runs natively in the browser process.
* **Privacy First**: Does not log keystrokes or transmit data (only updates a timestamp on intent).

‎‎‎‎‎‎‎‎ㅤ
## ⫸ Case Scenarios
No configuration is needed. It works out of the box.

| Scenario | What happens |
| :--- | :--- |
| **Open a login page** (where Windows normally auto-prompts) | The automatic popup is **Blocked** 🛑 |
| **Click / Tap "Sign in with passkey"** on the website | The popup is **Allowed** ✅ |
| **Keyboard users**: focus the passkey sign-in option and press **Enter** / **Space** | The popup is **Allowed** ✅ |
| A site tries to trigger passkeys automatically (scripts/autofill/background behavior) | The popup is **Blocked** 🛑 |
| A website feels **too strict** (your valid click is still getting blocked) | Increase **Interaction Timeout** in settings |
| A specific website behaves strangely when the popup is blocked | Switch **Block Behavior** to **Not Found (Compatibility)** |

‎‎‎‎‎‎‎‎ㅤ
## ⫸ Advanced Settings

### Interaction Timeout (ms)
Default: **500ms** (max **10000ms**) — how long after a click/keypress the popup is allowed.  
Only change this if it blocks a real click.

### Block Behavior
What error code to return when blocking:
- **User Cancelled (Recommended)** → `NTE_USER_CANCELLED`
- **Not Found (Compatibility)** → `NTE_NOT_FOUND`

‎‎‎‎‎‎‎‎ㅤ
## ⫸ Supported Browsers
The mod works with almost all Chromium-based and Gecko-based browsers:
* **Chrome**
* **Edge**
* **Firefox** (and forks like Waterfox, LibreWolf)
* **Brave**
* **Opera** / **Opera GX**
* **Vivaldi**
* **Thorium**
* **Chromium**

‎‎‎‎‎‎‎‎ㅤ
## 🔒 Privacy & Security
Transparency is critical for mods that handle input.
* **No Keylogging**: The mod does **not** record what you type. It only checks for specific “intent” inputs (click/tap or Enter/Space) and updates a timestamp.
* **No Data Transmission**: The mod runs entirely locally. No data is sent to any server.
* **Safety**: Passwords, usernames, and page content are never touched or inspected.

‎‎‎‎‎‎‎‎ㅤ
## 📄 Credits
* **Author**: [mode0192](https://github.com/mode0192)
* **License**: CC BY-NC-SA 4.0 (Free for personal use, no commercial sale)

‎‎‎‎‎‎‎‎ㅤ
## 📜 Changelog
**3.0.0** (Feb 10, 2026)
* **Major**: Added **multi-process browser support** using **shared memory** to sync user intent across Chrome/Edge tabs and sandboxed processes (fixes cases where clicks were not seen by the WebAuthn call).
* **Added**: New **Block Behavior** setting: return **NTE_USER_CANCELLED** (default/recommended) or **NTE_NOT_FOUND** (compatibility).
* **Improved**: Expanded intent detection to include mouse/touch **down/up** events and key **down/up** for **Enter/Space** (including system key variants).
* **Improved**: Added **Null DACL** security attributes so sandboxed Chromium processes can access the shared memory mapping.
* **Improved**: More robust initialization — the mod fails cleanly if the WebAuthn hook can’t be established.
* **Fixed**: Proper resource cleanup on unload (`Wh_ModUninit`) and safer timing logic (wraparound-safe tick arithmetic).

**2.1.0** (Jan 15, 2026)
* **Fixed**: Critical bug where the mod blocked all passkeys (even manual clicks) after closing and reopening the browser.
* **Improved**: Fixed compatibility conflicts with other mods (e.g., Edge Tab Manager blockers).
* **Tech**: Switched from `DispatchMessage` to `PeekMessage` to correctly handle browser process restarts and background threads.

**2.0.0** (Jan 10, 2026)
* Initial release
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- timeout: 500
  $name: Interaction Timeout (ms)
  $description: "Time window after click/keypress to allow popup. Default: 500ms."

- block_result: user_cancelled
  $name: Block Behavior
  $description: "Error code returned when blocking."
  $options:
    - user_cancelled: User Cancelled (Recommended)
    - not_found: Not Found (Compatibility)
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <winerror.h>
#include <windhawk_utils.h>
#include <wchar.h>  // wcsrchr, swprintf_s

#define DEFAULT_TIMEOUT 500
#define MAX_TIMEOUT     10000

// Shared state for multi-process browsers (Chrome/Edge use separate processes per tab)
#pragma pack(push, 4)
struct SharedState {
    volatile LONG lastTick;
};
#pragma pack(pop)

// Globals
static HANDLE       g_hMap = nullptr;
static SharedState* g_pState = nullptr;
static LONG         g_localTick = 0;
static int          g_timeoutMs = DEFAULT_TIMEOUT;
static bool         g_blockAsCancel = true;

// Function pointers
typedef BOOL (WINAPI *PeekMessageW_t)(LPMSG, HWND, UINT, UINT, UINT);
typedef BOOL (WINAPI *GetMessageW_t)(LPMSG, HWND, UINT, UINT);
typedef HRESULT (WINAPI *WebAuthNGetAssertion_t)(HWND, LPCWSTR, void*, void*, void**);

static PeekMessageW_t g_origPeek = nullptr;
static GetMessageW_t g_origGet = nullptr;
static WebAuthNGetAssertion_t g_origWebAuthn = nullptr;
static HMODULE g_hWebAuthn = nullptr;

//-----------------------------------------------------------------------------
// Shared Memory (syncs user intent across browser processes)
//-----------------------------------------------------------------------------
static void InitSharedMemory() {
    wchar_t path[MAX_PATH];
    GetModuleFileNameW(nullptr, path, _countof(path));

    wchar_t* name = wcsrchr(path, L'\\');
    name = name ? name + 1 : path;

    DWORD sessionId = 0;
    ProcessIdToSessionId(GetCurrentProcessId(), &sessionId);

    // Safer mapName buffer + count-aware swprintf_s
    wchar_t mapName[MAX_PATH + 64];
    swprintf_s(mapName, _countof(mapName), L"Local\\wh_passkey_%s_%u", name, sessionId);

    // NULL DACL: Required for Chrome/Edge sandbox processes to access shared memory
    SECURITY_DESCRIPTOR sd;
    InitializeSecurityDescriptor(&sd, SECURITY_DESCRIPTOR_REVISION);
    SetSecurityDescriptorDacl(&sd, TRUE, nullptr, FALSE);
    SECURITY_ATTRIBUTES sa = { sizeof(sa), &sd, FALSE };

    g_hMap = CreateFileMappingW(INVALID_HANDLE_VALUE, &sa, PAGE_READWRITE, 0,
                                sizeof(SharedState), mapName);
    if (g_hMap) {
        g_pState = (SharedState*)MapViewOfFile(g_hMap, FILE_MAP_ALL_ACCESS, 0, 0,
                                               sizeof(SharedState));

        // If mapping fails, close the handle so we fall back cleanly
        if (!g_pState) {
            CloseHandle(g_hMap);
            g_hMap = nullptr;
        }
    }
}

static void CleanupSharedMemory() {
    if (g_pState) UnmapViewOfFile(g_pState);
    if (g_hMap) CloseHandle(g_hMap);
    g_pState = nullptr;
    g_hMap = nullptr;
}

static inline void SetLastTick(DWORD tick) {
    if (g_pState) {
        InterlockedExchange(&g_pState->lastTick, (LONG)tick);
    } else {
        g_localTick = (LONG)tick;
    }
}

static inline DWORD GetLastTick() {
    if (g_pState) {
        return (DWORD)InterlockedCompareExchange(&g_pState->lastTick, 0, 0);
    }
    return (DWORD)g_localTick;
}

//-----------------------------------------------------------------------------
// Settings
//-----------------------------------------------------------------------------
static void LoadSettings() {
    g_timeoutMs = Wh_GetIntSetting(L"timeout");
    if (g_timeoutMs <= 0 || g_timeoutMs > MAX_TIMEOUT) {
        g_timeoutMs = DEFAULT_TIMEOUT;
    }

    PCWSTR result = Wh_GetStringSetting(L"block_result");
    g_blockAsCancel = !result || wcscmp(result, L"not_found") != 0;
    if (result) Wh_FreeStringSetting(result);
}

//-----------------------------------------------------------------------------
// Intent Detection
//-----------------------------------------------------------------------------
static inline bool IsIntentMsg(UINT msg, WPARAM wp) {
    switch (msg) {
        case WM_LBUTTONDOWN: case WM_LBUTTONUP:
        case WM_NCLBUTTONDOWN: case WM_NCLBUTTONUP:
        case WM_POINTERDOWN: case WM_POINTERUP:
            return true;
        case WM_KEYDOWN: case WM_KEYUP:
        case WM_SYSKEYDOWN: case WM_SYSKEYUP:
            return (wp == VK_RETURN || wp == VK_SPACE);
    }
    return false;
}

//-----------------------------------------------------------------------------
// Message Loop Hooks
//-----------------------------------------------------------------------------
static BOOL WINAPI HookPeekMessageW(LPMSG m, HWND w, UINT a, UINT b, UINT r) {
    BOOL ok = g_origPeek(m, w, a, b, r);
    if (ok && m && IsIntentMsg(m->message, m->wParam)) {
        SetLastTick(m->time ? m->time : GetTickCount());
    }
    return ok;
}

static BOOL WINAPI HookGetMessageW(LPMSG m, HWND w, UINT a, UINT b) {
    BOOL ok = g_origGet(m, w, a, b);
    if (ok > 0 && m && IsIntentMsg(m->message, m->wParam)) {
        SetLastTick(m->time ? m->time : GetTickCount());
    }
    return ok;
}

//-----------------------------------------------------------------------------
// WebAuthn Hook (the actual blocker)
//-----------------------------------------------------------------------------
static HRESULT WINAPI HookWebAuthn(HWND hwnd, LPCWSTR rpId, void* cd, void* opt, void** out) {
    DWORD now = GetTickCount();
    DWORD last = GetLastTick();

    // Check if user interacted recently (DWORD arithmetic handles wraparound)
    if (last != 0 && (now - last) < (DWORD)g_timeoutMs) {
        return g_origWebAuthn(hwnd, rpId, cd, opt, out);
    }

    // Block automatic popup
    return g_blockAsCancel ? NTE_USER_CANCELLED : NTE_NOT_FOUND;
}

//-----------------------------------------------------------------------------
// Windhawk Callbacks
//-----------------------------------------------------------------------------
BOOL Wh_ModInit() {
    LoadSettings();
    InitSharedMemory();

    // Hook message loops
    HMODULE hUser = GetModuleHandleW(L"user32.dll");
    if (hUser) {
        void* pPeek = (void*)GetProcAddress(hUser, "PeekMessageW");
        void* pGet  = (void*)GetProcAddress(hUser, "GetMessageW");

        if (pPeek) Wh_SetFunctionHook(pPeek, (void*)HookPeekMessageW, (void**)&g_origPeek);
        if (pGet)  Wh_SetFunctionHook(pGet,  (void*)HookGetMessageW,  (void**)&g_origGet);
    }

    // Hook WebAuthn (critical - fail init if this doesn't work)
    g_hWebAuthn = LoadLibraryExW(L"webauthn.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!g_hWebAuthn) {
        CleanupSharedMemory();
        return FALSE;
    }

    void* pWA = (void*)GetProcAddress(g_hWebAuthn, "WebAuthNAuthenticatorGetAssertion");
    if (!pWA || !Wh_SetFunctionHook(pWA, (void*)HookWebAuthn, (void**)&g_origWebAuthn)) {
        FreeLibrary(g_hWebAuthn);
        g_hWebAuthn = nullptr;
        CleanupSharedMemory();
        return FALSE;
    }

    return TRUE;
}

void Wh_ModUninit() {
    CleanupSharedMemory();
    if (g_hWebAuthn) {
        FreeLibrary(g_hWebAuthn);
        g_hWebAuthn = nullptr;
    }
}

void Wh_ModSettingsChanged() {
    LoadSettings();
}
