// ==WindhawkMod==
// @id tray-overflow-list
// @name Tray Overflow List
// @description Hidden tray icons in a list with application names
// @version 0.1.8.7
// @author HeyOkay
// @github https://github.com/HeyOkay
// @homepage https://github.com/HeyOkay/TrayOverflowList
// @include explorer.exe
// @architecture x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject -lversion
// @license GPL-3.0-only
// ==/WindhawkMod==
// Derived helpers: Ris Peng's System Tray Folders and m417z tray mods,
// GPL-3.0-only.
// https://github.com/ramensoftware/windhawk-mods/blob/main/mods/system-tray-folders.wh.cpp
// ==WindhawkModReadme==
/*
# Tray Overflow List

Displays hidden Windows 11 tray icons as a vertical list, with icons on the left
and application names on the right.

![Tray Overflow List
preview](https://raw.githubusercontent.com/HeyOkay/TrayOverflowList/70bedeeab69d9ac2a1f9c486ffe8ac594fa06fd6/docs/images/preview.png)

The original tray controls handle clicks, context menus, and dragging. The mod
preserves the native flyout background and measures its width. Long lists scroll
vertically, and long names are shortened with an ellipsis.

## Settings

- **Row height** defaults to 33 pixels. Set it to 0 to keep the native height;
  custom heights are applied from 20 pixels upward.
- **Hidden name suffixes** defaults to `.Root, .App, .Runner`. Listed endings
are removed from the end of labels without regard to case. For example,
  `WhatsApp.Root` becomes `WhatsApp`. Leave the setting empty to keep the
original names. Dots inside names and unlisted endings are preserved.

Changing settings reloads the mod to restore the native layout first. Reopen
the flyout to apply the new settings.

## Compatibility

Tested on Windows 11 26H2 build 26300.9457, x64, with an English Windows
interface. Other Windows builds and display languages have not been verified.
This mod uses internal tray interfaces, so Windows updates can affect
compatibility.

System Tray Folders and Taskbar tray icon spacing and grid change the same
flyout geometry. Disable them when investigating layout problems.
*/
// ==/WindhawkModReadme==
// clang-format off
// ==WindhawkModSettings==
/*
- rowHeight: 33
  $name: Row height (px)
  $description: Height of each row. 0 keeps the native height. Reopen the hidden icons flyout to apply.
- hiddenNameSuffixes: .Root, .App, .Runner
  $name: Hidden name suffixes
  $description: Comma-separated endings to hide from tray labels (case-insensitive). Only complete endings are removed. Leave empty to keep original names. Reopen the flyout to apply.
*/
// ==/WindhawkModSettings==
// clang-format on
#include <windhawk_utils.h>
#include <algorithm>
#include <atomic>
#include <cmath>
#include <cwctype>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <vector>
#undef GetCurrentTime
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Data.h>
#include <winrt/Windows.UI.Xaml.Markup.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.h>
using namespace winrt::Windows::UI::Xaml;
namespace wf = winrt::Windows::Foundation;
std::atomic<bool> g_unloading;
std::atomic<DWORD> g_overflowThreadId;
// Extend the highlight to the native flyout's right edge.
constexpr int kRightAdjust = 1;
int g_rowHeight = 33;
using NameSuffixList = std::vector<std::wstring>;
NameSuffixList g_nameSuffixes{L".Root", L".App", L".Runner"};

std::wstring Trim(std::wstring_view s) {
    size_t begin = 0;
    size_t end = s.size();
    while (begin < end && iswspace(s[begin])) {
        begin++;
    }
    while (end > begin && iswspace(s[end - 1])) {
        end--;
    }
    return std::wstring(s.substr(begin, end - begin));
}

// Only remove explicit dot-delimited suffixes at the end. Never infer
// word boundaries, strip arbitrary extensions, or return an empty label.
NameSuffixList ParseNameSuffixes(std::wstring_view text) {
    NameSuffixList result;
    size_t begin = 0;
    while (begin <= text.size()) {
        size_t end = text.find_first_of(L",;\r\n", begin);
        if (end == std::wstring_view::npos)
            end = text.size();
        auto suffix = Trim(text.substr(begin, end - begin));
        if (!suffix.empty()) {
            if (suffix.front() != L'.')
                suffix.insert(suffix.begin(), L'.');
            if (suffix.size() > 1 &&
                suffix.find_first_of(L" \t") == std::wstring::npos)
                result.push_back(std::move(suffix));
        }
        if (end == text.size())
            break;
        begin = end + 1;
    }
    // Prefer a complete multi-part ending if the user configured overlapping
    // ones.
    std::stable_sort(
        result.begin(), result.end(),
        [](auto const& a, auto const& b) { return a.size() > b.size(); });
    return result;
}
std::wstring StripNameSuffixes(std::wstring name,
                               NameSuffixList const& suffixes) {
    bool removed;
    do {
        removed = false;
        for (auto const& suffix : suffixes) {
            if (suffix.size() < 2 || suffix.front() != L'.' ||
                name.size() <= suffix.size())
                continue;
            auto tail = name.data() + name.size() - suffix.size();
            if (CompareStringOrdinal(
                    tail, static_cast<int>(suffix.size()), suffix.data(),
                    static_cast<int>(suffix.size()), TRUE) == CSTR_EQUAL) {
                auto candidate = Trim(std::wstring_view(
                    name.data(), name.size() - suffix.size()));
                if (candidate.empty())
                    continue;
                name = std::move(candidate);
                removed = true;
                break;
            }
        }
    } while (removed);
    return name;
}
std::wstring CleanTrayName(std::wstring name) {
    return StripNameSuffixes(std::move(name), g_nameSuffixes);
}
// Collapse line breaks and repeated spaces in names supplied by applications.
std::wstring NormalizeName(std::wstring_view s) {
    std::wstring result;
    result.reserve(s.size());
    bool lastWasSpace = false;
    for (WCHAR c : s) {
        if (iswspace(c)) {
            if (!lastWasSpace) {
                result.push_back(L' ');
            }
            lastWasSpace = true;
        } else {
            result.push_back(c);
            lastWasSpace = false;
        }
    }
    return Trim(result);
}

