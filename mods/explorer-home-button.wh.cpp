// ==WindhawkMod==
// @id              explorer-home-button
// @name            File Explorer Address Bar Home Button
// @description     Adds a dedicated "Home" (This PC) button next to the Refresh button in the Windows 11 File Explorer address bar
// @version         1.0.3
// @author          RobotsNeverDie
// @github          https://github.com/RBND
// @homepage        https://github.com/RBND/HomeExplorer
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject -lshell32 -luuid
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# File Explorer Address Bar Home Button

Adds a native WinUI 3 **Home ("This PC")** button directly adjacent to the **Refresh** button in the Windows 11 File Explorer address bar.

![File Explorer Address Bar Home Button](https://raw.githubusercontent.com/RBND/HomeExplorer/main/screenshot.png)

## Features
- **Native WinUI 3 Integration**: Injects an `AppBarButton` directly into File Explorer's address bar command bar (`CommandBar#NavigationCommands`) next to `refreshButton`.
- **Exact Visual & Theme Match**: Inherits the native `refreshButton`'s style, dimensions, margins, corner radius, and hover/pressed/focus visual states, dynamically matching Windows 11 Light, Dark, and High Contrast themes.
- **Active-Tab-Only Navigation**: Uses COM Shell APIs (`IShellWindows`, `IServiceProvider`, `IShellBrowser::BrowseObject`, and `IWebBrowser2::Navigate2`) matched against the active `ShellTabWindowClass` child window so only the currently active tab navigates to **This PC** (`shell:MyComputerFolder` / `::{20D04FE0-3AEA-1069-A2D8-08002B30309D}`).
- **Configurable Settings**: Customize the target shell location, button placement (before or after the Refresh button), icon glyph (`Home \uE80F` or `Computer \uE770`), and tooltip text (`This PC`).
- **Clean Lifecycle**: Supports live injection into open File Explorer windows (appearing immediately or on window focus), live settings updates, and clean removal on mod unload without leaking XAML elements or conflicting with other XAML mods.
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
- tooltip: "This PC"
  $name: Tooltip Text
  $description: >-
    The tooltip shown when hovering over the Home button. Defaults to "This PC".
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <exdisp.h>
#include <knownfolders.h>
#include <shlobj.h>
#include <shobjidl.h>

#include <atomic>
#include <cmath>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#undef GetCurrentTime

#include <winrt/Microsoft.UI.Xaml.Automation.h>
#include <winrt/Microsoft.UI.Xaml.Controls.Primitives.h>  // Required for ButtonBase::Click
#include <winrt/Microsoft.UI.Xaml.Controls.h>
#include <winrt/Microsoft.UI.Xaml.Media.h>
#include <winrt/Microsoft.UI.Xaml.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.Xaml.Interop.h>
#include <winrt/base.h>

namespace wf = winrt::Windows::Foundation;
namespace mux = winrt::Microsoft::UI::Xaml;
namespace muxc = winrt::Microsoft::UI::Xaml::Controls;
namespace muxa = winrt::Microsoft::UI::Xaml::Automation;
namespace muxm = winrt::Microsoft::UI::Xaml::Media;

constexpr WCHAR kHomeButtonName[] = L"WindhawkHomeExplorerButton";
constexpr WCHAR kThisPcClsidPath[] = L"::{20D04FE0-3AEA-1069-A2D8-08002B30309D}";
constexpr WCHAR kDefaultTargetLocation[] = L"shell:MyComputerFolder";
constexpr WCHAR kDefaultTooltip[] = L"This PC";
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
std::atomic<bool> g_unloading{false};

std::atomic<bool> g_fileExplorerExtensionsHooked{false};
std::atomic<bool> g_fileExplorerExtensionsHookAttempted{false};

ModSettings GetSettingsSnapshot() {
    AcquireSRWLockShared(&g_settingsLock);
    ModSettings copy = g_settings;
    ReleaseSRWLockShared(&g_settingsLock);
    return copy;
}

void LoadSettings() {
    ModSettings s;

    WindhawkUtils::StringSetting targetLocation =
        WindhawkUtils::StringSetting::make(L"targetLocation");
    if (*targetLocation.get()) {
        s.targetLocation = targetLocation.get();
    } else {
        s.targetLocation = kDefaultTargetLocation;
    }

    WindhawkUtils::StringSetting placement =
        WindhawkUtils::StringSetting::make(L"placement");
    if (wcscmp(placement.get(), L"before") == 0) {
        s.placement = ButtonPlacement::BeforeRefresh;
    } else {
        s.placement = ButtonPlacement::AfterRefresh;
    }

    WindhawkUtils::StringSetting iconGlyph =
        WindhawkUtils::StringSetting::make(L"iconGlyph");
    if (wcscmp(iconGlyph.get(), L"computer") == 0) {
        s.iconGlyph = kGlyphComputer;
    } else {
        s.iconGlyph = kGlyphHome;
    }

    WindhawkUtils::StringSetting tooltip =
        WindhawkUtils::StringSetting::make(L"tooltip");
    if (*tooltip.get()) {
        s.tooltip = tooltip.get();
    } else {
        s.tooltip = kDefaultTooltip;
    }

    AcquireSRWLockExclusive(&g_settingsLock);
    g_settings = std::move(s);
    ReleaseSRWLockExclusive(&g_settingsLock);
}

////////////////////////////////////////////////////////////////////////////////
// Window and Tab Helpers
////////////////////////////////////////////////////////////////////////////////

bool IsCabinetWindow(HWND hWnd) {
    if (!hWnd || !IsWindow(hWnd)) {
        return false;
    }
    WCHAR className[64] = {};
    if (!GetClassNameW(hWnd, className, ARRAYSIZE(className))) {
        return false;
    }
    return wcscmp(className, L"CabinetWClass") == 0;
}

HWND FindActiveTabWindow(HWND hCabinetWnd) {
    if (!hCabinetWnd) {
        return nullptr;
    }

    struct EnumContext {
        HWND hActiveTab = nullptr;
    } ctx;

    EnumChildWindows(
        hCabinetWnd,
        [](HWND hWnd, LPARAM lParam) -> BOOL {
            WCHAR className[64] = {};
            if (GetClassNameW(hWnd, className, ARRAYSIZE(className)) &&
                wcscmp(className, L"ShellTabWindowClass") == 0) {
                if (IsWindowVisible(hWnd)) {
                    auto pCtx = reinterpret_cast<EnumContext*>(lParam);
                    pCtx->hActiveTab = hWnd;
                    return FALSE;
                }
            }
            return TRUE;
        },
        reinterpret_cast<LPARAM>(&ctx));

    return ctx.hActiveTab;
}

HWND GetExplorerCabinetWindowForCurrentThread() {
    DWORD currentTid = GetCurrentThreadId();
    HWND foundWnd = nullptr;
    EnumThreadWindows(
        currentTid,
        [](HWND hWnd, LPARAM lParam) -> BOOL {
            if (IsCabinetWindow(hWnd)) {
                *reinterpret_cast<HWND*>(lParam) = hWnd;
                return FALSE;
            }
            return TRUE;
        },
        reinterpret_cast<LPARAM>(&foundWnd));

    return foundWnd;
}

PIDLIST_ABSOLUTE ResolveTargetPidl(const std::wstring& target) {
    if (target.empty()) {
        return nullptr;
    }

    PIDLIST_ABSOLUTE pidl = nullptr;
    HRESULT hr = S_OK;

    if (_wcsicmp(target.c_str(), L"shell:MyComputerFolder") == 0 ||
        _wcsicmp(target.c_str(), kThisPcClsidPath) == 0) {
        hr = SHGetKnownFolderIDList(FOLDERID_ComputerFolder, KF_FLAG_DEFAULT,
                                    nullptr, &pidl);
        if (SUCCEEDED(hr) && pidl) {
            return pidl;
        }
    }

    hr = SHParseDisplayName(target.c_str(), nullptr, &pidl, 0, nullptr);
    if (SUCCEEDED(hr) && pidl) {
        return pidl;
    }

    return nullptr;
}

bool NavigateActiveTabToLocation(HWND hExplorerWnd,
                                 const std::wstring& targetLocation) {
    PIDLIST_ABSOLUTE pidl = ResolveTargetPidl(targetLocation);
    if (!pidl) {
        return false;
    }

    HWND hActiveTab = FindActiveTabWindow(hExplorerWnd);

    winrt::com_ptr<IShellWindows> spShellWindows;
    HRESULT hr = CoCreateInstance(CLSID_ShellWindows, nullptr,
                                  CLSCTX_LOCAL_SERVER | CLSCTX_INPROC_SERVER,
                                  IID_PPV_ARGS(&spShellWindows));
    if (FAILED(hr) || !spShellWindows) {
        ILFree(pidl);
        return false;
    }

    long windowCount = 0;
    spShellWindows->get_Count(&windowCount);

    bool navigated = false;

    for (long i = 0; i < windowCount && !navigated; ++i) {
        winrt::com_ptr<IDispatch> spDisp;
        VARIANT vIndex;
        VariantInit(&vIndex);
        vIndex.vt = VT_I4;
        vIndex.lVal = i;

        hr = spShellWindows->Item(vIndex, spDisp.put());
        VariantClear(&vIndex);
        if (FAILED(hr) || !spDisp) {
            continue;
        }

        auto spWebBrowser = spDisp.try_as<IWebBrowser2>();
        if (!spWebBrowser) {
            continue;
        }

        // Fast HWND check before doing cross-thread/in-proc QueryService
        SHANDLE_PTR hWindowRaw = 0;
        if (FAILED(spWebBrowser->get_HWND(&hWindowRaw))) {
            continue;
        }

        HWND hItemWnd = reinterpret_cast<HWND>(hWindowRaw);
        if (hItemWnd != hExplorerWnd && hItemWnd != hActiveTab) {
            continue;
        }

        auto spServiceProvider = spDisp.try_as<IServiceProvider>();
        if (!spServiceProvider) {
            continue;
        }

        winrt::com_ptr<IShellBrowser> spShellBrowser;
        hr = spServiceProvider->QueryService(SID_STopLevelBrowser,
                                             IID_PPV_ARGS(&spShellBrowser));
        if (FAILED(hr) || !spShellBrowser) {
            continue;
        }

        HWND hBrowserTabWnd = nullptr;
        if (SUCCEEDED(spShellBrowser->GetWindow(&hBrowserTabWnd))) {
            if (hActiveTab && hBrowserTabWnd != hActiveTab) {
                continue;
            }
        }

        hr = spShellBrowser->BrowseObject(pidl, SBSP_SAMEBROWSER | SBSP_ABSOLUTE);
        if (SUCCEEDED(hr)) {
            navigated = true;
            break;
        }

        VARIANT vPidl;
        VariantInit(&vPidl);
        vPidl.vt = VT_ARRAY | VT_UI1;
        UINT cbPidl = ILGetSize(pidl);
        SAFEARRAY* psa = SafeArrayCreateVector(VT_UI1, 0, cbPidl);
        if (psa) {
            void* pData = nullptr;
            SafeArrayAccessData(psa, &pData);
            memcpy(pData, pidl, cbPidl);
            SafeArrayUnaccessData(psa);
            vPidl.parray = psa;

            VARIANT vFlags;
            VariantInit(&vFlags);
            vFlags.vt = VT_I4;
            vFlags.lVal = 0;

            hr = spWebBrowser->Navigate2(&vPidl, &vFlags, nullptr, nullptr, nullptr);
            VariantClear(&vPidl);
            VariantClear(&vFlags);
            if (SUCCEEDED(hr)) {
                navigated = true;
                break;
            }
        }
    }

    ILFree(pidl);
    return navigated;
}

void NavigateActiveTabToHome() {
    const ModSettings settings = GetSettingsSnapshot();

    HWND hExplorerWnd = GetExplorerCabinetWindowForCurrentThread();
    if (!hExplorerWnd) {
        Wh_Log(L"Failed to identify CabinetWClass window for current thread");
        return;
    }

    if (!NavigateActiveTabToLocation(hExplorerWnd, settings.targetLocation)) {
        Wh_Log(L"NavigateActiveTabToLocation failed for target: %ls",
               settings.targetLocation.c_str());
    }
}

////////////////////////////////////////////////////////////////////////////////
// UI Tracking and Visual Tree Helpers
////////////////////////////////////////////////////////////////////////////////

struct InjectedButtonRecord {
    winrt::weak_ref<muxc::AppBarButton> homeButton;
    winrt::weak_ref<muxc::AppBarButton> refreshButton;
    winrt::weak_ref<muxc::CommandBar> commandBar;
    winrt::event_token clickToken{};
    winrt::event_token loadedToken{};
    bool syncedWhenLoaded = false;
};

struct PendingRefreshRecord {
    winrt::weak_ref<muxc::AppBarButton> refreshButton;
    winrt::event_token loadedToken{};
};

struct KnownCommandBarRecord {
    winrt::weak_ref<muxc::CommandBar> commandBar;
    winrt::event_token loadedToken{};
};

struct KnownNavBarRecord {
    winrt::weak_ref<mux::UIElement> navBar;
    void* refreshKey = nullptr;
};

thread_local std::unordered_map<void*, InjectedButtonRecord> g_injectedButtonsForThread;
thread_local std::unordered_map<void*, PendingRefreshRecord> g_pendingRefreshForThread;
thread_local std::unordered_map<void*, KnownCommandBarRecord> g_knownCommandBarsForThread;
thread_local std::unordered_map<void*, KnownNavBarRecord> g_knownNavBarsForThread;

void* GetElementKey(mux::DependencyObject const& elem) {
    if (!elem) {
        return nullptr;
    }
    return winrt::get_abi(elem);
}

template <typename T>
T FindDescendantByName(mux::DependencyObject const& root,
                       std::wstring_view targetName,
                       int maxDepth = 64) {
    if (!root || maxDepth <= 0) {
        return nullptr;
    }

    if (auto elem = root.try_as<mux::FrameworkElement>()) {
        if (elem.Name() == targetName) {
            if (auto typed = root.try_as<T>()) {
                return typed;
            }
        }
    }

    int count = 0;
    try {
        count = muxm::VisualTreeHelper::GetChildrenCount(root);
    } catch (...) {
        return nullptr;
    }

    for (int i = 0; i < count; ++i) {
        mux::DependencyObject child{nullptr};
        try {
            child = muxm::VisualTreeHelper::GetChild(root, i);
        } catch (...) {
            continue;
        }
        if (auto found = FindDescendantByName<T>(child, targetName, maxDepth - 1)) {
            return found;
        }
    }
    return nullptr;
}

muxc::CommandBar FindAncestorCommandBar(mux::DependencyObject const& start) {
    if (!start) {
        return nullptr;
    }

    mux::DependencyObject current = start;
    int depth = 0;
    while (current && depth < 32) {
        mux::DependencyObject parent{nullptr};
        try {
            parent = muxm::VisualTreeHelper::GetParent(current);
        } catch (...) {
            break;
        }
        if (!parent) {
            if (auto fe = current.try_as<mux::FrameworkElement>()) {
                try {
                    parent = fe.Parent();
                } catch (...) {
                }
            }
        }
        if (!parent) {
            break;
        }
        if (auto cmdBar = parent.try_as<muxc::CommandBar>()) {
            return cmdBar;
        }
        current = parent;
        ++depth;
    }
    return nullptr;
}

mux::Style FindNavigationBarAppBarButtonStyle(mux::DependencyObject const& start) {
    mux::DependencyObject curr = start;
    int depth = 0;
    while (curr && depth < 32) {
        if (auto fe = curr.try_as<mux::FrameworkElement>()) {
            try {
                if (auto res = fe.Resources()) {
                    if (auto obj = res.TryLookup(winrt::box_value(
                            winrt::xaml_typename<muxc::AppBarButton>()))) {
                        if (auto style = obj.try_as<mux::Style>()) {
                            return style;
                        }
                    }
                    for (auto const& kvp : res) {
                        if (auto style = kvp.Value().try_as<mux::Style>()) {
                            if (style.TargetType().Name ==
                                L"Microsoft.UI.Xaml.Controls.AppBarButton") {
                                return style;
                            }
                        }
                    }
                }
            } catch (...) {
            }
        }

        mux::DependencyObject parent{nullptr};
        try {
            parent = muxm::VisualTreeHelper::GetParent(curr);
        } catch (...) {
        }
        if (!parent) {
            if (auto fe = curr.try_as<mux::FrameworkElement>()) {
                try {
                    parent = fe.Parent();
                } catch (...) {
                }
            }
        }
        curr = parent;
        ++depth;
    }
    return nullptr;
}

void CopyVisualProperties(muxc::AppBarButton const& source,
                          muxc::AppBarButton const& target) {
    if (!source || !target) {
        return;
    }

    try {
        mux::Style style = source.Style();
        if (!style) {
            style = FindNavigationBarAppBarButtonStyle(source);
        }
        if (style) {
            target.Style(style);
        }
    } catch (...) {
    }

    try {
        target.IsCompact(source.IsCompact());
        target.LabelPosition(muxc::CommandBarLabelPosition::Collapsed);
    } catch (...) {
    }

    // In DefaultAppBarButtonStyle, Width defaults to 68.0 before
    // NavigationBarControlGrid's implicit AppBarButton Style (32x32) is
    // resolved. Ignore unstyled >= 60 DIP values and default to 32.0.
    double btnWidth = 32.0;
    try {
        if (source.IsLoaded()) {
            double aw = source.ActualWidth();
            double w = source.Width();
            if (aw > 0.0 && aw < 60.0) {
                btnWidth = aw;
            } else if (!std::isnan(w) && w > 0.0 && w < 60.0) {
                btnWidth = w;
            }
        }
    } catch (...) {
    }

    try {
        target.Width(btnWidth);
        target.MinWidth(btnWidth);
        target.MaxWidth(btnWidth);
    } catch (...) {
    }

    double btnHeight = 32.0;
    try {
        if (source.IsLoaded()) {
            double ah = source.ActualHeight();
            double h = source.Height();
            if (ah > 0.0 && ah < 60.0) {
                btnHeight = ah;
            } else if (!std::isnan(h) && h > 0.0 && h < 60.0) {
                btnHeight = h;
            }
        }
    } catch (...) {
    }

    try {
        target.Height(btnHeight);
        target.MinHeight(btnHeight);
        target.MaxHeight(btnHeight);
    } catch (...) {
    }

    // Native NavigationCommands (height 48) uses AppBarButton with Height=32,
    // Margin={8,8,8,0}, VerticalAlignment=Top, and HorizontalAlignment=Left,
    // which places the 32 DIP button at y=8..40 (centered with 8 DIPs top and
    // 8 DIPs bottom). Using VerticalAlignment::Center with Margin.Top=8 would
    // shift the button down by +4 DIPs to y=12..44.
    try {
        target.HorizontalContentAlignment(mux::HorizontalAlignment::Center);
        target.VerticalContentAlignment(mux::VerticalAlignment::Center);
        target.HorizontalAlignment(source.IsLoaded()
                                       ? source.HorizontalAlignment()
                                       : mux::HorizontalAlignment::Left);
        target.VerticalAlignment(source.IsLoaded()
                                     ? source.VerticalAlignment()
                                     : mux::VerticalAlignment::Top);
    } catch (...) {
    }

    try {
        mux::Thickness margin{8.0, 8.0, 8.0, 0.0};
        if (source.IsLoaded()) {
            mux::Thickness sm = source.Margin();
            if (sm.Left != 0.0 || sm.Top != 0.0 || sm.Right != 0.0 || sm.Bottom != 0.0) {
                margin = sm;
            }
        }
        target.Margin(margin);
        target.Padding(source.IsLoaded() ? source.Padding()
                                         : mux::Thickness{0.0, 0.0, 0.0, 0.0});
    } catch (...) {
    }

    try {
        if (source.IsLoaded()) {
            target.CornerRadius(source.CornerRadius());
            target.BorderThickness(source.BorderThickness());
            target.UseSystemFocusVisuals(source.UseSystemFocusVisuals());
            target.FocusVisualMargin(source.FocusVisualMargin());
            target.FocusVisualPrimaryThickness(source.FocusVisualPrimaryThickness());
            target.FocusVisualSecondaryThickness(source.FocusVisualSecondaryThickness());
        }
    } catch (...) {
    }
}

void SyncHomeButtonTemplateParts(muxc::AppBarButton const& homeButton,
                                 muxc::AppBarButton const& refreshButton,
                                 double targetIconSize,
                                 double baseIconSize) {
    if (!homeButton) {
        return;
    }

    try {
        homeButton.ApplyTemplate();
    } catch (...) {
    }

    try {
        mux::VisualStateManager::GoToState(homeButton, L"LabelCollapsed", false);
    } catch (...) {
    }

    try {
        if (auto textLabel =
                FindDescendantByName<mux::UIElement>(homeButton, L"TextLabel")) {
            textLabel.Visibility(mux::Visibility::Collapsed);
        }
    } catch (...) {
    }

    try {
        if (auto homeInnerBorder = FindDescendantByName<mux::FrameworkElement>(
                homeButton, L"AppBarButtonInnerBorder")) {
            if (refreshButton) {
                if (auto refreshInnerBorder =
                        FindDescendantByName<mux::FrameworkElement>(
                            refreshButton, L"AppBarButtonInnerBorder")) {
                    homeInnerBorder.Margin(refreshInnerBorder.Margin());
                } else {
                    homeInnerBorder.Margin(mux::Thickness{0.0, 0.0, 0.0, 0.0});
                }
            } else {
                homeInnerBorder.Margin(mux::Thickness{0.0, 0.0, 0.0, 0.0});
            }
        }
    } catch (...) {
    }

    try {
        if (auto homeViewbox = FindDescendantByName<mux::FrameworkElement>(
                homeButton, L"ContentViewbox")) {
            // In NavigationBarControlGrid, AppBarButtonContentHeight is 12.0 and
            // AppBarButtonContentViewboxCollapsedMargin is {10, 10, 10, 10}
            // inside the 32x32 button (10 + 12 + 10 = 32). When targetIconSize
            // is optically scaled (e.g. 14.0 for E80F whose glyph outline is
            // 1792/2048 of em vs 2048/2048 for Back/Forward/Up/Refresh), adjust
            // the Viewbox size and margin symmetrically (9 + 14 + 9 = 32) so the
            // Viewbox doesn't shrink the 14 DIP FontIcon back to 12 DIPs.
            mux::Thickness baseMargin{10.0, 10.0, 10.0, 10.0};
            if (refreshButton) {
                if (auto refreshViewbox =
                        FindDescendantByName<mux::FrameworkElement>(
                            refreshButton, L"ContentViewbox")) {
                    mux::Thickness rm = refreshViewbox.Margin();
                    if (rm.Left > 0.0 && rm.Top > 0.0 && rm.Right > 0.0 &&
                        rm.Bottom > 0.0) {
                        baseMargin = rm;
                    }
                }
            }

            double halfDelta = (targetIconSize - baseIconSize) * 0.5;
            homeViewbox.Height(targetIconSize);
            homeViewbox.Width(targetIconSize);
            homeViewbox.Margin(mux::Thickness{
                (std::max)(0.0, baseMargin.Left - halfDelta),
                (std::max)(0.0, baseMargin.Top - halfDelta),
                (std::max)(0.0, baseMargin.Right - halfDelta),
                (std::max)(0.0, baseMargin.Bottom - halfDelta),
            });
        }
    } catch (...) {
    }
}

void ConfigureHomeButtonVisuals(muxc::AppBarButton const& homeButton,
                                muxc::AppBarButton const& refreshButton,
                                ModSettings const& settings) {
    if (!homeButton || !refreshButton) {
        return;
    }

    CopyVisualProperties(refreshButton, homeButton);

    muxc::FontIcon homeIcon;
    homeIcon.Glyph(winrt::hstring(settings.iconGlyph));

    double baseIconSize = 12.0; // Native NavBarIconSize in File Explorer NavigationCommands
    try {
        if (auto refreshIcon = refreshButton.Icon().try_as<muxc::FontIcon>()) {
            if (auto ff = refreshIcon.FontFamily()) {
                homeIcon.FontFamily(ff);
            }
            if (!std::isnan(refreshIcon.FontSize()) && refreshIcon.FontSize() > 0) {
                baseIconSize = refreshIcon.FontSize();
            }
            homeIcon.FontWeight(refreshIcon.FontWeight());
            homeIcon.FontStyle(refreshIcon.FontStyle());
            homeIcon.IsTextScaleFactorEnabled(refreshIcon.IsTextScaleFactorEnabled());
            homeIcon.MirroredWhenRightToLeft(refreshIcon.MirroredWhenRightToLeft());
        }
    } catch (...) {
    }

    // In Segoe Fluent Icons (UPM=2048), Back (E72B), Forward (E72A), Up (E74A),
    // and Refresh (E72C) fill the full 2048-unit em box (11x11 px at 12 DIPs),
    // whereas Home (E80F) has inset glyph bounds (1792x1918 units, walls 1536
    // units = 9x10 px at 12 DIPs). Scaling E80F by 14/12 (14.0 DIPs) matches
    // the exact 11x11 px visual size and stroke weight of the native buttons.
    double targetIconSize = (settings.iconGlyph == kGlyphHome)
                                ? std::round(baseIconSize * (14.0 / 12.0))
                                : baseIconSize;

    homeIcon.FontSize(targetIconSize);
    homeButton.Icon(homeIcon);

    SyncHomeButtonTemplateParts(homeButton, refreshButton, targetIconSize,
                                baseIconSize);

    muxc::ToolTipService::SetToolTip(
        homeButton, winrt::box_value(winrt::hstring(settings.tooltip)));

    // Keep automation / accessible name in sync with user-facing tooltip
    muxa::AutomationProperties::SetName(homeButton,
                                        winrt::hstring(settings.tooltip));
}

void PositionHomeButtonInCommandBar(muxc::CommandBar const& commandBar,
                                    muxc::AppBarButton const& refreshButton,
                                    muxc::AppBarButton const& homeButton,
                                    ButtonPlacement placement) {
    if (!commandBar || !refreshButton || !homeButton) {
        return;
    }

    auto commands = commandBar.PrimaryCommands();
    uint32_t refreshIdx = 0;
    uint32_t existingHomeIdx = 0;
    bool hasRefresh = commands.IndexOf(refreshButton, refreshIdx);
    bool hasHome = commands.IndexOf(homeButton, existingHomeIdx);

    if (hasRefresh && hasHome) {
        if (placement == ButtonPlacement::BeforeRefresh) {
            if (existingHomeIdx + 1 == refreshIdx) {
                return;
            }
        } else {
            uint32_t expectedIdx = refreshIdx + 1;
            if (refreshIdx + 1 < commands.Size()) {
                if (auto nextElem =
                        commands.GetAt(refreshIdx + 1).try_as<mux::FrameworkElement>()) {
                    if (nextElem.Name() == L"stopButton") {
                        expectedIdx = refreshIdx + 2;
                    }
                }
            }
            if (existingHomeIdx == expectedIdx) {
                return;
            }
        }
    }

    if (hasHome) {
        commands.RemoveAt(existingHomeIdx);
    }

    if (commands.IndexOf(refreshButton, refreshIdx)) {
        uint32_t insertIdx = refreshIdx + 1;
        if (placement == ButtonPlacement::BeforeRefresh) {
            insertIdx = refreshIdx;
        } else {
            // If stopButton immediately follows refreshButton, insert after stopButton
            if (refreshIdx + 1 < commands.Size()) {
                if (auto nextElem =
                        commands.GetAt(refreshIdx + 1).try_as<mux::FrameworkElement>()) {
                    if (nextElem.Name() == L"stopButton") {
                        insertIdx = refreshIdx + 2;
                    }
                }
            }
        }
        if (insertIdx > commands.Size()) {
            insertIdx = commands.Size();
        }
        commands.InsertAt(insertIdx, homeButton);
    } else {
        commands.Append(homeButton);
    }
}

void RemovePendingRefreshRecord(void* key) {
    auto it = g_pendingRefreshForThread.find(key);
    if (it == g_pendingRefreshForThread.end()) {
        return;
    }
    try {
        if (auto btn = it->second.refreshButton.get()) {
            if (it->second.loadedToken) {
                btn.Loaded(it->second.loadedToken);
            }
        }
    } catch (...) {
    }
    g_pendingRefreshForThread.erase(it);
}

bool TryInjectHomeButtonForRefreshButton(
    muxc::AppBarButton const& refreshButton,
    muxc::CommandBar const& knownCommandBar = nullptr) {
    if (!refreshButton || g_unloading.load()) {
        return false;
    }

    void* key = GetElementKey(refreshButton);
    if (!key) {
        return false;
    }

    auto existingIt = g_injectedButtonsForThread.find(key);
    if (existingIt != g_injectedButtonsForThread.end()) {
        if (auto existingBtn = existingIt->second.homeButton.get()) {
            if (!existingIt->second.syncedWhenLoaded &&
                existingBtn.IsLoaded() && refreshButton.IsLoaded()) {
                ConfigureHomeButtonVisuals(existingBtn, refreshButton,
                                           GetSettingsSnapshot());
                existingIt->second.syncedWhenLoaded = true;
            }
            return true;
        }
        g_injectedButtonsForThread.erase(existingIt);
    }

    muxc::CommandBar commandBar = knownCommandBar;
    if (!commandBar) {
        commandBar = FindAncestorCommandBar(refreshButton);
    }

    if (!commandBar) {
        return false;
    }

    uint32_t idx = 0;
    if (!commandBar.PrimaryCommands().IndexOf(refreshButton, idx)) {
        return false;
    }

    const ModSettings settings = GetSettingsSnapshot();

    muxc::AppBarButton homeButton;
    homeButton.Name(kHomeButtonName);
    ConfigureHomeButtonVisuals(homeButton, refreshButton, settings);

    InjectedButtonRecord record;
    record.homeButton = winrt::make_weak(homeButton);
    record.refreshButton = winrt::make_weak(refreshButton);
    record.commandBar = winrt::make_weak(commandBar);

    winrt::weak_ref<muxc::AppBarButton> weakRefresh(refreshButton);
    record.loadedToken = homeButton.Loaded(
        [key, weakRefresh](wf::IInspectable const& sender,
                           mux::RoutedEventArgs const&) {
            if (g_unloading.load()) {
                return;
            }
            try {
                auto btn = sender.try_as<muxc::AppBarButton>();
                auto refreshBtn = weakRefresh.get();
                if (btn && refreshBtn) {
                    ConfigureHomeButtonVisuals(btn, refreshBtn,
                                               GetSettingsSnapshot());
                    auto it = g_injectedButtonsForThread.find(key);
                    if (it != g_injectedButtonsForThread.end()) {
                        it->second.syncedWhenLoaded =
                            btn.IsLoaded() && refreshBtn.IsLoaded();
                    }
                }
            } catch (...) {
            }
        });

    record.clickToken = homeButton.Click(
        [](wf::IInspectable const&, mux::RoutedEventArgs const&) {
            try {
                NavigateActiveTabToHome();
            } catch (...) {
                Wh_Log(L"Exception in Home button Click handler: %08X",
                       static_cast<unsigned>(winrt::to_hresult()));
            }
        });

    PositionHomeButtonInCommandBar(commandBar, refreshButton, homeButton,
                                   settings.placement);

    // Re-sync visuals & template parts now that homeButton is inside CommandBar
    ConfigureHomeButtonVisuals(homeButton, refreshButton, settings);
    record.syncedWhenLoaded = homeButton.IsLoaded() && refreshButton.IsLoaded();

    g_injectedButtonsForThread.emplace(key, std::move(record));
    RemovePendingRefreshRecord(key);

    Wh_Log(L"Injected Home button adjacent to refreshButton (%p)", key);
    return true;
}

bool EnsureRefreshButtonHooked(muxc::AppBarButton const& refreshButton,
                               muxc::CommandBar const& knownCommandBar = nullptr) {
    if (!refreshButton || g_unloading.load()) {
        return false;
    }

    if (TryInjectHomeButtonForRefreshButton(refreshButton, knownCommandBar)) {
        return true;
    }

    void* key = GetElementKey(refreshButton);
    if (!key) {
        return false;
    }

    auto existingIt = g_pendingRefreshForThread.find(key);
    if (existingIt != g_pendingRefreshForThread.end()) {
        if (existingIt->second.refreshButton.get()) {
            return false;
        }
        g_pendingRefreshForThread.erase(existingIt);
    }

    PendingRefreshRecord pending;
    pending.refreshButton = winrt::make_weak(refreshButton);
    pending.loadedToken = refreshButton.Loaded(
        [key](wf::IInspectable const& sender, mux::RoutedEventArgs const&) {
            RemovePendingRefreshRecord(key);
            if (g_unloading.load()) {
                return;
            }
            try {
                if (auto btn = sender.try_as<muxc::AppBarButton>()) {
                    TryInjectHomeButtonForRefreshButton(btn);
                }
            } catch (...) {
            }
        });
    g_pendingRefreshForThread.emplace(key, std::move(pending));
    return false;
}

void* InspectXamlElement(mux::UIElement const& element) {
    if (!element || g_unloading.load()) {
        return nullptr;
    }

    try {
        if (auto navCmds =
                FindDescendantByName<muxc::CommandBar>(element, L"NavigationCommands")) {
            muxc::AppBarButton targetBtn{nullptr};
            for (auto const& item : navCmds.PrimaryCommands()) {
                if (auto btn = item.try_as<muxc::AppBarButton>()) {
                    auto name = btn.Name();
                    if (name == L"refreshButton") {
                        targetBtn = btn;
                        break;
                    }
                    if (name == L"stopButton" && !targetBtn) {
                        targetBtn = btn;
                    }
                }
            }
            if (targetBtn) {
                if (EnsureRefreshButtonHooked(targetBtn, navCmds)) {
                    return GetElementKey(targetBtn);
                }
                return nullptr;
            }
        }

        if (auto refreshBtn =
                FindDescendantByName<muxc::AppBarButton>(element, L"refreshButton")) {
            muxc::CommandBar parentBar = FindAncestorCommandBar(refreshBtn);
            if (EnsureRefreshButtonHooked(refreshBtn, parentBar)) {
                return GetElementKey(refreshBtn);
            }
            return nullptr;
        }

        if (auto stopBtn =
                FindDescendantByName<muxc::AppBarButton>(element, L"stopButton")) {
            muxc::CommandBar parentBar = FindAncestorCommandBar(stopBtn);
            if (EnsureRefreshButtonHooked(stopBtn, parentBar)) {
                return GetElementKey(stopBtn);
            }
            return nullptr;
        }
    } catch (...) {
    }
    return nullptr;
}

bool AllKnownNavBarsHaveLiveHomeButton() {
    if (g_knownNavBarsForThread.empty()) {
        return false;
    }
    bool anyLiveNavBar = false;
    for (auto it = g_knownNavBarsForThread.begin();
         it != g_knownNavBarsForThread.end();) {
        auto navBar = it->second.navBar.get();
        if (!navBar) {
            it = g_knownNavBarsForThread.erase(it);
            continue;
        }
        anyLiveNavBar = true;
        if (!it->second.refreshKey) {
            return false;
        }
        auto btnIt = g_injectedButtonsForThread.find(it->second.refreshKey);
        if (btnIt == g_injectedButtonsForThread.end() ||
            !btnIt->second.homeButton.get() ||
            !btnIt->second.commandBar.get()) {
            return false;
        }
        ++it;
    }
    return anyLiveNavBar;
}

void InspectNavigationBarOverridesThis(void* pOverridesThis, bool isLayoutPass) {
    if (!pOverridesThis || g_unloading.load()) {
        return;
    }

    if (isLayoutPass) {
        auto it = g_knownNavBarsForThread.find(pOverridesThis);
        if (it != g_knownNavBarsForThread.end()) {
            if (!it->second.navBar.get()) {
                g_knownNavBarsForThread.erase(it);
            } else if (it->second.refreshKey) {
                auto btnIt = g_injectedButtonsForThread.find(it->second.refreshKey);
                if (btnIt != g_injectedButtonsForThread.end()) {
                    auto homeBtn = btnIt->second.homeButton.get();
                    auto refreshBtn = btnIt->second.refreshButton.get();
                    auto cmdBar = btnIt->second.commandBar.get();
                    if (homeBtn && refreshBtn && cmdBar) {
                        if (!btnIt->second.syncedWhenLoaded &&
                            homeBtn.IsLoaded() && refreshBtn.IsLoaded()) {
                            try {
                                ConfigureHomeButtonVisuals(homeBtn, refreshBtn,
                                                           GetSettingsSnapshot());
                                btnIt->second.syncedWhenLoaded = true;
                            } catch (...) {
                            }
                        }
                        return;
                    }
                }
                it->second.refreshKey = nullptr;
            }
        }
    }

    try {
        for (auto it = g_knownNavBarsForThread.begin();
             it != g_knownNavBarsForThread.end();) {
            if (!it->second.navBar.get()) {
                it = g_knownNavBarsForThread.erase(it);
            } else {
                ++it;
            }
        }

        wf::IInspectable inspectable{nullptr};
        winrt::copy_from_abi(inspectable, pOverridesThis);
        if (auto navBarElement = inspectable ? inspectable.try_as<mux::UIElement>() : nullptr) {
            void* refreshKey = InspectXamlElement(navBarElement);
            KnownNavBarRecord& rec = g_knownNavBarsForThread[pOverridesThis];
            rec.navBar = winrt::make_weak(navBarElement);
            if (refreshKey) {
                rec.refreshKey = refreshKey;
            }
        }
    } catch (...) {
    }
}

void ScanCurrentThreadForNavigationCommands() {
    if (g_unloading.load()) {
        return;
    }

    for (auto it = g_knownNavBarsForThread.begin();
         it != g_knownNavBarsForThread.end();) {
        auto navBar = it->second.navBar.get();
        if (!navBar) {
            it = g_knownNavBarsForThread.erase(it);
            continue;
        }

        try {
            if (void* refreshKey = InspectXamlElement(navBar)) {
                it->second.refreshKey = refreshKey;
            }
        } catch (...) {
        }
        ++it;
    }

    for (auto it = g_knownCommandBarsForThread.begin();
         it != g_knownCommandBarsForThread.end();) {
        auto bar = it->second.commandBar.get();
        if (!bar) {
            it = g_knownCommandBarsForThread.erase(it);
            continue;
        }

        try {
            if (auto root = bar.XamlRoot()) {
                if (auto content = root.Content()) {
                    InspectXamlElement(content);
                }
            }
        } catch (...) {
        }
        ++it;
    }
}

void UpdateInjectedButtonsForCurrentThread() try {
    const ModSettings settings = GetSettingsSnapshot();

    for (auto it = g_injectedButtonsForThread.begin();
         it != g_injectedButtonsForThread.end();) {
        auto homeBtn = it->second.homeButton.get();
        auto refreshBtn = it->second.refreshButton.get();
        auto cmdBar = it->second.commandBar.get();

        if (!homeBtn || !refreshBtn || !cmdBar) {
            it = g_injectedButtonsForThread.erase(it);
            continue;
        }

        ConfigureHomeButtonVisuals(homeBtn, refreshBtn, settings);
        PositionHomeButtonInCommandBar(cmdBar, refreshBtn, homeBtn,
                                       settings.placement);
        ++it;
    }
} catch (...) {
    Wh_Log(L"Exception in UpdateInjectedButtonsForCurrentThread");
}

void RemoveInjectedButtonsForCurrentThread() try {
    g_knownNavBarsForThread.clear();

    for (auto& [key, rec] : g_knownCommandBarsForThread) {
        try {
            if (auto bar = rec.commandBar.get()) {
                if (rec.loadedToken) {
                    bar.Loaded(rec.loadedToken);
                }
            }
        } catch (...) {
        }
    }
    g_knownCommandBarsForThread.clear();

    for (auto& [key, rec] : g_pendingRefreshForThread) {
        try {
            if (auto btn = rec.refreshButton.get()) {
                if (rec.loadedToken) {
                    btn.Loaded(rec.loadedToken);
                }
            }
        } catch (...) {
        }
    }
    g_pendingRefreshForThread.clear();

    for (auto& [key, record] : g_injectedButtonsForThread) {
        if (auto homeBtn = record.homeButton.get()) {
            if (record.clickToken) {
                try {
                    homeBtn.Click(record.clickToken);
                } catch (...) {
                }
            }
            if (record.loadedToken) {
                try {
                    homeBtn.Loaded(record.loadedToken);
                } catch (...) {
                }
            }
            if (auto cmdBar = record.commandBar.get()) {
                try {
                    uint32_t idx = 0;
                    if (cmdBar.PrimaryCommands().IndexOf(homeBtn, idx)) {
                        cmdBar.PrimaryCommands().RemoveAt(idx);
                    }
                } catch (...) {
                }
            }
        }
    }
    g_injectedButtonsForThread.clear();
} catch (...) {
    Wh_Log(L"Exception in RemoveInjectedButtonsForCurrentThread");
}

////////////////////////////////////////////////////////////////////////////////
// Hooks of File Explorer WinUI 3 (FileExplorerExtensions.dll)
////////////////////////////////////////////////////////////////////////////////

void InspectHookSenderIfNeeded(void* sender) {
    if (g_unloading.load() || !sender) {
        return;
    }

    if (AllKnownNavBarsHaveLiveHomeButton()) {
        return;
    }

    try {
        auto inspectable = *reinterpret_cast<wf::IInspectable const*>(sender);
        if (auto uiElement = inspectable ? inspectable.try_as<mux::UIElement>() : nullptr) {
            InspectXamlElement(uiElement);
            if (auto xamlRoot = uiElement.XamlRoot()) {
                if (auto content = xamlRoot.Content()) {
                    InspectXamlElement(content);
                }
            }
        }
    } catch (...) {
    }
}

using NavigationBarControl_Overrides_OnApplyTemplate_t = int32_t(WINAPI*)(void* pThis);
NavigationBarControl_Overrides_OnApplyTemplate_t
    NavigationBarControl_Overrides_OnApplyTemplate_Original = nullptr;
int32_t WINAPI NavigationBarControl_Overrides_OnApplyTemplate_Hook(void* pThis) {
    int32_t hr = NavigationBarControl_Overrides_OnApplyTemplate_Original(pThis);
    if (!g_unloading.load()) {
        InspectNavigationBarOverridesThis(pThis, false);
    }
    return hr;
}

using NavigationBarControl_Overrides_MeasureOverride_t =
    int32_t(WINAPI*)(void* pThis, uint64_t availableSize, void* resultSize);
NavigationBarControl_Overrides_MeasureOverride_t
    NavigationBarControl_Overrides_MeasureOverride_Original = nullptr;
int32_t WINAPI NavigationBarControl_Overrides_MeasureOverride_Hook(
    void* pThis,
    uint64_t availableSize,
    void* resultSize) {
    if (!g_unloading.load()) {
        InspectNavigationBarOverridesThis(pThis, true);
    }
    return NavigationBarControl_Overrides_MeasureOverride_Original(
        pThis, availableSize, resultSize);
}

using NavigationBarControl_Overrides_ArrangeOverride_t =
    int32_t(WINAPI*)(void* pThis, uint64_t finalSize, void* resultSize);
NavigationBarControl_Overrides_ArrangeOverride_t
    NavigationBarControl_Overrides_ArrangeOverride_Original = nullptr;
int32_t WINAPI NavigationBarControl_Overrides_ArrangeOverride_Hook(
    void* pThis,
    uint64_t finalSize,
    void* resultSize) {
    int32_t hr = NavigationBarControl_Overrides_ArrangeOverride_Original(
        pThis, finalSize, resultSize);
    if (!g_unloading.load()) {
        InspectNavigationBarOverridesThis(pThis, true);
    }
    return hr;
}

using NavigationBarControl_OnApplyTemplate_t = void(WINAPI*)(void* pThis);
NavigationBarControl_OnApplyTemplate_t NavigationBarControl_OnApplyTemplate_Original = nullptr;
void WINAPI NavigationBarControl_OnApplyTemplate_Hook(void* pThis) {
    NavigationBarControl_OnApplyTemplate_Original(pThis);
    if (!g_unloading.load()) {
        ScanCurrentThreadForNavigationCommands();
    }
}

using NavigationBarControl_LosingFocusHandler_t =
    void(WINAPI*)(void* pThis, void* sender, void* args);
NavigationBarControl_LosingFocusHandler_t NavigationBarControl_LosingFocusHandler_Original = nullptr;
void WINAPI NavigationBarControl_LosingFocusHandler_Hook(void* pThis,
                                                         void* sender,
                                                         void* args) {
    NavigationBarControl_LosingFocusHandler_Original(pThis, sender, args);
    InspectHookSenderIfNeeded(sender);
}

using CommandBarManager_CommandBar_t = void(WINAPI*)(void* pThis, void* commandBar);
CommandBarManager_CommandBar_t CommandBarManager_CommandBar_Original = nullptr;
void WINAPI CommandBarManager_CommandBar_Hook(void* pThis, void* commandBar) {
    CommandBarManager_CommandBar_Original(pThis, commandBar);
    if (g_unloading.load() || !commandBar) {
        return;
    }

    try {
        auto bar = *reinterpret_cast<muxc::CommandBar const*>(commandBar);
        if (!bar) {
            return;
        }

        if (auto root = bar.XamlRoot()) {
            if (auto content = root.Content()) {
                InspectXamlElement(content);
            }
        }

        // Prune any dead commandBar records before inserting
        for (auto it = g_knownCommandBarsForThread.begin();
             it != g_knownCommandBarsForThread.end();) {
            if (!it->second.commandBar.get()) {
                it = g_knownCommandBarsForThread.erase(it);
            } else {
                ++it;
            }
        }

        void* barKey = GetElementKey(bar);
        if (barKey && !g_knownCommandBarsForThread.contains(barKey)) {
            KnownCommandBarRecord rec;
            rec.commandBar = winrt::make_weak(bar);
            rec.loadedToken = bar.Loaded(
                [](wf::IInspectable const& sender, mux::RoutedEventArgs const&) {
                    if (g_unloading.load()) {
                        return;
                    }
                    try {
                        if (auto b = sender.try_as<muxc::CommandBar>()) {
                            if (auto r = b.XamlRoot()) {
                                if (auto c = r.Content()) {
                                    InspectXamlElement(c);
                                }
                            }
                        }
                    } catch (...) {
                    }
                });
            g_knownCommandBarsForThread.emplace(barKey, std::move(rec));
        }
    } catch (...) {
        Wh_Log(L"Exception in CommandBarManager_CommandBar_Hook: %08X",
               static_cast<unsigned>(winrt::to_hresult()));
    }
}

using CommandBarControl_OnApplyTemplate_t = void(WINAPI*)(void* pThis);
CommandBarControl_OnApplyTemplate_t CommandBarControl_OnApplyTemplate_Original = nullptr;
CommandBarControl_OnApplyTemplate_t CommandBarControl_Wave1_OnApplyTemplate_Original = nullptr;

void WINAPI CommandBarControl_OnApplyTemplate_Hook(void* pThis) {
    CommandBarControl_OnApplyTemplate_Original(pThis);
    if (!g_unloading.load()) {
        ScanCurrentThreadForNavigationCommands();
    }
}

void WINAPI CommandBarControl_Wave1_OnApplyTemplate_Hook(void* pThis) {
    CommandBarControl_Wave1_OnApplyTemplate_Original(pThis);
    if (!g_unloading.load()) {
        ScanCurrentThreadForNavigationCommands();
    }
}

using CommandBarControl_GotFocusHandler_t =
    void(WINAPI*)(void* pThis, void* sender, void* args);
CommandBarControl_GotFocusHandler_t CommandBarControl_GotFocusHandler_Original = nullptr;
CommandBarControl_GotFocusHandler_t CommandBarControl_Wave1_GotFocusHandler_Original = nullptr;

void WINAPI CommandBarControl_GotFocusHandler_Hook(void* pThis, void* sender, void* args) {
    CommandBarControl_GotFocusHandler_Original(pThis, sender, args);
    InspectHookSenderIfNeeded(sender);
}

void WINAPI CommandBarControl_Wave1_GotFocusHandler_Hook(void* pThis, void* sender, void* args) {
    CommandBarControl_Wave1_GotFocusHandler_Original(pThis, sender, args);
    InspectHookSenderIfNeeded(sender);
}

bool HookFileExplorerExtensionsSymbols(HMODULE module) {
    WindhawkUtils::SYMBOL_HOOK fileExplorerExtensionsDllHooks[] = {
        {
            {
                LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::FileExplorerExtensions::implementation::NavigationBarControl,struct winrt::Microsoft::UI::Xaml::IFrameworkElementOverrides>::OnApplyTemplate(void))",
            },
            &NavigationBarControl_Overrides_OnApplyTemplate_Original,
            NavigationBarControl_Overrides_OnApplyTemplate_Hook,
            true,
        },
        {
            {
                LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::FileExplorerExtensions::implementation::NavigationBarControl,struct winrt::Microsoft::UI::Xaml::IFrameworkElementOverrides>::MeasureOverride(struct winrt::Windows::Foundation::Size,struct winrt::Windows::Foundation::Size *))",
            },
            &NavigationBarControl_Overrides_MeasureOverride_Original,
            NavigationBarControl_Overrides_MeasureOverride_Hook,
            true,
        },
        {
            {
                LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::FileExplorerExtensions::implementation::NavigationBarControl,struct winrt::Microsoft::UI::Xaml::IFrameworkElementOverrides>::ArrangeOverride(struct winrt::Windows::Foundation::Size,struct winrt::Windows::Foundation::Size *))",
            },
            &NavigationBarControl_Overrides_ArrangeOverride_Original,
            NavigationBarControl_Overrides_ArrangeOverride_Hook,
            true,
        },
        {
            {
                LR"(public: void __cdecl winrt::FileExplorerExtensions::implementation::NavigationBarControl::OnApplyTemplate(void))",
            },
            &NavigationBarControl_OnApplyTemplate_Original,
            NavigationBarControl_OnApplyTemplate_Hook,
            true,
        },
        {
            {
                LR"(public: void __cdecl winrt::FileExplorerExtensions::implementation::NavigationBarControl::LosingFocusHandler(struct winrt::Windows::Foundation::IInspectable const &,struct winrt::Microsoft::UI::Xaml::Input::LosingFocusEventArgs const &))",
            },
            &NavigationBarControl_LosingFocusHandler_Original,
            NavigationBarControl_LosingFocusHandler_Hook,
            true,
        },
        {
            {
                LR"(public: void __cdecl winrt::FileExplorerExtensions::implementation::CommandBarManager::CommandBar(struct winrt::Microsoft::UI::Xaml::Controls::CommandBar const &))",
            },
            &CommandBarManager_CommandBar_Original,
            CommandBarManager_CommandBar_Hook,
            true,
        },
        {
            {
                LR"(public: void __cdecl winrt::FileExplorerExtensions::implementation::CommandBarControl::OnApplyTemplate(void))",
            },
            &CommandBarControl_OnApplyTemplate_Original,
            CommandBarControl_OnApplyTemplate_Hook,
            true,
        },
        {
            {
                LR"(public: void __cdecl winrt::FileExplorerExtensions::implementation::CommandBarControl_Wave1::OnApplyTemplate(void))",
            },
            &CommandBarControl_Wave1_OnApplyTemplate_Original,
            CommandBarControl_Wave1_OnApplyTemplate_Hook,
            true,
        },
        {
            {
                LR"(public: void __cdecl winrt::FileExplorerExtensions::implementation::CommandBarControl::CommandBarControlGotFocusHandler(struct winrt::Windows::Foundation::IInspectable const &,struct winrt::Microsoft::UI::Xaml::RoutedEventArgs const &))",
            },
            &CommandBarControl_GotFocusHandler_Original,
            CommandBarControl_GotFocusHandler_Hook,
            true,
        },
        {
            {
                LR"(public: void __cdecl winrt::FileExplorerExtensions::implementation::CommandBarControl_Wave1::CommandBarControlGotFocusHandler(struct winrt::Windows::Foundation::IInspectable const &,struct winrt::Microsoft::UI::Xaml::RoutedEventArgs const &))",
            },
            &CommandBarControl_Wave1_GotFocusHandler_Original,
            CommandBarControl_Wave1_GotFocusHandler_Hook,
            true,
        },
    };

    if (!WindhawkUtils::HookSymbols(module, fileExplorerExtensionsDllHooks,
                                   ARRAYSIZE(fileExplorerExtensionsDllHooks))) {
        Wh_Log(L"HookSymbols failed for FileExplorerExtensions.dll");
        return false;
    }
    return true;
}

