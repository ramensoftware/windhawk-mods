// ==WindhawkMod==
// @id              altf4-preferences
// @name            Alt+F4 Dialog Preferences
// @description     Choose which action is preselected in the Alt+F4 "Shut Down Windows" dialog
// @version         1.0.0
// @author          Marco Kraus
// @github          https://github.com/MaKra
// @homepage        https://www.kraus.tk
// @include         windhawk.exe
// @compilerOptions -lpowrprof -lshell32
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Alt+F4 Dialog Preferences

Sets the action which is preselected in the classic **"Shut Down Windows"**
dialog that appears when pressing `Alt+F4` on the desktop.

Instead of always starting with "Shut down", the dialog can preselect any of
its actions: 
 * Shut down
 * Restart
 * Sign out
 * Switch user
 * Sleep
 * Hibernate

## How it works

The dialog's preselected action follows the Start menu power button action,
stored per user in the registry value
`HKCU\Software\Microsoft\Windows\CurrentVersion\Explorer\Advanced\Start_PowerButtonAction`.
This mod writes that value according to the mod settings. The previous state is
restored whenever the mod is disabled or uninstalled in Windhawk - if the value
did not exist before, it is removed again.

Changes apply immediately: the dialog reads the value each time it opens, no
restart of Explorer or Windows is needed.

## Notes

- The same registry value also defines the default action of the Start menu
  power button; this side effect is inherent to the mechanism.
- Sleep and Hibernate only appear in the dialog if they are enabled under
  Control Panel -> Power Options -> System Settings ("Shutdown settings"
  checkboxes). If a state is not available, Windows falls back to "Shut down".
- This mod runs in a dedicated windhawk.exe process (tool mod) instead of
  being injected into Explorer.

## Compatibility

Works on Windows 10 and Windows 11. Tested on Windows 11 26H2 (build 26300).
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- DefaultAction: shutdown
  $name: Default action
  $description: The action preselected in the Alt+F4 Shut Down Windows dialog
  $options:
    - shutdown: Shut down
    - restart: Restart
    - signout: Sign out
    - switchuser: Switch user
    - sleep: Sleep
    - hibernate: Hibernate
*/
// ==/WindhawkModSettings==

#include <stdio.h>
#include <windhawk_api.h>

// PowrProf availability queries. Declared manually to be independent of the
// toolchain header state; both are exported by powrprof.dll since Windows XP.
extern "C" {
BOOL WINAPI IsPwrSuspendAllowed(void);
BOOL WINAPI IsPwrHibernateAllowed(void);
}

// Enum values are the registry values of Start_PowerButtonAction (hex).
enum class DefaultAction : DWORD {
    SignOut    = 0x1,
    ShutDown   = 0x2,
    Restart    = 0x4,
    Sleep      = 0x10,
    Hibernate  = 0x40,
    SwitchUser = 0x100,
};

constexpr PCWSTR kRegistrySubKey = L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced";
constexpr PCWSTR kRegistryValue  = L"Start_PowerButtonAction";

// Mod storage keys (Windhawk local storage, not the registry).
constexpr PCWSTR kStorageApplied        = L"applied";
constexpr PCWSTR kStorageOriginalExists = L"originalExists";
constexpr PCWSTR kStorageOriginalValue  = L"originalValue";

struct {
    DefaultAction action;
} g_settings;

HANDLE g_stopEvent;
HANDLE g_keepAliveThread;

DefaultAction ParseDefaultAction(PCWSTR action) {
    if (wcscmp(action, L"restart") == 0)
        return DefaultAction::Restart;
    if (wcscmp(action, L"signout") == 0)
        return DefaultAction::SignOut;
    if (wcscmp(action, L"switchuser") == 0)
        return DefaultAction::SwitchUser;
    if (wcscmp(action, L"sleep") == 0)
        return DefaultAction::Sleep;
    if (wcscmp(action, L"hibernate") == 0)
        return DefaultAction::Hibernate;
    return DefaultAction::ShutDown;
}

void LoadSettings() {
    PCWSTR action = Wh_GetStringSetting(L"DefaultAction");
    g_settings.action = ParseDefaultAction(action);
    Wh_Log(L"Settings: DefaultAction=%s (0x%08lX)", action, (unsigned long)g_settings.action);
    Wh_FreeStringSetting(action);
}

// Returns true on success. On a successful query, *exists reports whether the
// value is present and *value contains the current data (0 if absent).
bool QueryStartPowerButtonAction(bool* exists, DWORD* value) {
    *exists = false;
    *value = 0;

    HKEY hKey;
    LONG res = RegOpenKeyEx(HKEY_CURRENT_USER, kRegistrySubKey, 0, KEY_QUERY_VALUE, &hKey);
    if (res != ERROR_SUCCESS) {
        Wh_Log(L"RegOpenKeyEx failed (%ld)", res);
        return false;
    }

    DWORD type = 0;
    DWORD size = sizeof(DWORD);
    DWORD data = 0;
    res = RegQueryValueEx(hKey, kRegistryValue, nullptr, &type, (LPBYTE)&data, &size);
    RegCloseKey(hKey);

    if (res == ERROR_FILE_NOT_FOUND) {
        return true;
    }
    if (res != ERROR_SUCCESS) {
        Wh_Log(L"RegQueryValueEx failed (%ld)", res);
        return false;
    }
    if (type != REG_DWORD) {
        Wh_Log(L"Unexpected value type (%lu), ignoring", type);
        return false;
    }

    *exists = true;
    *value = data;
    return true;
}

