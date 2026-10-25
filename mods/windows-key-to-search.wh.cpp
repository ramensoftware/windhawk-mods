// ==WindhawkMod==
// @id              windows-key-to-search
// @name            Windows Key to Search
// @description     Open Windows Search (Win+S) when either Windows key is tapped.
// @version         1.0
// @author          Tyler
// @github          https://github.com/snowtyler
// @include         windhawk.exe
// @include         windhawk-mod.exe
// @include         windhawk-mod-uiaccess.exe
// @compilerOptions -luser32 -lshell32 -ladvapi32 -lole32
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Windows Key to Search

Tap either Windows key to open Search instead of Start. Search opens when the
key is released. Windows-key combinations such as Win+E, Win+R, Win+Shift+S,
and Win+S continue to work normally. The generated shortcut is held for 50 ms
before release.

By default, the helper runs without requesting administrator privileges. On
Windhawk 1.7.3, administrator windows keep their normal Windows-key behavior.
On Windhawk 2.0 alpha 4 or later, the mod requests UIAccess, which also covers
administrator windows when that host is available.

The optional **Support administrator windows on Windhawk 1.7.3** setting launches
an elevated helper. It is off by default. Enabling it can show a UAC prompt at
every sign-in, and after enabling or updating the mod. The prompt runs on a
background thread, and an already-running helper prevents another prompt.
If elevation is cancelled, the helper runs normally instead. Disable and
re-enable the mod after changing this setting. On Windhawk 2.0, leave this
setting off and use its UIAccess host.

The helper uses Windhawk's [tool-mod launcher](https://github.com/ramensoftware/windhawk/wiki/Mods-as-tools:-Running-mods-in-a-dedicated-process),
with an optional background elevation path on older Windhawk versions.

Windows-key mouse gestures used by tools such as AltDrag or AltSnap are not
supported: the mod withholds the Windows key until another keyboard key is
pressed. UAC's secure desktop is outside the scope of the mod.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- supportAdministratorWindows: false
  $name: Support administrator windows on Windhawk 1.7.3
  $description: >-
    Launch an administrator helper. This can show a UAC prompt at every sign-in
    and after enabling or updating the mod. Off by default. If cancelled, normal
    windows still work. Disable and re-enable the mod after changing this option.
    Leave off on Windhawk 2.0, which provides a UIAccess host.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <shellapi.h>
#include <cwchar>

