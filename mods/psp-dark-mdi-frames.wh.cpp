// ==WindhawkMod==
// @id              psp-dark-mdi-frames
// @name            PaintShop Pro Dark Mode
// @description     Darker interface, black title bar, accent color and dark image windows for Corel PaintShop Pro
// @version         2.0.0
// @author          MrSmith
// @github          https://github.com/MrSmith2017
// @homepage        https://github.com/MrSmith2017/psp-dark-image-windows
// @license         MIT
// @include         Corel PaintShop Pro.exe
// @compilerOptions -lgdi32 -luser32 -lcomctl32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# PaintShop Pro Dark Mode

![Before and after: PaintShop Pro with its original look versus the dark mode](https://raw.githubusercontent.com/MrSmith2017/psp-dark-image-windows/main/psp-dark-mode.png)

Corel PaintShop Pro has a dark theme, but it is not really dark: the surfaces are
a medium gray, the title bar is gray with an orange tab, and the image windows in
the workspace keep a light blue-gray frame that looks like Windows XP or 7. This
mod goes the rest of the way.

## What it changes

* **Interface darkness:** the dark gray surfaces (tool bars, tool column, palettes,
  tool options bar, dialogs, the area around the color wheel) get darker. The
  level is a setting, and the hue stays the same.
* **Title bar:** the title bar of the main window gets a color of your choice,
  black by default.
* **Accent color:** the orange of the "Edit" tab and of the line below the title
  bar can be replaced by any color.
* **Image windows:** the frames of the image windows (MDI child windows) are
  painted dark: title bar, border, icon, title text, minimize, maximize and close
  buttons with hover and pressed states, and minimized image windows.
* **Start tiles (optional):** hides the eight learning tiles and the hint text in
  the middle of the empty workspace.
* **Flash fix (optional):** when you switch between images, PaintShop Pro
  rebuilds the drop-down lists in the tool options bar. For a few frames their
  selection fields flash white; the fix paints that field dark instead.
* **Tool options bar flicker (optional, experimental, off by default):** while
  PaintShop Pro refills the tool options bar, the bar is not redrawn for a short
  moment (at most 250 ms) and drawn once afterwards. This uses
  `LockWindowUpdate`, which can lock only one window on the whole desktop at a
  time, so you have to switch it on yourself.

Everything is done by re-coloring while PaintShop Pro draws, so no theme files,
program files or system settings are touched, and the mod only affects PaintShop
Pro. Maximized image windows are left to Windows.

## Settings

* Colors are entered as hex codes, for example `282828` or `#282828`. A value that
  cannot be read falls back to its default.
* The frame colors, the layout and the flash fix apply right away. The interface
  darkness, the title bar color and the accent color apply when PaintShop Pro is
  restarted.
* Leave the accent color empty to keep the original orange.

## Notes

* Tested with PaintShop Pro 2022 on Windows 11. Other versions may work, but have
  not been tested.
* Some parts of the interface are recognized by their size and color, so a
  different version may look different in places. Tested at 100% and 120%
  display scaling.
* The swatches in the Material palette and in the color dialog show the exact color you
  chose, and the content of your images is never touched. Other places that draw
  your colors may show very dark grays slightly darker.
* If something looks wrong, disable the mod: the original look comes back after
  restarting PaintShop Pro.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- colors:
  - captionActive: "282828"
    $name: Title bar, active window
    $description: Hex color, for example 282828.
  - captionInactive: "202020"
    $name: Title bar, inactive window
  - textActive: "ECECEC"
    $name: Title text, active window
  - textInactive: "828282"
    $name: Title text, inactive window
  - border: "3A3A3A"
    $name: Outer border line
  - clientEdge: "101010"
    $name: Thin line around the image area
  - glyph: "E4E4E4"
    $name: Button symbols, active window
  - glyphInactive: "787878"
    $name: Button symbols, inactive window
  - buttonHover: "464646"
    $name: Button background on hover
  - buttonPressed: "5A5A5A"
    $name: Button background when pressed
  - closeHover: "C42B1C"
    $name: Close button background on hover
  - closePressed: "961E14"
    $name: Close button background when pressed
  $name: Colors
- layout:
  - buttonWidth: 29
    $name: Distance between the caption buttons
    $description: In pixels at 100% scaling.
  - glyphSize: 10
    $name: Size of the button symbols
    $description: In pixels at 100% scaling.
  $name: Layout
- interfaceDarkness: 60
  $name: Interface darkness
  $description: In percent of the original brightness of the dark gray surfaces. 100 is unchanged, lower is darker (60 is about the usual dark mode of other apps). Restart PaintShop Pro to apply fully.
- accentColor: ""
  $name: Accent color
  $description: Hex color that replaces the orange of the title bar tab and the line below the title bar, for example 01D0E5. Leave empty for the original orange. Restart PaintShop Pro to apply.
- hideStartTiles: false
  $name: Hide the start tiles
  $description: Hides the eight learning tiles and the hint text in the middle of the empty workspace.
- titleBarColor: "000000"
  $name: Title bar color
  $description: Hex color of the title bar of the main window, for example 000000. Leave empty for the original gray. Restart PaintShop Pro to apply.
- fixComboFlash: true
  $name: Fix white flash in the tool options bar
  $description: Paints the selection field of the drop-down lists dark while PaintShop Pro rebuilds them.
- freezeOptionsBar: false
  $name: Reduce flicker of the tool options bar (experimental)
  $description: While PaintShop Pro refills the controls of the tool options bar (for example when another image is activated), the bar is not redrawn for a short moment, at most 250 ms, and drawn once afterwards. This uses LockWindowUpdate, which can lock only one window on the whole desktop at a time, so it is off by default.
- flashColor: "3C3C3C"
  $name: Color used instead of white
  $description: Hex color for the flash fix.
*/
// ==/WindhawkModSettings==

#include <windhawk_api.h>
#include <windhawk_utils.h>
#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <algorithm>
#include <atomic>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <cwchar>
#include <cwctype>
#include <unordered_map>

// ---- Settings ---------------------------------------------------------------

struct Settings {
    COLORREF captionActive;
    COLORREF captionInactive;
    COLORREF textActive;
    COLORREF textInactive;
    COLORREF border;
    COLORREF clientEdge;
    COLORREF glyph;
    COLORREF glyphInactive;
    COLORREF buttonHot;
    COLORREF buttonPressed;
    COLORREF closeHot;
    COLORREF closePressed;
    int buttonWidth;
    int glyphSize;
    bool fixComboFlash;
    COLORREF flashColor;
    int darkness;
    bool titleOn;
    COLORREF titleColor;
    bool accentOn;
    float accentH, accentS, accentL;
    bool hideTiles;
    bool freezeBar;
};

static Settings g_cfg;
static void EnsureOptionsHook();

static const COLORREF kGlyphDisabled = RGB(72, 72, 72);

// Parses "282828", "#282828" or "#fff". Returns the fallback for anything else.
static COLORREF ParseColor(PCWSTR text, COLORREF fallback) {
    if (!text) {
        return fallback;
    }
    while (*text == L' ' || *text == L'#') {
        text++;
    }
    int digits[6];
    int count = 0;
    while (iswxdigit(*text) && count < 6) {
        wchar_t c = *text++;
        digits[count++] = (c >= L'a') ? c - L'a' + 10 : (c >= L'A') ? c - L'A' + 10 : c - L'0';
    }
    while (*text == L' ') {
        text++;
    }
    if (*text != 0) {
        return fallback;
    }
    if (count == 3) {
        return RGB(digits[0] * 17, digits[1] * 17, digits[2] * 17);
    }
    if (count == 6) {
        return RGB(digits[0] * 16 + digits[1], digits[2] * 16 + digits[3], digits[4] * 16 + digits[5]);
    }
    return fallback;
}

static COLORREF GetColorSetting(PCWSTR name, COLORREF fallback) {
    PCWSTR text = Wh_GetStringSetting(name);
    COLORREF color = ParseColor(text, fallback);
    Wh_FreeStringSetting(text);
    return color;
}

static int GetSizeSetting(PCWSTR name, int fallback, int min, int max) {
    int value = Wh_GetIntSetting(name);
    return (value < min || value > max) ? fallback : value;
}


// ---- small HSL helpers for the accent color -------------------------------------------------
static void RgbToHslF(float r, float g, float b, float* h, float* s, float* l) {
    const float mx = fmaxf(r, fmaxf(g, b)), mn = fminf(r, fminf(g, b));
    const float d = mx - mn;
    *l = (mx + mn) * 0.5f;
    if (d < 1e-6f) {
        *h = 0;
        *s = 0;
        return;
    }
    *s = *l > 0.5f ? d / (2.0f - mx - mn) : d / (mx + mn);
    float hh;
    if (mx == r) hh = (g - b) / d + (g < b ? 6.0f : 0.0f);
    else if (mx == g) hh = (b - r) / d + 2.0f;
    else hh = (r - g) / d + 4.0f;
    *h = hh * 60.0f;
}

static void HslToRgbF(float h, float s, float l, float* r, float* g, float* b) {
    const float c = (1.0f - fabsf(2.0f * l - 1.0f)) * s;
    const float hh = h / 60.0f;
    const float x = c * (1.0f - fabsf(fmodf(hh, 2.0f) - 1.0f));
    float rr = 0, gg = 0, bb = 0;
    if (hh < 1) { rr = c; gg = x; }
    else if (hh < 2) { rr = x; gg = c; }
    else if (hh < 3) { gg = c; bb = x; }
    else if (hh < 4) { gg = x; bb = c; }
    else if (hh < 5) { rr = x; bb = c; }
    else { rr = c; bb = x; }
    const float m = l - c * 0.5f;
    *r = rr + m;
    *g = gg + m;
    *b = bb + m;
}

static void LoadSettings() {
    g_cfg.captionActive = GetColorSetting(L"colors.captionActive", RGB(0x28, 0x28, 0x28));
    g_cfg.captionInactive = GetColorSetting(L"colors.captionInactive", RGB(0x20, 0x20, 0x20));
    g_cfg.textActive = GetColorSetting(L"colors.textActive", RGB(0xEC, 0xEC, 0xEC));
    g_cfg.textInactive = GetColorSetting(L"colors.textInactive", RGB(0x82, 0x82, 0x82));
    g_cfg.border = GetColorSetting(L"colors.border", RGB(0x3A, 0x3A, 0x3A));
    g_cfg.clientEdge = GetColorSetting(L"colors.clientEdge", RGB(0x10, 0x10, 0x10));
    g_cfg.glyph = GetColorSetting(L"colors.glyph", RGB(0xE4, 0xE4, 0xE4));
    g_cfg.glyphInactive = GetColorSetting(L"colors.glyphInactive", RGB(0x78, 0x78, 0x78));
    g_cfg.buttonHot = GetColorSetting(L"colors.buttonHover", RGB(0x46, 0x46, 0x46));
    g_cfg.buttonPressed = GetColorSetting(L"colors.buttonPressed", RGB(0x5A, 0x5A, 0x5A));
    g_cfg.closeHot = GetColorSetting(L"colors.closeHover", RGB(0xC4, 0x2B, 0x1C));
    g_cfg.closePressed = GetColorSetting(L"colors.closePressed", RGB(0x96, 0x1E, 0x14));
    g_cfg.buttonWidth = GetSizeSetting(L"layout.buttonWidth", 29, 16, 80);
    g_cfg.glyphSize = GetSizeSetting(L"layout.glyphSize", 10, 6, 20);
    g_cfg.fixComboFlash = Wh_GetIntSetting(L"fixComboFlash") != 0;
    g_cfg.flashColor = GetColorSetting(L"flashColor", RGB(0x3C, 0x3C, 0x3C));
    int darkness = Wh_GetIntSetting(L"interfaceDarkness");
    g_cfg.darkness = darkness <= 0 ? 60 : (darkness < 20 ? 20 : (darkness > 100 ? 100 : darkness));
    PCWSTR title = Wh_GetStringSetting(L"titleBarColor");
    g_cfg.titleOn = *title != 0;
    g_cfg.titleColor = g_cfg.titleOn ? ParseColor(title, RGB(0, 0, 0)) : 0;
    Wh_FreeStringSetting(title);
    PCWSTR accent = Wh_GetStringSetting(L"accentColor");
    COLORREF ac = *accent ? ParseColor(accent, CLR_INVALID) : CLR_INVALID;
    Wh_FreeStringSetting(accent);
    g_cfg.accentOn = ac != CLR_INVALID;
    if (g_cfg.accentOn) {
        float h, sat, l;
        RgbToHslF(GetRValue(ac) / 255.0f, GetGValue(ac) / 255.0f, GetBValue(ac) / 255.0f, &h, &sat, &l);
        g_cfg.accentH = h;
        g_cfg.accentS = sat;
        g_cfg.accentL = l;
    }
    g_cfg.hideTiles = Wh_GetIntSetting(L"hideStartTiles") != 0;
    g_cfg.freezeBar = Wh_GetIntSetting(L"freezeOptionsBar") != 0;
}

// ---- Layout constants (at 100% scaling) --------------------------------------
static const int kButtonWidthMinimized = 24;
static const int kButtonHeight = 22;
static const int kButtonInset = 8;           // close button to the right edge
static const int kButtonInsetMinimized = 3;

static const LRESULT kBtnHit[3] = {HTMINBUTTON, HTMAXBUTTON, HTCLOSE};

static const wchar_t kHotProp[] = L"PspDarkMdi.Hot";
static const wchar_t kPressedProp[] = L"PspDarkMdi.Pressed";
static const wchar_t kActiveProp[] = L"PspDarkMdi.Active";  // 1 = active, 2 = inactive

typedef LRESULT(WINAPI* DefMDIChildProc_t)(HWND, UINT, WPARAM, LPARAM);
static DefMDIChildProc_t DefMDIChildProcW_orig;
static DefMDIChildProc_t DefMDIChildProcA_orig;

// ---- Helpers ----------------------------------------------------------------

static bool ShouldHandle(HWND hWnd) {
    LONG style = GetWindowLongW(hWnd, GWL_STYLE);
    LONG exStyle = GetWindowLongW(hWnd, GWL_EXSTYLE);
    return (exStyle & WS_EX_MDICHILD) && (style & WS_CAPTION) == WS_CAPTION &&
           (style & WS_VISIBLE) && !IsZoomed(hWnd);
}

static int GetState(HWND hWnd, const wchar_t* prop) {
    return (int)(INT_PTR)GetPropW(hWnd, prop);
}

static void SetState(HWND hWnd, const wchar_t* prop, int value) {
    if (value) {
        SetPropW(hWnd, prop, (HANDLE)(INT_PTR)value);
    } else {
        RemovePropW(hWnd, prop);
    }
}

static bool IsActiveMdiChild(HWND hWnd) {
    int state = GetState(hWnd, kActiveProp);
    if (state) {
        return state == 1;
    }
    HWND client = GetParent(hWnd);
    if (!client) {
        return true;
    }
    return (HWND)SendMessageW(client, WM_MDIGETACTIVE, 0, 0) == hWnd;
}

// Geometry of the non-client area, in window coordinates.
struct FrameInfo {
    RECT window;    // 0, 0, W, H
    RECT client;    // client area (empty at the bottom edge for minimized windows)
    int capBottom;  // bottom of the title area (above the client edge)
    int centerY;    // vertical center of the title area
    bool iconic;
    int dpi;
    RECT screen;    // window rectangle in screen coordinates
};

static bool GetFrameInfo(HWND hWnd, FrameInfo* fi) {
    RECT wr;
    if (!GetWindowRect(hWnd, &wr)) {
        return false;
    }
    const int W = wr.right - wr.left;
    const int H = wr.bottom - wr.top;

    fi->screen = wr;
    fi->window = {0, 0, W, H};
    fi->iconic = IsIconic(hWnd) != FALSE;
    fi->dpi = (int)GetDpiForWindow(hWnd);

    if (fi->iconic) {
        // A minimized window is only a title bar.
        fi->client = {0, H, 0, H};
        fi->capBottom = H - 1;
    } else {
        POINT origin = {0, 0};
        ClientToScreen(hWnd, &origin);
        RECT cr;
        GetClientRect(hWnd, &cr);
        fi->client = {origin.x - wr.left, origin.y - wr.top,
                      origin.x - wr.left + cr.right, origin.y - wr.top + cr.bottom};

        int edge = (GetWindowLongW(hWnd, GWL_EXSTYLE) & WS_EX_CLIENTEDGE) ? GetSystemMetricsForDpi(SM_CYEDGE, fi->dpi) : 0;
        fi->capBottom = fi->client.top - edge;
    }
    fi->centerY = (1 + fi->capBottom) / 2;
    return true;
}

// Our own layout of the three caption buttons (window coordinates).
// idx: 0 = minimize (restore on a minimized window), 1 = maximize, 2 = close.
static bool GetButton(HWND hWnd, const FrameInfo& fi, int idx, RECT* out, DWORD* state) {
    LONG style = GetWindowLongW(hWnd, GWL_STYLE);
    if (!(style & WS_SYSMENU)) {
        return false;
    }

    const int bw = MulDiv(fi.iconic ? kButtonWidthMinimized : g_cfg.buttonWidth, fi.dpi, 96);
    int bh = MulDiv(kButtonHeight, fi.dpi, 96);
    if (bh > fi.capBottom - 2) {
        bh = fi.capBottom - 2;
    }
    const int rightEdge = fi.window.right - MulDiv(fi.iconic ? kButtonInsetMinimized : kButtonInset, fi.dpi, 96);

    RECT b;
    b.right = rightEdge - (2 - idx) * bw;
    b.left = b.right - bw;
    b.top = fi.centerY - bh / 2;
    b.bottom = b.top + bh;
    *out = b;

    DWORD st = 0;
    if (idx == 0 && !fi.iconic && !(style & WS_MINIMIZEBOX)) {
        st = STATE_SYSTEM_UNAVAILABLE;
    } else if (idx == 1 && !(style & WS_MAXIMIZEBOX)) {
        st = STATE_SYSTEM_UNAVAILABLE;
    }
    *state = st;
    return true;
}

// which: 0 = minimize, 1 = maximize, 2 = close, 3 = restore
static void DrawGlyph(HDC dc, int which, const RECT& b, COLORREF color, int dpi) {
    const int gw = MulDiv(g_cfg.glyphSize, dpi, 96);
    const int cx = (b.left + b.right) / 2;
    const int cy = (b.top + b.bottom) / 2;
    const int l = cx - gw / 2;
    const int r = l + gw;
    const int t = cy - gw / 2;
    const int bt = t + gw;
    const int d = MulDiv(2, dpi, 96);

    int penWidth = MulDiv(1, dpi, 96);
    if (penWidth < 1) {
        penWidth = 1;
    }
    HPEN pen = CreatePen(PS_SOLID, penWidth, color);
    HPEN oldPen = (HPEN)SelectObject(dc, pen);
    HBRUSH oldBrush = (HBRUSH)SelectObject(dc, GetStockObject(NULL_BRUSH));

    switch (which) {
        case 0:  // minimize
            MoveToEx(dc, l, cy + gw / 4, nullptr);
            LineTo(dc, r, cy + gw / 4);
            break;
        case 1:  // maximize
            Rectangle(dc, l, t, r, bt);
            break;
        case 2:  // close
            MoveToEx(dc, l, t, nullptr);
            LineTo(dc, r, bt);
            MoveToEx(dc, l, bt - 1, nullptr);
            LineTo(dc, r, t - 1);
            break;
        case 3:  // restore
            Rectangle(dc, l, t + d, r - d, bt);
            MoveToEx(dc, l + d, t + d, nullptr);
            LineTo(dc, l + d, t);
            LineTo(dc, r - 1, t);
            LineTo(dc, r - 1, bt - d);
            LineTo(dc, r - d - 1, bt - d);
            break;
    }

    SelectObject(dc, oldBrush);
    SelectObject(dc, oldPen);
    DeleteObject(pen);
}

static void PaintFrame(HWND hWnd) {
    FrameInfo fi;
    if (!GetFrameInfo(hWnd, &fi)) {
        return;
    }
    const int W = fi.window.right;
    const int H = fi.window.bottom;
    const RECT& client = fi.client;
    const int dpi = fi.dpi;

    LONG style = GetWindowLongW(hWnd, GWL_STYLE);
    const int scrollW = (!fi.iconic && (style & WS_VSCROLL)) ? GetSystemMetricsForDpi(SM_CXVSCROLL, dpi) : 0;
    const int scrollH = (!fi.iconic && (style & WS_HSCROLL)) ? GetSystemMetricsForDpi(SM_CYHSCROLL, dpi) : 0;

    const bool active = IsActiveMdiChild(hWnd);
    const int hot = GetState(hWnd, kHotProp) - 1;
    const int pressed = GetState(hWnd, kPressedProp) - 1;

    HDC dc = GetWindowDC(hWnd);
    if (!dc) {
        return;
    }

    HBRUSH capBrush = CreateSolidBrush(active ? g_cfg.captionActive : g_cfg.captionInactive);

    // Side and bottom borders go straight onto the window DC.
    RECT left = {0, client.top, client.left, H};
    RECT right = {client.right + scrollW, client.top, W, H};
    RECT bottom = {0, client.bottom + scrollH, W, H};
    FillRect(dc, &left, capBrush);
    FillRect(dc, &right, capBrush);
    FillRect(dc, &bottom, capBrush);

    // Title area is double buffered to avoid flicker.
    HDC mem = CreateCompatibleDC(dc);
    HBITMAP bmp = CreateCompatibleBitmap(dc, W, client.top);
    HBITMAP oldBmp = (HBITMAP)SelectObject(mem, bmp);

    RECT top = {0, 0, W, client.top};
    FillRect(mem, &top, capBrush);

    // Buttons
    const int pad = MulDiv(6, dpi, 96);
    int textRight = W - pad;
    for (int i = 0; i < 3; i++) {
        RECT b;
        DWORD state;
        if (!GetButton(hWnd, fi, i, &b, &state)) {
            continue;
        }
        if (b.left - pad < textRight) {
            textRight = b.left - pad;
        }

        const bool disabled = (state & STATE_SYSTEM_UNAVAILABLE) != 0;
        COLORREF glyph = disabled ? kGlyphDisabled : (active ? g_cfg.glyph : g_cfg.glyphInactive);

        if (!disabled && (pressed == i || hot == i)) {
            COLORREF bg = (i == 2) ? (pressed == i ? g_cfg.closePressed : g_cfg.closeHot)
                                   : (pressed == i ? g_cfg.buttonPressed : g_cfg.buttonHot);
            HBRUSH bb = CreateSolidBrush(bg);
            FillRect(mem, &b, bb);
            DeleteObject(bb);
            glyph = RGB(255, 255, 255);
        }
        DrawGlyph(mem, (i == 0 && fi.iconic) ? 3 : i, b, glyph, dpi);
    }

    // Icon, centered vertically
    int textLeft = fi.iconic ? MulDiv(6, dpi, 96) : client.left + MulDiv(2, dpi, 96);
    HICON icon = (HICON)SendMessageW(hWnd, WM_GETICON, ICON_SMALL, 0);
    if (!icon) {
        icon = (HICON)GetClassLongPtrW(hWnd, GCLP_HICONSM);
    }
    if (icon) {
        int iw = GetSystemMetricsForDpi(SM_CXSMICON, dpi);
        int ih = GetSystemMetricsForDpi(SM_CYSMICON, dpi);
        DrawIconEx(mem, textLeft, fi.centerY - ih / 2, icon, iw, ih, 0, nullptr, DI_NORMAL);
        textLeft += iw + MulDiv(fi.iconic ? 4 : 6, dpi, 96);
    }

    // Title, centered vertically
    wchar_t title[256];
    int len = GetWindowTextW(hWnd, title, ARRAYSIZE(title));
    if (len > 0 && textRight > textLeft) {
        NONCLIENTMETRICSW ncm = {};
        ncm.cbSize = sizeof(ncm);
        SystemParametersInfoForDpi(SPI_GETNONCLIENTMETRICS, sizeof(ncm), &ncm, 0, dpi);
        HFONT font = CreateFontIndirectW(&ncm.lfCaptionFont);
        HFONT oldFont = (HFONT)SelectObject(mem, font);
        SetBkMode(mem, TRANSPARENT);
        SetTextColor(mem, active ? g_cfg.textActive : g_cfg.textInactive);
        RECT tr = {textLeft, 1, textRight, fi.capBottom};
        DrawTextW(mem, title, len, &tr,
                  DT_SINGLELINE | DT_VCENTER | DT_LEFT | DT_END_ELLIPSIS | DT_NOPREFIX);
        SelectObject(mem, oldFont);
        DeleteObject(font);
    }

    // Outer 1px line (top part, buffered)
    HBRUSH outerBrush = CreateSolidBrush(g_cfg.border);
    RECT l1 = {0, 0, W, 1};
    RECT l2 = {0, 0, 1, client.top};
    RECT l3 = {W - 1, 0, W, client.top};
    FillRect(mem, &l1, outerBrush);
    FillRect(mem, &l2, outerBrush);
    FillRect(mem, &l3, outerBrush);
    if (fi.iconic) {
        RECT l4 = {0, H - 1, W, H};
        FillRect(mem, &l4, outerBrush);
    }

    BitBlt(dc, 0, 0, W, client.top, mem, 0, 0, SRCCOPY);

    SelectObject(mem, oldBmp);
    DeleteObject(bmp);
    DeleteDC(mem);

    if (!fi.iconic) {
        // Outer line (sides, bottom) and thin dark edge around the client area.
        RECT s1 = {0, client.top, 1, H};
        RECT s2 = {W - 1, client.top, W, H};
        RECT s3 = {0, H - 1, W, H};
        FillRect(dc, &s1, outerBrush);
        FillRect(dc, &s2, outerBrush);
        FillRect(dc, &s3, outerBrush);

        HBRUSH innerBrush = CreateSolidBrush(g_cfg.clientEdge);
        RECT e1 = {client.left - 1, client.top - 1, client.right + 1, client.top};
        RECT e2 = {client.left - 1, client.top, client.left, client.bottom};
        RECT e3 = {client.right, client.top, client.right + 1, client.bottom};
        RECT e4 = {client.left - 1, client.bottom, client.right + 1, client.bottom + 1};
        if (scrollW == 0) {
            FillRect(dc, &e3, innerBrush);
        }
        if (scrollH == 0) {
            FillRect(dc, &e4, innerBrush);
        }
        FillRect(dc, &e1, innerBrush);
        FillRect(dc, &e2, innerBrush);
        DeleteObject(innerBrush);
    }
    DeleteObject(outerBrush);

    DeleteObject(capBrush);
    ReleaseDC(hWnd, dc);
}

// Our own hit test for the three caption buttons: returns the button index or -1.
static int ButtonAtScreenPoint(HWND hWnd, POINT pt, DWORD* state) {
    FrameInfo fi;
    if (!GetFrameInfo(hWnd, &fi)) {
        return -1;
    }
    POINT local = {pt.x - fi.screen.left, pt.y - fi.screen.top};
    for (int i = 0; i < 3; i++) {
        RECT b;
        DWORD st;
        if (GetButton(hWnd, fi, i, &b, &st) && PtInRect(&b, local)) {
            *state = st;
            return i;
        }
    }
    return -1;
}

static int ButtonFromHitTest(WPARAM hit) {
    for (int i = 0; i < 3; i++) {
        if ((LRESULT)hit == kBtnHit[i]) {
            return i;
        }
    }
    return -1;
}

// Replacement for the default press-and-release tracking of a caption button.
static void TrackButton(HWND hWnd, int idx) {
    SetState(hWnd, kPressedProp, idx + 1);
    SetCapture(hWnd);
    PaintFrame(hWnd);

    bool over = true;
    bool released = false;
    MSG msg;
    while (GetCapture() == hWnd && IsWindow(hWnd) && GetMessageW(&msg, nullptr, 0, 0) > 0) {
        if (msg.message == WM_LBUTTONUP || msg.message == WM_NCLBUTTONUP) {
            released = true;
            break;
        }
        if (msg.message == WM_MOUSEMOVE || msg.message == WM_NCMOUSEMOVE) {
            POINT pt;
            GetCursorPos(&pt);
            DWORD st;
            bool nowOver = ButtonAtScreenPoint(hWnd, pt, &st) == idx;
            if (nowOver != over) {
                over = nowOver;
                SetState(hWnd, kPressedProp, over ? idx + 1 : 0);
                PaintFrame(hWnd);
            }
            continue;
        }
        if (msg.message == WM_KEYDOWN && msg.wParam == VK_ESCAPE) {
            break;
        }
        if (msg.message == WM_QUIT) {
            PostQuitMessage((int)msg.wParam);
            break;
        }
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    if (GetCapture() == hWnd) {
        ReleaseCapture();
    }
    if (!IsWindow(hWnd)) {
        return;
    }
    SetState(hWnd, kPressedProp, 0);
    SetState(hWnd, kHotProp, 0);

    if (released && over) {
        WPARAM cmd;
        if (idx == 0) {
            cmd = IsIconic(hWnd) ? SC_RESTORE : SC_MINIMIZE;
        } else if (idx == 1) {
            cmd = SC_MAXIMIZE;
        } else {
            cmd = SC_CLOSE;
        }
        PostMessageW(hWnd, WM_SYSCOMMAND, cmd, 0);
    }
    if (ShouldHandle(hWnd)) {
        PaintFrame(hWnd);
    }
}

static LRESULT HandleMessage(DefMDIChildProc_t orig, HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == WM_NCDESTROY) {
        RemovePropW(hWnd, kHotProp);
        RemovePropW(hWnd, kPressedProp);
        RemovePropW(hWnd, kActiveProp);
        return orig(hWnd, msg, wParam, lParam);
    }

    // Activation state can arrive while the window is not drawable by us yet.
    if (msg == WM_MDIACTIVATE) {
        SetState(hWnd, kActiveProp, (HWND)lParam == hWnd ? 1 : 2);
    }

    if (!ShouldHandle(hWnd)) {
        if (msg == WM_NCACTIVATE) {
            SetState(hWnd, kActiveProp, wParam ? 1 : 2);
        }
        return orig(hWnd, msg, wParam, lParam);
    }

    switch (msg) {
        case WM_NCPAINT:
            PaintFrame(hWnd);
            return 0;

        case WM_NCACTIVATE: {
            SetState(hWnd, kActiveProp, wParam ? 1 : 2);
            // lParam = -1 stops Windows from painting its own caption.
            LRESULT res = orig(hWnd, msg, wParam, (LPARAM)-1);
            PaintFrame(hWnd);
            return res;
        }

        case WM_MDIACTIVATE:
        case WM_ACTIVATE:
        case WM_SIZE:
        case WM_SHOWWINDOW:
        case WM_STYLECHANGED:
        case WM_THEMECHANGED:
        case WM_DWMCOMPOSITIONCHANGED:
        case WM_SETICON: {
            LRESULT res = orig(hWnd, msg, wParam, lParam);
            if (ShouldHandle(hWnd)) {
                PaintFrame(hWnd);
            }
            return res;
        }

        case WM_SETTEXT: {
            // Hide the window from the default repaint, then paint ourselves.
            LONG style = GetWindowLongW(hWnd, GWL_STYLE);
            SetWindowLongW(hWnd, GWL_STYLE, style & ~WS_VISIBLE);
            LRESULT res = orig(hWnd, msg, wParam, lParam);
            SetWindowLongW(hWnd, GWL_STYLE, style);
            PaintFrame(hWnd);
            return res;
        }

        case WM_NCHITTEST: {
            LRESULT res = orig(hWnd, msg, wParam, lParam);
            POINT pt = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
            DWORD st;
            int btn = ButtonAtScreenPoint(hWnd, pt, &st);
            if (btn >= 0) {
                return (st & STATE_SYSTEM_UNAVAILABLE) ? HTCAPTION : kBtnHit[btn];
            }
            // Windows' own button layout differs from what we draw.
            if (res == HTMINBUTTON || res == HTMAXBUTTON || res == HTCLOSE || res == HTHELP) {
                return HTCAPTION;
            }
            return res;
        }

        case WM_NCMOUSEMOVE: {
            int btn = ButtonFromHitTest(wParam);
            if (GetState(hWnd, kHotProp) != btn + 1) {
                SetState(hWnd, kHotProp, btn + 1);
                PaintFrame(hWnd);
            }
            TRACKMOUSEEVENT tme = {sizeof(tme), TME_LEAVE | TME_NONCLIENT, hWnd, 0};
            TrackMouseEvent(&tme);
            return 0;  // keep Windows from painting its own hover state
        }

        case WM_NCMOUSELEAVE:
            if (GetState(hWnd, kHotProp)) {
                SetState(hWnd, kHotProp, 0);
                PaintFrame(hWnd);
            }
            return 0;

        case WM_NCLBUTTONDOWN:
        case WM_NCLBUTTONDBLCLK: {  // a double-click on a button counts as another press
            int btn = ButtonFromHitTest(wParam);
            if (btn >= 0) {
                TrackButton(hWnd, btn);
                return 0;
            }
            break;
        }
    }

    return orig(hWnd, msg, wParam, lParam);
}

static LRESULT WINAPI DefMDIChildProcW_hook(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    EnsureOptionsHook();
    return HandleMessage(DefMDIChildProcW_orig, hWnd, msg, wParam, lParam);
}

static LRESULT WINAPI DefMDIChildProcA_hook(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    return HandleMessage(DefMDIChildProcA_orig, hWnd, msg, wParam, lParam);
}

// ---- Flash fix for the tool options bar ----------------------------------------
// When PaintShop Pro rebuilds the drop-down lists of the tool options bar (for
// example when another image is activated), comctl32's ComboBox briefly fills its
// selection field with the default white brush before PSP paints its own dark look.
// Fill it with a dark gray instead.

typedef BOOL(WINAPI* PatBlt_t)(HDC, int, int, int, int, DWORD);
static PatBlt_t PatBlt_orig;

static bool IsPspComboBox(HWND hWnd) {
    wchar_t cls[16];
    if (!GetClassNameW(hWnd, cls, ARRAYSIZE(cls)) || _wcsicmp(cls, L"ComboBox") != 0) {
        return false;
    }
    HWND root = GetAncestor(hWnd, GA_ROOT);
    wchar_t rootCls[64];
    return root && GetClassNameW(root, rootCls, ARRAYSIZE(rootCls)) && wcsstr(rootCls, L"PSPMainFrame");
}

static BOOL WINAPI PatBlt_hook(HDC hdc, int x, int y, int w, int h, DWORD rop) {
    if (g_cfg.fixComboFlash && rop == PATCOPY && w >= 20 && w <= 400 && h >= 8 && h <= 40) {
        LOGBRUSH lb;
        HGDIOBJ brush = GetCurrentObject(hdc, OBJ_BRUSH);
        if (brush && GetObjectW(brush, sizeof(lb), &lb) == sizeof(lb) && lb.lbStyle == BS_SOLID &&
            lb.lbColor == RGB(255, 255, 255)) {
            HWND wnd = WindowFromDC(hdc);
            if (wnd && IsPspComboBox(wnd)) {
                // The stock DC brush can neither leak nor be freed while another thread draws with it.
                HGDIOBJ oldBrush = SelectObject(hdc, GetStockObject(DC_BRUSH));
                COLORREF oldColor = SetDCBrushColor(hdc, g_cfg.flashColor);
                BOOL res = PatBlt_orig(hdc, x, y, w, h, rop);
                SetDCBrushColor(hdc, oldColor);
                SelectObject(hdc, oldBrush);
                return res;
            }
        }
    }
    return PatBlt_orig(hdc, x, y, w, h, rop);
}


// ---- Tool options bar: no half-updated controls -----------------------------------
// On an image switch PaintShop Pro empties, refills and enables / disables the controls of the tool
// options bar one by one, and every step is painted: the bar flickers. While such a change is going
// on, the bar is not redrawn (LockWindowUpdate; the bar is drawn once, when things have
// settled 80 ms after the last change). Nothing else is touched: only the rebar of the tool options bar.

static const UINT_PTR kThawTimer = 0xF1A5;
static const UINT kSettleMs = 80;       // quiet time after the last change before the bar is drawn
static const ULONGLONG kMaxHoldMs = 250;  // the longest the bar is held back
static const wchar_t kFrozenProp[] = L"PspDarkMdi.Frozen";
static HHOOK g_optionsHook;
static UINT g_teardownMsg;  // sent to the main frame: the hook is taken down on the thread that runs it
static void ThawAll();

static bool IsMainFrameClass(HWND w) {
    wchar_t cls[64];
    return w && GetClassNameW(w, cls, ARRAYSIZE(cls)) && wcsstr(cls, L"PSPMainFrame");
}

static BOOL CALLBACK FindMainFrame(HWND h, LPARAM l) {
    DWORD pid = 0;
    GetWindowThreadProcessId(h, &pid);
    if (pid == GetCurrentProcessId() && IsMainFrameClass(h)) {
        *(HWND*)l = h;
        return FALSE;
    }
    return TRUE;
}

static void CALLBACK ThawProc(HWND hWnd, UINT, UINT_PTR id, DWORD) {
    KillTimer(hWnd, id);
    RemovePropW(hWnd, kFrozenProp);
    LockWindowUpdate(nullptr);
    // no RDW_ERASE: an edit field would fill its background white before it draws the text
    RedrawWindow(hWnd, nullptr, nullptr, RDW_INVALIDATE | RDW_ALLCHILDREN | RDW_UPDATENOW);
}

// the dialog in the rebar that holds the control (nullptr for anything else)
static HWND OptionsDialog(HWND hWnd) {
    HWND best = nullptr;
    wchar_t cls[48];
    for (HWND w = GetParent(hWnd); w; w = GetParent(w)) {
        if (!GetClassNameW(w, cls, ARRAYSIZE(cls))) {
            return nullptr;
        }
        if (wcsstr(cls, L"PSPMainFrame")) {
            HWND par = best ? GetParent(best) : nullptr;
            wchar_t pc[48];
            if (par && GetClassNameW(par, pc, ARRAYSIZE(pc)) && wcscmp(pc, L"ReBarWindow32") == 0) {
                return best;
            }
            return nullptr;
        }
        if (wcscmp(cls, L"#32770") == 0) {
            best = w;
        }
    }
    return nullptr;
}

// messages with which PaintShop Pro changes the state of a control
static bool IsStateChange(HWND hWnd, UINT msg, WPARAM wParam) {
    wchar_t cls[32];
    if (!GetClassNameW(hWnd, cls, ARRAYSIZE(cls))) {
        return false;
    }
    const bool combo = _wcsicmp(cls, L"ComboBox") == 0;
    const bool edit = _wcsicmp(cls, L"Edit") == 0;
    const bool updown = _wcsicmp(cls, L"msctls_updown32") == 0;
    const bool wrapper = _wcsicmp(cls, L"AfxWnd140u") == 0;
    switch (msg) {
        case CB_RESETCONTENT:
        case CB_SETCURSEL:
            return combo;
        case WM_SETTEXT:
            return edit;
        case UDM_SETPOS:
        case UDM_SETPOS32:
            return updown;
        case WM_SETREDRAW:
            return wrapper && wParam == FALSE;
        case WM_ENABLE:
            return combo || edit || updown || wrapper || _wcsicmp(cls, L"Button") == 0;
    }
    return false;
}

static LRESULT CALLBACK OptionsHookProc(int code, WPARAM wParam, LPARAM lParam) {
    if (code == HC_ACTION && lParam && g_teardownMsg && ((const CWPSTRUCT*)lParam)->message == g_teardownMsg) {
        // runs on the UI thread, where the lock belongs: release it and take the hook down
        HHOOK hook = g_optionsHook;
        g_optionsHook = nullptr;
        ThawAll();
        if (hook) {
            UnhookWindowsHookEx(hook);
        }
        return 0;
    }
    if (code == HC_ACTION && lParam && g_cfg.freezeBar) {
        const CWPSTRUCT* m = (const CWPSTRUCT*)lParam;
        switch (m->message) {
            case CB_RESETCONTENT:
            case CB_SETCURSEL:
            case WM_SETTEXT:
            case UDM_SETPOS:
            case UDM_SETPOS32:
            case WM_SETREDRAW:
            case WM_ENABLE:
                if (IsStateChange(m->hwnd, m->message, m->wParam)) {
                    HWND dlg = OptionsDialog(m->hwnd);
                    HWND bar = dlg ? GetParent(dlg) : nullptr;  // the rebar that holds all dialogs of the bar
                    if (bar) {
                        const ULONGLONG now = GetTickCount64();
                        HANDLE first = GetPropW(bar, kFrozenProp);
                        if (!first) {
                            // LockWindowUpdate keeps what is on screen (SETREDRAW would leave holes)
                            if (LockWindowUpdate(bar)) {
                                SetPropW(bar, kFrozenProp, (HANDLE)(ULONG_PTR)now);
                                SetTimer(bar, kThawTimer, kSettleMs, ThawProc);
                            }
                        } else if (now - (ULONGLONG)(ULONG_PTR)first < kMaxHoldMs) {
                            SetTimer(bar, kThawTimer, kSettleMs, ThawProc);  // restart while PSP keeps changing things
                        }  // after kMaxHoldMs the pending timer runs out: a held spin button must keep updating
                    }
                }
                break;
        }
    }
    return CallNextHookEx(nullptr, code, wParam, lParam);
}

// installed from the first image window on (the UI thread of PaintShop Pro), not during startup
static void EnsureOptionsHook() {
    if (g_optionsHook || !g_cfg.freezeBar) {
        return;
    }
    HMODULE self = nullptr;
    if (GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                           (LPCWSTR)&OptionsHookProc, &self)) {
        g_optionsHook = SetWindowsHookExW(WH_CALLWNDPROC, OptionsHookProc, self, GetCurrentThreadId());
    }
}

