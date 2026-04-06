// ==WindhawkMod==
// @id              net-toggle
// @name            Network Toggle
// @description     Adds a network toggle to the system tray - click the icon to enable/disable network adapters
// @version         1.0
// @author          BlackPaw
// @github          https://github.com/BlackPaw21
// @include         windhawk.exe
// @compilerOptions -lshell32 -lgdi32 -luser32 -lwtsapi32 -lwininet
// ==/WindhawkMod==

#include <windhawk_utils.h>
#include <windows.h>
#include <shellapi.h>
#include <wininet.h>
#include <wtsapi32.h>

#ifdef _MSC_VER
#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "wtsapi32.lib")
#endif

#define TRAY_ICON_ID 1
#define WM_TRAY_CALLBACK (WM_USER + 1)
#define WM_UPDATE_TRAY_STATE (WM_USER + 2)
#define WM_RUN_ELEVATED (WM_USER + 3)

const DWORD CLICK_DEBOUNCE_MS = 2000;

static volatile LONG g_isProcessingClick = 0;
static volatile LONG g_trayIconInstalled = 0;
static volatile LONG g_networkIsUp = 1;
static HANDLE g_trayThread = nullptr;
static HWND g_trayHwnd = nullptr;
static HINSTANCE g_hInstance = nullptr;
static HICON g_iconEnabled = nullptr;
static HICON g_iconDisabled = nullptr;
static DWORD g_lastClickTime = 0;

void LogLastError(LPCWSTR context) {
    DWORD error = GetLastError();
    if (error == 0) return;
    
    LPWSTR errorMsg = nullptr;
    if (FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                       nullptr, error, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                       (LPWSTR)&errorMsg, 0, nullptr)) {
        Wh_Log(L"[ERROR] %s: %s (0x%X)", context, errorMsg, error);
        LocalFree(errorMsg);
    } else {
        Wh_Log(L"[ERROR] %s: Error %d", context, error);
    }
}

BOOL CheckActualNetworkState() {
    SECURITY_ATTRIBUTES sa = {sizeof(sa), nullptr, TRUE};
    HANDLE stdoutRead, stdoutWrite;
    if (!CreatePipe(&stdoutRead, &stdoutWrite, &sa, 0)) return TRUE;
    SetHandleInformation(stdoutRead, HANDLE_FLAG_INHERIT, 0);

    PROCESS_INFORMATION pi = {};
    STARTUPINFOW si = {sizeof(si)};
    si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;
    si.hStdOutput = stdoutWrite;
    si.hStdError = stdoutWrite;

    BOOL isUp = TRUE;

    if (CreateProcessW(L"C:\\Windows\\System32\\WindowsPowerShell\\v1.0\\powershell.exe", 
                       (LPWSTR)L"powershell.exe -NoProfile -NonInteractive -Command \"(Get-NetAdapter -Physical | Where-Object Status -ne 'Disabled').Count\"",
                       nullptr, nullptr, TRUE, CREATE_NO_WINDOW, nullptr, nullptr, &si, &pi)) {
        CloseHandle(stdoutWrite);
        char buffer[128] = {0};
        DWORD bytesRead;
        if (ReadFile(stdoutRead, buffer, sizeof(buffer) - 1, &bytesRead, nullptr) && bytesRead > 0) {
            int count = atoi(buffer);
            isUp = (count > 0);
        }
        CloseHandle(stdoutRead);
        WaitForSingleObject(pi.hProcess, 5000);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    } else {
        CloseHandle(stdoutRead);
        CloseHandle(stdoutWrite);
    }
    
    return isUp;
}

