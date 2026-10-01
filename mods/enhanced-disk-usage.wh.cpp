// ==WindhawkMod==
// @id              enhanced-disk-usage
// @name            Enhanced Disk Usage
// @description     Enables the ability to customize the disk drive tiles in explorer, targeting the disk's usage bar, as well as the details that appear below.
// @version         1.2.0
// @author          bbmaster123
// @github          https://github.com/bbmaster123
// @include         explorer.exe
// @compilerOptions -luser32 -lgdi32 -luxtheme -lshlwapi -lgdiplus
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
![Screenshot](https://raw.githubusercontent.com/bbmaster123/FWFU/refs/heads/main/Assets/screenshot-1.2.0.png)

Enables the ability to customize the disk drive tiles in explorer, targeting the disk's usage bar, as well
as the details that appear below.

New in 1.2.0
- separate disk bar and text customization toggles
- updated text formatting to support any subset of stats in any order
- independent bold toggles for free, used, total, and both percentages
- added used percentage (%p) and free percentage (%fp) stats
- added unit normalization (ex show 1.5TB as 1536GB, or 512GB as 0.5TB)
- optional unit precision (0-10 decimals when converting units) and separate percentage precision (0-10)

Features
- follow system accent color, or set custom colors with transparency for disk usage, track (background/unused), and outline
- separate disk colors for when drive is near full
- linear gradient support with configurable direction
- rounded corners
- glossy overlay toggle option (for a more Windows Aero-ish looking aesthetic)
- height/width (inset) controls for disk bar and track
- custom disk usage text with font size adjustment, multi-line support, line-height adjustment, and more
- named stat placeholders (%f for free, %u for used, %t for total, %p for used percentage, %fp for free percentage)

Named placeholders can appear in any order or be repeated. Legacy `%s` placeholders
still insert free, used, and total space in that order. Use `%%` for a literal
percent sign. Values are estimated from Explorer's displayed, rounded sizes.

ex.
100GB free | 100GB/200GB
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- enableBarCustomization: true
  $name: Enable Disk Bar Customization
  $description: Customize disk usage bars. Turn off to keep Windows' default bars while still customizing the text.
- useAccentColor: false
  $name: Follow System Accent Color
  $description: Uses the system accent color for the disk usage bar gradient. Disable to use custom colors below
- accentColorGradientDelta: 15
  $name: Accent Color Gradient Delta (%)
  $description: How strong the gradient appears. Set to 0 for a solid color.
- barNormalStart: "#FF2ECC71"
  $name: Disk Bar Color Gradient Start
  $description: set start and end to the same value if you do not want a gradient
- barNormalEnd: "#FF27AE60"
  $name: Disk Bar Color Gradient End
- barFullStart: "#FFE74C3C"
  $name: Disk Bar Full Color Gradient Start
- barFullEnd: "#FFC0392B"
  $name: Disk Bar Full Color Gradient End
- gradientDirection: 90
  $name: Gradient Direction
- cornerRadius: 24
  $name: Corner Radius (Quarter Pixels)
  $description: Set corner radius in quarter pixels. Ex. 41 = 10.25px, 22 = 5.5px
- roundFillBothSides: true
  $name: Round Both Sides of Fill
  $description: If enabled, both sides of the progress bar will be rounded. If disabled, the right side will be flat unless full.   
- showGloss: true
  $name: Enable Glossy Overlay
- fillPadding: 0
  $name: Disk Bar Fill Padding (Quarter Pixels)
  $description: Extra space between the progress fill and the track border in quarter pixels.  
- leftInset: 0
  $name: Disk Bar Inset Left
- rightInset: 0
  $name: Disk Bar Inset Right
- topInset: 0
  $name: Disk Bar Inset Top
- bottomInset: 0
  $name: Disk Bar Inset Bottom
- barYOffset: 0
  $name: Disk Bar Vertical Offset  
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
- borderColor: "#80BBBBBB"
  $name: Border Color
- borderThickness: 4
  $name: Border Thickness (Quarter Pixels)
  $description: Border thickness in quarter pixels
- trackBorderOffset: 0
  $name: Border Offset (Quarter Pixels)
  $description: Adjusts the border position relative to the track in quarter pixels. Positive values expand outwards.
- enableTextCustomization: true
  $name: Enable Text Customization
  $description: Enables custom disk usage text display. If disabled, Windows default disk text is shown.
- formatString: "%f free | %u used\\n%t Total"
  $name: Text Display Format
  $description: (in any order) %f free | %u used | %t total | %p (used %) | %fp (free %). Legacy format using %s will continue to work as before. Use \n for new line.
- unitGranularity: auto
  $name: Unit Normalization
  $description: Controls how units (GB, TB, etc.) are matched across free, used, and total stats
  $options:
    - auto: Auto (Windows default per stat)
    - match-largest: Match Largest Unit
    - match-smallest: Match Smallest Unit
    - match-total: Match Total Drive Unit
    - gb: Always GB
    - mb: Always MB
    - tb: Always TB
- enableCustomDecimals: false
  $name: Custom Unit Precision
  $description: Enable custom decimal places for disk sizes. Converted values can show more digits, but are estimated from Explorer's rounded sizes.
- decimalPlaces: 2
  $name: Unit Decimal Places
  $description: Converted sizes use 0 to 10 decimal places. Unconverted sizes use no more decimal places than Explorer shows. Values above 10 are capped at 10.
- percentageDecimalPlaces: 2
  $name: Percentage Decimal Places
  $description: Used and free percentages use 0 to 10 decimal places; values above 10 are capped at 10. You can use -1 for automatic formatting. Percentages are estimated from Explorer's rounded sizes.
- boldUsed: true
  $name: Bold Used Space Value
- boldFree: false
  $name: Bold Free Space Value
- boldTotal: false
  $name: Bold Total Space Value
- boldUsedPercent: false
  $name: Bold Used Percentage
- boldFreePercent: false
  $name: Bold Free Percentage
- boldStyle: sans-serif
  $name: Text Style
  $options:
    - serif: Serif
    - sans-serif: Sans-Serif
- removeSpace: false
  $name: Remove Space before Units (100GB/100 GB)
- lineYOffset: 0
  $name: Text Vertical Offset
- lineSpacing: 0
  $name: Line Height
  $description: Adjusts the vertical space between lines of text
- fontSize: 0
  $name: Font Size (Quarter Pixels)
  $description: Adjusts the font size (positive is larger, negative is smaller) in quarter pixels. Ex. 4 = +1px, -2 = -0.5px.
- enableWordEllipsis: false
  $name: Enable Word Ellipsis
  $description: (Adds "..." if text is too long)
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>
#include <windows.h>
#include <gdiplus.h>
#include <shlwapi.h>
#include <uxtheme.h>
#include <windhawk_api.h>
#include <algorithm>
#include <atomic>
#include <cmath>
#include <cwchar>
#include <charconv>
#include <cwctype>
#include <string>
#include <vector>

using namespace Gdiplus;

// --- Global State ---
enum class BoldStyle { Serif, SansSerif };
enum class UnitGranularity {
    Auto,
    MatchLargest,
    MatchSmallest,
    MatchTotal,
    ForceGB,
    ForceMB,
    ForceTB
};

std::wstring g_formatString;
wchar_t g_decimalSeparator = L'.', g_thousandsSeparator = L',';
bool g_boldUsed, g_boldFree, g_boldTotal, g_boldUsedPercent,
    g_boldFreePercent, g_removeSpace, g_showGloss, g_enableWordEllipsis,
    g_roundFillBothSides, g_useAccentColor, g_enableBarCustomization,
    g_enableTextCustomization,
    g_enableCustomDecimals;
int g_lineYOffset, g_accentColorGradientDelta;
int g_barYOffset, g_lineSpacing, g_decimalPlaces, g_percentageDecimalPlaces;
int g_leftInset, g_rightInset, g_topInset, g_bottomInset;
int g_trackLeftInset, g_trackRightInset, g_trackTopInset, g_trackBottomInset;
int g_gradientDirection;
ARGB g_barNormalStart, g_barNormalEnd;
ARGB g_barFullStart, g_barFullEnd, g_trackColor, g_borderColor;
float g_borderThickness, g_trackBorderOffset, g_fillPadding, g_cornerRadius,
    g_fontSize;
BoldStyle g_boldStyle;
UnitGranularity g_unitGranularity = UnitGranularity::Auto;
ULONG_PTR g_gdiplusToken;
std::atomic<bool> g_unloading{false};
std::atomic<unsigned> g_activeBarCalls{0};
std::atomic<bool> g_settingsReloading{false};

class ActiveBarCall {
public:
    ActiveBarCall() { ++g_activeBarCalls; }
    ~ActiveBarCall() { --g_activeBarCalls; }
    ActiveBarCall(const ActiveBarCall&) = delete;
    ActiveBarCall& operator=(const ActiveBarCall&) = delete;
};

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
    if (s.length() != 6 && s.length() != 8)
        return fallback;
    if (s.find_first_not_of(L"0123456789abcdefABCDEF") != std::wstring::npos)
        return fallback;
    try {
        unsigned long val = std::stoul(s, nullptr, 16);
        if (s.length() == 6)
            val |= 0xFF000000;
        return (ARGB)val;
    } catch (...) {
        return fallback;
    }
}

