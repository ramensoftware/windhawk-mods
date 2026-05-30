// ==WindhawkMod==
// @id              caps-ime-switcher
// @name            Caps IME Switcher
// @description     Brings the macOS Caps Lock input source switching behavior to Windows.
// @version         0.7
// @author          ZeonXr
// @github          https://github.com/ZeonXr
// @include         windhawk.exe
// @compilerOptions -luser32 -lshell32
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Caps IME Switcher

Brings the macOS Caps Lock input source switching behavior to Windows.

Short-press Caps Lock to switch to the next input language.
Long-press Caps Lock to toggle Caps Lock on/off.

**Pair with** IME Native Mode Lock (`ime-mode-lock`) — auto-locks
IMEs to their native mode. Best experience when used together.

---

将 macOS 的 Caps Lock 输入法切换方式带到 Windows。

短按 Caps Lock 切换下一个输入语言，长按触发大小写锁定。

**推荐搭配** IME Native Mode Lock (`ime-mode-lock`) — 可在切换输入法时
自动锁定中文模式，两个 mod 配合使用体验最佳。
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- LongPressMs: 300
  $name: Long press threshold (ms)
  $description: How long to hold Caps Lock (in milliseconds) before it toggles Caps Lock instead of switching input language
- PassThroughProcesses: ""
  $name: Caps Lock pass-through process list
  $description: Comma-separated process names where Caps Lock should not be intercepted (e.g. "Moonlight.exe,mstsc.exe"). Useful for remote desktop clients.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <shellapi.h>

#include <windhawk_api.h>
#include <windhawk_utils.h>

#include <atomic>
#include <cwchar>
#include <string>
#include <vector>

constexpr UINT kSwitchInputSourceMessage = WM_APP + 1;
constexpr ULONG_PTR kInjectedCapsExtraInfo = 0x43494D45;

std::atomic<int> g_longPressMs{300};

HHOOK g_keyboardHook = nullptr;
HANDLE g_workerThread = nullptr;
std::atomic<DWORD> g_workerThreadId{0};
std::atomic<UINT_PTR> g_longPressTimerId{0};

std::atomic<bool> g_capsIsDown{false};
std::atomic<bool> g_capsLongPressTriggered{false};

enum class ProcessRole { kNone, kLauncher, kToolMod };
ProcessRole g_processRole = ProcessRole::kNone;
HANDLE g_toolModProcessMutex = nullptr;

std::vector<std::wstring> g_passThroughList;

bool IsCurrentProcessWindhawk() {
    WCHAR processPath[MAX_PATH];
    DWORD pathLength =
        GetModuleFileNameW(nullptr, processPath, ARRAYSIZE(processPath));
    if (pathLength == 0 || pathLength == ARRAYSIZE(processPath)) {
        return false;
    }

    const WCHAR* fileName = wcsrchr(processPath, L'\\');
    fileName = fileName ? fileName + 1 : processPath;
    return lstrcmpiW(fileName, L"windhawk.exe") == 0;
}

void LoadSettings() {
    int longPressMs = Wh_GetIntSetting(L"LongPressMs");
    if (longPressMs < 50) {
        longPressMs = 50;
    } else if (longPressMs > 5000) {
        longPressMs = 5000;
    }

    PCWSTR passThroughProcesses = Wh_GetStringSetting(L"PassThroughProcesses");
    g_passThroughList.clear();
    if (passThroughProcesses) {
        if (*passThroughProcesses) {
            std::wstring list(passThroughProcesses);
            size_t pos = 0;
            while (pos < list.size()) {
                size_t comma = list.find(L',', pos);
                std::wstring token = list.substr(pos, comma - pos);
                size_t start = token.find_first_not_of(L" \t");
                if (start != std::wstring::npos) {
                    size_t end = token.find_last_not_of(L" \t");
                    token = token.substr(start, end - start + 1);
                    if (!token.empty()) {
                        g_passThroughList.push_back(token);
                    }
                }
                if (comma == std::wstring::npos) break;
                pos = comma + 1;
            }
        }
        Wh_FreeStringSetting(passThroughProcesses);
    }

    g_longPressMs.store(longPressMs, std::memory_order_relaxed);
}

