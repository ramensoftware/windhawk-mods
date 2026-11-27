// ==WindhawkMod==
// @id              windhawk-transparent
// @name            Transparent Windhawk
// @description     Makes the Windhawk window itself semi-transparent, with an adjustable opacity
// @version         1.0.0
// @author          MmaxLordofSheep and Claude
// @include         VSCodium.exe
// @include         windhawk.exe
// @compilerOptions -luser32
// @github https://github.com/MmaxLordofSheep
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Transparent Windhawk

Makes the Windhawk window semi-transparent. Set the opacity in this mod's
settings (100 = fully opaque, limited to 10-100 so the window can never
disappear).

## Notes

* Windhawk's window is hosted by `VSCodium.exe` (`windhawk.exe` is also targeted
  in case your version uses that instead). If you also use VSCodium as an
  editor, it will become transparent too.
* Only visible windows of at least 200x100 pixels are touched; tooltips, menus
  and hidden helper windows are ignored.
* Changing the setting or disabling the mod updates / restores the window live.
* If you also run the multi-app *Transparent Apps* mod, disable one of the two.
  Whichever loads first wins, and the other leaves the window alone.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- opacity: 80
  $name: Opacity (%)
  $description: 100 = fully opaque, lower = more transparent (10-100)
*/
// ==/WindhawkModSettings==

#include <windows.h>

#include <atomic>
#include <mutex>
#include <unordered_set>

static std::atomic<int> g_opacity{100};
static std::atomic<bool> g_unloading{false};

static std::mutex g_mutex;
static std::unordered_set<HWND> g_modified;  // windows we made layered
static std::unordered_set<HWND> g_failed;    // windows where it didn't work

static HANDLE g_stopEvent = nullptr;
static HANDLE g_pollThread = nullptr;

static int ReadOpacity() {
    int value = Wh_GetIntSetting(L"opacity");
    if (value < 10) value = 10;
    if (value > 100) value = 100;
    return value;
}

// Only real, visible top-level app windows: no child windows, message-only
// windows, tooltips, menus, IME helpers or tiny helper windows.
static bool IsEligible(HWND hwnd) {
    if (!IsWindow(hwnd)) {
        return false;
    }
    if (GetAncestor(hwnd, GA_PARENT) != GetDesktopWindow()) {
        return false;
    }
    if (!IsWindowVisible(hwnd)) {
        return false;
    }

    LONG_PTR ex = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);
    if (ex & (WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE)) {
        return false;
    }

    RECT rc;
    if (!GetWindowRect(hwnd, &rc) || rc.right - rc.left < 200 ||
        rc.bottom - rc.top < 100) {
        return false;
    }

    WCHAR cls[128];
    if (!GetClassNameW(hwnd, cls, ARRAYSIZE(cls))) {
        return false;
    }
    static const PCWSTR kSkipClasses[] = {
        L"#32768", L"tooltips_class32", L"IME", L"MSCTFIME UI",
        L"SysShadow", L"CiceroUIWndFrame",
    };
    for (PCWSTR skip : kSkipClasses) {
        if (_wcsicmp(cls, skip) == 0) {
            return false;
        }
    }
    return true;
}

static void Restore(HWND hwnd) {
    {
        std::lock_guard<std::mutex> lock(g_mutex);
        g_failed.erase(hwnd);
        if (!g_modified.erase(hwnd)) {
            return;  // not ours
        }
    }
    if (!IsWindow(hwnd)) {
        return;
    }

    LONG_PTR ex = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);
    SetWindowLongPtrW(hwnd, GWL_EXSTYLE, ex & ~(LONG_PTR)WS_EX_LAYERED);
    RedrawWindow(hwnd, nullptr, nullptr,
                 RDW_INVALIDATE | RDW_ERASE | RDW_FRAME | RDW_ALLCHILDREN);
}

