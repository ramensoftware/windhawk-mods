// ==WindhawkMod==
// @id           toggle-hidden-files
// @name         Toggle Hidden Files
// @description  Toggle the visibility of hidden files in Windows Explorer using Ctrl+H
// @version      1.0.0
// @author       Asteski
// @github       https://github.com/Asteski
// @include      windhawk.exe
// ==/WindhawkMod==

// ==WindhawkModSettings==
/*
- toggleProtectedFiles: true
  $name: Also toggle protected OS files
  $description: When enabled, Ctrl+H will also toggle the visibility of protected operating system files
*/
// ==/WindhawkModSettings==

// ==WindhawkModReadme==
/*
# Toggle Hidden Files

This mod allows you to toggle the visibility of hidden files in Windows Explorer using the Ctrl+H keyboard shortcut.

![toggle-hidden-files-gif](https://raw.githubusercontent.com/Asteski/Windhawk-Mods/refs/heads/main/img/toggle-hidden-files/toggle-hidden-files.gif)

## Features
- Ctrl+H hotkey that works only when Explorer windows are focused
- Toggles the "Show hidden files" setting
- Optional: Also toggle protected OS files
- Applies changes immediately to Explorer windows
- Works with all Windows Explorer windows

## Usage
1. **Focus an Explorer window** - Click on or open any File Explorer window
2. **Press Ctrl+H** - Use the keyboard shortcut to toggle hidden files visibility
3. **The setting will be applied immediately** to all Explorer windows

## Settings
- **Also toggle protected OS files**: When enabled, Ctrl+H will also show/hide protected operating system files

## Technical Details
- Only activates when Windows Explorer windows are in focus
- Queues toggle work to a worker thread to keep input handling responsive
- Uses Windows shell APIs to update hidden-file visibility live
- Notifies the shell to refresh views after toggling
- Handles proper cleanup when the mod is unloaded
*/
// ==/WindhawkModReadme==

#include <windows.h>
#include <shlobj.h>
#include <shellapi.h>
#include <stdio.h>

// Settings structure
struct {
    bool toggleProtectedFiles;
} g_settings;

// Global variables
HHOOK g_hKeyboardHook = nullptr;
HANDLE g_hookThread = nullptr;
DWORD g_hookThreadId = 0;
HANDLE g_hookThreadReadyEvent = nullptr;
bool g_hookInstallSucceeded = false;

constexpr UINT WM_APP_TOGGLE_HIDDEN_FILES = WM_APP + 1;


// Window context enumeration
enum WindowContext {
    CONTEXT_UNKNOWN = 0,
    CONTEXT_EXPLORER = 1
};

// Function declarations
LRESULT CALLBACK KeyboardHookProc(int nCode, WPARAM wParam, LPARAM lParam);
DWORD WINAPI HookThreadProc(LPVOID lpParameter);
bool ToggleHiddenFilesInShellState();
bool IsCtrlHPressed(WPARAM wParam, LPARAM lParam);
void LoadSettings();
WindowContext GetCurrentWindowContext();

// Get current window context based on focused window
WindowContext GetCurrentWindowContext() {
    HWND hForeground = GetForegroundWindow();
    if (!hForeground) {
        return CONTEXT_UNKNOWN;
    }
    
    wchar_t className[256];
    if (GetClassNameW(hForeground, className, sizeof(className) / sizeof(wchar_t)) == 0) {
        return CONTEXT_UNKNOWN;
    }
    
    // Check for Explorer windows
    if (wcscmp(className, L"CabinetWClass") == 0 || 
        wcscmp(className, L"ExploreWClass") == 0) {
        return CONTEXT_EXPLORER;
    }
    
    return CONTEXT_UNKNOWN;
}

// Toggle hidden files visibility using the shell state API.
bool ToggleHiddenFilesInShellState() {
    SHELLSTATE shellState{};
    constexpr DWORD kReadMask = SSF_SHOWALLOBJECTS | SSF_SHOWSUPERHIDDEN;

    SHGetSetSettings(&shellState, kReadMask, FALSE);

    shellState.fShowAllObjects = !shellState.fShowAllObjects;

    DWORD writeMask = SSF_SHOWALLOBJECTS;
    if (g_settings.toggleProtectedFiles) {
        shellState.fShowSuperHidden = !shellState.fShowSuperHidden;
        writeMask |= SSF_SHOWSUPERHIDDEN;
    }

    SHGetSetSettings(&shellState, writeMask, TRUE);
    SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, nullptr, nullptr);
    return true;
}

// Load settings from Windhawk configuration
void LoadSettings() {
    g_settings.toggleProtectedFiles =
        Wh_GetIntSetting(L"toggleProtectedFiles") != 0;
}