BOOL RunPowerShellCommandAsSystem(LPCWSTR psCommand, BOOL targetState) {
    Wh_Log(L"Executing PowerShell command with Windhawk privileges");

    WCHAR cmdLine[2048];
    if (wsprintfW(cmdLine, L"\"C:\\Windows\\System32\\WindowsPowerShell\\v1.0\\powershell.exe\" -NoProfile -NonInteractive -Command \"%s 2>&1\"", psCommand) <= 0) {
        Wh_Log(L"Failed to format command");
        return FALSE;
    }

    HMODULE kernel32 = GetModuleHandle(L"kernel32");
    if (!kernel32) {
        Wh_Log(L"Failed to get kernel32 handle");
        return FALSE;
    }

    typedef BOOL(WINAPI* CreateProcessAsUserW_t)(HANDLE, LPCWSTR, LPWSTR, LPSECURITY_ATTRIBUTES, 
        LPSECURITY_ATTRIBUTES, BOOL, DWORD, LPVOID, LPCWSTR, LPSTARTUPINFOW, LPPROCESS_INFORMATION);
    CreateProcessAsUserW_t pCreateProcessAsUserW = 
        (CreateProcessAsUserW_t)GetProcAddress(kernel32, "CreateProcessAsUserW");

    HANDLE hProcessToken = NULL;
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ALL_ACCESS, &hProcessToken)) {
        Wh_Log(L"OpenProcessToken failed: %d", GetLastError());
        return FALSE;
    }

    SECURITY_ATTRIBUTES sa = {sizeof(sa), nullptr, TRUE};
    HANDLE stdoutRead, stdoutWrite;
    if (!CreatePipe(&stdoutRead, &stdoutWrite, &sa, 0)) {
        Wh_Log(L"CreatePipe failed");
        CloseHandle(hProcessToken);
        return FALSE;
    }
    SetHandleInformation(stdoutRead, HANDLE_FLAG_INHERIT, 0);

    STARTUPINFOW si{sizeof(si)};
    si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
    si.wShowWindow = SW_HIDE;
    si.hStdOutput = stdoutWrite;
    si.hStdError = stdoutWrite;
    
    PROCESS_INFORMATION pi{};
    
    BOOL success = FALSE;
    if (pCreateProcessAsUserW) {
        success = pCreateProcessAsUserW(hProcessToken, nullptr, cmdLine,
            nullptr, nullptr, TRUE, CREATE_NO_WINDOW | CREATE_UNICODE_ENVIRONMENT,
            nullptr, L"C:\\Windows\\System32", &si, &pi);
    }
    
    CloseHandle(hProcessToken);
    
    if (!success) {
        Wh_Log(L"CreateProcessAsUserW failed, trying direct: %d", GetLastError());
        CloseHandle(stdoutWrite);
        CloseHandle(stdoutRead);
        
        if (!CreateProcessW(nullptr, cmdLine, nullptr, nullptr, TRUE, 
            CREATE_NO_WINDOW, nullptr, L"C:\\Windows\\System32", &si, &pi)) {
            Wh_Log(L"CreateProcess failed: %d", GetLastError());
            return FALSE;
        }
        success = TRUE;
    }

    CloseHandle(stdoutWrite);

    Wh_Log(L"Process started with PID: %d", pi.dwProcessId);

    InterlockedExchange(&g_networkIsUp, targetState ? 1 : 0);
    if (g_trayHwnd) {
        PostMessageW(g_trayHwnd, WM_UPDATE_TRAY_STATE, (WPARAM)targetState, 0);
    }

    if (!pi.hProcess) {
        Wh_Log(L"No process handle returned");
        CloseHandle(stdoutRead);
        return FALSE;
    }

    DWORD waitResult = WaitForSingleObject(pi.hProcess, 20000);
    
    char output[4096] = {0};
    DWORD bytesRead;
    if (ReadFile(stdoutRead, output, sizeof(output) - 1, &bytesRead, nullptr) && bytesRead > 0) {
        output[bytesRead] = '\0';
        Wh_Log(L"PowerShell output: %S", output);
    }
    CloseHandle(stdoutRead);

    if (waitResult == WAIT_TIMEOUT) {
        Wh_Log(L"Process timed out, terminating");
        TerminateProcess(pi.hProcess, 1);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        return FALSE;
    }

    DWORD exitCode = 1;
    if (!GetExitCodeProcess(pi.hProcess, &exitCode)) {
        LogLastError(L"GetExitCodeProcess");
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        return FALSE;
    }

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    Wh_Log(L"Process exited with code: %d", exitCode);
    return exitCode == 0;
}

