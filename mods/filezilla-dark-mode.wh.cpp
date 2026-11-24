// ==WindhawkMod==
// @id              filezilla-dark-mode
// @name            FileZilla Dark Mode
// @description     Forces a dark theme on FileZilla Client for Windows
// @version         1.5
// @author          tulukaruk
// @github          https://github.com/tulukaruk
// @include         filezilla.exe
// @compilerOptions -luxtheme -ldwmapi -lgdi32 -luser32 -lcomctl32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# FileZilla Dark Mode

FileZilla on Windows draws itself using the Windows *system colors*
(window background, text, button face...). This mod swaps those colors
for a dark palette **inside filezilla.exe only**, so the rest of Windows stays
untouched. It also darkens the title bar, menus, scrollbars, lists, trees,
column headers, text boxes, labels and the message log.

**Fully close and reopen FileZilla** after enabling the mod or changing colors.

## Known limitations
- The top menu bar (File, Edit, View...) stays light. Windows has no supported
  way to make a classic Win32 menu bar dark.
- If "Show accent color on title bars" is on in Windows, the title bar keeps
  your accent color.
- Pop-up dialogs (e.g. "Target file already exists", Settings) are kept in
  FileZilla's normal light style, because they can't be darkened cleanly.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- windowBackground: "#1E1E1E"
  $name: List / text box background
- panelBackground: "#2B2B2B"
  $name: Panel, toolbar and dialog background
- text: "#E6E6E6"
  $name: Text color
- disabledText: "#8A8A8A"
  $name: Disabled text color
- selection: "#264F78"
  $name: Selection background
- selectionText: "#FFFFFF"
  $name: Selection text color
- accent: "#4FC1FF"
  $name: Link / hot-tracking color
- borderColor: "#3C3C3C"
  $name: Border line color
  $description: The thin lines around panels, lists and text boxes
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <commctrl.h>
#include <richedit.h>
#include <uxtheme.h>
#include <dwmapi.h>
#include <cwchar>
#include <cstring>

// ---------------------------------------------------------------------------
// Undocumented uxtheme exports (by ordinal) that Win32 apps use for dark mode.
// ---------------------------------------------------------------------------
enum PreferredAppMode { AppModeDefault, AllowDark, ForceDark, ForceLight, AppModeMax };
using SetPreferredAppMode_t              = PreferredAppMode(WINAPI*)(PreferredAppMode);
using AllowDarkModeForWindow_t           = bool(WINAPI*)(HWND, bool);
using RefreshImmersiveColorPolicyState_t = void(WINAPI*)();
using FlushMenuThemes_t                  = void(WINAPI*)();

static AllowDarkModeForWindow_t pAllowDarkModeForWindow = nullptr;

// ---------------------------------------------------------------------------
// Palette
// ---------------------------------------------------------------------------
constexpr int kMaxColor = COLOR_MENUBAR;  // 30
static COLORREF g_colors[kMaxColor + 1];
static bool     g_override[kMaxColor + 1];
static HBRUSH   g_brushes[kMaxColor + 1];
static HBRUSH   g_origBrushes[kMaxColor + 1];  // real system brushes, to recognise cached copies

static COLORREF g_bg, g_face, g_text, g_border;
static HBRUSH g_borderBrush;
static UINT g_msgUnsubclass;
// Pop-up dialogs are always left in the normal light style.
static const bool g_darkDialogs = false;

// While > 0 on a thread, all hooks pass through untouched (used while a
// light-mode dialog handles a message).
static thread_local int g_light = 0;
static const wchar_t kLightProp[] = L"Windhawk_FZDM_LightDialog";

// A pop-up window with a title bar that belongs to another window
// (the main FileZilla window has no owner).
static bool IsDialogTopLevel(HWND hwnd) {
    LONG_PTR style = GetWindowLongPtrW(hwnd, GWL_STYLE);
    if (style & WS_CHILD) return false;
    if (!(style & WS_DLGFRAME)) return false;  // no title bar / dialog frame
    return GetWindow(hwnd, GW_OWNER) != nullptr;
}

static bool IsInLightDialog(HWND hwnd) {
    if (g_darkDialogs || !hwnd) return false;
    HWND root = GetAncestor(hwnd, GA_ROOT);
    if (!root) return false;
    return GetPropW(root, kLightProp) || IsDialogTopLevel(root);
}

static COLORREF ParseColor(PCWSTR s, COLORREF fallback) {
    if (!s) return fallback;
    while (*s == L' ') s++;
    if (*s == L'#') s++;
    if (wcslen(s) != 6) return fallback;
    wchar_t* end = nullptr;
    unsigned long v = wcstoul(s, &end, 16);
    if (!end || *end) return fallback;
    return RGB((v >> 16) & 0xFF, (v >> 8) & 0xFF, v & 0xFF);
}

static COLORREF ColorSetting(PCWSTR name, COLORREF fallback) {
    PCWSTR s = Wh_GetStringSetting(name);
    COLORREF c = ParseColor(s, fallback);
    Wh_FreeStringSetting(s);
    return c;
}

static BYTE Clamp(int v) { return (BYTE)(v < 0 ? 0 : v > 255 ? 255 : v); }

