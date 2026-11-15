// ==WindhawkMod==
// @id              uwp-clean
// @name            Clean up UWP processes
// @description     Automatically end UWP processes when no UWP apps are open.
// @version         0.1
// @author          Alcatel
// @github          https://github.com/arukateru
// @include         ApplicationFrameHost.exe
// @include         RuntimeBroker.exe
// @include         TextInputHost.exe
// @include         SystemSettingsAdminFlows.exe
// @include         ctfmon.exe
// @compilerOptions -lcomdlg32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*

The Universal Windows Platform (UWP), in the spirit of being unoptimized, keep all of their many processes operating all of the time: at the logon of the user, RuntimeBroker automatically restarting, and even when UWP apps are closed. This mod makes ApplicationFrameHost and RuntimeBroker automatically terminate unless Settings or Windows Security are open. You can add to the list of apps to keep the programs open if you so choose.

**If you want to add more apps like WhatsApp etc to this list, you must turn off background processes for the app in the Metro Settings, or the app stays suspended.**

*/
// ==/WindhawkModReadme==

#include <Windows.h>
#include <iostream>
#include <string>
#include <thread>
#include <TlHelp32.h>

bool IsProcessRunning(const std::wstring& processName) {
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnap == INVALID_HANDLE_VALUE) {
        return false;
    }

    PROCESSENTRY32W pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32W);
    if (!Process32FirstW(hSnap, &pe32)) {
        CloseHandle(hSnap);
        return false;
    }

    do {
        if (pe32.szExeFile == processName) {
            CloseHandle(hSnap);
            return true;
        }
    } while (Process32NextW(hSnap, &pe32));

    CloseHandle(hSnap);
    return false;
}

void CheckProcesses() {
    while (true) {
        bool systemSettingsRunning = IsProcessRunning(L"SystemSettings.exe");
        bool wduiRunning = IsProcessRunning(L"SecHealthUI.exe");
        bool storeRunning = IsProcessRunning(L"WinStore.App.exe");


        if (!systemSettingsRunning && !wduiRunning && !storeRunning) {

            ExitProcess(0);
        }

        std::this_thread::sleep_for(std::chrono::seconds(15));
    }
}

BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved
) {
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        std::thread(CheckProcesses).detach();
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}
