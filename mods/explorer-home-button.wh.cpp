// ==WindhawkMod==
// @id              explorer-home-button
// @name            File Explorer Address Bar Home Button
// @description     Adds a dedicated "Home" (This PC) button next to the Refresh button in the Windows 11 File Explorer address bar
// @version         1.0.1
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
- **Clean Lifecycle**: Supports live injection into open File Explorer windows, live settings updates, and clean removal on mod unload without leaking XAML elements or conflicting with other XAML mods.
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
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <utility>
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
#include <winrt/base.h>

namespace wf = winrt::Windows::Foundation;
namespace mux = winrt::Microsoft::UI::Xaml;
namespace muxc = winrt::Microsoft::UI::Xaml::Controls;
namespace muxa = winrt::Microsoft::UI::Xaml::Automation;
namespace muxm = winrt::Microsoft::UI::Xaml::Media;
namespace muxi = winrt::Microsoft::UI::Xaml::Input;

#define kHomeButtonName L"WindhawkHomeExplorerButton"
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
    if (targetLocation.get() && *targetLocation.get()) {
        s.targetLocation = targetLocation.get();
    } else {
        s.targetLocation = kDefaultTargetLocation;
    }

    WindhawkUtils::StringSetting placement =
        WindhawkUtils::StringSetting::make(L"placement");
    if (placement.get() && wcscmp(placement.get(), L"before") == 0) {
        s.placement = ButtonPlacement::BeforeRefresh;
    } else {
        s.placement = ButtonPlacement::AfterRefresh;
    }

    WindhawkUtils::StringSetting iconGlyph =
        WindhawkUtils::StringSetting::make(L"iconGlyph");
    if (iconGlyph.get() && wcscmp(iconGlyph.get(), L"computer") == 0) {
        s.iconGlyph = kGlyphComputer;
    } else {
        s.iconGlyph = kGlyphHome;
    }

    WindhawkUtils::StringSetting tooltip =
        WindhawkUtils::StringSetting::make(L"tooltip");
    if (tooltip.get() && *tooltip.get()) {
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
    muxc::AppBarButton::Click_revoker clickRevoker;
};

struct PendingRefreshRecord {
    winrt::weak_ref<muxc::AppBarButton> refreshButton;
    mux::FrameworkElement::Loaded_revoker loadedRevoker;
};

thread_local std::unordered_map<void*, InjectedButtonRecord> g_injectedButtonsForThread;
thread_local std::unordered_map<void*, PendingRefreshRecord> g_pendingRefreshForThread;

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

void CopyVisualProperties(muxc::AppBarButton const& source,
                          muxc::AppBarButton const& target) {
    if (!source || !target) {
        return;
    }

    try {
        target.Width(source.Width());
        target.Height(source.Height());
        target.MinWidth(source.MinWidth());
        target.MinHeight(source.MinHeight());
        target.Margin(source.Margin());
        target.Padding(source.Padding());
        target.CornerRadius(source.CornerRadius());
        target.Style(source.Style());
        target.VerticalAlignment(source.VerticalAlignment());
        target.HorizontalAlignment(source.HorizontalAlignment());
    } catch (...) {
    }
}

void ConfigureHomeButtonVisuals(muxc::AppBarButton const& homeButton,
                                muxc::AppBarButton const& refreshButton,
                                ModSettings const& settings) {
    if (!homeButton) {
        return;
    }

    if (refreshButton) {
        CopyVisualProperties(refreshButton, homeButton);

        muxc::FontIcon homeIcon;
        homeIcon.Glyph(winrt::hstring(settings.iconGlyph));

        if (auto refreshIcon = refreshButton.Icon().try_as<muxc::FontIcon>()) {
            homeIcon.FontFamily(refreshIcon.FontFamily());
            homeIcon.FontSize(refreshIcon.FontSize());
            homeIcon.FontWeight(refreshIcon.FontWeight());
            homeIcon.FontStyle(refreshIcon.FontStyle());
            homeIcon.IsTextScaleFactorEnabled(refreshIcon.IsTextScaleFactorEnabled());
            homeIcon.MirroredWhenRightToLeft(refreshIcon.MirroredWhenRightToLeft());
        } else {
            homeIcon.FontFamily(muxm::FontFamily(L"Segoe Fluent Icons, Segoe MDL2 Assets"));
            homeIcon.FontSize(16.0);
        }

        homeButton.Icon(homeIcon);
    }

    muxc::ToolTipService::SetToolTip(homeButton,
                                     winrt::box_value(winrt::hstring(settings.tooltip)));

    // Keep automation / accessible name in sync with user-facing tooltip
    muxa::AutomationProperties::SetName(homeButton, winrt::hstring(settings.tooltip));
}