static void GetNormalBarColors(ARGB& start, ARGB& end) {
    start = g_barNormalStart;
    end = g_barNormalEnd;
    if (!g_useAccentColor)
        return;
    DWORD color = 0;
    DWORD size = sizeof(color);
    if (RegGetValueW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\DWM",
                     L"ColorizationColor", RRF_RT_DWORD, nullptr, &color,
                     &size) != ERROR_SUCCESS)
        return;

    color |= 0xFF000000;
    float multiplier = std::clamp(
        1.0f - (float)g_accentColorGradientDelta / 100.0f, 0.0f, 1.0f);
    DWORD r = (DWORD)(((color >> 16) & 0xFF) * multiplier);
    DWORD g = (DWORD)(((color >> 8) & 0xFF) * multiplier);
    DWORD b = (DWORD)((color & 0xFF) * multiplier);
    // Keep the pair local to this paint; other Explorer threads never write
    // the settings or observe a partially updated gradient.
    start = color;
    end = 0xFF000000u | (r << 16) | (g << 8) | b;
}

void LoadSettings() {
    wchar_t separator[8] = {};
    if (GetLocaleInfoEx(LOCALE_NAME_USER_DEFAULT, LOCALE_SDECIMAL,
                        separator, (int)std::size(separator)) > 1)
        g_decimalSeparator = separator[0];
    if (GetLocaleInfoEx(LOCALE_NAME_USER_DEFAULT, LOCALE_STHOUSAND,
                        separator, (int)std::size(separator)) > 1)
        g_thousandsSeparator = separator[0];
    g_enableBarCustomization = Wh_GetIntSetting(L"enableBarCustomization") != 0;
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
    g_borderColor = ParseHexARGB(s, 0x80BBBBBB);
    Wh_FreeStringSetting(s);
    g_gradientDirection = Wh_GetIntSetting(L"gradientDirection");
    g_cornerRadius = (float)Wh_GetIntSetting(L"cornerRadius") / 4.0f;
    g_showGloss = Wh_GetIntSetting(L"showGloss") != 0;
    g_roundFillBothSides = Wh_GetIntSetting(L"roundFillBothSides") != 0;
    g_borderThickness = (float)Wh_GetIntSetting(L"borderThickness") / 4.0f;
    g_trackBorderOffset = (float)Wh_GetIntSetting(L"trackBorderOffset") / 4.0f;
    g_fillPadding = (float)Wh_GetIntSetting(L"fillPadding") / 4.0f;
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
        g_formatString = L"%f free | %u used\n%t Total";
    }
    Wh_FreeStringSetting(s);
    if (g_formatString.size() > 4096)
        g_formatString.resize(4096);
    size_t pos = 0;
    while ((pos = g_formatString.find(L"\\n", pos)) != std::wstring::npos) {
        g_formatString.replace(pos, 2, L"\n");
        pos += 1;
    }

    s = Wh_GetStringSetting(L"unitGranularity");
    if (s) {
        if (wcscmp(s, L"match-largest") == 0)
            g_unitGranularity = UnitGranularity::MatchLargest;
        else if (wcscmp(s, L"match-smallest") == 0)
            g_unitGranularity = UnitGranularity::MatchSmallest;
        else if (wcscmp(s, L"match-total") == 0)
            g_unitGranularity = UnitGranularity::MatchTotal;
        else if (wcscmp(s, L"gb") == 0)
            g_unitGranularity = UnitGranularity::ForceGB;
        else if (wcscmp(s, L"mb") == 0)
            g_unitGranularity = UnitGranularity::ForceMB;
        else if (wcscmp(s, L"tb") == 0)
            g_unitGranularity = UnitGranularity::ForceTB;
        else
            g_unitGranularity = UnitGranularity::Auto;
        Wh_FreeStringSetting(s);
    } else {
        g_unitGranularity = UnitGranularity::Auto;
    }

    g_enableCustomDecimals = Wh_GetIntSetting(L"enableCustomDecimals") != 0;
    g_decimalPlaces = std::clamp(Wh_GetIntSetting(L"decimalPlaces"), 0, 10);
    g_percentageDecimalPlaces =
        std::clamp(Wh_GetIntSetting(L"percentageDecimalPlaces"), -1, 10);
    g_boldUsed = Wh_GetIntSetting(L"boldUsed") != 0;
    g_boldFree = Wh_GetIntSetting(L"boldFree") != 0;
    g_boldTotal = Wh_GetIntSetting(L"boldTotal") != 0;
    g_boldUsedPercent = Wh_GetIntSetting(L"boldUsedPercent") != 0;
    g_boldFreePercent = Wh_GetIntSetting(L"boldFreePercent") != 0;
    g_removeSpace = Wh_GetIntSetting(L"removeSpace") != 0;
    g_enableWordEllipsis = Wh_GetIntSetting(L"enableWordEllipsis") != 0;
    g_enableTextCustomization = Wh_GetIntSetting(L"enableTextCustomization") != 0;
    s = Wh_GetStringSetting(L"boldStyle");
    g_boldStyle = (s && wcscmp(s, L"serif") == 0) ? BoldStyle::Serif
                                                  : BoldStyle::SansSerif;
    Wh_FreeStringSetting(s);
    g_lineYOffset = std::clamp(Wh_GetIntSetting(L"lineYOffset"), -32768, 32767);
    g_barYOffset = Wh_GetIntSetting(L"barYOffset");
    g_lineSpacing = std::clamp(Wh_GetIntSetting(L"lineSpacing"), -4096, 4096);
    g_fontSize = (float)Wh_GetIntSetting(L"fontSize") / 4.0f;
    g_useAccentColor = Wh_GetIntSetting(L"useAccentColor") != 0;
    g_accentColorGradientDelta = Wh_GetIntSetting(L"accentColorGradientDelta");

}

