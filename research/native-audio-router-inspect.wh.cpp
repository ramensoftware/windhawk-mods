// ==WindhawkMod==
// @id native-audio-router-inspect
// @name Native Audio Router mixer inspection
// @description Inspect the native mixer row model when the mixer receives keyboard focus
// @version 0.0.1
// @include ShellHost.exe
// @architecture x86-64
// @compilerOptions -lruntimeobject -loleaut32
// @license MIT
// ==/WindhawkMod==

#include <windows.h>
#include <unknwn.h>
#undef GetCurrentTime
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Data.h>
#include <winrt/Windows.UI.Xaml.Markup.h>
#include <winrt/Windows.UI.Xaml.Media.h>

#include <string_view>
#include <vector>

namespace {

namespace Xaml = winrt::Windows::UI::Xaml;
namespace Controls = Xaml::Controls;
using winrt::Windows::Foundation::IInspectable;

// Matched against the PDB for the local ControlCenter.dll. The decorated
// symbol encodes the target class, projected interface and x64 signature.
constexpr wchar_t focusSymbol[] =
    L"?OnGotFocus@?$produce@UVolumeMixerList@implementation@ControlCenter@winrt@@"
    L"UIControlOverrides@Controls@Xaml@UI@Windows@4@@impl@winrt@@UEAAHPEAX@Z";
using FocusMethod = HRESULT (WINAPI*)(void*, void*);
FocusMethod originalFocus{};

IInspectable RowModel(const Controls::Slider& slider) {
    auto node = Xaml::Media::VisualTreeHelper::GetParent(slider);
    Controls::ListViewItem container{nullptr};
    while (node) {
        if (auto item = node.try_as<Controls::ListViewItem>(); item && !container) {
            container = item;
        }
        if (auto list = node.try_as<Controls::ListViewBase>(); list && container) {
            auto model = list.ItemFromContainer(container);
            if (model && model != Xaml::DependencyProperty::UnsetValue()) return model;
            break;
        }
        node = Xaml::Media::VisualTreeHelper::GetParent(node);
    }
    return slider.DataContext();
}

IInspectable ReadProperty(const IInspectable& model, const wchar_t* name) {
    if (auto custom = model.try_as<Xaml::Data::ICustomPropertyProvider>()) {
        auto property = custom.GetCustomProperty(name);
        if (property && property.CanRead()) return property.GetValue(model);
    }
    const auto provider = Xaml::Application::Current().try_as<Xaml::Markup::IXamlMetadataProvider>();
    if (!provider) return nullptr;
    const auto type = provider.GetXamlType(winrt::get_class_name(model));
    if (const auto member = type ? type.GetMember(name) : nullptr) return member.GetValue(model);
    return nullptr;
}

void DescribeModel(const IInspectable& model) {
    if (!model) {
        Wh_Log(L"AppVolumeLevel row model unavailable");
        return;
    }
    Wh_Log(L"AppVolumeLevel model class: %ls", winrt::get_class_name(model).c_str());
    // Some model wrappers advertise arbitrary custom properties. Verify that
    // each getter returns a value, while keeping the values out of the log.
    for (const auto name : {L"AppName", L"Id", L"ProcessId", L"SessionIdentifier",
                            L"SessionInstanceIdentifier", L"AppUserModelId",
                            L"SelectedOutput", L"PossibleOutputs"}) {
        try {
            const auto value = ReadProperty(model, name);
            if (const auto scalar = value.try_as<winrt::Windows::Foundation::IPropertyValue>()) {
                Wh_Log(L"Model property %ls, WinRT type %d", name, static_cast<int>(scalar.Type()));
            } else if (value) {
                Wh_Log(L"Model property %ls, runtime class %ls", name,
                       winrt::get_class_name(value).c_str());
            }
        } catch (...) {
            Wh_Log(L"Model property %ls unavailable (0x%08X)", name,
                   static_cast<unsigned>(winrt::to_hresult().value));
        }
    }
}

void ReportMixer(const Xaml::DependencyObject& mixer) {
    std::vector<Xaml::DependencyObject> pending{mixer};
    unsigned rows = 0;
    for (unsigned budget = 1024; budget && !pending.empty() && rows < 12; --budget) {
        auto node = std::move(pending.back());
        pending.pop_back();
        if (const auto slider = node.try_as<Controls::Slider>();
            slider && slider.Name() == L"AppVolumeLevel") {
            ++rows;
            DescribeModel(RowModel(slider));
        }
        const int count = Xaml::Media::VisualTreeHelper::GetChildrenCount(node);
        for (int i = count; i > 0 && pending.size() < budget; --i) {
            pending.push_back(Xaml::Media::VisualTreeHelper::GetChild(node, i - 1));
        }
    }
    Wh_Log(L"VolumeMixerList focus, AppVolumeLevel rows inspected: %u", rows);
}

HRESULT WINAPI OnMixerFocus(void* object, void* args) {
    const auto result = originalFocus(object, args);
    if (!object || FAILED(result)) return result;
    // The hook runs on the XAML thread. No subscriptions or dispatcher work
    // survive the call, so Windhawk's regular hook teardown is sufficient.
    static thread_local ULONGLONG lastReport{};
    const auto now = GetTickCount64();
    if (lastReport && now - lastReport < 500) return result;
    lastReport = now;
    try {
        Xaml::FrameworkElement mixer{nullptr};
        if (SUCCEEDED(static_cast<IUnknown*>(object)->QueryInterface(
                winrt::guid_of<Xaml::FrameworkElement>(), winrt::put_abi(mixer))) && mixer) {
            ReportMixer(mixer);
        }
    } catch (...) {
        Wh_Log(L"Mixer inspection failed: 0x%08X", static_cast<unsigned>(winrt::to_hresult().value));
    }
    return result;
}

}  // namespace

BOOL Wh_ModInit() {
    const auto module = GetModuleHandleW(L"ControlCenter.dll");
    if (!module) {
        Wh_Log(L"ControlCenter.dll is not loaded. Open Quick Settings before enabling this diagnostic.");
        return FALSE;
    }
    const WH_FIND_SYMBOL_OPTIONS options{sizeof(WH_FIND_SYMBOL_OPTIONS), nullptr, TRUE};
    WH_FIND_SYMBOL symbol{};
    const auto search = Wh_FindFirstSymbol(module, &options, &symbol);
    if (!search) return FALSE;
    void* address = nullptr;
    do {
        if (symbol.symbolDecorated && std::wstring_view(symbol.symbolDecorated) == focusSymbol) {
            address = symbol.address;
            break;
        }
    } while (Wh_FindNextSymbol(search, &symbol));
    Wh_FindCloseSymbol(search);
    if (!address) {
        Wh_Log(L"Exact VolumeMixerList focus symbol unavailable");
        return FALSE;
    }
    return Wh_SetFunctionHook(address, reinterpret_cast<void*>(OnMixerFocus),
                              reinterpret_cast<void**>(&originalFocus));
}
