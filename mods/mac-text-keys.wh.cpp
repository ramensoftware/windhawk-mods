// ==WindhawkMod==
// @id              mac-text-keys
// @name            Mac-style Text Navigation
// @description     Ctrl+Arrows jump to line start/end and Alt+Arrows jump by word, like Cmd/Option on macOS
// @version         0.1
// @author          Caio Lemos
// @github          https://github.com/lemoscaio
// @include         windhawk.exe
// @compilerOptions -lshell32
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Mac-style Text Navigation

Makes the modifier keys behave like on macOS when moving the text cursor:
**Ctrl acts like Cmd** and **Alt acts like Option**.

| Shortcut         | Does the same as (Windows) | macOS equivalent |
|------------------|----------------------------|------------------|
| Ctrl + ←         | Home (start of line)       | Cmd + ←          |
| Ctrl + →         | End (end of line)          | Cmd + →          |
| Alt + ←          | Ctrl + ← (previous word)   | Option + ←       |
| Alt + →          | Ctrl + → (next word)       | Option + →       |

Holding **Shift** as well extends the selection, as expected.

## Notes
- The mod runs in a dedicated background `windhawk.exe` process and uses a
  global low-level keyboard hook, so it works in every application.
- The remapping applies everywhere, not only in text fields. For example,
  Alt+Left/Right no longer go Back/Forward in browsers, File Explorer and
  Settings, and Ctrl+Left/Right lose app-specific meanings (such as jumping to
  the edge of a data region in Excel). Use the "Excluded programs" setting for
  apps where you want the original behavior.
- Windows running as administrator are not affected unless Windhawk itself
  runs elevated (a Windows security restriction).
- Ctrl+Alt combinations (including AltGr) and combinations with the Win key
  are left untouched.
- Modifiers produced by remapping tools that inject input (e.g. PowerToys
  Keyboard Manager, AutoHotkey) are not recognized. Remaps done through the
  scancode map (e.g. SharpKeys) work.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- lineNavigation: true
  $name: Ctrl + Left/Right → Home/End
  $description: Move to the start/end of the line, like Cmd + Left/Right on macOS.
- wordNavigation: true
  $name: Alt + Left/Right → Ctrl + Left/Right
  $description: Move by word, like Option + Left/Right on macOS.
- excludedPrograms:
  - ""
  $name: Excluded programs
  $description: >-
    Executable names (e.g. mstsc.exe) where the remapping is disabled. The
    process of the foreground window is matched, so UWP apps hosted by
    ApplicationFrameHost.exe can't be excluded by their own executable name.
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>
#include <windows.h>

#include <string>
#include <vector>

////////////////////////////////////////////////////////////////////////////////
// Settings

struct {
    bool lineNavigation;
    bool wordNavigation;
    std::vector<std::wstring> excludedPrograms;
} g_settings;

SRWLOCK g_settingsLock = SRWLOCK_INIT;

void LoadSettings() {
    AcquireSRWLockExclusive(&g_settingsLock);

    g_settings.lineNavigation = Wh_GetIntSetting(L"lineNavigation");
    g_settings.wordNavigation = Wh_GetIntSetting(L"wordNavigation");

    g_settings.excludedPrograms.clear();
    for (int i = 0;; i++) {
        auto program =
            WindhawkUtils::StringSetting::make(L"excludedPrograms[%d]", i);
        if (!*program.get()) {
            break;
        }
        g_settings.excludedPrograms.push_back(program.get());
    }

    ReleaseSRWLockExclusive(&g_settingsLock);
}

////////////////////////////////////////////////////////////////////////////////
// Keyboard remapping

// An unassigned virtual key, sent around injected modifier changes so that
// Windows doesn't treat them as a lone Alt tap (which would activate the menu
// bar) or as the Alt+Shift / Ctrl+Shift input language hotkey.
constexpr WORD kMaskVk = 0xE8;

HHOOK g_keyboardHook;
HANDLE g_hookThread;
DWORD g_hookThreadId;

// Physical (non-injected) modifier state, tracked by the hook itself.
struct {
    bool lctrl, rctrl, lalt, ralt, lwin, rwin;
} g_phys;

// Per arrow key: whether a key-down was swallowed / passed through since the
// last key-up. The key-up is swallowed only if no key-down was passed through,
// so that the key never stays logically pressed.
bool g_keyDownSwallowed[256];
bool g_keyDownPassed[256];