bool WriteStartPowerButtonAction(DWORD value) {
    HKEY hKey;
    LONG res = RegOpenKeyEx(HKEY_CURRENT_USER, kRegistrySubKey, 0, KEY_SET_VALUE, &hKey);
    if (res != ERROR_SUCCESS) {
        Wh_Log(L"RegOpenKeyEx failed (%ld)", res);
        return false;
    }

    res = RegSetValueEx(hKey, kRegistryValue, 0, REG_DWORD, (const BYTE*)&value, sizeof(value));
    RegCloseKey(hKey);

    if (res != ERROR_SUCCESS) {
        Wh_Log(L"RegSetValueEx failed (%ld)", res);
        return false;
    }

    return true;
}

bool DeleteStartPowerButtonAction() {
    HKEY hKey;
    LONG res = RegOpenKeyEx(HKEY_CURRENT_USER, kRegistrySubKey, 0, KEY_SET_VALUE, &hKey);
    if (res != ERROR_SUCCESS) {
        Wh_Log(L"RegOpenKeyEx failed (%ld)", res);
        return false;
    }

    res = RegDeleteValue(hKey, kRegistryValue);
    RegCloseKey(hKey);

    if (res != ERROR_SUCCESS && res != ERROR_FILE_NOT_FOUND) {
        Wh_Log(L"RegDeleteValue failed (%ld)", res);
        return false;
    }

    return true;
}

// Restores the state recorded before the mod first applied its value:
// writes back the original value (Case B) or deletes the value it created
// (Case A). Called when the mod is unloaded (disabled/uninstalled/reloaded);
// only acts if the mod recorded having applied something.
void RestoreOriginal() {
    if (Wh_GetIntValue(kStorageApplied, 0) == 0) {
        return;
    }

    bool exists = Wh_GetIntValue(kStorageOriginalExists, 1) != 0;
    DWORD value = (DWORD)Wh_GetIntValue(kStorageOriginalValue, 0);

    bool ok;
    if (exists) {
        ok = WriteStartPowerButtonAction(value);
        Wh_Log(L"Restoring original value: 0x%08lX (%s)", value, ok ? L"ok" : L"FAILED");
    } else {
        ok = DeleteStartPowerButtonAction();
        Wh_Log(L"Restoring original state: value removed (%s)", ok ? L"ok" : L"FAILED");
    }

    if (ok) {
        Wh_SetIntValue(kStorageApplied, 0);
    }
}

void ApplyDefaultAction() {
    if (Wh_GetIntValue(kStorageApplied, 0) == 0) {
        bool exists;
        DWORD value;
        if (!QueryStartPowerButtonAction(&exists, &value)) {
            Wh_Log(L"Cannot record original state, not applying");
            return;
        }

        Wh_SetIntValue(kStorageOriginalExists, exists ? 1 : 0);
        Wh_SetIntValue(kStorageOriginalValue, (int)value);
        Wh_SetIntValue(kStorageApplied, 1);
        Wh_Log(L"Recorded original state: %s (0x%08lX)", exists ? L"present" : L"absent",
               value);
    }

    if (g_settings.action == DefaultAction::Sleep && !IsPwrSuspendAllowed()) {
        Wh_Log(L"WARNING: Sleep is not currently available (Power Options > System "
               L"Settings). Windows will fall back to Shut down.");
    }
    if (g_settings.action == DefaultAction::Hibernate && !IsPwrHibernateAllowed()) {
        Wh_Log(L"WARNING: Hibernate is not currently available (Power Options > System "
               L"Settings). Windows will fall back to Shut down.");
    }

    DWORD action = (DWORD)g_settings.action;
    if (WriteStartPowerButtonAction(action)) {
        Wh_Log(L"Default action applied: 0x%08lX", action);
    }
}

DWORD WINAPI KeepAliveThreadProc(LPVOID) {
    WaitForSingleObject(g_stopEvent, INFINITE);
    return 0;
}

BOOL WhTool_ModInit() {
    Wh_Log(L">");
    LoadSettings();
    ApplyDefaultAction();

    // Keep the tool process alive so settings changes reach us. The process
    // entry point is hooked by the tool mod runtime below and the main thread
    // ends there; this thread holds the process open.
    g_stopEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
    if (!g_stopEvent) {
        Wh_Log(L"CreateEvent failed");
        return FALSE;
    }

    g_keepAliveThread = CreateThread(nullptr, 0, KeepAliveThreadProc, nullptr, 0, nullptr);
    if (!g_keepAliveThread) {
        Wh_Log(L"CreateThread failed");
        return FALSE;
    }

    return TRUE;
}

void WhTool_ModSettingsChanged() {
    Wh_Log(L">");
    LoadSettings();
    ApplyDefaultAction();
}

void WhTool_ModUninit() {
    Wh_Log(L">");

    // Restore the state recorded before the mod first applied its value: the
    // native Windhawk enable/disable toggle (and uninstalling) is the off
    // switch. Uninit also runs on reload cycles (e.g. mod updates), where the
    // mod simply re-applies on the next init.
    RestoreOriginal();

    if (g_stopEvent) {
        SetEvent(g_stopEvent);
    }
    if (g_keepAliveThread) {
        WaitForSingleObject(g_keepAliveThread, INFINITE);
        CloseHandle(g_keepAliveThread);
        g_keepAliveThread = nullptr;
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