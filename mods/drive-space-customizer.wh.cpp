// ==WindhawkMod==
// @id              drive-space-customizer
// @name            Drive Space Customizer
// @description     Full customization of This PC drive space labels (order, colors, bold, brackets, custom text)
// @version         2.0
// @author          iMAboud
// @github          https://github.com/iMAboud
// @include         explorer.exe
// @compilerOptions -lgdi32 -luxtheme -lshlwapi
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Drive Space Label Customizer

Customizes the storage space label under drives in **This PC** (`X GB free of Y GB`)[cite: 1, 2].

![Preview](https://i.imgur.com/nFk8U8m.jpeg)

## Features & Settings
- **Display Order & Separator:** Reorder `used`, `free`, and `total` segments and set a custom divider (default: ` | `)[cite: 3, 4, 5].
- **Per-Segment Controls (Used / Free / Total):**
  - **Toggle:** Show or hide any segment.
  - **Custom Format:** Customize text and brackets using `{val}` (size) and `{pct}` (percentage) — e.g., `{val} free`, `{val} used`, `({val})`[cite: 3, 4, 5].
  - **Color Modes:** Default theme color, Dynamic Used % (Low = Green, High = Red), Dynamic Free % (High = Green, Low = Red), or Custom Static Hex.
  - **Bold Text:** Toggle bold font weight per segment[cite: 3, 4, 5].
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

static void LoadSectionSetting(LPCWSTR prefix, SegmentSetting* out, bool defShow, LPCWSTR defFmt, LPCWSTR defMode, LPCWSTR defHex) {
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

    LoadSectionSetting(L"usedSection", &g_settings.used, true, L"{val}", L"default", L"#4ADE80");
    LoadSectionSetting(L"freeSection", &g_settings.free, true, L"{val} free", L"dynamic_used", L"#4ADE80");
    LoadSectionSetting(L"totalSection", &g_settings.total, true, L"({val})", L"default", L"#9CA3AF");

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

static void CleanStr(const wchar_t* src, int len, wchar_t* dst, size_t maxDst) {
    size_t j = 0;
    for (int i = 0; i < len && src[i] && j < maxDst - 1; i++) {
        wchar_t c = src[i];
        if (c == 0x200E || c == 0x200F || (c >= 0x202A && c <= 0x202E) || (c >= 0x2066 && c <= 0x2069))
            continue;
        if (c == 0x00A0) c = L' ';
        dst[j++] = c;
    }
    dst[j] = L'\0';
}

static bool ParseStorageValue(const wchar_t* str, double* val, wchar_t* unit, size_t unitMax) {
    while (*str && (*str < L'0' || *str > L'9')) str++;
    if (!*str) return false;

    wchar_t numBuf[32];
    size_t n = 0;
    while (*str && ((*str >= L'0' && *str <= L'9') || *str == L'.' || *str == L',') && n < 31) {
        if (*str == L',') {
            if (str[1] >= L'0' && str[1] <= L'9' && str[2] >= L'0' && str[2] <= L'9' && (str[3] < L'0' || str[3] > L'9'))
                numBuf[n++] = L'.';
        } else {
            numBuf[n++] = *str;
        }
        str++;
    }
    numBuf[n] = L'\0';
    *val = wcstod(numBuf, nullptr);

    while (*str == L' ') str++;
    size_t i = 0;
    while (str[i] && str[i] != L' ' && i < unitMax - 1) {
        unit[i] = str[i];
        i++;
    }
    unit[i] = L'\0';
    return i > 0;
}

static double ToBytes(double val, const wchar_t* unit) {
    if (_wcsicmp(unit, L"TB") == 0 || _wcsicmp(unit, L"تيرابايت") == 0) return val * 1099511627776.0;
    if (_wcsicmp(unit, L"GB") == 0 || _wcsicmp(unit, L"غيغابايت") == 0 || _wcsicmp(unit, L"جيجابايت") == 0) return val * 1073741824.0;
    if (_wcsicmp(unit, L"MB") == 0 || _wcsicmp(unit, L"ميغابايت") == 0 || _wcsicmp(unit, L"ميجابايت") == 0) return val * 1048576.0;
    if (_wcsicmp(unit, L"KB") == 0 || _wcsicmp(unit, L"كيلوبايت") == 0) return val * 1024.0;
    return val;
}

static void FormatBytes(double bytes, wchar_t* out, size_t maxOut) {
    if (bytes >= 1099511627776.0) {
        swprintf_s(out, maxOut, L"%.2f TB", bytes / 1099511627776.0);
    } else if (bytes >= 1073741824.0) {
        double gb = bytes / 1073741824.0;
        if (gb >= 100.0) swprintf_s(out, maxOut, L"%.0f GB", gb);
        else swprintf_s(out, maxOut, L"%.1f GB", gb);
    } else {
        swprintf_s(out, maxOut, L"%.0f MB", bytes / 1048576.0);
    }
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

static bool BuildDriveChunks(LPCWSTR lpchText, int cchText, std::vector<RenderChunk>& chunks) {
    if (!lpchText) return false;
    int len = (cchText == -1) ? (int)wcslen(lpchText) : cchText;
    if (len < 10 || len > 120) return false;

    wchar_t clean[128];
    CleanStr(lpchText, len, clean, 128);

    wchar_t* freePos = StrStrIW(clean, L" free of ");
    if (!freePos) return false;

    *freePos = L'\0';
    wchar_t* totalPart = freePos + 9;

    double freeVal = 0, totalVal = 0;
    wchar_t freeUnit[16] = {}, totalUnit[16] = {};

    if (!ParseStorageValue(clean, &freeVal, freeUnit, 16) ||
        !ParseStorageValue(totalPart, &totalVal, totalUnit, 16)) {
        return false;
    }

    double totalBytes = ToBytes(totalVal, totalUnit);
    double freeBytes = ToBytes(freeVal, freeUnit);
    double usedBytes = totalBytes - freeBytes;
    if (usedBytes < 0) usedBytes = 0;

    double usedPct = (totalBytes > 0) ? (usedBytes / totalBytes) * 100.0 : 0.0;
    double freePct = 100.0 - usedPct;

    wchar_t usedValStr[32], freeValStr[32], totalValStr[32];
    FormatBytes(usedBytes, usedValStr, 32);
    swprintf_s(freeValStr, L"%s", clean);
    swprintf_s(totalValStr, L"%s", totalPart);

    wchar_t usedPctStr[16], freePctStr[16];
    swprintf_s(usedPctStr, L"%.0f%%", usedPct);
    swprintf_s(freePctStr, L"%.0f%%", freePct);

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
        const wchar_t* valReplacement = L"";
        const wchar_t* pctReplacement = L"";

        if (_wcsicmp(token.c_str(), L"used") == 0) {
            seg = &g_settings.used;
            valReplacement = usedValStr;
            pctReplacement = usedPctStr;
        } else if (_wcsicmp(token.c_str(), L"free") == 0) {
            seg = &g_settings.free;
            valReplacement = freeValStr;
            pctReplacement = freePctStr;
        } else if (_wcsicmp(token.c_str(), L"total") == 0) {
            seg = &g_settings.total;
            valReplacement = totalValStr;
            pctReplacement = L"100%";
        }

        if (seg && seg->show) {
            if (!first && !g_settings.separator.empty()) {
                chunks.push_back({g_settings.separator, false, 0, false});
            }
            first = false;

            std::wstring formatted = seg->format;
            ReplaceAll(formatted, L"{val}", valReplacement);
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

            if (dwTextFlags & DT_CALCRECT) {
                std::wstring combined;
                for (const auto& c : chunks) combined += c.text;
                HRESULT hr = DrawThemeTextEx_Original(hTheme, hdc, iPartId, iStateId, combined.c_str(), -1, dwTextFlags, pRect, pOptions);
                g_inHook = false;
                return hr;
            }

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
                DrawThemeTextEx_Original(hTheme, hdc, iPartId, iStateId, chunk.text.c_str(), -1, (dwTextFlags & ~DT_END_ELLIPSIS) | DT_CALCRECT, &calcRect, &calcOpts);
                int w = calcRect.right - calcRect.left;

                DTTOPTS drawOpts = pOptions ? *pOptions : DTTOPTS{sizeof(DTTOPTS)};
                if (chunk.useCustomColor) {
                    drawOpts.dwFlags |= DTT_TEXTCOLOR;
                    drawOpts.crText = chunk.color;
                }
                hr = DrawThemeTextEx_Original(hTheme, hdc, iPartId, iStateId, chunk.text.c_str(), -1, dwTextFlags, &curRect, &drawOpts);

                if (hOldFont) SelectObject(hdc, hOldFont);
                curRect.left += w;
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

            if (format & DT_CALCRECT) {
                std::wstring combined;
                for (const auto& c : chunks) combined += c.text;
                int res = DrawTextExW_Original(hdc, (LPWSTR)combined.c_str(), -1, lprc, format, lpdtp);
                g_inHook = false;
                return res;
            }

            HTHEME hTheme = OpenThemeData(NULL, L"ItemsView");
            if (hTheme) {
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
                    DrawThemeTextEx_Original(hTheme, hdc, 0, 0, chunk.text.c_str(), -1, (format & ~DT_END_ELLIPSIS) | DT_CALCRECT, &calcRect, &calcOpts);
                    int w = calcRect.right - calcRect.left;
                    height = calcRect.bottom - calcRect.top;

                    if (chunk.useCustomColor) {
                        DTTOPTS colorOpts = {sizeof(DTTOPTS), DTT_TEXTCOLOR | DTT_COMPOSITED};
                        colorOpts.crText = chunk.color;
                        DrawThemeTextEx_Original(hTheme, hdc, 0, 0, chunk.text.c_str(), -1, format, &curRect, &colorOpts);
                    } else {
                        DrawTextExW_Original(hdc, (LPWSTR)chunk.text.c_str(), -1, &curRect, format, lpdtp);
                    }

                    if (hOldFont) SelectObject(hdc, hOldFont);
                    curRect.left += w;
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