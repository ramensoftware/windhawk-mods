// ==WindhawkMod==
// @id              win10-taskbar-clock-seconds
// @name            Taskbar Clock with Seconds (Windows 10)
// @description     Enables seconds in the taskbar clock.
// @version         1.0
// @author          Half
// @github       https://github.com/halfofknowledge-cpu
// @include         explorer.exe
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
This will make it so your taskbar clock will display hours, minutes, seconds.
*/
// ==/WindhawkModReadme==

#include <windows.h>

static DWORD g_oldValue = 0;
static bool g_hadOldValue = false;

static bool ShouldRestart(DWORD newValue) {
    HKEY hKey;
    DWORD current = 0;
    DWORD size = sizeof(DWORD);

    if (RegOpenKeyExW(
        HKEY_CURRENT_USER,
        L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced",
        0,
        KEY_READ,
        &hKey
    ) != ERROR_SUCCESS) {
        return false;
    }

    bool changed = true;

    if (RegQueryValueExW(
        hKey,
        L"ShowSecondsInSystemClock",
        nullptr,
        nullptr,
        (LPBYTE)&current,
        &size
    ) == ERROR_SUCCESS) {
        changed = (current != newValue);
    }

    RegCloseKey(hKey);
    return changed;
}

static void RestartExplorerOnce() {
    static bool restarted = false;
    if (restarted) return;
    restarted = true;

    STARTUPINFOW si{};
    PROCESS_INFORMATION pi{};
    si.cb = sizeof(si);

    CreateProcessW(
        nullptr,
        (LPWSTR)L"cmd.exe /c taskkill /f /im explorer.exe",
        nullptr,
        nullptr,
        FALSE,
        CREATE_NO_WINDOW,
        nullptr,
        nullptr,
        &si,
        &pi
    );

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}

BOOL Wh_ModInit() {
    if (!ShouldRestart(1))
        return TRUE;

    HKEY hKey;
    DWORD value = 1;
    DWORD size = sizeof(DWORD);

    if (RegOpenKeyExW(
        HKEY_CURRENT_USER,
        L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced",
        0,
        KEY_READ | KEY_WRITE,
        &hKey
    ) != ERROR_SUCCESS) {
        return FALSE;
    }

    if (RegQueryValueExW(
        hKey,
        L"ShowSecondsInSystemClock",
        nullptr,
        nullptr,
        (LPBYTE)&g_oldValue,
        &size
    ) == ERROR_SUCCESS) {
        g_hadOldValue = true;
    }

    RegSetValueExW(
        hKey,
        L"ShowSecondsInSystemClock",
        0,
        REG_DWORD,
        (const BYTE*)&value,
        sizeof(DWORD)
    );

    RegCloseKey(hKey);

    RestartExplorerOnce();
    return TRUE;
}

void Wh_ModUninit() {
    if (!ShouldRestart(g_hadOldValue ? g_oldValue : 0))
        return;

    HKEY hKey;

    if (RegOpenKeyExW(
        HKEY_CURRENT_USER,
        L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced",
        0,
        KEY_WRITE,
        &hKey
    ) != ERROR_SUCCESS) {
        return;
    }

    if (g_hadOldValue) {
        RegSetValueExW(
            hKey,
            L"ShowSecondsInSystemClock",
            0,
            REG_DWORD,
            (const BYTE*)&g_oldValue,
            sizeof(DWORD)
        );
    } else {
        RegDeleteValueW(hKey, L"ShowSecondsInSystemClock");
    }

    RegCloseKey(hKey);

    RestartExplorerOnce();
}
