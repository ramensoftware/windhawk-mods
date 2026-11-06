// ==WindhawkMod==
// @id              multi-dpi-cursor
// @name            Multi-DPI Cursor
// @description     Keeps the mouse cursor the same visual size across monitors with different display scaling
// @version         1.0
// @author          Marco Kretz
// @github          https://github.com/marco-kretz
// @homepage        https://github.com/marco-kretz/win11-multi-dpi-cursor
// @license         MIT
// @include         windhawk.exe
// @compilerOptions -lshcore -lshell32 -ladvapi32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Multi-DPI Cursor

Windows uses a single cursor size (in pixels) for all monitors, so on a mixed
setup (e.g. 100 % and 125 % scaling) the cursor looks smaller on the
higher-DPI monitor.

Whenever the cursor moves to another monitor, this mod sets the cursor size to
`baseSize × monitorDpi / 96`. At 125 % a base size of 32 becomes 40.

By default, the base size is the pointer size chosen in Windows Settings
(Accessibility > Mouse pointer and touch), so the Settings slider keeps working.
Set a base size in the mod settings to override it. While dragging the
slider, the cursor briefly jumps between the Settings size and the scaled
size, and settles once the slider is released.

Cursor movement is received via an out-of-context WinEvent hook, so there is
no polling and mouse input is never delayed. Disabling the mod restores the
pointer size from Windows Settings.

The size is applied live only and never written to the registry. It relies on
the undocumented `SystemParametersInfo(0x2029)` call used by the Settings app,
which a Windows update could change.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- baseSize: 0
  $name: Base cursor size
  $description: Cursor size in pixels at 100 % scaling (maximum 256). 0 uses the pointer size from Windows Settings.
  #! $min: 0
  #! $max: 256
*/
// ==/WindhawkModSettings==

#include <shellscalingapi.h>
#include <stdio.h>

#include <atomic>

// Undocumented SPI used by Settings > Accessibility > Mouse pointer size.
constexpr UINT SPI_SETCURSORSIZE = 0x2029;
constexpr UINT WM_APP_SETTINGS = WM_APP + 1;

std::atomic<UINT> g_baseSize;  // 0: use the Windows Settings size
std::atomic<HWND> g_hwnd;
std::atomic<bool> g_stopping;
HANDLE g_thread;
HMONITOR g_lastMonitor;  // only touched on the window thread

void SetCursorSize(UINT size) {
    if (!SystemParametersInfo(SPI_SETCURSORSIZE, 0, (PVOID)(UINT_PTR)size, 0)) {
        Wh_Log(L"SystemParametersInfo(%u) failed: %u", size, GetLastError());
    }
}

// The size from Settings, which our live-only SPI calls never overwrite.
UINT GetWindowsCursorSize() {
    DWORD size = 0;
    DWORD cb = sizeof(size);
    if (RegGetValue(HKEY_CURRENT_USER, L"Control Panel\\Cursors",
                    L"CursorBaseSize", RRF_RT_REG_DWORD, nullptr, &size,
                    &cb) != ERROR_SUCCESS ||
        size == 0) {
        return 32;
    }
    return size;
}

void LoadSettings() {
    int size = Wh_GetIntSetting(L"baseSize");
    // Settings caps at 256 px, and 1.7.3 doesn't enforce $min/$max.
    g_baseSize = size <= 0 ? 0 : size > 256 ? 256 : size;
}

void Update() {
    POINT pt;
    GetCursorPos(&pt);
    HMONITOR monitor = MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
    if (monitor == g_lastMonitor) {
        return;  // cheap path for every mouse move on the same screen
    }
    g_lastMonitor = monitor;

    UINT dpi, dpiY;
    HRESULT hr = GetDpiForMonitor(monitor, MDT_EFFECTIVE_DPI, &dpi, &dpiY);
    if (FAILED(hr)) {
        Wh_Log(L"GetDpiForMonitor failed: 0x%08X", hr);
        return;
    }
    UINT baseSize = g_baseSize;
    if (!baseSize) {
        baseSize = GetWindowsCursorSize();
    }
    UINT size = MulDiv(baseSize, dpi, 96);
    Wh_Log(L"Monitor %p at (%d, %d): dpi=%u, size=%u", monitor, pt.x, pt.y,
           dpi, size);
    SetCursorSize(size);
}

void CALLBACK CursorMoved(HWINEVENTHOOK, DWORD, HWND, LONG idObject, LONG,
                          DWORD, DWORD) {
    if (idObject == OBJID_CURSOR) {
        Update();
    }
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        // Scaling or cursor changes keep the same HMONITOR, so force a refresh.
        // WM_SETTINGCHANGE also covers the Settings pointer size slider.
        case WM_DISPLAYCHANGE:
        case WM_SETTINGCHANGE:
        case WM_DPICHANGED:
        case WM_APP_SETTINGS:
            g_lastMonitor = nullptr;
            Update();
            return 0;
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

// Out-of-context WinEvents instead of polling: no wakeups while the mouse is
// idle, and unlike a low-level hook they are delivered asynchronously, so they
// can never stall mouse input. Raw input (RIDEV_INPUTSINK) was tried first, but
// stopped delivering mouse moves after dragging some windows until the next click.
DWORD WINAPI WindowThread(LPVOID) {
    // Otherwise every monitor reports 96 DPI.
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

    WNDCLASS wc{
        .lpfnWndProc = WndProc,
        .hInstance = GetModuleHandle(nullptr),
        .lpszClassName = L"MultiDPICursor_" WH_MOD_ID,
    };
    RegisterClass(&wc);

    // A real (hidden) top-level window, since message-only windows don't get
    // broadcasts like WM_DISPLAYCHANGE.
    HWND hwnd = CreateWindowEx(0, wc.lpszClassName, L"", 0, 0, 0, 0, 0,
                               nullptr, nullptr, wc.hInstance, nullptr);
    if (!hwnd) {
        Wh_Log(L"CreateWindowEx failed: %u", GetLastError());
        return 1;
    }

    HWINEVENTHOOK hook =
        SetWinEventHook(EVENT_OBJECT_LOCATIONCHANGE, EVENT_OBJECT_LOCATIONCHANGE,
                        nullptr, CursorMoved, 0, 0, WINEVENT_OUTOFCONTEXT);
    if (!hook) {
        Wh_Log(L"SetWinEventHook failed: %u", GetLastError());
        DestroyWindow(hwnd);
        return 1;
    }

    g_hwnd = hwnd;
    if (g_stopping) {
        DestroyWindow(hwnd);  // Uninit ran before the window existed.
    }
    Update();

    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0) > 0) {
        DispatchMessage(&msg);
    }

    UnhookWinEvent(hook);
    UnregisterClass(wc.lpszClassName, wc.hInstance);
    return 0;
}

BOOL WhTool_ModInit() {
    LoadSettings();
    g_thread = CreateThread(nullptr, 0, WindowThread, nullptr, 0, nullptr);
    return g_thread != nullptr;
}

void WhTool_ModSettingsChanged() {
    LoadSettings();
    if (HWND hwnd = g_hwnd) {
        PostMessage(hwnd, WM_APP_SETTINGS, 0, 0);
    }
}

void WhTool_ModUninit() {
    g_stopping = true;
    if (HWND hwnd = g_hwnd) {
        PostMessage(hwnd, WM_CLOSE, 0, 0);
    }
    WaitForSingleObject(g_thread, INFINITE);
    SetCursorSize(GetWindowsCursorSize());
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
