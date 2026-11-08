// ==WindhawkMod==
// @id              compact-start-menu
// @name            Compact Start Menu
// @description     Cleans up the Windows 11 Start menu by showing only a compact, ungrouped All apps view.
// @version         1.0.0
// @author          Asteski
// @github          https://github.com/Asteski
// @include         StartMenuExperienceHost.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject
// ==/WindhawkMod==

// ==WindhawkModSettings==
/*
- hideTopLevelHeader: true
  $name: Hide top header
  $description: Hide the All apps top header row with the title and view selector.
- hideCategoryViewOption: true
  $name: Hide category view option
  $description: Hide the Category option from the All apps view selector.
- headerText: "Installed apps"
  $name: Change header text
  $description: Text to show in the All apps header.
- hiddenHeaderIconGap: 0
  $name: Hidden header icon gap
  $description: "Top gap, in pixels, between search and the first icon row when the top header is hidden. Recommended values range from 16 to 24."
- scrollBarMode: "showWhileScrolling"
  $name: Scroll bar
  $description: Controls All apps scroll bar visibility.
  $options:
  - show: Show
  - hide: Hide
  - showWhileScrolling: Show while scrolling
- SectionsAndHeaders:
    - showSectionSeparators: false
      $name: Show section separators
      $description: Add thin separator lines between Start menu sections.
    - hidePinnedSection: true
      $name: Hide pinned section
      $description: Hide the Start menu pinned apps section.
    - hidePinnedHeader: true
      $name: Hide pinned header
      $description: Hide the Pinned section header.
    - hideRecommendedSection: true
      $name: Hide recommended section
      $description: Hide the Start menu recommended section.
    - hideRecommendedHeader: true
      $name: Hide recommended header
      $description: Hide the Recommended section header.
  $name: Other sections
*/
// ==/WindhawkModSettings==

// ==WindhawkModReadme==
/*
# Compact Start Menu
This mod simplifies the Start Menu by directly adjusting All apps XAML sections.

| All apps without header (default) | Pinned without header + Separator + All apps in List view |
| :---: | :---: |
| ![Default](https://raw.githubusercontent.com/Asteski/windhawk-mods/main/img/compact-start-menu/1.png) | ![Pinned section and separator](https://raw.githubusercontent.com/Asteski/windhawk-mods/main/img/compact-start-menu/2.png) |

### Features:
* **Shows "All apps":** Keeps the app list visible.
* **Optional pinned section hiding:** Can hide or show the section with pinned apps.
* **Optional recommended section hiding:** Can hide or show the section with recent files.
* **Optional section separators:** Can add thin separator lines between sections.
* **Optional section headers:** Can hide pinned and recommended headers independently.
* **Hides group headers:** Removes the visible All apps group headers in grid and list views.
* **Compacts grid view:** Reduces group-header spacing in the All apps grid.
* **Optional top header:** Can hide the All apps title/view row.
* **Optional category view hiding:** Can hide the Category option from the view selector.
* **Hidden header icon gap:** Can adjust the first icon row spacing when the top header is hidden.
* **Scrollbar modes:** Can show, hide, or reveal the visual All apps scroll bar only while scrolling.

### Instructions:
* Enable the mod in Windhawk.
* **Restart StartMenuExperienceHost.exe**, sign out/in, or restart your computer to apply changes.
*/
// ==/WindhawkModReadme==

#include <windhawk_utils.h>
#include <atomic>
#include <chrono>
#include <string>
#include <vector>

#undef GetCurrentTime

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.UI.h>
#include <winrt/Windows.UI.Xaml.Controls.Primitives.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Data.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.Shapes.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/base.h>

namespace wf = winrt::Windows::Foundation;
namespace wfc = winrt::Windows::Foundation::Collections;
namespace wu = winrt::Windows::UI;
namespace wux = winrt::Windows::UI::Xaml;
namespace wuc = winrt::Windows::UI::Xaml::Controls;
namespace wucp = winrt::Windows::UI::Xaml::Controls::Primitives;
namespace wuxm = winrt::Windows::UI::Xaml::Media;
namespace wuxs = winrt::Windows::UI::Xaml::Shapes;
namespace wuxd = winrt::Windows::UI::Xaml::Data;

enum class ScrollBarMode {
    Show,
    Hide,
    ShowWhileScrolling,
};

struct Settings {
    bool showSectionSeparators = false;
    bool hidePinnedSection = true;
    bool hidePinnedHeader = true;
    bool hideRecommendedSection = true;
    bool hideRecommendedHeader = true;
    bool hideTopLevelHeader = true;
    std::wstring headerText = L"Installed apps";
    bool hideCategoryViewOption = true;
    int hiddenHeaderIconGap = 0;
    ScrollBarMode scrollBarMode = ScrollBarMode::ShowWhileScrolling;
};

Settings& g_settings = *new Settings();

std::atomic<bool> g_xamlTraversalInstalled = false;
thread_local bool g_processingXamlElement = false;

struct FlattenedSourceEntry {
    winrt::weak_ref<wuc::ItemsControl> itemsControl;
    wf::IInspectable originalSource{nullptr};
    wfc::IObservableVector<wf::IInspectable> flatSource{nullptr};
    std::vector<wuc::GroupStyle> originalGroupStyles;
};

struct ElementState {
    wux::Visibility visibility;
    double width;
    double minWidth;
    double maxWidth;
    double minHeight;
    double height;
    double maxHeight;
    wux::Thickness margin;
    wux::HorizontalAlignment horizontalAlignment;
    wux::VerticalAlignment verticalAlignment;
    double opacity;
    bool isHitTestVisible;
};

struct CollapsedElementEntry {
    winrt::weak_ref<wux::FrameworkElement> element;
    ElementState originalState;
    int64_t visibilityToken = 0;
    int64_t widthToken = 0;
    int64_t heightToken = 0;
};

struct SemanticZoomEntry {
    winrt::weak_ref<wuc::SemanticZoom> semanticZoom;
    bool originalCanChangeViews;
    bool originalIsZoomOutButtonEnabled;
    bool originalIsZoomedInViewActive;
    int64_t canChangeViewsToken = 0;
    int64_t isZoomOutButtonEnabledToken = 0;
    int64_t isZoomedInViewActiveToken = 0;
};

struct ItemsWrapGridEntry {
    winrt::weak_ref<wuc::ItemsWrapGrid> itemsWrapGrid;
    wux::Thickness originalMargin;
    wux::Thickness originalGroupPadding;
    bool originalAreStickyGroupHeadersEnabled;
    wucp::GroupHeaderPlacement originalGroupHeaderPlacement;
    int64_t marginToken = 0;
};

struct HeaderTextEntry {
    winrt::weak_ref<wuc::TextBlock> textBlock;
    winrt::hstring originalText;
    int64_t textToken = 0;
};

struct ScrollViewerEntry {
    winrt::weak_ref<wuc::ScrollViewer> scrollViewer;
    wuc::ScrollBarVisibility originalVerticalScrollBarVisibility;
    winrt::event_token viewChangedToken{};
    bool hasViewChangedToken = false;
};

struct ScrollBarEntry {
    winrt::weak_ref<wucp::ScrollBar> scrollBar;
    double originalOpacity;
    bool originalIsHitTestVisible;
};

struct SectionSeparatorEntry {
    winrt::weak_ref<wuc::Panel> parent;
    winrt::weak_ref<wux::UIElement> separator;
    int kind;
};

std::vector<FlattenedSourceEntry>& g_flattenedSources =
    *new std::vector<FlattenedSourceEntry>();
std::vector<SemanticZoomEntry>& g_hookedSemanticZooms =
    *new std::vector<SemanticZoomEntry>();
