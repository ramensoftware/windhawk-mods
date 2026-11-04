// ==WindhawkMod==
// @id              windows-11-minimal-start-menu
// @name            Windows 11 Minimal Start Menu
// @description     Hides the All Apps section in the redesigned Windows 11 Start menu on Windows 11 build 26200.8655 and newer while keeping search and pinned apps.
// @version         1.5
// @author          Gaika
// @github          https://github.com/tchack
// @include         StartMenuExperienceHost.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Windows 11 Minimal Start Menu

This mod customizes the redesigned Windows 11 Start menu in
`StartMenuExperienceHost.exe`. It targets the new Start menu in Windows 11
build 26200.8655 and newer.

Features:
- Hides the All Apps block in the new UWP/XAML Start menu.
- Keeps the search box and pinned apps visible.
- Hides the "All" heading and the view selector row.
- Renames the pinned apps heading to a custom text.
- Forces the compact Start menu layout by default so the same pinned grid width
  is used across monitors.

The optional "Enable large Start menu" setting restores the native large Start
menu visual state on displays where Windows chooses it.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- enableLargeStartMenu: false
  $name: Enable large Start menu
  $description: Allow Windows to use the native large Start menu layout. When disabled, the mod forces the compact layout across monitors.
- pinnedHeaderText: Закреплено
  $name: Pinned apps heading text
  $description: Text shown above the pinned apps grid.
*/
// ==/WindhawkModSettings==

#include <xamlom.h>

#include <atomic>
#include <vector>

#undef GetCurrentTime

#include <winrt/Windows.UI.Xaml.h>

enum class StyleRule {
    Height0,
    MinHeight0,
    MaxHeight0,
    Width0,
    MinWidth0,
    MaxWidth0,
    Margin0,
    Padding0,
    VisibilityCollapsed,
    GridRow0,
    MaxHeight54,
    TranslateXMinus135,
    TranslateY22,
};

struct ThemeTargetStyles {
    PCWSTR target;
    std::vector<StyleRule> styles;
};

struct Theme {
    std::vector<ThemeTargetStyles> targetStyles;
};

// clang-format off

const Theme g_themeWithoutAllApps = {{
    ThemeTargetStyles{L"Grid#AllListHeading", {
        StyleRule::Height0,
        StyleRule::MinHeight0,
        StyleRule::MaxHeight0,
        StyleRule::Margin0,
        StyleRule::VisibilityCollapsed}},
    ThemeTargetStyles{L"TextBlock#AllListHeadingText", {
        StyleRule::VisibilityCollapsed}},
    ThemeTargetStyles{L"Microsoft.UI.Xaml.Controls.DropDownButton#ViewSelectionButton", {
        StyleRule::Width0,
        StyleRule::MinWidth0,
        StyleRule::MaxWidth0,
        StyleRule::Margin0,
        StyleRule::VisibilityCollapsed}},
    ThemeTargetStyles{L"StartMenu.CategoryControl", {
        StyleRule::Height0,
        StyleRule::MinHeight0,
        StyleRule::MaxHeight0,
        StyleRule::Margin0,
        StyleRule::VisibilityCollapsed}},
    ThemeTargetStyles{L"StartDocked.AllAppsGridListViewItem", {
        StyleRule::Height0,
        StyleRule::MinHeight0,
        StyleRule::MaxHeight0,
        StyleRule::Margin0,
        StyleRule::VisibilityCollapsed}},
    ThemeTargetStyles{L"StartDocked.AllAppsGridListView#AppsList", {
        StyleRule::Width0,
        StyleRule::MinWidth0,
        StyleRule::MaxWidth0,
        StyleRule::Padding0,
        StyleRule::Margin0,
        StyleRule::VisibilityCollapsed}},
    ThemeTargetStyles{L"Grid#TopLevelSuggestionsListHeader", {
        StyleRule::Height0,
        StyleRule::VisibilityCollapsed}},
    ThemeTargetStyles{L"Grid#TopLevelSuggestionsRoot", {
        StyleRule::GridRow0,
        StyleRule::MaxHeight54,
        StyleRule::TranslateY22}},
    ThemeTargetStyles{L"Grid#ShowMoreSuggestions", {
        StyleRule::VisibilityCollapsed,
        StyleRule::Margin0}},
    ThemeTargetStyles{L"Grid#TopLevelHeader > Grid[2] > Button", {
        StyleRule::TranslateXMinus135}},
}};

// clang-format on

std::atomic<DWORD> g_targetThreadId = 0;

void ApplyCustomizations(InstanceHandle handle,
                         winrt::Windows::UI::Xaml::FrameworkElement element,
                         PCWSTR fallbackClassName);
void CleanupCustomizations(InstanceHandle handle);
void ForceSmallStartLayout();
void ForceSmallStartLayout(
    winrt::Windows::UI::Xaml::FrameworkElement const& element);

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

#include <Unknwn.h>
#include <winrt/base.h>

// forward declare namespaces we alias
namespace winrt {
    namespace Windows {
        namespace Foundation {}
        namespace UI::Xaml {}
    }
}

// alias some long namespaces for convenience
namespace wf = winrt::Windows::Foundation;
namespace wux = winrt::Windows::UI::Xaml;

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

    wf::IInspectable FromHandle(InstanceHandle handle)
    {
        wf::IInspectable obj;
        winrt::check_hresult(m_XamlDiagnostics->GetIInspectableFromHandle(handle, reinterpret_cast<::IInspectable**>(winrt::put_abi(obj))));
        return obj;
    }

    winrt::com_ptr<IXamlDiagnostics> m_XamlDiagnostics = nullptr;
};

#pragma endregion  // visualtreewatcher_hpp

#pragma region visualtreewatcher_cpp

VisualTreeWatcher::VisualTreeWatcher(winrt::com_ptr<IUnknown> site) :
    m_XamlDiagnostics(site.as<IXamlDiagnostics>())
{
    // winrt::check_hresult(m_XamlDiagnostics.as<IVisualTreeService3>()->AdviseVisualTreeChange(this));

    // Calling AdviseVisualTreeChange from the current thread causes the app to
    // hang in Advising::RunOnUIThread sometimes. Creating a new thread and
    // calling it from there fixes it.
    HANDLE thread = CreateThread(
        nullptr, 0,
        [](LPVOID lpParam) -> DWORD {
            auto watcher = reinterpret_cast<VisualTreeWatcher*>(lpParam);
            HRESULT hr = watcher->m_XamlDiagnostics.as<IVisualTreeService3>()->AdviseVisualTreeChange(watcher);
            watcher->Release();
            if (FAILED(hr)) {
                Wh_Log(L"Error %08X", hr);
            }
            return 0;
        },
        this, 0, nullptr);
    if (thread) {
        AddRef();
        CloseHandle(thread);
    }
}