void PositionHomeButtonInCommandBar(muxc::CommandBar const& commandBar,
                                    muxc::AppBarButton const& refreshButton,
                                    muxc::AppBarButton const& homeButton,
                                    ButtonPlacement placement) {
    if (!commandBar || !refreshButton || !homeButton) {
        return;
    }

    auto commands = commandBar.PrimaryCommands();
    uint32_t existingHomeIdx = 0;
    if (commands.IndexOf(homeButton, existingHomeIdx)) {
        commands.RemoveAt(existingHomeIdx);
    }

    uint32_t refreshIdx = 0;
    if (commands.IndexOf(refreshButton, refreshIdx)) {
        uint32_t insertIdx = (placement == ButtonPlacement::BeforeRefresh)
                                 ? refreshIdx
                                 : (refreshIdx + 1);
        if (insertIdx > commands.Size()) {
            insertIdx = commands.Size();
        }
        commands.InsertAt(insertIdx, homeButton);
    } else {
        commands.Append(homeButton);
    }
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

    record.clickRevoker = homeButton.Click(
        winrt::auto_revoke,
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

    g_injectedButtonsForThread.emplace(key, std::move(record));
    g_pendingRefreshForThread.erase(key);

    Wh_Log(L"Injected Home button adjacent to refreshButton (%p)", key);
    return true;
}

void EnsureRefreshButtonHooked(muxc::AppBarButton const& refreshButton,
                               muxc::CommandBar const& knownCommandBar = nullptr) {
    if (!refreshButton || g_unloading.load()) {
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

void InspectXamlElement(mux::UIElement const& element) {
    if (!element || g_unloading.load()) {
        return;
    }

    try {
        if (auto refreshBtn =
                FindDescendantByName<muxc::AppBarButton>(element, L"refreshButton")) {
            EnsureRefreshButtonHooked(refreshBtn);
            return;
        }

        if (auto navCmds =
                FindDescendantByName<muxc::CommandBar>(element, L"NavigationCommands")) {
            for (auto const& item : navCmds.PrimaryCommands()) {
                if (auto btn = item.try_as<muxc::AppBarButton>()) {
                    if (btn.Name() == L"refreshButton") {
                        EnsureRefreshButtonHooked(btn, navCmds);
                        return;
                    }
                }
            }
        }
    } catch (...) {
    }
}

void ScanCurrentThreadForNavigationCommands() {
    if (g_unloading.load()) {
        return;
    }
    try {
        auto focused = muxi::FocusManager::GetFocusedElement();
        if (auto element = focused ? focused.try_as<mux::UIElement>() : nullptr) {
            if (auto root = element.XamlRoot()) {
                if (auto content = root.Content()) {
                    InspectXamlElement(content);
                    return;
                }
            }
        }
    } catch (...) {
    }
}

void ScheduleCurrentThreadScan() {
    if (g_unloading.load()) {
        return;
    }
    try {
        auto dispatcherQueue =
            winrt::Microsoft::UI::Dispatching::DispatcherQueue::GetForCurrentThread();
        if (dispatcherQueue) {
            dispatcherQueue.TryEnqueue([]() {
                ScanCurrentThreadForNavigationCommands();
            });
            return;
        }
    } catch (...) {
    }
    ScanCurrentThreadForNavigationCommands();
}

void UpdateInjectedButtonsForCurrentThread() {
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
}

void RemoveInjectedButtonsForCurrentThread() {
    for (auto& [key, record] : g_injectedButtonsForThread) {
        record.clickRevoker.revoke();
        if (auto cmdBar = record.commandBar.get()) {
            if (auto homeBtn = record.homeButton.get()) {
                uint32_t idx = 0;
                if (cmdBar.PrimaryCommands().IndexOf(homeBtn, idx)) {
                    cmdBar.PrimaryCommands().RemoveAt(idx);
                }
            }
        }
    }
    std::exchange(g_injectedButtonsForThread, {});
    std::exchange(g_pendingRefreshForThread, {});
}

////////////////////////////////////////////////////////////////////////////////
// Hooks of File Explorer WinUI 3 (FileExplorerExtensions.dll)
////////////////////////////////////////////////////////////////////////////////

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

        bar.Loaded([](wf::IInspectable const& sender, mux::RoutedEventArgs const&) {
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
        ScheduleCurrentThreadScan();
    }
}

void WINAPI CommandBarControl_Wave1_OnApplyTemplate_Hook(void* pThis) {
    CommandBarControl_Wave1_OnApplyTemplate_Original(pThis);
    if (!g_unloading.load()) {
        ScheduleCurrentThreadScan();
    }
}

using CommandBarControl_GotFocusHandler_t =
    void(WINAPI*)(void* pThis, void* sender, void* args);
CommandBarControl_GotFocusHandler_t CommandBarControl_GotFocusHandler_Original = nullptr;
CommandBarControl_GotFocusHandler_t CommandBarControl_Wave1_GotFocusHandler_Original = nullptr;

void WINAPI CommandBarControl_GotFocusHandler_Hook(void* pThis, void* sender, void* args) {
    CommandBarControl_GotFocusHandler_Original(pThis, sender, args);
    if (!g_unloading.load()) {
        ScheduleCurrentThreadScan();
    }
}

void WINAPI CommandBarControl_Wave1_GotFocusHandler_Hook(void* pThis, void* sender, void* args) {
    CommandBarControl_Wave1_GotFocusHandler_Original(pThis, sender, args);
    if (!g_unloading.load()) {
        ScheduleCurrentThreadScan();
    }
}

bool HookFileExplorerExtensionsSymbols(HMODULE module) {
    WindhawkUtils::SYMBOL_HOOK hooks[] = {
        {
            {LR"(public: void __cdecl winrt::FileExplorerExtensions::implementation::CommandBarManager::CommandBar(struct winrt::Microsoft::UI::Xaml::Controls::CommandBar const &))"},
            &CommandBarManager_CommandBar_Original,
            CommandBarManager_CommandBar_Hook,
            true,
        },
        {
            {LR"(public: void __cdecl winrt::FileExplorerExtensions::implementation::CommandBarControl::OnApplyTemplate(void))"},
            &CommandBarControl_OnApplyTemplate_Original,
            CommandBarControl_OnApplyTemplate_Hook,
            true,
        },
        {
            {LR"(public: void __cdecl winrt::FileExplorerExtensions::implementation::CommandBarControl_Wave1::OnApplyTemplate(void))"},
            &CommandBarControl_Wave1_OnApplyTemplate_Original,
            CommandBarControl_Wave1_OnApplyTemplate_Hook,
            true,
        },
        {
            {LR"(public: void __cdecl winrt::FileExplorerExtensions::implementation::CommandBarControl::CommandBarControlGotFocusHandler(struct winrt::Windows::Foundation::IInspectable const &,struct winrt::Microsoft::UI::Xaml::RoutedEventArgs const &))"},
            &CommandBarControl_GotFocusHandler_Original,
            CommandBarControl_GotFocusHandler_Hook,
            true,
        },
        {
            {LR"(public: void __cdecl winrt::FileExplorerExtensions::implementation::CommandBarControl_Wave1::CommandBarControlGotFocusHandler(struct winrt::Windows::Foundation::IInspectable const &,struct winrt::Microsoft::UI::Xaml::RoutedEventArgs const &))"},
            &CommandBarControl_Wave1_GotFocusHandler_Original,
            CommandBarControl_Wave1_GotFocusHandler_Hook,
            true,
        },
    };

    if (!WindhawkUtils::HookSymbols(module, hooks, ARRAYSIZE(hooks))) {
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
// Thread Invocation Helpers
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
        RegisterWindowMessageW(L"Windhawk_RunFromWindowThread_" kHomeButtonName);

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

////////////////////////////////////////////////////////////////////////////////
// Windhawk Mod Entry Points
////////////////////////////////////////////////////////////////////////////////

BOOL Wh_ModInit() {
    Wh_Log(L"Initializing explorer-home-button mod");

    g_unloading.store(false);
    LoadSettings();

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
    std::unordered_set<DWORD> visitedThreads;
    for (HWND hWnd : windows) {
        DWORD tid = GetWindowThreadProcessId(hWnd, nullptr);
        if (tid && visitedThreads.insert(tid).second) {
            RunFromWindowThread(
                hWnd,
                [](PVOID) {
                    ScheduleCurrentThreadScan();
                },
                nullptr);
        }
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
