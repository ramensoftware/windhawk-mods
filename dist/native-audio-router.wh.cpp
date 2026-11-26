// ==WindhawkMod==
// @id native-audio-router
// @name Native Audio Router
// @description Choose a Windows output device from each Quick Settings mixer row
// @description:zh-CN 在任务栏原生音量合成器中选择逐应用输出设备
// @version 0.0.5
// @author KKKKeybird
// @github https://github.com/KKKKeybird
// @homepage https://github.com/KKKKeybird/native-audio-router
// @architecture x86-64
// @include ShellHost.exe
// @compilerOptions -lruntimeobject -loleaut32 -lole32
// @license MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Native Audio Router

Adds an output-device menu to application volume rows in Windows 11 Quick
Settings. Use a row's arrow to select a device or follow the system default.
The menu uses the Windows per-application output setting. Apps that retain
their original audio stream may require restarting playback.

Open Quick Settings once before enabling the mod, then reopen the Sound output
page. This preview was loaded on Windows 11 x64 build 26300.9974 with Windhawk
1.7.3. Runtime support depends on the shell symbols and row model.

## 中文

在任务栏快捷设置的「声音输出」页中，为应用音量行加入设备菜单。通过行尾
箭头选择设备或跟随系统默认。部分应用需要重新开始播放，才能切换现有音频流。
启用前先打开一次快捷设置，启用后重新进入「声音输出」页。

See the [project repository](https://github.com/KKKKeybird/native-audio-router)
for compatibility, test status, and source.
*/
// ==/WindhawkModReadme==

#include <windows.h>
#include <unknwn.h>
#include <mmdeviceapi.h>
#undef GetCurrentTime
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.UI.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Automation.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Controls.Primitives.h>
#include <winrt/Windows.UI.Xaml.Data.h>
#include <winrt/Windows.UI.Xaml.Interop.h>
#include <winrt/Windows.UI.Xaml.Media.h>

#include <algorithm>
#include <atomic>
#include <chrono>
#include <mutex>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace {

namespace X = winrt::Windows::UI::Xaml;
namespace C = X::Controls;
namespace F = winrt::Windows::Foundation;
namespace Core = winrt::Windows::UI::Core;

constexpr wchar_t templateSymbol[] =
    L"?OnApplyTemplate@?$produce@UVolumeMixerList@implementation@ControlCenter@winrt@@"
    L"UIFrameworkElementOverrides@Xaml@UI@Windows@4@@impl@winrt@@UEAAHXZ";
constexpr wchar_t focusSymbol[] =
    L"?OnGotFocus@?$produce@UVolumeMixerList@implementation@ControlCenter@winrt@@"
    L"UIControlOverrides@Controls@Xaml@UI@Windows@4@@impl@winrt@@UEAAHPEAX@Z";
constexpr wchar_t loadedSymbol[] =
    L"?OnVolumeMixerListLoaded@VolumeMixerList@implementation@ControlCenter@winrt@@"
    L"QEAAXUIInspectable@Foundation@Windows@4@URoutedEventArgs@Xaml@UI@74@@Z";

HRESULT (WINAPI* originalTemplate)(void*);
HRESULT (WINAPI* originalFocus)(void*, void*);
void (WINAPI* originalLoaded)(void*, void*, void*);
std::atomic_bool stopping{};
std::mutex dispatcherLock;
Core::CoreDispatcher dispatcher{nullptr};
std::atomic_uint64_t endpointGeneration{1};
winrt::com_ptr<IMMDeviceEnumerator> endpointEnumerator;
winrt::com_ptr<IMMNotificationClient> endpointNotifications;
bool endpointWatchAttempted{};

struct EndpointNotifications : winrt::implements<EndpointNotifications, IMMNotificationClient> {
    HRESULT STDMETHODCALLTYPE OnDeviceStateChanged(LPCWSTR, DWORD) noexcept override {
        return Invalidate();
    }
    HRESULT STDMETHODCALLTYPE OnDeviceAdded(LPCWSTR) noexcept override { return Invalidate(); }
    HRESULT STDMETHODCALLTYPE OnDeviceRemoved(LPCWSTR) noexcept override { return Invalidate(); }
    HRESULT STDMETHODCALLTYPE OnDefaultDeviceChanged(EDataFlow flow, ERole, LPCWSTR) noexcept override {
        return flow == eRender ? Invalidate() : S_OK;
    }
    HRESULT STDMETHODCALLTYPE OnPropertyValueChanged(LPCWSTR, const PROPERTYKEY) noexcept override {
        return Invalidate();
    }

    static HRESULT Invalidate() noexcept {
        ++endpointGeneration;
        return S_OK;
    }
};

enum class MenuRequest { None, Warm, Open };

struct Row {
    winrt::weak_ref<C::Grid> grid;
    winrt::weak_ref<C::Slider> slider;
    C::ColumnDefinition column{nullptr};
    C::Button button{nullptr};
    C::MenuFlyout menu{nullptr};
    winrt::event_token click{};
    std::vector<std::pair<C::ToggleMenuFlyoutItem, winrt::event_token>> itemClicks;
    X::DispatcherTimer refresh{nullptr};
    winrt::event_token tick{};
    ULONGLONG deadline{};
    MenuRequest request{MenuRequest::None};
    winrt::weak_ref<F::IInspectable> requestModel;
};

struct Mixer {
    winrt::weak_ref<X::FrameworkElement> element;
    winrt::event_token layout{};
};

struct Selection {
    int32_t index;
    std::vector<winrt::hstring> names;
};

struct CachedOutputs {
    winrt::weak_ref<F::IInspectable> model;
    uint64_t generation;
    Selection selection;
};

struct Change {
    winrt::weak_ref<F::IInspectable> model;
    winrt::hstring original;
    winrt::hstring applied;
    ULONGLONG verifyUntil{};
};

std::vector<Row> rows;
std::vector<Mixer> mixers;
std::vector<Change> changes;
std::vector<CachedOutputs> outputCache;

void StartEndpointWatch() {
    if (endpointWatchAttempted) return;
    endpointWatchAttempted = true;
    try {
        winrt::com_ptr<IMMDeviceEnumerator> enumerator;
        winrt::check_hresult(CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr,
                                             CLSCTX_INPROC_SERVER, IID_PPV_ARGS(enumerator.put())));
        auto notifications = winrt::make<EndpointNotifications>();
        winrt::check_hresult(enumerator->RegisterEndpointNotificationCallback(notifications.get()));
        endpointEnumerator = std::move(enumerator);
        endpointNotifications = std::move(notifications);
        Wh_Log(L"Audio endpoint change notifications registered");
    } catch (...) {
        Wh_Log(L"Endpoint notifications unavailable, device cache disabled (0x%08X)",
               static_cast<unsigned>(winrt::to_hresult().value));
    }
}