bool HookFileExplorerExtensionsIfLoaded(bool apply) {
    if (g_fileExplorerExtensionsHooked.load()) {
        return true;
    }
    HMODULE module = GetModuleHandleW(L"FileExplorerExtensions.dll");
    if (!module) {
        return false;
    }
    bool expected = false;
    if (!g_fileExplorerExtensionsHookAttempted.compare_exchange_strong(expected, true)) {
        return g_fileExplorerExtensionsHooked.load();
    }
    if (!HookFileExplorerExtensionsSymbols(module)) {
        Wh_Log(L"FileExplorerExtensions symbol hook failed");
        return false;
    }
    g_fileExplorerExtensionsHooked.store(true);
    if (apply) {
        Wh_ApplyHookOperations();
    }
    return true;
}

using LoadLibraryExW_t = decltype(&LoadLibraryExW);
LoadLibraryExW_t LoadLibraryExW_Original = nullptr;
HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR lpLibFileName,
                                   HANDLE hFile,
                                   DWORD dwFlags) {
    HMODULE module = LoadLibraryExW_Original(lpLibFileName, hFile, dwFlags);
    if (module && lpLibFileName && !g_unloading.load() &&
        !g_fileExplorerExtensionsHookAttempted.load()) {
        PCWSTR fileName = wcsrchr(lpLibFileName, L'\\');
        fileName = fileName ? fileName + 1 : lpLibFileName;
        if (_wcsicmp(fileName, L"FileExplorerExtensions.dll") == 0) {
            HookFileExplorerExtensionsIfLoaded(true);
        }
    }
    return module;
}

