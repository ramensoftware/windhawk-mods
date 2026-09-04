// ==WindhawkMod==
// @id              ctrl-to-cross-monitors
// @name            Hold Ctrl to cross monitors
// @description     Prevents the mouse cursor from moving to another monitor unless Ctrl is held while moving the mouse.
// @version         0.1
// @author          manudesir
// @github          https://github.com/manudesir
// @include         windhawk.exe
// @compilerOptions -luser32
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Hold Ctrl to cross monitors

This mod blocks the mouse cursor from crossing from one monitor to another unless
Ctrl is currently held down.

## Behavior
- Normal mouse movement inside the current monitor is unaffected.
- If the cursor tries to move to another monitor and Ctrl is **not** pressed,
  the cursor is forced back to its previous position.
- If Ctrl is pressed while continuing the mouse movement, the cross-monitor move
  is allowed.

## Notes
- This is a single-file Windhawk mod, which matches Windhawk's documented mod format. ([github.com](https://github.com/ramensoftware/windhawk/wiki/Creating-a-new-mod))
- It runs as a dedicated Windhawk tool process rather than inside Explorer. ([github.com](https://github.com/ramensoftware/windhawk/wiki/Mods-as-tools:-Running-mods-in-a-dedicated-process))
- It uses a low-level mouse hook (`WH_MOUSE_LL`), which Microsoft says should run
  on a dedicated thread with a message loop and return quickly. ([learn.microsoft.com](https://learn.microsoft.com/en-us/windows/win32/winmsg/lowlevelmouseproc))
- It checks Ctrl with `GetAsyncKeyState(VK_CONTROL)`. Microsoft documents `VK_CONTROL`
  specifically for querying the state of Ctrl without distinguishing left vs right. ([learn.microsoft.com](https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-getasynckeystate))

## Limitation
If this still has absolutely no effect on your machine, the likely issue is that
the dedicated Windhawk tool process didn't start correctly or the hook didn't
become active, not the core cursor logic itself.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- enabled: true
  $name: Enabled
- logTransitions: false
  $name: Log transitions
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <strsafe.h>

struct Settings {
    bool enabled;
    bool logTransitions;
};

static Settings g_settings{};
static HHOOK g_mouseHook = nullptr;
static HMODULE g_moduleHandle = nullptr;
static HANDLE g_hookThread = nullptr;
static DWORD g_hookThreadId = 0;
static HANDLE g_threadReadyEvent = nullptr;
static bool g_isToolModProcessLauncher = false;
static HANDLE g_toolModProcessMutex = nullptr;

struct CursorState {
    POINT lastPoint{};
    HMONITOR lastMonitor = nullptr;
    POINT forcedPoint{};
    bool hasLastPoint = false;
    bool ignoreForcedPoint = false;
};

static CursorState g_cursorState{};

static void LoadSettings() {
    g_settings.enabled = Wh_GetIntSetting(L"enabled") != 0;
    g_settings.logTransitions = Wh_GetIntSetting(L"logTransitions") != 0;
}

static bool GetCurrentModuleHandle(HMODULE* moduleHandle) {
    return GetModuleHandleExW(
        GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
            GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
        reinterpret_cast<LPCWSTR>(&GetCurrentModuleHandle),
        moduleHandle);
}

static bool IsCtrlHeld() {
    return GetAsyncKeyState(VK_CONTROL) < 0;
}

static bool AreSamePoint(const POINT& left, const POINT& right) {
    return left.x == right.x && left.y == right.y;
}

static void CloseHandleIfValid(HANDLE& handle) {
    if (handle) {
        CloseHandle(handle);
        handle = nullptr;
    }
}

static void ResetCursorState() {
    g_cursorState = {};
}

static void RememberCursorPosition(const POINT& point, HMONITOR monitor = nullptr) {
    g_cursorState.lastPoint = point;
    g_cursorState.lastMonitor =
        monitor ? monitor : MonitorFromPoint(point, MONITOR_DEFAULTTONEAREST);
    g_cursorState.hasLastPoint = true;
}

static void RememberCursorPointOnly(const POINT& point) {
    g_cursorState.lastPoint = point;
    g_cursorState.lastMonitor = nullptr;
    g_cursorState.hasLastPoint = true;
}

static void LogMonitorCrossing(bool blocked,
                               const POINT& previousPoint,
                               const POINT& currentPoint) {
    if (!g_settings.logTransitions) {
        return;
    }

    if (blocked) {
        Wh_Log(L"Blocked monitor crossing from (%d, %d) to (%d, %d)",
               previousPoint.x,
               previousPoint.y,
               currentPoint.x,
               currentPoint.y);
        return;
    }

    Wh_Log(L"Allowed monitor crossing with Ctrl from (%d, %d) to (%d, %d)",
           previousPoint.x,
           previousPoint.y,
           currentPoint.x,
           currentPoint.y);
}

static void ForceCursorTo(const POINT& point) {
    g_cursorState.forcedPoint = point;
    g_cursorState.ignoreForcedPoint = true;
    SetCursorPos(point.x, point.y);
}

static LRESULT CALLBACK LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode < 0 || wParam != WM_MOUSEMOVE) {
        return CallNextHookEx(nullptr, nCode, wParam, lParam);
    }

    const auto* mouseInfo = reinterpret_cast<const MSLLHOOKSTRUCT*>(lParam);
    const POINT currentPoint = mouseInfo->pt;

    if (g_cursorState.ignoreForcedPoint &&
        AreSamePoint(currentPoint, g_cursorState.forcedPoint)) {
        g_cursorState.ignoreForcedPoint = false;
        RememberCursorPosition(currentPoint, g_cursorState.lastMonitor);
        return CallNextHookEx(nullptr, nCode, wParam, lParam);
    }

    if (!g_settings.enabled) {
        RememberCursorPointOnly(currentPoint);
        return CallNextHookEx(nullptr, nCode, wParam, lParam);
    }

    if (!g_cursorState.hasLastPoint) {
        RememberCursorPosition(currentPoint);
        return CallNextHookEx(nullptr, nCode, wParam, lParam);
    }

    if (AreSamePoint(currentPoint, g_cursorState.lastPoint)) {
        return CallNextHookEx(nullptr, nCode, wParam, lParam);
    }

    HMONITOR lastMonitor = g_cursorState.lastMonitor;
    if (!lastMonitor) {
        lastMonitor = MonitorFromPoint(g_cursorState.lastPoint,
                                       MONITOR_DEFAULTTONEAREST);
        g_cursorState.lastMonitor = lastMonitor;
    }

    const HMONITOR currentMonitor =
        MonitorFromPoint(currentPoint, MONITOR_DEFAULTTONEAREST);
    const bool crossedMonitor =
        lastMonitor && currentMonitor && lastMonitor != currentMonitor;

    if (crossedMonitor && !IsCtrlHeld()) {
        LogMonitorCrossing(true, g_cursorState.lastPoint, currentPoint);
        ForceCursorTo(g_cursorState.lastPoint);
        return 1;
    }

    if (crossedMonitor) {
        LogMonitorCrossing(false, g_cursorState.lastPoint, currentPoint);
    }

    RememberCursorPosition(currentPoint, currentMonitor);
    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

static DWORD WINAPI HookThreadProc(void*) {
    MSG msg{};
    PeekMessageW(&msg, nullptr, WM_USER, WM_USER, PM_NOREMOVE);

    g_mouseHook = SetWindowsHookExW(WH_MOUSE_LL, LowLevelMouseProc, g_moduleHandle, 0);
    if (!g_mouseHook) {
        Wh_Log(L"SetWindowsHookExW failed: %u", GetLastError());
        SetEvent(g_threadReadyEvent);
        return 1;
    }

    SetEvent(g_threadReadyEvent);

    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
    }

    if (g_mouseHook) {
        UnhookWindowsHookEx(g_mouseHook);
        g_mouseHook = nullptr;
    }

    return 0;
}

BOOL WhTool_ModInit() {
    LoadSettings();

    if (!GetCurrentModuleHandle(&g_moduleHandle) || !g_moduleHandle) {
        Wh_Log(L"Failed to get current module handle");
        return FALSE;
    }

    g_threadReadyEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_threadReadyEvent) {
        Wh_Log(L"CreateEventW failed: %u", GetLastError());
        return FALSE;
    }

    g_hookThread = CreateThread(nullptr, 0, HookThreadProc, nullptr, 0, &g_hookThreadId);
    if (!g_hookThread) {
        Wh_Log(L"CreateThread failed: %u", GetLastError());
        CloseHandleIfValid(g_threadReadyEvent);
        return FALSE;
    }

    WaitForSingleObject(g_threadReadyEvent, INFINITE);

    if (!g_mouseHook) {
        WaitForSingleObject(g_hookThread, INFINITE);
        CloseHandleIfValid(g_hookThread);
        CloseHandleIfValid(g_threadReadyEvent);
        return FALSE;
    }

    Wh_Log(L"Hold Ctrl to cross monitors initialized");
    return TRUE;
}

void WhTool_ModUninit() {
    if (g_hookThreadId) {
        PostThreadMessageW(g_hookThreadId, WM_QUIT, 0, 0);
    }

    if (g_hookThread) {
        WaitForSingleObject(g_hookThread, INFINITE);
    }
    CloseHandleIfValid(g_hookThread);

    CloseHandleIfValid(g_threadReadyEvent);

    g_mouseHook = nullptr;
    g_hookThreadId = 0;
    ResetCursorState();

    Wh_Log(L"Hold Ctrl to cross monitors uninitialized");
}

void WhTool_ModSettingsChanged() {
    LoadSettings();
    ResetCursorState();
}

// Everything below is Windhawk tool boilerplate; the cursor behavior lives above.
////////////////////////////////////////////////////////////////////////////////
// Windhawk tool mod implementation for mods which don't need to inject to other
// processes or hook other functions. Context:
// [https://github.com/ramensoftware/windhawk-mods/pull/1916](https://github.com/ramensoftware/windhawk-mods/pull/1916)
//
// The mod will load and run in a dedicated windhawk.exe process.
//
// Paste the code below as part of the mod code, and use these callbacks:
// * WhTool_ModInit
// * WhTool_ModSettingsChanged
// * WhTool_ModUninit
//
// Currently, other callbacks are not supported.

void WINAPI EntryPoint_Hook() {
    ExitThread(0);
}

BOOL Wh_ModInit() {
    bool isService = false;
    bool isToolModProcess = false;
    bool isCurrentToolModProcess = false;
    int argc;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLine(), &argc);
    if (!argv) {
        Wh_Log(L"CommandLineToArgvW failed");
        return FALSE;
    }

    for (int i = 1; i < argc; i++) {
        if (wcscmp(argv[i], L"-service") == 0) {
            isService = true;
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

    if (isService) {
        return FALSE;
    }

    if (isCurrentToolModProcess) {
        g_toolModProcessMutex =
            CreateMutexW(nullptr, TRUE, L"windhawk-tool-mod_" WH_MOD_ID);
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
            reinterpret_cast<IMAGE_DOS_HEADER*>(GetModuleHandleW(nullptr));
        IMAGE_NT_HEADERS* ntHeaders = reinterpret_cast<IMAGE_NT_HEADERS*>(
            reinterpret_cast<BYTE*>(dosHeader) + dosHeader->e_lfanew);

        DWORD entryPointRVA = ntHeaders->OptionalHeader.AddressOfEntryPoint;
        void* entryPoint = reinterpret_cast<BYTE*>(dosHeader) + entryPointRVA;

        Wh_SetFunctionHook(entryPoint, reinterpret_cast<void*>(EntryPoint_Hook),
                           nullptr);
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
    switch (GetModuleFileNameW(nullptr, currentProcessPath,
                               ARRAYSIZE(currentProcessPath))) {
        case 0:
        case ARRAYSIZE(currentProcessPath):
            Wh_Log(L"GetModuleFileName failed");
            return;
    }

    WCHAR commandLine[MAX_PATH + 2 +
                      (sizeof(L" -tool-mod \"" WH_MOD_ID "\"") /
                           sizeof(WCHAR)) -
                      1];
    if (FAILED(StringCchPrintfW(commandLine, ARRAYSIZE(commandLine),
                                L"\"%s\" -tool-mod \"%s\"",
                                currentProcessPath, WH_MOD_ID))) {
        Wh_Log(L"StringCchPrintfW failed");
        return;
    }

    HMODULE kernelModule = GetModuleHandleW(L"kernelbase.dll");
    if (!kernelModule) {
        kernelModule = GetModuleHandleW(L"kernel32.dll");
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
        reinterpret_cast<CreateProcessInternalW_t>(
            GetProcAddress(kernelModule, "CreateProcessInternalW"));
    if (!pCreateProcessInternalW) {
        Wh_Log(L"No CreateProcessInternalW");
        return;
    }

    STARTUPINFOW si{
        .cb = sizeof(STARTUPINFOW),
        .dwFlags = STARTF_FORCEOFFFEEDBACK,
    };
    PROCESS_INFORMATION pi{};
    if (!pCreateProcessInternalW(nullptr, currentProcessPath, commandLine,
                                 nullptr, nullptr, FALSE, CREATE_NO_WINDOW,
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
    CloseHandleIfValid(g_toolModProcessMutex);
}