X::Data::ICustomProperty Property(const F::IInspectable& model, const wchar_t* name) {
    if (!model) return nullptr;
    const auto properties = model.try_as<X::Data::ICustomPropertyProvider>();
    return properties ? properties.GetCustomProperty(name) : nullptr;
}

F::IInspectable Model(const C::Slider& slider) {
    auto node = X::Media::VisualTreeHelper::GetParent(slider);
    C::ListViewItem item{nullptr};
    while (node) {
        if (!item) item = node.try_as<C::ListViewItem>();
        if (const auto list = node.try_as<C::ListViewBase>(); list && item) {
            const auto model = list.ItemFromContainer(item);
            if (model && model != X::DependencyProperty::UnsetValue() &&
                winrt::get_class_name(model) == L"ControlCenter.QuickActionViewModel") {
                return model;
            }
            return nullptr;
        }
        node = X::Media::VisualTreeHelper::GetParent(node);
    }
    return nullptr;
}

std::optional<uint32_t> ReadSelectedIndex(const F::IInspectable& model) {
    const auto selected = Property(model, L"SelectedOutput");
    if (!selected || !selected.CanRead()) return std::nullopt;
    const auto value = selected.GetValue(model).try_as<F::IPropertyValue>();
    if (!value || value.Type() != F::PropertyType::UInt32) return std::nullopt;
    return value.GetUInt32();
}

std::optional<Selection> ReadSelection(const F::IInspectable& model) {
    const auto options = Property(model, L"PossibleOutputs");
    // On this native model, QuickActionDataProperty::CanWrite always returns
    // false. Its SetValue implementation still forwards SelectedOutput to
    // the Windows setting item. This applies only to the checked model class.
    if (!options || !options.CanRead()) return std::nullopt;
    const auto list = options.GetValue(model).try_as<X::Interop::IBindableVector>();
    if (!list) return std::nullopt;
    const auto index = ReadSelectedIndex(model);
    const auto count = list.Size();
    if (!index || count > 128 || *index >= count) return std::nullopt;
    Selection state{static_cast<int32_t>(*index), {}};
    state.names.reserve(count);
    for (uint32_t i = 0; i < count; ++i) {
        auto name = winrt::unbox_value_or<winrt::hstring>(list.GetAt(i), L"");
        if (name.empty()) return std::nullopt;
        state.names.push_back(std::move(name));
    }
    return state;
}