static bool IsSizeSpace(wchar_t c) {
    return c == L' ' || c == 0x00A0 || c == 0x202F;
}

static bool IsSizeNumericChar(wchar_t c) {
    return (c >= L'0' && c <= L'9') || c == L'.' || c == L',' ||
           c == L'\'' || c == 0x2019 || IsSizeSpace(c);
}

// Recognize common units first, then Finnish-style kt/Mt/Gt. Accepting any
// two letters after a size prefix would mistake distances such as Km for bytes.
static std::wstring UpperSizeUnit(const wchar_t* unit) {
    std::wstring result = unit;
    for (auto& c : result) {
        if (c >= L'a' && c <= L'z')
            c -= L'a' - L'A';
        else if (c >= 0x0430 && c <= 0x044F)
            c -= 0x20; // Cyrillic casing without a dependency on CRT locale.
    }
    return result;
}

double GetUnitMultiplier(const wchar_t* unit) {
    // Finnish uses a lowercase t for a byte, and kt for a kilobyte.
    if (wcscmp(unit, L"t") == 0)
        return 1.0;
    const std::wstring up = UpperSizeUnit(unit);
    if (up == L"B" || up == L"O" || up == L"BYTE" || up == L"BYTES" ||
        up == L"\x0411" || up == L"\x0411\x0410\x0419\x0422" ||
        up == L"\x0411\x0410\x0419\x0422\x0410" ||
        up == L"\x0411\x0410\x0419\x0422\x041E\x0412")
        return 1.0;
    static const wchar_t* const units[][3] = {
        {L"KB", L"KO", L"\x041A\x0411"},
        {L"MB", L"MO", L"\x041C\x0411"},
        {L"GB", L"GO", L"\x0413\x0411"},
        {L"TB", L"TO", L"\x0422\x0411"},
        {L"PB", L"PO", L"\x041F\x0411"},
        {L"EB", L"EO", L"\x042D\x0411"},
    };
    double multiplier = 1024.0;
    for (const auto& group : units) {
        for (const auto* name : group)
            if (up == name)
                return multiplier;
        multiplier *= 1024.0;
    }
    if (up.size() == 2 && (up[1] == L'T' || up[1] == L'\x0422')) {
        static const wchar_t* const prefixes = L"KMGTPE";
        static const wchar_t* const cyrillic = L"\x041A\x041C\x0413\x0422\x041F";
        double value = 1024.0;
        for (int i = 0; prefixes[i]; ++i, value *= 1024.0) {
            if (up[0] == prefixes[i] || (i < 5 && up[0] == cyrillic[i]))
                return value;
        }
    }
    return 0.0;
}

bool IsValidUnitString(const wchar_t* unit) {
    return GetUnitMultiplier(unit) > 0.0;
}

// Explorer uses the user's number separators. Parse digits explicitly so a
// different process CRT locale cannot change the value, and EB cannot become
// a floating-point exponent.
static bool ParseSpaceValue(const std::wstring& text, double& value,
                            std::wstring& unit, int* sourceDecimals = nullptr) {
    size_t unitStart = 0;
    while (unitStart < text.size() && IsSizeNumericChar(text[unitStart]))
        ++unitStart;
    unit = text.substr(unitStart);
    if (!IsValidUnitString(unit.c_str()))
        return false;

    bool decimal = false, grouped = false;
    int groupDigits = 0, fractionalDigits = 0;
    double divisor = 1.0;
    value = 0.0;
    for (size_t i = 0; i < unitStart; ++i) {
        wchar_t c = text[i];
        if (c >= L'0' && c <= L'9') {
            if (decimal) {
                divisor *= 10.0;
                value += (c - L'0') / divisor;
                ++fractionalDigits;
            } else {
                value = value * 10.0 + (c - L'0');
                ++groupDigits;
            }
            continue;
        }
        if (IsSizeSpace(c)) {
            size_t next = i;
            while (next < unitStart && IsSizeSpace(text[next]))
                ++next;
            if (next == unitStart)
                break; // The space between the number and the unit.
            i = next - 1;
        }
        if (c == g_decimalSeparator) {
            if (decimal || groupDigits == 0 || (grouped && groupDigits != 3))
                return false;
            decimal = true;
        } else if (c == g_thousandsSeparator || IsSizeSpace(c)) {
            if (decimal || groupDigits == 0 || groupDigits > 3 ||
                (grouped && groupDigits < 2))
                return false;
            grouped = true;
            groupDigits = 0;
        } else {
            return false;
        }
    }
    bool valid = groupDigits > 0 && (!grouped || groupDigits == 3) &&
                 (!decimal || fractionalDigits > 0) && std::isfinite(value);
    if (valid && sourceDecimals)
        *sourceDecimals = fractionalDigits;
    return valid;
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
                wCls.find(L"treeview") != std::wstring::npos ||
                wCls.find(L"toolbar") != std::wstring::npos ||
                wCls.find(L"breadcrumb") != std::wstring::npos ||
                wCls.find(L"address") != std::wstring::npos ||
                (wCls.find(L"property") != std::wstring::npos &&
                 wCls.find(L"propertycontrol") == std::wstring::npos)) {
                return false;
            }
        }
        walk = GetParent(walk);
    }
    return true;
}

thread_local static HDC g_lastBarDC = NULL;
thread_local static RECT g_lastBarRect = {};
thread_local static HTHEME g_lastBarTheme = NULL;

