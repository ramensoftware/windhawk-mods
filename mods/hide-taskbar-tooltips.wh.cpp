// ==WindhawkMod==
// @id              hide-taskbar-tooltips
// @name            Hide Taskbar Tooltips
// @description     Suppresses native Windows 11 XAML hover tooltips in Explorer, with an option to also hide application preview thumbnails.
// @version         1.1.0
// @author          gilnett
// @github          https://github.com/gilnett
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject
// @donateUrl       https://ko-fi.com/gilnet
// @license         GPL-3.0
// ==/WindhawkMod==

// Source code is published under The GNU General Public License v3.0.

// ==WindhawkModReadme==
/*
# Hide Taskbar Tooltips

Suppresses native Windows 11 XAML hover tooltips in Explorer, covering:
- Taskbar app icon tooltips and labels on hover
- System tray status icons tooltips (Network, Volume, Battery, Clock)
- System XAML shell UI hosted in Explorer (Task View, snap layouts, and virtual desktops)

Optionally, it can also hide application window preview thumbnails on hover.

## Preview

**Before** (native Windows 11 tooltip on hover):  
![Before](https://i.imgur.com/XW2Ygxq.png)

**After** (tooltip suppressed):  
![After](https://i.imgur.com/IBPlzQf.png)

## Settings

- **Also hide app window preview thumbnails**: When enabled, suppresses taskbar window preview thumbnails from popping up when hovering over application icons.

## Compatibility

- Only Windows 11 is supported.

## Support

If you find this mod useful, you can support its development and maintenance:
- [Support on Ko-fi](https://ko-fi.com/gilnet)
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- hideThumbnails: false
  $name: Also hide app window preview thumbnails
  $description: >-
    In addition to suppressing hover tooltips, also prevents window preview
    thumbnails from popping up when hovering over taskbar application buttons.
*/
// ==/WindhawkModSettings==

#include <windhawk_api.h>
#include <windhawk_utils.h>

#include <windows.h>

#include <atomic>
#include <mutex>
#include <unordered_set>

#undef GetCurrentTime

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.Xaml.Controls.h>

static bool IsWindows11OrGreater() {
    HMODULE hNtdll = GetModuleHandleW(L"ntdll.dll");
    if (!hNtdll) {
        return false;
    }

    using fnRtlGetNtVersionNumbers =
        void(WINAPI*)(LPDWORD major, LPDWORD minor, LPDWORD build);
    auto RtlGetNtVersionNumbers =
        reinterpret_cast<fnRtlGetNtVersionNumbers>(
            GetProcAddress(hNtdll, "RtlGetNtVersionNumbers"));
    if (!RtlGetNtVersionNumbers) {
        return false;
    }

    DWORD major = 0, minor = 0, build = 0;
    RtlGetNtVersionNumbers(&major, &minor, &build);
    build &= ~0xF0000000;
    return (major > 10) || (major == 10 && build >= 22000);
}

static std::atomic<DWORD> g_taskbarThreadId{0};
static thread_local int t_tooltipScopeDepth = 0;

struct Settings {
    bool hideThumbnails{false};
} g_settings;

static void LoadSettings() {
    g_settings.hideThumbnails = Wh_GetIntSetting(L"hideThumbnails") != 0;
}

static std::atomic<DWORD> g_showTaskListButtonHoverFlyoutThreadId{0};
static thread_local bool t_inTransitionToFlyoutVisibleStickyState{false};

struct ToolTipScope {
    ToolTipScope() {
        t_tooltipScopeDepth++;
    }
    ~ToolTipScope() {
        t_tooltipScopeDepth--;
    }
};

static std::unordered_set<HWND> s_tooltipWindows;
static std::mutex s_tooltipWindowsMutex;
static std::atomic<bool> s_hasTrackedWindows{false};

static void RecordTooltipWindow(HWND hWnd) {
    if (!hWnd) {
        return;
    }
    std::lock_guard<std::mutex> lock(s_tooltipWindowsMutex);
    s_tooltipWindows.insert(hWnd);
    s_hasTrackedWindows.store(true, std::memory_order_relaxed);
}

static bool IsTooltipWindow(HWND hWnd) {
    if (!hWnd || !s_hasTrackedWindows.load(std::memory_order_relaxed)) {
        return false;
    }
    if (!IsWindow(hWnd)) {
        return false;
    }
    std::lock_guard<std::mutex> lock(s_tooltipWindowsMutex);
    auto it = s_tooltipWindows.find(hWnd);
    if (it == s_tooltipWindows.end()) {
        return false;
    }
    WCHAR className[64] = {0};
    if (!GetClassNameW(hWnd, className, ARRAYSIZE(className)) ||
        _wcsicmp(className, L"Xaml_WindowedPopupClass") != 0) {
        s_tooltipWindows.erase(it);
        if (s_tooltipWindows.empty()) {
            s_hasTrackedWindows.store(false, std::memory_order_relaxed);
        }
        return false;
    }
    return true;
}

using CreateWindowExW_t = decltype(&CreateWindowExW);
static CreateWindowExW_t CreateWindowExW_Original = nullptr;

