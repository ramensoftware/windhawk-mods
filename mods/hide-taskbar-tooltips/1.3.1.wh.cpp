// ==WindhawkMod==
// @id              hide-taskbar-tooltips
// @name            Hide Taskbar Tooltips
// @description     Suppresses native Windows 11 XAML hover tooltips in Explorer, with selective area filtering and optional delayed hover display.
// @version         1.3.1
// @author          gilnett
// @github          https://github.com/gilnett
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lruntimeobject
// @donateUrl       https://buymeacoffee.com/gilnett
// @license         GPL-3.0
// ==/WindhawkMod==

// Source code is published under The GNU General Public License v3.0.

// ==WindhawkModReadme==
/*
# Hide Taskbar Tooltips

Suppresses native Windows 11 XAML hover tooltips in Explorer, covering:
- Taskbar app icon tooltips and labels on hover
- Windows Start button hover tooltip
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
- **Hide Start Button Tooltip**: Suppresses the hover tooltip appearing over the Windows Start button.
- **Hide System Tray & Status Tooltips**: Suppresses hover tooltips over system status icons (Network, Volume, Battery, and Clock).
- **Hide System Tray Overflow Tooltips**: Suppresses hover tooltips for hidden application icons inside the system tray overflow drawer. When turned off, tooltips remain visible for apps in the drawer.
- **Additional Hover Delay for Visible Tooltips (ms)**: Adds an extra stationary hover delay in milliseconds before displaying tooltips for areas that are **not** hidden (`0` = native timing). Areas configured to be hidden remain permanently suppressed and are unaffected by this delay.

## Compatibility

- Only Windows 11 is supported.

## Support

If you find this mod useful, you can support its development and maintenance:
- [Support on Buy Me a Coffee](https://buymeacoffee.com/gilnett)
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- hideTaskbarAppTooltips: true
  $name: Hide Taskbar Application Tooltips
  $description: >-
    Suppresses hover tooltips and labels appearing over pinned or running
    taskbar application buttons.
- hideStartButtonTooltip: true
  $name: Hide Start Button Tooltip
  $description: >-
    Suppresses the hover tooltip appearing over the Windows Start button.
- hideSystemTrayTooltips: true
  $name: Hide System Tray & Status Tooltips
  $description: >-
    Suppresses hover tooltips over system status icons (Network, Volume,
    Battery, and Clock).
- hideTrayOverflowTooltips: true
  $name: Hide System Tray Overflow Tooltips
  $description: >-
    Suppresses hover tooltips for hidden application icons inside the system
    tray overflow drawer. When turned off, tooltips remain visible for apps in
    the drawer.
- tooltipHoverDelayMs: 0
  $name: Additional Hover Delay for Visible Tooltips (ms)
  $description: >-
    Adds an extra stationary hover delay in milliseconds before displaying
    tooltips that are not hidden (0 = native timing). Hidden tooltip areas
    remain permanently suppressed.
*/
// ==/WindhawkModSettings==

#include <windhawk_api.h>
#include <windhawk_utils.h>

#include <inspectable.h>
#include <roapi.h>
#include <weakreference.h>
#include <windows.h>
#include <winstring.h>
#include <wrl/client.h>
#include <wrl/wrappers/corewrappers.h>

#include <algorithm>
#include <atomic>
#include <cstdint>
#include <cstdlib>
#include <mutex>
#include <string_view>
#include <unordered_set>

using Microsoft::WRL::ComPtr;
using Microsoft::WRL::Wrappers::HString;
using Microsoft::WRL::Wrappers::HStringReference;

struct WinrtSize {
    float Width;
    float Height;
};

struct WinrtPoint {
    float X;
    float Y;
};

struct WinrtRect {
    float X;
    float Y;
    float Width;
    float Height;
};

// {5C526665-F60E-4912-AF59-5FE0680F089D}
static constexpr GUID kIID_IDependencyObject = {
    0x5C526665,
    0xF60E,
    0x4912,
    {0xAF, 0x59, 0x5F, 0xE0, 0x68, 0x0F, 0x08, 0x9D}};

// {7DE5D75F-4415-429F-B523-699BDB8C30DB}
static constexpr GUID kIID_IToolTip = {
    0x7DE5D75F,
    0x4415,
    0x429F,
    {0xB5, 0x23, 0x69, 0x9B, 0xDB, 0x8C, 0x30, 0xDB}};

// {89839403-B538-49E3-A430-3AC037DC6FE0}
static constexpr GUID kIID_IToolTipFactory = {
    0x89839403,
    0xB538,
    0x49E3,
    {0xA4, 0x30, 0x3A, 0xC0, 0x37, 0xDC, 0x6F, 0xE0}};

// {A26DD1DC-CD44-435C-BE94-01D6241C231C}
static constexpr GUID kIID_IContentControl = {
    0xA26DD1DC,
    0xCD44,
    0x435C,
    {0xBE, 0x94, 0x01, 0xD6, 0x24, 0x1C, 0x23, 0x1C}};

// {E75758C4-D25D-4B1D-971F-596F17F12BAA}
static constexpr GUID kIID_IVisualTreeHelperStatics = {
    0xE75758C4,
    0xD25D,
    0x4B1D,
    {0x97, 0x1F, 0x59, 0x6F, 0x17, 0xF1, 0x2B, 0xAA}};

// {86E649F8-E245-48AA-A8C8-D1073ED76319}
static constexpr GUID kIID_IToolTipServiceStatics = {
    0x86E649F8,
    0xE245,
    0x48AA,
    {0xA8, 0xC8, 0xD1, 0x07, 0x3E, 0xD7, 0x63, 0x19}};

struct IToolTipAbi : public IInspectable {
    virtual HRESULT STDMETHODCALLTYPE get_HorizontalOffset(double* value) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_HorizontalOffset(double value) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_IsOpen(boolean* value) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_IsOpen(boolean value) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Placement(int32_t* value) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_Placement(int32_t value) = 0;
    virtual HRESULT STDMETHODCALLTYPE
    get_PlacementTarget(IInspectable** value) = 0;
};

struct IToolTipFactoryAbi : public IInspectable {
    virtual HRESULT STDMETHODCALLTYPE
    CreateInstance(IInspectable* baseInterface,
                   IInspectable** innerInterface,
                   IToolTipAbi** value) = 0;
};

struct IContentControlAbi : public IInspectable {
    virtual HRESULT STDMETHODCALLTYPE get_Content(IInspectable** value) = 0;
};

