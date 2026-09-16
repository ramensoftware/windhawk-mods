// ==WindhawkMod==
// @id              taskbar-volume-percentage
// @name            Taskbar Volume Percentage Indicator
// @description     Shows the master volume percentage in the Windows 11 system tray volume icon
// @version         1.5.8
// @author          gilnett
// @github          https://github.com/gilnett
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject -lversion
// @license         GPL-3.0
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Taskbar Volume Percentage Indicator

Replaces the Windows 11 system tray volume icon with the current master volume
level, updated in real time.

![Taskbar Volume Percentage Preview 1](https://i.imgur.com/vjwali8.png)
![Taskbar Volume Percentage Preview 2](https://i.imgur.com/XEf8m50.png)
![Taskbar Volume Percentage Preview 3](https://i.imgur.com/27iyViO.png)
![Taskbar Volume Percentage Preview 4](https://i.imgur.com/1alQd1j.png)

## Settings

- **Display style**: Format of the volume indicator in the taskbar:
  - Percentage (`50%`)
  - Number only (`50`)
  - Prefix and percentage (`Vol 50%`, with custom prefix)
  - Icon and percentage (`🔊 50%`)
  - Percentage and native icon
  - Number and native icon
- **Custom prefix**: Text prefix used with the "Prefix and percentage" style (default: `Vol `).
- **Mute display style**: Style of the mute indicator when audio is muted:
  - MUT
  - Mute
  - 0%
  - Cross symbol (`✕`)
  - Mute emoji (`🔇`)
  - Native mute icon
  - Custom text
- **Custom mute text**: Text used when the "Custom text" mute style is selected (default: `Mute`).
- **Container width**: Width in device-independent pixels (DIPs) of the volume container to prevent adjacent icons from shifting when digit count changes (`0` for dynamic auto-calculation, `-1` for Windows default).

## Compatibility

- Only Windows 11 is supported.
- If the mod is enabled while Explorer is already running, the text appears
  after the next volume change.

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
  - glyphRight: Percentage and native icon
  - glyphNumber: Number and native icon
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
  - custom: Custom text
- customMuteText: "Mute"
  $name: Custom mute text
  $description: Used with the "Custom text" mute display style.
- fixedContainerWidth: 0
  $name: Container width
  $description: >-
    Width in device-independent pixels (DIPs) of the volume icon container to
    keep adjacent icons from shifting when the number of digits changes. Set to
    0 for an automatic width dynamically adapted to the text, or to -1 to leave
    the width to Windows.
*/
// ==/WindhawkModSettings==

#include <windhawk_api.h>
#include <windhawk_utils.h>

#include <algorithm>
#include <atomic>
#include <cmath>
#include <list>
#include <string>
#include <string_view>
#include <vector>

#undef GetCurrentTime

#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.Text.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
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

enum class MuteStyle {
    mut,
    mute,
    zero,
    cross,
    emoji,
    glyph,
    custom,
};

struct {
    DisplayStyle displayStyle;
    WindhawkUtils::StringSetting customPrefix;
    MuteStyle muteStyle;
    WindhawkUtils::StringSetting customMuteText;
    int fixedContainerWidth;
} g_settings;

std::atomic<bool> g_unloading;
std::atomic<bool> g_systemTrayModuleHooked;

// The state below is only touched on the taskbar UI thread: the hooks run
// there, and the mod callbacks marshal to it.

// Volume state, from the UpdateVolume hook.
bool g_hasVolumeState;
float g_volumeLevel;
bool g_isMuted;

// Dynamic observed width (DIPs) to adapt to active DPI scaling and long strings.
double g_maxObservedWidth = 0.0;

// Volume data models (one per system tray), kept to make them re-notify their
// view models on settings change and on unload. The raw pointer is the C++
// `this` for calling UpdateVolume, valid while the weak reference resolves.
struct VolumeDataModel {
    void* pThis;
    winrt::weak_ref<winrt::Windows::Foundation::IInspectable> weakRef;
};
std::vector<VolumeDataModel> g_volumeDataModels;

// The interface pointer of the data model whose get_CurrentData ABI thunk is
// in progress. The thunk calls CurrentData on the C++ object, which pairs the
// two.
void* g_gettingCurrentDataModel;

// The spatial sound name UpdateVolume was last given. It only affects the
// tooltip, and UpdateVolume gets it back when the mod re-runs it.
winrt::hstring g_spatialSoundName;

// The IconData most recently produced by a volume data model. The view model
// receives that same object right afterwards on the same thread, so pointer
// identity tells the volume icon's view model apart from other text icons.
[[clang::no_destroy]] winrt::Windows::Foundation::IUnknown g_volumeIconData;

// The text the mod last set on the volume view model. The XAML binding puts
// it in the InnerTextBlock of the TextIconContent showing the volume icon,
// which is how that element is told apart from the other text icons.
std::wstring g_volumeText;

// Set when the text changed and the element showing it is to be looked up in
// the next measure passes.
bool g_volumeTextChanged;

// The view model of the volume text icon. Held temporarily during initial
// layout lookup to restore the native speaker glyph, then cleared.
void* g_volumeViewModel = nullptr;

// The native speaker glyph captured from Windows.
winrt::hstring g_nativeVolumeGlyph;
bool g_capturingVolumeGlyph = false;

// IconView elements that were given a custom width, with snapshot for reversibility.
struct TrackedIconView {
    winrt::weak_ref<FrameworkElement> iconView;
    bool hasCustomWidth = false;
    winrt::Windows::Foundation::IInspectable origMinWidth{nullptr};
    winrt::Windows::Foundation::IInspectable origWidth{nullptr};
    winrt::Windows::Foundation::IInspectable origAlignment{nullptr};
};
std::vector<TrackedIconView> g_volumeIconViews;

// Tracked TextIconContent and its sub-box hierarchy for dual-column mode.
struct TrackedVolumeContent {
    winrt::weak_ref<FrameworkElement> textIconContent;
    winrt::weak_ref<Controls::Grid> containerGrid;
    winrt::weak_ref<FrameworkElement> baseElement;
    winrt::weak_ref<FrameworkElement> underlayElement;
    winrt::weak_ref<Controls::TextBlock> subBlock;
    bool isDualBoxConfigured = false;
    winrt::Windows::Foundation::IInspectable origUnderlayVisibility{nullptr};
    winrt::Windows::Foundation::IInspectable origBaseVisibility{nullptr};
    winrt::Windows::Foundation::IInspectable origUnderlayAlignment{nullptr};
    winrt::Windows::Foundation::IInspectable origBaseAlignment{nullptr};
    winrt::Windows::Foundation::IInspectable origUnderlayColumn{nullptr};
    winrt::Windows::Foundation::IInspectable origBaseColumn{nullptr};
};
std::vector<TrackedVolumeContent> g_trackedVolumeContents;

using FrameworkElementLayoutUpdatedEventRevoker = winrt::impl::event_revoker<
    IFrameworkElement,
    &winrt::impl::abi<IFrameworkElement>::type::remove_LayoutUpdated>;

// Layout properties set from inside a measure pass are not picked up by it,
// so the width of a view found there is applied once the pass is over.
[[clang::no_destroy]] std::list<FrameworkElementLayoutUpdatedEventRevoker>
    g_autoRevokerList;

////////////////////////////////////////////////////////////////////////////////
// Taskbar thread

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
    SendMessage(hWnd, runFromWindowThreadRegisteredMsg, 0, (LPARAM)&param);

    UnhookWindowsHookEx(hook);

    return true;
}

bool RunFromTaskbarThread(RunFromWindowThreadProc_t proc) {
    HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();
    return hTaskbarWnd && RunFromWindowThread(hTaskbarWnd, proc, nullptr);
}

////////////////////////////////////////////////////////////////////////////////
// Visual tree

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

////////////////////////////////////////////////////////////////////////////////
// Text and width

bool IsDualBoxStyle() {
    return g_settings.displayStyle == DisplayStyle::glyphRight ||
           g_settings.displayStyle == DisplayStyle::glyphNumber;
}

std::wstring FormatVolumeText(float volumeLevel, bool isMuted) {
    if (isMuted) {
        return (g_settings.muteStyle == MuteStyle::mute)    ? L"Mute"
               : (g_settings.muteStyle == MuteStyle::zero)  ? L"0%"
               : (g_settings.muteStyle == MuteStyle::cross) ? L"\u2715"
               : (g_settings.muteStyle == MuteStyle::emoji) ? L"\U0001F507"
               : (g_settings.muteStyle == MuteStyle::glyph) ? L"\uE74F"
               : (g_settings.muteStyle == MuteStyle::custom)
                   ? g_settings.customMuteText.get()
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
        return (prefix ? prefix : L"") + number + L"%";
    }
    if (g_settings.displayStyle == DisplayStyle::emoji) {
        return L"\U0001F50A " + number + L"%";
    }

    return number + L"%";
}

std::wstring GetNativeVolumeGlyph(float volumeLevel, bool isMuted) {
    if (isMuted) {
        return L"\uE74F";
    }

    int percentage =
        std::clamp((int)std::lround(volumeLevel * 100.0f), 0, 100);
    return (percentage == 0)   ? L"\uE992"
           : (percentage < 33) ? L"\uE993"
           : (percentage < 66) ? L"\uE994"
                               : L"\uE995";
}

// Calculates dynamic baseline width adapted to custom prefix length and mute text.
double CalculateAutoWidth() {
    std::wstring maxVolText = FormatVolumeText(1.0f, false);
    std::wstring muteText = FormatVolumeText(0.0f, true);
    size_t maxLen = (std::max)(maxVolText.length(), muteText.length());

    // Compact padding (~8 DIPs) + Segoe UI Variable character metrics (~7.0 DIPs/char)
    double estimatedWidth = 8.0 + (static_cast<double>(maxLen) * 7.0);
    if (IsDualBoxStyle()) {
        estimatedWidth += 20.0; // Native speaker icon (~16 DIPs) + column gap (4 DIPs)
    }

    double minFloor = (g_settings.displayStyle == DisplayStyle::glyphRight)    ? 54.0
                      : (g_settings.displayStyle == DisplayStyle::emoji)       ? 50.0
                      : (g_settings.displayStyle == DisplayStyle::glyphNumber) ? 48.0
                      : (g_settings.displayStyle == DisplayStyle::prefix)      ? 42.0
                      : (g_settings.displayStyle == DisplayStyle::percentage)  ? 38.0
                                                                               : 30.0;

    return (std::max)(estimatedWidth, minFloor);
}

// Zero means leaving the width to Windows.
double GetContainerWidth() {
    if (g_unloading || g_settings.fixedContainerWidth < 0) {
        return 0;
    }

    if (g_settings.fixedContainerWidth > 0) {
        return g_settings.fixedContainerWidth;
    }

    return (std::max)(CalculateAutoWidth(), g_maxObservedWidth);
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

////////////////////////////////////////////////////////////////////////////////
// Object tracking

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
    std::erase_if(g_volumeIconViews,
                  [](const auto& tracked) { return !tracked.iconView.get(); });

    for (const auto& tracked : g_volumeIconViews) {
        if (tracked.iconView.get() == iconView) {
            return;
        }
    }

    Wh_Log(L"Volume icon view %p", winrt::get_abi(iconView));
    g_volumeIconViews.push_back({iconView});
}

void ApplyVolumeIconViewsWidth() {
    for (auto& tracked : g_volumeIconViews) {
        if (auto iconView = tracked.iconView.get()) {
            ApplyIconViewWidth(tracked, iconView);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
// Layout management

void PruneTrackedVolumeContents() {
    std::erase_if(g_trackedVolumeContents, [](const auto& tracked) {
        return !tracked.textIconContent.get();
    });
}

TrackedVolumeContent* FindTrackedVolumeContent(
    FrameworkElement const& textIconContent) {
    for (auto& tracked : g_trackedVolumeContents) {
        if (tracked.textIconContent.get() == textIconContent) {
            return &tracked;
        }
    }

    return nullptr;
}

void RestoreVolumeLayout(TrackedVolumeContent& tracked) {
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

    if (!tracked.isDualBoxConfigured) {
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
        restoreProp(base, UIElement::VisibilityProperty(),
                    tracked.origBaseVisibility);
        restoreProp(base, FrameworkElement::HorizontalAlignmentProperty(),
                    tracked.origBaseAlignment);
        restoreProp(base, Controls::Grid::ColumnProperty(),
                    tracked.origBaseColumn);
    }
    if (auto underlay = tracked.underlayElement.get()) {
        restoreProp(underlay, UIElement::VisibilityProperty(),
                    tracked.origUnderlayVisibility);
        restoreProp(underlay, FrameworkElement::HorizontalAlignmentProperty(),
                    tracked.origUnderlayAlignment);
        restoreProp(underlay, Controls::Grid::ColumnProperty(),
                    tracked.origUnderlayColumn);
    }

    tracked.isDualBoxConfigured = false;
}

void RestoreAllVolumeLayouts() {
    g_autoRevokerList.clear();
    for (auto& tracked : g_trackedVolumeContents) {
        RestoreVolumeLayout(tracked);
    }
    g_volumeViewModel = nullptr;
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
        g_trackedVolumeContents.push_back({
            textIconContent,
            containerGrid,
            baseElement,
            underlayElement,
            nullptr,
            false,
        });
        tracked = &g_trackedVolumeContents.back();
    } else {
        tracked->containerGrid = containerGrid;
        tracked->baseElement = baseElement;
        tracked->underlayElement = underlayElement;
    }

    if (!IsDualBoxStyle()) {
        RestoreVolumeLayout(*tracked);
        return;
    }

    Controls::TextBlock subBlock = nullptr;
    if (auto existingSubBlock = tracked->subBlock.get()) {
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
        subBlock.FontFamily(
            Media::FontFamily(L"Segoe UI Variable Text, Segoe UI"));
        subBlock.FontSize(12.0);

        containerGrid.Children().Append(subBlock);
        tracked->subBlock = subBlock;
    }

    FrameworkElement textBlockEl =
        FindChildByName(baseElement, L"InnerTextBlock");
    if (textBlockEl) {
        if (auto innerTextBlock =
                textBlockEl.try_as<Controls::TextBlock>()) {
            subBlock.Foreground(innerTextBlock.Foreground());
            subBlock.FontWeight(innerTextBlock.FontWeight());
        }
    }

    auto applyLayout = [](FrameworkElement const& el, int col, Visibility vis) {
        if (el) {
            el.Visibility(vis);
            el.HorizontalAlignment(HorizontalAlignment::Center);
            Controls::Grid::SetColumn(el, col);
        }
    };

    if (!tracked->isDualBoxConfigured) {
        if (auto base = tracked->baseElement.get()) {
            tracked->origBaseVisibility =
                base.ReadLocalValue(UIElement::VisibilityProperty());
            tracked->origBaseAlignment = base.ReadLocalValue(
                FrameworkElement::HorizontalAlignmentProperty());
            tracked->origBaseColumn =
                base.ReadLocalValue(Controls::Grid::ColumnProperty());
        }
        if (auto underlay = tracked->underlayElement.get()) {
            tracked->origUnderlayVisibility =
                underlay.ReadLocalValue(UIElement::VisibilityProperty());
            tracked->origUnderlayAlignment = underlay.ReadLocalValue(
                FrameworkElement::HorizontalAlignmentProperty());
            tracked->origUnderlayColumn =
                underlay.ReadLocalValue(Controls::Grid::ColumnProperty());
        }
    }

    if (g_isMuted) {
        // In mute state, display a single centered indicator rather than
        // two indicators side-by-side.
        containerGrid.ColumnDefinitions().Clear();
        bool isMuteGlyph = (g_settings.muteStyle == MuteStyle::glyph);

        if (isMuteGlyph) {
            subBlock.Text(L"");
            subBlock.Visibility(Visibility::Collapsed);
        } else {
            subBlock.Text(g_volumeText);
            subBlock.HorizontalAlignment(HorizontalAlignment::Center);
            subBlock.Margin(Thickness{0.0, 0.0, 0.0, 0.0});
            subBlock.Visibility(Visibility::Visible);
            Controls::Grid::SetColumn(subBlock, 0);
        }

        Visibility baseVis =
            isMuteGlyph ? Visibility::Visible : Visibility::Collapsed;
        applyLayout(baseElement, 0, baseVis);
        applyLayout(underlayElement, 0, baseVis);
    } else {
        // Unmuted: dual-column layout (percentage on left, speaker on right).
        if (containerGrid.ColumnDefinitions().Size() < 2) {
            containerGrid.ColumnDefinitions().Clear();
            Controls::ColumnDefinition col0, col1;
            col0.Width(GridLength{1.0, GridUnitType::Auto});
            col1.Width(GridLength{1.0, GridUnitType::Auto});
            containerGrid.ColumnDefinitions().Append(col0);
            containerGrid.ColumnDefinitions().Append(col1);
        }

        subBlock.Text(g_volumeText);
        subBlock.HorizontalAlignment(HorizontalAlignment::Right);
        subBlock.Margin(Thickness{0.0, 0.0, 4.0, 0.0});
        subBlock.Visibility(Visibility::Visible);
        Controls::Grid::SetColumn(subBlock, 0);

        applyLayout(baseElement, 1, Visibility::Visible);
        applyLayout(underlayElement, 1, Visibility::Visible);
    }

    tracked->isDualBoxConfigured = true;
}

void UpdateAllVolumeLayouts() {
    PruneTrackedVolumeContents();

    if (g_trackedVolumeContents.empty()) {
        g_volumeViewModel = nullptr;
    }

    std::vector<FrameworkElement> targets;
    targets.reserve(g_trackedVolumeContents.size());
    for (auto& tracked : g_trackedVolumeContents) {
        if (auto textIconContent = tracked.textIconContent.get()) {
            targets.push_back(textIconContent);
        }
    }

    for (auto const& textIconContent : targets) {
        SetupVolumeLayout(textIconContent);
    }
}

// SystemTray.TextIconContent > Grid#ContainerGrid >
// SystemTray.AdaptiveTextBlock#Base > TextBlock#InnerTextBlock
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

// Whether the element showing the volume text has to be looked up: after the
// text changed, or while no IconView is known to show it (Explorer start, tray
// rebuild).
bool IsVolumeIconViewLookupPending(FrameworkElement const& textIconContent) {
    if (g_unloading || g_volumeText.empty()) {
        return false;
    }

    return g_volumeTextChanged || !FindTrackedVolumeContent(textIconContent);
}

// Setters taking winrt::hstring by value. On x64 that is a pointer to a
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

void RestoreNativeBaseGlyph() {
    if (g_volumeViewModel) {
        winrt::hstring nativeText{
            !g_nativeVolumeGlyph.empty()
                ? g_nativeVolumeGlyph
                : winrt::hstring(
                      GetNativeVolumeGlyph(g_volumeLevel, g_isMuted))};
        TextIconContentViewModel_BaseText_Original(g_volumeViewModel,
                                                   &nativeText);
        g_volumeViewModel = nullptr;
    }
}

// Finds the IconView hosting the volume icon once the text set on the view
// model reached the element. Done from the element's measure, which follows a
// text change and the creation of a new element.
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

    bool wasEmpty = g_trackedVolumeContents.empty();

    SetupVolumeLayout(textIconContent);

    if (wasEmpty && IsDualBoxStyle()) {
        RestoreNativeBaseGlyph();
    }

    g_volumeTextChanged = false;

    RememberVolumeIconView(iconView);

    g_autoRevokerList.emplace_back();
    auto autoRevokerIt = g_autoRevokerList.end();
    --autoRevokerIt;

    *autoRevokerIt = iconView.LayoutUpdated(
        winrt::auto_revoke_t{},
        [autoRevokerIt, wasEmpty](
            winrt::Windows::Foundation::IInspectable const&,
            winrt::Windows::Foundation::IInspectable const&) {
            const bool wasEmptyLocal = wasEmpty;
            g_autoRevokerList.erase(autoRevokerIt);

            SafeXamlCall([wasEmptyLocal] {
                ApplyVolumeIconViewsWidth();
                if (wasEmptyLocal && IsDualBoxStyle()) {
                    RefreshVolumeIcons();
                }
            });
        });
}

////////////////////////////////////////////////////////////////////////////////
// Hooks

// The hstring is the spatial sound format name, which only shows up in the
// tooltip. The icon glyph is derived from the level and the mute state.
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
    Wh_Log(L"> volume=%.2f muted=%d", volumeLevel, isMuted);

    g_volumeLevel = volumeLevel;
    g_isMuted = isMuted;
    g_hasVolumeState = true;

    if (spatialSoundName) {
        g_spatialSoundName = *spatialSoundName;
    } else {
        g_spatialSoundName.clear();
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

void WINAPI TextIconContentViewModel_BaseText_Hook(void* pThis,
                                                   winrt::hstring* text) {
    if (g_capturingVolumeGlyph && text && !text->empty()) {
        g_nativeVolumeGlyph = *text;
    }
    TextIconContentViewModel_BaseText_Original(pThis, text);
}

void WINAPI TextIconContentViewModel_UpdateStyles_Hook(void* pThis,
                                                       void** iconData) {
    bool isVolume = (g_volumeIconData && iconData &&
                     *iconData == winrt::get_abi(g_volumeIconData));
    if (isVolume) {
        g_capturingVolumeGlyph = true;
        if (IsDualBoxStyle() && g_trackedVolumeContents.empty()) {
            g_volumeViewModel = pThis;
        } else {
            g_volumeViewModel = nullptr;
        }
    }

    struct FlagGuard {
        bool* flag;
        ~FlagGuard() {
            if (flag) {
                *flag = false;
            }
        }
    } flagGuard{isVolume ? &g_capturingVolumeGlyph : nullptr};

    TextIconContentViewModel_UpdateStyles_Original(pThis, iconData);

    if (g_unloading || !g_hasVolumeState || !isVolume) {
        return;
    }

    Wh_Log(L"> Volume view model %p", pThis);

    SafeXamlCall([pThis] {
        g_volumeText = FormatVolumeText(g_volumeLevel, g_isMuted);
        g_volumeTextChanged = true;

        if (IsDualBoxStyle()) {
            if (g_trackedVolumeContents.empty()) {
                winrt::hstring triggerText{g_volumeText};
                TextIconContentViewModel_BaseText_Original(pThis, &triggerText);
                return;
            }
        } else {
            winrt::hstring baseText{g_volumeText};
            TextIconContentViewModel_BaseText_Original(pThis, &baseText);

            // The volume bars outline drawn behind the glyph.
            winrt::hstring underlayText;
            TextIconContentViewModel_UnderlayText_Original(pThis, &underlayText);
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
            // Track maximum real XAML-measured width strictly for the volume icon
            // to dynamically adapt to DPI and font sizes without interference from
            // other tray text icons (e.g. keyboard language switcher ENG/FRA).
            if (resultSize && resultSize->Width > 0) {
                double measuredWithPadding =
                    static_cast<double>(resultSize->Width) + 6.0;
                if (measuredWithPadding > g_maxObservedWidth) {
                    g_maxObservedWidth = measuredWithPadding;
                    g_volumeTextChanged = true;
                }
            }

            if (IsVolumeIconViewLookupPending(textIconContent)) {
                LookUpVolumeIconView(textIconContent);
            }
        }
    });

    return ret;
}

////////////////////////////////////////////////////////////////////////////////
// Refresh

// Makes the volume data models recompute their icon and re-notify their view
// models, the same path a real volume change takes. With the mod unloading,
// that restores the Windows glyph.
void RefreshVolumeIcons() {
    if (!g_hasVolumeState) {
        return;
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
        winrt::hstring spatialSoundName = g_spatialSoundName;
        VolumeSystemTrayIconDataModel_UpdateVolume_Original(
            pThis, g_volumeLevel, g_isMuted, &spatialSoundName);
    }
}

////////////////////////////////////////////////////////////////////////////////
// Module handling

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
         &TextIconContentViewModel_BaseText_Original,
         TextIconContentViewModel_BaseText_Hook},
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

////////////////////////////////////////////////////////////////////////////////
// Mod lifetime

void LoadSettings() {
    auto displayStyle = WindhawkUtils::StringSetting::make(L"displayStyle");
    std::wstring_view ds = displayStyle.get();
    g_settings.displayStyle = (ds == L"number")        ? DisplayStyle::number
                              : (ds == L"prefix")      ? DisplayStyle::prefix
                              : (ds == L"emoji")       ? DisplayStyle::emoji
                              : (ds == L"glyphRight")  ? DisplayStyle::glyphRight
                              : (ds == L"glyphNumber") ? DisplayStyle::glyphNumber
                                                       : DisplayStyle::percentage;

    g_settings.customPrefix =
        WindhawkUtils::StringSetting::make(L"customPrefix");

    auto muteStyle = WindhawkUtils::StringSetting::make(L"muteStyle");
    std::wstring_view ms = muteStyle.get();
    g_settings.muteStyle = (ms == L"mute")     ? MuteStyle::mute
                           : (ms == L"zero")   ? MuteStyle::zero
                           : (ms == L"cross")  ? MuteStyle::cross
                           : (ms == L"emoji")  ? MuteStyle::emoji
                           : (ms == L"glyph")  ? MuteStyle::glyph
                           : (ms == L"custom") ? MuteStyle::custom
                                               : MuteStyle::mut;

    g_settings.customMuteText =
        WindhawkUtils::StringSetting::make(L"customMuteText");

    g_settings.fixedContainerWidth = Wh_GetIntSetting(L"fixedContainerWidth");
    g_maxObservedWidth = 0.0;
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

    RunFromTaskbarThread([](void*) {
        g_autoRevokerList.clear();

        SafeXamlCall([] {
            RestoreAllVolumeLayouts();
            RefreshVolumeIcons();
            ApplyVolumeIconViewsWidth();
        });

        g_trackedVolumeContents.clear();
        g_volumeIconViews.clear();
        g_volumeText.clear();
        g_volumeTextChanged = false;
        g_volumeIconData = nullptr;
        g_volumeDataModels.clear();
        g_spatialSoundName.clear();
        g_maxObservedWidth = 0.0;
        g_volumeViewModel = nullptr;
        g_nativeVolumeGlyph.clear();
        g_capturingVolumeGlyph = false;
    });
}

void Wh_ModUninit() {
    Wh_Log(L">");
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");

    // Settings are read by the hooks on the taskbar thread.
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