std::vector<ItemsWrapGridEntry>& g_gapAdjustedItemsWrapGrids =
    *new std::vector<ItemsWrapGridEntry>();
std::vector<CollapsedElementEntry>& g_keepCollapsedElements =
    *new std::vector<CollapsedElementEntry>();
std::vector<HeaderTextEntry>& g_headerTextElements =
    *new std::vector<HeaderTextEntry>();
std::vector<ScrollViewerEntry>& g_scrollBarModeScrollViewers =
    *new std::vector<ScrollViewerEntry>();
std::vector<ScrollBarEntry>& g_scrollBarModeScrollBars =
    *new std::vector<ScrollBarEntry>();
std::vector<SectionSeparatorEntry>& g_sectionSeparators =
    *new std::vector<SectionSeparatorEntry>();

struct TimerState {
    wux::DispatcherTimer xamlTraversalTimer{nullptr};
    wux::DispatcherTimer scrollBarHideTimer{nullptr};
    wux::DispatcherTimer scrollBarFadeTimer{nullptr};
};

TimerState& g_timers = *new TimerState();
wux::DispatcherTimer& g_xamlTraversalTimer = g_timers.xamlTraversalTimer;
wux::DispatcherTimer& g_scrollBarHideTimer = g_timers.scrollBarHideTimer;
wux::DispatcherTimer& g_scrollBarFadeTimer = g_timers.scrollBarFadeTimer;

const wchar_t kSectionSeparatorName[] = L"CompactStartMenuSectionSeparator";

void LoadSettings() {
    g_settings.showSectionSeparators =
        Wh_GetIntSetting(L"SectionsAndHeaders.showSectionSeparators") != 0;

    g_settings.hidePinnedSection =
        Wh_GetIntSetting(L"SectionsAndHeaders.hidePinnedSection") != 0;
    g_settings.hidePinnedHeader =
        Wh_GetIntSetting(L"SectionsAndHeaders.hidePinnedHeader") != 0;
    g_settings.hideRecommendedSection =
        Wh_GetIntSetting(L"SectionsAndHeaders.hideRecommendedSection") != 0;
    g_settings.hideRecommendedHeader =
        Wh_GetIntSetting(L"SectionsAndHeaders.hideRecommendedHeader") != 0;
    g_settings.hideTopLevelHeader = Wh_GetIntSetting(L"hideTopLevelHeader") != 0;

    auto headerText = WindhawkUtils::StringSetting::make(L"headerText");
    PCWSTR headerTextValue = headerText.get();
    g_settings.headerText =
        *headerTextValue ? headerTextValue : L"Installed apps";

    g_settings.hideCategoryViewOption =
        Wh_GetIntSetting(L"hideCategoryViewOption") != 0;
    g_settings.hiddenHeaderIconGap = Wh_GetIntSetting(L"hiddenHeaderIconGap");
    if (g_settings.hiddenHeaderIconGap < 0) {
        g_settings.hiddenHeaderIconGap = 0;
    }

    auto scrollBarMode = WindhawkUtils::StringSetting::make(L"scrollBarMode");
    PCWSTR scrollBarModeValue = scrollBarMode.get();
    if (lstrcmpiW(scrollBarModeValue, L"hide") == 0) {
        g_settings.scrollBarMode = ScrollBarMode::Hide;
    } else if (lstrcmpiW(scrollBarModeValue, L"showWhileScrolling") == 0) {
        g_settings.scrollBarMode = ScrollBarMode::ShowWhileScrolling;
    } else {
        g_settings.scrollBarMode = ScrollBarMode::Show;
    }
}

bool IsPinnedSectionElement(wux::FrameworkElement const& element) {
    auto name = element.Name();
    auto className = winrt::get_class_name(element);

    return name == L"StartMenuPinnedList" ||
           name == L"PinnedList" ||
           name == L"PinnedListPipsPager" ||
           className == L"StartMenu.PinnedList";
}

bool IsPinnedHeaderElement(wux::FrameworkElement const& element) {
    auto name = element.Name();
    return name == L"PinnedListHeaderGrid" ||
           name == L"PinnedListHeaderText" ||
           name == L"ShowMorePinnedGrid" ||
           name == L"ShowMorePinnedButton" ||
           name == L"ShowMorePinnedButtonText";
}

bool IsRecommendedHeaderElement(wux::FrameworkElement const& element) {
    auto name = element.Name();
    return name == L"TopLevelSuggestionsListHeader" ||
           name == L"TopLevelSuggestionsListHeaderText" ||
           name == L"MoreSuggestionsListHeaderText" ||
           name == L"ShowMoreSuggestions" ||
           name == L"ShowMoreSuggestionsButton" ||
           name == L"HideMoreSuggestionsButton";
}

bool IsAllAppsTopHeaderElement(wux::FrameworkElement const& element) {
    auto name = element.Name();
    return name == L"AllAppsPaneHeader" ||
           name == L"AllListHeading" ||
           name == L"AllListHeadingText" ||
           name == L"ViewSelectionButton";
}

bool IsGroupHeaderElement(wux::FrameworkElement const& element) {
    auto className = winrt::get_class_name(element);

    return element.try_as<wuc::GridViewHeaderItem>() ||
           element.try_as<wuc::ListViewHeaderItem>() ||
           className == L"Windows.UI.Xaml.Controls.GridViewHeaderItem" ||
           className == L"Windows.UI.Xaml.Controls.ListViewHeaderItem";
}

void CollapseElement(wux::FrameworkElement const& element) {
    if (element.Visibility() != wux::Visibility::Collapsed) {
        element.Visibility(wux::Visibility::Collapsed);
    }
    if (element.Width() != 0) {
        element.Width(0);
    }
    if (element.MinWidth() != 0) {
        element.MinWidth(0);
    }
    if (element.MaxWidth() != 0) {
        element.MaxWidth(0);
    }
    if (element.MinHeight() != 0) {
        element.MinHeight(0);
    }
    if (element.Height() != 0) {
        element.Height(0);
    }
    if (element.MaxHeight() != 0) {
        element.MaxHeight(0);
    }
    if (element.Margin() != wux::Thickness{}) {
        element.Margin({});
    }
    if (element.HorizontalAlignment() != wux::HorizontalAlignment::Left) {
        element.HorizontalAlignment(wux::HorizontalAlignment::Left);
    }
    if (element.VerticalAlignment() != wux::VerticalAlignment::Top) {
        element.VerticalAlignment(wux::VerticalAlignment::Top);
    }
    if (element.Opacity() != 0) {
        element.Opacity(0);
    }
    if (element.IsHitTestVisible()) {
        element.IsHitTestVisible(false);
    }
}

ElementState CaptureElementState(wux::FrameworkElement const& element) {
    return {
        element.Visibility(),
        element.Width(),
        element.MinWidth(),
        element.MaxWidth(),
        element.MinHeight(),
        element.Height(),
        element.MaxHeight(),
        element.Margin(),
        element.HorizontalAlignment(),
        element.VerticalAlignment(),
        element.Opacity(),
        element.IsHitTestVisible(),
    };
}

void RestoreElementState(wux::FrameworkElement const& element,
                         ElementState const& state) {
    element.Visibility(state.visibility);
    element.Width(state.width);
    element.MinWidth(state.minWidth);
    element.MaxWidth(state.maxWidth);
    element.MinHeight(state.minHeight);
    element.Height(state.height);
    element.MaxHeight(state.maxHeight);
    element.Margin(state.margin);
    element.HorizontalAlignment(state.horizontalAlignment);
    element.VerticalAlignment(state.verticalAlignment);
    element.Opacity(state.opacity);
    element.IsHitTestVisible(state.isHitTestVisible);
}