static COLORREF Shade(COLORREF c, int delta) {
    return RGB(Clamp(GetRValue(c) + delta), Clamp(GetGValue(c) + delta),
               Clamp(GetBValue(c) + delta));
}

static double Luma(COLORREF c) {
    return (0.299 * GetRValue(c) + 0.587 * GetGValue(c) + 0.114 * GetBValue(c)) / 255.0;
}

static double HueToRgb(double p, double q, double t) {
    if (t < 0) t += 1;
    if (t > 1) t -= 1;
    if (t < 1.0 / 6) return p + (q - p) * 6 * t;
    if (t < 1.0 / 2) return q;
    if (t < 2.0 / 3) return p + (q - p) * (2.0 / 3 - t) * 6;
    return p;
}

// Makes dark text colors readable on a dark background, keeping their hue
// (dark blue -> light blue, dark green -> light green, black -> text color).
static COLORREF LightenForDark(COLORREF c) {
    double r = GetRValue(c) / 255.0, g = GetGValue(c) / 255.0, b = GetBValue(c) / 255.0;
    double mx = r > g ? (r > b ? r : b) : (g > b ? g : b);
    double mn = r < g ? (r < b ? r : b) : (g < b ? g : b);
    double l = (mx + mn) / 2, d = mx - mn, h = 0, s = 0;
    if (l >= 0.62) return c;
    if (d > 0) {
        s = l > 0.5 ? d / (2 - mx - mn) : d / (mx + mn);
        if (mx == r)      h = (g - b) / d + (g < b ? 6 : 0);
        else if (mx == g) h = (b - r) / d + 2;
        else              h = (r - g) / d + 4;
        h /= 6;
    }
    if (s < 0.08) return g_text;  // black / grey -> normal text
    double nl = 1.0 - l;
    if (nl < 0.62) nl = 0.62;
    if (nl > 0.80) nl = 0.80;
    double q = nl < 0.5 ? nl * (1 + s) : nl + s - nl * s;
    double p = 2 * nl - q;
    return RGB(Clamp((int)(HueToRgb(p, q, h + 1.0 / 3) * 255 + 0.5)),
               Clamp((int)(HueToRgb(p, q, h) * 255 + 0.5)),
               Clamp((int)(HueToRgb(p, q, h - 1.0 / 3) * 255 + 0.5)));
}

static void SetColor(int index, COLORREF c) {
    g_colors[index] = c;
    g_override[index] = true;
}

static void LoadPalette() {
    COLORREF bg      = ColorSetting(L"windowBackground", RGB(0x1E, 0x1E, 0x1E));
    COLORREF face    = ColorSetting(L"panelBackground",  RGB(0x2B, 0x2B, 0x2B));
    COLORREF text    = ColorSetting(L"text",             RGB(0xE6, 0xE6, 0xE6));
    COLORREF dim     = ColorSetting(L"disabledText",     RGB(0x8A, 0x8A, 0x8A));
    COLORREF sel     = ColorSetting(L"selection",        RGB(0x26, 0x4F, 0x78));
    COLORREF selText = ColorSetting(L"selectionText",    RGB(0xFF, 0xFF, 0xFF));
    COLORREF accent  = ColorSetting(L"accent",           RGB(0x4F, 0xC1, 0xFF));
    g_bg = bg; g_face = face; g_text = text;
    g_border = ColorSetting(L"borderColor", RGB(0x3C, 0x3C, 0x3C));
    if (!g_borderBrush) g_borderBrush = CreateSolidBrush(g_border);

    SetColor(COLOR_WINDOW,        bg);
    SetColor(COLOR_WINDOWTEXT,    text);
    SetColor(COLOR_BTNFACE,       face);
    SetColor(COLOR_BTNTEXT,       text);
    SetColor(COLOR_GRAYTEXT,      dim);
    SetColor(COLOR_HIGHLIGHT,     sel);
    SetColor(COLOR_HIGHLIGHTTEXT, selText);
    SetColor(COLOR_MENU,          face);
    SetColor(COLOR_MENUTEXT,      text);
    SetColor(COLOR_MENUBAR,       face);
    SetColor(COLOR_MENUHILIGHT,   sel);
    SetColor(COLOR_INFOBK,        face);
    SetColor(COLOR_INFOTEXT,      text);
    SetColor(COLOR_APPWORKSPACE,  Shade(bg, -8));
    SetColor(COLOR_SCROLLBAR,     face);
    SetColor(COLOR_BTNSHADOW,     Shade(face, 16));
    SetColor(COLOR_3DDKSHADOW,    Shade(bg, -14));
    SetColor(COLOR_3DLIGHT,       Shade(face, 10));
    SetColor(COLOR_BTNHIGHLIGHT,  Shade(face, 28));
    SetColor(COLOR_WINDOWFRAME,   Shade(face, 20));
    SetColor(COLOR_HOTLIGHT,      accent);

    for (int i = 0; i <= kMaxColor; i++) {
        g_origBrushes[i] = GetSysColorBrush(i);  // hooks are not active yet here
        if (g_override[i] && !g_brushes[i]) g_brushes[i] = CreateSolidBrush(g_colors[i]);
    }
}

