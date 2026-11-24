// ==WindhawkMod==
// @id              edge-no-bleed
// @name            Edge No Bleed
// @description     Eliminates 1-2px screen edge gaps where desktop wallpaper bleeds behind maximized windows.
// @version         1.0.0
// @author          Dev-StudioCode
// @github          https://github.com/Dev-StudioCode
// @include         windhawk.exe
// @compilerOptions -ldwmapi
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Edge No Bleed

Eliminates the persistent 1-2px margin gap where desktop wallpaper bleeds through along screen edges behind maximized windows.

## The Problem

On Windows 10 and 11, users running dark-mode themes, IDEs, terminals, and browsers frequently notice a razor-thin 1-2px strip of desktop wallpaper leaking through along the edges of maximized windows (especially at the bottom or side monitor borders).

### Why This Happens

* **DWM Frame Padding & Fractional DPI Rounding:** Windows calculates maximized client coordinates by converting logical device-independent units into physical screen pixels. At fractional display scaling factors (such as 125%, 150%, or 175%), rounding calculations frequently leave a 1-2 physical pixel deficit against the work area, exposing bright desktop wallpaper pixels.
* **Invisible Margin Bounds:** Normal Win32 maximized frames hang invisible resize margins off-screen, but custom-chrome and modern applications (such as Chromium, Electron, and Windows Terminal) size their own frames and can fall short of physical display boundaries.

## How It Works

`Edge No Bleed` runs as a dedicated tool mod within Windhawk without injecting invasive hooks into third-party processes. It monitors window position and state changes via out-of-context WinEvents on a dedicated message-loop worker thread:

1. When a window is maximized, the mod measures its true visible frame bounds using `DwmGetWindowAttribute(DWMWA_EXTENDED_FRAME_BOUNDS)` against the monitor's work area (`MONITORINFO::rcWork`).
2. If a 1-2px fractional rounding gap is detected, the mod expands the window frame via `SetWindowPos` by the exact measured discrepancy, sealing the edge flush against the screen.
3. If an auto-hide taskbar is active on that edge, the mod detects it via `SHAppBarMessage(ABM_GETAUTOHIDEBAREX)` and preserves the 1px mouse-hover reveal strip.

## Comparison With Other Mods

* **Maximized Window Margins:** That mod intentionally adds large margins (10px to 200px, macOS-style) by shrinking the system work area with AppBars. `Edge No Bleed` does the opposite: it eliminates unwanted 1-2px fractional DPI gaps so maximized windows sit completely flush.
* **Classic Maximized Windows Fix:** That mod is exclusively designed for the legacy Classic 3D Theme on Windows 10/11 using region clipping (`SetWindowRgn`). `Edge No Bleed` targets standard modern DWM themes and dark-mode setups.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- clampBottom: true
  $name: Clamp Bottom Edge
  $description: Extends maximized window frames flush against the bottom monitor boundary.
- clampSides: true
  $name: Clamp Side Edges
  $description: Extends maximized window frames flush against left and right monitor boundaries.
- clampTop: false
  $name: Clamp Top Edge
  $description: Extends maximized window frames flush against the top monitor boundary if an appbar or top taskbar leaves a gap.
- maxGapCompensationPx: 2
  $name: Maximum Gap Compensation
  $description: Maximum gap in pixels to compensate for fractional DPI rounding (1 to 8, default 2).
- preserveAutoHideTaskbar: true
  $name: Preserve Auto-Hide Taskbar Strip
  $description: Preserves the 1px edge strip when an auto-hide taskbar is docked at that edge.
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>
#include <windows.h>
#include <dwmapi.h>
#include <shellapi.h>

#include <algorithm>
#include <unordered_map>
#include <vector>

#ifndef ABM_GETAUTOHIDEBAREX
#define ABM_GETAUTOHIDEBAREX 0x0000000b
#endif

// ---------------------------------------------------------------------------
// Settings and State
// ---------------------------------------------------------------------------

struct Settings {
    bool clampBottom = true;
    bool clampSides = true;
    bool clampTop = false;
    int maxGapCompensationPx = 2;
    bool preserveAutoHideTaskbar = true;
} g_settings;

struct WindowAdjustState {
    DWORD lastAdjustTick = 0;
    int adjustCount = 0;
};

static std::unordered_map<HWND, WindowAdjustState> g_adjustStates;
static std::vector<HWINEVENTHOOK> g_winEventHooks;
static HANDLE g_workerThread = nullptr;
static DWORD g_workerThreadId = 0;
constexpr UINT kReloadSettingsMsg = WM_APP + 1;