namespace {
constexpr ULONG_PTR kInputTag = 0x57485352;
constexpr UINT kOpenSearchMessage = WM_APP + 1;
// Unassigned virtual key, used to prevent a Win release from opening Start.
constexpr WORD kMenuMaskKey = 0xE8;
HANDLE g_thread;
HANDLE g_ready;
DWORD g_threadId;
bool g_hooksInstalled;
bool g_hasElevatedInputAccess;
bool g_canRemapForeground;
HWND g_cachedForeground;
HANDLE g_elevationThread;
HANDLE g_elevationThreadDone;
bool g_keys[256];
bool g_tap[2];
bool g_pending[2];
bool g_swallowUp[2];
bool g_shortcutUsed[2];
bool g_searchActive;
WORD g_searchWin;
UINT_PTR g_searchTimer;
ULONGLONG g_searchReleaseAt;
INPUT g_restoreKeys[7];
UINT g_restoreCount;
constexpr UINT kSearchHoldMs = 50;

bool IsWindowsKey(DWORD vk) {
    return vk == VK_LWIN || vk == VK_RWIN;
}

bool IsModifierKey(DWORD vk) {
    return vk == VK_SHIFT || vk == VK_CONTROL || vk == VK_MENU ||
           vk == VK_LSHIFT || vk == VK_RSHIFT ||
           vk == VK_LCONTROL || vk == VK_RCONTROL ||
           vk == VK_LMENU || vk == VK_RMENU;
}

INPUT KeyboardInput(WORD vk, DWORD flags = 0, WORD scan = 0) {
    if (vk == VK_RCONTROL || vk == VK_RMENU || IsWindowsKey(vk)) {
        flags |= KEYEVENTF_EXTENDEDKEY;
    }
    INPUT input = {};
    input.type = INPUT_KEYBOARD;
    input.ki.wVk = vk;
    input.ki.wScan = scan;
    input.ki.dwFlags = flags;
    input.ki.dwExtraInfo = kInputTag;
    return input;
}

bool Inject(INPUT* inputs, UINT count) {
    const UINT sent = SendInput(count, inputs, sizeof(INPUT));
    if (sent == count) {
        return true;
    }
    // Release any key-downs that made it into a partially accepted batch.
    INPUT releases[32] = {};
    UINT releaseCount = 0;
    for (UINT i = 0; i < sent; ++i) {
        if (!(inputs[i].ki.dwFlags & KEYEVENTF_KEYUP)) {
            if (IsWindowsKey(inputs[i].ki.wVk)) {
                releases[releaseCount++] = KeyboardInput(kMenuMaskKey);
                releases[releaseCount++] = KeyboardInput(kMenuMaskKey, KEYEVENTF_KEYUP);
            }
            releases[releaseCount] = inputs[i];
            releases[releaseCount++].ki.dwFlags |= KEYEVENTF_KEYUP;
        }
    }
    if (releaseCount) {
        SendInput(releaseCount, releases, sizeof(INPUT));
    }
    return false;
}

bool CompleteSearch(const KBDLLHOOKSTRUCT* replay = nullptr, bool up = false) {
    if (!g_searchActive) {
        return true;
    }
    if (g_searchTimer) {
        KillTimer(nullptr, g_searchTimer);
        g_searchTimer = 0;
    }
    INPUT inputs[10] = {};
    UINT count = 0;
    inputs[count++] = KeyboardInput('S', KEYEVENTF_KEYUP);
    inputs[count++] = KeyboardInput(g_searchWin, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP);
    for (UINT i = 0; i < g_restoreCount; ++i) {
        inputs[count++] = g_restoreKeys[i];
    }
    if (replay) {
        // Put new physical input AFTER our generated key releases, in one
        // batch. Passing it through would race those queued releases.
        inputs[count++] = KeyboardInput(
            static_cast<WORD>(replay->vkCode),
            (up ? KEYEVENTF_KEYUP : 0) |
                ((replay->flags & LLKHF_EXTENDED) ? KEYEVENTF_EXTENDEDKEY : 0),
            static_cast<WORD>(replay->scanCode));
    }
    g_searchActive = false;
    g_restoreCount = 0;
    return Inject(inputs, count);
}

void OpenSearch(WORD win) {
    CompleteSearch();
    // Query held keys only after the physical Win-up callback has returned.
    const WORD keys[] = {VK_LSHIFT, VK_RSHIFT, VK_LCONTROL, VK_RCONTROL,
                         VK_LMENU, VK_RMENU, 'S'};
    INPUT inputs[9] = {};
    UINT count = 0;
    g_restoreCount = 0;
    for (WORD vk : keys) {
        if (GetAsyncKeyState(vk) & 0x8000) {
            inputs[count++] = KeyboardInput(vk, KEYEVENTF_KEYUP);
            g_restoreKeys[g_restoreCount++] = KeyboardInput(vk);
        }
    }
    inputs[count++] = KeyboardInput(win, KEYEVENTF_EXTENDEDKEY);
    inputs[count++] = KeyboardInput('S');
    g_searchWin = win;
    g_searchActive = true;
    const bool accepted = Inject(inputs, count);
    if (!accepted) {
        CompleteSearch();
        return;
    }
    // The working physical shortcut has a real hold between down and up.
    // A timer preserves that ordering without blocking the hook's thread.
    g_searchReleaseAt = GetTickCount64() + kSearchHoldMs;
    g_searchTimer = SetTimer(nullptr, 0, kSearchHoldMs, nullptr);
    if (!g_searchTimer) {
        CompleteSearch();
    }
}

LRESULT CALLBACK KeyboardProc(int code, WPARAM message, LPARAM param) {
    if (code != HC_ACTION) {
        return CallNextHookEx(nullptr, code, message, param);
    }
    const auto& key = *reinterpret_cast<const KBDLLHOOKSTRUCT*>(param);
    if (key.dwExtraInfo == kInputTag || key.vkCode >= 256) {
        return CallNextHookEx(nullptr, code, message, param);
    }
    const bool down = message == WM_KEYDOWN || message == WM_SYSKEYDOWN;
    const bool up = message == WM_KEYUP || message == WM_SYSKEYUP;
    if (!down && !up) {
        return CallNextHookEx(nullptr, code, message, param);
    }
    if (!g_hasElevatedInputAccess &&
        (!g_canRemapForeground || GetForegroundWindow() != g_cachedForeground)) {
        // Inaccessible/elevated foreground windows retain their original input.
        ZeroMemory(g_keys, sizeof(g_keys));
        ZeroMemory(g_pending, sizeof(g_pending));
        ZeroMemory(g_tap, sizeof(g_tap));
        ZeroMemory(g_swallowUp, sizeof(g_swallowUp));
        ZeroMemory(g_shortcutUsed, sizeof(g_shortcutUsed));
        return CallNextHookEx(nullptr, code, message, param);
    }
    if (g_searchActive) {
        if (!IsWindowsKey(key.vkCode)) {
            g_keys[key.vkCode] = down;
            if (CompleteSearch(&key, up)) {
                return 1;
            }
        } else {
            // Release our synthetic Win before tracking a fresh physical tap.
            CompleteSearch();
        }
    }
    const bool win = IsWindowsKey(key.vkCode);
    const int index = key.vkCode == VK_LWIN ? 0 : 1;
    if (win && up && !g_keys[key.vkCode]) {
        // A down outside our desktop/access scope must keep its original up.
        return CallNextHookEx(nullptr, code, message, param);
    }
    if (win && down && g_keys[key.vkCode] && !g_swallowUp[index] &&
        !(GetAsyncKeyState(key.vkCode) & 0x8000)) {
        // This is the state BEFORE the current down. A forwarded key which is
        // now up lost its release (for example, to Win+L's lock desktop).
        g_keys[key.vkCode] = false;
        g_pending[index] = g_tap[index] = g_shortcutUsed[index] = false;
    }
    if (down && win && g_keys[key.vkCode]) {
        // Discard repeat events while the initial down is withheld.
        if (g_swallowUp[index]) {
            return 1;
        }
    } else if (down) {
        const bool repeat = g_keys[key.vkCode];
        g_keys[key.vkCode] = true;
        // Repeats of a key held before Win are not a new Win shortcut.
        if (!win && repeat && (g_pending[0] || g_pending[1])) {
            return CallNextHookEx(nullptr, code, message, param);
        }
        if (win && !repeat) {
            g_shortcutUsed[index] = false;
        } else if (!win && !IsModifierKey(key.vkCode)) {
            for (int i = 0; i < 2; ++i) {
                if (g_keys[i == 0 ? VK_LWIN : VK_RWIN]) {
                    g_shortcutUsed[i] = true;
                }
            }
        }
        if (g_pending[0] || g_pending[1]) {
            // Send the withheld Win-down AND this key together. Passing this
            // physical event through would race the queued Win-down.
            INPUT inputs[3] = {};
            UINT count = 0;
            for (int i = 0; i < 2; ++i) {
                if (g_pending[i]) {
                    inputs[count++] = KeyboardInput(i == 0 ? VK_LWIN : VK_RWIN,
                                                     KEYEVENTF_EXTENDEDKEY);
                }
            }
            inputs[count++] = KeyboardInput(
                static_cast<WORD>(key.vkCode),
                (key.flags & LLKHF_EXTENDED) ? KEYEVENTF_EXTENDEDKEY : 0,
                static_cast<WORD>(key.scanCode));
            const bool sent = Inject(inputs, count);
            for (int i = 0; i < 2; ++i) {
                if (g_pending[i]) {
                    g_swallowUp[i] = !sent;
                }
                g_pending[i] = g_tap[i] = false;
            }
            if (win) {
                g_swallowUp[index] = !sent;
            }
            return 1;
        }
        if (win) {
            // Always withhold Win, even if another key was already held.
            // Otherwise releasing that other key first can open Start.
            g_pending[index] = g_swallowUp[index] = true;
            g_tap[index] = true;
            return 1;
        }
        g_tap[0] = g_tap[1] = false;
    } else if (up) {
        g_keys[key.vkCode] = false;
        if (win) {
            const bool tap = g_pending[index] && g_tap[index];
            const bool swallow = g_swallowUp[index];
            const bool shortcutUsed = g_shortcutUsed[index];
            g_pending[index] = g_tap[index] = g_swallowUp[index] = false;
            g_shortcutUsed[index] = false;
            if (tap) {
                // Post to our own thread so input is sent after this hook
                // returns, rather than during the physical Win-up callback.
                if (!PostThreadMessageW(g_threadId, kOpenSearchMessage,
                                        key.vkCode, 0)) {
                    Wh_Log(L"Could not queue Search (error %lu)", GetLastError());
                }
            }
            if (swallow) {
                return 1;
            }
            if (shortcutUsed) {
                // Win+V and other flyouts must receive the real release, with
                // no fresh Win-down or mask keystroke to dismiss their UI.
                return CallNextHookEx(nullptr, code, message, param);
            }
            // Modifier-only combinations (e.g. Win+Shift) can still be
            // interpreted as a bare Win tap. Only those need a masked release.
            INPUT release[] = {
                KeyboardInput(static_cast<WORD>(key.vkCode), KEYEVENTF_EXTENDEDKEY),
                KeyboardInput(kMenuMaskKey),
                KeyboardInput(kMenuMaskKey, KEYEVENTF_KEYUP),
                KeyboardInput(static_cast<WORD>(key.vkCode),
                              KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP),
            };
            if (Inject(release, ARRAYSIZE(release))) {
                return 1;
            }
        }
    }
    return CallNextHookEx(nullptr, code, message, param);
}

void ResetKeyTracking() {
    ZeroMemory(g_keys, sizeof(g_keys));
    ZeroMemory(g_pending, sizeof(g_pending));
    ZeroMemory(g_tap, sizeof(g_tap));
    ZeroMemory(g_swallowUp, sizeof(g_swallowUp));
    ZeroMemory(g_shortcutUsed, sizeof(g_shortcutUsed));
}

void RefreshForegroundAccess() {
    g_cachedForeground = GetForegroundWindow();
    g_canRemapForeground = g_hasElevatedInputAccess;
    if (g_hasElevatedInputAccess) {
        return;
    }
    DWORD pid = 0;
    GetWindowThreadProcessId(g_cachedForeground, &pid);
    HANDLE process = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    HANDLE token = nullptr;
    if (process && OpenProcessToken(process, TOKEN_QUERY, &token)) {
        TOKEN_ELEVATION elevation = {};
        DWORD uiAccess = 0;
        DWORD length = 0;
        const BOOL readElevation = GetTokenInformation(
            token, TokenElevation, &elevation, sizeof(elevation), &length);
        const BOOL readUiAccess = GetTokenInformation(
            token, TokenUIAccess, &uiAccess, sizeof(uiAccess), &length);
        g_canRemapForeground = readElevation && readUiAccess &&
                              !elevation.TokenIsElevated && !uiAccess;
        CloseHandle(token);
    }
    if (process) {
        CloseHandle(process);
    }
    if (!g_canRemapForeground) {
        ResetKeyTracking();
    }
}

void CALLBACK InputContextChanged(HWINEVENTHOOK, DWORD event, HWND, LONG,
                                  LONG, DWORD, DWORD) {
    if (event == EVENT_SYSTEM_DESKTOPSWITCH) {
        // Releases on the lock screen/UAC desktop are invisible to our hook.
        ResetKeyTracking();
    }
    RefreshForegroundAccess();
}

DWORD WINAPI HookThread(void*) {
    // Create the message queue before publishing readiness to ModInit.
    MSG message;
    PeekMessageW(&message, nullptr, 0, 0, PM_NOREMOVE);
    for (int vk = VK_BACK; vk < 256; ++vk) {
        // Hooks report left/right modifiers, never these aggregate values.
        // Seeding aggregates would leave stale "held" keys after release.
        if (vk != VK_SHIFT && vk != VK_CONTROL && vk != VK_MENU) {
            g_keys[vk] = (GetAsyncKeyState(vk) & 0x8000) != 0;
        }
    }
    HMODULE module = nullptr;
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                          GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                      reinterpret_cast<LPCWSTR>(&KeyboardProc), &module);
    HWINEVENTHOOK foregroundHook = SetWinEventHook(
        EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND, nullptr,
        InputContextChanged, 0, 0, WINEVENT_OUTOFCONTEXT);
    HWINEVENTHOOK desktopHook = SetWinEventHook(
        EVENT_SYSTEM_DESKTOPSWITCH, EVENT_SYSTEM_DESKTOPSWITCH, nullptr,
        InputContextChanged, 0, 0, WINEVENT_OUTOFCONTEXT);
    RefreshForegroundAccess();
    HHOOK keyboard = SetWindowsHookExW(WH_KEYBOARD_LL, KeyboardProc, module, 0);
    g_hooksInstalled = keyboard && foregroundHook && desktopHook;
    SetEvent(g_ready);

