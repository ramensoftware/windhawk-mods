// ==WindhawkMod==
// @id              explorer-zebra-stripes
// @name            Explorer List Zebra Stripes
// @description     Add stripes for Details and List items in File Explorer
// @version         0.14.2
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

Adds alternating stripes to compact file-list items in Explorer, including Details and List views.

## Colors

* Supports #RRGGBB and dynamic system color names: `accent`, `background`, `foreground`, `highlight`, `highlighttext`, `window`, `windowtext`, `buttonface`, `buttontext`, `hotlight`, `graytext`. `accent` uses the Windows DWM colorization color. `background` and `foreground` are theme-aware neutral colors (high-contrast aware); other system names use Win32 system colors.
* Opacity is a percentage: 0 = invisible, 100 = fully opaque.

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
- darkAlphaPercent: 4
  $name: Dark-theme opacity (%)
  $description: '0 = invisible, 50 = 50% opacity, 100 = fully opaque. Ignored when useSameThemeSettings is enabled.'
- cornerRadius: 8
  $name: Corner radius (at 96 DPI)
  $description: '-1 = fully rounded pill; 0 = square corners; > 0 : standard rounded rect.'
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
  $name: Log bounded resize/release and identity fallback details
  $description: Normally off to avoid per-row logging or diagnostic counters.

*/
// ==/WindhawkModSettings==
#include <windows.h>
#include <oleacc.h>
#include <dwmapi.h>
#include <atomic>
#include <algorithm>
#include <array>
#include <memory>
#include <new>
#include <limits>
#include <cwchar>
#include <cstdint>
#include <windhawk_utils.h>

#ifndef REG_NOTIFY_THREAD_AGNOSTIC
#define REG_NOTIFY_THREAD_AGNOSTIC 0x10000000L
#endif