// Check if Ctrl+H is pressed
bool IsCtrlHPressed(WPARAM wParam, LPARAM lParam) {
    if (wParam != WM_KEYDOWN) {
        return false;
    }
    
    KBDLLHOOKSTRUCT* pKeyboard = (KBDLLHOOKSTRUCT*)lParam;
    
    // Check if 'H' key is pressed
    if (pKeyboard->vkCode != 'H') {
        return false;
    }
    
    const bool ctrlPressed = (GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0;
    const bool shiftPressed = (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0;
    const bool altPressed = (GetAsyncKeyState(VK_MENU) & 0x8000) != 0;
    const bool winPressed = (GetAsyncKeyState(VK_LWIN) & 0x8000) != 0 ||
                            (GetAsyncKeyState(VK_RWIN) & 0x8000) != 0;

    // Require Ctrl-only to avoid clobbering combinations like Ctrl+Shift+H.
    return ctrlPressed && !shiftPressed && !altPressed && !winPressed;
}

DWORD WINAPI HookThreadProc(LPVOID) {
    MSG msg;

    // Ensure the message queue exists before hooks/messages are used.
    PeekMessageW(&msg, nullptr, WM_USER, WM_USER, PM_NOREMOVE);

    g_hKeyboardHook = SetWindowsHookExW(WH_KEYBOARD_LL, KeyboardHookProc,
                                        nullptr, 0);
    g_hookInstallSucceeded = g_hKeyboardHook != nullptr;

    if (g_hookThreadReadyEvent) {
        SetEvent(g_hookThreadReadyEvent);
    }

    if (!g_hookInstallSucceeded) {
        return 0;
    }

    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        if (msg.message == WM_APP_TOGGLE_HIDDEN_FILES) {
            ToggleHiddenFilesInShellState();
        }
    }

    UnhookWindowsHookEx(g_hKeyboardHook);
    g_hKeyboardHook = nullptr;

    return 0;
}

// Keyboard hook procedure
LRESULT CALLBACK KeyboardHookProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode >= 0) {
        WindowContext context = GetCurrentWindowContext();
        
        // Only process if we're in Explorer windows
        if (context == CONTEXT_EXPLORER && IsCtrlHPressed(wParam, lParam)) {
            if (g_hookThreadId != 0) {
                PostThreadMessageW(g_hookThreadId, WM_APP_TOGGLE_HIDDEN_FILES,
                                   0, 0);
            }
            
            // Consume the key press
            return 1;
        }
    }
    
    return CallNextHookEx(g_hKeyboardHook, nCode, wParam, lParam);
}

// Mod initialization
BOOL WhTool_ModInit() {
    // Load settings
    LoadSettings();

    g_hookThreadReadyEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_hookThreadReadyEvent) {
        return FALSE;
    }

    g_hookInstallSucceeded = false;
    g_hookThread = CreateThread(nullptr, 0, HookThreadProc, nullptr, 0,
                                &g_hookThreadId);
    if (!g_hookThread) {
        CloseHandle(g_hookThreadReadyEvent);
        g_hookThreadReadyEvent = nullptr;
        g_hookThreadId = 0;
        return FALSE;
    }

    if (WaitForSingleObject(g_hookThreadReadyEvent, 5000) != WAIT_OBJECT_0 ||
        !g_hookInstallSucceeded) {
        if (g_hookThreadId != 0) {
            PostThreadMessageW(g_hookThreadId, WM_QUIT, 0, 0);
        }
        WaitForSingleObject(g_hookThread, 5000);
        CloseHandle(g_hookThread);
        g_hookThread = nullptr;
        g_hookThreadId = 0;
        CloseHandle(g_hookThreadReadyEvent);
        g_hookThreadReadyEvent = nullptr;
        return FALSE;
    }

    CloseHandle(g_hookThreadReadyEvent);
    g_hookThreadReadyEvent = nullptr;

    return TRUE;
}

// Settings changed callback
void WhTool_ModSettingsChanged() {
    LoadSettings();
}

// Mod cleanup
void WhTool_ModUninit() {
    if (g_hookThreadId != 0) {
        PostThreadMessageW(g_hookThreadId, WM_QUIT, 0, 0);
    }

    if (g_hookThread) {
        WaitForSingleObject(g_hookThread, 5000);
        CloseHandle(g_hookThread);
        g_hookThread = nullptr;
    }

    g_hookThreadId = 0;

    if (g_hookThreadReadyEvent) {
        CloseHandle(g_hookThreadReadyEvent);
        g_hookThreadReadyEvent = nullptr;
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
        LPSECURITY_ATTRIBUTES lpThreadAttributes, BOOL bInheritHandles,
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
