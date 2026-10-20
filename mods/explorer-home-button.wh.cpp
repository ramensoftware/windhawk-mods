// ==WindhawkMod==
// @id              explorer-home-button
// @name            File Explorer Address Bar Home Button
// @description     Adds a dedicated "Home" (This PC) button next to the Refresh button in the Windows 11 File Explorer address bar
// @version         1.0.0
// @author          RobotsNeverDie
// @github          https://github.com/RBND
// @homepage        https://github.com/RBND/HomeExplorer
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject -lshell32 -lshlwapi -luuid
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# File Explorer Address Bar Home Button

Adds a native WinUI 3 **Home ("This PC")** button directly adjacent to the **Refresh** button in the Windows 11 File Explorer address bar.

## Features
- **Native WinUI 3 / XAML Islands Integration**: Injects an `AppBarButton` directly into File Explorer's `NavigationBarControl` (`CommandBar#NavigationCommands`) next to `refreshButton`.
- **Exact Visual & Theme Match**: Inherits the native `refreshButton`'s style, dimensions, margins, corner radius, hover/pressed/focus visual states, and dynamically adapts to Windows 11 Light, Dark, and High Contrast themes.
- **Active-Tab-Only Navigation**: Uses COM Shell APIs (`IShellWindows`, `IServiceProvider`, `IShellBrowser::BrowseObject`, and `IWebBrowser2::Navigate2`) matched against the active `ShellTabWindowClass` child window so only the currently active tab navigates to **This PC** (`shell:MyComputerFolder` / `::{20D04FE0-3AEA-1069-A2D8-08002B30309D}`).
- **Configurable Settings**: Customize the target shell location, button placement (before or after the Refresh button), icon glyph (`Home \uE80F` or `Computer \uE770`), and tooltip text (`This PC (Alt+Home)`).
- **Clean Lifecycle**: Supports live injection into open File Explorer windows, live settings updates, and clean removal on mod unload without leaking XAML elements.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- targetLocation: "shell:MyComputerFolder"
  $name: Target Location
  $description: >-
    The Shell location, CLSID, or folder path to open in the active tab when the
    Home button is clicked. Defaults to "shell:MyComputerFolder"
    (::{20D04FE0-3AEA-1069-A2D8-08002B30309D}).
- placement: after
  $name: Button Placement
  $description: >-
    Place the Home button immediately before or after the Refresh button in the
    address bar.
  $options:
    - before: Before Refresh button
    - after: After Refresh button
- iconGlyph: home
  $name: Icon Glyph
  $description: >-
    Select the Segoe Fluent Icons glyph displayed on the button.
  $options:
    - home: Home (E80F)
    - computer: Computer / This PC (E770)
- tooltip: "This PC (Alt+Home)"
  $name: Tooltip Text
  $description: >-
    The tooltip shown when hovering over the Home button.
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <exdisp.h>
#include <knownfolders.h>
#include <ocidl.h>
#include <shdeprecated.h>
#include <shlguid.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <shobjidl.h>
#include <weakreference.h>
#include <xamlom.h>

#include <atomic>
#include <cmath>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#undef GetCurrentTime

#include <winrt/Microsoft.UI.Content.h>
#include <winrt/Microsoft.UI.Dispatching.h>
#include <winrt/Microsoft.UI.Xaml.Automation.h>
#include <winrt/Microsoft.UI.Xaml.Controls.Primitives.h>
#include <winrt/Microsoft.UI.Xaml.Controls.h>
#include <winrt/Microsoft.UI.Xaml.Input.h>
#include <winrt/Microsoft.UI.Xaml.Media.h>
#include <winrt/Microsoft.UI.Xaml.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.System.h>
#include <winrt/base.h>

namespace wf = winrt::Windows::Foundation;
namespace mux = winrt::Microsoft::UI::Xaml;
namespace muxc = winrt::Microsoft::UI::Xaml::Controls;
namespace muxa = winrt::Microsoft::UI::Xaml::Automation;
namespace muxm = winrt::Microsoft::UI::Xaml::Media;

constexpr WCHAR kHomeButtonName[] = L"WindhawkHomeExplorerButton";
constexpr WCHAR kThisPcClsidPath[] = L"::{20D04FE0-3AEA-1069-A2D8-08002B30309D}";
constexpr WCHAR kDefaultTargetLocation[] = L"shell:MyComputerFolder";
constexpr WCHAR kDefaultTooltip[] = L"This PC (Alt+Home)";
constexpr WCHAR kGlyphHome[] = L"\uE80F";
constexpr WCHAR kGlyphComputer[] = L"\uE770";

enum class ButtonPlacement {
    BeforeRefresh,
    AfterRefresh,
};

struct ModSettings {
    std::wstring targetLocation = kDefaultTargetLocation;
    ButtonPlacement placement = ButtonPlacement::AfterRefresh;
    std::wstring iconGlyph = kGlyphHome;
    std::wstring tooltip = kDefaultTooltip;
};

SRWLOCK g_settingsLock = SRWLOCK_INIT;
ModSettings g_settings;

std::atomic<bool> g_tapInitialized{false};
std::atomic<bool> g_uninitializing{false};

HMODULE GetCurrentModuleHandle() {
    HMODULE module = nullptr;
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                           GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                       reinterpret_cast<LPCWSTR>(&GetCurrentModuleHandle),
                       &module);
    return module;
}

ModSettings GetSettingsSnapshot() {
    AcquireSRWLockShared(&g_settingsLock);
    ModSettings copy = g_settings;
    ReleaseSRWLockShared(&g_settingsLock);
    return copy;
}