static HWND WINAPI CreateWindowExW_Hook(DWORD dwExStyle,
                                        LPCWSTR lpClassName,
                                        LPCWSTR lpWindowName,
                                        DWORD dwStyle,
                                        int X,
                                        int Y,
                                        int nWidth,
                                        int nHeight,
                                        HWND hWndParent,
                                        HMENU hMenu,
                                        HINSTANCE hInstance,
                                        LPVOID lpParam) {
    DWORD taskbarThreadId = g_taskbarThreadId.load(std::memory_order_relaxed);
    if (taskbarThreadId && GetCurrentThreadId() != taskbarThreadId) {
        return CreateWindowExW_Original(dwExStyle, lpClassName, lpWindowName,
                                        dwStyle, X, Y, nWidth, nHeight,
                                        hWndParent, hMenu, hInstance, lpParam);
    }

    bool isTooltipScope = (t_tooltipScopeDepth > 0);
    bool isXamlPopup =
        (lpClassName && (reinterpret_cast<ULONG_PTR>(lpClassName) > 0xFFFF) &&
         _wcsicmp(lpClassName, L"Xaml_WindowedPopupClass") == 0);

    if (isTooltipScope && isXamlPopup) {
        Wh_Log(L"> CreateWindowExW in tooltip scope: class=%s, style=0x%X",
               lpClassName, dwStyle);
        dwStyle &= ~WS_VISIBLE;
    }
    HWND hWnd = CreateWindowExW_Original(dwExStyle, lpClassName, lpWindowName,
                                         dwStyle, X, Y, nWidth, nHeight,
                                         hWndParent, hMenu, hInstance, lpParam);
    if (hWnd && isTooltipScope && isXamlPopup) {
        RecordTooltipWindow(hWnd);
    }
    return hWnd;
}

using ShowWindow_t = decltype(&ShowWindow);
static ShowWindow_t ShowWindow_Original = nullptr;

static BOOL WINAPI ShowWindow_Hook(HWND hWnd, int nCmdShow) {
    DWORD taskbarThreadId = g_taskbarThreadId.load(std::memory_order_relaxed);
    if (taskbarThreadId && GetCurrentThreadId() != taskbarThreadId) {
        return ShowWindow_Original(hWnd, nCmdShow);
    }

    if (t_tooltipScopeDepth > 0 || IsTooltipWindow(hWnd)) {
        Wh_Log(L"> ShowWindow suppressed for tooltip window %p, cmd=%d", hWnd,
               nCmdShow);
        if (nCmdShow != SW_HIDE) {
            return FALSE;
        }
    }

    if (g_settings.hideThumbnails &&
        g_showTaskListButtonHoverFlyoutThreadId.load(
            std::memory_order_relaxed) == GetCurrentThreadId() &&
        !t_inTransitionToFlyoutVisibleStickyState) {
        if (nCmdShow != SW_HIDE) {
            Wh_Log(L"> ShowWindow suppressed for thumbnail flyout %p, cmd=%d",
                   hWnd, nCmdShow);
            return TRUE;
        }
    }
    return ShowWindow_Original(hWnd, nCmdShow);
}

using SetWindowPos_t = decltype(&SetWindowPos);
static SetWindowPos_t SetWindowPos_Original = nullptr;

static BOOL WINAPI SetWindowPos_Hook(HWND hWnd,
                                     HWND hWndInsertAfter,
                                     int X,
                                     int Y,
                                     int cx,
                                     int cy,
                                     UINT uFlags) {
    DWORD taskbarThreadId = g_taskbarThreadId.load(std::memory_order_relaxed);
    if (taskbarThreadId && GetCurrentThreadId() != taskbarThreadId) {
        return SetWindowPos_Original(hWnd, hWndInsertAfter, X, Y, cx, cy,
                                     uFlags);
    }

    if (t_tooltipScopeDepth > 0) {
        if (!IsTooltipWindow(hWnd)) {
            WCHAR className[64] = {0};
            if (GetClassNameW(hWnd, className, ARRAYSIZE(className)) &&
                _wcsicmp(className, L"Xaml_WindowedPopupClass") == 0) {
                Wh_Log(L"> SetWindowPos in tooltip scope: %p (%s), flags=0x%X",
                       hWnd, className, uFlags);
                RecordTooltipWindow(hWnd);
            }
        }
    }

    if (IsTooltipWindow(hWnd)) {
        if (uFlags & SWP_SHOWWINDOW) {
            Wh_Log(L"> SetWindowPos SWP_SHOWWINDOW stripped for %p", hWnd);
            uFlags &= ~SWP_SHOWWINDOW;
            uFlags |= SWP_HIDEWINDOW;
        }
    }
    return SetWindowPos_Original(hWnd, hWndInsertAfter, X, Y, cx, cy, uFlags);
}

// WinRT IToolTip ABI: put_IsOpen is at vtable index 9
using ToolTip_put_IsOpen_t = HRESULT(__stdcall*)(void* pThis, boolean value);
static ToolTip_put_IsOpen_t g_toolTipPutIsOpenOriginal = nullptr;
static std::atomic<bool> g_putIsOpenHooked{false};

static HRESULT __stdcall ToolTip_put_IsOpen_Hook(void* pThis, boolean value) {
    if (value) {
        // Suppress opening any tooltip popup in the XAML framework with zero overhead
        return S_OK;
    }
    if (g_toolTipPutIsOpenOriginal) {
        return g_toolTipPutIsOpenOriginal(pThis, value);
    }
    return S_OK;
}

