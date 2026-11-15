// ==WindhawkMod==
// @id              winv-panel-tweaks
// @name            Win+V Panel Tweaks
// @description     Resize the Win+V clipboard / Win+. emoji panel and hide the tabs you don't use (Emoji, GIF, Kaomoji, Symbols, Most recently used)
// @version         1.0.1
// @author          jayll1303
// @github          https://github.com/jayll1303
// @license         MIT
// @include         TextInputHost.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject -lshcore
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Win+V Panel Tweaks

The Win+V clipboard panel and the Win+. emoji panel are the same panel, with
tabs for Most recently used, Emoji, GIF, Kaomoji, Symbols and Clipboard. This
mod makes the panel bigger or smaller and hides the tabs you turn off in its
settings.

![A taller Win+V panel showing more clipboard entries, with only the Clipboard tab left](https://raw.githubusercontent.com/jayll1303/WindowsClipboardMods/main/images/winv-panel-tweaks-taller-clipboard-only.png)

## Panel size

The panel is normally 360 x 400 (in display-scaled pixels). Width and height
are scaled separately, e.g. width 150% and height 100% gives 540 x 400.
The panel never grows past the usable area of the screen it opens on. A new
size applies the next time the panel opens.

By default the items keep their normal size, so a taller panel shows more
clipboard entries and a wider one fits more emoji per row. Turn on "Scale
content with the panel" to have the items grow with the panel instead.

## Tabs

The Clipboard tab is always kept. Hiding a tab only removes its button: a
shortcut that opens a view directly still opens it (e.g. Win+. still opens
Emoji even when the Emoji tab is hidden).

## How it works

The panel is drawn by TextInputHost.exe. Its size is computed in
`InputApp.dll`, and capped by a limit that `TextInput.dll` reports (360 x 418
by default), so a bigger panel needs that limit raised too. The mod hooks both
using Microsoft's public symbols, so the first start after installing, and the
first start after a Windows update, may take a moment while Windhawk downloads
them.

The tab strip lives in a web page hosted in a WebView inside the panel. The
same hook finds that WebView each time the panel opens and runs a small script
in the page, which marks the chosen tabs as hidden in the page's own view
model, re-applying it whenever the page resets them. Tweaks therefore show up
the next time the panel opens after the mod is enabled.

That page ships with Windows but Microsoft can update it separately. If it
changes in a way the mod doesn't recognize, the mod leaves it alone and the
panel keeps working as usual.

Tested on Windows 11 25H2 (build 26200).

Source and research notes: https://github.com/jayll1303/WindowsClipboardMods
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- panelWidth: 100
  $name: Panel width (%)
  $description: >-
    Percent of the default width (360). Between 50 and 300.
- panelHeight: 100
  $name: Panel height (%)
  $description: >-
    Percent of the default height (400). Between 50 and 300.
- scaleContent: false
  $name: Scale content with the panel
  $description: >-
    Off: items keep their normal size and a bigger panel shows more of them.
    On: items grow and shrink with the panel, as Windows does by default.
- hideRecent: false
  $name: Hide "Most recently used" tab
- hideEmoji: false
  $name: Hide "Emoji" tab
- hideGif: true
  $name: Hide "GIF" tab
- hideKaomoji: true
  $name: Hide "Kaomoji" tab
- hideSymbols: false
  $name: Hide "Symbols" tab
*/
// ==/WindhawkModSettings==

#include <windows.h>

#include <windhawk_utils.h>

#include <shellscalingapi.h>

#include <algorithm>
#include <atomic>
#include <cfloat>
#include <mutex>
#include <string>

#undef GetCurrentTime

#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.h>

namespace wf = winrt::Windows::Foundation;
namespace wux = winrt::Windows::UI::Xaml;
namespace wuxc = winrt::Windows::UI::Xaml::Controls;
namespace wuxm = winrt::Windows::UI::Xaml::Media;

////////////////////////////////////////////////////////////////////////////////
// Settings and the page script.

// Tab ids as the page names them (the part before any '/').
struct TabSetting {
    PCWSTR settingName;
    PCWSTR tabId;
};

constexpr TabSetting kTabSettings[] = {
    {L"hideRecent", L"recent"},   {L"hideEmoji", L"emoji"},
    {L"hideGif", L"gif"},         {L"hideKaomoji", L"kaomoji"},
    {L"hideSymbols", L"symbol"},
};

// The panel's default size, in DIPs, as set by InputApp.dll: 360 wide, and the
// title row (32) plus 368 for the web content high.
constexpr float kDefaultPanelWidth = 360;
constexpr float kDefaultPanelHeight = 400;
constexpr float kDefaultContentHeight = 368;

constexpr int kMinPanelPercent = 50;
constexpr int kMaxPanelPercent = 300;

std::atomic<int> g_panelWidthPercent = 100;
std::atomic<int> g_panelHeightPercent = 100;

std::mutex g_settingsMutex;
// The page script's configuration, as a JavaScript object literal.
std::wstring g_pageConfig;

int GetPanelPercentSetting(PCWSTR name) {
    int value = Wh_GetIntSetting(name);
    if (value <= 0) {
        // Unset or invalid: keep the default size.
        return 100;
    }
    return std::clamp(value, kMinPanelPercent, kMaxPanelPercent);
}

// The configuration that undoes everything the script changed.
constexpr wchar_t kRestorePageConfig[] =
    L"{hidden:[],keepScale:false,contentWidth:0,contentHeight:0}";

void LoadSettings() {
    int widthPercent = GetPanelPercentSetting(L"panelWidth");
    int heightPercent = GetPanelPercentSetting(L"panelHeight");
    bool resized = widthPercent != 100 || heightPercent != 100;
    bool keepScale = resized && !Wh_GetIntSetting(L"scaleContent");
    g_panelWidthPercent = widthPercent;
    g_panelHeightPercent = heightPercent;

    std::wstring hidden;
    for (auto const& tab : kTabSettings) {
        if (Wh_GetIntSetting(tab.settingName)) {
            if (!hidden.empty()) {
                hidden += L",";
            }
            hidden += L"'";
            hidden += tab.tabId;
            hidden += L"'";
        }
    }

    WCHAR sizeConfig[256];
    swprintf_s(sizeConfig, L"keepScale:%s,contentWidth:%g,contentHeight:%g",
               keepScale ? L"true" : L"false", kDefaultPanelWidth,
               kDefaultContentHeight);

    std::wstring config = L"{hidden:[" + hidden + L"]," + sizeConfig + L"}";

    std::lock_guard lock(g_settingsMutex);
    g_pageConfig = config;
    Wh_Log(L"Panel %d%% x %d%%, page config: %s", widthPercent, heightPercent,
           config.c_str());
}

// Installs the page tweaks, or removes them with kRestorePageConfig.
// Idempotent: state lives in window.__winvPanelTweaks and every run replaces
// the previous configuration. Waits for the page's Knockout bindings if they
// aren't ready yet, and gives up quietly if the page doesn't look as expected.
//
// Tabs: the chosen tabs are marked hidden in the page's view model, and kept
// hidden whenever the page resets them; tabs this script hid but which are no
// longer chosen are shown again.
//
// Content scale: the page scales its content to the size of its area in the
// panel. With keepScale, the page is told the default size of that area when
// it computes the scale, so the content keeps its normal scale and a bigger
// panel shows more of it. The layout itself still uses the real size.
constexpr wchar_t kScriptTemplate[] = LR"JS((function (config) {
  var S = window.__winvPanelTweaks || (window.__winvPanelTweaks = { subs: [], changed: {} });
  S.subs.forEach(function (s) { s.dispose(); });
  S.subs = [];
  if (S.timer) { clearInterval(S.timer); S.timer = 0; }
  S.config = config;
  var hidden = config.hidden;

  function baseId(id) { id = String(id || ''); var i = id.indexOf('/'); return i < 0 ? id : id.substring(0, i); }
  function isTarget(id) { return hidden.indexOf(baseId(id)) >= 0; }

  var style = document.getElementById('winv-panel-tweaks-style');
  if (hidden.length) {
    if (!style) {
      style = document.createElement('style');
      style.id = 'winv-panel-tweaks-style';
      (document.head || document.documentElement).appendChild(style);
    }
    style.textContent = hidden.map(function (h) {
      return '[id^="navigation-menu-item-container-' + h + '"]{display:none!important}';
    }).join('\n');
  } else if (style) {
    style.parentNode.removeChild(style);
  }

  function hookSizeInfo(provider) {
    if (!provider || typeof provider.onContainerViewSizeInfoUpdated !== 'function') return;
    if (!config.keepScale) {
      if (S.provider) { delete S.provider.onContainerViewSizeInfoUpdated; S.provider = null; }
      return;
    }
    if (S.provider === provider) return;
    var original = Object.getPrototypeOf(provider).onContainerViewSizeInfoUpdated;
    if (typeof original !== 'function') return;
    provider.onContainerViewSizeInfoUpdated = function (info) {
      var c = window.__winvPanelTweaks && window.__winvPanelTweaks.config;
      if (c && c.keepScale && info) {
        info = Object.assign({}, info);
        if (typeof info.totalViewWidth === 'number') info.totalViewWidth = c.contentWidth;
        if (typeof info.totalViewHeight === 'number') info.totalViewHeight = (info.collapsedViewHeight || 0) + c.contentHeight;
      }
      return original.call(this, info);
    };
    S.provider = provider;
  }

  function install() {
    if (typeof ko === 'undefined') return false;
    var container = document.getElementById('navigation-menu-container');
    var root = container && ko.dataFor(container);
    var nav = root && root.navigationMenuViewModel;
    if (!nav || !nav.navigationMenuItems) return false;

    hookSizeInfo(root.viewDimensionsProvider);

    var itemSubs = [];
    function apply() {
      itemSubs.forEach(function (s) { s.dispose(); });
      itemSubs = [];
      (ko.unwrap(nav.navigationMenuItems) || []).forEach(function (item) {
        var flag = item.shouldBeHidden;
        if (!ko.isWriteableObservable(flag)) return;
        var id = baseId(ko.unwrap(item.id));
        if (isTarget(id)) {
          if (!flag.peek()) { flag(true); S.changed[id] = true; }
          itemSubs.push(flag.subscribe(function (v) { if (!v) flag(true); }));
        } else if (S.changed[id]) {
          flag(false);
          delete S.changed[id];
        }
      });
    }
    apply();
    S.subs.push({ dispose: function () { itemSubs.forEach(function (s) { s.dispose(); }); itemSubs = []; } });
    if (ko.isObservable(nav.navigationMenuItems)) {
      S.subs.push(nav.navigationMenuItems.subscribe(apply));
    }
    return true;
  }

  if (!install()) {
    var tries = 0;
    S.timer = setInterval(function () {
      if (install() || ++tries > 100) { clearInterval(S.timer); S.timer = 0; }
    }, 200);
    return 'waiting';
  }
  if (!hidden.length && !config.keepScale) {
    // Everything was restored: drop the subscriptions and the state.
    S.subs.forEach(function (s) { s.dispose(); });
    delete window.__winvPanelTweaks;
    return 'removed';
  }
  return 'installed';
})(%CONFIG%))JS";