bool UniqueName(const Selection& state, const winrt::hstring& name) {
    return std::ranges::count(state.names, name) == 1;
}

auto FindChange(const F::IInspectable& model) {
    return std::ranges::find_if(changes, [&model](const auto& entry) {
        return entry.model.get() == model;
    });
}

void UpdateMenuIndex(const F::IInspectable& model, Selection& selection,
                     std::optional<uint32_t> reported) {
    const auto change = FindChange(model);
    if (change != changes.end() && GetTickCount64() < change->verifyUntil) {
        const auto requested = std::ranges::find(selection.names, change->applied);
        if (requested != selection.names.end() && UniqueName(selection, change->applied)) {
            const auto index = static_cast<uint32_t>(requested - selection.names.begin());
            if (reported && *reported == index) change->verifyUntil = 0;
            selection.index = static_cast<int32_t>(index);
            return;
        }
    }
    if (reported && *reported < selection.names.size()) selection.index = static_cast<int32_t>(*reported);
}

void Select(const F::IInspectable& model, const Selection& shown, int32_t index, uint64_t generation) {
    const auto current = ReadSelection(model);
    if (!current || generation != endpointGeneration.load()) return;
    if (current->names != shown.names) {
        std::erase_if(outputCache, [&model](const auto& entry) { return entry.model.get() == model; });
        return;
    }
    if (!UniqueName(*current, current->names[index]) ||
        !UniqueName(*current, current->names[current->index])) {
        return;
    }
    auto change = FindChange(model);
    const auto now = GetTickCount64();
    const auto& reported = current->names[current->index];
    const auto& requested = current->names[index];
    const bool awaiting = change != changes.end() && now < change->verifyUntil;
    if (awaiting ? change->applied == requested : current->index == index) return;
    const bool newChange = change == changes.end();
    if (newChange) changes.reserve(changes.size() + 1);
    Property(model, L"SelectedOutput").SetValue(model, winrt::box_value(index));
    const auto verifyUntil = GetTickCount64() + 1500;
    if (newChange) {
        changes.push_back({winrt::make_weak(model), reported, requested, verifyUntil});
    } else {
        if (!awaiting && change->applied != reported) change->original = reported;
        change->applied = requested;
        change->verifyUntil = verifyUntil;
    }
    Wh_Log(L"Requested native output index %d", index);
}

void ClearMenu(Row& row) {
    for (const auto& [item, token] : row.itemClicks) item.Click(token);
    row.itemClicks.clear();
    row.menu.Items().Clear();
}

void ShowMenu(Row& row, const C::Slider& slider, const F::IInspectable& model,
              const Selection& shown, uint64_t generation) {
    ClearMenu(row);
    const auto& menu = row.menu;
    for (int32_t i = 0; i < static_cast<int32_t>(shown.names.size()); ++i) {
        C::ToggleMenuFlyoutItem item;
        item.Text(shown.names[i]);
        item.IsChecked(i == shown.index);
        item.IsEnabled(UniqueName(shown, shown.names[i]));
        const auto token = item.Click([weakSlider = winrt::make_weak(slider), weakModel = winrt::make_weak(model),
                                       snapshot = shown, generation, i](auto&&, auto&&) {
            if (stopping || generation != endpointGeneration.load()) return;
            try {
                const auto currentSlider = weakSlider.get();
                const auto currentModel = currentSlider ? Model(currentSlider) : nullptr;
                if (currentModel && currentModel == weakModel.get()) {
                    Select(currentModel, snapshot, i, generation);
                }
            } catch (...) {
                Wh_Log(L"Native output selection failed (0x%08X)", static_cast<unsigned>(winrt::to_hresult().value));
            }
        });
        row.itemClicks.emplace_back(item, token);
        menu.Items().Append(item);
    }
    Wh_Log(L"Native output menu opened with %zu options, selected index %d",
           shown.names.size(), shown.index);
    row.menu.ShowAt(row.button);
}

void StopRequest(Row& row) {
    row.refresh.Stop();
    row.request = MenuRequest::None;
    row.requestModel = {};
}

void RequestMenu(Row& row, const F::IInspectable& model, MenuRequest request) {
    row.request = request;
    row.requestModel = winrt::make_weak(model);
    row.deadline = GetTickCount64() + 1800;
}

