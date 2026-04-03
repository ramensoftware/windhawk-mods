// ==WindhawkMod==
// @id              taskbar-center-hover-only
// @name            Taskbar Center Hover Only
// @description     Taskbar auto-hide only triggers in the center zone of the screen. The Win key is suppressed. Requires taskbar auto-hide to be enabled in Windows settings.
// @version         1.0
// @author          Greyxp1
// @github          https://github.com/greyxp1
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lversion
// ==/WindhawkMod==

// ==WindhawkModSettings==
/*
- centerZonePercent: 20
  $name: Center zone width (%)
  $description: >-
    Width of the hover-active zone as a percentage of the monitor width,
    centered on screen. E.g. 20 means the middle 20% triggers the taskbar;
    the outer 40% on each side is a dead zone. Range: 10-100.
- winKeyMode: doublePress
  $name: Win key behavior
  $options:
  - normal: Normal (unmodified)
  - suppress: Suppress (do nothing)
  - doublePress: Open Start on double press only
- doublePressDurationMs: 250
  $name: Double press duration (ms)
  $description: >-
    Maximum time in milliseconds between two Win key presses to count as a
    double press. Only applies when Win key behavior is set to double press.
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <atomic>

// ─── Version ──────────────────────────────────────────────────────────────────

enum class WinVersion { Unsupported, Win10, Win11, Win11_24H2 };
WinVersion g_winVersion;

std::atomic<bool> g_taskbarViewDllLoaded;

// ─── Settings ─────────────────────────────────────────────────────────────────

int g_deadZonePct = 20;

enum class WinKeyMode { Normal, Suppress, DoublePress };
WinKeyMode g_winKeyMode = WinKeyMode::DoublePress;

DWORD g_doublePressDurationMs = 250;

void LoadSettings() {
    int center = Wh_GetIntSetting(L"centerZonePercent");
    if (center < 10)  center = 10;
    if (center > 100) center = 100;
    g_deadZonePct = (100 - center) / 2;

    PCWSTR mode = Wh_GetStringSetting(L"winKeyMode");
    if (wcscmp(mode, L"normal") == 0)        g_winKeyMode = WinKeyMode::Normal;
    else if (wcscmp(mode, L"suppress") == 0) g_winKeyMode = WinKeyMode::Suppress;
    else                                      g_winKeyMode = WinKeyMode::DoublePress;
    Wh_FreeStringSetting(mode);

    int ms = Wh_GetIntSetting(L"doublePressDurationMs");
    if (ms < 50)   ms = 50;
    if (ms > 2000) ms = 2000;
    g_doublePressDurationMs = (DWORD)ms;
}

// ─── Center-zone check ────────────────────────────────────────────────────────

bool IsMouseInCenterZone() {
    POINT pt;
    if (!GetCursorPos(&pt)) return false;
    HMONITOR mon = MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi = { sizeof(mi) };
    if (!GetMonitorInfo(mon, &mi)) return false;
    int w    = mi.rcMonitor.right - mi.rcMonitor.left;
    int dead = w * g_deadZonePct / 100;
    return pt.x >= mi.rcMonitor.left + dead && pt.x <= mi.rcMonitor.right - dead;
}

// ─── Taskbar window check ─────────────────────────────────────────────────────

bool IsTaskbarWindow(HWND hWnd) {
    WCHAR cls[32];
    return hWnd && GetClassName(hWnd, cls, 32) &&
           (_wcsicmp(cls, L"Shell_TrayWnd") == 0 ||
            _wcsicmp(cls, L"Shell_SecondaryTrayWnd") == 0);
}

// ─── SetTimer hook (old taskbar / Win10) ──────────────────────────────────────
// Windows fires kTimerUnhide when the mouse touches the screen edge.
// Block it when the cursor is outside the center zone.

enum { kTimerUnhide = 3 };

using SetTimer_t = decltype(&SetTimer);
SetTimer_t SetTimer_Original;

UINT_PTR WINAPI SetTimer_Hook(HWND hWnd, UINT_PTR id, UINT ms, TIMERPROC fn) {
    if (id == kTimerUnhide && IsTaskbarWindow(hWnd) && !IsMouseInCenterZone())
        return 1;
    return SetTimer_Original(hWnd, id, ms, fn);
}

// ─── ViewCoordinator hooks (Win11) ────────────────────────────────────────────
// Track pointer enter/leave so we only block the enter direction.

bool g_isPointerOverTaskbar = false;
constexpr int kReasonPointerOverFrame = 7;
constexpr int kReasonScreenEdgeStroke = 8;

using VC_HandlePointerOver_t = void(WINAPI*)(void*, HWND, bool, int);
VC_HandlePointerOver_t VC_HandlePointerOver_Original;

void WINAPI VC_HandlePointerOver_Hook(void* pThis, HWND hWnd, bool over, int kind) {
    g_isPointerOverTaskbar = over;
    VC_HandlePointerOver_Original(pThis, hWnd, over, kind);
    g_isPointerOverTaskbar = false;
}

using VC_UpdateIsExpanded_t = void(WINAPI*)(void*, HWND, int);
VC_UpdateIsExpanded_t VC_UpdateIsExpanded_Original;

void WINAPI VC_UpdateIsExpanded_Hook(void* pThis, HWND hWnd, int reason) {
    if ((reason == kReasonPointerOverFrame && g_isPointerOverTaskbar) ||
        reason == kReasonScreenEdgeStroke) {
        if (!IsMouseInCenterZone()) return;
    }
    VC_UpdateIsExpanded_Original(pThis, hWnd, reason);
}

// ─── Win key hook ─────────────────────────────────────────────────────────────
// method == 1 is the keyboard Win key; other values are Start button clicks
// and are always passed through unchanged.
//
// Double-press: first press is swallowed and the timestamp recorded. If a
// second press arrives within GetDoubleClickTime() ms the Start menu opens
// normally and the timestamp is cleared.

DWORD g_lastWinKeyTick = 0;

using XamlLauncher_ShowStartView_t = HRESULT(WINAPI*)(void*, int, int);
XamlLauncher_ShowStartView_t XamlLauncher_ShowStartView_Original;

HRESULT WINAPI XamlLauncher_ShowStartView_Hook(void* pThis, int method, int flags) {
    if (method != 1)
        return XamlLauncher_ShowStartView_Original(pThis, method, flags);

    switch (g_winKeyMode) {
        case WinKeyMode::Normal:
            return XamlLauncher_ShowStartView_Original(pThis, method, flags);

        case WinKeyMode::Suppress:
            return S_OK;

        case WinKeyMode::DoublePress: {
            DWORD now  = GetTickCount();
            DWORD last = g_lastWinKeyTick;
            if (last != 0 && (now - last) <= g_doublePressDurationMs) {
                g_lastWinKeyTick = 0;
                return XamlLauncher_ShowStartView_Original(pThis, method, flags);
            }
            g_lastWinKeyTick = now;
            return S_OK;
        }
    }
    return S_OK;
}

// ─── Symbol hooking ───────────────────────────────────────────────────────────

bool HookTaskbarViewDll(HMODULE module) {
    WindhawkUtils::SYMBOL_HOOK hooks[] = {
        {
            {LR"(public: void __cdecl winrt::Taskbar::implementation::ViewCoordinator::HandleIsPointerOverTaskbarFrameChanged(unsigned __int64,bool,enum winrt::WindowsUdk::UI::Shell::InputDeviceKind))"},
            &VC_HandlePointerOver_Original, VC_HandlePointerOver_Hook, true,
        },
        {
            {LR"(public: void __cdecl winrt::Taskbar::implementation::ViewCoordinator::UpdateIsExpanded(unsigned __int64,enum TaskbarTipTest::TaskbarExpandCollapseReason))"},
            &VC_UpdateIsExpanded_Original, VC_UpdateIsExpanded_Hook, true,
        },
    };
    return HookSymbols(module, hooks, ARRAYSIZE(hooks));
}

HMODULE GetTaskbarViewModule() {
    HMODULE m = GetModuleHandle(L"Taskbar.View.dll");
    return m ? m : GetModuleHandle(L"ExplorerExtensions.dll");
}

void OnTaskbarViewLoaded(HMODULE module, LPCWSTR name) {
    if (g_winVersion >= WinVersion::Win11 && !g_taskbarViewDllLoaded &&
        GetTaskbarViewModule() == module && !g_taskbarViewDllLoaded.exchange(true)) {
        Wh_Log(L"Loaded %s", name);
        if (HookTaskbarViewDll(module)) Wh_ApplyHookOperations();
    }
}

bool HookTwinuiPcshell() {
    HMODULE module = LoadLibraryEx(L"twinui.pcshell.dll", nullptr,
                                   LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!module) { Wh_Log(L"Couldn't load twinui.pcshell.dll"); return false; }

    WindhawkUtils::SYMBOL_HOOK hooks[] = {
        {
            {LR"(public: virtual long __cdecl XamlLauncher::ShowStartView(enum IMMERSIVELAUNCHERSHOWMETHOD,enum IMMERSIVELAUNCHERSHOWFLAGS))"},
            &XamlLauncher_ShowStartView_Original, XamlLauncher_ShowStartView_Hook,
        },
    };
    return HookSymbols(module, hooks, ARRAYSIZE(hooks));
}

// ─── Version detection ────────────────────────────────────────────────────────

WinVersion GetExplorerVersion() {
    HRSRC hRes = FindResource(nullptr, MAKEINTRESOURCE(VS_VERSION_INFO), RT_VERSION);
    if (!hRes) return WinVersion::Unsupported;
    void* pData = LockResource(LoadResource(nullptr, hRes));
    if (!pData) return WinVersion::Unsupported;
    VS_FIXEDFILEINFO* fi; UINT len;
    if (!VerQueryValue(pData, L"\\", (void**)&fi, &len) || !len)
        return WinVersion::Unsupported;
    WORD build = HIWORD(fi->dwFileVersionLS);
    if (HIWORD(fi->dwFileVersionMS) != 10) return WinVersion::Unsupported;
    if (build < 22000) return WinVersion::Win10;
    if (build < 26100) return WinVersion::Win11;
    return WinVersion::Win11_24H2;
}

using LoadLibraryExW_t = decltype(&LoadLibraryExW);
LoadLibraryExW_t LoadLibraryExW_Original;

HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR name, HANDLE file, DWORD flags) {
    HMODULE module = LoadLibraryExW_Original(name, file, flags);
    if (module) OnTaskbarViewLoaded(module, name);
    return module;
}

// ─── Mod lifecycle ────────────────────────────────────────────────────────────

BOOL Wh_ModInit() {
    Wh_Log(L">");

    LoadSettings();

    g_winVersion = GetExplorerVersion();
    if (g_winVersion == WinVersion::Unsupported) {
        Wh_Log(L"Unsupported Windows version");
        return FALSE;
    }

    if (g_winVersion >= WinVersion::Win11) {
        if (HMODULE m = GetTaskbarViewModule()) {
            g_taskbarViewDllLoaded = true;
            if (!HookTaskbarViewDll(m)) return FALSE;
        }
    }

    if (!HookTwinuiPcshell()) return FALSE;

    HMODULE kb = GetModuleHandle(L"kernelbase.dll");
    WindhawkUtils::SetFunctionHook(
        (decltype(&LoadLibraryExW))GetProcAddress(kb, "LoadLibraryExW"),
        LoadLibraryExW_Hook, &LoadLibraryExW_Original);
    WindhawkUtils::SetFunctionHook(SetTimer, SetTimer_Hook, &SetTimer_Original);

    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L">");

    if (g_winVersion >= WinVersion::Win11 && !g_taskbarViewDllLoaded) {
        if (HMODULE m = GetTaskbarViewModule())
            if (!g_taskbarViewDllLoaded.exchange(true))
                if (HookTaskbarViewDll(m)) Wh_ApplyHookOperations();
    }
}

void Wh_ModUninit() { Wh_Log(L">"); }

BOOL Wh_ModSettingsChanged(BOOL* bReload) {
    *bReload = FALSE;
    LoadSettings();
    return TRUE;
}
