// ==WindhawkMod==
// @id              brave-acrylic
// @name            Brave Acrylic
// @description     Alpha: Native Brave background transparency; requires Translucent Windows
// @version         3.5
// @author          bardelyne
// @github          https://github.com/bardelyne
// @homepage        https://github.com/bardelyne/brave-acrylic
// @license         MIT
// @architecture    x86-64
// @include         brave.exe
// @compilerOptions -lshell32 -luser32 -lwinhttp -lbcrypt -lcrypt32 -lole32 -loleaut32 -lruntimeobject
// ==/WindhawkMod==
// ==WindhawkModReadme==
/*
# Brave Acrylic

![Brave Acrylic](https://raw.githubusercontent.com/bardelyne/brave-acrylic/main/assets/brave-acrylic-v3.3.png)

Reveal the Windows backdrop through Brave's vertical tab area, frame, toolbar, address field, and webpage area. Native background targeting replaces the old color list. Use a website-styling extension for backgrounds painted by websites.

> **Alpha software.** **[Translucent Windows](https://windhawk.net/mods/translucent-windows) is required**, configured for `brave.exe`. Brave Acrylic exposes the background; Translucent Windows supplies the effect.

## Requirements

- x64 version of Brave.
- Windhawk with Brave Acrylic enabled.
- Translucent Windows configured to apply a translucent effect to `brave.exe`.
- Windows Transparency effects enabled.

## Compatibility

Windows 11 is the tested environment. Windows 10 is unverified; the backdrop provider and selected effect must support that Windows version. Native ARM64 Brave is unsupported.

Tested with Brave 1.98.60 / Chromium 156. Other builds need valid compatibility data for their exact `chrome.dll`. Native interfaces and rendering behavior can change and may require a mod update.

## Fast setup

1. Enable Translucent Windows for `brave.exe` and choose the desired backdrop.
2. Enable Brave Acrylic. Leave **Vertical tabs background**, **Toolbar**, and **Webpage background** on.
3. Close all Brave windows normally and reopen Brave. Exit its background instance too if it stays running.
4. Optionally use Zen Internet or Stylus to remove backgrounds inside websites, as explained below. Stylus requires custom CSS.

No theme-color setup is required. Old saved color-list and website-code settings are ignored by this version.

## Controls

| Setting | Default | Purpose |
| --- | --- | --- |
| **Vertical tabs background** | On | Reveal the backdrop behind the vertical tab area and browser frame. |
| **Toolbar** | On | Clear the toolbar, bookmarks bar, and unfocused address-field backgrounds. Focused address fields keep their native theme. |
| **Webpage background** | On | Clear only Chromium's background behind the webpage area, including its default white fallback. Website HTML, CSS, and content are unchanged. Use Zen Internet or Stylus for website backgrounds; Stylus requires custom CSS. |
| **Hide duplicate Windows buttons** | Off | Optional workaround that removes the real Windows system-menu style. It can affect Alt+Space, taskbar menus and other window tools. Prefer Brave's **Use compact mode** option in Appearance. |
| **Use verified online symbol cache** | On | Download and verify exact-build data against the approved GitHub pipeline before accepting any Windhawk symbol addresses. Turn off for previously verified builds only. |

Some backing views and compositor properties are created with the window, so changing Vertical tabs background or Toolbar may require reopening Brave. Enabling Webpage background applies when a tab is selected, without refreshing the page. Reopening Brave after disabling it restores existing renderer defaults. The caption-button option applies immediately.

## Website backgrounds with extensions

**Webpage background** clears only Chromium's backing layer underneath websites, including the default white fallback. It does not modify website HTML, CSS, or content. To remove backgrounds painted by a website, use a styling extension such as Zen Internet or Stylus.

### Zen Internet

For backgrounds inside websites, install [Zen Internet from the Chrome Web Store](https://chromewebstore.google.com/detail/zen-internet/dpkamhkboipomecencjpammmjdfkgjha). Open the extension, go to **Advanced Settings**, and turn **Disable Transparency** **off** under **Global Feature Controls**. Keep **Webpage background** enabled in Brave Acrylic. Open a new tab or reload an existing page to apply extension setting changes.

Zen Internet applies website-specific styles; coverage and results vary by site. Brave Acrylic removes the browser's backing and white fallback so CSS-transparent page areas can reveal the Windows backdrop. The extension handles website styling. Brave Acrylic does not inject JavaScript or CSS into pages.

The extension's store page cautions that ordinary Chromium does not support transparent pages. Brave Acrylic provides the native transparency path; an extension alone cannot provide the Windows effect. The extension's [source](https://github.com/sameerasw/zeninternet) and [website styles](https://github.com/sameerasw/my-internet) are available separately.

### Stylus

[Stylus from the Chrome Web Store](https://chromewebstore.google.com/detail/stylus/clngdbkpkpeebahjckkjfobafhncgmne) lets you create and manage custom CSS styles for individual websites. **It does not remove website backgrounds out of the box.** You must write or paste CSS rules that make the desired backgrounds transparent, or install a suitable existing userstyle. Complex websites may need rules for several background elements. Keep **Webpage background** enabled in Brave Acrylic so those transparent areas reveal the backdrop.

See the [Stylus guide to writing styles](https://github.com/openstyles/stylus/wiki/Writing-styles) for creating a style and limiting it to a particular website.

## How it works

Brave's existing transparent-host path enables its compositor to preserve transparency. The mod removes the actual vertical-tab backing through Brave's own setter and clears selected native background roles within their paint clip. Child controls and text retain their original painting. Native focus checks preserve the focused address field.

The webpage option clears the native backing during content-view updates and sets Chromium's persistent page-base color when a tab is attached. That value is also used by future renderers, so switching or restoring tabs does not bring the default white background back. Explicit website backgrounds remain visible unless a website-styling extension changes them.

The native path uses **13 functions**, of which **7 are private hooks**. The implementation uses no sampled colors, private object-member offsets, guessed screen rectangles, screenshot polling, extra backing windows, or GPU hooks. Normal DirectComposition stays enabled, with no special browser launch flag. Windows supplies one backdrop. Acrylic can turn gray when inactive; choose a suitable blur mode in Translucent Windows if you prefer a persistent effect.

## Symbol resolution, privacy and updates

Brave Acrylic still resolves the thirteen full C++ prototypes with Windhawk's supported `HookSymbols` / `WH_HOOK_SYMBOLS_OPTIONS::onlineCacheUrl` mechanism. Before accepting any returned address, the mod requires compatibility data authenticated against its approved generation pipeline. No build-specific addresses are embedded and remote PDB downloads remain disabled.

The small text cache and a proof locator are published on this repository's [`symbol-cache` branch](https://github.com/bardelyne/brave-acrylic/tree/symbol-cache). Branch contents alone are not trusted. On first use of a build, the mod checks GitHub's HTTPS API for the immutable artifact's SHA256, exact PDB identity, original repository, successful run and GitHub-hosted job attempts. It fetches the workflow at that run's immutable commit and checks its complete bytes against a SHA256 compiled into the mod. The approved workflow checks out fixed extractor and action revisions. Every Windhawk-resolved address must then match the verified named entry and pass local executable-boundary, leaf, thunk and overlap checks.

Editing the cache branch, replacing the locator, changing the workflow, or changing tools on main cannot silently authorize different functions. A pipeline change requires a reviewed mod update approving the new policy. This trusts GitHub's API/TLS and hosted infrastructure plus the approved extractor and official Brave inputs; it is not a proof that a native ABI can never change. The data is author-hosted, not the official Windhawk cache, and maintainer acceptance remains a separate decision. See the [trust design](https://github.com/bardelyne/brave-acrylic/blob/main/docs/trusted-pipeline.md).

Successfully verified data is retained in Windhawk's per-mod storage, scoped to the exact PDB identity and approved workflow. Warm launches reuse it offline. Old Windhawk entries without this proof cannot bypass the gate. If proof, rate limits, networking or function validation prevent a first-time verification, transparency stays off rather than using unverified addresses. There is no polling or retry thread in the mod.

**No telemetry, uploads or login:** requests are download-only GETs to GitHub. No token, browser cookies, history, page content, settings, account details or analytics are sent. GitHub necessarily sees the requester's IP and requested URLs, including the build identity. Turn **Use verified online symbol cache** off and re-enable the mod for offline-only operation; a build must have been verified previously. Initial verification adds several small API requests at startup and can be affected by GitHub's anonymous rate limits. Existing requests cannot be withdrawn by changing the setting.

The workflow requests a check **every two hours, with a backup 30 minutes later**. GitHub can delay or drop scheduled starts, so this is not an exact timer. Unchanged releases skip all browser and PDB downloads. Large official files stay on the GitHub runner. The raw exact-PDB artifact is retained for 90 days; local verified entries continue working if upstream metadata later becomes unavailable. Re-enable the mod or reopen Brave normally after new data becomes available.

## Duplicate corner buttons

If two sets of minimize/maximize/close buttons appear, first enable **Use compact mode** in Brave **Settings → Appearance**. The optional **Hide duplicate Windows buttons** workaround is off on a fresh install because it removes the window's actual `WS_SYSMENU` style. Windows system menus, taskbar interactions, accessibility tools or other window managers may be affected. Previously saved preferences are preserved. Disabling the option or unloading the mod restores its recorded style changes.

## Troubleshooting

Reopen Brave normally after installation or native-area changes. Enable Windhawk debug logging to check **13/13 native functions** or a Windhawk symbol-cache message. For website styling, check Zen Internet's transparency setting and whether that site has a supported style, or check the CSS and site scope of your Stylus style.

For reports, include Windows/Brave versions, window type, enabled areas, backdrop configuration, and reproduction steps. See the [advanced reference](https://github.com/bardelyne/brave-acrylic/blob/main/docs/advanced.md), [compatibility data](https://github.com/bardelyne/brave-acrylic/blob/main/docs/compatibility.md), [webview investigation](https://github.com/bardelyne/brave-acrylic/blob/main/docs/webview-background.md), and [changelog](https://github.com/bardelyne/brave-acrylic/blob/main/CHANGELOG.md).
*/
// ==/WindhawkModReadme==
// ==WindhawkModSettings==
/*
- transparentBackground: true
  $name: Vertical tabs background
  $description: Reveal the backdrop behind the vertical tab area and browser frame.
- transparentToolbar: true
  $name: Toolbar
  $description: Reveal the backdrop behind the toolbar, bookmarks bar and unfocused address field.
- transparentPageBackground: true
  $name: Webpage background
  $description: Clear only Chromium's background behind the webpage area, including its default white fallback. Website HTML, CSS and content are unchanged. Use Zen Internet or Stylus for website backgrounds; Stylus requires custom CSS.
- hideNativeCaptionButtons: false
  $name: Hide duplicate Windows buttons
  $description: "Optional workaround: removes the real Windows system-menu style and can affect Alt+Space, taskbar menus, accessibility tools and window managers. Prefer Brave Appearance > Use compact mode. Applies immediately."
- useOnlineSymbolCache: true
  $name: Use verified online symbol cache
  $description: Download exact-build symbols and verify GitHub artifact, run and approved workflow metadata. No login, uploads, telemetry or PDB downloads. GitHub sees your IP and requested URLs. Turn off and re-enable for offline-only operation with previously verified data. Missing proof disables transparency.
*/
// ==/WindhawkModSettings==
#include <windhawk_api.h>
#ifndef _M_X64
BOOL Wh_ModInit() {
    Wh_Log(L"Brave Acrylic requires x64 Brave.");
    return FALSE;
}
#else
#include <shellapi.h>
#include <atomic>
#include <cstdio>
#include <cwchar>
#include <cstring>
#include <windhawk_utils.h>
#include <winhttp.h>
#include <bcrypt.h>
#include <wincrypt.h>
#include <roapi.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Data.Json.h>
#include <algorithm>
#include <array>
#include <cmath>
#include <functional>
#include <stdexcept>
#include <vector>


