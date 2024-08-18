// ==WindhawkMod==
// @id              taskbar-icon-size
// @name            Taskbar height and icon size
// @description     Control the taskbar height and icon size, improve icon quality (Windows 11 only)
// @version         1.2.11
// @author          m417z
// @github          https://github.com/m417z
// @twitter         https://twitter.com/m417z
// @homepage        https://m417z.com/
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -DWINVER=0x0605 -lcomctl32 -lole32 -loleaut32 -lruntimeobject -lshcore -lwininet
// ==/WindhawkMod==

// Source code is published under The GNU General Public License v3.0.
//
// For bug reports and feature requests, please open an issue here:
// https://github.com/ramensoftware/windhawk-mods/issues
//
// For pull requests, development takes place here:
// https://github.com/m417z/my-windhawk-mods

// ==WindhawkModReadme==
/*
# Taskbar height and icon size

Control the taskbar height and icon size. Make the taskbar icons large and
crisp, or small and compact.

By default, the Windows 11 taskbar shows taskbar icons with the 24x24 size.
Since icons in Windows are either 16x16 or 32x32, the 24x24 icons are downscaled
versions of the 32x32 variants, which makes them blurry. This mod allows to
change the size of icons, and so the original quality icons can be used, as well
as any other icon size.

![Before screenshot](https://i.imgur.com/9F4ibhX.png) \
*Icon size: 24x24, taskbar height: 48 (Windows 11 default)*

![After screenshot, large icons](https://i.imgur.com/DtsNIew.png) \
*Icon size: 32x32, taskbar height: 52*

![After screenshot, small icons](https://i.imgur.com/HrKj49g.png) \
*Icon size: 16x16, taskbar height: 34*

Only Windows 11 is supported. For older Windows versions check out [7+ Taskbar
Tweaker](https://tweaker.ramensoftware.com/).
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- IconSize: 32
  $name: Icon size
  $description: >-
    The size, in pixels, of icons on the taskbar (Windows 11 default: 24)
- TaskbarHeight: 52
  $name: Taskbar height
  $description: >-
    The height, in pixels, of the taskbar (Windows 11 default: 48)
- TaskbarButtonWidth: 44
  $name: Taskbar button width
  $description: >-
    The width, in pixels, of the taskbar buttons (Windows 11 default: 44)

    Note: Explorer has to be restarted after changing this option
*/
// ==/WindhawkModSettings==

#include <initguid.h>  // must come before knownfolders.h

#include <knownfolders.h>
#include <shlobj.h>
#include <wininet.h>

#undef GetCurrentTime

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.Xaml.Media.h>

#include <algorithm>
#include <atomic>
#include <functional>
#include <limits>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

using namespace winrt::Windows::UI::Xaml;

#ifndef SPI_SETLOGICALDPIOVERRIDE
#define SPI_SETLOGICALDPIOVERRIDE 0x009F
#endif

struct {
    int iconSize;
    int taskbarHeight;
    int taskbarButtonWidth;
} g_settings;

WCHAR g_taskbarViewDllPath[MAX_PATH];
std::atomic<bool> g_taskbarViewDllLoaded;
std::atomic<bool> g_applyingSettings;
std::atomic<bool> g_pendingMeasureOverride;
std::atomic<bool> g_unloading;
std::atomic<int> g_hookCallCounter;

int g_originalTaskbarHeight;
int g_taskbarHeight;
bool g_inSystemTrayController_UpdateFrameSize;
bool g_inAugmentedEntryPointButton_UpdateButtonPadding;

double* double_48_value_Original;

HANDLE g_restartExplorerPromptThread;
std::atomic<HWND> g_restartExplorerPromptWindow;

constexpr WCHAR kRestartExplorerPromptTitle[] =
    L"Taskbar height and icon size - Windhawk";
constexpr WCHAR kRestartExplorerPromptText[] =
    L"Explorer needs to be restarted to apply the new taskbar button width. "
    L"Restart now?";

WINUSERAPI UINT WINAPI GetDpiForWindow(HWND hwnd);
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

using ResourceDictionary_Lookup_t = winrt::Windows::Foundation::IInspectable*(
    WINAPI*)(void* pThis,
             void** result,
             winrt::Windows::Foundation::IInspectable* key);
ResourceDictionary_Lookup_t ResourceDictionary_Lookup_Original;
winrt::Windows::Foundation::IInspectable* WINAPI
ResourceDictionary_Lookup_Hook(void* pThis,
                               void** result,
                               winrt::Windows::Foundation::IInspectable* key) {
    // Wh_Log(L">");

    auto ret = ResourceDictionary_Lookup_Original(pThis, result, key);
    if (!*ret) {
        return ret;
    }

    auto keyString = key->try_as<winrt::hstring>();
    if (!keyString || keyString != L"MediumTaskbarButtonExtent") {
        return ret;
    }

    auto valueDouble = ret->try_as<double>();
    if (!valueDouble) {
        return ret;
    }

    double newValueDouble = g_settings.taskbarButtonWidth;
    if (newValueDouble != *valueDouble) {
        Wh_Log(L"Overriding value %s: %f->%f", keyString->c_str(), *valueDouble,
               newValueDouble);
        *ret = winrt::box_value(newValueDouble);
    }

    return ret;
}