static void LoadSettings() {
    g_settings.clampBottom = Wh_GetIntSetting(L"clampBottom") != 0;
    g_settings.clampSides = Wh_GetIntSetting(L"clampSides") != 0;
    g_settings.clampTop = Wh_GetIntSetting(L"clampTop") != 0;
    int maxGap = Wh_GetIntSetting(L"maxGapCompensationPx");
    g_settings.maxGapCompensationPx = std::clamp(maxGap, 1, 8);
    g_settings.preserveAutoHideTaskbar = Wh_GetIntSetting(L"preserveAutoHideTaskbar") != 0;
}

// ---------------------------------------------------------------------------
// Window Geometry Inspection & Clamping
// ---------------------------------------------------------------------------

static bool HasAutoHideTaskbarOnEdge(HMONITOR hMon, UINT edge) {
    MONITORINFO mi = {sizeof(MONITORINFO)};
    if (!GetMonitorInfoW(hMon, &mi)) {
        return false;
    }

    APPBARDATA abd = {sizeof(APPBARDATA)};
    abd.uEdge = edge;
    abd.rc = mi.rcMonitor;

    HWND hAutoHide = (HWND)SHAppBarMessage(ABM_GETAUTOHIDEBAREX, &abd);
    return hAutoHide != nullptr;
}

static bool IsShellOrDesktopWindow(HWND hwnd) {
    WCHAR className[64];
    if (GetClassNameW(hwnd, className, ARRAYSIZE(className)) > 0) {
        if (wcscmp(className, L"Progman") == 0 ||
            wcscmp(className, L"WorkerW") == 0 ||
            wcscmp(className, L"Shell_TrayWnd") == 0 ||
            wcscmp(className, L"Shell_SecondaryTrayWnd") == 0 ||
            wcscmp(className, L"Windows.UI.Core.CoreWindow") == 0) {
            return true;
        }
    }
    return false;
}

static void CheckAndClampWindow(HWND hwnd) {
    // Only inspect top-level windows
    if (GetAncestor(hwnd, GA_ROOT) != hwnd) {
        return;
    }

    if (!IsWindow(hwnd) || !IsWindowVisible(hwnd) || IsIconic(hwnd)) {
        return;
    }

    // Reset adjustment count if window is no longer maximized
    if (!IsZoomed(hwnd)) {
        auto it = g_adjustStates.find(hwnd);
        if (it != g_adjustStates.end()) {
            it->second.adjustCount = 0;
        }
        return;
    }

    if (IsShellOrDesktopWindow(hwnd)) {
        return;
    }

    DWORD now = GetTickCount();
    auto& state = g_adjustStates[hwnd];

    // Throttle adjustments per window to avoid rapid updates
    if (now - state.lastAdjustTick < 250) {
        return;
    }

    // Break potential loops if an application resists external sizing
    if (state.adjustCount >= 3) {
        return;
    }

    HMONITOR hMon = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
    if (!hMon) {
        return;
    }

    MONITORINFO mi = {sizeof(MONITORINFO)};
    if (!GetMonitorInfoW(hMon, &mi)) {
        return;
    }

    RECT rcFrame = {0};
    if (FAILED(DwmGetWindowAttribute(hwnd, DWMWA_EXTENDED_FRAME_BOUNDS, &rcFrame, sizeof(rcFrame)))) {
        return;
    }

    int adjustBottom = 0;
    int adjustRight = 0;
    int adjustLeft = 0;
    int adjustTop = 0;

    // Bottom edge check: compute gap first, query taskbar only if gap in range
    if (g_settings.clampBottom) {
        int gapBottom = mi.rcWork.bottom - rcFrame.bottom;
        if (gapBottom > 0 && gapBottom <= g_settings.maxGapCompensationPx) {
            bool skipBottom = g_settings.preserveAutoHideTaskbar && HasAutoHideTaskbarOnEdge(hMon, ABE_BOTTOM);
            if (!skipBottom) {
                adjustBottom = gapBottom;
            }
        }
    }

    // Side edges check
    if (g_settings.clampSides) {
        int gapLeft = rcFrame.left - mi.rcWork.left;
        if (gapLeft > 0 && gapLeft <= g_settings.maxGapCompensationPx) {
            bool skipLeft = g_settings.preserveAutoHideTaskbar && HasAutoHideTaskbarOnEdge(hMon, ABE_LEFT);
            if (!skipLeft) {
                adjustLeft = gapLeft;
            }
        }

        int gapRight = mi.rcWork.right - rcFrame.right;
        if (gapRight > 0 && gapRight <= g_settings.maxGapCompensationPx) {
            bool skipRight = g_settings.preserveAutoHideTaskbar && HasAutoHideTaskbarOnEdge(hMon, ABE_RIGHT);
            if (!skipRight) {
                adjustRight = gapRight;
            }
        }
    }

    // Top edge check
    if (g_settings.clampTop) {
        int gapTop = rcFrame.top - mi.rcWork.top;
        if (gapTop > 0 && gapTop <= g_settings.maxGapCompensationPx) {
            bool skipTop = g_settings.preserveAutoHideTaskbar && HasAutoHideTaskbarOnEdge(hMon, ABE_TOP);
            if (!skipTop) {
                adjustTop = gapTop;
            }
        }
    }

    if (adjustBottom > 0 || adjustRight > 0 || adjustLeft > 0 || adjustTop > 0) {
        RECT rcWindow = {0};
        if (!GetWindowRect(hwnd, &rcWindow)) {
            return;
        }

        state.lastAdjustTick = now;
        state.adjustCount++;

        int newLeft = rcWindow.left - adjustLeft;
        int newTop = rcWindow.top - adjustTop;
        int newWidth = (rcWindow.right - rcWindow.left) + adjustLeft + adjustRight;
        int newHeight = (rcWindow.bottom - rcWindow.top) + adjustTop + adjustBottom;

        SetWindowPos(hwnd, nullptr, newLeft, newTop, newWidth, newHeight,
                     SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_ASYNCWINDOWPOS);
    }
}

static BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam) {
    CheckAndClampWindow(hwnd);
    return TRUE;
}

static void CALLBACK WinEventProc(
    HWINEVENTHOOK hWinEventHook,
    DWORD event,
    HWND hwnd,
    LONG idObject,
    LONG idChild,
    DWORD idEventThread,
    DWORD dwmsEventTime
) {
    if (idObject == OBJID_WINDOW && idChild == CHILDID_SELF && hwnd) {
        if (event == EVENT_OBJECT_DESTROY) {
            g_adjustStates.erase(hwnd);
        } else {
            CheckAndClampWindow(hwnd);
        }
    }
}

// ---------------------------------------------------------------------------
// Worker Thread with Message Loop
// ---------------------------------------------------------------------------

static DWORD WINAPI WorkerThread(LPVOID param) {
    constexpr std::pair<DWORD, DWORD> kEventRanges[] = {
        {EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND},
        {EVENT_SYSTEM_MOVESIZEEND, EVENT_SYSTEM_MOVESIZEEND},
        {EVENT_OBJECT_SHOW, EVENT_OBJECT_SHOW},
        {EVENT_OBJECT_LOCATIONCHANGE, EVENT_OBJECT_LOCATIONCHANGE},
        {EVENT_OBJECT_DESTROY, EVENT_OBJECT_DESTROY},
    };

    for (const auto& [first, last] : kEventRanges) {
        HWINEVENTHOOK hook = SetWinEventHook(
            first,
            last,
            nullptr,
            WinEventProc,
            0,
            0,
            WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS
        );
        if (hook) {
            g_winEventHooks.push_back(hook);
        } else {
            Wh_Log(L"SetWinEventHook failed: %u", GetLastError());
        }
    }

    EnumWindows(EnumWindowsProc, 0);

    // Signal ready to WhTool_ModInit
    SetEvent((HANDLE)param);

    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0) > 0) {
        if (!msg.hwnd && msg.message == kReloadSettingsMsg) {
            LoadSettings();
            EnumWindows(EnumWindowsProc, 0);
            continue;
        }
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    for (HWINEVENTHOOK hook : g_winEventHooks) {
        UnhookWinEvent(hook);
    }
    g_winEventHooks.clear();
    g_adjustStates.clear();

    return 0;
}

// ---------------------------------------------------------------------------
// Tool Mod Lifecycle Callbacks
// ---------------------------------------------------------------------------

BOOL WhTool_ModInit() {
    Wh_Log(L"Initializing Edge No Bleed Tool Process");
    LoadSettings();

    HANDLE readyEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
    if (!readyEvent) {
        return FALSE;
    }

    g_workerThread = CreateThread(nullptr, 0, WorkerThread, readyEvent, 0, &g_workerThreadId);
    if (!g_workerThread) {
        Wh_Log(L"CreateThread failed: %u", GetLastError());
        CloseHandle(readyEvent);
        return FALSE;
    }

    WaitForSingleObject(readyEvent, INFINITE);
    CloseHandle(readyEvent);
    return TRUE;
}

void WhTool_ModSettingsChanged() {
    Wh_Log(L"Posting reload settings message to worker thread");
    if (g_workerThreadId) {
        PostThreadMessage(g_workerThreadId, kReloadSettingsMsg, 0, 0);
    }
}

void WhTool_ModUninit() {
    Wh_Log(L"Uninitializing Edge No Bleed Tool Process");
    if (g_workerThread && g_workerThreadId) {
        PostThreadMessage(g_workerThreadId, WM_QUIT, 0, 0);
        WaitForSingleObject(g_workerThread, INFINITE);
        CloseHandle(g_workerThread);
        g_workerThread = nullptr;
        g_workerThreadId = 0;
    }
}

// ---------------------------------------------------------------------------
// Tool-mod launcher boilerplate (verbatim from Windhawk wiki)
// ---------------------------------------------------------------------------

// clang-format off

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

// clang-format on
