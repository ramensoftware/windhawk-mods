// ==WindhawkMod==
// @id              battery-percentage
// @name            Battery Percentage on Taskbar
// @description     Replaces the battery tray icon with a percentage number
// @version         0.5
// @author          ALMAS CP
// @github          https://github.com/almas-cp
// @homepage        https://github.com/almas-cp
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject
// ==/WindhawkMod==


// ==WindhawkModReadme==
/*
# Battery Percentage
Replaces the built-in battery tray icon with a clear, color-coded percentage
number directly in the system tray.

## Features
- **Replaces built-in icon**: Hides the battery glyph and shows percentage text
- **Color-coded levels**: Green (good), Yellow (low), Red (critical), Cyan (charging)
- **Periodic updates**: Timer keeps the percentage accurate
- **Customizable**: Configure colors and thresholds

Only Windows 11 is supported.
*/
// ==/WindhawkModReadme==


// ==WindhawkModSettings==
/*
- lowThreshold: 30
  $name: Low Battery Threshold (%)
  $description: Below this level the color changes to low color

- criticalThreshold: 15
  $name: Critical Battery Threshold (%)
  $description: Below this level the color changes to critical color

- updateIntervalSec: 15
  $name: Update Interval (seconds)
  $description: How often to refresh the battery percentage (minimum 5)

- chargingColorR: 0
  $name: Charging Color - Red
- chargingColorG: 220
  $name: Charging Color - Green
- chargingColorB: 220
  $name: Charging Color - Blue

- goodColorR: 0
  $name: Good Color - Red
- goodColorG: 230
  $name: Good Color - Green
- goodColorB: 0
  $name: Good Color - Blue

- lowColorR: 255
  $name: Low Color - Red
- lowColorG: 200
  $name: Low Color - Green
- lowColorB: 0
  $name: Low Color - Blue

- criticalColorR: 255
  $name: Critical Color - Red
- criticalColorG: 50
  $name: Critical Color - Green
- criticalColorB: 50
  $name: Critical Color - Blue
*/
// ==/WindhawkModSettings==


#include <windhawk_utils.h>

#include <atomic>
#include <functional>
#include <list>
#include <string>

#undef GetCurrentTime

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.Xaml.Automation.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/base.h>

using namespace winrt::Windows::UI::Xaml;


// ─── Settings ────────────────────────────────────────────────────────────────

struct {
    int lowThreshold;
    int criticalThreshold;
    int updateIntervalSec;
    int chargingColorR, chargingColorG, chargingColorB;
    int goodColorR, goodColorG, goodColorB;
    int lowColorR, lowColorG, lowColorB;
    int criticalColorR, criticalColorG, criticalColorB;
} g_settings;


// ─── Globals ─────────────────────────────────────────────────────────────────

std::atomic<bool> g_taskbarViewDllLoaded;
std::atomic<bool> g_unloading;

using FrameworkElementLoadedEventRevoker = winrt::impl::event_revoker<
    IFrameworkElement,
    &winrt::impl::abi<IFrameworkElement>::type::remove_Loaded>;

std::list<FrameworkElementLoadedEventRevoker> g_autoRevokerList;

// Track the battery percentage TextBlock we create
winrt::weak_ref<Controls::TextBlock> g_batteryPercentTextBlock;

// Timer
UINT_PTR g_updateTimerId = 0;

// Track the original battery icon content so we can hide/restore it
winrt::weak_ref<FrameworkElement> g_batteryIconContent;


// ─── Helper: Find taskbar window ─────────────────────────────────────────────

HWND FindCurrentProcessTaskbarWnd() {
    HWND hTaskbarWnd = nullptr;
    EnumWindows(
        [](HWND hWnd, LPARAM lParam) -> BOOL {
            DWORD dwProcessId;
            WCHAR className[32];
            if (GetWindowThreadProcessId(hWnd, &dwProcessId) &&
                dwProcessId == GetCurrentProcessId() &&
                GetClassName(hWnd, className, ARRAYSIZE(className)) &&
                _wcsicmp(className, L"Shell_TrayWnd") == 0) {
                *reinterpret_cast<HWND*>(lParam) = hWnd;
                return FALSE;
            }
            return TRUE;
        },
        reinterpret_cast<LPARAM>(&hTaskbarWnd));
    return hTaskbarWnd;
}


