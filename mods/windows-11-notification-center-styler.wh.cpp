// ==WindhawkMod==
// @id              windows-11-notification-center-styler
// @name            Windows 11 Notification Center Styler
// @description     An advanced mod to override style attributes of the Notification Center
// @version         1.0.1
// @author          m417z
// @github          https://github.com/m417z
// @twitter         https://twitter.com/m417z
// @homepage        https://m417z.com/
// @include         ShellExperienceHost.exe
// @architecture    x86-64
// @compilerOptions -lcomctl32 -lole32 -loleaut32 -lruntimeobject -Wl,--export-all-symbols
// ==/WindhawkMod==

// Source code is published under The GNU General Public License v3.0.
//
// For bug reports and feature requests, please open an issue here:
// https://github.com/ramensoftware/windhawk-mods/issues
//
// For pull requests, development takes place here:
// https://github.com/m417z/my-windhawk-mods

// ==WindhawkModReadme==
/*
# Windows 11 Notification Center Styler

An advanced mod to override style attributes of the notification center and
calendar.

**Note**: This mod requires Windhawk v1.4 or later.

Also check out the **Windows 11 Taskbar Styler**, **Windows 11 Start Menu
Styler** mods.

## Examples

### Hide the focus assist section
**Target**: `ActionCenter.FocusSessionControl` \
**Style**: `Height=0`

### Square the corners of the notification center
**Target**: `Windows.UI.Xaml.Controls.Grid#NotificationCenterGrid` \
**Style**: `CornerRadius=0`

### Square the corners of the calendar
**Target**: `Windows.UI.Xaml.Controls.Grid#CalendarCenterGrid` \
**Style**: `CornerRadius=0`

### Square the corners of the quick action center
**Target**: `Windows.UI.Xaml.Controls.Grid#ControlCenterRegion` \
**Style**: `CornerRadius=0`

### Calendar and notification titlebars: titles on the right, buttons on the left
**Target**: `Windows.UI.Xaml.Controls.Grid#RootContent` \
**Style**: `FlowDirection=1`

### Add accelerator key (ALT+X) to clear all notifications
**Target**: `Windows.UI.Xaml.Controls.Button#ClearAll` \
**Style**: `AccessKey=x`

### Add accelerator key (ALT+E) to expand/collapse the calendar
**Target**: `Windows.UI.Xaml.Controls.Button#ExpandCollapseButton` \
**Style**: `AccessKey=e`

## Advanced styling

The settings have two sections: control styles and resource
variables. Control styles allow to override styles, such as size and color, for
the target elements. Resource variables allow to override predefined variables.
For a more detailed explanation and examples, refer to the sections below.

The [UWPSpy](https://ramensoftware.com/uwpspy) tool can be used to inspect the
notification center control elements in real time, and experiment with various styles.

### Control styles

Each entry has a target control and a list of styles.

The target control is written as `Class` or `Class#Name`, i.e. the target
control class name (the tag name in XAML resource files), such as
`ActionCenter.FocusSessionControl` or `Rectangle`, optionally followed by `#` and the
target control's name (`x:Name` attribute in XAML resource files). The target
control can also include:
* Child control index, for example: `Class#Name[2]` will only match the relevant
  control that's also the second child among all of its parent's child controls.
* Control properties, for example:
  `Class#Name[Property1=Value1][Property2=Value2]`.
* Parent controls, separated by `>`, for example: `ParentClass#ParentName >
  Class#Name`.
* Visual state group name, for example: `Class#Name@VisualStateGroupName`. It
  can be specified for the target control or for a parent control, but can be
  specified only once per target. The visual state group can be used in styles
  as specified below.

**Note**: The target is evaluated only once. If, for example, the index or the
properties of a control change, the target conditions aren't evaluated again.

Each style is written as `Style=Value`, for example: `Height=5`. The `:=` syntax
can be used to use XAML syntax, for example: `Fill:=<SolidColorBrush
Color="Red"/>`. Specifying an empty value with the XAML syntax will clear the
property value, for example: `Fill:=`. In addition, a visual state can be
specified as following: `Style@VisualState=Value`, in which case the style will
only apply when the visual state group specified in the target matches the
specified visual state.

### Resource variables

Some variables, such as size and padding for various controls, are defined as
resource variables.

## Implementation notes

The VisualTreeWatcher implementation is based on the
[ExplorerTAP](https://github.com/TranslucentTB/TranslucentTB/tree/develop/ExplorerTAP)
code from the **TranslucentTB** project.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- controlStyles:
  - - target: ""
      $name: Target
    - styles: [""]
      $name: Styles
  $name: Control styles
- resourceVariables:
  - - variableKey: ""
      $name: Variable key
    - value: ""
      $name: Value
  $name: Resource variables
*/
// ==/WindhawkModSettings==

#include <windows.ui.xaml.hosting.desktopwindowxamlsource.h>
#include <xamlom.h>

#include <atomic>
#include <vector>

#undef GetCurrentTime

#include <winrt/Windows.UI.Xaml.h>

std::atomic<DWORD> g_targetThreadId = 0;

void ApplyCustomizations(InstanceHandle handle,
                         winrt::Windows::UI::Xaml::FrameworkElement element,
                         PCWSTR fallbackClassName);
void CleanupCustomizations(InstanceHandle handle);

HMODULE GetCurrentModuleHandle() {
    HMODULE module;
    if (!GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                               GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                           L"", &module)) {
        return nullptr;
    }

    return module;
}

////////////////////////////////////////////////////////////////////////////////
// clang-format off

#pragma region winrt_hpp

#include <guiddef.h>
#include <Unknwn.h>
#include <winrt/base.h>

// forward declare namespaces we alias
namespace winrt {
    namespace Windows {
        namespace Foundation::Collections {}
        namespace UI::Xaml {
            namespace Controls {}
            namespace Hosting {}
        }
    }
}

// alias some long namespaces for convenience
namespace wf = winrt::Windows::Foundation;
// namespace wfc = wf::Collections;
namespace wux = winrt::Windows::UI::Xaml;
// namespace wuxc = wux::Controls;
namespace wuxh = wux::Hosting;

#pragma endregion  // winrt_hpp

#pragma region visualtreewatcher_hpp

#include <winrt/Windows.UI.Xaml.h>

class VisualTreeWatcher : public winrt::implements<VisualTreeWatcher, IVisualTreeServiceCallback2, winrt::non_agile>
{
public:
    VisualTreeWatcher(winrt::com_ptr<IUnknown> site);

