// ==WindhawkMod==
// @id              brave-acrylic
// @name            Brave Acrylic
// @description     Alpha: Native Brave background transparency; requires Translucent Windows
// @version         4.0
// @author          bardelyne
// @github          https://github.com/bardelyne
// @homepage        https://github.com/bardelyne/brave-acrylic
// @license         MIT
// @architecture    x86-64
// @include         brave.exe
// @compilerOptions -ldwmapi -ladvapi32 -lcomctl32 -lshell32 -luser32 -lgdi32 -lole32 -loleaut32 -lruntimeobject
// ==/WindhawkMod==
// ==WindhawkModReadme==
/*
# Brave Acrylic

> **Very important — Symbol cache:** Controlled and maintained by **bardelyne**, the author of Brave Acrylic. This is an author-hosted cache, not an official Windhawk service. Our GitHub Actions workflow checks for new Brave releases and prepares small compatibility files from Brave’s official symbols. By default, Windhawk downloads only the small cache for your Brave build. The optional **Download symbols from Brave as a fallback** setting is off by default.

> **If the mod isn’t working after a Brave update**
>
> - **Wait for the small cache (default).** The workflow checks for new Brave builds every hour. Give it about an hour, then fully exit and reopen Brave. GitHub can delay scheduled runs.
> - **Download directly from Brave.** If you have a fast connection and enough disk space, enable **Download symbols from Brave as a fallback** in the mod’s Windhawk settings, then fully exit and reopen Brave. On a cache miss, Windhawk can download the full symbols from Brave’s official server. For a recent build, the download was about **1.2 GB** and expanded to about **6 GB** on disk. If symbols are still loading after **five seconds**, Brave opens without acrylic. Leave Brave open until Windhawk finishes loading symbols, then restart Brave to apply the effect.

![Brave Acrylic](https://raw.githubusercontent.com/bardelyne/brave-acrylic/main/assets/brave-acrylic-v3.3.png)

Reveal the Windows backdrop through Brave's vertical tab area, frame, toolbar, address field, and webpage area. Add rounded acrylic menus and native popups, plus blur behind address-bar suggestions. Native background targeting works across theme colors. Use a website-styling extension for backgrounds painted by websites.

> **Alpha software.** **[Translucent Windows](https://windhawk.net/mods/translucent-windows) is required**, configured for `brave.exe`. Brave Acrylic exposes the background; Translucent Windows supplies the effect.

**Restart Brave after applying the mod, updating it, or changing its settings.** Close all Brave windows normally and exit any remaining background Brave instance, then reopen it. This rebuilds native windows and their cached backgrounds.

## Requirements

- x64 version of Brave.
- Windhawk with Brave Acrylic enabled.
- Translucent Windows configured to apply a translucent effect to `brave.exe`.
- Windows Transparency effects enabled.

## Compatibility

Windows 11 is the tested environment. Windows 10 is unverified; the backdrop provider and selected effect must support that Windows version. Native ARM64 Brave is unsupported.

Native popup suggestions were tested in Brave 1.98, including YouTube with Zen Internet transparency enabled. Version 4.0 adds rounded-content and popup coverage and separates the Toolbar and Address bar options.

The 4.0 compatibility study validated all eight feature groups on one official stable build from each Brave 1.93–1.99 release line (Chromium 151–157). Builds tested from 1.88–1.92 had partial coverage. The study checks matching binaries and symbols. Visual behavior is tested separately. Brave Acrylic uses compatibility data for the exact installed `chrome.dll`. See the [compatibility matrix run](https://github.com/bardelyne/brave-acrylic/actions/runs/42106823647) and [compiler study](https://github.com/bardelyne/brave-acrylic/blob/main/docs/compiler-resilience.md).

## Fast setup

1. Enable Translucent Windows for `brave.exe` and choose the desired backdrop.
2. Enable Brave Acrylic. Leave **Vertical tabs background**, **Toolbar**, **Address bar**, and **Webpage background** on.
3. Close all Brave windows normally and reopen Brave. Exit its background instance too if it stays running.
4. Optionally use Zen Internet or Stylus to remove backgrounds inside websites, as explained below. Stylus requires custom CSS.

No theme-color setup is required.

Brave's **Show rounded corners on main content areas** setting can remain enabled when the exact-build compatibility data includes rounded webpage support. The mod clears the outer content surround and the outline painter's backing fill while retaining the thin outline and native rounded clipping.

## Controls

| Setting | Default | Purpose |
| --- | --- | --- |
| **Vertical tabs background** | On | Reveal the backdrop behind the vertical tab area and browser frame. |
| **Toolbar** | On | Clear the toolbar and bookmarks bar backgrounds. |
| **Address bar** | On | Clear the unfocused address field and, where supported, its hover shadow. The focused field keeps its native appearance. |
| **Webpage background** | On | Clear only Chromium's background behind the webpage area, including its default white fallback and the fill behind rounded content areas. Native rounded clipping and the thin outline stay intact. Website HTML, CSS, and content are unchanged. Use Zen Internet or Stylus for website backgrounds; Stylus requires custom CSS. |
| **Acrylic menus** | On | Blur native context menus, the main menu, and submenus. Text, icons, selection, and input stay native. |
| **Acrylic popups** | On | Blur supported native browser bubbles, site information, recent downloads, media controls, and the native Extensions manager. Permission controls, download actions, album art, text and playback buttons keep their native painting. Extension and website HTML overlays keep their own styling. |
| **Blur search suggestions** | On | Use a native results popup with Windows blur below the input cutout. Typed text, result text and input remain native and sharp. |
| **Popup background opacity (%)** | 62 | Control the background tint of menus, native popups and suggestions from 0 (transparent) to 100 (opaque). Text, icons and controls retain their native opacity. Restart Brave after changing it. |
| **Hide duplicate Windows buttons** | On | Hide the extra Windows minimize, maximize and close buttons while keeping Brave’s controls. This can affect Alt+Space, taskbar menus and other window tools. |
| **Use online symbol cache** | On | Let Windhawk download small exact-build files from the author-controlled cache. |
| **Download symbols from Brave as a fallback** | Off | Let Windhawk download the full PDB from Brave’s official symbol server if it cannot resolve symbols from the available caches. A tested build used about 1.2 GB of bandwidth and 6.2 GB of disk space. Restart Brave after changing this setting. |

Restart Brave after applying settings, even when a change appears immediately. Some backing views and native popup choices are created with their windows. Webpage backing can apply on tab selection without refreshing the page; restarting after disabling it restores existing renderer defaults. The caption-button option can apply immediately.

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

The full native path uses **36 functions**, of which **24 are private hooks**, and one immutable background-class vtable. Each feature has its own dependency set. Features with missing dependencies keep their native appearance. The rounded webpage effect reads Chromium’s packed paint flags to distinguish the backing fill from the outline. Changes to that layout can affect the rounded webpage effect. The implementation uses no sampled colors, guessed screen rectangles, screenshot polling, input forwarding, or GPU hooks. Normal DirectComposition stays enabled, with no special browser launch flag. Windows supplies the backdrop. Acrylic can turn gray when inactive. Choose a suitable blur mode in Translucent Windows for the main window if you prefer a persistent effect.

## Menus, popups and suggestions

Native menus and bubbles keep their own theme colors with reduced background alpha. A Windows host-backdrop visual sits underneath Chromium in the same popup window and follows its actual rounded background geometry. There are no extra overlay windows or input forwarding. Where supported, bubble popups use a short fade instead of scaling so their contents and backdrop appear and disappear together. Context menus, the main menu and bookmark menus retain their native animation path. Windows reduced-motion preferences are respected. A private composition worker owns its own dispatcher; disabling the mod closes its targets and joins that worker without changing Brave's dispatcher. If Windows cannot read the host-backdrop capability flag, cleanup removes the visual effect without guessing a prior flag value.

Tab and group hover cards, including their fading text and memory icons, keep their foreground painting. Their separate client and footer backgrounds are removed, while a tint derived from Brave’s own background color sits beneath Chromium’s content. This keeps the tint consistent during transitions and avoids black fade masks. The same native-popup path covers dialogs such as **Restore pages?**. Windows-drawn popup borders are suppressed where supported and their previous value is restored on unload.

The native **Extensions** manager also clears its repeated scroll-container backgrounds so they cannot cover the blur. Extension names, icons, pin buttons, menus and the Manage extensions button keep their native painting and input. This does not alter HTML content inside an extension's own popup.

Suggestions use Chromium's normal desktop-popup factory instead of its in-window Aura override. Windows supplies the blur behind that native results popup, which also works with the tested Zen Internet transparency setup. Chromium still owns its contents, layout, keyboard navigation and mouse input. Native view bounds and header height define a rounded window region below the address field; there is no input catcher or separate overlay. Resizing and DPI changes update the region, and cleanup restores the actual prior region on the owning UI thread. Restart Brave after applying this option so existing results widgets are rebuilt. These options do not turn website dialogs or extension HTML into acrylic. Windows high contrast and transparency preferences are cached and updated from Windows notifications off Brave’s paint path.

## Symbol resolution, privacy and updates

Brave Acrylic uses Windhawk’s `HookSymbols` to resolve exact C++ prototypes and the background-class identity. Windhawk checks its local symbol cache and then the author-hosted cache when enabled. If those lookups do not resolve the symbols and the fallback is on, Windhawk can download and unpack the official Brave PDB. Both sources use the mod’s local executable-range, unwind-boundary, approved-leaf, thunk and overlap checks before hooks are registered. The workflow also checks for compiler aliases when generating the small cache.

Small named-symbol files are published under `brave-acrylic-features-v9/` on our [`symbol-cache` branch](https://github.com/bardelyne/brave-acrylic/tree/symbol-cache). They contain the native function locations needed for each Brave build. The service and publishing workflow are controlled and maintained by bardelyne.

Windhawk handles compatibility downloads and local caching through its standard mechanism. Our workflow uses pinned actions and checks the official Brave archive and matching symbols before publishing. See [cache hosting](https://github.com/bardelyne/brave-acrylic/blob/main/docs/trusted-pipeline.md) for the technical details.

**No telemetry, uploads or login.** Windhawk downloads symbol data. The server used for the download receives your IP address and the requested build. Browser cookies, history, page content, settings and account details stay on your PC. Turn both symbol download options off and restart Brave to use local symbols only.

The workflow checks the stable release, asset identities and native contract before downloading large files. Unchanged releases skip browser and PDB downloads. Both generation and publication use the run's same source revision; large official files stay on the GitHub runner. Restart Brave after newly required compatibility data becomes available.

The extractor handles verified compiler folding according to each function's role, while rejecting unrelated sharing on patched functions. The workflow reports compatibility separately for vertical tabs, the toolbar, the address bar, webpage backing, menus, popups, suggestions, and rounded webpage backing. A missing or changed function disables only its dependent features. Validated features are still published, and the workflow summary lists unavailable features with their reasons. Invalid files, mismatched symbols, failed downloads, and publishing errors still stop publication.

## Duplicate corner buttons

**Hide duplicate Windows buttons** is on by default. It removes the window’s `WS_SYSMENU` style, which can affect Alt+Space, taskbar menus, accessibility tools and window managers. Turn the option off to restore the original window style.

## Troubleshooting

After installing or applying changes, close all Brave windows and its background instance, then reopen normally. If Windhawk cached a failed symbol lookup, enable debug logging in the mod’s Advanced tab before restarting Brave to retry. For website styling, check Zen Internet's transparency setting and whether that site has a supported style, or check the CSS and site scope of your Stylus style.

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
  $description: Reveal the backdrop behind the toolbar and bookmarks bar.
- transparentAddressBar: true
  $name: Address bar
  $description: Clear the unfocused address field and, where supported, its hover shadow. The focused field keeps its native appearance.
- transparentPageBackground: true
  $name: Webpage background
  $description: Clear Chromium's webpage backing, including its default white fallback and the fill behind rounded content. The rounded outline and website content stay intact. Use Zen Internet or custom CSS with Stylus for website backgrounds.
- blurMenus: true
  $name: Acrylic menus
  $description: Blur native context menus and submenus while preserving their theme, rounded corners, text and input.
- blurPopups: true
  $name: Acrylic popups
  $description: Blur supported native browser bubbles such as Site information, Recent downloads and Media controls. Extension and website HTML popups keep their own styling.
- blurSuggestions: true
  $name: Blur search suggestions
  $description: Blur address-bar suggestions below the input field. Typed text, result text and selection stay sharp.
- popupOpacity: 62
  $name: Popup background opacity (%)
  #! $min: 0
  #! $max: 100
  $description: "Background tint for menus, native popups and search suggestions. 0 is transparent and 100 is opaque; text, icons and controls stay fully visible."
- hideNativeCaptionButtons: true
  $name: Hide duplicate Windows buttons
  $description: Hide the extra Windows minimize, maximize and close buttons while keeping Brave’s controls. This removes the Windows system-menu style and can affect Alt+Space, taskbar menus, accessibility tools and window managers. Applies immediately.
- useOnlineSymbolCache: true
  $name: Use online symbol cache
  $description: Download small compatibility files from the author-controlled symbol cache. GitHub sees your IP and requested build URLs. Restart Brave after applying changes.
- allowBraveSymbolServerFallback: false
  $name: Download symbols from Brave as a fallback
  $description: If the cache cannot resolve symbols, let Windhawk download the full PDB from Brave’s official symbol server. Brave continues after five seconds if symbols are still loading. Restart Brave after Windhawk finishes. One tested build downloaded about 1.2 GB and used 6.2 GB on disk.
*/
// ==/WindhawkModSettings==
#include <windows.h>
#include <commctrl.h>
#include <dwmapi.h>
#include <memory>
#include <unordered_map>
#include <winrt/Windows.System.h>
#include <winrt/Windows.UI.Composition.h>
#include <winrt/Windows.UI.Composition.Desktop.h>
#include <windhawk_api.h>
#ifndef _M_X64
BOOL Wh_ModInit() {
    Wh_Log(L"Brave Acrylic requires x64 Brave.");
    return FALSE;
}
#else
#include <shellapi.h>
#include <atomic>
#include <cwchar>
#include <cstring>
#include <windhawk_utils.h>
#include <roapi.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <algorithm>
#include <array>
#include <cmath>
#include <vector>
#include <chrono>

// Native background roles, MIT, bardelyne.
// Native ABI: x64 Brave 1.98.60 / Chromium 156. Resolve and validate every
// function for the exact installed DLL before publishing ready=true.

#include <cstddef>
#include <string>
#include <string_view>

namespace BraveBackground {
struct ClassName {
    const char *data;
    size_t length;
};
using Paint = void (*)(void *, void *);
using Clear = void (*)(void *, unsigned int, int);
using GetClass = ClassName *(*)(const void *, ClassName *);
using Transparent = bool (*)(const void *);
using Construct = void *(*)(void *, void *, void *);
// The native nontrivial unique_ptr parameter is passed indirectly on x64.
// Only an empty native unique_ptr is supplied; Brave destroys its old object.
using SetBackground = void (*)(void *, void **);
enum Area : unsigned {
    Background = 1,
    Toolbar = 2,
    Page = 4,
    AddressBar = 128,
    All = Background | Toolbar | Page | AddressBar,
};

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
        return originalPaint && clear && getClass && originalTransparent && originalConstruct &&
               setBackground;
    }
};