// ─── XAML Tree Helpers (same pattern as the reference mod) ───────────────────

FrameworkElement EnumChildElements(
    FrameworkElement element,
    std::function<bool(FrameworkElement)> enumCallback) {
    int childrenCount = Media::VisualTreeHelper::GetChildrenCount(element);
    for (int i = 0; i < childrenCount; i++) {
        auto child = Media::VisualTreeHelper::GetChild(element, i)
                         .try_as<FrameworkElement>();
        if (!child) continue;
        if (enumCallback(child)) return child;
    }
    return nullptr;
}

FrameworkElement FindChildByName(FrameworkElement element, PCWSTR name) {
    return EnumChildElements(element, [name](FrameworkElement child) {
        return child.Name() == name;
    });
}

FrameworkElement FindChildByClassName(FrameworkElement element,
                                      PCWSTR className) {
    return EnumChildElements(element, [className](FrameworkElement child) {
        return winrt::get_class_name(child) == className;
    });
}

FrameworkElement EnumParentElements(
    FrameworkElement element,
    std::function<bool(FrameworkElement)> enumCallback) {
    auto parent = element;
    while (true) {
        parent = Media::VisualTreeHelper::GetParent(parent)
                     .try_as<FrameworkElement>();
        if (!parent) return nullptr;
        if (enumCallback(parent)) return parent;
    }
}

bool IsChildOfElementByName(FrameworkElement element, PCWSTR name) {
    return !!EnumParentElements(element, [name](FrameworkElement parent) {
        return parent.Name() == name;
    });
}


// ─── Battery Color Logic ─────────────────────────────────────────────────────

static int Clamp(int v, int lo, int hi) {
    return (v < lo) ? lo : (v > hi) ? hi : v;
}

winrt::Windows::UI::Color GetBatteryColor(int percent, bool charging) {
    int r, g, b;

    if (charging) {
        r = g_settings.chargingColorR;
        g = g_settings.chargingColorG;
        b = g_settings.chargingColorB;
    } else if (percent <= g_settings.criticalThreshold) {
        r = g_settings.criticalColorR;
        g = g_settings.criticalColorG;
        b = g_settings.criticalColorB;
    } else if (percent <= g_settings.lowThreshold) {
        r = g_settings.lowColorR;
        g = g_settings.lowColorG;
        b = g_settings.lowColorB;
    } else {
        r = g_settings.goodColorR;
        g = g_settings.goodColorG;
        b = g_settings.goodColorB;
    }

    return {255,
            (uint8_t)Clamp(r, 0, 255),
            (uint8_t)Clamp(g, 0, 255),
            (uint8_t)Clamp(b, 0, 255)};
}


// ─── Update the percentage text ──────────────────────────────────────────────

void UpdateBatteryPercentText() {
    auto textBlock = g_batteryPercentTextBlock.get();
    if (!textBlock) return;

    SYSTEM_POWER_STATUS sps;
    if (!GetSystemPowerStatus(&sps)) return;

    bool hasBattery = (sps.BatteryFlag != 128);
    if (!hasBattery || sps.BatteryLifePercent == 255) {
        textBlock.Text(L"AC");
        textBlock.Foreground(
            Media::SolidColorBrush({255, 200, 200, 200}));
        return;
    }

    int percent = (int)sps.BatteryLifePercent;
    if (percent > 100) percent = 100;

    bool charging = (sps.ACLineStatus == 1) && (sps.BatteryFlag & 8);
    auto color = GetBatteryColor(percent, charging);

    wchar_t buf[8];
    swprintf_s(buf, L"%d", percent);
    textBlock.Text(buf);
    textBlock.Foreground(Media::SolidColorBrush(color));
}


