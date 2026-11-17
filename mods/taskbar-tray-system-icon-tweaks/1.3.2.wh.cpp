// ==WindhawkMod==
// @id              taskbar-tray-system-icon-tweaks
// @name            Taskbar tray system icon tweaks
// @description     Allows hiding system icons: volume, network, battery, microphone, location/GPS, Studio Effects, Recall, language bar, bell (always or when there are no new notifications), and the "Show desktop" button (hide or set width)
// @version         1.3.2
// @author          m417z
// @github          https://github.com/m417z
// @twitter         https://twitter.com/m417z
// @homepage        https://m417z.com/
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject -lversion
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
# Taskbar tray system icon tweaks

Allows hiding system icons: volume, network, battery, microphone, location/GPS,
Studio Effects, Recall, language bar, bell (always or when there are no new
notifications), and the "Show desktop" button (hide or set width). Also allows
showing the battery icon in grayscale instead of its colored variant.

Only Windows 11 is supported.

![Demonstration](https://i.imgur.com/YfO676m.gif)
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- hideVolumeIcon: false
  $name: Hide volume icon
- hideNetworkIcon: false
  $name: Hide network icon
- hideBatteryIcon: false
  $name: Hide battery icon
- grayscaleBatteryIcon: false
  $name: Grayscale battery icon
  $description: >-
    Show the battery icon using the standard text color instead of its colored
    (e.g. green/yellow) variant.
- hideMicrophoneIcon: false
  $name: Hide microphone icon
- hideGeolocationIcon: false
  $name: Hide location (e.g. GPS) icon
- hideStudioEffectsIcon: false
  $name: Hide Studio Effects icon
- hideRecallIcon: false
  $name: Hide Recall icon
- hideLanguageBar: false
  $name: Hide language bar
- hideLanguageSupplementaryIcons: false
  $name: Hide language supplementary icons
- hideBellIcon: never
  $name: Hide bell icon
  $options:
  - never: Never
  - whenInactive: When there are no new notifications
  - whenInactiveAndNoDnd: >-
      When there are no new notifications and "Do not disturb" is off
  - always: Always
- showDesktopButtonWidth: 12
  $name: '"Show desktop" button width'
  $description: With the vertical taskbar, sets the button height.
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <algorithm>
#include <atomic>
#include <functional>
#include <list>
#include <optional>
#include <string>
#include <vector>

#undef GetCurrentTime

#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.Xaml.Automation.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/base.h>

using namespace winrt::Windows::UI::Xaml;

enum class HideBellIcon {
    never,
    whenInactive,
    whenInactiveAndNoDnd,
    always,
};

struct {
    bool hideVolumeIcon;
    bool hideNetworkIcon;
    bool hideBatteryIcon;
    bool grayscaleBatteryIcon;
    bool hideMicrophoneIcon;
    bool hideGeolocationIcon;
    bool hideStudioEffectsIcon;
    bool hideRecallIcon;
    bool hideLanguageBar;
    bool hideLanguageSupplementaryIcons;
    HideBellIcon hideBellIcon;
    int showDesktopButtonWidth;
} g_settings;

std::atomic<bool> g_systemTrayModuleHooked;
std::atomic<bool> g_unloading;

using FrameworkElementLoadedEventRevoker = winrt::impl::event_revoker<
    IFrameworkElement,
    &winrt::impl::abi<IFrameworkElement>::type::remove_Loaded>;

[[clang::no_destroy]] std::optional<
    std::list<FrameworkElementLoadedEventRevoker>> g_autoRevokerList{
    std::in_place};

struct MainStackTextBlockState {
    winrt::weak_ref<Controls::TextBlock> textBlock;
    int64_t textChangedToken{0};
};

std::vector<MainStackTextBlockState> g_mainStackTextBlockStates;

struct BellIconState {
    winrt::weak_ref<FrameworkElement> element;
    int64_t automationNameChangedToken{0};
};

std::vector<BellIconState> g_bellIconStates;

winrt::weak_ref<FrameworkElement> g_controlCenterPanel;
winrt::event_token g_controlCenterPanelSizeChangedToken;

struct BatteryTextBlockState {
    winrt::weak_ref<Controls::TextBlock> textBlock;
    Media::Brush savedForeground{nullptr};
    int64_t foregroundChangedToken{0};
};

[[clang::no_destroy]] std::optional<std::vector<BatteryTextBlockState>>
    g_batteryTextBlockStates{std::in_place};

// Local MinWidth/MaxWidth values (MinHeight/MaxHeight if vertical) replaced by
// the show desktop size override, nullopt for a property that had no local
// value.
struct ShowDesktopSizeState {
    winrt::weak_ref<FrameworkElement> element;
    bool vertical;
    std::optional<double> savedMin;
    std::optional<double> savedMax;
};

std::vector<ShowDesktopSizeState> g_showDesktopSizeStates;

struct OrientationStatesSubscription {
    winrt::weak_ref<VisualStateGroup> group;
    winrt::event_token currentStateChangedToken;
};

std::vector<OrientationStatesSubscription> g_orientationStatesSubscriptions;

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

FrameworkElement EnumParentElements(
    FrameworkElement element,
    std::function<bool(FrameworkElement)> enumCallback) {
    auto parent = element;
    while (true) {
        parent = Media::VisualTreeHelper::GetParent(parent)
                     .try_as<FrameworkElement>();
        if (!parent) {
            return nullptr;
        }

        if (enumCallback(parent)) {
            return parent;
        }
    }
}

FrameworkElement GetParentElementByName(FrameworkElement element, PCWSTR name) {
    return EnumParentElements(element, [name](FrameworkElement parent) {
        return parent.Name() == name;
    });
}

bool IsChildOfElementByName(FrameworkElement element, PCWSTR name) {
    return !!GetParentElementByName(element, name);
}

FrameworkElement GetParentElementByClassName(FrameworkElement element,
                                             PCWSTR className) {
    return EnumParentElements(element, [className](FrameworkElement parent) {
        return winrt::get_class_name(parent) == className;
    });
}

bool IsChildOfElementByClassName(FrameworkElement element, PCWSTR className) {
    return !!GetParentElementByClassName(element, className);
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

FrameworkElement FindChildPanel(FrameworkElement element) {
    return EnumChildElements(element, [](FrameworkElement child) {
        return !!child.try_as<Controls::Panel>();
    });
}

// The native vertical taskbar switches the tray to a vertical layout with a
// state of this group. A tray which is rotated as a whole keeps the horizontal
// layout.
VisualStateGroup GetOrientationStatesGroup(
    FrameworkElement systemTrayFrameGrid) {
    for (const auto& group :
         VisualStateManager::GetVisualStateGroups(systemTrayFrameGrid)) {
        if (group.Name() == L"OrientationStates") {
            return group;
        }
    }

    return nullptr;
}

bool IsVerticalOrientationState(VisualStateGroup orientationStates) {
    auto currentState =
        orientationStates ? orientationStates.CurrentState() : nullptr;
    return currentState && currentState.Name() == L"VerticalOrientation";
}

// https://stackoverflow.com/a/3382894
std::wstring StringToHex(std::wstring_view input) {
    static const WCHAR kHexDigits[] = L"0123456789ABCDEF";

    std::wstring output;
    output.reserve(input.length() * 5);
    for (WCHAR c : input) {
        output.push_back(kHexDigits[(c >> 12) & 15]);
        output.push_back(kHexDigits[(c >> 8) & 15]);
        output.push_back(kHexDigits[(c >> 4) & 15]);
        output.push_back(kHexDigits[c & 15]);
        output.push_back(L' ');
    }

    if (!output.empty() && output.back() == L' ') {
        output.resize(output.length() - 1);
    }

    return output;
}

enum class SystemTrayIconIdent {
    kUnknown,
    kNone,
    kVolume,
    kNetwork,
    kBattery,
    kMicrophone,
    kGeolocation,
    kMicrophoneAndGeolocation,
    kBellEmpty,
    kBellEmptyDnd,
    kBellFull,
    kBellFullDnd,
    kLanguage,
    kStudioEffects,
    kRecall,
};

SystemTrayIconIdent IdentifySystemTrayIconFromText(std::wstring_view text) {
    switch (text.length()) {
        case 0:
            return SystemTrayIconIdent::kNone;

        case 1:
            break;

        default:
            return SystemTrayIconIdent::kUnknown;
    }

    switch (text[0]) {
        case L'\uE74F':  // Mute
        case L'\uE992':  // Volume0
        case L'\uE993':  // Volume1
        case L'\uE994':  // Volume2
        case L'\uE995':  // Volume3
        case L'\uEA85':  // VolumeDisabled
        case L'\uEBC5':  // VolumeBars
            return SystemTrayIconIdent::kVolume;

        case L'\uE709':  // Airplane
        case L'\uE7F4':  // TVMonitor
        case L'\uE839':  // Ethernet
        case L'\uE86C':  // SignalBars1
        case L'\uE86D':  // SignalBars2
        case L'\uE86E':  // SignalBars3
        case L'\uE86F':  // SignalBars4
        case L'\uE870':  // SignalBars5
        case L'\uEAA1':  // Drinks
        case L'\uEAA2':  // DropShot
        case L'\uEAA3':  // FlavorProfile
        case L'\uEAA4':  // Float
        case L'\uEAA5':  // FluteGlass
        case L'\uEAA8':  // GobletGlass
        case L'\uEC1E':  // SignalRoaming
        case L'\uEC3C':  // MobWifi1
        case L'\uEC3D':  // MobWifi2
        case L'\uEC3E':  // MobWifi3
        case L'\uEC3F':  // MobWifi4
        case L'\uF384':  // NetworkOffline
        case L'\uF8C0':  // SysLocationArrow
        case L'\uF8C1':  // SysMicrophone
        case L'\uF8C2':  // SysVideo
        case L'\uF8C3':  // SysWifi4
        case L'\uF8C4':  // SysWifi3
        case L'\uF8C5':  // SysWifi2
        case L'\uF8C6':  // SysWifi1
        case L'\uF8C7':  // SysSignalBars5
        case L'\uF8C8':  // SysSignalBars4
        case L'\uF8C9':  // SysSignalBars3
        case L'\uF8CA':  // SysSignalBars2
        case L'\uF8CB':  // SysSignalBars1
        case L'\uF8CC':  // SysNetworkOffline
            return SystemTrayIconIdent::kNetwork;

        // Charging levels.
        case L'\uE3C1':  // Private Use
        case L'\uE3C2':  // Private Use
        case L'\uE3C3':  // Private Use
        case L'\uE3C4':  // Private Use
        case L'\uE3C5':  // Private Use
        case L'\uE3C6':  // Private Use
        case L'\uE3C7':  // Private Use
        case L'\uE3C8':  // Private Use
        case L'\uE3C9':  // Private Use
        case L'\uE3CA':  // Private Use
        case L'\uE3CB':  // Private Use
        case L'\uE408':  // Private Use
        case L'\uE409':  // Private Use
        case L'\uE40A':  // Private Use
        case L'\uE40B':  // Private Use
        case L'\uE40C':  // Private Use
        case L'\uE40D':  // Private Use
        case L'\uE40E':  // Private Use
        case L'\uE40F':  // Private Use
        case L'\uE410':  // Private Use
        case L'\uE411':  // Private Use
        case L'\uE412':  // Private Use
        case L'\uE413':  // Private Use
        case L'\uE414':  // Private Use
        case L'\uE415':  // Private Use
        case L'\uE416':  // Private Use
        case L'\uE417':  // Private Use
        case L'\uE418':  // Private Use
        case L'\uE419':  // Private Use
        case L'\uE41A':  // Private Use
        case L'\uE41B':  // Private Use
        case L'\uE41C':  // Private Use
        case L'\uE41D':  // Private Use
        case L'\uEBA0':  // MobBattery0
        case L'\uEBA1':  // MobBattery1
        case L'\uEBA2':  // MobBattery2
        case L'\uEBA3':  // MobBattery3
        case L'\uEBA4':  // MobBattery4
        case L'\uEBA5':  // MobBattery5
        case L'\uEBA6':  // MobBattery6
        case L'\uEBA7':  // MobBattery7
        case L'\uEBA8':  // MobBattery8
        case L'\uEBA9':  // MobBattery9
        case L'\uEBAA':  // MobBattery10
        case L'\uEBAB':  // MobBatteryCharging0
        case L'\uEBAC':  // MobBatteryCharging1
        case L'\uEBAD':  // MobBatteryCharging2
        case L'\uEBAE':  // MobBatteryCharging3
        case L'\uEBAF':  // MobBatteryCharging4
        case L'\uEBB0':  // MobBatteryCharging5
        case L'\uEBB1':  // MobBatteryCharging6
        case L'\uEBB2':  // MobBatteryCharging7
        case L'\uEBB3':  // MobBatteryCharging8
        case L'\uEBB4':  // MobBatteryCharging9
        case L'\uEBB5':  // MobBatteryCharging10
        case L'\uEBB6':  // MobBatterySaver0
        case L'\uEBB7':  // MobBatterySaver1
        case L'\uEBB8':  // MobBatterySaver2
        case L'\uEBB9':  // MobBatterySaver3
        case L'\uEBBA':  // MobBatterySaver4
        case L'\uEBBB':  // MobBatterySaver5
        case L'\uEBBC':  // MobBatterySaver6
        case L'\uEBBD':  // MobBatterySaver7
        case L'\uEBBE':  // MobBatterySaver8
        case L'\uEBBF':  // MobBatterySaver9
        case L'\uEBC0':  // MobBatterySaver10
        // Misc.
        case L'\uEB17':
        case L'\uEC02':
        case L'\uF1E8':
            return SystemTrayIconIdent::kBattery;

        case L'\uE361':  // Private Use
        case L'\uE720':  // Microphone
        case L'\uEC71':  // MicOn
            return SystemTrayIconIdent::kMicrophone;

        case L'\uE37A':
            return SystemTrayIconIdent::kGeolocation;

        case L'\uF47F':
            return SystemTrayIconIdent::kMicrophoneAndGeolocation;

        case L'\uF2A3':  // Empty bell
            return SystemTrayIconIdent::kBellEmpty;

        case L'\uF285':  // Empty bell, Do Not Disturb
            return SystemTrayIconIdent::kBellEmptyDnd;

        case L'\uF2A5':  // Full bell
            return SystemTrayIconIdent::kBellFull;

        case L'\uF2A8':  // Full bell, Do Not Disturb
            return SystemTrayIconIdent::kBellFullDnd;

        // Language supplementary icons.
        // Found by installing all the built-in input methods from:
        // https://learn.microsoft.com/en-us/windows-hardware/manufacture/desktop/windows-language-pack-default-values?view=windows-11#input-method-editors
        // and identifying the icon code in the fonts Segoe Fluent and
        // AXPIcons.ttf.
        // https://learn.microsoft.com/en-us/windows/apps/design/style/segoe-fluent-icons-font
        // %SystemRoot%\SystemApps\MicrosoftWindows.Client.Core_cw5n1h2txyewy\SystemTray\Assets\AXPIcons.ttf
        case L'\uE4D7':  // (Maybe) English Private mode
        case L'\uE4D8':  // (Maybe) Chinese Private mode
        case L'\uE5BF':  // (Maybe) English mode locked
        case L'\uE97E':  // HalfAlpha
        case L'\uE97F':  // FullAlpha
        case L'\uE980':  // Key12On (Korean mode)
        case L'\uE982':  // QWERTYOn (Chinese mode)
        case L'\uE983':  // QWERTYOff (English mode)
        case L'\uE986':  // FullHiragana
        case L'\uE987':  // FullKatakana
        case L'\uE988':  // HalfKatakana
        case L'\uEB90':  // StatusErrorFull (Input method disabled)
        case L'\uEE41':  // FullHiraganaPrivateMode
        case L'\uEE42':  // FullKatakanaPrivateMode
        case L'\uEE43':  // HalfAlphaPrivateMode
        case L'\uEE44':  // HalfKatakanaPrivateMode
        case L'\uEE45':  // FullAlphaPrivateMode
        case L'\uEE75':  // (Maybe) HalfAlpha
        case L'\uEE76':  // (Maybe) HalfAlphaPrivateMode
            return SystemTrayIconIdent::kLanguage;

        case L'\uEABC':
            return SystemTrayIconIdent::kStudioEffects;

        // From:
        // C:\Windows\SystemApps\MicrosoftWindows.Client.Core_cw5n1h2txyewy\SystemTray\Assets\RecallIcons.ttf
        case L'\uEC83':
        case L'\uEADD':
        case L'\uEB16':
        case L'\uEF97':
        case L'\uF1C6':
            return SystemTrayIconIdent::kRecall;
    }

    return SystemTrayIconIdent::kUnknown;
}

// A wrap grid places each item in the cell of its index, so a hidden item keeps
// its cell. Move the visible items with margins to the cells they'd have
// without the hidden ones, in as many columns as the taskbar uses for that many
// items, and trim the rows that are left empty. Returns the number of visible
// items, or nullopt if the items are unknown.
std::optional<int> UpdateWrapGridHiddenItems(
    Controls::WrapGrid wrapGrid,
    std::function<bool(FrameworkElement)> isContainerHidden) {
    auto itemsControl = Controls::ItemsControl::GetItemsOwner(wrapGrid);
    if (!itemsControl) {
        Wh_Log(L"Failed to get items owner");
        return std::nullopt;
    }

    int itemCount = itemsControl.Items().Size();
    auto getContainer = [&itemsControl](int index) {
        return itemsControl.ContainerFromIndex(index)
            .try_as<FrameworkElement>();
    };

    int visibleCount = 0;
    for (int i = 0; i < itemCount; i++) {
        auto container = getContainer(i);
        if (!container || !isContainerHidden(container)) {
            visibleCount++;
        }
    }

    // The default layout is right even if the grid isn't configured for the
    // current items yet, e.g. right after an item is removed.
    if (visibleCount == itemCount) {
        for (int i = 0; i < itemCount; i++) {
            if (auto container = getContainer(i)) {
                container.ClearValue(FrameworkElement::MarginProperty());
            }
        }
        wrapGrid.ClearValue(FrameworkElement::MaxHeightProperty());
        return visibleCount;
    }

    int columns = wrapGrid.MaximumRowsOrColumns();
    double itemWidth = wrapGrid.ItemWidth();
    double itemHeight = wrapGrid.ItemHeight();
    if (wrapGrid.Orientation() != Controls::Orientation::Horizontal ||
        columns < 1 || !(itemWidth > 0) || !(itemHeight > 0)) {
        Wh_Log(L"Unsupported WrapGrid");
        return visibleCount;
    }

    // The taskbar uses a column for each item, up to a maximum.
    int visibleColumns = std::min(visibleCount, columns);
    double visibleItemWidth = visibleColumns == columns
                                  ? itemWidth
                                  : itemWidth * columns / visibleColumns;

    int visibleIndex = 0;
    for (int i = 0; i < itemCount; i++) {
        auto container = getContainer(i);
        if (!container) {
            visibleIndex++;
            continue;
        }

        if (isContainerHidden(container)) {
            container.ClearValue(FrameworkElement::MarginProperty());
            continue;
        }

        int column = i % columns;
        int row = i / columns;
        int visibleColumn = visibleIndex % visibleColumns;
        int visibleRow = visibleIndex / visibleColumns;

        double x = column * itemWidth;
        double y = row * itemHeight;
        double visibleX = visibleColumn * visibleItemWidth;
        double visibleY = visibleRow * itemHeight;
        visibleIndex++;

        if (visibleX == x && visibleY == y && visibleItemWidth == itemWidth) {
            container.ClearValue(FrameworkElement::MarginProperty());
        } else {
            container.Margin(Thickness{
                visibleX - x,
                visibleY - y,
                x + itemWidth - visibleX - visibleItemWidth,
                y - visibleY,
            });
        }
    }

    int rows = (itemCount + columns - 1) / columns;
    int visibleRows = visibleColumns
                          ? (visibleCount + visibleColumns - 1) / visibleColumns
                          : 0;
    if (visibleRows < rows) {
        wrapGrid.MaxHeight(visibleRows * itemHeight);
    } else {
        wrapGrid.ClearValue(FrameworkElement::MaxHeightProperty());
    }

    return visibleCount;
}

void UpdateMainStackWrapGridOfIcon(FrameworkElement notifyIconViewElement) {
    auto contentPresenter =
        Media::VisualTreeHelper::GetParent(notifyIconViewElement);
    auto wrapGrid = contentPresenter
                        ? Media::VisualTreeHelper::GetParent(contentPresenter)
                              .try_as<Controls::WrapGrid>()
                        : nullptr;
    if (!wrapGrid) {
        return;
    }

    UpdateWrapGridHiddenItems(wrapGrid, [](FrameworkElement container) {
        auto iconView = FindChildByName(container, L"SystemTrayIcon");
        return iconView && iconView.Visibility() == Visibility::Collapsed;
    });
}

void ApplyMainStackIconViewStyle(FrameworkElement notifyIconViewElement) {
    FrameworkElement systemTrayTextIconContent = nullptr;

    FrameworkElement child = notifyIconViewElement;
    if ((child = FindChildByName(child, L"ContainerGrid")) &&
        (child = FindChildByName(child, L"ContentPresenter")) &&
        (child = FindChildByName(child, L"ContentGrid")) &&
        (child = FindChildByClassName(child, L"SystemTray.TextIconContent"))) {
        systemTrayTextIconContent = child;
    } else {
        Wh_Log(L"Failed to get SystemTray.TextIconContent");
        return;
    }

    Controls::TextBlock innerTextBlock = nullptr;

    child = systemTrayTextIconContent;
    if ((child = FindChildByName(child, L"ContainerGrid")) &&
        (child = FindChildByName(child, L"Base")) &&
        (child = FindChildByName(child, L"InnerTextBlock"))) {
        innerTextBlock = child.as<Controls::TextBlock>();
    } else {
        Wh_Log(L"Failed to get InnerTextBlock");
        return;
    }

    auto shouldHide = [](Controls::TextBlock innerTextBlock) {
        auto text = innerTextBlock.Text();
        auto systemTrayIconIdent = IdentifySystemTrayIconFromText(text);

        bool hide = false;
        if (!g_unloading) {
            switch (systemTrayIconIdent) {
                case SystemTrayIconIdent::kMicrophone:
                    hide = g_settings.hideMicrophoneIcon;
                    break;

                case SystemTrayIconIdent::kGeolocation:
                    hide = g_settings.hideGeolocationIcon;
                    break;

                case SystemTrayIconIdent::kMicrophoneAndGeolocation:
                    hide = g_settings.hideMicrophoneIcon &&
                           g_settings.hideGeolocationIcon;
                    break;

                case SystemTrayIconIdent::kStudioEffects:
                    hide = g_settings.hideStudioEffectsIcon;
                    break;

                case SystemTrayIconIdent::kRecall:
                    hide = g_settings.hideRecallIcon;
                    break;

                case SystemTrayIconIdent::kNone:
                    // Happens when the icon is about to disappear.
                    break;

                default:
                    Wh_Log(L"Failed");
                    break;
            }
        }

        Wh_Log(L"Main stack icon %d (%s), hide=%d", (int)systemTrayIconIdent,
               StringToHex(text).c_str(), hide);

        return hide;
    };

    bool hide = shouldHide(innerTextBlock);

    notifyIconViewElement.Visibility(hide ? Visibility::Collapsed
                                          : Visibility::Visible);
    UpdateMainStackWrapGridOfIcon(notifyIconViewElement);

    if (g_unloading) {
        return;
    }

    std::erase_if(
        g_mainStackTextBlockStates,
        [](const MainStackTextBlockState& s) { return !s.textBlock.get(); });

    for (const auto& state : g_mainStackTextBlockStates) {
        if (state.textBlock.get() == innerTextBlock) {
            return;
        }
    }

    auto notifyIconViewElementWeakRef = winrt::make_weak(notifyIconViewElement);
    g_mainStackTextBlockStates.push_back({
        .textBlock = innerTextBlock,
        .textChangedToken = innerTextBlock.RegisterPropertyChangedCallback(
            Controls::TextBlock::TextProperty(),
            [notifyIconViewElementWeakRef, shouldHide](
                DependencyObject sender, DependencyProperty property) {
                auto innerTextBlock = sender.try_as<Controls::TextBlock>();
                if (!innerTextBlock) {
                    return;
                }

                auto notifyIconViewElement = notifyIconViewElementWeakRef.get();
                if (!notifyIconViewElement) {
                    return;
                }

                bool hide = shouldHide(innerTextBlock);

                Wh_Log(L"Main stack icon, hide=%d", hide);

                notifyIconViewElement.Visibility(hide ? Visibility::Collapsed
                                                      : Visibility::Visible);
                UpdateMainStackWrapGridOfIcon(notifyIconViewElement);
            }),
    });
}

void ApplyNonActivatableStackIconViewStyle(
    FrameworkElement notifyIconViewElement) {
    FrameworkElement child = notifyIconViewElement;
    bool hide = false;
    if ((child = FindChildByName(child, L"ContainerGrid")) &&
        (child = FindChildByName(child, L"ContentPresenter")) &&
        (child = FindChildByName(child, L"ContentGrid"))) {
        child = EnumChildElements(child, [&hide](FrameworkElement child) {
            auto className = winrt::get_class_name(child);
            if (className == L"SystemTray.TextIconContent") {
                Controls::TextBlock innerTextBlock = nullptr;

                if ((child = FindChildByName(child, L"ContainerGrid")) &&
                    (child = FindChildByName(child, L"Base")) &&
                    (child = FindChildByName(child, L"InnerTextBlock"))) {
                    innerTextBlock = child.as<Controls::TextBlock>();
                } else {
                    Wh_Log(L"Failed to get InnerTextBlock");
                    return false;
                }

                auto text = innerTextBlock.Text();
                auto systemTrayIconIdent = IdentifySystemTrayIconFromText(text);

                if (systemTrayIconIdent == SystemTrayIconIdent::kLanguage) {
                    Wh_Log(L"Language supplementary icon %d (%s)",
                           (int)systemTrayIconIdent, StringToHex(text).c_str());
                    hide = g_settings.hideLanguageSupplementaryIcons;
                    return true;
                } else {
                    Wh_Log(L"Language bar unknown icon %d (%s)",
                           (int)systemTrayIconIdent, StringToHex(text).c_str());
                    return false;
                }
            } else if (className == L"SystemTray.ImageIconContent") {
                Wh_Log(L"Language supplementary icon (image)");
                hide = g_settings.hideLanguageSupplementaryIcons;
                return true;
            } else if (className == L"SystemTray.LanguageTextIconContent" ||
                       className == L"SystemTray.LanguageImageIconContent") {
                Wh_Log(L"Language bar main icon");
                hide = g_settings.hideLanguageBar;
                return true;
            }

            Wh_Log(L"Unsupported class name %s of child", className.c_str());
            return false;
        });
    }

    if (!child) {
        // Some input methods use LanguageImageIconContent/ImageIconContent
        // instead of LanguageTextIconContent/TextIconContent with icon fonts.
        // If the language bar is hidden and the user switches from a "text"
        // input method to a "image" input method, the invisible element will
        // not be populated with the new type of icon content but become empty
        // instead. Then the icon will be permanently hidden even after
        // disabling the mod. This code forces the empty element to become
        // visible and populated, fixing this issue.
        if (Media::VisualTreeHelper::GetChildrenCount(notifyIconViewElement) ==
            0) {
            notifyIconViewElement.Visibility(Visibility::Visible);
        }
        return;
    }

    hide = !g_unloading && hide;

    Wh_Log(L"Language bar, hide=%d", hide);

    notifyIconViewElement.Visibility(hide ? Visibility::Collapsed
                                          : Visibility::Visible);
}

// Path inside SystemTray.BatteryIconContent:
// Grid#ContainerGrid > StackPanel > Grid > TextBlock.
//
// The system sets a colored Foreground brush on each battery TextBlock.
// Clearing the local Foreground value lets the inherited TextFillColorPrimary
// take over, giving a grayscale look. The system re-applies the colored brush
// on theme changes (and similar updates), so we listen for ForegroundProperty
// changes, save the latest value (so we can restore it on disable), and clear
// it again.
void ApplyBatteryIconGrayscaleStyle(FrameworkElement batteryIconContent) {
    bool grayscale = !g_unloading && g_settings.grayscaleBatteryIcon;

    // Drop any state for TextBlocks that no longer exist.
    std::erase_if(
        *g_batteryTextBlockStates,
        [](const BatteryTextBlockState& s) { return !s.textBlock.get(); });

    // Nothing to apply and nothing to restore - skip the tree traversal.
    if (!grayscale && g_batteryTextBlockStates->empty()) {
        return;
    }

    FrameworkElement grid = nullptr;

    FrameworkElement child = batteryIconContent;
    if ((child = FindChildByName(child, L"ContainerGrid")) &&
        (child = FindChildByClassName(
             child, L"Windows.UI.Xaml.Controls.StackPanel")) &&
        (child =
             FindChildByClassName(child, L"Windows.UI.Xaml.Controls.Grid"))) {
        grid = child;
    } else {
        Wh_Log(L"Failed to navigate to battery Grid");
        return;
    }

    EnumChildElements(grid, [grayscale](FrameworkElement textChild) {
        auto textBlock = textChild.try_as<Controls::TextBlock>();
        if (!textBlock) {
            return false;
        }

        auto it = g_batteryTextBlockStates->begin();
        for (; it != g_batteryTextBlockStates->end(); ++it) {
            if (it->textBlock.get() == textBlock) {
                break;
            }
        }
        bool managed = (it != g_batteryTextBlockStates->end());

        if (grayscale && !managed) {
            auto localForeground =
                textBlock
                    .ReadLocalValue(Controls::TextBlock::ForegroundProperty())
                    .try_as<Media::Brush>();
            if (!localForeground) {
                // No local Foreground set, so clearing would be a no-op and
                // there'd be nothing meaningful to restore later.
                return false;
            }

            BatteryTextBlockState state;
            state.textBlock = textBlock;
            state.savedForeground = localForeground;
            state.foregroundChangedToken =
                textBlock.RegisterPropertyChangedCallback(
                    Controls::TextBlock::ForegroundProperty(),
                    [](DependencyObject sender, DependencyProperty) {
                        auto tb = sender.try_as<Controls::TextBlock>();
                        if (!tb) {
                            return;
                        }
                        auto fg =
                            tb.ReadLocalValue(
                                  Controls::TextBlock::ForegroundProperty())
                                .try_as<Media::Brush>();
                        if (!fg) {
                            return;
                        }
                        for (auto& s : *g_batteryTextBlockStates) {
                            if (s.textBlock.get() == tb) {
                                s.savedForeground = fg;
                                break;
                            }
                        }
                        tb.as<DependencyObject>().ClearValue(
                            Controls::TextBlock::ForegroundProperty());
                    });
            g_batteryTextBlockStates->push_back(std::move(state));
            textBlock.as<DependencyObject>().ClearValue(
                Controls::TextBlock::ForegroundProperty());
        } else if (!grayscale && managed) {
            textBlock.UnregisterPropertyChangedCallback(
                Controls::TextBlock::ForegroundProperty(),
                it->foregroundChangedToken);
            textBlock.Foreground(it->savedForeground);
            g_batteryTextBlockStates->erase(it);
        }
        return false;
    });
}

void SetItemContainerMargin(FrameworkElement container,
                            double before,
                            double after,
                            bool vertical) {
    if (before == 0 && after == 0) {
        container.ClearValue(FrameworkElement::MarginProperty());
    } else if (vertical) {
        container.Margin(Thickness{0, before, 0, after});
    } else {
        container.Margin(Thickness{before, 0, after, 0});
    }
}

// A hidden icon keeps its (empty) item container in the panel, and the panel
// adds Spacing for every child regardless of visibility or size. Cancel the
// extra spacing with negative margins on the neighboring visible items. Returns
// whether any item is visible.
bool UpdateControlCenterButtonStackPanel(FrameworkElement stackPanel) {
    double spacing = 0;
    bool vertical = false;
    if (auto panel = stackPanel.try_as<Controls::StackPanel>()) {
        spacing = panel.Spacing();
        vertical = panel.Orientation() == Controls::Orientation::Vertical;
    }

    // Margins are assigned once the next visible item (or the end) is known,
    // so that the trailing hidden items can be folded into the after margin.
    FrameworkElement pendingContainer = nullptr;
    double pendingBefore = 0;
    int hiddenRun = 0;

    EnumChildElements(stackPanel, [&](FrameworkElement child) {
        auto childClassName = winrt::get_class_name(child);
        if (childClassName != L"Windows.UI.Xaml.Controls.ContentPresenter") {
            Wh_Log(L"Unsupported class name %s of child",
                   childClassName.c_str());
            return false;
        }

        auto systemTrayIconElement = FindChildByName(child, L"SystemTrayIcon")
                                         .try_as<Controls::Control>();
        if (!systemTrayIconElement) {
            Wh_Log(L"Failed to get SystemTrayIcon of child");
            return false;
        }

        if (!systemTrayIconElement.IsEnabled()) {
            hiddenRun++;
            return false;
        }

        if (pendingContainer) {
            SetItemContainerMargin(pendingContainer, pendingBefore, 0,
                                   vertical);
        }

        pendingContainer = child;
        pendingBefore = -spacing * hiddenRun;
        hiddenRun = 0;
        return false;
    });

    if (pendingContainer) {
        SetItemContainerMargin(pendingContainer, pendingBefore,
                               -spacing * hiddenRun, vertical);
    }

    return !!pendingContainer;
}

// Hides the button if all icons are hidden.
void UpdateControlCenterButtonPanel(FrameworkElement panel) {
    bool anyVisible;
    if (auto wrapGrid = panel.try_as<Controls::WrapGrid>()) {
        auto visibleCount =
            UpdateWrapGridHiddenItems(wrapGrid, [](FrameworkElement container) {
                auto systemTrayIconElement =
                    FindChildByName(container, L"SystemTrayIcon")
                        .try_as<Controls::Control>();
                return systemTrayIconElement &&
                       !systemTrayIconElement.IsEnabled();
            });
        if (!visibleCount) {
            return;
        }

        anyVisible = *visibleCount > 0;
    } else {
        anyVisible = UpdateControlCenterButtonStackPanel(panel);
    }

    // The button template has some width of its own (e.g. the background
    // border margins), so an empty panel alone doesn't make it disappear.
    FrameworkElement controlCenterButton =
        GetParentElementByName(panel, L"ControlCenterButton");
    if (!controlCenterButton) {
        Wh_Log(L"Failed to get ControlCenterButton");
        return;
    }

    controlCenterButton.Visibility(anyVisible ? Visibility::Visible
                                              : Visibility::Collapsed);
}

void ApplyControlCenterButtonIconStyle(FrameworkElement systemTrayIconElement) {
    FrameworkElement contentGrid = nullptr;

    FrameworkElement child = systemTrayIconElement;
    if ((child = FindChildByName(child, L"ContainerGrid")) &&
        (child = FindChildByName(child, L"ContentGrid"))) {
        contentGrid = child;
    } else {
        Wh_Log(L"Failed to get ContentGrid");
        return;
    }

    bool hide = false;
    FrameworkElement systemTrayTextIconContent =
        FindChildByClassName(contentGrid, L"SystemTray.BatteryIconContent");
    if (systemTrayTextIconContent) {
        if (!g_unloading) {
            hide = g_settings.hideBatteryIcon;
        }

        Wh_Log(L"System battery tray icon, hide=%d", hide);

        ApplyBatteryIconGrayscaleStyle(systemTrayTextIconContent);
    } else {
        systemTrayTextIconContent =
            FindChildByClassName(contentGrid, L"SystemTray.TextIconContent");
        if (!systemTrayTextIconContent) {
            Wh_Log(L"Failed to get SystemTray.TextIconContent");
            return;
        }

        Controls::TextBlock innerTextBlock = nullptr;

        child = systemTrayTextIconContent;
        if ((child = FindChildByName(child, L"ContainerGrid")) &&
            (child = FindChildByName(child, L"Base")) &&
            (child = FindChildByName(child, L"InnerTextBlock"))) {
            innerTextBlock = child.as<Controls::TextBlock>();
        } else {
            Wh_Log(L"Failed to get InnerTextBlock");
            return;
        }

        auto text = innerTextBlock.Text();
        auto systemTrayIconIdent = IdentifySystemTrayIconFromText(text);

        if (!g_unloading) {
            switch (systemTrayIconIdent) {
                case SystemTrayIconIdent::kVolume:
                    hide = g_settings.hideVolumeIcon;
                    break;

                case SystemTrayIconIdent::kNetwork:
                    hide = g_settings.hideNetworkIcon;
                    break;

                case SystemTrayIconIdent::kBattery:
                    hide = g_settings.hideBatteryIcon;
                    break;

                default:
                    Wh_Log(L"Failed");
                    break;
            }
        }

        Wh_Log(L"System tray icon %d (%s), hide=%d", (int)systemTrayIconIdent,
               StringToHex(text).c_str(), hide);
    }

    bool hidden =
        systemTrayTextIconContent.Visibility() == Visibility::Collapsed;
    if (hide != hidden) {
        systemTrayTextIconContent.Visibility(hide ? Visibility::Collapsed
                                                  : Visibility::Visible);
        if (auto control = systemTrayIconElement.try_as<Controls::Control>()) {
            control.IsEnabled(!hide);
        } else {
            Wh_Log(L"Failed");
        }
    }

    // The layout depends on the siblings too, e.g. a newly added visible icon
    // must show the button even if this icon's own state is unchanged.
    FrameworkElement parent = systemTrayIconElement;
    if ((parent = Media::VisualTreeHelper::GetParent(parent)
                      .try_as<FrameworkElement>()) &&
        winrt::get_class_name(parent) ==
            L"Windows.UI.Xaml.Controls.ContentPresenter" &&
        (parent = Media::VisualTreeHelper::GetParent(parent)
                      .try_as<FrameworkElement>()) &&
        parent.try_as<Controls::Panel>()) {
        // Removing an item container changes the size of a stack panel, which
        // is the only signal that the margins of the remaining items are
        // stale. The panel is replaced when the taskbar layout changes.
        if (!g_unloading && g_controlCenterPanel.get() != parent) {
            if (auto previousPanel = g_controlCenterPanel.get()) {
                previousPanel.SizeChanged(g_controlCenterPanelSizeChangedToken);
            }

            g_controlCenterPanel = parent;
            g_controlCenterPanelSizeChangedToken = parent.SizeChanged(
                [](winrt::Windows::Foundation::IInspectable const& sender,
                   SizeChangedEventArgs const& e) {
                    Wh_Log(L">");

                    auto panel = sender.try_as<FrameworkElement>();
                    if (!panel) {
                        Wh_Log(L"Failed to get sender");
                        return;
                    }

                    UpdateControlCenterButtonPanel(panel);
                });
        }

        UpdateControlCenterButtonPanel(parent);
    } else {
        Wh_Log(L"Failed");
    }
}

void ApplyBellIconStyle(FrameworkElement systemTrayIconElement);

// When the clock is hidden, the bell icon behaves differently - its content is
// recreated each time the bell icon changes. Therefore we retry until the
// content is there.
void ApplyBellIconStyleWithRetry(FrameworkElement systemTrayIconElement,
                                 int attempt) {
    Wh_Log(L"> %d", attempt);

    if (attempt == 10 || g_unloading) {
        return;
    }

    FrameworkElement containerGrid =
        FindChildByName(systemTrayIconElement, L"ContainerGrid");
    if (!containerGrid) {
        systemTrayIconElement.Dispatcher().TryRunAsync(
            winrt::Windows::UI::Core::CoreDispatcherPriority::Low,
            [systemTrayIconElement, attempt]() {
                ApplyBellIconStyleWithRetry(systemTrayIconElement, attempt + 1);
            });
        return;
    }

    ApplyBellIconStyle(systemTrayIconElement);
}

void ApplyBellIconStyle(FrameworkElement systemTrayIconElement) {
    FrameworkElement containerGrid =
        FindChildByName(systemTrayIconElement, L"ContainerGrid");
    if (!containerGrid) {
        Wh_Log(L"Failed to get ContainerGrid");
        return;
    }

    FrameworkElement systemTrayTextIconContent = nullptr;
    bool hasContentPresenterForMissingClock = true;

    // When the clock is hidden, the element path is:
    //
    // #ContainerGrid > #ContentPresenter > #ContentGrid >
    // SystemTray.TextIconContent
    //
    // When the clock is visible, ContentPresenter is missing:
    //
    // #ContainerGrid > #ContentGrid > SystemTray.TextIconContent
    FrameworkElement child =
        FindChildByName(containerGrid, L"ContentPresenter");
    if (!child) {
        hasContentPresenterForMissingClock = false;
        child = containerGrid;
    }

    if ((child = FindChildByName(child, L"ContentGrid")) &&
        (child = FindChildByClassName(child, L"SystemTray.TextIconContent"))) {
        systemTrayTextIconContent = child;
    } else {
        // Wh_Log(L"Failed to get SystemTray.TextIconContent");
        return;
    }

    auto contentPresenter =
        Media::VisualTreeHelper::GetParent(systemTrayIconElement)
            .try_as<FrameworkElement>();

    bool hide = false;

    if (!g_unloading) {
        if (g_settings.hideBellIcon == HideBellIcon::always) {
            hide = true;
        } else if (g_settings.hideBellIcon == HideBellIcon::whenInactive ||
                   g_settings.hideBellIcon ==
                       HideBellIcon::whenInactiveAndNoDnd) {
            Controls::TextBlock innerTextBlock = nullptr;

            child = systemTrayTextIconContent;
            if ((child = FindChildByName(child, L"ContainerGrid")) &&
                (child = FindChildByName(child, L"Base")) &&
                (child = FindChildByName(child, L"InnerTextBlock"))) {
                innerTextBlock = child.as<Controls::TextBlock>();
            } else {
                Wh_Log(L"Failed to get InnerTextBlock");
                return;
            }

            auto shouldHide = [](Controls::TextBlock innerTextBlock) {
                auto text = innerTextBlock.Text();
                auto systemTrayIconIdent = IdentifySystemTrayIconFromText(text);
                switch (systemTrayIconIdent) {
                    case SystemTrayIconIdent::kBellEmpty:
                        return true;

                    case SystemTrayIconIdent::kBellEmptyDnd:
                        return g_settings.hideBellIcon !=
                               HideBellIcon::whenInactiveAndNoDnd;

                    case SystemTrayIconIdent::kBellFull:
                    case SystemTrayIconIdent::kBellFullDnd:
                        return false;

                    default:
                        Wh_Log(L"Unknown bell icon %d (%s)",
                               (int)systemTrayIconIdent,
                               StringToHex(text).c_str());
                        return false;
                }
            };

            hide = shouldHide(innerTextBlock);

            std::erase_if(g_bellIconStates, [](const BellIconState& s) {
                return !s.element.get();
            });

            bool registered = std::any_of(
                g_bellIconStates.begin(), g_bellIconStates.end(),
                [&systemTrayIconElement](const BellIconState& s) {
                    return s.element.get() == systemTrayIconElement;
                });
            if (!registered) {
                g_bellIconStates.push_back({
                    .element = systemTrayIconElement,
                    .automationNameChangedToken =
                        systemTrayIconElement.RegisterPropertyChangedCallback(
                            Automation::AutomationProperties::NameProperty(),
                            [](DependencyObject sender,
                               DependencyProperty property) {
                                Wh_Log(L">");

                                auto bellSystemTrayIconElement =
                                    sender.try_as<FrameworkElement>();
                                if (!bellSystemTrayIconElement) {
                                    Wh_Log(L"Failed to get sender");
                                    return;
                                }

                                ApplyBellIconStyleWithRetry(
                                    bellSystemTrayIconElement, 0);
                            }),
                });
            }
        }
    }

    Wh_Log(L"Bell icon, hide=%d", hide);

    systemTrayTextIconContent.Visibility(hide ? Visibility::Collapsed
                                              : Visibility::Visible);

    if (contentPresenter) {
        if (hide && hasContentPresenterForMissingClock) {
            contentPresenter.MaxWidth(0);
        } else {
            contentPresenter.ClearValue(FrameworkElement::MaxWidthProperty());
        }
    }
}

std::pair<DependencyProperty, DependencyProperty> GetMinMaxSizeProperties(
    bool vertical) {
    if (vertical) {
        return {FrameworkElement::MinHeightProperty(),
                FrameworkElement::MaxHeightProperty()};
    }

    return {FrameworkElement::MinWidthProperty(),
            FrameworkElement::MaxWidthProperty()};
}

void RestoreShowDesktopSize(const ShowDesktopSizeState& state) {
    auto element = state.element.get();
    if (!element) {
        return;
    }

    auto [minProperty, maxProperty] = GetMinMaxSizeProperties(state.vertical);
    if (state.savedMin) {
        element.SetValue(minProperty, winrt::box_value(*state.savedMin));
    } else {
        element.ClearValue(minProperty);
    }
    if (state.savedMax) {
        element.SetValue(maxProperty, winrt::box_value(*state.savedMax));
    } else {
        element.ClearValue(maxProperty);
    }
}

// MinWidth/MaxWidth take precedence over an explicit Width, so they override
// the width without having to replace the Width value itself, and the same
// goes for the height. The local values are saved the first time an element is
// seen along an axis.
void OverrideShowDesktopSize(FrameworkElement element,
                             double size,
                             bool vertical) {
    auto [minProperty, maxProperty] = GetMinMaxSizeProperties(vertical);

    // After a taskbar orientation change, undo the override along the other
    // axis.
    std::erase_if(
        g_showDesktopSizeStates, [&](const ShowDesktopSizeState& state) {
            if (state.vertical == vertical || state.element.get() != element) {
                return false;
            }

            RestoreShowDesktopSize(state);
            return true;
        });

    bool managed = false;
    for (const auto& state : g_showDesktopSizeStates) {
        if (state.element.get() == element && state.vertical == vertical) {
            managed = true;
            break;
        }
    }

    if (!managed) {
        g_showDesktopSizeStates.push_back({
            .element = element,
            .vertical = vertical,
            .savedMin = element.ReadLocalValue(minProperty).try_as<double>(),
            .savedMax = element.ReadLocalValue(maxProperty).try_as<double>(),
        });
    }

    element.SetValue(minProperty, winrt::box_value(size));
    element.SetValue(maxProperty, winrt::box_value(size));
}

bool ApplyShowDesktopStackStyle(FrameworkElement container);

// The icon view isn't necessarily recreated when the tray orientation changes,
// so the show desktop size is re-applied for the new orientation.
void SubscribeToOrientationChanges(FrameworkElement systemTrayFrameGrid,
                                   VisualStateGroup orientationStates) {
    std::erase_if(
        g_orientationStatesSubscriptions,
        [](const OrientationStatesSubscription& s) { return !s.group.get(); });

    for (const auto& subscription : g_orientationStatesSubscriptions) {
        if (subscription.group.get() == orientationStates) {
            return;
        }
    }

    auto systemTrayFrameGridWeakRef = winrt::make_weak(systemTrayFrameGrid);
    g_orientationStatesSubscriptions.push_back({
        .group = orientationStates,
        .currentStateChangedToken = orientationStates.CurrentStateChanged(
            [systemTrayFrameGridWeakRef](
                winrt::Windows::Foundation::IInspectable const& sender,
                VisualStateChangedEventArgs const& e) {
                Wh_Log(L">");

                auto systemTrayFrameGrid = systemTrayFrameGridWeakRef.get();
                if (!systemTrayFrameGrid) {
                    return;
                }

                if (auto showDesktopStack = FindChildByName(
                        systemTrayFrameGrid, L"ShowDesktopStack")) {
                    ApplyShowDesktopStackStyle(showDesktopStack);
                }
            }),
    });
}

void ApplyShowDesktopStyle(FrameworkElement systemTrayIconElement) {
    if (g_unloading) {
        // Restored in ApplySettings.
        return;
    }

    auto showDesktopStack =
        GetParentElementByName(systemTrayIconElement, L"ShowDesktopStack");
    if (!showDesktopStack) {
        Wh_Log(L"Failed");
        return;
    }

    // Drop any state for elements that no longer exist.
    std::erase_if(g_showDesktopSizeStates, [](const ShowDesktopSizeState& s) {
        return !s.element.get();
    });

    auto systemTrayFrameGrid =
        GetParentElementByName(showDesktopStack, L"SystemTrayFrameGrid");
    auto orientationStates =
        systemTrayFrameGrid ? GetOrientationStatesGroup(systemTrayFrameGrid)
                            : nullptr;
    if (orientationStates) {
        SubscribeToOrientationChanges(systemTrayFrameGrid, orientationStates);
    }

    bool vertical = IsVerticalOrientationState(orientationStates);

    // The width setting is the extent of the button along the taskbar.
    int size = g_settings.showDesktopButtonWidth;
    Wh_Log(L"Show desktop button, size=%d, vertical=%d", size, vertical);

    OverrideShowDesktopSize(systemTrayIconElement, size, vertical);
    OverrideShowDesktopSize(showDesktopStack, size, vertical);

    // The container grid has an explicit width (height if vertical), which
    // keeps its content at the default size regardless of the size of the icon
    // view.
    auto containerGrid =
        FindChildByName(systemTrayIconElement, L"ContainerGrid");
    if (containerGrid) {
        OverrideShowDesktopSize(containerGrid, size, vertical);
    } else {
        Wh_Log(L"Failed to get ContainerGrid");
    }
}

bool ApplyMainStackStyle(FrameworkElement container) {
    FrameworkElement panel = nullptr;

    FrameworkElement child = container;
    if ((child = FindChildByName(child, L"Content")) &&
        (child = FindChildByName(child, L"IconStack")) &&
        (child = FindChildByClassName(
             child, L"Windows.UI.Xaml.Controls.ItemsPresenter")) &&
        (child = FindChildPanel(child))) {
        panel = child;
    }

    if (!panel) {
        return false;
    }

    EnumChildElements(panel, [](FrameworkElement child) {
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

        ApplyMainStackIconViewStyle(systemTrayIconElement);
        return false;
    });

    return true;
}

bool ApplyNonActivatableStackStyle(FrameworkElement container) {
    FrameworkElement panel = nullptr;

    FrameworkElement child = container;
    if ((child = FindChildByName(child, L"Content")) &&
        (child = FindChildByName(child, L"IconStack")) &&
        (child = FindChildByClassName(
             child, L"Windows.UI.Xaml.Controls.ItemsPresenter")) &&
        (child = FindChildPanel(child))) {
        panel = child;
    }

    if (!panel) {
        return false;
    }

    EnumChildElements(panel, [](FrameworkElement child) {
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

        ApplyNonActivatableStackIconViewStyle(systemTrayIconElement);
        return false;
    });

    return true;
}

bool ApplyControlCenterButtonStyle(FrameworkElement controlCenterButton) {
    FrameworkElement panel = nullptr;

    FrameworkElement child = controlCenterButton;
    if ((child =
             FindChildByClassName(child, L"Windows.UI.Xaml.Controls.Grid")) &&
        (child = FindChildByName(child, L"ContentPresenter")) &&
        (child = FindChildByClassName(
             child, L"Windows.UI.Xaml.Controls.ItemsPresenter")) &&
        (child = FindChildPanel(child))) {
        panel = child;
    }

    if (!panel) {
        return false;
    }

    EnumChildElements(panel, [](FrameworkElement child) {
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

        ApplyControlCenterButtonIconStyle(systemTrayIconElement);
        return false;
    });

    return true;
}

bool ApplyNotificationCenterButtonStyle(FrameworkElement controlCenterButton) {
    FrameworkElement panel = nullptr;

    FrameworkElement child = controlCenterButton;
    if ((child =
             FindChildByClassName(child, L"Windows.UI.Xaml.Controls.Grid")) &&
        (child = FindChildByName(child, L"ContentPresenter")) &&
        (child = FindChildByClassName(
             child, L"Windows.UI.Xaml.Controls.ItemsPresenter")) &&
        (child = FindChildPanel(child))) {
        panel = child;
    }

    if (!panel) {
        return false;
    }

    EnumChildElements(panel, [](FrameworkElement child) {
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

        ApplyBellIconStyle(systemTrayIconElement);
        return false;
    });

    return true;
}

bool ApplyShowDesktopStackStyle(FrameworkElement container) {
    FrameworkElement panel = nullptr;

    FrameworkElement child = container;
    if ((child = FindChildByName(child, L"Content")) &&
        (child = FindChildByName(child, L"IconStack")) &&
        (child = FindChildByClassName(
             child, L"Windows.UI.Xaml.Controls.ItemsPresenter")) &&
        (child = FindChildPanel(child))) {
        panel = child;
    }

    if (!panel) {
        return false;
    }

    EnumChildElements(panel, [](FrameworkElement child) {
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

        ApplyShowDesktopStyle(systemTrayIconElement);
        return false;
    });

    return true;
}

// On newer builds, the XAML root content is the taskbar frame, and the tray
// frame is an item of its repeater.
FrameworkElement FindSystemTrayFrame(FrameworkElement xamlRootContent) {
    if (winrt::get_class_name(xamlRootContent) != L"Taskbar.TaskbarFrame") {
        return FindChildByClassName(xamlRootContent,
                                    L"SystemTray.SystemTrayFrame");
    }

    FrameworkElement child = xamlRootContent;
    if (!(child = FindChildByName(child, L"RootGrid")) ||
        !(child = FindChildByName(child, L"TaskbarFrameRepeater"))) {
        return nullptr;
    }

    FrameworkElement systemTrayFrame = nullptr;
    EnumChildElements(child, [&systemTrayFrame](FrameworkElement item) {
        if (winrt::get_class_name(item) != L"Taskbar.TaskbarExtensionElement") {
            return false;
        }

        auto extensionElement = item.try_as<Controls::ContentControl>();
        auto content =
            extensionElement
                ? extensionElement.Content().try_as<FrameworkElement>()
                : nullptr;
        if (!content ||
            winrt::get_class_name(content) != L"SystemTray.SystemTrayFrame") {
            return false;
        }

        systemTrayFrame = content;
        return true;
    });

    return systemTrayFrame;
}

bool ApplyStyle(XamlRoot xamlRoot) {
    FrameworkElement systemTrayFrameGrid = nullptr;

    FrameworkElement child = xamlRoot.Content().try_as<FrameworkElement>();
    if (child && (child = FindSystemTrayFrame(child)) &&
        (child = FindChildByName(child, L"SystemTrayFrameGrid"))) {
        systemTrayFrameGrid = child;
    }

    if (!systemTrayFrameGrid) {
        return false;
    }

    bool somethingSucceeded = false;

    FrameworkElement mainStack =
        FindChildByName(systemTrayFrameGrid, L"MainStack");
    if (mainStack) {
        somethingSucceeded |= ApplyMainStackStyle(mainStack);
    }

    FrameworkElement nonActivatableStack =
        FindChildByName(systemTrayFrameGrid, L"NonActivatableStack");
    if (nonActivatableStack) {
        somethingSucceeded |=
            ApplyNonActivatableStackStyle(nonActivatableStack);
    }

    FrameworkElement controlCenterButton =
        FindChildByName(systemTrayFrameGrid, L"ControlCenterButton");
    if (controlCenterButton) {
        somethingSucceeded |=
            ApplyControlCenterButtonStyle(controlCenterButton);
    }

    FrameworkElement notificationCenterButton =
        FindChildByName(systemTrayFrameGrid, L"NotificationCenterButton");
    if (notificationCenterButton) {
        somethingSucceeded |=
            ApplyNotificationCenterButtonStyle(notificationCenterButton);
    }

    FrameworkElement showDesktopStack =
        FindChildByName(systemTrayFrameGrid, L"ShowDesktopStack");
    if (showDesktopStack) {
        somethingSucceeded |= ApplyShowDesktopStackStyle(showDesktopStack);
    }

    return somethingSucceeded;
}

using IconView_IconView_t = void*(WINAPI*)(void* pThis);
IconView_IconView_t IconView_IconView_Original;
void* WINAPI IconView_IconView_Hook(void* pThis) {
    Wh_Log(L">");

    void* ret = IconView_IconView_Original(pThis);

    // A new icon view has nothing to restore, and the revoker list is released
    // while unloading.
    if (g_unloading) {
        return ret;
    }

    FrameworkElement iconView = nullptr;
    ((IUnknown**)pThis)[1]->QueryInterface(winrt::guid_of<FrameworkElement>(),
                                           winrt::put_abi(iconView));
    if (!iconView) {
        return ret;
    }

    g_autoRevokerList->emplace_back();
    auto autoRevokerIt = g_autoRevokerList->end();
    --autoRevokerIt;

    *autoRevokerIt = iconView.Loaded(
        winrt::auto_revoke_t{},
        [autoRevokerIt](winrt::Windows::Foundation::IInspectable const& sender,
                        RoutedEventArgs const& e) {
            Wh_Log(L">");

            g_autoRevokerList->erase(autoRevokerIt);

            auto iconView = sender.try_as<FrameworkElement>();
            if (!iconView) {
                return;
            }

            auto className = winrt::get_class_name(iconView);
            Wh_Log(L"className: %s", className.c_str());

            if (className == L"SystemTray.IconView") {
                if (iconView.Name() == L"SystemTrayIcon") {
                    if (IsChildOfElementByName(iconView, L"MainStack")) {
                        ApplyMainStackIconViewStyle(iconView);
                    } else if (IsChildOfElementByName(iconView,
                                                      L"NonActivatableStack")) {
                        ApplyNonActivatableStackIconViewStyle(iconView);
                    } else if (IsChildOfElementByName(iconView,
                                                      L"ControlCenterButton")) {
                        ApplyControlCenterButtonIconStyle(iconView);
                    } else if (IsChildOfElementByName(
                                   iconView, L"NotificationCenterButton")) {
                        ApplyBellIconStyle(iconView);
                    } else if (IsChildOfElementByName(iconView,
                                                      L"ShowDesktopStack")) {
                        ApplyShowDesktopStyle(iconView);
                    }
                }
            }
        });

    return ret;
}

// Sets the columns and item width of a wrap grid for all of its icons,
// including the hidden ones, e.g. when icons are added or removed. The hidden
// icons are excluded from the layout again afterwards.
using SystemTrayFrame_ConfigureWrapGridColumns_t =
    void(WINAPI*)(void* element, unsigned int itemCount, double width);
SystemTrayFrame_ConfigureWrapGridColumns_t
    SystemTrayFrame_ConfigureWrapGridColumns_Original;
void WINAPI
SystemTrayFrame_ConfigureWrapGridColumns_Hook(void* element,
                                              unsigned int itemCount,
                                              double width) {
    SystemTrayFrame_ConfigureWrapGridColumns_Original(element, itemCount,
                                                      width);

    if (g_unloading) {
        return;
    }

    try {
        DependencyObject dependencyObject = nullptr;
        winrt::copy_from_abi(dependencyObject, *(void**)element);
        auto frameworkElement = dependencyObject.try_as<FrameworkElement>();
        if (!frameworkElement) {
            return;
        }

        auto name = frameworkElement.Name();
        Wh_Log(L"> %s %u", name.c_str(), itemCount);

        if (name == L"MainStack") {
            ApplyMainStackStyle(frameworkElement);
        } else if (name == L"ControlCenterButton") {
            ApplyControlCenterButtonStyle(frameworkElement);
        }
    } catch (...) {
        HRESULT hr = winrt::to_hresult();
        Wh_Log(L"Error %08X", hr);
    }
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

    size_t taskbarElementIUnknownOffset = 0x10;

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
    {
        // 7f2303d5 pacibsp
        // fd7bbfa9 stp     fp, lr, [sp, #-0x10]!
        // fd030091 mov     fp, sp
        // 080c41f8 ldr     x8, [x0, #0x10]!
        const DWORD* p = (const DWORD*)TaskbarHost_FrameHeight_Original;
        if (p[0] == 0xD503237F && (p[1] & 0xFFC07FFF) == 0xA9807BFD &&
            p[2] == 0x910003FD && (p[3] & 0xFFF00FE0) == 0xF8400C00) {
            taskbarElementIUnknownOffset = (p[3] >> 12) & 0xFF;
        } else {
            Wh_Log(L"Unsupported TaskbarHost::FrameHeight");
        }
    }
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

void LoadSettings() {
    g_settings.hideVolumeIcon = Wh_GetIntSetting(L"hideVolumeIcon");
    g_settings.hideNetworkIcon = Wh_GetIntSetting(L"hideNetworkIcon");
    g_settings.hideBatteryIcon = Wh_GetIntSetting(L"hideBatteryIcon");
    g_settings.grayscaleBatteryIcon = Wh_GetIntSetting(L"grayscaleBatteryIcon");
    g_settings.hideMicrophoneIcon = Wh_GetIntSetting(L"hideMicrophoneIcon");
    g_settings.hideGeolocationIcon = Wh_GetIntSetting(L"hideGeolocationIcon");
    g_settings.hideStudioEffectsIcon =
        Wh_GetIntSetting(L"hideStudioEffectsIcon");
    g_settings.hideRecallIcon = Wh_GetIntSetting(L"hideRecallIcon");
    g_settings.hideLanguageBar = Wh_GetIntSetting(L"hideLanguageBar");
    g_settings.hideLanguageSupplementaryIcons =
        Wh_GetIntSetting(L"hideLanguageSupplementaryIcons");

    PCWSTR hideBellIcon = Wh_GetStringSetting(L"hideBellIcon");
    g_settings.hideBellIcon = HideBellIcon::never;
    if (wcscmp(hideBellIcon, L"whenInactive") == 0) {
        g_settings.hideBellIcon = HideBellIcon::whenInactive;
    } else if (wcscmp(hideBellIcon, L"whenInactiveAndNoDnd") == 0) {
        g_settings.hideBellIcon = HideBellIcon::whenInactiveAndNoDnd;
    } else if (wcscmp(hideBellIcon, L"always") == 0) {
        g_settings.hideBellIcon = HideBellIcon::always;
    }
    Wh_FreeStringSetting(hideBellIcon);

    g_settings.showDesktopButtonWidth =
        std::clamp(Wh_GetIntSetting(L"showDesktopButtonWidth"), 0, 800);
}

void ApplySettings() {
    Wh_Log(L"Applying settings");

    HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();
    if (!hTaskbarWnd) {
        Wh_Log(L"No taskbar found");
        return;
    }

    RunFromWindowThread(
        hTaskbarWnd,
        [](void* pParam) {
            g_autoRevokerList->clear();

            for (const auto& state : g_bellIconStates) {
                if (auto element = state.element.get()) {
                    element.UnregisterPropertyChangedCallback(
                        Automation::AutomationProperties::NameProperty(),
                        state.automationNameChangedToken);
                }
            }
            g_bellIconStates.clear();

            for (const auto& state : g_mainStackTextBlockStates) {
                if (auto textBlock = state.textBlock.get()) {
                    textBlock.UnregisterPropertyChangedCallback(
                        Controls::TextBlock::TextProperty(),
                        state.textChangedToken);
                }
            }
            g_mainStackTextBlockStates.clear();

            if (auto controlCenterPanel = g_controlCenterPanel.get()) {
                controlCenterPanel.SizeChanged(
                    g_controlCenterPanelSizeChangedToken);
                g_controlCenterPanel = nullptr;
                g_controlCenterPanelSizeChangedToken = {};
            }

            // Unregister and restore battery TextBlock foregrounds. ApplyStyle
            // below will re-register if grayscale is still enabled. This also
            // covers stale entries whose icon view was destroyed.
            for (auto& state : *g_batteryTextBlockStates) {
                auto textBlock = state.textBlock.get();
                if (!textBlock) {
                    continue;
                }
                textBlock.UnregisterPropertyChangedCallback(
                    Controls::TextBlock::ForegroundProperty(),
                    state.foregroundChangedToken);
                textBlock.Foreground(state.savedForeground);
            }
            g_batteryTextBlockStates->clear();

            for (const auto& subscription : g_orientationStatesSubscriptions) {
                if (auto group = subscription.group.get()) {
                    group.CurrentStateChanged(
                        subscription.currentStateChangedToken);
                }
            }
            g_orientationStatesSubscriptions.clear();

            // Restore the show desktop size overrides. ApplyStyle below
            // re-applies them unless the mod is unloading.
            for (const auto& state : g_showDesktopSizeStates) {
                RestoreShowDesktopSize(state);
            }
            g_showDesktopSizeStates.clear();

            // Secondary taskbars are created on the primary taskbar's thread.
            EnumThreadWindows(
                GetCurrentThreadId(),
                [](HWND hWnd, LPARAM lParam) -> BOOL {
                    WCHAR className[32];
                    if (!GetClassName(hWnd, className, ARRAYSIZE(className))) {
                        return TRUE;
                    }

                    XamlRoot xamlRoot = nullptr;
                    if (_wcsicmp(className, L"Shell_TrayWnd") == 0) {
                        xamlRoot = GetTaskbarXamlRoot(hWnd);
                    } else if (_wcsicmp(className, L"Shell_SecondaryTrayWnd") ==
                               0) {
                        xamlRoot = GetSecondaryTaskbarXamlRoot(hWnd);
                    } else {
                        return TRUE;
                    }

                    if (!xamlRoot) {
                        Wh_Log(L"Getting XamlRoot failed");
                    } else if (!ApplyStyle(xamlRoot)) {
                        Wh_Log(L"ApplyStyle failed");
                    }

                    return TRUE;
                },
                0);

            if (g_unloading) {
                g_autoRevokerList.reset();
                g_batteryTextBlockStates.reset();
            }
        },
        nullptr);
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
            {LR"(private: static void __cdecl winrt::SystemTray::implementation::SystemTrayFrame::ConfigureWrapGridColumns(struct winrt::Windows::UI::Xaml::DependencyObject const &,unsigned int,double))"},
            &SystemTrayFrame_ConfigureWrapGridColumns_Original,
            SystemTrayFrame_ConfigureWrapGridColumns_Hook,
            true,  // Only used with the native vertical taskbar.
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

HMODULE GetSystemTrayModuleHandle() {
    HMODULE module = GetModuleHandle(L"SystemTray.dll");
    if (!module) {
        module = GetModuleHandle(L"Taskbar.View.dll");
        if (module) {
            // Starting with Taskbar.View.dll 2604.8002.200.6000, the SystemTray
            // types moved out of Taskbar.View.dll into SystemTray.dll, so don't
            // hook Taskbar.View.dll at this version and above.
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
    };

    return HookSymbols(module, taskbarDllHooks, ARRAYSIZE(taskbarDllHooks));
}

BOOL Wh_ModInit() {
    Wh_Log(L">");

    LoadSettings();

    if (HMODULE systemTrayModule = GetSystemTrayModuleHandle()) {
        g_systemTrayModuleHooked = true;
        if (!HookSystemTraySymbols(systemTrayModule)) {
            return FALSE;
        }
    } else {
        Wh_Log(L"System tray module not loaded yet");

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
    Wh_Log(L">");

    if (!g_systemTrayModuleHooked) {
        if (HMODULE systemTrayModule = GetSystemTrayModuleHandle()) {
            if (!g_systemTrayModuleHooked.exchange(true)) {
                Wh_Log(L"Got system tray module");

                if (HookSystemTraySymbols(systemTrayModule)) {
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