    if (g_hooksInstalled) {
        while (GetMessageW(&message, nullptr, 0, 0) > 0) {
            if (message.message == WM_TIMER && g_searchTimer &&
                message.wParam == g_searchTimer) {
                // Timer IDs can be reused while an old WM_TIMER is queued.
                if (GetTickCount64() >= g_searchReleaseAt) {
                    CompleteSearch();
                }
                continue;
            }
            if (message.message == kOpenSearchMessage) {
                RefreshForegroundAccess();
                if (!g_canRemapForeground) {
                    continue;
                }
                OpenSearch(static_cast<WORD>(message.wParam));
                continue;
            }
            TranslateMessage(&message);
            DispatchMessageW(&message);
        }
    }
    CompleteSearch();
    if (keyboard) {
        UnhookWindowsHookEx(keyboard);
    }
    if (foregroundHook) {
        UnhookWinEvent(foregroundHook);
    }
    if (desktopHook) {
        UnhookWinEvent(desktopHook);
    }
    // A forwarded Windows key may still be held when the mod is disabled.
    INPUT releases[6] = {};
    UINT count = 0;
    for (int i = 0; i < 2; ++i) {
        const WORD vk = i == 0 ? VK_LWIN : VK_RWIN;
        if (g_keys[vk] && !g_swallowUp[i]) {
            releases[count++] = KeyboardInput(kMenuMaskKey);
            releases[count++] = KeyboardInput(kMenuMaskKey, KEYEVENTF_KEYUP);
            releases[count++] = KeyboardInput(vk, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP);
        }
    }
    if (count) {
        Inject(releases, count);
    }
    return 0;
}