// ─── Apply battery percentage to a BatteryIconContent element ────────────────

void ApplyBatteryPercentage(FrameworkElement batteryIconContent) {
    if (g_unloading) {
        // Restore: show the original battery icon, remove our text
        batteryIconContent.Visibility(Visibility::Visible);

        // Remove our TextBlock if we can find it
        auto parent = Media::VisualTreeHelper::GetParent(batteryIconContent)
                          .try_as<Controls::Panel>();
        if (parent) {
            auto children = parent.Children();
            for (uint32_t i = 0; i < children.Size(); i++) {
                auto child = children.GetAt(i).try_as<FrameworkElement>();
                if (child && child.Name() == L"BatteryPercentText") {
                    children.RemoveAt(i);
                    break;
                }
            }
        }

        g_batteryPercentTextBlock = nullptr;
        return;
    }

    // Hide the original battery icon content
    batteryIconContent.Visibility(Visibility::Collapsed);
    g_batteryIconContent = batteryIconContent;

    // Find the parent container (ContentGrid) to add our TextBlock
    auto contentGrid = Media::VisualTreeHelper::GetParent(batteryIconContent)
                           .try_as<Controls::Panel>();
    if (!contentGrid) {
        Wh_Log(L"❌ Failed to get parent panel of BatteryIconContent");
        return;
    }

    // Check if we already added a TextBlock
    auto existingText = g_batteryPercentTextBlock.get();
    if (existingText) {
        UpdateBatteryPercentText();
        return;
    }

    // Create our percentage TextBlock
    Controls::TextBlock percentText;
    percentText.Name(L"BatteryPercentText");
    percentText.Text(L"--");
    percentText.FontSize(12);
    percentText.FontWeight({700});  // Bold
    percentText.Foreground(
        Media::SolidColorBrush({255, 0, 230, 0}));
    percentText.HorizontalAlignment(HorizontalAlignment::Center);
    percentText.VerticalAlignment(VerticalAlignment::Center);
    percentText.TextAlignment(winrt::Windows::UI::Xaml::TextAlignment::Center);

    // Add to the same container
    contentGrid.Children().Append(percentText);

    g_batteryPercentTextBlock = percentText;

    Wh_Log(L"✅ Battery percentage TextBlock added");

    // Immediately update with current battery level
    UpdateBatteryPercentText();
}


// ─── Process the ControlCenterButton to find battery icon ────────────────────

void ProcessControlCenterButtonIcon(FrameworkElement systemTrayIconElement) {
    FrameworkElement contentGrid = nullptr;

    FrameworkElement child = systemTrayIconElement;
    if ((child = FindChildByName(child, L"ContainerGrid")) &&
        (child = FindChildByName(child, L"ContentGrid"))) {
        contentGrid = child;
    } else {
        return;
    }

    // Look for BatteryIconContent
    FrameworkElement batteryContent =
        FindChildByClassName(contentGrid, L"SystemTray.BatteryIconContent");
    if (!batteryContent) {
        return;  // Not the battery icon
    }

    Wh_Log(L"🔋 Found BatteryIconContent in ControlCenterButton!");
    ApplyBatteryPercentage(batteryContent);
}


// ─── Apply to ControlCenterButton stack ──────────────────────────────────────

bool ApplyControlCenterButtonStyle(FrameworkElement controlCenterButton) {
    FrameworkElement stackPanel = nullptr;

    FrameworkElement child = controlCenterButton;
    if ((child =
             FindChildByClassName(child, L"Windows.UI.Xaml.Controls.Grid")) &&
        (child = FindChildByName(child, L"ContentPresenter")) &&
        (child = FindChildByClassName(
             child, L"Windows.UI.Xaml.Controls.ItemsPresenter")) &&
        (child = FindChildByClassName(
             child, L"Windows.UI.Xaml.Controls.StackPanel"))) {
        stackPanel = child;
    }

    if (!stackPanel) {
        return false;
    }

    EnumChildElements(stackPanel, [](FrameworkElement child) {
        auto childClassName = winrt::get_class_name(child);
        if (childClassName != L"Windows.UI.Xaml.Controls.ContentPresenter") {
            return false;
        }

        FrameworkElement systemTrayIconElement =
            FindChildByName(child, L"SystemTrayIcon");
        if (!systemTrayIconElement) {
            return false;
        }

        ProcessControlCenterButtonIcon(systemTrayIconElement);
        return false;
    });

    return true;
}