HWND GetInputTargetWindow() {
    HWND foregroundWindow = GetForegroundWindow();
    if (!foregroundWindow) {
        return nullptr;
    }

    DWORD foregroundThreadId = GetWindowThreadProcessId(foregroundWindow, nullptr);
    if (!foregroundThreadId) {
        return foregroundWindow;
    }

    GUITHREADINFO guiThreadInfo = {sizeof(guiThreadInfo)};
    if (GetGUIThreadInfo(foregroundThreadId, &guiThreadInfo) &&
        guiThreadInfo.hwndFocus) {
        return guiThreadInfo.hwndFocus;
    }

    return foregroundWindow;
}

bool TryGetNextKeyboardLayout(HWND targetWindow, HKL* nextLayout) {
    int layoutCount = GetKeyboardLayoutList(0, nullptr);
    if (layoutCount <= 1) {
        return false;
    }

    std::vector<HKL> layouts(layoutCount);
    layoutCount = GetKeyboardLayoutList(layoutCount, layouts.data());
    if (layoutCount <= 1) {
        return false;
    }

    DWORD targetThreadId = GetWindowThreadProcessId(targetWindow, nullptr);
    HKL currentLayout = GetKeyboardLayout(targetThreadId ? targetThreadId : 0);

    int nextIndex = -1;
    for (int i = 0; i < layoutCount; ++i) {
        if (layouts[i] == currentLayout) {
            nextIndex = (i + 1) % layoutCount;
            break;
        }
    }

    if (nextIndex < 0) {
        Wh_Log(L"Current input language not found");
        return false;
    }

    *nextLayout = layouts[nextIndex];
    return true;
}

void RequestNextInputSource() {
    HWND targetWindow = GetInputTargetWindow();
    if (!targetWindow) {
        Wh_Log(L"No foreground window for input language switch");
        return;
    }

    HKL nextLayout = nullptr;
    if (!TryGetNextKeyboardLayout(targetWindow, &nextLayout)) {
        Wh_Log(L"No next input language available");
        return;
    }

    if (!PostMessageW(targetWindow, WM_INPUTLANGCHANGEREQUEST,
                      INPUTLANGCHANGE_FORWARD,
                      reinterpret_cast<LPARAM>(nextLayout))) {
        Wh_Log(L"WM_INPUTLANGCHANGEREQUEST failed: %lu", GetLastError());
    }
}

void SendRealCapsLockToggle() {
    INPUT inputs[2] = {};

    inputs[0].type = INPUT_KEYBOARD;
    inputs[0].ki.wVk = VK_CAPITAL;
    inputs[0].ki.dwExtraInfo = kInjectedCapsExtraInfo;

    inputs[1].type = INPUT_KEYBOARD;
    inputs[1].ki.wVk = VK_CAPITAL;
    inputs[1].ki.dwFlags = KEYEVENTF_KEYUP;
    inputs[1].ki.dwExtraInfo = kInjectedCapsExtraInfo;

    if (SendInput(2, inputs, sizeof(INPUT)) != 2) {
        Wh_Log(L"SendInput for Caps Lock failed: %lu", GetLastError());
    }
}

// Must only be called on the worker thread.
void StopLongPressTimer() {
    if (g_longPressTimerId) {
        KillTimer(nullptr, g_longPressTimerId);
        g_longPressTimerId = 0;
    }
}

// Must only be called on the worker thread.
void TriggerLongPressIfNeeded() {
    if (!g_capsIsDown || g_capsLongPressTriggered) {
        return;
    }

    g_capsLongPressTriggered = true;
    StopLongPressTimer();
    SendRealCapsLockToggle();
}

// Must only be called on the worker thread.
void StartLongPressTimer() {
    StopLongPressTimer();

    int longPressMs = g_longPressMs.load(std::memory_order_relaxed);
    g_longPressTimerId = SetTimer(nullptr, 0, static_cast<UINT>(longPressMs),
                                  nullptr);
    if (!g_longPressTimerId) {
        Wh_Log(L"SetTimer failed: %lu", GetLastError());
    }
}

bool IsOwnInjectedCapsEvent(const KBDLLHOOKSTRUCT* keyboardInfo) {
    return (keyboardInfo->flags & LLKHF_INJECTED) &&
           keyboardInfo->dwExtraInfo == kInjectedCapsExtraInfo;
}