void UpdateMenuRequest(Row& row) {
    const auto slider = row.slider.get();
    const auto model = row.requestModel.get();
    const bool opening = row.request == MenuRequest::Open;
    if (!slider || !model || !slider.IsLoaded() || Model(slider) != model ||
        (opening && row.button.FocusState() == X::FocusState::Unfocused)) {
        StopRequest(row);
        return;
    }

    const auto generation = endpointGeneration.load();
    auto selection = ReadSelection(model);
    const bool expired = GetTickCount64() >= row.deadline;
    if (generation != endpointGeneration.load() ||
        (!expired && (!selection || selection->names.size() == 1))) {
        if (expired) StopRequest(row);
        return;
    }
    StopRequest(row);
    if (!selection) return;

    if (endpointEnumerator) {
        std::erase_if(outputCache, [&model, generation](const auto& entry) {
            const auto previous = entry.model.get();
            return !previous || previous == model || entry.generation != generation;
        });
        outputCache.push_back({winrt::make_weak(model), generation, *selection});
    }
    if (opening) {
        UpdateMenuIndex(model, *selection, static_cast<uint32_t>(selection->index));
        ShowMenu(row, slider, model, *selection, generation);
    }
}

template <typename Collection, typename Item>
void Remove(const Collection& collection, const Item& item) {
    uint32_t index{};
    if (collection.IndexOf(item, index)) collection.RemoveAt(index);
}

void RemoveRow(Row& row) {
    row.refresh.Stop();
    row.refresh.Tick(row.tick);
    row.button.Click(row.click);
    ClearMenu(row);
    row.menu.Hide();
    if (const auto grid = row.grid.get()) {
        Remove(grid.Children(), row.button);
        Remove(grid.ColumnDefinitions(), row.column);
    }
}

void AddRow(const C::Slider& slider) {
    const auto grid = X::Media::VisualTreeHelper::GetParent(slider).try_as<C::Grid>();
    if (!grid || grid.ColumnDefinitions().Size() == 0) return;
    const auto existing = std::ranges::find_if(rows, [&grid](const auto& row) { return row.grid.get() == grid; });
    if (existing != rows.end()) {
        existing->slider = winrt::make_weak(slider);
        return;
    }
    const auto model = Model(slider);
    if (!model) return;

    C::Button button;
    C::ColumnDefinition column;
    C::MenuFlyout menu;
    X::DispatcherTimer refresh;
    refresh.Interval(std::chrono::milliseconds(150));
    C::FontIcon icon;
    icon.FontFamily(X::Media::FontFamily(L"Segoe Fluent Icons"));
    icon.Glyph(L"\uE70D");
    icon.FontSize(10);
    button.Content(icon);
    button.Padding({3, 0, 3, 0});
    button.MinWidth(24);
    button.MinHeight(24);
    button.Background(X::Media::SolidColorBrush(winrt::Windows::UI::Colors::Transparent()));
    button.BorderThickness({0, 0, 0, 0});
    button.VerticalAlignment(X::VerticalAlignment::Center);
    const wchar_t* title = PRIMARYLANGID(GetUserDefaultUILanguage()) == LANG_CHINESE
                               ? L"输出设备" : L"Output device";
    X::Automation::AutomationProperties::SetName(button, title);
    C::ToolTipService::SetToolTip(button, winrt::box_value(title));
    const auto click = button.Click([](auto&& sender, auto&&) {
        if (stopping) return;
        try {
            const auto clicked = sender.template try_as<C::Button>();
            const auto row = std::ranges::find_if(rows, [&clicked](const auto& candidate) {
                return candidate.button == clicked;
            });
            if (row == rows.end()) return;
            const auto slider = row->slider.get();
            const auto model = slider ? Model(slider) : nullptr;
            for (auto& entry : rows) {
                if (entry.request == MenuRequest::Open) StopRequest(entry);
            }
            StopRequest(*row);
            if (!model) return;

            const auto generation = endpointGeneration.load();
            const auto cached = std::ranges::find_if(outputCache, [&model, generation](const auto& entry) {
                return entry.generation == generation && entry.model.get() == model;
            });
            if (cached != outputCache.end()) {
                const auto change = FindChange(model);
                const bool awaiting = change != changes.end() && GetTickCount64() < change->verifyUntil;
                const auto selected = awaiting ? std::nullopt : ReadSelectedIndex(model);
                if ((awaiting || (selected && *selected < cached->selection.names.size())) &&
                    generation == endpointGeneration.load()) {
                    UpdateMenuIndex(model, cached->selection, selected);
                    Wh_Log(L"Reused cached output list");
                    ShowMenu(*row, slider, model, cached->selection, generation);
                    return;
                }
            }
            RequestMenu(*row, model, MenuRequest::Open);
            row->refresh.Interval(std::chrono::milliseconds(150));
            row->refresh.Start();
        } catch (...) {
            Wh_Log(L"Output menu failed (0x%08X)", static_cast<unsigned>(winrt::to_hresult().value));
        }
    });
    const auto tick = refresh.Tick([](auto&& sender, auto&&) {
        if (stopping) return;
        const auto source = sender.template try_as<X::DispatcherTimer>();
        if (!source) return;
        const auto row = std::ranges::find_if(rows, [&source](const auto& entry) {
            return entry.refresh == source;
        });
        if (row == rows.end() || row->request == MenuRequest::None) return;
        try {
            UpdateMenuRequest(*row);
        } catch (...) {
            StopRequest(*row);
            Wh_Log(L"Output menu refresh failed (0x%08X)", static_cast<unsigned>(winrt::to_hresult().value));
        }
    });

    const auto columns = grid.ColumnDefinitions();
    column.Width({0, X::GridUnitType::Auto});
    C::Grid::SetColumn(button, static_cast<int>(columns.Size()));
    C::Grid::SetRow(button, C::Grid::GetRow(slider));
    rows.push_back({winrt::make_weak(grid), winrt::make_weak(slider), column, button,
                    menu, click, {}, refresh, tick, 0, MenuRequest::None, {}});
    try {
        columns.Append(column);
        grid.Children().Append(button);
        if (endpointEnumerator) {
            const auto generation = endpointGeneration.load();
            if (std::ranges::none_of(outputCache, [&model, generation](const auto& entry) {
                    return entry.generation == generation && entry.model.get() == model;
                })) {
                RequestMenu(rows.back(), model, MenuRequest::Warm);
                refresh.Interval(std::chrono::milliseconds(80 + 30 * std::min<size_t>(rows.size(), 10)));
                refresh.Start();
            }
        }
    } catch (...) {
        RemoveRow(rows.back());
        rows.pop_back();
        throw;
    }
    Wh_Log(L"Added native output menu to AppVolumeLevel row");
}