// Native background roles, MIT, bardelyne.
// Native ABI: x64 Brave 1.98.60 / Chromium 156. Resolve and validate every
// function for the exact installed DLL before publishing ready=true.

#include <cstddef>
#include <string>

namespace BraveBackground {
struct ClassName { const char* data; size_t length; };
using Paint = void (*)(void*, void*);
using Clear = void (*)(void*, unsigned int, int);
using GetClass = ClassName* (*)(const void*, ClassName*);
using Transparent = bool (*)(const void*);
using Construct = void* (*)(void*, void*, void*);
// The native nontrivial unique_ptr parameter is passed indirectly on x64.
// Only an empty native unique_ptr is supplied; Brave destroys its old object.
using SetBackground = void (*)(void*, void**);
enum Area : unsigned { Background = 1, Toolbar = 2, Page = 4, All = Background | Toolbar | Page };

struct Context {
    std::atomic<bool> ready{false};
    std::atomic<unsigned> areas{All};
    Paint originalPaint = nullptr;
    Clear clear = nullptr;
    GetClass getClass = nullptr;
    Transparent originalTransparent = nullptr;
    Construct originalConstruct = nullptr;
    SetBackground setBackground = nullptr;

    bool Complete() const {
        return originalPaint && clear && getClass && originalTransparent &&
               originalConstruct && setBackground;
    }
};

inline bool IsBackgroundContainer(ClassName name, unsigned areas) {
    if (!name.data || !name.length || name.length >= 128 || !areas) return false;
    struct Role { const char* name; unsigned areas; };
    static constexpr Role roles[] = {
        // A transparent shared root is required for either child area. An
        // unselected child still paints its own original opaque background.
        {"BrowserView", All}, {"BraveBrowserView", All},
        {"TopContainerView", Toolbar}, {"ToolbarView", Toolbar},
        {"BraveToolbarView", Toolbar}, {"BookmarkBarView", Toolbar},
        {"BraveBookmarkBarView", Toolbar},
        {"VerticalTabStripRegionView", Background},
        {"BraveVerticalTabStripRegionView", Background},
        {"VerticalTabStripContainerView", Background},
        {"BraveVerticalTabStripContainerView", Background}
    };
    for (const auto& role : roles) {
        const size_t length = std::strlen(role.name);
        if ((areas & role.areas) && length == name.length &&
            !std::memcmp(name.data, role.name, length)) return true;
    }
    return false;
}

inline void PaintBackground(Context& context, void* view, void* canvas) {
    if (view && canvas && context.ready.load(std::memory_order_acquire)) {
        ClassName name{};
        context.getClass(view, &name);
        if (IsBackgroundContainer(name, context.areas.load(std::memory_order_relaxed)) &&
            context.ready.load(std::memory_order_acquire)) {
            // The caller's native clip covers only this background. Borders,
            // foreground content, and children paint afterward as usual.
            context.clear(canvas, 0, 0 /* SkBlendMode::kClear */);
            return;
        }
    }
    if (context.originalPaint) context.originalPaint(view, canvas);
}

inline bool WindowContentsTransparent(Context& context, const void* host) {
    if (context.ready.load(std::memory_order_acquire) &&
        context.areas.load(std::memory_order_relaxed)) return true;
    return context.originalTransparent ? context.originalTransparent(host) : false;
}

inline void* ConstructSidebar(Context& context, void* self, void* browser,
                              void* backingView) {
    void* result = context.originalConstruct(self, browser, backingView);
    if (backingView && context.ready.load(std::memory_order_acquire) &&
        (context.areas.load(std::memory_order_relaxed) & Background)) {
        // Brave passes this otherwise anonymous View to the constructor. No
        // object-layout offsets, geometry guesses, or retained View pointers.
        void* empty = nullptr;
        context.setBackground(backingView, &empty);
    }
    return result;
}
}  // namespace BraveBackground

static BraveBackground::Context s_background;
static auto& s_transparencyReady = s_background.ready;
static constexpr unsigned kFunctionSlots = 13;
static std::atomic<bool> s_chromeAttempted{false}, s_useOnlineSymbolCache{true};
static SRWLOCK s_initLock = SRWLOCK_INIT;
static bool s_stopping = false, s_activateAfterInit = false, s_isBrowserProcess = false;
using LoadLibraryExW_t = decltype(&LoadLibraryExW);
static LoadLibraryExW_t pOrigLoadLibraryExW = nullptr;

// Native x64 SkColor4f is passed indirectly as a 16-byte aggregate.
struct NativeColor { float r, g, b, a; };
static_assert(sizeof(NativeColor) == 16);
using NativeFocus = bool (*)(const void*);
using NativeEvent = void (*)(void*);
using NativeLayerColor = void (*)(void*, const NativeColor*);
using NativeSetPageBase = void (*)(void*, uint64_t);
using NativeAttachContents = void (*)(void*, void*);
static NativeFocus s_locationFocus = nullptr, s_viewFocus = nullptr;
static NativeEvent s_originalTheme = nullptr, s_originalReady = nullptr;
static NativeLayerColor s_originalLayerColor = nullptr;
static NativeSetPageBase s_setPageBase = nullptr;
static NativeAttachContents s_originalAttachContents = nullptr;
static thread_local unsigned s_pageBackgroundScope = 0;

// Limit both color overrides to native content-view updates on this thread.
struct PageScope {
    unsigned old = s_pageBackgroundScope;
    PageScope() {
        if (s_transparencyReady.load(std::memory_order_acquire) &&
            (s_background.areas.load(std::memory_order_relaxed) & BraveBackground::Page))
            ++s_pageBackgroundScope;
    }
    ~PageScope() { s_pageBackgroundScope = old; }
};
static bool ClearPageFallback() {
    return s_pageBackgroundScope && s_transparencyReady.load(std::memory_order_acquire) &&
        (s_background.areas.load(std::memory_order_relaxed) & BraveBackground::Page);
}
static void PageThemeHook(void* self) { PageScope scope; s_originalTheme(self); }
static void PageReadyHook(void* self) { PageScope scope; s_originalReady(self); }
static void PageLayerColorHook(void* self, const NativeColor* color) {
    const NativeColor transparent{};
    s_originalLayerColor(self, ClearPageFallback() ? &transparent : color);
}
// The native optional<SkColor> argument occupies RDX: color in the low
// DWORD, has-value in byte 4. Set an explicit transparent base, not nullopt.
// Chromium stores it on WebContents and sends it to current and future renderers.
static void AttachContentsHook(void* self, void* contents) {
    if (contents && s_transparencyReady.load(std::memory_order_acquire) &&
        (s_background.areas.load(std::memory_order_relaxed) & BraveBackground::Page)) {
        s_setPageBase(contents, uint64_t(1) << 32);
    }
    // Use the supplied canonical WebContents pointer before attach notifications.
    // No retained objects, member offsets, reloads or selection polling.
    s_originalAttachContents(self, contents);
}
static bool AddressShouldClear(void* view) {
    if (!view || !s_transparencyReady.load(std::memory_order_acquire) ||
        !(s_background.areas.load(std::memory_order_relaxed) & BraveBackground::Toolbar)) return false;
    BraveBackground::ClassName name{};
    s_background.getClass(view, &name);
    if (!name.data || name.length > 127) return false;
    auto is = [&](const char* target) {
        return strlen(target) == name.length && !memcmp(name.data, target, name.length);
    };
    const bool location = is("LocationBarView") || is("BraveLocationBarView");
    const bool input = is("OmniboxViewViews") || is("BraveOmniboxViewViews");
    if (!location && !input) return false;
    return !(location ? s_locationFocus(view) : s_viewFocus(view));
}