VisualTreeWatcher::~VisualTreeWatcher()
{
}

void VisualTreeWatcher::UnadviseVisualTreeChange()
{
    HRESULT hr = m_XamlDiagnostics.as<IVisualTreeService3>()->UnadviseVisualTreeChange(this);
    if (FAILED(hr)) {
        Wh_Log(L"UnadviseVisualTreeChange failed with error %08X", hr);
    }
}

HRESULT VisualTreeWatcher::OnVisualTreeChange(ParentChildRelation, VisualElement element, VisualMutationType mutationType) try
{
    if (GetCurrentThreadId() != g_targetThreadId)
    {
        return S_OK;
    }

    if (mutationType == Add)
    {
        const auto inspectable = FromHandle(element.Handle);
        auto frameworkElement = inspectable.try_as<wux::FrameworkElement>();
        if (frameworkElement)
        {
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
    try {
        ForceSmallStartLayout();
    } catch (...) {
        Wh_Log(L"Error %08X", winrt::to_hresult());
    }

    return S_OK;
}

#pragma endregion  // visualtreewatcher_cpp

#pragma region tap_hpp

#include <ocidl.h>

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
    HRESULT hr = winrt::to_hresult();
    Wh_Log(L"Error %08X", hr);
    return hr;
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
        HRESULT hr = winrt::to_hresult();
        Wh_Log(L"Error %08X", hr);
        return hr;
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

#pragma endregion  // api_cpp

// clang-format on
////////////////////////////////////////////////////////////////////////////////

#include <algorithm>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <initguid.h>

#include <roapi.h>
#include <winstring.h>

#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.h>

using namespace winrt::Windows::UI::Xaml;

bool g_visualStateManagerHooked = false;
bool g_enableLargeStartMenu = false;
WCHAR g_pinnedHeaderText[128] = L"Закреплено";
Controls::Control g_startBlendedFlexFrame{nullptr};

void LoadSettings() {
    g_enableLargeStartMenu = Wh_GetIntSetting(L"enableLargeStartMenu") != 0;

    PCWSTR pinnedHeaderText = Wh_GetStringSetting(L"pinnedHeaderText");
    lstrcpynW(g_pinnedHeaderText, pinnedHeaderText, ARRAYSIZE(g_pinnedHeaderText));
    Wh_FreeStringSetting(pinnedHeaderText);
}

using VisualStateManagerGoToState_t =
    HRESULT(WINAPI*)(void* self,
                     void* control,
                     void* stateName,
                     bool useTransitions,
                     bool* result);
VisualStateManagerGoToState_t VisualStateManagerGoToState_Original;

HRESULT WINAPI VisualStateManagerGoToState_Hook(void* self,
                                                void* controlAbi,
                                                void* stateNameAbi,
                                                bool useTransitions,
                                                bool* result) {
    void* effectiveStateNameAbi = stateNameAbi;
    winrt::hstring smallStartState;

    try {
        Controls::Control control = nullptr;
        winrt::copy_from_abi(control, controlAbi);

        winrt::hstring stateName;
        winrt::copy_from_abi(stateName, stateNameAbi);

        if (!g_enableLargeStartMenu && control &&
            winrt::get_class_name(control) ==
                L"StartMenu.StartBlendedFlexFrame" &&
            (stateName == L"LargeStart" ||
             stateName == L"LargeStart_WithCompanion")) {
            smallStartState = L"SmallStart";
            effectiveStateNameAbi = winrt::get_abi(smallStartState);
            useTransitions = false;
        }
    } catch (...) {
    }

    return VisualStateManagerGoToState_Original(
        self, controlAbi, effectiveStateNameAbi, useTransitions, result);
}

void HookVisualStateManagerGoToState() {
    if (g_visualStateManagerHooked) {
        return;
    }

    try {
        auto statics =
            winrt::get_activation_factory<VisualStateManager,
                                          IVisualStateManagerStatics>();
        auto vtable =
            *reinterpret_cast<void***>(winrt::get_abi(statics));

        constexpr size_t kGoToStateVtableIndex = 10;
        void* goToState = vtable[kGoToStateVtableIndex];
        if (!goToState) {
            return;
        }

        Wh_SetFunctionHook(
            goToState,
            reinterpret_cast<void*>(VisualStateManagerGoToState_Hook),
            reinterpret_cast<void**>(&VisualStateManagerGoToState_Original));
        g_visualStateManagerHooked = true;
    } catch (...) {
        Wh_Log(L"HookVisualStateManagerGoToState error: %08X",
               winrt::to_hresult());
    }
}

void ForceSmallStartLayout(FrameworkElement const& element) {
    if (g_enableLargeStartMenu) {
        return;
    }

    if (winrt::get_class_name(element) == L"StartMenu.StartBlendedFlexFrame") {
        if (auto control = element.try_as<Controls::Control>()) {
            g_startBlendedFlexFrame = control;
        }
    }

    if (g_startBlendedFlexFrame) {
        VisualStateManager::GoToState(g_startBlendedFlexFrame, L"SmallStart",
                                      false);
    }
}

void ForceSmallStartLayout() {
    if (g_enableLargeStartMenu) {
        return;
    }

    if (g_startBlendedFlexFrame) {
        VisualStateManager::GoToState(g_startBlendedFlexFrame, L"SmallStart",
                                      false);
    }
}

struct ElementMatcher {
    std::wstring type;
    std::wstring name;
    int oneBasedIndex = 0;
};

using PropertyOverrideValue = winrt::Windows::Foundation::IInspectable;

using PropertyOverrides =
    std::unordered_map<DependencyProperty, PropertyOverrideValue>;

struct ElementCustomizationRules {
    ElementMatcher elementMatcher;
    std::vector<ElementMatcher> parentElementMatchers;
    std::vector<StyleRule> styles;
};

std::vector<ElementCustomizationRules> g_elementsCustomizationRules;

struct ElementPropertyCustomizationState {
    std::optional<winrt::Windows::Foundation::IInspectable> originalValue;
    // The most recently applied value, re-pushed by the per-DP
    // property-changed callback when something external (animation, system
    // Setter) overrides it.
    std::optional<PropertyOverrideValue> customValue;
    winrt::Windows::Foundation::IInspectable lastAppliedValue{nullptr};
    int64_t propertyChangedToken = 0;
};

struct ElementCustomizationState {
    winrt::weak_ref<FrameworkElement> element;

    std::unordered_map<DependencyProperty, ElementPropertyCustomizationState>
        propertyCustomizationStates;
};

std::unordered_map<InstanceHandle, ElementCustomizationState>
    g_elementsCustomizationState;

bool g_elementPropertyModifying;

bool g_windowsDefaultIsNewLayout = false;

bool DoesLayoutOverrideMatchWindowsDefault() {
    return g_windowsDefaultIsNewLayout;
}

winrt::Windows::Foundation::IInspectable ReadLocalValueWithWorkaround(
    DependencyObject elementDo,
    DependencyProperty property) {
    auto value = elementDo.ReadLocalValue(property);

    if (value) {
        auto className = winrt::get_class_name(value);
        if (className == L"Windows.UI.Xaml.Data.BindingExpressionBase" ||
            className == L"Windows.UI.Xaml.Data.BindingExpression") {
            // BindingExpressionBase was observed to be returned for XAML
            // properties that were declared as following:
            //
            // <Border ... CornerRadius="{TemplateBinding CornerRadius}" />
            //
            // Calling SetValue with it fails with an error, so we won't be able
            // to use it to restore the value. As a workaround, we use
            // GetAnimationBaseValue to get the value.
            value = elementDo.GetAnimationBaseValue(property);
        }
    }

    return value;
}

void SetOrClearValue(DependencyObject elementDo,
                     DependencyProperty property,
                     const PropertyOverrideValue& overrideValue) {
    winrt::Windows::Foundation::IInspectable value = overrideValue;

    if (value == DependencyProperty::UnsetValue()) {
        try {
            elementDo.ClearValue(property);
        } catch (winrt::hresult_error const& ex) {
            Wh_Log(L"Error %08X: %s", ex.code(), ex.message().c_str());
        }
        return;
    }

    try {
        elementDo.SetValue(property, value);
    } catch (winrt::hresult_error const& ex) {
        Wh_Log(L"Error %08X: %s", ex.code(), ex.message().c_str());
    }
}

std::wstring_view TrimStringView(std::wstring_view s) {
    s.remove_prefix(std::min(s.find_first_not_of(L" \t\r\v\n"), s.size()));
    s.remove_suffix(
        std::min(s.size() - s.find_last_not_of(L" \t\r\v\n") - 1, s.size()));
    return s;
}

std::vector<std::wstring_view> SplitStringView(std::wstring_view s,
                                               std::wstring_view delimiter) {
    size_t pos_start = 0, pos_end, delim_len = delimiter.length();
    std::vector<std::wstring_view> res;

    while ((pos_end = s.find(delimiter, pos_start)) !=
           std::wstring_view::npos) {
        res.push_back(s.substr(pos_start, pos_end - pos_start));
        pos_start = pos_end + delim_len;
    }

    res.push_back(s.substr(pos_start));
    return res;
}

void AddStyleOverride(PropertyOverrides& overrides, StyleRule style) {
    auto zero = winrt::box_value(0.0);
    auto zeroThickness = winrt::box_value(Thickness{0.0, 0.0, 0.0, 0.0});

    switch (style) {
        case StyleRule::Height0:
            overrides[FrameworkElement::HeightProperty()] = zero;
            break;
        case StyleRule::MinHeight0:
            overrides[FrameworkElement::MinHeightProperty()] = zero;
            break;
        case StyleRule::MaxHeight0:
            overrides[FrameworkElement::MaxHeightProperty()] = zero;
            break;
        case StyleRule::Width0:
            overrides[FrameworkElement::WidthProperty()] = zero;
            break;
        case StyleRule::MinWidth0:
            overrides[FrameworkElement::MinWidthProperty()] = zero;
            break;
        case StyleRule::MaxWidth0:
            overrides[FrameworkElement::MaxWidthProperty()] = zero;
            break;
        case StyleRule::Margin0:
            overrides[FrameworkElement::MarginProperty()] = zeroThickness;
            break;
        case StyleRule::Padding0:
            overrides[Controls::Control::PaddingProperty()] = zeroThickness;
            break;
        case StyleRule::VisibilityCollapsed:
            overrides[UIElement::VisibilityProperty()] =
                winrt::box_value(Visibility::Collapsed);
            break;
        case StyleRule::GridRow0:
            overrides[Controls::Grid::RowProperty()] = winrt::box_value(0);
            break;
        case StyleRule::MaxHeight54:
            overrides[FrameworkElement::MaxHeightProperty()] =
                winrt::box_value(54.0);
            break;
        case StyleRule::TranslateXMinus135: {
            Media::TranslateTransform transform;
            transform.X(-135.0);
            overrides[UIElement::RenderTransformProperty()] =
                transform.as<winrt::Windows::Foundation::IInspectable>();
            break;
        }
        case StyleRule::TranslateY22: {
            Media::TranslateTransform transform;
            transform.Y(22.0);
            overrides[UIElement::RenderTransformProperty()] =
                transform.as<winrt::Windows::Foundation::IInspectable>();
            break;
        }
    }
}
bool TestElementMatcher(FrameworkElement element,
                        ElementMatcher& matcher,
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

    return true;
}

struct MatchedElementRules {
    PropertyOverrides propertyOverrides;
};

MatchedElementRules FindElementPropertyOverrides(FrameworkElement element,
                                                 PCWSTR fallbackClassName) {
    MatchedElementRules result;
    std::unordered_set<DependencyProperty> propertiesAdded;

    for (auto it = g_elementsCustomizationRules.rbegin();
         it != g_elementsCustomizationRules.rend(); ++it) {
        auto& override = *it;

        if (fallbackClassName &&
            wcscmp(fallbackClassName, L"Windows.UI.Xaml.PopupRoot") == 0 &&
            !override.elementMatcher.type.empty() &&
            override.elementMatcher.type != L"Windows.UI.Xaml.PopupRoot") {
            // PopupRoot is expected to be the root element. Its class name is
            // Canvas, but its fallback class name is PopupRoot. Only match
            // PopupRoot to prevent colliding with Canvas rules.
            continue;
        }

        if (!TestElementMatcher(element, override.elementMatcher,
                                fallbackClassName)) {
            continue;
        }

        // Using iter.Parent() was sometimes returning null, so use
        // VisualTreeHelper::GetParent below instead.
        //
        // Match the direct parent chain declared with `>` selectors.
        auto& parentMatchers = override.parentElementMatchers;
        auto matchParents = [&](auto& self, FrameworkElement iter,
                                size_t mi) -> bool {
            if (mi >= parentMatchers.size()) {
                return true;
            }

            auto& matcher = parentMatchers[mi];

            auto parent = Media::VisualTreeHelper::GetParent(iter)
                              .try_as<FrameworkElement>();
            if (!parent) {
                return false;
            }

            if (!TestElementMatcher(parent, matcher, nullptr)) {
                return false;
            }

            return self(self, parent, mi + 1);
        };

        if (!matchParents(matchParents, element, 0)) {
            continue;
        }

        PropertyOverrides propertyOverrides;
        for (const auto& style : override.styles) {
            AddStyleOverride(propertyOverrides, style);
        }

        for (const auto& [property, value] : propertyOverrides) {
            bool propertyInserted = propertiesAdded.insert(property).second;
            if (!propertyInserted) {
                continue;
            }

            result.propertyOverrides.insert({property, value});
        }
    }

    return result;
}

void ApplyPropertyCustomizations(FrameworkElement element,
                                 PropertyOverrides propertyOverrides,
                                 ElementCustomizationState* elementState) {
    auto elementDo = element.as<DependencyObject>();

    for (const auto& [property, value] : propertyOverrides) {
        const auto [propertyCustomizationStatesIt, inserted] =
            elementState->propertyCustomizationStates.insert({property, {}});
        if (!inserted) {
            continue;
        }

        auto& propertyCustomizationState =
            propertyCustomizationStatesIt->second;

        if (value) {
            propertyCustomizationState.originalValue =
                ReadLocalValueWithWorkaround(element, property);
            propertyCustomizationState.customValue = value;
            SetOrClearValue(element, property, value);
            propertyCustomizationState.lastAppliedValue =
                ReadLocalValueWithWorkaround(element, property);
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
                    if (!element || !propertyCustomizationState.customValue) {
                        return;
                    }

                    auto localValue =
                        ReadLocalValueWithWorkaround(element, property);

                    if (localValue !=
                        propertyCustomizationState.lastAppliedValue) {
                        propertyCustomizationState.originalValue = localValue;
                    }

                    g_elementPropertyModifying = true;
                    SetOrClearValue(element, property,
                                    *propertyCustomizationState.customValue);
                    propertyCustomizationState.lastAppliedValue =
                        ReadLocalValueWithWorkaround(element, property);
                    g_elementPropertyModifying = false;
                });
    }
}

void RestorePropertyCustomizations(
    FrameworkElement element,
    const ElementCustomizationState& elementCustomizationState) {
    if (element) {
        for (const auto& [property, propState] :
             elementCustomizationState.propertyCustomizationStates) {
            try {
                element.UnregisterPropertyChangedCallback(
                    property, propState.propertyChangedToken);
            } catch (winrt::hresult_error const& ex) {
                Wh_Log(L"Error %08X: %s", ex.code(), ex.message().c_str());
            }

            if (propState.originalValue) {
                SetOrClearValue(element, property, *propState.originalValue);
            }
        }
    }
}
// === Old layout pinned items scroll ===
//
// The pinned items header, the show-more-pinned button, the pinned tiles, and
// the recommended panel are reparented from the apps list's ScrollViewer into
// a new ScrollViewer that lives as a sibling of the apps ScrollViewer. Final
// structure under
// `GridView#AllAppsGrid > Border`:
//
//   Border > Grid#OldLayoutPinnedWrapper
//              > ScrollViewer (original, scrolls the apps list)
//              > ScrollViewer#OldLayoutPinnedScrollViewer
//                  > Grid#OldLayoutPinnedContent
//                      > Grid#PinnedListHeaderGrid (moved)
//                      > Grid#ShowMorePinnedGrid (moved)
//                      > StartMenu.PinnedList#StartMenuPinnedList (moved)
//                      > Grid#TopLevelSuggestionsRoot (moved)
//
// The wrapper and the new ScrollViewer/content grid are named for stable
// diagnostics and property workarounds. The reparenting is idempotent and
// deferred to the next dispatcher tick to avoid re-entering the visual tree
// watcher mid-mutation.

constexpr WCHAR kOldLayoutPinnedWrapperName[] = L"OldLayoutPinnedWrapper";
constexpr WCHAR kOldLayoutPinnedScrollName[] = L"OldLayoutPinnedScrollViewer";
constexpr WCHAR kOldLayoutPinnedContentName[] = L"OldLayoutPinnedContent";

// Tracking for runtime enable/disable. We remember each restructured Border
// (so we can revert it on toggle-off) and a weak ref to the source
// TopLevelHeader (so revert can move the children back without re-walking
// the visual tree to find it). Entries survive across Initialize/Uninitialize
// cycles and are only cleared by `RevertAllOldLayoutPinnedScroll`. The
// Wh_ModSettingsChanged path uses this to undo the tree mutation when layout
// settings change at runtime.
struct OldLayoutPinnedScrollEntry {
    winrt::weak_ref<Controls::Border> border;
    winrt::weak_ref<Controls::Grid> sourceTopLevelHeader;
};
std::vector<OldLayoutPinnedScrollEntry> g_oldLayoutPinnedScrollEntries;

std::vector<winrt::Windows::Foundation::IAsyncOperation<bool>>
    g_oldLayoutPinnedScrollPendingActions;

bool IsOldLayoutPinnedScrollTargetName(winrt::hstring const& name) {
    return name == L"PinnedListHeaderGrid" || name == L"ShowMorePinnedGrid" ||
           name == L"StartMenuPinnedList" || name == L"TopLevelSuggestionsRoot";
}

void ConfigureOldLayoutPinnedWrapperColumns(Controls::Grid wrapper) {
    auto cols = wrapper.ColumnDefinitions();
    while (cols.Size() < 2) {
        cols.Append(Controls::ColumnDefinition{});
    }

    // This mod hides the All Apps list completely, so the pinned apps panel
    // should use the full Start menu width. Keeping the original Styler column
    // split here is what limited the pinned grid to 5 columns.
    cols.GetAt(0).Width(GridLength{1, GridUnitType::Star});
    cols.GetAt(1).Width(GridLength{0, GridUnitType::Pixel});
}

// Walk the visual tree below `root` and return the first FrameworkElement
// whose Name matches `name` and that casts to Grid. Used to (re-)locate the
// original Grid#TopLevelHeader inside a restructured wrapper when we don't
// have a live weak_ref for it (e.g. after re-init).
Controls::Grid FindFirstGridDescendantNamed(DependencyObject root,
                                            PCWSTR name) {
    if (!root) {
        return nullptr;
    }
    std::vector<DependencyObject> stack;
    stack.push_back(root);
    while (!stack.empty()) {
        auto current = stack.back();
        stack.pop_back();
        if (auto fe = current.try_as<FrameworkElement>()) {
            if (fe.Name() == name) {
                if (auto grid = fe.try_as<Controls::Grid>()) {
                    return grid;
                }
            }
        }
        int n = Media::VisualTreeHelper::GetChildrenCount(current);
        for (int i = 0; i < n; i++) {
            stack.push_back(Media::VisualTreeHelper::GetChild(current, i));
        }
    }
    return nullptr;
}

// Copy the column/row definitions from the original Grid#TopLevelHeader to the
// new content Grid so that each moved child's Grid.Row/Grid.Column/
// Grid.ColumnSpan attached properties still resolve to the same logical layout
// slot. Width is intentionally not set: the new ScrollViewer has
// HorizontalScrollMode=Disabled, so its ScrollContentPresenter measures content
// against its own viewport - meaning the content Grid stretches to the
// ScrollViewer's actual width (which in turn is whatever yaml chose, e.g. via
// Grid.Column placement in a star-sized wrapper). Forcing a width derived from
// the original TopLevelHeader would tie the pinned content to the apps column's
// width when both live inside the new column-split wrapper, breaking the
// layout.
void ConfigurePinnedContentFromSource(Controls::Grid target,
                                      Controls::Grid source) {
    target.ColumnDefinitions().Clear();
    for (auto const& def : source.ColumnDefinitions()) {
        Controls::ColumnDefinition newDef;
        newDef.Width(def.Width());
        newDef.MinWidth(def.MinWidth());
        newDef.MaxWidth(def.MaxWidth());
        target.ColumnDefinitions().Append(newDef);
    }

    target.RowDefinitions().Clear();
    for (auto const& def : source.RowDefinitions()) {
        Controls::RowDefinition newDef;
        newDef.Height(def.Height());
        newDef.MinHeight(def.MinHeight());
        newDef.MaxHeight(def.MaxHeight());
        target.RowDefinitions().Append(newDef);
    }
}

// Undo `EnsureOldLayoutPinnedScrollViewer` for a single Border:
// 1. Move each child back from OldLayoutPinnedContent into TopLevelHeader.
// 2. Detach the original ScrollViewer from the wrapper and reset Border.Child
//    to it, releasing the wrapper for GC.
//
// Best-effort: silently skips parts that have already been disposed.
void RevertOldLayoutPinnedScrollForBorder(Controls::Border border,
                                          Controls::Grid sourceTopLevelHeader) {
    if (!border) {
        return;
    }

    auto wrapper = border.Child().try_as<Controls::Grid>();
    if (!wrapper || wrapper.Name() != kOldLayoutPinnedWrapperName) {
        return;
    }

    UIElement originalScrollViewer{nullptr};
    Controls::Grid contentGrid{nullptr};
    for (uint32_t i = 0; i < wrapper.Children().Size(); i++) {
        auto child = wrapper.Children().GetAt(i);
        auto sv = child.try_as<Controls::ScrollViewer>();
        if (sv && sv.Name() == kOldLayoutPinnedScrollName) {
            contentGrid = sv.Content().try_as<Controls::Grid>();
        } else if (!originalScrollViewer) {
            originalScrollViewer = child.try_as<UIElement>();
        }
    }

    // If we lost the source ref (e.g. after a re-init), try to (re)find it
    // by walking the original ScrollViewer's subtree.
    if (!sourceTopLevelHeader && originalScrollViewer) {
        sourceTopLevelHeader = FindFirstGridDescendantNamed(
            originalScrollViewer.as<DependencyObject>(), L"TopLevelHeader");
    }

    if (sourceTopLevelHeader && contentGrid) {
        while (contentGrid.Children().Size() > 0) {
            auto child = contentGrid.Children().GetAt(0).try_as<UIElement>();
            contentGrid.Children().RemoveAt(0);
            if (child) {
                sourceTopLevelHeader.Children().Append(child);
            }
        }
    }

    if (originalScrollViewer) {
        uint32_t idx = 0;
        if (wrapper.Children().IndexOf(originalScrollViewer, idx)) {
            wrapper.Children().RemoveAt(idx);
        }
        border.Child(nullptr);
        border.Child(originalScrollViewer);

        // Note: the HorizontalScrollMode / HorizontalScrollBarVisibility that
        // EnsureOldLayoutPinnedScrollViewer forced to Disabled on the original
        // apps ScrollViewer are intentionally not restored here. The apps list
        // never scrolls horizontally, so the values match the effective default
        // anyway, and we don't store the pre-override values to restore them.
        // A toggle-off only happens when the override matches the Windows
        // default (no process restart); otherwise the process restarts and the
        // ScrollViewer is recreated fresh.
    }
}

// Walk every tracked entry and revert the tree mutation. Called when the
// user toggles the setting off at runtime and on Wh_ModUninit.
void RevertAllOldLayoutPinnedScroll() {
    for (auto const& entry : g_oldLayoutPinnedScrollEntries) {
        try {
            RevertOldLayoutPinnedScrollForBorder(
                entry.border.get(), entry.sourceTopLevelHeader.get());
        } catch (winrt::hresult_error const& ex) {
            Wh_Log(L"Error %08X: %s", ex.code(), ex.message().c_str());
        }
    }
    g_oldLayoutPinnedScrollEntries.clear();
}

// Wrap the inner ScrollViewer in a sibling-capable Grid the first time we see
// the AllAppsGrid's Border, and create the new ScrollViewer + content Grid
// for the moved elements. Idempotent: returns the existing content Grid if
// the Border has already been restructured. `*outCreated` is set to true on
// the call that performs the wrapping (so the caller can do one-time
// configuration like copying layout definitions from the source Grid).
Controls::Grid EnsureOldLayoutPinnedScrollViewer(Controls::Border border,
                                                 bool* outCreated) {
    if (outCreated) {
        *outCreated = false;
    }

    auto currentChild = border.Child();
    if (!currentChild) {
        return nullptr;
    }

    if (auto wrapper = currentChild.try_as<Controls::Grid>()) {
        if (wrapper.Name() == kOldLayoutPinnedWrapperName) {
            ConfigureOldLayoutPinnedWrapperColumns(wrapper);
            for (uint32_t i = 0; i < wrapper.Children().Size(); i++) {
                auto sv = wrapper.Children()
                              .GetAt(i)
                              .try_as<Controls::ScrollViewer>();
                if (sv && sv.Name() == kOldLayoutPinnedScrollName) {
                    return sv.Content().try_as<Controls::Grid>();
                }
            }
            return nullptr;
        }
    }

    auto originalScrollViewer = currentChild.try_as<FrameworkElement>();
    if (!originalScrollViewer) {
        return nullptr;
    }

    // Apply the same horizontal-no-scroll configuration to the original apps
    // ScrollViewer that we use for the new pinned ScrollViewer below. Without
    // this, once the apps ScrollViewer is placed inside a star-sized column
    // of the wrapper Grid, its ScrollContentPresenter measures content with
    // infinity width and the inner ItemsPresenter / ItemsWrapGrid grows
    // unbounded instead of fitting the column.
    if (auto originalSv =
            originalScrollViewer.try_as<Controls::ScrollViewer>()) {
        originalSv.HorizontalScrollMode(Controls::ScrollMode::Disabled);
        originalSv.HorizontalScrollBarVisibility(
            Controls::ScrollBarVisibility::Disabled);
    }

    Controls::ScrollViewer newScroll;
    newScroll.Name(kOldLayoutPinnedScrollName);
    newScroll.VerticalScrollMode(Controls::ScrollMode::Enabled);
    newScroll.VerticalScrollBarVisibility(Controls::ScrollBarVisibility::Auto);
    // Use ScrollBarVisibility::Disabled (not Hidden) for the horizontal axis
    // so the ScrollContentPresenter's CanHorizontallyScroll evaluates to
    // false and the content is measured against the ScrollViewer's viewport
    // width instead of with infinity. Without this, a content Grid with
    // star-sized column definitions cannot resolve its column widths and
    // either collapses or grows unbounded.
    newScroll.HorizontalScrollMode(Controls::ScrollMode::Disabled);
    newScroll.HorizontalScrollBarVisibility(
        Controls::ScrollBarVisibility::Disabled);
    newScroll.Background(nullptr);

    Controls::Grid contentGrid;
    contentGrid.Name(kOldLayoutPinnedContentName);
    newScroll.Content(contentGrid);

    Controls::Grid wrapper;
    wrapper.Name(kOldLayoutPinnedWrapperName);

    auto cols = wrapper.ColumnDefinitions();
    Controls::ColumnDefinition pinnedCol;
    cols.Append(pinnedCol);
    Controls::ColumnDefinition appsCol;
    cols.Append(appsCol);
    ConfigureOldLayoutPinnedWrapperColumns(wrapper);

    Controls::Grid::SetColumn(newScroll, 0);
    Controls::Grid::SetColumn(originalScrollViewer, 1);

    border.Child(nullptr);
    wrapper.Children().Append(originalScrollViewer);
    wrapper.Children().Append(newScroll);
    border.Child(wrapper);

    if (outCreated) {
        *outCreated = true;
    }
    return contentGrid;
}

// Reparent `element` from its current Panel parent to `target`. No-op if it
// is already there or if the current parent is not a Panel (e.g. a
// ContentControl/ContentPresenter, which shouldn't happen for any of the
// four target elements since they are direct children of Grid#TopLevelHeader).
void MoveElementToPinnedContent(FrameworkElement element,
                                Controls::Grid target) {
    auto currentParent = Media::VisualTreeHelper::GetParent(element);
    if (!currentParent) {
        return;
    }

    if (currentParent == target.as<DependencyObject>()) {
        return;
    }

    auto parentPanel = currentParent.try_as<Controls::Panel>();
    if (!parentPanel) {
        return;
    }

    auto uiElement = element.try_as<UIElement>();
    if (!uiElement) {
        return;
    }

    uint32_t index = 0;
    if (!parentPanel.Children().IndexOf(uiElement, index)) {
        return;
    }

    parentPanel.Children().RemoveAt(index);
    target.Children().Append(uiElement);
}

void HandleOldLayoutPinnedScroll(FrameworkElement element) {
    if (!IsOldLayoutPinnedScrollTargetName(element.Name())) {
        return;
    }

    // Defer to the next dispatcher tick so we don't mutate the visual tree
    // while the tree watcher is still delivering the Add event for `element`.
    auto action = element.Dispatcher().TryRunAsync(
        winrt::Windows::UI::Core::CoreDispatcherPriority::Normal,
        [weakElement = winrt::make_weak(element)]() {
            auto element = weakElement.get();
            if (!element) {
                return;
            }

            try {
                FrameworkElement allAppsGrid{nullptr};
                Controls::Grid topLevelHeader{nullptr};
                DependencyObject iter = element;
                while (auto parent = Media::VisualTreeHelper::GetParent(iter)) {
                    iter = parent;
                    auto fe = iter.try_as<FrameworkElement>();
                    if (!fe) {
                        continue;
                    }
                    if (!topLevelHeader && fe.Name() == L"TopLevelHeader") {
                        topLevelHeader = fe.try_as<Controls::Grid>();
                    }
                    if (fe.Name() == L"AllAppsGrid" &&
                        winrt::get_class_name(fe) ==
                            L"Windows.UI.Xaml.Controls.GridView") {
                        allAppsGrid = fe;
                        break;
                    }
                }
                if (!allAppsGrid) {
                    return;
                }

                if (Media::VisualTreeHelper::GetChildrenCount(allAppsGrid) ==
                    0) {
                    return;
                }
                auto firstChild =
                    Media::VisualTreeHelper::GetChild(allAppsGrid, 0);
                auto border = firstChild.try_as<Controls::Border>();
                if (!border) {
                    return;
                }

                bool created = false;
                auto contentGrid =
                    EnsureOldLayoutPinnedScrollViewer(border, &created);
                if (!contentGrid) {
                    return;
                }

                if (created && topLevelHeader) {
                    ConfigurePinnedContentFromSource(contentGrid,
                                                     topLevelHeader);
                    // Prune entries whose Border has been destroyed (e.g.
                    // Start menu instances opened in previous sessions of
                    // the host process) before pushing the new one.
                    std::erase_if(g_oldLayoutPinnedScrollEntries,
                                  [](OldLayoutPinnedScrollEntry const& e) {
                                      return !e.border.get();
                                  });
                    g_oldLayoutPinnedScrollEntries.push_back(
                        {winrt::make_weak(border),
                         winrt::make_weak(topLevelHeader)});
                }

                MoveElementToPinnedContent(element, contentGrid);
            } catch (winrt::hresult_error const& ex) {
                Wh_Log(L"Error %08X: %s", ex.code(), ex.message().c_str());
            }
        });

    // Track the pending callback so UninitializeSettingsAndTap can cancel it on
    // teardown. Prune already-finished actions first to keep the list bounded
    // (at most a handful are ever in flight - one per target element per Start
    // menu instance).
    std::erase_if(
        g_oldLayoutPinnedScrollPendingActions,
        [](winrt::Windows::Foundation::IAsyncOperation<bool> const& a) {
            return !a || a.Status() !=
                             winrt::Windows::Foundation::AsyncStatus::Started;
        });
    if (action) {
        g_oldLayoutPinnedScrollPendingActions.push_back(std::move(action));
    }
}

void RenamePinnedHeader(FrameworkElement const& element) {
    if (element.Name() != L"PinnedListHeaderText") {
        return;
    }

    if (auto textBlock = element.try_as<Controls::TextBlock>()) {
        textBlock.Text(g_pinnedHeaderText);
    }
}

void ApplyCustomizations(InstanceHandle handle,
                         FrameworkElement element,
                         PCWSTR fallbackClassName) {
    ForceSmallStartLayout(element);

    // Merge resource dictionary on first element add. Merging it earlier on
    // window creation doesn't work, perhaps merged dictionaries are reset
    // during initialization.

    RenamePinnedHeader(element);

    HandleOldLayoutPinnedScroll(element);

    auto resolved = FindElementPropertyOverrides(element, fallbackClassName);
    if (resolved.propertyOverrides.empty()) {
        return;
    }

    auto& elementCustomizationState = g_elementsCustomizationState[handle];

    RestorePropertyCustomizations(element, elementCustomizationState);

    elementCustomizationState.element = element;
    elementCustomizationState.propertyCustomizationStates.clear();

    ApplyPropertyCustomizations(element, std::move(resolved.propertyOverrides),
                                &elementCustomizationState);
}

void CleanupCustomizations(InstanceHandle handle) {
    if (auto it = g_elementsCustomizationState.find(handle);
        it != g_elementsCustomizationState.end()) {
        auto& elementCustomizationState = it->second;

        auto element = elementCustomizationState.element.get();

        RestorePropertyCustomizations(element, elementCustomizationState);

        g_elementsCustomizationState.erase(it);
    }

}

ElementMatcher ElementMatcherFromString(std::wstring_view str) {
    ElementMatcher result;

    auto trimmed = TrimStringView(str);
    if (trimmed == L"*" || trimmed == L":root") {
        throw std::runtime_error("Bad target syntax, unsupported selector");
    }

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
                throw std::runtime_error(
                    "Bad target syntax, visual state groups aren't supported");

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

                throw std::runtime_error(
                    "Bad target syntax, only numeric indexes are supported");
            }

            default:
                throw std::runtime_error("Bad target syntax");
        }

        i = iNext;
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
            {L"StartMenu:", L"StartMenu."},
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
                                  std::vector<StyleRule> styles) {
    ElementCustomizationRules elementCustomizationRules;

    auto targetParts = SplitStringView(target, L" > ");

    bool first = true;
    for (auto i = targetParts.rbegin(); i != targetParts.rend(); ++i) {
        const auto& targetPart = *i;

        auto matcher = ElementMatcherFromString(targetPart);
        matcher.type = AdjustTypeName(matcher.type);

        if (first) {
            elementCustomizationRules.elementMatcher = std::move(matcher);
            elementCustomizationRules.styles = std::move(styles);
        } else {
            elementCustomizationRules.parentElementMatchers.push_back(
                std::move(matcher));
        }

        first = false;
    }

    g_elementsCustomizationRules.push_back(
        std::move(elementCustomizationRules));
}

