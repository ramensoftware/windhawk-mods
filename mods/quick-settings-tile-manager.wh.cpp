// ==WindhawkMod==
// @id              quick-settings-tile-manager
// @name            Quick Settings Tile Manager
// @description     Hide or show the Quick Settings tiles in the Windows 11 control center, and choose how many tile rows are shown
// @version         1.0.1
// @author          MohamedElnaggar00
// @github          https://github.com/MohamedElnaggar00
// @license         GPL-3.0-only
// @include         ShellHost.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject
// ==/WindhawkMod==

// Source code is published under the GNU General Public License v3.0.
// The visual-tree watcher is adapted from m417z's Windows 11 Notification
// Center Styler and the TranslucentTB project's ExplorerTAP implementation.
// Upstream attribution is retained in the source and the README.

// ==WindhawkModReadme==
/*
# Quick Settings Tile Manager

Hide or show the Quick Settings tiles (Wi-Fi, Bluetooth, Airplane mode,
Energy saver, Accessibility, Live captions, ...) in the Windows 11 control
center flyout, and choose how many rows of tiles are shown at once. Made
for Windows 11 24H2 / 26H2, where the flyout runs in ShellHost.exe.

Tile order is left to Windows: reorder the tiles by holding and dragging
them in the flyout. This mod never changes the order, it only removes the
tiles you turn off and brings them back when you turn them on again.

## How to use

Each known tile has a **Show ...** switch. Turn a switch off to remove that
tile from the flyout, turn it back on to bring it back. The remaining tiles
move up without leaving gaps, and a tile that comes back returns near its
original place.

Tiles that aren't in the list, including tiles added by a Windows update,
are always shown.

Matching is by part of the name, not case-sensitive: "Wi-Fi" matches
"Manage Wi-Fi connections".

## Tile rows

**Tile rows** chooses how many rows of tiles the flyout shows at once: 2
(the Windows default, the layout is left alone), 3, or 4. With 4 rows both
pages of the default layout are combined into one, so every tile is
visible at once. The flyout grows taller to fit the extra rows, and the
sub-pages (such as the Wi-Fi list) grow with it - a side effect of how the
control center sizes itself.

The grid height is measured from the flyout's own layout so the chosen
rows fit exactly, without a clipped row or a leftover scrollable range.
When every shown tile fits in the chosen rows, the tile grid is not
scrollable and the navigation dots next to the tiles are hidden. When the
tiles overflow into extra pages, scrolling and the navigation dots stay
available. Hidden tiles don't shrink the flyout back: with a custom row
count the grid keeps the chosen height.

After changing the settings, close and reopen the Quick Settings flyout. If
nothing changes, restart Explorer (or sign out and back in) so ShellHost.exe
is restarted.

Tip: the mod logs the name of every tile it finds. Open the mod's log in
Windhawk (mod details -> log) after opening the flyout once, and copy the
names from there - this is the reliable way to get the exact names in your
display language.

## Notes and limitations

* Adding tiles that Windows doesn't offer is not possible. This mod can
  only hide/show the tiles that already exist.
* The Quick Settings grid is paginated and virtualized: tiles on pages that
  were never shown have no name the mod can read. On first open (and
  whenever the tile count changes) the mod quickly scrolls through the
  pages to learn every tile's name - this can be visible as a brief
  flicker - so the Show switches apply to the right tiles on every page.
* Don't use this mod together with the **Windows 11 Notification Center
  Styler** mod. Both attach to the same XAML diagnostics interface of
  ShellHost.exe, and only one consumer can use it at a time, so one of the
  two mods will silently stop working. Disable one before enabling the
  other.
* The tile list is modified live in the ShellHost.exe process. Hidden tiles
  are restored when the mod is disabled or unloaded, but if ShellHost.exe
  crashes while tiles are hidden, they stay hidden until Explorer is
  restarted (nothing is written to disk, so a restart always brings
  everything back).
* If the tile list of your build can't be modified directly, the mod falls
  back to hiding tiles via Visibility=Collapsed instead of removing them.
  Check the mod log to see which mode is in effect.
* If the taller layout looks off on your build or display scaling, switch
  Tile rows back to 2.

## License and acknowledgements

Released under the GNU General Public License v3.0 (GPL-3.0-only).

The visual-tree watcher is adapted from
[Windows 11 Notification Center Styler](https://github.com/ramensoftware/windhawk-mods/blob/main/mods/windows-11-notification-center-styler.wh.cpp)
by [m417z](https://github.com/m417z), which is based on
[ExplorerTAP](https://github.com/TranslucentTB/TranslucentTB/tree/develop/ExplorerTAP)
from the **TranslucentTB** project. These projects are distributed under GPLv3.

brought to you by [Instinct](https://instinct.com)
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- show_wifi: true
  $name: Show Wi-Fi
- show_bluetooth: true
  $name: Show Bluetooth
- show_airplane: true
  $name: Show Airplane mode
- show_energysaver: true
  $name: Show Energy saver
- show_livecaptions: true
  $name: Show Live captions
- show_nightlight: true
  $name: Show Night light
- show_hotspot: true
  $name: Show Mobile hotspot
- show_nearbysharing: true
  $name: Show Nearby sharing
- show_cast: true
  $name: Show Cast
- show_project: true
  $name: Show Project
- show_accessibility: true
  $name: Show Accessibility
- rows: "2"
  $name: Tile rows
  $description: How many rows of tiles to show at once. 4 combines both pages into one.
  $options:
  - "2": "2 rows (Windows default)"
  - "3": "3 rows"
  - "4": "4 rows"
*/
// ==/WindhawkModSettings==

// Include classic COM declarations before any C++/WinRT headers.
#include <Unknwn.h>
#include <xamlom.h>

#include <algorithm>
#include <atomic>
#include <chrono>
#include <cmath>
#include <memory>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#undef GetCurrentTime

#include <ocidl.h>
#include <combaseapi.h>

#include <winrt/base.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.System.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Automation.h>
#include <winrt/Windows.UI.Xaml.Automation.Peers.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Media.h>

namespace wf = winrt::Windows::Foundation;
namespace wfc = winrt::Windows::Foundation::Collections;
namespace wux = winrt::Windows::UI::Xaml;

std::atomic<bool> g_initialized;
thread_local bool g_initializedForThread;

HMODULE GetCurrentModuleHandle() {
    HMODULE module;
    if (!GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                               GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                           L"", &module)) {
        return nullptr;
    }

    return module;
}

// The XAML composition diagnostics rebuild a process-wide visual tree walker
// without any locking whenever a DirectComposition visual is added, so any UI
// thread which adds one corrupts the heap while another thread is in the same
// code. Only element mutations are needed here, and those are reported by an
// unrelated code path, so the composition diagnostics are kept from being
// created at all: XamlDiagnostics::CreateCompVisualDiag skips them when the
// HKLM\Software\Microsoft\XAML\Debug\DisableCompositionDiag value is 1.
// Windows.UI.Xaml.dll reads and caches the value once, from within
// AdviseVisualTreeChange, so answering that single read is enough.
// (Mechanism and comments courtesy of m417z's styler mods.)
thread_local bool g_reportCompositionDiagAsDisabled;

////////////////////////////////////////////////////////////////////////////////
// Settings

struct TileRule {
    std::wstring name;
    bool enabled = true;
};

thread_local std::vector<TileRule> g_rules;

// How many rows of tiles to show at once: 2 (the Windows default), 3 or 4.
thread_local int g_rows = 2;

// Whether a tile with the given rule is shown. Tiles that match no rule
// (unknown tiles, or tiles added by a Windows update) are always shown.
bool IsShown(const TileRule* rule) {
    return rule ? rule->enabled : true;
}

void LoadSettings() {
    g_rules.clear();

    static const wchar_t* const kTiles[] = {L"Wi-Fi",L"Bluetooth",L"Airplane mode",L"Energy saver",L"Live captions",L"Night light",L"Mobile hotspot",L"Nearby sharing",L"Cast",L"Project",L"Accessibility"};
    static const wchar_t* const kKeys[] = {L"wifi",L"bluetooth",L"airplane",L"energysaver",L"livecaptions",L"nightlight",L"hotspot",L"nearbysharing",L"cast",L"project",L"accessibility"};
    constexpr int kCount = 11;

    for (int i = 0; i < kCount; i++) {
        std::wstring key = std::wstring(L"show_") + kKeys[i];
        TileRule rule;
        rule.name = kTiles[i];
        rule.enabled = Wh_GetIntSetting(key.c_str()) != 0;
        Wh_Log(L"Setting %s=%d (%s)", key.c_str(),
               rule.enabled ? 1 : 0, rule.enabled ? L"show" : L"hide");
        g_rules.push_back(std::move(rule));
    }

    PCWSTR rowsValue = Wh_GetStringSetting(L"rows");
    g_rows = rowsValue && rowsValue[0] >= L'2' && rowsValue[0] <= L'4'
                 ? rowsValue[0] - L'0'
                 : 2;
    Wh_FreeStringSetting(rowsValue);

    Wh_Log(L"Loaded %zu tile rules, %d tile rows", g_rules.size(), g_rows);
}

std::wstring TrimmedLower(std::wstring_view value) {
    while (!value.empty() &&
           (value.front() == L' ' || value.front() == L'\t')) {
        value.remove_prefix(1);
    }
    while (!value.empty() &&
           (value.back() == L' ' || value.back() == L'\t')) {
        value.remove_suffix(1);
    }

    std::wstring result{value};
    for (auto& ch : result) {
        if (ch >= L'A' && ch <= L'Z') {
            ch += L'a' - L'A';
        }
    }
    return result;
}

// Case-insensitive match: a rule matches when its name is contained in the
// tile name (an exact match always wins, otherwise the longest rule name).
const TileRule* FindRule(std::wstring_view tileName) {
    std::wstring key = TrimmedLower(tileName);
    if (key.empty()) {
        return nullptr;
    }

    const TileRule* best = nullptr;
    size_t bestLen = 0;
    for (const auto& rule : g_rules) {
        std::wstring ruleKey = TrimmedLower(rule.name);
        if (ruleKey.empty()) {
            continue;
        }
        if (ruleKey == key) {
            return &rule;
        }
        if (key.find(ruleKey) != std::wstring::npos &&
            ruleKey.size() > bestLen) {
            best = &rule;
            bestLen = ruleKey.size();
        }
    }
    return best;
}

