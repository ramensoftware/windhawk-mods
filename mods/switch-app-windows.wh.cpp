// ==WindhawkMod==
// @id              switch-app-windows
// @name            Switch App Windows
// @description     Alt+` / Alt+Shift+` cycles windows of the foreground application without a popup.
// @version         1.0.0
// @author          asteski
// @include         explorer.exe
// @compilerOptions -lole32 -luuid -ldwmapi
// ==/WindhawkMod==

// ==WindhawkModSettings==
/*
- ignoreMinimized: false
  $name: Ignore minimized windows
  $description: Exclude minimized windows from cycling.
- sameVirtualDesktop: true
  $name: Only current virtual desktop
  $description: Cycle only windows on the current virtual desktop.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <dwmapi.h>
#include <shobjidl.h>
#include <string>
#include <vector>
#include <algorithm>
#include <cwctype>

struct Settings { bool ignoreMinimized; bool sameVirtualDesktop; };
static Settings g_settings{};
static HHOOK g_hook = nullptr;
static DWORD g_threadId = 0;
static HWND g_messageWindow = nullptr;
static constexpr UINT WM_CYCLE = WM_APP + 42;
static std::vector<HWND> g_cycle;
static std::wstring g_cycleApp;
static size_t g_index = 0;
static bool g_altDown = false;
static bool g_backtickDown = false;
static IVirtualDesktopManager* g_desktopManager = nullptr;

static void ResetCycle() { g_cycle.clear(); g_cycleApp.clear(); g_index = 0; }

static std::wstring ProcessPath(HWND hwnd) {
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    if (!pid) return {};
    HANDLE process = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
    if (!process) return {};
    wchar_t path[32768];
    DWORD size = ARRAYSIZE(path);
    bool ok = QueryFullProcessImageNameW(process, 0, path, &size) != FALSE;
    CloseHandle(process);
    if (!ok) return {};
    std::wstring result(path, size);
    std::transform(result.begin(), result.end(), result.begin(), towlower);
    return result;
}

// Explorer.exe hosts desktop/taskbar/shell surfaces as well as folder windows.
// Identify actual File Explorer frames by their window class, not their PID.
static bool IsExplorerPath(const std::wstring& path) {
    constexpr wchar_t suffix[] = L"\\explorer.exe";
    const size_t n = sizeof(suffix) / sizeof(suffix[0]) - 1;
    return path.size() >= n && path.compare(path.size() - n, n, suffix) == 0;
}

static bool IsFileExplorerWindow(HWND hwnd) {
    wchar_t className[256]{};
    if (!GetClassNameW(hwnd, className, ARRAYSIZE(className))) return false;
    return wcscmp(className, L"CabinetWClass") == 0 ||
           wcscmp(className, L"ExploreWClass") == 0;
}

static bool Eligible(HWND hwnd) {
    if (!IsWindow(hwnd) || !IsWindowVisible(hwnd)) return false;
    if (g_settings.ignoreMinimized && IsIconic(hwnd)) return false;
    LONG_PTR ex = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);
    if (ex & WS_EX_TOOLWINDOW) return false;
    // Keep normal top-level windows, including independently displayed owned windows.
    if (GetWindow(hwnd, GW_OWNER) && !(ex & WS_EX_APPWINDOW)) return false;
    DWORD cloaked = 0;
    if (SUCCEEDED(DwmGetWindowAttribute(hwnd, DWMWA_CLOAKED, &cloaked, sizeof(cloaked))) && cloaked) return false;
    if (g_settings.sameVirtualDesktop && g_desktopManager) {
        BOOL onDesktop = FALSE;
        if (SUCCEEDED(g_desktopManager->IsWindowOnCurrentVirtualDesktop(hwnd, &onDesktop)) && !onDesktop) return false;
    }
    return true;
}

struct EnumerateContext { std::wstring app; bool explorerOnly; std::vector<HWND>* windows; };
static BOOL CALLBACK Enumerate(HWND hwnd, LPARAM data) {
    auto* ctx = reinterpret_cast<EnumerateContext*>(data);
    if (Eligible(hwnd) && (!ctx->explorerOnly || IsFileExplorerWindow(hwnd)) &&
        ProcessPath(hwnd) == ctx->app) ctx->windows->push_back(hwnd);
    return TRUE;
}

static void Cycle(bool backwards) {
    HWND foreground = GetForegroundWindow();
    if (!foreground) return;
    std::wstring app = ProcessPath(foreground);
    if (app.empty()) return;
    // Only use the Explorer-specific filter when cycling from a folder window.
    const bool explorerOnly = IsExplorerPath(app) && IsFileExplorerWindow(foreground);
    if (IsExplorerPath(app) && !explorerOnly) return;
    std::vector<HWND> available;
    EnumerateContext ctx{app, explorerOnly, &available};
    EnumWindows(Enumerate, reinterpret_cast<LPARAM>(&ctx));
    if (available.size() < 2) { ResetCycle(); return; }

    bool reuse = !g_cycle.empty() && g_cycleApp == app;
    if (reuse) {
        std::vector<HWND> retained;
        for (HWND hwnd : g_cycle)
            if (std::find(available.begin(), available.end(), hwnd) != available.end()) retained.push_back(hwnd);
        for (HWND hwnd : available)
            if (std::find(retained.begin(), retained.end(), hwnd) == retained.end()) retained.push_back(hwnd);
        HWND previous = g_cycle[g_index];
        g_cycle = std::move(retained);
        auto it = std::find(g_cycle.begin(), g_cycle.end(), previous);
        if (it == g_cycle.end()) reuse = false;
        else g_index = static_cast<size_t>(it - g_cycle.begin());
    }
    if (!reuse) {
        g_cycle = std::move(available);
        g_cycleApp = app;
        auto it = std::find(g_cycle.begin(), g_cycle.end(), foreground);
        g_index = it == g_cycle.end() ? 0 : static_cast<size_t>(it - g_cycle.begin());
    }
    g_index = backwards ? (g_index + g_cycle.size() - 1) % g_cycle.size()
                        : (g_index + 1) % g_cycle.size();
    HWND target = g_cycle[g_index];
    if (IsIconic(target)) ShowWindow(target, SW_RESTORE);
    SetForegroundWindow(target);
}

static LRESULT CALLBACK MessageProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    if (msg == WM_CYCLE) { Cycle(wp != 0); return 0; }
    return DefWindowProcW(hwnd, msg, wp, lp);
}

static LRESULT CALLBACK KeyboardProc(int code, WPARAM wp, LPARAM lp) {
    if (code == HC_ACTION) {
        const auto* k = reinterpret_cast<const KBDLLHOOKSTRUCT*>(lp);
        bool down = wp == WM_KEYDOWN || wp == WM_SYSKEYDOWN;
        bool up = wp == WM_KEYUP || wp == WM_SYSKEYUP;
        if (k->vkCode == VK_MENU || k->vkCode == VK_LMENU || k->vkCode == VK_RMENU) {
            if (up) { g_altDown = false; g_backtickDown = false; ResetCycle(); }
            if (down) g_altDown = true;
        }
        // VK_OEM_3 is the US-layout backtick/tilde key; scan code 0x29 covers its physical position.
        bool backtick = k->vkCode == VK_OEM_3 || k->scanCode == 0x29;
        if (backtick && (g_altDown || (GetAsyncKeyState(VK_MENU) & 0x8000))) {
            if (down && !g_backtickDown) {
                g_backtickDown = true;
                bool backwards = (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0;
                PostMessageW(g_messageWindow, WM_CYCLE, backwards, 0);
            }
            if (up) g_backtickDown = false;
            return 1;
        }
        if (backtick && up) g_backtickDown = false;
    }
    return CallNextHookEx(g_hook, code, wp, lp);
}

static DWORD WINAPI HookThread(LPVOID) {
    g_threadId = GetCurrentThreadId();
    CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    if (g_settings.sameVirtualDesktop)
        CoCreateInstance(CLSID_VirtualDesktopManager, nullptr, CLSCTX_INPROC_SERVER,
                         IID_IVirtualDesktopManager, reinterpret_cast<void**>(&g_desktopManager));
    const wchar_t* className = L"WindhawkSameAppWindowCycler";
    WNDCLASSW wc{};
    wc.lpfnWndProc = MessageProc;
    wc.hInstance = GetModuleHandleW(nullptr);
    wc.lpszClassName = className;
    RegisterClassW(&wc);
    g_messageWindow = CreateWindowExW(0, className, L"", 0, 0, 0, 0, 0, HWND_MESSAGE, nullptr, wc.hInstance, nullptr);
    g_hook = SetWindowsHookExW(WH_KEYBOARD_LL, KeyboardProc, GetModuleHandleW(nullptr), 0);
    MSG msg;
    while (GetMessageW(&msg, nullptr, 0, 0) > 0) { TranslateMessage(&msg); DispatchMessageW(&msg); }
    if (g_hook) { UnhookWindowsHookEx(g_hook); g_hook = nullptr; }
    if (g_messageWindow) { DestroyWindow(g_messageWindow); g_messageWindow = nullptr; }
    if (g_desktopManager) { g_desktopManager->Release(); g_desktopManager = nullptr; }
    UnregisterClassW(className, wc.hInstance);
    CoUninitialize();
    return 0;
}

static HANDLE g_worker = nullptr;
void Wh_ModSettingsChanged() {
    g_settings.ignoreMinimized = Wh_GetIntSetting(L"ignoreMinimized") != 0;
    g_settings.sameVirtualDesktop = Wh_GetIntSetting(L"sameVirtualDesktop") != 0;
    // Reinitialize the desktop manager when the setting changes.
    if (g_threadId) PostThreadMessageW(g_threadId, WM_QUIT, 0, 0);
    if (g_worker) { WaitForSingleObject(g_worker, INFINITE); CloseHandle(g_worker); }
    g_worker = CreateThread(nullptr, 0, HookThread, nullptr, 0, nullptr);
}
BOOL Wh_ModInit() {
    Wh_ModSettingsChanged();
    return g_worker != nullptr;
}
void Wh_ModUninit() {
    if (g_threadId) PostThreadMessageW(g_threadId, WM_QUIT, 0, 0);
    if (g_worker) { WaitForSingleObject(g_worker, INFINITE); CloseHandle(g_worker); g_worker = nullptr; }
}
