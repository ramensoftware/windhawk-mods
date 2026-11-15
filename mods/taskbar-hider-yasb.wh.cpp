// ==WindhawkMod==
// @id              taskbar-hider-yasb
// @name            Taskbar hider for YASB
// @description     Keeps the Windows taskbar hidden without touching the work area, so YASB keeps its space
// @version         1.2
// @author          Fritz Reigar
// @github          https://github.com/FritzReigar
// @twitter         https://twitter.com/FritzReigar
// @include         explorer.exe
// @architecture    x86-64
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Taskbar hider for YASB

Hides the Windows taskbar (the main one and the ones on other monitors) and
keeps it hidden, for setups that use [YASB](https://github.com/amnweb/yasb)
as their bar.

- The work area is not changed, so the space YASB reserves for itself is
  respected.
- The Start menu still opens with the Windows key or YASB's start button.
- If Explorer restarts while YASB is running, the mod runs `yasbc reload` so
  YASB takes its space back instead of ending up under other windows.

## Requirements

- Taskbar set to **Automatically hide the taskbar** in Windows settings.
- YASB installed. The mod looks for `yasbc.exe` in the Scoop install folder
  first and then in `PATH`. Hiding the taskbar works without it; only the
  reload after an Explorer restart needs it.

## How it works

A background thread checks every 250 ms whether the taskbar is visible again
and hides it. Polling keeps the mod simple and independent of taskbar
internals that change between Windows builds; the check is a single window
enumeration.

Disabling the mod shows the taskbar again.

## Related mod

[Taskbar auto-hide fine tuning](https://windhawk.net/mods/taskbar-auto-hide-keyboard-only)
has a "Never" mode that also keeps the taskbar hidden, with many more
options. This mod is a small alternative focused on YASB: it adds the
automatic YASB reload after Explorer restarts.
*/
// ==/WindhawkModReadme==

#include <windows.h>
#include <tlhelp32.h>

static HANDLE g_thread = nullptr;
static HANDLE g_stopEvent = nullptr;

// Folder windows can run in separate explorer.exe processes; only act in the
// one that owns the desktop and the taskbar.
static bool IsShellProcess() {
    LPCWSTR cmd = GetCommandLineW();
    return !wcsstr(cmd, L"/factory") && !wcsstr(cmd, L"-Embedding");
}

static bool IsProcessRunning(LPCWSTR exeName) {
    HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snap == INVALID_HANDLE_VALUE) {
        return false;
    }
    PROCESSENTRY32W entry = {sizeof(entry)};
    bool found = false;
    for (BOOL ok = Process32FirstW(snap, &entry); ok; ok = Process32NextW(snap, &entry)) {
        if (_wcsicmp(entry.szExeFile, exeName) == 0) {
            found = true;
            break;
        }
    }
    CloseHandle(snap);
    return found;
}

// Scoop folder first, then PATH.
static bool FindYasbc(WCHAR* path, DWORD size) {
    if (ExpandEnvironmentStringsW(L"%USERPROFILE%\\scoop\\apps\\yasb\\current\\yasbc.exe",
                                  path, size) &&
        GetFileAttributesW(path) != INVALID_FILE_ATTRIBUTES) {
        return true;
    }
    return SearchPathW(nullptr, L"yasbc.exe", nullptr, size, path, nullptr) != 0;
}

// Runs "yasbc reload" without a console window.
static void ReloadYasb() {
    WCHAR path[MAX_PATH];
    if (!FindYasbc(path, ARRAYSIZE(path))) {
        Wh_Log(L"yasbc.exe not found, skipping reload");
        return;
    }
    WCHAR cmdLine[MAX_PATH + 16];
    wsprintfW(cmdLine, L"\"%s\" reload", path);
    STARTUPINFOW si = {sizeof(si)};
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;
    PROCESS_INFORMATION pi = {};
    if (CreateProcessW(path, cmdLine, nullptr, nullptr, FALSE, CREATE_NO_WINDOW,
                       nullptr, nullptr, &si, &pi)) {
        CloseHandle(pi.hThread);
        CloseHandle(pi.hProcess);
        Wh_Log(L"YASB reloaded");
    }
}

// Hides (or shows again) the Windows taskbars.
static BOOL CALLBACK ApplyToTrayWindow(HWND hwnd, LPARAM show) {
    WCHAR className[64];
    if (!GetClassNameW(hwnd, className, ARRAYSIZE(className))) {
        return TRUE;
    }
    if (wcscmp(className, L"Shell_TrayWnd") != 0 &&
        wcscmp(className, L"Shell_SecondaryTrayWnd") != 0) {
        return TRUE;
    }
    if (show) {
        ShowWindow(hwnd, SW_SHOWNA);
    } else if (IsWindowVisible(hwnd)) {
        ShowWindow(hwnd, SW_HIDE);
    }
    return TRUE;
}

// Watcher thread: if the taskbar shows up again, hide it.
static DWORD WINAPI WatchThread(LPVOID) {
    // YASB already running when Explorer starts means Explorer restarted:
    // wait for the taskbar to exist, then reload YASB once.
    bool yasbWasRunning = IsProcessRunning(L"yasb.exe");
    DWORD elapsed = 0;
    bool reloaded = false;
    do {
        EnumWindows(ApplyToTrayWindow, FALSE);
        if (yasbWasRunning && !reloaded && elapsed >= 4000 &&
            FindWindowW(L"Shell_TrayWnd", nullptr)) {
            ReloadYasb();
            reloaded = true;
        }
        elapsed += 250;
    } while (WaitForSingleObject(g_stopEvent, 250) == WAIT_TIMEOUT);
    return 0;
}

BOOL Wh_ModInit() {
    if (!IsShellProcess()) {
        return TRUE;
    }
    Wh_Log(L"Init");
    g_stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_stopEvent) {
        return FALSE;
    }
    g_thread = CreateThread(nullptr, 0, WatchThread, nullptr, 0, nullptr);
    if (!g_thread) {
        CloseHandle(g_stopEvent);
        g_stopEvent = nullptr;
        return FALSE;
    }
    return TRUE;
}

void Wh_ModUninit() {
    if (!g_thread) {
        return;
    }
    Wh_Log(L"Uninit");
    SetEvent(g_stopEvent);
    WaitForSingleObject(g_thread, INFINITE);
    CloseHandle(g_thread);
    g_thread = nullptr;
    CloseHandle(g_stopEvent);
    g_stopEvent = nullptr;
    EnumWindows(ApplyToTrayWindow, TRUE);
}
