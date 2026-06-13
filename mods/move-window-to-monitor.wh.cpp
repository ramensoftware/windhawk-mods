// ==WindhawkMod==
// @id              move-window-to-monitor
// @name            Move Window to Monitor
// @description     Moves the active window to a specific monitor using configurable hotkeys (default: Ctrl+Alt+Arrows)
// @version         1.1.0
// @author          TomberWolf
// @github          https://github.com/TomberWolf
// @include         windhawk.exe
// @compilerOptions -luser32 -lgdi32 -lshcore
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Move Window to Monitor

Moves the active window to a specific monitor using configurable hotkeys.

## Default Shortcut: Ctrl+Alt+Arrow keys

## Setup

Monitors are numbered automatically, sorted **left to right, then top to bottom**
based on their physical position in your Windows display layout.


**Example for a setup with 3 monitors on the bottom and 1 on top center:**

```
        [ 1 ]
[ 0 ]   [ 2 ]   [ 3 ]
```

Set: UP=1, LEFT=0, DOWN=2, RIGHT=3

Use **-1** to keep automatic geometric detection for a direction.

When the active window is already on the target monitor, the hotkey does nothing.

The **Center window** option centers the window on the target monitor instead
of preserving its relative position. Works independently of **Keep window size**.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- modifierKeys: "ctrl_alt"
  $name: Modifier Keys
  $options:
    - ctrl_alt: Ctrl+Alt
    - ctrl_shift: Ctrl+Shift
    - alt_shift: Alt+Shift
    - ctrl_alt_shift: Ctrl+Alt+Shift
- targetUp: -1
  $name: "Target monitor UP (-1 = automatic)"
  $description: Index of the monitor to move to when pressing the UP hotkey
- targetDown: -1
  $name: "Target monitor DOWN (-1 = automatic)"
  $description: Index of the monitor to move to when pressing the DOWN hotkey
- targetLeft: -1
  $name: "Target monitor LEFT (-1 = automatic)"
  $description: Index of the monitor to move to when pressing the LEFT hotkey
- targetRight: -1
  $name: "Target monitor RIGHT (-1 = automatic)"
  $description: Index of the monitor to move to when pressing the RIGHT hotkey
- keepSize: true
  $name: Keep window size when moving
  $description: If disabled, the window is scaled proportionally to the target monitor
- centerOnMove: false
  $name: Center window on target monitor
  $description: If enabled, the window is centered on the target monitor instead of keeping its relative position
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <shellscalingapi.h>
#include <vector>
#include <atomic>
#include <algorithm>

#define HOTKEY_UP    1
#define HOTKEY_DOWN  2
#define HOTKEY_LEFT  3
#define HOTKEY_RIGHT 4

// Custom message posted from WhTool_ModSettingsChanged to the hotkey thread
// so that (Un)RegisterHotKey always runs on the thread that owns the window.
#define WM_APP_SETTINGS_CHANGED (WM_APP + 1)

struct ModSettings {
    UINT modifierKeys = MOD_CONTROL | MOD_ALT;
    int  targetUp     = -1;
    int  targetDown   = -1;
    int  targetLeft   = -1;
    int  targetRight  = -1;
    bool keepSize     = true;
    bool centerOnMove = false;
};

static ModSettings       g_settings;
static HANDLE            g_hThread = nullptr;
static HWND              g_hMsgWnd = nullptr;
static std::atomic<bool> g_running{false};

// ── settings ──────────────────────────────────────────────────────────────────

static UINT SettingToMod(const wchar_t* v) {
    if (wcscmp(v, L"ctrl_shift")     == 0) return MOD_CONTROL | MOD_SHIFT;
    if (wcscmp(v, L"alt_shift")      == 0) return MOD_ALT     | MOD_SHIFT;
    if (wcscmp(v, L"ctrl_alt_shift") == 0) return MOD_CONTROL | MOD_ALT | MOD_SHIFT;
    return MOD_CONTROL | MOD_ALT;
}

static void LoadSettings() {
    PCWSTR s = Wh_GetStringSetting(L"modifierKeys");
    g_settings.modifierKeys = SettingToMod(s);
    Wh_FreeStringSetting(s);
    g_settings.targetUp     = Wh_GetIntSetting(L"targetUp");
    g_settings.targetDown   = Wh_GetIntSetting(L"targetDown");
    g_settings.targetLeft   = Wh_GetIntSetting(L"targetLeft");
    g_settings.targetRight  = Wh_GetIntSetting(L"targetRight");
    g_settings.keepSize     = Wh_GetIntSetting(L"keepSize") != 0;
    g_settings.centerOnMove = Wh_GetIntSetting(L"centerOnMove") != 0;
}

