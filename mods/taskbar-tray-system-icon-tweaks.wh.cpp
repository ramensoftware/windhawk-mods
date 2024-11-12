// ==WindhawkMod==
// @id              taskbar-tray-system-icon-tweaks
// @name            Taskbar tray system icon tweaks
// @description     Allows hiding system icons (volume, network, battery), the bell (always or when there are no new notifications), and the "Show desktop" button (Windows 11 only)
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
# Taskbar tray system icon tweaks

Allows hiding system icons (volume, network, battery), the bell (always or when
there are no new notifications), and the "Show desktop" button.

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
- hideMicrophoneIcon: false
  $name: Hide microphone icon
- hideGeolocationIcon: false
  $name: Hide location (e.g. GPS) icon
- hideLanguageBar: false
  $name: Hide language bar
- hideBellIcon: never
  $name: Hide bell icon
  $options:
  - never: Never
  - whenInactive: When there are no new notifications
  - always: Always
- showDesktopButtonWidth: 12
  $name: '"Show desktop" button width'
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

enum class HideBellIcon {
    never,
    whenInactive,
    always,
};

struct {
    bool hideVolumeIcon;
    bool hideNetworkIcon;
    bool hideBatteryIcon;
    bool hideMicrophoneIcon;
    bool hideGeolocationIcon;
    bool hideLanguageBar;
    HideBellIcon hideBellIcon;
    int showDesktopButtonWidth;
} g_settings;

std::atomic<bool> g_unloading;

using FrameworkElementLoadedEventRevoker = winrt::impl::event_revoker<
    IFrameworkElement,
    &winrt::impl::abi<IFrameworkElement>::type::remove_Loaded>;

std::list<FrameworkElementLoadedEventRevoker> g_autoRevokerList;

winrt::weak_ref<Controls::TextBlock> g_mainStackInnerTextBlock;
int64_t g_mainStackTextChangedToken;

winrt::weak_ref<Controls::TextBlock> g_bellInnerTextBlock;
int64_t g_bellTextChangedToken;

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
    kBellFull,
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

        case L'\uF285':  // Empty bell, Do Not Disturb
        case L'\uF2A3':  // Empty bell
            return SystemTrayIconIdent::kBellEmpty;

        case L'\uF2A5':  // Full bell
        case L'\uF2A8':  // Full bell, Do Not Disturb
            return SystemTrayIconIdent::kBellFull;
    }

    return SystemTrayIconIdent::kUnknown;
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

    if (!g_unloading && !g_mainStackInnerTextBlock.get()) {
        auto notifyIconViewElementWeakRef =
            winrt::make_weak(notifyIconViewElement);
        g_mainStackInnerTextBlock = innerTextBlock;
        g_mainStackTextChangedToken =
            innerTextBlock.RegisterPropertyChangedCallback(
                Controls::TextBlock::TextProperty(),
                [notifyIconViewElementWeakRef, &shouldHide](
                    DependencyObject sender, DependencyProperty property) {
                    auto innerTextBlock = sender.try_as<Controls::TextBlock>();
                    if (!innerTextBlock) {
                        return;
                    }

                    auto notifyIconViewElement =
                        notifyIconViewElementWeakRef.get();
                    if (!notifyIconViewElement) {
                        return;
                    }

                    bool hide = shouldHide(innerTextBlock);

                    Wh_Log(L"Main stack icon, hide=%d", hide);

                    notifyIconViewElement.Visibility(
                        hide ? Visibility::Collapsed : Visibility::Visible);
                });
    }
}

void ApplyNonActivatableStackIconViewStyle(
    FrameworkElement notifyIconViewElement) {
    FrameworkElement systemTrayLanguageTextIconContent = nullptr;

    FrameworkElement child = notifyIconViewElement;
    if ((child = FindChildByName(child, L"ContainerGrid")) &&
        (child = FindChildByName(child, L"ContentPresenter")) &&
        (child = FindChildByName(child, L"ContentGrid")) &&
        (child = FindChildByClassName(child,
                                      L"SystemTray.LanguageTextIconContent"))) {
        systemTrayLanguageTextIconContent = child;
    } else {
        Wh_Log(L"Failed to get SystemTray.LanguageTextIconContent");
        return;
    }

    bool hide = !g_unloading && g_settings.hideLanguageBar;

    Wh_Log(L"Language bar, hide=%d", hide);

    notifyIconViewElement.Visibility(hide ? Visibility::Collapsed
                                          : Visibility::Visible);
}