////////////////////////////////////////////////////////////////////////////////
// Thread Invocation and Live Window Layout Nudge Helpers
////////////////////////////////////////////////////////////////////////////////

using RUN_FROM_WINDOW_THREAD_PROC = void (*)(PVOID param);

struct RUN_FROM_WINDOW_THREAD_PARAM {
    RUN_FROM_WINDOW_THREAD_PROC proc;
    PVOID procParam;
};

bool RunFromWindowThread(HWND hWnd,
                         RUN_FROM_WINDOW_THREAD_PROC proc,
                         PVOID procParam) {
    static UINT runFromWindowThreadRegisteredMsg =
        RegisterWindowMessageW(L"Windhawk_RunFromWindowThread_" WH_MOD_ID);

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
                auto pMessage = reinterpret_cast<const CWPSTRUCT*>(lParam);
                if (pMessage->message == runFromWindowThreadRegisteredMsg) {
                    auto pParam = reinterpret_cast<RUN_FROM_WINDOW_THREAD_PARAM*>(
                        pMessage->lParam);
                    pParam->proc(pParam->procParam);
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

void TriggerLiveLayoutPassForWindow(HWND hCabinetWnd) {
    if (!hCabinetWnd || g_unloading.load()) {
        return;
    }

    ScanCurrentThreadForNavigationCommands();

    EnumChildWindows(
        hCabinetWnd,
        [](HWND hChild, LPARAM) -> BOOL {
            WCHAR cls[128] = {};
            if (GetClassNameW(hChild, cls, ARRAYSIZE(cls))) {
                if (wcscmp(cls, L"Microsoft.UI.Content.DesktopChildSiteBridge") == 0 ||
                    wcscmp(cls, L"Windows.UI.Composition.DesktopWindowContentBridge") == 0 ||
                    wcscmp(cls, L"WorkerW") == 0) {
                    RECT rc{};
                    if (GetWindowRect(hChild, &rc)) {
                        int w = rc.right - rc.left;
                        int h = rc.bottom - rc.top;
                        if (w > 10 && h > 10) {
                            constexpr UINT kFlags = SWP_NOMOVE | SWP_NOZORDER |
                                                    SWP_NOACTIVATE |
                                                    SWP_NOOWNERZORDER;
                            SetWindowPos(hChild, nullptr, 0, 0, w + 1, h, kFlags);
                            UpdateWindow(hChild);
                            SetWindowPos(hChild, nullptr, 0, 0, w, h, kFlags);
                        }
                    }
                }
            }
            return TRUE;
        },
        0);

    ScanCurrentThreadForNavigationCommands();
}

////////////////////////////////////////////////////////////////////////////////
// Windhawk Mod Entry Points
////////////////////////////////////////////////////////////////////////////////

BOOL Wh_ModInit() {
    Wh_Log(L"Initializing explorer-home-button mod");

    g_unloading.store(false);
    LoadSettings();

    // If FileExplorerExtensions.dll is already loaded in explorer.exe, register
    // its symbol hooks immediately during ModInit so Windhawk applies them
    // alongside LoadLibraryExW.
    HookFileExplorerExtensionsIfLoaded(false);

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
    HookFileExplorerExtensionsIfLoaded(true);

    auto windows = GetExplorerCabinetWindows();
    for (HWND hWnd : windows) {
        RunFromWindowThread(
            hWnd,
            [](PVOID param) {
                TriggerLiveLayoutPassForWindow(static_cast<HWND>(param));
            },
            hWnd);
    }
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

void Wh_ModBeforeUninit() {
    Wh_Log(L"Wh_ModBeforeUninit");
    g_unloading.store(true);
}

void Wh_ModUninit() {
    Wh_Log(L"Wh_ModUninit");
    g_unloading.store(true);

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
}