void KeepElementCollapsed(wux::FrameworkElement const& element) {
    for (auto const& collapsedElement : g_keepCollapsedElements) {
        if (collapsedElement.element.get() == element) {
            return;
        }
    }

    CollapsedElementEntry entry{
        winrt::make_weak(element),
        CaptureElementState(element),
    };

    entry.visibilityToken = element.RegisterPropertyChangedCallback(
        wux::UIElement::VisibilityProperty(),
        [](wux::DependencyObject const& sender, wux::DependencyProperty const&) {
            if (auto element = sender.try_as<wux::FrameworkElement>()) {
                if (element.Visibility() != wux::Visibility::Collapsed) {
                    CollapseElement(element);
                }
            }
        });
    entry.widthToken = element.RegisterPropertyChangedCallback(
        wux::FrameworkElement::WidthProperty(),
        [](wux::DependencyObject const& sender, wux::DependencyProperty const&) {
            if (auto element = sender.try_as<wux::FrameworkElement>()) {
                if (element.Width() != 0) {
                    CollapseElement(element);
                }
            }
        });
    entry.heightToken = element.RegisterPropertyChangedCallback(
        wux::FrameworkElement::HeightProperty(),
        [](wux::DependencyObject const& sender, wux::DependencyProperty const&) {
            if (auto element = sender.try_as<wux::FrameworkElement>()) {
                if (element.Height() != 0) {
                    CollapseElement(element);
                }
            }
        });

    g_keepCollapsedElements.push_back(std::move(entry));
}

void CollapseAndKeepElement(wux::FrameworkElement const& element) {
    KeepElementCollapsed(element);
    CollapseElement(element);
}

bool IsCategoryText(winrt::hstring const& text) {
    return lstrcmpiW(text.c_str(), L"Category") == 0;
}

bool IsLastMenuFlyoutItemInThreeItemFlyout(wuc::MenuFlyoutItem const& item) {
    auto parent = wux::Media::VisualTreeHelper::GetParent(item)
                      .try_as<wux::FrameworkElement>();
    if (!parent) {
        return false;
    }

    auto panel = parent.try_as<wuc::Panel>();
    if (!panel) {
        return false;
    }

    auto children = panel.Children();
    uint32_t menuFlyoutItemCount = 0;
    uint32_t itemMenuFlyoutIndex = 0;
    bool foundItem = false;

    for (uint32_t i = 0; i < children.Size(); i++) {
        auto childMenuFlyoutItem = children.GetAt(i).try_as<wuc::MenuFlyoutItem>();
        if (!childMenuFlyoutItem) {
            continue;
        }

        if (childMenuFlyoutItem == item) {
            itemMenuFlyoutIndex = menuFlyoutItemCount;
            foundItem = true;
        }

        menuFlyoutItemCount++;
    }

    return foundItem &&
           menuFlyoutItemCount == 3 &&
           itemMenuFlyoutIndex == menuFlyoutItemCount - 1;
}

bool IsCategoryViewOption(wuc::MenuFlyoutItem const& item) {
    return IsCategoryText(item.Text()) ||
           IsLastMenuFlyoutItemInThreeItemFlyout(item);
}

wux::FrameworkElement FindCategoryViewOptionTarget(
    wux::FrameworkElement const& element) {
    if (auto menuFlyoutItem = element.try_as<wuc::MenuFlyoutItem>()) {
        if (IsCategoryViewOption(menuFlyoutItem)) {
            return element;
        }
    }

    if (auto textBlock = element.try_as<wuc::TextBlock>()) {
        auto current = element;
        while (current) {
            if (auto menuFlyoutItem = current.try_as<wuc::MenuFlyoutItem>()) {
                if (IsCategoryText(textBlock.Text()) ||
                    IsCategoryViewOption(menuFlyoutItem)) {
                    return current;
                }

                return nullptr;
            }

            current = wux::Media::VisualTreeHelper::GetParent(current)
                          .try_as<wux::FrameworkElement>();
        }

        if (IsCategoryText(textBlock.Text())) {
            return element;
        }
    }

    return nullptr;
}

void ApplyHeaderText(wux::FrameworkElement const& element) {
    if (g_settings.hideTopLevelHeader) {
        return;
    }

    if (element.Name() != L"AllListHeadingText") {
        return;
    }

    if (auto textBlock = element.try_as<wuc::TextBlock>()) {
        for (auto const& headerTextElement : g_headerTextElements) {
            if (headerTextElement.textBlock.get() == textBlock) {
                if (textBlock.Text() != g_settings.headerText) {
                    textBlock.Text(g_settings.headerText);
                }
                return;
            }
        }

        HeaderTextEntry entry{
            winrt::make_weak(textBlock),
            textBlock.Text(),
        };

        if (textBlock.Text() != g_settings.headerText) {
            textBlock.Text(g_settings.headerText);
        }

        entry.textToken = textBlock.RegisterPropertyChangedCallback(
            wuc::TextBlock::TextProperty(),
            [](wux::DependencyObject const& sender, wux::DependencyProperty const&) {
                if (auto textBlock = sender.try_as<wuc::TextBlock>()) {
                    if (textBlock.Text() != g_settings.headerText) {
                        textBlock.Text(g_settings.headerText);
                    }
                }
            });

        g_headerTextElements.push_back(std::move(entry));
    }
}

void ApplyHiddenHeaderIconGap(wuc::ItemsWrapGrid const& itemsWrapGrid);

ItemsWrapGridEntry* FindItemsWrapGridEntry(
    wuc::ItemsWrapGrid const& itemsWrapGrid) {
    for (auto& adjustedItemsWrapGrid : g_gapAdjustedItemsWrapGrids) {
        if (adjustedItemsWrapGrid.itemsWrapGrid.get() == itemsWrapGrid) {
            return &adjustedItemsWrapGrid;
        }
    }

    return nullptr;
}

void EnsureItemsWrapGridEntry(wuc::ItemsWrapGrid const& itemsWrapGrid) {
    if (FindItemsWrapGridEntry(itemsWrapGrid)) {
        return;
    }

    ItemsWrapGridEntry entry{
        winrt::make_weak(itemsWrapGrid),
        itemsWrapGrid.Margin(),
        itemsWrapGrid.GroupPadding(),
        itemsWrapGrid.AreStickyGroupHeadersEnabled(),
        itemsWrapGrid.GroupHeaderPlacement(),
    };

    entry.marginToken = itemsWrapGrid.RegisterPropertyChangedCallback(
        wux::FrameworkElement::MarginProperty(),
        [](wux::DependencyObject const& sender, wux::DependencyProperty const&) {
            if (auto itemsWrapGrid = sender.try_as<wuc::ItemsWrapGrid>()) {
                ApplyHiddenHeaderIconGap(itemsWrapGrid);
            }
        });

    g_gapAdjustedItemsWrapGrids.push_back(std::move(entry));
}

void ApplyHiddenHeaderIconGap(wuc::ItemsWrapGrid const& itemsWrapGrid) {
    if (!g_settings.hideTopLevelHeader) {
        return;
    }

    EnsureItemsWrapGridEntry(itemsWrapGrid);

    auto margin = itemsWrapGrid.Margin();
    double topGap = static_cast<double>(g_settings.hiddenHeaderIconGap);
    if (margin.Top != topGap) {
        margin.Top = topGap;
        itemsWrapGrid.Margin(margin);
    }
}

bool IsInAllAppsArea(wux::FrameworkElement const& element) {
    for (auto current = element; current;) {
        if (current.Name() == L"AllAppsGrid" ||
            current.Name() == L"AppsList" ||
            (winrt::get_class_name(current) == L"StartDocked.AllAppsGridListView")) {
            return true;
        }

        current = wux::Media::VisualTreeHelper::GetParent(current)
                      .try_as<wux::FrameworkElement>();
    }

    return false;
}

