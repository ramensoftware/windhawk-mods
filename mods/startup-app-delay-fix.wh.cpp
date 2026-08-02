// ==WindhawkMod==
// @id              startup-app-delay-fix
// @name            Startup App Delay Fix
// @description     Restores WaitforIdleState=0 whenever Windhawk starts
// @version         1.0
// @author          meteoni
// @github          https://github.com/Meteony
// @include         windhawk.exe
// @compilerOptions -ladvapi32 -lshell32
// ==/WindhawkMod==
// ==WindhawkModReadme==
/*
Windows intentionally delays startup applications after sign-in to reduce system load, which can cause many apps to open minutes after system startup. 

![Key](https://raw.githubusercontent.com/Meteony/meteoni-assets/main/startup-app-delay-fix/startup-app-delay-fix.png)

This can be disabled by setting the following registry value:

`HKEY_CURRENT_USER\Software\Microsoft\Windows\CurrentVersion\Explorer\Serialize
WaitforIdleState    REG_DWORD    0`

However, some Windows updates reset this value. This mod automates 
the process of checking and setting that key as a more persistent fix.   
*/
// ==/WindhawkModReadme==

#include <windows.h>
#include <shellapi.h>
#include <strsafe.h>

// Registry repair logic

constexpr wchar_t kRegistryPath[] =
    L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Serialize";

constexpr wchar_t kValueName[] = L"WaitforIdleState";
constexpr DWORD kDesiredValue = 0;