// Maps (HBRUSH)(COLOR_xxx + 1) values and real system brushes to our brushes.
static HBRUSH MapBrush(HBRUSH h) {
    UINT_PTR v = (UINT_PTR)h;
    if (v >= 1 && v <= (UINT_PTR)kMaxColor + 1) {
        int i = (int)v - 1;
        return (g_override[i] && g_brushes[i]) ? g_brushes[i] : h;
    }
    if (h) {
        for (int i = 0; i <= kMaxColor; i++) {
            if (h == g_origBrushes[i] && g_override[i] && g_brushes[i]) return g_brushes[i];
        }
    }
    return h;
}

static bool ClassIs(const wchar_t* cls, const wchar_t* name) { return _wcsicmp(cls, name) == 0; }
static bool IsRichEdit(const wchar_t* cls) { return _wcsnicmp(cls, L"RichEdit", 8) == 0; }

static bool IsClass(HWND hwnd, bool (*pred)(const wchar_t*)) {
    wchar_t cls[64];
    return GetClassNameW(hwnd, cls, ARRAYSIZE(cls)) && pred(cls);
}
static bool PredListView(const wchar_t* c) { return ClassIs(c, WC_LISTVIEWW); }
static bool PredTreeView(const wchar_t* c) { return ClassIs(c, WC_TREEVIEWW); }
static bool PredHeader(const wchar_t* c)   { return ClassIs(c, WC_HEADERW); }

// ---------------------------------------------------------------------------
// Hooks
// ---------------------------------------------------------------------------
using GetSysColor_t = decltype(&GetSysColor);
static GetSysColor_t GetSysColor_Original;
static DWORD WINAPI GetSysColor_Hook(int index) {
    if (!g_light && index >= 0 && index <= kMaxColor && g_override[index]) return g_colors[index];
    return GetSysColor_Original(index);
}

using GetSysColorBrush_t = decltype(&GetSysColorBrush);
static GetSysColorBrush_t GetSysColorBrush_Original;
static HBRUSH WINAPI GetSysColorBrush_Hook(int index) {
    if (!g_light && index >= 0 && index <= kMaxColor && g_override[index] && g_brushes[index]) {
        return g_brushes[index];
    }
    return GetSysColorBrush_Original(index);
}

using FillRect_t = decltype(&FillRect);
static FillRect_t FillRect_Original;
// Our brush -> the real system brush it replaces (for light-mode dialogs).
static HBRUSH UnmapBrush(HBRUSH h) {
    if (!h) return h;
    for (int i = 0; i <= kMaxColor; i++) {
        if (g_brushes[i] == h) return g_origBrushes[i];
    }
    return h;
}

static int WINAPI FillRect_Hook(HDC hdc, const RECT* rc, HBRUSH hbr) {
    return FillRect_Original(hdc, rc, g_light ? UnmapBrush(hbr) : MapBrush(hbr));
}

static bool IsOurBrush(HBRUSH h) {
    if (!h) return false;
    for (int i = 0; i <= kMaxColor; i++) if (g_brushes[i] == h) return true;
    return false;
}

// Dark text drawn on a dark (or transparent) background gets lightened.
// Dark text on an explicitly light, opaque background is left alone.
using ExtTextOutW_t = decltype(&ExtTextOutW);
static ExtTextOutW_t ExtTextOutW_Original;
static BOOL WINAPI ExtTextOutW_Hook(HDC hdc, int x, int y, UINT options, const RECT* rc,
                                    LPCWSTR str, UINT count, const INT* dx) {
    if (count > 0 && !g_light) {
        COLORREF cur = GetTextColor(hdc);
        if (cur != CLR_INVALID && Luma(cur) < 0.35) {
            bool opaqueBg = (options & ETO_OPAQUE) || GetBkMode(hdc) == OPAQUE;
            if (!opaqueBg || Luma(GetBkColor(hdc)) < 0.5) {
                SetTextColor(hdc, LightenForDark(cur));
                BOOL r = ExtTextOutW_Original(hdc, x, y, options, rc, str, count, dx);
                SetTextColor(hdc, cur);
                return r;
            }
        }
    }
    return ExtTextOutW_Original(hdc, x, y, options, rc, str, count, dx);
}

// --- Visual-style (uxtheme) backgrounds: dialog faces and tab pages --------
enum ThemeKind { TK_OTHER, TK_WINDOW, TK_TAB, TK_BUTTON };
struct ThemeEntry { HTHEME h; ThemeKind kind; };
constexpr LONG kThemeSlots = 128;
static ThemeEntry g_themes[kThemeSlots];
static volatile LONG g_themeCount;

static bool ClassListIs(LPCWSTR list, LPCWSTR name) {
    // Accepts "Name", "Name;Other" and "Prefix::Name".
    for (LPCWSTR p = list; p && *p;) {
        LPCWSTR end = wcschr(p, L';');
        size_t len = end ? (size_t)(end - p) : wcslen(p);
        LPCWSTR colon = nullptr;
        for (LPCWSTR q = p; q + 1 < p + len; q++) if (q[0] == L':' && q[1] == L':') colon = q + 2;
        LPCWSTR start = colon ? colon : p;
        size_t n = len - (size_t)(start - p);
        if (n == wcslen(name) && _wcsnicmp(start, name, n) == 0) return true;
        p = end ? end + 1 : nullptr;
    }
    return false;
}

