// ==WindhawkMod==
// @id              liquid-glass-icons
// @name            Liquid Glass Icons
// @description     Transforms desktop icons into translucent Liquid Glass tiles with inset artwork, specular rims, and glossy highlights without altering File Explorer or any windows
// @version         1.0.0
// @author          Arbaz14
// @github          https://github.com/Arbaz14
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lcomctl32 -lgdi32 -luser32
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Liquid Glass Icons

Transforms Windows 11 desktop icons (`FolderView` `SysListView32`) into sleek Liquid Glass tiles:

![Liquid Glass Icons Preview](https://raw.githubusercontent.com/Arbaz14/windhawk-mods/main/mods/liquid-glass-icons/screenshot.png)

- **Translucent Glass Backing**: Renders customizable glass tiles behind desktop icons with multiple optical presets (`Liquid Glass`, `Clear Transparent Glass`, `Frosted Crystal`, `Obsidian Tint`, `Glacial Aurora`, or custom hex tints).
- **Independent Rim & Sheen Control**: Dedicated slider to dial in border specular brightness (0â€“100%) independently from glass body transparency.
- **Translucent Icons**: Blends the icon graphic itself into the glass tile (`iconOpacity` slider, default `75%`) instead of rendering opaque blocks.
- **Inset Framing (No Clipped Corners)**: Scales icon artwork and shortcut overlays proportionally inside the Liquid Glass squircle so square icons never poke through the rounded glass border.
- **Logo Theming**: Optional colorize, liquid glass embossed logo, monochrome, silhouette, or silver grayscale.
- **Icons Only**: Strictly scoped to desktop icons (`FolderView` `SysListView32`), leaving File Explorer, taskbar, Start menu, and windows untouched.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- glassPreset: liquid
  $name: Glass Tile Preset
  $description: Visual style and optical material for the Liquid Glass icon tiles.
  $options:
  - liquid: Liquid Glass (Glossy fluid lens with vibrant specular rim & reflections)
  - transparent: Clear Transparent Glass (Ultra-clean crystal glass with sharp refraction)
  - frosted: Frosted Crystal (Balanced translucent white-blue frost)
  - obsidian: Obsidian Tint (Smoky dark glass)
  - aurora: Glacial Aurora (Cool cyan-ice tint)
  - custom: Custom Tint Color
- tintColor: "#DCEBFF"
  $name: Custom Tint Color (Hex)
  $description: Hex RGB tint used when Glass Tile Preset is set to Custom Tint Color.
- tileOpacity: 55
  $name: Glass Tile Body Opacity (%)
  $description: Visibility of the rounded glass backing tile (0-100). Lower values make the glass transparent / see-through. Set to 0 for no backing tile.
- glassRimBrightness: 85
  $name: Glass Rim & Specular Brightness (%)
  $description: Brightness of the glass specular border and top glossy reflection sheen (0-100). Keep high (75-100) for glowing liquid glass edges even when body opacity is low.
- iconOpacity: 75
  $name: Icon Artwork Opacity (%)
  $description: Transparency of the icon graphic itself (15-100). Lower values let more wallpaper and glass show through.
- iconScale: 84
  $name: Icon Size Inside Tile (%)
  $description: Scales icons proportionally inside the glass tile (65-100) so square icons sit cleanly inside the rounded border.
- cornerRadius: 10
  $name: Tile Corner Radius (px)
  $description: Squircle corner rounding in 96-DPI pixels (0-22).
- showGlassSheen: true
  $name: Top Glass Reflection Sheen
  $description: Adds a soft curved glossy highlight across the upper half of each icon tile.
- iconTheming: none
  $name: Icon Theming Mode
  $description: How to recolor desktop app icon logos.
  $options:
  - none: Original Icon Colors
  - colorize: Colorize Logo (Preserves white lines, outlines, & details - keeps the real logo shape!)
  - glass: Liquid Glass Logo (Logo itself becomes translucent glass with specular highlights & rim!)
  - monochrome: Monochrome / Material You (Uniform single-color luminance)
  - silhouette: Flat Android Silhouette (Solid single-color icon glyph)
  - grayscale: Silver Grayscale (Clean monochromatic gray)
- iconThemeColor: white
  $name: Themed Icon Color
  $description: The color applied to the icon artwork when Icon Theming is enabled.
  $options:
  - white: Crisp Pure White (#FFFFFF)
  - tile: Match Glass Tile Tint
  - dark: Dark Charcoal (#1E1E1E)
  - custom: Custom Icon Color (Hex)
- iconCustomColor: "#FFFFFF"
  $name: Custom Icon Color (Hex)
  $description: Hex RGB color used when Themed Icon Color is set to Custom.
*/
// ==/WindhawkModSettings==

#include <commctrl.h>
#include <d2d1.h>

#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif

#include <algorithm>
#include <atomic>
#include <cstdint>
#include <cstring>
#include <cwctype>
#include <optional>
#include <string_view>
#include <type_traits>
#include <unordered_map>
#include <vector>

namespace {

// ============================================================================
// Pure Settings, Color Math & Icon Geometry
// ============================================================================

enum class IconGlassPreset {
    kLiquid,
    kTransparent,
    kFrosted,
    kObsidian,
    kAurora,
    kCustom,
};

enum class IconThemingMode {
    kNone,
    kColorize,
    kGlass,
    kMonochrome,
    kSilhouette,
    kGrayscale,
};

enum class IconThemeColorChoice {
    kWhite,
    kTile,
    kDark,
    kCustom,
};

struct IconModSettings {
    IconGlassPreset preset = IconGlassPreset::kLiquid;
    COLORREF tintColor = RGB(0xDC, 0xEB, 0xFF);
    int iconOpacity = 75;
    int tileOpacity = 55;
    int glassRimBrightness = 85;
    int iconScale = 84;
    int cornerRadius = 10;
    bool showGlassSheen = true;
    IconThemingMode iconTheming = IconThemingMode::kColorize;
    IconThemeColorChoice iconThemeColor = IconThemeColorChoice::kWhite;
    COLORREF iconCustomColor = RGB(0xFF, 0xFF, 0xFF);
};

struct RgbaColorStop {
    BYTE a;
    BYTE r;
    BYTE g;
    BYTE b;
};

struct ResolvedIconPalette {
    RgbaColorStop topFill;
    RgbaColorStop midFill;
    RgbaColorStop botFill;
    RgbaColorStop topRim;
    RgbaColorStop midRim;
    RgbaColorStop botRim;
};

constexpr size_t kThemeCacheSlotCount = 2;  // [0]=light, [1]=dark

struct IconCacheEntry {
    HDC tile[kThemeCacheSlotCount] = {};
    HDC sheen[kThemeCacheSlotCount] = {};
};

constexpr WCHAR kRegistryPersonalize[] =
    L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize";

constexpr UINT_PTR kDesktopListSubclassId = 0x4C47494Fu;  // "LGIO"

int HexDigit(WCHAR ch) {
    if (ch >= L'0' && ch <= L'9') return ch - L'0';
    if (ch >= L'a' && ch <= L'f') return 10 + (ch - L'a');
    if (ch >= L'A' && ch <= L'F') return 10 + (ch - L'A');
    return -1;
}

std::optional<COLORREF> TryParseHexColor(std::wstring_view raw) {
    while (!raw.empty() && std::iswspace(raw.front())) raw.remove_prefix(1);
    while (!raw.empty() && std::iswspace(raw.back())) raw.remove_suffix(1);
    if (!raw.empty() && raw.front() == L'#') {
        raw.remove_prefix(1);
    } else if (raw.size() >= 2 && raw[0] == L'0' &&
               (raw[1] == L'x' || raw[1] == L'X')) {
        raw.remove_prefix(2);
    }
    if (raw.size() != 6) {
        return std::nullopt;
    }
    int d[6] = {};
    for (size_t i = 0; i < 6; ++i) {
        d[i] = HexDigit(raw[i]);
        if (d[i] < 0) return std::nullopt;
    }
    return RGB((d[0] << 4) | d[1], (d[2] << 4) | d[3], (d[4] << 4) | d[5]);
}

bool EqualsIgnoreCase(std::wstring_view a, std::wstring_view b) {
    if (a.size() != b.size()) return false;
    for (size_t i = 0; i < a.size(); ++i) {
        if (std::towlower(a[i]) != std::towlower(b[i])) return false;
    }
    return true;
}

IconGlassPreset ParsePreset(std::wstring_view raw) {
    if (EqualsIgnoreCase(raw, L"liquid")) return IconGlassPreset::kLiquid;
    if (EqualsIgnoreCase(raw, L"transparent")) return IconGlassPreset::kTransparent;
    if (EqualsIgnoreCase(raw, L"obsidian")) return IconGlassPreset::kObsidian;
    if (EqualsIgnoreCase(raw, L"aurora")) return IconGlassPreset::kAurora;
    if (EqualsIgnoreCase(raw, L"custom")) return IconGlassPreset::kCustom;
    return IconGlassPreset::kFrosted;
}

IconThemingMode ParseIconTheming(std::wstring_view raw) {
    if (EqualsIgnoreCase(raw, L"none")) return IconThemingMode::kNone;
    if (EqualsIgnoreCase(raw, L"glass")) return IconThemingMode::kGlass;
    if (EqualsIgnoreCase(raw, L"monochrome")) return IconThemingMode::kMonochrome;
    if (EqualsIgnoreCase(raw, L"silhouette")) return IconThemingMode::kSilhouette;
    if (EqualsIgnoreCase(raw, L"grayscale")) return IconThemingMode::kGrayscale;
    return IconThemingMode::kColorize;
}

IconThemeColorChoice ParseIconThemeColor(std::wstring_view raw) {
    if (EqualsIgnoreCase(raw, L"tile")) return IconThemeColorChoice::kTile;
    if (EqualsIgnoreCase(raw, L"dark")) return IconThemeColorChoice::kDark;
    if (EqualsIgnoreCase(raw, L"custom")) return IconThemeColorChoice::kCustom;
    return IconThemeColorChoice::kWhite;
}

BYTE MulAlphaPct(BYTE baseAlpha, int pct) {
    int clamped = std::clamp(pct, 0, 100);
    return static_cast<BYTE>((static_cast<int>(baseAlpha) * clamped + 50) / 100);
}

BYTE BlendChannel(BYTE a, BYTE b, int weightBOf100) {
    return static_cast<BYTE>(
        (static_cast<int>(a) * (100 - weightBOf100) +
         static_cast<int>(b) * weightBOf100 + 50) /
        100);
}

ResolvedIconPalette ResolveIconPalette(const IconModSettings& cfg, bool dark) {
    auto scaleStops = [&](ResolvedIconPalette pal) {
        pal.topFill.a = MulAlphaPct(pal.topFill.a, cfg.tileOpacity);
        pal.midFill.a = MulAlphaPct(pal.midFill.a, cfg.tileOpacity);
        pal.botFill.a = MulAlphaPct(pal.botFill.a, cfg.tileOpacity);
        int rimFactor = (cfg.tileOpacity > 0) ? cfg.glassRimBrightness : 0;
        pal.topRim.a = MulAlphaPct(pal.topRim.a, rimFactor);
        pal.midRim.a = MulAlphaPct(pal.midRim.a, rimFactor);
        pal.botRim.a = MulAlphaPct(pal.botRim.a, rimFactor);
        return pal;
    };

    switch (cfg.preset) {
        case IconGlassPreset::kLiquid: {
            if (dark) {
                ResolvedIconPalette pal = {
                    {55, 210, 235, 255}, {20, 160, 205, 245}, {60, 140, 190, 235},
                    {255, 255, 255, 255}, {130, 205, 235, 255}, {180, 220, 245, 255},
                };
                return scaleStops(pal);
            }
            ResolvedIconPalette pal = {
                {65, 240, 248, 255}, {25, 220, 240, 255}, {70, 190, 225, 250},
                {255, 255, 255, 255}, {145, 225, 248, 255}, {195, 235, 252, 255},
            };
            return scaleStops(pal);
        }
        case IconGlassPreset::kTransparent: {
            if (dark) {
                ResolvedIconPalette pal = {
                    {28, 240, 245, 255}, {10, 200, 215, 235}, {32, 210, 225, 245},
                    {220, 255, 255, 255}, {95, 220, 235, 250}, {140, 235, 245, 255},
                };
                return scaleStops(pal);
            }
            ResolvedIconPalette pal = {
                {32, 255, 255, 255}, {12, 255, 255, 255}, {38, 250, 252, 255},
                {230, 255, 255, 255}, {105, 250, 252, 255}, {150, 255, 255, 255},
            };
            return scaleStops(pal);
        }
        case IconGlassPreset::kObsidian: {
            ResolvedIconPalette pal = {
                {118, 148, 166, 188}, {76, 84, 98, 118},   {92, 62, 72, 88},
                {220, 210, 232, 255}, {108, 175, 205, 235}, {148, 188, 214, 244},
            };
            return scaleStops(pal);
        }
        case IconGlassPreset::kAurora: {
            ResolvedIconPalette pal = {
                {138, 196, 240, 255}, {84, 96, 186, 232},  {100, 68, 148, 204},
                {235, 200, 248, 255}, {122, 140, 224, 255}, {162, 168, 234, 255},
            };
            return scaleStops(pal);
        }
        case IconGlassPreset::kCustom: {
            BYTE r = GetRValue(cfg.tintColor);
            BYTE g = GetGValue(cfg.tintColor);
            BYTE b = GetBValue(cfg.tintColor);
            ResolvedIconPalette pal = {
                {128, BlendChannel(r, 248, 45), BlendChannel(g, 250, 45),
                 BlendChannel(b, 255, 45)},
                {82, r, g, b},
                {96, BlendChannel(r, 12, 22), BlendChannel(g, 16, 22),
                 BlendChannel(b, 24, 22)},
                {230, BlendChannel(r, 255, 65), BlendChannel(g, 255, 65),
                 BlendChannel(b, 255, 65)},
                {116, BlendChannel(r, 245, 45), BlendChannel(g, 248, 45),
                 BlendChannel(b, 255, 45)},
                {154, BlendChannel(r, 255, 55), BlendChannel(g, 255, 55),
                 BlendChannel(b, 255, 55)},
            };
            return scaleStops(pal);
        }
        case IconGlassPreset::kFrosted:
        default: {
            if (dark) {
                ResolvedIconPalette pal = {
                    {122, 215, 232, 250}, {72, 134, 162, 192}, {88, 102, 126, 152},
                    {225, 222, 240, 255}, {112, 184, 216, 246}, {150, 196, 224, 252},
                };
                return scaleStops(pal);
            }
            ResolvedIconPalette pal = {
                {148, 255, 255, 255}, {98, 236, 244, 252},  {114, 212, 224, 238},
                {240, 255, 255, 255}, {132, 232, 240, 250}, {168, 242, 248, 255},
            };
            return scaleStops(pal);
        }
    }
}

COLORREF ResolveIconThemeColor(const IconModSettings& cfg, bool dark) {
    switch (cfg.iconThemeColor) {
        case IconThemeColorChoice::kWhite:
            return RGB(255, 255, 255);
        case IconThemeColorChoice::kDark:
            return RGB(30, 30, 30);
        case IconThemeColorChoice::kTile: {
            ResolvedIconPalette pal = ResolveIconPalette(cfg, dark);
            return RGB(pal.topRim.r, pal.topRim.g, pal.topRim.b);
        }
        case IconThemeColorChoice::kCustom:
        default:
            return cfg.iconCustomColor;
    }
}

bool IsDesktopIconListViewHost(std::wstring_view listClass,
                               std::wstring_view listTitle,
                               std::wstring_view parentClass,
                               std::wstring_view topClass,
                               bool topIsShell) {
    if (!EqualsIgnoreCase(listClass, L"SysListView32") ||
        !EqualsIgnoreCase(listTitle, L"FolderView") ||
        !EqualsIgnoreCase(parentClass, L"SHELLDLL_DefView")) {
        return false;
    }
    if (topIsShell) {
        return true;
    }
    return EqualsIgnoreCase(topClass, L"Progman") ||
           EqualsIgnoreCase(topClass, L"WorkerW");
}

bool ShouldStyleIconSize(int width, int height) {
    return width >= 20 && height >= 20 && width <= 256 && height <= 256;
}

RECT ComputeIconGlassTileRect(RECT iconRect) {
    int w = iconRect.right - iconRect.left;
    int h = iconRect.bottom - iconRect.top;
    int minDim = std::min(w, h);
    int sidePad = std::clamp(minDim / 11, 3, 9);
    // Stay within Desktop ListView's 2-4px top margin above LVIR_ICON so single-item
    // hover repaints never leave clipped upper strips behind.
    int topPad = std::clamp(minDim / 20, 2, 4);
    int bottomPad = std::clamp(minDim / 14, 2, 5);
    return {iconRect.left - sidePad, iconRect.top - topPad,
            iconRect.right + sidePad, iconRect.bottom + bottomPad};
}

// ============================================================================
// Runtime Globals & Settings
// ============================================================================

SRWLOCK g_settingsLock = SRWLOCK_INIT;
IconModSettings g_settings;

SRWLOCK g_cacheLock = SRWLOCK_INIT;
HMODULE g_d2d1Module = nullptr;
ID2D1Factory* g_d2dFactory = nullptr;
using D2D1CreateFactory_t = HRESULT(WINAPI*)(D2D1_FACTORY_TYPE,
                                             REFIID,
                                             const D2D1_FACTORY_OPTIONS*,
                                             void**);
D2D1CreateFactory_t g_pD2D1CreateFactory = nullptr;
constexpr size_t kMaxIconTileCacheEntries = 32;
constexpr size_t kMaxFrameTrackedIcons = 512;

struct PaintedDesktopIcon {
    HDC hdc;
    RECT iconRect;
};

std::unordered_map<uint64_t, IconCacheEntry> g_iconCache;

std::atomic<bool> g_unloading = false;
std::atomic<HWND> g_desktopListWnd = nullptr;
std::atomic<UINT> g_desktopDpi = 96;
std::atomic<bool> g_isDarkMode = true;

thread_local int g_desktopListPaintDepth = 0;
thread_local int g_iconDrawDepth = 0;
std::vector<PaintedDesktopIcon> g_paintedIconsThisFrame;

using ImageList_DrawIndirect_t = decltype(&ImageList_DrawIndirect);
ImageList_DrawIndirect_t ImageList_DrawIndirect_Original = nullptr;
ImageList_DrawIndirect_t ImageList_DrawIndirectV6_Original = nullptr;
std::atomic<void*> g_hookedV6DrawIndirectTarget = nullptr;

using IImageList_Draw_t =
    HRESULT(STDMETHODCALLTYPE*)(void* pThis, IMAGELISTDRAWPARAMS* pimldp);
IImageList_Draw_t IImageList_DrawV6_Original = nullptr;
std::atomic<void*> g_hookedV6ImlDrawTarget = nullptr;

using DrawIconEx_t = decltype(&DrawIconEx);
DrawIconEx_t DrawIconEx_Original = nullptr;

using CreateWindowExW_t = decltype(&CreateWindowExW);
CreateWindowExW_t CreateWindowExW_Original = nullptr;

IconModSettings GetSettingsSnapshot() {
    AcquireSRWLockShared(&g_settingsLock);
    IconModSettings copy = g_settings;
    ReleaseSRWLockShared(&g_settingsLock);
    return copy;
}

void ClearIconTileCache();

void LoadSettings() {
    IconModSettings next;
    if (PCWSTR raw = Wh_GetStringSetting(L"glassPreset")) {
        if (*raw != L'\0') {
            next.preset = ParsePreset(raw);
        }
        Wh_FreeStringSetting(raw);
    }
    if (PCWSTR raw = Wh_GetStringSetting(L"tintColor")) {
        if (auto c = TryParseHexColor(raw)) {
            next.tintColor = *c;
        }
        Wh_FreeStringSetting(raw);
    }
    if (PCWSTR raw = Wh_GetStringSetting(L"iconOpacity")) {
        if (*raw != L'\0') {
            next.iconOpacity = std::clamp(Wh_GetIntSetting(L"iconOpacity"), 15, 100);
        }
        Wh_FreeStringSetting(raw);
    }
    if (PCWSTR raw = Wh_GetStringSetting(L"tileOpacity")) {
        if (*raw != L'\0') {
            next.tileOpacity = std::clamp(Wh_GetIntSetting(L"tileOpacity"), 0, 100);
        }
        Wh_FreeStringSetting(raw);
    }
    if (PCWSTR raw = Wh_GetStringSetting(L"glassRimBrightness")) {
        if (*raw != L'\0') {
            next.glassRimBrightness =
                std::clamp(Wh_GetIntSetting(L"glassRimBrightness"), 0, 100);
        }
        Wh_FreeStringSetting(raw);
    }
    if (PCWSTR raw = Wh_GetStringSetting(L"iconScale")) {
        if (*raw != L'\0') {
            next.iconScale = std::clamp(Wh_GetIntSetting(L"iconScale"), 65, 100);
        }
        Wh_FreeStringSetting(raw);
    }
    if (PCWSTR raw = Wh_GetStringSetting(L"cornerRadius")) {
        if (*raw != L'\0') {
            next.cornerRadius = std::clamp(Wh_GetIntSetting(L"cornerRadius"), 0, 22);
        }
        Wh_FreeStringSetting(raw);
    }
    if (PCWSTR raw = Wh_GetStringSetting(L"showGlassSheen")) {
        if (*raw != L'\0') {
            next.showGlassSheen = Wh_GetIntSetting(L"showGlassSheen") != 0;
        }
        Wh_FreeStringSetting(raw);
    }
    if (PCWSTR raw = Wh_GetStringSetting(L"iconTheming")) {
        if (*raw != L'\0') {
            next.iconTheming = ParseIconTheming(raw);
        }
        Wh_FreeStringSetting(raw);
    }
    if (PCWSTR raw = Wh_GetStringSetting(L"iconThemeColor")) {
        if (*raw != L'\0') {
            next.iconThemeColor = ParseIconThemeColor(raw);
        }
        Wh_FreeStringSetting(raw);
    }
    if (PCWSTR raw = Wh_GetStringSetting(L"iconCustomColor")) {
        if (auto c = TryParseHexColor(raw)) {
            next.iconCustomColor = *c;
        }
        Wh_FreeStringSetting(raw);
    }

    AcquireSRWLockExclusive(&g_settingsLock);
    g_settings = next;
    ReleaseSRWLockExclusive(&g_settingsLock);
}

// ============================================================================
// D2D1 Liquid Glass Tile & Sheen Cache
// ============================================================================

void DeleteCacheBitmap(HDC& hdc) {
    if (!hdc) return;
    HGDIOBJ bmp = GetCurrentObject(hdc, OBJ_BITMAP);
    DeleteDC(hdc);
    if (bmp) DeleteObject(bmp);
    hdc = nullptr;
}

void ClearIconTileCacheLocked() {
    for (auto& [key, entry] : g_iconCache) {
        for (size_t i = 0; i < kThemeCacheSlotCount; ++i) {
            DeleteCacheBitmap(entry.tile[i]);
            DeleteCacheBitmap(entry.sheen[i]);
        }
    }
    g_iconCache.clear();
}

void ClearIconTileCache() {
    AcquireSRWLockExclusive(&g_cacheLock);
    ClearIconTileCacheLocked();
    ReleaseSRWLockExclusive(&g_cacheLock);
}

bool EnsureD2DFactory() {
    if (g_d2dFactory) {
        return true;
    }
    if (!g_pD2D1CreateFactory) {
        g_d2d1Module =
            LoadLibraryExW(L"d2d1.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
        if (!g_d2d1Module) {
            return false;
        }
        g_pD2D1CreateFactory = reinterpret_cast<D2D1CreateFactory_t>(
            GetProcAddress(g_d2d1Module, "D2D1CreateFactory"));
        if (!g_pD2D1CreateFactory) {
            return false;
        }
    }
    return SUCCEEDED(g_pD2D1CreateFactory(
        D2D1_FACTORY_TYPE_SINGLE_THREADED, __uuidof(ID2D1Factory), nullptr,
        reinterpret_cast<void**>(&g_d2dFactory)));
}

D2D1_COLOR_F ToD2DColor(RgbaColorStop c) {
    return D2D1::ColorF(c.r / 255.0f, c.g / 255.0f, c.b / 255.0f, c.a / 255.0f);
}

ID2D1LinearGradientBrush* CreateVerticalGradientBrush(
    ID2D1RenderTarget* target,
    float topY,
    float bottomY,
    const D2D1_GRADIENT_STOP* stops,
    UINT32 count) {
    ID2D1GradientStopCollection* collection = nullptr;
    if (FAILED(target->CreateGradientStopCollection(stops, count,
                                                    &collection))) {
        return nullptr;
    }
    ID2D1LinearGradientBrush* brush = nullptr;
    target->CreateLinearGradientBrush(
        D2D1::LinearGradientBrushProperties(D2D1::Point2F(0, topY),
                                            D2D1::Point2F(0, bottomY)),
        collection, &brush);
    collection->Release();
    return brush;
}

template <typename DrawFunc>
HDC CreateGlassPartBitmap(int width, int height, DrawFunc drawFunc) {
    if (width <= 0 || height <= 0 || !EnsureD2DFactory()) {
        return nullptr;
    }
    HDC hdc = CreateCompatibleDC(nullptr);
    if (!hdc) {
        return nullptr;
    }
    BITMAPINFO bmi = {};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = width;
    bmi.bmiHeader.biHeight = -height;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;
    void* bits = nullptr;
    HBITMAP bitmap =
        CreateDIBSection(hdc, &bmi, DIB_RGB_COLORS, &bits, nullptr, 0);
    if (!bitmap) {
        DeleteDC(hdc);
        return nullptr;
    }
    SelectObject(hdc, bitmap);

    D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties(
        D2D1_RENDER_TARGET_TYPE_SOFTWARE,
        D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM,
                          D2D1_ALPHA_MODE_PREMULTIPLIED),
        0, 0, D2D1_RENDER_TARGET_USAGE_GDI_COMPATIBLE);
    ID2D1DCRenderTarget* target = nullptr;
    RECT rc = {0, 0, width, height};
    if (FAILED(g_d2dFactory->CreateDCRenderTarget(&props, &target)) ||
        FAILED(target->BindDC(hdc, &rc))) {
        if (target) target->Release();
        DeleteCacheBitmap(hdc);
        return nullptr;
    }
    target->BeginDraw();
    target->Clear(D2D1::ColorF(0, 0, 0, 0));
    drawFunc(target);
    HRESULT hr = target->EndDraw();
    target->Release();
    if (FAILED(hr)) {
        DeleteCacheBitmap(hdc);
        return nullptr;
    }
    return hdc;
}

uint64_t MakeTileCacheKey(UINT dpi, int width, int height) {
    return (static_cast<uint64_t>(dpi) << 32) |
           (static_cast<uint64_t>(static_cast<uint16_t>(width)) << 16) |
           static_cast<uint64_t>(static_cast<uint16_t>(height));
}

template <typename DrawFn>
bool PaintCachedGlassPart(HDC hdc,
                          LPCRECT pRect,
                          UINT dpi,
                          size_t slot,
                          bool isSheen,
                          BYTE alpha,
                          DrawFn&& drawPart) {
    if (!hdc || !pRect || alpha == 0) {
        return false;
    }
    int width = pRect->right - pRect->left;
    int height = pRect->bottom - pRect->top;
    if (width <= 0 || height <= 0 || width > 512 || height > 512) {
        return false;
    }

    uint64_t key = MakeTileCacheKey(dpi, width, height);
    AcquireSRWLockExclusive(&g_cacheLock);
    if (g_iconCache.find(key) == g_iconCache.end() &&
        g_iconCache.size() >= kMaxIconTileCacheEntries) {
        ClearIconTileCacheLocked();
    }
    IconCacheEntry& entry = g_iconCache[key];
    HDC& cached = isSheen ? entry.sheen[slot] : entry.tile[slot];
    if (!cached) {
        cached = CreateGlassPartBitmap(width, height,
                                       std::forward<DrawFn>(drawPart));
    }
    bool ok = false;
    if (cached) {
        BLENDFUNCTION blend = {AC_SRC_OVER, 0, alpha, AC_SRC_ALPHA};
        GdiAlphaBlend(hdc, pRect->left, pRect->top, width, height, cached, 0, 0,
                      width, height, blend);
        ok = true;
    }
    ReleaseSRWLockExclusive(&g_cacheLock);
    return ok;
}

bool PaintGlassIconTile(HDC hdc,
                        LPCRECT pRect,
                        UINT dpi,
                        bool dark,
                        BYTE alpha,
                        const IconModSettings& cfg) {
    if (!pRect || cfg.tileOpacity <= 0 || alpha == 0) {
        return false;
    }
    int width = pRect->right - pRect->left;
    int height = pRect->bottom - pRect->top;
    float scale = dpi / 96.0f;
    size_t slot = dark ? 1 : 0;

    return PaintCachedGlassPart(
        hdc, pRect, dpi, slot, false, alpha, [&](ID2D1RenderTarget* rt) {
            ResolvedIconPalette pal = ResolveIconPalette(cfg, dark);
            float margin = 0.5f * scale;
            float maxR = std::max(
                0.0f,
                std::min(width - margin * 2.0f, height - margin * 2.0f) * 0.5f);
            float radius =
                std::min(static_cast<float>(cfg.cornerRadius) * scale, maxR);
            D2D1_ROUNDED_RECT rrect = D2D1::RoundedRect(
                D2D1::RectF(margin, margin, width - margin, height - margin),
                radius, radius);
            D2D1_GRADIENT_STOP fillStops[] = {
                {0.0f, ToD2DColor(pal.topFill)},
                {0.48f, ToD2DColor(pal.midFill)},
                {1.0f, ToD2DColor(pal.botFill)},
            };
            if (auto* fill = CreateVerticalGradientBrush(
                    rt, margin, height - margin, fillStops,
                    ARRAYSIZE(fillStops))) {
                rt->FillRoundedRectangle(rrect, fill);
                fill->Release();
            }
            D2D1_GRADIENT_STOP rimStops[] = {
                {0.0f, ToD2DColor(pal.topRim)},
                {0.52f, ToD2DColor(pal.midRim)},
                {1.0f, ToD2DColor(pal.botRim)},
            };
            if (auto* rim = CreateVerticalGradientBrush(
                    rt, margin, height - margin, rimStops,
                    ARRAYSIZE(rimStops))) {
                rt->DrawRoundedRectangle(rrect, rim, 1.0f * scale);
                rim->Release();
            }
        });
}

bool PaintGlassIconSheen(HDC hdc,
                         LPCRECT pRect,
                         UINT dpi,
                         bool dark,
                         BYTE alpha,
                         const IconModSettings& cfg) {
    if (!pRect || !cfg.showGlassSheen || cfg.tileOpacity <= 0 || alpha == 0 ||
        cfg.glassRimBrightness <= 0) {
        return false;
    }
    int width = pRect->right - pRect->left;
    int height = pRect->bottom - pRect->top;
    float scale = dpi / 96.0f;
    size_t slot = dark ? 1 : 0;

    return PaintCachedGlassPart(
        hdc, pRect, dpi, slot, true, alpha, [&](ID2D1RenderTarget* rt) {
            BYTE topAlpha = MulAlphaPct(dark ? 75 : 85, cfg.glassRimBrightness);
            float margin = 1.5f * scale;
            float maxR = std::max(
                0.0f,
                std::min(width - margin * 2.0f, height - margin * 2.0f) * 0.5f);
            float radius = std::min(
                std::max(0.0f,
                         static_cast<float>(cfg.cornerRadius) * scale - scale),
                maxR);
            D2D1_ROUNDED_RECT rrect = D2D1::RoundedRect(
                D2D1::RectF(margin, margin, width - margin, height - margin),
                radius, radius);
            D2D1_GRADIENT_STOP stops[] = {
                {0.0f, D2D1::ColorF(1.0f, 1.0f, 1.0f, topAlpha / 255.0f)},
                {0.46f, D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.0f)},
                {1.0f, D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.0f)},
            };
            if (auto* brush = CreateVerticalGradientBrush(
                    rt, margin, height - margin, stops, ARRAYSIZE(stops))) {
                rt->FillRoundedRectangle(rrect, brush);
                brush->Release();
            }
        });
}

// ============================================================================
// Icon Hooks (IImageList v6 + ImageList_DrawIndirect + DrawIconEx)
// ============================================================================

struct ScopedIconScaleTransform {
    HDC hdc = nullptr;
    int prevMode = 0;
    XFORM prevXf = {};
    bool active = false;

    ScopedIconScaleTransform(HDC dc, int x, int y, int w, int h, int scalePct) {
        if (!dc || scalePct >= 100 || w <= 0 || h <= 0) {
            return;
        }
        int mode = SetGraphicsMode(dc, GM_ADVANCED);
        if (!mode) {
            return;
        }
        XFORM xfPrev = {};
        if (!GetWorldTransform(dc, &xfPrev)) {
            SetGraphicsMode(dc, mode);
            return;
        }
        float s = std::clamp(scalePct, 50, 100) / 100.0f;
        float cx = static_cast<float>(x) + static_cast<float>(w) * 0.5f;
        float cy = static_cast<float>(y) + static_cast<float>(h) * 0.5f;
        XFORM scaleXf = {s, 0.0f, 0.0f, s, cx * (1.0f - s), cy * (1.0f - s)};
        if (!ModifyWorldTransform(dc, &scaleXf, MWT_LEFTMULTIPLY)) {
            SetWorldTransform(dc, &xfPrev);
            SetGraphicsMode(dc, mode);
            return;
        }
        hdc = dc;
        prevMode = mode;
        prevXf = xfPrev;
        active = true;
    }

    ~ScopedIconScaleTransform() {
        if (active) {
            SetWorldTransform(hdc, &prevXf);
            SetGraphicsMode(hdc, prevMode);
        }
    }
};

void RefreshSystemDarkMode() {
    DWORD val = 0;
    DWORD cb = sizeof(val);
    if (RegGetValueW(HKEY_CURRENT_USER, kRegistryPersonalize,
                     L"SystemUsesLightTheme", RRF_RT_REG_DWORD, nullptr, &val,
                     &cb) == ERROR_SUCCESS) {
        g_isDarkMode.store(val == 0, std::memory_order_relaxed);
    } else {
        g_isDarkMode.store(true, std::memory_order_relaxed);
    }
}

bool IsDesktopSystemDarkMode() {
    return g_isDarkMode.load(std::memory_order_relaxed);
}

UINT ResolveDesktopIconDpi(HDC hdc) {
    UINT d = g_desktopDpi.load(std::memory_order_relaxed);
    if (d >= 96) return d;
    if (HWND w = WindowFromDC(hdc)) {
        UINT wd = GetDpiForWindow(w);
        if (wd >= 96) return wd;
    }
    return GetDpiForSystem();
}

bool IsLikelyIconCacheScratchDraw(const IMAGELISTDRAWPARAMS* p, int w, int h) {
    if (!p || !p->hdcDst) {
        return true;
    }
    if ((p->cx > 0 && p->cy > 0) || WindowFromDC(p->hdcDst) != nullptr) {
        return false;
    }
    HGDIOBJ bmp = GetCurrentObject(p->hdcDst, OBJ_BITMAP);
    BITMAP bm = {};
    if (bmp && GetObjectW(bmp, sizeof(bm), &bm) == sizeof(bm) &&
        bm.bmWidth > 0 && bm.bmHeight > 0) {
        return bm.bmWidth <= w && bm.bmHeight <= h && p->x == 0 && p->y == 0;
    }
    return false;
}

struct ScopedDesktopListPaint {
    ScopedDesktopListPaint() { ++g_desktopListPaintDepth; }
    ~ScopedDesktopListPaint() {
        if (--g_desktopListPaintDepth <= 0) {
            g_desktopListPaintDepth = 0;
            g_paintedIconsThisFrame.clear();
        }
    }
};

struct DesktopIconPaintState {
    bool styled = false;
    bool drawTile = false;
    RECT tileRect = {};
    RECT scaleRect = {};
    UINT dpi = 96;
    bool dark = true;
    IconModSettings cfg;
};

bool FindOverlappingParentIconRect(HDC hdc, RECT rect, RECT* outParentRect) {
    int64_t areaCur = static_cast<int64_t>(rect.right - rect.left) *
                      (rect.bottom - rect.top);
    if (areaCur <= 0) {
        return false;
    }
    for (auto it = g_paintedIconsThisFrame.rbegin();
         it != g_paintedIconsThisFrame.rend(); ++it) {
        if (it->hdc != hdc) {
            continue;
        }
        const RECT& p = it->iconRect;
        int ix0 = std::max(rect.left, p.left);
        int iy0 = std::max(rect.top, p.top);
        int ix1 = std::min(rect.right, p.right);
        int iy1 = std::min(rect.bottom, p.bottom);
        if (ix1 <= ix0 || iy1 <= iy0) {
            continue;
        }
        int64_t inter = static_cast<int64_t>(ix1 - ix0) * (iy1 - iy0);
        int64_t areaPrev =
            static_cast<int64_t>(p.right - p.left) * (p.bottom - p.top);
        if (inter * 2 >= std::min(areaCur, areaPrev)) {
            *outParentRect = p;
            return true;
        }
    }
    return false;
}

DesktopIconPaintState TryBeginDesktopIconPaint(
    HDC hdc, int x, int y, int w, int h, BYTE alpha) {
    DesktopIconPaintState st;
    if (g_unloading.load(std::memory_order_relaxed) ||
        g_desktopListPaintDepth <= 0 || g_iconDrawDepth > 0 || alpha == 0 ||
        !hdc || w <= 0 || h <= 0) {
        return st;
    }

    RECT iconRect = {x, y, x + w, y + h};
    RECT parentRect = {};
    if (FindOverlappingParentIconRect(hdc, iconRect, &parentRect)) {
        // Sequential overlay badges (e.g. shortcut arrows) draw over an already-
        // tiled icon in the same WM_PAINT pass: skip the 2nd glass circle and
        // scale around the parent icon's center so the badge stays aligned.
        st.styled = true;
        st.drawTile = false;
        st.scaleRect = parentRect;
        st.cfg = GetSettingsSnapshot();
        return st;
    }

    if (!ShouldStyleIconSize(w, h)) {
        return st;
    }

    st.styled = true;
    st.drawTile = true;
    st.cfg = GetSettingsSnapshot();
    st.tileRect = ComputeIconGlassTileRect(iconRect);
    st.scaleRect = iconRect;
    st.dpi = ResolveDesktopIconDpi(hdc);
    st.dark = IsDesktopSystemDarkMode();
    if (st.cfg.tileOpacity > 0) {
        PaintGlassIconTile(hdc, &st.tileRect, st.dpi, st.dark, alpha, st.cfg);
    }
    if (g_paintedIconsThisFrame.size() < kMaxFrameTrackedIcons) {
        g_paintedIconsThisFrame.push_back({hdc, iconRect});
    }
    return st;
}

void RgbToHsl(BYTE r, BYTE g, BYTE b, float& h, float& s, float& l) {
    float rf = r / 255.0f, gf = g / 255.0f, bf = b / 255.0f;
    float max = std::max({rf, gf, bf});
    float min = std::min({rf, gf, bf});
    l = (max + min) * 0.5f;
    if (max == min) {
        h = s = 0.0f;
    } else {
        float d = max - min;
        s = l > 0.5f ? d / (2.0f - max - min) : d / (max + min);
        if (max == rf) {
            h = (gf - bf) / d + (gf < bf ? 6.0f : 0.0f);
        } else if (max == gf) {
            h = (bf - rf) / d + 2.0f;
        } else {
            h = (rf - gf) / d + 4.0f;
        }
        h /= 6.0f;
    }
}

float HueToRgb(float p, float q, float t) {
    if (t < 0.0f) t += 1.0f;
    if (t > 1.0f) t -= 1.0f;
    if (t < 1.0f / 6.0f) return p + (q - p) * 6.0f * t;
    if (t < 1.0f / 2.0f) return q;
    if (t < 2.0f / 3.0f) return p + (q - p) * (2.0f / 3.0f - t) * 6.0f;
    return p;
}

void HslToRgb(float h, float s, float l, BYTE& r, BYTE& g, BYTE& b) {
    if (s <= 0.001f) {
        r = g = b = static_cast<BYTE>(std::clamp(l * 255.0f, 0.0f, 255.0f));
    } else {
        float q = l < 0.5f ? l * (1.0f + s) : l + s - l * s;
        float p = 2.0f * l - q;
        r = static_cast<BYTE>(
            std::clamp(HueToRgb(p, q, h + 1.0f / 3.0f) * 255.0f, 0.0f, 255.0f));
        g = static_cast<BYTE>(
            std::clamp(HueToRgb(p, q, h) * 255.0f, 0.0f, 255.0f));
        b = static_cast<BYTE>(
            std::clamp(HueToRgb(p, q, h - 1.0f / 3.0f) * 255.0f, 0.0f, 255.0f));
    }
}

void RecolorIconPixels(DWORD* pixels,
                       int w,
                       int h,
                       const IconModSettings& cfg,
                       bool dark) {
    if (!pixels || w <= 0 || h <= 0 ||
        cfg.iconTheming == IconThemingMode::kNone) {
        return;
    }
    COLORREF themeClr = ResolveIconThemeColor(cfg, dark);
    BYTE tr = GetRValue(themeClr);
    BYTE tg = GetGValue(themeClr);
    BYTE tb = GetBValue(themeClr);
    float targetH = 0.0f, targetS = 0.0f, targetL = 0.5f;
    RgbToHsl(tr, tg, tb, targetH, targetS, targetL);

    size_t count = static_cast<size_t>(w) * static_cast<size_t>(h);
    for (size_t i = 0; i < count; ++i) {
        DWORD px = pixels[i];
        BYTE a = static_cast<BYTE>((px >> 24) & 0xFF);
        if (a == 0) {
            continue;
        }
        BYTE r = static_cast<BYTE>((px >> 16) & 0xFF);
        BYTE g = static_cast<BYTE>((px >> 8) & 0xFF);
        BYTE b = static_cast<BYTE>(px & 0xFF);

        int ur = (static_cast<int>(r) * 255) / a;
        int ug = (static_cast<int>(g) * 255) / a;
        int ub = (static_cast<int>(b) * 255) / a;
        ur = std::min(255, ur);
        ug = std::min(255, ug);
        ub = std::min(255, ub);

        int lum = (54 * ur + 183 * ug + 19 * ub) >> 8;

        int outR = r;
        int outG = g;
        int outB = b;

        switch (cfg.iconTheming) {
            case IconThemingMode::kColorize: {
                float ph, ps, pl;
                RgbToHsl(static_cast<BYTE>(ur), static_cast<BYTE>(ug),
                         static_cast<BYTE>(ub), ph, ps, pl);
                if (ps < 0.10f || pl > 0.94f || pl < 0.06f) {
                    outR = (ur * a) / 255;
                    outG = (ug * a) / 255;
                    outB = (ub * a) / 255;
                } else if (targetS <= 0.05f) {
                    // White/charcoal targets have no hue: produce a neutral tone, not hue 0 (red).
                    float newL = pl + (targetL - pl) * 0.5f;
                    BYTE cr, cg, cb;
                    HslToRgb(0.0f, 0.0f, newL, cr, cg, cb);
                    outR = (static_cast<int>(cr) * a) / 255;
                    outG = (static_cast<int>(cg) * a) / 255;
                    outB = (static_cast<int>(cb) * a) / 255;
                } else {
                    BYTE cr, cg, cb;
                    float newS = std::max(ps, targetS);
                    HslToRgb(targetH, newS, pl, cr, cg, cb);
                    outR = (static_cast<int>(cr) * a) / 255;
                    outG = (static_cast<int>(cg) * a) / 255;
                    outB = (static_cast<int>(cb) * a) / 255;
                }
                break;
            }
            case IconThemingMode::kGlass: {
                int glassAlpha =
                    std::clamp((a * (35 + (lum * 170) / 255)) / 255, 0, 255);
                int spec = std::max(0, lum - 130);
                int specWeight = std::min(100, (spec * 100) / 125);
                BYTE gr = BlendChannel(tr, 255, specWeight);
                BYTE gg = BlendChannel(tg, 255, specWeight);
                BYTE gb = BlendChannel(tb, 255, specWeight);
                outR = (static_cast<int>(gr) * glassAlpha) / 255;
                outG = (static_cast<int>(gg) * glassAlpha) / 255;
                outB = (static_cast<int>(gb) * glassAlpha) / 255;
                a = static_cast<BYTE>(glassAlpha);
                break;
            }
            case IconThemingMode::kMonochrome: {
                int curR = (tr * lum) / 255;
                int curG = (tg * lum) / 255;
                int curB = (tb * lum) / 255;
                outR = (curR * a) / 255;
                outG = (curG * a) / 255;
                outB = (curB * a) / 255;
                break;
            }
            case IconThemingMode::kSilhouette: {
                if (lum < 35) {
                    outR = (tr * lum * a) / (35 * 255);
                    outG = (tg * lum * a) / (35 * 255);
                    outB = (tb * lum * a) / (35 * 255);
                } else {
                    outR = (tr * a) / 255;
                    outG = (tg * a) / 255;
                    outB = (tb * a) / 255;
                }
                break;
            }
            case IconThemingMode::kGrayscale: {
                outR = (lum * a) / 255;
                outG = (lum * a) / 255;
                outB = (lum * a) / 255;
                break;
            }
            case IconThemingMode::kNone:
            default:
                break;
        }

        pixels[i] = (static_cast<DWORD>(a) << 24) |
                    (static_cast<DWORD>(std::clamp(outR, 0, 255)) << 16) |
                    (static_cast<DWORD>(std::clamp(outG, 0, 255)) << 8) |
                    static_cast<DWORD>(std::clamp(outB, 0, 255));
    }
}

bool PaintThemedIconEx(HDC hdc,
                       int xLeft,
                       int yTop,
                       HICON hIcon,
                       int w,
                       int h,
                       UINT istepIfAniCur,
                       UINT diFlags,
                       BYTE contentAlpha,
                       const IconModSettings& cfg,
                       bool dark) {
    if (!DrawIconEx_Original || w <= 0 || h <= 0 || w > 512 || h > 512) {
        return false;
    }
    BITMAPINFO bmi = {};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = w;
    bmi.bmiHeader.biHeight = -(h * 2);
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;
    void* rawBits = nullptr;
    HBITMAP bmp =
        CreateDIBSection(hdc, &bmi, DIB_RGB_COLORS, &rawBits, nullptr, 0);
    if (!bmp || !rawBits) {
        if (bmp) DeleteObject(bmp);
        return false;
    }
    HDC memDc = CreateCompatibleDC(hdc);
    if (!memDc) {
        DeleteObject(bmp);
        return false;
    }
    HGDIOBJ oldBmp = SelectObject(memDc, bmp);
    auto* pixels = static_cast<DWORD*>(rawBits);
    size_t count = static_cast<size_t>(w) * static_cast<size_t>(h);
    std::memset(pixels, 0x00, count * sizeof(DWORD));
    std::memset(pixels + count, 0xFF, count * sizeof(DWORD));

    BOOL okBlack = DrawIconEx_Original(memDc, 0, 0, hIcon, w, h, istepIfAniCur,
                                       nullptr, diFlags);
    BOOL okWhite = DrawIconEx_Original(memDc, 0, h, hIcon, w, h, istepIfAniCur,
                                       nullptr, diFlags);
    bool blended = false;
    if (okBlack && okWhite) {
        for (size_t i = 0; i < count; ++i) {
            DWORD bp = pixels[i];
            DWORD wp = pixels[count + i];
            int db = static_cast<int>(wp & 0xFF) - static_cast<int>(bp & 0xFF);
            int dg = static_cast<int>((wp >> 8) & 0xFF) -
                     static_cast<int>((bp >> 8) & 0xFF);
            int dr = static_cast<int>((wp >> 16) & 0xFF) -
                     static_cast<int>((bp >> 16) & 0xFF);
            int bg = std::clamp((dr + dg + db + 1) / 3, 0, 255);
            int a = 255 - bg;
            if (a <= 0) {
                pixels[i] = 0;
                continue;
            }
            int b = std::min(static_cast<int>(bp & 0xFF), a);
            int g = std::min(static_cast<int>((bp >> 8) & 0xFF), a);
            int r = std::min(static_cast<int>((bp >> 16) & 0xFF), a);
            pixels[i] = (static_cast<DWORD>(a) << 24) |
                        (static_cast<DWORD>(r) << 16) |
                        (static_cast<DWORD>(g) << 8) | static_cast<DWORD>(b);
        }
        RecolorIconPixels(pixels, w, h, cfg, dark);

        BLENDFUNCTION bf = {AC_SRC_OVER, 0, contentAlpha, AC_SRC_ALPHA};
        blended = GdiAlphaBlend(hdc, xLeft, yTop, w, h, memDc, 0, 0, w, h, bf) !=
                  FALSE;
    }

    SelectObject(memDc, oldBmp);
    DeleteDC(memDc);
    DeleteObject(bmp);
    return blended;
}

template <typename OrigFn, typename IsOkFn>
auto HandleImageListDrawParams(void* comSelf,
                               IMAGELISTDRAWPARAMS* p,
                               OrigFn&& callOrig,
                               IsOkFn&& isOk) -> decltype(callOrig(p)) {
    if (g_desktopListPaintDepth <= 0 || g_iconDrawDepth > 0 || !p ||
        p->cbSize < IMAGELISTDRAWPARAMS_V3_SIZE) {
        return callOrig(p);
    }
    HIMAGELIST himl =
        p->himl ? p->himl : reinterpret_cast<HIMAGELIST>(comSelf);
    if (!himl) {
        return callOrig(p);
    }
    int w = p->cx;
    int h = p->cy;
    if (w <= 0 || h <= 0) {
        if (!ImageList_GetIconSize(himl, &w, &h)) {
            return callOrig(p);
        }
    }
    if (comSelf && IsLikelyIconCacheScratchDraw(p, w, h)) {
        return callOrig(p);
    }

    BYTE alpha =
        (p->cbSize >= sizeof(IMAGELISTDRAWPARAMS) && (p->fState & ILS_ALPHA))
            ? static_cast<BYTE>(p->Frame)
            : 255;

    DesktopIconPaintState st =
        TryBeginDesktopIconPaint(p->hdcDst, p->x, p->y, w, h, alpha);

    IMAGELISTDRAWPARAMS* drawParams = p;
    IMAGELISTDRAWPARAMS localParams = {};
    if (st.styled && st.cfg.iconOpacity < 100) {
        size_t copyBytes =
            p->cbSize < sizeof(localParams) ? p->cbSize : sizeof(localParams);
        std::memcpy(&localParams, p, copyBytes);
        localParams.cbSize = sizeof(localParams);
        localParams.fState |= ILS_ALPHA;
        localParams.Frame = MulAlphaPct(alpha, st.cfg.iconOpacity);
        drawParams = &localParams;
    }

    bool ok = false;
    using RetType = decltype(callOrig(drawParams));
    RetType res{};
    if constexpr (std::is_same_v<RetType, HRESULT>) {
        res = S_OK;
    } else {
        res = TRUE;
    }

    {
        std::optional<ScopedIconScaleTransform> scaleGuard;
        if (st.styled && st.cfg.iconScale < 100) {
            scaleGuard.emplace(p->hdcDst, st.scaleRect.left, st.scaleRect.top,
                               st.scaleRect.right - st.scaleRect.left,
                               st.scaleRect.bottom - st.scaleRect.top,
                               st.cfg.iconScale);
        }
        ++g_iconDrawDepth;
        if (st.styled && st.cfg.iconTheming != IconThemingMode::kNone &&
            w > 0 && h > 0 && w <= 256 && h <= 256) {
            BITMAPINFO bmi = {};
            bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
            bmi.bmiHeader.biWidth = w;
            bmi.bmiHeader.biHeight = -h;
            bmi.bmiHeader.biPlanes = 1;
            bmi.bmiHeader.biBitCount = 32;
            bmi.bmiHeader.biCompression = BI_RGB;
            void* rawBits = nullptr;
            HDC memDc = CreateCompatibleDC(p->hdcDst);
            HBITMAP dib = memDc ? CreateDIBSection(memDc, &bmi, DIB_RGB_COLORS,
                                                   &rawBits, nullptr, 0)
                                : nullptr;
            if (memDc && dib && rawBits) {
                HGDIOBJ oldBmp = SelectObject(memDc, dib);
                std::memset(rawBits, 0x00,
                            static_cast<size_t>(w) * h * sizeof(DWORD));

                IMAGELISTDRAWPARAMS offParams = {};
                size_t copyBytes =
                    p->cbSize < sizeof(offParams) ? p->cbSize : sizeof(offParams);
                std::memcpy(&offParams, p, copyBytes);
                offParams.cbSize = sizeof(IMAGELISTDRAWPARAMS);
                offParams.hdcDst = memDc;
                offParams.x = 0;
                offParams.y = 0;
                offParams.cx = w;
                offParams.cy = h;
                offParams.fStyle =
                    (p->fStyle & ILD_OVERLAYMASK) | ILD_TRANSPARENT;
                offParams.fState &= ~ILS_ALPHA;
                offParams.Frame = 255;

                res = callOrig(&offParams);
                if (isOk(res)) {
                    RecolorIconPixels(static_cast<DWORD*>(rawBits), w, h,
                                      st.cfg, st.dark);

                    BYTE effAlpha = MulAlphaPct(alpha, st.cfg.iconOpacity);
                    BLENDFUNCTION bf = {AC_SRC_OVER, 0, effAlpha,
                                        AC_SRC_ALPHA};
                    GdiAlphaBlend(p->hdcDst, p->x, p->y, w, h, memDc, 0, 0,
                                  w, h, bf);
                    ok = true;
                }
                SelectObject(memDc, oldBmp);
                DeleteObject(dib);
                DeleteDC(memDc);
            }
        }
        if (!ok) {
            res = callOrig(drawParams);
            ok = isOk(res);
        }
        --g_iconDrawDepth;
    }

    if (st.drawTile && ok) {
        PaintGlassIconSheen(p->hdcDst, &st.tileRect, st.dpi, st.dark, alpha,
                            st.cfg);
    }
    return res;
}

BOOL HandleImageListDrawIndirect(IMAGELISTDRAWPARAMS* p,
                                 ImageList_DrawIndirect_t origFn) {
    if (!origFn) {
        return FALSE;
    }
    return HandleImageListDrawParams(
        nullptr, p,
        [origFn](IMAGELISTDRAWPARAMS* arg) { return origFn(arg); },
        [](BOOL ok) { return ok != FALSE; });
}

BOOL WINAPI ImageList_DrawIndirect_Hook(IMAGELISTDRAWPARAMS* pimldp) {
    return HandleImageListDrawIndirect(pimldp, ImageList_DrawIndirect_Original);
}

BOOL WINAPI ImageList_DrawIndirectV6_Hook(IMAGELISTDRAWPARAMS* pimldp) {
    return HandleImageListDrawIndirect(pimldp,
                                       ImageList_DrawIndirectV6_Original);
}

HRESULT STDMETHODCALLTYPE IImageList_DrawV6_Hook(void* pThis,
                                                 IMAGELISTDRAWPARAMS* pimldp) {
    if (!IImageList_DrawV6_Original) {
        return E_FAIL;
    }
    return HandleImageListDrawParams(
        pThis, pimldp,
        [pThis](IMAGELISTDRAWPARAMS* arg) {
            return IImageList_DrawV6_Original(pThis, arg);
        },
        [](HRESULT hr) { return SUCCEEDED(hr); });
}

BOOL WINAPI DrawIconEx_Hook(HDC hdc,
                            int xLeft,
                            int yTop,
                            HICON hIcon,
                            int cxWidth,
                            int cyWidth,
                            UINT istepIfAniCur,
                            HBRUSH hbrFlickerFreeDraw,
                            UINT diFlags) {
    if (!hIcon || !(diFlags & DI_IMAGE) || g_desktopListPaintDepth <= 0) {
        return DrawIconEx_Original(hdc, xLeft, yTop, hIcon, cxWidth, cyWidth,
                                   istepIfAniCur, hbrFlickerFreeDraw, diFlags);
    }
    UINT hintDpi = ResolveDesktopIconDpi(hdc);
    int w = cxWidth ? cxWidth : MulDiv(32, static_cast<int>(hintDpi), 96);
    int h = cyWidth ? cyWidth : MulDiv(32, static_cast<int>(hintDpi), 96);

    DesktopIconPaintState st =
        TryBeginDesktopIconPaint(hdc, xLeft, yTop, w, h, 255);

    BOOL ok = FALSE;
    {
        std::optional<ScopedIconScaleTransform> scaleGuard;
        if (st.styled && st.cfg.iconScale < 100) {
            scaleGuard.emplace(hdc, st.scaleRect.left, st.scaleRect.top,
                               st.scaleRect.right - st.scaleRect.left,
                               st.scaleRect.bottom - st.scaleRect.top,
                               st.cfg.iconScale);
        }
        ++g_iconDrawDepth;
        if (st.styled &&
            (st.cfg.iconTheming != IconThemingMode::kNone ||
             st.cfg.iconOpacity < 100) &&
            (diFlags & DI_NORMAL) == DI_NORMAL && !hbrFlickerFreeDraw) {
            ok = PaintThemedIconEx(hdc, xLeft, yTop, hIcon, w, h, istepIfAniCur,
                                   diFlags,
                                   MulAlphaPct(255, st.cfg.iconOpacity), st.cfg,
                                   st.dark);
        }
        if (!ok) {
            ok = DrawIconEx_Original(hdc, xLeft, yTop, hIcon, cxWidth, cyWidth,
                                     istepIfAniCur, hbrFlickerFreeDraw,
                                     diFlags);
        }
        --g_iconDrawDepth;
    }

    if (st.drawTile && ok) {
        PaintGlassIconSheen(hdc, &st.tileRect, st.dpi, st.dark, 255, st.cfg);
    }
    return ok;
}

// ============================================================================
// Desktop ListView Detection, V6 COMCTL32 Hooking & Cross-Thread Subclassing
// ============================================================================

void* ResolveComctlV6ImageListDraw(HMODULE comctlMod) {
    if (!comctlMod) {
        return nullptr;
    }
    using ImageList_Create_t = HIMAGELIST(WINAPI*)(int, int, UINT, int, int);
    using ImageList_Destroy_t = BOOL(WINAPI*)(HIMAGELIST);
    using QI_t = HRESULT(WINAPI*)(HIMAGELIST, REFIID, void**);
    auto pCreate = reinterpret_cast<ImageList_Create_t>(
        GetProcAddress(comctlMod, "ImageList_Create"));
    auto pQI = reinterpret_cast<QI_t>(
        GetProcAddress(comctlMod, "HIMAGELIST_QueryInterface"));
    auto pDestroy = reinterpret_cast<ImageList_Destroy_t>(
        GetProcAddress(comctlMod, "ImageList_Destroy"));
    if (!pCreate || !pQI || !pDestroy) {
        return nullptr;
    }
    HIMAGELIST tmp = pCreate(16, 16, ILC_COLOR32, 1, 1);
    if (!tmp) {
        return nullptr;
    }
    constexpr size_t kIImageListDrawVtblSlot = 8;
    constexpr IID kIidIImageList = {
        0x46eb5926, 0x582e, 0x4017,
        {0x9f, 0xdf, 0xe8, 0x99, 0x8d, 0xaa, 0x09, 0x50}};
    IUnknown* iml = nullptr;
    void* drawFn = nullptr;
    if (SUCCEEDED(pQI(tmp, kIidIImageList, reinterpret_cast<void**>(&iml))) &&
        iml) {
        void** vtbl = *reinterpret_cast<void***>(iml);
        if (vtbl) {
            void* candidate = vtbl[kIImageListDrawVtblSlot];
            HMODULE fnMod = nullptr;
            if (candidate &&
                GetModuleHandleExW(
                    GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                        GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                    reinterpret_cast<LPCWSTR>(candidate), &fnMod) &&
                fnMod == comctlMod) {
                drawFn = candidate;
            }
        }
        iml->Release();
    }
    pDestroy(tmp);
    return drawFn;
}

void HookComctl32V6Module(HMODULE comctlMod, bool applyOpsNow) {
    if (!comctlMod || g_unloading.load(std::memory_order_relaxed)) {
        return;
    }
    bool needApply = false;
    void* fn = reinterpret_cast<void*>(
        GetProcAddress(comctlMod, "ImageList_DrawIndirect"));
    if (fn && fn != reinterpret_cast<void*>(ImageList_DrawIndirect)) {
        void* expected = nullptr;
        if (g_hookedV6DrawIndirectTarget.compare_exchange_strong(expected, fn)) {
            if (Wh_SetFunctionHook(
                    fn,
                    reinterpret_cast<void*>(ImageList_DrawIndirectV6_Hook),
                    reinterpret_cast<void**>(
                        &ImageList_DrawIndirectV6_Original))) {
                needApply = true;
            }
        }
    }

    if (void* imlDrawFn = ResolveComctlV6ImageListDraw(comctlMod)) {
        if (imlDrawFn != fn &&
            imlDrawFn != reinterpret_cast<void*>(ImageList_DrawIndirect)) {
            void* expected = nullptr;
            if (g_hookedV6ImlDrawTarget.compare_exchange_strong(expected,
                                                                imlDrawFn)) {
                if (Wh_SetFunctionHook(
                        imlDrawFn,
                        reinterpret_cast<void*>(IImageList_DrawV6_Hook),
                        reinterpret_cast<void**>(&IImageList_DrawV6_Original))) {
                    needApply = true;
                }
            }
        }
    }

    if (needApply && applyOpsNow &&
        !g_unloading.load(std::memory_order_relaxed)) {
        Wh_ApplyHookOperations();
    }
}

bool IsDesktopListViewWnd(HWND hWnd) {
    if (!hWnd || !IsWindow(hWnd)) {
        return false;
    }
    WCHAR listCls[64] = {};
    WCHAR listTitle[64] = {};
    if (!GetClassNameW(hWnd, listCls, ARRAYSIZE(listCls)) ||
        !GetWindowTextW(hWnd, listTitle, ARRAYSIZE(listTitle))) {
        return false;
    }
    HWND parent = GetAncestor(hWnd, GA_PARENT);
    WCHAR parentCls[64] = {};
    if (!parent || !GetClassNameW(parent, parentCls, ARRAYSIZE(parentCls))) {
        return false;
    }
    HWND top = GetAncestor(parent, GA_PARENT);
    WCHAR topCls[64] = {};
    if (!top) {
        return false;
    }
    bool topIsShell = (top == GetShellWindow());
    if (!topIsShell && !GetClassNameW(top, topCls, ARRAYSIZE(topCls))) {
        return false;
    }
    return IsDesktopIconListViewHost(listCls, listTitle, parentCls, topCls,
                                     topIsShell);
}

LRESULT CALLBACK DesktopListSubclassProc(HWND hWnd,
                                         UINT uMsg,
                                         WPARAM wParam,
                                         LPARAM lParam,
                                         DWORD_PTR dwRefData) {
    switch (uMsg) {
        case WM_NCDESTROY: {
            HWND self = hWnd;
            g_desktopListWnd.compare_exchange_strong(
                self, nullptr, std::memory_order_relaxed);
            break;
        }
        case WM_DPICHANGED_BEFOREPARENT:
        case WM_DPICHANGED_AFTERPARENT: {
            UINT dpi = GetDpiForWindow(hWnd);
            if (dpi) {
                g_desktopDpi.store(dpi, std::memory_order_relaxed);
            }
            break;
        }
        case WM_SETTINGCHANGE: {
            RefreshSystemDarkMode();
            InvalidateRect(hWnd, nullptr, TRUE);
            break;
        }
        case WM_PAINT:
        case WM_PRINTCLIENT: {
            ScopedDesktopListPaint paintScope;
            return DefSubclassProc(hWnd, uMsg, wParam, lParam);
        }
    }
    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

void AttachDesktopListView(HWND hWnd) {
    if (!hWnd || !IsWindow(hWnd)) {
        return;
    }
    UINT dpi = GetDpiForWindow(hWnd);
    if (dpi) {
        g_desktopDpi.store(dpi, std::memory_order_relaxed);
    }
    if (!WindhawkUtils::SetWindowSubclassFromAnyThread(
            hWnd, DesktopListSubclassProc, 0)) {
        return;
    }
    HWND prev = g_desktopListWnd.exchange(hWnd, std::memory_order_relaxed);
    if (prev && prev != hWnd && IsWindow(prev)) {
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(
            prev, DesktopListSubclassProc);
    }
    InvalidateRect(hWnd, nullptr, TRUE);
}

HWND FindDesktopListUnder(HWND shellView) {
    if (!shellView) {
        return nullptr;
    }
    HWND listView = FindWindowExW(shellView, nullptr, L"SysListView32", nullptr);
    if (!listView) {
        return nullptr;
    }
    DWORD pid = 0;
    GetWindowThreadProcessId(listView, &pid);
    if (pid != GetCurrentProcessId() || !IsDesktopListViewWnd(listView)) {
        return nullptr;
    }
    return listView;
}

HWND FindExistingDesktopListView() {
    HWND shellWnd = GetShellWindow();
    if (HWND lv = FindDesktopListUnder(
            FindWindowExW(shellWnd, nullptr, L"SHELLDLL_DefView", nullptr))) {
        return lv;
    }
    if (shellWnd) {
        HWND childWorker = nullptr;
        while ((childWorker = FindWindowExW(shellWnd, childWorker, L"WorkerW",
                                            nullptr)) != nullptr) {
            if (HWND lv = FindDesktopListUnder(FindWindowExW(
                    childWorker, nullptr, L"SHELLDLL_DefView", nullptr))) {
                return lv;
            }
        }
    }
    HWND worker = nullptr;
    while ((worker = FindWindowExW(nullptr, worker, L"WorkerW", nullptr)) !=
           nullptr) {
        if (HWND lv = FindDesktopListUnder(
                FindWindowExW(worker, nullptr, L"SHELLDLL_DefView", nullptr))) {
            return lv;
        }
    }
    return nullptr;
}

HWND WINAPI CreateWindowExW_Hook(DWORD dwExStyle,
                                 LPCWSTR lpClassName,
                                 LPCWSTR lpWindowName,
                                 DWORD dwStyle,
                                 int X,
                                 int Y,
                                 int nWidth,
                                 int nHeight,
                                 HWND hWndParent,
                                 HMENU hMenu,
                                 HINSTANCE hInstance,
                                 LPVOID lpParam) {
    HWND hWnd = CreateWindowExW_Original(
        dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight,
        hWndParent, hMenu, hInstance, lpParam);
    if (hWnd && hWndParent && !g_unloading.load(std::memory_order_relaxed) &&
        !g_desktopListWnd.load(std::memory_order_relaxed) &&
        ((reinterpret_cast<ULONG_PTR>(lpClassName) <= 0xFFFF) ||
         EqualsIgnoreCase(lpClassName, WC_LISTVIEWW)) &&
        IsDesktopListViewWnd(hWnd)) {
        HookComctl32V6Module(
            reinterpret_cast<HMODULE>(GetClassLongPtrW(hWnd, GCLP_HMODULE)),
            true);
        AttachDesktopListView(hWnd);
    }
    return hWnd;
}

}  // namespace

// ============================================================================
// Windhawk Entry Points
// ============================================================================

BOOL Wh_ModInit() {
    LoadSettings();
    RefreshSystemDarkMode();

    Wh_SetFunctionHook(reinterpret_cast<void*>(ImageList_DrawIndirect),
                       reinterpret_cast<void*>(ImageList_DrawIndirect_Hook),
                       reinterpret_cast<void**>(&ImageList_DrawIndirect_Original));
    Wh_SetFunctionHook(reinterpret_cast<void*>(DrawIconEx),
                       reinterpret_cast<void*>(DrawIconEx_Hook),
                       reinterpret_cast<void**>(&DrawIconEx_Original));
    Wh_SetFunctionHook(reinterpret_cast<void*>(CreateWindowExW),
                       reinterpret_cast<void*>(CreateWindowExW_Hook),
                       reinterpret_cast<void**>(&CreateWindowExW_Original));

    WNDCLASSEXW lvClass = {sizeof(lvClass)};
    if (GetClassInfoExW(nullptr, L"SysListView32", &lvClass) &&
        lvClass.hInstance) {
        HookComctl32V6Module(lvClass.hInstance, false);
    }
    return TRUE;
}

void Wh_ModAfterInit() {
    if (HWND lv = FindExistingDesktopListView()) {
        HookComctl32V6Module(
            reinterpret_cast<HMODULE>(GetClassLongPtrW(lv, GCLP_HMODULE)),
            true);
        AttachDesktopListView(lv);
    }
}

void Wh_ModBeforeUninit() {
    g_unloading.store(true, std::memory_order_relaxed);
}

void Wh_ModUninit() {
    if (HWND lv =
            g_desktopListWnd.exchange(nullptr, std::memory_order_relaxed)) {
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(
            lv, DesktopListSubclassProc);
        InvalidateRect(lv, nullptr, TRUE);
    }
    g_paintedIconsThisFrame.clear();
    g_paintedIconsThisFrame.shrink_to_fit();
    ClearIconTileCache();
    if (g_d2dFactory) {
        g_d2dFactory->Release();
        g_d2dFactory = nullptr;
    }
    if (g_d2d1Module) {
        FreeLibrary(g_d2d1Module);
        g_d2d1Module = nullptr;
        g_pD2D1CreateFactory = nullptr;
    }
}

void Wh_ModSettingsChanged() {
    LoadSettings();
    ClearIconTileCache();
    if (HWND lv = FindExistingDesktopListView()) {
        AttachDesktopListView(lv);
    }
}
