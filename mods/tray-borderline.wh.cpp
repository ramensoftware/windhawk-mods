// ==WindhawkMod==
// @id             tray-borderline
// @name           Tray Borderline
// @description    Add a tray button to open an app.
// @version        1.0.1
// @author         allelimo
// @github         https://github.com/allelimo
// @include        windhawk.exe
// @compilerOptions -lole32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Tray Borderline

![Screenshot](https://i.imgur.com/YSu5aBK.jpeg)

Click the tray icon to instantly recover all the "lost" windows.

This is a simple way to use the app "Borderline" by James Lin from the tray.

To use the mod you need to download Borderline.exe and link it in the Settings page.

Borderline is not opensource but it is available for free from his [Taenarum Software](https://www.taenarum.com/software/).

The mod is just a simple fork of the [Theme Toggler Tray](https://github.com/ramensoftware/windhawk-mods/blob/main/mods/theme-toggler-tray.wh.cpp) mod by [Husam Abdulraheem](https://github.com/Husam-Abdulraheem).

All credits goes to James Lin and Husam Abdulraheem.

*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- app_path: ""
  $name: Borderline.exe path
  $description: Full path to the executable to launch - Borderline.exe
- app_options: ""
  $name: Command line options
  $description: Optional arguments passed to the application. Leave empty for none.
- icon_file: shell32.dll
  $name: Icon File (DLL or EXE)
  $description: Full path to Icon resource (DLL or EXE)
- icon_index: 77
  $name: Icon Index
  $description: Icon Index refers to the icon resource index within the DLL/EXE.
- tooltip_text: Recover all the lost windows
  $name: Tooltip Text
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <shellapi.h>
#include <stdio.h>
#include <windhawk_utils.h>  //ai strings leaked

#define WM_USER_TRAYICON (WM_USER + 1)
#define WM_USER_UPDATESETTINGS (WM_USER + 2)

#define MENU_TITLE_BORDERLINE       9000
#define MENU_EXECUTE_BORDERLINE     9100
#define MENU_OPEN_WINDHAWK          9200

// --- Global Variables ---
HWND g_hWnd = NULL;
NOTIFYICONDATAW g_nid = {0};
HANDLE g_hThread = NULL;
UINT g_uMsgTaskbarCreated = 0;
// allelimo
//PCWSTR g_appPath;
//PCWSTR g_appOptions;

WindhawkUtils::StringSetting g_appPath;
WindhawkUtils::StringSetting g_appOptions;

static HINSTANCE           g_hInstance    = nullptr;
static WCHAR               g_windhawkPath[MAX_PATH] = {};

// --- Functional Logic ---
void ApplySettingsToTray() {
    // PCWSTR iconFile = Wh_GetStringSetting(L"icon_file");
    // int iconIndex = Wh_GetIntSetting(L"icon_index");
    // PCWSTR tooltipText = Wh_GetStringSetting(L"tooltip_text");
    // // allelimo
    // g_appPath = Wh_GetStringSetting(L"app_path");
    // g_appOptions = Wh_GetStringSetting(L"app_options");

    WindhawkUtils::StringSetting iconFile =
        WindhawkUtils::StringSetting::make(L"icon_file");
    int iconIndex = Wh_GetIntSetting(L"icon_index");
    WindhawkUtils::StringSetting tooltipText =
        WindhawkUtils::StringSetting::make(L"tooltip_text");

    g_appPath = WindhawkUtils::StringSetting::make(L"app_path");
    g_appOptions = WindhawkUtils::StringSetting::make(L"app_options");



    // if (!*g_appOptions) {
    //     g_appOptions = nullptr;  // Wh_GetStringSetting returns L"" when unset, never NULL
    // }

    HICON hOldIcon = g_nid.hIcon;
    ExtractIconExW(iconFile, iconIndex, NULL, &g_nid.hIcon, 1);
    if (!g_nid.hIcon) g_nid.hIcon = (HICON)LoadImageW(NULL, iconFile, IMAGE_ICON, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE);
    if (!g_nid.hIcon) g_nid.hIcon = LoadIcon(NULL, IDI_INFORMATION);

    lstrcpynW(g_nid.szTip, tooltipText, ARRAYSIZE(g_nid.szTip));
    if (!Shell_NotifyIconW(NIM_MODIFY, &g_nid)) Shell_NotifyIconW(NIM_ADD, &g_nid);

    if (hOldIcon && hOldIcon != g_nid.hIcon) DestroyIcon(hOldIcon);
    // Wh_FreeStringSetting(iconFile);
    // Wh_FreeStringSetting(tooltipText);
    
}

// allelimo
void OpenApp() {
    if (!g_appPath || !*g_appPath) {
        Wh_Log(L"No app path configured");
        return;
    }

    HINSTANCE result =
        ShellExecuteW(nullptr, L"open", g_appPath, g_appOptions, nullptr, SW_SHOWNORMAL);
    if ((INT_PTR)result <= 32) {
        Wh_Log(L"ShellExecute failed: %d", (int)(INT_PTR)result);
    }
}

// main
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if (g_uMsgTaskbarCreated != 0 && uMsg == g_uMsgTaskbarCreated) {
        ApplySettingsToTray();
        return 0;
    }
    switch (uMsg) {
        case WM_USER_TRAYICON:
            if (LOWORD(lParam) == WM_LBUTTONUP) {
                
                OpenApp();  // allelimo
            } 
            else if(LOWORD(lParam) == WM_RBUTTONUP) {
                
                HMENU hMenu = CreatePopupMenu();
                AppendMenuW(hMenu, MF_STRING | MF_DISABLED | MF_GRAYED, MENU_TITLE_BORDERLINE, L"Tray Borderline");
                AppendMenuW(hMenu, MF_SEPARATOR, 0, nullptr);
                AppendMenuW(hMenu, MF_STRING, MENU_EXECUTE_BORDERLINE, L"Execute Tray Borderline");
                AppendMenuW(hMenu, MF_STRING, MENU_OPEN_WINDHAWK, L"Open Windhawk");

                POINT pt;
                GetCursorPos(&pt);
                SetForegroundWindow(hwnd);
                int cmd = TrackPopupMenu(hMenu, TPM_RETURNCMD | TPM_RIGHTBUTTON |
                    TPM_BOTTOMALIGN /*| TPM_RIGHTALIGN*/,
                    pt.x, pt.y, 0, hwnd, nullptr);
                PostMessageW(hwnd, WM_NULL, 0, 0);
                DestroyMenu(hMenu);

                if (cmd == MENU_EXECUTE_BORDERLINE){
                    OpenApp();
                }
                else if (cmd == MENU_OPEN_WINDHAWK) {
                    SHELLEXECUTEINFOW sei = {sizeof(sei)};
                    sei.lpFile = g_windhawkPath;
                    sei.nShow  = SW_SHOWNORMAL;
                    ShellExecuteExW(&sei);
                }
            }
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

    // initialize COM before calling ShellExecuteW
    HRESULT hrCo = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
        if (FAILED(hrCo) && hrCo != RPC_E_CHANGED_MODE) {
            Wh_Log(L"CoInitializeEx failed (0x%08X)", hrCo);
        return 1;
    }

    g_uMsgTaskbarCreated = RegisterWindowMessageW(L"TaskbarCreated");
    WNDCLASSW wc = {0};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = GetModuleHandleW(NULL);
    wc.lpszClassName = L"TrayBorderlineToolWindow";
    RegisterClassW(&wc);
    
    g_hWnd = CreateWindowExW(WS_EX_TOOLWINDOW, wc.lpszClassName, L"", WS_POPUP, 0, 0, 0, 0, NULL, NULL, wc.hInstance, NULL);
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

    g_hInstance = GetModuleHandleW(nullptr);
    switch (GetModuleFileNameW(g_hInstance, g_windhawkPath, ARRAYSIZE(g_windhawkPath))) {
        case 0:
        case ARRAYSIZE(g_windhawkPath):
            Wh_Log(L"GetModuleFileNameW failed");
            break;
    }

    g_hThread = CreateThread(NULL, 0, TrayThread, NULL, 0, NULL);
    return (g_hThread != NULL);
}

void WhTool_ModSettingsChanged() {
    if (g_hWnd) PostMessage(g_hWnd, WM_USER_UPDATESETTINGS, 0, 0);
}

void WhTool_ModUninit() {
    //if (g_hWnd) SendMessageW(g_hWnd, WM_CLOSE, 0, 0);
    if (g_hWnd) PostMessageW(g_hWnd, WM_CLOSE, 0, 0);
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
