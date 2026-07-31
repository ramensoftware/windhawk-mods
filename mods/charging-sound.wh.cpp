// ==WindhawkMod==
// @id              charging-sound
// @name            Charging Sound
// @description     Plays a custom sound when the laptop is plugged in or unplugged
// @version         1.5
// @author          khonloi
// @github          https://github.com/khonloi
// @license         MIT
// @include         windhawk.exe
// @compilerOptions -lwinmm -luuid -lshell32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Charging Sound
Plays a custom `.wav` sound whenever you connect your laptop to a charger (AC
power) or unplug it. By default it plays the Windows Notify Messaging sound.

Please note that this mod only works on devices with a battery (e.g. laptops).
*/
// ==/WindhawkModReadme==

// clang-format off
// ==WindhawkModSettings==
/*
- pluggedInSoundPath: "Notification.IM"
  $name: Plugged-in Sound File Path
  $description: "The absolute path to a .wav file or a system sound alias (e.g., Notification.IM) to play when plugged in. Leave empty to disable."
- unpluggedSoundPath: ""
  $name: Unplugged Sound File Path
  $description: "The absolute path to a .wav file or a system sound alias to play when unplugged. Leave empty to disable."
- suppressWhenBusy: true
  $name: Don't play while busy
  $description: "Skip the sound when a full-screen app is running, presentation mode is active, or notifications are otherwise suppressed."
*/
// ==/WindhawkModSettings==
// clang-format on

#include <windows.h>

#include <mmsystem.h>
#include <shellapi.h>

#include <cstring>
#include <mutex>
#include <string>
#include <utility>

#include <windhawk_utils.h>

struct {
    std::wstring pluggedInSoundPath;
    std::wstring unpluggedSoundPath;
    bool suppressWhenBusy;
    std::mutex mtx;
} g_settings;

HANDLE g_hThread = NULL;
DWORD g_threadId = 0;
HANDLE g_readyEvent = NULL;

// -1 = unknown, so the first notification only establishes the baseline.
int g_lastPowerCondition = -1;

void PlaySoundPath(const std::wstring& path) {
    if (path.empty()) {
        return;
    }

    bool suppress = false;
    {
        std::lock_guard<std::mutex> lock(g_settings.mtx);
        suppress = g_settings.suppressWhenBusy;
    }

    if (suppress) {
        QUERY_USER_NOTIFICATION_STATE quns;
        if (SUCCEEDED(SHQueryUserNotificationState(&quns)) &&
            quns != QUNS_ACCEPTS_NOTIFICATIONS && quns != QUNS_NOT_PRESENT) {
            Wh_Log(L"Notifications are suppressed (state %d), skipping sound.",
                   quns);
            return;
        }
    }

    Wh_Log(L"Playing sound: %s", path.c_str());

    bool isFilePath = path.find(L"\\") != std::wstring::npos ||
                      path.find(L":") != std::wstring::npos;
    DWORD flags = SND_ASYNC | SND_NODEFAULT;

    if (isFilePath) {
        if (!PlaySoundW(path.c_str(), nullptr, flags | SND_FILENAME)) {
            Wh_Log(L"PlaySoundW failed for: %s", path.c_str());
        }
    } else {
        if (!PlaySoundW(path.c_str(), nullptr, flags | SND_ALIAS | SND_SYSTEM)) {
            Wh_Log(L"PlaySoundW failed for: %s", path.c_str());
        }
    }
}

LRESULT CALLBACK PowerWindowProc(HWND hwnd,
                                 UINT uMsg,
                                 WPARAM wParam,
                                 LPARAM lParam) {
    if (uMsg == WM_POWERBROADCAST) {
        if (wParam == PBT_POWERSETTINGCHANGE) {
            POWERBROADCAST_SETTING* pbs = (POWERBROADCAST_SETTING*)lParam;
            if (pbs->PowerSetting == GUID_ACDC_POWER_SOURCE &&
                pbs->DataLength == sizeof(DWORD)) {
                DWORD currentACStatus;
                std::memcpy(&currentACStatus, pbs->Data, sizeof(DWORD));

                int state = (currentACStatus == PoAc) ? PoAc : PoDc;
                int previous = std::exchange(g_lastPowerCondition, state);
                if (previous == state || previous == -1) {
                    return TRUE;
                }

                Wh_Log(
                    L"Power status change detected. Current Power Condition: "
                    L"%u",
                    currentACStatus);

                if (state == PoAc) {  // Plugged in (AC)
                    std::wstring currentSoundPath;
                    {
                        std::lock_guard<std::mutex> lock(g_settings.mtx);
                        currentSoundPath = g_settings.pluggedInSoundPath;
                    }

                    Wh_Log(L"Laptop plugged in.");
                    PlaySoundPath(currentSoundPath);
                } else if (state == PoDc) {  // Unplugged (DC or UPS backup)
                    std::wstring currentUnpluggedSoundPath;
                    {
                        std::lock_guard<std::mutex> lock(g_settings.mtx);
                        currentUnpluggedSoundPath =
                            g_settings.unpluggedSoundPath;
                    }

                    Wh_Log(L"Laptop unplugged.");
                    PlaySoundPath(currentUnpluggedSoundPath);
                }
            }
        }
        return TRUE;
    }
    return DefWindowProcW(hwnd, uMsg, wParam, lParam);
}