    VisualTreeWatcher(const VisualTreeWatcher&) = delete;
    VisualTreeWatcher& operator=(const VisualTreeWatcher&) = delete;

    VisualTreeWatcher(VisualTreeWatcher&&) = delete;
    VisualTreeWatcher& operator=(VisualTreeWatcher&&) = delete;

    ~VisualTreeWatcher();

    void UnadviseVisualTreeChange();

private:
    HRESULT STDMETHODCALLTYPE OnVisualTreeChange(ParentChildRelation relation, VisualElement element, VisualMutationType mutationType) override;
    HRESULT STDMETHODCALLTYPE OnElementStateChanged(InstanceHandle element, VisualElementState elementState, LPCWSTR context) noexcept override;

    template<typename T>
    T FromHandle(InstanceHandle handle)
    {
        wf::IInspectable obj;
        winrt::check_hresult(m_XamlDiagnostics->GetIInspectableFromHandle(handle, reinterpret_cast<::IInspectable**>(winrt::put_abi(obj))));

        return obj.as<T>();
    }

    winrt::com_ptr<IXamlDiagnostics> m_XamlDiagnostics = nullptr;
};

#pragma endregion  // visualtreewatcher_hpp

#pragma region visualtreewatcher_cpp

#include <winrt/Windows.UI.Xaml.Hosting.h>

VisualTreeWatcher::VisualTreeWatcher(winrt::com_ptr<IUnknown> site) :
    m_XamlDiagnostics(site.as<IXamlDiagnostics>())
{
    Wh_Log(L"Constructing VisualTreeWatcher");
    winrt::check_hresult(m_XamlDiagnostics.as<IVisualTreeService3>()->AdviseVisualTreeChange(this));
}

VisualTreeWatcher::~VisualTreeWatcher()
{
    Wh_Log(L"Destructing VisualTreeWatcher");
}

void VisualTreeWatcher::UnadviseVisualTreeChange()
{
    Wh_Log(L"UnadviseVisualTreeChange VisualTreeWatcher");
    winrt::check_hresult(m_XamlDiagnostics.as<IVisualTreeService3>()->UnadviseVisualTreeChange(this));
}

HRESULT VisualTreeWatcher::OnVisualTreeChange(ParentChildRelation, VisualElement element, VisualMutationType mutationType) try
{
    if (GetCurrentThreadId() != g_targetThreadId)
    {
        return S_OK;
    }

    Wh_Log(L"========================================");

    switch (mutationType)
    {
    case Add:
        Wh_Log(L"Mutation type: Add");
        break;

    case Remove:
        Wh_Log(L"Mutation type: Remove");
        break;

    default:
        Wh_Log(L"Mutation type: %d", static_cast<int>(mutationType));
        break;
    }

    Wh_Log(L"Element type: %s", element.Type);

    if (mutationType == Add)
    {
        const auto inspectable = FromHandle<wf::IInspectable>(element.Handle);

        auto frameworkElement = inspectable.try_as<wux::FrameworkElement>();
        if (!frameworkElement)
        {
            const auto desktopXamlSource = FromHandle<wuxh::DesktopWindowXamlSource>(element.Handle);
            frameworkElement = desktopXamlSource.Content().try_as<wux::FrameworkElement>();
        }

        if (frameworkElement)
        {
            Wh_Log(L"FrameworkElement name: %s", frameworkElement.Name().c_str());
            ApplyCustomizations(element.Handle, frameworkElement, element.Type);
        }
    }
    else if (mutationType == Remove)
    {
        CleanupCustomizations(element.Handle);
    }

    return S_OK;
}
catch (...)
{
    HRESULT hr = winrt::to_hresult();
    Wh_Log(L"Error %08X", hr);

    // Returning an error prevents (some?) further messages, always return
    // success.
    // return hr;
    return S_OK;
}

HRESULT VisualTreeWatcher::OnElementStateChanged(InstanceHandle, VisualElementState, LPCWSTR) noexcept
{
    return S_OK;
}

#pragma endregion  // visualtreewatcher_cpp

#pragma region tap_hpp

#include <ocidl.h>

// TODO: weak_ref might be better here.
winrt::com_ptr<VisualTreeWatcher> g_visualTreeWatcher;

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

#pragma endregion  // tap_hpp

#pragma region tap_cpp

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

        g_visualTreeWatcher = winrt::make_self<VisualTreeWatcher>(site);
    }

    return S_OK;
}
catch (...)
{
    return winrt::to_hresult();
}

HRESULT WindhawkTAP::GetSite(REFIID riid, void **ppvSite) noexcept
{
    return site.as(riid, ppvSite);
}

#pragma endregion  // tap_cpp

#pragma region simplefactory_hpp

#include <Unknwn.h>

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
        return winrt::to_hresult();
    }

    HRESULT STDMETHODCALLTYPE LockServer(BOOL) noexcept override
    {
        return S_OK;
    }
};

#pragma endregion  // simplefactory_hpp

#pragma region module_cpp

#include <combaseapi.h>

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
    return winrt::to_hresult();
}

__declspec(dllexport)
_Use_decl_annotations_ STDAPI DllCanUnloadNow(void)
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

#pragma endregion  // module_cpp

#pragma region api_cpp

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
    if (!wux) [[unlikely]]
    {
        return HRESULT_FROM_WIN32(GetLastError());
    }

    const auto ixde = reinterpret_cast<PFN_INITIALIZE_XAML_DIAGNOSTICS_EX>(GetProcAddress(wux, "InitializeXamlDiagnosticsEx"));
    if (!ixde) [[unlikely]]
    {
        return HRESULT_FROM_WIN32(GetLastError());
    }

    const HRESULT hr2 = ixde(L"VisualDiagConnection1", GetCurrentProcessId(), nullptr, location, CLSID_WindhawkTAP, nullptr);
    if (FAILED(hr2)) [[unlikely]]
    {
        return hr2;
    }

    return S_OK;
}

#pragma endregion  // api_cpp

// clang-format on
////////////////////////////////////////////////////////////////////////////////

#include <list>
#include <optional>
#include <sstream>
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

// https://stackoverflow.com/a/51274008
template <auto fn>
struct deleter_from_fn {
    template <typename T>
    constexpr void operator()(T* arg) const {
        fn(arg);
    }
};
using string_setting_unique_ptr =
    std::unique_ptr<const WCHAR[], deleter_from_fn<Wh_FreeStringSetting>>;

