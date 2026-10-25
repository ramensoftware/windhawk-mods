// ==WindhawkMod==
// @id              psp-dark-mdi-frames
// @name            PaintShop Pro Dark Image Windows
// @description     Dark title bars and frames for the image windows in Corel PaintShop Pro
// @version         1.0.0
// @author          MrSmith
// @github          https://github.com/MrSmith2017
// @homepage        https://github.com/MrSmith2017/psp-dark-image-windows
// @license         MIT
// @include         Corel PaintShop Pro.exe
// @compilerOptions -lgdi32 -luser32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# PaintShop Pro Dark Image Windows

![Before and after: light blue-gray image window frames versus dark ones](https://raw.githubusercontent.com/MrSmith2017/psp-dark-image-windows/main/psp-dark-image-windows.png)

Corel PaintShop Pro has a dark theme, but the image windows inside the
workspace keep a light blue-gray frame that looks like Windows XP or 7. They
are classic MDI child windows, and Windows draws their frames itself, with no
dark variant.

This mod paints those frames in dark colors instead:

* title bar, border, icon and title text
* minimize, maximize and close buttons with hover and pressed states
* minimized image windows

Maximized image windows are left to Windows. Everything is drawn by the mod,
so no theme files or system settings are touched, and the mod only affects
PaintShop Pro.

## Optional flash fix

When you switch between images, PaintShop Pro rebuilds the drop-down lists in
the tool options bar. For a few frames their selection fields flash white,
because the Windows combo box falls back to its default brush. The flash fix
paints that field in dark gray instead.

## Settings

All colors are entered as hex codes, for example `282828` or `#282828`. A
value that cannot be read falls back to its default. Changes apply right away.

## Notes

* Tested with PaintShop Pro 2022 on Windows 11. Other versions may work, but
  have not been tested.
* If something looks wrong, disable the mod: the original frames come back
  immediately.
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
- fixComboFlash: true
  $name: Fix white flash in the tool options bar
  $description: Paints the selection field of the drop-down lists dark while PaintShop Pro rebuilds them.
- flashColor: "3C3C3C"
  $name: Color used instead of white
  $description: Hex color for the flash fix.
*/
// ==/WindhawkModSettings==

#include <windhawk_api.h>
#include <windows.h>
#include <windowsx.h>
#include <cwchar>
#include <cwctype>

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
};

static Settings g_cfg;

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
    return TRUE;
}

void Wh_ModAfterInit() {
    // Repaint image windows that are already open.
    EnumWindows(RedrawTopLevel, 0);
}

void Wh_ModSettingsChanged() {
    LoadSettings();
    EnumWindows(RedrawTopLevel, 0);
}

void Wh_ModUninit() {
    g_unloading = true;
    EnumWindows(RedrawTopLevel, 0);
}
