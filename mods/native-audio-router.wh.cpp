// ==WindhawkMod==
// @id native-audio-router
// @name Native Audio Router
// @description Choose a Windows output device from each Quick Settings mixer row
// @description:zh-CN 在任务栏原生音量合成器中选择逐应用输出设备
// @version 0.0.7
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
Each pick is the same per-application output setting as Settings > System >
Sound > Volume mixer: Windows saves it for the current user, and it keeps
working after the mod is disabled or removed.

The mod does not route audio and keeps no copy of the selection. It only adds
a shortcut to the Windows setting, so there is nothing to restore on unload.
To change a choice, use the same arrow or the Volume mixer page; choose
"Default" there to follow the system default again. Apps that retain their
original audio stream may need playback restarted.

![Output device menu in Windows 11 Quick Settings](https://raw.githubusercontent.com/KKKKeybird/native-audio-router/main/docs/images/taskbar-output-menu.png)

Tested on Windows 11 x64 build 26300.9974 with Windhawk 1.7.3. Windhawk also
builds this mod for ARM64, but that build has not been run or verified. Not
tested together with Refined Quick Settings and Sound, which changes the same
volume rows.

## 中文

在任务栏快捷设置的「声音输出」页中，为应用音量行加入设备菜单。通过行尾
箭头选择设备或跟随系统默认。每次选择写入的就是 Windows 自身的逐应用输出
设置（与「设置 > 系统 > 声音 > 音量合成器」相同），由 Windows 按当前用户
保存，模块停用或移除后仍然有效。

模块不接管音频，也不另存一份选择状态，只是为 Windows 设置提供一个等价的
快捷入口，因此卸载时没有需要还原的内容。要修改选择，用同一个箭头，或到
「设置 > 系统 > 声音 > 音量合成器」操作；在那里选择「默认」即可重新跟随
系统默认。部分应用需要重新开始播放，才能切换现有音频流。

已在 Windows 11 x64 构建 26300.9974 与 Windhawk 1.7.3 上测试。Windhawk 也会
为 ARM64 原生构建本模块，但尚未在 ARM64 上运行或验证。未与 Refined Quick
Settings and Sound 联合测试，该模块会修改同一批音量行。

See the [project repository](https://github.com/KKKKeybird/native-audio-router)
for source and test status.
*/
// ==/WindhawkModReadme==

#include <windows.h>
#include <unknwn.h>
#include <mmdeviceapi.h>
#include <windhawk_utils.h>

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

HRESULT (WINAPI* originalTemplate)(void*);
HRESULT (WINAPI* originalFocus)(void*, void*);
void (WINAPI* originalLoaded)(void*, void*, void*);
decltype(&LoadLibraryExW) originalLoadLibraryExW;
std::atomic_bool stopping{};
std::atomic_bool watchReady{};
std::atomic_bool hookAttempted{};
std::mutex hookInstallLock;
std::mutex dispatcherLock;
[[clang::no_destroy]] Core::CoreDispatcher dispatcher{nullptr};
std::atomic_uint64_t endpointGeneration{1};
[[clang::no_destroy]] winrt::com_ptr<IMMDeviceEnumerator> endpointEnumerator;
[[clang::no_destroy]] winrt::com_ptr<IMMNotificationClient> endpointNotifications;
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
    std::vector<std::pair<winrt::weak_ref<C::ListViewBase>, winrt::event_token>> containerEvents;
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

struct Pending {
    winrt::weak_ref<F::IInspectable> model;
    winrt::hstring applied;
    ULONGLONG verifyUntil{};
};

[[clang::no_destroy]] std::vector<Row> rows;
std::vector<Mixer> mixers;
std::vector<Pending> pendingSelections;
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

auto FindPending(const F::IInspectable& model) {
    const auto now = GetTickCount64();
    std::erase_if(pendingSelections, [now](const auto& entry) {
        return !entry.model.get() || (entry.verifyUntil && now >= entry.verifyUntil);
    });
    return std::ranges::find_if(pendingSelections, [&model](const auto& entry) {
        return entry.model.get() == model;
    });
}

void UpdateMenuIndex(const F::IInspectable& model, Selection& selection,
                     std::optional<uint32_t> reported) {
    const auto pending = FindPending(model);
    if (pending != pendingSelections.end() && GetTickCount64() < pending->verifyUntil) {
        const auto requested = std::ranges::find(selection.names, pending->applied);
        if (requested != selection.names.end() && UniqueName(selection, pending->applied)) {
            const auto index = static_cast<uint32_t>(requested - selection.names.begin());
            if (reported && *reported == index) pending->verifyUntil = 0;
            selection.index = static_cast<int32_t>(index);
            return;
        }
    }
    if (reported && *reported < selection.names.size()) selection.index = static_cast<int32_t>(*reported);
}

void Select(const F::IInspectable& model, const Selection& shown, int32_t index) {
    const auto current = ReadSelection(model);
    if (!current) return;
    if (current->names != shown.names) {
        std::erase_if(outputCache, [&model](const auto& entry) { return entry.model.get() == model; });
        return;
    }
    if (!UniqueName(*current, current->names[index]) ||
        !UniqueName(*current, current->names[current->index])) {
        return;
    }
    auto pending = FindPending(model);
    const auto now = GetTickCount64();
    const auto& reported = current->names[current->index];
    const auto& requested = current->names[index];
    const bool awaiting = pending != pendingSelections.end() && now < pending->verifyUntil;
    if (awaiting ? pending->applied == requested : current->index == index) return;
    const auto property = Property(model, L"SelectedOutput");
    if (!property) return;
    winrt::hstring previous;
    const bool added = pending == pendingSelections.end();
    if (added) {
        pendingSelections.push_back({winrt::make_weak(model), requested, 0});
        pending = pendingSelections.end() - 1;
    } else {
        previous = pending->applied;
        pending->applied = requested;
    }
    try {
        property.SetValue(model, winrt::box_value(static_cast<uint32_t>(index)));
    } catch (...) {
        if (added) {
            pendingSelections.pop_back();
        } else {
            pending->applied = previous;
        }
        throw;
    }
    pending->verifyUntil = GetTickCount64() + 1500;
    Wh_Log(L"Requested native output index %d", index);
}

void ClearMenu(Row& row) {
    for (const auto& [item, token] : row.itemClicks) item.Click(token);
    row.itemClicks.clear();
    row.menu.Items().Clear();
}

void ShowMenu(Row& row, const C::Slider& slider, const F::IInspectable& model,
              const Selection& shown) {
    ClearMenu(row);
    const auto& menu = row.menu;
    for (int32_t i = 0; i < static_cast<int32_t>(shown.names.size()); ++i) {
        C::ToggleMenuFlyoutItem item;
        item.Text(shown.names[i]);
        item.IsChecked(i == shown.index);
        item.IsEnabled(UniqueName(shown, shown.names[i]));
        const auto token = item.Click([weakSlider = winrt::make_weak(slider), weakModel = winrt::make_weak(model),
                                       snapshot = shown, i](auto&&, auto&&) {
            if (stopping) return;
            try {
                const auto currentSlider = weakSlider.get();
                const auto currentModel = currentSlider ? Model(currentSlider) : nullptr;
                if (currentModel && currentModel == weakModel.get()) {
                    Select(currentModel, snapshot, i);
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
    if (!slider || !model ||
        (opening && row.button.FocusState() == X::FocusState::Unfocused)) {
        StopRequest(row);
        return;
    }
    // A template hook can find the row before XAML finishes loading it.
    if (!slider.IsLoaded()) {
        if (GetTickCount64() >= row.deadline) StopRequest(row);
        return;
    }
    const auto currentModel = Model(slider);
    if (currentModel != model) {
        if (currentModel || GetTickCount64() >= row.deadline) StopRequest(row);
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
        ShowMenu(row, slider, model, *selection);
    }
}

template <typename Collection, typename Item>
void Remove(const Collection& collection, const Item& item) {
    uint32_t index{};
    if (collection.IndexOf(item, index)) collection.RemoveAt(index);
}

void RemoveRow(Row& row) {
    row.refresh.Tick(row.tick);
    row.button.Click(row.click);
    try {
        row.refresh.Stop();
        ClearMenu(row);
        row.menu.Hide();
    } catch (...) {
        Wh_Log(L"Detached row menu could not be closed (0x%08X)",
               static_cast<unsigned>(winrt::to_hresult().value));
    }
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
                const auto pending = FindPending(model);
                const bool awaiting = pending != pendingSelections.end() && GetTickCount64() < pending->verifyUntil;
                const auto selected = awaiting ? std::nullopt : ReadSelectedIndex(model);
                if ((awaiting || (selected && *selected < cached->selection.names.size())) &&
                    generation == endpointGeneration.load()) {
                    UpdateMenuIndex(model, cached->selection, selected);
                    Wh_Log(L"Reused cached output list");
                    ShowMenu(*row, slider, model, cached->selection);
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
    // A XAML change made during discovery can synchronously raise focus.
    static thread_local bool scanning = false;
    if (scanning) return;
    scanning = true;
    struct ResetScan {
        bool& active;
        ~ResetScan() { active = false; }
    } reset{scanning};

    std::erase_if(rows, [](auto& row) {
        if (row.grid.get()) return false;
        RemoveRow(row);
        return true;
    });
    std::vector<X::DependencyObject> pending{mixer};
    for (size_t budget = 1024; budget && !pending.empty(); --budget) {
        auto node = std::move(pending.back());
        pending.pop_back();
        if (const auto list = node.try_as<C::ListViewBase>(); list) {
            const auto watchedMixer = std::ranges::find_if(mixers, [&mixer](const auto& entry) {
                return entry.element.get() == mixer;
            });
            if (watchedMixer != mixers.end() &&
                std::ranges::none_of(watchedMixer->containerEvents, [&list](const auto& entry) {
                    return entry.first.get() == list;
                })) {
                const auto weakMixer = winrt::make_weak(mixer);
                const auto token = list.ContainerContentChanging([weakMixer](auto&&, auto&&) {
                    if (stopping) return;
                    try {
                        if (const auto current = weakMixer.get(); current && current.IsLoaded()) Scan(current);
                    } catch (...) {
                        Wh_Log(L"Mixer container scan failed (0x%08X)",
                               static_cast<unsigned>(winrt::to_hresult().value));
                    }
                });
                watchedMixer->containerEvents.emplace_back(winrt::make_weak(list), token);
            }
        }
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
        std::erase_if(mixers, [](auto& entry) {
            if (entry.element.get()) return false;
            for (const auto& [weakList, token] : entry.containerEvents) {
                if (const auto list = weakList.get()) list.ContainerContentChanging(token);
            }
            return true;
        });
        if (std::ranges::none_of(mixers, [&mixer](const auto& entry) {
                return entry.element.get() == mixer;
            })) {
            mixers.push_back({winrt::make_weak(mixer), {}});
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

void Teardown() {
    if (endpointEnumerator) {
        const auto result = endpointEnumerator->UnregisterEndpointNotificationCallback(endpointNotifications.get());
        if (FAILED(result)) {
            Wh_Log(L"Could not unregister endpoint notifications (0x%08X)", static_cast<unsigned>(result));
        }
        endpointEnumerator = nullptr;
        endpointNotifications = nullptr;
    }
    std::vector<CachedOutputs>().swap(outputCache);
    for (auto& mixer : mixers) {
        for (const auto& [weakList, token] : mixer.containerEvents) {
            if (const auto list = weakList.get()) list.ContainerContentChanging(token);
        }
        std::vector<std::pair<winrt::weak_ref<C::ListViewBase>, winrt::event_token>>().swap(
            mixer.containerEvents);
    }
    std::vector<Mixer>().swap(mixers);
    for (auto& row : rows) {
        try {
            RemoveRow(row);
        } catch (...) {
            Wh_Log(L"Could not detach a mixer row (0x%08X)",
                   static_cast<unsigned>(winrt::to_hresult().value));
        }
    }
    std::vector<Row>().swap(rows);
    std::vector<Pending>().swap(pendingSelections);
    std::lock_guard lock(dispatcherLock);
    dispatcher = nullptr;
}

bool HookControlCenter(HMODULE module) {
    const WindhawkUtils::SYMBOL_HOOK controlCenterDllHooks[]{
        {{LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::ControlCenter::implementation::VolumeMixerList,struct winrt::Windows::UI::Xaml::IFrameworkElementOverrides>::OnApplyTemplate(void))"},
         &originalTemplate, OnTemplate, true},
        {{LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::ControlCenter::implementation::VolumeMixerList,struct winrt::Windows::UI::Xaml::Controls::IControlOverrides>::OnGotFocus(void *))"},
         &originalFocus, OnFocus, true},
        {{LR"(public: void __cdecl winrt::ControlCenter::implementation::VolumeMixerList::OnVolumeMixerListLoaded(struct winrt::Windows::Foundation::IInspectable,struct winrt::Windows::UI::Xaml::RoutedEventArgs))"},
         &originalLoaded, OnLoaded},
    };
    const bool resolved = WindhawkUtils::HookSymbols(module, controlCenterDllHooks,
                                                    ARRAYSIZE(controlCenterDllHooks));
    Wh_Log(L"ControlCenter mixer hooks resolved: %d", resolved);
    return resolved;
}

void TryLateHook() noexcept {
    if (!watchReady || stopping || hookAttempted) return;
    const auto module = GetModuleHandleW(L"ControlCenter.dll");
    if (!module || hookAttempted.exchange(true)) return;
    try {
        std::lock_guard lock(hookInstallLock);
        if (stopping) return;
        if (HookControlCenter(module) && !Wh_ApplyHookOperations()) {
            Wh_Log(L"Could not apply late ControlCenter hooks");
        }
    } catch (...) {
        Wh_Log(L"Late ControlCenter initialization failed (0x%08X)",
               static_cast<unsigned>(winrt::to_hresult().value));
    }
}

HMODULE WINAPI OnLoadLibraryExW(LPCWSTR fileName, HANDLE file, DWORD flags) {
    const auto result = originalLoadLibraryExW(fileName, file, flags);
    if (result) {
        const auto error = GetLastError();
        TryLateHook();
        SetLastError(error);
    }
    return result;
}

}  // namespace

BOOL Wh_ModInit() {
    if (const auto module = GetModuleHandleW(L"ControlCenter.dll")) {
        hookAttempted = true;
        return HookControlCenter(module);
    }
    const auto kernelBase = GetModuleHandleW(L"kernelbase.dll");
    if (!kernelBase) return FALSE;
    const auto load = reinterpret_cast<decltype(originalLoadLibraryExW)>(
        GetProcAddress(kernelBase, "LoadLibraryExW"));
    return load && WindhawkUtils::SetFunctionHook(load, OnLoadLibraryExW, &originalLoadLibraryExW);
}

void Wh_ModAfterInit() {
    watchReady = true;
    TryLateHook();
}

void Wh_ModBeforeUninit() {
    stopping = true;
    {
        std::lock_guard lock(hookInstallLock);
    }
    Core::CoreDispatcher ui{nullptr};
    {
        std::lock_guard lock(dispatcherLock);
        ui = dispatcher;
    }
    if (!ui) return;
    try {
        if (ui.HasThreadAccess()) {
            Teardown();
        } else {
            ui.RunAsync(Core::CoreDispatcherPriority::High, [] { Teardown(); }).get();
        }
    } catch (...) {
        Wh_Log(L"Mixer teardown failed (0x%08X)", static_cast<unsigned>(winrt::to_hresult().value));
    }
}