DWORD WINAPI WorkerThreadProc(LPVOID lpParam) {
    BOOL enable = (BOOL)(UINT_PTR)lpParam;
    
    Wh_Log(L"Toggling network adapters: %s", enable ? L"ENABLE" : L"DISABLE");
    
    WCHAR cmdArgs[2048];
    LPCWSTR psCommand = enable 
        ? L"Get-NetAdapter -Physical | Enable-NetAdapter -Confirm:$false"
        : L"Get-NetAdapter -Physical | Disable-NetAdapter -Confirm:$false";
    
    if (wsprintfW(cmdArgs, L"-NoProfile -NonInteractive -WindowStyle Hidden -Command \"%s\"", psCommand) <= 0) {
        Wh_Log(L"Failed to format command");
        InterlockedExchange(&g_isProcessingClick, 0);
        return 0;
    }

    SHELLEXECUTEINFOW sei = {sizeof(sei)};
    sei.fMask = SEE_MASK_NOCLOSEPROCESS | SEE_MASK_NO_CONSOLE;
    sei.hwnd = nullptr;
    sei.lpVerb = L"runas";
    sei.lpFile = L"powershell.exe";
    sei.lpParameters = cmdArgs;
    sei.nShow = SW_HIDE;

    if (!ShellExecuteExW(&sei)) {
        DWORD error = GetLastError();
        if (error == ERROR_CANCELLED) {
            Wh_Log(L"UAC prompt cancelled by user");
        } else {
            Wh_Log(L"ShellExecuteEx failed: %d", error);
        }
        InterlockedExchange(&g_isProcessingClick, 0);
        return 0;
    }

    Wh_Log(L"UAC cleared. Updating UI to target state: %d", enable);
    InterlockedExchange(&g_networkIsUp, enable ? 1 : 0);
    if (g_trayHwnd) {
        PostMessageW(g_trayHwnd, WM_UPDATE_TRAY_STATE, (WPARAM)enable, 0);
    }

    if (!sei.hProcess) {
        Wh_Log(L"No process handle returned");
        InterlockedExchange(&g_isProcessingClick, 0);
        return 0;
    }

    DWORD waitResult = WaitForSingleObject(sei.hProcess, 20000);
    BOOL success = FALSE;
    if (waitResult == WAIT_TIMEOUT) {
        Wh_Log(L"Process timed out, terminating");
        TerminateProcess(sei.hProcess, 1);
    } else {
        DWORD exitCode = 1;
        if (GetExitCodeProcess(sei.hProcess, &exitCode)) {
            success = (exitCode == 0);
            Wh_Log(L"Process exited with code: %d", exitCode);
        }
    }

    CloseHandle(sei.hProcess);

    if (success) {
        Wh_Log(L"Network %s operation completed successfully", enable ? L"enable" : L"disable");
    } else {
        Wh_Log(L"Network toggle operation failed");
    }

    if (success) {
        Wh_Log(L"Network %s operation completed successfully", enable ? L"enable" : L"disable");
    } else {
        Wh_Log(L"Network toggle operation failed or cancelled");
    }

    InterlockedExchange(&g_isProcessingClick, 0);
    return 0;
}

void UpdateTrayIconTooltip(BOOL enabled) {
    if (!g_trayHwnd) return;

    NOTIFYICONDATAW nid = {sizeof(nid)};
    nid.hWnd = g_trayHwnd;
    nid.uID = TRAY_ICON_ID;
    nid.uFlags = NIF_TIP | NIF_ICON;
    wsprintfW(nid.szTip, enabled ? L"Network Toggle: ON (Click to disable)" : L"Network Toggle: OFF (Click to enable)");
    nid.hIcon = enabled ? g_iconEnabled : g_iconDisabled;
    
    if (!Shell_NotifyIconW(NIM_MODIFY, &nid)) {
        LogLastError(L"NIM_MODIFY");
    }
}

void ProcessTrayClick() {
    if (InterlockedExchange(&g_isProcessingClick, 1) != 0) {
        Wh_Log(L"Already processing a click, ignoring");
        return;
    }

    DWORD now = GetTickCount();
    if (now - g_lastClickTime < CLICK_DEBOUNCE_MS) {
        Wh_Log(L"Click debounced (cooldown active)");
        InterlockedExchange(&g_isProcessingClick, 0);
        return;
    }
    g_lastClickTime = now;

    BOOL targetState = (g_networkIsUp == 0); 
    Wh_Log(L"Processing network toggle click. Target state: %s", targetState ? L"ON" : L"OFF");

    DWORD threadId;
    HANDLE hWorker = CreateThread(nullptr, 0, WorkerThreadProc, (LPVOID)(UINT_PTR)targetState, 0, &threadId);
    if (hWorker) {
        CloseHandle(hWorker); 
    } else {
        InterlockedExchange(&g_isProcessingClick, 0); 
    }
}