void Cleanup() {
    if (g_thread) {
        PostThreadMessageW(g_threadId, WM_QUIT, 0, 0);
        WaitForSingleObject(g_thread, INFINITE);
        CloseHandle(g_thread);
        g_thread = nullptr;
    }
    if (g_ready) {
        CloseHandle(g_ready);
        g_ready = nullptr;
    }
}
}  // namespace

bool HasElevatedInputAccess() {
    HANDLE token = nullptr;
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &token)) {
        return false;
    }
    TOKEN_ELEVATION elevation = {};
    DWORD uiAccess = 0;
    DWORD length = 0;
    const BOOL readElevation = GetTokenInformation(
        token, TokenElevation, &elevation, sizeof(elevation), &length);
    const BOOL readUiAccess = GetTokenInformation(
        token, TokenUIAccess, &uiAccess, sizeof(uiAccess), &length);
    CloseHandle(token);
    Wh_Log(L"Keyboard helper PID=%lu: elevated=%lu, UIAccess=%lu",
           GetCurrentProcessId(), elevation.TokenIsElevated, uiAccess);
    return (readElevation && elevation.TokenIsElevated) ||
           (readUiAccess && uiAccess);
}

BOOL WhTool_ModInit() {
    g_hasElevatedInputAccess = HasElevatedInputAccess();
    g_ready = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_ready) {
        Cleanup();
        return FALSE;
    }
    g_thread = CreateThread(nullptr, 0, HookThread, nullptr, 0, &g_threadId);
    if (!g_thread) {
        Cleanup();
        return FALSE;
    }
    WaitForSingleObject(g_ready, INFINITE);
    if (!g_hooksInstalled) {
        Cleanup();
        return FALSE;
    }
    return TRUE;
}