inline bool IsBackgroundContainer(ClassName name, unsigned areas) {
    if (!name.data || !name.length || name.length >= 128 || !areas)
        return false;
    struct Role {
        const char *name;
        unsigned areas;
    };
    static constexpr Role roles[] = {
        // A transparent shared root is required for either child area. An
        // unselected child still paints its own original opaque background.
        {"BrowserView", All},
        {"BraveBrowserView", All},
        {"TopContainerView", Toolbar},
        {"ToolbarView", Toolbar},
        {"BraveToolbarView", Toolbar},
        {"BookmarkBarView", Toolbar},
        {"BraveBookmarkBarView", Toolbar},
        {"VerticalTabStripRegionView", Background},
        {"BraveVerticalTabStripRegionView", Background},
        {"VerticalTabStripContainerView", Background},
        {"BraveVerticalTabStripContainerView", Background},
        {"ContentsContainerView", Page},
        {"MainBackgroundRegionView", Page},
        {"BraveContentsContainerView", Page}};
    for (const auto &role : roles) {
        const size_t length = std::strlen(role.name);
        if ((areas & role.areas) && length == name.length &&
            !std::memcmp(name.data, role.name, length))
            return true;
    }
    return false;
}

inline void PaintBackground(Context &context, void *view, void *canvas) {
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
    if (context.originalPaint)
        context.originalPaint(view, canvas);
}

inline bool WindowContentsTransparent(Context &context, const void *host) {
    if (context.ready.load(std::memory_order_acquire) &&
        context.areas.load(std::memory_order_relaxed))
        return true;
    return context.originalTransparent ? context.originalTransparent(host) : false;
}

inline void *ConstructSidebar(Context &context, void *self, void *browser, void *backingView) {
    void *result = context.originalConstruct(self, browser, backingView);
    if (backingView && context.ready.load(std::memory_order_acquire) &&
        (context.areas.load(std::memory_order_relaxed) & Background)) {
        // Brave passes this otherwise anonymous View to the constructor. No
        // object-layout offsets, geometry guesses, or retained View pointers.
        void *empty = nullptr;
        context.setBackground(backingView, &empty);
    }
    return result;
}
} // namespace BraveBackground

static BraveBackground::Context s_background;
static auto &s_transparencyReady = s_background.ready;
static std::atomic<unsigned> s_availableAreas{BraveBackground::All};
static std::atomic<unsigned> s_requestedUi{8 | 16 | 32};
static std::atomic<unsigned> s_popupOpacity{62};
static std::atomic<bool> s_chromeAttempted{false}, s_useOnlineSymbolCache{true};
static std::atomic<bool> s_allowBraveSymbolServerFallback{false};
static SRWLOCK s_initLock = SRWLOCK_INIT;
static bool s_stopping = false, s_activateAfterInit = false, s_isBrowserProcess = false;
using LoadLibraryExW_t = decltype(&LoadLibraryExW);
static LoadLibraryExW_t pOrigLoadLibraryExW = nullptr;

// Native x64 SkColor4f is passed indirectly as a 16-byte aggregate.
struct NativeColor {
    float r, g, b, a;
};
static_assert(sizeof(NativeColor) == 16);
using NativeFocus = bool (*)(const void *);
using NativeEvent = void (*)(void *);
using NativeLayerColor = void (*)(void *, const NativeColor *);
using NativeSetPageBase = void (*)(void *, uint64_t);
using NativeAttachContents = void (*)(void *, void *);
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
    ~PageScope() {
        s_pageBackgroundScope = old;
    }
};
static bool ClearPageFallback() {
    return s_pageBackgroundScope && s_transparencyReady.load(std::memory_order_acquire) &&
           (s_background.areas.load(std::memory_order_relaxed) & BraveBackground::Page);
}
// Brave retains the outline stroke, rounded clip and layout insets.
namespace RoundedContents {
using BorderPaint = void (*)(void *, void *);
using PathPaint = void (*)(void *, const void *, const void *, int);
static BorderPaint originalBorder = nullptr;
static PathPaint originalPath = nullptr;
static std::atomic<bool> available{false};
static thread_local bool clearBackingFill = false;
static bool Target(void *view) {
    if (!view || !available.load(std::memory_order_relaxed) ||
        !s_transparencyReady.load(std::memory_order_acquire) ||
        !(s_background.areas.load(std::memory_order_relaxed) & BraveBackground::Page))
        return false;
    BraveBackground::ClassName name{};
    s_background.getClass(view, &name);
    constexpr std::string_view role = "BraveContentsContainerView";
    return name.data && name.length == role.size() &&
           !memcmp(name.data, role.data(), role.size());
}
static void BorderHook(void *view, void *canvas) {
    const bool previous = clearBackingFill;
    clearBackingFill = canvas && Target(view);
    originalBorder(view, canvas);
    clearBackingFill = previous;
}
static bool IsBackingFill(const void *flags) {
    if (!flags)
        return false;
    // Chromium x64 CorePaintFlags stores its packed paint options after
    // SkColor4f, stroke_width and miter_limit. Brave's rounded outline painter
    // uses kSrc. Match only its fill style, regardless of path draw order.
    constexpr size_t kPaintOptionsOffset = 24;
    constexpr uint32_t kStyleMask = 0xC0, kBlendModeMask = 0x1F00, kSourceBlend = 0x100;
    uint32_t options = 0;
    memcpy(&options, static_cast<const unsigned char *>(flags) + kPaintOptionsOffset,
           sizeof(options));
    return (options & (kStyleMask | kBlendModeMask)) == kSourceBlend;
}
static void PathHook(void *canvas, const void *path, const void *flags, int cache) {
    if (clearBackingFill && IsBackingFill(flags)) {
        clearBackingFill = false;
        return;
    }
    originalPath(canvas, path, flags, cache);
}
} // namespace RoundedContents
static void PageThemeHook(void *self) {
    PageScope scope;
    s_originalTheme(self);
}
static void PageReadyHook(void *self) {
    PageScope scope;
    s_originalReady(self);
}
static void PageLayerColorHook(void *self, const NativeColor *color) {
    const NativeColor transparent{};
    s_originalLayerColor(self, ClearPageFallback() ? &transparent : color);
}
// The native optional<SkColor> argument occupies RDX: color in the low
// DWORD, has-value in byte 4. Set an explicit transparent base, not nullopt.
// Chromium stores it on WebContents and sends it to current and future renderers.
static void AttachContentsHook(void *self, void *contents) {
    if (contents && s_transparencyReady.load(std::memory_order_acquire) &&
        (s_background.areas.load(std::memory_order_relaxed) & BraveBackground::Page)) {
        s_setPageBase(contents, uint64_t(1) << 32);
    }
    // Use the supplied canonical WebContents pointer before attach notifications.
    // No retained objects, member offsets, reloads or selection polling.
    s_originalAttachContents(self, contents);
}
static bool AddressBarActive() {
    return s_transparencyReady.load(std::memory_order_acquire) &&
           (s_background.areas.load(std::memory_order_relaxed) & BraveBackground::AddressBar);
}

static bool AddressShouldClear(void *view) {
    if (!view || !AddressBarActive())
        return false;
    BraveBackground::ClassName name{};
    s_background.getClass(view, &name);
    if (!name.data || name.length > 127)
        return false;
    auto is = [&](const char *target) {
        return strlen(target) == name.length && !memcmp(name.data, target, name.length);
    };
    const bool location = is("LocationBarView") || is("BraveLocationBarView");
    const bool input = is("OmniboxViewViews") || is("BraveOmniboxViewViews");
    if (!location && !input)
        return false;
    return !(location ? s_locationFocus(view) : s_viewFocus(view));
}

// Brave passes this value by const reference and copies it into its shadow
// cache. Leave the original argument, geometry and native layer lifetime alone.
struct ShadowParameters {
    int offsetX;
    int offsetY;
    int blurRadius;
    uint32_t color;
};
static_assert(sizeof(ShadowParameters) == 16 && offsetof(ShadowParameters, color) == 12);
using ConstructShadow = void *(*)(void *, void *, const void *, const ShadowParameters *);
static ConstructShadow s_originalShadow = nullptr;

static void *ConstructShadowHook(void *self, void *view, const void *corners,
                                 const ShadowParameters *params) {
    if (view && params && AddressBarActive()) {
        BraveBackground::ClassName role{};
        s_background.getClass(view, &role);
        if (role.data && std::string_view(role.data, role.length) == "BraveLocationBarView") {
            auto transparent = *params;
            transparent.color = 0;
            return s_originalShadow(self, view, corners, &transparent);
        }
    }
    return s_originalShadow(self, view, corners, params);
}

static bool HasCompleteTransparencyPath() {
    const unsigned areas = s_availableAreas.load(std::memory_order_acquire);
    return areas && s_background.originalPaint && s_background.clear && s_background.getClass &&
           s_background.originalTransparent &&
           (!(areas & BraveBackground::Background) ||
            (s_background.originalConstruct && s_background.setBackground)) &&
           (!(areas & BraveBackground::AddressBar) || (s_locationFocus && s_viewFocus)) &&
           (!(areas & BraveBackground::Page) ||
            (s_originalTheme && s_originalReady && s_originalLayerColor && s_setPageBase &&
             s_originalAttachContents));
}
namespace NativeUi {
static bool SuppressPopupBackground(void *view);
}
static void ViewOnPaintBackground_Hook(void *view, void *canvas) {
    if (canvas && NativeUi::SuppressPopupBackground(view)) {
        BraveBackground::ClassName role{};
        s_background.getClass(view, &role);
        if (role.data && std::string_view(role.data, role.length) != "FooterView" &&
            std::string_view(role.data, role.length) != "FadeLabel")
            s_background.clear(canvas, 0, 0);
        return;
    }
    if (canvas && AddressShouldClear(view)) {
        s_background.clear(canvas, 0, 0);
        return;
    }
    BraveBackground::PaintBackground(s_background, view, canvas);
}
static bool WindowContentsTransparent_Hook(const void *host) {
    return BraveBackground::WindowContentsTransparent(s_background, host);
}
static void *SidebarConstructor_Hook(void *self, void *browser, void *host) {
    return BraveBackground::ConstructSidebar(s_background, self, browser, host);
}

// Windows still draws its caption buttons underneath Chromium's transparent
// custom frame. Hide only WS_SYSMENU, retaining the resize/caption/min/max bits.
// Chromium reads its original logical style, including when saving fullscreen
// state; DWM sees the real style without the native buttons.
static constexpr wchar_t kNativeCaptionProperty[] = L"Windhawk.BraveAcrylic.NativeCaption";
static std::atomic<bool> s_hideNativeCaptionButtons{false};
static std::atomic<bool> s_captionFixActive{false};
static std::atomic<bool> s_captionStopping{false};
// Stay conservative after the option has been used: a concurrent style write
// may still need its ownership property while settings or shutdown restore it.
static std::atomic<bool> s_captionPropertyUsed{false};
static bool s_captionHooksInstalled = false;
static decltype(&CreateWindowExW) pOrigCreateWindowExW = nullptr;
static decltype(&GetWindowLongPtrW) pOrigGetWindowLongPtrW = nullptr;
static decltype(&SetWindowLongPtrW) pOrigSetWindowLongPtrW = nullptr;
static decltype(&GetWindowLongW) pOrigGetWindowLongW = nullptr;
static decltype(&SetWindowLongW) pOrigSetWindowLongW = nullptr;
static thread_local HWND s_captionStyleWrite = nullptr;

static bool IsBrowserCaptionFrame(HWND window, LONG_PTR style) {
    constexpr LONG_PTR required = WS_CAPTION | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX;
    if (!window || (style & required) != required || (style & WS_CHILD))
        return false;
    DWORD process = 0;
    GetWindowThreadProcessId(window, &process);
    if (process != GetCurrentProcessId() || GetAncestor(window, GA_ROOT) != window ||
        GetWindow(window, GW_OWNER))
        return false;
    const auto exStyle = pOrigGetWindowLongPtrW(window, GWL_EXSTYLE);
    if (exStyle & (WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE | WS_EX_DLGMODALFRAME))
        return false;
    wchar_t name[32]{};
    return GetClassNameW(window, name, ARRAYSIZE(name)) && !wcscmp(name, L"Chrome_WidgetWin_1");
}

static void RefreshNativeCaption(HWND window) {
    SetWindowPos(window, nullptr, 0, 0, 0, 0,
                 SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);
}

static void RestoreNativeCaption(HWND window) {
    if (!GetPropW(window, kNativeCaptionProperty))
        return;
    RemovePropW(window, kNativeCaptionProperty);
    // Restore before hooks are removed; the ownership property is already gone.
    const auto style = GetWindowLongPtrW(window, GWL_STYLE);
    SetWindowLongPtrW(window, GWL_STYLE, style | WS_SYSMENU);
    RefreshNativeCaption(window);
}

template <typename Value>
static Value ReadCaptionStyle(HWND window, int index, Value(WINAPI *original)(HWND, int)) {
    if (index != GWL_STYLE ||
        (!s_captionFixActive.load(std::memory_order_acquire) &&
         !s_captionPropertyUsed.load(std::memory_order_relaxed)))
        return original(window, index);
    auto value = original(window, index);
    const DWORD error = GetLastError();
    if (index == GWL_STYLE && GetPropW(window, kNativeCaptionProperty))
        value |= WS_SYSMENU;
    SetLastError(error);
    return value;
}

static LONG_PTR WINAPI GetWindowLongPtrW_Hook(HWND window, int index) {
    return ReadCaptionStyle(window, index, pOrigGetWindowLongPtrW);
}

static LONG WINAPI GetWindowLongW_Hook(HWND window, int index) {
    return ReadCaptionStyle(window, index, pOrigGetWindowLongW);
}