static bool SuppressToolTip(void* pToolTip) {
    if (!g_taskbarThreadId.load(std::memory_order_relaxed)) {
        g_taskbarThreadId.store(GetCurrentThreadId(),
                                std::memory_order_relaxed);
    }

    if (!pToolTip) {
        return false;
    }

    void* pIToolTip = *(reinterpret_cast<void**>(pToolTip));
    if (!pIToolTip) {
        return false;
    }

    // Verify and obtain true IToolTip vtable via QueryInterface
    IUnknown* unk = reinterpret_cast<IUnknown*>(pIToolTip);
    winrt::com_ptr<winrt::Windows::UI::Xaml::Controls::IToolTip> spToolTip;
    if (FAILED(unk->QueryInterface(
            winrt::guid_of<winrt::Windows::UI::Xaml::Controls::IToolTip>(),
            spToolTip.put_void()))) {
        return false;
    }

    void** vtable = *reinterpret_cast<void***>(spToolTip.get());
    if (!vtable || !vtable[9]) {
        return false;
    }

    auto put_IsOpen = reinterpret_cast<ToolTip_put_IsOpen_t>(vtable[9]);

    // Install global hook on ToolTip::put_IsOpen exactly once if not already hooked eagerly
    if (!g_putIsOpenHooked.exchange(true)) {
        WindhawkUtils::SetFunctionHook(
            reinterpret_cast<void*>(put_IsOpen),
            reinterpret_cast<void*>(ToolTip_put_IsOpen_Hook),
            reinterpret_cast<void**>(&g_toolTipPutIsOpenOriginal));
        Wh_ApplyHookOperations();
        Wh_Log(L"> Successfully hooked ToolTip::put_IsOpen on first live instance");
    }

    // Immediately close the tooltip instance
    put_IsOpen(spToolTip.get(), FALSE);
    return true;
}

static BOOL CALLBACK EnumWindowsTaskbarProc(HWND hWnd, LPARAM lParam) {
    DWORD dwProcessId = 0;
    WCHAR className[32] = {0};
    if (GetWindowThreadProcessId(hWnd, &dwProcessId) &&
        dwProcessId == GetCurrentProcessId() &&
        GetClassNameW(hWnd, className, ARRAYSIZE(className)) &&
        _wcsicmp(className, L"Shell_TrayWnd") == 0) {
        *reinterpret_cast<HWND*>(lParam) = hWnd;
        return FALSE;
    }
    return TRUE;
}

static HWND FindCurrentProcessTaskbarWnd() {
    HWND hTaskbarWnd = nullptr;
    EnumWindows(EnumWindowsTaskbarProc, reinterpret_cast<LPARAM>(&hTaskbarWnd));
    return hTaskbarWnd;
}

using RunFromWindowThreadProc_t = void(WINAPI*)(void* parameter);

static UINT GetRunFromWindowThreadMessage() {
    static const UINT message =
        RegisterWindowMessageW(L"Windhawk_RunFromWindowThread_" WH_MOD_ID);
    return message;
}

struct RunFromWindowThreadParam {
    RunFromWindowThreadProc_t proc;
    void* procParam;
};

static LRESULT CALLBACK CallWndProcHook(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        const auto* cwp = reinterpret_cast<const CWPSTRUCT*>(lParam);
        if (cwp->message == GetRunFromWindowThreadMessage()) {
            auto* param =
                reinterpret_cast<RunFromWindowThreadParam*>(cwp->lParam);
            if (param && param->proc) {
                param->proc(param->procParam);
            }
        }
    }
    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

static bool RunFromWindowThread(HWND hWnd,
                                RunFromWindowThreadProc_t proc,
                                void* procParam) {
    DWORD dwThreadId = GetWindowThreadProcessId(hWnd, nullptr);
    if (dwThreadId == 0) {
        return false;
    }

    if (dwThreadId == GetCurrentThreadId()) {
        proc(procParam);
        return true;
    }

    HHOOK hook = SetWindowsHookExW(
        WH_CALLWNDPROC,
        CallWndProcHook,
        nullptr, dwThreadId);
    if (!hook) {
        return false;
    }

    RunFromWindowThreadParam param{proc, procParam};
    SendMessageW(hWnd, GetRunFromWindowThreadMessage(), 0,
                 reinterpret_cast<LPARAM>(&param));

    UnhookWindowsHookEx(hook);
    return true;
}

static void WINAPI EagerHookToolTipOnTaskbarThread(void* /*procParam*/) {
    try {
        winrt::Windows::UI::Xaml::Controls::ToolTip dummyToolTip;
        void* pUnk = winrt::get_abi(dummyToolTip);
        if (pUnk) {
            void** vtable = *reinterpret_cast<void***>(pUnk);
            if (vtable && vtable[9]) {
                auto put_IsOpen =
                    reinterpret_cast<ToolTip_put_IsOpen_t>(vtable[9]);
                if (!g_putIsOpenHooked.exchange(true)) {
                    WindhawkUtils::SetFunctionHook(
                        reinterpret_cast<void*>(put_IsOpen),
                        reinterpret_cast<void*>(ToolTip_put_IsOpen_Hook),
                        reinterpret_cast<void**>(&g_toolTipPutIsOpenOriginal));
                    Wh_ApplyHookOperations();
                    Wh_Log(L"> Successfully hooked ToolTip::put_IsOpen eagerly on Taskbar UI thread!");
                }
            }
        }
    } catch (...) {
        Wh_Log(L"> Eager ToolTip hook on UI thread caught exception");
    }
}

