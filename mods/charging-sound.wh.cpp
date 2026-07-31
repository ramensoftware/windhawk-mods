// ==WindhawkMod==
// @id              charging-sound
// @name            Charging Sound
// @description     Plays a custom sound when the laptop is plugged in or unplugged
// @version         1.2
// @author          khonloi
// @github          https://github.com/khonloi
// @license         MIT
// @include         windhawk.exe
// @compilerOptions -lwinmm -luuid
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Charging Sound
Plays a custom `.wav` sound whenever you connect your laptop to a charger (AC
power) or unplug it. By default it plays the Windows Notify Messaging sound.

Please note that this mod only works on devices with a battery (e.g. laptops).
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- soundPath: "Notification.IM"
  $name: Plugged-in Sound File Path
  $description: "The absolute path to a .wav file or a system sound alias (e.g.,
Notification.IM) to play when plugged in."
- unpluggedSoundPath: ""
  $name: Unplugged Sound File Path
  $description: "The absolute path to a .wav file or a system sound alias to
play when unplugged. Leave empty to disable."
*/
// ==/WindhawkModSettings==

#include <mmsystem.h>
#include <windows.h>
#include <mutex>
#include <string>

#include <windhawk_utils.h>

struct {
    std::wstring soundPath;
    std::wstring unpluggedSoundPath;
    std::mutex mtx;
} settings;

HWND g_hwnd = NULL;
HANDLE g_hThread = NULL;
DWORD g_threadId = 0;
HANDLE g_readyEvent = NULL;

bool EndsWithWav(const std::wstring& path) {
    if (path.length() >= 4) {
        std::wstring ext = path.substr(path.length() - 4);
        return _wcsicmp(ext.c_str(), L".wav") == 0;
    }
    return false;
}

void PlaySoundPath(const std::wstring& path) {
    if (path.empty()) {
        return;
    }

    bool isFilePath = EndsWithWav(path);
    DWORD flags =
        SND_ASYNC | SND_NODEFAULT | (isFilePath ? SND_FILENAME : SND_ALIAS);

    if (!PlaySoundW(path.c_str(), nullptr, flags)) {
        Wh_Log(L"PlaySoundW failed for: %s", path.c_str());
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
                pbs->DataLength == sizeof(int)) {
                int currentACStatus = *(int*)pbs->Data;

                Wh_Log(
                    L"Power status change detected. Current ACLineStatus: %d",
                    currentACStatus);

                if (currentACStatus == 0) {  // Plugged in (AC)
                    std::wstring currentSoundPath;
                    {
                        std::lock_guard<std::mutex> lock(settings.mtx);
                        currentSoundPath = settings.soundPath;
                    }

                    Wh_Log(L"Laptop plugged in. Playing sound: %s",
                           currentSoundPath.c_str());
                    PlaySoundPath(currentSoundPath);
                } else if (currentACStatus == 1) {  // Unplugged (DC)
                    std::wstring currentUnpluggedSoundPath;
                    {
                        std::lock_guard<std::mutex> lock(settings.mtx);
                        currentUnpluggedSoundPath = settings.unpluggedSoundPath;
                    }

                    Wh_Log(L"Laptop unplugged. Playing sound: %s",
                           currentUnpluggedSoundPath.c_str());
                    PlaySoundPath(currentUnpluggedSoundPath);
                }
            }
        }
    }
    return DefWindowProcW(hwnd, uMsg, wParam, lParam);
}

DWORD WINAPI PowerNotifyThread(LPVOID lpParam) {
    Wh_Log(L"PowerNotifyThread started.");

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

    g_hwnd = CreateWindowExW(0, wc.lpszClassName, L"WindhawkChargingSound",
                             WS_OVERLAPPED, 0, 0, 0, 0, NULL, NULL,
                             wc.hInstance, NULL);

    if (g_hwnd) {
        Wh_Log(L"Hidden window created successfully.");

        HPOWERNOTIFY hPowerNotify = RegisterPowerSettingNotification(
            g_hwnd, &GUID_ACDC_POWER_SOURCE, DEVICE_NOTIFY_WINDOW_HANDLE);

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
    auto unpluggedSoundPath =
        WindhawkUtils::StringSetting::make(L"unpluggedSoundPath");
    std::lock_guard<std::mutex> lock(settings.mtx);
    settings.soundPath = *soundPath ? soundPath.get() : L"Notification.IM";
    settings.unpluggedSoundPath =
        *unpluggedSoundPath ? unpluggedSoundPath.get() : L"";
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

        PostThreadMessageW(g_threadId, WM_QUIT, 0, 0);

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