// ─── Apply styles to the full system tray ────────────────────────────────────

bool ApplyStyle(XamlRoot xamlRoot) {
    FrameworkElement systemTrayFrameGrid = nullptr;

    FrameworkElement child = xamlRoot.Content().try_as<FrameworkElement>();
    if (child &&
        (child = FindChildByClassName(child, L"SystemTray.SystemTrayFrame")) &&
        (child = FindChildByName(child, L"SystemTrayFrameGrid"))) {
        systemTrayFrameGrid = child;
    }

    if (!systemTrayFrameGrid) {
        return false;
    }

    FrameworkElement controlCenterButton =
        FindChildByName(systemTrayFrameGrid, L"ControlCenterButton");
    if (controlCenterButton) {
        return ApplyControlCenterButtonStyle(controlCenterButton);
    }

    return false;
}


// ─── Hook IconView constructor (from Taskbar.View.dll) ───────────────────────

using IconView_IconView_t = void*(WINAPI*)(void* pThis);
IconView_IconView_t IconView_IconView_Original;

void* WINAPI IconView_IconView_Hook(void* pThis) {
    void* ret = IconView_IconView_Original(pThis);

    FrameworkElement iconView = nullptr;
    ((IUnknown**)pThis)[1]->QueryInterface(winrt::guid_of<FrameworkElement>(),
                                           winrt::put_abi(iconView));
    if (!iconView) {
        return ret;
    }

    g_autoRevokerList.emplace_back();
    auto autoRevokerIt = g_autoRevokerList.end();
    --autoRevokerIt;

    *autoRevokerIt = iconView.Loaded(
        winrt::auto_revoke_t{},
        [autoRevokerIt](winrt::Windows::Foundation::IInspectable const& sender,
                        RoutedEventArgs const& e) {
            g_autoRevokerList.erase(autoRevokerIt);

            auto iconView = sender.try_as<FrameworkElement>();
            if (!iconView) return;

            auto className = winrt::get_class_name(iconView);

            if (className == L"SystemTray.IconView") {
                if (iconView.Name() == L"SystemTrayIcon") {
                    if (IsChildOfElementByName(iconView,
                                               L"ControlCenterButton")) {
                        ProcessControlCenterButtonIcon(iconView);
                    }
                }
            }
        });

    return ret;
}


// ─── Taskbar symbol resolution (same pattern as reference) ───────────────────

void* CTaskBand_ITaskListWndSite_vftable;

using CTaskBand_GetTaskbarHost_t = void*(WINAPI*)(void* pThis, void** result);
CTaskBand_GetTaskbarHost_t CTaskBand_GetTaskbarHost_Original;

void* TaskbarHost_FrameHeight_Original;

using std__Ref_count_base__Decref_t = void(WINAPI*)(void* pThis);
std__Ref_count_base__Decref_t std__Ref_count_base__Decref_Original;

