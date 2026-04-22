// ==WindhawkMod==
// @id              enhanced-disk-usage
// @name            Enhanced Disk Usage
// @description     Enables the ability to customize the disk drive tiles in explorer. Several customizations are available for enhancing the
// @version         1.0
// @author          bbmaster123
// @github          https://github.com/bbmaster123
// @include         explorer.exe
// @compilerOptions -lcomctl32 -lole32 -luuid -luser32 -lgdi32 -luxtheme -lshlwapi -lmsimg32 -lgdiplus
// ==/WindhawkMod==
// ==WindhawkModReadme==
/*
![Screenshot](https://raw.githubusercontent.com/bbmaster123/FWFU/refs/heads/main/Assets/screenshot.png)

Enables the ability to customize the disk drive tiles in explorer.
Several customizations are available for enhancing the disk's usage bar, as well
as the details that appear below.

- custom colors with transparency for disk usage, track (background/unused), and
outline
- separate disk colors for when drive is near full
- vertical linear gradient support
- rounded corners
- glossy overlay toggle option (for a more Windows Aero-ish look aestethic)
- height/width controls (inset)
- custom disk usage text.
ex.
100GB free | 100GB/200GB Used
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*

- barNormalStart: "#2ECC71"
  $name: Bar Color Gradient Start
  $description: set start and end to the same value if you don't want a gradient
- barNormalEnd: "#27AE60"
  $name: Bar Color Gradient End
- barFullStart: "#E74C3C"
  $name: Bar Full Color Gradient Start
- barFullEnd: "#C0392B"
  $name: Bar Full Color Graient End
- trackColor: "#20000000"
  $name: Track Color (Unused Space)
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
  $name: Inset Left
- rightInset: 0
  $name: Inset Right
- topInset: 0
  $name: Inset Top
- bottomInset: 0
  $name: Inset Bottom
- formatString: "%s free | %s used of %s"
  $name: Text Display Format
- boldUsed: true
  $name: Bold Used Space text
- boldStyle: sans-serif
  $name: Text Style
  $options:
    - serif: Serif Bold
    - sans-serif: Sans-Serif Bold
- removeSpace: false
  $name: Remove Space before Units (100GB/100 GB)
- lineYOffset: 0
  $name: Text Vertical Offset
- boldYOffset: 0
  $name: Text Bold Segment Offset
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
#include <string>
#include <vector>

using namespace Gdiplus;

// --- Global State ---
enum class BoldStyle { Serif, SansSerif };
std::wstring g_formatString;
bool g_boldUsed, g_removeSpace, g_showGloss;
int g_lineYOffset, g_boldYOffset, g_cornerRadius;
int g_leftInset, g_rightInset, g_topInset, g_bottomInset;
int g_gradientDirection;
ARGB g_barNormalStart, g_barNormalEnd, g_barFullStart, g_barFullEnd,
    g_trackColor, g_borderColor;
float g_borderThickness;
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
    g_borderThickness = (float)Wh_GetIntSetting(L"borderThickness");
    g_leftInset = Wh_GetIntSetting(L"leftInset");
    g_rightInset = Wh_GetIntSetting(L"rightInset");
    g_topInset = Wh_GetIntSetting(L"topInset");
    g_bottomInset = Wh_GetIntSetting(L"bottomInset");

    s = Wh_GetStringSetting(L"formatString");
    g_formatString = s ? s : L"%s free | %s used of %s";
    Wh_FreeStringSetting(s);
    g_boldUsed = Wh_GetIntSetting(L"boldUsed") != 0;
    g_removeSpace = Wh_GetIntSetting(L"removeSpace") != 0;
    s = Wh_GetStringSetting(L"boldStyle");
    g_boldStyle = (s && wcscmp(s, L"serif") == 0) ? BoldStyle::Serif
                                                  : BoldStyle::SansSerif;
    Wh_FreeStringSetting(s);
    g_lineYOffset = Wh_GetIntSetting(L"lineYOffset");
    g_boldYOffset = Wh_GetIntSetting(L"boldYOffset");
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
                result += check;
            }
            continue;
        }
        if (check != L' ')
            result += check;
    }
    return result;
}

double GetUnitMultiplier(const wchar_t* u) {
    if (wcsstr(u, L"TB"))
        return 1099511627776.0;
    if (wcsstr(u, L"GB"))
        return 1073741824.0;
    if (wcsstr(u, L"MB"))
        return 1048576.0;
    if (wcsstr(u, L"KB"))
        return 1024.0;
    return 1.0;  // bytes or B
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

static void BuildRoundedPath(GraphicsPath& path, RectF rect, float radius) {
    float d = std::min(radius * 2.0f, rect.Height);
    if (d < 1.0f) {
        path.AddRectangle(rect);
        return;
    }
    path.AddArc(rect.X, rect.Y, d, d, 180.0f, 90.0f);
    path.AddArc(rect.X + rect.Width - d, rect.Y, d, d, 270.0f, 90.0f);
    path.AddArc(rect.X + rect.Width - d, rect.Y + rect.Height - d, d, d, 0.0f,
                90.0f);
    path.AddArc(rect.X, rect.Y + rect.Height - d, d, d, 90.0f, 90.0f);
    path.CloseFigure();
}

static void PaintEnhancedBar(HDC hdc,
                             LPCRECT pRect,
                             int iStateId,
                             bool isFill) {
    Graphics graphics{hdc};
    graphics.SetSmoothingMode(SmoothingModeAntiAlias);
    graphics.SetPixelOffsetMode(PixelOffsetModeHighQuality);
    RectF rect{(REAL)pRect->left, (REAL)pRect->top,
               (REAL)(pRect->right - pRect->left),
               (REAL)(pRect->bottom - pRect->top)};

    GraphicsPath trackPath;
    BuildRoundedPath(trackPath, rect, (float)g_cornerRadius);

    if (isFill) {
        rect.X += (float)g_leftInset;
        rect.Y += (float)g_topInset;
        rect.Width -= (float)(g_leftInset + g_rightInset);
        rect.Height -= (float)(g_topInset + g_bottomInset);
        if (rect.Width > 1 && rect.Height > 1) {
            GraphicsPath fillPath;
            BuildRoundedPath(fillPath, rect, (float)g_cornerRadius);
            ARGB c1 = (iStateId == 2) ? g_barFullStart : g_barNormalStart;
            ARGB c2 = (iStateId == 2) ? g_barFullEnd : g_barNormalEnd;
            LinearGradientBrush br{rect, Color{c1}, Color{c2},
                                   static_cast<REAL>(g_gradientDirection)};
            graphics.FillPath(&br, &fillPath);
            if (g_showGloss) {
                // Smooth full-height gloss
                LinearGradientBrush gBr{rect, Color{142, 255, 255, 255},
                                        Color{0, 255, 255, 255}, 90.0f};
                graphics.SetClip(&fillPath);
                graphics.FillRectangle(&gBr, rect);
                graphics.ResetClip();
            }
        }
    } else {
        SolidBrush trBr{Color{g_trackColor}};
        graphics.FillPath(&trBr, &trackPath);
        if (((g_borderColor >> 24) & 0xFF) > 0) {
            Pen p{Color{g_borderColor}, g_borderThickness};
            p.SetAlignment(PenAlignmentCenter);
            graphics.DrawPath(&p, &trackPath);
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

static RECT g_lastBarRect = {0};
static HDC g_lastBarDC = NULL;

HRESULT WINAPI HookedDrawThemeBackground(HTHEME hTheme,
                                         HDC hdc,
                                         int iPartId,
                                         int iStateId,
                                         LPCRECT pRect,
                                         LPCRECT pClipRect) {
    if (IsDiskBar(hTheme, hdc, iPartId, iStateId, pRect)) {
        // Suppress native drawing entirely
        bool first = !(hdc == g_lastBarDC && EqualRect(pRect, &g_lastBarRect));

        if (iPartId == 5) {
            PaintEnhancedBar(hdc, pRect, iStateId, true);
        } else if (iPartId == 1 || iPartId == 11) {
            if (first)
                PaintEnhancedBar(hdc, pRect, iStateId, false);
        }

        g_lastBarDC = hdc;
        g_lastBarRect = *pRect;
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

    // Search for the split points used by Explorer
    size_t split = nt.find(L" free of ");
    if (split == std::wstring::npos)
        split = nt.find(L" free | ");

    if (split != std::wstring::npos) {
        f = nt.substr(0, split);  // Number + Unit (e.g. "128 GB" or "0 bytes")
        tot = nt.substr(split + (nt[split + 6] == L'|' ? 8 : 9));  // Total
        return true;
    }

    // Fallback: search for first unit pair
    const wchar_t* units[] = {L" bytes", L" KB", L" MB", L" GB", L" TB", L" B"};
    size_t u1 = std::wstring::npos;
    for (auto u : units) {
        size_t p = nt.find(u);
        if (p != std::wstring::npos) {
            u1 = p + wcslen(u);
            break;
        }
    }
    if (u1 == std::wstring::npos)
        return false;

    f = nt.substr(0, u1);
    // Find a second unit for total
    size_t lastNum = nt.find_last_of(L"0123456789");
    if (lastNum != std::wstring::npos && lastNum > u1 + 2) {
        size_t nextU = std::wstring::npos;
        for (auto u : units) {
            size_t p = nt.find(u, lastNum);
            if (p != std::wstring::npos) {
                nextU = p + wcslen(u);
                break;
            }
        }
        if (nextU != std::wstring::npos) {
            size_t start = nt.find_last_not_of(L"0123456789., ", lastNum);
            if (start == std::wstring::npos)
                start = 0;
            else
                start++;
            tot = nt.substr(start, nextU - start);
            return true;
        }
    }
    return false;
}

int WINAPI DrawTextW_Hook(HDC hdc, LPCWSTR psz, int cch, LPRECT prc, UINT fmt) {
    if (!psz || !prc)
        return DrawTextW_Orig(hdc, psz, cch, prc, fmt);
    std::wstring t(psz, cch == -1 ? wcslen(psz) : cch);
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
            std::wstring us = std::wstring(StrFormatByteSizeW(
                (ULONGLONG)std::max(0.0, (tv * GetUnitMultiplier(tu) -
                                          fv * GetUnitMultiplier(fu))),
                fu, 16));

            if (g_removeSpace)
                us.erase(std::remove(us.begin(), us.end(), L' '), us.end());
            std::wstring fS = g_formatString;
            size_t p1 = fS.find(L"%s"), p2 = fS.find(L"%s", p1 + 2),
                   p3 = fS.find(L"%s", p2 + 2);
            std::wstring s1 = fS.substr(0, p1) + fs +
                              fS.substr(p1 + 2, p2 - p1 - 2),
                         s2 = g_boldUsed ? MakeBoldText(us) : us,
                         s3 = fS.substr(p2 + 2, p3 - p2 - 2) + ts +
                              fS.substr(p3 + 2);

            if (fmt & DT_CALCRECT) {
                std::wstring ft = s1 + s2 + s3;
                return DrawTextW_Orig(hdc, ft.c_str(), (int)ft.length(), prc,
                                      fmt);
            }
            UINT dF =
                (fmt &
                 ~(DT_END_ELLIPSIS | DT_PATH_ELLIPSIS | DT_WORD_ELLIPSIS)) |
                DT_LEFT | DT_SINGLELINE | DT_NOPREFIX | DT_NOCLIP | DT_TOP;
            SIZE sz1, sz2, sz3;
            GetTextExtentPoint32W(hdc, s1.c_str(), (int)s1.length(), &sz1);
            GetTextExtentPoint32W(hdc, s2.c_str(), (int)s2.length(), &sz2);
            GetTextExtentPoint32W(hdc, s3.c_str(), (int)s3.length(), &sz3);
            int x = prc->left;
            if (fmt & DT_CENTER)
                x += (prc->right - prc->left - (sz1.cx + sz2.cx + sz3.cx)) / 2;
            else if (fmt & DT_RIGHT)
                x = prc->right - (sz1.cx + sz2.cx + sz3.cx);
            RECT r = *prc;
            r.top += g_lineYOffset;
            r.bottom += g_lineYOffset;
            r.left = x;
            r.right = x + sz1.cx;
            DrawTextW_Orig(hdc, s1.c_str(), (int)s1.length(), &r, dF);
            r.left = r.right;
            r.right = r.left + sz2.cx;
            r.top += g_boldYOffset;
            r.bottom += g_boldYOffset;
            DrawTextW_Orig(hdc, s2.c_str(), (int)s2.length(), &r, dF);
            r.left = r.right;
            r.right = r.left + sz3.cx;
            r.top -= g_boldYOffset;
            r.bottom -= g_boldYOffset;
            return DrawTextW_Orig(hdc, s3.c_str(), (int)s3.length(), &r, dF);
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
    std::wstring t(psz, cch == -1 ? wcslen(psz) : cch);
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
            std::wstring us = std::wstring(StrFormatByteSizeW(
                (ULONGLONG)std::max(0.0, (tv * GetUnitMultiplier(tu) -
                                          fv * GetUnitMultiplier(fu))),
                fu, 16));

            if (g_removeSpace)
                us.erase(std::remove(us.begin(), us.end(), L' '), us.end());
            std::wstring fS = g_formatString;
            size_t p1 = fS.find(L"%s"), p2 = fS.find(L"%s", p1 + 2),
                   p3 = fS.find(L"%s", p2 + 2);
            std::wstring s1 = fS.substr(0, p1) + fs +
                              fS.substr(p1 + 2, p2 - p1 - 2),
                         s2 = g_boldUsed ? MakeBoldText(us) : us,
                         s3 = fS.substr(p2 + 2, p3 - p2 - 2) + ts +
                              fS.substr(p3 + 2);

            if (fmt & DT_CALCRECT) {
                std::wstring ft = s1 + s2 + s3;
                std::vector<wchar_t> b(ft.begin(), ft.end());
                b.push_back(0);
                return DrawTextExW_Orig(hdc, b.data(), (int)ft.length(), prc,
                                        fmt, pDtp);
            }

            SIZE sz1, sz2, sz3;

            GetTextExtentPoint32W(hdc, s1.c_str(), (int)s1.length(), &sz1);
            GetTextExtentPoint32W(hdc, s2.c_str(), (int)s2.length(), &sz2);
            GetTextExtentPoint32W(hdc, s3.c_str(), (int)s3.length(), &sz3);
            int x = prc->left;

            if (fmt & DT_CENTER)
                x += (prc->right - prc->left - (sz1.cx + sz2.cx + sz3.cx)) / 2;
            RECT r = *prc;
            r.top += g_lineYOffset;
            r.bottom += g_lineYOffset;
            r.left = x;
            r.right = x + sz1.cx;
            std::vector<wchar_t> b1(s1.begin(), s1.end());
            b1.push_back(0);
            DrawTextExW_Orig(hdc, b1.data(), (int)s1.length(), &r,
                             fmt | DT_NOPREFIX | DT_LEFT, pDtp);
            r.left = r.right;
            r.right = r.left + sz2.cx;
            r.top += g_boldYOffset;
            r.bottom += g_boldYOffset;
            std::vector<wchar_t> b2(s2.begin(), s2.end());
            b2.push_back(0);
            DrawTextExW_Orig(hdc, b2.data(), (int)s2.length(), &r,
                             fmt | DT_NOPREFIX | DT_LEFT, pDtp);
            r.left = r.right;
            r.right = r.left + sz3.cx;
            r.top -= g_boldYOffset;
            r.bottom -= g_boldYOffset;
            std::vector<wchar_t> b3(s3.begin(), s3.end());
            b3.push_back(0);
            return DrawTextExW_Orig(hdc, b3.data(), (int)s3.length(), &r,
                                    fmt | DT_NOPREFIX | DT_LEFT, pDtp);
        }
    }
    return DrawTextExW_Orig(hdc, psz, cch, prc, fmt, pDtp);
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
        Wh_SetFunctionHook(
            (void*)GetProcAddress(uxtheme, "DrawThemeBackground"),
            (void*)HookedDrawThemeBackground,
            (void**)&DrawThemeBackground_Orig);
    }
    Wh_SetFunctionHook(
        (void*)GetProcAddress(GetModuleHandle(L"user32.dll"), "DrawTextW"),
        (void*)DrawTextW_Hook, (void**)&DrawTextW_Orig);
    Wh_SetFunctionHook(
        (void*)GetProcAddress(GetModuleHandle(L"user32.dll"), "DrawTextExW"),
        (void*)DrawTextExW_Hook, (void**)&DrawTextExW_Orig);
    return TRUE;
}
void Wh_ModUninit() {
    GdiplusShutdown(g_gdiplusToken);
}
void Wh_ModSettingsChanged() {
    LoadSettings();
}