void LoadSettings() {
    ModSettings updated;

    PCWSTR rawTarget = Wh_GetStringSetting(L"targetLocation");
    if (rawTarget && *rawTarget) {
        updated.targetLocation = rawTarget;
    } else {
        updated.targetLocation = kDefaultTargetLocation;
    }
    Wh_FreeStringSetting(rawTarget);

    PCWSTR rawPlacement = Wh_GetStringSetting(L"placement");
    if (rawPlacement && _wcsicmp(rawPlacement, L"before") == 0) {
        updated.placement = ButtonPlacement::BeforeRefresh;
    } else {
        updated.placement = ButtonPlacement::AfterRefresh;
    }
    Wh_FreeStringSetting(rawPlacement);

    PCWSTR rawGlyph = Wh_GetStringSetting(L"iconGlyph");
    if (rawGlyph && _wcsicmp(rawGlyph, L"computer") == 0) {
        updated.iconGlyph = kGlyphComputer;
    } else {
        updated.iconGlyph = kGlyphHome;
    }
    Wh_FreeStringSetting(rawGlyph);

    PCWSTR rawTooltip = Wh_GetStringSetting(L"tooltip");
    if (rawTooltip && *rawTooltip) {
        updated.tooltip = rawTooltip;
    } else {
        updated.tooltip = kDefaultTooltip;
    }
    Wh_FreeStringSetting(rawTooltip);

    AcquireSRWLockExclusive(&g_settingsLock);
    g_settings = std::move(updated);
    ReleaseSRWLockExclusive(&g_settingsLock);
}

////////////////////////////////////////////////////////////////////////////////
// Window & Active Tab COM Shell Navigation
////////////////////////////////////////////////////////////////////////////////

bool IsCabinetWindow(HWND hWnd) {
    if (!hWnd || !IsWindow(hWnd)) {
        return false;
    }
    WCHAR className[64]{};
    if (!GetClassNameW(hWnd, className, ARRAYSIZE(className))) {
        return false;
    }
    return _wcsicmp(className, L"CabinetWClass") == 0;
}

HWND GetExplorerCabinetWindowFromElement(mux::UIElement const& element) {
    try {
        if (element) {
            auto xamlRoot = element.XamlRoot();
            if (xamlRoot) {
                auto islandEnv = xamlRoot.ContentIslandEnvironment();
                if (islandEnv) {
                    HWND islandWnd = reinterpret_cast<HWND>(
                        static_cast<uintptr_t>(islandEnv.AppWindowId().Value));
                    if (islandWnd && IsWindow(islandWnd)) {
                        HWND rootWnd = GetAncestor(islandWnd, GA_ROOT);
                        if (IsCabinetWindow(rootWnd)) {
                            return rootWnd;
                        }
                        if (IsCabinetWindow(islandWnd)) {
                            return islandWnd;
                        }
                    }
                }
            }
        }
    } catch (...) {
        Wh_Log(L"Failed to resolve XamlRoot AppWindowId: %08X",
               static_cast<unsigned>(winrt::to_hresult()));
    }

    HWND activeWnd = GetActiveWindow();
    if (activeWnd) {
        HWND rootWnd = GetAncestor(activeWnd, GA_ROOT);
        if (IsCabinetWindow(rootWnd)) {
            return rootWnd;
        }
    }

    HWND foundCabinet = nullptr;
    EnumThreadWindows(
        GetCurrentThreadId(),
        [](HWND hWnd, LPARAM lParam) -> BOOL {
            if (IsCabinetWindow(hWnd)) {
                *reinterpret_cast<HWND*>(lParam) = hWnd;
                return FALSE;
            }
            return TRUE;
        },
        reinterpret_cast<LPARAM>(&foundCabinet));

    if (foundCabinet) {
        return foundCabinet;
    }

    HWND fgWnd = GetForegroundWindow();
    if (IsCabinetWindow(fgWnd)) {
        return fgWnd;
    }

    return nullptr;
}

HWND FindActiveShellTabWindow(HWND hwndCabinet) {
    if (!hwndCabinet) {
        return nullptr;
    }

    HWND firstTab = nullptr;
    HWND child = nullptr;
    while ((child = FindWindowExW(hwndCabinet, child, L"ShellTabWindowClass",
                                  nullptr)) != nullptr) {
        if (!firstTab) {
            firstTab = child;
        }
        if (IsWindowVisible(child)) {
            return child;
        }
    }
    return firstTab;
}

PIDLIST_ABSOLUTE ResolveTargetPidl(const std::wstring& targetLocation) {
    PIDLIST_ABSOLUTE pidl = nullptr;

    HRESULT hr =
        SHParseDisplayName(targetLocation.c_str(), nullptr, &pidl, 0, nullptr);
    if (SUCCEEDED(hr) && pidl) {
        return pidl;
    }

    if (_wcsicmp(targetLocation.c_str(), L"shell:MyComputerFolder") == 0 ||
        _wcsicmp(targetLocation.c_str(), kThisPcClsidPath) == 0 ||
        _wcsicmp(targetLocation.c_str(), L"This PC") == 0) {
        hr = SHParseDisplayName(kThisPcClsidPath, nullptr, &pidl, 0, nullptr);
        if (SUCCEEDED(hr) && pidl) {
            return pidl;
        }

        hr = SHGetKnownFolderIDList(FOLDERID_ComputerFolder, 0, nullptr, &pidl);
        if (SUCCEEDED(hr) && pidl) {
            return pidl;
        }
    }

    return nullptr;
}

bool NavigateBrowserToTarget(IShellBrowser* shellBrowser,
                             IWebBrowser2* webBrowser,
                             PIDLIST_ABSOLUTE pidl,
                             const std::wstring& targetLocation) {
    if (shellBrowser && pidl) {
        HRESULT hr = shellBrowser->BrowseObject(
            pidl, SBSP_DEFBROWSER | SBSP_SAMEBROWSER | SBSP_ABSOLUTE);
        if (SUCCEEDED(hr)) {
            Wh_Log(L"Navigated active tab via IShellBrowser::BrowseObject");
            return true;
        }
        Wh_Log(L"IShellBrowser::BrowseObject returned %08X, trying IWebBrowser2",
               static_cast<unsigned>(hr));
    }

    if (webBrowser) {
        VARIANT varTarget{};
        varTarget.vt = VT_BSTR;
        const std::wstring& navPath =
            (_wcsicmp(targetLocation.c_str(), L"shell:MyComputerFolder") == 0)
                ? std::wstring(kThisPcClsidPath)
                : targetLocation;
        varTarget.bstrVal = SysAllocString(navPath.c_str());
        if (varTarget.bstrVal) {
            VARIANT varFlags{};
            varFlags.vt = VT_I4;
            varFlags.lVal = 0;

            VARIANT varEmpty{};
            VariantInit(&varEmpty);

            HRESULT hr = webBrowser->Navigate2(&varTarget, &varFlags, &varEmpty,
                                               &varEmpty, &varEmpty);
            SysFreeString(varTarget.bstrVal);
            if (SUCCEEDED(hr)) {
                Wh_Log(L"Navigated active tab via IWebBrowser2::Navigate2");
                return true;
            }
            Wh_Log(L"IWebBrowser2::Navigate2 failed: %08X",
                   static_cast<unsigned>(hr));
        }
    }

    return false;
}