static BOOL CALLBACK ThawChild(HWND hWnd, LPARAM) {
    if (GetPropW(hWnd, kFrozenProp)) {
        ThawProc(hWnd, 0, kThawTimer, 0);
    }
    return TRUE;
}

static BOOL CALLBACK ThawTopLevel(HWND hWnd, LPARAM) {
    DWORD pid = 0;
    GetWindowThreadProcessId(hWnd, &pid);
    if (pid == GetCurrentProcessId()) {
        EnumChildWindows(hWnd, ThawChild, 0);
    }
    return TRUE;
}

static void ThawAll() {
    EnumWindows(ThawTopLevel, 0);
}

// Takes the hook down on the UI thread of PaintShop Pro (the lock on the window and the timers belong to
// that thread).
static void RemoveOptionsHook() {
    if (!g_optionsHook) {
        return;
    }
    HWND frame = nullptr;
    EnumWindows(FindMainFrame, (LPARAM)&frame);
    if (frame && g_teardownMsg) {
        // wait for the UI thread (it may be busy): a timer of the mod must not outlive the mod. This cannot
        // deadlock, the UI thread never waits for the engine thread, and the call returns when the window goes away.
        SendMessageW(frame, g_teardownMsg, 0, 0);
    }
    if (g_optionsHook) {  // no main frame: its child windows and their timers are gone too
        UnhookWindowsHookEx(g_optionsHook);
        g_optionsHook = nullptr;
    }
}