XamlRoot GetTaskbarXamlRoot(HWND hTaskbarWnd) {
    HWND hTaskSwWnd = (HWND)GetProp(hTaskbarWnd, L"TaskbandHWND");
    if (!hTaskSwWnd) return nullptr;

    void* taskBand = (void*)GetWindowLongPtr(hTaskSwWnd, 0);
    void* taskBandForTaskListWndSite = taskBand;
    for (int i = 0;
         *(void**)taskBandForTaskListWndSite !=
         CTaskBand_ITaskListWndSite_vftable;
         i++) {
        if (i == 20) return nullptr;
        taskBandForTaskListWndSite = (void**)taskBandForTaskListWndSite + 1;
    }

    void* taskbarHostSharedPtr[2]{};
    CTaskBand_GetTaskbarHost_Original(taskBandForTaskListWndSite,
                                      taskbarHostSharedPtr);
    if (!taskbarHostSharedPtr[0] && !taskbarHostSharedPtr[1]) return nullptr;

    size_t taskbarElementIUnknownOffset = 0x48;

#if defined(_M_X64)
    {
        const BYTE* b = (const BYTE*)TaskbarHost_FrameHeight_Original;
        if (b[0] == 0x48 && b[1] == 0x83 && b[2] == 0xEC && b[4] == 0x48 &&
            b[5] == 0x83 && b[6] == 0xC1 && b[7] <= 0x7F) {
            taskbarElementIUnknownOffset = b[7];
        }
    }
#endif

    auto* taskbarElementIUnknown =
        *(IUnknown**)((BYTE*)taskbarHostSharedPtr[0] +
                      taskbarElementIUnknownOffset);

    FrameworkElement taskbarElement = nullptr;
    taskbarElementIUnknown->QueryInterface(winrt::guid_of<FrameworkElement>(),
                                           winrt::put_abi(taskbarElement));

    auto result = taskbarElement ? taskbarElement.XamlRoot() : nullptr;

    std__Ref_count_base__Decref_Original(taskbarHostSharedPtr[1]);

    return result;
}


// ─── Run from window thread ─────────────────────────────────────────────────

using RunFromWindowThreadProc_t = void(WINAPI*)(void* parameter);

bool RunFromWindowThread(HWND hWnd,
                         RunFromWindowThreadProc_t proc,
                         void* procParam) {
    static const UINT runFromWindowThreadRegisteredMsg =
        RegisterWindowMessage(L"Windhawk_RunFromWindowThread_" WH_MOD_ID);

    struct RUN_FROM_WINDOW_THREAD_PARAM {
        RunFromWindowThreadProc_t proc;
        void* procParam;
    };

    DWORD dwThreadId = GetWindowThreadProcessId(hWnd, nullptr);
    if (dwThreadId == 0) return false;

    if (dwThreadId == GetCurrentThreadId()) {
        proc(procParam);
        return true;
    }

    HHOOK hook = SetWindowsHookEx(
        WH_CALLWNDPROC,
        [](int nCode, WPARAM wParam, LPARAM lParam) -> LRESULT {
            if (nCode == HC_ACTION) {
                const CWPSTRUCT* cwp = (const CWPSTRUCT*)lParam;
                static const UINT msg = RegisterWindowMessage(
                    L"Windhawk_RunFromWindowThread_" WH_MOD_ID);
                if (cwp->message == msg) {
                    RUN_FROM_WINDOW_THREAD_PARAM* param =
                        (RUN_FROM_WINDOW_THREAD_PARAM*)cwp->lParam;
                    param->proc(param->procParam);
                }
            }
            return CallNextHookEx(nullptr, nCode, wParam, lParam);
        },
        nullptr, dwThreadId);
    if (!hook) return false;

    RUN_FROM_WINDOW_THREAD_PARAM param;
    param.proc = proc;
    param.procParam = procParam;
    SendMessage(hWnd, runFromWindowThreadRegisteredMsg, 0, (LPARAM)&param);

    UnhookWindowsHookEx(hook);
    return true;
}


// ─── Timer for periodic battery updates ──────────────────────────────────────

void CALLBACK BatteryTimerProc(HWND, UINT, UINT_PTR, DWORD) {
    HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();
    if (!hTaskbarWnd) return;

    RunFromWindowThread(
        hTaskbarWnd,
        [](void*) {
            UpdateBatteryPercentText();
        },
        nullptr);
}


// ─── Settings ────────────────────────────────────────────────────────────────

