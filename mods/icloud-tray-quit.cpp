// ==WindhawkMod==
// @id              icloud-tray-quit
// @name            iCloud Tray Quit Button
// @description     Adds a Quit option to the iCloud system tray menu and kills iCloud background processes.
// @version         1.0.0
// @author          Lautner
// @github          https://github.com/Iautner
// @homepage        https://lautner.co/
// @include         iCloudHome.exe
// @license         MIT
// @architecture    x86-64
// ==/WindhawkMod==

#include <windows.h>
#include <tlhelp32.h>
#include <windhawk_utils.h>

#define IDM_QUIT_ICLOUD 42069 

decltype(&TrackPopupMenuEx) TrackPopupMenuEx_Original;

void KillProcessByName(const wchar_t* processName) {
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnap != INVALID_HANDLE_VALUE) {
        PROCESSENTRY32W pe32;
        pe32.dwSize = sizeof(PROCESSENTRY32W);
        if (Process32FirstW(hSnap, &pe32)) {
            do {
                if (_wcsicmp(pe32.szExeFile, processName) == 0) {
                    HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, pe32.th32ProcessID);
                    if (hProcess) {
                        TerminateProcess(hProcess, 0);
                        CloseHandle(hProcess);
                    }
                }
            } while (Process32NextW(hSnap, &pe32));
        }
        CloseHandle(hSnap);
    }
}

BOOL WINAPI TrackPopupMenuEx_Hook(
    HMENU hMenu, UINT fuFlags, int x, int y, HWND hwnd, LPTPMPARAMS lptpm
) {
    AppendMenuW(hMenu, MF_SEPARATOR, 0, NULL);
    AppendMenuW(hMenu, MF_STRING, IDM_QUIT_ICLOUD, L"Quit iCloud");

    UINT modifiedFlags = fuFlags | TPM_RETURNCMD;

    int clickedId = TrackPopupMenuEx_Original(hMenu, modifiedFlags, x, y, hwnd, lptpm);

    if (clickedId == IDM_QUIT_ICLOUD) {
        Wh_Log(L"Quit iCloud clicked. Terminating core ecosystem...");
        
        KillProcessByName(L"iCloudDrive.exe");
        KillProcessByName(L"iCloudCKKS.exe");
        KillProcessByName(L"iCloudServices.exe"); 
        
        ExitProcess(0);
    }

    if (clickedId != 0 && !(fuFlags & TPM_RETURNCMD)) {
        PostMessageW(hwnd, WM_COMMAND, MAKEWPARAM(clickedId, 0), (LPARAM)hwnd);
    }

    return clickedId;
}

BOOL Wh_ModInit() {
    HMODULE hUser32 = GetModuleHandle(L"user32.dll");
    if (!hUser32) return FALSE;

    void* pTrackPopupMenuEx = (void*)GetProcAddress(hUser32, "TrackPopupMenuEx");
    if (pTrackPopupMenuEx) {
        Wh_SetFunctionHook(pTrackPopupMenuEx, (void*)TrackPopupMenuEx_Hook, (void**)&TrackPopupMenuEx_Original);
        return TRUE;
    }

    return FALSE;
}