void WhTool_ModUninit() {
    Cleanup();
}

void WhTool_ModSettingsChanged() {
}

////////////////////////////////////////////////////////////////////////////////
// Windhawk tool-mod launcher. The default CreateProcessInternalW path follows
// the wiki snippet. The optional elevation path runs on a background thread.
// For mods which don't need to inject to other
// processes or hook other functions. Context:
// https://github.com/ramensoftware/windhawk/wiki/Mods-as-tools:-Running-mods-in-a-dedicated-process
//
// The mod will load and run in a dedicated windhawk.exe process.
//
// Paste the code below as part of the mod code, and use these callbacks:
// * WhTool_ModInit
// * WhTool_ModSettingsChanged
// * WhTool_ModUninit
//
// Currently, other callbacks are not supported.

bool g_isToolModProcessLauncher;
HANDLE g_toolModProcessMutex;

void WINAPI EntryPoint_Hook() {
    Wh_Log(L">");
    ExitThread(0);
}

BOOL Wh_ModInit() {
    DWORD sessionId;
    if (ProcessIdToSessionId(GetCurrentProcessId(), &sessionId) &&
        sessionId == 0) {
        return FALSE;
    }
    bool isExcluded = false;
    bool isToolModProcess = false;
    bool isCurrentToolModProcess = false;
    int argc;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLine(), &argc);
    if (!argv) {
        Wh_Log(L"CommandLineToArgvW failed");
        return FALSE;
    }

    for (int i = 1; i < argc; i++) {
        if (wcscmp(argv[i], L"-service") == 0 ||
            wcscmp(argv[i], L"-service-start") == 0 ||
            wcscmp(argv[i], L"-service-stop") == 0) {
            isExcluded = true;
            break;
        }
    }

    for (int i = 1; i < argc - 1; i++) {
        if (wcscmp(argv[i], L"-tool-mod") == 0) {
            isToolModProcess = true;
            if (wcscmp(argv[i + 1], WH_MOD_ID) == 0) {
                isCurrentToolModProcess = true;
            }
            break;
        }
    }

    LocalFree(argv);
    if (isExcluded) {
        return FALSE;
    }

    if (isCurrentToolModProcess) {
        g_toolModProcessMutex =
            CreateMutex(nullptr, TRUE, L"windhawk-tool-mod_" WH_MOD_ID);
        if (!g_toolModProcessMutex) {
            Wh_Log(L"CreateMutex failed");
            ExitProcess(1);
        }

        if (GetLastError() == ERROR_ALREADY_EXISTS) {
            Wh_Log(L"Tool mod already running (%s)", WH_MOD_ID);
            ExitProcess(1);
        }

        if (!WhTool_ModInit()) {
            ExitProcess(1);
        }

        IMAGE_DOS_HEADER* dosHeader =
            (IMAGE_DOS_HEADER*)GetModuleHandle(nullptr);
        IMAGE_NT_HEADERS* ntHeaders =
            (IMAGE_NT_HEADERS*)((BYTE*)dosHeader + dosHeader->e_lfanew);

        DWORD entryPointRVA = ntHeaders->OptionalHeader.AddressOfEntryPoint;
        void* entryPoint = (BYTE*)dosHeader + entryPointRVA;

        Wh_SetFunctionHook(entryPoint, (void*)EntryPoint_Hook, nullptr);
        return TRUE;
    }

    if (isToolModProcess) {
        return FALSE;
    }

    g_isToolModProcessLauncher = true;
    return TRUE;
}

