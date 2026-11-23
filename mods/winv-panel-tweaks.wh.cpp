// ==WindhawkMod==
// @id              winv-panel-tweaks
// @name            Win+V Panel Tweaks
// @description     Resize the Win+V clipboard / Win+. emoji panel, hide the tabs you don't use, and show only pinned clipboard items with a button or Win+Alt+V
// @version         1.1.0
// @author          jayll1303
// @github          https://github.com/jayll1303
// @license         MIT
// @include         TextInputHost.exe
// @include         windhawk.exe
// @compilerOptions -lole32 -loleaut32 -lruntimeobject -lshcore -lshell32
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

## Pinned items

A "Pinned" button next to "Clear all" shows only your pinned clipboard items,
so they're easy to reach even when they're far down the list. Click it again
to show everything.

Win+Alt+V opens the panel with only the pinned items shown; Win+V still opens
it with everything. The shortcut can be changed or turned off in the settings.
It has to be one that Windows and other programs don't already use (Win+Shift+V
and Win+Ctrl+V are taken, for example).

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
model, re-applying it whenever the page resets them. The same script adds the
Pinned button and hides the unpinned rows. Tweaks therefore show up the next
time the panel opens after the mod is enabled.

The pinned view shortcut is registered as a global hotkey from a small
windhawk.exe process the mod starts for it, because TextInputHost.exe isn't
allowed to register one. When it's pressed, the mod presses Win+V and the
panel that opens switches to the pinned view.

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
- pinnedHotkey: Win+Alt+V
  $name: Pinned view shortcut
  $description: >-
    Opens the Win+V panel showing only pinned clipboard items. Win, Ctrl, Alt
    and Shift joined with + to a letter, a digit or F1 to F24, e.g. Win+Alt+V
    or Ctrl+Alt+V, not already used by Windows or another program. Leave empty
    to turn the shortcut off.
*/
// ==/WindhawkModSettings==

#include <windows.h>

#include <windhawk_utils.h>

#include <shellscalingapi.h>

#include <algorithm>
#include <atomic>
#include <cfloat>
#include <cwctype>
#include <mutex>
#include <string>
#include <string_view>
#include <vector>

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
    L"{hidden:[],keepScale:false,contentWidth:0,contentHeight:0,"
    L"pinnedButton:false}";

// The pinned view shortcut, as MOD_* flags and a virtual-key code. A key of 0
// means the shortcut is off.
std::atomic<UINT> g_hotkeyModifiers;
std::atomic<UINT> g_hotkeyVk;

std::wstring TrimSpaces(std::wstring_view text) {
    size_t begin = text.find_first_not_of(L" \t");
    if (begin == text.npos) {
        return {};
    }
    size_t end = text.find_last_not_of(L" \t");
    return std::wstring(text.substr(begin, end - begin + 1));
}