void ProcessAllStylesFromSettings() {
    for (const auto& themeTargetStyle : g_themeWithoutAllApps.targetStyles) {
        try {
            AddElementCustomizationRules(themeTargetStyle.target,
                                         themeTargetStyle.styles);
        } catch (winrt::hresult_error const& ex) {
            Wh_Log(L"Error %08X", ex.code());
        } catch (std::exception const& ex) {
            Wh_Log(L"Error: %S", ex.what());
        }
    }
}
void UninitializeSettingsAndTap() {
    for (auto const& action : g_oldLayoutPinnedScrollPendingActions) {
        if (action && action.Status() ==
                          winrt::Windows::Foundation::AsyncStatus::Started) {
            action.Cancel();
        }
    }
    g_oldLayoutPinnedScrollPendingActions.clear();

    for (const auto& [handle, elementCustomizationState] :
         g_elementsCustomizationState) {
        auto element = elementCustomizationState.element.get();

        RestorePropertyCustomizations(element, elementCustomizationState);
    }

    g_elementsCustomizationState.clear();

    g_elementsCustomizationRules.clear();



    g_targetThreadId = 0;
    g_startBlendedFlexFrame = nullptr;
}

void InitializeSettingsAndTap() {
    DWORD kNoThreadId = 0;
    if (!g_targetThreadId.compare_exchange_strong(kNoThreadId,
                                                  GetCurrentThreadId())) {
        return;
    }

    ProcessAllStylesFromSettings();

    HRESULT hr = InjectWindhawkTAP();
    if (FAILED(hr)) {
        Wh_Log(L"Error %08X", hr);
    }
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

void OnWindowCreated(HWND hWnd, LPCWSTR lpClassName) {
    BOOL bTextualClassName = ((ULONG_PTR)lpClassName & ~(ULONG_PTR)0xffff) != 0;

    if (bTextualClassName &&
        _wcsicmp(lpClassName, L"Windows.UI.Core.CoreWindow") == 0) {
        InitializeSettingsAndTap();
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

    OnWindowCreated(hWnd, lpClassName);

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

    OnWindowCreated(hWnd, lpClassName);

    return hWnd;
}

HWND GetCoreWnd() {
    struct ENUM_WINDOWS_PARAM {
        HWND* hWnd;
    };

    HWND hWnd = nullptr;
    ENUM_WINDOWS_PARAM param = {&hWnd};
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

            if (_wcsicmp(szClassName, L"Windows.UI.Core.CoreWindow") == 0) {
                *param.hWnd = hWnd;
                return FALSE;
            }

            return TRUE;
        },
        (LPARAM)&param);

    return hWnd;
}

