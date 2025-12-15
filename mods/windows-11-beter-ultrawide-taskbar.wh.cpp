// ==WindhawkMod==
// @id              windows-11-beter-ultrawide-taskbar
// @name            Windows 11 Better Ultrawide Taskbar
// @description     Centers the taskbar and system tray as a single unit
// @version         1.0
// @author          Molko
// @github          https://github.com/roeseth
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lcomctl32 -lole32 -loleaut32 -lruntimeobject -Wl,--export-all-symbols
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Windows 11 Better Ultrawide Taskbar

Centers the taskbar buttons and system tray as a single unit on the Windows 11
taskbar. The mod dynamically adjusts positioning when windows are opened or
closed.

## Modes

### Center Mode (default)
Centers both the taskbar buttons and system tray together as a unified group.
The centering is calculated based on the combined width of both elements.

**Settings:**
- **Minimum total width**: Set a minimum width for the centered group
- **Gap**: Add spacing between taskbar buttons and system tray
- **Maximum taskbar width**: Limit the taskbar buttons area width

### Offset Only Mode
Only shifts the system tray by a fixed offset, reducing the taskbar width to
allow the system tray to overlap into the taskbar area. The taskbar buttons
remain in their original position.

**Settings:**
- **Offset value**: How many pixels to shift the system tray left

## Common Settings

- **Animation duration**: Smooth transition animation when repositioning
  (set to 0 to disable)

## Multi-Monitor Support

The mod applies centering/offset to the primary monitor's taskbar only.
Secondary monitor taskbars are not affected.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- offsetOnlyMode: false
  $name: Offset only mode
  $description: >-
    OFF = Center mode: Centers both taskbar and system tray as a single unit.
    ON = Offset only mode: Only applies offset to system tray, reduces taskbar
    width to create overlap.
- OffsetOnlySettings:
    - offsetValue: 200
      $name: Offset value
      $description: >-
        The offset (in pixels) to shift the system tray left. The taskbar width
        will be reduced by 2x this value to allow overlap.
- CenterSettings:
    - minTotalWidth: 0
      $name: Minimum total width
      $description: >-
        The minimum combined width (in pixels) for the taskbar buttons and system
        tray. If the actual width is less than this value, the centering offset
        will be calculated as if the width were this minimum value. Set to 0 to
        disable.
    - gap: 0
      $name: Gap
      $description: >-
        Additional spacing (in pixels) between the taskbar buttons and system tray.
        Positive values push them apart, negative values pull them closer together.
    - maxTotalWidth: 0
      $name: Maximum taskbar width
      $description: >-
        The maximum width (in pixels) for the taskbar buttons area. This sets the
        Width property on TaskbarFrame directly, leveraging native XAML layout.
        Set to 0 to disable (no maximum).
- animationDuration: 200
  $name: Animation duration (ms)
  $description: >-
    Duration of the transition animation in milliseconds when the taskbar
    repositions. Set to 0 to disable animation (instant repositioning).
*/
// ==/WindhawkModSettings==

#include <xamlom.h>

#include <atomic>
#include <limits>
#include <unordered_map>
#include <vector>

#undef GetCurrentTime

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.Media.Animation.h>

std::atomic<bool> g_initialized;
thread_local bool g_initializedForThread;

// Settings
struct {
    bool offsetOnlyMode;   // false = Center mode, true = Offset only mode
    int offsetValue;       // Offset value for offset only mode
    int minTotalWidth;
    int gap;
    int maxTotalWidth;
    int animationDuration;
} g_settings;

// Element tracking info
struct ElementInfo {
    winrt::weak_ref<winrt::Windows::UI::Xaml::FrameworkElement> element;
    winrt::event_token sizeChangedToken;
    bool isTaskbarFrame;  // true = TaskbarFrame, false = SystemTrayFrame
    winrt::Windows::UI::Xaml::HorizontalAlignment originalAlignment;  // Store original alignment for restoration
    double originalWidth;  // Store original width for TaskbarFrame (NaN means auto)
};

// Store references to elements we've customized
thread_local std::unordered_map<InstanceHandle, ElementInfo> g_customizedElements;

