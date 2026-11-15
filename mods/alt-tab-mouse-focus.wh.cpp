// ==WindhawkMod==
// @id              alt-tab-mouse-focus
// @name            AltTab Mouse Focus
// @description     Moves the mouse cursor to the center of the window that gets focused after switching with Alt+Tab.
// @version         1.1
// @author          tafadias
// @github          https://github.com/tafadias
// @include         windhawk.exe
// @license         MIT
// @compilerOptions -luser32 -ldwmapi
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# AltTab Mouse Focus

Moves the mouse cursor to the center of the window that gets focused after
switching windows with **Alt+Tab**.

- Only acts on Alt+Tab. Clicking, other keyboard shortcuts or focus changes
  made by other programs do not move the cursor.
- The cursor is moved once the **Alt** key is released, after the window
  switcher closes.
- Ignores system overlays (desktop, taskbar, the Alt+Tab switcher).
- Moves the cursor as soon as the selected window receives focus
  (`EVENT_SYSTEM_FOREGROUND`), instead of polling.
- Works with multiple monitors: the cursor is centered on the monitor where
  the focused window is.

The mod runs as a tool mod in a dedicated process, so it does not inject into
`explorer.exe`.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- onlyOnWindowChange: false
  $name: Only move when the window changes
  $description: If enabled, only move when the focused window actually changes.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <dwmapi.h>
#include <stdio.h>

#include <atomic>

#define WM_ALT_TAB_QUIT   (WM_APP + 1)
#define WM_ALT_TAB_RELOAD (WM_APP + 2)
#define TIMER_WAIT 1
// How long to wait for the selected window to become foreground after Alt is
// released before giving up. Activation is normally immediate.
#define WAIT_TIMEOUT_MS 1000

static HHOOK g_kbHook = nullptr;
static HWINEVENTHOOK g_weHook = nullptr;
static std::atomic<HWND> g_hwnd{nullptr};
static HANDLE g_thread = nullptr;
static HANDLE g_readyEvent = nullptr;
static bool g_initOk = false;

static bool g_armed = false;
static bool g_waiting = false;
static int g_waitRearms = 0;
static HWND g_fgAtArm = nullptr;

static int g_onlyOnWindowChange = 0;

static void LoadSettings() {
    g_onlyOnWindowChange = Wh_GetIntSetting(L"onlyOnWindowChange") ? 1 : 0;
}

// System overlays that should never receive the cursor.
static bool IsOverlay(HWND hwnd) {
    if (!hwnd)
        return true;

    WCHAR cls[256] = {};
    if (!GetClassNameW(hwnd, cls, ARRAYSIZE(cls)))
        return true;

    static const WCHAR* kOverlays[] = {
        L"Progman",
        L"WorkerW",
        L"Shell_TrayWnd",
        L"Shell_SecondaryTrayWnd",
        L"MultitaskingViewFrame",
        L"ForegroundStaging",
        L"XamlExplorerHostIslandWindow",
        L"#32771",
    };
    for (const WCHAR* name : kOverlays) {
        if (wcscmp(cls, name) == 0)
            return true;
    }
    return false;
}

// The Alt+Tab / Task View switcher hosts. While one is in front, the user has
// not picked a window yet, so a pending wait should keep going.
static bool IsSwitcher(HWND hwnd) {
    if (!hwnd)
        return false;

    WCHAR cls[64] = {};
    if (!GetClassNameW(hwnd, cls, ARRAYSIZE(cls)))
        return false;

    static const WCHAR* kSwitchers[] = {
        L"MultitaskingViewFrame",
        L"XamlExplorerHostIslandWindow",
        L"ForegroundStaging",
        L"#32771",
    };
    for (const WCHAR* name : kSwitchers) {
        if (wcscmp(cls, name) == 0)
            return true;
    }
    return false;
}

static bool GetVisibleRect(HWND hwnd, RECT* rc) {
    if (SUCCEEDED(DwmGetWindowAttribute(hwnd, DWMWA_EXTENDED_FRAME_BOUNDS, rc, sizeof(*rc))))
        return true;
    return GetWindowRect(hwnd, rc) != FALSE;
}