void Scan(const X::FrameworkElement& mixer) {
    std::erase_if(rows, [](auto& row) {
        if (row.grid.get()) return false;
        RemoveRow(row);
        return true;
    });
    std::vector<X::DependencyObject> pending{mixer};
    for (size_t budget = 1024; budget && !pending.empty(); --budget) {
        auto node = std::move(pending.back());
        pending.pop_back();
        if (const auto slider = node.try_as<C::Slider>(); slider && slider.Name() == L"AppVolumeLevel") {
            AddRow(slider);
        }
        for (int i = X::Media::VisualTreeHelper::GetChildrenCount(node);
             i > 0 && pending.size() < budget; --i) {
            pending.push_back(X::Media::VisualTreeHelper::GetChild(node, i - 1));
        }
    }
}

void Discover(void* object) noexcept {
    if (!object || stopping) return;
    try {
        X::FrameworkElement mixer{nullptr};
        if (FAILED(static_cast<IUnknown*>(object)->QueryInterface(
                winrt::guid_of<X::FrameworkElement>(), winrt::put_abi(mixer))) || !mixer) return;
        {
            std::lock_guard lock(dispatcherLock);
            if (stopping) return;
            if (!dispatcher) dispatcher = mixer.Dispatcher();
            if (!dispatcher || !dispatcher.HasThreadAccess()) return;
        }
        StartEndpointWatch();
        std::erase_if(mixers, [](const auto& entry) { return !entry.element.get(); });
        if (std::ranges::none_of(mixers, [&mixer](const auto& entry) {
                return entry.element.get() == mixer;
            })) {
            const auto weak = winrt::make_weak(mixer);
            const auto event = mixer.LayoutUpdated([weak](auto&&, auto&&) {
                if (stopping) return;
                try {
                    if (const auto current = weak.get(); current && current.IsLoaded()) Scan(current);
                } catch (...) {
                    Wh_Log(L"Mixer layout scan failed (0x%08X)", static_cast<unsigned>(winrt::to_hresult().value));
                }
            });
            mixers.push_back({weak, event});
        }
        Scan(mixer);
    } catch (...) {
        Wh_Log(L"Mixer hook failed (0x%08X)", static_cast<unsigned>(winrt::to_hresult().value));
    }
}