struct IToolTipServiceStaticsAbi : public IInspectable {
    virtual HRESULT STDMETHODCALLTYPE
    get_PlacementProperty(IInspectable** value) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetPlacement(IInspectable* element,
                                                   int32_t* value) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetPlacement(IInspectable* element,
                                                   int32_t value) = 0;
    virtual HRESULT STDMETHODCALLTYPE
    get_PlacementTargetProperty(IInspectable** value) = 0;
    virtual HRESULT STDMETHODCALLTYPE
    GetPlacementTarget(IInspectable* element, IInspectable** value) = 0;
    virtual HRESULT STDMETHODCALLTYPE
    SetPlacementTarget(IInspectable* element, IInspectable* value) = 0;
    virtual HRESULT STDMETHODCALLTYPE
    get_ToolTipProperty(IInspectable** value) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetToolTip(IInspectable* element,
                                                 IInspectable** value) = 0;
};

struct IVisualTreeHelperStaticsAbi : public IInspectable {
    virtual HRESULT STDMETHODCALLTYPE
    FindElementsInHostCoordinatesPoint(WinrtPoint intersectingPoint,
                                       IInspectable* subtree,
                                       IInspectable** elements) = 0;
    virtual HRESULT STDMETHODCALLTYPE
    FindElementsInHostCoordinatesRect(WinrtRect intersectingRect,
                                      IInspectable* subtree,
                                      IInspectable** elements) = 0;
    virtual HRESULT STDMETHODCALLTYPE
    FindAllElementsInHostCoordinatesPoint(WinrtPoint intersectingPoint,
                                          IInspectable* subtree,
                                          boolean includeAllElements,
                                          IInspectable** elements) = 0;
    virtual HRESULT STDMETHODCALLTYPE
    FindAllElementsInHostCoordinatesRect(WinrtRect intersectingRect,
                                         IInspectable* subtree,
                                         boolean includeAllElements,
                                         IInspectable** elements) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetChild(IInspectable* reference,
                                               int32_t childIndex,
                                               IInspectable** child) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetChildrenCount(IInspectable* reference,
                                                       int32_t* count) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetParent(IInspectable* reference,
                                                IInspectable** parent) = 0;
};

// {B618FD7B-32D0-4970-9C42-7C039AC7BE78}
static constexpr GUID kIID_IAutomationPropertiesStatics = {
    0xB618FD7B,
    0x32D0,
    0x4970,
    {0x9C, 0x42, 0x7C, 0x03, 0x9A, 0xC7, 0xBE, 0x78}};

struct IAutomationPropertiesStaticsAbi : public IInspectable {
    virtual HRESULT STDMETHODCALLTYPE
    get_AcceleratorKeyProperty(void** value) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetAcceleratorKey(void* element,
                                                        HSTRING* result) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetAcceleratorKey(void* element,
                                                        HSTRING value) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_AccessKeyProperty(void** value) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetAccessKey(void* element,
                                                   HSTRING* result) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetAccessKey(void* element,
                                                   HSTRING value) = 0;
    virtual HRESULT STDMETHODCALLTYPE
    get_AutomationIdProperty(void** value) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetAutomationId(void* element,
                                                      HSTRING* result) = 0;
};

static HString GetRuntimeClassName(IUnknown* pUnk) {
    HString str;
    if (!pUnk) {
        return str;
    }
    ComPtr<IInspectable> spInsp;
    if (SUCCEEDED(pUnk->QueryInterface(__uuidof(IInspectable), &spInsp)) &&
        spInsp) {
        spInsp->GetRuntimeClassName(str.GetAddressOf());
    }
    return str;
}

static std::wstring_view GetStringView(const HString& str) {
    unsigned int len = 0;
    const wchar_t* raw = str.GetRawBuffer(&len);
    return raw ? std::wstring_view(raw, len) : std::wstring_view{};
}

static ComPtr<IVisualTreeHelperStaticsAbi> GetVisualTreeHelperStatics() {
    ComPtr<IVisualTreeHelperStaticsAbi> spStatics;
    RoGetActivationFactory(
        HStringReference(L"Windows.UI.Xaml.Media.VisualTreeHelper").Get(),
        kIID_IVisualTreeHelperStatics, &spStatics);
    return spStatics;
}

static ComPtr<IInspectable> GetVisualTreeParent(
    IVisualTreeHelperStaticsAbi* pHelper, IUnknown* pElement) {
    ComPtr<IInspectable> spParent;
    if (!pHelper || !pElement) {
        return spParent;
    }
    ComPtr<IInspectable> spDepObj;
    if (FAILED(pElement->QueryInterface(kIID_IDependencyObject, &spDepObj)) ||
        !spDepObj) {
        return spParent;
    }
    pHelper->GetParent(spDepObj.Get(), &spParent);
    return spParent;
}

static bool IsElementStartButton(IUnknown* pElement) {
    if (!pElement) {
        return false;
    }
    ComPtr<IInspectable> spDepObj;
    if (FAILED(pElement->QueryInterface(kIID_IDependencyObject, &spDepObj)) ||
        !spDepObj) {
        return false;
    }
    ComPtr<IAutomationPropertiesStaticsAbi> spAutomationStatics;
    if (FAILED(RoGetActivationFactory(
            HStringReference(
                L"Windows.UI.Xaml.Automation.AutomationProperties")
                .Get(),
            kIID_IAutomationPropertiesStatics, &spAutomationStatics)) ||
        !spAutomationStatics) {
        return false;
    }
    HString autoId;
    if (SUCCEEDED(spAutomationStatics->GetAutomationId(spDepObj.Get(),
                                                       autoId.GetAddressOf()))) {
        unsigned int len = 0;
        const wchar_t* raw = autoId.GetRawBuffer(&len);
        if (raw && std::wstring_view(raw, len) == L"StartButton") {
            return true;
        }
    }
    return false;
}

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
static std::atomic<HWND> g_taskbarWnd{nullptr};
static std::atomic<bool> g_uninitializing{false};
static thread_local int t_tooltipScopeDepth = 0;
static thread_local int t_startButtonHoverDepth = 0;
static thread_local bool t_allowingDelayedOpen = false;

struct Settings {
    std::atomic<bool> hideTaskbarAppTooltips{true};
    std::atomic<bool> hideStartButtonTooltip{true};
    std::atomic<bool> hideSystemTrayTooltips{true};
    std::atomic<bool> hideTrayOverflowTooltips{true};
    std::atomic<int> tooltipHoverDelayMs{0};
} g_settings;