using PropertyKeyValue =
    std::pair<DependencyProperty, winrt::Windows::Foundation::IInspectable>;

using PropertyValuesUnresolved =
    std::vector<std::pair<std::wstring, std::wstring>>;
using PropertyValues = std::vector<PropertyKeyValue>;
using PropertyValuesMaybeUnresolved =
    std::variant<PropertyValuesUnresolved, PropertyValues>;

struct ElementMatcher {
    std::wstring type;
    std::wstring name;
    std::optional<std::wstring> visualStateGroupName;
    int oneBasedIndex = 0;
    PropertyValuesMaybeUnresolved propertyValues;
};

struct StyleRule {
    std::wstring name;
    std::wstring visualState;
    std::wstring value;
    bool isXamlValue = false;
};

using PropertyOverridesUnresolved = std::vector<StyleRule>;

// Property -> visual state -> value.
using PropertyOverrides = std::unordered_map<
    DependencyProperty,
    std::unordered_map<std::wstring, winrt::Windows::Foundation::IInspectable>>;

using PropertyOverridesMaybeUnresolved =
    std::variant<PropertyOverridesUnresolved, PropertyOverrides>;

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
    std::unordered_map<DependencyProperty, ElementPropertyCustomizationState>
        propertyCustomizationStates;
    winrt::event_token visualStateGroupCurrentStateChangedToken;
};

struct ElementCustomizationState {
    winrt::weak_ref<FrameworkElement> element;

    // Use list to avoid reallocations on insertion, as pointers to items are
    // captured in callbacks and stored.
    std::list<std::pair<std::optional<winrt::weak_ref<VisualStateGroup>>,
                        ElementCustomizationStateForVisualStateGroup>>
        perVisualStateGroup;
};

std::unordered_map<InstanceHandle, ElementCustomizationState>
    g_elementsCustomizationState;

bool g_elementPropertyModifying;

winrt::Windows::Foundation::IInspectable ReadLocalValueWithWorkaround(
    DependencyObject elementDo,
    DependencyProperty property) {
    const auto value = elementDo.ReadLocalValue(property);
    if (value && winrt::get_class_name(value) ==
                     L"Windows.UI.Xaml.Data.BindingExpressionBase") {
        // BindingExpressionBase was observed to be returned for XAML properties
        // that were declared as following:
        //
        // <Border ... CornerRadius="{TemplateBinding CornerRadius}" />
        //
        // Calling SetValue with it fails with an error, so we won't be able to
        // use it to restore the value. As a workaround, we use
        // GetAnimationBaseValue to get the value.
        return elementDo.GetAnimationBaseValue(property);
    }

    return value;
}

void SetOrClearValue(DependencyObject elementDo,
                     DependencyProperty property,
                     winrt::Windows::Foundation::IInspectable value) {
    if (value == DependencyProperty::UnsetValue()) {
        elementDo.ClearValue(property);
        return;
    }

    // This might fail. See `ReadLocalValueWithWorkaround` for an example (which
    // we now handle but there might be other cases).
    try {
        // `setter.Value()` returns font weight as an int. Using it with
        // `SetValue` results in the following error: 0x80004002 (No such
        // interface supported). Box it as `Windows.UI.Text.FontWeight` as a
        // workaround.
        if (property == Controls::TextBlock::FontWeightProperty()) {
            auto valueInt = value.try_as<int>();
            if (valueInt && *valueInt >= std::numeric_limits<uint16_t>::min() &&
                *valueInt <= std::numeric_limits<uint16_t>::max()) {
                value = winrt::box_value(winrt::Windows::UI::Text::FontWeight{
                    static_cast<uint16_t>(*valueInt)});
            }
        }

        elementDo.SetValue(property, value);
    } catch (winrt::hresult_error const& ex) {
        Wh_Log(L"Error %08X: %s", ex.code(), ex.message().c_str());
    }
}

// https://stackoverflow.com/a/5665377
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

// https://stackoverflow.com/a/54364173
std::wstring_view TrimStringView(std::wstring_view s) {
    s.remove_prefix(std::min(s.find_first_not_of(L" \t\r\v\n"), s.size()));
    s.remove_suffix(
        std::min(s.size() - s.find_last_not_of(L" \t\r\v\n") - 1, s.size()));
    return s;
}

// https://stackoverflow.com/a/46931770
std::vector<std::wstring_view> SplitStringView(std::wstring_view s,
                                               std::wstring_view delimiter) {
    size_t pos_start = 0, pos_end, delim_len = delimiter.length();
    std::wstring_view token;
    std::vector<std::wstring_view> res;

    while ((pos_end = s.find(delimiter, pos_start)) !=
           std::wstring_view::npos) {
        token = s.substr(pos_start, pos_end - pos_start);
        pos_start = pos_end + delim_len;
        res.push_back(token);
    }

    res.push_back(s.substr(pos_start));
    return res;
}

Style GetStyleFromXamlSetters(const std::wstring_view type,
                              const std::wstring_view xamlStyleSetters) {
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
        xaml +=
            L"\">\n"
            L"    <Style TargetType=\"windhawkstyler:";
        xaml += EscapeXmlAttribute(typeName);
        xaml += L"\">\n";
    } else {
        xaml +=
            L">\n"
            L"    <Style TargetType=\"";
        xaml += EscapeXmlAttribute(type);
        xaml += L"\">\n";
    }

    xaml += xamlStyleSetters;

    xaml +=
        L"    </Style>\n"
        L"</ResourceDictionary>";

    Wh_Log(L"======================================== XAML:");
    std::wstringstream ss(xaml);
    std::wstring line;
    while (std::getline(ss, line, L'\n')) {
        Wh_Log(L"%s", line.c_str());
    }
    Wh_Log(L"========================================");

    auto resourceDictionary =
        Markup::XamlReader::Load(xaml).as<ResourceDictionary>();

    auto [styleKey, styleInspectable] = resourceDictionary.First().Current();
    return styleInspectable.as<Style>();
}

