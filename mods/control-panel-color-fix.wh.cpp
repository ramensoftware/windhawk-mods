// ==WindhawkMod==
// @id              control-panel-color-fix
// @name            Control Panel Color Fix
// @description     For custom themes, fixes white header and sidebar in Control Panel
// @version         1.0.1
// @author          chip33
// @github          https://github.com/chip33
// @include         explorer.exe
// @include         systemsettings.exe
// @compilerOptions -luxtheme -lgdi32
// ==/WindhawkMod==

// ==WindhawkModSettings==
/*
- contrastEnabled: true
  $name: Header/Sidebar contrast offset

- contrastOffset: 22
  $name: " "
  $description: If Control Panel body color is 32,32,32, an offset of +22 lightens the header/sidebar to 54,54,54. Negative values darken.

- useManualRGB: false
  $name: Header/Sidebar color override

- manualRGB: "54,54,54"
  $name: " "
  $description: "Custom RGB color. Range: 0,0,0 (black) to 255,255,255 (white)."
*/
// ==/WindhawkModSettings==

// ==WindhawkModReadme==
/*
# Control Panel Color Fix

Repaint the hardcoded white header and sidebar to match the custom theme.

Optional ± contrast offset between Control Panel body and header/sidebar.

Manual header/sidebar color override.

### **Dark theme**

| Original | Contrast offset |
|----------|-----------------|
| ![Control Panel before](https://raw.githubusercontent.com/chip33/images/main/wh/cpcf/cpcf-cp-b.png) | ![Control Panel after](https://raw.githubusercontent.com/chip33/images/main/wh/cpcf/cpcf-cp-a.png) |
| ![Recovery before](https://raw.githubusercontent.com/chip33/images/main/wh/cpcf/cpcf-recovery-b.png) | ![Recovery after](https://raw.githubusercontent.com/chip33/images/main/wh/cpcf/cpcf-recovery-a.png) |

### **Dark theme with mica**

Original | Contrast offset |
|----------|-----------------|
| ![Control Panel mica before](https://raw.githubusercontent.com/chip33/images/main/wh/cpcf/cpcf-cp-mica-b.png) | ![Control Panel mica after](https://raw.githubusercontent.com/chip33/images/main/wh/cpcf/cpcf-cp-mica-a.png) |
| ![Recovery mica before](https://raw.githubusercontent.com/chip33/images/main/wh/cpcf/cpcf-recovery-mica-b.png) | ![Recovery mica after](https://raw.githubusercontent.com/chip33/images/main/wh/cpcf/cpcf-recovery-mica-a.png) |
*/
// ==/WindhawkModReadme==

// Based on earlier uxtheme-hook versions by rounk-ctrl.

#include <windows.h>
#include <uxtheme.h>
#include <vssym32.h>   // TMT_FILLCOLOR, TMT_TEXTCOLOR
#include <windhawk_api.h>
#include <windhawk_utils.h>
#include <algorithm>
#include <string>

#ifdef _WIN64
#define STDCALL __cdecl
#define SSTDCALL L"__cdecl"
#else
#define STDCALL __stdcall
#define SSTDCALL L"__stdcall"
#endif

typedef VOID(STDCALL *Element_PaintBgT)(
    class Element*, HDC, class Value*, LPRECT, LPRECT, LPRECT, LPRECT);
static Element_PaintBgT Element_PaintBg = nullptr;

// ---------------------------
// Settings state
// ---------------------------
static bool g_contrastEnabled = true;
static int  g_contrastOffset  = 22;
static bool g_useManualRGB    = false;
static COLORREF g_manualRGB   = RGB(54,54,54);

// ---------------------------
// Helpers
// ---------------------------

