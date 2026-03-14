// ==WindhawkMod==
// @id              alt-tab-per-monitor
// @name            Alt+Tab per monitor
// @description     Pressing Alt+Tab shows all open windows on the primary display. This mod shows only the windows on the monitor where the cursor is.
// @version         2.0.0
// @author          L3r0y (https://github.com/L3r0yThingz) & trosha_b
// @github          https://github.com/troshab
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lversion
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Alt+Tab per monitor v2

Rewritten version of [Alt+Tab per monitor](https://windhawk.net/mods/alt-tab-per-monitor)
by [L3r0y](https://github.com/L3r0yThingz).

## Changes in v2.0.0
- **State machine** instead of fragile timing hack (hardcoded 200ms threshold)
- **RAII PhaseGuard** for safe phase transitions (no leaked flags on error paths)
- **SRWLOCK** synchronization instead of 3 unsynchronized atomic variables
- **Wh_ModUninit** with hook call counter for safe unloading
- **Configurable threshold** (`postCreateThresholdMs`) in mod settings
- **Deduplicated** common symbol hooks between Win10/Win11 paths
- Removed per-window `Wh_Log` spam from IsViewVisible

## How it works
When you press Alt+Tab, the window switcher appears on the monitor where the
cursor is located, showing only the windows on that monitor.

Win+Alt+Tab still shows all windows across all monitors. You can configure
where the Win+Alt+Tab UI appears in the mod settings.

![Gif](https://i.imgur.com/Hpg8TKh.gif)
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- winAltTabLocation: primary
  $name: Win+Alt+Tab display location
  $description: |
    Choose where the Alt+Tab switcher appears when using Win+Alt+Tab (which
    shows all windows from all monitors)
  $options:
  - primary: Primary monitor (default Windows behavior)
  - cursor: Monitor where cursor is located
- postCreateThresholdMs: 200
  $name: Post-creation filter window (ms)
  $description: |
    After the Alt+Tab window list is built, additional window visibility checks
    may occur for the focused window. This controls how long after creation
    these checks are still filtered by monitor. Increase if windows from other
    monitors leak through. Decrease if non-Alt+Tab components are affected.
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <atomic>
#include <functional>

#include <winrt/windows.foundation.collections.h>

// ============================================================================
// Settings
// ============================================================================

enum class WinAltTabLocation {
    primary,
    cursor,
};

struct {
    WinAltTabLocation winAltTabLocation;
    ULONGLONG postCreateThresholdMs;
} g_settings;

// ============================================================================
// Version detection
// ============================================================================

enum class WinVersion {
    Unsupported,
    Win10,
    Win11,
};

WinVersion g_winVersion;

VS_FIXEDFILEINFO* GetModuleVersionInfo(HMODULE hModule, UINT* puPtrLen) {
    void* pFixedFileInfo = nullptr;
    UINT uPtrLen = 0;

    HRSRC hResource =
        FindResource(hModule, MAKEINTRESOURCE(VS_VERSION_INFO), RT_VERSION);
    if (hResource) {
        HGLOBAL hGlobal = LoadResource(hModule, hResource);
        if (hGlobal) {
            void* pData = LockResource(hGlobal);
            if (pData) {
                if (!VerQueryValue(pData, L"\\", &pFixedFileInfo, &uPtrLen) ||
                    uPtrLen == 0) {
                    pFixedFileInfo = nullptr;
                    uPtrLen = 0;
                }
            }
        }
    }

    if (puPtrLen) {
        *puPtrLen = uPtrLen;
    }

    return (VS_FIXEDFILEINFO*)pFixedFileInfo;
}

WinVersion GetWindowsVersion() {
    VS_FIXEDFILEINFO* fixedFileInfo = GetModuleVersionInfo(nullptr, nullptr);
    if (!fixedFileInfo) {
        return WinVersion::Unsupported;
    }

    WORD major = HIWORD(fixedFileInfo->dwFileVersionMS);
    WORD minor = LOWORD(fixedFileInfo->dwFileVersionMS);
    WORD build = HIWORD(fixedFileInfo->dwFileVersionLS);
    WORD qfe = LOWORD(fixedFileInfo->dwFileVersionLS);

    Wh_Log(L"Version: %u.%u.%u.%u", major, minor, build, qfe);

    switch (major) {
        case 10:
            if (build < 22000) {
                return WinVersion::Win10;
            } else {
                return WinVersion::Win11;
            }
            break;
    }

    return WinVersion::Unsupported;
}

// ============================================================================
// Alt+Tab session state machine
//
// Lifecycle:
//   [Inactive] --CreateInstance--> [Creating] --return--> [PostCreate]
//        ^                                                    |
//        |                                         Show called|
//        |                                                    v
//        +------------------Show returns------------- [Showing]
//
// IsViewVisible filters windows only during Creating and PostCreate phases.
// PostCreate has a configurable timeout (postCreateThresholdMs) to catch
// late IsViewVisible calls for the focused window that Windows adds after
// the initial window list is built.
// ============================================================================

enum class AltTabPhase {
    Inactive,
    Creating,    // Inside CreateInstance - building window list
    PostCreate,  // CreateInstance returned, may still get late visibility checks
    Showing,     // Inside Show - positioning the Alt+Tab window
};

struct AltTabSession {
    SRWLOCK lock = SRWLOCK_INIT;
    DWORD threadId = 0;
    AltTabPhase phase = AltTabPhase::Inactive;
    ULONGLONG createReturnedAt = 0;
};

AltTabSession g_session;

// RAII guard for phase transitions. Sets enterPhase on construction,
// exitPhase on destruction. Records tick when transitioning to PostCreate.
class PhaseGuard {
    AltTabSession& m_session;
    AltTabPhase m_exitPhase;

public:
    PhaseGuard(AltTabSession& session, AltTabPhase enterPhase,
               AltTabPhase exitPhase)
        : m_session(session), m_exitPhase(exitPhase) {
        AcquireSRWLockExclusive(&m_session.lock);
        m_session.threadId = GetCurrentThreadId();
        m_session.phase = enterPhase;
        ReleaseSRWLockExclusive(&m_session.lock);
    }

    ~PhaseGuard() {
        AcquireSRWLockExclusive(&m_session.lock);
        m_session.phase = m_exitPhase;
        if (m_exitPhase == AltTabPhase::PostCreate) {
            m_session.createReturnedAt = GetTickCount64();
        } else if (m_exitPhase == AltTabPhase::Inactive) {
            m_session.threadId = 0;
        }
        ReleaseSRWLockExclusive(&m_session.lock);
    }

    PhaseGuard(const PhaseGuard&) = delete;
    PhaseGuard& operator=(const PhaseGuard&) = delete;
};

// ============================================================================
// Unload safety
// ============================================================================

std::atomic<bool> g_unloading{false};
std::atomic<int> g_hookCallCounter{0};

struct HookCallCounterGuard {
    HookCallCounterGuard() { g_hookCallCounter++; }
    ~HookCallCounterGuard() { g_hookCallCounter--; }
    HookCallCounterGuard(const HookCallCounterGuard&) = delete;
    HookCallCounterGuard& operator=(const HookCallCounterGuard&) = delete;
};

// ============================================================================
// Utility functions
// ============================================================================

bool IsWinKeyPressed() {
    return GetAsyncKeyState(VK_LWIN) < 0 || GetAsyncKeyState(VK_RWIN) < 0;
}

bool HandleAltTabWindow(RECT* rect) {
    if (g_settings.winAltTabLocation == WinAltTabLocation::primary &&
        IsWinKeyPressed()) {
        return false;
    }

    POINT pt;
    if (!GetCursorPos(&pt)) {
        return false;
    }

    auto hMon = MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);

    MONITORINFO monInfo;
    monInfo.cbSize = sizeof(MONITORINFO);
    if (!GetMonitorInfo(hMon, &monInfo)) {
        return false;
    }

    *rect = monInfo.rcWork;
    return true;
}

// ============================================================================
// ApplicationView helpers
// ============================================================================

void* CWin32ApplicationView_vtable;
void* CWinRTApplicationView_vtable;

using CWin32ApplicationView_v_GetNativeWindow_t =
    HRESULT(WINAPI*)(void* pThis, HWND* windowHandle);
CWin32ApplicationView_v_GetNativeWindow_t
    CWin32ApplicationView_v_GetNativeWindow;

using CWinRTApplicationView_v_GetNativeWindow_t =
    HRESULT(WINAPI*)(void* pThis, HWND* windowHandle);
CWinRTApplicationView_v_GetNativeWindow_t
    CWinRTApplicationView_v_GetNativeWindow;

HRESULT GetWindowHandleFromApplicationView(void* applicationView,
                                           HWND* windowHandle) {
    *windowHandle = nullptr;
    void* vtable = *(void**)applicationView;
    HRESULT hr = E_FAIL;

    if (vtable == CWin32ApplicationView_vtable) {
        hr = CWin32ApplicationView_v_GetNativeWindow(applicationView,
                                                     windowHandle);
    } else if (vtable == CWinRTApplicationView_vtable) {
        hr = CWinRTApplicationView_v_GetNativeWindow(applicationView,
                                                     windowHandle);
    }

    return hr;
}

bool IsWindowOnCursorMonitor(HWND windowHandle) {
    POINT pt;
    if (!GetCursorPos(&pt)) {
        return false;
    }

    auto hMon = MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
    auto hMonFromWindow =
        MonitorFromWindow(windowHandle, MONITOR_DEFAULTTONEAREST);

    return hMon == hMonFromWindow;
}

// ============================================================================
// Hook: CVirtualDesktop::IsViewVisible
//
// Called for every window to check visibility. Also used by Win+Tab and
// the taskbar, so we only filter during our Alt+Tab session phases.
// ============================================================================

using CVirtualDesktop_IsViewVisible_t = HRESULT(WINAPI*)(void* pThis,
                                                         void* applicationView,
                                                         BOOL* isVisible);
CVirtualDesktop_IsViewVisible_t CVirtualDesktop_IsViewVisible_Original;
HRESULT WINAPI CVirtualDesktop_IsViewVisible_Hook(void* pThis,
                                                  void* applicationView,
                                                  BOOL* isVisible) {
    HookCallCounterGuard counter;
    if (g_unloading) {
        return CVirtualDesktop_IsViewVisible_Original(pThis, applicationView,
                                                      isVisible);
    }

    auto ret = CVirtualDesktop_IsViewVisible_Original(pThis, applicationView,
                                                      isVisible);
    if (FAILED(ret) || !*isVisible) {
        return ret;
    }

    // Check if this call belongs to our Alt+Tab session
    bool shouldFilter = false;
    {
        AcquireSRWLockShared(&g_session.lock);
        DWORD currentThread = GetCurrentThreadId();
        if (g_session.threadId == currentThread) {
            switch (g_session.phase) {
                case AltTabPhase::Creating:
                    shouldFilter = true;
                    break;
                case AltTabPhase::PostCreate:
                    if ((GetTickCount64() - g_session.createReturnedAt) <=
                        g_settings.postCreateThresholdMs) {
                        shouldFilter = true;
                    }
                    break;
                default:
                    break;
            }
        }
        ReleaseSRWLockShared(&g_session.lock);
    }

    if (!shouldFilter) {
        return ret;
    }

    HWND windowHandle;
    HRESULT hr =
        GetWindowHandleFromApplicationView(applicationView, &windowHandle);

    if (FAILED(hr) || !windowHandle) {
        return ret;
    }

    if (!IsWindowOnCursorMonitor(windowHandle)) {
        *isVisible = FALSE;
    }

    return ret;
}

// ============================================================================
// Hook: Show (XamlAltTabViewHost / CAltTabViewHost)
//
// Called when Alt+Tab window is being displayed. PhaseGuard sets Showing
// on entry and Inactive on exit, so Position/CreateFrame can detect they're
// being called from within Show.
// ============================================================================

using XamlAltTabViewHost_Show_t = HRESULT(WINAPI*)(void* pThis,
                                                   void* param1,
                                                   int param2,
                                                   void* param3);
XamlAltTabViewHost_Show_t XamlAltTabViewHost_Show_Original;
HRESULT WINAPI XamlAltTabViewHost_Show_Hook(void* pThis,
                                            void* param1,
                                            int param2,
                                            void* param3) {
    HookCallCounterGuard counter;
    if (g_unloading) {
        return XamlAltTabViewHost_Show_Original(pThis, param1, param2, param3);
    }

    Wh_Log(L">");
    PhaseGuard guard(g_session, AltTabPhase::Showing, AltTabPhase::Inactive);
    return XamlAltTabViewHost_Show_Original(pThis, param1, param2, param3);
}

using CAltTabViewHost_Show_t = HRESULT(WINAPI*)(void* pThis,
                                                void* param1,
                                                int param2,
                                                void* param3);
CAltTabViewHost_Show_t CAltTabViewHost_Show_Original;
HRESULT WINAPI CAltTabViewHost_Show_Hook(void* pThis,
                                         void* param1,
                                         int param2,
                                         void* param3) {
    HookCallCounterGuard counter;
    if (g_unloading) {
        return CAltTabViewHost_Show_Original(pThis, param1, param2, param3);
    }

    Wh_Log(L">");
    PhaseGuard guard(g_session, AltTabPhase::Showing, AltTabPhase::Inactive);
    return CAltTabViewHost_Show_Original(pThis, param1, param2, param3);
}

// ============================================================================
// Hook: Position / CreateFrame
//
// Called from within Show to position the Alt+Tab window on a monitor.
// Reads phase but does NOT clear it - PhaseGuard in Show handles cleanup.
// ============================================================================

using ITaskGroupWindowInformation_Position_t =
    HRESULT(WINAPI*)(void* pThis, winrt::Windows::Foundation::Rect* rect);
ITaskGroupWindowInformation_Position_t
    ITaskGroupWindowInformation_Position_Original;
HRESULT WINAPI ITaskGroupWindowInformation_Position_Hook(
    void* pThis,
    winrt::Windows::Foundation::Rect* rect) {
    HookCallCounterGuard counter;

    bool isOurCall = false;
    {
        AcquireSRWLockShared(&g_session.lock);
        isOurCall = (g_session.phase == AltTabPhase::Showing &&
                     g_session.threadId == GetCurrentThreadId());
        ReleaseSRWLockShared(&g_session.lock);
    }

    if (!isOurCall || g_unloading) {
        return ITaskGroupWindowInformation_Position_Original(pThis, rect);
    }

    RECT newRectNative;
    if (!HandleAltTabWindow(&newRectNative)) {
        return ITaskGroupWindowInformation_Position_Original(pThis, rect);
    }

    winrt::Windows::Foundation::Rect newRect{
        static_cast<float>(newRectNative.left),
        static_cast<float>(newRectNative.top),
        static_cast<float>(newRectNative.right - newRectNative.left),
        static_cast<float>(newRectNative.bottom - newRectNative.top),
    };

    return ITaskGroupWindowInformation_Position_Original(pThis, &newRect);
}

using CMultitaskingViewFrame_CreateFrame_t = HRESULT(WINAPI*)(void* pThis,
                                                              RECT* rect,
                                                              void* param2);
CMultitaskingViewFrame_CreateFrame_t
    CMultitaskingViewFrame_CreateFrame_Original;
HRESULT WINAPI CMultitaskingViewFrame_CreateFrame_Hook(void* pThis,
                                                       RECT* rect,
                                                       void* param2) {
    HookCallCounterGuard counter;

    bool isOurCall = false;
    {
        AcquireSRWLockShared(&g_session.lock);
        isOurCall = (g_session.phase == AltTabPhase::Showing &&
                     g_session.threadId == GetCurrentThreadId());
        ReleaseSRWLockShared(&g_session.lock);
    }

    if (!isOurCall || g_unloading) {
        return CMultitaskingViewFrame_CreateFrame_Original(pThis, rect, param2);
    }

    RECT newRect;
    if (!HandleAltTabWindow(&newRect)) {
        return CMultitaskingViewFrame_CreateFrame_Original(pThis, rect, param2);
    }

    return CMultitaskingViewFrame_CreateFrame_Original(pThis, &newRect, param2);
}

// ============================================================================
// Hook: CreateInstance
//
// Called when Alt+Tab session starts. PhaseGuard transitions:
//   Creating (during original call) -> PostCreate (after return)
// PostCreate allows late IsViewVisible calls to still be filtered
// within the configured threshold.
// ============================================================================

HRESULT CreateInstanceHook(std::function<HRESULT()> original) {
    HookCallCounterGuard counter;

    if (g_unloading || IsWinKeyPressed()) {
        return original();
    }

    PhaseGuard guard(g_session, AltTabPhase::Creating, AltTabPhase::PostCreate);
    return original();
}

using XamlAltTabViewHost_CreateInstance_t = HRESULT(WINAPI*)(void* pThis,
                                                             void* param1,
                                                             void* param2,
                                                             void* param3);
XamlAltTabViewHost_CreateInstance_t XamlAltTabViewHost_CreateInstance_Original;
HRESULT WINAPI XamlAltTabViewHost_CreateInstance_Hook(void* pThis,
                                                      void* param1,
                                                      void* param2,
                                                      void* param3) {
    Wh_Log(L">");
    return CreateInstanceHook([=]() {
        return XamlAltTabViewHost_CreateInstance_Original(pThis, param1, param2,
                                                          param3);
    });
}

using CAltTabViewHost_CreateInstance_t = HRESULT(WINAPI*)(void* pThis,
                                                          void* param1,
                                                          void* param2,
                                                          void* param3,
                                                          void* param4,
                                                          void* param5,
                                                          void* param6,
                                                          void* param7,
                                                          void* param8,
                                                          void* param9,
                                                          void* param10,
                                                          void* param11);
CAltTabViewHost_CreateInstance_t CAltTabViewHost_CreateInstance_Original;
HRESULT WINAPI CAltTabViewHost_CreateInstance_Hook(void* pThis,
                                                   void* param1,
                                                   void* param2,
                                                   void* param3,
                                                   void* param4,
                                                   void* param5,
                                                   void* param6,
                                                   void* param7,
                                                   void* param8,
                                                   void* param9,
                                                   void* param10,
                                                   void* param11) {
    Wh_Log(L">");
    return CreateInstanceHook([=]() {
        return CAltTabViewHost_CreateInstance_Original(
            pThis, param1, param2, param3, param4, param5, param6, param7,
            param8, param9, param10, param11);
    });
}

using CAltTabViewHost_CreateInstance_Win11_t = HRESULT(WINAPI*)(void* pThis,
                                                                void* param1,
                                                                void* param2,
                                                                void* param3,
                                                                void* param4,
                                                                void* param5,
                                                                void* param6,
                                                                void* param7,
                                                                void* param8,
                                                                void* param9,
                                                                void* param10);
CAltTabViewHost_CreateInstance_Win11_t
    CAltTabViewHost_CreateInstance_Win11_Original;
HRESULT WINAPI CAltTabViewHost_CreateInstance_Win11_Hook(void* pThis,
                                                         void* param1,
                                                         void* param2,
                                                         void* param3,
                                                         void* param4,
                                                         void* param5,
                                                         void* param6,
                                                         void* param7,
                                                         void* param8,
                                                         void* param9,
                                                         void* param10) {
    Wh_Log(L">");
    return CreateInstanceHook([=]() {
        return CAltTabViewHost_CreateInstance_Win11_Original(
            pThis, param1, param2, param3, param4, param5, param6, param7,
            param8, param9, param10);
    });
}

// ============================================================================
// Settings
// ============================================================================

void LoadSettings() {
    PCWSTR winAltTabLocation = Wh_GetStringSetting(L"winAltTabLocation");
    g_settings.winAltTabLocation = WinAltTabLocation::primary;
    if (wcscmp(winAltTabLocation, L"cursor") == 0) {
        g_settings.winAltTabLocation = WinAltTabLocation::cursor;
    }
    Wh_FreeStringSetting(winAltTabLocation);

    int threshold = Wh_GetIntSetting(L"postCreateThresholdMs");
    g_settings.postCreateThresholdMs =
        (threshold > 0) ? static_cast<ULONGLONG>(threshold) : 200;
}

// ============================================================================
// Mod lifecycle
// ============================================================================

// Common symbol hooks shared between Win10 and Win11
WindhawkUtils::SYMBOL_HOOK commonSymbolHooks[] = {
    {
        {LR"(public: virtual long __cdecl CVirtualDesktop::IsViewVisible(struct IApplicationView *,int *))"},
        &CVirtualDesktop_IsViewVisible_Original,
        CVirtualDesktop_IsViewVisible_Hook,
    },
    {
        {LR"(const CWin32ApplicationView::`vftable'{for `IApplicationView'})"},
        &CWin32ApplicationView_vtable,
    },
    {
        {LR"(private: virtual long __cdecl CWin32ApplicationView::v_GetNativeWindow(struct HWND__ * *))"},
        &CWin32ApplicationView_v_GetNativeWindow,
    },
    {
        {LR"(const CWinRTApplicationView::`vftable'{for `IApplicationView'})"},
        &CWinRTApplicationView_vtable,
    },
    {
        {LR"(private: virtual long __cdecl CWinRTApplicationView::v_GetNativeWindow(struct HWND__ * *))"},
        &CWinRTApplicationView_v_GetNativeWindow,
    },
};

BOOL Wh_ModInit() {
    Wh_Log(L">");

    LoadSettings();

    g_winVersion = GetWindowsVersion();

    HMODULE twinuiPcshellModule = LoadLibrary(L"twinui.pcshell.dll");
    if (!twinuiPcshellModule) {
        Wh_Log(L"Couldn't load twinui.pcshell.dll");
        return FALSE;
    }

    if (!HookSymbols(twinuiPcshellModule, commonSymbolHooks,
                     ARRAYSIZE(commonSymbolHooks))) {
        Wh_Log(L"HookSymbols failed for common hooks");
        return FALSE;
    }

    if (g_winVersion == WinVersion::Win11) {
        WindhawkUtils::SYMBOL_HOOK win11Hooks[] = {
            {
                {LR"(public: virtual long __cdecl XamlAltTabViewHost::Show(struct IImmersiveMonitor *,enum ALT_TAB_VIEW_FLAGS,struct IApplicationView *))"},
                &XamlAltTabViewHost_Show_Original,
                XamlAltTabViewHost_Show_Hook,
            },
            {
                {LR"(public: __cdecl winrt::impl::consume_Windows_Internal_Shell_TaskGroups_ITaskGroupWindowInformation<struct winrt::Windows::Internal::Shell::TaskGroups::ITaskGroupWindowInformation>::Position(struct winrt::Windows::Foundation::Rect const &)const )"},
                &ITaskGroupWindowInformation_Position_Original,
                ITaskGroupWindowInformation_Position_Hook,
            },
            {
                {LR"(long __cdecl XamlAltTabViewHost_CreateInstance(struct XamlViewHostInitializeArgs const &,struct _GUID const &,void * *))"},
                &XamlAltTabViewHost_CreateInstance_Original,
                XamlAltTabViewHost_CreateInstance_Hook,
            },
            // ExplorerPatcher compatibility: Win10-style Alt+Tab on Win11
            {
                {LR"(public: virtual long __cdecl CAltTabViewHost::Show(struct IImmersiveMonitor *,enum ALT_TAB_VIEW_FLAGS,struct IApplicationView *))"},
                &CAltTabViewHost_Show_Original,
                CAltTabViewHost_Show_Hook,
                true,
            },
            {
                {LR"(public: virtual long __cdecl CMultitaskingViewFrame::CreateFrame(struct Geometry::CRect const &,enum MultitaskingViewZOrder))"},
                &CMultitaskingViewFrame_CreateFrame_Original,
                CMultitaskingViewFrame_CreateFrame_Hook,
                true,
            },
            {
                {LR"(long __cdecl CAltTabViewHost_CreateInstance(struct IMultitaskingData *,struct IMultitaskingViewManagerInternal *,struct IApplicationViewSwitcher *,struct IImmersiveAppCrusher *,struct IMultitaskingViewVisibilityServiceInternal *,struct IMultitaskingViewGestureState *,struct IApplicationViewCollection *,struct ITouchGestureSettings *,struct _GUID const &,void * *))"},
                &CAltTabViewHost_CreateInstance_Win11_Original,
                CAltTabViewHost_CreateInstance_Win11_Hook,
                true,
            },
        };

        if (!HookSymbols(twinuiPcshellModule, win11Hooks,
                         ARRAYSIZE(win11Hooks))) {
            Wh_Log(L"HookSymbols failed for Win11 hooks");
            return FALSE;
        }
    } else if (g_winVersion == WinVersion::Win10) {
        WindhawkUtils::SYMBOL_HOOK win10Hooks[] = {
            {
                {LR"(public: virtual long __cdecl CAltTabViewHost::Show(struct IImmersiveMonitor *,enum ALT_TAB_VIEW_FLAGS,struct IApplicationView *))"},
                &CAltTabViewHost_Show_Original,
                CAltTabViewHost_Show_Hook,
            },
            {
                {LR"(public: virtual long __cdecl CMultitaskingViewFrame::CreateFrame(struct Geometry::CRect const &,enum MultitaskingViewZOrder))"},
                &CMultitaskingViewFrame_CreateFrame_Original,
                CMultitaskingViewFrame_CreateFrame_Hook,
            },
            {
                {LR"(long __cdecl CAltTabViewHost_CreateInstance(struct IMultitaskingData *,struct IMultitaskingViewManagerInternal *,struct IApplicationViewSwitcher *,struct IImmersiveAppCrusher *,struct IMultitaskingViewVisibilityServiceInternal *,struct IMultitaskingViewGestureState *,struct IApplicationViewCollection *,struct Windows::Internal::ComposableShell::Tabs::ITabController *,struct ITabViewManager *,struct _GUID const &,void * *))"},
                &CAltTabViewHost_CreateInstance_Original,
                CAltTabViewHost_CreateInstance_Hook,
            },
        };

        if (!HookSymbols(twinuiPcshellModule, win10Hooks,
                         ARRAYSIZE(win10Hooks))) {
            Wh_Log(L"HookSymbols failed for Win10 hooks");
            return FALSE;
        }
    } else {
        Wh_Log(L"Unsupported Windows version");
        return FALSE;
    }

    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L">");
    g_unloading = true;

    // Wait for any in-flight hook calls to complete
    int waitCount = 0;
    while (g_hookCallCounter > 0 && waitCount < 50) {
        Sleep(100);
        waitCount++;
    }
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");
    LoadSettings();
}
