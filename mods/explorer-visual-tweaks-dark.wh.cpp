// ==WindhawkMod==
// @id           explorer-visual-tweaks-dark
// @name         Explorer Visual Tweaks Dark
// @description  Explorer selection, progress, and Preview Pane visual tweaks
// @version      1.0.1
// @author       VitalS
// @github       https://github.com/VitalSkib
// @include      explorer.exe
// @include      prevhost.exe
// @architecture x86-64
// @compilerOptions -luser32 -lgdi32 -luxtheme -lmsimg32 -ladvapi32
// ==/WindhawkMod==
// ==WindhawkModReadme==
/*
This mod fixes visual inconsistencies in Windows Explorer and related windows,
primarily in the standard dark theme. It brings the Preview Pane, plain-text
preview, drive progress indicators, item selection, and Navigation Pane focus
states to a consistent appearance, and allows their colors and rounding to be
customized.
The selection and progress renderers aren't tied to a particular theme.
Their default colors are tuned for the standard dark theme, but can be changed
to suit another theme. The Preview Pane correction is intentionally inactive
in the standard light app mode, where the original color mismatch doesn't
exist.
Selection styling applies to Explorer and Explorer-based Open/Save dialogs.
Drive progress rendering replaces only the PROGRESS part/state pairs used by
Explorer. Preview Pane fixes cover Explorer, Open/Save dialog hosts, and
prevhost.exe.
Selection and Progress settings apply immediately.
ItemsView, Navigation Pane, and Progress customization can be disabled
independently. Disabled rendering is passed through to the next handler.
The Preview Pane switch follows the restart requirement below.
**Compatibility:** When using other mods that customize the same elements,
disable the overlapping blocks in this mod or the corresponding features in
the other mods to avoid conflicts.
### Screenshots
![Explorer selections and drive progress](https://raw.githubusercontent.com/VitalSkib/files/refs/heads/main/explorer-visual-tweaks-dark-thispc.png)
![Preview Pane and plain-text preview](https://raw.githubusercontent.com/VitalSkib/files/refs/heads/main/explorer-visual-tweaks-dark-text.png)
**Important:** Preview Pane tweaks require restarting File Explorer and any
included host applications for changed values and colors to apply correctly.
A restart is also required after disabling the mod to fully restore the
default Preview Pane appearance.
To style an Explorer-based Open/Save dialog, add its host application to
Windhawk's Inclusion List.
*/
// ==/WindhawkModReadme==
// ==WindhawkModSettings==
/*
- itemsView:
    - customizeItemsView: true
      $name: Enable file list customization
      $description: Turn off to let Windows or another mod handle this block.
  $name: File list selection (ItemsView)
  $description: Selection backgrounds and ghost-highlight correction in the file list.
- navigationPane:
    - customizeNavigationPane: true
      $name: Enable Navigation Pane customization
      $description: Turn off to let Windows or another mod handle this block.
    - showFocusPill: true
      $name: Show focus pill
      $description: Draws the vertical focus indicator in the Navigation Pane.
    - focusPillColor: "4CC2FF"
      $name: Focus pill color (RRGGBB)
  $name: Navigation Pane
  $description: Selection backgrounds and focus indicator in the left navigation pane.
- selectionAppearance:
    - cornerRadius: 2
      $name: Selection corner radius (0-6)
      $description: Visual corner rounding of selection backgrounds.
    - showBorder: true
      $name: Show selection border
      $description: Draws a 1 px border inside the selection background.
    - activeFillColor: "4D4D4D"
      $name: Active fill color (RRGGBB)
    - activeBorderColor: "555555"
      $name: Active border color (RRGGBB)
    - multiFillColor: "454545"
      $name: Multi-select fill color (RRGGBB)
    - multiBorderColor: "505050"
      $name: Multi-select border color (RRGGBB)
  $name: Shared selection appearance
  $description: Colors, borders, and rounding for both enabled selection blocks above.
- progress:
    - customizeProgress: true
      $name: Enable progress customization
      $description: Turn off to let Windows or another mod handle this block.
    - radius: 4
      $name: Progress radius (0-6)
    - fillLeft: "0078D7"
      $name: Progress fill left (RRGGBB)
    - fillRight: "0094FE"
      $name: Progress fill right (RRGGBB)
    - fullLeft: "E43060"
      $name: Full progress left (RRGGBB)
    - fullRight: "ED6050"
      $name: Full progress right (RRGGBB)
    - background: "454545"
      $name: Progress background (RRGGBB)
    - showProgressBorder: true
      $name: Show progress border
      $description: Draws the background border and keeps the fill inset inside it.
    - backgroundBorder: "323232"
      $name: Progress border (RRGGBB)
  $name: Progress indicators
  $description: Progress colors, gradients, borders, and rounding.
- previewPane:
    - matchDetailsPaneBg: true
      $name: Enable Preview Pane customization
      $description: Restart Explorer and included host applications after enabling or disabling.
    - previewPaneBgColor: "191919"
      $name: Preview Pane background color (RRGGBB)
      $description: Shared Preview Pane and text-preview background color.
  $name: Preview Pane
  $description: Background and plain-text preview in dark app mode. Restart Explorer after changing or disabling this block.
*/
// ==/WindhawkModSettings==
#include <windows.h>
#include <uxtheme.h>
#include <vssym32.h>
#include <algorithm>
#include <atomic>
#include <cmath>
#include <cwchar>
#include <utility>
#ifndef _ReturnAddress
#define _ReturnAddress() __builtin_return_address(0)
#endif
#ifndef TMT_FILLCOLOR
#define TMT_FILLCOLOR 3802
#endif
// Shared
using DrawThemeBackground_t = HRESULT (WINAPI *)(
    HTHEME, HDC, int, int, const RECT*, const RECT*);
using DrawThemeBackgroundEx_t = HRESULT (WINAPI *)(
    HTHEME, HDC, int, int, const RECT*, const DTBGOPTS*);
using GetThemeClass_t = HRESULT (WINAPI *)(HTHEME, LPWSTR, int);
using PaintBackground_t = void (*)(void*, HDC, void*, const RECT*,
                                   const RECT*, const RECT*, const RECT*);