bool EnsureStartupDelayFix() {
    HKEY key = nullptr;
    DWORD disposition = 0;

    LSTATUS status = RegCreateKeyExW(
        HKEY_CURRENT_USER,
        kRegistryPath,
        0,
        nullptr,
        REG_OPTION_NON_VOLATILE,
        KEY_QUERY_VALUE | KEY_SET_VALUE | KEY_WOW64_64KEY,
        nullptr,
        &key,
        &disposition
    );

    if (status != ERROR_SUCCESS) {
        Wh_Log(
            L"Failed to open or create HKCU\\%s: error %ld",
            kRegistryPath,
            status
        );
        return false;
    }

    DWORD existingValue = 0;
    DWORD existingType = 0;
    DWORD existingSize = sizeof(existingValue);

    status = RegQueryValueExW(
        key,
        kValueName,
        nullptr,
        &existingType,
        reinterpret_cast<BYTE*>(&existingValue),
        &existingSize
    );

    if (status == ERROR_SUCCESS &&
        existingType == REG_DWORD &&
        existingSize == sizeof(DWORD) &&
        existingValue == kDesiredValue) {
        Wh_Log(
            L"%s is already correctly set to %lu",
            kValueName,
            kDesiredValue
        );

        RegCloseKey(key);
        return true;
    }

    bool valueWasMissing = status == ERROR_FILE_NOT_FOUND;

    status = RegSetValueExW(
        key,
        kValueName,
        0,
        REG_DWORD,
        reinterpret_cast<const BYTE*>(&kDesiredValue),
        sizeof(kDesiredValue)
    );

    RegCloseKey(key);

    if (status != ERROR_SUCCESS) {
        Wh_Log(
            L"Failed to set %s: error %ld",
            kValueName,
            status
        );
        return false;
    }

    if (disposition == REG_CREATED_NEW_KEY) {
        Wh_Log(
            L"Created Serialize registry key and set %s to %lu",
            kValueName,
            kDesiredValue
        );
    } else if (valueWasMissing) {
        Wh_Log(
            L"Created missing %s value and set it to %lu",
            kValueName,
            kDesiredValue
        );
    } else {
        Wh_Log(
            L"Repaired %s and set it to %lu",
            kValueName,
            kDesiredValue
        );
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////
// One-shot tool callbacks

BOOL WhTool_ModInit() {
    const bool success = EnsureStartupDelayFix();

    // This is a one-shot tool. The dedicated process is no longer needed after
    // the registry value has been checked and repaired.
    ExitProcess(success ? 0 : 1);

    return FALSE;
}

void WhTool_ModSettingsChanged() {
}

void WhTool_ModUninit() {
}

/*
Windhawk tool-mod implementation

https://github.com/ramensoftware/windhawk/wiki/Mods-as-tools:-Running-mods-in-a-dedicated-process

This launches the mod in a separate windhawk.exe process.
*/
bool g_isToolModProcessLauncher = false;
HANDLE g_toolModProcessMutex = nullptr;

void WINAPI EntryPoint_Hook() {
    ExitThread(0);
}

BOOL Wh_ModInit() {
    DWORD sessionId = 0;

    if (ProcessIdToSessionId(GetCurrentProcessId(), &sessionId) &&
        sessionId == 0) {
        return FALSE;
    }

    bool isExcluded = false;
    bool isToolModProcess = false;
    bool isCurrentToolModProcess = false;

    int argc = 0;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);

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
            CreateMutexW(nullptr, TRUE, L"windhawk-tool-mod_" WH_MOD_ID);

        if (!g_toolModProcessMutex) {
            Wh_Log(L"CreateMutex failed: error %lu", GetLastError());
            ExitProcess(1);
        }

        if (GetLastError() == ERROR_ALREADY_EXISTS) {
            Wh_Log(L"Tool mod is already running: %s", WH_MOD_ID);
            ExitProcess(1);
        }

        if (!WhTool_ModInit()) {
            ExitProcess(1);
        }

        IMAGE_DOS_HEADER* dosHeader =
            reinterpret_cast<IMAGE_DOS_HEADER*>(GetModuleHandleW(nullptr));

        IMAGE_NT_HEADERS* ntHeaders =
            reinterpret_cast<IMAGE_NT_HEADERS*>(
                reinterpret_cast<BYTE*>(dosHeader) + dosHeader->e_lfanew
            );

        DWORD entryPointRVA =
            ntHeaders->OptionalHeader.AddressOfEntryPoint;

        void* entryPoint =
            reinterpret_cast<BYTE*>(dosHeader) + entryPointRVA;

        Wh_SetFunctionHook(
            entryPoint,
            reinterpret_cast<void*>(EntryPoint_Hook),
            nullptr
        );

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

    wchar_t currentProcessPath[MAX_PATH];

    DWORD pathLength = GetModuleFileNameW(
        nullptr,
        currentProcessPath,
        ARRAYSIZE(currentProcessPath)
    );

    if (pathLength == 0 || pathLength == ARRAYSIZE(currentProcessPath)) {
        Wh_Log(L"GetModuleFileNameW failed: error %lu", GetLastError());
        return;
    }

    wchar_t commandLine[MAX_PATH + 2 +
                        (sizeof(L" -tool-mod \"" WH_MOD_ID "\"") /
                         sizeof(wchar_t))];

    HRESULT formatResult = StringCchPrintfW(commandLine, ARRAYSIZE(commandLine),
                                            L"\"%s\" -tool-mod \"%s\"",
                                            currentProcessPath, WH_MOD_ID);

    if (FAILED(formatResult)) {
        Wh_Log(L"Failed to construct tool-mod command line: HRESULT 0x%08X",
               static_cast<unsigned int>(formatResult));
        return;
    }

    HMODULE kernelModule = GetModuleHandleW(L"kernelbase.dll");

    if (!kernelModule) {
        kernelModule = GetModuleHandleW(L"kernel32.dll");

        if (!kernelModule) {
            Wh_Log(L"Could not find kernelbase.dll or kernel32.dll");
            return;
        }
    }

    using CreateProcessInternalW_t = BOOL(WINAPI*)(
        HANDLE hUserToken,
        LPCWSTR lpApplicationName,
        LPWSTR lpCommandLine,
        LPSECURITY_ATTRIBUTES lpProcessAttributes,
        LPSECURITY_ATTRIBUTES lpThreadAttributes,
        BOOL bInheritHandles,
        DWORD dwCreationFlags,
        LPVOID lpEnvironment,
        LPCWSTR lpCurrentDirectory,
        LPSTARTUPINFOW lpStartupInfo,
        LPPROCESS_INFORMATION lpProcessInformation,
        PHANDLE hRestrictedUserToken
    );

    auto createProcessInternalW =
        reinterpret_cast<CreateProcessInternalW_t>(
            GetProcAddress(kernelModule, "CreateProcessInternalW")
        );

    if (!createProcessInternalW) {
        Wh_Log(L"CreateProcessInternalW was not found");
        return;
    }

    STARTUPINFOW startupInfo{};
    startupInfo.cb = sizeof(startupInfo);
    startupInfo.dwFlags = STARTF_FORCEOFFFEEDBACK;

    PROCESS_INFORMATION processInformation{};

    if (!createProcessInternalW(
            nullptr,
            currentProcessPath,
            commandLine,
            nullptr,
            nullptr,
            FALSE,
            NORMAL_PRIORITY_CLASS,
            nullptr,
            nullptr,
            &startupInfo,
            &processInformation,
            nullptr)) {
        Wh_Log(L"CreateProcessInternalW failed: error %lu", GetLastError());
        return;
    }

    CloseHandle(processInformation.hProcess);
    CloseHandle(processInformation.hThread);
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