static bool HasCompleteTransparencyPath() {
    return s_background.Complete() && s_locationFocus && s_viewFocus &&
        s_originalTheme && s_originalReady && s_originalLayerColor &&
        s_setPageBase && s_originalAttachContents;
}
static void ViewOnPaintBackground_Hook(void* view, void* canvas) {
    if (canvas && AddressShouldClear(view)) {
        s_background.clear(canvas, 0, 0);
        return;
    }
    BraveBackground::PaintBackground(s_background, view, canvas);
}
static bool WindowContentsTransparent_Hook(const void* host) {
    return BraveBackground::WindowContentsTransparent(s_background, host);
}
static void* SidebarConstructor_Hook(void* self, void* browser, void* host) {
    return BraveBackground::ConstructSidebar(s_background, self, browser, host);
}

// Windows still draws its caption buttons underneath Chromium's transparent
// custom frame. Hide only WS_SYSMENU, retaining the resize/caption/min/max bits.
// Chromium reads its original logical style, including when saving fullscreen
// state; DWM sees the real style without the native buttons.
static constexpr wchar_t kNativeCaptionProperty[] =
    L"Windhawk.BraveAcrylic.NativeCaption";
static std::atomic<bool> s_hideNativeCaptionButtons{false};
static std::atomic<bool> s_captionFixActive{false};
static std::atomic<bool> s_captionStopping{false};
static bool s_captionHooksInstalled = false;
static decltype(&CreateWindowExW) pOrigCreateWindowExW = nullptr;
static decltype(&GetWindowLongPtrW) pOrigGetWindowLongPtrW = nullptr;
static decltype(&SetWindowLongPtrW) pOrigSetWindowLongPtrW = nullptr;
static decltype(&GetWindowLongW) pOrigGetWindowLongW = nullptr;
static decltype(&SetWindowLongW) pOrigSetWindowLongW = nullptr;
static thread_local HWND s_captionStyleWrite = nullptr;

static bool IsBrowserCaptionFrame(HWND window, LONG_PTR style) {
    constexpr LONG_PTR required = WS_CAPTION | WS_THICKFRAME |
                                 WS_MINIMIZEBOX | WS_MAXIMIZEBOX;
    if (!window || (style & required) != required || (style & WS_CHILD)) return false;
    DWORD process = 0;
    GetWindowThreadProcessId(window, &process);
    if (process != GetCurrentProcessId() || GetAncestor(window, GA_ROOT) != window ||
        GetWindow(window, GW_OWNER)) return false;
    const auto exStyle = pOrigGetWindowLongPtrW(window, GWL_EXSTYLE);
    if (exStyle & (WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE | WS_EX_DLGMODALFRAME)) return false;
    wchar_t name[32]{};
    return GetClassNameW(window, name, ARRAYSIZE(name)) &&
           !wcscmp(name, L"Chrome_WidgetWin_1");
}

static void RefreshNativeCaption(HWND window) {
    SetWindowPos(window, nullptr, 0, 0, 0, 0,
                 SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE |
                 SWP_FRAMECHANGED);
}

static void RestoreNativeCaption(HWND window) {
    if (!GetPropW(window, kNativeCaptionProperty)) return;
    RemovePropW(window, kNativeCaptionProperty);
    // Restore before hooks are removed; the ownership property is already gone.
    const auto style = GetWindowLongPtrW(window, GWL_STYLE);
    SetWindowLongPtrW(window, GWL_STYLE, style | WS_SYSMENU);
    RefreshNativeCaption(window);
}

template<typename Value>
static Value ReadCaptionStyle(HWND window, int index, Value (WINAPI* original)(HWND, int)) {
    auto value = original(window, index);
    const DWORD error = GetLastError();
    if (index == GWL_STYLE && GetPropW(window, kNativeCaptionProperty)) value |= WS_SYSMENU;
    SetLastError(error);
    return value;
}

static LONG_PTR WINAPI GetWindowLongPtrW_Hook(HWND window, int index) {
    return ReadCaptionStyle(window, index, pOrigGetWindowLongPtrW);
}

static LONG WINAPI GetWindowLongW_Hook(HWND window, int index) {
    return ReadCaptionStyle(window, index, pOrigGetWindowLongW);
}

template<typename Value>
static Value WriteCaptionStyle(HWND window, int index, Value value,
                               Value (WINAPI* original)(HWND, int, Value)) {
    // Windows may forward one API to the other. Translate the style only once.
    if (index != GWL_STYLE || s_captionStyleWrite == window) return original(window, index, value);
    const bool owned = GetPropW(window, kNativeCaptionProperty) != nullptr;
    const bool hide = s_captionFixActive.load(std::memory_order_acquire) &&
        (value & WS_SYSMENU) && (owned || IsBrowserCaptionFrame(window, value));
    bool marked = owned;
    if (hide && !owned) {
        marked = SetPropW(window, kNativeCaptionProperty, reinterpret_cast<HANDLE>(1)) != FALSE;
    } else if (!hide && owned) {
        RemovePropW(window, kNativeCaptionProperty);
        marked = false;
    }
    const HWND previousWrite = s_captionStyleWrite;
    s_captionStyleWrite = window;
    // Zero is also a valid previous style. Clear stale errors before testing
    // whether the native setter failed, as required by SetWindowLong.
    SetLastError(ERROR_SUCCESS);
    const auto previous = original(window, index,
                                   hide && marked ? value & ~static_cast<Value>(WS_SYSMENU) : value);
    const DWORD error = GetLastError();
    s_captionStyleWrite = previousWrite;
    if (!previous && error) {
        if (owned) SetPropW(window, kNativeCaptionProperty, reinterpret_cast<HANDLE>(1));
        else RemovePropW(window, kNativeCaptionProperty);
    } else if (hide && marked && !s_captionFixActive.load(std::memory_order_acquire)) {
        // A setting can change while SetWindowLong dispatches style messages.
        RestoreNativeCaption(window);
    }
    SetLastError(error);
    return previous && owned ? previous | WS_SYSMENU : previous;
}

static LONG_PTR WINAPI SetWindowLongPtrW_Hook(HWND window, int index, LONG_PTR value) {
    return WriteCaptionStyle(window, index, value, pOrigSetWindowLongPtrW);
}

static LONG WINAPI SetWindowLongW_Hook(HWND window, int index, LONG value) {
    return WriteCaptionStyle(window, index, value, pOrigSetWindowLongW);
}

static void ApplyNativeCaption(HWND window) {
    if (!s_captionFixActive.load(std::memory_order_acquire) ||
        GetPropW(window, kNativeCaptionProperty)) return;
    const auto style = pOrigGetWindowLongPtrW(window, GWL_STYLE);
    if (!(style & WS_SYSMENU) || !IsBrowserCaptionFrame(window, style)) return;
    SetWindowLongPtrW_Hook(window, GWL_STYLE, style);
    if (GetPropW(window, kNativeCaptionProperty)) RefreshNativeCaption(window);
}

static HWND WINAPI CreateWindowExW_Hook(DWORD exStyle, LPCWSTR className, LPCWSTR name,
                                       DWORD style, int x, int y, int width, int height,
                                       HWND parent, HMENU menu, HINSTANCE instance, LPVOID param) {
    const auto window = pOrigCreateWindowExW(exStyle, className, name, style,
                                            x, y, width, height, parent, menu, instance, param);
    const DWORD error = GetLastError();
    if (window) ApplyNativeCaption(window);
    SetLastError(error);
    return window;
}

static BOOL CALLBACK SyncNativeCaptionWindow(HWND window, LPARAM) {
    DWORD process = 0;
    GetWindowThreadProcessId(window, &process);
    if (process == GetCurrentProcessId()) {
        if (s_captionFixActive.load(std::memory_order_acquire)) ApplyNativeCaption(window);
        else RestoreNativeCaption(window);
    }
    return TRUE;
}

static void SyncNativeCaptions() {
    const bool active = s_captionHooksInstalled &&
        !s_captionStopping.load(std::memory_order_acquire) &&
        s_transparencyReady.load(std::memory_order_acquire) &&
        s_background.areas.load(std::memory_order_relaxed) &&
        s_hideNativeCaptionButtons.load(std::memory_order_relaxed);
    s_captionFixActive.store(active, std::memory_order_release);
    if (s_captionHooksInstalled) EnumWindows(SyncNativeCaptionWindow, 0);
}

static void InstallNativeCaptionHooks() {
    s_captionHooksInstalled =
        WindhawkUtils::SetFunctionHook(CreateWindowExW, CreateWindowExW_Hook, &pOrigCreateWindowExW) &&
        WindhawkUtils::SetFunctionHook(GetWindowLongPtrW, GetWindowLongPtrW_Hook, &pOrigGetWindowLongPtrW) &&
        WindhawkUtils::SetFunctionHook(SetWindowLongPtrW, SetWindowLongPtrW_Hook, &pOrigSetWindowLongPtrW) &&
        WindhawkUtils::SetFunctionHook(GetWindowLongW, GetWindowLongW_Hook, &pOrigGetWindowLongW) &&
        WindhawkUtils::SetFunctionHook(SetWindowLongW, SetWindowLongW_Hook, &pOrigSetWindowLongW);
    if (!s_captionHooksInstalled) Wh_Log(L"Native caption fix unavailable; acrylic is unchanged");
}

