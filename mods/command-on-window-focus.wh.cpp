// ==WindhawkMod==
// @id              command-on-window-focus
// @name            Run command on window focus
// @description     Run a command on window focus matching a filter and optionally a command when not matching
// @version         0.1
// @author          rom4ster
// @github          https://github.com/rom4ster
// @include         explorer.exe
// @compilerOptions -lcomdlg32
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Your Awesome Mod
This is a place for useful information about your mod. Use it to describe the
mod, explain why it's useful, and add any other relevant details. You can use
[Markdown](https://en.wikipedia.org/wiki/Markdown) to add links and
**formatting** to the readme.

This short sample customizes Microsoft Paint by forcing it to use just a single
color, and by blocking file opening. To see the mod in action:
- Compile the mod with the button on the left or with Ctrl+B.
- Run Microsoft Paint from the start menu (type "Paint") or by running
  mspaint.exe.
- Draw something and notice that the orange color is always used, regardless of
  the color you pick.
- Try opening a file and notice that it's blocked.

# Getting started
Check out the documentation
[here](https://github.com/ramensoftware/windhawk/wiki/Creating-a-new-mod).
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- filters: [C:\\Program Files\\*]
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

// The source code of the mod starts here. This sample was inspired by the great
// article of Kyle Halladay, X64 Function Hooking by Example:
// https://kylehalladay.com/blog/2020/11/13/Hooking-By-Example.html
// If you're new to terms such as code injection and function hooking, the
// article is great to get started.

#include <gdiplus.h>
#include <regex>
#include <string>


using namespace Gdiplus;

struct {
    std::vector<std::wstring> pathFilters;
    std::wstring cmd;
    bool negcmd_enable;
    std::wstring negcmd;
} settings;


void LoadSettings() {
    int i = 0;
    settings.pathFilters.clear();
    PCWSTR current = L"";
    while( ( current = Wh_GetStringSetting(L"filters[%d]", i++))[0] != L'\0') {
        settings.pathFilters.push_back(current);
        Wh_FreeStringSetting(current);
    
    }
    for (int i =0; i < settings.pathFilters.size(); i++) Wh_Log(L"LIST VALUE:  %s", settings.pathFilters[i].c_str());
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
    if (QueryFullProcessImageNameW(hProcess, 0, exePath, &exePathSize)) {
        Wh_Log(L"Foreground window (PID %lu) launched from: %s", processId, exePath);
    } else {
        Wh_Log(L"Failed to get process path, error: %lu", GetLastError());
    }
    bool match = false;
    
    std::wregex pattern;
    Wh_Log(L"Found %d regexes to try", settings.pathFilters.size());
    for (int i =0; i < settings.pathFilters.size(); i++) {
        Wh_Log(L"Trying %s, iterator %d", settings.pathFilters[i].c_str(), i);
        try {
            pattern = std::wregex(settings.pathFilters[i]);
            PCWSTR exePathStr = exePath;
            if (std::regex_match(exePathStr, pattern)) {
                match = true;
                break;
            } else {
                Wh_Log(L"%s and %s: NO MATCH", exePathStr, settings.pathFilters[i].c_str() );
            }  
        } catch (const std::regex_error&) {
            Wh_Log(L"Invalid Regex: %s", settings.pathFilters[i].c_str());
           continue;
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


    CloseHandle(hProcess);
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
BOOL Wh_ModInit() {
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
void Wh_ModUninit() {
    Wh_Log(L"Uninit");
    if (g_hThread) {
        PostThreadMessage(g_threadId, WM_QUIT, 0, 0);
        WaitForSingleObject(g_hThread, INFINITE);
        CloseHandle(g_hThread);
        g_hThread = nullptr;
    }
}

// The mod setting were changed, reload them.
void Wh_ModSettingsChanged() {
    Wh_Log(L"SettingsChanged");

    LoadSettings();
}