bool IsPassThroughForeground() {
    if (g_passThroughList.empty()) {
        return false;
    }

    HWND foreground = GetForegroundWindow();
    if (!foreground) {
        return false;
    }

    DWORD pid = 0;
    GetWindowThreadProcessId(foreground, &pid);
    if (!pid) {
        return false;
    }

    HANDLE hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!hProcess) {
        return false;
    }

    WCHAR name[MAX_PATH] = {};
    DWORD size = ARRAYSIZE(name);
    QueryFullProcessImageNameW(hProcess, 0, name, &size);
    CloseHandle(hProcess);

    PCWSTR fileName = wcsrchr(name, L'\\');
    fileName = fileName ? fileName + 1 : name;

    for (const auto& token : g_passThroughList) {
        if (lstrcmpiW(fileName, token.c_str()) == 0) {
            return true;
        }
    }

    return false;
}

LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode != HC_ACTION) {
        return CallNextHookEx(g_keyboardHook, nCode, wParam, lParam);
    }

    KBDLLHOOKSTRUCT* keyboardInfo = reinterpret_cast<KBDLLHOOKSTRUCT*>(lParam);
    if (!keyboardInfo) {
        return CallNextHookEx(g_keyboardHook, nCode, wParam, lParam);
    }

    if (keyboardInfo->vkCode != VK_CAPITAL) {
        return CallNextHookEx(g_keyboardHook, nCode, wParam, lParam);
    }

    if (IsOwnInjectedCapsEvent(keyboardInfo)) {
        return CallNextHookEx(g_keyboardHook, nCode, wParam, lParam);
    }

    if (IsPassThroughForeground()) {
        return CallNextHookEx(g_keyboardHook, nCode, wParam, lParam);
    }

    bool isKeyDown = wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN;
    bool isKeyUp = wParam == WM_KEYUP || wParam == WM_SYSKEYUP;

    if (isKeyDown) {
        if (!g_capsIsDown) {
            g_capsIsDown = true;
            g_capsLongPressTriggered = false;
            StartLongPressTimer();
        }

        return 1;
    }

    if (isKeyUp) {
        if (g_capsIsDown) {
            StopLongPressTimer();

            DWORD workerThreadId = g_workerThreadId.load(std::memory_order_acquire);
            if (!g_capsLongPressTriggered && workerThreadId) {
                if (!PostThreadMessageW(workerThreadId, kSwitchInputSourceMessage, 0,
                                   0)) {
                    Wh_Log(L"PostThreadMessageW failed: %lu", GetLastError());
                }
            }
        }

        g_capsIsDown = false;
        g_capsLongPressTriggered = false;
        return 1;
    }

    return 1;
}

HMODULE GetCurrentModuleHandle() {
    HMODULE module = nullptr;
    if (!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                            GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                            reinterpret_cast<LPCWSTR>(&LowLevelKeyboardProc),
                            &module)) {
        Wh_Log(L"GetModuleHandleExW failed: %lu", GetLastError());
    }
    return module;
}

DWORD WINAPI WorkerThreadProc(LPVOID) {
    g_workerThreadId.store(GetCurrentThreadId(), std::memory_order_release);

    MSG msg;
    // Ensure the thread message queue exists before installing the hook,
    // so that PostThreadMessageW from other contexts will not fail.
    PeekMessageW(&msg, nullptr, WM_USER, WM_USER, PM_NOREMOVE);

    g_keyboardHook = SetWindowsHookExW(WH_KEYBOARD_LL, LowLevelKeyboardProc,
                                       GetCurrentModuleHandle(), 0);
    if (!g_keyboardHook) {
        Wh_Log(L"SetWindowsHookExW failed: %lu", GetLastError());
        g_workerThreadId.store(0, std::memory_order_release);
        return 1;
    }

    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        if (msg.message == kSwitchInputSourceMessage) {
            RequestNextInputSource();
        } else if (msg.message == WM_TIMER &&
                   msg.wParam == static_cast<WPARAM>(g_longPressTimerId.load())) {
            TriggerLongPressIfNeeded();
        } else {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
    }

    StopLongPressTimer();

    if (g_keyboardHook) {
        UnhookWindowsHookEx(g_keyboardHook);
        g_keyboardHook = nullptr;
    }

    g_workerThreadId.store(0, std::memory_order_release);
    return 0;
}