// Track taskbar child elements (all direct children of TaskbarFrameRepeater)
struct TaskbarChildInfo {
    winrt::weak_ref<winrt::Windows::UI::Xaml::FrameworkElement> element;
    winrt::event_token sizeChangedToken;
    double lastWidth;
};
thread_local std::unordered_map<InstanceHandle, TaskbarChildInfo> g_taskbarChildren;

// Track previous count for change detection
thread_local size_t g_lastTaskbarChildCount = 0;

// References to main frames for centering
thread_local winrt::weak_ref<winrt::Windows::UI::Xaml::FrameworkElement> g_taskbarFrame;
thread_local winrt::weak_ref<winrt::Windows::UI::Xaml::FrameworkElement> g_systemTrayFrame;

// Track original taskbar frame width for shift mode (before any modifications)
thread_local double g_originalTaskbarFrameWidth = 0;

// Calculate total width of all taskbar children
// Only counts elements with CanBeScrollAnchor=true (actual visible buttons)
double CalculateTotalTaskbarWidth() {
    double totalWidth = 0;
    for (auto& [handle, info] : g_taskbarChildren) {
        if (auto elem = info.element.get()) {
            // Only count elements that can be scroll anchors (actual visible buttons)
            // Elements with CanBeScrollAnchor=false are invisible placeholders
            try {
                if (elem.CanBeScrollAnchor()) {
                    totalWidth += elem.ActualWidth();
                }
            } catch (...) {
                // Fallback: count all elements if property access fails
                totalWidth += elem.ActualWidth();
            }
        }
    }
    return totalWidth;
}

// Forward declarations
void ApplyTransformOffset(winrt::Windows::UI::Xaml::FrameworkElement element, double offsetX);
void ActionOnTaskbarChanges();

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
namespace wuxm = winrt::Windows::UI::Xaml::Media;
namespace wuxma = winrt::Windows::UI::Xaml::Media::Animation;

#pragma endregion  // winrt_hpp

#pragma region visualtreewatcher_hpp

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
    Wh_Log(L"Constructing VisualTreeWatcher");

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
    Wh_Log(L"Destructing VisualTreeWatcher");
}

void VisualTreeWatcher::UnadviseVisualTreeChange()
{
    Wh_Log(L"UnadviseVisualTreeChange VisualTreeWatcher");
    winrt::check_hresult(m_XamlDiagnostics.as<IVisualTreeService3>()->UnadviseVisualTreeChange(this));
}

HRESULT VisualTreeWatcher::OnVisualTreeChange(ParentChildRelation, VisualElement element, VisualMutationType mutationType) try
{
    if (!g_initializedForThread) {
        return S_OK;
    }

    switch (mutationType)
    {
    case Add:
        {
            const auto inspectable = FromHandle(element.Handle);
            auto frameworkElement = inspectable.try_as<wux::FrameworkElement>();
            if (frameworkElement)
            {
                ApplyCustomizations(element.Handle, frameworkElement, element.Type);
            }
        }
        break;

    case Remove:
        CleanupCustomizations(element.Handle);
        break;
    }

    return S_OK;
}
catch (...)
{
    HRESULT hr = winrt::to_hresult();
    Wh_Log(L"Error %08X", hr);
    return S_OK;
}

HRESULT VisualTreeWatcher::OnElementStateChanged(InstanceHandle, VisualElementState, LPCWSTR) noexcept
{
    return S_OK;
}

#pragma endregion  // visualtreewatcher_cpp

#pragma region tap_hpp

#include <ocidl.h>

winrt::com_ptr<VisualTreeWatcher> g_visualTreeWatcher;

// {D6E7B2A1-9C3F-4E8A-B5D2-1A2B3C4D5E6F}
static constexpr CLSID CLSID_WindhawkTAP = { 0xd6e7b2a1, 0x9c3f, 0x4e8a, { 0xb5, 0xd2, 0x1a, 0x2b, 0x3c, 0x4d, 0x5e, 0x6f } };

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
_Use_decl_annotations_ STDAPI DllCanUnloadNow(void)
{
    if (winrt::get_module_lock())
    {
        return S_FALSE;
    }

    return S_OK;
}