std::wstring BuildScript(const std::wstring& pageConfig) {
    std::wstring script = kScriptTemplate;
    script.replace(script.find(L"%CONFIG%"), 8, pageConfig);
    return script;
}

std::wstring CurrentScript() {
    std::lock_guard lock(g_settingsMutex);
    return BuildScript(g_pageConfig);
}

////////////////////////////////////////////////////////////////////////////////
// The panel's WebView.
//
// The WebView is found from the ResetControlSize hook, which InputApp.dll
// calls on the panel's UI thread each time the panel opens. Its state below is
// only touched on that thread; other threads get there through
// RunOnPanelThread.

std::atomic<bool> g_unloading;

// The panel's UI thread, once the WebView was found there.
std::atomic<DWORD> g_panelThreadId;

winrt::weak_ref<wuxc::WebView> g_webView;
winrt::event_token g_navigationCompletedToken;

// No completion handler is attached, so nothing from the mod is left on the
// async operation.
void RunScript(wuxc::WebView const& webView, std::wstring const& script) {
    try {
        webView.InvokeScriptAsync(L"eval", {winrt::hstring(script)});
    } catch (...) {
        Wh_Log(L"InvokeScriptAsync error %08X", winrt::to_hresult());
    }
}

wuxc::WebView FindWebView(wux::DependencyObject const& element) {
    if (auto webView = element.try_as<wuxc::WebView>()) {
        return webView;
    }
    int count = wuxm::VisualTreeHelper::GetChildrenCount(element);
    for (int i = 0; i < count; i++) {
        if (auto webView =
                FindWebView(wuxm::VisualTreeHelper::GetChild(element, i))) {
            return webView;
        }
    }
    return nullptr;
}