void CompactAllAppsItemsWrapGrid(wuc::ItemsWrapGrid const& itemsWrapGrid) {
    if (!IsInAllAppsArea(itemsWrapGrid)) {
        return;
    }

    EnsureItemsWrapGridEntry(itemsWrapGrid);

    itemsWrapGrid.GroupPadding({});
    itemsWrapGrid.AreStickyGroupHeadersEnabled(false);
    itemsWrapGrid.GroupHeaderPlacement(wucp::GroupHeaderPlacement::Top);
    ApplyHiddenHeaderIconGap(itemsWrapGrid);
}

bool IsAllAppsGridView(wux::FrameworkElement const& element) {
    return element.Name() == L"AllAppsGrid" ||
           element.Name() == L"AppsList" ||
           winrt::get_class_name(element) == L"StartDocked.AllAppsGridListView";
}

bool IsPinnedSectionSeparatorTarget(wux::FrameworkElement const& element) {
    auto name = element.Name();
    auto className = winrt::get_class_name(element);

    return name == L"StartMenuPinnedList" ||
           name == L"PinnedList" ||
           className == L"StartMenu.PinnedList";
}

bool IsRecommendedSectionSeparatorTarget(wux::FrameworkElement const& element) {
    return element.Name() == L"TopLevelSuggestionsRoot";
}

enum SectionSeparatorKind {
    SectionSeparatorKindNone = 0,
    SectionSeparatorKindPinned = 1,
    SectionSeparatorKindRecommended = 2,
};

int GetSectionSeparatorKind(wux::FrameworkElement const& element) {
    if (!g_settings.showSectionSeparators) {
        return SectionSeparatorKindNone;
    }

    bool pinnedVisible = !g_settings.hidePinnedSection;
    bool recommendedVisible = !g_settings.hideRecommendedSection;

    if (pinnedVisible && IsPinnedSectionSeparatorTarget(element)) {
        return SectionSeparatorKindPinned;
    }

    if (recommendedVisible && IsRecommendedSectionSeparatorTarget(element)) {
        return SectionSeparatorKindRecommended;
    }

    return SectionSeparatorKindNone;
}

void ApplySectionSeparator(wux::FrameworkElement const& target) {
    int separatorKind = GetSectionSeparatorKind(target);
    if (separatorKind == SectionSeparatorKindNone) {
        return;
    }

    for (auto const& entry : g_sectionSeparators) {
        if (entry.kind == separatorKind) {
            return;
        }
    }

    auto parent = wux::Media::VisualTreeHelper::GetParent(target).try_as<wuc::Panel>();
    if (!parent) {
        return;
    }

    auto children = parent.Children();
    uint32_t targetIndex = 0;
    if (!children.IndexOf(target.as<wux::UIElement>(), targetIndex)) {
        return;
    }

    wuxs::Rectangle separator;
    separator.Name(kSectionSeparatorName);
    separator.Height(1);
    separator.MinHeight(1);
    separator.HorizontalAlignment(wux::HorizontalAlignment::Stretch);
    separator.VerticalAlignment(wux::VerticalAlignment::Bottom);
    separator.Margin({40, 0, 40, 0});
    separator.Opacity(0.55);
    separator.IsHitTestVisible(false);
    separator.Fill(wuxm::SolidColorBrush(wu::Color{255, 160, 160, 160}));

    if (auto grid = parent.try_as<wuc::Grid>()) {
        wuc::Grid::SetRow(separator, wuc::Grid::GetRow(target));
        wuc::Grid::SetRowSpan(separator, wuc::Grid::GetRowSpan(target));
        wuc::Grid::SetColumn(separator, wuc::Grid::GetColumn(target));
        wuc::Grid::SetColumnSpan(separator, wuc::Grid::GetColumnSpan(target));
        children.Append(separator);
    } else {
        children.InsertAt(targetIndex + 1, separator);
    }

    g_sectionSeparators.push_back({
        winrt::make_weak(parent),
        winrt::make_weak(separator.as<wux::UIElement>()),
        separatorKind,
    });
}

std::vector<wf::IInspectable> GetFlatItemsFromGroupedSource(
    wf::IInspectable const& source) {
    std::vector<wf::IInspectable> flatItems;

    auto collectionView = source.try_as<wuxd::ICollectionView>();
    if (!collectionView) {
        return flatItems;
    }

    auto groups = collectionView.CollectionGroups();
    if (!groups || groups.Size() == 0) {
        return flatItems;
    }

    for (uint32_t groupIndex = 0; groupIndex < groups.Size(); groupIndex++) {
        auto group = groups.GetAt(groupIndex).try_as<wuxd::ICollectionViewGroup>();
        if (!group) {
            continue;
        }

        auto groupItems = group.GroupItems();
        if (!groupItems) {
            continue;
        }

        flatItems.reserve(flatItems.size() + groupItems.Size());
        for (uint32_t itemIndex = 0; itemIndex < groupItems.Size(); itemIndex++) {
            flatItems.push_back(groupItems.GetAt(itemIndex));
        }
    }

    return flatItems;
}

bool FlatSourceMatchesItems(
    wfc::IObservableVector<wf::IInspectable> const& flatSource,
    std::vector<wf::IInspectable> const& flatItems) {
    if (!flatSource || flatSource.Size() != flatItems.size()) {
        return false;
    }

    for (uint32_t i = 0; i < flatItems.size(); i++) {
        if (flatSource.GetAt(i) != flatItems[i]) {
            return false;
        }
    }

    return true;
}

void ReplaceFlatSourceItems(
    wfc::IObservableVector<wf::IInspectable> const& flatSource,
    std::vector<wf::IInspectable> const& flatItems) {
    flatSource.Clear();
    for (auto const& item : flatItems) {
        flatSource.Append(item);
    }
}

void RefreshFlattenedSource(FlattenedSourceEntry& entry) {
    auto itemsControl = entry.itemsControl.get();
    if (!itemsControl || !entry.originalSource) {
        return;
    }

    auto currentSource = itemsControl.ItemsSource();
    if (entry.flatSource && currentSource != entry.flatSource) {
        entry.originalSource = currentSource;
    }

    auto flatItems = GetFlatItemsFromGroupedSource(entry.originalSource);
    if (flatItems.empty()) {
        return;
    }

    if (!entry.flatSource) {
        entry.flatSource =
            winrt::single_threaded_observable_vector<wf::IInspectable>();
    }

    if (!FlatSourceMatchesItems(entry.flatSource, flatItems)) {
        ReplaceFlatSourceItems(entry.flatSource, flatItems);
    }

    if (itemsControl.ItemsSource() != entry.flatSource) {
        itemsControl.ItemsSource(entry.flatSource);
    }

    if (itemsControl.GroupStyle().Size() > 0) {
        itemsControl.GroupStyle().Clear();
    }
}

void FlattenAllAppsGridSource(wux::FrameworkElement const& element) {
    if (!IsAllAppsGridView(element)) {
        return;
    }

    auto itemsControl = element.try_as<wuc::ItemsControl>();
    if (!itemsControl) {
        return;
    }

    for (auto& entry : g_flattenedSources) {
        if (entry.itemsControl.get() == itemsControl) {
            RefreshFlattenedSource(entry);
            return;
        }
    }

    auto originalSource = itemsControl.ItemsSource();
    auto flatItems = GetFlatItemsFromGroupedSource(originalSource);
    if (flatItems.empty()) {
        return;
    }

    std::vector<wuc::GroupStyle> originalGroupStyles;
    auto groupStyles = itemsControl.GroupStyle();
    originalGroupStyles.reserve(groupStyles.Size());
    for (uint32_t i = 0; i < groupStyles.Size(); i++) {
        originalGroupStyles.push_back(groupStyles.GetAt(i));
    }

    auto flatSource = winrt::single_threaded_observable_vector(std::move(flatItems));
    itemsControl.ItemsSource(flatSource);
    if (groupStyles.Size() > 0) {
        groupStyles.Clear();
    }

    g_flattenedSources.push_back({winrt::make_weak(itemsControl),
                                  originalSource,
                                  flatSource,
                                  std::move(originalGroupStyles)});
}

