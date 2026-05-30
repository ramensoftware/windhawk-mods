// ==WindhawkMod==
// @id              block-windows-startmenu-and-hosts
// @name            Block Start Menu and Hosts
// @description     Kills existing selected UI components on mod init and blocks any future launches.
// @version         1.4
// @author          Exiled Eye
// @github          https://github.com/ExiledEye
// @homepage        https://exiledeye.github.io/
// @include         explorer.exe
// @include         svchost.exe
// @include         ShellHost.exe
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Block Start Menu and Hosts
 
This Windhawk mod actively terminates existing instances of specific Windows host processes upon loading and intercepts process creation to prevent them from launching again in the future. 
This way the mod ensures that unused selected UI components and their process are actually not running instead of simply hiding them.
 
## Features
You can individually toggle the blocking of the following processes dynamically in the mod settings:
*   **StartMenuExperienceHost.exe**     -> The main Start Menu UI and process ("WIN" key).
*   **SearchHost.exe**                  -> The Search Menu UI and process ("WIN + S" and "WIN+ Q" shortcut) **Note**: _may_ cause issues within folder search.
*   **TextInputHost.exe**               -> The Emoji and Clipboard UI and process ("WIN + ." and "WIN + V" shortcuts).
*   **ShellHost.exe**                   -> The Action Center UI and process ("WIN + A" shortcut).
*   **ShellExperienceHost.exe**         -> The Calendar and Notifications UI and process ("WIN + N" shortcut).
*   **MicrosoftStartFeedProvider.exe**  -> The Start Menu process, which provides news, weather, stock quotes, interest cards, recommended and etc.
*   **WidgetBoard.exe**                 -> The Widgets board UI and process ("WIN + W" shortcut).
*   **WidgetService.exe**               -> The Widgets board background data, content and service infrastructure process.

## How it works
The mod injects into `explorer.exe`, `svchost.exe`, and `ShellHost.exe`.  
Upon init, it kills any active instances of the selected hosts.  
It then places a hook on the `CreateProcessInternalW` function: If the system attempts to launch any of the blocked .exe (example: user press WIN key), the hook intercepts the request and returns an `ERROR_ACCESS_DENIED` flag, preventing the launch.  
For `ShellHost.exe` specifically, the mod injects directly into it and calls `ExitProcess` on init if blocking is enabled, since ShellHost respawns itself and cannot be kept dead by the hook alone.

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
- BlockShellHost: false
  $name: Block ShellHost.exe
  $description: Kills existing instances and blocks future launches of Action Center host.
- BlockShellExperienceHost: false
  $name: Block ShellExperienceHost.exe
  $description: Kills existing instances and blocks future launches of Calendar and Notifications host.
- BlockMicrosoftStartFeedProvider: false
  $name: Block MicrosoftStartFeedProvider.exe
  $description: Kills existing instances and blocks future launches of Start Feed Provider host.
- BlockWidgetBoard: false
  $name: Block WidgetBoard.exe
  $description: Kills existing instances and blocks future launches of Widgets host.
- BlockWidgetService: false
  $name: Block WidgetService.exe
  $description: Kills existing instances and blocks future launches of Widgets Service host.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <windhawk_api.h>
#include <tlhelp32.h>

// Global variables
bool g_blockStartMenu = true;
bool g_blockSearch = true;
bool g_blockTextInput = false;
bool g_blockShellHost = false;
bool g_blockShellExperienceHost = false;
bool g_blockMicrosoftStartFeedProvider = false;
bool g_blockWidgetBoard = false;
bool g_blockWidgetService = false;

// Settings Loader
void LoadSettings() {
    g_blockStartMenu                  = Wh_GetIntSetting(L"BlockStartMenu");
    g_blockSearch                     = Wh_GetIntSetting(L"BlockSearch");
    g_blockTextInput                  = Wh_GetIntSetting(L"BlockTextInput");
    g_blockShellHost                  = Wh_GetIntSetting(L"BlockShellHost");
    g_blockShellExperienceHost        = Wh_GetIntSetting(L"BlockShellExperienceHost");
    g_blockMicrosoftStartFeedProvider = Wh_GetIntSetting(L"BlockMicrosoftStartFeedProvider");
    g_blockWidgetBoard                = Wh_GetIntSetting(L"BlockWidgetBoard");
    g_blockWidgetService              = Wh_GetIntSetting(L"BlockWidgetService");
}

// Returns just the filename part of the current process path
const wchar_t* GetCurrentProcessName() {
    static wchar_t path[MAX_PATH];
    GetModuleFileNameW(NULL, path, MAX_PATH);
    const wchar_t* name = wcsrchr(path, L'\\');
    return name ? name + 1 : path;
}

// Helper for process killing
void TerminateProcessByName(const wchar_t* filename) {
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnap == INVALID_HANDLE_VALUE) return;
    PROCESSENTRY32W pe;
    pe.dwSize = sizeof(pe);
    if (Process32FirstW(hSnap, &pe)) {
        do {
            if (_wcsicmp(pe.szExeFile, filename) == 0) {
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
    if (g_blockStartMenu)                  TerminateProcessByName(L"StartMenuExperienceHost.exe");
    if (g_blockSearch)                     TerminateProcessByName(L"SearchHost.exe");
    if (g_blockTextInput)                  TerminateProcessByName(L"TextInputHost.exe");
    if (g_blockShellHost)                  TerminateProcessByName(L"ShellHost.exe");
    if (g_blockShellExperienceHost)        TerminateProcessByName(L"ShellExperienceHost.exe");
    if (g_blockMicrosoftStartFeedProvider) TerminateProcessByName(L"MicrosoftStartFeedProvider.exe");
    if (g_blockWidgetBoard)                TerminateProcessByName(L"WidgetBoard.exe");
    if (g_blockWidgetService)              TerminateProcessByName(L"WidgetService.exe");
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
    auto matches = [&](const wchar_t* target) {
        return (lpCmdLine && wcsistr(lpCmdLine, target)) ||
               (lpAppName && wcsistr(lpAppName, target));
    };

    bool block =
        (g_blockStartMenu                  && matches(L"StartMenuExperienceHost.exe"))     ||
        (g_blockSearch                     && matches(L"SearchHost.exe"))                  ||
        (g_blockTextInput                  && matches(L"TextInputHost.exe"))               ||
        (g_blockShellHost                  && matches(L"ShellHost.exe"))                   ||
        (g_blockShellExperienceHost        && matches(L"ShellExperienceHost.exe"))         ||
        (g_blockMicrosoftStartFeedProvider && matches(L"MicrosoftStartFeedProvider.exe")) ||
        (g_blockWidgetBoard                && matches(L"WidgetBoard.exe"))                 ||
        (g_blockWidgetService              && matches(L"WidgetService.exe"));

    if (block) {
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

void Wh_ModUninit() {}

BOOL Wh_ModInit() {
    LoadSettings();

    // ShellHost respawn block
    if (_wcsicmp(GetCurrentProcessName(), L"ShellHost.exe") == 0) {
        if (g_blockShellHost) {
            Wh_Log(L"ShellHost.exe is blocked, exiting process.");
            ExitProcess(0);
        }
        return FALSE;
    }

    // Kill running instances and set up the hook.
    TerminateConfiguredProcesses();

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
