// ==WindhawkMod==
// @id              taskbar-icon-size
// @name            Taskbar height, width and icon size
// @description     Control the taskbar height and icon size, improve icon quality (Windows 11 only)
// @version         1.1
// @author          m417z
// @github          https://github.com/m417z
// @twitter         https://twitter.com/m417z
// @homepage        https://m417z.com/
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -DWINVER=0x0605 -loleaut32 -lole32 -lruntimeobject
// ==/WindhawkMod==

// Source code is published under The GNU General Public License v3.0.

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
*Icon size: 24x24, taskbar height: 48 (default)*

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
- StartButtonWidth: 52
  $name: Start button width
  $description: >-
    The width, in pixels, of the start button
- TaskbarHeight: 50
  $name: Taskbar height
  $description: >-
    The height, in pixels, of the taskbar (Windows 11 default: 48)
- TaskbarItemWidth: 52
  $name: Icon width
  $description: >-
    The width in pixels of icons on the taskbar
- NonTaskItemAlignment: center
  $name: Start button alignment
  $options:
  - left: Left aligned
  - right: Right aligned
  - center: Centered
  - stretch: Stretched
  $description: >-
    The alignment to apply to the start button
- LeftAndRightPaddingSize: 0
  $name: Left and right padding
  $description: >-
    Left and right padding size to apply to the start button
*/
// ==/WindhawkModSettings==
#undef GetCurrentTime

#include <initguid.h>  // must come before knownfolders.h

#include <knownfolders.h>
#include <shlobj.h>

#include <winrt/Windows.UI.Xaml.Media.h>

#include <algorithm>
#include <atomic>
#include <limits>
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
    int taskbarItemWidth;
    int startButtonWidth;
    HorizontalAlignment nonTaskItemAlignment;
    int leftAndRightPaddingSize;
} g_settings;

WCHAR g_taskbarViewDllPath[MAX_PATH];
std::atomic<bool> g_taskbarViewDllLoaded = false;
std::atomic<bool> g_applyingSettings = false;
std::atomic<bool> g_unloading = false;

int g_originalTaskbarHeight;
int g_taskbarHeight;
double g_initialTaskbarItemWidth;

double* double_48_value_Original;

WINUSERAPI UINT WINAPI GetDpiForWindow(HWND hwnd);

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

using TaskbarConfiguration_GetIconHeightInViewPixels_taskbarSizeEnum_t =
    double(WINAPI*)(int enumTaskbarSize);
TaskbarConfiguration_GetIconHeightInViewPixels_taskbarSizeEnum_t
    TaskbarConfiguration_GetIconHeightInViewPixels_taskbarSizeEnum_Original;