static bool MoveToWindow(HWND fg) {
    if (!fg || IsOverlay(fg))
        return false;
    if (IsIconic(fg))
        return false;

    // Only relevant if the window changed.
    if (g_onlyOnWindowChange && fg == g_fgAtArm)
        return false;

    RECT rc;
    if (!GetVisibleRect(fg, &rc))
        return false;

    LONG w = rc.right - rc.left;
    LONG h = rc.bottom - rc.top;
    if (w <= 0 || h <= 0)
        return false;

    SetCursorPos(rc.left + w / 2, rc.top + h / 2);
    return true;
}

// The selected window becomes foreground once the switcher closes. Move the
// cursor only then, so the window being left is never centered by mistake.
static void CALLBACK WinEventProc(HWINEVENTHOOK, DWORD event, HWND hwnd, LONG idObject, LONG,
                                  DWORD, DWORD) {
    if (event != EVENT_SYSTEM_FOREGROUND || idObject != OBJID_WINDOW)
        return;
    if (!g_waiting || IsOverlay(hwnd))
        return;

    g_waiting = false;
    HWND wnd = g_hwnd.load();
    if (wnd)
        KillTimer(wnd, TIMER_WAIT);
    MoveToWindow(hwnd);
}

static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_TIMER:
        if (wParam == TIMER_WAIT) {
            KillTimer(hwnd, TIMER_WAIT);
            if (g_waiting && g_waitRearms < 30 && IsSwitcher(GetForegroundWindow())) {
                // The switcher is still open (e.g. Ctrl+Alt+Tab): keep waiting,
                // but never indefinitely.
                g_waitRearms++;
                SetTimer(hwnd, TIMER_WAIT, WAIT_TIMEOUT_MS, nullptr);
            } else {
                // No activation arrived after Alt release: give up, no late move.
                g_waiting = false;
            }
        }
        return 0;
    case WM_ALT_TAB_RELOAD:
        LoadSettings();
        return 0;
    case WM_ALT_TAB_QUIT:
        DestroyWindow(hwnd);
        return 0;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

static LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        auto* kbd = reinterpret_cast<KBDLLHOOKSTRUCT*>(lParam);
        bool isDown = (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN);
        bool isUp = (wParam == WM_KEYUP || wParam == WM_SYSKEYUP);

        if (isDown && kbd->vkCode == VK_TAB && (kbd->flags & LLKHF_ALTDOWN)) {
            // Alt+Tab: arm, cancel any pending move, remember the current window.
            g_armed = true;
            g_waiting = false;
            g_waitRearms = 0;
            g_fgAtArm = GetForegroundWindow();
            HWND hwnd = g_hwnd.load();
            if (hwnd)
                KillTimer(hwnd, TIMER_WAIT);
        } else if (isDown && kbd->vkCode == VK_ESCAPE) {
            // Cancel a pending wait too: Esc dismisses the open switcher.
            g_armed = false;
            g_waiting = false;
            HWND hwnd = g_hwnd.load();
            if (hwnd)
                KillTimer(hwnd, TIMER_WAIT);
        } else if (isUp && g_armed &&
                   (kbd->vkCode == VK_MENU || kbd->vkCode == VK_LMENU ||
                    kbd->vkCode == VK_RMENU)) {
            // Alt released: wait for the selected window to get focus.
            g_armed = false;
            g_waiting = true;
            g_waitRearms = 0;
            HWND hwnd = g_hwnd.load();
            if (hwnd)
                SetTimer(hwnd, TIMER_WAIT, WAIT_TIMEOUT_MS, nullptr);
        }
    }
    return CallNextHookEx(g_kbHook, nCode, wParam, lParam);
}

static HMODULE GetCurrentModule() {
    HMODULE module = nullptr;
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                           GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                       (LPCWSTR)&WndProc, &module);
    return module;
}