////////////////////////////////////////////////////////////////////////////////
// Tile layout logic

struct GridState {
    winrt::weak_ref<wux::Controls::GridView> grid;
    bool originalCaptured = false;
    std::vector<wf::IInspectable> originalItems;
    bool mutationUnsupported = false;
    bool applyQueued = false;
    bool applying = false;
    bool loggedNames = false;
    bool loggedSkip = false;
    bool heightOriginalSaved = false;
    double originalHeight = 0;
    double originalActualHeight = 0;
    bool scrollLocked = false;
    wux::Controls::ScrollMode originalVScrollMode =
        wux::Controls::ScrollMode::Auto;
    wux::Controls::ScrollBarVisibility originalVScrollBarVisibility =
        wux::Controls::ScrollBarVisibility::Auto;
    wux::Controls::ZoomMode originalZoomMode = wux::Controls::ZoomMode::Disabled;
    uint32_t realizePassSize = 0;
    std::vector<std::pair<wf::IInspectable, std::wstring>> cachedNames;
};

// Only ever touched on the control center UI thread.
thread_local std::vector<std::unique_ptr<GridState>> g_gridStates;

struct PagerState {
    winrt::weak_ref<wux::FrameworkElement> pager;
    bool collapsedByUs = false;
    wux::Visibility originalVisibility = wux::Visibility::Visible;
    int64_t visibilityToken = 0;
    bool hasVisibilityToken = false;
};

// The page indicator (navigation dots) of the tile grid.
thread_local std::vector<std::unique_ptr<PagerState>> g_pagers;
thread_local bool g_reevaluatingPager = false;

GridState* FindGridState(wux::Controls::GridView const& grid) {
    for (auto& state : g_gridStates) {
        auto existing = state->grid.get();
        if (existing && winrt::get_abi(existing) == winrt::get_abi(grid)) {
            return state.get();
        }
    }
    return nullptr;
}

GridState& GetOrCreateGridState(wux::Controls::GridView const& grid) {
    // Prune dead entries first.
    for (size_t i = g_gridStates.size(); i-- > 0;) {
        if (!g_gridStates[i]->grid.get()) {
            g_gridStates.erase(g_gridStates.begin() + i);
        }
    }

    if (auto* state = FindGridState(grid)) {
        return *state;
    }

    auto state = std::make_unique<GridState>();
    state->grid = winrt::make_weak(grid);
    g_gridStates.push_back(std::move(state));
    return *g_gridStates.back();
}

// Returns the mutable item vector of the grid: the bound items source when
// there is one, otherwise the item collection itself. Returns null when the
// items source doesn't expose a mutable vector.
wfc::IVector<wf::IInspectable> GetMutableItemsVector(
    wux::Controls::GridView const& grid) {
    if (auto itemsSource = grid.ItemsSource()) {
        return itemsSource.try_as<wfc::IVector<wf::IInspectable>>();
    }

    return grid.Items();
}

// The first non-empty automation name in the subtree, or the first non-empty
// TextBlock text when no element has an automation name.
std::wstring FindNameInSubtree(wux::DependencyObject const& root) {
    std::vector<wux::DependencyObject> stack{root};
    std::wstring textFallback;
    size_t visited = 0;

    while (!stack.empty() && visited < 300) {
        auto current = stack.back();
        stack.pop_back();
        visited++;

        if (auto fe = current.try_as<wux::FrameworkElement>()) {
            auto automationName =
                wux::Automation::AutomationProperties::GetName(fe);
            if (!automationName.empty()) {
                return automationName.c_str();
            }

            if (textFallback.empty()) {
                if (auto textBlock =
                        current.try_as<wux::Controls::TextBlock>()) {
                    auto text = textBlock.Text();
                    if (!text.empty()) {
                        textFallback = text.c_str();
                    }
                }
            }
        }

        int childCount = wux::Media::VisualTreeHelper::GetChildrenCount(current);
        for (int i = 0; i < childCount; i++) {
            stack.push_back(
                wux::Media::VisualTreeHelper::GetChild(current, i));
        }
    }

    return textFallback;
}

// Extra name sources for tiles without an automation name: the automation
// peer's name, the tooltip, the item's own Name/Content and its string form.
std::wstring FindExtraName(wux::DependencyObject const& container,
                           wf::IInspectable const& item) {
    try {
        if (auto fe = container.try_as<wux::FrameworkElement>()) {
            auto peer =
                wux::Automation::Peers::FrameworkElementAutomationPeer::
                    FromElement(fe);
            if (peer) {
                auto n = peer.GetName();
                if (!n.empty()) {
                    return n.c_str();
                }
            }
        }
    } catch (...) {
    }

    try {
        if (auto tip = wux::Controls::ToolTipService::GetToolTip(container)) {
            if (auto s = tip.try_as<wf::IReference<winrt::hstring>>()) {
                if (!s.Value().empty()) {
                    return s.Value().c_str();
                }
            }
        }
    } catch (...) {
    }

    try {
        if (auto fe = item.try_as<wux::FrameworkElement>()) {
            auto n = wux::Automation::AutomationProperties::GetName(fe);
            if (!n.empty()) {
                return n.c_str();
            }
            auto subtree = FindNameInSubtree(fe);
            if (!subtree.empty()) {
                return subtree;
            }
        }
    } catch (...) {
    }

    try {
        if (auto str = item.try_as<wf::IStringable>()) {
            auto text = str.ToString();
            // Skip default "Namespace.Class" strings.
            if (!text.empty() && text != winrt::get_class_name(item)) {
                return text.c_str();
            }
        }
    } catch (...) {
    }

    return {};
}

// Logs everything that could identify a tile whose name can't be resolved.
void LogUnresolvedTile(wux::Controls::GridView const& grid, uint32_t index,
                       wf::IInspectable const& item) {
    try {
        Wh_Log(L"Tile %u unresolved: item class=%s", index,
               winrt::get_class_name(item).c_str());
        auto container = grid.ContainerFromItem(item);
        if (!container) {
            Wh_Log(L"Tile %u: no realized container", index);
            return;
        }
        Wh_Log(L"Tile %u: container class=%s", index,
               winrt::get_class_name(container).c_str());
        if (auto fe = container.try_as<wux::FrameworkElement>()) {
            Wh_Log(L"Tile %u: container Name='%s' AutomationId='%s' "
                   L"HelpText='%s' LocalizedControlType='%s' "
                   L"Visibility=%d Size=%.0fx%.0f",
                   index, fe.Name().c_str(),
                   wux::Automation::AutomationProperties::GetAutomationId(fe)
                       .c_str(),
                   wux::Automation::AutomationProperties::GetHelpText(fe)
                       .c_str(),
                   wux::Automation::AutomationProperties::
                       GetLocalizedControlType(fe)
                           .c_str(),
                   (int)fe.Visibility(), fe.ActualWidth(), fe.ActualHeight());
        }
        if (auto cc = container.try_as<wux::Controls::ContentControl>()) {
            if (auto content = cc.Content()) {
                Wh_Log(L"Tile %u: content class=%s", index,
                       winrt::get_class_name(content).c_str());
            }
        }
        // Names of all descendants with a Name or class, first few only.
        std::vector<wux::DependencyObject> stack{container};
        size_t visited = 0;
        while (!stack.empty() && visited < 40) {
            auto cur = stack.back();
            stack.pop_back();
            visited++;
            std::wstring nm;
            if (auto fe = cur.try_as<wux::FrameworkElement>()) {
                nm = fe.Name().c_str();
            }
            Wh_Log(L"Tile %u:   child class=%s name='%s'", index,
                   winrt::get_class_name(cur).c_str(), nm.c_str());
            int n = wux::Media::VisualTreeHelper::GetChildrenCount(cur);
            for (int i = n - 1; i >= 0; i--) {
                stack.push_back(wux::Media::VisualTreeHelper::GetChild(cur, i));
            }
        }
    } catch (...) {
        Wh_Log(L"Tile %u: logging failed", index);
    }
}

// The display name of a tile item, from its identity cache or live container.
// An empty string means neither source could supply a name.
std::wstring ResolveItemName(wux::Controls::GridView const& grid,
                             wf::IInspectable const& item,
                             GridState const* state) {
    // Names belong to item identities, not recycled visual containers.
    // Keep them even after an item is removed from the bound collection.
    if (state) {
        for (const auto& entry : state->cachedNames) {
            if (winrt::get_abi(entry.first) == winrt::get_abi(item)) {
                return entry.second;
            }
        }
    }

    if (auto container = grid.ContainerFromItem(item)) {
        if (auto fe = container.try_as<wux::FrameworkElement>()) {
            auto automationName =
                wux::Automation::AutomationProperties::GetName(fe);
            if (!automationName.empty()) {
                return automationName.c_str();
            }
        }

        auto subtreeName = FindNameInSubtree(container);
        if (!subtreeName.empty()) {
            return subtreeName;
        }

        auto extra = FindExtraName(container, item);
        if (!extra.empty()) {
            return extra;
        }
    }

    return {};
}

void CacheItemName(GridState& state, wf::IInspectable const& item,
                   std::wstring const& name) {
    if (name.empty()) {
        return;
    }
    for (auto& entry : state.cachedNames) {
        if (winrt::get_abi(entry.first) == winrt::get_abi(item)) {
            entry.second = name;
            return;
        }
    }
    state.cachedNames.emplace_back(item, name);
}

bool SameItems(wfc::IVector<wf::IInspectable> const& vec,
               const std::vector<wf::IInspectable>& items) {
    if (vec.Size() != items.size()) {
        return false;
    }

    for (uint32_t i = 0; i < vec.Size(); i++) {
        if (winrt::get_abi(vec.GetAt(i)) != winrt::get_abi(items[i])) {
            return false;
        }
    }
    return true;
}