constexpr GUID IID_INotificationAreaIcon6 = {
    0xD76CAD80,
    0x3103,
    0x54F4,
    {0x9A, 0xBE, 0x5F, 0x04, 0x7C, 0xBF, 0xE8, 0x5C}};
constexpr int kNotificationAreaIcon6_get_AppFriendlyName = 6;

// Resolved from symbols.
GUID* g_iconConfigurationIid;
void* g_iconConfigurationVftable;
void* g_iconConfiguration_get_DataModel;
GUID* g_notificationAreaIconsDataModelIid;
void* g_notificationAreaIconsDataModelVftable;
void* g_notificationAreaIconsDataModel_get_UDKObject;
GUID* g_iconContentViewModelIid;
void* g_imageContentViewModelBaseVftable;
void* g_imageContentViewModel_get_ToolTipText;
GUID* g_imageContentViewModelIid;
void* g_imageContentViewModelVftable;
void* g_imageContentViewModel_get_AutomationPropertyName;

Markup::IXamlMember GetXamlMember(PCWSTR typeName, PCWSTR memberName) {
    auto provider =
        Application::Current().try_as<Markup::IXamlMetadataProvider>();
    if (!provider) {
        return nullptr;
    }
    auto xamlType = provider.GetXamlType(typeName);
    if (!xamlType) {
        return nullptr;
    }
    return xamlType.GetMember(memberName);
}

struct IconInfo {
    wf::IInspectable viewModel{nullptr};
    std::wstring appName;
    std::wstring toolTip;
    std::wstring name;
};

[[clang::no_destroy]] Markup::IXamlMember g_contentVmMember{nullptr};
[[clang::no_destroy]] Markup::IXamlMember g_configurationMember{nullptr};

// Queries an interface of an internal SystemTray.dll type. The result is only
// returned if it's backed by the expected implementation vtable, which makes
// calling that implementation's methods, resolved from symbols, on it safe.
winrt::com_ptr<IUnknown> QueryVerifiedInterface(wf::IInspectable const& object,
                                                GUID const* iid,
                                                void* expectedVftable) {
    winrt::com_ptr<IUnknown> result;
    if (!object || !iid || !expectedVftable) {
        return result;
    }
    ((IUnknown*)winrt::get_abi(object))
        ->QueryInterface(*iid, result.put_void());
    if (result && *(void**)result.get() != expectedVftable) {
        result = nullptr;
    }
    return result;
}

void* CallAbiGetter(IUnknown* object, void* getter) {
    void* value = nullptr;
    HRESULT hr = ((HRESULT(WINAPI*)(void*, void**))getter)(object, &value);
    return SUCCEEDED(hr) ? value : nullptr;
}

std::wstring CallStringGetter(IUnknown* object, void* getter) {
    winrt::hstring value;
    winrt::attach_abi(value, CallAbiGetter(object, getter));
    return NormalizeName(value);
}

// The app name comes from the executable, so it stays the same regardless of
// what the app puts in its tooltip.
void ReadAppName(IconInfo& info) {
    if (!g_iconConfiguration_get_DataModel ||
        !g_notificationAreaIconsDataModel_get_UDKObject) {
        return;
    }

    if (!g_configurationMember) {
        g_configurationMember =
            GetXamlMember(L"SystemTray.IconViewModel", L"Configuration");
        if (!g_configurationMember) {
            return;
        }
    }

    wf::IInspectable configuration{nullptr};
    try {
        configuration = g_configurationMember.GetValue(info.viewModel);
    } catch (...) {
        return;
    }

    auto configurationInterface = QueryVerifiedInterface(
        configuration, g_iconConfigurationIid, g_iconConfigurationVftable);
    if (!configurationInterface) {
        return;
    }

    wf::IInspectable dataModel{nullptr};
    winrt::attach_abi(dataModel,
                      CallAbiGetter(configurationInterface.get(),
                                    g_iconConfiguration_get_DataModel));

    auto dataModelInterface =
        QueryVerifiedInterface(dataModel, g_notificationAreaIconsDataModelIid,
                               g_notificationAreaIconsDataModelVftable);
    if (!dataModelInterface) {
        return;
    }

    wf::IInspectable udkIcon{nullptr};
    winrt::attach_abi(
        udkIcon, CallAbiGetter(dataModelInterface.get(),
                               g_notificationAreaIconsDataModel_get_UDKObject));
    if (!udkIcon) {
        return;
    }

    // Public interface, no vtable verification needed.
    winrt::com_ptr<IUnknown> icon6;
    ((IUnknown*)winrt::get_abi(udkIcon))
        ->QueryInterface(IID_INotificationAreaIcon6, icon6.put_void());
    if (icon6) {
        void* getter =
            (*(void***)icon6.get())[kNotificationAreaIcon6_get_AppFriendlyName];
        info.appName = CallStringGetter(icon6.get(), getter);
    }
}

void ReadIconInfo(IconInfo& info) {
    ReadAppName(info);

    if (!g_contentVmMember) {
        g_contentVmMember =
            GetXamlMember(L"SystemTray.IconViewModel", L"ContentVM");
        if (!g_contentVmMember) {
            return;
        }
    }

    wf::IInspectable contentViewModel{nullptr};
    try {
        contentViewModel = g_contentVmMember.GetValue(info.viewModel);
    } catch (...) {
        return;
    }

    if (g_imageContentViewModel_get_ToolTipText) {
        if (auto baseInterface = QueryVerifiedInterface(
                contentViewModel, g_iconContentViewModelIid,
                g_imageContentViewModelBaseVftable)) {
            info.toolTip = CallStringGetter(
                baseInterface.get(), g_imageContentViewModel_get_ToolTipText);
        }
    }

    // Only image icons (regular app icons) implement this interface.
    auto imageInterface =
        QueryVerifiedInterface(contentViewModel, g_imageContentViewModelIid,
                               g_imageContentViewModelVftable);
    if (!imageInterface) {
        return;
    }

    if (g_imageContentViewModel_get_AutomationPropertyName) {
        info.name = CallStringGetter(
            imageInterface.get(),
            g_imageContentViewModel_get_AutomationPropertyName);
    }
}