namespace {
constexpr int kReferenceDpi = 96;
constexpr int kMinSupportedDpi = 72;
constexpr int kMaxSupportedDpi = 768;
constexpr int kCompactRowMinHeight = 19;  // At kReferenceDpi.
constexpr int kCompactRowMaxHeight = 52;  // At kReferenceDpi.
constexpr int kMaxAncestryDepth = 8;
constexpr int kMaxWindowAncestryDepth = 16;
constexpr int kMaxIdentityCodeUnits = 2048; // Virtual shell names may exceed MAX_PATH.
// Every eligible row is at most 52 logical pixels high, even at maximum DPI.
// Keep a defensive cap if the row-classification contract ever changes.
constexpr int kMaxRoundedBitmapHeight =
    (kCompactRowMaxHeight * kMaxSupportedDpi + kReferenceDpi - 1) / kReferenceDpi;
// A pathological/invalid DirectUI width must never request a giant stretched
// AlphaBlend operation on Explorer's UI thread. Crop such rows to the HDC clip.
constexpr int kMaxUnclippedBlendWidth = 16384;
constexpr DWORD kFailedShapeRetryMs = 2000;
constexpr DWORD kThemeFallbackRefreshMs = 3000;
constexpr DWORD kThemeEventProbeMs = 200;
constexpr DWORD kFileNameAtomProbeMs = 250;
constexpr DWORD kParentTreeCacheMs = 250;
constexpr DWORD kResizeHistoryMs = 5000;
// Smooth scrolling can continue to paint after the input message was handled.
constexpr DWORD kScrollPaintGraceMs = 750;
// WM_PAINT normally separates batches; these cover sent/nonstandard paints.
constexpr DWORD kStripePaintIdleGapMs = 50;
constexpr int kStripePaintIndexWrap = 8;
constexpr size_t kWidthHistorySlots = 128;
constexpr size_t kParentHistorySlots = 32;
static_assert((kWidthHistorySlots & (kWidthHistorySlots - 1)) == 0);
static_assert((kParentHistorySlots & (kParentHistorySlots - 1)) == 0);
using PaintBackground_t = void(void*, HDC, void*, const RECT*,
                               const RECT*, const RECT*, const RECT*);

using DispatchMessageW_t = decltype(&DispatchMessageW);
using CreateWindowExW_t = decltype(&CreateWindowExW);
using IntFn = int (*)(void*);
using ParentFn = void* (*)(void*);
using ContentStringFn = PCWSTR (*)(void*, void**);
using ReleaseValueFn = void (*)(void*);
using StrToIDFn = ATOM (WINAPI *)(PCWSTR);
using FindDescendentFn = void* (*)(void*, ATOM);
HMODULE g_dui = nullptr;
PaintBackground_t* g_backgroundOriginal = nullptr;
DispatchMessageW_t g_dispatchOriginal = nullptr;
CreateWindowExW_t g_createWindowOriginal = nullptr;
// Until a real CabinetWClass folder view exists, only a one-shot window-
// creation sentinel is hooked. No DirectUI, GDI, registry or message hook yet.
SRWLOCK g_activationLock = SRWLOCK_INIT;
// Allocated only on the first folder activation. Windhawk must not unload
// this image until its last threadpool callback has actually returned.
PTP_WORK g_activationWork = nullptr;
bool g_activationReady = false;
bool g_activationQueued = false;
bool g_activationInstalled = false;
bool g_activationAttempted = false;
bool g_settingsDirtyDuringActivation = false;
std::atomic<bool> g_stopping{false};
IntFn g_role = nullptr, g_index = nullptr, g_dpi = nullptr;
ParentFn g_parent = nullptr;
// Optional text accessors provide a file identity independent of the
// recycled DirectUI row's current viewport-relative index.
ContentStringFn g_getAccName = nullptr, g_getContentString = nullptr;
ReleaseValueFn g_releaseValue = nullptr;
StrToIDFn g_strToID = nullptr;
FindDescendentFn g_findDescendent = nullptr;
std::atomic<ATOM> g_fileNameAtom{0};
// Avoid calling StrToID on every row while Explorer has not yet registered
// FileName. A late registration is picked up within one short probe interval.
std::atomic<DWORD> g_lastFileNameAtomProbe{0};
HBRUSH g_dcBrush = nullptr;
std::atomic<bool> g_enabled{false};
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
// The original 1x1 source is retained for fast square/zero-radius rows.
// Shared corner bitmaps below are only used when a stripe is rounded.
SRWLOCK g_blendLock = SRWLOCK_INIT;

HDC g_blendDC = nullptr;

HBITMAP g_blendBitmap = nullptr;

HGDIOBJ g_blendOldBitmap = nullptr;
DWORD* g_blendBits = nullptr;

DWORD g_blendCachedPixel = 0;
// Each cache entry is keyed by its physical height, radius, AND exact
// premultiplied color. A theme change never reuses stale colored caps.
// GDI source DC blits are serialized; misses rasterize outside the lock.
struct RoundedStripeShape {
    int height = 0;
    int twiceRadius = 0;
    int capWidth = 0;
    DWORD pixel = 0;
    size_t bytes = 0;
    HDC dc = nullptr;
    HBITMAP bitmap = nullptr;
    HGDIOBJ previous = nullptr;
    RoundedStripeShape() = default;
    RoundedStripeShape(const RoundedStripeShape&) = delete;
    RoundedStripeShape& operator=(const RoundedStripeShape&) = delete;
    ~RoundedStripeShape() {
        if (dc && previous && previous != HGDI_ERROR)
            SelectObject(dc, previous);
        if (bitmap) DeleteObject(bitmap);
        if (dc) DeleteDC(dc);
    }
};
constexpr size_t kRoundedCacheSlots = 24;
SRWLOCK g_roundedLock = SRWLOCK_INIT;
std::array<std::unique_ptr<RoundedStripeShape>, kRoundedCacheSlots> g_roundedShapes{};
size_t g_nextRoundedSlot = 0;
constexpr size_t kRoundedCacheByteBudget = 4 * 1024 * 1024;
size_t g_roundedCachedBytes = 0;
// Allocation/rasterization happens lazily and can fail under memory pressure.
// Permit only one expensive cache fill at a time (without holding the cache
// lock), and throttle failures so a huge list cannot retry once per row.
std::atomic_flag g_shapeBuildInProgress = ATOMIC_FLAG_INIT;
std::atomic<DWORD> g_retryShapeAfter{0};
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
// The registry notification is manual-reset, so there is no need to poll the
// event separately for every row in one Explorer paint burst.
std::atomic<DWORD> g_lastThemeEventProbe{0};
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
// These are only used by the Explorer UI thread which dispatches/paints a view.
// No per-item message processing or additional window hooks are required.
thread_local unsigned g_paintPass = 1;
thread_local DWORD g_lastScrollMessage = 0;
thread_local bool g_scrollbarDrag = false;
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
    const void* listScope;
    const void* scopeContext;
};
// Per-thread row geometry history.
thread_local RowWidthHistory g_widthHistory[kWidthHistorySlots]{};
// Per-thread ancestry cache avoids re-walking a shared parent per row.
thread_local ParentTreeHistory g_treeHistory[kParentHistorySlots]{};
bool WidthChangedWhilePressed(void* item, int index, const RECT& row,
                              DWORD now, unsigned generation) {
    const auto key = reinterpret_cast<std::uintptr_t>(item);
    RowWidthHistory& slot = g_widthHistory[((key >> 4) ^ (key >> 13)) & (kWidthHistorySlots - 1)];
    bool changed = false;
    if (slot.item == item && slot.generation == generation &&
        slot.index == index && slot.left == row.left &&
        slot.top == row.top && slot.bottom == row.bottom &&
        now - slot.tick < kResizeHistoryMs) {
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
    g_radius.store(std::clamp(Wh_GetIntSetting(L"cornerRadius"), -1, 30),
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
    g_lastThemeEventProbe.store(0, std::memory_order_relaxed);
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
    const bool fallbackDue = !previous ||
        now - previous >= kThemeFallbackRefreshMs;
    if (!forced && !fallbackDue) {
        // A cheap atomic gate replaces a kernel event check on EVERY stripe.
        // A manual-reset notification remains signaled until consumed below.
        if (!g_personalizeEvent) return;
        DWORD last = g_lastThemeEventProbe.load(std::memory_order_relaxed);
        if (last && now - last < kThemeEventProbeMs) return;
        if (!g_lastThemeEventProbe.compare_exchange_strong(
                last, now, std::memory_order_relaxed)) return;
        if (WaitForSingleObject(g_personalizeEvent, 0) != WAIT_OBJECT_0)
            return;
    }
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
    if (!forced && !changed && previous && now - previous < kThemeFallbackRefreshMs) {
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
           width <= std::numeric_limits<int>::max() &&
           height <= kMaxRoundedBitmapHeight &&
           width >= g_minWidth.load(std::memory_order_relaxed);
}

int RowDpi(void* element) {
    const int dpi = g_dpi(element);
    // Guard a bad/unsupported DirectUI return rather than mis-draw.
    return dpi >= kMinSupportedDpi && dpi <= kMaxSupportedDpi ? dpi : kReferenceDpi;
}

bool IsCompactListItemHeight(const RECT& row, int dpi) {
    const auto height = static_cast<int64_t>(row.bottom) - row.top;
    // Retain a compact-item height guard (19..52 at 96 DPI) so large
    // icon/thumbnail tiles aren't treated as horizontal list rows.
    // The range scales with the item's actual DPI.
    // Integer comparisons avoid MulDiv calls and rounding ambiguity.
    const auto scaled = height * kReferenceDpi;
    return scaled >= kCompactRowMinHeight * dpi &&
           scaled <= kCompactRowMaxHeight * dpi;
}
// A stable list ancestor separates independent Explorer views and tabs.
// The parent cache is shared by the tree filter and parity resolver, avoiding
// another DirectUI ancestry walk per painted row.
bool IsInNavigationTree(void* element, DWORD now, unsigned generation,
                        const void** listScope, const void** scopeContext) {
    void* first = g_parent(element);
    if (!first) {
        *listScope = nullptr;
        *scopeContext = nullptr;
        return false;
    }
    const auto key = reinterpret_cast<std::uintptr_t>(first);
    ParentTreeHistory& slot = g_treeHistory[((key >> 4) ^ (key >> 12)) & (kParentHistorySlots - 1)];
    if (slot.parent == first && slot.generation == generation &&
        now - slot.tick < kParentTreeCacheMs) {
        *listScope = slot.listScope;
        *scopeContext = slot.scopeContext;
        return slot.inTree;
    }
    bool inTree = false;
    const void* owner = first;
    const void* ownerParent = nullptr;
    void* p = first;
    for (int i = 0; p && i < kMaxAncestryDepth; ++i) {
        const int role = g_role(p);
        if (role == ROLE_SYSTEM_OUTLINE || role == ROLE_SYSTEM_OUTLINEITEM) {
            inTree = true;
            break;
        }
        if (role == ROLE_SYSTEM_LIST) {
            owner = p;
            // Only one extra parent lookup per 250 ms ancestry-cache miss.
            // A reallocated list at the same address often has a new owner.
            ownerParent = g_parent(p);
            break;
        }
        void* next = g_parent(p);
        if (p == first) ownerParent = next;
        if (next == p) break;
        p = next;
    }
    slot = {first, now, generation, inTree, owner, ownerParent};
    *listScope = owner;
    *scopeContext = ownerParent;
    return inTree;
}

// GetIndex is viewport-relative in some Explorer builds. Reusing the raw
// index while scrolling flips the color of a file; blindly retaining each
// file's old parity breaks alternation after sorting/inserting/deleting rows.
// Use the scope's current index-to-parity bias as the authority for ordinary
// paints. Only scrolling is allowed to re-anchor that bias from a cached file.
// A visible duplicate name is marked ambiguous instead of being forced to
// share the same cached parity. No filenames, paths or paint-time allocations.
struct StableStripeEntry {
    const void* scope = nullptr;
    std::uint64_t nameHash = 0;
    unsigned generation = 0;
    unsigned scopeSerial = 0;
    DWORD lastUsed = 0;
    unsigned seenPaint = 0;
    int seenIndex = -1;
    BYTE parity = 0;
    bool ambiguous = false;
};
struct StripeScopeBias {
    const void* scope = nullptr;
    const void* context = nullptr;
    unsigned generation = 0;
    unsigned serial = 0;
    std::uint64_t viewEpoch = 0;
    DWORD lastUsed = 0;
    DWORD threadId = 0;
    unsigned paintPass = 0;
    unsigned paintSerial = 0;
    int lastIndex = -1;
    BYTE bias = 0;
    bool anchoredThisPaint = false;
};
// Keep the 32K-entry, 4-way cache for large directories. The scope serial
// prevents an evicted scope's old entries from matching a newly reused slot.
constexpr size_t kStripeCacheSets = 8192;
constexpr size_t kStripeWays = 4;
constexpr size_t kScopeSlots = 64;
static_assert((kStripeCacheSets & (kStripeCacheSets - 1)) == 0);
SRWLOCK g_stripeIdentityLock = SRWLOCK_INIT;
std::array<StableStripeEntry, kStripeCacheSets * kStripeWays> g_stripeEntries{};
std::array<StripeScopeBias, kScopeSlots> g_stripeBiases{};
unsigned g_nextScopeSerial = 0; // Protected by g_stripeIdentityLock.
std::atomic<int> g_identityFallbackSamples{0};
// The cached index is always revalidated under the global identity lock.
thread_local size_t g_recentBiasSlot = kScopeSlots;

// The folder-view HWND has its own epoch, so different Explorer tabs on the
// same UI thread don't invalidate each other's stripe identities. Window
// properties disappear with the HWND and handle address reuse safely.
// No Shell COM queries or paint-time allocations are necessary.
constexpr PCWSTR kFolderViewEpochProperty =
    L"Windhawk.ExplorerZebraStripes.FolderViewEpoch";
std::atomic<std::uintptr_t> g_nextFolderViewEpoch{0};
thread_local std::uintptr_t g_fallbackViewEpoch = 0;
thread_local std::uintptr_t g_paintViewEpoch = 0;

std::uintptr_t NextFolderViewEpoch() {
    return g_nextFolderViewEpoch.fetch_add(1, std::memory_order_relaxed) + 1;
}

std::uintptr_t FallbackFolderViewEpoch() {
    if (!g_fallbackViewEpoch)
        g_fallbackViewEpoch = NextFolderViewEpoch();
    return g_fallbackViewEpoch;
}

std::uintptr_t EpochForFolderView(HWND hwnd) {
    if (!hwnd) return 0;
    auto epoch = reinterpret_cast<std::uintptr_t>(
        GetPropW(hwnd, kFolderViewEpochProperty));
    if (epoch) return epoch;
    epoch = NextFolderViewEpoch();
    return SetPropW(hwnd, kFolderViewEpochProperty,
                    reinterpret_cast<HANDLE>(epoch)) ? epoch : 0;
}

// Called only for WM_PAINT, never once per item. The paint callback can read
// g_paintViewEpoch directly without looking at its potentially offscreen HDC.
HWND FolderViewAncestor(HWND hwnd) {
    HWND root = hwnd ? GetAncestor(hwnd, GA_ROOT) : nullptr;
    wchar_t rootClass[64]{};
    if (!root || !GetClassNameW(root, rootClass, ARRAYSIZE(rootClass)) ||
        _wcsicmp(rootClass, L"CabinetWClass") != 0)
        return nullptr;
    for (int i = 0; hwnd && i < kMaxWindowAncestryDepth;
         ++i, hwnd = GetParent(hwnd)) {
        wchar_t cls[64]{};
        if (GetClassNameW(hwnd, cls, ARRAYSIZE(cls)) &&
            _wcsicmp(cls, L"SHELLDLL_DefView") == 0)
            return hwnd;
    }
    return nullptr;
}

std::uint64_t HashFileName(PCWSTR name) {
    if (!name || !*name) return 0;
    // FNV-1a over UTF-16 code units (not a random/process-specific hash).
    std::uint64_t hash = 14695981039346656037ULL;
    int length = 0;
    for (; length < kMaxIdentityCodeUnits && name[length]; ++length) {
        hash ^= static_cast<unsigned short>(name[length]);
        hash *= 1099511628211ULL;
    }
    // Exclude incomplete/truncated identities and empty generic properties.
    return length && length < kMaxIdentityCodeUnits ? hash : 0;
}

// A Value returned by DirectUI is borrowed only for the duration of this
// function, and is released before the row painter proceeds.
std::uint64_t HashElementName(void* element, ContentStringFn getter) {
    if (!getter || !g_releaseValue || !element) return 0;
    void* value = nullptr;
    PCWSTR name = getter(element, &value);
    std::uint64_t hash = HashFileName(name);
    if (value) g_releaseValue(value);
    return hash;
}

std::uint64_t GetRowIdentity(void* element, DWORD now) {
    // DirectUI's FileName child is preferred over the parent row's accessible
    // name: it identifies the file better than a state-dependent row label.
    if (g_findDescendent && g_strToID) {
        ATOM atom = g_fileNameAtom.load(std::memory_order_acquire);
        if (!atom) {
            DWORD last = g_lastFileNameAtomProbe.load(std::memory_order_relaxed);
            if ((!last || now - last >= kFileNameAtomProbeMs) &&
                g_lastFileNameAtomProbe.compare_exchange_strong(
                    last, now, std::memory_order_relaxed)) {
                atom = g_strToID(L"FileName");
                if (atom)
                    g_fileNameAtom.store(atom, std::memory_order_release);
            }
        }
        if (atom) {
            void* filename = g_findDescendent(element, atom);
            if (filename) {
                std::uint64_t hash = HashElementName(filename, g_getContentString);
                if (!hash) hash = HashElementName(filename, g_getAccName);
                if (hash) return hash;
            }
        }
    }
    return HashElementName(element, g_getAccName);
}

// Call only after geometry/tree checks; both striped and unstriped rows are
// needed to detect changes in order. A normal repaint adjusts stale entries
// to the current order immediately. During scrolling, cached items establish
// the same offset for the entire paint batch, preserving per-file stripes.
int StableRowParity(void* element, const void* scope,
                    const void* scopeContext, int visualIndex,
                    DWORD now, unsigned generation) {
    if (visualIndex < 0) return -1;
    const std::uint64_t name = scope ? GetRowIdentity(element, now) : 0;
    if (!name || !scope) {
        if (g_diagnostics.load(std::memory_order_relaxed)) {
            const int n = g_identityFallbackSamples.fetch_add(
                1, std::memory_order_relaxed);
            if (n < 8) Wh_Log(L"Zebra: stable file identity unavailable "
                              L"(sample=%d, view=%d, index=%d); raw-index fallback",
                              n, scope != nullptr, visualIndex);
        }
        if (!scope) return visualIndex & 1;
        // With no name, use the current view bias instead of an unrelated
        // raw index. Other named rows still establish scroll continuity.
    }
    // WM_PAINT supplies the exact view. Internal paints use the last view
    // observed on this UI thread, without any per-row Window or Shell call.
    const std::uint64_t viewEpoch = g_paintViewEpoch ?
        g_paintViewEpoch : FallbackFolderViewEpoch();
    AcquireSRWLockExclusive(&g_stripeIdentityLock);
    StripeScopeBias* bias = nullptr;
    if (g_recentBiasSlot < kScopeSlots) {
        auto& candidate = g_stripeBiases[g_recentBiasSlot];
        if (candidate.scope == scope && candidate.context == scopeContext &&
            candidate.generation == generation &&
            candidate.viewEpoch == viewEpoch)
            bias = &candidate;
    }
    if (!bias) {
        StripeScopeBias* victim = nullptr;
        for (auto& b : g_stripeBiases) {
            if (b.scope == scope && b.context == scopeContext &&
                b.generation == generation && b.viewEpoch == viewEpoch) {
                bias = &b;
                break;
            }
            if (!victim || !b.scope || b.generation != generation ||
                (b.scope == scope &&
                 (b.context != scopeContext || b.viewEpoch != viewEpoch)) ||
                (victim->scope && victim->generation == generation &&
                 static_cast<DWORD>(now - b.lastUsed) >
                 static_cast<DWORD>(now - victim->lastUsed)))
                victim = &b;
        }
        if (!bias) {
            bias = victim;
            // A new scope starts at the view's normal odd/even index phase.
            // The next observed scroll can learn its viewport-relative offset.
            const unsigned serial = ++g_nextScopeSerial ? g_nextScopeSerial
                                                         : ++g_nextScopeSerial;
            *bias = {};
            bias->scope = scope;
            bias->context = scopeContext;
            bias->generation = generation;
            bias->serial = serial;
            bias->viewEpoch = viewEpoch;
        }
        g_recentBiasSlot = static_cast<size_t>(bias - g_stripeBiases.data());
    }
    const DWORD previous = bias->lastUsed;
    const DWORD threadId = GetCurrentThreadId();
    const bool newPaint = bias->threadId != threadId ||
        bias->paintPass != g_paintPass || !previous ||
        now - previous >= kStripePaintIdleGapMs ||
        (bias->lastIndex >= kStripePaintIndexWrap &&
         visualIndex <= bias->lastIndex - kStripePaintIndexWrap);
    if (newPaint) {
        bias->threadId = threadId;
        bias->paintPass = g_paintPass;
        ++bias->paintSerial;
        if (!bias->paintSerial) ++bias->paintSerial;
        bias->anchoredThisPaint = false;
    }
    bias->lastUsed = now;
    bias->lastIndex = visualIndex;
    if (!name) {
        const int parity = (visualIndex & 1) ^ bias->bias;
        ReleaseSRWLockExclusive(&g_stripeIdentityLock);
        return parity;
    }
    const std::uintptr_t s = reinterpret_cast<std::uintptr_t>(scope);
    const size_t set = static_cast<size_t>(
        (name ^ (name >> 32) ^ (s >> 4) ^ (s >> 17)) & (kStripeCacheSets - 1));
    StableStripeEntry* entry = nullptr;
    StableStripeEntry* victim = nullptr;
    for (size_t way = 0; way < kStripeWays; ++way) {
        auto& e = g_stripeEntries[set * kStripeWays + way];
        if (e.scope == scope && e.nameHash == name &&
            e.generation == generation && e.scopeSerial == bias->serial) {
            entry = &e;
            break;
        }
        if (!victim || e.generation != generation || !e.scope ||
            (e.scope == scope && e.scopeSerial != bias->serial) ||
            (victim->scope && victim->generation == generation &&
             static_cast<DWORD>(now - e.lastUsed) >
             static_cast<DWORD>(now - victim->lastUsed)))
            victim = &e;
    }
    // The scroll signal comes from our existing DispatchMessageW hook. Only
    // during a scroll can a uniformly shifted set of cached indices change
    // the viewport bias. Otherwise sorting, deletion and insertion must
    // *correct* the old assignments rather than endlessly moving the bias.
    const bool scrolling = g_lastScrollMessage &&
        now - g_lastScrollMessage < kScrollPaintGraceMs;
    if (entry && !entry->ambiguous && scrolling &&
        !bias->anchoredThisPaint && entry->seenPaint != bias->paintSerial) {
        bias->bias = static_cast<BYTE>(entry->parity ^ (visualIndex & 1));
        bias->anchoredThisPaint = true;
    }
    const BYTE parity = static_cast<BYTE>((visualIndex & 1) ^ bias->bias);
    if (entry) {
        // Two different row positions displaying the same name during one
        // paint indicate ambiguous search results. Do not let those distinct
        // files repeatedly flip one shared cache entry's parity.
        if (entry->seenPaint == bias->paintSerial &&
            entry->seenIndex != visualIndex)
            entry->ambiguous = true;
        entry->parity = parity;
        entry->lastUsed = now;
        entry->seenPaint = bias->paintSerial;
        entry->seenIndex = visualIndex;
    } else {
        *victim = {scope, name, generation, bias->serial, now,
                   bias->paintSerial, visualIndex, parity, false};
    }
    ReleaseSRWLockExclusive(&g_stripeIdentityLock);
    return parity;
}

// The selection mod's 4x4 coverage rasterization, specialized to a single
// color with no border. Source layout: left cap | 1px solid center | right cap.
// The source is intentionally independent of stripe WIDTH. Thus a resized row
// needs no new allocation or rasterization, only three AlphaBlend transfers.
std::unique_ptr<RoundedStripeShape> CreateRoundedShape(int height,
                                                       int twiceRadius,
                                                       DWORD pixel) {
    if (height <= 0 || height > kMaxRoundedBitmapHeight || twiceRadius <= 0 ||
        twiceRadius > height) return nullptr;
    // Never propagate std::bad_alloc from a hooked Explorer paint callback.
    std::unique_ptr<RoundedStripeShape> shape(
        new (std::nothrow) RoundedStripeShape());
    if (!shape) return nullptr;
    shape->height = height;
    shape->twiceRadius = twiceRadius;
    shape->capWidth = std::max(1, (twiceRadius + 1) / 2);
    shape->pixel = pixel;
    const int width = 2 * shape->capWidth + 1;
    shape->bytes = static_cast<size_t>(width) * height * sizeof(DWORD);
    // The cache can outlive the Explorer UI thread that first paints a row.
    // A NULL parent DC would make the memory DC die with that thread.
    HDC screen = GetDC(nullptr);
    if (!screen) return nullptr;
    shape->dc = CreateCompatibleDC(screen);
    ReleaseDC(nullptr, screen);
    if (!shape->dc) return nullptr;
    BITMAPINFO info{};
    info.bmiHeader.biSize = sizeof(info.bmiHeader);
    info.bmiHeader.biWidth = width;
    info.bmiHeader.biHeight = -height;
    info.bmiHeader.biPlanes = 1;
    info.bmiHeader.biBitCount = 32;
    info.bmiHeader.biCompression = BI_RGB;
    void* bits = nullptr;
    shape->bitmap = CreateDIBSection(shape->dc, &info, DIB_RGB_COLORS,
                                     &bits, nullptr, 0);
    if (!shape->bitmap || !bits) return nullptr;
    shape->previous = SelectObject(shape->dc, shape->bitmap);
    if (!shape->previous || shape->previous == HGDI_ERROR) return nullptr;

    // Same signed-distance rounded-rectangle test as the selection mod,
    // simplified from sqrt(distance) <= radius to squared-distance <= r^2.
    constexpr int samples = 4;
    constexpr int total = samples * samples;
    const float radius = twiceRadius * 0.5f;
    const float radiusSquared = radius * radius;
    const float innerRadiusSquared = std::max(radius - 1.0f, 0.0f) *
                                     std::max(radius - 1.0f, 0.0f);
    const float outerRadiusSquared = (radius + 1.0f) * (radius + 1.0f);
    DWORD* const pixels = static_cast<DWORD*>(bits);
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int covered = 0;
            if (x == shape->capWidth) {
                covered = total;
            } else {
                const bool rightCap = x > shape->capWidth;
                const float centerX = x + 0.5f;
                const float centerY = y + 0.5f;
                const float edgeX = rightCap ? width - centerX : centerX;
                const float edgeY = std::min(centerY, static_cast<float>(height) - centerY);
                const float dx = std::max(radius - edgeX, 0.0f);
                const float dy = std::max(radius - edgeY, 0.0f);
                const float distanceSquared = dx * dx + dy * dy;
                // Up to 0.53 px separates a 4x4 sample from the pixel center.
                // A conservative 1 px margin makes these fast classifications
                // identical to doing all sixteen subpixel tests.
                if (radius >= 1.0f && distanceSquared <= innerRadiusSquared) {
                    covered = total;
                } else if (distanceSquared < outerRadiusSquared) {
                    for (int sy = 0; sy < samples; ++sy) {
                    const float yy = y + (sy + 0.5f) / samples;
                    const float edgeY = std::min(yy, static_cast<float>(height) - yy);
                    const float dy = std::max(radius - edgeY, 0.0f);
                    for (int sx = 0; sx < samples; ++sx) {
                        const float xx = x + (sx + 0.5f) / samples;
                        const float edgeX = rightCap ? width - xx : xx;
                        const float dx = std::max(radius - edgeX, 0.0f);
                        if (dx * dx + dy * dy <= radiusSquared) ++covered;
                    }
                    }
                }
            }
            // The global pixel is already premultiplied BGRA. Multiplying
            // *every* channel by coverage keeps the cache correct for
            // partial/fully opaque stripes and semi-transparent edge pixels.
            const auto shade = [covered](DWORD channel) -> DWORD {
                return (channel * covered + total / 2) / total;
            };
            pixels[y * width + x] =
                (shade((pixel >> 24) & 255) << 24) |
                (shade((pixel >> 16) & 255) << 16) |
                (shade((pixel >> 8) & 255) << 8) |
                 shade(pixel & 255);
        }
    }
    return shape;
}
RoundedStripeShape* FindRoundedShape(int height, int twiceRadius, DWORD pixel) {
    for (const auto& entry : g_roundedShapes)
        if (entry && entry->height == height &&
            entry->twiceRadius == twiceRadius && entry->pixel == pixel)
            return entry.get();
    return nullptr;
}
// For unusually wide (including corrupted) item rectangles, determine which
// horizontal part of the row is visible. The center source is one pixel wide;
// cropping its destination doesn't change the pattern. Caps are 1:1 mapped
// and can be cropped with the same source-pixel offset.
struct StripeClip {
    int left;
    int right;
};
StripeClip GetStripeClip(HDC dc, const RECT& rect) {
    StripeClip clip{static_cast<int>(rect.left), static_cast<int>(rect.right)};
    if (static_cast<std::int64_t>(rect.right) - rect.left <=
        kMaxUnclippedBlendWidth) return clip;
    RECT visible{};
    const int type = GetClipBox(dc, &visible);
    if (type == ERROR || type == NULLREGION) return {static_cast<int>(rect.left), static_cast<int>(rect.left)};
    clip.left = std::max(clip.left, static_cast<int>(visible.left));
    clip.right = std::min(clip.right, static_cast<int>(visible.right));
    return clip;
}

// Use only the left cap and the stretched center while columns are being
// resized: this keeps the right edge flat without regenerating the source.
// A cache miss is built outside the global shape lock, preventing other
// Explorer UI threads from blocking on up to ~millions of AA sample tests.
bool BlendRoundedStripe(HDC dc, const RECT& dst, int twiceRadius,
                        DWORD pixel, bool flatRight, DWORD now) {
    const int height = dst.bottom - dst.top;
    const int width = dst.right - dst.left;
    AcquireSRWLockExclusive(&g_roundedLock);
    RoundedStripeShape* shape = FindRoundedShape(height, twiceRadius, pixel);
    if (!shape) {
        ReleaseSRWLockExclusive(&g_roundedLock);
        const DWORD retry = g_retryShapeAfter.load(std::memory_order_relaxed);
        const bool canRetry = !retry || static_cast<std::int32_t>(now - retry) >= 0;
        if (canRetry && !g_shapeBuildInProgress.test_and_set(
                std::memory_order_acquire)) {
            // Other threads continue painting with the cheap solid fallback;
            // they never wait for the slow first-time rasterization.
            auto created = CreateRoundedShape(height, twiceRadius, pixel);
            if (created) {
                AcquireSRWLockExclusive(&g_roundedLock);
                if (!FindRoundedShape(height, twiceRadius, pixel) &&
                    g_enabled.load(std::memory_order_relaxed)) {
                    // Cap *bytes*, not just the number of shapes: at extreme
                    // DPI a single pill can be hundreds of KiB of GDI memory.
                    while (g_roundedCachedBytes + created->bytes >
                           kRoundedCacheByteBudget) {
                        auto& evicted = g_roundedShapes[g_nextRoundedSlot];
                        if (evicted) {
                            g_roundedCachedBytes -= evicted->bytes;
                            evicted.reset();
                        }
                        g_nextRoundedSlot =
                            (g_nextRoundedSlot + 1) % kRoundedCacheSlots;
                    }
                    auto& slot = g_roundedShapes[g_nextRoundedSlot];
                    if (slot) g_roundedCachedBytes -= slot->bytes;
                    slot = std::move(created);
                    g_roundedCachedBytes += slot->bytes;
                    g_nextRoundedSlot =
                        (g_nextRoundedSlot + 1) % kRoundedCacheSlots;
                }
                ReleaseSRWLockExclusive(&g_roundedLock);
                g_retryShapeAfter.store(0, std::memory_order_relaxed);
            } else {
                g_retryShapeAfter.store(now + kFailedShapeRetryMs,
                                        std::memory_order_relaxed);
            }
            g_shapeBuildInProgress.clear(std::memory_order_release);
        }
        AcquireSRWLockExclusive(&g_roundedLock);
        shape = FindRoundedShape(height, twiceRadius, pixel);
    }
    if (!shape) {
        ReleaseSRWLockExclusive(&g_roundedLock);
        return false;
    }
    const int leftWidth = std::min(shape->capWidth, (width + 1) / 2);
    const int rightWidth = flatRight ? 0 : std::min(shape->capWidth,
                                                    width - leftWidth);
    const int middleWidth = width - leftWidth - rightWidth;
    const int sourceWidth = 2 * shape->capWidth + 1;
    const StripeClip clip = GetStripeClip(dc, dst);
    const BLENDFUNCTION blend{AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
    bool drew = false;
    auto patch = [&](int destinationX, int destinationWidth,
                     int sourceX, bool solidMiddle) {
        if (destinationWidth <= 0) return;
        const int left = std::max(destinationX, clip.left);
        const int right = std::min(destinationX + destinationWidth, clip.right);
        if (right <= left) return;
        const int sourceOffset = solidMiddle ? 0 : left - destinationX;
        drew = !!GdiAlphaBlend(dc, left, dst.top, right - left, height,
                               shape->dc, sourceX + sourceOffset, 0,
                               solidMiddle ? 1 : right - left, height,
                               blend) || drew;
    };
    patch(dst.left, leftWidth, 0, false);
    patch(dst.left + leftWidth, middleWidth, shape->capWidth, true);
    patch(dst.right - rightWidth, rightWidth, sourceWidth - rightWidth, false);
    ReleaseSRWLockExclusive(&g_roundedLock);
    // An entirely offscreen row is not an allocation/drawing failure.
    return drew || clip.right <= clip.left;
}
void FreeRoundedShapes() {
    AcquireSRWLockExclusive(&g_roundedLock);
    for (auto& entry : g_roundedShapes) entry.reset();
    g_nextRoundedSlot = 0;
    g_roundedCachedBytes = 0;
    g_retryShapeAfter.store(0, std::memory_order_relaxed);
    ReleaseSRWLockExclusive(&g_roundedLock);
}
// A guaranteed allocation-free fallback for an uncached shape or GDI failure.
// Preserve the muted alpha even when no rounded bitmap can be allocated.
bool DrawSolidStripe(HDC dc, const RECT& dst, DWORD pixel) {
    const BYTE alpha = static_cast<BYTE>(pixel >> 24);
    if (!alpha) return false;
    if (alpha == 255) {
        const COLORREF fill = RGB((pixel >> 16) & 255,
                                  (pixel >> 8) & 255, pixel & 255);
        const COLORREF old = SetDCBrushColor(dc, fill);
        if (old == CLR_INVALID) return false;
        const bool painted = !!FillRect(dc, &dst, g_dcBrush);
        SetDCBrushColor(dc, old);
        return painted;
    }
    const StripeClip clip = GetStripeClip(dc, dst);
    if (clip.right <= clip.left) return true;
    AcquireSRWLockExclusive(&g_blendLock);
    if (pixel != g_blendCachedPixel) {
        *g_blendBits = pixel;
        g_blendCachedPixel = pixel;
    }
    const BLENDFUNCTION blend{AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
    const bool painted = !!GdiAlphaBlend(dc, clip.left, dst.top,
                                         clip.right - clip.left, dst.bottom - dst.top,
                                         g_blendDC, 0, 0, 1, 1, blend);
    ReleaseSRWLockExclusive(&g_blendLock);
    return painted;
}
// Rounded stripes use the AA cache, while square stripes and low-memory
// recovery use the existing reusable 1x1 premultiplied DIB.
bool DrawStripe(HDC dc, const RECT& nativeRow, int dpi, DWORD now,
                bool flatRight) {
    const int horizontal = g_horizontal.load(std::memory_order_relaxed);
    const int vertical = g_vertical.load(std::memory_order_relaxed);
    const int requestedRadius = g_radius.load(std::memory_order_relaxed);
    const int hx = dpi == kReferenceDpi ? horizontal : MulDiv(horizontal, dpi, kReferenceDpi);
    const int vy = dpi == kReferenceDpi ? vertical : MulDiv(vertical, dpi, kReferenceDpi);
    // Widen before adding/subtracting padding: DirectUI may return odd geometry
    // during virtualization. Never overflow a signed RECT coordinate/width.
    const std::int64_t left = static_cast<std::int64_t>(nativeRow.left) + hx;
    const std::int64_t right = static_cast<std::int64_t>(nativeRow.right) - hx;
    const std::int64_t top = static_cast<std::int64_t>(nativeRow.top) + vy;
    const std::int64_t bottom = static_cast<std::int64_t>(nativeRow.bottom) - vy;
    if (left < std::numeric_limits<std::int32_t>::min() ||
        right > std::numeric_limits<std::int32_t>::max() ||
        top < std::numeric_limits<std::int32_t>::min() ||
        bottom > std::numeric_limits<std::int32_t>::max() ||
        right <= left || bottom <= top ||
        right - left > std::numeric_limits<int>::max() ||
        bottom - top > kMaxRoundedBitmapHeight) return false;
    const RECT dst{static_cast<LONG>(left), static_cast<LONG>(top),
                   static_cast<LONG>(right), static_cast<LONG>(bottom)};
    const int width = static_cast<int>(right - left);
    const int height = static_cast<int>(bottom - top);
    const int maxDiameter = std::min(width, height);
    const int scaledRadius = requestedRadius < 0 ? 0 :
        (dpi == kReferenceDpi ? requestedRadius : MulDiv(requestedRadius, dpi, kReferenceDpi));
    const int twiceRadius = requestedRadius == -1 ? maxDiameter :
        std::min(maxDiameter, 2 * scaledRadius);
    RefreshColorsIfDue(now);
    const DWORD pixel = g_paintPixel.load(std::memory_order_relaxed);
    if (!(pixel >> 24)) return false;
    if (twiceRadius && BlendRoundedStripe(dc, dst, twiceRadius, pixel,
                                          flatRight, now)) return true;
    return DrawSolidStripe(dc, dst, pixel);
}

void PaintBackgroundHook(void* element, HDC dc, void* value,
                         const RECT* rc, const RECT* b,
                         const RECT* c, const RECT* d) {
    // Run Explorer's complete themed background pipeline first. This also
    // includes DrawThemeBackgroundEx overrides by other mods (e.g. the accent
    // selection mod), regardless of hook registration order.
    g_backgroundOriginal(element, dc, value, rc, b, c, d);
    if (!g_enabled.load(std::memory_order_relaxed) || !element || !dc ||
        !HasPossibleRowGeometry(rc) || g_role(element) != ROLE_SYSTEM_LISTITEM)
        return;
    // A configured 0% alpha should cost virtually nothing. Still perform the
    // throttled palette refresh, so switching between light/dark themes can
    // reactivate stripes without requiring a settings reload.
    if ((g_paintPixel.load(std::memory_order_relaxed) >> 24) == 0) {
        RefreshColorsIfDue(GetTickCount());
        if ((g_paintPixel.load(std::memory_order_relaxed) >> 24) == 0) return;
    }
    const int index = g_index(element);
    // Background tint, not an interaction decoration. Always draw on every
    // matching parity row, including hovered, selected, keyboard-focused and
    // selected+hovered items. The accent-selection mod intercepts the themed
    // backgrounds inside g_backgroundOriginal; our tint is composed AFTER it,
    // but still before Explorer draws the item's foreground content.
    // Never query selection or mouse focus here: both are transient and were
    // the reason alternating stripes vanished while interacting with items.
    // Query DPI once for both row classification and drawing. This makes
    // the row filter work on mixed-DPI systems without an extra API call.
    const int dpi = RowDpi(element);
    if (!IsCompactListItemHeight(*rc, dpi)) return;
    const DWORD now = GetTickCount();
    const unsigned generation = g_generation.load(std::memory_order_relaxed);
    SyncDragGeneration(generation);
    const void* listScope = nullptr;
    const void* scopeContext = nullptr;
    if (IsInNavigationTree(element, now, generation, &listScope,
                           &scopeContext)) return;
    const int stableParity = StableRowParity(element, listScope, scopeContext,
                                             index, now, generation);
    if (stableParity < 0 ||
        stableParity != g_parity.load(std::memory_order_relaxed)) return;
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
                Wh_Log(L"Zebra: resize[%d] row=%d width=%ld; flat right cap active",
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
    const std::uintptr_t previousPaintViewEpoch = g_paintViewEpoch;
    if (msg && msg->message == WM_PAINT) {
        // One ancestor walk per posted paint, not per DirectUI row. This
        // distinguishes tabs even when they share an Explorer UI thread.
        const HWND view = FolderViewAncestor(msg->hwnd);
        g_paintViewEpoch = EpochForFolderView(view);
        if (g_paintViewEpoch)
            g_fallbackViewEpoch = g_paintViewEpoch;
    }
    // Delimit a batch before Explorer runs its paint callbacks, rather than
    // resetting the stripe bias for each row in the middle of the batch.
    if (msg) {
        const UINT message = msg->message;
        if (message == WM_PAINT) {
            ++g_paintPass;
            if (!g_paintPass) ++g_paintPass;
        } else if (message == WM_VSCROLL || message == WM_HSCROLL ||
                   message == WM_MOUSEWHEEL || message == WM_MOUSEHWHEEL ||
                   message == WM_POINTERWHEEL || message == WM_POINTERHWHEEL ||
                   message == WM_GESTURE ||
                   (message == WM_KEYDOWN &&
                    (msg->wParam == VK_UP || msg->wParam == VK_DOWN ||
                     msg->wParam == VK_NEXT || msg->wParam == VK_PRIOR ||
                     msg->wParam == VK_HOME || msg->wParam == VK_END))) {
            g_lastScrollMessage = GetTickCount();
        } else if (message == WM_LBUTTONDOWN) {
            // Clicking a column heading to sort must terminate scroll grace.
            g_lastScrollMessage = 0;
            g_scrollbarDrag = false;
            if (msg->hwnd) {
                wchar_t cls[32]{};
                if (GetClassNameW(msg->hwnd, cls, ARRAYSIZE(cls)) &&
                    _wcsicmp(cls, L"ScrollBar") == 0) {
                    g_scrollbarDrag = true;
                } else {
                    // Native scrollbar thumb messages may be sent rather than
                    // dispatched. Detect thumb drags once at mouse-down only.
                    SCROLLBARINFO info{};
                    info.cbSize = sizeof(info);
                    if ((GetScrollBarInfo(msg->hwnd, OBJID_VSCROLL, &info) &&
                         PtInRect(&info.rcScrollBar, msg->pt)) ||
                        (GetScrollBarInfo(msg->hwnd, OBJID_HSCROLL, &info) &&
                         PtInRect(&info.rcScrollBar, msg->pt)))
                        g_scrollbarDrag = true;
                }
            }
        } else if (message == WM_MOUSEMOVE && g_scrollbarDrag &&
                   (msg->wParam & MK_LBUTTON)) {
            g_lastScrollMessage = GetTickCount();
        } else if (message == WM_LBUTTONUP || message == WM_CAPTURECHANGED ||
                   message == WM_CANCELMODE) {
            g_scrollbarDrag = false;
        }
    }
    LRESULT result = g_dispatchOriginal(msg);
    g_paintViewEpoch = previousPaintViewEpoch;
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
                Wh_Log(L"Zebra: drag ended (msg=0x%x); one repaint", m);
            RepaintAfterDrag(msg->hwnd);
        }
    }
    return result;
}

template<typename T>
T Export(HMODULE module, const char* symbol) {
    return reinterpret_cast<T>(GetProcAddress(module, symbol));
}
// Folder windows are CabinetWClass; the desktop's SHELLDLL_DefView is not.
// Check the owning window so desktop initialization never activates the painter.
bool IsFolderDefView(HWND hwnd) {
    wchar_t cls[64]{};
    if (!hwnd || !GetClassNameW(hwnd, cls, ARRAYSIZE(cls)) ||
        _wcsicmp(cls, L"SHELLDLL_DefView") != 0)
        return false;
    HWND root = GetAncestor(hwnd, GA_ROOT);
    if (!root || root == hwnd ||
        !GetClassNameW(root, cls, ARRAYSIZE(cls)) ||
        _wcsicmp(cls, L"CabinetWClass") != 0)
        return false;
    DWORD pid = 0;
    GetWindowThreadProcessId(root, &pid);
    return pid == GetCurrentProcessId();
}

// Before activation this hook queues the first folder's painter. Afterwards
// it only stamps new SHELLDLL_DefView windows so cached row identities from
// a previous folder cannot be mistaken for those in a reused DirectUI tree.
// No synchronous hook installation or Shell lookup runs on the UI thread.
VOID CALLBACK ActivateOnWorker(PTP_CALLBACK_INSTANCE, void*, PTP_WORK);
void QueueFolderActivation() {
    AcquireSRWLockExclusive(&g_activationLock);
    if (g_activationReady && !g_activationQueued &&
        !g_activationAttempted && !g_stopping.load(std::memory_order_acquire)) {
        if (!g_activationWork)
            g_activationWork = CreateThreadpoolWork(ActivateOnWorker, nullptr, nullptr);
        if (g_activationWork) {
            g_activationQueued = true;
            SubmitThreadpoolWork(g_activationWork);
        }
        // If allocation failed, retry on the next folder creation.
    }
    ReleaseSRWLockExclusive(&g_activationLock);
}

HWND WINAPI CreateWindowExWHook(DWORD exStyle, LPCWSTR className,
                                LPCWSTR windowName, DWORD style,
                                int x, int y, int width, int height,
                                HWND parent, HMENU menu, HINSTANCE instance,
                                LPVOID param) {
    HWND hwnd = g_createWindowOriginal(exStyle, className, windowName,
                                       style, x, y, width, height,
                                       parent, menu, instance, param);
    // Catch the folder frame itself before its first list paint; a DefView
    // fallback covers folder views hosted by an already existing frame.
    // Atom-registered classes use the actual HWND class as the final check.
    if (hwnd && (IS_INTRESOURCE(className) ||
                 _wcsicmp(className, L"CabinetWClass") == 0 ||
                 _wcsicmp(className, L"SHELLDLL_DefView") == 0)) {
        wchar_t cls[64]{};
        if (GetClassNameW(hwnd, cls, ARRAYSIZE(cls)) &&
            _wcsicmp(cls, L"CabinetWClass") == 0) {
            DWORD pid = 0;
            GetWindowThreadProcessId(hwnd, &pid);
            if (pid == GetCurrentProcessId()) QueueFolderActivation();
        } else if (IsFolderDefView(hwnd)) {
            // New view, new identity; this property is attached to the HWND,
            // not all tabs on its owning Explorer UI thread.
            const auto epoch = EpochForFolderView(hwnd);
            if (epoch) g_fallbackViewEpoch = epoch;
            QueueFolderActivation();
        }
    }
    return hwnd;
}

BOOL CALLBACK FindFolderViewChild(HWND hwnd, LPARAM foundPtr) {
    if (IsFolderDefView(hwnd)) {
        *reinterpret_cast<bool*>(foundPtr) = true;
        return FALSE;
    }
    return TRUE;
}

BOOL CALLBACK FindFolderViewWindow(HWND hwnd, LPARAM foundPtr) {
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    if (pid != GetCurrentProcessId()) return TRUE;
    wchar_t cls[64]{};
    if (GetClassNameW(hwnd, cls, ARRAYSIZE(cls)) &&
        _wcsicmp(cls, L"CabinetWClass") == 0)
        EnumChildWindows(hwnd, FindFolderViewChild, foundPtr);
    return !*reinterpret_cast<bool*>(foundPtr);
}

bool AnyFolderViewExists() {
    bool found = false;
    EnumWindows(FindFolderViewWindow, reinterpret_cast<LPARAM>(&found));
    return found;
}

// Expensive initialization and both active hooks are deferred until a real
// Explorer folder view exists. The tracked work object is joined at unload;
// no permanent thread, timer, polling loop or window subclass is needed.
bool InstallFolderPainter() {
    if (g_stopping.load(std::memory_order_acquire)) return false;
    LoadSettings();
    InitPersonalizeNotification();
    RefreshColorsIfDue(GetTickCount(), true);

    // Hold the module while the installed DirectUI detour can call into it.
    g_dui = LoadLibraryExW(L"DUI70.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!g_dui) {
        Wh_Log(L"Zebra: unable to load DUI70.dll");
        return false;
    }
    g_role = Export<IntFn>(g_dui, "?GetAccRole@Element@DirectUI@@QEAAHXZ");
    g_index = Export<IntFn>(g_dui, "?GetIndex@Element@DirectUI@@QEAAHXZ");
    g_dpi = Export<IntFn>(g_dui, "?GetDPI@Element@DirectUI@@QEAAHXZ");
    g_parent = Export<ParentFn>(g_dui, "?GetParent@Element@DirectUI@@QEAAPEAV12@XZ");
    g_getAccName = Export<ContentStringFn>(g_dui,
        "?GetAccName@Element@DirectUI@@QEAAPEBGPEAPEAVValue@2@@Z");
    g_getContentString = Export<ContentStringFn>(g_dui,
        "?GetContentString@Element@DirectUI@@QEAAPEBGPEAPEAVValue@2@@Z");
    g_releaseValue = Export<ReleaseValueFn>(g_dui,
        "?Release@Value@DirectUI@@QEAAXXZ");
    g_strToID = Export<StrToIDFn>(g_dui, "StrToID");
    g_findDescendent = Export<FindDescendentFn>(g_dui,
        "?FindDescendent@Element@DirectUI@@QEAAPEAV12@G@Z");
    if ((!g_getAccName || !g_releaseValue) &&
        g_diagnostics.load(std::memory_order_relaxed)) {
        Wh_Log(L"Zebra: filename identity API incomplete; "
               L"falling back to raw indices where names are unavailable");
    }
    auto bg = Export<PaintBackground_t*>(g_dui,
        "?PaintBackground@Element@DirectUI@@QEAAXPEAUHDC__@@PEAVValue@2@"
        "AEBUtagRECT@@222@Z");
    if (!g_role || !g_index || !g_dpi || !g_parent || !bg) {
        Wh_Log(L"Zebra: missing required export");
        return false;
    }
    g_dcBrush = reinterpret_cast<HBRUSH>(GetStockObject(DC_BRUSH));
    if (!g_dcBrush) {
        Wh_Log(L"Zebra: unable to get stock GDI objects");
        return false;
    }
    if (!InitBlendSource()) {
        Wh_Log(L"Zebra: unable to create alpha blend source");
        return false;
    }
    if (g_stopping.load(std::memory_order_acquire)) return false;

    // Keep the very cheap window-creation sentinel to notice subsequent
    // folder-view replacement. Hook changes still run on the worker.
    if (!WindhawkUtils::SetFunctionHook(bg, PaintBackgroundHook,
                                         &g_backgroundOriginal) ||
        !WindhawkUtils::SetFunctionHook(DispatchMessageW, DispatchMessageHook,
                                         &g_dispatchOriginal) ||
        !Wh_ApplyHookOperations()) {
        Wh_Log(L"Zebra: hook registration failed");
        return false;
    }
    return true;
}

VOID CALLBACK ActivateOnWorker(PTP_CALLBACK_INSTANCE, void*, PTP_WORK) {
    const bool installed = InstallFolderPainter();
    if (installed) {
        // A settings change during deferred initialization must not be lost.
        // Publish 'installed' under the same lock used by settings callbacks.
        for (;;) {
            AcquireSRWLockExclusive(&g_activationLock);
            if (!g_settingsDirtyDuringActivation ||
                g_stopping.load(std::memory_order_relaxed)) {
                g_activationInstalled = true;
                ReleaseSRWLockExclusive(&g_activationLock);
                break;
            }
            g_settingsDirtyDuringActivation = false;
            ReleaseSRWLockExclusive(&g_activationLock);
            LoadSettings();
            RefreshColorsIfDue(GetTickCount(), true);
        }
        if (!g_stopping.load(std::memory_order_acquire)) {
            g_enabled.store(true, std::memory_order_release);
            // The view may already have painted during hook installation.
            RepaintExplorerOnce();
        }
    }
    AcquireSRWLockExclusive(&g_activationLock);
    // An unsuccessful hook transaction must not repeatedly allocate GDI/DUI
    // resources every time a new folder opens. A mod reload can retry it.
    g_activationAttempted = true;
    g_activationQueued = false;
    ReleaseSRWLockExclusive(&g_activationLock);
}

} // namespace
BOOL Wh_ModInit() {
    // Windhawk must load the mod into explorer.exe, but ordinary shell startup
    // gets only the lightweight window-creation hook. No active paint hooks,
    // DirectUI loads, graphics buffers, registry handles or worker threads.
    if (!WindhawkUtils::SetFunctionHook(CreateWindowExW, CreateWindowExWHook,
                                         &g_createWindowOriginal)) {
        Wh_Log(L"Zebra: hook registration failed");
        return FALSE;
    }
    return TRUE;
}

void Wh_ModAfterInit() {
    AcquireSRWLockExclusive(&g_activationLock);
    g_activationReady = true;
    ReleaseSRWLockExclusive(&g_activationLock);
    // Handles enabling the mod while an Explorer folder window is already up.
    if (AnyFolderViewExists()) QueueFolderActivation();
}

void Wh_ModSettingsChanged() {
    AcquireSRWLockExclusive(&g_activationLock);
    const bool installed = g_activationInstalled;
    if (g_activationQueued && !installed)
        g_settingsDirtyDuringActivation = true;
    ReleaseSRWLockExclusive(&g_activationLock);
    if (!installed) return; // Dormant: settings load when the first view opens.
    LoadSettings();
    RefreshColorsIfDue(GetTickCount(), true);
    RepaintExplorerOnce();
}

void Wh_ModBeforeUninit() {
    g_stopping.store(true, std::memory_order_release);
    g_enabled.store(false, std::memory_order_release);
    // Check/stop submissions under the same lock as QueueFolderActivation.
    // WaitForThreadpoolWorkCallbacks waits for the callback's actual return,
    // unlike a flag cleared by that callback before its return instruction.
    AcquireSRWLockExclusive(&g_activationLock);
    PTP_WORK work = g_activationWork;
    g_activationWork = nullptr;
    ReleaseSRWLockExclusive(&g_activationLock);
    if (work) {
        WaitForThreadpoolWorkCallbacks(work, FALSE);
        CloseThreadpoolWork(work);
    }
}

BOOL CALLBACK RemoveFolderViewEpochChild(HWND hwnd, LPARAM) {
    wchar_t cls[64]{};
    if (GetClassNameW(hwnd, cls, ARRAYSIZE(cls)) &&
        _wcsicmp(cls, L"SHELLDLL_DefView") == 0)
        RemovePropW(hwnd, kFolderViewEpochProperty);
    return TRUE;
}

BOOL CALLBACK RemoveFolderViewEpochWindow(HWND hwnd, LPARAM) {
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    if (pid != GetCurrentProcessId()) return TRUE;
    wchar_t cls[64]{};
    if (GetClassNameW(hwnd, cls, ARRAYSIZE(cls)) &&
        _wcsicmp(cls, L"CabinetWClass") == 0)
        EnumChildWindows(hwnd, RemoveFolderViewEpochChild, 0);
    return TRUE;
}

void Wh_ModUninit() {
    if (g_nextFolderViewEpoch.load(std::memory_order_relaxed))
        EnumWindows(RemoveFolderViewEpochWindow, 0);
    FreePersonalizeNotification();
    FreeRoundedShapes();
    FreeBlendSource();
    if (g_dui) {
        FreeLibrary(g_dui);
        g_dui = nullptr;
    }
    if (g_activationInstalled)
        RepaintExplorerOnce();
}