void FlattenAllAppsGridSourceFromAncestor(wux::FrameworkElement const& element) {
    auto current = element;
    while (current) {
        FlattenAllAppsGridSource(current);
        current = wux::Media::VisualTreeHelper::GetParent(current)
                      .try_as<wux::FrameworkElement>();
    }
}

void DisableCategorySemanticZoom(wuc::SemanticZoom const& semanticZoom) {
    if (!g_settings.hideCategoryViewOption) {
        return;
    }

    for (auto const& hookedSemanticZoom : g_hookedSemanticZooms) {
        if (hookedSemanticZoom.semanticZoom.get() == semanticZoom) {
            return;
        }
    }

    SemanticZoomEntry entry{
        winrt::make_weak(semanticZoom),
        semanticZoom.CanChangeViews(),
        semanticZoom.IsZoomOutButtonEnabled(),
        semanticZoom.IsZoomedInViewActive(),
    };

    semanticZoom.CanChangeViews(false);
    semanticZoom.IsZoomOutButtonEnabled(false);
    if (!semanticZoom.IsZoomedInViewActive()) {
        semanticZoom.IsZoomedInViewActive(true);
    }

    entry.canChangeViewsToken = semanticZoom.RegisterPropertyChangedCallback(
        wuc::SemanticZoom::CanChangeViewsProperty(),
        [](wux::DependencyObject const& sender, wux::DependencyProperty const&) {
            if (auto semanticZoom = sender.try_as<wuc::SemanticZoom>()) {
                if (semanticZoom.CanChangeViews()) {
                    semanticZoom.CanChangeViews(false);
                }
            }
        });

    entry.isZoomOutButtonEnabledToken = semanticZoom.RegisterPropertyChangedCallback(
        wuc::SemanticZoom::IsZoomOutButtonEnabledProperty(),
        [](wux::DependencyObject const& sender, wux::DependencyProperty const&) {
            if (auto semanticZoom = sender.try_as<wuc::SemanticZoom>()) {
                if (semanticZoom.IsZoomOutButtonEnabled()) {
                    semanticZoom.IsZoomOutButtonEnabled(false);
                }
            }
        });

    entry.isZoomedInViewActiveToken = semanticZoom.RegisterPropertyChangedCallback(
        wuc::SemanticZoom::IsZoomedInViewActiveProperty(),
        [](wux::DependencyObject const& sender, wux::DependencyProperty const&) {
            if (auto semanticZoom = sender.try_as<wuc::SemanticZoom>()) {
                if (!semanticZoom.IsZoomedInViewActive()) {
                    semanticZoom.IsZoomedInViewActive(true);
                }
            }
        });

    g_hookedSemanticZooms.push_back(std::move(entry));
}

void SetScrollViewerVerticalScrollBarVisibility(
    wuc::ScrollViewer const& scrollViewer,
    wuc::ScrollBarVisibility visibility) {
    if (scrollViewer.VerticalScrollBarVisibility() != visibility) {
        scrollViewer.VerticalScrollBarVisibility(visibility);
    }
}

void SetScrollBarOpacity(wucp::ScrollBar const& scrollBar, double opacity) {
    if (scrollBar.Opacity() != opacity) {
        scrollBar.Opacity(opacity);
    }

    bool visible = opacity > 0;
    if (scrollBar.IsHitTestVisible() != visible) {
        scrollBar.IsHitTestVisible(visible);
    }
}

void StopScrollBarFadeTimer() {
    if (g_scrollBarFadeTimer) {
        g_scrollBarFadeTimer.Stop();
    }
}

void FadeAutoScrollBars() {
    if (g_settings.scrollBarMode != ScrollBarMode::ShowWhileScrolling) {
        return;
    }

    bool stillFading = false;
    for (auto const& entry : g_scrollBarModeScrollBars) {
        if (auto scrollBar = entry.scrollBar.get()) {
            double opacity = scrollBar.Opacity();
            double nextOpacity = opacity > 0.2 ? opacity - 0.2 : 0;
            SetScrollBarOpacity(scrollBar, nextOpacity);
            if (nextOpacity > 0) {
                stillFading = true;
            }
        }
    }

    if (!stillFading) {
        StopScrollBarFadeTimer();
    }
}

void StartScrollBarFadeTimer() {
    if (!g_scrollBarFadeTimer) {
        g_scrollBarFadeTimer = wux::DispatcherTimer();
        g_scrollBarFadeTimer.Interval(std::chrono::milliseconds(50));
        g_scrollBarFadeTimer.Tick(
            [](wf::IInspectable const&, wf::IInspectable const&) {
                FadeAutoScrollBars();
            });
    }

    g_scrollBarFadeTimer.Stop();
    g_scrollBarFadeTimer.Start();
}

void RestartScrollBarHideTimer() {
    if (!g_scrollBarHideTimer) {
        g_scrollBarHideTimer = wux::DispatcherTimer();
        g_scrollBarHideTimer.Interval(std::chrono::milliseconds(200));
        g_scrollBarHideTimer.Tick(
            [](wf::IInspectable const&, wf::IInspectable const&) {
                if (g_scrollBarHideTimer) {
                    g_scrollBarHideTimer.Stop();
                }
                StartScrollBarFadeTimer();
            });
    }

    g_scrollBarHideTimer.Stop();
    g_scrollBarHideTimer.Start();
}

void ShowAutoScrollBarsWhileScrolling(wuc::ScrollViewer const& scrollViewer) {
    if (g_settings.scrollBarMode != ScrollBarMode::ShowWhileScrolling) {
        return;
    }

    StopScrollBarFadeTimer();
    SetScrollViewerVerticalScrollBarVisibility(
        scrollViewer, wuc::ScrollBarVisibility::Visible);

    for (auto const& entry : g_scrollBarModeScrollBars) {
        if (auto scrollBar = entry.scrollBar.get()) {
            SetScrollBarOpacity(scrollBar, 1);
        }
    }

    RestartScrollBarHideTimer();
}

ScrollBarEntry* EnsureScrollBarEntry(wucp::ScrollBar const& scrollBar) {
    for (auto& entry : g_scrollBarModeScrollBars) {
        if (entry.scrollBar.get() == scrollBar) {
            return &entry;
        }
    }

    g_scrollBarModeScrollBars.push_back({
        winrt::make_weak(scrollBar),
        scrollBar.Opacity(),
        scrollBar.IsHitTestVisible(),
    });

    return &g_scrollBarModeScrollBars.back();
}

void ApplyScrollBarElementMode(wucp::ScrollBar const& scrollBar) {
    if (!IsInAllAppsArea(scrollBar)) {
        return;
    }

    EnsureScrollBarEntry(scrollBar);

    if (g_settings.scrollBarMode == ScrollBarMode::Show) {
        SetScrollBarOpacity(scrollBar, 1);
        return;
    }

    if (g_settings.scrollBarMode == ScrollBarMode::Hide) {
        SetScrollBarOpacity(scrollBar, 0);
        return;
    }

    SetScrollBarOpacity(scrollBar, 0);
}