DWORD WINAPI PowerNotifyThread(LPVOID lpParam) {
    Wh_Log(L"PowerNotifyThread started.");

    SYSTEM_POWER_STATUS sps;
    if (GetSystemPowerStatus(&sps) && sps.ACLineStatus != 255) {
        g_lastPowerCondition = sps.ACLineStatus == 1 ? PoAc : PoDc;
    }

    MSG msg;
    // Ensure the thread message queue exists, so PostThreadMessageW never
    // fails.
    PeekMessageW(&msg, nullptr, WM_USER, WM_USER, PM_NOREMOVE);
    SetEvent(g_readyEvent);

    WNDCLASSEXW wc = {0};
    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.lpfnWndProc = PowerWindowProc;
    // Executable's (windhawk.exe) module handle
    wc.hInstance = GetModuleHandleW(NULL);
    wc.lpszClassName = L"WindhawkChargingSoundHiddenWindow";

    if (!RegisterClassExW(&wc)) {
        Wh_Log(L"RegisterClassExW failed with error: %u", GetLastError());
        return 1;
    }

    HWND hwnd =
        CreateWindowExW(0, wc.lpszClassName, L"WindhawkChargingSound", 0, 0, 0,
                        0, 0, HWND_MESSAGE, NULL, wc.hInstance, NULL);

    if (hwnd) {
        Wh_Log(L"Hidden window created successfully.");

        HPOWERNOTIFY hPowerNotify = RegisterPowerSettingNotification(
            hwnd, &GUID_ACDC_POWER_SOURCE, DEVICE_NOTIFY_WINDOW_HANDLE);

        if (!hPowerNotify) {
            Wh_Log(L"RegisterPowerSettingNotification failed with error: %u",
                   GetLastError());
        }

        while (GetMessageW(&msg, NULL, 0, 0) > 0) {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }

        if (hPowerNotify) {
            UnregisterPowerSettingNotification(hPowerNotify);
        }

        DestroyWindow(hwnd);
    } else {
        Wh_Log(L"CreateWindowExW failed with error: %u", GetLastError());
    }

    UnregisterClassW(wc.lpszClassName, wc.hInstance);
    Wh_Log(L"PowerNotifyThread exited.");
    return 0;
}

void LoadSettings() {
    auto pluggedInSoundPath =
        WindhawkUtils::StringSetting::make(L"pluggedInSoundPath");
    auto unpluggedSoundPath =
        WindhawkUtils::StringSetting::make(L"unpluggedSoundPath");
    bool suppressWhenBusy = Wh_GetIntSetting(L"suppressWhenBusy") != 0;

    std::lock_guard<std::mutex> lock(g_settings.mtx);
    g_settings.pluggedInSoundPath = pluggedInSoundPath.get();
    g_settings.unpluggedSoundPath = unpluggedSoundPath.get();
    g_settings.suppressWhenBusy = suppressWhenBusy;
}

BOOL WhTool_ModInit() {
    Wh_Log(L"Init");
    LoadSettings();
    g_readyEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_readyEvent) {
        Wh_Log(L"Failed to create ready event: %u", GetLastError());
        return FALSE;
    }
    g_hThread = CreateThread(NULL, 0, PowerNotifyThread, NULL, 0, &g_threadId);
    if (!g_hThread) {
        Wh_Log(L"Failed to create thread: %u", GetLastError());
        CloseHandle(g_readyEvent);
        g_readyEvent = NULL;
        return FALSE;
    }
    return TRUE;
}

void WhTool_ModUninit() {
    Wh_Log(L"Uninit");

    // Stop any playing sound
    PlaySoundW(nullptr, nullptr, 0);

    if (g_threadId != 0 && g_hThread != NULL) {
        if (g_readyEvent) {
            WaitForSingleObject(g_readyEvent, INFINITE);
        }

        PostThreadMessageW(g_threadId, WM_QUIT, 0, 0);

        WaitForSingleObject(g_hThread, INFINITE);
        CloseHandle(g_hThread);
        g_hThread = NULL;
        g_threadId = 0;
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

// clang-format off
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
// clang-format on
