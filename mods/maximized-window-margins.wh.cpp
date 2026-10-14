// ==WindhawkMod==
// @id              maximized-window-margins
// @name            Maximized Window Margins
// @description     Leaves a configurable gap around maximized windows, macOS style, by shrinking the desktop work area
// @version         1.0
// @author          Mário Barbosa
// @github          https://github.com/mariogomesbarbosa
// @include         windhawk.exe
// @compilerOptions -lshcore
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Maximized Window Margins

Maximized windows no longer touch the screen edges. A configurable gap is kept
on every side, which pairs nicely with a floating dock (for example the
*Luminosity* or *DockLike* themes of **Windows 11 Taskbar Styler**).

## How it works

The mod runs as a *tool mod* (in its own `windhawk.exe` process, nothing is
injected into other programs). For each monitor and each edge with a margin
greater than zero, it registers an invisible
[AppBar](https://learn.microsoft.com/windows/win32/shell/application-desktop-toolbars)
of the configured thickness. Windows then reduces the work area, and every
maximized window fits inside it.

* Margins are set in DIPs and scaled per monitor DPI.
* AppBars are registered again when Explorer restarts, when displays change,
  and when the taskbar or another AppBar moves.
* Disabling the mod restores the original work area immediately.

## Side effects

Since the work area itself shrinks, the gap also applies to:

* Snap / Snap Layouts (snapped windows keep the margin too).
* Desktop icon placement.
* Anything else that positions itself from the work area, such as pop-ups and
  notifications that apps place in a corner of the screen.

Real fullscreen apps (games, videos, F11) cover the whole monitor and are not
affected.

## Tips

* With a floating dock, the taskbar still reserves its own height at the
  bottom. The bottom margin is added on top of it, so a small value (0–10)
  is usually enough.
* Maximized windows have square corners in Windows 11. To round them, install
  *Custom Window Corner Radius* and set "Rounded corners for maximized and
  snapped windows" to "Round snapped and maximized windows". The "Corner
  radius" setting of that mod then applies to maximized windows too.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- top: 10
  $name: Top margin
  $description: Gap in pixels (at 100% scaling) between the top edge of the screen and maximized windows. 0 to 200.
- bottom: 10
  $name: Bottom margin
  $description: Gap in pixels (at 100% scaling) between the taskbar (or the bottom edge of the screen) and maximized windows. 0 to 200.
- left: 10
  $name: Left margin
  $description: Gap in pixels (at 100% scaling) between the left edge of the screen and maximized windows. 0 to 200.
- right: 10
  $name: Right margin
  $description: Gap in pixels (at 100% scaling) between the right edge of the screen and maximized windows. 0 to 200.
- monitors: all
  $name: Monitors
  $options:
  - all: All monitors
  - primary: Primary monitor only
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <windows.h>

#include <shellapi.h>
#include <shellscalingapi.h>

#include <algorithm>
#include <vector>

////////////////////////////////////////////////////////////////////////////////
// Settings

struct Settings {
    int margin[4];  // Indexed by ABE_LEFT, ABE_TOP, ABE_RIGHT, ABE_BOTTOM.
    bool primaryOnly;
};

Settings g_settings;

constexpr int kMaxMargin = 200;

int ClampMargin(int value) {
    if (value < 0) {
        return 0;
    }
    if (value > kMaxMargin) {
        return kMaxMargin;
    }
    return value;
}

void LoadSettings() {
    g_settings.margin[ABE_LEFT] = ClampMargin(Wh_GetIntSetting(L"left"));
    g_settings.margin[ABE_TOP] = ClampMargin(Wh_GetIntSetting(L"top"));
    g_settings.margin[ABE_RIGHT] = ClampMargin(Wh_GetIntSetting(L"right"));
    g_settings.margin[ABE_BOTTOM] = ClampMargin(Wh_GetIntSetting(L"bottom"));

    g_settings.primaryOnly =
        wcscmp(WindhawkUtils::StringSetting::make(L"monitors"), L"primary") ==
        0;
}

////////////////////////////////////////////////////////////////////////////////
// AppBar management. Everything below runs on the worker thread only.

constexpr WCHAR kWindowClass[] = L"WindhawkMaximizedWindowMargins_" WH_MOD_ID;

// Callback message for AppBar notifications (ABN_*).
constexpr UINT kAppBarCallbackMsg = WM_APP + 1;
// Posted by the mod callbacks to request a rebuild with fresh settings.
constexpr UINT kReloadSettingsMsg = WM_APP + 2;

constexpr UINT_PTR kRebuildTimerId = 1;
constexpr UINT_PTR kSyncTimerId = 2;
constexpr UINT kDebounceMs = 300;

struct AppBar {
    HWND hwnd;
    HMONITOR monitor;
    UINT edge;
    int thickness;  // Physical pixels.
    RECT rect;      // Last rect passed to ABM_SETPOS.
};

DWORD g_threadId;
HANDLE g_thread;
HWND g_controlWnd;
UINT g_taskbarCreatedMsg;
std::vector<AppBar> g_appBars;
bool g_positioning;

int ScaleForMonitor(int dips, HMONITOR monitor) {
    UINT dpiX = 96, dpiY = 96;
    if (FAILED(GetDpiForMonitor(monitor, MDT_EFFECTIVE_DPI, &dpiX, &dpiY))) {
        dpiX = 96;
    }
    return MulDiv(dips, dpiX, 96);
}

// Computes the AppBar rect for its edge, letting the shell move it out of the
// way of the taskbar and other AppBars. Returns false on failure.
bool QueryAppBarRect(const AppBar& bar, RECT* result) {
    MONITORINFO mi{.cbSize = sizeof(mi)};
    if (!GetMonitorInfo(bar.monitor, &mi)) {
        return false;
    }

    APPBARDATA abd{
        .cbSize = sizeof(abd),
        .hWnd = bar.hwnd,
        .uEdge = bar.edge,
        .rc = mi.rcMonitor,
    };

    // Proposed rect: a strip along the edge of the full monitor rect.
    switch (bar.edge) {
        case ABE_LEFT:
            abd.rc.right = abd.rc.left + bar.thickness;
            break;
        case ABE_TOP:
            abd.rc.bottom = abd.rc.top + bar.thickness;
            break;
        case ABE_RIGHT:
            abd.rc.left = abd.rc.right - bar.thickness;
            break;
        case ABE_BOTTOM:
            abd.rc.top = abd.rc.bottom - bar.thickness;
            break;
    }

    SHAppBarMessage(ABM_QUERYPOS, &abd);

    // The shell moves the edge that touches the screen border (e.g. above the
    // taskbar). Recompute the opposite side so the thickness is preserved.
    switch (bar.edge) {
        case ABE_LEFT:
            abd.rc.right = abd.rc.left + bar.thickness;
            break;
        case ABE_TOP:
            abd.rc.bottom = abd.rc.top + bar.thickness;
            break;
        case ABE_RIGHT:
            abd.rc.left = abd.rc.right - bar.thickness;
            break;
        case ABE_BOTTOM:
            abd.rc.top = abd.rc.bottom - bar.thickness;
            break;
    }

    *result = abd.rc;
    return true;
}

void PositionAppBar(AppBar& bar) {
    RECT rc;
    if (!QueryAppBarRect(bar, &rc)) {
        return;
    }

    // Avoid ABM_SETPOS when nothing changed: each SETPOS notifies every other
    // AppBar, so unconditional updates would ping-pong forever.
    if (EqualRect(&rc, &bar.rect)) {
        return;
    }

    APPBARDATA abd{
        .cbSize = sizeof(abd),
        .hWnd = bar.hwnd,
        .uEdge = bar.edge,
        .rc = rc,
    };
    SHAppBarMessage(ABM_SETPOS, &abd);

    bar.rect = abd.rc;
    SetWindowPos(bar.hwnd, nullptr, abd.rc.left, abd.rc.top,
                 abd.rc.right - abd.rc.left, abd.rc.bottom - abd.rc.top,
                 SWP_NOZORDER | SWP_NOACTIVATE);

    Wh_Log(L"AppBar edge=%u rect=(%d,%d,%d,%d)", bar.edge, abd.rc.left,
           abd.rc.top, abd.rc.right, abd.rc.bottom);
}

void PositionAllAppBars() {
    g_positioning = true;
    for (auto& bar : g_appBars) {
        PositionAppBar(bar);
    }
    g_positioning = false;
}

void DestroyAppBars() {
    for (auto& bar : g_appBars) {
        APPBARDATA abd{.cbSize = sizeof(abd), .hWnd = bar.hwnd};
        SHAppBarMessage(ABM_REMOVE, &abd);
        DestroyWindow(bar.hwnd);
    }
    g_appBars.clear();
}

HWND CreateHiddenWindow() {
    // Never shown. WS_EX_TOOLWINDOW keeps it out of Alt+Tab and the taskbar.
    return CreateWindowEx(WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE, kWindowClass,
                          nullptr, WS_POPUP, 0, 0, 0, 0, nullptr, nullptr,
                          GetModuleHandle(nullptr), nullptr);
}

struct DesiredAppBar {
    HMONITOR monitor;
    UINT edge;
    int thickness;
};

BOOL CALLBACK EnumMonitorsProc(HMONITOR monitor, HDC, LPRECT, LPARAM param) {
    auto& desired = *reinterpret_cast<std::vector<DesiredAppBar>*>(param);

    if (g_settings.primaryOnly) {
        MONITORINFO mi{.cbSize = sizeof(mi)};
        if (!GetMonitorInfo(monitor, &mi) ||
            !(mi.dwFlags & MONITORINFOF_PRIMARY)) {
            return TRUE;
        }
    }

    for (UINT edge = ABE_LEFT; edge <= ABE_BOTTOM; edge++) {
        if (g_settings.margin[edge] == 0) {
            continue;
        }

        desired.push_back({
            .monitor = monitor,
            .edge = edge,
            .thickness = ScaleForMonitor(g_settings.margin[edge], monitor),
        });
    }

    return TRUE;
}

void CreateAppBar(const DesiredAppBar& desired) {
    HWND hwnd = CreateHiddenWindow();
    if (!hwnd) {
        Wh_Log(L"CreateWindowEx failed: %u", GetLastError());
        return;
    }

    APPBARDATA abd{
        .cbSize = sizeof(abd),
        .hWnd = hwnd,
        .uCallbackMessage = kAppBarCallbackMsg,
    };
    if (!SHAppBarMessage(ABM_NEW, &abd)) {
        Wh_Log(L"ABM_NEW failed for edge %u", desired.edge);
        DestroyWindow(hwnd);
        return;
    }

    g_appBars.push_back({
        .hwnd = hwnd,
        .monitor = desired.monitor,
        .edge = desired.edge,
        .thickness = desired.thickness,
        .rect = {},
    });
}

// Brings the AppBars in line with the settings and the connected monitors.
// Only the bars that differ are touched, so the work area changes once per
// affected edge and maximized windows don't jump to full screen and back.
void SyncAppBars() {
    std::vector<DesiredAppBar> desired;
    EnumDisplayMonitors(nullptr, nullptr, EnumMonitorsProc,
                        reinterpret_cast<LPARAM>(&desired));

    g_positioning = true;

    std::erase_if(g_appBars, [&](const AppBar& bar) {
        bool wanted = std::ranges::any_of(desired, [&](const auto& d) {
            return d.monitor == bar.monitor && d.edge == bar.edge;
        });
        if (!wanted) {
            APPBARDATA abd{.cbSize = sizeof(abd), .hWnd = bar.hwnd};
            SHAppBarMessage(ABM_REMOVE, &abd);
            DestroyWindow(bar.hwnd);
        }
        return !wanted;
    });

    for (const auto& d : desired) {
        auto it = std::ranges::find_if(g_appBars, [&](const AppBar& bar) {
            return bar.monitor == d.monitor && bar.edge == d.edge;
        });
        if (it != g_appBars.end()) {
            it->thickness = d.thickness;
        } else {
            CreateAppBar(d);
        }
    }

    // ABM_SETPOS goes only to new bars and bars whose rect changed.
    PositionAllAppBars();
}

// Registers every AppBar from scratch. Needed after Explorer restarts, since
// the new taskbar doesn't know about the old registrations.
void RebuildAppBars() {
    DestroyAppBars();
    SyncAppBars();
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == kAppBarCallbackMsg) {
        // ABN_POSCHANGED: the taskbar or another AppBar moved. Debounce, since
        // every one of our bars receives it.
        if (wParam == ABN_POSCHANGED && !g_positioning) {
            SetTimer(g_controlWnd, kSyncTimerId, kDebounceMs, nullptr);
        }
        return 0;
    }

    if (hwnd != g_controlWnd) {
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }

    if (g_taskbarCreatedMsg && msg == g_taskbarCreatedMsg) {
        // Explorer restarted: all AppBar registrations were lost.
        Wh_Log(L"TaskbarCreated");
        SetTimer(hwnd, kRebuildTimerId, kDebounceMs, nullptr);
        return 0;
    }

    switch (msg) {
        case WM_DISPLAYCHANGE:
        case WM_DPICHANGED:
            SetTimer(hwnd, kSyncTimerId, kDebounceMs, nullptr);
            return 0;

        case WM_SETTINGCHANGE:
            // Monitor scaling changes arrive as a settings change.
            if (wParam == SPI_SETWORKAREA || wParam == SPI_SETNONCLIENTMETRICS ||
                wParam == 0) {
                SetTimer(hwnd, kSyncTimerId, kDebounceMs, nullptr);
            }
            return 0;

        case WM_TIMER:
            KillTimer(hwnd, wParam);
            if (wParam == kRebuildTimerId) {
                KillTimer(hwnd, kSyncTimerId);
                RebuildAppBars();
            } else if (wParam == kSyncTimerId) {
                // Follows monitor, scaling, taskbar and other AppBar changes.
                SyncAppBars();
            }
            return 0;

        case kReloadSettingsMsg:
            LoadSettings();
            SyncAppBars();
            return 0;

        case WM_CLOSE:
            DestroyAppBars();
            DestroyWindow(hwnd);
            return 0;

        case WM_DESTROY:
            g_controlWnd = nullptr;
            PostQuitMessage(0);
            return 0;
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}

DWORD WINAPI WorkerThread(LPVOID param) {
    HANDLE readyEvent = (HANDLE)param;

    // Work with physical pixels on every monitor.
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

    WNDCLASS wc{
        .lpfnWndProc = WndProc,
        .hInstance = GetModuleHandle(nullptr),
        .lpszClassName = kWindowClass,
    };
    if (!RegisterClass(&wc)) {
        Wh_Log(L"RegisterClass failed: %u", GetLastError());
        SetEvent(readyEvent);
        return 1;
    }

    g_taskbarCreatedMsg = RegisterWindowMessage(L"TaskbarCreated");

    // A regular (hidden) top-level window, not message-only, so that it
    // receives broadcasts such as TaskbarCreated and WM_DISPLAYCHANGE.
    g_controlWnd = CreateHiddenWindow();
    if (!g_controlWnd) {
        Wh_Log(L"CreateWindowEx failed: %u", GetLastError());
        UnregisterClass(kWindowClass, GetModuleHandle(nullptr));
        SetEvent(readyEvent);
        return 1;
    }

    // Allow TaskbarCreated through UIPI in case Explorer runs elevated.
    ChangeWindowMessageFilterEx(g_controlWnd, g_taskbarCreatedMsg,
                                MSGFLT_ALLOW, nullptr);

    SyncAppBars();
    SetEvent(readyEvent);

    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    DestroyAppBars();
    UnregisterClass(kWindowClass, GetModuleHandle(nullptr));
    return 0;
}

////////////////////////////////////////////////////////////////////////////////
// Tool mod callbacks

BOOL WhTool_ModInit() {
    LoadSettings();

    HANDLE readyEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
    if (!readyEvent) {
        return FALSE;
    }

    g_thread = CreateThread(nullptr, 0, WorkerThread, readyEvent, 0,
                            &g_threadId);
    if (!g_thread) {
        Wh_Log(L"CreateThread failed: %u", GetLastError());
        CloseHandle(readyEvent);
        return FALSE;
    }

    WaitForSingleObject(readyEvent, INFINITE);
    CloseHandle(readyEvent);

    // The worker exits right away if it couldn't set up its window. Report the
    // failure so the launcher ends the tool process instead of leaving it idle.
    if (!g_controlWnd) {
        WaitForSingleObject(g_thread, INFINITE);
        CloseHandle(g_thread);
        g_thread = nullptr;
        return FALSE;
    }

    return TRUE;
}

void WhTool_ModSettingsChanged() {
    if (g_controlWnd) {
        PostMessage(g_controlWnd, kReloadSettingsMsg, 0, 0);
    }
}

void WhTool_ModUninit() {
    if (!g_thread) {
        return;
    }

    if (g_controlWnd) {
        PostMessage(g_controlWnd, WM_CLOSE, 0, 0);
    } else {
        PostThreadMessage(g_threadId, WM_QUIT, 0, 0);
    }

    WaitForSingleObject(g_thread, INFINITE);
    CloseHandle(g_thread);
    g_thread = nullptr;
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
