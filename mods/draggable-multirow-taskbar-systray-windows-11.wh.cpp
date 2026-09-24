// ==WindhawkMod==
// @id              draggable-multirow-taskbar-systray-windows-11
// @name            Draggable, All-in-One Multirow Taskbar & Systray for Windows 11
// @description     Drag the Windows 11 taskbar into multiple rows, with synced taskbar-button rows, notification-area icon wrapping, and Win10-style taskbar behavior in one integrated mod.
// @version         0.1.0
// @author          andkon
// @github          https://github.com/andkondev
// @homepage        https://andkon.dev/
// @license         GPL-3.0
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lcomctl32 -lole32 -loleaut32 -lruntimeobject -lshcore -lversion -ladvapi32 -ld2d1 -lgdi32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Draggable, All-in-One Multirow Taskbar & Systray for Windows 11

Drag the top edge of the Windows 11 taskbar to resize it into multiple rows.
Taskbar buttons and ordinary notification-area icons stay in sync with the
chosen row count, giving Windows 11 a more Windows 10-like multirow taskbar
without running several separate taskbar mods at once.

![Three-row taskbar with multirow tray](https://raw.githubusercontent.com/andkondev/draggable-multirow-taskbar-systray-windows-11/main/assets/screenshots/taskbar-3-row.png)

## Features

- Drag the top edge of the taskbar to switch between one or more rows.
- Span taskbar buttons across multiple rows.
- Wrap ordinary notification-area icons across matching rows.
- Keep Wi-Fi, sound, battery, clock, overflow, and the native one-row fallback
  behavior separate from the ordinary tray-icon grid.
- Use conservative first-install defaults: one row, native-looking taskbar
  height, and native single-row tray behavior until you resize.
- Include Win10-style running indicators and compact Start/Search/Task View
  hover behavior as part of the integrated setup.
- Use hardcoded integrated defaults; resize by dragging the taskbar edge rather
  than changing Windhawk settings.

## Notes and limitations

- The full Windows Search box can make the Search flyout overlap the taskbar on
  multirow layouts. The Search icon and Search icon-and-label modes behave
  better.
- Hidden tray icons are still controlled by Windows in Settings >
  Personalization > Taskbar > Other system tray icons.
- Disabling and re-enabling the mod can briefly flash while Explorer and
  Windhawk reload taskbar components.

![Full Search box overlap limitation](https://raw.githubusercontent.com/andkondev/draggable-multirow-taskbar-systray-windows-11/main/assets/screenshots/search-box-overlap.png)

## No warranty

This mod is provided as-is, without warranty of any kind. It modifies Windows
Explorer and taskbar behavior through Windhawk hooks, so use it at your own
risk. Windows updates, Windhawk updates, Explorer changes, or taskbar-related
system changes may break behavior.

## Credits and license

This mod is GPL-3.0 and is built on m417z's Windhawk taskbar work:

- Taskbar height and icon size
- Multirow taskbar for Windows 11
- Taskbar tray icon spacing and grid
- Windows 11 Taskbar Styler

Thank you to m417z and the Windhawk project for the original mods and the
platform that makes this kind of taskbar customization possible.
*/
// ==/WindhawkModReadme==

// This single mod composes coordinated taskbar height, multirow taskbar,
// notification-area wrapping, and taskbar styling behavior behind one Windhawk
// mod ID.

#ifndef WINVER
#define WINVER 0x0A00
#endif

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0A00
#endif

#ifndef EDGE_RESIZE_FRAME_HOOKS_ONLY
#define EDGE_RESIZE_FRAME_HOOKS_ONLY 1
#endif

#include <windhawk_utils.h>

#include <Unknwn.h>
#include <algorithm>
#include <atomic>
#include <charconv>
#include <chrono>
#include <cmath>
#include <commctrl.h>
#include <combaseapi.h>
#include <cstdarg>
#include <cstdint>
#include <ctime>
#include <functional>
#include <initguid.h>
#include <d2d1_1.h>
#include <limits>
#include <list>
#include <mutex>
#include <ocidl.h>
#include <optional>
#include <random>
#include <regex>
#include <roapi.h>
#include <sstream>
#include <string>
#include <string_view>
#include <thread>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <variant>
#include <vector>
#include <windows.graphics.effects.h>
#include <windowsx.h>
#include <winstring.h>
#include <xamlom.h>

#undef GetCurrentTime

#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Graphics.Effects.h>
#include <winrt/Windows.Networking.Connectivity.h>
#include <winrt/Windows.Storage.Streams.h>
#include <winrt/Windows.System.Power.h>
#include <winrt/Windows.System.h>
#include <winrt/Windows.UI.Composition.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.Text.h>
#include <winrt/Windows.UI.ViewManagement.h>
#include <winrt/Windows.UI.Xaml.Automation.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Hosting.h>
#include <winrt/Windows.UI.Xaml.Markup.h>
#include <winrt/Windows.UI.Xaml.Media.Imaging.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.Shapes.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/base.h>

#define WIN11_TASKBAR_WIN10_MULTIROW_COMBINED 1

namespace MultirowComponent {
void RequestTaskbarRowsRefresh();
void SetLiveTaskbarHeightHintNoRefresh(int height);
void SetLiveTaskbarHeightHint(int height);
void SetLiveRowsHint(int rows);
void SetLiveLayoutHints(int rows, int height);
}

namespace TrayComponent {
void ApplyXamlRootFromMultirow(::winrt::Windows::UI::Xaml::XamlRoot xamlRoot);
void RequestNotificationIconRowsRefresh();
void SetLiveNotificationIconRowsHint(int rows);
void SetLiveTaskbarHeightHint(int height);
void SetLiveLayoutHints(int rows, int height);
}

namespace HeightComponent {
namespace winrt = ::winrt;
// BEGIN INLINED COMPONENT: taskbar-height-and-resize
// Source code is published under The GNU General Public License v3.0.
//
// For bug reports and feature requests, please open an issue here:
// https://github.com/ramensoftware/windhawk-mods/issues
//
// For pull requests, development takes place here:
// https://github.com/m417z/my-windhawk-mods



#include <windhawk_utils.h>

#undef GetCurrentTime

#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.Xaml.Automation.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.Shapes.h>

#include <atomic>
#include <commctrl.h>
#include <cstdarg>
#include <ctime>
#include <functional>
#include <limits>
#include <optional>
#include <regex>
#include <string>
#include <string_view>
#include <vector>
#include <windowsx.h>

#ifndef EDGE_RESIZE_PROCESS_DIAG
#define EDGE_RESIZE_PROCESS_DIAG 0
#endif

#ifndef EDGE_RESIZE_FILE_DIAG
#define EDGE_RESIZE_FILE_DIAG 0
#endif

#ifndef EDGE_RESIZE_RUNNING_INDICATOR_EXPERIMENT
#define EDGE_RESIZE_RUNNING_INDICATOR_EXPERIMENT 0
#endif

#ifndef EDGE_RESIZE_RUNNING_INDICATOR_DIAG_ONLY
#define EDGE_RESIZE_RUNNING_INDICATOR_DIAG_ONLY 0
#endif

#ifndef EDGE_RESIZE_DISABLE_TASKBAR_VIEW_HOOKS
#define EDGE_RESIZE_DISABLE_TASKBAR_VIEW_HOOKS 0
#endif

#ifndef EDGE_RESIZE_FRAME_HOOKS_ONLY
#define EDGE_RESIZE_FRAME_HOOKS_ONLY 0
#endif

#ifndef EDGE_RESIZE_ENABLE_LEADING_BUTTON_HOOKS
#define EDGE_RESIZE_ENABLE_LEADING_BUTTON_HOOKS 0
#endif

#ifndef EDGE_RESIZE_ENABLE_ICON_METRIC_HOOKS
#define EDGE_RESIZE_ENABLE_ICON_METRIC_HOOKS 0
#endif

using namespace winrt::Windows::UI::Xaml;

struct {
    int taskbarHeight;
    int iconSize;
    int taskbarButtonWidth;
    int iconSizeSmall;
    int taskbarButtonWidthSmall;
    bool edgeResizeEnabled;
    int edgeResizeGripPixels;
    int edgeResizeRowHeight;
    int edgeResizeMinRows;
    int edgeResizeMaxRows;
} g_settings;

std::atomic<bool> g_systemTrayModuleHooked;
std::atomic<bool> g_taskbarViewDllLoaded;
std::atomic<bool> g_searchUxUiDllLoaded;
std::atomic<bool> g_applyingSettings;
std::atomic<bool> g_pendingMeasureOverride;
std::atomic<bool> g_unloading;
std::atomic<int> g_hookCallCounter;
std::atomic<int> g_taskButtonVisualDeferredCount;
std::atomic<int> g_taskbarFrameVisualDeferredCount;
std::atomic<int> g_runningIndicatorDiagLogCount;
std::atomic<int> g_runningIndicatorDiagPollThreadRequests;
std::atomic<bool> g_runningIndicatorDiagHookAttemptRunning;

bool g_hasDynamicIconScaling;
bool g_smallIconSize;
int g_originalTaskbarHeight;
int g_taskbarHeight;
std::atomic<DWORD> g_shellIconLoaderV2_LoadAsyncIcon__ResumeCoro_ThreadId;
bool g_inSystemTrayController_UpdateFrameSize;
bool g_taskbarButtonWidthCustomized;
bool g_inAugmentedEntryPointButton_UpdateButtonPadding;

double* double_48_value_Original;

void EdgeResizeUpdateGripWindowPosition();

#ifndef WIN11_TASKBAR_WIN10_MULTIROW_COMBINED
WINUSERAPI UINT WINAPI GetDpiForWindow(HWND hwnd);
#endif
typedef enum MONITOR_DPI_TYPE {
    MDT_EFFECTIVE_DPI = 0,
    MDT_ANGULAR_DPI = 1,
    MDT_RAW_DPI = 2,
    MDT_DEFAULT = MDT_EFFECTIVE_DPI
} MONITOR_DPI_TYPE;
STDAPI GetDpiForMonitor(HMONITOR hmonitor,
                        MONITOR_DPI_TYPE dpiType,
                        UINT* dpiX,
                        UINT* dpiY);

size_t OffsetFromAssemblyRegex(void* func,
                               size_t defValue,
                               std::regex regex,
                               int limit = 30) {
    BYTE* p = (BYTE*)func;
    for (int i = 0; i < limit; i++) {
        WH_DISASM_RESULT result;
        if (!Wh_Disasm(p, &result)) {
            break;
        }

        p += result.length;

        std::string_view s = result.text;
        if (s == "ret") {
            break;
        }

        std::match_results<std::string_view::const_iterator> match;
        if (std::regex_match(s.begin(), s.end(), match, regex)) {
            // Wh_Log(L"%S", result.text);
            return std::stoull(match[1], nullptr, 16);
        }
    }

    Wh_Log(L"Failed for %p", func);
    return defValue;
}

std::optional<bool> IsOsFeatureEnabled(UINT32 featureId) {
    enum FEATURE_ENABLED_STATE {
        FEATURE_ENABLED_STATE_DEFAULT = 0,
        FEATURE_ENABLED_STATE_DISABLED = 1,
        FEATURE_ENABLED_STATE_ENABLED = 2,
    };

#pragma pack(push, 1)
    struct RTL_FEATURE_CONFIGURATION {
        unsigned int featureId;
        unsigned __int32 group : 4;
        FEATURE_ENABLED_STATE enabledState : 2;
        unsigned __int32 enabledStateOptions : 1;
        unsigned __int32 unused1 : 1;
        unsigned __int32 variant : 6;
        unsigned __int32 variantPayloadKind : 2;
        unsigned __int32 unused2 : 16;
        unsigned int payload;
    };
#pragma pack(pop)

    using RtlQueryFeatureConfiguration_t =
        int(NTAPI*)(UINT32, int, INT64*, RTL_FEATURE_CONFIGURATION*);
    static RtlQueryFeatureConfiguration_t pRtlQueryFeatureConfiguration = []() {
        HMODULE hNtDll = GetModuleHandle(L"ntdll.dll");
        return hNtDll ? (RtlQueryFeatureConfiguration_t)GetProcAddress(
                            hNtDll, "RtlQueryFeatureConfiguration")
                      : nullptr;
    }();

    if (!pRtlQueryFeatureConfiguration) {
        Wh_Log(L"RtlQueryFeatureConfiguration not found");
        return std::nullopt;
    }

    RTL_FEATURE_CONFIGURATION feature = {0};
    INT64 changeStamp = 0;
    HRESULT hr =
        pRtlQueryFeatureConfiguration(featureId, 1, &changeStamp, &feature);
    if (SUCCEEDED(hr)) {
        Wh_Log(L"RtlQueryFeatureConfiguration result for %u: %d", featureId,
               feature.enabledState);

        switch (feature.enabledState) {
            case FEATURE_ENABLED_STATE_DISABLED:
                return false;
            case FEATURE_ENABLED_STATE_ENABLED:
                return true;
            case FEATURE_ENABLED_STATE_DEFAULT:
                return std::nullopt;
        }
    } else {
        Wh_Log(L"RtlQueryFeatureConfiguration error for %u: %08X", featureId,
               hr);
    }

    return std::nullopt;
}

FrameworkElement EnumChildElements(
    FrameworkElement element,
    std::function<bool(FrameworkElement)> enumCallback) {
    int childrenCount = Media::VisualTreeHelper::GetChildrenCount(element);

    for (int i = 0; i < childrenCount; i++) {
        auto child = Media::VisualTreeHelper::GetChild(element, i)
                         .try_as<FrameworkElement>();
        if (!child) {
            Wh_Log(L"Failed to get child %d of %d", i + 1, childrenCount);
            continue;
        }

        if (enumCallback(child)) {
            return child;
        }
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

bool IsVerticalTaskbar() {
    APPBARDATA appBarData = {
        .cbSize = sizeof(APPBARDATA),
    };
    if (!SHAppBarMessage(ABM_GETTASKBARPOS, &appBarData)) {
        Wh_Log(L"SHAppBarMessage(ABM_GETTASKBARPOS) failed");
        return false;
    }

    return appBarData.uEdge == ABE_LEFT || appBarData.uEdge == ABE_RIGHT;
}

void OverrideResourceDirectoryLookup(
    PCSTR sourceFunctionName,
    const winrt::Windows::Foundation::IInspectable* key,
    winrt::Windows::Foundation::IInspectable* value) {
    if (g_unloading) {
        return;
    }

    const auto keyString = key->try_as<winrt::hstring>();
    if (!keyString) {
        return;
    }

    double newValueDouble;
    if (*keyString == L"MediumTaskbarButtonExtent") {
        newValueDouble = g_settings.taskbarButtonWidth;
    } else if (*keyString == L"SmallTaskbarButtonExtent") {
        newValueDouble = g_settings.taskbarButtonWidthSmall;
    } else {
        return;
    }

    const auto valueDouble = value->try_as<double>();
    if (!valueDouble) {
        return;
    }

    if (newValueDouble != *valueDouble) {
        Wh_Log(L"[%S] Overriding value %s: %f->%f", sourceFunctionName,
               keyString->c_str(), *valueDouble, newValueDouble);
        *value = winrt::box_value(newValueDouble);
    }
}

using ResourceDictionary_Lookup_TaskbarView_t =
    winrt::Windows::Foundation::IInspectable*(
        WINAPI*)(void* pThis,
                 void** result,
                 winrt::Windows::Foundation::IInspectable* key);
ResourceDictionary_Lookup_TaskbarView_t
    ResourceDictionary_Lookup_TaskbarView_Original;
winrt::Windows::Foundation::IInspectable* WINAPI
ResourceDictionary_Lookup_TaskbarView_Hook(
    void* pThis,
    void** result,
    winrt::Windows::Foundation::IInspectable* key) {
    // Wh_Log(L">");

    auto ret =
        ResourceDictionary_Lookup_TaskbarView_Original(pThis, result, key);
    if (!*ret) {
        return ret;
    }

    OverrideResourceDirectoryLookup(__FUNCTION__, key, ret);

    return ret;
}

using ResourceDictionary_Lookup_SearchUxUi_t =
    winrt::Windows::Foundation::IInspectable*(
        WINAPI*)(void* pThis,
                 void** result,
                 winrt::Windows::Foundation::IInspectable* key);
ResourceDictionary_Lookup_SearchUxUi_t
    ResourceDictionary_Lookup_SearchUxUi_Original;
winrt::Windows::Foundation::IInspectable* WINAPI
ResourceDictionary_Lookup_SearchUxUi_Hook(
    void* pThis,
    void** result,
    winrt::Windows::Foundation::IInspectable* key) {
    // Wh_Log(L">");

    auto ret =
        ResourceDictionary_Lookup_SearchUxUi_Original(pThis, result, key);
    if (!*ret) {
        return ret;
    }

    OverrideResourceDirectoryLookup(__FUNCTION__, key, ret);

    return ret;
}

using IconUtils_GetIconSize_t = void(WINAPI*)(bool isSmall,
                                              int type,
                                              SIZE* size);
IconUtils_GetIconSize_t IconUtils_GetIconSize_Original;
void WINAPI IconUtils_GetIconSize_Hook(bool isSmall, int type, SIZE* size) {
    [[maybe_unused]] static bool logged = [] {
        Wh_Log(L"> [%S] First call, hasDynamicIconScaling=%d",
               __PRETTY_FUNCTION__, g_hasDynamicIconScaling);
        return true;
    }();

    if (g_hasDynamicIconScaling) {
        IconUtils_GetIconSize_Original(isSmall, type, size);
        return;
    }

    IconUtils_GetIconSize_Original(isSmall, type, size);

    if (!g_unloading && !isSmall) {
        size->cx = MulDiv(size->cx, g_settings.iconSize, 24);
        size->cy = MulDiv(size->cy, g_settings.iconSize, 24);
    }
}

using IconContainer_IsStorageRecreationRequired_t = bool(WINAPI*)(void* pThis,
                                                                  void* param1,
                                                                  int flags);
IconContainer_IsStorageRecreationRequired_t
    IconContainer_IsStorageRecreationRequired_Original;
bool WINAPI IconContainer_IsStorageRecreationRequired_Hook(void* pThis,
                                                           void* param1,
                                                           int flags) {
    [[maybe_unused]] static bool logged = [] {
        Wh_Log(L"> [%S] First call, hasDynamicIconScaling=%d",
               __PRETTY_FUNCTION__, g_hasDynamicIconScaling);
        return true;
    }();

    if (g_hasDynamicIconScaling) {
        return IconContainer_IsStorageRecreationRequired_Original(pThis, param1,
                                                                  flags);
    }

    if (g_applyingSettings) {
        return true;
    }

    return IconContainer_IsStorageRecreationRequired_Original(pThis, param1,
                                                              flags);
}

using TrayUI_GetMinSize_t = void(WINAPI*)(void* pThis,
                                          HMONITOR monitor,
                                          SIZE* size);
TrayUI_GetMinSize_t TrayUI_GetMinSize_Original;
void WINAPI TrayUI_GetMinSize_Hook(void* pThis, HMONITOR monitor, SIZE* size) {
    Wh_Log(L">");

    TrayUI_GetMinSize_Original(pThis, monitor, size);

    // Reassign min height to fix displaced secondary taskbar when auto-hide is
    // enabled.
    if (!IsVerticalTaskbar() && g_taskbarHeight) {
        UINT dpiX = 0;
        UINT dpiY = 0;
        GetDpiForMonitor(monitor, MDT_DEFAULT, &dpiX, &dpiY);

        size->cy = MulDiv(g_taskbarHeight, dpiY, 96);
    }
}

using CIconLoadingFunctions_GetClassLongPtrW_t = ULONG_PTR(WINAPI*)(void* pThis,
                                                                    HWND hWnd,
                                                                    int nIndex);
CIconLoadingFunctions_GetClassLongPtrW_t
    CIconLoadingFunctions_GetClassLongPtrW_Original;
ULONG_PTR WINAPI CIconLoadingFunctions_GetClassLongPtrW_Hook(void* pThis,
                                                             HWND hWnd,
                                                             int nIndex) {
    Wh_Log(L"> hasDynamicIconScaling=%d, nIndex=%d", g_hasDynamicIconScaling,
           nIndex);

    if (g_hasDynamicIconScaling) {
        return CIconLoadingFunctions_GetClassLongPtrW_Original(pThis, hWnd,
                                                               nIndex);
    }

    if (!g_unloading && nIndex == GCLP_HICON && g_settings.iconSize <= 16) {
        nIndex = GCLP_HICONSM;
    }

    ULONG_PTR ret =
        CIconLoadingFunctions_GetClassLongPtrW_Original(pThis, hWnd, nIndex);

    return ret;
}

using CIconLoadingFunctions_SendMessageCallbackW_t =
    BOOL(WINAPI*)(void* pThis,
                  HWND hWnd,
                  UINT Msg,
                  WPARAM wParam,
                  LPARAM lParam,
                  SENDASYNCPROC lpResultCallBack,
                  ULONG_PTR dwData);
CIconLoadingFunctions_SendMessageCallbackW_t
    CIconLoadingFunctions_SendMessageCallbackW_Original;
BOOL WINAPI
CIconLoadingFunctions_SendMessageCallbackW_Hook(void* pThis,
                                                HWND hWnd,
                                                UINT Msg,
                                                WPARAM wParam,
                                                LPARAM lParam,
                                                SENDASYNCPROC lpResultCallBack,
                                                ULONG_PTR dwData) {
    Wh_Log(L"> hasDynamicIconScaling=%d, Msg=%u, wParam=%zu, lParam=%zu",
           g_hasDynamicIconScaling, Msg, wParam, lParam);

    if (g_hasDynamicIconScaling) {
        return CIconLoadingFunctions_SendMessageCallbackW_Original(
            pThis, hWnd, Msg, wParam, lParam, lpResultCallBack, dwData);
    }

    if (!g_unloading && Msg == WM_GETICON && wParam == ICON_BIG &&
        g_settings.iconSize <= 16) {
        wParam = ICON_SMALL2;
    }

    BOOL ret = CIconLoadingFunctions_SendMessageCallbackW_Original(
        pThis, hWnd, Msg, wParam, lParam, lpResultCallBack, dwData);

    return ret;
}

using ShellIconLoaderV2_LoadAsyncIcon__ResumeCoro_t =
    void(WINAPI*)(void* pThis);
ShellIconLoaderV2_LoadAsyncIcon__ResumeCoro_t
    ShellIconLoaderV2_LoadAsyncIcon__ResumeCoro_Original;
void WINAPI ShellIconLoaderV2_LoadAsyncIcon__ResumeCoro_Hook(void* pThis) {
    Wh_Log(L"> hasDynamicIconScaling=%d", g_hasDynamicIconScaling);

    if (g_hasDynamicIconScaling) {
        ShellIconLoaderV2_LoadAsyncIcon__ResumeCoro_Original(pThis);
        return;
    }

    g_shellIconLoaderV2_LoadAsyncIcon__ResumeCoro_ThreadId =
        GetCurrentThreadId();

    ShellIconLoaderV2_LoadAsyncIcon__ResumeCoro_Original(pThis);

    g_shellIconLoaderV2_LoadAsyncIcon__ResumeCoro_ThreadId = 0;
}

using TrayUI__StuckTrayChange_t = void(WINAPI*)(void* pThis);
TrayUI__StuckTrayChange_t TrayUI__StuckTrayChange_Original;

using TrayUI__HandleSettingChange_t = void(WINAPI*)(void* pThis,
                                                    void* param1,
                                                    void* param2,
                                                    void* param3,
                                                    void* param4);
TrayUI__HandleSettingChange_t TrayUI__HandleSettingChange_Original;
void WINAPI TrayUI__HandleSettingChange_Hook(void* pThis,
                                             void* param1,
                                             void* param2,
                                             void* param3,
                                             void* param4) {
    Wh_Log(L">");

    TrayUI__HandleSettingChange_Original(pThis, param1, param2, param3, param4);

    if (g_applyingSettings) {
        TrayUI__StuckTrayChange_Original(pThis);
    }
}

using TaskListItemViewModel_GetIconHeight_t = int(WINAPI*)(void* pThis,
                                                           void* param1,
                                                           double* iconHeight);
TaskListItemViewModel_GetIconHeight_t
    TaskListItemViewModel_GetIconHeight_Original;
int WINAPI TaskListItemViewModel_GetIconHeight_Hook(void* pThis,
                                                    void* param1,
                                                    double* iconHeight) {
    [[maybe_unused]] static bool logged = [] {
        Wh_Log(L"> [%S] First call, hasDynamicIconScaling=%d",
               __PRETTY_FUNCTION__, g_hasDynamicIconScaling);
        return true;
    }();

    if (g_hasDynamicIconScaling) {
        return TaskListItemViewModel_GetIconHeight_Original(pThis, param1,
                                                            iconHeight);
    }

    int ret =
        TaskListItemViewModel_GetIconHeight_Original(pThis, param1, iconHeight);

    if (!g_unloading) {
        *iconHeight = g_settings.iconSize;
    }

    return ret;
}

using TaskListGroupViewModel_GetIconHeight_t = int(WINAPI*)(void* pThis,
                                                            void* param1,
                                                            double* iconHeight);
TaskListGroupViewModel_GetIconHeight_t
    TaskListGroupViewModel_GetIconHeight_Original;
int WINAPI TaskListGroupViewModel_GetIconHeight_Hook(void* pThis,
                                                     void* param1,
                                                     double* iconHeight) {
    Wh_Log(L"> hasDynamicIconScaling=%d", g_hasDynamicIconScaling);

    if (g_hasDynamicIconScaling) {
        return TaskListGroupViewModel_GetIconHeight_Original(pThis, param1,
                                                             iconHeight);
    }

    int ret = TaskListGroupViewModel_GetIconHeight_Original(pThis, param1,
                                                            iconHeight);

    if (!g_unloading) {
        *iconHeight = g_settings.iconSize;
    }

    return ret;
}

using TaskbarConfiguration_GetIconHeightInViewPixels_taskbarSizeEnum_t =
    double(WINAPI*)(int enumTaskbarSize);
TaskbarConfiguration_GetIconHeightInViewPixels_taskbarSizeEnum_t
    TaskbarConfiguration_GetIconHeightInViewPixels_taskbarSizeEnum_Original;
double WINAPI
TaskbarConfiguration_GetIconHeightInViewPixels_taskbarSizeEnum_Hook(
    int enumTaskbarSize) {
    Wh_Log(L"> hasDynamicIconScaling=%d, enumTaskbarSize=%d",
           g_hasDynamicIconScaling, enumTaskbarSize);

    // Even if the feature flag is enabled, the feature may not be actually
    // enabled for some reason. Handle this here by resetting the flag.
    if (g_hasDynamicIconScaling) {
        Wh_Log(L"Setting hasDynamicIconScaling to false");
        g_hasDynamicIconScaling = false;
    }

    if (!g_unloading && (enumTaskbarSize == 1 || enumTaskbarSize == 2)) {
        return g_settings.iconSize;
    }

    return TaskbarConfiguration_GetIconHeightInViewPixels_taskbarSizeEnum_Original(
        enumTaskbarSize);
}

using TaskbarConfiguration_GetIconHeightInViewPixels_double_t =
    double(WINAPI*)(double baseHeight);
TaskbarConfiguration_GetIconHeightInViewPixels_double_t
    TaskbarConfiguration_GetIconHeightInViewPixels_double_Original;
double WINAPI
TaskbarConfiguration_GetIconHeightInViewPixels_double_Hook(double baseHeight) {
    Wh_Log(L"> hasDynamicIconScaling=%d, baseHeight=%f",
           g_hasDynamicIconScaling, baseHeight);

    // Even if the feature flag is enabled, the feature may not be actually
    // enabled for some reason. Handle this here by resetting the flag.
    if (g_hasDynamicIconScaling) {
        Wh_Log(L"Setting hasDynamicIconScaling to false");
        g_hasDynamicIconScaling = false;
    }

    if (!g_unloading) {
        return g_settings.iconSize;
    }

    return TaskbarConfiguration_GetIconHeightInViewPixels_double_Original(
        baseHeight);
}

using TaskbarConfiguration_GetIconHeightInViewPixels_method_t =
    double(WINAPI*)(void* pThis);
TaskbarConfiguration_GetIconHeightInViewPixels_method_t
    TaskbarConfiguration_GetIconHeightInViewPixels_method_Original;
double WINAPI
TaskbarConfiguration_GetIconHeightInViewPixels_method_Hook(void* pThis) {
    [[maybe_unused]] static bool logged = [] {
        Wh_Log(L"> [%S] First call, hasDynamicIconScaling=%d",
               __PRETTY_FUNCTION__, g_hasDynamicIconScaling);
        return true;
    }();

    double iconSize =
        TaskbarConfiguration_GetIconHeightInViewPixels_method_Original(pThis);

    if (!g_unloading) {
        return iconSize <= 16 ? g_settings.iconSizeSmall : g_settings.iconSize;
    }

    return iconSize;
}

using TaskListButton_IconHeight_t = void(WINAPI*)(void* pThis, double height);
TaskListButton_IconHeight_t TaskListButton_IconHeight_Original;

size_t GetIconHeightOffset() {
    static size_t iconHeightOffset = []() -> size_t {
        if (!TaskListButton_IconHeight_Original) {
            Wh_Log(L"Error: TaskListButton_IconHeight_Original is null");
            return 0;
        }

        size_t offset =
#if defined(_M_X64)
            OffsetFromAssemblyRegex(
                (void*)TaskListButton_IconHeight_Original, 0,
                std::regex(R"(movsd xmm\d+, qword ptr \[rcx\+0x([0-9a-f]+)\])",
                           std::regex_constants::icase),
                30);
#elif defined(_M_ARM64)
            OffsetFromAssemblyRegex(
                (void*)TaskListButton_IconHeight_Original, 0,
                std::regex(R"(ldr\s+d\d+, \[x\d+, #0x([0-9a-f]+)\])",
                           std::regex_constants::icase),
                30);
#else
#error "Unsupported architecture"
#endif
        Wh_Log(L"iconHeightOffset=0x%X", offset);
        return offset > 0xFFFF ? 0 : offset;
    }();

    return iconHeightOffset;
}

void TaskListButton_IconHeight_InitOffsets() {
    GetIconHeightOffset();
}

using SystemTrayController_GetFrameSize_t =
    double(WINAPI*)(void* pThis, int enumTaskbarSize);
SystemTrayController_GetFrameSize_t SystemTrayController_GetFrameSize_Original;
double WINAPI SystemTrayController_GetFrameSize_Hook(void* pThis,
                                                     int enumTaskbarSize) {
    Wh_Log(L"> %d", enumTaskbarSize);

    if (!IsVerticalTaskbar() && g_taskbarHeight &&
        (enumTaskbarSize == 1 || enumTaskbarSize == 2)) {
        return g_taskbarHeight;
    }

    return SystemTrayController_GetFrameSize_Original(pThis, enumTaskbarSize);
}

using SystemTraySecondaryController_GetFrameSize_t =
    double(WINAPI*)(void* pThis, int enumTaskbarSize);
SystemTraySecondaryController_GetFrameSize_t
    SystemTraySecondaryController_GetFrameSize_Original;
double WINAPI
SystemTraySecondaryController_GetFrameSize_Hook(void* pThis,
                                                int enumTaskbarSize) {
    Wh_Log(L"> %d", enumTaskbarSize);

    if (!IsVerticalTaskbar() && g_taskbarHeight &&
        (enumTaskbarSize == 1 || enumTaskbarSize == 2)) {
        return g_taskbarHeight;
    }

    return SystemTraySecondaryController_GetFrameSize_Original(pThis,
                                                               enumTaskbarSize);
}

using TaskbarConfiguration_GetFrameSize_t =
    double(WINAPI*)(int enumTaskbarSize);
TaskbarConfiguration_GetFrameSize_t TaskbarConfiguration_GetFrameSize_Original;
double WINAPI TaskbarConfiguration_GetFrameSize_Hook(int enumTaskbarSize) {
    Wh_Log(L"> %d", enumTaskbarSize);

    if (!g_originalTaskbarHeight &&
        (enumTaskbarSize == 1 || enumTaskbarSize == 2)) {
        g_originalTaskbarHeight =
            TaskbarConfiguration_GetFrameSize_Original(enumTaskbarSize);
    }

    if (!IsVerticalTaskbar() && g_taskbarHeight &&
        (enumTaskbarSize == 1 || enumTaskbarSize == 2)) {
        return g_taskbarHeight;
    }

    return TaskbarConfiguration_GetFrameSize_Original(enumTaskbarSize);
}

#ifdef _M_ARM64
thread_local double* g_TaskbarConfiguration_UpdateFrameSize_frameSize;

using TaskbarConfiguration_UpdateFrameSize_t = void(WINAPI*)(void* pThis);
TaskbarConfiguration_UpdateFrameSize_t
    TaskbarConfiguration_UpdateFrameSize_SymbolAddress;

LONG GetFrameSizeOffset() {
    static LONG frameSizeOffset = []() -> LONG {
        if (!TaskbarConfiguration_UpdateFrameSize_SymbolAddress) {
            Wh_Log(
                L"Error: TaskbarConfiguration_UpdateFrameSize_SymbolAddress is "
                L"null");
            return 0;
        }

        // Find the offset to the frame size.
        // str d16, [x19, #0x50]
        const DWORD* start =
            (const DWORD*)TaskbarConfiguration_UpdateFrameSize_SymbolAddress;
        const DWORD* end = start + 0x80;
        std::regex regex1(R"(str\s+d\d+, \[x\d+, #0x([0-9a-f]+)\])");
        for (const DWORD* p = start; p != end; p++) {
            WH_DISASM_RESULT result1;
            if (!Wh_Disasm((void*)p, &result1)) {
                break;
            }

            std::string_view s1 = result1.text;
            if (s1 == "ret") {
                break;
            }

            std::match_results<std::string_view::const_iterator> match1;
            if (!std::regex_match(s1.begin(), s1.end(), match1, regex1)) {
                continue;
            }

            // Wh_Log(L"%S", result1.text);
            LONG offset = std::stoull(match1[1], nullptr, 16);
            Wh_Log(L"frameSizeOffset=0x%X", offset);
            return (offset < 0 || offset > 0xFFFF) ? 0 : offset;
        }

        Wh_Log(L"frameSizeOffset not found");
        return 0;
    }();

    return frameSizeOffset;
}

void TaskbarConfiguration_UpdateFrameSize_InitOffsets() {
    GetFrameSizeOffset();
}

TaskbarConfiguration_UpdateFrameSize_t
    TaskbarConfiguration_UpdateFrameSize_Original;
void WINAPI TaskbarConfiguration_UpdateFrameSize_Hook(void* pThis) {
    Wh_Log(L">");

    LONG frameSizeOffset = GetFrameSizeOffset();
    if (!frameSizeOffset) {
        Wh_Log(L"Error: frameSizeOffset is invalid");
        TaskbarConfiguration_UpdateFrameSize_Original(pThis);
        return;
    }

    g_TaskbarConfiguration_UpdateFrameSize_frameSize =
        (double*)((BYTE*)pThis + frameSizeOffset);

    TaskbarConfiguration_UpdateFrameSize_Original(pThis);

    g_TaskbarConfiguration_UpdateFrameSize_frameSize = nullptr;
}

using Event_operator_call_t = void(WINAPI*)(void* pThis);
Event_operator_call_t Event_operator_call_Original;
void WINAPI Event_operator_call_Hook(void* pThis) {
    Wh_Log(L">");

    if (g_TaskbarConfiguration_UpdateFrameSize_frameSize) {
        if (!g_originalTaskbarHeight) {
            g_originalTaskbarHeight =
                *g_TaskbarConfiguration_UpdateFrameSize_frameSize;
        }

        if (!IsVerticalTaskbar() && g_taskbarHeight) {
            *g_TaskbarConfiguration_UpdateFrameSize_frameSize = g_taskbarHeight;
        }
    }

    Event_operator_call_Original(pThis);
}
#endif  // _M_ARM64

using SystemTrayController_UpdateFrameSize_t = void(WINAPI*)(void* pThis);
SystemTrayController_UpdateFrameSize_t
    SystemTrayController_UpdateFrameSize_SymbolAddress;

LONG GetLastHeightOffset() {
    static LONG lastHeightOffset = []() -> LONG {
        if (!SystemTrayController_UpdateFrameSize_SymbolAddress) {
            Wh_Log(
                L"Error: SystemTrayController_UpdateFrameSize_SymbolAddress is "
                L"null");
            return 0;
        }

        // Find the last height offset to reset the height value.
#if defined(_M_X64)
        // 66 0f 2e b3 b0 00 00 00 UCOMISD    uVar4,qword ptr [RBX + 0xb0]
        // 7a 4c                   JP         LAB_180075641
        // 75 4a                   JNZ        LAB_180075641
        //
        // Newer insider builds (first seen in 2126.5501.20.6000):
        // 660f2e87b0000000 ucomisd xmm0, mmword ptr [rdi+0B0h]
        // 7a02             jp      18006c931
        // 7410             je      18006c941
        //
        // Newer insider builds (first seen in 2604.8002.400.0):
        // 66 0f 2e b7 b0 00 00 00   UCOMISD    XMM6,qword ptr [RDI + 0xb0]
        // 7a 06                     JP         LAB_1800828e3
        // 0f 84 84 00 00 00         JZ         LAB_180082967
        const BYTE* start =
            (const BYTE*)SystemTrayController_UpdateFrameSize_SymbolAddress;
        const BYTE* end = start + 0x400;
        for (const BYTE* p = start; p != end; p++) {
            if (p[0] == 0x66 && p[1] == 0x0F && p[2] == 0x2E &&
                (p[3] & 0xC0) == 0x80 && p[8] == 0x7A &&
                (p[10] == 0x74 || p[10] == 0x75 ||
                 (p[10] == 0x0F && (p[11] == 0x84 || p[11] == 0x85)))) {
                LONG offset = *(LONG*)(p + 4);
                Wh_Log(L"lastHeightOffset=0x%X", offset);
                return (offset < 0 || offset > 0xFFFF) ? 0 : offset;
            }
        }
#elif defined(_M_ARM64)
        // fd405a70 ldr  d16,[x19,#0xB0]
        // 1e702000 fcmp d0,d16
        // 54000080 beq  [...]::UpdateFrameSize+0x6c
        const DWORD* start =
            (const DWORD*)SystemTrayController_UpdateFrameSize_SymbolAddress;
        const DWORD* end = start + 0x100;
        std::regex regex1(R"(ldr\s+d\d+, \[x\d+, #0x([0-9a-f]+)\])");
        std::regex regex2(R"(fcmp\s+d\d+, d\d+)");
        std::regex regex3(R"(b\.eq\s+0x[0-9a-f]+)");
        for (const DWORD* p = start; p != end; p++) {
            WH_DISASM_RESULT result1;
            if (!Wh_Disasm((void*)p, &result1)) {
                break;
            }

            std::string_view s1 = result1.text;
            if (s1 == "ret") {
                break;
            }

            std::match_results<std::string_view::const_iterator> match1;
            if (!std::regex_match(s1.begin(), s1.end(), match1, regex1)) {
                continue;
            }

            WH_DISASM_RESULT result2;
            if (!Wh_Disasm((void*)(p + 1), &result2)) {
                break;
            }
            std::string_view s2 = result2.text;
            if (!std::regex_match(s2.begin(), s2.end(), regex2)) {
                continue;
            }
            WH_DISASM_RESULT result3;
            if (!Wh_Disasm((void*)(p + 2), &result3)) {
                break;
            }
            std::string_view s3 = result3.text;
            if (!std::regex_match(s3.begin(), s3.end(), regex3)) {
                continue;
            }

            // Wh_Log(L"%S", result1.text);
            // Wh_Log(L"%S", result2.text);
            // Wh_Log(L"%S", result3.text);
            LONG offset = std::stoull(match1[1], nullptr, 16);
            Wh_Log(L"lastHeightOffset=0x%X", offset);
            return (offset < 0 || offset > 0xFFFF) ? 0 : offset;
        }
#else
#error "Unsupported architecture"
#endif

        Wh_Log(L"lastHeightOffset not found");
        return 0;
    }();

    return lastHeightOffset;
}

void SystemTrayController_UpdateFrameSize_InitOffsets() {
    GetLastHeightOffset();
}

SystemTrayController_UpdateFrameSize_t
    SystemTrayController_UpdateFrameSize_Original;
void WINAPI SystemTrayController_UpdateFrameSize_Hook(void* pThis) {
    Wh_Log(L">");

    if (IsVerticalTaskbar()) {
        SystemTrayController_UpdateFrameSize_Original(pThis);
        return;
    }

    LONG lastHeightOffset = GetLastHeightOffset();
    if (lastHeightOffset) {
        *(double*)((BYTE*)pThis + lastHeightOffset) = 0;
    } else {
        Wh_Log(L"Error: lastHeightOffset is invalid");
    }

    g_inSystemTrayController_UpdateFrameSize = true;

    SystemTrayController_UpdateFrameSize_Original(pThis);

    g_inSystemTrayController_UpdateFrameSize = false;
}

using TaskbarFrame_MaxHeight_double_t = void(WINAPI*)(void* pThis,
                                                      double value);
TaskbarFrame_MaxHeight_double_t TaskbarFrame_MaxHeight_double_Original;

using TaskbarFrame_Height_double_t = void(WINAPI*)(void* pThis, double value);
TaskbarFrame_Height_double_t TaskbarFrame_Height_double_Original;
void WINAPI TaskbarFrame_Height_double_Hook(void* pThis, double value) {
    Wh_Log(L">");

    if (IsVerticalTaskbar()) {
        TaskbarFrame_Height_double_Original(pThis, value);
        return;
    }

    if (TaskbarFrame_MaxHeight_double_Original) {
        TaskbarFrame_MaxHeight_double_Original(
            pThis, std::numeric_limits<double>::infinity());
    }

    return TaskbarFrame_Height_double_Original(pThis, value);
}

void* TaskbarController_OnGroupingModeChanged_Original;

LONG GetTaskbarFrameOffset() {
    static LONG taskbarFrameOffset = []() -> LONG {
        if (!TaskbarController_OnGroupingModeChanged_Original) {
            Wh_Log(
                L"Error: TaskbarController_OnGroupingModeChanged_Original is "
                L"null");
            return 0;
        }

#if defined(_M_X64)
        // 48:83EC 28               | sub rsp,28
        // 48:8B81 88020000         | mov rax,qword ptr ds:[rcx+288]
        // or
        // 4C:8B81 80020000         | mov r8,qword ptr ds:[rcx+280]
        const BYTE* p =
            (const BYTE*)TaskbarController_OnGroupingModeChanged_Original;
        if (p && p[0] == 0x48 && p[1] == 0x83 && p[2] == 0xEC &&
            (p[4] == 0x48 || p[4] == 0x4C) && p[5] == 0x8B &&
            (p[6] & 0xC0) == 0x80) {
            LONG offset = *(LONG*)(p + 7);
            Wh_Log(L"taskbarFrameOffset=0x%X", offset);
            return (offset < 0 || offset > 0xFFFF) ? 0 : offset;
        }
#elif defined(_M_ARM64)
        // 00000001`806b1810 a9bf7bfd stp fp,lr,[sp,#-0x10]!
        // 00000001`806b1814 910003fd mov fp,sp
        // 00000001`806b1818 aa0003e8 mov x8,x0
        // 00000001`806b181c f9414500 ldr x0,[x8,#0x288]
        const DWORD* start =
            (const DWORD*)TaskbarController_OnGroupingModeChanged_Original;
        const DWORD* end = start + 10;
        std::regex regex1(R"(ldr\s+x\d+, \[x\d+, #0x([0-9a-f]+)\])");
        for (const DWORD* p = start; p != end; p++) {
            WH_DISASM_RESULT result1;
            if (!Wh_Disasm((void*)p, &result1)) {
                break;
            }

            std::string_view s1 = result1.text;
            if (s1 == "ret") {
                break;
            }

            std::match_results<std::string_view::const_iterator> match1;
            if (!std::regex_match(s1.begin(), s1.end(), match1, regex1)) {
                continue;
            }

            // Wh_Log(L"%S", result1.text);
            LONG offset = std::stoull(match1[1], nullptr, 16);
            Wh_Log(L"taskbarFrameOffset=0x%X", offset);
            return (offset < 0 || offset > 0xFFFF) ? 0 : offset;
        }
#else
#error "Unsupported architecture"
#endif

        Wh_Log(L"taskbarFrameOffset not found");
        return 0;
    }();

    return taskbarFrameOffset;
}

void TaskbarController_OnGroupingModeChanged_InitOffsets() {
    GetTaskbarFrameOffset();
}

using TaskbarController_UpdateFrameHeight_t = void(WINAPI*)(void* pThis);
TaskbarController_UpdateFrameHeight_t
    TaskbarController_UpdateFrameHeight_Original;
void WINAPI TaskbarController_UpdateFrameHeight_Hook(void* pThis) {
    Wh_Log(L">");

    if (IsVerticalTaskbar()) {
        TaskbarController_UpdateFrameHeight_Original(pThis);
        return;
    }

    LONG taskbarFrameOffset = GetTaskbarFrameOffset();
    if (!taskbarFrameOffset) {
        Wh_Log(L"Error: taskbarFrameOffset is invalid");
        TaskbarController_UpdateFrameHeight_Original(pThis);
        return;
    }

    void* taskbarFrame = *(void**)((BYTE*)pThis + taskbarFrameOffset);
    if (!taskbarFrame) {
        Wh_Log(L"Error: taskbarFrame is null");
        TaskbarController_UpdateFrameHeight_Original(pThis);
        return;
    }

    FrameworkElement taskbarFrameElement = nullptr;
    ((IUnknown**)taskbarFrame)[1]->QueryInterface(
        winrt::guid_of<FrameworkElement>(),
        winrt::put_abi(taskbarFrameElement));
    if (!taskbarFrameElement) {
        Wh_Log(L"Error: taskbarFrameElement is null");
        TaskbarController_UpdateFrameHeight_Original(pThis);
        return;
    }

    taskbarFrameElement.MaxHeight(std::numeric_limits<double>::infinity());

    TaskbarController_UpdateFrameHeight_Original(pThis);

    // Adjust parent grid height if needed.
    auto contentGrid = Media::VisualTreeHelper::GetParent(taskbarFrameElement)
                           .try_as<FrameworkElement>();
    if (contentGrid) {
        double height = taskbarFrameElement.Height();
        double contentGridHeight = contentGrid.Height();
        if (contentGridHeight > 0 && contentGridHeight != height) {
            Wh_Log(L"Adjusting contentGrid.Height: %f->%f", contentGridHeight,
                   height);
            contentGrid.Height(height);
        }
    }

}

using SystemTraySecondaryController_UpdateFrameSize_t =
    void(WINAPI*)(void* pThis);
SystemTraySecondaryController_UpdateFrameSize_t
    SystemTraySecondaryController_UpdateFrameSize_Original;
void WINAPI SystemTraySecondaryController_UpdateFrameSize_Hook(void* pThis) {
    Wh_Log(L">");

    g_inSystemTrayController_UpdateFrameSize = true;

    SystemTraySecondaryController_UpdateFrameSize_Original(pThis);

    g_inSystemTrayController_UpdateFrameSize = false;
}

using SystemTrayFrame_Height_t = void(WINAPI*)(void* pThis, double value);
SystemTrayFrame_Height_t SystemTrayFrame_Height_Original;
void WINAPI SystemTrayFrame_Height_Hook(void* pThis, double value) {
    // Wh_Log(L">");

    if (!IsVerticalTaskbar() && g_inSystemTrayController_UpdateFrameSize) {
        Wh_Log(L">");
        // Set the system tray height to NaN, otherwise it may not match the
        // custom taskbar height.
        value = std::numeric_limits<double>::quiet_NaN();
    }

    SystemTrayFrame_Height_Original(pThis, value);
}

using TaskbarFrame_MeasureOverride_t =
    int(WINAPI*)(void* pThis,
                 winrt::Windows::Foundation::Size size,
                 winrt::Windows::Foundation::Size* resultSize);
TaskbarFrame_MeasureOverride_t TaskbarFrame_MeasureOverride_Original;
int WINAPI TaskbarFrame_MeasureOverride_Hook(
    void* pThis,
    winrt::Windows::Foundation::Size size,
    winrt::Windows::Foundation::Size* resultSize) {
    g_hookCallCounter++;

    Wh_Log(L">");

    int ret = TaskbarFrame_MeasureOverride_Original(pThis, size, resultSize);

    g_pendingMeasureOverride = false;

    g_hookCallCounter--;

    return ret;
}

using TaskListButton_UpdateButtonPadding_t = void(WINAPI*)(void* pThis);
TaskListButton_UpdateButtonPadding_t
    TaskListButton_UpdateButtonPadding_Original;
void WINAPI TaskListButton_UpdateButtonPadding_Hook(void* pThis) {
    Wh_Log(L"> hasDynamicIconScaling=%d", g_hasDynamicIconScaling);

    if (!g_hasDynamicIconScaling || g_unloading) {
        TaskListButton_UpdateButtonPadding_Original(pThis);
        return;
    }

    // Make sure to use a different value for other calculations such as
    // padding. Value 16 and 32 have special treatment.
    double* iconHeight = nullptr;
    double prevIconHeight;
    if (size_t iconHeightOffset = GetIconHeightOffset()) {
        iconHeight = (double*)((BYTE*)pThis + iconHeightOffset);
        prevIconHeight = *iconHeight;
        double newIconHeight = g_smallIconSize ? 16 : 24;
        Wh_Log(L"Setting iconHeight: %f->%f", prevIconHeight, newIconHeight);
        *iconHeight = newIconHeight;
    }

    TaskListButton_UpdateButtonPadding_Original(pThis);

    if (iconHeight) {
        *iconHeight = prevIconHeight;
    }
}

using TaskListButton_OverlayIcon_t = void(WINAPI*)(void* pThis, void* param1);
TaskListButton_OverlayIcon_t TaskListButton_OverlayIcon_Original;
void WINAPI TaskListButton_OverlayIcon_Hook(void* pThis, void* param1) {
    Wh_Log(L"> hasDynamicIconScaling=%d", g_hasDynamicIconScaling);

    if (!g_hasDynamicIconScaling || g_unloading) {
        TaskListButton_OverlayIcon_Original(pThis, param1);
        return;
    }

    // Value 16 causes badges to be shown as a small dot. There are still some
    // glitches with the badges, e.g. switching from large icons to small icons
    // doesn't update from the badge to the dot, but new badges are shown as
    // dots with small icons. Fixing it might require hooking several additional
    // functions. Maybe one day...
    //
    // This hook handles non-UWP badges (e.g. the Win7 taskbar sample).
    double* iconHeight = nullptr;
    double prevIconHeight;
    if (size_t iconHeightOffset = GetIconHeightOffset()) {
        iconHeight = (double*)((BYTE*)pThis + iconHeightOffset);
        prevIconHeight = *iconHeight;
        double newIconHeight = 24;
        Wh_Log(L"Setting iconHeight: %f->%f", prevIconHeight, newIconHeight);
        *iconHeight = newIconHeight;
    }

    TaskListButton_OverlayIcon_Original(pThis, param1);

    if (iconHeight) {
        *iconHeight = prevIconHeight;
    }
}

using TaskListButton_UpdateBadge_t = void(WINAPI*)(void* pThis);
TaskListButton_UpdateBadge_t TaskListButton_UpdateBadge_Original;
void WINAPI TaskListButton_UpdateBadge_Hook(void* pThis) {
    Wh_Log(L"> hasDynamicIconScaling=%d", g_hasDynamicIconScaling);

    if (!g_hasDynamicIconScaling || g_unloading) {
        TaskListButton_UpdateBadge_Original(pThis);
        return;
    }

    // Value 16 causes badges to be shown as a small dot. There are still some
    // glitches with the badges, e.g. switching from large icons to small icons
    // doesn't update from the badge to the dot, but new badges are shown as
    // dots with small icons. Fixing it might require hooking several additional
    // functions. Maybe one day...
    //
    // This hook handles UWP badges (e.g. Unigram).
    double* iconHeight = nullptr;
    double prevIconHeight;
    if (size_t iconHeightOffset = GetIconHeightOffset()) {
        iconHeight = (double*)((BYTE*)pThis + iconHeightOffset);
        prevIconHeight = *iconHeight;
        double newIconHeight = 24;
        Wh_Log(L"Setting iconHeight: %f->%f", prevIconHeight, newIconHeight);
        *iconHeight = newIconHeight;
    }

    TaskListButton_UpdateBadge_Original(pThis);

    if (iconHeight) {
        *iconHeight = prevIconHeight;
    }
}

void* TaskListButton_UpdateIconColumnDefinition_Original;

LONG GetMediumTaskbarButtonExtentOffset() {
    static LONG mediumTaskbarButtonExtentOffset = []() -> LONG {
#if defined(_M_X64)
        // Search for movsd followed by subsd. In newer builds with vertical
        // taskbar support, there may be an additional movsd without a matching
        // subsd above.
        //
        // f20f10b648030000 movsd   xmm6,mmword ptr [rsi+348h]
        // f20f5cb680030000 subsd   xmm6,mmword ptr [rsi+380h]
        // f20f5cb690030000 subsd   xmm6,mmword ptr [rsi+390h]

        const BYTE* start =
            (const BYTE*)TaskListButton_UpdateIconColumnDefinition_Original;
        const BYTE* end = start + 0x200;
        LONG offsetCandidate = 0;
        LONG offset = 0;
        for (const BYTE* p = start; p != end; p++) {
            if (p[0] == 0xF2 && p[1] == 0x0F && p[2] == 0x10 &&
                (p[3] & 0xC0) == 0x80) {
                offsetCandidate = *(LONG*)(p + 4);
            }

            if (p[0] == 0xF2 && p[1] == 0x44 && p[2] == 0x0F && p[3] == 0x10 &&
                (p[4] & 0xC0) == 0x80) {
                offsetCandidate = *(LONG*)(p + 5);
            }

            if (p[0] == 0xF2 && p[1] == 0x0F && p[2] == 0x5C &&
                (p[3] & 0xC0) == 0x80) {
                offset = offsetCandidate;
                break;
            }

            if (p[0] == 0xF2 && p[1] == 0x44 && p[2] == 0x0F && p[3] == 0x5C &&
                (p[4] & 0xC0) == 0x80) {
                offset = offsetCandidate;
                break;
            }
        }

        Wh_Log(L"mediumTaskbarButtonExtentOffset=0x%X", offset);
        return (offset < 0 || offset > 0xFFFF) ? 0 : offset;
#elif defined(_M_ARM64)
        // ...
        // fd41b670 ldr  d16,[x19,#0x368]
        // fd419e71 ldr  d17,[x19,#0x338]
        // 1e703a31 fsub d17,d17,d16
        // fd41be70 ldr  d16,[x19,#0x378]
        // 1e703a28 fsub d8,d17,d16
        const DWORD* start =
            (const DWORD*)TaskListButton_UpdateIconColumnDefinition_Original;
        const DWORD* end = start + 0x80;
        std::regex regexLdr(R"(ldr\s+(d\d+), \[(x\d+), #0x([0-9a-f]+)\])");
        std::regex regexLdrOther(R"(ldr\s+(d\d+),.*)");
        std::regex regexFsub(R"(fsub\s+d\d+, (d\d+), (d\d+))");
        struct {
            std::string reg;
            std::string regSrc;
            LONG offset;
        } ldrs[32];
        size_t ldrCount = 0;
        for (const DWORD* p = start; p != end; p++) {
            WH_DISASM_RESULT result;
            if (!Wh_Disasm((void*)p, &result)) {
                break;
            }

            std::string_view s = result.text;
            if (s == "ret") {
                break;
            }

            if (ldrCount == ARRAYSIZE(ldrs)) {
                Wh_Log(L"Too many ldr instructions");
                break;
            }

            std::match_results<std::string_view::const_iterator> matchLdr;
            if (std::regex_match(s.begin(), s.end(), matchLdr, regexLdr)) {
                // Wh_Log(L"%S", result.text);
                std::string reg = matchLdr[1];
                std::string regSrc = matchLdr[2];
                LONG offset = std::stoull(matchLdr[3], nullptr, 16);
                ldrs[ldrCount++] = {std::move(reg), std::move(regSrc), offset};
                continue;
            }

            std::match_results<std::string_view::const_iterator> matchLdrOther;
            if (std::regex_match(s.begin(), s.end(), matchLdrOther,
                                 regexLdrOther)) {
                // Wh_Log(L"%S", result.text);
                std::string reg = matchLdrOther[1];
                ldrs[ldrCount++] = {std::move(reg), std::string(), 0};
                continue;
            }

            std::match_results<std::string_view::const_iterator> matchFsub;
            if (std::regex_match(s.begin(), s.end(), matchFsub, regexFsub)) {
                // Wh_Log(L"%S", result.text);
                std::string regA = matchFsub[1];
                std::string regB = matchFsub[2];

                std::remove_reference_t<decltype(ldrs[0])>* ldrA = nullptr;
                std::remove_reference_t<decltype(ldrs[0])>* ldrB = nullptr;

                for (size_t i = 0; i < ldrCount; i++) {
                    const auto& [ldrReg, ldrRegSrc, ldrOffset] =
                        ldrs[ldrCount - 1 - i];
                    if (!ldrA && ldrReg == regA) {
                        ldrA = &ldrs[ldrCount - 1 - i];
                    }
                    if (!ldrB && ldrReg == regB) {
                        ldrB = &ldrs[ldrCount - 1 - i];
                    }
                }

                if (ldrA && ldrB && ldrA->regSrc == ldrB->regSrc) {
                    LONG offset = ldrA->offset;
                    Wh_Log(L"mediumTaskbarButtonExtentOffset=0x%X", offset);
                    return (offset < 0 || offset > 0xFFFF) ? 0 : offset;
                }
            }
        }
#else
#error "Unsupported architecture"
#endif

        Wh_Log(L"Error: mediumTaskbarButtonExtentOffset not found");
        return 0;
    }();

    return mediumTaskbarButtonExtentOffset;
}

void TaskListButton_UpdateIconColumnDefinition_InitOffsets() {
    GetMediumTaskbarButtonExtentOffset();
}

void EdgeResizeLog(PCWSTR format, ...);

constexpr int kRunningIndicatorDiagLogLimit = 8000;

void RunningIndicatorDiagLog(PCWSTR format, ...) {
    int logIndex = g_runningIndicatorDiagLogCount.fetch_add(1);
    if (logIndex >= kRunningIndicatorDiagLogLimit) {
        return;
    }

    wchar_t message[1024]{};
    va_list args;
    va_start(args, format);
    _vsnwprintf_s(message, _TRUNCATE, format, args);
    va_end(args);

    EdgeResizeLog(L"RUNNING_INDICATOR_DIAG #%d %s", logIndex + 1, message);
}

std::wstring RunningIndicatorDiagSanitize(winrt::hstring value) {
    std::wstring result{value.c_str(), value.size()};
    for (wchar_t& ch : result) {
        if (ch == L'\r' || ch == L'\n' || ch == L'\t') {
            ch = L' ';
        }
    }

    if (result.size() > 160) {
        result.resize(160);
        result += L"...";
    }

    return result;
}

std::wstring RunningIndicatorDiagAutomationName(FrameworkElement element) {
    try {
        return RunningIndicatorDiagSanitize(
            Automation::AutomationProperties::GetName(element));
    } catch (...) {
        return L"<name-exception>";
    }
}

std::wstring RunningIndicatorDiagAutomationId(FrameworkElement element) {
    try {
        return RunningIndicatorDiagSanitize(
            Automation::AutomationProperties::GetAutomationId(element));
    } catch (...) {
        return L"<id-exception>";
    }
}

PCWSTR RunningIndicatorDiagOptionalBoolText(std::optional<bool> value) {
    if (!value.has_value()) {
        return L"unknown";
    }

    return *value ? L"true" : L"false";
}

void RunningIndicatorDiagLogState(PCWSTR stage,
                                  int seq,
                                  FrameworkElement taskListButtonElement,
                                  Shapes::Rectangle runningIndicator,
                                  std::optional<bool> isRunning) {
    try {
        std::wstring name =
            RunningIndicatorDiagAutomationName(taskListButtonElement);
        std::wstring automationId =
            RunningIndicatorDiagAutomationId(taskListButtonElement);

        if (!runningIndicator) {
            RunningIndicatorDiagLog(
                L"stage=%s seq=%d taskAbi=0x%p name=\"%ls\" id=\"%ls\" "
                L"isRunning=%s indicator=<missing>",
                stage, seq, winrt::get_abi(taskListButtonElement), name.c_str(),
                automationId.c_str(),
                RunningIndicatorDiagOptionalBoolText(isRunning));
            return;
        }

        RunningIndicatorDiagLog(
            L"stage=%s seq=%d taskAbi=0x%p indicatorAbi=0x%p name=\"%ls\" "
            L"id=\"%ls\" isRunning=%s actual=%.1fx%.1f prop=%.1fx%.1f "
            L"min=%.1fx%.1f max=%.1fx%.1f opacity=%.3f visibility=%d",
            stage, seq, winrt::get_abi(taskListButtonElement),
            winrt::get_abi(runningIndicator), name.c_str(),
            automationId.c_str(), RunningIndicatorDiagOptionalBoolText(isRunning),
            runningIndicator.ActualWidth(), runningIndicator.ActualHeight(),
            runningIndicator.Width(), runningIndicator.Height(),
            runningIndicator.MinWidth(), runningIndicator.MinHeight(),
            runningIndicator.MaxWidth(), runningIndicator.MaxHeight(),
            runningIndicator.Opacity(), static_cast<int>(runningIndicator.Visibility()));
    } catch (...) {
        RunningIndicatorDiagLog(L"stage=%s seq=%d state-log exception=0x%08X",
                                stage, seq, winrt::to_hresult());
    }
}

using TaskListButton_get_IsRunning_t = HRESULT(WINAPI*)(void* pThis,
                                                        bool* running);
TaskListButton_get_IsRunning_t TaskListButton_get_IsRunning_Original;

std::optional<bool> TryTaskListButtonIsRunning(
    FrameworkElement taskListButtonElement) {
    if (!TaskListButton_get_IsRunning_Original || !taskListButtonElement) {
        RunningIndicatorDiagLog(
            L"get_IsRunning unavailable taskAbi=0x%p hasOriginal=%d",
            winrt::get_abi(taskListButtonElement),
            TaskListButton_get_IsRunning_Original ? 1 : 0);
        return std::nullopt;
    }

    try {
        bool isRunning = false;
        HRESULT hr = TaskListButton_get_IsRunning_Original(
            winrt::get_abi(taskListButtonElement
                               .as<winrt::Windows::Foundation::IUnknown>()),
            &isRunning);
        if (FAILED(hr)) {
            Wh_Log(L"TaskListButton::get_IsRunning failed: 0x%08X", hr);
            RunningIndicatorDiagLog(
                L"get_IsRunning failed taskAbi=0x%p hr=0x%08X",
                winrt::get_abi(taskListButtonElement), hr);
            return std::nullopt;
        }

        RunningIndicatorDiagLog(
            L"get_IsRunning ok taskAbi=0x%p result=%d name=\"%ls\" id=\"%ls\"",
            winrt::get_abi(taskListButtonElement), isRunning ? 1 : 0,
            RunningIndicatorDiagAutomationName(taskListButtonElement).c_str(),
            RunningIndicatorDiagAutomationId(taskListButtonElement).c_str());
        return isRunning;
    } catch (...) {
        Wh_Log(L"TaskListButton::get_IsRunning exception: 0x%08X",
               winrt::to_hresult());
        RunningIndicatorDiagLog(
            L"get_IsRunning exception taskAbi=0x%p hr=0x%08X",
            winrt::get_abi(taskListButtonElement), winrt::to_hresult());
        return std::nullopt;
    }
}

bool ApplyWin10TaskbarButtonVisualsNow(
    FrameworkElement taskListButtonElement,
    std::optional<bool> isRunning = std::nullopt,
    PCWSTR diagCaller = L"unspecified",
    int diagSeq = 0) {
#if EDGE_RESIZE_RUNNING_INDICATOR_EXPERIMENT
    try {
        if (!taskListButtonElement || g_unloading) {
            return false;
        }

#if EDGE_RESIZE_RUNNING_INDICATOR_DIAG_ONLY
        RunningIndicatorDiagLog(
            L"diag-only skip visual apply caller=%s seq=%d taskAbi=0x%p",
            diagCaller, diagSeq, winrt::get_abi(taskListButtonElement));
        return false;
#endif

        FrameworkElement iconPanelElement =
            FindChildByName(taskListButtonElement, L"IconPanel");
        if (!iconPanelElement) {
            return false;
        }

        double buttonWidth = taskListButtonElement.ActualWidth();
        if (!(buttonWidth > 0)) {
            buttonWidth = iconPanelElement.ActualWidth();
        }

        if (!(buttonWidth > 0)) {
            return false;
        }

        auto backgroundBorder =
            FindChildByName(iconPanelElement, L"BackgroundElement")
                .try_as<Controls::Border>();
        if (backgroundBorder) {
            backgroundBorder.HorizontalAlignment(HorizontalAlignment::Stretch);
            backgroundBorder.Margin(Thickness{});
        }

        if (auto runningIndicator = FindChildByName(iconPanelElement,
                                                    L"RunningIndicator")
                                        .try_as<Shapes::Rectangle>()) {
            RunningIndicatorDiagLogState(diagCaller, diagSeq,
                                         taskListButtonElement,
                                         runningIndicator, isRunning);
            bool collapseIndicator = isRunning.has_value() && !*isRunning;
            runningIndicator.Width(
                collapseIndicator ? 0.0
                                  : std::numeric_limits<double>::quiet_NaN());
            runningIndicator.MinWidth(collapseIndicator ? 0.0 : 12.0);
            runningIndicator.MaxWidth(
                collapseIndicator ? 0.0
                                  : std::numeric_limits<double>::infinity());
            runningIndicator.Height(collapseIndicator ? 0.0 : 3.0);
            runningIndicator.MinHeight(collapseIndicator ? 0.0 : 3.0);
            runningIndicator.MaxHeight(collapseIndicator ? 0.0 : 3.0);
            runningIndicator.HorizontalAlignment(HorizontalAlignment::Stretch);
            runningIndicator.VerticalAlignment(VerticalAlignment::Bottom);
            runningIndicator.Margin(collapseIndicator ? Thickness{}
                                                      : Thickness{4, 0, 4, 0});
            runningIndicator.RadiusX(0);
            runningIndicator.RadiusY(0);
            Controls::Grid::SetColumn(runningIndicator, 0);
            Controls::Grid::SetColumnSpan(runningIndicator, 99);
            Controls::Grid::SetRow(runningIndicator, 0);
            Controls::Grid::SetRowSpan(runningIndicator, 99);
            RunningIndicatorDiagLogState(
                collapseIndicator ? L"after-collapse" : L"after-apply",
                diagSeq, taskListButtonElement, runningIndicator, isRunning);

            if (backgroundBorder) {
                backgroundBorder.BorderThickness(Thickness{});
                backgroundBorder.BorderBrush(nullptr);
            }
        } else {
            RunningIndicatorDiagLogState(diagCaller, diagSeq,
                                         taskListButtonElement, nullptr,
                                         isRunning);
            if (backgroundBorder) {
                backgroundBorder.BorderThickness(Thickness{});
                backgroundBorder.BorderBrush(nullptr);
            }
        }

        return true;
    } catch (...) {
        Wh_Log(L"ApplyWin10TaskbarButtonVisualsNow exception: 0x%08X",
               winrt::to_hresult());
        return false;
    }
#else
    (void)taskListButtonElement;
    (void)isRunning;
    (void)diagCaller;
    (void)diagSeq;
    return false;
#endif
}

void ApplyWin10TaskbarButtonVisuals(FrameworkElement taskListButtonElement) {
#if EDGE_RESIZE_RUNNING_INDICATOR_EXPERIMENT
    try {
        if (!taskListButtonElement || g_unloading) {
            return;
        }

        if (ApplyWin10TaskbarButtonVisualsNow(taskListButtonElement)) {
            return;
        }

        int deferredCount = g_taskButtonVisualDeferredCount.fetch_add(1);
        if (deferredCount >= 64) {
            return;
        }

        auto dispatcher = taskListButtonElement.Dispatcher();
        if (!dispatcher) {
            return;
        }

        dispatcher.TryRunAsync(
            winrt::Windows::UI::Core::CoreDispatcherPriority::Low,
            [taskListButtonElement]() {
                ApplyWin10TaskbarButtonVisualsNow(taskListButtonElement);
            });
    } catch (...) {
        Wh_Log(L"ApplyWin10TaskbarButtonVisuals exception: 0x%08X",
               winrt::to_hresult());
    }
#else
    (void)taskListButtonElement;
#endif
}

// {0BD894F2-EDFC-5DDF-A166-2DB14BBFDF35}
constexpr winrt::guid IItemsRepeater{
    0x0BD894F2,
    0xEDFC,
    0x5DDF,
    {0xA1, 0x66, 0x2D, 0xB1, 0x4B, 0xBF, 0xDF, 0x35}};

FrameworkElement ItemsRepeater_TryGetElement(
    FrameworkElement taskbarFrameRepeaterElement,
    int index) {
    winrt::Windows::Foundation::IUnknown pThis = nullptr;
    taskbarFrameRepeaterElement.as(IItemsRepeater, winrt::put_abi(pThis));
    if (!pThis) {
        return nullptr;
    }

    using TryGetElement_t =
        HRESULT(WINAPI*)(void* pThis, int index, void** uiElement);

    void** vtable = *(void***)winrt::get_abi(pThis);
    auto TryGetElement = (TryGetElement_t)vtable[20];

    void* uiElement = nullptr;
    if (FAILED(TryGetElement(winrt::get_abi(pThis), index, &uiElement)) ||
        !uiElement) {
        return nullptr;
    }

    return UIElement{uiElement, winrt::take_ownership_from_abi}
        .try_as<FrameworkElement>();
}

void ApplyWin10TaskbarButtonVisualsFromFrame(
    FrameworkElement taskbarFrameElement) {
#if EDGE_RESIZE_RUNNING_INDICATOR_EXPERIMENT
    try {
        if (!taskbarFrameElement || g_unloading) {
            return;
        }

        auto taskbarFrameRepeaterContainerElement =
            FindChildByName(taskbarFrameElement, L"RootGrid");
        if (!taskbarFrameRepeaterContainerElement) {
            taskbarFrameRepeaterContainerElement =
                FindChildByName(taskbarFrameElement, L"TaskbarFrameBorder");
        }

        if (!taskbarFrameRepeaterContainerElement) {
            return;
        }

        auto taskbarFrameRepeaterElement = FindChildByName(
            taskbarFrameRepeaterContainerElement, L"TaskbarFrameRepeater");
        if (!taskbarFrameRepeaterElement) {
            return;
        }

        for (int i = 0;; i++) {
            auto child =
                ItemsRepeater_TryGetElement(taskbarFrameRepeaterElement, i);
            if (!child) {
                break;
            }

            if (child.Name() == L"TaskListButton") {
                ApplyWin10TaskbarButtonVisualsNow(child);
            }
        }
    } catch (...) {
        Wh_Log(L"ApplyWin10TaskbarButtonVisualsFromFrame exception: 0x%08X",
               winrt::to_hresult());
    }
#else
    (void)taskbarFrameElement;
#endif
}

using TaskbarFrame_OnTaskbarLayoutChildBoundsChanged_t =
    void(WINAPI*)(void* pThis);
TaskbarFrame_OnTaskbarLayoutChildBoundsChanged_t
    TaskbarFrame_OnTaskbarLayoutChildBoundsChanged_Original;
void WINAPI TaskbarFrame_OnTaskbarLayoutChildBoundsChanged_Hook(void* pThis) {
    Wh_Log(L">");

    TaskbarFrame_OnTaskbarLayoutChildBoundsChanged_Original(pThis);

#if EDGE_RESIZE_RUNNING_INDICATOR_EXPERIMENT
    try {
        void* taskbarFrameIUnknownPtr = (void**)pThis + 3;
        winrt::Windows::Foundation::IUnknown taskbarFrameIUnknown;
        winrt::copy_from_abi(taskbarFrameIUnknown, taskbarFrameIUnknownPtr);

        auto taskbarFrameElement = taskbarFrameIUnknown.as<FrameworkElement>();
        auto dispatcher = taskbarFrameElement.Dispatcher();
        int deferredCount = g_taskbarFrameVisualDeferredCount.fetch_add(1);
        if (dispatcher && deferredCount < 128) {
            dispatcher.TryRunAsync(
                winrt::Windows::UI::Core::CoreDispatcherPriority::Low,
                [taskbarFrameElement]() {
                    ApplyWin10TaskbarButtonVisualsFromFrame(
                        taskbarFrameElement);
                });
        } else {
            ApplyWin10TaskbarButtonVisualsFromFrame(taskbarFrameElement);
        }
    } catch (...) {
        Wh_Log(L"TaskbarFrame layout visual hook exception: 0x%08X",
               winrt::to_hresult());
    }
#endif
}

using TaskListButton_UpdateVisualStates_t = void(WINAPI*)(void* pThis);
TaskListButton_UpdateVisualStates_t TaskListButton_UpdateVisualStates_Original;
void WINAPI TaskListButton_UpdateVisualStates_Hook(void* pThis) {
    Wh_Log(L">");

#if EDGE_RESIZE_RUNNING_INDICATOR_EXPERIMENT
    int diagSeq = g_runningIndicatorDiagLogCount.load() + 1;
    RunningIndicatorDiagLog(L"UpdateVisualStates enter seq=%d pThis=0x%p",
                            diagSeq, pThis);
#endif

    if (TaskListButton_UpdateIconColumnDefinition_Original &&
        (g_applyingSettings || g_taskbarButtonWidthCustomized)) {
        LONG mediumTaskbarButtonExtentOffset =
            GetMediumTaskbarButtonExtentOffset();
        if (mediumTaskbarButtonExtentOffset) {
            bool updateButtonPadding = false;

            double* mediumTaskbarButtonExtent =
                (double*)((BYTE*)pThis + mediumTaskbarButtonExtentOffset);
            if (*mediumTaskbarButtonExtent >= 1 &&
                *mediumTaskbarButtonExtent < 10000) {
                double newValue =
                    g_unloading ? 44 : g_settings.taskbarButtonWidth;
                if (newValue != *mediumTaskbarButtonExtent) {
                    Wh_Log(
                        L"Updating MediumTaskbarButtonExtent for "
                        L"TaskListButton: %f->%f",
                        *mediumTaskbarButtonExtent, newValue);
                    *mediumTaskbarButtonExtent = newValue;
                    updateButtonPadding = true;
                }
            }

            double* smallTaskbarButtonExtent =
                g_hasDynamicIconScaling ? mediumTaskbarButtonExtent - 1
                                        : nullptr;
            if (smallTaskbarButtonExtent && *smallTaskbarButtonExtent >= 1 &&
                *smallTaskbarButtonExtent < 10000) {
                double newValue =
                    g_unloading ? 32 : g_settings.taskbarButtonWidthSmall;
                if (newValue != *smallTaskbarButtonExtent) {
                    Wh_Log(
                        L"Updating SmallTaskbarButtonExtent for "
                        L"TaskListButton: %f->%f",
                        *smallTaskbarButtonExtent, newValue);
                    *smallTaskbarButtonExtent = newValue;
                    updateButtonPadding = true;
                }
            }

            if (updateButtonPadding) {
                g_taskbarButtonWidthCustomized = true;
                TaskListButton_UpdateButtonPadding_Hook(pThis);
            }
        } else {
            Wh_Log(L"Error: mediumTaskbarButtonExtentOffset is invalid");
        }
    }

    TaskListButton_UpdateVisualStates_Original(pThis);

#if EDGE_RESIZE_RUNNING_INDICATOR_EXPERIMENT
    RunningIndicatorDiagLog(L"UpdateVisualStates after-original seq=%d pThis=0x%p",
                            diagSeq, pThis);
#endif

    try {
        void* taskListButtonIUnknownPtr = (void**)pThis + 3;
        winrt::Windows::Foundation::IUnknown taskListButtonIUnknown;
        winrt::copy_from_abi(taskListButtonIUnknown,
                             taskListButtonIUnknownPtr);
        auto taskListButtonElement =
            taskListButtonIUnknown.as<FrameworkElement>();

#if EDGE_RESIZE_RUNNING_INDICATOR_EXPERIMENT
        RunningIndicatorDiagLog(
            L"UpdateVisualStates element seq=%d pThis=0x%p taskAbi=0x%p "
            L"name=\"%ls\" id=\"%ls\"",
            diagSeq, pThis, winrt::get_abi(taskListButtonElement),
            RunningIndicatorDiagAutomationName(taskListButtonElement).c_str(),
            RunningIndicatorDiagAutomationId(taskListButtonElement).c_str());
        ApplyWin10TaskbarButtonVisualsNow(
            taskListButtonElement,
            TryTaskListButtonIsRunning(taskListButtonElement),
            L"UpdateVisualStates", diagSeq);
#endif

        if (g_applyingSettings && !g_hasDynamicIconScaling) {
            if (taskListButtonElement) {
                if (auto iconPanelElement =
                        FindChildByName(taskListButtonElement, L"IconPanel")) {
                    if (auto iconElement =
                            FindChildByName(iconPanelElement, L"Icon")) {
                        double iconSize =
                            g_unloading ? 24 : g_settings.iconSize;
                        iconElement.Width(iconSize);
                        iconElement.Height(iconSize);
                    }
                }
            }
        }
    } catch (...) {
        Wh_Log(L"TaskListButton_UpdateVisualStates post-update exception: "
               L"0x%08X",
               winrt::to_hresult());
    }
}

using LaunchListItemViewModel_IconHeight_t = void(WINAPI*)(void* pThis,
                                                           double iconHeight);
LaunchListItemViewModel_IconHeight_t
    LaunchListItemViewModel_IconHeight_Original;
void WINAPI LaunchListItemViewModel_IconHeight_Hook(void* pThis,
                                                    double iconHeight) {
    Wh_Log(L"> iconHeight=%f", iconHeight);

    g_smallIconSize = iconHeight == g_settings.iconSizeSmall &&
                      iconHeight != g_settings.iconSize;

    LaunchListItemViewModel_IconHeight_Original(pThis, iconHeight);
}

using ExperienceToggleButton_UpdateButtonPadding_t = void(WINAPI*)(void* pThis);
ExperienceToggleButton_UpdateButtonPadding_t
    ExperienceToggleButton_UpdateButtonPadding_Original;

void WINAPI ExperienceToggleButton_UpdateButtonPadding_Hook(void* pThis) {
    Wh_Log(L">");

    ExperienceToggleButton_UpdateButtonPadding_Original(pThis);

    if (g_hasDynamicIconScaling && g_unloading) {
        return;
    }

    FrameworkElement toggleButtonElement = nullptr;
    ((IUnknown**)pThis)[1]->QueryInterface(winrt::guid_of<FrameworkElement>(),
                                           winrt::put_abi(toggleButtonElement));
    if (!toggleButtonElement) {
        return;
    }

    auto panelElement =
        FindChildByName(toggleButtonElement, L"ExperienceToggleButtonRootPanel")
            .try_as<Controls::Grid>();
    if (!panelElement) {
        return;
    }

    double defaultWidthExtra = -4;

    auto className = winrt::get_class_name(toggleButtonElement);
    if (className == L"Taskbar.ExperienceToggleButton") {
        auto automationId = Automation::AutomationProperties::GetAutomationId(
            toggleButtonElement);
        if (automationId == L"StartButton") {
            defaultWidthExtra = -3;
        }
    } else if (className == L"Taskbar.SearchBoxButton") {
        // Only if search icon and not a search box.
        if (panelElement.Margin() != Thickness{}) {
            return;
        }
    } else {
        return;
    }

    double buttonWidth = panelElement.Width();
    if (!(buttonWidth > 0)) {
        return;
    }

    // For the start button, the padding is different depending on the alignment
    // of the taskbar (left/center).
    auto buttonPadding = panelElement.Padding();

    double defaultWidth = g_smallIconSize ? 32 : 44;
    double overrideWidth =
        g_unloading ? defaultWidth
                    : (g_smallIconSize ? g_settings.taskbarButtonWidthSmall
                                       : g_settings.taskbarButtonWidth);

    double newWidth = overrideWidth + buttonPadding.Left + buttonPadding.Right +
                      defaultWidthExtra;
    if (newWidth != buttonWidth) {
        Wh_Log(L"Updating MediumTaskbarButtonExtent for %s: %f->%f",
               className.c_str(), buttonWidth, newWidth);
        panelElement.Width(newWidth);
    }

}

using SearchButtonBase_UpdateButtonPadding_t = void(WINAPI*)(void* pThis);
SearchButtonBase_UpdateButtonPadding_t
    SearchButtonBase_UpdateButtonPadding_Original;
void WINAPI SearchButtonBase_UpdateButtonPadding_Hook(void* pThis) {
    Wh_Log(L">");

    SearchButtonBase_UpdateButtonPadding_Original(pThis);

    if (g_hasDynamicIconScaling && g_unloading) {
        return;
    }

    FrameworkElement toggleButtonElement = nullptr;
    ((IUnknown**)pThis)[1]->QueryInterface(winrt::guid_of<FrameworkElement>(),
                                           winrt::put_abi(toggleButtonElement));
    if (!toggleButtonElement) {
        return;
    }

    auto panelElement =
        FindChildByName(toggleButtonElement, L"SearchBoxButtonRootPanel")
            .try_as<Controls::Grid>();
    if (!panelElement) {
        return;
    }

    // Only if search icon and not a search box.
    if (FindChildByName(panelElement, L"SearchBoxTextBlock")) {
        return;
    }

    double buttonWidth = panelElement.Width();
    if (!(buttonWidth > 0)) {
        return;
    }

    auto buttonPadding = panelElement.Padding();

    double defaultWidth = g_smallIconSize ? 32 : 44;
    double overrideWidth =
        g_unloading ? defaultWidth
                    : (g_smallIconSize ? g_settings.taskbarButtonWidthSmall
                                       : g_settings.taskbarButtonWidth);

    double newWidth =
        overrideWidth + buttonPadding.Left + buttonPadding.Right - 4;
    if (newWidth != buttonWidth) {
        Wh_Log(L"Updating MediumTaskbarButtonExtent: %f->%f", buttonWidth,
               newWidth);
        panelElement.Width(newWidth);
    }

}

using AugmentedEntryPointButton_UpdateButtonPadding_t =
    void(WINAPI*)(void* pThis);
AugmentedEntryPointButton_UpdateButtonPadding_t
    AugmentedEntryPointButton_UpdateButtonPadding_Original;
void WINAPI AugmentedEntryPointButton_UpdateButtonPadding_Hook(void* pThis) {
    Wh_Log(L">");

    g_inAugmentedEntryPointButton_UpdateButtonPadding = true;

    AugmentedEntryPointButton_UpdateButtonPadding_Original(pThis);

    g_inAugmentedEntryPointButton_UpdateButtonPadding = false;
}

using RepeatButton_Width_t = void(WINAPI*)(void* pThis, double width);
RepeatButton_Width_t RepeatButton_Width_Original;
void WINAPI RepeatButton_Width_Hook(void* pThis, double width) {
    Wh_Log(L"> width=%f", width);

    RepeatButton_Width_Original(pThis, width);

    if (!g_inAugmentedEntryPointButton_UpdateButtonPadding) {
        return;
    }

    FrameworkElement button = nullptr;
    (*(IUnknown**)pThis)
        ->QueryInterface(winrt::guid_of<FrameworkElement>(),
                         winrt::put_abi(button));
    if (!button) {
        return;
    }

    FrameworkElement augmentedEntryPointContentGrid =
        FindChildByName(button, L"AugmentedEntryPointContentGrid");
    if (!augmentedEntryPointContentGrid) {
        return;
    }

    double marginValue = static_cast<double>(40 - g_settings.iconSize) / 2;
    if (marginValue < 0) {
        marginValue = 0;
    }

    EnumChildElements(augmentedEntryPointContentGrid, [marginValue](
                                                          FrameworkElement
                                                              child) {
        if (winrt::get_class_name(child) != L"Windows.UI.Xaml.Controls.Grid") {
            return false;
        }

        FrameworkElement panelGrid =
            FindChildByClassName(child, L"Windows.UI.Xaml.Controls.Grid");
        if (!panelGrid) {
            return false;
        }

        FrameworkElement panel = FindChildByClassName(
            panelGrid, L"AdaptiveCards.Rendering.Uwp.WholeItemsPanel");
        if (!panel) {
            return false;
        }

        Wh_Log(L"Processing %f x %f widget", panelGrid.Width(),
               panelGrid.Height());

        double labelsTopBorderExtraMargin = 0;

        bool widePanel = panelGrid.Width() > panelGrid.Height();
        if (widePanel) {
            auto margin = Thickness{3, 3, 3, 3};

            if (!g_unloading && marginValue <= 3) {
                labelsTopBorderExtraMargin = 3 - marginValue;
                margin.Left = marginValue;
                margin.Top = marginValue;

                // Logically these should be marginValue too, but having no
                // right/bottom margin doesn't seem to matter, while having
                // values which are too tight sometimes cause the icon to
                // disappear for some reason. Relevant issue:
                // https://github.com/ramensoftware/windhawk-mods/issues/726
                margin.Right = 0;
                margin.Bottom = 0;
            }

            Wh_Log(L"Setting Margin=%f,%f,%f,%f for panel", margin.Left,
                   margin.Top, margin.Right, margin.Bottom);

            panel.Margin(margin);

            panelGrid.VerticalAlignment(g_unloading
                                            ? VerticalAlignment::Stretch
                                            : VerticalAlignment::Center);
        } else {
            auto margin = Thickness{8, 8, 8, 8};

            if (!g_unloading) {
                margin.Left = marginValue;
                margin.Top = marginValue;

                // Logically these should be marginValue too, but having no
                // right/bottom margin doesn't seem to matter, while having
                // values which are too tight sometimes cause the icon to
                // disappear for some reason. Relevant issue:
                // https://github.com/ramensoftware/windhawk-mods/issues/726
                margin.Right = 0;
                margin.Bottom = 0;

                if (g_taskbarHeight < 48) {
                    margin.Top -= static_cast<double>(48 - g_taskbarHeight) / 2;
                    if (margin.Top < 0) {
                        margin.Top = 0;
                    }
                }
            }

            Wh_Log(L"Setting Margin=%f,%f,%f,%f for panel", margin.Left,
                   margin.Top, margin.Right, margin.Bottom);

            panel.Margin(margin);
        }

        FrameworkElement tickerGrid = panel;
        if ((tickerGrid = FindChildByClassName(
                 tickerGrid, L"Windows.UI.Xaml.Controls.Border")) &&
            (tickerGrid = FindChildByClassName(
                 tickerGrid, L"AdaptiveCards.Rendering.Uwp.WholeItemsPanel")) &&
            (tickerGrid = FindChildByClassName(
                 tickerGrid, L"Windows.UI.Xaml.Controls.Grid"))) {
            // OK.
        } else {
            return false;
        }

        double badgeMaxValue = g_unloading ? 24 : 40 - marginValue * 2;

        FrameworkElement badgeSmall = tickerGrid;
        if ((badgeSmall = FindChildByName(badgeSmall, L"SmallTicker1")) &&
            (badgeSmall = FindChildByClassName(
                 badgeSmall, L"AdaptiveCards.Rendering.Uwp.WholeItemsPanel")) &&
            (badgeSmall =
                 FindChildByName(badgeSmall, L"BadgeAnchorSmallTicker"))) {
            Wh_Log(L"Setting MaxWidth=%f, MaxHeight=%f for small badge",
                   badgeMaxValue, badgeMaxValue);

            badgeSmall.MaxWidth(badgeMaxValue);
            badgeSmall.MaxHeight(badgeMaxValue);
        }

        FrameworkElement badgeLarge = tickerGrid;
        if ((badgeLarge = FindChildByName(badgeLarge, L"LargeTicker1")) &&
            (badgeLarge = FindChildByClassName(
                 badgeLarge, L"AdaptiveCards.Rendering.Uwp.WholeItemsPanel")) &&
            (badgeLarge =
                 FindChildByName(badgeLarge, L"BadgeAnchorLargeTicker"))) {
            Wh_Log(L"Setting MaxWidth=%f, MaxHeight=%f for large badge",
                   badgeMaxValue, badgeMaxValue);

            badgeLarge.MaxWidth(badgeMaxValue);
            badgeLarge.MaxHeight(badgeMaxValue);
        }

        FrameworkElement labelsBorder = tickerGrid;
        if ((labelsBorder = FindChildByName(labelsBorder, L"LargeTicker2"))) {
            auto margin = Thickness{0, labelsTopBorderExtraMargin, 0, 0};

            Wh_Log(L"Setting Margin=%f,%f,%f,%f for labels border", margin.Left,
                   margin.Top, margin.Right, margin.Bottom);

            labelsBorder.Margin(margin);
        }

        return false;
    });
}

using SHAppBarMessage_t = decltype(&SHAppBarMessage);
SHAppBarMessage_t SHAppBarMessage_Original;
void RunningIndicatorDiagEnsureTaskbarViewHooked(PCWSTR reason);
auto WINAPI SHAppBarMessage_Hook(DWORD dwMessage, PAPPBARDATA pData) {
    auto ret = SHAppBarMessage_Original(dwMessage, pData);
    RunningIndicatorDiagEnsureTaskbarViewHooked(L"SHAppBarMessage");

    // This is used to position secondary taskbars.
    if (dwMessage == ABM_QUERYPOS && ret && !IsVerticalTaskbar() &&
        g_taskbarHeight) {
        Wh_Log(L">");
        pData->rc.top =
            pData->rc.bottom -
            MulDiv(g_taskbarHeight, GetDpiForWindow(pData->hWnd), 96);
    }

    return ret;
}

using SendMessageTimeoutW_t = decltype(&SendMessageTimeoutW);
SendMessageTimeoutW_t SendMessageTimeoutW_Original;
LRESULT WINAPI SendMessageTimeoutW_Hook(HWND hWnd,
                                        UINT Msg,
                                        WPARAM wParam,
                                        LPARAM lParam,
                                        UINT fuFlags,
                                        UINT uTimeout,
                                        PDWORD_PTR lpdwResult) {
    if (g_shellIconLoaderV2_LoadAsyncIcon__ResumeCoro_ThreadId ==
            GetCurrentThreadId() &&
        !g_unloading && Msg == WM_GETICON && wParam == ICON_BIG &&
        (g_smallIconSize ? g_settings.iconSizeSmall : g_settings.iconSize) <=
            16) {
        Wh_Log(L">");
        wParam = ICON_SMALL2;
    }

    LRESULT ret = SendMessageTimeoutW_Original(hWnd, Msg, wParam, lParam,
                                               fuFlags, uTimeout, lpdwResult);

    RunningIndicatorDiagEnsureTaskbarViewHooked(L"SendMessageTimeoutW");

    return ret;
}

#ifdef WIN11_TASKBAR_WIN10_MULTIROW_COMBINED
int EdgeResizeGetNotificationIconRowsSetting() {
    int persistedRows = Wh_GetIntValue(L"EdgeResizeNotificationIconRows", 0);
    if (persistedRows >= 1 && persistedRows <= 10) {
        return persistedRows;
    }

    return 1;
}
#endif

void LoadSettings() {
#ifdef WIN11_TASKBAR_WIN10_MULTIROW_COMBINED
    g_settings.taskbarHeight = 48;
    g_settings.iconSize = 24;
    g_settings.taskbarButtonWidth = 44;
    g_settings.iconSizeSmall = 16;
    g_settings.taskbarButtonWidthSmall = 32;
    g_settings.edgeResizeEnabled = true;
    g_settings.edgeResizeGripPixels = 8;
    g_settings.edgeResizeRowHeight = 48;
    g_settings.edgeResizeMinRows = 1;
    g_settings.edgeResizeMaxRows = 10;
#else
    g_settings.taskbarHeight = Wh_GetIntSetting(L"TaskbarHeight");
    g_settings.iconSize = Wh_GetIntSetting(L"IconSize");
    g_settings.taskbarButtonWidth = Wh_GetIntSetting(L"TaskbarButtonWidth");
    g_settings.iconSizeSmall = Wh_GetIntSetting(L"IconSizeSmall");
    g_settings.taskbarButtonWidthSmall =
        Wh_GetIntSetting(L"TaskbarButtonWidthSmall");
    g_settings.edgeResizeEnabled =
        Wh_GetIntSetting(L"EdgeResizeEnabled") != 0;
    g_settings.edgeResizeGripPixels =
        Wh_GetIntSetting(L"EdgeResizeGripPixels");
    g_settings.edgeResizeRowHeight = Wh_GetIntSetting(L"EdgeResizeRowHeight");
    g_settings.edgeResizeMinRows = Wh_GetIntSetting(L"EdgeResizeMinRows");
    g_settings.edgeResizeMaxRows = Wh_GetIntSetting(L"EdgeResizeMaxRows");
#endif

    if (g_settings.edgeResizeGripPixels < 2) {
        g_settings.edgeResizeGripPixels = 8;
    }
    if (g_settings.edgeResizeRowHeight < 24) {
        g_settings.edgeResizeRowHeight = 48;
    }
    if (g_settings.edgeResizeMinRows < 1) {
        g_settings.edgeResizeMinRows = 1;
    }
    if (g_settings.edgeResizeMaxRows < g_settings.edgeResizeMinRows) {
        g_settings.edgeResizeMaxRows = g_settings.edgeResizeMinRows;
    }

    int draggedHeight = Wh_GetIntValue(L"EdgeResizeTaskbarHeight", 0);
    if (draggedHeight >= g_settings.edgeResizeRowHeight *
                             g_settings.edgeResizeMinRows &&
        draggedHeight <= g_settings.edgeResizeRowHeight *
                             g_settings.edgeResizeMaxRows) {
#ifdef WIN11_TASKBAR_WIN10_MULTIROW_COMBINED
        auto rowsFromHeight = [](int height) {
            int rows =
                (height + g_settings.edgeResizeRowHeight / 2) /
                g_settings.edgeResizeRowHeight;
            if (rows < g_settings.edgeResizeMinRows) {
                rows = g_settings.edgeResizeMinRows;
            } else if (rows > g_settings.edgeResizeMaxRows) {
                rows = g_settings.edgeResizeMaxRows;
            }
            return rows;
        };

        int settingsRows = rowsFromHeight(g_settings.taskbarHeight);
        int draggedRows = rowsFromHeight(draggedHeight);
        if (draggedRows != settingsRows) {
            Wh_Log(L"Using edge-resize storage height=%d rows=%d over "
                   L"settings height=%d rows=%d",
                   draggedHeight, draggedRows, g_settings.taskbarHeight,
                   settingsRows);
        }

        g_settings.taskbarHeight = draggedHeight;
#else
        g_settings.taskbarHeight = draggedHeight;
#endif
    }
}

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

bool ProtectAndMemcpy(DWORD protect, void* dst, const void* src, size_t size) {
    DWORD oldProtect;
    if (!VirtualProtect(dst, size, protect, &oldProtect)) {
        return false;
    }

    memcpy(dst, src, size);
    VirtualProtect(dst, size, oldProtect, &oldProtect);
    return true;
}

void EdgeResizeUpdateGripWindowPosition();

void EdgeResizeWaitForMeasureOverrideWithGripUpdates() {
    constexpr int kWaitStepMs = 16;
    constexpr int kMaxWaitMs = 10000;
    for (int elapsedMs = 0; elapsedMs < kMaxWaitMs;
         elapsedMs += kWaitStepMs) {
        if (!g_pendingMeasureOverride) {
            break;
        }

        EdgeResizeUpdateGripWindowPosition();
        Sleep(kWaitStepMs);
    }
}

void ApplySettings(int taskbarHeight, bool allowRefreshNudge = true) {
    if (taskbarHeight < 2) {
        taskbarHeight = 2;
    }

    HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();
    if (!hTaskbarWnd) {
        Wh_Log(L"No taskbar found");
        g_taskbarHeight = taskbarHeight;
        return;
    }

    Wh_Log(L"Applying settings for taskbar %08X",
           (DWORD)(DWORD_PTR)hTaskbarWnd);

    if (!g_taskbarHeight) {
        RECT taskbarRect{};
        GetWindowRect(hTaskbarWnd, &taskbarRect);
        g_taskbarHeight = MulDiv(taskbarRect.bottom - taskbarRect.top, 96,
                                 GetDpiForWindow(hTaskbarWnd));
    }

    g_applyingSettings = true;

    bool sameLogicalHeight = taskbarHeight == g_taskbarHeight;
    bool verticalTaskbar = IsVerticalTaskbar();
    if (!allowRefreshNudge && sameLogicalHeight && !verticalTaskbar) {
        EdgeResizeLog(L"skip same-height refresh nudge height=%d",
                      taskbarHeight);
    }

    if (allowRefreshNudge && !verticalTaskbar && sameLogicalHeight) {
        g_pendingMeasureOverride = true;

        // Temporarily change the height to force a UI refresh.
        g_taskbarHeight = taskbarHeight - 1;
        if (!TaskbarConfiguration_GetFrameSize_Original &&
            double_48_value_Original) {
            double tempTaskbarHeight = g_taskbarHeight;
            ProtectAndMemcpy(PAGE_READWRITE, double_48_value_Original,
                             &tempTaskbarHeight, sizeof(double));
        }

        // Trigger TrayUI::_HandleSettingChange.
        SendMessage(hTaskbarWnd, WM_SETTINGCHANGE, SPI_SETLOGICALDPIOVERRIDE,
                    0);

        // Wait for the change to apply.
        EdgeResizeWaitForMeasureOverrideWithGripUpdates();
    }

    g_pendingMeasureOverride = true;

    g_taskbarHeight = taskbarHeight;
    if (!TaskbarConfiguration_GetFrameSize_Original &&
        double_48_value_Original) {
        double tempTaskbarHeight = g_taskbarHeight;
        ProtectAndMemcpy(PAGE_READWRITE, double_48_value_Original,
                         &tempTaskbarHeight, sizeof(double));
    }

    // Trigger TrayUI::_HandleSettingChange.
    SendMessage(hTaskbarWnd, WM_SETTINGCHANGE, SPI_SETLOGICALDPIOVERRIDE, 0);

    if (!verticalTaskbar) {
        // Wait for the change to apply.
        EdgeResizeWaitForMeasureOverrideWithGripUpdates();
    } else {
        g_pendingMeasureOverride = false;
    }

    HWND hReBarWindow32 =
        FindWindowEx(hTaskbarWnd, nullptr, L"ReBarWindow32", nullptr);
    if (hReBarWindow32) {
        HWND hMSTaskSwWClass =
            FindWindowEx(hReBarWindow32, nullptr, L"MSTaskSwWClass", nullptr);
        if (hMSTaskSwWClass) {
            // Trigger CTaskBand::_HandleSyncDisplayChange.
            SendMessage(hMSTaskSwWClass, 0x452, 3, 0);
        }
    }

    g_applyingSettings = false;
}

HWND g_edgeResizeTaskbarWnd = nullptr;
HWND g_edgeResizeCoreWindow = nullptr;
HWND g_edgeResizeInputSiteWnd = nullptr;
std::vector<HWND> g_edgeResizeChildWindows;
bool g_edgeResizeDragging;
int g_edgeResizeDragStartY;
int g_edgeResizeDragStartHeight;
int g_edgeResizeLastLiveHeight;
std::atomic<int> g_edgeResizeRequestedHeight;
std::atomic<bool> g_edgeResizeRequestedPersist;
std::atomic<bool> g_edgeResizeApplyRunning;
std::atomic<bool> g_edgeResizeDelayedInstallRunning;
std::atomic<int> g_edgeResizeGripPredictedHeight;
std::atomic<bool> g_edgeResizeMouseHookThreadRunning;
HHOOK g_edgeResizeMouseHook = nullptr;
DWORD g_edgeResizeMouseHookThreadId = 0;
UINT_PTR g_edgeResizeCursorTimer = 0;
UINT g_edgeResizeTaskbarCreatedMessage;
HWND g_edgeResizeGripWnd = nullptr;
DWORD g_edgeResizeGripThreadId = 0;
std::atomic<bool> g_edgeResizeGripThreadRunning;
HWND g_edgeResizeGrowBackfillWnd = nullptr;
HBRUSH g_edgeResizeGrowBackfillBrush = nullptr;
bool g_edgeResizeGrowBackfillClassRegistered;
HBITMAP g_edgeResizeGrowBackfillBitmap = nullptr;
int g_edgeResizeGrowBackfillBitmapWidth = 0;
int g_edgeResizeGrowBackfillBitmapHeight = 0;

bool EdgeResizeGetDiagLogDirPath(WCHAR* logDir, DWORD logDirCch) {
    if (!logDir || logDirCch == 0) {
        return false;
    }

    logDir[0] = L'\0';

    WCHAR baseDir[MAX_PATH]{};
    DWORD baseDirLength =
        GetEnvironmentVariableW(L"LOCALAPPDATA", baseDir, ARRAYSIZE(baseDir));
    if (baseDirLength == 0 || baseDirLength >= ARRAYSIZE(baseDir)) {
        baseDirLength =
            GetEnvironmentVariableW(L"USERPROFILE", baseDir, ARRAYSIZE(baseDir));
        if (baseDirLength > 0 && baseDirLength < ARRAYSIZE(baseDir)) {
            wcscat_s(baseDir, ARRAYSIZE(baseDir), L"\\AppData\\Local");
        } else {
            wcscpy_s(baseDir, ARRAYSIZE(baseDir), L"C:\\Temp");
        }
    }

    WCHAR rootDir[MAX_PATH]{};
    _snwprintf_s(rootDir, _TRUNCATE,
                 L"%s\\DraggableMultirowTaskbarSystray", baseDir);
    CreateDirectoryW(rootDir, nullptr);

    _snwprintf_s(logDir, logDirCch, _TRUNCATE, L"%s\\Logs", rootDir);
    CreateDirectoryW(logDir, nullptr);
    return logDir[0] != L'\0';
}

std::wstring EdgeResizeGetDiagLogDir() {
    WCHAR logDir[MAX_PATH]{};
    if (EdgeResizeGetDiagLogDirPath(logDir, ARRAYSIZE(logDir))) {
        return logDir;
    }

    return L"C:\\Temp";
}

void EdgeResizeLog(PCWSTR format, ...) {
#if EDGE_RESIZE_FILE_DIAG || EDGE_RESIZE_PROCESS_DIAG
    std::wstring logDir = EdgeResizeGetDiagLogDir();

    SYSTEMTIME st{};
    GetLocalTime(&st);

    wchar_t message[768]{};
    va_list args;
    va_start(args, format);
    _vsnwprintf_s(message, _TRUNCATE, format, args);
    va_end(args);

    wchar_t line[1024]{};
    _snwprintf_s(line, _TRUNCATE,
                 L"%04u-%02u-%02u %02u:%02u:%02u.%03u pid=%lu tid=%lu %s\r\n",
                 st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute,
                 st.wSecond, st.wMilliseconds, GetCurrentProcessId(),
                 GetCurrentThreadId(), message);

    auto writeLog = [&](const std::wstring& logPath) {
        HANDLE file = CreateFileW(
            logPath.c_str(), FILE_APPEND_DATA,
            FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, nullptr,
            OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
        if (file == INVALID_HANDLE_VALUE) {
            return;
        }

        DWORD bytesWritten = 0;
        WriteFile(file, line, static_cast<DWORD>(wcslen(line) * sizeof(wchar_t)),
                  &bytesWritten, nullptr);
        CloseHandle(file);
    };

    writeLog(logDir +
             L"\\draggable-multirow-taskbar-systray-windows-11.log");

#if EDGE_RESIZE_PROCESS_DIAG
    wchar_t pidLogName[96]{};
    _snwprintf_s(pidLogName, _TRUNCATE,
                 L"\\draggable-multirow-taskbar-systray-windows-11-pid-%lu.log",
                 GetCurrentProcessId());
    writeLog(logDir + pidLogName);
#endif
#else
    UNREFERENCED_PARAMETER(format);
#endif
}

constexpr PCWSTR kEdgeResizeGrowBackfillClass =
    L"DraggableMultirowTaskbarGrowBackfillWindow";

LRESULT CALLBACK EdgeResizeGrowBackfillWndProc(HWND hWnd,
                                               UINT uMsg,
                                               WPARAM wParam,
                                               LPARAM lParam) {
    switch (uMsg) {
        case WM_ERASEBKGND:
            return g_edgeResizeGrowBackfillBitmap ? 1 : 0;

        case WM_PAINT: {
            PAINTSTRUCT ps{};
            HDC hdc = BeginPaint(hWnd, &ps);
            if (hdc && g_edgeResizeGrowBackfillBitmap) {
                HDC memDc = CreateCompatibleDC(hdc);
                if (memDc) {
                    HGDIOBJ oldBitmap =
                        SelectObject(memDc, g_edgeResizeGrowBackfillBitmap);
                    BitBlt(hdc, 0, 0, g_edgeResizeGrowBackfillBitmapWidth,
                           g_edgeResizeGrowBackfillBitmapHeight, memDc, 0, 0,
                           SRCCOPY);
                    SelectObject(memDc, oldBitmap);
                    DeleteDC(memDc);
                }
            } else if (hdc && g_edgeResizeGrowBackfillBrush) {
                FillRect(hdc, &ps.rcPaint, g_edgeResizeGrowBackfillBrush);
            }
            EndPaint(hWnd, &ps);
            return 0;
        }
    }

    return DefWindowProcW(hWnd, uMsg, wParam, lParam);
}

bool EdgeResizeEnsureGrowBackfillClass() {
    if (g_edgeResizeGrowBackfillClassRegistered) {
        return true;
    }

    if (!g_edgeResizeGrowBackfillBrush) {
        g_edgeResizeGrowBackfillBrush = CreateSolidBrush(RGB(32, 32, 32));
    }

    WNDCLASSW wc{};
    wc.lpfnWndProc = EdgeResizeGrowBackfillWndProc;
    wc.hInstance = GetModuleHandleW(nullptr);
    wc.hbrBackground = g_edgeResizeGrowBackfillBrush;
    wc.lpszClassName = kEdgeResizeGrowBackfillClass;

    if (!RegisterClassW(&wc)) {
        DWORD error = GetLastError();
        if (error != ERROR_CLASS_ALREADY_EXISTS) {
            EdgeResizeLog(L"grow backfill register failed error=%lu", error);
            return false;
        }
    }

    g_edgeResizeGrowBackfillClassRegistered = true;
    return true;
}

void EdgeResizeClearGrowBackfillBitmap() {
    if (g_edgeResizeGrowBackfillBitmap) {
        DeleteObject(g_edgeResizeGrowBackfillBitmap);
        g_edgeResizeGrowBackfillBitmap = nullptr;
    }

    g_edgeResizeGrowBackfillBitmapWidth = 0;
    g_edgeResizeGrowBackfillBitmapHeight = 0;
}

bool EdgeResizeCaptureGrowBackfillBitmap(int x,
                                         int y,
                                         int width,
                                         int height) {
    EdgeResizeClearGrowBackfillBitmap();
    if (width < 1 || height < 1) {
        return false;
    }

    HDC screenDc = GetDC(nullptr);
    if (!screenDc) {
        return false;
    }

    HDC memDc = CreateCompatibleDC(screenDc);
    HBITMAP bitmap = nullptr;
    if (memDc) {
        bitmap = CreateCompatibleBitmap(screenDc, width, height);
    }

    bool captured = false;
    if (memDc && bitmap) {
        HGDIOBJ oldBitmap = SelectObject(memDc, bitmap);
        captured = BitBlt(memDc, 0, 0, width, height, screenDc, x, y,
                          SRCCOPY | CAPTUREBLT) != FALSE;
        SelectObject(memDc, oldBitmap);
    }

    if (memDc) {
        DeleteDC(memDc);
    }
    ReleaseDC(nullptr, screenDc);

    if (!captured) {
        if (bitmap) {
            DeleteObject(bitmap);
        }
        return false;
    }

    g_edgeResizeGrowBackfillBitmap = bitmap;
    g_edgeResizeGrowBackfillBitmapWidth = width;
    g_edgeResizeGrowBackfillBitmapHeight = height;
    return true;
}

void EdgeResizeDestroyGrowBackfillWindow(PCWSTR reason) {
    HWND hwnd = g_edgeResizeGrowBackfillWnd;
    if (hwnd) {
        g_edgeResizeGrowBackfillWnd = nullptr;
        DestroyWindow(hwnd);
        EdgeResizeLog(L"grow backfill destroyed reason=%s hwnd=%08X",
                      reason ? reason : L"<unknown>",
                      (DWORD)(DWORD_PTR)hwnd);
    }
    EdgeResizeClearGrowBackfillBitmap();
}

bool EdgeResizeShowGrowBackfill(HWND hTaskbarWnd,
                                int currentHeight,
                                int targetHeight,
                                PCWSTR phase) {
    if (!hTaskbarWnd || g_unloading || IsVerticalTaskbar()) {
        return false;
    }

    if (!EdgeResizeEnsureGrowBackfillClass()) {
        return false;
    }

    RECT taskbarRect{};
    if (!GetWindowRect(hTaskbarWnd, &taskbarRect)) {
        EdgeResizeLog(L"grow backfill no taskbar rect phase=%s",
                      phase ? phase : L"<unknown>");
        return false;
    }

    int dpi = GetDpiForWindow(hTaskbarWnd);
    if (!dpi) {
        dpi = 96;
    }

    int backfillLogicalHeight = targetHeight;
    if (backfillLogicalHeight < g_settings.edgeResizeRowHeight) {
        backfillLogicalHeight = g_settings.edgeResizeRowHeight;
    }

    int backfillPhysicalHeight = MulDiv(backfillLogicalHeight, dpi, 96);
    int taskbarPhysicalHeight = taskbarRect.bottom - taskbarRect.top;
    if (backfillPhysicalHeight < 1) {
        backfillPhysicalHeight = 1;
    } else if (taskbarPhysicalHeight > 0 &&
               backfillPhysicalHeight < taskbarPhysicalHeight) {
        backfillPhysicalHeight = taskbarPhysicalHeight;
    }

    HWND hwnd = g_edgeResizeGrowBackfillWnd;
    int overlayX = taskbarRect.left;
    int overlayY = taskbarRect.bottom - backfillPhysicalHeight;
    int overlayWidth = taskbarRect.right - taskbarRect.left;
    int overlayHeight = backfillPhysicalHeight;
    bool captured = EdgeResizeCaptureGrowBackfillBitmap(
        overlayX, overlayY, overlayWidth, overlayHeight);

    if (!hwnd || !IsWindow(hwnd)) {
        hwnd = CreateWindowExW(WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE |
                                   WS_EX_TRANSPARENT,
                               kEdgeResizeGrowBackfillClass, L"",
                               WS_POPUP | WS_VISIBLE,
                               overlayX, overlayY, overlayWidth,
                               overlayHeight, nullptr, nullptr,
                               GetModuleHandleW(nullptr), nullptr);
        if (!hwnd) {
            EdgeResizeLog(L"grow backfill create failed error=%lu",
                          GetLastError());
            EdgeResizeClearGrowBackfillBitmap();
            return false;
        }
        g_edgeResizeGrowBackfillWnd = hwnd;
    }

    SetWindowPos(hwnd, HWND_TOPMOST, overlayX, overlayY, overlayWidth,
                 overlayHeight,
                 SWP_NOACTIVATE | SWP_SHOWWINDOW | SWP_ASYNCWINDOWPOS |
                     SWP_NOSENDCHANGING);
    RedrawWindow(hwnd, nullptr, nullptr,
                  RDW_INVALIDATE | RDW_ERASE | RDW_UPDATENOW);

    EdgeResizeLog(
        L"grow backfill show phase=%s hwnd=%08X taskbar=%08X rect=%d,%d,%d,%d "
        L"current=%d target=%d backfillLogical=%d backfillPhysical=%d "
        L"snapshot=%d",
        phase ? phase : L"<unknown>", (DWORD)(DWORD_PTR)hwnd,
        (DWORD)(DWORD_PTR)hTaskbarWnd, taskbarRect.left, taskbarRect.top,
        taskbarRect.right, taskbarRect.bottom, currentHeight, targetHeight,
        backfillLogicalHeight, backfillPhysicalHeight, captured ? 1 : 0);
    return true;
}

#if EDGE_RESIZE_PROCESS_DIAG
void EdgeResizeProcessDiagAppendLiteral(WCHAR*& out, PCWSTR text) {
    while (*text) {
        *out++ = *text++;
    }
}

void EdgeResizeProcessDiagAppendUInt(WCHAR*& out, DWORD value) {
    WCHAR digits[16]{};
    int count = 0;
    do {
        digits[count++] = static_cast<WCHAR>(L'0' + (value % 10));
        value /= 10;
    } while (value && count < ARRAYSIZE(digits));

    while (count > 0) {
        *out++ = digits[--count];
    }
}

void EdgeResizeProcessDiagRawMarker(PCWSTR eventName) {
    WCHAR logDir[MAX_PATH]{};
    EdgeResizeGetDiagLogDirPath(logDir, ARRAYSIZE(logDir));

    WCHAR logPath[MAX_PATH]{};
    _snwprintf_s(logPath, _TRUNCATE,
                 L"%s\\draggable-multirow-taskbar-systray-windows-11-raw-pid-%lu.log",
                 logDir, GetCurrentProcessId());

    WCHAR line[256]{};
    WCHAR* lineOut = line;
    EdgeResizeProcessDiagAppendLiteral(lineOut, eventName);
    EdgeResizeProcessDiagAppendLiteral(lineOut, L" pid=");
    EdgeResizeProcessDiagAppendUInt(lineOut, GetCurrentProcessId());
    EdgeResizeProcessDiagAppendLiteral(lineOut, L" tid=");
    EdgeResizeProcessDiagAppendUInt(lineOut, GetCurrentThreadId());
    EdgeResizeProcessDiagAppendLiteral(lineOut, L"\r\n");
    *lineOut = L'\0';

    HANDLE file = CreateFileW(
        logPath, FILE_APPEND_DATA,
        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, nullptr,
        OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (file != INVALID_HANDLE_VALUE) {
        DWORD bytesWritten = 0;
        WriteFile(file, line,
                  static_cast<DWORD>((lineOut - line) * sizeof(wchar_t)),
                  &bytesWritten, nullptr);
        CloseHandle(file);
    }
}

void EdgeResizeProcessDiagRawLog(PCWSTR eventName, HINSTANCE module) {
    EdgeResizeProcessDiagRawMarker(eventName);

    WCHAR logDir[MAX_PATH]{};
    EdgeResizeGetDiagLogDirPath(logDir, ARRAYSIZE(logDir));

    WCHAR logPath[MAX_PATH]{};
    _snwprintf_s(logPath, _TRUNCATE,
                 L"%s\\draggable-multirow-taskbar-systray-windows-11-raw-pid-%lu.log",
                 logDir, GetCurrentProcessId());

    SYSTEMTIME st{};
    GetLocalTime(&st);

    WCHAR line[768]{};
    _snwprintf_s(line, _TRUNCATE,
                 L"%04u-%02u-%02u %02u:%02u:%02u.%03u pid=%lu tid=%lu "
                 L"%s module=%p taskbarView=%p systemTray=%p\r\n",
                 st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute,
                 st.wSecond, st.wMilliseconds, GetCurrentProcessId(),
                 GetCurrentThreadId(), eventName, module,
                 GetModuleHandleW(L"Taskbar.View.dll"),
                 GetModuleHandleW(L"SystemTray.dll"));

    HANDLE file = CreateFileW(
        logPath, FILE_APPEND_DATA,
        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, nullptr,
        OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (file != INVALID_HANDLE_VALUE) {
        DWORD bytesWritten = 0;
        WriteFile(file, line, static_cast<DWORD>(wcslen(line) * sizeof(wchar_t)),
                  &bytesWritten, nullptr);
        CloseHandle(file);
    }
}

BOOL WINAPI DllMain(HINSTANCE module, DWORD reason, LPVOID) {
    if (reason == DLL_PROCESS_ATTACH) {
        EdgeResizeProcessDiagRawLog(L"DllMain attach", module);
        EdgeResizeLog(L"DllMain attach module=%p taskbarView=%p systemTray=%p",
                      module, GetModuleHandleW(L"Taskbar.View.dll"),
                      GetModuleHandleW(L"SystemTray.dll"));
    } else if (reason == DLL_PROCESS_DETACH) {
        EdgeResizeProcessDiagRawLog(L"DllMain detach", module);
        EdgeResizeLog(L"DllMain detach module=%p", module);
    }

    return TRUE;
}
#endif

int EdgeResizeClampInt(int value, int minValue, int maxValue) {
    if (value < minValue) {
        return minValue;
    }
    if (value > maxValue) {
        return maxValue;
    }
    return value;
}

HWND EdgeResizeGetTaskbarRoot(HWND hWnd) {
    HWND root = GetAncestor(hWnd, GA_ROOT);
    if (!root) {
        return nullptr;
    }

    WCHAR className[64]{};
    if (!GetClassNameW(root, className, ARRAYSIZE(className))) {
        return nullptr;
    }

    return _wcsicmp(className, L"Shell_TrayWnd") == 0 ? root : nullptr;
}

int EdgeResizeGetCurrentTaskbarHeight(HWND hTaskbarWnd) {
    RECT rect{};
    if (!GetWindowRect(hTaskbarWnd, &rect)) {
        return g_taskbarHeight ? g_taskbarHeight : g_settings.taskbarHeight;
    }

    return MulDiv(rect.bottom - rect.top, 96, GetDpiForWindow(hTaskbarWnd));
}

bool EdgeResizeForceTaskbarWindowHeight(HWND hTaskbarWnd,
                                        int targetHeight,
                                        PCWSTR phase) {
    if (!hTaskbarWnd || g_unloading || IsVerticalTaskbar()) {
        return false;
    }

    RECT rect{};
    if (!GetWindowRect(hTaskbarWnd, &rect)) {
        EdgeResizeLog(L"force taskbar window no rect phase=%s",
                      phase ? phase : L"<unknown>");
        return false;
    }

    int dpi = GetDpiForWindow(hTaskbarWnd);
    if (!dpi) {
        dpi = 96;
    }

    int targetPhysicalHeight = MulDiv(targetHeight, dpi, 96);
    if (targetPhysicalHeight < 1) {
        targetPhysicalHeight = 1;
    }

    int currentPhysicalHeight = rect.bottom - rect.top;
    if (currentPhysicalHeight >= targetPhysicalHeight - 1 &&
        currentPhysicalHeight <= targetPhysicalHeight + 1) {
        EdgeResizeLog(
            L"force taskbar window already target phase=%s rect=%d,%d,%d,%d "
            L"targetHeight=%d targetPhysical=%d",
            phase ? phase : L"<unknown>", rect.left, rect.top, rect.right,
            rect.bottom, targetHeight, targetPhysicalHeight);
        return true;
    }

    int newTop = rect.bottom - targetPhysicalHeight;
    BOOL moved = SetWindowPos(
        hTaskbarWnd, nullptr, rect.left, newTop, rect.right - rect.left,
        targetPhysicalHeight,
        SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOACTIVATE);
    if (!moved) {
        EdgeResizeLog(
            L"force taskbar window failed phase=%s error=%lu rect=%d,%d,%d,%d "
            L"targetHeight=%d targetPhysical=%d",
            phase ? phase : L"<unknown>", GetLastError(), rect.left, rect.top,
            rect.right, rect.bottom, targetHeight, targetPhysicalHeight);
        return false;
    }

    RedrawWindow(hTaskbarWnd, nullptr, nullptr,
                 RDW_INVALIDATE | RDW_UPDATENOW | RDW_ALLCHILDREN);
    EdgeResizeLog(
        L"force taskbar window moved phase=%s rect=%d,%d,%d,%d newTop=%d "
        L"targetHeight=%d targetPhysical=%d",
        phase ? phase : L"<unknown>", rect.left, rect.top, rect.right,
        rect.bottom, newTop, targetHeight, targetPhysicalHeight);
    return true;
}

void EdgeResizeFlushTaskbarPaint(HWND hTaskbarWnd, PCWSTR phase) {
    if (!hTaskbarWnd || g_unloading) {
        return;
    }

    RedrawWindow(hTaskbarWnd, nullptr, nullptr,
                 RDW_INVALIDATE | RDW_UPDATENOW | RDW_ALLCHILDREN |
                     RDW_FRAME);

    using DwmFlush_t = HRESULT(WINAPI*)();
    static DwmFlush_t dwmFlush = []() -> DwmFlush_t {
        HMODULE module = LoadLibraryW(L"dwmapi.dll");
        if (!module) {
            return nullptr;
        }

        return reinterpret_cast<DwmFlush_t>(
            GetProcAddress(module, "DwmFlush"));
    }();

    HRESULT hr = S_FALSE;
    if (dwmFlush) {
        hr = dwmFlush();
    }

    EdgeResizeLog(L"flush taskbar paint phase=%s hwnd=%08X dwm=0x%08X",
                  phase ? phase : L"<unknown>",
                  (DWORD)(DWORD_PTR)hTaskbarWnd, static_cast<unsigned>(hr));
}

bool EdgeResizeWaitForTaskbarHeight(int targetHeight, DWORD timeoutMs) {
    DWORD startTick = GetTickCount();
    int currentHeight = 0;

    while (GetTickCount() - startTick < timeoutMs) {
        HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();
        if (!hTaskbarWnd) {
            break;
        }

        currentHeight = EdgeResizeGetCurrentTaskbarHeight(hTaskbarWnd);
        int delta = currentHeight > targetHeight ? currentHeight - targetHeight
                                                 : targetHeight - currentHeight;
        if (delta <= 1) {
            EdgeResizeLog(L"settled height target=%d current=%d",
                          targetHeight, currentHeight);
            EdgeResizeUpdateGripWindowPosition();
            return true;
        }

        EdgeResizeUpdateGripWindowPosition();
        Sleep(16);
    }

    EdgeResizeLog(L"settle timeout target=%d current=%d", targetHeight,
                  currentHeight);
    return false;
}

bool EdgeResizePointInGrip(HWND hWnd, POINT ptScreen) {
    if (!g_settings.edgeResizeEnabled || g_unloading || IsVerticalTaskbar()) {
        return false;
    }

    HWND hTaskbarWnd = EdgeResizeGetTaskbarRoot(hWnd);
    if (!hTaskbarWnd) {
        return false;
    }

    RECT rect{};
    if (!GetWindowRect(hTaskbarWnd, &rect)) {
        return false;
    }

    int gripPixels =
        MulDiv(g_settings.edgeResizeGripPixels, GetDpiForWindow(hTaskbarWnd),
               96);

    return ptScreen.x >= rect.left && ptScreen.x < rect.right &&
           ptScreen.y >= rect.top && ptScreen.y < rect.top + gripPixels;
}

bool EdgeResizePointInGripScreen(POINT ptScreen, HWND* taskbarWnd = nullptr) {
    if (taskbarWnd) {
        *taskbarWnd = nullptr;
    }

    if (!g_settings.edgeResizeEnabled || g_unloading || IsVerticalTaskbar()) {
        return false;
    }

    HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();
    if (!hTaskbarWnd) {
        return false;
    }

    RECT rect{};
    if (!GetWindowRect(hTaskbarWnd, &rect)) {
        return false;
    }

    int gripPixels =
        MulDiv(g_settings.edgeResizeGripPixels, GetDpiForWindow(hTaskbarWnd),
               96);

    bool inGrip = ptScreen.x >= rect.left && ptScreen.x < rect.right &&
                  ptScreen.y >= rect.top && ptScreen.y < rect.top + gripPixels;
    if (inGrip && taskbarWnd) {
        *taskbarWnd = hTaskbarWnd;
    }

    return inGrip;
}

int EdgeResizeRowsFromHeight(int height) {
    int rowHeight = g_settings.edgeResizeRowHeight;
    int rows = (height + rowHeight / 2) / rowHeight;
    return EdgeResizeClampInt(rows, g_settings.edgeResizeMinRows,
                              g_settings.edgeResizeMaxRows);
}

int EdgeResizeHeightFromRows(int rows) {
    rows = EdgeResizeClampInt(rows, g_settings.edgeResizeMinRows,
                              g_settings.edgeResizeMaxRows);
    return rows * g_settings.edgeResizeRowHeight;
}

DWORD EdgeResizeNextSettingsChangeTime(HKEY key) {
    DWORD now = static_cast<DWORD>(std::time(nullptr));
    DWORD current = 0;
    DWORD currentType = 0;
    DWORD currentSize = sizeof(current);

    if (RegQueryValueExW(key, L"SettingsChangeTime", nullptr, &currentType,
                         reinterpret_cast<BYTE*>(&current),
                         &currentSize) == ERROR_SUCCESS &&
        currentType == REG_DWORD && currentSize == sizeof(current) &&
        current >= now && current != MAXDWORD) {
        return current + 1;
    }

    return now;
}

void EdgeResizeTouchModSettings(PCWSTR modSubKey) {
    HKEY key{};
    if (RegCreateKeyExW(HKEY_LOCAL_MACHINE, modSubKey, 0, nullptr, 0,
                        KEY_QUERY_VALUE | KEY_SET_VALUE, nullptr, &key,
                        nullptr) != ERROR_SUCCESS) {
        return;
    }

    DWORD settingsChangeTime = EdgeResizeNextSettingsChangeTime(key);
    RegSetValueExW(key, L"SettingsChangeTime", 0, REG_DWORD,
                   reinterpret_cast<const BYTE*>(&settingsChangeTime),
                   sizeof(settingsChangeTime));
    RegCloseKey(key);
}

void EdgeResizePersistHeightAndRows(int height) {
    Wh_SetIntValue(L"EdgeResizeTaskbarHeight", height);

    int rows = EdgeResizeRowsFromHeight(height);

#ifdef WIN11_TASKBAR_WIN10_MULTIROW_COMBINED
    int notificationIconRows = rows;
    Wh_SetIntValue(L"EdgeResizeRows", rows);
    Wh_SetIntValue(L"EdgeResizeNotificationIconRows", notificationIconRows);
#else
    HKEY key{};
    if (RegCreateKeyExW(
            HKEY_LOCAL_MACHINE,
            L"SOFTWARE\\Windhawk\\Engine\\Mods\\" WH_MOD_ID L"\\Settings",
            0, nullptr, 0, KEY_SET_VALUE, nullptr, &key, nullptr) ==
        ERROR_SUCCESS) {
        DWORD value = static_cast<DWORD>(height);
        RegSetValueExW(key, L"TaskbarHeight", 0, REG_DWORD,
                       reinterpret_cast<const BYTE*>(&value), sizeof(value));
        RegCloseKey(key);
    }
    EdgeResizeTouchModSettings(
        L"SOFTWARE\\Windhawk\\Engine\\Mods\\" WH_MOD_ID);

    if (RegCreateKeyExW(
            HKEY_LOCAL_MACHINE,
            L"SOFTWARE\\Windhawk\\Engine\\Mods\\taskbar-multirow\\Settings", 0,
            nullptr, 0, KEY_SET_VALUE, nullptr, &key, nullptr) ==
        ERROR_SUCCESS) {
        DWORD value = static_cast<DWORD>(rows);
        RegSetValueExW(key, L"rows", 0, REG_DWORD,
                       reinterpret_cast<const BYTE*>(&value), sizeof(value));
        RegCloseKey(key);
    }
    EdgeResizeTouchModSettings(
        L"SOFTWARE\\Windhawk\\Engine\\Mods\\taskbar-multirow");

    if (RegCreateKeyExW(
            HKEY_LOCAL_MACHINE,
            L"SOFTWARE\\Windhawk\\Engine\\Mods\\taskbar-notification-icon-spacing\\Settings",
            0, nullptr, 0, KEY_SET_VALUE, nullptr, &key, nullptr) ==
        ERROR_SUCCESS) {
        DWORD value = static_cast<DWORD>(rows);
        RegSetValueExW(key, L"notificationIconRows", 0, REG_DWORD,
                       reinterpret_cast<const BYTE*>(&value), sizeof(value));
        RegCloseKey(key);
    }
    EdgeResizeTouchModSettings(
        L"SOFTWARE\\Windhawk\\Engine\\Mods\\taskbar-notification-icon-spacing");
#endif
}

void EdgeResizeUpdateGripWindowPosition();

bool EdgeResizeWaitForGrowMaterialGate(int height, DWORD delayMs) {
    DWORD startTick = GetTickCount();
    while (GetTickCount() - startTick < delayMs) {
        int pendingHeight = g_edgeResizeRequestedHeight.load();
        if (pendingHeight && pendingHeight != height) {
            EdgeResizeLog(L"material gate interrupted height=%d pending=%d "
                          L"elapsed=%lu",
                          height, pendingHeight, GetTickCount() - startTick);
            return false;
        }

        Sleep(16);
    }

    EdgeResizeLog(L"material gate passed height=%d delay=%lu", height,
                  delayMs);
    return true;
}

void EdgeResizeHoldGrowBackfillAfterLayout(int height, DWORD delayMs) {
    DWORD startTick = GetTickCount();
    DWORD elapsedMs = 0;
    while (elapsedMs < delayMs) {
        int pendingHeight = g_edgeResizeRequestedHeight.load();
        if (pendingHeight && pendingHeight != height) {
            EdgeResizeLog(L"post-layout backfill hold interrupted height=%d "
                          L"pending=%d elapsed=%lu",
                          height, pendingHeight, elapsedMs);
            return;
        }

        Sleep(16);
        elapsedMs = GetTickCount() - startTick;
    }

    EdgeResizeLog(
        L"post-layout backfill hold passed height=%d delay=%lu elapsed=%lu",
        height, delayMs, elapsedMs);
}

#ifdef WIN11_TASKBAR_WIN10_MULTIROW_COMBINED
void EdgeResizePublishLiveTaskbarHeightHint(int height, PCWSTR phase) {
    EdgeResizeLog(L"publish taskbar height phase=%s height=%d",
                  phase ? phase : L"<unknown>", height);
    ::MultirowComponent::SetLiveTaskbarHeightHintNoRefresh(height);
}

void EdgeResizePublishLiveLayoutHints(int rows, int height, PCWSTR phase) {
    EdgeResizeLog(L"publish layout phase=%s rows=%d height=%d",
                  phase ? phase : L"<unknown>", rows, height);
    ::TrayComponent::SetLiveLayoutHints(rows, height);
    ::MultirowComponent::SetLiveLayoutHints(rows, height);
}
#endif

DWORD WINAPI EdgeResizeApplyThreadProc(void*) {
    while (!g_unloading) {
        int height = g_edgeResizeRequestedHeight.exchange(0);
        if (!height) {
            break;
        }

        bool persist = g_edgeResizeRequestedPersist.exchange(false);
        int rows = EdgeResizeRowsFromHeight(height);
        int currentHeight = 0;
        HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();
        if (hTaskbarWnd) {
            currentHeight = EdgeResizeGetCurrentTaskbarHeight(hTaskbarWnd);
        }

        bool growing = currentHeight > 0 && height > currentHeight + 1;
        bool shrinking = currentHeight > 0 && height < currentHeight - 1;
        bool alreadyAtRequestedHeight =
            currentHeight > 0 && currentHeight >= height - 1 &&
            currentHeight <= height + 1 && g_taskbarHeight == height;
        bool skipVisualApply = persist && alreadyAtRequestedHeight;
        bool growBackfillShown = false;

        Wh_Log(L"Edge resize applying height=%d rows=%d persist=%d current=%d "
               L"growing=%d shrinking=%d skipVisual=%d",
               height, rows, persist, currentHeight, growing, shrinking,
               skipVisualApply);
        EdgeResizeLog(L"apply height=%d rows=%d persist=%d current=%d "
                      L"growing=%d shrinking=%d skipVisual=%d",
                      height, rows, persist, currentHeight, growing,
                      shrinking, skipVisualApply);
        g_settings.taskbarHeight = height;
        if (persist) {
            EdgeResizePersistHeightAndRows(height);
        }
#ifdef WIN11_TASKBAR_WIN10_MULTIROW_COMBINED
        if (growing) {
            g_edgeResizeGripPredictedHeight = height;
            EdgeResizeUpdateGripWindowPosition();
            growBackfillShown = EdgeResizeShowGrowBackfill(
                hTaskbarWnd, currentHeight, height, L"pre-grow-freeze");
            if (growBackfillShown) {
                constexpr DWORD kPreGrowFreezePaintMs = 16;
                EdgeResizeLog(L"pre-grow freeze paint delay=%lu",
                              kPreGrowFreezePaintMs);
                Sleep(kPreGrowFreezePaintMs);
            }
            EdgeResizePublishLiveLayoutHints(
                rows, height, L"pre-grow-layout-native-window");
        } else {
            EdgeResizePublishLiveLayoutHints(
                rows, height, shrinking ? L"pre-shrink" : L"pre-stable");
        }
#endif
        bool heightSettled = true;
        if (skipVisualApply) {
            EdgeResizeLog(L"skip visual apply height=%d rows=%d current=%d",
                          height, rows, currentHeight);
        } else {
            ApplySettings(height, false);
            if (growing) {
                EdgeResizeFlushTaskbarPaint(hTaskbarWnd,
                                            L"after-grow-applysettings");
            }
            heightSettled =
                EdgeResizeWaitForTaskbarHeight(height, persist ? 1200 : 700);
        }
#ifdef WIN11_TASKBAR_WIN10_MULTIROW_COMBINED
        if (growing) {
            bool publishPostGrowLayout = true;
            PCWSTR postGrowPhase =
                heightSettled ? L"post-grow-settled" : L"post-grow-timeout";

            if (publishPostGrowLayout) {
                EdgeResizePublishLiveLayoutHints(rows, height, postGrowPhase);
                EdgeResizeFlushTaskbarPaint(hTaskbarWnd, postGrowPhase);
            } else {
                EdgeResizeLog(L"skip stale post-grow layout height=%d rows=%d",
                              height, rows);
            }
            if (growBackfillShown) {
                if (publishPostGrowLayout) {
                    constexpr DWORD kPostGrowFreezeHoldMs = 160;
                    EdgeResizeHoldGrowBackfillAfterLayout(
                        height, kPostGrowFreezeHoldMs);
                }
                EdgeResizeDestroyGrowBackfillWindow(postGrowPhase);
            }
            g_edgeResizeGripPredictedHeight = 0;
        } else {
            g_edgeResizeGripPredictedHeight = 0;
            ::MultirowComponent::RequestTaskbarRowsRefresh();
            ::TrayComponent::RequestNotificationIconRowsRefresh();
        }
#endif
        EdgeResizeUpdateGripWindowPosition();
        Sleep(32);
        EdgeResizeUpdateGripWindowPosition();
    }

    g_edgeResizeApplyRunning = false;

    int pendingHeight = g_edgeResizeRequestedHeight.load();
    if (pendingHeight && !g_unloading &&
        !g_edgeResizeApplyRunning.exchange(true)) {
        HANDLE thread = CreateThread(nullptr, 0, EdgeResizeApplyThreadProc,
                                     nullptr, 0, nullptr);
        if (thread) {
            CloseHandle(thread);
        } else {
            g_edgeResizeApplyRunning = false;
        }
    }

    return 0;
}

void EdgeResizeRequestApply(int height, bool persist) {
    if (height < 2 || g_unloading) {
        return;
    }

    if (persist) {
        g_edgeResizeRequestedPersist = true;
    }
    g_edgeResizeRequestedHeight = height;
    if (!g_edgeResizeApplyRunning.exchange(true)) {
        HANDLE thread =
            CreateThread(nullptr, 0, EdgeResizeApplyThreadProc, nullptr, 0,
                         nullptr);
        if (thread) {
            CloseHandle(thread);
        } else {
            g_edgeResizeApplyRunning = false;
        }
    }
}

void EdgeResizeApplyConfiguredHeight(PCWSTR phase) {
#ifdef WIN11_TASKBAR_WIN10_MULTIROW_COMBINED
    HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();
    int currentHeight =
        hTaskbarWnd ? EdgeResizeGetCurrentTaskbarHeight(hTaskbarWnd) : 0;
    int delta = currentHeight > g_settings.taskbarHeight
                    ? currentHeight - g_settings.taskbarHeight
                    : g_settings.taskbarHeight - currentHeight;

    if (g_settings.edgeResizeEnabled && !g_unloading && hTaskbarWnd &&
        delta > 1) {
        EdgeResizeLog(L"staged configured height phase=%s target=%d "
                      L"current=%d",
                      phase ? phase : L"<unknown>", g_settings.taskbarHeight,
                      currentHeight);
        EdgeResizeRequestApply(g_settings.taskbarHeight, true);
        return;
    }
#endif

    EdgeResizePersistHeightAndRows(g_settings.taskbarHeight);
#ifdef WIN11_TASKBAR_WIN10_MULTIROW_COMBINED
    ::TrayComponent::SetLiveNotificationIconRowsHint(
        EdgeResizeGetNotificationIconRowsSetting());
#endif
    EdgeResizeLog(L"direct configured height phase=%s target=%d",
                  phase ? phase : L"<unknown>", g_settings.taskbarHeight);
    ApplySettings(g_settings.taskbarHeight);
}

int EdgeResizeSnappedHeightFromPoint(POINT pt,
                                     HWND hTaskbarWnd,
                                     int* desiredHeightOut = nullptr,
                                     int* rowsOut = nullptr) {
    int dpi = hTaskbarWnd ? GetDpiForWindow(hTaskbarWnd) : 96;
    int dyLogical = MulDiv(g_edgeResizeDragStartY - pt.y, 96, dpi);
    int desiredHeight = g_edgeResizeDragStartHeight + dyLogical;
    int rows = EdgeResizeRowsFromHeight(desiredHeight);

    if (desiredHeightOut) {
        *desiredHeightOut = desiredHeight;
    }
    if (rowsOut) {
        *rowsOut = rows;
    }

    return EdgeResizeHeightFromRows(rows);
}

void EdgeResizeInitializeLiveHeight() {
    g_edgeResizeLastLiveHeight =
        EdgeResizeHeightFromRows(EdgeResizeRowsFromHeight(
            g_edgeResizeDragStartHeight));
}

void EdgeResizeUpdateLiveDragAtPoint(POINT pt, HWND hTaskbarWnd) {
    if (!g_edgeResizeDragging) {
        return;
    }

    int desiredHeight = 0;
    int rows = 0;
    int snappedHeight =
        EdgeResizeSnappedHeightFromPoint(pt, hTaskbarWnd, &desiredHeight,
                                         &rows);

    if (snappedHeight == g_edgeResizeLastLiveHeight) {
        return;
    }

    g_edgeResizeLastLiveHeight = snappedHeight;
    Wh_Log(L"Edge resize live desired=%d snapped=%d rows=%d", desiredHeight,
           snappedHeight, rows);
    EdgeResizeLog(L"live desired=%d snapped=%d rows=%d", desiredHeight,
                  snappedHeight, rows);
    EdgeResizeRequestApply(snappedHeight, false);
}

void EdgeResizeFinishDragAtPoint(POINT pt, HWND hTaskbarWnd) {
    if (!g_edgeResizeDragging) {
        return;
    }

    int desiredHeight = 0;
    int rows = 0;
    int snappedHeight =
        EdgeResizeSnappedHeightFromPoint(pt, hTaskbarWnd, &desiredHeight,
                                         &rows);

    Wh_Log(L"Edge resize released startHeight=%d desired=%d snapped=%d rows=%d",
           g_edgeResizeDragStartHeight, desiredHeight, snappedHeight, rows);
    EdgeResizeLog(L"release startHeight=%d desired=%d snapped=%d rows=%d",
                  g_edgeResizeDragStartHeight, desiredHeight, snappedHeight,
                  rows);

    g_edgeResizeDragging = false;
    g_edgeResizeLastLiveHeight = snappedHeight;
    EdgeResizeRequestApply(snappedHeight, true);
}

void EdgeResizeFinishDrag(HWND hWnd) {
    if (!g_edgeResizeDragging) {
        return;
    }

    POINT pt{};
    GetCursorPos(&pt);

    HWND hTaskbarWnd = EdgeResizeGetTaskbarRoot(hWnd);
    if (GetCapture() == hWnd) {
        ReleaseCapture();
    }

    EdgeResizeFinishDragAtPoint(pt, hTaskbarWnd);
}

LRESULT CALLBACK EdgeResizeLowLevelMouseProc(int nCode,
                                             WPARAM wParam,
                                             LPARAM lParam) {
    if (nCode == HC_ACTION && !g_unloading && g_settings.edgeResizeEnabled) {
        auto* mouse = reinterpret_cast<MSLLHOOKSTRUCT*>(lParam);
        POINT pt = mouse->pt;

        if (wParam == WM_MOUSEMOVE) {
            if (g_edgeResizeDragging) {
                HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();
                EdgeResizeUpdateLiveDragAtPoint(pt, hTaskbarWnd);
                SetCursor(LoadCursorW(nullptr, IDC_SIZENS));
            } else if (EdgeResizePointInGripScreen(pt)) {
                SetCursor(LoadCursorW(nullptr, IDC_SIZENS));
            }

        } else if (wParam == WM_LBUTTONDOWN) {
            HWND hTaskbarWnd = nullptr;
            bool inGrip = EdgeResizePointInGripScreen(pt, &hTaskbarWnd);
            EdgeResizeLog(L"ll lbuttondown pt=%d,%d inGrip=%d taskbar=%08X",
                          pt.x, pt.y, inGrip,
                          (DWORD)(DWORD_PTR)hTaskbarWnd);
            if (inGrip && hTaskbarWnd) {
                g_edgeResizeDragging = true;
                g_edgeResizeDragStartY = pt.y;
                g_edgeResizeDragStartHeight =
                    EdgeResizeGetCurrentTaskbarHeight(hTaskbarWnd);
                EdgeResizeInitializeLiveHeight();
                SetCursor(LoadCursorW(nullptr, IDC_SIZENS));
                EdgeResizeLog(L"ll start height=%d",
                              g_edgeResizeDragStartHeight);
            }
        } else if (wParam == WM_LBUTTONUP) {
            if (g_edgeResizeDragging) {
                HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();
                EdgeResizeFinishDragAtPoint(pt, hTaskbarWnd);
            }
        }
    }

    return CallNextHookEx(g_edgeResizeMouseHook, nCode, wParam, lParam);
}

DWORD WINAPI EdgeResizeMouseHookThreadProc(void*) {
    g_edgeResizeMouseHookThreadId = GetCurrentThreadId();
    g_edgeResizeMouseHook =
        SetWindowsHookExW(WH_MOUSE_LL, EdgeResizeLowLevelMouseProc, nullptr, 0);
    g_edgeResizeCursorTimer = SetTimer(nullptr, 0, 50, nullptr);

    EdgeResizeLog(L"mouse hook thread=%u hook=%p timer=%llu",
                  g_edgeResizeMouseHookThreadId, g_edgeResizeMouseHook,
                  static_cast<unsigned long long>(g_edgeResizeCursorTimer));

    if (g_edgeResizeMouseHook) {
        MSG msg{};
        while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
            if (msg.message == WM_TIMER &&
                msg.wParam == g_edgeResizeCursorTimer) {
                POINT pt{};
                GetCursorPos(&pt);
                if (g_edgeResizeDragging) {
                    HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();
                    EdgeResizeUpdateLiveDragAtPoint(pt, hTaskbarWnd);
                    SetCursor(LoadCursorW(nullptr, IDC_SIZENS));
                } else if (EdgeResizePointInGripScreen(pt)) {
                    SetCursor(LoadCursorW(nullptr, IDC_SIZENS));
                }
                continue;
            }

            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }

        if (g_edgeResizeCursorTimer) {
            KillTimer(nullptr, g_edgeResizeCursorTimer);
            g_edgeResizeCursorTimer = 0;
        }

        UnhookWindowsHookEx(g_edgeResizeMouseHook);
        g_edgeResizeMouseHook = nullptr;
    }

    g_edgeResizeMouseHookThreadId = 0;
    g_edgeResizeMouseHookThreadRunning = false;
    EdgeResizeLog(L"mouse hook thread stopped");
    return 0;
}

void EdgeResizeInstallMouseHook() {
    if (g_edgeResizeMouseHookThreadRunning.exchange(true)) {
        return;
    }

    HANDLE thread = CreateThread(nullptr, 0, EdgeResizeMouseHookThreadProc,
                                 nullptr, 0, nullptr);
    if (thread) {
        CloseHandle(thread);
    } else {
        g_edgeResizeMouseHookThreadRunning = false;
        EdgeResizeLog(L"failed to create mouse hook thread");
    }
}

void EdgeResizeUninstallMouseHook() {
    DWORD threadId = g_edgeResizeMouseHookThreadId;
    if (threadId) {
        PostThreadMessageW(threadId, WM_QUIT, 0, 0);
    }
}

void EdgeResizeUpdateGripWindowPosition() {
    if (!g_edgeResizeGripWnd || g_unloading ||
        !g_settings.edgeResizeEnabled || IsVerticalTaskbar()) {
        return;
    }

    HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();
    if (!hTaskbarWnd) {
        ShowWindow(g_edgeResizeGripWnd, SW_HIDE);
        return;
    }

    RECT rect{};
    if (!GetWindowRect(hTaskbarWnd, &rect)) {
        ShowWindow(g_edgeResizeGripWnd, SW_HIDE);
        return;
    }

    int dpi = GetDpiForWindow(hTaskbarWnd);
    if (!dpi) {
        dpi = 96;
    }

    int gripPixels = MulDiv(g_settings.edgeResizeGripPixels, dpi, 96);
    if (gripPixels < 2) {
        gripPixels = 2;
    }

    int gripTop = rect.top;
    int predictedHeight = g_edgeResizeGripPredictedHeight.load();
    if (predictedHeight > 0) {
        int predictedPhysicalHeight = MulDiv(predictedHeight, dpi, 96);
        int currentPhysicalHeight = rect.bottom - rect.top;
        if (predictedPhysicalHeight > currentPhysicalHeight + 1) {
            gripTop = rect.bottom - predictedPhysicalHeight;
        }
    }

    SetWindowPos(g_edgeResizeGripWnd, HWND_TOPMOST, rect.left, gripTop,
                 rect.right - rect.left, gripPixels,
                 SWP_NOACTIVATE | SWP_SHOWWINDOW | SWP_ASYNCWINDOWPOS |
                     SWP_NOSENDCHANGING);
}

LRESULT CALLBACK EdgeResizeGripWndProc(HWND hWnd,
                                       UINT msg,
                                       WPARAM wParam,
                                       LPARAM lParam) {
    switch (msg) {
        case WM_CREATE:
            SetTimer(hWnd, 1, 500, nullptr);
            return 0;

        case WM_TIMER:
            if (!g_edgeResizeDragging) {
                EdgeResizeUpdateGripWindowPosition();
            }
            return 0;

        case WM_SETCURSOR:
            SetCursor(LoadCursorW(nullptr, IDC_SIZENS));
            return TRUE;

        case WM_LBUTTONDOWN: {
            POINT pt{};
            GetCursorPos(&pt);
            HWND hTaskbarWnd = nullptr;
            if (EdgeResizePointInGripScreen(pt, &hTaskbarWnd) && hTaskbarWnd) {
                g_edgeResizeDragging = true;
                g_edgeResizeDragStartY = pt.y;
                g_edgeResizeDragStartHeight =
                    EdgeResizeGetCurrentTaskbarHeight(hTaskbarWnd);
                EdgeResizeInitializeLiveHeight();
                SetCapture(hWnd);
                SetCursor(LoadCursorW(nullptr, IDC_SIZENS));
                EdgeResizeLog(L"grip start height=%d pt=%d,%d",
                              g_edgeResizeDragStartHeight, pt.x, pt.y);
                return 0;
            }
            break;
        }

        case WM_MOUSEMOVE:
            if (g_edgeResizeDragging) {
                POINT pt{};
                GetCursorPos(&pt);
                HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();
                EdgeResizeUpdateLiveDragAtPoint(pt, hTaskbarWnd);
            }
            SetCursor(LoadCursorW(nullptr, IDC_SIZENS));
            return 0;

        case WM_LBUTTONUP:
            if (g_edgeResizeDragging) {
                POINT pt{};
                GetCursorPos(&pt);
                HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();
                EdgeResizeFinishDragAtPoint(pt, hTaskbarWnd);
            }
            if (GetCapture() == hWnd) {
                ReleaseCapture();
            }
            EdgeResizeUpdateGripWindowPosition();
            return 0;

        case WM_CANCELMODE:
        case WM_CAPTURECHANGED:
            g_edgeResizeDragging = false;
            return 0;

        case WM_DESTROY:
            KillTimer(hWnd, 1);
            if (g_edgeResizeGripWnd == hWnd) {
                g_edgeResizeGripWnd = nullptr;
            }
            return 0;
    }

    return DefWindowProcW(hWnd, msg, wParam, lParam);
}

DWORD WINAPI EdgeResizeGripThreadProc(void*) {
    g_edgeResizeGripThreadId = GetCurrentThreadId();

    WNDCLASSW wc{};
    wc.lpfnWndProc = EdgeResizeGripWndProc;
    wc.hInstance = GetModuleHandleW(nullptr);
    wc.hCursor = LoadCursorW(nullptr, IDC_SIZENS);
    wc.lpszClassName = L"WindhawkEdgeResizeGripWindow";
    RegisterClassW(&wc);

    g_edgeResizeGripWnd = CreateWindowExW(
        WS_EX_TOOLWINDOW | WS_EX_TOPMOST | WS_EX_NOACTIVATE,
        wc.lpszClassName, L"", WS_POPUP, 0, 0, 1, 1, nullptr, nullptr,
        wc.hInstance, nullptr);

    if (g_edgeResizeGripWnd) {
        EdgeResizeUpdateGripWindowPosition();
        EdgeResizeLog(L"grip window created hwnd=%08X",
                      (DWORD)(DWORD_PTR)g_edgeResizeGripWnd);
    } else {
        EdgeResizeLog(L"failed to create grip window");
    }

    MSG msg{};
    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    if (g_edgeResizeGripWnd) {
        DestroyWindow(g_edgeResizeGripWnd);
        g_edgeResizeGripWnd = nullptr;
    }

    g_edgeResizeGripThreadId = 0;
    g_edgeResizeGripThreadRunning = false;
    EdgeResizeLog(L"grip thread stopped");
    return 0;
}

void EdgeResizeInstallGripWindow() {
    if (!g_settings.edgeResizeEnabled || g_unloading ||
        g_edgeResizeGripThreadRunning.exchange(true)) {
        return;
    }

    HANDLE thread = CreateThread(nullptr, 0, EdgeResizeGripThreadProc, nullptr,
                                 0, nullptr);
    if (thread) {
        CloseHandle(thread);
    } else {
        g_edgeResizeGripThreadRunning = false;
        EdgeResizeLog(L"failed to create grip thread");
    }
}

void EdgeResizeUninstallGripWindow() {
    DWORD threadId = g_edgeResizeGripThreadId;
    HWND gripWnd = g_edgeResizeGripWnd;
    if (gripWnd) {
        PostMessageW(gripWnd, WM_CLOSE, 0, 0);
    }
    if (threadId) {
        PostThreadMessageW(threadId, WM_QUIT, 0, 0);
    }
}

LRESULT CALLBACK EdgeResizeSubclassProc(HWND hWnd,
                                        UINT uMsg,
                                        WPARAM wParam,
                                        LPARAM lParam,
                                        DWORD_PTR) {
    if (uMsg == g_edgeResizeTaskbarCreatedMessage) {
        g_edgeResizeDragging = false;
        return DefSubclassProc(hWnd, uMsg, wParam, lParam);
    }

    switch (uMsg) {
        case WM_SETCURSOR: {
            POINT pt{};
            GetCursorPos(&pt);
            if (EdgeResizePointInGrip(hWnd, pt) || g_edgeResizeDragging) {
                SetCursor(LoadCursorW(nullptr, IDC_SIZENS));
                return TRUE;
            }
            break;
        }

        case WM_LBUTTONDOWN: {
            POINT pt{};
            GetCursorPos(&pt);
            bool inGrip = EdgeResizePointInGrip(hWnd, pt);
            EdgeResizeLog(L"lbuttondown hwnd=%08X pt=%d,%d inGrip=%d",
                          (DWORD)(DWORD_PTR)hWnd, pt.x, pt.y, inGrip);
            if (inGrip) {
                HWND hTaskbarWnd = EdgeResizeGetTaskbarRoot(hWnd);
                g_edgeResizeDragging = true;
                g_edgeResizeDragStartY = pt.y;
                g_edgeResizeDragStartHeight =
                    hTaskbarWnd ? EdgeResizeGetCurrentTaskbarHeight(hTaskbarWnd)
                                : g_settings.taskbarHeight;
                EdgeResizeInitializeLiveHeight();
                SetCapture(hWnd);
                SetCursor(LoadCursorW(nullptr, IDC_SIZENS));
                Wh_Log(L"Edge resize started height=%d",
                       g_edgeResizeDragStartHeight);
                EdgeResizeLog(L"start height=%d hwnd=%08X root=%08X",
                              g_edgeResizeDragStartHeight,
                              (DWORD)(DWORD_PTR)hWnd,
                              (DWORD)(DWORD_PTR)hTaskbarWnd);
                return 0;
            }
            break;
        }

        case WM_MOUSEMOVE:
            if (g_edgeResizeDragging) {
                POINT pt{};
                GetCursorPos(&pt);
                HWND hTaskbarWnd = EdgeResizeGetTaskbarRoot(hWnd);
                EdgeResizeUpdateLiveDragAtPoint(pt, hTaskbarWnd);
                SetCursor(LoadCursorW(nullptr, IDC_SIZENS));
                return 0;
            }
            break;

        case WM_LBUTTONUP:
            if (g_edgeResizeDragging) {
                EdgeResizeFinishDrag(hWnd);
                return 0;
            }
            break;

        case WM_CANCELMODE:
        case WM_CAPTURECHANGED:
            if (g_edgeResizeDragging) {
                g_edgeResizeDragging = false;
            }
            break;

        case WM_DESTROY:
            if (hWnd == g_edgeResizeTaskbarWnd) {
                g_edgeResizeTaskbarWnd = nullptr;
            }
            if (hWnd == g_edgeResizeCoreWindow) {
                g_edgeResizeCoreWindow = nullptr;
            }
            if (hWnd == g_edgeResizeInputSiteWnd) {
                g_edgeResizeInputSiteWnd = nullptr;
            }
            break;
    }

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

HWND EdgeResizeFindInputSiteWindow(HWND hTaskbarWnd) {
    HWND bridge = FindWindowExW(
        hTaskbarWnd, nullptr,
        L"Windows.UI.Composition.DesktopWindowContentBridge", nullptr);
    if (!bridge) {
        return nullptr;
    }

    return FindWindowExW(bridge, nullptr,
                         L"Windows.UI.Input.InputSite.WindowClass", nullptr);
}

HWND EdgeResizeFindCoreWindow(HWND hTaskbarWnd) {
    return FindWindowExW(hTaskbarWnd, nullptr, L"Windows.UI.Core.CoreWindow",
                         nullptr);
}

bool EdgeResizeIsKnownSubclassedWindow(HWND hWnd) {
    if (hWnd == g_edgeResizeTaskbarWnd || hWnd == g_edgeResizeCoreWindow ||
        hWnd == g_edgeResizeInputSiteWnd) {
        return true;
    }

    for (HWND child : g_edgeResizeChildWindows) {
        if (child == hWnd) {
            return true;
        }
    }

    return false;
}

void EdgeResizeSubclassChildWindow(HWND hWnd) {
    if (!hWnd || EdgeResizeIsKnownSubclassedWindow(hWnd)) {
        return;
    }

    WCHAR className[96]{};
    GetClassNameW(hWnd, className, ARRAYSIZE(className));

    RECT rect{};
    GetWindowRect(hWnd, &rect);

    if (WindhawkUtils::SetWindowSubclassFromAnyThread(
            hWnd, EdgeResizeSubclassProc, 0)) {
        g_edgeResizeChildWindows.push_back(hWnd);

        EdgeResizeLog(L"subclassed child=%08X class=%s rect=%d,%d,%d,%d",
                      (DWORD)(DWORD_PTR)hWnd, className, rect.left, rect.top,
                      rect.right, rect.bottom);
    } else {
        EdgeResizeLog(L"failed child=%08X class=%s rect=%d,%d,%d,%d",
                      (DWORD)(DWORD_PTR)hWnd, className, rect.left, rect.top,
                      rect.right, rect.bottom);
    }
}

BOOL CALLBACK EdgeResizeEnumChildProc(HWND hWnd, LPARAM) {
    EdgeResizeSubclassChildWindow(hWnd);
    return TRUE;
}

void EdgeResizeAttachToTaskbar(HWND hTaskbarWnd) {
    if (!hTaskbarWnd) {
        return;
    }

    if (g_edgeResizeTaskbarWnd != hTaskbarWnd) {
        if (g_edgeResizeTaskbarWnd) {
            WindhawkUtils::RemoveWindowSubclassFromAnyThread(
                g_edgeResizeTaskbarWnd, EdgeResizeSubclassProc);
            g_edgeResizeTaskbarWnd = nullptr;
        }

        if (WindhawkUtils::SetWindowSubclassFromAnyThread(
                hTaskbarWnd, EdgeResizeSubclassProc, 0)) {
            g_edgeResizeTaskbarWnd = hTaskbarWnd;
            EdgeResizeLog(L"subclassed taskbar=%08X",
                          (DWORD)(DWORD_PTR)hTaskbarWnd);
        } else {
            EdgeResizeLog(L"failed to subclass taskbar=%08X",
                          (DWORD)(DWORD_PTR)hTaskbarWnd);
        }
    }

    EdgeResizeLog(L"install mouse hook taskbar=%08X",
                  (DWORD)(DWORD_PTR)hTaskbarWnd);
    EdgeResizeInstallMouseHook();
    EdgeResizeInstallGripWindow();
}

DWORD WINAPI EdgeResizeDelayedInstallThreadProc(void*) {
    for (int attempt = 1; attempt <= 40 && !g_unloading &&
                            g_settings.edgeResizeEnabled;
         attempt++) {
        Sleep(500);
        HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();
        EdgeResizeLog(L"delayed install pass attempt=%d taskbar=%08X",
                      attempt, (DWORD)(DWORD_PTR)hTaskbarWnd);
        if (hTaskbarWnd) {
            EdgeResizeAttachToTaskbar(hTaskbarWnd);
            EdgeResizeLog(L"delayed install applying height=%d",
                          g_settings.taskbarHeight);
            ApplySettings(g_settings.taskbarHeight, false);
#ifdef WIN11_TASKBAR_WIN10_MULTIROW_COMBINED
            EdgeResizePublishLiveLayoutHints(
                EdgeResizeRowsFromHeight(g_settings.taskbarHeight),
                g_settings.taskbarHeight, L"delayed-install");
#endif
            break;
        }
    }

    g_edgeResizeDelayedInstallRunning = false;
    return 0;
}

void EdgeResizeScheduleDelayedInstallPass() {
    if (g_edgeResizeDelayedInstallRunning.exchange(true)) {
        return;
    }

    HANDLE thread = CreateThread(nullptr, 0, EdgeResizeDelayedInstallThreadProc,
                                 nullptr, 0, nullptr);
    if (thread) {
        CloseHandle(thread);
    } else {
        g_edgeResizeDelayedInstallRunning = false;
    }
}

void EdgeResizeInstallSubclass() {
    if (!g_settings.edgeResizeEnabled || g_unloading) {
        return;
    }

    HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();
    if (!hTaskbarWnd) {
        EdgeResizeLog(L"skip mouse hook: no Shell_TrayWnd in this process");
        EdgeResizeScheduleDelayedInstallPass();
        return;
    }

    EdgeResizeAttachToTaskbar(hTaskbarWnd);
}

void EdgeResizeUninstallSubclass() {
    if (g_edgeResizeTaskbarWnd) {
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(
            g_edgeResizeTaskbarWnd, EdgeResizeSubclassProc);
        g_edgeResizeTaskbarWnd = nullptr;
    }

    if (g_edgeResizeCoreWindow) {
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(
            g_edgeResizeCoreWindow, EdgeResizeSubclassProc);
        g_edgeResizeCoreWindow = nullptr;
    }

    if (g_edgeResizeInputSiteWnd) {
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(
            g_edgeResizeInputSiteWnd, EdgeResizeSubclassProc);
        g_edgeResizeInputSiteWnd = nullptr;
    }

    for (HWND child : g_edgeResizeChildWindows) {
        if (child) {
            WindhawkUtils::RemoveWindowSubclassFromAnyThread(
                child, EdgeResizeSubclassProc);
        }
    }
    g_edgeResizeChildWindows.clear();

    g_edgeResizeDragging = false;
    EdgeResizeDestroyGrowBackfillWindow(L"uninstall-subclass");
    EdgeResizeUninstallMouseHook();
    EdgeResizeUninstallGripWindow();
}

bool HookSystemTraySymbols(HMODULE module) {
    // SystemTray.dll
    WindhawkUtils::SYMBOL_HOOK symbolHooks[] = {
        {
            {LR"(private: double __cdecl winrt::SystemTray::implementation::SystemTrayController::GetFrameSize(enum winrt::WindowsUdk::UI::Shell::TaskbarSize))"},
            &SystemTrayController_GetFrameSize_Original,
            SystemTrayController_GetFrameSize_Hook,
            true,  // From Windows 11 version 22H2, inlined sometimes.
        },
        {
            {LR"(private: double __cdecl winrt::SystemTray::implementation::SystemTraySecondaryController::GetFrameSize(enum winrt::WindowsUdk::UI::Shell::TaskbarSize))"},
            &SystemTraySecondaryController_GetFrameSize_Original,
            SystemTraySecondaryController_GetFrameSize_Hook,
            true,  // From Windows 11 version 22H2.
        },
        {
            {LR"(private: void __cdecl winrt::SystemTray::implementation::SystemTrayController::UpdateFrameSize(void))"},
            &SystemTrayController_UpdateFrameSize_SymbolAddress,
            nullptr,  // Hooked manually, we need the symbol address.
            true,     // Missing in older Windows 11 versions.
        },
        {
            {LR"(private: void __cdecl winrt::SystemTray::implementation::SystemTraySecondaryController::UpdateFrameSize(void))"},
            &SystemTraySecondaryController_UpdateFrameSize_Original,
            SystemTraySecondaryController_UpdateFrameSize_Hook,
            true,  // Missing in older Windows 11 versions.
        },
        {
            {LR"(public: __cdecl winrt::impl::consume_Windows_UI_Xaml_IFrameworkElement<struct winrt::SystemTray::SystemTrayFrame>::Height(double)const )"},
            &SystemTrayFrame_Height_Original,
            SystemTrayFrame_Height_Hook,
            true,  // From Windows 11 version 22H2.
        },
    };

    if (!HookSymbols(module, symbolHooks, ARRAYSIZE(symbolHooks))) {
        Wh_Log(L"HookSymbols failed");
        return false;
    }

    if (SystemTrayController_UpdateFrameSize_SymbolAddress) {
        SystemTrayController_UpdateFrameSize_InitOffsets();
        WindhawkUtils::Wh_SetFunctionHookT(
            SystemTrayController_UpdateFrameSize_SymbolAddress,
            SystemTrayController_UpdateFrameSize_Hook,
            &SystemTrayController_UpdateFrameSize_Original);
    }

    return true;
}

bool HookTaskbarViewDllSymbols(HMODULE module,
                               bool hookSystemTraySymbolsInline) {
#if EDGE_RESIZE_DISABLE_TASKBAR_VIEW_HOOKS
    EdgeResizeLog(L"diag HookTaskbarViewDllSymbols skipped module=%p "
                  L"inlineTray=%d due to "
                  L"EDGE_RESIZE_DISABLE_TASKBAR_VIEW_HOOKS",
                  module, hookSystemTraySymbolsInline ? 1 : 0);
    return true;
#endif

    EdgeResizeLog(L"diag HookTaskbarViewDllSymbols start module=%p inlineTray=%d",
                  module, hookSystemTraySymbolsInline ? 1 : 0);

    // Taskbar.View.dll, ExplorerExtensions.dll
    WindhawkUtils::SYMBOL_HOOK symbolHooks[] =  //
        {
            {
                // For Windows 11 version 21H2.
                {LR"(__real@4048000000000000)"},
                &double_48_value_Original,
                nullptr,
                true,
            },
#if !EDGE_RESIZE_FRAME_HOOKS_ONLY || EDGE_RESIZE_ENABLE_ICON_METRIC_HOOKS
            {
                {
                    LR"(public: __cdecl winrt::impl::consume_Windows_Foundation_Collections_IMap<struct winrt::Windows::UI::Xaml::ResourceDictionary,struct winrt::Windows::Foundation::IInspectable,struct winrt::Windows::Foundation::IInspectable>::Lookup(struct winrt::Windows::Foundation::IInspectable const &)const )",

                    // Windows 11 version 21H2.
                    LR"(public: struct winrt::Windows::Foundation::IInspectable __cdecl winrt::impl::consume_Windows_Foundation_Collections_IMap<struct winrt::Windows::UI::Xaml::ResourceDictionary,struct winrt::Windows::Foundation::IInspectable,struct winrt::Windows::Foundation::IInspectable>::Lookup(struct winrt::Windows::Foundation::IInspectable const &)const )",
                },
                &ResourceDictionary_Lookup_TaskbarView_Original,
                ResourceDictionary_Lookup_TaskbarView_Hook,
            },
            {
                // Pre-DynamicIconScaling.
                {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskListItemViewModel,struct winrt::Taskbar::ITaskListItemViewModel>::GetIconHeight(void *,double *))"},
                &TaskListItemViewModel_GetIconHeight_Original,
                TaskListItemViewModel_GetIconHeight_Hook,
                true,  // Gone in KB5040527 (Taskbar.View.dll 2124.16310.10.0).
            },
            {
                // Pre-DynamicIconScaling.
                {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskListGroupViewModel,struct winrt::Taskbar::ITaskbarAppItemViewModel>::GetIconHeight(void *,double *))"},
                &TaskListGroupViewModel_GetIconHeight_Original,
                TaskListGroupViewModel_GetIconHeight_Hook,
                true,  // Missing in older Windows 11 versions.
            },
            {
                // Pre-DynamicIconScaling.
                {LR"(public: static double __cdecl winrt::Taskbar::implementation::TaskbarConfiguration::GetIconHeightInViewPixels(enum winrt::WindowsUdk::UI::Shell::TaskbarSize))"},
                &TaskbarConfiguration_GetIconHeightInViewPixels_taskbarSizeEnum_Original,
                TaskbarConfiguration_GetIconHeightInViewPixels_taskbarSizeEnum_Hook,
            },
            {
                // Pre-DynamicIconScaling.
                {LR"(public: static double __cdecl winrt::Taskbar::implementation::TaskbarConfiguration::GetIconHeightInViewPixels(double))"},
                &TaskbarConfiguration_GetIconHeightInViewPixels_double_Original,
                TaskbarConfiguration_GetIconHeightInViewPixels_double_Hook,
                true,  // From Windows 11 version 22H2.
            },
            {
                {LR"(public: double __cdecl winrt::Taskbar::implementation::TaskbarConfiguration::GetIconHeightInViewPixels(void))"},
                &TaskbarConfiguration_GetIconHeightInViewPixels_method_Original,
                TaskbarConfiguration_GetIconHeightInViewPixels_method_Hook,
                true,  // From KB5044384 (October 2024).
            },
            {
                {LR"(public: void __cdecl winrt::Taskbar::implementation::TaskListButton::IconHeight(double))"},
                &TaskListButton_IconHeight_Original,
                nullptr,
                true,  // From KB5058499 (May 2025).
            },
#endif
            {
                {LR"(public: static double __cdecl winrt::Taskbar::implementation::TaskbarConfiguration::GetFrameSize(enum winrt::WindowsUdk::UI::Shell::TaskbarSize))"},
                &TaskbarConfiguration_GetFrameSize_Original,
                TaskbarConfiguration_GetFrameSize_Hook,
                true,  // From Windows 11 version 22H2.
            },
#ifdef _M_ARM64
            // In ARM64, the TaskbarConfiguration::GetFrameSize function is
            // inlined. As a workaround, hook
            // TaskbarConfiguration::UpdateFrameSize which its inlined in and do
            // some ugly assembly tinkering.
            {
                {LR"(private: void __cdecl winrt::Taskbar::implementation::TaskbarConfiguration::UpdateFrameSize(void))"},
                &TaskbarConfiguration_UpdateFrameSize_SymbolAddress,
                nullptr,  // Hooked manually, we need the symbol address.
            },
            {
                {LR"(public: void __cdecl winrt::event<struct winrt::delegate<> >::operator()<>(void))"},
                &Event_operator_call_Original,
                Event_operator_call_Hook,
            },
#endif
            {
                {LR"(public: __cdecl winrt::impl::consume_Windows_UI_Xaml_IFrameworkElement<struct winrt::Taskbar::implementation::TaskbarFrame>::MaxHeight(double)const )"},
                &TaskbarFrame_MaxHeight_double_Original,
                nullptr,
                true,  // From Windows 11 version 22H2.
            },
            {
                {
                    LR"(public: __cdecl winrt::impl::consume_Windows_UI_Xaml_IFrameworkElement<struct winrt::Taskbar::implementation::TaskbarFrame>::Height(double)const )",

                    // Windows 11 version 21H2.
                    LR"(public: void __cdecl winrt::impl::consume_Windows_UI_Xaml_IFrameworkElement<struct winrt::Taskbar::implementation::TaskbarFrame>::Height(double)const )",
                },
                &TaskbarFrame_Height_double_Original,
                TaskbarFrame_Height_double_Hook,
                true,  // Gone in Windows 11 version 24H2.
            },
            {
                {LR"(private: void __cdecl winrt::Taskbar::implementation::TaskbarController::OnGroupingModeChanged(void))"},
                &TaskbarController_OnGroupingModeChanged_Original,
                nullptr,
                true,  // Missing in older Windows 11 versions.
            },
            {
                {LR"(private: void __cdecl winrt::Taskbar::implementation::TaskbarController::UpdateFrameHeight(void))"},
                &TaskbarController_UpdateFrameHeight_Original,
                TaskbarController_UpdateFrameHeight_Hook,
                true,  // Missing in older Windows 11 versions.
            },
            {
                {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskbarFrame,struct winrt::Windows::UI::Xaml::IFrameworkElementOverrides>::MeasureOverride(struct winrt::Windows::Foundation::Size,struct winrt::Windows::Foundation::Size *))"},
                &TaskbarFrame_MeasureOverride_Original,
                TaskbarFrame_MeasureOverride_Hook,
            },
#if EDGE_RESIZE_RUNNING_INDICATOR_EXPERIMENT
            {
                {LR"(private: void __cdecl winrt::Taskbar::implementation::TaskbarFrame::OnTaskbarLayoutChildBoundsChanged(void))"},
                &TaskbarFrame_OnTaskbarLayoutChildBoundsChanged_Original,
                TaskbarFrame_OnTaskbarLayoutChildBoundsChanged_Hook,
                true,  // Missing in older Windows 11 versions.
            },
#endif
#if !EDGE_RESIZE_FRAME_HOOKS_ONLY
            {
                {LR"(private: void __cdecl winrt::Taskbar::implementation::TaskListButton::UpdateButtonPadding(void))"},
                &TaskListButton_UpdateButtonPadding_Original,
                TaskListButton_UpdateButtonPadding_Hook,
            },
            {
                {LR"(public: void __cdecl winrt::Taskbar::implementation::TaskListButton::OverlayIcon(struct winrt::Windows::Storage::Streams::IRandomAccessStream const &))"},
                &TaskListButton_OverlayIcon_Original,
                TaskListButton_OverlayIcon_Hook,
            },
            {
                {LR"(private: void __cdecl winrt::Taskbar::implementation::TaskListButton::UpdateBadge(void))"},
                &TaskListButton_UpdateBadge_Original,
                TaskListButton_UpdateBadge_Hook,
            },
            {
                {LR"(private: void __cdecl winrt::Taskbar::implementation::TaskListButton::UpdateIconColumnDefinition(void))"},
                &TaskListButton_UpdateIconColumnDefinition_Original,
                nullptr,
                true,  // Missing in older Windows 11 versions.
            },
#if EDGE_RESIZE_RUNNING_INDICATOR_EXPERIMENT
            {
                {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskListButton,struct winrt::Taskbar::ITaskListButton>::get_IsRunning(bool *))"},
                &TaskListButton_get_IsRunning_Original,
                nullptr,
                true,  // Optional; used only to avoid stale running bars.
            },
#endif
#if EDGE_RESIZE_RUNNING_INDICATOR_EXPERIMENT
            {
                {LR"(private: void __cdecl winrt::Taskbar::implementation::TaskListButton::UpdateVisualStates(void))"},
                &TaskListButton_UpdateVisualStates_Original,
                TaskListButton_UpdateVisualStates_Hook,
            },
#endif
            {
                {LR"(public: virtual void __cdecl winrt::Taskbar::implementation::LaunchListItemViewModel::IconHeight(double))"},
                &LaunchListItemViewModel_IconHeight_Original,
                LaunchListItemViewModel_IconHeight_Hook,
                true,
            },
#endif
#if !EDGE_RESIZE_FRAME_HOOKS_ONLY || EDGE_RESIZE_ENABLE_LEADING_BUTTON_HOOKS
            {
                {LR"(protected: virtual void __cdecl winrt::Taskbar::implementation::ExperienceToggleButton::UpdateButtonPadding(void))"},
                &ExperienceToggleButton_UpdateButtonPadding_Original,
                ExperienceToggleButton_UpdateButtonPadding_Hook,
            },
            {
                {LR"(protected: virtual void __cdecl winrt::Taskbar::implementation::AugmentedEntryPointButton::UpdateButtonPadding(void))"},
                &AugmentedEntryPointButton_UpdateButtonPadding_Original,
                AugmentedEntryPointButton_UpdateButtonPadding_Hook,
            },
            {
                {LR"(public: __cdecl winrt::impl::consume_Windows_UI_Xaml_IFrameworkElement<struct winrt::Windows::UI::Xaml::Controls::Primitives::RepeatButton>::Width(double)const )"},
                &RepeatButton_Width_Original,
                RepeatButton_Width_Hook,
                true,  // From Windows 11 version 22H2.
            },
#endif
        };

    // On older Taskbar.View.dll versions (before the SystemTray types moved out
    // into SystemTray.dll), these SystemTray symbols live in Taskbar.View.dll
    // itself, so include them in the same hook batch when
    // hookSystemTraySymbolsInline is set.

    // Taskbar.View.dll, ExplorerExtensions.dll
    WindhawkUtils::SYMBOL_HOOK symbolHooksSystemTray[] = {
        {
            {LR"(private: double __cdecl winrt::SystemTray::implementation::SystemTrayController::GetFrameSize(enum winrt::WindowsUdk::UI::Shell::TaskbarSize))"},
            &SystemTrayController_GetFrameSize_Original,
            SystemTrayController_GetFrameSize_Hook,
            true,  // From Windows 11 version 22H2, inlined sometimes.
        },
        {
            {LR"(private: double __cdecl winrt::SystemTray::implementation::SystemTraySecondaryController::GetFrameSize(enum winrt::WindowsUdk::UI::Shell::TaskbarSize))"},
            &SystemTraySecondaryController_GetFrameSize_Original,
            SystemTraySecondaryController_GetFrameSize_Hook,
            true,  // From Windows 11 version 22H2.
        },
        {
            {LR"(private: void __cdecl winrt::SystemTray::implementation::SystemTrayController::UpdateFrameSize(void))"},
            &SystemTrayController_UpdateFrameSize_SymbolAddress,
            nullptr,  // Hooked manually, we need the symbol address.
            true,     // Missing in older Windows 11 versions.
        },
        {
            {LR"(private: void __cdecl winrt::SystemTray::implementation::SystemTraySecondaryController::UpdateFrameSize(void))"},
            &SystemTraySecondaryController_UpdateFrameSize_Original,
            SystemTraySecondaryController_UpdateFrameSize_Hook,
            true,  // Missing in older Windows 11 versions.
        },
        {
            {LR"(public: __cdecl winrt::impl::consume_Windows_UI_Xaml_IFrameworkElement<struct winrt::SystemTray::SystemTrayFrame>::Height(double)const )"},
            &SystemTrayFrame_Height_Original,
            SystemTrayFrame_Height_Hook,
            true,  // From Windows 11 version 22H2.
        },
    };

    // Alias for the extract_mod_symbols.py script.
    using COMBINED_SH = WindhawkUtils::SYMBOL_HOOK;
    COMBINED_SH allHooks[  //
        ARRAYSIZE(symbolHooks) + ARRAYSIZE(symbolHooksSystemTray)];
    int index = 0;

    for (auto& hook : symbolHooks) {
        allHooks[index++] = std::move(hook);
    }

    if (hookSystemTraySymbolsInline) {
        for (auto& hook : symbolHooksSystemTray) {
            allHooks[index++] = std::move(hook);
        }
    }

    if (!HookSymbols(module, allHooks, index)) {
        Wh_Log(L"HookSymbols failed");
        EdgeResizeLog(L"diag HookTaskbarViewDllSymbols HookSymbols FAILED "
                      L"module=%p hooks=%d updateVisualStatesOriginal=%p "
                      L"getIsRunningOriginal=%p",
                      module, index, TaskListButton_UpdateVisualStates_Original,
                      TaskListButton_get_IsRunning_Original);
        return false;
    }

    EdgeResizeLog(L"diag HookTaskbarViewDllSymbols ok module=%p hooks=%d "
                  L"updateVisualStatesOriginal=%p getIsRunningOriginal=%p "
                  L"frameLayoutOriginal=%p",
                  module, index, TaskListButton_UpdateVisualStates_Original,
                  TaskListButton_get_IsRunning_Original,
                  TaskbarFrame_OnTaskbarLayoutChildBoundsChanged_Original);

    if (TaskListButton_IconHeight_Original) {
        TaskListButton_IconHeight_InitOffsets();
    }

#ifdef _M_ARM64
    if (TaskbarConfiguration_UpdateFrameSize_SymbolAddress) {
        TaskbarConfiguration_UpdateFrameSize_InitOffsets();
        WindhawkUtils::Wh_SetFunctionHookT(
            TaskbarConfiguration_UpdateFrameSize_SymbolAddress,
            TaskbarConfiguration_UpdateFrameSize_Hook,
            &TaskbarConfiguration_UpdateFrameSize_Original);
    }
#endif

    if (hookSystemTraySymbolsInline &&
        SystemTrayController_UpdateFrameSize_SymbolAddress) {
        SystemTrayController_UpdateFrameSize_InitOffsets();
        WindhawkUtils::Wh_SetFunctionHookT(
            SystemTrayController_UpdateFrameSize_SymbolAddress,
            SystemTrayController_UpdateFrameSize_Hook,
            &SystemTrayController_UpdateFrameSize_Original);
    }

    if (TaskbarController_OnGroupingModeChanged_Original) {
        TaskbarController_OnGroupingModeChanged_InitOffsets();
    }

    if (TaskListButton_UpdateIconColumnDefinition_Original) {
        TaskListButton_UpdateIconColumnDefinition_InitOffsets();
    }

    constexpr UINT kDynamicIconScaling = 29785184;
    if (TaskbarConfiguration_GetIconHeightInViewPixels_method_Original &&
        IsOsFeatureEnabled(kDynamicIconScaling).value_or(true)) {
        g_hasDynamicIconScaling = true;
        Wh_Log(L"Dynamic icon scaling is enabled");
    }

    return true;
}

bool HookSearchUxUiDllSymbols(HMODULE module) {
    // SearchUx.UI.dll
    WindhawkUtils::SYMBOL_HOOK symbolHooks[] = {
        {
            {LR"(public: __cdecl winrt::impl::consume_Windows_Foundation_Collections_IMap<struct winrt::Windows::UI::Xaml::ResourceDictionary,struct winrt::Windows::Foundation::IInspectable,struct winrt::Windows::Foundation::IInspectable>::Lookup(struct winrt::Windows::Foundation::IInspectable const &)const )"},
            &ResourceDictionary_Lookup_SearchUxUi_Original,
            ResourceDictionary_Lookup_SearchUxUi_Hook,
        },
        {
            {LR"(protected: virtual void __cdecl winrt::SearchUx::SearchUI::implementation::SearchButtonBase::UpdateButtonPadding(void))"},
            &SearchButtonBase_UpdateButtonPadding_Original,
            SearchButtonBase_UpdateButtonPadding_Hook,
        },
    };

    if (!HookSymbols(module, symbolHooks, ARRAYSIZE(symbolHooks))) {
        Wh_Log(L"HookSymbols failed");
        return false;
    }

    return true;
}

bool HookTaskbarDllSymbols() {
    HMODULE module =
        LoadLibraryEx(L"taskbar.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!module) {
        Wh_Log(L"Failed to load taskbar.dll");
        return false;
    }

    WindhawkUtils::SYMBOL_HOOK taskbarDllHooks[] = {
        {
            // Pre-DynamicIconScaling.
            {LR"(void __cdecl IconUtils::GetIconSize(bool,enum IconUtils::IconType,struct tagSIZE *))"},
            &IconUtils_GetIconSize_Original,
            IconUtils_GetIconSize_Hook,
        },
        {
            // Pre-DynamicIconScaling.
            {LR"(public: virtual bool __cdecl IconContainer::IsStorageRecreationRequired(class CCoSimpleArray<unsigned int,4294967294,class CSimpleArrayStandardCompareHelper<unsigned int> > const &,enum IconContainerFlags))"},
            &IconContainer_IsStorageRecreationRequired_Original,
            IconContainer_IsStorageRecreationRequired_Hook,
        },
        {
            {LR"(public: virtual void __cdecl TrayUI::GetMinSize(struct HMONITOR__ *,struct tagSIZE *))"},
            &TrayUI_GetMinSize_Original,
            TrayUI_GetMinSize_Hook,
            true,
        },
        {
            // Pre-DynamicIconScaling.
            {LR"(public: virtual unsigned __int64 __cdecl CIconLoadingFunctions::GetClassLongPtrW(struct HWND__ *,int))"},
            &CIconLoadingFunctions_GetClassLongPtrW_Original,
            CIconLoadingFunctions_GetClassLongPtrW_Hook,
        },
        {
            // Pre-DynamicIconScaling.
            {LR"(public: virtual int __cdecl CIconLoadingFunctions::SendMessageCallbackW(struct HWND__ *,unsigned int,unsigned __int64,__int64,void (__cdecl*)(struct HWND__ *,unsigned int,unsigned __int64,__int64),unsigned __int64))"},
            &CIconLoadingFunctions_SendMessageCallbackW_Original,
            CIconLoadingFunctions_SendMessageCallbackW_Hook,
        },
        {
            // Pre-DynamicIconScaling.
            {LR"(static  ShellIconLoaderV2::LoadAsyncIcon$_ResumeCoro$1())"},
            &ShellIconLoaderV2_LoadAsyncIcon__ResumeCoro_Original,
            ShellIconLoaderV2_LoadAsyncIcon__ResumeCoro_Hook,
            true,
        },
        {
            {LR"(public: void __cdecl TrayUI::_StuckTrayChange(void))"},
            &TrayUI__StuckTrayChange_Original,
        },
        {
            {LR"(public: void __cdecl TrayUI::_HandleSettingChange(struct HWND__ *,unsigned int,unsigned __int64,__int64))"},
            &TrayUI__HandleSettingChange_Original,
            TrayUI__HandleSettingChange_Hook,
        },
    };

    if (!HookSymbols(module, taskbarDllHooks, ARRAYSIZE(taskbarDllHooks))) {
        Wh_Log(L"HookSymbols failed");
        return false;
    }

    return true;
}

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

HMODULE GetTaskbarViewModuleHandle() {
    HMODULE module = GetModuleHandle(L"Taskbar.View.dll");
    if (!module) {
        module = GetModuleHandle(L"ExplorerExtensions.dll");
    }

    return module;
}

HMODULE GetSystemTrayModuleHandle() {
    HMODULE module = GetModuleHandle(L"SystemTray.dll");
    if (!module) {
        module = GetModuleHandle(L"Taskbar.View.dll");
        if (module) {
            // Starting with Taskbar.View.dll 2604.8002.200.6000, the SystemTray
            // types moved out of Taskbar.View.dll into SystemTray.dll, so don't
            // treat Taskbar.View.dll as the host at this version and above.
            VS_FIXEDFILEINFO* fixedFileInfo =
                GetModuleVersionInfo(module, nullptr);
            WORD moduleMajor =
                fixedFileInfo ? HIWORD(fixedFileInfo->dwFileVersionMS) : 0;
            if (!moduleMajor || moduleMajor >= 2604) {
                Wh_Log(L"Skipping Taskbar.View.dll version %d", moduleMajor);
                module = nullptr;
            }
        }
    }
    if (!module) {
        module = GetModuleHandle(L"ExplorerExtensions.dll");
    }

    return module;
}

HMODULE GetSearchUxUiModuleHandle() {
    return GetModuleHandle(L"SearchUx.UI.dll");
}

void RunningIndicatorDiagEnsureTaskbarViewHooked(PCWSTR reason) {
    if (g_taskbarViewDllLoaded.load() || g_unloading) {
        return;
    }

    if (g_runningIndicatorDiagHookAttemptRunning.exchange(true)) {
        return;
    }

    HMODULE taskbarViewModule = GetTaskbarViewModuleHandle();
    if (!taskbarViewModule) {
        EdgeResizeLog(L"diag ensure reason=%s no Taskbar.View yet", reason);
        g_runningIndicatorDiagHookAttemptRunning = false;
        return;
    }

    EdgeResizeLog(L"diag ensure reason=%s found taskbarView=%p", reason,
                  taskbarViewModule);

    bool hookSystemTraySymbolsInline =
        !g_systemTrayModuleHooked && GetSystemTrayModuleHandle() == taskbarViewModule;
    if (hookSystemTraySymbolsInline) {
        g_systemTrayModuleHooked = true;
    }

    if (HookTaskbarViewDllSymbols(taskbarViewModule,
                                  hookSystemTraySymbolsInline)) {
        g_taskbarViewDllLoaded = true;
        Wh_ApplyHookOperations();
        EdgeResizeLog(L"diag ensure reason=%s HookTaskbarViewDllSymbols ok",
                      reason);
    } else {
        if (hookSystemTraySymbolsInline) {
            g_systemTrayModuleHooked = false;
        }
        EdgeResizeLog(L"diag ensure reason=%s HookTaskbarViewDllSymbols FAILED",
                      reason);
    }

    g_runningIndicatorDiagHookAttemptRunning = false;
}

using LoadLibraryExW_t = decltype(&LoadLibraryExW);
LoadLibraryExW_t LoadLibraryExW_Original;
using LoadLibraryW_t = decltype(&LoadLibraryW);
LoadLibraryW_t LoadLibraryW_Original;
using LoadPackagedLibrary_t = HMODULE(WINAPI*)(LPCWSTR lpwLibFileName,
                                               DWORD Reserved);
LoadPackagedLibrary_t LoadPackagedLibrary_Original;

void RunningIndicatorDiagAfterLibraryLoad(PCWSTR reason,
                                          PCWSTR libraryName,
                                          HMODULE module) {
    if (!module || g_taskbarViewDllLoaded.load()) {
        return;
    }

    HMODULE taskbarViewModule = GetTaskbarViewModuleHandle();
    if (!taskbarViewModule) {
        return;
    }

    EdgeResizeLog(L"diag loader reason=%s name=%s module=%p taskbarView=%p",
                  reason, libraryName ? libraryName : L"<null>", module,
                  taskbarViewModule);
    RunningIndicatorDiagEnsureTaskbarViewHooked(reason);
}

HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR lpLibFileName,
                                   HANDLE hFile,
                                   DWORD dwFlags) {
    HMODULE module = LoadLibraryExW_Original(lpLibFileName, hFile, dwFlags);
    if (!module) {
        return module;
    }

    // SystemTray.dll - skipped here when the resolved module is actually an
    // older Taskbar.View.dll (the block below hooks both in a single batch).
    if (!g_systemTrayModuleHooked && GetSystemTrayModuleHandle() == module &&
        module != GetTaskbarViewModuleHandle() &&
        !g_systemTrayModuleHooked.exchange(true)) {
        Wh_Log(L"Loaded %s", lpLibFileName);

        if (HookSystemTraySymbols(module)) {
            Wh_ApplyHookOperations();
        }
    }

    if (!g_taskbarViewDllLoaded && GetTaskbarViewModuleHandle() == module &&
        !g_taskbarViewDllLoaded.exchange(true)) {
        Wh_Log(L"Loaded %s", lpLibFileName);

        // If SystemTray.dll wasn't loaded above and this Taskbar.View.dll is an
        // older version that hosts SystemTray symbols inline, hook them in the
        // same batch.
        bool hookSystemTraySymbolsInline =
            !g_systemTrayModuleHooked &&
            GetSystemTrayModuleHandle() == module &&
            !g_systemTrayModuleHooked.exchange(true);

        if (HookTaskbarViewDllSymbols(module, hookSystemTraySymbolsInline)) {
            Wh_ApplyHookOperations();
            EdgeResizeLog(L"diag LoadLibraryExW HookTaskbarViewDllSymbols ok");
        } else {
            EdgeResizeLog(
                L"diag LoadLibraryExW HookTaskbarViewDllSymbols FAILED");
        }
    }

    if (!g_searchUxUiDllLoaded && GetSearchUxUiModuleHandle() == module &&
        !g_searchUxUiDllLoaded.exchange(true)) {
        Wh_Log(L"Loaded %s", lpLibFileName);

        if (HookSearchUxUiDllSymbols(module)) {
            Wh_ApplyHookOperations();
        }
    }

    RunningIndicatorDiagAfterLibraryLoad(L"LoadLibraryExW", lpLibFileName,
                                         module);
    return module;
}

HMODULE WINAPI LoadLibraryW_Hook(LPCWSTR lpLibFileName) {
    HMODULE module = LoadLibraryW_Original(lpLibFileName);
    RunningIndicatorDiagAfterLibraryLoad(L"LoadLibraryW", lpLibFileName,
                                         module);
    return module;
}

HMODULE WINAPI LoadPackagedLibrary_Hook(LPCWSTR lpwLibFileName,
                                        DWORD Reserved) {
    HMODULE module = LoadPackagedLibrary_Original(lpwLibFileName, Reserved);
    RunningIndicatorDiagAfterLibraryLoad(L"LoadPackagedLibrary",
                                         lpwLibFileName, module);
    return module;
}

DWORD WINAPI RunningIndicatorDiagTaskbarViewPollThreadProc(void*) {
    EdgeResizeLog(L"diag poll thread start");

    for (int attempt = 0; attempt < 100 && !g_unloading; attempt++) {
        HMODULE taskbarViewModule = GetTaskbarViewModuleHandle();
        if (taskbarViewModule) {
            EdgeResizeLog(L"diag poll found taskbarView=%p attempt=%d "
                          L"loadedFlag=%d",
                          taskbarViewModule, attempt,
                          g_taskbarViewDllLoaded.load() ? 1 : 0);

            if (!g_taskbarViewDllLoaded.exchange(true)) {
                bool hookSystemTraySymbolsInline =
                    !g_systemTrayModuleHooked &&
                    GetSystemTrayModuleHandle() == taskbarViewModule &&
                    !g_systemTrayModuleHooked.exchange(true);

                if (HookTaskbarViewDllSymbols(taskbarViewModule,
                                              hookSystemTraySymbolsInline)) {
                    g_taskbarViewDllLoaded = true;
                    Wh_ApplyHookOperations();
                    EdgeResizeLog(L"diag poll HookTaskbarViewDllSymbols ok");
                } else {
                    if (hookSystemTraySymbolsInline) {
                        g_systemTrayModuleHooked = false;
                    }
                    EdgeResizeLog(
                        L"diag poll HookTaskbarViewDllSymbols FAILED");
                }
            }

            return 0;
        }

        Sleep(100);
    }

    EdgeResizeLog(L"diag poll thread gave up loadedFlag=%d",
                  g_taskbarViewDllLoaded.load() ? 1 : 0);
    return 0;
}

void RunningIndicatorDiagStartTaskbarViewPollThread() {
    int request = g_runningIndicatorDiagPollThreadRequests.fetch_add(1) + 1;
    if (request > 5) {
        EdgeResizeLog(L"diag skip poll thread request=%d", request);
        return;
    }

    EdgeResizeLog(L"diag create poll thread request=%d", request);
    HANDLE thread = CreateThread(nullptr, 0,
                                 RunningIndicatorDiagTaskbarViewPollThreadProc,
                                 nullptr, 0, nullptr);
    if (thread) {
        EdgeResizeLog(L"diag created poll thread request=%d handle=%p",
                      request, thread);
        CloseHandle(thread);
    } else {
        EdgeResizeLog(L"diag failed to start poll thread request=%d gle=%lu",
                      request, GetLastError());
    }
}

BOOL Wh_ModInit() {
    Wh_Log(L">");

    g_edgeResizeTaskbarCreatedMessage = RegisterWindowMessageW(L"TaskbarCreated");
    EdgeResizeLog(L"mod init diag build taskbarView=%p systemTray=%p search=%p",
                  GetTaskbarViewModuleHandle(), GetSystemTrayModuleHandle(),
                  GetSearchUxUiModuleHandle());

    LoadSettings();

    if (!HookTaskbarDllSymbols()) {
        EdgeResizeLog(L"diag HookTaskbarDllSymbols FAILED");
        return FALSE;
    }
    EdgeResizeLog(L"diag HookTaskbarDllSymbols ok");

    bool delayLoadingNeeded = false;

    if (HMODULE systemTrayModule = GetSystemTrayModuleHandle()) {
        // For older Taskbar.View.dll builds the resolved module is the same
        // Taskbar.View.dll handle - in that case, defer hooking SystemTray
        // symbols until HookTaskbarViewDllSymbols runs below so it can do them
        // in a single HookSymbols batch.
        if (systemTrayModule != GetTaskbarViewModuleHandle()) {
            g_systemTrayModuleHooked = true;
            if (!HookSystemTraySymbols(systemTrayModule)) {
                return FALSE;
            }
        }
    }

    if (HMODULE taskbarViewModule = GetTaskbarViewModuleHandle()) {
        g_taskbarViewDllLoaded = true;
        bool hookSystemTraySymbolsInline =
            !g_systemTrayModuleHooked &&
            GetSystemTrayModuleHandle() == taskbarViewModule;
        if (hookSystemTraySymbolsInline) {
            g_systemTrayModuleHooked = true;
        }
        if (!HookTaskbarViewDllSymbols(taskbarViewModule,
                                       hookSystemTraySymbolsInline)) {
            EdgeResizeLog(L"diag initial HookTaskbarViewDllSymbols FAILED");
            return FALSE;
        }
        EdgeResizeLog(L"diag initial HookTaskbarViewDllSymbols ok");
    } else {
        Wh_Log(L"Taskbar view module not loaded yet");
        EdgeResizeLog(L"diag taskbar view module not loaded yet");
        delayLoadingNeeded = true;
    }

    // SystemTray.dll may load after Taskbar.View.dll on newer Windows 11
    // builds, so make sure the LoadLibraryExW hook is installed to catch it.
    if (!g_systemTrayModuleHooked) {
        delayLoadingNeeded = true;
    }

    if (HMODULE searchUxUiModule = GetSearchUxUiModuleHandle()) {
        g_searchUxUiDllLoaded = true;
        if (!HookSearchUxUiDllSymbols(searchUxUiModule)) {
            return FALSE;
        }
    } else {
        Wh_Log(L"Search UX UI module not loaded yet");
        delayLoadingNeeded = true;
    }

    EdgeResizeLog(L"diag delayLoadingNeeded=%d taskbarViewLoaded=%d "
                  L"systemTrayHooked=%d searchLoaded=%d",
                  delayLoadingNeeded ? 1 : 0,
                  g_taskbarViewDllLoaded.load() ? 1 : 0,
                  g_systemTrayModuleHooked.load() ? 1 : 0,
                  g_searchUxUiDllLoaded.load() ? 1 : 0);

    if (delayLoadingNeeded) {
        HMODULE kernelBaseModule = GetModuleHandle(L"kernelbase.dll");
        EdgeResizeLog(L"diag installing delay hooks kernelbase=%p",
                      kernelBaseModule);
        auto pKernelBaseLoadLibraryExW =
            (decltype(&LoadLibraryExW))GetProcAddress(kernelBaseModule,
                                                      "LoadLibraryExW");
        EdgeResizeLog(L"diag delay hook LoadLibraryExW ptr=%p",
                      pKernelBaseLoadLibraryExW);
        if (pKernelBaseLoadLibraryExW) {
            WindhawkUtils::Wh_SetFunctionHookT(pKernelBaseLoadLibraryExW,
                                               LoadLibraryExW_Hook,
                                               &LoadLibraryExW_Original);
        }
        if (auto pKernelBaseLoadLibraryW =
                (decltype(&LoadLibraryW))GetProcAddress(kernelBaseModule,
                                                        "LoadLibraryW")) {
            EdgeResizeLog(L"diag delay hook LoadLibraryW ptr=%p",
                          pKernelBaseLoadLibraryW);
            WindhawkUtils::Wh_SetFunctionHookT(pKernelBaseLoadLibraryW,
                                               LoadLibraryW_Hook,
                                               &LoadLibraryW_Original);
        } else {
            EdgeResizeLog(L"diag delay hook LoadLibraryW ptr=<null>");
        }
        if (auto pKernelBaseLoadPackagedLibrary =
                (LoadPackagedLibrary_t)GetProcAddress(
                    kernelBaseModule, "LoadPackagedLibrary")) {
            EdgeResizeLog(L"diag delay hook LoadPackagedLibrary ptr=%p",
                          pKernelBaseLoadPackagedLibrary);
            WindhawkUtils::Wh_SetFunctionHookT(
                pKernelBaseLoadPackagedLibrary, LoadPackagedLibrary_Hook,
                &LoadPackagedLibrary_Original);
        } else {
            EdgeResizeLog(L"diag delay hook LoadPackagedLibrary ptr=<null>");
        }
        EdgeResizeLog(L"diag requesting poll thread");
        RunningIndicatorDiagStartTaskbarViewPollThread();
        EdgeResizeLog(L"diag requested poll thread");
    }

    WindhawkUtils::Wh_SetFunctionHookT(SHAppBarMessage, SHAppBarMessage_Hook,
                                       &SHAppBarMessage_Original);

    WindhawkUtils::Wh_SetFunctionHookT(SendMessageTimeoutW,
                                       SendMessageTimeoutW_Hook,
                                       &SendMessageTimeoutW_Original);

    RunningIndicatorDiagEnsureTaskbarViewHooked(L"Wh_ModInit-end");

    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L">");
    EdgeResizeLog(L"mod afterinit start taskbarViewLoaded=%d systemTrayHooked=%d",
                  g_taskbarViewDllLoaded.load() ? 1 : 0,
                  g_systemTrayModuleHooked.load() ? 1 : 0);

    if (!g_systemTrayModuleHooked) {
        if (HMODULE systemTrayModule = GetSystemTrayModuleHandle()) {
            if (systemTrayModule != GetTaskbarViewModuleHandle() &&
                !g_systemTrayModuleHooked.exchange(true)) {
                Wh_Log(L"Got system tray module");

                if (HookSystemTraySymbols(systemTrayModule)) {
                    Wh_ApplyHookOperations();
                }
            }
        }
    }

    if (!g_taskbarViewDllLoaded) {
        if (HMODULE taskbarViewModule = GetTaskbarViewModuleHandle()) {
            if (!g_taskbarViewDllLoaded.exchange(true)) {
                Wh_Log(L"Got Taskbar.View.dll");

                bool hookSystemTraySymbolsInline =
                    !g_systemTrayModuleHooked &&
                    GetSystemTrayModuleHandle() == taskbarViewModule &&
                    !g_systemTrayModuleHooked.exchange(true);

                if (HookTaskbarViewDllSymbols(taskbarViewModule,
                                              hookSystemTraySymbolsInline)) {
                    Wh_ApplyHookOperations();
                }
            }
        }

        if (!g_taskbarViewDllLoaded) {
            RunningIndicatorDiagStartTaskbarViewPollThread();
        }
    }

    if (!g_searchUxUiDllLoaded) {
        if (HMODULE searchUxUiModule = GetSearchUxUiModuleHandle()) {
            if (!g_searchUxUiDllLoaded.exchange(true)) {
                Wh_Log(L"Got SearchUx.UI.dll");

                if (HookSearchUxUiDllSymbols(searchUxUiModule)) {
                    Wh_ApplyHookOperations();
                }
            }
        }
    }

    EdgeResizeInstallSubclass();
    EdgeResizeLog(L"mod afterinit before configured apply height=%d",
                  g_settings.taskbarHeight);
    EdgeResizeApplyConfiguredHeight(L"after-init");
    EdgeResizeLog(L"mod afterinit after configured apply");
    RunningIndicatorDiagEnsureTaskbarViewHooked(L"Wh_ModAfterInit-end");
}

void Wh_ModBeforeUninit() {
    Wh_Log(L">");

    EdgeResizeUninstallSubclass();

    g_unloading = true;

    ApplySettings(g_originalTaskbarHeight ? g_originalTaskbarHeight : 48);
}

void Wh_ModUninit() {
    Wh_Log(L">");

    while (g_hookCallCounter > 0) {
        Sleep(100);
    }
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");
    EdgeResizeLog(L"mod settings changed");
    RunningIndicatorDiagEnsureTaskbarViewHooked(L"Wh_ModSettingsChanged-start");

    LoadSettings();

    EdgeResizeApplyConfiguredHeight(L"settings-changed");

    if (g_settings.edgeResizeEnabled) {
        EdgeResizeInstallSubclass();
    } else {
        EdgeResizeUninstallSubclass();
    }

    RunningIndicatorDiagEnsureTaskbarViewHooked(L"Wh_ModSettingsChanged-end");
}
// END INLINED COMPONENT: taskbar-height-and-resize
}

namespace MultirowComponent {
namespace winrt = ::winrt;
// BEGIN INLINED COMPONENT: taskbar-multirow
// Source code is published under The GNU General Public License v3.0.
//
// For bug reports and feature requests, please open an issue here:
// https://github.com/ramensoftware/windhawk-mods/issues
//
// For pull requests, development takes place here:
// https://github.com/m417z/my-windhawk-mods



#include <windhawk_utils.h>

#include <atomic>
#include <cstdarg>
#include <functional>
#include <limits>
#include <string>
#include <unordered_map>
#include <vector>

#include <windowsx.h>

#undef GetCurrentTime

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.Xaml.Automation.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.Shapes.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/base.h>

using namespace winrt::Windows::UI::Xaml;

enum class SearchBoxVerticalPlacement {
    Center,
    TopForMultirow,
};

enum class MultirowSearchBoxPolicy {
    Keep,
    CompactButton,
};

struct {
    int rows;
    bool fullHeightStartButton;
    bool compactLeadingButtonHover;
    bool centerSearchControl;
    SearchBoxVerticalPlacement searchBoxVerticalPlacement;
    MultirowSearchBoxPolicy multirowSearchBoxPolicy;
    bool debugDisableArrangeHook;
    bool debugDisableSearchArrange;
    bool debugDisableTaskListItemArrange;
} g_settings;

std::atomic<bool> g_taskbarViewDllLoaded;
std::atomic<bool> g_unloading;
std::atomic<int> g_liveRowsHint;
std::atomic<DWORD> g_liveRowsHintTick;
std::atomic<int> g_liveTaskbarHeightHint;
std::atomic<DWORD> g_liveTaskbarHeightHintTick;
std::atomic<int> g_taskbarRowsRefreshRequest;
std::atomic<bool> g_taskbarRowsRefreshRunning;
std::atomic<int> g_multirowDiagLineCount;
std::atomic<int> g_searchArrangeDiagLineCount;

thread_local bool g_inTaskbarCollapsibleLayoutXamlTraits_ArrangeOverride;
double g_lastTaskViewButtonWidth;

struct TaskbarState {
    winrt::weak_ref<XamlRoot> xamlRoot;
    std::vector<float> rowOffsetAdjustment;
    int lastRows = 0;
    double lastWidthWithoutExtent = 0;
    double lastLeadingFullHeightControlsWidth = 0;
};

std::unordered_map<void*, TaskbarState> g_taskbarState;

void RequestTaskbarRowsRefresh();
void ApplyTaskbarRowsRefreshPass(PCWSTR reason);
void ApplySettings(HWND hTaskbarWnd);

#ifndef EDGE_RESIZE_FILE_DIAG
#define EDGE_RESIZE_FILE_DIAG 0
#endif

bool BuildMultirowDiagPath(WCHAR* path, DWORD pathCount) {
    if (!path || pathCount == 0) {
        return false;
    }

    DWORD len = GetTempPathW(pathCount, path);
    if (len == 0 || len >= pathCount) {
        return false;
    }

    if (wcscat_s(path, pathCount,
                 L"draggable-multirow-taskbar-systray-windows-11") != 0) {
        return false;
    }

    CreateDirectoryW(path, nullptr);

    return wcscat_s(path, pathCount, L"\\multirow-runtime-diag.txt") == 0;
}

void MultirowDiagLog(PCWSTR format, ...) {
#if EDGE_RESIZE_FILE_DIAG
    int lineIndex = g_multirowDiagLineCount.fetch_add(1);
    if (lineIndex > 3000) {
        return;
    }

    WCHAR message[1536];
    va_list args;
    va_start(args, format);
    _vsnwprintf_s(message, ARRAYSIZE(message), _TRUNCATE, format, args);
    va_end(args);

    SYSTEMTIME st{};
    GetLocalTime(&st);

    WCHAR line[1792];
    swprintf_s(line, L"%02u:%02u:%02u.%03u %s\r\n", st.wHour, st.wMinute,
               st.wSecond, st.wMilliseconds, message);

    WCHAR path[MAX_PATH];
    if (!BuildMultirowDiagPath(path, ARRAYSIZE(path))) {
        return;
    }

    HANDLE file = CreateFileW(path, FILE_APPEND_DATA,
                              FILE_SHARE_READ | FILE_SHARE_WRITE |
                                  FILE_SHARE_DELETE,
                              nullptr, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL,
                              nullptr);
    if (file == INVALID_HANDLE_VALUE) {
        return;
    }

    int bytes = WideCharToMultiByte(CP_UTF8, 0, line, -1, nullptr, 0, nullptr,
                                    nullptr);
    if (bytes > 1) {
        std::string utf8(bytes - 1, '\0');
        WideCharToMultiByte(CP_UTF8, 0, line, -1, utf8.data(), bytes, nullptr,
                            nullptr);
        DWORD written = 0;
        WriteFile(file, utf8.data(), bytes - 1, &written, nullptr);
    }

    CloseHandle(file);
#else
    UNREFERENCED_PARAMETER(format);
#endif
}

#ifndef WIN11_TASKBAR_WIN10_MULTIROW_COMBINED
WINUSERAPI UINT WINAPI GetDpiForWindow(HWND hwnd);
#endif

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

int GetFreshLiveTaskbarHeightHint() {
    int hintedHeight = g_liveTaskbarHeightHint.load();
    DWORD hintTick = g_liveTaskbarHeightHintTick.load();
    if (hintedHeight > 0 && GetTickCount() - hintTick < 5000) {
        if (hintedHeight < 2) {
            hintedHeight = 2;
        } else if (hintedHeight > 480) {
            hintedHeight = 480;
        }

        return hintedHeight;
    }

    return 0;
}

double GetTargetTaskbarLogicalHeightForRows(int rows) {
    int hintedHeight = GetFreshLiveTaskbarHeightHint();
    if (hintedHeight > 0) {
        return static_cast<double>(hintedHeight);
    }

    if (rows < 1) {
        rows = 1;
    } else if (rows > 10) {
        rows = 10;
    }

    return rows * 48.0;
}

double GetCurrentTaskbarLogicalHeight() {
    int hintedHeight = GetFreshLiveTaskbarHeightHint();
    if (hintedHeight > 0) {
        return static_cast<double>(hintedHeight);
    }

    HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();
    if (!hTaskbarWnd) {
        return 0;
    }

    RECT rect{};
    if (!GetWindowRect(hTaskbarWnd, &rect)) {
        return 0;
    }

    int dpi = GetDpiForWindow(hTaskbarWnd);
    return static_cast<double>(
        MulDiv(rect.bottom - rect.top, 96, dpi ? dpi : 96));
}

int GetEffectiveRows() {
    if (g_unloading) {
        return 1;
    }

    int hintedRows = g_liveRowsHint.load();
    DWORD hintTick = g_liveRowsHintTick.load();
    if (hintedRows > 0 && GetTickCount() - hintTick < 5000) {
        if (hintedRows < 1) {
            hintedRows = 1;
        } else if (hintedRows > 10) {
            hintedRows = 10;
        }

        return hintedRows;
    }

    int rows = g_settings.rows;
    if (rows < 1) {
        rows = 1;
    } else if (rows > 10) {
        rows = 10;
    }

    return rows;
}

double AbsDiff(double a, double b) {
    return a > b ? a - b : b - a;
}

bool IsHeightShrinkPending(FrameworkElement element, double targetHeight) {
    if (!element || !(targetHeight > 0)) {
        return false;
    }

    try {
        double actualHeight = element.ActualHeight();
        return actualHeight > targetHeight + 0.5;
    } catch (...) {
        return false;
    }
}

void InvalidateElementLayoutOnly(FrameworkElement element, PCWSTR reason) {
    if (!element) {
        return;
    }

    try {
        element.InvalidateMeasure();
        element.InvalidateArrange();
        MultirowDiagLog(
            L"[layout-invalidate-only] reason=%s class=%s name=%s "
            L"actual=%.1fx%.1f height=%.1f min=%.1f",
            reason ? reason : L"<unknown>",
            winrt::get_class_name(element).c_str(), element.Name().c_str(),
            element.ActualWidth(), element.ActualHeight(), element.Height(),
            element.MinHeight());
    } catch (...) {
        MultirowDiagLog(L"[layout-invalidate-only] failed reason=%s",
                        reason ? reason : L"<unknown>");
    }
}

void ForceElementLayout(FrameworkElement element, PCWSTR reason) {
    for (int depth = 0; depth < 8 && element; depth++) {
        try {
            double actualHeight = element.ActualHeight();
            double height = element.Height();
            double minHeight = element.MinHeight();
            if (height > 0 && minHeight > 0 && height + 0.5 < minHeight) {
                MultirowDiagLog(
                    L"[force-layout-skip] reason=%s depth=%d class=%s name=%s "
                    L"actual=%.1fx%.1f height=%.1f min=%.1f",
                    reason ? reason : L"<unknown>", depth,
                    winrt::get_class_name(element).c_str(),
                    element.Name().c_str(), element.ActualWidth(),
                    element.ActualHeight(), height, minHeight);
                return;
            }

            if (height > 0 && actualHeight > height + 0.5) {
                MultirowDiagLog(
                    L"[force-layout-defer-shrink] reason=%s depth=%d class=%s "
                    L"name=%s actual=%.1fx%.1f height=%.1f min=%.1f",
                    reason ? reason : L"<unknown>", depth,
                    winrt::get_class_name(element).c_str(),
                    element.Name().c_str(), element.ActualWidth(),
                    actualHeight, height, minHeight);
                element.InvalidateMeasure();
                element.InvalidateArrange();
                return;
            }

            element.InvalidateMeasure();
            element.InvalidateArrange();
            MultirowDiagLog(
                L"[force-layout] reason=%s depth=%d class=%s name=%s "
                L"actual=%.1fx%.1f height=%.1f min=%.1f",
                reason ? reason : L"<unknown>", depth,
                winrt::get_class_name(element).c_str(), element.Name().c_str(),
                element.ActualWidth(), element.ActualHeight(), element.Height(),
                element.MinHeight());
        } catch (...) {
            MultirowDiagLog(L"[force-layout] failed reason=%s depth=%d",
                            reason ? reason : L"<unknown>", depth);
            return;
        }

        element = Media::VisualTreeHelper::GetParent(element)
                      .try_as<FrameworkElement>();
    }
}

void ApplyCurrentHeightBounds(FrameworkElement element,
                              double taskbarHeight,
                              PCWSTR reason) {
    if (!element || !(taskbarHeight > 0)) {
        return;
    }

    try {
        MultirowDiagLog(
            L"[height-bounds-before] reason=%s class=%s name=%s actual=%.1fx%.1f "
            L"height=%.1f min=%.1f max=%.1f target=%.1f",
            reason ? reason : L"<unknown>",
            winrt::get_class_name(element).c_str(), element.Name().c_str(),
            element.ActualWidth(), element.ActualHeight(), element.Height(),
            element.MinHeight(), element.MaxHeight(), taskbarHeight);
        element.Height(taskbarHeight);
        element.MinHeight(taskbarHeight);
        element.MaxHeight(std::numeric_limits<double>::infinity());
        element.VerticalAlignment(VerticalAlignment::Stretch);
        MultirowDiagLog(
            L"[height-bounds-after] reason=%s class=%s name=%s actual=%.1fx%.1f "
            L"height=%.1f min=%.1f max=%.1f",
            reason ? reason : L"<unknown>",
            winrt::get_class_name(element).c_str(), element.Name().c_str(),
            element.ActualWidth(), element.ActualHeight(), element.Height(),
            element.MinHeight(), element.MaxHeight());
    } catch (...) {
        MultirowDiagLog(L"[height-bounds] failed reason=%s",
                        reason ? reason : L"<unknown>");
    }
}

void SetLiveRowsHint(int rows) {
    if (rows < 1) {
        rows = 1;
    } else if (rows > 10) {
        rows = 10;
    }

    int oldRows = g_liveRowsHint.exchange(rows);
    g_liveRowsHintTick = GetTickCount();
    g_settings.rows = rows;
    Wh_Log(L"Live taskbar rows hint=%d old=%d", rows, oldRows);
    RequestTaskbarRowsRefresh();
}

void SetLiveTaskbarHeightHintNoRefresh(int height) {
    if (height < 2) {
        height = 2;
    } else if (height > 480) {
        height = 480;
    }

    int oldHeight = g_liveTaskbarHeightHint.exchange(height);
    g_liveTaskbarHeightHintTick = GetTickCount();
    Wh_Log(L"Live taskbar height hint=%d old=%d", height, oldHeight);
    MultirowDiagLog(L"[height-hint] height=%d old=%d", height, oldHeight);
}

void SetLiveTaskbarHeightHint(int height) {
    SetLiveTaskbarHeightHintNoRefresh(height);
    RequestTaskbarRowsRefresh();
}

void SetLiveLayoutHints(int rows, int height) {
    if (rows < 1) {
        rows = 1;
    } else if (rows > 10) {
        rows = 10;
    }

    if (height < 2) {
        height = 2;
    } else if (height > 480) {
        height = 480;
    }

    int oldRows = g_liveRowsHint.exchange(rows);
    g_liveRowsHintTick = GetTickCount();
    g_settings.rows = rows;
    int oldHeight = g_liveTaskbarHeightHint.exchange(height);
    g_liveTaskbarHeightHintTick = GetTickCount();

    Wh_Log(L"Live taskbar layout hint rows=%d oldRows=%d height=%d oldHeight=%d",
           rows, oldRows, height, oldHeight);
    MultirowDiagLog(L"[live-layout] rows=%d oldRows=%d height=%d oldHeight=%d",
                    rows, oldRows, height, oldHeight);
    ApplyTaskbarRowsRefreshPass(L"live-layout-immediate");
    RequestTaskbarRowsRefresh();
}

void PrepareNativeUnload(int nativeHeight) {
    if (nativeHeight < 2) {
        nativeHeight = 48;
    } else if (nativeHeight > 480) {
        nativeHeight = 480;
    }

    int oldRows = g_liveRowsHint.exchange(1);
    g_liveRowsHintTick = GetTickCount();
    int oldHeight = g_liveTaskbarHeightHint.exchange(nativeHeight);
    g_liveTaskbarHeightHintTick = GetTickCount();

    Wh_Log(L"Prepare native unload rows=1 oldRows=%d height=%d oldHeight=%d",
           oldRows, nativeHeight, oldHeight);
    MultirowDiagLog(L"[native-unload] rows=1 oldRows=%d height=%d oldHeight=%d",
                    oldRows, nativeHeight, oldHeight);

    HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();
    if (hTaskbarWnd) {
        ApplySettings(hTaskbarWnd);
    }
}

double GetElementEffectiveWidth(FrameworkElement element) {
    double width = element.ActualWidth();
    if (!(width > 0)) {
        width = element.Width();
    }

    return width > 0 ? width : 0;
}

double GetTaskbarWidthWithoutExtent(FrameworkElement taskbarFrameElement,
                                    double systemTrayFrameWidth) {
    double taskbarFrameWidth = GetElementEffectiveWidth(taskbarFrameElement);
    if (!(taskbarFrameWidth > 0)) {
        return 0;
    }

    double widthWithoutExtent = taskbarFrameWidth - systemTrayFrameWidth;
    return widthWithoutExtent > 0 ? widthWithoutExtent : taskbarFrameWidth;
}

double GetTaskbarWidthWithoutExtent(FrameworkElement taskbarFrameElement,
                                    FrameworkElement systemTrayFrame) {
    return GetTaskbarWidthWithoutExtent(
        taskbarFrameElement, GetElementEffectiveWidth(systemTrayFrame));
}

FrameworkElement EnumChildElements(
    FrameworkElement element,
    std::function<bool(FrameworkElement)> enumCallback) {
    int childrenCount = Media::VisualTreeHelper::GetChildrenCount(element);

    for (int i = 0; i < childrenCount; i++) {
        auto child = Media::VisualTreeHelper::GetChild(element, i)
                         .try_as<FrameworkElement>();
        if (!child) {
            Wh_Log(L"Failed to get child %d of %d", i + 1, childrenCount);
            continue;
        }

        if (enumCallback(child)) {
            return child;
        }
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

FrameworkElement FindDescendantByName(FrameworkElement element, PCWSTR name) {
    FrameworkElement result = nullptr;
    EnumChildElements(element, [&](FrameworkElement child) {
        if (child.Name() == name) {
            result = child;
            return true;
        }

        result = FindDescendantByName(child, name);
        return !!result;
    });

    return result;
}

FrameworkElement FindDescendantByClassName(FrameworkElement element,
                                           PCWSTR className) {
    FrameworkElement result = nullptr;
    EnumChildElements(element, [&](FrameworkElement child) {
        if (winrt::get_class_name(child) == className) {
            result = child;
            return true;
        }

        result = FindDescendantByClassName(child, className);
        return !!result;
    });

    return result;
}

bool IsDirectChildOf(FrameworkElement child, FrameworkElement parent) {
    auto childParent =
        Media::VisualTreeHelper::GetParent(child).try_as<FrameworkElement>();
    return childParent && winrt::get_abi(childParent) == winrt::get_abi(parent);
}

bool IsLeadingFullHeightControl(FrameworkElement element) {
    auto automationId = Automation::AutomationProperties::GetAutomationId(
        element);
    if (automationId == L"StartButton" || automationId == L"SearchButton" ||
        automationId == L"TaskViewButton") {
        return true;
    }

    auto automationName = Automation::AutomationProperties::GetName(element);
    if (automationName == L"Start" || automationName == L"Search" ||
        automationName == L"Task View") {
        return true;
    }

    auto className = winrt::get_class_name(element);
    if (className == L"Taskbar.SearchBoxButton" ||
        className == L"SearchUx.SearchUI.SearchBoxButton" ||
        className == L"SearchUx.SearchUI.SearchIconButton" ||
        className == L"SearchUx.SearchUI.SearchPillButton" ||
        className == L"SearchUx.SearchUI.SearchButtonRootGrid") {
        return true;
    }

    if (FindDescendantByName(element, L"SearchBoxButtonRootPanel") ||
        FindDescendantByName(element, L"SearchBoxContentGrid") ||
        FindDescendantByName(element, L"SearchBoxFontIcon") ||
        FindDescendantByName(element, L"SearchBoxTextBlock")) {
        return true;
    }

    if (className != L"Taskbar.ExperienceToggleButton") {
        return false;
    }

    return false;
}

bool IsTaskViewControl(FrameworkElement element) {
    auto automationId = Automation::AutomationProperties::GetAutomationId(
        element);
    if (automationId == L"TaskViewButton") {
        return true;
    }

    auto automationName = Automation::AutomationProperties::GetName(element);
    return automationName == L"Task View";
}

bool IsSearchControl(FrameworkElement element) {
    auto automationId = Automation::AutomationProperties::GetAutomationId(
        element);
    if (automationId == L"SearchButton") {
        return true;
    }

    auto automationName = Automation::AutomationProperties::GetName(element);
    if (automationName == L"Search") {
        return true;
    }

    auto className = winrt::get_class_name(element);
    return className == L"Taskbar.SearchBoxButton" ||
           className == L"SearchUx.SearchUI.SearchBoxButton" ||
           className == L"SearchUx.SearchUI.SearchIconButton" ||
           className == L"SearchUx.SearchUI.SearchPillButton" ||
           className == L"SearchUx.SearchUI.SearchButtonRootGrid" ||
           FindDescendantByName(element, L"SearchBoxButtonRootPanel") ||
           FindDescendantByName(element, L"SearchBoxContentGrid") ||
           FindDescendantByName(element, L"SearchBoxFontIcon") ||
           FindDescendantByName(element, L"SearchBoxTextBlock");
}

bool HasSearchBoxTextBlock(FrameworkElement element) {
    if (element.Name() == L"SearchBoxTextBlock") {
        return true;
    }

    return !!FindDescendantByName(element, L"SearchBoxTextBlock");
}

bool IsFullSearchBoxControl(FrameworkElement element, double arrangeWidth) {
    if (!HasSearchBoxTextBlock(element)) {
        return false;
    }

    double width = arrangeWidth;
    if (!(width > 0)) {
        width = element.ActualWidth();
    }

    return width >= 140.0;
}

void LogSearchArrangeDecision(FrameworkElement element,
                              winrt::Windows::Foundation::Rect originalRect,
                              winrt::Windows::Foundation::Rect arrangedRect,
                              int rows,
                              double frameHeight,
                              bool isFullSearchBox,
                              PCWSTR action) {
    int lineIndex = g_searchArrangeDiagLineCount.fetch_add(1);
    if (lineIndex >= 120) {
        return;
    }

    auto automationId = Automation::AutomationProperties::GetAutomationId(
        element);
    auto automationName = Automation::AutomationProperties::GetName(element);

    MultirowDiagLog(
        L"[search-arrange] action=%s class=%s name=%s automationId=%s "
        L"automationName=%s hasSearchBoxTextBlock=%d isFullSearchBox=%d "
        L"rows=%d frameHeight=%.1f actual=%.1fx%.1f original=%.1f,%.1f "
        L"%.1fx%.1f arranged=%.1f,%.1f %.1fx%.1f",
        action, winrt::get_class_name(element).c_str(), element.Name().c_str(),
        automationId.c_str(), automationName.c_str(),
        HasSearchBoxTextBlock(element) ? 1 : 0, isFullSearchBox ? 1 : 0, rows,
        frameHeight, element.ActualWidth(), element.ActualHeight(),
        originalRect.X, originalRect.Y, originalRect.Width, originalRect.Height,
        arrangedRect.X, arrangedRect.Y, arrangedRect.Width,
        arrangedRect.Height);
}

double GetLeadingFullHeightControlsWidth(FrameworkElement taskbarFrameRepeater) {
    double width = 0;

    EnumChildElements(taskbarFrameRepeater, [&](FrameworkElement child) {
        if (IsLeadingFullHeightControl(child)) {
            double childWidth = child.ActualWidth();
            if (childWidth > 0) {
                width += childWidth;
            }
        }

        return false;
    });

    return width;
}

TaskbarState* GetTaskbarState(XamlRoot xamlRoot) {
    void* xamlRootAbi = winrt::get_abi(xamlRoot);

    auto [it, inserted] = g_taskbarState.insert(
        {xamlRootAbi, TaskbarState{
                          .xamlRoot = winrt::make_weak(xamlRoot),
                      }});

    if (!inserted && !it->second.xamlRoot.get()) {
        it->second = TaskbarState{
            .xamlRoot = winrt::make_weak(xamlRoot),
        };
    }

    // Update size in case it was just created as an empty vector or in case the
    // settings changed.
    it->second.rowOffsetAdjustment.resize(GetEffectiveRows());

    return &it->second;
}

XamlRoot GetCachedTaskbarXamlRoot() {
    for (auto it = g_taskbarState.begin(); it != g_taskbarState.end();) {
        if (auto xamlRoot = it->second.xamlRoot.get()) {
            return xamlRoot;
        }

        it = g_taskbarState.erase(it);
    }

    return nullptr;
}

void UpdateTaskbarStateGeometry(TaskbarState* taskbarState,
                                int rows,
                                double widthWithoutExtent,
                                double leadingFullHeightControlsWidth) {
    if (!taskbarState) {
        return;
    }

    bool resetOffsets =
        taskbarState->lastRows != rows ||
        taskbarState->rowOffsetAdjustment.size() != static_cast<size_t>(rows) ||
        AbsDiff(taskbarState->lastWidthWithoutExtent, widthWithoutExtent) >
            0.5 ||
        AbsDiff(taskbarState->lastLeadingFullHeightControlsWidth,
                leadingFullHeightControlsWidth) > 0.5;

    if (resetOffsets) {
        taskbarState->rowOffsetAdjustment.assign(rows, 0);
        taskbarState->lastRows = rows;
        taskbarState->lastWidthWithoutExtent = widthWithoutExtent;
        taskbarState->lastLeadingFullHeightControlsWidth =
            leadingFullHeightControlsWidth;
    }
}

void UpdateTaskbarFrameRepeaterMargin(FrameworkElement taskbarFrameRepeater,
                                      TaskbarState* taskbarState,
                                      double widthWithoutExtent,
                                      bool forceUpdate = false) {
    double desiredMargin = 0;

    if (!g_unloading) {
        int rows = GetEffectiveRows();
        desiredMargin = -widthWithoutExtent * (rows - 1);

        for (const auto f : taskbarState->rowOffsetAdjustment) {
            desiredMargin += f;
        }

        if (desiredMargin > 0) {
            desiredMargin = 0;
        }
    }

    auto margin = taskbarFrameRepeater.Margin();
    if (forceUpdate) {
        Wh_Log(L"Re-setting margin.Right=%f (widthWithoutExtent=%f)",
               desiredMargin, widthWithoutExtent);
        margin.Right = desiredMargin + 1;
        taskbarFrameRepeater.Margin(margin);
        margin.Right = desiredMargin;
        taskbarFrameRepeater.Margin(margin);
    } else if (margin.Right != desiredMargin) {
        Wh_Log(L"Setting margin.Right=%f (widthWithoutExtent=%f)",
               desiredMargin, widthWithoutExtent);
        margin.Right = desiredMargin;
        taskbarFrameRepeater.Margin(margin);
    }
}

bool ApplyStyle(XamlRoot xamlRoot) {
    TaskbarState* taskbarState = GetTaskbarState(xamlRoot);

    auto xamlRootContent = xamlRoot.Content().as<FrameworkElement>();

    FrameworkElement taskbarFrameRepeater = nullptr;

    FrameworkElement child = xamlRootContent;
    if (child &&
        (child = FindChildByClassName(child, L"Taskbar.TaskbarFrame")) &&
        (child = FindChildByName(child, L"RootGrid")) &&
        (child = FindChildByName(child, L"TaskbarFrameRepeater"))) {
        taskbarFrameRepeater = child;
    }

    if (!taskbarFrameRepeater) {
        return false;
    }

    auto taskbarFrameElement =
        FindChildByName(xamlRootContent, L"TaskbarFrame");
    if (!taskbarFrameElement) {
        return false;
    }

    auto systemTrayFrame =
        FindChildByClassName(xamlRootContent, L"SystemTray.SystemTrayFrame");
    if (!systemTrayFrame) {
        return false;
    }

    int rows = GetEffectiveRows();
    double taskbarHeight = GetTargetTaskbarLogicalHeightForRows(rows);
    if (!(taskbarHeight > 0)) {
        taskbarHeight = rows * 48.0;
    }

    auto taskbarRootGrid = FindChildByName(taskbarFrameElement, L"RootGrid");
    auto taskbarBackground = FindDescendantByClassName(
        taskbarFrameElement, L"Taskbar.TaskbarBackground");
    auto backgroundControl =
        FindDescendantByName(taskbarFrameElement, L"BackgroundControl");
    auto backgroundFill =
        FindDescendantByName(taskbarFrameElement, L"BackgroundFill");

    MultirowDiagLog(
        L"[apply-style] rows=%d taskbarHeight=%.1f frameActual=%.1fx%.1f "
        L"repeaterActual=%.1fx%.1f rootGrid=%d background=%d fill=%d",
        rows, taskbarHeight, taskbarFrameElement.ActualWidth(),
        taskbarFrameElement.ActualHeight(), taskbarFrameRepeater.ActualWidth(),
        taskbarFrameRepeater.ActualHeight(), taskbarRootGrid ? 1 : 0,
        taskbarBackground ? 1 : 0, backgroundFill ? 1 : 0);

    ApplyCurrentHeightBounds(taskbarFrameElement, taskbarHeight,
                             L"taskbar-frame");
    ApplyCurrentHeightBounds(taskbarRootGrid, taskbarHeight,
                             L"taskbar-root-grid");
    ApplyCurrentHeightBounds(taskbarFrameRepeater, taskbarHeight,
                             L"taskbar-frame-repeater");
    ApplyCurrentHeightBounds(taskbarBackground, taskbarHeight,
                             L"taskbar-background");
    ApplyCurrentHeightBounds(backgroundControl, taskbarHeight,
                             L"background-control");
    ApplyCurrentHeightBounds(backgroundFill, taskbarHeight,
                             L"background-fill");

    double widthWithoutExtent =
        GetTaskbarWidthWithoutExtent(taskbarFrameElement, systemTrayFrame);

    UpdateTaskbarFrameRepeaterMargin(taskbarFrameRepeater, taskbarState,
                                     widthWithoutExtent, /*forceUpdate=*/true);
    if (IsHeightShrinkPending(taskbarFrameRepeater, taskbarHeight) ||
        IsHeightShrinkPending(taskbarRootGrid, taskbarHeight) ||
        IsHeightShrinkPending(taskbarFrameElement, taskbarHeight)) {
        MultirowDiagLog(
            L"[apply-style-defer-force-layout] rows=%d taskbarHeight=%.1f "
            L"frameActual=%.1f rootActual=%.1f repeaterActual=%.1f",
            rows, taskbarHeight, taskbarFrameElement.ActualHeight(),
            taskbarRootGrid ? taskbarRootGrid.ActualHeight() : 0,
            taskbarFrameRepeater.ActualHeight());
        InvalidateElementLayoutOnly(taskbarFrameRepeater,
                                    L"defer-shrink-repeater");
        InvalidateElementLayoutOnly(taskbarFrameElement, L"defer-shrink-frame");
        InvalidateElementLayoutOnly(xamlRootContent, L"defer-shrink-root");
        return true;
    }

    ForceElementLayout(taskbarFrameRepeater, L"apply-style-repeater");
    ForceElementLayout(taskbarFrameElement, L"apply-style-frame");
    ForceElementLayout(xamlRootContent, L"apply-style-root");

    return true;
}

void* CTaskBand_ITaskListWndSite_vftable;

void* CSecondaryTaskBand_ITaskListWndSite_vftable;

using CTaskBand_GetTaskbarHost_t = void*(WINAPI*)(void* pThis, void** result);
CTaskBand_GetTaskbarHost_t CTaskBand_GetTaskbarHost_Original;

void* TaskbarHost_FrameHeight_Original;

using CSecondaryTaskBand_GetTaskbarHost_t = void*(WINAPI*)(void* pThis,
                                                           void** result);
CSecondaryTaskBand_GetTaskbarHost_t CSecondaryTaskBand_GetTaskbarHost_Original;

using std__Ref_count_base__Decref_t = void(WINAPI*)(void* pThis);
std__Ref_count_base__Decref_t std__Ref_count_base__Decref_Original;

XamlRoot XamlRootFromTaskbarHostSharedPtr(void* taskbarHostSharedPtr[2]) {
    if (!taskbarHostSharedPtr[0] && !taskbarHostSharedPtr[1]) {
        return nullptr;
    }

    size_t taskbarElementIUnknownOffset = 0x48;

#if defined(_M_X64)
    {
        // 48:83EC 28 | sub rsp,28
        // 48:83C1 48 | add rcx,48
        const BYTE* b = (const BYTE*)TaskbarHost_FrameHeight_Original;
        if (b[0] == 0x48 && b[1] == 0x83 && b[2] == 0xEC && b[4] == 0x48 &&
            b[5] == 0x83 && b[6] == 0xC1 && b[7] <= 0x7F) {
            taskbarElementIUnknownOffset = b[7];
        } else {
            Wh_Log(L"Unsupported TaskbarHost::FrameHeight");
        }
    }
#elif defined(_M_ARM64)
    // Just use the default offset which will hopefully work in most cases.
#else
#error "Unsupported architecture"
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

XamlRoot GetTaskbarXamlRoot(HWND hTaskbarWnd) {
    HWND hTaskSwWnd = (HWND)GetProp(hTaskbarWnd, L"TaskbandHWND");
    if (!hTaskSwWnd) {
        return nullptr;
    }

    void* taskBand = (void*)GetWindowLongPtr(hTaskSwWnd, 0);
    void* taskBandForTaskListWndSite = taskBand;
    for (int i = 0; *(void**)taskBandForTaskListWndSite !=
                    CTaskBand_ITaskListWndSite_vftable;
         i++) {
        if (i == 20) {
            return nullptr;
        }

        taskBandForTaskListWndSite = (void**)taskBandForTaskListWndSite + 1;
    }

    void* taskbarHostSharedPtr[2]{};
    CTaskBand_GetTaskbarHost_Original(taskBandForTaskListWndSite,
                                      taskbarHostSharedPtr);

    return XamlRootFromTaskbarHostSharedPtr(taskbarHostSharedPtr);
}

XamlRoot GetSecondaryTaskbarXamlRoot(HWND hSecondaryTaskbarWnd) {
    HWND hTaskSwWnd =
        (HWND)FindWindowEx(hSecondaryTaskbarWnd, nullptr, L"WorkerW", nullptr);
    if (!hTaskSwWnd) {
        return nullptr;
    }

    void* taskBand = (void*)GetWindowLongPtr(hTaskSwWnd, 0);
    void* taskBandForTaskListWndSite = taskBand;
    for (int i = 0; *(void**)taskBandForTaskListWndSite !=
                    CSecondaryTaskBand_ITaskListWndSite_vftable;
         i++) {
        if (i == 20) {
            return nullptr;
        }

        taskBandForTaskListWndSite = (void**)taskBandForTaskListWndSite + 1;
    }

    void* taskbarHostSharedPtr[2]{};
    CSecondaryTaskBand_GetTaskbarHost_Original(taskBandForTaskListWndSite,
                                               taskbarHostSharedPtr);

    return XamlRootFromTaskbarHostSharedPtr(taskbarHostSharedPtr);
}

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
    if (dwThreadId == 0) {
        return false;
    }

    if (dwThreadId == GetCurrentThreadId()) {
        proc(procParam);
        return true;
    }

    HHOOK hook = SetWindowsHookEx(
        WH_CALLWNDPROC,
        [](int nCode, WPARAM wParam, LPARAM lParam) -> LRESULT {
            if (nCode == HC_ACTION) {
                const CWPSTRUCT* cwp = (const CWPSTRUCT*)lParam;
                if (cwp->message == runFromWindowThreadRegisteredMsg) {
                    RUN_FROM_WINDOW_THREAD_PARAM* param =
                        (RUN_FROM_WINDOW_THREAD_PARAM*)cwp->lParam;
                    param->proc(param->procParam);
                }
            }

            return CallNextHookEx(nullptr, nCode, wParam, lParam);
        },
        nullptr, dwThreadId);
    if (!hook) {
        return false;
    }

    RUN_FROM_WINDOW_THREAD_PARAM param;
    param.proc = proc;
    param.procParam = procParam;
    SendMessage(hWnd, runFromWindowThreadRegisteredMsg, 0, (LPARAM)&param);

    UnhookWindowsHookEx(hook);

    return true;
}

void ApplySettingsFromTaskbarThread() {
    Wh_Log(L"Applying settings");

    EnumThreadWindows(
        GetCurrentThreadId(),
        [](HWND hWnd, LPARAM lParam) -> BOOL {
            WCHAR szClassName[32];
            if (GetClassName(hWnd, szClassName, ARRAYSIZE(szClassName)) == 0) {
                return TRUE;
            }

            XamlRoot xamlRoot = nullptr;
            if (_wcsicmp(szClassName, L"Shell_TrayWnd") == 0) {
                xamlRoot = GetTaskbarXamlRoot(hWnd);
            } else if (_wcsicmp(szClassName, L"Shell_SecondaryTrayWnd") == 0) {
                xamlRoot = GetSecondaryTaskbarXamlRoot(hWnd);
            } else {
                return TRUE;
            }

            if (!xamlRoot) {
                Wh_Log(L"Getting XamlRoot failed");
                return TRUE;
            }

            if (!ApplyStyle(xamlRoot)) {
                Wh_Log(L"ApplyStyles failed");
                return TRUE;
            }

            return TRUE;
        },
        0);

    // Touch a registry value to trigger a watcher for the settings.
    constexpr WCHAR kTempValueName[] = L"_temp_windhawk_" WH_MOD_ID;
    HKEY hSubKey;
    LONG result = RegOpenKeyEx(
        HKEY_CURRENT_USER,
        LR"(SOFTWARE\Microsoft\Windows\CurrentVersion\Explorer\Advanced)", 0,
        KEY_WRITE, &hSubKey);
    if (result == ERROR_SUCCESS) {
        if (RegSetValueEx(hSubKey, kTempValueName, 0, REG_SZ, (const BYTE*)L"",
                          sizeof(WCHAR)) != ERROR_SUCCESS) {
            Wh_Log(L"Failed to create temp value");
        } else if (RegDeleteValue(hSubKey, kTempValueName) != ERROR_SUCCESS) {
            Wh_Log(L"Failed to remove temp value");
        }

        RegCloseKey(hSubKey);
    } else {
        Wh_Log(L"Failed to open subkey: %d", result);
    }
}

void ApplySettings(HWND hTaskbarWnd) {
    RunFromWindowThread(
        hTaskbarWnd,
        [](void* pParam) -> void { ApplySettingsFromTaskbarThread(); }, 0);
}

void ApplyTaskbarRowsRefreshPass(PCWSTR reason) {
    if (g_unloading) {
        return;
    }

    HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();
    Wh_Log(L"Taskbar rows refresh pass reason=%s rows=%d hwnd=%08X",
           reason ? reason : L"<unknown>", GetEffectiveRows(),
           (DWORD)(DWORD_PTR)hTaskbarWnd);
    if (hTaskbarWnd) {
        ApplySettings(hTaskbarWnd);
    }
}

DWORD WINAPI TaskbarRowsRefreshThreadProc(void*) {
    int handledRequest = 0;

    while (!g_unloading) {
        handledRequest = g_taskbarRowsRefreshRequest.load();

        constexpr DWORD kDelaysMs[] = {0, 16, 33, 50, 100, 150, 250};
        for (DWORD delayMs : kDelaysMs) {
            if (g_unloading) {
                break;
            }

            if (delayMs) {
                Sleep(delayMs);
            }

            ApplyTaskbarRowsRefreshPass(L"scheduled");
        }

        if (g_taskbarRowsRefreshRequest.load() == handledRequest) {
            break;
        }
    }

    g_taskbarRowsRefreshRunning = false;

    if (!g_unloading && g_taskbarRowsRefreshRequest.load() != handledRequest &&
        !g_taskbarRowsRefreshRunning.exchange(true)) {
        HANDLE thread =
            CreateThread(nullptr, 0, TaskbarRowsRefreshThreadProc, nullptr, 0,
                         nullptr);
        if (thread) {
            CloseHandle(thread);
        } else {
            g_taskbarRowsRefreshRunning = false;
        }
    }

    return 0;
}

void RequestTaskbarRowsRefresh() {
    if (g_unloading) {
        return;
    }

    g_taskbarRowsRefreshRequest++;

    if (!g_taskbarRowsRefreshRunning.exchange(true)) {
        HANDLE thread =
            CreateThread(nullptr, 0, TaskbarRowsRefreshThreadProc, nullptr, 0,
                         nullptr);
        if (thread) {
            CloseHandle(thread);
        } else {
            g_taskbarRowsRefreshRunning = false;
        }
    }
}

using CTaskListWnd_ComputeJumpViewPosition_t =
    HRESULT(WINAPI*)(void* pThis,
                     void* taskBtnGroup,
                     int param2,
                     winrt::Windows::Foundation::Point* point,
                     HorizontalAlignment* horizontalAlignment,
                     VerticalAlignment* verticalAlignment);
CTaskListWnd_ComputeJumpViewPosition_t
    CTaskListWnd_ComputeJumpViewPosition_Original;
HRESULT WINAPI CTaskListWnd_ComputeJumpViewPosition_Hook(
    void* pThis,
    void* taskBtnGroup,
    int param2,
    winrt::Windows::Foundation::Point* point,
    HorizontalAlignment* horizontalAlignment,
    VerticalAlignment* verticalAlignment) {
    Wh_Log(L">");

    HRESULT ret = CTaskListWnd_ComputeJumpViewPosition_Original(
        pThis, taskBtnGroup, param2, point, horizontalAlignment,
        verticalAlignment);

    DWORD messagePos = GetMessagePos();
    POINT pt{
        GET_X_LPARAM(messagePos),
        GET_Y_LPARAM(messagePos),
    };

    point->X = pt.x;

    return ret;
}

using IUIElement_Arrange_t =
    HRESULT(WINAPI*)(void* pThis, winrt::Windows::Foundation::Rect rect);
IUIElement_Arrange_t IUIElement_Arrange_Original;
HRESULT WINAPI IUIElement_Arrange_Hook(void* pThis,
                                       winrt::Windows::Foundation::Rect rect) {
    Wh_Log(L">");

    auto original = [=] { return IUIElement_Arrange_Original(pThis, rect); };

    if (g_settings.debugDisableArrangeHook ||
        !g_inTaskbarCollapsibleLayoutXamlTraits_ArrangeOverride ||
        g_unloading) {
        return original();
    }

    FrameworkElement element = nullptr;
    ((IUnknown*)pThis)
        ->QueryInterface(winrt::guid_of<FrameworkElement>(),
                         winrt::put_abi(element));
    if (!element) {
        return original();
    }

    auto taskbarFrameRepeater =
        Media::VisualTreeHelper::GetParent(element).as<FrameworkElement>();
    if (!taskbarFrameRepeater ||
        taskbarFrameRepeater.Name() != L"TaskbarFrameRepeater") {
        return original();
    }

    double leadingFullHeightControlsWidth = 0;
    if (g_settings.fullHeightStartButton) {
        if (IsLeadingFullHeightControl(element)) {
            bool isSearchControl = IsSearchControl(element);
            if (g_settings.debugDisableSearchArrange && isSearchControl) {
                static bool logged = false;
                if (!logged) {
                    logged = true;
                    MultirowDiagLog(L"[debug-disable-search-arrange] "
                                    L"bypassing Search arrange path");
                }
                return original();
            }

            if (IsTaskViewControl(element) && rect.Width > 0) {
                g_lastTaskViewButtonWidth = rect.Width;
            }

            auto newRect = rect;
            int rows = GetEffectiveRows();
            double frameHeight = GetTargetTaskbarLogicalHeightForRows(rows);
            if (!(frameHeight > 0)) {
                frameHeight = taskbarFrameRepeater.ActualHeight();
            }

            if (rows <= 1 && frameHeight > 0 &&
                (newRect.Height > frameHeight + 0.5 ||
                 AbsDiff(newRect.Y, 0) > 0.5)) {
                MultirowDiagLog(
                    L"[arrange-clamp-leading] class=%s name=%s rect=%.1f,%.1f "
                    L"%.1fx%.1f frameHeight=%.1f",
                    winrt::get_class_name(element).c_str(),
                    element.Name().c_str(), rect.X, rect.Y, rect.Width,
                    rect.Height, frameHeight);
                newRect.Y = 0;
                newRect.Height = frameHeight;
                return IUIElement_Arrange_Original(pThis, newRect);
            }

            bool isFullSearchBox =
                isSearchControl && IsFullSearchBoxControl(element, rect.Width);
            bool topAlignFullSearchBox =
                isFullSearchBox &&
                g_settings.searchBoxVerticalPlacement ==
                    SearchBoxVerticalPlacement::TopForMultirow;
            bool compactFullSearchBox =
                isFullSearchBox &&
                g_settings.multirowSearchBoxPolicy ==
                    MultirowSearchBoxPolicy::CompactButton;

            if (rows > 1 && isSearchControl &&
                g_settings.centerSearchControl) {
                double rowHeight = frameHeight / rows;
                if (rowHeight > 0) {
                    newRect.Height = rowHeight;
                    if (compactFullSearchBox) {
                        double compactWidth = g_lastTaskViewButtonWidth > 0
                                                  ? g_lastTaskViewButtonWidth
                                                  : 36.0;
                        if (compactWidth > 36.0) {
                            compactWidth = 36.0;
                        }
                        if (compactWidth > 0) {
                            newRect.Width = compactWidth;
                        }
                    }
                    if (compactFullSearchBox || topAlignFullSearchBox) {
                        newRect.Y = 0;
                    } else {
                        newRect.Y = (frameHeight - rowHeight) / 2;
                    }
                }

                LogSearchArrangeDecision(element, rect, newRect, rows,
                                         frameHeight, isFullSearchBox,
                                         compactFullSearchBox
                                             ? L"compact-full-search-box-top-row"
                                         : topAlignFullSearchBox
                                             ? L"top-full-search-box"
                                             : (isFullSearchBox
                                                    ? L"center-full-search-box"
                                                    : L"center-compact-search"));
                return IUIElement_Arrange_Original(pThis, newRect);
            }

            if (rows > 1 && !isSearchControl) {
                double rowHeight = frameHeight / rows;
                if (g_settings.compactLeadingButtonHover && rowHeight > 0) {
                    newRect.Y = (frameHeight - rowHeight) / 2;
                    newRect.Height = rowHeight;
                } else {
                    newRect.Y = 0;
                    newRect.Height = frameHeight;
                }
                return IUIElement_Arrange_Original(pThis, newRect);
            }

            if (rows > 1 && isSearchControl) {
                LogSearchArrangeDecision(element, rect, rect, rows, frameHeight,
                                         isFullSearchBox,
                                         L"native-search-fallback");
            }

            return original();
        }

        leadingFullHeightControlsWidth =
            GetLeadingFullHeightControlsWidth(taskbarFrameRepeater);
    }

    if (g_settings.debugDisableTaskListItemArrange) {
        static bool logged = false;
        if (!logged) {
            logged = true;
            MultirowDiagLog(L"[debug-disable-tasklist-arrange] bypassing "
                            L"task-list item arrange path");
        }
        return original();
    }

    auto xamlRoot = taskbarFrameRepeater.XamlRoot();
#ifdef WIN11_TASKBAR_WIN10_MULTIROW_COMBINED
    ::TrayComponent::ApplyXamlRootFromMultirow(xamlRoot);
#endif

    TaskbarState* taskbarState = GetTaskbarState(xamlRoot);

    auto xamlRootContent = xamlRoot.Content().as<FrameworkElement>();

    auto taskbarFrameElement =
        FindChildByName(xamlRootContent, L"TaskbarFrame");
    if (!taskbarFrameElement) {
        return original();
    }

    auto systemTrayFrame =
        FindChildByClassName(xamlRootContent, L"SystemTray.SystemTrayFrame");
    if (!systemTrayFrame) {
        return original();
    }

    double widthWithoutExtent =
        GetTaskbarWidthWithoutExtent(taskbarFrameElement, systemTrayFrame);

    int rows = GetEffectiveRows();
    UpdateTaskbarStateGeometry(taskbarState, rows, widthWithoutExtent,
                               leadingFullHeightControlsWidth);

    winrt::Windows::Foundation::Rect newRect = rect;
    double taskbarHeight = GetTargetTaskbarLogicalHeightForRows(rows);
    if (rows > 1 && taskbarHeight > 0) {
        double rowHeight = taskbarHeight / rows;
        if (rowHeight > 0) {
            if (AbsDiff(newRect.Height, taskbarHeight) > 0.5 ||
                AbsDiff(newRect.Height / rows, rowHeight) > 0.5) {
                MultirowDiagLog(
                    L"[arrange-row-height-from-target] class=%s name=%s "
                    L"rect=%.1f,%.1f %.1fx%.1f rows=%d taskbarHeight=%.1f "
                    L"oldRowHeight=%.1f newRowHeight=%.1f",
                    winrt::get_class_name(element).c_str(),
                    element.Name().c_str(), rect.X, rect.Y, rect.Width,
                    rect.Height, rows, taskbarHeight, rect.Height / rows,
                    rowHeight);
            }

            newRect.Height = rowHeight;
        } else {
            newRect.Height /= rows;
        }
    } else {
        newRect.Height /= rows;
    }

    if (rows <= 1 && taskbarHeight > 0 &&
        (newRect.Height > taskbarHeight + 0.5 ||
         AbsDiff(newRect.Y, 0) > 0.5)) {
        MultirowDiagLog(
            L"[arrange-clamp-item] class=%s name=%s rect=%.1f,%.1f %.1fx%.1f "
            L"taskbarHeight=%.1f",
            winrt::get_class_name(element).c_str(), element.Name().c_str(),
            rect.X, rect.Y, rect.Width, rect.Height, taskbarHeight);
        newRect.Y = 0;
        newRect.Height = taskbarHeight;
    }

    for (int i = 0; i < rows - 1 &&
                    newRect.X + newRect.Width > widthWithoutExtent;
         i++) {
        newRect.X -= widthWithoutExtent;
        if (newRect.X <= 0) {
            taskbarState->rowOffsetAdjustment[i] =
                -newRect.X + leadingFullHeightControlsWidth;
            newRect.X = leadingFullHeightControlsWidth;
        } else {
            newRect.X += taskbarState->rowOffsetAdjustment[i];
        }

        newRect.Y += newRect.Height;
    }

    if (newRect.X + newRect.Width > widthWithoutExtent) {
        UpdateTaskbarFrameRepeaterMargin(taskbarFrameRepeater, taskbarState,
                                         widthWithoutExtent);
    }

    return IUIElement_Arrange_Original(pThis, newRect);
}

using TaskbarCollapsibleLayoutXamlTraits_ArrangeOverride_t =
    HRESULT(WINAPI*)(void* pThis,
                     void* context,
                     winrt::Windows::Foundation::Size size,
                     winrt::Windows::Foundation::Size* resultSize);
TaskbarCollapsibleLayoutXamlTraits_ArrangeOverride_t
    TaskbarCollapsibleLayoutXamlTraits_ArrangeOverride_Original;
HRESULT WINAPI TaskbarCollapsibleLayoutXamlTraits_ArrangeOverride_Hook(
    void* pThis,
    void* context,
    winrt::Windows::Foundation::Size size,
    winrt::Windows::Foundation::Size* resultSize) {
    Wh_Log(L">");

    if (g_settings.debugDisableArrangeHook) {
        static bool logged = false;
        if (!logged) {
            logged = true;
            MultirowDiagLog(L"[debug-disable-arrange-hook] bypassing "
                            L"TaskbarCollapsibleLayout arrange override");
        }
        return TaskbarCollapsibleLayoutXamlTraits_ArrangeOverride_Original(
            pThis, context, size, resultSize);
    }

    [[maybe_unused]] static bool hooked = [] {
        Shapes::Rectangle rectangle;
        IUIElement element = rectangle;

        void** vtable = *(void***)winrt::get_abi(element);
        auto arrange = (IUIElement_Arrange_t)vtable[92];

        WindhawkUtils::Wh_SetFunctionHookT(arrange, IUIElement_Arrange_Hook,
                                           &IUIElement_Arrange_Original);
        Wh_ApplyHookOperations();
        return true;
    }();

    g_inTaskbarCollapsibleLayoutXamlTraits_ArrangeOverride = true;

    HRESULT ret = TaskbarCollapsibleLayoutXamlTraits_ArrangeOverride_Original(
        pThis, context, size, resultSize);

    g_inTaskbarCollapsibleLayoutXamlTraits_ArrangeOverride = false;

    return ret;
}

using TaskbarFrame_SystemTrayExtent_t = void(WINAPI*)(void* pThis,
                                                      double value);
TaskbarFrame_SystemTrayExtent_t TaskbarFrame_SystemTrayExtent_Original;
void WINAPI TaskbarFrame_SystemTrayExtent_Hook(void* pThis, double value) {
    Wh_Log(L"> %f", value);

    TaskbarFrame_SystemTrayExtent_Original(pThis, value);

    FrameworkElement taskbarFrameElement = nullptr;
    ((IUnknown**)pThis)[1]->QueryInterface(winrt::guid_of<FrameworkElement>(),
                                           winrt::put_abi(taskbarFrameElement));
    if (!taskbarFrameElement) {
        return;
    }

    FrameworkElement taskbarFrameRepeater = nullptr;

    FrameworkElement child = taskbarFrameElement;
    if ((child = FindChildByName(child, L"RootGrid")) &&
        (child = FindChildByName(child, L"TaskbarFrameRepeater"))) {
        taskbarFrameRepeater = child;
    }

    if (!taskbarFrameRepeater) {
        return;
    }

    auto xamlRoot = taskbarFrameRepeater.XamlRoot();
#ifdef WIN11_TASKBAR_WIN10_MULTIROW_COMBINED
    ::TrayComponent::ApplyXamlRootFromMultirow(xamlRoot);
#endif

    TaskbarState* taskbarState = GetTaskbarState(xamlRoot);

    double widthWithoutExtent =
        GetTaskbarWidthWithoutExtent(taskbarFrameElement, value);

    UpdateTaskbarFrameRepeaterMargin(taskbarFrameRepeater, taskbarState,
                                     widthWithoutExtent);
    ForceElementLayout(taskbarFrameRepeater, L"system-tray-extent-repeater");
}

using RegGetValueW_t = decltype(&RegGetValueW);
RegGetValueW_t RegGetValueW_Original;
LONG WINAPI RegGetValueW_Hook(HKEY hkey,
                              LPCWSTR lpSubKey,
                              LPCWSTR lpValue,
                              DWORD dwFlags,
                              LPDWORD pdwType,
                              PVOID pvData,
                              LPDWORD pcbData) {
    LONG ret = RegGetValueW_Original(hkey, lpSubKey, lpValue, dwFlags, pdwType,
                                     pvData, pcbData);

    if (hkey == HKEY_CURRENT_USER && lpSubKey &&
        _wcsicmp(
            lpSubKey,
            LR"(SOFTWARE\Microsoft\Windows\CurrentVersion\Explorer\Advanced)") ==
            0 &&
        lpValue && _wcsicmp(lpValue, L"TaskbarAl") == 0 &&
        dwFlags == RRF_RT_REG_DWORD && pvData && pcbData &&
        *pcbData == sizeof(DWORD)) {
        Wh_Log(L"> %u", ret);

        if (!g_unloading) {
            Wh_Log(L"Overriding");

            *(DWORD*)pvData = 0;

            if (pdwType) {
                *pdwType = REG_DWORD;
            }

            ret = ERROR_SUCCESS;
        } else {
            Wh_Log(L"Returning original value: %u", *(DWORD*)pvData);
        }
    }

    return ret;
}

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
            {LR"(const CSecondaryTaskBand::`vftable'{for `ITaskListWndSite'})"},
            &CSecondaryTaskBand_ITaskListWndSite_vftable,
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
            {LR"(public: virtual class std::shared_ptr<class TaskbarHost> __cdecl CSecondaryTaskBand::GetTaskbarHost(void)const )"},
            &CSecondaryTaskBand_GetTaskbarHost_Original,
        },
        {
            {LR"(public: void __cdecl std::_Ref_count_base::_Decref(void))"},
            &std__Ref_count_base__Decref_Original,
        },
        {
            {LR"(protected: long __cdecl CTaskListWnd::_ComputeJumpViewPosition(struct ITaskBtnGroup *,int,struct Windows::Foundation::Point &,enum Windows::UI::Xaml::HorizontalAlignment &,enum Windows::UI::Xaml::VerticalAlignment &)const )"},
            &CTaskListWnd_ComputeJumpViewPosition_Original,
            CTaskListWnd_ComputeJumpViewPosition_Hook,
        },
    };

    if (!HookSymbols(module, taskbarDllHooks, ARRAYSIZE(taskbarDllHooks))) {
        Wh_Log(L"HookSymbols failed");
        return false;
    }

    return true;
}

bool HookTaskbarViewDllSymbols(HMODULE module) {
    // Taskbar.View.dll
    WindhawkUtils::SYMBOL_HOOK symbolHooks[] = {
        {
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskbarCollapsibleLayout,struct winrt::Microsoft::UI::Xaml::Controls::IVirtualizingLayoutOverrides>::ArrangeOverride(void *,struct winrt::Windows::Foundation::Size,struct winrt::Windows::Foundation::Size *))"},
            &TaskbarCollapsibleLayoutXamlTraits_ArrangeOverride_Original,
            TaskbarCollapsibleLayoutXamlTraits_ArrangeOverride_Hook,
        },
        {
            {LR"(public: void __cdecl winrt::Taskbar::implementation::TaskbarFrame::SystemTrayExtent(double))"},
            &TaskbarFrame_SystemTrayExtent_Original,
            TaskbarFrame_SystemTrayExtent_Hook,
        },
    };

    if (!HookSymbols(module, symbolHooks, ARRAYSIZE(symbolHooks))) {
        Wh_Log(L"HookSymbols failed");
        return false;
    }

    return true;
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
            HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();
            Wh_Log(L"Applying settings after delayed Taskbar.View hook hwnd=%08X",
                   (DWORD)(DWORD_PTR)hTaskbarWnd);
            if (hTaskbarWnd) {
                ApplySettings(hTaskbarWnd);
            }
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

void LoadSettings() {
#ifdef WIN11_TASKBAR_WIN10_MULTIROW_COMBINED
    g_settings.rows = 1;
#else
    g_settings.rows = Wh_GetIntSetting(L"rows");
#endif
#ifdef WIN11_TASKBAR_WIN10_MULTIROW_COMBINED
    int persistedRows = Wh_GetIntValue(L"EdgeResizeRows", 0);
    if (persistedRows > 0) {
        if (persistedRows < 1) {
            persistedRows = 1;
        } else if (persistedRows > 10) {
            persistedRows = 10;
        }

        if (persistedRows != g_settings.rows) {
            Wh_Log(L"Using edge-resize storage rows=%d over settings rows=%d",
                   persistedRows, g_settings.rows);
        }
        g_settings.rows = persistedRows;
    }
#endif
#ifdef WIN11_TASKBAR_WIN10_MULTIROW_COMBINED
    g_settings.fullHeightStartButton = true;
    g_settings.compactLeadingButtonHover = true;
    g_settings.centerSearchControl = true;
    g_settings.debugDisableArrangeHook = false;
    g_settings.debugDisableSearchArrange = false;
    g_settings.debugDisableTaskListItemArrange = false;
    g_settings.searchBoxVerticalPlacement = SearchBoxVerticalPlacement::Center;
    g_settings.multirowSearchBoxPolicy = MultirowSearchBoxPolicy::Keep;
#else
    g_settings.fullHeightStartButton =
        Wh_GetIntSetting(L"fullHeightStartButton");
    g_settings.compactLeadingButtonHover =
        Wh_GetIntSetting(L"compactLeadingButtonHover");
    g_settings.centerSearchControl = Wh_GetIntSetting(L"centerSearchControl");
    g_settings.debugDisableArrangeHook =
        Wh_GetIntSetting(L"debugDisableArrangeHook");
    g_settings.debugDisableSearchArrange =
        Wh_GetIntSetting(L"debugDisableSearchArrange");
    g_settings.debugDisableTaskListItemArrange =
        Wh_GetIntSetting(L"debugDisableTaskListItemArrange");
    PCWSTR searchBoxVerticalPlacement =
        Wh_GetStringSetting(L"searchBoxVerticalPlacement");
    g_settings.searchBoxVerticalPlacement = SearchBoxVerticalPlacement::Center;
    if (searchBoxVerticalPlacement &&
        wcscmp(searchBoxVerticalPlacement, L"topForMultirow") == 0) {
        g_settings.searchBoxVerticalPlacement =
            SearchBoxVerticalPlacement::TopForMultirow;
    }
    if (searchBoxVerticalPlacement) {
        Wh_FreeStringSetting(searchBoxVerticalPlacement);
    }
    PCWSTR multirowSearchBoxPolicy =
        Wh_GetStringSetting(L"multirowSearchBoxPolicy");
    g_settings.multirowSearchBoxPolicy = MultirowSearchBoxPolicy::Keep;
    if (multirowSearchBoxPolicy &&
        wcscmp(multirowSearchBoxPolicy, L"compactButton") == 0) {
        g_settings.multirowSearchBoxPolicy =
            MultirowSearchBoxPolicy::CompactButton;
    }
    if (multirowSearchBoxPolicy) {
        Wh_FreeStringSetting(multirowSearchBoxPolicy);
    }
#endif
    g_liveRowsHint = 0;
    g_liveRowsHintTick = 0;
    g_liveTaskbarHeightHint = 0;
    g_liveTaskbarHeightHintTick = 0;
}

BOOL Wh_ModInit() {
    Wh_Log(L">");

    LoadSettings();

    if (!HookTaskbarDllSymbols()) {
        return FALSE;
    }

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
        WindhawkUtils::Wh_SetFunctionHookT(pKernelBaseLoadLibraryExW,
                                           LoadLibraryExW_Hook,
                                           &LoadLibraryExW_Original);
    }

    HMODULE kernelBaseModule = GetModuleHandle(L"kernelbase.dll");
    auto pKernelBaseRegGetValueW = (decltype(&RegGetValueW))GetProcAddress(
        kernelBaseModule, "RegGetValueW");
    WindhawkUtils::Wh_SetFunctionHookT(
        pKernelBaseRegGetValueW, RegGetValueW_Hook, &RegGetValueW_Original);

    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L">");

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

    HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();
    if (hTaskbarWnd) {
        ApplySettings(hTaskbarWnd);
    }
}

void Wh_ModBeforeUninit() {
    Wh_Log(L">");

    PrepareNativeUnload(48);

    g_unloading = true;
}

void Wh_ModUninit() {
    Wh_Log(L">");
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");

    LoadSettings();

    HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();
    if (hTaskbarWnd) {
        ApplySettings(hTaskbarWnd);
    }
}
// END INLINED COMPONENT: taskbar-multirow
}

namespace TrayComponent {
namespace winrt = ::winrt;
// BEGIN INLINED COMPONENT: notification-area
// Source code is published under The GNU General Public License v3.0.
//
// For bug reports and feature requests, please open an issue here:
// https://github.com/ramensoftware/windhawk-mods/issues
//
// For pull requests, development takes place here:
// https://github.com/m417z/my-windhawk-mods



#include <windhawk_utils.h>

#include <atomic>
#include <cmath>
#include <chrono>
#include <cstdarg>
#include <cstdint>
#include <functional>
#include <list>
#include <memory>
#include <sstream>
#include <string>
#include <thread>
#include <unordered_set>
#include <vector>

#undef GetCurrentTime

#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Automation.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.Xaml.Markup.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/base.h>

using namespace winrt::Windows::UI::Xaml;

enum class GridArrangement {
    rowFirstLeftToRight,
    columnFirstTopToBottom,
    rowFirstBottomRowFirst,
    columnFirstBottomToTop,
    columnFirstBottomToTopRightToLeft,
};

enum class TrayLayoutMode {
    nativeStack,
    realWrapGrid,
    legacyTransform,
};

struct {
    int taskbarHeight;
    int notificationIconWidth;
    int notificationIconRows;
    TrayLayoutMode trayLayoutMode;
    GridArrangement gridArrangement;
    int overflowIconWidth;
    int overflowIconsPerRow;
} g_settings;

std::atomic<bool> g_systemTrayModuleHooked;
std::atomic<bool> g_unloading;
std::atomic<bool> g_heightMonitorStop;
std::atomic<bool> g_heightMonitorRunning;
std::atomic<bool> g_notificationRowsRefreshRunning;
std::atomic<int> g_notificationRowsRefreshRequest;
std::atomic<int> g_lastAppliedNotificationRows;
std::atomic<int> g_liveNotificationIconRowsHint;
std::atomic<DWORD> g_liveNotificationIconRowsHintTick;
std::atomic<int> g_liveTaskbarHeightHint;
std::atomic<DWORD> g_liveTaskbarHeightHintTick;
std::atomic<int> g_trayDiagLineCount;

using FrameworkElementLoadedEventRevoker = winrt::impl::event_revoker<
    IFrameworkElement,
    &winrt::impl::abi<IFrameworkElement>::type::remove_Loaded>;

std::list<FrameworkElementLoadedEventRevoker> g_autoRevokerList;
std::list<FrameworkElement::SizeChanged_revoker> g_sizeChangedRevokerList;
std::list<FrameworkElement::SizeChanged_revoker> g_containerSizeChangedRevokerList;
std::unordered_set<uintptr_t> g_sizeChangedHookedElements;
std::unordered_set<uintptr_t> g_containerSizeHookedElements;

winrt::weak_ref<FrameworkElement> g_notificationAreaIconsStackPanel;
winrt::weak_ref<FrameworkElement> g_overflowRootGrid;
winrt::weak_ref<XamlRoot> g_cachedTaskbarXamlRoot;
Controls::ItemsPanelTemplate g_originalNotificationAreaItemsPanelTemplate =
    nullptr;
std::wstring g_pendingTrayGridFingerprint;
std::wstring g_lastAppliedTrayGridFingerprint;
std::unordered_set<uintptr_t> g_lastAppliedTrayGridIconKeys;
DWORD g_pendingTrayGridFingerprintTick;

void RequestNotificationIconRowsRefresh();
void ApplyNotificationIconRowsRefreshPass(PCWSTR reason);
void TrayDiagLog(PCWSTR format, ...);
PCWSTR TrayLayoutModeName(TrayLayoutMode mode);

uintptr_t GetFrameworkElementKey(FrameworkElement element) {
    if (!element) {
        return 0;
    }

    return reinterpret_cast<uintptr_t>(winrt::get_abi(element));
}

void ForceElementLayout(FrameworkElement element, PCWSTR reason) {
    for (int depth = 0; depth < 12 && element; depth++) {
        try {
            element.InvalidateMeasure();
            element.InvalidateArrange();
            element.UpdateLayout();
        } catch (...) {
            TrayDiagLog(L"[force-layout] reason=%s depth=%d failed", reason,
                        depth);
            return;
        }

        std::wstring className = winrt::get_class_name(element).c_str();
        if (className == L"SystemTray.SystemTrayFrame") {
            break;
        }

        element = Media::VisualTreeHelper::GetParent(element)
                      .try_as<FrameworkElement>();
    }
}

void SetLiveNotificationIconRowsHint(int rows) {
    if (rows < 1) {
        rows = 1;
    } else if (rows > 10) {
        rows = 10;
    }

    int oldRows = g_liveNotificationIconRowsHint.exchange(rows);
    g_liveNotificationIconRowsHintTick = GetTickCount();
#ifdef WIN11_TASKBAR_WIN10_MULTIROW_COMBINED
    int oldSettingsRows = g_settings.notificationIconRows;
    TrayLayoutMode oldTrayLayoutMode = g_settings.trayLayoutMode;
    g_settings.notificationIconRows = rows;
    g_settings.trayLayoutMode = rows > 1 ? TrayLayoutMode::realWrapGrid
                                         : TrayLayoutMode::nativeStack;
#endif
    TrayDiagLog(L"[live-rows] hint=%d old=%d", rows, oldRows);
#ifdef WIN11_TASKBAR_WIN10_MULTIROW_COMBINED
    TrayDiagLog(L"[live-rows] settingsRows=%d oldSettingsRows=%d mode=%s "
                L"oldMode=%s",
                g_settings.notificationIconRows, oldSettingsRows,
                TrayLayoutModeName(g_settings.trayLayoutMode),
                TrayLayoutModeName(oldTrayLayoutMode));
#endif
    RequestNotificationIconRowsRefresh();
}

void SetLiveTaskbarHeightHint(int height) {
    if (height < 1) {
        height = 1;
    }

    int oldHeight = g_liveTaskbarHeightHint.exchange(height);
    g_liveTaskbarHeightHintTick = GetTickCount();
    TrayDiagLog(L"[live-height] hint=%d old=%d", height, oldHeight);
    RequestNotificationIconRowsRefresh();
}

bool BuildTrayDiagPath(WCHAR* path, DWORD pathCount) {
    if (!path || pathCount == 0) {
        return false;
    }

    DWORD len = GetTempPathW(pathCount, path);
    if (len == 0 || len >= pathCount) {
        return false;
    }

    if (wcscat_s(path, pathCount,
                 L"draggable-multirow-taskbar-systray-windows-11") != 0) {
        return false;
    }

    CreateDirectoryW(path, nullptr);

    return wcscat_s(path, pathCount, L"\\tray-runtime-diag.txt") == 0;
}

void SetLiveLayoutHints(int rows, int height) {
    if (rows < 1) {
        rows = 1;
    } else if (rows > 10) {
        rows = 10;
    }

    if (height < 1) {
        height = 1;
    }

    int oldHeight = g_liveTaskbarHeightHint.exchange(height);
    g_liveTaskbarHeightHintTick = GetTickCount();
    int oldRows = g_liveNotificationIconRowsHint.exchange(rows);
    g_liveNotificationIconRowsHintTick = GetTickCount();
#ifdef WIN11_TASKBAR_WIN10_MULTIROW_COMBINED
    int oldSettingsHeight = g_settings.taskbarHeight;
    int oldSettingsRows = g_settings.notificationIconRows;
    TrayLayoutMode oldTrayLayoutMode = g_settings.trayLayoutMode;
    g_settings.taskbarHeight = height;
    g_settings.notificationIconRows = rows;
    g_settings.trayLayoutMode = rows > 1 ? TrayLayoutMode::realWrapGrid
                                         : TrayLayoutMode::nativeStack;
#endif

    TrayDiagLog(L"[live-layout] rows=%d oldRows=%d height=%d oldHeight=%d",
                rows, oldRows, height, oldHeight);
#ifdef WIN11_TASKBAR_WIN10_MULTIROW_COMBINED
    TrayDiagLog(L"[live-layout] settingsRows=%d oldSettingsRows=%d "
                L"settingsHeight=%d oldSettingsHeight=%d mode=%s oldMode=%s",
                g_settings.notificationIconRows, oldSettingsRows,
                g_settings.taskbarHeight, oldSettingsHeight,
                TrayLayoutModeName(g_settings.trayLayoutMode),
                TrayLayoutModeName(oldTrayLayoutMode));
#endif
    ApplyNotificationIconRowsRefreshPass(L"live-layout-immediate");
    RequestNotificationIconRowsRefresh();
}

void TrayDiagLog(PCWSTR format, ...) {
    int lineIndex = g_trayDiagLineCount.fetch_add(1);
    if (lineIndex > 50000) {
        return;
    }

    WCHAR message[1536];
    va_list args;
    va_start(args, format);
    _vsnwprintf_s(message, ARRAYSIZE(message), _TRUNCATE, format, args);
    va_end(args);

    SYSTEMTIME st{};
    GetLocalTime(&st);

    WCHAR line[1792];
    swprintf_s(line, L"%02u:%02u:%02u.%03u %s\r\n", st.wHour, st.wMinute,
               st.wSecond, st.wMilliseconds, message);

    WCHAR path[MAX_PATH];
    if (!BuildTrayDiagPath(path, ARRAYSIZE(path))) {
        return;
    }

    HANDLE file = CreateFileW(path, FILE_APPEND_DATA,
                              FILE_SHARE_READ | FILE_SHARE_WRITE |
                                  FILE_SHARE_DELETE,
                              nullptr, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL,
                              nullptr);
    if (file == INVALID_HANDLE_VALUE) {
        return;
    }

    int bytes = WideCharToMultiByte(CP_UTF8, 0, line, -1, nullptr, 0, nullptr,
                                    nullptr);
    if (bytes > 1) {
        std::string utf8(bytes, '\0');
        if (WideCharToMultiByte(CP_UTF8, 0, line, -1, utf8.data(), bytes,
                                nullptr, nullptr)) {
            DWORD written;
            WriteFile(file, utf8.data(), bytes - 1, &written, nullptr);
        }
    }

    CloseHandle(file);
}

void AppendParentChainDiag(std::wstring text) {
    TrayDiagLog(L"%s", text.c_str());
}

void DumpParentChainDiag(FrameworkElement element, PCWSTR reason) {
    TrayDiagLog(L"[parent-chain] %s", reason);
    FrameworkElement current = element;
    for (int depth = 0; depth < 16 && current; depth++) {
        auto className = winrt::get_class_name(current);
        int childCount = 0;
        try {
            childCount = Media::VisualTreeHelper::GetChildrenCount(current);
        } catch (...) {
        }

        TrayDiagLog(
            L"  depth=%d name=%s class=%s actual=%.1fx%.1f size=%.1fx%.1f "
            L"min=%.1fx%.1f max=%.1fx%.1f children=%d vis=%d",
            depth, current.Name().c_str(), className.c_str(),
            current.ActualWidth(), current.ActualHeight(), current.Width(),
            current.Height(), current.MinWidth(), current.MinHeight(),
            current.MaxWidth(), current.MaxHeight(), childCount,
            static_cast<int>(current.Visibility()));

        current = Media::VisualTreeHelper::GetParent(current)
                      .try_as<FrameworkElement>();
    }
}


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

int ClampNotificationIconRows(int rows) {
    if (rows < 1) {
        return 1;
    }
    if (rows > 10) {
        return 10;
    }

    return rows;
}

int RowsFromLogicalHeight(double logicalHeight) {
    if (logicalHeight <= 0.0) {
        return 1;
    }

    return ClampNotificationIconRows(
        static_cast<int>((logicalHeight + 24.0) / 48.0));
}

int GetFreshLiveNotificationIconRowsHint() {
    int hintedRows = g_liveNotificationIconRowsHint.load();
    DWORD hintTick = g_liveNotificationIconRowsHintTick.load();
    if (hintedRows > 0 && GetTickCount() - hintTick < 5000) {
        return ClampNotificationIconRows(hintedRows);
    }

    return 0;
}

TrayLayoutMode GetEffectiveTrayLayoutMode() {
#ifdef WIN11_TASKBAR_WIN10_MULTIROW_COMBINED
    int hintedRows = GetFreshLiveNotificationIconRowsHint();
    if (hintedRows > 1) {
        return TrayLayoutMode::realWrapGrid;
    }
    if (hintedRows == 1) {
        return TrayLayoutMode::nativeStack;
    }
#endif

    return g_settings.trayLayoutMode;
}

int GetPersistedEdgeResizeTaskbarHeight() {
#ifdef WIN11_TASKBAR_WIN10_MULTIROW_COMBINED
    int height = Wh_GetIntValue(L"EdgeResizeTaskbarHeight", 0);
    if (height > 0 && height <= 480) {
        return height;
    }
#endif

    return 0;
}

int ResolveTaskbarLogicalHeight(int windowLogicalHeight) {
    int persistedHeight = GetPersistedEdgeResizeTaskbarHeight();
    if (persistedHeight > 0) {
        return persistedHeight;
    }

    int logicalHeight = windowLogicalHeight;
    if (g_settings.taskbarHeight > logicalHeight) {
        logicalHeight = g_settings.taskbarHeight;
    }

    return logicalHeight;
}

int GetEffectiveNotificationIconRows() {
    if (g_unloading) {
        return 1;
    }

    int hintedRows = GetFreshLiveNotificationIconRowsHint();
    if (hintedRows > 0) {
        return hintedRows;
    }

    int rows = ClampNotificationIconRows(g_settings.notificationIconRows);

    HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();
    if (!hTaskbarWnd) {
        return rows;
    }

    RECT rect{};
    if (!GetWindowRect(hTaskbarWnd, &rect)) {
        return rows;
    }

    int dpi = GetDpiForWindow(hTaskbarWnd);
    int windowLogicalHeight =
        MulDiv(rect.bottom - rect.top, 96, dpi ? dpi : 96);
    int logicalHeight = ResolveTaskbarLogicalHeight(windowLogicalHeight);
    int dynamicRows = RowsFromLogicalHeight(logicalHeight);

#ifdef WIN11_TASKBAR_WIN10_MULTIROW_COMBINED
    if (GetEffectiveTrayLayoutMode() == TrayLayoutMode::realWrapGrid) {
        int effectiveRows =
            GetPersistedEdgeResizeTaskbarHeight() > 0
                ? dynamicRows
                : std::max(rows, dynamicRows);
        if (effectiveRows != rows) {
            TrayDiagLog(
                L"[rows-resolve] realWrapGrid settingRows=%d dynamicRows=%d "
                L"windowHeight=%d logicalHeight=%d persistedHeight=%d using=%d",
                rows, dynamicRows, windowLogicalHeight, logicalHeight,
                GetPersistedEdgeResizeTaskbarHeight(), effectiveRows);
        }

        return effectiveRows;
    }

    return rows;
#endif

    return dynamicRows;
}

int GetEffectiveTaskbarLogicalHeight() {
    int hintedHeight = g_liveTaskbarHeightHint.load();
    DWORD hintTick = g_liveTaskbarHeightHintTick.load();
    if (hintedHeight > 0 && GetTickCount() - hintTick < 5000) {
        return hintedHeight;
    }

    HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();
    if (!hTaskbarWnd) {
        return 0;
    }

    RECT rect{};
    if (!GetWindowRect(hTaskbarWnd, &rect)) {
        return 0;
    }

    int dpi = GetDpiForWindow(hTaskbarWnd);
    int windowLogicalHeight =
        MulDiv(rect.bottom - rect.top, 96, dpi ? dpi : 96);
    return ResolveTaskbarLogicalHeight(windowLogicalHeight);
}

int GetEffectiveNotificationIconRowHeight(int rows, int width) {
    if (rows < 1) {
        rows = 1;
    }

    int logicalHeight = GetEffectiveTaskbarLogicalHeight();
    if (logicalHeight <= 0) {
        logicalHeight = rows * 48;
    }

    double gap = logicalHeight - 16 * rows;
    double gapPerItem = std::fmax(gap, 0.0) / (rows + 1);
    int gapPerItemEven = static_cast<int>(gapPerItem) / 2 * 2;
    int itemHeight = 16 + gapPerItemEven;

    int minPitch = std::max(width, 24);
    if (itemHeight < minPitch) {
        itemHeight = minPitch;
    }

    return itemHeight;
}

int GetRealWrapGridNotificationIconSlotHeight(int rows,
                                              int targetPanelHeight,
                                              int compactItemHeight) {
    if (rows < 1) {
        rows = 1;
    }

    int fullRowSlotHeight = targetPanelHeight > 0 ? targetPanelHeight / rows : 0;
    return std::max(compactItemHeight, fullRowSlotHeight);
}

void ApplyRowCellBounds(FrameworkElement element, int width, int itemHeight) {
    element.Width(width);
    element.MinWidth(width);
    element.Height(itemHeight);
    element.MaxHeight(itemHeight);
    element.VerticalAlignment(VerticalAlignment::Top);

    Media::RectangleGeometry clip;
    clip.Rect(winrt::Windows::Foundation::Rect{
        0.0f,
        0.0f,
        static_cast<float>(width),
        static_cast<float>(itemHeight),
    });
    element.Clip(clip);
}

void ApplyNotifyIconViewRowBounds(FrameworkElement notifyIconViewElement,
                                  int rows,
                                  int width) {
    auto notifyIconViewDp = notifyIconViewElement.as<DependencyObject>();
    if (rows > 1) {
        int itemHeight = GetEffectiveNotificationIconRowHeight(rows, width);
        notifyIconViewElement.Height(itemHeight);
        notifyIconViewElement.MaxHeight(itemHeight);
        notifyIconViewElement.VerticalAlignment(VerticalAlignment::Top);
    } else {
        notifyIconViewDp.ClearValue(FrameworkElement::HeightProperty());
        notifyIconViewDp.ClearValue(FrameworkElement::MaxHeightProperty());
        notifyIconViewElement.VerticalAlignment(VerticalAlignment::Center);
    }
}

void ResetNotifyIconViewLayout(FrameworkElement notifyIconViewElement) {
    if (!notifyIconViewElement) {
        return;
    }

    auto notifyIconViewDp = notifyIconViewElement.as<DependencyObject>();
    notifyIconViewDp.ClearValue(FrameworkElement::WidthProperty());
    notifyIconViewDp.ClearValue(FrameworkElement::MinWidthProperty());
    notifyIconViewDp.ClearValue(FrameworkElement::MaxWidthProperty());
    notifyIconViewDp.ClearValue(FrameworkElement::HeightProperty());
    notifyIconViewDp.ClearValue(FrameworkElement::MinHeightProperty());
    notifyIconViewDp.ClearValue(FrameworkElement::MaxHeightProperty());
    notifyIconViewDp.ClearValue(FrameworkElement::VerticalAlignmentProperty());
}

bool IsChildOfElementByName(FrameworkElement element, PCWSTR name) {
    auto parent = element;
    while (true) {
        parent = Media::VisualTreeHelper::GetParent(parent)
                     .try_as<FrameworkElement>();
        if (!parent) {
            return false;
        }

        if (parent.Name() == name) {
            return true;
        }
    }
}

bool IsChildOfElementByClassName(FrameworkElement element, PCWSTR className) {
    auto parent = element;
    while (true) {
        parent = Media::VisualTreeHelper::GetParent(parent)
                     .try_as<FrameworkElement>();
        if (!parent) {
            return false;
        }

        if (winrt::get_class_name(parent) == className) {
            return true;
        }
    }
}

FrameworkElement FindAncestorByClassName(FrameworkElement element,
                                         PCWSTR className,
                                         int maxDepth = 16) {
    auto parent = element;
    for (int depth = 0; depth < maxDepth; depth++) {
        parent = Media::VisualTreeHelper::GetParent(parent)
                     .try_as<FrameworkElement>();
        if (!parent) {
            return nullptr;
        }

        if (winrt::get_class_name(parent) == className) {
            return parent;
        }
    }

    return nullptr;
}

FrameworkElement EnumChildElements(
    FrameworkElement element,
    std::function<bool(FrameworkElement)> enumCallback) {
    int childrenCount = Media::VisualTreeHelper::GetChildrenCount(element);

    for (int i = 0; i < childrenCount; i++) {
        auto child = Media::VisualTreeHelper::GetChild(element, i)
                         .try_as<FrameworkElement>();
        if (!child) {
            Wh_Log(L"Failed to get child %d of %d", i + 1, childrenCount);
            continue;
        }

        if (enumCallback(child)) {
            return child;
        }
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

bool EnumDescendantElements(
    FrameworkElement element,
    std::function<bool(FrameworkElement, int)> enumCallback,
    int depth = 0) {
    if (!element || depth > 64) {
        return false;
    }

    int childrenCount = Media::VisualTreeHelper::GetChildrenCount(element);
    for (int i = 0; i < childrenCount; i++) {
        auto child = Media::VisualTreeHelper::GetChild(element, i)
                         .try_as<FrameworkElement>();
        if (!child) {
            continue;
        }

        if (enumCallback(child, depth + 1)) {
            return true;
        }

        if (EnumDescendantElements(child, enumCallback, depth + 1)) {
            return true;
        }
    }

    return false;
}

FrameworkElement FindDescendantByName(FrameworkElement element, PCWSTR name) {
    FrameworkElement result = nullptr;
    EnumDescendantElements(element, [name, &result](FrameworkElement child,
                                                    int) {
        if (child.Name() == name) {
            result = child;
            return true;
        }

        return false;
    });

    return result;
}

FrameworkElement FindDescendantByClassName(FrameworkElement element,
                                           PCWSTR className) {
    FrameworkElement result = nullptr;
    EnumDescendantElements(element, [className, &result](FrameworkElement child,
                                                         int) {
        if (winrt::get_class_name(child) == className) {
            result = child;
            return true;
        }

        return false;
    });

    return result;
}

int CountDirectChildren(FrameworkElement element) {
    if (!element) {
        return 0;
    }

    return Media::VisualTreeHelper::GetChildrenCount(element);
}

bool IsElementVisibleEnough(FrameworkElement element) {
    if (!element || !element.IsLoaded() ||
        element.Visibility() != Visibility::Visible) {
        return false;
    }

    return element.ActualWidth() > 0 || element.ActualHeight() > 0;
}

void ApplyNotifyIconViewOverflowStyle(FrameworkElement notifyIconViewElement,
                                      int width) {
    Wh_Log(L"Setting MinWidth=%d for NotifyIconView (overflow)", width);
    notifyIconViewElement.MinWidth(width);

    Wh_Log(L"Setting Height=%d for NotifyIconView (overflow)", width);
    notifyIconViewElement.Height(width);

    FrameworkElement child = notifyIconViewElement;
    if ((child = FindChildByName(child, L"ContainerGrid")) &&
        (child = FindChildByName(child, L"ContentPresenter")) &&
        (child = FindChildByName(child, L"ContentGrid"))) {
        EnumChildElements(child, [](FrameworkElement child) {
            auto className = winrt::get_class_name(child);
            if (className == L"SystemTray.ImageIconContent") {
                auto containerGrid = FindChildByName(child, L"ContainerGrid")
                                         .try_as<Controls::Grid>();
                if (containerGrid) {
                    Wh_Log(L"Setting Padding=0 for ContainerGrid");
                    containerGrid.Padding(Thickness{});
                }
            } else {
                Wh_Log(L"Unsupported class name %s of child",
                       className.c_str());
            }

            return false;
        });
    }
}

void ApplyNotifyIconViewStyle(FrameworkElement notifyIconViewElement,
                              int width) {
    Wh_Log(L"Setting MinWidth=%d for NotifyIconView", width);
    notifyIconViewElement.MinWidth(width);
    notifyIconViewElement.MinHeight(std::max(width, 24));

    FrameworkElement child = notifyIconViewElement;
    if ((child = FindChildByName(child, L"ContainerGrid")) &&
        (child = FindChildByName(child, L"ContentPresenter")) &&
        (child = FindChildByName(child, L"ContentGrid"))) {
        EnumChildElements(child, [width](FrameworkElement child) {
            auto className = winrt::get_class_name(child);
            if (className == L"SystemTray.TextIconContent" ||
                className == L"SystemTray.ImageIconContent") {
                auto containerGrid = FindChildByName(child, L"ContainerGrid")
                                         .try_as<Controls::Grid>();
                if (containerGrid) {
                    Wh_Log(L"Setting Padding=0 for ContainerGrid");
                    containerGrid.Padding(Thickness{});
                }
            } else if (className == L"SystemTray.LanguageTextIconContent") {
                child.Width(std::numeric_limits<double>::quiet_NaN());

                // Every language has a different width. ENG is 24. Default
                // width is 44.
                double minWidth = width + 12;
                Wh_Log(L"Setting MinWidth=%f for LanguageTextIconContent",
                       minWidth);
                child.MinWidth(minWidth);
            } else {
                Wh_Log(L"Unsupported class name %s of child",
                       className.c_str());
            }

            return false;
        });
    }
}

bool ShouldGridStackPanelChild(FrameworkElement child,
                               bool excludeControlCenterChildren) {
    if (!FindChildByName(child, L"NotifyItemIcon") &&
        !FindDescendantByClassName(child, L"SystemTray.NotifyIconView") &&
        !FindChildByName(child, L"SystemTrayIcon")) {
        return false;
    }

    if (!excludeControlCenterChildren) {
        return true;
    }

    FrameworkElement systemTrayIcon =
        FindChildByName(child, L"SystemTrayIcon");
    if (!systemTrayIcon) {
        return true;
    }

    return !IsChildOfElementByName(systemTrayIcon, L"ControlCenterButton");
}

void ApplyTrayGridVerticalBounds(FrameworkElement stackPanel,
                                 int rows,
                                 int itemHeight,
                                 bool expandVerticalBounds) {
    FrameworkElement element = stackPanel;
    for (int depth = 0; depth < 12 && element; depth++) {
        auto elementDp = element.as<DependencyObject>();

        if (rows > 1 && expandVerticalBounds) {
            int taskbarHeight = GetEffectiveTaskbarLogicalHeight();
            double layoutHeight = static_cast<double>(
                std::max(taskbarHeight, itemHeight * rows));
            elementDp.ClearValue(FrameworkElement::MaxHeightProperty());
            elementDp.ClearValue(UIElement::ClipProperty());
            if (depth == 0) {
                element.MinHeight(layoutHeight);
                element.Height(layoutHeight);
                element.VerticalAlignment(VerticalAlignment::Stretch);
            } else {
                element.MinHeight(layoutHeight);
                element.Height(layoutHeight);
                element.VerticalAlignment(VerticalAlignment::Stretch);
            }
        } else {
            elementDp.ClearValue(FrameworkElement::MinHeightProperty());
            elementDp.ClearValue(FrameworkElement::HeightProperty());
            elementDp.ClearValue(FrameworkElement::MaxHeightProperty());
            elementDp.ClearValue(UIElement::ClipProperty());
            elementDp.ClearValue(
                FrameworkElement::VerticalAlignmentProperty());
            if (depth == 0) {
                element.VerticalAlignment(VerticalAlignment::Center);
            }
        }

        std::wstring elementName = element.Name().c_str();
        std::wstring className = winrt::get_class_name(element).c_str();
        element = Media::VisualTreeHelper::GetParent(element)
                      .try_as<FrameworkElement>();
    }
}

struct TrayGridChildInfo {
    FrameworkElement presenter{nullptr};
    FrameworkElement iconView{nullptr};
    int visualIndex = 0;
    bool eligible = false;
    bool ready = false;
    std::wstring presenterAutomationName;
    std::wstring iconAutomationName;
    std::wstring iconToolTipText;
    std::wstring renderDescription;
};

std::wstring GetAutomationNameForDiag(FrameworkElement element) {
    if (!element) {
        return L"";
    }

    try {
        return std::wstring(
            winrt::Windows::UI::Xaml::Automation::AutomationProperties::GetName(
                element)
                .c_str());
    } catch (...) {
        return L"";
    }
}

std::wstring GetToolTipTextForDiag(FrameworkElement element) {
    if (!element) {
        return L"";
    }

    try {
        auto toolTipValue =
            Controls::ToolTipService::GetToolTip(element.as<DependencyObject>());
        if (!toolTipValue) {
            return L"";
        }

        auto toolTip = toolTipValue.try_as<Controls::ToolTip>();
        if (toolTip) {
            auto content = toolTip.Content();
            if (!content) {
                return L"";
            }

            auto text = winrt::unbox_value_or<winrt::hstring>(content, L"");
            if (!text.empty()) {
                return std::wstring(text.c_str());
            }

            auto textBlock = content.try_as<Controls::TextBlock>();
            if (textBlock) {
                return std::wstring(textBlock.Text().c_str());
            }

            auto contentElement = content.try_as<FrameworkElement>();
            if (contentElement) {
                return GetAutomationNameForDiag(contentElement);
            }

            return L"";
        }

        auto text = winrt::unbox_value_or<winrt::hstring>(toolTipValue, L"");
        return std::wstring(text.c_str());
    } catch (...) {
        return L"";
    }
}

FrameworkElement FindTrayGridIconElement(FrameworkElement child) {
    FrameworkElement icon = FindChildByName(child, L"NotifyItemIcon");
    if (!icon) {
        icon = FindDescendantByClassName(child, L"SystemTray.NotifyIconView");
    }
    if (!icon) {
        icon = FindChildByName(child, L"SystemTrayIcon");
    }

    return icon;
}

bool HasRealLayoutSize(FrameworkElement element) {
    return element && element.IsLoaded() &&
           element.Visibility() == Visibility::Visible &&
           element.ActualWidth() > 0.5 && element.ActualHeight() > 0.5;
}

bool HasVisibleOpacity(FrameworkElement element) {
    try {
        return element.Opacity() > 0.01;
    } catch (...) {
        return true;
    }
}

bool IsVisibleThroughAncestorChain(FrameworkElement element,
                                   FrameworkElement stopAt) {
    FrameworkElement current = element;
    while (current) {
        if (!current.IsLoaded() ||
            current.Visibility() != Visibility::Visible ||
            !HasVisibleOpacity(current)) {
            return false;
        }

        if (GetFrameworkElementKey(current) == GetFrameworkElementKey(stopAt)) {
            return true;
        }

        current = Media::VisualTreeHelper::GetParent(current)
                      .try_as<FrameworkElement>();
    }

    return false;
}

bool IsRenderableTrayIconElement(FrameworkElement element,
                                 FrameworkElement iconView) {
    if (!HasRealLayoutSize(element) ||
        !IsVisibleThroughAncestorChain(element, iconView)) {
        return false;
    }

    std::wstring className = winrt::get_class_name(element).c_str();
    if (className == L"SystemTray.NotifyIconView" ||
        className == L"SystemTray.ImageIconContent" ||
        className == L"SystemTray.TextIconContent" ||
        className == L"SystemTray.LanguageTextIconContent" ||
        className == L"Windows.UI.Xaml.Controls.ContentPresenter" ||
        className == L"Windows.UI.Xaml.Controls.Grid" ||
        className == L"Windows.UI.Xaml.Controls.Border" ||
        className == L"Windows.UI.Xaml.Controls.StackPanel" ||
        className == L"Windows.UI.Xaml.Controls.Canvas") {
        return false;
    }

    if (className == L"Windows.UI.Xaml.Controls.Image") {
        auto image = element.try_as<Controls::Image>();
        if (!image || !image.Source()) {
            return false;
        }
    }

    return className.find(L"Image") != std::wstring::npos ||
           className.find(L"Icon") != std::wstring::npos ||
           className.find(L"Text") != std::wstring::npos ||
           className.find(L"Path") != std::wstring::npos ||
           className.find(L"Glyph") != std::wstring::npos ||
           className.find(L"Bitmap") != std::wstring::npos ||
           className.find(L"Svg") != std::wstring::npos ||
           className.find(L"Font") != std::wstring::npos;
}

bool HasRenderableTrayIconContent(FrameworkElement iconView) {
    if (!iconView) {
        return false;
    }

    if (IsRenderableTrayIconElement(iconView, iconView)) {
        return true;
    }

    bool found = false;
    EnumDescendantElements(iconView, [&found, iconView](
                                         FrameworkElement child, int) {
        if (IsRenderableTrayIconElement(child, iconView)) {
            found = true;
            return true;
        }

        return false;
    });

    return found;
}

std::wstring DescribeRenderableTrayIconContent(FrameworkElement iconView) {
    if (!iconView) {
        return L"";
    }

    std::wstringstream stream;
    int count = 0;
    EnumDescendantElements(iconView, [&stream, &count, iconView](
                                         FrameworkElement child, int depth) {
        if (!IsRenderableTrayIconElement(child, iconView)) {
            return false;
        }

        if (count > 0) {
            stream << L" | ";
        }
        stream << L"d" << depth << L":" << winrt::get_class_name(child).c_str()
               << L"#" << child.Name().c_str() << L"="
               << child.ActualWidth() << L"x" << child.ActualHeight()
               << L" op=" << child.Opacity();
        auto image = child.try_as<Controls::Image>();
        if (image) {
            stream << L" src=" << (image.Source() ? 1 : 0);
        }
        count++;
        return count >= 4;
    });

    if (count == 0) {
        return L"none";
    }

    return stream.str();
}

bool IsTrayGridIconReady(FrameworkElement presenter,
                         FrameworkElement iconView) {
    return HasRealLayoutSize(presenter) && HasRealLayoutSize(iconView) &&
           HasRenderableTrayIconContent(iconView);
}

void ResetTrayGridChildLayout(FrameworkElement child, int rows) {
    auto childDp = child.as<DependencyObject>();
    childDp.ClearValue(FrameworkElement::HeightProperty());
    childDp.ClearValue(FrameworkElement::MaxHeightProperty());
    childDp.ClearValue(FrameworkElement::WidthProperty());
    childDp.ClearValue(FrameworkElement::MinWidthProperty());
    childDp.ClearValue(FrameworkElement::MaxWidthProperty());
    childDp.ClearValue(FrameworkElement::MarginProperty());
    childDp.ClearValue(FrameworkElement::VerticalAlignmentProperty());
    childDp.ClearValue(UIElement::ClipProperty());
    childDp.ClearValue(UIElement::RenderTransformProperty());
    if (rows <= 1) {
        child.VerticalAlignment(VerticalAlignment::Center);
    }
}

void CollapsePendingTrayGridChild(FrameworkElement child) {
    child.Width(0);
    child.MinWidth(0);
    child.MaxWidth(0);
    child.Height(0);
    child.MaxHeight(0);
    child.VerticalAlignment(VerticalAlignment::Top);

    Media::RectangleGeometry clip;
    clip.Rect(winrt::Windows::Foundation::Rect{0.0f, 0.0f, 0.0f, 0.0f});
    child.Clip(clip);

    child.as<DependencyObject>().ClearValue(UIElement::RenderTransformProperty());
}

void SetOrdinaryTrayGridGate(FrameworkElement stackPanel,
                             bool hidden,
                             PCWSTR reason,
                             int rows,
                             int readyChildren,
                             int pendingChildren) {
    if (!stackPanel) {
        return;
    }

    try {
        stackPanel.Opacity(hidden ? 0.0 : 1.0);
        stackPanel.IsHitTestVisible(!hidden);
        TrayDiagLog(
            L"[grid-gate] hidden=%d reason=%s rows=%d ready=%d pending=%d "
            L"stackActual=%.1fx%.1f stackName=%s",
            hidden, reason, rows, readyChildren, pendingChildren,
            stackPanel.ActualWidth(), stackPanel.ActualHeight(),
            stackPanel.Name().c_str());
    } catch (...) {
        TrayDiagLog(L"[grid-gate] failed hidden=%d reason=%s", hidden, reason);
    }
}

bool IsOrdinaryTrayGridGateHidden(FrameworkElement stackPanel) {
    if (!stackPanel) {
        return false;
    }

    try {
        return stackPanel.Opacity() <= 0.01;
    } catch (...) {
        return false;
    }
}

std::wstring FormatRowCountsForLog(const std::vector<int>& rowCounts,
                                   int rows) {
    std::wstringstream stream;
    for (int i = 0; i < rows; i++) {
        if (i > 0) {
            stream << L"/";
        }
        stream << rowCounts[i];
    }

    return stream.str();
}

winrt::Windows::Foundation::Point GetElementOffsetWithin(
    FrameworkElement element,
    FrameworkElement relativeTo) {
    try {
        auto transform = element.TransformToVisual(relativeTo);
        return transform.TransformPoint(
            winrt::Windows::Foundation::Point{0.0f, 0.0f});
    } catch (...) {
        return winrt::Windows::Foundation::Point{0.0f, 0.0f};
    }
}

void LogNotificationAreaWrapGridChildren(FrameworkElement panelRoot,
                                         int rows,
                                         int width,
                                         int itemHeight) {
    if (!panelRoot) {
        return;
    }

    try {
        int childrenCount = Media::VisualTreeHelper::GetChildrenCount(panelRoot);
        TrayDiagLog(
            L"[real-layout-children] panelClass=%s panelActual=%.1fx%.1f "
            L"rows=%d width=%d itemHeight=%d children=%d",
            winrt::get_class_name(panelRoot).c_str(), panelRoot.ActualWidth(),
            panelRoot.ActualHeight(), rows, width, itemHeight, childrenCount);

        int logged = 0;
        for (int i = 0; i < childrenCount && logged < 36; i++) {
            auto child = Media::VisualTreeHelper::GetChild(panelRoot, i)
                             .try_as<FrameworkElement>();
            if (!child) {
                continue;
            }

            auto iconView = FindTrayGridIconElement(child);
            auto childOffset = GetElementOffsetWithin(child, panelRoot);
            auto iconOffset = iconView
                                  ? GetElementOffsetWithin(iconView, panelRoot)
                                  : winrt::Windows::Foundation::Point{0.0f,
                                                                      0.0f};
            std::wstring childClass = winrt::get_class_name(child).c_str();
            std::wstring iconClass =
                iconView ? winrt::get_class_name(iconView).c_str() : L"<none>";
            std::wstring automationName =
                iconView ? GetAutomationNameForDiag(iconView)
                         : GetAutomationNameForDiag(child);
            std::wstring tooltip =
                iconView ? GetToolTipTextForDiag(iconView)
                         : GetToolTipTextForDiag(child);
            std::wstring renderDescription =
                DescribeRenderableTrayIconContent(iconView ? iconView : child);
            auto renderTransform = child.RenderTransform();
            std::wstring renderTransformClass =
                renderTransform ? winrt::get_class_name(renderTransform).c_str()
                                : L"<none>";

            TrayDiagLog(
                L"[real-layout-child] index=%d childClass=%s childName=%s "
                L"childActual=%.1fx%.1f childOffset=%.1f,%.1f "
                L"childMin=%.1fx%.1f childMax=%.1fx%.1f "
                L"childVerticalAlignment=%d childRenderTransform=%s "
                L"iconClass=%s iconName=%s iconActual=%.1fx%.1f "
                L"iconOffset=%.1f,%.1f automation=%s tooltip=%s render=%s",
                i, childClass.c_str(), child.Name().c_str(),
                child.ActualWidth(), child.ActualHeight(), childOffset.X,
                childOffset.Y, child.MinWidth(), child.MinHeight(),
                child.MaxWidth(), child.MaxHeight(),
                static_cast<int>(child.VerticalAlignment()),
                renderTransformClass.c_str(), iconClass.c_str(),
                iconView ? iconView.Name().c_str() : L"<none>",
                iconView ? iconView.ActualWidth() : 0.0,
                iconView ? iconView.ActualHeight() : 0.0, iconOffset.X,
                iconOffset.Y, automationName.c_str(), tooltip.c_str(),
                renderDescription.c_str());
            logged++;
        }
    } catch (winrt::hresult_error const& e) {
        TrayDiagLog(L"[real-layout-children] failed hr=0x%08X %s",
                    static_cast<unsigned>(e.code()), e.message().c_str());
    } catch (...) {
        TrayDiagLog(L"[real-layout-children] failed");
    }
}

uintptr_t GetTrayGridChildKey(const TrayGridChildInfo& child) {
    if (child.iconView) {
        return GetFrameworkElementKey(child.iconView);
    }

    return GetFrameworkElementKey(child.presenter);
}

std::wstring BuildTrayReadyFingerprint(
    const std::vector<TrayGridChildInfo>& children,
    std::vector<uintptr_t>& readyKeys) {
    std::wstringstream stream;
    for (auto const& child : children) {
        if (!child.eligible || !child.ready) {
            continue;
        }

        uintptr_t key = GetTrayGridChildKey(child);
        if (!key) {
            continue;
        }

        if (!readyKeys.empty()) {
            stream << L";";
        }
        stream << std::hex << key;
        readyKeys.push_back(key);
    }

    return stream.str();
}

void PromoteTrayGridSnapshot(const std::wstring& fingerprint,
                             const std::vector<uintptr_t>& readyKeys) {
    g_lastAppliedTrayGridFingerprint = fingerprint;
    g_lastAppliedTrayGridIconKeys.clear();
    for (uintptr_t key : readyKeys) {
        g_lastAppliedTrayGridIconKeys.insert(key);
    }
}

bool IsTrayGridSnapshotAccepted(const std::wstring& fingerprint,
                                const std::vector<uintptr_t>& readyKeys,
                                int rows) {
    if (rows <= 1) {
        PromoteTrayGridSnapshot(fingerprint, readyKeys);
        g_pendingTrayGridFingerprint.clear();
        g_pendingTrayGridFingerprintTick = 0;
        return true;
    }

    if (fingerprint == g_lastAppliedTrayGridFingerprint) {
        return true;
    }

    if (readyKeys.size() < g_lastAppliedTrayGridIconKeys.size()) {
        PromoteTrayGridSnapshot(fingerprint, readyKeys);
        g_pendingTrayGridFingerprint.clear();
        g_pendingTrayGridFingerprintTick = 0;
        return true;
    }

    DWORD now = GetTickCount();
    constexpr DWORD kReadySnapshotDebounceMs = 1500;
    if (fingerprint != g_pendingTrayGridFingerprint) {
        g_pendingTrayGridFingerprint = fingerprint;
        g_pendingTrayGridFingerprintTick = now;
        RequestNotificationIconRowsRefresh();
        return false;
    }

    if (now - g_pendingTrayGridFingerprintTick < kReadySnapshotDebounceMs) {
        RequestNotificationIconRowsRefresh();
        return false;
    }

    PromoteTrayGridSnapshot(fingerprint, readyKeys);
    g_pendingTrayGridFingerprint.clear();
    g_pendingTrayGridFingerprintTick = 0;
    return true;
}

void ApplyNotifyIconsStackPanelGridStyle(FrameworkElement stackPanel,
                                         int rows,
                                         int width,
                                         bool excludeControlCenterChildren = false,
                                         bool compactPanelWidth = true,
                                         bool expandVerticalBounds = true) {
    if (rows < 1) {
        rows = 1;
    }

    auto stackPanelDp = stackPanel.as<DependencyObject>();
    stackPanelDp.ClearValue(FrameworkElement::WidthProperty());
    stackPanelDp.ClearValue(FrameworkElement::MinWidthProperty());

    std::vector<TrayGridChildInfo> children;
    int totalChildren = 0;
    int eligibleChildren = 0;
    int excludedChildren = 0;

    EnumChildElements(stackPanel, [rows, width, excludeControlCenterChildren,
                                   &children, &totalChildren,
                                   &eligibleChildren,
                                   &excludedChildren](FrameworkElement child) {
        auto childClassName = winrt::get_class_name(child);
        if (childClassName != L"Windows.UI.Xaml.Controls.ContentPresenter") {
            Wh_Log(L"Unsupported class name %s of child",
                   childClassName.c_str());
            return false;
        }

        ResetTrayGridChildLayout(child, rows);

        TrayGridChildInfo info;
        info.presenter = child;
        info.visualIndex = totalChildren++;
        info.eligible =
            ShouldGridStackPanelChild(child, excludeControlCenterChildren);

        if (info.eligible) {
            info.iconView = FindTrayGridIconElement(child);
            if (info.iconView) {
                ResetNotifyIconViewLayout(info.iconView);
            }
            eligibleChildren++;
        } else {
            excludedChildren++;
        }

        children.push_back(info);
        return false;
    });

    int itemHeight = GetEffectiveNotificationIconRowHeight(rows, width);
    ApplyTrayGridVerticalBounds(stackPanel, rows, itemHeight,
                                expandVerticalBounds);
    ForceElementLayout(stackPanel, L"pre-classify");

    int readyChildren = 0;
    int pendingChildren = 0;
    for (auto& child : children) {
        if (!child.eligible) {
            continue;
        }

        child.ready = IsTrayGridIconReady(child.presenter, child.iconView);
        child.presenterAutomationName =
            GetAutomationNameForDiag(child.presenter);
        child.iconAutomationName = GetAutomationNameForDiag(child.iconView);
        child.iconToolTipText = GetToolTipTextForDiag(child.iconView);
        child.renderDescription =
            DescribeRenderableTrayIconContent(child.iconView);
        TrayDiagLog(
            L"[grid-ready] visual=%d key=%p ready=%d presenterLoaded=%d "
            L"presenter=%.1fx%.1f iconLoaded=%d iconVisible=%d "
            L"iconActual=%.1fx%.1f renderable=%d iconClass=%s iconName=%s "
            L"presenterAutomation=\"%s\" iconAutomation=\"%s\" "
            L"iconToolTip=\"%s\" renderDesc=%s",
            child.visualIndex,
            reinterpret_cast<void*>(GetTrayGridChildKey(child)), child.ready,
            child.presenter.IsLoaded(), child.presenter.ActualWidth(),
            child.presenter.ActualHeight(),
            child.iconView ? child.iconView.IsLoaded() : false,
            child.iconView ? static_cast<int>(child.iconView.Visibility())
                           : -1,
            child.iconView ? child.iconView.ActualWidth() : 0.0,
            child.iconView ? child.iconView.ActualHeight() : 0.0,
            HasRenderableTrayIconContent(child.iconView),
            child.iconView ? winrt::get_class_name(child.iconView).c_str()
                           : L"",
            child.iconView ? child.iconView.Name().c_str() : L"",
            child.presenterAutomationName.c_str(),
            child.iconAutomationName.c_str(), child.iconToolTipText.c_str(),
            child.renderDescription.c_str());
        if (child.ready) {
            readyChildren++;
        } else {
            pendingChildren++;
        }
    }

    std::vector<uintptr_t> readyKeys;
    std::wstring readyFingerprint =
        BuildTrayReadyFingerprint(children, readyKeys);
    bool snapshotAccepted =
        IsTrayGridSnapshotAccepted(readyFingerprint, readyKeys, rows);
    int withheldReadyChildren = 0;
    if (rows > 1 && !snapshotAccepted) {
        for (auto& child : children) {
            if (!child.eligible || !child.ready) {
                continue;
            }

            uintptr_t key = GetTrayGridChildKey(child);
            if (!key || !g_lastAppliedTrayGridIconKeys.count(key)) {
                child.ready = false;
                withheldReadyChildren++;
            }
        }

        readyChildren -= withheldReadyChildren;
        pendingChildren += withheldReadyChildren;
        if (readyChildren < 0) {
            readyChildren = 0;
        }
    }

    bool hasOnlyPendingOrdinaryTrayChildren =
        eligibleChildren > 0 && readyChildren == 0 && pendingChildren > 0;
    bool hideOrdinaryTrayUntilStable =
        rows > 1 && (!snapshotAccepted || hasOnlyPendingOrdinaryTrayChildren);
    PCWSTR gateReason = hideOrdinaryTrayUntilStable
                            ? (hasOnlyPendingOrdinaryTrayChildren
                                   ? L"pending-only"
                                   : L"snapshot-not-stable")
                            : L"snapshot-stable";
    bool gateWasHidden = IsOrdinaryTrayGridGateHidden(stackPanel);
    if (rows > 1) {
        SetOrdinaryTrayGridGate(stackPanel,
                                hideOrdinaryTrayUntilStable || gateWasHidden,
                                hideOrdinaryTrayUntilStable
                                    ? gateReason
                                    : L"pre-layout-stable",
                                rows, readyChildren, pendingChildren);
    }

    int activeRows = rows > 1 ? rows : 1;
    int cols = (readyChildren + activeRows - 1) / activeRows;
    int rowFirstBaseCols = readyChildren / activeRows;
    int rowFirstExtraCols = readyChildren % activeRows;
    int rowFirstMaxCols = rowFirstBaseCols + (rowFirstExtraCols ? 1 : 0);
    int layoutHeight = std::max(GetEffectiveTaskbarLogicalHeight(),
                                itemHeight * rows);
    GridArrangement arrangement = g_settings.gridArrangement;
    int desiredWidth =
        width * (arrangement == GridArrangement::rowFirstLeftToRight ||
                         arrangement == GridArrangement::rowFirstBottomRowFirst
                     ? rowFirstMaxCols
                     : cols);

    if (rows > 1 && compactPanelWidth) {
        stackPanel.Width(desiredWidth);
        stackPanel.MinWidth(desiredWidth);
    } else {
        stackPanelDp.ClearValue(FrameworkElement::WidthProperty());
        stackPanelDp.ClearValue(FrameworkElement::MinWidthProperty());
    }

    TrayDiagLog(
        L"[grid-stack] name=%s class=%s rows=%d activeRows=%d width=%d "
        L"itemHeight=%d total=%d eligible=%d ready=%d pending=%d "
        L"withheld=%d accepted=%d excluded=%d compact=%d expand=%d "
        L"desiredWidth=%d actual=%.1fx%.1f",
        stackPanel.Name().c_str(), winrt::get_class_name(stackPanel).c_str(),
        rows, activeRows, width, itemHeight, totalChildren, eligibleChildren,
        readyChildren, pendingChildren, withheldReadyChildren, snapshotAccepted,
        excludedChildren, compactPanelWidth, expandVerticalBounds, desiredWidth,
        stackPanel.ActualWidth(), stackPanel.ActualHeight());

    int indexIter = 0;
    int collapsedPendingChildren = 0;
    std::vector<int> rowCounts(activeRows, 0);
    for (auto& child : children) {
        if (!child.eligible) {
            continue;
        }

        if (!child.ready) {
            if (rows > 1) {
                CollapsePendingTrayGridChild(child.presenter);
                collapsedPendingChildren++;
            }
            continue;
        }

        if (child.iconView) {
            ApplyNotifyIconViewStyle(child.iconView, width);
            ApplyNotifyIconViewRowBounds(child.iconView, rows, width);
        }

        if (rows > 1) {
            ApplyRowCellBounds(child.presenter, width, itemHeight);
        }
    }

    if (rows > 1) {
        ForceElementLayout(stackPanel, L"post-collapse");
    }

    for (auto& child : children) {
        if (!child.eligible || !child.ready) {
            continue;
        }

        int index = indexIter++;

        if (rows > 1) {
            int col, row;
            switch (arrangement) {
                case GridArrangement::rowFirstLeftToRight:
                case GridArrangement::rowFirstBottomRowFirst: {
                    int remainingIndex = index;
                    row = 0;
                    for (; row < activeRows; row++) {
                        int rowLength = rowFirstBaseCols +
                                        (row < rowFirstExtraCols ? 1 : 0);
                        if (remainingIndex < rowLength) {
                            break;
                        }

                        remainingIndex -= rowLength;
                    }

                    if (row >= activeRows) {
                        row = activeRows - 1;
                    }
                    col = remainingIndex;

                    if (arrangement == GridArrangement::rowFirstBottomRowFirst) {
                        row = (activeRows - 1) - row;
                    }
                    break;
                }
                case GridArrangement::columnFirstTopToBottom:
                    col = index / activeRows;
                    row = index % activeRows;
                    break;
                case GridArrangement::columnFirstBottomToTop:
                    col = index / activeRows;
                    row = (activeRows - 1) - (index % activeRows);
                    break;
                case GridArrangement::columnFirstBottomToTopRightToLeft:
                    col = (cols - 1) - (index / activeRows);
                    row = (activeRows - 1) - (index % activeRows);
                    break;
            }

            Media::TranslateTransform transform;

            double xOffset = width * (col - index);
            transform.X(xOffset);

            double yOffset =
                itemHeight * row - itemHeight * (activeRows - 1) / 2.0;
            transform.Y(yOffset);

            if (row >= 0 && row < static_cast<int>(rowCounts.size())) {
                rowCounts[row]++;
            }

            child.presenter.RenderTransform(transform);
            TrayDiagLog(
                L"[grid-child] stack=%s child=%d visual=%d row=%d col=%d "
                L"offset=%.1fx%.1f actual=%.1fx%.1f",
                stackPanel.Name().c_str(), index, child.visualIndex, row, col,
                xOffset, yOffset, child.presenter.ActualWidth(),
                child.presenter.ActualHeight());
        }
    }

    if (rows > 1 && compactPanelWidth) {
        stackPanel.Width(desiredWidth);
        stackPanel.MinWidth(desiredWidth);
    } else {
        stackPanelDp.ClearValue(FrameworkElement::WidthProperty());
        stackPanelDp.ClearValue(FrameworkElement::MinWidthProperty());
    }

    ForceElementLayout(stackPanel, L"post-transform");
    std::wstring rowCountsText = FormatRowCountsForLog(rowCounts, activeRows);
    TrayDiagLog(
        L"[grid-stack-final] name=%s rows=%d desiredWidth=%d "
        L"actual=%.1fx%.1f total=%d eligible=%d ready=%d pending=%d "
        L"withheld=%d accepted=%d collapsed=%d rowCounts=%s placed=%d",
        stackPanel.Name().c_str(), rows, desiredWidth, stackPanel.ActualWidth(),
        stackPanel.ActualHeight(), totalChildren, eligibleChildren,
        readyChildren, pendingChildren, withheldReadyChildren, snapshotAccepted,
        collapsedPendingChildren, rowCountsText.c_str(), indexIter);

    SetOrdinaryTrayGridGate(stackPanel, hideOrdinaryTrayUntilStable,
                            hideOrdinaryTrayUntilStable
                                ? gateReason
                                : L"stable-after-layout",
                            rows, readyChildren, pendingChildren);

    g_notificationAreaIconsStackPanel = stackPanel;
}

void ApplyNotifyIconsStackPanelGridStyleOfIcon(
    FrameworkElement notifyIconViewElement,
    int rows,
    int width,
    bool excludeControlCenterChildren = false,
    bool compactPanelWidth = true,
    bool expandVerticalBounds = true) {
    auto contentPresenter = FindAncestorByClassName(
        notifyIconViewElement, L"Windows.UI.Xaml.Controls.ContentPresenter");
    if (!contentPresenter) {
        TrayDiagLog(
            L"[icon-grid] loaded icon has no ContentPresenter ancestor "
            L"name=%s class=%s",
            notifyIconViewElement.Name().c_str(),
            winrt::get_class_name(notifyIconViewElement).c_str());
        return;
    }

    auto stackPanel = FindAncestorByClassName(
        contentPresenter, L"Windows.UI.Xaml.Controls.StackPanel");
    if (!stackPanel) {
        TrayDiagLog(
            L"[icon-grid] loaded icon has no StackPanel ancestor "
            L"name=%s class=%s contentPresenterActual=%.1fx%.1f",
            notifyIconViewElement.Name().c_str(),
            winrt::get_class_name(notifyIconViewElement).c_str(),
            contentPresenter.ActualWidth(), contentPresenter.ActualHeight());
        return;
    }

    ApplyNotifyIconsStackPanelGridStyle(
        stackPanel, rows, width, excludeControlCenterChildren,
        compactPanelWidth, expandVerticalBounds);
}

void ApplyNotifyIconLateLayoutRetry(FrameworkElement iconView,
                                    PCWSTR reason) {
    if (!iconView || g_unloading) {
        return;
    }

    if (winrt::get_class_name(iconView) != L"SystemTray.NotifyIconView") {
        return;
    }

    if (IsChildOfElementByClassName(iconView,
                                    L"SystemTray.NotificationAreaOverflow")) {
        ApplyNotifyIconViewOverflowStyle(iconView, g_settings.overflowIconWidth);
        return;
    }

    ApplyNotifyIconViewStyle(iconView, g_settings.notificationIconWidth);
    RequestNotificationIconRowsRefresh();

    if (g_settings.trayLayoutMode != TrayLayoutMode::legacyTransform) {
        TrayDiagLog(
            L"[icon-late-reflow] mode=%s defers to real/native panel layout "
            L"reason=%s name=%s actual=%.1fx%.1f",
            TrayLayoutModeName(g_settings.trayLayoutMode), reason,
            iconView.Name().c_str(), iconView.ActualWidth(),
            iconView.ActualHeight());
        return;
    }

    if (!IsChildOfElementByName(iconView, L"NotificationAreaIcons")) {
        TrayDiagLog(
            L"[icon-late-reflow] waiting reason=%s name=%s actual=%.1fx%.1f",
            reason, iconView.Name().c_str(), iconView.ActualWidth(),
            iconView.ActualHeight());
        return;
    }

    int rows = GetEffectiveNotificationIconRows();
    ApplyNotifyIconViewRowBounds(iconView, rows,
                                 g_settings.notificationIconWidth);
    ApplyNotifyIconsStackPanelGridStyleOfIcon(
        iconView, rows, g_settings.notificationIconWidth);
    TrayDiagLog(
        L"[icon-late-reflow] applied reason=%s rows=%d width=%d name=%s "
        L"actual=%.1fx%.1f",
        reason, rows, g_settings.notificationIconWidth, iconView.Name().c_str(),
        iconView.ActualWidth(), iconView.ActualHeight());
}

void AttachTrayContainerLayoutRetry(FrameworkElement element, PCWSTR tag) {
    if (!element || g_unloading) {
        return;
    }

    uintptr_t key = GetFrameworkElementKey(element);
    if (!key || !g_containerSizeHookedElements.insert(key).second) {
        return;
    }

    g_containerSizeChangedRevokerList.emplace_back();
    auto revokerIt = g_containerSizeChangedRevokerList.end();
    --revokerIt;

    auto weakElement = winrt::make_weak(element);
    std::wstring tagCopy = tag ? tag : L"container";

    *revokerIt = element.SizeChanged(
        winrt::auto_revoke,
        [weakElement, key, tagCopy, revokerIt](
            winrt::Windows::Foundation::IInspectable const&,
            SizeChangedEventArgs const& e) {
            if (g_unloading) {
                return;
            }

            auto liveElement = weakElement.get();
            if (!liveElement) {
                g_containerSizeHookedElements.erase(key);
                g_containerSizeChangedRevokerList.erase(revokerIt);
                return;
            }

            TrayDiagLog(
                L"[container-size] %s new=%.1fx%.1f actual=%.1fx%.1f "
                L"name=%s class=%s",
                tagCopy.c_str(), e.NewSize().Width, e.NewSize().Height,
                liveElement.ActualWidth(), liveElement.ActualHeight(),
                liveElement.Name().c_str(),
                winrt::get_class_name(liveElement).c_str());
            RequestNotificationIconRowsRefresh();
        });

    TrayDiagLog(L"[container-size] hooked %s actual=%.1fx%.1f name=%s class=%s",
                tagCopy.c_str(), element.ActualWidth(), element.ActualHeight(),
                element.Name().c_str(), winrt::get_class_name(element).c_str());
}

void AttachNotifyIconLateLayoutRetry(FrameworkElement iconView) {
    if (!iconView || g_unloading ||
        winrt::get_class_name(iconView) != L"SystemTray.NotifyIconView") {
        return;
    }

    uintptr_t key = GetFrameworkElementKey(iconView);
    if (!key || !g_sizeChangedHookedElements.insert(key).second) {
        ApplyNotifyIconLateLayoutRetry(iconView, L"constructed-existing");
        return;
    }

    auto weakIcon = winrt::make_weak(iconView);
    auto attempts = std::make_shared<int>(0);

    g_sizeChangedRevokerList.emplace_back();
    auto revokerIt = g_sizeChangedRevokerList.end();
    --revokerIt;

    *revokerIt = iconView.SizeChanged(
        winrt::auto_revoke,
        [weakIcon, key, attempts, revokerIt](
            winrt::Windows::Foundation::IInspectable const&,
            SizeChangedEventArgs const& e) {
            if (g_unloading) {
                return;
            }

            (*attempts)++;

            auto icon = weakIcon.get();
            if (!icon) {
                g_sizeChangedHookedElements.erase(key);
                g_sizeChangedRevokerList.erase(revokerIt);
                return;
            }

            TrayDiagLog(
                L"[icon-size] attempt=%d new=%.1fx%.1f name=%s",
                *attempts, e.NewSize().Width, e.NewSize().Height,
                icon.Name().c_str());
            ApplyNotifyIconLateLayoutRetry(icon, L"size-changed");

            if (*attempts >= 12) {
                g_sizeChangedHookedElements.erase(key);
                g_sizeChangedRevokerList.erase(revokerIt);
            }
        });

    ApplyNotifyIconLateLayoutRetry(iconView, L"constructed");
}

PCWSTR TrayLayoutModeName(TrayLayoutMode mode) {
    switch (mode) {
        case TrayLayoutMode::realWrapGrid:
            return L"realWrapGrid";
        case TrayLayoutMode::legacyTransform:
            return L"legacyTransform";
        case TrayLayoutMode::nativeStack:
        default:
            return L"nativeStack";
    }
}

struct NotificationAreaItemsControlProbe {
    Controls::ItemsControl itemsControl{nullptr};
    FrameworkElement itemsControlElement{nullptr};
    FrameworkElement itemsPanelRootElement{nullptr};
    std::wstring source;
    bool directCast = false;
};

NotificationAreaItemsControlProbe ProbeNotificationAreaItemsControl(
    FrameworkElement notificationAreaIcons) {
    NotificationAreaItemsControlProbe probe;

    if (!notificationAreaIcons) {
        return probe;
    }

    if (auto direct = notificationAreaIcons.try_as<Controls::ItemsControl>()) {
        probe.itemsControl = direct;
        probe.itemsControlElement = notificationAreaIcons;
        probe.source = L"direct";
        probe.directCast = true;
    }

    if (!probe.itemsControl) {
        FrameworkElement ancestor = notificationAreaIcons;
        for (int depth = 0; depth < 8 && ancestor; depth++) {
            ancestor = Media::VisualTreeHelper::GetParent(ancestor)
                           .try_as<FrameworkElement>();
            if (!ancestor) {
                break;
            }

            if (auto ancestorItemsControl =
                    ancestor.try_as<Controls::ItemsControl>()) {
                probe.itemsControl = ancestorItemsControl;
                probe.itemsControlElement = ancestor;
                std::wstringstream source;
                source << L"ancestor-depth-" << (depth + 1);
                probe.source = source.str();
                break;
            }
        }
    }

    if (!probe.itemsControl) {
        EnumDescendantElements(
            notificationAreaIcons,
            [&probe](FrameworkElement element, int depth) {
                if (auto descendantItemsControl =
                        element.try_as<Controls::ItemsControl>()) {
                    probe.itemsControl = descendantItemsControl;
                    probe.itemsControlElement = element;
                    std::wstringstream source;
                    source << L"descendant-depth-" << depth;
                    probe.source = source.str();
                    return true;
                }

                return false;
            });
    }

    if (probe.itemsControl) {
        try {
            probe.itemsPanelRootElement =
                probe.itemsControl.ItemsPanelRoot().try_as<FrameworkElement>();
        } catch (...) {
            TrayDiagLog(
                L"[real-layout] ItemsPanelRoot read failed source=%s",
                probe.source.c_str());
        }
    }

    return probe;
}

void LogNotificationAreaItemsControlProbe(
    FrameworkElement notificationAreaIcons,
    NotificationAreaItemsControlProbe const& probe,
    int rows,
    int width) {
    PCWSTR itemsControlName = L"<none>";
    PCWSTR itemsControlClass = L"<none>";
    double itemsControlActualWidth = 0.0;
    double itemsControlActualHeight = 0.0;
    int itemsControlChildren = 0;

    std::wstring itemsControlNameStorage;
    std::wstring itemsControlClassStorage;
    if (probe.itemsControlElement) {
        itemsControlNameStorage = probe.itemsControlElement.Name().c_str();
        itemsControlName = itemsControlNameStorage.c_str();
        itemsControlClassStorage = winrt::get_class_name(probe.itemsControlElement);
        itemsControlClass = itemsControlClassStorage.c_str();
        itemsControlActualWidth = probe.itemsControlElement.ActualWidth();
        itemsControlActualHeight = probe.itemsControlElement.ActualHeight();
        itemsControlChildren = CountDirectChildren(probe.itemsControlElement);
    }

    PCWSTR panelName = L"<none>";
    PCWSTR panelClass = L"<none>";
    double panelActualWidth = 0.0;
    double panelActualHeight = 0.0;
    int panelChildren = 0;

    std::wstring panelNameStorage;
    std::wstring panelClassStorage;
    if (probe.itemsPanelRootElement) {
        panelNameStorage = probe.itemsPanelRootElement.Name().c_str();
        panelName = panelNameStorage.c_str();
        panelClassStorage = winrt::get_class_name(probe.itemsPanelRootElement);
        panelClass = panelClassStorage.c_str();
        panelActualWidth = probe.itemsPanelRootElement.ActualWidth();
        panelActualHeight = probe.itemsPanelRootElement.ActualHeight();
        panelChildren = CountDirectChildren(probe.itemsPanelRootElement);
    }

    TrayDiagLog(
        L"[real-layout] targetName=%s targetClass=%s directItemsControl=%d "
        L"itemsControlFound=%d source=%s itemsControlName=%s "
        L"itemsControlClass=%s itemsControlActual=%.1fx%.1f "
        L"itemsControlChildren=%d itemsPanelRootName=%s "
        L"itemsPanelRootClass=%s itemsPanelRootActual=%.1fx%.1f "
        L"itemsPanelRootChildren=%d mode=%s rows=%d width=%d",
        notificationAreaIcons.Name().c_str(),
        winrt::get_class_name(notificationAreaIcons).c_str(),
        probe.directCast, probe.itemsControl ? 1 : 0, probe.source.c_str(),
        itemsControlName, itemsControlClass, itemsControlActualWidth,
        itemsControlActualHeight, itemsControlChildren, panelName, panelClass,
        panelActualWidth, panelActualHeight, panelChildren,
        TrayLayoutModeName(g_settings.trayLayoutMode), rows, width);
}

int ResolveNotificationAreaRowsFromActualHeight(
    FrameworkElement notificationAreaIcons,
    NotificationAreaItemsControlProbe const& probe,
    int rows) {
    rows = ClampNotificationIconRows(rows);

    if (GetEffectiveTrayLayoutMode() != TrayLayoutMode::realWrapGrid ||
        GetFreshLiveNotificationIconRowsHint() > 0) {
        return rows;
    }

    int targetRowsFromTaskbarHeight =
        RowsFromLogicalHeight(GetEffectiveTaskbarLogicalHeight());
    if (targetRowsFromTaskbarHeight <= rows) {
        return rows;
    }

    double targetActualHeight =
        notificationAreaIcons ? notificationAreaIcons.ActualHeight() : 0.0;
    double itemsControlActualHeight =
        probe.itemsControlElement ? probe.itemsControlElement.ActualHeight()
                                  : 0.0;
    double panelRootActualHeight =
        probe.itemsPanelRootElement ? probe.itemsPanelRootElement.ActualHeight()
                                    : 0.0;
    double actualHeight =
        std::fmax(targetActualHeight,
                  std::fmax(itemsControlActualHeight, panelRootActualHeight));
    int actualRows = std::min(RowsFromLogicalHeight(actualHeight),
                              targetRowsFromTaskbarHeight);
    if (actualRows > rows) {
        TrayDiagLog(
            L"[rows-resolve-xaml] realWrapGrid rows=%d actualRows=%d "
            L"targetRowsFromTaskbarHeight=%d targetActualHeight=%.1f "
            L"itemsControlActualHeight=%.1f panelRootActualHeight=%.1f",
            rows, actualRows, targetRowsFromTaskbarHeight,
            targetActualHeight, itemsControlActualHeight, panelRootActualHeight);
        return actualRows;
    }

    return rows;
}

Controls::ItemsPanelTemplate LoadItemsPanelTemplateFromXaml(
    std::wstring const& xaml,
    PCWSTR reason) {
    try {
        return Markup::XamlReader::Load(xaml)
            .try_as<Controls::ItemsPanelTemplate>();
    } catch (winrt::hresult_error const& e) {
        TrayDiagLog(L"[real-layout] XamlReader failed reason=%s hr=0x%08X %s",
                    reason, static_cast<unsigned>(e.code()),
                    e.message().c_str());
    } catch (...) {
        TrayDiagLog(L"[real-layout] XamlReader failed reason=%s", reason);
    }

    return nullptr;
}

Controls::ItemsPanelTemplate CreateWrapGridItemsPanelTemplate(int rows,
                                                              int width,
                                                              int itemHeight) {
    std::wstringstream xaml;
    xaml << L"<ItemsPanelTemplate "
         << L"xmlns=\"http://schemas.microsoft.com/winfx/2006/xaml/presentation\">"
         << L"<WrapGrid Orientation=\"Vertical\" MaximumRowsOrColumns=\""
         << rows << L"\" ItemWidth=\"" << width << L"\" ItemHeight=\""
         << itemHeight << L"\"/>"
         << L"</ItemsPanelTemplate>";

    return LoadItemsPanelTemplateFromXaml(xaml.str(), L"wrap-grid");
}

Controls::ItemsPanelTemplate CreateNativeStackPanelTemplate() {
    return LoadItemsPanelTemplateFromXaml(
        L"<ItemsPanelTemplate "
        L"xmlns=\"http://schemas.microsoft.com/winfx/2006/xaml/presentation\">"
        L"<StackPanel Orientation=\"Horizontal\"/>"
        L"</ItemsPanelTemplate>",
        L"native-stack");
}

void ClearNotificationAreaLegacyLayout(FrameworkElement notificationAreaIcons) {
    if (!notificationAreaIcons) {
        return;
    }

    EnumDescendantElements(
        notificationAreaIcons,
        [](FrameworkElement element, int) {
            auto className = winrt::get_class_name(element);
            if (className ==
                    L"Windows.UI.Xaml.Controls.ContentPresenter" ||
                className == L"SystemTray.NotifyIconView") {
                auto elementDp = element.as<DependencyObject>();
                elementDp.ClearValue(FrameworkElement::WidthProperty());
                elementDp.ClearValue(FrameworkElement::MinWidthProperty());
                elementDp.ClearValue(FrameworkElement::MaxWidthProperty());
                elementDp.ClearValue(FrameworkElement::HeightProperty());
                elementDp.ClearValue(FrameworkElement::MinHeightProperty());
                elementDp.ClearValue(FrameworkElement::MaxHeightProperty());
                elementDp.ClearValue(
                    FrameworkElement::VerticalAlignmentProperty());
                elementDp.ClearValue(UIElement::ClipProperty());
                elementDp.ClearValue(UIElement::RenderTransformProperty());
            }

            return false;
        });
}

void RestoreNotificationAreaNativeItemsPanel(
    Controls::ItemsControl itemsControl,
    PCWSTR reason) {
    if (!itemsControl) {
        return;
    }

    try {
        if (g_originalNotificationAreaItemsPanelTemplate) {
            itemsControl.ItemsPanel(g_originalNotificationAreaItemsPanelTemplate);
            TrayDiagLog(
                L"[real-layout] restored original ItemsPanel reason=%s",
                reason);
        } else {
            FrameworkElement panelRoot = nullptr;
            try {
                panelRoot =
                    itemsControl.ItemsPanelRoot().try_as<FrameworkElement>();
            } catch (...) {
            }

            bool currentPanelIsWrapGrid =
                panelRoot &&
                winrt::get_class_name(panelRoot) ==
                    L"Windows.UI.Xaml.Controls.WrapGrid";
            if (currentPanelIsWrapGrid) {
                if (auto nativeTemplate = CreateNativeStackPanelTemplate()) {
                    itemsControl.ItemsPanel(nativeTemplate);
                    TrayDiagLog(
                        L"[real-layout] restored synthetic native StackPanel "
                        L"reason=%s oldPanel=%s",
                        reason, winrt::get_class_name(panelRoot).c_str());
                }
            } else {
                TrayDiagLog(
                    L"[real-layout] native ItemsPanel left unchanged reason=%s "
                    L"panel=%s",
                    reason,
                    panelRoot ? winrt::get_class_name(panelRoot).c_str()
                              : L"<none>");
            }
        }

        if (auto itemsControlElement =
                itemsControl.try_as<FrameworkElement>()) {
            ApplyTrayGridVerticalBounds(itemsControlElement, 1, 0, false);
            auto elementDp = itemsControlElement.as<DependencyObject>();
            elementDp.ClearValue(FrameworkElement::HeightProperty());
            elementDp.ClearValue(FrameworkElement::MinHeightProperty());
            elementDp.ClearValue(FrameworkElement::MaxHeightProperty());
            ForceElementLayout(itemsControlElement, L"restore-native-panel");
        }
    } catch (winrt::hresult_error const& e) {
        TrayDiagLog(
            L"[real-layout] restore native ItemsPanel failed reason=%s "
            L"hr=0x%08X %s",
            reason, static_cast<unsigned>(e.code()), e.message().c_str());
    } catch (...) {
        TrayDiagLog(L"[real-layout] restore native ItemsPanel failed reason=%s",
                    reason);
    }
}

bool ApplyNotificationAreaWrapGridLayout(
    FrameworkElement notificationAreaIcons,
    NotificationAreaItemsControlProbe const& probe,
    int rows,
    int width) {
    if (!probe.itemsControl) {
        TrayDiagLog(
            L"[real-layout] skip WrapGrid: NotificationAreaIcons has no "
            L"ItemsControl");
        return false;
    }

    rows =
        ResolveNotificationAreaRowsFromActualHeight(notificationAreaIcons, probe,
                                                   rows);

    if (rows < 2) {
        RestoreNotificationAreaNativeItemsPanel(probe.itemsControl,
                                                L"rows-less-than-two");
        return false;
    }

    int compactItemHeight = GetEffectiveNotificationIconRowHeight(rows, width);
    int itemWidth = std::max(width, 32);
    int targetPanelHeight =
        std::max(compactItemHeight * rows, GetEffectiveTaskbarLogicalHeight());
    int itemHeight = GetRealWrapGridNotificationIconSlotHeight(
        rows, targetPanelHeight, compactItemHeight);
    targetPanelHeight = std::max(itemHeight * rows, targetPanelHeight);

    FrameworkElement existingPanelRoot = nullptr;
    Controls::WrapGrid existingWrapGrid = nullptr;
    try {
        existingPanelRoot =
            probe.itemsControl.ItemsPanelRoot().try_as<FrameworkElement>();
        existingWrapGrid = existingPanelRoot.try_as<Controls::WrapGrid>();
    } catch (...) {
    }

    Controls::ItemsPanelTemplate wrapGridTemplate = nullptr;
    if (!existingWrapGrid) {
        wrapGridTemplate =
            CreateWrapGridItemsPanelTemplate(rows, itemWidth, itemHeight);
        if (!wrapGridTemplate) {
            return false;
        }
    }

    try {
        if (!g_originalNotificationAreaItemsPanelTemplate) {
            g_originalNotificationAreaItemsPanelTemplate =
                probe.itemsControl.ItemsPanel();
            TrayDiagLog(L"[real-layout] cached original ItemsPanel hasValue=%d",
                        g_originalNotificationAreaItemsPanelTemplate ? 1 : 0);
        }

        ClearNotificationAreaLegacyLayout(notificationAreaIcons);
        EnumDescendantElements(
            notificationAreaIcons,
            [width](FrameworkElement element, int) {
                if (winrt::get_class_name(element) ==
                    L"SystemTray.NotifyIconView") {
                    ApplyNotifyIconViewStyle(element, width);
                }

                return false;
            });
        if (wrapGridTemplate) {
            probe.itemsControl.ItemsPanel(wrapGridTemplate);
        } else {
            TrayDiagLog(
                L"[real-layout] reusing existing WrapGrid rows=%d width=%d "
                L"itemWidth=%d itemHeight=%d panelActual=%.1fx%.1f",
                rows, width, itemWidth, itemHeight,
                existingPanelRoot ? existingPanelRoot.ActualWidth() : 0.0,
                existingPanelRoot ? existingPanelRoot.ActualHeight() : 0.0);
        }

        if (auto itemsControlElement =
                probe.itemsControl.try_as<FrameworkElement>()) {
            ApplyTrayGridVerticalBounds(itemsControlElement, rows, itemHeight,
                                        true);
            itemsControlElement.Height(targetPanelHeight);
            itemsControlElement.MinHeight(targetPanelHeight);
            ForceElementLayout(itemsControlElement, L"apply-wrapgrid-panel");
        }

        auto panelRoot = probe.itemsControl.ItemsPanelRoot()
                             .try_as<FrameworkElement>();
        auto wrapGrid = panelRoot.try_as<Controls::WrapGrid>();
        if (wrapGrid) {
            wrapGrid.Orientation(Controls::Orientation::Vertical);
            wrapGrid.MaximumRowsOrColumns(rows);
            wrapGrid.ItemWidth(itemWidth);
            wrapGrid.ItemHeight(itemHeight);
            panelRoot.Height(targetPanelHeight);
            panelRoot.MinHeight(targetPanelHeight);
            panelRoot.VerticalAlignment(VerticalAlignment::Stretch);
            ForceElementLayout(panelRoot, L"apply-wrapgrid-root");
        } else if (panelRoot) {
            TrayDiagLog(
                L"[real-layout] WrapGrid template pending rows=%d "
                L"panelClass=%s panelActual=%.1fx%.1f",
                rows, winrt::get_class_name(panelRoot).c_str(),
                panelRoot.ActualWidth(), panelRoot.ActualHeight());
            RequestNotificationIconRowsRefresh();
        }

        TrayDiagLog(
            L"[real-layout] applied WrapGrid rows=%d width=%d itemWidth=%d "
            L"itemHeight=%d compactItemHeight=%d targetHeight=%d "
            L"panelClass=%s isWrapGrid=%d panelActual=%.1fx%.1f "
            L"panelChildren=%d",
            rows, width, itemWidth, itemHeight, compactItemHeight,
            targetPanelHeight,
            panelRoot ? winrt::get_class_name(panelRoot).c_str() : L"<none>",
            wrapGrid ? 1 : 0, panelRoot ? panelRoot.ActualWidth() : 0.0,
            panelRoot ? panelRoot.ActualHeight() : 0.0,
            CountDirectChildren(panelRoot));

        if (wrapGrid) {
            LogNotificationAreaWrapGridChildren(panelRoot, rows, width,
                                                itemHeight);
        }

        AttachTrayContainerLayoutRetry(notificationAreaIcons,
                                       L"NotificationAreaIcons");
        if (panelRoot) {
            AttachTrayContainerLayoutRetry(panelRoot,
                                           L"NotificationAreaItemsPanelRoot");
            g_notificationAreaIconsStackPanel = panelRoot;
        }

        return true;
    } catch (winrt::hresult_error const& e) {
        TrayDiagLog(L"[real-layout] apply WrapGrid failed hr=0x%08X %s",
                    static_cast<unsigned>(e.code()), e.message().c_str());
    } catch (...) {
        TrayDiagLog(L"[real-layout] apply WrapGrid failed");
    }

    return false;
}

bool ApplyNotifyIconsStyle(FrameworkElement notificationAreaIcons,
                           int rows,
                           int width) {
    auto probe = ProbeNotificationAreaItemsControl(notificationAreaIcons);
    LogNotificationAreaItemsControlProbe(notificationAreaIcons, probe, rows,
                                         width);

    TrayLayoutMode effectiveTrayLayoutMode = GetEffectiveTrayLayoutMode();
    if (effectiveTrayLayoutMode != g_settings.trayLayoutMode) {
        TrayDiagLog(L"[real-layout] effective mode=%s base=%s rows=%d width=%d",
                    TrayLayoutModeName(effectiveTrayLayoutMode),
                    TrayLayoutModeName(g_settings.trayLayoutMode), rows,
                    width);
    }

    if (effectiveTrayLayoutMode == TrayLayoutMode::realWrapGrid) {
        if (ApplyNotificationAreaWrapGridLayout(notificationAreaIcons, probe,
                                               rows, width)) {
            return true;
        }

        TrayDiagLog(
            L"[real-layout] WrapGrid not active; falling back to native "
            L"single-row layout");
        rows = 1;
    } else if (effectiveTrayLayoutMode == TrayLayoutMode::nativeStack) {
        RestoreNotificationAreaNativeItemsPanel(probe.itemsControl,
                                                L"native-mode");
        rows = 1;
    }

    FrameworkElement stackPanel = nullptr;

    FrameworkElement child = notificationAreaIcons;
    if ((child = FindChildByClassName(
             child, L"Windows.UI.Xaml.Controls.ItemsPresenter")) &&
        (child = FindChildByClassName(
             child, L"Windows.UI.Xaml.Controls.StackPanel"))) {
        stackPanel = child;
    }
    if (!stackPanel) {
        stackPanel = FindDescendantByClassName(
            notificationAreaIcons, L"Windows.UI.Xaml.Controls.StackPanel");
    }

    if (!stackPanel) {
        TrayDiagLog(L"[candidate] NotificationAreaIcons has no stack panel");
        return false;
    }

    AttachTrayContainerLayoutRetry(notificationAreaIcons, L"NotificationAreaIcons");
    AttachTrayContainerLayoutRetry(stackPanel, L"NotificationAreaIconsStackPanel");

    int eligibleChildren = 0;
    EnumChildElements(stackPanel, [width, rows,
                                    &eligibleChildren](FrameworkElement child) {
        auto childClassName = winrt::get_class_name(child);
        if (childClassName != L"Windows.UI.Xaml.Controls.ContentPresenter") {
            Wh_Log(L"Unsupported class name %s of child",
                   childClassName.c_str());
            return false;
        }

        FrameworkElement notifyIconViewElement =
            FindChildByName(child, L"NotifyItemIcon");
        if (!notifyIconViewElement) {
            notifyIconViewElement =
                FindDescendantByClassName(child, L"SystemTray.NotifyIconView");
        }
        if (!notifyIconViewElement) {
            Wh_Log(L"Failed to get notifyIconViewElement of child");
            return false;
        }

        eligibleChildren++;
        return false;
    });

    TrayDiagLog(
        L"[candidate] NotificationAreaIcons selected=%d loaded=%d visible=%d "
        L"actual=%.1fx%.1f stackActual=%.1fx%.1f stackChildren=%d "
        L"eligible=%d",
        eligibleChildren > 0, notificationAreaIcons.IsLoaded(),
        notificationAreaIcons.Visibility() == Visibility::Visible,
        notificationAreaIcons.ActualWidth(), notificationAreaIcons.ActualHeight(),
        stackPanel.ActualWidth(), stackPanel.ActualHeight(),
        CountDirectChildren(stackPanel), eligibleChildren);

    if (eligibleChildren == 0) {
        return false;
    }

    ApplyNotifyIconsStackPanelGridStyle(stackPanel, rows, width);

    return true;
}

void ApplySystemTrayIconStyle(FrameworkElement systemTrayIconElement,
                              int width) {
    Wh_Log(L"Setting width %d for SystemTrayIcon", width);

    FrameworkElement child = systemTrayIconElement;
    if ((child = FindChildByName(child, L"ContainerGrid")) &&
        (child = FindChildByName(child, L"ContentGrid")) &&
        (child = FindChildByClassName(child, L"SystemTray.TextIconContent")) &&
        (child = FindChildByName(child, L"ContainerGrid"))) {
        auto childControl = child.try_as<Controls::Grid>();
        if (childControl) {
            int newPadding = 4;

            if (width > 32) {
                newPadding = (8 + width - 32) / 2;
            } else if (width < 24) {
                newPadding = (8 + width - 24) / 2;
                if (newPadding < 0) {
                    newPadding = 0;
                }
            }

            Wh_Log(L"Setting Padding=%d for ContainerGrid", newPadding);
            childControl.Padding(Thickness{
                .Left = static_cast<double>(newPadding),
                .Right = static_cast<double>(newPadding),
            });
        }
    }
}

bool ApplyControlCenterButtonStyle(FrameworkElement controlCenterButton,
                                   int width) {
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

    EnumChildElements(stackPanel, [width](FrameworkElement child) {
        auto childClassName = winrt::get_class_name(child);
        if (childClassName != L"Windows.UI.Xaml.Controls.ContentPresenter") {
            Wh_Log(L"Unsupported class name %s of child",
                   childClassName.c_str());
            return false;
        }

        FrameworkElement systemTrayIconElement =
            FindChildByName(child, L"SystemTrayIcon");
        if (!systemTrayIconElement) {
            Wh_Log(L"Failed to get SystemTrayIcon of child");
            return false;
        }

        ApplySystemTrayIconStyle(systemTrayIconElement, width);
        return false;
    });

    return true;
}

bool ApplyIconStackStyle(PCWSTR containerName,
                         FrameworkElement container,
                         int rows,
                         int width) {
    (void)rows;

    FrameworkElement stackPanel = nullptr;

    FrameworkElement child = container;
    if ((child = FindChildByName(child, L"Content")) &&
        (child = FindChildByName(child, L"IconStack")) &&
        (child = FindChildByClassName(
             child, L"Windows.UI.Xaml.Controls.ItemsPresenter")) &&
        (child = FindChildByClassName(
             child, L"Windows.UI.Xaml.Controls.StackPanel"))) {
        stackPanel = child;
    }

    if (!stackPanel) {
        return false;
    }

    EnumChildElements(stackPanel, [containerName,
                                   width](FrameworkElement child) {
        auto childClassName = winrt::get_class_name(child);
        if (childClassName != L"Windows.UI.Xaml.Controls.ContentPresenter") {
            Wh_Log(L"Unsupported class name %s of child",
                   childClassName.c_str());
            return false;
        }

        if (wcscmp(containerName, L"NotifyIconStack") == 0) {
            FrameworkElement systemTrayChevronIconViewElement =
                FindChildByClassName(child, L"SystemTray.ChevronIconView");
            if (!systemTrayChevronIconViewElement) {
                Wh_Log(L"Failed to get SystemTray.ChevronIconView of child");
                return false;
            }

            ApplyNotifyIconViewStyle(systemTrayChevronIconViewElement, width);
        } else {
            FrameworkElement systemTrayIconElement =
                FindChildByName(child, L"SystemTrayIcon");
            if (!systemTrayIconElement) {
                Wh_Log(L"Failed to get SystemTrayIcon of child");
                return false;
            }

            if (IsChildOfElementByName(systemTrayIconElement,
                                       L"ControlCenterButton")) {
                return false;
            }

            ApplyNotifyIconViewStyle(systemTrayIconElement, width);
        }

        return false;
    });

    TrayDiagLog(L"[icon-stack] name=%s stackActual=%.1fx%.1f children=%d",
                containerName, stackPanel.ActualWidth(),
                stackPanel.ActualHeight(), CountDirectChildren(stackPanel));

    return true;
}

bool ApplyStyle(XamlRoot xamlRoot, int rows, int width) {
    FrameworkElement root = xamlRoot.Content().try_as<FrameworkElement>();
    if (!root) {
        return false;
    }

    std::vector<FrameworkElement> frameGrids;
    if (root.Name() == L"SystemTrayFrameGrid") {
        frameGrids.push_back(root);
    }

    EnumDescendantElements(root, [&frameGrids](FrameworkElement element, int) {
        if (element.Name() == L"SystemTrayFrameGrid") {
            frameGrids.push_back(element);
        }

        return false;
    });

    TrayDiagLog(L"[apply-style] rows=%d width=%d frameGrids=%u root=%s/%s",
                rows, width, static_cast<unsigned>(frameGrids.size()),
                root.Name().c_str(), winrt::get_class_name(root).c_str());

    bool somethingSucceeded = false;

    for (auto const& systemTrayFrameGrid : frameGrids) {
        int notificationCandidateCount = 0;
        EnumDescendantElements(
            systemTrayFrameGrid,
            [rows, width, &notificationCandidateCount,
             &somethingSucceeded](FrameworkElement element, int depth) {
                if (element.Name() != L"NotificationAreaIcons") {
                    return false;
                }

                notificationCandidateCount++;
                TrayDiagLog(
                    L"[candidate] NotificationAreaIcons depth=%d live=%d "
                    L"actual=%.1fx%.1f children=%d",
                    depth, IsElementVisibleEnough(element),
                    element.ActualWidth(), element.ActualHeight(),
                    CountDirectChildren(element));
                DumpParentChainDiag(element, L"NotificationAreaIcons");
                somethingSucceeded |= ApplyNotifyIconsStyle(element, rows, width);
                return false;
            });

        TrayDiagLog(L"[apply-style] frame notificationCandidates=%d",
                    notificationCandidateCount);

        for (PCWSTR containerName : {
                 L"NotifyIconStack",
                 L"MainStack",
                 L"NonActivatableStack",
             }) {
            EnumDescendantElements(
                systemTrayFrameGrid,
                [containerName, rows, width,
                 &somethingSucceeded](FrameworkElement element, int depth) {
                    if (element.Name() != containerName) {
                        return false;
                    }

                    TrayDiagLog(
                        L"[candidate] %s depth=%d live=%d actual=%.1fx%.1f "
                        L"children=%d",
                        containerName, depth, IsElementVisibleEnough(element),
                        element.ActualWidth(), element.ActualHeight(),
                        CountDirectChildren(element));
                    DumpParentChainDiag(element, containerName);
                    somethingSucceeded |=
                        ApplyIconStackStyle(containerName, element, rows, width);
                    return false;
                });
        }
    }

    return somethingSucceeded;
}

void ApplyXamlRootFromMultirow(XamlRoot xamlRoot) {
    if (!xamlRoot || g_unloading) {
        return;
    }

    g_cachedTaskbarXamlRoot = xamlRoot;

    static DWORD lastApplyTick = 0;
    DWORD now = GetTickCount();
    if (lastApplyTick && now - lastApplyTick < 50) {
        RequestNotificationIconRowsRefresh();
        return;
    }

    lastApplyTick = now;

    int rows = GetEffectiveNotificationIconRows();
    int width = g_settings.notificationIconWidth;
    TrayDiagLog(L"[multirow-root] applying rows=%d width=%d", rows, width);
    if (ApplyStyle(xamlRoot, rows, width)) {
        g_lastAppliedNotificationRows = rows;
    }
}

using IconView_IconView_t = void*(WINAPI*)(void* pThis);
IconView_IconView_t IconView_IconView_Original;
void* WINAPI IconView_IconView_Hook(void* pThis) {
    Wh_Log(L">");

    void* ret = IconView_IconView_Original(pThis);
    if (g_unloading) {
        return ret;
    }

    FrameworkElement iconView = nullptr;
    ((IUnknown**)pThis)[1]->QueryInterface(winrt::guid_of<FrameworkElement>(),
                                           winrt::put_abi(iconView));
    if (!iconView) {
        return ret;
    }

    AttachNotifyIconLateLayoutRetry(iconView);

    g_autoRevokerList.emplace_back();
    auto autoRevokerIt = g_autoRevokerList.end();
    --autoRevokerIt;

    *autoRevokerIt = iconView.Loaded(
        winrt::auto_revoke_t{},
        [autoRevokerIt](winrt::Windows::Foundation::IInspectable const& sender,
                        RoutedEventArgs const& e) {
            Wh_Log(L">");
            if (g_unloading) {
                return;
            }

            g_autoRevokerList.erase(autoRevokerIt);

            auto iconView = sender.try_as<FrameworkElement>();
            if (!iconView) {
                return;
            }

            auto className = winrt::get_class_name(iconView);
            Wh_Log(L"className: %s", className.c_str());
            DumpParentChainDiag(iconView, L"IconViewLoaded");

            if (className == L"SystemTray.NotifyIconView") {
                ApplyNotifyIconLateLayoutRetry(iconView, L"loaded");
            } else if (className == L"SystemTray.IconView") {
                if (iconView.Name() == L"SystemTrayIcon") {
                    if (IsChildOfElementByName(iconView,
                                               L"ControlCenterButton")) {
                        return;
                    } else if (IsChildOfElementByName(iconView, L"MainStack")) {
                        ApplyNotifyIconViewStyle(
                            iconView, g_settings.notificationIconWidth);
                    } else if (IsChildOfElementByName(
                                   iconView, L"NonActivatableStack")) {
                        ApplyNotifyIconViewStyle(
                            iconView, g_settings.notificationIconWidth);
                    }
                }
            } else if (className == L"SystemTray.ChevronIconView") {
                if (IsChildOfElementByName(iconView, L"NotifyIconStack")) {
                    ApplyNotifyIconViewStyle(iconView,
                                             g_settings.notificationIconWidth);
                }
            }
        });

    return ret;
}

void ApplyOverflowStyle(FrameworkElement overflowRootGrid) {
    Controls::WrapGrid wrapGrid = nullptr;

    FrameworkElement child = overflowRootGrid;
    if ((child = FindChildByClassName(
             child, L"Windows.UI.Xaml.Controls.ItemsControl")) &&
        (child = FindChildByClassName(
             child, L"Windows.UI.Xaml.Controls.ItemsPresenter")) &&
        (child = FindChildByClassName(child,
                                      L"Windows.UI.Xaml.Controls.WrapGrid"))) {
        wrapGrid = child.try_as<Controls::WrapGrid>();
    }

    if (!wrapGrid) {
        return;
    }

    int width = g_unloading ? 40 : g_settings.overflowIconWidth;
    int maxRows = g_unloading ? 5 : g_settings.overflowIconsPerRow;
    Wh_Log(
        L"Setting ItemWidth/ItemHeight=%d, MaximumRowsOrColumns=%d for "
        L"WrapGrid",
        width, maxRows);

    wrapGrid.ItemWidth(width);
    wrapGrid.ItemHeight(width);
    wrapGrid.MaximumRowsOrColumns(maxRows);

    EnumChildElements(wrapGrid, [width](FrameworkElement child) {
        auto className = winrt::get_class_name(child);
        if (className != L"Windows.UI.Xaml.Controls.ContentPresenter") {
            Wh_Log(L"Unsupported class name %s of child", className.c_str());
            return false;
        }

        auto notifyIconView =
            FindChildByClassName(child, L"SystemTray.NotifyIconView");
        if (notifyIconView) {
            ApplyNotifyIconViewOverflowStyle(notifyIconView, width);
        }

        return false;
    });
}

using OverflowXamlIslandManager_InitializeIfNeeded_t =
    void(WINAPI*)(void* pThis);
OverflowXamlIslandManager_InitializeIfNeeded_t
    OverflowXamlIslandManager_InitializeIfNeeded_Original;
void WINAPI OverflowXamlIslandManager_InitializeIfNeeded_Hook(void* pThis) {
    Wh_Log(L">");

    OverflowXamlIslandManager_InitializeIfNeeded_Original(pThis);
    if (g_unloading) {
        return;
    }

    if (g_overflowRootGrid.get()) {
        return;
    }

    FrameworkElement overflowRootGrid = nullptr;
    ((IUnknown**)pThis)[5]->QueryInterface(winrt::guid_of<Controls::Grid>(),
                                           winrt::put_abi(overflowRootGrid));
    if (!overflowRootGrid) {
        Wh_Log(L"No OverflowRootGrid");
        return;
    }

    if (!overflowRootGrid.IsLoaded()) {
        Wh_Log(L"OverflowRootGrid not loaded");
        return;
    }

    g_overflowRootGrid = overflowRootGrid;
    ApplyOverflowStyle(overflowRootGrid);
}

using StackViewModel_UpdateIconIndexes_t = void(WINAPI*)(void* pThis);
StackViewModel_UpdateIconIndexes_t StackViewModel_UpdateIconIndexes_Original;
void WINAPI StackViewModel_UpdateIconIndexes_Hook(void* pThis) {
    Wh_Log(L">");

    StackViewModel_UpdateIconIndexes_Original(pThis);
    if (g_unloading) {
        return;
    }

    RequestNotificationIconRowsRefresh();
}

void* CTaskBand_ITaskListWndSite_vftable;

using CTaskBand_GetTaskbarHost_t = void*(WINAPI*)(void* pThis, void** result);
CTaskBand_GetTaskbarHost_t CTaskBand_GetTaskbarHost_Original;

void* TaskbarHost_FrameHeight_Original;

using std__Ref_count_base__Decref_t = void(WINAPI*)(void* pThis);
std__Ref_count_base__Decref_t std__Ref_count_base__Decref_Original;

XamlRoot GetTaskbarXamlRoot(HWND hTaskbarWnd) {
    if (auto xamlRoot = g_cachedTaskbarXamlRoot.get()) {
        AppendParentChainDiag(L"[GetTaskbarXamlRoot] using tray cached xaml root");
        return xamlRoot;
    }

#ifdef WIN11_TASKBAR_WIN10_MULTIROW_COMBINED
    if (auto xamlRoot = ::MultirowComponent::GetCachedTaskbarXamlRoot()) {
        AppendParentChainDiag(L"[GetTaskbarXamlRoot] using cached multirow xaml root");
        return xamlRoot;
    }

    if (auto xamlRoot = ::MultirowComponent::GetTaskbarXamlRoot(hTaskbarWnd)) {
        AppendParentChainDiag(L"[GetTaskbarXamlRoot] using multirow xaml root");
        return xamlRoot;
    }
    AppendParentChainDiag(L"[GetTaskbarXamlRoot] multirow xaml root failed, falling back");
#endif

    HWND hTaskSwWnd = (HWND)GetProp(hTaskbarWnd, L"TaskbandHWND");
    if (!hTaskSwWnd) {
        AppendParentChainDiag(L"[GetTaskbarXamlRoot] no TaskbandHWND");
        return nullptr;
    }
    AppendParentChainDiag(L"[GetTaskbarXamlRoot] got TaskbandHWND");

    void* taskBand = (void*)GetWindowLongPtr(hTaskSwWnd, 0);
    if (!taskBand) {
        AppendParentChainDiag(L"[GetTaskbarXamlRoot] no taskBand");
        return nullptr;
    }

    void* taskBandForTaskListWndSite = taskBand;
    for (int i = 0; *(void**)taskBandForTaskListWndSite !=
                    CTaskBand_ITaskListWndSite_vftable;
         i++) {
        if (i == 20) {
            AppendParentChainDiag(L"[GetTaskbarXamlRoot] vtable not found");
            return nullptr;
        }

        taskBandForTaskListWndSite = (void**)taskBandForTaskListWndSite + 1;
    }
    AppendParentChainDiag(L"[GetTaskbarXamlRoot] vtable found");

    void* taskbarHostSharedPtr[2]{};
    CTaskBand_GetTaskbarHost_Original(taskBandForTaskListWndSite,
                                       taskbarHostSharedPtr);
    if (!taskbarHostSharedPtr[0] && !taskbarHostSharedPtr[1]) {
        AppendParentChainDiag(L"[GetTaskbarXamlRoot] host shared ptr empty");
        return nullptr;
    }
    AppendParentChainDiag(L"[GetTaskbarXamlRoot] host shared ptr ok");

    size_t taskbarElementIUnknownOffset = 0x48;

#if defined(_M_X64)
    {
        // 48:83EC 28 | sub rsp,28
        // 48:83C1 48 | add rcx,48
        const BYTE* b = (const BYTE*)TaskbarHost_FrameHeight_Original;
        if (b[0] == 0x48 && b[1] == 0x83 && b[2] == 0xEC && b[4] == 0x48 &&
            b[5] == 0x83 && b[6] == 0xC1 && b[7] <= 0x7F) {
            taskbarElementIUnknownOffset = b[7];
        } else {
            Wh_Log(L"Unsupported TaskbarHost::FrameHeight");
            AppendParentChainDiag(L"[GetTaskbarXamlRoot] unsupported frame height");
        }
    }
#elif defined(_M_ARM64)
    // Just use the default offset which will hopefully work in most cases.
#else
#error "Unsupported architecture"
#endif

    auto* taskbarElementIUnknown =
        *(IUnknown**)((BYTE*)taskbarHostSharedPtr[0] +
                      taskbarElementIUnknownOffset);

    FrameworkElement taskbarElement = nullptr;
    taskbarElementIUnknown->QueryInterface(winrt::guid_of<FrameworkElement>(),
                                           winrt::put_abi(taskbarElement));
    if (!taskbarElement) {
        AppendParentChainDiag(L"[GetTaskbarXamlRoot] taskbar element query failed");
    } else {
        AppendParentChainDiag(L"[GetTaskbarXamlRoot] taskbar element ok");
    }

    auto result = taskbarElement ? taskbarElement.XamlRoot() : nullptr;
    if (!result) {
        AppendParentChainDiag(L"[GetTaskbarXamlRoot] result null");
    } else {
        AppendParentChainDiag(L"[GetTaskbarXamlRoot] result ok");
    }

    std__Ref_count_base__Decref_Original(taskbarHostSharedPtr[1]);

    return result;
}

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
    if (dwThreadId == 0) {
        return false;
    }

    if (dwThreadId == GetCurrentThreadId()) {
        proc(procParam);
        return true;
    }

    HHOOK hook = SetWindowsHookEx(
        WH_CALLWNDPROC,
        [](int nCode, WPARAM wParam, LPARAM lParam) -> LRESULT {
            if (nCode == HC_ACTION) {
                const CWPSTRUCT* cwp = (const CWPSTRUCT*)lParam;
                if (cwp->message == runFromWindowThreadRegisteredMsg) {
                    RUN_FROM_WINDOW_THREAD_PARAM* param =
                        (RUN_FROM_WINDOW_THREAD_PARAM*)cwp->lParam;
                    param->proc(param->procParam);
                }
            }

            return CallNextHookEx(nullptr, nCode, wParam, lParam);
        },
        nullptr, dwThreadId);
    if (!hook) {
        return false;
    }

    RUN_FROM_WINDOW_THREAD_PARAM param;
    param.proc = proc;
    param.procParam = procParam;
    SendMessage(hWnd, runFromWindowThreadRegisteredMsg, 0, (LPARAM)&param);

    UnhookWindowsHookEx(hook);

    return true;
}

void LoadSettings() {
#ifdef WIN11_TASKBAR_WIN10_MULTIROW_COMBINED
    g_settings.taskbarHeight = 48;
    g_settings.notificationIconWidth = 32;
    g_settings.notificationIconRows = 1;
    g_settings.trayLayoutMode = TrayLayoutMode::nativeStack;
#else
    g_settings.taskbarHeight = std::max(Wh_GetIntSetting(L"TaskbarHeight"), 0);
    g_settings.notificationIconWidth =
        std::max(Wh_GetIntSetting(L"notificationIconWidth"), 1);
    g_settings.notificationIconRows =
        std::max(Wh_GetIntSetting(L"notificationIconRows"), 1);

    PCWSTR trayLayoutMode = Wh_GetStringSetting(L"trayLayoutMode");
    g_settings.trayLayoutMode = TrayLayoutMode::nativeStack;
    if (wcscmp(trayLayoutMode, L"realWrapGrid") == 0) {
        g_settings.trayLayoutMode = TrayLayoutMode::realWrapGrid;
    } else if (wcscmp(trayLayoutMode, L"legacyTransform") == 0) {
        g_settings.trayLayoutMode = TrayLayoutMode::legacyTransform;
    }
    Wh_FreeStringSetting(trayLayoutMode);
#endif

#ifdef WIN11_TASKBAR_WIN10_MULTIROW_COMBINED
    int persistedHeight = Wh_GetIntValue(L"EdgeResizeTaskbarHeight", 0);
    if (persistedHeight > 0 && persistedHeight <= 480) {
        if (persistedHeight != g_settings.taskbarHeight) {
            Wh_Log(L"Using edge-resize storage taskbarHeight=%d over "
                   L"settings taskbarHeight=%d",
                   persistedHeight, g_settings.taskbarHeight);
        }
        g_settings.taskbarHeight = persistedHeight;
    }

    int persistedNotificationIconRows =
        Wh_GetIntValue(L"EdgeResizeNotificationIconRows", 0);
    if (persistedNotificationIconRows > 0) {
        persistedNotificationIconRows =
            ClampNotificationIconRows(persistedNotificationIconRows);
        if (persistedNotificationIconRows !=
            g_settings.notificationIconRows) {
            Wh_Log(L"Using edge-resize storage notificationIconRows=%d over "
                   L"settings notificationIconRows=%d",
                   persistedNotificationIconRows,
                   g_settings.notificationIconRows);
        }
        g_settings.notificationIconRows = persistedNotificationIconRows;
        g_settings.trayLayoutMode = persistedNotificationIconRows > 1
                                        ? TrayLayoutMode::realWrapGrid
                                        : TrayLayoutMode::nativeStack;
        Wh_Log(L"Using edge-resize storage trayLayoutMode=%s",
               TrayLayoutModeName(g_settings.trayLayoutMode));
    }
#endif

#ifdef WIN11_TASKBAR_WIN10_MULTIROW_COMBINED
    g_settings.gridArrangement = GridArrangement::rowFirstLeftToRight;
    g_settings.overflowIconWidth = 32;
    g_settings.overflowIconsPerRow = 5;
#else
    PCWSTR gridArrangement = Wh_GetStringSetting(L"gridArrangement");
    g_settings.gridArrangement = GridArrangement::rowFirstLeftToRight;
    if (wcscmp(gridArrangement, L"columnFirstTopToBottom") == 0) {
        g_settings.gridArrangement = GridArrangement::columnFirstTopToBottom;
    } else if (wcscmp(gridArrangement, L"rowFirstBottomRowFirst") == 0) {
        g_settings.gridArrangement = GridArrangement::rowFirstBottomRowFirst;
    } else if (wcscmp(gridArrangement, L"columnFirstBottomToTop") == 0) {
        g_settings.gridArrangement = GridArrangement::columnFirstBottomToTop;
    } else if (wcscmp(gridArrangement, L"columnFirstBottomToTopRightToLeft") ==
               0) {
        g_settings.gridArrangement =
            GridArrangement::columnFirstBottomToTopRightToLeft;
    }
    Wh_FreeStringSetting(gridArrangement);

    g_settings.overflowIconWidth =
        std::max(Wh_GetIntSetting(L"overflowIconWidth"), 1);
    g_settings.overflowIconsPerRow =
        std::max(Wh_GetIntSetting(L"overflowIconsPerRow"), 1);
#endif
}

void ApplySettings() {
    struct ApplySettingsParam {
        HWND hTaskbarWnd;
        int rows;
        int width;
    };

    Wh_Log(L"Applying settings");
    AppendParentChainDiag(L"[ApplySettings] start");
    if (g_unloading) {
        Wh_Log(L"Skipping ApplySettings while unloading");
        AppendParentChainDiag(L"[ApplySettings] skipped unloading");
        return;
    }

    HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();
    if (!hTaskbarWnd) {
        Wh_Log(L"No taskbar found");
        AppendParentChainDiag(L"[ApplySettings] no taskbar found");
        return;
    }
    AppendParentChainDiag(L"[ApplySettings] taskbar found");

    ApplySettingsParam param{
        .hTaskbarWnd = hTaskbarWnd,
        .rows = GetEffectiveNotificationIconRows(),
        .width = g_settings.notificationIconWidth,
    };

    RunFromWindowThread(
        hTaskbarWnd,
        [](void* pParam) {
            ApplySettingsParam& param = *(ApplySettingsParam*)pParam;

            auto xamlRoot = GetTaskbarXamlRoot(param.hTaskbarWnd);
            if (!xamlRoot) {
                Wh_Log(L"Getting XamlRoot failed");
                AppendParentChainDiag(L"[ApplySettings] xaml root failed");
                return;
            }
            AppendParentChainDiag(L"[ApplySettings] xaml root ok");

            if (!ApplyStyle(xamlRoot, param.rows, param.width)) {
                Wh_Log(L"ApplyStyle failed");
                AppendParentChainDiag(L"[ApplySettings] apply style failed");
            } else {
                AppendParentChainDiag(L"[ApplySettings] apply style ok");
            }

            if (auto overflowRootGrid = g_overflowRootGrid.get()) {
                ApplyOverflowStyle(overflowRootGrid);
            }
        },
        &param);
}

void PrepareNativeUnload(int nativeHeight) {
    if (nativeHeight < 1) {
        nativeHeight = 48;
    }

    TrayDiagLog(L"[native-unload] start height=%d", nativeHeight);
    AppendParentChainDiag(L"[native-unload] start");

    int oldHeight = g_liveTaskbarHeightHint.exchange(nativeHeight);
    g_liveTaskbarHeightHintTick = GetTickCount();
    int oldRows = g_liveNotificationIconRowsHint.exchange(1);
    g_liveNotificationIconRowsHintTick = GetTickCount();

    int oldWidth = g_settings.notificationIconWidth;
    TrayLayoutMode oldTrayLayoutMode = g_settings.trayLayoutMode;
    g_settings.notificationIconWidth = 32;
    g_settings.trayLayoutMode = TrayLayoutMode::nativeStack;

    TrayDiagLog(L"[native-unload] rows=1 oldRows=%d width=32 oldWidth=%d "
                L"height=%d oldHeight=%d",
                oldRows, oldWidth, nativeHeight, oldHeight);

    ApplySettings();

    g_settings.notificationIconWidth = oldWidth;
    g_settings.trayLayoutMode = oldTrayLayoutMode;

    AppendParentChainDiag(L"[native-unload] done");
}

void ApplyNotificationIconRowsRefreshPass(PCWSTR reason) {
    if (g_unloading) {
        return;
    }

    int rows = GetEffectiveNotificationIconRows();
    Wh_Log(L"Notification icon rows refresh pass reason=%s rows=%d", reason,
           rows);
    ApplySettings();
    g_lastAppliedNotificationRows = rows;
}

DWORD WINAPI NotificationIconRowsRefreshThreadProc(void*) {
    int handledRequest = 0;

    while (!g_unloading) {
        handledRequest = g_notificationRowsRefreshRequest.load();

        constexpr DWORD kDelaysMs[] = {
            0,   16,   33,   50,   100,  150,  250,  350,  500,
            750, 1000, 1500, 2000, 2500, 3000, 3000, 3000, 5000,
            5000, 5000,
        };
        for (DWORD delayMs : kDelaysMs) {
            if (g_unloading) {
                break;
            }

            if (delayMs) {
                Sleep(delayMs);
            }

            ApplyNotificationIconRowsRefreshPass(L"scheduled");
        }

        if (g_notificationRowsRefreshRequest.load() == handledRequest) {
            break;
        }
    }

    g_notificationRowsRefreshRunning = false;

    if (!g_unloading &&
        g_notificationRowsRefreshRequest.load() != handledRequest &&
        !g_notificationRowsRefreshRunning.exchange(true)) {
        HANDLE thread = CreateThread(nullptr, 0,
                                     NotificationIconRowsRefreshThreadProc,
                                     nullptr, 0, nullptr);
        if (thread) {
            CloseHandle(thread);
        } else {
            g_notificationRowsRefreshRunning = false;
        }
    }

    return 0;
}

void RequestNotificationIconRowsRefresh() {
    if (g_unloading) {
        return;
    }

    g_notificationRowsRefreshRequest++;

    if (!g_notificationRowsRefreshRunning.exchange(true)) {
        HANDLE thread =
            CreateThread(nullptr, 0, NotificationIconRowsRefreshThreadProc,
                         nullptr, 0, nullptr);
        if (thread) {
            CloseHandle(thread);
        } else {
            g_notificationRowsRefreshRunning = false;
        }
    }
}

void StartHeightMonitor() {
    if (FindCurrentProcessTaskbarWnd()) {
        AppendParentChainDiag(L"[startup retry] taskbar already present");
        ApplySettings();
        g_lastAppliedNotificationRows = GetEffectiveNotificationIconRows();
        RequestNotificationIconRowsRefresh();
        return;
    }

    bool wasRunning = g_heightMonitorRunning.exchange(true);
    if (wasRunning) {
        return;
    }

    g_heightMonitorStop = false;
    std::thread([] {
        // Finite startup retry only. A permanent monitor caused unload
        // instability on this Windows 11 build.
        for (int i = 0; i < 80 && !g_heightMonitorStop && !g_unloading; i++) {
            if (FindCurrentProcessTaskbarWnd()) {
                AppendParentChainDiag(L"[startup retry] taskbar found");
                ApplySettings();
                g_lastAppliedNotificationRows = GetEffectiveNotificationIconRows();
                RequestNotificationIconRowsRefresh();
                g_heightMonitorRunning = false;
                return;
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(250));
        }

        if (!g_heightMonitorStop && !g_unloading) {
            AppendParentChainDiag(L"[startup retry] taskbar not found");
        }
        g_heightMonitorRunning = false;
    }).detach();
}

void StopHeightMonitor() {
    g_heightMonitorStop = true;
    g_heightMonitorRunning = false;
}

bool HookSystemTraySymbols(HMODULE module) {
    // SystemTray.dll, Taskbar.View.dll
    WindhawkUtils::SYMBOL_HOOK symbolHooks[] = {
        {
            {LR"(public: __cdecl winrt::SystemTray::implementation::IconView::IconView(void))"},
            &IconView_IconView_Original,
            IconView_IconView_Hook,
        },
        {
            {LR"(private: void __cdecl winrt::SystemTray::OverflowXamlIslandManager::InitializeIfNeeded(void))"},
            &OverflowXamlIslandManager_InitializeIfNeeded_Original,
            OverflowXamlIslandManager_InitializeIfNeeded_Hook,
        },
        {
            {LR"(private: void __cdecl winrt::SystemTray::implementation::StackViewModel::UpdateIconIndexes(void))"},
            &StackViewModel_UpdateIconIndexes_Original,
            StackViewModel_UpdateIconIndexes_Hook,
        },
    };

    if (!HookSymbols(module, symbolHooks, ARRAYSIZE(symbolHooks))) {
        Wh_Log(L"HookSymbols failed");
        return false;
    }

    return true;
}

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

// Returns the module that hosts winrt::SystemTray::* in the current build.
// Order matters: SystemTray.dll is the new home (Win11 Insider 26200+);
// Taskbar.View.dll is kept as fallbacks so this still works on older builds.
HMODULE GetSystemTrayModuleHandle() {
    HMODULE module = GetModuleHandle(L"SystemTray.dll");
    if (!module) {
        module = GetModuleHandle(L"Taskbar.View.dll");
        if (module) {
            // First known module version without SystemTray is Taskbar.View.dll
            // 2604.8002.200.6000.
            VS_FIXEDFILEINFO* fixedFileInfo =
                GetModuleVersionInfo(module, nullptr);
            WORD moduleMajor =
                fixedFileInfo ? HIWORD(fixedFileInfo->dwFileVersionMS) : 0;
            if (!moduleMajor || moduleMajor >= 2604) {
                Wh_Log(L"Skipping Taskbar.View.dll version %d", moduleMajor);
                module = nullptr;
            }
        }
    }

    return module;
}

void HandleLoadedModuleIfSystemTray(HMODULE module, LPCWSTR lpLibFileName) {
    if (!g_systemTrayModuleHooked && GetSystemTrayModuleHandle() == module &&
        !g_systemTrayModuleHooked.exchange(true)) {
        Wh_Log(L"Loaded %s", lpLibFileName);

        if (HookSystemTraySymbols(module)) {
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
        HandleLoadedModuleIfSystemTray(module, lpLibFileName);
    }

    return module;
}

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

BOOL Wh_ModInit() {
    Wh_Log(L">");
    g_trayDiagLineCount = 0;
    WCHAR trayDiagPath[MAX_PATH];
    if (BuildTrayDiagPath(trayDiagPath, ARRAYSIZE(trayDiagPath))) {
        DeleteFileW(trayDiagPath);
    }
    AppendParentChainDiag(L"[Wh_ModInit] start");

    LoadSettings();
    AppendParentChainDiag(L"[Wh_ModInit] settings loaded");

    if (HMODULE systemTrayModule = GetSystemTrayModuleHandle()) {
        g_systemTrayModuleHooked = true;
        AppendParentChainDiag(L"[Wh_ModInit] system tray module present");
        if (!HookSystemTraySymbols(systemTrayModule)) {
            AppendParentChainDiag(L"[Wh_ModInit] system tray hook failed");
            return FALSE;
        }
    } else {
        Wh_Log(L"System tray module not loaded yet");
        AppendParentChainDiag(L"[Wh_ModInit] system tray module absent");

        HMODULE kernelBaseModule = GetModuleHandle(L"kernelbase.dll");
        auto pKernelBaseLoadLibraryExW =
            (decltype(&LoadLibraryExW))GetProcAddress(kernelBaseModule,
                                                      "LoadLibraryExW");
        WindhawkUtils::Wh_SetFunctionHookT(pKernelBaseLoadLibraryExW,
                                           LoadLibraryExW_Hook,
                                           &LoadLibraryExW_Original);
    }

    if (!HookTaskbarDllSymbols()) {
        AppendParentChainDiag(L"[Wh_ModInit] taskbar dll hook failed");
        return FALSE;
    }

    AppendParentChainDiag(L"[Wh_ModInit] done");
    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L">");
    AppendParentChainDiag(L"[Wh_ModAfterInit] start");

    if (!g_systemTrayModuleHooked) {
        if (HMODULE systemTrayModule = GetSystemTrayModuleHandle()) {
            if (!g_systemTrayModuleHooked.exchange(true)) {
                Wh_Log(L"Got system tray module");
                AppendParentChainDiag(L"[Wh_ModAfterInit] late system tray module present");

                if (HookSystemTraySymbols(systemTrayModule)) {
                    Wh_ApplyHookOperations();
                } else {
                    AppendParentChainDiag(L"[Wh_ModAfterInit] late system tray hook failed");
                }
            }
        }
    }

    ApplySettings();
    AppendParentChainDiag(L"[Wh_ModAfterInit] done");
    g_lastAppliedNotificationRows = GetEffectiveNotificationIconRows();
    RequestNotificationIconRowsRefresh();
    StartHeightMonitor();
}

void Wh_ModBeforeUninit() {
    Wh_Log(L">");

    StopHeightMonitor();
    PrepareNativeUnload(48);
    g_unloading = true;
    g_autoRevokerList.clear();
    g_sizeChangedRevokerList.clear();
    g_notificationAreaIconsStackPanel = nullptr;
    g_overflowRootGrid = nullptr;
    g_cachedTaskbarXamlRoot = nullptr;
}

void Wh_ModUninit() {
    Wh_Log(L">");
    StopHeightMonitor();
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");

    LoadSettings();

    ApplySettings();
    g_lastAppliedNotificationRows = GetEffectiveNotificationIconRows();
    RequestNotificationIconRowsRefresh();
}
// END INLINED COMPONENT: notification-area
}

#define Wh_ModInit Styler_Wh_ModInit
#define Wh_ModAfterInit Styler_Wh_ModAfterInit
#define Wh_ModUninit Styler_Wh_ModUninit
#define Wh_ModSettingsChanged Styler_Wh_ModSettingsChanged
// BEGIN INLINED COMPONENT: taskbar-styler
// Source code is published under The GNU General Public License v3.0.
//
// For bug reports and feature requests, please open an issue here:
// https://github.com/ramensoftware/windhawk-mods/issues
//
// For pull requests, development takes place here:
// https://github.com/m417z/my-windhawk-mods



#include <commctrl.h>
#include <xamlom.h>

#include <atomic>
#include <vector>

#undef GetCurrentTime

#include <winrt/Windows.UI.Xaml.h>

struct ThemeTargetStyles {
    PCWSTR target;
    std::vector<PCWSTR> styles;
};

struct Theme {
    std::vector<ThemeTargetStyles> targetStyles;
    std::vector<PCWSTR> styleConstants;
    std::vector<PCWSTR> themeResourceVariables;
};

// clang-format off

const Theme g_themeTranslucentTaskbar = {{
    ThemeTargetStyles{L"Taskbar.TaskbarFrame > Grid#RootGrid > Taskbar.TaskbarBackground > Grid > Rectangle#BackgroundFill", {
        L"Fill:=$CommonBgBrush"}},
    ThemeTargetStyles{L"Taskbar.TaskbarBackground#HoverFlyoutBackgroundControl > Grid > Rectangle#BackgroundFill", {
        L"Fill:=$CommonBgBrush"}},
    ThemeTargetStyles{L"Rectangle#BackgroundStroke", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"MenuFlyoutPresenter > Border", {
        L"Background:=$CommonBgBrush",
        L"BorderThickness=0,0,0,0",
        L"CornerRadius=14",
        L"Padding=3,4,3,4"}},
    ThemeTargetStyles{L"Border#OverflowFlyoutBackgroundBorder", {
        L"Background:=$CommonBgBrush",
        L"BorderThickness=0,0,0,0",
        L"CornerRadius=15",
        L"Margin=-2,-2,-2,-2"}},
    ThemeTargetStyles{L"Grid#ConfirmatorMainGrid", {
        L"Background:=$CommonBgBrush",
        L"BorderThickness=0"}},
    ThemeTargetStyles{L"WindowsInternal.ComposableShell.Experiences.TextInput.Common.InputSwitcher > ContentControl > ContentPresenter > Grid", {
        L"Background:=<WindhawkBlur BlurAmount=\"25\" TintColor=\"#25323232\"/>",
        L"BorderThickness=0"}},
    ThemeTargetStyles{L"WindowsInternal.ComposableShell.Experiences.TextInput.Common.InputSwitcher > ContentControl > ContentPresenter > Grid > Grid", {
        L"Background:=<SolidColorBrush Color=\"Transparent\"/>"}},
}, {
    L"CommonBgBrush=<WindhawkBlur BlurAmount=\"18\" TintColor=\"#25323232\"/>",
}};

const Theme g_themeDockLike = {{
    ThemeTargetStyles{L"Taskbar.TaskbarFrame", {
        L"Width=Auto",
        L"HorizontalAlignment=Center",
        L"Margin=250,0,250,0"}},
    ThemeTargetStyles{L"Taskbar.TaskbarFrame > Grid#RootGrid", {
        L"Background:=<AcrylicBrush TintColor=\"{ThemeResource SystemChromeAltHighColor}\" TintOpacity=\"0.8\" FallbackColor=\"{ThemeResource SystemChromeLowColor}\" />",
        L"Padding=6,0,6,0",
        L"CornerRadius=8,8,0,0",
        L"BorderBrush:=<SolidColorBrush Color=\"{ThemeResource SurfaceStrokeColorDefault}\" />"}},
    ThemeTargetStyles{L"Taskbar.TaskbarFrame > Grid#RootGrid > Taskbar.TaskbarBackground > Grid > Rectangle#BackgroundFill", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Rectangle#BackgroundStroke", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Taskbar.AugmentedEntryPointButton#AugmentedEntryPointButton > Taskbar.TaskListButtonPanel#ExperienceToggleButtonRootPanel", {
        L"Margin=0"}},
    ThemeTargetStyles{L"Grid#SystemTrayFrameGrid", {
        L"Background:=<AcrylicBrush TintColor=\"{ThemeResource SystemChromeAltHighColor}\" TintOpacity=\"0.8\" FallbackColor=\"{ThemeResource SystemChromeLowColor}\" />",
        L"Margin=-4,-8,-4,-8",
        L"CornerRadius=10",
        L"BorderThickness=12,12,12,12",
        L"BackgroundSizing=InnerBorderEdge"}},
    ThemeTargetStyles{L"SystemTray.ChevronIconView", {
        L"Padding=0"}},
    ThemeTargetStyles{L"SystemTray.NotifyIconView#NotifyItemIcon", {
        L"Padding=0"}},
    ThemeTargetStyles{L"SystemTray.OmniButton", {
        L"Padding=0"}},
    ThemeTargetStyles{L"SystemTray.CopilotIcon", {
        L"Padding=0"}},
    ThemeTargetStyles{L"SystemTray.OmniButton#NotificationCenterButton > Grid > ContentPresenter > ItemsPresenter > StackPanel > ContentPresenter > systemtray:IconView#SystemTrayIcon > Grid", {
        L"Padding=4,0,4,0"}},
    ThemeTargetStyles{L"SystemTray.IconView#SystemTrayIcon > Grid#ContainerGrid > ContentPresenter#ContentPresenter > Grid#ContentGrid > SystemTray.TextIconContent > Grid#ContainerGrid", {
        L"Padding=0"}},
    ThemeTargetStyles{L"SystemTray.StackListView#IconStack > ItemsPresenter > StackPanel > ContentPresenter > SystemTray.IconView#SystemTrayIcon", {
        L"Padding=0"}},
    ThemeTargetStyles{L"SystemTray.Stack#ShowDesktopStack", {
        L"Margin=0,-4,-12,-4"}},
    ThemeTargetStyles{L"Taskbar.Gripper#GripperControl", {
        L"Width=Auto",
        L"MinWidth=24"}},
}};

const Theme g_themeSimplyTransparent = {{
    ThemeTargetStyles{L"Taskbar.TaskbarFrame > Grid#RootGrid > Taskbar.TaskbarBackground > Grid > Rectangle#BackgroundFill", {
        L"Fill=Transparent"}},
    ThemeTargetStyles{L"Rectangle#BackgroundStroke", {
        L"Fill=Transparent"}},
}};

const Theme g_themeSquircle = {{
    ThemeTargetStyles{L"Taskbar.TaskbarFrame > Grid#RootGrid > Taskbar.TaskbarBackground > Grid > Rectangle#BackgroundFill", {
        L"Fill=Transparent"}},
    ThemeTargetStyles{L"Taskbar.TaskbarBackground#HoverFlyoutBackgroundControl > Grid > Rectangle#BackgroundFill", {
        L"Fill=#CC222222"}},
    ThemeTargetStyles{L"Taskbar.TaskListButtonPanel@CommonStates > Border#BackgroundElement", {
        L"CornerRadius=5",
        L"Background:=<AcrylicBrush TintColor=\"Black\" TintOpacity=\"0.8\" FallbackColor=\"#BB222222\" />",
        L"Background@InactivePointerOver:=<AcrylicBrush TintColor=\"Black\" TintOpacity=\"0.8\" FallbackColor=\"#CC222222\" />",
        L"Background@ActivePointerOver:=<AcrylicBrush TintColor=\"Black\" TintOpacity=\"0.9\" FallbackColor=\"#CC222222\" />",
        L"Background@ActiveNormal:=<AcrylicBrush TintColor=\"Black\" TintOpacity=\"0.8\" FallbackColor=\"#CC222222\" />",
        L"Background@InactiveNormal:=<AcrylicBrush TintColor=\"Black\" TintOpacity=\"0.7\" FallbackColor=\"#BB222222\" />",
        L"Background@InactivePressed:=<AcrylicBrush TintColor=\"Black\" TintOpacity=\"0.8\" FallbackColor=\"#CC222222\" />",
        L"Background@ActivePressed:=<AcrylicBrush TintColor=\"Black\" TintOpacity=\"0.8\" FallbackColor=\"#CC222222\" />"}},
    ThemeTargetStyles{L"Grid#SystemTrayFrameGrid", {
        L"Background:=<AcrylicBrush TintColor=\"Black\" TintOpacity=\"0.8\" FallbackColor=\"#BB222222\"/>",
        L"CornerRadius=5",
        L"Margin=0,5,14,5",
        L"Padding=10,0,0,0"}},
    ThemeTargetStyles{L"Rectangle#RunningIndicator", {
        L"Fill=Transparent",
        L"RadiusX=5",
        L"RadiusY=5",
        L"Height=38",
        L"Width=40"}},
    ThemeTargetStyles{L"Taskbar.TaskListLabeledButtonPanel > TextBlock#LabelControl", {
        L"Margin=4,0,0,0",
        L"Foreground=White"}},
    ThemeTargetStyles{L"Taskbar.SearchBoxButton", {
        L"Foreground=White",
        L"Margin=-11,0,0,0"}},
    ThemeTargetStyles{L"TextBlock#SearchBoxTextBlock", {
        L"FontSize=12",
        L"Foreground=White"}},
    ThemeTargetStyles{L"Rectangle#BackgroundStroke", {
        L"Fill=Transparent"}},
    ThemeTargetStyles{L"Grid", {
        L"RequestedTheme=2"}},
    ThemeTargetStyles{L"Taskbar.TaskListButton#TaskListButton[AutomationProperties.Name=Copilot] > Taskbar.TaskListLabeledButtonPanel#IconPanel > Border#BackgroundElement", {
        L"Background:=<AcrylicBrush TintColor=\"Red\" TintOpacity=\"0.8\" />"}},
    ThemeTargetStyles{L"Border#BackgroundBorder", {
        L"Margin=0,3,0,3",
        L"CornerRadius=5"}},
    ThemeTargetStyles{L"Taskbar.AugmentedEntryPointButton#AugmentedEntryPointButton > Taskbar.TaskListButtonPanel#ExperienceToggleButtonRootPanel > Border#BackgroundElement@CommonStates", {
        L"Background@InactivePointerOver:=<AcrylicBrush TintColor=\"Black\" TintOpacity=\"0\" />",
        L"Background:=<AcrylicBrush TintColor=\"Black\" TintOpacity=\"0.8\" FallbackColor=\"#BB222222\" />"}},
    ThemeTargetStyles{L"Border#MultiWindowElement", {
        L"Background:=<AcrylicBrush TintColor=\"Black\" TintOpacity=\"0.8\" FallbackColor=\"#CC222222\" />"}},
    ThemeTargetStyles{L"TextBlock#TimeInnerTextBlock", {
        L"Foreground=White"}},
    ThemeTargetStyles{L"TextBlock#DateInnerTextBlock", {
        L"Foreground=White"}},
    ThemeTargetStyles{L"SystemTray.TextIconContent > Grid > SystemTray.AdaptiveTextBlock#Base > TextBlock", {
        L"Foreground=White"}},
    ThemeTargetStyles{L"Border#BackgroundElement", {
        L"BorderThickness=0"}},
    ThemeTargetStyles{L"Taskbar.AugmentedEntryPointButton#AugmentedEntryPointButton", {
        L"Margin=-11,0,0,0"}},
    ThemeTargetStyles{L"Taskbar.ExperienceToggleButton#LaunchListButton[AutomationProperties.Name=Task View]", {
        L"Margin=-12,0,0,0"}},
    ThemeTargetStyles{L"taskbar:TaskListLabeledButtonPanel@RunningIndicatorStates > Border", {
        L"Background@ActiveRunningIndicator:=<AcrylicBrush TintOpacity=\"0.8\" TintColor=\"Black\" />",
        L"Background@InactiveRunningIndicator:=<AcrylicBrush TintOpacity=\"0.8\" TintColor=\"Black\" />",
        L"Background@InactiveRunningIndicatorPointerOver:=<AcrylicBrush TintOpacity=\"0.8\" TintColor=\"Black\" />"}},
    ThemeTargetStyles{L"Taskbar.TaskListLabeledButtonPanel@CommonStates > Border#BackgroundElement", {
        L"Background@InactivePointerOver:=<AcrylicBrush TintOpacity=\"0.8\" TintColor=\"Black\" FallbackColor=\"#DD222222\"/>",
        L"Background@ActivePointerOver:=<AcrylicBrush TintOpacity=\"0.8\" TintColor=\"Black\" FallbackColor=\"#EE222222\"/>",
        L"Background@InactiveNormal:=<AcrylicBrush TintOpacity=\"0.2\" TintColor=\"Black\" FallbackColor=\"#BB222222\"/>",
        L"Background@ActiveNormal:=<AcrylicBrush TintOpacity=\"0.8\" TintColor=\"Black\" FallbackColor=\"#CC222222\"/>",
        L"Background@ActivePressed:=<AcrylicBrush TintOpacity=\"0.8\" TintColor=\"#333333\" FallbackColor=\"#BB333333\" />",
        L"Background@InactivePressed:=<AcrylicBrush TintOpacity=\"0.8\" TintColor=\"#333333\" FallbackColor=\"#BB333333\" />",
        L"CornerRadius=5",
        L"Margin=1"}},
}};

const Theme g_themeSquircle_variant_WeatherOnTheRight = {{
    ThemeTargetStyles{L"Rectangle#BackgroundFill", {
        L"Fill=Transparent"}},
    ThemeTargetStyles{L"Taskbar.TaskListLabeledButtonPanel@RunningIndicatorStates > Border#BackgroundElement", {
        L"Background@NoRunningIndicator:=<AcrylicBrush TintOpacity=\"0.2\" TintColor=\"#222222\" FallbackColor=\"#DD222222\" />",
        L"Background:=<AcrylicBrush TintOpacity=\"0.8\" TintColor=\"Black\" FallbackColor=\"#DD222222\" />",
        L"Margin=0",
        L"BorderThickness@NoRunningIndicator=1",
        L"Background@InactiveRunningIndicator:=<AcrylicBrush TintOpacity=\"0.7\" TintColor=\"Black\" FallbackColor=\"#DD222222\" />",
        L"Background@ActiveRunningIndicator:=<AcrylicBrush TintOpacity=\"1\" TintColor=\"Black\" FallbackColor=\"#DD222222\" />"}},
    ThemeTargetStyles{L"Grid#SystemTrayFrameGrid", {
        L"Background:=<AcrylicBrush TintOpacity=\"0.8\" TintColor=\"Black\" FallbackColor=\"#BB222222\" />",
        L"Margin=0,5,18,5",
        L"Padding=10,0,0,0",
        L"CornerRadius=5"}},
    ThemeTargetStyles{L"Taskbar.SearchBoxButton", {
        L"Margin=0,-2,0,0"}},
    ThemeTargetStyles{L"Border#MultiWindowElement", {
        L"Height=0"}},
    ThemeTargetStyles{L"Grid#OverflowRootGrid > Border", {
        L"Background:=<AcrylicBrush TintOpacity=\"0.8\" TintColor=\"Black\" FallbackColor=\"#BB222222\" />"}},
    ThemeTargetStyles{L"Grid", {
        L"RequestedTheme=2"}},
    ThemeTargetStyles{L"Taskbar.AugmentedEntryPointButton#AugmentedEntryPointButton > Taskbar.TaskListButtonPanel#ExperienceToggleButtonRootPanel > Border#BackgroundElement@CommonStates", {
        L"Background:=<AcrylicBrush TintOpacity=\"0.8\" TintColor=\"Black\" FallbackColor=\"#BB222222\" />",
        L"Width=125",
        L"BorderBrush=Transparent"}},
    ThemeTargetStyles{L"Grid#OverflowRootGrid", {
        L"Background:=<AcrylicBrush TintOpacity=\"0.8\" TintColor=\"Black\" FallbackColor=\"#BB222222\" />",
        L"Padding=0",
        L"Margin=0,0,0,12",
        L"CornerRadius=5"}},
    ThemeTargetStyles{L"Taskbar.ExperienceToggleButton#LaunchListButton > Taskbar.TaskListButtonPanel > Border", {
        L"Background:=<AcrylicBrush TintOpacity=\"0.8\" TintColor=\"Black\" FallbackColor=\"#BB222222\" />"}},
    ThemeTargetStyles{L"Taskbar.AugmentedEntryPointButton#AugmentedEntryPointButton", {
        L"Margin=20,1,-20,1"}},
    ThemeTargetStyles{L"Grid#AugmentedEntryPointContentGrid", {
        L"Margin=10,0,-5,0"}},
    ThemeTargetStyles{L"Taskbar.ExperienceToggleButton#LaunchListButton[AutomationProperties.Name=Task View] > Taskbar.TaskListButtonPanel > Border", {
        L"CornerRadius=0,5,5,0"}},
    ThemeTargetStyles{L"SearchUx.SearchUI.SearchBoxButton", {
        L"Background:=<AcrylicBrush TintOpacity=\"0.8\" TintColor=\"Black\" FallbackColor=\"#BB333333\" />"}},
    ThemeTargetStyles{L"SearchUx.SearchUI.SearchButtonRootGrid > Border", {
        L"Opacity=0"}},
    ThemeTargetStyles{L"SearchUx.SearchUI.SearchButtonRootGrid", {
        L"Background:=<AcrylicBrush TintOpacity=\"0.8\" TintColor=\"Black\" FallbackColor=\"#BB222222\" />",
        L"MinWidth=0",
        L"CornerRadius=0,5,5,0",
        L"Margin=0,4,2,4"}},
    ThemeTargetStyles{L"SearchUx.SearchUI.SearchBoxButton#SearchBox > SearchUx.SearchUI.SearchButtonRootGrid@CommonStates > Microsoft.UI.Xaml.Controls.AnimatedVisualPlayer#Icon", {
        L"Margin=14,0,0,0"}},
    ThemeTargetStyles{L"Taskbar.ExperienceToggleButton#LaunchListButton[AutomationProperties.Name=Start] > Taskbar.TaskListButtonPanel > Border", {
        L"Margin=1,0,0,0"}},
    ThemeTargetStyles{L"Taskbar.ExperienceToggleButton#LaunchListButton[AutomationProperties.Name=Start] > Taskbar.TaskListButtonPanel", {
        L"RenderTransform:=<TranslateTransform X=\"5\" />"}},
    ThemeTargetStyles{L"Taskbar.ExperienceToggleButton#LaunchListButton[AutomationProperties.Name=Task View] > Taskbar.TaskListButtonPanel", {
        L"RenderTransform:=<TranslateTransform X=\"-9\" />"}},
    ThemeTargetStyles{L"Border#BackgroundBorder", {
        L"Margin=0,3,0,3"}},
    ThemeTargetStyles{L"Taskbar.TaskbarBackground#HoverFlyoutBackgroundControl > Grid > Rectangle#BackgroundFill", {
        L"Fill:=<WindhawkBlur BlurAmount=\"12\" TintColor=\"#00000000\" />"}},
    ThemeTargetStyles{L"Border#BackgroundElement", {
        L"BorderThickness=0"}},
    ThemeTargetStyles{L"Rectangle#BackgroundStroke", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Taskbar.TaskListLabeledButtonPanel@CommonStates > Border#BackgroundElement", {
        L"BorderBrush=Transparent",
        L"BorderBrush@ActivePointerOver:=<AcrylicBrush TintOpacity=\"0.2\" TintColor=\"Black\" FallbackColor=\"#DD222222\" />",
        L"BorderBrush@InactivePointerOver:=<AcrylicBrush TintOpacity=\"0.2\" TintColor=\"Black\" FallbackColor=\"#DD222222\" />",
        L"CornerRadius=5"}},
    ThemeTargetStyles{L"Taskbar.TaskListLabeledButtonPanel@CommonStates > Rectangle#RunningIndicator", {
        L"Fill=Transparent",
        L"Fill@ActivePointerOver:=<AcrylicBrush TintOpacity=\"0.4\" TintColor=\"#88444444\" FallbackColor=\"#BB222222\" />",
        L"Fill@InactivePointerOver:=<AcrylicBrush TintOpacity=\"0.2\" TintColor=\"#88444444\" FallbackColor=\"#BB222222\" />",
        L"Fill@MultiWindowNormal=#22AAAABB",
        L"Fill@MultiWindowPointerOver=#88AAAABB",
        L"Fill@MultiWindowActive=#88AAAABB",
        L"Fill@MultiWindowPressed=#88AAAABB",
        L"Fill@RequestingAttention:=<SolidColorBrush Opacity=\"0.4\" Color=\"DarkOrange\" />",
        L"Fill@RequestingAttentionPointerOver:=<SolidColorBrush Opacity=\"0.4\" Color=\"Orange\" />",
        L"RadiusX=5",
        L"RadiusY=5",
        L"Height=38",
        L"Width=39",
        L"MinWidth=Auto"}},
}};

const Theme g_themeMatter = {{
    ThemeTargetStyles{L"Taskbar.TaskbarFrame > Grid#RootGrid > Taskbar.TaskbarBackground > Grid > Rectangle#BackgroundFill", {
        L"Fill := $transparent"}},
    ThemeTargetStyles{L"Rectangle#BackgroundStroke", {
        L"Fill := $transparent"}},
    ThemeTargetStyles{L"Taskbar.TaskbarBackground#HoverFlyoutBackgroundControl", {
        L"Fill:=$base",
        L"CornerRadius = $mainRadius"}},
    ThemeTargetStyles{L"Taskbar.AugmentedEntryPointButton#AugmentedEntryPointButton", {
        L"Margin=-1,1,1,1"}},
    ThemeTargetStyles{L"Taskbar.TaskListButtonPanel@CommonStates > Border#BackgroundElement", {
        L"CornerRadius = $mainRadius",
        L"Background :=$base",
        L"Background@InactivePointerOver :=$overlay2",
        L"Background@ActivePointerOver:=$overlay",
        L"Background@ActiveNormal :=$active"}},
    ThemeTargetStyles{L"Taskbar.ExperienceToggleButton#LaunchListButton[AutomationProperties.Name=Task View]", {
        L"Margin=0,0,2,0"}},
    ThemeTargetStyles{L"Taskbar.TaskListButton#TaskListButton[AutomationProperties.Name=Copilot] > Taskbar.TaskListLabeledButtonPanel#IconPanel > Border#BackgroundElement", {
        L"Visibility = 1"}},
    ThemeTargetStyles{L"Taskbar.SearchBoxButton", {
        L"Margin=0,0,2,0"}},
    ThemeTargetStyles{L"Border#BackgroundElement", {
        L"BorderThickness=0"}},
    ThemeTargetStyles{L"Taskbar.TaskListLabeledButtonPanel@CommonStates > Border#BackgroundElement", {
        L"Background@InactiveNormal :=$base",
        L"Background@ActiveNormal :=$active",
        L"Background@InactivePointerOver :=$overlay2",
        L"Background@ActivePointerOver:=$overlay",
        L"CornerRadius = $mainRadius",
        L"Margin = 1,0,1,0",
        L"Background@MultiWindowNormal:=$base",
        L"Background@MultiWindowPointerOver:=$overlay2",
        L"Background@MultiWindowActive:=$active",
        L"Background@MultiWindowPressed:=$overlay"}},
    ThemeTargetStyles{L"Border#MultiWindowElement", {
        L"CornerRadius = $mainRadius",
        L"Padding = 7,0,8,0",
        L"Background :=$accentColor"}},
    ThemeTargetStyles{L"Taskbar.TaskListLabeledButtonPanel > TextBlock#LabelControl", {
        L"Margin=0,0,2,0"}},
    ThemeTargetStyles{L"Taskbar.TaskListLabeledButtonPanel@RunningIndicatorStates > Rectangle#RunningIndicator", {
        L"Fill := $inverseBW",
        L"RadiusX=1.5",
        L"RadiusY=1.5",
        L"Height=4",
        L"Width=12",
        L"Fill@ActiveRunningIndicator :=$accentColor",
        L"Width@ActiveRunningIndicator=21"}},
    ThemeTargetStyles{L"Grid#SystemTrayFrameGrid", {
        L"Background:=$base",
        L"CornerRadius = $mainRadius",
        L"Margin=0,5,12,5",
        L"Padding=5,0,0,0"}},
    ThemeTargetStyles{L"Border#BackgroundBorder", {
        L"Margin=2,5,2,5",
        L"CornerRadius=8",
        L"BorderThickness = 0"}},
    ThemeTargetStyles{L"Grid#OverflowRootGrid > Border", {
        L"Background:=$base",
        L"Shadow :=",
        L"CornerRadius = 14"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Shapes.Rectangle#HorizontalTrackRect", {
        L"Height = 8",
        L"Margin = 0",
        L"Fill := $overlay"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Shapes.Rectangle#HorizontalDecreaseRect", {
        L"Height = 8"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.TextBlock#volumeLevelText", {
        L"FontFamily = Tektur",
        L"Margin = 0,-2,0,0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#VolumeConfirmator", {
        L"Padding = 8,0,3,0",
        L"CornerRadius = 20"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#ConfirmatorMainGrid", {
        L"Background :=$base",
        L"CornerRadius = 14",
        L"BorderThickness = 0",
        L"Margin = 0,0,0,10",
        L"Shadow :="}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#BrightnessConfirmator", {
        L"Padding = 15,0,17,0",
        L"CornerRadius = 20"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.AnimatedIcon#BrightnessIcon", {
        L"Margin = 0,-1,12,0"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.ProgressBar#ProgressIndicator", {
        L"Margin = 0,0,0,1"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Shapes.Rectangle#ProgressBarTrack", {
        L"Fill := $inverseBW",
        L"RadiusX = 1.5",
        L"RadiusY = 1.5"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Shapes.Rectangle#DeterminateProgressBarIndicator", {
        L"Fill :=$accentColor"}},
    ThemeTargetStyles{L"Taskbar.TaskListButton#TaskListButton > Taskbar.TaskListLabeledButtonPanel#IconPanel > Microsoft.UI.Xaml.Controls.ProgressBar#ProgressIndicator", {
        L"MinHeight = 4",
        L"Width = 26"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.ContentPresenter#ContentPresenter", {
        L"BorderThickness = 0"}},
    ThemeTargetStyles{L"Taskbar.ExperienceToggleButton#LaunchListButton[AutomationProperties.Name=Start]", {
        L"Margin = 0,0,2,0"}},
    ThemeTargetStyles{L"Taskbar.Badge#BadgeControl", {
        L"Height = 14",
        L"MinWidth = 14",
        L"Margin = 0,0,0,0",
        L"CornerRadius = 20"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Shapes.Rectangle#BackgroundRect", {
        L"RadiusX = 4",
        L"RadiusY = 4"}},
    ThemeTargetStyles{L"MenuFlyoutPresenter", {
        L"Background := $base",
        L"Shadow :=",
        L"CornerRadius = 8"}},
    ThemeTargetStyles{L"SearchUx.SearchUI.SearchButtonRootGrid@CommonStates > Border#BackgroundElement", {
        L"Background@InactiveNormal :=$base",
        L"CornerRadius = 8"}},
}, {
    L"mainRadius = 8",
    L"transparent = <SolidColorBrush Color=\"Transparent\"/>",
    L"base = <AcrylicBrush TintColor=\"{ThemeResource SystemAltLowColor}\" TintOpacity=\"1\" TintLuminosityOpacity=\"0.7\" FallbackColor=\"{ThemeResource SystemChromeLowColor}\" />",
    L"overlay = <AcrylicBrush TintColor=\"{ThemeResource SystemAltLowColor}\" TintOpacity=\"1\" TintLuminosityOpacity=\"0.8\" FallbackColor=\"{ThemeResource CardStrokeColorDefaultSolid}\" />",
    L"overlay2 = <AcrylicBrush TintColor=\"{ThemeResource SystemAltLowColor}\" TintOpacity=\"1\" TintLuminosityOpacity=\"0.5\" FallbackColor=\"{ThemeResource CardStrokeColorDefaultSolid}\" />",
    L"accentColor = <SolidColorBrush Color=\"{ThemeResource SystemAccentColor}\" Opacity = \"1\" />",
    L"inverseBW = <SolidColorBrush Color=\"{ThemeResource SystemBaseHighColor}\" Opacity = \"1\" />",
    L"active = <AcrylicBrush TintColor=\"{ThemeResource SystemAltLowColor}\" TintOpacity=\"1\" TintLuminosityOpacity=\"1\" FallbackColor=\"{ThemeResource CardStrokeColorDefaultSolid}\" />",
}};

const Theme g_themeWinXP = {{
    ThemeTargetStyles{L"Rectangle#BackgroundStroke", {
        L"Fill:=<LinearGradientBrush StartPoint=\"0.5,0\" EndPoint=\"0.5,1\"> <GradientStop Color=\"#3168d5\" Offset=\"0.0\" /> <GradientStop Color=\"#4993E6\" Offset=\"0.1\" /> <GradientStop Color=\"#2157D7\" Offset=\"0.35\" /> <GradientStop Color=\"#2663E0\" Offset=\"0.8\" /> <GradientStop Color=\"#1941A5\" Offset=\"1.0\" /></LinearGradientBrush>",
        L"VerticalAlignment=Stretch",
        L"Height=Auto"}},
    ThemeTargetStyles{L"Taskbar.TaskbarBackground#HoverFlyoutBackgroundControl > Grid > Rectangle#BackgroundFill", {
        L"Fill:=<LinearGradientBrush StartPoint=\"0.5,0\" EndPoint=\"0.5,1\"> <GradientStop Color=\"#3168d5\" Offset=\"0.0\" /> <GradientStop Color=\"#4993E6\" Offset=\"0.1\" /> <GradientStop Color=\"#2157D7\" Offset=\"0.35\" /> <GradientStop Color=\"#2663E0\" Offset=\"0.8\" /> <GradientStop Color=\"#1941A5\" Offset=\"1.0\" /></LinearGradientBrush>"}},
    ThemeTargetStyles{L"Taskbar.ExperienceToggleButton#LaunchListButton[AutomationProperties.AutomationId=StartButton]", {
        L"Margin=-9,0,10,0"}},
    ThemeTargetStyles{L"Taskbar.ExperienceToggleButton#LaunchListButton[AutomationProperties.AutomationId=StartButton] > Taskbar.TaskListButtonPanel", {
        L"Padding=0",
        L"Width=60",
        L"CornerRadius=9",
        L"Background:=<LinearGradientBrush StartPoint=\"0.5,0\" EndPoint=\"0.5,1\"> <GradientStop Color=\"#2D6B2D\" Offset=\"0.0\" /> <GradientStop Color=\"#7ED57E\" Offset=\"0.08\" /> <GradientStop Color=\"#3DB43D\" Offset=\"0.35\" /> <GradientStop Color=\"#2A752E\" Offset=\"0.85\" /> <GradientStop Color=\"#144818\" Offset=\"1.0\" /></LinearGradientBrush>",
        L"BorderThickness=0,0,2,0",
        L"BorderBrush:=<LinearGradientBrush StartPoint=\"0.5,0\" EndPoint=\"0.5,1\"> <GradientStop Color=\"#400D330D\" Offset=\"0.0\" /> <GradientStop Color=\"#800D330D\" Offset=\"0.4\" /> <GradientStop Color=\"#FF0D330D\" Offset=\"1.0\" /></LinearGradientBrush>"}},
    ThemeTargetStyles{L"Taskbar.ExperienceToggleButton#LaunchListButton[AutomationProperties.AutomationId=StartButton] > Taskbar.TaskListButtonPanel > Border#BackgroundElement", {
        L"Background:=<ImageBrush Stretch=\"None\" ImageSource=\"https://raw.githubusercontent.com/ramensoftware/windows-11-taskbar-styling-guide/refs/heads/main/Themes/WinXP/Assets/orb.png\" />"}},
    ThemeTargetStyles{L"Taskbar.ExperienceToggleButton#LaunchListButton[AutomationProperties.AutomationId=StartButton] > Taskbar.TaskListButtonPanel > Microsoft.UI.Xaml.Controls.AnimatedVisualPlayer#Icon", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"TextBlock#LabelControl", {
        L"Foreground=White"}},
    ThemeTargetStyles{L"Rectangle#RunningIndicator", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"TextBlock#TimeInnerTextBlock", {
        L"Foreground=White"}},
    ThemeTargetStyles{L"TextBlock#DateInnerTextBlock", {
        L"Foreground=White"}},
    ThemeTargetStyles{L"SystemTray.TextIconContent > Grid > SystemTray.AdaptiveTextBlock#Base > TextBlock", {
        L"Foreground=White"}},
    ThemeTargetStyles{L"SystemTray.BatteryIconContent > Grid#ContainerGrid > StackPanel > Grid > TextBlock[1]", {
        L"Foreground=White"}},
    ThemeTargetStyles{L"Taskbar.TaskListLabeledButtonPanel@RunningIndicatorStates > Border#BackgroundElement", {
        L"Background@NoRunningIndicator=Transparent",
        L"Background@ActiveRunningIndicator:=<LinearGradientBrush StartPoint=\"0.5,0\" EndPoint=\"0.5,1\"> <GradientStop Color=\"#1542A8\" Offset=\"0.0\" /> <GradientStop Color=\"#245DD4\" Offset=\"0.2\" /> <GradientStop Color=\"#1542A8\" Offset=\"1.0\" /></LinearGradientBrush>",
        L"Background:=<LinearGradientBrush StartPoint=\"0.5,0\" EndPoint=\"0.5,1\"> <GradientStop Color=\"#6599E6\" Offset=\"0.0\" /> <GradientStop Color=\"#4A88E0\" Offset=\"0.1\" /> <GradientStop Color=\"#4282D9\" Offset=\"0.9\" /> <GradientStop Color=\"#2A62B5\" Offset=\"1.0\" /></LinearGradientBrush>",
        L"BorderBrush@NoRunningIndicator=Transparent",
        L"BorderBrush@ActiveRunningIndicator=#083192",
        L"BorderBrush=#1A4DBF"}},
    ThemeTargetStyles{L"Taskbar.TaskListButton", {
        L"Margin=-2"}},
    ThemeTargetStyles{L"Grid#SystemTrayFrameGrid", {
        L"Background:=<LinearGradientBrush StartPoint=\"0.5,0\" EndPoint=\"0.5,1\"> <GradientStop Color=\"#16ADF0\" Offset=\"0.0\" /> <GradientStop Color=\"#19B9F3\" Offset=\"0.1\" /> <GradientStop Color=\"#118FE9\" Offset=\"0.35\" /> <GradientStop Color=\"#0E9EF0\" Offset=\"0.8\" /> <GradientStop Color=\"#1580D9\" Offset=\"1.0\" /></LinearGradientBrush>",
        L"BorderThickness=1,1,0,1",
        L"BorderBrush=#095BC9",
        L"Padding=4,-1,0,-1"}},
    ThemeTargetStyles{L"Grid#OverflowRootGrid > Border", {
        L"Background:=<LinearGradientBrush StartPoint=\"0.5,0\" EndPoint=\"0.5,1\"> <GradientStop Color=\"#3168d5\" Offset=\"0.0\" /> <GradientStop Color=\"#4993E6\" Offset=\"0.1\" /> <GradientStop Color=\"#2157D7\" Offset=\"0.35\" /> <GradientStop Color=\"#2663E0\" Offset=\"0.8\" /> <GradientStop Color=\"#1941A5\" Offset=\"1.0\" /></LinearGradientBrush>"}},
}};

const Theme g_themeWinXP_variant_Zune = {{
    ThemeTargetStyles{L"Rectangle#BackgroundFill", {
        L"Fill:=<LinearGradientBrush StartPoint=\"0.5,0.5\" EndPoint=\"0.5,1\"> <GradientStop Color=\"#656565\" Offset=\"0.0\" /> <GradientStop Color=\"#363636\" Offset=\"0.1\" /> <GradientStop Color=\"#363636\" Offset=\"0.35\" /> <GradientStop Color=\"#363636\" Offset=\"0.8\" /> <GradientStop Color=\"#363636\" Offset=\"1.0\" /></LinearGradientBrush>",
        L"VerticalAlignment=Stretch",
        L"Height=Auto"}},
    ThemeTargetStyles{L"Taskbar.ExperienceToggleButton#LaunchListButton[AutomationProperties.AutomationId=StartButton]", {
        L"CornerRadius=0",
        L"Margin=-4,0,4,0",
        L"MaxWidth=48"}},
    ThemeTargetStyles{L"Taskbar.ExperienceToggleButton#LaunchListButton[AutomationProperties.AutomationId=StartButton] > Taskbar.TaskListButtonPanel", {
        L"Padding=0",
        L"Background:=<LinearGradientBrush StartPoint=\"0.5,0.5\" EndPoint=\"0.5,1\"> <GradientStop Color=\"#D76A27\" Offset=\"0.05\" /> <GradientStop Color=\"#B44704\" Offset=\"0.1\" /> <GradientStop Color=\"#772E01\" Offset=\"0.5\" /> <GradientStop Color=\"#772E01\" Offset=\"1\" /> <GradientStop Color=\"#AA4201\" Offset=\"1\" /></LinearGradientBrush>"}},
    ThemeTargetStyles{L"Taskbar.ExperienceToggleButton#LaunchListButton[AutomationProperties.AutomationId=StartButton] > Taskbar.TaskListButtonPanel > Border#BackgroundElement", {
        L"Background:=<ImageBrush Stretch=\"Uniform\" ImageSource=\"https://raw.githubusercontent.com/ramensoftware/windows-11-taskbar-styling-guide/refs/heads/main/Themes/WinXP/Assets/orb.png\" />",
        L"Height=32"}},
    ThemeTargetStyles{L"Taskbar.ExperienceToggleButton#LaunchListButton[AutomationProperties.AutomationId=StartButton] > Taskbar.TaskListButtonPanel > Microsoft.UI.Xaml.Controls.AnimatedVisualPlayer#Icon", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"TextBlock#LabelControl", {
        L"Foreground=White"}},
    ThemeTargetStyles{L"Rectangle#RunningIndicator", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"TextBlock#TimeInnerTextBlock", {
        L"Foreground=White"}},
    ThemeTargetStyles{L"TextBlock#DateInnerTextBlock", {
        L"Foreground=White"}},
    ThemeTargetStyles{L"SystemTray.TextIconContent > Grid > SystemTray.AdaptiveTextBlock#Base > TextBlock", {
        L"Foreground=White"}},
    ThemeTargetStyles{L"Taskbar.TaskListLabeledButtonPanel@RunningIndicatorStates > Border", {
        L"BorderThickness=1",
        L"CornerRadius=2",
        L"BorderBrush@NoRunningIndicator=Transparent",
        L"Margin=-2,-1,-2,-1"}},
    ThemeTargetStyles{L"Taskbar.TaskListLabeledButtonPanel@RunningIndicatorStates > Border#BackgroundElement", {
        L"BorderBrush=#BB4B4B4B",
        L"Margin=1",
        L"BorderThickness=1",
        L"Background:=<LinearGradientBrush StartPoint=\"0.5,0.42\" EndPoint=\"0.5,0.75\"> <GradientStop Color=\"#6B6B6B\" Offset=\"0.0\" /> <GradientStop Color=\"#363636\" Offset=\"0.5\" /> <GradientStop Color=\"#363636\" Offset=\"0.35\" /> <GradientStop Color=\"#363636\" Offset=\"0.8\" /> <GradientStop Color=\"#363636\" Offset=\"1.0\" /></LinearGradientBrush>",
        L"Background@ActiveRunningIndicator:=<LinearGradientBrush StartPoint=\"0.5,0.5\" EndPoint=\"0.5,1\"> <GradientStop Color=\"#6B6B6B\" Offset=\"0.0\" /> <GradientStop Color=\"#434343\" Offset=\"0.1\" /> <GradientStop Color=\"#434343\" Offset=\"0.35\" /> <GradientStop Color=\"#434343\" Offset=\"0.8\" /> <GradientStop Color=\"#434343\" Offset=\"1.0\" /></LinearGradientBrush>",
        L"BorderBrush@NoRunningIndicator=Transparent",
        L"Background@NoRunningIndicator=Transparent"}},
    ThemeTargetStyles{L"Rectangle#BackgroundStroke", {
        L"Fill=#858585"}},
    ThemeTargetStyles{L"Grid#SystemTrayFrameGrid", {
        L"Background:=<LinearGradientBrush StartPoint=\"0.5,0.42\" EndPoint=\"0.5,0.75\"> <GradientStop Color=\"#454545\" Offset=\"0.0\" /> <GradientStop Color=\"#313131\" Offset=\"0.5\" /> <GradientStop Color=\"#363636\" Offset=\"0.35\" /> <GradientStop Color=\"#1D1D1D\" Offset=\"0.8\" /> <GradientStop Color=\"#1D1D1D\" Offset=\"1.0\" /></LinearGradientBrush>",
        L"BorderThickness=1,0,0,0",
        L"BorderBrush=#222222",
        L"Padding=4,0,0,0"}},
    ThemeTargetStyles{L"Taskbar.TaskListButton#TaskListButton > Taskbar.TaskListLabeledButtonPanel#IconPanel@RunningIndicatorStates > Windows.UI.Xaml.Controls.Image#Icon", {
        L"Height@NoRunningIndicator=16"}},
    ThemeTargetStyles{L"Taskbar.TaskListButton#TaskListButton > Taskbar.TaskListLabeledButtonPanel#IconPanel@RunningIndicatorStates", {
        L"Margin@NoRunningIndicator=-7,0,-7,0",
        L"Padding@NoRunningIndicator=0"}},
    ThemeTargetStyles{L"Taskbar.TaskListButton", {
        L"Margin=-1.5"}},
    ThemeTargetStyles{L"Grid#OverflowRootGrid > Border", {
        L"Background:=<LinearGradientBrush StartPoint=\"0.5,0.5\" EndPoint=\"0.5,1\"> <GradientStop Color=\"#656565\" Offset=\"0.0\" /> <GradientStop Color=\"#363636\" Offset=\"0.1\" /> <GradientStop Color=\"#363636\" Offset=\"0.35\" /> <GradientStop Color=\"#363636\" Offset=\"0.8\" /> <GradientStop Color=\"#363636\" Offset=\"1.0\" /></LinearGradientBrush>"}},
}};

const Theme g_themeBubbles = {{
    ThemeTargetStyles{L"Rectangle#BackgroundFill", {
        L"Fill:=<SolidColorBrush x:Name=\"SystemChromeLow\" Color=\"{ThemeResource SystemChromeLowColor}\" />"}},
    ThemeTargetStyles{L"Taskbar.TaskListLabeledButtonPanel@RunningIndicatorStates > Border#BackgroundElement", {
        L"CornerRadius=20",
        L"Background@NoRunningIndicator:=<SolidColorBrush x:Name=\"SystemChromeHigh\" Opacity=\"0.18\" Color=\"{ThemeResource SystemChromeHighColor}\" />",
        L"Background:=<SolidColorBrush x:Name=\"SearchBoxTextBlock\" Opacity=\"0.15\" Color=\"{ThemeResource SearchPillButtonForeground}\" />",
        L"BorderThickness=1.5",
        L"BorderBrush:=<SolidColorBrush x:Name=\"SearchBoxTextBlock\" Opacity=\"0.25\" Color=\"{ThemeResource SearchPillButtonForeground}\" />",
        L"BorderThickness@NoRunningIndicator=1",
        L"BorderBrush@NoRunningIndicator:=<SolidColorBrush x:Name=\"SearchBoxTextBlock\" Opacity=\"0.15\" Color=\"{ThemeResource SearchPillButtonForeground}\" />",
        L"Margin=1"}},
    ThemeTargetStyles{L"Taskbar.TaskListButtonPanel@CommonStates > Border#BackgroundElement", {
        L"Background:=<SolidColorBrush x:Name=\"SystemChromeHigh\" Opacity=\"0.3\" Color=\"{ThemeResource SystemChromeHighColor}\" />",
        L"BorderBrush:=<SolidColorBrush x:Name=\"SystemChromeHigh\" Opacity=\"0.6\" Color=\"{ThemeResource SystemChromeHighColor}\" />",
        L"Background@ActivePointerOver:=<SolidColorBrush x:Name=\"SystemChromeHigh\" Opacity=\"0.8\" Color=\"{ThemeResource SystemChromeHighColor}\" />",
        L"Background@InactivePointerOver:=<SolidColorBrush x:Name=\"SystemChromeHigh\" Opacity=\"0.8\" Color=\"{ThemeResource SystemChromeHighColor}\" />",
        L"Background@ActivePressed:=<SolidColorBrush x:Name=\"SystemChromeHigh\" Opacity=\"1\" Color=\"{ThemeResource SystemChromeHighColor}\" />",
        L"Background@InactivePressed:=<SolidColorBrush x:Name=\"SystemChromeHigh\" Opacity=\"1\" Color=\"{ThemeResource SystemChromeHighColor}\" />",
        L"BorderBrush@InactivePressed:=<SolidColorBrush x:Name=\"SystemChromeHigh\" Opacity=\"0.8\" Color=\"{ThemeResource SystemAccentColor}\" />",
        L"CornerRadius=20",
        L"BorderThickness@InactivePressed=3",
        L"BorderThickness=2"}},
    ThemeTargetStyles{L"Grid#SystemTrayFrameGrid", {
        L"Background:=<SolidColorBrush x:Name=\"SystemChromeHigh\" Opacity=\"0.6\" Color=\"{ThemeResource SystemChromeHighColor}\" />",
        L"CornerRadius=20",
        L"Margin=-5,5,8,5",
        L"Padding=10,0,-10,0",
        L"BorderBrush:=<SolidColorBrush x:Name=\"SystemChromeHigh\" Opacity=\"0.9\" Color=\"{ThemeResource SystemChromeHighColor}\" />",
        L"BorderThickness=1.5"}},
    ThemeTargetStyles{L"Taskbar.TaskListLabeledButtonPanel@CommonStates > Rectangle#RunningIndicator", {
        L"Stroke@InactivePointerOver=#75A8E6",
        L"Stroke@InactivePressed=#7CB1F2",
        L"Stroke@ActiveNormal=#5F87B9",
        L"Stroke@ActivePointerOver=#75A8E6",
        L"Stroke@ActivePressed=#7CB1F2",
        L"Fill=Transparent",
        L"RadiusX=20",
        L"RadiusY=20",
        L"StrokeThickness=3",
        L"Stroke@MultiWindowPointerOver=#CCCCDD",
        L"Stroke@MultiWindowPressed=White",
        L"Stroke@MultiWindowActive=#BBBBCC",
        L"Fill@MultiWindowNormal=#88AAAABB",
        L"Fill@MultiWindowPointerOver=#88AAAABB",
        L"Fill@MultiWindowActive=#88AAAABB",
        L"Fill@MultiWindowPressed=#88AAAABB",
        L"Stroke@RequestingAttention=Crimson",
        L"Stroke@RequestingAttentionPointerOver=Red",
        L"Fill@RequestingAttention:=<SolidColorBrush Opacity=\"0.4\" Color=\"DarkOrange\" />",
        L"Fill@RequestingAttentionPointerOver:=<SolidColorBrush Opacity=\"0.4\" Color=\"Orange\" />",
        L"StrokeThickness@RequestingAttention=2.5",
        L"StrokeThickness@RequestingAttentionPointerOver=2.5",
        L"Height=39",
        L"Width=39",
        L"MinWidth=Auto"}},
    ThemeTargetStyles{L"Taskbar.TaskListLabeledButtonPanel > TextBlock#LabelControl", {
        L"Margin=4,0,0,0"}},
    ThemeTargetStyles{L"Taskbar.SearchBoxButton", {
        L"Height=48",
        L"Margin=0,-2,0,0"}},
    ThemeTargetStyles{L"Border#MultiWindowElement", {
        L"Height=0"}},
    ThemeTargetStyles{L"Grid#OverflowRootGrid > Border", {
        L"Background:=<SolidColorBrush x:Name=\"SystemChromeHigh\" Opacity=\"0.8\" Color=\"{ThemeResource SystemChromeHighColor}\" />",
        L"BorderBrush:=<SolidColorBrush x:Name=\"SearchBoxTextBlock\" Opacity=\"0.1\" Color=\"{ThemeResource SearchPillButtonForeground}\" />"}},
    ThemeTargetStyles{L"Taskbar.ExperienceToggleButton#LaunchListButton[AutomationProperties.AutomationId=StartButton] > Taskbar.TaskListButtonPanel > Microsoft.UI.Xaml.Controls.AnimatedVisualPlayer#Icon", {
        L"Margin=1,0,0,0"}},
    ThemeTargetStyles{L"SystemTray.Stack#ShowDesktopStack", {
        L"Padding=5,0,5,0",
        L"Margin=2,0,10,0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Shapes.Rectangle#ShowDesktopPipe", {
        L"MinWidth=4",
        L"RadiusX=2",
        L"RadiusY=2",
        L"Margin=-5,0,5,0"}},
    ThemeTargetStyles{L"SystemTray.Stack#NotifyIconStack > Windows.UI.Xaml.Controls.Grid > SystemTray.StackListView > Windows.UI.Xaml.Controls.ItemsPresenter > Windows.UI.Xaml.Controls.StackPanel > Windows.UI.Xaml.Controls.ContentPresenter > SystemTray.ChevronIconView > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.Border#BackgroundBorder", {
        L"CornerRadius=16,5,5,16",
        L"Margin=-3,4,0,4"}},
    ThemeTargetStyles{L"TextBlock#InnerTextBlock", {
        L"Foreground:=<SolidColorBrush x:Name=\"SearchBoxTextBlock\" Color=\"{ThemeResource SearchPillButtonForeground}\" />"}},
    ThemeTargetStyles{L"TextBlock#LabelControl", {
        L"Foreground:=<SolidColorBrush x:Name=\"SearchBoxTextBlock\" Color=\"{ThemeResource SearchPillButtonForeground}\" />"}},
    ThemeTargetStyles{L"TextBlock#TimeInnerTextBlock", {
        L"Foreground:=<SolidColorBrush x:Name=\"SearchBoxTextBlock\" Color=\"{ThemeResource SearchPillButtonForeground}\" />",
        L"Margin=0,2.5,0,-2.5"}},
    ThemeTargetStyles{L"TextBlock#DateInnerTextBlock", {
        L"Foreground:=<SolidColorBrush x:Name=\"SearchBoxTextBlock\" Color=\"{ThemeResource SearchPillButtonForeground}\" />",
        L"Margin=0,-1,0,2"}},
    ThemeTargetStyles{L"Grid#ContainerGrid@ > Border#BackgroundBorder", {
        L"Background@PointerOver:=<SolidColorBrush x:Name=\"SearchBoxTextBlock\" Opacity=\"0.2\" Color=\"{ThemeResource SearchPillButtonForeground}\" />",
        L"CornerRadius=20",
        L"Margin=-1",
        L"Height=28",
        L"Width=28",
        L"BorderBrush=Transparent"}},
    ThemeTargetStyles{L"SystemTray.IconView > Grid#ContainerGrid@ > Border#BackgroundBorder", {
        L"CornerRadius=20",
        L"Background@PointerOver:=<SolidColorBrush x:Name=\"SearchBoxTextBlock\" Opacity=\"0.15\" Color=\"{ThemeResource SearchPillButtonForeground}\" />",
        L"Height=34",
        L"Width=34"}},
    ThemeTargetStyles{L"SystemTray.OmniButton#NotificationCenterButton > Grid@CommonStates > Border#BackgroundBorder", {
        L"CornerRadius=20",
        L"Width=75",
        L"Margin=-2,1,-2,1",
        L"Background@PointerOver:=<SolidColorBrush x:Name=\"SearchBoxTextBlock\" Opacity=\"0.15\" Color=\"{ThemeResource SearchPillButtonForeground}\" />"}},
    ThemeTargetStyles{L"SystemTray.OmniButton#ControlCenterButton > Grid@CommonStates > Border#BackgroundBorder", {
        L"CornerRadius=20",
        L"Background@PointerOver:=<SolidColorBrush x:Name=\"SearchBoxTextBlock\" Opacity=\"0.15\" Color=\"{ThemeResource SearchPillButtonForeground}\" />",
        L"Margin=1"}},
    ThemeTargetStyles{L"Taskbar.AugmentedEntryPointButton > Taskbar.TaskListButtonPanel@CommonStates", {
        L"Background:=<SolidColorBrush x:Name=\"SystemChromeHigh\" Opacity=\"0.6\" Color=\"{ThemeResource SystemChromeHighColor}\" />",
        L"BorderBrush:=<SolidColorBrush x:Name=\"SystemChromeHigh\" Opacity=\"0.9\" Color=\"{ThemeResource SystemChromeHighColor}\" />",
        L"BorderThickness=1.5",
        L"Margin=5",
        L"Background@InactivePointerOver:=<SolidColorBrush x:Name=\"SystemChromeHigh\" Opacity=\"1\" Color=\"{ThemeResource SystemChromeHighColor}\" />",
        L"Padding=-1.5,-1,-1.5,-1",
        L"CornerRadius=20"}},
    ThemeTargetStyles{L"Grid#OverflowRootGrid", {
        L"Background:=<SolidColorBrush x:Name=\"SystemChromeHigh\" Opacity=\"0.5\" Color=\"{ThemeResource SystemChromeHighColor}\" />",
        L"Padding=0",
        L"Margin=0,0,0,12",
        L"CornerRadius=8"}},
    ThemeTargetStyles{L"Rectangle#LeftDropInsertionMarker", {
        L"Fill:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColorLight1}\" />"}},
    ThemeTargetStyles{L"Rectangle#RightDropInsertionMarker", {
        L"Fill:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColorLight1}\" />"}},
    ThemeTargetStyles{L"Taskbar.ExperienceToggleButton#LaunchListButton > Taskbar.TaskListButtonPanel > Border", {
        L"CornerRadius=20",
        L"Background:=<SolidColorBrush x:Name=\"SearchBoxTextBlock\" Opacity=\"0.15\" Color=\"{ThemeResource SearchPillButtonForeground}\" />",
        L"BorderBrush:=<SolidColorBrush x:Name=\"SearchBoxTextBlock\" Opacity=\"0.25\" Color=\"{ThemeResource SearchPillButtonForeground}\" />"}},
}};

const Theme g_themeRosePine = {{
    ThemeTargetStyles{L"Taskbar.TaskListButton", {
        L"CornerRadius=3"}},
    ThemeTargetStyles{L"SystemTray.TextIconContent > Grid#ContainerGrid > SystemTray.AdaptiveTextBlock#Base > TextBlock#InnerTextBlock", {
        L"FontSize=16"}},
    ThemeTargetStyles{L"SystemTray.NotifyIconView#NotifyItemIcon", {
        L"MinWidth=25"}},
    ThemeTargetStyles{L"SystemTray.OmniButton#ControlCenterButton > Grid > ContentPresenter > ItemsPresenter > StackPanel > ContentPresenter[1] > SystemTray.IconView > Grid > Grid", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"SystemTray.TextIconContent > Grid#ContainerGrid", {
        L"Padding=2"}},
    ThemeTargetStyles{L"SystemTray.ChevronIconView", {
        L"MinWidth=27"}},
    ThemeTargetStyles{L"SystemTray.OmniButton#NotificationCenterButton > Grid > ContentPresenter > ItemsPresenter > StackPanel > ContentPresenter > SystemTray.IconView#SystemTrayIcon > Grid > Grid > SystemTray.TextIconContent", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Taskbar.TaskListLabeledButtonPanel > Border#BackgroundElement", {
        L"Background:=#302d47",
        L"CornerRadius=6"}},
    ThemeTargetStyles{L"Grid#SystemTrayFrameGrid", {
        L"Background:=#302d47",
        L"CornerRadius=6",
        L"Margin=0,5,4,4",
        L"Padding=3,0,-8,0"}},
    ThemeTargetStyles{L"Taskbar.TaskListLabeledButtonPanel@CommonStates > Rectangle#RunningIndicator", {
        L"Height=27",
        L"RadiusX=5",
        L"RadiusY=5",
        L"StrokeThickness=2",
        L"Stroke@InactivePointerOver=#ebbcba",
        L"Stroke@InactivePressed=#ebbcba",
        L"Stroke@ActiveNormal=#ebbcba",
        L"Stroke@ActivePointerOver=#ebbcba",
        L"Stroke@ActivePressed=#ebbcba",
        L"Fill=Transparent",
        L"Width=37",
        L"VerticalAlignment=1",
        L"Canvas.ZIndex=1"}},
    ThemeTargetStyles{L"SystemTray.ImageIconContent > Grid#ContainerGrid > Image", {
        L"Width=13"}},
    ThemeTargetStyles{L"SystemTray.TextIconContent > Grid#ContainerGrid > SystemTray.AdaptiveTextBlock#Base > TextBlock#InnerTextBlock", {
        L"FontSize=14"}},
    ThemeTargetStyles{L"TextBlock#LabelControl", {
        L"FontFamily=JetBrainsMono NF",
        L"Foreground=#e0def4",
        L"Padding=2,0,8,0"}},
    ThemeTargetStyles{L"Taskbar.ExperienceToggleButton#LaunchListButton[AutomationProperties.AutomationId=StartButton]", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.TextBlock#InnerTextBlock[Text=\uE971]", {
        L"Text=\uE712"}},
    ThemeTargetStyles{L"Taskbar.TaskbarFrame > Grid#RootGrid > Taskbar.TaskbarBackground > Grid > Rectangle#BackgroundFill", {
        L"Fill=Transparent"}},
    ThemeTargetStyles{L"Taskbar.TaskbarBackground#HoverFlyoutBackgroundControl > Grid > Rectangle#BackgroundFill", {
        L"Fill=#302d47"}},
    ThemeTargetStyles{L"Rectangle#BackgroundStroke", {
        L"Fill=Transparent"}},
    ThemeTargetStyles{L"Taskbar.TaskListButtonPanel#ExperienceToggleButtonRootPanel > Border#BackgroundElement", {
        L"Background=#302d47"}},
    ThemeTargetStyles{L"TextBlock#DateInnerTextBlock", {
        L"Margin=0,0,0,-2"}},
    ThemeTargetStyles{L"Grid#OverflowRootGrid > Border", {
        L"Background=#302d47"}},
}};

const Theme g_themeWinVista = {{
    ThemeTargetStyles{L"Taskbar.ExperienceToggleButton", {
        L"CornerRadius=2"}},
    ThemeTargetStyles{L"Taskbar.SearchBoxButton", {
        L"CornerRadius=2"}},
    ThemeTargetStyles{L"Taskbar.TaskListButton", {
        L"CornerRadius=2"}},
    ThemeTargetStyles{L"Taskbar.TaskListLabeledButtonPanel@RunningIndicatorStates > Rectangle#RunningIndicator", {
        L"Height=2",
        L"Width@ActiveRunningIndicator=30",
        L"Width@InactiveRunningIndicator=8",
        L"Fill@ActiveRunningIndicator=#00BEE0",
        L"Fill@InactiveRunningIndicator=#DDDDDD"}},
    ThemeTargetStyles{L"Rectangle#BackgroundFill", {
        L"Fill:=<LinearGradientBrush StartPoint=\"0,0\" EndPoint=\"0,1\" Opacity=\"0.7\"><GradientStop Color=\"#B5B9BC\" Offset=\"0.0\" /><GradientStop Color=\"#B5B9BC\" Offset=\"0.03125\" /><GradientStop Color=\"#909296\" Offset=\"0.03125\" /><GradientStop Color=\"#464B51\" Offset=\"0.5\" /><GradientStop Color=\"#060F15\" Offset=\"0.5\" /><GradientStop Color=\"#040C11\" Offset=\"0.96875\" /><GradientStop Color=\"#000000\" Offset=\"0.96875\" /><GradientStop Color=\"#000000\" Offset=\"1.0\" /></LinearGradientBrush>"}},
    ThemeTargetStyles{L"Taskbar.TaskListLabeledButtonPanel@RunningIndicatorStates > Border", {
        L"Background@ActiveRunningIndicator:=<LinearGradientBrush StartPoint=\"0,0\" EndPoint=\"0,1\" Opacity=\"0.2\"><GradientStop Color=\"#111111\" Offset=\"0.0\" /><GradientStop Color=\"#111111\" Offset=\"1.0\" /></LinearGradientBrush>",
        L"CornerRadius=2",
        L"Background@RequestingAttentionRunningIndicator:=<LinearGradientBrush StartPoint=\"0,0\" EndPoint=\"0,1\" Opacity=\"0.2\"><GradientStop Color=\"#D53300\" Offset=\"0.0\" /><GradientStop Color=\"#111111\" Offset=\"1.0\" /></LinearGradientBrush>",
        L"BorderBrush=#33101010",
        L"BorderThickness=1",
        L"BorderBrush@NoRunningIndicator=Transparent",
        L"Background@NoRunningIndicator=Transparent",
        L"Background@ActiveRunningIndicator=#55BBBBBB",
        L"BorderBrush@ActiveRunningIndicator=#55212121"}},
    ThemeTargetStyles{L"Taskbar.TaskListLabeledButtonPanel@CommonStates > Border#BackgroundElement", {
        L"Margin=0,0,0,2",
        L"BorderThickness=1",
        L"Background@ActivePointerOver=#88DDDDDD",
        L"Background@ActiveNormal=#33BBBBBB",
        L"Background@InactivePointerOver=#33BBBBBB",
        L"BorderBrush@ActiveNormal=#44AAAAAA",
        L"BorderBrush@ActivePointerOver=#FF888888",
        L"BorderBrush@InactiveNormal=Transparent"}},
    ThemeTargetStyles{L"Taskbar.TaskListLabeledButtonPanel > TextBlock", {
        L"FontFamily=Segoe UI"}},
    ThemeTargetStyles{L"SystemTray.AdaptiveTextBlock#LanguageInnerTextBlock > TextBlock#InnerTextBlock", {
        L"FontFamily=Segoe UI"}},
    ThemeTargetStyles{L"TextBlock#TimeInnerTextBlock", {
        L"FontFamily=Segoe UI"}},
    ThemeTargetStyles{L"Grid", {
        L"RequestedTheme=2"}},
    ThemeTargetStyles{L"Taskbar.TaskbarBackground#BackgroundControl > Grid", {
        L"Background:=<AcrylicBrush TintColor=\"Transparent\" TintOpacity=\"0\" TintLuminosityOpacity=\"0.1\" Opacity=\"1\" />"}},
    ThemeTargetStyles{L"Border#MultiWindowElement", {
        L"Background=#BB212121",
        L"BorderThickness=0",
        L"Margin=0,2,1,4"}},
    ThemeTargetStyles{L"Grid#OverflowRootGrid > Border", {
        L"Background:=<LinearGradientBrush StartPoint=\"0,0\" EndPoint=\"0,1\" Opacity=\"0.7\"><GradientStop Color=\"#B5B9BC\" Offset=\"0.0\" /><GradientStop Color=\"#B5B9BC\" Offset=\"0.03125\" /><GradientStop Color=\"#909296\" Offset=\"0.03125\" /><GradientStop Color=\"#464B51\" Offset=\"0.5\" /><GradientStop Color=\"#060F15\" Offset=\"0.5\" /><GradientStop Color=\"#040C11\" Offset=\"0.96875\" /><GradientStop Color=\"#000000\" Offset=\"0.96875\" /><GradientStop Color=\"#000000\" Offset=\"1.0\" /></LinearGradientBrush>"}},
    ThemeTargetStyles{L"Grid#OverflowRootGrid", {
        L"Background:=<AcrylicBrush TintColor=\"Transparent\" TintOpacity=\"0\" TintLuminosityOpacity=\"0.1\" Opacity=\"1\" />",
        L"Padding=-1",
        L"Margin=0,6,0,6",
        L"CornerRadius=8"}},
}};

const Theme g_themeCleanSlate = {{
    ThemeTargetStyles{L"Taskbar.TaskbarFrame > Grid#RootGrid > Taskbar.TaskbarBackground > Grid > Rectangle#BackgroundFill", {
        L"Fill:=<AcrylicBrush TintColor=\"{ThemeResource SystemAccentColorDark2}\" TintOpacity=\"0.4\" FallbackColor=\"{ThemeResource SystemAccentColorDark1}\" />"}},
    ThemeTargetStyles{L"Taskbar.TaskListButtonPanel@CommonStates > Border#BackgroundElement", {
        L"CornerRadius=100",
        L"Background:=<AcrylicBrush TintColor=\"{ThemeResource SystemAccentColorDark2}\" TintOpacity=\"0.4\" FallbackColor=\"{ThemeResource SystemAccentColorDark2}\" />",
        L"Background@InactivePointerOver:=<AcrylicBrush TintColor=\"{ThemeResource SystemAccentColorDark1}\" TintOpacity=\"0.4\" FallbackColor=\"{ThemeResource SystemAccentColorDark2}\"/>",
        L"Background@ActivePointerOver:=<AcrylicBrush TintColor=\"{ThemeResource SystemAccentColorDark1}\" TintOpacity=\"0.6\" FallbackColor=\"{ThemeResource SystemAccentColorDark2}\" />",
        L"Background@ActiveNormal:=<AcrylicBrush TintColor=\"{ThemeResource SystemAccentColorDark1}\" TintOpacity=\"0.6\" FallbackColor=\"{ThemeResource SystemAccentColorDark2}\"/>",
        L"Background@InactivePressed:=<AcrylicBrush TintColor=\"{ThemeResource SystemAccentColorDark1}\" TintOpacity=\"0.6\" FallbackColor=\"{ThemeResource SystemAccentColorDark2}\" />",
        L"Background@ActivePressed:=<AcrylicBrush TintColor=\"Black\" TintOpacity=\"0.8\" />"}},
    ThemeTargetStyles{L"Grid#SystemTrayFrameGrid", {
        L"Background:=<AcrylicBrush TintColor=\"{ThemeResource SystemAccentColorDark2}\" TintOpacity=\"0.5\" FallbackColor=\"{ThemeResource SystemAccentColorDark2}\" />",
        L"CornerRadius=5",
        L"Margin=0,5,5,5",
        L"Padding=1,0,-10,0"}},
    ThemeTargetStyles{L"Rectangle#RunningIndicator", {
        L"Fill=Transparent"}},
    ThemeTargetStyles{L"Taskbar.TaskListLabeledButtonPanel > TextBlock#LabelControl", {
        L"Margin=8,0,0,0",
        L"Foreground=White"}},
    ThemeTargetStyles{L"Taskbar.SearchBoxButton", {
        L"Padding=8",
        L"Margin=4,0,4,0"}},
    ThemeTargetStyles{L"TextBlock#SearchBoxTextBlock", {
        L"FontSize=12"}},
    ThemeTargetStyles{L"Rectangle#BackgroundStroke", {
        L"Fill=Transparent"}},
    ThemeTargetStyles{L"SystemTray.AdaptiveTextBlock", {
        L"Foreground=White"}},
    ThemeTargetStyles{L"Taskbar.TaskListButton#TaskListButton[AutomationProperties.Name=Copilot] > Taskbar.TaskListLabeledButtonPanel#IconPanel > Border#BackgroundElement", {
        L"Background:=<AcrylicBrush TintColor=\"Black\" TintOpacity=\"0.8\" />"}},
    ThemeTargetStyles{L"SystemTray.NotifyIconView > Grid > Border#BackgroundBorder", {
        L"Margin=0,3,0,3"}},
    ThemeTargetStyles{L"Taskbar.AugmentedEntryPointButton#AugmentedEntryPointButton > Taskbar.TaskListButtonPanel#ExperienceToggleButtonRootPanel > Border#BackgroundElement@CommonStates", {
        L"Background:=<AcrylicBrush TintColor=\"{ThemeResource SystemAccentColorDark2}\" TintOpacity=\"0.4\" FallbackColor=\"{ThemeResource SystemAccentColorDark2}\" />",
        L"CornerRadius=20",
        L"Margin=0,1,0,1"}},
    ThemeTargetStyles{L"TextBlock#TimeInnerTextBlock", {
        L"Foreground=White"}},
    ThemeTargetStyles{L"TextBlock#DateInnerTextBlock", {
        L"Foreground=White"}},
    ThemeTargetStyles{L"SystemTray.TextIconContent > Grid > SystemTray.AdaptiveTextBlock#Base > TextBlock", {
        L"Foreground=White"}},
    ThemeTargetStyles{L"Border#BackgroundElement", {
        L"BorderThickness=0"}},
    ThemeTargetStyles{L"Taskbar.TaskListLabeledButtonPanel@RunningIndicatorStates > Border#BackgroundElement", {
        L"Background@InactiveRunningIndicator:=<SolidColorBrush Color=\"Black\" Opacity=\"0.4\" />",
        L"Background@InactiveRunningIndicator:=<SolidColorBrush Color=\"Black\" Opacity=\"0.4\" />",
        L"Background@ActiveRunningIndicator:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColorDark2}\" Opacity=\"0.4\" />",
        L"Background@RequestingAttentionRunningIndicator:=<SolidColorBrush Color=\"#ffdf5e\" Opacity=\"0.4\" />"}},
    ThemeTargetStyles{L"Rectangle#ShowDesktopPipe", {
        L"Width=12",
        L"Height=38",
        L"Margin=-6,0,0,0"}},
    ThemeTargetStyles{L"SystemTray.Stack#ShowDesktopStack", {
        L"Width=12"}},
    ThemeTargetStyles{L"Taskbar.TaskListButtonPanel", {
        L"Margin=-3,0,0,0"}},
    ThemeTargetStyles{L"Grid#OverflowRootGrid > Border", {
        L"Background:=<AcrylicBrush TintColor=\"{ThemeResource SystemAccentColorDark2}\" TintOpacity=\"0.4\" FallbackColor=\"{ThemeResource SystemAccentColorDark1}\" />",
        L"BorderBrush:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColor}\" />"}},
    ThemeTargetStyles{L"Taskbar.TaskItemThumbnailView > Grid > Border", {
        L"Background:=<AcrylicBrush TintColor=\"{ThemeResource SystemAccentColorDark3}\" TintOpacity=\"0.4\" FallbackColor=\"{ThemeResource SystemAccentColorDark1}\" />",
        L"BorderBrush:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColor}\" />",
        L"BorderThickness=1",
        L"CornerRadius=5"}},
    ThemeTargetStyles{L"Border#ProgressBarRoot > Border > Grid > Rectangle", {
        L"Fill:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColorLight1}\" />"}},
    ThemeTargetStyles{L"Border#ProgressBarRoot > Border > Grid > Rectangle#ProgressBarTrack", {
        L"Fill:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColorDark3}\" />"}},
    ThemeTargetStyles{L"Taskbar.TaskListLabeledButtonPanel@CommonStates > Border#BackgroundElement", {
        L"BorderBrush@InactivePointerOver:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColorLight1}\"  />",
        L"BorderBrush@ActiveNormal:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColorLight2}\" />",
        L"BorderBrush@ActivePointerOver:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColorLight1}\"  />",
        L"BorderBrush@MultiWindowPointerOver:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColorLight1}\"  />",
        L"BorderThickness@ActiveNormal=1",
        L"BorderThickness@ActivePointerOver=1",
        L"BorderThickness@MultiWindowActive=2",
        L"BorderThickness@MultiWindowNormal=2",
        L"BorderThickness@MultiWindowPointerOver=2.5",
        L"BorderBrush@MultiWindowActive:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColorLight1}\"  />",
        L"BorderBrush@MultiWindowNormal:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColorLight1}\"  />",
        L"Margin@MultiWindowNormal=0",
        L"Margin@MultiWindowPointerOver=0",
        L"Margin@MultiWindowPressed=0"}},
    ThemeTargetStyles{L"ToolTip", {
        L"Background:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColorDark2}\"  />",
        L"Foreground=White"}},
    ThemeTargetStyles{L"Taskbar.TaskListLabeledButtonPanel@CommonStates > Border#MultiWindowElement", {
        L"Background=Transparent",
        L"BorderThickness=0"}},
    ThemeTargetStyles{L"Taskbar.SearchBoxButton > Taskbar.TaskListButtonPanel@CommonStates > Border#BackgroundElement", {
        L"CornerRadius=5",
        L"CornerRadius@InactiveNormal_SearchIcon=100",
        L"CornerRadius@InactivePointerOver_SearchIcon=100",
        L"CornerRadius@InactivePressed_SearchIcon=100",
        L"CornerRadius@ActiveNormal_SearchIcon=100",
        L"CornerRadius@ActivePointerOver_SearchIcon=100",
        L"CornerRadius@ActivePressed_SearchIcon=100",
        L"Background@InactiveNormal_SearchIcon:=<AcrylicBrush TintColor=\"{ThemeResource SystemAccentColorDark2}\" TintOpacity=\"0.4\" FallbackColor=\"{ThemeResource SystemAccentColorDark2}\" />",
        L"Background@InactivePointerOver_SearchIcon:=<AcrylicBrush TintColor=\"{ThemeResource SystemAccentColorDark1}\" TintOpacity=\"0.4\" FallbackColor=\"{ThemeResource SystemAccentColorDark1}\" />",
        L"Background@InactivePressed_SearchIcon:=<AcrylicBrush TintColor=\"{ThemeResource SystemAccentColorDark1}\" TintOpacity=\"0.4\" FallbackColor=\"{ThemeResource SystemAccentColorDark1}\" />",
        L"Background@ActiveNormal_SearchIcon:=<AcrylicBrush TintColor=\"{ThemeResource SystemAccentColorDark2}\" TintOpacity=\"0.4\" FallbackColor=\"{ThemeResource SystemAccentColorDark2}\" />",
        L"Background@ActivePointerOver_SearchIcon:=<AcrylicBrush TintColor=\"{ThemeResource SystemAccentColorDark1}\" TintOpacity=\"0.4\" FallbackColor=\"{ThemeResource SystemAccentColorDark1}\" />",
        L"Background@ActivePressed_SearchIcon:=<AcrylicBrush TintColor=\"{ThemeResource SystemAccentColorDark1}\" TintOpacity=\"0.4\" FallbackColor=\"{ThemeResource SystemAccentColorDark1}\" />"}},
    ThemeTargetStyles{L"Taskbar.OverflowToggleButton", {
        L"Margin=8,0,8,0",
        L"Background:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColorDark2}\"  />"}},
    ThemeTargetStyles{L"Rectangle#RightOverflowButtonDivider", {
        L"Fill:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColorLight2}\"  />",
        L"Margin=8,4,-8,4"}},
    ThemeTargetStyles{L"Taskbar.OverflowToggleButton > Taskbar.TaskListButtonPanel@CommonStates > Border", {
        L"Background:=<AcrylicBrush TintColor=\"{ThemeResource SystemAccentColorDark2}\" TintOpacity=\"0.4\" FallbackColor=\"{ThemeResource SystemAccentColorDark2}\" />",
        L"Background@InactivePointerOver:=<AcrylicBrush TintColor=\"{ThemeResource SystemAccentColorDark1}\" TintOpacity=\"0.4\" FallbackColor=\"{ThemeResource SystemAccentColorDark1}\" />"}},
    ThemeTargetStyles{L"Taskbar.TaskListLabeledButtonPanel", {
        L"Margin=0"}},
    ThemeTargetStyles{L"SearchUx.SearchUI.SearchIconButton > SearchUx.SearchUI.SearchButtonRootGrid@CommonStates > Border#BackgroundElement", {
        L"CornerRadius=100",
        L"Background:=<AcrylicBrush TintColor=\"{ThemeResource SystemAccentColorDark2}\" TintOpacity=\"0.4\" FallbackColor=\"{ThemeResource SystemAccentColorDark2}\" />"}},
    ThemeTargetStyles{L"SearchUx.SearchUI.SearchButtonRootGrid", {
        L"Margin=0,0,3,0"}},
    ThemeTargetStyles{L"SearchUx.SearchUI.SearchButtonRootGrid > Border#BackgroundElement", {
        L"Background:=<AcrylicBrush TintColor=\"{ThemeResource SystemAccentColorDark2}\" TintOpacity=\"0.4\" FallbackColor=\"{ThemeResource SystemAccentColorDark2}\" />"}},
    ThemeTargetStyles{L"Border#SearchPillBackgroundElement", {
        L"BorderBrush:=<AcrylicBrush TintColor=\"{ThemeResource SystemAccentColorDark2}\" TintOpacity=\"0.4\" FallbackColor=\"{ThemeResource SystemAccentColorDark2}\" />",
        L"BorderThickness=16",
        L"CornerRadius=8"}},
}};

const Theme g_themeLucent = {{
    ThemeTargetStyles{L"Taskbar.TaskbarFrame > Grid#RootGrid > Taskbar.TaskbarBackground > Grid > Rectangle#BackgroundFill", {
        L"Fill:=<LinearGradientBrush StartPoint=\"0,0\" EndPoint=\"0,1\"><GradientStop Color=\"#00000000\" Offset=\"0.3\" /><GradientStop Color=\"#AA000000\" Offset=\"0.9\" /></LinearGradientBrush>"}},
    ThemeTargetStyles{L"Taskbar.TaskbarBackground#HoverFlyoutBackgroundControl > Grid > Rectangle#BackgroundFill", {
        L"Fill:=<LinearGradientBrush StartPoint=\"0,0.5\" EndPoint=\"0,1\"><GradientStop Color=\"#ee000000\" Offset=\"0.1\" /><GradientStop Color=\"{ThemeResource SystemAccentColorDark2}\" Offset=\"0.9\" /><GradientStop Color=\"#AAFFFFFF\" Offset=\"1.0\" /></LinearGradientBrush>"}},
    ThemeTargetStyles{L"Taskbar.TaskListLabeledButtonPanel > Rectangle#RunningIndicator", {
        L"Fill=Transparent"}},
    ThemeTargetStyles{L"Rectangle#BackgroundStroke", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Taskbar.TaskListLabeledButtonPanel@RunningIndicatorStates > Border#BackgroundElement", {
        L"Background@InactiveRunningIndicator:=<LinearGradientBrush StartPoint=\"0,0.5\" EndPoint=\"0,1\"><GradientStop Color=\"#3300290c\" Offset=\"0.1\" /><GradientStop Color=\"{ThemeResource SystemAccentColorDark2}\" Offset=\"0.9\" /><GradientStop Color=\"#AAFFFFFF\" Offset=\"1.0\" /></LinearGradientBrush>",
        L"Margin=0,-1,0,-1"}},
    ThemeTargetStyles{L"Taskbar.TaskListLabeledButtonPanel@CommonStates > TextBlock#LabelControl", {
        L"Foreground@ActiveNormal=Black",
        L"Foreground@ActivePointerOver=Black",
        L"Margin=0,0,3,0",
        L"Foreground@ActivePressed=#BFBFBF"}},
    ThemeTargetStyles{L"SystemTray.SystemTrayFrame > Grid", {
        L"Background:=<LinearGradientBrush StartPoint=\"0,0\" EndPoint=\"0,1\"><GradientStop Color=\"#50000000\" Offset=\"0.3\" /><GradientStop Color=\"#EE000000\" Offset=\"0.9\" /></LinearGradientBrush>",
        L"Margin=0",
        L"CornerRadius=0"}},
    ThemeTargetStyles{L"SystemTray.NotifyIconView#NotifyItemIcon", {
        L"Padding=2"}},
    ThemeTargetStyles{L"Taskbar.ExperienceToggleButton#LaunchListButton[AutomationProperties.AutomationId=StartButton] > Taskbar.TaskListButtonPanel", {
        L"CornerRadius=0",
        L"BorderThickness=0",
        L"MaxWidth=48",
        L"Margin=0",
        L"Padding=0",
        L"Background:=<LinearGradientBrush StartPoint=\"0,0\" EndPoint=\"0,1\"><GradientStop Color=\"#80000000\" Offset=\"0.0\" /><GradientStop Color=\"#FF000000\" Offset=\"1.0\" /></LinearGradientBrush>"}},
    ThemeTargetStyles{L"Grid", {
        L"RequestedTheme=2"}},
    ThemeTargetStyles{L"Grid#OverflowRootGrid > Border", {
        L"Background:=<LinearGradientBrush StartPoint=\"0,0\" EndPoint=\"0,1\"><GradientStop Color=\"#50000000\" Offset=\"0.3\" /><GradientStop Color=\"#EE000000\" Offset=\"0.9\" /></LinearGradientBrush>",
        L"CornerRadius=0"}},
    ThemeTargetStyles{L"Taskbar.AugmentedEntryPointButton", {
        L"Margin=10,0,-10,0"}},
    ThemeTargetStyles{L"Taskbar.TaskListLabeledButtonPanel@CommonStates", {
        L"Background@ActiveNormal:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColorLight3}\"/>",
        L"Background@ActivePointerOver:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColorLight2}\"/>",
        L"Background@InactivePointerOver=#EBEBEB",
        L"Background@InactivePressed=#BBBBBB",
        L"Background@ActivePressed:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColorLight1}\"/>",
        L"Background@RequestingAttention=#FFE9AFAA",
        L"Background@RequestingAttentionPointerOver=#FFF8E7E5",
        L"Background@RequestingAttentionPressed=#FFFEEEF0",
        L"Background@MultiWindowPointerOver:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColorLight2}\"/>",
        L"Background@MultiWindowActive:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColorLight3}\"/>",
        L"Background@MultiWindowPressed:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColorLight1}\"/>"}},
    ThemeTargetStyles{L"Taskbar.TaskListLabeledButtonPanel@CommonStates > Border", {
        L"BorderThickness=0",
        L"Margin=-2,-4,-2,-4",
        L"CornerRadius=0"}},
    ThemeTargetStyles{L"Taskbar.TaskListButtonPanel@CommonStates > Border", {
        L"CornerRadius=0",
        L"Background@InactivePointerOver:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColorLight3}\"/>",
        L"Background@InactivePressed:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColorLight2}\"/>",
        L"Background@ActiveNormal:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColorLight3}\"/>",
        L"Background@ActivePointerOver:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColorLight3}\"/>",
        L"Background@ActivePressed:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColorLight1}\"/>",
        L"Margin=-3,-10,-3,-10",
        L"BorderThickness=0"}},
    ThemeTargetStyles{L"Grid#ContainerGrid@", {
        L"Background@PointerOver:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColorLight3}\"/>",
        L"CornerRadius=0"}},
    ThemeTargetStyles{L"SystemTray.OmniButton#ControlCenterButton > Grid@CommonStates > Border#BackgroundBorder", {
        L"Margin=0",
        L"Padding=0",
        L"CornerRadius=0",
        L"Background=Transparent",
        L"BorderThickness=0"}},
    ThemeTargetStyles{L"SystemTray.OmniButton > Grid@CommonStates", {
        L"Background@PointerOver:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColorLight3}\"/>",
        L"Background@Pressed:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColorLight1}\"/>",
        L"Background@Checked:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColorLight2}\"/>",
        L"Background@CheckedPointerOver:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColorLight3}\"/>",
        L"Background@CheckedPressed:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColorLight1}\"/>"}},
    ThemeTargetStyles{L"Rectangle#ShowDesktopPipe", {
        L"Opacity=0"}},
    ThemeTargetStyles{L"SystemTray.ChevronIconView > Grid@", {
        L"Background@Pressed:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColorLight1}\"/>",
        L"Background@CheckedPressed:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColorLight1}\"/>",
        L"Background@PointerOver:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColorLight3}\"/>",
        L"Background@CheckedNormal:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColorLight2}\"/>",
        L"Background@CheckedPointerOver:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColorLight3}\"/>"}},
    ThemeTargetStyles{L"SystemTray.Stack#NonActivatableStack > Grid > SystemTray.StackListView > Windows.UI.Xaml.Controls.ItemsPresenter > Windows.UI.Xaml.Controls.StackPanel > Windows.UI.Xaml.Controls.ContentPresenter > SystemTray.IconView > Grid@", {
        L"Background@Pressed:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColorLight1}\"/>",
        L"Background@CheckedPressed:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColorLight1}\"/>",
        L"Background@PointerOver:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColorLight2}\"/>",
        L"Background@CheckedNormal:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColorLight2}\"/>",
        L"Background@CheckedPointerOver:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColorLight3}\"/>"}},
    ThemeTargetStyles{L"Grid#OverflowRootGrid > Windows.UI.Xaml.Controls.ItemsControl > Windows.UI.Xaml.Controls.ItemsPresenter > Windows.UI.Xaml.Controls.WrapGrid > Windows.UI.Xaml.Controls.ContentPresenter > SystemTray.NotifyIconView > Grid@", {
        L"CornerRadius=0",
        L"Margin=2,-5,2,-5"}},
    ThemeTargetStyles{L"Taskbar.ExperienceToggleButton#LaunchListButton[AutomationProperties.AutomationId=StartButton] > Taskbar.TaskListButtonPanel > Border", {
        L"Width=48"}},
    ThemeTargetStyles{L"Taskbar.TaskListLabeledButtonPanel > Border#MultiWindowElement", {
        L"Height=0"}},
    ThemeTargetStyles{L"Taskbar.TaskListLabeledButtonPanel@CommonStates > Rectangle", {
        L"StrokeThickness=3",
        L"Stroke@MultiWindowNormal:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColorLight3}\"/>",
        L"Stroke@MultiWindowPointerOver:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColorLight2}\"/>",
        L"Stroke@MultiWindowActive:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColorLight1}\"/>",
        L"Stroke@MultiWindowPressed:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColorLight1}\"/>",
        L"Width=Auto",
        L"RadiusX=0",
        L"Margin=-2,0,-2,-3"}},
    ThemeTargetStyles{L"SystemTray.ChevronIconView > Grid > Border", {
        L"BorderThickness=0",
        L"Background=Transparent"}},
    ThemeTargetStyles{L"SystemTray.OmniButton > Grid > Border", {
        L"BorderThickness=0",
        L"Background=Transparent"}},
    ThemeTargetStyles{L"Taskbar.SearchBoxButton > Taskbar.TaskListButtonPanel@CommonStates", {
        L"Background@InactivePointerOver_SearchIcon=#BEBEBE",
        L"Background@InactivePressed_SearchIcon=#DDDDDD",
        L"Background@ActiveNormal_SearchIcon=#BEBEBE",
        L"Background@ActivePointerOver_SearchIcon=#DDDDDD",
        L"Background@ActivePressed_SearchIcon=#EEEEEE"}},
}};

const Theme g_themeLucent_variant_Light = {{
    ThemeTargetStyles{L"Taskbar.TaskbarFrame > Grid#RootGrid > Taskbar.TaskbarBackground > Grid > Rectangle#BackgroundFill", {
        L"Fill:=<LinearGradientBrush StartPoint=\"0,0\" EndPoint=\"0,1\"><GradientStop Color=\"#00000000\" Offset=\"0.3\" /><GradientStop Color=\"#AA000000\" Offset=\"0.9\" /></LinearGradientBrush>"}},
    ThemeTargetStyles{L"Taskbar.TaskbarBackground#HoverFlyoutBackgroundControl > Grid > Rectangle#BackgroundFill", {
        L"Fill:=<LinearGradientBrush StartPoint=\"0,0.5\" EndPoint=\"0,1\"><GradientStop Color=\"#ee000000\" Offset=\"0.1\" /><GradientStop Color=\"#EBEBEB\" Offset=\"0.9\" /><GradientStop Color=\"#AAFFFFFF\" Offset=\"1.0\" /></LinearGradientBrush>"}},
    ThemeTargetStyles{L"Taskbar.TaskListLabeledButtonPanel > Rectangle#RunningIndicator", {
        L"Fill=Transparent"}},
    ThemeTargetStyles{L"Rectangle#BackgroundStroke", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Taskbar.TaskListLabeledButtonPanel@RunningIndicatorStates > Border#BackgroundElement", {
        L"Background@InactiveRunningIndicator:=<LinearGradientBrush StartPoint=\"0,0.5\" EndPoint=\"0,1\"><GradientStop Color=\"#33000000\" Offset=\"0.1\" /><GradientStop Color=\"#33EBEBEB\" Offset=\"0.9\" /><GradientStop Color=\"#AAFFFFFF\" Offset=\"1.0\" /></LinearGradientBrush>",
        L"Margin=0,-1,0,-1"}},
    ThemeTargetStyles{L"Taskbar.TaskListLabeledButtonPanel@CommonStates > TextBlock#LabelControl", {
        L"Foreground@ActiveNormal=Black",
        L"Foreground@ActivePointerOver=Black",
        L"Margin=0,0,3,0",
        L"Foreground@ActivePressed=#424242"}},
    ThemeTargetStyles{L"SystemTray.SystemTrayFrame > Grid", {
        L"Background:=<LinearGradientBrush StartPoint=\"0,0\" EndPoint=\"0,1\"><GradientStop Color=\"#50000000\" Offset=\"0.3\" /><GradientStop Color=\"#EE000000\" Offset=\"0.9\" /></LinearGradientBrush>",
        L"Margin=0",
        L"CornerRadius=0"}},
    ThemeTargetStyles{L"SystemTray.NotifyIconView#NotifyItemIcon", {
        L"Padding=2"}},
    ThemeTargetStyles{L"Taskbar.ExperienceToggleButton#LaunchListButton[AutomationProperties.AutomationId=StartButton] > Taskbar.TaskListButtonPanel", {
        L"CornerRadius=0",
        L"BorderThickness=0",
        L"MaxWidth=48",
        L"Margin=0",
        L"Padding=0",
        L"Background:=<LinearGradientBrush StartPoint=\"0,0\" EndPoint=\"0,1\"><GradientStop Color=\"#80000000\" Offset=\"0.0\" /><GradientStop Color=\"#FF000000\" Offset=\"1.0\" /></LinearGradientBrush>"}},
    ThemeTargetStyles{L"Grid", {
        L"RequestedTheme=2"}},
    ThemeTargetStyles{L"Grid#OverflowRootGrid > Border", {
        L"Background:=<LinearGradientBrush StartPoint=\"0,0\" EndPoint=\"0,1\"><GradientStop Color=\"#50000000\" Offset=\"0.3\" /><GradientStop Color=\"#EE000000\" Offset=\"0.9\" /></LinearGradientBrush>",
        L"CornerRadius=0"}},
    ThemeTargetStyles{L"Taskbar.AugmentedEntryPointButton", {
        L"Margin=10,0,-10,0"}},
    ThemeTargetStyles{L"Taskbar.TaskListLabeledButtonPanel@CommonStates", {
        L"Background@ActiveNormal=#FCFCFC",
        L"Background@ActivePointerOver=#BBBBBB",
        L"Background@InactivePointerOver=#BBBBBB",
        L"Background@InactivePressed=#EBEBEB",
        L"Background@ActivePressed=#EBEBEB",
        L"Background@RequestingAttention=#FFE9AFAA",
        L"Background@RequestingAttentionPointerOver=#FFF8E7E5",
        L"Background@RequestingAttentionPressed=#FFFEEEF0",
        L"Background@MultiWindowPointerOver=#BBBBBB",
        L"Background@MultiWindowActive=#BBBBBB",
        L"Background@MultiWindowPressed=#EBEBEB"}},
    ThemeTargetStyles{L"Taskbar.TaskListLabeledButtonPanel@CommonStates > Border", {
        L"BorderThickness=0",
        L"Margin=-2,-4,-2,-4",
        L"CornerRadius=0"}},
    ThemeTargetStyles{L"Taskbar.TaskListButtonPanel@CommonStates > Border", {
        L"CornerRadius=0",
        L"Background@InactivePointerOver=#BBBBBB",
        L"Background@InactivePressed=#EBEBEB",
        L"Background@ActiveNormal=#BBBBBB",
        L"Background@ActivePointerOver=#BBBBBB",
        L"Background@ActivePressed=#EBEBEB",
        L"Margin=-3,-10,-3,-10",
        L"BorderThickness=0"}},
    ThemeTargetStyles{L"Grid#ContainerGrid@", {
        L"Background@PointerOver=#BBBBBB",
        L"CornerRadius=0"}},
    ThemeTargetStyles{L"SystemTray.OmniButton#ControlCenterButton > Grid@CommonStates > Border#BackgroundBorder", {
        L"Background@PointerOver=#BBBBBB",
        L"Margin=0",
        L"Padding=0",
        L"CornerRadius=0"}},
    ThemeTargetStyles{L"SystemTray.OmniButton > Grid@CommonStates", {
        L"Background@PointerOver=#BBBBBB",
        L"Background@Pressed=#EBEBEB",
        L"Background@Checked=#BBBBBB",
        L"Background@CheckedPointerOver=#BBBBBB",
        L"Background@CheckedPressed=#EBEBEB"}},
    ThemeTargetStyles{L"Rectangle#ShowDesktopPipe", {
        L"Opacity=0"}},
    ThemeTargetStyles{L"SystemTray.ChevronIconView > Grid@", {
        L"Background@Pressed=#EBEBEB",
        L"Background@CheckedPressed=#EBEBEB",
        L"Background@PointerOver=#BBBBBB",
        L"Background@CheckedNormal=#BBBBBB",
        L"Background@CheckedPointerOver=#EBEBEB"}},
    ThemeTargetStyles{L"SystemTray.Stack#NonActivatableStack > Grid > SystemTray.StackListView > Windows.UI.Xaml.Controls.ItemsPresenter > Windows.UI.Xaml.Controls.StackPanel > Windows.UI.Xaml.Controls.ContentPresenter > SystemTray.IconView > Grid@", {
        L"Background@Pressed=#EBEBEB",
        L"Background@CheckedPressed=#EBEBEB",
        L"Background@PointerOver=#BBBBBB",
        L"Background@CheckedNormal=#BBBBBB",
        L" Background@CheckedPointerOver=#EBEBEB"}},
    ThemeTargetStyles{L"Grid#OverflowRootGrid > Windows.UI.Xaml.Controls.ItemsControl > Windows.UI.Xaml.Controls.ItemsPresenter > Windows.UI.Xaml.Controls.WrapGrid > Windows.UI.Xaml.Controls.ContentPresenter > SystemTray.NotifyIconView > Grid@", {
        L"CornerRadius=0",
        L"Margin=2,-5,2,-5"}},
    ThemeTargetStyles{L"Taskbar.ExperienceToggleButton#LaunchListButton[AutomationProperties.AutomationId=StartButton] > Taskbar.TaskListButtonPanel > Border", {
        L"Width=48"}},
    ThemeTargetStyles{L"Taskbar.TaskListLabeledButtonPanel > Border#MultiWindowElement", {
        L"Height=0"}},
    ThemeTargetStyles{L"Taskbar.TaskListLabeledButtonPanel@CommonStates > Rectangle", {
        L"StrokeThickness=3",
        L"Stroke@MultiWindowNormal=#BBBBBB",
        L"Stroke@MultiWindowPointerOver=#EBEBEB",
        L"Stroke@MultiWindowPressed=#EBEBEB",
        L"Stroke@MultiWindowActive=#EBEBEB",
        L"Width=Auto",
        L"RadiusX=0",
        L"Margin=-2,0,-2,-3"}},
    ThemeTargetStyles{L"SystemTray.ChevronIconView > Grid > Border", {
        L"BorderThickness=0",
        L"Background=Transparent"}},
    ThemeTargetStyles{L"SystemTray.OmniButton > Grid > Border", {
        L"BorderThickness=0",
        L"Background=Transparent"}},
    ThemeTargetStyles{L"Taskbar.SearchBoxButton > Taskbar.TaskListButtonPanel@CommonStates", {
        L"Background@InactivePointerOver_SearchIcon=#BEBEBE",
        L"Background@ActiveNormal_SearchIcon=#BEBEBE",
        L"Background@ActivePointerOver_SearchIcon=#EBEBEB"}},
}};

const Theme g_themeSunValley = {{
    ThemeTargetStyles{L"Taskbar.SearchBoxButton#SearchBoxButton > Taskbar.TaskListButtonPanel#ExperienceToggleButtonRootPanel > Windows.UI.Xaml.Controls.Border#BackgroundElement", {
        L"CornerRadius=4",
        L"BorderThickness=0,1,0,0"}},
    ThemeTargetStyles{L"Taskbar.SearchBoxButton", {
        L"Margin=0"}},
    ThemeTargetStyles{L"SystemTray.OmniButton#NotificationCenterButton > Grid > ContentPresenter > ItemsPresenter > StackPanel > ContentPresenter > SystemTray.IconView#SystemTrayIcon > Grid > Grid > SystemTray.TextIconContent > Windows.UI.Xaml.Controls.Grid > SystemTray.AdaptiveTextBlock#Base > Windows.UI.Xaml.Controls.TextBlock", {
        L"Text=\u200E \u200E\u200E\u200E\uE7E7",
        L"FontSize=17.3",
        L"Width=30",
        L"FontWeight=ExtraLight",
        L"Foreground:=<SolidColorBrush Color=\"{ThemeResource SystemBaseHighColor}\" />"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.FontIcon#SearchBoxFontIcon", {
        L"FontFamily=Segoe Fluent Icons"}},
    ThemeTargetStyles{L"SearchUx.SearchUI.SearchBoxButton#SearchBox > SearchUx.SearchUI.SearchButtonRootGrid#SearchBoxButtonRootPanel > Windows.UI.Xaml.Controls.TextBlock#SearchBoxTextBlock", {
        L"Text=Type here to search",
        L"FontSize=15",
        L"FontFamily=Segoe UI Variable Text",
        L"Margin=35,0,0,0"}},
    ThemeTargetStyles{L"SystemTray.NotifyIconView#NotifyItemIcon", {
        L"CornerRadius=3",
        L"Height=Auto",
        L"Margin=0,0,0,0",
        L"Padding=0,4,0,2",
        L"BorderThickness=0,1,0,0"}},
    ThemeTargetStyles{L"SystemTray.ChevronIconView", {
        L"CornerRadius=3",
        L"Height=Auto",
        L"Width=24",
        L"BorderThickness=0,1,0,0",
        L"Padding=0,2,0,2"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.Button#GleamEntryPointButton > Windows.UI.Xaml.Controls.Border", {
        L"CornerRadius=4"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#DynamicSearchBoxGleamContainer", {
        L"CornerRadius=3"}},
    ThemeTargetStyles{L"SystemTray.OmniButton#NotificationCenterButton", {
        L"CornerRadius=3",
        L"Padding=0,2,0,2",
        L"Margin=0,0,5,0",
        L"BorderThickness=0,1,0,0"}},
    ThemeTargetStyles{L"SystemTray.Stack#NonActivatableStack", {
        L"Height=Auto",
        L"CornerRadius=3",
        L"Margin=0,0,0,0",
        L"Padding=0,2,0,2",
        L"BorderThickness=0,1,0,0",
        L"Grid.Column=4"}},
    ThemeTargetStyles{L"Rectangle#ShowDesktopPipe@CommonStates", {
        L"Width=9",
        L"Margin=0,0,-10,0",
        L"Height=500",
        L"Fill@Active:=<AcrylicBrush TintColor=\"{ThemeResource SystemBaseLowColor}\" TintOpacity=\"0.5\" Opacity=\"0\"/>",
        L"Stroke:=<SolidColorBrush Color=\"{ThemeResource SystemBaseHighColor}\" Opacity=\"0.3\"/>"}},
    ThemeTargetStyles{L"SystemTray.OmniButton#ControlCenterButton", {
        L"Padding=0,2,0,2",
        L"CornerRadius=3",
        L"Margin=0,0,0,0",
        L"BorderThickness=0,1,0,0",
        L"Grid.Column=3"}},
    ThemeTargetStyles{L"SystemTray.AdaptiveTextBlock#LanguageInnerTextBlock > TextBlock#InnerTextBlock", {
        L"FontFamily=Segoe UI Variable Text",
        L"Margin=-8,0,0,0",
        L"FontSize=12"}},
    ThemeTargetStyles{L"SystemTray.SystemTrayFrame > Windows.UI.Xaml.Controls.Grid#SystemTrayFrameGrid > SystemTray.Stack#NotifyIconStack > Windows.UI.Xaml.Controls.Grid#Content > SystemTray.StackListView#IconStack > Windows.UI.Xaml.Controls.ItemsPresenter > Windows.UI.Xaml.Controls.StackPanel > Windows.UI.Xaml.Controls.ContentPresenter > SystemTray.ChevronIconView > Windows.UI.Xaml.Controls.Grid#ContainerGrid > Windows.UI.Xaml.Controls.ContentPresenter#ContentPresenter > Windows.UI.Xaml.Controls.Grid#ContentGrid > SystemTray.TextIconContent > Windows.UI.Xaml.Controls.Grid#ContainerGrid > SystemTray.AdaptiveTextBlock#Base > Windows.UI.Xaml.Controls.TextBlock#InnerTextBlock", {
        L"FontFamily=Segoe Fluent Icons",
        L"FontSize=12.4",
        L"Foreground:=<SolidColorBrush Color=\"{ThemeResource SystemBaseHighColor}\" />"}},
    ThemeTargetStyles{L"SystemTray.SystemTrayFrame > Windows.UI.Xaml.Controls.Grid#SystemTrayFrameGrid > SystemTray.Stack#NotifyIconStack > Windows.UI.Xaml.Controls.Grid#Content > SystemTray.StackListView#IconStack > Windows.UI.Xaml.Controls.ItemsPresenter > Windows.UI.Xaml.Controls.StackPanel > Windows.UI.Xaml.Controls.ContentPresenter", {
        L"Width=30"}},
    ThemeTargetStyles{L"SystemTray.AdaptiveTextBlock#Base > Windows.UI.Xaml.Controls.TextBlock#InnerTextBlock", {
        L"FontFamily=Segoe Fluent Icons",
        L"Foreground:=<SolidColorBrush Color=\"{ThemeResource SystemBaseHighColor}\" />"}},
    ThemeTargetStyles{L"SystemTray.AdaptiveTextBlock#AccentOverlay > Windows.UI.Xaml.Controls.TextBlock#InnerTextBlock", {
        L"FontFamily=Segoe Fluent Icons"}},
    ThemeTargetStyles{L"SystemTray.AdaptiveTextBlock#Underlay > Windows.UI.Xaml.Controls.TextBlock#InnerTextBlock", {
        L"FontFamily=Segoe Fluent Icons"}},
    ThemeTargetStyles{L"SystemTray.OmniButton#ControlCenterButton > Grid > ContentPresenter > ItemsPresenter > StackPanel > ContentPresenter[1] > SystemTray.IconView > Grid > Grid", {
        L"Margin=-3,0,0,0"}},
    ThemeTargetStyles{L"SystemTray.Stack#MainStack > Windows.UI.Xaml.Controls.Grid#Content", {
        L"CornerRadius=3",
        L"Height=Auto",
        L"Margin=0,0,0,0",
        L"Padding=0,4,0,4",
        L"BorderThickness=0,1,0,0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.StackPanel > Windows.UI.Xaml.Controls.TextBlock#TimeInnerTextBlock", {
        L"FontFamily=Segoe UI Variable Display",
        L"TextAlignment=0",
        L"FontSize=12",
        L"Margin=0,1,0,0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.StackPanel > Windows.UI.Xaml.Controls.TextBlock#DateInnerTextBlock", {
        L"FontFamily=Segoe UI Variable",
        L"TextAlignment=0",
        L"FontSize=12",
        L"Margin=0,0,0,0"}},
    ThemeTargetStyles{L"SystemTray.NotificationAreaIcons#NotificationAreaIcons > Windows.UI.Xaml.Controls.ItemsPresenter > Windows.UI.Xaml.Controls.StackPanel > Windows.UI.Xaml.Controls.ContentPresenter", {
        L"Width=23",
        L"Margin=0,-2,0,0"}},
    ThemeTargetStyles{L"SystemTray.NotificationAreaIcons#NotificationAreaIcons > Windows.UI.Xaml.Controls.ItemsPresenter > Windows.UI.Xaml.Controls.StackPanel > Windows.UI.Xaml.Controls.ContentPresenter > SystemTray.NotifyIconView#NotifyItemIcon > Windows.UI.Xaml.Controls.Grid#ContainerGrid", {
        L"Transform3D:=<CompositeTransform3D TranslateY=\"0\" TranslateX=\"0\" />",
        L"HorizontalAlignment=0"}},
    ThemeTargetStyles{L"SystemTray.Stack#NotifyIconStack", {
        L"Width=24"}},
    ThemeTargetStyles{L"SystemTray.OmniButton#ControlCenterButton > Grid > ContentPresenter > ItemsPresenter > StackPanel > ContentPresenter > SystemTray.IconView#SystemTrayIcon > Grid > Grid > SystemTray.TextIconContent > Windows.UI.Xaml.Controls.Grid > SystemTray.AdaptiveTextBlock > Windows.UI.Xaml.Controls.TextBlock", {
        L"FontSize=16",
        L"Margin=0,-1,-0,0",
        L"FontWeight=0"}},
    ThemeTargetStyles{L"SystemTray.CopilotIcon#CopilotIcon", {
        L"Visibility=Visible",
        L"Padding=2",
        L"Height=61"}},
    ThemeTargetStyles{L"SystemTray.NotificationAreaOverflow > Windows.UI.Xaml.Controls.Grid#OverflowRootGrid > Windows.UI.Xaml.Controls.Border#OverflowFlyoutBackgroundBorder", {
        L"CornerRadius=7",
        L"Background:=<AcrylicBrush TintColor=\"{ThemeResource SystemChromeMediumHighColor}\" TintOpacity=\"0\" TintLuminosityOpacity=\"0.8\" FallbackColor=\"{ThemeResource SystemChromeMediumColor}\" />",
        L"Margin=0,0,0,0",
        L"BorderBrush:=<SolidColorBrush Color=\"{ThemeResource SystemChromeHighColor}\" Opacity=\"0.5\" />",
        L"BorderThickness=1"}},
    ThemeTargetStyles{L"SystemTray.NotificationAreaOverflow > Windows.UI.Xaml.Controls.Grid#OverflowRootGrid > Windows.UI.Xaml.Controls.ItemsControl > Windows.UI.Xaml.Controls.ItemsPresenter > Windows.UI.Xaml.Controls.WrapGrid", {
        L"Margin=0,0,0,0"}},
    ThemeTargetStyles{L"SystemTray.NotifyIconView", {
        L"CornerRadius=3"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.ScrollViewer > Windows.UI.Xaml.Controls.ScrollContentPresenter > Windows.UI.Xaml.Controls.Border > SystemTray.NotificationAreaOverflow", {
        L"Transform3D:=<CompositeTransform3D TranslateY=\"0\" />"}},
    ThemeTargetStyles{L"SystemTray.OmniButton#ControlCenterButton", {
        L"Visibility=Visible"}},
    ThemeTargetStyles{L"Taskbar.TaskbarBackground#BackgroundControl > Grid", {
        L"Background:=<AcrylicBrush TintColor=\"{ThemeResource SystemChromeAltHighColor}\" TintOpacity=\"0.4\" TintLuminosityOpacity=\"0.4\" FallbackColor=\"{ThemeResource SystemChromeMediumColor}\" />"}},
    ThemeTargetStyles{L"Taskbar.TaskbarBackground#BackgroundControl > Grid > Windows.UI.Xaml.Shapes.Rectangle#BackgroundFill", {
        L"Opacity=0.5"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Shapes.Rectangle#BackgroundStroke", {
        L"Opacity=0.5"}},
    ThemeTargetStyles{L"SystemTray.OmniButton#ControlCenterButton > Grid > ContentPresenter > ItemsPresenter > StackPanel > ContentPresenter[3] > SystemTray.IconView > Grid > Grid", {
        L"Margin=2,0,-4,0",
        L"RenderTransform:=<ScaleTransform ScaleX=\"1\" />"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.ContentPresenter#HoverFlyoutContent", {
        L"CornerRadius=7",
        L"Margin=0,0,0,0",
        L"Background:=<AcrylicBrush TintColor=\"{ThemeResource SystemChromeMediumHighColor}\" TintOpacity=\"0\" TintLuminosityOpacity=\"0.8\" FallbackColor=\"{ThemeResource SystemChromeMediumColor}\" />",
        L"BorderBrush:=<SolidColorBrush Color=\"{ThemeResource SystemChromeHighColor}\" Opacity=\"0.5\" />",
        L"BorderThickness=1"}},
    ThemeTargetStyles{L"Taskbar.TaskItemThumbnailView > Grid > TextBlock", {
        L"FontFamily=Segoe UI",
        L"FontSize=12",
        L"Margin=3,0,8,8"}},
    ThemeTargetStyles{L"Taskbar.TaskItemThumbnailView > Windows.UI.Xaml.Controls.Grid > Microsoft.UI.Xaml.Controls.ItemsRepeater > Windows.UI.Xaml.Controls.Image", {
        L"Margin=0,-7,0,0"}},
    ThemeTargetStyles{L"Taskbar.TaskItemThumbnailView > Grid > Button > ContentPresenter > TextBlock", {
        L"FontFamily=Segoe Fluent Icons"}},
    ThemeTargetStyles{L"Taskbar.TaskItemThumbnailView > Grid > Button", {
        L"CornerRadius=4",
        L"Height=31",
        L"Margin=0,0,0,8",
        L"Width=31"}},
    ThemeTargetStyles{L"Grid#DetailedViewGrid", {
        L"Margin=0,-7,0,0"}},
    ThemeTargetStyles{L"Taskbar.TaskItemThumbnailView > Grid > Border", {
        L"BorderBrush:=<SolidColorBrush Color=\"{ThemeResource SystemBaseHighColor}\" Opacity=\"0.5\" />",
        L"CornerRadius=0"}},
    ThemeTargetStyles{L"SystemTray.OmniButton#NotificationCenterButton > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.ContentPresenter > Windows.UI.Xaml.Controls.ItemsPresenter > Windows.UI.Xaml.Controls.StackPanel > Windows.UI.Xaml.Controls.ContentPresenter > SystemTray.IconView > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.ContentPresenter > Windows.UI.Xaml.Controls.Grid > SystemTray.TextIconContent > Windows.UI.Xaml.Controls.Grid > SystemTray.AdaptiveTextBlock#Base > Windows.UI.Xaml.Controls.TextBlock", {
        L"Text=\u200E\uE7E7",
        L"FontWeight=Light",
        L"FontSize=17.3",
        L"Foreground:=<SolidColorBrush Color=\"{ThemeResource SystemBaseHighColor}\" />",
        L"Margin=-0.5,0,1,0"}},
    ThemeTargetStyles{L"SystemTray.OmniButton#NotificationCenterButton > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.ContentPresenter > Windows.UI.Xaml.Controls.ItemsPresenter > Windows.UI.Xaml.Controls.StackPanel > Windows.UI.Xaml.Controls.ContentPresenter > SystemTray.IconView", {
        L"CornerRadius=0",
        L"Padding=0,0,0,0"}},
    ThemeTargetStyles{L"SystemTray.DateTimeIconContent > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.StackPanel > Windows.UI.Xaml.Controls.TextBlock", {
        L"FontFamily=Segoe UI",
        L"TextAlignment=Center"}},
    ThemeTargetStyles{L"SystemTray.OmniButton#ControlCenterButton > Grid > ContentPresenter > ItemsPresenter > StackPanel > ContentPresenter[2] > SystemTray.IconView > Grid > Grid", {
        L"Margin=0,0,-3,0"}},
    ThemeTargetStyles{L"Taskbar.ThumbBarButton > ContentPresenter", {
        L"CornerRadius=4",
        L"Background:=<SolidColorBrush Color=\"{ThemeResource SystemChromeMediumHighColor}\" Opacity=\"0.3\" />",
        L"BorderBrush:=<SolidColorBrush Color=\"{ThemeResource SystemChromeHighColor}\" Opacity=\"0.3\" />",
        L"BorderThickness=1"}},
    ThemeTargetStyles{L"Taskbar.ThumbBarButton > ContentPresenter > Image", {
        L"Height=15",
        L"Width=15"}},
    ThemeTargetStyles{L"SearchUx.SearchUI.SearchButtonRootGrid > Border", {
        L"CornerRadius=4"}},
    ThemeTargetStyles{L"TextBlock#BatteryTextBlock", {
        L"FontFamily=Segoe UI Variable Text",
        L"Margin=2,0,-2,0"}},
    ThemeTargetStyles{L"SystemTray.BatteryIconContent > Grid > Windows.UI.Xaml.Controls.StackPanel > Grid > TextBlock", {
        L"RenderTransform:=<ScaleTransform ScaleX=\"1\" />"}},
    ThemeTargetStyles{L"SystemTray.IconView#SystemTrayIcon > Grid#ContainerGrid > Border#BackgroundBorder", {
        L"CornerRadius=0",
        L"Opacity=0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Button#GleamEntryPointButton > Windows.UI.Xaml.Controls.Border > Windows.UI.Xaml.Controls.ContentPresenter", {
        L"CornerRadius=3"}},
    ThemeTargetStyles{L"SearchUx.SearchUI.SearchButtonRootGrid", {
        L"Margin=2,0,2,0"}},
    ThemeTargetStyles{L"SystemTray.ChevronIconView > Grid > ContentPresenter", {
        L"Transform3D:=<CompositeTransform3D TranslateY=\"-1\" TranslateX=\"2\" />",
        L"HorizontalAlignment=0"}},
    ThemeTargetStyles{L"Border#SearchPillBackgroundElement", {
        L"CornerRadius=4"}},
    ThemeTargetStyles{L"Grid#ConfirmatorMainGrid", {
        L"Background:=<AcrylicBrush TintColor=\"{ThemeResource SystemChromeMediumHighColor}\" TintOpacity=\"0\" TintLuminosityOpacity=\"0.8\" FallbackColor=\"{ThemeResource SystemChromeMediumColor}\" />",
        L"BorderBrush:=<SolidColorBrush Color=\"{ThemeResource SystemChromeHighColor}\" Opacity=\"0.5\" />",
        L"CornerRadius=6",
        L"BorderThickness=1"}},
    ThemeTargetStyles{L"Slider > Grid > Grid > Grid > Windows.UI.Xaml.Controls.Primitives.Thumb#HorizontalThumb", {
        L"Visibility=Visible",
        L"Height=18",
        L"Width=18"}},
    ThemeTargetStyles{L"Slider > Grid > Grid > Grid > Windows.UI.Xaml.Controls.Primitives.Thumb#HorizontalThumb > Border", {
        L"BorderBrush:=<AcrylicBrush TintColor=\"{ThemeResource SystemChromeLowColor}\" TintOpacity=\"0.7\" TintLuminosityOpacity=\"0.7\" FallbackColor=\"{ThemeResource SystemChromeMediumColor}\" />",
        L"BorderThickness=4",
        L"Background:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColorLight1}\" />",
        L"CornerRadius=10"}},
    ThemeTargetStyles{L"Slider > Grid > Grid > Grid > Rectangle#HorizontalDecreaseRect", {
        L"Fill:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColorLight1}\" />"}},
    ThemeTargetStyles{L"Slider > Grid > Grid > Grid > Windows.UI.Xaml.Controls.Primitives.Thumb#HorizontalThumb", {
        L"BorderBrush:=<AcrylicBrush TintColor=\"{ThemeResource SystemChromeHighColor}\" TintOpacity=\"0.7\" TintLuminosityOpacity=\"0.7\" FallbackColor=\"{ThemeResource SystemChromeMediumColor}\" />",
        L"BorderThickness=0,1,0,0"}},
    ThemeTargetStyles{L"SearchUx.SearchUI.SearchBoxButton", {
        L"Width=340"}},
    ThemeTargetStyles{L"SearchUx.SearchUI.SearchBoxButton > SearchUx.SearchUI.SearchButtonRootGrid", {
        L"Width=340",
        L"Margin=-5,-6,-4,-6"}},
    ThemeTargetStyles{L"SearchUx.SearchUI.SearchBoxButton > SearchUx.SearchUI.SearchButtonRootGrid > Grid", {
        L"Margin=0,-1,-1,-1",
        L"Width=80",
        L"Transitions:=<TransitionCollection>              <ContentThemeTransition/>           </TransitionCollection>"}},
    ThemeTargetStyles{L"SearchUx.SearchUI.SearchBoxButton > SearchUx.SearchUI.SearchButtonRootGrid > Grid > Grid > Image", {
        L"Width=80",
        L"Height=40"}},
    ThemeTargetStyles{L"Taskbar.ExperienceToggleButton#LaunchListButton[AutomationProperties.AutomationId=StartButton] > Taskbar.TaskListButtonPanel", {
        L"Width=48",
        L"Background:=<ImageBrush Stretch=\"Uniform\" ImageSource=\"https://raw.githubusercontent.com/ramensoftware/windows-11-taskbar-styling-guide/refs/heads/main/Themes/SunValley/Assets/start.png\" />"}},
    ThemeTargetStyles{L"Taskbar.ExperienceToggleButton#LaunchListButton[AutomationProperties.AutomationId=StartButton] > Taskbar.TaskListButtonPanel > Microsoft.UI.Xaml.Controls.AnimatedVisualPlayer#Icon", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"ToolTip > ContentPresenter > TextBlock", {
        L"FontFamily=Segoe UI Variable Text",
        L"FontSize=13",
        L"Margin=0,1,0,-2"}},
    ThemeTargetStyles{L"ToolTip > ContentPresenter > StackPanel > TextBlock", {
        L"FontFamily=Segoe UI Variable Text",
        L"FontSize=13",
        L"Margin=0,1,0,-2"}},
    ThemeTargetStyles{L"ToolTip", {
        L"Background:=<AcrylicBrush TintColor=\"{ThemeResource SystemChromeMediumHighColor}\" TintOpacity=\"0\" TintLuminosityOpacity=\"0.8\" FallbackColor=\"{ThemeResource SystemChromeMediumColor}\" />",
        L"BorderBrush:=<SolidColorBrush Color=\"{ThemeResource SystemChromeHighColor}\" Opacity=\"0.5\" />",
        L"BorderThickness=1"}},
    ThemeTargetStyles{L"ToolTip > ContentPresenter", {
        L"Transitions:=<TransitionCollection>              <ContentThemeTransition VerticalOffset=\"60\" />           </TransitionCollection>"}},
    ThemeTargetStyles{L"Taskbar.AugmentedEntryPointButton#AugmentedEntryPointButton[AutomationProperties.AutomationId=WidgetsButton] > Taskbar.TaskListButtonPanel#ExperienceToggleButtonRootPanel", {
        L"Padding=0,2,0,2"}},
    ThemeTargetStyles{L"TextBlock#VirtualDesktopNameBlock", {
        L"FontFamily=Segoe UI Variable Text",
        L"FontSize=13"}},
    ThemeTargetStyles{L"Grid#RootGrid > Grid#TitleGrid > TextBlock#DisplayName", {
        L"FontFamily=Segoe UI Variable Text",
        L"FontSize=13"}},
    ThemeTargetStyles{L"Grid#MainGrid > TextBlock", {
        L"FontFamily=Segoe UI Variable Text",
        L"FontSize=13"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Border#BackgroundDimmingLayer", {
        L"Background:=<AcrylicBrush TintColor=\"{ThemeResource SystemChromeMediumHighColor}\" TintOpacity=\"0\" TintLuminosityOpacity=\"0.7\" FallbackColor=\"{ThemeResource SystemChromeMediumColor}\" />"}},
    ThemeTargetStyles{L"Taskbar.ExperienceToggleButton#LaunchListButton[AutomationProperties.AutomationId=TaskViewButton] > Taskbar.TaskListButtonPanel > Microsoft.UI.Xaml.Controls.AnimatedVisualPlayer#Icon", {
        L"RenderTransform:=<ScaleTransform ScaleX=\"1.1\" ScaleY=\"0.9\" />",
        L"Transform3D:=<CompositeTransform3D TranslateY=\"2\" TranslateX=\"-2\" />",
        L"FlowDirection=1"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#ModalRootGrid > Windows.UI.Xaml.Controls.Border#BackgroundElement", {
        L"Background=Transparent",
        L"Transitions:=<TransitionCollection>              <ContentThemeTransition VerticalOffset=\"-1000\" />           </TransitionCollection>"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#ModalRootGrid > Windows.UI.Xaml.Controls.Border#BackgroundElement > WindowsInternal.ComposableShell.Experiences.Switcher.SwitchItemList", {
        L"Background:=<AcrylicBrush TintColor=\"{ThemeResource SystemChromeMediumHighColor}\" TintOpacity=\"0.2\" TintLuminosityOpacity=\"0.9\" FallbackColor=\"{ThemeResource SystemChromeMediumColor}\" />"}},
    ThemeTargetStyles{L"SearchUx.SearchUI.SearchBoxButton > SearchUx.SearchUI.SearchButtonRootGrid > Microsoft.UI.Xaml.Controls.AnimatedVisualPlayer", {
        L"Height=18",
        L"Width=18",
        L"Transform3D:=<CompositeTransform3D TranslateX=\"0\" />"}},
    ThemeTargetStyles{L"SearchUx.SearchUI.SearchPillButton > SearchUx.SearchUI.SearchButtonRootGrid", {
        L"Margin=-3,-6,-3,-6",
        L"Width=346"}},
    ThemeTargetStyles{L"SearchUx.SearchUI.SearchPillButton > SearchUx.SearchUI.SearchButtonRootGrid > Grid#SearchBoxContentGrid", {
        L"HorizontalAlignment=0"}},
    ThemeTargetStyles{L"SearchUx.SearchUI.SearchPillButton > SearchUx.SearchUI.SearchButtonRootGrid > Grid#SearchBoxContentGrid > FontIcon", {
        L"Transform3D:=<CompositeTransform3D TranslateY=\"-1\" TranslateX=\"-10.5\" />",
        L"FontSize=19.4",
        L"FontFamily=Segoe Fluent Icons",
        L"FontWeight=SemiLight",
        L"Opacity=0.7",
        L"Grid.Column=0",
        L"HorizontalAlignment=0"}},
    ThemeTargetStyles{L"SearchUx.SearchUI.SearchPillButton > SearchUx.SearchUI.SearchButtonRootGrid > Grid#SearchBoxContentGrid > TextBlock", {
        L"Transform3D:=<CompositeTransform3D TranslateX=\"-5\" />",
        L"FontFamily=Segoe UI",
        L"Opacity=0.7",
        L"Text=Type here to search",
        L"Grid.Column=1",
        L"HorizontalAlignment=0",
        L"FontSize=15"}},
    ThemeTargetStyles{L"SearchUx.SearchUI.SearchPillButton > SearchUx.SearchUI.SearchButtonRootGrid > Border#SearchPillBackgroundElement", {
        L"CornerRadius=4",
        L"BorderThickness=1",
        L"BorderBrush:=<SolidColorBrush Color=\"{ThemeResource SystemChromeHighColor}\" Opacity=\"0.7\" />"}},
    ThemeTargetStyles{L"Grid#SearchBoxContentGrid", {
        L"Visibility=Visible"}},
    ThemeTargetStyles{L"Taskbar.TaskListButton#TaskListButton > Taskbar.TaskListLabeledButtonPanel#IconPanel", {
        L"Padding=2",
        L"Margin=2,0,2,0"}},
    ThemeTargetStyles{L"Taskbar.TaskListButton > Taskbar.TaskListLabeledButtonPanel > Rectangle", {
        L"RadiusX=2",
        L"Margin=0,0,0,0"}},
    ThemeTargetStyles{L"Taskbar.ExperienceToggleButton > Taskbar.TaskListButtonPanel", {
        L"Padding=2",
        L"Margin=1,0,2,0"}},
    ThemeTargetStyles{L"MenuFlyoutPresenter", {
        L"CornerRadius=7"}},
    ThemeTargetStyles{L"MenuFlyoutPresenter > Border", {
        L"Background:=<AcrylicBrush TintColor=\"{ThemeResource SystemChromeMediumHighColor}\" TintOpacity=\"0.3\" TintLuminosityOpacity=\"0.8\" FallbackColor=\"{ThemeResource SystemChromeMediumColor}\" />",
        L"Transitions:=<TransitionCollection>              <ContentThemeTransition VerticalOffset=\"100\" />           </TransitionCollection>",
        L"BorderThickness=1",
        L"BorderBrush:=<SolidColorBrush Color=\"{ThemeResource SystemChromeHighColor}\" Opacity=\"0.5\" />"}},
    ThemeTargetStyles{L"Taskbar.TaskListLabeledButtonPanel#IconPanel > Image#OverlayIcon", {
        L"Transform3D:=<CompositeTransform3D TranslateY=\"15\" TranslateX=\"-2\" />"}},
    ThemeTargetStyles{L"Taskbar.TaskListLabeledButtonPanel#IconPanel > Taskbar.Badge", {
        L"Transform3D:=<CompositeTransform3D TranslateY=\"15\" TranslateX=\"-2\" />"}},
    ThemeTargetStyles{L"Taskbar.TaskListLabeledButtonPanel#IconPanel > Taskbar.Badge > Grid > Rectangle", {
        L"Fill=Transparent"}},
    ThemeTargetStyles{L"Taskbar.TaskListLabeledButtonPanel#IconPanel > Taskbar.Badge > Grid", {
        L"Background:=<AcrylicBrush TintColor=\"{ThemeResource SystemChromeMediumHighColor}\" TintOpacity=\"0\" TintLuminosityOpacity=\"0,7\" FallbackColor=\"{ThemeResource SystemChromeMediumColor}\" />",
        L"BorderBrush:=<AcrylicBrush TintColor=\"{ThemeResource SystemChromeHighColor}\" TintOpacity=\"0\" TintLuminosityOpacity=\"0.5\" FallbackColor=\"{ThemeResource SystemChromeMediumColor}\" />",
        L"BorderThickness=0,1,0,0",
        L"CornerRadius=15"}},
    ThemeTargetStyles{L"Taskbar.TaskListLabeledButtonPanel#IconPanel > Taskbar.Badge > Grid > TextBlock", {
        L"Foreground:=<SolidColorBrush Color=\"{ThemeResource SystemBaseHighColor}\" />",
        L"Transform3D:=<CompositeTransform3D TranslateY=\"-1\" />"}},
    ThemeTargetStyles{L"Taskbar.TaskListLabeledButtonPanel#IconPanel > Taskbar.Badge", {
        L"Height=17",
        L"Width=17"}},
    ThemeTargetStyles{L"SearchUx.SearchUI.SearchIconButton > SearchUx.SearchUI.SearchButtonRootGrid", {
        L"Padding=0,2,2,2",
        L"Width=48"}},
    ThemeTargetStyles{L"SearchUx.SearchUI.SearchBoxButton > SearchUx.SearchUI.SearchButtonRootGrid > TextBlock", {
        L"Transform3D:=<CompositeTransform3D TranslateX=\"5\" TranslateY=\"1\" />"}},
    ThemeTargetStyles{L"SearchUx.SearchUI.SearchButtonRootGrid#SearchV2ButtonRootPanel", {
        L"Padding=0,2,0,2"}},
    ThemeTargetStyles{L"SearchUx.SearchUI.SearchV2Button", {
        L"CornerRadius=4",
        L"Width=346"}},
    ThemeTargetStyles{L"Grid#SearchV2ButtonInactiveUIGrid", {
        L"MaxWidth=346",
        L"MinWidth=346"}},
    ThemeTargetStyles{L"Grid#SearchV2ButtonActiveUIGridWithAnimations", {
        L"Width=346"}},
    ThemeTargetStyles{L"Grid#SearchV2ButtonActiveUIGridWithAnimations > StackPanel", {
        L"HorizontalAlignment=0",
        L"Margin=13,0,0,0"}},
    ThemeTargetStyles{L"Grid#SearchV2ButtonInactiveUIGrid > Button", {
        L"Height=40",
        L"Width=30",
        L"CornerRadius=4"}},
    ThemeTargetStyles{L"TextBlock#SearchV2OnTaskbarButtonText", {
        L"FontFamily=Segoe UI",
        L"FontSize=15",
        L"Transform3D:=<CompositeTransform3D TranslateY=\"-1\" />",
        L"Text=Ask me anything"}},
    ThemeTargetStyles{L"Taskbar.ExperienceToggleButton#LaunchListButton[AutomationProperties.AutomationId=TaskViewButton] > Taskbar.TaskListButtonPanel > Microsoft.UI.Xaml.Controls.AnimatedVisualPlayer#Icon", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Taskbar.ExperienceToggleButton#LaunchListButton[AutomationProperties.AutomationId=TaskViewButton] > Taskbar.TaskListButtonPanel@CommonStates", {
        L"Background:=<ImageBrush Stretch=\"Uniform\" ImageSource=\"https://raw.githubusercontent.com/ramensoftware/windows-11-taskbar-styling-guide/refs/heads/main/Themes/SunValley/Assets/taskview.png\" />",
        L"Width=48"}},
    ThemeTargetStyles{L"SearchUx.SearchUI.SearchIconButton > SearchUx.SearchUI.SearchButtonRootGrid > Grid#SearchBoxContentGrid > FontIcon > Grid > TextBlock", {
        L"FontWeight=Light"}},
    ThemeTargetStyles{L"SystemTray.NotificationAreaOverflow > Windows.UI.Xaml.Controls.Grid#OverflowRootGrid", {
        L"Margin=7,0,0,0"}},
    ThemeTargetStyles{L"Taskbar.TaskListButton#TaskListButton > Taskbar.TaskListLabeledButtonPanel#IconPanel > Border#BackgroundElement", {
        L"Margin=-3,0,-3,0"}},
    ThemeTargetStyles{L"Taskbar.TaskListButton#TaskListButton > Taskbar.TaskListLabeledButtonPanel#IconPanel@RunningIndicatorStates > Rectangle", {
        L"Width@ActiveRunningIndicator=40",
        L"RadiusX=2",
        L"RadiusY=2",
        L"Width@InactiveRunningIndicator=35"}},
    ThemeTargetStyles{L"Taskbar.ExperienceToggleButton > Taskbar.TaskListButtonPanel > Border", {
        L"Margin=-3,0,-3,0"}},
    ThemeTargetStyles{L"SearchUx.SearchUI.SearchIconButton > SearchUx.SearchUI.SearchButtonRootGrid > Border", {
        L"Margin=-1,0,-1,0",
        L"Opacity=0.5"}},
    ThemeTargetStyles{L"Taskbar.TaskListButton#TaskListButton > Taskbar.TaskListLabeledButtonPanel#IconPanel@CommonStates > Border#MultiWindowElement", {
        L"Margin=-4,0,-4,0",
        L"Opacity=0.5"}},
    ThemeTargetStyles{L"Taskbar.TaskListButtonPanel#ExperienceToggleButtonRootPanel > Border", {
        L"Opacity=0.5"}},
    ThemeTargetStyles{L"Taskbar.TaskListLabeledButtonPanel > Border#BackgroundElement", {
        L"Opacity=0.5"}},
    ThemeTargetStyles{L"SearchUx.SearchUI.SearchPillButton > SearchUx.SearchUI.SearchButtonRootGrid > Grid#SearchBoxContentGrid", {
        L"Width=344"}},
    ThemeTargetStyles{L"SearchUx.SearchUI.SearchBoxButton > SearchUx.SearchUI.SearchButtonRootGrid@CommonStates > Border", {
        L"BorderBrush@InactiveNormal_SearchBox_Wave3:=<SolidColorBrush Color=\"{ThemeResource SystemChromeHighColor}\" Opacity=\"0.7\" />",
        L"Background@InactiveNormal_SearchBox_Wave3:=<SolidColorBrush Color=\"{ThemeResource SystemBaseHighColor}\" Opacity=\"0.06\" />",
        L"Background@InactivePointerOver_SearchBox_Wave3:=<SolidColorBrush Color=\"{ThemeResource SystemChromeHighColor}\" Opacity=\"0.2\" />",
        L"Background@InactivePressed_SearchBox_Wave3:=<SolidColorBrush Color=\"{ThemeResource SystemChromeHighColor}\" Opacity=\"0.05\" />",
        L"BorderThickness@InactivePointerOver_SearchBox_Wave3=2",
        L"BorderThickness@InactivePressed_SearchBox_Wave3=2",
        L"BorderThickness@InactiveNormal_SearchBox_Wave3=1",
        L"Background@ActivePointerOver_SearchBox_Wave3:=<SolidColorBrush Color=\"{ThemeResource SystemChromeHighColor}\" Opacity=\"0\" />",
        L"Background@ActivePressed_SearchBox_Wave3:=<SolidColorBrush Color=\"{ThemeResource SystemChromeHighColor}\" Opacity=\"0\" />",
        L"Background@ActiveNormal_SearchBox_Wave3:=<SolidColorBrush Color=\"{ThemeResource SystemChromeHighColor}\" Opacity=\"0\" />",
        L"BorderBrush@InactivePointerOver_SearchBox_Wave3:=<SolidColorBrush Color=\"{ThemeResource SystemChromeHighColor}\" Opacity=\"0.7\" />",
        L"BorderBrush@InactivePressed_SearchBox_Wave3:=<SolidColorBrush Color=\"{ThemeResource SystemChromeHighColor}\" Opacity=\"0.7\" />",
        L"BorderThickness@InactivePointerOver_SearchBoxCustomTheme=2",
        L"BorderThickness@InactivePressed_SearchBoxCustomTheme=2",
        L"BorderThickness@InactiveNormal_SearchBoxCustomTheme=1",
        L"BorderBrush@InactiveNormal_SearchBoxCustomTheme:=<SolidColorBrush Color=\"{ThemeResource SystemChromeHighColor}\" Opacity=\"0.7\" />",
        L"BorderBrush@InactivePointerOver_SearchBoxCustomTheme:=<SolidColorBrush Color=\"{ThemeResource SystemChromeHighColor}\" Opacity=\"0.7\" />",
        L"BorderBrush@InactivePressed_SearchBoxCustomTheme:=<SolidColorBrush Color=\"{ThemeResource SystemChromeHighColor}\" Opacity=\"0.7\" />",
        L"Background@InactivePointerOver_SearchBoxCustomTheme:=<SolidColorBrush Color=\"White\" Opacity=\"1\" />",
        L"Background@InactiveNormal_SearchBoxCustomTheme:=<SolidColorBrush Color=\"White\" Opacity=\"0.9\" />",
        L"Background@InactivePressed_SearchBoxCustomTheme:=<SolidColorBrush Color=\"White\" Opacity=\"0.7\" />"}},
}};

const Theme g_theme21996Taskbar = {{
    ThemeTargetStyles{L"Taskbar.SearchBoxButton#SearchBoxButton > Taskbar.TaskListButtonPanel#ExperienceToggleButtonRootPanel > Windows.UI.Xaml.Controls.Border#BackgroundElement", {
        L"CornerRadius=4",
        L"BorderThickness=0"}},
    ThemeTargetStyles{L"Taskbar.SearchBoxButton", {
        L"Margin=0"}},
    ThemeTargetStyles{L"SystemTray.OmniButton#NotificationCenterButton > Grid > ContentPresenter > ItemsPresenter > StackPanel > ContentPresenter > SystemTray.IconView#SystemTrayIcon > Grid > Grid > SystemTray.TextIconContent > Windows.UI.Xaml.Controls.Grid > SystemTray.AdaptiveTextBlock > Windows.UI.Xaml.Controls.TextBlock", {
        L"Visibility=Visible",
        L"Text=\u200E \u200E\u200E\u200E\uE91C",
        L"FontSize=16.4",
        L"FontFamily=Segoe MDL2 Assets",
        L"Width=30",
        L"FontWeight=ExtraLight",
        L"Foreground:=<SolidColorBrush Color=\"{ThemeResource SystemBaseHighColor}\" />"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.FontIcon#SearchBoxFontIcon", {
        L"FontFamily=Segoe Fluent Icons"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.TextBlock#SearchBoxTextBlock", {
        L"Text=Type here to search",
        L"FontSize=14"}},
    ThemeTargetStyles{L"SystemTray.NotifyIconView#NotifyItemIcon", {
        L"CornerRadius=0",
        L"Height=Auto",
        L"Margin=0,0,0,0",
        L"Padding=0",
        L"BorderThickness=0"}},
    ThemeTargetStyles{L"SystemTray.ChevronIconView", {
        L"CornerRadius=0",
        L"Height=Auto",
        L"Width=24",
        L"BorderThickness=0",
        L"Padding=0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.Button#GleamEntryPointButton > Windows.UI.Xaml.Controls.Border", {
        L"CornerRadius=4"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#DynamicSearchBoxGleamContainer", {
        L"CornerRadius=4"}},
    ThemeTargetStyles{L"SystemTray.OmniButton#NotificationCenterButton", {
        L"CornerRadius=0",
        L"Padding=0,0,0,0",
        L"Margin=0,0,0,0",
        L"BorderThickness=0"}},
    ThemeTargetStyles{L"SystemTray.Stack#NonActivatableStack", {
        L"Height=Auto",
        L"CornerRadius=0",
        L"Margin=0,0,0,0",
        L"Padding=0",
        L"BorderThickness=0"}},
    ThemeTargetStyles{L"Rectangle#ShowDesktopPipe@CommonStates", {
        L"Width=9",
        L"Margin=0,0,-10,0",
        L"Height=500",
        L"Fill@Active:=<AcrylicBrush TintColor=\"{ThemeResource SystemBaseLowColor}\" TintOpacity=\"0.5\" Opacity=\"0\"/>",
        L"Stroke:=<SolidColorBrush Color=\"{ThemeResource SystemBaseHighColor}\" Opacity=\"0.5\"/>"}},
    ThemeTargetStyles{L"SystemTray.OmniButton#ControlCenterButton", {
        L"Padding=0",
        L"CornerRadius=0",
        L"Margin=0,0,0,0",
        L"BorderThickness=0"}},
    ThemeTargetStyles{L"SystemTray.AdaptiveTextBlock#LanguageInnerTextBlock > TextBlock#InnerTextBlock", {
        L"FontFamily=Segoe UI",
        L"Margin=-8,0,0,0",
        L"FontSize=12"}},
    ThemeTargetStyles{L"SystemTray.SystemTrayFrame > Windows.UI.Xaml.Controls.Grid#SystemTrayFrameGrid > SystemTray.Stack#NotifyIconStack > Windows.UI.Xaml.Controls.Grid#Content > SystemTray.StackListView#IconStack > Windows.UI.Xaml.Controls.ItemsPresenter > Windows.UI.Xaml.Controls.StackPanel > Windows.UI.Xaml.Controls.ContentPresenter > SystemTray.ChevronIconView > Windows.UI.Xaml.Controls.Grid#ContainerGrid > Windows.UI.Xaml.Controls.ContentPresenter#ContentPresenter > Windows.UI.Xaml.Controls.Grid#ContentGrid > SystemTray.TextIconContent > Windows.UI.Xaml.Controls.Grid#ContainerGrid > SystemTray.AdaptiveTextBlock#Base > Windows.UI.Xaml.Controls.TextBlock#InnerTextBlock", {
        L"FontFamily=Segoe MDL2 Assets",
        L"FontSize=12.4",
        L"Foreground:=<SolidColorBrush Color=\"{ThemeResource SystemBaseHighColor}\" />"}},
    ThemeTargetStyles{L"SystemTray.SystemTrayFrame > Windows.UI.Xaml.Controls.Grid#SystemTrayFrameGrid > SystemTray.Stack#NotifyIconStack > Windows.UI.Xaml.Controls.Grid#Content > SystemTray.StackListView#IconStack > Windows.UI.Xaml.Controls.ItemsPresenter > Windows.UI.Xaml.Controls.StackPanel > Windows.UI.Xaml.Controls.ContentPresenter", {
        L"Width=30"}},
    ThemeTargetStyles{L"SystemTray.AdaptiveTextBlock#Base > Windows.UI.Xaml.Controls.TextBlock#InnerTextBlock", {
        L"FontFamily=Segoe MDL2 Assets",
        L"Foreground:=<SolidColorBrush Color=\"{ThemeResource SystemBaseHighColor}\" />"}},
    ThemeTargetStyles{L"SystemTray.AdaptiveTextBlock#AccentOverlay > Windows.UI.Xaml.Controls.TextBlock#InnerTextBlock", {
        L"FontFamily=Segoe MDL2 Assets"}},
    ThemeTargetStyles{L"SystemTray.AdaptiveTextBlock#Underlay > Windows.UI.Xaml.Controls.TextBlock#InnerTextBlock", {
        L"FontFamily=Segoe MDL2 Assets"}},
    ThemeTargetStyles{L"SystemTray.OmniButton#ControlCenterButton > Grid > ContentPresenter > ItemsPresenter > StackPanel > ContentPresenter[1] > SystemTray.IconView > Grid > Grid", {
        L"Margin=-6,0,0,0"}},
    ThemeTargetStyles{L"SystemTray.Stack#MainStack > Windows.UI.Xaml.Controls.Grid#Content", {
        L"CornerRadius=0",
        L"Height=Auto",
        L"Margin=0,0,0,0",
        L"Padding=0",
        L"BorderThickness=0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.StackPanel > Windows.UI.Xaml.Controls.TextBlock#TimeInnerTextBlock", {
        L"FontFamily=Segoe UI",
        L"TextAlignment=0",
        L"FontSize=12",
        L"Margin=0,-1,0,0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.StackPanel > Windows.UI.Xaml.Controls.TextBlock#DateInnerTextBlock", {
        L"FontFamily=Segoe UI",
        L"TextAlignment=0",
        L"FontSize=12",
        L"Margin=0,2,0,0"}},
    ThemeTargetStyles{L"SystemTray.NotificationAreaIcons#NotificationAreaIcons > Windows.UI.Xaml.Controls.ItemsPresenter > Windows.UI.Xaml.Controls.StackPanel > Windows.UI.Xaml.Controls.ContentPresenter", {
        L"Width=23",
        L"Margin=0,-2,0,0"}},
    ThemeTargetStyles{L"SystemTray.NotificationAreaIcons#NotificationAreaIcons > Windows.UI.Xaml.Controls.ItemsPresenter > Windows.UI.Xaml.Controls.StackPanel > Windows.UI.Xaml.Controls.ContentPresenter > SystemTray.NotifyIconView#NotifyItemIcon > Windows.UI.Xaml.Controls.Grid#ContainerGrid", {
        L"Transform3D:=<CompositeTransform3D TranslateY=\"0\" TranslateX=\"0\" />",
        L"HorizontalAlignment=0"}},
    ThemeTargetStyles{L"SystemTray.Stack#NotifyIconStack", {
        L"Width=24"}},
    ThemeTargetStyles{L"SystemTray.OmniButton#ControlCenterButton > Grid > ContentPresenter > ItemsPresenter > StackPanel > ContentPresenter > SystemTray.IconView#SystemTrayIcon > Grid > Grid > SystemTray.TextIconContent > Windows.UI.Xaml.Controls.Grid > SystemTray.AdaptiveTextBlock > Windows.UI.Xaml.Controls.TextBlock", {
        L"FontSize=16",
        L"Margin=0,-1,-0,0",
        L"FontWeight=0"}},
    ThemeTargetStyles{L"SystemTray.CopilotIcon#CopilotIcon", {
        L"Visibility=Visible",
        L"Margin=0,-7,0,0",
        L"Height=61"}},
    ThemeTargetStyles{L"SystemTray.NotificationAreaOverflow > Windows.UI.Xaml.Controls.Grid#OverflowRootGrid > Windows.UI.Xaml.Controls.Border#OverflowFlyoutBackgroundBorder", {
        L"CornerRadius=0",
        L"Background:=<AcrylicBrush TintColor=\"{ThemeResource SystemAltHighColor}\" TintOpacity=\"0.7\" FallbackColor=\"{ThemeResource SystemChromeMediumColor}\" />"}},
    ThemeTargetStyles{L"SystemTray.NotificationAreaOverflow > Windows.UI.Xaml.Controls.Grid#OverflowRootGrid > Windows.UI.Xaml.Controls.ItemsControl > Windows.UI.Xaml.Controls.ItemsPresenter > Windows.UI.Xaml.Controls.WrapGrid", {
        L"Margin=0,0,0,0"}},
    ThemeTargetStyles{L"SystemTray.NotifyIconView", {
        L"CornerRadius=0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.ScrollViewer > Windows.UI.Xaml.Controls.ScrollContentPresenter > Windows.UI.Xaml.Controls.Border > SystemTray.NotificationAreaOverflow", {
        L"Transform3D:=<CompositeTransform3D TranslateY=\"13\" />"}},
    ThemeTargetStyles{L"SystemTray.OmniButton#ControlCenterButton", {
        L"Visibility=Visible"}},
    ThemeTargetStyles{L"Taskbar.TaskbarBackground#BackgroundControl > Grid", {
        L"Background:=<AcrylicBrush TintColor=\"{ThemeResource SystemChromeMediumHighColor}\" TintOpacity=\"0\" TintLuminosityOpacity=\"0.8\" FallbackColor=\"{ThemeResource SystemChromeMediumColor}\" />"}},
    ThemeTargetStyles{L"Taskbar.TaskbarBackground#BackgroundControl > Grid > Windows.UI.Xaml.Shapes.Rectangle#BackgroundFill", {
        L"Opacity=0.5"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Shapes.Rectangle#BackgroundStroke", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"SystemTray.OmniButton#ControlCenterButton > Grid > ContentPresenter > ItemsPresenter > StackPanel > ContentPresenter[3] > SystemTray.IconView > Grid > Grid", {
        L"Margin=2,0,-8,0",
        L"RenderTransform:=<ScaleTransform ScaleX=\"0.86\" />"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.ContentPresenter#HoverFlyoutContent", {
        L"CornerRadius=0",
        L"Margin=0,0,0,-15",
        L"Background:=<AcrylicBrush TintColor=\"{ThemeResource SystemAltHighColor}\" TintOpacity=\"0.7\" FallbackColor=\"{ThemeResource SystemChromeMediumColor}\" />"}},
    ThemeTargetStyles{L"Taskbar.TaskItemThumbnailView > Grid > TextBlock", {
        L"FontFamily=Segoe UI",
        L"FontSize=12",
        L"Margin=3,0,8,8"}},
    ThemeTargetStyles{L"Taskbar.TaskItemThumbnailView > Windows.UI.Xaml.Controls.Grid > Microsoft.UI.Xaml.Controls.ItemsRepeater > Windows.UI.Xaml.Controls.Image", {
        L"Margin=0,-7,0,0"}},
    ThemeTargetStyles{L"Taskbar.TaskItemThumbnailView > Grid > Button > ContentPresenter > TextBlock", {
        L"FontFamily=Segoe MDL2 Assets"}},
    ThemeTargetStyles{L"Taskbar.TaskItemThumbnailView > Grid > Button", {
        L"CornerRadius=0",
        L"Height=32",
        L"Margin=0,0,0,9",
        L"Width=32"}},
    ThemeTargetStyles{L"Grid#DetailedViewGrid", {
        L"Margin=0,-7,0,0"}},
    ThemeTargetStyles{L"Taskbar.TaskItemThumbnailView > Grid > Border", {
        L"BorderBrush:=<SolidColorBrush Color=\"{ThemeResource SystemBaseHighColor}\" Opacity=\"0.5\" />",
        L"CornerRadius=0"}},
    ThemeTargetStyles{L"SystemTray.OmniButton#NotificationCenterButton > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.ContentPresenter > Windows.UI.Xaml.Controls.ItemsPresenter > Windows.UI.Xaml.Controls.StackPanel > Windows.UI.Xaml.Controls.ContentPresenter > SystemTray.IconView > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.ContentPresenter > Windows.UI.Xaml.Controls.Grid > SystemTray.TextIconContent > Windows.UI.Xaml.Controls.Grid > SystemTray.AdaptiveTextBlock#Base > Windows.UI.Xaml.Controls.TextBlock", {
        L"Text=\u200E\uE91C",
        L"FontWeight=Light",
        L"FontSize=16.4",
        L"Foreground:=<SolidColorBrush Color=\"{ThemeResource SystemBaseHighColor}\" />",
        L"Margin=-1,0,1,0"}},
    ThemeTargetStyles{L"SystemTray.OmniButton#NotificationCenterButton > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.ContentPresenter > Windows.UI.Xaml.Controls.ItemsPresenter > Windows.UI.Xaml.Controls.StackPanel > Windows.UI.Xaml.Controls.ContentPresenter > SystemTray.IconView", {
        L"CornerRadius=0",
        L"Padding=0,0,0,0"}},
    ThemeTargetStyles{L"SystemTray.DateTimeIconContent > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.StackPanel > Windows.UI.Xaml.Controls.TextBlock", {
        L"FontFamily=Segoe UI",
        L"TextAlignment=Center"}},
    ThemeTargetStyles{L"SystemTray.OmniButton#ControlCenterButton > Grid > ContentPresenter > ItemsPresenter > StackPanel > ContentPresenter[2] > SystemTray.IconView > Grid > Grid", {
        L"Margin=0,0,-3,0"}},
    ThemeTargetStyles{L"Taskbar.ThumbBarButton > ContentPresenter", {
        L"CornerRadius=0",
        L"Background:=<SolidColorBrush Opacity=\"0.5\" />",
        L"BorderBrush:=<AcrylicBrush TintColor=\"{ThemeResource SystemChromeLowColor}\" FallbackColor=\"{ThemeResource SystemChromeLowColor}\" />"}},
    ThemeTargetStyles{L"Taskbar.ThumbBarButton > ContentPresenter > Image", {
        L"Height=15",
        L"Width=15"}},
    ThemeTargetStyles{L"SearchUx.SearchUI.SearchButtonRootGrid > Border", {
        L"CornerRadius=4"}},
    ThemeTargetStyles{L"TextBlock#BatteryTextBlock", {
        L"FontFamily=Segoe UI",
        L"Margin=2,0,-2,0"}},
    ThemeTargetStyles{L"SystemTray.BatteryIconContent > Grid > Windows.UI.Xaml.Controls.StackPanel > Grid > TextBlock", {
        L"RenderTransform:=<ScaleTransform ScaleX=\"1\" />"}},
    ThemeTargetStyles{L"SystemTray.IconView#SystemTrayIcon > Grid#ContainerGrid > Border#BackgroundBorder", {
        L"CornerRadius=0",
        L"Opacity=0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Button#GleamEntryPointButton > Windows.UI.Xaml.Controls.Border > Windows.UI.Xaml.Controls.ContentPresenter", {
        L"CornerRadius=3"}},
    ThemeTargetStyles{L"SearchUx.SearchUI.SearchButtonRootGrid", {
        L"Margin=2,0,0,0"}},
    ThemeTargetStyles{L"SystemTray.ChevronIconView > Grid > ContentPresenter", {
        L"Transform3D:=<CompositeTransform3D TranslateY=\"-1\" TranslateX=\"2\" />",
        L"HorizontalAlignment=0"}},
    ThemeTargetStyles{L"Border#SearchPillBackgroundElement", {
        L"CornerRadius=4"}},
    ThemeTargetStyles{L"Grid#ConfirmatorMainGrid", {
        L"Background:=<AcrylicBrush TintColor=\"{ThemeResource SystemChromeMediumHighColor}\" TintOpacity=\"0\" TintLuminosityOpacity=\"0.8\" FallbackColor=\"{ThemeResource SystemChromeMediumColor}\" />",
        L"BorderBrush:=<AcrylicBrush TintColor=\"{ThemeResource SystemChromeHighColor}\" TintOpacity=\"0\" TintLuminosityOpacity=\"0.3\" FallbackColor=\"{ThemeResource SystemChromeMediumColor}\" />",
        L"CornerRadius=6",
        L"BorderThickness=0,1,0,0"}},
    ThemeTargetStyles{L"Slider > Grid > Grid > Grid > Windows.UI.Xaml.Controls.Primitives.Thumb#HorizontalThumb", {
        L"Visibility=Visible",
        L"Height=18",
        L"Width=18"}},
    ThemeTargetStyles{L"Slider > Grid > Grid > Grid > Windows.UI.Xaml.Controls.Primitives.Thumb#HorizontalThumb > Border", {
        L"BorderBrush:=<AcrylicBrush TintColor=\"{ThemeResource SystemChromeLowColor}\" TintOpacity=\"0.7\" TintLuminosityOpacity=\"0.7\" FallbackColor=\"{ThemeResource SystemChromeMediumColor}\" />",
        L"BorderThickness=4",
        L"Background:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColorLight1}\" />",
        L"CornerRadius=10"}},
    ThemeTargetStyles{L"Slider > Grid > Grid > Grid > Rectangle#HorizontalDecreaseRect", {
        L"Fill:=<SolidColorBrush Color=\"{ThemeResource SystemAccentColorLight1}\" />"}},
    ThemeTargetStyles{L"Slider > Grid > Grid > Grid > Windows.UI.Xaml.Controls.Primitives.Thumb#HorizontalThumb", {
        L"BorderBrush:=<AcrylicBrush TintColor=\"{ThemeResource SystemChromeHighColor}\" TintOpacity=\"0.7\" TintLuminosityOpacity=\"0.7\" FallbackColor=\"{ThemeResource SystemChromeMediumColor}\" />",
        L"BorderThickness=0,1,0,0"}},
}};

const Theme g_themeBottomDensy = {{
    ThemeTargetStyles{L"Taskbar.TaskbarFrame > Grid#RootGrid > Taskbar.TaskbarBackground > Grid > Rectangle#BackgroundFill", {
        L"Fill=Transparent"}},
    ThemeTargetStyles{L"Rectangle#BackgroundStroke", {
        L"Fill=Transparent"}},
    ThemeTargetStyles{L"Taskbar.TaskListLabeledButtonPanel@RunningIndicatorStates > Rectangle#RunningIndicator", {
        L"Fill=#8f8f8f",
        L"Fill@ActiveRunningIndicator=#fef9f0",
        L"Width=2",
        L"Height=2",
        L"Margin=0,-2,0,0",
        L"Width@ActiveRunningIndicator=32"}},
    ThemeTargetStyles{L"Taskbar.TaskListLabeledButtonPanel@RunningIndicatorStates > muxc:ProgressBar#ProgressIndicator", {
        L"VerticalAlignment=0"}},
    ThemeTargetStyles{L"Rectangle#RunningIndicator", {
        L"VerticalAlignment=0"}},
    ThemeTargetStyles{L"Border#ProgressBarRoot", {
        L"VerticalAlignment=0"}},
    ThemeTargetStyles{L"Rectangle#IndeterminateProgressBarIndicator", {
        L"VerticalAlignment=0"}},
    ThemeTargetStyles{L"Rectangle#IndeterminateProgressBarIndicator2", {
        L"VerticalAlignment=0"}},
    ThemeTargetStyles{L"Taskbar.TaskListLabeledButtonPanel", {
        L"Padding=2,0,2,0",
        L"VerticalAlignment=2"}},
    ThemeTargetStyles{L"Taskbar.ExperienceToggleButton#LaunchListButton[AutomationProperties.AutomationId=StartButton]", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"SystemTray.Stack#ShowDesktopStack", {
        L"Width=1"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Shapes.Rectangle#ShowDesktopPipe", {
        L"HorizontalAlignment=0"}},
    ThemeTargetStyles{L"SystemTray.NotificationAreaIcons#NotificationAreaIcons > ItemsPresenter > StackPanel > ContentPresenter > SystemTray.NotifyIconView#NotifyItemIcon > Grid#ContainerGrid > ContentPresenter#ContentPresenter > Grid#ContentGrid > SystemTray.ImageIconContent > Grid#ContainerGrid > Image", {
        L"Width=20",
        L"Height=20"}},
    ThemeTargetStyles{L"WrapGrid > ContentPresenter > SystemTray.NotifyIconView > Grid#ContainerGrid > ContentPresenter#ContentPresenter > Grid#ContentGrid > SystemTray.ImageIconContent > Grid#ContainerGrid > Image", {
        L"Width=20",
        L"Height=20"}},
}};

const Theme g_themeBottomDensy_variant_NoInd = {{
    ThemeTargetStyles{L"Taskbar.TaskbarFrame > Grid#RootGrid > Taskbar.TaskbarBackground > Grid > Rectangle#BackgroundFill", {
        L"Fill=Transparent"}},
    ThemeTargetStyles{L"Rectangle#BackgroundStroke", {
        L"Fill=Transparent"}},
    ThemeTargetStyles{L"Taskbar.TaskListLabeledButtonPanel@RunningIndicatorStates > Rectangle#RunningIndicator", {
        L"Fill=#8f8f8f",
        L"Fill@ActiveRunningIndicator=#fef9f0",
        L"Width=0",
        L"Height=0",
        L"Margin=0,0,0,0",
        L"Width@ActiveRunningIndicator=32",
        L"Height@ActiveRunningIndicator=2",
        L"Margin@ActiveRunningIndicator=0,-2,0,0"}},
    ThemeTargetStyles{L"Taskbar.TaskListLabeledButtonPanel@RunningIndicatorStates > muxc:ProgressBar#ProgressIndicator", {
        L"VerticalAlignment=0"}},
    ThemeTargetStyles{L"Rectangle#RunningIndicator", {
        L"VerticalAlignment=0"}},
    ThemeTargetStyles{L"Border#ProgressBarRoot", {
        L"VerticalAlignment=0"}},
    ThemeTargetStyles{L"Rectangle#IndeterminateProgressBarIndicator", {
        L"VerticalAlignment=0"}},
    ThemeTargetStyles{L"Rectangle#IndeterminateProgressBarIndicator2", {
        L"VerticalAlignment=0"}},
    ThemeTargetStyles{L"Taskbar.TaskListLabeledButtonPanel", {
        L"Padding=2,0,2,0",
        L"VerticalAlignment=2"}},
    ThemeTargetStyles{L"Taskbar.ExperienceToggleButton#LaunchListButton[AutomationProperties.AutomationId=StartButton]", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"SystemTray.Stack#ShowDesktopStack", {
        L"Width=1"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Shapes.Rectangle#ShowDesktopPipe", {
        L"HorizontalAlignment=0"}},
    ThemeTargetStyles{L"SystemTray.NotificationAreaIcons#NotificationAreaIcons > ItemsPresenter > StackPanel > ContentPresenter > SystemTray.NotifyIconView#NotifyItemIcon > Grid#ContainerGrid > ContentPresenter#ContentPresenter > Grid#ContentGrid > SystemTray.ImageIconContent > Grid#ContainerGrid > Image", {
        L"Width=20",
        L"Height=20"}},
    ThemeTargetStyles{L"WrapGrid > ContentPresenter > SystemTray.NotifyIconView > Grid#ContainerGrid > ContentPresenter#ContentPresenter > Grid#ContentGrid > SystemTray.ImageIconContent > Grid#ContainerGrid > Image", {
        L"Width=20",
        L"Height=20"}},
    ThemeTargetStyles{L"Taskbar.TaskListLabeledButtonPanel@RunningIndicatorStates > Image#Icon", {
        L"Width@ActiveRunningIndicator=30",
        L"Height@ActiveRunningIndicator=30",
        L"Width@NoRunningIndicator=26",
        L"Height@NoRunningIndicator=26",
        L"Margin@NoRunningIndicator=0,6,0,0"}},
}};

const Theme g_themeTaskbarXII = {{
    ThemeTargetStyles{L"ScrollViewer > ScrollContentPresenter > Border > Grid", {
        L"Background:=<AcrylicBrush TintColor=\"{ThemeResource SystemListLowColor}\" TintOpacity=\"0.1\" FallbackColor=\"{ThemeResource SystemChromeHighColor}\" />"}},
    ThemeTargetStyles{L"Taskbar.TaskbarFrame", {
        L"HorizontalAlignment=Right",
        L"Width=Auto",
        L"Height=56",
        L"Grid.Column=0",
        L"Margin=0,0,2,0"}},
    ThemeTargetStyles{L"Taskbar.TaskbarFrame > Grid", {
        L"Height=48",
        L"CornerRadius=4"}},
    ThemeTargetStyles{L"Taskbar.TaskbarBackground#BackgroundControl", {
        L"Height=48",
        L"Opacity=0.7"}},
    ThemeTargetStyles{L"Taskbar.TaskbarBackground > Grid", {
        L"CornerRadius=4",
        L"Opacity=1"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.ItemsRepeater#TaskbarFrameRepeater", {
        L"Margin=0,0,3,0"}},
    ThemeTargetStyles{L"Taskbar.SearchBoxButton > Taskbar.TaskListButtonPanel", {
        L"Margin=2,0,6,0"}},
    ThemeTargetStyles{L"TextBlock#SearchBoxTextBlock", {
        L"Text=\u2726 Meow"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Shapes.Rectangle#BackgroundStroke", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Border#LargeTicker1", {
        L"Margin=0,2,4,0"}},
    ThemeTargetStyles{L"Border#LargeTicker1 > AdaptiveCards.Rendering.Uwp.WholeItemsPanel > Image", {
        L"MaxHeight=27",
        L"MaxWidth=27"}},
    ThemeTargetStyles{L"Border#LargeTicker1 > AdaptiveCards.Rendering.Uwp.WholeItemsPanel > Microsoft.UI.Xaml.Controls.AnimatedVisualPlayer", {
        L"MaxHeight=27",
        L"MaxWidth=27"}},
    ThemeTargetStyles{L"SystemTray.SystemTrayFrame", {
        L"HorizontalAlignment=Left",
        L"Grid.Column=1",
        L"Margin=2,0,0,0"}},
    ThemeTargetStyles{L"Grid#SystemTrayFrameGrid", {
        L"Background:=<SolidColorBrush Color=\"{ThemeResource SystemChromeAltHighColor}\" Opacity=\"0.6\" />",
        L"CornerRadius=4",
        L"Padding=8,3,0,3"}},
    ThemeTargetStyles{L"SystemTray.Stack#SecondaryClockStack", {
        L"Grid.Column=8"}},
    ThemeTargetStyles{L"SystemTray.OmniButton#ControlCenterButton", {
        L"Grid.Column=4"}},
    ThemeTargetStyles{L"SystemTray.OmniButton#NotificationCenterButton", {
        L"Grid.Column=5"}},
    ThemeTargetStyles{L"SystemTray.Stack#MainStack", {
        L"Grid.Column=6"}},
    ThemeTargetStyles{L"SystemTray.Stack#ShowDesktopStack", {
        L"Grid.Column=7"}},
    ThemeTargetStyles{L"SystemTray.DateTimeIconContent > Grid > StackPanel", {
        L"Orientation=Horizontal",
        L"Spacing=12"}},
    ThemeTargetStyles{L"TextBlock#InnerTextBlock[Text=\uE971]", {
        L"Text=\uED14"}},
    ThemeTargetStyles{L"TextBlock#TimeInnerTextBlock", {
        L"FontSize=15",
        L"FontWeight=Bold"}},
    ThemeTargetStyles{L"TextBlock#DateInnerTextBlock", {
        L"FontSize=15",
        L"FontWeight=SemiBold"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.ScrollViewer > Windows.UI.Xaml.Controls.ScrollContentPresenter > Windows.UI.Xaml.Controls.Border > Windows.UI.Xaml.Controls.Grid", {
        L"ColumnDefinitions:=<ColumnDefinitionCollection><ColumnDefinition Width=\"*\"/><ColumnDefinition Width=\"*\"/></ColumnDefinitionCollection>"}},
    ThemeTargetStyles{L"Taskbar.AugmentedEntryPointButton > Taskbar.TaskListButtonPanel", {
        L"Margin=0"}},
}};

const Theme g_themexdark = {{
    ThemeTargetStyles{L"Taskbar.TaskListButton", {
        L"CornerRadius=13",
        L"Padding=6,0,6,0",
        L"HorizontalContentAlignment=Left"}},
    ThemeTargetStyles{L"SystemTray.TextIconContent > Grid#ContainerGrid > SystemTray.AdaptiveTextBlock#Base > TextBlock#InnerTextBlock", {
        L"FontSize=16",
        L"Foreground=#facc15"}},
    ThemeTargetStyles{L"SystemTray.NotifyIconView#NotifyItemIcon", {
        L"MinWidth=25"}},
    ThemeTargetStyles{L"SystemTray.OmniButton#ControlCenterButton > Grid > ContentPresenter > ItemsPresenter > StackPanel > ContentPresenter[1] > SystemTray.IconView > Grid > Grid", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"SystemTray.TextIconContent > Grid#ContainerGrid", {
        L"Padding=2"}},
    ThemeTargetStyles{L"SystemTray.ChevronIconView", {
        L"MinWidth=27"}},
    ThemeTargetStyles{L"SystemTray.OmniButton#NotificationCenterButton > Grid > ContentPresenter > ItemsPresenter > StackPanel > ContentPresenter > SystemTray.IconView#SystemTrayIcon > Grid > Grid > SystemTray.TextIconContent", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Taskbar.TaskListLabeledButtonPanel > Border#BackgroundElement", {
        L"Background=#000000"}},
    ThemeTargetStyles{L"Grid#SystemTrayFrameGrid", {
        L"Background=#000000",
        L"CornerRadius=13",
        L"Margin=0,5,4,5",
        L"Padding=2,0,-18,0"}},
    ThemeTargetStyles{L"Taskbar.TaskListButton > Grid > Rectangle#RunningIndicator", {
        L"Height=3",
        L"RadiusX=1.5",
        L"RadiusY=1.5",
        L"Fill@ActiveNormal=#facc15",
        L"VerticalAlignment=Bottom",
        L"Margin=16,0,16,4",
        L"StrokeThickness=0"}},
    ThemeTargetStyles{L"SystemTray.ImageIconContent > Grid#ContainerGrid > Image", {
        L"Width=13"}},
    ThemeTargetStyles{L"SystemTray.TextIconContent > Grid#ContainerGrid > SystemTray.AdaptiveTextBlock#Base > TextBlock#InnerTextBlock", {
        L"FontSize=13",
        L"Foreground=#facc15"}},
    ThemeTargetStyles{L"TextBlock#LabelControl", {
        L"FontFamily=Segoe UI Medium",
        L"Foreground=#facc15",
        L"Margin=1,0,0,0",
        L"VerticalAlignment=Center",
        L"TextWrapping=NoWrap"}},
    ThemeTargetStyles{L"Taskbar.ExperienceToggleButton#LaunchListButton[AutomationProperties.AutomationId=StartButton]", {
        L"Visibility=Visible"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.TextBlock#InnerTextBlock[Text=\uE971]", {
        L"Text=\uE712",
        L"Foreground=#facc15"}},
    ThemeTargetStyles{L"Taskbar.TaskbarFrame > Grid#RootGrid > Taskbar.TaskbarBackground > Grid > Rectangle#BackgroundFill", {
        L"Fill=Transparent"}},
    ThemeTargetStyles{L"Rectangle#BackgroundStroke", {
        L"Fill=Transparent"}},
    ThemeTargetStyles{L"SystemTray.TextIconContent > Grid#ContainerGrid > SystemTray.AdaptiveTextBlock#Base > TextBlock#InnerTextBlock", {
        L"Foreground=#facc15"}},
}};

const Theme g_themeWindows7 = {{
    ThemeTargetStyles{L"Taskbar.TaskListLabeledButtonPanel@CommonStates > Windows.UI.Xaml.Controls.Border#BackgroundElement", {
        L"Background@InactiveNormal:=<ImageBrush Stretch=\"Fill\" ImageSource=\"$taskbandInactiveNormal\" />",
        L"Background@InactivePointerOver:=<ImageBrush Stretch=\"Fill\" ImageSource=\"$taskbandInactivePointerOver\" />",
        L"Background@ActiveNormal:=<ImageBrush Stretch=\"Fill\" ImageSource=\"$taskbandActiveNormal\" />",
        L"Background@ActivePressed:=<ImageBrush Stretch=\"Fill\" ImageSource=\"$taskbandActivePressed\" />",
        L"Background@ActivePointerOver:=<ImageBrush Stretch=\"Fill\" ImageSource=\"$taskbandActivePointerOver\" />",
        L"Background@InactivePressed:=<ImageBrush Stretch=\"Fill\" ImageSource=\"$taskbandInactivePressed\" />",
        L"BorderThickness=0",
        L"Background@MultiWindowNormal:=<ImageBrush Stretch=\"Fill\" ImageSource=\"$taskbandInactiveNormal\" />",
        L"Background@MultiWindowActive:=<ImageBrush Stretch=\"Fill\" ImageSource=\"$taskbandActiveNormal\" />",
        L"Background@MultiWindowPressed:=<ImageBrush Stretch=\"Fill\" ImageSource=\"$taskbandActivePressed\" />",
        L"Background@MultiWindowPointerOver:=<ImageBrush Stretch=\"Fill\" ImageSource=\"$taskbandActivePointerOver\" />",
        L"Background@RequestingAttention:=<ImageBrush Stretch=\"Fill\" ImageSource=\"$taskbandRequestingAttention\" />",
        L"Background@RequestingAttentionPointerOver:=<ImageBrush Stretch=\"Fill\" ImageSource=\"$taskbandRequestingAttention\" />",
        L"Background@RequestingAttentionPressed:=<ImageBrush Stretch=\"Fill\" ImageSource=\"$taskbandRequestingAttention\" />",
        L"Background@RequestingAttentionMulti:=<ImageBrush Stretch=\"Fill\" ImageSource=\"$taskbandRequestingAttention\" />",
        L"Background@RequestingAttentionMultiPointerOver:=<ImageBrush Stretch=\"Fill\" ImageSource=\"$taskbandRequestingAttention\" />",
        L"Background@RequestingAttentionMultiPressed:=<ImageBrush Stretch=\"Fill\" ImageSource=\"$taskbandRequestingAttention\" />"}},
    ThemeTargetStyles{L"Taskbar.TaskListLabeledButtonPanel", {
        L"Padding=0,0,0,0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Shapes.Rectangle#RunningIndicator", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Taskbar.TaskListLabeledButtonPanel > Image", {
        L"RenderTransform:=<TranslateTransform X=\"2\" />"}},
    ThemeTargetStyles{L"Taskbar.TaskListLabeledButtonPanel@RunningIndicatorStates > Windows.UI.Xaml.Controls.Border#BackgroundElement", {
        L"Opacity@NoRunningIndicator=0"}},
    ThemeTargetStyles{L"Taskbar.ExperienceToggleButton#LaunchListButton[AutomationProperties.AutomationId=StartButton] > Taskbar.TaskListButtonPanel@CommonStates > Border#BackgroundElement", {
        L"Background@InactiveNormal:=<ImageBrush Stretch=\"None\" ImageSource=\"$orbNormal\" />",
        L"Background@InactivePointerOver:=<ImageBrush Stretch=\"None\" ImageSource=\"$orbPointerOver\" />",
        L"Background@InactivePressed:=<ImageBrush Stretch=\"None\" ImageSource=\"$orbPressed\" />",
        L"Background@ActiveNormal:=<ImageBrush Stretch=\"None\" ImageSource=\"$orbPressed\" />",
        L"Background@ActivePointerOver:=<ImageBrush Stretch=\"None\" ImageSource=\"$orbPointerOver\" />",
        L"Background@ActivePressed:=<ImageBrush Stretch=\"None\" ImageSource=\"$orbPressed\" />",
        L"BorderThickness=0",
        L"Width=54"}},
    ThemeTargetStyles{L"Taskbar.ExperienceToggleButton#LaunchListButton[AutomationProperties.AutomationId=StartButton] > Taskbar.TaskListButtonPanel > Microsoft.UI.Xaml.Controls.AnimatedVisualPlayer#Icon", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Taskbar.TaskListButtonPanel#ExperienceToggleButtonRootPanel", {
        L"Padding=0,0,0,0",
        L"MinWidth=55",
        L"Margin=0,0,5,0"}},
    ThemeTargetStyles{L"Taskbar.TaskbarFrame > Grid#RootGrid > Taskbar.TaskbarBackground > Grid", {
        L"Background:=<WindhawkBlur BlurAmount=\"3\" TintOpacity=\"$aeroOpacity\" TintColor=\"$aeroColor\" />"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Shapes.Rectangle#BackgroundStroke", {
        L"Fill:=<ImageBrush Stretch=\"UniformToFill\" ImageSource=\"$reflection\" />",
        L"Height=100"}},
    ThemeTargetStyles{L"Taskbar.TaskbarFrame > Grid#RootGrid > Taskbar.TaskbarBackground > Grid > Rectangle#BackgroundFill", {
        L"Fill:=<ImageBrush Stretch=\"Fill\" ImageSource=\"$taskbarBackground\" />"}},
    ThemeTargetStyles{L"Taskbar.TaskListButton", {
        L"Margin=1,0,1,0"}},
    ThemeTargetStyles{L"Taskbar.TaskListLabeledButtonPanel@CommonStates > Windows.UI.Xaml.Controls.Border#MultiWindowElement", {
        L"Background@MultiWindowNormal:=<ImageBrush Stretch=\"Fill\" ImageSource=\"$taskbandInactiveNormal\" />",
        L"Background@MultiWindowActive:=<ImageBrush Stretch=\"Fill\" ImageSource=\"$taskbandActiveNormal\" />",
        L"Background@MultiWindowPointerOver:=<ImageBrush Stretch=\"Fill\" ImageSource=\"$taskbandActiveNormal\" />",
        L"BorderThickness=0",
        L"RenderTransform:=<TranslateTransform X=\"2\" />",
        L"Background@MultiWindowPressed:=<ImageBrush Stretch=\"Fill\" ImageSource=\"$taskbandActiveNormal\" />",
        L"Background@RequestingAttentionMulti:=<ImageBrush Stretch=\"Fill\" ImageSource=\"$taskbandRequestingAttention\" />",
        L"Background@RequestingAttentionMultiPointerOver:=<ImageBrush Stretch=\"Fill\" ImageSource=\"$taskbandRequestingAttention\" />",
        L"Background@RequestingAttentionMultiPressed:=<ImageBrush Stretch=\"Fill\" ImageSource=\"$taskbandRequestingAttention\" />"}},
    ThemeTargetStyles{L"SystemTray.ChevronIconView > Windows.UI.Xaml.Controls.Grid#ContainerGrid > Windows.UI.Xaml.Controls.ContentPresenter", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"SystemTray.ChevronIconView > Windows.UI.Xaml.Controls.Grid#ContainerGrid@ > Windows.UI.Xaml.Controls.Border#BackgroundBorder", {
        L"Background@Normal:=<ImageBrush Stretch=\"None\" ImageSource=\"$overflowNormal\" />",
        L"Background@PointerOver:=<ImageBrush Stretch=\"None\" ImageSource=\"$overflowPointerOver\" />",
        L"Background@Pressed:=<ImageBrush Stretch=\"None\" ImageSource=\"$overflowPressed\" />",
        L"Background@CheckedNormal:=<ImageBrush Stretch=\"None\" ImageSource=\"$overflowPressed\" />",
        L"Background@CheckedPointerOver:=<ImageBrush Stretch=\"None\" ImageSource=\"$overflowPressed\" />",
        L"Background@CheckedPressed:=<ImageBrush Stretch=\"None\" ImageSource=\"$overflowPressed\" />",
        L"BorderThickness=0",
        L"Width=21",
        L"Height=20"}},
    ThemeTargetStyles{L"SystemTray.OmniButton#NotificationCenterButton > Windows.UI.Xaml.Controls.Grid@CommonStates > Windows.UI.Xaml.Controls.Border#BackgroundBorder", {
        L"Background@Normal=Transparent",
        L"Background@PointerOver:=<ImageBrush Stretch=\"Fill\" ImageSource=\"$clockPointerOver\" />",
        L"Background@Pressed:=<ImageBrush Stretch=\"Fill\" ImageSource=\"$clockPressed\" />",
        L"BorderThickness=0",
        L"Margin=0",
        L"MinWidth=68",
        L"Background@Checked:=<ImageBrush Stretch=\"Fill\" ImageSource=\"$clockPointerOver\" />",
        L"Background@CheckedPointerOver:=<ImageBrush Stretch=\"Fill\" ImageSource=\"$clockPointerOver\" />",
        L"Background@CheckedPressed:=<ImageBrush Stretch=\"Fill\" ImageSource=\"$clockPressed\" />"}},
    ThemeTargetStyles{L"SystemTray.DateTimeIconContent > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.StackPanel > Windows.UI.Xaml.Controls.TextBlock", {
        L"TextAlignment=0",
        L"Foreground=White",
        L"FontFamily=Segoe UI",
        L"FlowDirection=0",
        L"Typography.StylisticSet1=true"}},
    ThemeTargetStyles{L"SystemTray.OmniButton#ControlCenterButton > Windows.UI.Xaml.Controls.Grid@CommonStates > Windows.UI.Xaml.Controls.Border#BackgroundBorder", {
        L"Background@Normal=Transparent",
        L"Background@PointerOver:=<ImageBrush Stretch=\"Fill\" ImageSource=\"$clockPointerOver\" />",
        L"Background@Pressed:=<ImageBrush Stretch=\"Fill\" ImageSource=\"$clockPressed\" />",
        L"BorderThickness=0",
        L"Margin=0",
        L"Background@Checked:=<ImageBrush Stretch=\"Fill\" ImageSource=\"$clockPointerOver\" />",
        L"Background@CheckedPointerOver:=<ImageBrush Stretch=\"Fill\" ImageSource=\"$clockPointerOver\" />",
        L"Background@CheckedPressed:=<ImageBrush Stretch=\"Fill\" ImageSource=\"$clockPressed\" />"}},
    ThemeTargetStyles{L"SystemTray.AdaptiveTextBlock > Windows.UI.Xaml.Controls.TextBlock", {
        L"Foreground=White"}},
    ThemeTargetStyles{L"SystemTray.NotifyIconView#NotifyItemIcon > Windows.UI.Xaml.Controls.Grid#ContainerGrid@ > Windows.UI.Xaml.Controls.Border#BackgroundBorder", {
        L"Background@Normal=Transparent",
        L"Background@PointerOver:=<ImageBrush Stretch=\"Fill\" ImageSource=\"$trayPointerOver\" />",
        L"BorderThickness=0",
        L"Margin=0",
        L"Width=24"}},
    ThemeTargetStyles{L"SystemTray.NotificationAreaIcons > Windows.UI.Xaml.Controls.ItemsPresenter > Windows.UI.Xaml.Controls.StackPanel > Windows.UI.Xaml.Controls.ContentPresenter", {
        L"Width=24",
        L"Padding=-2,0,-2,0"}},
    ThemeTargetStyles{L"SystemTray.OmniButton#NotificationCenterButton > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.ContentPresenter > Windows.UI.Xaml.Controls.ItemsPresenter > Windows.UI.Xaml.Controls.StackPanel > Windows.UI.Xaml.Controls.ContentPresenter > SystemTray.IconView > Windows.UI.Xaml.Controls.Grid#ContainerGrid > Windows.UI.Xaml.Controls.Grid#ContentGrid > SystemTray.TextIconContent > Windows.UI.Xaml.Controls.Grid#ContainerGrid > SystemTray.AdaptiveTextBlock[2] > Windows.UI.Xaml.Controls.TextBlock", {
        L"FontFamily=Segoe MDL2 Assets",
        L"Text=\uE81B"}},
    ThemeTargetStyles{L"SystemTray.OmniButton#NotificationCenterButton > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.ContentPresenter > Windows.UI.Xaml.Controls.ItemsPresenter > Windows.UI.Xaml.Controls.StackPanel", {
        L"FlowDirection=1"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#ContainerGrid@ > Windows.UI.Xaml.Shapes.Rectangle#ShowDesktopPipe", {
        L"Fill@Normal:=<ImageBrush Stretch=\"Fill\" ImageSource=\"$desktopNormal\" />",
        L"Height=39",
        L"Width=$desktopWidth",
        L"RadiusX=0",
        L"RadiusY=0",
        L"Fill@PointerOver:=<ImageBrush Stretch=\"Fill\" ImageSource=\"$desktopPointerOver\" />",
        L"Fill@Pressed:=<ImageBrush Stretch=\"Fill\" ImageSource=\"$desktopPressed\" />"}},
    ThemeTargetStyles{L"SystemTray.Stack#ShowDesktopStack", {
        L"Width=$desktopWidth"}},
    ThemeTargetStyles{L"SystemTray.Stack#ShowDesktopStack > Windows.UI.Xaml.Controls.Grid > SystemTray.StackListView > Windows.UI.Xaml.Controls.ItemsPresenter > Windows.UI.Xaml.Controls.StackPanel > Windows.UI.Xaml.Controls.ContentPresenter", {
        L"Width=$desktopWidth"}},
    ThemeTargetStyles{L"SystemTray.Stack#ShowDesktopStack > Windows.UI.Xaml.Controls.Grid > SystemTray.StackListView > Windows.UI.Xaml.Controls.ItemsPresenter > Windows.UI.Xaml.Controls.StackPanel > Windows.UI.Xaml.Controls.ContentPresenter > SystemTray.IconView", {
        L"Width=$desktopWidth"}},
    ThemeTargetStyles{L"SystemTray.OmniButton#ControlCenterButton > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.ContentPresenter > Windows.UI.Xaml.Controls.ItemsPresenter > Windows.UI.Xaml.Controls.StackPanel > Windows.UI.Xaml.Controls.ContentPresenter > SystemTray.IconView#SystemTrayIcon > Windows.UI.Xaml.Controls.Grid#ContainerGrid", {
        L"Padding=0"}},
    ThemeTargetStyles{L"SystemTray.OmniButton#ControlCenterButton", {
        L"Margin=3,0,0,0"}},
    ThemeTargetStyles{L"SystemTray.Stack#MainStack > Windows.UI.Xaml.Controls.Grid > SystemTray.StackListView > Windows.UI.Xaml.Controls.ItemsPresenter > Windows.UI.Xaml.Controls.StackPanel > Windows.UI.Xaml.Controls.ContentPresenter > SystemTray.IconView > Windows.UI.Xaml.Controls.Grid#ContainerGrid@ > Windows.UI.Xaml.Controls.Border#BackgroundBorder", {
        L"Background@Normal=Transparent",
        L"Background@PointerOver:=<ImageBrush Stretch=\"Fill\" ImageSource=\"$trayPointerOver\" />",
        L"Background@Pressed:=<ImageBrush Stretch=\"Fill\" ImageSource=\"$trayPressed\" />",
        L"BorderThickness=0",
        L"Margin=0",
        L"Background@Checked:=<ImageBrush Stretch=\"Fill\" ImageSource=\"$trayPointerOver\" />",
        L"Background@CheckedPointerOver:=<ImageBrush Stretch=\"Fill\" ImageSource=\"$trayPointerOver\" />",
        L"Background@CheckedPressed:=<ImageBrush Stretch=\"Fill\" ImageSource=\"$trayPressed\" />",
        L"Width=24"}},
    ThemeTargetStyles{L"Taskbar.TaskListLabeledButtonPanel#IconPanel@RunningIndicatorStates > Windows.UI.Xaml.Shapes.Rectangle#DefaultIcon", {
        L"Visibility=Collapsed",
        L"Visibility@NoRunningIndicator=Visible"}},
    ThemeTargetStyles{L"Taskbar.TaskListLabeledButtonPanel#IconPanel@CommonStates > Windows.UI.Xaml.Shapes.Rectangle#DefaultIcon", {
        L"Fill=Transparent",
        L"Width=54",
        L"Height=54",
        L"Fill@InactivePointerOver:=<ImageBrush Stretch=\"Uniform\" ImageSource=\"$taskbandNotRunningPointerOver\" />",
        L"Fill@InactivePressed:=<ImageBrush Stretch=\"Uniform\" ImageSource=\"$taskbandNotRunningPressed\" />",
        L"Transform3D:=<CompositeTransform3D ScaleY=\"1.1\" ScaleX=\"1.04\" TranslateY=\"1\" CenterY=\"27\" />"}},
    ThemeTargetStyles{L"SystemTray.AdaptiveTextBlock#LanguageInnerTextBlock > TextBlock#InnerTextBlock", {
        L"FontFamily=Segoe UI",
        L"Typography.StylisticSet1=true"}},
    ThemeTargetStyles{L"Border#SearchPillBackgroundElement", {
        L"BorderBrush=#4F222222",
        L"BorderThickness=1"}},
    ThemeTargetStyles{L"Taskbar.ExperienceToggleButton#LaunchListButton[AutomationProperties.AutomationId=TaskViewButton] > Taskbar.TaskListButtonPanel@CommonStates > Border#BackgroundElement", {
        L"BorderBrush@InactivePointerOver:=<ImageBrush Stretch=\"Uniform\" ImageSource=\"$taskbandNotRunningPointerOver\" />",
        L"BorderThickness=2",
        L"Background:=<ImageBrush Stretch=\"None\" ImageSource=\"$taskviewIcon\" />",
        L"BorderBrush@InactivePressed:=<ImageBrush Stretch=\"Uniform\" ImageSource=\"$taskbandNotRunningPressed\" />",
        L"BorderBrush@ActivePressed:=<ImageBrush Stretch=\"Uniform\" ImageSource=\"$taskbandNotRunningPressed\" />",
        L"BorderBrush@ActivePointerOver:=<ImageBrush Stretch=\"Uniform\" ImageSource=\"$taskbandNotRunningPointerOver\" />",
        L"BorderBrush@ActiveNormal=Transparent"}},
    ThemeTargetStyles{L"Taskbar.ExperienceToggleButton#LaunchListButton[AutomationProperties.AutomationId=TaskViewButton] > Taskbar.TaskListButtonPanel > Microsoft.UI.Xaml.Controls.AnimatedVisualPlayer", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Taskbar.SearchBoxButton#SearchBoxButton[AutomationProperties.AutomationId=SearchButton] > Taskbar.TaskListButtonPanel@CommonStates > Windows.UI.Xaml.Controls.Border#BackgroundElement", {
        L"BorderBrush@InactivePointerOver_SearchIcon:=<ImageBrush Stretch=\"Uniform\" ImageSource=\"$taskbandNotRunningPointerOver\" />",
        L"BorderBrush@InactivePressed_SearchIcon:=<ImageBrush Stretch=\"Uniform\" ImageSource=\"$taskbandNotRunningPressed\" />",
        L"BorderBrush@ActivePressed_SearchIcon:=<ImageBrush Stretch=\"Uniform\" ImageSource=\"$taskbandNotRunningPressed\" />",
        L"BorderBrush@ActivePointerOver_SearchIcon:=<ImageBrush Stretch=\"Uniform\" ImageSource=\"$taskbandNotRunningPointerOver\" />",
        L"BorderBrush@ActiveNormal_SearchIcon=Transparent",
        L"BorderThickness@InactivePointerOver_SearchIcon=2",
        L"BorderThickness@InactivePressed_SearchIcon=2",
        L"BorderThickness@ActivePressed_SearchIcon=2",
        L"BorderThickness@ActivePointerOver_SearchIcon=2",
        L"Background@ActiveNormal_SearchIcon:=<ImageBrush Stretch=\"None\" ImageSource=\"$searchIcon\" />",
        L"Background@InactivePointerOver_SearchIcon:=<ImageBrush Stretch=\"None\" ImageSource=\"$searchIcon\" />",
        L"Background@InactivePressed_SearchIcon:=<ImageBrush Stretch=\"None\" ImageSource=\"$searchIcon\" />",
        L"Background@ActivePressed_SearchIcon:=<ImageBrush Stretch=\"None\" ImageSource=\"$searchIcon\" />",
        L"Background@ActivePointerOver_SearchIcon:=<ImageBrush Stretch=\"None\" ImageSource=\"$searchIcon\" />",
        L"Background@InactiveNormal_SearchIcon:=<ImageBrush Stretch=\"None\" ImageSource=\"$searchIcon\" />",
        L"Height=30",
        L"Height@ActiveNormal_SearchIcon=Auto",
        L"Height@InactivePointerOver_SearchIcon=Auto",
        L"Height@InactivePressed_SearchIcon=Auto",
        L"Height@ActivePressed_SearchIcon=Auto",
        L"Height@ActivePointerOver_SearchIcon=Auto",
        L"Height@InactiveNormal_SearchIcon=Auto"}},
    ThemeTargetStyles{L"Taskbar.SearchBoxButton#SearchBoxButton[AutomationProperties.AutomationId=SearchButton] > Taskbar.TaskListButtonPanel@CommonStates > Microsoft.UI.Xaml.Controls.AnimatedVisualPlayer", {
        L"Visibility@ActiveNormal_SearchIcon=Collapsed",
        L"Visibility@InactivePointerOver_SearchIcon=Collapsed",
        L"Visibility@InactivePressed_SearchIcon=Collapsed",
        L"Visibility@ActivePressed_SearchIcon=Collapsed",
        L"Visibility@ActivePointerOver_SearchIcon=Collapsed",
        L"Visibility@InactiveNormal_SearchIcon=Collapsed"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.StackPanel > Windows.UI.Xaml.Controls.ContentPresenter > SystemTray.IconView > Windows.UI.Xaml.Controls.Grid@ > Windows.UI.Xaml.Controls.Border#BackgroundBorder", {
        L"Background@CheckedPressed:=<ImageBrush Stretch=\"Fill\" ImageSource=\"$clockPressed\" />",
        L"Background@CheckedPointerOver:=<ImageBrush Stretch=\"Fill\" ImageSource=\"$clockPointerOver\" />",
        L"Background@CheckedNormal:=<ImageBrush Stretch=\"Fill\" ImageSource=\"$clockPointerOver\" />",
        L"BorderThickness=0",
        L"Background@Pressed:=<ImageBrush Stretch=\"Fill\" ImageSource=\"$clockPressed\" />",
        L"Background@PointerOver:=<ImageBrush Stretch=\"Fill\" ImageSource=\"$clockPointerOver\" />",
        L"Background@Normal=Transparent",
        L"Margin=0"}},
    ThemeTargetStyles{L"SystemTray.Stack#NonActivatableStack", {
        L"Margin=4,0,0,0"}},
    ThemeTargetStyles{L"Taskbar.AugmentedEntryPointButton[AutomationProperties.AutomationId=WidgetsButton] > Taskbar.TaskListButtonPanel@CommonStates > Windows.UI.Xaml.Controls.Border#BackgroundElement", {
        L"Background@InactivePointerOver:=<ImageBrush Stretch=\"Fill\" ImageSource=\"$widgetsPointerOver\" />",
        L"Background@InactivePressed:=<ImageBrush Stretch=\"Fill\" ImageSource=\"$widgetsPressed\" />",
        L"Background@ActivePointerOver:=<ImageBrush Stretch=\"Fill\" ImageSource=\"$widgetsPointerOver\" />",
        L"Background@ActivePressed:=<ImageBrush Stretch=\"Fill\" ImageSource=\"$widgetsPressed\" />",
        L"Background@ActiveNormal:=<ImageBrush Stretch=\"Fill\" ImageSource=\"$widgetsPointerOver\" />",
        L"BorderThickness=0",
        L"Margin=0"}},
    ThemeTargetStyles{L"Taskbar.ExperienceToggleButton#LaunchListButton[AutomationProperties.AutomationId=StartButton] > Taskbar.TaskListButtonPanel#ExperienceToggleButtonRootPanel", {
        L"Width=54"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.ToolTip > Windows.UI.Xaml.Controls.ContentPresenter > Windows.UI.Xaml.Controls.StackPanel > Windows.UI.Xaml.Controls.TextBlock", {
        L"FontFamily=Segoe UI",
        L"Typography.StylisticSet1=true"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.TextBlock#LabelControl", {
        L"FontFamily=Segoe UI",
        L"Typography.StylisticSet1=true",
        L"Foreground=White"}},
}, {
    L"orbNormal=https://raw.githubusercontent.com/ramensoftware/windows-11-taskbar-styling-guide/refs/heads/main/Themes/Windows7/ThemeResources/orbNormal.png",
    L"orbPointerOver=https://raw.githubusercontent.com/ramensoftware/windows-11-taskbar-styling-guide/refs/heads/main/Themes/Windows7/ThemeResources/orbHover.png",
    L"orbPressed=https://raw.githubusercontent.com/ramensoftware/windows-11-taskbar-styling-guide/refs/heads/main/Themes/Windows7/ThemeResources/orbPressed.png",
    L"aeroColor={ThemeResource SystemAccentColor}",
    L"aeroOpacity=0.3",
    L"reflection=https://raw.githubusercontent.com/ramensoftware/windows-11-taskbar-styling-guide/refs/heads/main/Themes/Windows7/ThemeResources/AeroPeek.png",
    L"taskbandInactiveNormal=https://raw.githubusercontent.com/ramensoftware/windows-11-taskbar-styling-guide/refs/heads/main/Themes/Windows7/ThemeResources/InactiveNormal.png",
    L"taskbandInactivePointerOver=https://raw.githubusercontent.com/ramensoftware/windows-11-taskbar-styling-guide/refs/heads/main/Themes/Windows7/ThemeResources/InactivePointerOver.png",
    L"taskbandInactivePressed=https://raw.githubusercontent.com/ramensoftware/windows-11-taskbar-styling-guide/refs/heads/main/Themes/Windows7/ThemeResources/ActiveNormal.png",
    L"taskbandActiveNormal=https://raw.githubusercontent.com/ramensoftware/windows-11-taskbar-styling-guide/refs/heads/main/Themes/Windows7/ThemeResources/ActiveNormal.png",
    L"taskbandActivePointerOver=https://raw.githubusercontent.com/ramensoftware/windows-11-taskbar-styling-guide/refs/heads/main/Themes/Windows7/ThemeResources/ActiveNormal.png",
    L"taskbandActivePressed=https://raw.githubusercontent.com/ramensoftware/windows-11-taskbar-styling-guide/refs/heads/main/Themes/Windows7/ThemeResources/ActiveNormal.png",
    L"overflowNormal=https://raw.githubusercontent.com/ramensoftware/windows-11-taskbar-styling-guide/refs/heads/main/Themes/Windows7/ThemeResources/overflowNormal.png",
    L"overflowPointerOver=https://raw.githubusercontent.com/ramensoftware/windows-11-taskbar-styling-guide/refs/heads/main/Themes/Windows7/ThemeResources/overflowPointerOver.png",
    L"overflowPressed=https://raw.githubusercontent.com/ramensoftware/windows-11-taskbar-styling-guide/refs/heads/main/Themes/Windows7/ThemeResources/overflowPressed.png",
    L"clockPointerOver=https://raw.githubusercontent.com/ramensoftware/windows-11-taskbar-styling-guide/refs/heads/main/Themes/Windows7/ThemeResources/clockPointerOver.png",
    L"clockPressed=https://raw.githubusercontent.com/ramensoftware/windows-11-taskbar-styling-guide/refs/heads/main/Themes/Windows7/ThemeResources/clockPressed.png",
    L"trayPointerOver=https://raw.githubusercontent.com/ramensoftware/windows-11-taskbar-styling-guide/refs/heads/main/Themes/Windows7/ThemeResources/trayPointerOver.png",
    L"trayPressed=https://raw.githubusercontent.com/ramensoftware/windows-11-taskbar-styling-guide/refs/heads/main/Themes/Windows7/ThemeResources/trayPressed.png",
    L"desktopNormal=https://raw.githubusercontent.com/ramensoftware/windows-11-taskbar-styling-guide/refs/heads/main/Themes/Windows7/ThemeResources/desktopNormal.png",
    L"desktopPointerOver=https://raw.githubusercontent.com/ramensoftware/windows-11-taskbar-styling-guide/refs/heads/main/Themes/Windows7/ThemeResources/desktopPointerOver.png",
    L"desktopPressed=https://raw.githubusercontent.com/ramensoftware/windows-11-taskbar-styling-guide/refs/heads/main/Themes/Windows7/ThemeResources/desktopPressed.png",
    L"desktopWidth=15",
    L"taskbandRequestingAttention=https://raw.githubusercontent.com/ramensoftware/windows-11-taskbar-styling-guide/refs/heads/main/Themes/Windows7/ThemeResources/requestingAttention.png",
    L"taskbandNotRunningPointerOver=https://raw.githubusercontent.com/ramensoftware/windows-11-taskbar-styling-guide/refs/heads/main/Themes/Windows7/ThemeResources/notRunningPointerOver.png",
    L"taskbandNotRunningPressed=https://raw.githubusercontent.com/ramensoftware/windows-11-taskbar-styling-guide/refs/heads/main/Themes/Windows7/ThemeResources/notRunningPressed.png",
    L"taskbarBackground=https://raw.githubusercontent.com/ramensoftware/windows-11-taskbar-styling-guide/refs/heads/main/Themes/Windows7/ThemeResources/taskbarBackground.png",
    L"taskviewIcon=https://raw.githubusercontent.com/ramensoftware/windows-11-taskbar-styling-guide/refs/heads/main/Themes/Windows7/ThemeResources/taskview.png",
    L"searchIcon=https://raw.githubusercontent.com/ramensoftware/windows-11-taskbar-styling-guide/refs/heads/main/Themes/Windows7/ThemeResources/search.png",
    L"widgetsPointerOver=https://raw.githubusercontent.com/ramensoftware/windows-11-taskbar-styling-guide/refs/heads/main/Themes/Windows7/ThemeResources/widgetsPointerOver.png",
    L"widgetsPressed=https://raw.githubusercontent.com/ramensoftware/windows-11-taskbar-styling-guide/refs/heads/main/Themes/Windows7/ThemeResources/widgetsPressed.png",
}};

const Theme g_themeAeris = {{
    ThemeTargetStyles{L"Taskbar.TaskbarFrame > Grid#RootGrid", {
        L"Background:=$taskbarBackground"}},
    ThemeTargetStyles{L"Taskbar.TaskbarFrame > Grid#RootGrid > Taskbar.TaskbarBackground > Grid", {
        L"Background:=<WindhawkBlur BlurAmount=\"$taskbarBlurIncreace\" TintColor=\"#00000000\" />"}},
    ThemeTargetStyles{L"Taskbar.TaskbarFrame > Grid#RootGrid > Taskbar.TaskbarBackground > Grid > Rectangle#BackgroundFill", {
        L"Fill:=<SolidColorBrush Color=\"$themeColor\" Opacity=\"$themeColorOpacity\"/>"}},
    ThemeTargetStyles{L"Taskbar.TaskbarFrame > Grid#RootGrid > Taskbar.TaskbarBackground > Grid > Rectangle#BackgroundStroke", {
        L"Visibility=Collapsed",
        L"Fill:=<SolidColorBrush Color=\"$primaryColor\" Opacity=\"0.05\"/>"}},
    ThemeTargetStyles{L"Taskbar.TaskListButtonPanel@CommonStates", {
        L"Padding=0",
        L"Margin=$taskListMargin,0,$taskListMargin,0",
        L"Background@ActiveNormal:=<SolidColorBrush Color=\"$activeColor\" Opacity=\"0.5\"/>",
        L"Background@ActivePointerOver:=<SolidColorBrush Color=\"$activeColor\" Opacity=\"0.5\"/>",
        L"Background@ActivePressed:=<SolidColorBrush Color=\"$activeColor\" Opacity=\"0.5\"/>"}},
    ThemeTargetStyles{L"Taskbar.TaskListButtonPanel@CommonStates > Border#BackgroundElement", {
        L"Background:=$transparent",
        L"Background@InactivePointerOver:=$pointerOver",
        L"Background@InactivePressed:=$pressed",
        L"Background@ActivePointerOver:=$pointerOver",
        L"Background@ActivePressed:=$pressed",
        L"BorderThickness=0",
        L"CornerRadius=0",
        L"Margin=0"}},
    ThemeTargetStyles{L"Taskbar.TaskListButton > Taskbar.TaskListLabeledButtonPanel@RunningIndicatorStates", {
        L"Padding=0",
        L"Margin=$taskListMargin,0,$taskListMargin,0",
        L"Background@NoRunningIndicator:=$transparent",
        L"Background@InactiveRunningIndicator:=<SolidColorBrush Color=\"$primaryColor\" Opacity=\"0.1\"/>",
        L"Background@ActiveRunningIndicator:=<SolidColorBrush Color=\"$activeColor\" Opacity=\"0.5\"/>",
        L"Background:=<SolidColorBrush Color=\"$requestAttentionColor\" Opacity=\"0.5\"/>"}},
    ThemeTargetStyles{L"Taskbar.TaskListButton > Taskbar.TaskListLabeledButtonPanel@CommonStates > Border#BackgroundElement", {
        L"Background:=$transparent",
        L"Background@InactivePointerOver:=$pointerOver",
        L"Background@InactivePressed:=$pressed",
        L"Background@ActivePointerOver:=$pointerOver",
        L"Background@ActivePressed:=$pressed",
        L"Background@MultiWindowPointerOver:=$pointerOver",
        L"Background@MultiWindowPressed:=$pressed",
        L"Background@RequestingAttentionPointerOver:=$pointerOver",
        L"Background@RequestingAttentionPressed:=$pressed",
        L"Background@RequestingAttentionMultiPointerOver:=$pointerOver",
        L"Background@RequestingAttentionMultiPressed:=$pressed",
        L"BorderThickness=0",
        L"CornerRadius=0",
        L"Margin=0"}},
    ThemeTargetStyles{L"Taskbar.TaskListLabeledButtonPanel > Border#MultiWindowElement", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Taskbar.TaskListLabeledButtonPanel@CommonStates > Rectangle#RunningIndicator", {
        L"Visibility=Collapsed",
        L"Fill:=<SolidColorBrush Color=\"$primaryColor\" Opacity=\"0.2\"/>",
        L"VerticalAlignment=0",
        L"HorizontalAlignment=Stretch",
        L"Margin=0,0,-4,0",
        L"Width=Auto",
        L"Height=3",
        L"RadiusX=0",
        L"RadiusY=0",
        L"Visibility@MultiWindowNormal=Visible",
        L"Visibility@MultiWindowActive=Visible",
        L"Visibility@MultiWindowPointerOver=Visible",
        L"Visibility@MultiWindowPressed=Visible",
        L"Visibility@RequestingAttentionMulti=Visible",
        L"Visibility@RequestingAttentionMultiPointerOver=Visible",
        L"Visibility@RequestingAttentionMultiPressed=Visible"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.ProgressBar#ProgressIndicator", {
        L"VerticalAlignment=Stretch",
        L"HorizontalAlignment=Stretch",
        L"Margin=0,0,-4,0",
        L"Width=Auto",
        L"Height=Auto",
        L"CornerRadius=0"}},
    ThemeTargetStyles{L"Border#ProgressBarRoot > Border > Grid", {
        L"Height=Auto"}},
    ThemeTargetStyles{L"Grid#LayoutRoot@CommonStates > Border#ProgressBarRoot > Border > Grid > Rectangle#ProgressBarTrack", {
        L"Margin=0",
        L"RadiusX=0",
        L"RadiusY=0",
        L"Fill:=<SolidColorBrush Color=\"$progressColor\" Opacity=\"0.15\"/>",
        L"Fill@Paused:=<SolidColorBrush Color=\"$progressPausedColor\" Opacity=\"0.15\"/>"}},
    ThemeTargetStyles{L"Grid#LayoutRoot@CommonStates > Border#ProgressBarRoot > Border > Grid > Rectangle#DeterminateProgressBarIndicator", {
        L"RadiusX=0",
        L"RadiusY=0",
        L"Fill:=<SolidColorBrush Color=\"$progressColor\" Opacity=\"0.4\"/>",
        L"Fill@Paused:=<SolidColorBrush Color=\"$progressPausedColor\" Opacity=\"0.4\"/>"}},
    ThemeTargetStyles{L"Taskbar.TaskListLabeledButtonPanel > Image", {
        L"Transform3D:=<CompositeTransform3D TranslateX=\"2\" TranslateY=\"1\" />"}},
    ThemeTargetStyles{L"Taskbar.TaskListLabeledButtonPanel > Rectangle#DefaultIcon", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Taskbar.TaskListButtonPanel > AnimatedVisualPlayer", {
        L"Transform3D:=<CompositeTransform3D TranslateX=\"0\" TranslateY=\"1\" />"}},
    ThemeTargetStyles{L"Taskbar.ExperienceToggleButton#LaunchListButton[AutomationProperties.AutomationId=StartButton] > Taskbar.TaskListButtonPanel", {
        L"MinWidth=60",
        L"Margin=0,0,$taskListMargin,0"}},
    ThemeTargetStyles{L"Taskbar.SearchBoxButton > Taskbar.TaskListButtonPanel@CommonStates", {
        L"Padding=0",
        L"Margin=$taskListMargin,0,$taskListMargin,0",
        L"Background@ActiveNormal_SearchIcon:=<SolidColorBrush Color=\"$activeColor\" Opacity=\"0.5\"/>",
        L"Background@ActivePointerOver_SearchIcon:=<SolidColorBrush Color=\"$activeColor\" Opacity=\"0.5\"/>",
        L"Background@ActivePressed_SearchIcon:=<SolidColorBrush Color=\"$activeColor\" Opacity=\"0.5\"/>"}},
    ThemeTargetStyles{L"Taskbar.SearchBoxLaunchListButton > Taskbar.TaskListButtonPanel", {
        L"Padding=0",
        L"Margin=$taskListMargin,0,$taskListMargin,0"}},
    ThemeTargetStyles{L"SearchUx.SearchUI.SearchButtonRootGrid@CommonStates", {
        L"Padding=0",
        L"Margin=$taskListMargin,0,$taskListMargin,0",
        L"Background@ActiveNormal:=<SolidColorBrush Color=\"$activeColor\" Opacity=\"0.5\"/>",
        L"Background@ActivePointerOver:=<SolidColorBrush Color=\"$activeColor\" Opacity=\"0.5\"/>",
        L"Background@ActivePressed:=<SolidColorBrush Color=\"$activeColor\" Opacity=\"0.5\"/>"}},
    ThemeTargetStyles{L"Taskbar.SearchBoxButton > Taskbar.TaskListButtonPanel@CommonStates > Border#BackgroundElement", {
        L"BorderThickness=0",
        L"CornerRadius=0",
        L"Margin=0",
        L"Background@InactiveNormal_SearchIcon:=$transparent",
        L"Background@InactivePointerOver_SearchIcon:=$pointerOver",
        L"Background@InactivePressed_SearchIcon:=$pressed",
        L"Background@ActiveNormal_SearchIcon:=$transparent",
        L"Background@ActivePointerOver_SearchIcon:=$pointerOver",
        L"Background@ActivePressed_SearchIcon:=$pressed"}},
    ThemeTargetStyles{L"Taskbar.SearchBoxLaunchListButton > Taskbar.TaskListButtonPanel > Border#BackgroundElement", {
        L"BorderThickness=0",
        L"CornerRadius=0",
        L"Margin=0"}},
    ThemeTargetStyles{L"SearchUx.SearchUI.SearchButtonRootGrid@CommonStates > Border#BackgroundElement", {
        L"BorderThickness=0",
        L"CornerRadius=0",
        L"Margin=0",
        L"Background@InactiveNormal:=$transparent",
        L"Background@InactivePointerOver:=$pointerOver",
        L"Background@InactivePressed:=$pressed",
        L"Background@ActiveNormal:=$transparent",
        L"Background@ActivePointerOver:=$pointerOver",
        L"Background@ActivePressed:=$pressed"}},
    ThemeTargetStyles{L"Border#SearchPillBackgroundElement", {
        L"BorderThickness=0",
        L"CornerRadius=0",
        L"Margin=0"}},
    ThemeTargetStyles{L"Grid#DynamicSearchBoxGleamContainer", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Grid@ > Border#BackgroundBorder", {
        L"Padding=0",
        L"CornerRadius=0",
        L"Margin=0",
        L"BorderThickness=0",
        L"Background@PointerOver:=$pointerOver",
        L"Background@Pressed:=$pressed"}},
    ThemeTargetStyles{L"SystemTray.OmniButton > Grid@CommonStates > Border#BackgroundBorder", {
        L"Background@PointerOver:=$pointerOver",
        L"Background@Pressed:=$pressed"}},
    ThemeTargetStyles{L"SystemTray.Stack#ShowDesktopStack", {
        L"MinWidth=$showDesktopWidth",
        L"MaxWidth=$showDesktopWidth"}},
    ThemeTargetStyles{L"SystemTray.Stack#ShowDesktopStack > Grid > SystemTray.StackListView > Windows.UI.Xaml.Controls.ItemsPresenter > Windows.UI.Xaml.Controls.StackPanel > Windows.UI.Xaml.Controls.ContentPresenter > SystemTray.IconView", {
        L"MinWidth=$showDesktopWidth",
        L"MaxWidth=$showDesktopWidth"}},
    ThemeTargetStyles{L"SystemTray.IconView#SystemTrayIcon > Grid#ContainerGrid@ > Rectangle#ShowDesktopPipe", {
        L"VerticalAlignment=Stretch",
        L"HorizontalAlignment=Stretch",
        L"Height=Auto",
        L"Width=Auto",
        L"Fill@PointerOver:=$pointerOver",
        L"Fill@Pressed:=$pressed"}},
}, {
    L"themeColor={ThemeResource SystemAccentColorDark1}",
    L"themeColorOpacity=0",
    L"primaryColor={ThemeResource TextFillColorPrimary}",
    L"activeColor=#33AAFF",
    L"requestAttentionColor=#FF7788",
    L"progressColor=#44CC66",
    L"progressPausedColor=#EECC44",
    L"taskbarBackground=<AcrylicBrush TintColor=\"{ThemeResource CardStrokeColorDefaultSolid}\" FallbackColor=\"{ThemeResource CardStrokeColorDefaultSolid}\" TintOpacity=\"0\" TintLuminosityOpacity=\"0.8\" Opacity=\"1\"/>",
    L"taskbarBlurIncreace=0",
    L"taskListMargin=2",
    L"showDesktopWidth=10",
    L"transparent=<SolidColorBrush Color=\"{ThemeResource TextFillColorPrimary}\" Opacity=\"0\"/>",
    L"pointerOver=<SolidColorBrush Color=\"{ThemeResource TextFillColorPrimary}\" Opacity=\"0.075\"/>",
    L"pressed=<SolidColorBrush Color=\"{ThemeResource TextFillColorPrimary}\" Opacity=\"0.05\"/>",
}};

const Theme g_themePlasma = {{
    ThemeTargetStyles{L"Taskbar.TaskListButton > Taskbar.TaskListLabeledButtonPanel", {
        L"Padding=0"}},
    ThemeTargetStyles{L"Taskbar.TaskListButton > Taskbar.TaskListLabeledButtonPanel@CommonStates > Windows.UI.Xaml.Controls.Border#BackgroundElement", {
        L"CornerRadius=0",
        L"BorderThickness=1,0,1,0",
        L"BorderBrush=Transparent",
        L"Background@ActiveNormal:=$taskbandActive",
        L"Background@InactiveNormal:=$taskbandInactiveNormal",
        L"Background@ActivePointerOver:=$taskbandPointerOver",
        L"Background@MultiWindowNormal:=$taskbandInactiveNormal",
        L"Background@MultiWindowPointerOver:=$taskbandPointerOver",
        L"Background@InactivePointerOver:=$taskbandPointerOver",
        L"Background@ActivePressed:=$taskbandPointerOver",
        L"Background@InactivePressed:=$taskbandPointerOver",
        L"Margin=0",
        L"Background@MultiWindowPressed:=$taskbandPointerOver",
        L"Background@MultiWindowActive:=$taskbandActive",
        L"Background@RequestingAttention:=$taskbandAttention",
        L"Background@RequestingAttentionPointerOver:=$taskbandPointerOver",
        L"Background@RequestingAttentionPressed:=$taskbandPointerOver",
        L"Background@RequestingAttentionMulti:=$taskbandAttention",
        L"Background@RequestingAttentionMultiPointerOver:=$taskbandPointerOver",
        L"Background@RequestingAttentionMultiPressed:=$taskbandPointerOver"}},
    ThemeTargetStyles{L"Taskbar.TaskListButton > Taskbar.TaskListLabeledButtonPanel@RunningIndicatorStates > Windows.UI.Xaml.Controls.Border#BackgroundElement", {
        L"Opacity@NoRunningIndicator=0"}},
    ThemeTargetStyles{L"Taskbar.TaskListLabeledButtonPanel#IconPanel > Windows.UI.Xaml.Controls.Image#Icon", {
        L"RenderTransform:=<TranslateTransform X=\"2\" />"}},
    ThemeTargetStyles{L"Taskbar.TaskListLabeledButtonPanel@CommonStates > Windows.UI.Xaml.Shapes.Rectangle#RunningIndicator", {
        L"Width=50",
        L"RadiusX=0",
        L"RadiusY=0",
        L"Height=3",
        L"VerticalAlignment=Top",
        L"RenderTransform:=<TranslateTransform X=\"2\" />",
        L"Margin=-1,0,-1,0",
        L"Fill@ActiveNormal:=$indicatorActive",
        L"Fill@ActivePointerOver:=$indicatorPointerOver",
        L"Fill:=$indicatorInactive",
        L"Fill@InactivePointerOver:=$indicatorPointerOver",
        L"Fill@ActivePressed:=$indicatorPointerOver",
        L"Fill@InactivePressed:=$indicatorPointerOver",
        L"Fill@MultiWindowNormal:=$indicatorInactive",
        L"Fill@MultiWindowPointerOver:=$indicatorPointerOver",
        L"Fill@MultiWindowPressed:=$indicatorPointerOver",
        L"Fill@MultiWindowActive:=$indicatorActive",
        L"Fill@RequestingAttention:=$indicatorAttention",
        L"Fill@RequestingAttentionPointerOver:=$indicatorPointerOver",
        L"Fill@RequestingAttentionPressed:=$indicatorPointerOver",
        L"Fill@RequestingAttentionMulti:=$indicatorAttention",
        L"Fill@RequestingAttentionMultiPointerOver:=$indicatorPointerOver",
        L"Fill@RequestingAttentionMultiPressed:=$indicatorPointerOver"}},
    ThemeTargetStyles{L"Rectangle#BackgroundStroke", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Taskbar.TaskbarFrame > Grid#RootGrid > Taskbar.TaskbarBackground > Grid > Rectangle#BackgroundFill", {
        L"Fill:=$WindhawkBlur"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Border#MultiWindowElement", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Taskbar.TaskListLabeledButtonPanel@CommonStates > Windows.UI.Xaml.Shapes.Rectangle#DefaultIcon", {
        L"Fill:=<ImageBrush Stretch=\"Uniform\" ImageSource=\"$plusIndicator\" />",
        L"Width=11",
        L"Height=11",
        L"RadiusX=0",
        L"RadiusY=0",
        L"VerticalAlignment=Bottom",
        L"RenderTransform:=<TranslateTransform X=\"1\" />",
        L"Visibility@MultiWindowNormal=Visible",
        L"Visibility@MultiWindowActive=Visible",
        L"Visibility@MultiWindowPointerOver=Visible",
        L"HorizontalAlignment=Center",
        L"Visibility@MultiWindowPressed=Visible",
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Taskbar.TaskListButtonPanel#ExperienceToggleButtonRootPanel", {
        L"Padding=0",
        L"Width=50"}},
    ThemeTargetStyles{L"Taskbar.TaskListButtonPanel#ExperienceToggleButtonRootPanel@CommonStates > Windows.UI.Xaml.Controls.Border#BackgroundElement", {
        L"CornerRadius=0",
        L"BorderThickness=0",
        L"Width=32",
        L"Background=Transparent"}},
    ThemeTargetStyles{L"Taskbar.ExperienceToggleButton#LaunchListButton[AutomationProperties.AutomationId=StartButton] > Taskbar.TaskListButtonPanel > Microsoft.UI.Xaml.Controls.AnimatedVisualPlayer#Icon", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Taskbar.TaskListButtonPanel#ExperienceToggleButtonRootPanel@CommonStates", {
        L"BorderThickness@ActiveNormal=0,3,0,0",
        L"Width=50",
        L"BorderBrush:=$selectionBorder",
        L"BorderThickness@ActivePointerOver=0,3,0,0",
        L"BorderThickness@ActivePressed=0,3,0,0"}},
    ThemeTargetStyles{L"Taskbar.ExperienceToggleButton#LaunchListButton[AutomationProperties.AutomationId=StartButton] > Taskbar.TaskListButtonPanel > Border#BackgroundElement", {
        L"Background:=<ImageBrush Stretch=\"Uniform\" ImageSource=\"https://raw.githubusercontent.com/ramensoftware/windows-11-taskbar-styling-guide/refs/heads/main/Themes/Plasma/ThemeResources/$StartButton.png\" />"}},
    ThemeTargetStyles{L"Taskbar.AugmentedEntryPointButton[AutomationProperties.AutomationId=WidgetsButton] > Taskbar.TaskListButtonPanel#ExperienceToggleButtonRootPanel", {
        L"Width=Auto"}},
    ThemeTargetStyles{L"SystemTray.Stack#NotifyIconStack", {
        L"Grid.Column=5"}},
    ThemeTargetStyles{L"SystemTray.NotificationAreaIcons#NotificationAreaIcons", {
        L"Grid.Column=0"}},
    ThemeTargetStyles{L"SystemTray.OmniButton#ControlCenterButton", {
        L"Grid.Column=1"}},
    ThemeTargetStyles{L"SystemTray.OmniButton > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.Border#BackgroundBorder", {
        L"Background=Transparent",
        L"Margin=1,0,1,0",
        L"CornerRadius=0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#ContainerGrid@ > Windows.UI.Xaml.Controls.Border#BackgroundBorder", {
        L"BorderBrush@CheckedNormal:=$selectionBorderExtended",
        L"BorderThickness@CheckedNormal:=0,3,0,0",
        L"BorderBrush@CheckedPointerOver:=$selectionBorderExtended",
        L"BorderThickness@CheckedPointerOver:=0,3,0,0",
        L"BorderBrush@CheckedPressed:=$selectionBorderExtended",
        L"BorderThickness@CheckedPressed:=0,3,0,0",
        L"BorderThickness=0"}},
    ThemeTargetStyles{L"SystemTray.OmniButton > Windows.UI.Xaml.Controls.Grid@CommonStates > Windows.UI.Xaml.Controls.Border#BackgroundBorder", {
        L"BorderBrush@Checked:=$selectionBorderExtended",
        L"BorderThickness@Checked:=0,3,0,0",
        L"BorderBrush@CheckedPointerOver:=$selectionBorderExtended",
        L"BorderThickness@CheckedPointerOver:=0,3,0,0",
        L"BorderBrush@CheckedPressed:=$selectionBorderExtended",
        L"BorderThickness@CheckedPressed:=0,3,0,0",
        L"BorderThickness=0"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.TextBlock#TimeInnerTextBlock", {
        L"FontSize=17.33",
        L"TextAlignment=Center",
        L"FontFamily=Noto Sans, Segoe UI",
        L"Foreground=White"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.TextBlock#DateInnerTextBlock", {
        L"FontSize=13.33",
        L"TextAlignment=Center",
        L"FontFamily=Noto Sans, Segoe UI",
        L"Margin=0,-5,0,0",
        L"Foreground=White"}},
    ThemeTargetStyles{L"SystemTray.DateTimeIconContent > Windows.UI.Xaml.Controls.Grid#ContainerGrid", {
        L"Padding=0"}},
    ThemeTargetStyles{L"SystemTray.ChevronIconView > Windows.UI.Xaml.Controls.Grid#ContainerGrid > Windows.UI.Xaml.Controls.ContentPresenter#ContentPresenter > Windows.UI.Xaml.Controls.Grid#ContentGrid > SystemTray.TextIconContent > Windows.UI.Xaml.Controls.Grid#ContainerGrid > SystemTray.AdaptiveTextBlock#Base > Windows.UI.Xaml.Controls.TextBlock#InnerTextBlock", {
        L"FontSize=17.33",
        L"Foreground=White"}},
    ThemeTargetStyles{L"SystemTray.ChevronIconView", {
        L"Margin=-5,0"}},
    ThemeTargetStyles{L"SystemTray.NotifyIconView#NotifyItemIcon", {
        L"MinWidth=28"}},
    ThemeTargetStyles{L"SystemTray.Stack#ShowDesktopStack", {
        L"Width=48",
        L"Height=Auto"}},
    ThemeTargetStyles{L"SystemTray.IconView[AutomationProperties.Name=Show Desktop]", {
        L"Width=48"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Shapes.Rectangle#ShowDesktopPipe", {
        L"Width=48",
        L"Height=50",
        L"Fill:=<ImageBrush Stretch=\"None\" ImageSource=\"$desktopButton\" />"}},
    ThemeTargetStyles{L"SystemTray.ChevronIconView > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.Border#BackgroundBorder", {
        L"Background=Transparent",
        L"Margin=1,0,1,0",
        L"CornerRadius=0"}},
    ThemeTargetStyles{L"SystemTray.NotifyIconView > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.Border#BackgroundBorder", {
        L"Background=Transparent",
        L"Margin=1,0,1,0",
        L"CornerRadius=0"}},
    ThemeTargetStyles{L"SystemTray.Stack#NonActivatableStack", {
        L"Grid.Column=3"}},
    ThemeTargetStyles{L"SystemTray.IconView#SystemTrayIcon > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.Border#BackgroundBorder", {
        L"Background=Transparent",
        L"Margin=1,0,1,0",
        L"CornerRadius=0"}},
    ThemeTargetStyles{L"SystemTray.StackListView[AutomationProperties.AutomationId=Main]", {
        L"Margin=-8,0,0,0"}},
    ThemeTargetStyles{L"SystemTray.AdaptiveTextBlock#LanguageInnerTextBlock > Windows.UI.Xaml.Controls.TextBlock", {
        L"FontFamily=Noto Sans, Segoe UI"}},
}, {
    L"taskbandInactiveNormal=<SolidColorBrush Color=\"Gray\" Opacity=\"0.25\" />",
    L"taskbandPointerOver=<SolidColorBrush Color=\"{ThemeResource SystemAccentColorLight1}\" Opacity=\"0.4\" />",
    L"taskbandActive=<SolidColorBrush Color=\"{ThemeResource SystemAccentColorLight1}\" Opacity=\"0.5\" />",
    L"indicatorActive=<SolidColorBrush Color=\"{ThemeResource SystemAccentColorLight2}\" Opacity=\"0.7\" />",
    L"indicatorInactive=<SolidColorBrush Color=\"Gray\" Opacity=\"0.7\" />",
    L"indicatorPointerOver=<SolidColorBrush Color=\"{ThemeResource SystemAccentColorLight2}\" Opacity=\"0.8\" />",
    L"taskbandAttention=<SolidColorBrush Color=\"#ce640c\" Opacity=\"0.5\" />",
    L"indicatorAttention=<SolidColorBrush Color=\"#ce640c\" Opacity=\"0.9\" />",
    L"selectionBorder=<LinearGradientBrush StartPoint='0,0' EndPoint='1,0'><GradientStop Color='Transparent' Offset='0.0' /><GradientStop Color='Transparent' Offset='0.2' /><GradientStop Color='{ThemeResource SystemAccentColorLight2}' Offset='0.2' /><GradientStop Color='{ThemeResource SystemAccentColorLight2}' Offset='0.8' /><GradientStop Color='Transparent' Offset='0.8' /><GradientStop Color='Transparent' Offset='1.0' /></LinearGradientBrush>",
    L"selectionBorderExtended=<SolidColorBrush Color=\"{ThemeResource SystemAccentColorLight2}\" />",
    L"desktopButton=https://raw.githubusercontent.com/ramensoftware/windows-11-taskbar-styling-guide/refs/heads/main/Themes/Plasma/ThemeResources/desktop.png",
    L"plusIndicator=https://raw.githubusercontent.com/ramensoftware/windows-11-taskbar-styling-guide/refs/heads/main/Themes/Plasma/ThemeResources/plus.png",
    L"StartButton=default",
    L"WindhawkBlur=<WindhawkBlur BlurAmount=\"30\" TintColor=\"#cc2a2e32\" />",
    L"Acrylic=<AcrylicBrush TintColor=\"#2a2e32\" TintOpacity=\"0.8\" FallbackColor=\"#2a2e32\" />",
}};

const Theme g_themeWindowGlass = {{
    ThemeTargetStyles{L"Taskbar.TaskbarFrame", {
        L"MaxWidth={{min($TaskbarFrameMaxWidth, containerGridWidth)}}",
        L"Width=Auto",
        L"MinWidth:=100",
        L"Grid.Column=1"}},
    ThemeTargetStyles{L"Taskbar.TaskbarFrame > Grid#RootGrid", {
        L"Margin=10,2,0,2",
        L"BorderThickness=0.5,1,0,1",
        L"CornerRadius=25,0,0,25",
        L"Background:=$Background",
        L"Padding=10,0,0,0"}},
    ThemeTargetStyles{L"Taskbar.TaskbarBackground#BackgroundControl > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Shapes.Rectangle#BackgroundFill", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Taskbar.TaskbarBackground#BackgroundControl > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Shapes.Rectangle#BackgroundStroke", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Taskbar.AugmentedEntryPointButton#AugmentedEntryPointButton > Taskbar.TaskListButtonPanel#ExperienceToggleButtonRootPanel", {
        L"Margin=0,4,0,4",
        L"Background:=Transparent",
        L"CornerRadius=0",
        L"BorderThickness=0,0,1,0",
        L"BorderBrush:=#20808080",
        L"Padding=2,0,5,0",
        L"MaxWidth:=200"}},
    ThemeTargetStyles{L"SystemTray.SystemTrayFrame", {
        L"Grid.Column=2",
        L"Width=Auto",
        L"HorizontalAlignment=Left",
        L"Margin=0"}},
    ThemeTargetStyles{L"Grid#SystemTrayFrameGrid", {
        L"Margin=0,2,0,2",
        L"Padding=0,0,8,0",
        L"Background:=$Background",
        L"BorderThickness=0,1,0.5,1",
        L"CornerRadius=0,25,25,0"}},
    ThemeTargetStyles{L":root > ScrollViewer > ScrollContentPresenter > Border > Grid", {
        L"ColumnDefinitions:=<ColumnDefinitionCollection><ColumnDefinition Width=\"*\"/><ColumnDefinition Width=\"Auto\"/><ColumnDefinition Width=\"Auto\"/><ColumnDefinition Width=\"*\"/></ColumnDefinitionCollection>",
        L"ActualWidth=>containerGridWidth"}},
    ThemeTargetStyles{L"SystemTray.ChevronIconView", {
        L"Padding=6,0,6,0",
        L"CornerRadius=8",
        L"Margin=2,0,0,0"}},
    ThemeTargetStyles{L"SystemTray.NotifyIconView#NotifyItemIcon", {
        L"Padding=6,0,6,0",
        L"CornerRadius=8",
        L"Margin=2,0,0,0"}},
    ThemeTargetStyles{L"SystemTray.OmniButton", {
        L"Padding=6,0,6,0",
        L"CornerRadius=8",
        L"Margin=2,0,0,0"}},
    ThemeTargetStyles{L"SystemTray.CopilotIcon", {
        L"Padding=6,0,6,0",
        L"Margin=2,0,0,0"}},
    ThemeTargetStyles{L"SystemTray.OmniButton#NotificationCenterButton > Grid > ContentPresenter > ItemsPresenter > StackPanel > ContentPresenter > systemtray:IconView#SystemTrayIcon > Grid", {
        L"Padding=$TrayPadding"}},
    ThemeTargetStyles{L"SystemTray.IconView#SystemTrayIcon > Grid#ContainerGrid > ContentPresenter#ContentPresenter > Grid#ContentGrid > SystemTray.TextIconContent > Grid#ContainerGrid", {
        L"Padding=10",
        L"CornerRadius=10"}},
    ThemeTargetStyles{L"SystemTray.StackListView#IconStack > ItemsPresenter > StackPanel > ContentPresenter > SystemTray.IconView#SystemTrayIcon", {
        L"Padding=0"}},
    ThemeTargetStyles{L"SystemTray.Stack#ShowDesktopStack", {
        L"Visibility=Visible"}},
    ThemeTargetStyles{L"Taskbar.Gripper#GripperControl", {
        L"Width=Auto",
        L"MinWidth=24"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#AugmentedEntryPointContentGrid", {
        L"Margin=4,0,0,0"}},
    ThemeTargetStyles{L"TextBlock#TimeInnerTextBlock", {
        L"FontSize=13",
        L"FontFamily=vivo Sans EN VF",
        L"Margin=0",
        L"Padding=0",
        L"RenderTransform:=<TranslateTransform X=\"0\" Y=\"0\" />"}},
    ThemeTargetStyles{L"TextBlock#DateInnerTextBlock", {
        L"Visibility=Collapsed",
        L"RenderTransform:=<TranslateTransform X=\"0\" Y=\"-9\" />",
        L"FontSize=11",
        L"FontFamily=vivo Sans EN VF"}},
    ThemeTargetStyles{L"TextBlock#InnerTextBlock[Text=\uE971]", {
        L"Text=\uED14"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#ConfirmatorMainGrid", {
        L"CornerRadius=22",
        L"BorderThickness=$BorderThickness",
        L"BorderBrush:=$BorderBrush",
        L"Background:=$Background"}},
    ThemeTargetStyles{L"TextBlock#SearchBoxTextBlock", {
        L"Text=Search",
        L"FontSize=10",
        L"FontFamily=vivo Sans EN VF"}},
    ThemeTargetStyles{L"SystemTray.OmniButton#NotificationCenterButton > Grid > ContentPresenter > ItemsPresenter > StackPanel > ContentPresenter > SystemTray.IconView#SystemTrayIcon > Grid > Grid > SystemTray.TextIconContent", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Button", {
        L"BorderThickness=$BorderThickness"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#ModalRootGrid > Windows.UI.Xaml.Controls.Border#BackgroundElement", {
        L"BorderThickness=$BorderThickness",
        L"BorderBrush:=$BorderBrush",
        L"Background=Transparent",
        L"CornerRadius=$CornerRadius"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#ModalRootGrid > Windows.UI.Xaml.Controls.Border#BackgroundElement > WindowsInternal.ComposableShell.Experiences.Switcher.SwitchItemList", {
        L"Background:=$Background"}},
    ThemeTargetStyles{L"WindowsInternal.ComposableShell.Experiences.Switcher.VirtualDesktopBarElement#VirtualDesktopBar > Grid > Border", {
        L"BorderThickness=$BorderThickness",
        L"BorderBrush:=$BorderBrush",
        L"Background:=$Background",
        L"CornerRadius=$CornerRadius"}},
    ThemeTargetStyles{L"WindowsInternal.ComposableShell.Experiences.Switcher.VirtualDesktopBarElement#VirtualDesktopBar", {
        L"Width=Auto",
        L"Visibility=Visible",
        L"HorizontalAlignment=1"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Border#BackgroundDimmingLayer", {
        L"Background:=Transparent"}},
    ThemeTargetStyles{L"Taskbar.TaskListButton#TaskListButton", {
        L"CornerRadius=10"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Border#SnapBarBorder", {
        L"Background:=$Background",
        L"BorderBrush:=$BorderBrush",
        L"CornerRadius=$CornerRadius",
        L"BorderThickness=$BorderThickness",
        L"RenderTransform:=<TranslateTransform X=\"0\" Y=\"10\" />",
        L"Margin=0,0,0,-10"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Border#SnapPickerBorder", {
        L"Background:=$Background",
        L"BorderBrush:=$BorderBrush",
        L"CornerRadius=$CornerRadius",
        L"BorderThickness=$BorderThickness"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Border#SearchPillBackgroundElement", {
        L"BorderBrush:=$ElementBorderBrush",
        L"CornerRadius=$ElementCornerRadius",
        L"BorderThickness=$ElementBorderThickness",
        L"MaxWidth:=100",
        L"Width=Auto"}},
    ThemeTargetStyles{L"Taskbar.TaskbarExtensionElement", {
        L"RenderTransform:=<TranslateTransform X=\"0\" Y=\"0\" />"}},
    ThemeTargetStyles{L"Taskbar.TaskListButtonPanel#ExperienceToggleButtonRootPanel", {
        L"RenderTransform:=<TranslateTransform X=\"0\" Y=\"0\" />"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.ToolTip > Windows.UI.Xaml.Controls.ContentPresenter#LayoutRoot", {
        L"Background:=$Background",
        L"BorderBrush:=$BorderBrush",
        L"BorderThickness:=$BorderThickness",
        L"CornerRadius=12"}},
    ThemeTargetStyles{L"SearchUx.SearchUI.SearchButtonControl", {
        L"MaxWidth:=300",
        L"MinWidth:=10",
        L"Width=Auto",
        L"Margin=0"}},
    ThemeTargetStyles{L"WindowsInternal.ComposableShell.Experiences.Switcher.VirtualDesktopBarElement > Windows.UI.Xaml.Controls.Grid#GridElement > Windows.UI.Xaml.Controls.Border#VirtualDesktopSwitcherBackground", {
        L"Background:=$Background",
        L"BorderBrush:=$BorderBrush",
        L"BorderThickness=$BorderThickness",
        L"CornerRadius=$CornerRadius"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Shapes.Rectangle#BackgroundFill", {
        L"Fill:=$Background"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Border#OverflowFlyoutBackgroundBorder", {
        L"Background:=$Background",
        L"BorderBrush:=$BorderBrush",
        L"BorderThickness=$BorderThickness",
        L"CornerRadius=$CornerRadius"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.MenuFlyoutPresenter > Windows.UI.Xaml.Controls.Border", {
        L"Background:=$Background",
        L"BorderBrush:=$BorderBrush",
        L"BorderThickness=$BorderThickness",
        L"CornerRadius=$CornerRadius"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#HoverFlyoutGrid > Windows.UI.Xaml.Controls.Border#HoverFlyoutBackground", {
        L"Background:=Transparent",
        L"BorderBrush:=$BorderBrush",
        L"BorderThickness=$BorderThickness",
        L"CornerRadius=$CornerRadius"}},
}, {
    L"Translucent=<WindhawkBlur BlurAmount=\"15\" TintColor=\"#10808080\"/>",
    L"Glass=<WindhawkBlur BlurAmount=\"5\" TintColor=\"{ThemeResource SystemChromeMediumColor}\" TintOpacity=\"0.7\" />",
    L"Frosted=<WindhawkBlur BlurAmount=\"20\" TintColor=\"{ThemeResource SystemChromeMediumColor}\" TintOpacity=\"0.7\" />",
    L"Acrylic=<WindhawkBlur BlurAmount=\"30\" TintColor=\"{ThemeResource SystemChromeMediumColor}\" TintOpacity=\"0.8\" />",
    L"Background=$Glass",
    L"BorderBrush=<LinearGradientBrush StartPoint=\"0,0\" EndPoint=\"0,1\"><GradientStop Color=\"#50808080\" Offset=\"0.0\" /><GradientStop Color=\"#30404040\" Offset=\"0.25\" /><GradientStop Color=\"#40808080\" Offset=\"1\" /></LinearGradientBrush>",
    L"BorderBrush2=<WindhawkBlur BlurAmount=\"10\" TintColor=\"#909090\" TintOpacity=\"0.2\"/>",
    L"ElementBG=<SolidColorBrush Color=\"{ThemeResource SystemChromeLowColor}\" Opacity=\"0.3\" />",
    L"ElementBorderThickness=0.3,0.3,0.3,1",
    L"ElementBorderBrush=<LinearGradientBrush StartPoint=\"0,0\" EndPoint=\"0,1\"><GradientStop Color=\"#50808080\" Offset=\"1\" /><GradientStop Color=\"#50606060\" Offset=\"0.15\" /></LinearGradientBrush>",
    L"ElementCornerRadius=12",
    L"ElementSysColor=<SolidColorBrush Color=\"{ThemeResource SystemAccentColorLight1}\" Opacity=\"1\" />",
    L"ElementSysColor2=<SolidColorBrush Color=\"{ThemeResource SystemAccentColorLight2}\" Opacity=\"1\" />",
    L"ElementSysColor3=<SolidColorBrush Color=\"{ThemeResource SystemAccentColorLight3}\" Opacity=\"1\" />",
    L"ElementSysColor4=<SolidColorBrush Color=\"{ThemeResource SystemAccentColorDark1}\" Opacity=\"1\" />",
    L"BorderThickness=0.5,1,0.5,1",
    L"CornerRadius=25",
    L"TrayPadding=2,4,2,4",
    L"Height=70",
    L"TaskbarFrameMaxWidth=1895",
}};

const Theme g_themeWindowGlass_variant_Split = {{
    ThemeTargetStyles{L"Taskbar.TaskbarFrame", {
        L"Grid.Column=1",
        L"MaxWidth={{min($TaskbarFrameMaxWidth, containerGridWidth)}}",
        L"Width=Auto",
        L"MinWidth:=100"}},
    ThemeTargetStyles{L"Taskbar.TaskbarFrame > Grid#RootGrid", {
        L"Margin=10,2,3,2",
        L"BorderThickness=$BorderThickness",
        L"CornerRadius=25,5,5,25",
        L"Background:=$Background",
        L"Padding=10,0,0,0"}},
    ThemeTargetStyles{L"Taskbar.TaskbarBackground#BackgroundControl > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Shapes.Rectangle#BackgroundFill", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Taskbar.TaskbarBackground#BackgroundControl > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Shapes.Rectangle#BackgroundStroke", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Taskbar.AugmentedEntryPointButton#AugmentedEntryPointButton > Taskbar.TaskListButtonPanel#ExperienceToggleButtonRootPanel", {
        L"Margin=0,8,0,8",
        L"Background:=Transparent",
        L"CornerRadius=0",
        L"BorderThickness=0,0,1,0",
        L"BorderBrush:=#20808080",
        L"Padding=2,0,5,0",
        L"MaxWidth:=200"}},
    ThemeTargetStyles{L"Grid#SystemTrayFrameGrid", {
        L"Margin=0,2,3,2",
        L"Background:=$Background",
        L"BorderBrush:=$BorderBrush",
        L"BorderThickness=$BorderThickness",
        L"CornerRadius=5,25,25,5",
        L"Padding=0,0,10,0"}},
    ThemeTargetStyles{L"SystemTray.ChevronIconView", {
        L"Padding=$TrayPadding",
        L"CornerRadius=10",
        L"Margin=5,0,0,0"}},
    ThemeTargetStyles{L"SystemTray.NotifyIconView#NotifyItemIcon", {
        L"Padding=$TrayPadding",
        L"CornerRadius=10",
        L"Margin=5,0,0,0"}},
    ThemeTargetStyles{L"SystemTray.OmniButton", {
        L"Padding=$TrayPadding",
        L"CornerRadius=10"}},
    ThemeTargetStyles{L"SystemTray.CopilotIcon", {
        L"Padding=$TrayPadding"}},
    ThemeTargetStyles{L"SystemTray.OmniButton#NotificationCenterButton > Grid > ContentPresenter > ItemsPresenter > StackPanel > ContentPresenter > systemtray:IconView#SystemTrayIcon > Grid", {
        L"Padding=$TrayPadding"}},
    ThemeTargetStyles{L"SystemTray.IconView#SystemTrayIcon > Grid#ContainerGrid > ContentPresenter#ContentPresenter > Grid#ContentGrid > SystemTray.TextIconContent > Grid#ContainerGrid", {
        L"Padding=10",
        L"CornerRadius=10"}},
    ThemeTargetStyles{L"SystemTray.StackListView#IconStack > ItemsPresenter > StackPanel > ContentPresenter > SystemTray.IconView#SystemTrayIcon", {
        L"Padding=0"}},
    ThemeTargetStyles{L"SystemTray.Stack#ShowDesktopStack", {
        L"Visibility=Visible"}},
    ThemeTargetStyles{L"Taskbar.Gripper#GripperControl", {
        L"Width=Auto",
        L"MinWidth=24"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#AugmentedEntryPointContentGrid", {
        L"Margin=4,0,0,0"}},
    ThemeTargetStyles{L"TextBlock#TimeInnerTextBlock", {
        L"FontSize=13",
        L"FontFamily=vivo Sans EN VF",
        L"Margin=0",
        L"Padding=0",
        L"RenderTransform:=<TranslateTransform X=\"0\" Y=\"0\" />"}},
    ThemeTargetStyles{L"TextBlock#DateInnerTextBlock", {
        L"Visibility=Collapsed",
        L"RenderTransform:=<TranslateTransform X=\"0\" Y=\"-9\" />",
        L"FontSize=11",
        L"FontFamily=vivo Sans EN VF"}},
    ThemeTargetStyles{L"TextBlock#InnerTextBlock[Text=\uE971]", {
        L"Text=\uED14"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#ConfirmatorMainGrid", {
        L"CornerRadius=22",
        L"BorderThickness=$BorderThickness",
        L"BorderBrush:=$BorderBrush",
        L"Background:=$Background"}},
    ThemeTargetStyles{L"TextBlock#SearchBoxTextBlock", {
        L"Text=Search",
        L"FontSize=10",
        L"FontFamily=vivo Sans EN VF"}},
    ThemeTargetStyles{L"SystemTray.OmniButton#NotificationCenterButton > Grid > ContentPresenter > ItemsPresenter > StackPanel > ContentPresenter > SystemTray.IconView#SystemTrayIcon > Grid > Grid > SystemTray.TextIconContent", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Button", {
        L"BorderThickness=$BorderThickness"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#ModalRootGrid > Windows.UI.Xaml.Controls.Border#BackgroundElement", {
        L"BorderThickness=$BorderThickness",
        L"BorderBrush:=$BorderBrush",
        L"Background=Transparent",
        L"CornerRadius=$CornerRadius"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#ModalRootGrid > Windows.UI.Xaml.Controls.Border#BackgroundElement > WindowsInternal.ComposableShell.Experiences.Switcher.SwitchItemList", {
        L"Background:=$Background"}},
    ThemeTargetStyles{L"WindowsInternal.ComposableShell.Experiences.Switcher.VirtualDesktopBarElement#VirtualDesktopBar > Grid > Border", {
        L"BorderThickness=$BorderThickness",
        L"BorderBrush:=$BorderBrush",
        L"Background:=$Background",
        L"CornerRadius=$CornerRadius"}},
    ThemeTargetStyles{L"WindowsInternal.ComposableShell.Experiences.Switcher.VirtualDesktopBarElement#VirtualDesktopBar", {
        L"Width=Auto",
        L"Visibility=Visible",
        L"HorizontalAlignment=1"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Border#BackgroundDimmingLayer", {
        L"Background:=Transparent"}},
    ThemeTargetStyles{L"Taskbar.TaskListButtonPanel#ExperienceToggleButtonRootPanel > Windows.UI.Xaml.Controls.Border#BackgroundElement", {
        L"CornerRadius=10"}},
    ThemeTargetStyles{L"Taskbar.TaskListButton#TaskListButton", {
        L"CornerRadius=10"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Border#SnapBarBorder", {
        L"Background:=$Background",
        L"BorderBrush:=$BorderBrush",
        L"CornerRadius=$CornerRadius",
        L"BorderThickness=$BorderThickness",
        L"RenderTransform:=<TranslateTransform X=\"0\" Y=\"10\" />",
        L"Margin=0,0,0,-10"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Border#SnapPickerBorder", {
        L"Background:=$Background",
        L"BorderBrush:=$BorderBrush",
        L"CornerRadius=$CornerRadius",
        L"BorderThickness=$BorderThickness"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Border#SearchPillBackgroundElement", {
        L"BorderBrush:=$ElementBorderBrush",
        L"CornerRadius=$ElementCornerRadius",
        L"BorderThickness=$ElementBorderThickness",
        L"MaxWidth:=100",
        L"Width=Auto"}},
    ThemeTargetStyles{L"Taskbar.TaskbarExtensionElement", {
        L"RenderTransform:=<TranslateTransform X=\"0\" Y=\"0\" />"}},
    ThemeTargetStyles{L"Taskbar.TaskListButtonPanel#ExperienceToggleButtonRootPanel", {
        L"RenderTransform:=<TranslateTransform X=\"0\" Y=\"0\" />"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.ToolTip > Windows.UI.Xaml.Controls.ContentPresenter#LayoutRoot", {
        L"Background:=$Background",
        L"BorderBrush:=$BorderBrush",
        L"BorderThickness:=$BorderThickness",
        L"CornerRadius=12"}},
    ThemeTargetStyles{L"SearchUx.SearchUI.SearchButtonControl", {
        L"MaxWidth:=300",
        L"MinWidth:=10",
        L"Width=Auto",
        L"Margin=0"}},
    ThemeTargetStyles{L"WindowsInternal.ComposableShell.Experiences.Switcher.VirtualDesktopBarElement > Windows.UI.Xaml.Controls.Grid#GridElement > Windows.UI.Xaml.Controls.Border#VirtualDesktopSwitcherBackground", {
        L"Background:=$Background",
        L"BorderBrush:=$BorderBrush",
        L"BorderThickness=$BorderThickness",
        L"CornerRadius=$CornerRadius"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Shapes.Rectangle#BackgroundFill", {
        L"Fill:=$Background"}},
    ThemeTargetStyles{L"SystemTray.SystemTrayFrame", {
        L"Grid.Column=2"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Border#OverflowFlyoutBackgroundBorder", {
        L"Background:=$Background",
        L"BorderBrush:=$BorderBrush",
        L"BorderThickness=$BorderThickness",
        L"CornerRadius=$CornerRadius"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.MenuFlyoutPresenter > Windows.UI.Xaml.Controls.Border", {
        L"Background:=$Background",
        L"BorderBrush:=$BorderBrush",
        L"BorderThickness=$BorderThickness",
        L"CornerRadius=$CornerRadius"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#HoverFlyoutGrid > Windows.UI.Xaml.Controls.Border#HoverFlyoutBackground", {
        L"Background:=Transparent",
        L"BorderBrush:=$BorderBrush",
        L"BorderThickness=$BorderThickness",
        L"CornerRadius=$CornerRadius"}},
    ThemeTargetStyles{L":root > ScrollViewer > ScrollContentPresenter > Border > Grid", {
        L"ColumnDefinitions:=<ColumnDefinitionCollection><ColumnDefinition Width=\"*\"/><ColumnDefinition Width=\"Auto\"/><ColumnDefinition Width=\"Auto\"/><ColumnDefinition Width=\"*\"/></ColumnDefinitionCollection>",
        L"ActualWidth=>containerGridWidth"}},
}, {
    L"Translucent=<WindhawkBlur BlurAmount=\"15\" TintColor=\"#10808080\"/>",
    L"Glass=<WindhawkBlur BlurAmount=\"5\" TintColor=\"{ThemeResource SystemChromeMediumColor}\" TintOpacity=\"0.7\" />",
    L"Frosted=<WindhawkBlur BlurAmount=\"20\" TintColor=\"{ThemeResource SystemChromeMediumColor}\" TintOpacity=\"0.7\" />",
    L"Acrylic=<WindhawkBlur BlurAmount=\"30\" TintColor=\"{ThemeResource SystemChromeMediumColor}\" TintOpacity=\"0.8\" />",
    L"Background=$Glass",
    L"BorderBrush=<LinearGradientBrush StartPoint=\"0,0\" EndPoint=\"0,1\"><GradientStop Color=\"#50808080\" Offset=\"0.0\" /><GradientStop Color=\"#30404040\" Offset=\"0.25\" /><GradientStop Color=\"#40808080\" Offset=\"1\" /></LinearGradientBrush>",
    L"BorderBrush2=<WindhawkBlur BlurAmount=\"10\" TintColor=\"#909090\" TintOpacity=\"0.2\"/>",
    L"ElementBG=<SolidColorBrush Color=\"{ThemeResource SystemChromeLowColor}\" Opacity=\"0.3\" />",
    L"ElementBorderThickness=0.3,0.3,0.3,1",
    L"ElementBorderBrush=<LinearGradientBrush StartPoint=\"0,0\" EndPoint=\"0,1\"><GradientStop Color=\"#50808080\" Offset=\"1\" /><GradientStop Color=\"#50606060\" Offset=\"0.15\" /></LinearGradientBrush>",
    L"ElementCornerRadius=12",
    L"ElementSysColor=<SolidColorBrush Color=\"{ThemeResource SystemAccentColorLight1}\" Opacity=\"1\" />",
    L"ElementSysColor2=<SolidColorBrush Color=\"{ThemeResource SystemAccentColorLight2}\" Opacity=\"1\" />",
    L"ElementSysColor3=<SolidColorBrush Color=\"{ThemeResource SystemAccentColorLight3}\" Opacity=\"1\" />",
    L"ElementSysColor4=<SolidColorBrush Color=\"{ThemeResource SystemAccentColorDark1}\" Opacity=\"1\" />",
    L"BorderThickness=0.5,1,0.5,1",
    L"CornerRadius=25",
    L"TrayPadding=2,4,2,4",
    L"Height=70",
    L"TaskbarFrameMaxWidth=1895",
}};

const Theme g_themeWindowGlass_variant_FullLength = {{
    ThemeTargetStyles{L"Taskbar.TaskbarFrame", {
        L"Width=Auto",
        L"MinWidth:=100"}},
    ThemeTargetStyles{L"Taskbar.TaskbarFrame > Grid#RootGrid", {
        L"Margin=10,2,10,2",
        L"BorderThickness=$BorderThickness",
        L"CornerRadius=$CornerRadius",
        L"Background:=$Background",
        L"Padding=0"}},
    ThemeTargetStyles{L"Taskbar.TaskbarBackground#BackgroundControl > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Shapes.Rectangle#BackgroundFill", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Taskbar.TaskbarBackground#BackgroundControl > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Shapes.Rectangle#BackgroundStroke", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Taskbar.AugmentedEntryPointButton#AugmentedEntryPointButton > Taskbar.TaskListButtonPanel#ExperienceToggleButtonRootPanel", {
        L"Margin=0,10,0,10",
        L"Background:=Transparent",
        L"CornerRadius=0",
        L"BorderThickness=1,0,1,0",
        L"BorderBrush:=#20808080",
        L"Padding=2,0,5,0",
        L"MaxWidth:=200"}},
    ThemeTargetStyles{L"Grid#SystemTrayFrameGrid", {
        L"Margin=0,8,8,7",
        L"Background:=Transparent",
        L"BorderBrush:=Transparent",
        L"BorderThickness=$ElementBorderThickness",
        L"CornerRadius=$ElementCornerRadius"}},
    ThemeTargetStyles{L"SystemTray.ChevronIconView", {
        L"Padding=$TrayPadding",
        L"CornerRadius=10",
        L"Margin=5,0,0,0"}},
    ThemeTargetStyles{L"SystemTray.NotifyIconView#NotifyItemIcon", {
        L"Padding=$TrayPadding",
        L"CornerRadius=10",
        L"Margin=5,0,0,0"}},
    ThemeTargetStyles{L"SystemTray.OmniButton", {
        L"Padding=$TrayPadding",
        L"CornerRadius=10"}},
    ThemeTargetStyles{L"SystemTray.CopilotIcon", {
        L"Padding=$TrayPadding"}},
    ThemeTargetStyles{L"SystemTray.OmniButton#NotificationCenterButton > Grid > ContentPresenter > ItemsPresenter > StackPanel > ContentPresenter > systemtray:IconView#SystemTrayIcon > Grid", {
        L"Padding=$TrayPadding"}},
    ThemeTargetStyles{L"SystemTray.IconView#SystemTrayIcon > Grid#ContainerGrid > ContentPresenter#ContentPresenter > Grid#ContentGrid > SystemTray.TextIconContent > Grid#ContainerGrid", {
        L"Padding=10",
        L"CornerRadius=10"}},
    ThemeTargetStyles{L"SystemTray.StackListView#IconStack > ItemsPresenter > StackPanel > ContentPresenter > SystemTray.IconView#SystemTrayIcon", {
        L"Padding=0"}},
    ThemeTargetStyles{L"SystemTray.Stack#ShowDesktopStack", {
        L"Visibility=Visible"}},
    ThemeTargetStyles{L"Taskbar.Gripper#GripperControl", {
        L"Width=Auto",
        L"MinWidth=24"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#AugmentedEntryPointContentGrid", {
        L"Margin=4,0,0,0",
        L"HorizontalAlignment=Left"}},
    ThemeTargetStyles{L"TextBlock#TimeInnerTextBlock", {
        L"FontSize=13",
        L"FontFamily=vivo Sans EN VF",
        L"Margin=0",
        L"Padding=0",
        L"RenderTransform:=<TranslateTransform X=\"0\" Y=\"0\" />"}},
    ThemeTargetStyles{L"TextBlock#DateInnerTextBlock", {
        L"Visibility=Collapsed",
        L"RenderTransform:=<TranslateTransform X=\"0\" Y=\"-9\" />",
        L"FontSize=11",
        L"FontFamily=vivo Sans EN VF"}},
    ThemeTargetStyles{L"TextBlock#InnerTextBlock[Text=\uE971]", {
        L"Text=\uED14"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#ConfirmatorMainGrid", {
        L"CornerRadius=22",
        L"BorderThickness=$BorderThickness",
        L"BorderBrush:=$BorderBrush",
        L"Background:=$Background"}},
    ThemeTargetStyles{L"TextBlock#SearchBoxTextBlock", {
        L"Text=Search",
        L"FontSize=10",
        L"FontFamily=vivo Sans EN VF"}},
    ThemeTargetStyles{L"SystemTray.OmniButton#NotificationCenterButton > Grid > ContentPresenter > ItemsPresenter > StackPanel > ContentPresenter > SystemTray.IconView#SystemTrayIcon > Grid > Grid > SystemTray.TextIconContent", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Button", {
        L"BorderThickness=$BorderThickness"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#ModalRootGrid > Windows.UI.Xaml.Controls.Border#BackgroundElement", {
        L"BorderThickness=$BorderThickness",
        L"BorderBrush:=$BorderBrush",
        L"Background=Transparent",
        L"CornerRadius=$CornerRadius"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#ModalRootGrid > Windows.UI.Xaml.Controls.Border#BackgroundElement > WindowsInternal.ComposableShell.Experiences.Switcher.SwitchItemList", {
        L"Background:=$Background"}},
    ThemeTargetStyles{L"WindowsInternal.ComposableShell.Experiences.Switcher.VirtualDesktopBarElement#VirtualDesktopBar > Grid > Border", {
        L"BorderThickness=$BorderThickness",
        L"BorderBrush:=$BorderBrush",
        L"Background:=$Background",
        L"CornerRadius=$CornerRadius"}},
    ThemeTargetStyles{L"WindowsInternal.ComposableShell.Experiences.Switcher.VirtualDesktopBarElement#VirtualDesktopBar", {
        L"Width=Auto",
        L"Visibility=Visible",
        L"HorizontalAlignment=1"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Border#BackgroundDimmingLayer", {
        L"Background:=Transparent"}},
    ThemeTargetStyles{L"Taskbar.TaskListButtonPanel#ExperienceToggleButtonRootPanel > Windows.UI.Xaml.Controls.Border#BackgroundElement", {
        L"CornerRadius=10"}},
    ThemeTargetStyles{L"Taskbar.TaskListButton#TaskListButton", {
        L"CornerRadius=10"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Border#SnapBarBorder", {
        L"Background:=$Background",
        L"BorderBrush:=$BorderBrush",
        L"CornerRadius=$CornerRadius",
        L"BorderThickness=$BorderThickness",
        L"RenderTransform:=<TranslateTransform X=\"0\" Y=\"10\" />",
        L"Margin=0,0,0,-10"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Border#SnapPickerBorder", {
        L"Background:=$Background",
        L"BorderBrush:=$BorderBrush",
        L"CornerRadius=$CornerRadius",
        L"BorderThickness=$BorderThickness"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Border#SearchPillBackgroundElement", {
        L"BorderBrush:=$ElementBorderBrush",
        L"CornerRadius=$ElementCornerRadius",
        L"BorderThickness=$ElementBorderThickness",
        L"MaxWidth:=100",
        L"Width=Auto"}},
    ThemeTargetStyles{L"Taskbar.TaskbarExtensionElement", {
        L"RenderTransform:=<TranslateTransform X=\"0\" Y=\"0\" />"}},
    ThemeTargetStyles{L"Taskbar.TaskListButtonPanel#ExperienceToggleButtonRootPanel", {
        L"RenderTransform:=<TranslateTransform X=\"0\" Y=\"0\" />"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.ToolTip > Windows.UI.Xaml.Controls.ContentPresenter#LayoutRoot", {
        L"Background:=$Background",
        L"BorderBrush:=$BorderBrush",
        L"BorderThickness:=$BorderThickness",
        L"CornerRadius=12"}},
    ThemeTargetStyles{L"SearchUx.SearchUI.SearchButtonControl", {
        L"MaxWidth:=300",
        L"MinWidth:=10",
        L"Width=Auto",
        L"Margin=0"}},
    ThemeTargetStyles{L"WindowsInternal.ComposableShell.Experiences.Switcher.VirtualDesktopBarElement > Windows.UI.Xaml.Controls.Grid#GridElement > Windows.UI.Xaml.Controls.Border#VirtualDesktopSwitcherBackground", {
        L"Background:=$Background",
        L"BorderBrush:=$BorderBrush",
        L"BorderThickness=$BorderThickness",
        L"CornerRadius=$CornerRadius"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Shapes.Rectangle#BackgroundFill", {
        L"Fill:=$Background"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Border#OverflowFlyoutBackgroundBorder", {
        L"Background:=$Background",
        L"BorderBrush:=$BorderBrush",
        L"BorderThickness=$BorderThickness",
        L"CornerRadius=$CornerRadius"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.MenuFlyoutPresenter > Windows.UI.Xaml.Controls.Border", {
        L"Background:=$Background",
        L"BorderBrush:=$BorderBrush",
        L"BorderThickness=$BorderThickness",
        L"CornerRadius=$CornerRadius"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#HoverFlyoutGrid > Windows.UI.Xaml.Controls.Border#HoverFlyoutBackground", {
        L"Background:=Transparent",
        L"BorderBrush:=$BorderBrush",
        L"BorderThickness=$BorderThickness",
        L"CornerRadius=$CornerRadius"}},
}, {
    L"Translucent=<WindhawkBlur BlurAmount=\"15\" TintColor=\"#10808080\"/>",
    L"Glass=<WindhawkBlur BlurAmount=\"5\" TintColor=\"{ThemeResource SystemChromeMediumColor}\" TintOpacity=\"0.7\" />",
    L"Frosted=<WindhawkBlur BlurAmount=\"20\" TintColor=\"{ThemeResource SystemChromeMediumColor}\" TintOpacity=\"0.7\" />",
    L"Acrylic=<WindhawkBlur BlurAmount=\"30\" TintColor=\"{ThemeResource SystemChromeMediumColor}\" TintOpacity=\"0.8\" />",
    L"Background=$Glass",
    L"BorderBrush=<LinearGradientBrush StartPoint=\"0,0\" EndPoint=\"0,1\"><GradientStop Color=\"#50808080\" Offset=\"0.0\" /><GradientStop Color=\"#30404040\" Offset=\"0.25\" /><GradientStop Color=\"#40808080\" Offset=\"1\" /></LinearGradientBrush>",
    L"BorderBrush2=<WindhawkBlur BlurAmount=\"10\" TintColor=\"#909090\" TintOpacity=\"0.2\"/>",
    L"ElementBG=<SolidColorBrush Color=\"{ThemeResource SystemChromeLowColor}\" Opacity=\"0.3\" />",
    L"ElementBorderThickness=0.3,0.3,0.3,1",
    L"ElementBorderBrush=<LinearGradientBrush StartPoint=\"0,0\" EndPoint=\"0,1\"><GradientStop Color=\"#50808080\" Offset=\"1\" /><GradientStop Color=\"#50606060\" Offset=\"0.15\" /></LinearGradientBrush>",
    L"ElementCornerRadius=12",
    L"ElementSysColor=<SolidColorBrush Color=\"{ThemeResource SystemAccentColorLight1}\" Opacity=\"1\" />",
    L"ElementSysColor2=<SolidColorBrush Color=\"{ThemeResource SystemAccentColorLight2}\" Opacity=\"1\" />",
    L"ElementSysColor3=<SolidColorBrush Color=\"{ThemeResource SystemAccentColorLight3}\" Opacity=\"1\" />",
    L"ElementSysColor4=<SolidColorBrush Color=\"{ThemeResource SystemAccentColorDark1}\" Opacity=\"1\" />",
    L"BorderThickness=0.5,1,0.5,1",
    L"CornerRadius=20",
    L"TrayPadding=2,4,2,4",
    L"Height=70",
}};

const Theme g_themeSurface = {{
    ThemeTargetStyles{L"Grid#RootGrid > Taskbar.TaskbarBackground > Grid", {
        L"CornerRadius=20",
        L"BorderThickness=1",
        L"Margin=-20,0,-20,0",
        L"BorderBrush=#40FFFFFF",
        L"Padding=-1"}},
    ThemeTargetStyles{L"Rectangle#BackgroundStroke", {
        L"Fill=Transparent"}},
    ThemeTargetStyles{L"Taskbar.TaskbarFrame", {
        L"Width=Auto",
        L"HorizontalAlignment=Center"}},
    ThemeTargetStyles{L"Taskbar.TaskbarFrame > Grid#RootGrid", {
        L"Visibility=Visible",
        L"Margin=0,0,0,10",
        L"Padding=20,0,20,0"}},
    ThemeTargetStyles{L"Grid#SystemTrayFrameGrid", {
        L"Margin=0,0,0,10",
        L"CornerRadius=20,0,0,20",
        L"BorderThickness=1,1,0,1",
        L"BorderBrush=#66FFFFFF",
        L"Padding=10,5,0,5",
        L"Background:=<WindhawkBlur BlurAmount=\"5\" TintColor=\"{ThemeResource SystemChromeAltHighColor}\" TintOpacity=\"0.5\" />",
        L"Visibility=Visible"}},
    ThemeTargetStyles{L"Taskbar.TaskbarFrame > Grid#RootGrid > Taskbar.TaskbarBackground > Grid > Rectangle#BackgroundFill", {
        L"Fill:=<WindhawkBlur BlurAmount=\"5\" TintColor=\"#12FFFFFF\"/>"}},
    ThemeTargetStyles{L"Taskbar.TaskListLabeledButtonPanel@RunningIndicatorStates > Border#BackgroundElement", {
        L"Background:=$TaskItemBackground",
        L"Margin=-1,5.5,1,4",
        L"CornerRadius=12",
        L"BorderThickness=2,1,0.5,2",
        L"BorderBrush:=$TaskItemBorder"}},
    ThemeTargetStyles{L"Taskbar.TaskListButtonPanel#ExperienceToggleButtonRootPanel > Border#BackgroundElement", {
        L"Background:=$SystemItemBackground",
        L"CornerRadius=12",
        L"Margin=-1,5.5,2.5,4",
        L"BorderThickness=2,1,0.5,2",
        L"BorderBrush:=$SystemItemBorder"}},
    ThemeTargetStyles{L"Taskbar.TaskListLabeledButtonPanel@CommonStates > Rectangle#RunningIndicator", {
        L"Margin=0,0,0,8"}},
    ThemeTargetStyles{L"Border#MultiWindowElement", {
        L"Height=0"}},
}, {
    L"TaskItemBackground=<AcrylicBrush TintColor=\"{ThemeResource SystemChromeAltHighColor}\" TintOpacity=\"0.9\" FallbackColor=\"{ThemeResource SystemChromeMediumColor}\" />",
    L"TaskItemBorder=<LinearGradientBrush StartPoint=\"0,0\" EndPoint=\"0.5,1\"><GradientStop Color=\"#00000000\" Offset=\"0\" /><GradientStop Color=\"#33000000\" Offset=\"1.5\" /></LinearGradientBrush>",
    L"SystemItemBackground=<AcrylicBrush TintColor=\"{ThemeResource SystemChromeAltHighColor}\" TintOpacity=\"0.8\" FallbackColor=\"{ThemeResource SystemChromeLowColor}\" />",
    L"SystemItemBorder=<LinearGradientBrush StartPoint=\"0,0\" EndPoint=\"0.5,1\"><GradientStop Color=\"#00000000\" Offset=\"0\" /><GradientStop Color=\"#33000000\" Offset=\"1.5\" /></LinearGradientBrush>",
}};

const Theme g_themeOversimplified_Accentuated = {{
    ThemeTargetStyles{L"MenuFlyoutPresenter", {
        L"Background:=$DarkAccent",
        L"BorderBrush=Transparent",
        L"Shadow:="}},
    ThemeTargetStyles{L"ToolTip > ContentPresenter#LayoutRoot", {
        L"Background:=$DarkAccent",
        L"BorderBrush=Transparent",
        L"Shadow:="}},
    ThemeTargetStyles{L"Taskbar.TaskbarFrame > Grid#RootGrid > Taskbar.TaskbarBackground > Grid > Rectangle#BackgroundFill", {
        L"Fill:=$Alt"}},
    ThemeTargetStyles{L"Rectangle#BackgroundStroke", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"SystemTray.OmniButton#ControlCenterButton > Grid > ContentPresenter#ContentPresenter > ItemsPresenter > StackPanel > ContentPresenter > SystemTray.IconView#SystemTrayIcon > Grid#ContainerGrid > Grid#ContentGrid > SystemTray.TextIconContent > Grid#ContainerGrid > SystemTray.AdaptiveTextBlock#Base > TextBlock#InnerTextBlock", {
        L"FontSize=22"}},
    ThemeTargetStyles{L"Taskbar.TaskListLabeledButtonPanel@RunningIndicatorStates > Rectangle#RunningIndicator", {
        L"Fill@ActiveRunningIndicator:=$SolidAccent",
        L"Height=4",
        L"Width@ActiveRunningIndicator=25"}},
    ThemeTargetStyles{L"Taskbar.TaskListButton > Taskbar.TaskListLabeledButtonPanel > Microsoft.UI.Xaml.Controls.ProgressBar#ProgressIndicator", {
        L"MinHeight=4",
        L"Width=25"}},
    ThemeTargetStyles{L"Grid#LayoutRoot@CommonStates > Border#ProgressBarRoot > Border > Grid > Rectangle#DeterminateProgressBarIndicator", {
        L"Fill@Updating:= <SolidColorBrush Color=\"Green\" Opacity=\"1\" />",
        L"Fill@Determinate:= <SolidColorBrush Color=\"Green\" Opacity=\"1\" />",
        L"Fill@Paused:= <SolidColorBrush Color=\"Orange\" Opacity=\"1\" />",
        L"Fill@Error:= <SolidColorBrush Color=\"Red\" Opacity=\"1\" />",
        L"Fill@UpdatingError:= <SolidColorBrush Color=\"Red\" Opacity=\"1\" />"}},
    ThemeTargetStyles{L"Rectangle#ProgressBarTrack", {
        L"Fill=Transparent"}},
    ThemeTargetStyles{L"Canvas#HoverFlyoutCanvas > Grid#HoverFlyoutGrid > Border#HoverFlyoutBackground", {
        L"BorderBrush=Transparent",
        L"Shadow:="}},
    ThemeTargetStyles{L"Taskbar.TaskbarBackground#HoverFlyoutBackgroundControl > Grid > Windows.UI.Xaml.Shapes.Rectangle#BackgroundFill", {
        L"Fill:=$DarkAccent"}},
    ThemeTargetStyles{L"Grid#OverflowRootGrid", {
        L"Padding:="}},
    ThemeTargetStyles{L"Border#OverflowFlyoutBackgroundBorder", {
        L"Background:=$Alt",
        L"Shadow:=",
        L"BorderThickness:="}},
    ThemeTargetStyles{L"SystemTray.ImageIconContent > Windows.UI.Xaml.Controls.Grid#ContainerGrid > Windows.UI.Xaml.Controls.Image", {
        L"Height=20",
        L"Width=20"}},
    ThemeTargetStyles{L"WindowsInternal.ComposableShell.Experiences.TextInput.Common.InputSwitcher > ContentControl > ContentPresenter > Grid", {
        L"Background:=$DarkAccent",
        L"BorderBrush=Transparent",
        L"Shadow:="}},
    ThemeTargetStyles{L"WindowsInternal.ComposableShell.Experiences.TextInput.Common.InputSwitcher > ContentControl > ContentPresenter > Grid > Grid#OverlayPanel", {
        L"Background=Transparent",
        L"BorderBrush=Transparent"}},
    ThemeTargetStyles{L"Grid > HyperlinkButton#Footer", {
        L"HorizontalContentAlignment = 1"}},
    ThemeTargetStyles{L"Grid#ConfirmatorMainGrid", {
        L"Background:=$DarkAccent",
        L"BorderBrush=Transparent",
        L"CornerRadius=15",
        L"Margin=0,0,0,5",
        L"Padding=4,0,0,0",
        L"Shadow:="}},
    ThemeTargetStyles{L"Grid#BrightnessConfirmator", {
        L"Padding=6,0,16,0"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.AnimatedIcon#BrightnessIcon", {
        L"Height=30",
        L"Width=30",
        L"Margin=0,-1,12,0"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.AnimatedIcon#VolumeIcon", {
        L"Height=30",
        L"Width=30"}},
    ThemeTargetStyles{L"TextBlock#volumeLevelText", {
        L"FontSize=15"}},
    ThemeTargetStyles{L"Rectangle#HorizontalDecreaseRect", {
        L"Height=6",
        L"Margin= 0,2,0,0"}},
    ThemeTargetStyles{L"Rectangle#HorizontalTrackRect", {
        L"Fill=Transparent",
        L"Height=6"}},
    ThemeTargetStyles{L"Grid#HorizontalTemplate > Rectangle#HorizontalDecreaseRect", {
        L"Fill:= <AcrylicBrush TintColor=\"{ThemeResource SystemAccentColor}\" TintOpacity=\"1\" TintLuminosityOpacity=\"1\" FallbackColor=\"{ThemeResource SystemAccentColorDark2}\" />"}},
    ThemeTargetStyles{L"Grid#ModalRootGrid > Border#BackgroundElement", {
        L"Background=Transparent",
        L"BorderBrush=Transparent",
        L"CornerRadius=20",
        L"Shadow:="}},
    ThemeTargetStyles{L"Grid#ModalRootGrid > Border#BackgroundElement > WindowsInternal.ComposableShell.Experiences.Switcher.SwitchItemList", {
        L"Background:=$DarkAccent"}},
    ThemeTargetStyles{L"Border#BackgroundDimmingLayer", {
        L"Background:= <WindhawkBlur BlurAmount=\"30\" TintColor=\"#00000080\" />"}},
    ThemeTargetStyles{L"Border#VirtualDesktopBarBackground", {
        L"Background:= <SolidColorBrush Color=\"{ThemeResource SystemAccentColorDark1}\" Opacity=\"0.4\" />",
        L"BorderBrush=Transparent"}},
}, {
    L"Alt= <AcrylicBrush TintColor=\"{ThemeResource SystemAltHighColor}\" TintOpacity=\"0.6\" TintLuminosityOpacity=\"0.6\" FallbackColor=\"{ThemeResource SystemAltHighColor}\" />",
    L"Accent = <AcrylicBrush TintColor=\"{ThemeResource SystemAccentColor}\" TintOpacity=\"0.6\" TintLuminosityOpacity=\"0.6\" FallbackColor=\"{ThemeResource SystemAccentColor}\" />",
    L"DarkAccent = <AcrylicBrush TintColor=\"{ThemeResource SystemAccentColorDark1}\" TintOpacity=\"0.6\" TintLuminosityOpacity=\"0.3\" FallbackColor=\"{ThemeResource SystemAccentColorDark1}\" />",
    L"SolidAccent = <SolidColorBrush Color=\"{ThemeResource SystemAccentColor}\" Opacity=\"1\" />",
    L"Reveal= <RevealBorderBrush Color=\"Transparent\" TargetTheme=\"1\" Opacity=\"1\" />",
}};

const Theme g_themeLuminosity_variant_Dock = {{
    ThemeTargetStyles{L"Taskbar.TaskbarFrame > Grid#RootGrid > Taskbar.TaskbarBackground > Grid > Rectangle#BackgroundFill", {
        L"Fill:=$mbg"}},
    ThemeTargetStyles{L"Taskbar.TaskListButtonPanel#ExperienceToggleButtonRootPanel > Windows.UI.Xaml.Controls.Border#BackgroundElement", {
        L"CornerRadius=$bcr"}},
    ThemeTargetStyles{L"Taskbar.ExperienceToggleButton", {
        L"CornerRadius=$bcr"}},
    ThemeTargetStyles{L"Taskbar.TaskListButton", {
        L"CornerRadius=$bcr"}},
    ThemeTargetStyles{L"SearchUx.SearchUI.SearchButtonRootGrid#SearchBoxButtonRootPanel > Windows.UI.Xaml.Controls.Border#BackgroundElement", {
        L"CornerRadius=$bcr"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Border#SearchPillBackgroundElement", {
        L"CornerRadius=$bcr"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Border#MultiWindowElement", {
        L"CornerRadius=$bcr"}},
    ThemeTargetStyles{L"SystemTray.ChevronIconView > Windows.UI.Xaml.Controls.Grid#ContainerGrid > Windows.UI.Xaml.Controls.Border#BackgroundBorder", {
        L"CornerRadius=$bcr",
        L"Margin=0,4,2,4"}},
    ThemeTargetStyles{L"SystemTray.NotifyIconView > Windows.UI.Xaml.Controls.Grid#ContainerGrid > Windows.UI.Xaml.Controls.Border#BackgroundBorder", {
        L"CornerRadius=$bcr",
        L"Margin=2,4,2,4"}},
    ThemeTargetStyles{L"SystemTray.IconView#SystemTrayIcon > Windows.UI.Xaml.Controls.Grid#ContainerGrid > Windows.UI.Xaml.Controls.Border#BackgroundBorder", {
        L"CornerRadius=$bcr",
        L"Margin=2,4,2,4"}},
    ThemeTargetStyles{L"SystemTray.OmniButton#ControlCenterButton > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.Border#BackgroundBorder", {
        L"CornerRadius=$bcr",
        L"Margin=2,4,2,4"}},
    ThemeTargetStyles{L"SystemTray.OmniButton#NotificationCenterButton > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.Border#BackgroundBorder", {
        L"CornerRadius=$bcr",
        L"Margin=2,4,2,4"}},
    ThemeTargetStyles{L"Border#OverflowFlyoutBackgroundBorder", {
        L"Background:=$mbg",
        L"CornerRadius:=$wcr",
        L"BorderThickness=$bt",
        L"BorderBrush=$bb",
        L"Shadow:="}},
    ThemeTargetStyles{L"Taskbar.OverflowToggleButton#OverflowButton > Taskbar.TaskListButtonPanel#OverflowToggleButtonRootPanel > Windows.UI.Xaml.Controls.Border#BackgroundElement", {
        L"CornerRadius=$bcr"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#ConfirmatorMainGrid", {
        L"Background:=$mbg",
        L"CornerRadius=$wcr",
        L"BorderThickness=$bt",
        L"BorderBrush=$bb",
        L"Shadow:="}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Shapes.Rectangle#HorizontalTrackRect", {
        L"Fill:=#10FFFFFF"}},
    ThemeTargetStyles{L"WindowsInternal.ComposableShell.Experiences.TextInput.Common.InputSwitcher > ContentControl > ContentPresenter > Grid", {
        L"Background:=$mbg",
        L"CornerRadius=$wcr",
        L"BorderThickness=$bt",
        L"BorderBrush=$bb",
        L"Shadow:="}},
    ThemeTargetStyles{L"WindowsInternal.ComposableShell.Experiences.TextInput.Common.InputSwitcher > ContentControl > ContentPresenter > Grid > Grid", {
        L"Background:=$t"}},
    ThemeTargetStyles{L"Taskbar.TaskbarBackground#HoverFlyoutBackgroundControl > Grid > Rectangle#BackgroundFill", {
        L"Fill:=$t"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#HoverFlyoutGrid > Windows.UI.Xaml.Controls.Border#HoverFlyoutBackground", {
        L"Background:=$mbg",
        L"CornerRadius=$mcr",
        L"BorderThickness=$bt",
        L"BorderBrush=$bb",
        L"Shadow:="}},
    ThemeTargetStyles{L"Taskbar.TaskItemThumbnailView > Grid@CommonStates > Border#BackgroundBorder", {
        L"Background=$t",
        L"CornerRadius=$mcr",
        L"BorderThickness@Normal=0",
        L"BorderThickness@PointerOver=0.05,0,0.05,1",
        L"BorderBrush@Normal=$t",
        L"BorderBrush@PointerOver:=$AccentColor"}},
    ThemeTargetStyles{L"Taskbar.TaskItemThumbnailView > Grid > Button#CloseButton", {
        L"CornerRadius=$mcr"}},
    ThemeTargetStyles{L"Taskbar.ThumbBarButton#ThumbBarButton > Windows.UI.Xaml.Controls.ContentPresenter#BorderElement@CommonStates", {
        L"CornerRadius=16",
        L"Margin=-1.5",
        L"Background@Disabled:=$t",
        L"Background@Normal:=$t",
        L"Background@PointerOver:=$nbth",
        L"Background@Pressed:=$nbtp",
        L"BorderThickness=2",
        L"BorderBrush@Disabled:=$t",
        L"BorderBrush@Normal:=$t",
        L"BorderBrush@PointerOver:=$nbb",
        L"BorderBrush@Pressed:=$nbb",
        L"BackgroundSizing=InnerBorderEdge",
        L"BackgroundTransition:=<BrushTransition Duration=\"0:0:0.200\" />"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#ModalRootGrid > Windows.UI.Xaml.Controls.Border#BackgroundElement", {
        L"Background=$t",
        L"CornerRadius=$wcr",
        L"BorderThickness=$bt",
        L"BorderBrush=$bb",
        L"Shadow:="}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#ModalRootGrid > Windows.UI.Xaml.Controls.Border#BackgroundElement > WindowsInternal.ComposableShell.Experiences.Switcher.SwitchItemList", {
        L"Background:=$mbg"}},
    ThemeTargetStyles{L"WindowsInternal.ComposableShell.Experiences.Switcher.DynamicFlowPanel > WindowsInternal.ComposableShell.Experiences.Switcher.SwitchItemListViewItem > Windows.UI.Xaml.Controls.Grid#Root@CommonStates > Windows.UI.Xaml.Controls.Border#BackgroundBorder", {
        L"Background:=#09FFFFFF",
        L"CornerRadius=$mcr",
        L"BorderThickness=0.05,1,0.05,0",
        L"BorderBrush@Normal=$t",
        L"BorderBrush@PointerOver:=$AccentColor"}},
    ThemeTargetStyles{L"WindowsInternal.ComposableShell.Experiences.Switcher.SwitchItemControl > Grid#Root > WindowsInternal.ComposableShell.Experiences.Switcher.SwitchItemThumbnailButton#ThumbnailHost > Windows.UI.Xaml.Controls.Grid#RootGrid", {
        L"CornerRadius=$bcr",
        L"Margin=5"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Border#BackgroundDimmingLayer", {
        L"Background:=$t"}},
    ThemeTargetStyles{L"WindowsInternal.ComposableShell.Experiences.Switcher.VirtualDesktopBarElement > Windows.UI.Xaml.Controls.Grid#GridElement > Windows.UI.Xaml.Controls.Border#VirtualDesktopSwitcherBackground", {
        L"CornerRadius=$wcr",
        L"BorderThickness=$bt",
        L"BorderBrush=$bb",
        L"Margin=-2,1,-1,2"}},
    ThemeTargetStyles{L"WindowsInternal.ComposableShell.Experiences.Switcher.DynamicFlowPanel#DFCPanel > WindowsInternal.ComposableShell.Experiences.Switcher.SwitchItemListViewItem > Windows.UI.Xaml.Controls.Grid#Root@CommonStates > Windows.UI.Xaml.Controls.Border#BackgroundBorder", {
        L"Background:=$mbg",
        L"CornerRadius=$wcr,$wcr,$bcr,$bcr",
        L"Margin:=-5,0,-5,-5",
        L"BorderThickness=0.05,1,0.05,0",
        L"BorderBrush@Normal=$t",
        L"BorderBrush@PointerOver:=$AccentColor"}},
    ThemeTargetStyles{L"WindowsInternal.ComposableShell.Experiences.Switcher.SwitchItemElement > Windows.UI.Xaml.Controls.Grid#RootGrid > Windows.UI.Xaml.Controls.Grid#TitleGrid > Windows.UI.Xaml.Controls.Border#BackgroundBorder", {
        L"Background:=$t"}},
    ThemeTargetStyles{L"WindowsInternal.ComposableShell.Experiences.Switcher.SwitchItemElement > Windows.UI.Xaml.Controls.Grid#RootGrid > Windows.UI.Xaml.Controls.Grid#TitleGrid > Image#IconImage", {
        L"RenderTransform:=<TranslateTransform X=\"0\" Y=\"1\" />"}},
    ThemeTargetStyles{L"WindowsInternal.ComposableShell.Experiences.Switcher.SwitchItemElement > Windows.UI.Xaml.Controls.Grid#RootGrid > Windows.UI.Xaml.Controls.Grid#TitleGrid > TextBlock#DisplayName", {
        L"RenderTransform:=<TranslateTransform X=\"0\" Y=\"1\" />"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Button#SwitchItemElementCloseButton > ContentPresenter#ContentPresenter", {
        L"CornerRadius=$mcr",
        L"Margin=5"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Button#SwitchItemElementCloseButton > ContentPresenter#ContentPresenter > TextBlock", {
        L"RenderTransform:=<TranslateTransform X=\"-0.8\" Y=\"-0.6\" />"}},
    ThemeTargetStyles{L"WindowsInternal.ComposableShell.Experiences.Switcher.SwitchItemElement > Grid#RootGrid > WindowsInternal.ComposableShell.Experiences.Switcher.SwitchItemThumbnailButton#ThumbnailHost > Grid#RootGrid", {
        L"CornerRadius=0"}},
    ThemeTargetStyles{L"WindowsInternal.ComposableShell.Experiences.Switcher.VirtualDesktopBarElement#VirtualDesktopBar > Grid > Border", {
        L"Background:=$mbg",
        L"CornerRadius=$wcr",
        L"BorderThickness=$bt",
        L"BorderBrush=$bb",
        L"Margin=-1,2,0,0"}},
    ThemeTargetStyles{L"WindowsInternal.ComposableShell.Experiences.Switcher.VirtualDesktopBarElement#VirtualDesktopBar", {
        L"Width=Auto",
        L"HorizontalAlignment=1"}},
    ThemeTargetStyles{L"WindowsInternal.ComposableShell.Experiences.Switcher.VirtualDesktopBarElement > Grid > Border", {
        L"Background:=$mbg",
        L"Shadow:="}},
    ThemeTargetStyles{L"WindowsInternal.ComposableShell.Experiences.Switcher.VirtualDesktopElementThemed > Windows.UI.Xaml.Controls.Grid#MainGrid > Windows.UI.Xaml.Controls.Border#MainBorder", {
        L"CornerRadius=$mcr"}},
    ThemeTargetStyles{L"WindowsInternal.ComposableShell.Experiences.Switcher.VirtualDesktopElementThemed > Windows.UI.Xaml.Controls.Grid#MainGrid > Windows.UI.Xaml.Controls.Border#BorderHighlight", {
        L"CornerRadius=$mcr"}},
    ThemeTargetStyles{L"WindowsInternal.ComposableShell.Experiences.Switcher.NewVirtualDesktopElementThemed#NewVirtualDesktopButtonThemed > Windows.UI.Xaml.Controls.Grid#MainGrid", {
        L"CornerRadius=$mcr",
        L"BorderThickness=$bt",
        L"Margin=2"}},
    ThemeTargetStyles{L"WindowsInternal.ComposableShell.Experiences.Switcher.NewVirtualDesktopElementThemed#NewVirtualDesktopButtonThemed > Windows.UI.Xaml.Controls.Grid#MainGrid > Windows.UI.Xaml.Controls.Border#MainBorder", {
        L"CornerRadius=$mcr"}},
    ThemeTargetStyles{L"WindowsInternal.ComposableShell.Experiences.Switcher.NewVirtualDesktopElementThemed#NewVirtualDesktopButtonThemed > Windows.UI.Xaml.Controls.Grid#MainGrid > Windows.UI.Xaml.Controls.Border#BorderHighlight", {
        L"CornerRadius=$mcr"}},
    ThemeTargetStyles{L"WindowsInternal.ComposableShell.Experiences.Switcher.VirtualDesktopThumbnailButton#ThumbnailButtonElement", {
        L"CornerRadius=$bcr"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Button#VirtualDesktopElementCloseButton", {
        L"CornerRadius=$bcr"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Border#SnapBarBorder", {
        L"Background:=$mbg",
        L"CornerRadius=$mcr",
        L"BorderThickness=$bt",
        L"BorderBrush=$bb",
        L"Shadow:="}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Border#SnapPickerBorder", {
        L"Background:=$mbg",
        L"CornerRadius=$mcr",
        L"BorderThickness=$bt",
        L"BorderBrush=$bb"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.FlyoutPresenter > Border", {
        L"Shadow:="}},
    ThemeTargetStyles{L"MenuFlyoutPresenter", {
        L"CornerRadius=$mcr",
        L"Shadow:="}},
    ThemeTargetStyles{L"MenuFlyoutPresenter > Border", {
        L"Background:=$mbg",
        L"CornerRadius=$mcr",
        L"BorderThickness=$bt",
        L"BorderBrush=$bb"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.MenuFlyoutItem", {
        L"CornerRadius=$bcr"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.MenuFlyoutSubItem", {
        L"CornerRadius=$bcr"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.ToolTip > Windows.UI.Xaml.Controls.ContentPresenter#LayoutRoot", {
        L"Background:=$mbg",
        L"CornerRadius=$mcr",
        L"BorderThickness=$bt",
        L"BorderBrush=$bb",
        L"Shadow:="}},
    ThemeTargetStyles{L"ScrollViewer#MenuFlyoutPresenterScrollViewer > Border > Grid > ScrollContentPresenter > ItemsPresenter > StackPanel", {
        L"ChildrenTransitions:=<TransitionCollection><EntranceThemeTransition $AnimationSettings /></TransitionCollection>"}},
    ThemeTargetStyles{L"Grid#LayoutRoot", {
        L"BackgroundTransition:=<BrushTransition Duration=\"0:0:0.100\" />"}},
    ThemeTargetStyles{L"Border#BackgroundBorder", {
        L"BackgroundTransition:=<BrushTransition Duration=\"0:0:0.100\" />"}},
    ThemeTargetStyles{L"Taskbar.TaskbarFrame > Grid#RootGrid > Taskbar.TaskbarBackground > Grid > Rectangle#BackgroundFill", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Taskbar.TaskbarFrame", {
        L"Margin=$DockMargin,0,$DockMargin,0"}},
    ThemeTargetStyles{L"Taskbar.TaskbarFrame > Grid#RootGrid", {
        L"Background:=$mbg",
        L"BorderThickness=$bt",
        L"BorderBrush=$bb",
        L"CornerRadius=$mcr",
        L"Margin=0,$DockTopGap,$DockMarginFix,5"}},
    ThemeTargetStyles{L"Taskbar.TaskbarBackground#BackgroundControl > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Shapes.Rectangle#BackgroundStroke", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Taskbar.TaskListButtonPanel#ExperienceToggleButtonRootPanel > Windows.UI.Xaml.Controls.Grid#AugmentedEntryPointContentGrid", {
        L"RenderTransform:=<TranslateTransform X=\"0\" Y=\"-1\" />"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Border#LargeTicker1", {
        L"Margin=0,0,0,-2"}},
    ThemeTargetStyles{L"Taskbar.AugmentedEntryPointButton#AugmentedEntryPointButton", {
        L"Margin=0,0,$WidgetGap57,0"}},
    ThemeTargetStyles{L"Grid#SystemTrayFrameGrid", {
        L"Margin=-$DockMargin,$DockTrayMarginUp,$DockMargin,$DockTrayMarginDown",
        L"HorizontalAlignment=Right"}},
    ThemeTargetStyles{L"Taskbar.TaskbarFrame", {
        L"Height=$DockHeight"}},
}, {
    L"DockMargin=250",
    L"DockMarginFix=500",
    L"DockHeight=58",
    L"DockTopGap=5",
    L"DockTrayMarginUp=1",
    L"DockTrayMarginDown=1",
    L"WidgetGap=-",
    L"AccentColor=<SolidColorBrush Color=\"{ThemeResource SystemAccentColorLight2}\" Opacity=\"1.0\" />",
    L"AnimationSettings=IsStaggeringEnabled=\"True\" FromHorizontalOffset=\"-50\" FromVerticalOffset=\"50\"",
    L"mbg=<WindhawkBlur BlurAmount=\"30\" TintColor=\"{ThemeResource CardStrokeColorDefaultSolid}\" TintOpacity=\"0.0\" TintLuminosityOpacity=\"1.0\" TintSaturation=\"1.0\" NoiseDensity=\"1.0\" NoiseOpacity=\"0.1\" />",
    L"bcr=10",
    L"wcr=20",
    L"mcr=15",
    L"t=Transparent",
    L"bb=#20FFFFFF",
    L"bt=1",
    L"nbb=<LinearGradientBrush x:Key=\"ShellTaskbarItemGradientStrokeColorSecondaryBrush\" MappingMode=\"Absolute\" StartPoint=\"0,0\" EndPoint=\"0,3\"><LinearGradientBrush.GradientStops><GradientStop Offset=\"0.33\" Color=\"#1AFFFFFF\" /><GradientStop Offset=\"1\" Color=\"#0FFFFFFF\" /></LinearGradientBrush.GradientStops></LinearGradientBrush>",
    L"nbt=<SolidColorBrush Color=\"{ThemeResource ControlFillColorDefault}\" />",
    L"nbth=<SolidColorBrush Color=\"{ThemeResource ControlFillColorSecondary}\" />",
    L"nbtp=<SolidColorBrush Color=\"{ThemeResource ControlFillColorTertiary}\" />",
}};

const Theme g_themeLuminosity_variant_Classic = {{
    ThemeTargetStyles{L"Taskbar.TaskbarFrame > Grid#RootGrid > Taskbar.TaskbarBackground > Grid > Rectangle#BackgroundFill", {
        L"Fill:=$mbg"}},
    ThemeTargetStyles{L"Taskbar.TaskListButtonPanel#ExperienceToggleButtonRootPanel > Windows.UI.Xaml.Controls.Border#BackgroundElement", {
        L"CornerRadius=$bcr"}},
    ThemeTargetStyles{L"Taskbar.ExperienceToggleButton", {
        L"CornerRadius=$bcr"}},
    ThemeTargetStyles{L"Taskbar.TaskListButton", {
        L"CornerRadius=$bcr"}},
    ThemeTargetStyles{L"SearchUx.SearchUI.SearchButtonRootGrid#SearchBoxButtonRootPanel > Windows.UI.Xaml.Controls.Border#BackgroundElement", {
        L"CornerRadius=$bcr"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Border#SearchPillBackgroundElement", {
        L"CornerRadius=$bcr"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Border#MultiWindowElement", {
        L"CornerRadius=$bcr"}},
    ThemeTargetStyles{L"SystemTray.ChevronIconView > Windows.UI.Xaml.Controls.Grid#ContainerGrid > Windows.UI.Xaml.Controls.Border#BackgroundBorder", {
        L"CornerRadius=$bcr",
        L"Margin=0,4,2,4"}},
    ThemeTargetStyles{L"SystemTray.NotifyIconView > Windows.UI.Xaml.Controls.Grid#ContainerGrid > Windows.UI.Xaml.Controls.Border#BackgroundBorder", {
        L"CornerRadius=$bcr",
        L"Margin=2,4,2,4"}},
    ThemeTargetStyles{L"SystemTray.IconView#SystemTrayIcon > Windows.UI.Xaml.Controls.Grid#ContainerGrid > Windows.UI.Xaml.Controls.Border#BackgroundBorder", {
        L"CornerRadius=$bcr",
        L"Margin=2,4,2,4"}},
    ThemeTargetStyles{L"SystemTray.OmniButton#ControlCenterButton > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.Border#BackgroundBorder", {
        L"CornerRadius=$bcr",
        L"Margin=2,4,2,4"}},
    ThemeTargetStyles{L"SystemTray.OmniButton#NotificationCenterButton > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.Border#BackgroundBorder", {
        L"CornerRadius=$bcr",
        L"Margin=2,4,2,4"}},
    ThemeTargetStyles{L"Border#OverflowFlyoutBackgroundBorder", {
        L"Background:=$mbg",
        L"CornerRadius:=$wcr",
        L"BorderThickness=$bt",
        L"BorderBrush=$bb",
        L"Shadow:="}},
    ThemeTargetStyles{L"Taskbar.OverflowToggleButton#OverflowButton > Taskbar.TaskListButtonPanel#OverflowToggleButtonRootPanel > Windows.UI.Xaml.Controls.Border#BackgroundElement", {
        L"CornerRadius=$bcr"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#ConfirmatorMainGrid", {
        L"Background:=$mbg",
        L"CornerRadius=$wcr",
        L"BorderThickness=$bt",
        L"BorderBrush=$bb",
        L"Shadow:="}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Shapes.Rectangle#HorizontalTrackRect", {
        L"Fill:=#10FFFFFF"}},
    ThemeTargetStyles{L"WindowsInternal.ComposableShell.Experiences.TextInput.Common.InputSwitcher > ContentControl > ContentPresenter > Grid", {
        L"Background:=$mbg",
        L"CornerRadius=$wcr",
        L"BorderThickness=$bt",
        L"BorderBrush=$bb",
        L"Shadow:="}},
    ThemeTargetStyles{L"WindowsInternal.ComposableShell.Experiences.TextInput.Common.InputSwitcher > ContentControl > ContentPresenter > Grid > Grid", {
        L"Background:=$t"}},
    ThemeTargetStyles{L"Taskbar.TaskbarBackground#HoverFlyoutBackgroundControl > Grid > Rectangle#BackgroundFill", {
        L"Fill:=$t"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#HoverFlyoutGrid > Windows.UI.Xaml.Controls.Border#HoverFlyoutBackground", {
        L"Background:=$mbg",
        L"CornerRadius=$mcr",
        L"BorderThickness=$bt",
        L"BorderBrush=$bb",
        L"Shadow:="}},
    ThemeTargetStyles{L"Taskbar.TaskItemThumbnailView > Grid@CommonStates > Border#BackgroundBorder", {
        L"Background=$t",
        L"CornerRadius=$mcr",
        L"BorderThickness@Normal=0",
        L"BorderThickness@PointerOver=0.05,0,0.05,1",
        L"BorderBrush@Normal=$t",
        L"BorderBrush@PointerOver:=$AccentColor"}},
    ThemeTargetStyles{L"Taskbar.TaskItemThumbnailView > Grid > Button#CloseButton", {
        L"CornerRadius=$mcr"}},
    ThemeTargetStyles{L"Taskbar.ThumbBarButton#ThumbBarButton > Windows.UI.Xaml.Controls.ContentPresenter#BorderElement@CommonStates", {
        L"CornerRadius=16",
        L"Margin=-1.5",
        L"Background@Disabled:=$t",
        L"Background@Normal:=$t",
        L"Background@PointerOver:=$nbth",
        L"Background@Pressed:=$nbtp",
        L"BorderThickness=2",
        L"BorderBrush@Disabled:=$t",
        L"BorderBrush@Normal:=$t",
        L"BorderBrush@PointerOver:=$nbb",
        L"BorderBrush@Pressed:=$nbb",
        L"BackgroundSizing=InnerBorderEdge",
        L"BackgroundTransition:=<BrushTransition Duration=\"0:0:0.200\" />"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#ModalRootGrid > Windows.UI.Xaml.Controls.Border#BackgroundElement", {
        L"Background=$t",
        L"CornerRadius=$wcr",
        L"BorderThickness=$bt",
        L"BorderBrush=$bb",
        L"Shadow:="}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#ModalRootGrid > Windows.UI.Xaml.Controls.Border#BackgroundElement > WindowsInternal.ComposableShell.Experiences.Switcher.SwitchItemList", {
        L"Background:=$mbg"}},
    ThemeTargetStyles{L"WindowsInternal.ComposableShell.Experiences.Switcher.DynamicFlowPanel > WindowsInternal.ComposableShell.Experiences.Switcher.SwitchItemListViewItem > Windows.UI.Xaml.Controls.Grid#Root@CommonStates > Windows.UI.Xaml.Controls.Border#BackgroundBorder", {
        L"Background:=#09FFFFFF",
        L"CornerRadius=$mcr",
        L"BorderThickness=0.05,1,0.05,0",
        L"BorderBrush@Normal=$t",
        L"BorderBrush@PointerOver:=$AccentColor"}},
    ThemeTargetStyles{L"WindowsInternal.ComposableShell.Experiences.Switcher.SwitchItemControl > Grid#Root > WindowsInternal.ComposableShell.Experiences.Switcher.SwitchItemThumbnailButton#ThumbnailHost > Windows.UI.Xaml.Controls.Grid#RootGrid", {
        L"CornerRadius=$bcr",
        L"Margin=5"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Border#BackgroundDimmingLayer", {
        L"Background:=$t"}},
    ThemeTargetStyles{L"WindowsInternal.ComposableShell.Experiences.Switcher.VirtualDesktopBarElement > Windows.UI.Xaml.Controls.Grid#GridElement > Windows.UI.Xaml.Controls.Border#VirtualDesktopSwitcherBackground", {
        L"CornerRadius=$wcr",
        L"BorderThickness=$bt",
        L"BorderBrush=$bb",
        L"Margin=-2,1,-1,2"}},
    ThemeTargetStyles{L"WindowsInternal.ComposableShell.Experiences.Switcher.DynamicFlowPanel#DFCPanel > WindowsInternal.ComposableShell.Experiences.Switcher.SwitchItemListViewItem > Windows.UI.Xaml.Controls.Grid#Root@CommonStates > Windows.UI.Xaml.Controls.Border#BackgroundBorder", {
        L"Background:=$mbg",
        L"CornerRadius=$wcr,$wcr,$bcr,$bcr",
        L"Margin:=-5,0,-5,-5",
        L"BorderThickness=0.05,1,0.05,0",
        L"BorderBrush@Normal=$t",
        L"BorderBrush@PointerOver:=$AccentColor"}},
    ThemeTargetStyles{L"WindowsInternal.ComposableShell.Experiences.Switcher.SwitchItemElement > Windows.UI.Xaml.Controls.Grid#RootGrid > Windows.UI.Xaml.Controls.Grid#TitleGrid > Windows.UI.Xaml.Controls.Border#BackgroundBorder", {
        L"Background:=$t"}},
    ThemeTargetStyles{L"WindowsInternal.ComposableShell.Experiences.Switcher.SwitchItemElement > Windows.UI.Xaml.Controls.Grid#RootGrid > Windows.UI.Xaml.Controls.Grid#TitleGrid > Image#IconImage", {
        L"RenderTransform:=<TranslateTransform X=\"0\" Y=\"1\" />"}},
    ThemeTargetStyles{L"WindowsInternal.ComposableShell.Experiences.Switcher.SwitchItemElement > Windows.UI.Xaml.Controls.Grid#RootGrid > Windows.UI.Xaml.Controls.Grid#TitleGrid > TextBlock#DisplayName", {
        L"RenderTransform:=<TranslateTransform X=\"0\" Y=\"1\" />"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Button#SwitchItemElementCloseButton > ContentPresenter#ContentPresenter", {
        L"CornerRadius=$mcr",
        L"Margin=5"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Button#SwitchItemElementCloseButton > ContentPresenter#ContentPresenter > TextBlock", {
        L"RenderTransform:=<TranslateTransform X=\"-0.8\" Y=\"-0.6\" />"}},
    ThemeTargetStyles{L"WindowsInternal.ComposableShell.Experiences.Switcher.SwitchItemElement > Grid#RootGrid > WindowsInternal.ComposableShell.Experiences.Switcher.SwitchItemThumbnailButton#ThumbnailHost > Grid#RootGrid", {
        L"CornerRadius=0"}},
    ThemeTargetStyles{L"WindowsInternal.ComposableShell.Experiences.Switcher.VirtualDesktopBarElement#VirtualDesktopBar > Grid > Border", {
        L"Background:=$mbg",
        L"CornerRadius=$wcr",
        L"BorderThickness=$bt",
        L"BorderBrush=$bb",
        L"Margin=-1,2,0,0"}},
    ThemeTargetStyles{L"WindowsInternal.ComposableShell.Experiences.Switcher.VirtualDesktopBarElement#VirtualDesktopBar", {
        L"Width=Auto",
        L"HorizontalAlignment=1"}},
    ThemeTargetStyles{L"WindowsInternal.ComposableShell.Experiences.Switcher.VirtualDesktopBarElement > Grid > Border", {
        L"Background:=$mbg",
        L"Shadow:="}},
    ThemeTargetStyles{L"WindowsInternal.ComposableShell.Experiences.Switcher.VirtualDesktopElementThemed > Windows.UI.Xaml.Controls.Grid#MainGrid > Windows.UI.Xaml.Controls.Border#MainBorder", {
        L"CornerRadius=$mcr"}},
    ThemeTargetStyles{L"WindowsInternal.ComposableShell.Experiences.Switcher.VirtualDesktopElementThemed > Windows.UI.Xaml.Controls.Grid#MainGrid > Windows.UI.Xaml.Controls.Border#BorderHighlight", {
        L"CornerRadius=$mcr"}},
    ThemeTargetStyles{L"WindowsInternal.ComposableShell.Experiences.Switcher.NewVirtualDesktopElementThemed#NewVirtualDesktopButtonThemed > Windows.UI.Xaml.Controls.Grid#MainGrid", {
        L"CornerRadius=$mcr",
        L"BorderThickness=$bt",
        L"Margin=2"}},
    ThemeTargetStyles{L"WindowsInternal.ComposableShell.Experiences.Switcher.NewVirtualDesktopElementThemed#NewVirtualDesktopButtonThemed > Windows.UI.Xaml.Controls.Grid#MainGrid > Windows.UI.Xaml.Controls.Border#MainBorder", {
        L"CornerRadius=$mcr"}},
    ThemeTargetStyles{L"WindowsInternal.ComposableShell.Experiences.Switcher.NewVirtualDesktopElementThemed#NewVirtualDesktopButtonThemed > Windows.UI.Xaml.Controls.Grid#MainGrid > Windows.UI.Xaml.Controls.Border#BorderHighlight", {
        L"CornerRadius=$mcr"}},
    ThemeTargetStyles{L"WindowsInternal.ComposableShell.Experiences.Switcher.VirtualDesktopThumbnailButton#ThumbnailButtonElement", {
        L"CornerRadius=$bcr"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Button#VirtualDesktopElementCloseButton", {
        L"CornerRadius=$bcr"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Border#SnapBarBorder", {
        L"Background:=$mbg",
        L"CornerRadius=$mcr",
        L"BorderThickness=$bt",
        L"BorderBrush=$bb",
        L"Shadow:="}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Border#SnapPickerBorder", {
        L"Background:=$mbg",
        L"CornerRadius=$mcr",
        L"BorderThickness=$bt",
        L"BorderBrush=$bb"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.FlyoutPresenter > Border", {
        L"Shadow:="}},
    ThemeTargetStyles{L"MenuFlyoutPresenter", {
        L"CornerRadius=$mcr",
        L"Shadow:="}},
    ThemeTargetStyles{L"MenuFlyoutPresenter > Border", {
        L"Background:=$mbg",
        L"CornerRadius=$mcr",
        L"BorderThickness=$bt",
        L"BorderBrush=$bb"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.MenuFlyoutItem", {
        L"CornerRadius=$bcr"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.MenuFlyoutSubItem", {
        L"CornerRadius=$bcr"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.ToolTip > Windows.UI.Xaml.Controls.ContentPresenter#LayoutRoot", {
        L"Background:=$mbg",
        L"CornerRadius=$mcr",
        L"BorderThickness=$bt",
        L"BorderBrush=$bb",
        L"Shadow:="}},
    ThemeTargetStyles{L"ScrollViewer#MenuFlyoutPresenterScrollViewer > Border > Grid > ScrollContentPresenter > ItemsPresenter > StackPanel", {
        L"ChildrenTransitions:=<TransitionCollection><EntranceThemeTransition $AnimationSettings /></TransitionCollection>"}},
    ThemeTargetStyles{L"Grid#LayoutRoot", {
        L"BackgroundTransition:=<BrushTransition Duration=\"0:0:0.100\" />"}},
    ThemeTargetStyles{L"Border#BackgroundBorder", {
        L"BackgroundTransition:=<BrushTransition Duration=\"0:0:0.100\" />"}},
}, {
    L"AccentColor=<SolidColorBrush Color=\"{ThemeResource SystemAccentColorLight2}\" Opacity=\"1.0\" />",
    L"AnimationSettings=IsStaggeringEnabled=\"True\" FromHorizontalOffset=\"-50\" FromVerticalOffset=\"50\"",
    L"mbg=<WindhawkBlur BlurAmount=\"30\" TintColor=\"{ThemeResource CardStrokeColorDefaultSolid}\" TintOpacity=\"0.0\" TintLuminosityOpacity=\"1.0\" TintSaturation=\"1.0\" NoiseDensity=\"1.0\" NoiseOpacity=\"0.1\" />",
    L"bcr=10",
    L"wcr=20",
    L"mcr=15",
    L"t=Transparent",
    L"bb=#20FFFFFF",
    L"bt=1",
    L"nbb=<LinearGradientBrush x:Key=\"ShellTaskbarItemGradientStrokeColorSecondaryBrush\" MappingMode=\"Absolute\" StartPoint=\"0,0\" EndPoint=\"0,3\"><LinearGradientBrush.GradientStops><GradientStop Offset=\"0.33\" Color=\"#1AFFFFFF\" /><GradientStop Offset=\"1\" Color=\"#0FFFFFFF\" /></LinearGradientBrush.GradientStops></LinearGradientBrush>",
    L"nbt=<SolidColorBrush Color=\"{ThemeResource ControlFillColorDefault}\" />",
    L"nbth=<SolidColorBrush Color=\"{ThemeResource ControlFillColorSecondary}\" />",
    L"nbtp=<SolidColorBrush Color=\"{ThemeResource ControlFillColorTertiary}\" />",
}};

const Theme g_themeLuminosity_variant_Compact = {{
    ThemeTargetStyles{L"Taskbar.TaskbarFrame > Grid#RootGrid > Taskbar.TaskbarBackground > Grid > Rectangle#BackgroundFill", {
        L"Fill:=$mbg"}},
    ThemeTargetStyles{L"Taskbar.AugmentedEntryPointButton#AugmentedEntryPointButton", {
        L"Margin=0,0,$WidgetGap57,0"}},
    ThemeTargetStyles{L"Taskbar.TaskListButtonPanel#ExperienceToggleButtonRootPanel > Windows.UI.Xaml.Controls.Border#BackgroundElement", {
        L"CornerRadius=$bcr"}},
    ThemeTargetStyles{L"Taskbar.ExperienceToggleButton", {
        L"CornerRadius=$bcr"}},
    ThemeTargetStyles{L"Taskbar.TaskListButton", {
        L"CornerRadius=$bcr"}},
    ThemeTargetStyles{L"SearchUx.SearchUI.SearchButtonRootGrid#SearchBoxButtonRootPanel > Windows.UI.Xaml.Controls.Border#BackgroundElement", {
        L"CornerRadius=$bcr"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Border#SearchPillBackgroundElement", {
        L"CornerRadius=$bcr"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Border#MultiWindowElement", {
        L"CornerRadius=$bcr"}},
    ThemeTargetStyles{L"SystemTray.ChevronIconView > Windows.UI.Xaml.Controls.Grid#ContainerGrid > Windows.UI.Xaml.Controls.Border#BackgroundBorder", {
        L"CornerRadius=$bcr",
        L"Margin=0,4,2,4"}},
    ThemeTargetStyles{L"SystemTray.NotifyIconView > Windows.UI.Xaml.Controls.Grid#ContainerGrid > Windows.UI.Xaml.Controls.Border#BackgroundBorder", {
        L"CornerRadius=$bcr",
        L"Margin=2,4,2,4"}},
    ThemeTargetStyles{L"SystemTray.IconView#SystemTrayIcon > Windows.UI.Xaml.Controls.Grid#ContainerGrid > Windows.UI.Xaml.Controls.Border#BackgroundBorder", {
        L"CornerRadius=$bcr",
        L"Margin=2,4,2,4"}},
    ThemeTargetStyles{L"SystemTray.OmniButton#ControlCenterButton > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.Border#BackgroundBorder", {
        L"CornerRadius=$bcr",
        L"Margin=2,4,2,4"}},
    ThemeTargetStyles{L"SystemTray.OmniButton#NotificationCenterButton > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.Border#BackgroundBorder", {
        L"CornerRadius=$bcr",
        L"Margin=2,4,2,4"}},
    ThemeTargetStyles{L"Border#OverflowFlyoutBackgroundBorder", {
        L"Background:=$mbg",
        L"CornerRadius:=$wcr",
        L"BorderThickness=$bt",
        L"BorderBrush=$bb",
        L"Shadow:="}},
    ThemeTargetStyles{L"Taskbar.OverflowToggleButton#OverflowButton > Taskbar.TaskListButtonPanel#OverflowToggleButtonRootPanel > Windows.UI.Xaml.Controls.Border#BackgroundElement", {
        L"CornerRadius=$bcr"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#ConfirmatorMainGrid", {
        L"Background:=$mbg",
        L"CornerRadius=$wcr",
        L"BorderThickness=$bt",
        L"BorderBrush=$bb",
        L"Shadow:="}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Shapes.Rectangle#HorizontalTrackRect", {
        L"Fill:=#10FFFFFF"}},
    ThemeTargetStyles{L"WindowsInternal.ComposableShell.Experiences.TextInput.Common.InputSwitcher > ContentControl > ContentPresenter > Grid", {
        L"Background:=$mbg",
        L"CornerRadius=$wcr",
        L"BorderThickness=$bt",
        L"BorderBrush=$bb",
        L"Shadow:="}},
    ThemeTargetStyles{L"WindowsInternal.ComposableShell.Experiences.TextInput.Common.InputSwitcher > ContentControl > ContentPresenter > Grid > Grid", {
        L"Background:=$t"}},
    ThemeTargetStyles{L"Taskbar.TaskbarBackground#HoverFlyoutBackgroundControl > Grid > Rectangle#BackgroundFill", {
        L"Fill:=$t"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#HoverFlyoutGrid > Windows.UI.Xaml.Controls.Border#HoverFlyoutBackground", {
        L"Background:=$mbg",
        L"CornerRadius=$mcr",
        L"BorderThickness=$bt",
        L"BorderBrush=$bb",
        L"Shadow:="}},
    ThemeTargetStyles{L"Taskbar.TaskItemThumbnailView > Grid@CommonStates > Border#BackgroundBorder", {
        L"Background=$t",
        L"CornerRadius=$mcr",
        L"BorderThickness@Normal=0",
        L"BorderThickness@PointerOver=0.05,0,0.05,1",
        L"BorderBrush@Normal=$t",
        L"BorderBrush@PointerOver:=$AccentColor"}},
    ThemeTargetStyles{L"Taskbar.TaskItemThumbnailView > Grid > Button#CloseButton", {
        L"CornerRadius=$mcr"}},
    ThemeTargetStyles{L"Taskbar.ThumbBarButton#ThumbBarButton > Windows.UI.Xaml.Controls.ContentPresenter#BorderElement@CommonStates", {
        L"CornerRadius=16",
        L"Margin=-1.5",
        L"Background@Disabled:=$t",
        L"Background@Normal:=$t",
        L"Background@PointerOver:=$nbth",
        L"Background@Pressed:=$nbtp",
        L"BorderThickness=2",
        L"BorderBrush@Disabled:=$t",
        L"BorderBrush@Normal:=$t",
        L"BorderBrush@PointerOver:=$nbb",
        L"BorderBrush@Pressed:=$nbb",
        L"BackgroundSizing=InnerBorderEdge",
        L"BackgroundTransition:=<BrushTransition Duration=\"0:0:0.200\" />"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#ModalRootGrid > Windows.UI.Xaml.Controls.Border#BackgroundElement", {
        L"Background=$t",
        L"CornerRadius=$wcr",
        L"BorderThickness=$bt",
        L"BorderBrush=$bb",
        L"Shadow:="}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#ModalRootGrid > Windows.UI.Xaml.Controls.Border#BackgroundElement > WindowsInternal.ComposableShell.Experiences.Switcher.SwitchItemList", {
        L"Background:=$mbg"}},
    ThemeTargetStyles{L"WindowsInternal.ComposableShell.Experiences.Switcher.DynamicFlowPanel > WindowsInternal.ComposableShell.Experiences.Switcher.SwitchItemListViewItem > Windows.UI.Xaml.Controls.Grid#Root@CommonStates > Windows.UI.Xaml.Controls.Border#BackgroundBorder", {
        L"Background:=#09FFFFFF",
        L"CornerRadius=$mcr",
        L"BorderThickness=0.05,1,0.05,0",
        L"BorderBrush@Normal=$t",
        L"BorderBrush@PointerOver:=$AccentColor"}},
    ThemeTargetStyles{L"WindowsInternal.ComposableShell.Experiences.Switcher.SwitchItemControl > Grid#Root > WindowsInternal.ComposableShell.Experiences.Switcher.SwitchItemThumbnailButton#ThumbnailHost > Windows.UI.Xaml.Controls.Grid#RootGrid", {
        L"CornerRadius=$bcr",
        L"Margin=5"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Border#BackgroundDimmingLayer", {
        L"Background:=$t"}},
    ThemeTargetStyles{L"WindowsInternal.ComposableShell.Experiences.Switcher.VirtualDesktopBarElement > Windows.UI.Xaml.Controls.Grid#GridElement > Windows.UI.Xaml.Controls.Border#VirtualDesktopSwitcherBackground", {
        L"CornerRadius=$wcr",
        L"BorderThickness=$bt",
        L"BorderBrush=$bb",
        L"Margin=-2,1,-1,2"}},
    ThemeTargetStyles{L"WindowsInternal.ComposableShell.Experiences.Switcher.DynamicFlowPanel#DFCPanel > WindowsInternal.ComposableShell.Experiences.Switcher.SwitchItemListViewItem > Windows.UI.Xaml.Controls.Grid#Root@CommonStates > Windows.UI.Xaml.Controls.Border#BackgroundBorder", {
        L"Background:=$mbg",
        L"CornerRadius=$wcr,$wcr,$bcr,$bcr",
        L"Margin:=-5,0,-5,-5",
        L"BorderThickness=0.05,1,0.05,0",
        L"BorderBrush@Normal=$t",
        L"BorderBrush@PointerOver:=$AccentColor"}},
    ThemeTargetStyles{L"WindowsInternal.ComposableShell.Experiences.Switcher.SwitchItemElement > Windows.UI.Xaml.Controls.Grid#RootGrid > Windows.UI.Xaml.Controls.Grid#TitleGrid > Windows.UI.Xaml.Controls.Border#BackgroundBorder", {
        L"Background:=$t"}},
    ThemeTargetStyles{L"WindowsInternal.ComposableShell.Experiences.Switcher.SwitchItemElement > Windows.UI.Xaml.Controls.Grid#RootGrid > Windows.UI.Xaml.Controls.Grid#TitleGrid > Image#IconImage", {
        L"RenderTransform:=<TranslateTransform X=\"0\" Y=\"1\" />"}},
    ThemeTargetStyles{L"WindowsInternal.ComposableShell.Experiences.Switcher.SwitchItemElement > Windows.UI.Xaml.Controls.Grid#RootGrid > Windows.UI.Xaml.Controls.Grid#TitleGrid > TextBlock#DisplayName", {
        L"RenderTransform:=<TranslateTransform X=\"0\" Y=\"1\" />"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Button#SwitchItemElementCloseButton > ContentPresenter#ContentPresenter", {
        L"CornerRadius=$mcr",
        L"Margin=5"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Button#SwitchItemElementCloseButton > ContentPresenter#ContentPresenter > TextBlock", {
        L"RenderTransform:=<TranslateTransform X=\"-0.8\" Y=\"-0.6\" />"}},
    ThemeTargetStyles{L"WindowsInternal.ComposableShell.Experiences.Switcher.SwitchItemElement > Grid#RootGrid > WindowsInternal.ComposableShell.Experiences.Switcher.SwitchItemThumbnailButton#ThumbnailHost > Grid#RootGrid", {
        L"CornerRadius=0"}},
    ThemeTargetStyles{L"WindowsInternal.ComposableShell.Experiences.Switcher.VirtualDesktopBarElement#VirtualDesktopBar > Grid > Border", {
        L"Background:=$mbg",
        L"CornerRadius=$wcr",
        L"BorderThickness=$bt",
        L"BorderBrush=$bb",
        L"Margin=-1,2,0,0"}},
    ThemeTargetStyles{L"WindowsInternal.ComposableShell.Experiences.Switcher.VirtualDesktopBarElement#VirtualDesktopBar", {
        L"Width=Auto",
        L"HorizontalAlignment=1"}},
    ThemeTargetStyles{L"WindowsInternal.ComposableShell.Experiences.Switcher.VirtualDesktopBarElement > Grid > Border", {
        L"Background:=$mbg",
        L"Shadow:="}},
    ThemeTargetStyles{L"WindowsInternal.ComposableShell.Experiences.Switcher.VirtualDesktopElementThemed > Windows.UI.Xaml.Controls.Grid#MainGrid > Windows.UI.Xaml.Controls.Border#MainBorder", {
        L"CornerRadius=$mcr"}},
    ThemeTargetStyles{L"WindowsInternal.ComposableShell.Experiences.Switcher.VirtualDesktopElementThemed > Windows.UI.Xaml.Controls.Grid#MainGrid > Windows.UI.Xaml.Controls.Border#BorderHighlight", {
        L"CornerRadius=$mcr"}},
    ThemeTargetStyles{L"WindowsInternal.ComposableShell.Experiences.Switcher.NewVirtualDesktopElementThemed#NewVirtualDesktopButtonThemed > Windows.UI.Xaml.Controls.Grid#MainGrid", {
        L"CornerRadius=$mcr",
        L"BorderThickness=$bt",
        L"Margin=2"}},
    ThemeTargetStyles{L"WindowsInternal.ComposableShell.Experiences.Switcher.NewVirtualDesktopElementThemed#NewVirtualDesktopButtonThemed > Windows.UI.Xaml.Controls.Grid#MainGrid > Windows.UI.Xaml.Controls.Border#MainBorder", {
        L"CornerRadius=$mcr"}},
    ThemeTargetStyles{L"WindowsInternal.ComposableShell.Experiences.Switcher.NewVirtualDesktopElementThemed#NewVirtualDesktopButtonThemed > Windows.UI.Xaml.Controls.Grid#MainGrid > Windows.UI.Xaml.Controls.Border#BorderHighlight", {
        L"CornerRadius=$mcr"}},
    ThemeTargetStyles{L"WindowsInternal.ComposableShell.Experiences.Switcher.VirtualDesktopThumbnailButton#ThumbnailButtonElement", {
        L"CornerRadius=$bcr"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Button#VirtualDesktopElementCloseButton", {
        L"CornerRadius=$bcr"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Border#SnapBarBorder", {
        L"Background:=$mbg",
        L"CornerRadius=$mcr",
        L"BorderThickness=$bt",
        L"BorderBrush=$bb",
        L"Shadow:="}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Border#SnapPickerBorder", {
        L"Background:=$mbg",
        L"CornerRadius=$mcr",
        L"BorderThickness=$bt",
        L"BorderBrush=$bb"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.FlyoutPresenter > Border", {
        L"Shadow:="}},
    ThemeTargetStyles{L"MenuFlyoutPresenter", {
        L"CornerRadius=$mcr",
        L"Shadow:="}},
    ThemeTargetStyles{L"MenuFlyoutPresenter > Border", {
        L"Background:=$mbg",
        L"CornerRadius=$mcr",
        L"BorderThickness=$bt",
        L"BorderBrush=$bb"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.MenuFlyoutItem", {
        L"CornerRadius=$bcr"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.MenuFlyoutSubItem", {
        L"CornerRadius=$bcr"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.ToolTip > Windows.UI.Xaml.Controls.ContentPresenter#LayoutRoot", {
        L"Background:=$mbg",
        L"CornerRadius=$mcr",
        L"BorderThickness=$bt",
        L"BorderBrush=$bb",
        L"Shadow:="}},
    ThemeTargetStyles{L"ScrollViewer#MenuFlyoutPresenterScrollViewer > Border > Grid > ScrollContentPresenter > ItemsPresenter > StackPanel", {
        L"ChildrenTransitions:=<TransitionCollection><EntranceThemeTransition $AnimationSettings /></TransitionCollection>"}},
    ThemeTargetStyles{L"Grid#LayoutRoot", {
        L"BackgroundTransition:=<BrushTransition Duration=\"0:0:0.100\" />"}},
    ThemeTargetStyles{L"Border#BackgroundBorder", {
        L"BackgroundTransition:=<BrushTransition Duration=\"0:0:0.100\" />"}},
    ThemeTargetStyles{L"Taskbar.TaskbarFrame", {
        L"Height=30"}},
    ThemeTargetStyles{L"Taskbar.TaskListButtonPanel#ExperienceToggleButtonRootPanel > Windows.UI.Xaml.Controls.Grid#AugmentedEntryPointContentGrid", {
        L"RenderTransform:=<TranslateTransform X=\"0\" Y=\"-1\" />"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Border#LargeTicker1", {
        L"Margin=1,-5,0,0",
        L"RenderTransform:=<ScaleTransform ScaleX=\"0.75\" ScaleY=\"0.75\" />"}},
    ThemeTargetStyles{L"SearchUx.SearchUI.SearchButtonRootGrid#SearchBoxButtonRootPanel > Windows.UI.Xaml.Controls.Border#BackgroundElement", {
        L"Margin=0,4,0,4"}},
    ThemeTargetStyles{L"SearchUx.SearchUI.SearchButtonControl", {
        L"Margin=0,-4,0,-4"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.AnimatedVisualPlayer#Icon", {
        L"Width=16",
        L"Height=16"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Image#Icon", {
        L"Width=16",
        L"Height=16"}},
    ThemeTargetStyles{L"Taskbar.TaskListButton#TaskListButton > Taskbar.TaskListLabeledButtonPanel#IconPanel > Windows.UI.Xaml.Controls.TextBlock#LabelControl", {
        L"RenderTransform:=<TranslateTransform X=\"0\" Y=\"-1\" />"}},
    ThemeTargetStyles{L"Taskbar.AugmentedEntryPointButton#AugmentedEntryPointButton", {
        L"Margin=0,0,$WidgetGap57,0"}},
    ThemeTargetStyles{L"Grid#SystemTrayFrameGrid", {
        L"Margin=0,0,0,18"}},
    ThemeTargetStyles{L"SystemTray.TextIconContent > Grid#ContainerGrid > SystemTray.AdaptiveTextBlock#Base > TextBlock#InnerTextBlock", {
        L"FontSize=14"}},
    ThemeTargetStyles{L"SystemTray.ImageIconContent > Grid#ContainerGrid > Image", {
        L"Width=14",
        L"Height=14"}},
}, {
    L"WidgetGap=-",
    L"AccentColor=<SolidColorBrush Color=\"{ThemeResource SystemAccentColorLight2}\" Opacity=\"1.0\" />",
    L"AnimationSettings=IsStaggeringEnabled=\"True\" FromHorizontalOffset=\"-50\" FromVerticalOffset=\"50\"",
    L"mbg=<WindhawkBlur BlurAmount=\"30\" TintColor=\"{ThemeResource CardStrokeColorDefaultSolid}\" TintOpacity=\"0.0\" TintLuminosityOpacity=\"1.0\" TintSaturation=\"1.0\" NoiseDensity=\"1.0\" NoiseOpacity=\"0.1\" />",
    L"bcr=10",
    L"wcr=20",
    L"mcr=15",
    L"t=Transparent",
    L"bb=#20FFFFFF",
    L"bt=1",
    L"nbb=<LinearGradientBrush x:Key=\"ShellTaskbarItemGradientStrokeColorSecondaryBrush\" MappingMode=\"Absolute\" StartPoint=\"0,0\" EndPoint=\"0,3\"><LinearGradientBrush.GradientStops><GradientStop Offset=\"0.33\" Color=\"#1AFFFFFF\" /><GradientStop Offset=\"1\" Color=\"#0FFFFFFF\" /></LinearGradientBrush.GradientStops></LinearGradientBrush>",
    L"nbt=<SolidColorBrush Color=\"{ThemeResource ControlFillColorDefault}\" />",
    L"nbth=<SolidColorBrush Color=\"{ThemeResource ControlFillColorSecondary}\" />",
    L"nbtp=<SolidColorBrush Color=\"{ThemeResource ControlFillColorTertiary}\" />",
}};

const Theme g_themeLayerMicaUI = {{
    ThemeTargetStyles{L"Border#BackgroundDimmingLayer", {
        L"Background:=$ThemeBlur"}},
    ThemeTargetStyles{L"Border#OverflowFlyoutBackgroundBorder", {
        L"Background:=$ThemeLayer",
        L"CornerRadius=$OuterRadius",
        L"BorderBrush:=$ThemeOutBorder",
        L"BorderThickness=1"}},
    ThemeTargetStyles{L"Border#VirtualDesktopBarBackground", {
        L"Background:=$ThemeLayer",
        L"CornerRadius=$OuterRadius",
        L"BorderBrush:=$ThemeOutBorder"}},
    ThemeTargetStyles{L"Border#SnapPickerBorder", {
        L"Background:=$ThemeBlur"}},
    ThemeTargetStyles{L"Border#SnapBarBorder", {
        L"Background:=$ThemeBlur"}},
    ThemeTargetStyles{L"Grid#SystemTrayFrameGrid", {
        L"Background:=$ThemeLayer",
        L"CornerRadius=$OuterRadius",
        L"Margin=0,3,8,3",
        L"Padding=4,0,0,0",
        L"BorderThickness=1"}},
    ThemeTargetStyles{L"Grid#ModalRootGrid > Border#BackgroundElement", {
        L"Background=Transparent",
        L"CornerRadius=$OuterRadius",
        L"BorderBrush:=$ThemeOutBorder"}},
    ThemeTargetStyles{L"Grid#ModalRootGrid > Border#BackgroundElement > WindowsInternal.ComposableShell.Experiences.Switcher.SwitchItemList", {
        L"Background:=$ThemeBlur"}},
    ThemeTargetStyles{L"Grid#ConfirmatorMainGrid", {
        L"Background:=$ThemeLayer",
        L"CornerRadius=$OuterRadius",
        L"BorderThickness=1",
        L"BorderBrush:=$ThemeOutBorder",
        L"Height=46"}},
    ThemeTargetStyles{L"Grid#HorizontalTemplate > Rectangle#HorizontalDecreaseRect", {
        L"Height=7.5",
        L"RadiusX=3",
        L"RadiusY=3"}},
    ThemeTargetStyles{L"Grid#HorizontalTemplate > Rectangle#HorizontalTrackRect", {
        L"Height=7.5",
        L"RadiusX=3",
        L"RadiusY=3",
        L"Opacity=0.5"}},
    ThemeTargetStyles{L"Grid#VolumeConfirmator > Slider#volumeSlider", {
        L"Margin=0,-3.5,0,0",
        L"Width=120"}},
    ThemeTargetStyles{L"Grid#BrightnessConfirmator > Slider#brightnessSlider", {
        L"Width=135",
        L"Margin=0,-2.5,0,0"}},
    ThemeTargetStyles{L"Grid#DynamicSearchBoxGleamContainer", {
        L"Height=30"}},
    ThemeTargetStyles{L"Grid > HyperlinkButton#Footer", {
        L"HorizontalContentAlignment=1"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.AnimatedIcon#BrightnessIcon", {
        L"Height=22",
        L"Width=22"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.AnimatedIcon#VolumeIcon", {
        L"Height=18",
        L"Width=18"}},
    ThemeTargetStyles{L"SystemTray.NotifyIconView#NotifyItemIcon[AutomationProperties.Name=Bluetooth Devices] > Grid#ContainerGrid > ContentPresenter#ContentPresenter > Grid#ContentGrid > SystemTray.ImageIconContent > Grid#ContainerGrid > Image", {
        L"Visibility=1"}},
    ThemeTargetStyles{L"SystemTray.NotifyIconView#NotifyItemIcon[AutomationProperties.Name=Safely Remove Hardware and Eject Media] > Grid#ContainerGrid > ContentPresenter#ContentPresenter > Grid#ContentGrid > SystemTray.ImageIconContent > Grid#ContainerGrid > Image", {
        L"Visibility=1"}},
    ThemeTargetStyles{L"SystemTray.NotifyIconView#NotifyItemIcon[AutomationProperties.Name=Bluetooth Devices] > Grid#ContainerGrid > ContentPresenter#ContentPresenter", {
        L"Content:=<FontIcon FontFamily=\"Segoe Fluent Icons\" Glyph=\"&#xE702;\" FontSize=\"16\"/>",
        L"Foreground:=<SolidColorBrush Color=\"{ThemeResource Accent1}\" />",
        L"Canvas.ZIndex=-1"}},
    ThemeTargetStyles{L"SystemTray.NotifyIconView#NotifyItemIcon[AutomationProperties.Name=Safely Remove Hardware and Eject Media] > Grid#ContainerGrid > ContentPresenter#ContentPresenter", {
        L"Content:=<FontIcon FontFamily=\"Segoe Fluent Icons\" Glyph=\"&#xE88E;\" FontSize=\"16\"/>",
        L"Foreground:=<SolidColorBrush Color=\"{ThemeResource Accent1}\" />",
        L"Canvas.ZIndex=-1"}},
    ThemeTargetStyles{L"SystemTray.OmniButton#ControlCenterButton", {
        L"Grid.Column=4",
        L"CornerRadius=$InnerRadius"}},
    ThemeTargetStyles{L"SystemTray.OmniButton#NotificationCenterButton", {
        L"Grid.Column=5",
        L"CornerRadius=$InnerRadius"}},
    ThemeTargetStyles{L"SystemTray.Stack#MainStack", {
        L"Grid.Column=6",
        L"CornerRadius=$InnerRadius"}},
    ThemeTargetStyles{L"SystemTray.Stack#ShowDesktopStack", {
        L"CornerRadius=$InnerRadius",
        L"Grid.Column=7",
        L"Width=5"}},
    ThemeTargetStyles{L"SystemTray.Stack#NonActivatableStack", {
        L"Grid.Column=2"}},
    ThemeTargetStyles{L"Taskbar.ExperienceToggleButton#LaunchListButton[AutomationProperties.AutomationId=TaskViewButton] > Taskbar.TaskListButtonPanel#ExperienceToggleButtonRootPanel", {
        L"CornerRadius=$InnerRadius",
        L"Width=44"}},
    ThemeTargetStyles{L"Taskbar.TaskItemThumbnailView > Grid > Border", {
        L"CornerRadius=$OuterRadius",
        L"Background:=$ThemeLayer",
        L"BorderThickness=1",
        L"BorderBrush:=$ThemeOutBorder"}},
    ThemeTargetStyles{L"Taskbar.TaskItemThumbnailView > Grid > TextBlock#DisplayNameTextBlock", {
        L"FontFamily=$ThFnt",
        L"FontWeight=Bold"}},
    ThemeTargetStyles{L"Taskbar.TaskListButton", {
        L"CornerRadius=$InnerRadius",
        L"Margin=0,-1,0,-1",
        L"BorderBrush:=$ThemeControlBorder",
        L"BorderThickness=1"}},
    ThemeTargetStyles{L"Taskbar.TaskbarBackground#BackgroundControl", {
        L"Opacity=0"}},
    ThemeTargetStyles{L"Taskbar.TaskbarBackground#HoverFlyoutBackgroundControl > Grid > Rectangle#BackgroundFill", {
        L"CornerRadius=$OuterRadius"}},
    ThemeTargetStyles{L"Taskbar.TaskbarBackground#HoverFlyoutBackgroundControl", {
        L"FontFamily=$ThFnt"}},
    ThemeTargetStyles{L"Taskbar.TaskbarFrame", {
        L"Width=Auto"}},
    ThemeTargetStyles{L"Taskbar.TaskbarFrame > Grid#RootGrid", {
        L"Margin=8,3,0,3",
        L"CornerRadius=$OuterRadius",
        L"Padding=-8,0,0,0",
        L"Background:=$ThemeLayer",
        L"BorderThickness=1",
        L"HorizontalAlignment=Left"}},
    ThemeTargetStyles{L"TextBlock#InnerTextBlock[Text=\uE971]", {
        L"Text=\uF0E2"}},
    ThemeTargetStyles{L"TextBlock#volumeLevelText", {
        L"FontSize=15",
        L"Margin=0,-1,0,0",
        L"FontFamily=$ThFnt",
        L"FontWeight=$ThFntWt"}},
    ThemeTargetStyles{L"TextBlock#BatteryTextBlock", {
        L"FontFamily=$ThFnt",
        L"FontSize=12",
        L"FontWeight=Bold"}},
    ThemeTargetStyles{L"ToolTip > ContentPresenter#LayoutRoot", {
        L"Background:=$ThemeFlyout",
        L"CornerRadius=$OuterRadius",
        L"BorderBrush:=$ThemeOutBorder",
        L"BorderThickness=1"}},
    ThemeTargetStyles{L"ToolTip > ContentPresenter#LayoutRoot > TextBlock", {
        L"FontFamily=$ThFnt",
        L"FontWeight=$ThFntWt",
        L"FontSize=13"}},
    ThemeTargetStyles{L"WindowsInternal.ComposableShell.Experiences.Switcher.NewVirtualDesktopElementThemed#NewVirtualDesktopButtonThemed > Grid#MainGrid > Border#MainBorder", {
        L"CornerRadius=$OuterRadius",
        L"Background:=$ThemeOverlay"}},
    ThemeTargetStyles{L"WindowsInternal.ComposableShell.Experiences.Switcher.NewVirtualDesktopElementThemed#NewVirtualDesktopButtonThemed > Grid#MainGrid > Border#BorderHighlight", {
        L"CornerRadius=$OuterRadius"}},
    ThemeTargetStyles{L"WindowsInternal.ComposableShell.Experiences.Switcher.SwitchItemElement > Grid#RootGrid > Grid#TitleGrid > Border#BackgroundBorder", {
        L"CornerRadius=$OuterRadius,$OuterRadius,0,0"}},
    ThemeTargetStyles{L"WindowsInternal.ComposableShell.Experiences.Switcher.SwitchItemElement > Grid#RootGrid > Grid#TitleGrid", {
        L"FontFamily=$ThFnt",
        L"FontWeight=$ThFntWt"}},
    ThemeTargetStyles{L"WindowsInternal.ComposableShell.Experiences.Switcher.SwitchItemListViewItem > Grid#Root > Border#BackgroundBorder", {
        L"CornerRadius=$OuterRadius",
        L"Background:=$ThemeLayer",
        L"BorderBrush:=$ThemeControlBorder",
        L"BorderThickness=1"}},
    ThemeTargetStyles{L"WindowsInternal.ComposableShell.Experiences.Switcher.VirtualDesktopElementThemed > Grid#MainGrid > Border#MainBorder", {
        L"CornerRadius=$OuterRadius",
        L"Background:=$ThemeOverlay"}},
    ThemeTargetStyles{L"WindowsInternal.ComposableShell.Experiences.Switcher.VirtualDesktopElementThemed > Grid#MainGrid > Border#BorderHighlight", {
        L"CornerRadius=$OuterRadius"}},
    ThemeTargetStyles{L"WindowsInternal.ComposableShell.Experiences.Switcher.VirtualDesktopElementThemed > Grid#MainGrid > Border#ActiveDesktopPill", {
        L"CornerRadius=$OuterRadius"}},
    ThemeTargetStyles{L"WindowsInternal.ComposableShell.Experiences.TextInput.Common.InputSwitcher > ContentControl > ContentPresenter > Grid", {
        L"Background:=$ThemeLayer",
        L"BorderBrush:=$ThemeOutBorder",
        L"Shadow:=",
        L"CornerRadius=$OuterRadius"}},
    ThemeTargetStyles{L"WindowsInternal.ComposableShell.Experiences.TextInput.Common.InputSwitcher > ContentControl > ContentPresenter > Grid > Grid#OverlayPanel", {
        L"Background=Transparent",
        L"BorderBrush=Transparent"}},
    ThemeTargetStyles{L"Button#GleamEntryPointButton > Border", {
        L"Height=30"}},
    ThemeTargetStyles{L"ContentPresenter > SearchUx.SearchUI.SearchButtonControl > Grid > SearchUx.SearchUI.SearchBoxButton#SearchBox > SearchUx.SearchUI.SearchButtonRootGrid#SearchBoxButtonRootPanel > TextBlock#SearchBoxTextBlock", {
        L"FontFamily=$ThFnt",
        L"FontWeight=$ThFntWt",
        L"FontSize=14",
        L"RenderTransform:=<TranslateTransform Y=\"1\" />"}},
    ThemeTargetStyles{L"MenuFlyoutPresenter", {
        L"Background:=$ThemeFlyout",
        L"CornerRadius=$OuterRadius",
        L"FontFamily=$ThFnt",
        L"FontWeight=$ThFntWt",
        L"BorderThickness=1",
        L"BorderBrush:=$ThemeOutBorder"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.ItemsRepeater#TaskbarFrameRepeater", {
        L"Margin=0,0,2,0"}},
    ThemeTargetStyles{L"Rectangle#BackgroundStroke", {
        L"Visibility=1"}},
    ThemeTargetStyles{L"ScrollViewer > ScrollContentPresenter > Border > Grid", {
        L"Background:=<WindhawkBlur BlurAmount=\"5\" TintColor=\"{ThemeResource SystemChromeMediumColor}\" TintSaturation=\"2\" NoiseOpacity=\"0.19\" NoiseDensity=\"0.8\" TintOpacity=\"0.2\" TintLuminosityOpacity=\"0\" />"}},
    ThemeTargetStyles{L"SearchUx.SearchUI.SearchBoxButton#SearchBox", {
        L"Margin=0,-1,0,0"}},
    ThemeTargetStyles{L"SearchUx.SearchUI.SearchButtonRootGrid@CommonStates > Border#BackgroundElement", {
        L"Background:=$ThemeIsland",
        L"BorderBrush:=$ThemeOutBorder",
        L"BorderThickness=1",
        L"CornerRadius=$InnerRadius",
        L"Margin=0,-2.5,0,-2.5"}},
    ThemeTargetStyles{L"SearchUx.SearchUI.SearchPillButton#SearchPill > SearchUx.SearchUI.SearchButtonRootGrid#SearchBoxButtonRootPanel > Border#SearchPillBackgroundElement", {
        L"CornerRadius=$InnerRadius",
        L"Height=26",
        L"BorderBrush:=$ThemeBorder",
        L"Background:=$ThemeOverlay"}},
    ThemeTargetStyles{L"SystemTray.NotificationAreaIcons#NotificationAreaIcons", {
        L"Grid.Column=1",
        L"CornerRadius=$InnerRadius"}},
    ThemeTargetStyles{L"SystemTray.SystemTrayFrame", {
        L"HorizontalAlignment=Right"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Hosting.DesktopWindowXamlSource", {
        L"Background:=$ThemeBlur",
        L"CornerRadius=$OuterRadius",
        L"BorderBrush:=$ThemeOutBorder"}},
    ThemeTargetStyles{L"WindowsInternal.ComposableShell.Experiences.Switcher.VirtualDesktopBarElement > Grid#GridElement > Border#VirtualDesktopSwitcherBackground", {
        L"Background:=$ThemeLayer"}},
}, {
    L"ThemeBlur=<WindhawkBlur BlurAmount=\"12\" TintColor=\"{ThemeResource SystemChromeMediumColor}\" TintSaturation=\"1.25\" NoiseOpacity=\"0.1\" NoiseDensity=\"0.8\" TintOpacity=\"0.2\" TintLuminosityOpacity=\"0.3\" />",
    L"ThemeLayer=<AcrylicBrush BackgroundSource=\"Backdrop\" TintColor=\"{ThemeResource SystemChromeMediumColor}\" TintOpacity=\"{ThemeResource bgOpacity}\" TintLuminosityOpacity=\"{ThemeResource bgLuminosity}\" FallbackColor=\"{ThemeResource SystemChromeMediumColor}\" />",
    L"OuterRadius=10",
    L"InnerRadius=8",
    L"ThemeBorder=<SolidColorBrush Color=\"{ThemeResource Border}\"/>",
    L"ThFnt=Nunito",
    L"ThemeOutBorder=<SolidColorBrush Color=\"#56757575\"/>",
    L"ThemeOverlay=<SolidColorBrush Color=\"{ThemeResource Overlay}\" />",
    L"ThemeIsland=<SolidColorBrush Color=\"{ThemeResource Island}\" />",
    L"ThemeControlBorder=<SolidColorBrush Color=\"{ThemeResource ControlBorder}\" />",
    L"ThFntWt=Normal",
    L"ThemeFlyout=<AcrylicBrush BackgroundSource=\"Backdrop\" TintColor=\"{ThemeResource SystemChromeMediumColor}\" TintOpacity=\"0.1\" TintLuminosityOpacity=\"0.8\" FallbackColor=\"{ThemeResource SystemChromeMediumColor}\" />",
}, {
    L"Accent1@Dark={ThemeResource SystemAccentColorLight3}",
    L"Accent1@Light={ThemeResource SystemAccentColorDark2}",
    L"bgOpacity@Dark=0.3",
    L"bgOpacity@Light=0.2",
    L"bgLuminosity@Light=1",
    L"bgLuminosity@Dark=0.9",
    L"Overlay@Light=#40FFFFFF",
    L"Overlay@Dark=#09FFFFFF",
    L"Border@Light=#0F000000",
    L"Border@Dark=#19000000",
    L"ControlBorder@Light=#0F000000",
    L"ControlBorder@Dark=#15FFFFFF",
    L"Island@Light=#5FFFFFFF",
    L"Island@Dark=#10FFFFFF",
}};

const Theme g_themeFluid = {{
    ThemeTargetStyles{L"Rectangle#BackgroundStroke", {
        L"Visibility=1"}},
    ThemeTargetStyles{L"Grid#OverflowRootGrid > Border", {
        L"CornerRadius=$CornerRadius"}},
    ThemeTargetStyles{L"Rectangle#RunningIndicator", {
        L"Fill:=<AcrylicBrush TintColor=\"{ThemeResource SystemChromeAltMedColor}\" TintOpacity=\"0.5\" />"}},
    ThemeTargetStyles{L"TextBlock#InnerTextBlock[Text=\uE971]", {
        L"Text=\uED14"}},
    ThemeTargetStyles{L"Border#BackgroundElement", {
        L"CornerRadius=$CornerRadius"}},
    ThemeTargetStyles{L"Border#BackgroundDimmingLayer", {
        L"CornerRadius=$CornerRadius"}},
    ThemeTargetStyles{L"WindowsInternal.ComposableShell.Experiences.Switcher.VirtualDesktopBarElement#VirtualDesktopBar", {
        L"CornerRadius=$CornerRadius"}},
    ThemeTargetStyles{L"MenuFlyoutPresenter", {
        L"CornerRadius=$CornerRadius"}},
    ThemeTargetStyles{L"Grid#LayoutRoot", {
        L"BackgroundTransition:=<BrushTransition Duration=\"0:0:0.083\" />"}},
    ThemeTargetStyles{L"Border#BackgroundBorder", {
        L"BackgroundTransition:=<BrushTransition Duration=\"0:0:0.083\" />"}},
    ThemeTargetStyles{L"Taskbar.AugmentedEntryPointButton#AugmentedEntryPointButton > Taskbar.TaskListButtonPanel#ExperienceToggleButtonRootPanel > Border#BackgroundElement@CommonStates", {
        L"Background@ActiveNormal:=$NormalBG",
        L"Background@ActivePointerOver:=$Hover",
        L"Background@ActivePressed:=$PressedBG",
        L"Background@InactivePointerOver:=$Hover",
        L"Background@InactivePressed:=$PressedBG",
        L"BorderBrush@ActiveNormal:=$BorderBrush",
        L"BorderBrush@ActivePointerOver:=$BorderBrush",
        L"BorderBrush@ActivePressed:=$BorderBrush",
        L"BorderBrush@InactivePointerOver:=$BorderBrush",
        L"BorderBrush@InactivePressed:=$BorderBrush",
        L"BackgroundTransition:=<BrushTransition Duration=\"0:0:0.083\" />",
        L"BackgroundSizing=InnerBorderEdge",
        L"Margin=1",
        L"CornerRadius=$CornerRadius",
        L"BorderThickness=$BorderThickness"}},
    ThemeTargetStyles{L"Taskbar.TaskListLabeledButtonPanel@CommonStates > Border#BackgroundElement", {
        L"Background@ActiveNormal:=$NormalBG",
        L"Background@ActivePointerOver:=$Hover",
        L"Background@ActivePressed:=$PressedBG",
        L"Background@InactivePointerOver:=$Hover",
        L"Background@InactivePressed:=$PressedBG",
        L"Background@MultiWindowNormal:=$NormalBG",
        L"Background@MultiWindowActive:=$NormalBG",
        L"Background@MultiWindowPointerOver:=$Hover",
        L"Background@MultiWindowPressed:=$PressedBG",
        L"BorderBrush@ActiveNormal:=$BorderBrush",
        L"BorderBrush@ActivePointerOver:=$BorderBrush",
        L"BorderBrush@ActivePressed:=$BorderBrush",
        L"BorderBrush@InactivePointerOver:=$BorderBrush",
        L"BorderBrush@InactivePressed:=$BorderBrush",
        L"BorderBrush@MultiWindowNormal:=$BorderBrush",
        L"BorderBrush@MultiWindowActive:=$BorderBrush",
        L"BorderBrush@MultiWindowPointerOver:=$BorderBrush",
        L"BorderBrush@MultiWindowPressed:=$BorderBrush",
        L"BackgroundTransition:=<BrushTransition Duration=\"0:0:0.083\" />",
        L"BackgroundSizing=InnerBorderEdge",
        L"Margin=2",
        L"BorderThickness=$BorderThickness"}},
    ThemeTargetStyles{L"Border#MultiWindowElement", {
        L"BorderBrush:=$BorderBrush",
        L"Background:=$NormalBG",
        L"BackgroundTransition:=<BrushTransition Duration=\"0:0:0.083\" />",
        L"BackgroundSizing=InnerBorderEdge",
        L"BorderThickness=$BorderThickness",
        L"CornerRadius=$CornerRadius",
        L"Margin=2"}},
    ThemeTargetStyles{L"ContentPresenter#ContentPresenter@CommonStates", {
        L"Background@ActiveNormal:=$NormalBG",
        L"Background@ActivePointerOver:=$Hover",
        L"Background@ActivePressed:=$PressedBG",
        L"Background@InactivePointerOver:=$Hover",
        L"Background@InactivePressed:=$PressedBG",
        L"BorderBrush@ActiveNormal:=$BorderBrush",
        L"BorderBrush@ActivePointerOver:=$BorderBrush",
        L"BorderBrush@ActivePressed:=$BorderBrush",
        L"BorderBrush@InactivePointerOver:=$BorderBrush",
        L"BorderBrush@InactivePressed:=$BorderBrush",
        L"BackgroundTransition:=<BrushTransition Duration=\"0:0:0.083\" />",
        L"BackgroundSizing=InnerBorderEdge",
        L"CornerRadius=$CornerRadius",
        L"BorderThickness=$BorderThickness",
        L"Margin=2"}},
    ThemeTargetStyles{L"ContentPresenter#ContentPresenter > Grid#ContentGrid > Microsoft.UI.Xaml.Controls.AnimatedVisualPlayer#LottieIcon", {
        L"Visibility=1"}},
    ThemeTargetStyles{L"SystemTray.CopilotIcon#CopilotIcon > Grid#ContainerGrid > Border#BackgroundBorder", {
        L"Visibility=1"}},
}, {
    L"BorderBrush=<LinearGradientBrush x:Key=\"ShellTaskbarItemGradientStrokeColorSecondaryBrush\" MappingMode=\"Absolute\" StartPoint=\"0,0\" EndPoint=\"0,3\"><LinearGradientBrush.GradientStops><GradientStop Offset=\"0.33\" Color=\"{ThemeResource ControlFillColorSecondary}\" /><GradientStop Offset=\"1\" Color=\"{ThemeResource ControlFillColorTertiary}\" /></LinearGradientBrush.GradientStops></LinearGradientBrush>",
    L"NormalBG=<SolidColorBrush Color=\"{ThemeResource ControlFillColorDefault}\" />",
    L"HoverBG=<SolidColorBrush Color=\"{ThemeResource ControlFillColorSecondary}\" />",
    L"PressedBG=<SolidColorBrush Color=\"{ThemeResource ControlFillColorTertiary}\" />",
    L"BorderThickness=1",
    L"CornerRadius=4",
}};

const Theme g_themeTintedGlass = {{
    ThemeTargetStyles{L"Taskbar.TaskbarFrame > Grid#RootGrid > Taskbar.TaskbarBackground > Grid > Rectangle#BackgroundFill", {
        L"Fill:=$CommonBgBrush"}},
    ThemeTargetStyles{L"Taskbar.TaskbarBackground#HoverFlyoutBackgroundControl > Grid > Rectangle#BackgroundFill", {
        L"Fill:=$CommonBgBrush"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#ModalRootGrid > Windows.UI.Xaml.Controls.Border#BackgroundElement", {
        L"Background=Transparent"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#ModalRootGrid > Windows.UI.Xaml.Controls.Border#BackgroundElement > WindowsInternal.ComposableShell.Experiences.Switcher.SwitchItemList", {
        L"Background:=$CommonBgBrush"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Border#BackgroundDimmingLayer", {
        L"Background:=$CommonBgBrush"}},
    ThemeTargetStyles{L"MenuFlyoutPresenter > Border", {
        L"Fill:=$CommonBgBrush",
        L"BorderThickness=0,0,0,0",
        L"CornerRadius=14",
        L"Padding=2,2,2,2"}},
    ThemeTargetStyles{L"Border#OverflowFlyoutBackgroundBorder", {
        L"Fill:=$CommonBgBrush",
        L"BorderThickness=0,0,0,0",
        L"CornerRadius=14",
        L"Margin=-2,-2,-2,-2"}},
    ThemeTargetStyles{L"SystemTray.AdaptiveTextBlock#Base > TextBlock#InnerTextBlock", {
        L"FontSize=18"}},
    ThemeTargetStyles{L"SystemTray.ImageIconContent > Grid#ContainerGrid > Image", {
        L"Width=18",
        L"Height=18"}},
    ThemeTargetStyles{L"SystemTray.Stack#ShowDesktopStack", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Taskbar.ExperienceToggleButton#LaunchListButton[AutomationProperties.AutomationId=StartButton] > Taskbar.TaskListButtonPanel > Microsoft.UI.Xaml.Controls.AnimatedVisualPlayer#Icon", {
        L"Height=32",
        L"Width=32"}},
    ThemeTargetStyles{L"Taskbar.TaskListLabeledButtonPanel#IconPanel", {
        L"Padding=2,2,2,2"}},
    ThemeTargetStyles{L"Taskbar.TaskListButtonPanel#ExperienceToggleButtonRootPanel", {
        L"Padding=2,2,2,2"}},
    ThemeTargetStyles{L"Grid#ContainerGrid", {
        L"Padding=2,2,2,2"}},
    ThemeTargetStyles{L"Taskbar.FlyoutFrame > Canvas#HoverFlyoutCanvas > Grid#HoverFlyoutGrid", {
        L"Padding=2,2,2,2"}},
    ThemeTargetStyles{L"Image#Icon", {
        L"Margin=2,2,2,2"}},
    ThemeTargetStyles{L"Rectangle#BackgroundStroke", {
        L"Fill:=<WindhawkBlur BlurAmount=\"18\" TintColor=\"#1AFFFFFF\"/>"}},
    ThemeTargetStyles{L"Grid#OverflowRootGrid > Border", {
        L"Background:=$CommonBgBrush"}},
    ThemeTargetStyles{L"Grid#ConfirmatorMainGrid", {
        L"Background:=$CommonBgBrush",
        L"BorderThickness=0"}},
    ThemeTargetStyles{L"WindowsInternal.ComposableShell.Experiences.TextInput.Common.InputSwitcher > ContentControl > ContentPresenter > Grid", {
        L"Background:=$CommonBgBrush",
        L"BorderThickness=0"}},
    ThemeTargetStyles{L"WindowsInternal.ComposableShell.Experiences.TextInput.Common.InputSwitcher > ContentControl > ContentPresenter > Grid > Grid", {
        L"Fill:=<WindhawkBlur BlurAmount=\"18\" TintColor=\"#1AFFFFFF\"/>"}},
}, {
    L"CommonBgBrush=<WindhawkBlur BlurAmount=\"18\" TintColor=\"#80000000\"/>",
}};

const Theme g_themeTaskbarToStatusbar = {{
    ThemeTargetStyles{L"Taskbar.TaskbarFrame > Grid#RootGrid > Microsoft.UI.Xaml.Controls.ItemsRepeater#TaskbarFrameRepeater", {
        L"Width=Auto",
        L"HorizontalAlignment=Left"}},
    ThemeTargetStyles{L"Taskbar.ExperienceToggleButton#LaunchListButton[AutomationProperties.AutomationId=TaskViewButton] > Taskbar.TaskListButtonPanel#ExperienceToggleButtonRootPanel", {
        L"Width=3840",
        L"Margin=0,0,-3840,0"}},
    ThemeTargetStyles{L"Taskbar.ExperienceToggleButton#LaunchListButton > Taskbar.TaskListButtonPanel > Microsoft.UI.Xaml.Controls.AnimatedVisualPlayer#Icon", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Taskbar.TaskListButtonPanel@CommonStates > Border#BackgroundElement", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Taskbar.SearchBoxButton#SearchBoxButton[AutomationProperties.AutomationId=SearchButton] > Taskbar.TaskListButtonPanel", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Taskbar.TaskbarExtensionElement", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Taskbar.AugmentedEntryPointButton#AugmentedEntryPointButton", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Taskbar.TaskListLabeledButtonPanel#IconPanel", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Taskbar.TaskListButtonPanel#OverflowToggleButtonRootPanel", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"SystemTray.SystemTrayFrame", {
        L"HorizontalAlignment=Center"}},
    ThemeTargetStyles{L"SystemTray.Stack#ShowDesktopStack", {
        L"Width=1920",
        L"Margin=0,0,-1920,0"}},
    ThemeTargetStyles{L"SystemTray.StackListView#IconStack[AutomationProperties.AutomationId=ShowDesktop] > ItemsPresenter > StackPanel > ContentPresenter > SystemTray.IconView#SystemTrayIcon", {
        L"Width=1920"}},
    ThemeTargetStyles{L"SystemTray.IconView#SystemTrayIcon > Grid#ContainerGrid@ > Rectangle#ShowDesktopPipe", {
        L"Fill=Transparent"}},
    ThemeTargetStyles{L"SystemTray.OmniButton#ControlCenterButton > Grid > ContentPresenter > ItemsPresenter > StackPanel > ContentPresenter > SystemTray.IconView#SystemTrayIcon > Grid#ContainerGrid", {
        L"Padding=4,0,4,0"}},
    ThemeTargetStyles{L"SystemTray.OmniButton#NotificationCenterButton > Grid > ContentPresenter > ItemsPresenter > StackPanel > ContentPresenter > SystemTray.IconView#SystemTrayIcon > Grid#ContainerGrid", {
        L"Padding=4,0,4,0"}},
    ThemeTargetStyles{L"SystemTray.AdaptiveTextBlock#LanguageInnerTextBlock > Windows.UI.Xaml.Controls.TextBlock#InnerTextBlock", {
        L"TextTrimming=1",
        L"Height=20",
        L"FontSize=14"}},
    ThemeTargetStyles{L"TextBlock#BatteryTextBlock", {
        L"FontSize=14"}},
    ThemeTargetStyles{L"TextBlock#TimeInnerTextBlock", {
        L"FontSize=14"}},
    ThemeTargetStyles{L"TextBlock#DateInnerTextBlock", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Grid#SystemTrayFrameGrid", {
        L"Margin=0,-4,0,-4",
        L"Opacity=0.75"}},
    ThemeTargetStyles{L"Taskbar.TaskbarFrame > Grid#RootGrid > Taskbar.TaskbarBackground > Grid > Rectangle#BackgroundStroke", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Taskbar.TaskbarFrame > Grid#RootGrid > Taskbar.TaskbarBackground > Grid > Rectangle#BackgroundFill", {
        L"Fill:=<SolidColorBrush Color=\"{ThemeResource TextFillColorInverse}\"  Opacity=\"1.0\" />"}},
    ThemeTargetStyles{L"Taskbar.TaskbarFrame > Grid#RootGrid > Taskbar.TaskbarBackground > Grid", {
        L"Background=Black"}},
}};

const Theme g_themeUltraWideFriendly = {{
    ThemeTargetStyles{L":root > ScrollViewer > ScrollContentPresenter > Border > Grid", {
        L"ColumnDefinitions:=<ColumnDefinitionCollection><ColumnDefinition Width=\"*\"/><ColumnDefinition Width=\"Auto\"/><ColumnDefinition Width=\"Auto\"/><ColumnDefinition Width=\"*\"/></ColumnDefinitionCollection>",
        L"HorizontalAlignment=Stretch",
        L"Background:=<SolidColorBrush Color=\"$GhostBarBackgroundColor\"/>",
        L"ActualWidth=>containerGridWidth"}},
    ThemeTargetStyles{L"Taskbar.TaskbarFrame", {
        L"Grid.Column=1",
        L"Width=Auto",
        L"HorizontalAlignment=Right",
        L"Margin=0,0,$IslandHorizontalMargin,0",
        L"MaxWidth={{min($TaskbarFrameMaxWidth, containerGridWidth)}}"}},
    ThemeTargetStyles{L"Taskbar.TaskbarFrame > Grid", {
        L"Background:=<SolidColorBrush Color=\"$IslandBackgroundColor\"/>",
        L"CornerRadius=10",
        L"Padding=25,0,25,0",
        L"Margin=0,$IslandVerticalMargin,0,$IslandVerticalMargin"}},
    ThemeTargetStyles{L"SystemTray.SystemTrayFrame", {
        L"Grid.Column=2",
        L"Width=Auto",
        L"HorizontalAlignment=Left",
        L"Margin=$IslandHorizontalMargin,0,0,0"}},
    ThemeTargetStyles{L"Grid#SystemTrayFrameGrid", {
        L"Background:=<SolidColorBrush Color=\"$IslandBackgroundColor\"/>",
        L"CornerRadius=10",
        L"Padding=5,0,5,0",
        L"Margin=0,$IslandVerticalMargin,0,$IslandVerticalMargin"}},
    ThemeTargetStyles{L"Taskbar.TaskbarBackground#BackgroundControl", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Taskbar.TaskbarFrame > Grid#RootGrid > Taskbar.TaskbarBackground > Grid > Rectangle#BackgroundFill", {
        L"Fill=Transparent"}},
}, {
    L"GhostBarBackgroundColor=#66000000",
    L"IslandBackgroundColor={ThemeResource ControlFillColorDefault}",
    L"IslandVerticalMargin=3",
    L"IslandHorizontalMargin=5",
    L"TaskbarFrameMaxWidth=8000",
}};

const Theme g_themeLiquidGlass = {{
    ThemeTargetStyles{L"Taskbar.TaskbarFrame > Grid#RootGrid", {
        L"BorderThickness=$BorderThickness",
        L"BorderBrush:=$BorderBrush",
        L"Background:=$Background"}},
    ThemeTargetStyles{L"Rectangle#BackgroundFill", {
        L"Visibility=1"}},
    ThemeTargetStyles{L"Rectangle#BackgroundStroke", {
        L"Visibility=1"}},
    ThemeTargetStyles{L"Taskbar.AugmentedEntryPointButton#AugmentedEntryPointButton > Taskbar.TaskListButtonPanel#ExperienceToggleButtonRootPanel", {
        L"Margin=-3,0"}},
    ThemeTargetStyles{L"Grid#SystemTrayFrameGrid", {
        L"Background:=$ElementBackground",
        L"BorderBrush:=$ElementBorderBrush",
        L"BorderThickness=$ElementBorderThickness",
        L"CornerRadius=$ElementCornerRadius",
        L"Margin=6"}},
    ThemeTargetStyles{L"SystemTray.ChevronIconView", {
        L"CornerRadius=$ElementCornerRadius"}},
    ThemeTargetStyles{L"SystemTray.NotifyIconView#NotifyItemIcon", {
        L"CornerRadius=$ElementCornerRadius"}},
    ThemeTargetStyles{L"SystemTray.OmniButton", {
        L"CornerRadius=$ElementCornerRadius"}},
    ThemeTargetStyles{L"SystemTray.IconView#SystemTrayIcon > Grid#ContainerGrid > ContentPresenter#ContentPresenter > Grid#ContentGrid > SystemTray.TextIconContent > Grid#ContainerGrid", {
        L"CornerRadius=$ElementCornerRadius"}},
    ThemeTargetStyles{L"Taskbar.Gripper#GripperControl", {
        L"Width=Auto",
        L"MinWidth=24",
        L"HorizontalAlignment=Left"}},
    ThemeTargetStyles{L"TextBlock#TimeInnerTextBlock", {
        L"FontSize=13",
        L"Margin=0",
        L"Padding=0",
        L"RenderTransform:=<TranslateTransform X=\"0\" Y=\"0\" />"}},
    ThemeTargetStyles{L"TextBlock#DateInnerTextBlock", {
        L"Visibility=1"}},
    ThemeTargetStyles{L"TextBlock#InnerTextBlock[Text=\uE971]", {
        L"Text=\uED14"}},
    ThemeTargetStyles{L"TextBlock#SearchBoxTextBlock", {
        L"Text=Search This PC",
        L"FontSize=10"}},
    ThemeTargetStyles{L"SystemTray.OmniButton#NotificationCenterButton > Grid > ContentPresenter > ItemsPresenter > StackPanel > ContentPresenter > SystemTray.IconView#SystemTrayIcon > Grid > Grid > SystemTray.TextIconContent", {
        L"Visibility=1"}},
    ThemeTargetStyles{L"Border#OverflowFlyoutBackgroundBorder", {
        L"BorderThickness=$BorderThickness",
        L"BorderBrush:=$BorderBrush",
        L"Background:=$Background",
        L"CornerRadius=$CornerRadius"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#ModalRootGrid > Windows.UI.Xaml.Controls.Border#BackgroundElement", {
        L"BorderThickness=$BorderThickness",
        L"BorderBrush:=$BorderBrush",
        L"Background=Transparent",
        L"CornerRadius=$CornerRadius"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#ModalRootGrid > Windows.UI.Xaml.Controls.Border#BackgroundElement > WindowsInternal.ComposableShell.Experiences.Switcher.SwitchItemList", {
        L"Background:=$Background"}},
    ThemeTargetStyles{L"WindowsInternal.ComposableShell.Experiences.Switcher.VirtualDesktopBarElement#VirtualDesktopBar", {
        L"CornerRadius=$CornerRadius",
        L"Background:=$Background"}},
    ThemeTargetStyles{L"Border#BackgroundDimmingLayer", {
        L"Background:=$Background",
        L"CornerRadius=$CornerRadius"}},
    ThemeTargetStyles{L"Taskbar.TaskListButtonPanel#ExperienceToggleButtonRootPanel > Border#BackgroundElement", {
        L"CornerRadius=$CornerRadius",
        L"BorderThickness=$ElementBorderThickness"}},
    ThemeTargetStyles{L"Taskbar.TaskListButton#TaskListButton", {
        L"CornerRadius=$ElementCornerRadius",
        L"BorderThickness=$ElementBorderThickness"}},
    ThemeTargetStyles{L"Border#SnapBarBorder", {
        L"Background:=$Background",
        L"BorderBrush:=$BorderBrush",
        L"CornerRadius=$CornerRadius",
        L"BorderThickness=$BorderThickness",
        L"Margin=2"}},
    ThemeTargetStyles{L"Taskbar.TaskListLabeledButtonPanel@CommonStates > Border#BackgroundElement", {
        L"CornerRadius=$ElementCornerRadius",
        L"BorderThickness=$ElementBorderThickness",
        L"Background@ActiveNormal:=$ElementBackground",
        L"Background@ActivePointerOver:=$AccentBackground",
        L"Background@ActivePressed:=$ElementBackground2",
        L"Background@InactivePointerOver:=$AccentBackground",
        L"Background@InactivePressed:=$ElementBackground2",
        L"BorderBrush@ActiveNormal:=$ElementBorderBrush",
        L"BorderBrush@ActivePointerOver:=$ElementBorderBrush",
        L"BorderBrush@ActivePressed:=$ElementBorderBrush",
        L"BorderBrush@InactivePointerOver:=$ElementBorderBrush",
        L"BorderBrush@InactivePressed:=$ElementBorderBrush",
        L"Margin=1"}},
    ThemeTargetStyles{L"ContentPresenter#ContentPresenter@CommonStates", {
        L"CornerRadius=$ElementCornerRadius",
        L"BorderThickness=$ElementBorderThickness",
        L"Background@ActiveNormal:=$ElementBackground",
        L"Background@ActivePointerOver:=$AccentBackground",
        L"Background@ActivePressed:=$ElementBackground2",
        L"Background@InactivePointerOver:=$AccentBackground",
        L"Background@InactivePressed:=$ElementBackground2",
        L"BorderBrush@ActiveNormal:=$ElementBorderBrush",
        L"BorderBrush@ActivePointerOver:=$ElementBorderBrush",
        L"BorderBrush@ActivePressed:=$ElementBorderBrush",
        L"BorderBrush@InactivePointerOver:=$ElementBorderBrush",
        L"BorderBrush@InactivePressed:=$ElementBorderBrush",
        L"Margin=1"}},
    ThemeTargetStyles{L"Border#SnapPickerBorder", {
        L"Background:=$Background",
        L"BorderBrush:=$BorderBrush",
        L"CornerRadius=$CornerRadius",
        L"BorderThickness=$BorderThickness",
        L"Margin=2"}},
    ThemeTargetStyles{L"Taskbar.TaskListButtonPanel#ExperienceToggleButtonRootPanel", {
        L"Background:=Transparent"}},
    ThemeTargetStyles{L"ToolTip > ContentPresenter#LayoutRoot", {
        L"Background:=$Background",
        L"BorderBrush:=$BorderBrush",
        L"BorderThickness:=$BorderThickness",
        L"CornerRadius=$CornerRadius"}},
    ThemeTargetStyles{L"WindowsInternal.ComposableShell.Experiences.Switcher.VirtualDesktopBarElement > Grid#GridElement > Border#VirtualDesktopSwitcherBackground", {
        L"BorderBrush:=$BorderBrush",
        L"BorderThickness=$BorderThickness",
        L"CornerRadius=$CornerRadius",
        L"Background:=$Background"}},
    ThemeTargetStyles{L"WindowsInternal.ComposableShell.Experiences.Switcher.SwitchItemListViewItem > Grid > Border", {
        L"CornerRadius=$CornerRadius"}},
    ThemeTargetStyles{L"Border#VirtualDesktopBarBackground", {
        L"Background:=$Background",
        L"BorderBrush:=$BorderBrush",
        L"BorderThickness=$BorderThickness",
        L"CornerRadius=$CornerRadius"}},
    ThemeTargetStyles{L"Rectangle#RunningIndicator", {
        L"Fill:=$AccentBackground",
        L"Width=14"}},
    ThemeTargetStyles{L"Rectangle#ShowDesktopPipe", {
        L"Visibility=1"}},
    ThemeTargetStyles{L"Rectangle#RightOverflowButtonDivider", {
        L"Visibility=1"}},
    ThemeTargetStyles{L"SearchUx.SearchUI.SearchIconButton > SearchUx.SearchUI.SearchButtonRootGrid@CommonStates > Border#BackgroundElement", {
        L"Background:=Transparent",
        L"BorderBrush:=Transparent"}},
    ThemeTargetStyles{L"SearchUx.SearchUI.SearchButtonRootGrid", {
        L"Background:=Transparent",
        L"BorderBrush:=Transparent"}},
    ThemeTargetStyles{L"Border#SearchPillBackgroundElement", {
        L"BorderBrush:=$ElementBorderBrush",
        L"BorderThickness=$ElementBorderThickness",
        L"CornerRadius=$ElementCornerRadius",
        L"Margin=0,1"}},
    ThemeTargetStyles{L"SearchUx.SearchUI.SearchBoxButton > SearchUx.SearchUI.SearchButtonRootGrid@CommonStates > Border#BackgroundElement", {
        L"CornerRadius=$ElementCornerRadius",
        L"BorderThickness=$ElementBorderThickness",
        L"BorderBrush:=$ElementBorderBrush",
        L"Background:=$ElementBackground",
        L"Margin=0,-4"}},
    ThemeTargetStyles{L"Canvas#HoverFlyoutCanvas > Grid#HoverFlyoutGrid > Border#HoverFlyoutBackground", {
        L"Shadow:=",
        L"Background:=$Background",
        L"BorderBrush:=$BorderBrush",
        L"BorderThickness=$BorderThickness",
        L"CornerRadius=$CornerRadius"}},
    ThemeTargetStyles{L"SystemTray.SystemTrayFrame", {
        L"HorizontalAlignment=Right"}},
    ThemeTargetStyles{L"Grid#AugmentedEntryPointContentGrid", {
        L"HorizontalAlignment=Left"}},
}, {
    L"Background=<WindhawkBlur BlurAmount=\"15\" TintColor=\"{ThemeResource SystemAltLowColor}\" TintOpacity=\"0.2\" />",
    L"ElementBackground=<WindhawkBlur BlurAmount=\"20\" TintColor=\"{ThemeResource SystemAltLowColor}\" TintOpacity=\"0.4\" />",
    L"ElementBackground2=<WindhawkBlur BlurAmount=\"20\" TintColor=\"{ThemeResource SystemAltLowColor}\" TintOpacity=\"0.2\" />",
    L"AccentBackground=<WindhawkBlur BlurAmount=\"15\" TintColor=\"{ThemeResource SystemAccentColorLight1}\" TintOpacity=\"0.2\" />",
    L"BorderBrush=<LinearGradientBrush StartPoint=\"0,0\" EndPoint=\"0,1\"><GradientStop Color=\"#50808080\" Offset=\"0.0\" /><GradientStop Color=\"#50404040\" Offset=\"0.25\" /><GradientStop Color=\"#50808080\" Offset=\"1\" /></LinearGradientBrush>",
    L"ElementBorderBrush=<LinearGradientBrush StartPoint=\"0,0\" EndPoint=\"0,1\"><GradientStop Color=\"#50808080\" Offset=\"1\" /><GradientStop Color=\"#50606060\" Offset=\"0.15\" /></LinearGradientBrush>",
    L"BorderThickness=0.3,1,0.3,0.3",
    L"ElementBorderThickness=0.3,0.3,0.3,1",
    L"CornerRadius=12",
    L"ElementCornerRadius=8",
}};

const Theme g_themeLiquidGlass_variant_Alternate = {{
    ThemeTargetStyles{L"Taskbar.TaskbarFrame", {
        L"Width=Auto",
        L"MinWidth:=100",
        L"MaxWidth:=1200",
        L"HorizontalAlignment=Center"}},
    ThemeTargetStyles{L"Taskbar.TaskbarFrame > Grid#RootGrid", {
        L"Margin=0,3,12,3",
        L"BorderThickness=$BorderThickness",
        L"BorderBrush:=$BorderBrush",
        L"CornerRadius=$CornerRadius",
        L"Background:=$Background",
        L"Padding=1"}},
    ThemeTargetStyles{L"Rectangle#BackgroundFill", {
        L"Visibility=1"}},
    ThemeTargetStyles{L"Rectangle#BackgroundStroke", {
        L"Visibility=1"}},
    ThemeTargetStyles{L"Taskbar.AugmentedEntryPointButton#AugmentedEntryPointButton > Taskbar.TaskListButtonPanel#ExperienceToggleButtonRootPanel", {
        L"Margin=-3,0"}},
    ThemeTargetStyles{L"Grid#SystemTrayFrameGrid", {
        L"Background:=$ElementBackground",
        L"BorderBrush:=$ElementBorderBrush",
        L"BorderThickness=$ElementBorderThickness",
        L"CornerRadius=$ElementCornerRadius",
        L"Margin=6"}},
    ThemeTargetStyles{L"SystemTray.ChevronIconView", {
        L"CornerRadius=$ElementCornerRadius"}},
    ThemeTargetStyles{L"SystemTray.NotifyIconView#NotifyItemIcon", {
        L"CornerRadius=$ElementCornerRadius"}},
    ThemeTargetStyles{L"SystemTray.OmniButton", {
        L"CornerRadius=$ElementCornerRadius"}},
    ThemeTargetStyles{L"SystemTray.IconView#SystemTrayIcon > Grid#ContainerGrid > ContentPresenter#ContentPresenter > Grid#ContentGrid > SystemTray.TextIconContent > Grid#ContainerGrid", {
        L"CornerRadius=$ElementCornerRadius"}},
    ThemeTargetStyles{L"Taskbar.Gripper#GripperControl", {
        L"Width=Auto",
        L"MinWidth=24",
        L"HorizontalAlignment=Left"}},
    ThemeTargetStyles{L"TextBlock#TimeInnerTextBlock", {
        L"FontSize=13",
        L"Margin=0",
        L"Padding=0",
        L"RenderTransform:=<TranslateTransform X=\"0\" Y=\"0\" />"}},
    ThemeTargetStyles{L"TextBlock#DateInnerTextBlock", {
        L"Visibility=1"}},
    ThemeTargetStyles{L"TextBlock#InnerTextBlock[Text=\uE971]", {
        L"Text=\uED14"}},
    ThemeTargetStyles{L"TextBlock#SearchBoxTextBlock", {
        L"Text=Search This PC",
        L"FontSize=10"}},
    ThemeTargetStyles{L"SystemTray.OmniButton#NotificationCenterButton > Grid > ContentPresenter > ItemsPresenter > StackPanel > ContentPresenter > SystemTray.IconView#SystemTrayIcon > Grid > Grid > SystemTray.TextIconContent", {
        L"Visibility=1"}},
    ThemeTargetStyles{L"Border#OverflowFlyoutBackgroundBorder", {
        L"BorderThickness=$BorderThickness",
        L"BorderBrush:=$BorderBrush",
        L"Background:=$Background",
        L"CornerRadius=$CornerRadius"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#ModalRootGrid > Windows.UI.Xaml.Controls.Border#BackgroundElement", {
        L"BorderThickness=$BorderThickness",
        L"BorderBrush:=$BorderBrush",
        L"Background=Transparent",
        L"CornerRadius=$CornerRadius"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#ModalRootGrid > Windows.UI.Xaml.Controls.Border#BackgroundElement > WindowsInternal.ComposableShell.Experiences.Switcher.SwitchItemList", {
        L"Background:=$Background"}},
    ThemeTargetStyles{L"WindowsInternal.ComposableShell.Experiences.Switcher.VirtualDesktopBarElement#VirtualDesktopBar", {
        L"CornerRadius=$CornerRadius",
        L"Background:=$Background"}},
    ThemeTargetStyles{L"Border#BackgroundDimmingLayer", {
        L"Background:=$Background",
        L"CornerRadius=$CornerRadius"}},
    ThemeTargetStyles{L"Taskbar.AugmentedEntryPointButton#AugmentedEntryPointButton > Taskbar.TaskListButtonPanel#ExperienceToggleButtonRootPanel", {
        L"Margin=2",
        L"Background:=$ElementBackground",
        L"CornerRadius=$ElementCornerRadius",
        L"BorderThickness=$ElementBorderThickness",
        L"BorderBrush:=$ElementBorderBrush",
        L"Padding=0,-6",
        L"MaxWidth:=200",
        L"MaxHeight=46"}},
    ThemeTargetStyles{L"Taskbar.TaskListButtonPanel#ExperienceToggleButtonRootPanel > Border#BackgroundElement", {
        L"CornerRadius=$ElementCornerRadius",
        L"BorderThickness=$ElementBorderThickness"}},
    ThemeTargetStyles{L"Taskbar.TaskListButton#TaskListButton", {
        L"CornerRadius=$ElementCornerRadius",
        L"BorderThickness=$ElementBorderThickness"}},
    ThemeTargetStyles{L"Border#SnapBarBorder", {
        L"Background:=$Background",
        L"BorderBrush:=$BorderBrush",
        L"CornerRadius=$CornerRadius",
        L"BorderThickness=$BorderThickness",
        L"Margin=2"}},
    ThemeTargetStyles{L"Taskbar.TaskListLabeledButtonPanel@CommonStates > Border#BackgroundElement", {
        L"CornerRadius=$ElementCornerRadius",
        L"BorderThickness=$ElementBorderThickness",
        L"Background@ActiveNormal:=$ElementBackground",
        L"Background@ActivePointerOver:=$AccentBackground",
        L"Background@ActivePressed:=$ElementBackground2",
        L"Background@InactivePointerOver:=$AccentBackground",
        L"Background@InactivePressed:=$ElementBackground2",
        L"BorderBrush@ActiveNormal:=$ElementBorderBrush",
        L"BorderBrush@ActivePointerOver:=$ElementBorderBrush",
        L"BorderBrush@ActivePressed:=$ElementBorderBrush",
        L"BorderBrush@InactivePointerOver:=$ElementBorderBrush",
        L"BorderBrush@InactivePressed:=$ElementBorderBrush",
        L"Margin=1"}},
    ThemeTargetStyles{L"ContentPresenter#ContentPresenter@CommonStates", {
        L"CornerRadius=$ElementCornerRadius",
        L"BorderThickness=$ElementBorderThickness",
        L"Background@ActiveNormal:=$ElementBackground",
        L"Background@ActivePointerOver:=$AccentBackground",
        L"Background@ActivePressed:=$ElementBackground2",
        L"Background@InactivePointerOver:=$AccentBackground",
        L"Background@InactivePressed:=$ElementBackground2",
        L"BorderBrush@ActiveNormal:=$ElementBorderBrush",
        L"BorderBrush@ActivePointerOver:=$ElementBorderBrush",
        L"BorderBrush@ActivePressed:=$ElementBorderBrush",
        L"BorderBrush@InactivePointerOver:=$ElementBorderBrush",
        L"BorderBrush@InactivePressed:=$ElementBorderBrush",
        L"Margin=1"}},
    ThemeTargetStyles{L"Border#SnapPickerBorder", {
        L"Background:=$Background",
        L"BorderBrush:=$BorderBrush",
        L"CornerRadius=$CornerRadius",
        L"BorderThickness=$BorderThickness",
        L"Margin=2"}},
    ThemeTargetStyles{L"ToolTip > ContentPresenter#LayoutRoot", {
        L"Background:=$Background",
        L"BorderBrush:=$BorderBrush",
        L"BorderThickness:=$BorderThickness",
        L"CornerRadius=$CornerRadius"}},
    ThemeTargetStyles{L"WindowsInternal.ComposableShell.Experiences.Switcher.VirtualDesktopBarElement > Grid#GridElement > Border#VirtualDesktopSwitcherBackground", {
        L"BorderBrush:=$BorderBrush",
        L"BorderThickness=$BorderThickness",
        L"CornerRadius=$CornerRadius",
        L"Background:=$Background"}},
    ThemeTargetStyles{L"WindowsInternal.ComposableShell.Experiences.Switcher.SwitchItemListViewItem > Grid > Border", {
        L"CornerRadius=$CornerRadius"}},
    ThemeTargetStyles{L"Border#VirtualDesktopBarBackground", {
        L"Background:=$Background",
        L"BorderBrush:=$BorderBrush",
        L"BorderThickness=$BorderThickness",
        L"CornerRadius=$CornerRadius"}},
    ThemeTargetStyles{L"Rectangle#RunningIndicator", {
        L"Fill:=$AccentBackground",
        L"Width=14"}},
    ThemeTargetStyles{L"Rectangle#ShowDesktopPipe", {
        L"Visibility=1"}},
    ThemeTargetStyles{L"Rectangle#RightOverflowButtonDivider", {
        L"Visibility=1"}},
    ThemeTargetStyles{L"SearchUx.SearchUI.SearchIconButton > SearchUx.SearchUI.SearchButtonRootGrid@CommonStates > Border#BackgroundElement", {
        L"Background:=Transparent",
        L"BorderBrush:=Transparent"}},
    ThemeTargetStyles{L"SearchUx.SearchUI.SearchButtonRootGrid", {
        L"Background:=Transparent",
        L"BorderBrush:=Transparent"}},
    ThemeTargetStyles{L"Border#SearchPillBackgroundElement", {
        L"BorderBrush:=$ElementBorderBrush",
        L"BorderThickness=$ElementBorderThickness",
        L"CornerRadius=$ElementCornerRadius",
        L"Margin=0,1"}},
    ThemeTargetStyles{L"SearchUx.SearchUI.SearchBoxButton > SearchUx.SearchUI.SearchButtonRootGrid@CommonStates > Border#BackgroundElement", {
        L"CornerRadius=$ElementCornerRadius",
        L"BorderThickness=$ElementBorderThickness",
        L"BorderBrush:=$ElementBorderBrush",
        L"Background:=$ElementBackground",
        L"Margin=0,-4"}},
    ThemeTargetStyles{L"Canvas#HoverFlyoutCanvas > Grid#HoverFlyoutGrid > Border#HoverFlyoutBackground", {
        L"Shadow:=",
        L"Background:=$Background",
        L"BorderBrush:=$BorderBrush",
        L"BorderThickness=$BorderThickness",
        L"CornerRadius=$CornerRadius"}},
    ThemeTargetStyles{L"SystemTray.SystemTrayFrame", {
        L"HorizontalAlignment=Right"}},
    ThemeTargetStyles{L"Grid#AugmentedEntryPointContentGrid", {
        L"HorizontalAlignment=Left"}},
}, {
    L"Background=<WindhawkBlur BlurAmount=\"15\" TintColor=\"{ThemeResource SystemAltLowColor}\" TintOpacity=\"0.2\" />",
    L"ElementBackground=<WindhawkBlur BlurAmount=\"20\" TintColor=\"{ThemeResource SystemAltLowColor}\" TintOpacity=\"0.4\" />",
    L"ElementBackground2=<WindhawkBlur BlurAmount=\"20\" TintColor=\"{ThemeResource SystemAltLowColor}\" TintOpacity=\"0.2\" />",
    L"AccentBackground=<WindhawkBlur BlurAmount=\"15\" TintColor=\"{ThemeResource SystemAccentColorLight1}\" TintOpacity=\"0.2\" />",
    L"BorderBrush=<LinearGradientBrush StartPoint=\"0,0\" EndPoint=\"0,1\"><GradientStop Color=\"#50808080\" Offset=\"0.0\" /><GradientStop Color=\"#50404040\" Offset=\"0.25\" /><GradientStop Color=\"#50808080\" Offset=\"1\" /></LinearGradientBrush>",
    L"ElementBorderBrush=<LinearGradientBrush StartPoint=\"0,0\" EndPoint=\"0,1\"><GradientStop Color=\"#50808080\" Offset=\"1\" /><GradientStop Color=\"#50606060\" Offset=\"0.15\" /></LinearGradientBrush>",
    L"BorderThickness=0.3,1,0.3,0.3",
    L"ElementBorderThickness=0.3,0.3,0.3,1",
    L"CornerRadius=12",
    L"ElementCornerRadius=8",
}};

const Theme g_themeBorderless = {{
    ThemeTargetStyles{L"ScrollViewer > ScrollContentPresenter > Border > Grid", {
        L"ColumnDefinitions:=<ColumnDefinitionCollection><ColumnDefinition Width=\"*\"/><ColumnDefinition Width=\"Auto\"/><ColumnDefinition Width=\"Auto\"/><ColumnDefinition Width=\"*\"/></ColumnDefinitionCollection>",
        L"HorizontalAlignment=Stretch"}},
    ThemeTargetStyles{L"Taskbar.TaskbarFrame", {
        L"Grid.Column=1",
        L"Width=$TaskbarFrameWidth",
        L"Margin=0",
        L"MaxWidth=$TaskbarFrameWidth"}},
    ThemeTargetStyles{L"SystemTray.SystemTrayFrame", {
        L"Grid.Column=1",
        L"Width=Auto",
        L"HorizontalAlignment=Right",
        L"Height=40",
        L"VerticalAlignment=Center"}},
    ThemeTargetStyles{L"Taskbar.TaskbarFrame > Windows.UI.Xaml.Controls.Grid#RootGrid > Taskbar.TaskbarBackground#BackgroundControl", {
        L"CornerRadius=6",
        L"Margin=0,2,0,2"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Shapes.Rectangle#BackgroundStroke", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.ItemsRepeater#TaskbarFrameRepeater", {
        L"Margin=0,4,0,4"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Border#OverflowFlyoutBackgroundBorder", {
        L"Shadow:=",
        L"BorderThickness:="}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Grid#ConfirmatorMainGrid", {
        L"Shadow:=",
        L"BorderThickness:="}},
    ThemeTargetStyles{L"Taskbar.OverflowToggleButton#OverflowButton > Taskbar.TaskListButtonPanel#OverflowToggleButtonRootPanel > Windows.UI.Xaml.Controls.FontIcon#FontIcon > Windows.UI.Xaml.Controls.Grid > Windows.UI.Xaml.Controls.TextBlock", {
        L"Text=\uE8F9"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Shapes.Rectangle#MostRecentlyUsedDivider", {
        L"Height=32",
        L"Width=2"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Shapes.Rectangle#LeftOverflowButtonDivider", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Shapes.Rectangle#RightOverflowButtonDivider", {
        L"Visibility=Collapsed"}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.ItemsRepeater#OverflowFlyoutListRepeater", {
        L"Height=48"}},
    ThemeTargetStyles{L"WindowsInternal.ComposableShell.Experiences.TextInput.Common.InputSwitcher > ContentControl > ContentPresenter > Grid > Grid", {
        L"Background:="}},
    ThemeTargetStyles{L"WindowsInternal.ComposableShell.Experiences.TextInput.Common.InputSwitcher > ContentControl > ContentPresenter > Grid", {
        L"Margin=390,0,574,12",
        L"Shadow:=",
        L"BorderThickness:="}},
    ThemeTargetStyles{L"Windows.UI.Xaml.Controls.Border#HoverFlyoutBackground", {
        L"Shadow:="}},
}, {
    L"TaskbarFrameWidth=800",
}};

// clang-format on

std::atomic<bool> g_initialized;
thread_local bool g_initializedForThread;

HANDLE g_restartExplorerPromptThread;
std::atomic<HWND> g_restartExplorerPromptWindow;

constexpr WCHAR kRestartExplorerPromptTitle[] =
    L"Windows 11 Taskbar Styler - Windhawk";
constexpr WCHAR kRestartExplorerPromptTextFormat[] =
    L"Restarting Explorer is required for the mod to activate.\n\nDo you want "
    L"to restart Explorer now?\n\nStatus code: 0x%08X";
constexpr WCHAR kRestartExplorerCommand[] =
    LR"(cmd /c "echo Terminating Explorer...)"
    LR"( & taskkill /f /im explorer.exe)"
    LR"( & timeout /t 1 /nobreak >nul)"
    LR"( & start explorer.exe)"
    LR"( & echo Starting Explorer...)"
    LR"( & timeout /t 3 /nobreak >nul")";

void PromptToRestartExplorer(HRESULT statusCode) {
#ifdef SEARCHHOST_STYLER_PROBE
    Wh_Log(L"SearchHost styler probe: suppressing Explorer restart prompt, "
           L"status=0x%08X",
           statusCode);
    return;
#else
    if (g_restartExplorerPromptThread) {
        if (WaitForSingleObject(g_restartExplorerPromptThread, 0) !=
            WAIT_OBJECT_0) {
            return;
        }

        CloseHandle(g_restartExplorerPromptThread);
    }

    g_restartExplorerPromptThread = CreateThread(
        nullptr, 0,
        [](LPVOID lpParameter) -> DWORD {
            HRESULT statusCode =
                static_cast<HRESULT>(reinterpret_cast<ULONG_PTR>(lpParameter));

            WCHAR promptText[256];
            _snwprintf_s(promptText, _TRUNCATE,
                         kRestartExplorerPromptTextFormat, statusCode);

            TASKDIALOGCONFIG taskDialogConfig{
                .cbSize = sizeof(taskDialogConfig),
                .dwFlags = TDF_ALLOW_DIALOG_CANCELLATION,
                .dwCommonButtons = TDCBF_YES_BUTTON | TDCBF_NO_BUTTON,
                .pszWindowTitle = kRestartExplorerPromptTitle,
                .pszMainIcon = TD_INFORMATION_ICON,
                .pszContent = promptText,
                .pfCallback = [](HWND hwnd, UINT msg, WPARAM wParam,
                                 LPARAM lParam, LONG_PTR lpRefData) -> HRESULT {
                    switch (msg) {
                        case TDN_CREATED:
                            g_restartExplorerPromptWindow = hwnd;
                            SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0,
                                         SWP_NOMOVE | SWP_NOSIZE);
                            break;

                        case TDN_DESTROYED:
                            g_restartExplorerPromptWindow = nullptr;
                            break;
                    }

                    return S_OK;
                },
            };

            int button;
            if (SUCCEEDED(TaskDialogIndirect(&taskDialogConfig, &button,
                                             nullptr, nullptr)) &&
                button == IDYES) {
                WCHAR commandLine[ARRAYSIZE(kRestartExplorerCommand)];
                memcpy(commandLine, kRestartExplorerCommand,
                       sizeof(kRestartExplorerCommand));
                STARTUPINFO si = {
                    .cb = sizeof(si),
                };
                PROCESS_INFORMATION pi{};
                if (CreateProcess(nullptr, commandLine, nullptr, nullptr, FALSE,
                                  0, nullptr, nullptr, &si, &pi)) {
                    CloseHandle(pi.hThread);
                    CloseHandle(pi.hProcess);
                }
            }

            return 0;
        },
        reinterpret_cast<LPVOID>(static_cast<ULONG_PTR>(statusCode)), 0,
        nullptr);
#endif
}

void ApplyCustomizations(InstanceHandle handle,
                         winrt::Windows::UI::Xaml::FrameworkElement element,
                         PCWSTR fallbackClassName);
void CleanupCustomizations(InstanceHandle handle);

HMODULE GetCurrentModuleHandle() {
    HMODULE module;
    if (!GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                               GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                           L"", &module)) {
        return nullptr;
    }

    return module;
}

////////////////////////////////////////////////////////////////////////////////
// clang-format off

#pragma region winrt_hpp

#include <Unknwn.h>
#include <winrt/base.h>

// forward declare namespaces we alias
namespace winrt {
    namespace Windows {
        namespace Foundation {}
        namespace UI::Xaml {}
    }
}

// alias some long namespaces for convenience
namespace wf = winrt::Windows::Foundation;
namespace wux = winrt::Windows::UI::Xaml;

#pragma endregion  // winrt_hpp

#pragma region visualtreewatcher_hpp

#include <winrt/Windows.UI.Xaml.h>

class VisualTreeWatcher : public winrt::implements<VisualTreeWatcher, IVisualTreeServiceCallback2, winrt::non_agile>
{
public:
    VisualTreeWatcher(winrt::com_ptr<IUnknown> site);

    VisualTreeWatcher(const VisualTreeWatcher&) = delete;
    VisualTreeWatcher& operator=(const VisualTreeWatcher&) = delete;

    VisualTreeWatcher(VisualTreeWatcher&&) = delete;
    VisualTreeWatcher& operator=(VisualTreeWatcher&&) = delete;

    ~VisualTreeWatcher();

    void UnadviseVisualTreeChange();

private:
    HRESULT STDMETHODCALLTYPE OnVisualTreeChange(ParentChildRelation relation, VisualElement element, VisualMutationType mutationType) override;
    HRESULT STDMETHODCALLTYPE OnElementStateChanged(InstanceHandle element, VisualElementState elementState, LPCWSTR context) noexcept override;

    wf::IInspectable FromHandle(InstanceHandle handle)
    {
        wf::IInspectable obj;
        winrt::check_hresult(m_XamlDiagnostics->GetIInspectableFromHandle(handle, reinterpret_cast<::IInspectable**>(winrt::put_abi(obj))));
        return obj;
    }

    winrt::com_ptr<IXamlDiagnostics> m_XamlDiagnostics = nullptr;
};

#pragma endregion  // visualtreewatcher_hpp

#pragma region visualtreewatcher_cpp

VisualTreeWatcher::VisualTreeWatcher(winrt::com_ptr<IUnknown> site) :
    m_XamlDiagnostics(site.as<IXamlDiagnostics>())
{
    Wh_Log(L"Constructing VisualTreeWatcher");
    // winrt::check_hresult(m_XamlDiagnostics.as<IVisualTreeService3>()->AdviseVisualTreeChange(this));

    // Calling AdviseVisualTreeChange from the current thread causes the app to
    // hang in Advising::RunOnUIThread sometimes. Creating a new thread and
    // calling it from there fixes it.
    HANDLE thread = CreateThread(
        nullptr, 0,
        [](LPVOID lpParam) -> DWORD {
            auto watcher = reinterpret_cast<VisualTreeWatcher*>(lpParam);
            HRESULT hr = watcher->m_XamlDiagnostics.as<IVisualTreeService3>()->AdviseVisualTreeChange(watcher);
            watcher->Release();
            if (FAILED(hr)) {
                Wh_Log(L"AdviseVisualTreeChange failed with error %08X", hr);
                PromptToRestartExplorer(hr);
            }
            return 0;
        },
        this, 0, nullptr);
    if (thread) {
        AddRef();
        CloseHandle(thread);
    }
}

VisualTreeWatcher::~VisualTreeWatcher()
{
    Wh_Log(L"Destructing VisualTreeWatcher");
}

void VisualTreeWatcher::UnadviseVisualTreeChange()
{
    Wh_Log(L"UnadviseVisualTreeChange VisualTreeWatcher");
    HRESULT hr = m_XamlDiagnostics.as<IVisualTreeService3>()->UnadviseVisualTreeChange(this);
    if (FAILED(hr)) {
        Wh_Log(L"UnadviseVisualTreeChange failed with error %08X", hr);
    }
}

HRESULT VisualTreeWatcher::OnVisualTreeChange(ParentChildRelation, VisualElement element, VisualMutationType mutationType) try
{
    Wh_Log(L"========================================");

    switch (mutationType)
    {
    case Add:
        Wh_Log(L"Mutation type: Add %llu", element.Handle);
        break;

    case Remove:
        Wh_Log(L"Mutation type: Remove %llu", element.Handle);
        break;

    default:
        Wh_Log(L"Mutation type: %d %llu", static_cast<int>(mutationType), element.Handle);
        break;
    }

    Wh_Log(L"Element type: %s", element.Type);

    if (!g_initializedForThread) {
        Wh_Log(L"Not initialized for thread %u", GetCurrentThreadId());
        return S_OK;
    }

    if (mutationType == Add)
    {
        const auto inspectable = FromHandle(element.Handle);
        auto frameworkElement = inspectable.try_as<wux::FrameworkElement>();
        if (frameworkElement)
        {
            Wh_Log(L"FrameworkElement name: %s", frameworkElement.Name().c_str());
            ApplyCustomizations(element.Handle, frameworkElement, element.Type);
        }
        else
        {
            Wh_Log(L"Skipping non-FrameworkElement");
        }
    }
    else if (mutationType == Remove)
    {
        CleanupCustomizations(element.Handle);
    }

    return S_OK;
}
catch (...)
{
    HRESULT hr = winrt::to_hresult();
    Wh_Log(L"Error %08X", hr);

    // Returning an error prevents (some?) further messages, always return
    // success.
    // return hr;
    return S_OK;
}

HRESULT VisualTreeWatcher::OnElementStateChanged(InstanceHandle, VisualElementState, LPCWSTR) noexcept
{
    return S_OK;
}

#pragma endregion  // visualtreewatcher_cpp

#pragma region tap_hpp

#include <ocidl.h>

winrt::com_ptr<VisualTreeWatcher> g_visualTreeWatcher;

// {C85D8CC7-5463-40E8-A432-F5916B6427E5}
static constexpr CLSID CLSID_WindhawkTAP = { 0xc85d8cc7, 0x5463, 0x40e8, { 0xa4, 0x32, 0xf5, 0x91, 0x6b, 0x64, 0x27, 0xe5 } };

class WindhawkTAP : public winrt::implements<WindhawkTAP, IObjectWithSite, winrt::non_agile>
{
public:
    HRESULT STDMETHODCALLTYPE SetSite(IUnknown *pUnkSite) override;
    HRESULT STDMETHODCALLTYPE GetSite(REFIID riid, void **ppvSite) noexcept override;

private:
    winrt::com_ptr<IUnknown> site;
};

#pragma endregion  // tap_hpp

#pragma region tap_cpp

HRESULT WindhawkTAP::SetSite(IUnknown *pUnkSite) try
{
    // Only ever 1 VTW at once.
    if (g_visualTreeWatcher)
    {
        g_visualTreeWatcher->UnadviseVisualTreeChange();
        g_visualTreeWatcher = nullptr;
    }

    site.copy_from(pUnkSite);

    if (site)
    {
        // Decrease refcount increased by InitializeXamlDiagnosticsEx.
        FreeLibrary(GetCurrentModuleHandle());

        g_visualTreeWatcher = winrt::make_self<VisualTreeWatcher>(site);
    }

    return S_OK;
}
catch (...)
{
    HRESULT hr = winrt::to_hresult();
    Wh_Log(L"Error %08X", hr);
    return hr;
}

HRESULT WindhawkTAP::GetSite(REFIID riid, void **ppvSite) noexcept
{
    return site.as(riid, ppvSite);
}

#pragma endregion  // tap_cpp

#pragma region simplefactory_hpp

#include <Unknwn.h>

template<class T>
struct SimpleFactory : winrt::implements<SimpleFactory<T>, IClassFactory, winrt::non_agile>
{
    HRESULT STDMETHODCALLTYPE CreateInstance(IUnknown* pUnkOuter, REFIID riid, void** ppvObject) override try
    {
        if (!pUnkOuter)
        {
            *ppvObject = nullptr;
            return winrt::make<T>().as(riid, ppvObject);
        }
        else
        {
            return CLASS_E_NOAGGREGATION;
        }
    }
    catch (...)
    {
        HRESULT hr = winrt::to_hresult();
        Wh_Log(L"Error %08X", hr);
        return hr;
    }

    HRESULT STDMETHODCALLTYPE LockServer(BOOL) noexcept override
    {
        return S_OK;
    }
};

#pragma endregion  // simplefactory_hpp

#pragma region module_cpp

#include <combaseapi.h>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdll-attribute-on-redeclaration"

__declspec(dllexport)
_Use_decl_annotations_ STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv) try
{
    if (rclsid == CLSID_WindhawkTAP)
    {
        *ppv = nullptr;
        return winrt::make<SimpleFactory<WindhawkTAP>>().as(riid, ppv);
    }
    else
    {
        return CLASS_E_CLASSNOTAVAILABLE;
    }
}
catch (...)
{
    HRESULT hr = winrt::to_hresult();
    Wh_Log(L"Error %08X", hr);
    return hr;
}

__declspec(dllexport)
_Use_decl_annotations_ STDAPI DllCanUnloadNow(void)
{
    if (winrt::get_module_lock())
    {
        return S_FALSE;
    }
    else
    {
        return S_OK;
    }
}

#pragma clang diagnostic pop

#pragma endregion  // module_cpp

#pragma region api_cpp

bool g_inInjectWindhawkTAP = false;

using PFN_INITIALIZE_XAML_DIAGNOSTICS_EX = decltype(&InitializeXamlDiagnosticsEx);

HRESULT InjectWindhawkTAP() noexcept
{
    HMODULE module = GetCurrentModuleHandle();
    if (!module)
    {
        return HRESULT_FROM_WIN32(GetLastError());
    }

    WCHAR location[MAX_PATH];
    switch (GetModuleFileName(module, location, ARRAYSIZE(location)))
    {
    case 0:
    case ARRAYSIZE(location):
        return HRESULT_FROM_WIN32(GetLastError());
    }

    const HMODULE wux(LoadLibraryEx(L"Windows.UI.Xaml.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32));
    if (!wux) [[unlikely]]
    {
        return HRESULT_FROM_WIN32(GetLastError());
    }

    const auto ixde = reinterpret_cast<PFN_INITIALIZE_XAML_DIAGNOSTICS_EX>(GetProcAddress(wux, "InitializeXamlDiagnosticsEx"));
    if (!ixde) [[unlikely]]
    {
        return HRESULT_FROM_WIN32(GetLastError());
    }

    // I didn't find a better way than trying many connections until one works.
    // Reference:
    // https://github.com/microsoft/microsoft-ui-xaml/blob/d74a0332cf0d5e58f12eddce1070fa7a79b4c2db/src/dxaml/xcp/dxaml/lib/DXamlCore.cpp#L2782
    g_inInjectWindhawkTAP = true;

    HRESULT hr;
    for (int i = 0; i < 10000; i++)
    {
        WCHAR connectionName[256];
        wsprintf(connectionName, L"VisualDiagConnection%d", i + 1);

        hr = ixde(connectionName, GetCurrentProcessId(), L"", location, CLSID_WindhawkTAP, nullptr);
        if (hr != HRESULT_FROM_WIN32(ERROR_NOT_FOUND))
        {
            break;
        }
    }

    g_inInjectWindhawkTAP = false;

    return hr;
}

#pragma endregion  // api_cpp

// clang-format on
////////////////////////////////////////////////////////////////////////////////

#include <windhawk_utils.h>

#include <algorithm>
#include <charconv>
#include <cmath>
#include <limits>
#include <list>
#include <mutex>
#include <optional>
#include <random>
#include <sstream>
#include <string>
#include <string_view>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <variant>
#include <vector>

using namespace std::string_view_literals;

#include <initguid.h>

#include <commctrl.h>
#include <d2d1_1.h>
#include <roapi.h>
#include <windows.graphics.effects.h>
#include <winstring.h>

#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Graphics.Effects.h>
#include <winrt/Windows.Networking.Connectivity.h>
#include <winrt/Windows.Storage.Streams.h>
#include <winrt/Windows.System.Power.h>
#include <winrt/Windows.System.h>
#include <winrt/Windows.UI.Composition.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.Text.h>
#include <winrt/Windows.UI.ViewManagement.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Hosting.h>
#include <winrt/Windows.UI.Xaml.Markup.h>
#include <winrt/Windows.UI.Xaml.Media.Imaging.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.Shapes.h>
#include <winrt/Windows.UI.Xaml.h>

using namespace winrt::Windows::UI::Xaml;

namespace wge = winrt::Windows::Graphics::Effects;
namespace wuc = winrt::Windows::UI::Composition;
namespace wuxh = wux::Hosting;
namespace awge = ABI::Windows::Graphics::Effects;

enum class XamlDiagnosticsHandling {
    kAlert,
    kBlock,
    kAllow,
};

struct {
    XamlDiagnosticsHandling xamlDiagnosticsHandling;
} g_settings;

// https://stackoverflow.com/a/51274008
template <auto fn>
struct deleter_from_fn {
    template <typename T>
    constexpr void operator()(T* arg) const {
        fn(arg);
    }
};
using string_setting_unique_ptr =
    std::unique_ptr<const WCHAR[], deleter_from_fn<Wh_FreeStringSetting>>;

using PropertyKeyValue =
    std::pair<DependencyProperty, winrt::Windows::Foundation::IInspectable>;

using PropertyValuesUnresolved =
    std::vector<std::pair<std::wstring, std::wstring>>;
using PropertyValues = std::vector<PropertyKeyValue>;
using PropertyValuesMaybeUnresolved =
    std::variant<PropertyValuesUnresolved, PropertyValues>;

struct ElementMatcher {
    enum class Kind {
        Element,   // Normal element matcher.
        Wildcard,  // '*': matches zero or more intermediate ancestors.
        Root,      // ':root': asserts the next element has no parent.
    };
    Kind kind = Kind::Element;
    std::wstring type;
    std::wstring name;
    std::optional<std::wstring> visualStateGroupName;
    int oneBasedIndex = 0;
    PropertyValuesMaybeUnresolved propertyValues;
};

// A `Property[@VisualState][:]=value` rule that sets a control property.
// `value` may contain `{{...}}` placeholders, in which case `isDynamic()`
// returns true and the rule is re-resolved on every apply.
struct ValueRule {
    std::wstring propertyName;
    std::wstring visualState;
    std::wstring value;
    bool isXamlValue = false;

    bool isDynamic() const { return value.find(L"{{") != std::wstring::npos; }
};

// A `Property=>VarName` rule that observes a control property and writes its
// current value into the named mod-global style variable.
struct CaptureRule {
    std::wstring propertyName;
    std::wstring varName;
};

// Parsed-but-not-yet-resolved rules for one target. Captures and value-rules
// are intentionally split: they live in different fields of `ResolvedRules`
// post-resolution, and the parser already validates that captures cannot carry
// `:=` or `@VisualState`.
struct UnresolvedRules {
    std::vector<ValueRule> valueRules;
    std::vector<CaptureRule> captureRules;
};

struct XamlBlurBrushParams {
    float blurAmount;
    winrt::Windows::UI::Color tint;
    std::optional<uint8_t> tintOpacity;
    std::wstring tintThemeResourceKey;  // Empty if not from ThemeResource
    std::optional<float> tintLuminosityOpacity;
    std::optional<float> tintSaturation;
    std::optional<float> noiseOpacity;
    std::optional<float> noiseDensity;
    std::optional<winrt::Windows::UI::Color> fallbackColor;
    std::wstring fallbackThemeResourceKey;  // Empty if not from ThemeResource
};

// Holds the raw rule body for a style whose value depends on `{{...}}`
// substitutions. Re-resolved on every apply and on every variable change.
// `propertyName` is kept alongside the value because Windows.UI.Xaml's
// DependencyProperty does not expose its name, and the re-resolution path needs
// to feed the name back to the XAML parser.
struct DynamicStyleTemplate {
    std::wstring propertyName;
    std::wstring rawValue;
    bool isXamlValue = false;
};

// Tagged value for one (property, visualState) cell of PropertyOverrides.
// Possible states:
// - IInspectable        : fully resolved WinRT value (literal or static XAML).
//                         Apply directly via SetValue.
// - XamlBlurBrushParams : parsed `<WindhawkBlur .../>` parameters. The brush
//                         instance is constructed at apply time (needs the live
//                         UIElement).
// - DynamicStyleTemplate: rule body contains `{{...}}` substitutions.
//                         Re-resolved on every apply and on every variable
//                         change. This arm appears only inside
//                         PropertyOverrides cells; it is never stored in
//                         ElementPropertyCustomizationState::customValue (see
//                         notes there).
using PropertyOverrideValue =
    std::variant<winrt::Windows::Foundation::IInspectable,
                 XamlBlurBrushParams,
                 DynamicStyleTemplate>;

// Property -> visual state -> value.
using PropertyOverrides =
    std::unordered_map<DependencyProperty,
                       std::unordered_map<std::wstring, PropertyOverrideValue>>;

// Resolved counterpart to CaptureRule: the property name string has been turned
// into an actual DependencyProperty by the XAML parser, so the apply path can
// call RegisterPropertyChangedCallback / GetValue directly without re-resolving
// on every use.
struct CaptureSpec {
    DependencyProperty property{nullptr};
    std::wstring varName;
};

struct ResolvedRules {
    PropertyOverrides propertyOverrides;
    std::vector<CaptureSpec> captures;
};

using PropertyOverridesMaybeUnresolved =
    std::variant<UnresolvedRules, ResolvedRules>;

struct ElementCustomizationRules {
    ElementMatcher elementMatcher;
    std::vector<ElementMatcher> parentElementMatchers;
    PropertyOverridesMaybeUnresolved propertyOverrides;
};

thread_local std::vector<ElementCustomizationRules>
    g_elementsCustomizationRules;

struct ElementPropertyCustomizationState {
    std::optional<winrt::Windows::Foundation::IInspectable> originalValue;
    // The most recently applied value, re-pushed by the per-DP property-
    // changed callback when something external (animation, system Setter)
    // overrides it. Although PropertyOverrideValue's variant declares a
    // DynamicStyleTemplate arm, customValue here is always either IInspectable
    // or XamlBlurBrushParams in practice -- dynamic styles get resolved into
    // one of those before being stored, and the source template lives
    // separately in `dynamicTemplate` below.
    std::optional<PropertyOverrideValue> customValue;
    winrt::Windows::Foundation::IInspectable lastAppliedValue{nullptr};
    int64_t propertyChangedToken = 0;
    // Source template for dynamic styles whose value contains `{{...}}`
    // substitutions; re-evaluated whenever a referenced variable changes, with
    // the resolved result written back into `customValue`. Empty for static
    // styles.
    std::optional<DynamicStyleTemplate> dynamicTemplate;
    // Names of style variables this property's value depends on. Populated
    // alongside `dynamicTemplate`; empty for static styles.
    std::vector<std::wstring> variableDependencies;
};

struct CapturePropertyCustomizationState {
    std::wstring varName;
    int64_t propertyChangedToken = 0;
};

struct ElementCustomizationStateForVisualStateGroup {
    std::unordered_map<DependencyProperty, ElementPropertyCustomizationState>
        propertyCustomizationStates;
    winrt::event_token visualStateGroupCurrentStateChangedToken;
};

struct ElementCustomizationState {
    winrt::weak_ref<FrameworkElement> element;

    // Cached weak ref to the element's XamlRoot at register time. Used during
    // cleanup paths to find this element's per-XamlRoot StyleVariableState
    // even after the element above has been GC'd. A weak_ref (not a raw
    // pointer) so that an expired XamlRoot does not silently collide with a
    // freshly-allocated one at the same address.
    winrt::weak_ref<XamlRoot> xamlRoot;

    // Capture state lives at the element level: capture rules (`Prop=>Var`) are
    // intentionally not visual-state-aware (the parser rejects `@VisualState`
    // on them), and a single element observed by multiple targets with
    // different VSGs should still only register one
    // RegisterPropertyChangedCallback per DP and one SizeChanged subscription.
    std::unordered_map<DependencyProperty, CapturePropertyCustomizationState>
        captureCustomizationStates;

    // ActualWidth/ActualHeight (and other layout-driven DPs) do not fire
    // RegisterPropertyChangedCallback on UWP, so any element with capture rules
    // also subscribes to `FrameworkElement.SizeChanged` to pick up size
    // changes.
    winrt::event_token captureSizeChangedToken;

    // Use list to avoid reallocations on insertion, as pointers to items are
    // captured in callbacks and stored.
    std::list<std::pair<std::optional<winrt::weak_ref<VisualStateGroup>>,
                        ElementCustomizationStateForVisualStateGroup>>
        perVisualStateGroup;
};

thread_local std::unordered_map<InstanceHandle, ElementCustomizationState>
    g_elementsCustomizationState;

// Mod-global style variable registry. Populated by `Property=>VarName` capture
// rules and consumed by `{{VarName}}` substitutions in other styles. Last
// writer wins -- a new capture from any element overwrites the value.
struct StyleVariableValue {
    std::wstring stringForm;        // invariant-formatted text representation
    std::optional<double> numeric;  // only present when source was numeric
    // True for primitive captures whose `stringForm` is meaningful to insert
    // verbatim into a XAML attribute (numeric, boolean, string). False for
    // opaque types -- their stringForm is the captured class name, kept only
    // for diagnostics; bare-identifier substitution skips such variables.
    bool substitutable = false;
};

struct StyleVariableConsumer {
    InstanceHandle elementHandle;
    DependencyProperty property{nullptr};
    // Each consumer remembers its own fallbackClassName so that propagation can
    // re-resolve dynamic styles using the consumer's match-site context, not
    // the (potentially different) capturer's.
    std::wstring fallbackClassName;
};

// Per-XamlRoot scope for the style variable registry. Multiple taskbars on one
// UI thread each have their own XamlRoot; keying by XamlRoot prevents
// `Property=>Var` captures on one taskbar from being substituted into
// `{{Var}}` on another. Identity is tracked via weak_ref so that a destroyed
// XamlRoot's slot cannot be confused with a new XamlRoot allocated at the
// same address. std::list is used because pointers to existing entries must
// stay valid as new entries are added or stale ones reaped: lambdas
// registered on per-element captures hold a StyleVariableState* for the
// lifetime of the entry.
struct StyleVariableState {
    winrt::weak_ref<XamlRoot> xamlRoot;
    std::unordered_map<std::wstring, StyleVariableValue> variables;
    std::unordered_map<std::wstring, std::vector<StyleVariableConsumer>>
        consumers;
};

thread_local std::list<StyleVariableState> g_styleVariableState;

// Look up (or create) the entry for a live XamlRoot. Reaps any entries whose
// XamlRoot has been destroyed before searching, so a recycled address cannot
// collide with a stale entry.
StyleVariableState* GetStyleVariableState(XamlRoot const& xamlRoot) {
    if (!xamlRoot) {
        return nullptr;
    }
    g_styleVariableState.remove_if(
        [](StyleVariableState const& entry) { return !entry.xamlRoot.get(); });
    for (auto& entry : g_styleVariableState) {
        if (entry.xamlRoot.get() == xamlRoot) {
            return &entry;
        }
    }
    auto& fresh = g_styleVariableState.emplace_back();
    fresh.xamlRoot = xamlRoot;
    return &fresh;
}

// Look up an existing entry from a cached weak_ref. Returns nullptr if the
// XamlRoot is already gone (cleanup is then a no-op since the entry has been
// or will be reaped).
StyleVariableState* GetStyleVariableState(
    winrt::weak_ref<XamlRoot> const& xamlRootWeak) {
    auto strong = xamlRootWeak.get();
    if (!strong) {
        return nullptr;
    }
    return GetStyleVariableState(strong);
}

// Convenience for entry points that have a FrameworkElement. Returns nullptr
// if the element is detached (no XamlRoot yet).
StyleVariableState* GetStyleVariableState(FrameworkElement const& element) {
    if (!element) {
        return nullptr;
    }
    XamlRoot xamlRoot{nullptr};
    try {
        xamlRoot = element.XamlRoot();
    } catch (...) {
        // Defensive: detached elements may throw on XamlRoot().
    }
    return GetStyleVariableState(xamlRoot);
}

thread_local bool g_elementPropertyModifying;

thread_local std::list<
    std::pair<winrt::weak_ref<DependencyObject>,
              winrt::Windows::Foundation::IAsyncOperation<bool>>>
    g_delayedBackgroundFillSet;

// Global list to track ImageBrushes with failed loads for retry on network
// reconnection.
struct ImageBrushFailedLoadInfo {
    winrt::weak_ref<Media::ImageBrush> brush;
    winrt::hstring imageSource;
    Media::ImageBrush::ImageFailed_revoker imageFailedRevoker;
    Media::ImageBrush::ImageOpened_revoker imageOpenedRevoker;
};

struct FailedImageBrushesForThread {
    std::list<ImageBrushFailedLoadInfo> failedImageBrushes;
    winrt::Windows::System::DispatcherQueue dispatcher{nullptr};
};

thread_local FailedImageBrushesForThread g_failedImageBrushesForThread;

// Global registry of all threads that have failed image brushes.
std::mutex g_failedImageBrushesRegistryMutex;
std::vector<winrt::weak_ref<winrt::Windows::System::DispatcherQueue>>
    g_failedImageBrushesRegistry;
winrt::event_token g_networkStatusChangedToken;

enum class ResourceVariableTheme {
    None,
    Dark,
    Light,
};

enum class ResourceVariableType {
    String,
    Xaml,
    ThemeResourceReference,
};

struct ResourceVariableEntry {
    std::wstring key;
    std::wstring value;
    ResourceVariableTheme theme;
    ResourceVariableType type;
};

thread_local std::vector<ResourceVariableEntry> g_resourceVariables;

// Track original resource values for restoration (per-thread since
// Application::Current().Resources() is per-thread).
thread_local std::unordered_map<std::wstring,
                                winrt::Windows::Foundation::IInspectable>
    g_originalResourceValues;

// Track our merged theme dictionary for cleanup (per-thread).
thread_local ResourceDictionary g_resourceVariablesThemeDict{nullptr};

// For listening to theme color changes (per-thread).
thread_local winrt::Windows::UI::ViewManagement::UISettings g_uiSettings{
    nullptr};
thread_local winrt::event_token g_colorValuesChangedToken;

thread_local winrt::Windows::UI::Xaml::FrameworkElement::SizeChanged_revoker
    g_workaroundSizeChangedRevoker;

winrt::Windows::Foundation::IInspectable ReadLocalValueWithWorkaround(
    DependencyObject elementDo,
    DependencyProperty property) {
    auto value = elementDo.ReadLocalValue(property);
    if (value) {
        auto className = winrt::get_class_name(value);
        if (className == L"Windows.UI.Xaml.Data.BindingExpressionBase" ||
            className == L"Windows.UI.Xaml.Data.BindingExpression") {
            // BindingExpressionBase was observed to be returned for XAML
            // properties that were declared as following:
            //
            // <Border ... CornerRadius="{TemplateBinding CornerRadius}" />
            //
            // Calling SetValue with it fails with an error, so we won't be able
            // to use it to restore the value. As a workaround, we use
            // GetAnimationBaseValue to get the value.
            Wh_Log(L"ReadLocalValue returned %s, using GetAnimationBaseValue",
                   className.c_str());
            value = elementDo.GetAnimationBaseValue(property);
        }
    }

    Wh_Log(L"Read property value %s",
           value ? (value == DependencyProperty::UnsetValue()
                        ? L"(unset)"
                        : winrt::get_class_name(value).c_str())
                 : L"(null)");

    return value;
}

////////////////////////////////////////////////////////////////////////////////
// Noise generation
//
// Generates a tileable noise BMP in memory. Density controls the brightness
// distribution curve via a power function (lower density = sparser bright
// pixels). Opacity is handled downstream by the composition effect graph.
winrt::Windows::Storage::Streams::IRandomAccessStream CreateNoiseStream(
    float density) {
    // Cache the last stream to avoid regenerating when density hasn't changed.
    // The cached stream is never read directly; callers get independent clones
    // via CloneStream() so they don't share a seek cursor.
    thread_local float cachedDensity = std::numeric_limits<float>::quiet_NaN();
    thread_local winrt::Windows::Storage::Streams::InMemoryRandomAccessStream
        cachedStream{nullptr};

    if (density == cachedDensity && cachedStream) {
        return cachedStream.CloneStream();
    }

    // Use 256x256 to minimize visible tiling seams.
    constexpr int kSize = 256;
    constexpr DWORD kBpp = 32;
    constexpr DWORD rowSize = kSize * (kBpp / 8);
    constexpr DWORD dataSize = rowSize * kSize;

    BITMAPFILEHEADER fileHeader{
        .bfType = 0x4D42,  // "BM"
        .bfSize =
            sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + dataSize,
        .bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER),
    };

    BITMAPINFOHEADER infoHeader{
        .biSize = sizeof(BITMAPINFOHEADER),
        .biWidth = kSize,
        .biHeight = kSize,
        .biPlanes = 1,
        .biBitCount = kBpp,
        .biSizeImage = dataSize,
    };

    std::vector<uint8_t> pixels(dataSize);

    // Precompute the density power curve as a lookup table so that
    // std::pow is called 256 times instead of once per pixel (65536).
    float safeDensity = std::clamp(density, 0.001f, 1.0f);
    float exponent = 1.0f / safeDensity;

    uint8_t lut[256];
    for (int i = 0; i < 256; i++) {
        lut[i] = static_cast<uint8_t>(std::pow(i / 255.0f, exponent) * 255.0f);
    }

    std::mt19937 rng(0);
    std::uniform_int_distribution<int> dist(0, 255);

    for (size_t i = 0; i < pixels.size(); i += 4) {
        uint8_t gray = lut[dist(rng)];

        // Fully opaque; opacity is applied downstream by ColorMatrixEffect.
        pixels[i] = gray;
        pixels[i + 1] = gray;
        pixels[i + 2] = gray;
        pixels[i + 3] = 255;
    }

    winrt::Windows::Storage::Streams::InMemoryRandomAccessStream stream;
    winrt::Windows::Storage::Streams::DataWriter writer(stream);
    writer.WriteBytes(winrt::array_view<const uint8_t>(
        reinterpret_cast<const uint8_t*>(&fileHeader), sizeof(fileHeader)));
    writer.WriteBytes(winrt::array_view<const uint8_t>(
        reinterpret_cast<const uint8_t*>(&infoHeader), sizeof(infoHeader)));
    writer.WriteBytes(pixels);
    writer.StoreAsync().get();
    writer.DetachStream();

    cachedStream = std::move(stream);
    cachedDensity = density;

    return cachedStream.CloneStream();
}

// Blur background implementation, copied from TranslucentTB.
////////////////////////////////////////////////////////////////////////////////
// clang-format off
template <> inline constexpr winrt::guid winrt::impl::guid_v<winrt::impl::abi_t<winrt::Windows::Foundation::IPropertyValue>>{
    winrt::impl::guid_v<winrt::Windows::Foundation::IPropertyValue>
};

typedef enum MY_D2D1_GAUSSIANBLUR_OPTIMIZATION
{
    MY_D2D1_GAUSSIANBLUR_OPTIMIZATION_SPEED = 0,
    MY_D2D1_GAUSSIANBLUR_OPTIMIZATION_BALANCED = 1,
    MY_D2D1_GAUSSIANBLUR_OPTIMIZATION_QUALITY = 2,
    MY_D2D1_GAUSSIANBLUR_OPTIMIZATION_FORCE_DWORD = 0xffffffff

} MY_D2D1_GAUSSIANBLUR_OPTIMIZATION;

////////////////////////////////////////////////////////////////////////////////
// XamlBlurBrush.h
class XamlBlurBrush : public Media::XamlCompositionBrushBaseT<XamlBlurBrush>
{
public:
    XamlBlurBrush(UIElement element,
                  float blurAmount,
                  winrt::Windows::UI::Color tint,
                  std::optional<uint8_t> tintOpacity,
                  winrt::hstring tintThemeResourceKey,
                  std::optional<float> tintLuminosityOpacity,
                  std::optional<float> tintSaturation,
                  std::optional<float> noiseOpacity,
                  std::optional<float> noiseDensity,
                  std::optional<winrt::Windows::UI::Color> fallbackColor,
                  winrt::hstring fallbackThemeResourceKey);
    ~XamlBlurBrush();

    void OnConnected();
    void OnDisconnected();

private:
    void RefreshThemeTint();
    void RefreshFallbackColor();
    bool ShouldUseFallback() const;
    void RefreshBrush();
    wuc::CompositionBrush CreateEffectBrush();
    wuc::CompositionBrush CreateFallbackBrush();

    wuc::Compositor m_compositor;
    float m_blurAmount;
    winrt::Windows::UI::Color m_tint;
    std::optional<uint8_t> m_tintOpacity;
    winrt::hstring m_tintThemeResourceKey;
    std::optional<float> m_tintLuminosityOpacity;
    std::optional<float> m_tintSaturation;
    std::optional<float> m_noiseOpacity;
    std::optional<float> m_noiseDensity;
    std::optional<winrt::Windows::UI::Color> m_fallbackColor;
    winrt::hstring m_fallbackThemeResourceKey;
    Media::SolidColorBrush m_proxyBrush{nullptr};
    Media::SolidColorBrush m_fallbackProxyBrush{nullptr};
    winrt::weak_ref<FrameworkElement> m_weakProxyElement;
    winrt::hstring m_proxyKey;
    winrt::hstring m_fallbackProxyKey;
    winrt::Windows::UI::ViewManagement::UISettings m_uiSettings{nullptr};
    winrt::event_token m_advancedEffectsEnabledChangedToken{};
    winrt::event_token m_energySaverStatusChangedToken{};
    winrt::Windows::System::DispatcherQueue m_dispatcher{nullptr};
    HKEY m_powerKey{nullptr};
    HANDLE m_regNotifyEvent{nullptr};
    HANDLE m_regWaitHandle{nullptr};

    static void CALLBACK OnEnergySaverRegistryChanged(PVOID context,
                                                      BOOLEAN timerOrWaitFired);
};

////////////////////////////////////////////////////////////////////////////////
// windows.graphics.effects.interop.h
#ifndef BUILD_WINDOWS
namespace ABI {
#endif
namespace Windows {
namespace Graphics {
namespace Effects {

typedef interface IGraphicsEffectSource                         IGraphicsEffectSource;
typedef interface IGraphicsEffectD2D1Interop                    IGraphicsEffectD2D1Interop;


typedef enum GRAPHICS_EFFECT_PROPERTY_MAPPING
{
    GRAPHICS_EFFECT_PROPERTY_MAPPING_UNKNOWN,
    GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT,
    GRAPHICS_EFFECT_PROPERTY_MAPPING_VECTORX,
    GRAPHICS_EFFECT_PROPERTY_MAPPING_VECTORY,
    GRAPHICS_EFFECT_PROPERTY_MAPPING_VECTORZ,
    GRAPHICS_EFFECT_PROPERTY_MAPPING_VECTORW,
    GRAPHICS_EFFECT_PROPERTY_MAPPING_RECT_TO_VECTOR4,
    GRAPHICS_EFFECT_PROPERTY_MAPPING_RADIANS_TO_DEGREES,
    GRAPHICS_EFFECT_PROPERTY_MAPPING_COLORMATRIX_ALPHA_MODE,
    GRAPHICS_EFFECT_PROPERTY_MAPPING_COLOR_TO_VECTOR3,
    GRAPHICS_EFFECT_PROPERTY_MAPPING_COLOR_TO_VECTOR4
} GRAPHICS_EFFECT_PROPERTY_MAPPING;

//+-----------------------------------------------------------------------------
//
//  Interface:
//      IGraphicsEffectD2D1Interop
//
//  Synopsis:
//      An interface providing a Interop counterpart to IGraphicsEffect
//      and allowing for metadata queries.
//
//------------------------------------------------------------------------------

#undef INTERFACE
#define INTERFACE IGraphicsEffectD2D1Interop
DECLARE_INTERFACE_IID_(IGraphicsEffectD2D1Interop, IUnknown, "2FC57384-A068-44D7-A331-30982FCF7177")
{
    STDMETHOD(GetEffectId)(
        _Out_ GUID * id
        ) PURE;

    STDMETHOD(GetNamedPropertyMapping)(
        LPCWSTR name,
        _Out_ UINT * index,
        _Out_ GRAPHICS_EFFECT_PROPERTY_MAPPING * mapping
        ) PURE;

    STDMETHOD(GetPropertyCount)(
        _Out_ UINT * count
        ) PURE;

    STDMETHOD(GetProperty)(
        UINT index,
        _Outptr_ winrt::impl::abi_t<winrt::Windows::Foundation::IPropertyValue> ** value
        ) PURE;

    STDMETHOD(GetSource)(
        UINT index,
        _Outptr_ IGraphicsEffectSource ** source
        ) PURE;

    STDMETHOD(GetSourceCount)(
        _Out_ UINT * count
        ) PURE;
};


} // namespace Effects
} // namespace Graphics
} // namespace Windows
#ifndef BUILD_WINDOWS
} // namespace ABI
#endif

template <> inline constexpr winrt::guid winrt::impl::guid_v<ABI::Windows::Graphics::Effects::IGraphicsEffectD2D1Interop>{
    0x2FC57384, 0xA068, 0x44D7, { 0xA3, 0x31, 0x30, 0x98, 0x2F, 0xCF, 0x71, 0x77 }
};


////////////////////////////////////////////////////////////////////////////////
// CompositeEffect.h
struct CompositeEffect : winrt::implements<CompositeEffect, wge::IGraphicsEffect, wge::IGraphicsEffectSource, awge::IGraphicsEffectD2D1Interop>
{
public:
    // IGraphicsEffectD2D1Interop
    HRESULT STDMETHODCALLTYPE GetEffectId(GUID* id) noexcept override;
    HRESULT STDMETHODCALLTYPE GetNamedPropertyMapping(LPCWSTR name, UINT* index, awge::GRAPHICS_EFFECT_PROPERTY_MAPPING* mapping) noexcept override;
    HRESULT STDMETHODCALLTYPE GetPropertyCount(UINT* count) noexcept override;
    HRESULT STDMETHODCALLTYPE GetProperty(UINT index, winrt::impl::abi_t<winrt::Windows::Foundation::IPropertyValue>** value) noexcept override;
    HRESULT STDMETHODCALLTYPE GetSource(UINT index, awge::IGraphicsEffectSource** source) noexcept override;
    HRESULT STDMETHODCALLTYPE GetSourceCount(UINT* count) noexcept override;

    // IGraphicsEffect
    winrt::hstring Name();
    void Name(winrt::hstring name);

    std::vector<wge::IGraphicsEffectSource> Sources;
    D2D1_COMPOSITE_MODE Mode = D2D1_COMPOSITE_MODE_SOURCE_OVER;
private:
    winrt::hstring m_name = L"CompositeEffect";
};

////////////////////////////////////////////////////////////////////////////////
// CompositeEffect.cpp
HRESULT CompositeEffect::GetEffectId(GUID* id) noexcept
{
    if (id == nullptr) [[unlikely]]
    {
        return E_INVALIDARG;
    }

    *id = CLSID_D2D1Composite;
    return S_OK;
}

HRESULT CompositeEffect::GetNamedPropertyMapping(LPCWSTR name, UINT* index, awge::GRAPHICS_EFFECT_PROPERTY_MAPPING* mapping) noexcept
{
    if (index == nullptr || mapping == nullptr) [[unlikely]]
    {
        return E_INVALIDARG;
    }

    const std::wstring_view nameView(name);
    if (nameView == L"Mode")
    {
        *index = D2D1_COMPOSITE_PROP_MODE;
        *mapping = awge::GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT;

        return S_OK;
    }

    return E_INVALIDARG;
}

HRESULT CompositeEffect::GetPropertyCount(UINT* count) noexcept
{
    if (count == nullptr) [[unlikely]]
    {
        return E_INVALIDARG;
    }

    *count = 1;
    return S_OK;
}

HRESULT CompositeEffect::GetProperty(UINT index, winrt::impl::abi_t<winrt::Windows::Foundation::IPropertyValue>** value) noexcept try
{
    if (value == nullptr) [[unlikely]]
    {
        return E_INVALIDARG;
    }

    switch (index)
    {
        case D2D1_COMPOSITE_PROP_MODE:
            *value = wf::PropertyValue::CreateUInt32((UINT32)Mode).as<winrt::impl::abi_t<winrt::Windows::Foundation::IPropertyValue>>().detach();
            break;

        default:
            return E_BOUNDS;
    }

    return S_OK;
}
catch (...)
{
    return winrt::to_hresult();
}

HRESULT CompositeEffect::GetSource(UINT index, awge::IGraphicsEffectSource** source) noexcept try
{
    if (source == nullptr) [[unlikely]]
    {
        return E_INVALIDARG;
    }

    winrt::copy_to_abi(Sources.at(index), *reinterpret_cast<void**>(source));
    return S_OK;
}
catch (...)
{
    return winrt::to_hresult();
}

HRESULT CompositeEffect::GetSourceCount(UINT* count) noexcept
{
    if (count == nullptr) [[unlikely]]
    {
        return E_INVALIDARG;
    }

    *count = static_cast<UINT>(Sources.size());
    return S_OK;
}

winrt::hstring CompositeEffect::Name()
{
    return m_name;
}

void CompositeEffect::Name(winrt::hstring name)
{
    m_name = name;
}

////////////////////////////////////////////////////////////////////////////////
// FloodEffect.h
struct FloodEffect : winrt::implements<FloodEffect, wge::IGraphicsEffect, wge::IGraphicsEffectSource, awge::IGraphicsEffectD2D1Interop>
{
public:
    // IGraphicsEffectD2D1Interop
    HRESULT STDMETHODCALLTYPE GetEffectId(GUID* id) noexcept override;
    HRESULT STDMETHODCALLTYPE GetNamedPropertyMapping(LPCWSTR name, UINT* index, awge::GRAPHICS_EFFECT_PROPERTY_MAPPING* mapping) noexcept override;
    HRESULT STDMETHODCALLTYPE GetPropertyCount(UINT* count) noexcept override;
    HRESULT STDMETHODCALLTYPE GetProperty(UINT index, winrt::impl::abi_t<winrt::Windows::Foundation::IPropertyValue>** value) noexcept override;
    HRESULT STDMETHODCALLTYPE GetSource(UINT index, awge::IGraphicsEffectSource** source) noexcept override;
    HRESULT STDMETHODCALLTYPE GetSourceCount(UINT* count) noexcept override;

    // IGraphicsEffect
    winrt::hstring Name();
    void Name(winrt::hstring name);

    winrt::Windows::UI::Color Color{};
private:
    winrt::hstring m_name = L"FloodEffect";
};

////////////////////////////////////////////////////////////////////////////////
// FloodEffect.cpp
HRESULT FloodEffect::GetEffectId(GUID* id) noexcept
{
    if (id == nullptr) [[unlikely]]
    {
        return E_INVALIDARG;
    }

    *id = CLSID_D2D1Flood;
    return S_OK;
}

HRESULT FloodEffect::GetNamedPropertyMapping(LPCWSTR name, UINT* index, awge::GRAPHICS_EFFECT_PROPERTY_MAPPING* mapping) noexcept
{
    if (index == nullptr || mapping == nullptr) [[unlikely]]
    {
        return E_INVALIDARG;
    }

    const std::wstring_view nameView(name);
    if (nameView == L"Color")
    {
        *index = D2D1_FLOOD_PROP_COLOR;
        *mapping = awge::GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT;

        return S_OK;
    }

    return E_INVALIDARG;
}

HRESULT FloodEffect::GetPropertyCount(UINT* count) noexcept
{
    if (count == nullptr) [[unlikely]]
    {
        return E_INVALIDARG;
    }

    *count = 1;
    return S_OK;
}

HRESULT FloodEffect::GetProperty(UINT index, winrt::impl::abi_t<winrt::Windows::Foundation::IPropertyValue>** value) noexcept try
{
    if (value == nullptr) [[unlikely]]
    {
        return E_INVALIDARG;
    }

    switch (index)
    {
        case D2D1_FLOOD_PROP_COLOR:
            *value = wf::PropertyValue::CreateSingleArray({
                Color.R / 255.0f,
                Color.G / 255.0f,
                Color.B / 255.0f,
                Color.A / 255.0f,
            }).as<winrt::impl::abi_t<winrt::Windows::Foundation::IPropertyValue>>().detach();
            break;

        default:
            return E_BOUNDS;
    }

    return S_OK;
}
catch (...)
{
    return winrt::to_hresult();
}

HRESULT FloodEffect::GetSource(UINT, awge::IGraphicsEffectSource** source) noexcept
{
    if (source == nullptr) [[unlikely]]
    {
        return E_INVALIDARG;
    }

    return E_BOUNDS;
}

HRESULT FloodEffect::GetSourceCount(UINT* count) noexcept
{
    if (count == nullptr) [[unlikely]]
    {
        return E_INVALIDARG;
    }

    *count = 0;
    return S_OK;
}

winrt::hstring FloodEffect::Name()
{
    return m_name;
}

void FloodEffect::Name(winrt::hstring name)
{
    m_name = name;
}

////////////////////////////////////////////////////////////////////////////////
// BorderEffect.h
struct BorderEffect : winrt::implements<BorderEffect, wge::IGraphicsEffect, wge::IGraphicsEffectSource, awge::IGraphicsEffectD2D1Interop>
{
public:
    // IGraphicsEffectD2D1Interop
    HRESULT STDMETHODCALLTYPE GetEffectId(GUID* id) noexcept override;
    HRESULT STDMETHODCALLTYPE GetNamedPropertyMapping(LPCWSTR name, UINT* index, awge::GRAPHICS_EFFECT_PROPERTY_MAPPING* mapping) noexcept override;
    HRESULT STDMETHODCALLTYPE GetPropertyCount(UINT* count) noexcept override;
    HRESULT STDMETHODCALLTYPE GetProperty(UINT index, winrt::impl::abi_t<winrt::Windows::Foundation::IPropertyValue>** value) noexcept override;
    HRESULT STDMETHODCALLTYPE GetSource(UINT index, awge::IGraphicsEffectSource** source) noexcept override;
    HRESULT STDMETHODCALLTYPE GetSourceCount(UINT* count) noexcept override;

    // IGraphicsEffect
    winrt::hstring Name();
    void Name(winrt::hstring name);

    wge::IGraphicsEffectSource Source{nullptr};
    D2D1_BORDER_EDGE_MODE ExtendX = D2D1_BORDER_EDGE_MODE_WRAP;
    D2D1_BORDER_EDGE_MODE ExtendY = D2D1_BORDER_EDGE_MODE_WRAP;
private:
    winrt::hstring m_name = L"BorderEffect";
};

////////////////////////////////////////////////////////////////////////////////
// BorderEffect.cpp
HRESULT BorderEffect::GetEffectId(GUID* id) noexcept
{
    if (!id)
    {
        return E_INVALIDARG;
    }

    *id = CLSID_D2D1Border;
    return S_OK;
}

HRESULT BorderEffect::GetNamedPropertyMapping(LPCWSTR name, UINT* index, awge::GRAPHICS_EFFECT_PROPERTY_MAPPING* mapping) noexcept
{
    if (!index || !mapping)
    {
        return E_INVALIDARG;
    }

    const std::wstring_view nameView(name);
    if (nameView == L"ExtendX")
    {
        *index = D2D1_BORDER_PROP_EDGE_MODE_X;
        *mapping = awge::GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT;

        return S_OK;
    }

    if (nameView == L"ExtendY")
    {
        *index = D2D1_BORDER_PROP_EDGE_MODE_Y;
        *mapping = awge::GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT;

        return S_OK;
    }

    return E_INVALIDARG;
}

HRESULT BorderEffect::GetPropertyCount(UINT* count) noexcept
{
    if (!count)
    {
        return E_INVALIDARG;
    }

    *count = 2;
    return S_OK;
}

HRESULT BorderEffect::GetProperty(UINT index, winrt::impl::abi_t<winrt::Windows::Foundation::IPropertyValue>** value) noexcept try
{
    if (!value)
    {
        return E_INVALIDARG;
    }

    switch (index)
    {
        case D2D1_BORDER_PROP_EDGE_MODE_X:
            *value = wf::PropertyValue::CreateUInt32((UINT32)ExtendX).as<winrt::impl::abi_t<winrt::Windows::Foundation::IPropertyValue>>().detach();
            break;

        case D2D1_BORDER_PROP_EDGE_MODE_Y:
            *value = wf::PropertyValue::CreateUInt32((UINT32)ExtendY).as<winrt::impl::abi_t<winrt::Windows::Foundation::IPropertyValue>>().detach();
            break;

        default:
            return E_BOUNDS;
    }

    return S_OK;
}
catch (...)
{
    return winrt::to_hresult();
}

HRESULT BorderEffect::GetSource(UINT index, awge::IGraphicsEffectSource** source) noexcept
{
    if (!source)
    {
        return E_INVALIDARG;
    }

    if (index == 0 && Source)
    {
        winrt::copy_to_abi(Source, *reinterpret_cast<void**>(source));
        return S_OK;
    }

    return E_BOUNDS;
}

HRESULT BorderEffect::GetSourceCount(UINT* count) noexcept
{
    if (!count)
    {
        return E_INVALIDARG;
    }

    *count = 1;
    return S_OK;
}

winrt::hstring BorderEffect::Name()
{
    return m_name;
}

void BorderEffect::Name(winrt::hstring name)
{
    m_name = name;
}

////////////////////////////////////////////////////////////////////////////////
// GaussianBlurEffect.h
struct GaussianBlurEffect : winrt::implements<GaussianBlurEffect, wge::IGraphicsEffect, wge::IGraphicsEffectSource, awge::IGraphicsEffectD2D1Interop>
{
public:
    // IGraphicsEffectD2D1Interop
    HRESULT STDMETHODCALLTYPE GetEffectId(GUID* id) noexcept override;
    HRESULT STDMETHODCALLTYPE GetNamedPropertyMapping(LPCWSTR name, UINT* index, awge::GRAPHICS_EFFECT_PROPERTY_MAPPING* mapping) noexcept override;
    HRESULT STDMETHODCALLTYPE GetPropertyCount(UINT* count) noexcept override;
    HRESULT STDMETHODCALLTYPE GetProperty(UINT index, winrt::impl::abi_t<winrt::Windows::Foundation::IPropertyValue>** value) noexcept override;
    HRESULT STDMETHODCALLTYPE GetSource(UINT index, awge::IGraphicsEffectSource** source) noexcept override;
    HRESULT STDMETHODCALLTYPE GetSourceCount(UINT* count) noexcept override;

    // IGraphicsEffect
    winrt::hstring Name();
    void Name(winrt::hstring name);

    wge::IGraphicsEffectSource Source;

    float BlurAmount = 3.0f;
    MY_D2D1_GAUSSIANBLUR_OPTIMIZATION Optimization = MY_D2D1_GAUSSIANBLUR_OPTIMIZATION_BALANCED;
    D2D1_BORDER_MODE BorderMode = D2D1_BORDER_MODE_SOFT;
private:
    winrt::hstring m_name = L"GaussianBlurEffect";
};

////////////////////////////////////////////////////////////////////////////////
// GaussianBlurEffect.cpp
HRESULT GaussianBlurEffect::GetEffectId(GUID* id) noexcept
{
    if (id == nullptr) [[unlikely]]
    {
        return E_INVALIDARG;
    }

    *id = CLSID_D2D1GaussianBlur;
    return S_OK;
}

HRESULT GaussianBlurEffect::GetNamedPropertyMapping(LPCWSTR name, UINT* index, awge::GRAPHICS_EFFECT_PROPERTY_MAPPING* mapping) noexcept
{
    if (index == nullptr || mapping == nullptr) [[unlikely]]
    {
        return E_INVALIDARG;
    }

    const std::wstring_view nameView(name);
    if (nameView == L"BlurAmount")
    {
        *index = D2D1_GAUSSIANBLUR_PROP_STANDARD_DEVIATION;
        *mapping = awge::GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT;

        return S_OK;
    }
    else if (nameView == L"Optimization")
    {
        *index = D2D1_GAUSSIANBLUR_PROP_OPTIMIZATION;
        *mapping = awge::GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT;

        return S_OK;
    }
    else if (nameView == L"BorderMode")
    {
        *index = D2D1_GAUSSIANBLUR_PROP_BORDER_MODE;
        *mapping = awge::GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT;

        return S_OK;
    }

    return E_INVALIDARG;
}

HRESULT GaussianBlurEffect::GetPropertyCount(UINT* count) noexcept
{
    if (count == nullptr) [[unlikely]]
    {
        return E_INVALIDARG;
    }

    *count = 3;
    return S_OK;
}

HRESULT GaussianBlurEffect::GetProperty(UINT index, winrt::impl::abi_t<winrt::Windows::Foundation::IPropertyValue>** value) noexcept try
{
    if (value == nullptr) [[unlikely]]
    {
        return E_INVALIDARG;
    }

    switch (index)
    {
        case D2D1_GAUSSIANBLUR_PROP_STANDARD_DEVIATION:
            *value = wf::PropertyValue::CreateSingle(BlurAmount).as<winrt::impl::abi_t<winrt::Windows::Foundation::IPropertyValue>>().detach();
            break;

        case D2D1_GAUSSIANBLUR_PROP_OPTIMIZATION:
            *value = wf::PropertyValue::CreateUInt32((UINT32)Optimization).as<winrt::impl::abi_t<winrt::Windows::Foundation::IPropertyValue>>().detach();
            break;

        case D2D1_GAUSSIANBLUR_PROP_BORDER_MODE:
            *value = wf::PropertyValue::CreateUInt32((UINT32)BorderMode).as<winrt::impl::abi_t<winrt::Windows::Foundation::IPropertyValue>>().detach();
            break;

        default:
            return E_BOUNDS;
    }

    return S_OK;
}
catch (...)
{
    return winrt::to_hresult();
}

HRESULT GaussianBlurEffect::GetSource(UINT index, awge::IGraphicsEffectSource** source) noexcept
{
    if (source == nullptr) [[unlikely]]
    {
        return E_INVALIDARG;
    }

    if (index == 0)
    {
        winrt::copy_to_abi(Source, *reinterpret_cast<void**>(source));
        return S_OK;
    }
    else
    {
        return E_BOUNDS;
    }
}

HRESULT GaussianBlurEffect::GetSourceCount(UINT* count) noexcept
{
    if (count == nullptr) [[unlikely]]
    {
        return E_INVALIDARG;
    }

    *count = 1;
    return S_OK;
}

winrt::hstring GaussianBlurEffect::Name()
{
    return m_name;
}

void GaussianBlurEffect::Name(winrt::hstring name)
{
    m_name = name;
}

////////////////////////////////////////////////////////////////////////////////
// ColorMatrixEffect.h
struct ColorMatrixEffect : winrt::implements<ColorMatrixEffect, wge::IGraphicsEffect, wge::IGraphicsEffectSource, awge::IGraphicsEffectD2D1Interop>
{
public:
    // IGraphicsEffectD2D1Interop
    HRESULT STDMETHODCALLTYPE GetEffectId(GUID* id) noexcept override;
    HRESULT STDMETHODCALLTYPE GetNamedPropertyMapping(LPCWSTR name, UINT* index, awge::GRAPHICS_EFFECT_PROPERTY_MAPPING* mapping) noexcept override;
    HRESULT STDMETHODCALLTYPE GetPropertyCount(UINT* count) noexcept override;
    HRESULT STDMETHODCALLTYPE GetProperty(UINT index, winrt::impl::abi_t<winrt::Windows::Foundation::IPropertyValue>** value) noexcept override;
    HRESULT STDMETHODCALLTYPE GetSource(UINT index, awge::IGraphicsEffectSource** source) noexcept override;
    HRESULT STDMETHODCALLTYPE GetSourceCount(UINT* count) noexcept override;

    // IGraphicsEffect
    winrt::hstring Name();
    void Name(winrt::hstring name);

    wge::IGraphicsEffectSource Source{nullptr};

    // D2D1_MATRIX_5X4_F: 5 rows x 4 columns (20 floats), initialized to identity.
    float Matrix[20] = {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1,
        0, 0, 0, 0,
    };

    uint32_t AlphaMode = D2D1_COLORMATRIX_ALPHA_MODE_PREMULTIPLIED;
    bool ClampOutput = false;
private:
    winrt::hstring m_name = L"ColorMatrixEffect";
};

////////////////////////////////////////////////////////////////////////////////
// ColorMatrixEffect.cpp
HRESULT ColorMatrixEffect::GetEffectId(GUID* id) noexcept
{
    if (!id)
    {
        return E_INVALIDARG;
    }

    *id = CLSID_D2D1ColorMatrix;
    return S_OK;
}

HRESULT ColorMatrixEffect::GetNamedPropertyMapping(LPCWSTR name, UINT* index, awge::GRAPHICS_EFFECT_PROPERTY_MAPPING* mapping) noexcept
{
    if (!index || !mapping)
    {
        return E_INVALIDARG;
    }

    const std::wstring_view nameView(name);
    if (nameView == L"ColorMatrix")
    {
        *index = D2D1_COLORMATRIX_PROP_COLOR_MATRIX;
        *mapping = awge::GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT;

        return S_OK;
    }

    if (nameView == L"AlphaMode")
    {
        *index = D2D1_COLORMATRIX_PROP_ALPHA_MODE;
        *mapping = awge::GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT;

        return S_OK;
    }

    if (nameView == L"ClampOutput")
    {
        *index = D2D1_COLORMATRIX_PROP_CLAMP_OUTPUT;
        *mapping = awge::GRAPHICS_EFFECT_PROPERTY_MAPPING_DIRECT;

        return S_OK;
    }

    return E_INVALIDARG;
}

HRESULT ColorMatrixEffect::GetPropertyCount(UINT* count) noexcept
{
    if (!count)
    {
        return E_INVALIDARG;
    }

    *count = 3;
    return S_OK;
}

HRESULT ColorMatrixEffect::GetProperty(UINT index, winrt::impl::abi_t<winrt::Windows::Foundation::IPropertyValue>** value) noexcept try
{
    if (!value)
    {
        return E_INVALIDARG;
    }

    switch (index)
    {
        case D2D1_COLORMATRIX_PROP_COLOR_MATRIX:
            *value = wf::PropertyValue::CreateSingleArray(
                winrt::array_view<const float>(Matrix, Matrix + 20)
            ).as<winrt::impl::abi_t<winrt::Windows::Foundation::IPropertyValue>>().detach();
            break;

        case D2D1_COLORMATRIX_PROP_ALPHA_MODE:
            *value = wf::PropertyValue::CreateUInt32(AlphaMode).as<winrt::impl::abi_t<winrt::Windows::Foundation::IPropertyValue>>().detach();
            break;

        case D2D1_COLORMATRIX_PROP_CLAMP_OUTPUT:
            *value = wf::PropertyValue::CreateBoolean(ClampOutput).as<winrt::impl::abi_t<winrt::Windows::Foundation::IPropertyValue>>().detach();
            break;

        default:
            return E_BOUNDS;
    }

    return S_OK;
}
catch (...)
{
    return winrt::to_hresult();
}

HRESULT ColorMatrixEffect::GetSource(UINT index, awge::IGraphicsEffectSource** source) noexcept
{
    if (!source)
    {
        return E_INVALIDARG;
    }

    if (index == 0 && Source)
    {
        winrt::copy_to_abi(Source, *reinterpret_cast<void**>(source));
        return S_OK;
    }

    return E_BOUNDS;
}

HRESULT ColorMatrixEffect::GetSourceCount(UINT* count) noexcept
{
    if (!count)
    {
        return E_INVALIDARG;
    }

    *count = 1;
    return S_OK;
}

winrt::hstring ColorMatrixEffect::Name()
{
    return m_name;
}

void ColorMatrixEffect::Name(winrt::hstring name)
{
    m_name = name;
}

////////////////////////////////////////////////////////////////////////////////
// XamlBlurBrush.cpp
XamlBlurBrush::XamlBlurBrush(UIElement element,
                             float blurAmount,
                             winrt::Windows::UI::Color tint,
                             std::optional<uint8_t> tintOpacity,
                             winrt::hstring tintThemeResourceKey,
                             std::optional<float> tintLuminosityOpacity,
                             std::optional<float> tintSaturation,
                             std::optional<float> noiseOpacity,
                             std::optional<float> noiseDensity,
                             std::optional<winrt::Windows::UI::Color> fallbackColor,
                             winrt::hstring fallbackThemeResourceKey) :
    m_compositor(wuxh::ElementCompositionPreview::GetElementVisual(element)
                     .Compositor()),
    m_blurAmount(blurAmount),
    m_tint(tint),
    m_tintOpacity(tintOpacity),
    m_tintThemeResourceKey(std::move(tintThemeResourceKey)),
    m_tintLuminosityOpacity(tintLuminosityOpacity),
    m_tintSaturation(tintSaturation),
    m_noiseOpacity(noiseOpacity),
    m_noiseDensity(noiseDensity),
    m_fallbackColor(fallbackColor),
    m_fallbackThemeResourceKey(std::move(fallbackThemeResourceKey))
{
    auto fe = element.try_as<FrameworkElement>();

    auto createProxy = [&](winrt::hstring const& themeResourceKey)
        -> Media::SolidColorBrush
    {
        if (!fe)
        {
            return nullptr;
        }
        std::wstring xaml =
            L"<SolidColorBrush"
            L" xmlns=\"http://schemas.microsoft.com/winfx/2006/xaml/"
            L"presentation\""
            L" Color=\"{ThemeResource " +
            std::wstring(themeResourceKey) + L"}\"/>";
        try
        {
            return Markup::XamlReader::Load(winrt::hstring(xaml))
                .try_as<Media::SolidColorBrush>();
        }
        catch (winrt::hresult_error const& ex)
        {
            Wh_Log(L"Failed to create proxy brush: %08X", ex.code());
            return nullptr;
        }
    };

    static std::atomic<uint64_t> s_proxyCounter{0};

    if (!m_tintThemeResourceKey.empty())
    {
        if (auto proxyBrush = createProxy(m_tintThemeResourceKey))
        {
            auto proxyKey = winrt::hstring(
                L"__WhBlurProxy_" +
                std::to_wstring(++s_proxyCounter));
            fe.Resources().Insert(
                winrt::box_value(proxyKey), proxyBrush);
            m_proxyBrush = proxyBrush;
            m_weakProxyElement = winrt::make_weak(fe);
            m_proxyKey = proxyKey;
            Wh_Log(L"Tint proxy brush for %s inserted with key %s",
                   m_tintThemeResourceKey.c_str(),
                   proxyKey.c_str());
        }

        if (m_proxyBrush)
        {
            m_proxyBrush.RegisterPropertyChangedCallback(
                Media::SolidColorBrush::ColorProperty(),
                [weakThis = get_weak()](auto&&, auto&&)
                {
                    if (auto self = weakThis.get())
                    {
                        Wh_Log(L"Tint theme color changed");
                        self->RefreshBrush();
                    }
                });
        }
    }

    if (!m_fallbackThemeResourceKey.empty())
    {
        if (auto proxyBrush = createProxy(m_fallbackThemeResourceKey))
        {
            auto proxyKey = winrt::hstring(
                L"__WhBlurFallbackProxy_" +
                std::to_wstring(++s_proxyCounter));
            fe.Resources().Insert(
                winrt::box_value(proxyKey), proxyBrush);
            m_fallbackProxyBrush = proxyBrush;
            if (!m_weakProxyElement.get())
            {
                m_weakProxyElement = winrt::make_weak(fe);
            }
            m_fallbackProxyKey = proxyKey;
            Wh_Log(L"Fallback proxy brush for %s inserted with key %s",
                   m_fallbackThemeResourceKey.c_str(),
                   proxyKey.c_str());
        }

        if (m_fallbackProxyBrush)
        {
            m_fallbackProxyBrush.RegisterPropertyChangedCallback(
                Media::SolidColorBrush::ColorProperty(),
                [weakThis = get_weak()](auto&&, auto&&)
                {
                    if (auto self = weakThis.get())
                    {
                        Wh_Log(L"Fallback theme color changed");
                        self->RefreshBrush();
                    }
                });
        }
    }

    if (m_fallbackColor || !m_fallbackThemeResourceKey.empty())
    {
        m_dispatcher =
            winrt::Windows::System::DispatcherQueue::GetForCurrentThread();

        try
        {
            m_uiSettings = winrt::Windows::UI::ViewManagement::UISettings();
            auto dispatcher = m_dispatcher;
            m_advancedEffectsEnabledChangedToken =
                m_uiSettings.AdvancedEffectsEnabledChanged(
                    [weakThis = get_weak(), dispatcher](auto&&, auto&&)
                    {
                        dispatcher.TryEnqueue([weakThis]
                        {
                            if (auto self = weakThis.get())
                            {
                                Wh_Log(L"AdvancedEffectsEnabled changed");
                                self->RefreshBrush();
                            }
                        });
                    });
            m_energySaverStatusChangedToken =
                winrt::Windows::System::Power::PowerManager::
                    EnergySaverStatusChanged(
                        [weakThis = get_weak(), dispatcher](auto&&, auto&&)
                        {
                            dispatcher.TryEnqueue([weakThis]
                            {
                                if (auto self = weakThis.get())
                                {
                                    Wh_Log(L"EnergySaverStatus changed");
                                    self->RefreshBrush();
                                }
                            });
                        });
        }
        catch (winrt::hresult_error const& ex)
        {
            Wh_Log(L"Failed to register fallback state listeners: %08X",
                   ex.code());
        }

        // Watch HKLM\SYSTEM\CurrentControlSet\Control\Power for changes to
        // EnergySaverState. On Windows 11 24H2+ neither the WinRT
        // PowerManager.EnergySaverStatus property nor the Win32
        // GetSystemPowerStatus.SystemStatusFlag flag reliably reflects the
        // "Always use energy saver" setting; the registry value is the only
        // signal that updates in that case. The wait callback re-arms the
        // notification and posts a brush refresh on the UI thread.
        LONG regStatus = RegOpenKeyExW(
            HKEY_LOCAL_MACHINE,
            L"SYSTEM\\CurrentControlSet\\Control\\Power", 0, KEY_NOTIFY,
            &m_powerKey);
        if (regStatus == ERROR_SUCCESS)
        {
            m_regNotifyEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
            if (m_regNotifyEvent)
            {
                regStatus = RegNotifyChangeKeyValue(m_powerKey, FALSE,
                                                   REG_NOTIFY_CHANGE_LAST_SET,
                                                   m_regNotifyEvent, TRUE);
                if (regStatus == ERROR_SUCCESS)
                {
                    if (!RegisterWaitForSingleObject(
                            &m_regWaitHandle, m_regNotifyEvent,
                            OnEnergySaverRegistryChanged, this, INFINITE,
                            WT_EXECUTEINWAITTHREAD))
                    {
                        Wh_Log(L"RegisterWaitForSingleObject failed: %lu",
                               GetLastError());
                        m_regWaitHandle = nullptr;
                    }
                }
                else
                {
                    Wh_Log(L"RegNotifyChangeKeyValue failed: %ld", regStatus);
                    CloseHandle(m_regNotifyEvent);
                    m_regNotifyEvent = nullptr;
                    RegCloseKey(m_powerKey);
                    m_powerKey = nullptr;
                }
            }
            else
            {
                Wh_Log(L"CreateEvent failed: %lu", GetLastError());
                RegCloseKey(m_powerKey);
                m_powerKey = nullptr;
            }
        }
        else
        {
            Wh_Log(L"RegOpenKeyEx for Power key failed: %ld", regStatus);
        }
    }
}

void CALLBACK XamlBlurBrush::OnEnergySaverRegistryChanged(PVOID context,
                                                          BOOLEAN)
{
    auto* self = static_cast<XamlBlurBrush*>(context);

    // Re-arm before dispatching so a rapid second change isn't dropped.
    if (self->m_powerKey && self->m_regNotifyEvent)
    {
        RegNotifyChangeKeyValue(self->m_powerKey, FALSE,
                                REG_NOTIFY_CHANGE_LAST_SET,
                                self->m_regNotifyEvent, TRUE);
    }

    if (self->m_dispatcher)
    {
        auto weakThis = self->get_weak();
        self->m_dispatcher.TryEnqueue([weakThis]
        {
            if (auto strongThis = weakThis.get())
            {
                Wh_Log(L"Power registry key changed, refreshing brush");
                strongThis->RefreshBrush();
            }
        });
    }
}

XamlBlurBrush::~XamlBlurBrush()
{
    // Tear down the registry watch first so no more callbacks can fire while
    // we close the underlying handles.
    if (m_regWaitHandle)
    {
        UnregisterWaitEx(m_regWaitHandle, INVALID_HANDLE_VALUE);
        m_regWaitHandle = nullptr;
    }
    if (m_regNotifyEvent)
    {
        CloseHandle(m_regNotifyEvent);
        m_regNotifyEvent = nullptr;
    }
    if (m_powerKey)
    {
        RegCloseKey(m_powerKey);
        m_powerKey = nullptr;
    }

    if (m_uiSettings && m_advancedEffectsEnabledChangedToken.value)
    {
        try
        {
            m_uiSettings.AdvancedEffectsEnabledChanged(
                m_advancedEffectsEnabledChangedToken);
        }
        catch (...)
        {
            Wh_Log(L"Error %08X", winrt::to_hresult());
        }
    }

    if (m_energySaverStatusChangedToken.value)
    {
        try
        {
            winrt::Windows::System::Power::PowerManager::
                EnergySaverStatusChanged(m_energySaverStatusChangedToken);
        }
        catch (...)
        {
            Wh_Log(L"Error %08X", winrt::to_hresult());
        }
    }

    if (auto element = m_weakProxyElement.get())
    {
        try
        {
            if (!m_proxyKey.empty())
            {
                element.Resources().Remove(winrt::box_value(m_proxyKey));
            }
            if (!m_fallbackProxyKey.empty())
            {
                element.Resources().Remove(
                    winrt::box_value(m_fallbackProxyKey));
            }
        }
        catch (...)
        {
            HRESULT hr = winrt::to_hresult();
            Wh_Log(L"Error %08X", hr);
        }
    }
}

void XamlBlurBrush::OnConnected()
{
    if (!CompositionBrush())
    {
        RefreshThemeTint();
        RefreshFallbackColor();

        CompositionBrush(ShouldUseFallback() ? CreateFallbackBrush()
                                             : CreateEffectBrush());
    }
}

wuc::CompositionBrush XamlBlurBrush::CreateFallbackBrush()
{
    return m_compositor.CreateColorBrush(m_fallbackColor.value_or(m_tint));
}

wuc::CompositionBrush XamlBlurBrush::CreateEffectBrush()
{
    auto backdropBrush = m_compositor.CreateBackdropBrush();

    // Rec. 709 luma coefficients, used for saturation and luminosity.
    constexpr float kLumaR = 0.2126f;
    constexpr float kLumaG = 0.7152f;
    constexpr float kLumaB = 0.0722f;

    // 1. Blur
    auto blurEffect = winrt::make_self<GaussianBlurEffect>();
    blurEffect->Source = wuc::CompositionEffectSourceParameter(L"backdrop");
    blurEffect->BlurAmount = m_blurAmount;
    blurEffect->Name(L"BlurEffect");

    wge::IGraphicsEffectSource topOfStack = *blurEffect;

    // 2. Saturation (optional)
    if (m_tintSaturation && *m_tintSaturation != 1.0f)
    {
        float s = std::max(*m_tintSaturation, 0.0f);
        float invS = 1.0f - s;

        auto satMatrix = winrt::make_self<ColorMatrixEffect>();
        satMatrix->Source = topOfStack;

        // Standard saturation matrix: lerp between luminance and identity.
        auto& m = satMatrix->Matrix;
        m[0]  = invS * kLumaR + s; m[1]  = invS * kLumaR;     m[2]  = invS * kLumaR;     m[3]  = 0.0f;
        m[4]  = invS * kLumaG;     m[5]  = invS * kLumaG + s; m[6]  = invS * kLumaG;     m[7]  = 0.0f;
        m[8]  = invS * kLumaB;     m[9]  = invS * kLumaB;     m[10] = invS * kLumaB + s; m[11] = 0.0f;
        m[12] = 0.0f;              m[13] = 0.0f;              m[14] = 0.0f;              m[15] = 1.0f;

        satMatrix->Name(L"SaturationEffect");
        topOfStack = *satMatrix;
    }

    // 3. Luminosity (optional) - shifts pixel luminance towards the tint's
    // luminance, blended by the opacity factor.
    if (m_tintLuminosityOpacity && *m_tintLuminosityOpacity > 0.0f)
    {
        float op = std::clamp(*m_tintLuminosityOpacity, 0.0f, 1.0f);

        float tintLum = (m_tint.R / 255.0f) * kLumaR +
                        (m_tint.G / 255.0f) * kLumaG +
                        (m_tint.B / 255.0f) * kLumaB;

        auto lumMatrix = winrt::make_self<ColorMatrixEffect>();
        lumMatrix->Source = topOfStack;

        auto& m = lumMatrix->Matrix;
        m[0]  = 1.0f - (kLumaR * op); m[1]  = -(kLumaR * op);       m[2]  = -(kLumaR * op);       m[3]  = 0.0f;
        m[4]  = -(kLumaG * op);       m[5]  = 1.0f - (kLumaG * op); m[6]  = -(kLumaG * op);       m[7]  = 0.0f;
        m[8]  = -(kLumaB * op);       m[9]  = -(kLumaB * op);       m[10] = 1.0f - (kLumaB * op); m[11] = 0.0f;
        m[12] = 0.0f;                 m[13] = 0.0f;                 m[14] = 0.0f;                 m[15] = 1.0f;
        m[16] = tintLum * op;         m[17] = tintLum * op;         m[18] = tintLum * op;         m[19] = 0.0f;

        lumMatrix->Name(L"LuminosityBlend");
        topOfStack = *lumMatrix;
    }

    // 4. Noise overlay (optional) - procedural tiled noise with adjustable
    // density and opacity.
    wuc::CompositionSurfaceBrush noiseBrush{nullptr};
    if (m_noiseOpacity && *m_noiseOpacity > 0.0f)
    {
        float density = m_noiseDensity.value_or(1.0f);

        auto stream = CreateNoiseStream(density);
        auto surface =
            Media::LoadedImageSurface::StartLoadFromStream(stream);
        noiseBrush = m_compositor.CreateSurfaceBrush(surface);
        noiseBrush.Stretch(wuc::CompositionStretch::None);

        // Tile via border effect (wrap mode).
        auto borderEffect = winrt::make_self<BorderEffect>();
        borderEffect->Source =
            wuc::CompositionEffectSourceParameter(L"NoiseSource");

        // Scale all channels by opacity for premultiplied blending.
        float nOp = std::clamp(*m_noiseOpacity, 0.0f, 1.0f);

        auto opacityEffect = winrt::make_self<ColorMatrixEffect>();
        opacityEffect->Source = *borderEffect;
        // Matrix: Scale all channels by opacity (for premultiplied blending).
        opacityEffect->Matrix[0] = nOp;
        opacityEffect->Matrix[5] = nOp;
        opacityEffect->Matrix[10] = nOp;
        opacityEffect->Matrix[15] = nOp;
        opacityEffect->Name(L"NoiseOpacityEffect");

        // Composite noise over the current stack.
        auto noiseComposite = winrt::make_self<CompositeEffect>();
        noiseComposite->Mode = D2D1_COMPOSITE_MODE_SOURCE_OVER;
        noiseComposite->Sources.push_back(topOfStack);
        noiseComposite->Sources.push_back(*opacityEffect);
        noiseComposite->Name(L"NoiseComposite");
        topOfStack = *noiseComposite;
    }

    // 5. Tint (flood color composited over the stack).
    auto floodEffect = winrt::make_self<FloodEffect>();
    floodEffect->Color = m_tint;
    floodEffect->Name(L"FloodEffect");

    auto compositeEffect = winrt::make_self<CompositeEffect>();
    compositeEffect->Mode = D2D1_COMPOSITE_MODE_SOURCE_OVER;
    compositeEffect->Sources.push_back(topOfStack);
    compositeEffect->Sources.push_back(*floodEffect);

    auto factory = m_compositor.CreateEffectFactory(*compositeEffect);
    auto brush = factory.CreateBrush();

    brush.SetSourceParameter(L"backdrop", backdropBrush);

    // Bind the noise brush if we created one.
    if (noiseBrush)
    {
        brush.SetSourceParameter(L"NoiseSource", noiseBrush);
    }

    return brush;
}

void XamlBlurBrush::OnDisconnected()
{
    if (const auto brush = CompositionBrush())
    {
        brush.Close();
        CompositionBrush(nullptr);
    }
}

void XamlBlurBrush::RefreshThemeTint()
{
    if (!m_proxyBrush)
    {
        return;
    }

    m_tint = m_proxyBrush.Color();
    if (m_tintOpacity)
    {
        m_tint.A = *m_tintOpacity;
    }
}

void XamlBlurBrush::RefreshFallbackColor()
{
    if (!m_fallbackProxyBrush)
    {
        return;
    }

    m_fallbackColor = m_fallbackProxyBrush.Color();
}

bool XamlBlurBrush::ShouldUseFallback() const
{
    if (!m_fallbackColor && m_fallbackThemeResourceKey.empty())
    {
        return false;
    }

    // The HKLM\SYSTEM\CurrentControlSet\Control\Power\EnergySaverState value
    // is the only signal that consistently reflects "Always use energy saver"
    // on Windows 11 24H2+; the WinRT and Win32 power-status APIs can stay
    // stuck in the off state on those builds. 1 = enabled, 2 = disabled.
    bool energySaverActive = false;
    HKEY key{};
    if (RegOpenKeyExW(HKEY_LOCAL_MACHINE,
                      L"SYSTEM\\CurrentControlSet\\Control\\Power", 0,
                      KEY_QUERY_VALUE, &key) == ERROR_SUCCESS)
    {
        DWORD value = 0;
        DWORD type = 0;
        DWORD size = sizeof(value);
        if (RegQueryValueExW(key, L"EnergySaverState", nullptr, &type,
                             reinterpret_cast<LPBYTE>(&value),
                             &size) == ERROR_SUCCESS &&
            type == REG_DWORD)
        {
            energySaverActive = (value == 1);
        }
        RegCloseKey(key);
    }

    // Backup for older Windows where the registry value above isn't populated.
    if (!energySaverActive)
    {
        SYSTEM_POWER_STATUS powerStatus{};
        if (GetSystemPowerStatus(&powerStatus) &&
            powerStatus.SystemStatusFlag != 0)
        {
            energySaverActive = true;
        }
    }

    bool advancedEffectsOff = false;
    if (m_uiSettings)
    {
        try
        {
            advancedEffectsOff = !m_uiSettings.AdvancedEffectsEnabled();
        }
        catch (...)
        {
            Wh_Log(L"AdvancedEffectsEnabled query failed: %08X",
                   winrt::to_hresult());
        }
    }

    return energySaverActive || advancedEffectsOff;
}

void XamlBlurBrush::RefreshBrush()
{
    if (const auto brush = CompositionBrush())
    {
        brush.Close();
        CompositionBrush(nullptr);
        OnConnected();
    }
}

// clang-format on
////////////////////////////////////////////////////////////////////////////////

// Helper functions for tracking and retrying failed ImageBrush loads.
void RetryFailedImageLoadsOnCurrentThread() {
    Wh_Log(L"Retrying failed image loads on current thread");

    auto& failedImageBrushes = g_failedImageBrushesForThread.failedImageBrushes;

    // Retry loading all failed images by re-setting the ImageSource property.
    for (auto& info : failedImageBrushes) {
        if (auto brush = info.brush.get()) {
            try {
                Wh_Log(L"Retrying image load for: %s",
                       info.imageSource.c_str());
                // Clear the ImageSource first to force a reload.
                brush.ImageSource(nullptr);
                // Then create a new BitmapImage and set it.
                Media::Imaging::BitmapImage bitmapImage;
                bitmapImage.UriSource(
                    winrt::Windows::Foundation::Uri(info.imageSource));
                brush.ImageSource(bitmapImage);
            } catch (winrt::hresult_error const& ex) {
                Wh_Log(L"Error retrying image load %08X: %s", ex.code(),
                       ex.message().c_str());
            }
        }
    }

    // Clean up any weak refs that are no longer valid.
    std::erase_if(failedImageBrushes,
                  [](const auto& info) { return !info.brush.get(); });
}

void OnNetworkStatusChanged(
    winrt::Windows::Foundation::IInspectable const& sender) {
    Wh_Log(L"Network status changed, dispatching retry to all UI threads");

    // Get snapshot of dispatchers under lock.
    std::vector<winrt::Windows::System::DispatcherQueue> dispatchers;
    {
        std::lock_guard<std::mutex> lock(g_failedImageBrushesRegistryMutex);

        for (auto& weakDispatcher : g_failedImageBrushesRegistry) {
            if (auto dispatcher = weakDispatcher.get()) {
                dispatchers.push_back(dispatcher);
            }
        }

        // Clean up dead weak refs.
        std::erase_if(
            g_failedImageBrushesRegistry,
            [](const auto& weakDispatcher) { return !weakDispatcher.get(); });
    }

    // Dispatch retry to each UI thread.
    for (auto& dispatcher : dispatchers) {
        try {
            dispatcher.TryEnqueue(
                []() { RetryFailedImageLoadsOnCurrentThread(); });
        } catch (winrt::hresult_error const& ex) {
            Wh_Log(L"Error dispatching retry to UI thread %08X: %s", ex.code(),
                   ex.message().c_str());
        }
    }
}

void RemoveFromFailedImageBrushes(Media::ImageBrush const& brush) {
    auto& failedImageBrushes = g_failedImageBrushesForThread.failedImageBrushes;

    std::erase_if(failedImageBrushes, [&brush](const auto& info) {
        if (auto existingBrush = info.brush.get()) {
            return existingBrush == brush;
        }
        return false;
    });
}

void SetupImageBrushTracking(Media::ImageBrush const& brush,
                             winrt::hstring const& imageSourceUrl) {
    // First remove any existing entry for this brush to avoid duplicates.
    RemoveFromFailedImageBrushes(brush);

    // Add new entry with event handlers.
    ImageBrushFailedLoadInfo info;
    info.brush = winrt::make_weak(brush);
    info.imageSource = imageSourceUrl;

    // Set up ImageFailed event handler - add to list only when load fails.
    info.imageFailedRevoker = brush.ImageFailed(
        winrt::auto_revoke,
        [brushWeak = winrt::make_weak(brush), imageSourceUrl](
            winrt::Windows::Foundation::IInspectable const& sender,
            ExceptionRoutedEventArgs const& e) {
            Wh_Log(L"ImageBrush load failed for: %s, error: %s",
                   imageSourceUrl.c_str(), e.ErrorMessage().c_str());
            // The brush should already be in the list, no action needed here as
            // we add it preemptively in SetupImageBrushTracking.
        });

    // Set up ImageOpened event handler - remove from list when load succeeds.
    info.imageOpenedRevoker = brush.ImageOpened(
        winrt::auto_revoke,
        [brushWeak = winrt::make_weak(brush)](
            winrt::Windows::Foundation::IInspectable const& sender,
            RoutedEventArgs const& e) {
            Wh_Log(L"ImageBrush loaded successfully, removing from retry list");

            if (auto brush = brushWeak.get()) {
                RemoveFromFailedImageBrushes(brush);
            }
        });

    // Add to the list preemptively - will be removed if load succeeds.
    auto& failedImageBrushes = g_failedImageBrushesForThread.failedImageBrushes;
    failedImageBrushes.push_back(std::move(info));

    // Ensure we have a dispatcher for this thread.
    if (!g_failedImageBrushesForThread.dispatcher) {
        try {
            g_failedImageBrushesForThread.dispatcher =
                winrt::Windows::System::DispatcherQueue::GetForCurrentThread();
            if (g_failedImageBrushesForThread.dispatcher) {
                // Register this thread's dispatcher globally.
                std::lock_guard<std::mutex> lock(
                    g_failedImageBrushesRegistryMutex);
                g_failedImageBrushesRegistry.push_back(
                    winrt::make_weak(g_failedImageBrushesForThread.dispatcher));
                Wh_Log(L"Registered UI thread dispatcher for network retry");
            }
        } catch (winrt::hresult_error const& ex) {
            Wh_Log(L"Error getting dispatcher for current thread %08X: %s",
                   ex.code(), ex.message().c_str());
        }
    }

    // Register global network status changed handler if not already registered.
    // This is a one-time global registration.
    [[maybe_unused]] static bool networkHandlerRegistered = []() {
        try {
            g_networkStatusChangedToken =
                winrt::Windows::Networking::Connectivity::NetworkInformation::
                    NetworkStatusChanged(OnNetworkStatusChanged);
            Wh_Log(L"Registered global network status change handler");
        } catch (winrt::hresult_error const& ex) {
            Wh_Log(L"Error registering network status handler %08X: %s",
                   ex.code(), ex.message().c_str());
        }
        return true;
    }();
}

void SetOrClearValue(DependencyObject elementDo,
                     DependencyProperty property,
                     const PropertyOverrideValue& overrideValue,
                     bool initialApply = false) {
    winrt::Windows::Foundation::IInspectable value;
    if (auto* inspectable =
            std::get_if<winrt::Windows::Foundation::IInspectable>(
                &overrideValue)) {
        value = *inspectable;
    } else if (auto* blurBrushParams =
                   std::get_if<XamlBlurBrushParams>(&overrideValue)) {
        if (auto uiElement = elementDo.try_as<UIElement>()) {
            value = winrt::make<XamlBlurBrush>(
                uiElement, blurBrushParams->blurAmount, blurBrushParams->tint,
                blurBrushParams->tintOpacity,
                winrt::hstring(blurBrushParams->tintThemeResourceKey),
                blurBrushParams->tintLuminosityOpacity,
                blurBrushParams->tintSaturation, blurBrushParams->noiseOpacity,
                blurBrushParams->noiseDensity, blurBrushParams->fallbackColor,
                winrt::hstring(blurBrushParams->fallbackThemeResourceKey));
        } else {
            Wh_Log(L"Can't get UIElement for blur brush");
            return;
        }
    } else {
        Wh_Log(L"Unsupported override value");
        return;
    }

    // If customized before
    // `winrt::Taskbar::implementation::TaskbarBackground::OnApplyTemplate` is
    // executed, it can lead to a crash, or the customization may be overridden.
    // See:
    // https://github.com/ramensoftware/windows-11-taskbar-styling-guide/issues/4
    if (winrt::get_class_name(elementDo) ==
            L"Windows.UI.Xaml.Shapes.Rectangle" &&
        elementDo.as<FrameworkElement>().Name() == L"BackgroundFill" &&
        property == Shapes::Shape::FillProperty()) {
        auto it = std::find_if(g_delayedBackgroundFillSet.begin(),
                               g_delayedBackgroundFillSet.end(),
                               [&elementDo](const auto& it) {
                                   if (auto elementDoIter = it.first.get()) {
                                       return elementDoIter == elementDo;
                                   }
                                   return false;
                               });

        if (value != DependencyProperty::UnsetValue() && initialApply &&
            it == g_delayedBackgroundFillSet.end()) {
            Wh_Log(L"Delaying SetValue for BackgroundFill");
            auto asyncOp = elementDo.Dispatcher().TryRunAsync(
                winrt::Windows::UI::Core::CoreDispatcherPriority::High,
                [elementDo = std::move(elementDo),
                 property = std::move(property), value = std::move(value)]() {
                    Wh_Log(L"Running delayed SetValue for BackgroundFill");
                    g_elementPropertyModifying = true;
                    try {
                        elementDo.SetValue(property, value);
                    } catch (winrt::hresult_error const& ex) {
                        Wh_Log(L"Error %08X: %s", ex.code(),
                               ex.message().c_str());
                    }
                    g_elementPropertyModifying = false;
                    std::erase_if(g_delayedBackgroundFillSet,
                                  [&elementDo](const auto& it) {
                                      if (auto elementDoIter = it.first.get()) {
                                          return elementDoIter == elementDo;
                                      }
                                      return false;
                                  });
                });
            g_delayedBackgroundFillSet.emplace_back(elementDo,
                                                    std::move(asyncOp));
            return;
        } else if (it != g_delayedBackgroundFillSet.end()) {
            Wh_Log(L"Canceling delayed SetValue for BackgroundFill");
            it->second.Cancel();
            g_delayedBackgroundFillSet.erase(it);
        }
    }

    if (value == DependencyProperty::UnsetValue()) {
        Wh_Log(L"Clearing property value");
        try {
            elementDo.ClearValue(property);
        } catch (winrt::hresult_error const& ex) {
            Wh_Log(L"Error %08X: %s", ex.code(), ex.message().c_str());
        }
        return;
    }

    Wh_Log(L"Setting property value %s",
           value ? winrt::get_class_name(value).c_str() : L"(null)");

    // Track ImageBrush with remote ImageSource for retry on network
    // reconnection. This handles cases where an ImageBrush is set as a property
    // value (e.g., Background).
    if (auto imageBrush = value.try_as<Media::ImageBrush>()) {
        auto imageSource = imageBrush.ImageSource();
        if (auto bitmapImage =
                imageSource.try_as<Media::Imaging::BitmapImage>()) {
            auto uriSource = bitmapImage.UriSource();
            if (uriSource) {
                winrt::hstring uriString = uriSource.ToString();
                if (uriString.starts_with(L"https://") ||
                    uriString.starts_with(L"http://")) {
                    Wh_Log(L"Tracking ImageBrush with remote source: %s",
                           uriString.c_str());
                    SetupImageBrushTracking(imageBrush, uriString);
                }
            }
        }
    }
    // Also handle direct ImageSource property being set on an ImageBrush.
    else if (auto imageBrush = elementDo.try_as<Media::ImageBrush>()) {
        if (property == Media::ImageBrush::ImageSourceProperty()) {
            // Check if the value is a BitmapImage with an http(s):// URI.
            if (auto bitmapImage =
                    value.try_as<Media::Imaging::BitmapImage>()) {
                auto uriSource = bitmapImage.UriSource();
                if (uriSource) {
                    winrt::hstring uriString = uriSource.ToString();
                    if (uriString.starts_with(L"https://") ||
                        uriString.starts_with(L"http://")) {
                        Wh_Log(
                            L"Tracking ImageBrush ImageSource property with "
                            L"remote source: %s",
                            uriString.c_str());
                        SetupImageBrushTracking(imageBrush, uriString);
                    }
                }
            }
        }
    }

    // This might fail. See `ReadLocalValueWithWorkaround` for an example (which
    // we now handle but there might be other cases).
    try {
        // `setter.Value()` returns font weight as an int. Using it with
        // `SetValue` results in the following error: 0x80004002 (No such
        // interface supported). Box it as `Windows.UI.Text.FontWeight` as a
        // workaround.
        if (property == Controls::TextBlock::FontWeightProperty() ||
            property == Controls::Control::FontWeightProperty()) {
            auto valueInt = value.try_as<int>();
            if (valueInt && *valueInt >= std::numeric_limits<uint16_t>::min() &&
                *valueInt <= std::numeric_limits<uint16_t>::max()) {
                value = winrt::box_value(winrt::Windows::UI::Text::FontWeight{
                    static_cast<uint16_t>(*valueInt)});
            }
        }

        elementDo.SetValue(property, value);
    } catch (winrt::hresult_error const& ex) {
        Wh_Log(L"Error %08X: %s", ex.code(), ex.message().c_str());
    }
}

// https://stackoverflow.com/a/5665377
std::wstring EscapeXmlAttribute(std::wstring_view data) {
    std::wstring buffer;
    buffer.reserve(data.size());
    for (const auto c : data) {
        switch (c) {
            case '&':
                buffer.append(L"&amp;");
                break;
            case '\"':
                buffer.append(L"&quot;");
                break;
            // case '\'':
            //     buffer.append(L"&apos;");
            //     break;
            case '<':
                buffer.append(L"&lt;");
                break;
            case '>':
                buffer.append(L"&gt;");
                break;
            default:
                buffer.push_back(c);
                break;
        }
    }

    return buffer;
}

// https://stackoverflow.com/a/54364173
std::wstring_view TrimStringView(std::wstring_view s) {
    s.remove_prefix(std::min(s.find_first_not_of(L" \t\r\v\n"), s.size()));
    s.remove_suffix(
        std::min(s.size() - s.find_last_not_of(L" \t\r\v\n") - 1, s.size()));
    return s;
}

// https://stackoverflow.com/a/46931770
std::vector<std::wstring_view> SplitStringView(std::wstring_view s,
                                               std::wstring_view delimiter) {
    size_t pos_start = 0, pos_end, delim_len = delimiter.length();
    std::wstring_view token;
    std::vector<std::wstring_view> res;

    while ((pos_end = s.find(delimiter, pos_start)) !=
           std::wstring_view::npos) {
        token = s.substr(pos_start, pos_end - pos_start);
        pos_start = pos_end + delim_len;
        res.push_back(token);
    }

    res.push_back(s.substr(pos_start));
    return res;
}

std::optional<PropertyOverrideValue> ParseNonXamlPropertyOverrideValue(
    std::wstring_view stringValue) {
    // Example:
    // <WindhawkBlur BlurAmount="10" TintColor="#FFFF0000"/>

    auto substr = TrimStringView(stringValue);

    constexpr auto kWindhawkBlurPrefix = L"<WindhawkBlur "sv;
    if (!substr.starts_with(kWindhawkBlurPrefix)) {
        return std::nullopt;
    }
    Wh_Log(L"%.*s", static_cast<int>(substr.length()), substr.data());
    substr = substr.substr(std::size(kWindhawkBlurPrefix));

    constexpr auto kWindhawkBlurSuffix = L"/>"sv;
    if (!substr.ends_with(kWindhawkBlurSuffix)) {
        throw std::runtime_error("WindhawkBlur: Bad suffix");
    }
    substr = substr.substr(0, substr.size() - std::size(kWindhawkBlurSuffix));

    bool pendingTintColorThemeResource = false;
    bool pendingFallbackColorThemeResource = false;
    std::wstring tintThemeResourceKey;
    std::wstring fallbackThemeResourceKey;
    winrt::Windows::UI::Color tint{};
    std::optional<winrt::Windows::UI::Color> fallbackColor;
    float tintOpacity = std::numeric_limits<float>::quiet_NaN();
    float tintLuminosityOpacity = std::numeric_limits<float>::quiet_NaN();
    float tintSaturation = std::numeric_limits<float>::quiet_NaN();
    float noiseOpacity = std::numeric_limits<float>::quiet_NaN();
    float noiseDensity = std::numeric_limits<float>::quiet_NaN();
    float blurAmount = 0;

    constexpr auto kTintColorThemeResourcePrefix =
        L"TintColor=\"{ThemeResource"sv;
    constexpr auto kTintColorThemeResourceSuffix = L"}\""sv;
    constexpr auto kTintColorPrefix = L"TintColor=\"#"sv;
    constexpr auto kTintOpacityPrefix = L"TintOpacity=\""sv;
    constexpr auto kTintLuminosityOpacityPrefix = L"TintLuminosityOpacity=\""sv;
    constexpr auto kTintSaturationPrefix = L"TintSaturation=\""sv;
    constexpr auto kNoiseOpacityPrefix = L"NoiseOpacity=\""sv;
    constexpr auto kNoiseDensityPrefix = L"NoiseDensity=\""sv;
    constexpr auto kBlurAmountPrefix = L"BlurAmount=\""sv;
    constexpr auto kFallbackColorThemeResourcePrefix =
        L"FallbackColor=\"{ThemeResource"sv;
    constexpr auto kFallbackColorThemeResourceSuffix = L"}\""sv;
    constexpr auto kFallbackColorPrefix = L"FallbackColor=\"#"sv;
    for (const auto prop : SplitStringView(substr, L" ")) {
        const auto propSubstr = TrimStringView(prop);
        if (propSubstr.empty()) {
            continue;
        }

        Wh_Log(L"  %.*s", static_cast<int>(propSubstr.length()),
               propSubstr.data());

        if (pendingTintColorThemeResource) {
            if (!propSubstr.ends_with(kTintColorThemeResourceSuffix)) {
                throw std::runtime_error(
                    "WindhawkBlur: Invalid TintColor theme resource syntax");
            }

            pendingTintColorThemeResource = false;

            tintThemeResourceKey = propSubstr.substr(
                0,
                propSubstr.size() - std::size(kTintColorThemeResourceSuffix));

            continue;
        }

        if (pendingFallbackColorThemeResource) {
            if (!propSubstr.ends_with(kFallbackColorThemeResourceSuffix)) {
                throw std::runtime_error(
                    "WindhawkBlur: Invalid FallbackColor theme resource "
                    "syntax");
            }

            pendingFallbackColorThemeResource = false;

            fallbackThemeResourceKey = propSubstr.substr(
                0, propSubstr.size() -
                       std::size(kFallbackColorThemeResourceSuffix));

            continue;
        }

        if (propSubstr == kTintColorThemeResourcePrefix) {
            pendingTintColorThemeResource = true;
            continue;
        }

        if (propSubstr == kFallbackColorThemeResourcePrefix) {
            pendingFallbackColorThemeResource = true;
            continue;
        }

        if (propSubstr.starts_with(kTintColorPrefix) &&
            propSubstr.back() == L'\"') {
            auto valStr = propSubstr.substr(
                std::size(kTintColorPrefix),
                propSubstr.size() - std::size(kTintColorPrefix) - 1);

            bool hasAlpha;
            switch (valStr.size()) {
                case 6:
                    hasAlpha = false;
                    break;
                case 8:
                    hasAlpha = true;
                    break;
                default:
                    throw std::runtime_error(
                        "WindhawkBlur: Unsupported TintColor value");
            }

            auto valNum = std::stoul(std::wstring(valStr), nullptr, 16);
            uint8_t a = hasAlpha ? HIBYTE(HIWORD(valNum)) : 255;
            uint8_t r = LOBYTE(HIWORD(valNum));
            uint8_t g = HIBYTE(LOWORD(valNum));
            uint8_t b = LOBYTE(LOWORD(valNum));
            tint = {a, r, g, b};
            continue;
        }

        if (propSubstr.starts_with(kFallbackColorPrefix) &&
            propSubstr.back() == L'\"') {
            auto valStr = propSubstr.substr(
                std::size(kFallbackColorPrefix),
                propSubstr.size() - std::size(kFallbackColorPrefix) - 1);

            bool hasAlpha;
            switch (valStr.size()) {
                case 6:
                    hasAlpha = false;
                    break;
                case 8:
                    hasAlpha = true;
                    break;
                default:
                    throw std::runtime_error(
                        "WindhawkBlur: Unsupported FallbackColor value");
            }

            auto valNum = std::stoul(std::wstring(valStr), nullptr, 16);
            uint8_t a = hasAlpha ? HIBYTE(HIWORD(valNum)) : 255;
            uint8_t r = LOBYTE(HIWORD(valNum));
            uint8_t g = HIBYTE(LOWORD(valNum));
            uint8_t b = LOBYTE(LOWORD(valNum));
            fallbackColor = winrt::Windows::UI::Color{a, r, g, b};
            continue;
        }

        if (propSubstr.starts_with(kTintOpacityPrefix) &&
            propSubstr.back() == L'\"') {
            auto valStr = propSubstr.substr(
                std::size(kTintOpacityPrefix),
                propSubstr.size() - std::size(kTintOpacityPrefix) - 1);
            tintOpacity = std::stof(std::wstring(valStr));
            continue;
        }

        if (propSubstr.starts_with(kTintLuminosityOpacityPrefix) &&
            propSubstr.back() == L'\"') {
            auto valStr = propSubstr.substr(
                std::size(kTintLuminosityOpacityPrefix),
                propSubstr.size() - std::size(kTintLuminosityOpacityPrefix) -
                    1);
            tintLuminosityOpacity = std::stof(std::wstring(valStr));
            continue;
        }

        if (propSubstr.starts_with(kTintSaturationPrefix) &&
            propSubstr.back() == L'\"') {
            auto valStr = propSubstr.substr(
                std::size(kTintSaturationPrefix),
                propSubstr.size() - std::size(kTintSaturationPrefix) - 1);
            tintSaturation = std::stof(std::wstring(valStr));
            continue;
        }

        if (propSubstr.starts_with(kNoiseOpacityPrefix) &&
            propSubstr.back() == L'\"') {
            auto valStr = propSubstr.substr(
                std::size(kNoiseOpacityPrefix),
                propSubstr.size() - std::size(kNoiseOpacityPrefix) - 1);
            noiseOpacity = std::stof(std::wstring(valStr));
            continue;
        }

        if (propSubstr.starts_with(kNoiseDensityPrefix) &&
            propSubstr.back() == L'\"') {
            auto valStr = propSubstr.substr(
                std::size(kNoiseDensityPrefix),
                propSubstr.size() - std::size(kNoiseDensityPrefix) - 1);
            noiseDensity = std::stof(std::wstring(valStr));
            continue;
        }

        if (propSubstr.starts_with(kBlurAmountPrefix) &&
            propSubstr.back() == L'\"') {
            auto valStr = propSubstr.substr(
                std::size(kBlurAmountPrefix),
                propSubstr.size() - std::size(kBlurAmountPrefix) - 1);
            blurAmount = std::stof(std::wstring(valStr));
            continue;
        }

        throw std::runtime_error("WindhawkBlur: Bad property");
    }

    if (pendingTintColorThemeResource) {
        throw std::runtime_error(
            "WindhawkBlur: Unterminated TintColor theme resource");
    }

    if (pendingFallbackColorThemeResource) {
        throw std::runtime_error(
            "WindhawkBlur: Unterminated FallbackColor theme resource");
    }

    if (!std::isnan(tintOpacity)) {
        if (tintOpacity < 0.0f) {
            tintOpacity = 0.0f;
        } else if (tintOpacity > 1.0f) {
            tintOpacity = 1.0f;
        }

        tint.A = static_cast<uint8_t>(tintOpacity * 255.0f);
    }

    return XamlBlurBrushParams{
        .blurAmount = blurAmount,
        .tint = tint,
        .tintOpacity =
            !std::isnan(tintOpacity) ? std::optional(tint.A) : std::nullopt,
        .tintThemeResourceKey = std::move(tintThemeResourceKey),
        .tintLuminosityOpacity = !std::isnan(tintLuminosityOpacity)
                                     ? std::optional(tintLuminosityOpacity)
                                     : std::nullopt,
        .tintSaturation = !std::isnan(tintSaturation)
                              ? std::optional(tintSaturation)
                              : std::nullopt,
        .noiseOpacity = !std::isnan(noiseOpacity) ? std::optional(noiseOpacity)
                                                  : std::nullopt,
        .noiseDensity = !std::isnan(noiseDensity) ? std::optional(noiseDensity)
                                                  : std::nullopt,
        .fallbackColor = fallbackColor,
        .fallbackThemeResourceKey = std::move(fallbackThemeResourceKey),
    };
}

Style GetStyleFromXamlSetters(const std::wstring_view type,
                              const std::wstring_view xamlStyleSetters) {
    std::wstring xaml =
        LR"(<ResourceDictionary
    xmlns="http://schemas.microsoft.com/winfx/2006/xaml/presentation"
    xmlns:x="http://schemas.microsoft.com/winfx/2006/xaml"
    xmlns:d="http://schemas.microsoft.com/expression/blend/2008"
    xmlns:mc="http://schemas.openxmlformats.org/markup-compatibility/2006"
    xmlns:muxc="using:Microsoft.UI.Xaml.Controls")";

    if (auto pos = type.rfind('.'); pos != type.npos) {
        auto typeNamespace = std::wstring_view(type).substr(0, pos);
        auto typeName = std::wstring_view(type).substr(pos + 1);

        xaml += L"\n    xmlns:windhawkstyler=\"using:";
        xaml += EscapeXmlAttribute(typeNamespace);
        xaml +=
            L"\">\n"
            L"    <Style TargetType=\"windhawkstyler:";
        xaml += EscapeXmlAttribute(typeName);
        xaml += L"\">\n";
    } else {
        xaml +=
            L">\n"
            L"    <Style TargetType=\"";
        xaml += EscapeXmlAttribute(type);
        xaml += L"\">\n";
    }

    xaml += xamlStyleSetters;

    xaml +=
        L"    </Style>\n"
        L"</ResourceDictionary>";

    Wh_Log(L"======================================== XAML:");
    std::wstringstream ss(xaml);
    std::wstring line;
    while (std::getline(ss, line, L'\n')) {
        Wh_Log(L"%s", line.c_str());
    }
    Wh_Log(L"========================================");

    auto resourceDictionary =
        Markup::XamlReader::Load(xaml).as<ResourceDictionary>();

    auto [styleKey, styleInspectable] = resourceDictionary.First().Current();
    return styleInspectable.as<Style>();
}

Style GetStyleFromXamlSettersWithFallbackType(
    const std::wstring_view type,
    const std::wstring_view fallbackType,
    const std::wstring_view xamlStyleSetters) {
    try {
        return GetStyleFromXamlSetters(type, xamlStyleSetters);
    } catch (winrt::hresult_error const& ex) {
        constexpr HRESULT kStowedException = 0x802B000A;
        if (ex.code() != kStowedException || fallbackType.empty() ||
            fallbackType == type) {
            throw;
        }

        // For some types such as JumpViewUI.JumpListListViewItem, the following
        // error is returned:
        //
        // Error 802B000A: Failed to create a 'System.Type' from the text
        // 'windhawkstyler:JumpListListViewItem'. [Line: 8 Position: 12]
        //
        // Retry with a fallback type, which will allow to at least use the
        // basic properties.
        Wh_Log(L"Retrying with fallback type type due to error %08X: %s",
               ex.code(), ex.message().c_str());
        return GetStyleFromXamlSetters(fallbackType, xamlStyleSetters);
    }
}

const ResolvedRules& GetResolvedPropertyOverrides(
    const std::wstring_view type,
    const std::wstring_view fallbackType,
    PropertyOverridesMaybeUnresolved* propertyOverridesMaybeUnresolved) {
    if (const auto* resolved =
            std::get_if<ResolvedRules>(propertyOverridesMaybeUnresolved)) {
        return *resolved;
    }

    ResolvedRules resolved;

    try {
        const auto& unresolved =
            std::get<UnresolvedRules>(*propertyOverridesMaybeUnresolved);
        const auto& valueRules = unresolved.valueRules;
        const auto& captureRules = unresolved.captureRules;

        if (!valueRules.empty() || !captureRules.empty()) {
            // Build a single XAML <Style> with one <Setter> per rule. Setters
            // for value rules come first, followed by one per capture rule.
            // Dynamic / capture rules emit a placeholder `{x:Null}` value -- we
            // only need the resolved DependencyProperty from those setters; the
            // value is computed elsewhere (per apply for dynamic, never for
            // captures).
            std::wstring xaml;

            std::vector<std::optional<PropertyOverrideValue>>
                propertyOverrideValues;
            propertyOverrideValues.reserve(valueRules.size());

            for (const auto& rule : valueRules) {
                const bool isDynamic = rule.isDynamic();

                propertyOverrideValues.push_back(
                    !isDynamic && rule.isXamlValue
                        ? ParseNonXamlPropertyOverrideValue(rule.value)
                        : std::nullopt);

                xaml += L"        <Setter Property=\"";
                xaml += EscapeXmlAttribute(rule.propertyName);
                xaml += L"\"";
                if (isDynamic || propertyOverrideValues.back() ||
                    (rule.isXamlValue && rule.value.empty())) {
                    xaml += L" Value=\"{x:Null}\" />\n";
                } else if (!rule.isXamlValue) {
                    xaml += L" Value=\"";
                    xaml += EscapeXmlAttribute(rule.value);
                    xaml += L"\" />\n";
                } else {
                    xaml +=
                        L">\n"
                        L"            <Setter.Value>\n";
                    xaml += rule.value;
                    xaml +=
                        L"\n"
                        L"            </Setter.Value>\n"
                        L"        </Setter>\n";
                }
            }

            for (const auto& rule : captureRules) {
                xaml += L"        <Setter Property=\"";
                xaml += EscapeXmlAttribute(rule.propertyName);
                xaml += L"\" Value=\"{x:Null}\" />\n";
            }

            auto style = GetStyleFromXamlSettersWithFallbackType(
                type, fallbackType, xaml);

            uint32_t setterIndex = 0;
            for (size_t i = 0; i < valueRules.size(); i++, setterIndex++) {
                const auto& rule = valueRules[i];
                const auto setter =
                    style.Setters().GetAt(setterIndex).as<Setter>();
                auto property = setter.Property();
                if (rule.isDynamic()) {
                    resolved.propertyOverrides[property][rule.visualState] =
                        DynamicStyleTemplate{rule.propertyName, rule.value,
                                             rule.isXamlValue};
                } else {
                    resolved.propertyOverrides[property][rule.visualState] =
                        propertyOverrideValues[i].value_or(
                            rule.isXamlValue && rule.value.empty()
                                ? DependencyProperty::UnsetValue()
                                : setter.Value());
                }
            }

            for (const auto& rule : captureRules) {
                const auto setter =
                    style.Setters().GetAt(setterIndex++).as<Setter>();
                resolved.captures.push_back({setter.Property(), rule.varName});
            }
        }

        Wh_Log(L"%.*s: %zu override styles, %zu captures",
               static_cast<int>(type.length()), type.data(),
               resolved.propertyOverrides.size(), resolved.captures.size());
    } catch (winrt::hresult_error const& ex) {
        Wh_Log(L"Error %08X: %s", ex.code(), ex.message().c_str());
    } catch (std::exception const& ex) {
        Wh_Log(L"Error: %S", ex.what());
    }

    *propertyOverridesMaybeUnresolved = std::move(resolved);
    return std::get<ResolvedRules>(*propertyOverridesMaybeUnresolved);
}

// Resolve a single style rule's expanded textual value into a usable
// PropertyOverrideValue. Built for re-resolving dynamic `{{...}}` styles on
// every variable change; falls back to the same XAML-Setter parse trick used by
// the bulk resolver above. propertyName is the property whose XAML name should
// appear on the synthetic Setter (already known at apply time).
std::optional<PropertyOverrideValue> ResolveExpandedSinglePropertyValue(
    std::wstring_view type,
    std::wstring_view fallbackType,
    std::wstring_view propertyName,
    std::wstring_view expandedValue,
    bool isXamlValue) {
    if (isXamlValue) {
        if (auto blur = ParseNonXamlPropertyOverrideValue(expandedValue)) {
            return *blur;
        }

        if (TrimStringView(expandedValue).empty()) {
            return PropertyOverrideValue{DependencyProperty::UnsetValue()};
        }
    }

    std::wstring xaml = L"        <Setter Property=\"";
    xaml += EscapeXmlAttribute(propertyName);
    xaml += L"\"";
    if (!isXamlValue) {
        xaml += L" Value=\"";
        xaml += EscapeXmlAttribute(expandedValue);
        xaml += L"\" />\n";
    } else {
        xaml +=
            L">\n"
            L"            <Setter.Value>\n";
        xaml += expandedValue;
        xaml +=
            L"\n"
            L"            </Setter.Value>\n"
            L"        </Setter>\n";
    }

    try {
        auto style =
            GetStyleFromXamlSettersWithFallbackType(type, fallbackType, xaml);
        const auto setter = style.Setters().GetAt(0).as<Setter>();
        return PropertyOverrideValue{setter.Value()};
    } catch (winrt::hresult_error const& ex) {
        Wh_Log(L"Error %08X: %s", ex.code(), ex.message().c_str());
    } catch (std::exception const& ex) {
        Wh_Log(L"Error: %S", ex.what());
    }

    return std::nullopt;
}

const PropertyValues& GetResolvedPropertyValues(
    const std::wstring_view type,
    const std::wstring_view fallbackType,
    PropertyValuesMaybeUnresolved* propertyValuesMaybeUnresolved) {
    if (const auto* resolved =
            std::get_if<PropertyValues>(propertyValuesMaybeUnresolved)) {
        return *resolved;
    }

    PropertyValues propertyValues;

    try {
        const auto& propertyValuesStr =
            std::get<PropertyValuesUnresolved>(*propertyValuesMaybeUnresolved);
        if (!propertyValuesStr.empty()) {
            std::wstring xaml;

            for (const auto& [property, value] : propertyValuesStr) {
                xaml += L"        <Setter Property=\"";
                xaml += EscapeXmlAttribute(property);
                xaml += L"\" Value=\"";
                xaml += EscapeXmlAttribute(value);
                xaml += L"\" />\n";
            }

            auto style = GetStyleFromXamlSettersWithFallbackType(
                type, fallbackType, xaml);

            for (size_t i = 0; i < propertyValuesStr.size(); i++) {
                const auto setter = style.Setters().GetAt(i).as<Setter>();
                propertyValues.push_back({
                    setter.Property(),
                    setter.Value(),
                });
            }
        }

        Wh_Log(L"%.*s: %zu matcher styles", static_cast<int>(type.length()),
               type.data(), propertyValues.size());
    } catch (winrt::hresult_error const& ex) {
        Wh_Log(L"Error %08X: %s", ex.code(), ex.message().c_str());
    } catch (std::exception const& ex) {
        Wh_Log(L"Error: %S", ex.what());
    }

    *propertyValuesMaybeUnresolved = std::move(propertyValues);
    return std::get<PropertyValues>(*propertyValuesMaybeUnresolved);
}

// https://stackoverflow.com/a/12835139
VisualStateGroup GetVisualStateGroup(FrameworkElement element,
                                     std::wstring_view visualStateGroupName) {
    // The TaskListButtonPanel child element of the search box (with "Icon and
    // label" configuration) returns a list of size 1, but accessing the first
    // item leads to a null dereference crash. Skip this element.
    if (winrt::get_class_name(element) == L"Taskbar.TaskListButtonPanel") {
        auto parent = Media::VisualTreeHelper::GetParent(element)
                          .try_as<FrameworkElement>();
        if (parent && winrt::get_class_name(parent) ==
                          L"Taskbar.SearchBoxLaunchListButton") {
            return nullptr;
        }
    }

    // Same as above for an updated element layout (around Jun 2025).
    if (winrt::get_class_name(element) ==
        L"SearchUx.SearchUI.SearchButtonRootGrid") {
        auto parent = Media::VisualTreeHelper::GetParent(element)
                          .try_as<FrameworkElement>();
        if (parent && winrt::get_class_name(parent) ==
                          L"SearchUx.SearchUI.SearchPillButton") {
            return nullptr;
        }
    }

    auto list = VisualStateManager::GetVisualStateGroups(element);

    for (const auto& v : list) {
        if (v.Name() == visualStateGroupName) {
            return v;
        }
    }

    return nullptr;
}

// Locale-independent double formatter. Uses `std::to_chars` shortest round-trip
// representation so XAML always sees `.` as the decimal separator.
std::wstring FormatDoubleInvariant(double d) {
    char buf[64];
    auto [end, ec] = std::to_chars(buf, buf + std::size(buf), d);
    if (ec != std::errc{}) {
        return L"0";
    }
    return std::wstring(buf, end);
}

// Locale-independent double parser. Accepts an optional leading sign followed
// by a decimal fraction or exponent. Returns std::nullopt on partial / bad
// input.
std::optional<double> ParseDoubleInvariant(std::wstring_view sv) {
    std::string narrow;
    narrow.reserve(sv.size());
    for (auto c : sv) {
        if (c > 127) {
            return std::nullopt;
        }
        narrow.push_back(static_cast<char>(c));
    }
    double result = 0;
    auto* first = narrow.data();
    auto* last = first + narrow.size();
    auto [ptr, ec] = std::from_chars(first, last, result);
    if (ec != std::errc{} || ptr != last) {
        return std::nullopt;
    }
    return result;
}

using UnboxedPropertyValue = std::variant<std::wstring,
                                          bool,
                                          char16_t,
                                          uint8_t,
                                          int16_t,
                                          uint16_t,
                                          int32_t,
                                          uint32_t,
                                          int64_t,
                                          uint64_t,
                                          float,
                                          double>;

// Unwraps a boxed primitive into a typed primitive variant. Dispatches on
// IPropertyValue::Type(). Returns std::nullopt for non-primitive (opaque)
// values such as brushes or thicknesses.
std::optional<UnboxedPropertyValue> TryUnboxPropertyValue(
    winrt::Windows::Foundation::IInspectable const& value) {
    using winrt::Windows::Foundation::IPropertyValue;
    using winrt::Windows::Foundation::PropertyType;

    auto pv = value.try_as<IPropertyValue>();
    if (!pv) {
        return std::nullopt;
    }

    switch (pv.Type()) {
        case PropertyType::String:
            return UnboxedPropertyValue{std::wstring(pv.GetString())};
        case PropertyType::Boolean:
            return UnboxedPropertyValue{pv.GetBoolean()};
        case PropertyType::Char16:
            return UnboxedPropertyValue{pv.GetChar16()};
        case PropertyType::Double:
            return UnboxedPropertyValue{pv.GetDouble()};
        case PropertyType::Single:
            return UnboxedPropertyValue{pv.GetSingle()};
        case PropertyType::UInt8:
            return UnboxedPropertyValue{pv.GetUInt8()};
        case PropertyType::Int16:
            return UnboxedPropertyValue{pv.GetInt16()};
        case PropertyType::UInt16:
            return UnboxedPropertyValue{pv.GetUInt16()};
        case PropertyType::Int32:
            return UnboxedPropertyValue{pv.GetInt32()};
        case PropertyType::UInt32:
            return UnboxedPropertyValue{pv.GetUInt32()};
        case PropertyType::Int64:
            return UnboxedPropertyValue{pv.GetInt64()};
        case PropertyType::UInt64:
            return UnboxedPropertyValue{pv.GetUInt64()};
        case PropertyType::OtherType: {
            // Common for enums.
            if (auto intVal = value.try_as<int32_t>()) {
                return UnboxedPropertyValue{*intVal};
            }
            return std::nullopt;
        }
        default: {
            return std::nullopt;
        }
    }
}

// Invariant-formatted text form, suitable for XAML attribute use or diagnostic
// logs.
std::wstring FormatUnboxedPropertyValue(UnboxedPropertyValue const& v) {
    return std::visit(
        [](auto const& x) -> std::wstring {
            using T = std::decay_t<decltype(x)>;
            if constexpr (std::is_same_v<T, std::wstring>) {
                return x;
            } else if constexpr (std::is_same_v<T, bool>) {
                return x ? L"True" : L"False";
            } else if constexpr (std::is_same_v<T, char16_t>) {
                // Single-character text form so substitution emits the
                // character itself.
                return std::wstring(1, static_cast<wchar_t>(x));
            } else if constexpr (std::is_floating_point_v<T>) {
                return FormatDoubleInvariant(static_cast<double>(x));
            } else {
                return std::to_wstring(x);
            }
        },
        v);
}

// Numeric-as-double form, or std::nullopt if the value isn't numeric (i.e.
// holds a string).
std::optional<double> UnboxedPropertyValueAsNumeric(
    UnboxedPropertyValue const& v) {
    return std::visit(
        [](auto const& x) -> std::optional<double> {
            using T = std::decay_t<decltype(x)>;
            if constexpr (std::is_same_v<T, std::wstring>) {
                return std::nullopt;
            } else {
                return static_cast<double>(x);
            }
        },
        v);
}

bool TestElementMatcher(FrameworkElement element,
                        ElementMatcher& matcher,
                        VisualStateGroup* visualStateGroup,
                        PCWSTR fallbackClassName) {
    if (!matcher.type.empty() &&
        matcher.type != winrt::get_class_name(element) &&
        (!fallbackClassName || matcher.type != fallbackClassName)) {
        return false;
    }

    if (!matcher.name.empty() && matcher.name != element.Name()) {
        return false;
    }

    if (matcher.oneBasedIndex) {
        auto parent = Media::VisualTreeHelper::GetParent(element);
        if (!parent) {
            return false;
        }

        int index = matcher.oneBasedIndex - 1;
        if (index < 0 ||
            index >= Media::VisualTreeHelper::GetChildrenCount(parent) ||
            Media::VisualTreeHelper::GetChild(parent, index) != element) {
            return false;
        }
    }

    auto elementDo = element.as<DependencyObject>();

    for (const auto& propertyValue : GetResolvedPropertyValues(
             matcher.type,
             fallbackClassName ? fallbackClassName
                               : winrt::name_of<FrameworkElement>(),
             &matcher.propertyValues)) {
        const auto value =
            ReadLocalValueWithWorkaround(elementDo, propertyValue.first);
        if (!value) {
            Wh_Log(L"Null property value");
            return false;
        } else if (value == DependencyProperty::UnsetValue()) {
            return false;
        }

        auto expectedUnboxed = TryUnboxPropertyValue(propertyValue.second);
        auto valueUnboxed = TryUnboxPropertyValue(value);
        if (!expectedUnboxed || !valueUnboxed) {
            Wh_Log(L"Unsupported property class: %s",
                   winrt::get_class_name(value).c_str());
            return false;
        }

        if (*expectedUnboxed != *valueUnboxed) {
            return false;
        }
    }

    if (matcher.visualStateGroupName && visualStateGroup) {
        *visualStateGroup =
            GetVisualStateGroup(element, *matcher.visualStateGroupName);
    }

    return true;
}

// Aggregated resolved rules for an element. Value-rules are still bucketed by
// visual-state-group (each target's rules live under that target's @VSGName);
// captures are intentionally NOT per-VSG -- they are wired up once at element
// level (see SetUpCapturesForElement).
struct ElementResolvedRules {
    std::unordered_map<VisualStateGroup, PropertyOverrides> overridesPerVSG;
    std::vector<CaptureSpec> captures;
};

ElementResolvedRules FindElementPropertyOverrides(FrameworkElement element,
                                                  PCWSTR fallbackClassName) {
    ElementResolvedRules result;
    std::unordered_set<DependencyProperty> propertiesAdded;
    std::unordered_set<std::wstring> capturesAdded;

    for (auto it = g_elementsCustomizationRules.rbegin();
         it != g_elementsCustomizationRules.rend(); ++it) {
        auto& override = *it;

        VisualStateGroup visualStateGroup = nullptr;

        if (!TestElementMatcher(element, override.elementMatcher,
                                &visualStateGroup, fallbackClassName)) {
            continue;
        }

        // Using iter.Parent() was sometimes returning null, so use
        // VisualTreeHelper::GetParent below instead.
        //
        // Recursive lambda so that '*' can backtrack: when a candidate match
        // for the wildcard's next matcher leads to a failure further up the
        // chain, retry with a farther ancestor.
        auto& parentMatchers = override.parentElementMatchers;
        auto matchParents = [&](auto& self, FrameworkElement iter,
                                size_t mi) -> bool {
            if (mi >= parentMatchers.size()) {
                return true;
            }

            auto& matcher = parentMatchers[mi];

            if (matcher.kind == ElementMatcher::Kind::Root) {
                if (Media::VisualTreeHelper::GetParent(iter)) {
                    return false;
                }

                return self(self, iter, mi + 1);
            }

            if (matcher.kind == ElementMatcher::Kind::Wildcard) {
                // '*' is always followed by an Element matcher (validated at
                // parse time). Walk up parents and try recursing for each
                // ancestor that matches the next matcher.
                auto& nextMatcher = parentMatchers[mi + 1];
                auto cur = iter;
                while (true) {
                    auto parent = Media::VisualTreeHelper::GetParent(cur)
                                      .try_as<FrameworkElement>();
                    if (!parent) {
                        return false;
                    }

                    cur = parent;
                    if (TestElementMatcher(cur, nextMatcher, &visualStateGroup,
                                           nullptr) &&
                        self(self, cur, mi + 2)) {
                        return true;
                    }
                }
            }

            auto parent = Media::VisualTreeHelper::GetParent(iter)
                              .try_as<FrameworkElement>();
            if (!parent) {
                return false;
            }

            if (!TestElementMatcher(parent, matcher, &visualStateGroup,
                                    nullptr)) {
                return false;
            }

            return self(self, parent, mi + 1);
        };

        if (!matchParents(matchParents, element, 0)) {
            continue;
        }

        const auto& resolvedRules = GetResolvedPropertyOverrides(
            override.elementMatcher.type,
            fallbackClassName ? fallbackClassName
                              : winrt::name_of<FrameworkElement>(),
            &override.propertyOverrides);

        auto& propertyOverridesForVSG =
            result.overridesPerVSG[visualStateGroup];
        for (const auto& [property, valuesPerVisualState] :
             resolvedRules.propertyOverrides) {
            bool propertyInserted = propertiesAdded.insert(property).second;
            if (!propertyInserted) {
                continue;
            }

            auto& propertyOverrides = propertyOverridesForVSG[property];
            for (const auto& [visualState, value] : valuesPerVisualState) {
                propertyOverrides.insert({visualState, value});
            }
        }

        for (const auto& capture : resolvedRules.captures) {
            if (!capturesAdded.insert(capture.varName).second) {
                continue;
            }

            result.captures.push_back(capture);
        }
    }

    std::erase_if(result.overridesPerVSG,
                  [](const auto& item) { return item.second.empty(); });

    return result;
}

bool IsValidStyleVariableIdentifier(std::wstring_view sv) {
    if (sv.empty()) {
        return false;
    }
    auto isStart = [](wchar_t c) {
        return (c >= L'A' && c <= L'Z') || (c >= L'a' && c <= L'z') ||
               c == L'_';
    };
    auto isCont = [&](wchar_t c) {
        return isStart(c) || (c >= L'0' && c <= L'9');
    };
    if (!isStart(sv[0])) {
        return false;
    }
    for (size_t i = 1; i < sv.size(); i++) {
        if (!isCont(sv[i])) {
            return false;
        }
    }
    return true;
}

// Recursive-descent evaluator for `{{ ... }}` expressions. Supports number
// literal, identifier (style variable reference), parenthesized subexpression,
// the binary ops + - * /, unary - / +, and the two-arg functions min(a, b) and
// max(a, b). Standard math precedence.
//
// Variable references pushed into outDeps so the dependent style can be
// re-evaluated when those variables change.
class StyleVariableExpressionEvaluator {
   public:
    StyleVariableExpressionEvaluator(std::wstring_view text,
                                     std::vector<std::wstring>* outDeps,
                                     StyleVariableState* state)
        : m_text(text), m_outDeps(outDeps), m_state(state) {}

    // Returns the numeric result of the expression. Throws std::runtime_error
    // on parse / evaluation failure (including when an identifier resolves to a
    // non-numeric variable, or when the expression produces a non-finite result
    // -- NaN/Inf can't be formatted into XAML attributes meaningfully and would
    // also break the consumer-equality check in
    // SetStyleVariableIfChangedAndPropagate, since NaN != NaN).
    double Evaluate() {
        m_pos = 0;
        SkipWhitespace();
        double v = ParseExpression();
        SkipWhitespace();
        if (m_pos != m_text.size()) {
            throw std::runtime_error(
                "Unexpected trailing characters in style variable expression");
        }
        if (!std::isfinite(v)) {
            throw std::runtime_error(
                "Style variable expression produced a non-finite result");
        }
        return v;
    }

   private:
    void SkipWhitespace() {
        while (m_pos < m_text.size() &&
               (m_text[m_pos] == L' ' || m_text[m_pos] == L'\t' ||
                m_text[m_pos] == L'\r' || m_text[m_pos] == L'\n')) {
            m_pos++;
        }
    }

    bool ConsumeChar(wchar_t c) {
        SkipWhitespace();
        if (m_pos < m_text.size() && m_text[m_pos] == c) {
            m_pos++;
            return true;
        }
        return false;
    }

    double ParseExpression() {
        double v = ParseTerm();
        while (true) {
            SkipWhitespace();
            if (ConsumeChar(L'+')) {
                v += ParseTerm();
            } else if (ConsumeChar(L'-')) {
                v -= ParseTerm();
            } else {
                break;
            }
        }
        return v;
    }

    double ParseTerm() {
        double v = ParseFactor();
        while (true) {
            SkipWhitespace();
            if (ConsumeChar(L'*')) {
                v *= ParseFactor();
            } else if (ConsumeChar(L'/')) {
                double rhs = ParseFactor();
                if (rhs == 0.0) {
                    throw std::runtime_error(
                        "Division by zero in style variable expression");
                }
                v /= rhs;
            } else {
                break;
            }
        }
        return v;
    }

    double ParseFactor() {
        SkipWhitespace();
        if (ConsumeChar(L'+')) {
            return ParseFactor();
        }
        if (ConsumeChar(L'-')) {
            return -ParseFactor();
        }
        return ParsePrimary();
    }

    double ParsePrimary() {
        SkipWhitespace();
        if (m_pos >= m_text.size()) {
            throw std::runtime_error(
                "Unexpected end of style variable expression");
        }

        wchar_t c = m_text[m_pos];
        if (c == L'(') {
            m_pos++;
            double v = ParseExpression();
            SkipWhitespace();
            if (!ConsumeChar(L')')) {
                throw std::runtime_error(
                    "Missing ')' in style variable expression");
            }
            return v;
        }

        if ((c >= L'0' && c <= L'9') || c == L'.') {
            return ParseNumberLiteral();
        }

        if ((c >= L'A' && c <= L'Z') || (c >= L'a' && c <= L'z') || c == L'_') {
            return ParseIdentifierOrCall();
        }

        throw std::runtime_error(
            "Unexpected character in style variable expression");
    }

    double ParseNumberLiteral() {
        size_t start = m_pos;
        bool sawDigit = false;
        bool sawDot = false;
        while (m_pos < m_text.size()) {
            wchar_t c = m_text[m_pos];
            if (c >= L'0' && c <= L'9') {
                sawDigit = true;
                m_pos++;
            } else if (c == L'.' && !sawDot) {
                sawDot = true;
                m_pos++;
            } else {
                break;
            }
        }
        if (m_pos < m_text.size() &&
            (m_text[m_pos] == L'e' || m_text[m_pos] == L'E')) {
            m_pos++;
            if (m_pos < m_text.size() &&
                (m_text[m_pos] == L'+' || m_text[m_pos] == L'-')) {
                m_pos++;
            }
            while (m_pos < m_text.size() && m_text[m_pos] >= L'0' &&
                   m_text[m_pos] <= L'9') {
                m_pos++;
            }
        }
        if (!sawDigit) {
            throw std::runtime_error(
                "Bad number literal in style variable expression");
        }
        auto parsed = ParseDoubleInvariant(m_text.substr(start, m_pos - start));
        if (!parsed) {
            throw std::runtime_error(
                "Bad number literal in style variable expression");
        }
        return *parsed;
    }

    double ParseIdentifierOrCall() {
        size_t start = m_pos;
        while (m_pos < m_text.size()) {
            wchar_t c = m_text[m_pos];
            if ((c >= L'A' && c <= L'Z') || (c >= L'a' && c <= L'z') ||
                (c >= L'0' && c <= L'9') || c == L'_') {
                m_pos++;
            } else {
                break;
            }
        }
        std::wstring_view ident = m_text.substr(start, m_pos - start);
        SkipWhitespace();
        if (m_pos < m_text.size() && m_text[m_pos] == L'(') {
            m_pos++;
            double a = ParseExpression();
            if (!ConsumeChar(L',')) {
                throw std::runtime_error(
                    "Expected ',' in min/max style variable call");
            }
            double b = ParseExpression();
            if (!ConsumeChar(L')')) {
                throw std::runtime_error(
                    "Missing ')' after min/max style variable call");
            }
            if (ident == L"min") {
                return (a < b) ? a : b;
            }
            if (ident == L"max") {
                return (a > b) ? a : b;
            }
            throw std::runtime_error(
                "Unknown function in style variable expression");
        }
        return LookupVariableNumeric(std::wstring(ident));
    }

    double LookupVariableNumeric(const std::wstring& name) {
        if (m_outDeps) {
            m_outDeps->push_back(name);
        }
        auto it = m_state->variables.find(name);
        if (it == m_state->variables.end()) {
            Wh_Log(L"Style variable '%s' not yet defined; treating as 0",
                   name.c_str());
            return 0.0;
        }
        if (!it->second.numeric) {
            throw std::runtime_error(
                "Style variable used in arithmetic is not numeric");
        }
        return *it->second.numeric;
    }

    std::wstring_view m_text;
    std::vector<std::wstring>* m_outDeps;
    StyleVariableState* m_state;
    size_t m_pos = 0;
};

// Evaluate a single expression body (the text between `{{` and `}}`). If the
// body is a bare identifier, returns the variable's `stringForm` directly --
// but only when the captured value is a primitive type flagged `substitutable`
// (numeric, boolean, or string). Missing variables and opaque-type captures
// both cause this function to return std::nullopt, at which point
// ExpandStyleVariables aborts the whole expansion and the consuming style is
// skipped. This matches the arithmetic path's behaviour of failing closed
// rather than substituting a value that won't parse.
std::optional<std::wstring> EvaluateStyleVariableExpression(
    std::wstring_view exprText,
    std::vector<std::wstring>* outDeps,
    StyleVariableState* state) {
    auto trimmed = TrimStringView(exprText);
    if (trimmed.empty()) {
        Wh_Log(L"Empty style variable expression");
        return std::nullopt;
    }

    if (IsValidStyleVariableIdentifier(trimmed)) {
        std::wstring name(trimmed);
        if (outDeps) {
            outDeps->push_back(name);
        }
        auto it = state->variables.find(name);
        if (it == state->variables.end()) {
            Wh_Log(L"Style variable '%s' not yet defined; skipping style",
                   name.c_str());
            return std::nullopt;
        }
        if (!it->second.substitutable) {
            Wh_Log(
                L"Style variable '%s' is not substitutable (captured type "
                L"'%s'); skipping style",
                name.c_str(), it->second.stringForm.c_str());
            return std::nullopt;
        }
        return it->second.stringForm;
    }

    try {
        StyleVariableExpressionEvaluator eval(trimmed, outDeps, state);
        double v = eval.Evaluate();
        return FormatDoubleInvariant(v);
    } catch (std::exception const& ex) {
        Wh_Log(L"Style variable expression failed: %S (in '%.*s')", ex.what(),
               static_cast<int>(trimmed.size()), trimmed.data());
        return std::nullopt;
    }
}

// Walks the input text, repeatedly expanding the innermost `{{ ... }}`
// substitution. Returns std::nullopt on parse failure (and logs a warning).
//
// Inner-matching rule: the first `}}` is paired with the *rightmost* `{{` that
// precedes it. So `{{{x}}}` -> `{` + value-of-x + `}` (literal outer braces).
//
// Substituted text is treated as literal (no further `{{...}}` expansion of the
// substituted output) to keep behavior predictable.
std::optional<std::wstring> ExpandStyleVariables(
    std::wstring_view input,
    std::vector<std::wstring>* outDeps,
    StyleVariableState* state) {
    std::wstring result(input);
    size_t scanFrom = 0;

    while (true) {
        size_t closePos = std::wstring::npos;
        for (size_t i = scanFrom; i + 1 < result.size(); i++) {
            if (result[i] == L'}' && result[i + 1] == L'}') {
                closePos = i;
                break;
            }
        }
        if (closePos == std::wstring::npos) {
            break;
        }

        // Find rightmost `{{` strictly before closePos. Search from closePos -
        // 1 downward; the pair occupies indices (j-1, j).
        size_t openPos = std::wstring::npos;
        if (closePos >= 2) {
            for (size_t j = closePos - 1; j >= 1; j--) {
                if (result[j - 1] == L'{' && result[j] == L'{') {
                    openPos = j - 1;
                    break;
                }
                if (j == 1) {
                    break;
                }
            }
        }

        if (openPos == std::wstring::npos) {
            Wh_Log(L"Unmatched '}}' in style value at offset %zu", closePos);
            return std::nullopt;
        }

        std::wstring_view exprText(result.data() + openPos + 2,
                                   closePos - openPos - 2);
        auto expanded =
            EvaluateStyleVariableExpression(exprText, outDeps, state);
        if (!expanded) {
            return std::nullopt;
        }

        size_t spanLen = closePos + 2 - openPos;
        result.replace(openPos, spanLen, *expanded);
        scanFrom = openPos + expanded->size();
    }

    return result;
}

// Read a property's current effective value and convert it to a
// StyleVariableValue suitable for `{{Var}}` substitution. Numeric primitives
// produce both string + numeric forms and are flagged substitutable; boolean
// and string primitives are flagged substitutable but have no numeric form.
// Opaque types (brushes, thicknesses, etc.) record only the captured class name
// as a diagnostic and are NOT flagged substitutable -- the bare- identifier
// substitution path skips them rather than emitting a class name into the XAML
// output.
StyleVariableValue ReadCapturedStyleVariableValue(FrameworkElement element,
                                                  DependencyProperty property) {
    StyleVariableValue out;

    auto elementDo = element.as<DependencyObject>();
    winrt::Windows::Foundation::IInspectable value{nullptr};
    // Get effective value so layout-driven properties like ActualWidth (which
    // never have a local value) still capture.
    try {
        value = elementDo.GetValue(property);
    } catch (winrt::hresult_error const& ex) {
        Wh_Log(L"Error %08X: %s", ex.code(), ex.message().c_str());
    }
    if (!value || value == DependencyProperty::UnsetValue()) {
        out.stringForm = L"";
        return out;
    }

    try {
        if (auto unboxed = TryUnboxPropertyValue(value)) {
            out.stringForm = FormatUnboxedPropertyValue(*unboxed);
            out.numeric = UnboxedPropertyValueAsNumeric(*unboxed);
            out.substitutable = true;
            return out;
        }

        // Opaque value (brush, thickness, etc.). Stored as a diagnostic only;
        // not flagged substitutable, so bare `{{Var}}` skips the consuming
        // style with a clear log message rather than emitting `className` into
        // the XAML.
        out.stringForm = std::wstring(winrt::get_class_name(value));
    } catch (winrt::hresult_error const& ex) {
        Wh_Log(L"Error %08X: %s", ex.code(), ex.message().c_str());
        out.stringForm = L"";
    }
    return out;
}

// Remove this (handle, property) entry from the consumer lists of every
// variable named in oldDeps, then add it for every variable named in newDeps.
// `fallbackClassName` is stored on each newly-added consumer entry so the
// per-consumer context is preserved across propagations; it is irrelevant when
// newDeps is empty (pure-removal calls from the cleanup paths).
void UpdateStyleVariableConsumers(StyleVariableState* state,
                                  InstanceHandle handle,
                                  DependencyProperty property,
                                  PCWSTR fallbackClassName,
                                  const std::vector<std::wstring>& oldDeps,
                                  const std::vector<std::wstring>& newDeps) {
    if (!state) {
        // The element's XamlRoot has already been destroyed (or was never
        // available); the StyleVariableState entry has been or will be reaped,
        // and there is nothing to clean up. New registrations (newDeps) are
        // also dropped on the floor: without a state we cannot route
        // propagations anyway.
        return;
    }

    for (const auto& dep : oldDeps) {
        auto it = state->consumers.find(dep);
        if (it == state->consumers.end()) {
            continue;
        }
        auto& consumers = it->second;
        std::erase_if(consumers, [&](const StyleVariableConsumer& c) {
            return c.elementHandle == handle && c.property == property;
        });
        if (consumers.empty()) {
            state->consumers.erase(it);
        }
    }

    std::wstring fallbackClassNameStr =
        fallbackClassName ? fallbackClassName : L"";
    for (const auto& dep : newDeps) {
        auto& consumers = state->consumers[dep];
        bool already = std::any_of(consumers.begin(), consumers.end(),
                                   [&](const StyleVariableConsumer& c) {
                                       return c.elementHandle == handle &&
                                              c.property == property;
                                   });
        if (!already) {
            consumers.push_back({handle, property, fallbackClassNameStr});
        }
    }
}

// Re-evaluate the dynamic template stored on `propertyCustomizationState` and
// return the resolved IInspectable / XamlBlurBrushParams ready to be applied.
// Updates the (handle, property) -> state->consumers registry to match the
// freshly computed dependency set so future variable changes route to this
// property. The dependency registry is committed *before* the final XAML
// resolution attempt: ExpandStyleVariables records every variable name it scans
// into newDeps even on partial parse failure, which lets a future change to any
// of those variables re-enter this function and retry. The trade-off is that on
// resolution failure the caller's last-good `customValue` is preserved (we
// return std::nullopt and the caller leaves the property as-is); this
// self-heals on the next variable change.
//
// `fallbackClassName` is the consumer-element's own fallback class name (the
// one that was used when matching the consumer's target rule), which is
// generally NOT the same as the capturer's. It is what
// ResolveExpandedSinglePropertyValue feeds to the synthetic <Style> used to
// re-parse the rule body, and it is also stored on each new
// StyleVariableConsumer entry so subsequent propagations route through this
// same context.
//
// Returns std::nullopt if the state has no template, expansion failed, or XAML
// resolution failed.
std::optional<PropertyOverrideValue> ResolveDynamicStyleValue(
    StyleVariableState* state,
    InstanceHandle handle,
    FrameworkElement element,
    DependencyProperty property,
    PCWSTR fallbackClassName,
    ElementPropertyCustomizationState* propertyCustomizationState) {
    if (!propertyCustomizationState->dynamicTemplate) {
        return std::nullopt;
    }

    const auto& tmpl = *propertyCustomizationState->dynamicTemplate;

    std::vector<std::wstring> newDeps;
    auto expanded = ExpandStyleVariables(tmpl.rawValue, &newDeps, state);

    UpdateStyleVariableConsumers(
        state, handle, property, fallbackClassName,
        propertyCustomizationState->variableDependencies, newDeps);
    propertyCustomizationState->variableDependencies = std::move(newDeps);

    if (!expanded) {
        return std::nullopt;
    }

    auto typeName = winrt::get_class_name(element);
    auto resolved = ResolveExpandedSinglePropertyValue(
        std::wstring_view(typeName),
        fallbackClassName ? std::wstring_view(fallbackClassName)
                          : winrt::name_of<FrameworkElement>(),
        tmpl.propertyName, *expanded, tmpl.isXamlValue);
    if (!resolved) {
        Wh_Log(
            L"Dynamic style resolution failed for '%s' on %s; keeping "
            L"previously applied value",
            tmpl.propertyName.c_str(), typeName.c_str());
    }
    return resolved;
}

// Re-evaluate every dependent style for the named variable. Driven by capture
// callbacks when the source property changes, and by the initial capture when a
// target is first matched. Each consumer carries its own fallbackClassName
// (recorded when the consumer was registered), so propagation correctly uses
// the consumer's own match-site context to re-parse the rule body, even when
// the capturer was matched against a different type/fallback class.
void PropagateStyleVariableChange(StyleVariableState* state,
                                  const std::wstring& varName) {
    auto consumersIt = state->consumers.find(varName);
    if (consumersIt == state->consumers.end()) {
        return;
    }

    auto consumersCopy = consumersIt->second;
    for (const auto& consumer : consumersCopy) {
        auto stateIt =
            g_elementsCustomizationState.find(consumer.elementHandle);
        if (stateIt == g_elementsCustomizationState.end()) {
            continue;
        }
        auto element = stateIt->second.element.get();
        if (!element) {
            continue;
        }

        PCWSTR consumerFallbackClassName =
            consumer.fallbackClassName.empty()
                ? nullptr
                : consumer.fallbackClassName.c_str();

        for (auto& [vsgWeak, vsgState] : stateIt->second.perVisualStateGroup) {
            auto propIt =
                vsgState.propertyCustomizationStates.find(consumer.property);
            if (propIt == vsgState.propertyCustomizationStates.end()) {
                continue;
            }
            auto& propState = propIt->second;
            if (!propState.dynamicTemplate) {
                continue;
            }

            auto resolved = ResolveDynamicStyleValue(
                state, consumer.elementHandle, element, consumer.property,
                consumerFallbackClassName, &propState);
            if (!resolved) {
                continue;
            }
            if (!propState.originalValue) {
                propState.originalValue =
                    ReadLocalValueWithWorkaround(element, consumer.property);
            }
            propState.customValue = *resolved;

            bool wasModifying = g_elementPropertyModifying;
            g_elementPropertyModifying = true;
            SetOrClearValue(element, consumer.property, *resolved);
            propState.lastAppliedValue =
                ReadLocalValueWithWorkaround(element, consumer.property);
            g_elementPropertyModifying = wasModifying;
        }
    }
}

// Compare a captured value to whatever's currently in state->variables for the
// same name; if different, store and notify dependents. Each consumer's own
// fallbackClassName lives on the consumer entry, so this function does not need
// to be told the capturer's context. Used by every path that wants to publish a
// captured value -- the per-property capture callback, the SizeChanged
// catch-all, and the initial seeding loop -- so the no-op fast path applies
// uniformly.
void SetStyleVariableIfChangedAndPropagate(StyleVariableState* state,
                                           const std::wstring& varName,
                                           StyleVariableValue value) {
    auto it = state->variables.find(varName);
    if (it != state->variables.end() &&
        it->second.stringForm == value.stringForm &&
        it->second.numeric == value.numeric &&
        it->second.substitutable == value.substitutable) {
        Wh_Log(L"Style variable '%s' unchanged at '%s'", varName.c_str(),
               value.stringForm.c_str());
        return;
    }

    Wh_Log(L"Style variable '%s' changed: '%s' -> '%s'", varName.c_str(),
           it != state->variables.end() ? it->second.stringForm.c_str()
                                        : L"(unset)",
           value.stringForm.c_str());
    state->variables[varName] = std::move(value);
    PropagateStyleVariableChange(state, varName);
}

// True for layout-driven DPs whose updates do not fire
// RegisterPropertyChangedCallback on UWP, so capture rules on those DPs need
// `FrameworkElement.SizeChanged` as their notification source instead.
bool IsLayoutDrivenSizeProperty(DependencyProperty property) {
    return property == FrameworkElement::ActualWidthProperty() ||
           property == FrameworkElement::ActualHeightProperty();
}

// Wire up `Property=>VarName` capture rules for an element. Called once per
// matched element (captures are not visual-state-aware). Seeds the variables
// from the current property values, registers per-DP property-changed
// callbacks, and -- because UWP's ActualWidth/ActualHeight don't fire those
// callbacks on layout -- subscribes to FrameworkElement.SizeChanged as a
// catch-all that re-reads every active capture on resize.
//
// Seeding writes the captured values into state->variables in a single batch
// (to avoid intermediate inconsistent states for consumers that depend on
// multiple variables from this element) and then propagates only the variables
// whose values actually changed -- the no-op fast path matches the one used by
// the change-driven callbacks below. The function does not need the capturer's
// fallbackClassName: each StyleVariableConsumer entry already carries its own
// consumer-side fallback, so propagation routes through the right context per
// consumer.
void SetUpCapturesForElement(StyleVariableState* state,
                             InstanceHandle handle,
                             FrameworkElement element,
                             const std::vector<CaptureSpec>& captures,
                             ElementCustomizationState* elementState) {
    if (captures.empty()) {
        return;
    }

    auto elementDo = element.as<DependencyObject>();
    winrt::weak_ref<FrameworkElement> elementWeakRef = element;

    // Names of variables whose seeded value differs from whatever's already in
    // state->variables. Only these need a propagation pass at the end.
    std::vector<std::wstring> changedVarNames;
    changedVarNames.reserve(captures.size());

    // Captures whose source DP is layout-driven (ActualWidth/ActualHeight) need
    // a SizeChanged subscription as their notification source. Collect them so
    // we only subscribe once and only when needed.
    std::vector<std::pair<DependencyProperty, std::wstring>>
        sizeChangedCaptures;

    for (const auto& capture : captures) {
        const auto [it, inserted] =
            elementState->captureCustomizationStates.insert(
                {capture.property, {}});
        if (!inserted) {
            // Same DP captured twice on this element (different rules with the
            // same property); keep the first and warn so the dropped second is
            // not a silent footgun for users who later try to reference the
            // dropped variable in a `{{...}}` substitution.
            Wh_Log(
                L"Capture for property already registered on %s; "
                L"dropping duplicate variable '%s' (kept: '%s')",
                winrt::get_class_name(element).c_str(), capture.varName.c_str(),
                it->second.varName.c_str());
            continue;
        }
        auto& captureState = it->second;
        captureState.varName = capture.varName;

        auto value = ReadCapturedStyleVariableValue(element, capture.property);

        auto existingIt = state->variables.find(capture.varName);
        const bool changed =
            existingIt == state->variables.end() ||
            existingIt->second.stringForm != value.stringForm ||
            existingIt->second.numeric != value.numeric ||
            existingIt->second.substitutable != value.substitutable;

        if (changed) {
            Wh_Log(
                L"Seeding capture variable '%s' from %s with value '%s' "
                L"(was: '%s')",
                capture.varName.c_str(), winrt::get_class_name(element).c_str(),
                value.stringForm.c_str(),
                existingIt != state->variables.end()
                    ? existingIt->second.stringForm.c_str()
                    : L"(unset)");
            state->variables[capture.varName] = std::move(value);
            changedVarNames.push_back(capture.varName);
        } else {
            Wh_Log(L"Capture variable '%s' from %s already at '%s'",
                   capture.varName.c_str(),
                   winrt::get_class_name(element).c_str(),
                   value.stringForm.c_str());
        }

        if (IsLayoutDrivenSizeProperty(capture.property)) {
            sizeChangedCaptures.push_back({capture.property, capture.varName});
            // No property-changed callback: the DP doesn't fire one for layout
            // updates anyway, and SizeChanged below covers it.
            continue;
        }

        std::wstring varName = capture.varName;
        captureState.propertyChangedToken =
            elementDo.RegisterPropertyChangedCallback(
                capture.property,
                [state, varName, elementWeakRef](DependencyObject sender,
                                                 DependencyProperty property) {
                    auto element = elementWeakRef.get();
                    if (!element) {
                        return;
                    }
                    auto value =
                        ReadCapturedStyleVariableValue(element, property);
                    SetStyleVariableIfChangedAndPropagate(state, varName,
                                                          std::move(value));
                });
    }

    if (!sizeChangedCaptures.empty()) {
        elementState->captureSizeChangedToken = element.SizeChanged(
            [state, elementWeakRef,
             sizeChangedCaptures = std::move(sizeChangedCaptures)](
                winrt::Windows::Foundation::IInspectable const& sender,
                SizeChangedEventArgs const& e) {
                auto element = elementWeakRef.get();
                if (!element) {
                    return;
                }
                Wh_Log(L"SizeChanged on %s: %.3fx%.3f",
                       winrt::get_class_name(element).c_str(),
                       e.NewSize().Width, e.NewSize().Height);
                for (const auto& [property, varName] : sizeChangedCaptures) {
                    auto value =
                        ReadCapturedStyleVariableValue(element, property);
                    SetStyleVariableIfChangedAndPropagate(state, varName,
                                                          std::move(value));
                }
            });
    }

    // Propagate the freshly seeded values to any consumers that were already
    // registered before this element was matched. Variables whose value did not
    // actually change are skipped, matching the per-callback fast path.
    for (const auto& varName : changedVarNames) {
        PropagateStyleVariableChange(state, varName);
    }
}

// Tear down capture subscriptions for an element. Called from
// CleanupCustomizations and UninitializeSettingsAndTap before the
// ElementCustomizationState entry is erased.
void RestoreCapturesForElement(FrameworkElement element,
                               const ElementCustomizationState& elementState) {
    if (!element) {
        return;
    }

    for (const auto& [property, captureState] :
         elementState.captureCustomizationStates) {
        if (!captureState.propertyChangedToken) {
            continue;
        }
        try {
            element.UnregisterPropertyChangedCallback(
                property, captureState.propertyChangedToken);
        } catch (winrt::hresult_error const& ex) {
            Wh_Log(L"Error %08X: %s", ex.code(), ex.message().c_str());
        }
    }

    if (elementState.captureSizeChangedToken) {
        try {
            element.SizeChanged(elementState.captureSizeChangedToken);
        } catch (winrt::hresult_error const& ex) {
            Wh_Log(L"Error %08X: %s", ex.code(), ex.message().c_str());
        }
    }
}

void ApplyCustomizationsForVisualStateGroup(
    StyleVariableState* state,
    InstanceHandle handle,
    FrameworkElement element,
    VisualStateGroup visualStateGroup,
    PCWSTR fallbackClassName,
    PropertyOverrides propertyOverrides,
    ElementCustomizationStateForVisualStateGroup*
        elementCustomizationStateForVisualStateGroup) {
    auto elementDo = element.as<DependencyObject>();

    VisualState currentVisualState(
        visualStateGroup ? visualStateGroup.CurrentState() : nullptr);

    std::wstring currentVisualStateName(
        currentVisualState ? currentVisualState.Name() : L"");

    for (const auto& [property, valuesPerVisualState] : propertyOverrides) {
        const auto [propertyCustomizationStatesIt, inserted] =
            elementCustomizationStateForVisualStateGroup
                ->propertyCustomizationStates.insert({property, {}});
        if (!inserted) {
            continue;
        }

        auto& propertyCustomizationState =
            propertyCustomizationStatesIt->second;

        auto it = valuesPerVisualState.find(currentVisualStateName);
        if (it == valuesPerVisualState.end() &&
            !currentVisualStateName.empty()) {
            it = valuesPerVisualState.find(L"");
        }

        if (it != valuesPerVisualState.end()) {
            std::optional<PropertyOverrideValue> resolved;
            if (auto* tmpl = std::get_if<DynamicStyleTemplate>(&it->second)) {
                propertyCustomizationState.dynamicTemplate = *tmpl;
                resolved = ResolveDynamicStyleValue(
                    state, handle, element, property, fallbackClassName,
                    &propertyCustomizationState);
            } else {
                resolved = it->second;
            }

            if (resolved) {
                propertyCustomizationState.originalValue =
                    ReadLocalValueWithWorkaround(element, property);
                propertyCustomizationState.customValue = *resolved;
                SetOrClearValue(element, property, *resolved,
                                /*initialApply=*/true);
                propertyCustomizationState.lastAppliedValue =
                    ReadLocalValueWithWorkaround(element, property);
            }
        }

        propertyCustomizationState.propertyChangedToken =
            elementDo.RegisterPropertyChangedCallback(
                property,
                [&propertyCustomizationState](DependencyObject sender,
                                              DependencyProperty property) {
                    if (g_elementPropertyModifying) {
                        return;
                    }

                    auto element = sender.try_as<FrameworkElement>();
                    if (!element) {
                        return;
                    }

                    if (!propertyCustomizationState.customValue) {
                        return;
                    }

                    auto localValue =
                        ReadLocalValueWithWorkaround(element, property);

                    // Only update originalValue if the local value was changed
                    // externally (e.g. by a Setter). When an animation changes
                    // only the effective value, the local value still matches
                    // what we set, so updating originalValue would corrupt it
                    // with our own brush - causing the brush to survive cleanup
                    // and crash when the mod's DLL is unloaded.
                    if (localValue !=
                        propertyCustomizationState.lastAppliedValue) {
                        propertyCustomizationState.originalValue = localValue;
                    }

                    Wh_Log(L"Re-applying style for %s",
                           winrt::get_class_name(element).c_str());

                    g_elementPropertyModifying = true;
                    SetOrClearValue(element, property,
                                    *propertyCustomizationState.customValue);
                    propertyCustomizationState.lastAppliedValue =
                        ReadLocalValueWithWorkaround(element, property);
                    g_elementPropertyModifying = false;
                });
    }

    if (visualStateGroup) {
        winrt::weak_ref<FrameworkElement> elementWeakRef = element;
        std::wstring fallbackClassNameStr =
            fallbackClassName ? fallbackClassName : L"";
        elementCustomizationStateForVisualStateGroup
            ->visualStateGroupCurrentStateChangedToken =
            visualStateGroup.CurrentStateChanged(
                [state, elementWeakRef, propertyOverrides, handle,
                 fallbackClassNameStr,
                 elementCustomizationStateForVisualStateGroup](
                    winrt::Windows::Foundation::IInspectable const& sender,
                    VisualStateChangedEventArgs const& e) {
                    auto element = elementWeakRef.get();
                    if (!element) {
                        return;
                    }

                    Wh_Log(L"Re-applying all styles for %s",
                           winrt::get_class_name(element).c_str());

                    g_elementPropertyModifying = true;

                    auto& propertyCustomizationStates =
                        elementCustomizationStateForVisualStateGroup
                            ->propertyCustomizationStates;

                    PCWSTR fallbackClassNamePtr =
                        fallbackClassNameStr.empty()
                            ? nullptr
                            : fallbackClassNameStr.c_str();

                    for (const auto& [property, valuesPerVisualState] :
                         propertyOverrides) {
                        auto& propertyCustomizationState =
                            propertyCustomizationStates.at(property);

                        auto newState = e.NewState();
                        auto newStateName =
                            std::wstring{newState ? newState.Name() : L""};
                        auto it = valuesPerVisualState.find(newStateName);
                        if (it == valuesPerVisualState.end()) {
                            it = valuesPerVisualState.find(L"");
                            if (it != valuesPerVisualState.end()) {
                                auto oldState = e.OldState();
                                auto oldStateName = std::wstring{
                                    oldState ? oldState.Name() : L""};
                                if (!valuesPerVisualState.contains(
                                        oldStateName)) {
                                    continue;
                                }
                            }
                        }

                        if (it != valuesPerVisualState.end()) {
                            std::optional<PropertyOverrideValue> resolved;
                            if (auto* tmpl = std::get_if<DynamicStyleTemplate>(
                                    &it->second)) {
                                propertyCustomizationState.dynamicTemplate =
                                    *tmpl;
                                resolved = ResolveDynamicStyleValue(
                                    state, handle, element, property,
                                    fallbackClassNamePtr,
                                    &propertyCustomizationState);
                            } else {
                                // Transitioning from dynamic to static for this
                                // visual state: clear template metadata and
                                // unregister consumer entries.
                                if (propertyCustomizationState
                                        .dynamicTemplate) {
                                    UpdateStyleVariableConsumers(
                                        state, handle, property,
                                        /*fallbackClassName=*/nullptr,
                                        propertyCustomizationState
                                            .variableDependencies,
                                        {});
                                    propertyCustomizationState
                                        .variableDependencies.clear();
                                    propertyCustomizationState.dynamicTemplate
                                        .reset();
                                }

                                resolved = it->second;
                            }

                            if (resolved) {
                                if (!propertyCustomizationState.originalValue) {
                                    propertyCustomizationState.originalValue =
                                        ReadLocalValueWithWorkaround(element,
                                                                     property);
                                }

                                propertyCustomizationState.customValue =
                                    *resolved;
                                SetOrClearValue(element, property, *resolved);
                                propertyCustomizationState.lastAppliedValue =
                                    ReadLocalValueWithWorkaround(element,
                                                                 property);
                            }
                        } else {
                            if (propertyCustomizationState.dynamicTemplate) {
                                UpdateStyleVariableConsumers(
                                    state, handle, property,
                                    /*fallbackClassName=*/nullptr,
                                    propertyCustomizationState
                                        .variableDependencies,
                                    {});
                                propertyCustomizationState.variableDependencies
                                    .clear();
                                propertyCustomizationState.dynamicTemplate
                                    .reset();
                            }
                            if (propertyCustomizationState.originalValue) {
                                SetOrClearValue(
                                    element, property,
                                    *propertyCustomizationState.originalValue);
                                propertyCustomizationState.originalValue
                                    .reset();
                            }
                            propertyCustomizationState.lastAppliedValue =
                                nullptr;

                            propertyCustomizationState.customValue.reset();
                        }
                    }

                    g_elementPropertyModifying = false;
                });
    }
}

void RestoreCustomizationsForVisualStateGroup(
    StyleVariableState* state,
    InstanceHandle handle,
    FrameworkElement element,
    std::optional<winrt::weak_ref<VisualStateGroup>>
        visualStateGroupOptionalWeakPtr,
    const ElementCustomizationStateForVisualStateGroup&
        elementCustomizationStateForVisualStateGroup) {
    if (element) {
        for (const auto& [property, propState] :
             elementCustomizationStateForVisualStateGroup
                 .propertyCustomizationStates) {
            try {
                element.UnregisterPropertyChangedCallback(
                    property, propState.propertyChangedToken);
            } catch (winrt::hresult_error const& ex) {
                Wh_Log(L"Error %08X: %s", ex.code(), ex.message().c_str());
            }

            if (!propState.variableDependencies.empty()) {
                UpdateStyleVariableConsumers(state, handle, property,
                                             /*fallbackClassName=*/nullptr,
                                             propState.variableDependencies,
                                             {});
            }

            if (propState.originalValue) {
                SetOrClearValue(element, property, *propState.originalValue);
            }
        }
    } else {
        // Element is gone; still clear consumer entries so a stale (handle,
        // property) pair isn't visited during PropagateStyleVariableChange.
        for (const auto& [property, propState] :
             elementCustomizationStateForVisualStateGroup
                 .propertyCustomizationStates) {
            if (!propState.variableDependencies.empty()) {
                UpdateStyleVariableConsumers(state, handle, property,
                                             /*fallbackClassName=*/nullptr,
                                             propState.variableDependencies,
                                             {});
            }
        }
    }

    auto visualStateGroupIter = visualStateGroupOptionalWeakPtr
                                    ? visualStateGroupOptionalWeakPtr->get()
                                    : nullptr;
    if (visualStateGroupIter && elementCustomizationStateForVisualStateGroup
                                    .visualStateGroupCurrentStateChangedToken) {
        try {
            visualStateGroupIter.CurrentStateChanged(
                elementCustomizationStateForVisualStateGroup
                    .visualStateGroupCurrentStateChangedToken);
        } catch (winrt::hresult_error const& ex) {
            Wh_Log(L"Error %08X: %s", ex.code(), ex.message().c_str());
        }
    }
}

// Workaround to the breaking layout with custom column definitions on multiple
// taskbars:
// https://github.com/ramensoftware/windows-11-taskbar-styling-guide/issues/507
void HookFirstTaskbarFrameLayoutWorkaround(
    winrt::Windows::UI::Xaml::FrameworkElement element) {
        if (g_workaroundSizeChangedRevoker) {
            return;
        }

    if (winrt::get_class_name(element) != L"Taskbar.TaskbarFrame") {
        return;
    }

    auto parent = Media::VisualTreeHelper::GetParent(element)
                      .try_as<winrt::Windows::UI::Xaml::FrameworkElement>();
    if (!parent) {
        return;
    }

    // cppwinrt doesn't expose Grid::ColumnDefinitionsProperty() as a static, so
    // resolve it the same way GetResolvedPropertyOverrides does - parse a
    // synthetic Setter and pull the DP out. The DP identity is stable, so the
    // result matches the key used in propertyCustomizationStates.
    DependencyProperty colDefsProp{nullptr};
    try {
        auto style = GetStyleFromXamlSettersWithFallbackType(
            L"Windows.UI.Xaml.Controls.Grid", L"Windows.UI.Xaml.Controls.Grid",
            L"        <Setter Property=\"ColumnDefinitions\" "
            L"Value=\"{x:Null}\" />\n");
        colDefsProp = style.Setters().GetAt(0).as<Setter>().Property();
    } catch (...) {
        Wh_Log(L"Error %08X: %s", winrt::to_hresult(),
               winrt::to_message().c_str());
        return;
    }

    if (!colDefsProp) {
        return;
    }

    // Find the parent's already-applied customization for ColumnDefinitions
    // (set when ApplyCustomizations ran on the parent moments earlier). If it's
    // not customized, there's nothing for us to restore.
    auto stateIt =
        std::find_if(g_elementsCustomizationState.begin(),
                     g_elementsCustomizationState.end(), [&](const auto& kv) {
                         auto el = kv.second.element.get();
                         return el && el == parent;
                     });
    if (stateIt == g_elementsCustomizationState.end()) {
        return;
    }

    // Only consider the no-visual-state entry: the layout Grid has no visual
    // states.
    auto vsgIt = std::find_if(stateIt->second.perVisualStateGroup.begin(),
                              stateIt->second.perVisualStateGroup.end(),
                              [](const auto& entry) { return !entry.first; });
    if (vsgIt == stateIt->second.perVisualStateGroup.end()) {
        return;
    }

    auto propIt = vsgIt->second.propertyCustomizationStates.find(colDefsProp);
    if (propIt == vsgIt->second.propertyCustomizationStates.end() ||
        !propIt->second.customValue) {
        return;
    }

    auto savedValue = *propIt->second.customValue;

    auto weakParent = winrt::make_weak(parent);
    g_workaroundSizeChangedRevoker = element.SizeChanged(
        winrt::auto_revoke, [weakParent, savedValue = std::move(savedValue),
                             colDefsProp](auto&&, auto&&) {
            auto parent = weakParent.get();
            if (!parent) {
                return;
            }

            // Suppress the per-DP property-changed callback installed by the
            // customization machinery so re-setting doesn't cascade.
            g_elementPropertyModifying = true;
            try {
                // Clear first, then set, otherwise the workaround doesn't work.
                SetOrClearValue(
                    parent, colDefsProp,
                    PropertyOverrideValue{DependencyProperty::UnsetValue()});
                SetOrClearValue(parent, colDefsProp, savedValue);
            } catch (...) {
                Wh_Log(L"Error %08X: %s", winrt::to_hresult(),
                       winrt::to_message().c_str());
            }
            g_elementPropertyModifying = false;
        });

    Wh_Log(L"Hooked TaskbarFrame SizeChanged for ColumnDefinitions workaround");
}

void UnhookFirstTaskbarFrameLayoutWorkaround() {
    g_workaroundSizeChangedRevoker = {};
}

void MergeResourceVariables();

void ApplyCustomizations(InstanceHandle handle,
                         FrameworkElement element,
                         PCWSTR fallbackClassName) {
    // Merge resource dictionary on first element add. Merging it earlier on
    // window creation doesn't work, perhaps merged dictionaries are reset
    // during initialization.
    if (!g_resourceVariablesThemeDict) {
        MergeResourceVariables();
    }

    auto* state = GetStyleVariableState(element);
    if (!state) {
        Wh_Log(L"No XamlRoot for %s, skipping",
               winrt::get_class_name(element).c_str());
        return;
    }

    auto resolved = FindElementPropertyOverrides(element, fallbackClassName);
    if (resolved.overridesPerVSG.empty() && resolved.captures.empty()) {
        return;
    }

    Wh_Log(L"Applying styles to %s", winrt::get_class_name(element).c_str());

    auto& elementCustomizationState = g_elementsCustomizationState[handle];

    for (const auto& [visualStateGroupOptionalWeakPtrIter, stateIter] :
         elementCustomizationState.perVisualStateGroup) {
        RestoreCustomizationsForVisualStateGroup(
            state, handle, element, visualStateGroupOptionalWeakPtrIter,
            stateIter);
    }

    elementCustomizationState.element = element;
    elementCustomizationState.xamlRoot = state->xamlRoot;
    elementCustomizationState.perVisualStateGroup.clear();

    // Wire up captures first so any variables they define are visible to
    // dynamic value-rules applied below. Note: SetUpCapturesForElement does not
    // need this element's fallbackClassName -- propagation routes through each
    // consumer's own stored fallback.
    SetUpCapturesForElement(state, handle, element, resolved.captures,
                            &elementCustomizationState);

    for (auto& [visualStateGroup, overridesForVisualStateGroup] :
         resolved.overridesPerVSG) {
        std::optional<winrt::weak_ref<VisualStateGroup>>
            visualStateGroupOptionalWeakPtr;
        if (visualStateGroup) {
            visualStateGroupOptionalWeakPtr = visualStateGroup;
        }

        elementCustomizationState.perVisualStateGroup.push_back(
            {visualStateGroupOptionalWeakPtr, {}});
        auto* elementCustomizationStateForVisualStateGroup =
            &elementCustomizationState.perVisualStateGroup.back().second;

        ApplyCustomizationsForVisualStateGroup(
            state, handle, element, visualStateGroup, fallbackClassName,
            std::move(overridesForVisualStateGroup),
            elementCustomizationStateForVisualStateGroup);
    }

    HookFirstTaskbarFrameLayoutWorkaround(element);
}

void CleanupCustomizations(InstanceHandle handle) {
    if (auto it = g_elementsCustomizationState.find(handle);
        it != g_elementsCustomizationState.end()) {
        auto& elementCustomizationState = it->second;

        auto element = elementCustomizationState.element.get();
        auto* state = GetStyleVariableState(elementCustomizationState.xamlRoot);

        RestoreCapturesForElement(element, elementCustomizationState);

        for (const auto& [visualStateGroupOptionalWeakPtrIter, stateIter] :
             elementCustomizationState.perVisualStateGroup) {
            RestoreCustomizationsForVisualStateGroup(
                state, handle, element, visualStateGroupOptionalWeakPtrIter,
                stateIter);
        }

        g_elementsCustomizationState.erase(it);
    }
}

using StyleConstant = std::pair<std::wstring, std::wstring>;
using StyleConstants = std::vector<StyleConstant>;

std::wstring ApplyStyleConstants(std::wstring_view style,
                                 const StyleConstants& styleConstants) {
    std::wstring result;

    size_t lastPos = 0;
    size_t findPos;

    while ((findPos = style.find('$', lastPos)) != style.npos) {
        result.append(style, lastPos, findPos - lastPos);

        const StyleConstant* constant = nullptr;
        for (const auto& s : styleConstants) {
            if (s.first == style.substr(findPos + 1, s.first.size())) {
                constant = &s;
                break;
            }
        }

        if (constant) {
            result += constant->second;
            lastPos = findPos + 1 + constant->first.size();
        } else {
            result += '$';
            lastPos = findPos + 1;
        }
    }

    // Care for the rest after last occurrence.
    result += style.substr(lastPos);

    return result;
}

std::optional<StyleConstant> ParseStyleConstant(
    std::wstring_view constant,
    const StyleConstants& styleConstants) {
    // Skip if commented.
    if (constant.starts_with(L"//")) {
        return std::nullopt;
    }

    auto eqPos = constant.find(L'=');
    if (eqPos == constant.npos) {
        Wh_Log(L"Skipping entry with no '=': %.*s",
               static_cast<int>(constant.length()), constant.data());
        return std::nullopt;
    }

    auto key = TrimStringView(constant.substr(0, eqPos));
    auto valueRaw = TrimStringView(constant.substr(eqPos + 1));
    auto value = ApplyStyleConstants(valueRaw, styleConstants);

    return StyleConstant{std::wstring(key), std::move(value)};
}

StyleConstants LoadStyleConstants(
    const std::vector<PCWSTR>& themeStyleConstants) {
    StyleConstants result;

    auto addToResult = [&result](StyleConstant sc) {
        // Keep sorted by name length to replace long names first. Reverse the
        // order to allow overriding definitions with the same name.
        auto insertIndex = std::lower_bound(
            result.begin(), result.end(), sc,
            [](const StyleConstant& a, const StyleConstant& b) {
                return a.first.size() > b.first.size();
            });

        result.insert(insertIndex, std::move(sc));
    };

    for (const auto themeStyleConstant : themeStyleConstants) {
        if (auto parsed = ParseStyleConstant(themeStyleConstant, result)) {
            addToResult(std::move(*parsed));
        }
    }

#ifndef WIN11_TASKBAR_WIN10_MULTIROW_COMBINED
    for (int i = 0;; i++) {
        string_setting_unique_ptr constantSetting(
            Wh_GetStringSetting(L"styleConstants[%d]", i));
        if (!*constantSetting.get()) {
            break;
        }

        if (auto parsed = ParseStyleConstant(constantSetting.get(), result)) {
            addToResult(std::move(*parsed));
        }
    }
#endif

    return result;
}

ElementMatcher ElementMatcherFromString(std::wstring_view str) {
    ElementMatcher result;
    PropertyValuesUnresolved propertyValuesUnresolved;

    auto trimmed = TrimStringView(str);
    if (trimmed == L"*") {
        result.kind = ElementMatcher::Kind::Wildcard;
        return result;
    }
    if (trimmed == L":root") {
        result.kind = ElementMatcher::Kind::Root;
        return result;
    }

    auto i = str.find_first_of(L"#@[");
    result.type = TrimStringView(str.substr(0, i));
    if (result.type.empty()) {
        throw std::runtime_error("Bad target syntax, empty type");
    }

    while (i != str.npos) {
        auto iNext = str.find_first_of(L"#@[", i + 1);
        auto nextPart =
            str.substr(i + 1, iNext == str.npos ? str.npos : iNext - (i + 1));

        switch (str[i]) {
            case L'#':
                if (!result.name.empty()) {
                    throw std::runtime_error(
                        "Bad target syntax, more than one name");
                }

                result.name = TrimStringView(nextPart);
                if (result.name.empty()) {
                    throw std::runtime_error("Bad target syntax, empty name");
                }
                break;

            case L'@':
                if (result.visualStateGroupName) {
                    throw std::runtime_error(
                        "Bad target syntax, more than one visual state group");
                }

                result.visualStateGroupName = TrimStringView(nextPart);
                break;

            case L'[': {
                auto rule = TrimStringView(nextPart);
                if (rule.length() == 0 || rule.back() != L']') {
                    throw std::runtime_error("Bad target syntax, missing ']'");
                }

                rule = TrimStringView(rule.substr(0, rule.length() - 1));
                if (rule.length() == 0) {
                    throw std::runtime_error(
                        "Bad target syntax, empty property");
                }

                if (rule.find_first_not_of(L"0123456789") == rule.npos) {
                    result.oneBasedIndex = std::stoi(std::wstring(rule));
                    break;
                }

                auto ruleEqPos = rule.find(L'=');
                if (ruleEqPos == rule.npos) {
                    throw std::runtime_error(
                        "Bad target syntax, missing '=' in property");
                }

                auto ruleKey = TrimStringView(rule.substr(0, ruleEqPos));
                auto ruleVal = TrimStringView(rule.substr(ruleEqPos + 1));

                if (ruleKey.length() == 0) {
                    throw std::runtime_error(
                        "Bad target syntax, empty property name");
                }

                propertyValuesUnresolved.push_back(
                    {std::wstring(ruleKey), std::wstring(ruleVal)});
                break;
            }

            default:
                throw std::runtime_error("Bad target syntax");
        }

        i = iNext;
    }

    result.propertyValues = std::move(propertyValuesUnresolved);

    return result;
}

// Parses a single `controlStyles[*].styles[*]` entry into either a ValueRule
// (`Property[@VisualState][:]=value`) or a CaptureRule (`Property=>VarName`).
// Throws std::runtime_error on malformed input or disallowed combinations such
// as `:=>` or `@VisualState=>`.
std::variant<ValueRule, CaptureRule> ParseRule(std::wstring_view str) {
    auto eqPos = str.find(L'=');
    if (eqPos == str.npos) {
        throw std::runtime_error("Bad style syntax, '=' is missing");
    }

    auto name = str.substr(0, eqPos);
    auto value = str.substr(eqPos + 1);

    if (!value.empty() && value.front() == L'>') {
        // Capture rule: `Property=>VarName`. The right-hand side (after the
        // leading `>` marker) is the name of a mod-global style variable into
        // which the property's current value is captured.
        value = value.substr(1);

        if (!name.empty() && name.back() == L':') {
            throw std::runtime_error(
                "Bad style syntax, ':=>' is not valid (':=' XAML value "
                "cannot be combined with '=>' capture)");
        }

        if (name.find(L'@') != name.npos) {
            throw std::runtime_error(
                "Bad style syntax, '@VisualState' not allowed on a capture "
                "rule");
        }

        auto trimmedPropertyName = TrimStringView(name);
        if (trimmedPropertyName.empty()) {
            throw std::runtime_error("Bad style syntax, empty name");
        }

        auto trimmedVarName = TrimStringView(value);
        if (trimmedVarName.empty()) {
            throw std::runtime_error(
                "Bad style syntax, empty capture variable name");
        }
        if (!IsValidStyleVariableIdentifier(trimmedVarName)) {
            throw std::runtime_error(
                "Bad style syntax, invalid capture variable name");
        }

        return CaptureRule{std::wstring(trimmedPropertyName),
                           std::wstring(trimmedVarName)};
    }

    ValueRule result;
    result.value = TrimStringView(value);

    if (!name.empty() && name.back() == L':') {
        result.isXamlValue = true;
        name = name.substr(0, name.size() - 1);
    }

    auto atPos = name.find(L'@');
    if (atPos != name.npos) {
        result.visualState = TrimStringView(name.substr(atPos + 1));
        name = name.substr(0, atPos);
    }

    result.propertyName = TrimStringView(name);
    if (result.propertyName.empty()) {
        throw std::runtime_error("Bad style syntax, empty name");
    }

    return result;
}

std::wstring AdjustTypeName(std::wstring_view type) {
    if (type.find_first_of(L".:") == type.npos) {
        if (type == L"Rectangle") {
            return L"Windows.UI.Xaml.Shapes.Rectangle";
        }

        return L"Windows.UI.Xaml.Controls." + std::wstring{type};
    }

    static const std::vector<std::pair<std::wstring_view, std::wstring_view>>
        adjustments = {
            {L"taskbar:", L"Taskbar."},
            {L"systemtray:", L"SystemTray."},
            {L"udk:", L"WindowsUdk.UI.Shell."},
            {L"muxc:", L"Microsoft.UI.Xaml.Controls."},
        };

    for (const auto& adjustment : adjustments) {
        if (type.starts_with(adjustment.first)) {
            auto result = std::wstring{adjustment.second};
            result += type.substr(adjustment.first.size());
            return result;
        }
    }

    return std::wstring{type};
}

void AddElementCustomizationRules(std::wstring_view target,
                                  std::vector<std::wstring> styles) {
    ElementCustomizationRules elementCustomizationRules;

    auto targetParts = SplitStringView(target, L" > ");

    bool first = true;
    bool hasVisualStateGroup = false;
    for (auto i = targetParts.rbegin(); i != targetParts.rend(); ++i) {
        const auto& targetPart = *i;
        const bool isLeftmost = (i + 1 == targetParts.rend());

        auto matcher = ElementMatcherFromString(targetPart);

        const auto& prevParents =
            elementCustomizationRules.parentElementMatchers;
        const bool prevIsWildcard =
            !prevParents.empty() &&
            prevParents.back().kind == ElementMatcher::Kind::Wildcard;

        switch (matcher.kind) {
            case ElementMatcher::Kind::Element:
                matcher.type = AdjustTypeName(matcher.type);
                break;

            case ElementMatcher::Kind::Wildcard:
                if (first) {
                    throw std::runtime_error(
                        "Bad target syntax, '*' can't be the matched element");
                }
                if (isLeftmost) {
                    throw std::runtime_error(
                        "Bad target syntax, '*' can't be the leftmost target "
                        "part");
                }
                if (prevIsWildcard) {
                    throw std::runtime_error(
                        "Bad target syntax, '*' can't be adjacent to another "
                        "'*'");
                }
                break;

            case ElementMatcher::Kind::Root:
                if (first) {
                    throw std::runtime_error(
                        "Bad target syntax, ':root' can't be the matched "
                        "element");
                }
                if (!isLeftmost) {
                    throw std::runtime_error(
                        "Bad target syntax, ':root' must be the leftmost "
                        "target part");
                }
                if (prevIsWildcard) {
                    throw std::runtime_error(
                        "Bad target syntax, ':root' must be followed by a "
                        "non-wildcard target part");
                }
                break;
        }

        if (matcher.visualStateGroupName) {
            if (hasVisualStateGroup) {
                throw std::runtime_error(
                    "Element type can't have more than one visual state group");
            }

            hasVisualStateGroup = true;
        }

        if (first) {
            UnresolvedRules unresolvedRules;
            for (const auto& style : styles) {
                auto parsed = ParseRule(style);
                if (auto* valueRule = std::get_if<ValueRule>(&parsed)) {
                    unresolvedRules.valueRules.push_back(std::move(*valueRule));
                } else {
                    unresolvedRules.captureRules.push_back(
                        std::move(std::get<CaptureRule>(parsed)));
                }
            }

            elementCustomizationRules.elementMatcher = std::move(matcher);
            elementCustomizationRules.propertyOverrides =
                std::move(unresolvedRules);
        } else {
            elementCustomizationRules.parentElementMatchers.push_back(
                std::move(matcher));
        }

        first = false;
    }

    g_elementsCustomizationRules.push_back(
        std::move(elementCustomizationRules));
}

bool ProcessSingleTargetStylesFromSettings(
    int index,
    const StyleConstants& styleConstants) {
    string_setting_unique_ptr targetStringSetting(
        Wh_GetStringSetting(L"controlStyles[%d].target", index));
    if (!*targetStringSetting.get()) {
        return false;
    }

    // Skip if commented.
    if (targetStringSetting[0] == L'/' && targetStringSetting[1] == L'/') {
        return true;
    }

    Wh_Log(L"Processing styles for %s", targetStringSetting.get());

    std::vector<std::wstring> styles;

    for (int styleIndex = 0;; styleIndex++) {
        string_setting_unique_ptr styleSetting(Wh_GetStringSetting(
            L"controlStyles[%d].styles[%d]", index, styleIndex));
        if (!*styleSetting.get()) {
            break;
        }

        // Skip if commented.
        if (styleSetting[0] == L'/' && styleSetting[1] == L'/') {
            continue;
        }

        styles.push_back(
            ApplyStyleConstants(styleSetting.get(), styleConstants));
    }

    if (styles.size() > 0) {
        AddElementCustomizationRules(targetStringSetting.get(),
                                     std::move(styles));
    }

    return true;
}

#ifdef WIN11_TASKBAR_WIN10_MULTIROW_COMBINED
void ProcessSingleModDefaultTargetStyles(const StyleConstants& styleConstants) {
    static const std::vector<ThemeTargetStyles> defaultTargetStyles = {
        ThemeTargetStyles{
            L"Taskbar.TaskListLabeledButtonPanel@RunningIndicatorStates > "
            L"Border#BackgroundElement",
            {
                L"CornerRadius=0",
                L"BorderThickness@ActiveRunningIndicator=0,0,0,3",
                L"BorderThickness@InactiveRunningIndicator=0,0,0,3",
                L"BorderThickness@InactiveRunningIndicatorPointerOver=0,0,0,3",
                L"BorderBrush@ActiveRunningIndicator:=<SolidColorBrush "
                L"Color=\"{ThemeResource SystemAccentColorLight2}\" />",
                L"BorderBrush@InactiveRunningIndicator:=<SolidColorBrush "
                L"Color=\"{ThemeResource SystemAccentColorLight2}\" />",
                L"BorderBrush@InactiveRunningIndicatorPointerOver:=<SolidColorBrush "
                L"Color=\"{ThemeResource SystemAccentColorLight2}\" />",
            }},
        ThemeTargetStyles{
            L"Taskbar.TaskListLabeledButtonPanel@RunningIndicatorStates > "
            L"Rectangle#RunningIndicator",
            {
                L"Opacity=0",
                L"Height=0",
                L"Width=0",
                L"Opacity@ActiveRunningIndicator=0",
                L"Opacity@InactiveRunningIndicator=0",
                L"Opacity@InactiveRunningIndicatorPointerOver=0",
                L"Height@ActiveRunningIndicator=0",
                L"Height@InactiveRunningIndicator=0",
                L"Height@InactiveRunningIndicatorPointerOver=0",
            }},
    };

    for (const auto& targetStyle : defaultTargetStyles) {
        try {
            std::vector<std::wstring> styles;
            styles.reserve(targetStyle.styles.size());
            for (const auto& style : targetStyle.styles) {
                styles.push_back(ApplyStyleConstants(style, styleConstants));
            }

            AddElementCustomizationRules(targetStyle.target, std::move(styles));
        } catch (winrt::hresult_error const& ex) {
            Wh_Log(L"Error %08X", ex.code());
        } catch (std::exception const& ex) {
            Wh_Log(L"Error: %S", ex.what());
        }
    }
}
#endif

std::optional<ResourceVariableEntry> ParseResourceVariable(
    std::wstring_view entry,
    const StyleConstants& styleConstants) {
    // Skip if commented.
    if (entry.starts_with(L"//")) {
        return std::nullopt;
    }

    // Find the first '=' to split key and value.
    auto eqPos = entry.find(L'=');
    if (eqPos == entry.npos) {
        Wh_Log(L"Skipping entry with no '=': %.*s",
               static_cast<int>(entry.length()), entry.data());
        return std::nullopt;
    }

    auto keyPart = TrimStringView(entry.substr(0, eqPos));
    auto valueRaw = TrimStringView(entry.substr(eqPos + 1));
    auto value = ApplyStyleConstants(valueRaw, styleConstants);

    constexpr std::wstring_view kThemeResourcePrefix = L"{ThemeResource ";

    ResourceVariableType type = ResourceVariableType::String;
    if (keyPart.size() > 0 && keyPart.back() == L':') {
        type = ResourceVariableType::Xaml;
        keyPart = keyPart.substr(0, keyPart.size() - 1);
        keyPart = TrimStringView(keyPart);
    } else if (value.starts_with(kThemeResourcePrefix) &&
               value.ends_with(L"}")) {
        type = ResourceVariableType::ThemeResourceReference;
        value = TrimStringView(
            value.substr(kThemeResourcePrefix.size(),
                         value.size() - kThemeResourcePrefix.size() - 1));
    }

    ResourceVariableTheme theme = ResourceVariableTheme::None;
    std::wstring key;

    // Check for @theme suffix in key part.
    auto atPos = keyPart.find(L'@');
    if (atPos != keyPart.npos) {
        key = TrimStringView(keyPart.substr(0, atPos));
        auto themePart = TrimStringView(keyPart.substr(atPos + 1));
        if (themePart == L"Dark") {
            theme = ResourceVariableTheme::Dark;
        } else if (themePart == L"Light") {
            theme = ResourceVariableTheme::Light;
        } else {
            Wh_Log(L"Unknown theme '%.*s', expected 'Dark' or 'Light'",
                   static_cast<int>(themePart.size()), themePart.data());
            return std::nullopt;
        }
    } else {
        key = std::wstring(keyPart);
    }

    return ResourceVariableEntry{std::move(key), std::move(value), theme, type};
}

winrt::Windows::Foundation::IInspectable ParseXamlValue(
    std::wstring_view xamlValue) {
    std::wstring xaml;
    xaml += L"        <Setter Property=\"Tag\">\n";
    xaml += L"            <Setter.Value>\n";
    xaml += xamlValue;
    xaml += L"\n";
    xaml += L"            </Setter.Value>\n";
    xaml += L"        </Setter>\n";

    auto style = GetStyleFromXamlSetters(L"FrameworkElement", xaml);
    return style.Setters().GetAt(0).as<Setter>().Value();
}

bool ProcessResourceVariable(ResourceDictionary resources,
                             ResourceDictionary darkDict,
                             ResourceDictionary lightDict,
                             const ResourceVariableEntry& entry) {
    auto boxedKey = winrt::box_value(entry.key);

    if (entry.theme != ResourceVariableTheme::None) {
        ResourceDictionary& targetDict =
            entry.theme == ResourceVariableTheme::Dark ? darkDict : lightDict;

        if (targetDict.HasKey(boxedKey)) {
            Wh_Log(
                L"Resource variable key '%s' already exists in theme '%s', "
                L"skipping",
                entry.key.c_str(),
                entry.theme == ResourceVariableTheme::Dark ? L"Dark"
                                                           : L"Light");
            return false;
        }

        winrt::Windows::Foundation::IInspectable value;
        switch (entry.type) {
            case ResourceVariableType::String:
                value = winrt::box_value(entry.value);
                break;
            case ResourceVariableType::Xaml:
                value =
                    entry.value.empty() ? nullptr : ParseXamlValue(entry.value);
                break;
            case ResourceVariableType::ThemeResourceReference:
                value = resources.Lookup(winrt::box_value(entry.value));
                break;
        }

        targetDict.Insert(boxedKey, value);

        return true;
    }

    // key= - convert using existing resource type.
    auto existingResource = resources.TryLookup(boxedKey);
    if (!existingResource) {
        Wh_Log(L"Resource variable key '%s' not found, skipping",
               entry.key.c_str());
        return false;
    }

    auto [it, inserted] =
        g_originalResourceValues.try_emplace(entry.key, existingResource);
    if (!inserted) {
        Wh_Log(L"Resource variable key '%s' already modified, skipping",
               entry.key.c_str());
        return false;
    }

    winrt::Windows::Foundation::IInspectable value;
    switch (entry.type) {
        case ResourceVariableType::String: {
            auto resourceClassName = winrt::get_class_name(existingResource);

            // Unwrap IReference<T> to get inner type name.
            if (resourceClassName.starts_with(
                    L"Windows.Foundation.IReference`1<") &&
                resourceClassName.ends_with(L'>')) {
                size_t prefixSize =
                    sizeof("Windows.Foundation.IReference`1<") - 1;
                resourceClassName =
                    winrt::hstring(resourceClassName.data() + prefixSize,
                                   resourceClassName.size() - prefixSize - 1);
            }

            value = Markup::XamlBindingHelper::ConvertValue(
                Interop::TypeName{resourceClassName},
                winrt::box_value(entry.value));
            break;
        }

        case ResourceVariableType::Xaml:
            value = entry.value.empty() ? nullptr : ParseXamlValue(entry.value);
            break;

        case ResourceVariableType::ThemeResourceReference:
            value = resources.Lookup(winrt::box_value(entry.value));
            break;
    }

    resources.Insert(boxedKey, value);

    return true;
}

void RefreshThemeResourceEntries() {
    if (g_resourceVariables.empty()) {
        return;
    }

    Wh_Log(L"Refreshing theme resource entries");

    auto resources = Application::Current().Resources();

    auto darkDict = g_resourceVariablesThemeDict.ThemeDictionaries()
                        .TryLookup(winrt::box_value(L"Dark"))
                        .try_as<ResourceDictionary>();
    auto lightDict = g_resourceVariablesThemeDict.ThemeDictionaries()
                         .TryLookup(winrt::box_value(L"Light"))
                         .try_as<ResourceDictionary>();

    for (const auto& entry : g_resourceVariables) {
        if (entry.type != ResourceVariableType::ThemeResourceReference) {
            continue;
        }

        try {
            auto boxedKey = winrt::box_value(entry.key);
            auto value = resources.Lookup(winrt::box_value(entry.value));

            if (entry.theme == ResourceVariableTheme::Dark && darkDict) {
                darkDict.Insert(boxedKey, value);
            } else if (entry.theme == ResourceVariableTheme::Light &&
                       lightDict) {
                lightDict.Insert(boxedKey, value);
            } else {
                resources.Insert(boxedKey, value);
            }
        } catch (winrt::hresult_error const& ex) {
            Wh_Log(L"Error refreshing '%s': %08X", entry.key.c_str(),
                   ex.code());
        }
    }
}

std::vector<ResourceVariableEntry> ProcessResourceVariablesFromSettings(
    const StyleConstants& styleConstants,
    const std::vector<PCWSTR>& themeResourceVariables) {
    std::vector<ResourceVariableEntry> resourceVariables;

    for (const auto& themeResourceVariable : themeResourceVariables) {
        Wh_Log(L"Processing theme resource variable %s", themeResourceVariable);

        auto parsed =
            ParseResourceVariable(themeResourceVariable, styleConstants);
        if (parsed) {
            resourceVariables.push_back(std::move(*parsed));
        }
    }

#ifndef WIN11_TASKBAR_WIN10_MULTIROW_COMBINED
    for (int i = 0;; i++) {
        string_setting_unique_ptr setting(
            Wh_GetStringSetting(L"themeResourceVariables[%d]", i));
        if (!*setting.get()) {
            break;
        }

        Wh_Log(L"Processing resource variable %s", setting.get());

        auto parsed = ParseResourceVariable(setting.get(), styleConstants);
        if (parsed) {
            resourceVariables.push_back(std::move(*parsed));
        }
    }
#endif

    return resourceVariables;
}

void MergeResourceVariables() {
    auto resources = Application::Current().Resources();

    // Create theme dictionaries for @Dark/@Light resources.
    g_resourceVariablesThemeDict = ResourceDictionary();
    ResourceDictionary darkDict;
    ResourceDictionary lightDict;
    bool hasThemeResources = false;
    bool hasThemeResourceReferences = false;

    for (auto it = g_resourceVariables.rbegin();
         it != g_resourceVariables.rend(); ++it) {
        Wh_Log(L"Processing resource variable %s", it->key.c_str());

        try {
            if (ProcessResourceVariable(resources, darkDict, lightDict, *it)) {
                if (it->theme != ResourceVariableTheme::None) {
                    hasThemeResources = true;
                }

                if (it->type == ResourceVariableType::ThemeResourceReference) {
                    hasThemeResourceReferences = true;
                }
            }
        } catch (winrt::hresult_error const& ex) {
            Wh_Log(L"Error %08X: %s", ex.code(), ex.message().c_str());
        } catch (std::exception const& ex) {
            Wh_Log(L"Error: %S", ex.what());
        }
    }

    if (hasThemeResources) {
        g_resourceVariablesThemeDict.ThemeDictionaries().Insert(
            winrt::box_value(L"Dark"), darkDict);
        g_resourceVariablesThemeDict.ThemeDictionaries().Insert(
            winrt::box_value(L"Light"), lightDict);

        resources.MergedDictionaries().Append(g_resourceVariablesThemeDict);
    }

    // Register for color changes to refresh theme resource references.
    if (hasThemeResourceReferences) {
        g_uiSettings = winrt::Windows::UI::ViewManagement::UISettings();
        auto dispatcherQueue =
            winrt::Windows::System::DispatcherQueue::GetForCurrentThread();
        g_colorValuesChangedToken =
            g_uiSettings.ColorValuesChanged([dispatcherQueue](auto&&, auto&&) {
                dispatcherQueue.TryEnqueue(RefreshThemeResourceEntries);
            });
    }
}

std::optional<bool> IsOsFeatureEnabled(UINT32 featureId) {
    enum FEATURE_ENABLED_STATE {
        FEATURE_ENABLED_STATE_DEFAULT = 0,
        FEATURE_ENABLED_STATE_DISABLED = 1,
        FEATURE_ENABLED_STATE_ENABLED = 2,
    };

#pragma pack(push, 1)
    struct RTL_FEATURE_CONFIGURATION {
        unsigned int featureId;
        unsigned __int32 group : 4;
        FEATURE_ENABLED_STATE enabledState : 2;
        unsigned __int32 enabledStateOptions : 1;
        unsigned __int32 unused1 : 1;
        unsigned __int32 variant : 6;
        unsigned __int32 variantPayloadKind : 2;
        unsigned __int32 unused2 : 16;
        unsigned int payload;
    };
#pragma pack(pop)

    using RtlQueryFeatureConfiguration_t =
        int(NTAPI*)(UINT32, int, INT64*, RTL_FEATURE_CONFIGURATION*);
    static RtlQueryFeatureConfiguration_t pRtlQueryFeatureConfiguration = []() {
        HMODULE hNtDll = LoadLibraryW(L"ntdll.dll");
        return hNtDll ? (RtlQueryFeatureConfiguration_t)GetProcAddress(
                            hNtDll, "RtlQueryFeatureConfiguration")
                      : nullptr;
    }();

    if (!pRtlQueryFeatureConfiguration) {
        Wh_Log(L"RtlQueryFeatureConfiguration not found");
        return std::nullopt;
    }

    RTL_FEATURE_CONFIGURATION feature = {0};
    INT64 changeStamp = 0;
    HRESULT hr =
        pRtlQueryFeatureConfiguration(featureId, 1, &changeStamp, &feature);
    if (SUCCEEDED(hr)) {
        Wh_Log(L"RtlQueryFeatureConfiguration result for %u: %d", featureId,
               feature.enabledState);

        switch (feature.enabledState) {
            case FEATURE_ENABLED_STATE_DISABLED:
                return false;
            case FEATURE_ENABLED_STATE_ENABLED:
                return true;
            case FEATURE_ENABLED_STATE_DEFAULT:
                return std::nullopt;
        }
    } else {
        Wh_Log(L"RtlQueryFeatureConfiguration error for %u: %08X", featureId,
               hr);
    }

    return std::nullopt;
}

void ProcessAllStylesFromSettings() {
#ifdef WIN11_TASKBAR_WIN10_MULTIROW_COMBINED
    const Theme* theme = nullptr;
#else
    PCWSTR themeName = Wh_GetStringSetting(L"theme");
    const Theme* theme = nullptr;
    if (wcscmp(themeName, L"TranslucentTaskbar") == 0) {
        theme = &g_themeTranslucentTaskbar;
    } else if (wcscmp(themeName, L"DockLike") == 0) {
        theme = &g_themeDockLike;
    } else if (wcscmp(themeName, L"SimplyTransparent") == 0) {
        theme = &g_themeSimplyTransparent;
    } else if (wcscmp(themeName, L"Squircle") == 0) {
        // Weather widget on the right.
        // https://www.reddit.com/r/Windows11/comments/1dnew8x/my_weather_widget_is_on_the_right_side/
        constexpr UINT32 kExtendedModeAEPForTaskbar = 48660958;
        theme = IsOsFeatureEnabled(kExtendedModeAEPForTaskbar).value_or(false)
                    ? &g_themeSquircle_variant_WeatherOnTheRight
                    : &g_themeSquircle;
    } else if (wcscmp(themeName, L"Matter") == 0) {
        theme = &g_themeMatter;
    } else if (wcscmp(themeName, L"WinXP") == 0) {
        theme = &g_themeWinXP;
    } else if (wcscmp(themeName, L"WinXP_variant_Zune") == 0) {
        theme = &g_themeWinXP_variant_Zune;
    } else if (wcscmp(themeName, L"Bubbles") == 0) {
        theme = &g_themeBubbles;
    } else if (wcscmp(themeName, L"RosePine") == 0) {
        theme = &g_themeRosePine;
    } else if (wcscmp(themeName, L"WinVista") == 0) {
        theme = &g_themeWinVista;
    } else if (wcscmp(themeName, L"CleanSlate") == 0) {
        theme = &g_themeCleanSlate;
    } else if (wcscmp(themeName, L"Lucent") == 0) {
        theme = &g_themeLucent;
    } else if (wcscmp(themeName, L"Lucent_variant_Light") == 0) {
        theme = &g_themeLucent_variant_Light;
    } else if (wcscmp(themeName, L"SunValley") == 0) {
        theme = &g_themeSunValley;
    } else if (wcscmp(themeName, L"21996Taskbar") == 0) {
        theme = &g_theme21996Taskbar;
    } else if (wcscmp(themeName, L"BottomDensy") == 0) {
        theme = &g_themeBottomDensy;
    } else if (wcscmp(themeName, L"BottomDensy_variant_NoInd") == 0) {
        theme = &g_themeBottomDensy_variant_NoInd;
    } else if (wcscmp(themeName, L"TaskbarXII") == 0) {
        theme = &g_themeTaskbarXII;
    } else if (wcscmp(themeName, L"xdark") == 0) {
        theme = &g_themexdark;
    } else if (wcscmp(themeName, L"Windows7") == 0) {
        theme = &g_themeWindows7;
    } else if (wcscmp(themeName, L"Aeris") == 0) {
        theme = &g_themeAeris;
    } else if (wcscmp(themeName, L"Plasma") == 0) {
        theme = &g_themePlasma;
    } else if (wcscmp(themeName, L"WindowGlass") == 0) {
        theme = &g_themeWindowGlass;
    } else if (wcscmp(themeName, L"WindowGlass_variant_Split") == 0) {
        theme = &g_themeWindowGlass_variant_Split;
    } else if (wcscmp(themeName, L"WindowGlass_variant_FullLength") == 0) {
        theme = &g_themeWindowGlass_variant_FullLength;
    } else if (wcscmp(themeName, L"Surface") == 0) {
        theme = &g_themeSurface;
    } else if (wcscmp(themeName, L"Oversimplified&Accentuated") == 0) {
        theme = &g_themeOversimplified_Accentuated;
    } else if (wcscmp(themeName, L"Luminosity_variant_Dock") == 0) {
        theme = &g_themeLuminosity_variant_Dock;
    } else if (wcscmp(themeName, L"Luminosity_variant_Classic") == 0) {
        theme = &g_themeLuminosity_variant_Classic;
    } else if (wcscmp(themeName, L"Luminosity_variant_Compact") == 0) {
        theme = &g_themeLuminosity_variant_Compact;
    } else if (wcscmp(themeName, L"LayerMicaUI") == 0) {
        theme = &g_themeLayerMicaUI;
    } else if (wcscmp(themeName, L"Fluid") == 0) {
        theme = &g_themeFluid;
    } else if (wcscmp(themeName, L"TintedGlass") == 0) {
        theme = &g_themeTintedGlass;
    } else if (wcscmp(themeName, L"TaskbarToStatusbar") == 0) {
        theme = &g_themeTaskbarToStatusbar;
    } else if (wcscmp(themeName, L"UltraWideFriendly") == 0) {
        theme = &g_themeUltraWideFriendly;
    } else if (wcscmp(themeName, L"LiquidGlass") == 0) {
        theme = &g_themeLiquidGlass;
    } else if (wcscmp(themeName, L"LiquidGlass_variant_Alternate") == 0) {
        theme = &g_themeLiquidGlass_variant_Alternate;
    } else if (wcscmp(themeName, L"Borderless") == 0) {
        theme = &g_themeBorderless;
    }
    Wh_FreeStringSetting(themeName);
#endif

    StyleConstants styleConstants = LoadStyleConstants(
        theme ? theme->styleConstants : std::vector<PCWSTR>{});

    if (theme) {
        for (const auto& themeTargetStyle : theme->targetStyles) {
            try {
                std::vector<std::wstring> styles;
                styles.reserve(themeTargetStyle.styles.size());
                for (const auto& s : themeTargetStyle.styles) {
                    styles.push_back(ApplyStyleConstants(s, styleConstants));
                }

                AddElementCustomizationRules(themeTargetStyle.target,
                                             std::move(styles));
            } catch (winrt::hresult_error const& ex) {
                Wh_Log(L"Error %08X", ex.code());
            } catch (std::exception const& ex) {
                Wh_Log(L"Error: %S", ex.what());
            }
        }
    }

#ifdef WIN11_TASKBAR_WIN10_MULTIROW_COMBINED
    ProcessSingleModDefaultTargetStyles(styleConstants);
#endif

#ifndef WIN11_TASKBAR_WIN10_MULTIROW_COMBINED
    for (int i = 0;; i++) {
        try {
            if (!ProcessSingleTargetStylesFromSettings(i, styleConstants)) {
                break;
            }
        } catch (winrt::hresult_error const& ex) {
            Wh_Log(L"Error %08X: %s", ex.code(), ex.message().c_str());
        } catch (std::exception const& ex) {
            Wh_Log(L"Error: %S", ex.what());
        }
    }
#endif

    g_resourceVariables = ProcessResourceVariablesFromSettings(
        styleConstants,
        theme ? theme->themeResourceVariables : std::vector<PCWSTR>{});
}

void UninitializeResourceVariables() {
    // Unregister color change handler.
    if (g_colorValuesChangedToken) {
        g_uiSettings.ColorValuesChanged(g_colorValuesChangedToken);
        g_colorValuesChangedToken = {};
    }
    g_uiSettings = nullptr;
    g_resourceVariables.clear();

    // Restore original resource values.
    auto resources = Application::Current().Resources();
    for (const auto& [key, originalValue] : g_originalResourceValues) {
        try {
            resources.Insert(winrt::box_value(key), originalValue);
        } catch (...) {
            HRESULT hr = winrt::to_hresult();
            Wh_Log(L"Error %08X", hr);
        }
    }
    g_originalResourceValues.clear();

    // Remove our merged theme dictionary.
    if (g_resourceVariablesThemeDict) {
        auto merged = resources.MergedDictionaries();
        uint32_t index;
        if (merged.IndexOf(g_resourceVariablesThemeDict, index)) {
            merged.RemoveAt(index);
        }
        g_resourceVariablesThemeDict = nullptr;
    }
}

void UninitializeForCurrentThread() {
    UnhookFirstTaskbarFrameLayoutWorkaround();

    // Clear failed image brushes list for this thread (revokers will
    // automatically unregister).
    g_failedImageBrushesForThread.failedImageBrushes.clear();
    g_failedImageBrushesForThread.dispatcher = nullptr;

    for (const auto& [elementDo, asyncOp] : g_delayedBackgroundFillSet) {
        asyncOp.Cancel();
    }

    g_delayedBackgroundFillSet.clear();

    for (const auto& [handle, elementCustomizationState] :
         g_elementsCustomizationState) {
        auto element = elementCustomizationState.element.get();
        auto* state = GetStyleVariableState(elementCustomizationState.xamlRoot);

        RestoreCapturesForElement(element, elementCustomizationState);

        for (const auto& [visualStateGroupOptionalWeakPtrIter, stateIter] :
             elementCustomizationState.perVisualStateGroup) {
            RestoreCustomizationsForVisualStateGroup(
                state, handle, element, visualStateGroupOptionalWeakPtrIter,
                stateIter);
        }
    }

    g_elementsCustomizationState.clear();
    g_styleVariableState.clear();

    g_elementsCustomizationRules.clear();

    UninitializeResourceVariables();

    g_initializedForThread = false;
}

void UninitializeSettingsAndTap() {
    if (g_visualTreeWatcher) {
        g_visualTreeWatcher->UnadviseVisualTreeChange();
        g_visualTreeWatcher = nullptr;
    }

    g_initialized = false;
}

void InitializeForCurrentThread() {
    if (g_initializedForThread) {
        return;
    }

    ProcessAllStylesFromSettings();

    g_initializedForThread = true;
}

void InitializeSettingsAndTap() {
    if (g_initialized.exchange(true)) {
        return;
    }

    HRESULT hr = InjectWindhawkTAP();
    if (FAILED(hr)) {
        Wh_Log(L"Error %08X", hr);
    }
}

using RunFromWindowThreadProc_t = void(WINAPI*)(PVOID parameter);

bool RunFromWindowThread(HWND hWnd,
                         RunFromWindowThreadProc_t proc,
                         PVOID procParam) {
    static const UINT runFromWindowThreadRegisteredMsg =
        RegisterWindowMessage(L"Windhawk_RunFromWindowThread_" WH_MOD_ID);

    struct RUN_FROM_WINDOW_THREAD_PARAM {
        RunFromWindowThreadProc_t proc;
        PVOID procParam;
    };

    DWORD dwThreadId = GetWindowThreadProcessId(hWnd, nullptr);
    if (dwThreadId == 0) {
        return false;
    }

    if (dwThreadId == GetCurrentThreadId()) {
        proc(procParam);
        return true;
    }

    HHOOK hook = SetWindowsHookEx(
        WH_CALLWNDPROC,
        [](int nCode, WPARAM wParam, LPARAM lParam) -> LRESULT {
            if (nCode == HC_ACTION) {
                const CWPSTRUCT* cwp = (const CWPSTRUCT*)lParam;
                if (cwp->message == runFromWindowThreadRegisteredMsg) {
                    RUN_FROM_WINDOW_THREAD_PARAM* param =
                        (RUN_FROM_WINDOW_THREAD_PARAM*)cwp->lParam;
                    param->proc(param->procParam);
                }
            }

            return CallNextHookEx(nullptr, nCode, wParam, lParam);
        },
        nullptr, dwThreadId);
    if (!hook) {
        return false;
    }

    RUN_FROM_WINDOW_THREAD_PARAM param;
    param.proc = proc;
    param.procParam = procParam;
    SendMessage(hWnd, runFromWindowThreadRegisteredMsg, 0, (LPARAM)&param);

    UnhookWindowsHookEx(hook);

    return true;
}

void OnWindowCreated(HWND hWnd,
                     HWND hWndParent,
                     LPCWSTR lpClassName,
                     PCSTR funcName) {
    BOOL bTextualClassName = ((ULONG_PTR)lpClassName & ~(ULONG_PTR)0xffff) != 0;

    WCHAR className[64];
    if (hWndParent && GetClassName(hWnd, className, ARRAYSIZE(className)) &&
        _wcsicmp(className,
                 L"Windows.UI.Composition.DesktopWindowContentBridge") == 0 &&
        GetClassName(hWndParent, className, ARRAYSIZE(className)) &&
        _wcsicmp(className, L"Shell_TrayWnd") == 0) {
        Wh_Log(L"Initializing - Created DesktopWindowContentBridge window");
        InitializeForCurrentThread();
        InitializeSettingsAndTap();
        return;
    }

    if (bTextualClassName &&
        (_wcsicmp(lpClassName, L"XamlExplorerHostIslandWindow") == 0 ||
         _wcsicmp(lpClassName, L"Shell_InputSwitchTopLevelWindow") == 0
#ifdef SEARCHHOST_STYLER_PROBE
         ||
         _wcsicmp(lpClassName, L"Windows.UI.Core.CoreWindow") == 0
#endif
             )) {
        Wh_Log(L"Initializing - Created XAML host window: %08X via %S",
               (DWORD)(ULONG_PTR)hWnd, funcName);
        InitializeForCurrentThread();
        InitializeSettingsAndTap();
        return;
    }
}

using CreateWindowExW_t = decltype(&CreateWindowExW);
CreateWindowExW_t CreateWindowExW_Original;
HWND WINAPI CreateWindowExW_Hook(DWORD dwExStyle,
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
                                 PVOID lpParam) {
    HWND hWnd = CreateWindowExW_Original(dwExStyle, lpClassName, lpWindowName,
                                         dwStyle, X, Y, nWidth, nHeight,
                                         hWndParent, hMenu, hInstance, lpParam);
    if (!hWnd) {
        return hWnd;
    }

    OnWindowCreated(hWnd, hWndParent, lpClassName, __FUNCTION__);

    return hWnd;
}

using CreateWindowInBand_t = HWND(WINAPI*)(DWORD dwExStyle,
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
                                           PVOID lpParam,
                                           DWORD dwBand);
CreateWindowInBand_t CreateWindowInBand_Original;
HWND WINAPI CreateWindowInBand_Hook(DWORD dwExStyle,
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
                                    PVOID lpParam,
                                    DWORD dwBand) {
    HWND hWnd = CreateWindowInBand_Original(
        dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight,
        hWndParent, hMenu, hInstance, lpParam, dwBand);
    if (!hWnd) {
        return hWnd;
    }

    OnWindowCreated(hWnd, hWndParent, lpClassName, __FUNCTION__);

    return hWnd;
}

using CreateWindowInBandEx_t = HWND(WINAPI*)(DWORD dwExStyle,
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
                                             PVOID lpParam,
                                             DWORD dwBand,
                                             DWORD dwTypeFlags);
CreateWindowInBandEx_t CreateWindowInBandEx_Original;
HWND WINAPI CreateWindowInBandEx_Hook(DWORD dwExStyle,
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
                                      PVOID lpParam,
                                      DWORD dwBand,
                                      DWORD dwTypeFlags) {
    HWND hWnd = CreateWindowInBandEx_Original(
        dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight,
        hWndParent, hMenu, hInstance, lpParam, dwBand, dwTypeFlags);
    if (!hWnd) {
        return hWnd;
    }

    OnWindowCreated(hWnd, hWndParent, lpClassName, __FUNCTION__);

    return hWnd;
}

PFN_INITIALIZE_XAML_DIAGNOSTICS_EX InitializeXamlDiagnosticsEx_Original;
HRESULT WINAPI
InitializeXamlDiagnosticsEx_Hook(_In_ PCWSTR endPointName,
                                 _In_ DWORD pid,
                                 _In_ PCWSTR wszDllXamlDiagnostics,
                                 _In_ PCWSTR wszTAPDllName,
                                 _In_ CLSID tapClsid,
                                 _In_opt_ PCWSTR wszInitializationData) {
    if (g_inInjectWindhawkTAP) {
        return InitializeXamlDiagnosticsEx_Original(
            endPointName, pid, wszDllXamlDiagnostics, wszTAPDllName, tapClsid,
            wszInitializationData);
    }

    bool blockCall = false;

    switch (g_settings.xamlDiagnosticsHandling) {
        case XamlDiagnosticsHandling::kAlert: {
            void* retAddress = __builtin_return_address(0);

            WCHAR modulePath[MAX_PATH];
            PCWSTR modulePathStr = L"<unknown>";
            HMODULE module;
            if (GetModuleHandleEx(
                    GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                        GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                    reinterpret_cast<LPCWSTR>(retAddress), &module)) {
                switch (GetModuleFileName(module, modulePath,
                                          ARRAYSIZE(modulePath))) {
                    case 0:
                    case ARRAYSIZE(modulePath):
                        break;

                    default:
                        modulePathStr = modulePath;
                        break;
                }
            }

            WCHAR message[1024];
            _snwprintf_s(
                message, _TRUNCATE,
                L"The following module is trying to use XAML diagnostics:\n\n"
                L"%s\n\n"
                L"There can only be one consumer at a time. Blocking it might "
                L"break that module, but allowing it might break this mod.\n\n"
                L"Do you want to block it?\n\n"
                L"Note: You can change this behavior in the mod settings.",
                modulePathStr);
            int result = MessageBox(nullptr, message,
                                    L"Windows 11 Taskbar Styler - Windhawk",
                                    MB_YESNO | MB_ICONQUESTION | MB_TOPMOST);
            blockCall = (result == IDYES);
            break;
        }

        case XamlDiagnosticsHandling::kBlock:
            blockCall = true;
            break;

        case XamlDiagnosticsHandling::kAllow:
            blockCall = false;
            break;
    }

    if (blockCall) {
        Wh_Log(L"Blocking InitializeXamlDiagnosticsEx call");
        // Return success to avoid exception in the caller.
        return S_OK;
    }

    Wh_Log(L"Allowing InitializeXamlDiagnosticsEx call");
    return InitializeXamlDiagnosticsEx_Original(
        endPointName, pid, wszDllXamlDiagnostics, wszTAPDllName, tapClsid,
        wszInitializationData);
}

void HookInitializeXamlDiagnosticsExIfNeeded() {
    if (InitializeXamlDiagnosticsEx_Original) {
        return;  // Already hooked
    }

    const HMODULE wux = GetModuleHandle(L"Windows.UI.Xaml.dll");
    if (!wux) {
        return;  // DLL not loaded yet
    }

    const auto ixde = reinterpret_cast<PFN_INITIALIZE_XAML_DIAGNOSTICS_EX>(
        GetProcAddress(wux, "InitializeXamlDiagnosticsEx"));
    if (!ixde) {
        return;
    }

    Wh_Log(L"Hooking InitializeXamlDiagnosticsEx to handle other consumers");
    WindhawkUtils::SetFunctionHook(ixde, InitializeXamlDiagnosticsEx_Hook,
                                   &InitializeXamlDiagnosticsEx_Original);
    Wh_ApplyHookOperations();
}

using LoadLibraryExW_t = decltype(&LoadLibraryExW);
LoadLibraryExW_t LoadLibraryExW_Original;
HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR lpLibFileName,
                                   HANDLE hFile,
                                   DWORD dwFlags) {
    HMODULE module = LoadLibraryExW_Original(lpLibFileName, hFile, dwFlags);

    if (module && !InitializeXamlDiagnosticsEx_Original && lpLibFileName) {
        PCWSTR fileName = wcsrchr(lpLibFileName, L'\\');
        fileName = fileName ? fileName + 1 : lpLibFileName;
        if (_wcsicmp(fileName, L"Windows.UI.Xaml.dll") == 0) {
            HookInitializeXamlDiagnosticsExIfNeeded();
        }
    }

    return module;
}

std::vector<HWND> GetXamlHostWnds() {
    struct ENUM_WINDOWS_PARAM {
        std::vector<HWND>* hWnds;
    };

    std::vector<HWND> hWnds;
    ENUM_WINDOWS_PARAM param = {&hWnds};
    EnumWindows(
        [](HWND hWnd, LPARAM lParam) -> BOOL {
            ENUM_WINDOWS_PARAM& param = *(ENUM_WINDOWS_PARAM*)lParam;

            DWORD dwProcessId = 0;
            if (!GetWindowThreadProcessId(hWnd, &dwProcessId) ||
                dwProcessId != GetCurrentProcessId()) {
                return TRUE;
            }

            WCHAR szClassName[64];
            if (GetClassName(hWnd, szClassName, ARRAYSIZE(szClassName)) == 0) {
                return TRUE;
            }

            if (_wcsicmp(szClassName, L"XamlExplorerHostIslandWindow") == 0 ||
                _wcsicmp(szClassName, L"Shell_InputSwitchTopLevelWindow") ==
                    0
#ifdef SEARCHHOST_STYLER_PROBE
                || _wcsicmp(szClassName, L"Windows.UI.Core.CoreWindow") == 0
#endif
                    ) {
                param.hWnds->push_back(hWnd);
            }

            return TRUE;
        },
        (LPARAM)&param);

    return hWnds;
}

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

HWND GetTaskbarUiWnd() {
    HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();
    if (!hTaskbarWnd) {
        return nullptr;
    }

    return FindWindowEx(hTaskbarWnd, nullptr,
                        L"Windows.UI.Composition.DesktopWindowContentBridge",
                        nullptr);
}

PTP_TIMER g_statsTimer;

bool StartStatsTimer() {
    static constexpr WCHAR kStatsBaseUrl[] =
        L"https://github.com/ramensoftware/"
        L"windows-11-taskbar-styling-guide/"
        L"releases/download/stats-v4/";

    ULONGLONG lastStatsTime = 0;
    Wh_GetBinaryValue(L"statsTimerLastTime", &lastStatsTime,
                      sizeof(lastStatsTime));

    // -1 can be set for disabling the stats timer.
    if (lastStatsTime == 0xFFFFFFFF'FFFFFFFF) {
        return false;
    }

    FILETIME currentTimeFt;
    GetSystemTimeAsFileTime(&currentTimeFt);

    ULONGLONG currentTime = ((ULONGLONG)currentTimeFt.dwHighDateTime << 32) |
                            currentTimeFt.dwLowDateTime;

    constexpr ULONGLONG k10Minutes = 10 * 60 * 10000000LL;
    constexpr ULONGLONG k24Hours = 24 * 60 * 60 * 10000000LL;

    ULONGLONG minDueTime = currentTime + k10Minutes;
    ULONGLONG maxDueTime = currentTime + k24Hours;

    ULONGLONG dueTime = k24Hours - (currentTime - lastStatsTime);
    if (dueTime < minDueTime) {
        dueTime = minDueTime;
    } else if (dueTime > maxDueTime) {
        dueTime = maxDueTime;
    }

    g_statsTimer = CreateThreadpoolTimer(
        [](PTP_CALLBACK_INSTANCE, PVOID, PTP_TIMER) {
            Wh_Log(L">");

            string_setting_unique_ptr themeName(Wh_GetStringSetting(L"theme"));
            if (!*themeName.get()) {
                return;
            }

            HANDLE mutex =
                CreateMutex(nullptr, FALSE, L"WindhawkStats_" WH_MOD_ID);
            if (mutex) {
                WaitForSingleObject(mutex, INFINITE);
            }

            ULONGLONG lastStatsTime = 0;
            Wh_GetBinaryValue(L"statsTimerLastTime", &lastStatsTime,
                              sizeof(lastStatsTime));

            FILETIME currentTimeFt;
            GetSystemTimeAsFileTime(&currentTimeFt);
            ULONGLONG currentTime =
                ((ULONGLONG)currentTimeFt.dwHighDateTime << 32) |
                currentTimeFt.dwLowDateTime;

            const WH_URL_CONTENT* content = nullptr;
            if (currentTime - lastStatsTime >= k10Minutes) {
                Wh_SetBinaryValue(L"statsTimerLastTime", &currentTime,
                                  sizeof(currentTime));

                std::wstring themeNameEscaped = themeName.get();
                std::replace(themeNameEscaped.begin(), themeNameEscaped.end(),
                             L' ', L'_');
                std::replace(themeNameEscaped.begin(), themeNameEscaped.end(),
                             L'&', L'_');

                std::wstring statsUrl = kStatsBaseUrl;
                statsUrl += themeNameEscaped;
                statsUrl += L".txt";

                Wh_Log(L"Submitting stats to %s", statsUrl.c_str());

                content = Wh_GetUrlContent(statsUrl.c_str(), nullptr);
            } else {
                Wh_Log(L"Skipping, last submission %llu seconds ago",
                       (currentTime - lastStatsTime) / 10000000LL);
            }

            if (mutex) {
                ReleaseMutex(mutex);
                CloseHandle(mutex);
            }

            if (!content) {
                Wh_Log(L"Failed to get stats content");
                return;
            }

            if (content->statusCode != 200) {
                Wh_Log(L"Stats content status code: %d", content->statusCode);
            }

            Wh_FreeUrlContent(content);
            Wh_Log(L"Stats content submitted");
        },
        nullptr, nullptr);
    if (!g_statsTimer) {
        Wh_Log(L"Failed to create stats timer");
        return false;
    }

    constexpr DWORD k24HoursInMs = 24 * 60 * 60 * 1000;
    constexpr ULONGLONG k10MinutesInMs = 10 * 60 * 1000;

    FILETIME dueTimeFt;
    dueTimeFt.dwLowDateTime = (DWORD)(dueTime & 0xFFFFFFFF);
    dueTimeFt.dwHighDateTime = (DWORD)(dueTime >> 32);
    SetThreadpoolTimer(g_statsTimer, &dueTimeFt, k24HoursInMs, k10MinutesInMs);
    return true;
}

void StopStatsTimer() {
    if (g_statsTimer) {
        SetThreadpoolTimer(g_statsTimer, nullptr, 0, 0);
        WaitForThreadpoolTimerCallbacks(g_statsTimer, TRUE);
        CloseThreadpoolTimer(g_statsTimer);
        g_statsTimer = nullptr;
    }
}

void LoadSettings() {
#ifdef WIN11_TASKBAR_WIN10_MULTIROW_COMBINED
    g_settings.xamlDiagnosticsHandling = XamlDiagnosticsHandling::kBlock;
#else
    PCWSTR xamlDiagnosticsHandling =
        Wh_GetStringSetting(L"xamlDiagnosticsHandling");
    g_settings.xamlDiagnosticsHandling = XamlDiagnosticsHandling::kAlert;
    if (wcscmp(xamlDiagnosticsHandling, L"block") == 0) {
        g_settings.xamlDiagnosticsHandling = XamlDiagnosticsHandling::kBlock;
    } else if (wcscmp(xamlDiagnosticsHandling, L"allow") == 0) {
        g_settings.xamlDiagnosticsHandling = XamlDiagnosticsHandling::kAllow;
    }
    Wh_FreeStringSetting(xamlDiagnosticsHandling);
#endif
}

BOOL Wh_ModInit() {
    Wh_Log(L">");

    LoadSettings();

    WindhawkUtils::SetFunctionHook(CreateWindowExW, CreateWindowExW_Hook,
                                   &CreateWindowExW_Original);

    HMODULE user32Module =
        LoadLibraryEx(L"user32.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (user32Module) {
        auto pCreateWindowInBand = (CreateWindowInBand_t)GetProcAddress(
            user32Module, "CreateWindowInBand");
        if (pCreateWindowInBand) {
            WindhawkUtils::SetFunctionHook(pCreateWindowInBand,
                                           CreateWindowInBand_Hook,
                                           &CreateWindowInBand_Original);
        }

        auto pCreateWindowInBandEx = (CreateWindowInBandEx_t)GetProcAddress(
            user32Module, "CreateWindowInBandEx");
        if (pCreateWindowInBandEx) {
            WindhawkUtils::SetFunctionHook(pCreateWindowInBandEx,
                                           CreateWindowInBandEx_Hook,
                                           &CreateWindowInBandEx_Original);
        }
    }

    HMODULE kernelBaseModule = GetModuleHandle(L"kernelbase.dll");
    auto pKernelBaseLoadLibraryExW = (decltype(&LoadLibraryExW))GetProcAddress(
        kernelBaseModule, "LoadLibraryExW");
    WindhawkUtils::SetFunctionHook(pKernelBaseLoadLibraryExW,
                                   LoadLibraryExW_Hook,
                                   &LoadLibraryExW_Original);

    // Hook immediately if DLL is already loaded.
    HookInitializeXamlDiagnosticsExIfNeeded();

    StartStatsTimer();

    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L">");

    bool initialize = false;

    HWND hTaskbarUiWnd = GetTaskbarUiWnd();
    if (hTaskbarUiWnd) {
        Wh_Log(L"Initializing - Found DesktopWindowContentBridge window");
        RunFromWindowThread(
            hTaskbarUiWnd, [](PVOID) { InitializeForCurrentThread(); },
            nullptr);
        initialize = true;
    }

    for (auto hXamlHostWnd : GetXamlHostWnds()) {
        Wh_Log(L"Initializing for %08X", (DWORD)(ULONG_PTR)hXamlHostWnd);
        RunFromWindowThread(
            hXamlHostWnd, [](PVOID) { InitializeForCurrentThread(); }, nullptr);
        initialize = true;
    }

    if (initialize) {
        InitializeSettingsAndTap();
    }
}

void Wh_ModUninit() {
    Wh_Log(L">");

    HWND restartExplorerPromptWindow = g_restartExplorerPromptWindow;
    if (restartExplorerPromptWindow) {
        PostMessage(restartExplorerPromptWindow, WM_CLOSE, 0, 0);
    }

    if (g_restartExplorerPromptThread) {
        WaitForSingleObject(g_restartExplorerPromptThread, INFINITE);
        CloseHandle(g_restartExplorerPromptThread);
        g_restartExplorerPromptThread = nullptr;
    }

    StopStatsTimer();

    UninitializeSettingsAndTap();

    HWND hTaskbarUiWnd = GetTaskbarUiWnd();
    if (hTaskbarUiWnd) {
        Wh_Log(L"Uninitializing - Found DesktopWindowContentBridge window");
        RunFromWindowThread(
            hTaskbarUiWnd, [](PVOID) { UninitializeForCurrentThread(); },
            nullptr);
    }

    for (auto hXamlHostWnd : GetXamlHostWnds()) {
        Wh_Log(L"Uninitializing for %08X", (DWORD)(ULONG_PTR)hXamlHostWnd);
        RunFromWindowThread(
            hXamlHostWnd, [](PVOID) { UninitializeForCurrentThread(); },
            nullptr);
    }

    // Unregister global network status change handler.
    if (g_networkStatusChangedToken) {
        try {
            winrt::Windows::Networking::Connectivity::NetworkInformation::
                NetworkStatusChanged(g_networkStatusChangedToken);
            Wh_Log(L"Unregistered global network status change handler");
        } catch (winrt::hresult_error const& ex) {
            Wh_Log(L"Error unregistering network status handler %08X: %s",
                   ex.code(), ex.message().c_str());
        }
        g_networkStatusChangedToken = {};
    }

    // Clear the dispatcher registry.
    {
        std::lock_guard<std::mutex> lock(g_failedImageBrushesRegistryMutex);
        g_failedImageBrushesRegistry.clear();
    }
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");

    UninitializeSettingsAndTap();

    LoadSettings();

    bool initialize = false;

    HWND hTaskbarUiWnd = GetTaskbarUiWnd();
    if (hTaskbarUiWnd) {
        Wh_Log(L"Reinitializing - Found DesktopWindowContentBridge window");
        RunFromWindowThread(
            hTaskbarUiWnd,
            [](PVOID) {
                UninitializeForCurrentThread();
                InitializeForCurrentThread();
            },
            nullptr);
        initialize = true;
    }

    for (auto hXamlHostWnd : GetXamlHostWnds()) {
        Wh_Log(L"Reinitializing for %08X", (DWORD)(ULONG_PTR)hXamlHostWnd);
        RunFromWindowThread(
            hXamlHostWnd,
            [](PVOID) {
                UninitializeForCurrentThread();
                InitializeForCurrentThread();
            },
            nullptr);
        initialize = true;
    }

    if (initialize) {
        InitializeSettingsAndTap();
    }
}
// END INLINED COMPONENT: taskbar-styler
#undef Wh_ModSettingsChanged
#undef Wh_ModUninit
#undef Wh_ModAfterInit
#undef Wh_ModInit

bool g_singleModStylerUninitDone = false;

void SingleModRedrawTaskbarWindows() {
    HWND taskbar = FindWindowW(L"Shell_TrayWnd", nullptr);
    if (taskbar) {
        RedrawWindow(taskbar, nullptr, nullptr,
                     RDW_INVALIDATE | RDW_ALLCHILDREN | RDW_ERASE |
                         RDW_UPDATENOW);
        UpdateWindow(taskbar);
    }

    HWND secondary = nullptr;
    while ((secondary = FindWindowExW(nullptr, secondary,
                                      L"Shell_SecondaryTrayWnd", nullptr))) {
        RedrawWindow(secondary, nullptr, nullptr,
                     RDW_INVALIDATE | RDW_ALLCHILDREN | RDW_ERASE |
                         RDW_UPDATENOW);
        UpdateWindow(secondary);
    }
}

void SingleModWaitForNativeUnloadSettle() {
    for (int i = 0; i < 4; i++) {
        SingleModRedrawTaskbarWindows();
        Sleep(50);
    }
}

BOOL Wh_ModInit() {
    Wh_Log(L"single-mod init start");
    g_singleModStylerUninitDone = false;

    if (!MultirowComponent::Wh_ModInit()) {
        Wh_Log(L"Multirow component init failed");
        return FALSE;
    }

    if (!TrayComponent::Wh_ModInit()) {
        Wh_Log(L"Tray component init failed");
        return FALSE;
    }

    if (!HeightComponent::Wh_ModInit()) {
        Wh_Log(L"Height component init failed");
        return FALSE;
    }

    if (!Styler_Wh_ModInit()) {
        Wh_Log(L"Styler component init failed");
        return FALSE;
    }

    Wh_Log(L"single-mod init done");
    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L"single-mod after-init start");

    MultirowComponent::Wh_ModAfterInit();
    TrayComponent::Wh_ModAfterInit();
    HeightComponent::Wh_ModAfterInit();
    Styler_Wh_ModAfterInit();

    Wh_Log(L"single-mod after-init done");
}

void Wh_ModBeforeUninit() {
    Wh_Log(L"single-mod before-uninit start");

    TrayComponent::Wh_ModBeforeUninit();
    MultirowComponent::Wh_ModBeforeUninit();
    HeightComponent::Wh_ModBeforeUninit();
    Styler_Wh_ModUninit();
    g_singleModStylerUninitDone = true;
    SingleModWaitForNativeUnloadSettle();

    Wh_Log(L"single-mod before-uninit done");
}

void Wh_ModUninit() {
    Wh_Log(L"single-mod uninit start");

    if (!g_singleModStylerUninitDone) {
        Styler_Wh_ModUninit();
        g_singleModStylerUninitDone = true;
    }
    TrayComponent::Wh_ModUninit();
    MultirowComponent::Wh_ModUninit();
    HeightComponent::Wh_ModUninit();

    Wh_Log(L"single-mod uninit done");
}

void Wh_ModSettingsChanged() {
    Wh_Log(L"single-mod settings changed start");

    HeightComponent::Wh_ModSettingsChanged();
    MultirowComponent::Wh_ModSettingsChanged();
    TrayComponent::Wh_ModSettingsChanged();
    Styler_Wh_ModSettingsChanged();

    Wh_Log(L"single-mod settings changed done");
}
