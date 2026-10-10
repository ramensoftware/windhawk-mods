// ==WindhawkMod==
// @id              taskbar-auto-hide-delay
// @name            Taskbar auto-hide after a delay
// @description     Hides the auto-hide taskbar a configurable number of seconds after it becomes visible, unless the mouse pointer is over it
// @version         1.0
// @author          nikitor32
// @github          https://github.com/nikitor32
// @include         windhawk.exe
// @compilerOptions -lshell32
// ==/WindhawkMod==

// Source code is published under The GNU General Public License v3.0.
//
// For bug reports and feature requests, please open an issue here:
// https://github.com/ramensoftware/windhawk-mods/issues

// ==WindhawkModReadme==
/*
# Taskbar auto-hide after a delay

Windows 11 (24H2) hides the auto-hide taskbar only when the pointer leaves
the taskbar. If the taskbar becomes visible for another reason — for
example, when a new window or dialog appears — no "pointer left" event ever
fires, and the taskbar stays on screen indefinitely.

The mod runs as a small background tool and watches the taskbars. As soon as
a taskbar becomes visible while the mouse pointer is **not** over it, a
countdown starts. When the countdown expires, the mod sends the taskbar its
own hide message, so it slides away with the standard animation.

If you move the pointer over the taskbar, the countdown is cancelled so the
taskbar stays available while you interact with it.

The mod does nothing if auto-hide is disabled in the Windows settings.

## Settings

* **Hide delay (seconds)** — how long the taskbar stays visible before it
  is hidden. Default: 3.

## Compared to Better Taskbar Autohide

[Better Taskbar Autohide](https://windhawk.net/mods/taskbar-autohide-better)
covers a narrower case: it hooks the taskbar internals and only helps when
Windows forces the taskbar to stay shown because an inactive window is
notifying. This mod reacts to any reason the taskbar becomes visible —
new windows, dialogs, notifications — and uses no hooks; the only internal
detail it relies on is the taskbar's auto-hide timer ID, which has been
stable for a long time and is also used by other taskbar mods.

## Compatibility

The mod doesn't use hooks, so it's designed to work together with other
taskbar mods such as
[Taskbar auto-hide when maximized](https://windhawk.net/mods/taskbar-auto-hide-when-maximized)
and
[Taskbar auto-hide keyboard only](https://windhawk.net/mods/taskbar-auto-hide-keyboard-only).
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- hideDelaySeconds: 3
  $name: Hide delay (seconds)
  $description: How long the taskbar stays visible (with the pointer not over it) before it's hidden.
*/
// ==/WindhawkModSettings==

#include <shellapi.h>

#include <algorithm>
#include <atomic>
#include <unordered_map>
#include <vector>

// The delay can be updated from another thread while the polling thread
// reads it, so an atomic type is used to avoid a data race.
std::atomic<int> g_hideDelaySeconds{3};

// The taskbar hides itself when its internal "hide" timer (ID 2) fires.
// We post WM_TIMER with that ID instead of arming a timer: the taskbar
// handles the message once, on its own thread, and no timer is left behind.
constexpr UINT_PTR kTrayUITimerHide = 2;

// How often we check the taskbar state, in milliseconds.
constexpr DWORD kPollIntervalMs = 300;

HANDLE g_stopEvent = nullptr;
HANDLE g_threadHandle = nullptr;

// Per-taskbar countdown state. Owned by the polling thread.
struct TaskbarState {
    bool countdownActive = false;
    ULONGLONG countdownStart = 0;
};

using StateMap = std::unordered_map<HWND, TaskbarState>;

void LoadSettings() {
    int hideDelaySeconds = Wh_GetIntSetting(L"hideDelaySeconds");
    if (hideDelaySeconds < 0) {
        hideDelaySeconds = 0;
    }
    g_hideDelaySeconds.store(hideDelaySeconds);
}

// Only makes sense while the Windows auto-hide option is turned on.
bool IsAutoHideEnabled() {
    APPBARDATA abd = {};
    abd.cbSize = sizeof(abd);
    return (SHAppBarMessage(ABM_GETSTATE, &abd) & ABS_AUTOHIDE) != 0;
}

BOOL CALLBACK CollectSecondaryTaskbarProc(HWND hwnd, LPARAM lParam) {
    WCHAR className[32];
    if (GetClassName(hwnd, className, ARRAYSIZE(className)) &&
        _wcsicmp(className, L"Shell_SecondaryTrayWnd") == 0) {
        reinterpret_cast<std::vector<HWND>*>(lParam)->push_back(hwnd);
    }
    return TRUE;
}

void CollectTaskbars(std::vector<HWND>* out) {
    out->clear();

    HWND primary = FindWindow(L"Shell_TrayWnd", nullptr);
    if (primary) {
        out->push_back(primary);
    }

    // Taskbars on secondary monitors.
    EnumWindows(CollectSecondaryTaskbarProc, reinterpret_cast<LPARAM>(out));
}