using ElementBoolGetter_t = bool (*)(void*);
using ElementPointerGetter_t = void* (*)(void*);
using FocusedElementGetter_t = void* (*)();
using AllowDarkModeForWindow_t = BOOL (WINAPI *)(HWND, BOOL);
using SetPreferredAppMode_t = DWORD (WINAPI *)(DWORD);
enum class HostKind { Unsupported, Explorer, Prevhost, DialogHost };
static HostKind g_hostKind = HostKind::Unsupported;
static DrawThemeBackground_t g_origDrawThemeBackground = nullptr;
static DrawThemeBackgroundEx_t g_origDrawThemeBackgroundEx = nullptr;
static GetThemeClass_t g_getThemeClass = nullptr;
static AllowDarkModeForWindow_t g_allowDarkModeForWindow = nullptr;
static SetPreferredAppMode_t g_setPreferredAppMode = nullptr;
static constexpr DWORD kForceDarkAppMode = 2;
struct RgbColor {
    BYTE r;
    BYTE g;
    BYTE b;
};
static bool IsThemeClass(HTHEME theme, PCWSTR expected) {
    wchar_t className[64] = {};
    if (!g_getThemeClass ||
        FAILED(g_getThemeClass(
            theme, className, static_cast<int>(ARRAYSIZE(className))))) {
        return false;
    }
    size_t classLength = wcslen(className);
    size_t expectedLength = wcslen(expected);
    if (classLength < expectedLength)
        return false;
    PCWSTR tail = className + classLength - expectedLength;
    if (_wcsicmp(tail, expected) != 0)
        return false;
    return tail == className ||
           (tail - className >= 2 && tail[-2] == L':' && tail[-1] == L':');
}
static int GetSafeDpi(HDC dc) {
    if (!dc)
        return 96;
    HWND window = WindowFromDC(dc);
    if (window) {
        UINT dpi = GetDpiForWindow(window);
        if (dpi)
            return static_cast<int>(dpi);
    }
    int dpi = GetDeviceCaps(dc, LOGPIXELSY);
    return dpi > 0 ? dpi : 96;
}
static bool ParseRgbColor(PCWSTR text, RgbColor& color) {
    if (!text)
        return false;
    if (*text == L'#')
        ++text;
    if (wcslen(text) != 6)
        return false;
    wchar_t* end = nullptr;
    unsigned long value = wcstoul(text, &end, 16);
    if (!end || *end)
        return false;
    color = {static_cast<BYTE>(value >> 16),
             static_cast<BYTE>(value >> 8),
             static_cast<BYTE>(value)};
    return true;
}
static RgbColor LoadRgbColor(PCWSTR name, RgbColor fallback) {
    PCWSTR value = Wh_GetStringSetting(name);
    ParseRgbColor(value, fallback);
    if (value)
        Wh_FreeStringSetting(value);
    return fallback;
}
static COLORREF LoadColorRef(PCWSTR name, COLORREF fallback) {
    RgbColor color = LoadRgbColor(
        name, {GetRValue(fallback), GetGValue(fallback), GetBValue(fallback)});
    return RGB(color.r, color.g, color.b);
}
static bool ResolveUxThemeFunctions(HMODULE uxTheme, bool requireDarkMode) {
    // Private UxTheme exports used on Windows 11:
    // https://github.com/winsiderss/systeminformer/blob/1ab5a1b98bf1de5ea92d189a29d1f28c476e27a4/phlib/guisup.c#L128-L139
    g_getThemeClass = reinterpret_cast<GetThemeClass_t>(
        GetProcAddress(uxTheme, MAKEINTRESOURCEA(74)));
    g_allowDarkModeForWindow = reinterpret_cast<AllowDarkModeForWindow_t>(
        GetProcAddress(uxTheme, MAKEINTRESOURCEA(133)));
    g_setPreferredAppMode = reinterpret_cast<SetPreferredAppMode_t>(
        GetProcAddress(uxTheme, MAKEINTRESOURCEA(135)));
    if (!g_getThemeClass ||
        (requireDarkMode &&
         (!g_allowDarkModeForWindow || !g_setPreferredAppMode))) {
        Wh_Log(L"[ERROR] Couldn't resolve required uxtheme.dll exports");
        return false;
    }
    return true;
}
// ============================================================================
// Drive progress renderer
// ============================================================================
namespace Progress {
enum ResourceId { Normal, Full, Background, ResourceCount };
struct Resource {
    int part;
    int state;
    RgbColor colorA;
    RgbColor colorB;
};
Resource g_resources[ResourceCount] = {
    {PP_FILL, PBFS_PARTIAL, {0x00, 0x78, 0xD7}, {0x00, 0x94, 0xFE}},
    {PP_FILL, PBFS_ERROR, {0xE4, 0x30, 0x60}, {0xED, 0x60, 0x50}},
    {PP_TRANSPARENTBAR, PBBS_PARTIAL,
     {0x45, 0x45, 0x45}, {0x32, 0x32, 0x32}},
};
constexpr int kCacheCapacity = 32;
struct CacheEntry {
    ResourceId resource;
    int width;
    int height;
    int radius;
    int inset;
    HBITMAP bitmap;
};
CacheEntry g_cache[kCacheCapacity] = {};
int g_cacheSize;
int g_radius = 4;
bool g_showProgressBorder = true;
std::atomic<bool> g_enabled{true};
bool g_initialized;
SRWLOCK g_lock = SRWLOCK_INIT;
volatile LONG g_renderFailureLogged;
ResourceId FindResource(int part, int state) {
    for (int i = 0; i < ResourceCount; ++i) {
        if (g_resources[i].part == part && g_resources[i].state == state)
            return static_cast<ResourceId>(i);
    }
    return ResourceCount;
}
bool IsProgressTheme(HTHEME theme) {
    return IsThemeClass(theme, L"PROGRESS");
}
bool InsideRoundedRect(float x, float y, float left, float top, float right,
                       float bottom, float radius) {
    if (x < left || y < top || x >= right || y >= bottom)
        return false;
    if (radius <= 0)
        return true;
    radius = std::min(radius, std::min((right - left) / 2,
                                      (bottom - top) / 2));
    float cx = std::clamp(x, left + radius, right - radius);
    float cy = std::clamp(y, top + radius, bottom - radius);
    float dx = x - cx;
    float dy = y - cy;
    return dx * dx + dy * dy <= radius * radius;
}
DWORD Premultiply(float r, float g, float b, float alpha) {
    BYTE a = static_cast<BYTE>(std::lround(alpha * 255));
    BYTE pr = static_cast<BYTE>(std::clamp(std::lround(r * alpha), 0l, 255l));
    BYTE pg = static_cast<BYTE>(std::clamp(std::lround(g * alpha), 0l, 255l));
    BYTE pb = static_cast<BYTE>(std::clamp(std::lround(b * alpha), 0l, 255l));
    return (DWORD(a) << 24) | (DWORD(pr) << 16) | (DWORD(pg) << 8) | pb;
}
HBITMAP CreateResourceBitmap(ResourceId id, int width, int height,
                             int radius, int inset) {
    if (width <= 0 || height <= 0)
        return nullptr;
    BITMAPINFO bmi = {};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = width;
    bmi.bmiHeader.biHeight = -height;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;
    void* bits = nullptr;
    HBITMAP bitmap =
        CreateDIBSection(nullptr, &bmi, DIB_RGB_COLORS, &bits, nullptr, 0);
    if (!bitmap || !bits) {
        if (bitmap)
            DeleteObject(bitmap);
        return nullptr;
    }
    const Resource& resource = g_resources[id];
    auto* pixels = static_cast<DWORD*>(bits);
    std::fill_n(pixels, static_cast<size_t>(width) * height, 0);
    constexpr int kScale = 8;
    constexpr int kSamples = kScale * kScale;
    radius = std::min(radius, std::min(width, height) / 2);
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            float sumR = 0, sumG = 0, sumB = 0, coverage = 0;
            for (int sy = 0; sy < kScale; ++sy) {
                for (int sx = 0; sx < kScale; ++sx) {
                    float fx = x + (sx + 0.5f) / kScale;
                    float fy = y + (sy + 0.5f) / kScale;
                    float sampleR, sampleG, sampleB;
                    if (id == Background) {
                        if (!InsideRoundedRect(fx, fy, 0, 0,
                                               static_cast<float>(width),
                                               static_cast<float>(height),
                                               radius)) {
                            continue;
                        }
                        bool inner = InsideRoundedRect(
                            fx, fy, static_cast<float>(inset),
                            static_cast<float>(inset),
                            static_cast<float>(width - inset),
                            static_cast<float>(height - inset),
                            std::max(0, radius - inset));
                        const RgbColor& color =
                            inner ? resource.colorA : resource.colorB;
                        sampleR = color.r;
                        sampleG = color.g;
                        sampleB = color.b;
                    } else {
                        if (!InsideRoundedRect(
                                fx, fy, static_cast<float>(inset),
                                static_cast<float>(inset),
                                static_cast<float>(width - inset),
                                static_cast<float>(height - inset), radius)) {
                            continue;
                        }
                        int gradientWidth = width - 2 * inset;
                        float t = gradientWidth > 0
                                      ? std::clamp((fx - inset) / gradientWidth,
                                                   0.0f, 1.0f)
                                      : 0.0f;
                        sampleR = resource.colorA.r +
                                  (resource.colorB.r - resource.colorA.r) * t;
                        sampleG = resource.colorA.g +
                                  (resource.colorB.g - resource.colorA.g) * t;
                        sampleB = resource.colorA.b +
                                  (resource.colorB.b - resource.colorA.b) * t;
                    }
                    sumR += sampleR;
                    sumG += sampleG;
                    sumB += sampleB;
                    ++coverage;
                }
            }
            if (coverage) {
                pixels[static_cast<size_t>(y) * width + x] = Premultiply(
                    sumR / coverage, sumG / coverage, sumB / coverage,
                    coverage / kSamples);
            }
        }
    }
    return bitmap;
}
void ClearCacheLocked() {
    for (int i = 0; i < g_cacheSize; ++i)
        DeleteObject(g_cache[i].bitmap);
    g_cacheSize = 0;
}
HBITMAP GetBitmapLocked(ResourceId id, int width, int height,
                        int radius, int inset) {
    for (int i = 0; i < g_cacheSize; ++i) {
        const CacheEntry& entry = g_cache[i];
        if (entry.resource == id && entry.width == width &&
            entry.height == height && entry.radius == radius &&
            entry.inset == inset) {
            return entry.bitmap;
        }
    }
    if (g_cacheSize == kCacheCapacity)
        ClearCacheLocked();
    HBITMAP bitmap =
        CreateResourceBitmap(id, width, height, radius, inset);
    if (!bitmap)
        return nullptr;
    g_cache[g_cacheSize++] = {id, width, height, radius, inset, bitmap};
    return bitmap;
}
bool PaintResourceLocked(HDC hdc, const RECT& rect, const RECT* clip,
                         ResourceId id) {
    int width = rect.right - rect.left;
    int height = rect.bottom - rect.top;
    int dpi = GetSafeDpi(hdc);
    int radius = std::max(0, MulDiv(g_radius, dpi, 96));
    int inset = g_showProgressBorder
                    ? std::max(1, MulDiv(1, dpi, 96))
                    : 0;
    int minimumDimension = std::min(width, height);
    radius = std::min(radius, minimumDimension / 2);
    inset = std::min(inset, std::max(0, (minimumDimension - 1) / 2));
    HBITMAP bitmap = GetBitmapLocked(id, width, height, radius, inset);
    if (!bitmap)
        return false;
    // Bitmaps are cached; a per-call DC avoids sharing mutable HDC state
    // between Explorer UI threads.
    HDC sourceDc = CreateCompatibleDC(hdc);
    if (!sourceDc)
        return false;
    HGDIOBJ previousBitmap = SelectObject(sourceDc, bitmap);
    if (!previousBitmap || previousBitmap == HGDI_ERROR) {
        DeleteDC(sourceDc);
        return false;
    }
    SetLayout(sourceDc, 0);
    int savedDc = SaveDC(hdc);
    if (!savedDc) {
        SelectObject(sourceDc, previousBitmap);
        DeleteDC(sourceDc);
        return false;
    }
    bool clipOk = !clip ||
                  IntersectClipRect(hdc, clip->left, clip->top,
                                    clip->right, clip->bottom) != ERROR;
    BLENDFUNCTION blend = {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
    bool drawn = clipOk && AlphaBlend(
        hdc, rect.left, rect.top, width, height, sourceDc, 0, 0,
        width, height, blend);
    RestoreDC(hdc, savedDc);
    SelectObject(sourceDc, previousBitmap);
    DeleteDC(sourceDc);
    return drawn;
}
void LoadSettings() {
    g_enabled.store(Wh_GetIntSetting(L"progress.customizeProgress") != 0);
    g_radius = std::clamp(Wh_GetIntSetting(L"progress.radius"), 0, 6);
    g_showProgressBorder =
        Wh_GetIntSetting(L"progress.showProgressBorder") != 0;
    g_resources[Normal].colorA =
        LoadRgbColor(L"progress.fillLeft", g_resources[Normal].colorA);
    g_resources[Normal].colorB =
        LoadRgbColor(L"progress.fillRight", g_resources[Normal].colorB);
    g_resources[Full].colorA =
        LoadRgbColor(L"progress.fullLeft", g_resources[Full].colorA);
    g_resources[Full].colorB =
        LoadRgbColor(L"progress.fullRight", g_resources[Full].colorB);
    g_resources[Background].colorA =
        LoadRgbColor(L"progress.background", g_resources[Background].colorA);
    g_resources[Background].colorB =
        LoadRgbColor(L"progress.backgroundBorder", g_resources[Background].colorB);
}
bool TryDraw(HTHEME theme, HDC hdc, int part, int state,
             const RECT* rect, const RECT* clip) {
    if (!g_enabled.load())
        return false;
    ResourceId id = FindResource(part, state);
    if (id == ResourceCount || !hdc || !rect || !IsProgressTheme(theme))
        return false;
    RECT destination = *rect;
    if (destination.left > destination.right)
        std::swap(destination.left, destination.right);
    if (destination.top > destination.bottom)
        std::swap(destination.top, destination.bottom);
    if (IsRectEmpty(&destination))
        return true;
    AcquireSRWLockExclusive(&g_lock);
    if (!g_initialized || !g_enabled.load()) {
        ReleaseSRWLockExclusive(&g_lock);
        return false;
    }
    bool drawn = PaintResourceLocked(hdc, destination, clip, id);
    ReleaseSRWLockExclusive(&g_lock);
    if (drawn)
        return true;
    if (InterlockedCompareExchange(&g_renderFailureLogged, 1, 0) == 0)
        Wh_Log(L"[ERROR] Progress drawing failed; using native fallback");
    return false;
}
void Initialize() {
    AcquireSRWLockExclusive(&g_lock);
    LoadSettings();
    ClearCacheLocked();
    g_initialized = true;
    ReleaseSRWLockExclusive(&g_lock);
}
void ReloadSettings() {
    Initialize();
    InterlockedExchange(&g_renderFailureLogged, 0);
}
void Uninitialize() {
    AcquireSRWLockExclusive(&g_lock);
    g_initialized = false;
    ClearCacheLocked();
    ReleaseSRWLockExclusive(&g_lock);
}
}  // namespace Progress
// ============================================================================
// Selection highlights
// ============================================================================
namespace Selection {
// Reverse-engineered ItemsView identifiers; no SDK constants exist.
static constexpr int kItemsViewTransitionPart = 3;
static constexpr int kItemsViewPendingSelectionState = 2;
static PaintBackground_t g_origPaintBackground = nullptr;
static ElementBoolGetter_t g_getSelected = nullptr;
static ElementBoolGetter_t g_getMouseFocused = nullptr;
static ElementPointerGetter_t g_getParent = nullptr;
static FocusedElementGetter_t g_getKeyFocusedElement = nullptr;
static SRWLOCK g_resourceLock = SRWLOCK_INIT;
static bool g_active = false;
static std::atomic<bool> g_itemsViewEnabled{true};
static std::atomic<bool> g_navigationPaneEnabled{true};
static int g_borderWidth = 1;
static HDC g_backgroundDc[2] = {nullptr, nullptr};
static HBITMAP g_backgroundBitmap[2] = {nullptr, nullptr};
static int g_ninePatchMargin = 4;
static HDC g_focusPillDc = nullptr;
static HBITMAP g_focusPillBitmap = nullptr;
static constexpr int kFocusPillWidth = 3;
static constexpr int kFocusPillVerticalInset = 3;
static constexpr int kFocusPillCapHeight = 2;
// Minimal 3-slice sample: top cap, one middle row, and bottom cap.
static constexpr int kFocusPillSourceHeight =
    2 * kFocusPillCapHeight + 1;
static thread_local void* g_currentPaintElement = nullptr;
struct BitmapResource {
    HDC dc = nullptr;
    HBITMAP bitmap = nullptr;
    ~BitmapResource() {
        if (dc)
            DeleteDC(dc);
        if (bitmap)
            DeleteObject(bitmap);
    }
    bool Create(int width, int height, void** bits) {
        HDC screenDc = GetDC(nullptr);
        if (!screenDc)
            return false;
        dc = CreateCompatibleDC(screenDc);
        ReleaseDC(nullptr, screenDc);
        if (!dc)
            return false;
        BITMAPINFO info = {};
        info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        info.bmiHeader.biWidth = width;
        info.bmiHeader.biHeight = -height;
        info.bmiHeader.biPlanes = 1;
        info.bmiHeader.biBitCount = 32;
        info.bmiHeader.biCompression = BI_RGB;
        bitmap = CreateDIBSection(
            dc, &info, DIB_RGB_COLORS, bits, nullptr, 0);
        if (!bitmap || !*bits)
            return false;
        HGDIOBJ previous = SelectObject(dc, bitmap);
        return previous && previous != HGDI_ERROR;
    }
    void Swap(HDC& otherDc, HBITMAP& otherBitmap) {
        std::swap(dc, otherDc);
        std::swap(bitmap, otherBitmap);
    }
};
static bool InitCustomNinePatch(BitmapResource& resource, int radius,
                                int margin, int borderWidth,
                                RgbColor fill, RgbColor border) {
    const int size = 2 * margin + 3;
    void* bits = nullptr;
    if (!resource.Create(size, size, &bits))
        return false;
    BYTE* pixels = static_cast<BYTE*>(bits);
    for (int y = 0; y < size; y++) {
        for (int x = 0; x < size; x++) {
            bool inCenterX = x >= margin && x <= margin + 2;
            bool inCenterY = y >= margin && y <= margin + 2;
            float insideDistance;
            if (radius == 0 || inCenterX || inCenterY) {
                int dx = std::min(x, 2 * margin + 2 - x);
                int dy = std::min(y, 2 * margin + 2 - y);
                insideDistance = static_cast<float>(std::min(dx, dy));
            } else {
                float centerX = x < margin
                                    ? static_cast<float>(margin)
                                    : static_cast<float>(margin + 2);
                float centerY = y < margin
                                    ? static_cast<float>(margin)
                                    : static_cast<float>(margin + 2);
                float dx = fabsf(static_cast<float>(x) - centerX) - 0.5f;
                float dy = fabsf(static_cast<float>(y) - centerY) - 0.5f;
                insideDistance =
                    static_cast<float>(margin) - sqrtf(dx * dx + dy * dy);
            }
            float alpha = std::max(0.0f, std::min(1.0f, insideDistance + 1.0f));
            bool isBorder = borderWidth > 0 &&
                            insideDistance < static_cast<float>(borderWidth);
            BYTE red = isBorder ? border.r : fill.r;
            BYTE green = isBorder ? border.g : fill.g;
            BYTE blue = isBorder ? border.b : fill.b;
            BYTE a = static_cast<BYTE>(255.0f * alpha + 0.5f);
            int offset = (y * size + x) * 4;
            pixels[offset + 0] = static_cast<BYTE>((blue * a) / 255);
            pixels[offset + 1] = static_cast<BYTE>((green * a) / 255);
            pixels[offset + 2] = static_cast<BYTE>((red * a) / 255);
            pixels[offset + 3] = a;
        }
    }
    return true;
}
static bool InitFocusPill(BitmapResource& resource, RgbColor color) {
    void* bits = nullptr;
    if (!resource.Create(kFocusPillWidth, kFocusPillSourceHeight, &bits))
        return false;
    constexpr float radius = kFocusPillWidth / 2.0f;
    BYTE* pixels = static_cast<BYTE*>(bits);
    for (int y = 0; y < kFocusPillSourceHeight; y++) {
        for (int x = 0; x < kFocusPillWidth; x++) {
            float pixelX = x + 0.5f;
            float pixelY = y + 0.5f;
            float centerX = std::max(
                radius,
                std::min(static_cast<float>(kFocusPillWidth) - radius,
                         pixelX));
            float centerY = std::max(
                radius,
                std::min(static_cast<float>(kFocusPillSourceHeight) - radius,
                         pixelY));
            float dx = pixelX - centerX;
            float dy = pixelY - centerY;
            float insideDistance = radius - sqrtf(dx * dx + dy * dy);
            float alpha = std::max(0.0f, std::min(1.0f, insideDistance + 0.5f));
            BYTE a = static_cast<BYTE>(255.0f * alpha + 0.5f);
            int offset = (y * kFocusPillWidth + x) * 4;
            pixels[offset + 0] = static_cast<BYTE>((color.b * a) / 255);
            pixels[offset + 1] = static_cast<BYTE>((color.g * a) / 255);
            pixels[offset + 2] = static_cast<BYTE>((color.r * a) / 255);
            pixels[offset + 3] = a;
        }
    }
    return true;
}
static void DrawNinePatch(HDC dc, const RECT* rect, int index) {
    if (index < 0 || index > 1 || !g_backgroundDc[index]) {
        return;
    }
    int destinationX = rect->left;
    int destinationY = rect->top;
    int destinationWidth = rect->right - rect->left;
    int destinationHeight = rect->bottom - rect->top;
    if (destinationWidth <= 0 || destinationHeight <= 0) {
        return;
    }
    const int sourceMargin = g_ninePatchMargin;
    HDC sourceDc = g_backgroundDc[index];
    BLENDFUNCTION blend = {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
    int dpi = GetSafeDpi(dc);
    int scaledMargin = MulDiv(sourceMargin, dpi, 96);
    // Keep corner tiles square when the radius exceeds half the
    // destination height; stretching the whole sample creates an oval.
    int effectiveMargin = std::min(
        {scaledMargin, destinationWidth / 2, destinationHeight / 2});
    int middleWidth = destinationWidth - effectiveMargin * 2;
    int middleHeight = destinationHeight - effectiveMargin * 2;
    AlphaBlend(dc, destinationX, destinationY, effectiveMargin,
               effectiveMargin, sourceDc, 0, 0, sourceMargin, sourceMargin,
               blend);
    AlphaBlend(dc, destinationX + destinationWidth - effectiveMargin,
               destinationY, effectiveMargin, effectiveMargin, sourceDc,
               sourceMargin + 3, 0, sourceMargin, sourceMargin, blend);
    AlphaBlend(dc, destinationX,
               destinationY + destinationHeight - effectiveMargin,
               effectiveMargin, effectiveMargin, sourceDc, 0, sourceMargin + 3,
               sourceMargin, sourceMargin, blend);
    AlphaBlend(dc, destinationX + destinationWidth - effectiveMargin,
               destinationY + destinationHeight - effectiveMargin,
               effectiveMargin, effectiveMargin, sourceDc, sourceMargin + 3,
               sourceMargin + 3, sourceMargin, sourceMargin, blend);
    AlphaBlend(dc, destinationX + effectiveMargin, destinationY, middleWidth,
               effectiveMargin, sourceDc, sourceMargin + 1, 0, 1,
               sourceMargin, blend);
    AlphaBlend(dc, destinationX + effectiveMargin,
               destinationY + destinationHeight - effectiveMargin,
               middleWidth, effectiveMargin, sourceDc, sourceMargin + 1,
               sourceMargin + 3, 1, sourceMargin, blend);
    AlphaBlend(dc, destinationX, destinationY + effectiveMargin,
               effectiveMargin, middleHeight, sourceDc, 0, sourceMargin + 1,
               sourceMargin, 1, blend);
    AlphaBlend(dc, destinationX + destinationWidth - effectiveMargin,
               destinationY + effectiveMargin, effectiveMargin,
               middleHeight, sourceDc, sourceMargin + 3, sourceMargin + 1,
               sourceMargin, 1, blend);
    AlphaBlend(dc, destinationX + effectiveMargin,
               destinationY + effectiveMargin, middleWidth, middleHeight,
               sourceDc, sourceMargin + 1, sourceMargin + 1, 1, 1, blend);
}
static void DrawFocusPill(HDC dc, const RECT* rect) {
    if (!g_focusPillDc) {
        return;
    }
    int dpi = GetSafeDpi(dc);
    int destinationWidth = MulDiv(kFocusPillWidth, dpi, 96);
    int verticalInset = MulDiv(kFocusPillVerticalInset, dpi, 96);
    int capHeight = MulDiv(kFocusPillCapHeight, dpi, 96);
    int destinationHeight =
        rect->bottom - rect->top - 2 * verticalInset;
    if (destinationHeight <= 0 || destinationWidth <= 0) {
        return;
    }
    int x = rect->left + MulDiv(g_borderWidth, dpi, 96);
    int y = rect->top + verticalInset;
    BLENDFUNCTION blend = {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
    auto drawSegment = [&](int destinationY, int segmentHeight,
                           int sourceY, int sourceHeight) {
        AlphaBlend(dc, x, destinationY, destinationWidth, segmentHeight,
                   g_focusPillDc, 0, sourceY, kFocusPillWidth, sourceHeight,
                   blend);
    };
    if (destinationHeight <= 2 * capHeight) {
        drawSegment(y, destinationHeight, 0, kFocusPillSourceHeight);
        return;
    }
    int middleHeight = destinationHeight - 2 * capHeight;
    int sourceMiddleHeight =
        kFocusPillSourceHeight - 2 * kFocusPillCapHeight;
    drawSegment(y, capHeight, 0, kFocusPillCapHeight);
    drawSegment(y + capHeight, middleHeight, kFocusPillCapHeight,
                sourceMiddleHeight);
    drawSegment(y + capHeight + middleHeight, capHeight,
                kFocusPillSourceHeight - kFocusPillCapHeight,
                kFocusPillCapHeight);
}
template <typename T>
static bool ResolveDuiExport(HMODULE dui70, const char* name, T* target) {
    *target = reinterpret_cast<T>(GetProcAddress(dui70, name));
    if (!*target) {
        Wh_Log(L"[ERROR] Required DUI70 export wasn't found: %S", name);
        return false;
    }
    return true;
}
static bool ResolveDuiFunctions(HMODULE dui70) {
    void* paintBackground = reinterpret_cast<void*>(GetProcAddress(
        dui70,
        "?PaintBackground@Element@DirectUI@@QEAAXPEAUHDC__@@PEAVValue@2@"
        "AEBUtagRECT@@222@Z"));
    if (!paintBackground) {
        Wh_Log(L"[ERROR] DirectUI::Element::PaintBackground wasn't found");
        return false;
    }
    if (!ResolveDuiExport(
            dui70, "?GetSelected@Element@DirectUI@@QEAA_NXZ", &g_getSelected) ||
        !ResolveDuiExport(dui70,
                          "?GetMouseFocused@Element@DirectUI@@QEAA_NXZ",
                          &g_getMouseFocused) ||
        !ResolveDuiExport(dui70,
                          "?GetParent@Element@DirectUI@@QEAAPEAV12@XZ",
                          &g_getParent) ||
        !ResolveDuiExport(
            dui70,
            "?GetKeyFocusedElement@HWNDElement@DirectUI@@SAPEAVElement@2@XZ",
            &g_getKeyFocusedElement)) {
        return false;
    }
    if (!Wh_SetFunctionHook(
            paintBackground,
            reinterpret_cast<void*>(+[](void* element,
                                        HDC dc,
                                        void* backgroundValue,
                                        const RECT* rect1,
                                        const RECT* rect2,
                                        const RECT* rect3,
                                        const RECT* rect4) {
                void* previousElement = g_currentPaintElement;
                g_currentPaintElement = element;
                g_origPaintBackground(element, dc, backgroundValue,
                                      rect1, rect2, rect3, rect4);
                g_currentPaintElement = previousElement;
            }),
            reinterpret_cast<void**>(&g_origPaintBackground))) {
        Wh_Log(L"[ERROR] Failed to install the DirectUI Element hook");
        return false;
    }
    return true;
}
static void DrawResources(HDC dc, const RECT* rect, int backgroundIndex,
                          bool drawPill) {
    AcquireSRWLockExclusive(&g_resourceLock);
    if (backgroundIndex >= 0) {
        DrawNinePatch(dc, rect, backgroundIndex);
    }
    if (drawPill) {
        DrawFocusPill(dc, rect);
    }
    ReleaseSRWLockExclusive(&g_resourceLock);
}
HRESULT HandleDrawThemeBackground(HTHEME theme, HDC dc, int partId,
                                  int stateId, const RECT* rect,
                                  const RECT* clipRect) {
    if (!g_navigationPaneEnabled.load() || !rect || partId != TVP_TREEITEM ||
        (stateId != TREIS_HOT && stateId != TREIS_SELECTED &&
         stateId != TREIS_SELECTEDNOTFOCUS &&
         stateId != TREIS_HOTSELECTED) ||
        !IsThemeClass(theme, L"TreeView")) {
        return g_origDrawThemeBackground(
            theme, dc, partId, stateId, rect, clipRect);
    }
    if (stateId == TREIS_HOT) {
        DrawResources(dc, rect, 0, false);
    } else if (stateId == TREIS_SELECTED) {
        bool ctrlOrShift = (GetAsyncKeyState(VK_CONTROL) & 0x8000) ||
                           (GetAsyncKeyState(VK_SHIFT) & 0x8000);
        if (ctrlOrShift) {
            DrawResources(dc, rect, 1, false);
        }
        // Suppress Explorer's stale previous-row transition.
    } else if (stateId == TREIS_SELECTEDNOTFOCUS) {
        return g_origDrawThemeBackground(
            theme, dc, partId, stateId, rect, clipRect);
    } else {  // TREIS_HOTSELECTED
        DrawResources(dc, rect, 0, true);
    }
    return S_OK;
}
static void* GetPendingMouseSelectionElement() {
    void* focusedElement = g_getKeyFocusedElement();
    if (!focusedElement || g_getSelected(focusedElement) ||
        !g_getMouseFocused(focusedElement)) {
        return nullptr;
    }
    return focusedElement;
}
static bool IsTransientSingleSelectionGhost() {
    void* oldElement = g_currentPaintElement;
    if (!oldElement)
        return false;
    void* newElement = GetPendingMouseSelectionElement();
    return newElement && newElement != oldElement &&
           g_getParent(newElement) == g_getParent(oldElement);
}
HRESULT WINAPI DrawThemeBackgroundExHook(HTHEME theme,
                                         HDC dc,
                                         int partId,
                                         int stateId,
                                         const RECT* rect,
                                         const DTBGOPTS* options) {
    if (!g_itemsViewEnabled.load() || !rect) {
        return g_origDrawThemeBackgroundEx(
            theme, dc, partId, stateId, rect, options);
    }
    if (partId == kItemsViewTransitionPart &&
        IsThemeClass(theme, L"ItemsView")) {
        // Suppress the native transition/focus layer; replace only
        // the matching, non-null pending item.
        void* currentElement = g_currentPaintElement;
        void* pending =
            stateId == kItemsViewPendingSelectionState && currentElement
                ? GetPendingMouseSelectionElement() : nullptr;
        if (pending && pending == currentElement) {
            DrawResources(dc, rect, 0, false);
        }
        return S_OK;
    }
    if (partId != LVP_LISTITEM ||
        (stateId != LISS_HOT && stateId != LISS_SELECTED &&
         stateId != LISS_SELECTEDNOTFOCUS &&
         stateId != LISS_HOTSELECTED) ||
        !IsThemeClass(theme, L"ListView")) {
        return g_origDrawThemeBackgroundEx(
            theme, dc, partId, stateId, rect, options);
    }
    if (stateId == LISS_HOT) {
        DrawResources(dc, rect, 0, false);
        return S_OK;
    }
    if (stateId == LISS_SELECTED) {
        if ((GetAsyncKeyState(VK_CONTROL) & 0x8000) ||
            (GetAsyncKeyState(VK_SHIFT) & 0x8000)) {
            DrawResources(dc, rect, 1, false);
            return S_OK;
        }
        if (IsTransientSingleSelectionGhost()) {
            return S_OK;
        }
        DrawResources(dc, rect, 1, false);
        return S_OK;
    }
    if (stateId == LISS_SELECTEDNOTFOCUS) {
        return g_origDrawThemeBackgroundEx(
            theme, dc, partId, stateId, rect, options);
    }
    DrawResources(dc, rect, 0, false);  // LISS_HOTSELECTED
    return S_OK;
}
static bool ApplySettings() {
    bool itemsViewEnabled = Wh_GetIntSetting(L"itemsView.customizeItemsView") != 0;
    bool navigationPaneEnabled =
        Wh_GetIntSetting(L"navigationPane.customizeNavigationPane") != 0;
    g_itemsViewEnabled.store(itemsViewEnabled);
    g_navigationPaneEnabled.store(navigationPaneEnabled);
    int radius = Wh_GetIntSetting(L"selectionAppearance.cornerRadius");
    radius = std::clamp(radius, 0, 6);
    int sampleRadius = radius * 2;
    int borderWidth = Wh_GetIntSetting(L"selectionAppearance.showBorder") ? 1 : 0;
    int margin = std::max(sampleRadius, borderWidth);
    RgbColor activeFill =
        LoadRgbColor(L"selectionAppearance.activeFillColor", {0x4D, 0x4D, 0x4D});
    RgbColor activeBorder =
        LoadRgbColor(L"selectionAppearance.activeBorderColor", {0x55, 0x55, 0x55});
    RgbColor multiFill =
        LoadRgbColor(L"selectionAppearance.multiFillColor", {0x45, 0x45, 0x45});
    RgbColor multiBorder =
        LoadRgbColor(L"selectionAppearance.multiBorderColor", {0x50, 0x50, 0x50});
    bool showFocusPill = Wh_GetIntSetting(L"navigationPane.showFocusPill") != 0;
    RgbColor pill = {};
    if (showFocusPill) {
        pill = LoadRgbColor(L"navigationPane.focusPillColor", {0x4C, 0xC2, 0xFF});
    }
    BitmapResource active, multi, focus;
    if (!InitCustomNinePatch(active, sampleRadius, margin, borderWidth,
                             activeFill, activeBorder) ||
        !InitCustomNinePatch(multi, sampleRadius, margin, borderWidth,
                             multiFill, multiBorder) ||
        (showFocusPill && !InitFocusPill(focus, pill))) {
        Wh_Log(L"[ERROR] Selection resources couldn't be created");
        return false;
    }
    AcquireSRWLockExclusive(&g_resourceLock);
    active.Swap(g_backgroundDc[0], g_backgroundBitmap[0]);
    multi.Swap(g_backgroundDc[1], g_backgroundBitmap[1]);
    focus.Swap(g_focusPillDc, g_focusPillBitmap);
    g_ninePatchMargin = margin;
    g_borderWidth = borderWidth;
    ReleaseSRWLockExclusive(&g_resourceLock);
    return true;
}
bool Initialize(HMODULE dui70) {
    if (!ResolveDuiFunctions(dui70) || !ApplySettings())
        return false;
    g_active = true;
    return true;
}
void ReloadSettings() {
    if (g_active)
        ApplySettings();
}
void Uninitialize() {
    if (!g_active)
        return;
    AcquireSRWLockExclusive(&g_resourceLock);
    BitmapResource active, multi, focus;
    active.Swap(g_backgroundDc[0], g_backgroundBitmap[0]);
    multi.Swap(g_backgroundDc[1], g_backgroundBitmap[1]);
    focus.Swap(g_focusPillDc, g_focusPillBitmap);
    ReleaseSRWLockExclusive(&g_resourceLock);
    g_active = false;
}
}  // namespace Selection
// ============================================================================
// Preview Pane
// ============================================================================
namespace Preview {
using GetThemeColor_t =
    HRESULT (WINAPI *)(HTHEME, int, int, int, COLORREF*);
static GetThemeColor_t GetThemeColor_orig = nullptr;
static std::atomic<HBRUSH> g_backgroundBrush{nullptr};
// Keeps unload cleanup from racing an in-flight class-brush handoff.
static SRWLOCK g_brushLock = SRWLOCK_INIT;
static bool g_isExplorer = false;
static bool g_isPrevhost = false;
static bool g_isDialogHost = false;
static bool g_fixActive = false;
static bool g_useDarkScrollbar = true;
static COLORREF g_previewBgColor = RGB(0x19, 0x19, 0x19);
static COLORREF g_previewTextColor = RGB(0xDC, 0xDC, 0xDC);
constexpr wchar_t kHostClass[] = L"Shell Preview Extension Host";
constexpr wchar_t kPreviewerClass[] =
    L"Shell Preview Extension Host Previewer";
constexpr wchar_t kRichEditClass[] = L"RICHEDIT50W";
constexpr int kReadingPaneBackgroundPart = 1;
void LoadSettings() {
    bool enabled = Wh_GetIntSetting(L"previewPane.matchDetailsPaneBg") != 0;
    g_previewBgColor = LoadColorRef(
        L"previewPane.previewPaneBgColor", RGB(0x19, 0x19, 0x19));
    int luminance = (GetRValue(g_previewBgColor) * 299 +
                     GetGValue(g_previewBgColor) * 587 +
                     GetBValue(g_previewBgColor) * 114) / 1000;
    g_useDarkScrollbar = luminance <= 128;
    g_previewTextColor = g_useDarkScrollbar
        ? RGB(0xDC, 0xDC, 0xDC)
        : RGB(0x20, 0x20, 0x20);
    g_fixActive = enabled;
}
// App theme is sampled only at startup.
bool DetectDarkTheme() {
    DWORD appsUseLightTheme = 1;
    DWORD size = sizeof(appsUseLightTheme);
    LONG result = RegGetValueW(
        HKEY_CURRENT_USER,
        L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
        L"AppsUseLightTheme", RRF_RT_REG_DWORD, nullptr,
        &appsUseLightTheme, &size);
    return result == ERROR_SUCCESS && appsUseLightTheme == 0;
}
bool IsInMsftedit(void* caller) {
    HMODULE callerModule = nullptr;
    if (!GetModuleHandleExW(
            GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS,
            reinterpret_cast<LPCWSTR>(caller),
            &callerModule)) {
        return false;
    }
    bool result = callerModule == GetModuleHandleW(L"msftedit.dll");
    FreeLibrary(callerModule);
    return result;
}
bool IsTargetClass(PCWSTR className) {
    if (!className || IS_INTRESOURCE(className))
        return false;
    return ((g_isExplorer || g_isDialogHost) &&
            _wcsicmp(className, kHostClass) == 0) ||
           (g_isPrevhost &&
            _wcsicmp(className, kPreviewerClass) == 0);
}
using RegisterClassExW_t = ATOM (WINAPI *)(const WNDCLASSEXW*);
static RegisterClassExW_t RegisterClassExW_orig = nullptr;
ATOM WINAPI RegisterClassExW_hook(const WNDCLASSEXW* input) {
    if (!g_fixActive || !input || !IsTargetClass(input->lpszClassName)) {
        return RegisterClassExW_orig(input);
    }
    AcquireSRWLockShared(&g_brushLock);
    HBRUSH brush = g_backgroundBrush.load(std::memory_order_acquire);
    if (!brush) {
        ReleaseSRWLockShared(&g_brushLock);
        return RegisterClassExW_orig(input);
    }
    WNDCLASSEXW copy = *input;
    copy.hbrBackground = brush;
    ATOM result = RegisterClassExW_orig(&copy);
    DWORD error = result ? ERROR_SUCCESS : GetLastError();
    if (result) {
        // Windows deletes class brushes on UnregisterClass; applications must
        // not delete them after successful registration:
        // https://learn.microsoft.com/windows/win32/api/winuser/ns-winuser-wndclassexw
        HBRUSH expected = brush;
        g_backgroundBrush.compare_exchange_strong(
            expected, nullptr, std::memory_order_acq_rel,
            std::memory_order_acquire);
    } else {
        if (error != ERROR_CLASS_ALREADY_EXISTS) {
            Wh_Log(L"[ERROR] RegisterClassExW failed for '%s'; error=%u",
                   copy.lpszClassName, error);
        }
    }
    ReleaseSRWLockShared(&g_brushLock);
    if (!result)
        SetLastError(error);
    return result;
}
using GetSysColor_t = COLORREF (WINAPI *)(int);
static GetSysColor_t GetSysColor_orig = nullptr;
COLORREF WINAPI GetSysColor_hook(int index) {
    if (g_fixActive && g_isPrevhost &&
        (index == COLOR_WINDOW || index == COLOR_WINDOWTEXT) &&
        IsInMsftedit(_ReturnAddress())) {
        return index == COLOR_WINDOWTEXT
            ? g_previewTextColor
            : g_previewBgColor;
    }
    return GetSysColor_orig(index);
}
using CreateWindowExW_t = HWND (WINAPI *)(
    DWORD, LPCWSTR, LPCWSTR, DWORD, int, int, int, int,
    HWND, HMENU, HINSTANCE, LPVOID);
static CreateWindowExW_t CreateWindowExW_orig = nullptr;
HWND WINAPI CreateWindowExW_hook(
    DWORD exStyle, LPCWSTR className, LPCWSTR windowName,
    DWORD style, int x, int y, int width, int height,
    HWND parent, HMENU menu, HINSTANCE instance, LPVOID param) {
    HWND window = CreateWindowExW_orig(
        exStyle, className, windowName, style,
        x, y, width, height, parent, menu, instance, param);
    if (window && g_fixActive && g_useDarkScrollbar &&
        g_isPrevhost && className &&
        !IS_INTRESOURCE(className) &&
        _wcsicmp(className, kRichEditClass) == 0) {
        if (g_allowDarkModeForWindow)
            g_allowDarkModeForWindow(window, TRUE);
        SetWindowTheme(window, L"Explorer", nullptr);
    }
    return window;
}
HRESULT WINAPI GetThemeColor_hook(HTHEME theme, int partId, int stateId,
                                  int propId, COLORREF* color) {
    HRESULT result = GetThemeColor_orig(
        theme, partId, stateId, propId, color);
    if (g_fixActive && result == S_OK && color &&
        partId == kReadingPaneBackgroundPart && propId == TMT_FILLCOLOR &&
        IsThemeClass(theme, L"ReadingPane")) {
        *color = g_previewBgColor;
    }
    return result;
}
void DeleteUnownedBrush() {
    AcquireSRWLockExclusive(&g_brushLock);
    HBRUSH brush = g_backgroundBrush.exchange(
        nullptr, std::memory_order_acq_rel);
    if (brush)
        DeleteObject(brush);
    ReleaseSRWLockExclusive(&g_brushLock);
}
bool Initialize(HostKind host, HMODULE uxTheme) {
    g_isExplorer = host == HostKind::Explorer;
    g_isPrevhost = host == HostKind::Prevhost;
    g_isDialogHost = host == HostKind::DialogHost;
    LoadSettings();
    g_fixActive = g_fixActive && DetectDarkTheme();
    bool needDarkModeFunctions =
        g_isPrevhost && g_fixActive && g_useDarkScrollbar;
    if (!ResolveUxThemeFunctions(uxTheme, needDarkModeFunctions))
        return false;
    void* getThemeColor =
        reinterpret_cast<void*>(GetProcAddress(uxTheme, "GetThemeColor"));
    if (!getThemeColor) {
        Wh_Log(L"[ERROR] GetThemeColor wasn't found");
        return false;
    }
    if (g_fixActive) {
        HBRUSH brush = CreateSolidBrush(g_previewBgColor);
        if (!brush) {
            Wh_Log(L"[ERROR] Preview background brush creation failed");
            return false;
        }
        g_backgroundBrush.store(brush, std::memory_order_release);
    }
    if (!Wh_SetFunctionHook(
            reinterpret_cast<void*>(RegisterClassExW),
            reinterpret_cast<void*>(RegisterClassExW_hook),
            reinterpret_cast<void**>(&RegisterClassExW_orig)) ||
        !Wh_SetFunctionHook(
            getThemeColor,
            reinterpret_cast<void*>(GetThemeColor_hook),
            reinterpret_cast<void**>(&GetThemeColor_orig))) {
        Wh_Log(L"[ERROR] Required Preview Pane hooks couldn't be installed");
        DeleteUnownedBrush();
        return false;
    }
    if (g_isPrevhost &&
        (!Wh_SetFunctionHook(
             reinterpret_cast<void*>(GetSysColor),
             reinterpret_cast<void*>(GetSysColor_hook),
             reinterpret_cast<void**>(&GetSysColor_orig)) ||
         !Wh_SetFunctionHook(
             reinterpret_cast<void*>(CreateWindowExW),
             reinterpret_cast<void*>(CreateWindowExW_hook),
             reinterpret_cast<void**>(&CreateWindowExW_orig)))) {
        Wh_Log(L"[ERROR] RichEdit hooks couldn't be installed");
        DeleteUnownedBrush();
        return false;
    }
    if (needDarkModeFunctions && g_setPreferredAppMode)
        g_setPreferredAppMode(kForceDarkAppMode);
    return true;
}
void Uninitialize() {
    DeleteUnownedBrush();
}
}  // namespace Preview
// ============================================================================
// Hook dispatch and lifecycle
// ============================================================================
static HRESULT WINAPI DrawThemeBackgroundHook(
    HTHEME theme, HDC hdc, int part, int state,
    const RECT* rect, const RECT* clip) {
    if (Progress::TryDraw(theme, hdc, part, state, rect, clip))
        return S_OK;
    return Selection::HandleDrawThemeBackground(
        theme, hdc, part, state, rect, clip);
}
static bool HasDesktopAccess() {
    HDC dc = GetDC(nullptr);
    if (!dc)
        return false;
    ReleaseDC(nullptr, dc);
    return true;
}
static BOOL FailInitialization() {
    Selection::Uninitialize();
    Progress::Uninitialize();
    Preview::Uninitialize();
    return FALSE;
}
BOOL Wh_ModInit() {
    wchar_t path[MAX_PATH] = {};
    GetModuleFileNameW(nullptr, path, ARRAYSIZE(path));
    const wchar_t* name = wcsrchr(path, L'\\');
    name = name ? name + 1 : path;
    if (_wcsicmp(name, L"explorer.exe") == 0)
        g_hostKind = HostKind::Explorer;
    else if (_wcsicmp(name, L"prevhost.exe") == 0)
        g_hostKind = HostKind::Prevhost;
    else {
        if (!HasDesktopAccess())
            return TRUE;
        g_hostKind = HostKind::DialogHost;
    }
    HMODULE uxTheme = GetModuleHandleW(L"uxtheme.dll");
    if (!uxTheme) {
        Wh_Log(L"[ERROR] uxtheme.dll isn't loaded");
        return FALSE;
    }
    if (!Preview::Initialize(g_hostKind, uxTheme))
        return FailInitialization();
    if (g_hostKind != HostKind::Prevhost) {
        Progress::Initialize();
        HMODULE dui70 = LoadLibraryExW(
            L"DUI70.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
        if (!dui70) {
            Wh_Log(L"[ERROR] DUI70.dll couldn't be loaded");
            return FailInitialization();
        }
        if (!Selection::Initialize(dui70)) {
            FreeLibrary(dui70);
            return FailInitialization();
        }
        if (!Wh_SetFunctionHook(
                reinterpret_cast<void*>(DrawThemeBackground),
                reinterpret_cast<void*>(DrawThemeBackgroundHook),
                reinterpret_cast<void**>(&g_origDrawThemeBackground)) ||
            !Wh_SetFunctionHook(
                reinterpret_cast<void*>(DrawThemeBackgroundEx),
                reinterpret_cast<void*>(Selection::DrawThemeBackgroundExHook),
                reinterpret_cast<void**>(&g_origDrawThemeBackgroundEx))) {
            Wh_Log(L"[ERROR] Theme drawing hooks couldn't be installed");
            FreeLibrary(dui70);
            return FailInitialization();
        }
        // Keep the private hook target loaded until process exit.
        HMODULE pinnedDui70 = nullptr;
        BOOL pinned = GetModuleHandleExW(
            GET_MODULE_HANDLE_EX_FLAG_PIN |
                GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS,
            reinterpret_cast<LPCWSTR>(dui70), &pinnedDui70);
        FreeLibrary(dui70);
        if (!pinned) {
            Wh_Log(L"[ERROR] DUI70.dll couldn't be pinned");
            return FailInitialization();
        }
    }
    if (g_hostKind == HostKind::Explorer)
        Wh_Log(L"[INIT] Explorer Visual Tweaks Dark ready");
    return TRUE;
}
void Wh_ModUninit() {
    Selection::Uninitialize();
    Progress::Uninitialize();
    Preview::Uninitialize();
}
void Wh_ModSettingsChanged() {
    if (g_hostKind == HostKind::Unsupported)
        return;
    if (g_hostKind != HostKind::Prevhost) {
        Progress::ReloadSettings();
        Selection::ReloadSettings();
    }
}