const PropertyOverrides& GetResolvedPropertyOverrides(
    const std::wstring_view type,
    PropertyOverridesMaybeUnresolved* propertyOverridesMaybeUnresolved) {
    if (const auto* resolved =
            std::get_if<PropertyOverrides>(propertyOverridesMaybeUnresolved)) {
        return *resolved;
    }

    PropertyOverrides propertyOverrides;

    try {
        const auto& styleRules = std::get<PropertyOverridesUnresolved>(
            *propertyOverridesMaybeUnresolved);
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
                    xaml +=
                        L">\n"
                        L"            <Setter.Value>\n";
                    xaml += rule.value;
                    xaml +=
                        L"\n"
                        L"            </Setter.Value>\n"
                        L"        </Setter>\n";
                }
            }

            auto style = GetStyleFromXamlSetters(type, xaml);

            uint32_t i = 0;
            for (const auto& rule : styleRules) {
                const auto setter = style.Setters().GetAt(i++).as<Setter>();
                propertyOverrides[setter.Property()][rule.visualState] =
                    rule.isXamlValue && rule.value.empty()
                        ? DependencyProperty::UnsetValue()
                        : setter.Value();
            }
        }

        Wh_Log(L"%.*s: %zu override styles", static_cast<int>(type.length()),
               type.data(), propertyOverrides.size());
    } catch (winrt::hresult_error const& ex) {
        Wh_Log(L"Error %08X: %s", ex.code(), ex.message().c_str());
    } catch (std::exception const& ex) {
        Wh_Log(L"Error: %S", ex.what());
    }

    *propertyOverridesMaybeUnresolved = std::move(propertyOverrides);
    return std::get<PropertyOverrides>(*propertyOverridesMaybeUnresolved);
}

const PropertyValues& GetResolvedPropertyValues(
    const std::wstring_view type,
    PropertyValuesMaybeUnresolved* propertyValuesMaybeUnresolved) {
    if (const auto* resolved =
            std::get_if<PropertyValues>(propertyValuesMaybeUnresolved)) {
        return *resolved;
    }

    PropertyValues propertyValues;

    try {
        const auto& propertyValuesStr =
            std::get<PropertyValuesUnresolved>(*propertyValuesMaybeUnresolved);
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

        Wh_Log(L"%.*s: %zu matcher styles", static_cast<int>(type.length()),
               type.data(), propertyValues.size());
    } catch (winrt::hresult_error const& ex) {
        Wh_Log(L"Error %08X: %s", ex.code(), ex.message().c_str());
    } catch (std::exception const& ex) {
        Wh_Log(L"Error: %S", ex.what());
    }

    *propertyValuesMaybeUnresolved = std::move(propertyValues);
    return std::get<PropertyValues>(*propertyValuesMaybeUnresolved);
}

// https://stackoverflow.com/a/12835139
VisualStateGroup GetVisualStateGroup(FrameworkElement element,
                                     std::wstring_view visualStateGroupName) {
    auto list = VisualStateManager::GetVisualStateGroups(element);

    for (const auto& v : list) {
        if (v.Name() == visualStateGroupName) {
            return v;
        }
    }

    return nullptr;
}

bool TestElementMatcher(FrameworkElement element,
                        ElementMatcher& matcher,
                        VisualStateGroup* visualStateGroup,
                        PCWSTR fallbackClassName) {
    if (!matcher.type.empty() &&
        matcher.type != winrt::get_class_name(element) &&
        (!fallbackClassName || matcher.type != fallbackClassName)) {
        return false;
    }

    if (!matcher.name.empty() && matcher.name != element.Name()) {
        return false;
    }

    if (matcher.oneBasedIndex) {
        auto parent = Media::VisualTreeHelper::GetParent(element);
        if (!parent) {
            return false;
        }

        int index = matcher.oneBasedIndex - 1;
        if (index < 0 ||
            index >= Media::VisualTreeHelper::GetChildrenCount(parent) ||
            Media::VisualTreeHelper::GetChild(parent, index) != element) {
            return false;
        }
    }

    auto elementDo = element.as<DependencyObject>();

    for (const auto& propertyValue :
         GetResolvedPropertyValues(matcher.type, &matcher.propertyValues)) {
        const auto value =
            ReadLocalValueWithWorkaround(elementDo, propertyValue.first);
        if (!value) {
            Wh_Log(L"Null property value");
            return false;
        }

        const auto className = winrt::get_class_name(value);
        const auto expectedClassName =
            winrt::get_class_name(propertyValue.second);
        if (className != expectedClassName) {
            Wh_Log(L"Different property class: %s vs. %s", className.c_str(),
                   expectedClassName.c_str());
            return false;
        }

        if (className == L"Windows.Foundation.IReference`1<String>") {
            if (winrt::unbox_value<winrt::hstring>(propertyValue.second) ==
                winrt::unbox_value<winrt::hstring>(value)) {
                continue;
            }

            return false;
        }

        if (className == L"Windows.Foundation.IReference`1<Double>") {
            if (winrt::unbox_value<double>(propertyValue.second) ==
                winrt::unbox_value<double>(value)) {
                continue;
            }

            return false;
        }

        if (className == L"Windows.Foundation.IReference`1<Boolean>") {
            if (winrt::unbox_value<bool>(propertyValue.second) ==
                winrt::unbox_value<bool>(value)) {
                continue;
            }

            return false;
        }

        Wh_Log(L"Unsupported property class: %s", className.c_str());
        return false;
    }

    if (matcher.visualStateGroupName && visualStateGroup) {
        *visualStateGroup =
            GetVisualStateGroup(element, *matcher.visualStateGroupName);
    }

    return true;
}

std::unordered_map<VisualStateGroup, PropertyOverrides>
FindElementPropertyOverrides(FrameworkElement element,
                             PCWSTR fallbackClassName) {
    std::unordered_map<VisualStateGroup, PropertyOverrides> overrides;
    std::unordered_set<DependencyProperty> propertiesAdded;

    for (auto it = g_elementsCustomizationRules.rbegin();
         it != g_elementsCustomizationRules.rend(); ++it) {
        auto& override = *it;

        VisualStateGroup visualStateGroup = nullptr;

        if (!TestElementMatcher(element, override.elementMatcher,
                                &visualStateGroup, fallbackClassName)) {
            continue;
        }

        auto parentElementIter = element;
        bool parentElementMatchFailed = false;

        for (auto& matcher : override.parentElementMatchers) {
            // Using parentElementIter.Parent() was sometimes returning null.
            parentElementIter =
                Media::VisualTreeHelper::GetParent(parentElementIter)
                    .try_as<FrameworkElement>();
            if (!parentElementIter) {
                parentElementMatchFailed = true;
                break;
            }

            if (!TestElementMatcher(parentElementIter, matcher,
                                    &visualStateGroup, nullptr)) {
                parentElementMatchFailed = true;
                break;
            }
        }

        if (parentElementMatchFailed) {
            continue;
        }

        auto& overridesForVisualStateGroup = overrides[visualStateGroup];
        for (const auto& [property, valuesPerVisualState] :
             GetResolvedPropertyOverrides(override.elementMatcher.type,
                                          &override.propertyOverrides)) {
            bool propertyInserted = propertiesAdded.insert(property).second;
            if (!propertyInserted) {
                continue;
            }

            auto& propertyOverrides = overridesForVisualStateGroup[property];
            for (const auto& [visualState, value] : valuesPerVisualState) {
                propertyOverrides.insert({visualState, value});
            }
        }
    }

    std::erase_if(overrides, [](const auto& item) {
        auto const& [key, value] = item;
        return value.empty();
    });

    return overrides;
}