static DWORD WINAPI ThreadProc(LPVOID) {
    WNDCLASSW wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = GetCurrentModule();
    wc.lpszClassName = L"AltTabMouseFocusWindow";

    if (!RegisterClassW(&wc)) {
        Wh_Log(L"RegisterClassW failed: %u", GetLastError());
        g_initOk = false;
        SetEvent(g_readyEvent);
        return 1;
    }

    HWND hwnd = CreateWindowExW(0, wc.lpszClassName, L"", 0, 0, 0, 0, 0, HWND_MESSAGE, nullptr,
                                wc.hInstance, nullptr);
    if (!hwnd) {
        Wh_Log(L"CreateWindowExW failed: %u", GetLastError());
        UnregisterClassW(wc.lpszClassName, wc.hInstance);
        g_initOk = false;
        SetEvent(g_readyEvent);
        return 1;
    }
    g_hwnd = hwnd;

    g_kbHook = SetWindowsHookExW(WH_KEYBOARD_LL, LowLevelKeyboardProc, wc.hInstance, 0);
    if (!g_kbHook) {
        Wh_Log(L"SetWindowsHookExW failed: %u", GetLastError());
        DestroyWindow(hwnd);
        UnregisterClassW(wc.lpszClassName, wc.hInstance);
        g_initOk = false;
        SetEvent(g_readyEvent);
        return 1;
    }

    g_weHook = SetWinEventHook(EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND, nullptr,
                               WinEventProc, 0, 0,
                               WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS);
    if (!g_weHook) {
        Wh_Log(L"SetWinEventHook failed: %u", GetLastError());
        UnhookWindowsHookEx(g_kbHook);
        g_kbHook = nullptr;
        DestroyWindow(hwnd);
        UnregisterClassW(wc.lpszClassName, wc.hInstance);
        g_initOk = false;
        SetEvent(g_readyEvent);
        return 1;
    }

    g_initOk = true;
    SetEvent(g_readyEvent);

    MSG msg;
    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    if (g_weHook) {
        UnhookWinEvent(g_weHook);
        g_weHook = nullptr;
    }
    if (g_kbHook) {
        UnhookWindowsHookEx(g_kbHook);
        g_kbHook = nullptr;
    }
    UnregisterClassW(wc.lpszClassName, wc.hInstance);
    return 0;
}

BOOL WhTool_ModInit() {
    LoadSettings();

    g_readyEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_readyEvent) {
        Wh_Log(L"CreateEvent failed: %u", GetLastError());
        return FALSE;
    }

    g_thread = CreateThread(nullptr, 0, ThreadProc, nullptr, 0, nullptr);
    if (!g_thread) {
        Wh_Log(L"CreateThread failed: %u", GetLastError());
        CloseHandle(g_readyEvent);
        g_readyEvent = nullptr;
        return FALSE;
    }

    WaitForSingleObject(g_readyEvent, INFINITE);

    if (!g_initOk) {
        WaitForSingleObject(g_thread, INFINITE);
        CloseHandle(g_thread);
        g_thread = nullptr;
        CloseHandle(g_readyEvent);
        g_readyEvent = nullptr;
        return FALSE;
    }

    return TRUE;
}

void WhTool_ModSettingsChanged() {
    HWND hwnd = g_hwnd.load();
    if (hwnd)
        PostMessageW(hwnd, WM_ALT_TAB_RELOAD, 0, 0);
}

void WhTool_ModUninit() {
    if (g_readyEvent)
        WaitForSingleObject(g_readyEvent, INFINITE);

    HWND hwnd = g_hwnd.load();
    if (hwnd)
        PostMessageW(hwnd, WM_ALT_TAB_QUIT, 0, 0);

    if (g_thread) {
        WaitForSingleObject(g_thread, INFINITE);
        CloseHandle(g_thread);
        g_thread = nullptr;
    }

    if (g_readyEvent) {
        CloseHandle(g_readyEvent);
        g_readyEvent = nullptr;
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