void NavigateActiveTabToHome(mux::UIElement const& senderElement) {
    const ModSettings settings = GetSettingsSnapshot();
    HWND hwndCabinet = GetExplorerCabinetWindowFromElement(senderElement);
    if (!hwndCabinet) {
        Wh_Log(L"Could not locate containing CabinetWClass window");
        return;
    }

    HWND hwndActiveTab = FindActiveShellTabWindow(hwndCabinet);
    Wh_Log(L"Navigating CabinetWClass=%p ActiveTab=%p to '%s'", hwndCabinet,
           hwndActiveTab, settings.targetLocation.c_str());

    PIDLIST_ABSOLUTE pidl = ResolveTargetPidl(settings.targetLocation);

    bool navigated = false;

    winrt::com_ptr<IShellWindows> shellWindows;
    HRESULT hr = CoCreateInstance(
        CLSID_ShellWindows, nullptr,
        CLSCTX_LOCAL_SERVER | CLSCTX_INPROC_SERVER,
        IID_PPV_ARGS(shellWindows.put()));

    if (SUCCEEDED(hr) && shellWindows) {
        long count = 0;
        if (SUCCEEDED(shellWindows->get_Count(&count))) {
            winrt::com_ptr<IShellBrowser> fallbackBrowser;
            winrt::com_ptr<IWebBrowser2> fallbackWebBrowser;

            for (long i = 0; i < count; i++) {
                VARIANT varIndex{};
                varIndex.vt = VT_I4;
                varIndex.lVal = i;

                winrt::com_ptr<IDispatch> disp;
                if (FAILED(shellWindows->Item(varIndex, disp.put())) || !disp) {
                    continue;
                }

                auto webBrowser = disp.try_as<IWebBrowser2>();
                auto serviceProvider = disp.try_as<::IServiceProvider>();
                if (!serviceProvider) {
                    continue;
                }

                winrt::com_ptr<IShellBrowser> shellBrowser;
                if (FAILED(serviceProvider->QueryService(
                        SID_STopLevelBrowser,
                        IID_PPV_ARGS(shellBrowser.put()))) ||
                    !shellBrowser) {
                    continue;
                }

                HWND hwndBrowser = nullptr;
                if (FAILED(shellBrowser->GetWindow(&hwndBrowser)) ||
                    !hwndBrowser) {
                    continue;
                }

                HWND rootWnd = GetAncestor(hwndBrowser, GA_ROOT);
                if (rootWnd != hwndCabinet) {
                    SHANDLE_PTR wbHwnd = 0;
                    if (!webBrowser || FAILED(webBrowser->get_HWND(&wbHwnd)) ||
                        reinterpret_cast<HWND>(wbHwnd) != hwndCabinet) {
                        continue;
                    }
                }

                // Exact match for the currently active tab in this window.
                if (hwndActiveTab &&
                    (hwndBrowser == hwndActiveTab ||
                     IsChild(hwndActiveTab, hwndBrowser))) {
                    navigated = NavigateBrowserToTarget(
                        shellBrowser.get(), webBrowser.get(), pidl,
                        settings.targetLocation);
                    break;
                }

                // Fallback candidate if the active tab is visible.
                if (IsWindowVisible(hwndBrowser) && !fallbackBrowser) {
                    fallbackBrowser = shellBrowser;
                    fallbackWebBrowser = webBrowser;
                }
            }

            if (!navigated && (fallbackBrowser || fallbackWebBrowser)) {
                navigated = NavigateBrowserToTarget(
                    fallbackBrowser.get(), fallbackWebBrowser.get(), pidl,
                    settings.targetLocation);
            }
        }
    } else {
        Wh_Log(L"CoCreateInstance(CLSID_ShellWindows) failed: %08X",
               static_cast<unsigned>(hr));
    }

    if (pidl) {
        CoTaskMemFree(pidl);
    }
}

////////////////////////////////////////////////////////////////////////////////
// WinUI 3 XAML Button Injection & Lifecycle Management
////////////////////////////////////////////////////////////////////////////////

struct InjectedButtonRecord {
    winrt::weak_ref<muxc::AppBarButton> homeButton;
    winrt::weak_ref<muxc::AppBarButton> refreshButton;
    winrt::weak_ref<muxc::CommandBar> commandBar;
    winrt::weak_ref<muxc::Panel> parentPanel;
    muxc::Button::Click_revoker clickRevoker;
    mux::FrameworkElement::ActualThemeChanged_revoker themeChangedRevoker;
};

struct PendingRefreshRecord {
    winrt::weak_ref<muxc::AppBarButton> refreshButton;
    mux::FrameworkElement::Loaded_revoker loadedRevoker;
};

// Accessed on the UI thread that owns the XAML elements.
thread_local std::unordered_map<void*, InjectedButtonRecord>
    g_injectedButtonsForThread;
thread_local std::unordered_map<void*, PendingRefreshRecord>
    g_pendingRefreshForThread;

void* GetElementKey(wf::IInspectable const& obj) {
    if (!obj) {
        return nullptr;
    }
    winrt::com_ptr<::IUnknown> unk;
    reinterpret_cast<::IUnknown*>(winrt::get_abi(obj))
        ->QueryInterface(IID_PPV_ARGS(unk.put()));
    return unk.get();
}