static void BuildRoundedPath(GraphicsPath& path,
                             RectF rect,
                             float radius,
                             bool rL = true,
                             bool rR = true) {
    path.Reset();
    if (rect.Width <= 0.0f || rect.Height <= 0.0f)
        return;
    float d = std::min({std::max(0.0f, radius) * 2.0f,
                        rect.Width, rect.Height});
    radius = d / 2.0f;
    if (d < 1.0f) {
        path.AddRectangle(rect);
        return;
    }

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
                             bool isFill,
                             LPCRECT pTrackRect = nullptr) {
    float scale = 1.0f;
    static auto pGetDpiForWindow = (UINT(WINAPI*)(HWND))GetProcAddress(
        GetModuleHandleW(L"user32.dll"), "GetDpiForWindow");

    HWND hwnd = WindowFromDC(hdc);
    if (pGetDpiForWindow && hwnd) {
        scale = (float)pGetDpiForWindow(hwnd) / 96.0f;
    } else {
        scale = (float)GetDeviceCaps(hdc, LOGPIXELSY) / 96.0f;
    }
    if (scale <= 0.0f)
        scale = 1.0f;

    Graphics graphics{hdc};
    if (pClipRect) {
        graphics.SetClip(Rect(pClipRect->left, pClipRect->top,
                              pClipRect->right - pClipRect->left,
                              pClipRect->bottom - pClipRect->top),
                         CombineModeIntersect);
    }

    graphics.SetSmoothingMode(SmoothingModeAntiAlias);
    graphics.SetPixelOffsetMode(PixelOffsetModeHighQuality);

    RectF barRect{(REAL)pRect->left, (REAL)pRect->top,
                  (REAL)(pRect->right - pRect->left),
                  (REAL)(pRect->bottom - pRect->top)};

    barRect.Y += (float)g_barYOffset;

    // 1. Calculate Track Geometry (The container)
    // Use a compatible track from this thread to round a full fill correctly.
    RectF trackRect;
    if (isFill && pTrackRect) {
        trackRect.X = (float)pTrackRect->left;
        trackRect.Y = (float)pTrackRect->top;
        trackRect.Width = (float)(pTrackRect->right - pTrackRect->left);
        trackRect.Height = (float)(pTrackRect->bottom - pTrackRect->top);
        trackRect.Y += (float)g_barYOffset;
    } else {
        trackRect = barRect;
    }

    trackRect.X += (float)g_trackLeftInset * scale;
    trackRect.Y += (float)g_trackTopInset * scale;
    trackRect.Width -= ((float)g_trackLeftInset + g_trackRightInset) * scale;
    trackRect.Height -= ((float)g_trackTopInset + g_trackBottomInset) * scale;

    if (trackRect.Width <= 0.1f || trackRect.Height <= 0.1f)
        return;

    if (!isFill) {
        // PASS A: Background
        GraphicsPath trackPath;
        BuildRoundedPath(trackPath, trackRect, (float)g_cornerRadius * scale);

        RectF borderRect = trackRect;
        float bOff = g_trackBorderOffset * scale;
        if (bOff != 0)
            borderRect.Inflate(bOff, bOff);
        GraphicsPath borderPath;
        BuildRoundedPath(borderPath, borderRect, (float)g_cornerRadius * scale);

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
        fillRect.Width -= ((float)g_leftInset + g_rightInset) * scale;
        fillRect.Height -= ((float)g_topInset + g_bottomInset) * scale;

        float fPad = g_fillPadding * scale;
        if (fPad != 0) {
            fillRect.Inflate(-fPad, -fPad);
        }

        if (fillRect.Width > 0.1f && fillRect.Height > 0.1f) {
            bool rR = g_roundFillBothSides;
            if (!rR) {
                // If fill reaches roughly the right side of the track, round it
                // too
                if (pTrackRect &&
                    pRect->right >= pTrackRect->right - (int)(1 * scale)) {
                    rR = true;
                }
            }

            GraphicsPath fillPath;
            BuildRoundedPath(fillPath, fillRect, (float)g_cornerRadius * scale,
                             true, rR);

            ARGB c1 = g_barFullStart, c2 = g_barFullEnd;
            if (iStateId != 2)
                GetNormalBarColors(c1, c2);
            RectF gradRect = fillRect;
            gradRect.Inflate(0.5f, 0.5f);
            LinearGradientBrush br{gradRect, Color{c1}, Color{c2},
                                   (REAL)g_gradientDirection};
            br.SetWrapMode(WrapModeTileFlipXY);
            graphics.FillPath(&br, &fillPath);

            if (g_showGloss) {
                graphics.SetClip(&fillPath, CombineModeIntersect);
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
                      LPCRECT pRect) {
    if (!hTheme || !hdc || !pRect || pRect->right <= pRect->left ||
        pRect->bottom <= pRect->top) {
        return false;
    }
    if (iPartId != 1 && iPartId != 5 && iPartId != 11) {
        return false;
    }

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

    if (scale <= 0.0f)
        scale = 1.0f;

    // Logical Dimensioning
    double h = (double)pRect->bottom - pRect->top;
    double w = (double)pRect->right - pRect->left;

    // Normalize physical pixels to logical bounds
    double logicalH = h / scale;
    double logicalW = w / scale;

    if (iPartId == 5) {
        if (logicalH < 2.0f || logicalH > 16.5f) {
            return false;
        }
    } else {
        if (logicalW < 30.0f) {
            return false;
        }
        if (logicalH < 4.0f || logicalH > 16.5f) {
            return false;
        }
    }

    if (GetThemeClassList_Ptr) {
        wchar_t themeCls[256] = {};
        if (SUCCEEDED(GetThemeClassList_Ptr(hTheme, themeCls, 256))) {
            std::wstring tCls(themeCls);
            std::transform(tCls.begin(), tCls.end(), tCls.begin(), ::towlower);
            if (tCls.find(L"progress") == std::wstring::npos) {
                return false;
            }
            if (tCls.find(L"scrollbar") != std::wstring::npos ||
                tCls.find(L"header") != std::wstring::npos) {
                return false;
            }
        }
    }

    bool isPropertyControl = false;
    if (hwnd) {
        HWND walk = hwnd;
        int limit = 15;
        while (walk && limit-- > 0) {
            wchar_t cls[MAX_PATH];
            if (GetClassNameW(walk, cls, MAX_PATH)) {
                std::wstring wCls(cls);
                std::transform(wCls.begin(), wCls.end(), wCls.begin(),
                               ::towlower);

                if (wCls.find(L"propertycontrol") != std::wstring::npos) {
                    isPropertyControl = true;
                }

                if (wCls == L"#32770" || wCls == L"msctls_progress32" ||
                    wCls.find(L"scrollbar") != std::wstring::npos ||
                    wCls.find(L"header") != std::wstring::npos ||
                    wCls.find(L"listview") != std::wstring::npos ||
                    wCls.find(L"treeview") != std::wstring::npos ||
                    wCls.find(L"toolbar") != std::wstring::npos ||
                    wCls.find(L"breadcrumb") != std::wstring::npos ||
                    wCls.find(L"address") != std::wstring::npos ||
                    (wCls.find(L"property") != std::wstring::npos &&
                     wCls.find(L"propertycontrol") == std::wstring::npos)) {
                    return false;
                }
            }
            walk = GetParent(walk);
        }
    }

    // prevent navpane being styled
    if (!isPropertyControl && pRect->left < (int)(32 * scale)) {
        return false;
    }
    return true;
}

// A fill may use only the immediately preceding compatible track on this thread.
static bool HasMatchingTrack(HTHEME theme, HDC dc, LPCRECT fill) {
    return dc == g_lastBarDC && theme == g_lastBarTheme &&
           g_lastBarRect.right > g_lastBarRect.left &&
           fill->left >= g_lastBarRect.left &&
           fill->right <= g_lastBarRect.right &&
           fill->top >= g_lastBarRect.top &&
           fill->bottom <= g_lastBarRect.bottom;
}

HRESULT WINAPI HookedDrawThemeBackground(HTHEME hTheme,
                                         HDC hdc,
                                         int iPartId,
                                         int iStateId,
                                         LPCRECT pRect,
                                         LPCRECT pClipRect) {
    // Increment before checking the stop flag. Cleanup sets the flag first,
    // then waits for all calls that could have entered GDI+ to finish.
    {
        ActiveBarCall active;
        try {
            if (!g_unloading.load() && IsDiskBar(hTheme, hdc, iPartId, pRect)) {
                if (iPartId == 5) {
                    RECT track = g_lastBarRect;
                    bool matching = HasMatchingTrack(hTheme, hdc, pRect);
                    g_lastBarDC = nullptr;
                    g_lastBarTheme = nullptr;
                    PaintEnhancedBar(hdc, pRect, pClipRect, iStateId, true,
                                     matching ? &track : nullptr);
                } else {
                    // Every paint must redraw the track, including a repaint at
                    // exactly the same coordinates on a reused DC.
                    g_lastBarDC = hdc;
                    g_lastBarTheme = hTheme;
                    g_lastBarRect = *pRect;
                    PaintEnhancedBar(hdc, pRect, pClipRect, iStateId, false);
                }
                return S_OK;
            }
        } catch (...) {
            Wh_Log(L"Bar customization failed; using the original drawing function");
        }
    }
    g_lastBarDC = nullptr;
    g_lastBarTheme = nullptr;
    return DrawThemeBackground_Orig(hTheme, hdc, iPartId, iStateId, pRect,
                                    pClipRect);
}

std::wstring GetLocalizedUnitName(double multiplier, const wchar_t* sampleUnit) {
    std::wstring up = UpperSizeUnit(sampleUnit ? sampleUnit : L"");

    bool isFrench = (up.find(L"O") != std::wstring::npos &&
                     up.find(L"BYTE") == std::wstring::npos &&
                     up.find(L"B") == std::wstring::npos);
    bool isCyrillic = (up.find(L"\x0411") != std::wstring::npos);

    // Preserve recognized Finnish and Cyrillic-T suffixes during conversion.
    bool finnishByte = sampleUnit && wcscmp(sampleUnit, L"t") == 0;
    if (finnishByte ||
        (up.size() == 2 && (up[1] == L'T' || up[1] == L'\x0422') &&
         GetUnitMultiplier(sampleUnit) > 0.0)) {
        const wchar_t* prefixes = L"KMGTPE";
        const wchar_t* cyrillic = L"\x041A\x041C\x0413\x0422\x041F\x042D";
        bool useCyrillic = up[0] == L'\x041A' || up[0] == L'\x041C' ||
                           up[0] == L'\x0413' || up[0] == L'\x0422' ||
                           up[0] == L'\x041F';
        wchar_t suffix = finnishByte ? L't' : sampleUnit[1];
        if (multiplier == 1.0 && suffix == L't' && !useCyrillic)
            return L"t";
        for (int i = 0; prefixes[i]; ++i) {
            double target = 1024.0;
            for (int j = 0; j < i; ++j)
                target *= 1024.0;
            if (multiplier == target) {
                wchar_t prefix = useCyrillic ? cyrillic[i] : prefixes[i];
                if (!useCyrillic && suffix == L't' && i == 0)
                    prefix = L'k';
                return std::wstring(1, prefix) + suffix;
            }
        }
    }

    if (multiplier >= 1152921504606846976.0) {  // EB
        if (isFrench)
            return L"Eo";
        if (isCyrillic)
            return L"\x042D\x0411";
        return L"EB";
    }
    if (multiplier >= 1125899906842624.0) {  // PB
        if (isFrench)
            return L"Po";
        if (isCyrillic)
            return L"\x041F\x0411";
        return L"PB";
    }
    if (multiplier >= 1099511627776.0) {  // TB
        if (isFrench)
            return L"To";
        if (isCyrillic)
            return L"\x0422\x0411";
        return L"TB";
    }
    if (multiplier >= 1073741824.0) {  // GB
        if (isFrench)
            return L"Go";
        if (isCyrillic)
            return L"\x0413\x0411";
        return L"GB";
    }
    if (multiplier >= 1048576.0) {  // MB
        if (isFrench)
            return L"Mo";
        if (isCyrillic)
            return L"\x041C\x0411";
        return L"MB";
    }
    if (multiplier >= 1024.0) {  // KB
        if (isFrench)
            return L"Ko";
        if (isCyrillic)
            return L"\x041A\x0411";
        return L"KB";
    }
    if (isFrench)
        return L"o";
    if (isCyrillic)
        return L"\x0411";
    return L"B";
}

double GetDisplayUnitMultiplier(double bytes) {
    if (bytes >= 1152921504606846976.0)
        return 1152921504606846976.0;
    if (bytes >= 1125899906842624.0)
        return 1125899906842624.0;
    if (bytes >= 1099511627776.0)
        return 1099511627776.0;
    if (bytes >= 1073741824.0)
        return 1073741824.0;
    if (bytes >= 1048576.0)
        return 1048576.0;
    if (bytes >= 1024.0)
        return 1024.0;
    return 1.0;
}

static std::wstring FormatFixedNumber(double value, int decimals,
                                      bool trimZeros) {
    char buffer[64];
    auto formatted = std::to_chars(std::begin(buffer), std::end(buffer), value,
                                    std::chars_format::fixed,
                                    std::clamp(decimals, 0, 10));
    if (formatted.ec != std::errc{})
        return L"0";
    std::string number(buffer, formatted.ptr);
    if (trimZeros && number.find('.') != std::string::npos) {
        while (!number.empty() && number.back() == '0')
            number.pop_back();
        if (!number.empty() && number.back() == '.')
            number.pop_back();
    }
    std::wstring result(number.begin(), number.end());
    std::replace(result.begin(), result.end(), L'.', g_decimalSeparator);
    return result;
}

std::wstring FormatValueWithDecimals(double value, int decimals) {
    if (decimals >= 0)
        return FormatFixedNumber(value, decimals, false);
    // Match Explorer's precision for typical values. Keep small converted
    // fractions visible without implying more than roughly two useful digits.
    int precision = value >= 100.0 ? 0 : value >= 10.0 ? 1 : 2;
    if (value > 0.0 && value < 0.01)
        precision = std::clamp(1 - (int)std::floor(std::log10(value)), 2, 10);
    return FormatFixedNumber(value, precision, true);
}

std::wstring FormatPercentage(double percentage, int decimals) {
    if (decimals >= 0)
        return FormatFixedNumber(percentage, decimals, false) + L"%";
    if (percentage > 0.0 && percentage < 1.0)
        return FormatFixedNumber(percentage, 1, false) + L"%";
    return FormatFixedNumber(std::round(percentage), 0, false) + L"%";
}

std::wstring ApplyPlaceholders(const std::wstring& fmt,
                               const std::wstring& freeStr,
                               const std::wstring& usedStr,
                               const std::wstring& totalStr,
                               const std::wstring& usedPctStr,
                               const std::wstring& freePctStr) {
    std::wstring result;
    const std::wstring displayedFree =
        g_boldFree ? MakeBoldText(freeStr) : freeStr;
    const std::wstring displayedUsed =
        g_boldUsed ? MakeBoldText(usedStr) : usedStr;
    const std::wstring displayedTotal =
        g_boldTotal ? MakeBoldText(totalStr) : totalStr;
    const std::wstring displayedUsedPct =
        g_boldUsedPercent ? MakeBoldText(usedPctStr) : usedPctStr;
    const std::wstring displayedFreePct =
        g_boldFreePercent ? MakeBoldText(freePctStr) : freePctStr;
    int seqIndex = 0;
    size_t i = 0;
    while (i < fmt.length()) {
        if (fmt[i] == L'%' && i + 1 < fmt.length()) {
            wchar_t next = towlower(fmt[i + 1]);
            if (next == L'f') {
                if (i + 2 < fmt.length() && towlower(fmt[i + 2]) == L'p') {
                    result += displayedFreePct;
                    i += 3;
                    continue;
                } else {
                    result += displayedFree;
                    i += 2;
                    continue;
                }
            } else if (next == L'u') {
                result += displayedUsed;
                i += 2;
                continue;
            } else if (next == L't') {
                result += displayedTotal;
                i += 2;
                continue;
            } else if (next == L'p') {
                result += displayedUsedPct;
                i += 2;
                continue;
            } else if (next == L's') {
                if (seqIndex == 0) {
                    result += displayedFree;
                } else if (seqIndex == 1) {
                    result += displayedUsed;
                } else if (seqIndex == 2) {
                    result += displayedTotal;
                }
                seqIndex++;
                i += 2;
                continue;
            } else if (fmt[i + 1] == L'%') {
                result += L'%';
                i += 2;
                continue;
            }
        }
        result += fmt[i];
        i++;
    }
    return result;
}

bool FindSpaceStats(const std::wstring& t, std::wstring& f, std::wstring& tot) {
    std::wstring nt = t;
    for (auto& c : nt) {
        if (IsSizeSpace(c))
            c = L' ';
    }

    size_t num1_start = nt.find_first_of(L"0123456789");
    if (num1_start == std::wstring::npos)
        return false;

    size_t pos = num1_start;
    while (pos < nt.length() &&
           IsSizeNumericChar(nt[pos]))
        pos++;

    size_t unit1_start = pos;
    while (pos < nt.length() && !iswdigit(nt[pos]) && nt[pos] != L' ')
        pos++;
    size_t size1_end = pos;

    std::wstring u1 = nt.substr(unit1_start, size1_end - unit1_start);
    if (!IsValidUnitString(u1.c_str()))
        return false;

    size_t num2_start = nt.find_first_of(L"0123456789", size1_end);
    if (num2_start == std::wstring::npos)
        return false;

    if (num2_start == size1_end)
        return false;

    pos = num2_start;
    while (pos < nt.length() &&
           IsSizeNumericChar(nt[pos]))
        pos++;

    size_t unit2_start = pos;
    while (pos < nt.length() && !iswdigit(nt[pos]) && nt[pos] != L' ')
        pos++;
    size_t size2_end = pos;

    std::wstring u2 = nt.substr(unit2_start, size2_end - unit2_start);
    if (!IsValidUnitString(u2.c_str()))
        return false;

    // Reject if there is another number after the second unit
    if (nt.find_first_of(L"0123456789", size2_end) != std::wstring::npos) {
        return false;
    }

    f = nt.substr(num1_start, size1_end - num1_start);
    tot = nt.substr(num2_start, size2_end - num2_start);

    while (!f.empty() && f.back() == L' ')
        f.pop_back();
    while (!tot.empty() && tot.back() == L' ')
        tot.pop_back();

    return true;
}

bool ProcessDiskUsageText(HDC hdc,
                          LPCWSTR psz,
                          int cch,
                          std::wstring& outCustomText) {
    if (!psz)
        return false;
    if (cch < -1)
        return false;
    // A disk detail label is short. Bound work on every Explorer text call.
    constexpr int kMaxDiskText = 512;
    int len = cch;
    if (cch == -1) {
        len = 0;
        while (len <= kMaxDiskText && psz[len] != L'\0')
            ++len;
    }
    if (len <= 0 || len > kMaxDiskText)
        return false;
    bool hasNum = false;
    for (int i = 0; i < len; ++i) {
        if (psz[i] >= L'0' && psz[i] <= L'9') {
            hasNum = true;
            break;
        }
    }
    if (!hasNum)
        return false;

    if (hdc) {
        HWND hwnd = WindowFromDC(hdc);
        if (!IsValidDiskBarWindow(hwnd)) {
            return false;
        }
    }

    std::wstring t(psz, len);
    std::wstring fs, ts;
    if (!FindSpaceStats(t, fs, ts)) {
        return false;
    }

    double fv = 0.0, tv = 0.0;
    int freeDecimals = 0, totalDecimals = 0;
    std::wstring fu, tu;
    if (!ParseSpaceValue(fs, fv, fu, &freeDecimals) ||
        !ParseSpaceValue(ts, tv, tu, &totalDecimals))
        return false;

    double um1 = GetUnitMultiplier(fu.c_str());
    double um2 = GetUnitMultiplier(tu.c_str());

    double freeBytes = fv * um1;
    double totalBytes = tv * um2;
    if (!std::isfinite(freeBytes) || !std::isfinite(totalBytes) ||
        freeBytes < 0.0 || totalBytes <= 0.0 ||
        freeBytes >= 9223372036854775808.0 ||
        totalBytes >= 9223372036854775808.0) {
        return false;
    }
    double usedBytes = std::max(0.0, totalBytes - freeBytes);

    double usedPct =
        (totalBytes > 0.0) ? (usedBytes / totalBytes) * 100.0 : 0.0;
    double freePct =
        std::clamp((freeBytes / totalBytes) * 100.0, 0.0, 100.0);

    int effectiveDecimals = g_enableCustomDecimals ? g_decimalPlaces : -1;
    int usedSourceDecimals = std::max(freeDecimals, totalDecimals);
    auto unitDecimals = [effectiveDecimals](bool converted, int sourceDecimals) {
        if (converted)
            return effectiveDecimals;
        return effectiveDecimals < 0 ? sourceDecimals
                                     : std::min(effectiveDecimals, sourceDecimals);
    };

    std::wstring usedPctStr =
        FormatPercentage(usedPct, g_percentageDecimalPlaces);
    std::wstring freePctStr =
        FormatPercentage(freePct, g_percentageDecimalPlaces);

    std::wstring outFree, outUsed, outTotal;

    if (g_unitGranularity == UnitGranularity::Auto) {
        if (g_enableCustomDecimals) {
            double usedMult = GetDisplayUnitMultiplier(usedBytes);
            outFree = FormatValueWithDecimals(
                          fv, unitDecimals(false, freeDecimals)) + L" " +
                      GetLocalizedUnitName(um1, fu.c_str());
            outTotal = FormatValueWithDecimals(
                           tv, unitDecimals(false, totalDecimals)) + L" " +
                       GetLocalizedUnitName(um2, tu.c_str());
            bool usedConverted = um2 != usedMult ||
                                 (freeBytes > 0.0 && um1 != usedMult);
            outUsed = FormatValueWithDecimals(usedBytes / usedMult,
                                              unitDecimals(usedConverted,
                                                           usedSourceDecimals)) +
                      L" " +
                      GetLocalizedUnitName(usedMult, tu.c_str());
        } else {
            outFree = fs;
            outTotal = ts;
            wchar_t buf[64] = {};
            if (!StrFormatByteSizeW((LONGLONG)usedBytes, buf, (UINT)std::size(buf)))
                return false;
            outUsed = buf;
        }
    } else {
        double targetMult = um2;
        if (g_unitGranularity == UnitGranularity::MatchLargest) {
            targetMult = std::max({um1, um2,
                                   usedBytes > 0.0
                                       ? GetDisplayUnitMultiplier(usedBytes)
                                       : 1.0});
        } else if (g_unitGranularity == UnitGranularity::MatchSmallest) {
            targetMult = std::min(um1, um2);
            if (usedBytes > 0.0)
                targetMult = std::min(targetMult,
                                      GetDisplayUnitMultiplier(usedBytes));
        } else if (g_unitGranularity == UnitGranularity::MatchTotal) {
            targetMult = um2;
        } else if (g_unitGranularity == UnitGranularity::ForceGB) {
            targetMult = 1073741824.0;
        } else if (g_unitGranularity == UnitGranularity::ForceMB) {
            targetMult = 1048576.0;
        } else if (g_unitGranularity == UnitGranularity::ForceTB) {
            targetMult = 1099511627776.0;
        }

        std::wstring unitName = GetLocalizedUnitName(targetMult, tu.c_str());
        std::wstring sep = g_removeSpace ? L"" : L" ";

        outFree = FormatValueWithDecimals(freeBytes / targetMult,
                                          unitDecimals(um1 != targetMult, freeDecimals)) +
                  sep + unitName;
        // When free space is zero, used space is the displayed total itself.
        // A zero in another unit does not make that value a conversion.
        bool usedConverted = um2 != targetMult ||
                             (freeBytes > 0.0 && um1 != targetMult);
        outUsed = FormatValueWithDecimals(usedBytes / targetMult,
                                          unitDecimals(usedConverted,
                                                       usedSourceDecimals)) +
                  sep + unitName;
        outTotal = FormatValueWithDecimals(totalBytes / targetMult,
                                           unitDecimals(um2 != targetMult, totalDecimals)) +
                   sep + unitName;
    }

    if (g_removeSpace) {
        outFree.erase(std::remove(outFree.begin(), outFree.end(), L' '),
                      outFree.end());
        outUsed.erase(std::remove(outUsed.begin(), outUsed.end(), L' '),
                      outUsed.end());
        outTotal.erase(std::remove(outTotal.begin(), outTotal.end(), L' '),
                       outTotal.end());
    }

    outCustomText = ApplyPlaceholders(g_formatString, outFree, outUsed,
                                      outTotal, usedPctStr, freePctStr);
    return true;
}

// Font selection is restored on every exit, including allocation failures.
class AdjustedDiskFont {
public:
    explicit AdjustedDiskFont(HDC dc) : dc_(dc) {
        if (g_fontSize == 0.0f)
            return;
        LOGFONTW lf{};
        auto current = GetCurrentObject(dc, OBJ_FONT);
        if (!current || GetObjectW(current, sizeof(lf), &lf) != sizeof(lf))
            return;
        double scale = (double)GetDeviceCaps(dc, LOGPIXELSY) / 96.0;
        if (scale <= 0.0)
            scale = 1.0;
        double magnitude = std::clamp(
            std::abs((double)lf.lfHeight) + std::round(g_fontSize * scale),
            1.0, 4096.0);
        lf.lfHeight = (LONG)(lf.lfHeight < 0 ? -magnitude : magnitude);
        font_ = CreateFontIndirectW(&lf);
        if (!font_)
            return;
        old_ = SelectObject(dc, font_);
        if (!old_ || old_ == HGDI_ERROR) {
            DeleteObject(font_);
            font_ = nullptr;
            old_ = nullptr;
        }
    }
    ~AdjustedDiskFont() {
        if (font_) {
            SelectObject(dc_, old_);
            DeleteObject(font_);
        }
    }
    AdjustedDiskFont(const AdjustedDiskFont&) = delete;
    AdjustedDiskFont& operator=(const AdjustedDiskFont&) = delete;
private:
    HDC dc_;
    HFONT font_ = nullptr;
    HGDIOBJ old_ = nullptr;
};

static int RenderFormattedDiskText(HDC hdc,
                                   const std::wstring& ft,
                                   LPRECT prc,
                                   UINT fmt,
                                   LPDRAWTEXTPARAMS pDtp,
                                   bool isEx) {
    AdjustedDiskFont font(hdc);
    const bool measureOnly = (fmt & DT_CALCRECT) != 0;
    // Caller-owned mutable strings are handled by the original hook path.
    // Internal strings must never be modified by DrawText's ellipsis logic.
    UINT flags = (fmt | DT_NOPREFIX) & ~DT_MODIFYSTRING;
    if (g_enableWordEllipsis)
        flags |= DT_WORD_ELLIPSIS | DT_END_ELLIPSIS;
    else
        flags &= ~(DT_END_ELLIPSIS | DT_PATH_ELLIPSIS | DT_WORD_ELLIPSIS);

    auto draw = [&](const std::wstring& text, RECT& rect, UINT drawFlags,
                    LPDRAWTEXTPARAMS params) {
        if (isEx) {
            std::vector<wchar_t> buffer(text.begin(), text.end());
            buffer.push_back(L'\0');
            return DrawTextExW_Orig(hdc, buffer.data(), (int)text.size(),
                                    &rect, drawFlags, params);
        }
        return DrawTextW_Orig(hdc, text.c_str(), (int)text.size(),
                              &rect, drawFlags);
    };

    if (ft.find(L'\n') == std::wstring::npos) {
        RECT rect = *prc;
        if (!measureOnly)
            OffsetRect(&rect, 0, g_lineYOffset);
        int result = draw(ft, rect, flags, pDtp);
        if (measureOnly)
            *prc = rect;
        return result;
    }

    flags &= ~(DT_VCENTER | DT_BOTTOM | DT_WORDBREAK | DT_CALCRECT);
    flags |= DT_TOP | DT_SINGLELINE | DT_NOCLIP;
    struct Line {
        std::wstring text;
        int top;
        int height;
    };
    std::vector<Line> lines;
    int totalHeight = 0;
    LONG maxWidth = 0;
    size_t start = 0;
    while (true) {
        size_t end = ft.find(L'\n', start);
        std::wstring text = ft.substr(start, end == std::wstring::npos
                                              ? end : end - start);
        RECT rect{prc->left, 0, prc->right, 0};
        DRAWTEXTPARAMS params{};
        if (pDtp)
            params = *pDtp;
        // Blank lines still occupy one line in the selected font.
        int measured = draw(text.empty() ? L" " : text, rect,
                            flags | DT_CALCRECT, pDtp ? &params : nullptr);
        if (measured <= 0)
            return 0;
        int height = std::max<LONG>(1, rect.bottom - rect.top);
        maxWidth = std::max(maxWidth, rect.right - rect.left);
        lines.push_back({std::move(text), totalHeight, height});
        if (end == std::wstring::npos) {
            totalHeight += height;
            break;
        }
        totalHeight += std::max(1, height + g_lineSpacing);
        start = end + 1;
    }

    if (measureOnly) {
        prc->right = prc->left + maxWidth;
        prc->bottom = prc->top + totalHeight;
    } else {
        for (const auto& line : lines) {
            RECT rect{prc->left, prc->top, prc->right,
                      prc->top + line.height};
            OffsetRect(&rect, 0, g_lineYOffset + line.top);
            DRAWTEXTPARAMS params{};
            if (pDtp)
                params = *pDtp;
            if (!line.text.empty())
                draw(line.text, rect, flags, pDtp ? &params : nullptr);
        }
    }
    return totalHeight;
}

thread_local static bool g_insideTextHook = false;
class TextHookScope {
public:
    TextHookScope() { g_insideTextHook = true; }
    ~TextHookScope() { g_insideTextHook = false; }
    TextHookScope(const TextHookScope&) = delete;
    TextHookScope& operator=(const TextHookScope&) = delete;
};

int WINAPI DrawTextW_Hook(HDC hdc,
                          LPCWSTR psz,
                          int cch,
                          LPRECT prc,
                          UINT fmt) {
    if (!hdc || !psz || !prc ||
        g_insideTextHook || g_unloading.load() || (fmt & DT_MODIFYSTRING))
        return DrawTextW_Orig(hdc, psz, cch, prc, fmt);

    TextHookScope scope;
    try {
        std::wstring customText;
        if (ProcessDiskUsageText(hdc, psz, cch, customText))
            return RenderFormattedDiskText(hdc, customText, prc, fmt,
                                            nullptr, false);
    } catch (...) {
        Wh_Log(L"Text customization failed; using the original drawing function");
    }
    return DrawTextW_Orig(hdc, psz, cch, prc, fmt);
}

int WINAPI DrawTextExW_Hook(HDC hdc,
                            LPWSTR psz,
                            int cch,
                            LPRECT prc,
                            UINT fmt,
                            LPDRAWTEXTPARAMS pDtp) {
    if (!hdc || !psz || !prc ||
        g_insideTextHook || g_unloading.load() || (fmt & DT_MODIFYSTRING) ||
        (pDtp && pDtp->cbSize != sizeof(*pDtp)))
        return DrawTextExW_Orig(hdc, psz, cch, prc, fmt, pDtp);

    TextHookScope scope;
    try {
        std::wstring customText;
        if (ProcessDiskUsageText(hdc, psz, cch, customText)) {
            int result = RenderFormattedDiskText(hdc, customText, prc, fmt,
                                                 pDtp, true);
            if (pDtp && result > 0)
                pDtp->uiLengthDrawn = cch == -1 ? (UINT)wcslen(psz) : (UINT)cch;
            return result;
        }
    } catch (...) {
        Wh_Log(L"Text customization failed; using the original drawing function");
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
    EnumWindows(RefreshExplorerCallback, 0);
}

static void ShutdownGdiPlus() {
    if (g_gdiplusToken) {
        GdiplusShutdown(g_gdiplusToken);
        g_gdiplusToken = 0;
    }
}

BOOL Wh_ModInit() {
    try {
        LoadSettings();
        if (g_enableBarCustomization) {
            GdiplusStartupInput gsi;
            if (GdiplusStartup(&g_gdiplusToken, &gsi, nullptr) != Ok) {
                g_gdiplusToken = 0;
                Wh_Log(L"GDI+ initialization failed");
                return FALSE;
            }
            HMODULE uxtheme = GetModuleHandleW(L"uxtheme.dll");
            if (uxtheme) {
                GetThemeWindow_Ptr = (GetThemeWindow_t)GetProcAddress(
                    uxtheme, "GetThemeWindow");
                GetThemeClassList_Ptr = (GetThemeClassList_t)GetProcAddress(
                    uxtheme, "GetThemeClassList");
            }
            // Referencing the imported function also ensures uxtheme stays
            // loaded throughout the lifetime of this mod.
            if (!WindhawkUtils::SetFunctionHook(DrawThemeBackground,
                                    HookedDrawThemeBackground,
                                    &DrawThemeBackground_Orig)) {
                Wh_Log(L"Failed to hook DrawThemeBackground");
                ShutdownGdiPlus();
                return FALSE;
            }
        }
        if (g_enableTextCustomization) {
            if (!WindhawkUtils::SetFunctionHook(DrawTextW, DrawTextW_Hook,
                                    &DrawTextW_Orig) ||
                !WindhawkUtils::SetFunctionHook(DrawTextExW, DrawTextExW_Hook,
                                    &DrawTextExW_Orig)) {
                Wh_Log(L"Failed to hook disk text drawing");
                ShutdownGdiPlus();
                return FALSE;
            }
        }
        return TRUE;
    } catch (...) {
        // A failed Wh_ModInit is not followed by Wh_ModUninit.
        ShutdownGdiPlus();
        return FALSE;
    }
}

void Wh_ModAfterInit() {
    RefreshExplorer();
}

void Wh_ModBeforeUninit() {
    g_unloading.store(true);
}

void Wh_ModUninit() {
    // Hooks have been disabled, but a paint already inside our hook may still
    // be running. Windhawk's DLL stack drain happens AFTER this callback.
    while (g_activeBarCalls.load() != 0)
        Sleep(1);
    ShutdownGdiPlus();
    // The new instance refreshes once in Wh_ModAfterInit. On a full disable,
    // there is no new instance, so restore the default view here.
    if (!g_settingsReloading.load())
        RefreshExplorer();
}

BOOL Wh_ModSettingsChanged(BOOL* bReload) {
    // Settings remain immutable while hooks run; Windhawk performs a normal
    // unload/reload to apply changes.
    g_settingsReloading.store(true);
    *bReload = TRUE;
    return TRUE;
}