////////////////////////////////////////////////////////////////////////////////
FrameworkElement FindDescendantByName(DependencyObject const& element,
                                      std::wstring_view name) {
    int count = Media::VisualTreeHelper::GetChildrenCount(element);
    for (int i = 0; i < count; i++) {
        auto child = Media::VisualTreeHelper::GetChild(element, i);
        if (auto frameworkElement = child.try_as<FrameworkElement>()) {
            if (frameworkElement.Name() == name) {
                return frameworkElement;
            }
        }
        if (auto found = FindDescendantByName(child, name)) {
            return found;
        }
    }
    return nullptr;
}

////////////////////////////////////////////////////////////////////////////////
struct SavedProperty {
    winrt::weak_ref<DependencyObject> object;
    DependencyProperty property{nullptr};
    wf::IInspectable value{nullptr};
    Data::Binding binding{nullptr};
};
struct SavedLabel {
    winrt::weak_ref<Controls::Grid> grid;
    winrt::weak_ref<Controls::TextBlock> label;
};
struct State {
    void* manager = nullptr;
    winrt::weak_ref<Controls::Control> root;
    winrt::weak_ref<Controls::ItemsControl> items;
    winrt::weak_ref<Controls::Grid> scrollParent;
    winrt::weak_ref<Controls::ScrollViewer> scroll;
    std::vector<SavedProperty> saved;
    std::vector<SavedLabel> labels;
    winrt::event_token layout{};
    bool busy = false;
    bool detaching = false;
    unsigned int activeCallbacks = 0;
    double width = 0;
    uint32_t originalIndex = 0;
    int logCount = 0;
    int passes = 0;
    std::wstring signature;
    uint32_t measuredCount = UINT32_MAX;
};
// Only the tray UI thread accesses this optional. Reset there on unload so
// XAML values/bindings and the vector's buffer are released on the right
// thread.
[[clang::no_destroy]] std::optional<std::vector<std::shared_ptr<State>>>
    g_states{std::in_place};