// Replaces the vector contents with the given items. Returns false when the
// collection refuses to be modified.
bool ReplaceItems(wfc::IVector<wf::IInspectable> const& vec,
                  const std::vector<wf::IInspectable>& items) {
    try {
        vec.ReplaceAll(winrt::array_view<wf::IInspectable const>(
            items.data(), static_cast<uint32_t>(items.size())));
        if (SameItems(vec, items)) {
            Wh_Log(L"ReplaceAll verified: %u items", vec.Size());
            return true;
        }
        Wh_Log(L"ReplaceAll returned but contents differ; trying Clear+Append");
    } catch (winrt::hresult_error const& ex) {
        Wh_Log(L"ReplaceAll failed: %08X, trying Clear+Append", ex.code());
    }

    try {
        vec.Clear();
        for (const auto& item : items) {
            vec.Append(item);
        }
        bool verified = SameItems(vec, items);
        Wh_Log(L"Clear+Append result: %s, %u items",
               verified ? L"verified" : L"contents differ", vec.Size());
        return verified;
    } catch (winrt::hresult_error const& ex) {
        Wh_Log(L"Clear+Append failed: %08X", ex.code());
    }

    return false;
}

bool ContainsItem(const std::vector<wf::IInspectable>& items,
                  wf::IInspectable const& item) {
    for (const auto& existing : items) {
        if (winrt::get_abi(existing) == winrt::get_abi(item)) {
            return true;
        }
    }
    return false;
}

// The position of an item in the captured original list, or UINT32_MAX
// when it isn't part of it.
uint32_t OriginalIndexOf(const std::vector<wf::IInspectable>& originalItems,
                         wf::IInspectable const& item) {
    for (uint32_t i = 0; i < originalItems.size(); i++) {
        if (winrt::get_abi(originalItems[i]) == winrt::get_abi(item)) {
            return i;
        }
    }
    return UINT32_MAX;
}

// Inserts an item after the last item whose original index is lower, or at
// the front when there is none, so a re-added tile comes back where Windows
// originally put it without touching the order of the other tiles.
void InsertAtOriginalPosition(
    const std::vector<wf::IInspectable>& originalItems, uint32_t originalIndex,
    wf::IInspectable const& item, std::vector<wf::IInspectable>& items) {
    size_t pos = 0;
    for (size_t i = 0; i < items.size(); i++) {
        uint32_t other = OriginalIndexOf(originalItems, items[i]);
        if (other != UINT32_MAX && other < originalIndex) {
            pos = i + 1;
        }
    }
    items.insert(items.begin() + pos, item);
}

// The first ScrollViewer found in the subtree, depth-first.
wux::Controls::ScrollViewer FindScrollViewer(
    wux::DependencyObject const& root) {
    std::vector<wux::DependencyObject> stack{root};
    size_t visited = 0;

    while (!stack.empty() && visited < 300) {
        auto current = stack.back();
        stack.pop_back();
        visited++;

        if (auto scrollViewer =
                current.try_as<wux::Controls::ScrollViewer>()) {
            return scrollViewer;
        }

        int childCount =
            wux::Media::VisualTreeHelper::GetChildrenCount(current);
        for (int i = childCount - 1; i >= 0; i--) {
            stack.push_back(
                wux::Media::VisualTreeHelper::GetChild(current, i));
        }
    }

    return nullptr;
}

// The paginated grid virtualizes its tiles: tiles on pages beyond the
// visible one have no realized container, and without a container a tile's
// name can't be resolved, so the Show switches can't be matched against it.
// Scroll through the grid and capture the name of each tile while its
// container is realized, then restore the scroll position.
void RealizeAllAndCaptureNames(wux::Controls::GridView const& grid,
                               wfc::IVector<wf::IInspectable> const& vec,
                               GridState& state) {
    uint32_t size = vec.Size();

    double savedOffset = 0;
    auto scrollViewer = FindScrollViewer(grid);
    if (scrollViewer) {
        savedOffset = scrollViewer.VerticalOffset();
    }

    for (uint32_t i = 0; i < size; i++) {
        try {
            if (!grid.ContainerFromIndex(i)) {
                grid.ScrollIntoView(vec.GetAt(i));
                grid.UpdateLayout();
            }

            auto item = vec.GetAt(i);
            std::wstring name = ResolveItemName(grid, item, nullptr);
            if (name.empty()) {
                Wh_Log(L"Realize pass: tile %u still has no name", i);
                LogUnresolvedTile(grid, i, item);
                continue;
            }

            CacheItemName(state, item, name);
        } catch (winrt::hresult_error const& ex) {
            Wh_Log(L"Realize pass failed at tile %u: %08X", i, ex.code());
            break;
        }
    }

    // Restore the scroll position so the pass stays invisible.
    try {
        if (scrollViewer) {
            scrollViewer.ChangeView(nullptr, savedOffset, nullptr, true);
        } else if (size > 0) {
            grid.ScrollIntoView(vec.GetAt(0));
        }
    } catch (...) {
    }
}

// The control center has a second RootGridView with the sliders (Brightness,
// sound output). Only treat a grid as the tile grid when at least one of its
// items resolves to a name matching a configured rule.
bool IsTileGrid(wux::Controls::GridView const& grid,
                wfc::IVector<wf::IInspectable> const& vec,
                GridState const& state) {
    uint32_t size = vec.Size();
    for (uint32_t i = 0; i < size; i++) {
        auto name = ResolveItemName(grid, vec.GetAt(i), &state);
        if (!name.empty() && FindRule(name)) {
            return true;
        }
    }
    return false;
}

// Last-resort grid heights per row count, as reported by the community
// (r/Windhawk). Used only when neither the realized tile containers nor
// the captured original height can supply a row height.
double HeightForRows(int rows) {
    switch (rows) {
        case 3:
            return 276;
        case 4:
            return 384;
        default:
            return 192;
    }
}

struct RowMetrics {
    bool valid = false;
    double pitch = 0;  // distance between the tops of two neighboring rows
    double top = 0;    // top offset of the first row inside the grid
};

// Measures the vertical row pitch from the realized tile containers.
RowMetrics MeasureRowMetrics(wux::Controls::GridView const& grid) {
    RowMetrics metrics;
    try {
        std::vector<double> tops;
        uint32_t size = grid.Items().Size();
        for (uint32_t i = 0; i < size; i++) {
            auto container = grid.ContainerFromIndex(i)
                                 .try_as<wux::FrameworkElement>();
            if (!container || container.ActualHeight() <= 0) {
                continue;
            }
            auto transform = container.TransformToVisual(grid);
            auto point = transform.TransformPoint(wf::Point(0, 0));
            tops.push_back(point.Y);
        }
        if (tops.empty()) {
            return metrics;
        }
        std::sort(tops.begin(), tops.end());
        std::vector<double> rowTops;
        for (double y : tops) {
            if (rowTops.empty() || y - rowTops.back() > 4) {
                rowTops.push_back(y);
            }
        }
        metrics.top = rowTops.front();
        if (rowTops.size() >= 2) {
            metrics.pitch = rowTops[1] - rowTops[0];
            metrics.valid = metrics.pitch > 0;
        }
    } catch (...) {
    }
    return metrics;
}

// The grid height which fits the given row count exactly. Prefers the
// measured row pitch, then the flyout's own original two-row height, and
// only then the community constants. A small allowance guards against
// sub-pixel rounding turning into a scrollable range.
double ComputeRowsHeight(wux::Controls::GridView const& grid,
                         GridState const& state, int rows, bool verbose) {
    auto metrics = MeasureRowMetrics(grid);
    if (metrics.valid) {
        double height = metrics.top + metrics.pitch * rows + 0.5;
        if (verbose) {
            Wh_Log(L"Row height: measured pitch %.1f + top %.1f -> %.1f for "
                   L"%d rows", metrics.pitch, metrics.top, height, rows);
        }
        return height;
    }

    double twoRowHeight = state.originalHeight;
    if (std::isnan(twoRowHeight) || twoRowHeight <= 0) {
        twoRowHeight = state.originalActualHeight;
    }
    if (!std::isnan(twoRowHeight) && twoRowHeight > 0) {
        double height = twoRowHeight / 2.0 * rows + 0.5;
        if (verbose) {
            Wh_Log(L"Row height: %.1f from the original two-row height %.1f",
                   height, twoRowHeight);
        }
        return height;
    }

    double height = HeightForRows(rows);
    if (verbose) {
        Wh_Log(L"Row height: %.1f from the built-in fallback (no "
               L"measurement available)", height);
    }
    return height;
}

// Applies the configured row count to the tile grid by resizing it; the
// flyout grows to fit. The original height is saved on the first change so
// it can be restored.
void ApplyRowHeight(wux::Controls::GridView const& grid, GridState& state) {
    try {
        if (!state.heightOriginalSaved) {
            state.originalHeight = grid.Height();
            state.originalActualHeight = grid.ActualHeight();
            state.heightOriginalSaved = true;
        }

        // Make sure the current containers are laid out before measuring.
        grid.UpdateLayout();

        double height = ComputeRowsHeight(grid, state, g_rows, false);
        if (grid.Height() != height) {
            ComputeRowsHeight(grid, state, g_rows, true);
            Wh_Log(L"Setting the tile grid height to %.1f (%d rows)", height,
                   g_rows);
            grid.Height(height);
        }
    } catch (winrt::hresult_error const& ex) {
        Wh_Log(L"Error %08X: %s", ex.code(), ex.message().c_str());
    } catch (...) {
        Wh_Log(L"Error %08X", winrt::to_hresult());
    }
}

void RestoreRowHeight(wux::Controls::GridView const& grid,
                      GridState& state) {
    if (!state.heightOriginalSaved) {
        return;
    }

    try {
        grid.Height(state.originalHeight);
    } catch (winrt::hresult_error const& ex) {
        Wh_Log(L"Error %08X: %s", ex.code(), ex.message().c_str());
    } catch (...) {
        Wh_Log(L"Error %08X", winrt::to_hresult());
    }

    state.heightOriginalSaved = false;
}