template <typename Value>
static Value WriteCaptionStyle(HWND window, int index, Value value,
                               Value(WINAPI *original)(HWND, int, Value)) {
    // Windows may forward one API to the other. Translate the style only once.
    if (index != GWL_STYLE || s_captionStyleWrite == window ||
        (!s_captionFixActive.load(std::memory_order_acquire) &&
         !s_captionPropertyUsed.load(std::memory_order_relaxed)))
        return original(window, index, value);
    const bool owned = GetPropW(window, kNativeCaptionProperty) != nullptr;
    const bool hide = s_captionFixActive.load(std::memory_order_acquire) && (value & WS_SYSMENU) &&
                      (owned || IsBrowserCaptionFrame(window, value));
    bool marked = owned;
    if (hide && !owned) {
        s_captionPropertyUsed.store(true, std::memory_order_relaxed);
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
    const auto previous =
        original(window, index, hide && marked ? value & ~static_cast<Value>(WS_SYSMENU) : value);
    const DWORD error = GetLastError();
    s_captionStyleWrite = previousWrite;
    if (!previous && error) {
        if (owned)
            SetPropW(window, kNativeCaptionProperty, reinterpret_cast<HANDLE>(1));
        else
            RemovePropW(window, kNativeCaptionProperty);
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
        GetPropW(window, kNativeCaptionProperty))
        return;
    const auto style = pOrigGetWindowLongPtrW(window, GWL_STYLE);
    if (!(style & WS_SYSMENU) || !IsBrowserCaptionFrame(window, style))
        return;
    SetWindowLongPtrW_Hook(window, GWL_STYLE, style);
    if (GetPropW(window, kNativeCaptionProperty))
        RefreshNativeCaption(window);
}

static HWND WINAPI CreateWindowExW_Hook(DWORD exStyle, LPCWSTR className, LPCWSTR name, DWORD style,
                                        int x, int y, int width, int height, HWND parent,
                                        HMENU menu, HINSTANCE instance, LPVOID param) {
    const auto window = pOrigCreateWindowExW(exStyle, className, name, style, x, y, width, height,
                                             parent, menu, instance, param);
    const DWORD error = GetLastError();
    if (window)
        ApplyNativeCaption(window);
    SetLastError(error);
    return window;
}

static BOOL CALLBACK SyncNativeCaptionWindow(HWND window, LPARAM) {
    DWORD process = 0;
    GetWindowThreadProcessId(window, &process);
    if (process == GetCurrentProcessId()) {
        if (s_captionFixActive.load(std::memory_order_acquire))
            ApplyNativeCaption(window);
        else
            RestoreNativeCaption(window);
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
    if (s_captionHooksInstalled)
        EnumWindows(SyncNativeCaptionWindow, 0);
}

static void InstallNativeCaptionHooks() {
    s_captionHooksInstalled =
        WindhawkUtils::SetFunctionHook(CreateWindowExW, CreateWindowExW_Hook,
                                       &pOrigCreateWindowExW) &&
        WindhawkUtils::SetFunctionHook(GetWindowLongPtrW, GetWindowLongPtrW_Hook,
                                       &pOrigGetWindowLongPtrW) &&
        WindhawkUtils::SetFunctionHook(SetWindowLongPtrW, SetWindowLongPtrW_Hook,
                                       &pOrigSetWindowLongPtrW) &&
        WindhawkUtils::SetFunctionHook(GetWindowLongW, GetWindowLongW_Hook, &pOrigGetWindowLongW) &&
        WindhawkUtils::SetFunctionHook(SetWindowLongW, SetWindowLongW_Hook, &pOrigSetWindowLongW);
    if (!s_captionHooksInstalled)
        Wh_Log(L"Native caption fix unavailable; acrylic is unchanged");
}

static void LoadModSettings() {
    unsigned areas = 0;
    if (Wh_GetIntSetting(L"transparentBackground"))
        areas |= BraveBackground::Background;
    if (Wh_GetIntSetting(L"transparentToolbar"))
        areas |= BraveBackground::Toolbar;
    if (Wh_GetIntSetting(L"transparentAddressBar"))
        areas |= BraveBackground::AddressBar;
    if (Wh_GetIntSetting(L"transparentPageBackground"))
        areas |= BraveBackground::Page;
    s_background.areas.store(areas & s_availableAreas.load(), std::memory_order_relaxed);
    unsigned ui = 0;
    if (Wh_GetIntSetting(L"blurMenus"))
        ui |= 8;
    if (Wh_GetIntSetting(L"blurPopups"))
        ui |= 16;
    if (Wh_GetIntSetting(L"blurSuggestions"))
        ui |= 32;
    s_requestedUi = ui;
    s_popupOpacity.store(static_cast<unsigned>(std::clamp(Wh_GetIntSetting(L"popupOpacity"), 0, 100)),
                         std::memory_order_relaxed);
    s_hideNativeCaptionButtons.store(Wh_GetIntSetting(L"hideNativeCaptionButtons") != 0,
                                     std::memory_order_relaxed);
    s_useOnlineSymbolCache.store(Wh_GetIntSetting(L"useOnlineSymbolCache") != 0,
                                 std::memory_order_release);
    s_allowBraveSymbolServerFallback.store(
        Wh_GetIntSetting(L"allowBraveSymbolServerFallback") != 0, std::memory_order_release);
}

struct ReadOnlyImage {
    const unsigned char *base;
    size_t size;
    const IMAGE_SECTION_HEADER *sections;
    unsigned count;

    bool Contains(size_t rva, size_t length) const {
        if (rva > size || length > size - rva)
            return false;
        for (unsigned i = 0; i < count; ++i) {
            const auto &s = sections[i];
            const DWORD flags = s.Characteristics;
            if (!(flags & IMAGE_SCN_MEM_READ) || !(flags & IMAGE_SCN_CNT_INITIALIZED_DATA) ||
                (flags & (IMAGE_SCN_MEM_WRITE | IMAGE_SCN_MEM_EXECUTE)) ||
                s.VirtualAddress > size || s.Misc.VirtualSize > size - s.VirtualAddress)
                continue;
            if (rva >= s.VirtualAddress && rva - s.VirtualAddress <= s.Misc.VirtualSize &&
                length <= s.Misc.VirtualSize - (rva - s.VirtualAddress))
                return true;
        }
        return false;
    }
};

static bool GetReadOnlyImage(HMODULE mod, ReadOnlyImage *out) {
    MEMORY_BASIC_INFORMATION memory{};
    if (!mod || !VirtualQuery(mod, &memory, sizeof(memory)) || memory.State != MEM_COMMIT ||
        (memory.Protect & (PAGE_GUARD | PAGE_NOACCESS)))
        return false;
    const auto *base = reinterpret_cast<const unsigned char *>(mod);
    const size_t headerBytes =
        memory.RegionSize - (base - static_cast<const unsigned char *>(memory.BaseAddress));
    if (headerBytes < sizeof(IMAGE_DOS_HEADER))
        return false;
    const auto *dos = reinterpret_cast<const IMAGE_DOS_HEADER *>(base);
    if (dos->e_magic != IMAGE_DOS_SIGNATURE || dos->e_lfanew < 0 ||
        static_cast<size_t>(dos->e_lfanew) > headerBytes ||
        sizeof(IMAGE_NT_HEADERS64) > headerBytes - dos->e_lfanew)
        return false;
    const auto *nt = reinterpret_cast<const IMAGE_NT_HEADERS64 *>(base + dos->e_lfanew);
    if (nt->Signature != IMAGE_NT_SIGNATURE ||
        nt->OptionalHeader.Magic != IMAGE_NT_OPTIONAL_HDR64_MAGIC ||
        nt->FileHeader.SizeOfOptionalHeader < sizeof(IMAGE_OPTIONAL_HEADER64) ||
        !nt->FileHeader.NumberOfSections || nt->FileHeader.NumberOfSections > 96)
        return false;
    const auto *sections = IMAGE_FIRST_SECTION(nt);
    const size_t sectionOffset = reinterpret_cast<const unsigned char *>(sections) - base;
    const size_t sectionBytes = nt->FileHeader.NumberOfSections * sizeof(*sections);
    const size_t imageSize = nt->OptionalHeader.SizeOfImage;
    if (sectionOffset > headerBytes || sectionBytes > headerBytes - sectionOffset ||
        sectionOffset > imageSize || sectionBytes > imageSize - sectionOffset)
        return false;
    *out = {base, imageSize, sections, nt->FileHeader.NumberOfSections};
    return true;
}

static bool GetFunctionTable(HMODULE mod, const RUNTIME_FUNCTION **table, size_t *count) {
    const auto *dos = reinterpret_cast<const IMAGE_DOS_HEADER *>(mod);
    const auto *nt = reinterpret_cast<const IMAGE_NT_HEADERS64 *>(
        reinterpret_cast<const unsigned char *>(mod) + dos->e_lfanew);
    const auto &dir = nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXCEPTION];
    const size_t imageSize = nt->OptionalHeader.SizeOfImage;
    if (!dir.VirtualAddress || dir.VirtualAddress > imageSize ||
        dir.Size > imageSize - dir.VirtualAddress || !dir.Size ||
        dir.Size % sizeof(RUNTIME_FUNCTION))
        return false;
    *table = reinterpret_cast<const RUNTIME_FUNCTION *>(
        reinterpret_cast<const unsigned char *>(mod) + dir.VirtualAddress);
    *count = dir.Size / sizeof(RUNTIME_FUNCTION);
    return true;
}

static bool FunctionContains(HMODULE mod, size_t rva, size_t length) {
    const RUNTIME_FUNCTION *table;
    size_t count;
    if (!GetFunctionTable(mod, &table, &count))
        return false;
    size_t lo = 0, hi = count;
    while (lo < hi) {
        const size_t mid = lo + (hi - lo) / 2;
        if (table[mid].BeginAddress < rva)
            lo = mid + 1;
        else
            hi = mid;
    }
    return lo < count && table[lo].BeginAddress == rva && table[lo].EndAddress > rva &&
           length <= table[lo].EndAddress - rva;
}

static bool FocusLeafLayout(const unsigned char *code) {
    const unsigned char lateFalse[] = {
        0x48, 0x83, 0xB9, 0,    0,    0,    0,    0, 0x74, 0x13, 0x48, 0x8B, 0x81, 0,    0,   0, 0,
        0x48, 0x8B, 0x40, 0x28, 0x83, 0x78, 0x18, 0, 0x0F, 0x95, 0xC0, 0xC3, 0x31, 0xC0, 0xC3};
    const unsigned char earlyFalse[] = {0x48, 0x83, 0xB9, 0,    0,    0,    0,    0,
                                        0x75, 3,    0x31, 0xC0, 0xC3, 0x48, 0x8B, 0x81,
                                        0,    0,    0,    0,    0x48, 0x8B, 0x40, 0x28,
                                        0x83, 0x78, 0x18, 0,    0x0F, 0x95, 0xC0, 0xC3};
    const auto matches = [&](const unsigned char *layout, unsigned member) {
        for (unsigned i = 0; i < 32; ++i) {
            const bool offset = (i >= 3 && i < 7) || (i >= member && i < member + 4);
            if (!offset && code[i] != layout[i])
                return false;
        }
        return true;
    };
    return matches(lateFalse, 13) || matches(earlyFalse, 16);
}
static bool LeafOutsideUnwind(HMODULE module, size_t rva, size_t length) {
    const RUNTIME_FUNCTION *table;
    size_t count;
    if (!GetFunctionTable(module, &table, &count))
        return false;
    // Inspect all preceding ranges, including optimized overlapping entries.
    // A nearest-predecessor lookup can miss an enclosing earlier function.
    for (size_t i = 0; i < count; ++i) {
        if (table[i].BeginAddress >= rva + length)
            break;
        if (table[i].BeginAddress >= rva || table[i].EndAddress > rva)
            return false;
    }
    return true;
}
// Windhawk retains standard named-symbol resolution and its own local cache.
// The author-hosted cache is trusted; local checks validate address structure.
// Windhawk handles the optional official PDB fallback when the cache misses.
static constexpr wchar_t kOnlineSymbolCache[] =
    L"https://raw.githubusercontent.com/bardelyne/brave-acrylic/symbol-cache/brave-acrylic-features-v9";
static constexpr wchar_t kBraveSymbolServer[] = L"https://brave-symbols.s3.brave.com";

// <feature-kernel>
static constexpr unsigned kFeatureSlots = 37;
static constexpr unsigned kVertical = 1, kToolbar = 2, kWebpage = 4, kMenus = 8, kPopups = 16,
                          kSuggestions = 32, kRoundedWebpage = 64,
                          kAddressBar = BraveBackground::AddressBar;
static constexpr unsigned kPopupFade = 256, kAddressShadow = 512;
static unsigned CompleteFeatures(const size_t *entries) {
    const auto all = [&](std::initializer_list<unsigned> slots) {
        for (unsigned i : slots)
            if (!entries[i])
                return false;
        return true;
    };
    return (all({0, 1, 2, 3, 4, 5}) ? kVertical : 0) | (all({0, 1, 2, 3}) ? kToolbar : 0) |
           (all({0, 1, 2, 3, 8, 9, 10, 11, 12}) ? kWebpage : 0) |
           (all({13, 14, 15, 17}) ? kMenus : 0) |
           (all({0, 1, 2, 13, 14, 16, 17, 25, 26, 27}) ? kPopups : 0) |
           (all({5, 13, 14, 18, 19, 20, 21, 22, 23, 24, 28, 29}) ? kSuggestions : 0) |
           (all({0, 1, 2, 3, 8, 9, 10, 11, 12, 30, 31}) ? kRoundedWebpage : 0) |
           (all({0, 1, 2, 3, 6, 7}) ? kAddressBar : 0);
}
static unsigned CompleteOptionalFeatures(const size_t *entries, unsigned features) {
    return ((features & kPopups) && entries[32] && entries[33] && entries[34] && entries[35]
                ? kPopupFade
                : 0) |
           ((features & kAddressBar) && entries[36] ? kAddressShadow : 0);
}
static bool MatchNativeLayout(const unsigned char *code, const unsigned char *pattern,
                              unsigned length, std::initializer_list<unsigned> displacements) {
    unsigned start = 0;
    for (unsigned offset : displacements) {
        if (memcmp(code + start, pattern + start, offset - start))
            return false;
        start = offset + 4;
    }
    return !memcmp(code + start, pattern + start, length - start);
}

static bool FeatureHelperLeaf(const unsigned char *code, unsigned slot, unsigned *length) {
    const auto match = [&](const auto &pattern, std::initializer_list<unsigned> displacements) {
        if (!MatchNativeLayout(code, pattern, sizeof(pattern), displacements))
            return false;
        *length = sizeof(pattern);
        return true;
    };
    constexpr unsigned char bounds[] = {
        0x48, 0x89, 0xD0, 0x48, 0xC7, 0x02, 0x00, 0x00, 0x00, 0x00, 0x48, 0x8B,
        0x89, 0x00, 0x00, 0x00, 0x00, 0x48, 0x89, 0x4A, 0x08, 0xC3,
    };
    constexpr unsigned char setter[] = {
        0x41, 0x89, 0xD0, 0x48, 0x81, 0xC1, 0x00, 0x00, 0x00, 0x00, 0x48, 0x8D,
        0x15, 0x00, 0x00, 0x00, 0x00, 0xE9, 0x00, 0x00, 0x00, 0x00,
    };
    // Both native HWND lookup layouts retain their exact control flow.
    constexpr unsigned char lookup0[] = {
        0x48, 0x85, 0xC9, 0x74, 0x19, 0x48, 0x89, 0xC8, 0x48, 0x83, 0xB8, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x75, 0x1B, 0x48, 0x8B, 0x80, 0x00, 0x00, 0x00,
        0x00, 0x48, 0x85, 0xC0, 0x75, 0xEA, 0x31, 0xC0, 0xC3, 0x48, 0x8B, 0x89,
        0x00, 0x00, 0x00, 0x00, 0x48, 0x85, 0xC9, 0x74, 0x0C, 0x48, 0x8B, 0x91,
        0x00, 0x00, 0x00, 0x00, 0x48, 0x85, 0xD2, 0x74, 0xE8, 0x48, 0x8B, 0x02,
        0x48, 0x8B, 0x40, 0x58, 0x48, 0x89, 0xD1, 0x48, 0xFF, 0x25, 0x00, 0x00,
        0x00, 0x00,
    };
    constexpr unsigned char lookup1[] = {
        0x48, 0x85, 0xC9, 0x74, 0x44, 0x48, 0x89, 0xC8, 0x48, 0x83, 0xB8, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x75, 0x1A, 0x48, 0x8B, 0x80, 0x00, 0x00, 0x00,
        0x00, 0x48, 0x85, 0xC0, 0x75, 0xEA, 0xEB, 0x29, 0x48, 0x8B, 0x89, 0x00,
        0x00, 0x00, 0x00, 0x48, 0x85, 0xC9, 0x74, 0x0C, 0x48, 0x8B, 0x91, 0x00,
        0x00, 0x00, 0x00, 0x48, 0x85, 0xD2, 0x74, 0xE8, 0x48, 0x8B, 0x02, 0x48,
        0x8B, 0x40, 0x58, 0x48, 0x89, 0xD1, 0x48, 0xFF, 0x25, 0x00, 0x00, 0x00,
        0x00, 0x31, 0xC0, 0xC3,
    };
    if (slot == 22)
        return match(bounds, {13});
    if (slot == 34)
        return match(setter, {6, 13, 18});
    if (slot == 33)
        return match(lookup0, {11, 21, 36, 48, 70}) || match(lookup1, {11, 21, 35, 47, 69});
    return false;
}
static bool ValidateFeatureAddresses(HMODULE module, void *const *addresses, size_t *result) {
    ReadOnlyImage image{};
    if (!GetReadOnlyImage(module, &image))
        return false;
    const IMAGE_SECTION_HEADER *text = nullptr;
    for (unsigned i = 0; i < image.count; ++i)
        if (!memcmp(image.sections[i].Name, ".text\0\0\0", 8) &&
            (image.sections[i].Characteristics & IMAGE_SCN_MEM_EXECUTE)) {
            if (text)
                return false;
            text = &image.sections[i];
        }
    if (!text || text->VirtualAddress > image.size ||
        text->Misc.VirtualSize > image.size - text->VirtualAddress)
        return false;
    const auto inText = [&](size_t start, size_t length) {
        return start >= text->VirtualAddress &&
               start - text->VirtualAddress <= text->Misc.VirtualSize &&
               length <= text->Misc.VirtualSize - (start - text->VirtualAddress);
    };
    struct Span {
        size_t start, length;
        unsigned slot;
    };
    std::vector<Span> spans;
    std::array<size_t, kFeatureSlots> normalized{};
    const auto base = reinterpret_cast<uintptr_t>(module);
    const auto validate = [&](unsigned i, size_t entry) -> size_t {
        if (i == 24) {
            bool immutable = false;
            for (unsigned n = 0; n < image.count; ++n) {
                const auto &section = image.sections[n];
                if (!memcmp(section.Name, ".rdata\0\0", 8) &&
                    !(section.Characteristics & (IMAGE_SCN_MEM_WRITE | IMAGE_SCN_MEM_EXECUTE)) &&
                    entry >= section.VirtualAddress &&
                    entry - section.VirtualAddress <= section.Misc.VirtualSize &&
                    16 <= section.Misc.VirtualSize - (entry - section.VirtualAddress))
                    immutable = true;
            }
            if (!immutable || entry % alignof(uintptr_t) || !image.Contains(entry, 16))
                return 0;
            for (unsigned p = 0; p < 16; p += 8) {
                uintptr_t method = 0;
                memcpy(&method, image.base + entry + p, 8);
                if (method < base || !inText(method - base, 1))
                    return 0;
            }
            spans.push_back({entry, 16, i});
            return entry;
        }
        if (!inText(entry, 32))
            return 0;
        size_t target = entry;
        bool thunk = false;
        if (!FunctionContains(module, entry, 1) && image.base[entry] == 0xE9) {
            int delta = 0;
            memcpy(&delta, image.base + entry + 1, 4);
            const int64_t destination = int64_t(entry) + 5 + delta;
            if (destination < 0 || uint64_t(destination) >= image.size ||
                !LeafOutsideUnwind(module, entry, 5))
                return 0;
            target = static_cast<size_t>(destination);
            thunk = true;
        }
        if (!inText(target, 32))
            return 0;
        unsigned length = 32;
        if (!FunctionContains(module, target, 32)) {
            if (i == 3) {
                const unsigned char first[] = {0x48, 0x83, 0xB9},
                                    last[] = {0, 0x74, 3, 0x31, 0xC0, 0xC3};
                if (memcmp(image.base + target, first, 3) ||
                    memcmp(image.base + target + 7, last, 6))
                    return 0;
                length = 13;
            } else if (i == 6 && FocusLeafLayout(image.base + target)) {
            } else if (!inText(target, i == 33 ? 80 : 32) ||
                       !FeatureHelperLeaf(image.base + target, i, &length))
                return 0;
            if (!LeafOutsideUnwind(module, target, length))
                return 0;
            if (i == 34) {
                int delta = 0;
                memcpy(&delta, image.base + target + 18, sizeof(delta));
                const int64_t destination = int64_t(target) + 22 + delta;
                if (destination < 0 || !inText(destination, 1) ||
                    !FunctionContains(module, destination, 1))
                    return 0;
            }
        }
        if (thunk)
            spans.push_back({entry, 5, i});
        spans.push_back({target, length, i});
        return target;
    };
    for (unsigned i = 0; i < kFeatureSlots; ++i) {
        const auto pointer = reinterpret_cast<uintptr_t>(addresses[i]);
        if (pointer >= base && pointer - base < image.size)
            normalized[i] = validate(i, pointer - base);
    }
    // Reject both roles if they resolve to overlapping code or data. Other
    // independently validated roles can still provide their features.
    for (size_t i = 0; i < spans.size(); ++i) {
        const auto &a = spans[i];
        for (size_t j = i + 1; j < spans.size(); ++j) {
            const auto &b = spans[j];
            if (a.start < b.start + b.length && b.start < a.start + a.length)
                normalized[a.slot] = normalized[b.slot] = 0;
        }
    }
    std::copy(normalized.begin(), normalized.end(), result);
    return true;
}
struct FeatureSymbol {
    std::wstring_view undecorated;
    std::wstring_view decorated;
    std::wstring_view alternate{};
};
static constexpr FeatureSymbol kFeatureSymbols[] = {
    {L"protected: virtual void __cdecl views::View::OnPaintBackground(class gfx::Canvas *)",
     LR"(?OnPaintBackground@View@views@@MEAAXPEAVCanvas@gfx@@@Z)"},
    {L"public: void __cdecl gfx::Canvas::DrawColor(unsigned int,enum SkBlendMode)",
     LR"(?DrawColor@Canvas@gfx@@QEAAXIW4SkBlendMode@@@Z)"},
    {L"public: class std::__Cr::basic_string_view<char,struct std::__Cr::char_traits<char> > __cdecl views::View::GetClassNameW(void)const",
     LR"(?GetClassNameW@View@views@@QEBA?AV?$basic_string_view@DU?$char_traits@D@__Cr@std@@@__Cr@std@@XZ)"},
    {L"private: virtual bool __cdecl BrowserDesktopWindowTreeHostWin_ChromiumImpl::ShouldWindowContentsBeTransparent(void)const",
     LR"(?ShouldWindowContentsBeTransparent@BrowserDesktopWindowTreeHostWin_ChromiumImpl@@EEBA_NXZ)"},
    {L"public: __cdecl BraveVerticalTabStripContainerView::BraveVerticalTabStripContainerView(class BrowserView *,class views::View *)",
     LR"(??0BraveVerticalTabStripContainerView@@QEAA@PEAVBrowserView@@PEAVView@views@@@Z)"},
    {L"public: void __cdecl views::View::SetBackground(class std::__Cr::unique_ptr<class views::Background,struct std::__Cr::default_delete<class views::Background> >)",
     LR"(?SetBackground@View@views@@QEAAXV?$unique_ptr@VBackground@views@@U?$default_delete@VBackground@views@@@__Cr@std@@@__Cr@std@@@Z)"},
    {L"public: virtual bool __cdecl LocationBarView::HasFocus(void)const",
     LR"(?HasFocus@LocationBarView@@UEBA_NXZ)"},
    {L"public: virtual bool __cdecl views::View::HasFocus(void)const",
     LR"(?HasFocus@View@views@@UEBA_NXZ)"},
    {L"public: virtual void __cdecl ContentsWebView::OnThemeChanged(void)",
     LR"(?OnThemeChanged@ContentsWebView@@UEAAXXZ)"},
    {L"public: virtual void __cdecl ContentsWebView::RenderViewReady(void)",
     LR"(?RenderViewReady@ContentsWebView@@UEAAXXZ)"},
    {L"public: virtual void __cdecl ui::LayerAnimator::SetColor(struct SkRGBA4f<3>)",
     LR"(?SetColor@LayerAnimator@ui@@UEAAXU?$SkRGBA4f@$02@@@Z)"},
    {L"public: virtual void __cdecl content::WebContentsImpl::SetPageBaseBackgroundColor(class std::__Cr::optional<unsigned int>)",
     LR"(?SetPageBaseBackgroundColor@WebContentsImpl@content@@UEAAXV?$optional@I@__Cr@std@@@Z)"},
    {L"public: virtual void __cdecl ContentsWebView::SetWebContents(class content::WebContents *)",
     LR"(?SetWebContents@ContentsWebView@@UEAAXPEAVWebContents@content@@@Z)"},
    {L"struct HWND__ * __cdecl views::HWNDForView(class views::View const *)",
     LR"(?HWNDForView@views@@YAPEAUHWND__@@PEBVView@1@@Z)"},
    {L"public: unsigned int __cdecl ui::ColorProvider::GetColor(int)const",
     LR"(?GetColor@ColorProvider@ui@@QEBAIH@Z)"},
    {L"public: virtual void __cdecl views::MenuScrollViewContainer::OnPaintBackground(class gfx::Canvas *)",
     LR"(?OnPaintBackground@MenuScrollViewContainer@views@@UEAAXPEAVCanvas@gfx@@@Z)"},
    {L"public: virtual void __cdecl views::BubbleBackground::Paint(class gfx::Canvas *,class views::View *)const",
     LR"(?Paint@BubbleBackground@views@@UEBAXPEAVCanvas@gfx@@PEAVView@2@@Z)"},
    {L"public: virtual void __cdecl cc::RecordPaintCanvas::drawRRect(class SkRRect const &,class cc::PaintFlags const &)",
     LR"(?drawRRect@RecordPaintCanvas@cc@@UEAAXAEBVSkRRect@@AEBVPaintFlags@2@@Z)"},
    {L"public: __cdecl RoundedOmniboxResultsFrame::RoundedOmniboxResultsFrame(class views::View *,class LocationBar *,bool)",
     LR"(??0RoundedOmniboxResultsFrame@@QEAA@PEAVView@views@@PEAVLocationBar@@_N@Z)"},
    {L"public: virtual __cdecl RoundedOmniboxResultsFrame::~RoundedOmniboxResultsFrame(void)",
     LR"(??1RoundedOmniboxResultsFrame@@UEAA@XZ)"},
    {L"private: virtual void __cdecl ui::Layer::SetRoundedCornersFromAnimation(class gfx::RoundedCornersF const &,enum ui::PropertyChangeReason)",
     LR"(?SetRoundedCornersFromAnimation@Layer@ui@@EEAAXAEBVRoundedCornersF@gfx@@W4PropertyChangeReason@2@@Z)"},
    {L"public: virtual void __cdecl views::SolidBackground::Paint(class gfx::Canvas *,class views::View *)const",
     LR"(?Paint@SolidBackground@views@@UEBAXPEAVCanvas@gfx@@PEAVView@2@@Z)"},
    {L"public: class gfx::Rect __cdecl views::View::GetLocalBounds(void)const",
     LR"(?GetLocalBounds@View@views@@QEBA?AVRect@gfx@@XZ)"},
    {L"public: static int __cdecl RoundedOmniboxResultsFrame::GetNonResultSectionHeight(bool)",
     LR"(?GetNonResultSectionHeight@RoundedOmniboxResultsFrame@@SAH_N@Z)"},
    {L"const views::SolidBackground::`vftable'",
     LR"(??_7SolidBackground@views@@6B@)"},
    {L"public: virtual void __cdecl views::BubbleFrameView::OnThemeChanged(void)",
     LR"(?OnThemeChanged@BubbleFrameView@views@@UEAAXXZ)"},
    {L"public: void __cdecl ui::Layer::SetFillsBoundsOpaquely(bool)",
     LR"(?SetFillsBoundsOpaquely@Layer@ui@@QEAAX_N@Z)",
     LR"(?SetFillsBoundsOpaquely@Layer@ui@@UEAAX_N@Z)"},
    {L"public: virtual void __cdecl FadeLabel::OnPaintBackground(class gfx::Canvas *)",
     LR"(?OnPaintBackground@FadeLabel@@UEAAXPEAVCanvas@gfx@@@Z)"},
    {L"public: class gfx::Rect __cdecl views::View::GetBoundsInScreen(void)const",
     LR"(?GetBoundsInScreen@View@views@@QEBA?AVRect@gfx@@XZ)"},
    {L"public: static void __cdecl RoundedOmniboxResultsFrame::OnBeforeWidgetInit(struct views::Widget::InitParams *,class views::Widget *)",
     LR"(?OnBeforeWidgetInit@RoundedOmniboxResultsFrame@@SAXPEAUInitParams@Widget@views@@PEAV34@@Z)"},
    {L"protected: virtual void __cdecl views::View::OnPaintBorder(class gfx::Canvas *)",
     LR"(?OnPaintBorder@View@views@@MEAAXPEAVCanvas@gfx@@@Z)"},
    {L"public: virtual void __cdecl cc::RecordPaintCanvas::drawPath(class SkPath const &,class cc::PaintFlags const &,enum cc::UsePaintCache)",
     LR"(?drawPath@RecordPaintCanvas@cc@@UEAAXAEBVSkPath@@AEBVPaintFlags@2@W4UsePaintCache@2@@Z)"},
    {L"bool __cdecl wm::AnimateOnChildWindowVisibilityChanged(class aura::Window *,bool)",
     LR"(?AnimateOnChildWindowVisibilityChanged@wm@@YA_NPEAVWindow@aura@@_N@Z)"},
    {L"struct HWND__ * __cdecl views::HWNDForNativeWindow(class aura::Window *const)",
     LR"(?HWNDForNativeWindow@views@@YAPEAUHWND__@@QEAVWindow@aura@@@Z)"},
    {L"void __cdecl wm::SetWindowVisibilityAnimationType(class aura::Window *,int)",
     LR"(?SetWindowVisibilityAnimationType@wm@@YAXPEAVWindow@aura@@H@Z)"},
    {L"void __cdecl wm::SetWindowVisibilityAnimationDuration(class aura::Window *,class base::TimeDelta const &)",
     LR"(?SetWindowVisibilityAnimationDuration@wm@@YAXPEAVWindow@aura@@AEBVTimeDelta@base@@@Z)"},
    {L"public: __cdecl ViewShadow::ViewShadow(class views::View *,class gfx::RoundedCornersF const &,struct ViewShadow::ShadowParameters const &)",
     LR"(??0ViewShadow@@QEAA@PEAVView@views@@AEBVRoundedCornersF@gfx@@AEBUShadowParameters@0@@Z)"},
};
static_assert(ARRAYSIZE(kFeatureSymbols) == kFeatureSlots);

static bool ResolveFeatureAddresses(HMODULE module, size_t *result, bool useOnlineCache,
                                    bool allowFallback) {
    void *addresses[kFeatureSlots]{};
    WindhawkUtils::SYMBOL_HOOK hooks[kFeatureSlots];
    for (unsigned i = 0; i < kFeatureSlots; ++i) {
        const auto &name = kFeatureSymbols[i];
        if (!allowFallback) {
            hooks[i] = {{name.undecorated}, &addresses[i], nullptr, true};
        } else if (name.alternate.empty()) {
            hooks[i] = {{name.undecorated, name.decorated}, &addresses[i], nullptr, true};
        } else {
            hooks[i] = {{name.undecorated, name.decorated, name.alternate},
                        &addresses[i], nullptr, true};
        }
    }

    // Keep the small cache names and avoid undecorating the full PDB.
    WH_HOOK_SYMBOLS_OPTIONS options{};
    options.optionsSize = sizeof(options);
    options.symbolServer = allowFallback ? kBraveSymbolServer : L"";
    options.onlineCacheUrl = useOnlineCache ? kOnlineSymbolCache : L"";
    options.noUndecoratedSymbols = allowFallback;
    if (!WindhawkUtils::HookSymbols(module, hooks, ARRAYSIZE(hooks), &options))
        return false;
    return ValidateFeatureAddresses(module, addresses, result);
}
// </feature-kernel>

// <native-ui>
// Native UI effects own their resources. No dispatcher belonging to Brave is
// stopped, and no callback from this DLL survives Wh_ModBeforeUninit.
namespace NativeUi {
enum Feature : unsigned {
    Vertical = 1,
    Toolbar = 2,
    Webpage = 4,
    Menus = 8,
    Popups = 16,
    Suggestions = 32
};
static std::atomic<unsigned> supported{0}, enabled{Menus | Popups | Suggestions};
static std::atomic<bool> ready{false};
using Paint = void (*)(void *, void *);
using BackgroundPaint = void (*)(const void *, void *, void *);
using GetColor = unsigned (*)(const void *, int);
using WindowForView = HWND (*)(const void *);
struct RoundRect {
    float left, top, right, bottom;
    float radii[8];
};
using DrawRoundRect = void (*)(void *, const RoundRect &, const void *);
static Paint originalMenu = nullptr;
static BackgroundPaint originalBubble = nullptr;
static GetColor originalColor = nullptr;
static WindowForView windowForView = nullptr;
static DrawRoundRect originalRoundRect = nullptr;
static thread_local unsigned backgroundScope = 0;
static thread_local HWND backgroundWindow = nullptr;
static bool Active(unsigned feature) {
    return ready.load(std::memory_order_acquire) &&
           (supported.load(std::memory_order_relaxed) & enabled.load(std::memory_order_relaxed) &
            feature);
}
static std::atomic<bool> transparencyAllowed{false};
static bool ReadTransparencyAllowed() {
    HIGHCONTRASTW contrast{sizeof(contrast)};
    if (!SystemParametersInfoW(SPI_GETHIGHCONTRAST, sizeof(contrast), &contrast, 0) ||
        (contrast.dwFlags & HCF_HIGHCONTRASTON))
        return false;
    DWORD enabled = 1, size = sizeof(enabled);
    RegGetValueW(HKEY_CURRENT_USER,
                 L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
                 L"EnableTransparency", RRF_RT_REG_DWORD, nullptr, &enabled, &size);
    return enabled != 0;
}
static bool RefreshTransparencyAllowed() {
    const bool allowed = ReadTransparencyAllowed();
    return transparencyAllowed.exchange(allowed, std::memory_order_relaxed) != allowed;
}
static bool UserAllowsTransparency() {
    return transparencyAllowed.load(std::memory_order_relaxed);
}
static bool PopupWindow(HWND hwnd) {
    DWORD process = 0;
    if (!hwnd || !GetWindowThreadProcessId(hwnd, &process) || process != GetCurrentProcessId())
        return false;
    const LONG_PTR style = GetWindowLongPtrW(hwnd, GWL_STYLE);
    if (!(style & WS_POPUP) || (style & (WS_CHILD | WS_THICKFRAME | WS_CAPTION)))
        return false;
    wchar_t name[48]{};
    return GetClassNameW(hwnd, name, ARRAYSIZE(name)) &&
           (!wcscmp(name, L"Chrome_WidgetWin_0") || !wcscmp(name, L"Chrome_WidgetWin_1"));
}
// Bubble contents and their Windows backdrop use a short fade, without scale.
// Menus do not receive this marker or animation policy.
namespace PopupTransition {
using Animate = bool (*)(void *, bool);
using WindowForNative = HWND (*)(void *);
using SetType = void (*)(void *, int);
using SetDuration = void (*)(void *, const long long &);
static constexpr unsigned kFadeMs = 160;
static SetType setType = nullptr;
static SetDuration setDuration = nullptr;
static bool CanFade();
static unsigned long long Begin(HWND hwnd, bool visible);
static void Immediate(HWND hwnd, unsigned long long serial);
static Animate original = nullptr;
static WindowForNative windowForNative = nullptr;
static SRWLOCK lock = SRWLOCK_INIT;
static std::vector<HWND> windows;
static bool stopped = false;
static constexpr wchar_t kProperty[] = L"BraveAcrylic.PopupFade.4.0";
static HANDLE Owner() { return reinterpret_cast<HANDLE>(&lock); }
static void Mark(HWND hwnd) {
    if (!original || !PopupWindow(hwnd) || !Active(Popups) || !UserAllowsTransparency())
        return;
    AcquireSRWLockExclusive(&lock);
    if (!stopped && Active(Popups) && CanFade() && !GetPropW(hwnd, kProperty) &&
        SetPropW(hwnd, kProperty, Owner()))
        windows.push_back(hwnd);
    ReleaseSRWLockExclusive(&lock);
}
static void Forget(HWND hwnd) {
    AcquireSRWLockExclusive(&lock);
    windows.erase(std::remove(windows.begin(), windows.end(), hwnd), windows.end());
    ReleaseSRWLockExclusive(&lock);
}
static bool AnimateHook(void *window, bool visible) {
    if (Active(Popups) && UserAllowsTransparency()) {
        const HWND hwnd = windowForNative(window);
        if (PopupWindow(hwnd) && GetPropW(hwnd, kProperty) == Owner()) {
            const auto serial = Begin(hwnd, visible);
            if (!serial)
                return original(window, visible);
            // Chromium's FADE type; transient native properties, not saved preferences.
            setType(window, 3);
            const long long duration = kFadeMs * 1000ll; // base::TimeDelta microseconds
            setDuration(window, duration);
            const bool animated = original(window, visible);
            if (!animated)
                Immediate(hwnd, serial);
            return animated;
        }
    }
    return original(window, visible);
}
static void Clear(bool stop) {
    AcquireSRWLockExclusive(&lock);
    stopped |= stop;
    for (HWND hwnd : windows)
        if (GetPropW(hwnd, kProperty) == Owner())
            RemovePropW(hwnd, kProperty);
    windows.clear();
    ReleaseSRWLockExclusive(&lock);
}
} // namespace PopupTransition
struct Scope {
    bool enabled;
    HWND previous;
    Scope(bool value, HWND window = nullptr) : enabled(value), previous(backgroundWindow) {
        if (enabled)
            ++backgroundScope;
        backgroundWindow = window;
    }
    ~Scope() {
        if (enabled)
            --backgroundScope;
        backgroundWindow = previous;
    }
};
static unsigned Tint(unsigned color, bool scoped) {
    return !scoped ? color : (color & 0x00ffffffu) |
        ((((color >> 24) * s_popupOpacity.load(std::memory_order_relaxed) + 50u) / 100u) << 24);
}

static thread_local unsigned popupTint = 0;
static thread_local bool popupTintKnown = false;
namespace Material {
namespace composition = winrt::Windows::UI::Composition;
constexpr wchar_t kProperty[] = L"BraveAcrylic.PopupMaterial.3.8";
MIDL_INTERFACE("29E691FA-4567-4DCA-B319-D0F207EB6807") DesktopInterop : IUnknown {
    virtual HRESULT STDMETHODCALLTYPE CreateDesktopWindowTarget(HWND, BOOL, void **) = 0;
    virtual HRESULT STDMETHODCALLTYPE EnsureOnThread(DWORD) = 0;
};
struct Shape {
    float left, top, width, height, rx, ry, scale;
};
static bool Equal(const Shape &a, const Shape &b) {
    return !memcmp(&a, &b, sizeof(Shape));
}
struct Fade {
    unsigned long long serial = 0;
    ULONGLONG start = 0;
    float from = 1, to = 1;
    unsigned duration = 0;
};
static float FadeValue(const Fade &fade, ULONGLONG now) {
    const float t = fade.duration ? std::min(1.f, static_cast<float>(now - fade.start) / fade.duration) : 1;
    return fade.from + (fade.to - fade.from) * t;
}
struct Request {
    Shape shape{};
    Fade fade{};
    bool valid = false, applied = false, pending = false;
    unsigned feature = 0;
    bool hoverCard = false;
    unsigned tint = 0;
    bool extensionsMenu = false;
    bool pageInfo = false, downloads = false, media = false;
};
struct State {
    composition::Desktop::DesktopWindowTarget target{nullptr};
    composition::ContainerVisual root{nullptr};
    unsigned long long fadeSerial = 0;
    composition::SpriteVisual sprite{nullptr}, shade{nullptr};
    composition::CompositionColorBrush shadeBrush{nullptr};
    composition::CompositionRoundedRectangleGeometry geometry{nullptr};
    BOOL previous = FALSE;
    bool changed = false, previousKnown = false;
    DWORD previousBorder = 0;
    bool borderKnown = false, borderChanged = false;
};
static SRWLOCK lock = SRWLOCK_INIT;
static std::unordered_map<HWND, Request> requests;
static HANDLE thread = nullptr, startEvent = nullptr;
static DWORD threadId = 0;
static std::atomic<bool> stopping{false};
static bool workerReady = false, workerFinished = false; // protected by lock
static constexpr UINT kUpdate = WM_APP + 0x137, kStop = WM_APP + 0x138, kRecheck = WM_APP + 0x139;
static bool CanQueueLocked() {
    return !stopping.load() && thread && !workerFinished;
}
static void NotifyLocked() {
    if (workerReady)
        PostThreadMessageW(threadId, kUpdate, 0, 0);
}
static constexpr wchar_t kSettingsClass[] = L"Windhawk.BraveAcrylic.TransparencySettings";
static LRESULT CALLBACK SettingsWindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    if ((message == WM_SETTINGCHANGE || message == WM_THEMECHANGED) &&
        RefreshTransparencyAllowed()) {
        PostThreadMessageW(GetCurrentThreadId(), kRecheck, 0, 0);
    }
    return DefWindowProcW(hwnd, message, wParam, lParam);
}
static std::unordered_map<HWND, State> *live = nullptr; // worker-thread only
static void Destroy(HWND hwnd) {
    if (!live)
        return;
    const auto found = live->find(hwnd);
    if (found == live->end())
        return;
    auto &state = found->second;
    if (GetPropW(hwnd, kProperty) == reinterpret_cast<HANDLE>(&state)) {
        RemovePropW(hwnd, kProperty);
        if (state.changed && state.previousKnown)
            DwmSetWindowAttribute(hwnd, 17, &state.previous, sizeof(BOOL));
        if (state.borderKnown && state.borderChanged)
            DwmSetWindowAttribute(hwnd, 34, &state.previousBorder, sizeof(DWORD));
    }
    try {
        if (state.target)
            state.target.Close();
    } catch (...) {
    }
    live->erase(found);
}
static void CALLBACK WindowEvent(HWINEVENTHOOK, DWORD, HWND hwnd, LONG object, LONG child, DWORD,
                                 DWORD) {
    if (object != OBJID_WINDOW || child != CHILDID_SELF || IsWindow(hwnd))
        return;
    Destroy(hwnd);
    PopupTransition::Forget(hwnd);
    AcquireSRWLockExclusive(&lock);
    requests.erase(hwnd);
    ReleaseSRWLockExclusive(&lock);
}
static DWORD WINAPI Run(void *) {
    HRESULT apartment = RoInitialize(RO_INIT_SINGLETHREADED);
    winrt::Windows::System::DispatcherQueueController queue{nullptr};
    composition::Compositor compositor{nullptr};
    std::unordered_map<HWND, State> states;
    HWINEVENTHOOK events = nullptr;
    HMODULE messaging = nullptr;
    HMODULE instance = nullptr;
    HWND settingsWindow = nullptr;
    ATOM settingsClass = 0;
    try {
        winrt::check_hresult(apartment);
        MSG ensure{};
        PeekMessageW(&ensure, nullptr, WM_USER, WM_USER, PM_NOREMOVE);
        struct Options {
            DWORD size, thread, apartment;
        } options{sizeof(Options), 2, 2};
        using CreateQueue = HRESULT(WINAPI *)(Options, void **);
        messaging = LoadLibraryExW(L"CoreMessaging.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
        const auto create = messaging ? reinterpret_cast<CreateQueue>(GetProcAddress(
                                            messaging, "CreateDispatcherQueueController"))
                                      : nullptr;
        if (!create)
            throw winrt::hresult_error(HRESULT_FROM_WIN32(ERROR_PROC_NOT_FOUND));
        winrt::check_hresult(create(options, winrt::put_abi(queue)));
        compositor = composition::Compositor();
        live = &states;
        events = SetWinEventHook(EVENT_OBJECT_DESTROY, EVENT_OBJECT_DESTROY, nullptr, WindowEvent,
                                 GetCurrentProcessId(), 0, WINEVENT_OUTOFCONTEXT);
        if (!events)
            throw winrt::hresult_error(HRESULT_FROM_WIN32(GetLastError()));
        if (!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                                    GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                                reinterpret_cast<LPCWSTR>(SettingsWindowProc), &instance)) {
            throw winrt::hresult_error(HRESULT_FROM_WIN32(GetLastError()));
        }
        WNDCLASSW windowClass{};
        windowClass.lpfnWndProc = SettingsWindowProc;
        windowClass.hInstance = instance;
        windowClass.lpszClassName = kSettingsClass;
        settingsClass = RegisterClassW(&windowClass);
        if (!settingsClass)
            throw winrt::hresult_error(HRESULT_FROM_WIN32(GetLastError()));
        // A hidden top-level window receives Windows setting broadcasts. A
        // message-only window does not. It never shows or handles browser input.
        settingsWindow = CreateWindowExW(0, kSettingsClass, L"", WS_POPUP, 0, 0, 0, 0, nullptr,
                                         nullptr, instance, nullptr);
        if (!settingsWindow)
            throw winrt::hresult_error(HRESULT_FROM_WIN32(GetLastError()));
        RefreshTransparencyAllowed();
        AcquireSRWLockExclusive(&lock);
        workerReady = true;
        ReleaseSRWLockExclusive(&lock);
        SetEvent(startEvent);
        PostThreadMessageW(GetCurrentThreadId(), kUpdate, 0, 0);
        MSG message{};
        while (GetMessageW(&message, nullptr, 0, 0) > 0) {
            if (message.message == kStop)
                break;
            if (message.message == kUpdate || message.message == kRecheck) {
                std::vector<std::pair<HWND, Request>> work;
                AcquireSRWLockExclusive(&lock);
                for (auto &[hwnd, request] : requests)
                    if (request.pending || message.message == kRecheck) {
                        request.pending = false;
                        work.emplace_back(hwnd, request);
                    }
                ReleaseSRWLockExclusive(&lock);
                const bool allowed = UserAllowsTransparency();
                for (const auto &[hwnd, request] : work) {
                    bool applied = false;
                    if (!stopping.load() && allowed && Active(request.feature) && request.valid &&
                        PopupWindow(hwnd))
                        try {
                            auto found = states.find(hwnd);
                            if (found == states.end()) {
                                auto [slot, inserted] = states.try_emplace(hwnd);
                                (void)inserted;
                                auto &state = slot->second;
                                if (!SetPropW(hwnd, kProperty, reinterpret_cast<HANDLE>(&state)))
                                    throw winrt::hresult_error(HRESULT_FROM_WIN32(GetLastError()));
                                // This attribute is set-only on some Windows builds.
                                // Closing our target removes its visual effect. Do
                                // not invent/restore a prior flag when it cannot be read.
                                state.previousKnown = SUCCEEDED(
                                    DwmGetWindowAttribute(hwnd, 17, &state.previous, sizeof(BOOL)));
                                state.borderKnown = SUCCEEDED(DwmGetWindowAttribute(
                                    hwnd, 34, &state.previousBorder, sizeof(DWORD)));
                                const DWORD noBorder = 0xfffffffeu; // DWMWA_COLOR_NONE
                                if (state.borderKnown)
                                    state.borderChanged = SUCCEEDED(DwmSetWindowAttribute(
                                        hwnd, 34, &noBorder, sizeof(noBorder)));
                                BOOL enable = TRUE;
                                winrt::check_hresult(
                                    DwmSetWindowAttribute(hwnd, 17, &enable, sizeof(enable)));
                                state.changed = true;
                                winrt::com_ptr<DesktopInterop> interop;
                                winrt::check_hresult(
                                    winrt::get_unknown(compositor)
                                        ->QueryInterface(
                                            GUID{0x29E691FA,
                                                 0x4567,
                                                 0x4DCA,
                                                 {0xB3, 0x19, 0xD0, 0xF2, 0x07, 0xEB, 0x68, 0x07}},
                                            interop.put_void()));
                                winrt::check_hresult(interop->CreateDesktopWindowTarget(
                                    hwnd, FALSE, winrt::put_abi(state.target)));
                                state.root = compositor.CreateContainerVisual();
                                state.sprite = compositor.CreateSpriteVisual();
                                state.geometry = compositor.CreateRoundedRectangleGeometry();
                                state.sprite.Brush(compositor.CreateHostBackdropBrush());
                                state.sprite.Clip(compositor.CreateGeometricClip(state.geometry));
                                state.root.Children().InsertAtBottom(state.sprite);
                                state.shade = compositor.CreateSpriteVisual();
                                state.shade.Clip(compositor.CreateGeometricClip(state.geometry));
                                state.shadeBrush = compositor.CreateColorBrush();
                                state.shade.Brush(state.shadeBrush);
                                state.root.Children().InsertAtTop(state.shade);
                                state.target.Root(state.root);
                                found = slot;
                            }
                            auto &state = found->second;
                            const auto &shape = request.shape;
                            if (request.feature == Popups && request.fade.serial != state.fadeSerial) {
                                const auto &fade = request.fade;
                                const ULONGLONG now = GetTickCount64();
                                const auto elapsed = now - fade.start;
                                state.root.StopAnimation(L"Opacity");
                                state.root.Opacity(fade.to);
                                if (fade.duration && elapsed < fade.duration) {
                                    auto animation = compositor.CreateScalarKeyFrameAnimation();
                                    auto linear = compositor.CreateLinearEasingFunction();
                                    animation.InsertKeyFrame(0, FadeValue(fade, now), linear);
                                    animation.InsertKeyFrame(1, fade.to, linear);
                                    animation.Duration(std::chrono::milliseconds(fade.duration - elapsed));
                                    state.root.StartAnimation(L"Opacity", animation);
                                }
                                state.fadeSerial = fade.serial;
                            }
                            state.sprite.Offset(
                                {shape.left * shape.scale, shape.top * shape.scale, 0});
                            state.sprite.Size(
                                {shape.width * shape.scale, shape.height * shape.scale});
                            state.shade.Offset(
                                {shape.left * shape.scale, shape.top * shape.scale, 0});
                            state.shade.Size(
                                {shape.width * shape.scale, shape.height * shape.scale});
                            const unsigned tint = request.tint;
                            state.shadeBrush.Color(winrt::Windows::UI::Color{
                                static_cast<BYTE>(tint >> 24), static_cast<BYTE>(tint >> 16),
                                static_cast<BYTE>(tint >> 8), static_cast<BYTE>(tint)});
                            state.geometry.Size(
                                {shape.width * shape.scale, shape.height * shape.scale});
                            state.geometry.CornerRadius(
                                {shape.rx * shape.scale, shape.ry * shape.scale});
                            applied = true;
                        } catch (const winrt::hresult_error &failure) {
                            Wh_Log(L"Popup backdrop unavailable: 0x%08X",
                                   static_cast<unsigned>(failure.code()));
                            Destroy(hwnd);
                        }
                    else
                        Destroy(hwnd);
                    AcquireSRWLockExclusive(&lock);
                    const auto current = requests.find(hwnd);
                    bool repaint = false;
                    if (current != requests.end() && Equal(current->second.shape, request.shape) &&
                        current->second.tint == request.tint) {
                        repaint = current->second.applied != applied;
                        current->second.applied = applied;
                    }
                    ReleaseSRWLockExclusive(&lock);
                    if (repaint)
                        InvalidateRect(hwnd, nullptr, FALSE);
                }
            } else {
                TranslateMessage(&message);
                DispatchMessageW(&message);
            }
        }
    } catch (const winrt::hresult_error &failure) {
        Wh_Log(L"Popup compositor unavailable: 0x%08X", static_cast<unsigned>(failure.code()));
    } catch (...) {
        Wh_Log(L"Popup compositor unavailable");
    }
    AcquireSRWLockExclusive(&lock);
    workerReady = false;
    workerFinished = true;
    requests.clear();
    ReleaseSRWLockExclusive(&lock);
    PopupTransition::Clear(false);
    SetEvent(startEvent);
    if (events)
        UnhookWinEvent(events);
    if (settingsWindow)
        DestroyWindow(settingsWindow);
    if (settingsClass)
        UnregisterClassW(kSettingsClass, instance);
    while (!states.empty())
        Destroy(states.begin()->first);
    live = nullptr;
    try {
        if (compositor)
            compositor.Close();
    } catch (...) {
    }
    compositor = nullptr;
    // This queue belongs only to this worker. Pump its asynchronous shutdown
    // to completion before releasing the callback or allowing DLL unload.
    if (queue)
        try {
            auto shutdown = queue.ShutdownQueueAsync();
            while (shutdown.Status() == winrt::Windows::Foundation::AsyncStatus::Started) {
                MSG message{};
                while (PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE)) {
                    TranslateMessage(&message);
                    DispatchMessageW(&message);
                }
                MsgWaitForMultipleObjects(0, nullptr, FALSE, 10, QS_ALLINPUT);
            }
            shutdown.GetResults();
        } catch (...) {
        }
    queue = nullptr;
    if (messaging)
        FreeLibrary(messaging);
    if (SUCCEEDED(apartment))
        RoUninitialize();
    return 0;
}
static bool Start() {
    AcquireSRWLockExclusive(&lock);
    if (stopping.load()) {
        ReleaseSRWLockExclusive(&lock);
        return false;
    }
    if (!thread) {
        startEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
        if (startEvent)
            thread = CreateThread(nullptr, 0, Run, nullptr, 0, &threadId);
        if (!thread && startEvent) {
            CloseHandle(startEvent);
            startEvent = nullptr;
        }
    }
    const bool started = CanQueueLocked();
    ReleaseSRWLockExclusive(&lock);
    return started;
}
static bool CanFade() {
    AcquireSRWLockShared(&lock);
    const bool result = workerReady && CanQueueLocked();
    ReleaseSRWLockShared(&lock);
    return result;
}
static bool Geometry(const RoundRect &input, HWND hwnd, Shape *out) {
    const float values[]{input.left,     input.top,      input.right,    input.bottom,
                         input.radii[0], input.radii[1], input.radii[2], input.radii[3],
                         input.radii[4], input.radii[5], input.radii[6], input.radii[7]};
    for (float value : values)
        if (!std::isfinite(value) || value < 0 || value > 32768)
            return false;
    for (unsigned i = 2; i < 8; ++i)
        if (input.radii[i] != input.radii[i % 2])
            return false;
    const float scale = GetDpiForWindow(hwnd) / 96.f;
    RECT bounds{};
    if (!GetClientRect(hwnd, &bounds) || scale <= 0)
        return false;
    const float width = input.right - input.left, height = input.bottom - input.top;
    if (width * scale < bounds.right * .6f || height * scale < bounds.bottom * .6f ||
        input.right * scale > bounds.right + 2 || input.bottom * scale > bounds.bottom + 2 ||
        input.radii[0] > width / 2 || input.radii[1] > height / 2)
        return false;
    *out = {input.left, input.top, width, height, input.radii[0], input.radii[1], scale};
    return true;
}
static void Update(HWND hwnd, const RoundRect &input, unsigned feature) {
    Shape shape{};
    if (!Geometry(input, hwnd, &shape) || !Start())
        return;
    // Initialization is asynchronous: the first paint stays fully native.
    // No UI thread waits for a compositor or a cross-thread window call.
    AcquireSRWLockExclusive(&lock);
    if (CanQueueLocked()) {
        auto &request = requests[hwnd];
        const unsigned tint = feature == Popups && popupTintKnown ? Tint(popupTint, true) : 0;
        if (!request.valid || !Equal(request.shape, shape) || request.feature != feature ||
            request.tint != tint) {
            request.shape = shape;
            request.feature = feature;
            request.tint = tint;
            request.valid = true;
            request.pending = true;
            NotifyLocked();
        }
    }
    ReleaseSRWLockExclusive(&lock);
}
static unsigned long long FadeBegin(HWND hwnd, bool visible) {
    unsigned long long serial = 0;
    AcquireSRWLockExclusive(&lock);
    if (workerReady && CanQueueLocked()) {
        auto &request = requests[hwnd];
        const auto now = GetTickCount64();
        const float from = request.fade.serial ? FadeValue(request.fade, now) : (visible ? 0.f : 1.f);
        serial = request.fade.serial + 1;
        request.fade = {serial, now, from, visible ? 1.f : 0.f, PopupTransition::kFadeMs};
        if (request.valid && !request.pending)
            NotifyLocked();
        request.pending = true;
    }
    ReleaseSRWLockExclusive(&lock);
    return serial;
}
static void FadeImmediate(HWND hwnd, unsigned long long serial) {
    AcquireSRWLockExclusive(&lock);
    const auto found = requests.find(hwnd);
    if (!stopping.load() && found != requests.end() && found->second.fade.serial == serial) {
        found->second.fade.duration = 0;
        ++found->second.fade.serial;
        if (!found->second.pending)
            NotifyLocked();
        found->second.pending = true;
    }
    ReleaseSRWLockExclusive(&lock);
}
static void Forget(HWND hwnd) {
    AcquireSRWLockExclusive(&lock);
    const auto found = requests.find(hwnd);
    if (found != requests.end()) {
        found->second.valid = false;
        found->second.pending = true;
        NotifyLocked();
    }
    ReleaseSRWLockExclusive(&lock);
}
static void Stop() {
    AcquireSRWLockExclusive(&lock);
    stopping = true;
    const HANDLE worker = thread;
    const DWORD id = threadId;
    ReleaseSRWLockExclusive(&lock);
    if (worker) {
        WaitForSingleObject(startEvent, INFINITE);
        PostThreadMessageW(id, kStop, 0, 0);
        WaitForSingleObject(worker, INFINITE);
        CloseHandle(worker);
    }
    AcquireSRWLockExclusive(&lock);
    requests.clear();
    thread = nullptr;
    threadId = 0;
    if (startEvent) {
        CloseHandle(startEvent);
        startEvent = nullptr;
    }
    ReleaseSRWLockExclusive(&lock);
}
} // namespace Material
namespace PopupTransition {
static bool CanFade() { return Material::CanFade(); }
static unsigned long long Begin(HWND hwnd, bool visible) { return Material::FadeBegin(hwnd, visible); }
static void Immediate(HWND hwnd, unsigned long long serial) { Material::FadeImmediate(hwnd, serial); }
} // namespace PopupTransition
static thread_local unsigned paintFeature = 0;
static void RoundRectHook(void *canvas, const RoundRect &shape, const void *flags) {
    if (backgroundWindow && Active(paintFeature))
        Material::Update(backgroundWindow, shape, paintFeature);
    Material::Shape bounds{};
    if (paintFeature == Popups && backgroundWindow && popupTintKnown && Active(Popups) &&
        Material::Geometry(shape, backgroundWindow, &bounds))
        return;
    originalRoundRect(canvas, shape, flags);
}
static unsigned ColorHook(const void *provider, int id) {
    const auto color = originalColor(provider, id);
    if (backgroundScope && paintFeature == Popups) {
        popupTint = color;
        popupTintKnown = true;
    }
    return Tint(color, ready.load(std::memory_order_relaxed) && backgroundScope != 0);
}
static HWND Candidate(const void *view, unsigned feature) {
    if (!view || !Active(feature))
        return nullptr;
    HWND hwnd = windowForView(view);
    return PopupWindow(hwnd) && UserAllowsTransparency() ? hwnd : nullptr;
}
// Suppress only known native backing roles inside a shaped popup. Foreground
// paint remains native, including cross-fading text and fading icon layers.
static bool SuppressPopupBackground(void *view) {
    if (!view || !Active(Popups) || !s_background.getClass)
        return false;
    BraveBackground::ClassName name{};
    s_background.getClass(view, &name);
    if (!name.data || name.length > 127)
        return false;
    const std::string_view cls(name.data, name.length);
    const bool hover = cls == "TabHoverCardBubbleView";
    const bool extensions = cls == "ExtensionsMenuView" || cls == "ExtensionsMenuMainPageView" ||
                            cls == "ExtensionsMenuSitePermissionsPageView";
    const bool pageInfo = cls == "BravePageInfoBubbleView" || cls == "PageInfoBubbleView" ||
                          cls == "InternalPageInfoBubbleView" || cls == "PageInfoMainView";
    const bool downloads = cls == "DownloadBubbleContentsView" || cls == "DownloadDialogView" ||
                           cls == "DownloadBubblePrimaryView" || cls == "DownloadBubblePartialView";
    const bool media = cls == "MediaDialogView" || cls == "MediaItemUIListView" ||
                       cls == "MediaItemUIUpdatedView" || cls == "MediaItemUIView";
    const bool scrollBacking = cls == "ScrollView" || cls == "ScrollView::Viewport";
    const bool pageSwitcher = cls == "PageSwitcherView";
    // ScrollView paints its own background, then repeats it on its viewport
    // and (with layered scrolling) its contents. These containers are cleared
    // only after this popup identifies itself as the native Extensions menu.
    // Extension HTML popups and generic containers in other bubbles stay native.
    const bool extensionBacking = cls == "ScrollView" || cls == "ScrollView::Viewport" ||
                                  cls == "View" || cls == "BoxLayoutView" ||
                                  cls == "FlexLayoutView" || cls == "SectionContainer";
    if (!hover && !extensions && !pageInfo && !downloads && !media && !pageSwitcher &&
        !extensionBacking && cls != "DialogClientView" &&
        cls != "FooterView" && cls != "ImageView" && cls != "FadeLabel")
        return false;
    const auto hwnd = NativeUi::Candidate(view, NativeUi::Popups);
    if (!hwnd)
        return false;
    AcquireSRWLockExclusive(&NativeUi::Material::lock);
    const auto found = NativeUi::Material::requests.find(hwnd);
    const bool shaped = found != NativeUi::Material::requests.end() && found->second.valid &&
                        found->second.feature == NativeUi::Popups;
    if (shaped && hover)
        found->second.hoverCard = true;
    if (shaped && extensions)
        found->second.extensionsMenu = true;
    if (shaped && pageInfo)
        found->second.pageInfo = true;
    if (shaped && downloads)
        found->second.downloads = true;
    if (shaped && media)
        found->second.media = true;
    const bool clear = shaped && !hover &&
                       (pageSwitcher ? found->second.pageInfo
                        : extensionBacking ? (found->second.extensionsMenu ||
                                              ((cls == "View" || cls == "FlexLayoutView") &&
                                               found->second.downloads) ||
                                              (scrollBacking && (found->second.downloads ||
                                                                 found->second.pageInfo ||
                                                                 found->second.media)))
                                         : ((cls != "ImageView" && cls != "FadeLabel") ||
                                            found->second.hoverCard));
    ReleaseSRWLockExclusive(&NativeUi::Material::lock);
    return clear;
}
static void (*originalBubbleTheme)(void *) = nullptr;
static void (*originalLayerOpaque)(void *, bool) = nullptr;
static void (*originalFadeBackground)(void *, void *) = nullptr;
static void FadeBackgroundHook(void *view, void *canvas) {
    if (canvas && SuppressPopupBackground(view))
        return;
    originalFadeBackground(view, canvas);
}
static thread_local bool bubbleThemeUpdating = false;
static void LayerOpaqueHook(void *layer, bool opaque) {
    originalLayerOpaque(layer, bubbleThemeUpdating ? false : opaque);
}
static void BubbleThemeHook(void *view) {
    const auto hwnd = NativeUi::Candidate(view, NativeUi::Popups);
    const bool previous = bubbleThemeUpdating;
    bubbleThemeUpdating = hwnd != nullptr;
    PopupTransition::Mark(hwnd);
    originalBubbleTheme(view);
    bubbleThemeUpdating = previous;
}