template <typename T>
T FindDescendantByName(mux::DependencyObject const& root, PCWSTR targetName) {
    if (!root) {
        return nullptr;
    }

    if (auto fe = root.try_as<mux::FrameworkElement>()) {
        if (fe.Name() == targetName) {
            if (auto matched = fe.try_as<T>()) {
                return matched;
            }
        }
    }

    int count = 0;
    try {
        count = muxm::VisualTreeHelper::GetChildrenCount(root);
    } catch (...) {
        return nullptr;
    }

    for (int i = 0; i < count; i++) {
        mux::DependencyObject child{nullptr};
        try {
            child = muxm::VisualTreeHelper::GetChild(root, i);
        } catch (...) {
            continue;
        }
        if (auto found = FindDescendantByName<T>(child, targetName)) {
            return found;
        }
    }

    return nullptr;
}

muxc::CommandBar FindAncestorCommandBar(mux::DependencyObject const& start) {
    mux::DependencyObject curr = start;
    while (curr) {
        if (auto cmdBar = curr.try_as<muxc::CommandBar>()) {
            return cmdBar;
        }
        try {
            curr = muxm::VisualTreeHelper::GetParent(curr);
        } catch (...) {
            break;
        }
    }
    return nullptr;
}

void ConfigureHomeButtonVisuals(muxc::AppBarButton const& homeButton,
                                muxc::AppBarButton const& refreshButton,
                                const ModSettings& settings) {
    if (!homeButton) {
        return;
    }

    if (refreshButton) {
        try {
            if (auto style = refreshButton.Style()) {
                homeButton.Style(style);
            }
            if (!std::isnan(refreshButton.Width())) {
                homeButton.Width(refreshButton.Width());
            }
            if (!std::isnan(refreshButton.Height())) {
                homeButton.Height(refreshButton.Height());
            }
            homeButton.MinWidth(refreshButton.MinWidth());
            homeButton.MinHeight(refreshButton.MinHeight());
            homeButton.MaxWidth(refreshButton.MaxWidth());
            homeButton.MaxHeight(refreshButton.MaxHeight());
            homeButton.Margin(refreshButton.Margin());
            homeButton.Padding(refreshButton.Padding());
            homeButton.CornerRadius(refreshButton.CornerRadius());
            homeButton.BorderThickness(refreshButton.BorderThickness());
            homeButton.IsCompact(refreshButton.IsCompact());
            homeButton.LabelPosition(refreshButton.LabelPosition());
            homeButton.VerticalAlignment(refreshButton.VerticalAlignment());
            homeButton.HorizontalAlignment(refreshButton.HorizontalAlignment());
            homeButton.UseSystemFocusVisuals(
                refreshButton.UseSystemFocusVisuals());
            homeButton.FocusVisualMargin(refreshButton.FocusVisualMargin());
            homeButton.FocusVisualPrimaryThickness(
                refreshButton.FocusVisualPrimaryThickness());
            homeButton.FocusVisualSecondaryThickness(
                refreshButton.FocusVisualSecondaryThickness());
            homeButton.RequestedTheme(refreshButton.RequestedTheme());
        } catch (...) {
            Wh_Log(L"Warning copying properties from refreshButton: %08X",
                   static_cast<unsigned>(winrt::to_hresult()));
        }
    } else {
        homeButton.IsCompact(true);
        homeButton.LabelPosition(muxc::CommandBarLabelPosition::Collapsed);
    }

    muxc::FontIcon fontIcon;
    fontIcon.Glyph(settings.iconGlyph);

    bool copiedIconMetrics = false;
    if (refreshButton) {
        try {
            if (auto refreshIcon =
                    refreshButton.Icon().try_as<muxc::FontIcon>()) {
                if (auto ff = refreshIcon.FontFamily()) {
                    fontIcon.FontFamily(ff);
                }
                if (refreshIcon.FontSize() > 0) {
                    fontIcon.FontSize(refreshIcon.FontSize());
                }
                fontIcon.FontWeight(refreshIcon.FontWeight());
                fontIcon.FontStyle(refreshIcon.FontStyle());
                fontIcon.Margin(refreshIcon.Margin());
                fontIcon.HorizontalAlignment(
                    refreshIcon.HorizontalAlignment());
                fontIcon.VerticalAlignment(refreshIcon.VerticalAlignment());
                copiedIconMetrics = true;
            }
        } catch (...) {
        }
    }

    if (!copiedIconMetrics) {
        fontIcon.FontFamily(
            muxm::FontFamily(L"Segoe Fluent Icons, Segoe MDL2 Assets"));
        fontIcon.FontSize(16.0);
    }

    homeButton.Icon(fontIcon);
    homeButton.Label(L"This PC");
    muxc::ToolTipService::SetToolTip(homeButton,
                                     winrt::box_value(settings.tooltip));
    muxa::AutomationProperties::SetName(homeButton, L"This PC");
}

void PositionHomeButtonInCommandBar(muxc::CommandBar const& commandBar,
                                    muxc::AppBarButton const& refreshButton,
                                    muxc::AppBarButton const& homeButton,
                                    ButtonPlacement placement) {
    auto primaryCommands = commandBar.PrimaryCommands();
    if (!primaryCommands) {
        return;
    }

    uint32_t existingHomeIdx = 0;
    if (primaryCommands.IndexOf(homeButton, existingHomeIdx)) {
        primaryCommands.RemoveAt(existingHomeIdx);
    }

    uint32_t refreshIdx = 0;
    if (!primaryCommands.IndexOf(refreshButton, refreshIdx)) {
        primaryCommands.Append(homeButton);
        return;
    }

    if (placement == ButtonPlacement::BeforeRefresh) {
        primaryCommands.InsertAt(refreshIdx, homeButton);
    } else {
        uint32_t insertIdx = refreshIdx + 1;
        // In Windows 11 NavigationBarControl, stopButton immediately follows
        // refreshButton and toggles visibility with it while loading. Placing
        // the Home button after stopButton keeps it directly adjacent to the
        // Refresh/Stop slot without shifting.
        if (insertIdx < primaryCommands.Size()) {
            if (auto nextFe =
                    primaryCommands.GetAt(insertIdx)
                        .try_as<mux::FrameworkElement>()) {
                if (nextFe.Name() == L"stopButton") {
                    insertIdx++;
                }
            }
        }
        if (insertIdx >= primaryCommands.Size()) {
            primaryCommands.Append(homeButton);
        } else {
            primaryCommands.InsertAt(insertIdx, homeButton);
        }
    }
}