// Parse "R,G,B" user setting to COLORREF.
// Example input: "34,35,40" → RGB(34,35,40).
static COLORREF ParseRGBString(const std::wstring& rgbStr) {
    int r = 0, g = 0, b = 0;
    if (swscanf(rgbStr.c_str(), L"%d,%d,%d", &r, &g, &b) == 3) {
        if (r >= 0 && r <= 255 &&
            g >= 0 && g <= 255 &&
            b >= 0 && b <= 255) {
            return RGB(r, g, b);
        }
    }
    // Fallback to default on invalid input.
    return g_manualRGB;
}

// Apply a uniform contrast delta to all channels while preserving hue.
// Positive values lighten, negative values darken.
// Example: RGB(30,60,200) +22 → RGB(52,82,222).
static COLORREF ApplyDelta(COLORREF base, int delta) {
    int r = std::clamp(GetRValue(base) + delta, 0, 255);
    int g = std::clamp(GetGValue(base) + delta, 0, 255);
    int b = std::clamp(GetBValue(base) + delta, 0, 255);
    return RGB(r, g, b);
}

// Fill a rectangle with a solid color.
static void FillRectColor(HDC hdc, const RECT* rc, COLORREF color) {
    if (!rc) return;
    // Skip if rect is empty or inverted (no drawable area).
    if (rc->right <= rc->left || rc->bottom <= rc->top) return;
    HBRUSH brush = CreateSolidBrush(color);
    FillRect(hdc, rc, brush);
    DeleteObject(brush);
}

// Detect if theme alters Control Panel body color (TMT_FILLCOLOR).
static bool ThemeAltersCPBody(HWND wnd) {
    constexpr COLORREF kDefaultCPBody = RGB(255,255,255); // Default CP body = white.
    HTHEME hTh = OpenThemeData(wnd, L"ControlPanel");
    if (!hTh) {
        Wh_Log(L"Failed to open theme data, skipping repaint.");
        return false;
    }

    // Fallback default if theme query fails.
    COLORREF clrBody = kDefaultCPBody;
    // partId=2, stateId=0 = Control Panel body color.
    // Not officially documented, identified through testing.
    if (SUCCEEDED(GetThemeColor(hTh, 2, 0, TMT_FILLCOLOR, &clrBody))) {
        // StartAllBack repaints visually but leaves TMT_FILLCOLOR at default,
        // so any significant difference means the theme has altered Control Panel.
        const int tolerance = 2; // Ignore small rounding drift from theme API.
        if (abs(GetRValue(clrBody) - GetRValue(kDefaultCPBody)) > tolerance ||
            abs(GetGValue(clrBody) - GetGValue(kDefaultCPBody)) > tolerance ||
            abs(GetBValue(clrBody) - GetBValue(kDefaultCPBody)) > tolerance) {
            CloseThemeData(hTh);
            return true;
        }
    } else Wh_Log(L"Failed to get theme color, skipping repaint.");

    CloseThemeData(hTh);
    return false;
}

// ---------------------------
// Settings load
// ---------------------------
static void LoadSettings() {
    g_contrastEnabled = Wh_GetIntSetting(L"contrastEnabled") != 0;
    g_contrastOffset  = Wh_GetIntSetting(L"contrastOffset");
    g_useManualRGB    = Wh_GetIntSetting(L"useManualRGB") != 0;
    g_manualRGB       = ParseRGBString(Wh_GetStringSetting(L"manualRGB"));
}