static void InitEagerToolTipHook() {
    HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();
    if (hTaskbarWnd) {
        DWORD threadId = GetWindowThreadProcessId(hTaskbarWnd, nullptr);
        g_taskbarThreadId.store(threadId, std::memory_order_relaxed);
        Wh_Log(L"> Dispatching eager ToolTip hook to Taskbar window %p (thread %u)",
               hTaskbarWnd, threadId);
        RunFromWindowThread(hTaskbarWnd, EagerHookToolTipOnTaskbarThread,
                            nullptr);
    } else {
        Wh_Log(L"> Taskbar window not found yet for eager ToolTip hook");
    }
}

// SystemTray.dll: TaskbarLocationHelpers::PositionTaskbarTooltip
using PositionTaskbarTooltip_t = void(__cdecl*)(void* pToolTip,
                                                void* pTarget,
                                                int location,
                                                bool b1,
                                                bool b2);
static PositionTaskbarTooltip_t PositionTaskbarTooltip_Original = nullptr;

static void __cdecl PositionTaskbarTooltip_Hook(void* pToolTip,
                                                void* pTarget,
                                                int location,
                                                bool b1,
                                                bool b2) {
    ToolTipScope scope;
    if (!SuppressToolTip(pToolTip) && PositionTaskbarTooltip_Original) {
        PositionTaskbarTooltip_Original(pToolTip, pTarget, location, b1, b2);
    }
}

// SystemTray.dll & Taskbar.View.dll: TaskbarLocationHelpers::ApplyTaskbarTooltipPlacement (4 params)
using ApplyTaskbarTooltipPlacement_t =
    void(__cdecl*)(void* pToolTip,
                   int location,
                   winrt::Windows::Foundation::Size size1,
                   winrt::Windows::Foundation::Size size2);

static ApplyTaskbarTooltipPlacement_t
    ApplyTaskbarTooltipPlacement_Tray_Original = nullptr;

static void __cdecl ApplyTaskbarTooltipPlacement_Tray_Hook(
    void* pToolTip,
    int location,
    winrt::Windows::Foundation::Size size1,
    winrt::Windows::Foundation::Size size2) {
    ToolTipScope scope;
    if (!SuppressToolTip(pToolTip) &&
        ApplyTaskbarTooltipPlacement_Tray_Original) {
        ApplyTaskbarTooltipPlacement_Tray_Original(pToolTip, location, size1,
                                                   size2);
    }
}

static ApplyTaskbarTooltipPlacement_t
    ApplyTaskbarTooltipPlacement_View_Original = nullptr;

static void __cdecl ApplyTaskbarTooltipPlacement_View_Hook(
    void* pToolTip,
    int location,
    winrt::Windows::Foundation::Size size1,
    winrt::Windows::Foundation::Size size2) {
    ToolTipScope scope;
    if (!SuppressToolTip(pToolTip) &&
        ApplyTaskbarTooltipPlacement_View_Original) {
        ApplyTaskbarTooltipPlacement_View_Original(pToolTip, location, size1,
                                                   size2);
    }
}

// Taskbar.View.dll: TaskbarLocationHelpers::ApplyTaskbarTooltipPlacement (6 params)
using ApplyTaskbarTooltipPlacement6_t =
    void(__cdecl*)(void* pToolTip,
                   int location,
                   winrt::Windows::Foundation::Size size1,
                   winrt::Windows::Foundation::Size size2,
                   bool b1,
                   bool b2);
static ApplyTaskbarTooltipPlacement6_t ApplyTaskbarTooltipPlacement6_Original =
    nullptr;

static void __cdecl ApplyTaskbarTooltipPlacement6_Hook(
    void* pToolTip,
    int location,
    winrt::Windows::Foundation::Size size1,
    winrt::Windows::Foundation::Size size2,
    bool b1,
    bool b2) {
    ToolTipScope scope;
    if (!SuppressToolTip(pToolTip) && ApplyTaskbarTooltipPlacement6_Original) {
        ApplyTaskbarTooltipPlacement6_Original(pToolTip, location, size1, size2,
                                               b1, b2);
    }
}

// Hover state hooks in Taskbar.View.dll (zero-cost no-ops)
using ExperienceToggleButton_UpdateHover_t = void(__cdecl*)(void* pThis,
                                                            bool isHover);
static ExperienceToggleButton_UpdateHover_t
    ExperienceToggleButton_UpdateHover_Original = nullptr;

static void __cdecl ExperienceToggleButton_UpdateHover_Hook(void* /*pThis*/,
                                                            bool /*isHover*/) {
}

using TaskListButton_UpdateHover_t = void(__cdecl*)(void* pThis);
static TaskListButton_UpdateHover_t TaskListButton_UpdateHover_Original =
    nullptr;

static void __cdecl TaskListButton_UpdateHover_Hook(void* /*pThis*/) {
}

using OverflowToggleButton_UpdateHover_t = void(__cdecl*)(void* pThis);
static OverflowToggleButton_UpdateHover_t
    OverflowToggleButton_UpdateHover_Original = nullptr;

