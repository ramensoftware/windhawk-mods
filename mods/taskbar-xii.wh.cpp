// ==WindhawkMod==
// @id              taskbar-xii
// @name            Taskbar XII
// @description     ✦ Windows 12-inspired redesign
// @version         4.0.0
// @author          ryokr
// @github          https://github.com/ryokr
// @homepage        https://ryokr.github.io
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lcomctl32 -lole32 -loleaut32 -lruntimeobject -Wl,--export-all-symbols
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
✦ Set display scale to 100% for optimal visuals.  
✦ Enhance experience with the Start11 app.

![demonstration](https://raw.githubusercontent.com/ryokr/TaskbarXII/main/Assets/preview.png)
*/
// ==/WindhawkModReadme==

#undef GetCurrentTime

#include <xamlom.h>
#include <atomic>
#include <vector>
#include <winrt/Windows.UI.Xaml.h>

struct ThemeTargetStyles { PCWSTR target; std::vector<PCWSTR> styles; };
struct Theme { std::vector<ThemeTargetStyles> targetStyles; };

// const PCWSTR WeatherImage = L"Background:=<ImageBrush Stretch=\"UniformToFill\" ImageSource=\"Path\" />";
// const PCWSTR TaskbarBG = L"Background:=<AcrylicBrush TintColor=\"{ThemeResource SystemChromeAltHighColor}\" TintOpacity=\"0.6\" />";
const PCWSTR TaskbarBG = L"Background:=<SolidColorBrush Color=\"{ThemeResource SystemChromeAltHighColor}\" Opacity=\"0.6\" />";

const PCWSTR HorizontalAlignRight = L"HorizontalAlignment=Right";
const PCWSTR HorizontalAlignLeft = L"HorizontalAlignment=Left";

const PCWSTR RoundAllCorner = L"CornerRadius=4";
const PCWSTR NormalHeight = L"Height=48";

const PCWSTR WeatherIconHeight = L"MaxHeight=27";
const PCWSTR WeatherIconWidth = L"MaxWidth=27";

const PCWSTR TaskbarBGTransform = L"Transform3D:=<CompositeTransform3D TranslateX=\"156.5\"/>";

const PCWSTR TaskbarTransform = L"Transform3D:=<CompositeTransform3D TranslateX=\"-820\"/>";        // Right
const PCWSTR SystemTrayTransform = L"Transform3D:=<CompositeTransform3D TranslateX=\"1104.5\"/>";   // Left  

const PCWSTR TimeTransform = L"Transform3D:=<CompositeTransform3D TranslateY=\"10\"/>";
const PCWSTR DateTransform = L"Transform3D:=<CompositeTransform3D TranslateY=\"-10\"/>";
// -------------------------------------------------------------------------------------------------------------
const Theme themeTaskbarXII = {{
    ThemeTargetStyles{L"Taskbar.TaskbarFrame#TaskbarFrame", { HorizontalAlignRight, TaskbarTransform, L"Width=Auto", L"Height=56" }},
    ThemeTargetStyles{L"Taskbar.TaskbarFrame#TaskbarFrame > Grid", { NormalHeight, RoundAllCorner }}, // right round main section

    ThemeTargetStyles{L"Taskbar.TaskbarBackground#BackgroundControl", { NormalHeight, TaskbarBGTransform, L"Opacity=0.7" }},
    ThemeTargetStyles{L"Taskbar.TaskbarBackground > Grid", { RoundAllCorner, L"Opacity=1" }}, // left round main section
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.ItemsRepeater#TaskbarFrameRepeater", { L"Margin=0,0,3,0" }}, // main section right margin

    ThemeTargetStyles{L"Taskbar.SearchBoxButton > Taskbar.TaskListButtonPanel", { L"Margin=2,0,6,0" }},
    ThemeTargetStyles{L"Taskbar.SearchBoxButton > Taskbar.TaskListButtonPanel > TextBlock", { L"Text=✦ Meow" }}, // searchbox text

    ThemeTargetStyles{L"Windows.UI.Xaml.Shapes.Rectangle#BackgroundStroke", { L"Visibility=Collapsed" }}, // top line in main section

    // -------------------------------------------------------------------------------------------------------------
    
    ThemeTargetStyles{L"Taskbar.AugmentedEntryPointButton > Taskbar.TaskListButtonPanel", { TaskbarBG, RoundAllCorner, L"Padding=0", L"Margin=0,0,7,0" }},
    ThemeTargetStyles{L"Taskbar.AugmentedEntryPointButton > Taskbar.TaskListButtonPanel > Grid", { L"Margin=8,0,0,0" }},

    // ThemeTargetStyles{L"Taskbar.AugmentedEntryPointButton > Taskbar.TaskListButtonPanel", { WeatherImage, RoundAllCorner, L"Padding=0", L"Margin=0,0,7,0" }},        
    // ThemeTargetStyles{L"Taskbar.AugmentedEntryPointButton > Taskbar.TaskListButtonPanel > Grid", { L"Visibility=Collapsed" }},

    ThemeTargetStyles{L"Border#LargeTicker1", { L"Margin=0,2,4,0" }},
    ThemeTargetStyles{L"Border#LargeTicker1 > AdaptiveCards.Rendering.Uwp.WholeItemsPanel > Image", { WeatherIconHeight, WeatherIconWidth }},                                           // weather icon
    ThemeTargetStyles{L"Border#LargeTicker1 > AdaptiveCards.Rendering.Uwp.WholeItemsPanel > Microsoft.UI.Xaml.Controls.AnimatedVisualPlayer", { WeatherIconHeight, WeatherIconWidth }}, // weather icon

    // -------------------------------------------------------------------------------------------------------------
    
    ThemeTargetStyles{L"SystemTray.SystemTrayFrame", { HorizontalAlignLeft, SystemTrayTransform }},
    ThemeTargetStyles{L"Grid#SystemTrayFrameGrid", { TaskbarBG, RoundAllCorner, L"Padding=8,3,0,3" }},

    // -------------------------------------------------------------------------------------------------------------
    
    // ThemeTargetStyles{L"SystemTray.Stack#NotifyIconStack", { L"Grid.Column=0" }}, // systemtray / order 0
    // ThemeTargetStyles{L"SystemTray.NotificationAreaIcons#NotificationAreaIcons", { L"Grid.Column=1" }}, // restart icon / order 1
    // ThemeTargetStyles{L"SystemTray.Stack#NonActivatableStack", { L"Grid.Column=3" }}, // mic icon / order 3
    ThemeTargetStyles{L"SystemTray.Stack#SecondaryClockStack", { L"Grid.Column=8" }}, // mic icon / order 4
    ThemeTargetStyles{L"SystemTray.OmniButton#ControlCenterButton", { L"Grid.Column=4" }}, // mic icon / order 5
    ThemeTargetStyles{L"SystemTray.OmniButton#NotificationCenterButton", { L"Grid.Column=5" }}, // mic icon / order 6
    ThemeTargetStyles{L"SystemTray.Stack#MainStack", { L"Grid.Column=6" }}, // mic icon / order 2
    ThemeTargetStyles{L"SystemTray.Stack#ShowDesktopStack", { L"Grid.Column=7"  }}, // hide window / order 7

    // -------------------------------------------------------------------------------------------------------------

    ThemeTargetStyles{L"TextBlock#InnerTextBlock[Text=\uE971]", { L"Text=\uED14" }}, // uE712-dots uE878-triangle uED14-QR

    ThemeTargetStyles{L"TextBlock#TimeInnerTextBlock", { TimeTransform, L"FontSize=15", L"FontWeight=Bold", L"Margin=94,0,0,0" }},
    ThemeTargetStyles{L"TextBlock#DateInnerTextBlock", { DateTransform, L"FontSize=15", L"FontWeight=SemiBold", HorizontalAlignLeft }},
}};
// -------------------------------------------------------------------------------------------------------------

std::atomic<DWORD> g_targetThreadId = 0;

void ApplyCustomizations(InstanceHandle handle, winrt::Windows::UI::Xaml::FrameworkElement element, PCWSTR fallbackClassName);
void CleanupCustomizations(InstanceHandle handle);

HMODULE GetCurrentModuleHandle() {
    HMODULE module;
    if (!GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, L"", &module)) return nullptr;
    return module;
}