void ApplyCustomizationsForVisualStateGroup(
    FrameworkElement element,
    VisualStateGroup visualStateGroup,
    PropertyOverrides propertyOverrides,
    ElementCustomizationStateForVisualStateGroup*
        elementCustomizationStateForVisualStateGroup) {
    auto elementDo = element.as<DependencyObject>();

    VisualState currentVisualState(
        visualStateGroup ? visualStateGroup.CurrentState() : nullptr);

    std::wstring currentVisualStateName(
        currentVisualState ? currentVisualState.Name() : L"");

    for (const auto& [property, valuesPerVisualState] : propertyOverrides) {
        const auto [propertyCustomizationStatesIt, inserted] =
            elementCustomizationStateForVisualStateGroup
                ->propertyCustomizationStates.insert({property, {}});
        if (!inserted) {
            continue;
        }

        auto& propertyCustomizationState =
            propertyCustomizationStatesIt->second;

        auto it = valuesPerVisualState.find(currentVisualStateName);
        if (it == valuesPerVisualState.end() &&
            !currentVisualStateName.empty()) {
            it = valuesPerVisualState.find(L"");
        }

        if (it != valuesPerVisualState.end()) {
            propertyCustomizationState.originalValue =
                ReadLocalValueWithWorkaround(element, property);
            propertyCustomizationState.customValue = it->second;
            SetOrClearValue(element, property, it->second);
        }

        propertyCustomizationState.propertyChangedToken =
            elementDo.RegisterPropertyChangedCallback(
                property,
                [&propertyCustomizationState](DependencyObject sender,
                                              DependencyProperty property) {
                    if (g_elementPropertyModifying) {
                        return;
                    }

                    auto element = sender.try_as<FrameworkElement>();
                    if (!element) {
                        return;
                    }

                    if (!propertyCustomizationState.customValue) {
                        return;
                    }

                    Wh_Log(L"Re-applying style for %s",
                           winrt::get_class_name(element).c_str());

                    auto localValue =
                        ReadLocalValueWithWorkaround(element, property);

                    if (*propertyCustomizationState.customValue != localValue) {
                        propertyCustomizationState.originalValue = localValue;
                    }

                    g_elementPropertyModifying = true;
                    SetOrClearValue(element, property,
                                    *propertyCustomizationState.customValue);
                    g_elementPropertyModifying = false;
                });
    }

    if (visualStateGroup) {
        winrt::weak_ref<FrameworkElement> elementWeakRef = element;
        elementCustomizationStateForVisualStateGroup
            ->visualStateGroupCurrentStateChangedToken =
            visualStateGroup.CurrentStateChanged(
                [elementWeakRef, propertyOverrides,
                 elementCustomizationStateForVisualStateGroup](
                    winrt::Windows::Foundation::IInspectable const& sender,
                    VisualStateChangedEventArgs const& e) {
                    auto element = elementWeakRef.get();
                    if (!element) {
                        return;
                    }

                    Wh_Log(L"Re-applying all styles for %s",
                           winrt::get_class_name(element).c_str());

                    g_elementPropertyModifying = true;

                    auto& propertyCustomizationStates =
                        elementCustomizationStateForVisualStateGroup
                            ->propertyCustomizationStates;

                    for (const auto& [property, valuesPerVisualState] :
                         propertyOverrides) {
                        auto& propertyCustomizationState =
                            propertyCustomizationStates.at(property);

                        auto newState = e.NewState();
                        auto newStateName =
                            std::wstring{newState ? newState.Name() : L""};
                        auto it = valuesPerVisualState.find(newStateName);
                        if (it == valuesPerVisualState.end()) {
                            it = valuesPerVisualState.find(L"");
                            if (it != valuesPerVisualState.end()) {
                                auto oldState = e.OldState();
                                auto oldStateName = std::wstring{
                                    oldState ? oldState.Name() : L""};
                                if (!valuesPerVisualState.contains(
                                        oldStateName)) {
                                    continue;
                                }
                            }
                        }

                        if (it != valuesPerVisualState.end()) {
                            if (!propertyCustomizationState.originalValue) {
                                propertyCustomizationState.originalValue =
                                    ReadLocalValueWithWorkaround(element,
                                                                 property);
                            }

                            propertyCustomizationState.customValue = it->second;
                            SetOrClearValue(element, property, it->second);
                        } else {
                            if (propertyCustomizationState.originalValue) {
                                SetOrClearValue(
                                    element, property,
                                    *propertyCustomizationState.originalValue);
                                propertyCustomizationState.originalValue
                                    .reset();
                            }

                            propertyCustomizationState.customValue.reset();
                        }
                    }

                    g_elementPropertyModifying = false;
                });
    }
}

void RestoreCustomizationsForVisualStateGroup(
    FrameworkElement element,
    std::optional<winrt::weak_ref<VisualStateGroup>>
        visualStateGroupOptionalWeakPtr,
    const ElementCustomizationStateForVisualStateGroup&
        elementCustomizationStateForVisualStateGroup) {
    if (element) {
        for (const auto& [property, state] :
             elementCustomizationStateForVisualStateGroup
                 .propertyCustomizationStates) {
            element.UnregisterPropertyChangedCallback(
                property, state.propertyChangedToken);

            if (state.originalValue) {
                SetOrClearValue(element, property, *state.originalValue);
            }
        }
    }

    auto visualStateGroupIter = visualStateGroupOptionalWeakPtr
                                    ? visualStateGroupOptionalWeakPtr->get()
                                    : nullptr;
    if (visualStateGroupIter && elementCustomizationStateForVisualStateGroup
                                    .visualStateGroupCurrentStateChangedToken) {
        visualStateGroupIter.CurrentStateChanged(
            elementCustomizationStateForVisualStateGroup
                .visualStateGroupCurrentStateChangedToken);
    }
}