// pageAbi is the ExpressiveSuggestionUIPage the hook was called for.
void TrackPanelWebView(void* pageAbi) {
    if (g_unloading || g_webView.get()) {
        return;
    }

    wf::IInspectable page;
    winrt::copy_from_abi(page, pageAbi);
    auto pageElement = page.try_as<wux::DependencyObject>();
    if (!pageElement) {
        return;
    }

    auto webView = FindWebView(pageElement);
    if (!webView) {
        // Not created yet, the next time the panel opens will retry.
        Wh_Log(L"The panel's WebView wasn't found");
        return;
    }

    // The page is reloaded at times, and every load starts without the tweaks.
    g_navigationCompletedToken = webView.NavigationCompleted(
        [](wuxc::WebView const& sender, auto const&) {
            if (!g_unloading) {
                RunScript(sender, CurrentScript());
            }
        });
    g_webView = winrt::make_weak(webView);
    g_panelThreadId = GetCurrentThreadId();
    Wh_Log(L"Found the panel's WebView on thread %u", GetCurrentThreadId());

    // The page may already be loaded.
    RunScript(webView, CurrentScript());
}

void WINAPI ApplyScriptOnPanelThread(PVOID param) {
    if (auto webView = g_webView.get()) {
        RunScript(webView, *static_cast<std::wstring const*>(param));
    }
}