// Disables vertical scrolling on the tile grid's inner ScrollViewer, or
// brings it back. Locked when every shown tile fits on one page: with an
// exactly-fitted grid there is nothing to scroll to, and a leftover
// scrollable range shows up as the grid jumping on mouse move or click.
// The original values are saved on the first lock and restored on unlock.
void SetScrollLock(wux::Controls::GridView const& grid, GridState& state,
                   bool lock) {
    try {
        auto scrollViewer = FindScrollViewer(grid);
        if (!scrollViewer) {
            Wh_Log(L"Scroll lock: no ScrollViewer found in the tile grid");
            return;
        }

        if (lock) {
            bool wasLocked = state.scrollLocked;
            if (!state.scrollLocked) {
                state.originalVScrollMode = scrollViewer.VerticalScrollMode();
                state.originalVScrollBarVisibility =
                    scrollViewer.VerticalScrollBarVisibility();
                state.originalZoomMode = scrollViewer.ZoomMode();
                state.scrollLocked = true;
                Wh_Log(L"Locking tile grid scrolling: every shown tile fits "
                       L"one page (was VerticalScrollMode=%d, "
                       L"VerticalScrollBarVisibility=%d, ZoomMode=%d)",
                       (int)state.originalVScrollMode,
                       (int)state.originalVScrollBarVisibility,
                       (int)state.originalZoomMode);
            }
            scrollViewer.VerticalScrollMode(
                wux::Controls::ScrollMode::Disabled);
            scrollViewer.VerticalScrollBarVisibility(
                wux::Controls::ScrollBarVisibility::Disabled);
            scrollViewer.ZoomMode(wux::Controls::ZoomMode::Disabled);
            if (scrollViewer.VerticalOffset() != 0) {
                Wh_Log(L"Scroll lock: resetting a leftover vertical offset "
                       L"of %.1f", scrollViewer.VerticalOffset());
                scrollViewer.ChangeView(nullptr, 0.0, nullptr, true);
            }
            if (!wasLocked) {
                Wh_Log(L"Scroll lock: VerticalScrollMode=%d, "
                       L"VerticalScrollBarVisibility=%d, VerticalOffset=%.1f",
                       (int)scrollViewer.VerticalScrollMode(),
                       (int)scrollViewer.VerticalScrollBarVisibility(),
                       scrollViewer.VerticalOffset());
            }
        } else if (state.scrollLocked) {
            scrollViewer.VerticalScrollMode(state.originalVScrollMode);
            scrollViewer.VerticalScrollBarVisibility(
                state.originalVScrollBarVisibility);
            scrollViewer.ZoomMode(state.originalZoomMode);
            state.scrollLocked = false;
            Wh_Log(L"Tile grid scrolling restored");
        }
    } catch (winrt::hresult_error const& ex) {
        Wh_Log(L"Error %08X: %s", ex.code(), ex.message().c_str());
    } catch (...) {
        Wh_Log(L"Error %08X", winrt::to_hresult());
    }
}

// The number of tile columns, measured from the realized tiles, or 0 when
// it can't be measured yet.
int MeasureGridColumns(wux::Controls::GridView const& grid) {
    double gridWidth = grid.ActualWidth();
    if (gridWidth <= 0) {
        return 0;
    }

    double itemWidth = 0;
    uint32_t size = grid.Items().Size();
    for (uint32_t i = 0; i < size; i++) {
        auto container = grid.ContainerFromIndex(i)
                             .try_as<wux::FrameworkElement>();
        if (container && container.ActualWidth() > 0) {
            itemWidth = container.ActualWidth();
            break;
        }
    }
    if (itemWidth <= 0) {
        return 0;
    }

    int columns = static_cast<int>(gridWidth / itemWidth + 0.01);
    return columns < 1 ? 1 : columns;
}

// Whether every shown tile fits on a single page with the configured row
// count. Scrolling is only needed when they don't.
bool FitsOnePage(wux::Controls::GridView const& grid, uint32_t shownTiles) {
    int columns = MeasureGridColumns(grid);
    if (columns < 1) {
        return false;
    }
    return shownTiles <=
           static_cast<uint32_t>(columns) * static_cast<uint32_t>(g_rows);
}

// Hides the page indicator when every shown tile fits on one page with the
// configured row count, and brings it back when there's more than one page
// again. The column count is measured from the realized tiles.
void UpdateQuickActionsPager(wux::Controls::GridView const& grid,
                             uint32_t shownTiles) {
    try {
        for (size_t i = g_pagers.size(); i-- > 0;) {
            if (!g_pagers[i]->pager.get()) {
                g_pagers.erase(g_pagers.begin() + i);
            }
        }
        if (g_pagers.empty()) {
            return;
        }

        if (g_rows == 2) {
            // The default layout leaves the indicator to Windows.
            for (auto& pagerState : g_pagers) {
                auto pager = pagerState->pager.get();
                if (pager && pagerState->collapsedByUs) {
                    pager.Visibility(pagerState->originalVisibility);
                    pagerState->collapsedByUs = false;
                }
            }
            return;
        }

        int columns = MeasureGridColumns(grid);
        if (columns < 1) {
            return;
        }

        uint32_t perPage = static_cast<uint32_t>(columns) *
                           static_cast<uint32_t>(g_rows);
        bool singlePage = shownTiles <= perPage;

        for (auto& pagerState : g_pagers) {
            auto pager = pagerState->pager.get();
            if (!pager) {
                continue;
            }

            if (singlePage) {
                if (pagerState->collapsedByUs &&
                    pager.Visibility() != wux::Visibility::Collapsed) {
                    // Windows made the indicator visible again (for example
                    // after dragging tiles); collapse it once more.
                    Wh_Log(L"Page indicator was re-shown by Windows, hiding "
                           L"again: shown=%u, capacity=%u", shownTiles,
                           perPage);
                    pagerState->originalVisibility = pager.Visibility();
                    pager.Visibility(wux::Visibility::Collapsed);
                } else if (!pagerState->collapsedByUs &&
                    pager.Visibility() != wux::Visibility::Collapsed) {
                    pagerState->originalVisibility = pager.Visibility();
                    Wh_Log(L"Hiding page indicator: shown=%u, columns=%d, "
                           L"rows=%d, capacity=%u", shownTiles, columns,
                           g_rows, perPage);
                    pager.Visibility(wux::Visibility::Collapsed);
                    pagerState->collapsedByUs = true;
                }
            } else if (pagerState->collapsedByUs) {
                Wh_Log(L"Showing page indicator: shown=%u, columns=%d, "
                       L"rows=%d, capacity=%u", shownTiles, columns,
                       g_rows, perPage);
                pager.Visibility(pagerState->originalVisibility);
                pagerState->collapsedByUs = false;
            }
        }
    } catch (winrt::hresult_error const& ex) {
        Wh_Log(L"Error %08X: %s", ex.code(), ex.message().c_str());
    } catch (...) {
        Wh_Log(L"Error %08X", winrt::to_hresult());
    }
}

void RestoreAllPagers() {
    g_reevaluatingPager = true;
    struct Reset { ~Reset() { g_reevaluatingPager = false; } } reset;
    for (auto& pagerState : g_pagers) {
        auto pager = pagerState->pager.get();
        if (!pager || !pagerState->collapsedByUs) {
            continue;
        }

        try {
            pager.Visibility(pagerState->originalVisibility);
        } catch (...) {
        }
        pagerState->collapsedByUs = false;
    }
}

void ScheduleApply(wux::Controls::GridView const& grid);

void OnQuickActionsPagerAdded(wux::FrameworkElement const& element) {
    Wh_Log(L"Quick actions page indicator added");

    auto pagerState = std::make_unique<PagerState>();
    pagerState->pager = winrt::make_weak(element);
    try {
        winrt::weak_ref<wux::FrameworkElement> weakPager =
            winrt::make_weak(element);
        pagerState->visibilityToken = element.RegisterPropertyChangedCallback(
            wux::UIElement::VisibilityProperty(),
            [weakPager](wux::DependencyObject const&,
                        wux::DependencyProperty const&) {
                try {
                    auto pager = weakPager.get();
                    if (!pager || g_reevaluatingPager ||
                        pager.Visibility() == wux::Visibility::Collapsed) {
                        return;
                    }
                    for (auto& gridState : g_gridStates) {
                        auto grid = gridState->grid.get();
                        if (!grid || !gridState->originalCaptured ||
                            gridState->mutationUnsupported || g_rows == 2 ||
                            gridState->applying) {
                            continue;
                        }
                        g_reevaluatingPager = true;
                        Wh_Log(L"Page indicator visibility changed to "
                               L"visible, re-evaluating");
                        UpdateQuickActionsPager(grid, grid.Items().Size());
                        g_reevaluatingPager = false;
                        break;
                    }
                } catch (...) {
                    g_reevaluatingPager = false;
                }
            });
        pagerState->hasVisibilityToken = true;
    } catch (...) {
        Wh_Log(L"Could not watch page indicator visibility");
    }
    g_pagers.push_back(std::move(pagerState));

    // Evaluate against the tile grid when it's already known.
    for (auto& gridState : g_gridStates) {
        auto grid = gridState->grid.get();
        if (!grid || !gridState->originalCaptured) {
            continue;
        }

        // Let ApplyTiles finish the collection update before counting pages.
        ScheduleApply(grid);
        break;
    }
}

// Fallback cannot guarantee gap-free packing, so leave paging to Windows.
void ApplyCollapsedTiles(wux::Controls::GridView const& grid,
                         GridState& state) {
    uint32_t collapsed = 0;
    uint32_t matched = 0;
    auto items = grid.Items();
    for (uint32_t i = 0; i < items.Size(); i++) {
        auto item = items.GetAt(i);
        auto name = ResolveItemName(grid, item, &state);
        CacheItemName(state, item, name);
        const TileRule* rule = FindRule(name);
        if (!rule) {
            continue;
        }
        matched++;
        auto container = grid.ContainerFromIndex(i)
                             .try_as<wux::FrameworkElement>();
        if (!container) {
            Wh_Log(L"Collapse deferred: '%s' has no container", name.c_str());
            continue;
        }
        bool shown = IsShown(rule);
        auto visibility = shown ? wux::Visibility::Visible
                                : wux::Visibility::Collapsed;
        container.Visibility(visibility);
        bool verified = container.Visibility() == visibility;
        Wh_Log(L"Collapse fallback: '%s' -> %s (%s)", name.c_str(),
               shown ? L"show" : L"hide", verified ? L"verified" : L"failed");
        if (!shown && verified) {
            collapsed++;
        }
    }
    if (matched && g_rows != 2) {
        ApplyRowHeight(grid, state);
        SetScrollLock(grid, state,
                      FitsOnePage(grid, items.Size() - collapsed));
    }
    Wh_Log(L"Collapse fallback complete: %u matched, %u collapsed", matched,
           collapsed);
    if (matched) {
        RestoreAllPagers();
    }
}