LRESULT CALLBACK TrayWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_TRAY_CALLBACK:
            if (lParam == WM_LBUTTONUP) {
                ProcessTrayClick();
            }
            return 0;

        case WM_UPDATE_TRAY_STATE:
            UpdateTrayIconTooltip((BOOL)wParam);
            return 0;

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProcW(hWnd, msg, wParam, lParam);
}

DWORD WINAPI TrayThreadProc(LPVOID) {
    Wh_Log(L"Tray thread started");

    BOOL initialState = CheckActualNetworkState();
    InterlockedExchange(&g_networkIsUp, initialState ? 1 : 0);

    WNDCLASSW wc = {0};
    wc.lpfnWndProc = TrayWndProc;
    wc.hInstance = g_hInstance;
    wc.lpszClassName = L"NetworkToggleWindowClass";
    wc.hIcon = initialState ? g_iconEnabled : g_iconDisabled;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);

    if (!RegisterClassW(&wc)) {
        LogLastError(L"RegisterClassW");
        return 1;
    }

    HWND hWnd = CreateWindowExW(
        WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
        wc.lpszClassName,
        L"Network Toggle",
        WS_POPUP,
        0, 0, 1, 1,
        nullptr, nullptr, g_hInstance, nullptr
    );

    if (!hWnd) {
        LogLastError(L"CreateWindowExW");
        UnregisterClassW(wc.lpszClassName, g_hInstance);
        return 1;
    }

    g_trayHwnd = hWnd;

    NOTIFYICONDATAW nid = {sizeof(nid)};
    nid.hWnd = hWnd;
    nid.uID = TRAY_ICON_ID;
    nid.uFlags = NIF_MESSAGE | NIF_TIP | NIF_ICON;
    nid.uCallbackMessage = WM_TRAY_CALLBACK;
    nid.hIcon = initialState ? g_iconEnabled : g_iconDisabled;
    wsprintfW(nid.szTip, initialState ? L"Network Toggle: ON (Click to disable)" : L"Network Toggle: OFF (Click to enable)");

    if (!Shell_NotifyIconW(NIM_ADD, &nid)) {
        LogLastError(L"Shell_NotifyIcon NIM_ADD");
        DestroyWindow(hWnd);
        UnregisterClassW(wc.lpszClassName, g_hInstance);
        return 1;
    }

    InterlockedExchange(&g_trayIconInstalled, 1);
    Wh_Log(L"Tray icon installed successfully. Initial state: %s", initialState ? L"ON" : L"OFF");

    MSG msg;
    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    Wh_Log(L"Tray message loop ending");

    Shell_NotifyIconW(NIM_DELETE, &nid);
    DestroyWindow(hWnd);
    UnregisterClassW(wc.lpszClassName, g_hInstance);
    InterlockedExchange(&g_trayIconInstalled, 0);
    g_trayHwnd = nullptr;

    return 0;
}

////////////////////////////////////////////////////////////////////////////////
// Windhawk tool mod implementation
bool g_isToolModProcessLauncher;
HANDLE g_toolModProcessMutex;

void WINAPI EntryPoint_Hook() {
    Wh_Log(L"Tool mod process started. Entering message loop.");

    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    Wh_Log(L"Tool mod message loop exited.");
}

BOOL WhTool_ModInit() {
    Wh_Log(L"Network Toggle Tool Mod Init");

    g_hInstance = GetModuleHandle(nullptr);
    if (!g_hInstance) {
        Wh_Log(L"Failed to get module handle");
        return FALSE;
    }

    ExtractIconExW(L"pnidui.dll", 4, nullptr, &g_iconEnabled, 1);  
    ExtractIconExW(L"pnidui.dll", 5, nullptr, &g_iconDisabled, 1); 

    if (!g_iconEnabled) ExtractIconExW(L"shell32.dll", 9, nullptr, &g_iconEnabled, 1);
    if (!g_iconDisabled) ExtractIconExW(L"shell32.dll", 131, nullptr, &g_iconDisabled, 1);

    Wh_Log(L"Icons loaded. Enabled: %p, Disabled: %p", g_iconEnabled, g_iconDisabled);

    DWORD threadId = 0;
    g_trayThread = CreateThread(nullptr, 0, TrayThreadProc, nullptr, CREATE_SUSPENDED, &threadId);

    if (!g_trayThread) {
        LogLastError(L"CreateThread");
        return FALSE;
    }

    SetThreadPriority(g_trayThread, THREAD_PRIORITY_ABOVE_NORMAL);

    if (ResumeThread(g_trayThread) == (DWORD)-1) {
        LogLastError(L"ResumeThread");
        CloseHandle(g_trayThread);
        g_trayThread = nullptr;
        return FALSE;
    }

    Wh_Log(L"Tray thread started successfully");
    return TRUE;
}