ScrollViewerEntry* EnsureScrollViewerEntry(
    wuc::ScrollViewer const& scrollViewer,
    bool registerViewChanged) {
    for (auto& entry : g_scrollBarModeScrollViewers) {
        if (entry.scrollViewer.get() == scrollViewer) {
            if (registerViewChanged && !entry.hasViewChangedToken) {
                entry.viewChangedToken = scrollViewer.ViewChanged(
                    [](wf::IInspectable const& sender,
                       wuc::ScrollViewerViewChangedEventArgs const&) {
                        if (auto scrollViewer = sender.try_as<wuc::ScrollViewer>()) {
                            ShowAutoScrollBarsWhileScrolling(scrollViewer);
                        }
                    });
                entry.hasViewChangedToken = true;
            }

            return &entry;
        }
    }

    ScrollViewerEntry entry{
        winrt::make_weak(scrollViewer),
        scrollViewer.VerticalScrollBarVisibility(),
    };

    if (registerViewChanged) {
        entry.viewChangedToken = scrollViewer.ViewChanged(
            [](wf::IInspectable const& sender,
               wuc::ScrollViewerViewChangedEventArgs const&) {
                if (auto scrollViewer = sender.try_as<wuc::ScrollViewer>()) {
                    ShowAutoScrollBarsWhileScrolling(scrollViewer);
                }
            });
        entry.hasViewChangedToken = true;
    }

    g_scrollBarModeScrollViewers.push_back(std::move(entry));
    return &g_scrollBarModeScrollViewers.back();
}

void ApplyScrollBarMode(wuc::ScrollViewer const& scrollViewer) {
    if (!IsInAllAppsArea(scrollViewer)) {
        return;
    }

    EnsureScrollViewerEntry(
        scrollViewer,
        g_settings.scrollBarMode == ScrollBarMode::ShowWhileScrolling);

    if (g_settings.scrollBarMode == ScrollBarMode::Show) {
        SetScrollViewerVerticalScrollBarVisibility(
            scrollViewer, wuc::ScrollBarVisibility::Auto);
        return;
    }

    if (g_settings.scrollBarMode == ScrollBarMode::Hide) {
        SetScrollViewerVerticalScrollBarVisibility(
            scrollViewer, wuc::ScrollBarVisibility::Hidden);
        return;
    }

    SetScrollViewerVerticalScrollBarVisibility(
        scrollViewer, wuc::ScrollBarVisibility::Visible);
}

void HideStartMenuElement(wux::FrameworkElement const& element) {
    ApplyHeaderText(element);

    FlattenAllAppsGridSource(element);
    ApplySectionSeparator(element);

    if (auto semanticZoom = element.try_as<wuc::SemanticZoom>()) {
        DisableCategorySemanticZoom(semanticZoom);
    }

    if (g_settings.hideCategoryViewOption) {
        if (auto categoryViewOption = FindCategoryViewOptionTarget(element)) {
            CollapseAndKeepElement(categoryViewOption);
            return;
        }
    }

    if (auto scrollViewer = element.try_as<wuc::ScrollViewer>()) {
        ApplyScrollBarMode(scrollViewer);
    }

    if (auto scrollBar = element.try_as<wucp::ScrollBar>()) {
        ApplyScrollBarElementMode(scrollBar);
    }

    if (IsGroupHeaderElement(element) ||
        (g_settings.hidePinnedSection &&
         IsPinnedSectionElement(element)) ||
        ((g_settings.hidePinnedSection || g_settings.hidePinnedHeader) &&
         IsPinnedHeaderElement(element)) ||
        (g_settings.hideRecommendedSection &&
         element.Name() == L"TopLevelSuggestionsRoot") ||
        ((g_settings.hideRecommendedSection ||
          g_settings.hideRecommendedHeader) &&
         IsRecommendedHeaderElement(element)) ||
        (g_settings.hideTopLevelHeader && IsAllAppsTopHeaderElement(element))) {
        CollapseAndKeepElement(element);
    }

    if (auto itemsWrapGrid = element.try_as<wuc::ItemsWrapGrid>()) {
        CompactAllAppsItemsWrapGrid(itemsWrapGrid);
        FlattenAllAppsGridSourceFromAncestor(element);
    } else if (IsGroupHeaderElement(element)) {
        FlattenAllAppsGridSourceFromAncestor(element);
    }
}

void ProcessXamlElement(wux::FrameworkElement const& element) {
    if (!element || g_processingXamlElement) {
        return;
    }

    g_processingXamlElement = true;

    try {
        HideStartMenuElement(element);
    } catch (...) {
        Wh_Log(L"ProcessXamlElement error: %08X", winrt::to_hresult());
    }

    g_processingXamlElement = false;
}

void ProcessXamlSubtree(wux::DependencyObject const& root) {
    if (!root) {
        return;
    }

    std::vector<wux::DependencyObject> stack;
    stack.push_back(root);

    while (!stack.empty()) {
        auto current = stack.back();
        stack.pop_back();

        if (auto element = current.try_as<wux::FrameworkElement>()) {
            ProcessXamlElement(element);
        }

        int childCount = wux::Media::VisualTreeHelper::GetChildrenCount(current);
        for (int i = childCount - 1; i >= 0; i--) {
            auto child = wux::Media::VisualTreeHelper::GetChild(current, i);
            if (child) {
                stack.push_back(child);
            }
        }
    }
}

void ProcessCurrentXamlTree() {
    try {
        auto window = wux::Window::Current();
        if (!window) {
            return;
        }

        auto root = window.Content().try_as<wux::DependencyObject>();
        ProcessXamlSubtree(root);

        auto popups = wux::Media::VisualTreeHelper::GetOpenPopups(window);
        for (uint32_t i = 0; i < popups.Size(); i++) {
            auto popup = popups.GetAt(i);
            if (!popup.IsOpen()) {
                continue;
            }

            if (auto popupChild = popup.Child().try_as<wux::DependencyObject>()) {
                ProcessXamlSubtree(popupChild);
            }
        }
    } catch (...) {
        Wh_Log(L"ProcessCurrentXamlTree error: %08X", winrt::to_hresult());
    }
}

void RemoveSectionSeparatorsFromCurrentXamlTree();

void InstallXamlTraversal() {
    bool expected = false;
    if (!g_xamlTraversalInstalled.compare_exchange_strong(expected, true)) {
        return;
    }

    try {
        RemoveSectionSeparatorsFromCurrentXamlTree();
        ProcessCurrentXamlTree();

        g_xamlTraversalTimer = wux::DispatcherTimer();
        g_xamlTraversalTimer.Interval(std::chrono::milliseconds(250));
        g_xamlTraversalTimer.Tick(
            [](wf::IInspectable const&, wf::IInspectable const&) {
                ProcessCurrentXamlTree();
            });
        g_xamlTraversalTimer.Start();
    } catch (...) {
        g_xamlTraversalInstalled = false;
        g_xamlTraversalTimer = nullptr;
        Wh_Log(L"InstallXamlTraversal error: %08X", winrt::to_hresult());
    }
}

void StopXamlTimers() {
    if (g_xamlTraversalTimer) {
        g_xamlTraversalTimer.Stop();
        g_xamlTraversalTimer = nullptr;
    }
    if (g_scrollBarHideTimer) {
        g_scrollBarHideTimer.Stop();
        g_scrollBarHideTimer = nullptr;
    }
    if (g_scrollBarFadeTimer) {
        g_scrollBarFadeTimer.Stop();
        g_scrollBarFadeTimer = nullptr;
    }
}

void RestoreFlattenedSources() {
    for (auto& entry : g_flattenedSources) {
        try {
            auto itemsControl = entry.itemsControl.get();
            if (!itemsControl) {
                continue;
            }

            if (entry.originalSource) {
                itemsControl.ItemsSource(entry.originalSource);
            }

            auto groupStyles = itemsControl.GroupStyle();
            groupStyles.Clear();
            for (auto const& groupStyle : entry.originalGroupStyles) {
                groupStyles.Append(groupStyle);
            }
        } catch (...) {
            Wh_Log(L"RestoreFlattenedSources error: %08X", winrt::to_hresult());
        }
    }
}

