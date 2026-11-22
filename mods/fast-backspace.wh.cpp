// ==WindhawkMod==
// @id              fast-backspace
// @name            Fast Backspace
// @description     Delete faster while holding Backspace, with a configurable repeat multiplier
// @version         1.0
// @author          Enrique Bos
// @github          https://github.com/enriquebos
// @include         windhawk.exe
// @compilerOptions -lshell32
// @license         MIT
// ==/WindhawkMod==

// clang-format off
// ==WindhawkModReadme==
/*
# Fast Backspace

Increase the speed of held Backspace without changing the repeat speed of other
keys. The first press and the Windows keyboard repeat delay stay unchanged.

## Settings

Set **Backspace speed multiplier** to an integer from **1** to **10**. The default is
**2**; **1** keeps normal Windows speed. Changes take effect immediately. Values
outside this range are clamped. Start with a low value to avoid deleting too much.

## Behavior

- Single presses stay normal. Acceleration starts when Windows repeats a held
  physical Backspace key.
- Ctrl, Alt, Shift, and Windows-key combinations are not accelerated. Pressing
  another key during a hold cancels acceleration until Backspace is released.
- A repeat observed in a different foreground window cancels acceleration for
  that hold.
- Repeats arrive in small batches, which can feel bursty at high multipliers.
  Actual deletion speed depends on the application.

## How it works

A dedicated Windhawk tool process uses a global keyboard hook to observe repeats
and queues bounded extra key-downs through `SendInput` outside the callback. Extra
events retain the source scan code; the physical key-up releases Backspace, and
stale input is discarded after release, cancellation, or shutdown.

## Compatibility and limitations

Designed for Windows 10 and 11 and applications that accept synthetic keyboard
input. The mod runs independently of Explorer and does not inject into each app.

- Elevated apps, games, raw-input apps, remappers, accessibility tools, and remote
  sessions may reject or handle injected input differently. The mod does not
  bypass Windows integrity restrictions or operate on the secure desktop.
- Backspace is accelerated wherever it is handled, including password fields
  and non-text actions such as navigation. The global hook cannot reliably
  identify text fields. Disable the mod where this behavior is undesirable.
- Foreground-window checks cannot detect every change of focus inside one window
  or an application-specific mode change. Release Backspace before changing focus.
- Other hooks can block injected input. Windows can also silently remove a
  low-level hook if its thread times out; disable and re-enable the mod to retry.
- Disabling the mod cancels pending work and removes its hook. Input already
  delivered to an application cannot be undone. Physical input is passed through.

## Troubleshooting

If Backspace is not faster, check that the multiplier exceeds 1, release and hold
the key again without modifiers, and try an ordinary, non-elevated text editor.
Disable and re-enable the mod if necessary. Windhawk logs initialization and
input-insertion failures when logging is enabled. Failed batches are not retried.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- RepeatMultiplier: 2
  $name: Backspace speed multiplier
  $description: >-
    Integer from 1 (normal speed) to 10. Default: 2. Only physical Backspace
    repeats are accelerated; the first press and repeat delay stay unchanged.
*/
// ==/WindhawkModSettings==
// clang-format on

#include <windows.h>

#include <shellapi.h>

#include <algorithm>
#include <atomic>