struct LiveState {
    Controls::Control root{nullptr};
    Controls::ItemsControl items{nullptr};
    Controls::Grid scrollParent{nullptr};
    Controls::ScrollViewer scroll{nullptr};
};
LiveState GetLiveState(State const& s) {
    return {s.root.get(), s.items.get(), s.scrollParent.get(), s.scroll.get()};
}
void PruneExpiredEntries(State& s) {
    std::erase_if(s.saved,
                  [](auto const& value) { return !value.object.get(); });
    std::erase_if(s.labels, [](auto const& value) {
        return !value.grid.get() || !value.label.get();
    });
}
void Save(State& s,
          DependencyObject const& object,
          DependencyProperty const& property) {
    for (auto const& value : s.saved) {
        auto savedObject = value.object.get();
        if (winrt::get_abi(savedObject) == winrt::get_abi(object) &&
            winrt::get_abi(value.property) == winrt::get_abi(property))
            return;
    }
    auto expression =
        object.as<FrameworkElement>().GetBindingExpression(property);
    s.saved.push_back({winrt::make_weak(object), property,
                       object.ReadLocalValue(property),
                       expression ? expression.ParentBinding() : nullptr});
}
void RestoreProperty(DependencyObject const& object,
                     SavedProperty const& value) {
    if (value.binding)
        Data::BindingOperations::SetBinding(object, value.property,
                                            value.binding);
    else if (value.value == DependencyProperty::UnsetValue())
        object.ClearValue(value.property);
    else
        object.SetValue(value.property, value.value);
}
// Revoke executable callbacks first. If revocation fails, retain the state
// for another cleanup attempt; never release it and pretend it was detached.
bool Restore(State& s) {
    if (s.activeCallbacks)
        return false;
    s.detaching = true;
    s.busy = true;
    LiveState live;
    try {
        live = GetLiveState(s);
        if (s.layout.value && live.root)
            live.root.LayoutUpdated(s.layout);
        s.layout = {};
    } catch (...) {
        Wh_Log(L"Could not revoke tray layout handler; will retry");
        return false;
    }
    for (auto const& value : s.labels) {
        try {
            auto grid = value.grid.get();
            auto label = value.label.get();
            if (label)
                label.ClearValue(Controls::TextBlock::ForegroundProperty());
            uint32_t index;
            if (grid && label && grid.Children().IndexOf(label, index))
                grid.Children().RemoveAt(index);
        } catch (...) {
            Wh_Log(L"Could not remove a tray label");
        }
    }
    for (auto it = s.saved.rbegin(); it != s.saved.rend(); ++it) {
        try {
            if (auto object = it->object.get())
                RestoreProperty(object, *it);
        } catch (...) {
            Wh_Log(L"Could not restore a tray property");
        }
    }
    try {
        if (live.scrollParent && live.scroll && live.items) {
            uint32_t index;
            if (live.scrollParent.Children().IndexOf(live.scroll, index)) {
                live.scrollParent.Children().RemoveAt(index);
                live.scroll.Content(nullptr);
                uint32_t existing;
                if (!live.scrollParent.Children().IndexOf(live.items, existing))
                    live.scrollParent.Children().InsertAt(
                        std::min(s.originalIndex,
                                 live.scrollParent.Children().Size()),
                        live.items);
            }
        }
    } catch (...) {
        Wh_Log(L"Could not restore native tray scroll layout");
    }
    try {
        if (live.root)
            live.root.InvalidateMeasure();
    } catch (...) {
        Wh_Log(L"Could not invalidate native tray layout");
    }
    s.labels.clear();
    s.saved.clear();
    return true;
}
void Visit(State& s, LiveState const& live, DependencyObject const& obj) {
    auto fe = obj.try_as<FrameworkElement>();
    if (fe && winrt::get_class_name(fe) == L"SystemTray.NotifyIconView") {
        auto control = fe.try_as<Controls::Control>();
        if (control)
            control.ApplyTemplate();
        auto grid =
            FindDescendantByName(fe, L"ContainerGrid").try_as<Controls::Grid>();
        auto content = FindDescendantByName(fe, L"ContentGrid");
        if (!grid || !content)
            return;
        // The item ends where the visible
        // list area ends. The list area itself stops a bit short of the
        // flyout's right edge, so the fixed right adjustment extends both the
        // area (negative right margin on the scroll viewer) and the item by N
        // px.
        int adjust = kRightAdjust;
        if (live.scroll) {
            auto m = live.scroll.Margin();
            if (m.Right != -adjust) {
                m.Right = -adjust;
                live.scroll.Margin(m);
            }
        }
        double itemWidth = fe.Width();
        if (!std::isfinite(itemWidth) || itemWidth <= 0)
            itemWidth = s.width;
        try {
            FrameworkElement area = live.scroll
                                        ? live.scroll.as<FrameworkElement>()
                                        : live.items.as<FrameworkElement>();
            double areaW = live.scroll ? live.scroll.ViewportWidth()
                                       : live.items.ActualWidth();
            if (!(areaW > 40))
                areaW = area.ActualWidth();
            if (fe.ActualWidth() > 0 && areaW > 40) {
                double left =
                    fe.TransformToVisual(area).TransformPoint({0, 0}).X;
                double areaX =
                    area.TransformToVisual(live.root).TransformPoint({0, 0}).X;
                double feX = left + areaX;
                double rootW = live.root.ActualWidth();
                // Width up to the area's edge, measured without our own
                // extension.
                double areaRight = areaX + areaW - (live.scroll ? adjust : 0);
                double wanted = areaW - (live.scroll ? adjust : 0) - 2 * left;
                if (rootW > 40 && feX >= 0 && feX < rootW / 4)
                    wanted = std::min(wanted,
                                      std::min(rootW - feX, areaRight) - feX);
                if (left >= 0 && left < areaW / 4 && wanted > 40) {
                    wanted = std::floor(wanted) + adjust;
                    if (s.logCount < 3) {
                        s.logCount++;
                        Wh_Log(
                            L"Geometry: feX=%f left=%f areaX=%f areaW=%f "
                            L"rootW=%f adjust=%d -> %f",
                            feX, left, areaX, areaW, rootW, adjust, wanted);
                    }
                    if (std::abs(wanted - itemWidth) > 0.5)
                        itemWidth = wanted;
                }
            }
        } catch (...) {
        }
        itemWidth = std::max(40.0, itemWidth);
        Save(s, fe, FrameworkElement::WidthProperty());
        if (fe.Width() != itemWidth)
            fe.Width(itemWidth);
        Save(s, fe, FrameworkElement::HorizontalAlignmentProperty());
        if (fe.HorizontalAlignment() != HorizontalAlignment::Left)
            fe.HorizontalAlignment(HorizontalAlignment::Left);
        if (int h = g_rowHeight; h >= 20) {
            auto m = fe.Margin();
            double fh = std::max(16.0, h - m.Top - m.Bottom);
            Save(s, fe, FrameworkElement::HeightProperty());
            if (fe.Height() != fh)
                fe.Height(fh);
            Save(s, fe, FrameworkElement::VerticalAlignmentProperty());
            if (fe.VerticalAlignment() != VerticalAlignment::Center)
                fe.VerticalAlignment(VerticalAlignment::Center);
        }
        Save(s, content, FrameworkElement::WidthProperty());
        if (content.Width() != 40)
            content.Width(40);
        Save(s, content, FrameworkElement::HorizontalAlignmentProperty());
        if (content.HorizontalAlignment() != HorizontalAlignment::Left)
            content.HorizontalAlignment(HorizontalAlignment::Left);
        Controls::TextBlock label{nullptr};
        for (auto const& pair : s.labels)
            if (winrt::get_abi(pair.grid.get()) == winrt::get_abi(grid)) {
                label = pair.label.get();
                break;
            }
        if (!label) {
            label = Controls::TextBlock();
            label.Name(L"WindhawkTrayListLabel");
            label.Margin({48, 0, 12, 0});
            label.VerticalAlignment(VerticalAlignment::Center);
            label.TextWrapping(TextWrapping::NoWrap);
            label.TextTrimming(TextTrimming::CharacterEllipsis);
            label.FontSize(13);
            label.IsHitTestVisible(false);
            if (control) {
                Data::Binding binding;
                binding.Source(control);
                binding.Path(PropertyPath(L"Foreground"));
                label.SetBinding(Controls::TextBlock::ForegroundProperty(),
                                 binding);
            }
            grid.Children().Append(label);
            s.labels.push_back(
                {winrt::make_weak(grid), winrt::make_weak(label)});
        }
        // Windows may remove injected children when recycling a dragged row.
        // A remembered label must actually belong to the current grid.
        uint32_t labelIndex;
        if (!grid.Children().IndexOf(label, labelIndex))
            grid.Children().Append(label);
        IconInfo info;
        info.viewModel = fe.DataContext();
        ReadIconInfo(info);
        std::wstring name = info.appName.empty()
                                ? (info.name.empty() ? info.toolTip : info.name)
                                : info.appName;
        name = CleanTrayName(std::move(name));
        if (name.empty())
            name = L"Application";
        if (label.Text() != name)
            label.Text(name);
        return;
    }
    int n = Media::VisualTreeHelper::GetChildrenCount(obj);
    for (int i = 0; i < n; i++)
        Visit(s, live, Media::VisualTreeHelper::GetChild(obj, i));
}
// LayoutUpdated fires on every layout pass, including every frame of the
// open animation. Walking the whole tree each time causes visible stutter, so
// only do the work when something relevant changed (plus a few follow-up
// passes, because the geometry needs a completed layout to settle).
void AppendRowSignature(State& s,
                        DependencyObject const& object,
                        std::wstring& signature) {
    auto element = object.try_as<FrameworkElement>();
    if (element &&
        winrt::get_class_name(element) == L"SystemTray.NotifyIconView") {
        auto grid = FindDescendantByName(element, L"ContainerGrid")
                        .try_as<Controls::Grid>();
        signature += L'|';
        signature += std::to_wstring(
            reinterpret_cast<uintptr_t>(winrt::get_abi(element)));
        signature += L':';
        signature += std::to_wstring(
            reinterpret_cast<uintptr_t>(winrt::get_abi(element.DataContext())));
        signature += L':';
        signature +=
            std::to_wstring(reinterpret_cast<uintptr_t>(winrt::get_abi(grid)));
        bool attached = false;
        if (grid)
            for (auto const& pair : s.labels)
                if (winrt::get_abi(pair.grid.get()) == winrt::get_abi(grid)) {
                    auto label = pair.label.get();
                    uint32_t index;
                    attached = label && grid.Children().IndexOf(label, index);
                    break;
                }
        signature += attached ? L":label" : L":missing";
        return;
    }
    int count = Media::VisualTreeHelper::GetChildrenCount(object);
    for (int i = 0; i < count; i++)
        AppendRowSignature(s, Media::VisualTreeHelper::GetChild(object, i),
                           signature);
}
std::wstring LayoutSignature(State& s, LiveState const& live) {
    std::wstring sig;
    try {
        sig += std::to_wstring(live.items.Items().Size());
        sig += L'|';
        if (auto panel = live.items.ItemsPanelRoot())
            sig += std::to_wstring(panel.Children().Size());
        sig += L'|';
        double w = live.scroll ? live.scroll.ViewportWidth()
                               : live.items.ActualWidth();
        sig += std::to_wstring((int)std::lround(w * 10));
        AppendRowSignature(s, live.items, sig);
    } catch (...) {
    }
    return sig;
}
// Re-measure the native grid when startup applications add their icons.
// A width captured while only two icons exist must not remain fixed forever.
void UpdateNativeWidth(State& s, LiveState const& live) {
    uint32_t count = live.items.Items().Size();
    if (count == s.measuredCount || count == 0)
        return;
    auto panel = live.items.ItemsPanelRoot().try_as<Controls::WrapGrid>();
    if (!panel)
        return;
    struct PropertySnapshot {
        DependencyObject object{nullptr};
        SavedProperty original;
        wf::IInspectable listValue{nullptr};
    };
    // Strong refs only for this measurement, so row replacement during layout
    // cannot invalidate the restoration snapshot.
    std::vector<PropertySnapshot> snapshots;
    snapshots.reserve(s.saved.size());
    for (auto const& value : s.saved) {
        if (auto object = value.object.get())
            snapshots.push_back(
                {object, value, object.ReadLocalValue(value.property)});
    }
    std::vector<Controls::TextBlock> labels;
    for (auto const& value : s.labels)
        if (auto label = value.label.get())
            labels.push_back(label);
    bool unwrapped = false;
    uint32_t wrapperIndex = 0;
    auto restoreList = [&]() {
        if (unwrapped) {
            uint32_t index;
            if (live.scrollParent.Children().IndexOf(live.items, index))
                live.scrollParent.Children().RemoveAt(index);
            live.scroll.Content(live.items);
            live.scrollParent.Children().InsertAt(
                std::min(wrapperIndex, live.scrollParent.Children().Size()),
                live.scroll);
            unwrapped = false;
        }
        for (auto const& snapshot : snapshots) {
            auto const& property = snapshot.original.property;
            if (snapshot.listValue == DependencyProperty::UnsetValue())
                snapshot.object.ClearValue(property);
            else
                snapshot.object.SetValue(property, snapshot.listValue);
        }
        for (auto const& label : labels)
            label.Visibility(Visibility::Visible);
    };
    double width = 0;
    try {
        for (auto const& label : labels)
            label.Visibility(Visibility::Collapsed);
        for (auto const& snapshot : snapshots)
            RestoreProperty(snapshot.object, snapshot.original);
        if (live.scrollParent && live.scroll &&
            live.scrollParent.Children().IndexOf(live.scroll, wrapperIndex)) {
            live.scrollParent.Children().RemoveAt(wrapperIndex);
            live.scroll.Content(nullptr);
            unwrapped = true;
            live.scrollParent.Children().InsertAt(wrapperIndex, live.items);
        }
        live.items.InvalidateMeasure();
        live.root.InvalidateMeasure();
        live.root.UpdateLayout();
        width = live.items.ActualWidth();
        if (width < 80 || !std::isfinite(width))
            width = live.items.DesiredSize().Width;
    } catch (...) {
        restoreList();
        throw;
    }
    restoreList();
    if (width < 80 || !std::isfinite(width))
        return;
    s.measuredCount = count;
    s.width = width;
    live.items.Width(width);
    panel.ItemWidth(width);
    Wh_Log(L"Native width refreshed: icons=%u width=%f", count, width);
    s.passes = 3;
}
void Refresh(State& s, bool force = false) {
    if (s.busy || s.detaching || g_unloading)
        return;
    auto live = GetLiveState(s);
    if (!live.root || !live.items)
        return;
    auto sig = LayoutSignature(s, live);
    if (force || sig != s.signature) {
        s.signature = sig;
        s.passes = 3;
    }
    if (s.passes <= 0)
        return;
    // Don't resolve every saved weak reference on each animation frame.
    PruneExpiredEntries(s);
    s.passes--;
    s.busy = true;
    try {
        UpdateNativeWidth(s, live);
    } catch (...) {
        Wh_Log(L"Native width refresh failed");
    }
    try {
        if (auto panel =
                live.items.ItemsPanelRoot().try_as<Controls::WrapGrid>()) {
            int h = g_rowHeight;
            if (h >= 20) {
                Save(s, panel, Controls::WrapGrid::ItemHeightProperty());
                if (panel.ItemHeight() != h)
                    panel.ItemHeight(h);
            }
        }
    } catch (...) {
    }
    try {
        Visit(s, live, live.items);
    } catch (...) {
        Wh_Log(L"List labels: update failed");
    }
    s.busy = false;
}
void UpdateScrollHeight(Controls::ScrollViewer const& scroll) {
    if (!scroll)
        return;
    RECT work{};
    if (!SystemParametersInfoW(SPI_GETWORKAREA, 0, &work, 0))
        return;
    UINT dpi = 96;
    if (HWND taskbar = FindWindowW(L"Shell_TrayWnd", nullptr))
        dpi = GetDpiForWindow(taskbar);
    if (!dpi)
        dpi = 96;
    scroll.MaxHeight(
        std::max(120.0, (work.bottom - work.top) * 96.0 / dpi - 80));
}
void Attach(void* manager, Controls::Control const& root) {
    if (!manager || !root || g_unloading)
        return;
    DWORD threadId = GetCurrentThreadId();
    DWORD expected = 0;
    if (!g_overflowThreadId.compare_exchange_strong(expected, threadId) &&
        expected != threadId) {
        Wh_Log(L"Skipping overflow on a different XAML thread");
        return;
    }
    if (!g_states)
        return;
    auto currentItems = FindDescendantByName(root, L"OverflowItemsControl")
                            .try_as<Controls::ItemsControl>();
    for (auto it = g_states->begin(); it != g_states->end();) {
        auto old = *it;
        auto oldRoot = old->root.get();
        if ((old->busy || old->activeCallbacks) &&
            (old->manager == manager || oldRoot == root))
            return;
        if (old->manager == manager && oldRoot == root && currentItems &&
            old->items.get() == currentItems && !old->detaching) {
            UpdateScrollHeight(old->scroll.get());
            Refresh(*old, true);
            return;
        }
        if (!oldRoot || old->manager == manager || oldRoot == root) {
            if (!Restore(*old))
                return;
            it = g_states->erase(it);
        } else
            ++it;
    }
    root.ApplyTemplate();
    root.UpdateLayout();
    auto items = FindDescendantByName(root, L"OverflowItemsControl")
                     .try_as<Controls::ItemsControl>();
    if (!items) {
        Wh_Log(L"OverflowItemsControl missing");
        return;
    }
    items.UpdateLayout();
    auto panel = items.ItemsPanelRoot().try_as<Controls::WrapGrid>();
    if (!panel) {
        Wh_Log(L"Unsupported overflow panel");
        return;
    }
    double width = items.ActualWidth();
    if (width < 80)
        width = items.DesiredSize().Width;
    if (width < 80 || !std::isfinite(width)) {
        Wh_Log(L"Native width not available");
        return;
    }
    auto s = std::make_shared<State>();
    s->manager = manager;
    s->root = winrt::make_weak(root);
    s->items = winrt::make_weak(items);
    s->width = width;
    g_states->push_back(s);
    Save(*s, items, FrameworkElement::WidthProperty());
    items.Width(width);
    Save(*s, panel, Controls::WrapGrid::OrientationProperty());
    panel.Orientation(Controls::Orientation::Horizontal);
    Save(*s, panel, Controls::WrapGrid::MaximumRowsOrColumnsProperty());
    panel.MaximumRowsOrColumns(1);
    Save(*s, panel, Controls::WrapGrid::ItemWidthProperty());
    panel.ItemWidth(width);
    auto parent =
        Media::VisualTreeHelper::GetParent(items).try_as<Controls::Grid>();
    if (parent && parent.Children().IndexOf(items, s->originalIndex)) {
        s->scrollParent = winrt::make_weak(parent);
        Controls::ScrollViewer scroll;
        s->scroll = winrt::make_weak(scroll);
        scroll.HorizontalScrollMode(Controls::ScrollMode::Disabled);
        scroll.HorizontalScrollBarVisibility(
            Controls::ScrollBarVisibility::Disabled);
        scroll.VerticalScrollMode(Controls::ScrollMode::Enabled);
        scroll.VerticalScrollBarVisibility(Controls::ScrollBarVisibility::Auto);
        UpdateScrollHeight(scroll);
        Controls::Grid::SetRow(scroll, Controls::Grid::GetRow(items));
        Controls::Grid::SetColumn(scroll, Controls::Grid::GetColumn(items));
        parent.Children().RemoveAt(s->originalIndex);
        scroll.Content(items);
        parent.Children().InsertAt(s->originalIndex, scroll);
    } else
        Wh_Log(L"Scroll wrapper unavailable: unexpected parent");
    std::weak_ptr<State> weak = s;
    s->layout = root.LayoutUpdated([weak](auto&&, auto&&) {
        if (g_unloading)
            return;
        if (auto locked = weak.lock()) {
            struct CallbackScope {
                State& state;
                explicit CallbackScope(State& value) : state(value) {
                    ++state.activeCallbacks;
                }
                ~CallbackScope() { --state.activeCallbacks; }
            } callbackScope(*locked);
            try {
                Refresh(*locked);
            } catch (...) {
                if (!g_unloading)
                    Wh_Log(L"Tray layout refresh failed");
            }
        }
    });
    Refresh(*s, true);
    root.InvalidateMeasure();
    root.UpdateLayout();
    Wh_Log(L"List attached: native content width=%f", width);
}
void* showing = nullptr;
using Show_t = void(WINAPI*)(void*, POINT, int);
Show_t ShowOriginal;
void WINAPI ShowHook(void* self, POINT pt, int kind) {
    showing = self;
    ShowOriginal(self, pt, kind);
    showing = nullptr;
}
using Measure_t = void(WINAPI*)(void*, wf::Size const*);
Measure_t MeasureOriginal;
void WINAPI MeasureHook(void* self, wf::Size const* size) {
    MeasureOriginal(self, size);
    if (showing && !g_unloading)
        try {
            auto root = reinterpret_cast<wf::IInspectable const*>(self)
                            ->try_as<Controls::Control>();
            if (root) {
                Attach(showing, root);
                MeasureOriginal(self, size);
            }
        } catch (...) {
            Wh_Log(L"List attachment failed");
        }
}