static void MenuHook(void *view, void *canvas) {
    HWND hwnd = Candidate(view, Menus);
    const auto previous = paintFeature;
    paintFeature = Menus;
    // Chromium retains the first display list and later repaints only damaged
    // rows. Its initial background must already carry alpha; HWND invalidation
    // after our asynchronous backdrop attaches cannot repaint retained Views.
    Scope scope(hwnd != nullptr, hwnd);
    originalMenu(view, canvas);
    paintFeature = previous;
}
static void BubbleHook(const void *background, void *canvas, void *view) {
    BraveBackground::ClassName role{};
    s_background.getClass(view, &role);
    if (!role.data || std::string_view(role.data, role.length) != "BubbleFrameView") {
        originalBubble(background, canvas, view);
        return;
    }
    HWND hwnd = Candidate(view, Popups);
    const auto previous = paintFeature;
    paintFeature = Popups;
    const auto previousTint = popupTint;
    const bool previousKnown = popupTintKnown;
    popupTintKnown = false;
    Scope scope(hwnd != nullptr, hwnd);
    originalBubble(background, canvas, view);
    paintFeature = previous;
    popupTint = previousTint;
    popupTintKnown = previousKnown;
}

namespace Omnibox {
struct Rect {
    int x, y, width, height;
};
using Constructor = void *(*)(void *, void *, void *, bool);
using Destructor = void (*)(void *);
using SetBackground = void (*)(void *, void *);
using SetRounded = void (*)(void *, const float *, int);
using Bounds = Rect *(*)(const void *, Rect *);
using HeaderHeight = int (*)(bool);
using BeforeWidget = void (*)(void *, void *);
static Constructor originalConstructor = nullptr;
static Destructor originalDestructor = nullptr;
static SetBackground originalBackground = nullptr;
static SetRounded originalRounded = nullptr;
static BackgroundPaint originalSolid = nullptr;
static BeforeWidget originalBeforeWidget = nullptr;
static Bounds getBounds = nullptr, getScreenBounds = nullptr;
static HeaderHeight getHeader = nullptr;
static const void *solidVtable = nullptr;
struct Capture {
    void *frame;
    const void *background = nullptr;
    void *layer = nullptr;
    unsigned backgrounds = 0, layers = 0;
    float radius = 0;
};
struct Record {
    void *frame;
    void *layer;
    HWND window = nullptr;
    Rect sampling{};
    float radius = 0, scale = 0;
    HRGN originalRegion = nullptr;
    bool painted = false;
};
static thread_local Capture *constructing = nullptr;
static SRWLOCK lock = SRWLOCK_INIT;
static std::unordered_map<const void *, Record> backgrounds;
static std::vector<HWND> subclassedWindows;
static constexpr UINT_PTR kSubclass = 0xBA3901;
static UINT cleanupMessage = 0;
static bool SameRect(const Rect &a, const Rect &b) {
    return a.x == b.x && a.y == b.y && a.width == b.width && a.height == b.height;
}
static bool SamplingBounds(const Rect &frame, const Rect &host, int header, Rect *result) {
    if (frame.width <= 0 || frame.width > 32768 || frame.height <= 0 || frame.height > 32768 ||
        host.width <= 0 || host.width > 32768 || host.height <= 0 || host.height > 32768 ||
        header < 0 || header >= host.height)
        return false;
    const int64_t x = int64_t(host.x) - frame.x, y = int64_t(host.y) - frame.y + header;
    if (x < 0 || y < 0 || x + host.width > frame.width || y + host.height - header > frame.height)
        return false;
    *result = {int(x), int(y), host.width, host.height - header};
    return true;
}
static void Restore(const Record &record) {
    // Windows takes ownership of a successfully restored region.
    if (!record.window || !IsWindow(record.window) ||
        !SetWindowRgn(record.window, record.originalRegion, TRUE))
        if (record.originalRegion)
            DeleteObject(record.originalRegion);
    if (record.window)
        Material::Forget(record.window);
}
static void RestoreWindow(HWND hwnd) {
    std::vector<Record> records;
    AcquireSRWLockExclusive(&lock);
    for (auto it = backgrounds.begin(); it != backgrounds.end();)
        if (it->second.window == hwnd) {
            records.push_back(it->second);
            it = backgrounds.erase(it);
        } else
            ++it;
    ReleaseSRWLockExclusive(&lock);
    for (const auto &record : records)
        Restore(record);
}
static LRESULT CALLBACK WindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam, UINT_PTR,
                                   DWORD_PTR) {
    if (message == cleanupMessage || message == WM_NCDESTROY) {
        RestoreWindow(hwnd);
        RemoveWindowSubclass(hwnd, WindowProc, kSubclass);
        AcquireSRWLockExclusive(&lock);
        std::erase(subclassedWindows, hwnd);
        ReleaseSRWLockExclusive(&lock);
    }
    return DefSubclassProc(hwnd, message, wParam, lParam);
}
static void BeforeWidgetHook(void *params, void *widget) {
    if (!Active(Suggestions) || !UserAllowsTransparency())
        originalBeforeWidget(params, widget);
    // Otherwise leave Chromium's desktop-popup factory selected instead of its
    // in-window Aura override. Chromium owns the native popup and all its input.
}
static void BackgroundHook(void *view, void *value) {
    const void *background = nullptr;
    if (constructing && value) {
        memcpy(&background, value, sizeof(void *));
        const void *table = nullptr;
        if (background)
            memcpy(&table, background, sizeof(void *));
        if (table != solidVtable)
            background = nullptr;
    }
    originalBackground(view, value);
    if (background) {
        ++constructing->backgrounds;
        if (constructing->backgrounds == 1)
            constructing->background = background;
    }
}
static bool RoundedShape(const float *radii) {
    if (!radii)
        return false;
    for (unsigned i = 0; i < 4; ++i)
        if (!std::isfinite(radii[i]) || radii[i] < 0 || radii[i] > 128)
            return false;
    return true;
}
static void RoundedHook(void *layer, const float *radii, int reason) {
    originalRounded(layer, radii, reason);
    if (constructing && layer && RoundedShape(radii) && constructing->layer != layer) {
        ++constructing->layers;
        if (constructing->layers == 1) {
            constructing->layer = layer;
            constructing->radius = *std::min_element(radii, radii + 4);
        }
    }
}
static void *ConstructorHook(void *self, void *contents, void *location, bool forward) {
    Capture capture{self};
    Capture *previous = constructing;
    constructing = Active(Suggestions) && UserAllowsTransparency() ? &capture : nullptr;
    void *result = originalConstructor(self, contents, location, forward);
    constructing = previous;
    if (Active(Suggestions) && capture.backgrounds == 1 && capture.layers == 1 &&
        capture.background && capture.layer) {
        AcquireSRWLockExclusive(&lock);
        if (Active(Suggestions))
            backgrounds[capture.background] = {self, capture.layer, nullptr, {}, capture.radius};
        ReleaseSRWLockExclusive(&lock);
    }
    return result;
}
static void Forget(void *frame) {
    std::vector<Record> records;
    AcquireSRWLockExclusive(&lock);
    for (auto it = backgrounds.begin(); it != backgrounds.end();)
        if (it->second.frame == frame) {
            records.push_back(it->second);
            it = backgrounds.erase(it);
        } else
            ++it;
    ReleaseSRWLockExclusive(&lock);
    for (const auto &record : records)
        Restore(record);
}
static void DestructorHook(void *self) {
    Forget(self);
    originalDestructor(self);
}
static bool Blur(const void *background, void *view) {
    Record record{};
    AcquireSRWLockShared(&lock);
    const auto found = backgrounds.find(background);
    if (found != backgrounds.end())
        record = found->second;
    ReleaseSRWLockShared(&lock);
    if (!record.layer || !view || !Active(Suggestions) || !UserAllowsTransparency())
        return false;
    HWND hwnd = windowForView(view);
    if (!PopupWindow(hwnd) || GetWindowThreadProcessId(hwnd, nullptr) != GetCurrentThreadId())
        return false;
    Rect frame{}, host{}, local{}, sampling{};
    getBounds(view, &local);
    getScreenBounds(record.frame, &frame);
    getScreenBounds(view, &host);
    if (local.width != host.width || local.height != host.height ||
        !SamplingBounds(frame, host, getHeader(true), &sampling))
        return false;
    RECT client{};
    const float scale = GetDpiForWindow(hwnd) / 96.f;
    if (!GetClientRect(hwnd, &client) || scale <= 0 || scale > 8 ||
        (sampling.x + sampling.width) * scale > client.right + 2 ||
        (sampling.y + sampling.height) * scale > client.bottom + 2)
        return false;
    if (!record.window) {
        HRGN saved = CreateRectRgn(0, 0, 0, 0);
        if (!saved)
            return false;
        if (GetWindowRgn(hwnd, saved) == ERROR) {
            DeleteObject(saved);
            saved = nullptr;
        }
        if (!SetWindowSubclass(hwnd, WindowProc, kSubclass, 0)) {
            if (saved)
                DeleteObject(saved);
            return false;
        }
        AcquireSRWLockExclusive(&lock);
        const auto current = backgrounds.find(background);
        const bool keep = Active(Suggestions) && current != backgrounds.end() &&
                          current->second.frame == record.frame;
        if (keep) {
            current->second.window = hwnd;
            current->second.originalRegion = saved;
            if (std::find(subclassedWindows.begin(), subclassedWindows.end(), hwnd) ==
                subclassedWindows.end())
                subclassedWindows.push_back(hwnd);
        }
        ReleaseSRWLockExclusive(&lock);
        if (!keep) {
            RemoveWindowSubclass(hwnd, WindowProc, kSubclass);
            if (saved)
                DeleteObject(saved);
            return false;
        }
        record.window = hwnd;
        record.originalRegion = saved;
    }
    const float radius = std::min(record.radius, std::min(sampling.width, sampling.height) / 2.f);
    if (!record.painted || !SameRect(record.sampling, sampling) || record.scale != scale) {
        // Exclude the actual input field and shadow margins from the popup's
        // rounded OS region. Pointer input goes directly to the native controls.
        HRGN region = CreateRoundRectRgn(int(sampling.x * scale), int(sampling.y * scale),
                                         int((sampling.x + sampling.width) * scale) + 1,
                                         int((sampling.y + sampling.height) * scale) + 1,
                                         int(radius * scale * 2), int(radius * scale * 2));
        if (!region)
            return false;
        if (!SetWindowRgn(hwnd, region, TRUE)) {
            DeleteObject(region);
            return false;
        }
    }
    if (!Material::Start())
        return false;
    Material::Shape shape{float(sampling.x),
                          float(sampling.y),
                          float(sampling.width),
                          float(sampling.height),
                          radius,
                          radius,
                          scale};
    AcquireSRWLockExclusive(&Material::lock);
    if (Material::CanQueueLocked()) {
        auto &request = Material::requests[hwnd];
        if (!request.valid || !Material::Equal(request.shape, shape) ||
            request.feature != Suggestions) {
            request.shape = shape;
            request.feature = Suggestions;
            request.tint = 0;
            request.valid = true;
            request.pending = true;
            Material::NotifyLocked();
        }
    }
    ReleaseSRWLockExclusive(&Material::lock);
    AcquireSRWLockExclusive(&lock);
    const auto current = backgrounds.find(background);
    if (current != backgrounds.end()) {
        current->second.sampling = sampling;
        current->second.scale = scale;
        current->second.painted = true;
    }
    ReleaseSRWLockExclusive(&lock);
    return true;
}
static void SolidHook(const void *background, void *canvas, void *view) {
    Scope scope(Blur(background, view));
    originalSolid(background, canvas, view);
}
static void Stop() {
    std::vector<HWND> windows;
    AcquireSRWLockShared(&lock);
    windows = subclassedWindows;
    ReleaseSRWLockShared(&lock);
    // Do not hold a registry lock across dispatch to the owning UI thread.
    for (HWND hwnd : windows)
        SendMessageW(hwnd, cleanupMessage, 0, 0);
    AcquireSRWLockExclusive(&lock);
    backgrounds.clear();
    ReleaseSRWLockExclusive(&lock);
}
} // namespace Omnibox

