// ==WindhawkMod==
// @id              remap-copilot-key
// @name            Remap Copilot Key
// @description     Global low-level hook running inside Windhawk to remap or disable the Copilot key.
// @version         1.0
// @author          Lukvbp
// @github          https://github.com/lukvbp
// @include         windhawk.exe
// @compilerOptions -luser32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Remap Copilot Key
A lightweight, global keyboard hook running directly inside the Windhawk engine process. 

Because modern Copilot keys send a macro sequence (`Left Shift + Left Win + F23`), this mod intercepts the sequence, forcefully lifts the modifier keys, and injects your custom keystroke.

### Virtual-Key (VK) Code Reference
If you choose the "Custom Remap" option, find the Hexadecimal code for your desired key below and enter it into the settings. For a complete list of all available keys, check the [Microsoft VK Code List](https://learn.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes).

#### ▶️ Media & Volume Controls
| Hex | Description | Hex | Description |
|---|---|---|---|
| `0xAD` | Volume Mute | `0xB0` | Next Track |
| `0xAE` | Volume Down | `0xB1` | Previous Track |
| `0xAF` | Volume Up | `0xB2` | Stop Media |
| `0xB3` | Play / Pause | `0xB5` | Select Media |

#### ⚙️ Modifiers & System Keys
| Hex | Description | Hex | Description |
|---|---|---|---|
| `0x08` | Backspace | `0xA0` | Left Shift |
| `0x09` | Tab | `0xA1` | Right Shift |
| `0x0D` | Enter | `0xA2` | Left Ctrl |
| `0x1B` | Escape | `0xA3` | Right Ctrl |
| `0x20` | Spacebar | `0xA4` | Left Alt |
| `0x5B` | Left Windows | `0xA5` | Right Alt |
| `0x5C` | Right Windows | `0x5D` | Application/Menu Key |
| `0x14` | Caps Lock | `0x5F` | Computer Sleep |

#### 🧭 Navigation & Arrows
| Hex | Description | Hex | Description |
|---|---|---|---|
| `0x21` | Page Up | `0x25` | Left Arrow |
| `0x22` | Page Down | `0x26` | Up Arrow |
| `0x23` | End | `0x27` | Right Arrow |
| `0x24` | Home | `0x28` | Down Arrow |
| `0x2D` | Insert | `0x2E` | Delete |

#### ⌨️ Alphabet & Numbers
| Hex | Description | Hex | Description | Hex | Description |
|---|---|---|---|---|---|
| `0x30` | 0 | `0x41` | A | `0x4E` | N |
| `0x31` | 1 | `0x42` | B | `0x4F` | O |
| `0x32` | 2 | `0x43` | C | `0x50` | P |
| `0x33` | 3 | `0x44` | D | `0x51` | Q |
| `0x34` | 4 | `0x45` | E | `0x52` | R |
| `0x35` | 5 | `0x46` | F | `0x53` | S |
| `0x36` | 6 | `0x47` | G | `0x54` | T |
| `0x37` | 7 | `0x48` | H | `0x55` | U |
| `0x38` | 8 | `0x49` | I | `0x56` | V |
| `0x39` | 9 | `0x4A` | J | `0x57` | W |
| | | `0x4B` | K | `0x58` | X |
| | | `0x4C` | L | `0x59` | Y |
| | | `0x4D` | M | `0x5A` | Z |

#### 🛠️ Function Keys (F1-F24)
| Hex | Description | Hex | Description | Hex | Description |
|---|---|---|---|---|---|
| `0x70` | F1 | `0x78` | F9 | `0x80` | F17 |
| `0x71` | F2 | `0x79` | F10 | `0x81` | F18 |
| `0x72` | F3 | `0x7A` | F11 | `0x82` | F19 |
| `0x73` | F4 | `0x7B` | F12 | `0x83` | F20 |
| `0x74` | F5 | `0x7C` | F13 | `0x84` | F21 |
| `0x75` | F6 | `0x7D` | F14 | `0x85` | F22 |
| `0x76` | F7 | `0x7E` | F15 | `0x86` | F23 |
| `0x77` | F8 | `0x7F` | F16 | `0x87` | F24 |

#### 🌐 Browser & Apps
| Hex | Description | Hex | Description |
|---|---|---|---|
| `0xA6` | Browser Back | `0xAA` | Browser Search |
| `0xA7` | Browser Forward | `0xAB` | Browser Favorites |
| `0xA8` | Browser Refresh | `0xAC` | Browser Home |
| `0xA9` | Browser Stop | `0xB4` | Start Mail |
| `0xB6` | Start App 1 (Calculator)| `0xB7` | Start App 2 |
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- CopilotMode: "Remap"
  $name: "Copilot Key Action"
  $options:
  - "Default": "Default"
  - "Disable": "Disabled"
  - "Remap": "Custom Remap"
- CopilotVkCode: "0xA3"
  $name: "Copilot Key Custom VK Code"
  $description: Enter hex (e.g., 0xA3) or decimal.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <thread>
#include <mutex>
#include <cwchar>

// --- ENUMS ---
enum class KeyMode {
    Default,
    Disable,
    Remap
};

// --- GLOBALS ---
HHOOK g_hKeyboardHook = NULL;
DWORD g_workerThreadId = 0;
std::thread g_workerThread;

struct {
    KeyMode copilotMode;
    int copilotVkCode;
} g_settings;
std::mutex g_settingsMutex;

// --- SETTINGS ---

KeyMode ParseModeString(PCWSTR modeStr) {
    if (!modeStr) return KeyMode::Default;
    if (wcscmp(modeStr, L"Disable") == 0) return KeyMode::Disable;
    if (wcscmp(modeStr, L"Remap") == 0) return KeyMode::Remap;
    return KeyMode::Default;
}

void LoadSettings() {
    std::lock_guard<std::mutex> lock(g_settingsMutex);
    
    // Parse Mode
    PCWSTR copilotModeStr = Wh_GetStringSetting(L"CopilotMode");
    g_settings.copilotMode = ParseModeString(copilotModeStr);
    Wh_FreeStringSetting(copilotModeStr);

    // Parse VK Code
    PCWSTR copilotVkStr = Wh_GetStringSetting(L"CopilotVkCode");
    g_settings.copilotVkCode = wcstol(copilotVkStr, nullptr, 0);
    Wh_FreeStringSetting(copilotVkStr);
}

void WhTool_ModSettingsChanged() {
    LoadSettings();
}

// --- MOD LOGIC ---

// Helper to determine if a VK code requires the Extended flag
bool IsExtendedKey(int vkCode) {
    switch (vkCode) {
        case VK_UP: case VK_DOWN: case VK_LEFT: case VK_RIGHT:
        case VK_HOME: case VK_END: case VK_PRIOR: case VK_NEXT:
        case VK_INSERT: case VK_DELETE:
        case VK_MEDIA_NEXT_TRACK: case VK_MEDIA_PREV_TRACK:
        case VK_MEDIA_STOP: case VK_MEDIA_PLAY_PAUSE:
        case VK_VOLUME_MUTE: case VK_VOLUME_DOWN: case VK_VOLUME_UP:
        case VK_BROWSER_BACK: case VK_BROWSER_FORWARD: case VK_BROWSER_REFRESH:
        case VK_BROWSER_STOP: case VK_BROWSER_SEARCH: case VK_BROWSER_FAVORITES:
        case VK_BROWSER_HOME: case VK_LAUNCH_MAIL: case VK_LAUNCH_MEDIA_SELECT:
        case VK_LAUNCH_APP1: case VK_LAUNCH_APP2:
        case VK_RCONTROL: case VK_RMENU: // Right Ctrl and Right Alt
            return true;
        default:
            return false;
    }
}

void SimulateKeystroke(int vkCode, bool isKeyDown) {
    INPUT input = {0};
    input.type = INPUT_KEYBOARD;
    input.ki.wVk = (WORD)vkCode;
    
    DWORD flags = isKeyDown ? 0 : KEYEVENTF_KEYUP;
    if (IsExtendedKey(vkCode)) {
        flags |= KEYEVENTF_EXTENDEDKEY;
    }
    
    input.ki.dwFlags = flags;
    SendInput(1, &input, sizeof(INPUT));
}

LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        KBDLLHOOKSTRUCT* pkbhs = (KBDLLHOOKSTRUCT*)lParam;

        // Ignore injected keys so our own SendInput doesn't loop infinitely
        if (pkbhs->flags & LLKHF_INJECTED) {
            return CallNextHookEx(g_hKeyboardHook, nCode, wParam, lParam);
        }

        bool isKeyDown = (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN);

        // Copilot Key Macro Intercept (LShift + LWin + F23)
        if (pkbhs->vkCode == VK_F23) {
            static bool s_copilotActive = false;
            static int s_activeTargetVk = 0; // Tracks the currently held key to prevent getting stuck
            
            KeyMode mode; int targetVk;
            {
                std::lock_guard<std::mutex> lock(g_settingsMutex);
                mode = g_settings.copilotMode;
                targetVk = g_settings.copilotVkCode;
            }
            
            if (mode == KeyMode::Disable) {
                if (isKeyDown) {
                    if (!s_copilotActive) {
                        INPUT inputs[2] = {};
                        inputs[0].type = INPUT_KEYBOARD; inputs[0].ki.wVk = VK_LSHIFT; inputs[0].ki.dwFlags = KEYEVENTF_KEYUP;
                        inputs[1].type = INPUT_KEYBOARD; inputs[1].ki.wVk = VK_LWIN; inputs[1].ki.dwFlags = KEYEVENTF_KEYUP;
                        SendInput(2, inputs, sizeof(INPUT));
                        s_copilotActive = true;
                    }
                } else {
                    s_copilotActive = false; 
                }
                return 1; // Swallow the F23
            }
            
            if (mode == KeyMode::Remap) {
                // Ensure targetVk is valid before executing
                if (targetVk > 0 && targetVk <= 254) {
                    if (isKeyDown) {
                        if (!s_copilotActive) {
                            s_activeTargetVk = targetVk; // Lock in the key we are pressing
                            
                            INPUT inputs[3] = {};
                            inputs[0].type = INPUT_KEYBOARD; inputs[0].ki.wVk = VK_LSHIFT; inputs[0].ki.dwFlags = KEYEVENTF_KEYUP;
                            inputs[1].type = INPUT_KEYBOARD; inputs[1].ki.wVk = VK_LWIN; inputs[1].ki.dwFlags = KEYEVENTF_KEYUP;
                            
                            inputs[2].type = INPUT_KEYBOARD; 
                            inputs[2].ki.wVk = (WORD)s_activeTargetVk; 
                            inputs[2].ki.dwFlags = IsExtendedKey(s_activeTargetVk) ? KEYEVENTF_EXTENDEDKEY : 0;
                            
                            SendInput(3, inputs, sizeof(INPUT));
                            s_copilotActive = true;
                        } else {
                            SimulateKeystroke(s_activeTargetVk, true);
                        }
                    } else {
                        // Release using the locked-in key, not the settings cache
                        SimulateKeystroke(s_activeTargetVk, false);
                        s_copilotActive = false; 
                        s_activeTargetVk = 0;
                    }
                    return 1;
                }
            }
        }
    }

    return CallNextHookEx(g_hKeyboardHook, nCode, wParam, lParam);
}

void WorkerLoop() {
    g_workerThreadId = GetCurrentThreadId();
    
    g_hKeyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, GetModuleHandle(NULL), 0);
    
    if (!g_hKeyboardHook) {
        Wh_Log(L"Failed to install keyboard hook.");
        return;
    }

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    UnhookWindowsHookEx(g_hKeyboardHook);
    g_hKeyboardHook = NULL;
}

// --- ENTRY POINTS ---

BOOL WhTool_ModInit() {
    LoadSettings();
    g_workerThread = std::thread(WorkerLoop);
    return TRUE;
}

void WhTool_ModUninit() {
    if (g_workerThreadId != 0) {
        PostThreadMessage(g_workerThreadId, WM_QUIT, 0, 0);
    }
    if (g_workerThread.joinable()) {
        g_workerThread.join();
    }
}

// --- WINDHAWK TOOL BOILERPLATE (DO NOT EDIT) ---

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