void LaunchStandardHelper() {
    if (!g_isToolModProcessLauncher) {
        return;
    }
    WCHAR currentProcessPath[MAX_PATH];
    switch (GetModuleFileName(nullptr, currentProcessPath,
                              ARRAYSIZE(currentProcessPath))) {
        case 0:
        case ARRAYSIZE(currentProcessPath):
            Wh_Log(L"GetModuleFileName failed");
            return;
    }
    WCHAR commandLine[MAX_PATH + 2 +
                      (sizeof(L" -tool-mod \"" WH_MOD_ID "\"") / sizeof(WCHAR)) - 1];
    swprintf_s(commandLine, L"\"%s\" -tool-mod \"%s\"", currentProcessPath,
               WH_MOD_ID);
    HMODULE kernelModule = GetModuleHandle(L"kernelbase.dll");
    if (!kernelModule) {
        kernelModule = GetModuleHandle(L"kernel32.dll");
        if (!kernelModule) {
            Wh_Log(L"No kernelbase.dll/kernel32.dll");
            return;
        }
    }
    using CreateProcessInternalW_t = BOOL(WINAPI*)(
        HANDLE hUserToken, LPCWSTR lpApplicationName, LPWSTR lpCommandLine,
        LPSECURITY_ATTRIBUTES lpProcessAttributes,
        LPSECURITY_ATTRIBUTES lpThreadAttributes, WINBOOL bInheritHandles,
        DWORD dwCreationFlags, LPVOID lpEnvironment, LPCWSTR lpCurrentDirectory,
        LPSTARTUPINFOW lpStartupInfo,
        LPPROCESS_INFORMATION lpProcessInformation,
        PHANDLE hRestrictedUserToken);
    CreateProcessInternalW_t pCreateProcessInternalW =
        (CreateProcessInternalW_t)GetProcAddress(kernelModule, "CreateProcessInternalW");
    if (!pCreateProcessInternalW) {
        Wh_Log(L"No CreateProcessInternalW");
        return;
    }
    STARTUPINFO si{
        .cb = sizeof(STARTUPINFO),
        .dwFlags = STARTF_FORCEOFFFEEDBACK,
    };
    PROCESS_INFORMATION pi;
    if (!pCreateProcessInternalW(nullptr, currentProcessPath, commandLine,
                                 nullptr, nullptr, FALSE, NORMAL_PRIORITY_CLASS,
                                 nullptr, nullptr, &si, &pi, nullptr)) {
        Wh_Log(L"CreateProcess failed");
        return;
    }
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}