enum FEATURE_ENABLED_STATE {
    FEATURE_ENABLED_STATE_DEFAULT = 0,
    FEATURE_ENABLED_STATE_DISABLED = 1,
    FEATURE_ENABLED_STATE_ENABLED = 2,
};

#pragma pack(push, 1)
struct RTL_FEATURE_CONFIGURATION {
    unsigned int featureId;
    unsigned __int32 group : 4;
    FEATURE_ENABLED_STATE enabledState : 2;
    unsigned __int32 enabledStateOptions : 1;
    unsigned __int32 unused1 : 1;
    unsigned __int32 variant : 6;
    unsigned __int32 variantPayloadKind : 2;
    unsigned __int32 unused2 : 16;
    unsigned int payload;
};
#pragma pack(pop)

using RtlQueryFeatureConfiguration_t = int(NTAPI*)(UINT32,
                                                   int,
                                                   INT64*,
                                                   RTL_FEATURE_CONFIGURATION*);
RtlQueryFeatureConfiguration_t RtlQueryFeatureConfiguration_Original;
int NTAPI RtlQueryFeatureConfiguration_Hook(UINT32 featureId,
                                            int group,
                                            INT64* variant,
                                            RTL_FEATURE_CONFIGURATION* config) {
    int ret = RtlQueryFeatureConfiguration_Original(featureId, group, variant,
                                                    config);

    switch (featureId) {
        // Disable the Start Menu Phone Link layout feature.
        // https://winaero.com/enable-phone-link-flyout-start-menu/
        case 48697323:  // Removed in StartDocked.dll 10.0.26100.8328
            config->enabledState = FEATURE_ENABLED_STATE_ENABLED;
            break;

        // Disable the revamped Start menu experience.
        // https://x.com/phantomofearth/status/1907877141540118888
        case 47205210:
        // case 49221331:
        case 49402389:
            config->enabledState = FEATURE_ENABLED_STATE_ENABLED;
            break;
    }

    return ret;
}

