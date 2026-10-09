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

// The state below is only touched by the worker thread: both the
// low-level mouse hook and the idle check run on that thread.
ULONGLONG g_lastActivity = 0;
bool g_hidden = false;
POINT g_lastPolledPos{};

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
    32640,  // OCR_SIZE
    32641,  // OCR_ICON
    32642,  // OCR_SIZENWSE
    32643,  // OCR_SIZENESW
    32644,  // OCR_SIZEWE
    32645,  // OCR_SIZENS
    32646,  // OCR_SIZEALL
    32647,  // OCR_ICOCUR
    32648,  // OCR_NO
    32649,  // OCR_HAND
    32650,  // OCR_APPSTARTING
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
    HCURSOR blank = CreateBlankCursor();
    if (!blank) {
        Wh_Log(L"CreateCursor failed");
        return;
    }
    for (int id : kSystemCursorIds) {
        HCURSOR copy =
            (HCURSOR)CopyImage(blank, IMAGE_CURSOR, 0, 0, LR_DEFAULTSIZE);
        if (copy) {
            SetSystemCursor(copy, id);
        }
    }
    DestroyCursor(blank);
    Wh_Log(L"Cursors hidden");
}

// Reloads the current cursor scheme; runtime replacements are dropped.
void RestoreCursors() {
    SystemParametersInfo(SPI_SETCURSORS, 0, nullptr, 0);
    Wh_Log(L"Cursors restored");
}

// Low-level hooks are always invoked on the thread that installed them,
// while that thread pumps messages.
LRESULT CALLBACK LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        g_lastActivity = GetTickCount64();
        if (g_hidden) {
            g_hidden = false;
            RestoreCursors();
        }
    }
    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

DWORD WINAPI WorkerThread(LPVOID /*param*/) {
    g_lastActivity = GetTickCount64();
    GetCursorPos(&g_lastPolledPos);

    // The low-level hook needs a message pump, so it is installed on
    // this thread and the idle check shares the same message loop.
    HHOOK mouseHook = SetWindowsHookEx(WH_MOUSE_LL, LowLevelMouseProc,
                                       GetModuleHandle(nullptr), 0);
    if (!mouseHook) {
        Wh_Log(L"SetWindowsHookEx failed");
        return 1;
    }
    Wh_Log(L"Mouse hook installed");

    bool stopping = false;
    while (!stopping) {
        // Wake up on input (needed for the hook), on the stop event, or
        // after roughly the interval used for the idle check. While the
        // cursor is hidden the loop polls more often so that movement is
        // noticed quickly even when the hook misses it (see below).
        DWORD wait =
            MsgWaitForMultipleObjects(1, &g_stopEvent, FALSE,
                                       g_hidden ? 100 : 500, QS_ALLINPUT);
        if (wait == WAIT_OBJECT_0) {
            stopping = true;
        }

        MSG msg;
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        if (stopping) {
            break;
        }

        // Low-level hooks don't receive mouse input that is destined for
        // windows running at a higher integrity level than this process
        // (for example, the elevated Windhawk window), so the hook alone
        // would miss movement over such windows: the mod would keep the
        // cursor hidden while the user works there. Polling the cursor
        // position covers that gap; it reads global state and needs no
        // special privileges.
        POINT polledPos;
        if (GetCursorPos(&polledPos) &&
            (polledPos.x != g_lastPolledPos.x ||
             polledPos.y != g_lastPolledPos.y)) {
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
            now - g_lastActivity >= (ULONGLONG)idleSeconds * 1000) {
            HideCursors();
            g_hidden = true;

            // Input may have arrived while the cursors were being
            // replaced; if so, show them again right away.
            if (GetTickCount64() - g_lastActivity < 250) {
                g_hidden = false;
                RestoreCursors();
            }
        }
    }

    UnhookWindowsHookEx(mouseHook);
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
    // Wait for the worker thread without a timeout: it may be inside the
    // hook or restoring cursors, and unloading the mod while it runs
    // would crash the process.
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