bool IsToolHelperRunning() {
    HANDLE helper = OpenMutexW(SYNCHRONIZE, FALSE, L"windhawk-tool-mod_" WH_MOD_ID);
    if (helper) {
        CloseHandle(helper);
        return true;
    }
    // A higher-integrity helper may deny access; it still exists.
    return GetLastError() == ERROR_ACCESS_DENIED;
}

DWORD WINAPI OptionalElevationThread(void*) {
    // Hold a separate reservation through the prompt and helper startup. This
    // prevents two launcher instances from showing concurrent UAC prompts.
    HANDLE reservation = CreateMutexW(
        nullptr, FALSE, L"Local\\Windhawk.Search.Launch_" WH_MOD_ID);
    if (!reservation) {
        Wh_Log(L"Could not reserve elevated helper launch");
        return 0;
    }
    if (GetLastError() == ERROR_ALREADY_EXISTS || IsToolHelperRunning() ||
        WaitForSingleObject(g_elevationThreadDone, 0) == WAIT_OBJECT_0) {
        CloseHandle(reservation);
        return 0;
    }
    WCHAR path[MAX_PATH];
    DWORD pathLength = GetModuleFileNameW(nullptr, path, ARRAYSIZE(path));
    if (!pathLength || pathLength == ARRAYSIZE(path)) {
        CloseHandle(reservation);
        return 0;
    }
    WCHAR parameters[sizeof(L"-tool-mod \"" WH_MOD_ID "\"") / sizeof(WCHAR)];
    swprintf_s(parameters, L"-tool-mod \"%s\"", WH_MOD_ID);
    SHELLEXECUTEINFOW launch = {};
    launch.cbSize = sizeof(launch);
    launch.fMask = SEE_MASK_NOCLOSEPROCESS | SEE_MASK_NOASYNC | SEE_MASK_FLAG_NO_UI;
    launch.lpVerb = L"runas";
    launch.lpFile = path;
    launch.lpParameters = parameters;
    launch.nShow = SW_HIDE;
    const HRESULT com = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    const BOOL launched = ShellExecuteExW(&launch);
    const DWORD error = launched ? ERROR_SUCCESS : GetLastError();
    if (SUCCEEDED(com)) {
        CoUninitialize();
    }
    if (!launched && WaitForSingleObject(g_elevationThreadDone, 0) != WAIT_OBJECT_0) {
        Wh_Log(L"Elevation cancelled or unavailable (error %lu); using normal helper", error);
        LaunchStandardHelper();
    }
    // Process creation can finish before Windhawk loads the mod into it. Keep
    // the reservation until the helper owns its mutex (or startup times out).
    const ULONGLONG deadline = GetTickCount64() + 10000;
    while (GetTickCount64() < deadline && !IsToolHelperRunning() &&
           WaitForSingleObject(g_elevationThreadDone, 50) == WAIT_TIMEOUT) {
        if (launched && launch.hProcess &&
            WaitForSingleObject(launch.hProcess, 0) == WAIT_OBJECT_0) {
            break;
        }
    }
    if (launch.hProcess) {
        CloseHandle(launch.hProcess);
    }
    CloseHandle(reservation);
    return 0;
}