static void RememberTheme(HTHEME h, LPCWSTR classList) {
    if (!h || !classList) return;
    ThemeKind kind;
    if (ClassListIs(classList, L"WINDOW")) kind = TK_WINDOW;
    else if (ClassListIs(classList, L"TAB")) kind = TK_TAB;
    else if (ClassListIs(classList, L"BUTTON")) kind = TK_BUTTON;
    else return;
    for (LONG i = 0; i < kThemeSlots; i++) if (g_themes[i].h == h) return;
    LONG slot = (InterlockedIncrement(&g_themeCount) - 1) % kThemeSlots;
    g_themes[slot].kind = kind;
    g_themes[slot].h = h;
}

static ThemeKind KindOf(HTHEME h) {
    for (LONG i = 0; i < kThemeSlots; i++) if (g_themes[i].h == h) return g_themes[i].kind;
    return TK_OTHER;
}

using OpenThemeData_t = decltype(&OpenThemeData);
static OpenThemeData_t OpenThemeData_Original;
static HTHEME WINAPI OpenThemeData_Hook(HWND hwnd, LPCWSTR classList) {
    HTHEME h = OpenThemeData_Original(hwnd, classList);
    RememberTheme(h, classList);
    return h;
}

using OpenThemeDataForDpi_t = HTHEME(WINAPI*)(HWND, LPCWSTR, UINT);
static OpenThemeDataForDpi_t OpenThemeDataForDpi_Original;
static HTHEME WINAPI OpenThemeDataForDpi_Hook(HWND hwnd, LPCWSTR classList, UINT dpi) {
    HTHEME h = OpenThemeDataForDpi_Original(hwnd, classList, dpi);
    RememberTheme(h, classList);
    return h;
}

constexpr int kWP_DIALOG = 29;
constexpr int kTABP_PANE = 9, kTABP_BODY = 10, kTABP_AEROWIZARDBODY = 11;
constexpr int kTIS_HOT = 2, kTIS_SELECTED = 3, kTIS_FOCUSED = 5;

using DrawThemeBackground_t = decltype(&DrawThemeBackground);
static DrawThemeBackground_t DrawThemeBackground_Original;
static HRESULT WINAPI DrawThemeBackground_Hook(HTHEME theme, HDC hdc, int part, int state,
                                               LPCRECT rc, LPCRECT clip) {
    ThemeKind kind = (rc && !g_light) ? KindOf(theme) : TK_OTHER;
    if (kind == TK_WINDOW || kind == TK_TAB) {
        RECT r = *rc;
        if (clip) IntersectRect(&r, &r, clip);
        if (kind == TK_WINDOW && part == kWP_DIALOG) {
            FillRect_Original(hdc, &r, g_brushes[COLOR_BTNFACE]);
            return S_OK;
        }
        if (kind == TK_TAB) {
            if (part == kTABP_BODY || part == kTABP_AEROWIZARDBODY) {
                FillRect_Original(hdc, &r, g_brushes[COLOR_BTNFACE]);
                return S_OK;
            }
            if (part == kTABP_PANE) {
                FillRect_Original(hdc, &r, g_brushes[COLOR_BTNFACE]);
                FrameRect(hdc, rc, g_brushes[COLOR_BTNSHADOW]);
                return S_OK;
            }
            if (part >= 1 && part <= 8) {  // tab headers
                bool active = state == kTIS_SELECTED || state == kTIS_HOT || state == kTIS_FOCUSED;
                HBRUSH fill = active ? g_brushes[COLOR_BTNHIGHLIGHT] : g_brushes[COLOR_BTNFACE];
                FillRect_Original(hdc, &r, fill);
                FrameRect(hdc, rc, g_brushes[COLOR_BTNSHADOW]);
                return S_OK;
            }
        }
    }
    return DrawThemeBackground_Original(theme, hdc, part, state, rc, clip);
}

// wxWidgets takes dialog and tab-page background colors from the visual style
// ("WINDOW" / "TAB" fill color), not from the system colors.
constexpr int kTMT_FILLCOLOR = 3802, kTMT_TEXTCOLOR = 3803;
using GetThemeColor_t = decltype(&GetThemeColor);
static GetThemeColor_t GetThemeColor_Original;
static HRESULT WINAPI GetThemeColor_Hook(HTHEME theme, int part, int state, int prop,
                                         COLORREF* color) {
    if (!g_light && color) {
        ThemeKind kind = KindOf(theme);
        if ((kind == TK_WINDOW || kind == TK_TAB) && prop == kTMT_FILLCOLOR) {
            *color = g_face;
            return S_OK;
        }
        if (kind != TK_OTHER && prop == kTMT_TEXTCOLOR) {
            *color = g_text;
            return S_OK;
        }
    }
    return GetThemeColor_Original(theme, part, state, prop, color);
}