static void Apply(HWND hwnd) {
    const int percent = g_opacity;
    if (percent >= 100) {
        Restore(hwnd);
        return;
    }

    {
        std::lock_guard<std::mutex> lock(g_mutex);
        if (g_failed.count(hwnd)) {
            return;  // already tried and it didn't work
        }
    }

    if (!IsEligible(hwnd)) {
        return;
    }

    LONG_PTR ex = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);
    const BYTE alpha = (BYTE)(percent * 255 / 100);

    bool ours;
    {
        std::lock_guard<std::mutex> lock(g_mutex);
        ours = g_modified.count(hwnd) > 0;
    }

    if (ex & WS_EX_LAYERED) {
        if (!ours) {
            return;  // the app manages its own layering, don't fight it
        }
        BYTE current = 0;
        DWORD flags = 0;
        if (GetLayeredWindowAttributes(hwnd, nullptr, &current, &flags) &&
            (flags & LWA_ALPHA) && current == alpha) {
            return;  // already correct
        }
    }

    // Track first, so a failure below can always be rolled back. A layered
    // window with no alpha set would otherwise stay invisible.
    {
        std::lock_guard<std::mutex> lock(g_mutex);
        g_modified.insert(hwnd);
    }

    if (!(ex & WS_EX_LAYERED)) {
        SetWindowLongPtrW(hwnd, GWL_EXSTYLE, ex | WS_EX_LAYERED);
    }

    if (!SetLayeredWindowAttributes(hwnd, 0, alpha, LWA_ALPHA)) {
        Wh_Log(L"SetLayeredWindowAttributes failed (error %lu), reverting",
               GetLastError());
        Restore(hwnd);
        std::lock_guard<std::mutex> lock(g_mutex);
        g_failed.insert(hwnd);
    }
}

static BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM) {
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    if (pid == GetCurrentProcessId()) {
        Apply(hwnd);
    }
    return TRUE;
}

// Forget handles of windows that no longer exist.
static void PruneDeadWindows() {
    std::lock_guard<std::mutex> lock(g_mutex);
    for (auto it = g_modified.begin(); it != g_modified.end();) {
        it = IsWindow(*it) ? std::next(it) : g_modified.erase(it);
    }
    for (auto it = g_failed.begin(); it != g_failed.end();) {
        it = IsWindow(*it) ? std::next(it) : g_failed.erase(it);
    }
}

// Sweeps the process's windows twice a second. This catches windows created
// hidden and shown later, and apps that reset their own styles.
static DWORD WINAPI PollThread(LPVOID) {
    while (WaitForSingleObject(g_stopEvent, 500) == WAIT_TIMEOUT) {
        EnumWindows(EnumWindowsProc, 0);
        PruneDeadWindows();
    }
    return 0;
}

// Applies instantly when a window is shown, so there is no opaque flash.
using ShowWindow_t = decltype(&ShowWindow);
static ShowWindow_t ShowWindow_Original;

static BOOL WINAPI ShowWindow_Hook(HWND hWnd, int nCmdShow) {
    BOOL ret = ShowWindow_Original(hWnd, nCmdShow);
    if (!g_unloading && nCmdShow != SW_HIDE) {
        Apply(hWnd);
    }
    return ret;
}

BOOL Wh_ModInit() {
    Wh_Log(L"Init");

    g_opacity = ReadOpacity();

    Wh_SetFunctionHook((void*)ShowWindow, (void*)ShowWindow_Hook,
                       (void**)&ShowWindow_Original);
    return TRUE;
}

void Wh_ModAfterInit() {
    EnumWindows(EnumWindowsProc, 0);

    g_stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (g_stopEvent) {
        g_pollThread = CreateThread(nullptr, 0, PollThread, nullptr, 0, nullptr);
    }
}

void Wh_ModSettingsChanged() {
    Wh_Log(L"Settings changed");

    g_opacity = ReadOpacity();
    {
        std::lock_guard<std::mutex> lock(g_mutex);
        g_failed.clear();
    }
    EnumWindows(EnumWindowsProc, 0);
}

void Wh_ModUninit() {
    Wh_Log(L"Uninit");
    g_unloading = true;

    bool pollDone = true;
    if (g_pollThread) {
        SetEvent(g_stopEvent);
        pollDone = WaitForSingleObject(g_pollThread, 3000) == WAIT_OBJECT_0;
        if (pollDone) {
            CloseHandle(g_pollThread);
        }
        g_pollThread = nullptr;
    }
    if (g_stopEvent && pollDone) {
        CloseHandle(g_stopEvent);
    }
    g_stopEvent = nullptr;

    std::unordered_set<HWND> toRestore;
    {
        std::lock_guard<std::mutex> lock(g_mutex);
        toRestore = g_modified;
    }
    for (HWND hwnd : toRestore) {
        Restore(hwnd);
    }
}