// ==WindhawkMod==
// @id              drive-space-customizer
// @name            Drive Space Customizer
// @description     Full customization of This PC drive space labels (order, colors, bold, brackets, custom text)
// @version         1.0.0
// @author          iMAboud
// @github          https://github.com/iMAboud
// @include         explorer.exe
// @compilerOptions -lgdi32 -luxtheme -lshlwapi
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Drive Space Label Customizer

Customizes the storage space label under drives in **This PC** (`X GB free of Y GB`).

![Preview](https://i.imgur.com/nFk8U8m.jpeg)

## Features & Settings
- **Display Order & Separator:** Reorder `used`, `free`, and `total` segments and set a custom divider (default: ` | `).
- **Per-Segment Controls (Used / Free / Total):**
  - **Toggle:** Show or hide any segment.
  - **Custom Format:** Customize text and brackets using `{val}` (size) and `{pct}` (percentage) — e.g., `{val} free`, `{val} used`, `({val})`.
  - **Color Modes:** Default theme color, Dynamic Used % (Low = Green, High = Red), Dynamic Free % (High = Green, Low = Red), or Custom Static Hex.
  - **Bold Text:** Toggle bold font weight per segment.
- **Dynamic Thresholds:** Customize the 3 percentage breakpoints (`t1`, `t2`, `t3`) and Hex colors for **Green**, **Yellow**, **Orange**, and **Red**.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- order: "free,used,total"
  $name: Display Order
  $description: "Comma-separated order of segments using: used, free, total (e.g. free,used,total)"
- separator: " | "
  $name: Separator
  $description: "Text placed between segments (e.g. ' • ', ' | ', ' / ', ' ')"

- usedSection:
  - show: true
    $name: Enable Used Storage
  - format: "{val}"
    $name: Format
    $description: "Placeholders: {val} = size (e.g. 628 GB), {pct} = percentage (e.g. 67%). Examples: '{val}', '{val} ({pct})', '[{val} Used]'"
  - colorMode: "default"
    $name: Color Mode
    $options:
      - default: Default Theme Color
      - dynamic_used: Dynamic Thresholds (Low Used% = Green, High = Red)
      - dynamic_free: Dynamic Inverse (High Free% = Green, Low = Red)
      - static: Custom Static Color
  - staticColor: "#4ADE80"
    $name: Custom Static Color (Hex)
  - bold: false
    $name: Bold Text
  $name: Used Space Settings

- freeSection:
  - show: true
    $name: Enable Free Storage
  - format: "{val} free"
    $name: Format
    $description: "Placeholders: {val} = size (e.g. 302 GB), {pct} = percentage (e.g. 33%). Examples: '{val} free', '{val} ({pct} free)'"
  - colorMode: "dynamic_used"
    $name: Color Mode
    $options:
      - default: Default Theme Color
      - dynamic_used: Dynamic Thresholds (Low Used% = Green, High = Red)
      - dynamic_free: Dynamic Inverse (High Free% = Green, Low = Red)
      - static: Custom Static Color
  - staticColor: "#4ADE80"
    $name: Custom Static Color (Hex)
  - bold: true
    $name: Bold Text
  $name: Free Space Settings

- totalSection:
  - show: true
    $name: Enable Total Storage
  - format: "({val})"
    $name: Format
    $description: "Placeholders: {val} = size (e.g. 930 GB). Examples: '({val} Total)', '({val})', 'of {val}'"
  - colorMode: "default"
    $name: Color Mode
    $options:
      - default: Default Theme Color
      - dynamic_used: Dynamic Thresholds (Low Used% = Green, High = Red)
      - dynamic_free: Dynamic Inverse (High Free% = Green, Low = Red)
      - static: Custom Static Color
  - staticColor: "#9CA3AF"
    $name: Custom Static Color (Hex)
  - bold: false
    $name: Bold Text
  $name: Total Space Settings

- thresholds:
  - t1: 31
    $name: Threshold 1 (%) - Green below this
  - t2: 51
    $name: Threshold 2 (%) - Yellow below this
  - t3: 71
    $name: Threshold 3 (%) - Orange below this (Red at or above)
  - colorGreen: "#4ADE80"
    $name: Green Color (Hex)
  - colorYellow: "#FACC15"
    $name: Yellow Color (Hex)
  - colorOrange: "#FB923C"
    $name: Orange Color (Hex)
  - colorRed: "#F87171"
    $name: Red Color (Hex)
  $name: Dynamic Thresholds & Colors
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <uxtheme.h>
#include <vssym32.h>
#include <shlwapi.h>
#include <stdio.h>
#include <wchar.h>
#include <algorithm>
#include <string>
#include <vector>

enum ColorMode {
    COLOR_DEFAULT = 0,
    COLOR_DYNAMIC_USED,
    COLOR_DYNAMIC_FREE,
    COLOR_STATIC
};

struct SegmentSetting {
    bool show;
    std::wstring format;
    ColorMode colorMode;
    COLORREF staticColor;
    bool bold;
};

struct ModSettings {
    std::wstring order;
    std::wstring separator;
    SegmentSetting used;
    SegmentSetting free;
    SegmentSetting total;
    int t1, t2, t3;
    COLORREF cGreen, cYellow, cOrange, cRed;
} g_settings;

struct RenderChunk {
    std::wstring text;
    bool useCustomColor;
    COLORREF color;
    bool bold;
};

struct UnitDef {
    std::wstring name;
    double multiplier;
};

struct LocalizedDrivePattern {
    bool valid = false;
    std::wstring prefix;
    std::wstring middle;
    std::wstring suffix;
    bool freeFirst = true;
};

static std::vector<UnitDef> g_units;
static std::wstring g_localUnitMB = L"MB";
static std::wstring g_localUnitGB = L"GB";
static std::wstring g_localUnitTB = L"TB";
static LocalizedDrivePattern g_drivePattern;

using DrawThemeTextEx_t = HRESULT (WINAPI*)(HTHEME hTheme, HDC hdc, int iPartId, int iStateId, LPCWSTR pszText, int cchText, DWORD dwTextFlags, LPRECT pRect, const DTTOPTS *pOptions);
DrawThemeTextEx_t DrawThemeTextEx_Original;

using DrawTextExW_t = int (WINAPI*)(HDC hdc, LPWSTR lpchText, int cchText, LPRECT lprc, UINT format, LPDRAWTEXTPARAMS lpdtp);
DrawTextExW_t DrawTextExW_Original;

using DrawTextW_t = int (WINAPI*)(HDC hdc, LPCWSTR lpchText, int cchText, LPRECT lprc, UINT format);
DrawTextW_t DrawTextW_Original;

static thread_local bool g_inHook = false;

static COLORREF ParseHexColor(LPCWSTR hexStr, COLORREF fallback) {
    if (!hexStr || !*hexStr) return fallback;
    while (*hexStr == L' ' || *hexStr == L'#') hexStr++;
    unsigned int val = 0;
    if (swscanf_s(hexStr, L"%x", &val) == 1) {
        return RGB((val >> 16) & 0xFF, (val >> 8) & 0xFF, val & 0xFF);
    }
    return fallback;
}

static ColorMode ParseColorMode(LPCWSTR modeStr) {
    if (!modeStr) return COLOR_DEFAULT;
    if (_wcsicmp(modeStr, L"dynamic_used") == 0) return COLOR_DYNAMIC_USED;
    if (_wcsicmp(modeStr, L"dynamic_free") == 0) return COLOR_DYNAMIC_FREE;
    if (_wcsicmp(modeStr, L"static") == 0) return COLOR_STATIC;
    return COLOR_DEFAULT;
}

static void LoadSectionSetting(LPCWSTR prefix, SegmentSetting* out, LPCWSTR defFmt, LPCWSTR defMode, LPCWSTR defHex) {
    wchar_t key[64];
    swprintf_s(key, L"%s.show", prefix);
    out->show = Wh_GetIntSetting(key) != 0;

    swprintf_s(key, L"%s.format", prefix);
    PCWSTR fmt = Wh_GetStringSetting(key);
    out->format = (fmt && *fmt) ? fmt : defFmt;
    Wh_FreeStringSetting(fmt);

    swprintf_s(key, L"%s.colorMode", prefix);
    PCWSTR mode = Wh_GetStringSetting(key);
    out->colorMode = ParseColorMode((mode && *mode) ? mode : defMode);
    Wh_FreeStringSetting(mode);

    swprintf_s(key, L"%s.staticColor", prefix);
    PCWSTR hex = Wh_GetStringSetting(key);
    out->staticColor = ParseHexColor((hex && *hex) ? hex : defHex, RGB(74, 222, 128));
    Wh_FreeStringSetting(hex);

    swprintf_s(key, L"%s.bold", prefix);
    out->bold = Wh_GetIntSetting(key) != 0;
}

static void LoadSettings() {
    PCWSTR order = Wh_GetStringSetting(L"order");
    g_settings.order = (order && *order) ? order : L"free,used,total";
    Wh_FreeStringSetting(order);

    PCWSTR sep = Wh_GetStringSetting(L"separator");
    g_settings.separator = sep ? sep : L" | ";
    Wh_FreeStringSetting(sep);

    LoadSectionSetting(L"usedSection", &g_settings.used, L"{val}", L"default", L"#4ADE80");
    LoadSectionSetting(L"freeSection", &g_settings.free, L"{val} free", L"dynamic_used", L"#4ADE80");
    LoadSectionSetting(L"totalSection", &g_settings.total, L"({val})", L"default", L"#9CA3AF");

    g_settings.t1 = Wh_GetIntSetting(L"thresholds.t1");
    g_settings.t2 = Wh_GetIntSetting(L"thresholds.t2");
    g_settings.t3 = Wh_GetIntSetting(L"thresholds.t3");
    if (g_settings.t1 <= 0) g_settings.t1 = 31;
    if (g_settings.t2 <= 0) g_settings.t2 = 51;
    if (g_settings.t3 <= 0) g_settings.t3 = 71;

    PCWSTR cg = Wh_GetStringSetting(L"thresholds.colorGreen");
    g_settings.cGreen = ParseHexColor(cg, RGB(74, 222, 128));
    Wh_FreeStringSetting(cg);

    PCWSTR cy = Wh_GetStringSetting(L"thresholds.colorYellow");
    g_settings.cYellow = ParseHexColor(cy, RGB(250, 204, 21));
    Wh_FreeStringSetting(cy);

    PCWSTR co = Wh_GetStringSetting(L"thresholds.colorOrange");
    g_settings.cOrange = ParseHexColor(co, RGB(251, 146, 60));
    Wh_FreeStringSetting(co);

    PCWSTR cr = Wh_GetStringSetting(L"thresholds.colorRed");
    g_settings.cRed = ParseHexColor(cr, RGB(248, 113, 113));
    Wh_FreeStringSetting(cr);
}

// Strips Unicode directional formatting marks and normalizes Eastern Arabic / Persian digits
static std::wstring CleanStr(const wchar_t* src, int len) {
    std::wstring dst;
    dst.reserve(len);
    for (int i = 0; i < len && src[i]; i++) {
        wchar_t c = src[i];
        // Strip BiDi control characters (LRM, RLM, LRE, RLE, PDF, LRI, RLI, FSI, PDI)
        if (c == 0x200E || c == 0x200F || (c >= 0x202A && c <= 0x202E) || (c >= 0x2066 && c <= 0x2069))
            continue;
        if (c == 0x00A0 || c == 0x202F) c = L' '; // Non-breaking spaces
        // Convert Eastern Arabic numerals (٠-٩) to standard ASCII digits (0-9)
        if (c >= 0x0660 && c <= 0x0669) c = L'0' + (c - 0x0660);
        // Convert Extended Arabic/Persian numerals (۰-۹) to standard ASCII digits (0-9)
        if (c >= 0x06F0 && c <= 0x06F9) c = L'0' + (c - 0x06F0);
        dst.push_back(c);
    }
    return dst;
}

static void AddUnit(const std::wstring& name, double mult) {
    if (name.empty()) return;
    for (const auto& u : g_units) {
        if (_wcsicmp(u.name.c_str(), name.c_str()) == 0) return;
    }
    g_units.push_back({name, mult});
}

static std::wstring ExtractUnitFromFormattedByteSize(LONGLONG bytes) {
    wchar_t buf[64] = {};
    if (StrFormatByteSizeW(bytes, buf, 64)) {
        std::wstring s = CleanStr(buf, (int)wcslen(buf));
        size_t i = 0;
        while (i < s.length() && ((s[i] >= L'0' && s[i] <= L'9') || s[i] == L'.' || s[i] == L',' || s[i] == L' ' || s[i] == 0x066B || s[i] == 0x066C)) {
            i++;
        }
        std::wstring u = s.substr(i);
        while (!u.empty() && u.back() == L' ') u.pop_back();
        return u;
    }
    return L"";
}

static bool TryParseTemplateString(const std::wstring& rawTpl, LocalizedDrivePattern& outPat) {
    std::wstring clean = CleanStr(rawTpl.c_str(), (int)rawTpl.length());
    std::wstring s1, s2;
    size_t p1 = std::wstring::npos, p2 = std::wstring::npos;

    for (const wchar_t* cand1 : {L"%1!s!", L"%1!ls!", L"%1"}) {
        p1 = clean.find(cand1);
        if (p1 != std::wstring::npos) { s1 = cand1; break; }
    }
    for (const wchar_t* cand2 : {L"%2!s!", L"%2!ls!", L"%2"}) {
        p2 = clean.find(cand2);
        if (p2 != std::wstring::npos) { s2 = cand2; break; }
    }

    if (p1 == std::wstring::npos || p2 == std::wstring::npos) return false;

    if (p1 < p2) {
        outPat.freeFirst = true;
        outPat.prefix = clean.substr(0, p1);
        outPat.middle = clean.substr(p1 + s1.length(), p2 - (p1 + s1.length()));
        outPat.suffix = clean.substr(p2 + s2.length());
    } else {
        outPat.freeFirst = false;
        outPat.prefix = clean.substr(0, p2);
        outPat.middle = clean.substr(p2 + s2.length(), p1 - (p2 + s2.length()));
        outPat.suffix = clean.substr(p1 + s1.length());
    }

    // Must have a meaningful separator between the two values
    if (outPat.middle.length() < 2) return false;
    outPat.valid = true;
    return true;
}

static void InitLocalization() {
    g_units.clear();

    // 1. Dynamically query the active OS language's byte size units via Shlwapi
    std::wstring uB  = ExtractUnitFromFormattedByteSize(500);
    std::wstring uKB = ExtractUnitFromFormattedByteSize(1024LL * 10);
    std::wstring uMB = ExtractUnitFromFormattedByteSize(1048576LL * 10);
    std::wstring uGB = ExtractUnitFromFormattedByteSize(1073741824LL * 10);
    std::wstring uTB = ExtractUnitFromFormattedByteSize(1099511627776LL * 10);
    std::wstring uPB = ExtractUnitFromFormattedByteSize(1125899906842624LL);

    if (!uMB.empty()) g_localUnitMB = uMB;
    if (!uGB.empty()) g_localUnitGB = uGB;
    if (!uTB.empty()) g_localUnitTB = uTB;

    AddUnit(uB,  1.0);
    AddUnit(uKB, 1024.0);
    AddUnit(uMB, 1048576.0);
    AddUnit(uGB, 1073741824.0);
    AddUnit(uTB, 1099511627776.0);
    AddUnit(uPB, 1125899906842624.0);

    // 2. Fallback / multi-language units (English, Arabic, French, Russian, Spanish, etc.)
    for (auto s : {L"bytes", L"byte", L"B", L"octets", L"octet", L"o", L"بايت", L"ب", L"байт", L"Б"}) AddUnit(s, 1.0);
    for (auto s : {L"KB", L"Ko", L"КБ", L"كيلوبايت", L"ك.ب"}) AddUnit(s, 1024.0);
    for (auto s : {L"MB", L"Mo", L"МБ", L"ميغابايت", L"ميجابايت", L"م.ب"}) AddUnit(s, 1048576.0);
    for (auto s : {L"GB", L"Go", L"ГБ", L"غيغابايت", L"جيجابايت", L"ج.ب", L"غ.ب"}) AddUnit(s, 1073741824.0);
    for (auto s : {L"TB", L"To", L"ТБ", L"تيرابايت", L"ت.ب"}) AddUnit(s, 1099511627776.0);
    for (auto s : {L"PB", L"Po", L"ПБ", L"بيتابايت", L"ب.ب"}) AddUnit(s, 1125899906842624.0);

    // 3. Load the exact localized "X free of Y" template from shell32.dll
    // In Windows Vista/7/8/10/11, IDS_SPACEFREE lives around resource ID 33224 (0x81C8)
    HMODULE hShell32 = GetModuleHandleW(L"shell32.dll");
    if (!hShell32) hShell32 = LoadLibraryExW(L"shell32.dll", NULL, LOAD_LIBRARY_AS_DATAFILE);
    if (hShell32) {
        wchar_t buf[256] = {};
        if (LoadStringW(hShell32, 33224, buf, 256) > 0) {
            TryParseTemplateString(buf, g_drivePattern);
        }
        if (!g_drivePattern.valid) {
            for (UINT id = 33200; id <= 33260; id++) {
                if (LoadStringW(hShell32, id, buf, 256) > 0) {
                    if (TryParseTemplateString(buf, g_drivePattern)) break;
                }
            }
        }
    }
}

static bool MatchUnit(const wchar_t* unitStr, double* outMultiplier) {
    for (const auto& u : g_units) {
        if (_wcsicmp(unitStr, u.name.c_str()) == 0) {
            *outMultiplier = u.multiplier;
            return true;
        }
    }
    return false;
}

static bool ParseStorageValue(const std::wstring& str, double* outBytes, std::wstring* outCleanValStr) {
    size_t start = 0;
    while (start < str.length() && str[start] == L' ') start++;
    size_t end = str.length();
    while (end > start && str[end - 1] == L' ') end--;
    if (start >= end) return false;

    std::wstring trimmed = str.substr(start, end - start);

    // Parse leading number
    wchar_t numBuf[32];
    size_t n = 0;
    size_t i = 0;
    while (i < trimmed.length() && n < 31) {
        wchar_t c = trimmed[i];
        if (c >= L'0' && c <= L'9') {
            numBuf[n++] = c;
        } else if (c == L'.' || c == 0x066B) { // Standard dot or Arabic decimal separator (٫)
            numBuf[n++] = L'.';
        } else if (c == L',') {
            // Check if comma is a decimal separator (e.g., 12,5 GB or 12,50 GB) vs thousands separator (1,000 GB)
            size_t remDigits = 0;
            for (size_t k = i + 1; k < trimmed.length() && (trimmed[k] >= L'0' && trimmed[k] <= L'9'); k++) remDigits++;
            if (remDigits > 0 && remDigits <= 2) {
                numBuf[n++] = L'.';
            }
        } else if (c == 0x066C || c == L'\'' || (c == L' ' && i + 1 < trimmed.length() && trimmed[i + 1] >= L'0' && trimmed[i + 1] <= L'9')) {
            // Ignore Arabic thousands separator (٬), apostrophe, or space between digits
        } else {
            break;
        }
        i++;
    }

    if (n == 0) return false;
    numBuf[n] = L'\0';

    wchar_t* endPtr = nullptr;
    double val = wcstod(numBuf, &endPtr);
    if (endPtr == numBuf || *endPtr != L'\0') return false;

    while (i < trimmed.length() && trimmed[i] == L' ') i++;
    if (i >= trimmed.length()) return false;

    std::wstring unitPart = trimmed.substr(i);
    double mult = 1.0;
    if (!MatchUnit(unitPart.c_str(), &mult)) return false;

    *outBytes = val * mult;
    if (outCleanValStr) *outCleanValStr = trimmed;
    return true;
}

static std::wstring FormatBytes(double bytes) {
    wchar_t buf[64];
    if (bytes >= 1099511627776.0) {
        swprintf_s(buf, L"%.2f %s", bytes / 1099511627776.0, g_localUnitTB.c_str());
        return buf;
    }
    if (bytes >= 1073741824.0) {
        double gb = bytes / 1073741824.0;
        if (gb >= 100.0) {
            swprintf_s(buf, L"%.0f %s", gb, g_localUnitGB.c_str());
        } else {
            swprintf_s(buf, L"%.1f %s", gb, g_localUnitGB.c_str());
        }
        return buf;
    }
    swprintf_s(buf, L"%.0f %s", bytes / 1048576.0, g_localUnitMB.c_str());
    return buf;
}

static void ReplaceAll(std::wstring& str, const std::wstring& from, const std::wstring& to) {
    if (from.empty()) return;
    size_t start_pos = 0;
    while ((start_pos = str.find(from, start_pos)) != std::wstring::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length();
    }
}

static COLORREF EvalDynamicColor(double usedPct, ColorMode mode, COLORREF staticCol) {
    if (mode == COLOR_STATIC) return staticCol;
    double metric = (mode == COLOR_DYNAMIC_FREE) ? (100.0 - usedPct) : usedPct;
    if (metric < (double)g_settings.t1) return g_settings.cGreen;
    if (metric < (double)g_settings.t2) return g_settings.cYellow;
    if (metric < (double)g_settings.t3) return g_settings.cOrange;
    return g_settings.cRed;
}

static bool ExtractFreeAndTotalParts(const std::wstring& clean, double* outFreeBytes, double* outTotalBytes, std::wstring* outFreeStr, std::wstring* outTotalStr) {
    // 1. Try dynamic OS pattern from shell32.dll first
    if (g_drivePattern.valid) {
        bool prefixOk = g_drivePattern.prefix.empty() ||
                        (clean.length() >= g_drivePattern.prefix.length() &&
                         _wcsnicmp(clean.c_str(), g_drivePattern.prefix.c_str(), g_drivePattern.prefix.length()) == 0);
        bool suffixOk = g_drivePattern.suffix.empty() ||
                        (clean.length() >= g_drivePattern.suffix.length() &&
                         _wcsnicmp(clean.c_str() + clean.length() - g_drivePattern.suffix.length(), g_drivePattern.suffix.c_str(), g_drivePattern.suffix.length()) == 0);

        if (prefixOk && suffixOk && clean.length() > g_drivePattern.prefix.length() + g_drivePattern.suffix.length()) {
            std::wstring body = clean.substr(
                g_drivePattern.prefix.length(),
                clean.length() - g_drivePattern.prefix.length() - g_drivePattern.suffix.length()
            );

            const wchar_t* midPos = StrStrIW(body.c_str(), g_drivePattern.middle.c_str());
            if (midPos) {
                size_t midIdx = (size_t)(midPos - body.c_str());
                std::wstring part1 = body.substr(0, midIdx);
                std::wstring part2 = body.substr(midIdx + g_drivePattern.middle.length());

                std::wstring& freeCandidate  = g_drivePattern.freeFirst ? part1 : part2;
                std::wstring& totalCandidate = g_drivePattern.freeFirst ? part2 : part1;

                if (ParseStorageValue(freeCandidate, outFreeBytes, outFreeStr) &&
                    ParseStorageValue(totalCandidate, outTotalBytes, outTotalStr)) {
                    return true;
                }
            }
        }
    }

    // 2. Built-in fallback list for common languages
    static const wchar_t* const kKnownMiddlePhrases[] = {
        L" free of ",           // English
        L" حر من ",             // Arabic (Standard Windows translation: X حر من Y)
        L" فارغ من ",           // Arabic (Alternative)
        L" متاح من ",           // Arabic (Alternative)
        L" libres sur ",        // French
        L" frei von ",          // German
        L" disponibles de ",    // Spanish
        L" disponibili su ",    // Italian
        L" disponível de ",     // Portuguese
        L" disponíveis de ",    // Portuguese (Plural)
        L" свободно из ",       // Russian
        L" wolne z ",           // Polish
        L" beschikbaar van ",   // Dutch
    };

    for (const wchar_t* phrase : kKnownMiddlePhrases) {
        const wchar_t* pos = StrStrIW(clean.c_str(), phrase);
        if (pos) {
            size_t idx = (size_t)(pos - clean.c_str());
            std::wstring part1 = clean.substr(0, idx);
            std::wstring part2 = clean.substr(idx + wcslen(phrase));
            if (ParseStorageValue(part1, outFreeBytes, outFreeStr) &&
                ParseStorageValue(part2, outTotalBytes, outTotalStr)) {
                return true;
            }
        }
    }

    return false;
}

static bool BuildDriveChunks(LPCWSTR lpchText, int cchText, std::vector<RenderChunk>& chunks) {
    if (!lpchText) return false;
    int len = (cchText == -1) ? (int)wcslen(lpchText) : cchText;
    if (len < 8 || len > 140) return false;

    std::wstring clean = CleanStr(lpchText, len);

    double freeBytes = 0, totalBytes = 0;
    std::wstring freeValStr, totalValStr;

    if (!ExtractFreeAndTotalParts(clean, &freeBytes, &totalBytes, &freeValStr, &totalValStr)) {
        return false;
    }

    double usedBytes = totalBytes - freeBytes;
    if (usedBytes < 0) usedBytes = 0;

    double usedPct = (totalBytes > 0) ? (usedBytes / totalBytes) * 100.0 : 0.0;
    double freePct = 100.0 - usedPct;

    std::wstring usedValStr = FormatBytes(usedBytes);

    wchar_t usedPctBuf[16], freePctBuf[16];
    swprintf_s(usedPctBuf, L"%.0f%%", usedPct);
    swprintf_s(freePctBuf, L"%.0f%%", freePct);
    std::wstring usedPctStr = usedPctBuf;
    std::wstring freePctStr = freePctBuf;

    std::wstring orderStr = g_settings.order;
    size_t pos = 0;
    bool first = true;

    while (pos < orderStr.length()) {
        size_t nextComma = orderStr.find(L',', pos);
        if (nextComma == std::wstring::npos) nextComma = orderStr.length();
        std::wstring token = orderStr.substr(pos, nextComma - pos);
        pos = nextComma + 1;

        while (!token.empty() && token.front() == L' ') token.erase(0, 1);
        while (!token.empty() && token.back() == L' ') token.pop_back();

        const SegmentSetting* seg = nullptr;
        const std::wstring* valReplacement = nullptr;
        const wchar_t* pctReplacement = L"";

        if (_wcsicmp(token.c_str(), L"used") == 0) {
            seg = &g_settings.used;
            valReplacement = &usedValStr;
            pctReplacement = usedPctStr.c_str();
        } else if (_wcsicmp(token.c_str(), L"free") == 0) {
            seg = &g_settings.free;
            valReplacement = &freeValStr;
            pctReplacement = freePctStr.c_str();
        } else if (_wcsicmp(token.c_str(), L"total") == 0) {
            seg = &g_settings.total;
            valReplacement = &totalValStr;
            pctReplacement = L"100%";
        }

        if (seg && seg->show && valReplacement) {
            if (!first && !g_settings.separator.empty()) {
                chunks.push_back({g_settings.separator, false, 0, false});
            }
            first = false;

            std::wstring formatted = seg->format;
            ReplaceAll(formatted, L"{val}", *valReplacement);
            ReplaceAll(formatted, L"{pct}", pctReplacement);

            bool useCol = (seg->colorMode != COLOR_DEFAULT);
            COLORREF col = useCol ? EvalDynamicColor(usedPct, seg->colorMode, seg->staticColor) : 0;
            chunks.push_back({formatted, useCol, col, seg->bold});
        }
    }

    return !chunks.empty();
}

static HFONT CreateBoldFontFromDC(HDC hdc) {
    HFONT hFont = (HFONT)GetCurrentObject(hdc, OBJ_FONT);
    if (!hFont) return NULL;
    LOGFONTW lf{};
    if (GetObjectW(hFont, sizeof(lf), &lf)) {
        lf.lfWeight = FW_BOLD;
        return CreateFontIndirectW(&lf);
    }
    return NULL;
}

HRESULT WINAPI DrawThemeTextEx_Hook(HTHEME hTheme, HDC hdc, int iPartId, int iStateId, LPCWSTR pszText, int cchText, DWORD dwTextFlags, LPRECT pRect, const DTTOPTS *pOptions) {
    if (!g_inHook && pszText && pRect) {
        std::vector<RenderChunk> chunks;
        if (BuildDriveChunks(pszText, cchText, chunks)) {
            g_inHook = true;
            DWORD safeFlags = dwTextFlags & ~DT_MODIFYSTRING;

            if (safeFlags & DT_CALCRECT) {
                std::wstring combined;
                for (const auto& c : chunks) combined += c.text;
                HRESULT hr = DrawThemeTextEx_Original(hTheme, hdc, iPartId, iStateId, combined.c_str(), -1, safeFlags, pRect, pOptions);
                g_inHook = false;
                return hr;
            }

            // If HDC is not mirrored via LAYOUT_RTL, advance from right-to-left when DT_RTLREADING/DT_RIGHT is set
            DWORD dcLayout = GetLayout(hdc);
            bool dcMirrored = (dcLayout != GDI_ERROR && (dcLayout & LAYOUT_RTL));
            bool advanceFromRight = !dcMirrored && ((safeFlags & (DT_RIGHT | DT_RTLREADING)) != 0);

            HFONT hBoldFont = NULL;
            RECT curRect = *pRect;
            HRESULT hr = S_OK;

            for (const auto& chunk : chunks) {
                if (curRect.left >= curRect.right) break;

                HFONT hOldFont = NULL;
                if (chunk.bold) {
                    if (!hBoldFont) hBoldFont = CreateBoldFontFromDC(hdc);
                    if (hBoldFont) hOldFont = (HFONT)SelectObject(hdc, hBoldFont);
                }

                RECT calcRect = curRect;
                DTTOPTS calcOpts = pOptions ? *pOptions : DTTOPTS{sizeof(DTTOPTS)};
                calcOpts.dwFlags |= DTT_CALCRECT;
                DrawThemeTextEx_Original(hTheme, hdc, iPartId, iStateId, chunk.text.c_str(), -1, (safeFlags & ~(DT_END_ELLIPSIS | DT_RIGHT)) | DT_CALCRECT, &calcRect, &calcOpts);
                int w = calcRect.right - calcRect.left;

                DTTOPTS drawOpts = pOptions ? *pOptions : DTTOPTS{sizeof(DTTOPTS)};
                if (chunk.useCustomColor) {
                    drawOpts.dwFlags |= DTT_TEXTCOLOR;
                    drawOpts.crText = chunk.color;
                }

                RECT drawRect = curRect;
                DWORD chunkFlags = safeFlags;
                if (advanceFromRight) {
                    drawRect.left = std::max(curRect.left, curRect.right - w);
                    chunkFlags |= DT_RIGHT;
                }

                hr = DrawThemeTextEx_Original(hTheme, hdc, iPartId, iStateId, chunk.text.c_str(), -1, chunkFlags, &drawRect, &drawOpts);

                if (hOldFont) SelectObject(hdc, hOldFont);
                if (advanceFromRight) {
                    curRect.right -= w;
                } else {
                    curRect.left += w;
                }
            }

            if (hBoldFont) DeleteObject(hBoldFont);
            g_inHook = false;
            return hr;
        }
    }
    return DrawThemeTextEx_Original(hTheme, hdc, iPartId, iStateId, pszText, cchText, dwTextFlags, pRect, pOptions);
}

int WINAPI DrawTextExW_Hook(HDC hdc, LPWSTR lpchText, int cchText, LPRECT lprc, UINT format, LPDRAWTEXTPARAMS lpdtp) {
    if (!g_inHook && lpchText && lprc) {
        std::vector<RenderChunk> chunks;
        if (BuildDriveChunks(lpchText, cchText, chunks)) {
            g_inHook = true;
            UINT safeFormat = format & ~DT_MODIFYSTRING;

            if (safeFormat & DT_CALCRECT) {
                std::wstring combined;
                for (const auto& c : chunks) combined += c.text;
                int res = DrawTextExW_Original(hdc, (LPWSTR)combined.c_str(), -1, lprc, safeFormat, lpdtp);
                g_inHook = false;
                return res;
            }

            HTHEME hTheme = OpenThemeData(NULL, L"ItemsView");
            if (hTheme) {
                DWORD dcLayout = GetLayout(hdc);
                bool dcMirrored = (dcLayout != GDI_ERROR && (dcLayout & LAYOUT_RTL));
                bool advanceFromRight = !dcMirrored && ((safeFormat & (DT_RIGHT | DT_RTLREADING)) != 0);

                HFONT hBoldFont = NULL;
                RECT curRect = *lprc;
                int height = 0;

                for (const auto& chunk : chunks) {
                    if (curRect.left >= curRect.right) break;

                    HFONT hOldFont = NULL;
                    if (chunk.bold) {
                        if (!hBoldFont) hBoldFont = CreateBoldFontFromDC(hdc);
                        if (hBoldFont) hOldFont = (HFONT)SelectObject(hdc, hBoldFont);
                    }

                    RECT calcRect = curRect;
                    DTTOPTS calcOpts = {sizeof(DTTOPTS), DTT_CALCRECT};
                    DrawThemeTextEx_Original(hTheme, hdc, 0, 0, chunk.text.c_str(), -1, (safeFormat & ~(DT_END_ELLIPSIS | DT_RIGHT)) | DT_CALCRECT, &calcRect, &calcOpts);
                    int w = calcRect.right - calcRect.left;
                    height = calcRect.bottom - calcRect.top;

                    RECT drawRect = curRect;
                    UINT chunkFormat = safeFormat;
                    if (advanceFromRight) {
                        drawRect.left = std::max(curRect.left, curRect.right - w);
                        chunkFormat |= DT_RIGHT;
                    }

                    if (chunk.useCustomColor) {
                        DTTOPTS colorOpts = {sizeof(DTTOPTS), DTT_TEXTCOLOR | DTT_COMPOSITED};
                        colorOpts.crText = chunk.color;
                        DrawThemeTextEx_Original(hTheme, hdc, 0, 0, chunk.text.c_str(), -1, chunkFormat, &drawRect, &colorOpts);
                    } else {
                        DrawTextExW_Original(hdc, (LPWSTR)chunk.text.c_str(), -1, &drawRect, chunkFormat, lpdtp);
                    }

                    if (hOldFont) SelectObject(hdc, hOldFont);
                    if (advanceFromRight) {
                        curRect.right -= w;
                    } else {
                        curRect.left += w;
                    }
                }

                if (hBoldFont) DeleteObject(hBoldFont);
                CloseThemeData(hTheme);
                g_inHook = false;
                return height;
            }

            g_inHook = false;
        }
    }
    return DrawTextExW_Original(hdc, lpchText, cchText, lprc, format, lpdtp);
}

int WINAPI DrawTextW_Hook(HDC hdc, LPCWSTR lpchText, int cchText, LPRECT lprc, UINT format) {
    return DrawTextExW_Hook(hdc, (LPWSTR)lpchText, cchText, lprc, format, NULL);
}

void Wh_ModSettingsChanged() {
    LoadSettings();
}

BOOL Wh_ModInit() {
    InitLocalization();
    LoadSettings();

    HMODULE hUxTheme = GetModuleHandleW(L"uxtheme.dll");
    if (hUxTheme) {
        void* pDrawThemeTextEx = (void*)GetProcAddress(hUxTheme, "DrawThemeTextEx");
        if (pDrawThemeTextEx) {
            Wh_SetFunctionHook(pDrawThemeTextEx, (void*)DrawThemeTextEx_Hook, (void**)&DrawThemeTextEx_Original);
        }
    }

    HMODULE hUser32 = GetModuleHandleW(L"user32.dll");
    if (hUser32) {
        void* pDrawTextExW = (void*)GetProcAddress(hUser32, "DrawTextExW");
        if (pDrawTextExW) {
            Wh_SetFunctionHook(pDrawTextExW, (void*)DrawTextExW_Hook, (void**)&DrawTextExW_Original);
        }
        void* pDrawTextW = (void*)GetProcAddress(hUser32, "DrawTextW");
        if (pDrawTextW) {
            Wh_SetFunctionHook(pDrawTextW, (void*)DrawTextW_Hook, (void**)&DrawTextW_Original);
        }
    }
    return TRUE;
}