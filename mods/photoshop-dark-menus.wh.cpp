// ==WindhawkMod==
// @id                photoshop-dark-menus
// @name              Photoshop Dark Menus
// @description       Enables dark mode and custom separator colors for all menus in Adobe Photoshop.
// @version           1.0.0
// @author            Saber Naeemi
// @github          https://github.com/sabergraphics
// @twitter         https://x.com/SaberNaeemi
// @homepage        https://www.sabernaeemi.com
// @include           Photoshop.exe
// @compilerOptions   -lUser32 -lGdi32
// @license      MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Photoshop Dark Menus

Forces dark mode for all context and top menu bar dropdowns in Adobe Photoshop without permanently altering registry keys on disk.

### Features
- Dark backgrounds and customizable text colors across all menus.
- Independent separator line color control (set to match menu background to hide separators).
- Legible disabled item text styling.
- Automatic restoration of default Windows system colors upon exiting Photoshop.

### Screenshots
![Photoshop Dark Menu Dropdown](images/photoshop-dark-menu-screenshot-1.png)

![Photoshop Dark Context Menu](imagesphotoshop-dark-menu-screenshot-2.png)
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- MenuBgColor: "#282828"
  $name: "Menu Background Color"
- MenuTextColor: "#DCDCDC"
  $name: "Menu Text Color"
- HighlightBgColor: "#505050"
  $name: "Highlight Background Color"
- SeparatorColor: "#383838"
  $name: "Separator Line Color (Set same as Menu Background Color to hide)"
- GrayTextColor: "#808080"
  $name: "Disabled Text Color"
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <windhawk_api.h>
#include <windhawk_utils.h>
#include <wchar.h>

constexpr int NUM_ELEMENTS = 10;

const INT g_sysElements[NUM_ELEMENTS] = {
    COLOR_MENU,
    COLOR_MENUTEXT,
    COLOR_HIGHLIGHT,
    COLOR_HIGHLIGHTTEXT,
    COLOR_BTNSHADOW,
    COLOR_GRAYTEXT,
    COLOR_BTNHIGHLIGHT,
    COLOR_3DDKSHADOW,
    COLOR_3DLIGHT,
    COLOR_MENUBAR
};

COLORREF g_origColors[NUM_ELEMENTS] = {};
bool g_hasSavedOrigColors = false;

HBRUSH g_hSeparatorBrush = nullptr;

COLORREF ParseHexColor(const PCWSTR hexStr, COLORREF defaultColor) {
    if (!hexStr || wcslen(hexStr) < 7 || hexStr[0] != L'#')
        return defaultColor;

    unsigned int r = 0, g = 0, b = 0;
    if (swscanf_s(hexStr + 1, L"%02x%02x%02x", &r, &g, &b) == 3) {
        return RGB(r, g, b);
    }
    return defaultColor;
}

void SaveOriginalColors() {
    if (g_hasSavedOrigColors) return;

    for (int i = 0; i < NUM_ELEMENTS; i++) {
        g_origColors[i] = GetSysColor(g_sysElements[i]);
    }
    g_hasSavedOrigColors = true;
}

void ApplyDarkSystemColors() {
    SaveOriginalColors();

    if (g_hSeparatorBrush) DeleteObject(g_hSeparatorBrush);

    const PCWSTR bgStr        = Wh_GetStringSetting(L"MenuBgColor");
    const PCWSTR textStr      = Wh_GetStringSetting(L"MenuTextColor");
    const PCWSTR highlightStr = Wh_GetStringSetting(L"HighlightBgColor");
    const PCWSTR sepStr       = Wh_GetStringSetting(L"SeparatorColor");
    const PCWSTR grayStr      = Wh_GetStringSetting(L"GrayTextColor");

    COLORREF colMenu      = ParseHexColor(bgStr,        RGB(40, 40, 40));
    COLORREF colText      = ParseHexColor(textStr,      RGB(220, 220, 220));
    COLORREF colHighlight = ParseHexColor(highlightStr, RGB(80, 80, 80));
    COLORREF colSep       = ParseHexColor(sepStr,        RGB(56, 56, 56));
    COLORREF colGray      = ParseHexColor(grayStr,      RGB(128, 128, 128));

    Wh_FreeStringSetting(bgStr);
    Wh_FreeStringSetting(textStr);
    Wh_FreeStringSetting(highlightStr);
    Wh_FreeStringSetting(sepStr);
    Wh_FreeStringSetting(grayStr);

    g_hSeparatorBrush = CreateSolidBrush(colSep);

    COLORREF darkColors[NUM_ELEMENTS] = {
        colMenu,
        colText,
        colHighlight,
        RGB(255, 255, 255),
        colMenu,
        colGray,
        colMenu,
        colMenu,
        colMenu,
        colMenu
    };

    SetSysColors(NUM_ELEMENTS, g_sysElements, darkColors);
}

void RestoreOriginalColors() {
    if (g_hasSavedOrigColors) {
        SetSysColors(NUM_ELEMENTS, g_sysElements, g_origColors);
    }
    if (g_hSeparatorBrush) DeleteObject(g_hSeparatorBrush);
}

using PatBlt_t = BOOL (WINAPI*)(HDC hdc, int x, int y, int w, int h, DWORD rop);
PatBlt_t PatBlt_Original = nullptr;

BOOL WINAPI PatBlt_Hook(HDC hdc, int x, int y, int w, int h, DWORD rop) {
    if ((h == 1 || h == 2) && w > 20 && g_hSeparatorBrush) {
        HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, g_hSeparatorBrush);
        BOOL bRes = PatBlt_Original(hdc, x, y, w, h, rop);
        SelectObject(hdc, hOldBrush);
        return bRes;
    }
    return PatBlt_Original(hdc, x, y, w, h, rop);
}

using FillRect_t = int (WINAPI*)(HDC hdc, const RECT *lprc, HBRUSH hbr);
FillRect_t FillRect_Original = nullptr;

int WINAPI FillRect_Hook(HDC hdc, const RECT *lprc, HBRUSH hbr) {
    if (lprc && g_hSeparatorBrush) {
        int h = lprc->bottom - lprc->top;
        int w = lprc->right - lprc->left;
        if ((h == 1 || h == 2) && w > 20) {
            return FillRect_Original(hdc, lprc, g_hSeparatorBrush);
        }
    }
    return FillRect_Original(hdc, lprc, hbr);
}

void Wh_ModSettingsChanged() {
    ApplyDarkSystemColors();
}

BOOL Wh_ModInit() {
    Wh_Log(L"Initializing Photoshop Dark Menus");
    ApplyDarkSystemColors();

    HMODULE hGdi32 = GetModuleHandleW(L"gdi32.dll");
    if (hGdi32) {
        void* pPatBlt = (void*)GetProcAddress(hGdi32, "PatBlt");
        if (pPatBlt) WindhawkUtils::SetFunctionHook(pPatBlt, (void*)PatBlt_Hook, (void**)&PatBlt_Original);
    }

    HMODULE hUser32 = GetModuleHandleW(L"user32.dll");
    if (hUser32) {
        void* pFillRect = (void*)GetProcAddress(hUser32, "FillRect");
        if (pFillRect) WindhawkUtils::SetFunctionHook(pFillRect, (void*)FillRect_Hook, (void**)&FillRect_Original);
    }

    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L"Restoring original system palette colors");
    RestoreOriginalColors();
}