BOOL WhTool_ModInit() {
    LoadSettings();

    g_workerThread = CreateThread(nullptr, 0, WorkerThreadProc, nullptr, 0,
                                  nullptr);
    if (!g_workerThread) {
        Wh_Log(L"CreateThread failed: %lu", GetLastError());
        return FALSE;
    }

    return TRUE;
}

void WhTool_ModSettingsChanged() {
    LoadSettings();
}

void WhTool_ModUninit() {
    DWORD workerThreadId = g_workerThreadId.load(std::memory_order_acquire);
    if (workerThreadId) {
        if (!PostThreadMessageW(workerThreadId, WM_QUIT, 0, 0)) {
            Wh_Log(L"PostThreadMessageW(WM_QUIT) failed: %lu", GetLastError());
        }
    }

    if (g_workerThread) {
        DWORD waitResult = WaitForSingleObject(g_workerThread, 5000);
        if (waitResult == WAIT_TIMEOUT) {
            Wh_Log(L"Worker thread did not exit in time");
            // Safe here because ExitProcess(0) follows in Wh_ModUninit,
            // which will clean up all thread resources.
            TerminateThread(g_workerThread, 1);
        }
        CloseHandle(g_workerThread);
        g_workerThread = nullptr;
    }
}

// Prevents the tool mod child process from running its normal entry point.
// The worker thread keeps running; this effectively makes the process a
// container for our keyboard hook logic.
void WINAPI EntryPoint_Hook() {
    ExitThread(0);
}

BOOL Wh_ModInit() {
    DWORD sessionId;
    if (ProcessIdToSessionId(GetCurrentProcessId(), &sessionId) &&
        sessionId == 0) {
        return FALSE;
    }

    bool isExcluded = false;
    bool isCurrentToolModProcess = false;
    bool isAnyToolModProcess = false;

    int argc;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);
    if (!argv) {
        Wh_Log(L"CommandLineToArgvW failed");
        return FALSE;
    }

    for (int i = 1; i < argc; ++i) {
        if (wcscmp(argv[i], L"-service") == 0 ||
            wcscmp(argv[i], L"-service-start") == 0 ||
            wcscmp(argv[i], L"-service-stop") == 0) {
            isExcluded = true;
            break;
        }
    }

    for (int i = 1; i < argc - 1; ++i) {
        if (wcscmp(argv[i], L"-tool-mod") == 0) {
            isAnyToolModProcess = true;
            isCurrentToolModProcess = wcscmp(argv[i + 1], WH_MOD_ID) == 0;
            break;
        }
    }

    LocalFree(argv);

    if (isExcluded) {
        return FALSE;
    }

    if (isCurrentToolModProcess) {
        g_processRole = ProcessRole::kToolMod;

        g_toolModProcessMutex =
            CreateMutexW(nullptr, TRUE, L"windhawk-tool-mod_" WH_MOD_ID);
        if (!g_toolModProcessMutex) {
            Wh_Log(L"CreateMutexW failed: %lu", GetLastError());
            ExitProcess(1);
        }

        if (GetLastError() == ERROR_ALREADY_EXISTS) {
            Wh_Log(L"Tool mod already running (%s)", WH_MOD_ID);
            CloseHandle(g_toolModProcessMutex);
            g_toolModProcessMutex = nullptr;
            ExitProcess(1);
        }

        if (!WhTool_ModInit()) {
            CloseHandle(g_toolModProcessMutex);
            g_toolModProcessMutex = nullptr;
            ExitProcess(1);
        }

        IMAGE_DOS_HEADER* dosHeader =
            reinterpret_cast<IMAGE_DOS_HEADER*>(GetModuleHandleW(nullptr));
        IMAGE_NT_HEADERS* ntHeaders = reinterpret_cast<IMAGE_NT_HEADERS*>(
            reinterpret_cast<BYTE*>(dosHeader) + dosHeader->e_lfanew);

        DWORD entryPointRva = ntHeaders->OptionalHeader.AddressOfEntryPoint;
        void* entryPoint = reinterpret_cast<BYTE*>(dosHeader) + entryPointRva;

        Wh_SetFunctionHook(entryPoint, reinterpret_cast<void*>(EntryPoint_Hook),
                           nullptr);
        return TRUE;
    }

    if (isAnyToolModProcess) {
        return FALSE;
    }

    LoadSettings();

    if (IsCurrentProcessWindhawk()) {
        g_processRole = ProcessRole::kLauncher;
        return TRUE;
    }

    return FALSE;
}

