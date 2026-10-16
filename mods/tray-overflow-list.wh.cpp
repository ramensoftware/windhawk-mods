// ==WindhawkMod==
// @id              tray-overflow-list
// @name            Tray overflow list
// @description     Show Windows 11 notification-area overflow icons as a list.
// @version         1.0.0
// @author          zty012
// @github          https://github.com/zty012
// @twitter         https://x.com/zty012x
// @license         MIT
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject -lversion
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Tray overflow list

Changes the Windows 11 notification-area overflow popup into a vertical list.

- Displays the tray icon and its name.
- Preserves the native popup padding and theme.
- Preserves the original tray icon control and input handling.
- Uses event-driven updates instead of a permanent timer.
- Supports dynamic tray icons added while the popup is open.
- Releases XAML objects on their owning UI thread.

The implementation targets Windows 11 Explorer and uses undocumented
SystemTray/XAML implementation details.

## Screenshots

See [my Twitter post](https://x.com/zty012x/status/2109292804828753993)
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- listWidth: 300
  $name: List width
  $description: Width of the list content in pixels.

- rowHeight: 40
  $name: Row height
  $description: Height of each tray icon row in pixels.

- iconAreaWidth: 40
  $name: Icon area width
  $description: Width reserved for the tray icon.

- fontSize: 13
  $name: Label font size
  $description: Font size used for tray icon names.

- maxHeight: 840
  $name: Maximum popup height
  $description: Maximum height of the overflow popup.

- scrollbarReserve: 9
  $name: Scrollbar reserve
  $description: Width reserved for the vertical scrollbar.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <winver.h>
#include <windhawk_utils.h>

#include <algorithm>
#include <atomic>
#include <cmath>
#include <limits>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <vector>

#undef GetCurrentTime

#include <winrt/base.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Automation.h>
#include <winrt/Windows.UI.Xaml.Automation.Peers.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Data.h>
#include <winrt/Windows.UI.Xaml.Media.h>

namespace WF = winrt::Windows::Foundation;
namespace X = winrt::Windows::UI::Xaml;
namespace C = winrt::Windows::UI::Xaml::Controls;
namespace A = winrt::Windows::UI::Xaml::Automation;
namespace AP = winrt::Windows::UI::Xaml::Automation::Peers;
namespace D = winrt::Windows::UI::Xaml::Data;
namespace M = winrt::Windows::UI::Xaml::Media;

namespace {

struct Settings {
    double listWidth = 300.0;
    double rowHeight = 40.0;
    double iconAreaWidth = 40.0;
    double fontSize = 13.0;
    double maxHeight = 840.0;
    double scrollbarReserve = 9.0;
};

Settings g_settings;

constexpr double kLabelGap = 4.0;
constexpr double kLabelRightInset = 12.0;
constexpr double kScreenVerticalReserve = 96.0;

constexpr UINT kCleanupMessage = WM_APP + 0x517;

constexpr wchar_t kWindowClass[] =
    L"Windhawk.TrayOverflowList.PrivateWindow.1_0";

constexpr wchar_t kLabelName[] =
    L"WindhawkTrayOverflowListLabel";

constexpr wchar_t kIconViewClass[] =
    L"SystemTray.NotifyIconView";

constexpr double kAuto =
    std::numeric_limits<double>::quiet_NaN();

constexpr double kInfinity =
    std::numeric_limits<double>::infinity();

std::atomic<bool> g_stopping{false};
std::atomic<int> g_hookState{0};

HMODULE g_self = nullptr;
ATOM g_windowClass = 0;

std::mutex g_windowsMutex;

using InitializeFn = void(WINAPI*)(void*);
InitializeFn g_initializeOriginal = nullptr;

using IconViewCtorFn = void*(WINAPI*)(void*);
IconViewCtorFn g_iconViewCtorOriginal = nullptr;

using LoadLibraryExWFn = decltype(&LoadLibraryExW);
LoadLibraryExWFn g_loadLibraryExWOriginal = nullptr;

struct State;

void LoadSettings() {
    g_settings.listWidth = std::clamp(
        static_cast<double>(Wh_GetIntSetting(L"listWidth")),
        160.0,
        800.0);

    g_settings.rowHeight = std::clamp(
        static_cast<double>(Wh_GetIntSetting(L"rowHeight")),
        24.0,
        96.0);

    g_settings.iconAreaWidth = std::clamp(
        static_cast<double>(Wh_GetIntSetting(L"iconAreaWidth")),
        16.0,
        96.0);

    g_settings.fontSize = std::clamp(
        static_cast<double>(Wh_GetIntSetting(L"fontSize")),
        8.0,
        32.0);

    g_settings.maxHeight = std::clamp(
        static_cast<double>(Wh_GetIntSetting(L"maxHeight")),
        200.0,
        2000.0);

    g_settings.scrollbarReserve = std::clamp(
        static_cast<double>(Wh_GetIntSetting(L"scrollbarReserve")),
        0.0,
        32.0);
}

void LogException(PCWSTR operation) noexcept {
    HRESULT hr = winrt::to_hresult();

    Wh_Log(
        L"%s failed: 0x%08X",
        operation,
        static_cast<unsigned int>(hr));
}

bool SameObject(
    WF::IInspectable const& first,
    WF::IInspectable const& second) {

    if (!first || !second) {
        return !first && !second;
    }

    auto firstUnknown = first.as<IUnknown>();
    auto secondUnknown = second.as<IUnknown>();

    return firstUnknown.get() == secondUnknown.get();
}

X::FrameworkElement FindDirectChild(
    X::DependencyObject const& parent,
    PCWSTR name = nullptr,
    PCWSTR className = nullptr) {

    if (!parent) {
        return nullptr;
    }

    int count =
        M::VisualTreeHelper::GetChildrenCount(parent);

    for (int i = 0; i < count; ++i) {
        auto child =
            M::VisualTreeHelper::GetChild(parent, i)
                .try_as<X::FrameworkElement>();

        if (!child) {
            continue;
        }

        if (name && child.Name() != name) {
            continue;
        }

        if (className &&
            winrt::get_class_name(child) != className) {
            continue;
        }

        return child;
    }

    return nullptr;
}

X::FrameworkElement FindDescendant(
    X::DependencyObject const& parent,
    PCWSTR className,
    int depth = 0) {

    if (!parent || depth > 24) {
        return nullptr;
    }

    int count =
        M::VisualTreeHelper::GetChildrenCount(parent);

    for (int i = 0; i < count; ++i) {
        auto child =
            M::VisualTreeHelper::GetChild(parent, i);

        auto element =
            child.try_as<X::FrameworkElement>();

        if (element &&
            element.Name() != kLabelName &&
            winrt::get_class_name(element) == className) {
            return element;
        }

        if (auto result =
                FindDescendant(child, className, depth + 1)) {
            return result;
        }
    }

    return nullptr;
}

bool IsDescendantOf(
    X::DependencyObject const& child,
    X::DependencyObject const& possibleParent) {

    auto current = child;

    while (current) {
        if (SameObject(current, possibleParent)) {
            return true;
        }

        current = M::VisualTreeHelper::GetParent(current);
    }

    return false;
}

std::wstring NormalizeText(
    winrt::hstring const& source) {

    std::wstring text(source.c_str(), source.size());

    auto newline =
        text.find_first_of(L"\r\n");

    if (newline != std::wstring::npos) {
        text.resize(newline);
    }

    for (auto& character : text) {
        if (character == L'\t') {
            character = L' ';
        }
    }

    auto first =
        text.find_first_not_of(L' ');

    if (first == std::wstring::npos) {
        return {};
    }

    auto last =
        text.find_last_not_of(L' ');

    text =
        text.substr(first, last - first + 1);

    if (text.size() > 256) {
        text.resize(256);
    }

    return text;
}

std::wstring ReadInspectableText(
    WF::IInspectable const& object,
    int depth = 0) {

    if (!object || depth > 8) {
        return {};
    }

    if (auto value =
            object.try_as<WF::IPropertyValue>()) {

        if (value.Type() ==
            WF::PropertyType::String) {
            return NormalizeText(
                value.GetString());
        }
    }

    if (auto block =
            object.try_as<C::TextBlock>()) {
        return NormalizeText(block.Text());
    }

    if (auto tooltip =
            object.try_as<C::ToolTip>()) {
        return ReadInspectableText(
            tooltip.Content(),
            depth + 1);
    }

    if (auto content =
            object.try_as<C::ContentControl>()) {
        return ReadInspectableText(
            content.Content(),
            depth + 1);
    }

    return {};
}

std::wstring ReadIconName(
    X::FrameworkElement const& element,
    int depth = 0) {

    if (!element ||
        depth > 12 ||
        element.Name() == kLabelName) {
        return {};
    }

    auto automationName =
        NormalizeText(
            A::AutomationProperties::GetName(element));

    if (!automationName.empty()) {
        return automationName;
    }

    auto tooltipText =
        ReadInspectableText(
            C::ToolTipService::GetToolTip(element));

    if (!tooltipText.empty()) {
        return tooltipText;
    }

    int count =
        M::VisualTreeHelper::GetChildrenCount(element);

    for (int i = 0; i < count; ++i) {
        auto child =
            M::VisualTreeHelper::GetChild(element, i)
                .try_as<X::FrameworkElement>();

        if (!child) {
            continue;
        }

        auto name =
            ReadIconName(child, depth + 1);

        if (!name.empty()) {
            return name;
        }
    }

    return {};
}

double HorizontalExtent(
    X::Thickness const& thickness) {

    return thickness.Left + thickness.Right;
}

double VerticalExtent(
    X::Thickness const& thickness) {

    return thickness.Top + thickness.Bottom;
}

struct SavedProperty {
    winrt::weak_ref<X::FrameworkElement> element;
    X::DependencyProperty property{nullptr};
    WF::IInspectable localValue{nullptr};
    D::Binding binding{nullptr};
};

struct LoadedHandler {
    winrt::weak_ref<X::FrameworkElement> element;
    winrt::event_token token{};
};

struct NameHandler {
    winrt::weak_ref<X::FrameworkElement> element;
    winrt::event_token token{};
};

struct LabelEntry {
    winrt::weak_ref<X::FrameworkElement> icon;
    winrt::weak_ref<C::Grid> container;
    winrt::weak_ref<C::TextBlock> label;
};

struct State {
    DWORD threadId = GetCurrentThreadId();

    winrt::weak_ref<C::Grid> root;
    winrt::weak_ref<C::Grid> scrollParent;

    C::ScrollViewer scroll{nullptr};
    C::ItemsControl items{nullptr};

    std::vector<SavedProperty> savedProperties;
    std::vector<LoadedHandler> loadedHandlers;
    std::vector<NameHandler> nameHandlers;
    std::vector<LabelEntry> labels;

    uint32_t originalChildIndex = 0;

    bool updating = false;
    bool disabled = false;

    explicit State(C::Grid const& rootGrid)
        : root(rootGrid) {
    }

    void SaveProperty(
        X::FrameworkElement const& element,
        X::DependencyProperty const& property) {

        for (auto const& saved : savedProperties) {
            auto existing = saved.element.get();

            if (existing &&
                SameObject(existing, element) &&
                saved.property == property) {
                return;
            }
        }

        SavedProperty saved;
        saved.element = element;
        saved.property = property;

        if (auto expression =
                element.GetBindingExpression(property)) {
            saved.binding =
                expression.ParentBinding();
        } else {
            saved.localValue =
                element.ReadLocalValue(property);
        }

        savedProperties.push_back(std::move(saved));
    }

    template<typename T>
    void SetProperty(
        X::FrameworkElement const& element,
        X::DependencyProperty const& property,
        T const& value) {

        if (!element) {
            return;
        }

        SaveProperty(element, property);

        element.SetValue(
            property,
            winrt::box_value(value));
    }

    void SetDouble(
        X::FrameworkElement const& element,
        X::DependencyProperty const& property,
        double value) {

        if (!element) {
            return;
        }

        auto currentValue =
            element.GetValue(property);

        double current = 0.0;

        try {
            current =
                winrt::unbox_value<double>(
                    currentValue);
        } catch (...) {
            current = 0.0;
        }

        if (current != value &&
            !(std::isnan(current) &&
              std::isnan(value))) {
            SetProperty(element, property, value);
        }
    }

    void SetAutoWidth(
        X::FrameworkElement const& element) {

        SetProperty(
            element,
            X::FrameworkElement::WidthProperty(),
            kAuto);

        SetProperty(
            element,
            X::FrameworkElement::MinWidthProperty(),
            0.0);

        SetProperty(
            element,
            X::FrameworkElement::MaxWidthProperty(),
            kInfinity);
    }

    void PruneEntries() {
        savedProperties.erase(
            std::remove_if(
                savedProperties.begin(),
                savedProperties.end(),
                [](SavedProperty const& value) {
                    return !value.element.get();
                }),
            savedProperties.end());

        labels.erase(
            std::remove_if(
                labels.begin(),
                labels.end(),
                [](LabelEntry const& value) {
                    return !value.icon.get() ||
                           !value.container.get() ||
                           !value.label.get();
                }),
            labels.end());
    }

    bool EnsureScrollViewer(
        C::Grid const& rootGrid,
        C::ItemsControl const& control) {

        if (scroll) {
            return items &&
                   SameObject(items, control) &&
                   SameObject(scroll.Content(), control);
        }

        auto parent =
            M::VisualTreeHelper::GetParent(control)
                .try_as<C::Grid>();

        if (!parent ||
            !SameObject(parent, rootGrid)) {
            return false;
        }

        uint32_t index = 0;

        if (!parent.Children().IndexOf(control, index)) {
            return false;
        }

        C::ScrollViewer newScroll;

        newScroll.HorizontalScrollMode(
            C::ScrollMode::Disabled);

        newScroll.HorizontalScrollBarVisibility(
            C::ScrollBarVisibility::Disabled);

        newScroll.VerticalScrollMode(
            C::ScrollMode::Enabled);

        newScroll.VerticalScrollBarVisibility(
            C::ScrollBarVisibility::Auto);

        newScroll.HorizontalContentAlignment(
            X::HorizontalAlignment::Left);

        newScroll.VerticalContentAlignment(
            X::VerticalAlignment::Top);

        /*
         * Do not modify padding, margin or border here.
         * These values belong to the native popup layout.
         */
        C::Grid::SetRow(
            newScroll,
            C::Grid::GetRow(control));

        C::Grid::SetColumn(
            newScroll,
            C::Grid::GetColumn(control));

        C::Grid::SetRowSpan(
            newScroll,
            C::Grid::GetRowSpan(control));

        C::Grid::SetColumnSpan(
            newScroll,
            C::Grid::GetColumnSpan(control));

        scroll = newScroll;
        items = control;
        scrollParent = parent;
        originalChildIndex = index;

        parent.Children().RemoveAt(index);
        newScroll.Content(control);
        parent.Children().InsertAt(index, newScroll);

        return true;
    }

    double HeightLimit(
        C::Grid const& rootGrid) {

        POINT cursor{};
        GetCursorPos(&cursor);

        HMONITOR monitor =
            MonitorFromPoint(
                cursor,
                MONITOR_DEFAULTTONEAREST);

        MONITORINFO info{
            sizeof(MONITORINFO)};

        double scale = 1.0;

        if (auto xamlRoot = rootGrid.XamlRoot()) {
            scale =
                xamlRoot.RasterizationScale();
        }

        if (!(scale > 0.0)) {
            scale = 1.0;
        }

        if (!GetMonitorInfoW(monitor, &info)) {
            return g_settings.maxHeight;
        }

        double workHeight =
            static_cast<double>(
                info.rcWork.bottom -
                info.rcWork.top) /
            scale;

        return std::max(
            g_settings.rowHeight,
            std::min(
                g_settings.maxHeight,
                workHeight -
                    kScreenVerticalReserve));
    }

    C::TextBlock GetOrCreateLabel(
        X::FrameworkElement const& icon,
        C::Grid const& container) {

        for (auto const& entry : labels) {
            auto existingIcon = entry.icon.get();
            auto existingContainer = entry.container.get();
            auto existingLabel = entry.label.get();

            if (existingIcon &&
                existingContainer &&
                existingLabel &&
                SameObject(existingIcon, icon) &&
                SameObject(existingContainer, container)) {
                return existingLabel;
            }
        }

        C::TextBlock label;

        label.Name(kLabelName);
        label.FontSize(g_settings.fontSize);

        label.TextWrapping(
            X::TextWrapping::NoWrap);

        label.TextTrimming(
            X::TextTrimming::CharacterEllipsis);

        label.VerticalAlignment(
            X::VerticalAlignment::Center);

        label.HorizontalAlignment(
            X::HorizontalAlignment::Stretch);

        label.Margin(
            X::Thickness{
                g_settings.iconAreaWidth +
                    kLabelGap,
                0.0,
                kLabelRightInset,
                0.0});

        label.IsHitTestVisible(false);

        A::AutomationProperties::SetAccessibilityView(
            label,
            AP::AccessibilityView::Raw);

        int columns = std::max(
            1,
            static_cast<int>(
                container.ColumnDefinitions().Size()));

        int rows = std::max(
            1,
            static_cast<int>(
                container.RowDefinitions().Size()));

        C::Grid::SetColumn(label, 0);
        C::Grid::SetRow(label, 0);
        C::Grid::SetColumnSpan(label, columns);
        C::Grid::SetRowSpan(label, rows);

        if (auto control =
                icon.try_as<C::Control>()) {

            D::Binding foreground;

            foreground.Source(control);
            foreground.Path(
                X::PropertyPath(L"Foreground"));
            foreground.Mode(D::BindingMode::OneWay);

            label.SetBinding(
                C::TextBlock::ForegroundProperty(),
                foreground);
        }

        labels.push_back(
            LabelEntry{
                winrt::make_weak(icon),
                winrt::make_weak(container),
                winrt::make_weak(label)});

        container.Children().Append(label);

        return label;
    }

    void RegisterIconEvents(
        X::FrameworkElement const& icon) {

        if (!icon) {
            return;
        }

        bool loadedRegistered = false;

        for (const auto& item : loadedHandlers) {
            auto element = item.element.get();

            if (element &&
                SameObject(element, icon)) {
                loadedRegistered = true;
                break;
            }
        }

        if (!loadedRegistered) {
            auto weakIcon =
                winrt::make_weak(icon);

            auto token = icon.Loaded(
                [this, weakIcon](
                    WF::IInspectable const&,
                    X::RoutedEventArgs const&) {

                    if (disabled ||
                        g_stopping.load()) {
                        return;
                    }

                    try {
                        if (auto loadedIcon = weakIcon.get()) {
                            StyleIcon(loadedIcon);
                            SizeViewport(root.get(), items);
                        }
                    } catch (...) {
                        LogException(
                            L"Icon Loaded handler");
                    }
                });

            loadedHandlers.push_back(
                LoadedHandler{
                    winrt::make_weak(icon),
                    token});
        }

        bool nameRegistered = false;

        for (const auto& item : nameHandlers) {
            auto element = item.element.get();

            if (element &&
                SameObject(element, icon)) {
                nameRegistered = true;
                break;
            }
        }

        if (!nameRegistered) {
            auto weakIcon =
                winrt::make_weak(icon);

            auto token =
                icon.RegisterPropertyChangedCallback(
                    A::AutomationProperties::NameProperty(),
                    [this, weakIcon](
                        X::DependencyObject const&,
                        X::DependencyProperty const&) {

                        if (disabled ||
                            g_stopping.load()) {
                            return;
                        }

                        try {
                            if (auto changedIcon =
                                    weakIcon.get()) {
                                StyleIcon(changedIcon);
                            }
                        } catch (...) {
                            LogException(
                                L"Icon name handler");
                        }
                    });

            nameHandlers.push_back(
                NameHandler{
                    winrt::make_weak(icon),
                    token});
        }
    }

    void StyleIcon(
        X::FrameworkElement const& icon) {

        if (!icon) {
            return;
        }

        auto container =
            FindDirectChild(
                icon,
                L"ContainerGrid")
                .try_as<C::Grid>();

        if (!container) {
            return;
        }

        auto presenter =
            FindDirectChild(
                container,
                L"ContentPresenter");

        if (!presenter) {
            return;
        }

        auto name =
            ReadIconName(icon);

        if (name.empty()) {
            name = L"Notification icon";
        }

        SetAutoWidth(icon);

        SetProperty(
            icon,
            X::FrameworkElement::
                HorizontalAlignmentProperty(),
            X::HorizontalAlignment::Stretch);

        SetProperty(
            icon,
            X::FrameworkElement::
                HeightProperty(),
            g_settings.rowHeight);

        SetAutoWidth(container);

        SetProperty(
            container,
            X::FrameworkElement::
                HorizontalAlignmentProperty(),
            X::HorizontalAlignment::Stretch);

        SetProperty(
            container,
            X::FrameworkElement::
                HeightProperty(),
            g_settings.rowHeight);

        SetProperty(
            presenter,
            X::FrameworkElement::
                MinWidthProperty(),
            0.0);

        SetProperty(
            presenter,
            X::FrameworkElement::
                WidthProperty(),
            g_settings.iconAreaWidth);

        SetProperty(
            presenter,
            X::FrameworkElement::
                HorizontalAlignmentProperty(),
            X::HorizontalAlignment::Left);

        SetProperty(
            presenter,
            X::FrameworkElement::
                VerticalAlignmentProperty(),
            X::VerticalAlignment::Center);

        if (auto control =
                icon.try_as<C::Control>()) {

            SetProperty(
                control,
                C::Control::
                    HorizontalContentAlignmentProperty(),
                X::HorizontalAlignment::Stretch);
        }

        auto label =
            GetOrCreateLabel(icon, container);

        label.FontSize(g_settings.fontSize);

        X::Thickness desiredMargin{
            g_settings.iconAreaWidth +
                kLabelGap,
            0.0,
            kLabelRightInset,
            0.0};

        label.Margin(desiredMargin);

        if (label.Text() != name) {
            label.Text(name);
        }

        RegisterIconEvents(icon);
    }

    void ConfigureLayout(
        C::Grid const& rootGrid,
        C::ItemsControl const& control,
        C::WrapGrid const& wrapGrid) {

        /*
         * RootGrid and all ancestors keep their original padding.
         * Only the list content receives a fixed row width.
         */
        SetAutoWidth(rootGrid);
        SetAutoWidth(control);
        SetAutoWidth(wrapGrid);

        SetProperty(
            control,
            X::FrameworkElement::
                HorizontalAlignmentProperty(),
            X::HorizontalAlignment::Left);

        SetProperty(
            wrapGrid,
            X::FrameworkElement::
                HorizontalAlignmentProperty(),
            X::HorizontalAlignment::Left);

        SetProperty(
            wrapGrid,
            C::WrapGrid::
                OrientationProperty(),
            C::Orientation::Horizontal);

        SetProperty(
            wrapGrid,
            C::WrapGrid::
                MaximumRowsOrColumnsProperty(),
            1);

        SetProperty(
            wrapGrid,
            C::WrapGrid::
                ItemWidthProperty(),
            g_settings.listWidth);

        SetProperty(
            wrapGrid,
            C::WrapGrid::
                ItemHeightProperty(),
            g_settings.rowHeight);
    }

    void SizeViewport(
        C::Grid const& rootGrid,
        C::ItemsControl const& control) {

        if (!scroll || !control) {
            return;
        }

        double maxHeight =
            HeightLimit(rootGrid);

        const float infinity =
            std::numeric_limits<float>::infinity();

        control.Measure(
            WF::Size{
                infinity,
                infinity});

        auto desired =
            control.DesiredSize();

        double desiredWidth =
            std::max(
                g_settings.listWidth,
                static_cast<double>(
                    desired.Width));

        double desiredHeight =
            static_cast<double>(
                desired.Height);

        double scrollChromeWidth =
            HorizontalExtent(scroll.Padding()) +
            HorizontalExtent(scroll.BorderThickness());

        double scrollChromeHeight =
            VerticalExtent(scroll.Padding()) +
            VerticalExtent(scroll.BorderThickness());

        double availableHeight =
            std::max(
                0.0,
                maxHeight -
                    scrollChromeHeight);

        bool needsScrollBar =
            desiredHeight >
            availableHeight + 0.5;

        /*
         * Do not add ancestor padding here. The popup presenter already
         * owns that padding. Adding it again causes the right side to
         * overflow while the left side appears too wide.
         */
        double viewportWidth =
            desiredWidth +
            scrollChromeWidth +
            (needsScrollBar
                 ? g_settings.scrollbarReserve
                 : 0.0);

        scroll.Width(viewportWidth);
        scroll.MaxHeight(maxHeight);
    }

    void ApplyLayout() {
        if (updating ||
            disabled ||
            g_stopping.load()) {
            return;
        }

        auto rootGrid = root.get();

        if (!rootGrid ||
            !rootGrid.IsLoaded()) {
            return;
        }

        updating = true;

        struct ResetUpdating {
            bool& value;

            ~ResetUpdating() {
                value = false;
            }
        } reset{updating};

        auto control = items;

        if (!control) {
            control =
                FindDescendant(
                    rootGrid,
                    L"Windows.UI.Xaml.Controls.ItemsControl")
                    .try_as<C::ItemsControl>();
        }

        if (!control) {
            return;
        }

        auto wrapGrid =
            control.ItemsPanelRoot()
                .try_as<C::WrapGrid>();

        if (!wrapGrid) {
            return;
        }

        if (!EnsureScrollViewer(
                rootGrid,
                control)) {
            return;
        }

        PruneEntries();
        ConfigureLayout(
            rootGrid,
            control,
            wrapGrid);

        int count =
            M::VisualTreeHelper::
                GetChildrenCount(wrapGrid);

        for (int i = 0; i < count; ++i) {
            auto child =
                M::VisualTreeHelper::
                    GetChild(wrapGrid, i);

            auto icon =
                FindDescendant(
                    child,
                    kIconViewClass);

            if (!icon) {
                continue;
            }

            if (auto presenter =
                    child.try_as<C::ContentPresenter>()) {
                SetProperty(
                    presenter,
                    C::ContentPresenter::
                        HorizontalContentAlignmentProperty(),
                    X::HorizontalAlignment::Stretch);
            }

            StyleIcon(icon);
        }

        SizeViewport(rootGrid, control);
    }

    void Restore() noexcept {
        disabled = true;

        for (const auto& item : loadedHandlers) {
            try {
                auto element = item.element.get();

                if (element) {
                    element.Loaded(item.token);
                }
            } catch (...) {
                LogException(L"Remove Loaded handler");
            }
        }

        loadedHandlers.clear();

        for (const auto& item : nameHandlers) {
            try {
                auto element = item.element.get();

                if (element) {
                    element.UnregisterPropertyChangedCallback(
                        A::AutomationProperties::NameProperty(),
                        item.token.value);
                }
            } catch (...) {
                LogException(L"Remove name handler");
            }
        }

        nameHandlers.clear();

        for (const auto& item : labels) {
            try {
                auto container = item.container.get();
                auto label = item.label.get();

                if (container && label) {
                    uint32_t index = 0;

                    if (container.Children().IndexOf(
                            label,
                            index)) {
                        container.Children().RemoveAt(index);
                    }
                }
            } catch (...) {
                LogException(L"Remove label");
            }
        }

        labels.clear();

        for (auto it = savedProperties.rbegin();
             it != savedProperties.rend();
             ++it) {

            try {
                auto element = it->element.get();

                if (!element) {
                    continue;
                }

                if (it->binding) {
                    element.SetBinding(
                        it->property,
                        it->binding);
                } else if (SameObject(
                               it->localValue,
                               X::DependencyProperty::
                                   UnsetValue())) {
                    element.ClearValue(it->property);
                } else {
                    element.SetValue(
                        it->property,
                        it->localValue);
                }
            } catch (...) {
                LogException(L"Restore property");
            }
        }

        savedProperties.clear();

        try {
            auto parent = scrollParent.get();

            if (parent && scroll && items) {
                uint32_t currentIndex = 0;
                uint32_t targetIndex = originalChildIndex;

                if (parent.Children().IndexOf(
                        scroll,
                        currentIndex)) {
                    targetIndex = currentIndex;
                    parent.Children().RemoveAt(currentIndex);
                }

                if (SameObject(
                        scroll.Content(),
                        items)) {
                    scroll.Content(nullptr);
                }

                if (!M::VisualTreeHelper::
                        GetParent(items)) {
                    targetIndex = std::min(
                        targetIndex,
                        parent.Children().Size());

                    parent.Children().InsertAt(
                        targetIndex,
                        items);
                }
            }
        } catch (...) {
            LogException(L"Restore visual tree");
        }

        items = nullptr;
        scroll = nullptr;
        scrollParent = {};
    }
};

struct OwnedWindow {
    HWND window = nullptr;
    std::unique_ptr<State> state;
};

[[clang::no_destroy]]
std::optional<std::vector<OwnedWindow>> g_ownedWindows{
    std::in_place
};

bool IsRootAlive(State const& state) {
    auto root = state.root.get();

    return root && root.IsLoaded();
}

State* FindStateForIcon(
    X::FrameworkElement const& icon) {

    if (!g_ownedWindows) {
        return nullptr;
    }

    DWORD threadId =
        GetCurrentThreadId();

    for (auto& owned : *g_ownedWindows) {
        if (!owned.state ||
            owned.state->threadId != threadId) {
            continue;
        }

        auto root =
            owned.state->root.get();

        if (root &&
            IsDescendantOf(icon, root)) {
            return owned.state.get();
        }
    }

    return nullptr;
}

void RegisterDynamicIcon(
    X::FrameworkElement const& icon) {

    if (!icon || !g_ownedWindows) {
        return;
    }

    auto weakIcon =
        winrt::make_weak(icon);

    icon.Loaded(
        [weakIcon](
            WF::IInspectable const&,
            X::RoutedEventArgs const&) {

            try {
                auto loadedIcon = weakIcon.get();

                if (!loadedIcon) {
                    return;
                }

                auto state =
                    FindStateForIcon(loadedIcon);

                if (!state || state->disabled) {
                    return;
                }

                state->ApplyLayout();
            } catch (...) {
                LogException(
                    L"Dynamic icon Loaded handler");
            }
        });
}

LRESULT CALLBACK HelperWindowProc(
    HWND window,
    UINT message,
    WPARAM wParam,
    LPARAM lParam) {

    auto state =
        reinterpret_cast<State*>(
            GetWindowLongPtrW(
                window,
                GWLP_USERDATA));

    if (message == WM_NCCREATE) {
        auto create =
            reinterpret_cast<CREATESTRUCTW*>(lParam);

        state =
            static_cast<State*>(
                create->lpCreateParams);

        SetWindowLongPtrW(
            window,
            GWLP_USERDATA,
            reinterpret_cast<LONG_PTR>(state));

        return TRUE;
    }

    if (message == kCleanupMessage) {
        if (state) {
            state->Restore();
        }

        DestroyWindow(window);
        return 0;
    }

    if (message == WM_NCDESTROY) {
        SetWindowLongPtrW(
            window,
            GWLP_USERDATA,
            0);
    }

    return DefWindowProcW(
        window,
        message,
        wParam,
        lParam);
}

void ObserveOverflowRoot(
    C::Grid const& root) {

    if (!root ||
        !g_ownedWindows ||
        g_stopping.load()) {
        return;
    }

    std::lock_guard lock(g_windowsMutex);

    DWORD threadId =
        GetCurrentThreadId();

    /*
     * Remove states whose overflow root has been destroyed.
     * Cleanup runs on this same taskbar UI thread.
     */
    for (auto it = g_ownedWindows->begin();
         it != g_ownedWindows->end();) {

        if (!it->state ||
            it->state->threadId != threadId) {
            ++it;
            continue;
        }

        auto oldRoot =
            it->state->root.get();

        if (!oldRoot ||
            !oldRoot.IsLoaded()) {

            it->state->Restore();

            if (IsWindow(it->window)) {
                DestroyWindow(it->window);
            }

            it =
                g_ownedWindows->erase(it);

            continue;
        }

        if (SameObject(oldRoot, root)) {
            try {
                it->state->ApplyLayout();
            } catch (...) {
                LogException(L"Refresh overflow layout");
            }

            return;
        }

        ++it;
    }

    auto state =
        std::make_unique<State>(root);

    HWND window =
        CreateWindowExW(
            0,
            kWindowClass,
            L"",
            0,
            0,
            0,
            0,
            0,
            HWND_MESSAGE,
            nullptr,
            g_self,
            state.get());

    if (!window) {
        Wh_Log(
            L"CreateWindowExW failed: %u",
            GetLastError());
        return;
    }

    State* statePointer = state.get();

    g_ownedWindows->push_back(
        OwnedWindow{
            window,
            std::move(state)});

    try {
        statePointer->ApplyLayout();
    } catch (...) {
        LogException(L"Initial overflow layout");
        statePointer->Restore();
    }
}

C::Grid ReadOverflowRoot(
    void* instance) {

    if (!instance) {
        return nullptr;
    }

    IUnknown* candidate = nullptr;
    SIZE_T bytesRead = 0;

    const BYTE* address =
        reinterpret_cast<const BYTE*>(instance) +
        5 * sizeof(void*);

    if (!ReadProcessMemory(
            GetCurrentProcess(),
            address,
            &candidate,
            sizeof(candidate),
            &bytesRead) ||
        bytesRead != sizeof(candidate) ||
        !candidate) {
        return nullptr;
    }

    C::Grid root{nullptr};

    HRESULT result =
        candidate->QueryInterface(
            winrt::guid_of<C::Grid>(),
            winrt::put_abi(root));

    if (FAILED(result)) {
        return nullptr;
    }

    return root;
}

void WINAPI InitializeHook(
    void* instance) {

    g_initializeOriginal(instance);

    if (g_stopping.load()) {
        return;
    }

    try {
        if (auto root =
                ReadOverflowRoot(instance)) {
            ObserveOverflowRoot(root);
        }
    } catch (...) {
        LogException(L"Observe overflow");
    }
}

void* WINAPI IconViewCtorHook(
    void* instance) {

    void* result =
        g_iconViewCtorOriginal(instance);

    if (!result ||
        g_stopping.load()) {
        return result;
    }

    try {
        auto unknown =
            *reinterpret_cast<IUnknown**>(
                reinterpret_cast<BYTE*>(instance) +
                sizeof(void*));

        if (!unknown) {
            return result;
        }

        X::FrameworkElement icon{nullptr};

        HRESULT queryResult =
            unknown->QueryInterface(
                winrt::guid_of<X::FrameworkElement>(),
                winrt::put_abi(icon));

        if (SUCCEEDED(queryResult) &&
            icon) {
            RegisterDynamicIcon(icon);
        }
    } catch (...) {
        LogException(L"IconView constructor");
    }

    return result;
}

unsigned GetModuleMajorVersion(
    HMODULE module) {

    HRSRC resource =
        FindResourceW(
            module,
            MAKEINTRESOURCEW(VS_VERSION_INFO),
            RT_VERSION);

    if (!resource) {
        return 0;
    }

    HGLOBAL loaded =
        LoadResource(module, resource);

    if (!loaded) {
        return 0;
    }

    void* data =
        LockResource(loaded);

    if (!data) {
        return 0;
    }

    VS_FIXEDFILEINFO* info = nullptr;
    UINT size = 0;

    if (!VerQueryValueW(
            data,
            L"\\",
            reinterpret_cast<void**>(&info),
            &size) ||
        !info ||
        size < sizeof(VS_FIXEDFILEINFO)) {
        return 0;
    }

    return HIWORD(info->dwFileVersionMS);
}

HMODULE FindSystemTrayModule() {
    if (auto module =
            GetModuleHandleW(L"SystemTray.dll")) {
        return module;
    }

    if (auto module =
            GetModuleHandleW(L"Taskbar.View.dll")) {

        unsigned major =
            GetModuleMajorVersion(module);

        if (major == 0 || major < 2604) {
            return module;
        }
    }

    return GetModuleHandleW(
        L"ExplorerExtensions.dll");
}

bool InstallSystemTraySymbols(
    HMODULE module) {

    // SystemTray.dll, Taskbar.View.dll, ExplorerExtensions.dll
    WindhawkUtils::SYMBOL_HOOK hooks[] = {
        {
            {
                LR"(private: void __cdecl winrt::SystemTray::OverflowXamlIslandManager::InitializeIfNeeded(void))"
            },
            &g_initializeOriginal,
            InitializeHook,
        },
        {
            {
                LR"(public: __cdecl winrt::SystemTray::implementation::IconView::IconView(void))"
            },
            &g_iconViewCtorOriginal,
            IconViewCtorHook,
        },
    };

    return WindhawkUtils::HookSymbols(
        module,
        hooks,
        ARRAYSIZE(hooks));
}

void TryInstallHooks(
    bool applyOperations) {

    if (g_stopping.load() ||
        g_hookState.load() != 0) {
        return;
    }

    HMODULE module =
        FindSystemTrayModule();

    if (!module) {
        return;
    }

    int expected = 0;

    if (!g_hookState.compare_exchange_strong(
            expected,
            1)) {
        return;
    }

    try {
        if (!InstallSystemTraySymbols(module)) {
            g_hookState.store(-1);

            Wh_Log(
                L"SystemTray symbols not found");

            return;
        }

        if (applyOperations) {
            Wh_ApplyHookOperations();
        }

        g_hookState.store(2);

        Wh_Log(
            L"Tray overflow hooks installed");
    } catch (...) {
        g_hookState.store(-1);
        LogException(L"Install hooks");
    }
}

HMODULE WINAPI LoadLibraryExWHook(
    LPCWSTR fileName,
    HANDLE file,
    DWORD flags) {

    HMODULE module =
        g_loadLibraryExWOriginal(
            fileName,
            file,
            flags);

    if (module &&
        !(flags &
          LOAD_LIBRARY_AS_DATAFILE) &&
        !(flags &
          LOAD_LIBRARY_AS_DATAFILE_EXCLUSIVE) &&
        !(flags &
          LOAD_LIBRARY_AS_IMAGE_RESOURCE)) {
        TryInstallHooks(true);
    }

    return module;
}

void StopAndRestore() {
    g_stopping.store(true);

    if (!g_ownedWindows) {
        return;
    }

    std::vector<HWND> windows;

    {
        std::lock_guard lock(g_windowsMutex);

        for (const auto& owned : *g_ownedWindows) {
            windows.push_back(owned.window);
        }
    }

    /*
     * Restore and destroy every helper window synchronously on the
     * taskbar UI thread before releasing the owning vector.
     */
    for (HWND window : windows) {
        if (IsWindow(window)) {
            SendMessageW(
                window,
                kCleanupMessage,
                0,
                0);
        }
    }

    {
        std::lock_guard lock(g_windowsMutex);

        /*
         * g_ownedWindows contains strong XAML references. Its storage must
         * be released only after Restore() ran on the UI thread.
         */
        g_ownedWindows.reset();
    }
}

}  // namespace

BOOL Wh_ModInit() {
    LoadSettings();

    if (!GetModuleHandleExW(
            GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
            reinterpret_cast<LPCWSTR>(
                &HelperWindowProc),
            &g_self)) {
        return FALSE;
    }

    WNDCLASSW windowClass{};

    windowClass.lpfnWndProc =
        HelperWindowProc;

    windowClass.hInstance =
        g_self;

    windowClass.lpszClassName =
        kWindowClass;

    g_windowClass =
        RegisterClassW(&windowClass);

    if (!g_windowClass) {
        Wh_Log(
            L"RegisterClassW failed: %u",
            GetLastError());

        return FALSE;
    }

    HMODULE kernelBase =
        GetModuleHandleW(
            L"kernelbase.dll");

    auto loadLibrary =
        kernelBase
            ? reinterpret_cast<
                  LoadLibraryExWFn>(
                  GetProcAddress(
                      kernelBase,
                      "LoadLibraryExW"))
            : nullptr;

    if (!loadLibrary ||
        !WindhawkUtils::SetFunctionHook(
            loadLibrary,
            LoadLibraryExWHook,
            &g_loadLibraryExWOriginal)) {

        UnregisterClassW(
            kWindowClass,
            g_self);

        g_windowClass = 0;

        return FALSE;
    }

    TryInstallHooks(false);

    if (g_hookState.load() == -1) {
        UnregisterClassW(
            kWindowClass,
            g_self);

        g_windowClass = 0;

        return FALSE;
    }

    return TRUE;
}

void Wh_ModAfterInit() {
    TryInstallHooks(true);
}

void Wh_ModBeforeUninit() {
    StopAndRestore();
}

void Wh_ModUninit() {
    if (g_windowClass) {
        UnregisterClassW(
            kWindowClass,
            g_self);

        g_windowClass = 0;
    }
}

void Wh_ModSettingsChanged() {
    LoadSettings();

    if (!g_ownedWindows) {
        return;
    }

    std::lock_guard lock(g_windowsMutex);

    for (auto& owned : *g_ownedWindows) {
        if (!owned.state ||
            owned.state->disabled) {
            continue;
        }

        try {
            owned.state->ApplyLayout();
        } catch (...) {
            LogException(
                L"Apply settings");
        }
    }
}