struct RunFromWindowThreadParam {
    std::function<void()> proc;
    bool succeeded = false;
};

bool RunFromWindowThread(HWND hWnd, std::function<void()> proc) {
    static const UINT message =
        RegisterWindowMessage(L"Windhawk_RunFromWindowThread_" WH_MOD_ID);

    DWORD threadId = GetWindowThreadProcessId(hWnd, nullptr);
    if (!threadId || !message) {
        return false;
    }
    if (threadId == GetCurrentThreadId()) {
        try {
            proc();
            return true;
        } catch (...) {
            Wh_Log(L"Window thread operation failed");
            return false;
        }
    }

    RunFromWindowThreadParam param{std::move(proc)};
    HHOOK hook = SetWindowsHookEx(
        WH_CALLWNDPROC,
        [](int nCode, WPARAM wParam, LPARAM lParam) -> LRESULT {
            if (nCode == HC_ACTION) {
                const auto& cwp = *reinterpret_cast<const CWPSTRUCT*>(lParam);
                if (cwp.message == message) {
                    auto& param = *reinterpret_cast<RunFromWindowThreadParam*>(
                        cwp.lParam);
                    try {
                        param.proc();
                        param.succeeded = true;
                    } catch (...) {
                        Wh_Log(L"Window thread operation failed");
                    }
                }
            }
            return CallNextHookEx(nullptr, nCode, wParam, lParam);
        },
        nullptr, threadId);
    if (!hook) {
        return false;
    }

    SendMessage(hWnd, message, 0, (LPARAM)&param);
    UnhookWindowsHookEx(hook);
    return param.succeeded;
}

