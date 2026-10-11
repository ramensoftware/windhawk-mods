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

Changes the Windows 11 notification-area overflow menu into a vertical list.

Features:

- Shows each tray icon in a separate row.
- Displays the icon name beside the icon.
- Preserves the original tray-icon click and context-menu behavior.
- Preserves the native popup padding and theme.
- Supports vertical scrolling when the list becomes too tall.

The mod only changes the layout of the notification-area overflow popup.
It does not modify tray-icon registration or notification behavior.

Changing settings may require reopening the overflow popup.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- listWidth: 300
  $name: List width
  $description: Width of the tray overflow list in pixels.

- rowHeight: 40
  $name: Row height
  $description: Height of each tray icon row in pixels.

- iconAreaWidth: 40
  $name: Icon area width
  $description: Width reserved for the tray icon in each row.

- maxHeight: 840
  $name: Maximum popup height
  $description: Maximum height of the popup in pixels.

- scrollbarReserve: 9
  $name: Scrollbar width reserve
  $description: Width reserved for the vertical scrollbar in pixels.

- fontSize: 13
  $name: Label font size
  $description: Font size of tray icon names.
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

constexpr double kLabelGap = 4.0;
constexpr double kLabelRightInset = 12.0;

constexpr double kScreenVerticalReserve = 96.0;

struct Settings {
    double listWidth = 300.0;
    double rowHeight = 40.0;
    double iconAreaWidth = 40.0;
    double fontSize = 13.0;
    double maxHeight = 840.0;
    double scrollbarReserve = 9.0;
};

Settings g_settings;

constexpr UINT_PTR kTimerId = 1;
constexpr UINT kTimerPeriodMs = 250;
constexpr UINT kCleanupMessage = WM_APP + 0x517;

constexpr wchar_t kWindowClass[] =
    L"Windhawk.TrayOverflowList.PrivateWindow.0_4";

constexpr wchar_t kLabelName[] =
    L"WindhawkTrayOverflowListLabel";

const double kAuto =
    std::numeric_limits<double>::quiet_NaN();

const double kInfinity =
    std::numeric_limits<double>::infinity();

std::atomic<bool> g_stopping{false};
std::atomic<int> g_hookState{0};

HMODULE g_self = nullptr;
ATOM g_windowClass = 0;

std::mutex g_windowsMutex;

using InitializeFn = void(WINAPI*)(void*);
InitializeFn g_initializeOriginal = nullptr;

using LoadLibraryExWFn = decltype(&LoadLibraryExW);
LoadLibraryExWFn g_loadLibraryExWOriginal = nullptr;

void LogException(PCWSTR operation) noexcept {
    HRESULT hr = winrt::to_hresult();

    Wh_Log(
        L"%s failed: 0x%08X",
        operation,
        static_cast<unsigned int>(hr));
}

void LoadSettings() {
    g_settings.listWidth =
        std::clamp(
            static_cast<double>(
                Wh_GetIntSetting(L"listWidth")),
            160.0,
            800.0);

    g_settings.rowHeight =
        std::clamp(
            static_cast<double>(
                Wh_GetIntSetting(L"rowHeight")),
            24.0,
            96.0);

    g_settings.iconAreaWidth =
        std::clamp(
            static_cast<double>(
                Wh_GetIntSetting(L"iconAreaWidth")),
            16.0,
            96.0);

    g_settings.fontSize =
        std::clamp(
            static_cast<double>(
                Wh_GetIntSetting(L"fontSize")),
            8.0,
            32.0);

    g_settings.maxHeight =
        std::clamp(
            static_cast<double>(
                Wh_GetIntSetting(L"maxHeight")),
            200.0,
            2000.0);

    g_settings.scrollbarReserve =
        std::clamp(
            static_cast<double>(
                Wh_GetIntSetting(L"scrollbarReserve")),
            0.0,
            32.0);
}

