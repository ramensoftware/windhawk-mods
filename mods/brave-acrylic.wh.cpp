// ==WindhawkMod==
// @id              brave-acrylic
// @name            Brave Acrylic
// @description     Alpha: Native Brave background transparency; requires Translucent Windows
// @version         3.3
// @author          bardelyne
// @github          https://github.com/bardelyne
// @homepage        https://github.com/bardelyne/brave-acrylic
// @license         MIT
// @architecture    x86-64
// @include         brave.exe
// @compilerOptions -lshell32 -luser32 -lwinhttp
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
| **Hide duplicate Windows buttons** | On | Hide the Windows caption-button layer underneath Brave's own controls. |
| **Download compatibility data after Brave updates** | On | Download a small compatibility file only when the exact Brave build has no valid local data. Turn off to prevent these downloads. |

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

The map contains **13 native functions**, of which **7 are private hooks**. The implementation uses no sampled colors, private object-member offsets, guessed screen rectangles, screenshot polling, extra backing windows, or GPU hooks. Normal DirectComposition stays enabled, with no special browser launch flag. Windows supplies one backdrop. Acrylic can turn gray when inactive; choose a suitable blur mode in Translucent Windows if you prefer a persistent effect.

## Updates and offline use

Every launch validates the loaded DLL's identity, architecture, function boundaries, and fingerprints. Direct compiler jump stubs can be resolved for any slot, retaining strict validation of their target. Built-in or cached data works offline. Data for another build or older map format is rejected.

For an unknown build, the optional worker requests a **1,080-byte** file from this repository's `compatibility-v6` branch. Missing or invalid data is retried **every 30 minutes** while Brave remains running and downloads are enabled, stopping after a valid file is cached. The worker never changes live hooks; restart Brave normally to use newly downloaded data. Known builds make no compatibility request. The mod downloads no PDBs, executable code, or mod updates.

The download setting is an opt-out for users who want no compatibility network requests. Leave it on for automatic recovery when new exact-build data is available; turn it off to use only built-in or already cached data. It cannot adapt to incompatible native ABI changes.

The repository workflow checks official stable releases **every two hours**. It downloads the browser and symbols only for new or changed assets or a changed compatibility contract. Cosmetic documentation changes skip heavy downloads. GitHub can delay scheduled runs. Workflow changes take effect when pushed to `main`; older client data branches remain separate.

## Troubleshooting

Reopen Brave normally after installation or native-area changes. Enable Windhawk debug logging to check the exact build key, **13/13 native functions**, or a compatibility-cache message. For website styling, check Zen Internet's transparency setting and whether that site has a supported style, or check the CSS and site scope of your Stylus style.

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
- hideNativeCaptionButtons: true
  $name: Hide duplicate Windows buttons
  $description: Keep Brave's own buttons; hide the Windows button layer underneath. Applies immediately.
- downloadCompatibilityData: true
  $name: Download compatibility data after Brave updates
  $description: Download a small compatibility file only when a Brave update has no valid local data. Cached builds work offline. Turn off to prevent these downloads.