// ---------------------------
// Hook
// ---------------------------
VOID STDCALL Element_PaintBgHook(
    class Element* This, HDC hdc, class Value* value,
    LPRECT pRect, LPRECT pClipRect, LPRECT pExcludeRect, LPRECT pTargetRect)
{
    // No theme value; call original function.
    if (!value) {
        Element_PaintBg(This, hdc, value, pRect, pClipRect, pExcludeRect, pTargetRect);
        return;
    }

    int elementType = (int)(*(DWORD*)value << 26) >> 26;
    if (elementType != 9 && pRect) {
        auto rawValueField   = *((unsigned __int64*)value + 1);
        auto elementTypeCode = (rawValueField + 20) & 7;

        // Observed mapping; not officially documented, identified through testing.
        // 6 -> selection
        // 3 -> hovered
        // 4 -> Control Panel header and sidebar
        // 1 -> cp_hub_frame (new Control Panel page style)

        static const bool debug = false; // set true when testing
        if (debug) Wh_Log(L"elementTypeCode=%llu elementType=%d", elementTypeCode, elementType);

        if (elementTypeCode == 4) {
            HWND wnd = WindowFromDC(hdc);
            // Repaint only if theme alters CP body color; avoids StartAllBack conflicts and redundant draws.
            if (ThemeAltersCPBody(wnd)) {
                HTHEME hTh = OpenThemeData(wnd, L"ControlPanel");
                // Fallback default if theme query fails (dark body baseline).
                COLORREF clrBody = RGB(32,32,32);
                if (hTh) {
                    // Query CP body color (TMT_FILLCOLOR, partId=2, stateId=0).
                    // Not officially documented, identified through testing.
                    GetThemeColor(hTh, 2, 0, TMT_FILLCOLOR, &clrBody);
                    CloseThemeData(hTh);
                }

                COLORREF clrHeader;
                if (g_useManualRGB) {
                    // Manual override: use configured RGB value.
                    clrHeader = g_manualRGB;
                } else if (g_contrastEnabled) {
                    // Contrast enabled: apply delta to body color.
                    clrHeader = ApplyDelta(clrBody, g_contrastOffset);
                } else {
                    // Contrast disabled: use body color unchanged.
                    clrHeader = clrBody;
                }

                FillRectColor(hdc, pRect, clrHeader);

                // Short-circuit: skip original repaint (prevents StartAllBack overwrite).
                return;
            } else {
                // Theme does not alter CP body; call original function.
                Element_PaintBg(This, hdc, value, pRect, pClipRect, pExcludeRect, pTargetRect);
                return;
            }
        }
    }

    // Non-header/sidebar elements; call original function.
    Element_PaintBg(This, hdc, value, pRect, pClipRect, pExcludeRect, pTargetRect);
}

// ---------------------------
// Windhawk entry points
// ---------------------------
typedef LONG (WINAPI *RtlGetVersionPtr)(OSVERSIONINFOEXW*);

BOOL Wh_ModInit() {
    OSVERSIONINFOEXW osvi = { sizeof(osvi) };

    HMODULE hNtdll = GetModuleHandleW(L"ntdll.dll");
    if (!hNtdll) {
        Wh_Log(L"Failed to get ntdll.dll handle.");
        return FALSE;
    }

    auto pRtlGetVersion = (RtlGetVersionPtr)GetProcAddress(hNtdll, "RtlGetVersion");
    if (!pRtlGetVersion || pRtlGetVersion(&osvi) != 0) {
        Wh_Log(L"Failed to query Windows version.");
        return FALSE;
    }

    // Log only if Windows 10 for OS context.
    if (osvi.dwMajorVersion == 10 && osvi.dwBuildNumber < 22000) {
        Wh_Log(L"Windows 10 detected (build %lu).", osvi.dwBuildNumber);
    }

    HMODULE hMod = LoadLibraryExW(L"dui70.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!hMod) {
        Wh_Log(L"Failed to load dui70.dll.");
        return FALSE;
    }

    // dui70.dll
    WindhawkUtils::SYMBOL_HOOK hook = {
        {L"public: void " SSTDCALL " DirectUI::Element::PaintBackground(struct HDC__ *,class DirectUI::Value *,struct tagRECT const &,struct tagRECT const &,struct tagRECT const &,struct tagRECT const &)"},
        &Element_PaintBg,
        Element_PaintBgHook,
        false
    };

    if (!WindhawkUtils::HookSymbols(hMod, &hook, 1)) {
        Wh_Log(L"Failed to hook DirectUI::Element::PaintBackground.");
        return FALSE;
    }

    LoadSettings();

    return TRUE;
}

void Wh_ModUninit() {}

void Wh_ModSettingsChanged() {
    LoadSettings();
}