namespace {

constexpr UINT kRepeatMessage = WM_APP + 1;
constexpr int kMaxMultiplier = 10;
constexpr ULONG_PTR kInputTag = 0xFA570000;
constexpr ULONG_PTR kGenerationMask = 0xFFFF;
constexpr ULONG_PTR kInputTagMask = ~kGenerationMask;

std::atomic<int> g_multiplier{2};
std::atomic<bool> g_stopping{true};
std::atomic<bool> g_hookInstalled{false};

HANDLE g_thread = nullptr;
HANDLE g_ready = nullptr;
DWORD g_threadId = 0;

// Hook-thread state. Generations invalidate queued work and injected input.
bool g_backspaceDown = false;
bool g_accelerateHold = false;
bool g_repeatPending = false;
HWND g_holdWindow = nullptr;
ULONG_PTR g_generation = 0;

void LoadSettings() {
    int value =
        std::clamp(Wh_GetIntSetting(L"RepeatMultiplier"), 1, kMaxMultiplier);
    g_multiplier.store(value);
    Wh_Log(L"Backspace multiplier: %d", value);
}

bool ModifiersPressed() {
    return (GetAsyncKeyState(VK_CONTROL) & 0x8000) ||
           (GetAsyncKeyState(VK_MENU) & 0x8000) ||
           (GetAsyncKeyState(VK_SHIFT) & 0x8000) ||
           (GetAsyncKeyState(VK_LWIN) & 0x8000) ||
           (GetAsyncKeyState(VK_RWIN) & 0x8000);
}

void CancelAcceleration() {
    g_accelerateHold = false;
    g_repeatPending = false;
    ++g_generation;
}

bool CanAccelerate() {
    return !g_stopping.load() && g_backspaceDown && g_accelerateHold &&
           g_holdWindow && GetForegroundWindow() == g_holdWindow &&
           (GetAsyncKeyState(VK_BACK) & 0x8000) && !ModifiersPressed();
}

LRESULT CALLBACK KeyboardHook(int code, WPARAM wParam, LPARAM lParam) {
    if (code != HC_ACTION) {
        return CallNextHookEx(nullptr, code, wParam, lParam);
    }

    const auto* key = reinterpret_cast<const KBDLLHOOKSTRUCT*>(lParam);
    bool injected = (key->flags & LLKHF_INJECTED) != 0;
    if (injected && key->vkCode == VK_BACK &&
        (key->dwExtraInfo & kInputTagMask) == kInputTag) {
        // Recheck at delivery: injected input can outlive its physical hold.
        if ((key->dwExtraInfo & kGenerationMask) !=
                (g_generation & kGenerationMask) ||
            !CanAccelerate()) {
            return 1;
        }
        return CallNextHookEx(nullptr, code, wParam, lParam);
    }

    if (g_stopping.load()) {
        return CallNextHookEx(nullptr, code, wParam, lParam);
    }

    bool keyDown = wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN;
    bool keyUp = wParam == WM_KEYUP || wParam == WM_SYSKEYUP;

    if (key->vkCode != VK_BACK) {
        // Also observe injected modifiers/shortcuts used by remapping tools.
        if (keyDown && g_backspaceDown) {
            CancelAcceleration();
        }
    } else if (!injected) {
        if (keyUp) {
            g_backspaceDown = false;
            g_holdWindow = nullptr;
            CancelAcceleration();
        } else if (keyDown) {
            // The callback precedes the async-state update. An up state here
            // also recovers from a release missed on another input desktop.
            if (!g_backspaceDown || !(GetAsyncKeyState(VK_BACK) & 0x8000)) {
                g_backspaceDown = true;
                g_holdWindow = GetForegroundWindow();
                ++g_generation;
                g_repeatPending = false;
                g_accelerateHold = g_holdWindow && !ModifiersPressed() &&
                                   !(key->flags & LLKHF_ALTDOWN);
            } else if (!CanAccelerate() || (key->flags & LLKHF_ALTDOWN)) {
                CancelAcceleration();
            } else if (g_multiplier.load() > 1 && !g_repeatPending) {
                g_repeatPending = true;
                DWORD flags =
                    (key->flags & LLKHF_EXTENDED) ? KEYEVENTF_EXTENDEDKEY : 0;
                LPARAM scanInfo = MAKELPARAM(key->scanCode, flags);
                if (!PostThreadMessageW(GetCurrentThreadId(), kRepeatMessage,
                                        g_generation, scanInfo)) {
                    g_repeatPending = false;
                }
            }
        }
    }

    // Never suppress physical input, even if queuing or injection fails.
    return CallNextHookEx(nullptr, code, wParam, lParam);
}

void ProcessRepeat(ULONG_PTR generation, LPARAM scanInfo) {
    if (generation != g_generation || !g_repeatPending) {
        return;
    }
    g_repeatPending = false;

    if (!CanAccelerate()) {
        CancelAcceleration();
        return;
    }

    int extra = g_multiplier.load() - 1;
    if (extra <= 0) {
        return;
    }

    INPUT inputs[kMaxMultiplier - 1]{};
    for (int i = 0; i < extra; ++i) {
        inputs[i].type = INPUT_KEYBOARD;
        inputs[i].ki.wVk = VK_BACK;
        inputs[i].ki.wScan = LOWORD(scanInfo);
        inputs[i].ki.dwFlags = HIWORD(scanInfo) & KEYEVENTF_EXTENDEDKEY;
        inputs[i].ki.dwExtraInfo = kInputTag | (generation & kGenerationMask);
    }

    // Key-downs extend the physical hold. Its physical key-up releases the key.
    UINT count = static_cast<UINT>(extra);
    UINT sent = SendInput(count, inputs, sizeof(INPUT));
    if (sent != count) {
        DWORD error = GetLastError();
        CancelAcceleration();
        Wh_Log(
            L"SendInput inserted %u of %u events (error %lu); "
            L"acceleration cancelled until release",
            sent, count, error);
    }
}

DWORD WINAPI HookThread(void*) {
    g_backspaceDown = false;
    g_holdWindow = nullptr;
    CancelAcceleration();

    MSG message;
    PeekMessageW(&message, nullptr, WM_USER, WM_USER, PM_NOREMOVE);
    if (g_stopping.load()) {
        SetEvent(g_ready);
        return 0;
    }

    HMODULE module = nullptr;
    if (!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                                GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                            reinterpret_cast<LPCWSTR>(&KeyboardHook),
                            &module)) {
        Wh_Log(L"GetModuleHandleEx failed: %lu", GetLastError());
        SetEvent(g_ready);
        return 1;
    }