// ---- Mod lifecycle ------------------------------------------------------------

static bool g_unloading;

static BOOL CALLBACK RedrawMdiChild(HWND hWnd, LPARAM) {
    if (GetWindowLongW(hWnd, GWL_EXSTYLE) & WS_EX_MDICHILD) {
        if (g_unloading) {
            // the per-window state must not outlive the mod (it would be stale when the mod is enabled again)
            RemovePropW(hWnd, kHotProp);
            RemovePropW(hWnd, kPressedProp);
            RemovePropW(hWnd, kActiveProp);
        }
        RedrawWindow(hWnd, nullptr, nullptr, RDW_FRAME | RDW_INVALIDATE);
    }
    return TRUE;
}

static BOOL CALLBACK RedrawTopLevel(HWND hWnd, LPARAM) {
    DWORD pid = 0;
    GetWindowThreadProcessId(hWnd, &pid);
    if (pid == GetCurrentProcessId()) {
        EnumChildWindows(hWnd, RedrawMdiChild, 0);
    }
    return TRUE;
}


// ---- Darker interface ----------------------------------------------------------------
// PaintShop Pro's dark theme uses neutral grays around #2F2F2F; other dark apps use about #1A1A1A. The
// colors the program itself picks for brushes, pens and backgrounds are made darker by one factor. Light
// colors (text, icons), near black and everything with a hue stay as they are, and so does the content of
// the image windows (it is not drawn with these colors).

