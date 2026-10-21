// ==WindhawkMod==
// @id              brave-acrylic
// @name            Brave Acrylic
// @description     Alpha: Acrylic toolbar and vertical tabs for Brave; requires Translucent Windows
// @version         2.8.2
// @author          bardelyne
// @github          https://github.com/bardelyne
// @homepage        https://github.com/bardelyne/brave-acrylic
// @license         MIT
// @architecture    x86-64
// @include         brave.exe
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Brave Acrylic

![Brave Acrylic with the built-in blue theme](https://raw.githubusercontent.com/bardelyne/brave-acrylic/main/assets/brave-acrylic-v2.8.png)

Brave Acrylic makes selected toolbar and vertical-tab backgrounds transparent so the desktop backdrop shows through. Regular, private, and Tor windows are checked against their own themes, so blue, purple, and pink windows can work together.

> **Alpha software.** This mod is still being tested and may need fixes after Brave updates. **[Translucent Windows](https://windhawk.net/mods/translucent-windows) is required** and must be enabled for `brave.exe` to provide the background effect.

## Requirements

- Windows 11 and the **x64 version of Brave**.
- [Windhawk](https://windhawk.net/) with **Brave Acrylic** enabled.
- The **[Translucent Windows](https://windhawk.net/mods/translucent-windows)** mod, configured to apply a translucent background effect to `brave.exe`.
- Windows **Transparency effects** enabled.

## Fast setup

**Go to Brave Settings → Appearance, select Dark appearance, then choose the built-in Blue color theme.** The default color list is already configured for it.

1. Enable **Brave Acrylic** and the required **Translucent Windows** mod. In Translucent Windows, apply the background effect you want to `brave.exe`.
2. Open `brave://settings/appearance`, select **Dark** appearance, and choose the built-in **Blue** theme.
3. Enable **vertical tabs** for the layout in the screenshot.
4. Close all Brave windows normally and reopen Brave.

> **Seeing two sets of minimize, maximize, and close buttons?** Enable Brave's **Use compact mode** option in Appearance. The same fix is explained in the mod's Settings tab.

### Install from source

Open [brave-acrylic.wh.cpp](https://github.com/bardelyne/brave-acrylic/blob/main/brave-acrylic.wh.cpp), choose **Raw**, and copy the source. In Windhawk, open **Create a new mod**, replace the editor contents with that source, then compile and install it. No additional headers or DLLs are needed.

## Advanced setup

### Choose your own colors

Open Brave Acrylic's **Settings** tab and edit **Colours to make transparent**. Add one six-digit RGB value per row, without an alpha channel.

| Default color | Surface in the built-in Dark + Blue theme |
| --- | --- |
| `333C4D` | Vertical tab strip |
| `1D2636` | Main toolbar |

**The window's original main toolbar color must be in the list.** That enables the effect for its palette. Other configured colors can then become transparent in that palette.

To find another theme's colors:

1. Temporarily disable Brave Acrylic and restart Brave to restore its original, opaque UI colors.
2. Take a screenshot and sample a flat area of the toolbar and sidebar. Avoid text, icons, borders, shadows, and hover effects.
3. Add the sampled RGB values to the mod's settings, enable the mod, and restart Brave normally.

Sample the **original paint color**, not an area already blended with acrylic. Enter colors as `RRGGBB`; a leading `#` is also accepted. Up to **16 colors** are supported. An empty list restores the built-in defaults. Invalid rows are reported in the log and skipped; later valid rows are still used.

Matching is exact. There is no tolerance setting that could also remove nearby colors.

### Blue, purple, and pink together

Keep the blue theme's entries and add the original toolbar and sidebar colors for a purple private window or a pink Tor window. Each window uses its own native color provider, so enabling one theme does not blindly clear another window's background.

An unlisted toolbar color keeps that palette's normal background painting. There are no hardcoded purple or pink presets: private and Tor windows use the same check as ordinary windows.

The color list is shared across eligible palettes. An exact listed color can also appear in text, buttons, or popup backgrounds. Add colors carefully: adding a common foreground color such as pure white can remove text too.

### Restart after changing colors

Restart Brave normally after enabling, disabling, or changing the mod. Brave caches brushes and opacity information when windows are created. Saving new colors does not rebuild every cached surface. Close **all** Brave windows normally and reopen Brave. If Brave is configured to keep running in the background, exit that background instance normally as well.

### Blur, tint, and backdrop

Use the required **Translucent Windows** mod to choose the background effect and tint for `brave.exe`. Brave Acrylic selects which original browser colors become transparent; Translucent Windows supplies the backdrop visible through them.

These are different controls: the color list selects paint to remove, while the backdrop settings control what you see behind it.

### Diagnostics

Turn on **Log colours (diagnostics)** in the mod's Settings tab to report original color IDs and RGB values. Capture them through Windhawk’s **Advanced → Debug logging → Detailed debug logs → Show log output**, then turn diagnostics off again.

Settings and the signature cache use Windhawk’s own storage. No separate configuration file or administrator access is needed to edit the mod settings.

The [advanced reference](https://github.com/bardelyne/brave-acrylic/blob/main/docs/advanced.md) explains theme detection, log collection, startup behavior, and cache validation.

## How it works

1. **Find Brave's native drawing functions.** A loader hook initializes the mod when `chrome.dll` is loaded, before Brave's caller starts using it. The resolver scans actual x64 function entries, with one unique match required for each function. There is no startup polling thread.
2. **Discover the toolbar color ID.** It finds `kColorToolbar` in Brave's own named-color tables. The enum number is discovered rather than fixed in the mod, and duplicate definitions must agree.
3. **Check the original palette for each window.** The window's native color provider supplies its original toolbar color. An unmatched or unavailable palette keeps its original background painting.
4. **Remove matching paint and clear old pixels.** Exact listed opaque colors become transparent. An allowlist of browser containers is cleared on repaint so previous tab labels and backgrounds do not leave trails. Interactive child views are not blanket-cleared.
5. **Let Translucent Windows supply the backdrop.** Brave Acrylic leaves window styles, fullscreen transitions, and backdrop selection alone. Its GPU hook only adjusts compatible RGBA composition surfaces; HWND, YUV, and protected-video surfaces keep their original settings.

Function addresses are cached in Windhawk’s per-mod storage for the exact `chrome.dll` build and rechecked before use. The named-color table is checked once per browser launch. There is no screenshot polling, webpage injection, or recurring full-module scan.

Function discovery is local: **no Brave debug-symbol download is used**.

## Compatibility

- Tested on **Windows 11 26H2**, **Brave 1.97.59** / **Chromium 155.0.8059.85**, x64.
- The per-window check has been tested with regular, private, and Tor window themes.
- Native ARM64 and 32-bit Brave are not supported. Unsupported native processes are refused before any hooks are installed.
- Website backgrounds remain under the website's control. Ordinary webpages are not made transparent by this mod.

**This is an alpha release, not a guarantee of future Brave compatibility.** The resolver tolerates changing relocation addresses and validates cached function boundaries, but it still works with private Brave functions. If a required function or the named-color table cannot be identified, the transparency path stays disabled.

## Troubleshooting

| Symptom | What to check |
| --- | --- |
| No transparency | Enable **both mods**, apply Translucent Windows to `brave.exe`, choose Dark + built-in Blue, then restart Brave. |
| Two sets of minimize/maximize/close buttons | Enable Brave's **Use compact mode** in Appearance, as explained in the mod settings. |
| Private or Tor window stays opaque | Add that theme's **original main toolbar color** and any sidebar colors you want removed. An unmatched palette stays opaque deliberately. |
| Text or icons disappear | Remove the color that also matches those elements, then restart Brave. Avoid adding common foreground colors. |
| Effect stops after a Brave update | Collect [Windhawk debug output](https://github.com/bardelyne/brave-acrylic/blob/main/docs/advanced.md#collect-logs) and include your exact Brave version in a bug report. |
| Old colors remain after saving settings | Exit every Brave window and any background instance normally, then reopen Brave. |

For a bug report, include Windows and Brave versions, the theme and window type, your color list, Translucent Windows settings, and steps to reproduce. Avoid sharing private browsing information in screenshots or logs.

## Credits and license

Created by [bardelyne](https://github.com/bardelyne) with AI assistance. Built for [Windhawk](https://windhawk.net/). The required [Translucent Windows](https://windhawk.net/mods/translucent-windows) mod provides the backdrop shown in the screenshot. Brave and Chromium provide the native UI and color-provider functions this mod works with.

[MIT license](https://github.com/bardelyne/brave-acrylic/blob/main/LICENSE).
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- colors:
  - "333C4D"
  - "1D2636"
  $name: Colours to make transparent
  $description: >-
    Exact RRGGBB colours. Include the original main toolbar colour to enable
    its theme. Requires Translucent Windows; see the README for setup.
    If minimize, maximize, and close buttons appear twice, enable Use compact
    mode in Brave Settings > Appearance.
- logColors: false
  $name: Log colours (diagnostics)
  $description: >-
    Log original native colour IDs and RGB values in Windhawk to identify
    theme colours. Enable Windhawk debug logging to collect them, then turn
    this off. No page contents or URLs are logged.
*/
// ==/WindhawkModSettings==

#include <windhawk_api.h>

#if !defined(__x86_64__) && !defined(_M_X64)

BOOL Wh_ModInit() {
    Wh_Log(L"Brave Acrylic requires x64 Brave; native ARM64 is not supported.");
    return FALSE;
}

#else

#include <dxgi1_2.h>
#include <atomic>
#include <cstdio>
#include <cstring>
#include <windhawk_utils.h>

static constexpr int kMaxTargetColors = 16;
static constexpr int kRequiredFunctions = 5;
static constexpr unsigned int kDefaultColors[] = {0x333C4D, 0x1D2636};

struct ColorSettings {
    unsigned int colors[kMaxTargetColors]{};
    int count = 0;
};

static SRWLOCK s_colorsLock = SRWLOCK_INIT;
static ColorSettings s_colors{{0x333C4D, 0x1D2636}, 2};
static std::atomic<bool> s_logColors{false};
static std::atomic<bool> s_transparencyReady{false};
static std::atomic<bool> s_chromeAttempted{false};
static SRWLOCK s_initLock = SRWLOCK_INIT;
static bool s_stopping = false;  // Protected by s_initLock.
static bool s_activateAfterInit = false;
static bool s_isBrowserProcess = false;
static HMODULE s_dxgiModule = nullptr;
static int s_toolbarColorId = -1;

struct StringView { const char* data; size_t length; };
using GetColor_t = unsigned int (*)(const void*, int);
using DrawColor_t = void (*)(void*, unsigned int, int);
using ViewGetClassNameW_t = StringView* (*)(const void*, StringView*);
using ViewGetColorProvider_t = const void* (*)(const void*);
using ViewOnPaintBackground_t = void (*)(void*, void*);
using LoadLibraryExW_t = decltype(&LoadLibraryExW);
using CreateSwapChainForComposition_t = HRESULT (WINAPI*)(
    IDXGIFactory2*, IUnknown*, const DXGI_SWAP_CHAIN_DESC1*, IDXGIOutput*, IDXGISwapChain1**);

static GetColor_t pOrigGetColor = nullptr;
static DrawColor_t pCanvasDrawColor = nullptr;
static ViewGetClassNameW_t pViewGetClassNameW = nullptr;
static ViewGetColorProvider_t pViewGetColorProvider = nullptr;
static ViewOnPaintBackground_t pOrigViewOnPaintBackground = nullptr;
static LoadLibraryExW_t pOrigLoadLibraryExW = nullptr;
static CreateSwapChainForComposition_t pOrigCreateSwapChainForComposition = nullptr;

static bool IsColorSpace(wchar_t ch) {
    return ch == L' ' || ch == L'\t' || ch == L'\r' || ch == L'\n';
}

static int ParseHexColor(PCWSTR text) {
    if (!text) return -1;
    while (IsColorSpace(*text)) ++text;
    if (*text == L'#') ++text;
    unsigned int value = 0;
    for (int i = 0; i < 6; ++i, ++text) {
        unsigned int digit;
        if (*text >= L'0' && *text <= L'9') digit = *text - L'0';
        else if (*text >= L'a' && *text <= L'f') digit = *text - L'a' + 10;
        else if (*text >= L'A' && *text <= L'F') digit = *text - L'A' + 10;
        else return -1;
        value = (value << 4) | digit;
    }
    while (IsColorSpace(*text)) ++text;
    return *text ? -1 : static_cast<int>(value);
}

static void LoadModSettings() {
    ColorSettings next{};
    for (int i = 0; i < kMaxTargetColors; ++i) {
        const auto setting = WindhawkUtils::StringSetting::make(L"colors[%d]", i);
        PCWSTR text = setting.get();
        if (!text) continue;
        while (IsColorSpace(*text)) ++text;
        if (!*text) continue;
        const int color = ParseHexColor(text);
        if (color < 0) {
            Wh_Log(L"Invalid colour row %d: enter exactly six RGB hex digits; row skipped", i + 1);
            continue;
        }
        next.colors[next.count++] = static_cast<unsigned int>(color);
    }
    if (!next.count) {
        memcpy(next.colors, kDefaultColors, sizeof(kDefaultColors));
        next.count = ARRAYSIZE(kDefaultColors);
    }

    AcquireSRWLockExclusive(&s_colorsLock);
    s_colors = next;
    ReleaseSRWLockExclusive(&s_colorsLock);
    s_logColors.store(Wh_GetIntSetting(L"logColors") != 0, std::memory_order_relaxed);

    char list[kMaxTargetColors * 7 + 1]{};
    size_t written = 0;
    for (int i = 0; i < next.count; ++i) {
        written += std::snprintf(list + written, sizeof(list) - written, "%06X ", next.colors[i]);
    }
    Wh_Log(L"settings: %d exact colour(s) [ %S]", next.count, list);
}

// Caller must hold s_colorsLock. Never hold this lock across native calls.
static bool ContainsColor(const ColorSettings& settings, unsigned int color) {
    for (int i = 0; i < settings.count; ++i) {
        if (settings.colors[i] == color) return true;
    }
    return false;
}

static bool IsThemeBackground(unsigned int color) {
    AcquireSRWLockShared(&s_colorsLock);
    const bool found = ContainsColor(s_colors, color);
    ReleaseSRWLockShared(&s_colorsLock);
    return found;
}

static bool HasCompleteTransparencyPath() {
    return pOrigGetColor && pCanvasDrawColor && pViewGetClassNameW &&
           pViewGetColorProvider && pOrigViewOnPaintBackground && s_toolbarColorId >= 0;
}

static bool ReadToolbarColor(const void* provider, unsigned int* color) {
    if (!s_transparencyReady.load(std::memory_order_acquire) || !provider ||
        !pOrigGetColor || s_toolbarColorId < 0) return false;
    *color = pOrigGetColor(provider, s_toolbarColorId);
    // gfx::kPlaceholderColor denotes a missing native colour ID.
    return *color != 0xFFFF0000 && (*color >> 24) == 0xFF;
}

static bool ThemeMatches(const void* provider) {
    unsigned int toolbar;
    return ReadToolbarColor(provider, &toolbar) && IsThemeBackground(toolbar & 0xFFFFFF);
}

static bool ShouldReplaceColor(const void* provider, unsigned int color) {
    if ((color >> 24) != 0xFF || !IsThemeBackground(color & 0xFFFFFF)) return false;
    unsigned int toolbar;
    if (!ReadToolbarColor(provider, &toolbar)) return false;
    // Recheck both colours in one published settings snapshot after the native call.
    AcquireSRWLockShared(&s_colorsLock);
    const bool replace = ContainsColor(s_colors, color & 0xFFFFFF) &&
                         ContainsColor(s_colors, toolbar & 0xFFFFFF);
    ReleaseSRWLockShared(&s_colorsLock);
    return replace;
}

static bool CanClearForTheme(const void* view) {
    return view && s_transparencyReady.load(std::memory_order_acquire) &&
           pViewGetColorProvider && ThemeMatches(pViewGetColorProvider(view));
}

static bool ShouldClearView(const void* view) {
    if (!view || !pViewGetClassNameW) return false;
    StringView name{};
    pViewGetClassNameW(view, &name);
    if (!name.data || !name.length || name.length >= 128) return false;
    static constexpr const char* containers[] = {
        "BrowserView", "BraveBrowserView",
        "VerticalTabStripRegionView", "BraveVerticalTabStripRegionView",
        "VerticalTabStripContainerView", "BraveVerticalTabStripContainerView",
        "TopContainerView", "ToolbarView", "BraveToolbarView",
        "BookmarkBarView", "BraveBookmarkBarView", "BrowserFrame",
        "ContentsSeparator", "RoundedSeparator", "ContentsBackground",
        "VerticalTabStripBackgroundBlurBackdrop",
    };
    for (const char* container : containers) {
        if (strlen(container) == name.length &&
            memcmp(name.data, container, name.length) == 0) return CanClearForTheme(view);
    }
    return false;
}

static void LogDiscoveredColor(int id, unsigned int color) {
    if (!s_logColors.load(std::memory_order_relaxed) || (color >> 24) != 0xFF) return;
    struct Entry { int id; unsigned int color; };
    static SRWLOCK lock = SRWLOCK_INIT;
    static Entry seen[256]{};
    static unsigned count = 0;
    bool fresh = false;
    AcquireSRWLockExclusive(&lock);
    unsigned i = 0;
    for (; i < count; ++i) {
        if (seen[i].id == id && seen[i].color == color) break;
    }
    if (i == count && count < ARRAYSIZE(seen)) {
        seen[count++] = {id, color};
        fresh = true;
    }
    ReleaseSRWLockExclusive(&lock);
    if (fresh) Wh_Log(L"colour id %d -> #%06X match=%d", id, color & 0xFFFFFF,
                      static_cast<int>(IsThemeBackground(color & 0xFFFFFF)));
}

static unsigned int GetColor_Hook(const void* provider, int id) {
    const unsigned int color = pOrigGetColor ? pOrigGetColor(provider, id) : 0;
    LogDiscoveredColor(id, color);
    return s_transparencyReady.load(std::memory_order_acquire) &&
           ShouldReplaceColor(provider, color) ? 0 : color;
}

static void ViewOnPaintBackground_Hook(void* view, void* canvas) {
    if (s_transparencyReady.load(std::memory_order_acquire) && canvas && ShouldClearView(view)) {
        pCanvasDrawColor(canvas, 0, 0 /* SkBlendMode::kClear */);
        return;
    }
    if (pOrigViewOnPaintBackground) pOrigViewOnPaintBackground(view, canvas);
}

static bool SupportsCompositionAlpha(DXGI_FORMAT format) {
    switch (format) {
        case DXGI_FORMAT_B8G8R8A8_UNORM:
        case DXGI_FORMAT_R8G8B8A8_UNORM:
        case DXGI_FORMAT_R16G16B16A16_FLOAT:
        case DXGI_FORMAT_R10G10B10A2_UNORM:
            return true;
        default:
            return false;
    }
}

static HRESULT WINAPI CreateSwapChainForComposition_Hook(
    IDXGIFactory2* factory, IUnknown* device, const DXGI_SWAP_CHAIN_DESC1* desc,
    IDXGIOutput* output, IDXGISwapChain1** swapChain) {
    if (!pOrigCreateSwapChainForComposition) return E_UNEXPECTED;
    if (!desc || !swapChain ||
        (desc->Flags & (DXGI_SWAP_CHAIN_FLAG_YUV_VIDEO | DXGI_SWAP_CHAIN_FLAG_HW_PROTECTED |
                        DXGI_SWAP_CHAIN_FLAG_RESTRICTED_CONTENT)) ||
        !SupportsCompositionAlpha(desc->Format) ||
        (desc->AlphaMode != DXGI_ALPHA_MODE_IGNORE &&
         desc->AlphaMode != DXGI_ALPHA_MODE_UNSPECIFIED)) {
        return pOrigCreateSwapChainForComposition(factory, device, desc, output, swapChain);
    }
    DXGI_SWAP_CHAIN_DESC1 adjusted = *desc;
    adjusted.AlphaMode = DXGI_ALPHA_MODE_PREMULTIPLIED;
    IDXGISwapChain1* created = nullptr;
    const HRESULT result = pOrigCreateSwapChainForComposition(
        factory, device, &adjusted, output, &created);
    if (SUCCEEDED(result)) {
        *swapChain = created;
        return result;
    }
    if (created) created->Release();
    if (result == DXGI_ERROR_INVALID_CALL || result == DXGI_ERROR_UNSUPPORTED) {
        return pOrigCreateSwapChainForComposition(factory, device, desc, output, swapChain);
    }
    *swapChain = nullptr;
    return result;
}

static bool InstallGpuHooks() {
    HMODULE module = LoadLibraryExW(L"dxgi.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!module) return false;
    using CreateFactory_t = HRESULT (WINAPI*)(REFIID, void**);
    const auto createFactory = reinterpret_cast<CreateFactory_t>(
        GetProcAddress(module, "CreateDXGIFactory1"));
    IDXGIFactory2* factory = nullptr;
    if (!createFactory || FAILED(createFactory(__uuidof(IDXGIFactory2),
                                               reinterpret_cast<void**>(&factory))) || !factory) {
        if (factory) factory->Release();
        FreeLibrary(module);
        return false;
    }
    // Only composition swap chains support this alpha override. Never hook HWND chains.
    const auto target = reinterpret_cast<CreateSwapChainForComposition_t>(
        (*reinterpret_cast<void***>(factory))[24]);
    const bool installed = target && WindhawkUtils::SetFunctionHook(
        target, CreateSwapChainForComposition_Hook, &pOrigCreateSwapChainForComposition);
    factory->Release();
    if (!installed) {
        FreeLibrary(module);
        return false;
    }
    // Keep the hook's module loaded until Windhawk has removed the hook.
    s_dxgiModule = module;
    return true;  // Windhawk applies the hook after Wh_ModInit returns.
}


struct Signature {
    const char* name;
    const unsigned char* bytes;
    const char* mask;
    size_t length;
};

static constexpr unsigned char kSigGetColor[] = {
    0x41, 0x57, 0x41, 0x56, 0x41, 0x54, 0x56, 0x57, 0x55, 0x53, 0x48, 0x83,
    0xEC, 0x20, 0x89, 0xD7, 0x48, 0x89, 0xCE, 0x4C, 0x8D, 0x71, 0x18, 0x48,
    0x8B, 0x41, 0x18, 0xEB, 0x09, 0x0F, 0x1F, 0x00, 0x7E, 0x18, 0x48, 0x8B,
    0x40, 0x08, 0x48, 0x85, 0xC0, 0x74, 0x0D, 0x3B, 0x78, 0x20, 0x7D, 0xF0,
    0x48, 0x8B, 0x00, 0x48, 0x85, 0xC0, 0x75, 0xF3, 0x31, 0xC0, 0x48, 0x85,
    0xC0, 0x49, 0x0F, 0x44, 0xC6, 0x4C, 0x39, 0xF0, 0x74, 0x14, 0x8B, 0x68,
    0x24, 0x89, 0xE8, 0x48, 0x83, 0xC4, 0x20, 0x5B, 0x5D, 0x5F, 0x5E, 0x41,
    0x5C, 0x41, 0x5E, 0x41, 0x5F, 0xC3, 0x48, 0x8B, 0x0E, 0xBD, 0x00, 0x00,
    0xFF, 0xFF, 0x48, 0x85, 0xC9, 0x74, 0xE2, 0x48, 0x83, 0xC1, 0x08, 0x89,
    0xFA, 0xE8, 0xDE, 0x97, 0xE2, 0xFE, 0x3D, 0x00, 0x00, 0xFF, 0xFF, 0x74,
    0xD0};
static constexpr char kMaskGetColor[] =
    "xxxxxxxxxxxxxxxxxxxxxx?xxx?x?xxxx?xxx?xxxx?xx?x?xxxxxxx?xxxxxxxxxxxxx?xx"
    "?xxxxxxxxxxxxxxxxxxxxxxxxxxxxx?xxxxxxx????xxxxxx?";

static constexpr unsigned char kSigOnPaintBackground[] = {
    0x56, 0x57, 0x48, 0x83, 0xEC, 0x38, 0x48, 0x89, 0xCE, 0x48, 0x8B, 0x05,
    0x70, 0x04, 0x7B, 0x12, 0x48, 0x31, 0xE0, 0x48, 0x89, 0x44, 0x24, 0x30,
    0x48, 0x8B, 0x89, 0x28, 0x02, 0x00, 0x00, 0x48, 0x85, 0xC9, 0x75, 0x1B,
    0x48, 0x8B, 0x44, 0x24, 0x30, 0x48, 0x31, 0xE0, 0x48, 0x8B, 0x0D, 0x4D,
    0x04, 0x7B, 0x12, 0x48, 0x39, 0xC1, 0x75, 0x64, 0x48, 0x83, 0xC4, 0x38,
    0x5F, 0x5E, 0xC3, 0x48, 0x89, 0xD7, 0x48, 0xC7, 0x44, 0x24, 0x20, 0xA2,
    0x00, 0x00, 0x00, 0x8A, 0x05, 0xC1, 0x40, 0x94, 0x12, 0x84, 0xC0, 0x75,
    0x54, 0x48, 0x8B, 0x01, 0x48, 0x8B, 0x40, 0x20, 0x48, 0x89, 0xFA, 0x49,
    0x89, 0xF0, 0xFF, 0x15, 0xD0, 0x57, 0x75, 0x12};
static constexpr char kMaskOnPaintBackground[] =
    "xxxxxxxxxxxx????xxxxxxx?xxx????xxxx?xxxx?xxxxxx????xxxx?xxxxxxxxxxxxxx??"
    "???xx????xxx?xxxxxx?xxxxxxxx????";

// The explicit blend-mode overload forwards r8d to the canvas call. The other
// overload inserts 'mov r8d, kSrcOver' here. Match through that distinction,
// rather than choosing an overload by its position in chrome.dll. Relocations,
// the canvas member displacement and the virtual-call slot are wildcarded.
static const unsigned char kSigDrawColor[] = {
    0x48, 0x83, 0xEC, 0x38, 0x48, 0x8B, 0x05, 0, 0, 0, 0, 0x48,
    0x31, 0xE0, 0x48, 0x89, 0x44, 0x24, 0x30, 0x48, 0x8B, 0x89, 0, 0,
    0, 0, 0x66, 0x0F, 0x6E, 0xC2, 0x66, 0x0F, 0xEF, 0xC9, 0x66, 0x0F,
    0x60, 0xC1, 0x66, 0x0F, 0x61, 0xC1, 0x0F, 0x5B, 0xC0, 0x0F, 0x28, 0x0D,
    0, 0, 0, 0, 0x0F, 0xC6, 0xC0, 0xC6, 0x0F, 0xC6, 0xC9, 0xC6,
    0x0F, 0x59, 0xC8, 0x48, 0x8D, 0x54, 0x24, 0x20, 0x0F, 0x29, 0x0A,
    0x48, 0x8B, 0x01, 0x48, 0x8B, 0x80, 0, 0, 0, 0, 0xFF, 0x15, 0, 0, 0, 0};
static const char kMaskDrawColor[] =
    "xxxxxxx????xxxxxxxxxxx????xxxxxxxxxxxxxxxxxxxxxx????"
    "xxxxxxxxxxxxxxxxxxxxxxxxx????xx????";

static constexpr unsigned char kSigGetClassNameW[] = {
    0x56, 0x48, 0x83, 0xEC, 0x20, 0x48, 0x89, 0xD6, 0x48, 0x8B, 0x81, 0x00,
    0x01, 0x00, 0x00, 0x48, 0x81, 0xC1, 0x00, 0x01, 0x00, 0x00, 0x48, 0x8B,
    0x40, 0x08, 0xFF, 0x15, 0x88, 0x9B, 0x33, 0x0F, 0x0F, 0x10, 0x40, 0x08,
    0x0F, 0x11, 0x06, 0x48, 0x89, 0xF0, 0x48, 0x83, 0xC4, 0x20, 0x5E, 0xC3};
static constexpr char kMaskGetClassNameW[] =
    "xxxxxxxxxxx????xxx????xxx?xx????xxx?xxxxxxxxxxxx";

static constexpr unsigned char kSigViewColorProvider[] = {
    0x48, 0x83, 0xEC, 0x28, 0x48, 0x8B, 0x01, 0x48, 0x8B, 0x40, 0x58, 0x48,
    0x8D, 0x15, 0x3E, 0xAE, 0x61, 0x00, 0x48, 0x39, 0xD0, 0x75, 0x2D, 0x48,
    0x8B, 0x89, 0x90, 0x01, 0x00, 0x00, 0x48, 0x85, 0xC9, 0x74, 0x37, 0x48,
    0x8B, 0x41, 0x48, 0x48, 0x83, 0xC1, 0x48, 0x48, 0x8B, 0x40, 0x08, 0x48,
    0x8D, 0x15, 0x3A, 0x00, 0x00, 0x00, 0x48, 0x39, 0xD0, 0x75, 0x14, 0x48,
    0x83, 0xC4, 0x28, 0xE9, 0x2C, 0x00, 0x00, 0x00, 0xFF, 0x15, 0x3E, 0x7F,
    0x12, 0x0D, 0x48, 0x89, 0xC1, 0xEB, 0xCF, 0x48, 0x83, 0xC4, 0x28, 0x48,
    0xFF, 0x25, 0x2E, 0x7F, 0x12, 0x0D, 0x31, 0xC0, 0x48, 0x83, 0xC4, 0x28,
    0xC3};
static constexpr char kMaskViewColorProvider[] =
    "xxxxxxxxxx?xxx????xxxx?xxx????xxxx?xxx?xxxxxxx?xxx????xxxx?xxxxx????xx??"
    "??xxxx?xxxxxxx????xxxxxxx";
static_assert(sizeof(kMaskViewColorProvider) == sizeof(kSigViewColorProvider) + 1);
static_assert(sizeof(kMaskGetColor) == sizeof(kSigGetColor) + 1);
static_assert(sizeof(kMaskOnPaintBackground) == sizeof(kSigOnPaintBackground) + 1);
static_assert(sizeof(kMaskDrawColor) == sizeof(kSigDrawColor) + 1);
static_assert(sizeof(kMaskGetClassNameW) == sizeof(kSigGetClassNameW) + 1);


// Discover the toolbar ID from Chromium's own read-only {id, name} tables.
// IDs can move between releases; do not hardcode them or read View member offsets.
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

static bool ReadColorName(const ReadOnlyImage& image, size_t row,
                          unsigned long long* id, size_t* nameRva, size_t* nameLength) {
    if (!image.Contains(row, 16)) return false;
    unsigned long long pointer;
    memcpy(id, image.base + row, 8);
    memcpy(&pointer, image.base + row + 8, 8);
    const auto baseAddress = reinterpret_cast<ULONG_PTR>(image.base);
    if (!*id || *id >= 0x10000 || pointer < baseAddress ||
        pointer - baseAddress >= image.size) return false;
    *nameRva = static_cast<size_t>(pointer - baseAddress);
    // All reads, including the terminator, stay inside read-only module data.
    for (size_t length = 0; length < 128; ++length) {
        if (!image.Contains(*nameRva, length + 1)) return false;
        if (!image.base[*nameRva + length]) {
            if (length < 7 || memcmp(image.base + *nameRva, "kColor", 6)) return false;
            *nameLength = length;
            return true;
        }
    }
    return false;
}

static int FindToolbarColorId(HMODULE mod) {
    ReadOnlyImage image{};
    if (!GetReadOnlyImage(mod, &image)) return -1;
    int found = -1;
    for (unsigned i = 0; i < image.count; ++i) {
        const auto& section = image.sections[i];
        const size_t start = section.VirtualAddress;
        const size_t size = section.Misc.VirtualSize;
        if (size < 48 || !image.Contains(start, size)) continue;
        for (size_t offset = 16; offset <= size - 32; offset += 8) {
            const size_t row = start + offset;
            // Most data is not a colour row. Reject it before inspecting pointers.
            unsigned long long id;
            memcpy(&id, image.base + row, 8);
            if (!id || id >= 0x10000) continue;
            size_t name, length;
            if (!ReadColorName(image, row, &id, &name, &length) ||
                length != sizeof("kColorToolbar") - 1 ||
                memcmp(image.base + name, "kColorToolbar", sizeof("kColorToolbar")))
                continue;
            unsigned long long previous, next;
            size_t neighborName, neighborLength;
            if (!ReadColorName(image, row - 16, &previous, &neighborName, &neighborLength) ||
                !ReadColorName(image, row + 16, &next, &neighborName, &neighborLength) ||
                previous + 1 != id || id + 1 != next) continue;
            // Duplicate tables are normal, but disagreeing definitions are unsafe.
            if (found >= 0 && found != static_cast<int>(id)) return -1;
            found = static_cast<int>(id);
        }
    }
    return found;
}

static bool GetTextSection(HMODULE mod, const unsigned char** outBase, size_t* outSize) {
    auto dos = (const IMAGE_DOS_HEADER*)mod;
    if (!dos || dos->e_magic != IMAGE_DOS_SIGNATURE) return false;
    auto nt = (const IMAGE_NT_HEADERS64*)((const unsigned char*)mod + dos->e_lfanew);
    if (nt->Signature != IMAGE_NT_SIGNATURE) return false;
    auto sec = IMAGE_FIRST_SECTION(nt);
    for (unsigned i = 0; i < nt->FileHeader.NumberOfSections; ++i) {
        if (memcmp(sec[i].Name, ".text", 5) == 0) {
            *outBase = (const unsigned char*)mod + sec[i].VirtualAddress;
            *outSize = sec[i].Misc.VirtualSize;
            return true;
        }
    }
    return false;
}

static bool MatchesAt(const unsigned char* p, const Signature& sig) {
    for (size_t j = 0; j < sig.length; ++j) {
        if (sig.mask[j] == 'x' && p[j] != sig.bytes[j]) return false;
    }
    return true;
}

struct ScanHit {
    const unsigned char* first;
    int count;
};

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

static bool ScanAll(HMODULE mod, const unsigned char* text, size_t size,
                    const Signature* sigs, int count, ScanHit* out) {
    unsigned int first4[5]{};
    if (count <= 0 || count > static_cast<int>(ARRAYSIZE(first4))) return false;
    for (int i = 0; i < count; ++i) {
        out[i] = {};
        if (sigs[i].length < 4) return false;
        memcpy(&first4[i], sigs[i].bytes, 4);
    }
    const RUNTIME_FUNCTION* functions;
    size_t functionCount;
    if (!GetFunctionTable(mod, &functions, &functionCount)) return false;
    const auto* base = reinterpret_cast<const unsigned char*>(mod);
    const size_t textRva = text - base;
    for (size_t i = 0; i < functionCount; ++i) {
        const auto& f = functions[i];
        if (f.BeginAddress < textRva || f.BeginAddress - textRva > size ||
            f.EndAddress <= f.BeginAddress) continue;
        const size_t remaining = size - (f.BeginAddress - textRva);
        if (remaining < 4) continue;
        const auto* at = base + f.BeginAddress;
        unsigned int prefix;
        memcpy(&prefix, at, 4);
        for (int s = 0; s < count; ++s) {
            if (prefix != first4[s] || sigs[s].length > remaining ||
                sigs[s].length > f.EndAddress - f.BeginAddress ||
                !MatchesAt(at, sigs[s])) continue;
            if (++out[s].count == 1) out[s].first = at;
        }
    }
    return true;
}


// A small value in Windhawk's per-mod storage; removed with the mod.
struct SignatureCache {
    unsigned long long key;
    unsigned int count;
    unsigned int reserved;
    unsigned long long rva[kRequiredFunctions];
    char version[64];
};
static_assert(sizeof(SignatureCache) == 120);

static unsigned long long BuildKey(HMODULE module) {
    const auto* dos = reinterpret_cast<const IMAGE_DOS_HEADER*>(module);
    const auto* nt = reinterpret_cast<const IMAGE_NT_HEADERS64*>(
        reinterpret_cast<const unsigned char*>(module) + dos->e_lfanew);
    constexpr unsigned long long signatureRevision = 13;
    return ((static_cast<unsigned long long>(nt->FileHeader.TimeDateStamp) << 32) ^
            nt->OptionalHeader.SizeOfImage) + signatureRevision;
}

static void GetBraveVersion(HMODULE module, char* output, size_t size) {
    if (!size) return;
    output[0] = 0;
    wchar_t path[MAX_PATH]{};
    if (!GetModuleFileNameW(module, path, ARRAYSIZE(path))) return;
    wchar_t* slash = wcsrchr(path, L'\\');
    if (!slash) return;
    *slash = 0;
    wchar_t* version = wcsrchr(path, L'\\');
    if (!version) return;
    WideCharToMultiByte(CP_UTF8, 0, version + 1, -1, output, static_cast<int>(size), nullptr, nullptr);
}

static bool LoadCache(unsigned long long key, size_t* rva, int count,
                      char* previousVersion, size_t previousSize) {
    if (previousVersion && previousSize) previousVersion[0] = 0;
    if (count != kRequiredFunctions) return false;
    SignatureCache cache{};
    if (Wh_GetBinaryValue(L"signatureCache", &cache, sizeof(cache)) != sizeof(cache) ||
        cache.count != kRequiredFunctions || cache.reserved ||
        !memchr(cache.version, 0, sizeof(cache.version))) return false;
    if (previousVersion && previousSize) {
        std::snprintf(previousVersion, previousSize, "%s", cache.version);
    }
    if (cache.key != key) return false;
    for (int i = 0; i < count; ++i) rva[i] = static_cast<size_t>(cache.rva[i]);
    return true;
}

static void SaveCache(unsigned long long key, const char* version, const size_t* rva, int count) {
    if (count != kRequiredFunctions) return;
    SignatureCache cache{};
    cache.key = key;
    cache.count = count;
    std::snprintf(cache.version, sizeof(cache.version), "%s", version && *version ? version : "?");
    for (int i = 0; i < count; ++i) cache.rva[i] = rva[i];
    if (!Wh_SetBinaryValue(L"signatureCache", &cache, sizeof(cache))) {
        Wh_Log(L"Could not save the signature cache; the next launch will rescan");
    }
}

static bool ResolveBySignature(HMODULE module) {
    const unsigned char* text = nullptr;
    size_t textSize = 0;
    if (!GetTextSection(module, &text, &textSize)) return false;
    const Signature signatures[kRequiredFunctions] = {
        {"ui::ColorProvider::GetColor", kSigGetColor, kMaskGetColor, sizeof(kSigGetColor)},
        {"views::View::OnPaintBackground", kSigOnPaintBackground, kMaskOnPaintBackground,
         sizeof(kSigOnPaintBackground)},
        {"gfx::Canvas::DrawColor", kSigDrawColor, kMaskDrawColor, sizeof(kSigDrawColor)},
        {"views::View::GetClassNameW", kSigGetClassNameW, kMaskGetClassNameW, sizeof(kSigGetClassNameW)},
        {"views::View::GetColorProvider", kSigViewColorProvider, kMaskViewColorProvider,
         sizeof(kSigViewColorProvider)},
    };
    auto* base = reinterpret_cast<unsigned char*>(module);
    const size_t textRva = text - base;
    const DWORD started = GetTickCount();
    const auto key = BuildKey(module);
    char version[64]{}, previousVersion[64]{};
    GetBraveVersion(module, version, sizeof(version));
    size_t rva[kRequiredFunctions]{};
    bool cached = LoadCache(key, rva, kRequiredFunctions, previousVersion, sizeof(previousVersion));
    if (cached) {
        for (int i = 0; i < kRequiredFunctions; ++i) {
            if (!rva[i] || rva[i] < textRva || rva[i] - textRva > textSize ||
                signatures[i].length > textSize - (rva[i] - textRva) ||
                !FunctionContains(module, rva[i], signatures[i].length) ||
                !MatchesAt(base + rva[i], signatures[i])) {
                cached = false;
                break;
            }
        }
    }
    if (!cached) {
        if (previousVersion[0]) Wh_Log(L"Cached Brave %S needs revalidation for %S", previousVersion, version);
        ScanHit hits[kRequiredFunctions]{};
        if (!ScanAll(module, text, textSize, signatures, kRequiredFunctions, hits)) return false;
        for (int i = 0; i < kRequiredFunctions; ++i) {
            if (hits[i].count != 1) {
                Wh_Log(L"%S: %d function matches; transparency stays off", signatures[i].name, hits[i].count);
                return false;
            }
            rva[i] = hits[i].first - base;
        }
    }

    pCanvasDrawColor = reinterpret_cast<DrawColor_t>(base + rva[2]);
    pViewGetClassNameW = reinterpret_cast<ViewGetClassNameW_t>(base + rva[3]);
    pViewGetColorProvider = reinterpret_cast<ViewGetColorProvider_t>(base + rva[4]);
    if (!WindhawkUtils::SetFunctionHook(reinterpret_cast<GetColor_t>(base + rva[0]),
                                       GetColor_Hook, &pOrigGetColor) ||
        !WindhawkUtils::SetFunctionHook(reinterpret_cast<ViewOnPaintBackground_t>(
                                           base + rva[1]),
                                       ViewOnPaintBackground_Hook, &pOrigViewOnPaintBackground) ||
        !HasCompleteTransparencyPath()) {
        Wh_Log(L"Required paint hooks could not be registered; transparency stays off");
        return false;
    }
    if (!cached) SaveCache(key, version, rva, kRequiredFunctions);
    Wh_Log(L"%s: %d/%d resolved in %u ms (Brave %S)",
           cached ? L"signature cache" : L"signature scan", kRequiredFunctions, kRequiredFunctions,
           GetTickCount() - started, version);
    return true;
}

static bool InstallChromeHooks(HMODULE module) {
    s_toolbarColorId = FindToolbarColorId(module);
    if (s_toolbarColorId < 0) {
        Wh_Log(L"Toolbar colour table missing or ambiguous; transparency stays off");
        return false;
    }
    Wh_Log(L"per-window theme guard: kColorToolbar ID=%d", s_toolbarColorId);
    return ResolveBySignature(module);
}

// Installation and shutdown share a barrier. No hook can be registered/applied
// after Wh_ModBeforeUninit returns, including a loader call that finishes late.
static bool TryInstallChromeHooks(HMODULE module, bool applyNow) {
    if (!module || s_chromeAttempted.exchange(true, std::memory_order_acq_rel)) return false;
    AcquireSRWLockExclusive(&s_initLock);
    bool installed = false;
    if (!s_stopping && InstallChromeHooks(module)) {
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
    return installed;
}

static bool IsExecutableLibraryLoad(DWORD flags) {
    return !(flags & (DONT_RESOLVE_DLL_REFERENCES | LOAD_LIBRARY_AS_DATAFILE |
                      LOAD_LIBRARY_AS_DATAFILE_EXCLUSIVE | LOAD_LIBRARY_AS_IMAGE_RESOURCE));
}

static HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR name, HANDLE file, DWORD flags) {
    const HMODULE module = pOrigLoadLibraryExW(name, file, flags);
    const DWORD error = GetLastError();
    if (module && IsExecutableLibraryLoad(flags) && module == GetModuleHandleW(L"chrome.dll")) {
        // Chrome's caller cannot use the new module until this hook returns.
        TryInstallChromeHooks(module, true);
    }
    SetLastError(error);
    return module;
}

BOOL Wh_ModInit() {
    const auto command = GetCommandLineW();
    if (wcsstr(command, L"--type=gpu-process")) return InstallGpuHooks();
    if (wcsstr(command, L"--type=")) return FALSE;
    s_isBrowserProcess = true;
    LoadModSettings();

    HMODULE chrome = GetModuleHandleW(L"chrome.dll");
    if (chrome) return TryInstallChromeHooks(chrome, false);

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
}

void Wh_ModSettingsChanged() {
    if (!s_isBrowserProcess) return;
    LoadModSettings();
    Wh_Log(L"Settings updated; restart Brave normally to rebuild its cached UI brushes");
}

void Wh_ModBeforeUninit() {
    AcquireSRWLockExclusive(&s_initLock);
    s_stopping = true;
    s_activateAfterInit = false;
    s_transparencyReady.store(false, std::memory_order_release);
    ReleaseSRWLockExclusive(&s_initLock);
}

void Wh_ModUninit() {
    if (s_dxgiModule) {
        FreeLibrary(s_dxgiModule);
        s_dxgiModule = nullptr;
    }
}

#endif  // x64 native resolver
