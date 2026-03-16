// ==WindhawkMod==
// @id              keep-rainmeter-always-bottom
// @name            Keep Rainmeter Always on Desktop
// @description     Keeps Rainmeter windows to stay on desktop.
// @version         1.1.1
// @author          BCRTVKCS
// @github          https://github.com/bcrtvkcs
// @twitter         https://x.com/bcrtvkcs
// @homepage        https://grdigital.pro
// @include         windhawk.exe
// @compilerOptions -luser32 -lkernel32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Keep Rainmeter Always on Desktop

Keeps Rainmeter windows to stay on desktop. Rainmeter skin windows can unexpectedly appear on top of other windows during window switching events (e.g. Alt+Tab), application focus changes, or when triggered by external scripts. This mod forces all Rainmeter windows to remain below all other windows at all times, using a `EVENT_SYSTEM_FOREGROUND` event hook to push them to the bottom of the Z-order whenever any window comes to the foreground.

## Compatibility
- Windows 10 and Windows 11
- Targets `windhawk.exe` only
*/
// ==/WindhawkModReadme==

#include <minwindef.h>
#include <windef.h>
#include <winuser.h>
#include <stdio.h>

HWINEVENTHOOK g_eventHook = nullptr;
static DWORD g_rainmeterPid = 0;

static DWORD GetRainmeterPid()
{
    HWND hw = FindWindowW(L"RainmeterMeterWindow", nullptr);
    if (!hw) hw = FindWindowW(L"RainmeterTrayClass", nullptr);
    if (!hw) return 0;
    DWORD pid = 0;
    GetWindowThreadProcessId(hw, &pid);
    return pid;
}

static BOOL CALLBACK PushWindowToBottom(HWND hw, LPARAM)
{
    DWORD wpid = 0;
    GetWindowThreadProcessId(hw, &wpid);
    if (wpid == g_rainmeterPid)
        SetWindowPos(hw, HWND_BOTTOM, 0, 0, 0, 0,
            SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOSENDCHANGING);
    return TRUE;
}

static void PushRainmeterToBottom()
{
    g_rainmeterPid = GetRainmeterPid();
    if (!g_rainmeterPid) return;
    EnumWindows(PushWindowToBottom, 0);
}

static void CALLBACK WinEventProc(
    HWINEVENTHOOK, DWORD event, HWND,
    LONG, LONG, DWORD, DWORD)
{
    if (event == EVENT_SYSTEM_FOREGROUND)
        PushRainmeterToBottom();
}

BOOL WhTool_ModInit()
{
    Wh_Log(L"keep-rainmeter-always-bottom init");

    g_eventHook = SetWinEventHook(
        EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND,
        nullptr, WinEventProc,
        0, 0,
        WINEVENT_OUTOFCONTEXT);

    if (!g_eventHook)
    {
        Wh_Log(L"SetWinEventHook failed: %u", GetLastError());
        return FALSE;
    }

    Wh_Log(L"WinEvent hook installed, running message loop");

    // WINEVENT_OUTOFCONTEXT requires a message loop to dispatch events
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return TRUE;
}

void WhTool_ModUninit()
{
    PostThreadMessage(GetCurrentThreadId(), WM_QUIT, 0, 0);
    if (g_eventHook)
    {
        UnhookWinEvent(g_eventHook);
        g_eventHook = nullptr;
    }
    Wh_Log(L"uninit");
}

////////////////////////////////////////////////////////////////////////////////
// Tool mod boilerplate

bool g_isToolModProcessLauncher;
HANDLE g_toolModProcessMutex;

void WINAPI EntryPoint_Hook() {
    Wh_Log(L">");
    ExitThread(0);
}

BOOL Wh_ModInit() {
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

    WCHAR commandLine[MAX_PATH + 2 +
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
        HANDLE, LPCWSTR, LPWSTR, LPSECURITY_ATTRIBUTES,
        LPSECURITY_ATTRIBUTES, WINBOOL, DWORD, LPVOID, LPCWSTR,
        LPSTARTUPINFOW, LPPROCESS_INFORMATION, PHANDLE);
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
}

void Wh_ModUninit() {
    if (g_isToolModProcessLauncher) {
        return;
    }

    WhTool_ModUninit();
    ExitProcess(0);
}