// Parses e.g. "Win+Alt+V". At least one modifier is required, so that the
// shortcut can't take over plain typing.
bool ParseHotkey(std::wstring_view text, UINT* modifiers, UINT* vk) {
    *modifiers = 0;
    *vk = 0;
    while (!text.empty()) {
        size_t plus = text.find(L'+');
        std::wstring part = TrimSpaces(text.substr(0, plus));
        text = plus == text.npos ? std::wstring_view() : text.substr(plus + 1);
        if (part.empty() || *vk) {
            return false;
        }
        if (_wcsicmp(part.c_str(), L"Win") == 0 ||
            _wcsicmp(part.c_str(), L"Windows") == 0) {
            *modifiers |= MOD_WIN;
        } else if (_wcsicmp(part.c_str(), L"Ctrl") == 0 ||
                   _wcsicmp(part.c_str(), L"Control") == 0) {
            *modifiers |= MOD_CONTROL;
        } else if (_wcsicmp(part.c_str(), L"Alt") == 0) {
            *modifiers |= MOD_ALT;
        } else if (_wcsicmp(part.c_str(), L"Shift") == 0) {
            *modifiers |= MOD_SHIFT;
        } else if (part.size() == 1 && iswalnum(part[0]) && part[0] < 0x80) {
            *vk = towupper(part[0]);
        } else if ((part[0] == L'F' || part[0] == L'f') && part.size() <= 3) {
            int n = _wtoi(part.c_str() + 1);
            if (n < 1 || n > 24 || std::to_wstring(n) != part.substr(1)) {
                return false;
            }
            *vk = VK_F1 + n - 1;
        } else {
            return false;
        }
    }
    return *vk && *modifiers;
}

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

    std::wstring config =
        L"{hidden:[" + hidden + L"]," + sizeConfig + L",pinnedButton:true}";

    UINT modifiers = 0, vk = 0;
    auto hotkey = WindhawkUtils::StringSetting::make(L"pinnedHotkey");
    if (*hotkey.get() && !ParseHotkey(hotkey.get(), &modifiers, &vk)) {
        Wh_Log(L"Invalid pinned view shortcut: %s", hotkey.get());
    }
    Wh_Log(L"Pinned view shortcut: %s (modifiers %X, key %X)", hotkey.get(),
           modifiers, vk);
    g_hotkeyModifiers = modifiers;
    g_hotkeyVk = vk;

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
//
// Pinned button: a "Pinned" button next to "Clear all" in the clipboard list's
// header toggles S.pinnedOnly, which hides the unpinned items. The items stay
// in the page's view model (the page edits that list by index); their rows are
// hidden and lose their tabindex attributes, which is how the page's keyboard
// navigation finds what it can focus. A MutationObserver keeps this applied as
// the page re-renders, and a computed tracks the items' pinned state.
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

  var kClipboardHeader = 'clipboard-history-list-header-template';
  var kItemSelector = 'li[ux-test-class="expanded-view-list-item"]';
  var pinnedStyle = document.getElementById('winv-panel-tweaks-pinned-style');
  function each(selector, root, f) { Array.prototype.forEach.call((root || document).querySelectorAll(selector), f); }

  function hideItem(li) {
    if (!li.hasAttribute('data-winv-unpinned')) li.setAttribute('data-winv-unpinned', '');
    [li].concat(Array.prototype.slice.call(li.querySelectorAll('[tabindex]'))).forEach(function (e) {
      if (!e.hasAttribute('tabindex')) return;
      e.setAttribute('data-winv-tabindex', e.getAttribute('tabindex'));
      e.removeAttribute('tabindex');
    });
  }
  function showItem(li) {
    if (li.hasAttribute('data-winv-unpinned')) li.removeAttribute('data-winv-unpinned');
    [li].concat(Array.prototype.slice.call(li.querySelectorAll('[data-winv-tabindex]'))).forEach(function (e) {
      if (!e.hasAttribute('data-winv-tabindex')) return;
      if (!e.hasAttribute('tabindex')) e.setAttribute('tabindex', e.getAttribute('data-winv-tabindex'));
      e.removeAttribute('data-winv-tabindex');
    });
  }
  function clearPinned() {
    each('.winv-pinned-action,.winv-pinned-empty', null, function (e) { e.parentNode.removeChild(e); });
    each('li[data-winv-unpinned],li [data-winv-tabindex]', null, function (e) { showItem(e.closest(kItemSelector) || e); });
    if (pinnedStyle) { pinnedStyle.parentNode.removeChild(pinnedStyle); pinnedStyle = null; }
  }

  function installPinned() {
    if (!pinnedStyle) {
      pinnedStyle = document.createElement('style');
      pinnedStyle.id = 'winv-panel-tweaks-pinned-style';
      pinnedStyle.textContent = [
        'li[data-winv-unpinned]{display:none!important}',
        '.winv-pinned-action{margin-left:auto;margin-right:calc(var(--expanded-view-scale-ratio) * 0.25rem)}',
        'body[dir=rtl] .winv-pinned-action{margin-right:auto;margin-left:calc(var(--expanded-view-scale-ratio) * 0.25rem)}',
        '.category-header .category-header-action-content.winv-pinned-on{background:var(--system-accent-color)!important;color:var(--text-on-accent-color)!important}',
        '@media (-ms-high-contrast:active){.category-header .category-header-action-content.winv-pinned-on{background:highlight!important;color:highlighttext!important}}',
        '.winv-pinned-empty{flex-basis:100%;opacity:.7;font-size:calc(12px * var(--expanded-view-scale-ratio));padding:calc(var(--expanded-view-scale-ratio) * 0.5rem) 0}'
      ].join('\n');
      (document.head || document.documentElement).appendChild(pinnedStyle);
    }

    var list = null, pinWatch = null, timer = 0;
    function schedule() { if (!timer) timer = setTimeout(function () { timer = 0; pass(); }, 0); }
    function watch(newList) {
      if (newList === list) return;
      if (pinWatch) pinWatch.dispose();
      list = newList;
      pinWatch = null;
      if (list && ko.isObservable(list.items)) {
        pinWatch = ko.computed(function () {
          (list.items() || []).forEach(function (item) { ko.unwrap(item.isPinned); });
          schedule();
        });
      }
    }
    function isPinned(item) { return !!ko.unwrap(item && item.isPinned); }

    function updateHeader(header, evl) {
      var action = header.querySelector('.winv-pinned-action');
      if (!action) {
        action = document.createElement('div');
        action.className = 'category-header-action winv-pinned-action';
        var button = document.createElement('button');
        button.className = 'category-header-action-content js-exclude-default-focus js-exclude-up-down-focus';
        button.setAttribute('tabindex', '0');
        button.setAttribute('data-custom-tabindex', '3');
        var text = document.createElement('div');
        text.className = 'category-header-action-content-text';
        text.textContent = 'Pinned';
        button.appendChild(text);
        button.addEventListener('click', function (e) {
          e.stopPropagation();
          var s = window.__winvPanelTweaks;
          if (s && s.pinnedPass) { s.pinnedOnly = !s.pinnedOnly; s.pinnedPass(); }
        });
        action.appendChild(button);
        // Right after the header text, outside the page's Knockout blocks.
        var label = header.querySelector('.category-header-text');
        header.insertBefore(action, label ? label.nextSibling : header.firstChild);
      }
      var b = action.firstChild;
      if (b.classList.contains('winv-pinned-on') !== !!S.pinnedOnly) b.classList.toggle('winv-pinned-on');
      if (b.getAttribute('aria-pressed') !== String(!!S.pinnedOnly)) b.setAttribute('aria-pressed', String(!!S.pinnedOnly));

      var empty = header.querySelector('.winv-pinned-empty');
      var showEmpty = S.pinnedOnly && !(ko.unwrap(evl.items) || []).some(isPinned);
      if (showEmpty && !empty) {
        empty = document.createElement('div');
        empty.className = 'winv-pinned-empty';
        empty.textContent = 'No pinned items';
        header.appendChild(empty);
      } else if (!showEmpty && empty) {
        empty.parentNode.removeChild(empty);
      }
    }

    function pass() {
      var found = null;
      each('.category-header-container', null, function (header) {
        // The lists are rendered with foreach ... as: 'expandedViewList', so
        // the list is a context variable (and the data itself).
        var context = ko.contextFor(header);
        var evl = context && (context.expandedViewList || context.$data);
        var headerConfig = evl && evl.config && evl.config.header;
        if (!headerConfig || ko.unwrap(headerConfig.template) !== kClipboardHeader) return;
        found = evl;
        updateHeader(header, evl);
      });
      watch(found);

      var active = document.activeElement, focusLost = false, firstShown = null;
      each(kItemSelector, null, function (li) {
        var item = ko.dataFor(li);
        if (!item || !ko.isObservable(item.isPinned)) return;
        if (S.pinnedOnly && !item.isPinned.peek()) {
          if (active && li.contains(active)) focusLost = true;
          hideItem(li);
        } else {
          showItem(li);
          if (!firstShown) firstShown = li;
        }
      });
      // Opened in the pinned view, or the focused row was just hidden: keep the
      // focus on a row Enter can paste, as the panel does when it opens.
      if (S.pinnedOnly && firstShown && (focusLost || S.focusPinned)) {
        S.focusPinned = false;
        firstShown.focus();
      }
    }

    var observer = new MutationObserver(schedule);
    observer.observe(document.body, { childList: true, subtree: true, attributes: true, attributeFilter: ['tabindex'] });
    S.pinnedPass = schedule;
    S.subs.push({ dispose: function () {
      observer.disconnect();
      if (pinWatch) pinWatch.dispose();
      if (timer) clearTimeout(timer);
      S.pinnedPass = null;
    } });
    schedule();
  }

  function install() {
    if (typeof ko === 'undefined') return false;
    var container = document.getElementById('navigation-menu-container');
    var root = container && ko.dataFor(container);
    var nav = root && root.navigationMenuViewModel;
    if (!nav || !nav.navigationMenuItems) return false;

    hookSizeInfo(root.viewDimensionsProvider);
    if (config.pinnedButton) {
      installPinned();
    } else {
      clearPinned();
    }

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
  if (!hidden.length && !config.keepScale && !config.pinnedButton) {
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

// Sets the view the panel opens in: only pinned items after the pinned view
// shortcut, everything otherwise. Run each time the panel opens, possibly
// before the main script has installed (it reads the same state).
std::wstring PinnedViewScript(bool pinnedOnly) {
    return std::wstring(LR"JS((function (on) {
  var S = window.__winvPanelTweaks || (window.__winvPanelTweaks = { subs: [], changed: {} });
  S.pinnedOnly = on;
  S.focusPinned = on;
  if (S.pinnedPass) S.pinnedPass();
})()JS") +
           (pinnedOnly ? L"true" : L"false") + L")";
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

// When the pinned view shortcut last opened the panel (GetTickCount64), or 0.
// The shortcut lives in the mod's tool process and the panel in
// TextInputHost.exe, so the time is kept in a small named shared memory block
// that both open.
HANDLE g_pinnedViewRequestMapping;
LONG64 volatile* g_pinnedViewRequestTime;

void OpenPinnedViewRequest() {
    g_pinnedViewRequestMapping = CreateFileMapping(
        INVALID_HANDLE_VALUE, nullptr, PAGE_READWRITE, 0, sizeof(LONG64),
        L"Local\\" WH_MOD_ID L"_PinnedViewRequest");
    if (!g_pinnedViewRequestMapping) {
        Wh_Log(L"CreateFileMapping error %u", GetLastError());
        return;
    }
    g_pinnedViewRequestTime = static_cast<LONG64 volatile*>(
        MapViewOfFile(g_pinnedViewRequestMapping, FILE_MAP_ALL_ACCESS, 0, 0,
                      sizeof(LONG64)));
    if (!g_pinnedViewRequestTime) {
        Wh_Log(L"MapViewOfFile error %u", GetLastError());
    }
}

void ClosePinnedViewRequest() {
    if (g_pinnedViewRequestTime) {
        UnmapViewOfFile(const_cast<LONG64*>(g_pinnedViewRequestTime));
        g_pinnedViewRequestTime = nullptr;
    }
    if (g_pinnedViewRequestMapping) {
        CloseHandle(g_pinnedViewRequestMapping);
        g_pinnedViewRequestMapping = nullptr;
    }
}

void SetPinnedViewRequest() {
    if (g_pinnedViewRequestTime) {
        InterlockedExchange64(g_pinnedViewRequestTime, GetTickCount64());
    }
}

// True if the panel that is opening was opened by the pinned view shortcut.
// The window is short so that a request the panel never acted on (the
// synthesized Win+V can be dropped, e.g. when an elevated window is in front)
// doesn't apply to a later, unrelated opening.
bool ConsumePinnedViewRequest() {
    constexpr ULONGLONG kMaxDelayMs = 500;
    if (!g_pinnedViewRequestTime) {
        return false;
    }
    ULONGLONG requestTime = InterlockedExchange64(g_pinnedViewRequestTime, 0);
    return requestTime && GetTickCount64() - requestTime < kMaxDelayMs;
}

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

// TryShow runs each time the panel opens (ResetControlSize also runs when the
// available size changes), so the view is set only here: a size change while
// the panel is open keeps the view the user chose.
using TryShow_t = void(WINAPI*)(void* pThis);
TryShow_t TryShow_Original;
void WINAPI TryShow_Hook(void* pThis) {
    TryShow_Original(pThis);

    try {
        if (auto webView = g_webView.get(); webView && !g_unloading) {
            RunScript(webView, PinnedViewScript(ConsumePinnedViewRequest()));
        }
    } catch (...) {
        Wh_Log(L"TryShow error %08X", winrt::to_hresult());
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
        {
            {LR"(private: void __cdecl WindowsInternal::ComposableShell::Experiences::TextInputUndocked::InputApp::ExpressiveSuggestionUIPage::TryShow(void))"},
            &TryShow_Original,
            TryShow_Hook,
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
// The pinned view shortcut.
//
// TextInputHost.exe can't have a global shortcut: RegisterHotKey fails there
// with ERROR_ACCESS_DENIED, and a low-level keyboard hook installs but is
// never called. So the shortcut runs in the mod's own tool process (a
// windhawk.exe started for this mod, see the end of the file), where it is
// registered with RegisterHotKey on a thread of its own.
//
// Pressing it records the time in the shared pinned view request and presses
// Win+V for the user, which opens the panel as usual; the next TryShow in
// TextInputHost.exe then switches the page to the pinned view. A shortcut that
// another program or the shell has already registered can't be used, and the
// log says so.

constexpr int kHotkeyId = 1;
constexpr UINT kReloadHotkeyMsg = WM_APP + 1;

std::mutex g_hotkeyThreadMutex;
HANDLE g_hotkeyThread;
DWORD g_hotkeyThreadId;

bool IsKeyDown(int vk) {
    return GetAsyncKeyState(vk) < 0;
}

void AddKey(std::vector<INPUT>& inputs, WORD vk, bool up) {
    INPUT input = {INPUT_KEYBOARD};
    input.ki.wVk = vk;
    input.ki.dwFlags = up ? KEYEVENTF_KEYUP : 0;
    if (vk == VK_LWIN || vk == VK_RWIN || vk == VK_RCONTROL || vk == VK_RMENU) {
        input.ki.dwFlags |= KEYEVENTF_EXTENDEDKEY;
    }
    inputs.push_back(input);
}

// An unassigned virtual-key code, pressed before Alt is released so that the
// foreground app doesn't take the release as a lone Alt press and open its
// menu bar (the shortcut's own key was taken by RegisterHotKey).
constexpr WORD kMaskKey = 0xE8;

// Presses Win+V for the user: the shortcut's other modifiers are released
// first, and Win is held only if the user isn't holding it already.
void OpenPinnedView() {
    std::vector<INPUT> inputs;
    if (IsKeyDown(VK_MENU)) {
        AddKey(inputs, kMaskKey, false);
        AddKey(inputs, kMaskKey, true);
    }
    for (WORD vk : {VK_LSHIFT, VK_RSHIFT, VK_LCONTROL, VK_RCONTROL, VK_LMENU,
                    VK_RMENU}) {
        if (IsKeyDown(vk)) {
            AddKey(inputs, vk, true);
        }
    }
    bool winDown = IsKeyDown(VK_LWIN) || IsKeyDown(VK_RWIN);
    if (!winDown) {
        AddKey(inputs, VK_LWIN, false);
    }
    AddKey(inputs, 'V', false);
    AddKey(inputs, 'V', true);
    if (!winDown) {
        AddKey(inputs, VK_LWIN, true);
    }

    SetPinnedViewRequest();
    if (SendInput((UINT)inputs.size(), inputs.data(), sizeof(INPUT)) !=
        inputs.size()) {
        Wh_Log(L"SendInput error %u", GetLastError());
    }
}

// Registers the shortcut from the current settings, replacing the previous
// one. Runs on the shortcut thread, which receives its WM_HOTKEY.
void RegisterPinnedViewHotkey() {
    UnregisterHotKey(nullptr, kHotkeyId);
    UINT vk = g_hotkeyVk;
    if (!vk) {
        return;
    }
    if (RegisterHotKey(nullptr, kHotkeyId, g_hotkeyModifiers | MOD_NOREPEAT,
                       vk)) {
        Wh_Log(L"Pinned view shortcut registered");
    } else {
        DWORD error = GetLastError();
        Wh_Log(L"Couldn't register the pinned view shortcut, error %u%s", error,
               error == ERROR_HOTKEY_ALREADY_REGISTERED
                   ? L" (already used by another program or by Windows)"
                   : L"");
    }
}

DWORD WINAPI HotkeyThreadProc(PVOID readyEvent) {
    // Creates the thread's message queue before the thread is reported ready,
    // so that messages posted to it aren't lost.
    MSG msg;
    PeekMessage(&msg, nullptr, WM_USER, WM_USER, PM_NOREMOVE);
    RegisterPinnedViewHotkey();
    SetEvent(static_cast<HANDLE>(readyEvent));

    while (GetMessage(&msg, nullptr, 0, 0) > 0) {
        if (msg.message == WM_HOTKEY && msg.wParam == kHotkeyId) {
            Wh_Log(L"Pinned view shortcut pressed");
            OpenPinnedView();
        } else if (msg.message == kReloadHotkeyMsg) {
            RegisterPinnedViewHotkey();
        }
    }

    UnregisterHotKey(nullptr, kHotkeyId);
    return 0;
}

// Starts the shortcut thread once, when this Explorer turns out to own the
// taskbar.
void StartHotkeyThread() {
    std::lock_guard lock(g_hotkeyThreadMutex);
    if (g_hotkeyThread || g_unloading) {
        return;
    }
    HANDLE readyEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
    if (!readyEvent) {
        return;
    }
    g_hotkeyThread = CreateThread(nullptr, 0, HotkeyThreadProc, readyEvent, 0,
                                  &g_hotkeyThreadId);
    if (g_hotkeyThread) {
        WaitForSingleObject(readyEvent, INFINITE);
    }
    CloseHandle(readyEvent);
}

void ReloadHotkey() {
    std::lock_guard lock(g_hotkeyThreadMutex);
    if (g_hotkeyThread) {
        PostThreadMessage(g_hotkeyThreadId, kReloadHotkeyMsg, 0, 0);
    }
}

void StopHotkeyThread() {
    std::lock_guard lock(g_hotkeyThreadMutex);
    if (!g_hotkeyThread) {
        return;
    }
    PostThreadMessage(g_hotkeyThreadId, WM_QUIT, 0, 0);
    WaitForSingleObject(g_hotkeyThread, INFINITE);
    CloseHandle(g_hotkeyThread);
    g_hotkeyThread = nullptr;
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
        // WINAPI is needed for the 32-bit build.
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

// The mod runs in two processes: TextInputHost.exe, which draws the panel and
// gets everything else, and its own tool process, which owns the pinned view
// shortcut.
bool g_isPanelProcess;

BOOL PanelModInit() {
    Wh_Log(L">");

    LoadSettings();
    OpenPinnedViewRequest();

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

void PanelModAfterInit() {
    Wh_Log(L">");

    // The modules may have loaded after the checks in PanelModInit, but before
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

void PanelModSettingsChanged() {
    Wh_Log(L">");
    LoadSettings();
    std::wstring script = CurrentScript();
    RunOnPanelThread(ApplyScriptOnPanelThread, &script);
}

void PanelModUninit() {
    Wh_Log(L">");
    g_unloading = true;

    // Blocks until the panel's thread has revoked the NavigationCompleted
    // handler, so no mod code is left attached to the WebView.
    std::wstring restoreScript = BuildScript(kRestorePageConfig);
    RunOnPanelThread(ReleaseWebViewOnPanelThread, &restoreScript);
    ClosePinnedViewRequest();
}

void Wh_ModBeforeUninit() {
    Wh_Log(L">");
    g_unloading = true;
}

// The tool process: its hotkey thread's message loop keeps it alive.
BOOL WhTool_ModInit() {
    Wh_Log(L">");
    LoadSettings();
    OpenPinnedViewRequest();
    StartHotkeyThread();
    return TRUE;
}

void WhTool_ModSettingsChanged() {
    Wh_Log(L">");
    LoadSettings();
    ReloadHotkey();
}

void WhTool_ModUninit() {
    Wh_Log(L">");
    g_unloading = true;
    StopHotkeyThread();
    ClosePinnedViewRequest();
}

////////////////////////////////////////////////////////////////////////////////
// Windhawk tool mod implementation for mods which don't need to inject to other
// processes or hook other functions. Context:
// https://github.com/ramensoftware/windhawk/wiki/Mods-as-tools:-Running-mods-in-a-dedicated-process
//
// The mod will load and run in a dedicated windhawk.exe process.
//
// Paste the code below as part of the mod code, and use these callbacks:
// * WhTool_ModInit
// * WhTool_ModSettingsChanged
// * WhTool_ModUninit
//
// Currently, other callbacks are not supported.

bool g_isToolModProcessLauncher;
HANDLE g_toolModProcessMutex;

void WINAPI EntryPoint_Hook() {
    Wh_Log(L">");
    ExitThread(0);
}

BOOL Wh_ModInit() {
    // TextInputHost.exe part.
    if (IsModuleNamed(nullptr, L"TextInputHost.exe")) {
        g_isPanelProcess = true;
        return PanelModInit();
    }

    DWORD sessionId;
    if (ProcessIdToSessionId(GetCurrentProcessId(), &sessionId) &&
        sessionId == 0) {
        return FALSE;
    }

    bool isExcluded = false;
    bool isToolModProcess = false;
    bool isCurrentToolModProcess = false;
    int argc;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLine(), &argc);
    if (!argv) {
        Wh_Log(L"CommandLineToArgvW failed");
        return FALSE;
    }

    for (int i = 1; i < argc; i++) {
        if (wcscmp(argv[i], L"-service") == 0 ||
            wcscmp(argv[i], L"-service-start") == 0 ||
            wcscmp(argv[i], L"-service-stop") == 0) {
            isExcluded = true;
            break;
        }
    }

    for (int i = 1; i < argc - 1; i++) {
        if (wcscmp(argv[i], L"-tool-mod") == 0) {
            isToolModProcess = true;
            if (wcscmp(argv[i + 1], WH_MOD_ID) == 0) {
                isCurrentToolModProcess = true;
            }
            break;
        }
    }

    LocalFree(argv);

    if (isExcluded) {
        return FALSE;
    }

    if (isCurrentToolModProcess) {
        g_toolModProcessMutex =
            CreateMutex(nullptr, TRUE, L"windhawk-tool-mod_" WH_MOD_ID);
        if (!g_toolModProcessMutex) {
            Wh_Log(L"CreateMutex failed");
            ExitProcess(1);
        }

        if (GetLastError() == ERROR_ALREADY_EXISTS) {
            Wh_Log(L"Tool mod already running (%s)", WH_MOD_ID);
            ExitProcess(1);
        }

        if (!WhTool_ModInit()) {
            ExitProcess(1);
        }

        IMAGE_DOS_HEADER* dosHeader =
            (IMAGE_DOS_HEADER*)GetModuleHandle(nullptr);
        IMAGE_NT_HEADERS* ntHeaders =
            (IMAGE_NT_HEADERS*)((BYTE*)dosHeader + dosHeader->e_lfanew);

        DWORD entryPointRVA = ntHeaders->OptionalHeader.AddressOfEntryPoint;
        void* entryPoint = (BYTE*)dosHeader + entryPointRVA;

        Wh_SetFunctionHook(entryPoint, (void*)EntryPoint_Hook, nullptr);
        return TRUE;
    }

    if (isToolModProcess) {
        return FALSE;
    }

    g_isToolModProcessLauncher = true;
    return TRUE;
}

void Wh_ModAfterInit() {
    // TextInputHost.exe part.
    if (g_isPanelProcess) {
        PanelModAfterInit();
        return;
    }

    if (!g_isToolModProcessLauncher) {
        return;
    }

    WCHAR currentProcessPath[MAX_PATH];
    switch (GetModuleFileName(nullptr, currentProcessPath,
                              ARRAYSIZE(currentProcessPath))) {
        case 0:
        case ARRAYSIZE(currentProcessPath):
            Wh_Log(L"GetModuleFileName failed");
            return;
    }

    WCHAR
    commandLine[MAX_PATH + 2 +
                (sizeof(L" -tool-mod \"" WH_MOD_ID "\"") / sizeof(WCHAR)) - 1];
    swprintf_s(commandLine, L"\"%s\" -tool-mod \"%s\"", currentProcessPath,
               WH_MOD_ID);

    HMODULE kernelModule = GetModuleHandle(L"kernelbase.dll");
    if (!kernelModule) {
        kernelModule = GetModuleHandle(L"kernel32.dll");
        if (!kernelModule) {
            Wh_Log(L"No kernelbase.dll/kernel32.dll");
            return;
        }
    }

    using CreateProcessInternalW_t = BOOL(WINAPI*)(
        HANDLE hUserToken, LPCWSTR lpApplicationName, LPWSTR lpCommandLine,
        LPSECURITY_ATTRIBUTES lpProcessAttributes,
        LPSECURITY_ATTRIBUTES lpThreadAttributes, WINBOOL bInheritHandles,
        DWORD dwCreationFlags, LPVOID lpEnvironment, LPCWSTR lpCurrentDirectory,
        LPSTARTUPINFOW lpStartupInfo,
        LPPROCESS_INFORMATION lpProcessInformation,
        PHANDLE hRestrictedUserToken);
    CreateProcessInternalW_t pCreateProcessInternalW =
        (CreateProcessInternalW_t)GetProcAddress(kernelModule,
                                                 "CreateProcessInternalW");
    if (!pCreateProcessInternalW) {
        Wh_Log(L"No CreateProcessInternalW");
        return;
    }

    STARTUPINFO si{
        .cb = sizeof(STARTUPINFO),
        .dwFlags = STARTF_FORCEOFFFEEDBACK,
    };
    PROCESS_INFORMATION pi;
    if (!pCreateProcessInternalW(nullptr, currentProcessPath, commandLine,
                                 nullptr, nullptr, FALSE, NORMAL_PRIORITY_CLASS,
                                 nullptr, nullptr, &si, &pi, nullptr)) {
        Wh_Log(L"CreateProcess failed");
        return;
    }

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}

void Wh_ModSettingsChanged() {
    // TextInputHost.exe part.
    if (g_isPanelProcess) {
        PanelModSettingsChanged();
        return;
    }

    if (g_isToolModProcessLauncher) {
        return;
    }

    WhTool_ModSettingsChanged();
}

void Wh_ModUninit() {
    // TextInputHost.exe part.
    if (g_isPanelProcess) {
        PanelModUninit();
        return;
    }

    if (g_isToolModProcessLauncher) {
        return;
    }

    WhTool_ModUninit();
    ExitProcess(0);
}