// Detaches from the WebView and runs the given script, which undoes the page
// tweaks.
void WINAPI ReleaseWebViewOnPanelThread(PVOID param) {
    if (auto webView = g_webView.get()) {
        try {
            webView.NavigationCompleted(g_navigationCompletedToken);
        } catch (...) {
            Wh_Log(L"NavigationCompleted revoke error %08X",
                   winrt::to_hresult());
        }
        RunScript(webView, *static_cast<std::wstring const*>(param));
    }
    g_webView = nullptr;
}

////////////////////////////////////////////////////////////////////////////////
// Panel size.
//
// ExpressiveSuggestionUIPage::ResetControlSize(Size) in InputApp.dll computes
// the panel's size and hands it to the view position and size manager. Its
// callers, TryShow and OnAvailableSizeChanged, pass Size::Empty, whose negative
// width means "use the default": 360 wide, and the title row (32) plus 368
// high. A non-negative size would be used as is. Either way the result is
// capped by the TextInputViewAvailableSize that the shell reports for the
// panel's position type, which is 360 x 418, so a bigger size needs that cap
// raised too. It is read through the IMapView that TextInput.dll implements,
// both by ResetControlSize and, later, when the shell places the panel.

// The size the last ResetControlSize asked for, or 0 when the panel keeps its
// default size.
std::atomic<float> g_requestedWidth = 0;
std::atomic<float> g_requestedHeight = 0;