static void LoadModSettings() {
    unsigned areas = 0;
    if (Wh_GetIntSetting(L"transparentBackground")) areas |= BraveBackground::Background;
    if (Wh_GetIntSetting(L"transparentToolbar")) areas |= BraveBackground::Toolbar;
    if (Wh_GetIntSetting(L"transparentPageBackground")) areas |= BraveBackground::Page;
    s_background.areas.store(areas, std::memory_order_relaxed);
    s_hideNativeCaptionButtons.store(Wh_GetIntSetting(L"hideNativeCaptionButtons") != 0,
                                    std::memory_order_relaxed);
    s_useOnlineSymbolCache.store(Wh_GetIntSetting(L"useOnlineSymbolCache") != 0,
                                  std::memory_order_release);
}

struct ReadOnlyImage {
    const unsigned char* base;
    size_t size;
    const IMAGE_SECTION_HEADER* sections;
    unsigned count;

    bool Contains(size_t rva, size_t length) const {
        if (rva > size || length > size - rva) return false;
        for (unsigned i = 0; i < count; ++i) {
            const auto& s = sections[i];
            const DWORD flags = s.Characteristics;
            if (!(flags & IMAGE_SCN_MEM_READ) ||
                !(flags & IMAGE_SCN_CNT_INITIALIZED_DATA) ||
                (flags & (IMAGE_SCN_MEM_WRITE | IMAGE_SCN_MEM_EXECUTE)) ||
                s.VirtualAddress > size || s.Misc.VirtualSize > size - s.VirtualAddress)
                continue;
            if (rva >= s.VirtualAddress && rva - s.VirtualAddress <= s.Misc.VirtualSize &&
                length <= s.Misc.VirtualSize - (rva - s.VirtualAddress)) return true;
        }
        return false;
    }
};

static bool GetReadOnlyImage(HMODULE mod, ReadOnlyImage* out) {
    MEMORY_BASIC_INFORMATION memory{};
    if (!mod || !VirtualQuery(mod, &memory, sizeof(memory)) ||
        memory.State != MEM_COMMIT || (memory.Protect & (PAGE_GUARD | PAGE_NOACCESS)))
        return false;
    const auto* base = reinterpret_cast<const unsigned char*>(mod);
    const size_t headerBytes = memory.RegionSize -
        (base - static_cast<const unsigned char*>(memory.BaseAddress));
    if (headerBytes < sizeof(IMAGE_DOS_HEADER)) return false;
    const auto* dos = reinterpret_cast<const IMAGE_DOS_HEADER*>(base);
    if (dos->e_magic != IMAGE_DOS_SIGNATURE || dos->e_lfanew < 0 ||
        static_cast<size_t>(dos->e_lfanew) > headerBytes ||
        sizeof(IMAGE_NT_HEADERS64) > headerBytes - dos->e_lfanew) return false;
    const auto* nt = reinterpret_cast<const IMAGE_NT_HEADERS64*>(base + dos->e_lfanew);
    if (nt->Signature != IMAGE_NT_SIGNATURE ||
        nt->OptionalHeader.Magic != IMAGE_NT_OPTIONAL_HDR64_MAGIC ||
        nt->FileHeader.SizeOfOptionalHeader < sizeof(IMAGE_OPTIONAL_HEADER64) ||
        !nt->FileHeader.NumberOfSections || nt->FileHeader.NumberOfSections > 96)
        return false;
    const auto* sections = IMAGE_FIRST_SECTION(nt);
    const size_t sectionOffset = reinterpret_cast<const unsigned char*>(sections) - base;
    const size_t sectionBytes = nt->FileHeader.NumberOfSections * sizeof(*sections);
    const size_t imageSize = nt->OptionalHeader.SizeOfImage;
    if (sectionOffset > headerBytes || sectionBytes > headerBytes - sectionOffset ||
        sectionOffset > imageSize || sectionBytes > imageSize - sectionOffset) return false;
    *out = {base, imageSize, sections, nt->FileHeader.NumberOfSections};
    return true;
}

static bool GetFunctionTable(HMODULE mod, const RUNTIME_FUNCTION** table, size_t* count) {
    const auto* dos = reinterpret_cast<const IMAGE_DOS_HEADER*>(mod);
    const auto* nt = reinterpret_cast<const IMAGE_NT_HEADERS64*>(
        reinterpret_cast<const unsigned char*>(mod) + dos->e_lfanew);
    const auto& dir = nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXCEPTION];
    const size_t imageSize = nt->OptionalHeader.SizeOfImage;
    if (!dir.VirtualAddress || dir.VirtualAddress > imageSize ||
        dir.Size > imageSize - dir.VirtualAddress ||
        !dir.Size || dir.Size % sizeof(RUNTIME_FUNCTION)) return false;
    *table = reinterpret_cast<const RUNTIME_FUNCTION*>(
        reinterpret_cast<const unsigned char*>(mod) + dir.VirtualAddress);
    *count = dir.Size / sizeof(RUNTIME_FUNCTION);
    return true;
}

static bool FunctionContains(HMODULE mod, size_t rva, size_t length) {
    const RUNTIME_FUNCTION* table;
    size_t count;
    if (!GetFunctionTable(mod, &table, &count)) return false;
    size_t lo = 0, hi = count;
    while (lo < hi) {
        const size_t mid = lo + (hi - lo) / 2;
        if (table[mid].BeginAddress < rva) lo = mid + 1;
        else hi = mid;
    }
    return lo < count && table[lo].BeginAddress == rva &&
           table[lo].EndAddress > rva && length <= table[lo].EndAddress - rva;
}

// Local validation description for the thirteen resolved native functions.
// No build-specific address table is embedded or loaded from a binary map.
struct FunctionFingerprint {
    DWORD rva, length, kind, reserved;
    unsigned char code[32], mask[32];
};
struct CompatibilityMap {
    DWORD magic, format;
    GUID guid;
    DWORD age, machine, timestamp, imageSize;
    FunctionFingerprint functions[kFunctionSlots];
};
static_assert(sizeof(FunctionFingerprint) == 80);
static_assert(sizeof(CompatibilityMap) == 1080);
struct BuildIdentity { GUID guid; DWORD age, machine, timestamp, imageSize; };

static bool GetBuildIdentity(HMODULE module, BuildIdentity* identity) {
    ReadOnlyImage image{};
    if (!GetReadOnlyImage(module, &image)) return false;
    const auto* dos = reinterpret_cast<const IMAGE_DOS_HEADER*>(image.base);
    const auto* nt = reinterpret_cast<const IMAGE_NT_HEADERS64*>(image.base + dos->e_lfanew);
    if (nt->FileHeader.Machine != IMAGE_FILE_MACHINE_AMD64 ||
        nt->OptionalHeader.NumberOfRvaAndSizes <= IMAGE_DIRECTORY_ENTRY_DEBUG) return false;
    const auto& dir = nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_DEBUG];
    if (!dir.Size || dir.Size % sizeof(IMAGE_DEBUG_DIRECTORY) ||
        !image.Contains(dir.VirtualAddress, dir.Size)) return false;
    bool found = false;
    BuildIdentity result{};
    for (size_t p = 0; p < dir.Size; p += sizeof(IMAGE_DEBUG_DIRECTORY)) {
        IMAGE_DEBUG_DIRECTORY debug{};
        memcpy(&debug, image.base + dir.VirtualAddress + p, sizeof(debug));
        if (debug.Type != IMAGE_DEBUG_TYPE_CODEVIEW) continue;
        if (debug.SizeOfData < 24 || debug.SizeOfData > 4096 ||
            !image.Contains(debug.AddressOfRawData, debug.SizeOfData)) return false;
        const auto* cv = image.base + debug.AddressOfRawData;
        if (memcmp(cv, "RSDS", 4)) continue;
        const char* name = reinterpret_cast<const char*>(cv + 24);
        const char* end = static_cast<const char*>(memchr(name, 0, debug.SizeOfData - 24));
        if (!end) return false;
        const char* leaf = name;
        for (const char* at = name; at < end; ++at) if (*at == '\\' || *at == '/') leaf = at + 1;
        if (strcmp(leaf, "chrome.dll.pdb")) return false;
        BuildIdentity candidate{};
        memcpy(&candidate.guid, cv + 4, sizeof(GUID));
        memcpy(&candidate.age, cv + 20, sizeof(DWORD));
        const GUID empty{};
        if (!candidate.age || !memcmp(&candidate.guid, &empty, sizeof(GUID))) return false;
        if (found && (memcmp(&candidate.guid, &result.guid, sizeof(GUID)) ||
                      candidate.age != result.age)) return false;
        result = candidate;
        found = true;
    }
    if (!found) return false;
    result.machine = nt->FileHeader.Machine;
    result.timestamp = nt->FileHeader.TimeDateStamp;
    result.imageSize = nt->OptionalHeader.SizeOfImage;
    *identity = result;
    return true;
}