// ── monitor enumeration ───────────────────────────────────────────────────────

struct MonitorInfo {
    HMONITOR hMon;
    RECT     rcWork;
    UINT     dpiX;
    UINT     dpiY;
};

static BOOL CALLBACK MonitorEnumProc(HMONITOR hMon, HDC, LPRECT, LPARAM lParam) {
    auto* list = reinterpret_cast<std::vector<MonitorInfo>*>(lParam);
    MONITORINFO mi = { sizeof(mi) };
    if (!GetMonitorInfo(hMon, &mi)) return TRUE;

    MonitorInfo info;
    info.hMon   = hMon;
    info.rcWork = mi.rcWork;
    info.dpiX   = 96;
    info.dpiY   = 96;

    typedef HRESULT (WINAPI* GetDpiForMonitorFn)(HMONITOR, int, UINT*, UINT*);
    static auto fn = reinterpret_cast<GetDpiForMonitorFn>(
        GetProcAddress(GetModuleHandle(L"shcore.dll"), "GetDpiForMonitor"));
    if (fn) fn(hMon, 0 /*MDT_EFFECTIVE_DPI*/, &info.dpiX, &info.dpiY);

    list->push_back(info);
    return TRUE;
}

static std::vector<MonitorInfo> GetAllMonitors() {
    std::vector<MonitorInfo> list;
    EnumDisplayMonitors(nullptr, nullptr, MonitorEnumProc,
                        reinterpret_cast<LPARAM>(&list));
    std::sort(list.begin(), list.end(), [](const MonitorInfo& a, const MonitorInfo& b) {
        if (a.rcWork.left != b.rcWork.left)
            return a.rcWork.left < b.rcWork.left;
        return a.rcWork.top < b.rcWork.top;
    });
    return list;
}

static POINT RectCenter(const RECT& r) {
    return { (r.left + r.right) / 2, (r.top + r.bottom) / 2 };
}

static int FindMonitorIndex(const std::vector<MonitorInfo>& monitors, HMONITOR hMon) {
    for (int i = 0; i < (int)monitors.size(); i++)
        if (monitors[i].hMon == hMon) return i;
    return -1;
}

static void LogAllMonitors(const std::vector<MonitorInfo>& monitors) {
    Wh_Log(L"=== Move Window to Monitor v6: %d monitor(s) detected ===",
           (int)monitors.size());
    Wh_Log(L"Sorted left-to-right, then top-to-bottom:");
    for (int i = 0; i < (int)monitors.size(); i++) {
        const auto& m = monitors[i];
        int w = m.rcWork.right  - m.rcWork.left;
        int h = m.rcWork.bottom - m.rcWork.top;
        Wh_Log(L"  Index %d | pos (%d, %d) | size %dx%d | DPI %u",
               i, m.rcWork.left, m.rcWork.top, w, h, m.dpiX);
    }
    Wh_Log(L"Settings: UP=%d DOWN=%d LEFT=%d RIGHT=%d  keepSize=%d  center=%d",
           g_settings.targetUp, g_settings.targetDown,
           g_settings.targetLeft, g_settings.targetRight,
           (int)g_settings.keepSize, (int)g_settings.centerOnMove);
}

// ── window moving ─────────────────────────────────────────────────────────────

enum class Direction { Up, Down, Left, Right };