// Returns true if enough of the taskbar is on screen to be considered
// visible. When auto-hidden, the taskbar is moved mostly off-screen, so only
// a sliver remains. The visible part is the intersection of the window rect
// with the monitor rect, which handles taskbars on any edge, including side
// taskbars placed by other mods.
bool IsTaskbarShown(HWND hwnd, RECT* rectOut) {
    // Skip windows that are hidden but may still be positioned on screen
    // (for example, hidden by another mod with ShowWindow).
    if (!IsWindowVisible(hwnd)) {
        return false;
    }

    RECT rc;
    if (!GetWindowRect(hwnd, &rc)) {
        return false;
    }

    HMONITOR monitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi = {};
    mi.cbSize = sizeof(mi);
    if (!GetMonitorInfo(monitor, &mi)) {
        return false;
    }

    RECT visible;
    if (!IntersectRect(&visible, &rc, &mi.rcMonitor)) {
        return false;
    }

    LONGLONG fullArea = (LONGLONG)(rc.right - rc.left) * (rc.bottom - rc.top);
    LONGLONG visibleArea = (LONGLONG)(visible.right - visible.left) *
                           (visible.bottom - visible.top);

    // At least half of the taskbar is on screen.
    bool shown = fullArea > 0 && visibleArea * 2 >= fullArea;
    if (shown) {
        *rectOut = rc;
    }
    return shown;
}

bool IsPointerOver(const RECT& rc) {
    POINT pt;
    if (!GetCursorPos(&pt)) {
        return false;
    }
    return PtInRect(&rc, pt) != 0;
}

void PollOnce(StateMap* states) {
    std::vector<HWND> taskbars;
    CollectTaskbars(&taskbars);

    // Forget state for taskbars that no longer exist.
    for (auto it = states->begin(); it != states->end();) {
        if (std::find(taskbars.begin(), taskbars.end(), it->first) ==
            taskbars.end()) {
            it = states->erase(it);
        } else {
            ++it;
        }
    }

    ULONGLONG now = GetTickCount64();
    int hideDelaySeconds = g_hideDelaySeconds.load();
    ULONGLONG delayMs = (ULONGLONG)hideDelaySeconds * 1000;

    for (HWND hwnd : taskbars) {
        RECT rc;
        bool shown = IsTaskbarShown(hwnd, &rc);

        TaskbarState& state = (*states)[hwnd];

        // Hidden -> reset the countdown.
        if (!shown) {
            state.countdownActive = false;
            state.countdownStart = 0;
            continue;
        }

        // Pointer over the taskbar -> the user wants to use it; cancel.
        if (IsPointerOver(rc)) {
            state.countdownActive = false;
            state.countdownStart = 0;
            continue;
        }

        // Visible and pointer is elsewhere -> start / continue the countdown.
        if (!state.countdownActive) {
            state.countdownActive = true;
            state.countdownStart = now;
            Wh_Log(L"Taskbar %08X visible, starting %d s countdown",
                   (DWORD)(ULONG_PTR)hwnd, hideDelaySeconds);
        } else if (now - state.countdownStart >= delayMs) {
            // Ask the taskbar to hide only when Windows auto-hide is on.
            // The state query happens once per countdown, not every poll.
            if (IsAutoHideEnabled()) {
                Wh_Log(L"Taskbar %08X: hiding after delay",
                       (DWORD)(ULONG_PTR)hwnd);
                PostMessage(hwnd, WM_TIMER, kTrayUITimerHide, 0);
            }
            state.countdownActive = false;
            state.countdownStart = 0;
        }
    }
}

DWORD WINAPI PollThread(LPVOID /*param*/) {
    // The state map is local: only this thread touches it.
    StateMap states;

    // Waiting on the stop event doubles as the polling interval, so the
    // thread exits promptly once the mod is unloaded.
    while (WaitForSingleObject(g_stopEvent, kPollIntervalMs) ==
           WAIT_TIMEOUT) {
        PollOnce(&states);
    }
    return 0;
}

BOOL WhTool_ModInit() {
    Wh_Log(L">");
    LoadSettings();

    g_stopEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
    if (!g_stopEvent) {
        Wh_Log(L"CreateEvent failed");
        return FALSE;
    }

    g_threadHandle = CreateThread(nullptr, 0, PollThread, nullptr, 0, nullptr);
    if (!g_threadHandle) {
        Wh_Log(L"CreateThread failed");
        CloseHandle(g_stopEvent);
        g_stopEvent = nullptr;
        return FALSE;
    }
    return TRUE;
}

void WhTool_ModSettingsChanged() {
    Wh_Log(L">");
    LoadSettings();
}

void WhTool_ModUninit() {
    Wh_Log(L">");
    // Wait for the polling thread without a timeout: it can block inside
    // SHAppBarMessage while the taskbar thread is busy, and unloading the
    // mod while the thread is still running would crash the process.
    if (g_stopEvent) {
        SetEvent(g_stopEvent);
    }
    if (g_threadHandle) {
        WaitForSingleObject(g_threadHandle, INFINITE);
        CloseHandle(g_threadHandle);
        g_threadHandle = nullptr;
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
