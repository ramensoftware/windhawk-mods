// ==WindhawkMod==
// @id              desktop-icon-selection-style
// @name            Desktop Icon Selection Style
// @description     Restyle the highlight behind selected desktop icons - size, transparency, rounded corners, glow, and a solid, dashed or dotted border
// @version         1.0.0
// @author          RiteshK
// @github          https://github.com/RiteshKhandekar
// @include         explorer.exe
// @compilerOptions -lcomctl32 -ldwmapi -lgdiplus
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Desktop Icon Selection Style

The highlight behind a selected desktop icon is a plain slab as wide as the whole
icon cell, so selections in neighbouring columns run together into one block.
This mod replaces it with one you can actually style.

**Before** - two selected icons, stock highlight, touching edge to edge:

![Two selected desktop icons with the stock highlight, a pair of rectangular slabs meeting with no gap between them](https://raw.githubusercontent.com/RiteshKhandekar/my-windhawk-mods/9570a63ad341cef48f91a5b37ad3d72fd95420e5/attachments/desktop-icon-selection-style-before.png)

**After** - narrowed, rounded, with a dashed border and an accent glow:

![The same two icons with the mod enabled, each highlight narrower than its cell with rounded corners, a dashed white border and a soft accent glow](https://raw.githubusercontent.com/RiteshKhandekar/my-windhawk-mods/9570a63ad341cef48f91a5b37ad3d72fd95420e5/attachments/desktop-icon-selection-style-after.png)

Everything in the after shot is configurable, and all of it can be turned off.

## Features

- **Narrow it.** Plate width is a percentage of the icon cell, so it looks right
  at any DPI. The stock plate is 100%.
- **Transparency.** Any fill colour and opacity, or `accent` to follow your
  system accent colour.
- **Rounded corners.** Any radius, or 0 for sharp corners.
- **Borders.** Solid, dashed, dotted or dash-dot, with your own colour, opacity,
  thickness and dash spacing.
- **Glow or drop shadow** around the plate, in any colour.
- **Hover styling**, with its own opacity. Turn it off to leave plain hover
  looking stock, or keep *Border on selection only* on so hovered icons get the
  fill but not the border.
- **Square shape**, if you would rather have a square than a rectangle.

Only selected icons are affected. Unselected icons keep their normal look, and
File Explorer windows are untouched - this is the desktop only.

## Notes worth knowing

**The plate follows the icon height on purpose.** Windows makes a selected icon
taller so its full file name can wrap onto extra lines, so the same icon needs a
short plate with a short name and a much taller one with a long name. The plate
grows with it, so a long name never runs out of the bottom of the highlight.
Width is a different matter: it is a percentage of the cell, while Windows wraps
the label to roughly the full cell, so a long name can overhang a narrow plate at
the sides. Raise **Plate width** if that bothers you. *Square* shape is
available, but it is capped at the size of the item, so a very large square comes
out smaller than requested.

**The glow is a fake, deliberately.** It is stacked translucent rings, not a real
blur, because a real blur is impossible here - the wallpaper simply is not
available at the moment the highlight is drawn. It still looks good, but do not
expect acrylic. It is also limited to roughly 10 pixels beyond the icon, which is
all Windows repaints around it; a larger glow gets cut off rather than smeared.

**Colours** take `accent` to follow your system accent colour, or a hex value
like `#3399FF` (the `#` is optional). Anything else is ignored, and the setting
keeps its default - the mod log says which value it rejected. The accent is read through the immersive colour API, so it is
the colour you actually picked in Settings rather than the blended colorization
value. Changing it is picked up as plates are next repainted, so selecting a
different icon shows the new colour; nothing is forced to redraw on a timer.

**Mixed-DPI setups** are fine, though not because anything is measured
per-monitor. The pixel settings are scaled by a single DPI, read from the
desktop's own window - but the icon rectangles the plate is drawn against come
from that same window, so the plate and the icon are always scaled by the same
number and stay in proportion to each other. Tested with a second display at a
different resolution and scale factor.

## Credit

The approach for finding the desktop list view and attaching to it - a
`CreateWindowExW` hook alongside `Wh_ModAfterInit` - follows
[Hide Desktop Icon Text and Shortcut Arrows](https://github.com/ramensoftware/windhawk-mods/blob/main/mods/hide-desktop-icon-text.wh.cpp)
by kivsak.

## Compatibility

Built and tested on Windows 11 build 26200. The highlight is intercepted through
`DrawThemeBackground`, which is what the desktop was measured to use. That
function is a wrapper over `DrawThemeBackgroundEx`, which is not hooked, so a
Windows build that called the inner function directly would leave the mod inert.
Inert, not broken: the stock highlight simply comes back.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- widthPercent: 75
  $name: Plate width
  $description: >-
    Width of the plate as a percentage of one icon cell. The stock plate is 100%,
    which is why adjacent selections touch. Lower values leave a gutter. Windows
    wraps a selected file name to about the full cell width, so a narrow plate
    can leave a long name overhanging its sides.
- topInset: 0
  $name: Top inset
  $description: >-
    Pixels to trim from the top of the plate. Trimming enough to collapse the
    plate entirely hides the highlight, which is a valid way to ask for no
    highlight at all.
- bottomInset: 0
  $name: Bottom inset
  $description: >-
    Pixels to trim from the bottom of the plate. Trimming enough to collapse the
    plate entirely hides the highlight.
- cornerRadius: 8
  $name: Corner radius
  $description: Corner rounding in pixels. 0 gives sharp corners.
- shape: auto
  $name: Shape
  $description: >-
    Auto follows the item, so the plate grows when a long file name wraps onto
    extra lines. Square forces equal width and height, anchored to the top of the
    item, and is capped at the size of the item being drawn.
  $options:
  - auto: Auto - follow the item
  - square: Square
- squareSize: 75
  $name: Square size
  $description: >-
    Side length in pixels for the square shape. 0 uses the plate width. Ignored
    when the shape is Auto.
- fillColor: accent
  $name: Fill colour
  $description: "accent to follow the system accent colour, or a hex value like #3399FF."
- fillOpacity: 40
  $name: Fill opacity
  $description: 0 is invisible, 100 is solid.
- glow: true
  $name: Fake glow (not a real blur)
  $description: >-
    Draws a soft edge around the plate using stacked translucent rings rather
    than a real blur. Windows only invalidates about 10 pixels around an icon and
    drawing is clipped to that, so a large glow gets cut off rather than drawn
    wrong.
- glowColor: accent
  $name: Glow colour
  $description: "Black reads as a drop shadow. accent, or a hex value like #3399FF."
- glowOpacity: 40
  $name: Glow opacity
  $description: Opacity reached at the edge of the plate.
- glowSize: 4
  $name: Glow size
  $description: >-
    How far the glow reaches beyond the plate, in pixels. Capped at 32, since
    each unit costs one antialiased fill per selected icon per repaint and
    anything past the invalidated area is clipped away unseen.
- glowOffsetY: 0
  $name: Glow vertical offset
  $description: >-
    Pushes the glow downwards, which reads as a drop shadow. Negative values push
    it up. Limited to 32 either way, since the glow is clipped a few pixels
    beyond the icon regardless.
- borderStyle: dash
  $name: Border style
  $options:
  - none: No border
  - solid: Solid
  - dash: Dashed
  - dot: Dotted
  - dashDot: Dash-dot
- borderOnSelectionOnly: true
  $name: Border on selection only
  $description: >-
    Keeps the border off the hover plate, so only a real selection gets one. Has
    no effect unless Style hover too is on, since hover is otherwise left stock.
    The plate itself stays exactly the same size either way, so nothing shifts
    when a hovered icon becomes selected.
- borderColor: "#FFFFFF"
  $name: Border colour
  $description: "accent to follow the system accent colour, or a hex value like #3399FF."
- borderOpacity: 70
  $name: Border opacity
  $description: 0 is invisible, 100 is solid.
- borderThickness: 1
  $name: Border thickness
  $description: Border thickness in pixels.
- dashLength: 2
  $name: Dash length
  $description: Length of one dash in pixels. Ignored by the solid and dotted styles.
- dashGap: 2
  $name: Dash gap
  $description: Gap between dashes or dots, in pixels.
- styleHover: true
  $name: Style hover too
  $description: >-
    Also restyle the plate shown when the mouse is over an unselected icon. Turn
    it off to leave plain hover looking stock.
- hoverOpacity: 40
  $name: Hover fill opacity
  $description: Fill opacity used for hover. Only applies when Style hover too is on.
*/
// ==/WindhawkModSettings==

#include <commctrl.h>
#include <dwmapi.h>
#include <uxtheme.h>

#include <objidl.h>

#include <gdiplus.h>

#include <windhawk_utils.h>

#include <algorithm>
#include <atomic>
#include <cmath>
#include <cstdlib>
#include <cwchar>
#include <cwctype>
#include <mutex>
#include <string>

// Child windows get the BEFOREPARENT/AFTERPARENT pair rather than
// WM_DPICHANGED, and older SDK headers do not always declare them.
#ifndef WM_DPICHANGED_BEFOREPARENT
#define WM_DPICHANGED_BEFOREPARENT 0x02E2
#endif
#ifndef WM_DPICHANGED_AFTERPARENT
#define WM_DPICHANGED_AFTERPARENT 0x02E3
#endif

////////////////////////////////////////////////////////////////////////////////
// Theme part and state ids are spelled out locally rather than pulled from
// <vssym32.h>, which is not reliably present in the mod compiler's headers.

constexpr int kLVP_LISTITEM = 1;

constexpr int kLISS_HOT = 2;
constexpr int kLISS_SELECTED = 3;
constexpr int kLISS_SELECTEDNOTFOCUS = 5;
constexpr int kLISS_HOTSELECTED = 6;

// A glow is clipped to the roughly ten pixels Windows invalidates around an
// item, so anything past this is invisible work: one antialiased path fill per
// unit, per selected item, per repaint.
constexpr int kMaxGlowSize = 32;
constexpr int kMaxGlowDevicePixels = 48;

// Every pixel setting is bounded, not just floored. MulDiv returns -1 on
// overflow rather than saturating, and a negative inset would widen the plate
// and then be clamped back to the whole item - the exact opposite of what the
// setting promises at its extreme.
constexpr int kMaxPixelSetting = 1024;

constexpr DWORD kAccentRefreshMs = 3000;

enum BorderStyle {
    kBorderNone = 0,
    kBorderSolid,
    kBorderDash,
    kBorderDot,
    kBorderDashDot,
};

struct {
    int widthPercent;
    int topInset;
    int bottomInset;
    int cornerRadius;
    bool square;
    int squareSize;
    COLORREF fillColor;
    bool fillUsesAccent;
    int fillOpacity;
    bool glow;
    COLORREF glowColor;
    bool glowUsesAccent;
    int glowOpacity;
    int glowSize;
    int glowOffsetY;
    int borderStyle;
    bool borderOnSelectionOnly;
    COLORREF borderColor;
    bool borderUsesAccent;
    int borderOpacity;
    int borderThickness;
    int dashLength;
    int dashGap;
    bool styleHover;
    int hoverOpacity;
} g_settings;

std::atomic<HWND> g_lv{nullptr};
std::atomic<UINT> g_dpi{96};
std::atomic<COLORREF> g_accentColor{RGB(0, 120, 215)};
std::atomic<DWORD> g_accentTick{0};

std::mutex g_gdiplusMutex;
ULONG_PTR g_gdiplusToken;
std::atomic<bool> g_gdiplusReady{false};

// Non-zero while the desktop list view is inside a paint message on this thread.
// Double buffering hands the theme call a memory DC, and WindowFromDC returns
// null for those, so this counter is the only way to know a draw belongs to the
// desktop. Measured, not assumed: every selection draw arrives with a null DC
// window.
thread_local int g_lvPaintDepth = 0;

////////////////////////////////////////////////////////////////////////////////
// Helpers

int Scale(int logical) {
    return MulDiv(logical, (int)g_dpi.load(std::memory_order_relaxed), 96);
}

// Called from the desktop UI thread by the CreateWindowExW hook and from
// Wh_ModAfterInit on Windhawk's thread, so it has to be safe to race.
bool EnsureGdiplus() {
    std::lock_guard<std::mutex> lock(g_gdiplusMutex);

    if (g_gdiplusReady.load(std::memory_order_relaxed)) {
        return true;
    }

    Gdiplus::GdiplusStartupInput startupInput;
    if (Gdiplus::GdiplusStartup(&g_gdiplusToken, &startupInput, nullptr) !=
        Gdiplus::Ok) {
        Wh_Log(L"GDI+ failed to start");
        return false;
    }

    g_gdiplusReady.store(true, std::memory_order_relaxed);
    return true;
}

COLORREF ReadAccentColor() {
    // DwmGetColorizationColor returns the colorization value, which is the
    // accent blended with the colorization intensity and afterglow, so it can
    // visibly differ from the colour picked in Settings. The immersive colour
    // API returns the real one; DWM stays as the fallback.
    using GetImmersiveColorFromColorSetEx_t = DWORD(WINAPI*)(DWORD, DWORD, BOOL, DWORD);
    using GetImmersiveColorTypeFromName_t = DWORD(WINAPI*)(LPCWSTR);
    using GetImmersiveUserColorSetPreference_t = DWORD(WINAPI*)(BOOL, BOOL);

    static const HMODULE uxtheme = GetModuleHandleW(L"uxtheme.dll");
    static const auto getColorFromColorSetEx =
        uxtheme ? (GetImmersiveColorFromColorSetEx_t)GetProcAddress(
                      uxtheme, MAKEINTRESOURCEA(95))
                : nullptr;
    static const auto getColorTypeFromName =
        uxtheme ? (GetImmersiveColorTypeFromName_t)GetProcAddress(
                      uxtheme, MAKEINTRESOURCEA(96))
                : nullptr;
    static const auto getUserColorSetPreference =
        uxtheme ? (GetImmersiveUserColorSetPreference_t)GetProcAddress(
                      uxtheme, MAKEINTRESOURCEA(98))
                : nullptr;

    if (getColorFromColorSetEx && getColorTypeFromName &&
        getUserColorSetPreference) {
        DWORD color = getColorFromColorSetEx(
            getUserColorSetPreference(FALSE, FALSE),
            getColorTypeFromName(L"ImmersiveStartHoverBackground"), TRUE, 0);
        return RGB(color & 0xFF, (color >> 8) & 0xFF, (color >> 16) & 0xFF);
    }

    DWORD color = 0;
    BOOL opaque = FALSE;
    if (SUCCEEDED(DwmGetColorizationColor(&color, &opaque))) {
        return RGB((color >> 16) & 0xFF, (color >> 8) & 0xFF, color & 0xFF);
    }

    return RGB(0, 120, 215);
}

bool AnythingUsesAccent() {
    return g_settings.fillUsesAccent ||
           (g_settings.glow && g_settings.glowUsesAccent) ||
           (g_settings.borderStyle != kBorderNone && g_settings.borderUsesAccent);
}

// Changing the accent colour broadcasts WM_DWMCOLORIZATIONCOLORCHANGED and
// WM_SETTINGCHANGE, but broadcasts only reach top level windows and the list
// view is a child, so those messages cannot be relied on. This poll is the
// backstop: at most one query every few seconds, and only while a colour is
// actually following the accent.
void RefreshAccentIfStale() {
    if (!AnythingUsesAccent()) {
        return;
    }

    DWORD now = GetTickCount();
    DWORD last = g_accentTick.load(std::memory_order_relaxed);
    if (last != 0 && now - last < kAccentRefreshMs) {
        return;
    }

    g_accentTick.store(now, std::memory_order_relaxed);
    g_accentColor.store(ReadAccentColor(), std::memory_order_relaxed);
}

UINT ReadWindowDpi(HWND hWnd) {
    using GetDpiForWindow_t = UINT(WINAPI*)(HWND);
    static GetDpiForWindow_t pGetDpiForWindow = []() -> GetDpiForWindow_t {
        HMODULE user32 = GetModuleHandleW(L"user32.dll");
        return user32 ? (GetDpiForWindow_t)GetProcAddress(user32, "GetDpiForWindow")
                      : nullptr;
    }();

    UINT dpi = pGetDpiForWindow ? pGetDpiForWindow(hWnd) : 0;
    return dpi ? dpi : 96;
}

// Accepts "accent", "#RRGGBB" or "RRGGBB". Returns false for anything else, and
// leaves the caller's colour untouched.
bool ParseColor(PCWSTR text, COLORREF* color, bool* usesAccent) {
    std::wstring value = text ? text : L"";

    while (!value.empty() && iswspace(value.front())) {
        value.erase(value.begin());
    }
    while (!value.empty() && iswspace(value.back())) {
        value.pop_back();
    }

    if (_wcsicmp(value.c_str(), L"accent") == 0) {
        *usesAccent = true;
        return true;
    }

    if (!value.empty() && value.front() == L'#') {
        value.erase(value.begin());
    }

    if (value.size() != 6) {
        return false;
    }

    // Checked digit by digit rather than left to wcstoul, which would happily
    // accept a sign or an 0x prefix and turn "-12345" into a colour.
    for (wchar_t c : value) {
        if (!iswxdigit(c)) {
            return false;
        }
    }

    unsigned long rgb = wcstoul(value.c_str(), nullptr, 16);

    *usesAccent = false;
    *color = RGB((rgb >> 16) & 0xFF, (rgb >> 8) & 0xFF, rgb & 0xFF);
    return true;
}

Gdiplus::Color MakeColor(COLORREF color, bool usesAccent, int opacityPercent) {
    if (usesAccent) {
        color = g_accentColor.load(std::memory_order_relaxed);
    }

    int alpha = MulDiv(std::clamp(opacityPercent, 0, 100), 255, 100);
    return Gdiplus::Color((BYTE)alpha, GetRValue(color), GetGValue(color),
                          GetBValue(color));
}

////////////////////////////////////////////////////////////////////////////////
// Finding the desktop
//
// The mod loads into every explorer.exe, not only the one hosting the shell, so
// ListViewUnder refuses a window belonging to another process. Without that, a
// secondary Explorer process would attach itself to the shell's desktop.
// IsDesktopListView needs no such check: the CreateWindowExW hook only ever
// hands it a window this process just created.

bool IsDesktopListView(HWND hWnd) {
    WCHAR buffer[64];

    if (!GetClassNameW(hWnd, buffer, ARRAYSIZE(buffer)) ||
        _wcsicmp(buffer, L"SysListView32") != 0) {
        return false;
    }

    if (!GetWindowTextW(hWnd, buffer, ARRAYSIZE(buffer)) ||
        _wcsicmp(buffer, L"FolderView") != 0) {
        return false;
    }

    HWND shellView = GetAncestor(hWnd, GA_PARENT);
    if (!shellView || !GetClassNameW(shellView, buffer, ARRAYSIZE(buffer)) ||
        _wcsicmp(buffer, L"SHELLDLL_DefView") != 0) {
        return false;
    }

    HWND top = GetAncestor(shellView, GA_PARENT);
    if (!top) {
        return false;
    }

    if (top == GetShellWindow()) {
        return true;
    }

    // Progman at creation time. A slideshow or a wallpaper tool can reparent
    // the shell view onto a WorkerW afterwards.
    if (!GetClassNameW(top, buffer, ARRAYSIZE(buffer))) {
        return false;
    }

    return _wcsicmp(buffer, L"Progman") == 0 || _wcsicmp(buffer, L"WorkerW") == 0;
}

HWND ListViewUnder(HWND shellView) {
    if (!shellView) {
        return nullptr;
    }

    HWND listView = FindWindowExW(shellView, nullptr, L"SysListView32", nullptr);
    if (!listView) {
        return nullptr;
    }

    DWORD pid = 0;
    GetWindowThreadProcessId(listView, &pid);
    return pid == GetCurrentProcessId() ? listView : nullptr;
}

HWND GetExistingDesktopListView() {
    // Every candidate host is tried, and the first one that actually yields a
    // list view wins. Finding a shell view is not enough on its own: depending
    // on the wallpaper arrangement, Progman can own a shell view that holds no
    // icons while the real one lives under a WorkerW. Stopping at the first
    // shell view found would miss the desktop entirely in that state.
    if (HWND listView =
            ListViewUnder(FindWindowExW(GetShellWindow(), nullptr,
                                        L"SHELLDLL_DefView", nullptr))) {
        return listView;
    }

    HWND worker = nullptr;
    while ((worker = FindWindowExW(nullptr, worker, L"WorkerW", nullptr)) !=
           nullptr) {
        if (HWND listView = ListViewUnder(FindWindowExW(
                worker, nullptr, L"SHELLDLL_DefView", nullptr))) {
            return listView;
        }
    }

    return nullptr;
}

////////////////////////////////////////////////////////////////////////////////
// Plate geometry
//
// The result is always clamped to the item rect. Windows only invalidated that
// much, so painting outside it would leave the old plate on screen when the
// selection moves.

RECT ComputePlateRect(const RECT& item) {
    int itemWidth = item.right - item.left;
    int itemHeight = item.bottom - item.top;
    if (itemWidth <= 0 || itemHeight <= 0) {
        return RECT{0, 0, 0, 0};
    }

    int width = MulDiv(itemWidth, std::clamp(g_settings.widthPercent, 1, 100), 100);

    RECT rc;
    rc.left = item.left + (itemWidth - width) / 2;
    rc.right = rc.left + width;
    rc.top = item.top + Scale(g_settings.topInset);
    rc.bottom = item.bottom - Scale(g_settings.bottomInset);

    if (g_settings.square) {
        int side = g_settings.squareSize > 0 ? Scale(g_settings.squareSize) : width;

        // Capped at the item, and anchored to the top of it, so the square stays
        // put over the icon instead of drifting when a long label wraps.
        int available = (int)(rc.bottom - rc.top);
        side = std::min(side, std::min(itemWidth, available));

        int centerX = (item.left + item.right) / 2;
        rc.left = centerX - side / 2;
        rc.right = rc.left + side;
        rc.bottom = rc.top + side;
    }

    RECT clamped;
    if (!IntersectRect(&clamped, &rc, &item)) {
        return RECT{0, 0, 0, 0};
    }

    return clamped;
}

////////////////////////////////////////////////////////////////////////////////
// Drawing

void AddRoundedRect(Gdiplus::GraphicsPath* path,
                    const Gdiplus::RectF& rect,
                    Gdiplus::REAL radius) {
    if (radius < 0.5f) {
        path->AddRectangle(rect);
        path->CloseFigure();
        return;
    }

    Gdiplus::REAL diameter = radius * 2;
    Gdiplus::REAL right = rect.X + rect.Width;
    Gdiplus::REAL bottom = rect.Y + rect.Height;

    path->AddArc(rect.X, rect.Y, diameter, diameter, 180.0f, 90.0f);
    path->AddArc(right - diameter, rect.Y, diameter, diameter, 270.0f, 90.0f);
    path->AddArc(right - diameter, bottom - diameter, diameter, diameter, 0.0f,
                 90.0f);
    path->AddArc(rect.X, bottom - diameter, diameter, diameter, 90.0f, 90.0f);
    path->CloseFigure();
}

// Stacked translucent rings, not a blur. Nothing clamps this to the item rect on
// purpose: GDI+ honours the device context's clip region, which is exactly the
// area Windows invalidated, so an oversized glow is cut off rather than left
// behind on screen when the selection moves.
void DrawGlow(Gdiplus::Graphics* graphics,
              const Gdiplus::RectF& plateRect,
              Gdiplus::REAL radius) {
    // The setting is capped in logical pixels, so the loop count would still
    // scale with DPI - 64 fills at 200%, 96 at 300%. Bounded again in device
    // pixels so the per-item cost cannot grow with the display.
    int size = std::min(Scale(g_settings.glowSize), kMaxGlowDevicePixels);
    if (size < 1) {
        return;
    }

    double target = std::clamp(g_settings.glowOpacity, 0, 100) / 100.0;
    if (target <= 0.0) {
        return;
    }

    // N stacked layers of alpha a compose to 1 - (1 - a)^N, so solve for the
    // per-layer alpha that lands on the requested opacity at the plate edge.
    double perLayer = 1.0 - std::pow(1.0 - target, 1.0 / size);
    BYTE alpha = (BYTE)std::clamp((int)std::lround(perLayer * 255.0), 1, 255);

    Gdiplus::Color base =
        MakeColor(g_settings.glowColor, g_settings.glowUsesAccent, 100);
    Gdiplus::SolidBrush brush(
        Gdiplus::Color(alpha, base.GetR(), base.GetG(), base.GetB()));

    Gdiplus::REAL offsetY = (Gdiplus::REAL)Scale(g_settings.glowOffsetY);

    for (int i = size; i >= 1; i--) {
        Gdiplus::RectF ring = plateRect;
        ring.Inflate((Gdiplus::REAL)i, (Gdiplus::REAL)i);
        ring.Offset(0.0f, offsetY);

        Gdiplus::GraphicsPath path;
        AddRoundedRect(&path, ring, radius + (Gdiplus::REAL)i);
        graphics->FillPath(&brush, &path);
    }
}

void ApplyDashPattern(Gdiplus::Pen* pen, Gdiplus::REAL thickness) {
    // GDI+ dash lengths are multiples of the pen width, so the settings, which
    // are in pixels, have to be divided by it.
    Gdiplus::REAL dash =
        std::max(1.0f, (Gdiplus::REAL)Scale(g_settings.dashLength)) / thickness;
    Gdiplus::REAL gap =
        std::max(1.0f, (Gdiplus::REAL)Scale(g_settings.dashGap)) / thickness;

    switch (g_settings.borderStyle) {
        case kBorderDash: {
            Gdiplus::REAL pattern[] = {dash, gap};
            pen->SetDashPattern(pattern, 2);
            break;
        }

        case kBorderDot: {
            // A dot is one pen width long, with a round cap so it reads as a dot
            // rather than a tiny square.
            Gdiplus::REAL pattern[] = {1.0f, gap};
            pen->SetDashPattern(pattern, 2);
            pen->SetDashCap(Gdiplus::DashCapRound);
            break;
        }

        case kBorderDashDot: {
            Gdiplus::REAL pattern[] = {dash, gap, 1.0f, gap};
            pen->SetDashPattern(pattern, 4);
            pen->SetDashCap(Gdiplus::DashCapRound);
            break;
        }
    }
}

bool DrawPlate(HDC hdc, const RECT& itemRect, LPCRECT clipRect, bool hot) {
    RefreshAccentIfStale();

    RECT plate = ComputePlateRect(itemRect);
    if (IsRectEmpty(&plate)) {
        // Nothing to paint, but the stock plate still stays suppressed - an
        // empty plate is a legitimate way to ask for no highlight at all.
        return true;
    }

    Gdiplus::Graphics graphics(hdc);
    if (graphics.GetLastStatus() != Gdiplus::Ok) {
        return false;
    }

    graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
    graphics.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHalf);

    if (clipRect) {
        graphics.SetClip(Gdiplus::Rect(clipRect->left, clipRect->top,
                                       clipRect->right - clipRect->left,
                                       clipRect->bottom - clipRect->top));
    }

    Gdiplus::REAL thickness =
        g_settings.borderStyle != kBorderNone
            ? (Gdiplus::REAL)std::max(1, Scale(g_settings.borderThickness))
            : 0.0f;

    Gdiplus::RectF rect((Gdiplus::REAL)plate.left, (Gdiplus::REAL)plate.top,
                        (Gdiplus::REAL)(plate.right - plate.left),
                        (Gdiplus::REAL)(plate.bottom - plate.top));

    // The stroke straddles the path, so pull the path in by half the pen width
    // to keep the whole border inside the plate. Capped first: a thickness wider
    // than the plate would inset it out of existence, and the selection would
    // vanish rather than merely look wrong.
    if (thickness > 0.0f) {
        Gdiplus::REAL maxThickness = std::min(rect.Width, rect.Height) - 1.0f;
        thickness = std::max(0.0f, std::min(thickness, maxThickness));
        rect.Inflate(-thickness / 2, -thickness / 2);
    }

    if (rect.Width <= 0.0f || rect.Height <= 0.0f) {
        return true;
    }

    Gdiplus::REAL radius =
        std::min((Gdiplus::REAL)Scale(g_settings.cornerRadius),
                 std::min(rect.Width, rect.Height) / 2);

    Gdiplus::GraphicsPath path;
    AddRoundedRect(&path, rect, radius);

    // Under everything, and with the plate's own area excluded so a translucent
    // fill is not darkened by the glow sitting behind it.
    if (g_settings.glow) {
        Gdiplus::GraphicsState state = graphics.Save();
        graphics.SetClip(&path, Gdiplus::CombineModeExclude);
        DrawGlow(&graphics, rect, radius);
        graphics.Restore(state);
    }

    int fillOpacity = hot ? g_settings.hoverOpacity : g_settings.fillOpacity;
    if (fillOpacity > 0) {
        Gdiplus::SolidBrush brush(
            MakeColor(g_settings.fillColor, g_settings.fillUsesAccent, fillOpacity));
        graphics.FillPath(&brush, &path);
    }

    // The plate was already inset by half the pen width above, whether or not
    // the border ends up being drawn, so a hovered icon and a selected one are
    // exactly the same size and nothing shifts between the two.
    bool drawBorder = thickness > 0.0f && g_settings.borderOpacity > 0 &&
                      !(hot && g_settings.borderOnSelectionOnly);

    if (drawBorder) {
        Gdiplus::Pen pen(MakeColor(g_settings.borderColor,
                                   g_settings.borderUsesAccent,
                                   g_settings.borderOpacity),
                         thickness);
        ApplyDashPattern(&pen, thickness);
        graphics.DrawPath(&pen, &path);
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////
// Hooks

using DrawThemeBackground_t = HRESULT(WINAPI*)(HTHEME, HDC, int, int, LPCRECT, LPCRECT);
DrawThemeBackground_t DrawThemeBackground_Original;

// Part 1 is LVP_LISTITEM only in the ListView class. Other classes number their
// parts from 1 too, so a draw is rejected when the theme it came from is
// recognisably one of those.
//
// Deliberately fail-open: anything not positively identified as a foreign class
// is allowed through. GetThemeClass is an undocumented ordinal, and matching the
// other way round - allowing only names ending in "ListView" - would silently
// disable the whole mod if it ever returned something unexpected, which is a far
// worse failure than the artifact it prevents.
bool IsForeignThemeClass(HTHEME hTheme) {
    using GetThemeClass_t = HRESULT(WINAPI*)(HTHEME, LPWSTR, int);
    static const auto pGetThemeClass = []() -> GetThemeClass_t {
        HMODULE uxtheme = GetModuleHandleW(L"uxtheme.dll");
        return uxtheme ? (GetThemeClass_t)GetProcAddress(uxtheme,
                                                         MAKEINTRESOURCEA(74))
                       : nullptr;
    }();

    if (!pGetThemeClass) {
        return false;
    }

    WCHAR cls[64];
    if (FAILED(pGetThemeClass(hTheme, cls, ARRAYSIZE(cls)))) {
        return false;
    }

    // A class list can be prefixed, as in "Explorer::ListView", so the tail is
    // what identifies it.
    size_t length = wcslen(cls);
    auto endsWith = [&](PCWSTR suffix) {
        size_t suffixLength = wcslen(suffix);
        return length >= suffixLength &&
               _wcsicmp(cls + length - suffixLength, suffix) == 0;
    };

    // Every class here numbers part 1 into the state range matched below:
    // SBP_ARROWBTN, EP_EDITTEXT, HP_HEADERITEM and BP_PUSHBUTTON.
    return endsWith(L"ScrollBar") || endsWith(L"Edit") ||
           endsWith(L"Header") || endsWith(L"Button");
}

bool ShouldReplace(HTHEME hTheme, int partId, int stateId, bool* hot) {
    if (g_lvPaintDepth <= 0 ||
        !g_gdiplusReady.load(std::memory_order_relaxed)) {
        return false;
    }

    if (partId != kLVP_LISTITEM || IsForeignThemeClass(hTheme)) {
        return false;
    }

    switch (stateId) {
        case kLISS_SELECTED:
        case kLISS_SELECTEDNOTFOCUS:
        // A selected icon under the mouse. Always ours, otherwise hovering a
        // selection would snap it back to the stock look.
        case kLISS_HOTSELECTED:
            *hot = false;
            return true;

        case kLISS_HOT:
            *hot = true;
            return g_settings.styleHover;
    }

    return false;
}

HRESULT WINAPI DrawThemeBackground_Hook(HTHEME hTheme,
                                        HDC hdc,
                                        int partId,
                                        int stateId,
                                        LPCRECT pRect,
                                        LPCRECT pClipRect) {
    bool hot = false;
    if (pRect && ShouldReplace(hTheme, partId, stateId, &hot) &&
        DrawPlate(hdc, *pRect, pClipRect, hot)) {
        return S_OK;
    }

    return DrawThemeBackground_Original(hTheme, hdc, partId, stateId, pRect,
                                        pClipRect);
}

////////////////////////////////////////////////////////////////////////////////
// Desktop subclass

LRESULT CALLBACK LvSubclassProc(HWND hWnd,
                                UINT uMsg,
                                WPARAM wParam,
                                LPARAM lParam,
                                DWORD_PTR dwRefData) {
    switch (uMsg) {
        case WM_NCDESTROY: {
            // The subclass itself is removed by the Windhawk wrapper.
            //
            // Cleared only if this is still the window being tracked. When the
            // shell rebuilds the desktop, the replacement list view can be
            // created and attached before the old one is destroyed, and an
            // unconditional store here would forget the live window.
            HWND self = hWnd;
            g_lv.compare_exchange_strong(self, nullptr, std::memory_order_relaxed);
            break;
        }

        // These rarely reach a child window, but cost nothing to honour when
        // they do. RefreshAccentIfStale is the reliable path. The repaint
        // matters: refreshing the colour alone only changes what the next draw
        // uses, so plates already on screen would keep the old accent.
        case WM_THEMECHANGED:
        case WM_SETTINGCHANGE:
        case WM_DWMCOLORIZATIONCOLORCHANGED:
            g_accentTick.store(0, std::memory_order_relaxed);
            g_dpi.store(ReadWindowDpi(hWnd), std::memory_order_relaxed);
            InvalidateRect(hWnd, nullptr, TRUE);
            break;

        case WM_DPICHANGED_BEFOREPARENT:
        case WM_DPICHANGED_AFTERPARENT:
            g_dpi.store(ReadWindowDpi(hWnd), std::memory_order_relaxed);
            break;

        // WM_NCPAINT is deliberately absent. Item backgrounds are never drawn
        // during non-client paint, but scroll bars are - by DefWindowProc, in
        // the ScrollBar theme class, where part 1 is SBP_ARROWBTN and its
        // states line up exactly with the list item states matched below. The
        // desktop list view has no LVS_NOSCROLL, so a resolution or DPI change
        // that pushes icons out of the work area gives it a scroll bar, and
        // counting non-client paint would draw a selection plate over the
        // arrows.
        case WM_PAINT:
        case WM_ERASEBKGND:
        case WM_PRINTCLIENT: {
            g_lvPaintDepth++;
            LRESULT result = DefSubclassProc(hWnd, uMsg, wParam, lParam);
            g_lvPaintDepth--;
            return result;
        }
    }

    return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

void AttachToListView(HWND hWnd) {
    // Deferred to here rather than done in Wh_ModInit, so the File Explorer
    // processes that can never host the desktop do not pay for a GDI+
    // initialization and its background thread.
    EnsureGdiplus();

    // Deliberately lock-free. Both subclass helpers end in an untimed
    // SendMessage to the window's own thread, and one of the two callers of
    // this function *is* that thread, through the CreateWindowExW hook. A lock
    // held across those sends would let the desktop thread block on it while
    // the holder waits on a send to the desktop thread - a permanent shell
    // hang. The ordering below needs no lock anyway: whichever thread exchanges
    // last owns g_lv, and every window it displaces is unsubclassed.
    if (!WindhawkUtils::SetWindowSubclassFromAnyThread(hWnd, LvSubclassProc, 0)) {
        return;
    }

    // Exactly one list view stays subclassed. When the shell rebuilds the
    // desktop the replacement can be attached before the old window is
    // destroyed, and leaving the old subclass in place would let it outlive the
    // mod image: unloading only ever unsubclasses the tracked window, so the
    // forgotten one would call into freed memory on its next message.
    HWND previous = g_lv.exchange(hWnd, std::memory_order_relaxed);
    if (previous && previous != hWnd) {
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(previous, LvSubclassProc);
    }

    g_dpi.store(ReadWindowDpi(hWnd), std::memory_order_relaxed);
    g_accentTick.store(0, std::memory_order_relaxed);
    InvalidateRect(hWnd, nullptr, TRUE);
}

// The desktop is rebuilt whenever the shell recreates it, so the list view is
// caught as it is created rather than polled for.
using CreateWindowExW_t = decltype(&CreateWindowExW);
CreateWindowExW_t CreateWindowExW_Original;

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
                                 PVOID lpParam) {
    HWND hWnd = CreateWindowExW_Original(dwExStyle, lpClassName, lpWindowName,
                                         dwStyle, X, Y, nWidth, nHeight,
                                         hWndParent, hMenu, hInstance, lpParam);
    if (!hWnd) {
        return hWnd;
    }

    // This hook sees every window Explorer creates, so the common case is
    // rejected on the class name the caller already passed, before spending
    // anything on walking the window's ancestry. A class atom rather than a
    // string just falls through to the full check.
    if (!IS_INTRESOURCE(lpClassName) &&
        _wcsicmp(lpClassName, L"SysListView32") != 0) {
        return hWnd;
    }

    if (IsDesktopListView(hWnd)) {
        AttachToListView(hWnd);
    }

    return hWnd;
}

////////////////////////////////////////////////////////////////////////////////
// Settings

int GetBorderStyleSetting() {
    WindhawkUtils::StringSetting value =
        WindhawkUtils::StringSetting::make(L"borderStyle");

    if (_wcsicmp(value.get(), L"solid") == 0) {
        return kBorderSolid;
    }
    if (_wcsicmp(value.get(), L"dash") == 0) {
        return kBorderDash;
    }
    if (_wcsicmp(value.get(), L"dot") == 0) {
        return kBorderDot;
    }
    if (_wcsicmp(value.get(), L"dashDot") == 0) {
        return kBorderDashDot;
    }

    return kBorderNone;
}

// The caller seeds the colour with the default its setting declares, so an
// unparseable string lands on the documented colour rather than on some other
// one the setting never advertised - and says so in the log, since otherwise a
// typo just quietly renders as something else.
void LoadColorSetting(PCWSTR name, COLORREF* color, bool* usesAccent) {
    WindhawkUtils::StringSetting value =
        WindhawkUtils::StringSetting::make(name);

    if (!ParseColor(value.get(), color, usesAccent)) {
        Wh_Log(L"%s: \"%s\" is not a colour, keeping the default", name,
               value.get());
    }
}

void LoadSettings() {
    // Built up locally and published in one assignment. The paint thread reads
    // g_settings without synchronisation, and writing it field by field let a
    // repaint land midway through - drawing one frame from a mix of old and new
    // values, and briefly showing the fallback colour seeded before each parse.
    decltype(g_settings) s{};

    s.widthPercent = std::clamp(Wh_GetIntSetting(L"widthPercent"), 1, 100);
    s.topInset = std::clamp(Wh_GetIntSetting(L"topInset"), 0, kMaxPixelSetting);
    s.bottomInset =
        std::clamp(Wh_GetIntSetting(L"bottomInset"), 0, kMaxPixelSetting);
    s.cornerRadius =
        std::clamp(Wh_GetIntSetting(L"cornerRadius"), 0, kMaxPixelSetting);

    WindhawkUtils::StringSetting shape =
        WindhawkUtils::StringSetting::make(L"shape");
    s.square = _wcsicmp(shape.get(), L"square") == 0;
    s.squareSize = std::clamp(Wh_GetIntSetting(L"squareSize"), 0, kMaxPixelSetting);

    s.fillColor = RGB(255, 255, 255);
    s.fillUsesAccent = true;
    LoadColorSetting(L"fillColor", &s.fillColor, &s.fillUsesAccent);
    s.fillOpacity = std::clamp(Wh_GetIntSetting(L"fillOpacity"), 0, 100);

    s.glow = Wh_GetIntSetting(L"glow");
    s.glowColor = RGB(0, 0, 0);
    s.glowUsesAccent = true;
    LoadColorSetting(L"glowColor", &s.glowColor, &s.glowUsesAccent);
    s.glowOpacity = std::clamp(Wh_GetIntSetting(L"glowOpacity"), 0, 100);
    s.glowSize = std::clamp(Wh_GetIntSetting(L"glowSize"), 0, kMaxGlowSize);
    s.glowOffsetY =
        std::clamp(Wh_GetIntSetting(L"glowOffsetY"), -kMaxGlowSize, kMaxGlowSize);

    s.borderStyle = GetBorderStyleSetting();
    s.borderOnSelectionOnly = Wh_GetIntSetting(L"borderOnSelectionOnly");
    s.borderColor = RGB(255, 255, 255);
    s.borderUsesAccent = false;
    LoadColorSetting(L"borderColor", &s.borderColor, &s.borderUsesAccent);
    s.borderOpacity = std::clamp(Wh_GetIntSetting(L"borderOpacity"), 0, 100);
    s.borderThickness =
        std::clamp(Wh_GetIntSetting(L"borderThickness"), 1, kMaxPixelSetting);

    s.dashLength = std::clamp(Wh_GetIntSetting(L"dashLength"), 1, kMaxPixelSetting);
    s.dashGap = std::clamp(Wh_GetIntSetting(L"dashGap"), 1, kMaxPixelSetting);

    s.styleHover = Wh_GetIntSetting(L"styleHover");
    s.hoverOpacity = std::clamp(Wh_GetIntSetting(L"hoverOpacity"), 0, 100);

    g_settings = s;
}

////////////////////////////////////////////////////////////////////////////////
// Mod entry points

BOOL Wh_ModInit() {
    Wh_Log(L"Init");

    LoadSettings();

    // uxtheme is always already loaded in Explorer, so there is no module
    // reference to acquire or leak here.
    HMODULE uxtheme = GetModuleHandleW(L"uxtheme.dll");
    auto drawThemeBackground =
        uxtheme ? (DrawThemeBackground_t)GetProcAddress(uxtheme,
                                                        "DrawThemeBackground")
                : nullptr;

    if (!drawThemeBackground ||
        !WindhawkUtils::SetFunctionHook(drawThemeBackground,
                                        DrawThemeBackground_Hook,
                                        &DrawThemeBackground_Original)) {
        Wh_Log(L"Failed to hook DrawThemeBackground");
        return FALSE;
    }

    if (!WindhawkUtils::SetFunctionHook(CreateWindowExW, CreateWindowExW_Hook,
                                        &CreateWindowExW_Original)) {
        Wh_Log(L"Failed to hook CreateWindowExW");
        return FALSE;
    }

    // GDI+ is started by the first attach instead of here. See AttachToListView.
    return TRUE;
}

void Wh_ModAfterInit() {
    // The CreateWindowExW hook may already have caught a newer list view while
    // this was starting up. Attaching the one found by searching would displace
    // it with a window that is on its way out.
    if (g_lv.load(std::memory_order_relaxed)) {
        return;
    }

    if (HWND listView = GetExistingDesktopListView()) {
        AttachToListView(listView);
    }
}

void Wh_ModUninit() {
    Wh_Log(L"Uninit");

    // g_lv is null exactly when nothing is subclassed, so there is nothing to
    // fall back to searching for.
    if (HWND listView = g_lv.exchange(nullptr, std::memory_order_relaxed)) {
        // An untimed send, so the mod cannot be unloaded while its subclass proc
        // is still on the list view's chain.
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(listView, LvSubclassProc);
        if (IsWindow(listView)) {
            InvalidateRect(listView, nullptr, TRUE);
        }
    }

    std::lock_guard<std::mutex> lock(g_gdiplusMutex);
    if (g_gdiplusReady.load(std::memory_order_relaxed)) {
        Gdiplus::GdiplusShutdown(g_gdiplusToken);
        g_gdiplusReady.store(false, std::memory_order_relaxed);
    }
}

void Wh_ModSettingsChanged() {
    Wh_Log(L"SettingsChanged");

    LoadSettings();

    HWND listView = g_lv.load(std::memory_order_relaxed);
    if (!listView) {
        // A retry, so a lookup that missed at startup - an unusual wallpaper
        // host, say - is recoverable by touching a setting rather than by
        // restarting Explorer.
        if (HWND found = GetExistingDesktopListView()) {
            AttachToListView(found);
            return;
        }
    }

    if (listView) {
        g_dpi.store(ReadWindowDpi(listView), std::memory_order_relaxed);
        g_accentTick.store(0, std::memory_order_relaxed);
        InvalidateRect(listView, nullptr, TRUE);
    }
}