void LoadSettings() {
    g_settings.lowThreshold = Wh_GetIntSetting(L"lowThreshold");
    g_settings.criticalThreshold = Wh_GetIntSetting(L"criticalThreshold");
    g_settings.updateIntervalSec =
        Clamp(Wh_GetIntSetting(L"updateIntervalSec"), 5, 3600);

    g_settings.chargingColorR = Wh_GetIntSetting(L"chargingColorR");
    g_settings.chargingColorG = Wh_GetIntSetting(L"chargingColorG");
    g_settings.chargingColorB = Wh_GetIntSetting(L"chargingColorB");

    g_settings.goodColorR = Wh_GetIntSetting(L"goodColorR");
    g_settings.goodColorG = Wh_GetIntSetting(L"goodColorG");
    g_settings.goodColorB = Wh_GetIntSetting(L"goodColorB");

    g_settings.lowColorR = Wh_GetIntSetting(L"lowColorR");
    g_settings.lowColorG = Wh_GetIntSetting(L"lowColorG");
    g_settings.lowColorB = Wh_GetIntSetting(L"lowColorB");

    g_settings.criticalColorR = Wh_GetIntSetting(L"criticalColorR");
    g_settings.criticalColorG = Wh_GetIntSetting(L"criticalColorG");
    g_settings.criticalColorB = Wh_GetIntSetting(L"criticalColorB");
}


// ─── Apply settings to already-initialized elements ──────────────────────────

void ApplySettings() {
    Wh_Log(L"Applying battery percentage settings");

    HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();
    if (!hTaskbarWnd) {
        Wh_Log(L"No taskbar found");
        return;
    }

    RunFromWindowThread(
        hTaskbarWnd,
        [](void* pParam) {
            HWND hTaskbarWnd = *(HWND*)pParam;

            g_autoRevokerList.clear();
            g_batteryPercentTextBlock = nullptr;

            auto xamlRoot = GetTaskbarXamlRoot(hTaskbarWnd);
            if (!xamlRoot) {
                Wh_Log(L"Getting XamlRoot failed");
                return;
            }

            if (!ApplyStyle(xamlRoot)) {
                Wh_Log(L"ApplyStyle failed");
            }
        },
        &hTaskbarWnd);
}


// ─── Hook Taskbar.View.dll symbols ───────────────────────────────────────────

bool HookTaskbarViewDllSymbols(HMODULE module) {
    WindhawkUtils::SYMBOL_HOOK symbolHooks[] = {
        {
            {LR"(public: __cdecl winrt::SystemTray::implementation::IconView::IconView(void))"},
            &IconView_IconView_Original,
            IconView_IconView_Hook,
        },
    };

    return HookSymbols(module, symbolHooks, ARRAYSIZE(symbolHooks));
}

HMODULE GetTaskbarViewModuleHandle() {
    HMODULE module = GetModuleHandle(L"Taskbar.View.dll");
    if (!module) {
        module = GetModuleHandle(L"ExplorerExtensions.dll");
    }
    return module;
}

void HandleLoadedModuleIfTaskbarView(HMODULE module, LPCWSTR lpLibFileName) {
    if (!g_taskbarViewDllLoaded && GetTaskbarViewModuleHandle() == module &&
        !g_taskbarViewDllLoaded.exchange(true)) {
        Wh_Log(L"Loaded %s", lpLibFileName);
        if (HookTaskbarViewDllSymbols(module)) {
            Wh_ApplyHookOperations();
        }
    }
}

using LoadLibraryExW_t = decltype(&LoadLibraryExW);
LoadLibraryExW_t LoadLibraryExW_Original;
HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR lpLibFileName,
                                   HANDLE hFile,
                                   DWORD dwFlags) {
    HMODULE module = LoadLibraryExW_Original(lpLibFileName, hFile, dwFlags);
    if (module) {
        HandleLoadedModuleIfTaskbarView(module, lpLibFileName);
    }
    return module;
}


// ─── Hook taskbar.dll symbols ────────────────────────────────────────────────

