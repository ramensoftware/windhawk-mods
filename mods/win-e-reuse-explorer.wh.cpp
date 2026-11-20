// ==WindhawkMod==
// @id              win-e-reuse-explorer
// @name            Win+E Reuse File Explorer
// @description     Restore an existing File Explorer window with Win+E, or open one normally if none exists
// @version         1.0.0
// @author          Asteski
// @github          https://github.com/Asteski
// @include         windhawk.exe
// @compilerOptions -ldwmapi
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Win+E Reuse File Explorer

Press **Win+E** to bring the most recently active File Explorer window forward.
Minimized windows are restored. If Explorer is already focused, it stays focused.

If no Explorer window exists on the current virtual desktop, Windows handles
Win+E normally and opens a new window. Extra modifiers (Ctrl, Shift, or Alt)
are left unchanged. There are no settings.
*/
// ==/WindhawkModReadme==

#include <windows.h>
#include <dwmapi.h>
#include <shellapi.h>
#include <cstdio>

static HANDLE g_thread;
static HANDLE g_ready;
static DWORD g_threadId;
static bool g_hookInstalled;
static bool g_suppressE;
static constexpr UINT kRestoreWindow = WM_APP + 1;

static BOOL CALLBACK FindExplorerWindow(HWND window, LPARAM param) {
    if (!IsWindowVisible(window) || GetWindow(window, GW_OWNER)) {
        return TRUE;
    }

    wchar_t className[32];
    if (!GetClassNameW(window, className, ARRAYSIZE(className)) ||
        (lstrcmpW(className, L"CabinetWClass") != 0 &&
         lstrcmpW(className, L"ExploreWClass") != 0)) {
        return TRUE;
    }

    DWORD cloaked = 0;
    if (SUCCEEDED(DwmGetWindowAttribute(window, DWMWA_CLOAKED,
                                      &cloaked, sizeof(cloaked))) && cloaked) {
        return TRUE;
    }

    // EnumWindows visits top-level windows in Z order.
    *reinterpret_cast<HWND*>(param) = window;
    return FALSE;
}

static bool KeyDown(int key) {
    return (GetAsyncKeyState(key) & 0x8000) != 0;
}

static LRESULT CALLBACK KeyboardHook(int code, WPARAM message, LPARAM param) {
    if (code == HC_ACTION) {
        const auto* key = reinterpret_cast<const KBDLLHOOKSTRUCT*>(param);
        if (key->vkCode == 'E' && !(key->flags & LLKHF_INJECTED)) {
            const bool down = message == WM_KEYDOWN || message == WM_SYSKEYDOWN;
            const bool up = message == WM_KEYUP || message == WM_SYSKEYUP;
            if (g_suppressE) {
                if (up) {
                    g_suppressE = false;
                }
                return 1;
            }

            if (down && (KeyDown(VK_LWIN) || KeyDown(VK_RWIN)) &&
                !KeyDown(VK_CONTROL) && !KeyDown(VK_SHIFT) && !KeyDown(VK_MENU)) {
                HWND window = nullptr;
                EnumWindows(FindExplorerWindow, reinterpret_cast<LPARAM>(&window));
                if (window && PostThreadMessageW(g_threadId, kRestoreWindow,
                                                reinterpret_cast<WPARAM>(window), 0)) {
                    g_suppressE = true;

                    // Mark Win as used so releasing it doesn't open Start.
                    INPUT input[2] = {};
                    input[0].type = input[1].type = INPUT_KEYBOARD;
                    input[0].ki.wVk = input[1].ki.wVk = 0xFF;  // Unassigned VK.
                    input[1].ki.dwFlags = KEYEVENTF_KEYUP;
                    SendInput(ARRAYSIZE(input), input, sizeof(INPUT));
                    return 1;
                }
            }
        }
    }
    return CallNextHookEx(nullptr, code, message, param);
}

static DWORD WINAPI HookThread(void*) {
    MSG message;
    PeekMessageW(&message, nullptr, WM_USER, WM_USER, PM_NOREMOVE);
    HHOOK hook = SetWindowsHookExW(WH_KEYBOARD_LL, KeyboardHook, nullptr, 0);
    if (!hook) {
        Wh_Log(L"SetWindowsHookExW failed: %u", GetLastError());
    }
    g_hookInstalled = hook != nullptr;
    SetEvent(g_ready);
    if (!hook) {
        return 0;
    }

    while (GetMessageW(&message, nullptr, 0, 0) > 0) {
        if (message.message == kRestoreWindow) {
            HWND window = reinterpret_cast<HWND>(message.wParam);
            if (IsWindow(window)) {
                if (IsIconic(window)) {
                    ShowWindowAsync(window, SW_RESTORE);
                }
                SetForegroundWindow(window);
            }
        }
    }
    UnhookWindowsHookEx(hook);
    return 0;
}

BOOL WhTool_ModInit() {
    g_ready = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_ready) {
        Wh_Log(L"CreateEventW failed: %u", GetLastError());
        return FALSE;
    }
    g_thread = CreateThread(nullptr, 0, HookThread, nullptr, 0, &g_threadId);
    if (!g_thread) {
        Wh_Log(L"CreateThread failed: %u", GetLastError());
    }
    if (g_thread) {
        WaitForSingleObject(g_ready, INFINITE);
    }
    CloseHandle(g_ready);
    g_ready = nullptr;
    if (!g_thread || !g_hookInstalled) {
        if (g_thread) {
            WaitForSingleObject(g_thread, INFINITE);
            CloseHandle(g_thread);
            g_thread = nullptr;
        }
        return FALSE;
    }
    return TRUE;
}

void WhTool_ModSettingsChanged() {
}

void WhTool_ModUninit() {
    if (g_thread) {
        PostThreadMessageW(g_threadId, WM_QUIT, 0, 0);
        WaitForSingleObject(g_thread, INFINITE);
        CloseHandle(g_thread);
        g_thread = nullptr;
    }
}

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