static void Stop() {
    ready = false;
    Omnibox::Stop();
    PopupTransition::Clear(true);
    Material::Stop();
}
} // namespace NativeUi
// </native-ui>

static void InstallNativeBackgroundHooks(HMODULE module, const size_t *rva) {
    unsigned features = CompleteFeatures(rva);
    const unsigned optional = CompleteOptionalFeatures(rva, features);
    auto *base = reinterpret_cast<unsigned char *>(module);
    const auto address = [&](unsigned i) -> void * { return rva[i] ? base + rva[i] : nullptr; };
    const auto hook = [&]<typename Function>(unsigned slot, Function callback, Function *original) {
        return address(slot) && WindhawkUtils::SetFunctionHook(
                                    reinterpret_cast<Function>(address(slot)), callback, original);
    };
    s_background.clear = reinterpret_cast<BraveBackground::Clear>(address(1));
    s_background.getClass = reinterpret_cast<BraveBackground::GetClass>(address(2));
    s_background.setBackground = reinterpret_cast<BraveBackground::SetBackground>(address(5));
    s_locationFocus = reinterpret_cast<NativeFocus>(address(6));
    s_viewFocus = reinterpret_cast<NativeFocus>(address(7));
    s_setPageBase = reinterpret_cast<NativeSetPageBase>(address(11));
    constexpr unsigned windowFeatures = BraveBackground::All;
    if ((features & (windowFeatures | kPopups)) &&
        !hook(0, ViewOnPaintBackground_Hook, &s_background.originalPaint))
        features &= ~(windowFeatures | kPopups);
    if ((features & windowFeatures) &&
        !hook(3, WindowContentsTransparent_Hook, &s_background.originalTransparent))
        features &= ~windowFeatures;
    if ((features & kAddressBar) && (optional & kAddressShadow))
        hook(36, ConstructShadowHook, &s_originalShadow);
    if ((features & kVertical) &&
        !hook(4, SidebarConstructor_Hook, &s_background.originalConstruct))
        features &= ~kVertical;
    if ((features & kWebpage) &&
        (!hook(8, PageThemeHook, &s_originalTheme) || !hook(9, PageReadyHook, &s_originalReady) ||
         !hook(10, PageLayerColorHook, &s_originalLayerColor) ||
         !hook(12, AttachContentsHook, &s_originalAttachContents)))
        features &= ~kWebpage;
    if (!(features & kWebpage) ||
        ((features & kRoundedWebpage) &&
         (!hook(30, RoundedContents::BorderHook, &RoundedContents::originalBorder) ||
          !hook(31, RoundedContents::PathHook, &RoundedContents::originalPath))))
        features &= ~kRoundedWebpage;
    RoundedContents::available = (features & kRoundedWebpage) != 0;
    NativeUi::windowForView = reinterpret_cast<NativeUi::WindowForView>(address(13));
    if ((features & (kMenus | kPopups | kSuggestions)) &&
        !hook(14, NativeUi::ColorHook, &NativeUi::originalColor))
        features &= ~(kMenus | kPopups | kSuggestions);
    if ((features & (kMenus | kPopups)) &&
        !hook(17, NativeUi::RoundRectHook, &NativeUi::originalRoundRect))
        features &= ~(kMenus | kPopups);
    if ((features & kMenus) && !hook(15, NativeUi::MenuHook, &NativeUi::originalMenu))
        features &= ~kMenus;
    if ((features & kPopups) &&
        (!hook(16, NativeUi::BubbleHook, &NativeUi::originalBubble) ||
         !hook(25, NativeUi::BubbleThemeHook, &NativeUi::originalBubbleTheme) ||
         !hook(26, NativeUi::LayerOpaqueHook, &NativeUi::originalLayerOpaque) ||
         !hook(27, NativeUi::FadeBackgroundHook, &NativeUi::originalFadeBackground)))
        features &= ~kPopups;
    if ((features & kPopups) && (optional & kPopupFade)) {
        using namespace NativeUi::PopupTransition;
        windowForNative = reinterpret_cast<WindowForNative>(address(33));
        setType = reinterpret_cast<SetType>(address(34));
        setDuration = reinterpret_cast<SetDuration>(address(35));
        hook(32, AnimateHook, &original);
    }
    if (features & kSuggestions) {
        using namespace NativeUi::Omnibox;
        getBounds = reinterpret_cast<Bounds>(address(22));
        getHeader = reinterpret_cast<HeaderHeight>(address(23));
        solidVtable = address(24);
        getScreenBounds = reinterpret_cast<Bounds>(address(28));
        cleanupMessage = RegisterWindowMessageW(L"BraveAcrylic.OmniboxCleanup.3.9");
        if (!cleanupMessage || !hook(5, BackgroundHook, &originalBackground) ||
            !hook(18, ConstructorHook, &originalConstructor) ||
            !hook(19, DestructorHook, &originalDestructor) ||
            !hook(20, RoundedHook, &originalRounded) || !hook(21, SolidHook, &originalSolid) ||
            !hook(29, BeforeWidgetHook, &originalBeforeWidget))
            features &= ~kSuggestions;
    }
    NativeUi::supported = features;
    NativeUi::enabled = s_requestedUi.load();
    // Start the settings listener before the first popup, without waiting on
    // Brave's UI/loader thread. It receives setting changes even while idle.
    if (features & s_requestedUi.load())
        NativeUi::Material::Start();
    s_availableAreas = features & windowFeatures;
    s_background.areas.fetch_and(features & windowFeatures);
    const wchar_t *labels[] = {L"Vertical tabs", L"Toolbar", L"Webpage background",
                               L"Menus",         L"Popups",  L"Search suggestions",
                               L"Rounded webpage backing", L"Address bar"};
    for (unsigned i = 0; i < ARRAYSIZE(labels); ++i)
        Wh_Log(L"Brave Acrylic %s: %s %s", WH_MOD_VERSION, labels[i],
               (features & (1u << i)) ? L"available" : L"unavailable; left native");
}