bool HookTaskbarDllSymbols() {
    HMODULE module =
        LoadLibraryEx(L"taskbar.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!module) {
        Wh_Log(L"Failed to load taskbar.dll");
        return false;
    }

    WindhawkUtils::SYMBOL_HOOK taskbarDllHooks[] = {
        {
            {LR"(const CTaskBand::`vftable'{for `ITaskListWndSite'})"},
            &CTaskBand_ITaskListWndSite_vftable,
        },
        {
            {LR"(public: virtual class std::shared_ptr<class TaskbarHost> __cdecl CTaskBand::GetTaskbarHost(void)const )"},
            &CTaskBand_GetTaskbarHost_Original,
        },
        {
            {LR"(public: int __cdecl TaskbarHost::FrameHeight(void)const )"},
            &TaskbarHost_FrameHeight_Original,
        },
        {
            {LR"(public: void __cdecl std::_Ref_count_base::_Decref(void))"},
            &std__Ref_count_base__Decref_Original,
        },
    };

    return HookSymbols(module, taskbarDllHooks, ARRAYSIZE(taskbarDllHooks));
}


// ─── Windhawk Callbacks ──────────────────────────────────────────────────────

BOOL Wh_ModInit() {
    Wh_Log(L"🔋 Battery Percentage mod initializing");

    LoadSettings();

    if (HMODULE taskbarViewModule = GetTaskbarViewModuleHandle()) {
        g_taskbarViewDllLoaded = true;
        if (!HookTaskbarViewDllSymbols(taskbarViewModule)) {
            return FALSE;
        }
    } else {
        Wh_Log(L"Taskbar view module not loaded yet");

        HMODULE kernelBaseModule = GetModuleHandle(L"kernelbase.dll");
        auto pKernelBaseLoadLibraryExW =
            (decltype(&LoadLibraryExW))GetProcAddress(kernelBaseModule,
                                                      "LoadLibraryExW");
        WindhawkUtils::SetFunctionHook(pKernelBaseLoadLibraryExW,
                                       LoadLibraryExW_Hook,
                                       &LoadLibraryExW_Original);
    }

    if (!HookTaskbarDllSymbols()) {
        return FALSE;
    }

    return TRUE;
}


void Wh_ModAfterInit() {
    Wh_Log(L"🔋 Battery Percentage mod after init");

    if (!g_taskbarViewDllLoaded) {
        if (HMODULE taskbarViewModule = GetTaskbarViewModuleHandle()) {
            if (!g_taskbarViewDllLoaded.exchange(true)) {
                Wh_Log(L"Got Taskbar.View.dll");
                if (HookTaskbarViewDllSymbols(taskbarViewModule)) {
                    Wh_ApplyHookOperations();
                }
            }
        }
    }

    ApplySettings();

    // Start periodic update timer
    g_updateTimerId = SetTimer(NULL, 0,
                               g_settings.updateIntervalSec * 1000,
                               BatteryTimerProc);
}


void Wh_ModBeforeUninit() {
    Wh_Log(L"🔋 Battery Percentage mod before uninit");

    // Kill timer
    if (g_updateTimerId) {
        KillTimer(NULL, g_updateTimerId);
        g_updateTimerId = 0;
    }

    g_unloading = true;

    // Restore original battery icon
    ApplySettings();
}


void Wh_ModUninit() {
    Wh_Log(L"🔋 Battery Percentage mod uninit");
}


void Wh_ModSettingsChanged() {
    Wh_Log(L"🔋 Battery Percentage settings changed");

    LoadSettings();

    // Restart timer with new interval
    if (g_updateTimerId) {
        KillTimer(NULL, g_updateTimerId);
    }
    g_updateTimerId = SetTimer(NULL, 0,
                               g_settings.updateIntervalSec * 1000,
                               BatteryTimerProc);

    // Apply new colors immediately
    HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();
    if (hTaskbarWnd) {
        RunFromWindowThread(
            hTaskbarWnd,
            [](void*) {
                UpdateBatteryPercentText();
            },
            nullptr);
    }
}