// -------------------------------------------------------------------------------------------------------------
#pragma region winrt_hpp

#include <guiddef.h>
#include <Unknwn.h>
#include <winrt/base.h>

namespace winrt {
    namespace Windows {
        namespace Foundation::Collections {}
        namespace UI::Xaml {
            namespace Controls {}
            namespace Hosting {}
        }
    }
}
// -------------------------------------------------------------------------------------------------------------
#pragma region visualtreewatcher_hpp

#include <winrt/Windows.UI.Xaml.h>

class VisualTreeWatcher : public winrt::implements<VisualTreeWatcher, IVisualTreeServiceCallback2, winrt::non_agile> {
    public:
        VisualTreeWatcher(winrt::com_ptr<IUnknown> site);

        VisualTreeWatcher(const VisualTreeWatcher&) = delete;
        VisualTreeWatcher& operator=(const VisualTreeWatcher&) = delete;

        VisualTreeWatcher(VisualTreeWatcher&&) = delete;
        VisualTreeWatcher& operator=(VisualTreeWatcher&&) = delete;

        void UnadviseVisualTreeChange();

    private:
        HRESULT STDMETHODCALLTYPE OnVisualTreeChange(ParentChildRelation relation, VisualElement element, VisualMutationType mutationType) override;
        HRESULT STDMETHODCALLTYPE OnElementStateChanged(InstanceHandle element, VisualElementState elementState, LPCWSTR context) noexcept override;

        template<typename T>
        T FromHandle(InstanceHandle handle) {
            winrt::Windows::Foundation::IInspectable obj;
            winrt::check_hresult(m_XamlDiagnostics->GetIInspectableFromHandle(handle, reinterpret_cast<::IInspectable**>(winrt::put_abi(obj))));

            return obj.as<T>();
        }

        winrt::com_ptr<IXamlDiagnostics> m_XamlDiagnostics = nullptr;
};
// -------------------------------------------------------------------------------------------------------------
#pragma region visualtreewatcher_cpp

#include <winrt/Windows.UI.Xaml.Hosting.h>

VisualTreeWatcher::VisualTreeWatcher(winrt::com_ptr<IUnknown> site) : m_XamlDiagnostics(site.as<IXamlDiagnostics>()) {
    winrt::check_hresult(m_XamlDiagnostics.as<IVisualTreeService3>()->AdviseVisualTreeChange(this));
}

void VisualTreeWatcher::UnadviseVisualTreeChange() {
    winrt::check_hresult(m_XamlDiagnostics.as<IVisualTreeService3>()->UnadviseVisualTreeChange(this));
}

HRESULT VisualTreeWatcher::OnVisualTreeChange(ParentChildRelation, VisualElement element, VisualMutationType mutationType) try {
    if (GetCurrentThreadId() != g_targetThreadId) return S_OK;

    if (mutationType == Add) {
        const auto inspectable = FromHandle<winrt::Windows::Foundation::IInspectable>(element.Handle);

        auto frameworkElement = inspectable.try_as<winrt::Windows::UI::Xaml::FrameworkElement>();
        if (!frameworkElement) {
            const auto desktopXamlSource = FromHandle<winrt::Windows::UI::Xaml::Hosting::DesktopWindowXamlSource>(element.Handle);
            frameworkElement = desktopXamlSource.Content().try_as<winrt::Windows::UI::Xaml::FrameworkElement>();
        }

        if (frameworkElement) { ApplyCustomizations(element.Handle, frameworkElement, element.Type); }
    }
    else if (mutationType == Remove) CleanupCustomizations(element.Handle);

    return S_OK;
} catch (...) {
    winrt::to_hresult();
    return S_OK;
}