// The TextInputViewPositionType the panel asks the limit for. It is read from
// the lookup ResetControlSize makes, so limits of other views in the process,
// such as the touch keyboard, stay as they are.
constexpr int kUnknownPositionType = -1;
std::atomic<int> g_panelPositionType = kUnknownPositionType;
thread_local bool t_inResetControlSize = false;

// The usable area of the monitor the panel opens on, in DIPs.
wf::Size GetWorkAreaSize() {
    HMONITOR monitor =
        MonitorFromWindow(GetForegroundWindow(), MONITOR_DEFAULTTONEAREST);
    MONITORINFO info = {sizeof(info)};
    if (!monitor || !GetMonitorInfo(monitor, &info)) {
        return {FLT_MAX, FLT_MAX};
    }

    UINT dpiX = 96, dpiY = 96;
    switch (GetAwarenessFromDpiAwarenessContext(GetThreadDpiAwarenessContext())) {
        case DPI_AWARENESS_PER_MONITOR_AWARE:
            if (FAILED(GetDpiForMonitor(monitor, MDT_EFFECTIVE_DPI, &dpiX,
                                        &dpiY))) {
                dpiX = dpiY = GetDpiForSystem();
            }
            break;
        case DPI_AWARENESS_SYSTEM_AWARE:
            dpiX = dpiY = GetDpiForSystem();
            break;
        default:
            break;
    }
    return {(info.rcWork.right - info.rcWork.left) * 96.0f / dpiX,
            (info.rcWork.bottom - info.rcWork.top) * 96.0f / dpiY};
}

using ResetControlSize_t = void(WINAPI*)(void* pThis, wf::Size size);
ResetControlSize_t ResetControlSize_Original;
void WINAPI ResetControlSize_Hook(void* pThis, wf::Size size) {
    int widthPercent = g_panelWidthPercent;
    int heightPercent = g_panelHeightPercent;
    Wh_Log(L"ResetControlSize %.1f x %.1f, scale %d%% x %d%%", size.Width,
           size.Height, widthPercent, heightPercent);
    bool resized = widthPercent != 100 || heightPercent != 100;
    if (resized) {
        if (size.Width < 0) {
            size = {kDefaultPanelWidth, kDefaultPanelHeight};
        }
        size.Width *= widthPercent / 100.0f;
        size.Height *= heightPercent / 100.0f;
    }

    g_requestedWidth = resized ? size.Width : 0;
    g_requestedHeight = resized ? size.Height : 0;
    t_inResetControlSize = true;
    ResetControlSize_Original(pThis, size);
    t_inResetControlSize = false;

    try {
        TrackPanelWebView(pThis);
    } catch (...) {
        Wh_Log(L"TrackPanelWebView error %08X", winrt::to_hresult());
    }
}

// TextInputViewAvailableSize starts with two Windows::Foundation::Size, both
// holding the available size.
struct AvailableSize {
    wf::Size size[2];
};

using AvailableSizeLookup_t = HRESULT(WINAPI*)(void* pThis,
                                               int key,
                                               AvailableSize* result);
AvailableSizeLookup_t AvailableSizeLookup_Original;
HRESULT WINAPI AvailableSizeLookup_Hook(void* pThis,
                                        int key,
                                        AvailableSize* result) {
    HRESULT hr = AvailableSizeLookup_Original(pThis, key, result);
    if (t_inResetControlSize) {
        g_panelPositionType = key;
    }
    if (FAILED(hr) || key != g_panelPositionType || g_requestedWidth <= 0 ||
        (g_panelWidthPercent == 100 && g_panelHeightPercent == 100)) {
        return hr;
    }
    // The layout of the struct comes from reverse engineering. Both sizes hold
    // the same limit, so leave the result alone if they don't.
    if (result->size[0].Width != result->size[1].Width ||
        result->size[0].Height != result->size[1].Height) {
        return hr;
    }

    wf::Size workArea = GetWorkAreaSize();
    float width = std::min(g_requestedWidth.load(), workArea.Width);
    float height = std::min(g_requestedHeight.load(), workArea.Height);
    for (auto& size : result->size) {
        size.Width = std::max(size.Width, width);
        size.Height = std::max(size.Height, height);
    }
    return hr;
}