static HMODULE g_exeModule;
static wchar_t g_exeDir[MAX_PATH];
static size_t g_exeDirLen;
static std::atomic<uintptr_t> g_callerCache[1024];  // address and kind in one word (the low 2 bits)

// true when the code that asks is PaintShop Pro itself (its exe or a library in its own folder)
static bool IsAppCaller(void* ret) {
    const uintptr_t a = (uintptr_t)ret;
    const size_t slot = ((a >> 2) ^ (a >> 12)) & 1023;
    const uintptr_t v = g_callerCache[slot].load(std::memory_order_relaxed);
    if (v != 0 && (v & ~(uintptr_t)3) == (a & ~(uintptr_t)3)) {
        return (v & 3) == 1;
    }
    HMODULE m = nullptr;
    bool app = false;
    if (GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, (LPCWSTR)ret, &m) && m) {
        if (m == g_exeModule) {
            app = true;
        } else {
            wchar_t path[MAX_PATH];
            if (GetModuleFileNameW(m, path, MAX_PATH)) {
                const wchar_t* base = wcsrchr(path, (wchar_t)92);
                base = base ? base + 1 : path;
                // the MFC runtime draws the palettes and tool bars for the program
                if (_wcsnicmp(path, g_exeDir, g_exeDirLen) == 0 || _wcsicmp(base, L"mfc140u.dll") == 0) {
                    app = true;
                }
            }
        }
    }
    g_callerCache[slot].store((a & ~(uintptr_t)3) | (app ? 1 : 2), std::memory_order_relaxed);
    return app;
}