static constexpr DWORD kBraveSymbolStartupWaitMs = 5000;
static CONDITION_VARIABLE s_symbolLookupChanged = CONDITION_VARIABLE_INIT;
static HANDLE s_symbolWorker = nullptr;
struct SymbolLookup {
    HMODULE module;
    bool useOnlineCache;
    bool finished;
    bool succeeded;
    ULONGLONG completedAt;
    size_t rva[kFeatureSlots];
};
static SymbolLookup s_symbolLookup{};

static DWORD WINAPI ResolveBraveSymbolsOnWorker(void *) {
    size_t rva[kFeatureSlots]{};
    HMODULE retainedModule = nullptr;
    const HMODULE module = s_symbolLookup.module;
    bool succeeded = GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS,
                                       reinterpret_cast<LPCWSTR>(module), &retainedModule) &&
                     ResolveFeatureAddresses(module, rva, s_symbolLookup.useOnlineCache, true);
    if (retainedModule)
        FreeLibrary(retainedModule);

    // Publish data only. After a timeout this lookup merely fills Windhawk's
    // cache for the next launch, even if all addresses are valid.
    AcquireSRWLockExclusive(&s_initLock);
    std::copy_n(rva, kFeatureSlots, s_symbolLookup.rva);
    s_symbolLookup.succeeded = succeeded;
    s_symbolLookup.finished = true;
    s_symbolLookup.completedAt = GetTickCount64();
    WakeAllConditionVariable(&s_symbolLookupChanged);
    ReleaseSRWLockExclusive(&s_initLock);
    return 0;
}