static bool FocusLeafLayout(const unsigned char* code) {
    const unsigned char layout[] = {
        0x48, 0x83, 0xB9, 0, 0, 0, 0, 0, 0x74, 0x13, 0x48, 0x8B, 0x81,
        0, 0, 0, 0, 0x48, 0x8B, 0x40, 0x28, 0x83, 0x78, 0x18, 0,
        0x0F, 0x95, 0xC0, 0xC3, 0x31, 0xC0, 0xC3
    };
    for (unsigned i = 0; i < ARRAYSIZE(layout); ++i) {
        const bool memberOffset = (i >= 3 && i < 7) || (i >= 13 && i < 17);
        if (!memberOffset && code[i] != layout[i]) return false;
    }
    return true;
}
static bool LeafOutsideUnwind(HMODULE module, size_t rva, size_t length) {
    const RUNTIME_FUNCTION* table;
    size_t count;
    if (!GetFunctionTable(module, &table, &count)) return false;
    // Inspect all preceding ranges, including optimized overlapping entries.
    // A nearest-predecessor lookup can miss an enclosing earlier function.
    for (size_t i = 0; i < count; ++i) {
        if (table[i].BeginAddress >= rva + length) break;
        if (table[i].BeginAddress >= rva || table[i].EndAddress > rva) return false;
    }
    return true;
}
static bool ValidateCompatibilityMap(HMODULE module, const CompatibilityMap& map, size_t* rva) {
    BuildIdentity id{};
    if (map.magic != 0x504D4142 || map.format != 6 || !GetBuildIdentity(module, &id) ||
        memcmp(&map.guid, &id.guid, sizeof(GUID)) || map.age != id.age ||
        map.machine != id.machine || map.timestamp != id.timestamp ||
        map.imageSize != id.imageSize) return false;
    ReadOnlyImage image{};
    if (!GetReadOnlyImage(module, &image)) return false;
    const IMAGE_SECTION_HEADER* text = nullptr;
    for (unsigned i = 0; i < image.count; ++i) {
        const auto& section = image.sections[i];
        if (!memcmp(section.Name, ".text\0\0\0", 8) &&
            (section.Characteristics & IMAGE_SCN_MEM_EXECUTE)) {
            if (text) return false;
            text = &section;
        }
    }
    if (!text || text->VirtualAddress > image.size ||
        text->Misc.VirtualSize > image.size - text->VirtualAddress) return false;
    for (unsigned i = 0; i < kFunctionSlots; ++i) {
        const auto& f = map.functions[i];
        const DWORD sourceLength = f.kind == 3 ? 5 : f.length;
        if (!f.rva || (f.kind != 3 && f.reserved) || f.kind > 3 || f.length > 32 ||
            f.rva < text->VirtualAddress ||
            f.rva - text->VirtualAddress > text->Misc.VirtualSize ||
            sourceLength > text->Misc.VirtualSize - (f.rva - text->VirtualAddress)) return false;
        const DWORD target = f.kind == 3 ? f.reserved : f.rva;
        if (target < text->VirtualAddress ||
            target - text->VirtualAddress > text->Misc.VirtualSize ||
            f.length > text->Misc.VirtualSize - (target - text->VirtualAddress)) return false;
        const unsigned char hostPrefix[] = {0x48, 0x83, 0xB9};
        const unsigned char hostSuffix[] = {0, 0x74, 3, 0x31, 0xC0, 0xC3};
        const bool shortHost = i == 3 && f.length == 13 && (f.kind == 1 || f.kind == 3);
        const bool shortFocus = i == 6 && f.length == 32 &&
            (f.kind == 2 || (f.kind == 3 && FocusLeafLayout(f.code)));
        if (f.kind == 0 || f.kind == 3) {
            if (shortHost) {
                if (memcmp(f.code, hostPrefix, 3) || memcmp(f.code + 7, hostSuffix, 6) ||
                    !LeafOutsideUnwind(module, target, f.length)) return false;
            } else if (shortFocus) {
                if (!FunctionContains(module, target, f.length) &&
                    !LeafOutsideUnwind(module, target, f.length)) return false;
            } else if (f.length < 16 || !FunctionContains(module, target, f.length)) {
                return false;
            }
            if (f.kind == 3) {
                if (f.rva - text->VirtualAddress > text->Misc.VirtualSize - 5 ||
                    image.base[f.rva] != 0xE9 || !LeafOutsideUnwind(module, f.rva, 5)) return false;
                int displacement;
                memcpy(&displacement, image.base + f.rva + 1, sizeof(displacement));
                if (int64_t(f.rva) + 5 + displacement != target) return false;
            }
        } else if (f.kind == 1) {
            if (!shortHost || memcmp(f.code, hostPrefix, 3) ||
                memcmp(f.code + 7, hostSuffix, 6) ||
                !LeafOutsideUnwind(module, target, f.length)) return false;
        } else if (!shortFocus || !FocusLeafLayout(f.code) ||
                   !LeafOutsideUnwind(module, target, f.length)) {
            return false;
        }
        unsigned fixed = 0;
        for (unsigned j = 0; j < 32; ++j) {
            if (f.mask[j] != 0 && f.mask[j] != 0xFF) return false;
            if (j >= f.length) {
                if (f.mask[j] || f.code[j]) return false;
            } else if (f.mask[j]) {
                if (image.base[target + j] != f.code[j]) return false;
                ++fixed;
            }
        }
        if (fixed < (shortHost ? 13u : shortFocus ? 32u : 16u)) return false;
        // A named jump stub and its body are separate protected ranges.
        // Reject overlap with either range, including the stub's own body.
        auto overlaps = [](DWORD a, DWORD n, DWORD b, DWORD m) {
            return uint64_t(a) < uint64_t(b) + m && uint64_t(b) < uint64_t(a) + n;
        };
        if (f.kind == 3 && overlaps(f.rva, 5, target, f.length)) return false;
        for (unsigned j = 0; j < i; ++j) {
            const auto& other = map.functions[j];
            if (!other.rva) continue;
            const DWORD otherTarget = other.kind == 3 ? other.reserved : other.rva;
            if (overlaps(target, f.length, otherTarget, other.length) ||
                (f.kind == 3 && overlaps(f.rva, 5, otherTarget, other.length)) ||
                (other.kind == 3 && overlaps(target, f.length, other.rva, 5)) ||
                (f.kind == 3 && other.kind == 3 && overlaps(f.rva, 5, other.rva, 5))) return false;
        }
    }
    // Never publish a partial address set on a rejected map.
    for (unsigned i = 0; i < kFunctionSlots; ++i) {
        rva[i] = map.functions[i].kind == 3 ? map.functions[i].reserved : map.functions[i].rva;
    }
    return true;
}
// Windhawk retains standard named-symbol resolution and its own local cache.
// The separate gate authenticates downloaded bytes and compares every result.
// No executable updates, retry worker, telemetry, or remote PDB fallback.
static constexpr wchar_t kOnlineSymbolCache[] =
    L"https://raw.githubusercontent.com/bardelyne/brave-acrylic/symbol-cache/brave-acrylic";

static bool ValidateSymbolAddresses(HMODULE module, void* const* addresses, size_t* rva) {
    BuildIdentity id{};
    ReadOnlyImage image{};
    if (!GetBuildIdentity(module, &id) || !GetReadOnlyImage(module, &image)) return false;
    const IMAGE_SECTION_HEADER* text = nullptr;
    for (unsigned i = 0; i < image.count; ++i) {
        const auto& section = image.sections[i];
        if (!memcmp(section.Name, ".text\0\0\0", 8) &&
            (section.Characteristics & IMAGE_SCN_MEM_EXECUTE)) {
            if (text) return false;
            text = &section;
        }
    }
    if (!text || text->VirtualAddress > image.size ||
        text->Misc.VirtualSize > image.size - text->VirtualAddress) return false;
    const auto inText = [&](size_t start, size_t length) {
        return start >= text->VirtualAddress &&
            start - text->VirtualAddress <= text->Misc.VirtualSize &&
            length <= text->Misc.VirtualSize - (start - text->VirtualAddress);
    };
    CompatibilityMap local{};
    local.magic = 0x504D4142;
    local.format = 6;
    local.guid = id.guid;
    local.age = id.age;
    local.machine = id.machine;
    local.timestamp = id.timestamp;
    local.imageSize = id.imageSize;
    for (unsigned i = 0; i < kFunctionSlots; ++i) {
        const auto pointer = reinterpret_cast<uintptr_t>(addresses[i]);
        const auto base = reinterpret_cast<uintptr_t>(module);
        if (pointer < base || pointer - base >= image.size) return false;
        const size_t entry = pointer - base;
        if (!inText(entry, 5)) return false;
        size_t target = entry;
        bool thunk = false;
        if (!FunctionContains(module, entry, 1) && image.base[entry] == 0xE9) {
            int displacement;
            memcpy(&displacement, image.base + entry + 1, sizeof(displacement));
            const int64_t destination = int64_t(entry) + 5 + displacement;
            if (destination < 0 || uint64_t(destination) >= image.size) return false;
            target = static_cast<size_t>(destination);
            thunk = true;
        }
        if (!inText(target, 32)) return false;
        auto& f = local.functions[i];
        f.rva = static_cast<DWORD>(entry);
        f.length = 32;
        f.kind = 0;
        if (!FunctionContains(module, target, 32)) {
            if (i == 3) {
                f.kind = 1;
                f.length = 13;
            } else if (i == 6 && FocusLeafLayout(image.base + target)) {
                f.kind = 2;
            } else {
                return false;
            }
        }
        if (thunk) {
            f.kind = 3;
            f.reserved = static_cast<DWORD>(target);
        }
        memcpy(f.code, image.base + target, f.length);
        memset(f.mask, 0xFF, f.length);
    }
    // Bounds, exact unwind entries, approved leaf shapes, direct E9 thunks,
    // and disjoint entry/body ranges are independent local safety checks.
    // Authentication of named entries is enforced separately by PipelineTrust.
    return ValidateCompatibilityMap(module, local, rva);
}