double WINAPI
TaskbarConfiguration_GetIconHeightInViewPixels_taskbarSizeEnum_Hook(
    int enumTaskbarSize) {
    if (!g_unloading) {
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

    if (g_taskbarHeight) {
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

    if (g_taskbarHeight) {
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

    double ret = TaskbarConfiguration_GetFrameSize_Original(enumTaskbarSize);

    if (!g_originalTaskbarHeight) {
        g_originalTaskbarHeight = ret;
    }

    if (g_taskbarHeight) {
        return g_taskbarHeight;
    }

    return ret;
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
    PCWSTR runningIndicatorStyle =
        Wh_GetStringSetting(L"NonTaskItemAlignment");
    g_settings.nonTaskItemAlignment = HorizontalAlignment::Center;
    if (wcscmp(runningIndicatorStyle, L"left") == 0) {
        g_settings.nonTaskItemAlignment = HorizontalAlignment::Left;
    } else if (wcscmp(runningIndicatorStyle, L"right") == 0) {
        g_settings.nonTaskItemAlignment = HorizontalAlignment::Right;
    } else if (wcscmp(runningIndicatorStyle, L"stretch") == 0) {
        g_settings.nonTaskItemAlignment = HorizontalAlignment::Stretch;
    }
    Wh_FreeStringSetting(runningIndicatorStyle);

    g_settings.startButtonWidth = Wh_GetIntSetting(L"StartButtonWidth");
    g_settings.leftAndRightPaddingSize = Wh_GetIntSetting(L"LeftAndRightPaddingSize");
    g_settings.iconSize = Wh_GetIntSetting(L"IconSize");
    g_settings.taskbarHeight = Wh_GetIntSetting(L"TaskbarHeight");
    g_settings.taskbarItemWidth = Wh_GetIntSetting(L"TaskbarItemWidth");
}

void FreeSettings() {
    // Nothing for now.
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


void RecalculateWidth() {
    HWND hTaskbarWnd = GetTaskbarWnd();
    if (!hTaskbarWnd) {
        return;
    }

    HWND hReBarWindow32 =
        FindWindowEx(hTaskbarWnd, nullptr, L"ReBarWindow32", nullptr);
    if (!hReBarWindow32) {
        return;
    }

    HWND hMSTaskSwWClass =
        FindWindowEx(hReBarWindow32, nullptr, L"MSTaskSwWClass", nullptr);
    if (!hMSTaskSwWClass) {
        return;
    }

    // Trigger CTaskBand::_HandleSyncDisplayChange.
    SendMessage(hMSTaskSwWClass, 0x452, 3, 0);
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

    using TryGetElement_t =
        void*(WINAPI*)(void* pThis, int index, void** uiElement);

    void** vtable = *(void***)winrt::get_abi(pThis);
    auto TryGetElement = (TryGetElement_t)vtable[20];

    void* uiElement = nullptr;
    TryGetElement(winrt::get_abi(pThis), index, &uiElement);

    return UIElement{uiElement, winrt::take_ownership_from_abi}
        .try_as<FrameworkElement>();
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
        RECT taskbarRect{};
        GetWindowRect(hTaskbarWnd, &taskbarRect);

        // Temporarily change the height to force a UI refresh.
        g_taskbarHeight = taskbarHeight - 1;
        if (!TaskbarConfiguration_GetFrameSize_Original) {
            double tempTaskbarHeight = g_taskbarHeight;
            ProtectAndMemcpy(PAGE_READWRITE, double_48_value_Original,
                             &tempTaskbarHeight, sizeof(double));
        }

        // Trigger TrayUI::_HandleSettingChange.
        SendMessage(hTaskbarWnd, WM_SETTINGCHANGE, SPI_SETLOGICALDPIOVERRIDE,
                    0);

        // Wait for the change to apply.
        RECT newTaskbarRect{};
        int counter = 0;
        while (GetWindowRect(hTaskbarWnd, &newTaskbarRect) &&
               newTaskbarRect.top == taskbarRect.top) {
            if (++counter >= 100) {
                break;
            }
            Sleep(100);
        }
    }

    g_taskbarHeight = taskbarHeight;
    if (!TaskbarConfiguration_GetFrameSize_Original) {
        double tempTaskbarHeight = g_taskbarHeight;
        ProtectAndMemcpy(PAGE_READWRITE, double_48_value_Original,
                         &tempTaskbarHeight, sizeof(double));
    }


    // Trigger TrayUI::_HandleSettingChange.
    SendMessage(hTaskbarWnd, WM_SETTINGCHANGE, SPI_SETLOGICALDPIOVERRIDE, 0);

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

    RecalculateWidth();    

    // Sometimes, the height doesn't fully apply at this point, and there's
    // still a transparent line at the bottom of the taskbar. Triggering
    // TrayUI::_HandleSettingChange again works as a workaround.
    Sleep(100);
    SendMessage(hTaskbarWnd, WM_SETTINGCHANGE, SPI_SETLOGICALDPIOVERRIDE, 0);
    
    g_applyingSettings = false;
}

FrameworkElement FindChildByName(FrameworkElement element, PCWSTR name, bool doLog = false) {
    int childrenCount = Media::VisualTreeHelper::GetChildrenCount(element);

    for (int i = 0; i < childrenCount; i++) {
        auto child = Media::VisualTreeHelper::GetChild(element, i)
                         .try_as<FrameworkElement>();
        if (!child) {
            Wh_Log(L"Failed to get child %d of %d", i + 1, childrenCount);
            continue;
        }
        if (doLog) {
            Wh_Log(L"%s", child.Name().c_str());
        }       
        if (child.Name() == name) {
            return child;
        }
    }

    return nullptr;
}

void UpdateExperiencePanelWidth(FrameworkElement taskListButtonElement, double widthToSet) {
    auto experiencePanelElement = FindChildByName(taskListButtonElement, L"ExperienceToggleButtonRootPanel");

    if (experiencePanelElement) {                

        auto parent = Media::VisualTreeHelper::GetParent(taskListButtonElement);
        auto firstChild = Media::VisualTreeHelper::GetChild(parent, 0).try_as<FrameworkElement>();
        bool isStartButton = firstChild == taskListButtonElement;

        auto iconElement = FindChildByName(experiencePanelElement, L"Icon", true);
        if (iconElement && isStartButton) {        
            iconElement.Margin(Thickness{ .Left = static_cast<double>(g_settings.leftAndRightPaddingSize), .Right = static_cast<double>(g_settings.leftAndRightPaddingSize)});    
            iconElement.HorizontalAlignment(g_settings.nonTaskItemAlignment);
            if (!g_unloading)
                widthToSet = g_settings.startButtonWidth;
        }
        taskListButtonElement.Width(widthToSet);
        experiencePanelElement.MinWidth(widthToSet);
    }
    return;
}

void UpdateTaskListButtonWidth(FrameworkElement taskListButtonElement, double widthToSet) {
    auto iconPanelElement =
        FindChildByName(taskListButtonElement, L"IconPanel");

    if (!iconPanelElement) {
        UpdateExperiencePanelWidth(taskListButtonElement, widthToSet);

        return;
    }        

    // Reset in case an old version of the mod was installed.
    taskListButtonElement.Width(std::numeric_limits<double>::quiet_NaN());

    iconPanelElement.Width(widthToSet);
}

void UpdateTaskListButtonCustomizations(
    FrameworkElement taskListButtonElement) {

    auto iconPanelElement =
        FindChildByName(taskListButtonElement, L"IconPanel");
        
    if (!iconPanelElement) {
        iconPanelElement = FindChildByName(taskListButtonElement, L"ExperienceToggleButtonRootPanel");
    }            
    if (!iconPanelElement) {
        return;
    } 

    auto taskbarFrameRepeaterElement =
        Media::VisualTreeHelper::GetParent(taskListButtonElement)
            .as<FrameworkElement>();

    if (!taskbarFrameRepeaterElement ||
        taskbarFrameRepeaterElement.Name() != L"TaskbarFrameRepeater") {
        // Can also be "OverflowFlyoutListRepeater".
        return;
    }

    double taskListButtonWidth = taskListButtonElement.ActualWidth();

    double iconPanelWidth = iconPanelElement.ActualWidth();

    // Check if non-positive or NaN.
    if (!(taskListButtonWidth > 0) || !(iconPanelWidth > 0)) {
        return;
    }

    if (!g_initialTaskbarItemWidth) {
        g_initialTaskbarItemWidth = iconPanelWidth;
    }

    bool adjustWidth = !g_unloading;

    double widthToSet;

    if (adjustWidth) {
        widthToSet = g_settings.taskbarItemWidth;       
    } else {
        widthToSet = g_initialTaskbarItemWidth;
    }

    UpdateTaskListButtonWidth(taskListButtonElement, widthToSet);
}

using TaskListButton_UpdateVisualStates_t = void(WINAPI*)(void* pThis);
TaskListButton_UpdateVisualStates_t TaskListButton_UpdateVisualStates_Original;
void WINAPI TaskListButton_UpdateVisualStates_Hook(void* pThis) {
    Wh_Log(L">");

    TaskListButton_UpdateVisualStates_Original(pThis);

    void* taskListButtonIUnknownPtr = (void**)pThis + 3;
    winrt::Windows::Foundation::IUnknown taskListButtonIUnknown;
    winrt::copy_from_abi(taskListButtonIUnknown, taskListButtonIUnknownPtr);

    auto taskListButtonElement = taskListButtonIUnknown.as<FrameworkElement>();

    UpdateTaskListButtonCustomizations(taskListButtonElement);
}

using TaskListButton_UpdateButtonPadding_t = void(WINAPI*)(void* pThis);
TaskListButton_UpdateButtonPadding_t
    TaskListButton_UpdateButtonPadding_Original;
void WINAPI TaskListButton_UpdateButtonPadding_Hook(void* pThis) {
    Wh_Log(L">");

    TaskListButton_UpdateButtonPadding_Original(pThis);

    void* taskListButtonIUnknownPtr = (void**)pThis + 3;
    winrt::Windows::Foundation::IUnknown taskListButtonIUnknown;
    winrt::copy_from_abi(taskListButtonIUnknown, taskListButtonIUnknownPtr);

    auto taskListButtonElement = taskListButtonIUnknown.as<FrameworkElement>();

    UpdateTaskListButtonCustomizations(taskListButtonElement);
}

using TaskbarFrame_OnTaskbarLayoutChildBoundsChanged_t =
    void(WINAPI*)(void* pThis);
TaskbarFrame_OnTaskbarLayoutChildBoundsChanged_t
    TaskbarFrame_OnTaskbarLayoutChildBoundsChanged_Original;
void WINAPI TaskbarFrame_OnTaskbarLayoutChildBoundsChanged_Hook(void* pThis) {
    Wh_Log(L">");

    TaskbarFrame_OnTaskbarLayoutChildBoundsChanged_Original(pThis);

    void* taskbarFrameIUnknownPtr = (void**)pThis + 3;
    winrt::Windows::Foundation::IUnknown taskbarFrameIUnknown;
    winrt::copy_from_abi(taskbarFrameIUnknown, taskbarFrameIUnknownPtr);

    auto taskbarFrameElement = taskbarFrameIUnknown.as<FrameworkElement>();

    auto taskbarFrameRepeaterContainerElement =
        FindChildByName(taskbarFrameElement, L"RootGrid");
    if (!taskbarFrameRepeaterContainerElement) {
        // For older versions (pre-KB5022913).
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

        //Wh_Log(L"%s", child.Name().c_str());

        if (child.Name() == L"TaskListButton" || child.Name() == L"LaunchListButton") {
            UpdateTaskListButtonCustomizations(child);
        }
    }
}


struct SYMBOL_HOOK {
    std::vector<std::wstring_view> symbols;
    void** pOriginalFunction;
    void* hookFunction = nullptr;
    bool optional = false;
};

bool HookSymbols(HMODULE module,
                 const SYMBOL_HOOK* symbolHooks,
                 size_t symbolHooksCount) {
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
            }
        }

        if (std::all_of(symbolResolved.begin(), symbolResolved.end(),
                        [](bool b) { return b; })) {
            return true;
        }
    }

    Wh_Log(L"Couldn't resolve all symbols from cache");

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
    SYMBOL_HOOK symbolHooks[] = {
        {
            // For Windows 11 version 21H2.
            {LR"(__real@4048000000000000)"},
            (void**)&double_48_value_Original,
        },
        {
            {
                LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskListItemViewModel,struct winrt::Taskbar::ITaskListItemViewModel>::GetIconHeight(void *,double *))",
                LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskListItemViewModel,struct winrt::Taskbar::ITaskListItemViewModel>::GetIconHeight(void * __ptr64,double * __ptr64) __ptr64)",
            },
            (void**)&TaskListItemViewModel_GetIconHeight_Original,
            (void*)TaskListItemViewModel_GetIconHeight_Hook,
        },
        {
            {LR"(public: static double __cdecl winrt::Taskbar::implementation::TaskbarConfiguration::GetIconHeightInViewPixels(enum winrt::WindowsUdk::UI::Shell::TaskbarSize))"},
            (void**)&TaskbarConfiguration_GetIconHeightInViewPixels_taskbarSizeEnum_Original,
            (void*)
                TaskbarConfiguration_GetIconHeightInViewPixels_taskbarSizeEnum_Hook,
        },
        {
            {LR"(public: static double __cdecl winrt::Taskbar::implementation::TaskbarConfiguration::GetIconHeightInViewPixels(double))"},
            (void**)&TaskbarConfiguration_GetIconHeightInViewPixels_double_Original,
            (void*)TaskbarConfiguration_GetIconHeightInViewPixels_double_Hook,
            true,  // From Windows 11 version 22H2.
        },
        {
            {
                LR"(private: double __cdecl winrt::SystemTray::implementation::SystemTrayController::GetFrameSize(enum winrt::WindowsUdk::UI::Shell::TaskbarSize))",
                LR"(private: double __cdecl winrt::SystemTray::implementation::SystemTrayController::GetFrameSize(enum winrt::WindowsUdk::UI::Shell::TaskbarSize) __ptr64)",
            },
            (void**)&SystemTrayController_GetFrameSize_Original,
            (void*)SystemTrayController_GetFrameSize_Hook,
            true,  // From Windows 11 version 22H2.
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
        },
        {
            {
                LR"(private: void __cdecl winrt::Taskbar::implementation::TaskListButton::UpdateVisualStates(void))",
                LR"(private: void __cdecl winrt::Taskbar::implementation::TaskListButton::UpdateVisualStates(void) __ptr64)",
            },
            (void**)&TaskListButton_UpdateVisualStates_Original,
            (void*)TaskListButton_UpdateVisualStates_Hook,
        },
        {
            {
                LR"(private: void __cdecl winrt::Taskbar::implementation::TaskListButton::UpdateButtonPadding(void))",
                LR"(private: void __cdecl winrt::Taskbar::implementation::TaskListButton::UpdateButtonPadding(void) __ptr64)",
            },
            (void**)&TaskListButton_UpdateButtonPadding_Original,
            (void*)TaskListButton_UpdateButtonPadding_Hook,
        }, 
        {
            {
                LR"(private: void __cdecl winrt::Taskbar::implementation::TaskbarFrame::OnTaskbarLayoutChildBoundsChanged(void))",
                LR"(private: void __cdecl winrt::Taskbar::implementation::TaskbarFrame::OnTaskbarLayoutChildBoundsChanged(void) __ptr64)",
            },
            (void**)&TaskbarFrame_OnTaskbarLayoutChildBoundsChanged_Original,
            (void*)TaskbarFrame_OnTaskbarLayoutChildBoundsChanged_Hook,
        }                       
    };

    return HookSymbols(module, symbolHooks, ARRAYSIZE(symbolHooks));
}

bool HookTaskbarDllSymbols() {
    HMODULE module = LoadLibrary(L"taskbar.dll");
    if (!module) {
        Wh_Log(L"Failed to load taskbar.dll");
        return false;
    }

    SYMBOL_HOOK symbolHooks[] = {
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
    };

    return HookSymbols(module, symbolHooks, ARRAYSIZE(symbolHooks));
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
        Sleep(400);
    }
}

void Wh_ModUninit() {
    Wh_Log(L">");

    FreeSettings();
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");

    FreeSettings();
    LoadSettings();

    if (g_taskbarViewDllLoaded) {
        ApplySettings(g_settings.taskbarHeight);
    }
}
