// ==WindhawkMod==
// @id              same-app-window-cycler
// @name            Same-App Window Cycler (Alt+`)
// @description     macOS-style Cmd+` for Windows: cycle through the windows of the current app with Alt+` (Alt+Shift+` goes backwards)
// @version         0.1.2
// @author          mdozkanji
// @github          https://github.com/mdozkanji
// @include         windhawk.exe
// @compilerOptions -ldwmapi -lshell32
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Same-App Window Cycler

Brings the macOS **Cmd+\`** behavior to Windows. While an app has several
windows open, press **Alt+\`** to jump to its next window, or **Alt+Shift+\`**
to go to the previous one. Repeated presses walk through all of the app's
windows, and Alt+Shift+\` walks through them in the opposite order.

The shortcut uses the physical key left of `1`. The key is mapped when the mod
starts, so it follows the keyboard layout that is active at that time.

## How it differs from the existing switcher mods
Simple Window Switcher, ClassicWindowSwitcher and Windows 7/8.1 Alt+Tab
Recreation also offer an Alt+\` "windows of the current app" mode, but all of
them replace the native Alt+Tab and show their own switcher UI. This mod leaves
Alt+Tab untouched and switches windows directly, with no UI. If you already use
one of those mods for Alt+\`, you don't need this one.

## Conflicts
Alt+\` is registered as a global hotkey, and only one program can own it.
- Simple Window Switcher registers Alt+\` by default (`altBacktickBehavior:
  backward`), so if both mods are enabled, whichever registers first gets the
  key and the other one only logs a failure. Set its Alt+\` behavior to "none",
  or don't enable both.
- Windows 7/8.1 Alt+Tab Recreation catches Alt+\` with a low-level keyboard hook
  and swallows it. With its `onlyThisAppHotkey` option on, that mod gets the key
  no matter which one registers first, and this mod's hotkey never fires.
- The same applies to other tools that use Alt+\`, such as AutoHotkey scripts.
- Apps that have their own Alt+\` or Alt+Shift+\` shortcut stop receiving it
  while this mod is on.

## How it works
The mod runs as a tool mod in a dedicated `windhawk.exe` process and registers a
global hotkey. When pressed, it takes the foreground window, finds all other
switchable top-level windows that belong to the same executable on the current
virtual desktop, and activates the next one.

## Limitations
- Windows are grouped by executable path. UWP apps hosted by
  ApplicationFrameHost.exe are all treated as one "app".
- Windows of elevated (administrator) apps may only flash instead of coming to
  the front.
- If you switch keyboard layouts and the key left of `1` produces a different
  virtual key, the hotkey stays on the key it was registered with. Disable and
  re-enable the mod after changing layouts.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- includeMinimized: true
  $name: Include minimized windows
  $description: Restore and switch to minimized windows of the same app.
*/
// ==/WindhawkModSettings==

#include <dwmapi.h>

#include <atomic>
#include <string>
#include <vector>

struct {
    // Written on the Windhawk engine thread, read on the hotkey thread.
    std::atomic<bool> includeMinimized;
} settings;

HANDLE g_hotkeyThread = nullptr;
DWORD g_hotkeyThreadId = 0;
HANDLE g_hotkeyThreadReady = nullptr;

constexpr int kHotkeyNext = 1;
constexpr int kHotkeyPrev = 2;

static bool GetExePath(HWND hwnd, std::wstring& out) {
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    if (!pid) {
        return false;
    }

    HANDLE process = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!process) {
        return false;
    }

    WCHAR buffer[MAX_PATH * 2];
    DWORD size = ARRAYSIZE(buffer);
    bool ok = QueryFullProcessImageNameW(process, 0, buffer, &size);
    CloseHandle(process);

    if (ok) {
        out.assign(buffer, size);
    }
    return ok;
}

// Roughly the same filter Alt+Tab uses.
static bool IsSwitchable(HWND hwnd) {
    if (!IsWindowVisible(hwnd)) {
        return false;
    }

    LONG exStyle = GetWindowLongW(hwnd, GWL_EXSTYLE);
    bool appWindow = (exStyle & WS_EX_APPWINDOW) != 0;

    if ((exStyle & WS_EX_TOOLWINDOW) && !appWindow) {
        return false;
    }

    if (GetWindow(hwnd, GW_OWNER) && !appWindow) {
        return false;
    }

    // Cloaked windows include those on other virtual desktops.
    DWORD cloaked = 0;
    if (SUCCEEDED(DwmGetWindowAttribute(hwnd, DWMWA_CLOAKED, &cloaked,
                                        sizeof(cloaked))) &&
        cloaked) {
        return false;
    }

    // This mod runs in its own process, so for windows of other processes this
    // doesn't send a message and can't block on a hung window.
    if (GetWindowTextLengthW(hwnd) == 0) {
        return false;
    }

    if (!settings.includeMinimized && IsIconic(hwnd)) {
        return false;
    }

    return true;
}

struct EnumContext {
    std::wstring exePath;
    std::vector<HWND> windows;  // Z-order, topmost first.
};

static BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam) {
    auto* ctx = reinterpret_cast<EnumContext*>(lParam);

    if (!IsSwitchable(hwnd)) {
        return TRUE;
    }

    std::wstring path;
    if (GetExePath(hwnd, path) &&
        _wcsicmp(path.c_str(), ctx->exePath.c_str()) == 0) {
        ctx->windows.push_back(hwnd);
    }
    return TRUE;
}

static void ActivateWindow(HWND hwnd) {
    if (IsIconic(hwnd)) {
        // Async, so that a hung app can't block the hotkey thread.
        ShowWindowAsync(hwnd, SW_RESTORE);
    }
    SetForegroundWindow(hwnd);
}

static void CycleWindows(bool backwards) {
    HWND foreground = GetForegroundWindow();
    if (!foreground) {
        return;
    }

    EnumContext ctx;
    if (!GetExePath(foreground, ctx.exePath)) {
        return;
    }

    EnumWindows(EnumWindowsProc, reinterpret_cast<LPARAM>(&ctx));

    if (ctx.windows.size() < 2) {
        return;
    }

    // Z-order is topmost first. Forward: bring the least recently used window
    // of the app to the front, so repeated presses walk through all windows.
    HWND target = ctx.windows.back();

    if (backwards) {
        if (ctx.windows[0] == foreground) {
            // Exact reverse of forward: push the current window below the
            // app's last window, then activate the one that was behind it.
            // Without this, backward would only toggle between two windows.
            SetWindowPos(ctx.windows[0], ctx.windows.back(), 0, 0, 0, 0,
                         SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE |
                             SWP_ASYNCWINDOWPOS);
            target = ctx.windows[1];
        } else {
            // The foreground window isn't one of the listed windows (for
            // example the desktop), so go to the most recent window.
            target = ctx.windows[0];
        }
    }

    Wh_Log(L"Cycling %zu windows of %s", ctx.windows.size(),
           ctx.exePath.c_str());
    ActivateWindow(target);
}

static DWORD WINAPI HotkeyThreadProc(LPVOID) {
    // Create this thread's message queue, then signal that it's safe to post
    // messages (such as WM_QUIT) to it.
    MSG msg;
    PeekMessageW(&msg, nullptr, WM_USER, WM_USER, PM_NOREMOVE);
    SetEvent(g_hotkeyThreadReady);

    // Physical key to the left of "1", regardless of keyboard layout.
    UINT vk = MapVirtualKeyW(0x29, MAPVK_VSC_TO_VK);
    if (!vk) {
        vk = VK_OEM_3;
    }

    bool nextOk =
        RegisterHotKey(nullptr, kHotkeyNext, MOD_ALT | MOD_NOREPEAT, vk);
    bool prevOk = RegisterHotKey(nullptr, kHotkeyPrev,
                                 MOD_ALT | MOD_SHIFT | MOD_NOREPEAT, vk);

    if (!nextOk || !prevOk) {
        Wh_Log(L"RegisterHotKey failed (next=%d, prev=%d, error=%lu)", nextOk,
               prevOk, GetLastError());
    }

    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        if (msg.message == WM_HOTKEY) {
            CycleWindows(msg.wParam == kHotkeyPrev);
        }
    }

    if (nextOk) {
        UnregisterHotKey(nullptr, kHotkeyNext);
    }
    if (prevOk) {
        UnregisterHotKey(nullptr, kHotkeyPrev);
    }

    return 0;
}

static void LoadSettings() {
    settings.includeMinimized = Wh_GetIntSetting(L"includeMinimized");
}

BOOL WhTool_ModInit() {
    Wh_Log(L"Init");

    LoadSettings();

    g_hotkeyThreadReady = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_hotkeyThreadReady) {
        return FALSE;
    }

    g_hotkeyThread = CreateThread(nullptr, 0, HotkeyThreadProc, nullptr, 0,
                                  &g_hotkeyThreadId);
    if (!g_hotkeyThread) {
        Wh_Log(L"CreateThread failed: %lu", GetLastError());
        CloseHandle(g_hotkeyThreadReady);
        g_hotkeyThreadReady = nullptr;
        return FALSE;
    }

    return TRUE;
}

void WhTool_ModUninit() {
    Wh_Log(L"Uninit");

    if (g_hotkeyThread) {
        // Don't post WM_QUIT before the thread has a message queue. Waiting on
        // the thread handle too covers the case of the thread exiting early.
        HANDLE handles[] = {g_hotkeyThreadReady, g_hotkeyThread};
        WaitForMultipleObjects(ARRAYSIZE(handles), handles, FALSE, INFINITE);
        PostThreadMessageW(g_hotkeyThreadId, WM_QUIT, 0, 0);

        // No timeout: the thread runs code from the mod image, so it must be
        // gone before the mod is unloaded.
        WaitForSingleObject(g_hotkeyThread, INFINITE);
        CloseHandle(g_hotkeyThread);
        g_hotkeyThread = nullptr;
    }

    if (g_hotkeyThreadReady) {
        CloseHandle(g_hotkeyThreadReady);
        g_hotkeyThreadReady = nullptr;
    }
}

void WhTool_ModSettingsChanged() {
    Wh_Log(L"SettingsChanged");

    LoadSettings();
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