// colors of the orange family (hue 12 to 45 degrees, clearly saturated) become the accent color
static COLORREF AccentColor(COLORREF c) {
    if (!g_cfg.accentOn || (c & 0xFF000000)) {
        return c;
    }
    float h, sat, l;
    RgbToHslF(GetRValue(c) / 255.0f, GetGValue(c) / 255.0f, GetBValue(c) / 255.0f, &h, &sat, &l);
    if (sat < 0.45f || h < 12.0f || h > 45.0f || l < 0.2f || l > 0.8f) {
        return c;
    }
    const float nl = fminf(0.95f, fmaxf(0.03f, l + (g_cfg.accentL - 0.56f)));
    float r, g, b;
    HslToRgbF(g_cfg.accentH, g_cfg.accentS, nl, &r, &g, &b);
    return RGB((int)(r * 255.0f + 0.5f), (int)(g * 255.0f + 0.5f), (int)(b * 255.0f + 0.5f));
}

static COLORREF DarkenColor(COLORREF c) {
    if (c & 0xFF000000) {  // palette index, system color index
        return c;
    }
    const int r = GetRValue(c), g = GetGValue(c), b = GetBValue(c);
    const int mx = std::max(r, std::max(g, b)), mn = std::min(r, std::min(g, b));
    if (mx - mn > 16) {
        return c;  // has a hue
    }
    const int v = (r + g + b) / 3;
    if (v < 0x1A || v > 0x48 || g_cfg.darkness >= 100) {
        return c;
    }
    const float f = g_cfg.darkness / 100.0f;
    return RGB((int)(r * f + 0.5f), (int)(g * f + 0.5f), (int)(b * f + 0.5f));
}

static COLORREF Dark(COLORREF c, void* ret) {
    return IsAppCaller(ret) ? DarkenColor(c) : c;
}

typedef HBRUSH(WINAPI* CreateSolidBrush_t)(COLORREF);
static CreateSolidBrush_t CreateSolidBrush_orig;
typedef DWORD(WINAPI* GetSysColor_t)(int);
static GetSysColor_t GetSysColor_orig;

// the critical section lives as long as the DLL (hooks may still run on other threads while the mod unloads)
struct BrushSync {
    CRITICAL_SECTION cs;
    BrushSync() { InitializeCriticalSection(&cs); }
    ~BrushSync() { DeleteCriticalSection(&cs); }
};
static BrushSync g_brushSync;

struct BrushLock {
    BrushLock() { EnterCriticalSection(&g_brushSync.cs); }
    ~BrushLock() { LeaveCriticalSection(&g_brushSync.cs); }
};

static std::unordered_map<HBRUSH, COLORREF> g_darkBrushes;    // brushes with the darker color -> the color that was asked for
static std::unordered_map<COLORREF, HBRUSH> g_modBrushes;     // brushes made by the mod, by color; kept alive (PaintShop Pro may cache them)
static std::unordered_map<ATOM, HBRUSH> g_origClassBrush;     // background brushes of window classes before the mod changed them

static COLORREF DarkenColorFrom(COLORREF c, int lowBound);

// colors below this are what the mod itself made: they must not be darkened a second time
static int DarkLowBound() {
    return std::max(0x1A, (int)(0x48 * g_cfg.darkness / 100.0f) + 1);
}

static void RememberDarkBrush(HBRUSH b, COLORREF original) {
    if (b) {
        BrushLock lock;
        g_darkBrushes[b] = original;
    }
}

static HBRUSH WINAPI CreateSolidBrush_hook(COLORREF c) {
    const COLORREF n = Dark(c, __builtin_return_address(0));
    HBRUSH b = CreateSolidBrush_orig(n);
    if (n != c) {
        RememberDarkBrush(b, c);
    }
    return b;
}

// a brush of the mod with this color
static HBRUSH ModBrush(COLORREF n, COLORREF original) {
    BrushLock lock;
    HBRUSH& slot = g_modBrushes[n];
    if (!slot) {
        slot = CreateSolidBrush_orig(n);
    }
    if (slot) {
        g_darkBrushes.emplace(slot, original);
    }
    return slot;
}

// the background brush of a window class: windows fill their empty area with it themselves
static HBRUSH DarkClassBrush(HBRUSH b, bool ownBrush = false) {
    if (!b) {
        return b;
    }
    COLORREF c;
    COLORREF n;
    if ((ULONG_PTR)b <= 32) {  // a system color index + 1
        c = GetSysColor_orig((int)(ULONG_PTR)b - 1);
        n = DarkenColor(c);
    } else {
        {
            BrushLock lock;
            if (g_darkBrushes.count(b)) {
                return b;
            }
        }
        LOGBRUSH lb;
        if (GetObjectW(b, sizeof(lb), &lb) != sizeof(lb) || lb.lbStyle != BS_SOLID) {
            return b;
        }
        c = lb.lbColor;
        n = DarkenColorFrom(c, DarkLowBound());  // a brush of an earlier run of the mod is already dark
    }
    if (n == c) {
        return b;
    }
    if (ownBrush) {
        // every class gets its own brush: Windows deletes the background brush when the class is unregistered
        HBRUSH own = CreateSolidBrush_orig(n);
        RememberDarkBrush(own, c);
        return own ? own : b;
    }
    return ModBrush(n, c);
}

static BOOL CALLBACK PatchClassBrushEnum(HWND hWnd, LPARAM) {
    const HBRUSH old = (HBRUSH)GetClassLongPtrW(hWnd, GCLP_HBRBACKGROUND);
    const HBRUSH nb = DarkClassBrush(old, true);
    if (nb != old) {
        const ATOM atom = (ATOM)GetClassLongPtrW(hWnd, GCW_ATOM);
        {
            BrushLock lock;
            g_origClassBrush.emplace(atom, old);  // the first one is the real original
        }
        SetClassLongPtrW(hWnd, GCLP_HBRBACKGROUND, (LONG_PTR)nb);
    }
    return TRUE;
}

static BOOL CALLBACK PatchClassBrushTop(HWND hWnd, LPARAM) {
    DWORD pid = 0;
    GetWindowThreadProcessId(hWnd, &pid);
    if (pid == GetCurrentProcessId()) {
        PatchClassBrushEnum(hWnd, 0);
        EnumChildWindows(hWnd, PatchClassBrushEnum, 0);
    }
    return TRUE;
}

// puts the original background brushes back (the brushes of the mod stay alive)
static BOOL CALLBACK RestoreClassBrushEnum(HWND hWnd, LPARAM) {
    const ATOM atom = (ATOM)GetClassLongPtrW(hWnd, GCW_ATOM);
    HBRUSH original = nullptr;
    bool ours = false;
    const HBRUSH current = (HBRUSH)GetClassLongPtrW(hWnd, GCLP_HBRBACKGROUND);
    {
        BrushLock lock;
        auto it = g_origClassBrush.find(atom);
        if (it == g_origClassBrush.end()) {
            return TRUE;
        }
        original = it->second;
        ours = g_darkBrushes.count(current) != 0;
    }
    if (ours && current != original) {
        SetClassLongPtrW(hWnd, GCLP_HBRBACKGROUND, (LONG_PTR)original);
    }
    return TRUE;
}

static BOOL CALLBACK RestoreClassBrushTop(HWND hWnd, LPARAM) {
    DWORD pid = 0;
    GetWindowThreadProcessId(hWnd, &pid);
    if (pid == GetCurrentProcessId()) {
        RestoreClassBrushEnum(hWnd, 0);
        EnumChildWindows(hWnd, RestoreClassBrushEnum, 0);
    }
    return TRUE;
}

typedef ATOM(WINAPI* RegisterClassW_t)(const WNDCLASSW*);
static RegisterClassW_t RegisterClassW_orig;
static ATOM WINAPI RegisterClassW_hook(const WNDCLASSW* wc) {
    if (wc && IsAppCaller(__builtin_return_address(0))) {
        WNDCLASSW copy = *wc;
        copy.hbrBackground = DarkClassBrush(wc->hbrBackground, true);
        const ATOM atom = RegisterClassW_orig(&copy);
        if (atom && copy.hbrBackground != wc->hbrBackground) {
            BrushLock lock;
            g_origClassBrush.emplace(atom, wc->hbrBackground);
        }
        return atom;
    }
    return RegisterClassW_orig(wc);
}

