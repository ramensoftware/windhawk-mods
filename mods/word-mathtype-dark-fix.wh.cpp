// ==WindhawkMod==
// @id            word-mathtype-dark-fix
// @name          Word MathType Dark Mode Fix
// @name:zh-CN    Word MathType 公式深色模式修复
// @description   Fix the readability issue of MathType Equations in Word dark mode.
// @description:zh-CN 解决 Word 深色模式下 MathType 公式的可读性问题
// @version       1.0.1
// @author        Joe Ye
// @github        https://github.com/JoeYe-233
// @include       winword.exe
// @compilerOptions -lgdi32 -lshlwapi
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Word MathType Dark Mode Fix

This mod fixes the broken dark mode rendering of MathType Equations in Word and solves the unreadable black-on-black issue by intercepting the dark mode state change and dynamically adjusting GDI colors for dark mode formula rendering.

*Note: this mod needs pdb symbol of `wwlib.dll` to work. The symbol file is expected to be a bit large (~90MB in size). Windhawk will download it automatically when you launch Word first time after you installed the mod (the popup at right bottom corner of your screen, please make sure that it shows percentage like "Loading symbols... 0% (wwlib.dll)", wait until it reaches 100% and the pop up disappears, otherwise please switch your network and try again) please wait patiently and **relaunch Word AS ADMINISTRATOR at least once** after it finishes, this is to write symbols being used to SymbolCache, which speeds up launching later on.*

## ⚠️ Note:
- It is advised to **turn off automatic updates** for Office applications, as PDB may need to be downloaded every time after updates.
- Please relaunch Word as administrator *at least once* after installing the mod and wait for symbol download to complete, this is to write symbols being used to SymbolCache, which speeds up launching later on.

*This mod is part of **The Ultimate Office Dark Mode Project***

