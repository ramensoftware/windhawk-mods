// ==WindhawkMod==
// @id              enhanced-disk-usage
// @name            Enhanced Disk Usage
// @description     Enables the ability to customize the disk drive tiles in explorer, targeting the disk's usage bar, as well as the details that appear below.
// @version         1.0
// @author          bbmaster123
// @github          https://github.com/bbmaster123
// @include         explorer.exe
// @compilerOptions -lcomctl32 -lole32 -luuid -luser32 -lgdi32 -luxtheme -lshlwapi -lmsimg32 -lgdiplus
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
![Screenshot](https://raw.githubusercontent.com/bbmaster123/FWFU/refs/heads/main/Assets/screenshot.png)

Enables the ability to customize the disk drive tiles in explorer, targeting the
disk's usage bar, as well as the details that appear below.

- custom colors with transparency for disk usage, track (background/unused), and
outline
- separate disk colors for when drive is near full
- linear gradient support with configurable direction
- rounded corners
- glossy overlay toggle option (for a more Windows Aero-ish looking aesthetic)
- height/width controls (inset) controls for disk bar and track
- custom disk usage text with font size adjustment, multi-line support,
line-height adjustment, and more

ex.
100GB free | 100GB/200GB
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*

- barNormalStart: "#2ECC71"
  $name: Bar Color Gradient Start
  $description: set start and end to the same value if you do not want a gradient
- barNormalEnd: "#27AE60"
  $name: Bar Color Gradient End
- barFullStart: "#E74C3C"
  $name: Bar Full Color Gradient Start
- barFullEnd: "#C0392B"
  $name: Bar Full Color Gradient End
- trackColor: "#20000000"
  $name: Track Color (Unused Space)
- trackLeftInset: 0
  $name: Track Inset Left
- trackRightInset: 0
  $name: Track Inset Right
- trackTopInset: 0
  $name: Track Inset Top
- trackBottomInset: 0
  $name: Track Inset Bottom
- gradientDirection: 90
  $name: Gradient Direction
- borderColor: "#80FFFFFF"
  $name: Border Color
- borderThickness: 1
  $name: Border Thickness
- cornerRadius: 6
  $name: Corner Radius
- showGloss: true
  $name: Enable Glossy Overlay
- leftInset: 0
  $name: Bar Inset Left
- rightInset: 0
  $name: Bar Inset Right
- topInset: 0
  $name: Bar Inset Top
- bottomInset: 0
  $name: Bar Inset Bottom
- formatString: "%s free | %s used\\n%s total"
  $name: Text Display Format
  $description: custom disk usage text.%s for each disk usage stat, \n for new line
- boldUsed: true
  $name: Bold Used Space Value
- boldStyle: sans-serif
  $name: Text Style
  $options:
    - serif: Serif
    - sans-serif: Sans-Serif
- removeSpace: false
  $name: Remove Space before Units (100GB/100 GB)
- lineYOffset: 0
  $name: Text Vertical Offset
- barYOffset: 0
  $name: Progress Bar Vertical Offset
- trackBorderOffset: 0
  $name: Track Border Offset
  $description: Adjusts the border position relative to the track. Positive values expand outwards.
- fillPadding: 0
  $name: Fill Bar Padding
  $description: Extra space between the progress fill and the track border.
- lineSpacing: 0
  $name: Line Height
  $description: Adjusts the vertical space between lines of text
- fontSize: 0
  $name: Font Size
  $description: Adjusts the font size (positive is larger, negative is smaller)
- roundFillBothSides: true
  $name: Round Both Sides of Fill
  $description: If enabled, both sides of the progress bar will be rounded. If disabled, the right side will be flat unless full.
- enableWordEllipsis: false
  $name: Enable Word Ellipsis
  $description: (Adds "..." if text is too long)
*/
// ==/WindhawkModSettings==

#include <commctrl.h>
#include <gdiplus.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <uxtheme.h>
#include <windhawk_api.h>
#include <windows.h>
#include <algorithm>
#include <cwchar>
#include <shared_mutex>
#include <string>
#include <unordered_map>
#include <vector>

using namespace Gdiplus;

// --- Global State ---
enum class BoldStyle { Serif, SansSerif };
std::wstring g_formatString;
bool g_boldUsed, g_removeSpace, g_showGloss, g_enableWordEllipsis,
    g_roundFillBothSides;
int g_lineYOffset, g_cornerRadius, g_fontSize;
int g_barYOffset, g_lineSpacing;
int g_leftInset, g_rightInset, g_topInset, g_bottomInset;
int g_trackLeftInset, g_trackRightInset, g_trackTopInset, g_trackBottomInset;
int g_gradientDirection;
ARGB g_barNormalStart, g_barNormalEnd, g_barFullStart, g_barFullEnd,
    g_trackColor, g_borderColor;
float g_borderThickness, g_trackBorderOffset, g_fillPadding;
BoldStyle g_boldStyle;
ULONG_PTR g_gdiplusToken;

typedef int(WINAPI* DrawTextW_t)(HDC hdc,
                                 LPCWSTR lpchText,
                                 int cchText,
                                 LPRECT lprc,
                                 UINT format);
DrawTextW_t DrawTextW_Orig;
typedef int(WINAPI* DrawTextExW_t)(HDC hdc,
                                   LPWSTR lpchText,
                                   int cchText,
                                   LPRECT lprc,
                                   UINT format,
                                   LPDRAWTEXTPARAMS lpdtp);
DrawTextExW_t DrawTextExW_Orig;
typedef HRESULT(WINAPI* DrawThemeBackground_t)(HTHEME hTheme,
                                               HDC hdc,
                                               int iPartId,
                                               int iStateId,
                                               LPCRECT pRect,
                                               LPCRECT pClipRect);
DrawThemeBackground_t DrawThemeBackground_Orig;

typedef HWND(WINAPI* GetThemeWindow_t)(HTHEME hTheme);
GetThemeWindow_t GetThemeWindow_Ptr;
typedef HRESULT(WINAPI* GetThemeClassList_t)(HTHEME hTheme,
                                             LPWSTR pszClassList,
                                             int cchClassList);
GetThemeClassList_t GetThemeClassList_Ptr;

std::shared_mutex g_themeClassMutex;
std::unordered_map<HTHEME, std::wstring> g_themeClasses;

typedef HTHEME(WINAPI* OpenThemeData_t)(HWND hwnd, LPCWSTR pszClassList);
OpenThemeData_t OpenThemeData_Orig;
HTHEME WINAPI OpenThemeData_Hook(HWND hwnd, LPCWSTR pszClassList) {
    HTHEME hTheme = OpenThemeData_Orig(hwnd, pszClassList);
    if (hTheme && pszClassList) {
        std::unique_lock lock(g_themeClassMutex);
        g_themeClasses[hTheme] = pszClassList;
    }
    return hTheme;
}

typedef HTHEME(WINAPI* OpenThemeDataEx_t)(HWND hwnd,
                                          LPCWSTR pszClassList,
                                          DWORD dwFlags);
OpenThemeDataEx_t OpenThemeDataEx_Orig;
HTHEME WINAPI OpenThemeDataEx_Hook(HWND hwnd,
                                   LPCWSTR pszClassList,
                                   DWORD dwFlags) {
    HTHEME hTheme = OpenThemeDataEx_Orig(hwnd, pszClassList, dwFlags);
    if (hTheme && pszClassList) {
        std::unique_lock lock(g_themeClassMutex);
        g_themeClasses[hTheme] = pszClassList;
    }
    return hTheme;
}

typedef HTHEME(WINAPI* OpenThemeDataForDpi_t)(HWND hwnd,
                                              LPCWSTR pszClassList,
                                              UINT dpi);
OpenThemeDataForDpi_t OpenThemeDataForDpi_Orig;
HTHEME WINAPI OpenThemeDataForDpi_Hook(HWND hwnd,
                                       LPCWSTR pszClassList,
                                       UINT dpi) {
    HTHEME hTheme = OpenThemeDataForDpi_Orig(hwnd, pszClassList, dpi);
    if (hTheme && pszClassList) {
        std::unique_lock lock(g_themeClassMutex);
        g_themeClasses[hTheme] = pszClassList;
    }
    return hTheme;
}

typedef HRESULT(WINAPI* CloseThemeData_t)(HTHEME hTheme);
CloseThemeData_t CloseThemeData_Orig;
HRESULT WINAPI CloseThemeData_Hook(HTHEME hTheme) {
    if (hTheme) {
        std::unique_lock lock(g_themeClassMutex);
        g_themeClasses.erase(hTheme);
    }
    return CloseThemeData_Orig(hTheme);
}

// --- Helpers ---
static ARGB ParseHexARGB(PCWSTR hex, ARGB fallback) {
    if (!hex || wcslen(hex) < 1)
        return fallback;
    std::wstring s(hex);
    if (s[0] == L'#')
        s = s.substr(1);
    try {
        unsigned long val = std::stoul(s, nullptr, 16);
        if (s.length() == 6)
            val |= 0xFF000000;
        return (ARGB)val;
    } catch (...) {
        return fallback;
    }
}

void LoadSettings() {
    PCWSTR s;
    s = Wh_GetStringSetting(L"barNormalStart");
    g_barNormalStart = ParseHexARGB(s, 0xFF2ECC71);
    Wh_FreeStringSetting(s);
    s = Wh_GetStringSetting(L"barNormalEnd");
    g_barNormalEnd = ParseHexARGB(s, 0xFF27AE60);
    Wh_FreeStringSetting(s);
    s = Wh_GetStringSetting(L"barFullStart");
    g_barFullStart = ParseHexARGB(s, 0xFFE74C3C);
    Wh_FreeStringSetting(s);
    s = Wh_GetStringSetting(L"barFullEnd");
    g_barFullEnd = ParseHexARGB(s, 0xFFC0392B);
    Wh_FreeStringSetting(s);
    s = Wh_GetStringSetting(L"trackColor");
    g_trackColor = ParseHexARGB(s, 0x20000000);
    Wh_FreeStringSetting(s);
    s = Wh_GetStringSetting(L"borderColor");
    g_borderColor = ParseHexARGB(s, 0x80FFFFFF);
    Wh_FreeStringSetting(s);
    g_gradientDirection = Wh_GetIntSetting(L"gradientDirection");
    g_cornerRadius = Wh_GetIntSetting(L"cornerRadius");
    g_showGloss = Wh_GetIntSetting(L"showGloss") != 0;
    g_roundFillBothSides = Wh_GetIntSetting(L"roundFillBothSides") != 0;
    g_borderThickness = (float)Wh_GetIntSetting(L"borderThickness");
    g_trackBorderOffset = (float)Wh_GetIntSetting(L"trackBorderOffset");
    g_fillPadding = (float)Wh_GetIntSetting(L"fillPadding");
    g_leftInset = Wh_GetIntSetting(L"leftInset");
    g_rightInset = Wh_GetIntSetting(L"rightInset");
    g_topInset = Wh_GetIntSetting(L"topInset");
    g_bottomInset = Wh_GetIntSetting(L"bottomInset");
    g_trackLeftInset = Wh_GetIntSetting(L"trackLeftInset");
    g_trackRightInset = Wh_GetIntSetting(L"trackRightInset");
    g_trackTopInset = Wh_GetIntSetting(L"trackTopInset");
    g_trackBottomInset = Wh_GetIntSetting(L"trackBottomInset");

    s = Wh_GetStringSetting(L"formatString");

    if (s && s[0] != L'\0') {
        g_formatString = s;
    } else {
        g_formatString = L"%s free | %s used\n%s total";
    }
    Wh_FreeStringSetting(s);
    size_t pos = 0;
    while ((pos = g_formatString.find(L"\\n", pos)) != std::wstring::npos) {
        g_formatString.replace(pos, 2, L"\n");
        pos += 1;
    }
    g_boldUsed = Wh_GetIntSetting(L"boldUsed") != 0;
    g_removeSpace = Wh_GetIntSetting(L"removeSpace") != 0;
    g_enableWordEllipsis = Wh_GetIntSetting(L"enableWordEllipsis") != 0;
    s = Wh_GetStringSetting(L"boldStyle");
    g_boldStyle = (s && wcscmp(s, L"serif") == 0) ? BoldStyle::Serif
                                                  : BoldStyle::SansSerif;
    Wh_FreeStringSetting(s);
    g_lineYOffset = Wh_GetIntSetting(L"lineYOffset");
    g_barYOffset = Wh_GetIntSetting(L"barYOffset");
    g_lineSpacing = Wh_GetIntSetting(L"lineSpacing");
    g_fontSize = Wh_GetIntSetting(L"fontSize");
}

std::wstring CleanNumericString(const std::wstring& s) {
    std::wstring result;
    bool start = false;
    for (wchar_t c : s) {
        wchar_t check = (c == 0xA0) ? L' ' : c;
        if (!start) {
            if (iswdigit(check) || check == L'.' || check == L',' ||
                check == L'-') {
                start = true;
                result += (check == L',') ? L'.' : check;
            }
            continue;
        }
        if (check != L' ')
            result += (check == L',') ? L'.' : check;
    }
    return result;
}

bool IsValidUnitString(const wchar_t* u) {
    size_t len = wcslen(u);
    if (len == 0 || len > 10)
        return false;
    for (size_t i = 0; i < len; ++i) {
        if (!iswalpha(u[i]))
            return false;
    }
    return true;
}

double GetUnitMultiplier(const wchar_t* u) {
    std::wstring up = u;
    std::transform(up.begin(), up.end(), up.begin(), ::towupper);

    wchar_t prefix = 0;
    for (wchar_t c : up) {
        if (c != L' ' && c != L'.' && c != L',') {
            prefix = c;
            break;
        }
    }

    if (prefix == L'T' || prefix == L'\x0422')
        return 1099511627776.0;
    if (prefix == L'G' || prefix == L'\x0413')
        return 1073741824.0;
    if (prefix == L'M' || prefix == L'\x041C')
        return 1048576.0;
    if (prefix == L'K' || prefix == L'\x041A')
        return 1024.0;
    if (prefix == L'B' || prefix == L'\x0411' || prefix == L'O')
        return 1.0;

    return 0.0;
}

std::wstring MakeBoldText(const std::wstring& s) {
    std::wstring res;
    for (wchar_t c : s) {
        if (c >= L'0' && c <= L'9') {
            res += (wchar_t)0xD835;
            res +=
                (wchar_t)((g_boldStyle == BoldStyle::Serif ? 0xDFCE : 0xDFEC) +
                          (c - L'0'));
        } else if (c >= L'A' && c <= L'Z') {
            res += (wchar_t)0xD835;
            res +=
                (wchar_t)((g_boldStyle == BoldStyle::Serif ? 0xDC00 : 0xDDD4) +
                          (c - L'A'));
        } else if (c >= L'a' && c <= L'z') {
            res += (wchar_t)0xD835;
            res +=
                (wchar_t)((g_boldStyle == BoldStyle::Serif ? 0xDC1A : 0xDDEE) +
                          (c - L'a'));
        } else
            res += c;
    }
    return res;
}

static bool IsValidDiskBarWindow(HWND hwnd) {
    if (!hwnd) {
        // WindowFromDC can return NULL for memory DCs (used in double
        // buffering). We must return true to allow drawing to these off-screen
        // buffers.
        return true;
    }
    HWND walk = hwnd;
    int limit = 15;
    while (walk && limit-- > 0) {
        wchar_t cls[MAX_PATH];
        if (GetClassNameW(walk, cls, MAX_PATH)) {
            std::wstring wCls(cls);
            std::transform(wCls.begin(), wCls.end(), wCls.begin(), ::towlower);

            if (wCls == L"#32770" || wCls == L"msctls_progress32" ||
                wCls.find(L"scrollbar") != std::wstring::npos ||
                wCls.find(L"header") != std::wstring::npos ||
                wCls.find(L"listview") != std::wstring::npos ||
                wCls.find(L"property") != std::wstring::npos) {
                return false;
            }
            if (wCls == L"directuihwnd") {
                return true;  // Fast exit if we hit the valid container
            }
        }
        walk = GetParent(walk);
    }
    return true;
}

thread_local static HDC g_lastBarDC = NULL;
thread_local static RECT g_lastBarRect = {0};

static void BuildRoundedPath(GraphicsPath& path,
                             RectF rect,
                             float radius,
                             bool rL = true,
                             bool rR = true) {
    path.Reset();
    if (radius < 0.5f) {
        path.AddRectangle(rect);
        return;
    }

    float d = radius * 2.0f;
    if (d > rect.Width)
        d = rect.Width;
    if (d > rect.Height)
        d = rect.Height;

    float x = rect.X;
    float y = rect.Y;
    float w = rect.Width;
    float h = rect.Height;

    path.StartFigure();

    // Top-left
    if (rL) {
        path.AddArc(x, y, d, d, 180, 90);
    } else {
        path.AddLine(x, y + radius, x, y);
        path.AddLine(x, y, x + radius, y);
    }

    // Top-right
    if (rR) {
        path.AddArc(x + w - d, y, d, d, 270, 90);
    } else {
        path.AddLine(x + w - radius, y, x + w, y);
        path.AddLine(x + w, y, x + w, y + radius);
    }

    // Bottom-right
    if (rR) {
        path.AddArc(x + w - d, y + h - d, d, d, 0, 90);
    } else {
        path.AddLine(x + w, y + h - radius, x + w, y + h);
        path.AddLine(x + w, y + h, x + w - radius, y + h);
    }

    // Bottom-left
    if (rL) {
        path.AddArc(x, y + h - d, d, d, 90, 90);
    } else {
        path.AddLine(x + radius, y + h, x, y + h);
        path.AddLine(x, y + h, x, y + h - radius);
    }
    path.CloseFigure();
}

static void PaintEnhancedBar(HDC hdc,
                             LPCRECT pRect,
                             LPCRECT pClipRect,
                             int iStateId,
                             bool isFill) {
    float scale = 1.0f;
    static auto pGetDpiForWindow = (UINT(WINAPI*)(HWND))GetProcAddress(
        GetModuleHandleW(L"user32.dll"), "GetDpiForWindow");

    HWND hwnd = WindowFromDC(hdc);
    if (pGetDpiForWindow && hwnd) {
        scale = (float)pGetDpiForWindow(hwnd) / 96.0f;
    } else {
        scale = (float)GetDeviceCaps(hdc, LOGPIXELSY) / 96.0f;
    }

    Graphics graphics{hdc};
    if (pClipRect) {
        graphics.SetClip(Rect(pClipRect->left, pClipRect->top,
                              pClipRect->right - pClipRect->left,
                              pClipRect->bottom - pClipRect->top));
    }

    graphics.SetSmoothingMode(SmoothingModeAntiAlias);
    graphics.SetPixelOffsetMode(PixelOffsetModeHalf);
    graphics.SetCompositingQuality(CompositingQualityHighQuality);
    graphics.SetInterpolationMode(InterpolationModeHighQualityBicubic);

    RectF barRect{(REAL)pRect->left, (REAL)pRect->top,
                  (REAL)(pRect->right - pRect->left),
                  (REAL)(pRect->bottom - pRect->top)};

    barRect.Y += (float)g_barYOffset;

    // 1. Calculate Track Geometry (The container)
    // We try to derive the track from g_lastBarRect to ensure the Fill pass
    // knows the full width for rounding its right side.
    RectF trackRect;
    if (isFill && g_lastBarRect.right > g_lastBarRect.left) {
        trackRect.X = (float)g_lastBarRect.left;
        trackRect.Y = (float)g_lastBarRect.top;
        trackRect.Width = (float)(g_lastBarRect.right - g_lastBarRect.left);
        trackRect.Height = (float)(g_lastBarRect.bottom - g_lastBarRect.top);
        trackRect.Y += (float)g_barYOffset;
    } else {
        trackRect = barRect;
    }

    trackRect.X += (float)g_trackLeftInset * scale;
    trackRect.Y += (float)g_trackTopInset * scale;
    trackRect.Width -= (float)(g_trackLeftInset + g_trackRightInset) * scale;
    trackRect.Height -= (float)(g_trackTopInset + g_trackBottomInset) * scale;

    if (trackRect.Width <= 0.1f || trackRect.Height <= 0.1f)
        return;

    // 2. Paths
    GraphicsPath trackPath;
    BuildRoundedPath(trackPath, trackRect, (float)g_cornerRadius * scale);

    RectF borderRect = trackRect;
    float bOff = g_trackBorderOffset * scale;
    if (bOff != 0) {
        borderRect.Inflate(bOff, bOff);
    }
    GraphicsPath borderPath;
    BuildRoundedPath(borderPath, borderRect, (float)g_cornerRadius * scale);

    if (!isFill) {
        // PASS A: Background
        SolidBrush trBr{Color{g_trackColor}};
        graphics.FillPath(&trBr, &trackPath);

        if (((g_borderColor >> 24) & 0xFF) > 0 && g_borderThickness > 0.01f) {
            Pen p{Color{g_borderColor}, g_borderThickness * scale};
            p.SetAlignment(PenAlignmentCenter);
            graphics.DrawPath(&p, &borderPath);
        }
    } else {
        // PASS B: Fill

        RectF fillRect = barRect;
        fillRect.X += (float)g_leftInset * scale;
        fillRect.Y += (float)g_topInset * scale;
        fillRect.Width -= (float)(g_leftInset + g_rightInset) * scale;
        fillRect.Height -= (float)(g_topInset + g_bottomInset) * scale;

        float fPad = g_fillPadding * scale;
        if (fPad != 0) {
            fillRect.Inflate(-fPad, -fPad);
        }

        if (fillRect.Width > 0.1f && fillRect.Height > 0.1f) {
            bool rR = g_roundFillBothSides;
            if (!rR) {
                // If fill reaches roughly the right side of the track, round it
                // too
                if (pRect->right >= g_lastBarRect.right - (int)(1 * scale)) {
                    rR = true;
                }
            }

            GraphicsPath fillPath;
            BuildRoundedPath(fillPath, fillRect, (float)g_cornerRadius * scale,
                             true, rR);

            ARGB c1 = (iStateId == 2) ? g_barFullStart : g_barNormalStart;
            ARGB c2 = (iStateId == 2) ? g_barFullEnd : g_barNormalEnd;
            RectF gradRect = fillRect;
            gradRect.Inflate(0.5f, 0.5f);
            LinearGradientBrush br{gradRect, Color{c1}, Color{c2},
                                   (REAL)g_gradientDirection};
            br.SetWrapMode(WrapModeTileFlipXY);
            graphics.FillPath(&br, &fillPath);

            if (g_showGloss) {
                graphics.SetClip(&fillPath);
                RectF gRect = fillRect;
                gRect.Y += 1.0f;
                gRect.Height -= 2.0f;
                gRect.Height = fmax(gRect.Height / 2.0f, 0.5f);
                LinearGradientBrush gBr{gRect, Color{142, 255, 255, 255},
                                        Color{0, 255, 255, 255}, 90.0f};
                gBr.SetWrapMode(WrapModeTileFlipXY);
                graphics.FillRectangle(&gBr, gRect);
                graphics.ResetClip();
            }
        }
    }
}

static bool IsDiskBar(HTHEME hTheme,
                      HDC hdc,
                      int iPartId,
                      int iStateId,
                      LPCRECT pRect) {
    if (!pRect)
        return false;
    if (iPartId != 1 && iPartId != 5 && iPartId != 11)
        return false;

    HWND hwnd = NULL;
    if (GetThemeWindow_Ptr)
        hwnd = GetThemeWindow_Ptr(hTheme);
    if (!hwnd)
        hwnd = WindowFromDC(hdc);
    if (!hwnd)
        hwnd = GetActiveWindow();

    static auto pGetDpiForWindow = (UINT(WINAPI*)(HWND))GetProcAddress(
        GetModuleHandleW(L"user32.dll"), "GetDpiForWindow");
    float scale = 1.0f;
    if (pGetDpiForWindow && hwnd) {
        scale = (float)pGetDpiForWindow(hwnd) / 96.0f;
    } else {
        // Fallback
        scale = (float)GetDeviceCaps(hdc, LOGPIXELSY) / 96.0f;
    }

    // Logical Dimensioning
    int h = pRect->bottom - pRect->top;
    int w = pRect->right - pRect->left;

    // Normalize physical pixels to logical bounds
    float logicalH = (float)h / scale;
    float logicalW = (float)w / scale;

    if (iPartId == 5) {
        if (logicalH < 2.0f || logicalH > 15.5f)
            return false;
    } else {
        if (logicalW < 30.0f)
            return false;
        if (logicalH < 6.0f || logicalH > 16.5f)
            return false;
    }

    if (GetThemeClassList_Ptr) {
        wchar_t themeCls[256];
        if (SUCCEEDED(GetThemeClassList_Ptr(hTheme, themeCls, 256))) {
            std::wstring tCls(themeCls);
            std::transform(tCls.begin(), tCls.end(), tCls.begin(), ::towlower);
            if (tCls.find(L"progress") == std::wstring::npos)
                return false;
            if (tCls.find(L"scrollbar") != std::wstring::npos ||
                tCls.find(L"header") != std::wstring::npos)
                return false;
        }
    }

    if (hwnd) {
        HWND walk = hwnd;
        int limit = 15;
        while (walk && limit-- > 0) {
            wchar_t cls[MAX_PATH];
            if (GetClassName(walk, cls, MAX_PATH)) {
                std::wstring wCls(cls);
                std::transform(wCls.begin(), wCls.end(), wCls.begin(),
                               ::towlower);

                if (wCls == L"#32770" || wCls == L"msctls_progress32" ||
                    wCls.find(L"scrollbar") != std::wstring::npos ||
                    wCls.find(L"header") != std::wstring::npos ||
                    wCls.find(L"listview") != std::wstring::npos ||
                    wCls.find(L"property") != std::wstring::npos) {
                    return false;
                }
            }
            walk = GetParent(walk);
        }
    }

    // prevent navpane being styled
    if (pRect->left < (int)(32 * scale))
        return false;
    return true;
}

// (Globals removed from here)

HRESULT WINAPI HookedDrawThemeBackground(HTHEME hTheme,
                                         HDC hdc,
                                         int iPartId,
                                         int iStateId,
                                         LPCRECT pRect,
                                         LPCRECT pClipRect) {
    if (IsDiskBar(hTheme, hdc, iPartId, iStateId, pRect)) {
        bool first = !(hdc == g_lastBarDC && EqualRect(pRect, &g_lastBarRect));

        if (iPartId == 5) {
            // Fill
            PaintEnhancedBar(hdc, pRect, pClipRect, iStateId, true);
        } else if (iPartId == 1 || iPartId == 11) {
            // Track / Background
            g_lastBarDC = hdc;
            g_lastBarRect = *pRect;
            if (first)
                PaintEnhancedBar(hdc, pRect, pClipRect, iStateId, false);
        }

        return S_OK;
    }
    return DrawThemeBackground_Orig(hTheme, hdc, iPartId, iStateId, pRect,
                                    pClipRect);
}

bool FindSpaceStats(const std::wstring& t, std::wstring& f, std::wstring& tot) {
    std::wstring nt = t;
    for (auto& c : nt) {
        if (c == 0xA0)
            c = L' ';
    }

    size_t num1_start = nt.find_first_of(L"0123456789");
    if (num1_start == std::wstring::npos)
        return false;

    size_t pos = num1_start;
    while (pos < nt.length() && (iswdigit(nt[pos]) || nt[pos] == L'.' ||
                                 nt[pos] == L',' || nt[pos] == L' '))
        pos++;
    while (pos < nt.length() && nt[pos] == L' ')
        pos++;
    while (pos < nt.length() && !iswdigit(nt[pos]) && nt[pos] != L' ')
        pos++;
    size_t size1_end = pos;

    size_t num2_start = nt.find_first_of(L"0123456789", size1_end);
    if (num2_start == std::wstring::npos)
        return false;

    if (num2_start == size1_end)
        return false;

    pos = num2_start;
    while (pos < nt.length() && (iswdigit(nt[pos]) || nt[pos] == L'.' ||
                                 nt[pos] == L',' || nt[pos] == L' '))
        pos++;
    while (pos < nt.length() && nt[pos] == L' ')
        pos++;
    while (pos < nt.length() && !iswdigit(nt[pos]) && nt[pos] != L' ')
        pos++;
    size_t size2_end = pos;

    f = nt.substr(num1_start, size1_end - num1_start);
    tot = nt.substr(num2_start, size2_end - num2_start);

    while (!f.empty() && f.back() == L' ')
        f.pop_back();
    while (!tot.empty() && tot.back() == L' ')
        tot.pop_back();

    return true;
}

int WINAPI DrawTextW_Hook(HDC hdc, LPCWSTR psz, int cch, LPRECT prc, UINT fmt) {
    if (!psz || !prc)
        return DrawTextW_Orig(hdc, psz, cch, prc, fmt);

    int len = (cch == -1) ? (int)wcslen(psz) : cch;
    bool hasNum = false;
    for (int i = 0; i < len; ++i) {
        if (psz[i] >= L'0' && psz[i] <= L'9') {
            hasNum = true;
            break;
        }
    }
    if (!hasNum)
        return DrawTextW_Orig(hdc, psz, cch, prc, fmt);

    // Target window verification
    if (hdc) {
        HWND hwnd = WindowFromDC(hdc);
        if (!IsValidDiskBarWindow(hwnd)) {
            return DrawTextW_Orig(hdc, psz, cch, prc, fmt);
        }
    }

    std::wstring t(psz, len);
    std::wstring fs, ts;
    if (FindSpaceStats(t, fs, ts)) {
        if (g_removeSpace) {
            fs.erase(std::remove(fs.begin(), fs.end(), L' '), fs.end());
            ts.erase(std::remove(ts.begin(), ts.end(), L' '), ts.end());
        }
        std::wstring cf = CleanNumericString(fs), ct = CleanNumericString(ts);
        double fv, tv;
        wchar_t fu[16], tu[16];

        if (swscanf(cf.c_str(), L"%lf %15s", &fv, fu) == 2 &&
            swscanf(ct.c_str(), L"%lf %15s", &tv, tu) == 2) {
            if (IsValidUnitString(fu) && IsValidUnitString(tu)) {
                double um1 = GetUnitMultiplier(fu);
                double um2 = GetUnitMultiplier(tu);
                if (um1 > 0.0 && um2 > 0.0) {
                    std::wstring us = std::wstring(StrFormatByteSizeW(
                        (ULONGLONG)std::max(0.0, (tv * um2 - fv * um1)), fu,
                        16));

                    if (g_removeSpace)
                        us.erase(std::remove(us.begin(), us.end(), L' '),
                                 us.end());
                    std::wstring fS = g_formatString;
                    size_t p1 = fS.find(L"%s");
                    size_t p2 = p1 != std::wstring::npos
                                    ? fS.find(L"%s", p1 + 2)
                                    : std::wstring::npos;
                    size_t p3 = p2 != std::wstring::npos
                                    ? fS.find(L"%s", p2 + 2)
                                    : std::wstring::npos;

                    if (p1 == std::wstring::npos || p2 == std::wstring::npos ||
                        p3 == std::wstring::npos) {
                        fS = L"%s free | %s used\n%s total";
                        p1 = fS.find(L"%s");
                        p2 = fS.find(L"%s", p1 + 2);
                        p3 = fS.find(L"%s", p2 + 2);
                    }

                    std::wstring s1 = fS.substr(0, p1) + fs +
                                      fS.substr(p1 + 2, p2 - p1 - 2),
                                 s2 = g_boldUsed ? MakeBoldText(us) : us,
                                 s3 = fS.substr(p2 + 2, p3 - p2 - 2) + ts +
                                      fS.substr(p3 + 2);

                    if (fmt & DT_CALCRECT) {
                        std::wstring ft = s1 + s2 + s3;
                        UINT calcFmt = fmt;
                        if (ft.find(L'\n') != std::wstring::npos) {
                            calcFmt &= ~DT_SINGLELINE;
                        }
                        return DrawTextW_Orig(hdc, ft.c_str(), (int)ft.length(),
                                              prc, calcFmt);
                    }

                    int applyLineYOffset = g_lineYOffset;

                    HFONT hOldFont = NULL;
                    HFONT hNewFont = NULL;
                    float scale = (float)GetDeviceCaps(hdc, LOGPIXELSY) / 96.0f;
                    if (g_fontSize != 0) {
                        HFONT hCurrent = (HFONT)GetCurrentObject(hdc, OBJ_FONT);
                        LOGFONTW lf;
                        if (GetObjectW(hCurrent, sizeof(lf), &lf)) {
                            if (lf.lfHeight < 0)
                                lf.lfHeight -= (int)(g_fontSize * scale);
                            else
                                lf.lfHeight += (int)(g_fontSize * scale);
                            hNewFont = CreateFontIndirectW(&lf);
                            if (hNewFont)
                                hOldFont = (HFONT)SelectObject(hdc, hNewFont);
                        }
                    }

                    std::wstring ft = s1 + s2 + s3;
                    bool multiLine = (ft.find(L'\n') != std::wstring::npos);

                    UINT dF = fmt | DT_NOPREFIX;
                    if (!g_enableWordEllipsis) {
                        dF &= ~(DT_END_ELLIPSIS | DT_PATH_ELLIPSIS |
                                DT_WORD_ELLIPSIS);
                    } else {
                        dF |= DT_WORD_ELLIPSIS | DT_END_ELLIPSIS;
                    }
                    RECT r = *prc;
                    r.top += applyLineYOffset;
                    r.bottom += applyLineYOffset;

                    int ft_ret = 0;
                    if (multiLine) {
                        std::vector<std::wstring> lines;
                        size_t start_pos = 0, end_pos = 0;
                        while ((end_pos = ft.find(L'\n', start_pos)) !=
                               std::wstring::npos) {
                            lines.push_back(
                                ft.substr(start_pos, end_pos - start_pos));
                            start_pos = end_pos + 1;
                        }
                        lines.push_back(ft.substr(start_pos));

                        dF &= ~(DT_SINGLELINE | DT_VCENTER | DT_BOTTOM);
                        dF |= DT_TOP | DT_SINGLELINE | DT_NOCLIP | DT_NOPREFIX;

                        int totalHeight = 0;
                        for (size_t i = 0; i < lines.size(); ++i) {
                            RECT lr = r;
                            lr.top += totalHeight;
                            lr.bottom += 1000;
                            if (fmt & DT_CALCRECT) {
                                DrawTextW_Orig(hdc, lines[i].c_str(),
                                               (int)lines[i].length(), &lr,
                                               dF | DT_CALCRECT);
                                totalHeight +=
                                    (lr.bottom - lr.top) + g_lineSpacing;
                            } else {
                                RECT calcR = lr;
                                DrawTextW_Orig(hdc, lines[i].c_str(),
                                               (int)lines[i].length(), &calcR,
                                               dF | DT_CALCRECT);
                                DrawTextW_Orig(hdc, lines[i].c_str(),
                                               (int)lines[i].length(), &lr, dF);
                                totalHeight +=
                                    (calcR.bottom - calcR.top) + g_lineSpacing;
                            }
                        }
                        if (fmt & DT_CALCRECT) {
                            prc->bottom =
                                prc->top + totalHeight - g_lineSpacing;
                        }
                        ft_ret = totalHeight - g_lineSpacing;
                    } else {
                        ft_ret = DrawTextW_Orig(hdc, ft.c_str(),
                                                (int)ft.length(), &r, dF);
                    }

                    if (hOldFont) {
                        SelectObject(hdc, hOldFont);
                        DeleteObject(hNewFont);
                    }
                    return ft_ret;
                }
            }
        }
    }

    return DrawTextW_Orig(hdc, psz, cch, prc, fmt);
}

int WINAPI DrawTextExW_Hook(HDC hdc,
                            LPWSTR psz,
                            int cch,
                            LPRECT prc,
                            UINT fmt,
                            LPDRAWTEXTPARAMS pDtp) {
    if (!psz || !prc)
        return DrawTextExW_Orig(hdc, psz, cch, prc, fmt, pDtp);

    int len = (cch == -1) ? (int)wcslen(psz) : cch;
    bool hasNum = false;
    for (int i = 0; i < len; ++i) {
        if (psz[i] >= L'0' && psz[i] <= L'9') {
            hasNum = true;
            break;
        }
    }
    if (!hasNum)
        return DrawTextExW_Orig(hdc, psz, cch, prc, fmt, pDtp);

    // Target window verification
    if (hdc) {
        HWND hwnd = WindowFromDC(hdc);
        if (!IsValidDiskBarWindow(hwnd)) {
            return DrawTextExW_Orig(hdc, psz, cch, prc, fmt, pDtp);
        }
    }

    std::wstring t(psz, len);
    std::wstring fs, ts;

    if (FindSpaceStats(t, fs, ts)) {
        if (g_removeSpace) {
            fs.erase(std::remove(fs.begin(), fs.end(), L' '), fs.end());
            ts.erase(std::remove(ts.begin(), ts.end(), L' '), ts.end());
        }
        std::wstring cf = CleanNumericString(fs), ct = CleanNumericString(ts);
        double fv, tv;
        wchar_t fu[16], tu[16];

        if (swscanf(cf.c_str(), L"%lf %15s", &fv, fu) == 2 &&
            swscanf(ct.c_str(), L"%lf %15s", &tv, tu) == 2) {
            if (IsValidUnitString(fu) && IsValidUnitString(tu)) {
                double um1 = GetUnitMultiplier(fu);
                double um2 = GetUnitMultiplier(tu);
                if (um1 > 0.0 && um2 > 0.0) {
                    std::wstring us = std::wstring(StrFormatByteSizeW(
                        (ULONGLONG)std::max(0.0, (tv * um2 - fv * um1)), fu,
                        16));

                    if (g_removeSpace)
                        us.erase(std::remove(us.begin(), us.end(), L' '),
                                 us.end());
                    std::wstring fS = g_formatString;
                    size_t p1 = fS.find(L"%s");
                    size_t p2 = p1 != std::wstring::npos
                                    ? fS.find(L"%s", p1 + 2)
                                    : std::wstring::npos;
                    size_t p3 = p2 != std::wstring::npos
                                    ? fS.find(L"%s", p2 + 2)
                                    : std::wstring::npos;

                    if (p1 == std::wstring::npos || p2 == std::wstring::npos ||
                        p3 == std::wstring::npos) {
                        fS = L"%s free | %s used\n%s total";
                        p1 = fS.find(L"%s");
                        p2 = fS.find(L"%s", p1 + 2);
                        p3 = fS.find(L"%s", p2 + 2);
                    }

                    std::wstring s1 = fS.substr(0, p1) + fs +
                                      fS.substr(p1 + 2, p2 - p1 - 2),
                                 s2 = g_boldUsed ? MakeBoldText(us) : us,
                                 s3 = fS.substr(p2 + 2, p3 - p2 - 2) + ts +
                                      fS.substr(p3 + 2);

                    if (fmt & DT_CALCRECT) {
                        std::wstring ft = s1 + s2 + s3;
                        std::vector<wchar_t> b(ft.begin(), ft.end());
                        b.push_back(0);
                        UINT calcFmt = fmt;
                        if (ft.find(L'\n') != std::wstring::npos) {
                            calcFmt &= ~DT_SINGLELINE;
                        }
                        return DrawTextExW_Orig(hdc, b.data(), (int)ft.length(),
                                                prc, calcFmt, pDtp);
                    }

                    int applyLineYOffset = g_lineYOffset;

                    HFONT hOldFont = NULL;
                    HFONT hNewFont = NULL;
                    float scale = (float)GetDeviceCaps(hdc, LOGPIXELSY) / 96.0f;
                    if (g_fontSize != 0) {
                        HFONT hCurrent = (HFONT)GetCurrentObject(hdc, OBJ_FONT);
                        LOGFONTW lf;
                        if (GetObjectW(hCurrent, sizeof(lf), &lf)) {
                            if (lf.lfHeight < 0)
                                lf.lfHeight -= (int)(g_fontSize * scale);
                            else
                                lf.lfHeight += (int)(g_fontSize * scale);
                            hNewFont = CreateFontIndirectW(&lf);
                            if (hNewFont)
                                hOldFont = (HFONT)SelectObject(hdc, hNewFont);
                        }
                    }

                    std::wstring ft = s1 + s2 + s3;
                    bool multiLine = (ft.find(L'\n') != std::wstring::npos);

                    std::vector<wchar_t> b(ft.begin(), ft.end());
                    b.push_back(0);

                    UINT dF = fmt | DT_NOPREFIX;
                    if (!g_enableWordEllipsis) {
                        dF &= ~(DT_END_ELLIPSIS | DT_PATH_ELLIPSIS |
                                DT_WORD_ELLIPSIS);
                    } else {
                        dF |= DT_WORD_ELLIPSIS | DT_END_ELLIPSIS;
                    }
                    RECT r = *prc;
                    r.top += applyLineYOffset;
                    r.bottom += applyLineYOffset;

                    int ft_ret = 0;
                    if (multiLine) {
                        std::vector<std::wstring> lines;
                        size_t start_pos = 0, end_pos = 0;
                        while ((end_pos = ft.find(L'\n', start_pos)) !=
                               std::wstring::npos) {
                            lines.push_back(
                                ft.substr(start_pos, end_pos - start_pos));
                            start_pos = end_pos + 1;
                        }
                        lines.push_back(ft.substr(start_pos));

                        dF &= ~(DT_SINGLELINE | DT_VCENTER | DT_BOTTOM);
                        dF |= DT_TOP | DT_SINGLELINE | DT_NOCLIP | DT_NOPREFIX;

                        int totalHeight = 0;
                        for (size_t i = 0; i < lines.size(); ++i) {
                            RECT lr = r;
                            lr.top += totalHeight;
                            lr.bottom += 1000;
                            std::vector<wchar_t> bl(lines[i].begin(),
                                                    lines[i].end());
                            bl.push_back(0);
                            if (fmt & DT_CALCRECT) {
                                DrawTextExW_Orig(hdc, bl.data(),
                                                 (int)lines[i].length(), &lr,
                                                 dF | DT_CALCRECT, pDtp);
                                totalHeight +=
                                    (lr.bottom - lr.top) + g_lineSpacing;
                            } else {
                                RECT calcR = lr;
                                DrawTextExW_Orig(hdc, bl.data(),
                                                 (int)lines[i].length(), &calcR,
                                                 dF | DT_CALCRECT, pDtp);
                                DrawTextExW_Orig(hdc, bl.data(),
                                                 (int)lines[i].length(), &lr,
                                                 dF, pDtp);
                                totalHeight +=
                                    (calcR.bottom - calcR.top) + g_lineSpacing;
                            }
                        }
                        if (fmt & DT_CALCRECT) {
                            prc->bottom =
                                prc->top + totalHeight - g_lineSpacing;
                        }
                        ft_ret = totalHeight - g_lineSpacing;
                    } else {
                        ft_ret = DrawTextExW_Orig(
                            hdc, b.data(), (int)ft.length(), &r, dF, pDtp);
                    }

                    if (hOldFont) {
                        SelectObject(hdc, hOldFont);
                        DeleteObject(hNewFont);
                    }
                    return ft_ret;
                }
            }
        }
    }

    return DrawTextExW_Orig(hdc, psz, cch, prc, fmt, pDtp);
}

static BOOL CALLBACK RefreshExplorerCallback(HWND hwnd, LPARAM lParam) {
    DWORD dwProcessId;
    GetWindowThreadProcessId(hwnd, &dwProcessId);
    if (dwProcessId != GetCurrentProcessId()) {
        return TRUE;
    }

    wchar_t cls[MAX_PATH];
    if (GetClassNameW(hwnd, cls, MAX_PATH)) {
        if (wcscmp(cls, L"CabinetWClass") == 0) {
            PostMessage(hwnd, WM_COMMAND, 41504, 0);  // Refresh command
            InvalidateRect(hwnd, NULL, TRUE);
        } else if (wcscmp(cls, L"DirectUIHWND") == 0) {
            InvalidateRect(hwnd, NULL, TRUE);
        }
    }
    return TRUE;
}

void RefreshExplorer() {
    SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, NULL, NULL);
    SendMessageTimeout(HWND_BROADCAST, WM_SETTINGCHANGE, 0,
                       (LPARAM)L"Transitions", SMTO_ABORTIFHUNG, 5000, NULL);
    SendMessageTimeout(HWND_BROADCAST, WM_THEMECHANGED, 0, 0, SMTO_ABORTIFHUNG,
                       5000, NULL);

    EnumWindows(RefreshExplorerCallback, 0);
}

