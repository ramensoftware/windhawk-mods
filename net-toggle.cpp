// ==WindhawkMod==
// @id              net-toggle
// @name            Network Toggle
// @description     Adds a network toggle to the system tray - click the icon to enable/disable network adapters
// @version         1.0
// @author          BlackPaw
// @github          https://github.com/blackpaw21
// @include         explorer.exe
// @architecture    x86-64
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Network Toggle

Adds a native-looking network toggle to the system tray for quick enable/disable of network adapters.

## Features

- **Hardware-Level Kill Switch:** Instantly disable or enable all physical network interface cards (NICs) with a single click.
- **Virtual-Environment Aware:** Automatically ignores software adapters like Hyper-V, WSL, and VM switches. It only targets actual hardware.
- **Native OS Integration:** Extracts official Windows network status icons directly from system DLLs for a seamless, built-in look.

*/
// ==/WindhawkModReadme==

#include <windhawk_utils.h>
#include <windows.h>
#include <shellapi.h>

#pragma comment(lib, "shell32.lib")

#define TRAY_ICON_ID 1
#define WM_TRAY_CALLBACK (WM_USER + 1)
#define WM_UPDATE_TRAY_STATE (WM_USER + 2)

const DWORD CLICK_DEBOUNCE_MS = 2000;

static volatile LONG g_isProcessingClick = 0;
static volatile LONG g_trayIconInstalled = 0;
static volatile LONG g_networkIsUp = 1; // 1 = ON, 0 = OFF
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
    if (!CreatePipe(&stdoutRead, &stdoutWrite, &sa, 0)) return TRUE; // Default to TRUE on fail
    SetHandleInformation(stdoutRead, HANDLE_FLAG_INHERIT, 0);

    PROCESS_INFORMATION pi = {};
    STARTUPINFOW si = {sizeof(si)};
    si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;
    si.hStdOutput = stdoutWrite;
    si.hStdError = stdoutWrite;

    BOOL isUp = TRUE;

    // Check if adapters are NOT disabled, rather than fully 'Up', to account for connection delays
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

BOOL RunPowerShellCommand(LPCWSTR psCommand, BOOL targetState) {
    Wh_Log(L"Executing PowerShell command");

    WCHAR cmdArgs[2048];
    if (wsprintfW(cmdArgs, L"-NoProfile -NonInteractive -WindowStyle Hidden -Command \"%s\"", psCommand) <= 0) {
        Wh_Log(L"Failed to format command");
        return FALSE;
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
        return FALSE;
    }

    // --- INSTANT UI UPDATE ---
    // At this exact line of code, the user just clicked "Yes" on the UAC prompt.
    // The PowerShell script is starting up, but the adapters haven't turned off yet.
    // We update the tray icon immediately so it feels perfectly responsive.
    Wh_Log(L"UAC cleared. Updating UI to target state: %d", targetState);
    InterlockedExchange(&g_networkIsUp, targetState ? 1 : 0);
    if (g_trayHwnd) {
        PostMessageW(g_trayHwnd, WM_UPDATE_TRAY_STATE, (WPARAM)targetState, 0);
    }

    if (!sei.hProcess) {
        Wh_Log(L"No process handle returned");
        return FALSE;
    }

    // Now we wait for the script to finish doing the actual heavy lifting
    DWORD waitResult = WaitForSingleObject(sei.hProcess, 20000);
    if (waitResult == WAIT_TIMEOUT) {
        Wh_Log(L"Process timed out, terminating");
        TerminateProcess(sei.hProcess, 1);
        CloseHandle(sei.hProcess);
        return FALSE;
    }

    DWORD exitCode = 1;
    if (!GetExitCodeProcess(sei.hProcess, &exitCode)) {
        LogLastError(L"GetExitCodeProcess");
        CloseHandle(sei.hProcess);
        return FALSE;
    }

    CloseHandle(sei.hProcess);
    Wh_Log(L"Process exited with code: %d", exitCode);
    return exitCode == 0;
}

// Background thread so the Tray UI doesn't freeze during UAC prompt
DWORD WINAPI WorkerThreadProc(LPVOID lpParam) {
    BOOL enable = (BOOL)(UINT_PTR)lpParam;
    
    Wh_Log(L"Toggling network adapters: %s", enable ? L"ENABLE" : L"DISABLE");
    LPCWSTR command = enable 
        ? L"Get-NetAdapter -Physical | Enable-NetAdapter -Confirm:$false"
        : L"Get-NetAdapter -Physical | Disable-NetAdapter -Confirm:$false";

    BOOL success = RunPowerShellCommand(command, enable);
    
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

    // Calculate our desired state based on current knowledge
    BOOL targetState = (g_networkIsUp == 0); 
    Wh_Log(L"Processing network toggle click. Target state: %s", targetState ? L"ON" : L"OFF");

    // Spawn worker thread to handle the heavy lifting (UAC & Execution)
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

BOOL Wh_ModInit() {
    Wh_Log(L"Network Toggle Mod Init");

    g_hInstance = GetModuleHandle(nullptr);
    if (!g_hInstance) {
        Wh_Log(L"Failed to get module handle");
        return FALSE;
    }

    // Extract the official Windows Network Connected/Disconnected icons
    // pnidui.dll (Personal Network Indicator UI) contains the actual system tray network icons
    // Index 4: Ethernet Connected (Computer monitor with cable)
    // Index 5: Ethernet Disconnected (Computer monitor with red X)
    ExtractIconExW(L"pnidui.dll", 4, nullptr, &g_iconEnabled, 1);  
    ExtractIconExW(L"pnidui.dll", 5, nullptr, &g_iconDisabled, 1); 

    // Safe fallbacks just in case the system is missing pnidui
    if (!g_iconEnabled) ExtractIconExW(L"shell32.dll", 9, nullptr, &g_iconEnabled, 1);
    if (!g_iconDisabled) ExtractIconExW(L"shell32.dll", 131, nullptr, &g_iconDisabled, 1);

    Wh_Log(L"Icons loaded successfully.");

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

    return TRUE;
}

void Wh_ModUninit() {
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

    Wh_Log(L"Network Toggle Mod Uninit complete");
}