static void __cdecl OverflowToggleButton_UpdateHover_Hook(void* /*pThis*/) {
}

using TaskListButton_AddToolTipContent_t = void(__cdecl*)(void* pThis);
static TaskListButton_AddToolTipContent_t
    TaskListButton_AddToolTipContent_Original = nullptr;

static void __cdecl TaskListButton_AddToolTipContent_Hook(void* /*pThis*/) {
}

using TaskItemThumbnailView_UpdateToolTip_t = void(__cdecl*)(void* pThis);
static TaskItemThumbnailView_UpdateToolTip_t
    TaskItemThumbnailView_UpdateToolTip_Original = nullptr;

static void __cdecl TaskItemThumbnailView_UpdateToolTip_Hook(void* /*pThis*/) {
}

// CTaskListWnd hooks for classic taskbar.dll / explorer.exe thumbnails
using CTaskListWnd__DisplayExtendedUI_t = HRESULT(WINAPI*)(void* pThis,
                                                           void* taskBtnGroup,
                                                           int param2,
                                                           DWORD flags,
                                                           int param4);
static CTaskListWnd__DisplayExtendedUI_t
    CTaskListWnd__DisplayExtendedUI_Original = nullptr;

static HRESULT WINAPI CTaskListWnd__DisplayExtendedUI_Hook(void* pThis,
                                                           void* taskBtnGroup,
                                                           int param2,
                                                           DWORD flags,
                                                           int param4) {
    bool persistent = (flags & 2) != 0;
    if (!persistent && g_settings.hideThumbnails) {
        return S_OK;
    }
    return CTaskListWnd__DisplayExtendedUI_Original(
        pThis, taskBtnGroup, param2, flags, param4);
}

using CTaskListThumbnailWnd__CanShowThumbnails_t = BOOL(WINAPI*)(void* pThis,
                                                                 void* param1,
                                                                 int param2,
                                                                 int param3);
static CTaskListThumbnailWnd__CanShowThumbnails_t
    CTaskListThumbnailWnd__CanShowThumbnails_Original = nullptr;

static BOOL WINAPI CTaskListThumbnailWnd__CanShowThumbnails_Hook(void* pThis,
                                                                 void* param1,
                                                                 int param2,
                                                                 int param3) {
    if (g_settings.hideThumbnails) {
        return FALSE;
    }
    return CTaskListThumbnailWnd__CanShowThumbnails_Original(pThis, param1,
                                                             param2, param3);
}

// Modern Taskbar.View.dll HoverFlyout hooks
using HoverFlyoutController_ShowTaskListButtonHoverFlyout_t =
    void(__cdecl*)(void* pThis,
                   void* param1,
                   void* param2,
                   int param3,
                   int param4);
static HoverFlyoutController_ShowTaskListButtonHoverFlyout_t
    HoverFlyoutController_ShowTaskListButtonHoverFlyout_Original = nullptr;

static void __cdecl HoverFlyoutController_ShowTaskListButtonHoverFlyout_Hook(
    void* pThis,
    void* param1,
    void* param2,
    int param3,
    int param4) {
    if (!g_settings.hideThumbnails) {
        HoverFlyoutController_ShowTaskListButtonHoverFlyout_Original(
            pThis, param1, param2, param3, param4);
        return;
    }

    g_showTaskListButtonHoverFlyoutThreadId.store(GetCurrentThreadId(),
                                                  std::memory_order_relaxed);
    HoverFlyoutController_ShowTaskListButtonHoverFlyout_Original(
        pThis, param1, param2, param3, param4);
    g_showTaskListButtonHoverFlyoutThreadId.store(0, std::memory_order_relaxed);
}

using HoverFlyoutController_ShowTaskListButtonHoverFlyout_Old1_t =
    void(__cdecl*)(void* pThis, void* param1, void* param2, int param3);
static HoverFlyoutController_ShowTaskListButtonHoverFlyout_Old1_t
    HoverFlyoutController_ShowTaskListButtonHoverFlyout_Old1_Original = nullptr;

static void __cdecl HoverFlyoutController_ShowTaskListButtonHoverFlyout_Old1_Hook(
    void* pThis,
    void* param1,
    void* param2,
    int param3) {
    if (!g_settings.hideThumbnails) {
        HoverFlyoutController_ShowTaskListButtonHoverFlyout_Old1_Original(
            pThis, param1, param2, param3);
        return;
    }

    g_showTaskListButtonHoverFlyoutThreadId.store(GetCurrentThreadId(),
                                                  std::memory_order_relaxed);
    HoverFlyoutController_ShowTaskListButtonHoverFlyout_Old1_Original(
        pThis, param1, param2, param3);
    g_showTaskListButtonHoverFlyoutThreadId.store(0, std::memory_order_relaxed);
}

using HoverFlyoutModel_TransitionToFlyoutVisibleStickyState_t =
    void(__cdecl*)(void* pThis, void* param1);
static HoverFlyoutModel_TransitionToFlyoutVisibleStickyState_t
    HoverFlyoutModel_TransitionToFlyoutVisibleStickyState_Original = nullptr;

static void __cdecl HoverFlyoutModel_TransitionToFlyoutVisibleStickyState_Hook(
    void* pThis,
    void* param1) {
    t_inTransitionToFlyoutVisibleStickyState = true;
    if (HoverFlyoutModel_TransitionToFlyoutVisibleStickyState_Original) {
        HoverFlyoutModel_TransitionToFlyoutVisibleStickyState_Original(pThis,
                                                                       param1);
    }
    t_inTransitionToFlyoutVisibleStickyState = false;
}

