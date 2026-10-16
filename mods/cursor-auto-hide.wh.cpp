// ==WindhawkMod==
// @id              cursor-auto-hide
// @name            Cursor auto-hide on idle
// @description     Hides the mouse cursor after a configurable idle timeout and shows it again as soon as the mouse moves or a button is clicked
// @version         1.0
// @author          nikitor32
// @github          https://github.com/nikitor32
// @include         windhawk.exe
// ==/WindhawkMod==

// Source code is published under The GNU General Public License v3.0.

// ==WindhawkModReadme==
/*
# Cursor auto-hide on idle

The mouse cursor usually stays on screen even when you are not using the
mouse, covering videos, text and presentations.

This mod hides the cursor after it has not moved for a configurable
number of seconds (5 by default). The cursor reappears immediately on
the first movement or click.

Cursor changes happen at runtime only: your cursor scheme is never
written to the registry, and any mouse activity restores it right away.

Only the system cursors are replaced. Applications that draw their own
cursor, such as games or drawing tools, keep showing it.

The mod runs in a dedicated Windhawk process (as a tool mod) and does
not inject into other applications.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- idleSeconds: 5
  $name: Idle timeout (seconds)
  $description: The cursor is hidden this many seconds after the last mouse movement or click. Set to 0 to disable hiding.
*/
// ==/WindhawkModSettings==

#include <stdio.h>
#include <windows.h>

#include <atomic>

// The setting can be updated from another thread while the worker thread
// reads it, so an atomic type is used to avoid a data race.
std::atomic<int> g_idleSeconds{5};

HANDLE g_stopEvent = nullptr;
HANDLE g_threadHandle = nullptr;

// The state below is only touched by the worker thread: raw input
// events and the idle check are processed on that thread.
ULONGLONG g_lastActivity = 0;
bool g_hidden = false;
POINT g_lastPolledPos{};

// Restores the cursors if the process dies while they are modified.
LPTOP_LEVEL_EXCEPTION_FILTER g_previousExceptionFilter = nullptr;

// OEM resource cursor IDs (see winuser.h). <windows.h> only declares the
// OCR_* names when OEMRESOURCE is defined before its first inclusion, so
// the values are spelled out here to avoid include-order surprises.
// HideCursors replaces every cursor in this list.
constexpr int kSystemCursorIds[] = {
    32512,  // OCR_NORMAL
    32513,  // OCR_IBEAM
    32514,  // OCR_WAIT
    32515,  // OCR_CROSS
    32516,  // OCR_UP
    32642,  // OCR_SIZENWSE
    32643,  // OCR_SIZENESW
    32644,  // OCR_SIZEWE
    32645,  // OCR_SIZENS
    32646,  // OCR_SIZEALL
    32648,  // OCR_NO
    32649,  // OCR_HAND
    32650,  // OCR_APPSTARTING
    32651,  // OCR_HELP
    32631,  // NWPen (handwriting)
    32671,  // Pin (location select)
    32672,  // Person (person select)
};

void LoadSettings() {
    int idleSeconds = Wh_GetIntSetting(L"idleSeconds");
    if (idleSeconds < 0) {
        idleSeconds = 0;
    }
    g_idleSeconds.store(idleSeconds);
}

// A fully transparent 32x32 cursor: the AND mask keeps the screen, the
// XOR mask changes nothing.
HCURSOR CreateBlankCursor() {
    BYTE andMask[128];
    for (BYTE& b : andMask) {
        b = 0xFF;
    }
    BYTE xorMask[128] = {};
    return CreateCursor(GetModuleHandle(nullptr), 0, 0, 32, 32, andMask,
                        xorMask);
}

// SetSystemCursor takes ownership of the handle it receives (and
// destroys it), so every system cursor gets its own copy.
void HideCursors() {
    // Remember that the cursors are modified, so the next start of this
    // tool can put them back if the process dies without cleaning up.
    Wh_SetIntValue(L"cursorsModified", 1);

    HCURSOR blank = CreateBlankCursor();
    if (!blank) {
        Wh_Log(L"CreateCursor failed");
        return;
    }
    for (int id : kSystemCursorIds) {
        HCURSOR copy =
            (HCURSOR)CopyImage(blank, IMAGE_CURSOR, 0, 0, LR_DEFAULTSIZE);
        if (copy && !SetSystemCursor(copy, id)) {
            // Ownership transfers only on success.
            DestroyCursor(copy);
        }
    }
    DestroyCursor(blank);
    Wh_Log(L"Cursors hidden");
}

// Reloads the current cursor scheme; runtime replacements are dropped.
void RestoreCursors() {
    SystemParametersInfo(SPI_SETCURSORS, 0, nullptr, 0);
    Wh_SetIntValue(L"cursorsModified", 0);
    Wh_Log(L"Cursors restored");
}

// Cursors left transparent by a crash would stay that way, so restore
// them as early as possible and pass the exception on.
LONG WINAPI RestoreCursorsOnCrash(EXCEPTION_POINTERS* exceptionInfo) {
    SystemParametersInfo(SPI_SETCURSORS, 0, nullptr, 0);
    return g_previousExceptionFilter
               ? g_previousExceptionFilter(exceptionInfo)
               : EXCEPTION_CONTINUE_SEARCH;
}

// A press-and-hold without movement (holding a scrollbar arrow, pausing
// mid-drag) is not an idle mouse: the cursors stay visible.
bool IsMouseButtonHeld() {
    constexpr int kButtons[] = {VK_LBUTTON, VK_RBUTTON, VK_MBUTTON,
                                VK_XBUTTON1, VK_XBUTTON2};
    for (int button : kButtons) {
        if (GetAsyncKeyState(button) & 0x8000) {
            return true;
        }
    }
    return false;
}