// Background/text colors for text boxes, labels and buttons that ask their
// parent and fall through to Windows' default (white) handling.
using DefWindowProcW_t = decltype(&DefWindowProcW);
static DefWindowProcW_t DefWindowProcW_Original;
static LRESULT WINAPI DefWindowProcW_Hook(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    if (g_light) {
        // The shared window class may carry our dark brush; erase light instead.
        if (msg == WM_ERASEBKGND) {
            HBRUSH brush = (HBRUSH)GetClassLongPtrW(hwnd, GCLP_HBRBACKGROUND);
            if (IsOurBrush(brush)) {
                RECT rc;
                GetClientRect(hwnd, &rc);
                FillRect_Original((HDC)wp, &rc, UnmapBrush(brush));
                return 1;
            }
        }
        return DefWindowProcW_Original(hwnd, msg, wp, lp);
    }
    switch (msg) {
        case WM_ERASEBKGND: {
            HBRUSH brush = MapBrush((HBRUSH)GetClassLongPtrW(hwnd, GCLP_HBRBACKGROUND));
            if (IsOurBrush(brush)) {
                RECT rc;
                GetClientRect(hwnd, &rc);
                FillRect_Original((HDC)wp, &rc, brush);
                return 1;
            }
            break;
        }
        case WM_CTLCOLOREDIT:
        case WM_CTLCOLORLISTBOX:
            SetTextColor((HDC)wp, g_text);
            SetBkColor((HDC)wp, g_bg);
            return (LRESULT)g_brushes[COLOR_WINDOW];
        case WM_CTLCOLORSTATIC:
        case WM_CTLCOLORBTN:
        case WM_CTLCOLORDLG:
            SetTextColor((HDC)wp, g_text);
            SetBkColor((HDC)wp, g_face);
            return (LRESULT)g_brushes[COLOR_BTNFACE];
    }
    return DefWindowProcW_Original(hwnd, msg, wp, lp);
}

// wxWidgets calls SetWindowTheme(hwnd, L"Explorer", ...) on list and tree
// controls, and list views pass their theme on to their column header.
using SetWindowTheme_t = decltype(&SetWindowTheme);
static SetWindowTheme_t SetWindowTheme_Original;
static HRESULT WINAPI SetWindowTheme_Hook(HWND hwnd, LPCWSTR subAppName, LPCWSTR subIdList) {
    if (g_light || IsInLightDialog(hwnd)) {
        return SetWindowTheme_Original(hwnd, subAppName, subIdList);
    }
    if (IsClass(hwnd, PredHeader)) {
        subAppName = L"DarkMode_ItemsView";
    } else if (subAppName && (_wcsicmp(subAppName, L"Explorer") == 0 ||
                              _wcsicmp(subAppName, L"ItemsView") == 0)) {
        subAppName = L"DarkMode_Explorer";
    }
    return SetWindowTheme_Original(hwnd, subAppName, subIdList);
}

// Recolors messages FileZilla sends to its message log (rich edit) and to
// list/tree controls.
using SendMessageW_t = decltype(&SendMessageW);
static SendMessageW_t SendMessageW_Original;
static LRESULT WINAPI SendMessageW_Hook(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    if (g_light) return SendMessageW_Original(hwnd, msg, wp, lp);
    switch (msg) {
        case EM_SETBKGNDCOLOR:
            if (IsClass(hwnd, IsRichEdit)) return SendMessageW_Original(hwnd, msg, 0, g_bg);
            break;

        case EM_SETCHARFORMAT:
            if (lp && IsClass(hwnd, IsRichEdit)) {
                CHARFORMAT2W cf;
                UINT size = ((const CHARFORMATW*)lp)->cbSize;
                if (size >= sizeof(CHARFORMATW) && size <= sizeof(cf)) {
                    memcpy(&cf, (const void*)lp, size);
                    if ((cf.dwMask & CFM_COLOR) && !(cf.dwEffects & CFE_AUTOCOLOR)) {
                        cf.crTextColor = LightenForDark(cf.crTextColor);
                    }
                    if (size >= sizeof(CHARFORMAT2W) && (cf.dwMask & CFM_BACKCOLOR) &&
                        !(cf.dwEffects & CFE_AUTOBACKCOLOR) && Luma(cf.crBackColor) > 0.5) {
                        cf.crBackColor = g_bg;
                    }
                    return SendMessageW_Original(hwnd, msg, wp, (LPARAM)&cf);
                }
            }
            break;

        case LVM_SETBKCOLOR:
        case LVM_SETTEXTBKCOLOR:
            if (IsClass(hwnd, PredListView) &&
                ((COLORREF)lp == CLR_DEFAULT || Luma((COLORREF)lp) > 0.6)) {
                return SendMessageW_Original(hwnd, msg, wp, g_bg);
            }
            break;
        case LVM_SETTEXTCOLOR:
            if (IsClass(hwnd, PredListView) &&
                ((COLORREF)lp == CLR_DEFAULT || Luma((COLORREF)lp) < 0.4)) {
                return SendMessageW_Original(hwnd, msg, wp, g_text);
            }
            break;
        case TVM_SETBKCOLOR:
            if (IsClass(hwnd, PredTreeView) &&
                ((COLORREF)lp == (COLORREF)-1 || Luma((COLORREF)lp) > 0.6)) {
                return SendMessageW_Original(hwnd, msg, wp, g_bg);
            }
            break;
        case TVM_SETTEXTCOLOR:
            if (IsClass(hwnd, PredTreeView) &&
                ((COLORREF)lp == (COLORREF)-1 || Luma((COLORREF)lp) < 0.4)) {
                return SendMessageW_Original(hwnd, msg, wp, g_text);
            }
            break;
    }
    return SendMessageW_Original(hwnd, msg, wp, lp);
}