HRESULT VisualTreeWatcher::OnElementStateChanged(InstanceHandle, VisualElementState, LPCWSTR) noexcept { return S_OK; }
// -------------------------------------------------------------------------------------------------------------
#pragma region tap_hpp

#include <ocidl.h>

winrt::com_ptr<VisualTreeWatcher> g_visualTreeWatcher;
static constexpr CLSID CLSID_WindhawkTAP = { 0xc85d8cc7, 0x5463, 0x40e8, { 0xa4, 0x32, 0xf5, 0x91, 0x6b, 0x64, 0x27, 0xe5 } }; // {C85D8CC7-5463-40E8-A432-F5916B6427E5}

class WindhawkTAP : public winrt::implements<WindhawkTAP, IObjectWithSite, winrt::non_agile> {
    public:
        HRESULT STDMETHODCALLTYPE SetSite(IUnknown *pUnkSite) override;
        HRESULT STDMETHODCALLTYPE GetSite(REFIID riid, void **ppvSite) noexcept override;

    private:
        winrt::com_ptr<IUnknown> site;
};
// -------------------------------------------------------------------------------------------------------------
#pragma region tap_cpp

HRESULT WindhawkTAP::SetSite(IUnknown *pUnkSite) try {
    if (g_visualTreeWatcher) {
        g_visualTreeWatcher->UnadviseVisualTreeChange();
        g_visualTreeWatcher = nullptr;
    }

    site.copy_from(pUnkSite);

    if (site) {
        FreeLibrary(GetCurrentModuleHandle());
        g_visualTreeWatcher = winrt::make_self<VisualTreeWatcher>(site);
    }
    return S_OK;
}
catch (...) { return winrt::to_hresult(); }

HRESULT WindhawkTAP::GetSite(REFIID riid, void **ppvSite) noexcept { return site.as(riid, ppvSite); }
// -------------------------------------------------------------------------------------------------------------
#pragma region simplefactory_hpp

#include <Unknwn.h>

template<class T>
struct SimpleFactory : winrt::implements<SimpleFactory<T>, IClassFactory> {
    HRESULT STDMETHODCALLTYPE CreateInstance(IUnknown* pUnkOuter, REFIID riid, void** ppvObject) override try {
        if (!pUnkOuter) {
            *ppvObject = nullptr;
            return winrt::make<T>().as(riid, ppvObject);
        }
        return CLASS_E_NOAGGREGATION;
    }
    catch (...) { return winrt::to_hresult(); }

    HRESULT STDMETHODCALLTYPE LockServer(BOOL) noexcept override { return S_OK; }
};
// -------------------------------------------------------------------------------------------------------------
#pragma region module_cpp

#include <combaseapi.h>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdll-attribute-on-redeclaration"

__declspec(dllexport)
_Use_decl_annotations_ STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv) try {
    if (rclsid == CLSID_WindhawkTAP) {
        *ppv = nullptr;
        return winrt::make<SimpleFactory<WindhawkTAP>>().as(riid, ppv);
    } else return CLASS_E_CLASSNOTAVAILABLE;
} catch (...) {
    return winrt::to_hresult();
}

__declspec(dllexport)
_Use_decl_annotations_ STDAPI DllCanUnloadNow(void) {
    if (winrt::get_module_lock()) return S_FALSE;
    else return S_OK;
}

#pragma clang diagnostic pop
// -------------------------------------------------------------------------------------------------------------
#pragma region api_cpp

using PFN_INITIALIZE_XAML_DIAGNOSTICS_EX = decltype(&InitializeXamlDiagnosticsEx);