void ApplyCustomizations(InstanceHandle handle,
                         FrameworkElement element,
                         PCWSTR fallbackClassName) {
    auto overrides = FindElementPropertyOverrides(element, fallbackClassName);
    if (overrides.empty()) {
        return;
    }

    Wh_Log(L"Applying styles");

    auto& elementCustomizationState = g_elementsCustomizationState[handle];

    for (const auto& [visualStateGroupOptionalWeakPtrIter, stateIter] :
         elementCustomizationState.perVisualStateGroup) {
        RestoreCustomizationsForVisualStateGroup(
            element, visualStateGroupOptionalWeakPtrIter, stateIter);
    }

    elementCustomizationState.element = element;
    elementCustomizationState.perVisualStateGroup.clear();

    for (auto& [visualStateGroup, overridesForVisualStateGroup] : overrides) {
        std::optional<winrt::weak_ref<VisualStateGroup>>
            visualStateGroupOptionalWeakPtr;
        if (visualStateGroup) {
            visualStateGroupOptionalWeakPtr = visualStateGroup;
        }

        elementCustomizationState.perVisualStateGroup.push_back(
            {visualStateGroupOptionalWeakPtr, {}});
        auto* elementCustomizationStateForVisualStateGroup =
            &elementCustomizationState.perVisualStateGroup.back().second;

        ApplyCustomizationsForVisualStateGroup(
            element, visualStateGroup, std::move(overridesForVisualStateGroup),
            elementCustomizationStateForVisualStateGroup);
    }
}

void CleanupCustomizations(InstanceHandle handle) {
    auto it = g_elementsCustomizationState.find(handle);
    if (it == g_elementsCustomizationState.end()) {
        return;
    }

    auto& elementCustomizationState = it->second;

    auto element = elementCustomizationState.element.get();

    for (const auto& [visualStateGroupOptionalWeakPtrIter, stateIter] :
         elementCustomizationState.perVisualStateGroup) {
        RestoreCustomizationsForVisualStateGroup(
            element, visualStateGroupOptionalWeakPtrIter, stateIter);
    }

    g_elementsCustomizationState.erase(it);
}

ElementMatcher ElementMatcherFromString(std::wstring_view str) {
    ElementMatcher result;
    PropertyValuesUnresolved propertyValuesUnresolved;

    auto i = str.find_first_of(L"#@[");
    result.type = TrimStringView(str.substr(0, i));
    if (result.type.empty()) {
        throw std::runtime_error("Bad target syntax, empty type");
    }

    while (i != str.npos) {
        auto iNext = str.find_first_of(L"#@[", i + 1);
        auto nextPart =
            str.substr(i + 1, iNext == str.npos ? str.npos : iNext - (i + 1));

        switch (str[i]) {
            case L'#':
                if (!result.name.empty()) {
                    throw std::runtime_error(
                        "Bad target syntax, more than one name");
                }

                result.name = TrimStringView(nextPart);
                if (result.name.empty()) {
                    throw std::runtime_error("Bad target syntax, empty name");
                }
                break;

            case L'@':
                if (result.visualStateGroupName) {
                    throw std::runtime_error(
                        "Bad target syntax, more than one visual state group");
                }

                result.visualStateGroupName = TrimStringView(nextPart);
                break;

            case L'[': {
                auto rule = TrimStringView(nextPart);
                if (rule.length() == 0 || rule.back() != L']') {
                    throw std::runtime_error("Bad target syntax, missing ']'");
                }

                rule = TrimStringView(rule.substr(0, rule.length() - 1));
                if (rule.length() == 0) {
                    throw std::runtime_error(
                        "Bad target syntax, empty property");
                }

                if (rule.find_first_not_of(L"0123456789") == rule.npos) {
                    result.oneBasedIndex = std::stoi(std::wstring(rule));
                    break;
                }

                auto ruleEqPos = rule.find(L'=');
                if (ruleEqPos == rule.npos) {
                    throw std::runtime_error(
                        "Bad target syntax, missing '=' in property");
                }

                auto ruleKey = TrimStringView(rule.substr(0, ruleEqPos));
                auto ruleVal = TrimStringView(rule.substr(ruleEqPos + 1));

                if (ruleKey.length() == 0) {
                    throw std::runtime_error(
                        "Bad target syntax, empty property name");
                }

                propertyValuesUnresolved.push_back(
                    {std::wstring(ruleKey), std::wstring(ruleVal)});
                break;
            }

            default:
                throw std::runtime_error("Bad target syntax");
        }

        i = iNext;
    }

    result.propertyValues = std::move(propertyValuesUnresolved);

    return result;
}

StyleRule StyleRuleFromString(std::wstring_view str) {
    StyleRule result;

    auto eqPos = str.find(L'=');
    if (eqPos == str.npos) {
        throw std::runtime_error("Bad style syntax, '=' is missing");
    }

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
        if (result.visualState.empty()) {
            throw std::runtime_error("Bad style syntax, empty visual state");
        }

        name = name.substr(0, atPos);
    }

    result.name = TrimStringView(name);
    if (result.name.empty()) {
        throw std::runtime_error("Bad style syntax, empty name");
    }

    return result;
}

std::wstring AdjustTypeName(std::wstring_view type) {
    if (type.find_first_of(L".:") == type.npos) {
        if (type == L"Rectangle") {
            return L"Windows.UI.Xaml.Shapes.Rectangle";
        }

        return L"Windows.UI.Xaml.Controls." + std::wstring{type};
    }

    static const std::vector<std::pair<std::wstring_view, std::wstring_view>>
        adjustments = {
            {L"muxc:", L"Microsoft.UI.Xaml.Controls."},
        };

    for (const auto& adjustment : adjustments) {
        if (type.starts_with(adjustment.first)) {
            auto result = std::wstring{adjustment.second};
            result += type.substr(adjustment.first.size());
            return result;
        }
    }

    return std::wstring{type};
}

