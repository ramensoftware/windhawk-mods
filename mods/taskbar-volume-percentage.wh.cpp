// ==WindhawkMod==
// @id              taskbar-volume-percentage
// @name            Taskbar Volume Percentage Indicator
// @description     Shows the master volume percentage in the Windows 11 system tray volume icon
// @version         1.8.1
// @author          gilnett
// @github          https://github.com/gilnett
// @architecture    x86-64
// @include         explorer.exe
// @compilerOptions -lole32 -loleaut32 -lruntimeobject -lversion
// @donateUrl       https://ko-fi.com/gilnet
// @license         GPL-3.0
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Taskbar Volume Percentage Indicator

Replaces the Windows 11 system tray volume icon with the current master volume
level, updated in real time.

![Taskbar Volume Percentage Preview 1](https://i.imgur.com/vjwali8.png)
![Taskbar Volume Percentage Preview 2](https://i.imgur.com/rbVvlEg.png)
![Taskbar Volume Percentage Preview 3](https://i.imgur.com/27iyViO.png)
![Taskbar Volume Percentage Preview 4](https://i.imgur.com/1alQd1j.png)

## Settings

- **Display style**: Format of the volume indicator in the taskbar:
  - Percentage (`50%`)
  - Number only (`50`)
  - Prefix and percentage (`50% Vol` or `Vol 50%`, with custom prefix)
  - Icon and percentage (`50% 🔊` or `🔊 50%`)
  - Native icon and percentage
  - Native icon and number
- **Element position**: Placement of the icon or prefix relative to the volume text for styles that include both:
  - On the right (default)
  - On the left
- **Custom prefix**: Text prefix used with the "Prefix and percentage" style (default: `Vol `).
- **Mute display style**: Style of the mute indicator when audio is muted:
  - MUT
  - Mute
  - 0%
  - Cross symbol (`✕`)
  - Mute emoji (`🔇`)
  - Native mute icon
  - Native mute icon and text
  - Native mute icon and 0%
  - Custom text
- **Custom mute text**: Text used when the "Custom text" or "Native mute icon and text" mute style is selected (default: `Mute`).
- **Icon spacing**: Space between the icon and the text (default: `-1` for automatic, `0` to disable, or any other number for a fixed size in pixels).
- **Container width**: Width of the volume area (default: `-1` for automatic, `0` to disable, or any other number for a fixed size in pixels).
- **Volume font size**: Size in pixels for the volume text indicator. Set to `0` to use the default size.
- **Prefix font size**: Size in pixels for the prefix text. Set to `0` to use the default size.
- **Icon and logo size**: Size in pixels for native volume/mute icons and emoji icons. Set to `0` to use the Windows default size.
- **Custom font family**: Name of any font installed in Windows (e.g. `JetBrains Mono`, `Fira Code`, `Bahnschrift`, `Consolas`, `Digital-7`). Leave empty to use the default taskbar font.

## Compatibility

- Only Windows 11 is supported.
- Supported Architectures: x64 and ARM64.
- If the mod is enabled while Explorer is already running, the text appears
  after the next volume change.
- Compatible with [Windows 11 Taskbar Styler](https://windhawk.net/mods/windows-11-taskbar-styler):
  - Spacing is managed via `Grid.ColumnSpacing`, leaving all custom `Margin` rules set by Taskbar Styler intact.
  - Custom font sizes on `TextBlock#InnerTextBlock` are automatically detected and mirrored.
  - The volume text block can also be styled directly via `TextBlock#VolumePercentageSubBlock`.

## Support

If you find this mod useful, you can support its development and maintenance:
- [Support on Ko-fi](https://ko-fi.com/gilnet)

## Credits

- Inspired by the taskbar visual customization concepts from [m417z](https://github.com/m417z) and [taskbar-tray-system-icon-tweaks](https://github.com/ramensoftware/windhawk-mods/blob/main/mods/taskbar-tray-system-icon-tweaks.wh.cpp).
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- displayStyle: percentage
  $name: Display style
  $options:
  - percentage: Percentage
  - number: Number only
  - prefix: Prefix and percentage
  - emoji: Icon and percentage
  - glyphRight: Native icon and percentage
  - glyphNumber: Native icon and number
- elementPosition: right
  $name: Element position
  $description: >-
    Placement of the icon or prefix relative to the volume text for styles
    that include an additional element.
  $options:
  - right: On the right
  - left: On the left
- customPrefix: "Vol "
  $name: Custom prefix
  $description: Used with the "Prefix and percentage" display style.
- muteStyle: mut
  $name: Mute display style
  $options:
  - mut: MUT
  - mute: Mute
  - zero: 0%
  - cross: "✕ (Cross symbol)"
  - emoji: "🔇 (Mute emoji)"
  - glyph: Native mute icon
  - glyphText: Native mute icon and text
  - glyphZero: Native mute icon and 0%
  - custom: Custom text
- customMuteText: "Mute"
  $name: Custom mute text
  $description: >-
    Used with the "Custom text" and "Native mute icon and text" mute display
    styles.
- iconSpacing: -1
  $name: Icon spacing
  $description: >-
    Space between the icon and the text. Set to -1 for automatic spacing, or
    to 0 to disable it. Any other number sets the space in pixels.
- containerWidth: -1
  $name: Container width
  $description: >-
    Width of the volume area. Set to -1 for automatic width, or to 0 to
    disable it. Any other number sets the width in pixels.
- fontSize: 0
  $name: Volume font size
  $description: >-
    Font size in pixels for the volume number or percentage text. Set to 0 to
    use the default size.
- prefixSize: 0
  $name: Prefix font size
  $description: >-
    Font size in pixels for the prefix text. Set to 0 to use the default size.
- iconSize: 0
  $name: Icon and logo size
  $description: >-
    Size in pixels for native volume/mute icons and emoji icons. Set to 0 to
    use the Windows default size.
- customFontFamily: ""
  $name: Custom font family
  $description: >-
    Name of any font installed in Windows (e.g. "JetBrains Mono", "Fira Code",
    "Bahnschrift", "Consolas", "Digital-7"). Leave empty to use the default
    taskbar font.
*/
// ==/WindhawkModSettings==

#include <windhawk_api.h>
#include <windhawk_utils.h>

#include <algorithm>
#include <atomic>
#include <cmath>
#include <cwctype>
#include <list>
#include <mutex>
#include <string>
#include <string_view>
#include <vector>

#undef GetCurrentTime

#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.Text.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Data.h>
#include <winrt/Windows.UI.Xaml.Documents.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Media.h>

using namespace winrt::Windows::UI::Xaml;

enum class DisplayStyle {
    percentage,
    number,
    prefix,
    emoji,
    glyphRight,
    glyphNumber,
};

enum class ElementPosition {
    right,
    left,
};

enum class MuteStyle {
    mut,
    mute,
    zero,
    cross,
    emoji,
    glyph,
    glyphText,
    glyphZero,
    custom,
};

struct {
    DisplayStyle displayStyle;
    ElementPosition elementPosition;
    WindhawkUtils::StringSetting customPrefix;
    MuteStyle muteStyle;
    WindhawkUtils::StringSetting customMuteText;
    int containerWidth;
    int iconSpacing;
    double fontSize;
    double prefixSize;
    double iconSize;
    WindhawkUtils::StringSetting customFontFamily;
} g_settings;

std::atomic<bool> g_unloading;
std::atomic<bool> g_systemTrayModuleHooked;

std::atomic<int> g_settingsGeneration;

std::mutex g_volumeStateMutex;
bool g_hasVolumeState;
float g_volumeLevel;
bool g_isMuted;

double g_maxObservedWidth = 0.0;

struct VolumeDataModel {
    void* pThis;
    winrt::weak_ref<winrt::Windows::Foundation::IInspectable> weakRef;
};
std::vector<VolumeDataModel> g_volumeDataModels;

void* g_gettingCurrentDataModel;

winrt::hstring g_spatialSoundName;

[[clang::no_destroy]] winrt::Windows::Foundation::IUnknown g_volumeIconData;

std::wstring g_volumeText;

bool g_volumeTextChanged;

struct TrackedIconView {
    winrt::weak_ref<FrameworkElement> iconView;
    bool hasCustomWidth = false;
    winrt::Windows::Foundation::IInspectable origMinWidth{nullptr};
    winrt::Windows::Foundation::IInspectable origWidth{nullptr};
    winrt::Windows::Foundation::IInspectable origAlignment{nullptr};
};
[[clang::no_destroy]] std::optional<std::vector<TrackedIconView>>
    g_volumeIconViews{std::in_place};

struct TrackedVolumeContent {
    winrt::weak_ref<FrameworkElement> textIconContent;
    winrt::weak_ref<Controls::Grid> containerGrid;
    winrt::weak_ref<FrameworkElement> baseElement;
    winrt::weak_ref<FrameworkElement> underlayElement;
    winrt::weak_ref<Controls::TextBlock> subBlock;
    bool isCustomLayoutConfigured = false;
    bool lastDualBoxMode = false;
    winrt::Windows::Foundation::IInspectable origUnderlayVisibility{nullptr};
    winrt::Windows::Foundation::IInspectable origBaseVisibility{nullptr};
    winrt::Windows::Foundation::IInspectable origUnderlayAlignment{nullptr};
    winrt::Windows::Foundation::IInspectable origBaseAlignment{nullptr};
    winrt::Windows::Foundation::IInspectable origUnderlayColumn{nullptr};
    winrt::Windows::Foundation::IInspectable origBaseColumn{nullptr};
    winrt::Windows::Foundation::IInspectable origContainerGridAlignment{nullptr};
    winrt::Windows::Foundation::IInspectable origContainerGridColumnSpacing{
        nullptr};
    winrt::Windows::Foundation::IInspectable origTextIconContentAlignment{
        nullptr};
    winrt::Windows::Foundation::IInspectable origBaseFontSize{nullptr};
    winrt::Windows::Foundation::IInspectable origBaseMinWidth{nullptr};
    winrt::Windows::Foundation::IInspectable origUnderlayFontSize{nullptr};
    winrt::Windows::Foundation::IInspectable origUnderlayMinWidth{nullptr};
    winrt::weak_ref<Controls::TextBlock> observedInnerTextBlock;
    int64_t fontSizeCallbackToken = 0;
    bool lastNativeGlyph = false;
    int layoutSettingsGeneration = -1;
};
[[clang::no_destroy]] std::optional<std::vector<TrackedVolumeContent>>
    g_trackedVolumeContents{std::in_place};

using FrameworkElementLayoutUpdatedEventRevoker = winrt::impl::event_revoker<
    IFrameworkElement,
    &winrt::impl::abi<IFrameworkElement>::type::remove_LayoutUpdated>;

[[clang::no_destroy]] std::optional<
    std::list<FrameworkElementLayoutUpdatedEventRevoker>>
    g_autoRevokerList{std::in_place};

static BOOL CALLBACK EnumWindowsTaskbarProc(HWND hWnd, LPARAM lParam) {
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
}

HWND FindCurrentProcessTaskbarWnd() {
    HWND hTaskbarWnd = nullptr;
    EnumWindows(EnumWindowsTaskbarProc, reinterpret_cast<LPARAM>(&hTaskbarWnd));
    return hTaskbarWnd;
}

using RunFromWindowThreadProc_t = void (*)(void* parameter);

struct RUN_FROM_WINDOW_THREAD_PARAM {
    RunFromWindowThreadProc_t proc;
    void* procParam;
};

static LRESULT CALLBACK CallWndProcHook(int nCode, WPARAM wParam, LPARAM lParam) {
    static const UINT runFromWindowThreadRegisteredMsg =
        RegisterWindowMessage(L"Windhawk_RunFromWindowThread_" WH_MOD_ID);

    if (nCode == HC_ACTION) {
        const auto* cwp = reinterpret_cast<const CWPSTRUCT*>(lParam);
        if (cwp->message == runFromWindowThreadRegisteredMsg) {
            auto* param =
                reinterpret_cast<RUN_FROM_WINDOW_THREAD_PARAM*>(cwp->lParam);
            param->proc(param->procParam);
        }
    }

    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

bool RunFromWindowThread(HWND hWnd,
                         RunFromWindowThreadProc_t proc,
                         void* procParam) {
    static const UINT runFromWindowThreadRegisteredMsg =
        RegisterWindowMessage(L"Windhawk_RunFromWindowThread_" WH_MOD_ID);

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
        CallWndProcHook,
        nullptr, dwThreadId);
    if (!hook) {
        return false;
    }

    RUN_FROM_WINDOW_THREAD_PARAM param;
    param.proc = proc;
    param.procParam = procParam;
    SendMessage(hWnd, runFromWindowThreadRegisteredMsg, 0,
                reinterpret_cast<LPARAM>(&param));

    UnhookWindowsHookEx(hook);

    return true;
}

bool RunFromTaskbarThread(RunFromWindowThreadProc_t proc) {
    HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();
    return hTaskbarWnd && RunFromWindowThread(hTaskbarWnd, proc, nullptr);
}

FrameworkElement GetParentElementByClassName(FrameworkElement const& element,
                                             PCWSTR className) {
    auto parent = element;
    while ((parent = Media::VisualTreeHelper::GetParent(parent)
                         .try_as<FrameworkElement>())) {
        if (winrt::get_class_name(parent) == className) {
            return parent;
        }
    }

    return nullptr;
}

FrameworkElement FindChildByName(FrameworkElement const& element, PCWSTR name) {
    int childrenCount = Media::VisualTreeHelper::GetChildrenCount(element);
    for (int i = 0; i < childrenCount; i++) {
        if (auto child = Media::VisualTreeHelper::GetChild(element, i)
                             .try_as<FrameworkElement>()) {
            if (child.Name() == name) {
                return child;
            }
        }
    }

    return nullptr;
}

bool IsDualBoxStyle() {
    return g_settings.displayStyle == DisplayStyle::glyphRight ||
           g_settings.displayStyle == DisplayStyle::glyphNumber;
}

std::wstring FormatVolumeText(float volumeLevel, bool isMuted) {
    if (isMuted) {
        std::wstring customText = g_settings.customMuteText.get();
        return (g_settings.muteStyle == MuteStyle::mute)        ? L"Mute"
               : (g_settings.muteStyle == MuteStyle::zero)      ? L"0%"
               : (g_settings.muteStyle == MuteStyle::cross)     ? L"\u2715"
               : (g_settings.muteStyle == MuteStyle::emoji)     ? L"\U0001F507"
               : (g_settings.muteStyle == MuteStyle::glyph)     ? L"\uE74F"
               : (g_settings.muteStyle == MuteStyle::glyphZero) ? L"0%"
               : (g_settings.muteStyle == MuteStyle::glyphText)
                   ? (!customText.empty() ? customText : L"Mute")
               : (g_settings.muteStyle == MuteStyle::custom)
                   ? customText
                   : L"MUT";
    }

    int percentage =
        std::clamp((int)std::lround(volumeLevel * 100.0f), 0, 100);
    std::wstring number = std::to_wstring(percentage);

    if (g_settings.displayStyle == DisplayStyle::number ||
        g_settings.displayStyle == DisplayStyle::glyphNumber) {
        return number;
    }
    if (g_settings.displayStyle == DisplayStyle::prefix) {
        PCWSTR prefix = g_settings.customPrefix.get();
        std::wstring rawPrefix = prefix ? prefix : L"";
        if (g_settings.elementPosition == ElementPosition::right) {
            auto start = rawPrefix.find_first_not_of(L" \t\r\n");
            if (start == std::wstring::npos) {
                return number + L"%";
            }
            auto end = rawPrefix.find_last_not_of(L" \t\r\n");
            std::wstring trimmed = rawPrefix.substr(start, end - start + 1);
            return number + L"% " + trimmed;
        }
        return rawPrefix + number + L"%";
    }
    if (g_settings.displayStyle == DisplayStyle::emoji) {
        if (g_settings.elementPosition == ElementPosition::left) {
            return L"\U0001F50A " + number + L"%";
        }
        return number + L"% \U0001F50A";
    }

    return number + L"%";
}

double CalculateAutoWidth() {
    std::wstring maxVolText = FormatVolumeText(1.0f, false);
    std::wstring muteText = FormatVolumeText(0.0f, true);
    size_t maxLen = (std::max)(maxVolText.length(), muteText.length());

    double volFontSize =
        (g_settings.fontSize > 0) ? (g_settings.fontSize * 0.85) : 10.0;
    double prefixFontSize = (g_settings.prefixSize > 0)
                                ? (g_settings.prefixSize * 0.85)
                                : 10.0;
    double iconW = (g_settings.iconSize > 0) ? g_settings.iconSize : 16.0;

    double estimatedWidth = 10.0;
    if (g_settings.displayStyle == DisplayStyle::prefix) {
        PCWSTR prefix = g_settings.customPrefix.get();
        size_t prefLen = prefix ? wcslen(prefix) : 4;
        estimatedWidth += (static_cast<double>(prefLen) * prefixFontSize) +
                          (4.0 * volFontSize);
    } else if (g_settings.displayStyle == DisplayStyle::emoji) {
        estimatedWidth += (4.0 * volFontSize) + iconW + 6.0;
    } else if (g_settings.displayStyle == DisplayStyle::number ||
               g_settings.displayStyle == DisplayStyle::glyphNumber) {
        estimatedWidth += (3.0 * volFontSize);
    } else {
        estimatedWidth += (static_cast<double>(maxLen) * volFontSize);
    }

    if (IsDualBoxStyle()) {
        double gap = (g_settings.iconSpacing < 0)
                         ? 4.0
                         : static_cast<double>(g_settings.iconSpacing);
        estimatedWidth += iconW + gap;
    }

    double baseFloor = (g_settings.displayStyle == DisplayStyle::glyphRight)    ? 56.0
                       : (g_settings.displayStyle == DisplayStyle::emoji)       ? 56.0
                       : (g_settings.displayStyle == DisplayStyle::glyphNumber) ? 48.0
                       : (g_settings.displayStyle == DisplayStyle::prefix)      ? 64.0
                       : (g_settings.displayStyle == DisplayStyle::percentage)  ? 40.0
                                                                                : 32.0;

    double dynamicFloor = baseFloor;
    if (g_settings.fontSize > 0) {
        dynamicFloor +=
            (g_settings.fontSize - 12.0) * static_cast<double>(maxLen) * 0.85;
    }
    if (g_settings.prefixSize > 0 &&
        g_settings.displayStyle == DisplayStyle::prefix) {
        dynamicFloor += (g_settings.prefixSize - 12.0) * 4.0 * 0.85;
    }
    if (g_settings.iconSize > 0 &&
        (IsDualBoxStyle() || g_settings.displayStyle == DisplayStyle::emoji)) {
        dynamicFloor += (g_settings.iconSize - 16.0);
    }

    return (std::max)({estimatedWidth, dynamicFloor, 32.0});
}

double GetContainerWidth() {
    if (g_unloading || g_settings.containerWidth == 0) {
        return 0;
    }

    if (g_settings.containerWidth > 0) {
        return g_settings.containerWidth;
    }

    if (g_maxObservedWidth > 0.0) {
        return g_maxObservedWidth;
    }

    return CalculateAutoWidth();
}

void ApplyIconViewWidth(TrackedIconView& tracked,
                        FrameworkElement const& iconView) {
    double width = GetContainerWidth();
    if (width > 0) {
        if (!tracked.hasCustomWidth) {
            tracked.origMinWidth =
                iconView.ReadLocalValue(FrameworkElement::MinWidthProperty());
            tracked.origWidth =
                iconView.ReadLocalValue(FrameworkElement::WidthProperty());
            tracked.origAlignment = iconView.ReadLocalValue(
                FrameworkElement::HorizontalAlignmentProperty());
            tracked.hasCustomWidth = true;
        }
        iconView.MinWidth(width);
        iconView.Width(width);
        iconView.HorizontalAlignment(HorizontalAlignment::Center);
    } else if (tracked.hasCustomWidth) {
        auto restoreProp =
            [](FrameworkElement const& el, DependencyProperty const& dp,
               winrt::Windows::Foundation::IInspectable const& orig) {
                if (!orig || orig == DependencyProperty::UnsetValue()) {
                    el.ClearValue(dp);
                } else {
                    el.SetValue(dp, orig);
                }
            };
        restoreProp(iconView, FrameworkElement::MinWidthProperty(),
                    tracked.origMinWidth);
        restoreProp(iconView, FrameworkElement::WidthProperty(),
                    tracked.origWidth);
        restoreProp(iconView, FrameworkElement::HorizontalAlignmentProperty(),
                    tracked.origAlignment);
        tracked.hasCustomWidth = false;
    }
}

void PruneVolumeDataModels() {
    std::erase_if(g_volumeDataModels, [](const auto& dataModel) {
        return !dataModel.weakRef.get();
    });
}

VolumeDataModel* FindVolumeDataModel(void* pThis) {
    for (auto& dataModel : g_volumeDataModels) {
        if (dataModel.pThis == pThis) {
            return &dataModel;
        }
    }

    return nullptr;
}

void RememberVolumeDataModel(
    void* pThis,
    winrt::Windows::Foundation::IInspectable const& dataModel) {
    PruneVolumeDataModels();

    if (g_unloading || FindVolumeDataModel(pThis)) {
        return;
    }

    Wh_Log(L"Volume data model %p", pThis);
    g_volumeDataModels.push_back({pThis, dataModel});
}

void RememberVolumeIconView(FrameworkElement const& iconView) {
    if (!g_volumeIconViews) {
        return;
    }

    std::erase_if(*g_volumeIconViews,
                  [](const auto& tracked) { return !tracked.iconView.get(); });

    for (const auto& tracked : *g_volumeIconViews) {
        if (tracked.iconView.get() == iconView) {
            return;
        }
    }

    Wh_Log(L"Volume icon view %p", winrt::get_abi(iconView));
    g_volumeIconViews->push_back({iconView});
}

void ApplyVolumeIconViewsWidth() {
    if (!g_volumeIconViews) {
        return;
    }

    for (auto& tracked : *g_volumeIconViews) {
        if (auto iconView = tracked.iconView.get()) {
            ApplyIconViewWidth(tracked, iconView);
        }
    }
}

void PruneTrackedVolumeContents() {
    if (!g_trackedVolumeContents) {
        return;
    }

    std::erase_if(*g_trackedVolumeContents, [](auto& tracked) {
        if (!tracked.textIconContent.get()) {
            if (tracked.fontSizeCallbackToken != 0) {
                if (auto observed = tracked.observedInnerTextBlock.get()) {
                    try {
                        observed.UnregisterPropertyChangedCallback(
                            Controls::TextBlock::FontSizeProperty(),
                            tracked.fontSizeCallbackToken);
                    } catch (winrt::hresult_error const& ex) {
                        Wh_Log(L"Error unregistering font size callback: %08X",
                               ex.code());
                    }
                }
                tracked.fontSizeCallbackToken = 0;
                tracked.observedInnerTextBlock = nullptr;
            }
            return true;
        }
        return false;
    });
}

TrackedVolumeContent* FindTrackedVolumeContent(
    FrameworkElement const& textIconContent) {
    if (!g_trackedVolumeContents) {
        return nullptr;
    }

    for (auto& tracked : *g_trackedVolumeContents) {
        if (tracked.textIconContent.get() == textIconContent) {
            return &tracked;
        }
    }

    return nullptr;
}

constexpr double kDefaultIconFontSize = 16.0;

void ApplyIconFontSize(
    FrameworkElement const& el,
    double size,
    winrt::Windows::Foundation::IInspectable const& origFontSize,
    winrt::Windows::Foundation::IInspectable const& origMinWidth) {
    if (!el) {
        return;
    }
    auto restoreProp =
        [](FrameworkElement const& target, DependencyProperty const& dp,
           winrt::Windows::Foundation::IInspectable const& orig) {
            if (target) {
                if (!orig || orig == DependencyProperty::UnsetValue()) {
                    target.ClearValue(dp);
                } else {
                    target.SetValue(dp, orig);
                }
            }
        };

    if (FrameworkElement tbEl = FindChildByName(el, L"InnerTextBlock")) {
        if (auto tb = tbEl.try_as<Controls::TextBlock>()) {
            if (size > 0) {
                tb.FontSize(size);
                el.MinWidth(size);
            } else {
                restoreProp(tb, Controls::TextBlock::FontSizeProperty(),
                            origFontSize);
                restoreProp(el, FrameworkElement::MinWidthProperty(),
                            origMinWidth);
            }
        }
    }
}

void RestoreVolumeLayout(TrackedVolumeContent& tracked) {
    if (tracked.fontSizeCallbackToken != 0) {
        if (auto observed = tracked.observedInnerTextBlock.get()) {
            try {
                observed.UnregisterPropertyChangedCallback(
                    Controls::TextBlock::FontSizeProperty(),
                    tracked.fontSizeCallbackToken);
            } catch (winrt::hresult_error const& ex) {
                Wh_Log(L"Error unregistering font size callback: %08X",
                       ex.code());
            }
        }
        tracked.fontSizeCallbackToken = 0;
        tracked.observedInnerTextBlock = nullptr;
    }

    auto containerGrid = tracked.containerGrid.get();
    if (!containerGrid) {
        return;
    }

    auto subBlock = tracked.subBlock.get();
    if (!subBlock) {
        if (FrameworkElement child =
                FindChildByName(containerGrid, L"VolumePercentageSubBlock")) {
            subBlock = child.try_as<Controls::TextBlock>();
        }
    }

    if (subBlock) {
        uint32_t index = 0;
        if (containerGrid.Children().IndexOf(subBlock, index)) {
            containerGrid.Children().RemoveAt(index);
        }
        tracked.subBlock = nullptr;
    }

    if (!tracked.isCustomLayoutConfigured) {
        return;
    }

    containerGrid.ColumnDefinitions().Clear();

    auto restoreProp =
        [](FrameworkElement const& el, DependencyProperty const& dp,
           winrt::Windows::Foundation::IInspectable const& orig) {
            if (el) {
                if (!orig || orig == DependencyProperty::UnsetValue()) {
                    el.ClearValue(dp);
                } else {
                    el.SetValue(dp, orig);
                }
            }
        };

    if (auto base = tracked.baseElement.get()) {
        restoreProp(base, FrameworkElement::MinWidthProperty(),
                    tracked.origBaseMinWidth);
        restoreProp(base, UIElement::VisibilityProperty(),
                    tracked.origBaseVisibility);
        restoreProp(base, FrameworkElement::HorizontalAlignmentProperty(),
                    tracked.origBaseAlignment);
        restoreProp(base, Controls::Grid::ColumnProperty(),
                    tracked.origBaseColumn);
        if (FrameworkElement tbEl = FindChildByName(base, L"InnerTextBlock")) {
            if (auto tb = tbEl.try_as<Controls::TextBlock>()) {
                tb.ClearValue(Controls::TextBlock::TextAlignmentProperty());
                tb.ClearValue(FrameworkElement::HorizontalAlignmentProperty());
                restoreProp(tb, Controls::TextBlock::FontSizeProperty(),
                            tracked.origBaseFontSize);
            }
        }
    }
    if (auto underlay = tracked.underlayElement.get()) {
        restoreProp(underlay, FrameworkElement::MinWidthProperty(),
                    tracked.origUnderlayMinWidth);
        restoreProp(underlay, UIElement::VisibilityProperty(),
                    tracked.origUnderlayVisibility);
        restoreProp(underlay, FrameworkElement::HorizontalAlignmentProperty(),
                    tracked.origUnderlayAlignment);
        restoreProp(underlay, Controls::Grid::ColumnProperty(),
                    tracked.origUnderlayColumn);
        if (FrameworkElement tbEl =
                FindChildByName(underlay, L"InnerTextBlock")) {
            if (auto tb = tbEl.try_as<Controls::TextBlock>()) {
                tb.ClearValue(Controls::TextBlock::TextAlignmentProperty());
                tb.ClearValue(FrameworkElement::HorizontalAlignmentProperty());
                restoreProp(tb, Controls::TextBlock::FontSizeProperty(),
                            tracked.origUnderlayFontSize);
            }
        }
    }

    restoreProp(containerGrid, FrameworkElement::HorizontalAlignmentProperty(),
                tracked.origContainerGridAlignment);
    restoreProp(containerGrid, Controls::Grid::ColumnSpacingProperty(),
                tracked.origContainerGridColumnSpacing);
    if (auto tic = tracked.textIconContent.get()) {
        restoreProp(tic, FrameworkElement::HorizontalAlignmentProperty(),
                    tracked.origTextIconContentAlignment);
    }

    tracked.lastDualBoxMode = false;
    tracked.lastNativeGlyph = false;
    tracked.isCustomLayoutConfigured = false;
}

void RestoreAllVolumeLayouts() {
    if (g_autoRevokerList) {
        g_autoRevokerList->clear();
    }
    if (g_trackedVolumeContents) {
        for (auto& tracked : *g_trackedVolumeContents) {
            if (tracked.fontSizeCallbackToken != 0) {
                if (auto observed = tracked.observedInnerTextBlock.get()) {
                    try {
                        observed.UnregisterPropertyChangedCallback(
                            Controls::TextBlock::FontSizeProperty(),
                            tracked.fontSizeCallbackToken);
                    } catch (winrt::hresult_error const& ex) {
                        Wh_Log(L"Error unregistering font size callback: %08X",
                               ex.code());
                    }
                }
                tracked.fontSizeCallbackToken = 0;
                tracked.observedInnerTextBlock = nullptr;
            }
        }
        for (auto& tracked : *g_trackedVolumeContents) {
            RestoreVolumeLayout(tracked);
        }
    }
}

void SyncSubBlockFontFamily(Controls::TextBlock const& subBlock) {
    if (!subBlock) {
        return;
    }
    PCWSTR fontSetting = g_settings.customFontFamily.get();
    std::wstring_view fontName = fontSetting ? fontSetting : L"";
    while (!fontName.empty() && iswspace(fontName.front())) {
        fontName.remove_prefix(1);
    }
    while (!fontName.empty() && iswspace(fontName.back())) {
        fontName.remove_suffix(1);
    }

    if (!fontName.empty()) {
        std::wstring fontChain =
            std::wstring(fontName) + L", Segoe UI Variable Text, Segoe UI";
        subBlock.FontFamily(Media::FontFamily(fontChain));
        return;
    }

    subBlock.ClearValue(Controls::TextBlock::FontFamilyProperty());
}

double GetSubBlockFontSize(Controls::TextBlock const& source) {
    if (g_settings.fontSize > 0) {
        return g_settings.fontSize;
    }
    // With iconSize set, the mod owns the source value - don't copy it.
    if (g_settings.iconSize <= 0 && source) {
        double sourceSize = source.FontSize();
        if (std::abs(sourceSize - kDefaultIconFontSize) > 0.01) {
            return sourceSize; // Customized, e.g. by Taskbar Styler.
        }
    }
    return 12.0;
}

void SyncSubBlockFontSize(Controls::TextBlock const& subBlock,
                          Controls::TextBlock const& source) {
    if (!subBlock) {
        return;
    }
    subBlock.FontSize(GetSubBlockFontSize(source));
}

void BindSubBlockTextStyle(TrackedVolumeContent& tracked,
                           Controls::TextBlock const& subBlock,
                           Controls::TextBlock const& source) {
    auto bind = [&](DependencyProperty const& dp, PCWSTR path) {
        Data::Binding binding;
        binding.Source(source);
        binding.Path(PropertyPath(path));
        subBlock.SetBinding(dp, binding);
    };
    bind(Controls::TextBlock::ForegroundProperty(), L"Foreground");
    bind(Controls::TextBlock::FontWeightProperty(), L"FontWeight");

    SyncSubBlockFontSize(subBlock, source);

    if (tracked.fontSizeCallbackToken == 0 ||
        tracked.observedInnerTextBlock.get() != source) {
        if (tracked.fontSizeCallbackToken != 0) {
            if (auto prevSource = tracked.observedInnerTextBlock.get()) {
                try {
                    prevSource.UnregisterPropertyChangedCallback(
                        Controls::TextBlock::FontSizeProperty(),
                        tracked.fontSizeCallbackToken);
                } catch (winrt::hresult_error const& ex) {
                    Wh_Log(L"Error unregistering font size callback: %08X",
                           ex.code());
                }
            }
            tracked.fontSizeCallbackToken = 0;
        }

        tracked.observedInnerTextBlock = source;
        if (source) {
            try {
                tracked.fontSizeCallbackToken =
                    source.RegisterPropertyChangedCallback(
                        Controls::TextBlock::FontSizeProperty(),
                        [subBlockWeak = winrt::make_weak(subBlock)](
                            DependencyObject const& sender,
                            DependencyProperty const&) {
                            if (auto tb = sender.try_as<Controls::TextBlock>()) {
                                if (auto sub = subBlockWeak.get()) {
                                    SyncSubBlockFontSize(sub, tb);
                                }
                            }
                        });
            } catch (winrt::hresult_error const& ex) {
                Wh_Log(L"Error registering font size callback: %08X", ex.code());
                tracked.fontSizeCallbackToken = 0;
            }
        }
    }

    SyncSubBlockFontFamily(subBlock);
}

Controls::TextBlock GetOrCreateSubBlock(TrackedVolumeContent& tracked,
                                        Controls::Grid const& containerGrid,
                                        FrameworkElement const& baseElement) {
    Controls::TextBlock subBlock = nullptr;
    if (auto existingSubBlock = tracked.subBlock.get()) {
        subBlock = existingSubBlock;
    } else {
        FrameworkElement child =
            FindChildByName(containerGrid, L"VolumePercentageSubBlock");
        if (child) {
            subBlock = child.try_as<Controls::TextBlock>();
        }
    }

    if (!subBlock) {
        subBlock = Controls::TextBlock();
        subBlock.Name(L"VolumePercentageSubBlock");
        subBlock.VerticalAlignment(VerticalAlignment::Center);
        SyncSubBlockFontFamily(subBlock);
        SyncSubBlockFontSize(subBlock, nullptr);

        containerGrid.Children().Append(subBlock);
        tracked.subBlock = subBlock;
    }

    FrameworkElement textBlockEl =
        FindChildByName(baseElement, L"InnerTextBlock");
    if (textBlockEl) {
        if (auto innerTextBlock = textBlockEl.try_as<Controls::TextBlock>()) {
            BindSubBlockTextStyle(tracked, subBlock, innerTextBlock);
        }
    } else {
        SyncSubBlockFontSize(subBlock, nullptr);
        SyncSubBlockFontFamily(subBlock);
    }

    return subBlock;
}

void UpdateSubBlockContent(Controls::TextBlock const& subBlock,
                           bool isMuted,
                           float volumeLevel) {
    if (!subBlock) {
        return;
    }

    if (isMuted) {
        if (g_settings.muteStyle == MuteStyle::emoji) {
            double effectiveIconSize = (g_settings.iconSize > 0)
                                           ? g_settings.iconSize
                                           : kDefaultIconFontSize;
            subBlock.Text(L"");
            subBlock.Inlines().Clear();
            Documents::Run run;
            run.Text(L"\U0001F507");
            run.FontSize(effectiveIconSize);
            subBlock.Inlines().Append(run);
            return;
        }
        subBlock.Inlines().Clear();
        subBlock.Text(FormatVolumeText(volumeLevel, isMuted));
        return;
    }

    int percentage =
        std::clamp(static_cast<int>(std::lround(volumeLevel * 100.0f)), 0, 100);
    std::wstring number = std::to_wstring(percentage);

    if (g_settings.displayStyle == DisplayStyle::prefix) {
        PCWSTR prefix = g_settings.customPrefix.get();
        std::wstring rawPrefix = prefix ? prefix : L"";
        std::wstring numPart = number + L"%";
        std::wstring prefPart;

        if (g_settings.elementPosition == ElementPosition::right) {
            auto start = rawPrefix.find_first_not_of(L" \t\r\n");
            auto end = rawPrefix.find_last_not_of(L" \t\r\n");
            if (start != std::wstring::npos) {
                prefPart = L" " + rawPrefix.substr(start, end - start + 1);
            }
        } else {
            prefPart = rawPrefix;
        }

        subBlock.Text(L"");
        subBlock.Inlines().Clear();

        Documents::Run prefRun;
        prefRun.Text(prefPart);
        if (g_settings.prefixSize > 0) {
            prefRun.FontSize(g_settings.prefixSize);
        }

        Documents::Run volRun;
        volRun.Text(numPart);
        if (g_settings.fontSize > 0) {
            volRun.FontSize(g_settings.fontSize);
        }

        if (g_settings.elementPosition == ElementPosition::right) {
            subBlock.Inlines().Append(volRun);
            subBlock.Inlines().Append(prefRun);
        } else {
            subBlock.Inlines().Append(prefRun);
            subBlock.Inlines().Append(volRun);
        }
        return;
    }

    if (g_settings.displayStyle == DisplayStyle::emoji) {
        double effectiveIconSize = (g_settings.iconSize > 0)
                                       ? g_settings.iconSize
                                       : kDefaultIconFontSize;
        std::wstring numPart = number + L"%";
        std::wstring emojiPart = L"\U0001F50A";

        subBlock.Text(L"");
        subBlock.Inlines().Clear();

        Documents::Run emojiRun;
        emojiRun.Text(emojiPart);
        emojiRun.FontSize(effectiveIconSize);

        Documents::Run volRun;
        volRun.Text(numPart);
        if (g_settings.fontSize > 0) {
            volRun.FontSize(g_settings.fontSize);
        }

        Documents::Run spaceRun;
        spaceRun.Text(L" ");
        if (g_settings.fontSize > 0) {
            spaceRun.FontSize(g_settings.fontSize);
        }

        if (g_settings.elementPosition == ElementPosition::left) {
            subBlock.Inlines().Append(emojiRun);
            subBlock.Inlines().Append(spaceRun);
            subBlock.Inlines().Append(volRun);
        } else {
            subBlock.Inlines().Append(volRun);
            subBlock.Inlines().Append(spaceRun);
            subBlock.Inlines().Append(emojiRun);
        }
        return;
    }

    subBlock.Inlines().Clear();
    subBlock.Text(FormatVolumeText(volumeLevel, isMuted));
}

void MeasureMaxContainerWidth(Controls::Grid const& containerGrid,
                              Controls::TextBlock const& subBlock,
                              bool useNativeGlyph) {
    if (useNativeGlyph || !containerGrid || !subBlock) {
        return;
    }
    try {
        auto measureCandidate = [&](float vol, bool muted) {
            UpdateSubBlockContent(subBlock, muted, vol);
            containerGrid.Measure(
                winrt::Windows::Foundation::Size{1000.0f, 1000.0f});
            double candWidth =
                static_cast<double>(containerGrid.DesiredSize().Width) + 6.0;
            if (candWidth > g_maxObservedWidth) {
                g_maxObservedWidth = candWidth;
            }
        };
        measureCandidate(1.0f, false);
        measureCandidate(0.0f, true);
    } catch (...) {
        Wh_Log(L"MeasureMaxContainerWidth error: %08X", winrt::to_hresult());
    }
    UpdateSubBlockContent(subBlock, g_isMuted, g_volumeLevel);
    ApplyVolumeIconViewsWidth();
}

void SetupVolumeLayout(FrameworkElement const& textIconContent) {
    if (g_unloading) {
        return;
    }

    FrameworkElement containerEl =
        FindChildByName(textIconContent, L"ContainerGrid");
    if (!containerEl) {
        return;
    }
    auto containerGrid = containerEl.try_as<Controls::Grid>();
    if (!containerGrid) {
        return;
    }

    FrameworkElement baseElement = FindChildByName(containerGrid, L"Base");
    if (!baseElement) {
        return;
    }
    FrameworkElement underlayElement =
        FindChildByName(containerGrid, L"Underlay");

    PruneTrackedVolumeContents();

    TrackedVolumeContent* tracked = FindTrackedVolumeContent(textIconContent);
    if (!tracked) {
        if (!g_trackedVolumeContents) {
            g_trackedVolumeContents.emplace();
        }
        TrackedVolumeContent newTracked;
        newTracked.textIconContent = textIconContent;
        newTracked.containerGrid = containerGrid;
        newTracked.baseElement = baseElement;
        newTracked.underlayElement = underlayElement;
        g_trackedVolumeContents->push_back(std::move(newTracked));
        tracked = &g_trackedVolumeContents->back();
    } else {
        tracked->containerGrid = containerGrid;
        tracked->baseElement = baseElement;
        tracked->underlayElement = underlayElement;
    }

    auto applyLayout = [](FrameworkElement const& el, int col, Visibility vis) {
        if (el) {
            el.Visibility(vis);
            el.HorizontalAlignment(HorizontalAlignment::Center);
            Controls::Grid::SetColumn(el, col);
        }
    };

    if (!tracked->isCustomLayoutConfigured) {
        if (auto base = tracked->baseElement.get()) {
            tracked->origBaseVisibility =
                base.ReadLocalValue(UIElement::VisibilityProperty());
            tracked->origBaseAlignment = base.ReadLocalValue(
                FrameworkElement::HorizontalAlignmentProperty());
            tracked->origBaseColumn =
                base.ReadLocalValue(Controls::Grid::ColumnProperty());
            tracked->origBaseMinWidth =
                base.ReadLocalValue(FrameworkElement::MinWidthProperty());
            if (FrameworkElement tbEl = FindChildByName(base, L"InnerTextBlock")) {
                tracked->origBaseFontSize =
                    tbEl.ReadLocalValue(Controls::TextBlock::FontSizeProperty());
            }
        }
        if (auto underlay = tracked->underlayElement.get()) {
            tracked->origUnderlayVisibility =
                underlay.ReadLocalValue(UIElement::VisibilityProperty());
            tracked->origUnderlayAlignment = underlay.ReadLocalValue(
                FrameworkElement::HorizontalAlignmentProperty());
            tracked->origUnderlayColumn =
                underlay.ReadLocalValue(Controls::Grid::ColumnProperty());
            tracked->origUnderlayMinWidth =
                underlay.ReadLocalValue(FrameworkElement::MinWidthProperty());
            if (FrameworkElement tbEl =
                    FindChildByName(underlay, L"InnerTextBlock")) {
                tracked->origUnderlayFontSize = tbEl.ReadLocalValue(
                    Controls::TextBlock::FontSizeProperty());
            }
        }
        tracked->origContainerGridAlignment = containerGrid.ReadLocalValue(
            FrameworkElement::HorizontalAlignmentProperty());
        tracked->origContainerGridColumnSpacing = containerGrid.ReadLocalValue(
            Controls::Grid::ColumnSpacingProperty());
        tracked->origTextIconContentAlignment = textIconContent.ReadLocalValue(
            FrameworkElement::HorizontalAlignmentProperty());
    }

    if (GetContainerWidth() > 0) {
        containerGrid.HorizontalAlignment(HorizontalAlignment::Center);
        textIconContent.HorizontalAlignment(HorizontalAlignment::Center);
    }

    ApplyIconFontSize(baseElement, g_settings.iconSize,
                      tracked->origBaseFontSize, tracked->origBaseMinWidth);
    ApplyIconFontSize(underlayElement, g_settings.iconSize,
                      tracked->origUnderlayFontSize,
                      tracked->origUnderlayMinWidth);

    bool isDualBoxRequested = IsDualBoxStyle();
    bool isMuteDualBox = isDualBoxRequested && g_isMuted &&
                         (g_settings.muteStyle == MuteStyle::glyphText ||
                          g_settings.muteStyle == MuteStyle::glyphZero);
    bool showDualBox = isDualBoxRequested && (!g_isMuted || isMuteDualBox);

    bool useNativeGlyph =
        g_isMuted && g_settings.muteStyle == MuteStyle::glyph;

    if (tracked->isCustomLayoutConfigured &&
        tracked->layoutSettingsGeneration == g_settingsGeneration &&
        tracked->lastDualBoxMode == showDualBox &&
        tracked->lastNativeGlyph == useNativeGlyph) {
        if (!useNativeGlyph) {
            if (auto subBlock = tracked->subBlock.get()) {
                UpdateSubBlockContent(subBlock, g_isMuted, g_volumeLevel);
            }
        }
        return;
    }

    if (!showDualBox) {
        containerGrid.ColumnDefinitions().Clear();
        containerGrid.ColumnSpacing(0.0);

        Controls::TextBlock subBlock =
            GetOrCreateSubBlock(*tracked, containerGrid, baseElement);

        if (useNativeGlyph) {
            subBlock.Text(L"");
            subBlock.Visibility(Visibility::Collapsed);
            if (FrameworkElement textBlockEl =
                    FindChildByName(baseElement, L"InnerTextBlock")) {
                if (auto innerTextBlock =
                        textBlockEl.try_as<Controls::TextBlock>()) {
                    innerTextBlock.TextAlignment(TextAlignment::Center);
                    innerTextBlock.HorizontalAlignment(
                        HorizontalAlignment::Center);
                }
            }
            if (FrameworkElement underlayTextBlockEl =
                    FindChildByName(underlayElement, L"InnerTextBlock")) {
                if (auto underlayTextBlock =
                        underlayTextBlockEl.try_as<Controls::TextBlock>()) {
                    underlayTextBlock.TextAlignment(TextAlignment::Center);
                    underlayTextBlock.HorizontalAlignment(
                        HorizontalAlignment::Center);
                }
            }
        } else {
            UpdateSubBlockContent(subBlock, g_isMuted, g_volumeLevel);
            subBlock.HorizontalAlignment(HorizontalAlignment::Center);
            subBlock.Visibility(Visibility::Visible);
            Controls::Grid::SetColumn(subBlock, 0);
        }

        Visibility baseVis =
            useNativeGlyph ? Visibility::Visible : Visibility::Collapsed;
        applyLayout(baseElement, 0, baseVis);
        applyLayout(underlayElement, 0, baseVis);

        MeasureMaxContainerWidth(containerGrid, subBlock, useNativeGlyph);

        tracked->lastDualBoxMode = false;
        tracked->lastNativeGlyph = useNativeGlyph;
        tracked->layoutSettingsGeneration = g_settingsGeneration;
        tracked->isCustomLayoutConfigured = true;
        return;
    }

    Controls::TextBlock subBlock =
        GetOrCreateSubBlock(*tracked, containerGrid, baseElement);

    if (containerGrid.ColumnDefinitions().Size() < 2) {
        containerGrid.ColumnDefinitions().Clear();
        Controls::ColumnDefinition col0, col1;
        col0.Width(GridLength{1.0, GridUnitType::Auto});
        col1.Width(GridLength{1.0, GridUnitType::Auto});
        containerGrid.ColumnDefinitions().Append(col0);
        containerGrid.ColumnDefinitions().Append(col1);
    }

    double spacing = (g_settings.iconSpacing < 0)
                         ? 4.0
                         : static_cast<double>(g_settings.iconSpacing);
    containerGrid.ColumnSpacing(spacing);

    if (g_settings.elementPosition == ElementPosition::left) {
        applyLayout(baseElement, 0, Visibility::Visible);
        applyLayout(underlayElement, 0, Visibility::Visible);

        UpdateSubBlockContent(subBlock, g_isMuted, g_volumeLevel);
        subBlock.HorizontalAlignment(HorizontalAlignment::Left);
        subBlock.Visibility(Visibility::Visible);
        Controls::Grid::SetColumn(subBlock, 1);
    } else {
        UpdateSubBlockContent(subBlock, g_isMuted, g_volumeLevel);
        subBlock.HorizontalAlignment(HorizontalAlignment::Right);
        subBlock.Visibility(Visibility::Visible);
        Controls::Grid::SetColumn(subBlock, 0);

        applyLayout(baseElement, 1, Visibility::Visible);
        applyLayout(underlayElement, 1, Visibility::Visible);
    }

    MeasureMaxContainerWidth(containerGrid, subBlock, false);

    tracked->lastDualBoxMode = true;
    tracked->lastNativeGlyph = false;
    tracked->layoutSettingsGeneration = g_settingsGeneration;
    tracked->isCustomLayoutConfigured = true;
}

void UpdateAllVolumeLayouts() {
    PruneTrackedVolumeContents();

    if (!g_trackedVolumeContents) {
        return;
    }

    std::vector<FrameworkElement> targets;
    targets.reserve(g_trackedVolumeContents->size());
    for (auto& tracked : *g_trackedVolumeContents) {
        if (auto textIconContent = tracked.textIconContent.get()) {
            targets.push_back(textIconContent);
        }
    }

    for (auto const& textIconContent : targets) {
        SetupVolumeLayout(textIconContent);
    }
}

bool IsVolumeTextIconContent(FrameworkElement const& textIconContent) {
    FrameworkElement container =
        FindChildByName(textIconContent, L"ContainerGrid");
    if (!container) {
        return false;
    }

    if (FindChildByName(container, L"VolumePercentageSubBlock")) {
        return true;
    }

    FrameworkElement baseChild = FindChildByName(container, L"Base");
    if (!baseChild) {
        return false;
    }

    auto textBlock = FindChildByName(baseChild, L"InnerTextBlock")
                         .try_as<Controls::TextBlock>();
    if (!textBlock) {
        return false;
    }

    std::wstring text{textBlock.Text()};
    if (!g_volumeText.empty() && text == g_volumeText) {
        return true;
    }
    if (IsDualBoxStyle() && !text.empty()) {
        wchar_t ch = text[0];
        return ch == L'\uE74F' || ch == L'\uEA85' || ch == L'\uEBC5' ||
               (ch >= L'\uE992' && ch <= L'\uE995');
    }

    return false;
}

bool IsVolumeIconViewLookupPending(FrameworkElement const& textIconContent) {
    if (g_unloading || g_volumeText.empty()) {
        return false;
    }

    return g_volumeTextChanged || !FindTrackedVolumeContent(textIconContent);
}

using TextIconContentViewModel_SetText_t = void(WINAPI*)(void*, winrt::hstring*);
TextIconContentViewModel_SetText_t TextIconContentViewModel_BaseText_Original;
TextIconContentViewModel_SetText_t
    TextIconContentViewModel_UnderlayText_Original;

void RefreshVolumeIcons();

template <typename F>
void SafeXamlCall(F&& func) {
    try {
        func();
    } catch (...) {
        Wh_Log(L"Error %08X", winrt::to_hresult());
    }
}

void LookUpVolumeIconView(FrameworkElement const& textIconContent) {
    if (!IsVolumeTextIconContent(textIconContent)) {
        return;
    }

    auto iconView =
        GetParentElementByClassName(textIconContent, L"SystemTray.IconView");
    if (!iconView) {
        Wh_Log(L"Failed to get SystemTray.IconView");
        return;
    }

    bool wasEmpty =
        !g_trackedVolumeContents || g_trackedVolumeContents->empty();

    g_volumeTextChanged = false;

    RememberVolumeIconView(iconView);

    if (!g_autoRevokerList) {
        g_autoRevokerList.emplace();
    }

    g_autoRevokerList->emplace_back();
    auto autoRevokerIt = g_autoRevokerList->end();
    --autoRevokerIt;

    *autoRevokerIt = iconView.LayoutUpdated(
        winrt::auto_revoke_t{},
        [autoRevokerIt, wasEmpty,
         textIconContentWeak = winrt::make_weak(textIconContent)](
            winrt::Windows::Foundation::IInspectable const&,
            winrt::Windows::Foundation::IInspectable const&) {
            const bool wasEmptyLocal = wasEmpty;
            auto textIconContentWeakLocal = textIconContentWeak;
            if (g_autoRevokerList) {
                g_autoRevokerList->erase(autoRevokerIt);
            }

            SafeXamlCall([wasEmptyLocal, textIconContentWeakLocal] {
                if (auto textIconContent = textIconContentWeakLocal.get()) {
                    SetupVolumeLayout(textIconContent);
                }
                ApplyVolumeIconViewsWidth();
                if (wasEmptyLocal && IsDualBoxStyle()) {
                    RefreshVolumeIcons();
                }
            });
        });
}

using VolumeSystemTrayIconDataModel_UpdateVolume_t =
    void(WINAPI*)(void*, float, bool, winrt::hstring*);
VolumeSystemTrayIconDataModel_UpdateVolume_t
    VolumeSystemTrayIconDataModel_UpdateVolume_Original;

using VolumeSystemTrayIconDataModel_get_CurrentData_t =
    HRESULT(WINAPI*)(void*, void**);
VolumeSystemTrayIconDataModel_get_CurrentData_t
    VolumeSystemTrayIconDataModel_get_CurrentData_Original;

using VolumeSystemTrayIconDataModel_CurrentData_t =
    void*(WINAPI*)(void*, void**);
VolumeSystemTrayIconDataModel_CurrentData_t
    VolumeSystemTrayIconDataModel_CurrentData_Original;

using TextIconContentViewModel_UpdateStyles_t = void(WINAPI*)(void*, void**);
TextIconContentViewModel_UpdateStyles_t
    TextIconContentViewModel_UpdateStyles_Original;

using TextIconContent_MeasureOverride_t =
    int(WINAPI*)(void*,
                 winrt::Windows::Foundation::Size,
                 winrt::Windows::Foundation::Size*);
TextIconContent_MeasureOverride_t TextIconContent_MeasureOverride_Original;

void WINAPI VolumeSystemTrayIconDataModel_UpdateVolume_Hook(
    void* pThis,
    float volumeLevel,
    bool isMuted,
    winrt::hstring* spatialSoundName) {
    Wh_Log(L"> tid=%u volume=%.2f muted=%d", GetCurrentThreadId(), volumeLevel,
           isMuted);

    {
        std::lock_guard<std::mutex> lock(g_volumeStateMutex);
        g_volumeLevel = volumeLevel;
        g_isMuted = isMuted;
        g_hasVolumeState = true;

        if (spatialSoundName) {
            g_spatialSoundName = *spatialSoundName;
        } else {
            g_spatialSoundName.clear();
        }
    }

    VolumeSystemTrayIconDataModel_UpdateVolume_Original(
        pThis, volumeLevel, isMuted, spatialSoundName);
}

HRESULT WINAPI
VolumeSystemTrayIconDataModel_get_CurrentData_Hook(void* pThis,
                                                   void** iconData) {
    g_gettingCurrentDataModel = pThis;
    HRESULT hr = VolumeSystemTrayIconDataModel_get_CurrentData_Original(
        pThis, iconData);
    g_gettingCurrentDataModel = nullptr;
    return hr;
}

void* WINAPI VolumeSystemTrayIconDataModel_CurrentData_Hook(void* pThis,
                                                            void** iconData) {
    Wh_Log(L">");

    void* ret =
        VolumeSystemTrayIconDataModel_CurrentData_Original(pThis, iconData);

    g_volumeIconData = nullptr;
    if (g_unloading) {
        return ret;
    }

    winrt::copy_from_abi(g_volumeIconData, *iconData);

    SafeXamlCall([pThis] {
        if (g_gettingCurrentDataModel) {
            winrt::Windows::Foundation::IInspectable dataModel = nullptr;
            winrt::copy_from_abi(dataModel, g_gettingCurrentDataModel);
            if (dataModel) {
                RememberVolumeDataModel(pThis, dataModel);
            }
        }
    });

    return ret;
}

void WINAPI TextIconContentViewModel_UpdateStyles_Hook(void* pThis,
                                                       void** iconData) {
    TextIconContentViewModel_UpdateStyles_Original(pThis, iconData);

    bool isVolume = (g_volumeIconData && iconData &&
                     *iconData == winrt::get_abi(g_volumeIconData));
    if (g_unloading || !g_hasVolumeState || !isVolume) {
        return;
    }

    Wh_Log(L"> tid=%u Volume view model %p", GetCurrentThreadId(), pThis);

    SafeXamlCall([pThis] {
        g_volumeText = FormatVolumeText(g_volumeLevel, g_isMuted);
        g_volumeTextChanged = true;

        if (IsDualBoxStyle()) {
            if (!g_trackedVolumeContents || g_trackedVolumeContents->empty()) {
                winrt::hstring triggerText{g_volumeText};
                TextIconContentViewModel_BaseText_Original(pThis, &triggerText);
                (void)winrt::detach_abi(triggerText);
                return;
            }
        } else {
            winrt::hstring baseText{g_volumeText};
            TextIconContentViewModel_BaseText_Original(pThis, &baseText);
            (void)winrt::detach_abi(baseText);

            winrt::hstring underlayText;
            TextIconContentViewModel_UnderlayText_Original(pThis, &underlayText);
            (void)winrt::detach_abi(underlayText);
        }

        UpdateAllVolumeLayouts();
        ApplyVolumeIconViewsWidth();
    });
}

int WINAPI TextIconContent_MeasureOverride_Hook(
    void* pThis,
    winrt::Windows::Foundation::Size size,
    winrt::Windows::Foundation::Size* resultSize) {
    int ret = TextIconContent_MeasureOverride_Original(pThis, size, resultSize);

    SafeXamlCall([pThis, resultSize] {
        FrameworkElement textIconContent = nullptr;
        ((IUnknown*)pThis)->QueryInterface(winrt::guid_of<FrameworkElement>(), winrt::put_abi(textIconContent));

        if (textIconContent && IsVolumeTextIconContent(textIconContent)) {
            if (resultSize && resultSize->Width > 0) {
                double measuredWithPadding =
                    static_cast<double>(resultSize->Width) + 6.0;
                if (measuredWithPadding > g_maxObservedWidth) {
                    g_maxObservedWidth = measuredWithPadding;
                    g_volumeTextChanged = true;
                    ApplyVolumeIconViewsWidth();
                }
            }

            if (IsVolumeIconViewLookupPending(textIconContent)) {
                LookUpVolumeIconView(textIconContent);
            }
        }
    });

    return ret;
}

void RefreshVolumeIcons() {
    float volumeLevel;
    bool isMuted;
    winrt::hstring spatialSoundName;
    {
        std::lock_guard<std::mutex> lock(g_volumeStateMutex);
        if (!g_hasVolumeState) {
            return;
        }
        volumeLevel = g_volumeLevel;
        isMuted = g_isMuted;
        spatialSoundName = g_spatialSoundName;
    }

    PruneVolumeDataModels();

    std::vector<std::pair<winrt::Windows::Foundation::IInspectable, void*>>
        targets;
    targets.reserve(g_volumeDataModels.size());
    for (const auto& dataModel : g_volumeDataModels) {
        if (auto strong = dataModel.weakRef.get()) {
            targets.emplace_back(std::move(strong), dataModel.pThis);
        }
    }

    for (const auto& [strong, pThis] : targets) {
        winrt::hstring soundNameCopy{spatialSoundName};
        VolumeSystemTrayIconDataModel_UpdateVolume_Original(
            pThis, volumeLevel, isMuted, &soundNameCopy);
        (void)winrt::detach_abi(soundNameCopy);
    }
}

bool HookSystemTraySymbols(HMODULE module) {
    // SystemTray.dll, Taskbar.View.dll
    WindhawkUtils::SYMBOL_HOOK symbolHooks[] = {
        {{LR"(public: void __cdecl winrt::SystemTray::implementation::VolumeSystemTrayIconDataModel::UpdateVolume(float,bool,struct winrt::hstring))"},
         &VolumeSystemTrayIconDataModel_UpdateVolume_Original,
         VolumeSystemTrayIconDataModel_UpdateVolume_Hook},
        {{LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::SystemTray::implementation::VolumeSystemTrayIconDataModel,struct winrt::SystemTray::IIconDataModel>::get_CurrentData(void * *))"},
         &VolumeSystemTrayIconDataModel_get_CurrentData_Original,
         VolumeSystemTrayIconDataModel_get_CurrentData_Hook},
        {{LR"(public: struct winrt::SystemTray::IconData __cdecl winrt::SystemTray::implementation::VolumeSystemTrayIconDataModel::CurrentData(void))"},
         &VolumeSystemTrayIconDataModel_CurrentData_Original,
         VolumeSystemTrayIconDataModel_CurrentData_Hook},
        {{LR"(private: void __cdecl winrt::SystemTray::implementation::TextIconContentViewModel::UpdateStyles(struct winrt::SystemTray::IconData const &))"},
         &TextIconContentViewModel_UpdateStyles_Original,
         TextIconContentViewModel_UpdateStyles_Hook},
        {{LR"(public: void __cdecl winrt::SystemTray::implementation::TextIconContentViewModel::BaseText(struct winrt::hstring))"},
         &TextIconContentViewModel_BaseText_Original},
        {{LR"(public: void __cdecl winrt::SystemTray::implementation::TextIconContentViewModel::UnderlayText(struct winrt::hstring))"},
         &TextIconContentViewModel_UnderlayText_Original},
        {{LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::SystemTray::implementation::TextIconContent,struct winrt::Windows::UI::Xaml::IFrameworkElementOverrides>::MeasureOverride(struct winrt::Windows::Foundation::Size,struct winrt::Windows::Foundation::Size *))"},
         &TextIconContent_MeasureOverride_Original,
         TextIconContent_MeasureOverride_Hook},
    };

    if (!WindhawkUtils::HookSymbols(module, symbolHooks,
                                    ARRAYSIZE(symbolHooks))) {
        Wh_Log(L"HookSymbols failed");
        return false;
    }

    return true;
}

VS_FIXEDFILEINFO* GetModuleVersionInfo(HMODULE hModule, UINT* puPtrLen) {
    void* pFixedFileInfo = nullptr;
    UINT uPtrLen = 0;

    if (HRSRC hRes = FindResource(hModule, MAKEINTRESOURCE(VS_VERSION_INFO), RT_VERSION)) {
        if (HGLOBAL hGlobal = LoadResource(hModule, hRes)) {
            if (void* pData = LockResource(hGlobal)) {
                if (!VerQueryValue(pData, L"\\", &pFixedFileInfo, &uPtrLen) || uPtrLen == 0) {
                    pFixedFileInfo = nullptr;
                    uPtrLen = 0;
                }
            }
        }
    }

    if (puPtrLen) {
        *puPtrLen = uPtrLen;
    }

    return static_cast<VS_FIXEDFILEINFO*>(pFixedFileInfo);
}

HMODULE GetSystemTrayModuleHandle() {
    HMODULE module = GetModuleHandle(L"SystemTray.dll");
    if (!module) {
        module = GetModuleHandle(L"Taskbar.View.dll");
        if (module) {
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

void LoadSettings() {
    auto displayStyle = WindhawkUtils::StringSetting::make(L"displayStyle");
    std::wstring_view ds = displayStyle.get();
    g_settings.displayStyle = (ds == L"number")        ? DisplayStyle::number
                              : (ds == L"prefix")      ? DisplayStyle::prefix
                              : (ds == L"emoji")       ? DisplayStyle::emoji
                              : (ds == L"glyphRight")  ? DisplayStyle::glyphRight
                              : (ds == L"glyphNumber") ? DisplayStyle::glyphNumber
                              : (ds == L"glyph")       ? DisplayStyle::glyphRight
                              : (ds == L"nativeIcon")  ? DisplayStyle::glyphRight
                              : (ds == L"nativeIconNumber")
                                  ? DisplayStyle::glyphNumber
                                  : DisplayStyle::percentage;

    auto elementPosition =
        WindhawkUtils::StringSetting::make(L"elementPosition");
    std::wstring_view ep = elementPosition.get();
    if (ep.empty()) {
        auto iconPos = WindhawkUtils::StringSetting::make(L"iconPosition");
        ep = iconPos.get();
    }
    g_settings.elementPosition =
        (ep == L"left") ? ElementPosition::left : ElementPosition::right;

    g_settings.customPrefix =
        WindhawkUtils::StringSetting::make(L"customPrefix");

    auto muteStyle = WindhawkUtils::StringSetting::make(L"muteStyle");
    std::wstring_view ms = muteStyle.get();
    g_settings.muteStyle = (ms == L"mute")        ? MuteStyle::mute
                           : (ms == L"zero")      ? MuteStyle::zero
                           : (ms == L"cross")     ? MuteStyle::cross
                           : (ms == L"emoji")     ? MuteStyle::emoji
                           : (ms == L"glyph")     ? MuteStyle::glyph
                           : (ms == L"glyphText") ? MuteStyle::glyphText
                           : (ms == L"glyphZero") ? MuteStyle::glyphZero
                           : (ms == L"custom")    ? MuteStyle::custom
                                                  : MuteStyle::mut;

    g_settings.customMuteText =
        WindhawkUtils::StringSetting::make(L"customMuteText");

    g_settings.iconSpacing = Wh_GetIntSetting(L"iconSpacing");

    g_settings.containerWidth = Wh_GetIntSetting(L"containerWidth");

    g_settings.fontSize = Wh_GetIntSetting(L"fontSize");
    if (g_settings.fontSize < 0) {
        g_settings.fontSize = 0;
    }

    g_settings.prefixSize = Wh_GetIntSetting(L"prefixSize");
    if (g_settings.prefixSize < 0) {
        g_settings.prefixSize = 0;
    }

    g_settings.iconSize = Wh_GetIntSetting(L"iconSize");
    if (g_settings.iconSize < 0) {
        g_settings.iconSize = 0;
    }

    g_settings.customFontFamily =
        WindhawkUtils::StringSetting::make(L"customFontFamily");

    g_maxObservedWidth = 0.0;

    g_settingsGeneration++;
}

bool ContainsCaseInsensitive(std::wstring_view text, std::wstring_view sub) {
    if (sub.empty()) {
        return true;
    }
    if (text.size() < sub.size()) {
        return false;
    }
    auto it = std::search(
        text.begin(), text.end(), sub.begin(), sub.end(),
        [](wchar_t c1, wchar_t c2) {
            return std::towlower(c1) == std::towlower(c2);
        });
    return it != text.end();
}

bool IsSecondaryExplorerProcess() {
    PCWSTR cmdLine = GetCommandLineW();
    if (!cmdLine || !*cmdLine) {
        return false;
    }

    std::wstring_view cmdView{cmdLine};
    return ContainsCaseInsensitive(cmdView, L"/factory") ||
           ContainsCaseInsensitive(cmdView, L"-Embedding") ||
           ContainsCaseInsensitive(cmdView, L"/separate");
}

BOOL Wh_ModInit() {
    Wh_Log(L">");

    if (IsSecondaryExplorerProcess()) {
        Wh_Log(L"Skipping secondary explorer process");
        return FALSE;
    }

    if (!g_volumeIconViews) {
        g_volumeIconViews.emplace();
    }
    if (!g_trackedVolumeContents) {
        g_trackedVolumeContents.emplace();
    }
    if (!g_autoRevokerList) {
        g_autoRevokerList.emplace();
    }

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
}

void Wh_ModBeforeUninit() {
    Wh_Log(L">");

    g_unloading = true;

    auto cleanupTask = [](void*) {
        if (g_autoRevokerList) {
            g_autoRevokerList->clear();
        }

        SafeXamlCall([] {
            RestoreAllVolumeLayouts();
            RefreshVolumeIcons();
            ApplyVolumeIconViewsWidth();
        });

        if (g_trackedVolumeContents) {
            g_trackedVolumeContents->clear();
            g_trackedVolumeContents.reset();
        }
        if (g_volumeIconViews) {
            g_volumeIconViews->clear();
            g_volumeIconViews.reset();
        }
        if (g_autoRevokerList) {
            g_autoRevokerList.reset();
        }

        g_volumeText.clear();
        g_volumeTextChanged = false;
        g_volumeIconData = nullptr;
        g_volumeDataModels.clear();
        g_spatialSoundName.clear();
        g_maxObservedWidth = 0.0;
    };

    bool cleanedUp = false;
    for (int attempt = 0; attempt < 5; ++attempt) {
        if (RunFromTaskbarThread(cleanupTask)) {
            cleanedUp = true;
            break;
        }
        Sleep(50);
    }

    if (!cleanedUp) {
        Wh_Log(
            L"Taskbar thread unreachable during uninit; skipping UI-affine "
            L"revoker reset to prevent RPC_E_WRONG_THREAD");
    }
}

void Wh_ModUninit() {
    Wh_Log(L">");
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");

    if (!RunFromTaskbarThread([](void*) {
            LoadSettings();

            SafeXamlCall([] {
                UpdateAllVolumeLayouts();
                RefreshVolumeIcons();
                ApplyVolumeIconViewsWidth();
            });
        })) {
        LoadSettings();
    }
}
