// ==WindhawkMod==
// @id              explorer-zebra-stripes
// @name            Explorer List Zebra Stripes
// @description     Add stripes for Details and List items in File Explorer
// @version         0.12.1
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
![image](https://raw.githubusercontent.com/DarkionAvey/windhawk-explorer-zebra-stripes/refs/heads/main/screenshot.png)

Adds alternating stripes to compact file-list items in Explorer, including Details and List views.


# Colors
Supports #RRGGBB and dynamic system color names.

Dynamic names: accent, background, foreground, highlight, highlighttext,
window, windowtext, buttonface, buttontext, hotlight, graytext.
`accent` uses the Windows DWM colorization color. `background` and
`foreground` are theme-aware neutral colors (high-contrast aware); other
system names use Win32 system colors.

Opacity is a percentage: 0 = invisible, 100 = fully opaque. 

The optional same-settings toggle uses lightColor/lightAlphaPercent for both modes.

Tested on Windows 11 24H2 & 26H2
*/
// ==/WindhawkModReadme==
// ==WindhawkModSettings==
/*
- enableDrawing: true
  $name: Draw zebra stripes
- useSameThemeSettings: false
  $name: Use light color and opacity for both themes
  $description: When enabled, lightColor and lightAlphaPercent apply in both light and dark mode. Dark color/opacity are ignored.
- lightColor: "foreground"
  $name: Light / shared stripe color
  $description: 'Copy a name: accent, background, foreground, highlight, highlighttext, window, windowtext, buttonface, buttontext, hotlight, graytext. Or enter RRGGBB / #RRGGBB. background/foreground follow theme.'
- lightAlphaPercent: 5
  $name: Light / shared opacity (%)
  $description: '0 = invisible, 50 = 50% opacity, 100 = fully opaque. Mixed once with a theme-aware approximate Explorer background, not per row.'
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
- stripeParity: 1
  $name: Item index parity (0 = even, 1 = odd)
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
namespace {
// Signature verified by the v0.7 tracing build on Windows 11 24H2.
using BackgroundFn = void (*)(void*, HDC, void*, const RECT*,
                              const RECT*, const RECT*, const RECT*);
using DispatchFn = LRESULT (WINAPI*)(const MSG*);
using IntFn = int (*)(void*);
using BoolFn = bool (*)(void*);
using ParentFn = void* (*)(void*);
HMODULE g_dui = nullptr;
BackgroundFn g_backgroundOriginal = nullptr;
DispatchFn g_dispatchOriginal = nullptr;
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
// One resolved, pre-mixed color for the active theme. The hot paint path
// reads only the color and whether 0% opacity disables drawing.
std::atomic<COLORREF> g_paintColor{RGB(240, 240, 240)};
std::atomic<bool> g_paintVisible{true};
std::atomic<BYTE> g_lightAlpha{255}, g_darkAlpha{255};
// These are only accessed under g_paletteLock (slow refresh path).
COLORREF g_lastSource = CLR_INVALID, g_lastBase = CLR_INVALID;
int g_lastAlpha = -1;
SRWLOCK g_paletteLock = SRWLOCK_INIT;
std::atomic<bool> g_shareThemes{false};
std::atomic<DWORD> g_lastColorEvent{0};
std::atomic<int> g_radius{8}, g_horizontal{0}, g_vertical{0};
std::atomic<int> g_minWidth{0}, g_parity{1};
std::atomic<bool> g_isDark{false};
std::atomic<DWORD> g_lastThemeCheck{0};
std::atomic<int> g_diagnosticSamples{0};
std::atomic<bool> g_inColumnDrag{false};
std::atomic<bool> g_repaintOnRelease{false};
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

// 128 * 40 bytes on x64 (previously 256 * 40 = 10 KiB).
thread_local RowWidthHistory g_widthHistory[128]{};
// 32 * 24 bytes on x64; avoids re-walking a shared parent for each row.
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
        // Compare actual geometry: rapid drags can exceed an arbitrary
        // per-paint pixel limit, but the native left/top/bottom stay fixed.
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
    if (!text) return {ColorKind::Fixed, fallback};
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
    PCWSTR setting = Wh_GetStringSetting(key);
    ColorSpec spec = ParseColorSpec(setting, fallback);
    if (setting) Wh_FreeStringSetting(setting);
    return spec;
}
// Convert the UI's integer percentage to the native 0..255 scale with
// round-to-nearest arithmetic; 0 => 0, 50 => 128, and 100 => exactly 255.
BYTE PercentToByte(int percentage) {
    return static_cast<BYTE>((std::clamp(percentage, 0, 100) * 255 + 50) / 100);
}

// Mix once against a cached neutral background, NOT the live destination DC.
// Integer-only math rounds each channel to nearest. No work in DrawStripe().
COLORREF PremixColor(COLORREF source, COLORREF background, BYTE alpha) {
    if (alpha == 255) return source;
    if (alpha == 0) return background;
    const unsigned inverse = 255 - alpha;
    const auto mix = [alpha, inverse](unsigned a, unsigned b) -> int {
        return static_cast<int>((a * alpha + b * inverse + 127) / 255);
    };
    return RGB(mix(GetRValue(source), GetRValue(background)),
               mix(GetGValue(source), GetGValue(background)),
               mix(GetBValue(source), GetBValue(background)));
}

void LoadSettings() {
    g_enabled.store(Wh_GetIntSetting(L"enableDrawing") != 0,
                    std::memory_order_relaxed);
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
    // Zero really disables the filter; no arbitrary 160 px floor.
    g_minWidth.store(std::max(0, Wh_GetIntSetting(L"minimumRowWidth")),
                     std::memory_order_relaxed);
    g_parity.store(Wh_GetIntSetting(L"stripeParity") & 1,
                   std::memory_order_relaxed);
    g_lastThemeCheck.store(0, std::memory_order_relaxed);
    g_lastColorEvent.store(0, std::memory_order_relaxed);
    g_diagnosticSamples.store(0, std::memory_order_relaxed);
    g_inColumnDrag.store(false, std::memory_order_relaxed);
    g_repaintOnRelease.store(false, std::memory_order_relaxed);
    g_generation.fetch_add(1, std::memory_order_relaxed);
}
// All Windows color queries are on a slow path: at initialization, settings
// changes, appearance notifications, and at most once per 3s as a fallback
// for synchronous broadcasts that DispatchMessageW does not intercept.
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
    if (!forced && previous && now - previous < 3000) return;

    // Only the infrequent refresh serializes. The paint drawing operations
    // themselves take no locks, allocate nothing and do no channel mixing.
    AcquireSRWLockExclusive(&g_paletteLock);
    previous = g_lastThemeCheck.load(std::memory_order_relaxed);
    if (!forced && previous && now - previous < 3000) {
        ReleaseSRWLockExclusive(&g_paletteLock);
        return;
    }

    DWORD lightSetting = 1, size = sizeof(lightSetting);
    bool darkMode = g_isDark.load(std::memory_order_relaxed);
    if (RegGetValueW(HKEY_CURRENT_USER,
                     L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
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
    const bool needsContrast = alpha != 255 || kind == ColorKind::Background ||
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
    const COLORREF base = contrast ? GetSysColor(COLOR_WINDOW)
                                   : (darkMode ? RGB(32,32,32) : RGB(255,255,255));

    // Only recomposite if the effective foreground, background or alpha
    // actually changed. The 3s fallback typically has no mixing work.
    if (source != g_lastSource || base != g_lastBase ||
        alpha != g_lastAlpha) {
        g_paintColor.store(PremixColor(source, base, alpha),
                           std::memory_order_relaxed);
        g_paintVisible.store(alpha != 0, std::memory_order_relaxed);
        g_lastSource = source;
        g_lastBase = base;
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

// Opacity has already been converted to one cached COLORREF. Nothing needs
// to allocate, lock, or call GdiAlphaBlend when an individual row is painted.

// Draw only inside the row's own paint callback. Opaque stripes keep the
// original low-overhead GDI path; cached opacity does not change drawing.
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
    if (!g_paintVisible.load(std::memory_order_relaxed)) return false;
    const COLORREF fill = g_paintColor.load(std::memory_order_relaxed);

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
    if (IsInNavigationTree(element, now, generation)) return;

    // Maintain per-row history even when the mouse isn't down. The first
    // width change then identifies a resize without additional event hooks.
    const bool resized = WidthChangedWhilePressed(element, index, *rc,
                                                   now, generation);
    const bool dragWasActive = g_inColumnDrag.load(std::memory_order_relaxed);

    // Avoid a User32 key-state call on every row during ordinary painting.
    // Only check it when a width change or existing drag makes it relevant.
    const bool down = (resized || dragWasActive) &&
                      (GetKeyState(VK_LBUTTON) & 0x8000) != 0;
    if (resized && down) {
        g_inColumnDrag.store(true, std::memory_order_relaxed);
        if (g_diagnostics.load(std::memory_order_relaxed)) {
            const int n = g_diagnosticSamples.fetch_add(1, std::memory_order_relaxed);
            if (n < 12) {
                Wh_Log(L"Zebra v0.12.1: resize[%d] row=%d width=%ld; flat right cap active",
                       n, index, rc->right - rc->left);
            }
        }
    }
    const bool flat = down && (resized || dragWasActive);
    const bool drew = DrawStripe(dc, *rc, dpi, now, flat);
    if (flat && drew) {
        g_repaintOnRelease.store(true, std::memory_order_relaxed);
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
// This hook has no persistent listeners or window subclasses. It only checks
// two inexpensive atomics on ordinary messages. After mouse release the
// original Windows/Explorer handler runs first, then we request one repaint.
LRESULT WINAPI DispatchMessageHook(const MSG* msg) {
    LRESULT result = g_dispatchOriginal(msg);
    if (!msg) return result;
    const UINT m = msg->message;
    // Existing DispatchMessageW hook can catch posted appearance changes.
    // Synchronous window broadcasts bypass it, so the 3s paint-path check
    // is retained. Invalidation is throttled for message storms.
    if (m == WM_DWMCOLORIZATIONCOLORCHANGED || m == WM_THEMECHANGED ||
        m == WM_SYSCOLORCHANGE || m == WM_SETTINGCHANGE) {
        const DWORD now = GetTickCount();
        const DWORD last = g_lastColorEvent.load(std::memory_order_relaxed);
        if (!last || now - last >= 250) {
            g_lastColorEvent.store(now, std::memory_order_relaxed);
            RefreshColorsIfDue(now, true);
            RepaintExplorerOnce();
        }
    }
    if (!g_inColumnDrag.load(std::memory_order_relaxed)) return result;
    const bool definiteEnd = m == WM_LBUTTONUP || m == WM_CANCELMODE ||
        (m == WM_KEYDOWN && msg->wParam == VK_ESCAPE) ||
        (m == WM_ACTIVATEAPP && msg->wParam == FALSE);
    // Fallback for capture cancellation or a different thread delivering
    // the release. Only checked while a repaint is pending, on mouse events.
    const bool endedWithoutButtonUp =
        (m == WM_MOUSEMOVE || m == WM_POINTERUP || m == WM_CAPTURECHANGED) &&
        !(GetAsyncKeyState(VK_LBUTTON) & 0x8000);
    if (definiteEnd || endedWithoutButtonUp) {
        g_inColumnDrag.store(false, std::memory_order_relaxed);
        if (g_repaintOnRelease.exchange(false, std::memory_order_acq_rel)) {
            if (g_diagnostics.load(std::memory_order_relaxed))
                Wh_Log(L"Zebra v0.12.1: drag ended (msg=0x%x); one repaint", m);
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
    // Resolve/pre-mix the first paint color before detours become active.
    RefreshColorsIfDue(GetTickCount(), true);
    // Hold a normal (non-pinned) module reference until Windhawk removes the
    // detour during unloading.
    g_dui = LoadLibraryExW(L"DUI70.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!g_dui) {
        Wh_Log(L"Zebra v0.12.1: unable to load DUI70.dll");
        return FALSE;
    }
    g_role = Export<IntFn>(g_dui, "?GetAccRole@Element@DirectUI@@QEAAHXZ");
    g_index = Export<IntFn>(g_dui, "?GetIndex@Element@DirectUI@@QEAAHXZ");
    g_dpi = Export<IntFn>(g_dui, "?GetDPI@Element@DirectUI@@QEAAHXZ");
    g_selected = Export<BoolFn>(g_dui, "?GetSelected@Element@DirectUI@@QEAA_NXZ");
    g_mouseFocused = Export<BoolFn>(g_dui, "?GetMouseFocused@Element@DirectUI@@QEAA_NXZ");
    g_parent = Export<ParentFn>(g_dui, "?GetParent@Element@DirectUI@@QEAAPEAV12@XZ");
    void* bg = Export<void*>(g_dui,
        "?PaintBackground@Element@DirectUI@@QEAAXPEAUHDC__@@PEAVValue@2@"
        "AEBUtagRECT@@222@Z");
    HMODULE user32 = GetModuleHandleW(L"user32.dll");
    void* dispatch = user32 ? Export<void*>(user32, "DispatchMessageW") : nullptr;
    if (!g_role || !g_index || !g_dpi || !g_selected ||
        !g_mouseFocused || !g_parent || !bg || !dispatch) {
        Wh_Log(L"Zebra v0.12.1: missing required export");
        FreeLibrary(g_dui);
        g_dui = nullptr;
        return FALSE;
    }
    // Cache stock GDI objects: avoid repeated GetStockObject calls on each
    // painted stripe. Neither stock object needs DeleteObject.
    g_dcBrush = reinterpret_cast<HBRUSH>(GetStockObject(DC_BRUSH));
    g_nullPen = reinterpret_cast<HPEN>(GetStockObject(NULL_PEN));
    if (!g_dcBrush || !g_nullPen) {
        Wh_Log(L"Zebra v0.12.1: unable to get stock GDI objects");
        FreeLibrary(g_dui);
        g_dui = nullptr;
        return FALSE;
    }

    // Windhawk applies and removes both detours as a unit. There are no
    // Win32 subclasses, window properties, or callback objects to detach.
    if (!Wh_SetFunctionHook(dispatch, reinterpret_cast<void*>(DispatchMessageHook),
                            reinterpret_cast<void**>(&g_dispatchOriginal)) ||
        !Wh_SetFunctionHook(bg, reinterpret_cast<void*>(PaintBackgroundHook),
                            reinterpret_cast<void**>(&g_backgroundOriginal))) {
        Wh_Log(L"Zebra v0.12.1: hook registration failed");
        FreeLibrary(g_dui);
        g_dui = nullptr;
        return FALSE;
    }
    Wh_Log(L"Zebra v0.12.1: PaintBackground + mouse-release hooks installed (PID=%lu)",
           GetCurrentProcessId());
    return TRUE;
}
void Wh_ModAfterInit() {
    RepaintExplorerOnce();
}
BOOL Wh_ModSettingsChanged(BOOL* reload) {
    (void)reload;
    LoadSettings();
    RefreshColorsIfDue(GetTickCount(), true);
    RepaintExplorerOnce();
    return TRUE;
}
void Wh_ModBeforeUninit() {
    g_enabled.store(false, std::memory_order_relaxed);
    g_repaintOnRelease.store(false, std::memory_order_relaxed);
    g_inColumnDrag.store(false, std::memory_order_relaxed);
}
void Wh_ModUninit() {
    if (g_dui) {
        FreeLibrary(g_dui);
        g_dui = nullptr;
    }
    // The host has removed the detour by this point. This is a single
    // asynchronous invalidation to remove any previously drawn stripes.
    RepaintExplorerOnce();
}
