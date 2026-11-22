// ==WindhawkMod==
// @id              move-active-window-to-monitor
// @name            Move Active Window to Monitor
// @description     Right Ctrl + Left/Right Arrow sends the active window to the monitor on that side. Right Ctrl + Right Shift + Left/Right sends ALL windows on the active monitor. Maximized windows stay maximized; others keep their size (shrunk to fit).
// @version         1.2.0
// @author          Momin (based on "Move Window to Monitor" by TomberWolf)
// @github          https://github.com/mominmansoor
// @include         windhawk.exe
// @compilerOptions -luser32 -lgdi32 -lshcore
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Move Active Window to Monitor

Hold **Right Ctrl** and press **Left** or **Right Arrow** to send the active
(focused) window to the neighbouring monitor on that side. The window stays
focused, so you can keep pressing the keys to bounce it between monitors.

Hold **Right Ctrl + Right Shift** and press **Left** or **Right Arrow** to send
**all** windows on the active monitor (the one holding the focused window, or
the one under the mouse if nothing is focused) to the neighbouring monitor.
Minimized windows, windows on other virtual desktops, tool windows, the desktop
and the taskbar are left alone.

- Maximized (or borderless fullscreen) windows stay maximized/fullscreen on the
  new monitor.
- Other windows keep their size (scaled for the monitor's DPI) and are centered
  on the new monitor. If a window is bigger than the new monitor it is shrunk
  to fit.
- Monitors are ordered left to right by their position in Windows' display
  settings. Pressing a direction with no monitor on that side does nothing.
- The desktop and taskbar are ignored.

Note: this mod uses a low-level keyboard hook so it can tell Right Ctrl from
Left Ctrl. It only reacts to Right Ctrl + Left/Right Arrow; while Right Ctrl is
held, those two arrow keys are not passed on to other programs.
*/
// ==/WindhawkModReadme==

#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <vector>
#include <atomic>
#include <algorithm>

// Posted from the keyboard hook to the message window so the (potentially
// slow) window moving never runs inside the hook callback.
// wParam: -1 = move left, +1 = move right
#define WM_APP_MOVE_WINDOW (WM_APP + 1)
#define WM_APP_MOVE_ALL    (WM_APP + 2)

struct MonitorInfo {
    HMONITOR hMon;
    RECT     rcMon;
    RECT     rcWork;
    UINT     dpi;
};

static HANDLE            g_hThread  = nullptr;
static DWORD             g_threadId = 0;
static HWND              g_hMsgWnd  = nullptr;
static HHOOK             g_hHook    = nullptr;
static DWORD             g_swallowedKey = 0;
static std::atomic<bool> g_running{false};

// ── monitors ──────────────────────────────────────────────────────────────────

static UINT GetMonitorDpi(HMONITOR hMon) {
    typedef HRESULT (WINAPI* GetDpiForMonitorFn)(HMONITOR, int, UINT*, UINT*);
    static GetDpiForMonitorFn fn = []() -> GetDpiForMonitorFn {
        HMODULE h = LoadLibraryW(L"shcore.dll");
        return h ? reinterpret_cast<GetDpiForMonitorFn>(
                       GetProcAddress(h, "GetDpiForMonitor"))
                 : nullptr;
    }();
    UINT x = 96, y = 96;
    if (fn) fn(hMon, 0 /*MDT_EFFECTIVE_DPI*/, &x, &y);
    return x ? x : 96;
}

static BOOL CALLBACK MonitorEnumProc(HMONITOR hMon, HDC, LPRECT, LPARAM lParam) {
    auto* list = reinterpret_cast<std::vector<MonitorInfo>*>(lParam);
    MONITORINFO mi = { sizeof(mi) };
    if (!GetMonitorInfo(hMon, &mi)) return TRUE;

    MonitorInfo info;
    info.hMon   = hMon;
    info.rcMon  = mi.rcMonitor;
    info.rcWork = mi.rcWork;
    info.dpi    = GetMonitorDpi(hMon);
    list->push_back(info);
    return TRUE;
}

// Sorted left to right (then top to bottom).
static std::vector<MonitorInfo> GetAllMonitors() {
    std::vector<MonitorInfo> list;
    EnumDisplayMonitors(nullptr, nullptr, MonitorEnumProc,
                        reinterpret_cast<LPARAM>(&list));
    std::sort(list.begin(), list.end(),
              [](const MonitorInfo& a, const MonitorInfo& b) {
                  if (a.rcMon.left != b.rcMon.left)
                      return a.rcMon.left < b.rcMon.left;
                  return a.rcMon.top < b.rcMon.top;
              });
    return list;
}

static int FindMonitorIndex(const std::vector<MonitorInfo>& monitors,
                            HMONITOR hMon) {
    for (int i = 0; i < (int)monitors.size(); i++)
        if (monitors[i].hMon == hMon) return i;
    return -1;
}

// WINDOWPLACEMENT uses "workspace" coordinates, which are offset from screen
// coordinates by the primary monitor's work area origin (e.g. a taskbar on the
// left or top). Returns that offset.
static POINT GetWorkspaceOffset() {
    POINT origin = {0, 0};
    HMONITOR hPrimary = MonitorFromPoint(origin, MONITOR_DEFAULTTOPRIMARY);
    MONITORINFO mi = { sizeof(mi) };
    POINT off = {0, 0};
    if (GetMonitorInfo(hPrimary, &mi)) {
        off.x = mi.rcWork.left;
        off.y = mi.rcWork.top;
    }
    return off;
}

// ── active window ─────────────────────────────────────────────────────────────

static HWND GetActiveTopLevelWindow() {
    HWND hwnd = GetForegroundWindow();
    if (!hwnd) return nullptr;

    hwnd = GetAncestor(hwnd, GA_ROOT);
    if (!hwnd || !IsWindowVisible(hwnd) || IsIconic(hwnd)) return nullptr;

    wchar_t cls[64] = {};
    GetClassNameW(hwnd, cls, ARRAYSIZE(cls));
    if (wcscmp(cls, L"Progman") == 0 || wcscmp(cls, L"WorkerW") == 0 ||
        wcscmp(cls, L"Shell_TrayWnd") == 0 ||
        wcscmp(cls, L"Shell_SecondaryTrayWnd") == 0) {
        return nullptr;
    }
    return hwnd;
}

// ── moving ────────────────────────────────────────────────────────────────────

// bulk = true when many windows are moved at once: windows are raised without
// being activated, and moved asynchronously so one hung app can't stall the rest.
static void MoveWindowToMonitor(HWND hwnd, const MonitorInfo& src,
                                const MonitorInfo& dst, bool bulk = false) {
    const int dstW = dst.rcWork.right  - dst.rcWork.left;
    const int dstH = dst.rcWork.bottom - dst.rcWork.top;

    WINDOWPLACEMENT wp = { sizeof(wp) };
    if (!GetWindowPlacement(hwnd, &wp)) return;

    RECT rcWin = {};
    GetWindowRect(hwnd, &rcWin);

    const bool maximized = IsZoomed(hwnd) != FALSE;

    // Borderless fullscreen (F11 browser, video player...): cover the new monitor.
    if (!maximized && EqualRect(&rcWin, &src.rcMon)) {
        SetWindowPos(hwnd, nullptr, dst.rcMon.left, dst.rcMon.top,
                     dst.rcMon.right - dst.rcMon.left,
                     dst.rcMon.bottom - dst.rcMon.top,
                     SWP_NOZORDER | SWP_NOACTIVATE);
        return;
    }

    // Size to carry over: the restored size if maximized, else the current size.
    const RECT rcSize = maximized ? wp.rcNormalPosition : rcWin;
    int w = rcSize.right  - rcSize.left;
    int h = rcSize.bottom - rcSize.top;

    if (src.dpi > 0 && dst.dpi > 0 && src.dpi != dst.dpi) {
        w = MulDiv(w, (int)dst.dpi, (int)src.dpi);
        h = MulDiv(h, (int)dst.dpi, (int)src.dpi);
    }

    // Try to fit.
    w = std::min(w, dstW);
    h = std::min(h, dstH);

    // Center on the new monitor's work area.
    const int x = dst.rcWork.left + (dstW - w) / 2;
    const int y = dst.rcWork.top  + (dstH - h) / 2;

    if (maximized) {
        // Place the restored rect on the destination monitor, then maximize:
        // Windows maximizes onto the monitor that holds the restored rect.
        const POINT off = GetWorkspaceOffset();
        wp.rcNormalPosition =
            RECT{x - off.x, y - off.y, x - off.x + w, y - off.y + h};

        wp.showCmd = SW_SHOWNORMAL;
        SetWindowPlacement(hwnd, &wp);

        wp.showCmd = SW_SHOWMAXIMIZED;
        SetWindowPlacement(hwnd, &wp);
    } else {
        // Applied twice: apps that react to a DPI change can resize themselves
        // right after the first move.
        HWND insertAfter = bulk ? HWND_TOP : nullptr;
        UINT flags = SWP_NOACTIVATE | (bulk ? SWP_ASYNCWINDOWPOS : SWP_NOZORDER);
        SetWindowPos(hwnd, insertAfter, x, y, w, h, flags);
        SetWindowPos(hwnd, insertAfter, x, y, w, h, flags);
    }
}

static void MoveActiveWindow(int direction /* -1 left, +1 right */) {
    HWND hwnd = GetActiveTopLevelWindow();
    if (!hwnd) {
        Wh_Log(L"No movable active window");
        return;
    }

    auto monitors = GetAllMonitors();
    HMONITOR hCur = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
    int curIdx = FindMonitorIndex(monitors, hCur);
    if (curIdx < 0) return;

    int dstIdx = curIdx + direction;
    if (dstIdx < 0 || dstIdx >= (int)monitors.size()) {
        Wh_Log(L"No monitor in that direction (current index %d of %d)",
               curIdx, (int)monitors.size());
        return;
    }

    Wh_Log(L"Moving window from monitor %d to %d", curIdx, dstIdx);
    MoveWindowToMonitor(hwnd, monitors[curIdx], monitors[dstIdx]);
}

// ── move all windows on a monitor ────────────────────────────────────────────

static bool IsWindowCloaked(HWND hwnd) {
    // Windows on other virtual desktops (and suspended UWP windows) are cloaked.
    typedef HRESULT(WINAPI * DwmGetWindowAttributeFn)(HWND, DWORD, PVOID, DWORD);
    static DwmGetWindowAttributeFn fn = []() -> DwmGetWindowAttributeFn {
        HMODULE h = LoadLibraryW(L"dwmapi.dll");
        return h ? reinterpret_cast<DwmGetWindowAttributeFn>(
                       GetProcAddress(h, "DwmGetWindowAttribute"))
                 : nullptr;
    }();
    DWORD cloaked = 0;
    if (fn && SUCCEEDED(fn(hwnd, 14 /*DWMWA_CLOAKED*/, &cloaked, sizeof(cloaked))))
        return cloaked != 0;
    return false;
}

static bool IsBulkMovableWindow(HWND hwnd) {
    if (!IsWindowVisible(hwnd) || IsIconic(hwnd)) return false;
    if (IsWindowCloaked(hwnd)) return false;
    if (GetWindowTextLengthW(hwnd) == 0) return false;

    const LONG_PTR ex = GetWindowLongPtr(hwnd, GWL_EXSTYLE);
    if ((ex & WS_EX_TOOLWINDOW) && !(ex & WS_EX_APPWINDOW)) return false;
    if (ex & WS_EX_NOACTIVATE) return false;

    wchar_t cls[64] = {};
    GetClassNameW(hwnd, cls, ARRAYSIZE(cls));
    if (wcscmp(cls, L"Progman") == 0 || wcscmp(cls, L"WorkerW") == 0 ||
        wcscmp(cls, L"Shell_TrayWnd") == 0 ||
        wcscmp(cls, L"Shell_SecondaryTrayWnd") == 0) {
        return false;
    }
    return true;
}

struct EnumContext {
    HMONITOR             src;
    std::vector<HWND>    windows;
};

static BOOL CALLBACK EnumWindowsCollectProc(HWND hwnd, LPARAM lParam) {
    auto* ctx = reinterpret_cast<EnumContext*>(lParam);
    if (!IsBulkMovableWindow(hwnd)) return TRUE;
    if (MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST) != ctx->src)
        return TRUE;
    ctx->windows.push_back(hwnd);
    return TRUE;
}