void AddElementCustomizationRules(std::wstring_view target,
                                  std::vector<std::wstring> styles) {
    ElementCustomizationRules elementCustomizationRules;

    auto targetParts = SplitStringView(target, L" > ");

    bool first = true;
    bool hasVisualStateGroup = false;
    for (auto i = targetParts.rbegin(); i != targetParts.rend(); ++i) {
        const auto& targetPart = *i;

        auto matcher = ElementMatcherFromString(targetPart);
        matcher.type = AdjustTypeName(matcher.type);

        if (matcher.visualStateGroupName) {
            if (hasVisualStateGroup) {
                throw std::runtime_error(
                    "Element type can't have more than one visual state group");
            }

            hasVisualStateGroup = true;
        }

        if (first) {
            std::vector<StyleRule> styleRules;
            for (const auto& style : styles) {
                styleRules.push_back(StyleRuleFromString(style));
            }

            elementCustomizationRules.elementMatcher = std::move(matcher);
            elementCustomizationRules.propertyOverrides = std::move(styleRules);
        } else {
            elementCustomizationRules.parentElementMatchers.push_back(
                std::move(matcher));
        }

        first = false;
    }

    g_elementsCustomizationRules.push_back(
        std::move(elementCustomizationRules));
}

bool ProcessSingleTargetStylesFromSettings(int index) {
    string_setting_unique_ptr targetStringSetting(
        Wh_GetStringSetting(L"controlStyles[%d].target", index));
    if (!*targetStringSetting.get()) {
        return false;
    }

    Wh_Log(L"Processing styles for %s", targetStringSetting.get());

    std::vector<std::wstring> styles;

    for (int styleIndex = 0;; styleIndex++) {
        string_setting_unique_ptr styleSetting(Wh_GetStringSetting(
            L"controlStyles[%d].styles[%d]", index, styleIndex));
        if (!*styleSetting.get()) {
            break;
        }

        // Skip if commented.
        if (styleSetting[0] == L'/' && styleSetting[1] == L'/') {
            continue;
        }

        styles.push_back(styleSetting.get());
    }

    if (styles.size() > 0) {
        AddElementCustomizationRules(targetStringSetting.get(),
                                     std::move(styles));
    }

    return true;
}

void ProcessAllStylesFromSettings() {
    for (int i = 0;; i++) {
        try {
            if (!ProcessSingleTargetStylesFromSettings(i)) {
                break;
            }
        } catch (winrt::hresult_error const& ex) {
            Wh_Log(L"Error %08X: %s", ex.code(), ex.message().c_str());
        } catch (std::exception const& ex) {
            Wh_Log(L"Error: %S", ex.what());
        }
    }
}

bool ProcessSingleResourceVariableFromSettings(int index) {
    string_setting_unique_ptr variableKeyStringSetting(
        Wh_GetStringSetting(L"resourceVariables[%d].variableKey", index));
    if (!*variableKeyStringSetting.get()) {
        return false;
    }

    Wh_Log(L"Processing resource variable %s", variableKeyStringSetting.get());

    std::wstring_view variableKey = variableKeyStringSetting.get();

    auto resources = Application::Current().Resources();

    auto resource = resources.Lookup(winrt::box_value(variableKey));

    // Example: Windows.Foundation.IReference`1<Windows.UI.Xaml.Thickness>
    auto resourceClassName = winrt::get_class_name(resource);
    if (resourceClassName.starts_with(L"Windows.Foundation.IReference`1<") &&
        resourceClassName.ends_with(L'>')) {
        size_t prefixSize = sizeof("Windows.Foundation.IReference`1<") - 1;
        resourceClassName =
            winrt::hstring(resourceClassName.data() + prefixSize,
                           resourceClassName.size() - prefixSize - 1);
    }

    auto resourceTypeName = Interop::TypeName{resourceClassName};

    string_setting_unique_ptr valueStringSetting(
        Wh_GetStringSetting(L"resourceVariables[%d].value", index));

    std::wstring_view value = valueStringSetting.get();

    resources.Insert(winrt::box_value(variableKey),
                     Markup::XamlBindingHelper::ConvertValue(
                         resourceTypeName, winrt::box_value(value)));

    return true;
}

void ProcessResourceVariablesFromSettings() {
    for (int i = 0;; i++) {
        try {
            if (!ProcessSingleResourceVariableFromSettings(i)) {
                break;
            }
        } catch (winrt::hresult_error const& ex) {
            Wh_Log(L"Error %08X: %s", ex.code(), ex.message().c_str());
        } catch (std::exception const& ex) {
            Wh_Log(L"Error: %S", ex.what());
        }
    }
}

void UninitializeSettingsAndTap() {
    for (const auto& [handle, elementCustomizationState] :
         g_elementsCustomizationState) {
        auto element = elementCustomizationState.element.get();

        for (const auto& [visualStateGroupOptionalWeakPtrIter, stateIter] :
             elementCustomizationState.perVisualStateGroup) {
            RestoreCustomizationsForVisualStateGroup(
                element, visualStateGroupOptionalWeakPtrIter, stateIter);
        }
    }

    g_elementsCustomizationState.clear();

    g_elementsCustomizationRules.clear();

    g_targetThreadId = 0;
}

void InitializeSettingsAndTap() {
    DWORD kNoThreadId = 0;
    if (!g_targetThreadId.compare_exchange_strong(kNoThreadId,
                                                  GetCurrentThreadId())) {
        return;
    }

    ProcessAllStylesFromSettings();
    ProcessResourceVariablesFromSettings();

    HRESULT hr = InjectWindhawkTAP();
    if (FAILED(hr)) {
        Wh_Log(L"Error %08X", hr);
    }
}

bool IsTargetThreadDescription(PCWSTR threadDescription) {
    return wcscmp(threadDescription, L"ActionCenter") == 0;
}