bool HookTextInput(HMODULE module) {
    // clang-format off
    WindhawkUtils::SYMBOL_HOOK textInputDllHooks[] = {
        {
            {LR"(?__abi_Windows_Foundation_Collections_?$IMapView@W4TextInputViewPositionType@Common@TextInput@Experiences@ComposableShell@WindowsInternal@@VTextInputViewAvailableSize@23456@____abi_Lookup@?Q?$IMapView@W4TextInputViewPositionType@Common@TextInput@Experiences@ComposableShell@WindowsInternal@@VTextInputViewAvailableSize@23456@@Collections@Foundation@Windows@@?$MapView@W4TextInputViewPositionType@Common@TextInput@Experiences@ComposableShell@WindowsInternal@@VTextInputViewAvailableSize@23456@U?$less@W4TextInputViewPositionType@Common@TextInput@Experiences@ComposableShell@WindowsInternal@@@std@@$00$00@2Platform@@UE$AAAJW4TextInputViewPositionType@Common@TextInput@Experiences@ComposableShell@WindowsInternal@@PEAVTextInputViewAvailableSize@89Experiences@ComposableShell@WindowsInternal@@@Z)"},
            &AvailableSizeLookup_Original,
            AvailableSizeLookup_Hook,
        },
    };
    // clang-format on
    if (!WindhawkUtils::HookSymbols(module, textInputDllHooks,
                                    ARRAYSIZE(textInputDllHooks))) {
        Wh_Log(L"Couldn't hook TextInput.dll, the panel can't grow past its "
               L"default size");
        return false;
    }
    return true;
}

std::atomic<bool> g_inputAppHooked;

bool HookInputApp(HMODULE module) {
    // clang-format off
    WindhawkUtils::SYMBOL_HOOK inputAppDllHooks[] = {
        {
            {LR"(private: void __cdecl WindowsInternal::ComposableShell::Experiences::TextInputUndocked::InputApp::ExpressiveSuggestionUIPage::ResetControlSize(class Windows::Foundation::Size))"},
            &ResetControlSize_Original,
            ResetControlSize_Hook,
        },
    };
    // clang-format on
    if (!WindhawkUtils::HookSymbols(module, inputAppDllHooks,
                                    ARRAYSIZE(inputAppDllHooks))) {
        Wh_Log(L"Couldn't hook InputApp.dll, the panel can't be changed");
        return false;
    }
    return true;
}

bool IsModuleNamed(HMODULE module, PCWSTR moduleName) {
    WCHAR path[MAX_PATH];
    DWORD length = GetModuleFileName(module, path, ARRAYSIZE(path));
    if (!length || length == ARRAYSIZE(path)) {
        return false;
    }
    PCWSTR name = wcsrchr(path, L'\\');
    return name && _wcsicmp(name + 1, moduleName) == 0;
}

std::atomic<bool> g_textInputHooked;

// Hooks InputApp.dll once, from whichever path sees it loaded first. Hooks set
// after Wh_ModInit need to be applied.
void HookInputAppOnce(HMODULE module, bool applyHookOperations) {
    if (g_inputAppHooked.exchange(true)) {
        return;
    }
    if (HookInputApp(module) && applyHookOperations) {
        Wh_ApplyHookOperations();
    }
}

// The same for TextInput.dll, which loads on its own schedule.
void HookTextInputOnce(HMODULE module, bool applyHookOperations) {
    if (g_textInputHooked.exchange(true)) {
        return;
    }
    if (HookTextInput(module) && applyHookOperations) {
        Wh_ApplyHookOperations();
    }
}