// Called with s_initLock held. Waiting releases it so disabling the mod can
// close the activation gate before waiting for the uncancellable resolver.
static bool ResolveBraveSymbolsWithStartupBudget(HMODULE module, size_t *rva,
                                                bool useOnlineCache, DWORD waitMs) {
    const ULONGLONG deadline = GetTickCount64() + waitMs;
    s_symbolLookup.module = module;
    s_symbolLookup.useOnlineCache = useOnlineCache;
    s_symbolWorker = CreateThread(nullptr, 0, ResolveBraveSymbolsOnWorker, nullptr, 0, nullptr);
    if (!s_symbolWorker) {
        Wh_Log(L"Could not start Brave symbol lookup; leaving Brave unmodified");
        return false;
    }

    while (!s_symbolLookup.finished && !s_stopping) {
        const ULONGLONG now = GetTickCount64();
        if (now >= deadline ||
            !SleepConditionVariableSRW(&s_symbolLookupChanged, &s_initLock,
                                       static_cast<DWORD>(deadline - now), 0)) {
            break;
        }
    }
    if (s_stopping)
        return false;
    if (!s_symbolLookup.finished || s_symbolLookup.completedAt > deadline) {
        Wh_Log(L"Brave continued without acrylic while symbols finish loading; restart afterward");
        return false;
    }
    if (!s_symbolLookup.succeeded)
        return false;
    std::copy_n(s_symbolLookup.rva, kFeatureSlots, rva);
    return true;
}

