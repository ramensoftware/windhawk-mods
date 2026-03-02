// ==WindhawkMod==
// @id              passkey-popup-blocker
// @name            "Sign in with a passkey" popup blocker
// @description     Blocks automatic Windows Security "Sign in with a passkey" popup in browser. Only allows them after a recent click/tap or Enter/Space.
// @version         3.1.0
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
This mod blocks the automatic **Windows Security passkey** popup that may appear immediately when opening a login page (because a passkey exists for that site), even if you didn't choose "Sign in with passkey".

The popup is only allowed after **recent user intent**:
- a **click/tap** (mouse or touch), or
- pressing **Enter** / **Space**
within a short time window (default **800ms**).

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
* **Accurate Timing**: Uses `GetTickCount()` for consistent tick stamping across all message hooks.
* **Configurable Block Behavior**: Choose which WebAuthn error code is returned when blocking (recommended default behaves like "user cancelled").
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
Default: **800ms** (max **10000ms**) — how long after a click/keypress the popup is allowed.
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
* **No Keylogging**: The mod does **not** record what you type. It only checks for specific "intent" inputs (click/tap or Enter/Space) and updates a timestamp.
* **No Data Transmission**: The mod runs entirely locally. No data is sent to any server.
* **Safety**: Passwords, usernames, and page content are never touched or inspected.