BOOL Wh_ModInit() {
    GdiplusStartupInput gsi;
    GdiplusStartup(&g_gdiplusToken, &gsi, NULL);
    LoadSettings();
    HMODULE uxtheme = GetModuleHandle(L"uxtheme.dll");
    if (uxtheme) {
        GetThemeWindow_Ptr =
            (GetThemeWindow_t)GetProcAddress(uxtheme, "GetThemeWindow");
        GetThemeClassList_Ptr =
            (GetThemeClassList_t)GetProcAddress(uxtheme, "GetThemeClassList");

        Wh_SetFunctionHook((void*)GetProcAddress(uxtheme, "OpenThemeData"),
                           (void*)OpenThemeData_Hook,
                           (void**)&OpenThemeData_Orig);
        Wh_SetFunctionHook((void*)GetProcAddress(uxtheme, "OpenThemeDataEx"),
                           (void*)OpenThemeDataEx_Hook,
                           (void**)&OpenThemeDataEx_Orig);
        Wh_SetFunctionHook((void*)GetProcAddress(uxtheme, "CloseThemeData"),
                           (void*)CloseThemeData_Hook,
                           (void**)&CloseThemeData_Orig);

        Wh_SetFunctionHook(
            (void*)GetProcAddress(uxtheme, "DrawThemeBackground"),
            (void*)HookedDrawThemeBackground,
            (void**)&DrawThemeBackground_Orig);
    }

    if (uxtheme) {
        void* pOpenDpi = (void*)GetProcAddress(uxtheme, "OpenThemeDataForDpi");
        if (pOpenDpi) {
            Wh_SetFunctionHook(pOpenDpi, (void*)OpenThemeDataForDpi_Hook,
                               (void**)&OpenThemeDataForDpi_Orig);
        }
    }

    HMODULE user32 = GetModuleHandle(L"user32.dll");
    if (user32) {
        Wh_SetFunctionHook((void*)GetProcAddress(user32, "DrawTextW"),
                           (void*)DrawTextW_Hook, (void**)&DrawTextW_Orig);
        Wh_SetFunctionHook((void*)GetProcAddress(user32, "DrawTextExW"),
                           (void*)DrawTextExW_Hook, (void**)&DrawTextExW_Orig);
    }
    RefreshExplorer();
    return TRUE;
}

void Wh_ModUninit() {
    GdiplusShutdown(g_gdiplusToken);
    RefreshExplorer();
}
void Wh_ModSettingsChanged() {
    LoadSettings();
    RefreshExplorer();
}