// Installation and shutdown share a barrier. No hook can be registered/applied
// after Wh_ModBeforeUninit returns, including a loader call that finishes late.
static bool TryInstallChromeHooks(HMODULE module, bool applyNow,
                                  DWORD waitMs = kBraveSymbolStartupWaitMs) {
    if (!module || s_chromeAttempted.exchange(true, std::memory_order_acq_rel))
        return false;
    AcquireSRWLockExclusive(&s_initLock);
    bool installed = false;
    size_t rva[kFeatureSlots]{};
    const bool useOnlineCache = s_useOnlineSymbolCache.load();
    const bool allowFallback = s_allowBraveSymbolServerFallback.load();
    const bool resolved = !s_stopping &&
        (allowFallback ? ResolveBraveSymbolsWithStartupBudget(module, rva, useOnlineCache, waitMs)
                       : ResolveFeatureAddresses(module, rva, useOnlineCache, false));
    if (resolved && !s_stopping) {
        InstallNativeBackgroundHooks(module, rva);
        if (applyNow) {
            installed = Wh_ApplyHookOperations() != FALSE;
            s_transparencyReady.store(installed && HasCompleteTransparencyPath(),
                                      std::memory_order_release);
            NativeUi::ready.store(installed, std::memory_order_release);
            if (!installed)
                Wh_Log(L"Could not activate paint hooks; transparency stays off");
        } else {
            // Windhawk applies these hooks after Wh_ModInit returns.
            s_activateAfterInit = true;
            installed = true;
        }
    }
    ReleaseSRWLockExclusive(&s_initLock);
    if (applyNow && installed)
        SyncNativeCaptions();
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
    auto *arguments = CommandLineToArgvW(GetCommandLineW(), &count);
    if (!arguments)
        return ProcessRole::Other;
    ProcessRole role = ProcessRole::Browser;
    for (int i = 1; i < count; ++i) {
        if (!wcscmp(arguments[i], L"--") || !wcscmp(arguments[i], L"--single-argument"))
            break;
        if (!wcsncmp(arguments[i], L"--type=", 7)) {
            role =
                !wcscmp(arguments[i] + 7, L"gpu-process") ? ProcessRole::Gpu : ProcessRole::Other;
        }
    }
    LocalFree(arguments);
    return role;
}

BOOL Wh_ModInit() {
    const auto role = GetProcessRole();
    if (role != ProcessRole::Browser)
        return FALSE;
    s_isBrowserProcess = true;
    NativeUi::RefreshTransparencyAllowed();
    LoadModSettings();
    InstallNativeCaptionHooks();

    HMODULE chrome = GetModuleHandleW(L"chrome.dll");
    if (chrome) {
        // The opt-in PDB lookup belongs to AfterInit when chrome.dll is
        // already loaded. Do not hold Windhawk's initialization on it.
        return s_allowBraveSymbolServerFallback.load() || TryInstallChromeHooks(chrome, false);
    }

    const HMODULE kernelBase = GetModuleHandleW(L"kernelbase.dll");
    const auto loader =
        kernelBase
            ? reinterpret_cast<LoadLibraryExW_t>(GetProcAddress(kernelBase, "LoadLibraryExW"))
            : nullptr;
    if (!loader ||
        !WindhawkUtils::SetFunctionHook(loader, LoadLibraryExW_Hook, &pOrigLoadLibraryExW)) {
        return FALSE;
    }
    return TRUE;
}

void Wh_ModAfterInit() {
    if (!s_isBrowserProcess)
        return;
    AcquireSRWLockExclusive(&s_initLock);
    if (!s_stopping && s_activateAfterInit) {
        s_activateAfterInit = false;
        s_transparencyReady.store(HasCompleteTransparencyPath(), std::memory_order_release);
        NativeUi::ready.store(true, std::memory_order_release);
    }
    ReleaseSRWLockExclusive(&s_initLock);
    // Cover a load between ModInit's check and activation of the loader hook.
    if (const auto chrome = GetModuleHandleW(L"chrome.dll"))
        TryInstallChromeHooks(chrome, true);
    SyncNativeCaptions();
}

void Wh_ModSettingsChanged() {
    if (!s_isBrowserProcess)
        return;
    LoadModSettings();
    SyncNativeCaptions();
    Wh_Log(L"Settings updated; restart Brave normally to rebuild its cached UI brushes");
}

void Wh_ModBeforeUninit() {
    s_captionStopping.store(true, std::memory_order_release);
    s_captionFixActive.store(false, std::memory_order_release);
    AcquireSRWLockExclusive(&s_initLock);
    s_stopping = true;
    s_activateAfterInit = false;
    s_transparencyReady.store(false, std::memory_order_release);
    const HANDLE symbolWorker = s_symbolWorker;
    s_symbolWorker = nullptr;
    WakeAllConditionVariable(&s_symbolLookupChanged);
    ReleaseSRWLockExclusive(&s_initLock);
    NativeUi::Stop();
    // Restore the real WS_SYSMENU bit while the getter/setter hooks are still
    // installed, so Chromium cannot save a stripped style during teardown.
    if (s_isBrowserProcess)
        EnumWindows(SyncNativeCaptionWindow, 0);
    // Windhawk's symbol lookup cannot be cancelled. The worker must return
    // before this mod's DLL can be unloaded.
    if (symbolWorker) {
        WaitForSingleObject(symbolWorker, INFINITE);
        CloseHandle(symbolWorker);
    }
}

#endif // x64 native background ABI