*/
// ==/WindhawkModSettings==
#include <windhawk_api.h>
#if !defined(__x86_64__) && !defined(_M_X64)
BOOL Wh_ModInit(){Wh_Log(L"Brave Acrylic requires x64 Brave.");return FALSE;}
#else
#include <shellapi.h>
#include <winhttp.h>
#include <atomic>
#include <cstdio>
#include <cwchar>
#include <cstring>
#include <windhawk_utils.h>

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
static std::atomic<bool> s_chromeAttempted{false}, s_downloadCompatibility{true};
static SRWLOCK s_initLock = SRWLOCK_INIT;
static bool s_stopping = false, s_activateAfterInit = false, s_isBrowserProcess = false;
static bool s_afterInit = false, s_downloadAttempted = false;
static HANDLE s_downloadThread = nullptr, s_downloadStop = nullptr;
static HMODULE s_recoveryModule = nullptr;
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
 return s_background.Complete()&&s_locationFocus&&s_viewFocus&&s_originalTheme&&s_originalReady&&
  s_originalLayerColor&&s_setPageBase&&s_originalAttachContents;
}
static void ViewOnPaintBackground_Hook(void* view, void* canvas) {
    if(canvas&&AddressShouldClear(view)){s_background.clear(canvas,0,0);return;}
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
static std::atomic<bool> s_hideNativeCaptionButtons{true};
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
    // Use the public APIs here: Windhawk has removed the hooks by ModUninit.
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
    if(Wh_GetIntSetting(L"transparentPageBackground"))areas|=BraveBackground::Page;
    s_background.areas.store(areas, std::memory_order_relaxed);
    s_hideNativeCaptionButtons.store(Wh_GetIntSetting(L"hideNativeCaptionButtons") != 0,
                                    std::memory_order_relaxed);
    s_downloadCompatibility.store(Wh_GetIntSetting(L"downloadCompatibilityData") != 0,
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

// Format 6 contains only the thirteen native background dependencies. Legacy maps are not read.
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
struct CompatibilityCache {
    CompatibilityMap map;
    DWORD revision, reserved;
    char braveVersion[64], modVersion[16];
};
static_assert(sizeof(FunctionFingerprint) == 80);
static_assert(sizeof(CompatibilityMap) == 1080);
static_assert(sizeof(CompatibilityCache) == 1168);
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

static void GetBraveVersion(HMODULE module, char* output, size_t size) {
    if (!size) return;
    output[0] = 0;
    wchar_t path[MAX_PATH]{};
    const DWORD length = GetModuleFileNameW(module, path, ARRAYSIZE(path));
    if (!length || length >= ARRAYSIZE(path)) return;
    wchar_t* slash = wcsrchr(path, L'\\');
    if (!slash) return;
    *slash = 0;
    wchar_t* version = wcsrchr(path, L'\\');
    if (!version) return;
    WideCharToMultiByte(CP_UTF8, 0, version + 1, -1, output, static_cast<int>(size), nullptr, nullptr);
}

static bool FocusLeafLayout(const unsigned char* code){
 const unsigned char layout[]={0x48,0x83,0xB9,0,0,0,0,0,0x74,0x13,0x48,0x8B,0x81,0,0,0,0,0x48,0x8B,0x40,0x28,0x83,0x78,0x18,0,0x0F,0x95,0xC0,0xC3,0x31,0xC0,0xC3};
 for(unsigned i=0;i<32;++i)if(!((i>=3&&i<7)||(i>=13&&i<17))&&code[i]!=layout[i])return false;
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
        if (!f.rva || (f.kind!=3 && f.reserved) || f.kind > 3 || f.length > 32 ||
            f.rva < text->VirtualAddress ||
            f.rva - text->VirtualAddress > text->Misc.VirtualSize ||
            sourceLength > text->Misc.VirtualSize - (f.rva - text->VirtualAddress)) return false;
        const DWORD target=f.kind==3?f.reserved:f.rva;
        if(target<text->VirtualAddress||target-text->VirtualAddress>text->Misc.VirtualSize||f.length>text->Misc.VirtualSize-(target-text->VirtualAddress))return false;
        const unsigned char hostPrefix[]={0x48,0x83,0xB9},hostSuffix[]={0,0x74,3,0x31,0xC0,0xC3};
        const bool shortHost=i==3&&f.length==13&&(f.kind==1||f.kind==3);
        const bool shortFocus=i==6&&f.length==32&&(f.kind==2||(f.kind==3&&FocusLeafLayout(f.code)));
        if(f.kind==0||f.kind==3){
            if(shortHost){
                if(memcmp(f.code,hostPrefix,3)||memcmp(f.code+7,hostSuffix,6)||!LeafOutsideUnwind(module,target,f.length))return false;
            }else if(shortFocus){
                if(!FunctionContains(module,target,f.length)&&!LeafOutsideUnwind(module,target,f.length))return false;
            }else if(f.length<16||!FunctionContains(module,target,f.length))return false;
            if(f.kind==3){
                if(f.rva-text->VirtualAddress>text->Misc.VirtualSize-5||image.base[f.rva]!=0xE9||!LeafOutsideUnwind(module,f.rva,5))return false;
                int d;memcpy(&d,image.base+f.rva+1,4);if(int64_t(f.rva)+5+d!=target)return false;
            }
        }else if(f.kind==1){
            const unsigned char prefix[]={0x48,0x83,0xB9},suffix[]={0,0x74,3,0x31,0xC0,0xC3};
            if(i!=3||f.length!=13||memcmp(f.code,prefix,3)||memcmp(f.code+7,suffix,6)||!LeafOutsideUnwind(module,target,f.length))return false;
        }else if(i!=6||f.length!=32||!FocusLeafLayout(f.code)||!LeafOutsideUnwind(module,target,f.length))return false;
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
        if (fixed < (shortHost?13u:shortFocus?32u:16u)) return false;
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
    for (unsigned i = 0; i < kFunctionSlots; ++i) rva[i]=map.functions[i].kind==3?map.functions[i].reserved:map.functions[i].rva;
    return true;
}
static bool LoadCompatibilityCache(HMODULE module, size_t* rva) {
    CompatibilityCache cache{};
    return Wh_GetBinaryValue(L"compatibilityMapV6", &cache, sizeof(cache)) == sizeof(cache) &&
        cache.revision == 1 && !cache.reserved &&
        memchr(cache.braveVersion, 0, sizeof(cache.braveVersion)) &&
        memchr(cache.modVersion, 0, sizeof(cache.modVersion)) &&
        ValidateCompatibilityMap(module, cache.map, rva);
}
// Generated from the official Brave 1.98.60 DLL/PDB; revalidated before use.
static constexpr unsigned char kBuiltinMap[1080] = {
    0x42, 0x41, 0x4D, 0x50, 0x06, 0x00, 0x00, 0x00, 0x6B, 0xDB, 0xD8, 0x75, 0xEC, 0xA8, 0xF6, 0x43,
    0x4C, 0x4C, 0x44, 0x20, 0x50, 0x44, 0x42, 0x2E, 0x01, 0x00, 0x00, 0x00, 0x64, 0x86, 0x00, 0x00,
    0x3B, 0x70, 0xD8, 0x6A, 0x00, 0x80, 0x12, 0x14, 0x40, 0x40, 0x56, 0x00, 0x20, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x56, 0x57, 0x48, 0x83, 0xEC, 0x38, 0x48, 0x89,
    0xCE, 0x48, 0x8B, 0x05, 0xF0, 0x6F, 0xE0, 0x12, 0x48, 0x31, 0xE0, 0x48, 0x89, 0x44, 0x24, 0x30,
    0x48, 0x8B, 0x89, 0x28, 0x02, 0x00, 0x00, 0x48, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x50, 0x85, 0xE5, 0x03, 0x20, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x48, 0x83, 0xEC, 0x38, 0x48, 0x8B, 0x05, 0xE5,
    0x2A, 0x51, 0x0F, 0x48, 0x31, 0xE0, 0x48, 0x89, 0x44, 0x24, 0x30, 0x48, 0x8B, 0x89, 0xC0, 0x00,
    0x00, 0x00, 0x66, 0x0F, 0x6E, 0xC2, 0x66, 0x0F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x10, 0xC2, 0xE2, 0x03, 0x20, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x56, 0x48, 0x83, 0xEC, 0x20, 0x48, 0x89, 0xD6,
    0x48, 0x8B, 0x81, 0x00, 0x01, 0x00, 0x00, 0x48, 0x81, 0xC1, 0x00, 0x01, 0x00, 0x00, 0x48, 0x8B,
    0x40, 0x08, 0xFF, 0x15, 0x48, 0x29, 0x4E, 0x0F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC0, 0xC1, 0xC6, 0x03, 0x0D, 0x00, 0x00, 0x00,
    0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x48, 0x83, 0xB9, 0xC8, 0x02, 0x00, 0x00, 0x00,
    0x74, 0x03, 0x31, 0xC0, 0xC3, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x70, 0xBE, 0xAD, 0x03, 0x20, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x41, 0x56, 0x56, 0x57, 0x53, 0x48, 0x83, 0xEC,
    0x48, 0x4C, 0x89, 0xC3, 0x48, 0x89, 0xD7, 0x48, 0x89, 0xCE, 0x48, 0x8B, 0x05, 0xB7, 0xF1, 0x88,
    0x0F, 0x48, 0x31, 0xE0, 0x48, 0x89, 0x44, 0x24, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xD0, 0xE9, 0xCD, 0x00, 0x20, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x56, 0x57, 0x48, 0x83, 0xEC, 0x38, 0x48, 0x89,
    0xD6, 0x48, 0x89, 0xCF, 0x48, 0x8B, 0x05, 0x5D, 0xC6, 0x68, 0x12, 0x48, 0x31, 0xE0, 0x48, 0x89,
    0x44, 0x24, 0x30, 0x48, 0x8B, 0x02, 0x48, 0xC7, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x70, 0xE2, 0x51, 0x0B, 0x20, 0x00, 0x00, 0x00,
    0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x48, 0x83, 0xB9, 0xF0, 0x03, 0x00, 0x00, 0x00,
    0x74, 0x13, 0x48, 0x8B, 0x81, 0xE8, 0x03, 0x00, 0x00, 0x48, 0x8B, 0x40, 0x28, 0x83, 0x78, 0x18,
    0x00, 0x0F, 0x95, 0xC0, 0xC3, 0x31, 0xC0, 0xC3, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x80, 0x68, 0x83, 0x00, 0x20, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x56, 0x48, 0x83, 0xEC, 0x20, 0x48, 0x89, 0xCE,
    0x48, 0x8B, 0x09, 0x48, 0x8B, 0x41, 0x50, 0x48, 0x8D, 0x15, 0x7A, 0xA5, 0xF0, 0x04, 0x48, 0x39,
    0xD0, 0x75, 0x04, 0x48, 0x8B, 0x41, 0x58, 0x48, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x60, 0x97, 0xBE, 0x01, 0x20, 0x00, 0x00, 0x00,
    0x03, 0x00, 0x00, 0x00, 0x70, 0x97, 0xBE, 0x01, 0x41, 0x56, 0x56, 0x57, 0x53, 0x48, 0x83, 0xEC,
    0x68, 0x0F, 0x29, 0x74, 0x24, 0x50, 0x48, 0x89, 0xCE, 0x48, 0x8B, 0x05, 0xB8, 0x18, 0x78, 0x11,
    0x48, 0x31, 0xE0, 0x48, 0x89, 0x44, 0x24, 0x48, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xD0, 0x9E, 0xFD, 0x02, 0x20, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x56, 0x48, 0x83, 0xEC, 0x20, 0x48, 0x89, 0xCE,
    0x48, 0x8B, 0x81, 0xC0, 0xFC, 0xFF, 0xFF, 0x48, 0x81, 0xC6, 0xC0, 0xFC, 0xFF, 0xFF, 0x48, 0x8B,
    0x40, 0x50, 0x48, 0x89, 0xF1, 0xFF, 0x15, 0x85, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x60, 0x48, 0x00, 0x01, 0x20, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x41, 0x56, 0x56, 0x57, 0x53, 0x48, 0x83, 0xEC,
    0x68, 0x48, 0x89, 0xD7, 0x48, 0x89, 0xCE, 0x48, 0x8B, 0x05, 0xCA, 0x67, 0x36, 0x12, 0x48, 0x31,
    0xE0, 0x48, 0x89, 0x44, 0x24, 0x60, 0x4C, 0x8B, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x80, 0x9E, 0xF4, 0x02, 0x20, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x56, 0x48, 0x83, 0xEC, 0x50, 0x48, 0x89, 0xCE,
    0x48, 0x8B, 0x05, 0xB1, 0x11, 0x42, 0x10, 0x48, 0x31, 0xE0, 0x48, 0x89, 0x44, 0x24, 0x48, 0x48,
    0x89, 0xD0, 0x48, 0xC1, 0xE8, 0x20, 0x8A, 0x89, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xB0, 0x4B, 0x5F, 0x02, 0x20, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x56, 0x57, 0x53, 0x48, 0x83, 0xEC, 0x30, 0x48,
    0x89, 0xD7, 0x48, 0x89, 0xCE, 0xE8, 0xAE, 0x7A, 0x4D, 0xFF, 0x48, 0x85, 0xFF, 0x75, 0x58, 0x48,
    0x8B, 0x8E, 0x08, 0x05, 0x00, 0x00, 0x48, 0xC7, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
};

static bool FormatBuildKey(const BuildIdentity& id, wchar_t* key, size_t count) {
    const auto& g = id.guid;
    const int n = std::swprintf(key, count,
        L"%08X%04X%04X%02X%02X%02X%02X%02X%02X%02X%02X%X",
        static_cast<unsigned>(g.Data1), static_cast<unsigned>(g.Data2), static_cast<unsigned>(g.Data3),
        g.Data4[0], g.Data4[1], g.Data4[2], g.Data4[3], g.Data4[4], g.Data4[5], g.Data4[6], g.Data4[7],
        static_cast<unsigned>(id.age));
    return n > 32 && static_cast<size_t>(n) < count;
}

struct InternetHandle {
    HINTERNET value = nullptr;
    ~InternetHandle() { if (value) WinHttpCloseHandle(value); }
};
static bool DownloadAllowed(HANDLE stop, ULONGLONG deadline) {
    return s_downloadCompatibility.load(std::memory_order_acquire) &&
           WaitForSingleObject(stop, 0) == WAIT_TIMEOUT && GetTickCount64() < deadline;
}
static bool SetDownloadTimeouts(HINTERNET handle, HANDLE stop, ULONGLONG deadline) {
    if (!DownloadAllowed(stop, deadline)) return false;
    const ULONGLONG now = GetTickCount64();
    if (now >= deadline) return false;
    const int remaining = static_cast<int>(deadline - now);
    const int timeout = remaining < 2000 ? remaining : 2000;
    return WinHttpSetTimeouts(handle, timeout, timeout, timeout, timeout) != FALSE;
}
static bool FetchCompatibilityMap(const BuildIdentity& id, HANDLE stop, CompatibilityMap* output) {
    wchar_t key[48]{}, path[192]{};
    if (!FormatBuildKey(id, key, ARRAYSIZE(key))) return false;
    std::swprintf(path, ARRAYSIZE(path),
        L"/bardelyne/brave-acrylic/compatibility-v6/maps/%ls.bin", key);
    const ULONGLONG deadline = GetTickCount64() + 8000;
    InternetHandle session{WinHttpOpen(L"BraveAcrylic/3.3", WINHTTP_ACCESS_TYPE_AUTOMATIC_PROXY,
        WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0)};
    if (!session.value || !SetDownloadTimeouts(session.value, stop, deadline)) return false;
    DWORD attempts = 2; // Permit one alternate address without an unbounded IP retry loop.
    if (!WinHttpSetOption(session.value, WINHTTP_OPTION_CONNECT_RETRIES, &attempts, sizeof(attempts))) return false;
    InternetHandle connection{WinHttpConnect(session.value, L"raw.githubusercontent.com", INTERNET_DEFAULT_HTTPS_PORT, 0)};
    if (!connection.value) return false;
    InternetHandle request{WinHttpOpenRequest(connection.value, L"GET", path, nullptr,
        WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, WINHTTP_FLAG_SECURE)};
    if (!request.value) return false;
    DWORD redirect = WINHTTP_OPTION_REDIRECT_POLICY_NEVER;
    DWORD disabled = WINHTTP_DISABLE_COOKIES | WINHTTP_DISABLE_AUTHENTICATION;
    if (!WinHttpSetOption(request.value, WINHTTP_OPTION_REDIRECT_POLICY, &redirect, sizeof(redirect)) ||
        !WinHttpSetOption(request.value, WINHTTP_OPTION_DISABLE_FEATURE, &disabled, sizeof(disabled)) ||
        !SetDownloadTimeouts(request.value, stop, deadline) ||
        !WinHttpSendRequest(request.value, WINHTTP_NO_ADDITIONAL_HEADERS, 0,
            WINHTTP_NO_REQUEST_DATA, 0, 0, 0) ||
        !SetDownloadTimeouts(request.value, stop, deadline) || !WinHttpReceiveResponse(request.value, nullptr)) return false;
    DWORD status = 0, size = sizeof(status);
    if (!WinHttpQueryHeaders(request.value, WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,
        WINHTTP_HEADER_NAME_BY_INDEX, &status, &size, WINHTTP_NO_HEADER_INDEX)) return false;
    if (status != 200) {
        Wh_Log(L"Compatibility download returned HTTP %u for this exact build", status);
        return false;
    }
    DWORD advertised = 0;
    size = sizeof(advertised);
    if (WinHttpQueryHeaders(request.value, WINHTTP_QUERY_CONTENT_LENGTH | WINHTTP_QUERY_FLAG_NUMBER,
        WINHTTP_HEADER_NAME_BY_INDEX, &advertised, &size, WINHTTP_NO_HEADER_INDEX)) {
        if (advertised != sizeof(CompatibilityMap)) return false;
    } else if (GetLastError() != ERROR_WINHTTP_HEADER_NOT_FOUND) return false;
    unsigned char bytes[sizeof(CompatibilityMap) + 1]{};
    DWORD total = 0;
    while (total < sizeof(bytes)) {
        DWORD received = 0;
        if (!SetDownloadTimeouts(request.value, stop, deadline) ||
            !WinHttpReadData(request.value, bytes + total, sizeof(bytes) - total, &received) ||
            received > sizeof(bytes) - total) return false;
        if (!received) {
            if (total != sizeof(CompatibilityMap) || !DownloadAllowed(stop, deadline)) return false;
            memcpy(output, bytes, sizeof(*output));
            return true;
        }
        total += received;
    }
    return false;
}

static constexpr DWORD kCompatibilityRetryMs = 30 * 60 * 1000;

static DWORD WINAPI CompatibilityWorker(void* argument) {
    HMODULE module = nullptr;
    // Acquire the loader reference outside s_initLock to avoid lock inversion
    // with a simultaneous library load. Never read a borrowed module in a worker.
    if (!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS,
        reinterpret_cast<LPCWSTR>(argument), &module)) return 0;
    if (module != static_cast<HMODULE>(argument)) { FreeLibrary(module); return 0; }
    BuildIdentity id{};
    if (GetBuildIdentity(module, &id)) {
        while (WaitForSingleObject(s_downloadStop, 0) == WAIT_TIMEOUT) {
            CompatibilityMap map{};
            size_t rva[kFunctionSlots]{};
            bool saved = false;
            if (s_downloadCompatibility.load(std::memory_order_acquire) &&
                FetchCompatibilityMap(id, s_downloadStop, &map) &&
                ValidateCompatibilityMap(module, map, rva)) {
                AcquireSRWLockExclusive(&s_initLock);
                if (!s_stopping && s_downloadCompatibility.load(std::memory_order_acquire) &&
                    WaitForSingleObject(s_downloadStop, 0) == WAIT_TIMEOUT) {
                    CompatibilityCache cache{};
                    cache.map = map;
                    cache.revision = 1;
                    std::snprintf(cache.modVersion, sizeof(cache.modVersion), "3.3");
                    GetBraveVersion(module, cache.braveVersion, sizeof(cache.braveVersion));
                    saved = Wh_SetBinaryValue(L"compatibilityMapV6", &cache, sizeof(cache)) != FALSE;
                    if (saved) Wh_Log(L"Compatibility data validated and cached; restart or re-enable the mod to apply it");
                    else Wh_Log(L"Could not save compatibility data; transparency stays off");
                }
                ReleaseSRWLockExclusive(&s_initLock);
            }
            if (saved) break;
            if (s_downloadCompatibility.load(std::memory_order_acquire))
                Wh_Log(L"No cached data for this exact build; retrying in 30 minutes");
            // An idle event wait consumes no recurring paint/startup work and
            // wakes immediately during unload. Disabled downloads stay paused.
            if (WaitForSingleObject(s_downloadStop, kCompatibilityRetryMs) != WAIT_TIMEOUT) break;
        }
    }
    FreeLibrary(module);
    return 0;
}

// Call under s_initLock, only after Windhawk has activated the initial hooks.
// This worker only downloads and caches data; it never changes a live paint path.
static void StartCompatibilityDownloadLocked() {
    if (s_stopping || !s_afterInit || s_downloadAttempted || !s_recoveryModule ||
        !s_downloadCompatibility.load(std::memory_order_acquire)) return;
    s_downloadStop = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (s_downloadStop) s_downloadThread = CreateThread(nullptr, 0, CompatibilityWorker, s_recoveryModule, 0, nullptr);
    if (!s_downloadThread) {
        if (s_downloadStop) CloseHandle(s_downloadStop);
        s_downloadStop = nullptr;
        return;
    }
    s_downloadAttempted = true;
    Wh_Log(L"Checking small exact-build compatibility data in the background");
}

static bool ResolveNativeBackground(HMODULE module){
    size_t rva[kFunctionSlots]{};
    const bool cached=LoadCompatibilityCache(module,rva);
    CompatibilityMap builtIn{};memcpy(&builtIn,kBuiltinMap,sizeof(builtIn));
    if(!cached&&!ValidateCompatibilityMap(module,builtIn,rva)){
        BuildIdentity id{};if(GetBuildIdentity(module,&id)){
            s_recoveryModule=module;wchar_t key[48]{};FormatBuildKey(id,key,ARRAYSIZE(key));
            Wh_Log(L"Brave Acrylic 3.3: no compatible format-6 data for build %s",key);
        }
        return false;
    }
    auto* base=reinterpret_cast<unsigned char*>(module);
    s_setPageBase=reinterpret_cast<NativeSetPageBase>(base+rva[11]);
    s_locationFocus=reinterpret_cast<NativeFocus>(base+rva[6]);
    s_viewFocus=reinterpret_cast<NativeFocus>(base+rva[7]);
    if(!WindhawkUtils::SetFunctionHook(reinterpret_cast<NativeEvent>(base+rva[8]),PageThemeHook,&s_originalTheme)||
       !WindhawkUtils::SetFunctionHook(reinterpret_cast<NativeEvent>(base+rva[9]),PageReadyHook,&s_originalReady)||
       !WindhawkUtils::SetFunctionHook(reinterpret_cast<NativeLayerColor>(base+rva[10]),PageLayerColorHook,&s_originalLayerColor)||
       !WindhawkUtils::SetFunctionHook(reinterpret_cast<NativeAttachContents>(base+rva[12]),AttachContentsHook,&s_originalAttachContents))return false;
    s_background.clear=reinterpret_cast<BraveBackground::Clear>(base+rva[1]);
    s_background.getClass=reinterpret_cast<BraveBackground::GetClass>(base+rva[2]);
    s_background.setBackground=reinterpret_cast<BraveBackground::SetBackground>(base+rva[5]);
    if(!WindhawkUtils::SetFunctionHook(reinterpret_cast<BraveBackground::Paint>(base+rva[0]),ViewOnPaintBackground_Hook,&s_background.originalPaint)||
       !WindhawkUtils::SetFunctionHook(reinterpret_cast<BraveBackground::Transparent>(base+rva[3]),WindowContentsTransparent_Hook,&s_background.originalTransparent)||
       !WindhawkUtils::SetFunctionHook(reinterpret_cast<BraveBackground::Construct>(base+rva[4]),SidebarConstructor_Hook,&s_background.originalConstruct)||!HasCompleteTransparencyPath())return false;
    unsigned resolved = 0;
    for (size_t address : rva) if (address) ++resolved;
    char version[64]{};GetBraveVersion(module,version,sizeof(version));
    Wh_Log(L"Brave Acrylic 3.3: %u/13 native functions from %s (Brave %S, format 6)", resolved,
           cached?L"exact-build cache":L"built-in data",version);
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
    if (!installed) StartCompatibilityDownloadLocked();
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
        const bool installed = TryInstallChromeHooks(chrome, false);
        // Keep the mod resident so AfterInit can check compatibility data if
        // address discovery failed. No network work runs inside ModInit.
        return installed || s_recoveryModule != nullptr;
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
    s_afterInit = true;
    if (!s_stopping && s_activateAfterInit) {
        s_activateAfterInit = false;
        s_transparencyReady.store(HasCompleteTransparencyPath(), std::memory_order_release);
    }
    StartCompatibilityDownloadLocked();
    ReleaseSRWLockExclusive(&s_initLock);
    // Cover a load between ModInit's check and activation of the loader hook.
    if (const auto chrome = GetModuleHandleW(L"chrome.dll")) TryInstallChromeHooks(chrome, true);
    SyncNativeCaptions();
}

void Wh_ModSettingsChanged() {
    if (!s_isBrowserProcess) return;
    LoadModSettings();
    AcquireSRWLockExclusive(&s_initLock);
    StartCompatibilityDownloadLocked();
    ReleaseSRWLockExclusive(&s_initLock);
    SyncNativeCaptions();
    Wh_Log(L"Settings updated; restart Brave normally to rebuild its cached UI brushes");
}

void Wh_ModBeforeUninit() {
    s_captionStopping.store(true, std::memory_order_release);
    s_captionFixActive.store(false, std::memory_order_release);
    AcquireSRWLockExclusive(&s_initLock);
    s_stopping = true;
    if (s_downloadStop) SetEvent(s_downloadStop);
    s_activateAfterInit = false;
    s_transparencyReady.store(false, std::memory_order_release);
    ReleaseSRWLockExclusive(&s_initLock);
    // All HTTP handles belong to this synchronous worker. Join it outside the
    // install lock; never close a WinHTTP handle during another thread's call.
    if (s_downloadThread) WaitForSingleObject(s_downloadThread, INFINITE);
}

void Wh_ModUninit() {
    if (s_downloadThread) { CloseHandle(s_downloadThread); s_downloadThread = nullptr; }
    if (s_downloadStop) { CloseHandle(s_downloadStop); s_downloadStop = nullptr; }
    if (s_isBrowserProcess) EnumWindows(SyncNativeCaptionWindow, 0);
}


#endif  // x64 native background ABI