void PositionHomeButtonInPanel(muxc::Panel const& panel,
                               muxc::AppBarButton const& refreshButton,
                               muxc::AppBarButton const& homeButton,
                               ButtonPlacement placement) {
    auto children = panel.Children();
    if (!children) {
        return;
    }

    uint32_t existingHomeIdx = 0;
    if (children.IndexOf(homeButton, existingHomeIdx)) {
        children.RemoveAt(existingHomeIdx);
    }

    uint32_t refreshIdx = 0;
    if (!children.IndexOf(refreshButton, refreshIdx)) {
        children.Append(homeButton);
        return;
    }

    if (placement == ButtonPlacement::BeforeRefresh) {
        children.InsertAt(refreshIdx, homeButton);
    } else {
        uint32_t insertIdx = refreshIdx + 1;
        if (insertIdx < children.Size()) {
            if (auto nextFe =
                    children.GetAt(insertIdx).try_as<mux::FrameworkElement>()) {
                if (nextFe.Name() == L"stopButton") {
                    insertIdx++;
                }
            }
        }
        if (insertIdx >= children.Size()) {
            children.Append(homeButton);
        } else {
            children.InsertAt(insertIdx, homeButton);
        }
    }
}

bool TryInjectHomeButtonForRefreshButton(
    muxc::AppBarButton const& refreshButton,
    muxc::CommandBar const& knownCommandBar = nullptr) {
    if (!refreshButton || g_uninitializing.load()) {
        return false;
    }

    void* key = GetElementKey(refreshButton);
    if (!key) {
        return false;
    }

    // Check if we already injected a live Home button for this refreshButton.
    auto existingIt = g_injectedButtonsForThread.find(key);
    if (existingIt != g_injectedButtonsForThread.end()) {
        if (auto existingBtn = existingIt->second.homeButton.get()) {
            return true;
        }
        g_injectedButtonsForThread.erase(existingIt);
    }

    muxc::CommandBar commandBar = knownCommandBar;
    if (!commandBar) {
        commandBar = FindAncestorCommandBar(refreshButton);
    }

    muxc::Panel parentPanel{nullptr};
    if (commandBar) {
        uint32_t idx = 0;
        if (!commandBar.PrimaryCommands().IndexOf(refreshButton, idx)) {
            commandBar = nullptr;
        }
    }

    if (!commandBar) {
        try {
            if (auto parent = muxm::VisualTreeHelper::GetParent(refreshButton)) {
                parentPanel = parent.try_as<muxc::Panel>();
            }
        } catch (...) {
        }
    }

    if (!commandBar && !parentPanel) {
        return false;
    }

    const ModSettings settings = GetSettingsSnapshot();

    muxc::AppBarButton homeButton;
    homeButton.Name(kHomeButtonName);
    ConfigureHomeButtonVisuals(homeButton, refreshButton, settings);

    InjectedButtonRecord record;
    record.homeButton = winrt::make_weak(homeButton);
    record.refreshButton = winrt::make_weak(refreshButton);

    record.clickRevoker = homeButton.Click(
        winrt::auto_revoke,
        [](wf::IInspectable const& sender, mux::RoutedEventArgs const&) {
            try {
                if (auto uiElem = sender.try_as<mux::UIElement>()) {
                    NavigateActiveTabToHome(uiElem);
                }
            } catch (...) {
                Wh_Log(L"Exception in Home button Click handler: %08X",
                       static_cast<unsigned>(winrt::to_hresult()));
            }
        });

    record.themeChangedRevoker = refreshButton.ActualThemeChanged(
        winrt::auto_revoke,
        [weakHome = winrt::make_weak(homeButton)](
            mux::FrameworkElement const& sender,
            wf::IInspectable const&) {
            try {
                if (auto home = weakHome.get()) {
                    home.RequestedTheme(sender.RequestedTheme());
                }
            } catch (...) {
            }
        });

    if (commandBar) {
        record.commandBar = winrt::make_weak(commandBar);
        PositionHomeButtonInCommandBar(commandBar, refreshButton, homeButton,
                                       settings.placement);
    } else if (parentPanel) {
        record.parentPanel = winrt::make_weak(parentPanel);
        PositionHomeButtonInPanel(parentPanel, refreshButton, homeButton,
                                  settings.placement);
    }

    g_injectedButtonsForThread.emplace(key, std::move(record));
    g_pendingRefreshForThread.erase(key);

    Wh_Log(L"Injected Home button adjacent to refreshButton (%p)", key);
    return true;
}

void EnsureRefreshButtonHooked(muxc::AppBarButton const& refreshButton,
                               muxc::CommandBar const& knownCommandBar = nullptr) {
    if (!refreshButton || g_uninitializing.load()) {
        return;
    }

    if (TryInjectHomeButtonForRefreshButton(refreshButton, knownCommandBar)) {
        return;
    }

    void* key = GetElementKey(refreshButton);
    if (!key || g_pendingRefreshForThread.contains(key)) {
        return;
    }

    PendingRefreshRecord pending;
    pending.refreshButton = winrt::make_weak(refreshButton);
    pending.loadedRevoker = refreshButton.Loaded(
        winrt::auto_revoke,
        [key](wf::IInspectable const& sender, mux::RoutedEventArgs const&) {
            g_pendingRefreshForThread.erase(key);
            if (auto btn = sender.try_as<muxc::AppBarButton>()) {
                TryInjectHomeButtonForRefreshButton(btn);
            }
        });
    g_pendingRefreshForThread.emplace(key, std::move(pending));
}