void RestoreCollapsedElements() {
    for (auto& entry : g_keepCollapsedElements) {
        try {
            auto element = entry.element.get();
            if (!element) {
                continue;
            }

            element.UnregisterPropertyChangedCallback(
                wux::UIElement::VisibilityProperty(), entry.visibilityToken);
            element.UnregisterPropertyChangedCallback(
                wux::FrameworkElement::WidthProperty(), entry.widthToken);
            element.UnregisterPropertyChangedCallback(
                wux::FrameworkElement::HeightProperty(), entry.heightToken);

            RestoreElementState(element, entry.originalState);
        } catch (...) {
            Wh_Log(L"RestoreCollapsedElements error: %08X", winrt::to_hresult());
        }
    }
}

void RestoreHeaderTextElements() {
    for (auto& entry : g_headerTextElements) {
        try {
            auto textBlock = entry.textBlock.get();
            if (!textBlock) {
                continue;
            }

            textBlock.UnregisterPropertyChangedCallback(
                wuc::TextBlock::TextProperty(), entry.textToken);
            textBlock.Text(entry.originalText);
        } catch (...) {
            Wh_Log(L"RestoreHeaderTextElements error: %08X", winrt::to_hresult());
        }
    }
}

void RestoreItemsWrapGrids() {
    for (auto& entry : g_gapAdjustedItemsWrapGrids) {
        try {
            auto itemsWrapGrid = entry.itemsWrapGrid.get();
            if (!itemsWrapGrid) {
                continue;
            }

            itemsWrapGrid.UnregisterPropertyChangedCallback(
                wux::FrameworkElement::MarginProperty(), entry.marginToken);
            itemsWrapGrid.Margin(entry.originalMargin);
            itemsWrapGrid.GroupPadding(entry.originalGroupPadding);
            itemsWrapGrid.AreStickyGroupHeadersEnabled(
                entry.originalAreStickyGroupHeadersEnabled);
            itemsWrapGrid.GroupHeaderPlacement(entry.originalGroupHeaderPlacement);
        } catch (...) {
            Wh_Log(L"RestoreItemsWrapGrids error: %08X", winrt::to_hresult());
        }
    }
}

void RestoreSemanticZooms() {
    for (auto& entry : g_hookedSemanticZooms) {
        try {
            auto semanticZoom = entry.semanticZoom.get();
            if (!semanticZoom) {
                continue;
            }

            semanticZoom.UnregisterPropertyChangedCallback(
                wuc::SemanticZoom::CanChangeViewsProperty(),
                entry.canChangeViewsToken);
            semanticZoom.UnregisterPropertyChangedCallback(
                wuc::SemanticZoom::IsZoomOutButtonEnabledProperty(),
                entry.isZoomOutButtonEnabledToken);
            semanticZoom.UnregisterPropertyChangedCallback(
                wuc::SemanticZoom::IsZoomedInViewActiveProperty(),
                entry.isZoomedInViewActiveToken);

            semanticZoom.CanChangeViews(entry.originalCanChangeViews);
            semanticZoom.IsZoomOutButtonEnabled(
                entry.originalIsZoomOutButtonEnabled);
            semanticZoom.IsZoomedInViewActive(entry.originalIsZoomedInViewActive);
        } catch (...) {
            Wh_Log(L"RestoreSemanticZooms error: %08X", winrt::to_hresult());
        }
    }
}

void RestoreScrollViewerModes() {
    for (auto& entry : g_scrollBarModeScrollViewers) {
        try {
            auto scrollViewer = entry.scrollViewer.get();
            if (!scrollViewer) {
                continue;
            }

            if (entry.hasViewChangedToken) {
                scrollViewer.ViewChanged(entry.viewChangedToken);
            }

            scrollViewer.VerticalScrollBarVisibility(
                entry.originalVerticalScrollBarVisibility);
        } catch (...) {
            Wh_Log(L"RestoreScrollViewerModes error: %08X", winrt::to_hresult());
        }
    }
}

void RestoreScrollBarModes() {
    for (auto& entry : g_scrollBarModeScrollBars) {
        try {
            auto scrollBar = entry.scrollBar.get();
            if (!scrollBar) {
                continue;
            }

            scrollBar.Opacity(entry.originalOpacity);
            scrollBar.IsHitTestVisible(entry.originalIsHitTestVisible);
        } catch (...) {
            Wh_Log(L"RestoreScrollBarModes error: %08X", winrt::to_hresult());
        }
    }
}

bool IsSectionSeparatorElement(wux::UIElement const& element) {
    if (auto frameworkElement = element.try_as<wux::FrameworkElement>()) {
        if (frameworkElement.Name() == kSectionSeparatorName) {
            return true;
        }
    }

    auto rectangle = element.try_as<wuxs::Rectangle>();
    if (!rectangle) {
        return false;
    }

    auto margin = rectangle.Margin();
    return rectangle.Height() == 1 &&
           rectangle.MinHeight() == 1 &&
           !rectangle.IsHitTestVisible() &&
           rectangle.Opacity() == 0.55 &&
           margin.Left == 40 &&
           margin.Right == 40;
}

void RemoveSectionSeparatorsFromSubtree(wux::DependencyObject const& root) {
    if (!root) {
        return;
    }

    std::vector<wux::DependencyObject> stack;
    stack.push_back(root);

    while (!stack.empty()) {
        auto current = stack.back();
        stack.pop_back();

        if (auto panel = current.try_as<wuc::Panel>()) {
            auto children = panel.Children();
            for (int i = static_cast<int>(children.Size()) - 1; i >= 0; i--) {
                auto child = children.GetAt(i);
                if (IsSectionSeparatorElement(child)) {
                    children.RemoveAt(i);
                } else if (auto childObject = child.try_as<wux::DependencyObject>()) {
                    stack.push_back(childObject);
                }
            }

            continue;
        }

        int childCount = wux::Media::VisualTreeHelper::GetChildrenCount(current);
        for (int i = childCount - 1; i >= 0; i--) {
            auto child = wux::Media::VisualTreeHelper::GetChild(current, i);
            if (child) {
                stack.push_back(child);
            }
        }
    }
}

void RemoveSectionSeparatorsFromCurrentXamlTree() {
    try {
        auto window = wux::Window::Current();
        if (!window) {
            return;
        }

        auto root = window.Content().try_as<wux::DependencyObject>();
        RemoveSectionSeparatorsFromSubtree(root);

        auto popups = wux::Media::VisualTreeHelper::GetOpenPopups(window);
        for (uint32_t i = 0; i < popups.Size(); i++) {
            auto popup = popups.GetAt(i);
            if (!popup.IsOpen()) {
                continue;
            }

            if (auto popupChild = popup.Child().try_as<wux::DependencyObject>()) {
                RemoveSectionSeparatorsFromSubtree(popupChild);
            }
        }
    } catch (...) {
        Wh_Log(L"RemoveSectionSeparatorsFromCurrentXamlTree error: %08X",
               winrt::to_hresult());
    }
}

void RemoveTrackedSectionSeparators() {
    for (auto& entry : g_sectionSeparators) {
        try {
            auto parent = entry.parent.get();
            auto separator = entry.separator.get();
            if (!parent || !separator) {
                continue;
            }

            auto children = parent.Children();
            uint32_t index = 0;
            if (children.IndexOf(separator, index)) {
                children.RemoveAt(index);
            }
        } catch (...) {
            Wh_Log(L"RemoveSectionSeparators error: %08X", winrt::to_hresult());
        }
    }
}

void RemoveSectionSeparators() {
    RemoveTrackedSectionSeparators();
    RemoveSectionSeparatorsFromCurrentXamlTree();
}