void Wh_ModAfterInit() {
    if (g_processRole != ProcessRole::kLauncher) {
        return;
    }

    WCHAR currentProcessPath[MAX_PATH];
    DWORD pathLength =
        GetModuleFileNameW(nullptr, currentProcessPath, ARRAYSIZE(currentProcessPath));
    if (pathLength == 0 || pathLength == ARRAYSIZE(currentProcessPath)) {
        Wh_Log(L"GetModuleFileNameW failed: %lu", GetLastError());
        return;
    }

    WCHAR commandLine[MAX_PATH * 2];
    int written = swprintf_s(commandLine, ARRAYSIZE(commandLine),
                           L"\"%s\" -tool-mod \"%s\"", currentProcessPath,
                           WH_MOD_ID);
    if (written <= 0) {
        Wh_Log(L"Command line formatting failed");
        return;
    }

    HMODULE kernelModule = GetModuleHandleW(L"kernelbase.dll");
    if (!kernelModule) {
        kernelModule = GetModuleHandleW(L"kernel32.dll");
    }

    if (!kernelModule) {
        Wh_Log(L"No kernelbase.dll/kernel32.dll");
        return;
    }

    using CreateProcessInternalW_t = BOOL(WINAPI*)(
        HANDLE hUserToken, LPCWSTR lpApplicationName, LPWSTR lpCommandLine,
        LPSECURITY_ATTRIBUTES lpProcessAttributes,
        LPSECURITY_ATTRIBUTES lpThreadAttributes, BOOL bInheritHandles,
        DWORD dwCreationFlags, LPVOID lpEnvironment, LPCWSTR lpCurrentDirectory,
        LPSTARTUPINFOW lpStartupInfo,
        LPPROCESS_INFORMATION lpProcessInformation,
        PHANDLE hRestrictedUserToken);

    auto createProcessInternalW = reinterpret_cast<CreateProcessInternalW_t>(
        GetProcAddress(kernelModule, "CreateProcessInternalW"));
    if (!createProcessInternalW) {
        Wh_Log(L"No CreateProcessInternalW");
        return;
    }

    STARTUPINFOW startupInfo = {sizeof(startupInfo)};
    startupInfo.dwFlags = STARTF_FORCEOFFFEEDBACK;

    PROCESS_INFORMATION processInformation = {};
    if (!createProcessInternalW(nullptr, currentProcessPath, commandLine,
                                nullptr, nullptr, FALSE, NORMAL_PRIORITY_CLASS,
                                nullptr, nullptr, &startupInfo,
                                &processInformation, nullptr)) {
        Wh_Log(L"CreateProcessInternalW failed: %lu", GetLastError());
        return;
    }

    CloseHandle(processInformation.hProcess);
    CloseHandle(processInformation.hThread);
}

BOOL Wh_ModSettingsChanged(BOOL* bReload) {
    if (g_processRole == ProcessRole::kLauncher) {
        return TRUE;
    }

    if (g_processRole == ProcessRole::kToolMod) {
        WhTool_ModSettingsChanged();
        return TRUE;
    }

    return TRUE;
}

void Wh_ModUninit() {
    if (g_processRole == ProcessRole::kLauncher) {
        return;
    }

    if (g_processRole == ProcessRole::kToolMod) {
        WhTool_ModUninit();

        if (g_toolModProcessMutex) {
            CloseHandle(g_toolModProcessMutex);
            g_toolModProcessMutex = nullptr;
        }

        // ExitProcess is necessary here because the original entry point
        // was hooked with EntryPoint_Hook, so the process has no normal
        // exit path.
        ExitProcess(0);
    }
}
