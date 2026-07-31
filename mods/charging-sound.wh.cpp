// ==WindhawkMod==
// @id              charging-sound
// @name            Charging Sound
// @description     Plays a custom sound when the laptop is plugged in
// @version         1.1
// @author          khonloi
// @github          https://github.com/khonloi
// @include         windhawk.exe
// @compilerOptions -lwinmm
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Charging Sound
Plays a custom `.wav` sound whenever you connect your laptop to a charger (AC
power). By default it plays the Windows Notify Messaging sound.

Please note that this mod only works on devices with a battery (e.g. laptops).
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- soundPath: "Notification.Messaging"
  $name: Sound File Path
  $description: "The absolute path to a .wav file or a system sound alias (e.g.,
Notification.Messaging) to play when plugged in."
*/
// ==/WindhawkModSettings==

#include <mmsystem.h>
#include <windows.h>
#include <mutex>
#include <string>

#include <windhawk_utils.h>

struct {
    std::wstring soundPath;
    std::mutex mtx;
} settings;

HWND g_hwnd = NULL;
HANDLE g_hThread = NULL;
DWORD g_threadId = 0;
HANDLE g_readyEvent = NULL;
int g_previousACStatus = -1;

LRESULT CALLBACK PowerWindowProc(HWND hwnd,
                                 UINT uMsg,
                                 WPARAM wParam,
                                 LPARAM lParam) {
    if (uMsg == WM_DESTROY) {
        PostQuitMessage(0);
        return 0;
    }

    if (uMsg == WM_POWERBROADCAST) {
        if (wParam == PBT_APMPOWERSTATUSCHANGE) {
            SYSTEM_POWER_STATUS sps;
            if (GetSystemPowerStatus(&sps)) {
                int currentACStatus = sps.ACLineStatus;

                // Ignore unknown status (255)
                if (currentACStatus == 255) {
                    return DefWindowProcW(hwnd, uMsg, wParam, lParam);
                }

                Wh_Log(
                    L"Power status change detected. Current ACLineStatus: %d",
                    currentACStatus);

                // Only trigger if we have a valid previous state and it
                // actually changed
                if (g_previousACStatus != -1 &&
                    currentACStatus != g_previousACStatus) {
                    if (currentACStatus == 1) {  // Plugged in
                        std::wstring currentSoundPath;
                        {
                            std::lock_guard<std::mutex> lock(settings.mtx);
                            currentSoundPath = settings.soundPath;
                        }

                        Wh_Log(L"Laptop plugged in. Playing sound: %s",
                               currentSoundPath.c_str());

                        // Treat it as a file if it looks like a path, otherwise
                        // as a system sound alias.
                        bool isFilePath =
                            currentSoundPath.find(L'\\') != std::wstring::npos;
                        if (isFilePath) {
                            PlaySoundW(
                                currentSoundPath.c_str(), nullptr,
                                SND_FILENAME | SND_ASYNC | SND_NODEFAULT);
                        } else {
                            PlaySoundW(currentSoundPath.c_str(), nullptr,
                                       SND_ALIAS | SND_ASYNC | SND_NODEFAULT);
                        }
                    } else if (currentACStatus == 0) {  // Unplugged
                        Wh_Log(L"Laptop unplugged.");
                    }
                }
                g_previousACStatus = currentACStatus;
            }
        }
    }
    return DefWindowProcW(hwnd, uMsg, wParam, lParam);
}

DWORD WINAPI PowerNotifyThread(LPVOID lpParam) {
    Wh_Log(L"PowerNotifyThread started.");

    // Initialize the previous status to the current status so it doesn't play
    // immediately on launch
    SYSTEM_POWER_STATUS sps;
    if (GetSystemPowerStatus(&sps)) {
        if (sps.ACLineStatus != 255) {
            g_previousACStatus = sps.ACLineStatus;
        }
    }

    WNDCLASSEXW wc = {0};
    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.lpfnWndProc = PowerWindowProc;
    // Mod's module handle
    wc.hInstance = GetModuleHandleW(NULL);
    wc.lpszClassName = L"WindhawkChargingSoundHiddenWindow";

    if (!RegisterClassExW(&wc)) {
        Wh_Log(L"RegisterClassExW failed with error: %u", GetLastError());
        return 1;
    }

    g_hwnd = CreateWindowExW(0, wc.lpszClassName, L"WindhawkChargingSound",
                             WS_OVERLAPPED, 0, 0, 0, 0, NULL, NULL,
                             wc.hInstance, NULL);

    if (g_hwnd) {
        Wh_Log(L"Hidden window created successfully.");

        MSG msg;
        // Ensure the thread message queue exists, so PostMessageW never fails.
        PeekMessageW(&msg, nullptr, WM_USER, WM_USER, PM_NOREMOVE);
        SetEvent(g_readyEvent);

        while (GetMessageW(&msg, NULL, 0, 0) > 0) {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }

        DestroyWindow(g_hwnd);
        g_hwnd = NULL;
    } else {
        Wh_Log(L"CreateWindowExW failed with error: %u", GetLastError());
    }

    UnregisterClassW(wc.lpszClassName, wc.hInstance);
    Wh_Log(L"PowerNotifyThread exited.");
    return 0;
}

void LoadSettings() {
    auto soundPath = WindhawkUtils::StringSetting::make(L"soundPath");
    std::lock_guard<std::mutex> lock(settings.mtx);
    settings.soundPath =
        *soundPath ? soundPath.get() : L"Notification.Messaging";
}

bool WhTool_ModInit() {
    Wh_Log(L"Init");
    LoadSettings();
    g_readyEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_readyEvent) {
        Wh_Log(L"Failed to create ready event: %u", GetLastError());
        return false;
    }
    g_hThread = CreateThread(NULL, 0, PowerNotifyThread, NULL, 0, &g_threadId);
    if (!g_hThread) {
        Wh_Log(L"Failed to create thread: %u", GetLastError());
        CloseHandle(g_readyEvent);
        g_readyEvent = NULL;
        return false;
    }
    return true;
}

void WhTool_ModUninit() {
    Wh_Log(L"Uninit");

    // Stop any playing sound
    PlaySoundW(nullptr, nullptr, 0);

    if (g_threadId != 0 && g_hThread != NULL) {
        if (g_readyEvent) {
            WaitForSingleObject(g_readyEvent, INFINITE);
        }

        if (g_hwnd) {
            PostMessageW(g_hwnd, WM_CLOSE, 0, 0);
        }

        WaitForSingleObject(g_hThread, INFINITE);
        CloseHandle(g_hThread);
        g_hThread = NULL;
        g_threadId = 0;
        g_hwnd = NULL;
    }

    if (g_readyEvent) {
        CloseHandle(g_readyEvent);
        g_readyEvent = NULL;
    }
}

void WhTool_ModSettingsChanged() {
    Wh_Log(L"SettingsChanged");
    LoadSettings();
}

// ============================================================================
// Tool Mod Boilerplate
// ============================================================================
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
