// ==WindhawkMod==
// @id                better-uwp-clean
// @name            Clean Up UWP Processes (Improved)
// @description    Automatically end UWP processes when no UWP apps are open, except for shell-critical processes.
// @version         0.1
// @author          Smlfrysamuri
// @github          https://github.com/smlfrysamuri
// @include         ApplicationFrameHost.exe
// @include         TextInputHost.exe
// @include         SystemSettingsAdminFlows.exe
// @include         ctfmon.exe
// @compilerOptions -lcomdlg32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# (Better) UWP Clean
This is a slightly modified version of the UWP Clean mod created by https://github.com/arukateru. Whereas the
original mod operated in such a way that caused RuntimeBroker.exe to exit, this mod still disables UWP processes
without killing RuntimeBroker. This preserves shell-critical processes that use UWP, most importantly, the ability to 
right-click taskbar menu buttons and access their context menus, which relies on RuntimeBroker.
*/
// ==/WindhawkModReadme==

#include <Windows.h>
#include <string>
#include <thread>
#include <TlHelp32.h>

bool IsProcessRunning(const std::wstring& processName) {
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnap == INVALID_HANDLE_VALUE) return false;

    PROCESSENTRY32W pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32W);
    if (!Process32FirstW(hSnap, &pe32)) {
        CloseHandle(hSnap);
        return false;
    }

    do {
        if (processName == pe32.szExeFile) {
            CloseHandle(hSnap);
            return true;
        }
    } while (Process32NextW(hSnap, &pe32));

    CloseHandle(hSnap);
    return false;
}

// Terminate a specific process by name instead of the current host process
void TerminateProcessByName(const std::wstring& processName) {
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnap == INVALID_HANDLE_VALUE) return;

    PROCESSENTRY32W pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32W);
    if (!Process32FirstW(hSnap, &pe32)) {
        CloseHandle(hSnap);
        return;
    }

    do {
        if (processName == pe32.szExeFile) {
            HANDLE hProc = OpenProcess(PROCESS_TERMINATE, FALSE, pe32.th32ProcessID);
            if (hProc) {
                TerminateProcess(hProc, 0);
                CloseHandle(hProc);
            }
        }
    } while (Process32NextW(hSnap, &pe32));

    CloseHandle(hSnap);
}

// Get the name of the current process
std::wstring GetCurrentProcessName() {
    wchar_t path[MAX_PATH];
    GetModuleFileNameW(NULL, path, MAX_PATH);
    std::wstring fullPath(path);
    size_t pos = fullPath.rfind(L'\\');
    return (pos != std::wstring::npos) ? fullPath.substr(pos + 1) : fullPath;
}

void CheckProcesses() {
    std::wstring currentProc = GetCurrentProcessName();

    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(15));

        bool systemSettingsRunning = IsProcessRunning(L"SystemSettings.exe");
        bool wduiRunning           = IsProcessRunning(L"SecHealthUI.exe");
        bool storeRunning          = IsProcessRunning(L"WinStore.App.exe");

        if (!systemSettingsRunning && !wduiRunning && !storeRunning) {
            // Only self-terminate safe, non-shell-critical processes.
            // Never terminate RuntimeBroker.exe — it backs taskbar context menus.
            if (currentProc == L"ApplicationFrameHost.exe" ||
                currentProc == L"TextInputHost.exe"        ||
                currentProc == L"SystemSettingsAdminFlows.exe" ||
                currentProc == L"ctfmon.exe") {
                ExitProcess(0);
            }
            // For any other included process, do nothing (safe fallback)
        }
    }
}

BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved
) {
    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:
        std::thread(CheckProcesses).detach();
        break;
    default:
        break;
    }
    return TRUE;
}