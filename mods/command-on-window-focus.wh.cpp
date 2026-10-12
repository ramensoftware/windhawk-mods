// ==WindhawkMod==
// @id              command-on-window-focus
// @name            Run command on window focus
// @description     Run a command on window focus matching a filter and optionally a command when not matching
// @version         0.1
// @author          rom4ster
// @github          https://github.com/rom4ster
// @include         windhawk.exe
// @compilerOptions -lcomdlg32
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Command On Window Focus
This mod observes a window focus and will run any command on window focus. 
This mod has a filter which lets you specify which windows will be allowed to have focus based on the path of the executable
running them. There is also the option of alternate commands which will only run if none of the filters match. Filters are regexes (ECMAScript) so providing a window title or something like "steam" will not work. 
An example use case for me personally is running a script that sets the scroll wheel on my mouse to a different scroll type every time 
I focus a game (and the alternate command will reset it to the original type). 
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- filters: [C:\\Program Files\\.*]
  $name: Path Filters
  $description: Paths to match against
- cmd: echo hi
  $name: Command
  $description: Command to run
- enableneg: false
  $name: Alternate Command Enable
  $description: Enable Alternate Command when filter not matched
- altcmd: echo hi
  $name: Alternate Command
  $description: Alternate Command For Non match filter, requires Alternate Command Enable to be true      
*/
// ==/WindhawkModSettings==

#include <gdiplus.h>
#include <regex>
#include <string>
#include <vector>

using namespace Gdiplus;



struct pattern_obj {
    std::wstring filter;
    std::wregex regex;
};

struct {
    std::vector<pattern_obj> pathFilters;
    std::wstring cmd;
    bool negcmd_enable;
    std::wstring negcmd;
} settings;


void LoadSettings() {
    int i = 0;
    settings.pathFilters.clear();
    PCWSTR current = L"";
    while( ( current = Wh_GetStringSetting(L"filters[%d]", i++))[0] != L'\0') {
        pattern_obj current_pattern;
        current_pattern.filter = current;
		try { 
			current_pattern.regex = std::wregex(current);
			settings.pathFilters.push_back(current_pattern);
		} catch (const std::regex_error&) {
            Wh_Log(L"Invalid Regex: %s", current);
        }

        Wh_FreeStringSetting(current);
    
    }
	Wh_FreeStringSetting(current);
    for (int i =0; i < settings.pathFilters.size(); i++) Wh_Log(L"LIST VALUE:  %s", settings.pathFilters[i].filter.c_str());
    PCWSTR cmd = Wh_GetStringSetting(L"cmd");
    settings.cmd = cmd;
    Wh_FreeStringSetting(cmd);
    Wh_Log(L"CMD IS %s", settings.cmd.c_str());

    bool enableneg = Wh_GetIntSetting(L"enableneg");
    settings.negcmd_enable = enableneg > 0 ? true : false;

    PCWSTR altcmd = Wh_GetStringSetting(L"altcmd");
    settings.negcmd = altcmd;
    Wh_FreeStringSetting(altcmd);
    Wh_Log(L"ALT CMD IS %s and it is %s", settings.negcmd.c_str(), settings.negcmd_enable ? L"ENABLED" : L"DISABLED");


}


void RunCommandHidden(const std::wstring& cmd) {
    STARTUPINFOW si = { sizeof(si) };
    PROCESS_INFORMATION pi;

    std::wstring cmdLine = L"cmd.exe /c " + cmd;

    if (CreateProcessW(
        nullptr,
        cmdLine.data(),     
        nullptr, nullptr,
        FALSE,
        CREATE_NO_WINDOW,   
        nullptr, nullptr,
        &si, &pi))
    {
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    } else {
        Wh_Log(L"Failed to launch command, error: %lu", GetLastError());
    }
}




HWINEVENTHOOK g_hWinEventHook;
HANDLE g_hThread;
DWORD g_threadId;

