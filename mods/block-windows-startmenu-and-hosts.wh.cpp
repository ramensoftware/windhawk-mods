// ==WindhawkMod==
// @id              block-windows-startmenu-and-hosts
// @name            Block Start Menu and Hosts
// @description     Kills existing selected UI components on mod init and blocks any future launches.
// @version         1.2
// @author          Exiled Eye
// @github          https://github.com/ExiledEye
// @homepage        https://exiledeye.github.io/
// @include         explorer.exe
// @include         svchost.exe
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Block Start Menu and Hosts
 
This Windhawk mod actively terminates existing instances of specific Windows host processes upon loading and intercepts process creation to prevent them from launching again in the future. 
This way the mod ensures that unused selected UI components and their process are actually not running instead of simply hiding them.
 
## Features
You can individually toggle the blocking of the following processes dynamically in the mod settings:
*   **StartMenuExperienceHost.exe** -> The main Start Menu UI and process ("WIN" key)
*   **SearchHost.exe**              -> The Search Menu UI and process ("WIN + S" shortcut) **Note**: _may_ cause issues within folder search.
*   **TextInputHost.exe**           -> The emoji and clipboard UI and process ("WIN + ." shortcut)
## How it Works
The mod injects into `explorer.exe` and `svchost.exe`.  
Upon init, it kills any active instances of the selected hosts.  
It then places a hook on the `CreateProcessInternalW` function: If the system attempts to launch any of the blocked .exe (example: user press WIN key), the hook intercepts the request and returns an `ERROR_ACCESS_DENIED` flag, preventing the launch.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- BlockStartMenu: true
  $name: Block StartMenuExperienceHost.exe
  $description: Kills existing instances and blocks future launches of the Start Menu host.
- BlockSearch: true
  $name: Block SearchHost.exe
  $description: Kills existing instances and blocks future launches of the Search host.
- BlockTextInput: false
  $name: Block TextInputHost.exe
  $description: Kills existing instances and blocks future launches of the Text Input host.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <windhawk_api.h>
#include <tlhelp32.h>

// Global variables
bool g_blockStartMenu = true;
bool g_blockSearch = true;
bool g_blockTextInput = false;

// Settings Loader
void LoadSettings() {
    g_blockStartMenu = Wh_GetIntSetting(L"BlockStartMenu");
    g_blockSearch = Wh_GetIntSetting(L"BlockSearch");
    g_blockTextInput = Wh_GetIntSetting(L"BlockTextInput");
}

// Helper for process killing
void TerminateProcessByName(const wchar_t* filename) {
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnap == INVALID_HANDLE_VALUE) return;

    PROCESSENTRY32W pe;
    pe.dwSize = sizeof(pe);

    if (Process32FirstW(hSnap, &pe)) {
        do {
            if (wcsicmp(pe.szExeFile, filename) == 0) {
                HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, pe.th32ProcessID);
                if (hProcess) {
                    TerminateProcess(hProcess, 0);
                    CloseHandle(hProcess);
                }
            }
        } while (Process32NextW(hSnap, &pe));
    }
    CloseHandle(hSnap);
}

// Helper to kill currently targeted processes based on settings
void TerminateConfiguredProcesses() {
    if (g_blockStartMenu) {
        TerminateProcessByName(L"StartMenuExperienceHost.exe");
    }
    if (g_blockSearch) {
        TerminateProcessByName(L"SearchHost.exe");
    }
    if (g_blockTextInput) {
        TerminateProcessByName(L"TextInputHost.exe");
    }
}

// Helper for case-insensitive substring search
const wchar_t* wcsistr(const wchar_t* str, const wchar_t* substr) {
    if (!str || !substr) return nullptr;
    size_t len = wcslen(substr);
    if (len == 0) return str;
    while (*str) {
        if (_wcsnicmp(str, substr, len) == 0) return str;
        str++;
    }
    return nullptr;
}

// Helper to prevent further process launches
typedef BOOL(WINAPI *CreateProcessInternalW_t)(
    HANDLE hToken, LPCWSTR lpApplicationName, LPWSTR lpCommandLine,
    LPSECURITY_ATTRIBUTES lpProcessAttributes, LPSECURITY_ATTRIBUTES lpThreadAttributes,
    BOOL bInheritHandles, DWORD dwCreationFlags, LPVOID lpEnvironment,
    LPCWSTR lpCurrentDirectory, LPSTARTUPINFOW lpStartupInfo,
    LPPROCESS_INFORMATION lpProcessInformation, PHANDLE hNewToken
);

CreateProcessInternalW_t CreateProcessInternalW_Original;

BOOL WINAPI CreateProcessInternalW_Hook(
    HANDLE hToken, LPCWSTR lpAppName, LPWSTR lpCmdLine,
    LPSECURITY_ATTRIBUTES lpPA, LPSECURITY_ATTRIBUTES lpTA,
    BOOL bInherit, DWORD dwFlags, LPVOID lpEnv,
    LPCWSTR lpDir, LPSTARTUPINFOW lpSI,
    LPPROCESS_INFORMATION lpPI, PHANDLE hNewToken) 
{
    bool block = false;

    if (g_blockStartMenu && ((lpCmdLine && wcsistr(lpCmdLine, L"StartMenuExperienceHost.exe")) || 
                             (lpAppName && wcsistr(lpAppName, L"StartMenuExperienceHost.exe")))) {
        block = true;
    }
    else if (g_blockSearch && ((lpCmdLine && wcsistr(lpCmdLine, L"SearchHost.exe")) || 
                               (lpAppName && wcsistr(lpAppName, L"SearchHost.exe")))) {
        block = true;
    }
    else if (g_blockTextInput && ((lpCmdLine && wcsistr(lpCmdLine, L"TextInputHost.exe")) || 
                                  (lpAppName && wcsistr(lpAppName, L"TextInputHost.exe")))) {
        block = true;
    }

    if (block) { // Logs if process is being blocked on keypress (if logging is enabled)
        Wh_Log(L"Blocked launch: AppName=%ls, CmdLine=%ls", 
            lpAppName ? lpAppName : L"NULL", 
            lpCmdLine ? lpCmdLine : L"NULL");
        SetLastError(ERROR_ACCESS_DENIED);
        return FALSE;
    }

    return CreateProcessInternalW_Original(hToken, lpAppName, lpCmdLine, lpPA, lpTA, bInherit, dwFlags, lpEnv, lpDir, lpSI, lpPI, hNewToken);
}

// Callback triggered when settings are saved
void Wh_ModSettingsChanged() {
    LoadSettings();
    TerminateConfiguredProcesses();
}

BOOL Wh_ModInit() {
    // 1. Load initial settings and kill running configured processes
    LoadSettings();
    TerminateConfiguredProcesses();

    // 2. Set up the hook to prevent them from ever starting again
    HMODULE hKernelBase = GetModuleHandleW(L"kernelbase.dll");
    if (hKernelBase) {
        void* pCreateProcessInternalW = (void*)GetProcAddress(hKernelBase, "CreateProcessInternalW");
        if (pCreateProcessInternalW) {
            Wh_SetFunctionHook(
                pCreateProcessInternalW,
                (void*)CreateProcessInternalW_Hook,
                (void**)&CreateProcessInternalW_Original
            );
        }
    }
    return TRUE;
}