DWORD WINAPI WorkerThread(LPVOID /*param*/) {
    g_lastActivity = GetTickCount64();
    GetCursorPos(&g_lastPolledPos);

    // Raw input delivers WM_INPUT for every mouse movement, button and
    // wheel event to a message-only window. Unlike a low-level hook it
    // is not part of the session's input path, so the work done here
    // (replacing the cursors) can never delay the mouse, and Windows
    // never drops the notification for being slow.
    HWND hwnd = CreateWindowEx(0, L"STATIC", nullptr, 0, 0, 0, 0, 0,
                               HWND_MESSAGE, nullptr, nullptr, nullptr);
    if (!hwnd) {
        Wh_Log(L"CreateWindowEx failed");
        return 1;
    }

    RAWINPUTDEVICE device{};
    device.usUsagePage = 0x01;  // Generic desktop controls
    device.usUsage = 0x02;      // Mouse
    device.dwFlags = RIDEV_INPUTSINK;
    device.hwndTarget = hwnd;
    if (!RegisterRawInputDevices(&device, 1, sizeof(device))) {
        Wh_Log(L"RegisterRawInputDevices failed: %u", GetLastError());
        DestroyWindow(hwnd);
        return 1;
    }
    Wh_Log(L"Raw input registered");

    bool stopping = false;
    while (!stopping) {
        // Wake up on raw input, on the stop event, or after roughly the
        // interval used for the idle check. While the cursor is hidden
        // the loop runs more often so that movement is noticed quickly.
        DWORD wait =
            MsgWaitForMultipleObjects(1, &g_stopEvent, FALSE,
                                       g_hidden ? 100 : 500, QS_ALLINPUT);
        if (wait == WAIT_OBJECT_0) {
            stopping = true;
        }

        bool mouseActivity = false;
        MSG msg;
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_INPUT) {
                mouseActivity = true;
            }
            TranslateMessage(&msg);
            DispatchMessage(&msg);  // DefWindowProc frees the raw data.
        }
        if (stopping) {
            break;
        }

        if (mouseActivity) {
            g_lastActivity = GetTickCount64();
            if (g_hidden) {
                g_hidden = false;
                RestoreCursors();
            }
        }

        // Polling the cursor position is a safety net for the cases raw
        // input doesn't cover, and it reads global state, so no special
        // privileges are needed. When it fails, another desktop (the
        // lock screen, a UAC prompt) has the input: activity there
        // can't be observed, so the cursors must not stay hidden while
        // the mod can't see the mouse.
        POINT polledPos;
        if (!GetCursorPos(&polledPos)) {
            g_lastActivity = GetTickCount64();
            if (g_hidden) {
                g_hidden = false;
                RestoreCursors();
            }
        } else if (polledPos.x != g_lastPolledPos.x ||
                   polledPos.y != g_lastPolledPos.y) {
            g_lastPolledPos = polledPos;
            g_lastActivity = GetTickCount64();
            if (g_hidden) {
                g_hidden = false;
                RestoreCursors();
            }
        }

        int idleSeconds = g_idleSeconds.load();
        if (idleSeconds <= 0) {
            // Hiding is disabled; make sure the cursor is shown again if
            // the setting was just changed while hidden.
            if (g_hidden) {
                g_hidden = false;
                RestoreCursors();
            }
            continue;
        }

        ULONGLONG now = GetTickCount64();
        if (!g_hidden &&
            now - g_lastActivity >= (ULONGLONG)idleSeconds * 1000 &&
            !IsMouseButtonHeld()) {
            HideCursors();
            g_hidden = true;
        }
    }

    DestroyWindow(hwnd);
    if (g_hidden) {
        g_hidden = false;
        RestoreCursors();
    }
    Wh_Log(L"Worker stopped");
    return 0;
}

BOOL WhTool_ModInit() {
    Wh_Log(L">");
    LoadSettings();

    // The previous run ended while the cursors were modified (a crash or
    // a process killed from Task Manager), so put the normal cursors back.
    if (Wh_GetIntValue(L"cursorsModified", 0)) {
        RestoreCursors();
    }

    g_previousExceptionFilter =
        SetUnhandledExceptionFilter(RestoreCursorsOnCrash);

    g_stopEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
    if (!g_stopEvent) {
        Wh_Log(L"CreateEvent failed");
        return FALSE;
    }

    g_threadHandle = CreateThread(nullptr, 0, WorkerThread, nullptr, 0, nullptr);
    if (!g_threadHandle) {
        Wh_Log(L"CreateThread failed");
        CloseHandle(g_stopEvent);
        g_stopEvent = nullptr;
        return FALSE;
    }
    return TRUE;
}

void WhTool_ModSettingsChanged() {
    Wh_Log(L">");
    LoadSettings();
}

void WhTool_ModUninit() {
    Wh_Log(L">");
    // Wait for the worker thread without a timeout: it may be inside
    // HideCursors or RestoreCursors, and unloading the mod while it
    // runs would crash the process.
    if (g_stopEvent) {
        SetEvent(g_stopEvent);
    }
    if (g_threadHandle) {
        WaitForSingleObject(g_threadHandle, INFINITE);
        CloseHandle(g_threadHandle);
        g_threadHandle = nullptr;
    }
    if (g_stopEvent) {
        CloseHandle(g_stopEvent);
        g_stopEvent = nullptr;
    }

    // Safety net in case the worker couldn't clean up in time.
    if (Wh_GetIntValue(L"cursorsModified", 0)) {
        RestoreCursors();
    }

    SetUnhandledExceptionFilter(g_previousExceptionFilter);
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