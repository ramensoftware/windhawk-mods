// ==WindhawkMod==
// @id              chrome-acrylic-titlebar
// @name            Chrome Acrylic Titlebar
// @description     Show the Windows Acrylic backdrop behind Chrome's tab strip and toolbar
// @version         1.0.0
// @author          shiinamachi
// @github          https://github.com/kentakang
// @homepage        https://github.com/shiinamachi
// @license         MIT
// @architecture    x86-64
// @include         chrome.exe
// @compilerOptions -ldwmapi -lshell32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Chrome Acrylic Titlebar

Lets the Windows 11 Acrylic effect show through the top of Google Chrome.
The tab strip and the toolbar become see-through and blurred, like the rest of
a Windows 11 style desktop, while web pages stay exactly as they are.

![Chrome with Chrome Acrylic Titlebar](https://raw.githubusercontent.com/shiinamachi/wh-chrome-acrylic-titlebar/refs/heads/main/images/chrome-normal.webp)

Incognito windows get a darker tint, so you can still tell them apart:

![An incognito window with Chrome Acrylic Titlebar](https://raw.githubusercontent.com/shiinamachi/wh-chrome-acrylic-titlebar/refs/heads/main/images/chrome-incognito.webp)

## Requirements

- Windows 11 22H2 or later, with **Transparency effects** turned on
  (Settings > Personalization > Colors).
- Google Chrome, 64-bit. Tested with Chrome 158.

## Getting started

To find the parts of Chrome it changes, the mod needs Chrome's debug symbols.
Windhawk downloads them from Google the first time, which takes a while.

1. Enable the mod and start Chrome. Chrome opens normally, without the
   effect, while the symbols download in the background (about 1.1 GB).
2. Wait a few minutes for the download to finish.
3. Fully exit Chrome, including any instance running in the background, and
   start it again. The effect is now applied.

Later starts apply the effect right away. After each Chrome update, the
symbols for the new version are downloaded once more, so repeat these steps.
Downloaded symbols are kept in `C:\ProgramData\Windhawk\Engine\Symbols`; you
can delete folders for old Chrome versions to free disk space.

## Settings

### Backdrop

Default: Acrylic

The Windows effect shown behind the tab strip. Applies to windows opened after
the change.

- **Acrylic**: a strong blur of whatever is behind the window.
- **Mica Alt**: a darker, subtle tint taken from your wallpaper.
- **Mica**: a lighter, subtle tint taken from your wallpaper.

### Transparent inactive tabs

Default: on

Tabs you're not viewing don't get a background of their own, so the effect
shows behind them too. The current tab and the tab under the mouse are still
highlighted. Applies to windows opened after the change.

### Transparent toolbar

Default: on

Extends the effect to the row with the address bar and to the bookmarks bar.
Turn it off to keep that row solid. Applies right away.

### Translucent controls

Default: on

Restyles the controls on top of the effect:

- the address bar becomes translucent, and its site info icon sits on it
  without a separate background;
- the current tab is shown as a soft, light rounded highlight;
- the lines between tabs and the divider next to your profile picture are
  hidden;
- the tab search and new tab buttons only show a background while you hover
  them;
- the "Incognito" label gets the same soft highlight as the current tab.

While you type in the address bar, it stays solid so it matches the list of
suggestions below it. Restart Chrome to apply.

### Incognito tint

Default: on

Adds a dark tint behind the tab strip and toolbar of incognito windows.

### Hide duplicate Windows caption buttons

Default: on

Once the tab strip is see-through, Windows' own minimize, maximize and close
buttons would show up behind Chrome's. This hides them. As a side effect, the
Alt+Space window menu may not open in Chrome windows.

### Download symbols from Google

Default: on

Lets Windhawk download Chrome's symbols when they aren't on your PC yet. If you
turn it off, the mod only works with symbols that were already downloaded.
Restart Chrome to apply.

## Good to know

- When a Chrome window isn't focused, Windows shows the backdrop as a plain
  color. It returns when you click the window.
- The tint follows the Windows light or dark mode.
- If something looks wrong, disable the mod and restart Chrome to get the
  original look back.

## Credits

Hiding the duplicate caption buttons and loading symbols in the background are
adapted from [Brave Acrylic](https://windhawk.net/mods/brave-acrylic) by
bardelyne (MIT).
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- backdrop: acrylic
  $name: Backdrop
  $description: The Windows effect shown behind the tab strip. Applies to windows opened after the change.
  $options:
  - acrylic: Acrylic
  - tabbed: Mica Alt
  - mica: Mica
- inactiveTabs: true
  $name: Transparent inactive tabs
  $description: Tabs you're not viewing don't get a background of their own, so the effect shows behind them too. Applies to windows opened after the change.
- toolbar: true
  $name: Transparent toolbar
  $description: Extend the effect to the address bar row and the bookmarks bar. Applies right away.
- translucentControls: true
  $name: Translucent controls
  $description: Make the address bar translucent, show the current tab as a soft highlight, hide the lines between tabs, and show the tab strip buttons' background only on hover. Restart Chrome to apply.
- incognitoTint: true
  $name: Incognito tint
  $description: Add a dark tint behind the tab strip and toolbar of incognito windows.
- hideNativeCaptionButtons: true
  $name: Hide duplicate Windows caption buttons
  $description: Hide the Windows minimize, maximize and close buttons that would show behind Chrome's own. The Alt+Space window menu may not open in Chrome windows.
- downloadSymbols: true
  $name: Download symbols from Google
  $description: Let Windhawk download Chrome's symbols (about 1.1 GB) when they aren't on your PC yet. When off, only symbols downloaded earlier are used. Restart Chrome to apply.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <dwmapi.h>
#include <shellapi.h>

#include <algorithm>
#include <atomic>
#include <climits>
#include <cstdint>
#include <cstring>
#include <cwchar>
#include <string_view>

#include <windhawk_utils.h>

namespace {

constexpr DWORD kBackdropTypeAttribute = 38;  // DWMWA_SYSTEMBACKDROP_TYPE
constexpr UINT kBackdropMica = 2;             // DWMSBT_MAINWINDOW
constexpr UINT kBackdropAcrylic = 3;          // DWMSBT_TRANSIENTWINDOW
constexpr UINT kBackdropMicaAlt = 4;          // DWMSBT_TABBEDWINDOW

// Chrome continues without the effect if symbols take longer than this.
constexpr DWORD kSymbolWaitMs = 4000;
constexpr wchar_t kChromeSymbolServer[] =
    L"https://chromium-browser-symsrv.commondatastorage.googleapis.com";

// Decorated names, checked against the Chrome 158.0.8127.37 PDB. None of them
// shares its address with another function (no identical-code folding).
enum Slot : unsigned {
    kTransparent,
    kFrameOnPaint,
    kOnPaintBackground,
    kGetClassName,
    kDrawColor,
    kShouldPaintTabBackground,
    kInitFromModel,
    kSetIsGlassFrame,
    kGetColor,
    kChromeColorIdName,
    kGetTabPath,
    kPaintTab,
    kDrawRoundRect,
    kGetBrowserView,
    kGetProfile,
    kIsIncognitoProfile,
    kHwndForView,
    kSlotCount,
};
constexpr std::wstring_view kSymbols[kSlotCount] = {
    // private: virtual bool BrowserDesktopWindowTreeHostWin::ShouldWindowContentsBeTransparent() const
    L"?ShouldWindowContentsBeTransparent@BrowserDesktopWindowTreeHostWin@@EEBA_NXZ",
    // protected: virtual void BrowserFrameViewWin::OnPaint(gfx::Canvas*)
    L"?OnPaint@BrowserFrameViewWin@@MEAAXPEAVCanvas@gfx@@@Z",
    // protected: virtual void views::View::OnPaintBackground(gfx::Canvas*)
    L"?OnPaintBackground@View@views@@MEAAXPEAVCanvas@gfx@@@Z",
    // public: std::string_view views::View::GetClassName() const (windows.h renames it)
    L"?GetClassNameW@View@views@@QEBA?AV?$basic_string_view@DU?$char_traits@D@__Cr@std@@@__Cr@std@@XZ",
    // public: void gfx::Canvas::DrawColor(SkColor, SkBlendMode)
    L"?DrawColor@Canvas@gfx@@QEAAXIW4SkBlendMode@@@Z",
    // public: virtual bool TabStyleViewDelegateImpl::ShouldPaintTabBackgroundColor() const
    // (tabs/common/tab_view.cc, the delegate of the TabView that draws tabs).
    // HorizontalTabStyleViews asks it only for inactive, unselected tabs:
    // active and selected tabs are painted before it is asked, and the hover
    // highlight is painted separately.
    L"?ShouldPaintTabBackgroundColor@TabStyleViewDelegateImpl@@UEBA_NXZ",
    // public: void BrowserTabStripController::InitFromModel(TabStrip*)
    // Runs once per browser window for the classic Tab/TabStrip path that
    // Chrome 158 still uses for the horizontal tab strip.
    L"?InitFromModel@BrowserTabStripController@@QEAAXPEAVTabStrip@@@Z",
    // public: void TabStrip::SetIsGlassFrame(bool)
    // Normally driven by GlassFrameService, which Windows builds never create.
    // A glass tab strip leaves inactive tabs unfilled (TabStrip::IsGlassFrame).
    L"?SetIsGlassFrame@TabStrip@@QEAAX_N@Z",
    // public: SkColor ui::ColorProvider::GetColor(ui::ColorId) const
    L"?GetColor@ColorProvider@ui@@QEBAIH@Z",
    // std::string ChromeColorIdName(ui::ColorId)
    // Not called: only its sorted {ColorId, name} table is read (see
    // ResolveColorIds), because it reports an error for unknown ids.
    L"?ChromeColorIdName@@YA?AV?$basic_string@DU?$char_traits@D@__Cr@std@@V?$allocator@D@23@@__Cr@std@@H@Z",
    // protected: virtual SkPath HorizontalTabStyleViews::GetPath(
    //     TabStyle::PathType, float, const TabPathFlags&) const
    L"?GetPath@HorizontalTabStyleViews@@MEBA?AVSkPath@@W4PathType@TabStyle@@MAEBUTabPathFlags@@@Z",
    // protected: virtual void HorizontalTabStyleViews::PaintTab(gfx::Canvas*) const
    L"?PaintTab@HorizontalTabStyleViews@@MEBAXPEAVCanvas@gfx@@@Z",
    // public: void gfx::Canvas::DrawRoundRect(const gfx::RectF&, float,
    //     const cc::PaintFlags&)
    // Tab painting fills with DrawRect and strokes with DrawPath; DrawRoundRect
    // only draws the separators, whose alpha Chrome sets itself.
    L"?DrawRoundRect@Canvas@gfx@@QEAAXAEBVRectF@2@MAEBVPaintFlags@cc@@@Z",
    // Incognito detection for a frame view:
    // public: BrowserView* BrowserFrameView::GetBrowserView() const
    L"?GetBrowserView@BrowserFrameView@@QEBAPEAVBrowserView@@XZ",
    // public: Profile* BrowserView::GetProfile() const
    L"?GetProfile@BrowserView@@QEBAPEAVProfile@@XZ",
    // public: bool Profile::IsIncognitoProfile() const
    L"?IsIncognitoProfile@Profile@@QEBA_NXZ",
    // HWND views::HWNDForView(const views::View*)
    L"?HWNDForView@views@@YAPEAUHWND__@@PEBVView@1@@Z",
};

// TabStyle::PathType. kActiveTab is only used for the active tab's fill;
// kHighlight is the rounded rectangle Chrome uses for hover and focus.
constexpr int kTabPathActiveTab = 0;
constexpr int kTabPathHighlight = 3;

// Native x64 ABI shapes. A member function returning std::string_view writes
// it through a hidden pointer passed after `this`.
struct NativeClassName {
    const char* data;
    size_t size;
};
using Transparent_t = bool (*)(const void* host);
using Paint_t = void (*)(void* view, void* canvas);
using GetClassName_t = NativeClassName* (*)(const void* view, NativeClassName* out);
using DrawColor_t = void (*)(void* canvas, unsigned int color, int mode);
using TabFill_t = bool (*)(const void* delegate);
using InitFromModel_t = void (*)(void* controller, void* tabStrip);
using SetIsGlassFrame_t = void (*)(void* tabStrip, bool isGlass);
using GetColor_t = unsigned int (*)(const void* provider, int id);
// SkPath is returned through a hidden pointer passed after `this`.
using GetTabPath_t = void* (*)(const void* style, void* path, int type, float scale,
                               const void* flags);
using PaintTab_t = void (*)(const void* style, void* canvas);
using GetPointer_t = void* (*)(const void* self);
using IsIncognitoProfile_t = bool (*)(const void* profile);
using HwndForView_t = HWND (*)(const void* view);
using DrawRoundRect_t = void (*)(void* canvas, const void* rect, float radius,
                                 const void* flags);
constexpr int kSkBlendModeClear = 0;
constexpr int kSkBlendModeSrcOver = 3;
// ARGB, painted over the backdrop of incognito windows.
constexpr unsigned int kIncognitoTint = 0x80181A1C;

// Settings.
std::atomic<UINT> g_backdrop{kBackdropAcrylic};
std::atomic<bool> g_inactiveTabs{true};
std::atomic<bool> g_toolbar{true};
std::atomic<bool> g_translucentControls{true};
std::atomic<bool> g_incognitoTint{true};
std::atomic<bool> g_hideCaptions{true};
std::atomic<bool> g_downloadSymbols{true};

// Set once the chrome.dll hooks are applied, before any browser window exists.
std::atomic<bool> g_active{false};
std::atomic<bool> g_toolbarAvailable{false};
std::atomic<bool> g_chromeAttempted{false};
bool g_isBrowserProcess = false;

Transparent_t g_origTransparent = nullptr;
Paint_t g_origFrameOnPaint = nullptr;
Paint_t g_origOnPaintBackground = nullptr;
GetClassName_t g_getClassName = nullptr;
DrawColor_t g_drawColor = nullptr;
TabFill_t g_origShouldPaintTabBackground = nullptr;
InitFromModel_t g_origInitFromModel = nullptr;
SetIsGlassFrame_t g_setIsGlassFrame = nullptr;
GetColor_t g_origGetColor = nullptr;
GetTabPath_t g_origGetTabPath = nullptr;
PaintTab_t g_origPaintTab = nullptr;
DrawRoundRect_t g_origDrawRoundRect = nullptr;
GetPointer_t g_getBrowserView = nullptr;
GetPointer_t g_getProfile = nullptr;
IsIncognitoProfile_t g_isIncognitoProfile = nullptr;
HwndForView_t g_hwndForView = nullptr;
thread_local unsigned t_tabPaintDepth = 0;

// Chrome colors to restyle, resolved by name at startup.
// kScaleAlpha keeps the RGB, so the light or dark theme still applies, and
// scales the alpha. kHighlight turns a dark color into a translucent white
// highlight (a light color is scaled like kScaleAlpha). Tab text colors come
// from their own ids, so their contrast doesn't depend on these.
enum class ColorMode { kScaleAlpha, kHighlight };
struct ColorOverride {
    const char* name;
    ColorMode mode;
    unsigned alpha;
    int id;
};
ColorOverride g_colorOverrides[] = {
    {"kColorLocationBarBackground", ColorMode::kScaleAlpha, 0x80, -1},
    {"kColorLocationBarBackgroundHovered", ColorMode::kScaleAlpha, 0xA0, -1},
    {"kColorTabBackgroundActiveFrameActive", ColorMode::kHighlight, 0x29, -1},
    {"kColorTabBackgroundActiveFrameInactive", ColorMode::kHighlight, 0x1C, -1},
    // Hover feedback stays: it is drawn by the button's ink drop.
    {"kColorNewTabButtonBackgroundFrameActive", ColorMode::kScaleAlpha, 0, -1},
    {"kColorNewTabButtonBackgroundFrameInactive", ColorMode::kScaleAlpha, 0, -1},
    {"kColorNewTabButtonCRBackgroundFrameActive", ColorMode::kScaleAlpha, 0, -1},
    {"kColorNewTabButtonCRBackgroundFrameInactive", ColorMode::kScaleAlpha, 0, -1},
    {"kColorTabSearchButtonBackground", ColorMode::kScaleAlpha, 0, -1},
    {"kColorAvatarButtonHighlightIncognito", ColorMode::kHighlight, 0x24, -1},
};
constexpr unsigned kLightColorAlpha = 0xCC;
int g_colorMin = 0;
int g_colorMax = -1;
std::atomic<bool> g_colorsReady{false};

decltype(&DwmSetWindowAttribute) g_origDwmSetWindowAttribute = nullptr;
decltype(&LoadLibraryExW) g_origLoadLibraryExW = nullptr;

// ---------------------------------------------------------------------------
// Settings

void LoadSettings() {
    PCWSTR backdrop = Wh_GetStringSetting(L"backdrop");
    UINT type = kBackdropAcrylic;
    if (backdrop && !wcscmp(backdrop, L"mica")) {
        type = kBackdropMica;
    } else if (backdrop && !wcscmp(backdrop, L"tabbed")) {
        type = kBackdropMicaAlt;
    }
    Wh_FreeStringSetting(backdrop);
    g_backdrop.store(type);
    g_inactiveTabs.store(Wh_GetIntSetting(L"inactiveTabs") != 0);
    g_toolbar.store(Wh_GetIntSetting(L"toolbar") != 0);
    g_translucentControls.store(Wh_GetIntSetting(L"translucentControls") != 0);
    g_incognitoTint.store(Wh_GetIntSetting(L"incognitoTint") != 0);
    g_hideCaptions.store(Wh_GetIntSetting(L"hideNativeCaptionButtons") != 0);
    g_downloadSymbols.store(Wh_GetIntSetting(L"downloadSymbols") != 0);
}

// ---------------------------------------------------------------------------
// Browser window identification
//
// Only browser windows (BrowserDesktopWindowTreeHostWin) ask the hooked
// ShouldWindowContentsBeTransparent while they are created, right after their
// HWND was created on the same thread. That HWND gets a property, and only
// marked windows get the backdrop and caption-button changes. Dialogs, task
// manager and other Chrome windows keep their native frame untouched.

constexpr wchar_t kBrowserFrameProperty[] = L"Windhawk.ChromeAcrylicTitlebar.Frame";
// Set on incognito browser windows when their frame paints, so views painted
// later (the toolbar) can repaint the tint after clearing their background.
constexpr wchar_t kIncognitoProperty[] = L"Windhawk.ChromeAcrylicTitlebar.Incognito";
constexpr ULONGLONG kCandidateMaxAgeMs = 2000;
thread_local HWND t_candidate = nullptr;
thread_local ULONGLONG t_candidateTick = 0;

LONG_PTR RealStyle(HWND window);
void ApplyNativeCaption(HWND window);

bool IsBrowserFrameWindow(HWND window) {
    return window && GetPropW(window, kBrowserFrameProperty) != nullptr;
}

bool IsFrameCandidate(HWND window, DWORD style) {
    constexpr DWORD kRequired =
        WS_CAPTION | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX;
    if ((style & kRequired) != kRequired || (style & WS_CHILD) ||
        GetAncestor(window, GA_ROOT) != window || GetWindow(window, GW_OWNER)) {
        return false;
    }
    wchar_t name[32]{};
    return GetClassNameW(window, name, ARRAYSIZE(name)) &&
           !wcscmp(name, L"Chrome_WidgetWin_1");
}

void MarkBrowserFrame(HWND window) {
    if (!SetPropW(window, kBrowserFrameProperty, reinterpret_cast<HANDLE>(1))) {
        return;
    }
    // Chrome already asked for Mica while creating the window.
    const UINT backdrop = g_backdrop.load();
    g_origDwmSetWindowAttribute(window, kBackdropTypeAttribute, &backdrop,
                                sizeof(backdrop));
    ApplyNativeCaption(window);
}

// ---------------------------------------------------------------------------
// chrome.dll hooks

bool Transparent_Hook(const void* host) {
    if (!g_active.load(std::memory_order_acquire)) {
        return g_origTransparent(host);
    }
    const HWND candidate = t_candidate;
    if (candidate) {
        t_candidate = nullptr;
        if (GetTickCount64() - t_candidateTick < kCandidateMaxAgeMs &&
            IsWindow(candidate)) {
            MarkBrowserFrame(candidate);
        }
    }
    return true;
}

bool IsIncognitoFrame(const void* frameView) {
    if (!g_getBrowserView || !g_getProfile || !g_isIncognitoProfile) {
        return false;
    }
    const void* browserView = g_getBrowserView(frameView);
    const void* profile = browserView ? g_getProfile(browserView) : nullptr;
    return profile && g_isIncognitoProfile(profile);
}

bool IsIncognitoWindowOf(const void* view) {
    const HWND window = g_hwndForView ? g_hwndForView(view) : nullptr;
    return window && GetPropW(window, kIncognitoProperty);
}

void FrameOnPaint_Hook(void* view, void* canvas) {
    if (!g_active.load(std::memory_order_acquire)) {
        g_origFrameOnPaint(view, canvas);
        return;
    }
    const bool incognito = IsIncognitoFrame(view);
    if (const HWND window = g_hwndForView ? g_hwndForView(view) : nullptr) {
        if (incognito) {
            SetPropW(window, kIncognitoProperty, reinterpret_cast<HANDLE>(1));
        } else if (GetPropW(window, kIncognitoProperty)) {
            RemovePropW(window, kIncognitoProperty);
        }
    }
    // Leave the titlebar unpainted so the DWM backdrop shows through. Child
    // views (tabs, caption buttons, toolbar) paint afterwards as usual.
    if (canvas && g_drawColor && incognito &&
        g_incognitoTint.load(std::memory_order_relaxed)) {
        g_drawColor(canvas, kIncognitoTint, kSkBlendModeSrcOver);
    }
}

enum class ViewRole { kOther, kToolbarRow, kOmniboxText, kOmniboxIcon, kToolbarDivider };

ViewRole GetViewRole(void* view) {
    NativeClassName name{};
    g_getClassName(view, &name);
    if (!name.data || name.size == 0 || name.size > 64) {
        return ViewRole::kOther;
    }
    const std::string_view cls(name.data, name.size);
    if (cls == "ToolbarView" || cls == "BookmarkBarView") {
        return ViewRole::kToolbarRow;
    }
    if (cls == "OmniboxViewViews") {
        return ViewRole::kOmniboxText;
    }
    if (cls == "LocationIconView") {
        return ViewRole::kOmniboxIcon;
    }
    if (cls == "ToolbarDivider") {
        return ViewRole::kToolbarDivider;
    }
    return ViewRole::kOther;
}

void OnPaintBackground_Hook(void* view, void* canvas) {
    const bool toolbar = g_toolbar.load(std::memory_order_relaxed);
    const bool controls = g_colorsReady.load(std::memory_order_acquire) &&
                          g_translucentControls.load(std::memory_order_relaxed);
    if (view && canvas && (toolbar || controls) &&
        g_toolbarAvailable.load(std::memory_order_acquire) &&
        g_active.load(std::memory_order_acquire)) {
        const ViewRole role = GetViewRole(view);
        if (toolbar && role == ViewRole::kToolbarRow) {
            // The canvas is clipped to this view; clear it to reveal the
            // backdrop. Chrome paints an opaque layer under the toolbar too, so
            // skipping the background alone isn't enough. Clearing also removes
            // the frame's incognito tint, so paint it again.
            g_drawColor(canvas, 0, kSkBlendModeClear);
            if (g_incognitoTint.load(std::memory_order_relaxed) && IsIncognitoWindowOf(view)) {
                g_drawColor(canvas, kIncognitoTint, kSkBlendModeSrcOver);
            }
            return;
        }
        if (controls && (role == ViewRole::kOmniboxText || role == ViewRole::kOmniboxIcon)) {
            // The location bar already paints its translucent background behind
            // these. Painting another background on top would stand out. The
            // icon's hover feedback is an ink drop and still shows.
            return;
        }
        if (controls && role == ViewRole::kToolbarDivider) {
            return;
        }
    }
    g_origOnPaintBackground(view, canvas);
}

unsigned int GetColor_Hook(const void* provider, int id) {
    const unsigned int color = g_origGetColor(provider, id);
    if (id < g_colorMin || id > g_colorMax ||
        !g_colorsReady.load(std::memory_order_acquire) ||
        !g_translucentControls.load(std::memory_order_relaxed) ||
        !g_active.load(std::memory_order_acquire)) {
        return color;
    }
    for (const ColorOverride& entry : g_colorOverrides) {
        if (entry.id != id) {
            continue;
        }
        unsigned int scale = entry.alpha;
        if (entry.mode == ColorMode::kHighlight) {
            const unsigned int luma = (((color >> 16) & 0xFF) * 299 +
                                       ((color >> 8) & 0xFF) * 587 + (color & 0xFF) * 114) /
                                      1000;
            if (luma < 0x80) {
                return 0x00FFFFFF | (entry.alpha << 24);
            }
            scale = kLightColorAlpha;
        }
        const unsigned int alpha = (color >> 24) * scale / 0xFF;
        return (color & 0x00FFFFFF) | (alpha << 24);
    }
    return color;
}

void* GetTabPath_Hook(const void* style, void* path, int type, float scale,
                      const void* flags) {
    if (type == kTabPathActiveTab && g_colorsReady.load(std::memory_order_acquire) &&
        g_translucentControls.load(std::memory_order_relaxed) &&
        g_active.load(std::memory_order_acquire)) {
        type = kTabPathHighlight;
    }
    return g_origGetTabPath(style, path, type, scale, flags);
}

void PaintTab_Hook(const void* style, void* canvas) {
    ++t_tabPaintDepth;
    g_origPaintTab(style, canvas);
    --t_tabPaintDepth;
}

void DrawRoundRect_Hook(void* canvas, const void* rect, float radius, const void* flags) {
    if (t_tabPaintDepth && g_colorsReady.load(std::memory_order_acquire) &&
        g_translucentControls.load(std::memory_order_relaxed) &&
        g_active.load(std::memory_order_acquire)) {
        return;  // A separator between tabs.
    }
    g_origDrawRoundRect(canvas, rect, radius, flags);
}

// ChromeColorIdName() binary-searches a sorted {ColorId, const char*} table.
// Find the table from the function's first instructions (`mov r8d, <count>`
// and `lea rax, [rip + <table>]`) and validate every entry before using it.
struct ColorNameEntry {
    int id;
    const char* name;
};
static_assert(sizeof(ColorNameEntry) == 16);

bool ResolveColorIds(HMODULE module, const void* nameFunction) {
    const auto* base = reinterpret_cast<const unsigned char*>(module);
    const auto* dos = reinterpret_cast<const IMAGE_DOS_HEADER*>(base);
    const auto* nt = reinterpret_cast<const IMAGE_NT_HEADERS*>(base + dos->e_lfanew);
    const size_t imageSize = nt->OptionalHeader.SizeOfImage;
    const auto inImage = [&](const void* p, size_t size) {
        const auto* bytes = static_cast<const unsigned char*>(p);
        return bytes >= base && size <= imageSize &&
               static_cast<size_t>(bytes - base) <= imageSize - size;
    };

    constexpr size_t kScanBytes = 0x40;
    const auto* code = static_cast<const unsigned char*>(nameFunction);
    if (!inImage(code, kScanBytes)) {
        return false;
    }
    unsigned int count = 0;
    const ColorNameEntry* table = nullptr;
    for (size_t i = 0; i + 7 <= kScanBytes; ++i) {
        if (!count && code[i] == 0x41 && code[i + 1] == 0xB8) {
            std::memcpy(&count, code + i + 2, sizeof(count));
        }
        if (!table && code[i] == 0x48 && code[i + 1] == 0x8D &&
            (code[i + 2] & 0xC7) == 0x05) {
            int32_t displacement = 0;
            std::memcpy(&displacement, code + i + 3, sizeof(displacement));
            table = reinterpret_cast<const ColorNameEntry*>(code + i + 7 + displacement);
        }
    }
    if (count < 16 || count > 8192 || !table ||
        !inImage(table, count * sizeof(ColorNameEntry))) {
        return false;
    }

    int previous = INT_MIN;
    for (unsigned int i = 0; i < count; ++i) {
        const ColorNameEntry& entry = table[i];
        if (entry.id <= previous || !inImage(entry.name, 8) ||
            std::strncmp(entry.name, "kColor", 6) != 0) {
            return false;
        }
        previous = entry.id;
        for (ColorOverride& color : g_colorOverrides) {
            const size_t length = std::strlen(color.name) + 1;
            if (inImage(entry.name, length) &&
                std::memcmp(entry.name, color.name, length) == 0) {
                color.id = entry.id;
            }
        }
    }

    int minId = INT_MAX;
    int maxId = INT_MIN;
    for (const ColorOverride& color : g_colorOverrides) {
        if (color.id >= 0) {
            minId = std::min(minId, color.id);
            maxId = std::max(maxId, color.id);
        }
    }
    if (minId > maxId) {
        return false;
    }
    g_colorMin = minId;
    g_colorMax = maxId;
    return true;
}

bool ShouldPaintTabBackground_Hook(const void* delegate) {
    if (g_inactiveTabs.load(std::memory_order_relaxed) &&
        g_active.load(std::memory_order_acquire)) {
        return false;
    }
    return g_origShouldPaintTabBackground(delegate);
}

void InitFromModel_Hook(void* controller, void* tabStrip) {
    g_origInitFromModel(controller, tabStrip);
    if (tabStrip && g_setIsGlassFrame &&
        g_inactiveTabs.load(std::memory_order_relaxed) &&
        g_active.load(std::memory_order_acquire)) {
        g_setIsGlassFrame(tabStrip, true);
    }
}

// ---------------------------------------------------------------------------
// Backdrop type

HRESULT WINAPI DwmSetWindowAttribute_Hook(HWND window,
                                          DWORD attribute,
                                          LPCVOID value,
                                          DWORD size) {
    if (attribute == kBackdropTypeAttribute && value && size == sizeof(UINT) &&
        *static_cast<const UINT*>(value) == kBackdropMica &&
        g_active.load(std::memory_order_acquire) && IsBrowserFrameWindow(window)) {
        const UINT backdrop = g_backdrop.load();
        return g_origDwmSetWindowAttribute(window, attribute, &backdrop,
                                           sizeof(backdrop));
    }
    return g_origDwmSetWindowAttribute(window, attribute, value, size);
}

// ---------------------------------------------------------------------------
// Native caption buttons
//
// Windows still draws its caption buttons underneath Chromium's transparent
// custom frame. Hide only WS_SYSMENU and keep reporting it to Chromium, so its
// own logic (including saving the style for fullscreen) sees the original.
// Adapted from Brave Acrylic (MIT, bardelyne).

constexpr wchar_t kCaptionProperty[] = L"Windhawk.ChromeAcrylicTitlebar.NativeCaption";
std::atomic<bool> g_captionFixActive{false};
std::atomic<bool> g_captionStopping{false};
std::atomic<bool> g_captionPropertyUsed{false};
bool g_captionHooksInstalled = false;
decltype(&CreateWindowExW) g_origCreateWindowExW = nullptr;
decltype(&GetWindowLongPtrW) g_origGetWindowLongPtrW = nullptr;
decltype(&SetWindowLongPtrW) g_origSetWindowLongPtrW = nullptr;
decltype(&GetWindowLongW) g_origGetWindowLongW = nullptr;
decltype(&SetWindowLongW) g_origSetWindowLongW = nullptr;
thread_local HWND t_captionStyleWrite = nullptr;

LONG_PTR RealStyle(HWND window) {
    return g_origGetWindowLongPtrW ? g_origGetWindowLongPtrW(window, GWL_STYLE)
                                   : GetWindowLongPtrW(window, GWL_STYLE);
}

void RefreshFrame(HWND window) {
    SetWindowPos(window, nullptr, 0, 0, 0, 0,
                 SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE |
                     SWP_FRAMECHANGED);
}

void RestoreNativeCaption(HWND window) {
    if (!GetPropW(window, kCaptionProperty)) {
        return;
    }
    RemovePropW(window, kCaptionProperty);
    // The property is gone, so the hooks pass this write through unchanged.
    const LONG_PTR style = GetWindowLongPtrW(window, GWL_STYLE);
    SetWindowLongPtrW(window, GWL_STYLE, style | WS_SYSMENU);
    RefreshFrame(window);
}

template <typename Value>
Value ReadCaptionStyle(HWND window, int index, Value(WINAPI* original)(HWND, int)) {
    if (index != GWL_STYLE ||
        (!g_captionFixActive.load(std::memory_order_acquire) &&
         !g_captionPropertyUsed.load(std::memory_order_relaxed))) {
        return original(window, index);
    }
    Value value = original(window, index);
    const DWORD error = GetLastError();
    if (GetPropW(window, kCaptionProperty)) {
        value |= WS_SYSMENU;
    }
    SetLastError(error);
    return value;
}

LONG_PTR WINAPI GetWindowLongPtrW_Hook(HWND window, int index) {
    return ReadCaptionStyle(window, index, g_origGetWindowLongPtrW);
}

LONG WINAPI GetWindowLongW_Hook(HWND window, int index) {
    return ReadCaptionStyle(window, index, g_origGetWindowLongW);
}

template <typename Value>
Value WriteCaptionStyle(HWND window,
                        int index,
                        Value value,
                        Value(WINAPI* original)(HWND, int, Value)) {
    // Windows may forward one API to the other; translate the style only once.
    if (index != GWL_STYLE || t_captionStyleWrite == window ||
        (!g_captionFixActive.load(std::memory_order_acquire) &&
         !g_captionPropertyUsed.load(std::memory_order_relaxed))) {
        return original(window, index, value);
    }
    const bool owned = GetPropW(window, kCaptionProperty) != nullptr;
    const bool hide = g_captionFixActive.load(std::memory_order_acquire) &&
                      (value & WS_SYSMENU) && (owned || IsBrowserFrameWindow(window));
    bool marked = owned;
    if (hide && !owned) {
        g_captionPropertyUsed.store(true, std::memory_order_relaxed);
        marked = SetPropW(window, kCaptionProperty, reinterpret_cast<HANDLE>(1)) != FALSE;
    } else if (!hide && owned) {
        RemovePropW(window, kCaptionProperty);
        marked = false;
    }
    const HWND previousWrite = t_captionStyleWrite;
    t_captionStyleWrite = window;
    // Zero is a valid previous style; clear stale errors first.
    SetLastError(ERROR_SUCCESS);
    const Value previous = original(
        window, index, hide && marked ? value & ~static_cast<Value>(WS_SYSMENU) : value);
    const DWORD error = GetLastError();
    t_captionStyleWrite = previousWrite;
    if (!previous && error) {
        if (owned) {
            SetPropW(window, kCaptionProperty, reinterpret_cast<HANDLE>(1));
        } else {
            RemovePropW(window, kCaptionProperty);
        }
    } else if (hide && marked && !g_captionFixActive.load(std::memory_order_acquire)) {
        // The setting changed while SetWindowLong dispatched style messages.
        RestoreNativeCaption(window);
    }
    SetLastError(error);
    return previous && owned ? previous | WS_SYSMENU : previous;
}

LONG_PTR WINAPI SetWindowLongPtrW_Hook(HWND window, int index, LONG_PTR value) {
    return WriteCaptionStyle(window, index, value, g_origSetWindowLongPtrW);
}

LONG WINAPI SetWindowLongW_Hook(HWND window, int index, LONG value) {
    return WriteCaptionStyle(window, index, value, g_origSetWindowLongW);
}

void ApplyNativeCaption(HWND window) {
    if (!g_captionFixActive.load(std::memory_order_acquire) ||
        GetPropW(window, kCaptionProperty)) {
        return;
    }
    const LONG_PTR style = RealStyle(window);
    if (!(style & WS_SYSMENU) || !IsBrowserFrameWindow(window)) {
        return;
    }
    SetWindowLongPtrW_Hook(window, GWL_STYLE, style);
    if (GetPropW(window, kCaptionProperty)) {
        RefreshFrame(window);
    }
}

HWND WINAPI CreateWindowExW_Hook(DWORD exStyle,
                                 LPCWSTR className,
                                 LPCWSTR windowName,
                                 DWORD style,
                                 int x,
                                 int y,
                                 int width,
                                 int height,
                                 HWND parent,
                                 HMENU menu,
                                 HINSTANCE instance,
                                 LPVOID param) {
    const HWND window = g_origCreateWindowExW(exStyle, className, windowName, style, x,
                                              y, width, height, parent, menu, instance,
                                              param);
    const DWORD error = GetLastError();
    if (window && g_active.load(std::memory_order_acquire) &&
        IsFrameCandidate(window, style)) {
        t_candidate = window;
        t_candidateTick = GetTickCount64();
    }
    SetLastError(error);
    return window;
}

BOOL CALLBACK SyncCaptionWindow(HWND window, LPARAM) {
    DWORD process = 0;
    GetWindowThreadProcessId(window, &process);
    if (process == GetCurrentProcessId()) {
        if (g_captionFixActive.load(std::memory_order_acquire)) {
            ApplyNativeCaption(window);
        } else {
            RestoreNativeCaption(window);
        }
    }
    return TRUE;
}

BOOL CALLBACK ForgetBrowserFrame(HWND window, LPARAM) {
    DWORD process = 0;
    GetWindowThreadProcessId(window, &process);
    if (process == GetCurrentProcessId()) {
        RemovePropW(window, kBrowserFrameProperty);
        RemovePropW(window, kIncognitoProperty);
    }
    return TRUE;
}

void SyncCaptions() {
    const bool active = g_captionHooksInstalled &&
                        !g_captionStopping.load(std::memory_order_acquire) &&
                        g_active.load(std::memory_order_acquire) &&
                        g_hideCaptions.load(std::memory_order_relaxed);
    g_captionFixActive.store(active, std::memory_order_release);
    if (g_captionHooksInstalled) {
        EnumWindows(SyncCaptionWindow, 0);
    }
}

BOOL CALLBACK UpdateBackdropWindow(HWND window, LPARAM) {
    if (IsBrowserFrameWindow(window)) {
        const UINT backdrop = g_backdrop.load();
        g_origDwmSetWindowAttribute(window, kBackdropTypeAttribute, &backdrop,
                                    sizeof(backdrop));
    }
    return TRUE;
}

// ---------------------------------------------------------------------------
// Symbol lookup

SRWLOCK g_lock = SRWLOCK_INIT;
CONDITION_VARIABLE g_lookupChanged = CONDITION_VARIABLE_INIT;
HANDLE g_worker = nullptr;
bool g_stopping = false;
struct {
    HMODULE module;
    ULONGLONG deadline;
    bool finished;
    bool succeeded;
    void* address[kSlotCount];
} g_lookup{};

bool ResolveAddresses(HMODULE module, void** result) {
    void* address[kSlotCount]{};
    WindhawkUtils::SYMBOL_HOOK chromeDllHooks[kSlotCount];
    for (unsigned i = 0; i < kSlotCount; ++i) {
        chromeDllHooks[i] = {{kSymbols[i]}, &address[i], nullptr, true};
    }
    WH_HOOK_SYMBOLS_OPTIONS options{};
    options.optionsSize = sizeof(options);
    options.symbolServer = g_downloadSymbols.load() ? kChromeSymbolServer : L"";
    // The Chrome PDB is about 5.8 GB with millions of symbols. Undecorating all
    // of them is very slow, so match the decorated names only.
    options.noUndecoratedSymbols = TRUE;
    if (!WindhawkUtils::HookSymbols(module, chromeDllHooks, kSlotCount, &options)) {
        return false;
    }
    std::memcpy(result, address, sizeof(address));
    return address[kTransparent] && address[kFrameOnPaint];
}

DWORD WINAPI LookupWorker(void*) {
    void* address[kSlotCount]{};
    HMODULE pinned = nullptr;
    const HMODULE module = g_lookup.module;
    const bool succeeded =
        GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS,
                           reinterpret_cast<LPCWSTR>(module), &pinned) &&
        ResolveAddresses(module, address);
    if (pinned) {
        FreeLibrary(pinned);
    }

    AcquireSRWLockExclusive(&g_lock);
    std::memcpy(g_lookup.address, address, sizeof(address));
    g_lookup.succeeded = succeeded;
    g_lookup.finished = true;
    const bool late = GetTickCount64() >= g_lookup.deadline;
    WakeAllConditionVariable(&g_lookupChanged);
    ReleaseSRWLockExclusive(&g_lock);

    if (!succeeded) {
        Wh_Log(L"Symbol lookup failed; Chrome stays unmodified");
    } else if (late) {
        Wh_Log(L"Symbols are ready; fully exit and restart Chrome to apply");
    }
    return 0;
}

bool InstallChromeHooks() {
    void* const* address = g_lookup.address;
    if (!WindhawkUtils::SetFunctionHook(
            reinterpret_cast<Transparent_t>(address[kTransparent]), Transparent_Hook,
            &g_origTransparent) ||
        !WindhawkUtils::SetFunctionHook(
            reinterpret_cast<Paint_t>(address[kFrameOnPaint]), FrameOnPaint_Hook,
            &g_origFrameOnPaint)) {
        return false;
    }
    if (address[kOnPaintBackground] && address[kGetClassName] && address[kDrawColor]) {
        g_getClassName = reinterpret_cast<GetClassName_t>(address[kGetClassName]);
        g_drawColor = reinterpret_cast<DrawColor_t>(address[kDrawColor]);
        g_toolbarAvailable.store(WindhawkUtils::SetFunctionHook(
            reinterpret_cast<Paint_t>(address[kOnPaintBackground]),
            OnPaintBackground_Hook, &g_origOnPaintBackground));
    }
    if (address[kShouldPaintTabBackground] &&
        !WindhawkUtils::SetFunctionHook(
            reinterpret_cast<TabFill_t>(address[kShouldPaintTabBackground]),
            ShouldPaintTabBackground_Hook, &g_origShouldPaintTabBackground)) {
        Wh_Log(L"Inactive tab background hook unavailable");
    }
    if (address[kInitFromModel] && address[kSetIsGlassFrame]) {
        g_setIsGlassFrame = reinterpret_cast<SetIsGlassFrame_t>(address[kSetIsGlassFrame]);
        if (!WindhawkUtils::SetFunctionHook(
                reinterpret_cast<InitFromModel_t>(address[kInitFromModel]),
                InitFromModel_Hook, &g_origInitFromModel)) {
            Wh_Log(L"Glass tab strip hook unavailable");
        }
    }
    if (address[kGetColor] && address[kChromeColorIdName]) {
        if (ResolveColorIds(g_lookup.module, address[kChromeColorIdName]) &&
            WindhawkUtils::SetFunctionHook(reinterpret_cast<GetColor_t>(address[kGetColor]),
                                           GetColor_Hook, &g_origGetColor)) {
            g_colorsReady.store(true, std::memory_order_release);
            if (address[kGetTabPath] &&
                !WindhawkUtils::SetFunctionHook(
                    reinterpret_cast<GetTabPath_t>(address[kGetTabPath]), GetTabPath_Hook,
                    &g_origGetTabPath)) {
                Wh_Log(L"Active tab shape hook unavailable");
            }
            if (address[kGetBrowserView] && address[kGetProfile] &&
                address[kIsIncognitoProfile]) {
                g_getBrowserView = reinterpret_cast<GetPointer_t>(address[kGetBrowserView]);
                g_getProfile = reinterpret_cast<GetPointer_t>(address[kGetProfile]);
                g_isIncognitoProfile =
                    reinterpret_cast<IsIncognitoProfile_t>(address[kIsIncognitoProfile]);
                g_hwndForView = reinterpret_cast<HwndForView_t>(address[kHwndForView]);
            }
            if (address[kPaintTab] && address[kDrawRoundRect] &&
                (!WindhawkUtils::SetFunctionHook(
                     reinterpret_cast<PaintTab_t>(address[kPaintTab]), PaintTab_Hook,
                     &g_origPaintTab) ||
                 !WindhawkUtils::SetFunctionHook(
                     reinterpret_cast<DrawRoundRect_t>(address[kDrawRoundRect]),
                     DrawRoundRect_Hook, &g_origDrawRoundRect))) {
                Wh_Log(L"Tab separator hooks unavailable");
            }
        } else {
            Wh_Log(L"Translucent controls unavailable");
        }
    }
    return true;
}

// Resolve symbols and, if they arrive within the budget, hook chrome.dll.
// `install` is false when Chrome already has windows: those decided to be
// opaque at creation, so the lookup only fills Windhawk's cache.
void AttemptChrome(HMODULE module, bool install) {
    if (!module || g_chromeAttempted.exchange(true)) {
        return;
    }
    AcquireSRWLockExclusive(&g_lock);
    if (g_stopping) {
        ReleaseSRWLockExclusive(&g_lock);
        return;
    }
    g_lookup.module = module;
    g_lookup.deadline = GetTickCount64() + (install ? kSymbolWaitMs : 0);
    g_worker = CreateThread(nullptr, 0, LookupWorker, nullptr, 0, nullptr);
    if (!g_worker) {
        ReleaseSRWLockExclusive(&g_lock);
        Wh_Log(L"Could not start the symbol lookup");
        return;
    }
    if (!install) {
        ReleaseSRWLockExclusive(&g_lock);
        Wh_Log(L"Chrome was already running; restart Chrome to apply");
        return;
    }
    while (!g_lookup.finished && !g_stopping) {
        const ULONGLONG now = GetTickCount64();
        if (now >= g_lookup.deadline ||
            !SleepConditionVariableSRW(&g_lookupChanged, &g_lock,
                                       static_cast<DWORD>(g_lookup.deadline - now), 0)) {
            break;
        }
    }
    bool active = false;
    if (!g_stopping && g_lookup.finished && g_lookup.succeeded) {
        active = InstallChromeHooks() && Wh_ApplyHookOperations();
        if (!active) {
            Wh_Log(L"Could not hook chrome.dll; Chrome stays unmodified");
        }
    } else if (!g_lookup.finished) {
        Wh_Log(L"Symbols are still loading; Chrome starts without the effect");
    }
    g_active.store(active, std::memory_order_release);
    ReleaseSRWLockExclusive(&g_lock);
    if (active) {
        Wh_Log(L"Transparent titlebar is active");
        SyncCaptions();
    }
}

bool IsExecutableLoad(DWORD flags) {
    return !(flags & (DONT_RESOLVE_DLL_REFERENCES | LOAD_LIBRARY_AS_DATAFILE |
                      LOAD_LIBRARY_AS_DATAFILE_EXCLUSIVE |
                      LOAD_LIBRARY_AS_IMAGE_RESOURCE));
}

HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR name, HANDLE file, DWORD flags) {
    const HMODULE module = g_origLoadLibraryExW(name, file, flags);
    const DWORD error = GetLastError();
    if (module && !g_chromeAttempted.load(std::memory_order_acquire) &&
        IsExecutableLoad(flags) && module == GetModuleHandleW(L"chrome.dll")) {
        // Chrome cannot use chrome.dll until this returns, so no window exists yet.
        AttemptChrome(module, true);
    }
    SetLastError(error);
    return module;
}

BOOL CALLBACK FindChromeWindow(HWND window, LPARAM found) {
    DWORD process = 0;
    GetWindowThreadProcessId(window, &process);
    wchar_t name[32]{};
    if (process == GetCurrentProcessId() &&
        GetClassNameW(window, name, ARRAYSIZE(name)) &&
        !wcscmp(name, L"Chrome_WidgetWin_1")) {
        *reinterpret_cast<bool*>(found) = true;
        return FALSE;
    }
    return TRUE;
}

bool IsBrowserProcess() {
    int count = 0;
    LPWSTR* args = CommandLineToArgvW(GetCommandLineW(), &count);
    if (!args) {
        return false;
    }
    bool browser = true;
    for (int i = 1; i < count; ++i) {
        if (!wcscmp(args[i], L"--") || !wcscmp(args[i], L"--single-argument")) {
            break;
        }
        if (!wcsncmp(args[i], L"--type=", 7)) {
            browser = false;
            break;
        }
    }
    LocalFree(args);
    return browser;
}

}  // namespace

BOOL Wh_ModInit() {
    if (!IsBrowserProcess()) {
        return FALSE;
    }
    g_isBrowserProcess = true;
    LoadSettings();

    if (!WindhawkUtils::SetFunctionHook(DwmSetWindowAttribute,
                                        DwmSetWindowAttribute_Hook,
                                        &g_origDwmSetWindowAttribute)) {
        Wh_Log(L"Could not hook DwmSetWindowAttribute");
        return FALSE;
    }
    g_captionHooksInstalled =
        WindhawkUtils::SetFunctionHook(CreateWindowExW, CreateWindowExW_Hook,
                                       &g_origCreateWindowExW) &&
        WindhawkUtils::SetFunctionHook(GetWindowLongPtrW, GetWindowLongPtrW_Hook,
                                       &g_origGetWindowLongPtrW) &&
        WindhawkUtils::SetFunctionHook(SetWindowLongPtrW, SetWindowLongPtrW_Hook,
                                       &g_origSetWindowLongPtrW) &&
        WindhawkUtils::SetFunctionHook(GetWindowLongW, GetWindowLongW_Hook,
                                       &g_origGetWindowLongW) &&
        WindhawkUtils::SetFunctionHook(SetWindowLongW, SetWindowLongW_Hook,
                                       &g_origSetWindowLongW);
    if (!g_captionHooksInstalled) {
        Wh_Log(L"Caption button fix unavailable");
    }

    if (GetModuleHandleW(L"chrome.dll")) {
        // Handled in Wh_ModAfterInit, once the hooks above are active.
        return TRUE;
    }
    const HMODULE kernelBase = GetModuleHandleW(L"kernelbase.dll");
    const auto loader =
        kernelBase ? reinterpret_cast<decltype(&LoadLibraryExW)>(
                         GetProcAddress(kernelBase, "LoadLibraryExW"))
                   : nullptr;
    if (!loader ||
        !WindhawkUtils::SetFunctionHook(loader, LoadLibraryExW_Hook,
                                        &g_origLoadLibraryExW)) {
        Wh_Log(L"Could not hook LoadLibraryExW");
        return FALSE;
    }
    return TRUE;
}

void Wh_ModAfterInit() {
    if (!g_isBrowserProcess) {
        return;
    }
    // chrome.dll was loaded before the loader hook became active.
    if (const HMODULE chrome = GetModuleHandleW(L"chrome.dll")) {
        bool hasWindows = false;
        EnumWindows(FindChromeWindow, reinterpret_cast<LPARAM>(&hasWindows));
        AttemptChrome(chrome, !hasWindows);
    }
}

void Wh_ModSettingsChanged() {
    if (!g_isBrowserProcess) {
        return;
    }
    LoadSettings();
    SyncCaptions();
    if (g_active.load(std::memory_order_acquire)) {
        EnumWindows(UpdateBackdropWindow, 0);
    }
}

void Wh_ModBeforeUninit() {
    g_captionStopping.store(true, std::memory_order_release);
    g_captionFixActive.store(false, std::memory_order_release);
    AcquireSRWLockExclusive(&g_lock);
    g_stopping = true;
    g_active.store(false, std::memory_order_release);
    const HANDLE worker = g_worker;
    g_worker = nullptr;
    WakeAllConditionVariable(&g_lookupChanged);
    ReleaseSRWLockExclusive(&g_lock);
    // Give WS_SYSMENU back while the style hooks are still installed.
    if (g_isBrowserProcess && g_captionHooksInstalled) {
        EnumWindows(SyncCaptionWindow, 0);
    }
    if (g_isBrowserProcess) {
        EnumWindows(ForgetBrowserFrame, 0);
    }
    // Windhawk's symbol lookup cannot be cancelled; it must finish before the
    // mod DLL unloads.
    if (worker) {
        WaitForSingleObject(worker, INFINITE);
        CloseHandle(worker);
    }
}