void ClearXamlState() {
    g_flattenedSources.clear();
    g_hookedSemanticZooms.clear();
    g_gapAdjustedItemsWrapGrids.clear();
    g_keepCollapsedElements.clear();
    g_headerTextElements.clear();
    g_scrollBarModeScrollViewers.clear();
    g_scrollBarModeScrollBars.clear();
    g_sectionSeparators.clear();
}

void UninstallXamlTraversal() {
    try {
        StopXamlTimers();
        RemoveSectionSeparators();
        RestoreScrollViewerModes();
        RestoreScrollBarModes();
        RestoreSemanticZooms();
        RestoreItemsWrapGrids();
        RestoreHeaderTextElements();
        RestoreCollapsedElements();
        RestoreFlattenedSources();
        ClearXamlState();
    } catch (...) {
        Wh_Log(L"UninstallXamlTraversal error: %08X", winrt::to_hresult());
    }

    g_xamlTraversalInstalled = false;
}

using RunFromWindowThreadProc_t = void(WINAPI*)(PVOID parameter);

bool RunFromWindowThread(HWND hWnd, RunFromWindowThreadProc_t proc, PVOID procParam) {
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
        [](int nCode, WPARAM wParam, LPARAM lParam) -> LRESULT {
            if (nCode == HC_ACTION) {
                const CWPSTRUCT* cwp = (const CWPSTRUCT*)lParam;
                if (cwp->message == runFromWindowThreadRegisteredMsg) {
                    auto* param = (RUN_FROM_WINDOW_THREAD_PARAM*)cwp->lParam;
                    param->proc(param->procParam);
                }
            }

            return CallNextHookEx(nullptr, nCode, wParam, lParam);
        },
        nullptr, dwThreadId);
    if (!hook) {
        return false;
    }

    RUN_FROM_WINDOW_THREAD_PARAM param{proc, procParam};
    SendMessage(hWnd, runFromWindowThreadRegisteredMsg, 0, (LPARAM)&param);
    UnhookWindowsHookEx(hook);

    return true;
}

HWND GetCoreWnd() {
    HWND coreWnd = nullptr;
    EnumWindows(
        [](HWND hWnd, LPARAM lParam) -> BOOL {
            DWORD dwProcessId = 0;
            if (!GetWindowThreadProcessId(hWnd, &dwProcessId) ||
                dwProcessId != GetCurrentProcessId()) {
                return TRUE;
            }

            WCHAR className[32];
            if (!GetClassName(hWnd, className, ARRAYSIZE(className))) {
                return TRUE;
            }

            if (_wcsicmp(className, L"Windows.UI.Core.CoreWindow") == 0) {
                *(HWND*)lParam = hWnd;
                return FALSE;
            }

            return TRUE;
        },
        (LPARAM)&coreWnd);

    return coreWnd;
}

void OnWindowCreated(HWND hWnd, LPCWSTR lpClassName) {
    BOOL textualClassName = ((ULONG_PTR)lpClassName & ~(ULONG_PTR)0xffff) != 0;
    if (!textualClassName ||
        _wcsicmp(lpClassName, L"Windows.UI.Core.CoreWindow") != 0) {
        return;
    }

    RunFromWindowThread(hWnd, [](PVOID) { InstallXamlTraversal(); }, nullptr);
}

using CreateWindowInBand_t = HWND(WINAPI*)(DWORD dwExStyle,
                                           LPCWSTR lpClassName,
                                           LPCWSTR lpWindowName,
                                           DWORD dwStyle,
                                           int X,
                                           int Y,
                                           int nWidth,
                                           int nHeight,
                                           HWND hWndParent,
                                           HMENU hMenu,
                                           HINSTANCE hInstance,
                                           PVOID lpParam,
                                           DWORD dwBand);
CreateWindowInBand_t CreateWindowInBand_Original;

HWND WINAPI CreateWindowInBand_Hook(DWORD dwExStyle,
                                    LPCWSTR lpClassName,
                                    LPCWSTR lpWindowName,
                                    DWORD dwStyle,
                                    int X,
                                    int Y,
                                    int nWidth,
                                    int nHeight,
                                    HWND hWndParent,
                                    HMENU hMenu,
                                    HINSTANCE hInstance,
                                    PVOID lpParam,
                                    DWORD dwBand) {
    HWND hWnd = CreateWindowInBand_Original(
        dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight,
        hWndParent, hMenu, hInstance, lpParam, dwBand);
    if (hWnd) {
        OnWindowCreated(hWnd, lpClassName);
    }

    return hWnd;
}

using CreateWindowInBandEx_t = HWND(WINAPI*)(DWORD dwExStyle,
                                             LPCWSTR lpClassName,
                                             LPCWSTR lpWindowName,
                                             DWORD dwStyle,
                                             int X,
                                             int Y,
                                             int nWidth,
                                             int nHeight,
                                             HWND hWndParent,
                                             HMENU hMenu,
                                             HINSTANCE hInstance,
                                             PVOID lpParam,
                                             DWORD dwBand,
                                             DWORD dwTypeFlags);
CreateWindowInBandEx_t CreateWindowInBandEx_Original;

HWND WINAPI CreateWindowInBandEx_Hook(DWORD dwExStyle,
                                      LPCWSTR lpClassName,
                                      LPCWSTR lpWindowName,
                                      DWORD dwStyle,
                                      int X,
                                      int Y,
                                      int nWidth,
                                      int nHeight,
                                      HWND hWndParent,
                                      HMENU hMenu,
                                      HINSTANCE hInstance,
                                      PVOID lpParam,
                                      DWORD dwBand,
                                      DWORD dwTypeFlags) {
    HWND hWnd = CreateWindowInBandEx_Original(
        dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight,
        hWndParent, hMenu, hInstance, lpParam, dwBand, dwTypeFlags);
    if (hWnd) {
        OnWindowCreated(hWnd, lpClassName);
    }

    return hWnd;
}

BOOL Wh_ModInit() {
    LoadSettings();

    HMODULE user32Module =
        LoadLibraryEx(L"user32.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (user32Module) {
        if (void* createWindowInBand =
                (void*)GetProcAddress(user32Module, "CreateWindowInBand")) {
            Wh_SetFunctionHook(createWindowInBand, (void*)CreateWindowInBand_Hook,
                               (void**)&CreateWindowInBand_Original);
        }

        if (void* createWindowInBandEx =
                (void*)GetProcAddress(user32Module, "CreateWindowInBandEx")) {
            Wh_SetFunctionHook(createWindowInBandEx,
                               (void*)CreateWindowInBandEx_Hook,
                               (void**)&CreateWindowInBandEx_Original);
        }
    }

    return TRUE;
}

void Wh_ModAfterInit() {
    HWND coreWnd = GetCoreWnd();
    if (coreWnd) {
        RunFromWindowThread(coreWnd, [](PVOID) { InstallXamlTraversal(); }, nullptr);
    } else {
        InstallXamlTraversal();
    }
}

void Wh_ModUninit() {
    HWND coreWnd = GetCoreWnd();
    if (coreWnd) {
        RunFromWindowThread(coreWnd, [](PVOID) { UninstallXamlTraversal(); },
                            nullptr);
    } else {
        UninstallXamlTraversal();
    }
}

void Wh_ModSettingsChanged() {
    HWND coreWnd = GetCoreWnd();
    if (coreWnd) {
        RunFromWindowThread(
            coreWnd,
            [](PVOID) {
                UninstallXamlTraversal();
                LoadSettings();
                InstallXamlTraversal();
            },
            nullptr);
    } else {
        UninstallXamlTraversal();
        LoadSettings();
        InstallXamlTraversal();
    }
}
