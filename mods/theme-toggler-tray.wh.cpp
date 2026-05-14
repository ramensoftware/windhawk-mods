// ==WindhawkMod==
// @id             theme-toggler-tray
// @name           Theme Toggler Tray
// @description    Add a system tray button to toggle between Light and Dark mode instantly.
// @version        1.2.1
// @author         Husam-Abdulraheem
// @github         https://github.com/Husam-Abdulraheem
// @include        windhawk.exe
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Theme Toggler Tray (Standalone Mode)

This mod now runs in a dedicated Windhawk process for better stability.
Click the tray icon to instantly toggle between Dark and Light mode.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- icon_file: shell32.dll
  $name: Icon File (DLL or EXE)
- icon_index: 25
  $name: Icon Index
- tooltip_text: Toggle Light/Dark Theme
  $name: Tooltip Text
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <shellapi.h>
#include <stdio.h>

#define WM_USER_TRAYICON (WM_USER + 1)
#define WM_USER_UPDATESETTINGS (WM_USER + 2)

// --- Global Variables ---
HWND g_hWnd = NULL;
NOTIFYICONDATAW g_nid = {0};
HANDLE g_hThread = NULL;
UINT g_uMsgTaskbarCreated = 0;

// --- Functional Logic ---

void ToggleTheme() {
    HKEY hKey;
    LPCWSTR path = L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize";
    if (RegOpenKeyExW(HKEY_CURRENT_USER, path, 0, KEY_READ | KEY_WRITE, &hKey) == ERROR_SUCCESS) {
        DWORD value = 0;
        DWORD size = sizeof(DWORD);
        if (RegQueryValueExW(hKey, L"SystemUsesLightTheme", NULL, NULL, (LPBYTE)&value, &size) == ERROR_SUCCESS) {
            DWORD newValue = (value == 0) ? 1 : 0;
            RegSetValueExW(hKey, L"SystemUsesLightTheme", 0, REG_DWORD, (const BYTE*)&newValue, sizeof(newValue));
            RegSetValueExW(hKey, L"AppsUseLightTheme", 0, REG_DWORD, (const BYTE*)&newValue, sizeof(newValue));
            SendMessageTimeoutW(HWND_BROADCAST, WM_SETTINGCHANGE, 0, (LPARAM)L"ImmersiveColorSet", SMTO_ABORTIFHUNG, 5000, NULL);
        }
        RegCloseKey(hKey);
    }
}

void ApplySettingsToTray() {
    PCWSTR iconFile = Wh_GetStringSetting(L"icon_file");
    int iconIndex = Wh_GetIntSetting(L"icon_index");
    PCWSTR tooltipText = Wh_GetStringSetting(L"tooltip_text");

    HICON hOldIcon = g_nid.hIcon;
    ExtractIconExW(iconFile, iconIndex, NULL, &g_nid.hIcon, 1);
    if (!g_nid.hIcon) g_nid.hIcon = (HICON)LoadImageW(NULL, iconFile, IMAGE_ICON, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE);
    if (!g_nid.hIcon) g_nid.hIcon = LoadIcon(NULL, IDI_INFORMATION);

    wcscpy_s(g_nid.szTip, tooltipText);
    if (!Shell_NotifyIconW(NIM_MODIFY, &g_nid)) Shell_NotifyIconW(NIM_ADD, &g_nid);

    if (hOldIcon && hOldIcon != g_nid.hIcon) DestroyIcon(hOldIcon);
    Wh_FreeStringSetting(iconFile);
    Wh_FreeStringSetting(tooltipText);
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if (g_uMsgTaskbarCreated != 0 && uMsg == g_uMsgTaskbarCreated) {
        Shell_NotifyIconW(NIM_ADD, &g_nid);
        ApplySettingsToTray();
        return 0;
    }
    switch (uMsg) {
        case WM_USER_TRAYICON:
            if (LOWORD(lParam) == WM_LBUTTONUP) ToggleTheme();
            return 0;
        case WM_USER_UPDATESETTINGS:
            ApplySettingsToTray();
            return 0;
        case WM_DESTROY:
            Shell_NotifyIconW(NIM_DELETE, &g_nid);
            if (g_nid.hIcon) DestroyIcon(g_nid.hIcon);
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProcW(hwnd, uMsg, wParam, lParam);
}

DWORD WINAPI TrayThread(LPVOID lpParam) {
    g_uMsgTaskbarCreated = RegisterWindowMessageW(L"TaskbarCreated");
    WNDCLASSW wc = {0};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = GetModuleHandleW(NULL);
    wc.lpszClassName = L"ThemeTogglerToolWindow";
    RegisterClassW(&wc);
    
    g_hWnd = CreateWindowExW(0, wc.lpszClassName, L"", 0, 0, 0, 0, 0, HWND_MESSAGE, NULL, wc.hInstance, NULL);
    if (g_hWnd) {
        g_nid.cbSize = sizeof(g_nid);
        g_nid.hWnd = g_hWnd;
        g_nid.uID = 1001;
        g_nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
        g_nid.uCallbackMessage = WM_USER_TRAYICON;
        Shell_NotifyIconW(NIM_ADD, &g_nid);
        ApplySettingsToTray();
        
        MSG msg;
        while (GetMessage(&msg, NULL, 0, 0)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
    return 0;
}

// --- WhTool Callbacks ---

BOOL WhTool_ModInit() {
    g_hThread = CreateThread(NULL, 0, TrayThread, NULL, 0, NULL);
    return (g_hThread != NULL);
}

void WhTool_ModSettingsChanged() {
    if (g_hWnd) PostMessage(g_hWnd, WM_USER_UPDATESETTINGS, 0, 0);
}

void WhTool_ModUninit() {
    if (g_hWnd) SendMessageW(g_hWnd, WM_CLOSE, 0, 0);
    if (g_hThread) {
        WaitForSingleObject(g_hThread, 2000);
        CloseHandle(g_hThread);
    }
}

// --- Windhawk Tool Mod Boilerplate (Do not modify) ---

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