// The hook doesn't see releases made on the secure desktop / lock screen or in
// elevated windows, so drop tracked modifiers that are no longer down.
void ResyncModifiers() {
    auto down = [](int vk) { return (GetAsyncKeyState(vk) & 0x8000) != 0; };
    g_phys.lctrl = g_phys.lctrl && down(VK_LCONTROL);
    g_phys.rctrl = g_phys.rctrl && down(VK_RCONTROL);
    g_phys.lalt = g_phys.lalt && down(VK_LMENU);
    g_phys.ralt = g_phys.ralt && down(VK_RMENU);
    g_phys.lwin = g_phys.lwin && down(VK_LWIN);
    g_phys.rwin = g_phys.rwin && down(VK_RWIN);
}

bool IsExtendedKey(WORD vk) {
    switch (vk) {
        case VK_RCONTROL:
        case VK_RMENU:
        case VK_HOME:
        case VK_END:
        case VK_LEFT:
        case VK_RIGHT:
            return true;
    }
    return false;
}

void AddKey(std::vector<INPUT>& inputs, WORD vk, bool down) {
    INPUT input{};
    input.type = INPUT_KEYBOARD;
    input.ki.wVk = vk;
    input.ki.wScan = (WORD)MapVirtualKeyW(vk, MAPVK_VK_TO_VSC);
    input.ki.dwFlags = (down ? 0 : KEYEVENTF_KEYUP) |
                       (IsExtendedKey(vk) ? KEYEVENTF_EXTENDEDKEY : 0);
    inputs.push_back(input);
}

void AddMask(std::vector<INPUT>& inputs) {
    AddKey(inputs, kMaskVk, true);
    AddKey(inputs, kMaskVk, false);
}

void Send(std::vector<INPUT>& inputs) {
    if (inputs.empty()) {
        return;
    }
    UINT sent = SendInput((UINT)inputs.size(), inputs.data(), sizeof(INPUT));
    if (sent != inputs.size()) {
        Wh_Log(L"SendInput sent %u of %zu inputs, error %u", sent,
               inputs.size(), GetLastError());
    }
}

bool IsForegroundProgramExcluded() {
    AcquireSRWLockShared(&g_settingsLock);
    bool hasExclusions = !g_settings.excludedPrograms.empty();
    ReleaseSRWLockShared(&g_settingsLock);
    if (!hasExclusions) {
        return false;
    }

    HWND hWnd = GetForegroundWindow();
    DWORD pid = 0;
    if (!hWnd || !GetWindowThreadProcessId(hWnd, &pid) || !pid) {
        return false;
    }

    HANDLE process =
        OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!process) {
        return false;
    }

    WCHAR path[MAX_PATH];
    DWORD size = ARRAYSIZE(path);
    bool ok = QueryFullProcessImageNameW(process, 0, path, &size);
    CloseHandle(process);
    if (!ok) {
        return false;
    }

    PCWSTR name = wcsrchr(path, L'\\');
    name = name ? name + 1 : path;

    bool excluded = false;
    AcquireSRWLockShared(&g_settingsLock);
    for (const auto& program : g_settings.excludedPrograms) {
        if (_wcsicmp(program.c_str(), name) == 0) {
            excluded = true;
            break;
        }
    }
    ReleaseSRWLockShared(&g_settingsLock);

    return excluded;
}

// Ctrl + Left/Right -> Home/End (Shift, if held, stays held).
void SendLineNavigation(WORD arrowVk) {
    WORD targetVk = arrowVk == VK_LEFT ? VK_HOME : VK_END;

    std::vector<INPUT> inputs;
    AddMask(inputs);
    if (g_phys.lctrl) AddKey(inputs, VK_LCONTROL, false);
    if (g_phys.rctrl) AddKey(inputs, VK_RCONTROL, false);
    AddKey(inputs, targetVk, true);
    AddKey(inputs, targetVk, false);
    if (g_phys.lctrl) AddKey(inputs, VK_LCONTROL, true);
    if (g_phys.rctrl) AddKey(inputs, VK_RCONTROL, true);
    AddMask(inputs);
    Send(inputs);
}