void ApplyControlCenterButtonIconStyle(FrameworkElement systemTrayIconElement) {
    FrameworkElement systemTrayTextIconContent = nullptr;

    FrameworkElement child = systemTrayIconElement;
    if ((child = FindChildByName(child, L"ContainerGrid")) &&
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

    auto text = innerTextBlock.Text();
    auto systemTrayIconIdent = IdentifySystemTrayIconFromText(text);

    bool hide = false;
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

    bool hidden =
        systemTrayTextIconContent.Visibility() == Visibility::Collapsed;
    if (hide == hidden) {
        return;
    }

    systemTrayTextIconContent.Visibility(hide ? Visibility::Collapsed
                                              : Visibility::Visible);
    if (auto control = systemTrayIconElement.try_as<Controls::Control>()) {
        control.IsEnabled(!hide);
    } else {
        Wh_Log(L"Failed");
    }

    // If all icons are hidden, hide container as well.
    FrameworkElement parent = systemTrayIconElement;
    if ((parent = Media::VisualTreeHelper::GetParent(parent)
                      .try_as<FrameworkElement>()) &&
        winrt::get_class_name(parent) ==
            L"Windows.UI.Xaml.Controls.ContentPresenter" &&
        (parent = Media::VisualTreeHelper::GetParent(parent)
                      .try_as<FrameworkElement>()) &&
        winrt::get_class_name(parent) ==
            L"Windows.UI.Xaml.Controls.StackPanel") {
        FrameworkElement stackPanel = parent;
        bool anyEnabledChild = false;
        EnumChildElements(
            stackPanel, [&anyEnabledChild](FrameworkElement child) {
                auto childClassName = winrt::get_class_name(child);
                if (childClassName !=
                    L"Windows.UI.Xaml.Controls.ContentPresenter") {
                    Wh_Log(L"Unsupported class name %s of child",
                           childClassName.c_str());
                    return false;
                }

                auto systemTrayIconElement =
                    FindChildByName(child, L"SystemTrayIcon")
                        .try_as<Controls::Control>();
                if (!systemTrayIconElement) {
                    Wh_Log(L"Failed to get SystemTrayIcon of child");
                    return false;
                }

                if (!systemTrayIconElement.IsEnabled()) {
                    return false;
                }

                anyEnabledChild = true;
                return true;
            });

        stackPanel.Visibility(anyEnabledChild ? Visibility::Visible
                                              : Visibility::Collapsed);
    } else {
        Wh_Log(L"Failed");
    }
}

void ApplyBellIconStyle(FrameworkElement systemTrayIconElement) {
    FrameworkElement systemTrayTextIconContent = nullptr;

    FrameworkElement child = systemTrayIconElement;
    if ((child = FindChildByName(child, L"ContainerGrid")) &&
        (child = FindChildByName(child, L"ContentGrid")) &&
        (child = FindChildByClassName(child, L"SystemTray.TextIconContent"))) {
        systemTrayTextIconContent = child;
    } else {
        // Wh_Log(L"Failed to get SystemTray.TextIconContent");
        return;
    }

    bool hide = false;

    if (!g_unloading) {
        if (g_settings.hideBellIcon == HideBellIcon::always) {
            hide = true;
        } else if (g_settings.hideBellIcon == HideBellIcon::whenInactive) {
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

                    case SystemTrayIconIdent::kBellFull:
                        return false;

                    default:
                        Wh_Log(L"Unknown bell icon %d (%s)",
                               (int)systemTrayIconIdent,
                               StringToHex(text).c_str());
                        return false;
                }
            };

            hide = shouldHide(innerTextBlock);

            if (!g_bellInnerTextBlock.get()) {
                auto systemTrayTextIconContentWeakRef =
                    winrt::make_weak(systemTrayTextIconContent);
                g_bellInnerTextBlock = innerTextBlock;
                g_bellTextChangedToken =
                    innerTextBlock.RegisterPropertyChangedCallback(
                        Controls::TextBlock::TextProperty(),
                        [systemTrayTextIconContentWeakRef, &shouldHide](
                            DependencyObject sender,
                            DependencyProperty property) {
                            auto innerTextBlock =
                                sender.try_as<Controls::TextBlock>();
                            if (!innerTextBlock) {
                                return;
                            }

                            auto systemTrayTextIconContent =
                                systemTrayTextIconContentWeakRef.get();
                            if (!systemTrayTextIconContent) {
                                return;
                            }

                            bool hide = shouldHide(innerTextBlock);

                            Wh_Log(L"Bell icon, hide=%d", hide);

                            systemTrayTextIconContent.Visibility(
                                hide ? Visibility::Collapsed
                                     : Visibility::Visible);
                        });
            }
        }
    }

    Wh_Log(L"Bell icon, hide=%d", hide);

    systemTrayTextIconContent.Visibility(hide ? Visibility::Collapsed
                                              : Visibility::Visible);
}

