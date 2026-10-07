// ==WindhawkMod==
// @id              hide-taskbar-tooltips
// @name            Hide Taskbar Tooltips
// @description     Suppresses native Windows 11 XAML hover tooltips in Explorer, with selective area filtering for taskbar apps and system tray icons.
// @version         1.1.0
// @author          gilnett
// @github          https://github.com/gilnett
// @include         explorer.exe
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

If you also wish to hide application window preview thumbnails, use the [Disable Taskbar Thumbnails](https://windhawk.net/mods/taskbar-thumbnails) mod, which can be used alongside this mod.

## Preview

**Before** (native Windows 11 tooltip on hover):  
![Before](https://i.imgur.com/XW2Ygxq.png)

**After** (tooltip suppressed):  
![After](https://i.imgur.com/IBPlzQf.png)

## Settings

- **Hide Taskbar Application Tooltips**: Suppresses hover tooltips and labels appearing over pinned or running taskbar application buttons.
- **Hide System Tray & Status Tooltips**: Suppresses hover tooltips over system status icons (Network, Volume, Battery, and Clock).

## Compatibility

- Only Windows 11 is supported.
- Supported Architectures: x86, x64, and ARM64.

## Support

If you find this mod useful, you can support its development and maintenance:
- [Support on Ko-fi](https://ko-fi.com/gilnet)
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- hideTaskbarAppTooltips: true
  $name: Hide Taskbar Application Tooltips
  $description: >-
    Suppresses hover tooltips and labels appearing over pinned or running
    taskbar application buttons.
- hideSystemTrayTooltips: true
  $name: Hide System Tray & Status Tooltips
  $description: >-
    Suppresses hover tooltips over system status icons (Network, Volume,
    Battery, and Clock).
*/
// ==/WindhawkModSettings==

#include <windhawk_api.h>
#include <windhawk_utils.h>

#include <windows.h>

#include <atomic>
#include <mutex>
#include <string_view>
#include <unordered_set>

#undef GetCurrentTime

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Media.h>

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
    bool hideTaskbarAppTooltips{true};
    bool hideSystemTrayTooltips{true};
} g_settings;

static void LoadSettings() {
    PCWSTR sApp = Wh_GetStringSetting(L"hideTaskbarAppTooltips");
    if (sApp) {
        g_settings.hideTaskbarAppTooltips =
            wcscmp(sApp, L"0") != 0 && _wcsicmp(sApp, L"false") != 0;
        Wh_FreeStringSetting(sApp);
    } else {
        g_settings.hideTaskbarAppTooltips = true;
    }

    PCWSTR sTray = Wh_GetStringSetting(L"hideSystemTrayTooltips");
    if (sTray) {
        g_settings.hideSystemTrayTooltips =
            wcscmp(sTray, L"0") != 0 && _wcsicmp(sTray, L"false") != 0;
        Wh_FreeStringSetting(sTray);
    } else {
        g_settings.hideSystemTrayTooltips = true;
    }
}

struct ToolTipScope {
    ToolTipScope() {
        t_tooltipScopeDepth++;
    }
    ~ToolTipScope() {
        t_tooltipScopeDepth--;
    }
};

enum class ToolTipArea {
    Unknown,
    TaskbarApp,
    SystemTray,
};

static std::unordered_set<void*> s_taskbarToolTips;
static std::unordered_set<void*> s_systemTrayToolTips;
static std::mutex s_toolTipsMutex;

static void RegisterToolTip(void* pToolTip, ToolTipArea area) {
    if (!pToolTip) {
        return;
    }
    std::lock_guard<std::mutex> lock(s_toolTipsMutex);
    if (area == ToolTipArea::TaskbarApp) {
        s_taskbarToolTips.insert(pToolTip);
    } else if (area == ToolTipArea::SystemTray) {
        s_systemTrayToolTips.insert(pToolTip);
    }
}

static ToolTipArea LookupRegisteredToolTip(void* pToolTip) {
    if (!pToolTip) {
        return ToolTipArea::Unknown;
    }
    std::lock_guard<std::mutex> lock(s_toolTipsMutex);
    if (s_taskbarToolTips.find(pToolTip) != s_taskbarToolTips.end()) {
        return ToolTipArea::TaskbarApp;
    }
    if (s_systemTrayToolTips.find(pToolTip) != s_systemTrayToolTips.end()) {
        return ToolTipArea::SystemTray;
    }
    return ToolTipArea::Unknown;
}

static ToolTipArea IdentifyCallerArea() {
    void* stack[32];
    USHORT frames = CaptureStackBackTrace(1, 32, stack, nullptr);
    HMODULE hSystemTray = GetModuleHandleW(L"SystemTray.dll");
    HMODULE hTaskbarView = GetModuleHandleW(L"Taskbar.View.dll");

    for (USHORT i = 0; i < frames; i++) {
        HMODULE hMod = nullptr;
        if (GetModuleHandleExW(
                GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                    GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                reinterpret_cast<LPCWSTR>(stack[i]),
                &hMod)) {
            if (hMod == hSystemTray) {
                return ToolTipArea::SystemTray;
            }
            if (hMod == hTaskbarView) {
                return ToolTipArea::TaskbarApp;
            }
        }
    }
    return ToolTipArea::Unknown;
}

static ToolTipArea IdentifyToolTipArea(void* pThis) {
    if (!pThis) {
        return ToolTipArea::TaskbarApp;
    }

    // 1. Check registered pointer cache
    ToolTipArea cached = LookupRegisteredToolTip(pThis);
    if (cached != ToolTipArea::Unknown) {
        return cached;
    }

    // 2. Check call stack caller module
    ToolTipArea callerArea = IdentifyCallerArea();
    if (callerArea != ToolTipArea::Unknown) {
        RegisterToolTip(pThis, callerArea);
        return callerArea;
    }

    // 3. Inspect PlacementTarget runtime class & visual tree
    try {
        winrt::Windows::UI::Xaml::Controls::ToolTip toolTip{nullptr};
        winrt::copy_from_abi(toolTip, pThis);
        if (toolTip) {
            winrt::Windows::UI::Xaml::UIElement target =
                toolTip.PlacementTarget();
            if (target) {
                winrt::Windows::UI::Xaml::DependencyObject current = target;
                for (int depth = 0; current && depth < 10; depth++) {
                    winrt::hstring name = winrt::get_class_name(current);
                    if (!name.empty()) {
                        std::wstring_view sv{name};
                        if (sv.find(L"SystemTray") != std::wstring_view::npos ||
                            sv.find(L"Tray") != std::wstring_view::npos ||
                            sv.find(L"Clock") != std::wstring_view::npos ||
                            sv.find(L"StatusArea") != std::wstring_view::npos) {
                            RegisterToolTip(pThis, ToolTipArea::SystemTray);
                            return ToolTipArea::SystemTray;
                        }
                        if (sv.find(L"Taskbar") != std::wstring_view::npos ||
                            sv.find(L"TaskList") != std::wstring_view::npos ||
                            sv.find(L"LaunchList") != std::wstring_view::npos) {
                            RegisterToolTip(pThis, ToolTipArea::TaskbarApp);
                            return ToolTipArea::TaskbarApp;
                        }
                    }
                    current = winrt::Windows::UI::Xaml::Media::
                        VisualTreeHelper::GetParent(current);
                }

                // 4. Physical screen coordinate fallback (System Tray is on the rightmost 30%)
                if (auto fe = target.try_as<
                              winrt::Windows::UI::Xaml::FrameworkElement>()) {
                    auto transform = fe.TransformToVisual(nullptr);
                    auto pt = transform.TransformPoint(
                        winrt::Windows::Foundation::Point{0, 0});
                    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
                    if (screenWidth > 0) {
                        ToolTipArea area = (pt.X > (screenWidth * 0.70f))
                                               ? ToolTipArea::SystemTray
                                               : ToolTipArea::TaskbarApp;
                        RegisterToolTip(pThis, area);
                        return area;
                    }
                }
            }
        }
    } catch (...) {
        Wh_Log(L"> IdentifyToolTipArea exception caught");
    }

    // Default fallback: In Explorer taskbar, any tooltip not explicitly
    // identified as System Tray belongs to taskbar apps / shell UI.
    return ToolTipArea::TaskbarApp;
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

    if (t_tooltipScopeDepth > 0) {
        bool isXamlPopup =
            (lpClassName &&
             (reinterpret_cast<ULONG_PTR>(lpClassName) > 0xFFFF) &&
             _wcsicmp(lpClassName, L"Xaml_WindowedPopupClass") == 0);

        if (isXamlPopup) {
            Wh_Log(L"> CreateWindowExW in tooltip scope: class=%s, style=0x%X",
                   lpClassName, dwStyle);
            dwStyle &= ~WS_VISIBLE;
        }
    }
    return CreateWindowExW_Original(dwExStyle, lpClassName, lpWindowName,
                                    dwStyle, X, Y, nWidth, nHeight,
                                    hWndParent, hMenu, hInstance, lpParam);
}

using ShowWindow_t = decltype(&ShowWindow);
static ShowWindow_t ShowWindow_Original = nullptr;

static BOOL WINAPI ShowWindow_Hook(HWND hWnd, int nCmdShow) {
    DWORD taskbarThreadId = g_taskbarThreadId.load(std::memory_order_relaxed);
    if (taskbarThreadId && GetCurrentThreadId() != taskbarThreadId) {
        return ShowWindow_Original(hWnd, nCmdShow);
    }

    if (t_tooltipScopeDepth > 0) {
        Wh_Log(L"> ShowWindow suppressed in tooltip scope for %p, cmd=%d", hWnd,
               nCmdShow);
        if (nCmdShow != SW_HIDE) {
            return FALSE;
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
        if (uFlags & SWP_SHOWWINDOW) {
            Wh_Log(
                L"> SetWindowPos SWP_SHOWWINDOW stripped in tooltip scope for "
                L"%p",
                hWnd);
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
        // Fast paths based on settings
        if (g_settings.hideTaskbarAppTooltips &&
            g_settings.hideSystemTrayTooltips) {
            // Both suppressed: identical to v1.0.9, suppress with zero overhead
            return S_OK;
        }

        if (!g_settings.hideTaskbarAppTooltips &&
            !g_settings.hideSystemTrayTooltips) {
            // Both unsuppressed: allow opening
            if (g_toolTipPutIsOpenOriginal) {
                return g_toolTipPutIsOpenOriginal(pThis, value);
            }
            return S_OK;
        }

        // Selective case: one enabled, one disabled
        ToolTipArea area = IdentifyToolTipArea(pThis);
        if (area == ToolTipArea::SystemTray) {
            if (g_settings.hideSystemTrayTooltips) {
                return S_OK;
            }
        } else {
            // TaskbarApp and other shell taskbar UI elements
            if (g_settings.hideTaskbarAppTooltips) {
                return S_OK;
            }
        }
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

static LRESULT CALLBACK CallWndProcHook(int nCode,
                                        WPARAM wParam,
                                        LPARAM lParam) {
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
    DWORD_PTR dwResult = 0;
    SendMessageTimeoutW(hWnd, GetRunFromWindowThreadMessage(), 0,
                        reinterpret_cast<LPARAM>(&param),
                        SMTO_ABORTIFHUNG | SMTO_NORMAL, 2000, &dwResult);

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
                    Wh_Log(
                        L"> Successfully hooked ToolTip::put_IsOpen eagerly on "
                        L"Taskbar UI thread!");
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
        Wh_Log(
            L"> Dispatching eager ToolTip hook to Taskbar window %p (thread %u)",
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
    void* pIToolTip =
        pToolTip ? *(reinterpret_cast<void**>(pToolTip)) : nullptr;
    if (pIToolTip) {
        RegisterToolTip(pIToolTip, ToolTipArea::SystemTray);
    }
    if (!g_settings.hideSystemTrayTooltips) {
        if (PositionTaskbarTooltip_Original) {
            PositionTaskbarTooltip_Original(pToolTip, pTarget, location, b1,
                                            b2);
        }
        return;
    }
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
    void* pIToolTip =
        pToolTip ? *(reinterpret_cast<void**>(pToolTip)) : nullptr;
    if (pIToolTip) {
        RegisterToolTip(pIToolTip, ToolTipArea::SystemTray);
    }
    if (!g_settings.hideSystemTrayTooltips) {
        if (ApplyTaskbarTooltipPlacement_Tray_Original) {
            ApplyTaskbarTooltipPlacement_Tray_Original(pToolTip, location,
                                                       size1, size2);
        }
        return;
    }
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
    void* pIToolTip =
        pToolTip ? *(reinterpret_cast<void**>(pToolTip)) : nullptr;
    if (pIToolTip) {
        RegisterToolTip(pIToolTip, ToolTipArea::TaskbarApp);
    }
    if (!g_settings.hideTaskbarAppTooltips) {
        if (ApplyTaskbarTooltipPlacement_View_Original) {
            ApplyTaskbarTooltipPlacement_View_Original(pToolTip, location,
                                                       size1, size2);
        }
        return;
    }
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
    void* pIToolTip =
        pToolTip ? *(reinterpret_cast<void**>(pToolTip)) : nullptr;
    if (pIToolTip) {
        RegisterToolTip(pIToolTip, ToolTipArea::TaskbarApp);
    }
    if (!g_settings.hideTaskbarAppTooltips) {
        if (ApplyTaskbarTooltipPlacement6_Original) {
            ApplyTaskbarTooltipPlacement6_Original(pToolTip, location, size1,
                                                   size2, b1, b2);
        }
        return;
    }
    ToolTipScope scope;
    if (!SuppressToolTip(pToolTip) && ApplyTaskbarTooltipPlacement6_Original) {
        ApplyTaskbarTooltipPlacement6_Original(pToolTip, location, size1, size2,
                                               b1, b2);
    }
}

// Hover state hooks in Taskbar.View.dll
using ExperienceToggleButton_UpdateHover_t = void(__cdecl*)(void* pThis,
                                                            bool isHover);
static ExperienceToggleButton_UpdateHover_t
    ExperienceToggleButton_UpdateHover_Original = nullptr;

static void __cdecl ExperienceToggleButton_UpdateHover_Hook(void* pThis,
                                                            bool isHover) {
    if (!g_settings.hideTaskbarAppTooltips) {
        if (ExperienceToggleButton_UpdateHover_Original) {
            ExperienceToggleButton_UpdateHover_Original(pThis, isHover);
        }
    }
}

using TaskListButton_UpdateHover_t = void(__cdecl*)(void* pThis);
static TaskListButton_UpdateHover_t TaskListButton_UpdateHover_Original =
    nullptr;

static void __cdecl TaskListButton_UpdateHover_Hook(void* pThis) {
    if (!g_settings.hideTaskbarAppTooltips) {
        if (TaskListButton_UpdateHover_Original) {
            TaskListButton_UpdateHover_Original(pThis);
        }
    }
}

using OverflowToggleButton_UpdateHover_t = void(__cdecl*)(void* pThis);
static OverflowToggleButton_UpdateHover_t
    OverflowToggleButton_UpdateHover_Original = nullptr;

static void __cdecl OverflowToggleButton_UpdateHover_Hook(void* pThis) {
    if (!g_settings.hideTaskbarAppTooltips) {
        if (OverflowToggleButton_UpdateHover_Original) {
            OverflowToggleButton_UpdateHover_Original(pThis);
        }
    }
}

// Hover state hook in SystemTray.dll
using IconView_UpdateOuterToolTipPlacement_t = void(__cdecl*)(void* pThis,
                                                              bool isHover);
static IconView_UpdateOuterToolTipPlacement_t
    IconView_UpdateOuterToolTipPlacement_Original = nullptr;

static void __cdecl IconView_UpdateOuterToolTipPlacement_Hook(void* pThis,
                                                              bool isHover) {
    if (!g_settings.hideSystemTrayTooltips) {
        if (IconView_UpdateOuterToolTipPlacement_Original) {
            IconView_UpdateOuterToolTipPlacement_Original(pThis, isHover);
        }
    }
}

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
                LR"(void __cdecl TaskbarLocationHelpers::ApplyTaskbarTooltipPlacement(struct winrt::Windows::UI::Xaml::Controls::ToolTip const &,enum winrt::WindowsUdk::UI::Shell::TaskbarLocation,struct winrt::Windows::Foundation::Size,struct winrt::Windows::Foundation::Size,bool,bool))",
                LR"(void __cdecl TaskbarLocationHelpers::ApplyTaskbarTooltipPlacement(struct winrt::Windows::UI::Xaml::Controls::ToolTip const &,enum winrt::WindowsUdk::Shell::TaskbarLocation,struct winrt::Windows::Foundation::Size,struct winrt::Windows::Foundation::Size,bool,bool))",
            },
            &ApplyTaskbarTooltipPlacement6_Original,
            ApplyTaskbarTooltipPlacement6_Hook,
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

    HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();
    if (hTaskbarWnd) {
        g_taskbarThreadId.store(GetWindowThreadProcessId(hTaskbarWnd, nullptr),
                                std::memory_order_relaxed);
    }

    bool hookedAny = false;

    WindhawkUtils::SetFunctionHook(CreateWindowExW, CreateWindowExW_Hook,
                                   &CreateWindowExW_Original);
    WindhawkUtils::SetFunctionHook(ShowWindow, ShowWindow_Hook,
                                   &ShowWindow_Original);
    WindhawkUtils::SetFunctionHook(SetWindowPos, SetWindowPos_Hook,
                                   &SetWindowPos_Original);

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
    Wh_Log(L"> Hide Taskbar Tooltips active — SystemTray: %d, TaskbarView: %d",
           (int)g_systemTrayHooked.load(), (int)g_taskbarViewHooked.load());
}

void Wh_ModSettingsChanged() {
    Wh_Log(L"> Settings changed");
    LoadSettings();
    Wh_Log(L"> Updated settings: hideTaskbarApp=%d, hideSystemTray=%d",
           (int)g_settings.hideTaskbarAppTooltips,
           (int)g_settings.hideSystemTrayTooltips);
}

void Wh_ModUninit() {
    Wh_Log(L"> Uninitializing Hide Taskbar Tooltips mod");
}