static LRESULT Send(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    return SendMessageW_Original ? SendMessageW_Original(hwnd, msg, wp, lp)
                                 : SendMessageW(hwnd, msg, wp, lp);
}

static HRESULT Theme(HWND hwnd, LPCWSTR name) {
    return SetWindowTheme_Original ? SetWindowTheme_Original(hwnd, name, nullptr)
                                   : SetWindowTheme(hwnd, name, nullptr);
}

static void ApplyDarkToWindow(HWND hwnd) {
    wchar_t cls[64];
    if (!GetClassNameW(hwnd, cls, ARRAYSIZE(cls))) return;

    LONG_PTR style = GetWindowLongPtrW(hwnd, GWL_STYLE);

    if (pAllowDarkModeForWindow) pAllowDarkModeForWindow(hwnd, true);

    // Window classes registered with a system-color background brush.
    HBRUSH classBrush = (HBRUSH)GetClassLongPtrW(hwnd, GCLP_HBRBACKGROUND);
    HBRUSH mapped = MapBrush(classBrush);
    if (mapped != classBrush) SetClassLongPtrW(hwnd, GCLP_HBRBACKGROUND, (LONG_PTR)mapped);

    if (!(style & WS_CHILD)) {
        BOOL on = TRUE;
        if (FAILED(DwmSetWindowAttribute(hwnd, 20, &on, sizeof(on)))) {
            DwmSetWindowAttribute(hwnd, 19, &on, sizeof(on));
        }
    }

    if (ClassIs(cls, WC_LISTVIEWW)) {
        Send(hwnd, WM_SYSCOLORCHANGE, 0, 0);  // refresh comctl32's cached colors
        Theme(hwnd, L"DarkMode_Explorer");
        Send(hwnd, LVM_SETBKCOLOR, 0, g_bg);
        Send(hwnd, LVM_SETTEXTBKCOLOR, 0, g_bg);
        Send(hwnd, LVM_SETTEXTCOLOR, 0, g_text);
        if (HWND header = (HWND)Send(hwnd, LVM_GETHEADER, 0, 0)) {
            Theme(header, L"DarkMode_ItemsView");
        }
    } else if (ClassIs(cls, WC_TREEVIEWW)) {
        Send(hwnd, WM_SYSCOLORCHANGE, 0, 0);
        Theme(hwnd, L"DarkMode_Explorer");
        Send(hwnd, TVM_SETBKCOLOR, 0, g_bg);
        Send(hwnd, TVM_SETTEXTCOLOR, 0, g_text);
    } else if (ClassIs(cls, WC_HEADERW)) {
        Theme(hwnd, L"DarkMode_ItemsView");
    } else if (IsRichEdit(cls)) {
        Theme(hwnd, L"DarkMode_Explorer");
        Send(hwnd, EM_SETBKGNDCOLOR, 0, g_bg);
        CHARFORMAT2W cf = {};
        cf.cbSize = sizeof(cf);
        cf.dwMask = CFM_COLOR;
        cf.crTextColor = g_text;
        Send(hwnd, EM_SETCHARFORMAT, SCF_DEFAULT, (LPARAM)&cf);
    } else if (ClassIs(cls, WC_EDITW) || ClassIs(cls, WC_COMBOBOXW) ||
               ClassIs(cls, L"ComboLBox") || ClassIs(cls, WC_COMBOBOXEXW)) {
        Theme(hwnd, L"DarkMode_CFD");
    } else if (ClassIs(cls, WC_BUTTONW) || ClassIs(cls, WC_SCROLLBARW) ||
               ClassIs(cls, TOOLTIPS_CLASSW) || ClassIs(cls, WC_LISTBOXW) ||
               _wcsnicmp(cls, L"wx", 2) == 0) {
        Theme(hwnd, L"DarkMode_Explorer");
    } else if (ClassIs(cls, STATUSCLASSNAMEW)) {
        // The themed status bar is always light; classic drawing uses our colors.
        Send(hwnd, WM_SYSCOLORCHANGE, 0, 0);
        if (SetWindowTheme_Original) SetWindowTheme_Original(hwnd, L"", L"");
        else SetWindowTheme(hwnd, L"", L"");
    } else if (ClassIs(cls, TOOLBARCLASSNAMEW) ||
               ClassIs(cls, WC_TABCONTROLW) || ClassIs(cls, REBARCLASSNAMEW)) {
        Send(hwnd, WM_SYSCOLORCHANGE, 0, 0);
    }
}