// Branch files only locate data. HTTPS GitHub API metadata authenticates the
// immutable artifact digest, source run and workflow blob. The approved YAML
// checks out fixed extractor/action commits; main cannot replace its tools.
namespace PipelineTrust {
using Json = winrt::Windows::Data::Json::JsonObject;
static constexpr wchar_t kWorkflowBlob[] = L"f6a8bb8ac2140b8b9e4d0ec116959fb5e91da5a1";
static constexpr wchar_t kWorkflowSha256[] =
    L"bfde353d078a3e83ca112228a8e72fa22fc7fdec89b9974f092e012c1cdd4ecf";
static constexpr uint64_t kRepositoryId = 1428612836, kWorkflowId = 394997888;
static constexpr size_t kMaxCache = 16384;
static std::atomic<bool> stopping{false};
using Getter = std::function<std::string(const wchar_t*, const std::wstring&, size_t)>;

static bool Hex(std::wstring_view value, size_t length) {
    return value.size() == length && std::all_of(value.begin(), value.end(), [](wchar_t c) {
               return (c >= L'0' && c <= L'9') || (c >= L'a' && c <= L'f');
           });
}
static uint64_t Id(const Json& json, const wchar_t* key) {
    const double value = json.GetNamedNumber(key);
    if (!std::isfinite(value) || value < 1 || value > 9007199254740991.0 ||
        std::floor(value) != value)
        throw std::runtime_error("invalid GitHub identifier");
    return static_cast<uint64_t>(value);
}
static Json Parse(const std::string& bytes) {
    if (bytes.empty() || bytes.size() > 128 * 1024)
        throw std::runtime_error("invalid metadata size");
    return Json::Parse(winrt::to_hstring(bytes));
}
static std::array<unsigned char, 32> Digest(std::string_view bytes) {
    std::array<unsigned char, 32> digest{};
    BCRYPT_ALG_HANDLE algorithm = nullptr;
    if (bytes.size() > kMaxCache ||
        BCryptOpenAlgorithmProvider(&algorithm, BCRYPT_SHA256_ALGORITHM, nullptr, 0) < 0)
        throw std::runtime_error("SHA256 unavailable");
    const auto status = BCryptHash(
        algorithm, nullptr, 0, reinterpret_cast<PUCHAR>(const_cast<char*>(bytes.data())),
        static_cast<ULONG>(bytes.size()), digest.data(), static_cast<ULONG>(digest.size()));
    BCryptCloseAlgorithmProvider(algorithm, 0);
    if (status < 0)
        throw std::runtime_error("SHA256 failed");
    return digest;
}
static std::wstring HashText(std::string_view bytes) {
    const auto digest = Digest(bytes);
    std::wstring result;
    for (const auto byte : digest) {
        result += L"0123456789abcdef"[byte >> 4];
        result += L"0123456789abcdef"[byte & 15];
    }
    return result;
}
static bool Verify(const std::string& cache, const std::string& locator, const std::wstring& pdbKey,
                   const Getter& get) try {
    if (cache.empty() || cache.size() > kMaxCache || stopping)
        return false;
    const auto pointer = Parse(locator);
    if (pointer.GetNamedNumber(L"schema") != 1)
        return false;
    const auto artifactId = Id(pointer, L"artifactId");
    const std::wstring root = L"/repos/bardelyne/brave-acrylic/";
    const auto artifact = Parse(
        get(L"api.github.com", root + L"actions/artifacts/" + std::to_wstring(artifactId), 32768));
    if (Id(artifact, L"id") != artifactId ||
        artifact.GetNamedString(L"name") != L"pdb_" + pdbKey + L".txt" ||
        Id(artifact, L"size_in_bytes") != cache.size() ||
        artifact.GetNamedString(L"digest") != L"sha256:" + HashText(cache))
        return false;
    const auto origin = artifact.GetNamedObject(L"workflow_run");
    const auto runId = Id(origin, L"id");
    const std::wstring revision{origin.GetNamedString(L"head_sha")};
    if (!Hex(revision, 40) || origin.GetNamedString(L"head_branch") != L"main" ||
        Id(origin, L"repository_id") != kRepositoryId ||
        Id(origin, L"head_repository_id") != kRepositoryId)
        return false;
    const auto run =
        Parse(get(L"api.github.com", root + L"actions/runs/" + std::to_wstring(runId), 65536));
    const auto event = run.GetNamedString(L"event");
    if (Id(run, L"id") != runId || Id(run, L"workflow_id") != kWorkflowId ||
        Id(run.GetNamedObject(L"repository"), L"id") != kRepositoryId ||
        Id(run.GetNamedObject(L"head_repository"), L"id") != kRepositoryId ||
        run.GetNamedString(L"path") != L".github/workflows/compatibility.yml" ||
        run.GetNamedString(L"head_sha") != revision ||
        run.GetNamedString(L"head_branch") != L"main" ||
        run.GetNamedString(L"status") != L"completed" ||
        run.GetNamedString(L"conclusion") != L"success" ||
        (event != L"schedule" && event != L"push" && event != L"workflow_dispatch"))
        return false;
    // A repository owner must not substitute a self-hosted machine for the
    // runner. Check every attempt, so an earlier untrusted attempt cannot be
    // laundered by a later hosted rerun of the same run ID.
    const auto jobs = Parse(
        get(L"api.github.com",
            root + L"actions/runs/" + std::to_wstring(runId) + L"/jobs?filter=all&per_page=100",
            128 * 1024));
    const auto items = jobs.GetNamedArray(L"jobs");
    if (items.Size() == 0 || items.Size() > 100 ||
        jobs.GetNamedNumber(L"total_count") != items.Size())
        return false;
    bool generated = false;
    for (const auto& item : items) {
        const auto job = item.GetObject();
        const auto name = job.GetNamedString(L"name");
        if (name != L"generate" && name != L"publish")
            return false;
        if (job.GetNamedValue(L"runner_id").ValueType() ==
            winrt::Windows::Data::Json::JsonValueType::Null)
            continue;
        if (job.GetNamedNumber(L"runner_group_id") != 0 ||
            job.GetNamedString(L"runner_group_name") != L"GitHub Actions" ||
            Id(job, L"runner_id") < 1 ||
            !std::wstring_view(job.GetNamedString(L"runner_name")).starts_with(L"GitHub Actions "))
            return false;
        if (name == L"generate" && job.GetNamedString(L"conclusion") == L"success")
            generated = true;
    }
    if (!generated)
        return false;
    // Read the workflow at the run's immutable commit, never the mutable main
    // branch or a caller-provided workflow hash. GitHub owns this association.
    const auto workflow =
        Parse(get(L"api.github.com",
                  root + L"contents/.github/workflows/compatibility.yml?ref=" + revision, 32768));
    if (workflow.GetNamedString(L"type") != L"file" ||
        workflow.GetNamedString(L"path") != L".github/workflows/compatibility.yml" ||
        workflow.GetNamedString(L"sha") != kWorkflowBlob ||
        workflow.GetNamedString(L"encoding") != L"base64")
        return false;
    const auto content = workflow.GetNamedString(L"content");
    DWORD length = 0;
    if (content.empty() || content.size() > 24000 ||
        !CryptStringToBinaryW(content.c_str(), content.size(),
                              CRYPT_STRING_BASE64 | CRYPT_STRING_STRICT, nullptr, &length, nullptr,
                              nullptr) ||
        length == 0 || length > kMaxCache)
        return false;
    std::string yaml(length, '\0');
    if (!CryptStringToBinaryW(content.c_str(), content.size(),
                              CRYPT_STRING_BASE64 | CRYPT_STRING_STRICT,
                              reinterpret_cast<BYTE*>(yaml.data()), &length, nullptr, nullptr) ||
        length != yaml.size())
        return false;
    return HashText(yaml) == kWorkflowSha256 && !stopping;
} catch (...) {
    return false;
}

struct Handle {
    HINTERNET value = nullptr;
    explicit Handle(HINTERNET v) : value(v) {
    }
    ~Handle() {
        if (value)
            WinHttpCloseHandle(value);
    }
    Handle(const Handle&) = delete;
    Handle& operator=(const Handle&) = delete;
};
class Http {
    const ULONGLONG deadline = GetTickCount64() + 8000;
    Handle session{WinHttpOpen(L"BraveAcrylic/" WH_MOD_VERSION, WINHTTP_ACCESS_TYPE_AUTOMATIC_PROXY,
                               WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0)};
    void Check() const {
        if (stopping || GetTickCount64() >= deadline)
            throw std::runtime_error("verification stopped");
    }

