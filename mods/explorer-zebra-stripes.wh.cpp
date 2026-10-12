// ==WindhawkMod==
// @id              explorer-zebra-stripes
// @name            Explorer List Zebra Stripes
// @description     Add stripes for Details and List items in File Explorer
// @version         0.13.0
// @author          DarkionAvey
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -luser32 -lgdi32 -ladvapi32 -ldwmapi
// @github          https://github.com/DarkionAvey
// @license         WTFPL
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Explorer List Zebra Stripes

![Screenshot](https://raw.githubusercontent.com/DarkionAvey/windhawk-explorer-zebra-stripes/refs/heads/main/screenshot.png)

Adds alternating stripes to compact file-list items in Explorer, including
Details and List views.

## Colors

Supports #RRGGBB and dynamic system color names: `accent`, `background`,
`foreground`, `highlight`, `highlighttext`, `window`, `windowtext`, `buttonface`,
`buttontext`, `hotlight`, `graytext`.

`accent` uses the Windows DWM colorization color. `background` and
`foreground` are theme-aware neutral colors (high-contrast aware); other
system names use Win32 system colors.

Opacity is a percentage: 0 = invisible, 100 = fully opaque. 


Mod was tested on Windows 11 24H2 & 26H2. 
*/

// ==/WindhawkModReadme==

// ==WindhawkModSettings==

/*
- useSameThemeSettings: false
  $name: Use light color and opacity for both themes
  $description: When enabled, lightColor and lightAlphaPercent apply in both light and dark mode. Dark color/opacity are ignored.
- lightColor: "foreground"
  $name: Light / shared stripe color
  $description: 'Copy a name: accent, background, foreground, highlight, highlighttext, window, windowtext, buttonface, buttontext, hotlight, graytext. Or enter RRGGBB / #RRGGBB. background/foreground follow theme.'
- lightAlphaPercent: 5
  $name: Light / shared opacity (%)
  $description: '0 = invisible, 50 = 50% opacity, 100 = fully opaque. Blends with the existing row background.'
- darkColor: "foreground"
  $name: Dark-theme stripe color
  $description: 'Copy a name: accent, background, foreground, highlight, highlighttext, window, windowtext, buttonface, buttontext, hotlight, graytext. Or enter RRGGBB / #RRGGBB. Ignored when useSameThemeSettings is enabled.'
- darkAlphaPercent: 2
  $name: Dark-theme opacity (%)
  $description: '0 = invisible, 50 = 50% opacity, 100 = fully opaque. Ignored when useSameThemeSettings is enabled.'
- cornerRadius: 8
  $name: Corner radius (at 96 DPI)
- horizontalInset: 0
  $name: Horizontal inset (at 96 DPI)
- verticalInset: 0
  $name: Vertical inset (at 96 DPI)
- minimumRowWidth: 0
  $name: Minimum item width (0 = unrestricted)
  $description: Optional width filter for compact list items. Leave at 0 to include narrow items and the normal List view.
- stripeParity: odd
  $name: Striped items
  $options:
    - odd: Odd items
    - even: Even items
- diagnostics: false
  $name: Log bounded resize/release details during testing
  $description: Normally off to avoid per-row logging or diagnostic counters.

*/
// ==/WindhawkModSettings==
#include <windows.h>
#include <oleacc.h>
#include <dwmapi.h>
#include <atomic>
#include <algorithm>
#include <cwchar>
#include <cstdint>
#include <windhawk_utils.h>

#ifndef REG_NOTIFY_THREAD_AGNOSTIC
#define REG_NOTIFY_THREAD_AGNOSTIC 0x10000000L
#endif

namespace {

using PaintBackground_t = void(void*, HDC, void*, const RECT*,
                               const RECT*, const RECT*, const RECT*);

using DispatchMessageW_t = decltype(&DispatchMessageW);

using IntFn = int (*)(void*);

using BoolFn = bool (*)(void*);

using ParentFn = void* (*)(void*);

HMODULE g_dui = nullptr;
PaintBackground_t* g_backgroundOriginal = nullptr;
DispatchMessageW_t g_dispatchOriginal = nullptr;
IntFn g_role = nullptr, g_index = nullptr, g_dpi = nullptr;
BoolFn g_selected = nullptr, g_mouseFocused = nullptr;
ParentFn g_parent = nullptr;

HBRUSH g_dcBrush = nullptr;

HPEN g_nullPen = nullptr;
std::atomic<bool> g_enabled{true};
std::atomic<bool> g_diagnostics{false};

enum class ColorKind : int {
    Fixed, Accent, Background, Foreground, Highlight, HighlightText,
    Window, WindowText, ButtonFace, ButtonText, HotLight, GrayText
};

struct ColorSpec {
    ColorKind kind;
    COLORREF fixed;
};
std::atomic<int> g_lightKind{static_cast<int>(ColorKind::Fixed)};
std::atomic<int> g_darkKind{static_cast<int>(ColorKind::Fixed)};
std::atomic<COLORREF> g_lightFixed{RGB(240, 240, 240)};
std::atomic<COLORREF> g_darkFixed{RGB(48, 48, 48)};
// Packed, premultiplied BGRA color. A single atomic read also gives the alpha.
std::atomic<DWORD> g_paintPixel{0xFFFFFFFF};
// One cached 1x1, top-down 32-bpp DIB used for alpha compositing.
// All accesses to its DC and pixels are serialized across Explorer threads.

SRWLOCK g_blendLock = SRWLOCK_INIT;

HDC g_blendDC = nullptr;

HBITMAP g_blendBitmap = nullptr;

HGDIOBJ g_blendOldBitmap = nullptr;
DWORD* g_blendBits = nullptr;

DWORD g_blendCachedPixel = 0;
std::atomic<BYTE> g_lightAlpha{255}, g_darkAlpha{255};
// These are only accessed under g_paletteLock (slow refresh path).

COLORREF g_lastSource = CLR_INVALID;

int g_lastAlpha = -1;

SRWLOCK g_paletteLock = SRWLOCK_INIT;
std::atomic<bool> g_shareThemes{false};
std::atomic<int> g_radius{8}, g_horizontal{0}, g_vertical{0};
std::atomic<int> g_minWidth{0}, g_parity{1};
std::atomic<bool> g_isDark{false};
std::atomic<DWORD> g_lastThemeCheck{0};

HKEY g_personalizeKey = nullptr;

HANDLE g_personalizeEvent = nullptr;

constexpr PCWSTR kPersonalizePath =
    L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize";
std::atomic<int> g_diagnosticSamples{0};
// Paint callbacks and posted mouse messages are handled on the same UI
// thread. Keep independent drag state for each Explorer UI thread.

thread_local bool g_inColumnDrag = false;

thread_local bool g_repaintOnRelease = false;

thread_local unsigned g_dragGeneration = 0;
std::atomic<unsigned> g_generation{1};
// Small, allocation-free per-thread caches. All entries are non-owning.
// The generation and short expiry avoid retaining stale tree classification
// when Explorer reuses DirectUI elements after navigation/tab changes.

struct RowWidthHistory {
    const void* item;
    int index;
    LONG left, top, bottom, right;
    DWORD tick;
    unsigned generation;
};

struct ParentTreeHistory {
    const void* parent;
    DWORD tick;
    unsigned generation;
    bool inTree;
};
// Per-thread row geometry history.

thread_local RowWidthHistory g_widthHistory[128]{};
// Per-thread ancestry cache avoids re-walking a shared parent per row.

thread_local ParentTreeHistory g_treeHistory[32]{};

bool WidthChangedWhilePressed(void* item, int index, const RECT& row,
                              DWORD now, unsigned generation) {
    const auto key = reinterpret_cast<std::uintptr_t>(item);
    RowWidthHistory& slot = g_widthHistory[((key >> 4) ^ (key >> 13)) & 127];
    bool changed = false;
    if (slot.item == item && slot.generation == generation &&
        slot.index == index && slot.left == row.left &&
        slot.top == row.top && slot.bottom == row.bottom &&
        now - slot.tick < 5000) {
        // Right-edge movement with stable other edges suggests resizing.
        changed = row.right != slot.right;
    }
    slot = {item, index, row.left, row.top, row.bottom,
            row.right, now, generation};
    return changed;
}

int HexDigit(wchar_t c) {
    if (c >= L'0' && c <= L'9') return c - L'0';
    if (c >= L'A' && c <= L'F') return c - L'A' + 10;
    if (c >= L'a' && c <= L'f') return c - L'a' + 10;
    return -1;
}

// Parse only on setting changes, never while painting. No allocations.

bool Matches(PCWSTR text, size_t length, PCWSTR keyword) {
    return wcslen(keyword) == length && _wcsnicmp(text, keyword, length) == 0;
}

ColorSpec ParseColorSpec(PCWSTR text, COLORREF fallback) {
    while (*text == L' ' || *text == L'\t') ++text;
    size_t len = wcslen(text);
    while (len && (text[len-1] == L' ' || text[len-1] == L'\t')) --len;
    if (len && *text == L'#') {++text; --len;}
    if (len == 6) {
        unsigned rgb = 0;
        bool valid = true;
        for (size_t i = 0; i < 6; ++i) {
            int digit = HexDigit(text[i]);
            if (digit < 0) {valid = false; break;}
            rgb = (rgb << 4) | static_cast<unsigned>(digit);
        }
        if (valid) return {ColorKind::Fixed,
            RGB((rgb >> 16) & 255, (rgb >> 8) & 255, rgb & 255)};
    }
    struct NamedColor {PCWSTR name; ColorKind kind;};
    static constexpr NamedColor names[] = {
        {L"accent", ColorKind::Accent},
        {L"background", ColorKind::Background},
        {L"foreground", ColorKind::Foreground},
        {L"highlight", ColorKind::Highlight},
        {L"highlighttext", ColorKind::HighlightText},
        {L"window", ColorKind::Window},
        {L"windowtext", ColorKind::WindowText},
        {L"buttonface", ColorKind::ButtonFace},
        {L"buttontext", ColorKind::ButtonText},
        {L"hotlight", ColorKind::HotLight},
        {L"graytext", ColorKind::GrayText},
    };
    for (const auto& name : names)
        if (Matches(text, len, name.name)) return {name.kind, fallback};
    return {ColorKind::Fixed, fallback};
}

ColorSpec ReadColorSpec(PCWSTR key, COLORREF fallback) {
    auto setting = WindhawkUtils::StringSetting::make(key);
    return ParseColorSpec(setting.get(), fallback);
}

// Convert the UI's integer percentage to the native 0..255 scale with
// round-to-nearest arithmetic; 0 => 0, 50 => 128, and 100 => exactly 255.

BYTE PercentToByte(int percentage) {
    return static_cast<BYTE>((std::clamp(percentage, 0, 100) * 255 + 50) / 100);
}

// Use premultiplied BGRA, required for AC_SRC_ALPHA.

DWORD PremultipliedPixel(COLORREF source, BYTE alpha) {
    const auto scale = [alpha](BYTE channel) -> DWORD {
        return (static_cast<DWORD>(channel) * alpha + 127) / 255;
    };
    return (static_cast<DWORD>(alpha) << 24) |
           (scale(GetRValue(source)) << 16) |
           (scale(GetGValue(source)) << 8) |
            scale(GetBValue(source));
}

bool InitBlendSource() {
    // Supplying a non-NULL DC avoids tying the source to Windhawk's
    // potentially short-lived initialization thread.
    HDC screen = GetDC(nullptr);
    if (!screen) return false;
    g_blendDC = CreateCompatibleDC(screen);
    ReleaseDC(nullptr, screen);
    if (!g_blendDC) return false;
    BITMAPINFO info{};
    info.bmiHeader.biSize = sizeof(info.bmiHeader);
    info.bmiHeader.biWidth = 1;
    info.bmiHeader.biHeight = -1; // top-down
    info.bmiHeader.biPlanes = 1;
    info.bmiHeader.biBitCount = 32;
    info.bmiHeader.biCompression = BI_RGB;
    void* bits = nullptr;
    g_blendBitmap = CreateDIBSection(g_blendDC, &info, DIB_RGB_COLORS,
                                     &bits, nullptr, 0);
    if (!g_blendBitmap || !bits) return false;
    g_blendOldBitmap = SelectObject(g_blendDC, g_blendBitmap);
    if (!g_blendOldBitmap || g_blendOldBitmap == HGDI_ERROR) {
        g_blendOldBitmap = nullptr;
        return false;
    }
    g_blendBits = static_cast<DWORD*>(bits);
    *g_blendBits = 0;
    return true;
}

void FreeBlendSource() {
    if (g_blendDC && g_blendOldBitmap)
        SelectObject(g_blendDC, g_blendOldBitmap);
    if (g_blendBitmap) DeleteObject(g_blendBitmap);
    if (g_blendDC) DeleteDC(g_blendDC);
    g_blendBitmap = nullptr;
    g_blendDC = nullptr;
    g_blendOldBitmap = nullptr;
    g_blendBits = nullptr;
}

// RegNotifyChangeKeyValue registrations are one-shot. Thread agnosticism is
// important because Windhawk can initialize the mod on a short-lived thread.

void ArmPersonalizeNotification() {
    if (g_personalizeKey && g_personalizeEvent) {
        RegNotifyChangeKeyValue(g_personalizeKey, FALSE,
            REG_NOTIFY_CHANGE_LAST_SET | REG_NOTIFY_THREAD_AGNOSTIC,
            g_personalizeEvent, TRUE);
    }
}

void InitPersonalizeNotification() {
    if (RegOpenKeyExW(HKEY_CURRENT_USER, kPersonalizePath, 0,
                      KEY_QUERY_VALUE | KEY_NOTIFY, &g_personalizeKey)
        != ERROR_SUCCESS) return;
    g_personalizeEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    ArmPersonalizeNotification();
}

void FreePersonalizeNotification() {
    if (g_personalizeEvent) CloseHandle(g_personalizeEvent);
    if (g_personalizeKey) RegCloseKey(g_personalizeKey);
    g_personalizeEvent = nullptr;
    g_personalizeKey = nullptr;
}

void SyncDragGeneration(unsigned generation) {
    if (g_dragGeneration != generation) {
        g_dragGeneration = generation;
        g_inColumnDrag = false;
        g_repaintOnRelease = false;
    }
}

void LoadSettings() {
    g_diagnostics.store(Wh_GetIntSetting(L"diagnostics") != 0,
                        std::memory_order_relaxed);
    const ColorSpec light = ReadColorSpec(L"lightColor", RGB(240, 240, 240));
    const ColorSpec dark = ReadColorSpec(L"darkColor", RGB(48, 48, 48));
    g_lightKind.store(static_cast<int>(light.kind), std::memory_order_relaxed);
    g_darkKind.store(static_cast<int>(dark.kind), std::memory_order_relaxed);
    g_lightFixed.store(light.fixed, std::memory_order_relaxed);
    g_darkFixed.store(dark.fixed, std::memory_order_relaxed);
    g_lightAlpha.store(PercentToByte(Wh_GetIntSetting(L"lightAlphaPercent")),
                       std::memory_order_relaxed);
    g_darkAlpha.store(PercentToByte(Wh_GetIntSetting(L"darkAlphaPercent")),
                      std::memory_order_relaxed);
    g_shareThemes.store(Wh_GetIntSetting(L"useSameThemeSettings") != 0,
                        std::memory_order_relaxed);
    g_radius.store(std::clamp(Wh_GetIntSetting(L"cornerRadius"), 0, 30),
                   std::memory_order_relaxed);
    g_horizontal.store(std::clamp(Wh_GetIntSetting(L"horizontalInset"), 0, 32),
                       std::memory_order_relaxed);
    g_vertical.store(std::clamp(Wh_GetIntSetting(L"verticalInset"), 0, 12),
                     std::memory_order_relaxed);
    g_minWidth.store(std::max(0, Wh_GetIntSetting(L"minimumRowWidth")),
                     std::memory_order_relaxed);
    auto parity = WindhawkUtils::StringSetting::make(L"stripeParity");
    // Accept older numeric values from pre-0.13 settings as well.
    g_parity.store(wcscmp(parity.get(), L"even") == 0 ||
                   wcscmp(parity.get(), L"0") == 0 ? 0 : 1,
                   std::memory_order_relaxed);
    g_lastThemeCheck.store(0, std::memory_order_relaxed);
    g_diagnosticSamples.store(0, std::memory_order_relaxed);
    g_generation.fetch_add(1, std::memory_order_relaxed);
}

// Color and appearance queries occur at initialization, settings changes,
// Personalize notifications, and at most once per 3s as a fallback.

COLORREF ResolveColor(ColorKind kind, COLORREF fixed, bool dark,
                      bool contrast, COLORREF accent) {
    switch (kind) {
    case ColorKind::Accent:        return accent;
    // COLOR_WINDOW isn't reliably dark-mode-aware inside File Explorer.
    case ColorKind::Background:    return contrast ? GetSysColor(COLOR_WINDOW)
                                                   : (dark ? RGB(32,32,32) : RGB(255,255,255));
    case ColorKind::Foreground:    return contrast ? GetSysColor(COLOR_WINDOWTEXT)
                                                   : (dark ? RGB(255,255,255) : RGB(0,0,0));
    case ColorKind::Highlight:     return GetSysColor(COLOR_HIGHLIGHT);
    case ColorKind::HighlightText: return GetSysColor(COLOR_HIGHLIGHTTEXT);
    case ColorKind::Window:        return GetSysColor(COLOR_WINDOW);
    case ColorKind::WindowText:    return GetSysColor(COLOR_WINDOWTEXT);
    case ColorKind::ButtonFace:    return GetSysColor(COLOR_3DFACE);
    case ColorKind::ButtonText:    return GetSysColor(COLOR_BTNTEXT);
    case ColorKind::HotLight:      return GetSysColor(COLOR_HOTLIGHT);
    case ColorKind::GrayText:      return GetSysColor(COLOR_GRAYTEXT);
    default:                       return fixed;
    }
}

void RefreshColorsIfDue(DWORD now, bool forced = false) {
    DWORD previous = g_lastThemeCheck.load(std::memory_order_relaxed);
    if (!forced && previous && now - previous < 3000 &&
        (!g_personalizeEvent ||
         WaitForSingleObject(g_personalizeEvent, 0) == WAIT_TIMEOUT))
        return;
    AcquireSRWLockExclusive(&g_paletteLock);
    // Consume/re-arm under the palette lock, so simultaneous paint threads
    // cannot both attempt to register the same one-shot notification.
    const bool changed = g_personalizeEvent &&
        WaitForSingleObject(g_personalizeEvent, 0) == WAIT_OBJECT_0;
    if (changed) {
        ResetEvent(g_personalizeEvent);
        ArmPersonalizeNotification();
    }
    previous = g_lastThemeCheck.load(std::memory_order_relaxed);
    if (!forced && !changed && previous && now - previous < 3000) {
        ReleaseSRWLockExclusive(&g_paletteLock);
        return;
    }
    DWORD lightSetting = 1, size = sizeof(lightSetting);
    bool darkMode = g_isDark.load(std::memory_order_relaxed);
    if (RegGetValueW(g_personalizeKey ? g_personalizeKey : HKEY_CURRENT_USER,
                     g_personalizeKey ? nullptr : kPersonalizePath,
                     L"AppsUseLightTheme", RRF_RT_REG_DWORD,
                     nullptr, &lightSetting, &size) == ERROR_SUCCESS)
        darkMode = (lightSetting == 0);
    g_isDark.store(darkMode, std::memory_order_relaxed);
    const bool shared = g_shareThemes.load(std::memory_order_relaxed);
    const bool chooseLight = shared || !darkMode;
    const ColorKind kind = static_cast<ColorKind>((chooseLight ? g_lightKind : g_darkKind)
                               .load(std::memory_order_relaxed));
    const COLORREF fixed = (chooseLight ? g_lightFixed : g_darkFixed)
                               .load(std::memory_order_relaxed);
    const BYTE alpha = (chooseLight ? g_lightAlpha : g_darkAlpha)
                               .load(std::memory_order_relaxed);
    const bool needsContrast = kind == ColorKind::Background ||
                               kind == ColorKind::Foreground;
    bool contrast = false;
    if (needsContrast) {
        HIGHCONTRASTW hc{};
        hc.cbSize = sizeof(hc);
        if (SystemParametersInfoW(SPI_GETHIGHCONTRAST, sizeof(hc), &hc, 0))
            contrast = (hc.dwFlags & HCF_HIGHCONTRASTON) != 0;
    }
    COLORREF accent = RGB(0, 120, 215); // fallback only if DWM fails
    if (kind == ColorKind::Accent) {
        DWORD colorization = 0;
        BOOL opaque = FALSE;
        if (SUCCEEDED(DwmGetColorizationColor(&colorization, &opaque)))
            accent = RGB((colorization >> 16) & 255,
                         (colorization >> 8) & 255, colorization & 255);
    }
    const COLORREF source = ResolveColor(kind, fixed, darkMode, contrast, accent);
    if (source != g_lastSource || alpha != g_lastAlpha) {
        g_paintPixel.store(PremultipliedPixel(source, alpha),
                           std::memory_order_relaxed);
        g_lastSource = source;
        g_lastAlpha = alpha;
    }
    g_lastThemeCheck.store(now, std::memory_order_relaxed);
    ReleaseSRWLockExclusive(&g_paletteLock);
}

// Cheap geometry checks before any DirectUI method is called. No hardcoded
// view-mode or minimum-width gate. A user-requested width filter is optional.
// Keep arithmetic widened to handle large virtual desktop coordinates.

bool HasPossibleRowGeometry(const RECT* rc) {
    if (!rc) return false;
    const auto width = static_cast<int64_t>(rc->right) - rc->left;
    const auto height = static_cast<int64_t>(rc->bottom) - rc->top;
    return width > 0 && height > 0 &&
           width >= g_minWidth.load(std::memory_order_relaxed);
}

int RowDpi(void* element) {
    const int dpi = g_dpi(element);
    // Guard a bad/unsupported DirectUI return rather than mis-draw.
    return dpi >= 72 && dpi <= 768 ? dpi : 96;
}

bool IsCompactListItemHeight(const RECT& row, int dpi) {
    const auto height = static_cast<int64_t>(row.bottom) - row.top;
    // Retain a compact-item height guard (19..52 at 96 DPI) so large
    // icon/thumbnail tiles aren't treated as horizontal list rows.
    // The range scales with the item's actual DPI.
    // Integer comparisons avoid MulDiv calls and rounding ambiguity.
    const auto scaled = height * 96;
    return scaled >= 19 * dpi && scaled <= 52 * dpi;
}

bool IsInNavigationTree(void* element, DWORD now, unsigned generation) {
    void* first = g_parent(element);
    if (!first) return false;
    const auto key = reinterpret_cast<std::uintptr_t>(first);
    ParentTreeHistory& slot = g_treeHistory[((key >> 4) ^ (key >> 12)) & 31];
    if (slot.parent == first && slot.generation == generation &&
        now - slot.tick < 250) {
        return slot.inTree;
    }
    bool inTree = false;
    void* p = first;
    for (int i = 0; p && i < 8; ++i) {
        const int role = g_role(p);
        if (role == ROLE_SYSTEM_OUTLINE || role == ROLE_SYSTEM_OUTLINEITEM) {
            inTree = true;
            break;
        }
        // A list ancestor is positive evidence that this isn't a tree item;
        // no reason to keep walking to the root in the common case.
        if (role == ROLE_SYSTEM_LIST) break;
        void* next = g_parent(p);
        if (next == p) break;
        p = next;
    }
    slot = {first, now, generation, inTree};
    return inTree;
}

// The fully opaque path uses ordinary GDI. Partial opacity is composed
// against the destination pixels; the cached DIB avoids per-row allocations.

bool DrawStripe(HDC dc, const RECT& nativeRow, int dpi, DWORD now,
                bool flatRight) {
    const int horizontal = g_horizontal.load(std::memory_order_relaxed);
    const int vertical = g_vertical.load(std::memory_order_relaxed);
    const int requestedRadius = g_radius.load(std::memory_order_relaxed);
    const int hx = dpi == 96 ? horizontal : MulDiv(horizontal, dpi, 96);
    const int vy = dpi == 96 ? vertical : MulDiv(vertical, dpi, 96);
    const RECT dst{nativeRow.left + hx, nativeRow.top + vy,
                   nativeRow.right - hx, nativeRow.bottom - vy};
    if (dst.left >= dst.right || dst.top >= dst.bottom) return false;
    int radius = dpi == 96 ? requestedRadius : MulDiv(requestedRadius, dpi, 96);
    radius = std::min(radius, static_cast<int>((dst.bottom - dst.top) / 2));
    radius = std::min(radius, static_cast<int>((dst.right - dst.left) / 2));
    RefreshColorsIfDue(now);
    const DWORD pixel = g_paintPixel.load(std::memory_order_relaxed);
    const BYTE alpha = static_cast<BYTE>(pixel >> 24);
    if (!alpha) return false;
    if (alpha == 255) {
        const COLORREF fill = RGB((pixel >> 16) & 255,
                                  (pixel >> 8) & 255, pixel & 255);
        if (radius == 0) {
            const COLORREF old = SetDCBrushColor(dc, fill);
            if (old == CLR_INVALID) return false;
            const bool painted = !!FillRect(dc, &dst, g_dcBrush);
            SetDCBrushColor(dc, old);
            return painted;
        }
        const int saved = SaveDC(dc);
        if (!saved) return false;
        bool painted = false;
        if (IntersectClipRect(dc, dst.left, dst.top, dst.right, dst.bottom) > 1) {
            SelectObject(dc, g_dcBrush);
            SelectObject(dc, g_nullPen);
            SetDCBrushColor(dc, fill);
            const int shapeRight = flatRight ? dst.right + radius * 2 + 2 : dst.right;
            painted = !!RoundRect(dc, dst.left, dst.top, shapeRight, dst.bottom,
                                  radius * 2, radius * 2);
        }
        RestoreDC(dc, saved);
        return painted;
    }
    const int saved = SaveDC(dc);
    if (!saved) return false;
    bool painted = false;
    const int clip = IntersectClipRect(dc, dst.left, dst.top, dst.right, dst.bottom);
    if (clip != ERROR && clip != NULLREGION) {
        bool shapeReady = true;
        if (radius) {
            const int shapeRight = flatRight ? dst.right + radius * 2 + 2 : dst.right;
            shapeReady = !!BeginPath(dc);
            if (shapeReady) {
                const BOOL rounded = RoundRect(dc, dst.left, dst.top,
                                               shapeRight, dst.bottom,
                                               radius * 2, radius * 2);
                const BOOL ended = EndPath(dc);
                shapeReady = rounded && ended &&
                             SelectClipPath(dc, RGN_AND) != ERROR;
            }
        }
        if (shapeReady) {
            AcquireSRWLockExclusive(&g_blendLock);
            if (pixel != g_blendCachedPixel) {
                // The source DC is only ever used under this lock.
                *g_blendBits = pixel;
                g_blendCachedPixel = pixel;
            }
            const BLENDFUNCTION blend{AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
            painted = !!GdiAlphaBlend(dc, dst.left, dst.top,
                                      dst.right - dst.left, dst.bottom - dst.top,
                                      g_blendDC, 0, 0, 1, 1, blend);
            ReleaseSRWLockExclusive(&g_blendLock);
        }
    }
    RestoreDC(dc, saved);
    return painted;
}

void PaintBackgroundHook(void* element, HDC dc, void* value,
                         const RECT* rc, const RECT* b,
                         const RECT* c, const RECT* d) {
    // Preserve Explorer's native background rendering order first.
    g_backgroundOriginal(element, dc, value, rc, b, c, d);
    if (!g_enabled.load(std::memory_order_relaxed) || !element || !dc ||
        !HasPossibleRowGeometry(rc) || g_role(element) != ROLE_SYSTEM_LISTITEM)
        return;
    const int index = g_index(element);
    if (index < 0 || (index & 1) != g_parity.load(std::memory_order_relaxed))
        return;
    if (g_selected(element) || g_mouseFocused(element)) return;
    // Query DPI once for both row classification and drawing. This makes
    // the row filter work on mixed-DPI systems without an extra API call.
    const int dpi = RowDpi(element);
    if (!IsCompactListItemHeight(*rc, dpi)) return;
    const DWORD now = GetTickCount();
    const unsigned generation = g_generation.load(std::memory_order_relaxed);
    SyncDragGeneration(generation);
    if (IsInNavigationTree(element, now, generation)) return;
    // Maintain per-row history even when the mouse isn't down. The first
    // width change then identifies a resize without additional event hooks.
    const bool resized = WidthChangedWhilePressed(element, index, *rc,
                                                   now, generation);
    const bool dragWasActive = g_inColumnDrag;
    // Avoid a User32 key-state call on every row during ordinary painting.
    // Only check it when a width change or existing drag makes it relevant.
    const bool down = (resized || dragWasActive) &&
                      (GetKeyState(VK_LBUTTON) & 0x8000) != 0;
    if (resized && down) {
        g_inColumnDrag = true;
        if (g_diagnostics.load(std::memory_order_relaxed)) {
            const int n = g_diagnosticSamples.fetch_add(1, std::memory_order_relaxed);
            if (n < 12) {
                Wh_Log(L"Zebra v0.13.0: resize[%d] row=%d width=%ld; flat right cap active",
                       n, index, rc->right - rc->left);
            }
        }
    }
    const bool flat = down && (resized || dragWasActive);
    const bool drew = DrawStripe(dc, *rc, dpi, now, flat);
    if (flat && drew) {
        g_repaintOnRelease = true;
    }
}

// Only used at setup/settings changes/teardown (NEVER on a resize frame).

BOOL CALLBACK InvalidateDirectUI(HWND hwnd, LPARAM) {
    wchar_t cls[64]{};
    if (GetClassNameW(hwnd, cls, ARRAYSIZE(cls)) &&
        (_wcsicmp(cls, L"DirectUIHWND") == 0 ||
         _wcsicmp(cls, L"SHELLDLL_DefView") == 0))
        InvalidateRect(hwnd, nullptr, FALSE);
    return TRUE;
}

BOOL CALLBACK InvalidateExplorerWindow(HWND hwnd, LPARAM) {
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    if (pid != GetCurrentProcessId()) return TRUE;
    wchar_t cls[64]{};
    if (GetClassNameW(hwnd, cls, ARRAYSIZE(cls)) &&
        _wcsicmp(cls, L"CabinetWClass") == 0)
        EnumChildWindows(hwnd, InvalidateDirectUI, 0);
    return TRUE;
}

void RepaintExplorerOnce() {
    EnumWindows(InvalidateExplorerWindow, 0);
}

// Prefer the window containing the release message, rather than every
// Explorer window. Fallback is also exactly one invalidation pass.

void RepaintAfterDrag(HWND sender) {
    HWND root = sender ? GetAncestor(sender, GA_ROOT) : nullptr;
    if (root && IsWindow(root)) {
        wchar_t cls[64]{};
        if (GetClassNameW(root, cls, ARRAYSIZE(cls)) &&
            _wcsicmp(cls, L"CabinetWClass") == 0) {
            EnumChildWindows(root, InvalidateDirectUI, 0);
            return;
        }
    }
    RepaintExplorerOnce();
}

// No persistent listeners or window subclasses. Process the original message
// first, then invalidate the affected Explorer window if a drag just ended.

LRESULT WINAPI DispatchMessageHook(const MSG* msg) {
    LRESULT result = g_dispatchOriginal(msg);
    if (!msg) return result;
    const UINT m = msg->message;
    SyncDragGeneration(g_generation.load(std::memory_order_relaxed));
    if (!g_inColumnDrag) return result;
    // DispatchMessageW sees posted messages, not synchronous window sends.
    const bool definiteEnd = m == WM_LBUTTONUP ||
        (m == WM_KEYDOWN && msg->wParam == VK_ESCAPE);
    const bool endedWithoutButtonUp =
        (m == WM_MOUSEMOVE || m == WM_POINTERUP) &&
        !(GetAsyncKeyState(VK_LBUTTON) & 0x8000);
    if (definiteEnd || endedWithoutButtonUp) {
        g_inColumnDrag = false;
        if (g_repaintOnRelease) {
            g_repaintOnRelease = false;
            if (g_diagnostics.load(std::memory_order_relaxed))
                Wh_Log(L"Zebra v0.13.0: drag ended (msg=0x%x); one repaint", m);
            RepaintAfterDrag(msg->hwnd);
        }
    }
    return result;
}

template<typename T>
T Export(HMODULE module, const char* symbol) {
    return reinterpret_cast<T>(GetProcAddress(module, symbol));
}
} // namespace

BOOL Wh_ModInit() {
    LoadSettings();
    InitPersonalizeNotification();
    RefreshColorsIfDue(GetTickCount(), true);
    // Hold a normal (non-pinned) module reference until Windhawk removes the
    // detour during unloading.
    g_dui = LoadLibraryExW(L"DUI70.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!g_dui) {
        Wh_Log(L"Zebra v0.13.0: unable to load DUI70.dll");
        FreePersonalizeNotification();
        return FALSE;
    }
    g_role = Export<IntFn>(g_dui, "?GetAccRole@Element@DirectUI@@QEAAHXZ");
    g_index = Export<IntFn>(g_dui, "?GetIndex@Element@DirectUI@@QEAAHXZ");
    g_dpi = Export<IntFn>(g_dui, "?GetDPI@Element@DirectUI@@QEAAHXZ");
    g_selected = Export<BoolFn>(g_dui, "?GetSelected@Element@DirectUI@@QEAA_NXZ");
    g_mouseFocused = Export<BoolFn>(g_dui, "?GetMouseFocused@Element@DirectUI@@QEAA_NXZ");
    g_parent = Export<ParentFn>(g_dui, "?GetParent@Element@DirectUI@@QEAAPEAV12@XZ");
    auto bg = Export<PaintBackground_t*>(g_dui,
        "?PaintBackground@Element@DirectUI@@QEAAXPEAUHDC__@@PEAVValue@2@"
        "AEBUtagRECT@@222@Z");
    auto dispatch = DispatchMessageW;
    if (!g_role || !g_index || !g_dpi || !g_selected ||
        !g_mouseFocused || !g_parent || !bg || !dispatch) {
        Wh_Log(L"Zebra v0.13.0: missing required export");
        FreePersonalizeNotification();
        FreeLibrary(g_dui);
        g_dui = nullptr;
        return FALSE;
    }
    // Cache stock GDI objects: avoid repeated GetStockObject calls on each
    // painted stripe. Neither stock object needs DeleteObject.
    g_dcBrush = reinterpret_cast<HBRUSH>(GetStockObject(DC_BRUSH));
    g_nullPen = reinterpret_cast<HPEN>(GetStockObject(NULL_PEN));
    if (!g_dcBrush || !g_nullPen) {
        Wh_Log(L"Zebra v0.13.0: unable to get stock GDI objects");
        FreePersonalizeNotification();
        FreeLibrary(g_dui);
        g_dui = nullptr;
        return FALSE;
    }
    if (!InitBlendSource()) {
        Wh_Log(L"Zebra v0.13.0: unable to create alpha blend source");
        FreeBlendSource();
        FreePersonalizeNotification();
        FreeLibrary(g_dui);
        g_dui = nullptr;
        return FALSE;
    }
    if (!WindhawkUtils::SetFunctionHook(dispatch, DispatchMessageHook,
                                        &g_dispatchOriginal) ||
        !WindhawkUtils::SetFunctionHook(bg, PaintBackgroundHook,
                                        &g_backgroundOriginal)) {
        Wh_Log(L"Zebra v0.13.0: hook registration failed");
        FreeBlendSource();
        FreePersonalizeNotification();
        FreeLibrary(g_dui);
        g_dui = nullptr;
        return FALSE;
    }
    Wh_Log(L"Zebra v0.13.0: PaintBackground + mouse-release hooks installed (PID=%lu)",
           GetCurrentProcessId());
    return TRUE;
}

void Wh_ModAfterInit() {
    RepaintExplorerOnce();
}

void Wh_ModSettingsChanged() {
    LoadSettings();
    RefreshColorsIfDue(GetTickCount(), true);
    RepaintExplorerOnce();
}

void Wh_ModBeforeUninit() {
    g_enabled.store(false, std::memory_order_relaxed);
}

void Wh_ModUninit() {
    FreePersonalizeNotification();
    FreeBlendSource();
    if (g_dui) {
        FreeLibrary(g_dui);
        g_dui = nullptr;
    }
    // Invalidate existing stripes after the detours have been removed.
    RepaintExplorerOnce();
}