// ---------------------------------------------------------------------------
// Per-window subclass: dark text-box/label colors and dark border lines.
// ---------------------------------------------------------------------------
static bool IsLightBrush(HBRUSH h) {
    h = MapBrush(h);
    if (!h) return true;
    if (IsOurBrush(h)) return false;
    LOGBRUSH lb;
    if (GetObjectW(h, sizeof(lb), &lb) != sizeof(lb)) return false;
    return lb.lbStyle == BS_SOLID && Luma(lb.lbColor) > 0.5;
}

static void DrawDarkBorder(HWND hwnd) {
    LONG_PTR style = GetWindowLongPtrW(hwnd, GWL_STYLE);
    LONG_PTR ex = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);
    if (!(style & WS_CHILD)) return;
    int thickness = (ex & WS_EX_CLIENTEDGE) ? 2
                  : ((ex & WS_EX_STATICEDGE) || (style & WS_BORDER)) ? 1 : 0;
    if (!thickness) return;
    RECT rc;
    GetWindowRect(hwnd, &rc);
    OffsetRect(&rc, -rc.left, -rc.top);
    HDC hdc = GetWindowDC(hwnd);
    if (!hdc) return;
    for (int i = 0; i < thickness; i++) {
        FrameRect(hdc, &rc, g_borderBrush);
        InflateRect(&rc, -1, -1);
    }
    ReleaseDC(hwnd, hdc);
}

static LRESULT CALLBACK DarkSubclassProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp,
                                         UINT_PTR id, DWORD_PTR) {
    if (msg == g_msgUnsubclass && g_msgUnsubclass) {
        RemoveWindowSubclass(hwnd, DarkSubclassProc, id);
        return 0;
    }
    if (msg == WM_NCDESTROY) {
        RemoveWindowSubclass(hwnd, DarkSubclassProc, id);
        return DefSubclassProc(hwnd, msg, wp, lp);
    }
    // Checked on every message, so it works even if the window only became
    // part of a dialog after it was created.
    if (IsInLightDialog(hwnd)) {
        g_light++;
        LRESULT r = DefSubclassProc(hwnd, msg, wp, lp);
        g_light--;
        return r;
    }
    switch (msg) {
        case WM_CTLCOLOREDIT:
        case WM_CTLCOLORLISTBOX:
        case WM_CTLCOLORSTATIC:
        case WM_CTLCOLORBTN:
        case WM_CTLCOLORDLG: {
            LRESULT r = DefSubclassProc(hwnd, msg, wp, lp);
            HDC hdc = (HDC)wp;
            bool editLike = msg == WM_CTLCOLOREDIT || msg == WM_CTLCOLORLISTBOX;
            if (IsLightBrush((HBRUSH)r)) {
                SetBkColor(hdc, editLike ? g_bg : g_face);
                r = (LRESULT)g_brushes[editLike ? COLOR_WINDOW : COLOR_BTNFACE];
            }
            COLORREF tc = GetTextColor(hdc);
            if (Luma(tc) < 0.35) SetTextColor(hdc, LightenForDark(tc));
            return r;
        }
        case WM_NCPAINT: {
            LRESULT r = DefSubclassProc(hwnd, msg, wp, lp);
            DrawDarkBorder(hwnd);
            return r;
        }
        case WM_NCDESTROY:
            RemoveWindowSubclass(hwnd, DarkSubclassProc, id);
            break;
    }
    return DefSubclassProc(hwnd, msg, wp, lp);
}

static void SubclassForDark(HWND hwnd) {
    DWORD pid = 0;
    if (GetWindowThreadProcessId(hwnd, &pid) != GetCurrentThreadId()) return;  // owner thread only
    SetWindowSubclass(hwnd, DarkSubclassProc, 0xFD4C, 0);
}

using CreateWindowExW_t = decltype(&CreateWindowExW);
static CreateWindowExW_t CreateWindowExW_Original;
static HWND WINAPI CreateWindowExW_Hook(DWORD exStyle, LPCWSTR className, LPCWSTR windowName,
                                        DWORD style, int x, int y, int width, int height,
                                        HWND parent, HMENU menu, HINSTANCE instance,
                                        LPVOID param) {
    bool light = IsInLightDialog(parent);
    if (light) g_light++;
    HWND hwnd = CreateWindowExW_Original(exStyle, className, windowName, style, x, y,
                                         width, height, parent, menu, instance, param);
    if (light) g_light--;
    if (!hwnd) return hwnd;

    if (!g_darkDialogs && !light && IsDialogTopLevel(hwnd)) {
        SetPropW(hwnd, kLightProp, (HANDLE)1);
        light = true;
    }
    if (!light) ApplyDarkToWindow(hwnd);
    // Every window gets the subclass; it decides per message (dark or light).
    SubclassForDark(hwnd);
    return hwnd;
}

// ---------------------------------------------------------------------------
// Re-theme windows that already exist (mod enabled while FileZilla is open).
// ---------------------------------------------------------------------------
static BOOL CALLBACK ApplyChildProc(HWND hwnd, LPARAM) {
    if (IsInLightDialog(hwnd)) return TRUE;
    ApplyDarkToWindow(hwnd);
    return TRUE;
}

