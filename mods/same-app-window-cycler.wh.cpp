// ==WindhawkMod==
// @id              same-app-window-cycler
// @name            Same-App Window Cycler (Alt+`)
// @description     macOS-style Cmd+` for Windows: cycle through the windows of the current app with Alt+` (Alt+Shift+` goes backwards)
// @version         0.1.0
// @author          mdozkanji
// @github          https://github.com/mdozkanji
// @include         explorer.exe
// @compilerOptions -ldwmapi
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Same-App Window Cycler

Brings the macOS **Cmd+`` ` ``** behavior to Windows. While an app has several
windows open, press **Alt+`` ` ``** to jump to its next window, or **Alt+Shift+`` ` ``**
to go to the previous one. Alt+Tab is unchanged.

The shortcut uses the physical key left of `1`, so it works on non-US keyboard
layouts too.

## How it works
The mod runs inside `explorer.exe` and registers a global hotkey. When pressed,
it takes the foreground window, finds all other switchable top-level windows
that belong to the same executable on the current virtual desktop, and
activates the next one.

## Limitations
- Windows are grouped by executable path. UWP apps hosted by
  ApplicationFrameHost.exe are all treated as one "app".
- Windows of elevated (administrator) apps may only flash instead of coming to
  the front.
- If Explorer is set to open folder windows in separate processes, only the
  first process will own the hotkey.
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

#include <string>
#include <vector>

struct {
    bool includeMinimized;
} settings;

HANDLE g_hotkeyThread = nullptr;
DWORD g_hotkeyThreadId = 0;

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
        ShowWindow(hwnd, SW_RESTORE);
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
    // Backward: go back to the one that was just behind the current window.
    HWND target = backwards ? ctx.windows[1] : ctx.windows.back();

    Wh_Log(L"Cycling %zu windows of %s", ctx.windows.size(),
           ctx.exePath.c_str());
    ActivateWindow(target);
}

static DWORD WINAPI HotkeyThreadProc(LPVOID) {
    // Make sure this thread has a message queue before anyone posts to it.
    MSG msg;
    PeekMessageW(&msg, nullptr, WM_USER, WM_USER, PM_NOREMOVE);

    // Physical key to the left of "1", regardless of keyboard layout.
    UINT vk = MapVirtualKeyW(0x29, MAPVK_VSC_TO_VK);
    if (!vk) {
        vk = VK_OEM_3;
    }

    bool nextOk = RegisterHotKey(nullptr, kHotkeyNext,
                                 MOD_ALT | MOD_NOREPEAT, vk);
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

BOOL Wh_ModInit() {
    Wh_Log(L"Init");

    LoadSettings();

    g_hotkeyThread = CreateThread(nullptr, 0, HotkeyThreadProc, nullptr, 0,
                                  &g_hotkeyThreadId);
    if (!g_hotkeyThread) {
        Wh_Log(L"CreateThread failed: %lu", GetLastError());
        return FALSE;
    }

    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L"Uninit");

    if (g_hotkeyThread) {
        PostThreadMessageW(g_hotkeyThreadId, WM_QUIT, 0, 0);
        WaitForSingleObject(g_hotkeyThread, 2000);
        CloseHandle(g_hotkeyThread);
        g_hotkeyThread = nullptr;
    }
}

void Wh_ModSettingsChanged() {
    Wh_Log(L"SettingsChanged");

    LoadSettings();
}