    HHOOK hook = SetWindowsHookExW(WH_KEYBOARD_LL, KeyboardHook, module, 0);
    if (!hook) {
        Wh_Log(L"SetWindowsHookEx failed: %lu", GetLastError());
        SetEvent(g_ready);
        return 1;
    }

    g_hookInstalled.store(true);
    SetEvent(g_ready);
    Wh_Log(L"Global keyboard hook installed");

    // Also check the flag before GetMessage: startup cancellation can precede
    // queue creation, in which case Cleanup's WM_QUIT post cannot succeed.
    BOOL result = 0;
    while (!g_stopping.load() &&
           (result = GetMessageW(&message, nullptr, 0, 0)) > 0) {
        if (message.message == kRepeatMessage) {
            ProcessRepeat(message.wParam, message.lParam);
        } else {
            TranslateMessage(&message);
            DispatchMessageW(&message);
        }
    }
    if (result == -1) {
        Wh_Log(L"GetMessage failed: %lu", GetLastError());
    }

    g_stopping.store(true);
    CancelAcceleration();
    UnhookWindowsHookEx(hook);
    g_hookInstalled.store(false);
    return 0;
}

void Cleanup() {
    g_stopping.store(true);
    if (g_thread) {
        if (!PostThreadMessageW(g_threadId, WM_QUIT, 0, 0)) {
            Wh_Log(L"PostThreadMessage(WM_QUIT) failed: %lu", GetLastError());
        }
        // The DLL must remain loaded until the callback and thread have exited.
        WaitForSingleObject(g_thread, INFINITE);
        CloseHandle(g_thread);
        g_thread = nullptr;
        g_threadId = 0;
    }
    if (g_ready) {
        CloseHandle(g_ready);
        g_ready = nullptr;
    }
}

}  // namespace

BOOL WhTool_ModInit() {
    g_stopping.store(true);
    g_hookInstalled.store(false);
    LoadSettings();

    g_ready = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_ready) {
        Wh_Log(L"Failed to create ready event: %lu", GetLastError());
        Cleanup();
        return FALSE;
    }

    g_stopping.store(false);
    g_thread = CreateThread(nullptr, 0, HookThread, nullptr, 0, &g_threadId);
    if (!g_thread) {
        Wh_Log(L"CreateThread failed: %lu", GetLastError());
        Cleanup();
        return FALSE;
    }

    HANDLE handles[] = {g_ready, g_thread};
    DWORD result =
        WaitForMultipleObjects(ARRAYSIZE(handles), handles, FALSE, 5000);
    if (result != WAIT_OBJECT_0 || !g_hookInstalled.load()) {
        Wh_Log(L"Keyboard hook startup failed (wait result %lu)", result);
        Cleanup();
        return FALSE;
    }

    return TRUE;
}

void WhTool_ModSettingsChanged() {
    LoadSettings();
}

void WhTool_ModUninit() {
    Cleanup();
}

#ifdef __clang__
#pragma clang diagnostic push
// Keep the official launcher's zero-initialized STARTUPINFO snippet unchanged.
#pragma clang diagnostic ignored "-Wmissing-designated-field-initializers"
#endif
// clang-format off
////////////////////////////////////////////////////////////////////////////////
// Windhawk tool mod implementation for mods which don't need to inject to other
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

void Wh_ModAfterInit() {
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

    WCHAR
    commandLine[MAX_PATH + 2 +
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
        (CreateProcessInternalW_t)GetProcAddress(kernelModule,
                                                 "CreateProcessInternalW");
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

void Wh_ModSettingsChanged() {
    if (g_isToolModProcessLauncher) {
        return;
    }

    WhTool_ModSettingsChanged();
}

void Wh_ModUninit() {
    if (g_isToolModProcessLauncher) {
        return;
    }

    WhTool_ModUninit();
    ExitProcess(0);
}
// clang-format on
#ifdef __clang__
#pragma clang diagnostic pop
#endif