std::optional<bool> IsOsFeatureEnabled(UINT32 featureId) {
    static RtlQueryFeatureConfiguration_t pRtlQueryFeatureConfiguration = []() {
        HMODULE hNtDll = LoadLibraryW(L"ntdll.dll");
        return hNtDll ? (RtlQueryFeatureConfiguration_t)GetProcAddress(
                            hNtDll, "RtlQueryFeatureConfiguration")
                      : nullptr;
    }();

    if (!pRtlQueryFeatureConfiguration) {
        Wh_Log(L"RtlQueryFeatureConfiguration not found");
        return std::nullopt;
    }

    RTL_FEATURE_CONFIGURATION feature = {0};
    INT64 changeStamp = 0;
    HRESULT hr =
        pRtlQueryFeatureConfiguration(featureId, 1, &changeStamp, &feature);
    if (SUCCEEDED(hr)) {
        switch (feature.enabledState) {
            case FEATURE_ENABLED_STATE_DISABLED:
                return false;
            case FEATURE_ENABLED_STATE_ENABLED:
                return true;
            case FEATURE_ENABLED_STATE_DEFAULT:
                return std::nullopt;
        }
    } else {
        Wh_Log(L"RtlQueryFeatureConfiguration error for %u: %08X", featureId,
               hr);
    }

    return std::nullopt;
}

