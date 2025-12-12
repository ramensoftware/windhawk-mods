// ==WindhawkMod==
// @id              xs-onedrive-no-msedgewebview2
// @name            OneDrive: No MS Edge WebView2
// @description     OneDrive may launch Edge WebView1 child processes, this prevents that.
// @version         1.0
// @author          Xetrill
// @github          https://github.com/Xetrill
// @include         onedrive.exe
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Description
Recent – at least as of version `25.222.1112.0002` – OneDrive may launch Edge WebView2 (`msedgewebview2.exe`) child processes.
Presumably this relates to newly added Copilot functionality.
I can't say for sure—Microsoft hasn't updated the [release notes](https://support.microsoft.com/en-us/office/onedrive-release-notes-845dcf18-f921-435e-bf28-4e24b95e5fc0) yet (as of 2025-12-12).

I didn't want that, so I made this mod. It blocks those processes and nothing more. Exactly how it should be.

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
    // Wh_Log(L"[CreateProcessW] Cmdline: '%s'", cmd);

    if (wcsstr(cmd, L"msedgewebview2.exe") != nullptr) {
        // Wh_Log(L"[CreateProcessW] blocking msedgewebview2.exe");
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

BOOL Wh_ModInit() {
    const auto hKernel32 = LoadLibraryW(L"Kernel32.dll");    
    const auto pCreateProcessW = (decltype(&CreateProcessW))GetProcAddress(hKernel32, "CreateProcessW");
    if (!WindhawkUtils::SetFunctionHook(pCreateProcessW, hk_CreateProcessW, &orig_CreateProcessW)) {
        Wh_Log(L"SetFunctionHook(CreateProcessW) failed (err=%u).", GetLastError());
        return FALSE;
    }
    return TRUE;
}