std::vector<HWND> GetOverflowThreadWindows() {
    std::vector<HWND> windows;
    if (DWORD threadId = g_overflowThreadId) {
        EnumThreadWindows(
            threadId,
            [](HWND hWnd, LPARAM lParam) -> BOOL {
                ((std::vector<HWND>*)lParam)->push_back(hWnd);
                return TRUE;
            },
            (LPARAM)&windows);
    }

    EnumWindows(
        [](HWND hWnd, LPARAM lParam) -> BOOL {
            DWORD processId;
            WCHAR className[32];
            if (GetWindowThreadProcessId(hWnd, &processId) &&
                processId == GetCurrentProcessId() &&
                GetClassName(hWnd, className, ARRAYSIZE(className)) &&
                _wcsicmp(className, L"Shell_TrayWnd") == 0) {
                ((std::vector<HWND>*)lParam)->push_back(hWnd);
                return FALSE;
            }
            return TRUE;
        },
        (LPARAM)&windows);

    return windows;
}

bool RunFromOverflowThread(std::function<void()> const& proc) {
    for (HWND hWnd : GetOverflowThreadWindows()) {
        if (GetWindowThreadProcessId(hWnd, nullptr) !=
            g_overflowThreadId.load())
            continue;
        bool ran = false;
        bool succeeded = RunFromWindowThread(hWnd, [&ran, &proc] {
            ran = true;
            proc();
        });
        if (ran) {
            return succeeded;
        }
    }
    return false;
}

