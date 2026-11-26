// ==WindhawkMod==
// @id           alt-tab-no-repeat
// @name         Alt+Tab No Repeat
// @description  Disable automatic repetition of Alt+Tab
// @version      1.0
// @author       ElektroGvozd
// @github       https://github.com/ElektroGvozd
// @include      windhawk.exe
// @compilerOptions -luser32 -lshell32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Alt+Tab No Repeat

Holding Alt+Tab opens the Windows task switcher and advances it once. Keep
holding Alt and release and press Tab again to move to the next window.

The mod also suppresses automatic Tab repeats in Shift+Alt+Tab and Ctrl+Alt+Tab.
The keyboard hook applies across applications on the current desktop, including
third-party switchers that receive these keyboard events. Tab repeats normally
when Alt is up.

The mod runs in a dedicated Windhawk process.
*/
// ==/WindhawkModReadme==

#include <windows.h>
#include <shellapi.h>
#include <wchar.h>

static HANDLE g_stopEvent;
static HANDLE g_readyEvent;
static HANDLE g_worker;
static bool g_tabDown;

static LRESULT CALLBACK KeyboardProc(int code, WPARAM message, LPARAM data) {
    if (code == HC_ACTION) {
        const auto& key = *reinterpret_cast<const KBDLLHOOKSTRUCT*>(data);
        const bool keyUp = message == WM_KEYUP || message == WM_SYSKEYUP;

        if (keyUp && (key.vkCode == VK_TAB || key.vkCode == VK_MENU ||
                      key.vkCode == VK_LMENU || key.vkCode == VK_RMENU)) {
            g_tabDown = false;
        }

        if (key.vkCode == VK_TAB &&
            (message == WM_KEYDOWN || message == WM_SYSKEYDOWN)) {
            const bool repeated = g_tabDown;
            g_tabDown = true;
            if (repeated && (key.flags & LLKHF_ALTDOWN)) {
                return 1;
            }
        }
    }

    return CallNextHookEx(nullptr, code, message, data);
}

static DWORD WINAPI HookThread(LPVOID) {
    MSG message;
    PeekMessageW(&message, nullptr, 0, 0, PM_NOREMOVE);

    HMODULE module;
    if (!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                                GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                            reinterpret_cast<LPCWSTR>(KeyboardProc), &module)) {
        Wh_Log(L"GetModuleHandleExW failed: %lu", GetLastError());
        return 1;
    }

    HHOOK hook = SetWindowsHookExW(WH_KEYBOARD_LL, KeyboardProc, module, 0);
    if (!hook) {
        Wh_Log(L"SetWindowsHookExW failed: %lu", GetLastError());
        return 1;
    }

    if (!SetEvent(g_readyEvent)) {
        Wh_Log(L"SetEvent (ready) failed: %lu", GetLastError());
    } else {
        while (true) {
            const DWORD result = MsgWaitForMultipleObjectsEx(
                1, &g_stopEvent, INFINITE, QS_ALLINPUT, MWMO_INPUTAVAILABLE);
            if (result == WAIT_OBJECT_0) {
                break;
            }
            if (result != WAIT_OBJECT_0 + 1) {
                Wh_Log(L"MsgWaitForMultipleObjectsEx failed: %lu", GetLastError());
                break;
            }

            // Process one queued message before checking the stop event again.
            if (PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE)) {
                if (message.message == WM_QUIT) {
                    break;
                }
                TranslateMessage(&message);
                DispatchMessageW(&message);
            }
        }
    }

    if (!UnhookWindowsHookEx(hook)) {
        Wh_Log(L"UnhookWindowsHookEx failed: %lu", GetLastError());
    }
    return 0;
}

void WhTool_ModUninit() {
    if (g_worker) {
        SetEvent(g_stopEvent);
        WaitForSingleObject(g_worker, INFINITE);
        CloseHandle(g_worker);
        g_worker = nullptr;
    }
    if (g_readyEvent) {
        CloseHandle(g_readyEvent);
        g_readyEvent = nullptr;
    }
    if (g_stopEvent) {
        CloseHandle(g_stopEvent);
        g_stopEvent = nullptr;
    }
}

BOOL WhTool_ModInit() {
    g_tabDown = false;

    g_stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_stopEvent) {
        Wh_Log(L"CreateEventW (stop) failed: %lu", GetLastError());
        return FALSE;
    }

    g_readyEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_readyEvent) {
        Wh_Log(L"CreateEventW (ready) failed: %lu", GetLastError());
        WhTool_ModUninit();
        return FALSE;
    }

    g_worker = CreateThread(nullptr, 0, HookThread, nullptr, 0, nullptr);
    if (!g_worker) {
        Wh_Log(L"CreateThread failed: %lu", GetLastError());
        WhTool_ModUninit();
        return FALSE;
    }

    const HANDLE startupHandles[] = {g_readyEvent, g_worker};
    const DWORD result = WaitForMultipleObjects(
        ARRAYSIZE(startupHandles), startupHandles, FALSE, INFINITE);
    if (result != WAIT_OBJECT_0) {
        if (result == WAIT_FAILED) {
            Wh_Log(L"WaitForMultipleObjects failed: %lu", GetLastError());
        }
        WhTool_ModUninit();
        return FALSE;
    }

    // Keep the event alive until the worker exits.
    return TRUE;
}

void WhTool_ModSettingsChanged() {}


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