// Hover state hook in SystemTray.dll (zero-cost no-ops)
using IconView_UpdateOuterToolTipPlacement_t = void(__cdecl*)(void* pThis,
                                                              bool isHover);
static IconView_UpdateOuterToolTipPlacement_t
    IconView_UpdateOuterToolTipPlacement_Original = nullptr;

static void __cdecl IconView_UpdateOuterToolTipPlacement_Hook(
    void* /*pThis*/,
    bool /*isHover*/) {
}

// Method IsToolTipEnabled -> return false
using IsToolTipEnabled_t = bool(__cdecl*)(void* pThis);

static bool __cdecl IsToolTipEnabled_Hook(void* /*pThis*/) {
    return false;
}

static IsToolTipEnabled_t IsToolTipEnabled_LaunchList_Orig = nullptr;
static IsToolTipEnabled_t IsToolTipEnabled_Augmented_Orig = nullptr;
static IsToolTipEnabled_t IsToolTipEnabled_Recommended_Orig = nullptr;

static std::atomic<bool> g_systemTrayHooked{false};
static std::atomic<bool> g_taskbarViewHooked{false};

static bool HookSystemTraySymbols(HMODULE module) {
    Wh_Log(L"> Hooking SystemTray symbols on %p", module);
    // SystemTray.dll
    WindhawkUtils::SYMBOL_HOOK systemTrayHooks[] = {
        {
            {
                LR"(void __cdecl TaskbarLocationHelpers::PositionTaskbarTooltip(struct winrt::Windows::UI::Xaml::Controls::ToolTip const &,struct winrt::Windows::UI::Xaml::FrameworkElement const &,enum winrt::WindowsUdk::UI::Shell::TaskbarLocation,bool,bool))",
                LR"(void __cdecl TaskbarLocationHelpers::PositionTaskbarTooltip(struct winrt::Windows::UI::Xaml::Controls::ToolTip const &,struct winrt::Windows::UI::Xaml::FrameworkElement const &,enum winrt::WindowsUdk::Shell::TaskbarLocation,bool,bool))",
            },
            &PositionTaskbarTooltip_Original,
            PositionTaskbarTooltip_Hook,
            true,
        },
        {
            {
                LR"(void __cdecl TaskbarLocationHelpers::ApplyTaskbarTooltipPlacement(struct winrt::Windows::UI::Xaml::Controls::ToolTip const &,enum winrt::WindowsUdk::UI::Shell::TaskbarLocation,struct winrt::Windows::Foundation::Size,struct winrt::Windows::Foundation::Size))",
                LR"(void __cdecl TaskbarLocationHelpers::ApplyTaskbarTooltipPlacement(struct winrt::Windows::UI::Xaml::Controls::ToolTip const &,enum winrt::WindowsUdk::Shell::TaskbarLocation,struct winrt::Windows::Foundation::Size,struct winrt::Windows::Foundation::Size))",
            },
            &ApplyTaskbarTooltipPlacement_Tray_Original,
            ApplyTaskbarTooltipPlacement_Tray_Hook,
            true,
        },
        {
            {
                LR"(private: void __cdecl winrt::SystemTray::implementation::IconView::UpdateOuterToolTipPlacement(bool))",
            },
            &IconView_UpdateOuterToolTipPlacement_Original,
            IconView_UpdateOuterToolTipPlacement_Hook,
            true,
        },
    };

    bool res = WindhawkUtils::HookSymbols(module, systemTrayHooks,
                                          ARRAYSIZE(systemTrayHooks));
    Wh_Log(L"> HookSystemTraySymbols result: %d", res ? 1 : 0);
    return res;
}