static void MoveAllWindows(int direction /* -1 left, +1 right */) {
    // Source monitor: the one with the focused window, else the one under the
    // mouse cursor.
    HMONITOR hSrc = nullptr;
    if (HWND active = GetActiveTopLevelWindow()) {
        hSrc = MonitorFromWindow(active, MONITOR_DEFAULTTONEAREST);
    } else {
        POINT pt;
        if (GetCursorPos(&pt))
            hSrc = MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
    }
    if (!hSrc) return;

    auto monitors = GetAllMonitors();
    int srcIdx = FindMonitorIndex(monitors, hSrc);
    if (srcIdx < 0) return;

    int dstIdx = srcIdx + direction;
    if (dstIdx < 0 || dstIdx >= (int)monitors.size()) {
        Wh_Log(L"No monitor in that direction (current index %d of %d)", srcIdx,
               (int)monitors.size());
        return;
    }

    EnumContext ctx;
    ctx.src = hSrc;
    EnumWindows(EnumWindowsCollectProc, reinterpret_cast<LPARAM>(&ctx));

    Wh_Log(L"Moving %d window(s) from monitor %d to %d",
           (int)ctx.windows.size(), srcIdx, dstIdx);

    // EnumWindows lists top to bottom; move bottom to top so the windows keep
    // their stacking order (the previously active window ends up on top).
    for (auto it = ctx.windows.rbegin(); it != ctx.windows.rend(); ++it) {
        MoveWindowToMonitor(*it, monitors[srcIdx], monitors[dstIdx], true);
    }
}