// Applies the configured hide/show to a grid. Safe to call any number of
// times: does nothing when no tile has to be removed or brought back.
void ApplyTiles(wux::Controls::GridView const& grid) {
    try {
        GridState& state = GetOrCreateGridState(grid);
        if (state.applying) {
            Wh_Log(L"Reentrant apply skipped while a pass is running");
            return;
        }
        struct ApplyGuard {
            bool& flag;
            explicit ApplyGuard(bool& value) : flag(value) { flag = true; }
            ~ApplyGuard() { flag = false; }
        } guard(state.applying);
        if (state.mutationUnsupported) {
            ApplyCollapsedTiles(grid, state);
            return;
        }

        auto vec = GetMutableItemsVector(grid);
        if (!vec) {
            state.mutationUnsupported = true;
            Wh_Log(L"Items source isn't a mutable vector, tile removal is "
                   L"unsupported; falling back to collapsing tiles");
            ApplyCollapsedTiles(grid, state);
            return;
        }

        uint32_t size = vec.Size();
        if (size == 0) {
            return;
        }

        if (!state.originalCaptured && !IsTileGrid(grid, vec, state)) {
            if (!state.loggedSkip) {
                state.loggedSkip = true;
                Wh_Log(L"Grid with %u items has no tile matching a rule; "
                       L"leaving it alone (sliders grid?)", size);
            }
            return;
        }

        // Capture EVERY currently resolvable name before scrolling or removal.
        // Without this, an all-realized first pass removes tiles with no cache;
        // on the next pass those removed items have no container/name and the
        // restore loop mistakes them for unknown (always shown) items.
        for (uint32_t i = 0; i < size; i++) {
            auto item = vec.GetAt(i);
            CacheItemName(state, item, ResolveItemName(grid, item, &state));
        }

        // A custom row count is realized by resizing the tile grid.
        if (g_rows != 2) {
            ApplyRowHeight(grid, state);
        }

        // The realize pass below may need to scroll through the pages.
        SetScrollLock(grid, state, false);

        // Tiles beyond the visible page may not have a realized container
        // yet, so their names - and with that whether they are hidden -
        // can't be resolved. Scroll through the grid once per size change
        // and capture all names, so the Show switches apply to the right
        // tiles on every page.
        if (state.realizePassSize != size) {
            bool anyUnrealized = false;
            for (uint32_t i = 0; i < size; i++) {
                if (!grid.ContainerFromIndex(i)) {
                    anyUnrealized = true;
                    break;
                }
            }

            if (anyUnrealized) {
                state.realizePassSize = size;
                Wh_Log(L"Realizing tile containers across all pages");
                RealizeAllAndCaptureNames(grid, vec, state);
                Wh_Log(L"Realize pass complete: %zu cached names",
                       state.cachedNames.size());
            }
        }

        // The full tile set is captured once, before the first mutation,
        // and is the baseline for bringing removed tiles back.
        if (!state.originalCaptured) {
            for (uint32_t i = 0; i < size; i++) {
                state.originalItems.push_back(vec.GetAt(i));
            }
            state.originalCaptured = true;
        }

        // Tiles added later (e.g. via the flyout's own edit mode) join the
        // anchor list at the end, so they can be brought back too.
        for (uint32_t i = 0; i < size; i++) {
            auto item = vec.GetAt(i);
            if (!ContainsItem(state.originalItems, item)) {
                Wh_Log(L"A new tile appeared, anchoring it at the end");
                state.originalItems.push_back(item);
            }
        }

        std::wstring loggedNames;
        for (uint32_t i = 0; i < size; i++) {
            auto name = ResolveItemName(grid, vec.GetAt(i), &state);
            CacheItemName(state, vec.GetAt(i), name);
            if (!name.empty()) {
                if (!loggedNames.empty()) {
                    loggedNames += L", ";
                }
                loggedNames += name;
            } else {
                Wh_Log(L"Tile %u: name couldn't be resolved (container not "
                       L"realized yet?)", i);
                LogUnresolvedTile(grid, i, vec.GetAt(i));
            }
        }

        if (!state.loggedNames && !loggedNames.empty()) {
            Wh_Log(L"Found tiles: %s", loggedNames.c_str());
            state.loggedNames = true;
        }

        // The current order is never touched: shown tiles stay exactly
        // where they are (including the user's own drag-arrangement), only
        // hidden tiles drop out.
        std::vector<wf::IInspectable> desired;
        desired.reserve(size);
        for (uint32_t i = 0; i < size; i++) {
            auto item = vec.GetAt(i);
            auto name = ResolveItemName(grid, item, &state);
            const TileRule* rule = FindRule(name);
            bool shown = IsShown(rule);
            Wh_Log(L"Tile decision: '%s', rule='%s', %s", name.c_str(),
                   rule ? rule->name.c_str() : L"(unknown)",
                   shown ? L"show" : L"HIDE");
            if (shown) {
                desired.push_back(item);
            }
        }

        // Tiles removed earlier that are shown again come back at their
        // original relative position.
        for (uint32_t i = 0;
             i < static_cast<uint32_t>(state.originalItems.size()); i++) {
            const auto& item = state.originalItems[i];
            if (ContainsItem(desired, item)) {
                continue;
            }
            auto name = ResolveItemName(grid, item, &state);
            const TileRule* rule = FindRule(name);
            if (!IsShown(rule)) {
                Wh_Log(L"Keep removed: '%s' (cached hide rule)", name.c_str());
                continue;
            }
            // Every removed item should have a cached name, since only a
            // named match can be hidden. Never resurrect an unnamed anchor.
            if (name.empty()) {
                Wh_Log(L"Restore deferred: anchor %u has no cached name", i);
                continue;
            }
            Wh_Log(L"Restoring tile: '%s'", name.c_str());
            InsertAtOriginalPosition(state.originalItems, i, item, desired);
        }

        Wh_Log(L"Apply plan: current=%u, desired=%zu, cached=%zu", size,
               desired.size(), state.cachedNames.size());
        if (SameItems(vec, desired)) {
            Wh_Log(L"No collection change: already matches the tile settings");
            UpdateQuickActionsPager(grid, vec.Size());
            SetScrollLock(grid, state,
                          g_rows != 2 && FitsOnePage(grid, vec.Size()));
            return;
        }

        double savedOffset = 0;
        auto scrollViewer = FindScrollViewer(grid);
        if (scrollViewer) {
            savedOffset = scrollViewer.VerticalOffset();
        }

        Wh_Log(L"Applying tile list: %u -> %zu items", size, desired.size());
        if (!ReplaceItems(vec, desired)) {
            state.mutationUnsupported = true;
            Wh_Log(L"Modifying the tile list failed, tile removal is "
                   L"unsupported; falling back to collapsing tiles");
            // Clear+Append can fail after a partial write. Try restoring the
            // baseline before applying container visibility; log any failure.
            if (!ReplaceItems(vec, state.originalItems)) {
                Wh_Log(L"WARNING: could not restore baseline after failed "
                       L"mutation; restart ShellHost to recover all tiles");
            }
            ApplyCollapsedTiles(grid, state);
            return;
        }
        Wh_Log(L"Applied tile list verified: %u -> %u items", size, vec.Size());
        UpdateQuickActionsPager(grid, vec.Size());
        if (scrollViewer) {
            // Replacing the items can reset the scroll position; keep the
            // page the user is looking at.
            try {
                scrollViewer.ChangeView(nullptr, savedOffset, nullptr, true);
            } catch (...) {
            }
        }
        SetScrollLock(grid, state,
                      g_rows != 2 && FitsOnePage(grid, vec.Size()));
    } catch (winrt::hresult_error const& ex) {
        Wh_Log(L"Error %08X: %s", ex.code(), ex.message().c_str());
    } catch (...) {
        Wh_Log(L"Error %08X", winrt::to_hresult());
    }
}

// Makes every realized container visible again. Used to undo the
// collapse-fallback when removed tiles are restored.
void RestoreContainerVisibility(wux::Controls::GridView const& grid) {
    try {
        auto vec = GetMutableItemsVector(grid);
        uint32_t size = vec ? vec.Size() : grid.Items().Size();

        for (uint32_t i = 0; i < size; i++) {
            auto container = grid.ContainerFromIndex(i)
                                 .try_as<wux::Controls::GridViewItem>();
            if (container) {
                container.Visibility(wux::Visibility::Visible);
            }
        }
    } catch (winrt::hresult_error const& ex) {
        Wh_Log(L"Error %08X: %s", ex.code(), ex.message().c_str());
    }
}

// Puts removed tiles back at their original relative position for all
// known grids, without changing the order of the tiles that stayed.
void RestoreAllGrids() {
    for (auto& state : g_gridStates) {
        auto grid = state->grid.get();
        if (!grid) {
            continue;
        }

        SetScrollLock(grid, *state, false);
        RestoreRowHeight(grid, *state);

        if (!state->originalCaptured) {
            if (state->mutationUnsupported) {
                RestoreContainerVisibility(grid);
            }
            continue;
        }

        try {
            auto vec = GetMutableItemsVector(grid);
            if (vec) {
                std::vector<wf::IInspectable> desired;
                uint32_t size = vec.Size();
                desired.reserve(state->originalItems.size() + size);
                for (uint32_t i = 0; i < size; i++) {
                    desired.push_back(vec.GetAt(i));
                }

                for (uint32_t i = 0;
                     i < static_cast<uint32_t>(state->originalItems.size());
                     i++) {
                    const auto& item = state->originalItems[i];
                    if (!ContainsItem(desired, item)) {
                        InsertAtOriginalPosition(state->originalItems, i,
                                                 item, desired);
                    }
                }

                if (!SameItems(vec, desired)) {
                    ReplaceItems(vec, desired);
                }
            }

            // Undo any collapse-fallback hiding.
            RestoreContainerVisibility(grid);
        } catch (winrt::hresult_error const& ex) {
            Wh_Log(L"Error %08X: %s", ex.code(), ex.message().c_str());
        }
    }
}

void ScheduleApply(wux::Controls::GridView const& grid) {
    try {
        GridState& state = GetOrCreateGridState(grid);
        if (state.applyQueued) {
            return;
        }
        winrt::weak_ref<wux::Controls::GridView> weakGrid =
            winrt::make_weak(grid);
        grid.Dispatcher().RunAsync(
            winrt::Windows::UI::Core::CoreDispatcherPriority::Normal,
            [weakGrid]() {
                auto grid = weakGrid.get();
                if (!grid) {
                    return;
                }

                if (auto* state = FindGridState(grid)) {
                    state->applyQueued = false;
                }

                ApplyTiles(grid);
            });
        state.applyQueued = true;
    } catch (winrt::hresult_error const& ex) {
        Wh_Log(L"Error %08X: %s", ex.code(), ex.message().c_str());
    }
}