typedef ATOM(WINAPI* RegisterClassExW_t)(const WNDCLASSEXW*);
static RegisterClassExW_t RegisterClassExW_orig;
static ATOM WINAPI RegisterClassExW_hook(const WNDCLASSEXW* wc) {
    if (wc && IsAppCaller(__builtin_return_address(0))) {
        WNDCLASSEXW copy = *wc;
        copy.hbrBackground = DarkClassBrush(wc->hbrBackground, true);
        const ATOM atom = RegisterClassExW_orig(&copy);
        if (atom && copy.hbrBackground != wc->hbrBackground) {
            BrushLock lock;
            g_origClassBrush.emplace(atom, wc->hbrBackground);
        }
        return atom;
    }
    return RegisterClassExW_orig(wc);
}

typedef HBRUSH(WINAPI* CreateBrushIndirect_t)(const LOGBRUSH*);
static CreateBrushIndirect_t CreateBrushIndirect_orig;
static HBRUSH WINAPI CreateBrushIndirect_hook(const LOGBRUSH* lb) {
    if (lb && lb->lbStyle == BS_SOLID) {
        LOGBRUSH copy = *lb;
        copy.lbColor = Dark(lb->lbColor, __builtin_return_address(0));
        HBRUSH b = CreateBrushIndirect_orig(&copy);
        if (copy.lbColor != lb->lbColor) {
            RememberDarkBrush(b, lb->lbColor);
        }
        return b;
    }
    return CreateBrushIndirect_orig(lb);
}

// GDI hands out the same handle values again: a deleted brush must not stay in the list
typedef BOOL(WINAPI* DeleteObject_t)(HGDIOBJ);
static DeleteObject_t DeleteObject_orig;
static BOOL WINAPI DeleteObject_hook(HGDIOBJ obj) {
    if (GetObjectType(obj) == OBJ_BRUSH) {  // bitmaps, DCs, fonts and pens skip the lock
        BrushLock lock;
        if (!g_darkBrushes.empty()) {
            g_darkBrushes.erase((HBRUSH)obj);
        }
    }
    return DeleteObject_orig(obj);
}

typedef HPEN(WINAPI* CreatePen_t)(int, int, COLORREF);
static CreatePen_t CreatePen_orig;
static HPEN WINAPI CreatePen_hook(int style, int width, COLORREF c) {
    return CreatePen_orig(style, width, Dark(c, __builtin_return_address(0)));
}

typedef HPEN(WINAPI* CreatePenIndirect_t)(const LOGPEN*);
static CreatePenIndirect_t CreatePenIndirect_orig;
static HPEN WINAPI CreatePenIndirect_hook(const LOGPEN* lp) {
    if (lp) {
        LOGPEN copy = *lp;
        copy.lopnColor = Dark(lp->lopnColor, __builtin_return_address(0));
        return CreatePenIndirect_orig(&copy);
    }
    return CreatePenIndirect_orig(lp);
}

typedef COLORREF(WINAPI* SetBkColor_t)(HDC, COLORREF);
static SetBkColor_t SetBkColor_orig;
static COLORREF WINAPI SetBkColor_hook(HDC hdc, COLORREF c) {
    return SetBkColor_orig(hdc, Dark(c, __builtin_return_address(0)));
}

typedef COLORREF(WINAPI* SetDCBrushColor_t)(HDC, COLORREF);
static SetDCBrushColor_t SetDCBrushColor_orig;
static COLORREF WINAPI SetDCBrushColor_hook(HDC hdc, COLORREF c) {
    return SetDCBrushColor_orig(hdc, Dark(c, __builtin_return_address(0)));
}

typedef COLORREF(WINAPI* SetDCPenColor_t)(HDC, COLORREF);
static SetDCPenColor_t SetDCPenColor_orig;
static COLORREF WINAPI SetDCPenColor_hook(HDC hdc, COLORREF c) {
    return SetDCPenColor_orig(hdc, Dark(c, __builtin_return_address(0)));
}

static DWORD WINAPI GetSysColor_hook(int index) {
    const DWORD c = GetSysColor_orig(index);
    return Dark(c, __builtin_return_address(0));
}

static HBRUSH g_darkSysBrush[32];

typedef HBRUSH(WINAPI* GetSysColorBrush_t)(int);
static GetSysColorBrush_t GetSysColorBrush_orig;
static HBRUSH WINAPI GetSysColorBrush_hook(int index) {
    if (index >= 0 && index < 32 && IsAppCaller(__builtin_return_address(0))) {
        const COLORREF c = GetSysColor_orig(index);
        const COLORREF n = DarkenColor(c);
        if (n != c) {
            if (!g_darkSysBrush[index]) {
                g_darkSysBrush[index] = CreateSolidBrush_orig(n);
            }
            return g_darkSysBrush[index];
        }
    }
    return GetSysColorBrush_orig(index);
}

// the color swatches (the current material color, the colors of the color dialog) are small MFC windows. They
// show colors the user chose, which must not be changed.
static thread_local HWND g_swatchLastWnd;
static thread_local bool g_swatchLastIs;

static bool IsSwatchDC(HDC dc) {
    HWND wnd = WindowFromDC(dc);
    if (wnd != g_swatchLastWnd) {
        char cls[16] = "";
        RECT r = {};
        g_swatchLastIs = wnd && GetClassNameA(wnd, cls, sizeof(cls)) && strncmp(cls, "AfxWnd140u", 10) == 0 &&
                         GetWindowRect(wnd, &r) && r.right - r.left <= 90 && r.bottom - r.top <= 90;
        g_swatchLastWnd = wnd;
    }
    return g_swatchLastIs;
}

typedef int(WINAPI* FillRect_t)(HDC, const RECT*, HBRUSH);
static FillRect_t FillRect_orig;
static int WINAPI FillRect_hook(HDC hdc, const RECT* rc, HBRUSH br) {
    if ((ULONG_PTR)br > 32 && g_cfg.darkness < 100 && IsAppCaller(__builtin_return_address(0))) {
        COLORREF original = 0;
        bool known = false;
        {
            BrushLock lock;
            auto it = g_darkBrushes.find(br);
            if (it != g_darkBrushes.end()) {
                original = it->second;
                known = true;
            }
        }
        if (known && IsSwatchDC(hdc)) {
            HBRUSH exact = CreateSolidBrush_orig(original);
            const int result = FillRect_orig(hdc, rc, exact);
            DeleteObject_orig(exact);
            return result;
        }
    }
    if ((ULONG_PTR)br > 0 && (ULONG_PTR)br <= 32 && IsAppCaller(__builtin_return_address(0))) {
        const int index = (int)(ULONG_PTR)br - 1;  // a system color index, as in FillRect(dc, rc, COLOR_x + 1)
        const COLORREF c = GetSysColor_orig(index);
        const COLORREF n = DarkenColor(c);
        if (n != c && index < 32) {
            if (!g_darkSysBrush[index]) {
                g_darkSysBrush[index] = CreateSolidBrush_orig(n);
            }
            br = g_darkSysBrush[index];
        }
    }
    if ((ULONG_PTR)br > 32 && IsAppCaller(__builtin_return_address(0))) {
        br = DarkClassBrush(br);  // brushes that were made before the hooks (the MFC bar backgrounds)
    }
    return FillRect_orig(hdc, rc, br);
}