bool SameObject(
    WF::IInspectable const& first,
    WF::IInspectable const& second) {

    if (!first || !second) {
        return !first && !second;
    }

    auto firstIdentity = first.as<IUnknown>();
    auto secondIdentity = second.as<IUnknown>();

    return firstIdentity.get() == secondIdentity.get();
}

bool EqualDouble(double first, double second) {
    return first == second ||
           (std::isnan(first) && std::isnan(second));
}

double HorizontalExtent(X::Thickness const& value) {
    return value.Left + value.Right;
}

double VerticalExtent(X::Thickness const& value) {
    return value.Top + value.Bottom;
}

X::FrameworkElement FindDirectChild(
    X::DependencyObject const& parent,
    PCWSTR name = nullptr,
    PCWSTR className = nullptr) {

    if (!parent) {
        return nullptr;
    }

    int count = M::VisualTreeHelper::GetChildrenCount(parent);

    for (int i = 0; i < count; ++i) {
        auto element =
            M::VisualTreeHelper::GetChild(parent, i)
                .try_as<X::FrameworkElement>();

        if (!element) {
            continue;
        }

        if (name && element.Name() != name) {
            continue;
        }

        if (className &&
            winrt::get_class_name(element) != className) {
            continue;
        }

        return element;
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

    int count = M::VisualTreeHelper::GetChildrenCount(parent);

    for (int i = 0; i < count; ++i) {
        auto child = M::VisualTreeHelper::GetChild(parent, i);
        auto element = child.try_as<X::FrameworkElement>();

        if (element && element.Name() == kLabelName) {
            continue;
        }

        if (element &&
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

std::wstring NormalizeText(winrt::hstring const& source) {
    std::wstring text(source.c_str(), source.size());

    auto newline = text.find_first_of(L"\r\n");
    if (newline != std::wstring::npos) {
        text.resize(newline);
    }

    for (auto& character : text) {
        if (character == L'\t') {
            character = L' ';
        }
    }

    auto first = text.find_first_not_of(L' ');
    if (first == std::wstring::npos) {
        return {};
    }

    auto last = text.find_last_not_of(L' ');
    text = text.substr(first, last - first + 1);

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

    if (auto value = object.try_as<WF::IPropertyValue>()) {
        if (value.Type() == WF::PropertyType::String) {
            return NormalizeText(value.GetString());
        }
    }

    if (auto block = object.try_as<C::TextBlock>()) {
        return NormalizeText(block.Text());
    }

    if (auto tooltip = object.try_as<C::ToolTip>()) {
        return ReadInspectableText(
            tooltip.Content(), depth + 1);
    }

    if (auto content = object.try_as<C::ContentControl>()) {
        return ReadInspectableText(
            content.Content(), depth + 1);
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

    auto name = NormalizeText(
        A::AutomationProperties::GetName(element));

    if (!name.empty()) {
        return name;
    }

    name = ReadInspectableText(
        C::ToolTipService::GetToolTip(element));

    if (!name.empty()) {
        return name;
    }

    int count = M::VisualTreeHelper::GetChildrenCount(element);

    for (int i = 0; i < count; ++i) {
        auto child =
            M::VisualTreeHelper::GetChild(element, i)
                .try_as<X::FrameworkElement>();

        if (!child) {
            continue;
        }

        name = ReadIconName(child, depth + 1);

        if (!name.empty()) {
            return name;
        }
    }

    return {};
}

struct SavedProperty {
    winrt::weak_ref<X::FrameworkElement> element;
    X::DependencyProperty property{nullptr};
    WF::IInspectable localValue{nullptr};
    D::Binding binding{nullptr};
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
    std::vector<LabelEntry> labels;

    uint32_t originalChildIndex = 0;

    bool updating = false;
    bool disabled = false;

    explicit State(C::Grid const& value) : root(value) {}

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
            saved.binding = expression.ParentBinding();
        } else {
            saved.localValue = element.ReadLocalValue(property);
        }

        savedProperties.push_back(std::move(saved));
    }

    template<typename T>
    void SetProperty(
        X::FrameworkElement const& element,
        X::DependencyProperty const& property,
        T const& value) {

        SaveProperty(element, property);
        element.SetValue(property, winrt::box_value(value));
    }

    void SetDouble(
        X::FrameworkElement const& element,
        X::DependencyProperty const& property,
        double value) {

        double current =
            winrt::unbox_value<double>(
                element.GetValue(property));

        if (!EqualDouble(current, value)) {
            SetProperty(element, property, value);
        }
    }

    void AutoWidth(X::FrameworkElement const& element) {
        SetDouble(
            element,
            X::FrameworkElement::MinWidthProperty(),
            0.0);

        SetDouble(
            element,
            X::FrameworkElement::MaxWidthProperty(),
            kInfinity);

        SetDouble(
            element,
            X::FrameworkElement::WidthProperty(),
            kAuto);
    }

    void PruneExpiredEntries() {
        savedProperties.erase(
            std::remove_if(
                savedProperties.begin(),
                savedProperties.end(),
                [](SavedProperty const& entry) {
                    return !entry.element.get();
                }),
            savedProperties.end());

        labels.erase(
            std::remove_if(
                labels.begin(),
                labels.end(),
                [](LabelEntry const& entry) {
                    return !entry.icon.get() ||
                           !entry.container.get() ||
                           !entry.label.get();
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

        if (!parent || !SameObject(parent, rootGrid)) {
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

        // Existing padding, margins and borders are left untouched.
        C::Grid::SetRow(
            newScroll, C::Grid::GetRow(control));

        C::Grid::SetColumn(
            newScroll, C::Grid::GetColumn(control));

        C::Grid::SetRowSpan(
            newScroll, C::Grid::GetRowSpan(control));

        C::Grid::SetColumnSpan(
            newScroll, C::Grid::GetColumnSpan(control));

        // Record the transaction before modifying the visual tree.
        scroll = newScroll;
        items = control;
        scrollParent = parent;
        originalChildIndex = index;

        parent.Children().RemoveAt(index);
        newScroll.Content(control);
        parent.Children().InsertAt(index, newScroll);

        return true;
    }

    double HeightLimit(C::Grid const& rootGrid) {
        POINT cursor{};
        GetCursorPos(&cursor);

        HMONITOR monitor =
            MonitorFromPoint(cursor, MONITOR_DEFAULTTONEAREST);

        MONITORINFO info{sizeof(info)};

        double scale = 1.0;
        if (auto xamlRoot = rootGrid.XamlRoot()) {
            scale = xamlRoot.RasterizationScale();
        }

        if (!(scale > 0.0)) {
            scale = 1.0;
        }

        if (!GetMonitorInfoW(monitor, &info)) {
            return g_settings.maxHeight;
        }

        double workHeight =
            static_cast<double>(
                info.rcWork.bottom - info.rcWork.top) /
            scale;

        return std::max(
            g_settings.rowHeight,
            std::min(
                g_settings.maxHeight,
                workHeight - kScreenVerticalReserve));
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

        label.TextWrapping(X::TextWrapping::NoWrap);
        label.TextTrimming(X::TextTrimming::CharacterEllipsis);

        label.VerticalAlignment(X::VerticalAlignment::Center);
        label.HorizontalAlignment(X::HorizontalAlignment::Stretch);

        label.Margin(X::Thickness{
            g_settings.iconAreaWidth + kLabelGap,
            0.0,
            kLabelRightInset,
            0.0
        });

        label.IsHitTestVisible(false);

        A::AutomationProperties::SetAccessibilityView(
            label, AP::AccessibilityView::Raw);

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

        if (auto control = icon.try_as<C::Control>()) {
            D::Binding foreground;

            foreground.Source(control);
            foreground.Path(X::PropertyPath(L"Foreground"));
            foreground.Mode(D::BindingMode::OneWay);

            label.SetBinding(
                C::TextBlock::ForegroundProperty(),
                foreground);
        }

        labels.push_back(LabelEntry{
            winrt::make_weak(icon),
            winrt::make_weak(container),
            winrt::make_weak(label)
        });

        container.Children().Append(label);
        return label;
    }

    void StyleIcon(X::FrameworkElement const& icon) {
        auto container =
            FindDirectChild(icon, L"ContainerGrid")
                .try_as<C::Grid>();

        if (!container) {
            return;
        }

        auto presenter =
            FindDirectChild(container, L"ContentPresenter");

        if (!presenter) {
            return;
        }

        auto name = ReadIconName(icon);
        if (name.empty()) {
            name = L"Notification icon";
        }

        // Let the icon fit the row slot, including its original margins.
        AutoWidth(icon);

        SetProperty(
            icon,
            X::FrameworkElement::HorizontalAlignmentProperty(),
            X::HorizontalAlignment::Stretch);

        SetDouble(
            icon,
            X::FrameworkElement::HeightProperty(),
            g_settings.rowHeight);

        // A fixed-width inner Grid would overflow a padded outer control.
        // Stretch it inside the original control's content area instead.
        AutoWidth(container);

        SetProperty(
            container,
            X::FrameworkElement::HorizontalAlignmentProperty(),
            X::HorizontalAlignment::Stretch);

        SetDouble(
            container,
            X::FrameworkElement::HeightProperty(),
            kAuto);

        if (auto control = icon.try_as<C::Control>()) {
            SetProperty(
                control,
                C::Control::HorizontalContentAlignmentProperty(),
                X::HorizontalAlignment::Stretch);
        }

        SetDouble(
            presenter,
            X::FrameworkElement::MinWidthProperty(),
            0.0);

        SetDouble(
            presenter,
            X::FrameworkElement::MaxWidthProperty(),
            kInfinity);

        SetDouble(
            presenter,
            X::FrameworkElement::WidthProperty(),
            g_settings.iconAreaWidth);

        SetProperty(
            presenter,
            X::FrameworkElement::HorizontalAlignmentProperty(),
            X::HorizontalAlignment::Left);

        SetProperty(
            presenter,
            X::FrameworkElement::VerticalAlignmentProperty(),
            X::VerticalAlignment::Center);

        auto label = GetOrCreateLabel(icon, container);

        // Only our new label receives additional spacing.
        auto presenterMargin = presenter.Margin();

        X::Thickness desiredMargin{
            std::max(0.0, presenterMargin.Left) +
                g_settings.iconAreaWidth +
                std::max(0.0, presenterMargin.Right) +
                kLabelGap,
            0.0,
            kLabelRightInset,
            0.0
        };

        auto currentMargin = label.Margin();

        if (currentMargin.Left != desiredMargin.Left ||
            currentMargin.Right != desiredMargin.Right) {
            label.Margin(desiredMargin);
        }

        if (label.Text() != name) {
            label.Text(name);
        }
    }

    void ConfigureWidths(
        C::Grid const& rootGrid,
        C::ItemsControl const& control,
        C::WrapGrid const& wrapGrid) {

        // Fixed row slots, but auto-sized surrounding boxes.
        // XAML includes each box's own padding/border/margin in Measure.
        AutoWidth(rootGrid);
        AutoWidth(control);
        AutoWidth(wrapGrid);

        SetProperty(
            control,
            X::FrameworkElement::HorizontalAlignmentProperty(),
            X::HorizontalAlignment::Left);

        SetProperty(
            wrapGrid,
            X::FrameworkElement::HorizontalAlignmentProperty(),
            X::HorizontalAlignment::Left);

        auto current =
            M::VisualTreeHelper::GetParent(wrapGrid);

        // Release stale width constraints in the ItemsControl template.
        // Never walk above the ItemsControl or alter ancestor spacing.
        for (int depth = 0; current && depth < 16; ++depth) {
            if (SameObject(current, control)) {
                break;
            }

            if (auto element =
                    current.try_as<X::FrameworkElement>()) {
                AutoWidth(element);
            }

            current = M::VisualTreeHelper::GetParent(current);
        }
    }

    void SizeViewport(
        C::Grid const& rootGrid,
        C::ItemsControl const& control) {

        const float infinity =
            std::numeric_limits<float>::infinity();

        // DesiredSize includes the control's own layout spacing.
        // Do not add control.Padding(), control.Margin() or parent
        // padding to this width again.
        control.Measure(WF::Size{infinity, infinity});

        WF::Size desired = control.DesiredSize();

        double contentWidth = desired.Width;
        double contentHeight = desired.Height;

        if (!std::isfinite(contentWidth) ||
            !std::isfinite(contentHeight)) {
            Wh_Log(L"Invalid ItemsControl desired size");
            return;
        }

        contentWidth = std::max(contentWidth, g_settings.listWidth);

        double maximumHeight = HeightLimit(rootGrid);

        // Width/MaxHeight exclude the ScrollViewer's own Margin.
        // Only its own padding and border belong in these dimensions.
        double horizontalChrome =
            HorizontalExtent(scroll.Padding()) +
            HorizontalExtent(scroll.BorderThickness());

        double verticalChrome =
            VerticalExtent(scroll.Padding()) +
            VerticalExtent(scroll.BorderThickness());

        double availableContentHeight =
            std::max(0.0, maximumHeight - verticalChrome);

        bool needsScrollBar =
            contentHeight > availableContentHeight + 0.5;

        double viewportWidth = std::ceil(
            contentWidth +
            horizontalChrome +
            (needsScrollBar ? g_settings.scrollbarReserve : 0.0));

        if (!EqualDouble(scroll.Width(), viewportWidth)) {
            scroll.Width(viewportWidth);
        }

        if (!EqualDouble(scroll.MaxHeight(), maximumHeight)) {
            scroll.MaxHeight(maximumHeight);
        }

        // RootGrid remains Auto. Its original padding, plus all outer
        // borders/presenters, is accounted for by normal XAML layout.
    }

    void ApplyLayout() {
        if (updating || disabled || g_stopping.load()) {
            return;
        }

        auto rootGrid = root.get();

        if (!rootGrid || !rootGrid.IsLoaded()) {
            return;
        }

        updating = true;

        struct ResetFlag {
            bool& flag;
            ~ResetFlag() { flag = false; }
        } reset{updating};

        auto control = items;

        if (!control) {
            control =
                FindDirectChild(
                    rootGrid,
                    nullptr,
                    L"Windows.UI.Xaml.Controls.ItemsControl")
                    .try_as<C::ItemsControl>();
        }

        if (!control) {
            return;
        }

        auto wrapGrid =
            control.ItemsPanelRoot().try_as<C::WrapGrid>();

        if (!wrapGrid) {
            return;
        }

        if (!EnsureScrollViewer(rootGrid, control)) {
            return;
        }

        PruneExpiredEntries();

        if (wrapGrid.Orientation() != C::Orientation::Horizontal) {
            SetProperty(
                wrapGrid,
                C::WrapGrid::OrientationProperty(),
                C::Orientation::Horizontal);
        }

        if (wrapGrid.MaximumRowsOrColumns() != 1) {
            SetProperty(
                wrapGrid,
                C::WrapGrid::MaximumRowsOrColumnsProperty(),
                1);
        }

        SetDouble(
            wrapGrid,
            C::WrapGrid::ItemWidthProperty(),
            g_settings.listWidth);

        SetDouble(
            wrapGrid,
            C::WrapGrid::ItemHeightProperty(),
            g_settings.rowHeight);

        ConfigureWidths(rootGrid, control, wrapGrid);

        int count =
            M::VisualTreeHelper::GetChildrenCount(wrapGrid);

        for (int i = 0; i < count; ++i) {
            auto child =
                M::VisualTreeHelper::GetChild(wrapGrid, i);

            auto icon =
                FindDescendant(
                    child,
                    L"SystemTray.NotifyIconView");

            if (!icon) {
                continue;
            }

            if (auto element = child.try_as<X::FrameworkElement>()) {
                AutoWidth(element);

                SetProperty(
                    element,
                    X::FrameworkElement::HorizontalAlignmentProperty(),
                    X::HorizontalAlignment::Stretch);

                if (auto presenter =
                        element.try_as<C::ContentPresenter>()) {
                    SetProperty(
                        presenter,
                        C::ContentPresenter::
                            HorizontalContentAlignmentProperty(),
                        X::HorizontalAlignment::Stretch);
                }
            }

            StyleIcon(icon);
        }

        SizeViewport(rootGrid, control);
    }

    void Restore() noexcept {
        disabled = true;

        for (auto const& entry : labels) {
            try {
                auto container = entry.container.get();
                auto label = entry.label.get();

                if (container && label) {
                    uint32_t index = 0;

                    if (container.Children().IndexOf(label, index)) {
                        container.Children().RemoveAt(index);
                    }
                }
            } catch (...) {
                LogException(L"Remove label");
            }
        }

        labels.clear();

        for (auto it = savedProperties.rbegin();
             it != savedProperties.rend(); ++it) {

            try {
                auto element = it->element.get();
                if (!element) {
                    continue;
                }

                if (it->binding) {
                    element.SetBinding(it->property, it->binding);
                } else if (SameObject(
                               it->localValue,
                               X::DependencyProperty::UnsetValue())) {
                    element.ClearValue(it->property);
                } else {
                    element.SetValue(it->property, it->localValue);
                }
            } catch (...) {
                LogException(L"Restore property");
            }
        }

        savedProperties.clear();

        try {
            auto parent = scrollParent.get();

            if (parent && scroll && items) {
                uint32_t targetIndex = originalChildIndex;
                uint32_t currentIndex = 0;

                if (parent.Children().IndexOf(scroll, currentIndex)) {
                    targetIndex = currentIndex;
                    parent.Children().RemoveAt(currentIndex);
                }

                if (SameObject(scroll.Content(), items)) {
                    scroll.Content(nullptr);
                }

                if (!M::VisualTreeHelper::GetParent(items)) {
                    targetIndex = std::min(
                        targetIndex, parent.Children().Size());

                    parent.Children().InsertAt(targetIndex, items);
                }
            }
        } catch (...) {
            LogException(L"Restore items parent");
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

std::vector<OwnedWindow> g_ownedWindows;

LRESULT CALLBACK HelperWindowProc(
    HWND window,
    UINT message,
    WPARAM wParam,
    LPARAM lParam) {

    auto state = reinterpret_cast<State*>(
        GetWindowLongPtrW(window, GWLP_USERDATA));

    if (message == WM_NCCREATE) {
        auto create = reinterpret_cast<CREATESTRUCTW*>(lParam);
        state = static_cast<State*>(create->lpCreateParams);

        SetWindowLongPtrW(
            window,
            GWLP_USERDATA,
            reinterpret_cast<LONG_PTR>(state));

        return TRUE;
    }

    if (message == WM_TIMER && wParam == kTimerId) {
        if (state && !g_stopping.load()) {
            try {
                state->ApplyLayout();
            } catch (...) {
                LogException(L"Apply layout");
                state->Restore();
                KillTimer(window, kTimerId);
            }
        }

        return 0;
    }

    if (message == kCleanupMessage) {
        KillTimer(window, kTimerId);

        if (state) {
            state->Restore();
            state->root = {};
        }

        DestroyWindow(window);
        return 0;
    }

    if (message == WM_NCDESTROY) {
        if (state && !state->disabled) {
            state->Restore();
        }

        SetWindowLongPtrW(window, GWLP_USERDATA, 0);
    }

    return DefWindowProcW(window, message, wParam, lParam);
}

void ObserveOverflowRoot(C::Grid const& root) {
    std::lock_guard lock(g_windowsMutex);

    if (g_stopping.load()) {
        return;
    }

    DWORD threadId = GetCurrentThreadId();

    for (auto const& owned : g_ownedWindows) {
        if (owned.state->threadId != threadId) {
            continue;
        }

        auto existing = owned.state->root.get();

        if (existing && SameObject(existing, root)) {
            if (!owned.state->disabled && IsWindow(owned.window)) {
                PostMessageW(owned.window, WM_TIMER, kTimerId, 0);
            }
            return;
        }
    }

    auto state = std::make_unique<State>(root);

    HWND window = CreateWindowExW(
        0,
        kWindowClass,
        L"",
        0,
        0, 0, 0, 0,
        HWND_MESSAGE,
        nullptr,
        g_self,
        state.get());

    if (!window) {
        Wh_Log(L"CreateWindowExW failed: %u", GetLastError());
        return;
    }

    if (!SetTimer(window, kTimerId, kTimerPeriodMs, nullptr)) {
        DWORD error = GetLastError();
        DestroyWindow(window);
        Wh_Log(L"SetTimer failed: %u", error);
        return;
    }

    g_ownedWindows.push_back(
        OwnedWindow{window, std::move(state)});

    PostMessageW(window, WM_TIMER, kTimerId, 0);
}

C::Grid ReadOverflowRoot(void* instance) {
    if (!instance) {
        return nullptr;
    }

    IUnknown* candidate = nullptr;
    SIZE_T bytesRead = 0;

    const auto address =
        reinterpret_cast<const BYTE*>(instance) +
        5 * sizeof(void*);

    // Protects the memory read, not the undocumented COM object layout.
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

    HRESULT result = candidate->QueryInterface(
        winrt::guid_of<C::Grid>(),
        winrt::put_abi(root));

    return SUCCEEDED(result) ? root : nullptr;
}

void WINAPI InitializeHook(void* instance) {
    g_initializeOriginal(instance);

    if (g_stopping.load()) {
        return;
    }

    try {
        if (auto root = ReadOverflowRoot(instance)) {
            ObserveOverflowRoot(root);
        }
    } catch (...) {
        LogException(L"Observe overflow");
    }
}

unsigned GetModuleMajorVersion(HMODULE module) {
    HRSRC resource = FindResourceW(
        module,
        MAKEINTRESOURCEW(VS_VERSION_INFO),
        RT_VERSION);

    if (!resource) {
        return 0;
    }

    HGLOBAL loaded = LoadResource(module, resource);
    if (!loaded) {
        return 0;
    }

    void* data = LockResource(loaded);
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
        size < sizeof(*info)) {
        return 0;
    }

    return HIWORD(info->dwFileVersionMS);
}

HMODULE FindSystemTrayModule() {
    if (auto module = GetModuleHandleW(L"SystemTray.dll")) {
        return module;
    }

    if (auto module = GetModuleHandleW(L"Taskbar.View.dll")) {
        unsigned major = GetModuleMajorVersion(module);

        if (major != 0 && major < 2604) {
            return module;
        }
    }

    return GetModuleHandleW(L"ExplorerExtensions.dll");
}

bool InstallSystemTraySymbols(HMODULE module) {
    // SystemTray.dll, Taskbar.View.dll, ExplorerExtensions.dll
    WindhawkUtils::SYMBOL_HOOK hooks[] = {
        {
            {
                LR"(private: void __cdecl winrt::SystemTray::OverflowXamlIslandManager::InitializeIfNeeded(void))"
            },
            &g_initializeOriginal,
            InitializeHook,
        },
    };

    return WindhawkUtils::HookSymbols(
        module,
        hooks,
        ARRAYSIZE(hooks));
}

void TryInstallHooks(bool applyOperations) {
    if (g_stopping.load() || g_hookState.load() != 0) {
        return;
    }

    HMODULE module = FindSystemTrayModule();
    if (!module) {
        return;
    }

    int expected = 0;
    if (!g_hookState.compare_exchange_strong(expected, 1)) {
        return;
    }

    try {
        if (!InstallSystemTraySymbols(module)) {
            g_hookState.store(-1);
            Wh_Log(L"Unsupported SystemTray symbols");
            return;
        }

        if (applyOperations) {
            Wh_ApplyHookOperations();
        }

        g_hookState.store(2);
        Wh_Log(L"Tray overflow hook installed");
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
        g_loadLibraryExWOriginal(fileName, file, flags);

    if (module &&
        !(flags & (
            LOAD_LIBRARY_AS_DATAFILE |
            LOAD_LIBRARY_AS_DATAFILE_EXCLUSIVE |
            LOAD_LIBRARY_AS_IMAGE_RESOURCE))) {
        TryInstallHooks(true);
    }

    return module;
}

void StopAndRestore() {
    g_stopping.store(true);

    std::vector<HWND> windows;

    {
        std::lock_guard lock(g_windowsMutex);

        for (auto const& owned : g_ownedWindows) {
            windows.push_back(owned.window);
        }
    }

    // Synchronous cleanup keeps callbacks from surviving DLL unload.
    for (HWND window : windows) {
        if (IsWindow(window)) {
            SendMessageW(window, kCleanupMessage, 0, 0);
        }
    }

    {
        std::lock_guard lock(g_windowsMutex);
        g_ownedWindows.clear();
    }
}

}  // namespace

BOOL Wh_ModInit() {
    LoadSettings();

    if (!GetModuleHandleExW(
            GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
            reinterpret_cast<LPCWSTR>(&HelperWindowProc),
            &g_self)) {
        return FALSE;
    }

    WNDCLASSW windowClass{};
    windowClass.lpfnWndProc = HelperWindowProc;
    windowClass.hInstance = g_self;
    windowClass.lpszClassName = kWindowClass;

    g_windowClass = RegisterClassW(&windowClass);

    if (!g_windowClass) {
        Wh_Log(L"RegisterClassW failed: %u", GetLastError());
        return FALSE;
    }

    HMODULE kernelBase = GetModuleHandleW(L"kernelbase.dll");

    auto loadLibrary = kernelBase
        ? reinterpret_cast<LoadLibraryExWFn>(
              GetProcAddress(kernelBase, "LoadLibraryExW"))
        : nullptr;

    if (!loadLibrary ||
        !WindhawkUtils::SetFunctionHook(
            loadLibrary,
            LoadLibraryExWHook,
            &g_loadLibraryExWOriginal)) {

        UnregisterClassW(kWindowClass, g_self);
        g_windowClass = 0;
        return FALSE;
    }

    TryInstallHooks(false);

    if (g_hookState.load() == -1) {
        UnregisterClassW(kWindowClass, g_self);
        g_windowClass = 0;
        return FALSE;
    }

    return TRUE;
}

void Wh_ModAfterInit() {
    TryInstallHooks(true);
}

void Wh_ModSettingsChanged() {
    LoadSettings();

    std::lock_guard lock(g_windowsMutex);

    for (const auto& owned : g_ownedWindows) {
        if (owned.window && IsWindow(owned.window)) {
            PostMessageW(
                owned.window,
                WM_TIMER,
                kTimerId,
                0);
        }
    }
}

void Wh_ModBeforeUninit() {
    StopAndRestore();
}

void Wh_ModUninit() {
    if (g_windowClass) {
        UnregisterClassW(kWindowClass, g_self);
        g_windowClass = 0;
    }
}