bool IsTargetThread(HANDLE thread) {
    using GetThreadDescription_t =
        HRESULT(WINAPI*)(HANDLE hThread, PWSTR * ppszThreadDescription);

    static auto pGetThreadDescription = []() -> GetThreadDescription_t {
        HMODULE hKernel32Module = LoadLibrary(L"kernel32.dll");
        if (!hKernel32Module) {
            return nullptr;
        }

        return (GetThreadDescription_t)GetProcAddress(hKernel32Module,
                                                      "GetThreadDescription");
    }();

    if (!pGetThreadDescription) {
        return false;
    }

    PWSTR threadDescription;
    bool isTargetThread = false;
    HRESULT hr = pGetThreadDescription(thread, &threadDescription);
    if (SUCCEEDED(hr)) {
        isTargetThread = IsTargetThreadDescription(threadDescription);
        LocalFree(threadDescription);
    }

    return isTargetThread;
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
    if (!hWnd || !IsTargetThread(GetCurrentThread())) {
        return hWnd;
    }

    BOOL bTextualClassName = ((ULONG_PTR)lpClassName & ~(ULONG_PTR)0xffff) != 0;

    if (bTextualClassName &&
        _wcsicmp(lpClassName, L"Windows.UI.Core.CoreWindow") == 0) {
        Wh_Log(L"Initializing - Created core window: %08X",
               (DWORD)(ULONG_PTR)hWnd);
        InitializeSettingsAndTap();
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
    if (!hWnd || !IsTargetThread(GetCurrentThread())) {
        return hWnd;
    }

    BOOL bTextualClassName = ((ULONG_PTR)lpClassName & ~(ULONG_PTR)0xffff) != 0;

    if (bTextualClassName &&
        _wcsicmp(lpClassName, L"Windows.UI.Core.CoreWindow") == 0) {
        Wh_Log(L"Initializing - Created core window: %08X",
               (DWORD)(ULONG_PTR)hWnd);
        InitializeSettingsAndTap();
    }

    return hWnd;
}

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
        [](int nCode, WPARAM wParam, LPARAM lParam) WINAPI -> LRESULT {
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

HWND GetCoreWnd() {
    struct ENUM_WINDOWS_PARAM {
        HWND* hWnd;
    };

    HWND hWnd = nullptr;
    ENUM_WINDOWS_PARAM param = {&hWnd};
    EnumWindows(
        [](HWND hWnd, LPARAM lParam) WINAPI -> BOOL {
            ENUM_WINDOWS_PARAM& param = *(ENUM_WINDOWS_PARAM*)lParam;

            DWORD dwProcessId = 0;
            DWORD dwThreadId = GetWindowThreadProcessId(hWnd, &dwProcessId);
            if (!dwThreadId || dwProcessId != GetCurrentProcessId()) {
                return TRUE;
            }

            bool isTargetThread = false;
            HANDLE hThread =
                OpenThread(THREAD_QUERY_LIMITED_INFORMATION, FALSE, dwThreadId);
            if (hThread) {
                isTargetThread = IsTargetThread(hThread);
                CloseHandle(hThread);
            }

            if (!isTargetThread) {
                return TRUE;
            }

            WCHAR szClassName[32];
            if (GetClassName(hWnd, szClassName, ARRAYSIZE(szClassName)) == 0) {
                return TRUE;
            }

            if (_wcsicmp(szClassName, L"Windows.UI.Core.CoreWindow") == 0) {
                *param.hWnd = hWnd;
                return FALSE;
            }

            return TRUE;
        },
        (LPARAM)&param);

    return hWnd;
}

HWND GetCoreWndOfThread(DWORD threadId) {
    struct ENUM_WINDOWS_PARAM {
        HWND* hWnd;
    };

    HWND hWnd = nullptr;
    ENUM_WINDOWS_PARAM param = {&hWnd};
    EnumThreadWindows(
        threadId,
        [](HWND hWnd, LPARAM lParam) WINAPI -> BOOL {
            ENUM_WINDOWS_PARAM& param = *(ENUM_WINDOWS_PARAM*)lParam;

            WCHAR szClassName[32];
            if (GetClassName(hWnd, szClassName, ARRAYSIZE(szClassName)) == 0) {
                return TRUE;
            }

            if (_wcsicmp(szClassName, L"Windows.UI.Core.CoreWindow") == 0) {
                *param.hWnd = hWnd;
                return FALSE;
            }

            return TRUE;
        },
        (LPARAM)&param);

    return hWnd;
}

using SetThreadDescription_t = HRESULT(WINAPI*)(HANDLE hThread,
                                                PCWSTR lpThreadDescription);
SetThreadDescription_t SetThreadDescription_Original;
HRESULT WINAPI SetThreadDescription_Hook(HANDLE hThread,
                                         PCWSTR lpThreadDescription) {
    Wh_Log(L"> %s", lpThreadDescription ? lpThreadDescription : L"(null)");

    if (lpThreadDescription && IsTargetThreadDescription(lpThreadDescription)) {
        HWND hCoreWnd = GetCoreWndOfThread(GetThreadId(hThread));
        if (hCoreWnd) {
            Wh_Log(L"Initializing - Thread description was set");
            RunFromWindowThread(
                hCoreWnd, [](PVOID) WINAPI { InitializeSettingsAndTap(); },
                nullptr);
        }
    }

    return SetThreadDescription_Original(hThread, lpThreadDescription);
}

BOOL Wh_ModInit() {
    Wh_Log(L">");

    HMODULE user32Module = LoadLibrary(L"user32.dll");
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

    HMODULE hKernel32Module = LoadLibrary(L"kernel32.dll");
    if (hKernel32Module) {
        void* pSetThreadDescription =
            (void*)GetProcAddress(hKernel32Module, "SetThreadDescription");
        if (pSetThreadDescription) {
            Wh_SetFunctionHook(pSetThreadDescription,
                               (void*)SetThreadDescription_Hook,
                               (void**)&SetThreadDescription_Original);
        }
    }

    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L">");

    HWND hCoreWnd = GetCoreWnd();
    if (hCoreWnd) {
        Wh_Log(L"Initializing - Found core window");
        RunFromWindowThread(
            hCoreWnd, [](PVOID) WINAPI { InitializeSettingsAndTap(); },
            nullptr);
    }
}

void Wh_ModUninit() {
    Wh_Log(L">");

    if (g_visualTreeWatcher) {
        g_visualTreeWatcher->UnadviseVisualTreeChange();
        g_visualTreeWatcher = nullptr;
    }

    HWND hCoreWnd = GetCoreWnd();
    if (hCoreWnd) {
        Wh_Log(L"Uninitializing - Found core window");
        RunFromWindowThread(
            hCoreWnd, [](PVOID) WINAPI { UninitializeSettingsAndTap(); },
            nullptr);
    }
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");

    if (g_visualTreeWatcher) {
        g_visualTreeWatcher->UnadviseVisualTreeChange();
        g_visualTreeWatcher = nullptr;
    }

    HWND hCoreWnd = GetCoreWnd();
    if (hCoreWnd) {
        Wh_Log(L"Reinitializing - Found core window");
        RunFromWindowThread(
            hCoreWnd,
            [](PVOID) WINAPI {
                UninitializeSettingsAndTap();
                InitializeSettingsAndTap();
            },
            nullptr);
    }
}