BOOL Wh_ModInit() {
    LoadSettings();
    HookVisualStateManagerGoToState();

    WCHAR moduleFilePath[MAX_PATH];
    switch (
        GetModuleFileName(nullptr, moduleFilePath, ARRAYSIZE(moduleFilePath))) {
        case 0:
        case ARRAYSIZE(moduleFilePath):
            Wh_Log(L"GetModuleFileName failed");
            return FALSE;

        default:
            if (!wcsrchr(moduleFilePath, L'\\')) {
                Wh_Log(L"GetModuleFileName returned an unsupported path");
                return FALSE;
            }
            break;
    }

    g_windowsDefaultIsNewLayout = IsOsFeatureEnabled(47205210).value_or(true) &&
                                  IsOsFeatureEnabled(49221331).value_or(true) &&
                                  IsOsFeatureEnabled(49402389).value_or(true);

    if (!DoesLayoutOverrideMatchWindowsDefault()) {
#ifdef _WIN64
        const size_t OFFSET_SAME_TEB_FLAGS = 0x17EE;
#else
        const size_t OFFSET_SAME_TEB_FLAGS = 0x0FCA;
#endif
        bool isInitialThread =
            *(USHORT*)((BYTE*)NtCurrentTeb() + OFFSET_SAME_TEB_FLAGS) & 0x0400;
        if (!isInitialThread) {
            // Throttle to avoid exiting in a loop if something goes wrong.
            WCHAR lastExitTickCountKey[64];
            wcscpy_s(lastExitTickCountKey, L"lastExitTickCount");
            DWORD lastTickCount =
                (DWORD)Wh_GetIntValue(lastExitTickCountKey, 0);
            DWORD currentTickCount = GetTickCount();
            if (currentTickCount - lastTickCount > 10 * 1000) {
                Wh_SetIntValue(lastExitTickCountKey, currentTickCount);
                // Exit to have the new setting take effect. The process will be
                // relaunched automatically.
                ExitProcess(0);
            }
        }
    }

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

    if (!DoesLayoutOverrideMatchWindowsDefault()) {
        HMODULE hNtDll = LoadLibraryW(L"ntdll.dll");
        RtlQueryFeatureConfiguration_t pRtlQueryFeatureConfiguration =
            (RtlQueryFeatureConfiguration_t)GetProcAddress(
                hNtDll, "RtlQueryFeatureConfiguration");
        if (pRtlQueryFeatureConfiguration) {
            Wh_SetFunctionHook((void*)pRtlQueryFeatureConfiguration,
                               (void*)RtlQueryFeatureConfiguration_Hook,
                               (void**)&RtlQueryFeatureConfiguration_Original);
        } else {
            Wh_Log(L"Failed to hook RtlQueryFeatureConfiguration");
        }
    }


    return TRUE;
}

