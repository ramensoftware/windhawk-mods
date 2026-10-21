// ==WindhawkMod==
// @id              brave-acrylic
// @name            Brave Acrylic
// @description     Alpha: Acrylic toolbar and vertical tabs for Brave; requires Translucent Windows
// @version         2.8.1
// @author          bardelyne
// @github          https://github.com/bardelyne
// @homepage        https://github.com/bardelyne/brave-acrylic
// @license         MIT
// @architecture    x86-64
// @include         brave.exe
// @compilerOptions -ldwmapi -lole32 -ldxgi -lcomctl32 -lgdi32
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

### Install before the Windhawk submission is merged

Open [brave-acrylic.wh.cpp](https://github.com/bardelyne/brave-acrylic/blob/main/brave-acrylic.wh.cpp), choose **Raw**, and copy the source. In Windhawk, open **Create a new mod**, replace the editor contents with that source, then compile and install it. No additional headers or DLLs are needed.

Once the mod is accepted into Windhawk's catalog, it can be installed from the catalog normally.

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

Sample the **original paint color**, not an area already blended with acrylic. Enter colors as `RRGGBB`; a leading `#` is also accepted. Up to **16 colors** are supported. An empty list retains the built-in defaults.

Matching is exact. There is no tolerance setting that could also remove nearby colors.

### Blue, purple, and pink together

Keep the blue theme's entries and add the original toolbar and sidebar colors for a purple private window or a pink Tor window. Each window uses its own native color provider, so enabling one theme does not blindly clear another window's background.

An unlisted toolbar color keeps that palette's normal background painting. There are no hardcoded purple or pink presets: private and Tor windows use the same check as ordinary windows.

The color list is shared across eligible palettes. An exact listed color can also appear in text, buttons, or popup backgrounds. Add colors carefully: adding a common foreground color such as pure white can remove text too.

### Restart after changing colors

Brave caches brushes and opacity information when windows are created. Saving new colors does not rebuild every cached surface. Close **all** Brave windows normally and reopen Brave. If Brave is configured to keep running in the background, exit that background instance normally as well.

### Blur, tint, and backdrop

Use the required **Translucent Windows** mod to choose the background effect and tint for `brave.exe`. Brave Acrylic selects which original browser colors become transparent; Translucent Windows supplies the backdrop visible through them.

These are different controls: the color list selects paint to remove, while the backdrop settings control what you see behind it.

### Diagnostics and optional switches

Most users only need the color list. Optional switches are read at browser startup from `C:\ProgramData\Windhawk\brave-acrylic.cfg`.

For example, `logColors=1` reports native color IDs and original RGB values through **Windhawk's debug logging**. Use it temporarily when identifying a theme, then turn it off.

The [advanced reference](https://github.com/bardelyne/brave-acrylic/blob/main/docs/advanced.md) explains every switch, log collection, and the resolver's safeguards.

## How it works

1. **Find Brave's native drawing functions.** The default resolver scans `chrome.dll` at actual x64 function entries. Each required function must have one unique signature match.
2. **Discover the toolbar color ID.** It finds `kColorToolbar` in Brave's own named-color tables. The enum number is discovered rather than fixed in the mod, and duplicate definitions must agree.
3. **Check the original palette for each window.** The window's native color provider supplies its original toolbar color. An unmatched or unavailable palette keeps its original background painting.
4. **Remove matching paint and clear old pixels.** Exact listed opaque colors become transparent. An allowlist of browser containers is cleared on repaint so previous tab labels and backgrounds do not leave trails. Interactive child views are not blanket-cleared.
5. **Let Windows composite the backdrop.** Frame adjustments apply to real browser windows. Owned menus and popups are excluded from those frame adjustments. Translucent Windows provides the configured background effect.

Function addresses are cached for the exact `chrome.dll` build and rechecked before use. The named-color table is checked once per browser launch. There is no screenshot polling, webpage injection, or recurring full-module scan.

Function discovery is local: **no Brave debug-symbol download is used**. The obsolete optional symbol-server path was removed in 2.8.1.

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
    ALPHA. Requires the Translucent Windows mod enabled for brave.exe.
    Quick setup: select Dark appearance and the built-in Blue theme in Brave.
    If you see two sets of window buttons (minimise/maximise/close), turn
    on compact mode in brave://settings/appearance > Use compact mode.
    A window is modified only when its main toolbar colour is in this list.
    Other themes keep their original backgrounds. Within a matched window,
    any UI painted in one of these colours becomes transparent, so the Acrylic
    backdrop shows through it. Add a row for each colour you want to see
    through, as RRGGBB. Defaults are Brave's dark+blue theme: 333C4D is the
    vertical tab strip, 1D2636 is the top bar.
    To find a colour, screenshot Brave and sample the pixel you want gone, or
    set logColors=1 in the cfg and read them from the log.
    Each RGB value is matched exactly. Any UI painted in that colour becomes
    transparent, including text if its colour matches. Add colours one at a
    time and check the result.
*/
// ==/WindhawkModSettings==

#include <windhawk_api.h>

#if !defined(__x86_64__) && !defined(_M_X64)

BOOL Wh_ModInit() {
    Wh_Log(L"Brave Acrylic requires x64 Brave; native ARM64 is not supported.");
    return FALSE;
}

#else

#include <windows.h>
#include <commctrl.h>
#include <dwmapi.h>
#include <dxgi1_2.h>
#include <atomic>
#include <cstdio>
#include <cstring>

#ifndef DWMWA_SYSTEMBACKDROP_TYPE
#define DWMWA_SYSTEMBACKDROP_TYPE 38
#endif
#ifndef DWMSBT_TRANSIENTWINDOW
#define DWMSBT_TRANSIENTWINDOW 3
#endif

// ---------------------------------------------------------------------------
// Runtime configuration (C:\ProgramData\Windhawk\brave-acrylic.cfg)
// ---------------------------------------------------------------------------

struct Config {
    int clearViews          = 1;  // clear container view backgrounds
    int colorProvider       = 1;  // return transparent from ColorProvider::GetColor
    // Off by default: extending the frame makes DWM render the system frame
    // behind the client area, so its caption buttons show through underneath
    // Chromium's own and you see two sets. The backdrop does not need it.
    int dwmExtend           = 0;
    int gpuAlpha            = 1;  // force DComp swapchains to premultiplied alpha
    int backdropType        = DWMSBT_TRANSIENTWINDOW;
    int heartbeat           = 1;  // periodic re-assert of styles/backdrop
    int keepThickFrame      = 1;  // re-add WS_THICKFRAME if Chromium drops it
    // Hex RGB filter for diagnostic traceColor stacks. Actual replacement
    // colours come from Windhawk's colours list, not this debug setting.
    int bgColor             = 0x333C4D;
    // Logs distinct opaque colours GetColor returns, to identify a new theme's
    // toolbar colour. Noisy; for discovery only.
    int logColors           = 0;
    int traceColor          = 0;
    int traceLimit          = 25;   // how many stacks traceColor captures
} g_cfg;

static void ApplyConfigLine(const char* key, const char* valueText) {
    // bgColor is written as RRGGBB, so it is read as hex; everything else is
    // a plain decimal number.
    const bool hex = (lstrcmpiA(key, "bgColor") == 0);
    const int value = (int)strtol(valueText, nullptr, hex ? 16 : 10);

#define CFG_SET(name)                                  \
    if (lstrcmpiA(key, #name) == 0) { g_cfg.name = value; return; }
    CFG_SET(clearViews)
    CFG_SET(colorProvider)
    CFG_SET(dwmExtend)
    CFG_SET(gpuAlpha)
    CFG_SET(backdropType)
    CFG_SET(heartbeat)
    CFG_SET(keepThickFrame)
    CFG_SET(bgColor)
    CFG_SET(logColors)
    CFG_SET(traceColor)
    CFG_SET(traceLimit)
#undef CFG_SET
}

static void LoadConfig() {
    HANDLE f = CreateFileW(L"C:\\ProgramData\\Windhawk\\brave-acrylic.cfg",
                           GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE,
                           nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (f == INVALID_HANDLE_VALUE) {
        Wh_Log(L"no cfg file, using defaults");
        return;
    }
    char buf[4096] = {};
    DWORD read = 0;
    ReadFile(f, buf, sizeof(buf) - 1, &read, nullptr);
    CloseHandle(f);
    buf[read] = 0;

    char key[64], val[40];
    size_t ki = 0, vi = 0;
    bool inValue = false;
    for (size_t i = 0; i <= read; ++i) {
        char c = buf[i];
        if (c == '\r') continue;
        if (c == '\n' || c == 0) {
            if (ki && vi) {
                key[ki] = 0;
                val[vi] = 0;
                ApplyConfigLine(key, val);
            }
            ki = 0; vi = 0; inValue = false;
            continue;
        }
        if (c == '#' && !inValue && ki == 0) {
            while (i <= read && buf[i] != '\n') ++i;
            --i;
            continue;
        }
        if (!inValue) {
            if (c == '=') { inValue = true; continue; }
            if (c == ' ' || c == '\t') continue;
            if (ki < sizeof(key) - 1) key[ki++] = c;
        } else {
            if (c == ' ' || c == '	') continue;
            if (vi < sizeof(val) - 1) val[vi++] = c;
        }
    }
    Wh_Log(L"cfg: clearViews=%d colorProvider=%d dwmExtend=%d gpuAlpha=%d "
           L"backdropType=%d heartbeat=%d keepThickFrame=%d",
           g_cfg.clearViews, g_cfg.colorProvider, g_cfg.dwmExtend, g_cfg.gpuAlpha,
           g_cfg.backdropType, g_cfg.heartbeat, g_cfg.keepThickFrame);
    Wh_Log(L"cfg: bgColor=%06X logColors=%d (exact colour matching)",
           g_cfg.bgColor, g_cfg.logColors);
}

// ---------------------------------------------------------------------------
// Original function pointers
// ---------------------------------------------------------------------------

struct StringView { const char* data; size_t length; };

using GetColor_t = unsigned int (*)(const void* pThis, int id);
static GetColor_t pOrigGetColor = nullptr;

using DrawColor_t = void (*)(void* canvas, unsigned int color, int mode);
static DrawColor_t pCanvasDrawColor = nullptr;

using ViewGetClassNameW_t = StringView* (*)(const void* pView, StringView* ret);
static ViewGetClassNameW_t pViewGetClassNameW = nullptr;

using ViewGetColorProvider_t = const void* (*)(const void* view);
static ViewGetColorProvider_t pViewGetColorProvider = nullptr;

using ViewOnPaintBackground_t = void (*)(void* pThis, void* canvas);
static ViewOnPaintBackground_t pOrigViewOnPaintBackground = nullptr;

using DwmExtendFrameIntoClientArea_t = HRESULT (WINAPI*)(HWND, const MARGINS*);
static DwmExtendFrameIntoClientArea_t pOrigDwmExtendFrameIntoClientArea = nullptr;

using CreateSwapChainForComposition_t = HRESULT (WINAPI*)(
    IDXGIFactory2*, IUnknown*, const DXGI_SWAP_CHAIN_DESC1*, IDXGIOutput*, IDXGISwapChain1**);
static CreateSwapChainForComposition_t pOrigCreateSwapChainForComposition = nullptr;

using CreateSwapChainForHwnd_t = HRESULT (WINAPI*)(
    IDXGIFactory2*, IUnknown*, HWND, const DXGI_SWAP_CHAIN_DESC1*,
    const DXGI_SWAP_CHAIN_FULLSCREEN_DESC*, IDXGIOutput*, IDXGISwapChain1**);
static CreateSwapChainForHwnd_t pOrigCreateSwapChainForHwnd = nullptr;

static std::atomic<int> s_logViewBg{0};
static std::atomic<int> s_logSwap{0};
static std::atomic<bool> s_stopThread{false};
static HANDLE s_hThread = nullptr;
static HANDLE s_hInitThread = nullptr;
static std::atomic<bool> s_transparencyReady{false};
static std::atomic<bool> s_modInitComplete{false};

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

// Colours to replace with transparency, taken from the mod's Windhawk settings.
// An empty settings list retains these defaults.
enum { kMaxTargetColors = 16 };
static int g_targetColors[kMaxTargetColors] = {0x333C4D, 0x1D2636};
static int g_targetColorCount = 2;

static int ParseHexColor(PCWSTR s) {
    if (!s) return -1;
    while (*s == L'#' || *s == L' ' || *s == L'\t') ++s;
    int v = 0, digits = 0;
    for (; *s; ++s) {
        int d;
        if (*s >= L'0' && *s <= L'9') d = *s - L'0';
        else if (*s >= L'a' && *s <= L'f') d = *s - L'a' + 10;
        else if (*s >= L'A' && *s <= L'F') d = *s - L'A' + 10;
        else break;
        v = v * 16 + d;
        ++digits;
    }
    return digits >= 6 ? (v & 0xFFFFFF) : -1;
}

static void LoadModSettings() {
    // Read colors[0..n] until a blank. If the user has not set any (or
    // Windhawk has never written the settings), the built-in defaults stand.
    int found = 0;
    for (int i = 0; i < kMaxTargetColors; ++i) {
        PCWSTR v = Wh_GetStringSetting(L"colors[%d]", i);
        const int c = ParseHexColor(v);
        Wh_FreeStringSetting(v);
        if (c < 0) break;
        g_targetColors[found++] = c;
    }
    if (found) g_targetColorCount = found;

    {
        char list[256];
        int w = 0;
        for (int i = 0; i < g_targetColorCount && w < (int)sizeof(list) - 10; ++i) {
            w += sprintf(list + w, "%06X ", g_targetColors[i]);
        }
        list[w] = 0;
        Wh_Log(L"settings: %d colour(s) [ %S] (exact matches)", g_targetColorCount, list);
    }
}

// The palette identifies the window theme; individual replacements still use
// the explicitly configured RGB values.
static inline bool IsThemeBackground(unsigned int color) {
    for (int i = 0; i < g_targetColorCount; ++i) {
        if (color == static_cast<unsigned int>(g_targetColors[i])) return true;
    }
    return false;
}

// Use this window's original palette, never a colour already replaced by our hook.
// Query the native accessor each time: palettes change when switching themes, and
// caching View/ColorProvider pointers would outlive their native objects.
static int s_toolbarColorId = -1;

static bool HasCompleteTransparencyPath() {
    return pOrigGetColor && pCanvasDrawColor && pViewGetClassNameW &&
           pViewGetColorProvider && pOrigViewOnPaintBackground && s_toolbarColorId >= 0;
}

static bool ThemeMatches(const void* provider) {
    if (!s_transparencyReady.load(std::memory_order_acquire) || !provider ||
        !pOrigGetColor || s_toolbarColorId < 0) return false;
    const unsigned int toolbar = pOrigGetColor(provider, s_toolbarColorId);
    // Chromium returns gfx::kPlaceholderColor (red) for a missing colour ID.
    return toolbar != 0xFFFF0000 && (toolbar >> 24) == 0xFF &&
           IsThemeBackground(toolbar & 0xFFFFFF);
}

static bool CanClearForTheme(const void* view) {
    return view && pViewGetColorProvider &&
           s_transparencyReady.load(std::memory_order_acquire) &&
           ThemeMatches(pViewGetColorProvider(view));
}

static bool ShouldClearView(const void* view) {
    if (!view || !pViewGetClassNameW) return false;
    StringView name{};
    pViewGetClassNameW(view, &name);
    if (!name.data || !name.length || name.length >= 128) return false;

    // An explicit container allowlist avoids walking Chromium's private View
    // layout. In particular, never read a presumed parent_ member offset.
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

static void ClearCanvas(void* canvas) {
    if (pCanvasDrawColor && canvas) {
        pCanvasDrawColor(canvas, 0x00000000, 0 /* SkBlendMode::kClear */);
    }
}

// ---------------------------------------------------------------------------
// Hooks
// ---------------------------------------------------------------------------

// chrome.dll bounds, so only interesting frames are logged.
static const unsigned char* g_chromeBase = nullptr;
static size_t g_chromeSize = 0;

using CaptureStackBackTrace_t = USHORT(WINAPI*)(ULONG, ULONG, PVOID*, PULONG);
static CaptureStackBackTrace_t pCaptureStack = nullptr;

static void TraceColorStack(int id, unsigned int rgb) {
    if (!pCaptureStack) {
        HMODULE nt = GetModuleHandleW(L"ntdll.dll");
        if (nt) {
            pCaptureStack =
                (CaptureStackBackTrace_t)GetProcAddress(nt, "RtlCaptureStackBackTrace");
        }
        if (!pCaptureStack) return;
    }
    void* frames[24] = {};
    USHORT n = pCaptureStack(1, 24, frames, nullptr);

    char line[900];
    int w = 0;
    for (USHORT i = 0; i < n && w < (int)sizeof(line) - 24; ++i) {
        const unsigned char* f = (const unsigned char*)frames[i];
        if (g_chromeBase && f >= g_chromeBase && f < g_chromeBase + g_chromeSize) {
            w += sprintf(line + w, "%llx ", (unsigned long long)(f - g_chromeBase));
        }
    }
    line[w] = 0;
    Wh_Log(L"TRACE id=%d #%06X <- chrome.dll+ %S", id, rgb, line);
}

static std::atomic<int> s_traceCount{0};


static unsigned int GetColor_Hook(const void* pThis, int id) {
    unsigned int c = pOrigGetColor ? pOrigGetColor(pThis, id) : 0;
    const unsigned int rgb = c & 0x00FFFFFF;
    const bool opaque = (c >> 24) == 0xFF;

    // Diagnostic mode logs distinct ID/colour pairs, not repeated calls to
    // the same colour. Activation and repaint colours can then be compared.
    if (g_cfg.logColors && opaque) {
        struct Entry { int id; unsigned int color; };
        static SRWLOCK lock = SRWLOCK_INIT;
        static Entry seen[256]{};
        static unsigned count = 0;
        bool fresh = false;
        AcquireSRWLockExclusive(&lock);
        unsigned i = 0;
        for (; i < count; ++i) {
            if (seen[i].id == id && seen[i].color == c) break;
        }
        if (i == count && count < ARRAYSIZE(seen)) {
            seen[count++] = {id, c};
            fresh = true;
        }
        ReleaseSRWLockExclusive(&lock);
        if (fresh) Wh_Log(L"colour id %d -> #%06X match=%d", id, rgb,
                          static_cast<int>(IsThemeBackground(rgb)));
    }


    if (g_cfg.traceColor && opaque && rgb == static_cast<unsigned int>(g_cfg.bgColor) &&
        s_traceCount.fetch_add(1) < g_cfg.traceLimit) {
        TraceColorStack(id, rgb);
    }

    // Replace only the explicitly configured opaque theme colours.
    if (s_transparencyReady.load(std::memory_order_relaxed) &&
        g_cfg.colorProvider && opaque && IsThemeBackground(rgb) &&
        ThemeMatches(pThis)) return 0x00000000;
    return c;
}


static void ViewOnPaintBackground_Hook(void* pThis, void* canvas) {
    if (g_cfg.clearViews && ShouldClearView(pThis)) {
        if (s_logViewBg.fetch_add(1) < 24) {
            StringView sv = {};
            if (pViewGetClassNameW) pViewGetClassNameW(pThis, &sv);
            Wh_Log(L"clearing view background: %.*S", (int)sv.length,
                   sv.data ? sv.data : "unknown");
        }
        ClearCanvas(canvas);
        return;
    }
    if (pOrigViewOnPaintBackground) pOrigViewOnPaintBackground(pThis, canvas);
}

// Only real browser frames may be restyled. Menus, bubbles and other flyouts
// are also Chrome_WidgetWin_1, but they are owned popups without min/max boxes;
// giving those a WS_THICKFRAME draws a visible resize border around them.
static bool IsBrowserFrameWindow(HWND hWnd) {
    if (!hWnd || !IsWindow(hWnd)) return false;
    wchar_t cls[64] = {};
    if (!GetClassNameW(hWnd, cls, ARRAYSIZE(cls))) return false;
    if (wcscmp(cls, L"Chrome_WidgetWin_1") != 0) return false;
    if (GetWindow(hWnd, GW_OWNER) != nullptr) return false;
    LONG_PTR style = GetWindowLongPtrW(hWnd, GWL_STYLE);
    if (style & (WS_CHILD | WS_POPUP)) return false;
    if (!(style & WS_MAXIMIZEBOX)) return false;
    return true;
}

// Remember which frames are configured. Re-issuing the DWM calls on every tick
// resets the extended frame constantly and the backdrop stops showing, so they
// must be applied once per window.
static HWND s_configured[32] = {};
static std::atomic<int> s_configuredCount{0};

static bool AlreadyConfigured(HWND hWnd) {
    int n = s_configuredCount.load();
    for (int i = 0; i < n && i < 32; ++i) {
        if (s_configured[i] == hWnd) return true;
    }
    return false;
}

static void MarkConfigured(HWND hWnd) {
    int n = s_configuredCount.load();
    if (n < 32) {
        s_configured[n] = hWnd;
        s_configuredCount.store(n + 1);
    }
}

static void ApplyWindowAttributes(HWND hWnd) {
    if (!IsBrowserFrameWindow(hWnd)) return;

    // Cheap safety net, re-checked on every heartbeat tick: if anything ever
    // strips the resize border, put it back.
    if (g_cfg.keepThickFrame) {
        LONG_PTR style = GetWindowLongPtrW(hWnd, GWL_STYLE);
        if (!(style & WS_THICKFRAME)) {
            SetWindowLongPtrW(hWnd, GWL_STYLE, style | WS_THICKFRAME);
            SetWindowPos(hWnd, nullptr, 0, 0, 0, 0,
                         SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED | SWP_NOACTIVATE);
        }
    }

    if (AlreadyConfigured(hWnd)) return;
    MarkConfigured(hWnd);

    if (g_cfg.backdropType >= 0) {
        int backdrop = g_cfg.backdropType;
        DwmSetWindowAttribute(hWnd, DWMWA_SYSTEMBACKDROP_TYPE, &backdrop, sizeof(backdrop));
    }
    if (g_cfg.dwmExtend) {
        MARGINS margins = {-1, -1, -1, -1};
        if (pOrigDwmExtendFrameIntoClientArea) {
            pOrigDwmExtendFrameIntoClientArea(hWnd, &margins);
        } else {
            DwmExtendFrameIntoClientArea(hWnd, &margins);
        }
    }
}

static BOOL CALLBACK EnumBraveWindowsProc(HWND hWnd, LPARAM);

static HRESULT WINAPI DwmExtendFrameIntoClientArea_Hook(HWND hWnd, const MARGINS* pMarInset) {
    // Never override the margins Chromium picks for menus, bubbles and other
    // flyouts.
    if (g_cfg.dwmExtend && IsBrowserFrameWindow(hWnd)) {
        MARGINS forced = {-1, -1, -1, -1};
        return pOrigDwmExtendFrameIntoClientArea(hWnd, &forced);
    }
    return pOrigDwmExtendFrameIntoClientArea(hWnd, pMarInset);
}

static BOOL CALLBACK EnumBraveWindowsProc(HWND hWnd, LPARAM) {
    DWORD pid = 0;
    GetWindowThreadProcessId(hWnd, &pid);
    if (pid != GetCurrentProcessId()) return TRUE;
    if (IsWindowVisible(hWnd)) ApplyWindowAttributes(hWnd);
    return TRUE;
}

static DWORD WINAPI HeartbeatThreadProc(LPVOID) {
    while (!s_stopThread.load()) {
        Sleep(1000);
        if (g_cfg.heartbeat) EnumWindows(EnumBraveWindowsProc, 0);
    }
    return 0;
}

// ---------------------------------------------------------------------------
// GPU process hooks
// ---------------------------------------------------------------------------

static HRESULT WINAPI CreateSwapChainForComposition_Hook(
    IDXGIFactory2* pFactory, IUnknown* pDevice, const DXGI_SWAP_CHAIN_DESC1* pDesc,
    IDXGIOutput* pRestrictToOutput, IDXGISwapChain1** ppSwapChain) {
    if (pDesc && g_cfg.gpuAlpha) {
        DXGI_SWAP_CHAIN_DESC1 desc = *pDesc;
        if (s_logSwap.fetch_add(1) < 12) {
            Wh_Log(L"CreateSwapChainForComposition %ux%u fmt=%d alpha=%d -> PREMULTIPLIED",
                   desc.Width, desc.Height, (int)desc.Format, (int)desc.AlphaMode);
        }
        desc.AlphaMode = DXGI_ALPHA_MODE_PREMULTIPLIED;
        return pOrigCreateSwapChainForComposition(pFactory, pDevice, &desc, pRestrictToOutput, ppSwapChain);
    }
    return pOrigCreateSwapChainForComposition(pFactory, pDevice, pDesc, pRestrictToOutput, ppSwapChain);
}

static HRESULT WINAPI CreateSwapChainForHwnd_Hook(
    IDXGIFactory2* pFactory, IUnknown* pDevice, HWND hWnd, const DXGI_SWAP_CHAIN_DESC1* pDesc,
    const DXGI_SWAP_CHAIN_FULLSCREEN_DESC* pFullscreenDesc, IDXGIOutput* pRestrictToOutput,
    IDXGISwapChain1** ppSwapChain) {
    if (pDesc && g_cfg.gpuAlpha) {
        DXGI_SWAP_CHAIN_DESC1 desc = *pDesc;
        desc.AlphaMode = DXGI_ALPHA_MODE_PREMULTIPLIED;
        return pOrigCreateSwapChainForHwnd(pFactory, pDevice, hWnd, &desc, pFullscreenDesc,
                                           pRestrictToOutput, ppSwapChain);
    }
    return pOrigCreateSwapChainForHwnd(pFactory, pDevice, hWnd, pDesc, pFullscreenDesc,
                                       pRestrictToOutput, ppSwapChain);
}

static bool InstallGpuHooks() {
    HMODULE hDxgi = LoadLibraryW(L"dxgi.dll");
    if (!hDxgi) return false;
    using CreateDXGIFactory1_t = HRESULT (WINAPI*)(REFIID, void**);
    auto pCreateDXGIFactory1 = (CreateDXGIFactory1_t)GetProcAddress(hDxgi, "CreateDXGIFactory1");
    if (!pCreateDXGIFactory1) return false;

    const IID iidFactory2 = {0x50c83a1c, 0xe072, 0x4c48, {0x87, 0xb0, 0x36, 0x30, 0xfa, 0x36, 0xa6, 0xd0}};
    IDXGIFactory2* pFactory = nullptr;
    if (FAILED(pCreateDXGIFactory1(iidFactory2, (void**)&pFactory)) || !pFactory) return false;

    void** vtbl = *(void***)pFactory;
    Wh_SetFunctionHook(vtbl[15], (void*)CreateSwapChainForHwnd_Hook, (void**)&pOrigCreateSwapChainForHwnd);
    Wh_SetFunctionHook(vtbl[24], (void*)CreateSwapChainForComposition_Hook, (void**)&pOrigCreateSwapChainForComposition);
    if (s_modInitComplete.load(std::memory_order_acquire)) Wh_ApplyHookOperations();
    pFactory->Release();
    Wh_Log(L"installed DXGI hooks in GPU process");
    return true;
}

// ---------------------------------------------------------------------------
// Browser process hooks
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Signature scanning
//
// Resolve only complete function entries described by the x64 exception table.
// Signatures inside a function cannot be mistaken for an entry point, and
// function alignment does not affect discovery. Every signature must have one unique function match.
struct Signature {
    const char* name;
    const unsigned char* bytes;
    const char* mask;
    size_t length;
};

static const unsigned char kSigGetColor[] = {
    0x41, 0x57, 0x41, 0x56, 0x41, 0x54, 0x56, 0x57, 0x55, 0x53, 0x48, 0x83,
    0xEC, 0x20, 0x89, 0xD7, 0x48, 0x89, 0xCE, 0x4C, 0x8D, 0x71, 0x18, 0x48};
static const char kMaskGetColor[] = "xxxxxxxxxxxxxxxxxxxxxxxx";

static const unsigned char kSigOnPaintBackground[] = {
    0x56, 0x57, 0x48, 0x83, 0xEC, 0x38, 0x48, 0x89, 0xCE, 0x48, 0x8B, 0x05,
    0x30, 0x60, 0x82, 0x12, 0x48, 0x31, 0xE0, 0x48, 0x89, 0x44, 0x24, 0x30,
    0x48, 0x8B, 0x89, 0x28, 0x02, 0x00, 0x00, 0x48};
static const char kMaskOnPaintBackground[] = "xxxxxxxxxxxx????xxxxxxxxxxxxxxxx";

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

static const unsigned char kSigGetClassNameW[] = {
    0x56, 0x48, 0x83, 0xEC, 0x20, 0x48, 0x89, 0xD6, 0x48, 0x8B, 0x81, 0x00,
    0x01, 0x00, 0x00, 0x48, 0x81, 0xC1, 0x00, 0x01, 0x00, 0x00, 0x48, 0x8B};
static const char kMaskGetClassNameW[] = "xxxxxxxxxxxxxxxxxxxxxxxx";

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
    const unsigned char* last;
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
            out[s].last = at;
        }
    }
    return true;
}


// chrome.dll identity, so a cached result is never reused across Brave builds.
static unsigned long long BuildKey(HMODULE mod) {
    auto dos = (const IMAGE_DOS_HEADER*)mod;
    auto nt = (const IMAGE_NT_HEADERS64*)((const unsigned char*)mod + dos->e_lfanew);
    const unsigned long long kSignatureSetRevision = 11;
    return (((unsigned long long)nt->FileHeader.TimeDateStamp << 32) ^
            (unsigned long long)nt->OptionalHeader.SizeOfImage) +
           kSignatureSetRevision;
}

// Kept in %LOCALAPPDATA%: the browser process runs at medium integrity and is
// denied file creation under C:\ProgramData\Windhawk (ACCESS_DENIED), which is
// why an earlier attempt to cache there silently rescanned on every launch.
// Brave's version is the folder holding chrome.dll:
// ...\Application\<version>\chrome.dll. Only used to make the log and the cache
// readable -- correctness rests on the build key and the byte re-check.
static void GetBraveVersion(HMODULE mod, char* out, size_t cch) {
    out[0] = 0;
    wchar_t path[MAX_PATH] = {};
    if (!GetModuleFileNameW(mod, path, MAX_PATH)) return;
    wchar_t* slash = wcsrchr(path, L'\\');
    if (!slash) return;
    *slash = 0;
    wchar_t* ver = wcsrchr(path, L'\\');
    if (!ver) return;
    WideCharToMultiByte(CP_UTF8, 0, ver + 1, -1, out, (int)cch, nullptr, nullptr);
}

static const wchar_t* CachePath() {
    static wchar_t path[MAX_PATH] = {};
    static bool tried = false;
    if (!tried) {
        tried = true;
        DWORD n = GetEnvironmentVariableW(L"LOCALAPPDATA", path, MAX_PATH);
        if (n == 0 || n >= MAX_PATH - 32) {
            path[0] = 0;
        } else {
            lstrcatW(path, L"\\brave-acrylic.cache");
        }
    }
    return path[0] ? path : nullptr;
}

static bool LoadCache(unsigned long long wantKey, size_t* rva, int count,
                      char* prevVersion, size_t cchPrev) {
    const wchar_t* path = CachePath();
    if (!path) return false;
    HANDLE f = CreateFileW(path, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE,
                           nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (f == INVALID_HANDLE_VALUE) return false;
    char buf[512] = {};
    DWORD read = 0;
    ReadFile(f, buf, sizeof(buf) - 1, &read, nullptr);
    CloseHandle(f);
    buf[read] = 0;

    // Layout: key, Brave version, then one RVA per line.
    unsigned long long key = 0;
    if (sscanf(buf, "%llx", &key) != 1) return false;

    const char* p = strchr(buf, '\n');
    if (!p) return false;
    ++p;
    if (prevVersion && cchPrev) {
        size_t n = 0;
        while (p[n] && p[n] != '\n' && n < cchPrev - 1) ++n;
        memcpy(prevVersion, p, n);
        prevVersion[n] = 0;
    }
    if (key != wantKey) return false;  // version reported, but cache unusable

    for (int i = 0; i < count; ++i) {
        p = strchr(p, '\n');
        if (!p) return false;
        ++p;
        unsigned long long v = 0;
        if (sscanf(p, "%llx", &v) != 1) return false;
        rva[i] = (size_t)v;
    }
    return true;
}

static void SaveCache(unsigned long long key, const char* version,
                      const size_t* rva, int count) {
    char buf[512];
    int n = sprintf(buf, "%llx\n%s\n", key, (version && version[0]) ? version : "?");
    for (int i = 0; i < count; ++i) n += sprintf(buf + n, "%llx\n", (unsigned long long)rva[i]);
    // FILE_WRITE_DATA only: GENERIC_WRITE additionally asks for WRITE_ATTRIBUTES,
    // which this directory's ACL denies to the (medium integrity) browser process.
    const wchar_t* path = CachePath();
    if (!path) return;
    HANDLE f = CreateFileW(path, FILE_WRITE_DATA, FILE_SHARE_READ | FILE_SHARE_WRITE,
                           nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (f == INVALID_HANDLE_VALUE) {
        Wh_Log(L"cache write failed: CreateFileW error %u", GetLastError());
        return;
    }
    DWORD written = 0;
    BOOL ok = WriteFile(f, buf, (DWORD)n, &written, nullptr);
    CloseHandle(f);
    Wh_Log(L"cache written: %d bytes formatted, %u written, ok=%d", n, written, (int)ok);
}

static bool ResolveBySignature(HMODULE hChrome) {
    const unsigned char* text = nullptr;
    size_t textSize = 0;
    if (!GetTextSection(hChrome, &text, &textSize)) {
        Wh_Log(L"could not locate .text in chrome.dll");
        return false;
    }

    enum { kNeeded = 5 };

    const Signature sigs[kNeeded] = {
        {"ui::ColorProvider::GetColor", kSigGetColor, kMaskGetColor,
         sizeof(kSigGetColor)},
        {"views::View::OnPaintBackground", kSigOnPaintBackground, kMaskOnPaintBackground,
         sizeof(kSigOnPaintBackground)},
        {"gfx::Canvas::DrawColor", kSigDrawColor, kMaskDrawColor,
         sizeof(kSigDrawColor)},
        {"views::View::GetClassNameW", kSigGetClassNameW, kMaskGetClassNameW,
         sizeof(kSigGetClassNameW)},
        {"views::View::GetColorProvider", kSigViewColorProvider, kMaskViewColorProvider,
         sizeof(kSigViewColorProvider)},
    };

    void** targets[kNeeded] = {
        (void**)&pOrigGetColor,    (void**)&pOrigViewOnPaintBackground,
        (void**)&pCanvasDrawColor, (void**)&pViewGetClassNameW,
        (void**)&pViewGetColorProvider,
    };
    // Drawing/class/palette accessors are only called, never hooked.
    void* hookFns[kNeeded] = {
        (void*)GetColor_Hook, (void*)ViewOnPaintBackground_Hook,
        nullptr,              nullptr,              nullptr,
    };

    const DWORD started = GetTickCount();
    const unsigned long long key = BuildKey(hChrome);
    char version[64] = {};
    char prevVersion[64] = {};
    GetBraveVersion(hChrome, version, sizeof(version));
    size_t rva[kNeeded] = {};
    bool fromCache = false;

    // A cached hit still has its bytes re-checked, so a stale or hand-edited
    // cache can never point a hook at the wrong code.
    if (LoadCache(key, rva, kNeeded, prevVersion, sizeof(prevVersion))) {
        fromCache = true;
        for (int i = 0; i < kNeeded; ++i) {
            if (rva[i] == 0) { fromCache = false; break; }
            const size_t textRva = text - reinterpret_cast<const unsigned char*>(hChrome);
            if (rva[i] < textRva || rva[i] - textRva > textSize ||
                sigs[i].length > textSize - (rva[i] - textRva) ||
                !FunctionContains(hChrome, rva[i], sigs[i].length) ||
                !MatchesAt(reinterpret_cast<const unsigned char*>(hChrome) + rva[i], sigs[i])) {
                Wh_Log(L"cache entry for %S failed verification; rescanning", sigs[i].name);
                fromCache = false;
                break;
            }

        }
    }

    if (!fromCache) {
        if (prevVersion[0]) {
            Wh_Log(L"Brave changed (%S -> %S); rescanning", prevVersion,
                   version[0] ? version : "?");
        } else {
            Wh_Log(L"no usable cache for Brave %S; scanning", version[0] ? version : "?");
        }

        ScanHit hits[kNeeded]{};
        if (!ScanAll(hChrome, text, textSize, sigs, kNeeded, hits)) {
            Wh_Log(L"missing or invalid x64 function table; refusing signature hooks");
            return false;
        }
        for (int i = 0; i < kNeeded; ++i) {
            rva[i] = hits[i].count == 1 ? hits[i].first -
                reinterpret_cast<const unsigned char*>(hChrome) : 0;
            if (!rva[i]) Wh_Log(L"%S: %d function matches (need exactly one)",
                               sigs[i].name, hits[i].count);
        }
    }

    // Never activate a partial clearing path. Matching a colour without being
    // able to erase prior pixels can leave stale tab text on the screen.
    for (int i = 0; i < kNeeded; ++i) {
        if (!rva[i]) {
            Wh_Log(L"required function missing: %S; transparency stays off", sigs[i].name);
            return false;
        }
    }


    int resolved = 0;
    for (int i = 0; i < kNeeded; ++i) {
        if (!rva[i]) {
            Wh_Log(L"signature did NOT match: %S", sigs[i].name);
            continue;
        }
        void* addr = (void*)((const unsigned char*)hChrome + rva[i]);
        if (hookFns[i]) {
            if (!Wh_SetFunctionHook(addr, hookFns[i], targets[i])) {
                *targets[i] = nullptr;
                Wh_Log(L"could not install required hook: %S", sigs[i].name);
                continue;
            }
        } else {
            *targets[i] = addr;
        }
        ++resolved;
    }
    if (s_modInitComplete.load(std::memory_order_acquire)) Wh_ApplyHookOperations();
    s_transparencyReady.store(g_cfg.clearViews && g_cfg.colorProvider &&
                              HasCompleteTransparencyPath(), std::memory_order_release);

    // Making backgrounds transparent without being able to erase the previous
    // frame leaves the tab strip ghosting, so transparency is only switched on
    // when the clearing path is fully available.
    if (!pCanvasDrawColor || !pViewGetClassNameW || !pViewGetColorProvider ||
        s_toolbarColorId < 0 || !pOrigViewOnPaintBackground) {
        if (g_cfg.colorProvider) {
            g_cfg.colorProvider = 0;
            Wh_Log(L"clearing path incomplete - disabling the colour substitution "
                   L"so the tab strip cannot ghost. Brave likely changed; "
                   L"the resolver must be updated for this Brave build.");
        }
    }

    const bool criticalOk = HasCompleteTransparencyPath();
    if (criticalOk && !fromCache) SaveCache(key, version, rva, kNeeded);

    Wh_Log(L"%s: %d/%d resolved in %u ms (Brave %S, key %llx)",
           fromCache ? L"signature cache" : L"signature scan", resolved, (int)kNeeded,
           GetTickCount() - started, version[0] ? version : "?", key);
    return resolved == kNeeded;
}

static void StartWindowWork() {
    if (s_stopThread.load()) return;
    EnumWindows(EnumBraveWindowsProc, 0);
    s_hThread = CreateThread(nullptr, 0, HeartbeatThreadProc, nullptr, 0, nullptr);
}

static bool InstallChromeHooks(HMODULE hChrome) {
    if (!hChrome) return false;
    Wh_Log(L"hooking chrome.dll at %p", hChrome);
    {
        auto dos = (const IMAGE_DOS_HEADER*)hChrome;
        auto nt = (const IMAGE_NT_HEADERS64*)((const unsigned char*)hChrome + dos->e_lfanew);
        g_chromeBase = (const unsigned char*)hChrome;
        g_chromeSize = nt->OptionalHeader.SizeOfImage;
    }

    s_toolbarColorId = FindToolbarColorId(hChrome);
    if (s_toolbarColorId < 0) {
        Wh_Log(L"toolbar colour table missing or ambiguous; transparency stays off");
        return false;
    }
    Wh_Log(L"per-window theme guard: kColorToolbar ID=%d", s_toolbarColorId);

    HMODULE hDwmapi = LoadLibraryW(L"dwmapi.dll");
    if (hDwmapi) {
        void* p = (void*)GetProcAddress(hDwmapi, "DwmExtendFrameIntoClientArea");
        if (p) {
            Wh_SetFunctionHook(p, (void*)DwmExtendFrameIntoClientArea_Hook,
                               (void**)&pOrigDwmExtendFrameIntoClientArea);
            if (s_modInitComplete.load(std::memory_order_acquire)) Wh_ApplyHookOperations();
        }
    }

    // Use only the validated local resolver; no symbol-server fallback.
    if (!ResolveBySignature(hChrome)) return false;
    StartWindowWork();
    return true;
}

// Waits for chrome.dll to be mapped, then hooks it. Polls tightly because the
// window that matters is created only a few hundred ms after the module loads.
static DWORD WINAPI ChromeInitThreadProc(LPVOID) {
    HMODULE hChrome = nullptr;
    for (int i = 0; i < 2000 && !s_stopThread.load(); ++i) {
        hChrome = GetModuleHandleW(L"chrome.dll");
        if (hChrome) break;
        Sleep(1);
    }
    if (s_stopThread.load()) return 0;
    if (!hChrome) {
        Wh_Log(L"chrome.dll never loaded");
        return 0;
    }
    // ApplyHookOperations is only supported after ModInit has returned.
    while (!s_modInitComplete.load(std::memory_order_acquire) && !s_stopThread.load()) Sleep(1);
    if (!s_stopThread.load()) InstallChromeHooks(hChrome);
    return 0;
}

// ---------------------------------------------------------------------------
// Lifecycle
// ---------------------------------------------------------------------------

BOOL Wh_ModInit() {
    LoadConfig();
    LoadModSettings();
    LPCWSTR cmdLine = GetCommandLineW();

    if (wcsstr(cmdLine, L"--type=gpu-process") != nullptr) {
        Wh_Log(L"init in GPU process %u", GetCurrentProcessId());
        InstallGpuHooks();
        return TRUE;
    }
    if (wcsstr(cmdLine, L"--type=") != nullptr) {
        return TRUE;
    }

    Wh_Log(L"init in browser process %u", GetCurrentProcessId());
    // chrome.dll is usually not mapped yet this early. Resolve inline when it
    // is, otherwise pick it up as soon as it appears -- the hooks must be live
    // before Brave builds its first browser window, because view layers get
    // their opacity at creation time and are not revisited afterwards.
    HMODULE hChrome = GetModuleHandleW(L"chrome.dll");
    if (hChrome) {
        InstallChromeHooks(hChrome);
    } else {
        s_hInitThread = CreateThread(nullptr, 0, ChromeInitThreadProc, nullptr, 0, nullptr);
    }
    return TRUE;
}

void Wh_ModAfterInit() {
    s_modInitComplete.store(true, std::memory_order_release);
}

// Views resolve their background colour when they are built, so re-reading the
// settings does not repaint anything. Posting WM_THEMECHANGED was tried and is
// NOT sufficient -- a colour added this way only takes effect after Brave is
// restarted. The post is kept because it is harmless and refreshes some
// surfaces, but the log tells the truth about needing a restart.
static BOOL CALLBACK RefreshThemeProc(HWND hWnd, LPARAM) {
    DWORD pid = 0;
    GetWindowThreadProcessId(hWnd, &pid);
    if (pid == GetCurrentProcessId() && IsBrowserFrameWindow(hWnd)) {
        PostMessageW(hWnd, WM_THEMECHANGED, 0, 0);
    }
    return TRUE;
}

void Wh_ModSettingsChanged() {
    LoadModSettings();
    EnumWindows(RefreshThemeProc, 0);
    Wh_Log(L"settings reloaded - RESTART BRAVE for colour changes to apply");
}

void Wh_ModUninit() {
    s_stopThread.store(true);
    s_transparencyReady.store(false);
    if (s_hInitThread) {
        WaitForSingleObject(s_hInitThread, INFINITE);
        CloseHandle(s_hInitThread);
        s_hInitThread = nullptr;
    }
    if (s_hThread) {
        WaitForSingleObject(s_hThread, INFINITE);
        CloseHandle(s_hThread);
        s_hThread = nullptr;
    }
    Wh_Log(L"uninit in %u", GetCurrentProcessId());
}

#endif  // x64 native resolver