HRESULT InjectWindhawkTAP() noexcept {
    HMODULE module = GetCurrentModuleHandle();
    if (!module) return HRESULT_FROM_WIN32(GetLastError());

    WCHAR location[MAX_PATH];
    switch (GetModuleFileName(module, location, ARRAYSIZE(location))) {
        case 0:
        case ARRAYSIZE(location):
            return HRESULT_FROM_WIN32(GetLastError());
    }

    const HMODULE wux(LoadLibraryEx(L"Windows.UI.Xaml.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32));
    if (!wux) [[unlikely]] return HRESULT_FROM_WIN32(GetLastError());

    const auto ixde = reinterpret_cast<PFN_INITIALIZE_XAML_DIAGNOSTICS_EX>(GetProcAddress(wux, "InitializeXamlDiagnosticsEx"));
    if (!ixde) [[unlikely]] return HRESULT_FROM_WIN32(GetLastError());

    const HRESULT hr2 = ixde(L"VisualDiagConnection1", GetCurrentProcessId(), nullptr, location, CLSID_WindhawkTAP, nullptr);
    if (FAILED(hr2)) [[unlikely]] return hr2;

    return S_OK;
}
// -------------------------------------------------------------------------------------------------------------

#include <list>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <variant>
#include <vector>

#include <commctrl.h>
#include <roapi.h>
#include <winstring.h>

#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.Text.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Markup.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.h>

using namespace winrt::Windows::UI::Xaml;

using PropertyKeyValue = std::pair<DependencyProperty, winrt::Windows::Foundation::IInspectable>;
using PropertyValuesUnresolved = std::vector<std::pair<std::wstring, std::wstring>>;
using PropertyValues = std::vector<PropertyKeyValue>;
using PropertyValuesMaybeUnresolved = std::variant<PropertyValuesUnresolved, PropertyValues>;

struct StyleRule {
    std::wstring name;
    std::wstring visualState;
    std::wstring value;
    bool isXamlValue = false;
};
using PropertyOverridesUnresolved = std::vector<StyleRule>;
using PropertyOverrides = std::unordered_map<DependencyProperty, std::unordered_map<std::wstring, winrt::Windows::Foundation::IInspectable>>;
using PropertyOverridesMaybeUnresolved = std::variant<PropertyOverridesUnresolved, PropertyOverrides>;

struct ElementMatcher {
    std::wstring type;
    std::wstring name;
    std::optional<std::wstring> visualStateGroupName;
    int oneBasedIndex = 0;
    PropertyValuesMaybeUnresolved propertyValues;
};
struct ElementCustomizationRules {
    ElementMatcher elementMatcher;
    std::vector<ElementMatcher> parentElementMatchers;
    PropertyOverridesMaybeUnresolved propertyOverrides;
};
std::vector<ElementCustomizationRules> g_elementsCustomizationRules;

struct ElementPropertyCustomizationState {
    std::optional<winrt::Windows::Foundation::IInspectable> originalValue;
    std::optional<winrt::Windows::Foundation::IInspectable> customValue;
    int64_t propertyChangedToken = 0;
};
struct ElementCustomizationStateForVisualStateGroup {
    std::unordered_map<DependencyProperty, ElementPropertyCustomizationState> propertyCustomizationStates;
    winrt::event_token VSGCurrentStateChangedToken;
};
struct ElementCustomizationState {
    winrt::weak_ref<FrameworkElement> element;
    std::list<std::pair<std::optional<winrt::weak_ref<VisualStateGroup>>, ElementCustomizationStateForVisualStateGroup>> perVisualStateGroup;
};
std::unordered_map<InstanceHandle, ElementCustomizationState> g_elementsCustomizationState;

struct BackgroundFillDelayedApplyData {
    UINT_PTR timer = 0;
    winrt::weak_ref<winrt::Windows::UI::Xaml::FrameworkElement> element;
    std::wstring fallbackClassName;
};
std::unordered_map<InstanceHandle, BackgroundFillDelayedApplyData> g_backgroundFillDelayedApplyData;

bool g_elementPropertyModifying;

winrt::Windows::Foundation::IInspectable ReadLocalValueWithWorkaround( DependencyObject elementDo, DependencyProperty property) {
    const auto value = elementDo.ReadLocalValue(property);
    if (value && winrt::get_class_name(value) == L"Windows.UI.Xaml.Data.BindingExpressionBase") return elementDo.GetAnimationBaseValue(property);
    return value;
}

void SetOrClearValue(DependencyObject elementDo, DependencyProperty property, winrt::Windows::Foundation::IInspectable value) {
    if (value == DependencyProperty::UnsetValue()) {
        elementDo.ClearValue(property);
        return;
    }

    try {
        if (property == Controls::TextBlock::FontWeightProperty()) {
            auto valueInt = value.try_as<int>();
            if (valueInt && *valueInt >= std::numeric_limits<uint16_t>::min() && *valueInt <= std::numeric_limits<uint16_t>::max()) {
                value = winrt::box_value(winrt::Windows::UI::Text::FontWeight{ static_cast<uint16_t>(*valueInt) });
            }
        }
        elementDo.SetValue(property, value);
    } catch (winrt::hresult_error const& ex) {}
}

std::wstring EscapeXmlAttribute(std::wstring_view data) {
    std::wstring buffer;
    buffer.reserve(data.size());
    for (size_t pos = 0; pos != data.size(); ++pos) {
        switch (data[pos]) {
            case '&':
                buffer.append(L"&amp;");
                break;
            case '\"':
                buffer.append(L"&quot;");
                break;
            // case '\'':
            //     buffer.append(L"&apos;");
            //     break;
            case '<':
                buffer.append(L"&lt;");
                break;
            case '>':
                buffer.append(L"&gt;");
                break;
            default:
                buffer.append(&data[pos], 1);
                break;
        }
    }
    return buffer;
}

std::wstring_view TrimStringView(std::wstring_view s) {
    s.remove_prefix(std::min(s.find_first_not_of(L" \t\r\v\n"), s.size()));
    s.remove_suffix(std::min(s.size() - s.find_last_not_of(L" \t\r\v\n") - 1, s.size()));
    return s;
}

std::vector<std::wstring_view> SplitStringView(std::wstring_view s, std::wstring_view delimiter) {
    size_t pos_start = 0, pos_end, delim_len = delimiter.length();
    std::wstring_view token;
    std::vector<std::wstring_view> res;

    while ((pos_end = s.find(delimiter, pos_start)) != std::wstring_view::npos) {
        token = s.substr(pos_start, pos_end - pos_start);
        pos_start = pos_end + delim_len;
        res.push_back(token);
    }

    res.push_back(s.substr(pos_start));
    return res;
}

Style GetStyleFromXamlSetters(const std::wstring_view type, const std::wstring_view xamlStyleSetters) {
    std::wstring xaml =
        LR"(<ResourceDictionary
        xmlns="http://schemas.microsoft.com/winfx/2006/xaml/presentation"
        xmlns:x="http://schemas.microsoft.com/winfx/2006/xaml"
        xmlns:d="http://schemas.microsoft.com/expression/blend/2008"
        xmlns:mc="http://schemas.openxmlformats.org/markup-compatibility/2006"
        xmlns:muxc="using:Microsoft.UI.Xaml.Controls")";

    if (auto pos = type.rfind('.'); pos != type.npos) {
        auto typeNamespace = std::wstring_view(type).substr(0, pos);
        auto typeName = std::wstring_view(type).substr(pos + 1);

        xaml += L"\n    xmlns:windhawkstyler=\"using:";
        xaml += EscapeXmlAttribute(typeNamespace);
        xaml += L"\">\n" L"    <Style TargetType=\"windhawkstyler:";
        xaml += EscapeXmlAttribute(typeName);
        xaml += L"\">\n";
    } else {
        xaml += L">\n" L"    <Style TargetType=\"";
        xaml += EscapeXmlAttribute(type);
        xaml += L"\">\n";
    }

    xaml += xamlStyleSetters;
    xaml += L"    </Style>\n" L"</ResourceDictionary>";

    auto resourceDictionary = Markup::XamlReader::Load(xaml).as<ResourceDictionary>();
    auto [styleKey, styleInspectable] = resourceDictionary.First().Current();
    return styleInspectable.as<Style>();
}

const PropertyOverrides& GetResolvedPropertyOverrides(const std::wstring_view type, PropertyOverridesMaybeUnresolved* propertyOverridesMaybeUnresolved) {
    if (const auto* resolved = std::get_if<PropertyOverrides>(propertyOverridesMaybeUnresolved)) return *resolved;

    PropertyOverrides propertyOverrides;
    try {
        const auto& styleRules = std::get<PropertyOverridesUnresolved>(*propertyOverridesMaybeUnresolved);
        if (!styleRules.empty()) {
            std::wstring xaml;

            for (const auto& rule : styleRules) {
                xaml += L"        <Setter Property=\"";
                xaml += EscapeXmlAttribute(rule.name);
                xaml += L"\"";
                if (rule.isXamlValue && rule.value.empty()) {
                    xaml += L" Value=\"{x:Null}\" />\n";
                } else if (!rule.isXamlValue) {
                    xaml += L" Value=\"";
                    xaml += EscapeXmlAttribute(rule.value);
                    xaml += L"\" />\n";
                } else {
                    xaml += L">\n" L"            <Setter.Value>\n";
                    xaml += rule.value;
                    xaml += L"\n" L"            </Setter.Value>\n" L"        </Setter>\n";
                }
            }

            auto style = GetStyleFromXamlSetters(type, xaml);

            uint32_t i = 0;
            for (const auto& rule : styleRules) {
                const auto setter = style.Setters().GetAt(i++).as<Setter>();
                propertyOverrides[setter.Property()][rule.visualState] = rule.isXamlValue && rule.value.empty() ? DependencyProperty::UnsetValue() : setter.Value();
            }
        }
    } catch (winrt::hresult_error const& ex) {
    } catch (std::exception const& ex) {}

    *propertyOverridesMaybeUnresolved = std::move(propertyOverrides);
    return std::get<PropertyOverrides>(*propertyOverridesMaybeUnresolved);
}

const PropertyValues& GetResolvedPropertyValues( const std::wstring_view type, PropertyValuesMaybeUnresolved* propertyValuesMaybeUnresolved) {
    if (const auto* resolved = std::get_if<PropertyValues>(propertyValuesMaybeUnresolved)) return *resolved;

    PropertyValues propertyValues;
    try {
        const auto& propertyValuesStr = std::get<PropertyValuesUnresolved>(*propertyValuesMaybeUnresolved);
        if (!propertyValuesStr.empty()) {
            std::wstring xaml;

            for (const auto& [property, value] : propertyValuesStr) {
                xaml += L"        <Setter Property=\"";
                xaml += EscapeXmlAttribute(property);
                xaml += L"\" Value=\"";
                xaml += EscapeXmlAttribute(value);
                xaml += L"\" />\n";
            }

            auto style = GetStyleFromXamlSetters(type, xaml);

            for (size_t i = 0; i < propertyValuesStr.size(); i++) {
                const auto setter = style.Setters().GetAt(i).as<Setter>();
                propertyValues.push_back({
                    setter.Property(),
                    setter.Value(),
                });
            }
        }
    } catch (winrt::hresult_error const& ex) {
    } catch (std::exception const& ex) {}

    *propertyValuesMaybeUnresolved = std::move(propertyValues);
    return std::get<PropertyValues>(*propertyValuesMaybeUnresolved);
}

VisualStateGroup GetVisualStateGroup(FrameworkElement element, std::wstring_view visualStateGroupName) {
    if (winrt::get_class_name(element) == L"Taskbar.TaskListButtonPanel" && element.Name() == L"ExperienceToggleButtonRootPanel") {
        auto parent = Media::VisualTreeHelper::GetParent(element).try_as<FrameworkElement>();
        if (parent && winrt::get_class_name(parent) == L"Taskbar.SearchBoxLaunchListButton" && parent.Name() == L"SearchBoxLaunchListButton") return nullptr;
    }

    auto list = VisualStateManager::GetVisualStateGroups(element);

    for (const auto& v : list) if (v.Name() == visualStateGroupName) return v;
    return nullptr;
}


bool TestElementMatcher(FrameworkElement element, ElementMatcher& matcher, VisualStateGroup* visualStateGroup, PCWSTR fallbackClassName) {
    if (!matcher.type.empty() && matcher.type != winrt::get_class_name(element) && (!fallbackClassName || matcher.type != fallbackClassName)) return false;
    if (!matcher.name.empty() && matcher.name != element.Name()) return false;

    if (matcher.oneBasedIndex) {
        auto parent = Media::VisualTreeHelper::GetParent(element);
        if (!parent) return false;

        int index = matcher.oneBasedIndex - 1;
        if (index < 0 || index >= Media::VisualTreeHelper::GetChildrenCount(parent) || Media::VisualTreeHelper::GetChild(parent, index) != element) return false;
    }

    auto elementDo = element.as<DependencyObject>();

    for (const auto& propertyValue : GetResolvedPropertyValues(matcher.type, &matcher.propertyValues)) {
        const auto value = ReadLocalValueWithWorkaround(elementDo, propertyValue.first);
        if (!value) return false;

        const auto className = winrt::get_class_name(value);
        const auto expectedClassName = winrt::get_class_name(propertyValue.second);
        if (className != expectedClassName) return false;

        if (className == L"Windows.Foundation.IReference`1<String>") {
            if (winrt::unbox_value<winrt::hstring>(propertyValue.second) == winrt::unbox_value<winrt::hstring>(value)) continue;
            return false;
        }

        if (className == L"Windows.Foundation.IReference`1<Double>") {
            if (winrt::unbox_value<double>(propertyValue.second) == winrt::unbox_value<double>(value)) continue;
            return false;
        }

        if (className == L"Windows.Foundation.IReference`1<Boolean>") {
            if (winrt::unbox_value<bool>(propertyValue.second) == winrt::unbox_value<bool>(value)) continue;
            return false;
        }
        return false;
    }
    
    if (matcher.visualStateGroupName && visualStateGroup) *visualStateGroup = GetVisualStateGroup(element, *matcher.visualStateGroupName);
    return true;
}

std::unordered_map<VisualStateGroup, PropertyOverrides> FindElementPropertyOverrides(FrameworkElement element, PCWSTR fallbackClassName) {
    std::unordered_map<VisualStateGroup, PropertyOverrides> overrides;
    std::unordered_set<DependencyProperty> propertiesAdded;

    for (auto it = g_elementsCustomizationRules.rbegin(); it != g_elementsCustomizationRules.rend(); ++it) {
        auto& override = *it;
        VisualStateGroup visualStateGroup = nullptr;

        if (!TestElementMatcher(element, override.elementMatcher, &visualStateGroup, fallbackClassName)) continue;

        auto parentElementIter = element;
        bool parentElementMatchFailed = false;

        for (auto& matcher : override.parentElementMatchers) {
            parentElementIter = Media::VisualTreeHelper::GetParent(parentElementIter).try_as<FrameworkElement>();
            if (!parentElementIter) {
                parentElementMatchFailed = true;
                break;
            }

            if (!TestElementMatcher(parentElementIter, matcher, &visualStateGroup, nullptr)) {
                parentElementMatchFailed = true;
                break;
            }
        }

        if (parentElementMatchFailed) continue;

        auto& overridesForVisualStateGroup = overrides[visualStateGroup];
        for (const auto& [property, valuesPerVisualState] : GetResolvedPropertyOverrides(override.elementMatcher.type, &override.propertyOverrides)) {
            bool propertyInserted = propertiesAdded.insert(property).second;
            if (!propertyInserted) continue;

            auto& propertyOverrides = overridesForVisualStateGroup[property];
            for (const auto& [visualState, value] : valuesPerVisualState) propertyOverrides.insert({visualState, value});
        }
    }

    std::erase_if(overrides, [](const auto& item) { return item.second.empty(); });
    return overrides;
}

void ApplyCustomizationsForVisualStateGroup(FrameworkElement element, VisualStateGroup visualStateGroup, PropertyOverrides propertyOverrides, ElementCustomizationStateForVisualStateGroup* ECSFVSG) {
    auto elementDo = element.as<DependencyObject>();

    VisualState currentVisualState(visualStateGroup ? visualStateGroup.CurrentState() : nullptr);
    std::wstring currentVisualStateName(currentVisualState ? currentVisualState.Name() : L"");

    for (const auto& [property, valuesPerVisualState] : propertyOverrides) {
        const auto [propertyCustomizationStatesIt, inserted] = ECSFVSG->propertyCustomizationStates.insert({property, {}});
        if (!inserted) continue;

        auto& propertyCustomizationState = propertyCustomizationStatesIt->second;

        auto it = valuesPerVisualState.find(currentVisualStateName);
        if (it == valuesPerVisualState.end() && !currentVisualStateName.empty()) it = valuesPerVisualState.find(L"");

        if (it != valuesPerVisualState.end()) {
            propertyCustomizationState.originalValue = ReadLocalValueWithWorkaround(element, property);
            propertyCustomizationState.customValue = it->second;
            SetOrClearValue(element, property, it->second);
        }

        propertyCustomizationState.propertyChangedToken = elementDo.RegisterPropertyChangedCallback(property, [&propertyCustomizationState](DependencyObject sender, DependencyProperty property) {
            if (g_elementPropertyModifying) return;

            auto element = sender.try_as<FrameworkElement>();
            if (!element) return;
            if (!propertyCustomizationState.customValue) return;

            auto localValue = ReadLocalValueWithWorkaround(element, property);

            if (*propertyCustomizationState.customValue != localValue) propertyCustomizationState.originalValue = localValue;

            g_elementPropertyModifying = true;
            SetOrClearValue(element, property, *propertyCustomizationState.customValue);
            g_elementPropertyModifying = false;
        });
    }

    if (visualStateGroup) {
        winrt::weak_ref<FrameworkElement> elementWeakRef = element;
        ECSFVSG->VSGCurrentStateChangedToken = visualStateGroup.CurrentStateChanged([elementWeakRef, propertyOverrides, ECSFVSG](winrt::Windows::Foundation::IInspectable const& sender, VisualStateChangedEventArgs const& e) {
            auto element = elementWeakRef.get();
            if (!element) return;

            g_elementPropertyModifying = true;

            auto& propertyCustomizationStates = ECSFVSG->propertyCustomizationStates;

            for (const auto& [property, valuesPerVisualState] : propertyOverrides) {
                auto& propertyCustomizationState = propertyCustomizationStates.at(property);

                auto newState = e.NewState();
                auto newStateName = std::wstring{newState ? newState.Name() : L""};
                auto it = valuesPerVisualState.find(newStateName);
                if (it == valuesPerVisualState.end()) {
                    it = valuesPerVisualState.find(L"");
                    if (it != valuesPerVisualState.end()) {
                        auto oldState = e.OldState();
                        auto oldStateName = std::wstring{oldState ? oldState.Name() : L""};
                        if (!valuesPerVisualState.contains(oldStateName)) continue;
                    }
                }

                if (it != valuesPerVisualState.end()) {
                    if (!propertyCustomizationState.originalValue) propertyCustomizationState.originalValue = ReadLocalValueWithWorkaround(element, property);

                    propertyCustomizationState.customValue = it->second;
                    SetOrClearValue(element, property, it->second);
                } else {
                    if (propertyCustomizationState.originalValue) {
                        SetOrClearValue(element, property, *propertyCustomizationState.originalValue);
                        propertyCustomizationState.originalValue.reset();
                    }

                    propertyCustomizationState.customValue.reset();
                }
            }

            g_elementPropertyModifying = false;
        });
    }
}

void RestoreVSG(FrameworkElement element, std::optional<winrt::weak_ref<VisualStateGroup>> VSGOptionalWeakPtr, const ElementCustomizationStateForVisualStateGroup& ECSFVSG) {
    if (element) {
        for (const auto& [property, state] : ECSFVSG.propertyCustomizationStates) {
            element.UnregisterPropertyChangedCallback(property, state.propertyChangedToken);
            if (state.originalValue) SetOrClearValue(element, property, *state.originalValue);
        }
    }

    auto visualStateGroupIter = VSGOptionalWeakPtr ? VSGOptionalWeakPtr->get() : nullptr;
    if (visualStateGroupIter && ECSFVSG.VSGCurrentStateChangedToken) {
        visualStateGroupIter.CurrentStateChanged(ECSFVSG.VSGCurrentStateChangedToken);
    }
}

void ApplyCustomizations(InstanceHandle handle, FrameworkElement element, PCWSTR fallbackClassName) {
    auto overrides = FindElementPropertyOverrides(element, fallbackClassName);
    if (overrides.empty()) return;

    auto& elementCustomizationState = g_elementsCustomizationState[handle];

    for (const auto& [vSGOptionalWeakPtrIter, stateIter] : elementCustomizationState.perVisualStateGroup) {
        RestoreVSG(element, vSGOptionalWeakPtrIter, stateIter);
    }

    elementCustomizationState.element = element;
    elementCustomizationState.perVisualStateGroup.clear();

    for (auto& [visualStateGroup, overridesForVisualStateGroup] : overrides) {
        std::optional<winrt::weak_ref<VisualStateGroup>> visualStateGroupOptionalWeakPtr;
        if (visualStateGroup) visualStateGroupOptionalWeakPtr = visualStateGroup;

        elementCustomizationState.perVisualStateGroup.push_back({visualStateGroupOptionalWeakPtr, {}});
        auto* ECSFVSG = &elementCustomizationState.perVisualStateGroup.back().second;

        ApplyCustomizationsForVisualStateGroup(element, visualStateGroup, std::move(overridesForVisualStateGroup), ECSFVSG);
    }
}

void CleanupCustomizations(InstanceHandle handle) {
    if (auto it = g_backgroundFillDelayedApplyData.find(handle); it != g_backgroundFillDelayedApplyData.end()) {
        KillTimer(nullptr, it->second.timer);
        g_backgroundFillDelayedApplyData.erase(it);
    }

    auto it = g_elementsCustomizationState.find(handle);
    if (it == g_elementsCustomizationState.end()) return;

    auto& elementCustomizationState = it->second;
    auto element = elementCustomizationState.element.get();

    for (const auto& [vSGOptionalWeakPtrIter, stateIter] : elementCustomizationState.perVisualStateGroup) {
        RestoreVSG(element, vSGOptionalWeakPtrIter, stateIter);
    }

    g_elementsCustomizationState.erase(it);
}

ElementMatcher ElementMatcherFromString(std::wstring_view str) {
    ElementMatcher result;
    PropertyValuesUnresolved propertyValuesUnresolved;

    auto i = str.find_first_of(L"#@[");
    result.type = TrimStringView(str.substr(0, i));

    while (i != str.npos) {
        auto iNext = str.find_first_of(L"#@[", i + 1);
        auto nextPart = str.substr(i + 1, iNext == str.npos ? str.npos : iNext - (i + 1));

        switch (str[i]) {
            case L'#':
                result.name = TrimStringView(nextPart);
                break;

            case L'@':
                result.visualStateGroupName = TrimStringView(nextPart);
                break;

            case L'[': {
                auto rule = TrimStringView(nextPart);
                rule = TrimStringView(rule.substr(0, rule.length() - 1));

                if (rule.find_first_not_of(L"0123456789") == rule.npos) {
                    result.oneBasedIndex = std::stoi(std::wstring(rule));
                    break;
                }

                auto ruleEqPos = rule.find(L'=');
                auto ruleKey = TrimStringView(rule.substr(0, ruleEqPos));
                auto ruleVal = TrimStringView(rule.substr(ruleEqPos + 1));

                propertyValuesUnresolved.push_back({std::wstring(ruleKey), std::wstring(ruleVal)});
                break;
            }
        }
        i = iNext;
    }

    result.propertyValues = std::move(propertyValuesUnresolved);
    return result;
}


StyleRule StyleRuleFromString(std::wstring_view str) {
    StyleRule result;

    auto eqPos = str.find(L'=');
    auto name = str.substr(0, eqPos);
    auto value = str.substr(eqPos + 1);

    result.value = TrimStringView(value);

    if (name.size() > 0 && name.back() == L':') {
        result.isXamlValue = true;
        name = name.substr(0, name.size() - 1);
    }

    auto atPos = name.find(L'@');
    if (atPos != name.npos) {
        result.visualState = TrimStringView(name.substr(atPos + 1));
        name = name.substr(0, atPos);
    }

    result.name = TrimStringView(name);
    return result;
}

std::wstring AdjustTypeName(std::wstring_view type) {
    if (type.find_first_of(L".:") == type.npos) return L"Windows.UI.Xaml.Controls." + std::wstring{type};
    return std::wstring{type};
}

void AddElementCustomizationRules(std::wstring_view target, std::vector<std::wstring> styles) {
    ElementCustomizationRules elementCustomizationRules;
    auto targetParts = SplitStringView(target, L" > ");
    
    bool first = true;
    bool hasVisualStateGroup = false;
    for (auto i = targetParts.rbegin(); i != targetParts.rend(); ++i) {
        const auto& targetPart = *i;

        auto matcher = ElementMatcherFromString(targetPart);
        matcher.type = AdjustTypeName(matcher.type);

        if (matcher.visualStateGroupName) {
            if (hasVisualStateGroup) throw std::runtime_error("Element type can't have more than one visual state group");
            hasVisualStateGroup = true;
        }

        if (first) {
            std::vector<StyleRule> styleRules;
            for (const auto& style : styles) styleRules.push_back(StyleRuleFromString(style));

            elementCustomizationRules.elementMatcher = std::move(matcher);
            elementCustomizationRules.propertyOverrides = std::move(styleRules);
        } else elementCustomizationRules.parentElementMatchers.push_back(std::move(matcher));

        first = false;
    }

    g_elementsCustomizationRules.push_back(std::move(elementCustomizationRules));
}

void ProcessTheme(const Theme& theme) {
    for (const auto& themeTargetStyle : theme.targetStyles) {
        try {
            AddElementCustomizationRules(themeTargetStyle.target, std::vector<std::wstring>{ themeTargetStyle.styles.begin(), themeTargetStyle.styles.end() });
        } catch (const winrt::hresult_error&) {
        } catch (const std::exception&) {}
    }
}

void InitializeSettingsAndTap() {
    DWORD kNoThreadId = 0;
    if (g_targetThreadId.compare_exchange_strong(kNoThreadId, GetCurrentThreadId())) {
        ProcessTheme(themeTaskbarXII);
        InjectWindhawkTAP();
    }
}

void UninitializeSettingsAndTap() {
    for (auto& [handle, data] : g_backgroundFillDelayedApplyData) KillTimer(nullptr, data.timer);
    g_backgroundFillDelayedApplyData.clear();

    for (const auto& [handle, elementCustomizationState] : g_elementsCustomizationState) {
        auto element = elementCustomizationState.element.get();

        for (const auto& [vSGOptionalWeakPtrIter, stateIter] : elementCustomizationState.perVisualStateGroup) { RestoreVSG(element, vSGOptionalWeakPtrIter, stateIter); }
    }

    g_elementsCustomizationState.clear();
    g_elementsCustomizationRules.clear();
    g_targetThreadId = 0;
}

using CreateWindowExW_t = decltype(&CreateWindowExW);
CreateWindowExW_t CreateWindowExW_Original;
HWND WINAPI CreateWindowExW_Hook(DWORD dwExStyle, LPCWSTR lpClassName, LPCWSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, PVOID lpParam) {
    HWND hWnd = CreateWindowExW_Original(dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
    if (!hWnd) return hWnd;
    if (g_targetThreadId || !hWndParent) return hWnd;

    const int classNameSize = 64;
    WCHAR className[classNameSize];

    if (GetClassName(hWnd, className, classNameSize) && _wcsicmp(className, L"Windows.UI.Composition.DesktopWindowContentBridge") == 0) {
        if (GetClassName(hWndParent, className, classNameSize) && _wcsicmp(className, L"Shell_TrayWnd") == 0) {
            InitializeSettingsAndTap();
        }
    }

    return hWnd;
}

using RunFromWindowThreadProc_t = void(WINAPI*)(PVOID parameter);
bool RunFromWindowThread(HWND hWnd, RunFromWindowThreadProc_t proc, PVOID procParam) {
    static const UINT runFromWindowThreadRegisteredMsg = RegisterWindowMessage(L"Ryo_RunFromWindowThread_" WH_MOD_ID);

    struct RUN_FROM_WINDOW_THREAD_PARAM {
        RunFromWindowThreadProc_t proc;
        PVOID procParam;
    };

    DWORD dwThreadId = GetWindowThreadProcessId(hWnd, nullptr);
    if (dwThreadId == 0) return false;

    if (dwThreadId == GetCurrentThreadId()) {
        proc(procParam);
        return true;
    }

    HHOOK hook = SetWindowsHookEx(WH_CALLWNDPROC, [](int nCode, WPARAM wParam, LPARAM lParam) WINAPI -> LRESULT {
        if (nCode == HC_ACTION) {
            const CWPSTRUCT* cwp = (const CWPSTRUCT*)lParam;
            if (cwp->message == runFromWindowThreadRegisteredMsg) {
                RUN_FROM_WINDOW_THREAD_PARAM* param = (RUN_FROM_WINDOW_THREAD_PARAM*)cwp->lParam;
                param->proc(param->procParam);
            }
        }
        return CallNextHookEx(nullptr, nCode, wParam, lParam);
    }, nullptr, dwThreadId);

    if (!hook) return false;

    RUN_FROM_WINDOW_THREAD_PARAM param = {proc, procParam};
    SendMessage(hWnd, runFromWindowThreadRegisteredMsg, 0, (LPARAM)&param);
    UnhookWindowsHookEx(hook);

    return true;
}

HWND GetTaskbarUiWnd() {
    DWORD dwProcessId; DWORD dwCurrentProcessId = GetCurrentProcessId();

    HWND hTaskbarWnd = FindWindow(L"Shell_TrayWnd", nullptr);
    if (!hTaskbarWnd || !GetWindowThreadProcessId(hTaskbarWnd, &dwProcessId) || dwProcessId != dwCurrentProcessId) return nullptr;

    return FindWindowEx(hTaskbarWnd, nullptr, L"Windows.UI.Composition.DesktopWindowContentBridge", nullptr);
}

BOOL Wh_ModInit() {
    Wh_SetFunctionHook((void*)CreateWindowExW, (void*)CreateWindowExW_Hook, (void**)&CreateWindowExW_Original);
    return TRUE;
}

void Wh_ModAfterInit() {
    HWND hTaskbarUiWnd = GetTaskbarUiWnd();
    if (hTaskbarUiWnd) RunFromWindowThread(hTaskbarUiWnd, [](PVOID) WINAPI { InitializeSettingsAndTap(); }, nullptr);
}

void Wh_ModUninit() {
    if (g_visualTreeWatcher) {
        g_visualTreeWatcher->UnadviseVisualTreeChange();
        g_visualTreeWatcher = nullptr;
    }

    HWND hTaskbarUiWnd = GetTaskbarUiWnd();
    if (hTaskbarUiWnd) RunFromWindowThread(hTaskbarUiWnd, [](PVOID) WINAPI { UninitializeSettingsAndTap(); }, nullptr);
}