static void InstallDarkHooks() {
    HMODULE user32 = GetModuleHandleW(L"user32.dll");
    HMODULE gdi32 = GetModuleHandleW(L"gdi32.dll");
    g_exeModule = GetModuleHandleW(nullptr);
    if (GetModuleFileNameW(g_exeModule, g_exeDir, MAX_PATH)) {
        wchar_t* sep = wcsrchr(g_exeDir, (wchar_t)92);
        if (sep) {
            sep[1] = 0;
        }
        g_exeDirLen = wcslen(g_exeDir);
    }
    if (!user32 || !gdi32) {
        return;
    }
#define DARK_HOOK(mod, name) \
    Wh_SetFunctionHook((void*)GetProcAddress(mod, #name), (void*)name##_hook, (void**)&name##_orig)
    DARK_HOOK(gdi32, CreateSolidBrush);
    DARK_HOOK(gdi32, DeleteObject);
    DARK_HOOK(gdi32, CreateBrushIndirect);
    DARK_HOOK(gdi32, CreatePen);
    DARK_HOOK(gdi32, CreatePenIndirect);
    DARK_HOOK(gdi32, SetBkColor);
    DARK_HOOK(gdi32, SetDCBrushColor);
    DARK_HOOK(gdi32, SetDCPenColor);
    DARK_HOOK(user32, GetSysColor);
    DARK_HOOK(user32, GetSysColorBrush);
    DARK_HOOK(user32, FillRect);
    DARK_HOOK(user32, RegisterClassW);
    DARK_HOOK(user32, RegisterClassExW);
#undef DARK_HOOK
}


// ---- The bar backgrounds are bitmaps ---------------------------------------------------
// The dock panes (tool bar band, tool column) and the tool bars of the palettes are filled with small
// bitmaps that are stretched or copied. For these windows the bitmap is copied to a temporary one,
// the dark grays in it are made darker, and that is drawn instead.

static thread_local HWND g_barLastWnd;
static thread_local bool g_barLastIs;

static bool IsBarDC(HDC d) {
    if (GetObjectType(d) != OBJ_DC) {
        return false;
    }
    HWND wnd = WindowFromDC(d);
    if (wnd != g_barLastWnd) {
        char cls[48] = "";
        if (wnd) {
            GetClassNameA(wnd, cls, sizeof(cls));
        }
        g_barLastIs = strncmp(cls, "Afx:DockPane", 12) == 0 || strncmp(cls, "Afx:ToolBar", 11) == 0;
        g_barLastWnd = wnd;
    }
    return g_barLastIs;
}

// like DarkenColor, but colors below lowBound are left alone: what the program has already painted with
// the darker colors must not be darkened a second time (the darker colors are at most 0x48 * factor)
static COLORREF DarkenColorFrom(COLORREF c, int lowBound) {
    const int r = GetRValue(c), g = GetGValue(c), b = GetBValue(c);
    const int mx = std::max(r, std::max(g, b)), mn = std::min(r, std::min(g, b));
    if (mx - mn > 16) {
        return c;
    }
    const int v = (r + g + b) / 3;
    if (v < lowBound || v > 0x48) {
        return c;
    }
    const float f = g_cfg.darkness / 100.0f;
    return RGB((int)(r * f + 0.5f), (int)(g * f + 0.5f), (int)(b * f + 0.5f));
}

static void DarkenBitmapBits(uint8_t* bits, int w, int h, int stride) {
    int lowBound = (int)(0x48 * g_cfg.darkness / 100.0f) + 1;
    if (lowBound < 0x1A) {
        lowBound = 0x1A;
    }
    uint32_t lastIn = 0xFFFFFFFFu, lastOut = 0;
    for (int y = 0; y < h; y++) {
        uint8_t* row = bits + (size_t)y * stride;
        for (int x = 0; x < w; x++) {
            uint8_t* px = row + x * 4;
            const uint32_t in = ((uint32_t)px[2] << 16) | ((uint32_t)px[1] << 8) | px[0];
            uint32_t out;
            if (in == lastIn) {
                out = lastOut;
            } else {
                const COLORREF c = DarkenColorFrom(RGB(px[2], px[1], px[0]), lowBound);
                out = ((uint32_t)GetRValue(c) << 16) | ((uint32_t)GetGValue(c) << 8) | GetBValue(c);
                lastIn = in;
                lastOut = out;
            }
            px[2] = (uint8_t)(out >> 16);
            px[1] = (uint8_t)(out >> 8);
            px[0] = (uint8_t)out;
        }
    }
}

// copies the source to a temporary bitmap in the size of the target, darkens it and draws it
static bool BlitDarker(HDC d, int x, int y, int w, int h, HDC src, int sx, int sy, int sw, int sh, DWORD rop, bool stretch);

typedef BOOL(WINAPI* BitBlt_t)(HDC, int, int, int, int, HDC, int, int, DWORD);
static BitBlt_t BitBlt_orig;
typedef BOOL(WINAPI* StretchBlt_t)(HDC, int, int, int, int, HDC, int, int, int, int, DWORD);
static StretchBlt_t StretchBlt_orig;

static bool BlitDarker(HDC d, int x, int y, int w, int h, HDC src, int sx, int sy, int sw, int sh, DWORD rop, bool stretch) {
    if (g_cfg.darkness >= 100 || !src || rop != SRCCOPY || w <= 0 || h <= 0 || w > 8192 || h > 8192 || !IsBarDC(d)) {
        return false;
    }
    BITMAPINFO bi = {};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = w;
    bi.bmiHeader.biHeight = -h;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;
    void* bits = nullptr;
    HBITMAP tmp = CreateDIBSection(d, &bi, DIB_RGB_COLORS, &bits, nullptr, 0);
    if (!tmp || !bits) {
        return false;
    }
    HDC mem = CreateCompatibleDC(d);
    HGDIOBJ old = SelectObject(mem, tmp);
    BOOL ok;
    if (stretch) {
        ok = StretchBlt_orig(mem, 0, 0, w, h, src, sx, sy, sw, sh, SRCCOPY);
    } else {
        ok = BitBlt_orig(mem, 0, 0, w, h, src, sx, sy, SRCCOPY);
    }
    if (ok) {
        DarkenBitmapBits((uint8_t*)bits, w, h, w * 4);
        ok = BitBlt_orig(d, x, y, w, h, mem, 0, 0, SRCCOPY);
    }
    SelectObject(mem, old);
    DeleteDC(mem);
    DeleteObject(tmp);
    return ok != FALSE;
}


// the tab control with the color wheel draws its whole area (including the lighter gray around the wheel) into a
// memory bitmap and copies that. The gray of that area is replaced by the darker gray.
static bool TabPaneBlit(HDC d, int x, int y, int w, int h, HDC src, int sx, int sy, DWORD rop) {
    if (g_cfg.darkness >= 100 || !src || rop != SRCCOPY || w < 300 || h < 200 || w > 4096 || h > 4096) {
        return false;
    }
    HWND wnd = WindowFromDC(d);
    char cls[24] = "";
    if (!wnd || !GetClassNameA(wnd, cls, sizeof(cls)) || (_stricmp(cls, "SysTabControl32") != 0 && _strnicmp(cls, "AfxWnd140u", 10) != 0)) {
        return false;
    }
    BITMAPINFO bi = {};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = w;
    bi.bmiHeader.biHeight = -h;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;
    void* bits = nullptr;
    HBITMAP tmp = CreateDIBSection(d, &bi, DIB_RGB_COLORS, &bits, nullptr, 0);
    if (!tmp || !bits) {
        return false;
    }
    HDC mem = CreateCompatibleDC(d);
    HGDIOBJ old = SelectObject(mem, tmp);
    BOOL ok = BitBlt_orig(mem, 0, 0, w, h, src, sx, sy, SRCCOPY);
    if (ok) {
        uint8_t* p = (uint8_t*)bits;
        const float f = g_cfg.darkness / 100.0f;
        for (size_t i = 0, n = (size_t)w * h; i < n; i++, p += 4) {
            const int b = p[0], g = p[1], r = p[2];
            if (r == 0x2F && g == 0x2F && b == 0x2F) {  // the gray around the wheel
                p[0] = p[1] = p[2] = (uint8_t)(r * f + 0.5f);
            }
        }
        ok = BitBlt_orig(d, x, y, w, h, mem, 0, 0, SRCCOPY);
    }
    SelectObject(mem, old);
    DeleteDC(mem);
    DeleteObject(tmp);
    return ok != FALSE;
}

static BOOL WINAPI BitBlt_hook(HDC d, int x, int y, int w, int h, HDC src, int sx, int sy, DWORD rop) {
    if (TabPaneBlit(d, x, y, w, h, src, sx, sy, rop)) {
        return TRUE;
    }
    if (BlitDarker(d, x, y, w, h, src, sx, sy, w, h, rop, false)) {
        return TRUE;
    }
    return BitBlt_orig(d, x, y, w, h, src, sx, sy, rop);
}

static BOOL WINAPI StretchBlt_hook(HDC d, int x, int y, int w, int h, HDC src, int sx, int sy, int sw, int sh, DWORD rop) {
    if (BlitDarker(d, x, y, w, h, src, sx, sy, sw, sh, rop, true)) {
        return TRUE;
    }
    return StretchBlt_orig(d, x, y, w, h, src, sx, sy, sw, sh, rop);
}

static void InstallBarHooks() {
    HMODULE gdi32 = GetModuleHandleW(L"gdi32.dll");
    if (!gdi32) {
        return;
    }
    Wh_SetFunctionHook((void*)GetProcAddress(gdi32, "BitBlt"), (void*)BitBlt_hook, (void**)&BitBlt_orig);
    Wh_SetFunctionHook((void*)GetProcAddress(gdi32, "StretchBlt"), (void*)StretchBlt_hook, (void**)&StretchBlt_orig);
}


// ---- Title bar and accent color ------------------------------------------------------------
// PaintShop Pro draws its own title bar with GDI+ into a bitmap: horizontal gray bands over the whole width,
// the orange tab and the orange line below it. PaintShop Pro draws them with fixed pixel sizes, also at a
// higher display scaling. Only fills at the top are looked at (the bands are 16 pixels high and end at
// 52 pixels, the tool bar below must stay as it is), so
// colors that the user works with (swatches, gradients, tool previews) are never changed.

typedef int(WINAPI* GdipFillRectangleI_t)(void*, void*, int, int, int, int);
static GdipFillRectangleI_t GdipFillRectangleI_orig;
typedef int(WINAPI* GdipGetBrushType_t)(void*, int*);
static GdipGetBrushType_t GdipGetBrushType_p;
typedef int(WINAPI* GdipCreateSolidFill_t)(DWORD, void**);
static GdipCreateSolidFill_t GdipCreateSolidFill_p;
typedef int(WINAPI* GdipDeleteBrush_t)(void*);
static GdipDeleteBrush_t GdipDeleteBrush_p;
typedef int(WINAPI* GdipGetSolidFillColor_t)(void*, DWORD*);
static GdipGetSolidFillColor_t GdipGetSolidFillColor_p;
typedef int(WINAPI* GdipGetLineColors_t)(void*, DWORD*);
static GdipGetLineColors_t GdipGetLineColors_p;
struct GpPointI {
    int X, Y;
};
typedef int(WINAPI* GdipCreateLineBrushI_t)(const GpPointI*, const GpPointI*, DWORD, DWORD, int, void**);
static GdipCreateLineBrushI_t GdipCreateLineBrushI_p;

static void* g_titleBrush;
static DWORD g_titleBrushArgb;

// the title bands span the whole window; a gradient preview of the same height is not as wide
static bool IsMainFrameWide(int w) {
    static std::atomic<HWND> cached;
    HWND frame = cached.load();
    if (!frame || !IsWindow(frame)) {
        frame = nullptr;
        EnumWindows(FindMainFrame, (LPARAM)&frame);
        cached = frame;
    }
    RECT r;
    return frame && GetWindowRect(frame, &r) && std::abs(w - (r.right - r.left)) <= 24;
}

static DWORD AccentArgb(DWORD argb) {
    const COLORREF c = AccentColor(RGB((argb >> 16) & 255, (argb >> 8) & 255, argb & 255));
    return (argb & 0xFF000000u) | ((DWORD)GetRValue(c) << 16) | ((DWORD)GetGValue(c) << 8) | GetBValue(c);
}

static void* TitleBrush() {
    const DWORD want = 0xFF000000u | ((DWORD)GetRValue(g_cfg.titleColor) << 16) |
                       ((DWORD)GetGValue(g_cfg.titleColor) << 8) | GetBValue(g_cfg.titleColor);
    {
        BrushLock lock;
        if (g_titleBrush && g_titleBrushArgb == want) {
            return g_titleBrush;
        }
    }
    void* nb = nullptr;
    if (GdipCreateSolidFill_p(want, &nb) != 0 || !nb) {
        BrushLock lock;
        return g_titleBrush;
    }
    void* result = nb;
    void* spare = nullptr;
    {
        BrushLock lock;
        if (g_titleBrush && g_titleBrushArgb == want) {  // another thread was faster
            spare = nb;
            result = g_titleBrush;
        } else {
            g_titleBrush = nb;  // a previous brush is not freed: another thread could be using it
            g_titleBrushArgb = want;
        }
    }
    if (spare) {
        GdipDeleteBrush_p(spare);
    }
    return result;
}

static int WINAPI GdipFillRectangleI_hook(void* g, void* brush, int x, int y, int w, int h) {
    if ((g_cfg.titleOn || g_cfg.accentOn) && y >= 0 && y + h <= 80 && h <= 40 && GdipGetBrushType_p &&
        GdipCreateSolidFill_p && GdipDeleteBrush_p) {
        int type = -1;
        if (GdipGetBrushType_p(brush, &type) == 0) {
            if (g_cfg.titleOn && x <= 8 && w >= 300 && h >= 14 && h <= 18 && y + h <= 56 && type == 4 &&
                IsMainFrameWide(w)) {
                // the title bar: three gradient bands of 16 pixels over the whole width
                void* tb = TitleBrush();
                if (tb) {
                    brush = tb;
                }
            } else if (g_cfg.accentOn) {
                void* temp = nullptr;
                if (type == 0 && GdipGetSolidFillColor_p) {
                    DWORD c = 0;
                    if (GdipGetSolidFillColor_p(brush, &c) == 0 && AccentArgb(c) != c) {
                        GdipCreateSolidFill_p(AccentArgb(c), &temp);
                    }
                } else if (type == 4 && GdipGetLineColors_p && GdipCreateLineBrushI_p) {
                    DWORD c[2] = {};
                    if (GdipGetLineColors_p(brush, c) == 0 && (AccentArgb(c[0]) != c[0] || AccentArgb(c[1]) != c[1])) {
                        const GpPointI p1 = {x, y}, p2 = {x, y + h};
                        GdipCreateLineBrushI_p(&p1, &p2, AccentArgb(c[0]), AccentArgb(c[1]), 0, &temp);
                    }
                }
                if (temp) {
                    const int result = GdipFillRectangleI_orig(g, temp, x, y, w, h);
                    GdipDeleteBrush_p(temp);
                    return result;
                }
            }
        }
    }
    return GdipFillRectangleI_orig(g, brush, x, y, w, h);
}

// the tab and the home button are rounded shapes, filled as paths
typedef int(WINAPI* GdipFillPath_t)(void*, void*, void*);
static GdipFillPath_t GdipFillPath_orig;
typedef int(WINAPI* GdipGetPathWorldBounds_t)(void*, float*, const void*, void*);
static GdipGetPathWorldBounds_t GdipGetPathWorldBounds_p;

static int WINAPI GdipFillPath_hook(void* g, void* brush, void* path) {
    if (g_cfg.accentOn && GdipGetBrushType_p && GdipGetSolidFillColor_p && GdipGetPathWorldBounds_p &&
        GdipCreateSolidFill_p && GdipDeleteBrush_p) {
        int type = -1;
        DWORD c = 0;
        if (GdipGetBrushType_p(brush, &type) == 0 && type == 0 && GdipGetSolidFillColor_p(brush, &c) == 0 &&
            AccentArgb(c) != c) {
            float b[4] = {};  // x, y, width, height
            void* temp = nullptr;
            if (GdipGetPathWorldBounds_p(path, b, nullptr, nullptr) == 0 && b[1] >= 0 && b[1] + b[3] <= 80 &&
                GdipCreateSolidFill_p(AccentArgb(c), &temp) == 0 && temp) {
                const int result = GdipFillPath_orig(g, temp, path);
                GdipDeleteBrush_p(temp);
                return result;
            }
        }
    }
    return GdipFillPath_orig(g, brush, path);
}

static void InstallTitleHooks() {
    HMODULE gdip = GetModuleHandleW(L"gdiplus.dll");
    if (!gdip) {
        gdip = LoadLibraryW(L"gdiplus.dll");
    }
    if (!gdip) {
        return;
    }
    GdipGetBrushType_p = (GdipGetBrushType_t)GetProcAddress(gdip, "GdipGetBrushType");
    GdipCreateSolidFill_p = (GdipCreateSolidFill_t)GetProcAddress(gdip, "GdipCreateSolidFill");
    GdipDeleteBrush_p = (GdipDeleteBrush_t)GetProcAddress(gdip, "GdipDeleteBrush");
    GdipGetSolidFillColor_p = (GdipGetSolidFillColor_t)GetProcAddress(gdip, "GdipGetSolidFillColor");
    GdipGetLineColors_p = (GdipGetLineColors_t)GetProcAddress(gdip, "GdipGetLineColors");
    GdipCreateLineBrushI_p = (GdipCreateLineBrushI_t)GetProcAddress(gdip, "GdipCreateLineBrushI");
    GdipGetPathWorldBounds_p = (GdipGetPathWorldBounds_t)GetProcAddress(gdip, "GdipGetPathWorldBounds");
    Wh_SetFunctionHook((void*)GetProcAddress(gdip, "GdipFillRectangleI"), (void*)GdipFillRectangleI_hook,
                       (void**)&GdipFillRectangleI_orig);
    Wh_SetFunctionHook((void*)GetProcAddress(gdip, "GdipFillPath"), (void*)GdipFillPath_hook, (void**)&GdipFillPath_orig);
}

// ---- The start tiles ----------------------------------------------------------------------------
// With no image open the workspace shows a dialog with the eight learning tiles (eight big buttons) and a
// hint. A small watcher thread hides that dialog (asynchronously, it never waits for the UI thread).

static HANDLE g_tileThread;
static HANDLE g_tileStopEvent;
static std::atomic<bool> g_tileStop;
static HWND g_tileDlg;

// keeps the dialog hidden even when the program tries to show it again (mouse over the workspace)
static LRESULT CALLBACK TileSubclassProc(HWND h, UINT m, WPARAM w, LPARAM l, DWORD_PTR) {
    if (g_cfg.hideTiles && !g_tileStop) {
        if (m == WM_WINDOWPOSCHANGING) {
            WINDOWPOS* wp = (WINDOWPOS*)l;
            if (wp->flags & SWP_SHOWWINDOW) {
                wp->flags = (wp->flags & ~SWP_SHOWWINDOW) | SWP_HIDEWINDOW;
            }
        } else if (m == WM_SHOWWINDOW && w) {
            return 0;
        } else if (m == WM_ERASEBKGND || m == WM_PAINT) {
            if (!IsWindowVisible(h)) {
                return m == WM_ERASEBKGND ? 1 : DefWindowProcW(h, m, w, l);
            }
        }
    }
    return DefSubclassProc(h, m, w, l);
}

static BOOL CALLBACK CountBigButtons(HWND h, LPARAM l) {
    wchar_t cls[16];
    if (GetClassNameW(h, cls, ARRAYSIZE(cls)) && _wcsicmp(cls, L"Button") == 0) {
        RECT r;
        GetWindowRect(h, &r);
        if (r.right - r.left >= 120 && r.bottom - r.top >= 90) {
            (*(int*)l)++;
        }
    }
    return TRUE;
}

static BOOL CALLBACK HideTilesChild(HWND h, LPARAM) {
    wchar_t cls[16];
    if (!GetClassNameW(h, cls, ARRAYSIZE(cls)) || wcscmp(cls, L"#32770") != 0 || !IsMainFrameClass(GetParent(h))) {
        return TRUE;
    }
    RECT r;
    GetWindowRect(h, &r);
    if (r.right - r.left < 700 || r.bottom - r.top < 400) {
        return TRUE;
    }
    int big = 0;
    EnumChildWindows(h, CountBigButtons, (LPARAM)&big);
    if (big >= 8) {
        if (g_tileDlg != h) {
            g_tileDlg = h;
            WindhawkUtils::SetWindowSubclassFromAnyThread(h, TileSubclassProc, 0);
        }
        if (IsWindowVisible(h)) {
            SetWindowPos(h, nullptr, 0, 0, 0, 0, SWP_HIDEWINDOW | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_ASYNCWINDOWPOS);
        }
        return FALSE;
    }
    return TRUE;
}

static BOOL CALLBACK HideTilesTop(HWND h, LPARAM) {
    DWORD pid = 0;
    GetWindowThreadProcessId(h, &pid);
    if (pid == GetCurrentProcessId() && IsMainFrameClass(h)) {
        EnumChildWindows(h, HideTilesChild, 0);
        return FALSE;
    }
    return TRUE;
}

// looks for the dialog quickly until it is found; once it is subclassed it only has to notice a new one
static DWORD WINAPI TileThread(LPVOID) {
    while (!g_tileStop) {
        EnumWindows(HideTilesTop, 0);
        // the stop event ends the wait at once
        if (WaitForSingleObject(g_tileStopEvent, g_tileDlg && IsWindow(g_tileDlg) ? 500 : 120) != WAIT_TIMEOUT) {
            break;
        }
    }
    return 0;
}

static void StartTileWatcher() {
    if (!g_tileThread) {
        g_tileStop = false;
        g_tileStopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
        if (g_tileStopEvent) {
            g_tileThread = CreateThread(nullptr, 0, TileThread, nullptr, 0, nullptr);
            if (!g_tileThread) {
                CloseHandle(g_tileStopEvent);
                g_tileStopEvent = nullptr;
            }
        }
    }
}

// The thread may be inside SetWindowSubclassFromAnyThread, which waits for the UI thread: wait until it is gone,
// the mod must not unload while it still runs mod code.
static void StopTileWatcher() {
    if (g_tileThread) {
        g_tileStop = true;
        SetEvent(g_tileStopEvent);
        WaitForSingleObject(g_tileThread, INFINITE);
        CloseHandle(g_tileThread);
        CloseHandle(g_tileStopEvent);
        g_tileThread = nullptr;
        g_tileStopEvent = nullptr;
    }
    if (g_tileDlg && IsWindow(g_tileDlg)) {
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(g_tileDlg, TileSubclassProc);
        HWND mdi = FindWindowExW(GetParent(g_tileDlg), nullptr, L"MDIClient", nullptr);
        if (!mdi || !GetWindow(mdi, GW_CHILD)) {  // only when no image is open
            SetWindowPos(g_tileDlg, nullptr, 0, 0, 0, 0, SWP_SHOWWINDOW | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_ASYNCWINDOWPOS);
        }
    }
    g_tileDlg = nullptr;
}

BOOL Wh_ModInit() {
    LoadSettings();

    HMODULE user32 = GetModuleHandleW(L"user32.dll");
    HMODULE gdi32 = GetModuleHandleW(L"gdi32.dll");
    if (!user32 || !gdi32) {
        return FALSE;
    }

    Wh_SetFunctionHook((void*)GetProcAddress(user32, "DefMDIChildProcW"),
                       (void*)DefMDIChildProcW_hook, (void**)&DefMDIChildProcW_orig);
    Wh_SetFunctionHook((void*)GetProcAddress(user32, "DefMDIChildProcA"),
                       (void*)DefMDIChildProcA_hook, (void**)&DefMDIChildProcA_orig);
    Wh_SetFunctionHook((void*)GetProcAddress(gdi32, "PatBlt"), (void*)PatBlt_hook,
                       (void**)&PatBlt_orig);
    g_teardownMsg = RegisterWindowMessageW(L"PspDarkMode.OptionsBarTeardown");
    InstallDarkHooks();
    InstallBarHooks();
    InstallTitleHooks();
    return TRUE;
}

void Wh_ModAfterInit() {
    if (g_cfg.hideTiles) {
        StartTileWatcher();
    }
    EnumWindows(PatchClassBrushTop, 0);  // classes that were registered before the hooks
    // Repaint image windows that are already open.
    EnumWindows(RedrawTopLevel, 0);
}

void Wh_ModSettingsChanged() {
    const bool hadFreeze = g_cfg.freezeBar;
    LoadSettings();
    if (g_cfg.hideTiles) {
        StartTileWatcher();
    } else {
        StopTileWatcher();
    }
    if (hadFreeze && !g_cfg.freezeBar) {
        RemoveOptionsHook();
    }
    EnumWindows(RedrawTopLevel, 0);
}

void Wh_ModUninit() {
    g_unloading = true;
    StopTileWatcher();
    RemoveOptionsHook();
    EnumWindows(RestoreClassBrushTop, 0);
    if (g_titleBrush && GdipDeleteBrush_p) {
        GdipDeleteBrush_p(g_titleBrush);
        g_titleBrush = nullptr;
    }
    EnumWindows(RedrawTopLevel, 0);
}