static void LoadSettings() {
    g_settings.hideTaskbarAppTooltips.store(
        Wh_GetIntSetting(L"hideTaskbarAppTooltips") != 0,
        std::memory_order_relaxed);
    g_settings.hideStartButtonTooltip.store(
        Wh_GetIntSetting(L"hideStartButtonTooltip") != 0,
        std::memory_order_relaxed);
    g_settings.hideSystemTrayTooltips.store(
        Wh_GetIntSetting(L"hideSystemTrayTooltips") != 0,
        std::memory_order_relaxed);
    g_settings.hideTrayOverflowTooltips.store(
        Wh_GetIntSetting(L"hideTrayOverflowTooltips") != 0,
        std::memory_order_relaxed);

    int delayMs = Wh_GetIntSetting(L"tooltipHoverDelayMs");
    delayMs = std::clamp(delayMs, 0, 60000);
    g_settings.tooltipHoverDelayMs.store(delayMs, std::memory_order_relaxed);
}

struct ToolTipScope {
    ToolTipScope() {
        t_tooltipScopeDepth++;
    }
    ~ToolTipScope() {
        t_tooltipScopeDepth--;
    }
};

struct StartButtonHoverScope {
    StartButtonHoverScope() {
        t_startButtonHoverDepth++;
    }
    ~StartButtonHoverScope() {
        t_startButtonHoverDepth--;
    }
};

enum class ToolTipArea {
    Unknown,
    TaskbarApp,
    StartButton,
    SystemTray,
    TrayOverflow,
};

static std::unordered_set<void*> s_taskbarToolTips;
static std::unordered_set<void*> s_startButtonToolTips;
static std::unordered_set<void*> s_systemTrayToolTips;
static std::unordered_set<void*> s_trayOverflowToolTips;
static std::mutex s_toolTipsMutex;

static bool IsAreaSuppressed(ToolTipArea area) {
    if (area == ToolTipArea::TrayOverflow) {
        return g_settings.hideTrayOverflowTooltips.load(
            std::memory_order_relaxed);
    }
    if (area == ToolTipArea::SystemTray) {
        return g_settings.hideSystemTrayTooltips.load(
            std::memory_order_relaxed);
    }
    if (area == ToolTipArea::StartButton) {
        return g_settings.hideStartButtonTooltip.load(
            std::memory_order_relaxed);
    }
    return g_settings.hideTaskbarAppTooltips.load(std::memory_order_relaxed);
}

static void RegisterToolTip(void* pToolTip, ToolTipArea area) {
    if (!pToolTip || area == ToolTipArea::Unknown) {
        return;
    }
    std::lock_guard<std::mutex> lock(s_toolTipsMutex);
    constexpr size_t kMaxTrackedToolTips = 500;
    if (area == ToolTipArea::TaskbarApp) {
        s_startButtonToolTips.erase(pToolTip);
        if (s_taskbarToolTips.size() >= kMaxTrackedToolTips) {
            s_taskbarToolTips.clear();
        }
        s_taskbarToolTips.insert(pToolTip);
    } else if (area == ToolTipArea::StartButton) {
        s_taskbarToolTips.erase(pToolTip);
        if (s_startButtonToolTips.size() >= kMaxTrackedToolTips) {
            s_startButtonToolTips.clear();
        }
        s_startButtonToolTips.insert(pToolTip);
    } else if (area == ToolTipArea::SystemTray) {
        s_trayOverflowToolTips.erase(pToolTip);
        if (s_systemTrayToolTips.size() >= kMaxTrackedToolTips) {
            s_systemTrayToolTips.clear();
        }
        s_systemTrayToolTips.insert(pToolTip);
    } else if (area == ToolTipArea::TrayOverflow) {
        s_systemTrayToolTips.erase(pToolTip);
        if (s_trayOverflowToolTips.size() >= kMaxTrackedToolTips) {
            s_trayOverflowToolTips.clear();
        }
        s_trayOverflowToolTips.insert(pToolTip);
    }
}

static ToolTipArea LookupRegisteredToolTip(void* pToolTip) {
    if (!pToolTip) {
        return ToolTipArea::Unknown;
    }
    std::lock_guard<std::mutex> lock(s_toolTipsMutex);
    if (s_trayOverflowToolTips.find(pToolTip) != s_trayOverflowToolTips.end()) {
        return ToolTipArea::TrayOverflow;
    }
    if (s_startButtonToolTips.find(pToolTip) != s_startButtonToolTips.end()) {
        return ToolTipArea::StartButton;
    }
    if (s_taskbarToolTips.find(pToolTip) != s_taskbarToolTips.end()) {
        return ToolTipArea::TaskbarApp;
    }
    if (s_systemTrayToolTips.find(pToolTip) != s_systemTrayToolTips.end()) {
        return ToolTipArea::SystemTray;
    }
    return ToolTipArea::Unknown;
}

static bool IsInNotificationAreaOverflow(IVisualTreeHelperStaticsAbi* pHelper,
                                         IUnknown* pElement) {
    if (!pHelper || !pElement) {
        return false;
    }
    ComPtr<IInspectable> current;
    if (FAILED(pElement->QueryInterface(__uuidof(IInspectable), &current))) {
        return false;
    }
    for (int depth = 0; current && depth < 24;
         depth++, current = GetVisualTreeParent(pHelper, current.Get())) {
        if (GetStringView(GetRuntimeClassName(current.Get())) ==
            L"SystemTray.NotificationAreaOverflow") {
            return true;
        }
    }
    return false;
}