// ── keyboard hook ─────────────────────────────────────────────────────────────

static bool KeyDown(int vk) {
    return (GetAsyncKeyState(vk) & 0x8000) != 0;
}

static LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam,
                                             LPARAM lParam) {
    if (nCode == HC_ACTION) {
        const auto* kb = reinterpret_cast<const KBDLLHOOKSTRUCT*>(lParam);

        if (!(kb->flags & LLKHF_INJECTED) &&
            (kb->vkCode == VK_LEFT || kb->vkCode == VK_RIGHT)) {
            const bool isDown =
                wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN;
            const bool isUp = wParam == WM_KEYUP || wParam == WM_SYSKEYUP;

            if (isDown) {
                // Right Ctrl with no Left Ctrl / Alt / Win.
                const bool baseMods =
                    KeyDown(VK_RCONTROL) && !KeyDown(VK_LCONTROL) &&
                    !KeyDown(VK_MENU) && !KeyDown(VK_LWIN) &&
                    !KeyDown(VK_RWIN);
                // Right Ctrl           -> move the active window
                // Right Ctrl + R.Shift -> move all windows on the monitor
                UINT msgId = 0;
                if (baseMods && !KeyDown(VK_SHIFT)) {
                    msgId = WM_APP_MOVE_WINDOW;
                } else if (baseMods && KeyDown(VK_RSHIFT) &&
                           !KeyDown(VK_LSHIFT)) {
                    msgId = WM_APP_MOVE_ALL;
                }

                if (msgId) {
                    // Ignore auto-repeat of a key we already handled.
                    if (g_swallowedKey != kb->vkCode) {
                        g_swallowedKey = kb->vkCode;
                        if (g_hMsgWnd) {
                            PostMessage(g_hMsgWnd, msgId,
                                        kb->vkCode == VK_LEFT ? (WPARAM)-1
                                                              : (WPARAM)1,
                                        0);
                        }
                    }
                    return 1;  // swallow
                }
            } else if (isUp && g_swallowedKey == kb->vkCode) {
                g_swallowedKey = 0;
                return 1;  // swallow the matching key-up
            }
        }
    }

    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

// ── hook thread ───────────────────────────────────────────────────────────────

static LRESULT CALLBACK MsgWndProc(HWND hwnd, UINT msg, WPARAM wParam,
                                   LPARAM lParam) {
    if (msg == WM_APP_MOVE_WINDOW) {
        MoveActiveWindow((int)(intptr_t)wParam);
        return 0;
    }
    if (msg == WM_APP_MOVE_ALL) {
        MoveAllWindows((int)(intptr_t)wParam);
        return 0;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

static void HookThreadProc() {
    // Use real (physical) pixel coordinates regardless of the host process'
    // DPI awareness. -4 = DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2
    typedef HANDLE(WINAPI * SetThreadDpiAwarenessContextFn)(HANDLE);
    auto setDpiCtx = reinterpret_cast<SetThreadDpiAwarenessContextFn>(
        GetProcAddress(GetModuleHandleW(L"user32.dll"),
                       "SetThreadDpiAwarenessContext"));
    if (setDpiCtx) setDpiCtx((HANDLE)-4);

    const wchar_t CLASS_NAME[] = L"WH_MoveActiveToMonitor_MsgWnd";
    WNDCLASSEX wc = {};
    wc.cbSize        = sizeof(wc);
    wc.lpfnWndProc   = MsgWndProc;
    wc.hInstance     = GetModuleHandle(nullptr);
    wc.lpszClassName = CLASS_NAME;
    RegisterClassEx(&wc);

    HWND hwnd = CreateWindowEx(0, CLASS_NAME, nullptr, 0, 0, 0, 0, 0,
                               HWND_MESSAGE, nullptr, GetModuleHandle(nullptr),
                               nullptr);
    if (!hwnd) {
        Wh_Log(L"Failed to create message window");
        return;
    }
    g_hMsgWnd = hwnd;

    g_hHook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc,
                               GetModuleHandle(nullptr), 0);
    if (!g_hHook) {
        Wh_Log(L"Failed to install keyboard hook (error %lu)", GetLastError());
    } else {
        auto monitors = GetAllMonitors();
        Wh_Log(L"Ready: %d monitor(s) detected (left to right):",
               (int)monitors.size());
        for (int i = 0; i < (int)monitors.size(); i++) {
            const auto& m = monitors[i];
            Wh_Log(L"  %d: pos (%ld,%ld) size %ldx%ld DPI %u", i, m.rcMon.left,
                   m.rcMon.top, m.rcMon.right - m.rcMon.left,
                   m.rcMon.bottom - m.rcMon.top, m.dpi);
        }
    }

    MSG msg;
    while (g_running && GetMessage(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    if (g_hHook) {
        UnhookWindowsHookEx(g_hHook);
        g_hHook = nullptr;
    }
    DestroyWindow(hwnd);
    g_hMsgWnd = nullptr;
    UnregisterClass(CLASS_NAME, GetModuleHandle(nullptr));
}

static DWORD WINAPI HookThreadEntry(LPVOID) {
    HookThreadProc();
    return 0;
}

// ── Tool mod callbacks ────────────────────────────────────────────────────────

BOOL WhTool_ModInit() {
    Wh_Log(L"Move Active Window to Monitor: Init");
    g_running = true;
    g_hThread = CreateThread(nullptr, 0, HookThreadEntry, nullptr, 0, &g_threadId);
    return g_hThread != nullptr;
}

void WhTool_ModSettingsChanged() {
    // No settings.
}

void WhTool_ModUninit() {
    g_running = false;
    if (g_threadId) PostThreadMessage(g_threadId, WM_QUIT, 0, 0);
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