// InputApp.dll and TextInput.dll are usually loaded after the mod, when the
// app activates.
using LoadLibraryExW_t = decltype(&LoadLibraryExW);
LoadLibraryExW_t LoadLibraryExW_Original;
HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR lpLibFileName,
                                   HANDLE hFile,
                                   DWORD dwFlags) {
    HMODULE module = LoadLibraryExW_Original(lpLibFileName, hFile, dwFlags);
    if (module && !g_inputAppHooked && IsModuleNamed(module, L"InputApp.dll")) {
        Wh_Log(L"InputApp.dll loaded");
        HookInputAppOnce(module, true);
    }
    if (module && !g_textInputHooked &&
        IsModuleNamed(module, L"TextInput.dll")) {
        Wh_Log(L"TextInput.dll loaded");
        HookTextInputOnce(module, true);
    }
    return module;
}

////////////////////////////////////////////////////////////////////////////////
// Running code on the panel's UI thread.

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

// Every XAML UI thread owns a message-only XAMLMessageWindowClass window, which
// is a reliable way to reach the thread.
HWND FindXamlMessageWindow(DWORD threadId) {
    HWND hWnd = nullptr;
    while ((hWnd = FindWindowEx(HWND_MESSAGE, hWnd, L"XAMLMessageWindowClass",
                                nullptr))) {
        if (GetWindowThreadProcessId(hWnd, nullptr) == threadId) {
            return hWnd;
        }
    }
    return nullptr;
}

// Runs proc on the panel's UI thread and waits for it, if the panel's WebView
// was found.
void RunOnPanelThread(RunFromWindowThreadProc_t proc, PVOID param) {
    DWORD threadId = g_panelThreadId;
    if (!threadId) {
        return;
    }
    HWND hWnd = FindXamlMessageWindow(threadId);
    if (!hWnd || !RunFromWindowThread(hWnd, proc, param)) {
        Wh_Log(L"Couldn't run on the panel's thread %u", threadId);
    }
}

////////////////////////////////////////////////////////////////////////////////

BOOL Wh_ModInit() {
    Wh_Log(L">");

    LoadSettings();

    HMODULE inputApp = GetModuleHandle(L"InputApp.dll");
    HMODULE textInput = GetModuleHandle(L"TextInput.dll");
    if (inputApp) {
        HookInputAppOnce(inputApp, false);
    }
    if (textInput) {
        HookTextInputOnce(textInput, false);
    }
    if (!inputApp || !textInput) {
        HMODULE kernelBase = GetModuleHandle(L"kernelbase.dll");
        auto pLoadLibraryExW = (LoadLibraryExW_t)GetProcAddress(
            kernelBase, "LoadLibraryExW");
        WindhawkUtils::SetFunctionHook(pLoadLibraryExW, LoadLibraryExW_Hook,
                                       &LoadLibraryExW_Original);
    }
    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L">");

    // The modules may have loaded after the checks in Wh_ModInit, but before
    // the LoadLibraryExW hook took effect.
    if (!g_inputAppHooked) {
        if (HMODULE inputApp = GetModuleHandle(L"InputApp.dll")) {
            HookInputAppOnce(inputApp, true);
        }
    }
    if (!g_textInputHooked) {
        if (HMODULE textInput = GetModuleHandle(L"TextInput.dll")) {
            HookTextInputOnce(textInput, true);
        }
    }
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");
    LoadSettings();
    std::wstring script = CurrentScript();
    RunOnPanelThread(ApplyScriptOnPanelThread, &script);
}

void Wh_ModBeforeUninit() {
    Wh_Log(L">");
    g_unloading = true;
}

void Wh_ModUninit() {
    Wh_Log(L">");
    g_unloading = true;

    // Blocks until the panel's thread has revoked the NavigationCompleted
    // handler, so no mod code is left attached to the WebView.
    std::wstring restoreScript = BuildScript(kRestorePageConfig);
    RunOnPanelThread(ReleaseWebViewOnPanelThread, &restoreScript);
}