static void MoveWindowToMonitor(HWND hwnd, const MonitorInfo& src, const MonitorInfo& dst) {
    bool wasMaximized = IsZoomed(hwnd);
    if (wasMaximized) ShowWindowAsync(hwnd, SW_RESTORE);

    RECT rcWin = {};
    GetWindowRect(hwnd, &rcWin);

    int ww   = rcWin.right  - rcWin.left;
    int wh   = rcWin.bottom - rcWin.top;
    int srcW = src.rcWork.right  - src.rcWork.left;
    int srcH = src.rcWork.bottom - src.rcWork.top;
    int dstW = dst.rcWork.right  - dst.rcWork.left;
    int dstH = dst.rcWork.bottom - dst.rcWork.top;

    float relX = srcW > 0 ? (float)(rcWin.left - src.rcWork.left) / srcW : 0.0f;
    float relY = srcH > 0 ? (float)(rcWin.top  - src.rcWork.top)  / srcH : 0.0f;

    int newW, newH;
    if (g_settings.keepSize) {
        float dpiScaleX = (dst.dpiX > 0 && src.dpiX > 0)
                          ? (float)dst.dpiX / src.dpiX : 1.0f;
        float dpiScaleY = (dst.dpiY > 0 && src.dpiY > 0)
                          ? (float)dst.dpiY / src.dpiY : 1.0f;
        newW = (int)(ww * dpiScaleX);
        newH = (int)(wh * dpiScaleY);
    } else {
        newW = (int)(ww * (float)dstW / srcW);
        newH = (int)(wh * (float)dstH / srcH);
    }

    int newX, newY;
    if (g_settings.centerOnMove) {
        newX = dst.rcWork.left + (dstW - newW) / 2;
        newY = dst.rcWork.top  + (dstH - newH) / 2;
    } else {
        newX = dst.rcWork.left + (int)(relX * dstW);
        newY = dst.rcWork.top  + (int)(relY * dstH);
    }

    if (newX + newW > dst.rcWork.right)  newX = dst.rcWork.right  - newW;
    if (newY + newH > dst.rcWork.bottom) newY = dst.rcWork.bottom - newH;
    if (newX < dst.rcWork.left)          newX = dst.rcWork.left;
    if (newY < dst.rcWork.top)           newY = dst.rcWork.top;

    SetWindowPos(hwnd, nullptr, newX, newY, newW, newH,
                 SWP_NOZORDER | SWP_NOACTIVATE);

    if (wasMaximized) ShowWindowAsync(hwnd, SW_MAXIMIZE);
}

static void MoveActiveWindowInDirection(Direction dir) {
    HWND hwnd = GetForegroundWindow();
    if (!hwnd) return;

    HMONITOR hCurrent = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
    MONITORINFO miCurrent = { sizeof(miCurrent) };
    if (!GetMonitorInfo(hCurrent, &miCurrent)) return;

    auto monitors = GetAllMonitors();
    int  curIdx   = FindMonitorIndex(monitors, hCurrent);
    if (curIdx < 0) return;

    int fixedIndex = -1;
    switch (dir) {
        case Direction::Up:    fixedIndex = g_settings.targetUp;    break;
        case Direction::Down:  fixedIndex = g_settings.targetDown;  break;
        case Direction::Left:  fixedIndex = g_settings.targetLeft;  break;
        case Direction::Right: fixedIndex = g_settings.targetRight; break;
    }

    int dstIdx = -1;

    if (fixedIndex >= 0 && fixedIndex < (int)monitors.size()) {
        if (fixedIndex != curIdx)
            dstIdx = fixedIndex;
    }

    if (dstIdx < 0 && fixedIndex < 0) {
        POINT curCenter = RectCenter(monitors[curIdx].rcWork);
        int   bestScore = INT_MAX;

        for (int i = 0; i < (int)monitors.size(); i++) {
            if (i == curIdx) continue;
            POINT c  = RectCenter(monitors[i].rcWork);
            int   dx = c.x - curCenter.x;
            int   dy = c.y - curCenter.y;
            bool  candidate = false;
            int   primary   = 0;

            switch (dir) {
                case Direction::Up:
                    candidate = (dy < 0) && (abs(dy) >= abs(dx)); primary = -dy; break;
                case Direction::Down:
                    candidate = (dy > 0) && (abs(dy) >= abs(dx)); primary = dy;  break;
                case Direction::Left:
                    candidate = (dx < 0) && (abs(dx) >= abs(dy)); primary = -dx; break;
                case Direction::Right:
                    candidate = (dx > 0) && (abs(dx) >= abs(dy)); primary = dx;  break;
            }

            if (candidate && primary < bestScore) {
                bestScore = primary;
                dstIdx    = i;
            }
        }
    }

    if (dstIdx < 0) return;
    MoveWindowToMonitor(hwnd, monitors[curIdx], monitors[dstIdx]);
}

// ── hotkey thread ─────────────────────────────────────────────────────────────