void ApplyShowDesktopStyle(FrameworkElement systemTrayIconElement) {
    auto showDesktopStack =
        GetParentElementByName(systemTrayIconElement, L"ShowDesktopStack");
    if (!showDesktopStack) {
        Wh_Log(L"Failed");
        return;
    }

    if (g_unloading) {
        Wh_Log(L"Show desktop button, setting default width");

        auto systemTrayIconElementDP =
            systemTrayIconElement.as<DependencyObject>();
        systemTrayIconElementDP.ClearValue(
            FrameworkElement::MinWidthProperty());
        systemTrayIconElementDP.ClearValue(
            FrameworkElement::MaxWidthProperty());

        auto showDesktopStackDP = showDesktopStack.as<DependencyObject>();
        showDesktopStackDP.ClearValue(FrameworkElement::MinWidthProperty());
        showDesktopStackDP.ClearValue(FrameworkElement::MaxWidthProperty());
    } else {
        int width = g_settings.showDesktopButtonWidth;
        Wh_Log(L"Show desktop button, width=%d", width);

        systemTrayIconElement.MinWidth(width);
        systemTrayIconElement.MaxWidth(width);
        showDesktopStack.MinWidth(width);
        showDesktopStack.MaxWidth(width);
    }
}

bool ApplyMainStackStyle(FrameworkElement container) {
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

    EnumChildElements(stackPanel, [](FrameworkElement child) {
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

    EnumChildElements(stackPanel, [](FrameworkElement child) {
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

    EnumChildElements(stackPanel, [](FrameworkElement child) {
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
    g_settings.hideVolumeIcon = Wh_GetIntSetting(L"hideVolumeIcon");
    g_settings.hideNetworkIcon = Wh_GetIntSetting(L"hideNetworkIcon");
    g_settings.hideBatteryIcon = Wh_GetIntSetting(L"hideBatteryIcon");
    g_settings.hideMicrophoneIcon = Wh_GetIntSetting(L"hideMicrophoneIcon");
    g_settings.hideGeolocationIcon = Wh_GetIntSetting(L"hideGeolocationIcon");
    g_settings.hideLanguageBar = Wh_GetIntSetting(L"hideLanguageBar");

    PCWSTR hideBellIcon = Wh_GetStringSetting(L"hideBellIcon");
    g_settings.hideBellIcon = HideBellIcon::never;
    if (wcscmp(hideBellIcon, L"whenInactive") == 0) {
        g_settings.hideBellIcon = HideBellIcon::whenInactive;
    } else if (wcscmp(hideBellIcon, L"always") == 0) {
        g_settings.hideBellIcon = HideBellIcon::always;
    }
    Wh_FreeStringSetting(hideBellIcon);

    g_settings.showDesktopButtonWidth =
        Wh_GetIntSetting(L"showDesktopButtonWidth");
}

void ApplySettings() {
    struct ApplySettingsParam {
        HWND hTaskbarWnd;
    };

    Wh_Log(L"Applying settings");

    HWND hTaskbarWnd = GetTaskbarWnd();
    if (!hTaskbarWnd) {
        Wh_Log(L"No taskbar found");
        return;
    }

    ApplySettingsParam param{
        .hTaskbarWnd = hTaskbarWnd,
    };

    RunFromWindowThread(
        hTaskbarWnd,
        [](PVOID pParam) WINAPI {
            ApplySettingsParam& param = *(ApplySettingsParam*)pParam;

            g_autoRevokerList.clear();

            if (auto bellInnerTextBlock = g_bellInnerTextBlock.get()) {
                bellInnerTextBlock.UnregisterPropertyChangedCallback(
                    Controls::TextBlock::TextProperty(),
                    g_bellTextChangedToken);
                g_bellInnerTextBlock = nullptr;
                g_bellTextChangedToken = 0;
            }

            if (auto mainStackInnerTextBlock =
                    g_mainStackInnerTextBlock.get()) {
                mainStackInnerTextBlock.UnregisterPropertyChangedCallback(
                    Controls::TextBlock::TextProperty(),
                    g_mainStackTextChangedToken);
                g_mainStackInnerTextBlock = nullptr;
                g_mainStackTextChangedToken = 0;
            }

            auto xamlRoot = GetTaskbarXamlRoot(param.hTaskbarWnd);
            if (!xamlRoot) {
                Wh_Log(L"Getting XamlRoot failed");
                return;
            }

            if (!ApplyStyle(xamlRoot)) {
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
    };

    return HookSymbols(module, symbolHooks, ARRAYSIZE(symbolHooks));
}

bool HookTaskbarDllSymbols() {
    HMODULE module = LoadLibrary(L"taskbar.dll");
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
            {LR"(public: void __cdecl std::_Ref_count_base::_Decref(void))"},
            &std__Ref_count_base__Decref_Original,
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
