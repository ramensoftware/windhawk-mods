// ==WindhawkMod==
// @id              xs-nomsedgewebview4copilot
// @name            No MS Edge WebView2 for Copilot
// @description     Disables MSEdgeWebView2 child process creation for certain application, which is used for Copilot functionality.
// @version         1.0
// @author          Xetrill
// @github          https://github.com/Xetrill
// @include         onedrive.exe
// @include         XboxGameBarWidgets.exe
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Description
All this mod does is disables certain processes from creating `msedgewebview2.exe` child processes. Which are used to provide Copilot functionality within the app.

# Hooked Processes
## OneDrive (`onedrive.exe`)
As of version `25.222.1112.0002`, OneDrive may launch `msedgewebview2.exe` child processes. This appears related to the Copilot Actions feature. You may see this regardless of whether you have an M365-enabled Copilot license.

## Gamebar (`XboxGameBarWidgets.exe`)
After updating to 25H2 build 7462 I observed `msedgewebview2.exe` instances started by `XboxGameBarWidgets.exe` (the Windows Game Bar). This is again for Copilot integration.

# Disclaimer
I haven't tested every scenario. I only verified that OneDrive (personal and business, running together) still starts and sync works.
*/
// ==/WindhawkModReadme==

#include <windhawk_utils.h>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <wchar.h>
#include <tlhelp32.h>

static WINBOOL (WINAPI *orig_CreateProcessW)(
    LPCWSTR,
    LPWSTR,
    LPSECURITY_ATTRIBUTES,
    LPSECURITY_ATTRIBUTES,
    WINBOOL,
    DWORD,
    LPVOID,
    LPCWSTR,
    LPSTARTUPINFOW,
    LPPROCESS_INFORMATION
) = nullptr;

static const wchar_t* wcsistr(const wchar_t* haystack, const wchar_t* needle)
{
    if (!haystack || !needle || !*needle)
        return nullptr;

    const size_t needle_len = wcslen(needle);

    for (const wchar_t* p = haystack; *p; ++p) {
        if (_wcsnicmp(p, needle, needle_len) == 0) {
            return p;
        }
    }
    return nullptr;
}

static WINBOOL WINAPI hk_CreateProcessW(
    LPCWSTR lpApplicationName,
    LPWSTR lpCommandLine,
    LPSECURITY_ATTRIBUTES lpProcessAttributes,
    LPSECURITY_ATTRIBUTES lpThreadAttributes,
    WINBOOL bInheritHandles,
    DWORD dwCreationFlags,
    LPVOID lpEnvironment,
    LPCWSTR lpCurrentDirectory,
    LPSTARTUPINFOW lpStartupInfo,
    LPPROCESS_INFORMATION lpProcessInformation
)
{
    const wchar_t *cmd = lpCommandLine ? lpCommandLine : L"";
    Wh_Log(L"[CreateProcessW] Cmdline: '%s'", cmd);

    if (wcsistr(cmd, L"msedgewebview2.exe") != nullptr) {
        Wh_Log(L"[CreateProcessW] blocking msedgewebview2.exe");
        SetLastError(ERROR_ACCESS_DENIED);
        return FALSE;
    }

    return orig_CreateProcessW(
        lpApplicationName,
        lpCommandLine,
        lpProcessAttributes,
        lpThreadAttributes,
        bInheritHandles,
        dwCreationFlags,
        lpEnvironment,
        lpCurrentDirectory,
        lpStartupInfo,
        lpProcessInformation
    );
}

static void TerminateRunningChildren()
{
    const auto myPid = GetCurrentProcessId();
    auto snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot == INVALID_HANDLE_VALUE)
        return;

    // We only need to find the main broker process, its children will terminate automatically
    auto foundChild = false;
    PROCESSENTRY32W pe{ sizeof(pe) };
    for (BOOL ok = Process32FirstW(snapshot, &pe); ok; ok = Process32NextW(snapshot, &pe)) {
        if (pe.th32ParentProcessID != myPid) {
            continue;
        }
        if (_wcsicmp(pe.szExeFile, L"msedgewebview2.exe") == 0) {
            foundChild = true;
            break;
        }
    }
    CloseHandle(snapshot);

    if (!foundChild) {
        return;
    }

    auto proc = OpenProcess(
        PROCESS_TERMINATE | PROCESS_QUERY_LIMITED_INFORMATION,
        FALSE,
        pe.th32ProcessID
    );
    if (proc) {
        Wh_Log(L"[TerminateRunningChildren] Killing pid=%u", pe.th32ProcessID);
        TerminateProcess(proc, 1);
        CloseHandle(proc);
    } else {
        Wh_Log(L"[TerminateRunningChildren] Found pid=%u but OpenProcess failed (err=%u)", pe.th32ProcessID, GetLastError());
    }
}

BOOL Wh_ModInit() {
    const auto hKernel32 = LoadLibraryW(L"Kernel32.dll");    
    const auto pCreateProcessW = (decltype(&CreateProcessW))GetProcAddress(hKernel32, "CreateProcessW");
    if (!WindhawkUtils::SetFunctionHook(pCreateProcessW, hk_CreateProcessW, &orig_CreateProcessW)) {
        Wh_Log(L"SetFunctionHook(CreateProcessW) failed (err=%u).", GetLastError());
        return FALSE;
    }

    TerminateRunningChildren();

    return TRUE;
}
