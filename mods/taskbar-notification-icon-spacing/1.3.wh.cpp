// ==WindhawkMod==
// @id              taskbar-notification-icon-spacing
// @name            Taskbar tray icon spacing and grid
// @description     Reduce or increase the spacing between tray icons on the taskbar, optionally have a grid of tray icons (Windows 11 only)
// @version         1.3
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
# Taskbar tray icon spacing and grid

Reduce or increase the spacing between tray icons on the taskbar, optionally
have a grid of tray icons.

Only Windows 11 version 22H2 or newer is currently supported. For older Windows
versions check out [7+ Taskbar Tweaker](https://tweaker.ramensoftware.com/).

![Tray icon width: 32](https://i.imgur.com/BGWZf6x.png) \
*Tray icon width: 32 (Windows 11 default)*

![Tray icon width: 24](https://i.imgur.com/EIyWATk.png) \
*Tray icon width: 24*

![Tray icon width: 18](https://i.imgur.com/MPi1F3m.png) \
*Tray icon width: 18*

![Tray icon width: 18, rows: 2](https://i.imgur.com/zOUUTmb.png) \
*Tray icon width: 18, rows: 2*
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- notificationIconWidth: 24
  $name: Tray icon width
  $description: 'Windows 11 default: 32'
- notificationIconRows: 1
  $name: Tray icon rows
  $description: >-
    Allows having a grid of tray icons
- gridArrangement: rowFirstLeftToRight
  $name: Grid arrangement
  $description: >-
    The order in which tray icons are arranged when using multiple rows.
    Row-first fills each row before moving to the next.
    Column-first fills each column before moving to the next.
    Examples with icons A-G and 2 rows:

      Row-first, left-to-right:
        A B C D
        E F G

      Column-first, top-to-bottom:
        A C E G
        B D F

      Row-first, bottom row first:
        E F G
        A B C D

      Column-first, bottom-to-top:
        B D F
        A C E G
  $options:
  - rowFirstLeftToRight: Row-first, left-to-right
  - columnFirstTopToBottom: Column-first, top-to-bottom
  - rowFirstBottomRowFirst: Row-first, bottom row first
  - columnFirstBottomToTop: Column-first, bottom-to-top
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

#include <atomic>
#include <functional>
#include <list>

#undef GetCurrentTime

#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/base.h>

using namespace winrt::Windows::UI::Xaml;

enum class GridArrangement {
    rowFirstLeftToRight,
    columnFirstTopToBottom,
    rowFirstBottomRowFirst,
    columnFirstBottomToTop,
};

struct {
    int notificationIconWidth;
    int notificationIconRows;
    GridArrangement gridArrangement;
    int overflowIconWidth;
    int overflowIconsPerRow;
} g_settings;

std::atomic<bool> g_taskbarViewDllLoaded;
std::atomic<bool> g_unloading;

using FrameworkElementLoadedEventRevoker = winrt::impl::event_revoker<
    IFrameworkElement,
    &winrt::impl::abi<IFrameworkElement>::type::remove_Loaded>;

std::list<FrameworkElementLoadedEventRevoker> g_autoRevokerList;

winrt::weak_ref<FrameworkElement> g_notificationAreaIconsStackPanel;
winrt::weak_ref<FrameworkElement> g_overflowRootGrid;

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

void ApplyNotifyIconsStackPanelGridStyle(FrameworkElement stackPanel,
                                         int rows,
                                         int width) {
    double itemHeight = 0;
    if (rows > 1) {
        double stackPanelHeight = stackPanel.ActualHeight();
        double gap = stackPanelHeight - 16 * rows;
        double gapPerItem = std::max(gap, 0.0) / (rows + 1);
        // Force the gap to be an even number to prevent blurry icons.
        int gapPerItemEven = static_cast<int>(gapPerItem) / 2 * 2;
        itemHeight = 16 + gapPerItemEven;
    }

    // Count children first for row-first arrangements.
    int childCount = Media::VisualTreeHelper::GetChildrenCount(stackPanel);
    int cols = (childCount + rows - 1) / rows;

    GridArrangement arrangement = g_settings.gridArrangement;

    int indexIter = 0;
    EnumChildElements(stackPanel, [width, rows, itemHeight, cols, arrangement,
                                   &indexIter](FrameworkElement child) {
        int index = indexIter++;

        auto childClassName = winrt::get_class_name(child);
        if (childClassName != L"Windows.UI.Xaml.Controls.ContentPresenter") {
            Wh_Log(L"Unsupported class name %s of child",
                   childClassName.c_str());
            return false;
        }

        if (rows > 1) {
            child.Height(itemHeight);

            int col, row;
            switch (arrangement) {
                case GridArrangement::rowFirstLeftToRight:
                    col = index % cols;
                    row = index / cols;
                    break;
                case GridArrangement::columnFirstTopToBottom:
                    col = index / rows;
                    row = index % rows;
                    break;
                case GridArrangement::rowFirstBottomRowFirst:
                    col = index % cols;
                    row = (rows - 1) - (index / cols);
                    break;
                case GridArrangement::columnFirstBottomToTop:
                    col = index / rows;
                    row = (rows - 1) - (index % rows);
                    break;
            }

            Media::TranslateTransform transform;

            int xOffset = width * (col - index);
            transform.X(xOffset);

            double yOffset = itemHeight * row - itemHeight * (rows - 1) / 2;
            transform.Y(yOffset);

            child.RenderTransform(transform);
        } else {
            auto childDp = child.as<DependencyObject>();
            childDp.ClearValue(FrameworkElement::HeightProperty());
            childDp.ClearValue(UIElement::RenderTransformProperty());
        }

        return false;
    });

    if (rows > 1) {
        int desiredWidth = width * ((indexIter + rows - 1) / rows);
        stackPanel.Width(desiredWidth);
    } else {
        stackPanel.as<DependencyObject>().ClearValue(
            FrameworkElement::WidthProperty());
    }

    g_notificationAreaIconsStackPanel = stackPanel;
}

void ApplyNotifyIconsStackPanelGridStyleOfIcon(
    FrameworkElement notifyIconViewElement,
    int rows,
    int width) {
    auto contentPresenter =
        Media::VisualTreeHelper::GetParent(notifyIconViewElement)
            .try_as<FrameworkElement>();
    if (!contentPresenter || winrt::get_class_name(contentPresenter) !=
                                 L"Windows.UI.Xaml.Controls.ContentPresenter") {
        return;
    }

    auto stackPanel = Media::VisualTreeHelper::GetParent(contentPresenter)
                          .try_as<FrameworkElement>();
    if (!stackPanel || winrt::get_class_name(stackPanel) !=
                           L"Windows.UI.Xaml.Controls.StackPanel") {
        return;
    }

    ApplyNotifyIconsStackPanelGridStyle(stackPanel, rows, width);
}

bool ApplyNotifyIconsStyle(FrameworkElement notificationAreaIcons,
                           int rows,
                           int width) {
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

bool ApplyStyle(XamlRoot xamlRoot, int rows, int width) {
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
            ApplyNotifyIconsStyle(notificationAreaIcons, rows, width);
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

using IconView_IconView_t = void*(WINAPI*)(void* pThis);
IconView_IconView_t IconView_IconView_Original;
void* WINAPI IconView_IconView_Hook(void* pThis) {
    Wh_Log(L">");

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

                    int rows =
                        g_unloading ? 1 : g_settings.notificationIconRows;
                    if (rows > 1) {
                        ApplyNotifyIconsStackPanelGridStyleOfIcon(
                            iconView, rows, g_settings.notificationIconWidth);
                    }
                }
            } else if (className == L"SystemTray.IconView") {
                if (iconView.Name() == L"SystemTrayIcon") {
                    if (IsChildOfElementByName(iconView,
                                               L"ControlCenterButton")) {
                        ApplySystemTrayIconStyle(
                            iconView, g_settings.notificationIconWidth);
                    } else if (IsChildOfElementByName(iconView, L"MainStack") ||
                               IsChildOfElementByName(iconView,
                                                      L"NonActivatableStack")) {
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

    int rows = g_unloading ? 1 : g_settings.notificationIconRows;
    if (rows > 1) {
        if (auto stackPanel = g_notificationAreaIconsStackPanel.get()) {
            ApplyNotifyIconsStackPanelGridStyle(
                stackPanel, rows, g_settings.notificationIconWidth);
        }
    }
}

void* CTaskBand_ITaskListWndSite_vftable;

using CTaskBand_GetTaskbarHost_t = void*(WINAPI*)(void* pThis, void** result);
CTaskBand_GetTaskbarHost_t CTaskBand_GetTaskbarHost_Original;

void* TaskbarHost_FrameHeight_Original;

using std__Ref_count_base__Decref_t = void(WINAPI*)(void* pThis);
std__Ref_count_base__Decref_t std__Ref_count_base__Decref_Original;

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
    g_settings.notificationIconWidth =
        std::max(Wh_GetIntSetting(L"notificationIconWidth"), 1);
    g_settings.notificationIconRows =
        std::max(Wh_GetIntSetting(L"notificationIconRows"), 1);

    PCWSTR gridArrangement = Wh_GetStringSetting(L"gridArrangement");
    g_settings.gridArrangement = GridArrangement::rowFirstLeftToRight;
    if (wcscmp(gridArrangement, L"columnFirstTopToBottom") == 0) {
        g_settings.gridArrangement = GridArrangement::columnFirstTopToBottom;
    } else if (wcscmp(gridArrangement, L"rowFirstBottomRowFirst") == 0) {
        g_settings.gridArrangement = GridArrangement::rowFirstBottomRowFirst;
    } else if (wcscmp(gridArrangement, L"columnFirstBottomToTop") == 0) {
        g_settings.gridArrangement = GridArrangement::columnFirstBottomToTop;
    }
    Wh_FreeStringSetting(gridArrangement);

    g_settings.overflowIconWidth =
        std::max(Wh_GetIntSetting(L"overflowIconWidth"), 1);
    g_settings.overflowIconsPerRow =
        std::max(Wh_GetIntSetting(L"overflowIconsPerRow"), 1);
}

void ApplySettings() {
    struct ApplySettingsParam {
        HWND hTaskbarWnd;
        int rows;
        int width;
    };

    Wh_Log(L"Applying settings");

    HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();
    if (!hTaskbarWnd) {
        Wh_Log(L"No taskbar found");
        return;
    }

    ApplySettingsParam param{
        .hTaskbarWnd = hTaskbarWnd,
        .rows = g_unloading ? 1 : g_settings.notificationIconRows,
        .width = g_unloading ? 32 : g_settings.notificationIconWidth,
    };

    RunFromWindowThread(
        hTaskbarWnd,
        [](void* pParam) {
            ApplySettingsParam& param = *(ApplySettingsParam*)pParam;

            g_autoRevokerList.clear();

            auto xamlRoot = GetTaskbarXamlRoot(param.hTaskbarWnd);
            if (!xamlRoot) {
                Wh_Log(L"Getting XamlRoot failed");
                return;
            }

            if (!ApplyStyle(xamlRoot, param.rows, param.width)) {
                Wh_Log(L"ApplyStyles failed");
            }

            if (auto overflowRootGrid = g_overflowRootGrid.get()) {
                ApplyOverflowStyle(overflowRootGrid);
            }
        },
        &param);
}

bool HookTaskbarViewDllSymbols(HMODULE module) {
    // Taskbar.View.dll
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
        WindhawkUtils::Wh_SetFunctionHookT(pKernelBaseLoadLibraryExW,
                                           LoadLibraryExW_Hook,
                                           &LoadLibraryExW_Original);
    }

    if (!HookTaskbarDllSymbols()) {
        return FALSE;
    }

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

    ApplySettings();
}

void Wh_ModBeforeUninit() {
    Wh_Log(L">");

    g_unloading = true;

    ApplySettings();
}

void Wh_ModUninit() {
    Wh_Log(L">");
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");

    LoadSettings();

    ApplySettings();
}