‎‎‎‎‎‎‎‎ㅤ
## 📄 Credits
* **Author**: [mode0192](https://github.com/mode0192)
* **License**: CC BY-NC-SA 4.0 (Free for personal use, no commercial sale)

‎‎‎‎‎‎‎‎ㅤ
## 🐛 Report a Bug
If you encounter any issues or have suggestions for the mod, please submit them on the [support forum](https://tinyurl.com/report-x-bug).

‎‎‎‎‎‎‎‎ㅤ
## 📜 Changelog
**3.1.0** (Feb 18, 2026)
* **Changed**: Increased default **Interaction Timeout** from **500ms** to **800ms**.
* **Improved**: **WebAuthn hook is now initialized first** (before message loop hooks) — mod fails cleanly early if the critical hook can't be established, avoiding partial hook state.
* **Improved**: `GetLastTick()` now uses `InterlockedOr(&val, 0)` instead of `InterlockedCompareExchange` for a more correct and idiomatic atomic read on LONG-aligned memory.
* **Improved**: `SetLastTick()` local fallback now also uses `InterlockedExchange` (was a plain assignment before), making it thread-safe in all cases.
* **Fixed**: Removed `m->time` as a tick source in message hooks — it was unreliable (can be 0 or stale in some browser message loops). `GetTickCount()` is now used consistently.
* **Code**: Replaced C-style casts with `static_cast` / `reinterpret_cast` throughout for cleaner, safer code.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- timeout: 800
  $name: Interaction Timeout (ms)
  $description: "Time window after click/keypress to allow popup. Default 800ms."

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
#include <wchar.h>

#define DEFAULT_TIMEOUT 800
#define MAX_TIMEOUT     10000

// ---------------------------------------------------------------------------
// Shared state — syncs user intent across Chromium's per-tab/sandboxed
// processes. Uses a named file mapping with a NULL DACL so sandboxed
// renderer processes can access it.
// ---------------------------------------------------------------------------
#pragma pack(push, 4)
struct SharedState {
    volatile LONG lastTick; // DWORD-width; 0 == "never interacted"
};
#pragma pack(pop)

static HANDLE       g_hMap   = nullptr;
static SharedState* g_pState = nullptr;
static LONG         g_localTick = 0; // fallback if shared memory fails

static int  g_timeoutMs    = DEFAULT_TIMEOUT;
static bool g_blockAsCancel = true;

// ---------------------------------------------------------------------------
// Typedefs
// ---------------------------------------------------------------------------
typedef BOOL    (WINAPI *PeekMessageW_t)(LPMSG, HWND, UINT, UINT, UINT);
typedef BOOL    (WINAPI *GetMessageW_t) (LPMSG, HWND, UINT, UINT);
typedef HRESULT (WINAPI *WebAuthNGetAssertion_t)(HWND, LPCWSTR, void*, void*, void**);

static PeekMessageW_t        g_origPeekW  = nullptr;
static GetMessageW_t         g_origGetW   = nullptr;
static WebAuthNGetAssertion_t g_origWebAuthn = nullptr;
static HMODULE               g_hWebAuthn  = nullptr;

// ---------------------------------------------------------------------------
// Shared Memory (syncs user intent across browser processes)
// ---------------------------------------------------------------------------
static void InitSharedMemory() {
    wchar_t path[MAX_PATH];
    GetModuleFileNameW(nullptr, path, _countof(path));

    const wchar_t* exeName = wcsrchr(path, L'\\');
    exeName = exeName ? exeName + 1 : path;

    DWORD sessionId = 0;
    ProcessIdToSessionId(GetCurrentProcessId(), &sessionId);

    wchar_t mapName[MAX_PATH + 64];
    swprintf_s(mapName, _countof(mapName), L"Local\\wh_passkey_%s_%u", exeName, sessionId);

    // NULL DACL: Required for Chrome/Edge sandbox processes to access shared memory
    SECURITY_DESCRIPTOR sd;
    InitializeSecurityDescriptor(&sd, SECURITY_DESCRIPTOR_REVISION);
    SetSecurityDescriptorDacl(&sd, TRUE, nullptr, FALSE);
    SECURITY_ATTRIBUTES sa = { sizeof(sa), &sd, FALSE };

    g_hMap = CreateFileMappingW(INVALID_HANDLE_VALUE, &sa, PAGE_READWRITE,
                                0, sizeof(SharedState), mapName);
    if (!g_hMap) return;

    g_pState = static_cast<SharedState*>(
        MapViewOfFile(g_hMap, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(SharedState)));

    if (!g_pState) {
        CloseHandle(g_hMap);
        g_hMap = nullptr;
    }
}

static void CleanupSharedMemory() {
    if (g_pState) { UnmapViewOfFile(g_pState); g_pState = nullptr; }
    if (g_hMap)   { CloseHandle(g_hMap);        g_hMap  = nullptr; }
}

static inline void SetLastTick(DWORD tick) {
    if (g_pState)
        InterlockedExchange(&g_pState->lastTick, static_cast<LONG>(tick));
    else
        InterlockedExchange(&g_localTick, static_cast<LONG>(tick));
}

static inline DWORD GetLastTick() {
    // InterlockedOr with 0 is an atomic read on LONG-aligned memory
    return static_cast<DWORD>(
        g_pState ? InterlockedOr(&g_pState->lastTick, 0)
                 : InterlockedOr(&g_localTick, 0));
}

// ---------------------------------------------------------------------------
// Settings
// ---------------------------------------------------------------------------
static void LoadSettings() {
    g_timeoutMs = Wh_GetIntSetting(L"timeout");
    if (g_timeoutMs <= 0 || g_timeoutMs > MAX_TIMEOUT)
        g_timeoutMs = DEFAULT_TIMEOUT;

    PCWSTR result = Wh_GetStringSetting(L"block_result");
    g_blockAsCancel = !(result && wcscmp(result, L"not_found") == 0);
    if (result) Wh_FreeStringSetting(result);
}

// ---------------------------------------------------------------------------
// Intent Detection
// ---------------------------------------------------------------------------
static inline bool IsIntentMsg(UINT msg, WPARAM wp) {
    switch (msg) {
        // Mouse / touch
        case WM_LBUTTONDOWN:   case WM_LBUTTONUP:
        case WM_NCLBUTTONDOWN: case WM_NCLBUTTONUP:
        case WM_POINTERDOWN:   case WM_POINTERUP:
            return true;

        // Keyboard: Enter and Space only (not arbitrary keystrokes)
        case WM_KEYDOWN:   case WM_KEYUP:
        case WM_SYSKEYDOWN: case WM_SYSKEYUP:
            return (wp == VK_RETURN || wp == VK_SPACE);
    }
    return false;
}

// ---------------------------------------------------------------------------
// Message loop hooks
// ---------------------------------------------------------------------------
static BOOL WINAPI HookPeekMessageW(LPMSG m, HWND w, UINT a, UINT b, UINT r) {
    BOOL ok = g_origPeekW(m, w, a, b, r);
    if (ok && m && IsIntentMsg(m->message, m->wParam))
        SetLastTick(GetTickCount());
    return ok;
}

static BOOL WINAPI HookGetMessageW(LPMSG m, HWND w, UINT a, UINT b) {
    BOOL ok = g_origGetW(m, w, a, b);
    if (ok > 0 && m && IsIntentMsg(m->message, m->wParam))
        SetLastTick(GetTickCount());
    return ok;
}

// ---------------------------------------------------------------------------
// WebAuthn Hook (the actual blocker)
// ---------------------------------------------------------------------------
static HRESULT WINAPI HookWebAuthn(HWND hwnd, LPCWSTR rpId,
                                   void* cd, void* opt, void** out) {
    const DWORD now  = GetTickCount();
    const DWORD last = GetLastTick();

    // DWORD subtraction is wraparound-safe (handles ~49-day tick rollover)
    // last == 0 means "never interacted" — always block in that case
    if (last != 0 && (now - last) < static_cast<DWORD>(g_timeoutMs))
        return g_origWebAuthn(hwnd, rpId, cd, opt, out);

    return g_blockAsCancel ? NTE_USER_CANCELLED : NTE_NOT_FOUND;
}

// ---------------------------------------------------------------------------
// Windhawk lifecycle
// ---------------------------------------------------------------------------
BOOL Wh_ModInit() {
    LoadSettings();
    InitSharedMemory();

    // Hook WebAuthn first — this is the critical path; bail out if it fails
    g_hWebAuthn = LoadLibraryExW(L"webauthn.dll", nullptr,
                                 LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!g_hWebAuthn) {
        CleanupSharedMemory();
        return FALSE;
    }

    void* pWA = reinterpret_cast<void*>(
        GetProcAddress(g_hWebAuthn, "WebAuthNAuthenticatorGetAssertion"));
    if (!pWA || !Wh_SetFunctionHook(pWA, reinterpret_cast<void*>(HookWebAuthn),
                                    reinterpret_cast<void**>(&g_origWebAuthn))) {
        FreeLibrary(g_hWebAuthn);
        g_hWebAuthn = nullptr;
        CleanupSharedMemory();
        return FALSE;
    }

    // Hook Unicode message loops for intent detection
    HMODULE hUser = GetModuleHandleW(L"user32.dll");
    if (hUser) {
        void* pPeekW = reinterpret_cast<void*>(GetProcAddress(hUser, "PeekMessageW"));
        void* pGetW  = reinterpret_cast<void*>(GetProcAddress(hUser, "GetMessageW"));

        if (pPeekW) Wh_SetFunctionHook(pPeekW, reinterpret_cast<void*>(HookPeekMessageW),
                                        reinterpret_cast<void**>(&g_origPeekW));
        if (pGetW)  Wh_SetFunctionHook(pGetW,  reinterpret_cast<void*>(HookGetMessageW),
                                        reinterpret_cast<void**>(&g_origGetW));
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