static bool HookTaskbarViewSymbols(HMODULE module) {
    Wh_Log(L"> Hooking Taskbar.View symbols on %p", module);
    // Taskbar.View.dll
    WindhawkUtils::SYMBOL_HOOK taskbarViewHooks[] = {
        {
            {
                LR"(public: void __cdecl winrt::Taskbar::implementation::ExperienceToggleButton::UpdateToolTipPlacementForHoverState(bool))",
            },
            &ExperienceToggleButton_UpdateHover_Original,
            ExperienceToggleButton_UpdateHover_Hook,
            true,
        },
        {
            {
                LR"(public: void __cdecl winrt::Taskbar::implementation::TaskListButton::UpdateToolTipPlacementForHoverState(void))",
            },
            &TaskListButton_UpdateHover_Original,
            TaskListButton_UpdateHover_Hook,
            true,
        },
        {
            {
                LR"(public: void __cdecl winrt::Taskbar::implementation::OverflowToggleButton::UpdateToolTipPlacementForHoverState(void))",
            },
            &OverflowToggleButton_UpdateHover_Original,
            OverflowToggleButton_UpdateHover_Hook,
            true,
        },
        {
            {
                LR"(public: void __cdecl winrt::Taskbar::implementation::TaskListButton::AddToolTipContent(void))",
            },
            &TaskListButton_AddToolTipContent_Original,
            TaskListButton_AddToolTipContent_Hook,
            true,
        },
        {
            {
                LR"(private: void __cdecl winrt::Taskbar::implementation::TaskItemThumbnailView::UpdateToolTip(void))",
            },
            &TaskItemThumbnailView_UpdateToolTip_Original,
            TaskItemThumbnailView_UpdateToolTip_Hook,
            true,
        },
        {
            {
                LR"(public: virtual bool __cdecl winrt::Taskbar::implementation::LaunchListItemViewModel::IsToolTipEnabled(void)const)",
            },
            &IsToolTipEnabled_LaunchList_Orig,
            IsToolTipEnabled_Hook,
            true,
        },
        {
            {
                LR"(public: virtual bool __cdecl winrt::Taskbar::implementation::AugmentedEntryPointViewModel::IsToolTipEnabled(void)const)",
            },
            &IsToolTipEnabled_Augmented_Orig,
            IsToolTipEnabled_Hook,
            true,
        },
        {
            {
                LR"(public: bool __cdecl winrt::Taskbar::implementation::RecommendedItemViewModel::IsToolTipEnabled(void)const)",
            },
            &IsToolTipEnabled_Recommended_Orig,
            IsToolTipEnabled_Hook,
            true,
        },
        {
            {
                LR"(void __cdecl TaskbarLocationHelpers::ApplyTaskbarTooltipPlacement(struct winrt::Windows::UI::Xaml::Controls::ToolTip const &,enum winrt::WindowsUdk::UI::Shell::TaskbarLocation,struct winrt::Windows::Foundation::Size,struct winrt::Windows::Foundation::Size,bool,bool))",
                LR"(void __cdecl TaskbarLocationHelpers::ApplyTaskbarTooltipPlacement(struct winrt::Windows::UI::Xaml::Controls::ToolTip const &,enum winrt::WindowsUdk::Shell::TaskbarLocation,struct winrt::Windows::Foundation::Size,struct winrt::Windows::Foundation::Size,bool,bool))",
            },
            &ApplyTaskbarTooltipPlacement6_Original,
            ApplyTaskbarTooltipPlacement6_Hook,
            true,
        },
        {
            {
                LR"(private: void __cdecl winrt::Taskbar::implementation::HoverFlyoutModel::TransitionToFlyoutVisibleStickyState(struct winrt::hstring))",
            },
            &HoverFlyoutModel_TransitionToFlyoutVisibleStickyState_Original,
            HoverFlyoutModel_TransitionToFlyoutVisibleStickyState_Hook,
            true,
        },
        {
            {
                LR"(private: void __cdecl winrt::Taskbar::implementation::HoverFlyoutController::ShowTaskListButtonHoverFlyout(class std::vector<struct winrt::weak_ref<struct winrt::Windows::UI::Xaml::FrameworkElement>,class std::allocator<struct winrt::weak_ref<struct winrt::Windows::UI::Xaml::FrameworkElement> > >,struct winrt::Windows::Foundation::Collections::IVector<struct winrt::Windows::Foundation::IInspectable> const &,enum winrt::WindowsUdk::UI::Shell::InputDeviceKind,enum winrt::WindowsUdk::UI::Shell::TaskbarFlyoutKind))",
            },
            &HoverFlyoutController_ShowTaskListButtonHoverFlyout_Original,
            HoverFlyoutController_ShowTaskListButtonHoverFlyout_Hook,
            true,
        },
        {
            {
                LR"(private: void __cdecl winrt::Taskbar::implementation::HoverFlyoutController::ShowTaskListButtonHoverFlyout(class std::vector<struct winrt::weak_ref<struct winrt::Windows::UI::Xaml::FrameworkElement>,class std::allocator<struct winrt::weak_ref<struct winrt::Windows::UI::Xaml::FrameworkElement> > >,struct winrt::Windows::Foundation::Collections::IVector<struct winrt::Windows::Foundation::IInspectable> const &,enum winrt::WindowsUdk::UI::Shell::InputDeviceKind))",
            },
            &HoverFlyoutController_ShowTaskListButtonHoverFlyout_Old1_Original,
            HoverFlyoutController_ShowTaskListButtonHoverFlyout_Old1_Hook,
            true,
        },
        {
            {
                LR"(void __cdecl TaskbarLocationHelpers::ApplyTaskbarTooltipPlacement(struct winrt::Windows::UI::Xaml::Controls::ToolTip const &,enum winrt::WindowsUdk::UI::Shell::TaskbarLocation,struct winrt::Windows::Foundation::Size,struct winrt::Windows::Foundation::Size))",
                LR"(void __cdecl TaskbarLocationHelpers::ApplyTaskbarTooltipPlacement(struct winrt::Windows::UI::Xaml::Controls::ToolTip const &,enum winrt::WindowsUdk::Shell::TaskbarLocation,struct winrt::Windows::Foundation::Size,struct winrt::Windows::Foundation::Size))",
            },
            &ApplyTaskbarTooltipPlacement_View_Original,
            ApplyTaskbarTooltipPlacement_View_Hook,
            true,
        },
    };

    bool res = WindhawkUtils::HookSymbols(module, taskbarViewHooks,
                                          ARRAYSIZE(taskbarViewHooks));
    Wh_Log(L"> HookTaskbarViewSymbols result: %d", res ? 1 : 0);
    return res;
}