void WhTool_ModUninit() {
    Wh_Log(L"Network Toggle Mod Uninit");

    if (g_trayHwnd) {
        PostMessageW(g_trayHwnd, WM_DESTROY, 0, 0);
    }

    if (g_trayThread) {
        Wh_Log(L"Waiting for tray thread to exit...");
        
        DWORD waitResult = WaitForSingleObject(g_trayThread, 5000);
        if (waitResult == WAIT_TIMEOUT) {
            Wh_Log(L"Thread didn't exit in time, terminating");
            TerminateThread(g_trayThread, 1);
        }

        CloseHandle(g_trayThread);
        g_trayThread = nullptr;
    }

    if (g_iconEnabled) {
        DestroyIcon(g_iconEnabled);
        g_iconEnabled = nullptr;
    }
    if (g_iconDisabled) {
        DestroyIcon(g_iconDisabled);
        g_iconDisabled = nullptr;
    }

    PostQuitMessage(0);
    Wh_Log(L"Network Toggle Mod Uninit complete");
}

////////////////////////////////////////////////////////////////////////////////
// Windhawk tool mod bootstrap - runs in windhawk.exe process

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
        g_toolModProcessMutex = CreateMutex(nullptr, TRUE, L"windhawk-tool-mod_" WH_MOD_ID);
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

        IMAGE_DOS_HEADER* dosHeader = (IMAGE_DOS_HEADER*)GetModuleHandle(nullptr);
        IMAGE_NT_HEADERS* ntHeaders = (IMAGE_NT_HEADERS*)((BYTE*)dosHeader + dosHeader->e_lfanew);
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
    switch (GetModuleFileName(nullptr, currentProcessPath, ARRAYSIZE(currentProcessPath))) {
        case 0:
        case ARRAYSIZE(currentProcessPath):
            Wh_Log(L"GetModuleFileName failed");
            return;
    }

    WCHAR commandLine[MAX_PATH + 2 + (sizeof(L" -tool-mod \"" WH_MOD_ID "\"") / sizeof(WCHAR)) - 1];
    swprintf_s(commandLine, L"\"%s\" -tool-mod \"%s\"", currentProcessPath, WH_MOD_ID);

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
        LPSECURITY_ATTRIBUTES lpProcessAttributes, LPSECURITY_ATTRIBUTES lpThreadAttributes,
        WINBOOL bInheritHandles, DWORD dwCreationFlags, LPVOID lpEnvironment,
        LPCWSTR lpCurrentDirectory, LPSTARTUPINFOW lpStartupInfo,
        LPPROCESS_INFORMATION lpProcessInformation, PHANDLE hRestrictedUserToken);
    
    CreateProcessInternalW_t pCreateProcessInternalW =
        (CreateProcessInternalW_t)GetProcAddress(kernelModule, "CreateProcessInternalW");
    if (!pCreateProcessInternalW) {
        Wh_Log(L"No CreateProcessInternalW");
        return;
    }

    STARTUPINFO si{ .cb = sizeof(STARTUPINFO), .dwFlags = STARTF_FORCEOFFFEEDBACK };
    PROCESS_INFORMATION pi;
    if (!pCreateProcessInternalW(nullptr, currentProcessPath, commandLine,
                                 nullptr, nullptr, FALSE, CREATE_NO_WINDOW,
                                 nullptr, nullptr, &si, &pi, nullptr)) {
        Wh_Log(L"CreateProcess failed, error: %d", GetLastError());
        return;
    }

    Wh_Log(L"Launched tool mod process, PID: %d", pi.dwProcessId);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}

void WhTool_ModSettingsChanged() {
    Wh_Log(L"Settings changed");
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