#pragma clang diagnostic pop

#pragma endregion  // module_cpp

#pragma region api_cpp

HRESULT InjectWindhawkTAP() noexcept
{
    HMODULE module = GetCurrentModuleHandle();
    if (!module) {
        return HRESULT_FROM_WIN32(GetLastError());
    }

    WCHAR location[MAX_PATH];
    switch (GetModuleFileName(module, location, ARRAYSIZE(location))) {
    case 0:
    case ARRAYSIZE(location):
        return HRESULT_FROM_WIN32(GetLastError());
    }

    const HMODULE wux(LoadLibraryEx(L"Windows.UI.Xaml.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32));
    if (!wux) [[unlikely]]
    {
        return HRESULT_FROM_WIN32(GetLastError());
    }

    const auto pInitializeXamlDiagnosticsEx = reinterpret_cast<decltype(&InitializeXamlDiagnosticsEx)>(GetProcAddress(wux, "InitializeXamlDiagnosticsEx"));
    if (!pInitializeXamlDiagnosticsEx) [[unlikely]]
    {
        return HRESULT_FROM_WIN32(GetLastError());
    }

    const HRESULT hr2 = pInitializeXamlDiagnosticsEx(L"VisualDiagConnection1", GetCurrentProcessId(), nullptr, location, CLSID_WindhawkTAP, nullptr);
    if (FAILED(hr2)) [[unlikely]]
    {
        return hr2;
    }

    return S_OK;
}

#pragma endregion  // api_cpp

// clang-format on
////////////////////////////////////////////////////////////////////////////////

void ApplyTransformOffset(wux::FrameworkElement element, double offsetX) {
    try {
        // Get or create the TranslateTransform
        wuxm::TranslateTransform transform{nullptr};
        auto existingTransform = element.RenderTransform();
        if (auto tt = existingTransform.try_as<wuxm::TranslateTransform>()) {
            transform = tt;
        } else {
            transform = wuxm::TranslateTransform();
            transform.Y(0);
            element.RenderTransform(transform);
        }
        
        // Skip if already at target position
        if (transform.X() == offsetX) {
            return;
        }
        
        // If animation is disabled, set directly
        if (g_settings.animationDuration <= 0) {
            transform.X(offsetX);
            return;
        }
        
        // Create animation
        wuxma::DoubleAnimation animation;
        animation.To(offsetX);
        animation.Duration(wux::DurationHelper::FromTimeSpan(
            std::chrono::milliseconds(g_settings.animationDuration)));
        
        // Add easing function for smooth animation
        wuxma::QuadraticEase easing;
        easing.EasingMode(wuxma::EasingMode::EaseOut);
        animation.EasingFunction(easing);
        
        // Create and configure storyboard
        wuxma::Storyboard storyboard;
        storyboard.Children().Append(animation);
        wuxma::Storyboard::SetTarget(animation, transform);
        wuxma::Storyboard::SetTargetProperty(animation, L"X");
        
        // Start animation
        storyboard.Begin();
    } catch (winrt::hresult_error const& ex) {
        Wh_Log(L"Error applying transform %08X: %s", ex.code(), ex.message().c_str());
    }
}

void ActionOnTaskbarChanges() {
    size_t currentCount = g_taskbarChildren.size();
    double totalWidth = CalculateTotalTaskbarWidth();
    
    // Log count change if it happened
    if (currentCount != g_lastTaskbarChildCount) {
        Wh_Log(L"Taskbar child count changed: %zu -> %zu", g_lastTaskbarChildCount, currentCount);
        g_lastTaskbarChildCount = currentCount;
    }
    
    Wh_Log(L"Taskbar total width: %f (from %zu children)", totalWidth, currentCount);
    
    // Get frame references
    auto taskbarFrame = g_taskbarFrame.get();
    auto systemTrayFrame = g_systemTrayFrame.get();
    
    if (!taskbarFrame || !systemTrayFrame) {
        return;
    }
    
    if (g_settings.offsetOnlyMode) {
        // Offset only mode: Only apply offset to system tray, reduce taskbar width for overlap
        double offsetValue = static_cast<double>(g_settings.offsetValue);
        
        // Use original width as base (captured when TaskbarFrame was first found)
        // This prevents compounding width reductions on repeated calls
        double baseWidth = g_originalTaskbarFrameWidth;
        if (baseWidth <= 0) {
            // Fallback: capture current width as original if not set
            baseWidth = taskbarFrame.ActualWidth();
            g_originalTaskbarFrameWidth = baseWidth;
        }
        
        // Reduce taskbar width by 2*offset to allow system tray to overlap
        double newTaskbarWidth = baseWidth - (2.0 * offsetValue);
        if (newTaskbarWidth > 0) {
            taskbarFrame.Width(newTaskbarWidth);
        }
        
        // Apply offset only to system tray (shift left into taskbar area)
        ApplyTransformOffset(taskbarFrame, 0);  // No offset for taskbar
        ApplyTransformOffset(systemTrayFrame, -offsetValue);  // Shift system tray left
        
        Wh_Log(L"Offset only mode: SystemTray offset=%f, TaskbarFrame width=%f (base=%f)", 
               -offsetValue, newTaskbarWidth, baseWidth);
    } else {
        // Center mode: Original behavior - center both as a single unit
        double systemTrayWidth = systemTrayFrame.ActualWidth();
        double halfGap = g_settings.gap / 2.0;
        
        // Apply minimum total width setting
        double effectiveTotalWidth = totalWidth + systemTrayWidth;
        if (g_settings.minTotalWidth > 0 && effectiveTotalWidth < g_settings.minTotalWidth) {
            // Distribute the extra width evenly to both sides
            halfGap = (g_settings.minTotalWidth - effectiveTotalWidth) / 2.0;
        }
        
        // Calculate offsets for centering both as a single unit:
        double taskbarOffset = -systemTrayWidth / 2.0 - halfGap;
        double systemTrayOffset = totalWidth / 2.0 + halfGap;
        
        Wh_Log(L"Center mode: TaskbarFrame=%f, SystemTrayFrame=%f (gap=%d)", 
               taskbarOffset, systemTrayOffset, g_settings.gap);
        
        ApplyTransformOffset(taskbarFrame, taskbarOffset);
        ApplyTransformOffset(systemTrayFrame, systemTrayOffset);
    }
}

void ApplyCustomizations(InstanceHandle handle,
                         wux::FrameworkElement element,
                         PCWSTR fallbackClassName) {
    // Get element type and name
    auto elementName = element.Name();
    std::wstring_view className(fallbackClassName);

    // Check if this element is a direct child of TaskbarFrameRepeater
    // AND belongs to the primary monitor's taskbar (has g_taskbarFrame as ancestor)
    bool isTaskbarChild = false;
    bool isPrimaryMonitor = false;
    try {
        auto parent = wux::Media::VisualTreeHelper::GetParent(element);
        if (parent) {
            if (auto parentElement = parent.try_as<wux::FrameworkElement>()) {
                if (parentElement.Name() == L"TaskbarFrameRepeater") {
                    isTaskbarChild = true;
                    
                    // Check if this belongs to primary monitor by walking up to find g_taskbarFrame
                    auto primaryFrame = g_taskbarFrame.get();
                    if (primaryFrame) {
                        auto current = parent;
                        for (int i = 0; i < 15 && current; i++) {
                            if (current == primaryFrame) {
                                isPrimaryMonitor = true;
                                break;
                            }
                            current = wux::Media::VisualTreeHelper::GetParent(current);
                        }
                    }
                }
            }
        }
    } catch (...) {
        // Ignore errors during parent check
    }
    
    // Only track children from the primary monitor
    if (isTaskbarChild && isPrimaryMonitor) {
        double buttonWidth = element.ActualWidth();
        
        // Set up SizeChanged event handler to track width changes
        auto sizeChangedToken = element.SizeChanged([className = std::wstring(className)](wf::IInspectable const& sender, wux::SizeChangedEventArgs const& args) {
            auto newSize = args.NewSize();
            auto prevSize = args.PreviousSize();
            if (prevSize.Width != newSize.Width) {
                Wh_Log(L"%s size changed: width %f -> %f", className.c_str(), prevSize.Width, newSize.Width);
                ActionOnTaskbarChanges();
            }
        });
        
        g_taskbarChildren[handle] = TaskbarChildInfo{
            .element = element,
            .sizeChangedToken = sizeChangedToken,
            .lastWidth = buttonWidth
        };
        
        Wh_Log(L"%s added (handle=%llu, width=%f)", fallbackClassName, (unsigned long long)handle, buttonWidth);
        ActionOnTaskbarChanges();
        return;
    }

    // Check if this is the TaskbarFrame or SystemTrayFrame element
    // Target: Taskbar.TaskbarFrame#TaskbarFrame
    bool isTaskbarFrame = (className == L"Taskbar.TaskbarFrame" && elementName == L"TaskbarFrame");
    // Target: SystemTray.SystemTrayFrame (no specific name)
    bool isSystemTrayFrame = (className == L"SystemTray.SystemTrayFrame");

    if (!isTaskbarFrame && !isSystemTrayFrame) {
        return;
    }

    Wh_Log(L"Found target element: %s#%s", fallbackClassName, elementName.c_str());

    // Log initial width
    double width = element.ActualWidth();
    const wchar_t* elementType = isTaskbarFrame ? L"TaskbarFrame" : L"SystemTrayFrame";
    Wh_Log(L"%s initial width: %f", elementType, width);

    // Store frame reference for centering - ONLY store the first (primary) monitor's frames
    if (isTaskbarFrame) {
        // Only store the first TaskbarFrame we encounter
        if (!g_taskbarFrame.get()) {
            g_taskbarFrame = element;
            Wh_Log(L"Stored TaskbarFrame (primary monitor)");
            
            // Store original width (NaN means auto/unset) for restoration
            double originalWidth = element.Width();
            
            // Capture original ActualWidth for shift mode calculations
            g_originalTaskbarFrameWidth = element.ActualWidth();
            Wh_Log(L"Captured original TaskbarFrame ActualWidth: %f", g_originalTaskbarFrameWidth);
            
            // Apply max width if configured (Center mode only)
            if (!g_settings.offsetOnlyMode && g_settings.maxTotalWidth > 0) {
                element.Width(static_cast<double>(g_settings.maxTotalWidth));
                Wh_Log(L"Set TaskbarFrame Width=%d", g_settings.maxTotalWidth);
            }
            
            // TaskbarFrame doesn't fire SizeChanged when content changes
            // We track content changes via taskbar child elements instead
            // Just store reference, no event handler needed
            g_customizedElements[handle] = ElementInfo{
                .element = element,
                .sizeChangedToken = {},  // No event token
                .isTaskbarFrame = true,
                .originalAlignment = wux::HorizontalAlignment::Stretch,  // TaskbarFrame default
                .originalWidth = originalWidth
            };
        } else {
            Wh_Log(L"TaskbarFrame found (secondary monitor) - skipping storage");
        }
    } else {
        // Only store the first SystemTrayFrame we encounter
        if (!g_systemTrayFrame.get()) {
            g_systemTrayFrame = element;
            Wh_Log(L"Stored SystemTrayFrame (primary monitor)");
            
            // Save original alignment BEFORE modifying
            auto originalAlignment = element.HorizontalAlignment();
            
            // Apply HorizontalAlignment=Center to SystemTrayFrame ONLY in Center mode
            if (!g_settings.offsetOnlyMode) {
                element.HorizontalAlignment(wux::HorizontalAlignment::Center);
                Wh_Log(L"Set SystemTrayFrame HorizontalAlignment=Center (Center mode)");
            } else {
                Wh_Log(L"Keeping SystemTrayFrame original alignment (Offset only mode)");
            }
            
            // SystemTrayFrame does fire SizeChanged, so monitor it
            auto sizeChangedToken = element.SizeChanged([](wf::IInspectable const& sender, wux::SizeChangedEventArgs const& args) {
                auto newSize = args.NewSize();
                auto prevSize = args.PreviousSize();
                Wh_Log(L"SystemTrayFrame size changed: width %f -> %f", prevSize.Width, newSize.Width);
                ActionOnTaskbarChanges();
            });
            
            g_customizedElements[handle] = ElementInfo{
                .element = element,
                .sizeChangedToken = sizeChangedToken,
                .isTaskbarFrame = false,
                .originalAlignment = originalAlignment,  // Store original for restoration
                .originalWidth = std::numeric_limits<double>::quiet_NaN()  // Not used for SystemTrayFrame
            };
        } else {
            Wh_Log(L"SystemTrayFrame found (secondary monitor) - skipping storage");
            
            // Apply centering to secondary monitor ONLY in Center mode
            if (!g_settings.offsetOnlyMode) {
                element.HorizontalAlignment(wux::HorizontalAlignment::Center);
            }
        }
    }
    
    // Apply initial centering (only if we have both frames from primary monitor)
    if (g_taskbarFrame.get() && g_systemTrayFrame.get()) {
        ActionOnTaskbarChanges();
    }
}

void CleanupCustomizations(InstanceHandle handle) {
    // Check if this is a taskbar child element being removed
    auto childIt = g_taskbarChildren.find(handle);
    if (childIt != g_taskbarChildren.end()) {
        auto& info = childIt->second;
        if (auto element = info.element.get()) {
            try {
                element.SizeChanged(info.sizeChangedToken);
            } catch (...) {}
        }
        g_taskbarChildren.erase(childIt);
        Wh_Log(L"Taskbar child removed (handle=%llu)", (unsigned long long)handle);
        ActionOnTaskbarChanges();
        return;
    }

    auto it = g_customizedElements.find(handle);
    if (it != g_customizedElements.end()) {
        auto& info = it->second;
        if (auto element = info.element.get()) {
            try {
                element.SizeChanged(info.sizeChangedToken);
                // Clear RenderTransform
                element.RenderTransform(nullptr);
                // Restore SystemTrayFrame alignment to ORIGINAL value (not default)
                if (!info.isTaskbarFrame) {
                    element.HorizontalAlignment(info.originalAlignment);  // Restore to saved original
                    g_systemTrayFrame = nullptr;
                } else {
                    // Restore TaskbarFrame original width
                    element.Width(info.originalWidth);  // NaN restores to auto
                    g_taskbarFrame = nullptr;
                }
            } catch (...) {}
        }
        g_customizedElements.erase(it);
    }
}

void UninitializeForCurrentThread() {
    // Clean up taskbar children
    for (auto& [handle, info] : g_taskbarChildren) {
        if (auto element = info.element.get()) {
            try {
                element.SizeChanged(info.sizeChangedToken);
            } catch (...) {}
        }
    }
    g_taskbarChildren.clear();

    // Clean up customized elements
    for (auto& [handle, info] : g_customizedElements) {
        if (auto element = info.element.get()) {
            try {
                element.SizeChanged(info.sizeChangedToken);
                // Clear RenderTransform
                element.RenderTransform(nullptr);
                if (!info.isTaskbarFrame) {
                    // Restore SystemTrayFrame alignment to ORIGINAL value (not default)
                    element.HorizontalAlignment(info.originalAlignment);  // Restore to saved original
                } else {
                    // Restore TaskbarFrame original width
                    element.Width(info.originalWidth);  // NaN restores to auto
                }
            } catch (...) {}
        }
    }

    g_customizedElements.clear();
    g_taskbarFrame = nullptr;
    g_systemTrayFrame = nullptr;
    g_lastTaskbarChildCount = 0;
    g_originalTaskbarFrameWidth = 0;
    g_initializedForThread = false;
}

void UninitializeSettingsAndTap() {
    if (g_visualTreeWatcher) {
        g_visualTreeWatcher->UnadviseVisualTreeChange();
        g_visualTreeWatcher = nullptr;
    }

    g_initialized = false;
}

void InitializeForCurrentThread() {
    if (g_initializedForThread) {
        return;
    }

    g_initializedForThread = true;
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

std::vector<HWND> GetXamlHostWnds() {
    struct ENUM_WINDOWS_PARAM {
        std::vector<HWND>* hWnds;
    };

    std::vector<HWND> hWnds;
    ENUM_WINDOWS_PARAM param = {&hWnds};

    EnumWindows(
        [](HWND hWnd, LPARAM lParam) -> BOOL {
            auto param = reinterpret_cast<ENUM_WINDOWS_PARAM*>(lParam);

            DWORD processId = 0;
            if (!GetWindowThreadProcessId(hWnd, &processId) ||
                processId != GetCurrentProcessId()) {
                return TRUE;
            }

            WCHAR className[64];
            if (!GetClassName(hWnd, className, ARRAYSIZE(className))) {
                return TRUE;
            }

            if (_wcsicmp(className, L"XamlExplorerHostIslandWindow") == 0 ||
                _wcsicmp(className, L"Shell_InputSwitchTopLevelWindow") == 0) {
                param->hWnds->push_back(hWnd);
            }

            return TRUE;
        },
        (LPARAM)&param);

    return hWnds;
}

HWND FindCurrentProcessTaskbarWnd() {
    HWND hTaskbarWnd = nullptr;

    EnumWindows(
        [](HWND hWnd, LPARAM lParam) -> BOOL {
            DWORD processId = 0;
            if (!GetWindowThreadProcessId(hWnd, &processId) ||
                processId != GetCurrentProcessId()) {
                return TRUE;
            }

            WCHAR className[64];
            if (!GetClassName(hWnd, className, ARRAYSIZE(className))) {
                return TRUE;
            }

            if (_wcsicmp(className, L"Shell_TrayWnd") == 0) {
                *reinterpret_cast<HWND*>(lParam) = hWnd;
                return FALSE;
            }

            return TRUE;
        },
        reinterpret_cast<LPARAM>(&hTaskbarWnd));

    return hTaskbarWnd;
}

HWND GetTaskbarUiWnd() {
    HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();
    if (!hTaskbarWnd) {
        return nullptr;
    }

    return FindWindowEx(hTaskbarWnd, nullptr,
                        L"Windows.UI.Composition.DesktopWindowContentBridge",
                        nullptr);
}

void OnWindowCreated(HWND hWnd,
                     HWND hWndParent,
                     LPCWSTR lpClassName,
                     PCSTR funcName) {
    BOOL bTextualClassName = ((ULONG_PTR)lpClassName & ~(ULONG_PTR)0xffff) != 0;

    WCHAR className[64];
    if (hWndParent && GetClassName(hWnd, className, ARRAYSIZE(className)) &&
        _wcsicmp(className,
                 L"Windows.UI.Composition.DesktopWindowContentBridge") == 0 &&
        GetClassName(hWndParent, className, ARRAYSIZE(className)) &&
        _wcsicmp(className, L"Shell_TrayWnd") == 0) {
        Wh_Log(L"Initializing - Created DesktopWindowContentBridge window");
        InitializeForCurrentThread();
        InitializeSettingsAndTap();
        return;
    }

    if (bTextualClassName &&
        (_wcsicmp(lpClassName, L"XamlExplorerHostIslandWindow") == 0 ||
         _wcsicmp(lpClassName, L"Shell_InputSwitchTopLevelWindow") == 0)) {
        Wh_Log(L"Initializing - Created XAML host window: %08X via %S",
               (DWORD)(ULONG_PTR)hWnd, funcName);
        InitializeForCurrentThread();
        InitializeSettingsAndTap();
        return;
    }
}

using CreateWindowExW_t = decltype(&CreateWindowExW);
CreateWindowExW_t CreateWindowExW_Original;
HWND WINAPI CreateWindowExW_Hook(DWORD dwExStyle,
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
                                 PVOID lpParam) {
    HWND hWnd = CreateWindowExW_Original(dwExStyle, lpClassName, lpWindowName,
                                         dwStyle, X, Y, nWidth, nHeight,
                                         hWndParent, hMenu, hInstance, lpParam);
    if (hWnd) {
        OnWindowCreated(hWnd, hWndParent, lpClassName, __FUNCTION__);
    }

    return hWnd;
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
        OnWindowCreated(hWnd, hWndParent, lpClassName, __FUNCTION__);
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
        OnWindowCreated(hWnd, hWndParent, lpClassName, __FUNCTION__);
    }

    return hWnd;
}

void LoadSettings() {
    g_settings.offsetOnlyMode = Wh_GetIntSetting(L"offsetOnlyMode") != 0;
    g_settings.offsetValue = Wh_GetIntSetting(L"OffsetOnlySettings.offsetValue");
    g_settings.minTotalWidth = Wh_GetIntSetting(L"CenterSettings.minTotalWidth");
    g_settings.gap = Wh_GetIntSetting(L"CenterSettings.gap");
    g_settings.maxTotalWidth = Wh_GetIntSetting(L"CenterSettings.maxTotalWidth");
    g_settings.animationDuration = Wh_GetIntSetting(L"animationDuration");
    
    Wh_Log(L"Settings loaded: offsetOnlyMode=%d, offsetValue=%d, minTotalWidth=%d, gap=%d, maxTotalWidth=%d, animationDuration=%d", 
           g_settings.offsetOnlyMode, g_settings.offsetValue, g_settings.minTotalWidth, g_settings.gap, 
           g_settings.maxTotalWidth, g_settings.animationDuration);
}

BOOL Wh_ModInit() {
    Wh_Log(L">");
    
    LoadSettings();

    Wh_SetFunctionHook((void*)CreateWindowExW, (void*)CreateWindowExW_Hook,
                       (void**)&CreateWindowExW_Original);

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

    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L">");

    bool initialize = false;

    HWND hTaskbarUiWnd = GetTaskbarUiWnd();
    if (hTaskbarUiWnd) {
        Wh_Log(L"Initializing - Found DesktopWindowContentBridge window");
        RunFromWindowThread(
            hTaskbarUiWnd, [](PVOID) { InitializeForCurrentThread(); },
            nullptr);
        initialize = true;
    }

    for (auto hXamlHostWnd : GetXamlHostWnds()) {
        Wh_Log(L"Initializing for %08X", (DWORD)(ULONG_PTR)hXamlHostWnd);
        RunFromWindowThread(
            hXamlHostWnd, [](PVOID) { InitializeForCurrentThread(); }, nullptr);
        initialize = true;
    }

    if (initialize) {
        InitializeSettingsAndTap();
    }
}

void Wh_ModUninit() {
    Wh_Log(L">");
    
    // Reset settings to defaults
    g_settings.offsetOnlyMode = false;
    g_settings.offsetValue = 0;
    g_settings.minTotalWidth = 0;
    g_settings.gap = 0;
    g_settings.maxTotalWidth = 0;
    g_settings.animationDuration = 0;

    UninitializeSettingsAndTap();

    HWND hTaskbarUiWnd = GetTaskbarUiWnd();
    if (hTaskbarUiWnd) {
        Wh_Log(L"Uninitializing - Found DesktopWindowContentBridge window");
        RunFromWindowThread(
            hTaskbarUiWnd, [](PVOID) { UninitializeForCurrentThread(); },
            nullptr);
    }

    for (auto hXamlHostWnd : GetXamlHostWnds()) {
        Wh_Log(L"Uninitializing for %08X", (DWORD)(ULONG_PTR)hXamlHostWnd);
        RunFromWindowThread(
            hXamlHostWnd, [](PVOID) { UninitializeForCurrentThread(); },
            nullptr);
    }
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");
    
    LoadSettings();

    UninitializeSettingsAndTap();

    bool initialize = false;

    HWND hTaskbarUiWnd = GetTaskbarUiWnd();
    if (hTaskbarUiWnd) {
        Wh_Log(L"Reinitializing - Found DesktopWindowContentBridge window");
        RunFromWindowThread(
            hTaskbarUiWnd,
            [](PVOID) {
                UninitializeForCurrentThread();
                InitializeForCurrentThread();
            },
            nullptr);
        initialize = true;
    }

    for (auto hXamlHostWnd : GetXamlHostWnds()) {
        Wh_Log(L"Reinitializing for %08X", (DWORD)(ULONG_PTR)hXamlHostWnd);
        RunFromWindowThread(
            hXamlHostWnd,
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