   public:
    std::string Get(const wchar_t* host, const std::wstring& path, size_t maximum) {
        Check();
        if (!session.value ||
            (wcscmp(host, L"api.github.com") && wcscmp(host, L"raw.githubusercontent.com")) ||
            path.empty() || path.front() != L'/' || path.size() > 256 || maximum > 128 * 1024)
            throw std::runtime_error("invalid verification request");
        const int timeout =
            static_cast<int>(std::min<ULONGLONG>(2000, deadline - GetTickCount64()));
        if (timeout < 1 || !WinHttpSetTimeouts(session.value, timeout, timeout, timeout, timeout))
            throw std::runtime_error("verification timeout");
        Handle connection{WinHttpConnect(session.value, host, INTERNET_DEFAULT_HTTPS_PORT, 0)};
        Handle request{connection.value
                           ? WinHttpOpenRequest(connection.value, L"GET", path.c_str(), nullptr,
                                                WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES,
                                                WINHTTP_FLAG_SECURE)
                           : nullptr};
        DWORD disabled =
            WINHTTP_DISABLE_COOKIES | WINHTTP_DISABLE_AUTHENTICATION | WINHTTP_DISABLE_REDIRECTS;
        if (!request.value || !WinHttpSetOption(request.value, WINHTTP_OPTION_DISABLE_FEATURE,
                                                &disabled, sizeof(disabled)))
            throw std::runtime_error("verification connection failed");
        static constexpr wchar_t headers[] =
            L"Accept: application/vnd.github+json\r\nX-GitHub-Api-Version: 2022-11-28\r\n";
        Check();
        if (!WinHttpSendRequest(request.value, headers, static_cast<DWORD>(-1),
                                WINHTTP_NO_REQUEST_DATA, 0, 0, 0) ||
            !WinHttpReceiveResponse(request.value, nullptr))
            throw std::runtime_error("verification request failed");
        DWORD status = 0, size = sizeof(status);
        if (!WinHttpQueryHeaders(
                request.value, WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,
                WINHTTP_HEADER_NAME_BY_INDEX, &status, &size, WINHTTP_NO_HEADER_INDEX) ||
            status != 200)
            throw std::runtime_error("verification unavailable");
        std::string bytes;
        char buffer[4096];
        for (;;) {
            Check();
            DWORD count = 0;
            if (!WinHttpReadData(request.value, buffer, sizeof(buffer), &count))
                throw std::runtime_error("verification read failed");
            if (!count)
                break;
            if (bytes.size() + count > maximum)
                throw std::runtime_error("oversized verification response");
            bytes.append(buffer, count);
        }
        Check();
        return bytes;
    }
};
static std::wstring BuildKey(const BuildIdentity& id) {
    wchar_t key[48];
    swprintf_s(key, L"%08X%04X%04X%02X%02X%02X%02X%02X%02X%02X%02X%X",
               static_cast<unsigned>(id.guid.Data1), id.guid.Data2, id.guid.Data3, id.guid.Data4[0],
               id.guid.Data4[1], id.guid.Data4[2], id.guid.Data4[3], id.guid.Data4[4],
               id.guid.Data4[5], id.guid.Data4[6], id.guid.Data4[7], static_cast<unsigned>(id.age));
    return key;
}
static std::wstring StorageKey(const BuildIdentity& id) {
    return L"trustedPipeline1:" + std::wstring(kWorkflowBlob) + L":" + BuildKey(id);
}
static bool ReadLocal(const BuildIdentity& id, std::string* cache) try {
    std::array<unsigned char, kMaxCache + 32> bytes{};
    const auto count = Wh_GetBinaryValue(StorageKey(id).c_str(), bytes.data(), bytes.size());
    if (count <= 32 || count > bytes.size())
        return false;
    const std::string data(reinterpret_cast<const char*>(bytes.data() + 32), count - 32);
    const auto digest = Digest(data);
    if (!std::equal(digest.begin(), digest.end(), bytes.begin()))
        return false;
    *cache = data;
    return !stopping;
} catch (...) {
    return false;
}
static void SaveLocal(const BuildIdentity& id, const std::string& cache) {
    const auto digest = Digest(cache);
    std::vector<unsigned char> bytes(digest.begin(), digest.end());
    bytes.insert(bytes.end(), cache.begin(), cache.end());
    Wh_SetBinaryValue(StorageKey(id).c_str(), bytes.data(), bytes.size());
}
static bool Load(const BuildIdentity& id, bool online, std::string* cache) try {
    if (stopping)
        return false;
    if (ReadLocal(id, cache))
        return true;
    if (!online)
        return false;  // Old, unverified Windhawk entries cannot bypass the gate.
    struct Apartment {
        HRESULT result = RoInitialize(RO_INIT_MULTITHREADED);
        ~Apartment() {
            if (SUCCEEDED(result))
                RoUninitialize();
        }
    } apartment;
    if (FAILED(apartment.result) && apartment.result != RPC_E_CHANGED_MODE)
        return false;
    Http http;
    const std::wstring stem =
        L"/bardelyne/brave-acrylic/symbol-cache/brave-acrylic/pdb_" + BuildKey(id);
    // A shared policy query prevents an older policy's CDN entry from being
    // reused immediately after a publisher/mod update. No per-user identifier.
    const std::wstring policy = L"?policy=" + std::wstring(kWorkflowBlob);
    const auto locator =
        http.Get(L"raw.githubusercontent.com", stem + L".proof.json" + policy, 2048);
    const auto data = http.Get(L"raw.githubusercontent.com", stem + L".txt" + policy, kMaxCache);
    if (!Verify(data, locator, BuildKey(id), [&](auto host, const auto& path, size_t maximum) {
            return http.Get(host, path, maximum);
        }))
        return false;
    SaveLocal(id, data);
    *cache = data;
    return true;
} catch (...) {
    return false;
}

static constexpr const char* kSymbols[] = {
    "protected: virtual void __cdecl views::View::OnPaintBackground(class gfx::Canvas *)",
    "public: void __cdecl gfx::Canvas::DrawColor(unsigned int,enum SkBlendMode)",
    "public: class std::__Cr::basic_string_view<char,struct std::__Cr::char_traits<char> > __cdecl views::View::GetClassNameW(void)const",
    "private: virtual bool __cdecl BrowserDesktopWindowTreeHostWin_ChromiumImpl::ShouldWindowContentsBeTransparent(void)const",
    "public: __cdecl BraveVerticalTabStripContainerView::BraveVerticalTabStripContainerView(class BrowserView *,class views::View *)",
    "public: void __cdecl views::View::SetBackground(class std::__Cr::unique_ptr<class views::Background,struct std::__Cr::default_delete<class views::Background> >)",
    "public: virtual bool __cdecl LocationBarView::HasFocus(void)const",
    "public: virtual bool __cdecl views::View::HasFocus(void)const",
    "public: virtual void __cdecl ContentsWebView::OnThemeChanged(void)",
    "public: virtual void __cdecl ContentsWebView::RenderViewReady(void)",
    "public: virtual void __cdecl ui::LayerAnimator::SetColor(struct SkRGBA4f<3>)",
    "public: virtual void __cdecl content::WebContentsImpl::SetPageBaseBackgroundColor(class std::__Cr::optional<unsigned int>)",
    "public: virtual void __cdecl ContentsWebView::SetWebContents(class content::WebContents *)",
};
static_assert(ARRAYSIZE(kSymbols) == kFunctionSlots);
static bool ParseCache(const std::string& bytes, const BuildIdentity& id, size_t* result) {
    constexpr size_t count = kFunctionSlots;
    if (count != kFunctionSlots || bytes.empty() || bytes.size() > kMaxCache ||
        !std::all_of(bytes.begin(), bytes.end(),
                     [](unsigned char c) { return c >= 32 && c <= 126; }))
        return false;
    std::vector<std::string_view> fields;
    size_t begin = 0;
    for (size_t i = 0; i <= bytes.size(); ++i)
        if (i == bytes.size() || bytes[i] == '#') {
            fields.emplace_back(bytes.data() + begin, i - begin);
            begin = i + 1;
        }
    const auto identity =
        "x86-64_" + std::to_string(id.timestamp) + "_" + std::to_string(id.imageSize);
    if (fields.size() != 3 + count * 2 || fields[0] != "1" || fields[1] != "chrome.dll" ||
        fields[2] != identity)
        return false;
    std::array<size_t, kFunctionSlots> entries{};
    for (size_t i = 0; i < count; ++i) {
        const auto name = fields[3 + i * 2], number = fields[4 + i * 2];
        if (name != kSymbols[i] || number.empty() || number.size() > 10 || number.front() == '0')
            return false;
        uint64_t value = 0;
        for (const auto c : number) {
            if (c < '0' || c > '9')
                return false;
            value = value * 10 + c - '0';
        }
        if (!value || value >= id.imageSize ||
            std::find(entries.begin(), entries.begin() + i, value) != entries.begin() + i)
            return false;
        entries[i] = static_cast<size_t>(value);
    }
    std::copy(entries.begin(), entries.end(), result);
    return true;
}
}  // namespace PipelineTrust