void InspectFrameworkElement(mux::FrameworkElement const& fe) {
    if (!fe || g_uninitializing.load()) {
        return;
    }

    const auto name = fe.Name();
    if (name == kHomeButtonName) {
        return;
    }

    if (name == L"refreshButton") {
        if (auto refreshBtn = fe.try_as<muxc::AppBarButton>()) {
            EnsureRefreshButtonHooked(refreshBtn);
        }
        return;
    }

    if (name == L"NavigationCommands") {
        if (auto cmdBar = fe.try_as<muxc::CommandBar>()) {
            for (auto const& item : cmdBar.PrimaryCommands()) {
                if (auto btn = item.try_as<muxc::AppBarButton>()) {
                    if (btn.Name() == L"refreshButton") {
                        EnsureRefreshButtonHooked(btn, cmdBar);
                        break;
                    }
                }
            }
        }
        return;
    }

    if (name == L"NavigationBarControl" ||
        name == L"NavigationBarControlGrid") {
        if (auto cmdBar =
                FindDescendantByName<muxc::CommandBar>(fe, L"NavigationCommands")) {
            for (auto const& item : cmdBar.PrimaryCommands()) {
                if (auto btn = item.try_as<muxc::AppBarButton>()) {
                    if (btn.Name() == L"refreshButton") {
                        EnsureRefreshButtonHooked(btn, cmdBar);
                        break;
                    }
                }
            }
        } else if (auto refreshBtn = FindDescendantByName<muxc::AppBarButton>(
                       fe, L"refreshButton")) {
            EnsureRefreshButtonHooked(refreshBtn);
        }
    }
}

void UpdateInjectedButtonsForCurrentThread() {
    const ModSettings settings = GetSettingsSnapshot();

    for (auto it = g_injectedButtonsForThread.begin();
         it != g_injectedButtonsForThread.end();) {
        auto homeBtn = it->second.homeButton.get();
        auto refreshBtn = it->second.refreshButton.get();
        if (!homeBtn || !refreshBtn) {
            it = g_injectedButtonsForThread.erase(it);
            continue;
        }

        try {
            ConfigureHomeButtonVisuals(homeBtn, refreshBtn, settings);
            if (auto cmdBar = it->second.commandBar.get()) {
                PositionHomeButtonInCommandBar(cmdBar, refreshBtn, homeBtn,
                                               settings.placement);
            } else if (auto panel = it->second.parentPanel.get()) {
                PositionHomeButtonInPanel(panel, refreshBtn, homeBtn,
                                          settings.placement);
            }
        } catch (...) {
            Wh_Log(L"Error updating Home button on thread: %08X",
                   static_cast<unsigned>(winrt::to_hresult()));
        }

        ++it;
    }
}