static bool IsInNotificationAreaOverflow(IUnknown* pElement) {
    ComPtr<IVisualTreeHelperStaticsAbi> spVisualHelper =
        GetVisualTreeHelperStatics();
    return IsInNotificationAreaOverflow(spVisualHelper.Get(), pElement);
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

static ToolTipArea InspectPlacementTargetArea(void* pIToolTip) {
    if (!pIToolTip) {
        return ToolTipArea::Unknown;
    }
    try {
        auto* pUnk = static_cast<IUnknown*>(pIToolTip);
        ComPtr<IToolTipAbi> spToolTip;
        if (FAILED(pUnk->QueryInterface(kIID_IToolTip, &spToolTip)) ||
            !spToolTip) {
            return ToolTipArea::Unknown;
        }

        ComPtr<IInspectable> target;
        if (SUCCEEDED(spToolTip->get_PlacementTarget(&target)) && target) {
            ComPtr<IVisualTreeHelperStaticsAbi> spVisualHelper =
                GetVisualTreeHelperStatics();
            if (IsInNotificationAreaOverflow(spVisualHelper.Get(),
                                             target.Get())) {
                return ToolTipArea::TrayOverflow;
            }

            ComPtr<IInspectable> current = target;
            for (int depth = 0; current && depth < 16; depth++) {
                HString name = GetRuntimeClassName(current.Get());
                std::wstring_view sv = GetStringView(name);
                if (!sv.empty()) {
                    if (sv.find(L"ExperienceToggleButton") !=
                        std::wstring_view::npos) {
                        return IsElementStartButton(current.Get())
                                   ? ToolTipArea::StartButton
                                   : ToolTipArea::TaskbarApp;
                    }
                    if (sv.find(L"TaskListButton") != std::wstring_view::npos ||
                        sv.find(L"OverflowToggleButton") !=
                            std::wstring_view::npos) {
                        return ToolTipArea::TaskbarApp;
                    }
                    if (sv.find(L"SystemTray") != std::wstring_view::npos ||
                        sv.find(L"Tray") != std::wstring_view::npos ||
                        sv.find(L"Clock") != std::wstring_view::npos ||
                        sv.find(L"StatusArea") != std::wstring_view::npos) {
                        return ToolTipArea::SystemTray;
                    }
                    if (sv.find(L"Taskbar") != std::wstring_view::npos) {
                        return ToolTipArea::TaskbarApp;
                    }
                }
                current =
                    GetVisualTreeParent(spVisualHelper.Get(), current.Get());
            }
        }

        // In Taskbar.View.dll, PlacementTarget() is null when CToolTipService
        // invokes ToolTip::put_IsOpen on a timer. TaskListButton sets Content()
        // to a StackPanel.
        ComPtr<IContentControlAbi> spContentCtrl;
        if (SUCCEEDED(
                pUnk->QueryInterface(kIID_IContentControl, &spContentCtrl)) &&
            spContentCtrl) {
            ComPtr<IInspectable> content;
            if (SUCCEEDED(spContentCtrl->get_Content(&content)) && content) {
                HString contentClass = GetRuntimeClassName(content.Get());
                std::wstring_view csv = GetStringView(contentClass);
                if (csv == L"Windows.UI.Xaml.Controls.StackPanel") {
                    return ToolTipArea::TaskbarApp;
                }
            }
        }
    } catch (...) {
        Wh_Log(L"[TOOLTIP] InspectPlacementTargetArea caught exception");
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

    if (t_startButtonHoverDepth > 0) {
        RegisterToolTip(pThis, ToolTipArea::StartButton);
        return ToolTipArea::StartButton;
    }

    // 2. Inspect PlacementTarget runtime class & visual tree
    ToolTipArea targetArea = InspectPlacementTargetArea(pThis);
    if (targetArea != ToolTipArea::Unknown) {
        RegisterToolTip(pThis, targetArea);
        return targetArea;
    }

    // 3. Fallback: check call stack caller module
    ToolTipArea callerArea = IdentifyCallerArea();
    if (callerArea != ToolTipArea::Unknown) {
        RegisterToolTip(pThis, callerArea);
        return callerArea;
    }

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
            Wh_Log(
                L"[POPUP] CreateWindowExW in tooltip scope (class=%s, "
                L"style=0x%X)",
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
        Wh_Log(L"[POPUP] ShowWindow suppressed in tooltip scope (hwnd=%p, "
               L"cmd=%d)",
               hWnd, nCmdShow);
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
                L"[POPUP] SetWindowPos SWP_SHOWWINDOW stripped in tooltip "
                L"scope (hwnd=%p)",
                hWnd);
            uFlags &= ~SWP_SHOWWINDOW;
            uFlags |= SWP_HIDEWINDOW;
        }
    }
    return SetWindowPos_Original(hWnd, hWndInsertAfter, X, Y, cx, cy, uFlags);
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

static HWND GetTaskbarWndForTimer() {
    HWND hWnd = g_taskbarWnd.load(std::memory_order_relaxed);
    if (hWnd && IsWindow(hWnd)) {
        return hWnd;
    }
    hWnd = FindCurrentProcessTaskbarWnd();
    if (hWnd) {
        g_taskbarWnd.store(hWnd, std::memory_order_relaxed);
        DWORD threadId = GetWindowThreadProcessId(hWnd, nullptr);
        if (threadId) {
            g_taskbarThreadId.store(threadId, std::memory_order_relaxed);
        }
    }
    return hWnd;
}

// WinRT IToolTip ABI: put_IsOpen is at vtable index 9
using ToolTip_put_IsOpen_t = HRESULT(__stdcall*)(void* pThis, boolean value);
static ToolTip_put_IsOpen_t g_toolTipPutIsOpenOriginal = nullptr;
static std::atomic<bool> g_putIsOpenHooked{false};

constexpr UINT_PTR kDelayedTooltipOpenTimerId = 0x57485401;
constexpr UINT_PTR kDelayedTooltipWatchTimerId = 0x57485402;
constexpr UINT kDelayedTooltipWatchIntervalMs = 100;
constexpr int kCursorDriftTolerancePx = 12;

struct DelayedToolTipState {
    ComPtr<IWeakReference> pendingToolTip;
    ComPtr<IWeakReference> activeToolTip;
    void* pendingRawPtr = nullptr;
    void* activeRawPtr = nullptr;
    POINT initialPt{0, 0};
    bool openTimerArmed = false;
    bool watchTimerArmed = false;
};
static DelayedToolTipState g_delayedState;

static ComPtr<IWeakReference> MakeWeakToolTip(IUnknown* pUnk) {
    ComPtr<IWeakReference> spWeak;
    if (!pUnk) {
        return spWeak;
    }
    ComPtr<IWeakReferenceSource> spSource;
    if (SUCCEEDED(
            pUnk->QueryInterface(__uuidof(IWeakReferenceSource), &spSource)) &&
        spSource) {
        spSource->GetWeakReference(&spWeak);
    }
    return spWeak;
}

static ComPtr<IToolTipAbi> ResolveWeakToolTip(IWeakReference* pWeak) {
    ComPtr<IToolTipAbi> spToolTip;
    if (!pWeak) {
        return spToolTip;
    }
    ComPtr<IInspectable> spInsp;
    if (SUCCEEDED(pWeak->Resolve(kIID_IToolTip, &spInsp)) && spInsp) {
        spInsp->QueryInterface(kIID_IToolTip, &spToolTip);
    }
    return spToolTip;
}

static void CancelDelayedOpenTimer() {
    HWND hWnd = g_taskbarWnd.load(std::memory_order_relaxed);
    if (g_delayedState.openTimerArmed && hWnd && IsWindow(hWnd)) {
        KillTimer(hWnd, kDelayedTooltipOpenTimerId);
    }
    g_delayedState.openTimerArmed = false;
    g_delayedState.pendingToolTip.Reset();
    g_delayedState.pendingRawPtr = nullptr;
}

static void CloseActiveDelayedToolTip() {
    HWND hWnd = g_taskbarWnd.load(std::memory_order_relaxed);
    if (g_delayedState.watchTimerArmed && hWnd && IsWindow(hWnd)) {
        KillTimer(hWnd, kDelayedTooltipWatchTimerId);
    }
    g_delayedState.watchTimerArmed = false;
    ComPtr<IToolTipAbi> spActive =
        ResolveWeakToolTip(g_delayedState.activeToolTip.Get());
    g_delayedState.activeToolTip.Reset();
    g_delayedState.activeRawPtr = nullptr;
    if (spActive && g_toolTipPutIsOpenOriginal) {
        g_toolTipPutIsOpenOriginal(spActive.Get(), FALSE);
    }
}

static void CALLBACK DelayedTooltipTimerProc(HWND hWnd,
                                             UINT /*uMsg*/,
                                             UINT_PTR idEvent,
                                             DWORD /*dwTime*/) {
    if (g_uninitializing.load(std::memory_order_relaxed)) {
        KillTimer(hWnd, idEvent);
        return;
    }
    try {
        if (idEvent == kDelayedTooltipOpenTimerId) {
            KillTimer(hWnd, kDelayedTooltipOpenTimerId);
            g_delayedState.openTimerArmed = false;

            ComPtr<IToolTipAbi> spToolTip =
                ResolveWeakToolTip(g_delayedState.pendingToolTip.Get());
            void* rawPtr = g_delayedState.pendingRawPtr;
            g_delayedState.pendingToolTip.Reset();
            g_delayedState.pendingRawPtr = nullptr;

            if (!spToolTip || !g_toolTipPutIsOpenOriginal) {
                return;
            }

            if ((GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0 ||
                (GetAsyncKeyState(VK_RBUTTON) & 0x8000) != 0) {
                return;
            }

            POINT pt{};
            if (!GetCursorPos(&pt)) {
                return;
            }
            if (std::abs(pt.x - g_delayedState.initialPt.x) >
                    kCursorDriftTolerancePx ||
                std::abs(pt.y - g_delayedState.initialPt.y) >
                    kCursorDriftTolerancePx) {
                return;
            }

            t_allowingDelayedOpen = true;
            g_toolTipPutIsOpenOriginal(spToolTip.Get(), TRUE);
            t_allowingDelayedOpen = false;

            g_delayedState.activeToolTip = MakeWeakToolTip(spToolTip.Get());
            g_delayedState.activeRawPtr = rawPtr;
            if (SetTimer(hWnd, kDelayedTooltipWatchTimerId,
                         kDelayedTooltipWatchIntervalMs,
                         DelayedTooltipTimerProc) != 0) {
                g_delayedState.watchTimerArmed = true;
            }
        } else if (idEvent == kDelayedTooltipWatchTimerId) {
            POINT pt{};
            bool shouldClose =
                !GetCursorPos(&pt) ||
                (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0 ||
                (GetAsyncKeyState(VK_RBUTTON) & 0x8000) != 0 ||
                std::abs(pt.x - g_delayedState.initialPt.x) >
                    (kCursorDriftTolerancePx + 4) ||
                std::abs(pt.y - g_delayedState.initialPt.y) >
                    (kCursorDriftTolerancePx + 4);
            if (shouldClose) {
                CloseActiveDelayedToolTip();
            }
        }
    } catch (...) {
        t_allowingDelayedOpen = false;
        Wh_Log(L"[TOOLTIP] Exception in DelayedTooltipTimerProc");
    }
}

static bool ScheduleDelayedToolTipOpen(void* pThis, int delayMs) {
    if (!pThis || g_uninitializing.load(std::memory_order_relaxed)) {
        return false;
    }
    HWND hWnd = GetTaskbarWndForTimer();
    if (!hWnd) {
        return false;
    }
    if (GetWindowThreadProcessId(hWnd, nullptr) != GetCurrentThreadId()) {
        return false;
    }
    try {
        auto* pUnk = static_cast<IUnknown*>(pThis);
        ComPtr<IToolTipAbi> spToolTip;
        if (FAILED(pUnk->QueryInterface(kIID_IToolTip, &spToolTip)) ||
            !spToolTip) {
            return false;
        }

        POINT pt{};
        if (!GetCursorPos(&pt)) {
            return false;
        }

        if (g_delayedState.openTimerArmed &&
            g_delayedState.pendingRawPtr == pThis &&
            std::abs(pt.x - g_delayedState.initialPt.x) <=
                kCursorDriftTolerancePx &&
            std::abs(pt.y - g_delayedState.initialPt.y) <=
                kCursorDriftTolerancePx) {
            return true;
        }

        if (g_delayedState.activeRawPtr &&
            g_delayedState.activeRawPtr != pThis) {
            CloseActiveDelayedToolTip();
        }

        g_delayedState.pendingToolTip = MakeWeakToolTip(spToolTip.Get());
        g_delayedState.pendingRawPtr = pThis;
        g_delayedState.initialPt = pt;
        if (SetTimer(hWnd, kDelayedTooltipOpenTimerId,
                     static_cast<UINT>(delayMs),
                     DelayedTooltipTimerProc) != 0) {
            g_delayedState.openTimerArmed = true;
            return true;
        }
    } catch (...) {
        Wh_Log(L"[TOOLTIP] Exception in ScheduleDelayedToolTipOpen");
    }
    return false;
}

static HRESULT __stdcall ToolTip_put_IsOpen_Hook(void* pThis, boolean value) {
    if (t_allowingDelayedOpen) {
        if (g_toolTipPutIsOpenOriginal) {
            return g_toolTipPutIsOpenOriginal(pThis, value);
        }
        return S_OK;
    }

    if (!value) {
        DWORD taskbarThreadId =
            g_taskbarThreadId.load(std::memory_order_relaxed);
        if (taskbarThreadId != 0 && GetCurrentThreadId() == taskbarThreadId) {
            if (pThis && pThis == g_delayedState.pendingRawPtr) {
                CancelDelayedOpenTimer();
            }
            if (pThis && pThis == g_delayedState.activeRawPtr) {
                CloseActiveDelayedToolTip();
            }
        }
        if (g_toolTipPutIsOpenOriginal) {
            return g_toolTipPutIsOpenOriginal(pThis, value);
        }
        return S_OK;
    }

    bool hideApps =
        g_settings.hideTaskbarAppTooltips.load(std::memory_order_relaxed);
    bool hideStart =
        g_settings.hideStartButtonTooltip.load(std::memory_order_relaxed);
    bool hideTray =
        g_settings.hideSystemTrayTooltips.load(std::memory_order_relaxed);
    bool hideDrawer =
        g_settings.hideTrayOverflowTooltips.load(std::memory_order_relaxed);
    int delayMs =
        g_settings.tooltipHoverDelayMs.load(std::memory_order_relaxed);

    if (hideApps && hideStart && hideTray && hideDrawer) {
        // All areas suppressed: always suppress with zero overhead
        return S_OK;
    }

    if (!hideApps && !hideStart && !hideTray && !hideDrawer && delayMs <= 0) {
        // All areas visible and no extra delay: allow opening immediately
        if (g_toolTipPutIsOpenOriginal) {
            return g_toolTipPutIsOpenOriginal(pThis, value);
        }
        return S_OK;
    }

    ToolTipArea area = IdentifyToolTipArea(pThis);
    if (IsAreaSuppressed(area)) {
        // Hidden area: always suppress permanently
        return S_OK;
    }

    if (delayMs > 0 && ScheduleDelayedToolTipOpen(pThis, delayMs)) {
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
    ComPtr<IToolTipAbi> spToolTip;
    if (FAILED(unk->QueryInterface(kIID_IToolTip, &spToolTip)) || !spToolTip) {
        return false;
    }

    void** vtable = *reinterpret_cast<void***>(spToolTip.Get());
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
        Wh_Log(
            L"[HOOK] Hooked ToolTip::put_IsOpen on first live instance");
    }

    // Immediately close the tooltip instance
    put_IsOpen(spToolTip.Get(), FALSE);
    return true;
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
    SendMessageW(hWnd, GetRunFromWindowThreadMessage(), 0,
                 reinterpret_cast<LPARAM>(&param));

    UnhookWindowsHookEx(hook);
    return true;
}

static void WINAPI EagerHookToolTipOnTaskbarThread(void* /*procParam*/) {
    try {
        ComPtr<IToolTipFactoryAbi> spFactory;
        if (FAILED(RoGetActivationFactory(
                HStringReference(L"Windows.UI.Xaml.Controls.ToolTip").Get(),
                kIID_IToolTipFactory, &spFactory)) ||
            !spFactory) {
            return;
        }

        ComPtr<IInspectable> spInner;
        ComPtr<IToolTipAbi> spToolTip;
        if (SUCCEEDED(
                spFactory->CreateInstance(nullptr, &spInner, &spToolTip)) &&
            spToolTip) {
            void** vtable = *reinterpret_cast<void***>(spToolTip.Get());
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
                        L"[HOOK] Hooked ToolTip::put_IsOpen eagerly on "
                        L"Taskbar UI thread");
                }
            }
        }
    } catch (...) {
        Wh_Log(L"[HOOK] Eager ToolTip hook on UI thread caught exception");
    }
}