static bool ResolveSymbolAddresses(HMODULE module, size_t* rva) {
    void* addresses[kFunctionSlots]{};
    // chrome.dll
    WindhawkUtils::SYMBOL_HOOK chromeHooks[] = {
        {{L"protected: virtual void __cdecl views::View::OnPaintBackground(class gfx::Canvas *)"},
         &addresses[0]},
        {{L"public: void __cdecl gfx::Canvas::DrawColor(unsigned int,enum SkBlendMode)"},
         &addresses[1]},
        {{L"public: class std::__Cr::basic_string_view<char,struct std::__Cr::char_traits<char> > __cdecl views::View::GetClassNameW(void)const"},
         &addresses[2]},
        {{L"private: virtual bool __cdecl BrowserDesktopWindowTreeHostWin_ChromiumImpl::ShouldWindowContentsBeTransparent(void)const"},
         &addresses[3]},
        {{L"public: __cdecl BraveVerticalTabStripContainerView::BraveVerticalTabStripContainerView(class BrowserView *,class views::View *)"},
         &addresses[4]},
        {{L"public: void __cdecl views::View::SetBackground(class std::__Cr::unique_ptr<class views::Background,struct std::__Cr::default_delete<class views::Background> >)"},
         &addresses[5]},
        {{L"public: virtual bool __cdecl LocationBarView::HasFocus(void)const"}, &addresses[6]},
        {{L"public: virtual bool __cdecl views::View::HasFocus(void)const"}, &addresses[7]},
        {{L"public: virtual void __cdecl ContentsWebView::OnThemeChanged(void)"}, &addresses[8]},
        {{L"public: virtual void __cdecl ContentsWebView::RenderViewReady(void)"}, &addresses[9]},
        {{L"public: virtual void __cdecl ui::LayerAnimator::SetColor(struct SkRGBA4f<3>)"},
         &addresses[10]},
        {{L"public: virtual void __cdecl content::WebContentsImpl::SetPageBaseBackgroundColor(class std::__Cr::optional<unsigned int>)"},
         &addresses[11]},
        {{L"public: virtual void __cdecl ContentsWebView::SetWebContents(class content::WebContents *)"},
         &addresses[12]},
    };
    static_assert(ARRAYSIZE(chromeHooks) == kFunctionSlots);
    BuildIdentity identity{};
    std::string trustedCache;
    std::array<size_t, kFunctionSlots> trusted{};
    if (!GetBuildIdentity(module, &identity) ||
        !PipelineTrust::Load(identity, s_useOnlineSymbolCache.load(std::memory_order_acquire),
                             &trustedCache) ||
        !PipelineTrust::ParseCache(trustedCache, identity, trusted.data())) {
        Wh_Log(
            L"Trusted pipeline proof unavailable or rejected; no browser functions will be used");
        return false;
    }

    WH_HOOK_SYMBOLS_OPTIONS options{};
    options.optionsSize = sizeof(options);
    options.symbolServer = L"";  // Never fall back to downloading a multi-GB PDB.
    // Use Windhawk's default undecorated C++ prototypes for standard caching.
    options.onlineCacheUrl =
        s_useOnlineSymbolCache.load(std::memory_order_acquire) ? kOnlineSymbolCache : L"";
    // Resolve without installing hooks. HookSymbols can return partial outputs
    // on failure; nothing is published unless the entire set passes validation.
    if (!WindhawkUtils::HookSymbols(module, chromeHooks, ARRAYSIZE(chromeHooks), &options))
        return false;
    // The ordinary cache can be stale, replaced, or contain earlier unverified
    // entries. Compare every named entry BEFORE using any function pointer.
    const auto base = reinterpret_cast<uintptr_t>(module);
    for (size_t i = 0; i < kFunctionSlots; ++i)
        if (reinterpret_cast<uintptr_t>(addresses[i]) != base + trusted[i])
            return false;
    return !PipelineTrust::stopping && ValidateSymbolAddresses(module, addresses, rva);
}

static bool ResolveNativeBackground(HMODULE module) {
    size_t rva[kFunctionSlots]{};
    if (!ResolveSymbolAddresses(module, rva)) {
        Wh_Log(L"Brave Acrylic %s: exact-build symbols unavailable or unsupported; transparency stays off",
               WH_MOD_VERSION);
        return false;
    }
    auto* base = reinterpret_cast<unsigned char*>(module);
    s_setPageBase = reinterpret_cast<NativeSetPageBase>(base + rva[11]);
    s_locationFocus = reinterpret_cast<NativeFocus>(base + rva[6]);
    s_viewFocus = reinterpret_cast<NativeFocus>(base + rva[7]);
    s_background.clear = reinterpret_cast<BraveBackground::Clear>(base + rva[1]);
    s_background.getClass = reinterpret_cast<BraveBackground::GetClass>(base + rva[2]);
    s_background.setBackground = reinterpret_cast<BraveBackground::SetBackground>(base + rva[5]);
    if (!WindhawkUtils::SetFunctionHook(reinterpret_cast<NativeEvent>(base + rva[8]),
                                      PageThemeHook, &s_originalTheme) ||
        !WindhawkUtils::SetFunctionHook(reinterpret_cast<NativeEvent>(base + rva[9]),
                                      PageReadyHook, &s_originalReady) ||
        !WindhawkUtils::SetFunctionHook(reinterpret_cast<NativeLayerColor>(base + rva[10]),
                                      PageLayerColorHook, &s_originalLayerColor) ||
        !WindhawkUtils::SetFunctionHook(reinterpret_cast<NativeAttachContents>(base + rva[12]),
                                      AttachContentsHook, &s_originalAttachContents) ||
        !WindhawkUtils::SetFunctionHook(reinterpret_cast<BraveBackground::Paint>(base + rva[0]),
                                      ViewOnPaintBackground_Hook, &s_background.originalPaint) ||
        !WindhawkUtils::SetFunctionHook(reinterpret_cast<BraveBackground::Transparent>(base + rva[3]),
                                      WindowContentsTransparent_Hook, &s_background.originalTransparent) ||
        !WindhawkUtils::SetFunctionHook(reinterpret_cast<BraveBackground::Construct>(base + rva[4]),
                                      SidebarConstructor_Hook, &s_background.originalConstruct) ||
        !HasCompleteTransparencyPath()) return false;
    Wh_Log(L"Brave Acrylic %s: 13/13 native functions from Windhawk symbol resolution and approved pipeline",
           WH_MOD_VERSION);
    return true;
}

// Installation and shutdown share a barrier. No hook can be registered/applied
// after Wh_ModBeforeUninit returns, including a loader call that finishes late.
static bool TryInstallChromeHooks(HMODULE module, bool applyNow) {
    if (!module || s_chromeAttempted.exchange(true, std::memory_order_acq_rel)) return false;
    AcquireSRWLockExclusive(&s_initLock);
    bool installed = false;
    if (!s_stopping && ResolveNativeBackground(module)) {
        if (applyNow) {
            installed = Wh_ApplyHookOperations() != FALSE;
            s_transparencyReady.store(installed, std::memory_order_release);
            if (!installed) Wh_Log(L"Could not activate paint hooks; transparency stays off");
        } else {
            // Windhawk applies these hooks after Wh_ModInit returns.
            s_activateAfterInit = true;
            installed = true;
        }
    }
    ReleaseSRWLockExclusive(&s_initLock);
    if (applyNow && installed) SyncNativeCaptions();
    return installed;
}

static bool IsExecutableLibraryLoad(DWORD flags) {
    return !(flags & (DONT_RESOLVE_DLL_REFERENCES | LOAD_LIBRARY_AS_DATAFILE |
                      LOAD_LIBRARY_AS_DATAFILE_EXCLUSIVE | LOAD_LIBRARY_AS_IMAGE_RESOURCE));
}

static HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR name, HANDLE file, DWORD flags) {
    const HMODULE module = pOrigLoadLibraryExW(name, file, flags);
    const DWORD error = GetLastError();
    if (!s_chromeAttempted.load(std::memory_order_acquire) && module &&
        IsExecutableLibraryLoad(flags) && module == GetModuleHandleW(L"chrome.dll")) {
        // Chrome's caller cannot use the new module until this hook returns.
        TryInstallChromeHooks(module, true);
    }
    SetLastError(error);
    return module;
}

enum class ProcessRole { Browser, Gpu, Other };

static ProcessRole GetProcessRole() {
    int count = 0;
    auto* arguments = CommandLineToArgvW(GetCommandLineW(), &count);
    if (!arguments) return ProcessRole::Other;
    ProcessRole role = ProcessRole::Browser;
    for (int i = 1; i < count; ++i) {
        if (!wcscmp(arguments[i], L"--") ||
            !wcscmp(arguments[i], L"--single-argument")) break;
        if (!wcsncmp(arguments[i], L"--type=", 7)) {
            role = !wcscmp(arguments[i] + 7, L"gpu-process") ? ProcessRole::Gpu : ProcessRole::Other;
        }
    }
    LocalFree(arguments);
    return role;
}

BOOL Wh_ModInit() {
    const auto role = GetProcessRole();
    if (role != ProcessRole::Browser) return FALSE;
    s_isBrowserProcess = true;
    LoadModSettings();
    InstallNativeCaptionHooks();

    HMODULE chrome = GetModuleHandleW(L"chrome.dll");
    if (chrome) {
        return TryInstallChromeHooks(chrome, false);
    }

    const HMODULE kernelBase = GetModuleHandleW(L"kernelbase.dll");
    const auto loader = kernelBase ? reinterpret_cast<LoadLibraryExW_t>(
        GetProcAddress(kernelBase, "LoadLibraryExW")) : nullptr;
    if (!loader || !WindhawkUtils::SetFunctionHook(loader, LoadLibraryExW_Hook, &pOrigLoadLibraryExW)) {
        return FALSE;
    }
    return TRUE;
}

void Wh_ModAfterInit() {
    if (!s_isBrowserProcess) return;
    AcquireSRWLockExclusive(&s_initLock);
    if (!s_stopping && s_activateAfterInit) {
        s_activateAfterInit = false;
        s_transparencyReady.store(HasCompleteTransparencyPath(), std::memory_order_release);
    }
    ReleaseSRWLockExclusive(&s_initLock);
    // Cover a load between ModInit's check and activation of the loader hook.
    if (const auto chrome = GetModuleHandleW(L"chrome.dll")) TryInstallChromeHooks(chrome, true);
    SyncNativeCaptions();
}

void Wh_ModSettingsChanged() {
    if (!s_isBrowserProcess) return;
    LoadModSettings();
    SyncNativeCaptions();
    Wh_Log(L"Settings updated; restart Brave normally to rebuild its cached UI brushes");
}

void Wh_ModBeforeUninit() {
    PipelineTrust::stopping.store(true, std::memory_order_release);
    s_captionStopping.store(true, std::memory_order_release);
    s_captionFixActive.store(false, std::memory_order_release);
    AcquireSRWLockExclusive(&s_initLock);
    s_stopping = true;
    s_activateAfterInit = false;
    s_transparencyReady.store(false, std::memory_order_release);
    ReleaseSRWLockExclusive(&s_initLock);
    // Restore the real WS_SYSMENU bit while the getter/setter hooks are still
    // installed, so Chromium cannot save a stripped style during teardown.
    if (s_isBrowserProcess) EnumWindows(SyncNativeCaptionWindow, 0);
}

#endif  // x64 native background ABI