void Wh_ModAfterInit() {
    HWND hCoreWnd = GetCoreWnd();
    if (hCoreWnd) {
        RunFromWindowThread(
            hCoreWnd, [](PVOID) { InitializeSettingsAndTap(); }, nullptr);
    }
}

void Wh_ModUninit() {
    if (!DoesLayoutOverrideMatchWindowsDefault()) {
        // Exit to have the new setting take effect. The process will be
        // relaunched automatically.
        ExitProcess(0);
    }


    if (g_visualTreeWatcher) {
        g_visualTreeWatcher->UnadviseVisualTreeChange();
        g_visualTreeWatcher = nullptr;
    }

    HWND hCoreWnd = GetCoreWnd();
    if (hCoreWnd) {
        RunFromWindowThread(
            hCoreWnd,
            [](PVOID) {
                RevertAllOldLayoutPinnedScroll();
                UninitializeSettingsAndTap();
            },
            nullptr);
    }

}

void Wh_ModSettingsChanged() {
    bool oldEnableLargeStartMenu = g_enableLargeStartMenu;
    LoadSettings();

    bool needsRevert = oldEnableLargeStartMenu != g_enableLargeStartMenu;

    if (g_visualTreeWatcher) {
        g_visualTreeWatcher->UnadviseVisualTreeChange();
        g_visualTreeWatcher = nullptr;
    }

    HWND hCoreWnd = GetCoreWnd();
    if (hCoreWnd) {
        RunFromWindowThread(
            hCoreWnd,
            [](PVOID p) {
                bool needsRevert = *(bool*)p;
                if (needsRevert) {
                    RevertAllOldLayoutPinnedScroll();
                }
                UninitializeSettingsAndTap();
                InitializeSettingsAndTap();
                ForceSmallStartLayout();
            },
            &needsRevert);
    }
}