static void RegisterHotkeys(HWND hwnd) {
    UINT mod = g_settings.modifierKeys | MOD_NOREPEAT;
    RegisterHotKey(hwnd, HOTKEY_UP,    mod, VK_UP);
    RegisterHotKey(hwnd, HOTKEY_DOWN,  mod, VK_DOWN);
    RegisterHotKey(hwnd, HOTKEY_LEFT,  mod, VK_LEFT);
    RegisterHotKey(hwnd, HOTKEY_RIGHT, mod, VK_RIGHT);
}

static void UnregisterHotkeys(HWND hwnd) {
    UnregisterHotKey(hwnd, HOTKEY_UP);
    UnregisterHotKey(hwnd, HOTKEY_DOWN);
    UnregisterHotKey(hwnd, HOTKEY_LEFT);
    UnregisterHotKey(hwnd, HOTKEY_RIGHT);
}

static LRESULT CALLBACK HotkeyWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == WM_HOTKEY) {
        switch ((int)wParam) {
            case HOTKEY_UP:    MoveActiveWindowInDirection(Direction::Up);    break;
            case HOTKEY_DOWN:  MoveActiveWindowInDirection(Direction::Down);  break;
            case HOTKEY_LEFT:  MoveActiveWindowInDirection(Direction::Left);  break;
            case HOTKEY_RIGHT: MoveActiveWindowInDirection(Direction::Right); break;
        }
        return 0;
    }

    if (msg == WM_APP_SETTINGS_CHANGED) {
        // (Un)RegisterHotKey must run on the thread that owns the window
        UnregisterHotkeys(hwnd);
        LoadSettings();
        auto monitors = GetAllMonitors();
        LogAllMonitors(monitors);
        RegisterHotkeys(hwnd);
        return 0;
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}

static void HotkeyThreadProc() {
    const wchar_t CLASS_NAME[] = L"WH_MoveToMonitor_MsgWnd";
    WNDCLASSEX wc = {};
    wc.cbSize        = sizeof(wc);
    wc.lpfnWndProc   = HotkeyWndProc;
    wc.hInstance     = GetModuleHandle(nullptr);
    wc.lpszClassName = CLASS_NAME;
    RegisterClassEx(&wc);

    HWND hwnd = CreateWindowEx(0, CLASS_NAME, nullptr, 0, 0, 0, 0, 0,
                               HWND_MESSAGE, nullptr, GetModuleHandle(nullptr), nullptr);
    if (!hwnd) { Wh_Log(L"Failed to create message window"); return; }

    g_hMsgWnd = hwnd;
    RegisterHotkeys(hwnd);

    MSG msg;
    while (g_running && GetMessage(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    UnregisterHotkeys(hwnd);
    DestroyWindow(hwnd);
    g_hMsgWnd = nullptr;
    UnregisterClass(CLASS_NAME, GetModuleHandle(nullptr));
}

static DWORD WINAPI HotkeyThreadEntry(LPVOID) {
    HotkeyThreadProc();
    return 0;
}

// ── Tool mod callbacks (renamed as required by the tool mod pattern) ──────────

BOOL WhTool_ModInit() {
    Wh_Log(L"MoveWindowToMonitor v6: Init");
    LoadSettings();
    auto monitors = GetAllMonitors();
    LogAllMonitors(monitors);
    g_running = true;
    g_hThread = CreateThread(nullptr, 0, HotkeyThreadEntry, nullptr, 0, nullptr);
    return g_hThread != nullptr;
}

void WhTool_ModSettingsChanged() {
    Wh_Log(L"MoveWindowToMonitor: Settings changed, notifying hotkey thread");
    if (g_hMsgWnd) PostMessage(g_hMsgWnd, WM_APP_SETTINGS_CHANGED, 0, 0);
}

void WhTool_ModUninit() {
    g_running = false;
    if (g_hMsgWnd) PostMessage(g_hMsgWnd, WM_QUIT, 0, 0);
    if (g_hThread) {
        WaitForSingleObject(g_hThread, 3000);
        CloseHandle(g_hThread);
        g_hThread = nullptr;
    }
}

////////////////////////////////////////////////////////////////////////////////
// Windhawk tool mod implementation for mods which don't need to inject to other
// processes or hook other functions. Context:
// https://github.com/ramensoftware/windhawk/wiki/Mods-as-tools:-Running-mods-in-a-dedicated-process
//
// The mod will load and run in a dedicated windhawk.exe process.

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

    WCHAR commandLine[MAX_PATH + 2 +
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