void OnGridViewItemAdded(wux::Controls::GridViewItem const& item) {
    try {
        winrt::weak_ref<wux::Controls::GridViewItem> weakItem =
            winrt::make_weak(item);
        item.Dispatcher().RunAsync(
            winrt::Windows::UI::Core::CoreDispatcherPriority::Normal,
            [weakItem]() {
                auto item = weakItem.get();
                if (!item) {
                    return;
                }

                // Find the owning grid. The item may not have been attached at the
                // time of the Add report, hence the deferred lookup.
                wux::Controls::GridView grid{nullptr};
                auto parent = wux::Media::VisualTreeHelper::GetParent(item);
                while (parent) {
                    grid = parent.try_as<wux::Controls::GridView>();
                    if (grid) {
                        break;
                    }
                    parent = wux::Media::VisualTreeHelper::GetParent(parent);
                }

                if (!grid || grid.Name() != L"RootGridView") {
                    return;
                }

                GridState& state = GetOrCreateGridState(grid);

                // In collapse-fallback mode, apply visibility to the container.
                if (state.mutationUnsupported) {
                    ApplyCollapsedTiles(grid, state);
                    return;
                }

                // New containers can mean new items; re-apply (a no-op when the
                // order already matches).
                ScheduleApply(grid);
            });
    } catch (winrt::hresult_error const& ex) {
        Wh_Log(L"Error %08X: %s", ex.code(), ex.message().c_str());
    }
}

void HandleElementAdded(wux::FrameworkElement const& element) {
    if (auto grid = element.try_as<wux::Controls::GridView>()) {
        Wh_Log(L"GridView added, name: %s", grid.Name().c_str());
        if (grid.Name() == L"RootGridView") {
            ScheduleApply(grid);
        }
    } else if (auto item = element.try_as<wux::Controls::GridViewItem>()) {
        OnGridViewItemAdded(item);
    } else if (element.Name() == L"QuickActionsPager" &&
               winrt::get_class_name(element) ==
                   winrt::hstring{L"Microsoft.UI.Xaml.Controls.PipsPager"}) {
        OnQuickActionsPagerAdded(element);
    }
}

////////////////////////////////////////////////////////////////////////////////
// Visual tree watcher, based on the ExplorerTAP code from the TranslucentTB
// project, as adapted in m417z's Windows 11 Notification Center Styler mod.

// XamlDiagnostics implements this interface too, and xamlom.h does not declare
// it. UnregisterInstance closes the runtime object cached for a handle, the
// only reference the diagnostics keep to an element once it was reported.
static constexpr GUID IID_IXamlDiagnosticsTestHooks =
    {0x735941a2, 0x3ee3, 0x495a, {0x8d, 0xa9, 0x97, 0x26, 0x27, 0x00, 0x30, 0x75}};

// This private diagnostics interface is absent from xamlom.h. Its vtable
// must match XAML exactly; keep both methods in the upstream order.
struct IXamlDiagnosticsTestHooks : IUnknown {
    virtual HRESULT STDMETHODCALLTYPE UnregisterInstance(InstanceHandle handle) = 0;
    virtual HRESULT STDMETHODCALLTYPE TryGetDispatcherQueueForObject(
        InstanceHandle handle, void** dispatcherQueue) = 0;
};

// Implement this classic COM callback directly. This avoids depending on
// C++/WinRT's classic-COM producer selection for the SDK's xamlom.h types.
// Do not add a second IVisualTreeServiceCallback2 base to winrt::implements:
// that would create two competing callback subobjects.
class VisualTreeWatcher final : public IVisualTreeServiceCallback2
{
public:
    VisualTreeWatcher(winrt::com_ptr<IUnknown> site);

    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** object) noexcept override {
        if (!object) {
            return E_POINTER;
        }
        *object = nullptr;
        if (IsEqualIID(riid, __uuidof(IUnknown)) ||
            IsEqualIID(riid, __uuidof(IVisualTreeServiceCallback)) ||
            IsEqualIID(riid, __uuidof(IVisualTreeServiceCallback2))) {
            *object = static_cast<IVisualTreeServiceCallback2*>(this);
            AddRef();
            return S_OK;
        }
        return E_NOINTERFACE;
    }

    ULONG STDMETHODCALLTYPE AddRef() noexcept override {
        return m_referenceCount.fetch_add(1, std::memory_order_relaxed) + 1;
    }

    ULONG STDMETHODCALLTYPE Release() noexcept override {
        ULONG remaining =
            m_referenceCount.fetch_sub(1, std::memory_order_acq_rel) - 1;
        if (!remaining) {
            delete this;
        }
        return remaining;
    }

    void StartWatching();

    VisualTreeWatcher(const VisualTreeWatcher&) = delete;
    VisualTreeWatcher& operator=(const VisualTreeWatcher&) = delete;

    VisualTreeWatcher(VisualTreeWatcher&&) = delete;
    VisualTreeWatcher& operator=(VisualTreeWatcher&&) = delete;

    ~VisualTreeWatcher();

    void UnadviseVisualTreeChange();

    bool ReleaseDiagnosticsReference(InstanceHandle handle);

private:
    HRESULT STDMETHODCALLTYPE OnVisualTreeChange(ParentChildRelation relation, VisualElement element, VisualMutationType mutationType) override;
    HRESULT STDMETHODCALLTYPE OnElementStateChanged(InstanceHandle element, VisualElementState elementState, LPCWSTR context) noexcept override;

    wf::IInspectable FromHandle(InstanceHandle handle)
    {
        wf::IInspectable obj;
        winrt::check_hresult(m_XamlDiagnostics->GetIInspectableFromHandle(handle, reinterpret_cast<::IInspectable**>(winrt::put_abi(obj))));
        return obj;
    }

    std::atomic<ULONG> m_referenceCount{1};
    winrt::com_ptr<IXamlDiagnostics> m_XamlDiagnostics = nullptr;
    winrt::com_ptr<IXamlDiagnosticsTestHooks> m_XamlDiagnosticsTestHooks = nullptr;
};

VisualTreeWatcher::VisualTreeWatcher(winrt::com_ptr<IUnknown> site) :
    m_XamlDiagnostics(site.as<IXamlDiagnostics>())
{
    Wh_Log(L"Constructing VisualTreeWatcher");

    HRESULT hr = m_XamlDiagnostics->QueryInterface(IID_IXamlDiagnosticsTestHooks, m_XamlDiagnosticsTestHooks.put_void());
    if (FAILED(hr)) {
        Wh_Log(L"IXamlDiagnosticsTestHooks is unavailable, elements will be leaked: %08X", hr);
    }

    // Match winrt::implements' module accounting for this native COM object.
    ++winrt::get_module_lock();
}

void VisualTreeWatcher::StartWatching()
{
    // Advise may synchronously call into XAML's UI thread. Keep it on a worker,
    // and acquire its reference BEFORE the thread can run.
    AddRef();
    HANDLE thread = CreateThread(
        nullptr, 0,
        [](LPVOID lpParam) -> DWORD {
            auto watcher = static_cast<VisualTreeWatcher*>(lpParam);
            HRESULT hr = S_OK;
            try {
                auto service = watcher->m_XamlDiagnostics.as<IVisualTreeService3>();
                g_reportCompositionDiagAsDisabled = true;
                hr = service->AdviseVisualTreeChange(
                    static_cast<IVisualTreeServiceCallback*>(watcher));
                g_reportCompositionDiagAsDisabled = false;
            } catch (...) {
                g_reportCompositionDiagAsDisabled = false;
                hr = winrt::to_hresult();
            }
            if (FAILED(hr)) {
                Wh_Log(L"AdviseVisualTreeChange failed: %08X", hr);
            }
            watcher->Release();
            return 0;
        },
        this, 0, nullptr);
    if (thread) {
        CloseHandle(thread);
    } else {
        DWORD error = GetLastError();
        Release();
        winrt::throw_hresult(HRESULT_FROM_WIN32(error));
    }
}

VisualTreeWatcher::~VisualTreeWatcher()
{
    Wh_Log(L"Destructing VisualTreeWatcher");
    --winrt::get_module_lock();
}

void VisualTreeWatcher::UnadviseVisualTreeChange()
{
    Wh_Log(L"UnadviseVisualTreeChange VisualTreeWatcher");
    HRESULT hr = m_XamlDiagnostics.as<IVisualTreeService3>()->UnadviseVisualTreeChange(
        static_cast<IVisualTreeServiceCallback*>(this));
    if (FAILED(hr)) {
        Wh_Log(L"UnadviseVisualTreeChange failed with error %08X", hr);
    }
}

// Reports whether dropping the reference destroyed the element, which is what
// tells the caller that the handle is free to name a different element from
// now on.
bool VisualTreeWatcher::ReleaseDiagnosticsReference(InstanceHandle handle)
{
    if (!m_XamlDiagnosticsTestHooks) {
        return false;
    }

    HRESULT hr = m_XamlDiagnosticsTestHooks->UnregisterInstance(handle);
    if (FAILED(hr)) {
        Wh_Log(L"UnregisterInstance failed with error %08X", hr);
        return false;
    }

    return true;
}

winrt::com_ptr<VisualTreeWatcher> g_visualTreeWatcher;

// The diagnostics create a runtime object which holds every element they
// report, and drop it only once the element is reported as removed. A tree
// which is discarded whole is never taken apart that way, so its elements are
// handed back from here once their report was processed, letting the teardown
// happen. The release is deferred out of the report callback, which runs from
// inside XAML's own tree walks.
thread_local std::vector<InstanceHandle> g_pendingDiagnosticsRelease;
thread_local ULONGLONG g_lastDiagnosticsReleaseQueueTick;
thread_local bool g_diagnosticsReleaseDrainQueued;
thread_local winrt::Windows::System::DispatcherQueueTimer
    g_diagnosticsReleaseDrainTimer{nullptr};
thread_local winrt::Windows::System::DispatcherQueueTimer::Tick_revoker
    g_diagnosticsReleaseDrainTimerTickRevoker;

// Long enough to sit out a tree being built.
constexpr ULONGLONG kDiagnosticsReleaseDelay = 200;