HRESULT WINAPI OnTemplate(void* object) {
    const auto hr = originalTemplate(object);
    if (SUCCEEDED(hr)) Discover(object);
    return hr;
}

HRESULT WINAPI OnFocus(void* object, void* args) {
    const auto hr = originalFocus(object, args);
    if (SUCCEEDED(hr)) Discover(object);
    return hr;
}

void WINAPI OnLoaded(void* object, void* sender, void* args) {
    // The matched MS x64 symbol takes two nontrivial C++/WinRT values by
    // value. The calls pass pointers to their temporaries. Retain the sender
    // before the original can destroy the temporary.
    winrt::com_ptr<IUnknown> retained;
    if (!stopping && sender) retained.copy_from(*static_cast<IUnknown**>(sender));
    originalLoaded(object, sender, args);
    if (retained) Discover(retained.get());
}

void Restore() {
    if (endpointEnumerator) {
        const auto result = endpointEnumerator->UnregisterEndpointNotificationCallback(endpointNotifications.get());
        if (FAILED(result)) {
            Wh_Log(L"Could not unregister endpoint notifications (0x%08X)", static_cast<unsigned>(result));
        }
        endpointEnumerator = nullptr;
        endpointNotifications = nullptr;
    }
    outputCache.clear();
    for (const auto& mixer : mixers) {
        if (const auto element = mixer.element.get()) element.LayoutUpdated(mixer.layout);
    }
    mixers.clear();
    for (auto& row : rows) RemoveRow(row);
    rows.clear();
    for (const auto& change : changes) {
        try {
            const auto model = change.model.get();
            const auto state = ReadSelection(model);
            if (!state || state->names[state->index] != change.applied ||
                !UniqueName(*state, change.original)) continue;
            const auto index = std::ranges::find(state->names, change.original) - state->names.begin();
            Property(model, L"SelectedOutput").SetValue(model, winrt::box_value(static_cast<int32_t>(index)));
        } catch (...) {
            Wh_Log(L"Output restoration failed (0x%08X)", static_cast<unsigned>(winrt::to_hresult().value));
        }
    }
    changes.clear();
}

}  // namespace

BOOL Wh_ModInit() {
    const auto module = GetModuleHandleW(L"ControlCenter.dll");
    if (!module) return FALSE;
    const WH_FIND_SYMBOL_OPTIONS options{sizeof(WH_FIND_SYMBOL_OPTIONS), nullptr, TRUE};
    WH_FIND_SYMBOL symbol{};
    const auto search = Wh_FindFirstSymbol(module, &options, &symbol);
    if (!search) return FALSE;
    void* applyTemplate{};
    void* focus{};
    void* loaded{};
    do {
        if (!symbol.symbolDecorated) continue;
        const std::wstring_view name(symbol.symbolDecorated);
        if (name == templateSymbol) applyTemplate = symbol.address;
        if (name == focusSymbol) focus = symbol.address;
        if (name == loadedSymbol) loaded = symbol.address;
        if (applyTemplate && focus && loaded) break;
    } while (Wh_FindNextSymbol(search, &symbol));
    Wh_FindCloseSymbol(search);
    if (!applyTemplate || !focus || !loaded) return FALSE;
    return Wh_SetFunctionHook(applyTemplate, reinterpret_cast<void*>(OnTemplate),
                              reinterpret_cast<void**>(&originalTemplate)) &&
           Wh_SetFunctionHook(focus, reinterpret_cast<void*>(OnFocus),
                              reinterpret_cast<void**>(&originalFocus)) &&
           Wh_SetFunctionHook(loaded, reinterpret_cast<void*>(OnLoaded),
                              reinterpret_cast<void**>(&originalLoaded));
}

void Wh_ModBeforeUninit() {
    stopping = true;
    Core::CoreDispatcher ui{nullptr};
    {
        std::lock_guard lock(dispatcherLock);
        ui = dispatcher;
    }
    if (!ui) return;
    try {
        if (ui.HasThreadAccess()) {
            Restore();
        } else {
            ui.RunAsync(Core::CoreDispatcherPriority::High, [] { Restore(); }).get();
        }
    } catch (...) {
        Wh_Log(L"Mixer teardown failed (0x%08X)", static_cast<unsigned>(winrt::to_hresult().value));
    }
}