# Before-Original-After Comparison
![Before-Original-After](https://raw.githubusercontent.com/JoeYe-233/images/refs/heads/main/word-mathtype-dark-fix-before-original-after.png)
*/
// ==/WindhawkModReadme==

#include <map>
#include <algorithm>
#include <windows.h>
#include <shlwapi.h>
#include <windhawk_utils.h>
#include <atomic>

// =============================================================
// Global State & Utility Functions
// =============================================================
bool g_isDarkTheme = false;
std::atomic<bool> g_wwlibHooked{false};

bool IsPureBlack(COLORREF color) {
    return (color == RGB(0, 0, 0));
}
bool g_isInPrintPreview = false;
// =============================================================
// DarkModeState::SetDarkMode Hook
// =============================================================
#ifdef _WIN64
    #define WH_CALLCONV
    #define SYM_SetDarkMode L"?SetDarkMode@DarkModeState@@QEAAX_N0K@Z"
    #define SYM_CreatePrintPreviewHWND L"?CreatePrintPreviewHWND@PPU@@UEAAPEAUHWND__@@PEAU2@@Z"
    #define SYM_ClearPrintPreviewWwd L"?ClearPrintPreviewWwd@PPU@@UEAAXXZ"
#else
    #define WH_CALLCONV __thiscall
    #define SYM_SetDarkMode L"?SetDarkMode@DarkModeState@@QAEX_N0K@Z"
    #define SYM_CreatePrintPreviewHWND L"?CreatePrintPreviewHWND@PPU@@UAEPAUHWND__@@PAU2@@Z"
    #define SYM_ClearPrintPreviewWwd L"?ClearPrintPreviewWwd@OSU@@SGXPBUMWD@@@Z"
#endif

typedef void (WH_CALLCONV *SetDarkMode_t)(void* pThis, bool isDark, bool a2, unsigned long a3);
SetDarkMode_t pOrig_SetDarkMode = nullptr;

void WH_CALLCONV Hook_SetDarkMode(void* pThis, bool isDark, bool a2, unsigned long a3) {
    g_isDarkTheme = isDark;
    Wh_Log(L"[wwlib] SetDarkMode Intercepted! New State: %d", isDark);
    pOrig_SetDarkMode(pThis, isDark, a2, a3);
}

// -------------------------------------------------------------------------
// Print Preview State Hooks (To avoid applying dark mode colors in print preview mode, which will cause the previewed document to look messed up and unreadable)
// -------------------------------------------------------------------------
typedef HWND (WH_CALLCONV *CreatePrintPreviewHWND_t)(void* pThis, HWND hwnd);
CreatePrintPreviewHWND_t pOrig_CreatePrintPreviewHWND = nullptr;

HWND WH_CALLCONV Hook_CreatePrintPreviewHWND(void* pThis, HWND hwnd) {
    g_isInPrintPreview = true;
    return pOrig_CreatePrintPreviewHWND(pThis, hwnd);
}

typedef void (WINAPI *ClearPrintPreviewWwd_t)(void* pMwd);
ClearPrintPreviewWwd_t pOrig_ClearPrintPreviewWwd = nullptr;

void WINAPI Hook_ClearPrintPreviewWwd(void* pMwd) {
    g_isInPrintPreview = false;
    pOrig_ClearPrintPreviewWwd(pMwd);
}

void ScanAndHookWwlib() {
    HMODULE hWwlib = GetModuleHandleW(L"wwlib.dll");
    if (!hWwlib || g_wwlibHooked.exchange(true)) return;

    // wwlib.dll
    WindhawkUtils::SYMBOL_HOOK wwlibHook[] = {
        {
            { SYM_SetDarkMode },
            (void**)&pOrig_SetDarkMode,
            (void*)Hook_SetDarkMode,
            false
        },
        {
            { SYM_CreatePrintPreviewHWND },
            (void**)&pOrig_CreatePrintPreviewHWND,
            (void*)Hook_CreatePrintPreviewHWND,
            false
        },
        {
            { SYM_ClearPrintPreviewWwd },
            (void**)&pOrig_ClearPrintPreviewWwd,
            (void*)Hook_ClearPrintPreviewWwd,
            false
        }
    };

    WH_HOOK_SYMBOLS_OPTIONS options = {0};
    options.optionsSize = sizeof(options);
    options.noUndecoratedSymbols = TRUE;
    options.onlineCacheUrl = L"";        // Disable online cache to force using local symbol file, which is expected to be already downloaded by Windhawk

    Wh_Log(L"[Init] Attempting to hook wwlib.dll...");
    if (WindhawkUtils::HookSymbols(hWwlib, wwlibHook, ARRAYSIZE(wwlibHook), &options)) {
        Wh_ApplyHookOperations();
        Wh_Log(L"[Success] DarkModeState::SetDarkMode hooked successfully.");
    } else {
        Wh_Log(L"[Error] Failed to hook SetDarkMode in wwlib.dll.");
        g_wwlibHooked = false;
    }
}

bool IsDocumentDC(HDC hdc) {

    // If it's in print preview mode, we should not apply dark mode colors.
    if (g_isInPrintPreview) return false;

    // If it's a metafile DC, we should not apply dark mode colors, otherwise the exported PDF or the formula copied to clipboard will be messed up.
    DWORD dcType = GetObjectType(hdc);
    if (dcType == OBJ_ENHMETADC || dcType == OBJ_METADC) {
        return false;
    }

    // If the device is a printer, we should not apply dark mode colors, otherwise the printed document will be messed up.
    int tech = GetDeviceCaps(hdc, TECHNOLOGY);
    if (tech == DT_RASPRINTER) {
        return false;
    }

    // If the mapping mode is MM_TEXT, it's usually used for UI drawing with pure pixel coordinate, we should not apply dark mode colors to avoid messing up the UI, otherwise some UI elements will become invisible in dark mode (e.g. the text cursor in formula editor will become invisible if we apply dark mode color to it). Only when it's not MM_TEXT, it's usually used for canvas drawing with scaled coordinate system, we should apply dark mode colors to it.
    if (GetMapMode(hdc) == MM_TEXT) {
        return false;
    }

    return true;
}

// =============================================================
// Dynamic Color Calculation for Dark Mode & GDI Object Caching
// =============================================================
DWORD g_Gdi_BgColor = 0x00323232;
DWORD g_Gdi_TextColor = 0x00E0E0E0;
DWORD g_GdiPlus_BgColor = 0xFF323232;
DWORD g_GdiPlus_TextColor = 0xFFE0E0E0;

// =============================================================
// Color conversion utilities (RGB <-> HSL) and dynamic dark color calculation
// =============================================================
struct HSL { float h, s, l; };

HSL RGBtoHSL(float r, float g, float b) {
    float minVal = std::min({r, g, b});
    float maxVal = std::max({r, g, b});
    float delta = maxVal - minVal;
    HSL hsl = {0.0f, 0.0f, (maxVal + minVal) / 2.0f};

    if (delta > 0.0f) {
        hsl.s = (hsl.l < 0.5f) ? (delta / (maxVal + minVal)) : (delta / (2.0f - maxVal - minVal));
        if (maxVal == r) hsl.h = (g - b) / delta + (g < b ? 6.0f : 0.0f);
        else if (maxVal == g) hsl.h = (b - r) / delta + 2.0f;
        else hsl.h = (r - g) / delta + 4.0f;
        hsl.h /= 6.0f; // Normalize to [0,1]
    }
    return hsl;
}

float HueToRGB(float p, float q, float t) {
    if (t < 0.0f) t += 1.0f;
    if (t > 1.0f) t -= 1.0f;
    if (t < 1.0f / 6.0f) return p + (q - p) * 6.0f * t;
    if (t < 1.0f / 2.0f) return q;
    if (t < 2.0f / 3.0f) return p + (q - p) * (2.0f / 3.0f - t) * 6.0f;
    return p;
}

void ParseColors() {
    PCWSTR bgStr = Wh_GetStringSetting(L"darkBackgroundColor");
    PCWSTR textStr = Wh_GetStringSetting(L"lightTextColor");
    auto HexToRGB = [](PCWSTR hex) -> DWORD {
        if (!hex) return 0; if (hex[0] == L'#') hex++; unsigned int r, g, b;
        if (swscanf_s(hex, L"%02x%02x%02x", &r, &g, &b) == 3) return RGB(r, g, b); return 0;
    };
    if (bgStr) { g_Gdi_BgColor = HexToRGB(bgStr); Wh_FreeStringSetting(bgStr); }
    if (textStr) { g_Gdi_TextColor = HexToRGB(textStr); Wh_FreeStringSetting(textStr); }

    auto GdiToGdiPlus = [](DWORD gdi) -> DWORD { return 0xFF000000 | (GetRValue(gdi) << 16) | (GetGValue(gdi) << 8) | GetBValue(gdi); };
    g_GdiPlus_BgColor = GdiToGdiPlus(g_Gdi_BgColor);
    g_GdiPlus_TextColor = GdiToGdiPlus(g_Gdi_TextColor);
}

COLORREF GetDynamicDarkColor(COLORREF origColor) {
    // 1. Special case handling for pure black and pure white to ensure they map to the user-configured dark mode colors, improving readability and consistency.
    if (origColor == RGB(0, 0, 0)) return g_Gdi_TextColor;
    if (origColor == RGB(255, 255, 255)) return g_Gdi_BgColor;

    // 2. Convert RGB to 0.0 - 1.0 range and calculate HSL
    float r = GetRValue(origColor) / 255.0f;
    float g = GetGValue(origColor) / 255.0f;
    float b = GetBValue(origColor) / 255.0f;

    HSL hsl = RGBtoHSL(r, g, b);
    hsl.l = 1.0f - hsl.l;
    // Slightly reduce saturation to avoid the original high saturation causing an eye-catching neon effect in dark mode
    hsl.s *= 0.9f;

    // 3. Convert HSL back to RGB
    float nr, ng, nb;
    if (hsl.s <= 0.0f) {
        nr = ng = nb = hsl.l;
    } else {
        float q = hsl.l < 0.5f ? hsl.l * (1.0f + hsl.s) : hsl.l + hsl.s - hsl.l * hsl.s;
        float p = 2.0f * hsl.l - q;
        nr = HueToRGB(p, q, hsl.h + 1.0f / 3.0f);
        ng = HueToRGB(p, q, hsl.h);
        nb = HueToRGB(p, q, hsl.h - 1.0f / 3.0f);
    }

    return RGB((BYTE)(nr * 255.0f), (BYTE)(ng * 255.0f), (BYTE)(nb * 255.0f));
}

// --- GDI objects cache pool (to prevent memory/handle leaks) ---
std::map<COLORREF, HBRUSH> g_BrushCache;

struct PenKey {
    UINT style;
    LONG width;
    COLORREF color;
    bool operator<(const PenKey& o) const {
        if (style != o.style) return style < o.style;
        if (width != o.width) return width < o.width;
        return color < o.color;
    }
};
std::map<PenKey, HPEN> g_PenCache;

HBRUSH GetDarkBrush(COLORREF origColor) {
    COLORREF darkColor = GetDynamicDarkColor(origColor);
    auto it = g_BrushCache.find(darkColor);
    if (it != g_BrushCache.end()) return it->second;

    HBRUSH hb = CreateSolidBrush(darkColor);
    g_BrushCache[darkColor] = hb;
    return hb;
}

HPEN GetDarkPen(UINT style, LONG width, COLORREF origColor) {
    COLORREF darkColor = GetDynamicDarkColor(origColor);
    PenKey key = {style, width, darkColor};

    auto it = g_PenCache.find(key);
    if (it != g_PenCache.end()) return it->second;

    HPEN hp = CreatePen(style, width, darkColor);
    g_PenCache[key] = hp;
    return hp;
}


// =============================================================
// GDI+ Hooking Part (Handle OLE Object Hatch Brush for diagonal lines overlay, displayed when double clicking the formula in Word to edit)
// =============================================================
typedef DWORD ARGB;
typedef int HatchStyle;
typedef void GpHatch;
typedef int GpStatus;

typedef GpStatus (WINAPI *GdipCreateHatchBrush_t)(HatchStyle, ARGB, ARGB, GpHatch**);
GdipCreateHatchBrush_t pOrig_GdipCreateHatchBrush = nullptr;

GpStatus WINAPI Hook_GdipCreateHatchBrush(HatchStyle hatchstyle, ARGB forecol, ARGB backcol, GpHatch **brush) {
    if (g_isDarkTheme) {
        // ARGB Format: 0xAARRGGBB. 0xFF000000 is opaque pure black.
        // If the foreground color is black, force it to white to ensure visibility in dark mode.
        if (forecol == 0xFF000000) {
            forecol = 0x7FFFFFFF; // Semi-transparent white to maintain the hatch pattern visibility while ensuring it's visible on dark background
            Wh_Log(L"[GDI+] Intercepted GdipCreateHatchBrush, forced forecol to White.");
        }
    }

    return pOrig_GdipCreateHatchBrush(hatchstyle, forecol, backcol, brush);
}

void HookGdiPlus() {
    static bool s_gdiplusHooked = false;
    if (s_gdiplusHooked) return;

    HMODULE hGdiPlus = GetModuleHandleW(L"gdiplus.dll");
    if (hGdiPlus) {
        void* pFuncCreateHatch = (void*)GetProcAddress(hGdiPlus, "GdipCreateHatchBrush");
        if (pFuncCreateHatch) {
            Wh_SetFunctionHook(pFuncCreateHatch, (void*)Hook_GdipCreateHatchBrush, (void**)&pOrig_GdipCreateHatchBrush);
            Wh_ApplyHookOperations();
            s_gdiplusHooked = true;
            Wh_Log(L"[Init] GdipCreateHatchBrush hooked successfully.");
        }
    }
}

// =============================================================
// GDI Hooking Part (Handle the main canvas drawing, dynamically adjust colors for brushes/pens/text to achieve dynamic dark mode rendering)
// =============================================================
typedef HGDIOBJ (WINAPI *SelectObject_t)(HDC, HGDIOBJ);
typedef COLORREF (WINAPI *SetDCBrushColor_t)(HDC, COLORREF);
typedef COLORREF (WINAPI *SetDCPenColor_t)(HDC, COLORREF);
typedef COLORREF (WINAPI *SetTextColor_t)(HDC, COLORREF);

SelectObject_t pOrig_SelectObject = nullptr;
SetDCBrushColor_t pOrig_SetDCBrushColor = nullptr;
SetDCPenColor_t pOrig_SetDCPenColor = nullptr;
SetTextColor_t pOrig_SetTextColor = nullptr;

HBRUSH g_hWhiteBrush = nullptr;
HPEN g_hWhitePen = nullptr;

HGDIOBJ WINAPI Hook_SelectObject(HDC hdc, HGDIOBJ h) {
    if (!g_isDarkTheme || !IsDocumentDC(hdc)) return pOrig_SelectObject(hdc, h);

    // Exclude pure UI drawing by checking mapping mode, only handle canvas with scaled coordinate system
    if (GetMapMode(hdc) == MM_TEXT) return pOrig_SelectObject(hdc, h);

    DWORD type = GetObjectType(h);
    if (type == OBJ_BRUSH) {
        LOGBRUSH lb;
        // Intercept only solid brushes for inversion, pattern/shading brushes remain unchanged to preserve their intended visual effects in dark mode
        if (GetObject(h, sizeof(LOGBRUSH), &lb) && lb.lbStyle == BS_SOLID) {
            return pOrig_SelectObject(hdc, GetDarkBrush(lb.lbColor));
        }
    } else if (type == OBJ_PEN) {
        LOGPEN lp;
        if (GetObject(h, sizeof(LOGPEN), &lp)) {
            return pOrig_SelectObject(hdc, GetDarkPen(lp.lopnStyle, lp.lopnWidth.x, lp.lopnColor));
        }
    } else if (type == OBJ_EXTPEN) {
        EXTLOGPEN elp;
        if (GetObject(h, sizeof(EXTLOGPEN), &elp)) {
            return pOrig_SelectObject(hdc, GetDarkPen(elp.elpPenStyle, elp.elpWidth, elp.elpColor));
        }
    }
    return pOrig_SelectObject(hdc, h);
}

COLORREF WINAPI Hook_SetDCBrushColor(HDC hdc, COLORREF color) {
    if (g_isDarkTheme && IsDocumentDC(hdc)) color = GetDynamicDarkColor(color);
    return pOrig_SetDCBrushColor(hdc, color);
}

COLORREF WINAPI Hook_SetDCPenColor(HDC hdc, COLORREF color) {
    if (g_isDarkTheme && IsDocumentDC(hdc)) color = GetDynamicDarkColor(color);
    return pOrig_SetDCPenColor(hdc, color);
}

COLORREF WINAPI Hook_SetTextColor(HDC hdc, COLORREF color) {
    if (g_isDarkTheme && GetMapMode(hdc) != MM_TEXT && IsDocumentDC(hdc)) {
        color = GetDynamicDarkColor(color);
    }
    return pOrig_SetTextColor(hdc, color);
}
// =============================================================
// LoadLibrary Hooking Part (To ensure hooking works even if wwlib.dll and gdiplus.dll are loaded after our mod, by hooking LoadLibraryExW and LoadLibraryW to catch their loading and apply hooks immediately)
// =============================================================
typedef HMODULE (WINAPI *LoadLibraryExW_t)(LPCWSTR, HANDLE, DWORD);
typedef HMODULE (WINAPI *LoadLibraryW_t)(LPCWSTR);
LoadLibraryExW_t pOrig_LoadLibraryExW = nullptr;
LoadLibraryW_t pOrig_LoadLibraryW = nullptr;

HMODULE WINAPI Hook_LoadLibraryExW(LPCWSTR lpLibFileName, HANDLE hFile, DWORD dwFlags) {
    HMODULE hMod = pOrig_LoadLibraryExW(lpLibFileName, hFile, dwFlags);
    if (hMod && lpLibFileName) {
        if (StrStrIW(lpLibFileName, L"wwlib.dll")) {
            ScanAndHookWwlib();
        } else if (StrStrIW(lpLibFileName, L"gdiplus.dll")) {
            HookGdiPlus();
        }
    }
    return hMod;
}

HMODULE WINAPI Hook_LoadLibraryW(LPCWSTR lpLibFileName) {
    HMODULE hMod = pOrig_LoadLibraryW(lpLibFileName);
    if (hMod && lpLibFileName) {
        if (StrStrIW(lpLibFileName, L"wwlib.dll")) {
            ScanAndHookWwlib();
        } else if (StrStrIW(lpLibFileName, L"gdiplus.dll")) {
            HookGdiPlus();
        }
    }
    return hMod;
}

// =============================================================
// Windhawk Mod Lifecycle Functions
// =============================================================
BOOL Wh_ModInit() {
    Wh_Log(L"MathType Fix Loaded. Initializing...");

    g_isDarkTheme = false;

    HMODULE hGdi = LoadLibrary(L"gdi32.dll");
    if (!hGdi) return FALSE;

    g_hWhiteBrush = (HBRUSH)GetStockObject(WHITE_BRUSH);
    g_hWhitePen = (HPEN)GetStockObject(WHITE_PEN);


    void* pFuncSelect = (void*)GetProcAddress(hGdi, "SelectObject");
    if (pFuncSelect) Wh_SetFunctionHook(pFuncSelect, (void*)Hook_SelectObject, (void**)&pOrig_SelectObject);

    void* pFuncDCBrush = (void*)GetProcAddress(hGdi, "SetDCBrushColor");
    if (pFuncDCBrush) Wh_SetFunctionHook(pFuncDCBrush, (void*)Hook_SetDCBrushColor, (void**)&pOrig_SetDCBrushColor);

    void* pFuncDCPen = (void*)GetProcAddress(hGdi, "SetDCPenColor");
    if (pFuncDCPen) Wh_SetFunctionHook(pFuncDCPen, (void*)Hook_SetDCPenColor, (void**)&pOrig_SetDCPenColor);

    void* pFuncText = (void*)GetProcAddress(hGdi, "SetTextColor");
    if (pFuncText) Wh_SetFunctionHook(pFuncText, (void*)Hook_SetTextColor, (void**)&pOrig_SetTextColor);

    // Directly hook GDI+ if it's already loaded, to ensure the hatch brush issue is fixed as soon as possible, otherwise it will be fixed after the user triggers loading of GDI+ (e.g. by double clicking the formula to edit)
    if (GetModuleHandleW(L"wwlib.dll")) {
        ScanAndHookWwlib();
    }
    if (GetModuleHandleW(L"gdiplus.dll")) {
        HookGdiPlus();
    }

    HMODULE hKernel = GetModuleHandleW(L"kernelbase.dll");
    if (!hKernel) hKernel = GetModuleHandleW(L"kernel32.dll");

    if (hKernel) {
        void* pLoadLibraryExW = (void*)GetProcAddress(hKernel, "LoadLibraryExW");
        if (pLoadLibraryExW) Wh_SetFunctionHook(pLoadLibraryExW, (void*)Hook_LoadLibraryExW, (void**)&pOrig_LoadLibraryExW);

        void* pLoadLibraryW = (void*)GetProcAddress(hKernel, "LoadLibraryW");
        if (pLoadLibraryW) Wh_SetFunctionHook(pLoadLibraryW, (void*)Hook_LoadLibraryW, (void**)&pOrig_LoadLibraryW);
    }

    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L"MathType Fix Unloaded");
}