static bool HookTaskbarDllSymbols() {
    HMODULE module = LoadLibraryExW(L"taskbar.dll", nullptr,
                                    LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!module) {
        module = GetModuleHandleW(nullptr);
    }
    if (!module) {
        return false;
    }

    // explorer.exe, taskbar.dll
    WindhawkUtils::SYMBOL_HOOK hooks[] = {
        {
            {
                LR"(protected: long __cdecl CTaskListWnd::_DisplayExtendedUI(struct ITaskBtnGroup *,int,unsigned long,int))",
            },
            &CTaskListWnd__DisplayExtendedUI_Original,
            CTaskListWnd__DisplayExtendedUI_Hook,
            true,
        },
        {
            {
                LR"(private: int __cdecl CTaskListThumbnailWnd::_CanShowThumbnails(class CDPA<struct ITaskThumbnail,class CTContainer_PolicyUnOwned<struct ITaskThumbnail> > const *,int,int))",
            },
            &CTaskListThumbnailWnd__CanShowThumbnails_Original,
            CTaskListThumbnailWnd__CanShowThumbnails_Hook,
            true,
        },
    };

    return WindhawkUtils::HookSymbols(module, hooks, ARRAYSIZE(hooks));
}

static void HandleLoadedModule(HMODULE module) {
    if (!module) {
        return;
    }

    if (!g_systemTrayHooked && GetModuleHandleW(L"SystemTray.dll") == module) {
        if (!g_systemTrayHooked.exchange(true)) {
            Wh_Log(L"Loaded SystemTray.dll dynamically");
            HookSystemTraySymbols(module);
            Wh_ApplyHookOperations();
        }
    } else if (!g_taskbarViewHooked &&
               GetModuleHandleW(L"Taskbar.View.dll") == module) {
        if (!g_taskbarViewHooked.exchange(true)) {
            Wh_Log(L"Loaded Taskbar.View.dll dynamically");
            HookTaskbarViewSymbols(module);
            Wh_ApplyHookOperations();
        }
    }
}

using LoadLibraryExW_t = decltype(&LoadLibraryExW);
static LoadLibraryExW_t LoadLibraryExW_Original = nullptr;

static HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR lpLibFileName,
                                          HANDLE hFile,
                                          DWORD dwFlags) {
    HMODULE module = LoadLibraryExW_Original(lpLibFileName, hFile, dwFlags);
    if (module) {
        HandleLoadedModule(module);
    }
    return module;
}

BOOL Wh_ModInit() {
    Wh_Log(L"> Initializing Hide Taskbar Tooltips mod v" WH_MOD_VERSION);

    if (!IsWindows11OrGreater()) {
        Wh_Log(L"Hide Taskbar Tooltips: Only Windows 11 is supported");
        return FALSE;
    }

    LoadSettings();

    bool hookedAny = false;

    WindhawkUtils::SetFunctionHook(CreateWindowExW, CreateWindowExW_Hook,
                                   &CreateWindowExW_Original);

    HMODULE win32u = GetModuleHandleW(L"win32u.dll");
    if (win32u) {
        auto pNtUserShowWindow = reinterpret_cast<ShowWindow_t>(
            GetProcAddress(win32u, "NtUserShowWindow"));
        if (pNtUserShowWindow) {
            WindhawkUtils::SetFunctionHook(pNtUserShowWindow, ShowWindow_Hook,
                                           &ShowWindow_Original);
        }
    }
    if (!ShowWindow_Original) {
        WindhawkUtils::SetFunctionHook(ShowWindow, ShowWindow_Hook,
                                       &ShowWindow_Original);
    }

    WindhawkUtils::SetFunctionHook(SetWindowPos, SetWindowPos_Hook,
                                   &SetWindowPos_Original);

    if (HookTaskbarDllSymbols()) {
        hookedAny = true;
    }

    if (HMODULE systemTrayModule = GetModuleHandleW(L"SystemTray.dll")) {
        g_systemTrayHooked = true;
        if (HookSystemTraySymbols(systemTrayModule)) {
            hookedAny = true;
        }
    }

    if (HMODULE taskbarViewModule = GetModuleHandleW(L"Taskbar.View.dll")) {
        g_taskbarViewHooked = true;
        if (HookTaskbarViewSymbols(taskbarViewModule)) {
            hookedAny = true;
        }
    }

    bool waitingForModules = false;
    if (!g_systemTrayHooked || !g_taskbarViewHooked) {
        HMODULE kernelBaseModule = GetModuleHandleW(L"kernelbase.dll");
        if (kernelBaseModule) {
            auto pKernelBaseLoadLibraryExW =
                reinterpret_cast<decltype(&LoadLibraryExW)>(
                    GetProcAddress(kernelBaseModule, "LoadLibraryExW"));
            if (pKernelBaseLoadLibraryExW) {
                WindhawkUtils::SetFunctionHook(pKernelBaseLoadLibraryExW,
                                               LoadLibraryExW_Hook,
                                               &LoadLibraryExW_Original);
                waitingForModules = true;
            }
        }
    }

    return hookedAny || waitingForModules;
}

void Wh_ModAfterInit() {
    InitEagerToolTipHook();
}

void Wh_ModSettingsChanged() {
    Wh_Log(L"> Settings changed");
    LoadSettings();
}

void Wh_ModUninit() {
    Wh_Log(L"> Uninitializing Hide Taskbar Tooltips mod");
}