////////////////////////////////////////////////////////////////////////////////
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
            // Newer Taskbar.View.dll versions no longer contain the tray.
            VS_FIXEDFILEINFO* fixedFileInfo =
                GetModuleVersionInfo(module, nullptr);
            WORD moduleMajor =
                fixedFileInfo ? HIWORD(fixedFileInfo->dwFileVersionMS) : 0;
            if (!moduleMajor || moduleMajor >= 2604) {
                module = nullptr;
            }
        }
    }

    return module;
}

std::atomic<bool> g_systemTrayModuleHooked;

bool HookSystemTraySymbols(HMODULE module) {
    // clang-format off
    // SystemTray.dll, Taskbar.View.dll
    WindhawkUtils::SYMBOL_HOOK symbolHooks[] = {
        {
            {LR"(public: void __cdecl winrt::SystemTray::OverflowXamlIslandManager::Show(struct tagPOINT,enum winrt::WindowsUdk::UI::Shell::InputDeviceKind))"},
            &ShowOriginal,
            ShowHook,
        },
        {
            {LR"(public: __cdecl winrt::impl::consume_Windows_UI_Xaml_IUIElement<struct winrt::SystemTray::NotificationAreaOverflow>::Measure(struct winrt::Windows::Foundation::Size const &)const )"},
            &MeasureOriginal,
            MeasureHook,
        },
        {
            {LR"(struct guid::guid const winrt::impl::guid_v<struct winrt::SystemTray::IIconConfiguration>)"},
            &g_iconConfigurationIid,
            nullptr,
            true,
        },
        {
            {LR"(const winrt::impl::produce<struct winrt::SystemTray::implementation::IconConfiguration,struct winrt::SystemTray::IIconConfiguration>::`vftable')"},
            &g_iconConfigurationVftable,
            nullptr,
            true,
        },
        {
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::SystemTray::implementation::IconConfiguration,struct winrt::SystemTray::IIconConfiguration>::get_DataModel(void * *))"},
            &g_iconConfiguration_get_DataModel,
            nullptr,
            true,
        },
        {
            {LR"(struct guid::guid const winrt::impl::guid_v<struct winrt::SystemTray::INotificationAreaIconsDataModel>)"},
            &g_notificationAreaIconsDataModelIid,
            nullptr,
            true,
        },
        {
            {LR"(const winrt::impl::produce<struct winrt::SystemTray::implementation::NotificationAreaIconsDataModel,struct winrt::SystemTray::INotificationAreaIconsDataModel>::`vftable')"},
            &g_notificationAreaIconsDataModelVftable,
            nullptr,
            true,
        },
        {
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::SystemTray::implementation::NotificationAreaIconsDataModel,struct winrt::SystemTray::INotificationAreaIconsDataModel>::get_UDKObject(void * *))"},
            &g_notificationAreaIconsDataModel_get_UDKObject,
            nullptr,
            true,
        },
        {
            {LR"(struct guid::guid const winrt::impl::guid_v<struct winrt::SystemTray::IIconContentViewModel>)"},
            &g_iconContentViewModelIid,
            nullptr,
            true,
        },
        {
            {LR"(const winrt::impl::produce<struct winrt::SystemTray::implementation::ImageIconContentViewModel,struct winrt::SystemTray::IIconContentViewModel>::`vftable')"},
            &g_imageContentViewModelBaseVftable,
            nullptr,
            true,
        },
        {
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::SystemTray::implementation::ImageIconContentViewModel,struct winrt::SystemTray::IIconContentViewModel>::get_ToolTipText(void * *))"},
            &g_imageContentViewModel_get_ToolTipText,
            nullptr,
            true,
        },
        {
            {LR"(struct guid::guid const winrt::impl::guid_v<struct winrt::SystemTray::IImageIconContentViewModel>)"},
            &g_imageContentViewModelIid,
            nullptr,
            true,
        },
        {
            {LR"(const winrt::impl::produce<struct winrt::SystemTray::implementation::ImageIconContentViewModel,struct winrt::SystemTray::IImageIconContentViewModel>::`vftable')"},
            &g_imageContentViewModelVftable,
            nullptr,
            true,
        },
        {
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::SystemTray::implementation::ImageIconContentViewModel,struct winrt::SystemTray::IImageIconContentViewModel>::get_AutomationPropertyName(void * *))"},
            &g_imageContentViewModel_get_AutomationPropertyName,
            nullptr,
            true,
        },
    };

    // clang-format on

    if (!WindhawkUtils::HookSymbols(module, symbolHooks,
                                    ARRAYSIZE(symbolHooks))) {
        Wh_Log(L"HookSymbols failed");
        return false;
    }

    bool appNames = g_iconConfigurationIid && g_iconConfigurationVftable &&
                    g_iconConfiguration_get_DataModel &&
                    g_notificationAreaIconsDataModelIid &&
                    g_notificationAreaIconsDataModelVftable &&
                    g_notificationAreaIconsDataModel_get_UDKObject;
    bool toolTips = g_iconContentViewModelIid &&
                    g_imageContentViewModelBaseVftable &&
                    g_imageContentViewModel_get_ToolTipText;
    bool names = g_imageContentViewModelIid && g_imageContentViewModelVftable &&
                 g_imageContentViewModel_get_AutomationPropertyName;
    Wh_Log(L"Icon names: app=%d tooltip=%d name=%d", appNames, toolTips, names);

    if (!appNames) {
        Wh_Log(L"App name matching unavailable");
    }

    if (!toolTips && !names) {
        Wh_Log(L"Couldn't find the icon tooltip getters");
        return false;
    }

    return true;
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
// Mod lifecycle