// The drain waits on a one-shot timer, which the thread teardown can stop,
// rather than on a dispatcher item, which it cannot: the module is freed once
// the mod is uninitialized, and an item still on the dispatcher would call
// into it.
constexpr ULONGLONG kDiagnosticsReleaseDrainDelay = 1;

void FlushDiagnosticsReleases() {
    auto pending = std::move(g_pendingDiagnosticsRelease);
    g_pendingDiagnosticsRelease.clear();

    if (!g_visualTreeWatcher) {
        return;
    }

    // A handle is queued once per report naming it, so a parent appears once
    // per child.
    std::sort(pending.begin(), pending.end());
    pending.erase(std::unique(pending.begin(), pending.end()), pending.end());

    for (InstanceHandle handle : pending) {
        g_visualTreeWatcher->ReleaseDiagnosticsReference(handle);
    }
}

void QueueDiagnosticsRelease(InstanceHandle handle) {
    if (!handle) {
        return;
    }

    g_pendingDiagnosticsRelease.push_back(handle);
    g_lastDiagnosticsReleaseQueueTick = GetTickCount64();
}

void DrainDiagnosticsReleases() {
    g_diagnosticsReleaseDrainQueued = false;
    FlushDiagnosticsReleases();
}

void FlushDiagnosticsReleasesIfQuiet() {
    if (g_pendingDiagnosticsRelease.empty() ||
        g_diagnosticsReleaseDrainQueued ||
        GetTickCount64() - g_lastDiagnosticsReleaseQueueTick <
            kDiagnosticsReleaseDelay) {
        return;
    }

    try {
        if (!g_diagnosticsReleaseDrainTimer) {
            auto dispatcherQueue =
                winrt::Windows::System::DispatcherQueue::GetForCurrentThread();
            if (!dispatcherQueue) {
                // Releasing from here is the one thing that isn't safe, so
                // the elements stay held instead.
                Wh_Log(L"No dispatcher queue, elements will be held");
                return;
            }

            g_diagnosticsReleaseDrainTimer = dispatcherQueue.CreateTimer();
            g_diagnosticsReleaseDrainTimer.IsRepeating(false);
            g_diagnosticsReleaseDrainTimer.Interval(
                std::chrono::milliseconds{kDiagnosticsReleaseDrainDelay});
            g_diagnosticsReleaseDrainTimerTickRevoker =
                g_diagnosticsReleaseDrainTimer.Tick(
                    winrt::auto_revoke,
                    [](winrt::Windows::System::DispatcherQueueTimer const&,
                       wf::IInspectable const&) {
                        DrainDiagnosticsReleases();
                    });
        }

        g_diagnosticsReleaseDrainTimer.Start();
        g_diagnosticsReleaseDrainQueued = true;
    } catch (winrt::hresult_error const& ex) {
        Wh_Log(L"Error %08X: %s", ex.code(), ex.message().c_str());
    }
}

void StopDiagnosticsReleases() {
    g_pendingDiagnosticsRelease.clear();

    if (g_diagnosticsReleaseDrainTimer) {
        try {
            g_diagnosticsReleaseDrainTimer.Stop();
        } catch (winrt::hresult_error const& ex) {
            Wh_Log(L"Error %08X: %s", ex.code(), ex.message().c_str());
        }

        g_diagnosticsReleaseDrainTimerTickRevoker.revoke();
        g_diagnosticsReleaseDrainTimer = nullptr;
    }

    g_diagnosticsReleaseDrainQueued = false;
}

HRESULT VisualTreeWatcher::OnVisualTreeChange(ParentChildRelation relation, VisualElement element, VisualMutationType mutationType) try
{
    if (!g_initializedForThread)
    {
        return S_OK;
    }

    try
    {
        if (mutationType == Add)
        {
            const auto inspectable = FromHandle(element.Handle);
            if (auto frameworkElement = inspectable.try_as<wux::FrameworkElement>())
            {
                HandleElementAdded(frameworkElement);
            }
        }
    }
    catch (...)
    {
        Wh_Log(L"Error %08X", winrt::to_hresult());
    }

    // A tree discarded whole is never dismantled, so it reports no removals to
    // be released by.
    FlushDiagnosticsReleasesIfQuiet();

    if (mutationType == Add)
    {
        QueueDiagnosticsRelease(element.Handle);
        QueueDiagnosticsRelease(relation.Parent);
    }
    else if (mutationType == Remove)
    {
        // Queued rather than released outright: this report arrives from
        // inside the Leave walk which is still visiting the subtree being
        // removed.
        QueueDiagnosticsRelease(element.Handle);
    }

    return S_OK;
}
catch (...)
{
    Wh_Log(L"Error %08X", winrt::to_hresult());

    // Returning an error prevents (some?) further messages, always return
    // success.
    return S_OK;
}

HRESULT VisualTreeWatcher::OnElementStateChanged(InstanceHandle, VisualElementState, LPCWSTR) noexcept
{
    return S_OK;
}

////////////////////////////////////////////////////////////////////////////////
// Windhawk TAP

// {C85D8CC7-5463-40E8-A432-F5916B6427E5}
static constexpr CLSID CLSID_WindhawkTAP = { 0xc85d8cc7, 0x5463, 0x40e8, { 0xa4, 0x32, 0xf5, 0x91, 0x6b, 0x64, 0x27, 0xe5 } };

class WindhawkTAP : public winrt::implements<WindhawkTAP, IObjectWithSite, winrt::non_agile>
{
public:
    HRESULT STDMETHODCALLTYPE SetSite(IUnknown *pUnkSite) override;
    HRESULT STDMETHODCALLTYPE GetSite(REFIID riid, void **ppvSite) noexcept override;

private:
    winrt::com_ptr<IUnknown> site;
};

HRESULT WindhawkTAP::SetSite(IUnknown *pUnkSite) try
{
    // Only ever 1 VTW at once.
    if (g_visualTreeWatcher)
    {
        g_visualTreeWatcher->UnadviseVisualTreeChange();
        g_visualTreeWatcher = nullptr;
    }

    site.copy_from(pUnkSite);

    if (site)
    {
        // Decrease refcount increased by InitializeXamlDiagnosticsEx.
        FreeLibrary(GetCurrentModuleHandle());

        // The native callback starts with one owned COM reference.
        g_visualTreeWatcher.attach(new VisualTreeWatcher(site));
        g_visualTreeWatcher->StartWatching();
    }

    return S_OK;
}
catch (...)
{
    HRESULT hr = winrt::to_hresult();
    Wh_Log(L"Error %08X", hr);
    return hr;
}

HRESULT WindhawkTAP::GetSite(REFIID riid, void **ppvSite) noexcept
{
    return site.as(riid, ppvSite);
}

template<class T>
struct SimpleFactory : winrt::implements<SimpleFactory<T>, IClassFactory, winrt::non_agile>
{
    HRESULT STDMETHODCALLTYPE CreateInstance(IUnknown* pUnkOuter, REFIID riid, void** ppvObject) override try
    {
        if (!pUnkOuter)
        {
            *ppvObject = nullptr;
            return winrt::make<T>().as(riid, ppvObject);
        }
        else
        {
            return CLASS_E_NOAGGREGATION;
        }
    }
    catch (...)
    {
        HRESULT hr = winrt::to_hresult();
        Wh_Log(L"Error %08X", hr);
        return hr;
    }

    HRESULT STDMETHODCALLTYPE LockServer(BOOL) noexcept override
    {
        return S_OK;
    }
};

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdll-attribute-on-redeclaration"

__declspec(dllexport)
_Use_decl_annotations_ STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv) try
{
    if (rclsid == CLSID_WindhawkTAP)
    {
        *ppv = nullptr;
        return winrt::make<SimpleFactory<WindhawkTAP>>().as(riid, ppv);
    }
    else
    {
        return CLASS_E_CLASSNOTAVAILABLE;
    }
}
catch (...)
{
    HRESULT hr = winrt::to_hresult();
    Wh_Log(L"Error %08X", hr);
    return hr;
}

__declspec(dllexport)
_Use_decl_annotations_ STDAPI DllCanUnloadNow()
{
    if (winrt::get_module_lock())
    {
        return S_FALSE;
    }
    else
    {
        return S_OK;
    }
}

#pragma clang diagnostic pop

using PFN_INITIALIZE_XAML_DIAGNOSTICS_EX = decltype(&InitializeXamlDiagnosticsEx);