void Wh_ModAfterInit() {
    if (!g_isToolModProcessLauncher) {
        return;
    }
    if (!Wh_GetIntSetting(L"supportAdministratorWindows")) {
        LaunchStandardHelper();
        return;
    }
    if (IsToolHelperRunning()) {
        return;
    }
    g_elevationThreadDone = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (g_elevationThreadDone) {
        g_elevationThread = CreateThread(nullptr, 0, OptionalElevationThread,
                                         nullptr, 0, nullptr);
    }
    if (!g_elevationThread) {
        Wh_Log(L"Could not start elevation worker; using normal helper");
        if (g_elevationThreadDone) {
            CloseHandle(g_elevationThreadDone);
            g_elevationThreadDone = nullptr;
        }
        LaunchStandardHelper();
    }
}

void Wh_ModSettingsChanged() {
    if (g_isToolModProcessLauncher) {
        return;
    }

    WhTool_ModSettingsChanged();
}

void Wh_ModUninit() {
    if (g_isToolModProcessLauncher) {
        if (g_elevationThreadDone) {
            SetEvent(g_elevationThreadDone);
        }
        if (g_elevationThread) {
            // The DLL must remain mapped until its worker has returned.
            WaitForSingleObject(g_elevationThread, INFINITE);
            CloseHandle(g_elevationThread);
            g_elevationThread = nullptr;
        }
        if (g_elevationThreadDone) {
            CloseHandle(g_elevationThreadDone);
            g_elevationThreadDone = nullptr;
        }
        return;
    }
    WhTool_ModUninit();
    ExitProcess(0);
}