static BOOL CALLBACK ApplyTopProc(HWND hwnd, LPARAM) {
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    if (pid != GetCurrentProcessId()) return TRUE;
    ApplyDarkToWindow(hwnd);
    EnumChildWindows(hwnd, ApplyChildProc, 0);
    DWORD_PTR result;
    SendMessageTimeoutW(hwnd, WM_SYSCOLORCHANGE, 0, 0, SMTO_ABORTIFHUNG, 500, &result);
    RedrawWindow(hwnd, nullptr, nullptr,
                 RDW_INVALIDATE | RDW_ERASE | RDW_FRAME | RDW_ALLCHILDREN);
    return TRUE;
}

// ---------------------------------------------------------------------------
// Mod entry points
// ---------------------------------------------------------------------------
static BOOL CALLBACK UnsubclassChildProc(HWND hwnd, LPARAM) {
    DWORD_PTR result;
    SendMessageTimeoutW(hwnd, g_msgUnsubclass, 0, 0, SMTO_ABORTIFHUNG, 500, &result);
    return TRUE;
}

static BOOL CALLBACK UnsubclassTopProc(HWND hwnd, LPARAM) {
    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    if (pid != GetCurrentProcessId()) return TRUE;
    UnsubclassChildProc(hwnd, 0);
    EnumChildWindows(hwnd, UnsubclassChildProc, 0);
    return TRUE;
}

BOOL Wh_ModInit() {
    Wh_Log(L"FileZilla Dark Mode: init");

    g_msgUnsubclass = RegisterWindowMessageW(L"Windhawk_FileZillaDarkMode_Unsubclass");

    LoadPalette();

    HMODULE uxtheme = LoadLibraryExW(L"uxtheme.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (uxtheme) {
        auto refreshPolicy = (RefreshImmersiveColorPolicyState_t)GetProcAddress(uxtheme, MAKEINTRESOURCEA(104));
        pAllowDarkModeForWindow = (AllowDarkModeForWindow_t)GetProcAddress(uxtheme, MAKEINTRESOURCEA(133));
        auto setAppMode = (SetPreferredAppMode_t)GetProcAddress(uxtheme, MAKEINTRESOURCEA(135));
        auto flushMenus = (FlushMenuThemes_t)GetProcAddress(uxtheme, MAKEINTRESOURCEA(136));

        if (setAppMode) setAppMode(ForceDark);
        if (refreshPolicy) refreshPolicy();
        if (flushMenus) flushMenus();
    }

    Wh_SetFunctionHook((void*)GetSysColor, (void*)GetSysColor_Hook,
                       (void**)&GetSysColor_Original);
    Wh_SetFunctionHook((void*)GetSysColorBrush, (void*)GetSysColorBrush_Hook,
                       (void**)&GetSysColorBrush_Original);
    Wh_SetFunctionHook((void*)FillRect, (void*)FillRect_Hook,
                       (void**)&FillRect_Original);
    Wh_SetFunctionHook((void*)ExtTextOutW, (void*)ExtTextOutW_Hook,
                       (void**)&ExtTextOutW_Original);
    Wh_SetFunctionHook((void*)OpenThemeData, (void*)OpenThemeData_Hook,
                       (void**)&OpenThemeData_Original);
    Wh_SetFunctionHook((void*)DrawThemeBackground, (void*)DrawThemeBackground_Hook,
                       (void**)&DrawThemeBackground_Original);
    Wh_SetFunctionHook((void*)GetThemeColor, (void*)GetThemeColor_Hook,
                       (void**)&GetThemeColor_Original);
    if (uxtheme) {
        if (void* p = (void*)GetProcAddress(uxtheme, "OpenThemeDataForDpi")) {
            Wh_SetFunctionHook(p, (void*)OpenThemeDataForDpi_Hook,
                               (void**)&OpenThemeDataForDpi_Original);
        }
    }
    Wh_SetFunctionHook((void*)DefWindowProcW, (void*)DefWindowProcW_Hook,
                       (void**)&DefWindowProcW_Original);
    Wh_SetFunctionHook((void*)SetWindowTheme, (void*)SetWindowTheme_Hook,
                       (void**)&SetWindowTheme_Original);
    Wh_SetFunctionHook((void*)SendMessageW, (void*)SendMessageW_Hook,
                       (void**)&SendMessageW_Original);
    Wh_SetFunctionHook((void*)CreateWindowExW, (void*)CreateWindowExW_Hook,
                       (void**)&CreateWindowExW_Original);

    return TRUE;
}

void Wh_ModAfterInit() {
    // Hooks are live now; fix up any windows FileZilla already created.
    EnumWindows(ApplyTopProc, 0);
}

void Wh_ModUninit() {
    Wh_Log(L"FileZilla Dark Mode: uninit");
    // Detach our window subclass before the mod's code is unloaded.
    if (g_msgUnsubclass) EnumWindows(UnsubclassTopProc, 0);
    // Brushes are intentionally not deleted: FileZilla may still hold them.
    // Restart FileZilla to fully return to the light theme.
}