void CALLBACK WinEventProc(
    HWINEVENTHOOK hWinEventHook,
    DWORD event,
    HWND hwnd,
    LONG idObject,
    LONG idChild,
    DWORD dwEventThread,
    DWORD dwmsEventTime
)
{
    if (event != EVENT_SYSTEM_FOREGROUND || !hwnd) {
        return;
    }

    // 1. Get the process ID that owns this window
    DWORD processId = 0;
    GetWindowThreadProcessId(hwnd, &processId);

    // 2. Open the process so we can query it
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, processId);
    if (!hProcess) {
        Wh_Log(L"Failed to open process %lu, error: %lu", processId, GetLastError());
        return;
    }

    // 3. Get its executable path
    WCHAR exePath[MAX_PATH];
    DWORD exePathSize = ARRAYSIZE(exePath);
    BOOL processPath = QueryFullProcessImageNameW(hProcess, 0, exePath, &exePathSize);
    CloseHandle(hProcess);
    if (processPath) {
        Wh_Log(L"Foreground window (PID %lu) launched from: %s", processId, exePath);
    } else {
        Wh_Log(L"Failed to get process path, error: %lu", GetLastError());
        return;
    }
    bool match = false;
    

    Wh_Log(L"Found %d regexes to try", settings.pathFilters.size());
    for (int i =0; i < settings.pathFilters.size(); i++) {
        Wh_Log(L"Trying %s, iterator %d", settings.pathFilters[i].filter.c_str(), i);
            const std::wregex& pattern = settings.pathFilters[i].regex;
            PCWSTR exePathStr = exePath;
            if (std::regex_match(exePathStr, pattern)) {
                match = true;
                break;
            } else {
                Wh_Log(L"%s and %s: NO MATCH", exePathStr, settings.pathFilters[i].filter.c_str() );
            }  

 
    }
    
    if (match) {
        Wh_Log(L"found");
        RunCommandHidden(settings.cmd);
    } else {
        if (settings.negcmd_enable) {
            RunCommandHidden(settings.negcmd);
        }
    }
}


DWORD WINAPI HookThreadProc(LPVOID)
{

    // Actual hook for when process recieves foreground event
    g_hWinEventHook = SetWinEventHook(
        EVENT_SYSTEM_FOREGROUND,
        EVENT_SYSTEM_FOREGROUND,
        nullptr,           // no injection needed for OUTOFCONTEXT
        WinEventProc,
        0, 0,               // all processes, all threads
        WINEVENT_OUTOFCONTEXT
    );

    Wh_Log(L"Hook handle: %p, GetLastError: %lu", g_hWinEventHook, GetLastError());

    MSG msg;
    // First recieve a message for this thread
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        // Call WinProc for the given value in this msg hwnd. 
        // This is the WinEventProc we have for this event
        DispatchMessage(&msg); 
    }

    if (g_hWinEventHook) {
        UnhookWinEvent(g_hWinEventHook);
        g_hWinEventHook = nullptr;
    }

    return 0;
}

// The mod is being initialized, load settings, hook functions, and do other
// initialization stuff if required.
BOOL WhTool_ModInit() {
    Wh_Log(L"Init");
    LoadSettings();

    g_hThread = CreateThread(nullptr, 0, HookThreadProc, nullptr, 0, &g_threadId);
    if (!g_hThread) {
        Wh_Log(L"Failed to create hook thread");
        return FALSE;
    }

    return TRUE;
}


// The mod is being unloaded, free all allocated resources.
void WhTool_ModUninit() {
    Wh_Log(L"Uninit");
    if (g_hThread) {
        PostThreadMessage(g_threadId, WM_QUIT, 0, 0);
        WaitForSingleObject(g_hThread, 10000);
        CloseHandle(g_hThread);
        g_hThread = nullptr;
    }
}

// The mod setting were changed, reload them.
void WhTool_ModSettingsChanged () {
    Wh_Log(L"SettingsChanged");

    LoadSettings();
}

////////////////////////////////////////////////////////////////////////////////
// Windhawk tool mod implementation for mods which don't need to inject to other
// processes or hook other functions. Context:
// https://github.com/ramensoftware/windhawk/wiki/Mods-as-tools:-Running-mods-in-a-dedicated-process
//
// The mod will load and run in a dedicated windhawk.exe process.
//
// Paste the code below as part of the mod code, and use these callbacks:
// * WhTool_ModInit
// * WhTool_ModSettingsChanged
// * WhTool_ModUninit
//
// Currently, other callbacks are not supported.

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