using IconUtils_GetIconSize_t = void(WINAPI*)(bool small, int type, SIZE* size);
IconUtils_GetIconSize_t IconUtils_GetIconSize_Original;
void WINAPI IconUtils_GetIconSize_Hook(bool small, int type, SIZE* size) {
    IconUtils_GetIconSize_Original(small, type, size);

    if (!g_unloading && !small) {
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
    if (g_taskbarHeight) {
        UINT dpiX = 0;
        UINT dpiY = 0;
        GetDpiForMonitor(monitor, MDT_DEFAULT, &dpiX, &dpiY);

        size->cy = MulDiv(g_taskbarHeight, dpiY, 96);
    }
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
    if (enumTaskbarSize == 1 && !g_unloading) {
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
    if (!g_unloading) {
        return g_settings.iconSize;
    }

    return TaskbarConfiguration_GetIconHeightInViewPixels_double_Original(
        baseHeight);
}

using SystemTrayController_GetFrameSize_t =
    double(WINAPI*)(void* pThis, int enumTaskbarSize);
SystemTrayController_GetFrameSize_t SystemTrayController_GetFrameSize_Original;
double WINAPI SystemTrayController_GetFrameSize_Hook(void* pThis,
                                                     int enumTaskbarSize) {
    Wh_Log(L">");

    if (enumTaskbarSize == 1 && g_taskbarHeight) {
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
    Wh_Log(L">");

    if (enumTaskbarSize == 1 && g_taskbarHeight) {
        return g_taskbarHeight;
    }

    return SystemTraySecondaryController_GetFrameSize_Original(pThis,
                                                               enumTaskbarSize);
}

using TaskbarConfiguration_GetFrameSize_t =
    double(WINAPI*)(int enumTaskbarSize);
TaskbarConfiguration_GetFrameSize_t TaskbarConfiguration_GetFrameSize_Original;
double WINAPI TaskbarConfiguration_GetFrameSize_Hook(int enumTaskbarSize) {
    Wh_Log(L">");

    if (enumTaskbarSize == 1 && !g_originalTaskbarHeight) {
        g_originalTaskbarHeight =
            TaskbarConfiguration_GetFrameSize_Original(enumTaskbarSize);
    }

    if (enumTaskbarSize == 1 && g_taskbarHeight) {
        return g_taskbarHeight;
    }

    return TaskbarConfiguration_GetFrameSize_Original(enumTaskbarSize);
}

using SystemTrayController_UpdateFrameSize_t = void(WINAPI*)(void* pThis);
SystemTrayController_UpdateFrameSize_t
    SystemTrayController_UpdateFrameSize_SymbolAddress;
SystemTrayController_UpdateFrameSize_t
    SystemTrayController_UpdateFrameSize_Original;
void WINAPI SystemTrayController_UpdateFrameSize_Hook(void* pThis) {
    Wh_Log(L">");

    static LONG lastHeightOffset = []() -> LONG {
        // Find the last height offset and reset the height value.
        //
        // 66 0f 2e b3 b0 00 00 00 UCOMISD    uVar4,qword ptr [RBX + 0xb0]
        // 7a 4c                   JP         LAB_180075641
        // 75 4a                   JNZ        LAB_180075641
        const BYTE* start =
            (const BYTE*)SystemTrayController_UpdateFrameSize_SymbolAddress;
        const BYTE* end = start + 0x200;
        for (const BYTE* p = start; p != end; p++) {
            if (p[0] == 0x66 && p[1] == 0x0F && p[2] == 0x2E && p[3] == 0xB3 &&
                p[8] == 0x7A && p[10] == 0x75) {
                LONG offset = *(LONG*)(p + 4);
                Wh_Log(L"lastHeightOffset=0x%X", offset);
                return offset;
            }
        }

        Wh_Log(L"lastHeightOffset not found");
        return 0;
    }();

    if (lastHeightOffset > 0) {
        *(double*)((BYTE*)pThis + lastHeightOffset) = 0;
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

    if (TaskbarFrame_MaxHeight_double_Original) {
        TaskbarFrame_MaxHeight_double_Original(
            pThis, std::numeric_limits<double>::infinity());
    }

    return TaskbarFrame_Height_double_Original(pThis, value);
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

    if (g_inSystemTrayController_UpdateFrameSize) {
        Wh_Log(L">");
        // Set the system tray height to NaN, otherwise it may not match the
        // custom taskbar height.
        value = std::numeric_limits<double>::quiet_NaN();
    }

    SystemTrayFrame_Height_Original(pThis, value);
}

using TaskbarFrame_MeasureOverride_t =
    int(WINAPI*)(void* pThis,
                 void* param1,
                 winrt::Windows::Foundation::Size* resultSize);
TaskbarFrame_MeasureOverride_t TaskbarFrame_MeasureOverride_Original;
int WINAPI TaskbarFrame_MeasureOverride_Hook(
    void* pThis,
    void* param1,
    winrt::Windows::Foundation::Size* resultSize) {
    g_hookCallCounter++;

    Wh_Log(L">");

    FrameworkElement taskbarFrameElement = nullptr;
    ((IUnknown*)pThis)
        ->QueryInterface(winrt::guid_of<FrameworkElement>(),
                         winrt::put_abi(taskbarFrameElement));
    if (taskbarFrameElement) {
        taskbarFrameElement.MaxHeight(std::numeric_limits<double>::infinity());
    }

    int ret = TaskbarFrame_MeasureOverride_Original(pThis, param1, resultSize);

    g_pendingMeasureOverride = false;

    g_hookCallCounter--;

    return ret;
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
    Wh_Log(L">");

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

        bool widePanel = panelGrid.Width() > panelGrid.Height();
        if (widePanel) {
            panelGrid.VerticalAlignment(g_unloading
                                            ? VerticalAlignment::Stretch
                                            : VerticalAlignment::Center);
        } else {
            auto margin = Thickness{8, 8, 8, 8};

            if (!g_unloading) {
                margin.Left = marginValue;
                margin.Top = marginValue;
                margin.Right = marginValue;
                margin.Bottom = marginValue;

                if (g_taskbarHeight < 48) {
                    margin.Top -= static_cast<double>(48 - g_taskbarHeight) / 2;
                    if (margin.Top < 0) {
                        margin.Top = 0;
                    }

                    margin.Bottom = marginValue * 2 - margin.Top;
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

        return false;
    });
}

using SHAppBarMessage_t = decltype(&SHAppBarMessage);
SHAppBarMessage_t SHAppBarMessage_Original;
auto WINAPI SHAppBarMessage_Hook(DWORD dwMessage, PAPPBARDATA pData) {
    auto ret = SHAppBarMessage_Original(dwMessage, pData);

    // This is used to position secondary taskbars.
    if (dwMessage == ABM_QUERYPOS && ret && g_taskbarHeight) {
        pData->rc.top =
            pData->rc.bottom -
            MulDiv(g_taskbarHeight, GetDpiForWindow(pData->hWnd), 96);
    }

    return ret;
}

void LoadSettings() {
    g_settings.iconSize = Wh_GetIntSetting(L"IconSize");
    g_settings.taskbarHeight = Wh_GetIntSetting(L"TaskbarHeight");
    g_settings.taskbarButtonWidth = Wh_GetIntSetting(L"TaskbarButtonWidth");
}

HWND GetTaskbarWnd() {
    static HWND hTaskbarWnd;

    if (!hTaskbarWnd) {
        HWND hWnd = FindWindow(L"Shell_TrayWnd", nullptr);

        DWORD processId = 0;
        if (hWnd && GetWindowThreadProcessId(hWnd, &processId) &&
            processId == GetCurrentProcessId()) {
            hTaskbarWnd = hWnd;
        }
    }

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

void ApplySettings(int taskbarHeight) {
    if (taskbarHeight < 2) {
        taskbarHeight = 2;
    }

    HWND hTaskbarWnd = GetTaskbarWnd();
    if (!hTaskbarWnd) {
        g_taskbarHeight = taskbarHeight;
        return;
    }

    if (!g_taskbarHeight) {
        RECT taskbarRect{};
        GetWindowRect(hTaskbarWnd, &taskbarRect);
        g_taskbarHeight = MulDiv(taskbarRect.bottom - taskbarRect.top, 96,
                                 GetDpiForWindow(hTaskbarWnd));
    }

    g_applyingSettings = true;

    if (taskbarHeight == g_taskbarHeight) {
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
        for (int i = 0; i < 100; i++) {
            if (!g_pendingMeasureOverride) {
                break;
            }

            Sleep(100);
        }
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

    // Wait for the change to apply.
    for (int i = 0; i < 100; i++) {
        if (!g_pendingMeasureOverride) {
            break;
        }

        Sleep(100);
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

void PromptForExplorerRestart() {
    if (g_restartExplorerPromptThread) {
        if (WaitForSingleObject(g_restartExplorerPromptThread, 0) !=
            WAIT_OBJECT_0) {
            return;
        }

        CloseHandle(g_restartExplorerPromptThread);
    }

    g_restartExplorerPromptThread = CreateThread(
        nullptr, 0,
        [](LPVOID lpParameter) WINAPI -> DWORD {
            TASKDIALOGCONFIG taskDialogConfig{
                .cbSize = sizeof(taskDialogConfig),
                .dwFlags = TDF_ALLOW_DIALOG_CANCELLATION,
                .dwCommonButtons = TDCBF_YES_BUTTON | TDCBF_NO_BUTTON,
                .pszWindowTitle = kRestartExplorerPromptTitle,
                .pszMainIcon = TD_INFORMATION_ICON,
                .pszContent = kRestartExplorerPromptText,
                .pfCallback = [](HWND hwnd, UINT msg, WPARAM wParam,
                                 LPARAM lParam, LONG_PTR lpRefData)
                                  WINAPI -> HRESULT {
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
                WCHAR commandLine[] =
                    L"cmd.exe /c "
                    L"\"taskkill /F /IM explorer.exe & start explorer\"";
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
        nullptr, 0, nullptr);
}

struct SYMBOL_HOOK {
    std::vector<std::wstring_view> symbols;
    void** pOriginalFunction;
    void* hookFunction = nullptr;
    bool optional = false;
};

bool HookSymbols(HMODULE module,
                 const SYMBOL_HOOK* symbolHooks,
                 size_t symbolHooksCount,
                 bool cacheOnly = false) {
    const WCHAR cacheVer = L'1';
    const WCHAR cacheSep = L'#';
    constexpr size_t cacheMaxSize = 10240;

    WCHAR moduleFilePath[MAX_PATH];
    if (!GetModuleFileName(module, moduleFilePath, ARRAYSIZE(moduleFilePath))) {
        Wh_Log(L"GetModuleFileName failed");
        return false;
    }

    PCWSTR moduleFileName = wcsrchr(moduleFilePath, L'\\');
    if (!moduleFileName) {
        Wh_Log(L"GetModuleFileName returned unsupported path");
        return false;
    }

    moduleFileName++;

    WCHAR cacheBuffer[cacheMaxSize + 1];
    std::wstring cacheStrKey = std::wstring(L"symbol-cache-") + moduleFileName;
    Wh_GetStringValue(cacheStrKey.c_str(), cacheBuffer, ARRAYSIZE(cacheBuffer));

    std::wstring_view cacheBufferView(cacheBuffer);

    // https://stackoverflow.com/a/46931770
    auto splitStringView = [](std::wstring_view s, WCHAR delimiter) {
        size_t pos_start = 0, pos_end;
        std::wstring_view token;
        std::vector<std::wstring_view> res;

        while ((pos_end = s.find(delimiter, pos_start)) !=
               std::wstring_view::npos) {
            token = s.substr(pos_start, pos_end - pos_start);
            pos_start = pos_end + 1;
            res.push_back(token);
        }

        res.push_back(s.substr(pos_start));
        return res;
    };

    auto cacheParts = splitStringView(cacheBufferView, cacheSep);

    std::vector<bool> symbolResolved(symbolHooksCount, false);
    std::wstring newSystemCacheStr;

    auto onSymbolResolved = [symbolHooks, symbolHooksCount, &symbolResolved,
                             &newSystemCacheStr,
                             module](std::wstring_view symbol, void* address) {
        for (size_t i = 0; i < symbolHooksCount; i++) {
            if (symbolResolved[i]) {
                continue;
            }

            bool match = false;
            for (auto hookSymbol : symbolHooks[i].symbols) {
                if (hookSymbol == symbol) {
                    match = true;
                    break;
                }
            }

            if (!match) {
                continue;
            }

            if (symbolHooks[i].hookFunction) {
                Wh_SetFunctionHook(address, symbolHooks[i].hookFunction,
                                   symbolHooks[i].pOriginalFunction);
                Wh_Log(L"Hooked %p: %.*s", address, symbol.length(),
                       symbol.data());
            } else {
                *symbolHooks[i].pOriginalFunction = address;
                Wh_Log(L"Found %p: %.*s", address, symbol.length(),
                       symbol.data());
            }

            symbolResolved[i] = true;

            newSystemCacheStr += cacheSep;
            newSystemCacheStr += symbol;
            newSystemCacheStr += cacheSep;
            newSystemCacheStr +=
                std::to_wstring((ULONG_PTR)address - (ULONG_PTR)module);

            break;
        }
    };

    IMAGE_DOS_HEADER* dosHeader = (IMAGE_DOS_HEADER*)module;
    IMAGE_NT_HEADERS* header =
        (IMAGE_NT_HEADERS*)((BYTE*)dosHeader + dosHeader->e_lfanew);
    auto timeStamp = std::to_wstring(header->FileHeader.TimeDateStamp);
    auto imageSize = std::to_wstring(header->OptionalHeader.SizeOfImage);

    newSystemCacheStr += cacheVer;
    newSystemCacheStr += cacheSep;
    newSystemCacheStr += timeStamp;
    newSystemCacheStr += cacheSep;
    newSystemCacheStr += imageSize;

    if (cacheParts.size() >= 3 &&
        cacheParts[0] == std::wstring_view(&cacheVer, 1) &&
        cacheParts[1] == timeStamp && cacheParts[2] == imageSize) {
        for (size_t i = 3; i + 1 < cacheParts.size(); i += 2) {
            auto symbol = cacheParts[i];
            auto address = cacheParts[i + 1];
            if (address.length() == 0) {
                continue;
            }

            void* addressPtr =
                (void*)(std::stoull(std::wstring(address), nullptr, 10) +
                        (ULONG_PTR)module);

            onSymbolResolved(symbol, addressPtr);
        }

        for (size_t i = 0; i < symbolHooksCount; i++) {
            if (symbolResolved[i] || !symbolHooks[i].optional) {
                continue;
            }

            size_t noAddressMatchCount = 0;
            for (size_t j = 3; j + 1 < cacheParts.size(); j += 2) {
                auto symbol = cacheParts[j];
                auto address = cacheParts[j + 1];
                if (address.length() != 0) {
                    continue;
                }

                for (auto hookSymbol : symbolHooks[i].symbols) {
                    if (hookSymbol == symbol) {
                        noAddressMatchCount++;
                        break;
                    }
                }
            }

            if (noAddressMatchCount == symbolHooks[i].symbols.size()) {
                Wh_Log(L"Optional symbol %d doesn't exist (from cache)", i);

                symbolResolved[i] = true;

                for (auto hookSymbol : symbolHooks[i].symbols) {
                    newSystemCacheStr += cacheSep;
                    newSystemCacheStr += hookSymbol;
                    newSystemCacheStr += cacheSep;
                }
            }
        }

        if (std::all_of(symbolResolved.begin(), symbolResolved.end(),
                        [](bool b) { return b; })) {
            return true;
        }
    }

    Wh_Log(L"Couldn't resolve all symbols from cache");

    if (cacheOnly) {
        return false;
    }

    WH_FIND_SYMBOL findSymbol;
    HANDLE findSymbolHandle = Wh_FindFirstSymbol(module, nullptr, &findSymbol);
    if (!findSymbolHandle) {
        Wh_Log(L"Wh_FindFirstSymbol failed");
        return false;
    }

    do {
        onSymbolResolved(findSymbol.symbol, findSymbol.address);
    } while (Wh_FindNextSymbol(findSymbolHandle, &findSymbol));

    Wh_FindCloseSymbol(findSymbolHandle);

    for (size_t i = 0; i < symbolHooksCount; i++) {
        if (symbolResolved[i]) {
            continue;
        }

        if (!symbolHooks[i].optional) {
            Wh_Log(L"Unresolved symbol: %d", i);
            return false;
        }

        Wh_Log(L"Optional symbol %d doesn't exist", i);

        for (auto hookSymbol : symbolHooks[i].symbols) {
            newSystemCacheStr += cacheSep;
            newSystemCacheStr += hookSymbol;
            newSystemCacheStr += cacheSep;
        }
    }

    if (newSystemCacheStr.length() <= cacheMaxSize) {
        Wh_SetStringValue(cacheStrKey.c_str(), newSystemCacheStr.c_str());
    } else {
        Wh_Log(L"Cache is too large (%zu)", newSystemCacheStr.length());
    }

    return true;
}

std::optional<std::wstring> GetUrlContent(PCWSTR lpUrl) {
    HINTERNET hOpenHandle = InternetOpen(
        L"WindhawkMod", INTERNET_OPEN_TYPE_PRECONFIG, nullptr, nullptr, 0);
    if (!hOpenHandle) {
        return std::nullopt;
    }

    HINTERNET hUrlHandle =
        InternetOpenUrl(hOpenHandle, lpUrl, nullptr, 0,
                        INTERNET_FLAG_NO_AUTH | INTERNET_FLAG_NO_CACHE_WRITE |
                            INTERNET_FLAG_NO_COOKIES | INTERNET_FLAG_NO_UI |
                            INTERNET_FLAG_PRAGMA_NOCACHE | INTERNET_FLAG_RELOAD,
                        0);
    if (!hUrlHandle) {
        InternetCloseHandle(hOpenHandle);
        return std::nullopt;
    }

    DWORD dwStatusCode = 0;
    DWORD dwStatusCodeSize = sizeof(dwStatusCode);
    if (!HttpQueryInfo(hUrlHandle,
                       HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER,
                       &dwStatusCode, &dwStatusCodeSize, nullptr) ||
        dwStatusCode != 200) {
        InternetCloseHandle(hUrlHandle);
        InternetCloseHandle(hOpenHandle);
        return std::nullopt;
    }

    LPBYTE pUrlContent = (LPBYTE)HeapAlloc(GetProcessHeap(), 0, 0x400);
    if (!pUrlContent) {
        InternetCloseHandle(hUrlHandle);
        InternetCloseHandle(hOpenHandle);
        return std::nullopt;
    }

    DWORD dwNumberOfBytesRead;
    InternetReadFile(hUrlHandle, pUrlContent, 0x400, &dwNumberOfBytesRead);
    DWORD dwLength = dwNumberOfBytesRead;

    while (dwNumberOfBytesRead) {
        LPBYTE pNewUrlContent = (LPBYTE)HeapReAlloc(
            GetProcessHeap(), 0, pUrlContent, dwLength + 0x400);
        if (!pNewUrlContent) {
            InternetCloseHandle(hUrlHandle);
            InternetCloseHandle(hOpenHandle);
            HeapFree(GetProcessHeap(), 0, pUrlContent);
            return std::nullopt;
        }

        pUrlContent = pNewUrlContent;
        InternetReadFile(hUrlHandle, pUrlContent + dwLength, 0x400,
                         &dwNumberOfBytesRead);
        dwLength += dwNumberOfBytesRead;
    }

    InternetCloseHandle(hUrlHandle);
    InternetCloseHandle(hOpenHandle);

    // Assume UTF-8.
    int charsNeeded = MultiByteToWideChar(CP_UTF8, 0, (PCSTR)pUrlContent,
                                          dwLength, nullptr, 0);
    std::wstring unicodeContent(charsNeeded, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, (PCSTR)pUrlContent, dwLength,
                        unicodeContent.data(), unicodeContent.size());

    HeapFree(GetProcessHeap(), 0, pUrlContent);

    return unicodeContent;
}

bool HookSymbolsWithOnlineCacheFallback(HMODULE module,
                                        const SYMBOL_HOOK* symbolHooks,
                                        size_t symbolHooksCount) {
    constexpr WCHAR kModIdForCache[] = L"taskbar-icon-size";

    if (HookSymbols(module, symbolHooks, symbolHooksCount,
                    /*cacheOnly=*/true)) {
        return true;
    }

    Wh_Log(L"HookSymbols() from cache failed, trying to get an online cache");

    WCHAR moduleFilePath[MAX_PATH];
    DWORD moduleFilePathLen =
        GetModuleFileName(module, moduleFilePath, ARRAYSIZE(moduleFilePath));
    if (!moduleFilePathLen || moduleFilePathLen == ARRAYSIZE(moduleFilePath)) {
        Wh_Log(L"GetModuleFileName failed");
        return false;
    }

    PWSTR moduleFileName = wcsrchr(moduleFilePath, L'\\');
    if (!moduleFileName) {
        Wh_Log(L"GetModuleFileName returned unsupported path");
        return false;
    }

    moduleFileName++;

    DWORD moduleFileNameLen =
        moduleFilePathLen - (moduleFileName - moduleFilePath);

    LCMapStringEx(LOCALE_NAME_USER_DEFAULT, LCMAP_LOWERCASE, moduleFileName,
                  moduleFileNameLen, moduleFileName, moduleFileNameLen, nullptr,
                  nullptr, 0);

    IMAGE_DOS_HEADER* dosHeader = (IMAGE_DOS_HEADER*)module;
    IMAGE_NT_HEADERS* header =
        (IMAGE_NT_HEADERS*)((BYTE*)dosHeader + dosHeader->e_lfanew);
    auto timeStamp = std::to_wstring(header->FileHeader.TimeDateStamp);
    auto imageSize = std::to_wstring(header->OptionalHeader.SizeOfImage);

    std::wstring cacheStrKey =
#if defined(_M_IX86)
        L"symbol-x86-cache-";
#elif defined(_M_X64)
        L"symbol-cache-";
#else
#error "Unsupported architecture"
#endif
    cacheStrKey += moduleFileName;

    std::wstring onlineCacheUrl =
        L"https://ramensoftware.github.io/windhawk-mod-symbol-cache/";
    onlineCacheUrl += kModIdForCache;
    onlineCacheUrl += L'/';
    onlineCacheUrl += cacheStrKey;
    onlineCacheUrl += L'/';
    onlineCacheUrl += timeStamp;
    onlineCacheUrl += L'-';
    onlineCacheUrl += imageSize;
    onlineCacheUrl += L".txt";

    Wh_Log(L"Looking for an online cache at %s", onlineCacheUrl.c_str());

    auto onlineCache = GetUrlContent(onlineCacheUrl.c_str());
    if (onlineCache) {
        Wh_SetStringValue(cacheStrKey.c_str(), onlineCache->c_str());
    } else {
        Wh_Log(L"Failed to get online cache");
    }

    return HookSymbols(module, symbolHooks, symbolHooksCount);
}

bool GetTaskbarViewDllPath(WCHAR path[MAX_PATH]) {
    WCHAR szWindowsDirectory[MAX_PATH];
    if (!GetWindowsDirectory(szWindowsDirectory,
                             ARRAYSIZE(szWindowsDirectory))) {
        Wh_Log(L"GetWindowsDirectory failed");
        return false;
    }

    // Windows 11 version 22H2.
    wcscpy_s(path, MAX_PATH, szWindowsDirectory);
    wcscat_s(
        path, MAX_PATH,
        LR"(\SystemApps\MicrosoftWindows.Client.Core_cw5n1h2txyewy\Taskbar.View.dll)");
    if (GetFileAttributes(path) != INVALID_FILE_ATTRIBUTES) {
        return true;
    }

    // Windows 11 version 21H2.
    wcscpy_s(path, MAX_PATH, szWindowsDirectory);
    wcscat_s(
        path, MAX_PATH,
        LR"(\SystemApps\MicrosoftWindows.Client.CBS_cw5n1h2txyewy\ExplorerExtensions.dll)");
    if (GetFileAttributes(path) != INVALID_FILE_ATTRIBUTES) {
        return true;
    }

    return false;
}

bool HookTaskbarViewDllSymbols(HMODULE module) {
    // Taskbar.View.dll, ExplorerExtensions.dll
    SYMBOL_HOOK symbolHooks[] =  //
        {
            {
                // For Windows 11 version 21H2.
                {LR"(__real@4048000000000000)"},
                (void**)&double_48_value_Original,
                nullptr,
                true,
            },
            {
                {
                    LR"(public: __cdecl winrt::impl::consume_Windows_Foundation_Collections_IMap<struct winrt::Windows::UI::Xaml::ResourceDictionary,struct winrt::Windows::Foundation::IInspectable,struct winrt::Windows::Foundation::IInspectable>::Lookup(struct winrt::Windows::Foundation::IInspectable const &)const )",
                    LR"(public: __cdecl winrt::impl::consume_Windows_Foundation_Collections_IMap<struct winrt::Windows::UI::Xaml::ResourceDictionary,struct winrt::Windows::Foundation::IInspectable,struct winrt::Windows::Foundation::IInspectable>::Lookup(struct winrt::Windows::Foundation::IInspectable const & __ptr64)const __ptr64)",

                    // Windows 11 version 21H2.
                    LR"(public: struct winrt::Windows::Foundation::IInspectable __cdecl winrt::impl::consume_Windows_Foundation_Collections_IMap<struct winrt::Windows::UI::Xaml::ResourceDictionary,struct winrt::Windows::Foundation::IInspectable,struct winrt::Windows::Foundation::IInspectable>::Lookup(struct winrt::Windows::Foundation::IInspectable const &)const )",
                    LR"(public: struct winrt::Windows::Foundation::IInspectable __cdecl winrt::impl::consume_Windows_Foundation_Collections_IMap<struct winrt::Windows::UI::Xaml::ResourceDictionary,struct winrt::Windows::Foundation::IInspectable,struct winrt::Windows::Foundation::IInspectable>::Lookup(struct winrt::Windows::Foundation::IInspectable const & __ptr64)const __ptr64)",
                },
                (void**)&ResourceDictionary_Lookup_Original,
                (void*)ResourceDictionary_Lookup_Hook,
            },
            {
                {
                    LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskListItemViewModel,struct winrt::Taskbar::ITaskListItemViewModel>::GetIconHeight(void *,double *))",
                    LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskListItemViewModel,struct winrt::Taskbar::ITaskListItemViewModel>::GetIconHeight(void * __ptr64,double * __ptr64) __ptr64)",
                },
                (void**)&TaskListItemViewModel_GetIconHeight_Original,
                (void*)TaskListItemViewModel_GetIconHeight_Hook,
                true,  // Gone in KB5040527 (Taskbar.View.dll 2124.16310.10.0).
            },
            {
                {
                    LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskListGroupViewModel,struct winrt::Taskbar::ITaskbarAppItemViewModel>::GetIconHeight(void *,double *))",
                    LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskListGroupViewModel,struct winrt::Taskbar::ITaskbarAppItemViewModel>::GetIconHeight(void * __ptr64,double * __ptr64) __ptr64)",
                },
                (void**)&TaskListGroupViewModel_GetIconHeight_Original,
                (void*)TaskListGroupViewModel_GetIconHeight_Hook,
                true,  // Missing in older Windows 11 versions.
            },
            {
                {
                    LR"(public: static double __cdecl winrt::Taskbar::implementation::TaskbarConfiguration::GetIconHeightInViewPixels(enum winrt::WindowsUdk::UI::Shell::TaskbarSize))",
                    LR"(public: static double __cdecl winrt::Taskbar::implementation::TaskbarConfiguration::GetIconHeightInViewPixels(enum winrt::WindowsUdk::UI::Shell::TaskbarSize) __ptr64)",
                },
                (void**)&TaskbarConfiguration_GetIconHeightInViewPixels_taskbarSizeEnum_Original,
                (void*)
                    TaskbarConfiguration_GetIconHeightInViewPixels_taskbarSizeEnum_Hook,
            },
            {
                {
                    LR"(public: static double __cdecl winrt::Taskbar::implementation::TaskbarConfiguration::GetIconHeightInViewPixels(double))",
                    LR"(public: static double __cdecl winrt::Taskbar::implementation::TaskbarConfiguration::GetIconHeightInViewPixels(double) __ptr64)",
                },
                (void**)&TaskbarConfiguration_GetIconHeightInViewPixels_double_Original,
                (void*)
                    TaskbarConfiguration_GetIconHeightInViewPixels_double_Hook,
                true,  // From Windows 11 version 22H2.
            },
            {
                {
                    LR"(private: double __cdecl winrt::SystemTray::implementation::SystemTrayController::GetFrameSize(enum winrt::WindowsUdk::UI::Shell::TaskbarSize))",
                    LR"(private: double __cdecl winrt::SystemTray::implementation::SystemTrayController::GetFrameSize(enum winrt::WindowsUdk::UI::Shell::TaskbarSize) __ptr64)",
                },
                (void**)&SystemTrayController_GetFrameSize_Original,
                (void*)SystemTrayController_GetFrameSize_Hook,
                true,  // From Windows 11 version 22H2, inlined sometimes.
            },
            {
                {
                    LR"(private: double __cdecl winrt::SystemTray::implementation::SystemTraySecondaryController::GetFrameSize(enum winrt::WindowsUdk::UI::Shell::TaskbarSize))",
                    LR"(private: double __cdecl winrt::SystemTray::implementation::SystemTraySecondaryController::GetFrameSize(enum winrt::WindowsUdk::UI::Shell::TaskbarSize) __ptr64)",
                },
                (void**)&SystemTraySecondaryController_GetFrameSize_Original,
                (void*)SystemTraySecondaryController_GetFrameSize_Hook,
                true,  // From Windows 11 version 22H2.
            },
            {
                {
                    LR"(public: static double __cdecl winrt::Taskbar::implementation::TaskbarConfiguration::GetFrameSize(enum winrt::WindowsUdk::UI::Shell::TaskbarSize))",
                    LR"(public: static double __cdecl winrt::Taskbar::implementation::TaskbarConfiguration::GetFrameSize(enum winrt::WindowsUdk::UI::Shell::TaskbarSize) __ptr64)",
                },
                (void**)&TaskbarConfiguration_GetFrameSize_Original,
                (void*)TaskbarConfiguration_GetFrameSize_Hook,
                true,  // From Windows 11 version 22H2.
            },
            {
                {
                    LR"(private: void __cdecl winrt::SystemTray::implementation::SystemTrayController::UpdateFrameSize(void))",
                    LR"(private: void __cdecl winrt::SystemTray::implementation::SystemTrayController::UpdateFrameSize(void) __ptr64)",
                },
                (void**)&SystemTrayController_UpdateFrameSize_SymbolAddress,
                nullptr,  // Hooked manually, we need the symbol address.
                true,     // Missing in older Windows 11 versions.
            },
            {
                {
                    LR"(public: __cdecl winrt::impl::consume_Windows_UI_Xaml_IFrameworkElement<struct winrt::Taskbar::implementation::TaskbarFrame>::MaxHeight(double)const )",
                    LR"(public: __cdecl winrt::impl::consume_Windows_UI_Xaml_IFrameworkElement<struct winrt::Taskbar::implementation::TaskbarFrame>::MaxHeight(double)const __ptr64)",
                },
                (void**)&TaskbarFrame_MaxHeight_double_Original,
                nullptr,
                true,  // From Windows 11 version 22H2.
            },
            {
                {
                    LR"(public: __cdecl winrt::impl::consume_Windows_UI_Xaml_IFrameworkElement<struct winrt::Taskbar::implementation::TaskbarFrame>::Height(double)const )",
                    LR"(public: __cdecl winrt::impl::consume_Windows_UI_Xaml_IFrameworkElement<struct winrt::Taskbar::implementation::TaskbarFrame>::Height(double)const __ptr64)",

                    // Windows 11 version 21H2.
                    LR"(public: void __cdecl winrt::impl::consume_Windows_UI_Xaml_IFrameworkElement<struct winrt::Taskbar::implementation::TaskbarFrame>::Height(double)const )",
                    LR"(public: void __cdecl winrt::impl::consume_Windows_UI_Xaml_IFrameworkElement<struct winrt::Taskbar::implementation::TaskbarFrame>::Height(double)const __ptr64)",
                },
                (void**)&TaskbarFrame_Height_double_Original,
                (void*)TaskbarFrame_Height_double_Hook,
                true,  // Gone in Windows 11 version 24H2.
            },
            {
                {
                    LR"(private: void __cdecl winrt::SystemTray::implementation::SystemTraySecondaryController::UpdateFrameSize(void))",
                    LR"(private: void __cdecl winrt::SystemTray::implementation::SystemTraySecondaryController::UpdateFrameSize(void) __ptr64)",
                },
                (void**)&SystemTraySecondaryController_UpdateFrameSize_Original,
                (void*)SystemTraySecondaryController_UpdateFrameSize_Hook,
                true,  // Missing in older Windows 11 versions.
            },
            {
                {
                    LR"(public: __cdecl winrt::impl::consume_Windows_UI_Xaml_IFrameworkElement<struct winrt::SystemTray::SystemTrayFrame>::Height(double)const )",
                    LR"(public: __cdecl winrt::impl::consume_Windows_UI_Xaml_IFrameworkElement<struct winrt::SystemTray::SystemTrayFrame>::Height(double)const __ptr64)",
                },
                (void**)&SystemTrayFrame_Height_Original,
                (void*)SystemTrayFrame_Height_Hook,
                true,  // From Windows 11 version 22H2.
            },
            {
                {
                    LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskbarFrame,struct winrt::Windows::UI::Xaml::IFrameworkElementOverrides>::MeasureOverride(struct winrt::Windows::Foundation::Size,struct winrt::Windows::Foundation::Size *))",
                    LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskbarFrame,struct winrt::Windows::UI::Xaml::IFrameworkElementOverrides>::MeasureOverride(struct winrt::Windows::Foundation::Size,struct winrt::Windows::Foundation::Size * __ptr64) __ptr64)",
                },
                (void**)&TaskbarFrame_MeasureOverride_Original,
                (void*)TaskbarFrame_MeasureOverride_Hook,
            },
            {
                {
                    LR"(protected: virtual void __cdecl winrt::Taskbar::implementation::AugmentedEntryPointButton::UpdateButtonPadding(void))",
                    LR"(protected: virtual void __cdecl winrt::Taskbar::implementation::AugmentedEntryPointButton::UpdateButtonPadding(void) __ptr64)",
                },
                (void**)&AugmentedEntryPointButton_UpdateButtonPadding_Original,
                (void*)AugmentedEntryPointButton_UpdateButtonPadding_Hook,
            },
            {
                {
                    LR"(public: __cdecl winrt::impl::consume_Windows_UI_Xaml_IFrameworkElement<struct winrt::Windows::UI::Xaml::Controls::Primitives::RepeatButton>::Width(double)const )",
                    LR"(public: __cdecl winrt::impl::consume_Windows_UI_Xaml_IFrameworkElement<struct winrt::Windows::UI::Xaml::Controls::Primitives::RepeatButton>::Width(double)const __ptr64)",
                },
                (void**)&RepeatButton_Width_Original,
                (void*)RepeatButton_Width_Hook,
                true,  // From Windows 11 version 22H2.
            },
        };

    if (!HookSymbolsWithOnlineCacheFallback(module, symbolHooks,
                                            ARRAYSIZE(symbolHooks))) {
        return false;
    }

    if (SystemTrayController_UpdateFrameSize_SymbolAddress) {
        Wh_SetFunctionHook(
            (void*)SystemTrayController_UpdateFrameSize_SymbolAddress,
            (void*)SystemTrayController_UpdateFrameSize_Hook,
            (void**)&SystemTrayController_UpdateFrameSize_Original);
    }

    return true;
}

bool HookTaskbarDllSymbols() {
    HMODULE module = LoadLibrary(L"taskbar.dll");
    if (!module) {
        Wh_Log(L"Failed to load taskbar.dll");
        return false;
    }

    SYMBOL_HOOK taskbarDllHooks[] = {
        {
            {
                LR"(void __cdecl IconUtils::GetIconSize(bool,enum IconUtils::IconType,struct tagSIZE *))",
                LR"(void __cdecl IconUtils::GetIconSize(bool,enum IconUtils::IconType,struct tagSIZE * __ptr64))",
            },
            (void**)&IconUtils_GetIconSize_Original,
            (void*)IconUtils_GetIconSize_Hook,
        },
        {
            {
                LR"(public: virtual bool __cdecl IconContainer::IsStorageRecreationRequired(class CCoSimpleArray<unsigned int,4294967294,class CSimpleArrayStandardCompareHelper<unsigned int> > const &,enum IconContainerFlags))",
                LR"(public: virtual bool __cdecl IconContainer::IsStorageRecreationRequired(class CCoSimpleArray<unsigned int,4294967294,class CSimpleArrayStandardCompareHelper<unsigned int> > const & __ptr64,enum IconContainerFlags) __ptr64)",
            },
            (void**)&IconContainer_IsStorageRecreationRequired_Original,
            (void*)IconContainer_IsStorageRecreationRequired_Hook,
        },
        {
            {
                LR"(public: virtual void __cdecl TrayUI::GetMinSize(struct HMONITOR__ *,struct tagSIZE *))",
                LR"(public: virtual void __cdecl TrayUI::GetMinSize(struct HMONITOR__ * __ptr64,struct tagSIZE * __ptr64) __ptr64)",
            },
            (void**)&TrayUI_GetMinSize_Original,
            (void*)TrayUI_GetMinSize_Hook,
            true,
        },
        {
            {
                LR"(public: void __cdecl TrayUI::_StuckTrayChange(void))",
                LR"(public: void __cdecl TrayUI::_StuckTrayChange(void) __ptr64)",
            },
            (void**)&TrayUI__StuckTrayChange_Original,
        },
        {
            {
                LR"(public: void __cdecl TrayUI::_HandleSettingChange(struct HWND__ *,unsigned int,unsigned __int64,__int64))",
                LR"(public: void __cdecl TrayUI::_HandleSettingChange(struct HWND__ * __ptr64,unsigned int,unsigned __int64,__int64) __ptr64)",
            },
            (void**)&TrayUI__HandleSettingChange_Original,
            (void*)TrayUI__HandleSettingChange_Hook,
        },
    };

    return HookSymbolsWithOnlineCacheFallback(module, taskbarDllHooks,
                                              ARRAYSIZE(taskbarDllHooks));
}

BOOL ModInitWithTaskbarView(HMODULE taskbarViewModule) {
    if (!HookTaskbarViewDllSymbols(taskbarViewModule)) {
        return FALSE;
    }

    if (!HookTaskbarDllSymbols()) {
        return FALSE;
    }

    Wh_SetFunctionHook((void*)SHAppBarMessage, (void*)SHAppBarMessage_Hook,
                       (void**)&SHAppBarMessage_Original);

    return TRUE;
}

using LoadLibraryExW_t = decltype(&LoadLibraryExW);
LoadLibraryExW_t LoadLibraryExW_Original;
HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR lpLibFileName,
                                   HANDLE hFile,
                                   DWORD dwFlags) {
    HMODULE module = LoadLibraryExW_Original(lpLibFileName, hFile, dwFlags);
    if (!module || g_unloading) {
        return module;
    }

    if (!g_taskbarViewDllLoaded &&
        _wcsicmp(g_taskbarViewDllPath, lpLibFileName) == 0 &&
        !g_taskbarViewDllLoaded.exchange(true) &&
        ModInitWithTaskbarView(module)) {
        Wh_ApplyHookOperations();
        ApplySettings(g_settings.taskbarHeight);
    }

    return module;
}

BOOL Wh_ModInit() {
    Wh_Log(L">");

    LoadSettings();

    if (!GetTaskbarViewDllPath(g_taskbarViewDllPath)) {
        Wh_Log(L"Taskbar view module not found");
        return FALSE;
    }

    HMODULE taskbarViewModule = LoadLibraryEx(g_taskbarViewDllPath, nullptr,
                                              LOAD_WITH_ALTERED_SEARCH_PATH);
    if (taskbarViewModule) {
        g_taskbarViewDllLoaded = true;
        return ModInitWithTaskbarView(taskbarViewModule);
    }

    Wh_Log(L"Taskbar view module not loaded yet");

    HMODULE kernelBaseModule = GetModuleHandle(L"kernelbase.dll");
    FARPROC pKernelBaseLoadLibraryExW =
        GetProcAddress(kernelBaseModule, "LoadLibraryExW");
    Wh_SetFunctionHook((void*)pKernelBaseLoadLibraryExW,
                       (void*)LoadLibraryExW_Hook,
                       (void**)&LoadLibraryExW_Original);

    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L">");

    if (g_taskbarViewDllLoaded) {
        ApplySettings(g_settings.taskbarHeight);
    } else {
        HMODULE taskbarViewModule = LoadLibraryEx(
            g_taskbarViewDllPath, nullptr, LOAD_WITH_ALTERED_SEARCH_PATH);
        if (taskbarViewModule && !g_taskbarViewDllLoaded.exchange(true) &&
            ModInitWithTaskbarView(taskbarViewModule)) {
            Wh_ApplyHookOperations();
            ApplySettings(g_settings.taskbarHeight);
        }
    }
}

void Wh_ModBeforeUninit() {
    Wh_Log(L">");

    g_unloading = true;

    if (g_taskbarViewDllLoaded) {
        ApplySettings(g_originalTaskbarHeight ? g_originalTaskbarHeight : 48);
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

    while (g_hookCallCounter > 0) {
        Sleep(100);
    }
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");

    int oldTaskbarButtonWidth = g_settings.taskbarButtonWidth;

    LoadSettings();

    if (g_taskbarViewDllLoaded) {
        ApplySettings(g_settings.taskbarHeight);
    }

    if (g_settings.taskbarButtonWidth != oldTaskbarButtonWidth) {
        PromptForExplorerRestart();
    }
}