void LoadSettings() {
    g_rowHeight = Wh_GetIntSetting(L"rowHeight");
    auto text = WindhawkUtils::StringSetting::make(L"hiddenNameSuffixes");
    g_nameSuffixes = ParseNameSuffixes(text.get());
    Wh_Log(L"Settings: rowHeight=%d", g_rowHeight);
}

BOOL Wh_ModSettingsChanged(BOOL* reload) {
    // Restore the native layout before applying new settings, including height
    // 0.
    *reload = TRUE;
    return TRUE;
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
        if (!pKernelBaseLoadLibraryExW ||
            !WindhawkUtils::SetFunctionHook(pKernelBaseLoadLibraryExW,
                                            LoadLibraryExW_Hook,
                                            &LoadLibraryExW_Original)) {
            Wh_Log(L"Couldn't hook LoadLibraryExW");
            return FALSE;
        }
    }

    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L">");

    if (!g_systemTrayModuleHooked) {
        if (HMODULE systemTrayModule = GetSystemTrayModuleHandle()) {
            if (!g_systemTrayModuleHooked.exchange(true)) {
                if (HookSystemTraySymbols(systemTrayModule)) {
                    Wh_ApplyHookOperations();
                }
            }
        }
    }
}

void Wh_ModBeforeUninit() {
    g_unloading = true;
    if (!g_overflowThreadId.load())
        return;
    auto cleanup = [] {
        if (!g_states)
            return;
        for (auto it = g_states->begin(); it != g_states->end();) {
            bool detached = false;
            try {
                detached = Restore(**it);
            } catch (...) {
                Wh_Log(L"Tray state cleanup failed");
            }
            if (detached)
                it = g_states->erase(it);
            else
                ++it;
        }
        // Keep states with unrevoked handlers for the next attempt.
        if (!g_states->empty())
            return;
        g_states.reset();
        g_contentVmMember = nullptr;
        g_configurationMember = nullptr;
    };
    for (int attempt = 0; attempt < 3; ++attempt) {
        bool completed = false;
        bool dispatched = RunFromOverflowThread([&] {
            cleanup();
            completed = !g_states;
        });
        if (dispatched && completed)
            return;
        if (attempt < 2)
            Sleep(100);
    }
    // Windhawk cannot cancel an unload. On this exceptional path, retain the
    // DLL's code pages so an unrevoked XAML delegate cannot jump into unmapped
    // memory. The delegate sees g_unloading and immediately returns. No hooks
    // are kept active. Normal unloads release all state on the tray thread.
    HMODULE retainedModule = nullptr;
    if (GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                               GET_MODULE_HANDLE_EX_FLAG_PIN,
                           reinterpret_cast<LPCWSTR>(&Wh_ModBeforeUninit),
                           &retainedModule)) {
        Wh_Log(
            L"ERROR: tray cleanup could not finish; retained module for "
            L"callback safety. Restart Explorer before re-enabling the mod");
    } else {
        Wh_Log(
            L"ERROR: tray cleanup and module retention failed; restart "
            L"Explorer");
    }
}
