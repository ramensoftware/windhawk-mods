// ==WindhawkMod==
// @id              disk-usage-bar-color
// @name            Disk Usage Bar Color
// @description     Customize the disk usage bar color in File Explorer This PC view
// @version         1.1.0
// @author          dirtyrazkl
// @github          https://github.com/dirtyrazkl
// @include         explorer.exe
// @compilerOptions -luxtheme -lmsimg32 -lgdi32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Disk Usage Bar Color

Customizes the color of the disk usage progress bars shown in File Explorer's
**This PC** view.

## Settings

- **Bar Color (Normal)**: Hex color for drives with normal usage (default: `60CDFF`)
- **Bar Color (Full/Warning)**: Hex color for drives that are nearly full,
  shown in red by default (default: `E81123`)

Enter colors as 6-digit hex codes without the `#` prefix (e.g. `60CDFF`).

## Notes

- Only drive bars in the **This PC** view are affected. File copy/move progress
  bars are filtered out using window ancestry checks.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- barColor: 60CDFF
  $name: Bar Color (Normal)
  $description: >-
    Hex color code for the normal disk usage bar (no # prefix, e.g. 60CDFF)
- barColorFull: E81123
  $name: Bar Color (Full/Warning)
  $description: >-
    Hex color code for the bar when a drive is nearly full (no # prefix, e.g. E81123)
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>
#include <uxtheme.h>
#include <string>

static decltype(&DrawThemeBackground) DrawThemeBackground_orig = nullptr;
static COLORREF g_barColor     = RGB(96, 205, 255);  // PBFS_PARTIAL (normal)
static COLORREF g_barColorFull = RGB(232, 17, 35);   // PBFS_ERROR   (full/warning)

static std::wstring GetThemeClass(HTHEME hTheme)
{
    typedef HRESULT(WINAPI* pFn)(HTHEME, LPWSTR, INT);
    static auto fn = (pFn)GetProcAddress(GetModuleHandleW(L"uxtheme.dll"), MAKEINTRESOURCEA(74));
    if (!fn) return L"";
    WCHAR buf[255] = {};
    return SUCCEEDED(fn(hTheme, buf, 255)) ? buf : L"";
}

static COLORREF ParseHexColor(PCWSTR hex, COLORREF fallback)
{
    auto h = [](wchar_t c) -> int {
        if (c >= L'0' && c <= L'9') return c - L'0';
        if (c >= L'A' && c <= L'F') return c - L'A' + 10;
        if (c >= L'a' && c <= L'f') return c - L'a' + 10;
        return 0;
    };
    if (!hex || wcslen(hex) < 6) return fallback;
    return RGB((h(hex[0])<<4)|h(hex[1]), (h(hex[2])<<4)|h(hex[3]), (h(hex[4])<<4)|h(hex[5]));
}

static void LoadSettings()
{
    PCWSTR hex = Wh_GetStringSetting(L"barColor");
    g_barColor = ParseHexColor(hex, RGB(96, 205, 255));
    Wh_FreeStringSetting(hex);

    hex = Wh_GetStringSetting(L"barColorFull");
    g_barColorFull = ParseHexColor(hex, RGB(232, 17, 35));
    Wh_FreeStringSetting(hex);
}

// Fill with solid color + full alpha using AlphaBlend so the alpha channel
// in the buffered HDC is written correctly (FillRect leaves alpha=0 = transparent)
static void PaintSolidRect(HDC hdc, LPCRECT pRect, COLORREF color)
{
    int w = pRect->right - pRect->left;
    int h = pRect->bottom - pRect->top;
    if (w <= 0 || h <= 0) return;

    HDC memDC = CreateCompatibleDC(hdc);
    if (!memDC) return;

    BITMAPINFO bmi = {};
    bmi.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth       = w;
    bmi.bmiHeader.biHeight      = -h;
    bmi.bmiHeader.biPlanes      = 1;
    bmi.bmiHeader.biBitCount    = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    void* pixels = nullptr;
    HBITMAP dib = CreateDIBSection(hdc, &bmi, DIB_RGB_COLORS, &pixels, NULL, 0);
    if (!dib) { DeleteDC(memDC); return; }

    HGDIOBJ old = SelectObject(memDC, dib);

    BYTE r = GetRValue(color), g = GetGValue(color), b = GetBValue(color);
    BYTE* p = (BYTE*)pixels;
    for (int i = 0; i < w * h; i++, p += 4)
    {
        p[0] = b;
        p[1] = g;
        p[2] = r;
        p[3] = 255;
    }

    BLENDFUNCTION bf = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
    AlphaBlend(hdc, pRect->left, pRect->top, w, h, memDC, 0, 0, w, h, bf);

    SelectObject(memDC, old);
    DeleteObject(dib);
    DeleteDC(memDC);
}

// Returns true if the DC belongs to a window inside the main Explorer cabinet
// (the This PC view). Returns true also for DCs with no associated window (e.g.
// buffered painting), since we can't determine context in that case.
// Returns false only when we can positively identify a non-Explorer window (e.g.
// a file copy/move dialog), preventing those progress bars from being colored.
static bool IsDriveListDC(HDC hdc)
{
    HWND hwnd = WindowFromDC(hdc);
    if (!hwnd) return true;  // Can't determine context — allow through

    WCHAR cls[256];
    for (HWND cur = hwnd; cur; cur = GetAncestor(cur, GA_PARENT))
    {
        if (GetClassNameW(cur, cls, 256) && wcscmp(cls, L"CabinetWClass") == 0)
            return true;
    }
    return false;
}

HRESULT WINAPI HookedDrawThemeBackground(
    HTHEME hTheme, HDC hdc, INT iPartId, INT iStateId,
    LPCRECT pRect, LPCRECT pClipRect)
{
    // PP_FILL = 5; PBFS_PARTIAL = 4 (normal), PBFS_ERROR = 2 (full/warning)
    if (iPartId == 5 && pRect && GetThemeClass(hTheme) == L"Progress" && IsDriveListDC(hdc))
    {
        COLORREF color;
        if (iStateId == 2)       // PBFS_ERROR — drive nearly full
            color = g_barColorFull;
        else if (iStateId == 4)  // PBFS_PARTIAL — normal usage
            color = g_barColor;
        else
            return DrawThemeBackground_orig(hTheme, hdc, iPartId, iStateId, pRect, pClipRect);

        RECT clipRect = *pRect;
        if (pClipRect) IntersectRect(&clipRect, &clipRect, pClipRect);
        PaintSolidRect(hdc, &clipRect, color);
        return S_OK;
    }
    return DrawThemeBackground_orig(hTheme, hdc, iPartId, iStateId, pRect, pClipRect);
}

BOOL Wh_ModInit()
{
    LoadSettings();
    WindhawkUtils::SetFunctionHook(DrawThemeBackground, HookedDrawThemeBackground, &DrawThemeBackground_orig);
    return TRUE;
}

void Wh_ModSettingsChanged()
{
    LoadSettings();
}