static void WINAPI CleanupDelayedToolTipOnTaskbarThread(void* /*procParam*/) {
    CancelDelayedOpenTimer();
    CloseActiveDelayedToolTip();
}

static void EnsurePutIsOpenHooked() {
    if (!g_putIsOpenHooked.load(std::memory_order_relaxed)) {
        EagerHookToolTipOnTaskbarThread(nullptr);
    }
}

static void InitEagerToolTipHook() {
    HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();
    if (hTaskbarWnd) {
        g_taskbarWnd.store(hTaskbarWnd, std::memory_order_relaxed);
        DWORD threadId = GetWindowThreadProcessId(hTaskbarWnd, nullptr);
        g_taskbarThreadId.store(threadId, std::memory_order_relaxed);
        Wh_Log(
            L"[HOOK] Dispatching eager ToolTip hook to Taskbar window %p "
            L"(thread %u)",
            hTaskbarWnd, threadId);
        RunFromWindowThread(hTaskbarWnd, EagerHookToolTipOnTaskbarThread,
                            nullptr);
    } else {
        Wh_Log(L"[HOOK] Taskbar window not found yet for eager ToolTip hook");
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
    EnsurePutIsOpenHooked();
    void* pIToolTip =
        pToolTip ? *(reinterpret_cast<void**>(pToolTip)) : nullptr;

    bool isOverflow = false;
    void* pITarget = pTarget ? *(reinterpret_cast<void**>(pTarget)) : nullptr;
    if (pITarget) {
        try {
            if (IsInNotificationAreaOverflow(static_cast<IUnknown*>(pITarget))) {
                isOverflow = true;
            }
        } catch (...) {
            Wh_Log(L"[TOOLTIP] Target inspection caught exception");
        }
    }

    ToolTipArea area =
        isOverflow ? ToolTipArea::TrayOverflow : ToolTipArea::SystemTray;
    if (pIToolTip) {
        RegisterToolTip(pIToolTip, area);
    }

    if (!IsAreaSuppressed(area)) {
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
using ApplyTaskbarTooltipPlacement_t = void(__cdecl*)(void* pToolTip,
                                                      int location,
                                                      WinrtSize size1,
                                                      WinrtSize size2);

static ApplyTaskbarTooltipPlacement_t
    ApplyTaskbarTooltipPlacement_Tray_Original = nullptr;

static void __cdecl ApplyTaskbarTooltipPlacement_Tray_Hook(void* pToolTip,
                                                           int location,
                                                           WinrtSize size1,
                                                           WinrtSize size2) {
    EnsurePutIsOpenHooked();
    void* pIToolTip =
        pToolTip ? *(reinterpret_cast<void**>(pToolTip)) : nullptr;

    ToolTipArea area = ToolTipArea::SystemTray;
    if (pIToolTip) {
        area = LookupRegisteredToolTip(pIToolTip);
        if (area == ToolTipArea::Unknown) {
            ToolTipArea inspected = InspectPlacementTargetArea(pIToolTip);
            area = (inspected == ToolTipArea::TrayOverflow)
                       ? ToolTipArea::TrayOverflow
                       : ToolTipArea::SystemTray;
            RegisterToolTip(pIToolTip, area);
        }
    }

    if (!IsAreaSuppressed(area)) {
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

static void __cdecl ApplyTaskbarTooltipPlacement_View_Hook(void* pToolTip,
                                                           int location,
                                                           WinrtSize size1,
                                                           WinrtSize size2) {
    EnsurePutIsOpenHooked();
    void* pIToolTip =
        pToolTip ? *(reinterpret_cast<void**>(pToolTip)) : nullptr;
    ToolTipArea area =
        (t_startButtonHoverDepth > 0) ? ToolTipArea::StartButton
                                      : ToolTipArea::TaskbarApp;
    if (pIToolTip) {
        if (t_startButtonHoverDepth > 0) {
            RegisterToolTip(pIToolTip, ToolTipArea::StartButton);
        } else {
            ToolTipArea cached = LookupRegisteredToolTip(pIToolTip);
            if (cached != ToolTipArea::Unknown) {
                area = cached;
            } else {
                ToolTipArea inspected = InspectPlacementTargetArea(pIToolTip);
                area = (inspected == ToolTipArea::StartButton)
                           ? ToolTipArea::StartButton
                           : ToolTipArea::TaskbarApp;
                RegisterToolTip(pIToolTip, area);
            }
        }
    }
    if (!IsAreaSuppressed(area)) {
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
using ApplyTaskbarTooltipPlacement6_t = void(__cdecl*)(void* pToolTip,
                                                       int location,
                                                       WinrtSize size1,
                                                       WinrtSize size2,
                                                       bool b1,
                                                       bool b2);
static ApplyTaskbarTooltipPlacement6_t ApplyTaskbarTooltipPlacement6_Original =
    nullptr;

static void __cdecl ApplyTaskbarTooltipPlacement6_Hook(void* pToolTip,
                                                       int location,
                                                       WinrtSize size1,
                                                       WinrtSize size2,
                                                       bool b1,
                                                       bool b2) {
    EnsurePutIsOpenHooked();
    void* pIToolTip =
        pToolTip ? *(reinterpret_cast<void**>(pToolTip)) : nullptr;
    ToolTipArea area =
        (t_startButtonHoverDepth > 0) ? ToolTipArea::StartButton
                                      : ToolTipArea::TaskbarApp;
    if (pIToolTip) {
        if (t_startButtonHoverDepth > 0) {
            RegisterToolTip(pIToolTip, ToolTipArea::StartButton);
        } else {
            ToolTipArea cached = LookupRegisteredToolTip(pIToolTip);
            if (cached != ToolTipArea::Unknown) {
                area = cached;
            } else {
                ToolTipArea inspected = InspectPlacementTargetArea(pIToolTip);
                area = (inspected == ToolTipArea::StartButton)
                           ? ToolTipArea::StartButton
                           : ToolTipArea::TaskbarApp;
                RegisterToolTip(pIToolTip, area);
            }
        }
    }
    if (!IsAreaSuppressed(area)) {
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

// Taskbar.View.dll: TaskbarLocationHelpers::ApplyTaskbarTooltipOrientation
using ApplyTaskbarTooltipOrientation2_t = void(__cdecl*)(void* pToolTip,
                                                         int location);
static ApplyTaskbarTooltipOrientation2_t
    ApplyTaskbarTooltipOrientation2_Original = nullptr;

static void __cdecl ApplyTaskbarTooltipOrientation2_Hook(void* pToolTip,
                                                         int location) {
    EnsurePutIsOpenHooked();
    void* pIToolTip =
        pToolTip ? *(reinterpret_cast<void**>(pToolTip)) : nullptr;
    if (pIToolTip) {
        RegisterToolTip(pIToolTip, (t_startButtonHoverDepth > 0)
                                       ? ToolTipArea::StartButton
                                       : ToolTipArea::TaskbarApp);
    }
    if (ApplyTaskbarTooltipOrientation2_Original) {
        ApplyTaskbarTooltipOrientation2_Original(pToolTip, location);
    }
}

using ApplyTaskbarTooltipOrientation6_t = void(__cdecl*)(void* pToolTip,
                                                         int location,
                                                         bool b1,
                                                         bool b2,
                                                         float f1,
                                                         double d1);
static ApplyTaskbarTooltipOrientation6_t
    ApplyTaskbarTooltipOrientation6_Original = nullptr;

static void __cdecl ApplyTaskbarTooltipOrientation6_Hook(void* pToolTip,
                                                         int location,
                                                         bool b1,
                                                         bool b2,
                                                         float f1,
                                                         double d1) {
    EnsurePutIsOpenHooked();
    void* pIToolTip =
        pToolTip ? *(reinterpret_cast<void**>(pToolTip)) : nullptr;
    if (pIToolTip) {
        RegisterToolTip(pIToolTip, (t_startButtonHoverDepth > 0)
                                       ? ToolTipArea::StartButton
                                       : ToolTipArea::TaskbarApp);
    }
    if (ApplyTaskbarTooltipOrientation6_Original) {
        ApplyTaskbarTooltipOrientation6_Original(pToolTip, location, b1, b2, f1,
                                                 d1);
    }
}

// Hover state hooks in Taskbar.View.dll
using ExperienceToggleButton_UpdateHover_t = void(__cdecl*)(void* pThis,
                                                            bool isHover);
static ExperienceToggleButton_UpdateHover_t
    ExperienceToggleButton_UpdateHover_Original = nullptr;

static void RegisterStartButtonToolTipFromInstance(void* pThis) {
    if (!pThis) {
        return;
    }
    try {
        // In C++/WinRT composable classes (ExperienceToggleButton :
        // ToggleButtonT), offset +sizeof(void*) holds m_inner (the base
        // ToggleButton IInspectable*).
        auto* pInner = *(reinterpret_cast<IUnknown* const*>(pThis) + 1);
        if (!pInner || !IsElementStartButton(pInner)) {
            return;
        }
        ComPtr<IInspectable> spDepObj;
        if (FAILED(pInner->QueryInterface(kIID_IDependencyObject, &spDepObj)) ||
            !spDepObj) {
            return;
        }

        ComPtr<IToolTipServiceStaticsAbi> spService;
        if (FAILED(RoGetActivationFactory(
                HStringReference(L"Windows.UI.Xaml.Controls.ToolTipService")
                    .Get(),
                kIID_IToolTipServiceStatics, &spService)) ||
            !spService) {
            return;
        }

        ComPtr<IInspectable> spToolTipObj;
        if (SUCCEEDED(spService->GetToolTip(spDepObj.Get(), &spToolTipObj)) &&
            spToolTipObj) {
            ComPtr<IToolTipAbi> spToolTip;
            if (SUCCEEDED(
                    spToolTipObj->QueryInterface(kIID_IToolTip, &spToolTip)) &&
                spToolTip) {
                RegisterToolTip(spToolTip.Get(), ToolTipArea::StartButton);
                if (IsAreaSuppressed(ToolTipArea::StartButton)) {
                    spToolTip->put_IsOpen(FALSE);
                }
            }
        }
    } catch (...) {
        Wh_Log(L"[TOOLTIP] RegisterStartButtonToolTipFromInstance caught "
               L"exception");
    }
}

static void __cdecl ExperienceToggleButton_UpdateHover_Hook(void* pThis,
                                                            bool isHover) {
    EnsurePutIsOpenHooked();
    auto* pInner = pThis ? *(reinterpret_cast<IUnknown* const*>(pThis) + 1)
                         : nullptr;
    if (pInner && IsElementStartButton(pInner)) {
        RegisterStartButtonToolTipFromInstance(pThis);
        StartButtonHoverScope scope;
        if (ExperienceToggleButton_UpdateHover_Original) {
            ExperienceToggleButton_UpdateHover_Original(pThis, isHover);
        }
    } else {
        if (ExperienceToggleButton_UpdateHover_Original) {
            ExperienceToggleButton_UpdateHover_Original(pThis, isHover);
        }
    }
}

using TaskListButton_UpdateHover_t = void(__cdecl*)(void* pThis);
static TaskListButton_UpdateHover_t TaskListButton_UpdateHover_Original =
    nullptr;

static void __cdecl TaskListButton_UpdateHover_Hook(void* pThis) {
    EnsurePutIsOpenHooked();
    if (!IsAreaSuppressed(ToolTipArea::TaskbarApp)) {
        if (TaskListButton_UpdateHover_Original) {
            TaskListButton_UpdateHover_Original(pThis);
        }
    }
}

using OverflowToggleButton_UpdateHover_t = void(__cdecl*)(void* pThis);
static OverflowToggleButton_UpdateHover_t
    OverflowToggleButton_UpdateHover_Original = nullptr;

static void __cdecl OverflowToggleButton_UpdateHover_Hook(void* pThis) {
    EnsurePutIsOpenHooked();
    if (!IsAreaSuppressed(ToolTipArea::TaskbarApp)) {
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
    EnsurePutIsOpenHooked();
    if (IconView_UpdateOuterToolTipPlacement_Original) {
        IconView_UpdateOuterToolTipPlacement_Original(pThis, isHover);
    }
}

static std::atomic<bool> g_systemTrayHooked{false};
static std::atomic<bool> g_taskbarViewHooked{false};

static bool HookSystemTraySymbols(HMODULE module) {
    Wh_Log(L"[HOOK] Hooking SystemTray symbols (module=%p)", module);
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
    Wh_Log(L"[HOOK] HookSystemTraySymbols completed (result=%d)", res ? 1 : 0);
    return res;
}

static bool HookTaskbarViewSymbols(HMODULE module) {
    Wh_Log(L"[HOOK] Hooking Taskbar.View symbols (module=%p)", module);
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
        {
            {
                LR"(void __cdecl TaskbarLocationHelpers::ApplyTaskbarTooltipOrientation(struct winrt::Windows::UI::Xaml::Controls::ToolTip const &,enum winrt::WindowsUdk::UI::Shell::TaskbarLocation))",
                LR"(void __cdecl TaskbarLocationHelpers::ApplyTaskbarTooltipOrientation(struct winrt::Windows::UI::Xaml::Controls::ToolTip const &,enum winrt::WindowsUdk::Shell::TaskbarLocation))",
            },
            &ApplyTaskbarTooltipOrientation2_Original,
            ApplyTaskbarTooltipOrientation2_Hook,
            true,
        },
        {
            {
                LR"(void __cdecl TaskbarLocationHelpers::ApplyTaskbarTooltipOrientation(struct winrt::Windows::UI::Xaml::Controls::ToolTip const &,enum winrt::WindowsUdk::UI::Shell::TaskbarLocation,bool,bool,float,double))",
                LR"(void __cdecl TaskbarLocationHelpers::ApplyTaskbarTooltipOrientation(struct winrt::Windows::UI::Xaml::Controls::ToolTip const &,enum winrt::WindowsUdk::Shell::TaskbarLocation,bool,bool,float,double))",
            },
            &ApplyTaskbarTooltipOrientation6_Original,
            ApplyTaskbarTooltipOrientation6_Hook,
            true,
        },
    };

    bool res = WindhawkUtils::HookSymbols(module, taskbarViewHooks,
                                          ARRAYSIZE(taskbarViewHooks));
    Wh_Log(L"[HOOK] HookTaskbarViewSymbols completed (result=%d)", res ? 1 : 0);
    return res;
}

static void HandleLoadedModule(HMODULE module) {
    if (!module) {
        return;
    }

    if (!g_systemTrayHooked && GetModuleHandleW(L"SystemTray.dll") == module) {
        if (!g_systemTrayHooked.exchange(true)) {
            Wh_Log(L"[INIT] Loaded SystemTray.dll dynamically");
            HookSystemTraySymbols(module);
            Wh_ApplyHookOperations();
        }
    } else if (!g_taskbarViewHooked &&
               GetModuleHandleW(L"Taskbar.View.dll") == module) {
        if (!g_taskbarViewHooked.exchange(true)) {
            Wh_Log(L"[INIT] Loaded Taskbar.View.dll dynamically");
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
    Wh_Log(L"[INIT] Initializing Hide Taskbar Tooltips mod v" WH_MOD_VERSION);

    if (!IsWindows11OrGreater()) {
        Wh_Log(L"[INIT] Only Windows 11 is supported");
        return FALSE;
    }

    g_uninitializing.store(false, std::memory_order_relaxed);
    LoadSettings();

    HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();
    if (hTaskbarWnd) {
        g_taskbarWnd.store(hTaskbarWnd, std::memory_order_relaxed);
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
    Wh_Log(
        L"[INIT] Hide Taskbar Tooltips active (systemTray=%d, taskbarView=%d)",
        static_cast<int>(g_systemTrayHooked.load()),
        static_cast<int>(g_taskbarViewHooked.load()));
}

void Wh_ModSettingsChanged() {
    LoadSettings();
    HWND hTaskbarWnd = GetTaskbarWndForTimer();
    if (hTaskbarWnd) {
        RunFromWindowThread(hTaskbarWnd, CleanupDelayedToolTipOnTaskbarThread,
                            nullptr);
    }
    Wh_Log(
        L"[SETTINGS] Updated settings (hideTaskbarApp=%d, hideStartButton=%d, "
        L"hideSystemTray=%d, hideTrayOverflow=%d, delayMs=%d)",
        g_settings.hideTaskbarAppTooltips.load(std::memory_order_relaxed) ? 1
                                                                          : 0,
        g_settings.hideStartButtonTooltip.load(std::memory_order_relaxed) ? 1
                                                                          : 0,
        g_settings.hideSystemTrayTooltips.load(std::memory_order_relaxed) ? 1
                                                                          : 0,
        g_settings.hideTrayOverflowTooltips.load(std::memory_order_relaxed) ? 1
                                                                            : 0,
        g_settings.tooltipHoverDelayMs.load(std::memory_order_relaxed));
}

void Wh_ModBeforeUninit() {
    g_uninitializing.store(true, std::memory_order_relaxed);
    HWND hTaskbarWnd = g_taskbarWnd.load(std::memory_order_relaxed);
    if (hTaskbarWnd && IsWindow(hTaskbarWnd)) {
        RunFromWindowThread(hTaskbarWnd, CleanupDelayedToolTipOnTaskbarThread,
                            nullptr);
    }
}

void Wh_ModUninit() {
    Wh_Log(L"[UNINIT] Uninitializing Hide Taskbar Tooltips mod");
    std::lock_guard<std::mutex> lock(s_toolTipsMutex);
    s_taskbarToolTips.clear();
    s_startButtonToolTips.clear();
    s_systemTrayToolTips.clear();
    s_trayOverflowToolTips.clear();
}