HRESULT InjectWindhawkTAP() noexcept
{
    HMODULE module = GetCurrentModuleHandle();
    if (!module)
    {
        return HRESULT_FROM_WIN32(GetLastError());
    }

    WCHAR location[MAX_PATH];
    switch (GetModuleFileName(module, location, ARRAYSIZE(location)))
    {
    case 0:
    case ARRAYSIZE(location):
        return HRESULT_FROM_WIN32(GetLastError());
    }

    const HMODULE wux(LoadLibraryEx(L"Windows.UI.Xaml.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32));
    if (!wux)
    {
        return HRESULT_FROM_WIN32(GetLastError());
    }

    const auto ixde = reinterpret_cast<PFN_INITIALIZE_XAML_DIAGNOSTICS_EX>(GetProcAddress(wux, "InitializeXamlDiagnosticsEx"));
    if (!ixde)
    {
        return HRESULT_FROM_WIN32(GetLastError());
    }

    // I didn't find a better way than trying many connections until one works.
    // Reference:
    // https://github.com/microsoft/microsoft-ui-xaml/blob/d74a0332cf0d5e58f12eddce1070fa7a79b4c2db/src/dxaml/xcp/dxaml/lib/DXamlCore.cpp#L2782
    HRESULT hr;
    for (int i = 0; i < 10000; i++)
    {
        WCHAR connectionName[256];
        wsprintf(connectionName, L"VisualDiagConnection%d", i + 1);

        hr = ixde(connectionName, GetCurrentProcessId(), L"", location, CLSID_WindhawkTAP, nullptr);
        if (hr != HRESULT_FROM_WIN32(ERROR_NOT_FOUND))
        {
            break;
        }
    }

    return hr;
}

////////////////////////////////////////////////////////////////////////////////
// Initialization

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

bool RunFromWindowThreadViaPostMessage(HWND hWnd,
                                       RunFromWindowThreadProc_t proc,
                                       PVOID procParam) {
    static const UINT runFromWindowThreadRegisteredMsgViaPostMessage =
        RegisterWindowMessage(
            L"Windhawk_RunFromWindowThreadViaPostMessage_" WH_MOD_ID);

    struct RUN_FROM_WINDOW_THREAD_PARAM {
        RunFromWindowThreadProc_t proc;
        PVOID procParam;
        HHOOK hook;
    };

    DWORD dwThreadId = GetWindowThreadProcessId(hWnd, nullptr);
    if (dwThreadId == 0) {
        return false;
    }

    HHOOK hook = SetWindowsHookEx(
        WH_GETMESSAGE,
        [](int nCode, WPARAM wParam, LPARAM lParam) -> LRESULT {
            if (nCode == HC_ACTION && wParam == PM_REMOVE) {
                MSG* msg = (MSG*)lParam;
                if (msg->message ==
                    runFromWindowThreadRegisteredMsgViaPostMessage) {
                    auto* param = (RUN_FROM_WINDOW_THREAD_PARAM*)msg->lParam;
                    if (param) {
                        param->proc(param->procParam);
                        UnhookWindowsHookEx(param->hook);
                        delete param;
                        msg->lParam = 0;
                    }
                }
            }

            return CallNextHookEx(nullptr, nCode, wParam, lParam);
        },
        nullptr, dwThreadId);
    if (!hook) {
        return false;
    }

    auto* param = new RUN_FROM_WINDOW_THREAD_PARAM{
        .proc = proc,
        .procParam = procParam,
        .hook = hook,
    };
    if (!PostMessage(hWnd, runFromWindowThreadRegisteredMsgViaPostMessage, 0,
                     (LPARAM)param)) {
        UnhookWindowsHookEx(hook);
        delete param;
        return false;
    }

    return true;
}

void InitializeForCurrentThread() {
    if (g_initializedForThread) {
        return;
    }

    LoadSettings();

    g_initializedForThread = true;
}

void UninitializeForCurrentThread() {
    if (!g_initializedForThread) {
        return;
    }

    RestoreAllGrids();
    RestoreAllPagers();

    for (auto& pagerState : g_pagers) {
        auto pager = pagerState->pager.get();
        if (pager && pagerState->hasVisibilityToken) {
            try {
                pager.UnregisterPropertyChangedCallback(
                    wux::UIElement::VisibilityProperty(),
                    pagerState->visibilityToken);
            } catch (...) {
            }
        }
    }
    g_gridStates.clear();
    g_pagers.clear();
    g_rules.clear();
    StopDiagnosticsReleases();

    g_initializedForThread = false;
}

void InitializeSettingsAndTap() {
    if (g_initialized.exchange(true)) {
        return;
    }

    HRESULT hr = InjectWindhawkTAP();
    if (FAILED(hr)) {
        Wh_Log(L"Error %08X", hr);
    }
}

void UninitializeSettingsAndTap() {
    if (g_visualTreeWatcher) {
        g_visualTreeWatcher->UnadviseVisualTreeChange();
        g_visualTreeWatcher = nullptr;
    }

    g_initialized = false;
}

void OnWindowCreated(HWND hWnd, LPCWSTR lpClassName, PCSTR funcName) {
    BOOL bTextualClassName = ((ULONG_PTR)lpClassName & ~(ULONG_PTR)0xffff) != 0;

    if (bTextualClassName &&
        _wcsicmp(lpClassName, L"ControlCenterWindow") == 0) {
        Wh_Log(L"Initializing - Created ControlCenterWindow: %08X via %S",
               (DWORD)(ULONG_PTR)hWnd, funcName);
        // Initializing at this point is too early and doesn't work.
        RunFromWindowThreadViaPostMessage(
            hWnd,
            [](PVOID) {
                InitializeForCurrentThread();
                InitializeSettingsAndTap();
            },
            nullptr);
    }
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
    if (!hWnd) {
        return hWnd;
    }

    OnWindowCreated(hWnd, lpClassName, __FUNCTION__);

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
    if (!hWnd) {
        return hWnd;
    }

    OnWindowCreated(hWnd, lpClassName, __FUNCTION__);

    return hWnd;
}

using RegOpenKeyExW_t = decltype(&RegOpenKeyExW);
RegOpenKeyExW_t RegOpenKeyExW_Original;
LSTATUS WINAPI RegOpenKeyExW_Hook(HKEY hKey,
                                  LPCWSTR lpSubKey,
                                  DWORD ulOptions,
                                  REGSAM samDesired,
                                  PHKEY phkResult) {
    LSTATUS result = RegOpenKeyExW_Original(hKey, lpSubKey, ulOptions,
                                            samDesired, phkResult);
    if (result == ERROR_SUCCESS || !g_reportCompositionDiagAsDisabled ||
        hKey != HKEY_LOCAL_MACHINE || !lpSubKey ||
        _wcsicmp(lpSubKey, L"Software\\Microsoft\\XAML\\Debug") != 0) {
        return result;
    }

    // The key usually doesn't exist, and the value isn't queried unless the
    // key could be opened, so hand out a key which does exist.
    Wh_Log(L"Substituting the XAML debug key");
    return RegOpenKeyExW_Original(HKEY_LOCAL_MACHINE, L"Software\\Microsoft",
                                  ulOptions, samDesired, phkResult);
}

using RegQueryValueExW_t = decltype(&RegQueryValueExW);
RegQueryValueExW_t RegQueryValueExW_Original;
LSTATUS WINAPI RegQueryValueExW_Hook(HKEY hKey,
                                     LPCWSTR lpValueName,
                                     LPDWORD lpReserved,
                                     LPDWORD lpType,
                                     LPBYTE lpData,
                                     LPDWORD lpcbData) {
    if (!g_reportCompositionDiagAsDisabled || !lpValueName ||
        _wcsicmp(lpValueName, L"DisableCompositionDiag") != 0) {
        return RegQueryValueExW_Original(hKey, lpValueName, lpReserved,
                                         lpType, lpData, lpcbData);
    }

    Wh_Log(L"Reporting DisableCompositionDiag as set");

    if (lpType) {
        *lpType = REG_DWORD;
    }

    if (lpData && (!lpcbData || *lpcbData < sizeof(DWORD))) {
        if (lpcbData) {
            *lpcbData = sizeof(DWORD);
        }
        return ERROR_MORE_DATA;
    }

    if (lpData) {
        *reinterpret_cast<DWORD*>(lpData) = 1;
    }

    if (lpcbData) {
        *lpcbData = sizeof(DWORD);
    }

    return ERROR_SUCCESS;
}

std::vector<HWND> GetControlCenterWnds() {
    struct ENUM_WINDOWS_PARAM {
        std::vector<HWND>* hWnds;
    };

    std::vector<HWND> hWnds;
    ENUM_WINDOWS_PARAM param = {&hWnds};
    EnumWindows(
        [](HWND hWnd, LPARAM lParam) -> BOOL {
            ENUM_WINDOWS_PARAM& param = *(ENUM_WINDOWS_PARAM*)lParam;

            DWORD dwProcessId = 0;
            if (!GetWindowThreadProcessId(hWnd, &dwProcessId) ||
                dwProcessId != GetCurrentProcessId()) {
                return TRUE;
            }

            WCHAR szClassName[32];
            if (GetClassName(hWnd, szClassName, ARRAYSIZE(szClassName)) == 0) {
                return TRUE;
            }

            if (_wcsicmp(szClassName, L"ControlCenterWindow") == 0) {
                param.hWnds->push_back(hWnd);
            }

            return TRUE;
        },
        (LPARAM)&param);

    return hWnds;
}

BOOL Wh_ModInit() {
    Wh_Log(L">");

    HMODULE user32Module =
        LoadLibraryEx(L"user32.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (user32Module) {
        void* pCreateWindowInBand =
            (void*)GetProcAddress(user32Module, "CreateWindowInBand");
        if (pCreateWindowInBand) {
            Wh_SetFunctionHook(pCreateWindowInBand,
                               (void*)CreateWindowInBand_Hook,
                               (void**)&CreateWindowInBand_Original);
        }

        void* pCreateWindowInBandEx =
            (void*)GetProcAddress(user32Module, "CreateWindowInBandEx");
        if (pCreateWindowInBandEx) {
            Wh_SetFunctionHook(pCreateWindowInBandEx,
                               (void*)CreateWindowInBandEx_Hook,
                               (void**)&CreateWindowInBandEx_Original);
        }
    }

    HMODULE kernelBaseModule = GetModuleHandle(L"kernelbase.dll");
    void* pKernelBaseRegOpenKeyExW =
        (void*)GetProcAddress(kernelBaseModule, "RegOpenKeyExW");
    Wh_SetFunctionHook(pKernelBaseRegOpenKeyExW, (void*)RegOpenKeyExW_Hook,
                       (void**)&RegOpenKeyExW_Original);

    void* pKernelBaseRegQueryValueExW =
        (void*)GetProcAddress(kernelBaseModule, "RegQueryValueExW");
    Wh_SetFunctionHook(pKernelBaseRegQueryValueExW,
                       (void*)RegQueryValueExW_Hook,
                       (void**)&RegQueryValueExW_Original);

    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L">");

    bool initialize = false;

    for (auto hCoreWnd : GetControlCenterWnds()) {
        Wh_Log(L"Initializing for %08X", (DWORD)(ULONG_PTR)hCoreWnd);
        RunFromWindowThread(
            hCoreWnd, [](PVOID) { InitializeForCurrentThread(); }, nullptr);
        initialize = true;
    }

    if (initialize) {
        InitializeSettingsAndTap();
    }
}

void Wh_ModUninit() {
    Wh_Log(L">");

    UninitializeSettingsAndTap();

    for (auto hCoreWnd : GetControlCenterWnds()) {
        Wh_Log(L"Uninitializing for %08X", (DWORD)(ULONG_PTR)hCoreWnd);
        RunFromWindowThread(
            hCoreWnd, [](PVOID) { UninitializeForCurrentThread(); }, nullptr);
    }
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");

    UninitializeSettingsAndTap();

    bool initialize = false;

    for (auto hCoreWnd : GetControlCenterWnds()) {
        Wh_Log(L"Reinitializing for %08X", (DWORD)(ULONG_PTR)hCoreWnd);
        RunFromWindowThread(
            hCoreWnd,
            [](PVOID) {
                UninitializeForCurrentThread();
                InitializeForCurrentThread();
            },
            nullptr);
        initialize = true;
    }

    if (initialize) {
        InitializeSettingsAndTap();
    }
}