void RemoveInjectedButtonsForCurrentThread() {
    g_pendingRefreshForThread.clear();

    auto injected = std::move(g_injectedButtonsForThread);
    g_injectedButtonsForThread.clear();

    for (auto& [key, record] : injected) {
        record.clickRevoker.revoke();
        record.themeChangedRevoker.revoke();

        auto homeBtn = record.homeButton.get();
        if (!homeBtn) {
            continue;
        }

        try {
            if (auto cmdBar = record.commandBar.get()) {
                uint32_t idx = 0;
                if (cmdBar.PrimaryCommands().IndexOf(homeBtn, idx)) {
                    cmdBar.PrimaryCommands().RemoveAt(idx);
                }
            } else if (auto panel = record.parentPanel.get()) {
                uint32_t idx = 0;
                if (panel.Children().IndexOf(homeBtn, idx)) {
                    panel.Children().RemoveAt(idx);
                }
            }
        } catch (...) {
            Wh_Log(L"Error removing Home button during cleanup: %08X",
                   static_cast<unsigned>(winrt::to_hresult()));
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
// XAML Diagnostics TAP & VisualTreeWatcher
////////////////////////////////////////////////////////////////////////////////

static constexpr GUID IID_IXamlDiagnosticsTestHooks = {
    0x735941a2,
    0x3ee3,
    0x495a,
    {0x8d, 0xa9, 0x97, 0x26, 0x27, 0x00, 0x30, 0x75}};

struct IXamlDiagnosticsTestHooks : ::IUnknown {
    virtual HRESULT STDMETHODCALLTYPE
    UnregisterInstance(InstanceHandle handle) = 0;
    virtual HRESULT STDMETHODCALLTYPE
    TryGetDispatcherQueueForObject(InstanceHandle handle,
                                   void** dispatcherQueue) = 0;
};

// {74E8F5B1-39D8-46B2-B988-2A6998A1109D}
static constexpr CLSID CLSID_HomeExplorerTAP = {
    0x74e8f5b1,
    0x39d8,
    0x46b2,
    {0xb9, 0x88, 0x2a, 0x69, 0x98, 0xa1, 0x10, 0x9d}};

class VisualTreeWatcher
    : public winrt::implements<VisualTreeWatcher,
                               IVisualTreeServiceCallback2,
                               winrt::non_agile> {
   public:
    explicit VisualTreeWatcher(winrt::com_ptr<::IUnknown> site)
        : m_xamlDiagnostics(site.as<IXamlDiagnostics>()) {
        m_xamlDiagnostics->QueryInterface(
            IID_IXamlDiagnosticsTestHooks,
            m_xamlDiagnosticsTestHooks.put_void());

        // Advise on a worker thread to avoid deadlocking Advising::RunOnUIThread.
        HANDLE thread = CreateThread(
            nullptr, 0,
            [](LPVOID lpParam) -> DWORD {
                auto watcher = reinterpret_cast<VisualTreeWatcher*>(lpParam);
                HRESULT hr = watcher->m_xamlDiagnostics.as<IVisualTreeService3>()
                                 ->AdviseVisualTreeChange(watcher);
                watcher->Release();
                if (FAILED(hr)) {
                    Wh_Log(L"AdviseVisualTreeChange failed: %08X",
                           static_cast<unsigned>(hr));
                }
                return 0;
            },
            this, 0, nullptr);
        if (thread) {
            AddRef();
            CloseHandle(thread);
        }
    }

    VisualTreeWatcher(const VisualTreeWatcher&) = delete;
    VisualTreeWatcher& operator=(const VisualTreeWatcher&) = delete;

    void UnadviseVisualTreeChange() {
        if (m_xamlDiagnostics) {
            HRESULT hr = m_xamlDiagnostics.as<IVisualTreeService3>()
                             ->UnadviseVisualTreeChange(this);
            if (FAILED(hr)) {
                Wh_Log(L"UnadviseVisualTreeChange failed: %08X",
                       static_cast<unsigned>(hr));
            }
        }
    }

   private:
    HRESULT STDMETHODCALLTYPE OnVisualTreeChange(
        ParentChildRelation relation,
        VisualElement element,
        VisualMutationType mutationType) override try {
        if (g_uninitializing.load()) {
            return S_OK;
        }

        if (mutationType == Add) {
            wf::IInspectable inspectable;
            HRESULT hr = m_xamlDiagnostics->GetIInspectableFromHandle(
                element.Handle,
                reinterpret_cast<::IInspectable**>(
                    winrt::put_abi(inspectable)));
            if (SUCCEEDED(hr) && inspectable) {
                if (auto fe = inspectable.try_as<mux::FrameworkElement>()) {
                    InspectFrameworkElement(fe);
                }
            }
        }

        // Release XAML diagnostics handle references so elements aren't pinned.
        if (m_xamlDiagnosticsTestHooks) {
            if (mutationType == Add) {
                if (element.Handle) {
                    m_xamlDiagnosticsTestHooks->UnregisterInstance(
                        element.Handle);
                }
                if (relation.Parent) {
                    m_xamlDiagnosticsTestHooks->UnregisterInstance(
                        relation.Parent);
                }
            } else if (mutationType == Remove && element.Handle) {
                m_xamlDiagnosticsTestHooks->UnregisterInstance(element.Handle);
            }
        }

        return S_OK;
    } catch (...) {
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE OnElementStateChanged(
        InstanceHandle,
        VisualElementState,
        LPCWSTR) noexcept override {
        return S_OK;
    }

    winrt::com_ptr<IXamlDiagnostics> m_xamlDiagnostics;
    winrt::com_ptr<IXamlDiagnosticsTestHooks> m_xamlDiagnosticsTestHooks;
};

[[clang::no_destroy]] winrt::com_ptr<VisualTreeWatcher> g_visualTreeWatcher;
SRWLOCK g_visualTreeWatcherLock = SRWLOCK_INIT;

winrt::com_ptr<VisualTreeWatcher> ExchangeVisualTreeWatcher(
    winrt::com_ptr<VisualTreeWatcher> watcher) {
    AcquireSRWLockExclusive(&g_visualTreeWatcherLock);
    std::swap(g_visualTreeWatcher, watcher);
    ReleaseSRWLockExclusive(&g_visualTreeWatcherLock);
    return watcher;
}

class HomeExplorerTAP
    : public winrt::implements<HomeExplorerTAP,
                               IObjectWithSite,
                               winrt::non_agile> {
   public:
    HRESULT STDMETHODCALLTYPE SetSite(::IUnknown* pUnkSite) override try {
        if (auto previous = ExchangeVisualTreeWatcher(nullptr)) {
            previous->UnadviseVisualTreeChange();
        }

        m_site.copy_from(pUnkSite);

        if (m_site) {
            // Balance the LoadLibrary reference added by InitializeXamlDiagnosticsEx.
            FreeLibrary(GetCurrentModuleHandle());
            ExchangeVisualTreeWatcher(
                winrt::make_self<VisualTreeWatcher>(m_site));
        }

        return S_OK;
    } catch (...) {
        return winrt::to_hresult();
    }

    HRESULT STDMETHODCALLTYPE GetSite(REFIID riid,
                                      void** ppvSite) noexcept override {
        return m_site.as(riid, ppvSite);
    }

   private:
    winrt::com_ptr<::IUnknown> m_site;
};

template <class T>
struct SimpleFactory
    : winrt::implements<SimpleFactory<T>, IClassFactory, winrt::non_agile> {
    HRESULT STDMETHODCALLTYPE CreateInstance(::IUnknown* pUnkOuter,
                                             REFIID riid,
                                             void** ppvObject) override try {
        if (!pUnkOuter) {
            *ppvObject = nullptr;
            return winrt::make<T>().as(riid, ppvObject);
        }
        return CLASS_E_NOAGGREGATION;
    } catch (...) {
        return winrt::to_hresult();
    }

    HRESULT STDMETHODCALLTYPE LockServer(BOOL) noexcept override {
        return S_OK;
    }
};

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdll-attribute-on-redeclaration"

__declspec(dllexport) _Use_decl_annotations_ STDAPI DllGetClassObject(
    REFCLSID rclsid,
    REFIID riid,
    LPVOID* ppv) try {
    if (rclsid == CLSID_HomeExplorerTAP) {
        *ppv = nullptr;
        return winrt::make<SimpleFactory<HomeExplorerTAP>>().as(riid, ppv);
    }
    return CLASS_E_CLASSNOTAVAILABLE;
} catch (...) {
    return winrt::to_hresult();
}

__declspec(dllexport) _Use_decl_annotations_ STDAPI DllCanUnloadNow() {
    return winrt::get_module_lock() ? S_FALSE : S_OK;
}

#pragma clang diagnostic pop

using PFN_INITIALIZE_XAML_DIAGNOSTICS_EX =
    decltype(&InitializeXamlDiagnosticsEx);

HRESULT InjectWindhawkTAP() noexcept {
    HMODULE module = GetCurrentModuleHandle();
    if (!module) {
        return HRESULT_FROM_WIN32(GetLastError());
    }

    WCHAR location[MAX_PATH]{};
    DWORD len = GetModuleFileNameW(module, location, ARRAYSIZE(location));
    if (len == 0 || len == ARRAYSIZE(location)) {
        return HRESULT_FROM_WIN32(GetLastError());
    }

    const HMODULE wux = GetModuleHandleW(L"Microsoft.Internal.FrameworkUdk.dll");
    if (!wux) {
        return HRESULT_FROM_WIN32(GetLastError());
    }

    const auto ixde = reinterpret_cast<PFN_INITIALIZE_XAML_DIAGNOSTICS_EX>(
        GetProcAddress(wux, "InitializeXamlDiagnosticsEx"));
    if (!ixde) {
        return HRESULT_FROM_WIN32(GetLastError());
    }

    HRESULT hr = HRESULT_FROM_WIN32(ERROR_NOT_FOUND);
    for (int i = 0; i < 10000; i++) {
        WCHAR connectionName[256];
        wsprintfW(connectionName, L"WinUIVisualDiagConnection%d", i + 1);

        hr = ixde(connectionName, GetCurrentProcessId(), L"", location,
                  CLSID_HomeExplorerTAP, nullptr);
        if (hr != HRESULT_FROM_WIN32(ERROR_NOT_FOUND)) {
            break;
        }
    }

    return hr;
}

void EnsureTapInitialized() {
    if (g_uninitializing.load()) {
        return;
    }
    if (!GetModuleHandleW(L"Microsoft.Internal.FrameworkUdk.dll")) {
        return;
    }
    if (g_tapInitialized.exchange(true)) {
        return;
    }

    HRESULT hr = InjectWindhawkTAP();
    if (FAILED(hr)) {
        Wh_Log(L"InjectWindhawkTAP failed: %08X", static_cast<unsigned>(hr));
        g_tapInitialized.store(false);
    } else {
        Wh_Log(L"InjectWindhawkTAP succeeded");
    }
}

////////////////////////////////////////////////////////////////////////////////
// Window Thread Dispatch & Hooks
////////////////////////////////////////////////////////////////////////////////

using RunFromWindowThreadProc_t = void(WINAPI*)(PVOID parameter);

bool RunFromWindowThread(HWND hWnd,
                         RunFromWindowThreadProc_t proc,
                         PVOID procParam) {
    static const UINT runFromWindowThreadRegisteredMsg =
        RegisterWindowMessageW(L"Windhawk_RunFromWindowThread_" WH_MOD_ID);

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

    HHOOK hook = SetWindowsHookExW(
        WH_CALLWNDPROC,
        [](int nCode, WPARAM wParam, LPARAM lParam) -> LRESULT {
            if (nCode == HC_ACTION) {
                const CWPSTRUCT* cwp = reinterpret_cast<const CWPSTRUCT*>(lParam);
                if (cwp->message == runFromWindowThreadRegisteredMsg) {
                    auto* param = reinterpret_cast<RUN_FROM_WINDOW_THREAD_PARAM*>(
                        cwp->lParam);
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
    SendMessageW(hWnd, runFromWindowThreadRegisteredMsg, 0,
                 reinterpret_cast<LPARAM>(&param));

    UnhookWindowsHookEx(hook);
    return true;
}

std::vector<HWND> GetExplorerCabinetWindows() {
    std::vector<HWND> hWnds;
    EnumWindows(
        [](HWND hWnd, LPARAM lParam) -> BOOL {
            DWORD dwProcessId = 0;
            if (GetWindowThreadProcessId(hWnd, &dwProcessId) &&
                dwProcessId == GetCurrentProcessId() && IsCabinetWindow(hWnd)) {
                reinterpret_cast<std::vector<HWND>*>(lParam)->push_back(hWnd);
            }
            return TRUE;
        },
        reinterpret_cast<LPARAM>(&hWnds));
    return hWnds;
}

using CreateWindowExW_t = decltype(&CreateWindowExW);
CreateWindowExW_t CreateWindowExW_Original = nullptr;
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
    if (hWnd && IsCabinetWindow(hWnd)) {
        EnsureTapInitialized();
    }
    return hWnd;
}

using LoadLibraryExW_t = decltype(&LoadLibraryExW);
LoadLibraryExW_t LoadLibraryExW_Original = nullptr;
HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR lpLibFileName,
                                   HANDLE hFile,
                                   DWORD dwFlags) {
    HMODULE module = LoadLibraryExW_Original(lpLibFileName, hFile, dwFlags);
    if (module && lpLibFileName && !g_tapInitialized.load()) {
        PCWSTR fileName = wcsrchr(lpLibFileName, L'\\');
        fileName = fileName ? fileName + 1 : lpLibFileName;
        if (_wcsicmp(fileName, L"Microsoft.Internal.FrameworkUdk.dll") == 0 ||
            _wcsicmp(fileName, L"FileExplorerExtensions.dll") == 0 ||
            _wcsicmp(fileName, L"CoreMessagingXP.dll") == 0) {
            EnsureTapInitialized();
        }
    }
    return module;
}

////////////////////////////////////////////////////////////////////////////////
// Windhawk Mod Entry Points
////////////////////////////////////////////////////////////////////////////////

BOOL Wh_ModInit() {
    Wh_Log(L"Initializing explorer-home-button mod");

    g_uninitializing.store(false);
    LoadSettings();

    WindhawkUtils::SetFunctionHook(CreateWindowExW, CreateWindowExW_Hook,
                                   &CreateWindowExW_Original);

    HMODULE kernelBaseModule = GetModuleHandleW(L"kernelbase.dll");
    if (kernelBaseModule) {
        auto pLoadLibraryExW = reinterpret_cast<LoadLibraryExW_t>(
            GetProcAddress(kernelBaseModule, "LoadLibraryExW"));
        if (pLoadLibraryExW) {
            WindhawkUtils::SetFunctionHook(pLoadLibraryExW, LoadLibraryExW_Hook,
                                           &LoadLibraryExW_Original);
        }
    }

    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L"Wh_ModAfterInit");
    EnsureTapInitialized();
}

void Wh_ModSettingsChanged() {
    Wh_Log(L"Wh_ModSettingsChanged");
    LoadSettings();

    auto windows = GetExplorerCabinetWindows();
    std::unordered_set<DWORD> visitedThreads;
    for (HWND hWnd : windows) {
        DWORD tid = GetWindowThreadProcessId(hWnd, nullptr);
        if (tid && visitedThreads.insert(tid).second) {
            RunFromWindowThread(
                hWnd,
                [](PVOID) {
                    UpdateInjectedButtonsForCurrentThread();
                },
                nullptr);
        }
    }
}

void Wh_ModUninit() {
    Wh_Log(L"Uninitializing explorer-home-button mod");
    g_uninitializing.store(true);

    if (auto watcher = ExchangeVisualTreeWatcher(nullptr)) {
        watcher->UnadviseVisualTreeChange();
    }

    auto windows = GetExplorerCabinetWindows();
    std::unordered_set<DWORD> visitedThreads;
    for (HWND hWnd : windows) {
        DWORD tid = GetWindowThreadProcessId(hWnd, nullptr);
        if (tid && visitedThreads.insert(tid).second) {
            RunFromWindowThread(
                hWnd,
                [](PVOID) {
                    RemoveInjectedButtonsForCurrentThread();
                },
                nullptr);
        }
    }

    g_tapInitialized.store(false);
}
