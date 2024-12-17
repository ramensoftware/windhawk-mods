// ==WindhawkMod==
// @id              taskbar-notification-icon-spacing
// @name            Taskbar tray icon spacing
// @description     Reduce or increase the spacing between tray icons on the taskbar (Windows 11 only)
// @version         1.1
// @author          m417z
// @github          https://github.com/m417z
// @twitter         https://twitter.com/m417z
// @homepage        https://m417z.com/
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject
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
# Taskbar tray icon spacing

Reduce or increase the spacing between tray icons on the taskbar.

Only Windows 11 version 22H2 or newer is currently supported. For older Windows
versions check out [7+ Taskbar Tweaker](https://tweaker.ramensoftware.com/).

![Tray icon width: 32](https://i.imgur.com/78eRcAJ.png) \
*Tray icon width: 32 (Windows 11 default)*

![Tray icon width: 24](https://i.imgur.com/4hgxHJ0.png) \
*Tray icon width: 24*

![Tray icon width: 18](https://i.imgur.com/cErw24I.png) \
*Tray icon width: 18*
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- notificationIconWidth: 24
  $name: Tray icon width
  $description: 'Windows 11 default: 32'
- overflowIconWidth: 32
  $name: Tray overflow icon width
  $description: >-
    The width of icons that appear in the overflow popup when clicking on the
    chevron icon

    Windows 11 default: 40
- overflowIconsPerRow: 5
  $name: Tray overflow icons per row
  $description: >-
    The maximum amount of icons per row in the overflow popup

    Windows 11 default: 5
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <functional>
#include <list>

#undef GetCurrentTime

#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/base.h>

using namespace winrt::Windows::UI::Xaml;

struct {
    int notificationIconWidth;
    int overflowIconWidth;
    int overflowIconsPerRow;
} g_settings;

using FrameworkElementLoadedEventRevoker = winrt::impl::event_revoker<
    IFrameworkElement,
    &winrt::impl::abi<IFrameworkElement>::type::remove_Loaded>;

std::list<FrameworkElementLoadedEventRevoker> g_autoRevokerList;

bool g_overflowApplied;

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

bool ApplyNotifyIconsStyle(FrameworkElement notificationAreaIcons, int width) {
    FrameworkElement stackPanel = nullptr;

    FrameworkElement child = notificationAreaIcons;
    if ((child = FindChildByClassName(
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

        FrameworkElement notifyIconViewElement =
            FindChildByName(child, L"NotifyItemIcon");
        if (!notifyIconViewElement) {
            Wh_Log(L"Failed to get notifyIconViewElement of child");
            return false;
        }

        ApplyNotifyIconViewStyle(notifyIconViewElement, width);
        return false;
    });

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
                         int width) {
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

            ApplyNotifyIconViewStyle(systemTrayIconElement, width);
        }

        return false;
    });

    return true;
}

bool ApplyStyle(XamlRoot xamlRoot, int width) {
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

    bool somethingSucceeded = false;

    FrameworkElement notificationAreaIcons =
        FindChildByName(systemTrayFrameGrid, L"NotificationAreaIcons");
    if (notificationAreaIcons) {
        somethingSucceeded |=
            ApplyNotifyIconsStyle(notificationAreaIcons, width);
    }

    FrameworkElement controlCenterButton =
        FindChildByName(systemTrayFrameGrid, L"ControlCenterButton");
    if (controlCenterButton) {
        somethingSucceeded |=
            ApplyControlCenterButtonStyle(controlCenterButton, width);
    }

    for (PCWSTR containerName : {
             L"NotifyIconStack",
             L"MainStack",
             L"NonActivatableStack",
         }) {
        FrameworkElement container =
            FindChildByName(systemTrayFrameGrid, containerName);
        if (container) {
            somethingSucceeded |=
                ApplyIconStackStyle(containerName, container, width);
        }
    }

    return somethingSucceeded;
}

using IconView_IconView_t = void(WINAPI*)(PVOID pThis);
IconView_IconView_t IconView_IconView_Original;
void WINAPI IconView_IconView_Hook(PVOID pThis) {
    Wh_Log(L">");

    IconView_IconView_Original(pThis);

    FrameworkElement iconView = nullptr;
    ((IUnknown**)pThis)[1]->QueryInterface(winrt::guid_of<FrameworkElement>(),
                                           winrt::put_abi(iconView));
    if (!iconView) {
        return;
    }

    g_autoRevokerList.emplace_back();
    auto autoRevokerIt = g_autoRevokerList.end();
    --autoRevokerIt;

    *autoRevokerIt = iconView.Loaded(
        winrt::auto_revoke_t{},
        [autoRevokerIt](winrt::Windows::Foundation::IInspectable const& sender,
                        RoutedEventArgs const& e) {
            Wh_Log(L">");

            g_autoRevokerList.erase(autoRevokerIt);

            auto iconView = sender.try_as<FrameworkElement>();
            if (!iconView) {
                return;
            }

            auto className = winrt::get_class_name(iconView);
            Wh_Log(L"className: %s", className.c_str());

            if (className == L"SystemTray.NotifyIconView") {
                if (IsChildOfElementByClassName(
                        iconView, L"SystemTray.NotificationAreaOverflow")) {
                    ApplyNotifyIconViewOverflowStyle(
                        iconView, g_settings.overflowIconWidth);
                } else {
                    ApplyNotifyIconViewStyle(iconView,
                                             g_settings.notificationIconWidth);
                }
            } else if (className == L"SystemTray.IconView") {
                if (iconView.Name() == L"SystemTrayIcon") {
                    if (IsChildOfElementByName(iconView,
                                               L"ControlCenterButton")) {
                        ApplySystemTrayIconStyle(
                            iconView, g_settings.notificationIconWidth);
                    } else if (IsChildOfElementByName(iconView, L"MainStack") ||
                               IsChildOfElementByName(iconView,
                                                      L"NonActivatableStack") ||
                               IsChildOfElementByName(iconView,
                                                      L"ControlCenterButton")) {
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
}

using OverflowXamlIslandManager_ShowWindow_t =
    void(WINAPI*)(void* pThis, POINT pt, int inputDeviceKind);
OverflowXamlIslandManager_ShowWindow_t
    OverflowXamlIslandManager_ShowWindow_Original;
void WINAPI OverflowXamlIslandManager_ShowWindow_Hook(void* pThis,
                                                      POINT pt,
                                                      int inputDeviceKind) {
    Wh_Log(L">");

    OverflowXamlIslandManager_ShowWindow_Original(pThis, pt, inputDeviceKind);

    if (g_overflowApplied) {
        return;
    }

    g_overflowApplied = true;

    FrameworkElement overflowRootGrid = nullptr;
    ((IUnknown**)pThis)[5]->QueryInterface(winrt::guid_of<Controls::Grid>(),
                                           winrt::put_abi(overflowRootGrid));
    if (!overflowRootGrid) {
        return;
    }

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

    int width = g_settings.overflowIconWidth;

    Wh_Log(L"Setting ItemWidth, ItemWidth=%d for WrapGrid", width);
    wrapGrid.ItemWidth(width);
    wrapGrid.ItemHeight(width);

    wrapGrid.MaximumRowsOrColumns(g_settings.overflowIconsPerRow);

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

void* CTaskBand_ITaskListWndSite_vftable;

using CTaskBand_GetTaskbarHost_t = PVOID(WINAPI*)(PVOID pThis, PVOID* result);
CTaskBand_GetTaskbarHost_t CTaskBand_GetTaskbarHost_Original;

using std__Ref_count_base__Decref_t = void(WINAPI*)(PVOID pThis);
std__Ref_count_base__Decref_t std__Ref_count_base__Decref_Original;

XamlRoot GetTaskbarXamlRoot(HWND hTaskbarWnd) {
    HWND hTaskSwWnd = (HWND)GetProp(hTaskbarWnd, L"TaskbandHWND");
    if (!hTaskSwWnd) {
        return nullptr;
    }

    PVOID taskBand = (PVOID)GetWindowLongPtr(hTaskSwWnd, 0);
    PVOID taskBandForTaskListWndSite = taskBand;
    for (int i = 0; *(PVOID*)taskBandForTaskListWndSite !=
                    CTaskBand_ITaskListWndSite_vftable;
         i++) {
        if (i == 20) {
            return nullptr;
        }

        taskBandForTaskListWndSite = (PVOID*)taskBandForTaskListWndSite + 1;
    }

    PVOID taskbarHostSharedPtr[2]{};
    CTaskBand_GetTaskbarHost_Original(taskBandForTaskListWndSite,
                                      taskbarHostSharedPtr);
    if (!taskbarHostSharedPtr[0] && !taskbarHostSharedPtr[1]) {
        return nullptr;
    }

    // Reference: TaskbarHost::FrameHeight
    constexpr size_t kTaskbarElementIUnknownOffset = 0x40;

    auto* taskbarElementIUnknown =
        *(IUnknown**)((BYTE*)taskbarHostSharedPtr[0] +
                      kTaskbarElementIUnknownOffset);

    FrameworkElement taskbarElement = nullptr;
    taskbarElementIUnknown->QueryInterface(winrt::guid_of<FrameworkElement>(),
                                           winrt::put_abi(taskbarElement));

    auto result = taskbarElement ? taskbarElement.XamlRoot() : nullptr;

    std__Ref_count_base__Decref_Original(taskbarHostSharedPtr[1]);

    return result;
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
        [](int nCode, WPARAM wParam, LPARAM lParam) WINAPI -> LRESULT {
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
    g_settings.notificationIconWidth =
        Wh_GetIntSetting(L"notificationIconWidth");
    g_settings.overflowIconWidth = Wh_GetIntSetting(L"overflowIconWidth");
    g_settings.overflowIconsPerRow = Wh_GetIntSetting(L"overflowIconsPerRow");
}

void ApplySettings(int width) {
    struct ApplySettingsParam {
        HWND hTaskbarWnd;
        int width;
    };

    Wh_Log(L"Applying settings: %d", width);

    HWND hTaskbarWnd = GetTaskbarWnd();
    if (!hTaskbarWnd) {
        Wh_Log(L"No taskbar found");
        return;
    }

    ApplySettingsParam param{
        .hTaskbarWnd = hTaskbarWnd,
        .width = width,
    };

    RunFromWindowThread(
        hTaskbarWnd,
        [](PVOID pParam) WINAPI {
            ApplySettingsParam& param = *(ApplySettingsParam*)pParam;

            g_autoRevokerList.clear();
            g_overflowApplied = false;

            auto xamlRoot = GetTaskbarXamlRoot(param.hTaskbarWnd);
            if (!xamlRoot) {
                Wh_Log(L"Getting XamlRoot failed");
                return;
            }

            if (!ApplyStyle(xamlRoot, param.width)) {
                Wh_Log(L"ApplyStyles failed");
            }
        },
        &param);
}

bool HookTaskbarViewDllSymbols() {
    WCHAR dllPath[MAX_PATH];
    if (!GetWindowsDirectory(dllPath, ARRAYSIZE(dllPath))) {
        Wh_Log(L"GetWindowsDirectory failed");
        return false;
    }

    wcscat_s(
        dllPath, MAX_PATH,
        LR"(\SystemApps\MicrosoftWindows.Client.Core_cw5n1h2txyewy\Taskbar.View.dll)");

    HMODULE module =
        LoadLibraryEx(dllPath, nullptr, LOAD_WITH_ALTERED_SEARCH_PATH);
    if (!module) {
        Wh_Log(L"Taskbar view module couldn't be loaded");
        return false;
    }

    // Taskbar.View.dll
    WindhawkUtils::SYMBOL_HOOK symbolHooks[] = {
        {
            {LR"(public: __cdecl winrt::SystemTray::implementation::IconView::IconView(void))"},
            &IconView_IconView_Original,
            IconView_IconView_Hook,
        },
        {
            {LR"(private: void __cdecl winrt::SystemTray::OverflowXamlIslandManager::ShowWindow(struct tagPOINT,enum winrt::WindowsUdk::UI::Shell::InputDeviceKind))"},
            &OverflowXamlIslandManager_ShowWindow_Original,
            OverflowXamlIslandManager_ShowWindow_Hook,
        },
    };

    return HookSymbols(module, symbolHooks, ARRAYSIZE(symbolHooks));
}

BOOL HookTaskbarDllSymbols() {
    HMODULE module = LoadLibrary(L"taskbar.dll");
    if (!module) {
        Wh_Log(L"Failed to load taskbar.dll");
        return FALSE;
    }

    WindhawkUtils::SYMBOL_HOOK taskbarDllHooks[] = {
        {
            {LR"(const CTaskBand::`vftable'{for `ITaskListWndSite'})"},
            (void**)&CTaskBand_ITaskListWndSite_vftable,
        },
        {
            {LR"(public: virtual class std::shared_ptr<class TaskbarHost> __cdecl CTaskBand::GetTaskbarHost(void)const )"},
            (void**)&CTaskBand_GetTaskbarHost_Original,
        },
        {
            {LR"(public: void __cdecl std::_Ref_count_base::_Decref(void))"},
            (void**)&std__Ref_count_base__Decref_Original,
        },
    };

    return HookSymbols(module, taskbarDllHooks, ARRAYSIZE(taskbarDllHooks));
}

BOOL Wh_ModInit() {
    Wh_Log(L">");

    LoadSettings();

    if (!HookTaskbarViewDllSymbols()) {
        return FALSE;
    }

    if (!HookTaskbarDllSymbols()) {
        return FALSE;
    }

    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L">");

    ApplySettings(g_settings.notificationIconWidth);
}

void Wh_ModUninit() {
    Wh_Log(L">");

    ApplySettings(32);
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");

    LoadSettings();

    ApplySettings(g_settings.notificationIconWidth);
}