// Alt + Left/Right -> Ctrl + Left/Right (Shift, if held, stays held).
void SendWordNavigation(WORD arrowVk) {
    std::vector<INPUT> inputs;
    AddMask(inputs);
    if (g_phys.lalt) AddKey(inputs, VK_LMENU, false);
    if (g_phys.ralt) AddKey(inputs, VK_RMENU, false);
    AddKey(inputs, VK_LCONTROL, true);
    AddKey(inputs, arrowVk, true);
    AddKey(inputs, arrowVk, false);
    AddKey(inputs, VK_LCONTROL, false);
    if (g_phys.lalt) AddKey(inputs, VK_LMENU, true);
    if (g_phys.ralt) AddKey(inputs, VK_RMENU, true);
    // Also makes the later physical Alt release not count as a lone Alt tap.
    AddMask(inputs);
    Send(inputs);
}

LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode != HC_ACTION) {
        return CallNextHookEx(nullptr, nCode, wParam, lParam);
    }

    const KBDLLHOOKSTRUCT* kb = (const KBDLLHOOKSTRUCT*)lParam;

    // Let all injected input through (ours and other tools'), this also
    // prevents feedback loops.
    if (kb->flags & LLKHF_INJECTED) {
        return CallNextHookEx(nullptr, nCode, wParam, lParam);
    }

    bool isDown = wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN;
    DWORD vk = kb->vkCode;

    switch (vk) {
        case VK_LCONTROL: g_phys.lctrl = isDown; break;
        case VK_RCONTROL: g_phys.rctrl = isDown; break;
        case VK_LMENU:    g_phys.lalt = isDown;  break;
        case VK_RMENU:    g_phys.ralt = isDown;  break;
        case VK_LWIN:     g_phys.lwin = isDown;  break;
        case VK_RWIN:     g_phys.rwin = isDown;  break;

        case VK_LEFT:
        case VK_RIGHT: {
            if (!isDown) {
                bool swallow =
                    g_keyDownSwallowed[vk] && !g_keyDownPassed[vk];
                g_keyDownSwallowed[vk] = false;
                g_keyDownPassed[vk] = false;
                if (swallow) {
                    return 1;
                }
                break;
            }

            ResyncModifiers();

            bool ctrl = g_phys.lctrl || g_phys.rctrl;
            bool alt = g_phys.lalt || g_phys.ralt;
            bool win = g_phys.lwin || g_phys.rwin;

            bool remap = false;

            // Leave Win combos and Ctrl+Alt (incl. AltGr) untouched.
            if (!win && ctrl != alt) {
                AcquireSRWLockShared(&g_settingsLock);
                remap = ctrl ? g_settings.lineNavigation
                             : g_settings.wordNavigation;
                ReleaseSRWLockShared(&g_settingsLock);

                if (remap && IsForegroundProgramExcluded()) {
                    remap = false;
                }
            }

            if (!remap) {
                g_keyDownPassed[vk] = true;
                break;
            }

            if (ctrl) {
                SendLineNavigation((WORD)vk);
            } else {
                SendWordNavigation((WORD)vk);
            }

            g_keyDownSwallowed[vk] = true;
            return 1;
        }
    }

    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

DWORD WINAPI HookThreadProc(LPVOID) {
    HMODULE module = nullptr;
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                           GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                       (LPCWSTR)&LowLevelKeyboardProc, &module);

    g_keyboardHook =
        SetWindowsHookExW(WH_KEYBOARD_LL, LowLevelKeyboardProc, module, 0);
    if (!g_keyboardHook) {
        Wh_Log(L"SetWindowsHookExW failed, error %u", GetLastError());
        return 1;
    }

    Wh_Log(L"Keyboard hook installed");

    MSG msg;
    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    UnhookWindowsHookEx(g_keyboardHook);
    g_keyboardHook = nullptr;
    return 0;
}

////////////////////////////////////////////////////////////////////////////////
// Tool mod callbacks

BOOL WhTool_ModInit() {
    Wh_Log(L"Init");

    LoadSettings();

    g_hookThread =
        CreateThread(nullptr, 0, HookThreadProc, nullptr, 0, &g_hookThreadId);
    if (!g_hookThread) {
        Wh_Log(L"CreateThread failed");
        return FALSE;
    }

    return TRUE;
}

void WhTool_ModSettingsChanged() {
    Wh_Log(L"SettingsChanged");

    LoadSettings();
}

void WhTool_ModUninit() {
    Wh_Log(L"Uninit");

    if (g_hookThread) {
        PostThreadMessageW(g_hookThreadId, WM_QUIT, 0, 0);
        WaitForSingleObject(g_hookThread, 2000);
        CloseHandle(g_hookThread);
        g_hookThread = nullptr;
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
