// ==WindhawkMod==
// @id              accent-hl-color
// @name            Accent Highlight Color
// @description     Makes the highlight color use your system accent color in Win32 apps.
// @version         1.0
// @author          FireBlade
// @github          https://github.com/FireBlade211
// @include         *
// @compilerOptions -luser32 -ldwmapi -lgdi32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Accent Highlight Color
Makes the highlight color use your system accent color in Win32 apps instead of the
default blue.

**Before**:  
![Notepad with the text "test" selected in blue](https://raw.githubusercontent.com/FireBlade211/FireBlade211/refs/heads/main/WindhawkModReadmeImages/AccentHighlightColor/before.png)

**After**:  
![Notepad with the text "test" selected in the current accent color](https://raw.githubusercontent.com/FireBlade211/FireBlade211/refs/heads/main/WindhawkModReadmeImages/AccentHighlightColor/after.png)
*/
// ==/WindhawkModReadme==

#include <dwmapi.h>
#include <cmath>
#include <atomic>

using GetSysColor_t = decltype(&GetSysColor);
using GetSysColorBrush_t = decltype(&GetSysColorBrush);
GetSysColor_t GetSysColor_Original;
GetSysColorBrush_t GetSysColorBrush_Original;

#define LL(lib) LoadLibraryExW(lib, NULL, LOAD_LIBRARY_SEARCH_SYSTEM32)

DWORD WINAPI GetSysColor_Hook(int nIndex);
HBRUSH WINAPI GetSysColorBrush_Hook(int nIndex);

// The mod is being initialized, load settings, hook functions, and do other
// initialization stuff if required.
BOOL Wh_ModInit() {
    Wh_Log(L"Init");
    
    HMODULE hUser32 = LL(L"user32.dll");
    GetSysColor_t pfnGetSysColor =
        (GetSysColor_t)GetProcAddress(hUser32,
                                                "GetSysColor");

    GetSysColorBrush_t pfnGetSysColorBrush =
        (GetSysColorBrush_t)GetProcAddress(hUser32,
                                                "GetSysColorBrush");

    Wh_SetFunctionHook((void*)pfnGetSysColor,
                       (void*)GetSysColor_Hook,
                       (void**)&GetSysColor_Original);

                       
    Wh_SetFunctionHook((void*)pfnGetSysColorBrush,
                       (void*)GetSysColorBrush_Hook,
                       (void**)&GetSysColorBrush_Original);

    return TRUE;
}

DWORD g_lastArgb = 0;
std::atomic<HBRUSH> g_highlightBrush = NULL;
std::atomic<HBRUSH> g_highlightTextBrush = NULL;
ULONGLONG g_lastUpdateTick = 0;

// The mod is being unloaded, free all allocated resources.
void Wh_ModUninit() {
    Wh_Log(L"Uninit");

    HBRUSH b1 = g_highlightBrush.load();
    HBRUSH b2 = g_highlightTextBrush.load();

    if (b1) DeleteObject(b1);
    if (b2) DeleteObject(b2);
}

// helpers to get text color from accent color
// because the actual color is only accessible in winui
double srgb_to_linear(double c)
{
    c /= 255.0;
    return (c <= 0.04045)
        ? (c / 12.92)
        : pow((c + 0.055) / 1.055, 2.4);
}

double luminance(COLORREF c)
{
    double r = srgb_to_linear(GetRValue(c));
    double g = srgb_to_linear(GetGValue(c));
    double b = srgb_to_linear(GetBValue(c));

    return 0.2126 * r + 0.7152 * g + 0.0722 * b;
}

double contrast_ratio(double l1, double l2)
{
    double lighter = (l1 > l2) ? l1 : l2;
    double darker  = (l1 > l2) ? l2 : l1;
    return (lighter + 0.05) / (darker + 0.05);
}

COLORREF GetTextColorForAccent(DWORD argb)
{
    COLORREF accent = RGB(
        (argb >> 16) & 0xFF,
        (argb >> 8)  & 0xFF,
        argb & 0xFF
    );

    double lumAccent = luminance(accent);

    double lumWhite = 1.0; // #FFFFFF
    double lumBlack = 0.0; // #000000

    double contrastWhite = contrast_ratio(lumAccent, lumWhite);
    double contrastBlack = contrast_ratio(lumAccent, lumBlack);

    return (contrastWhite > contrastBlack)
        ? RGB(255, 255, 255)
        : RGB(0, 0, 0);
}

std::atomic<COLORREF> g_cachedHighlight;
std::atomic<COLORREF> g_cachedText;

void UpdateAccentCache();

DWORD WINAPI GetSysColor_Hook(int nIndex)
{
    if (nIndex == COLOR_HIGHLIGHT || nIndex == COLOR_HIGHLIGHTTEXT)
        UpdateAccentCache();

    if (nIndex == COLOR_HIGHLIGHT)
    {
        return g_cachedHighlight.load(std::memory_order_acquire);
    }
    else if (nIndex == COLOR_HIGHLIGHTTEXT)
    {
        return g_cachedText.load(std::memory_order_acquire);
    }

    return GetSysColor_Original(nIndex);
}

// we currently don't handle WNDCLASS registration
// because it doesn't matter because I don't think
// anyone uses a window class with a highlight color
// background anyway

void UpdateAccentCache()
{
    ULONGLONG now = GetTickCount64();

    if (now - g_lastUpdateTick < 1000) // 1 second cache
        return;

    g_lastUpdateTick = now;

    DWORD argb;
    BOOL opaque;

    if (FAILED(DwmGetColorizationColor(&argb, &opaque)))
        return;

    if (argb == g_lastArgb)
        return;

    g_lastArgb = argb;

    HBRUSH newBg = CreateSolidBrush(RGB(
        (argb >> 16) & 0xFF,
        (argb >> 8)  & 0xFF,
        argb & 0xFF
    ));

    HBRUSH newText = CreateSolidBrush(GetTextColorForAccent(argb));

    g_highlightBrush.store(newBg, std::memory_order_release);
    g_highlightTextBrush.store(newText, std::memory_order_release);
    g_cachedHighlight.store(RGB(
        (argb >> 16) & 0xFF,
        (argb >> 8)  & 0xFF,
        argb & 0xFF
    ), std::memory_order_release);

    g_cachedText.store(
        GetTextColorForAccent(argb),
        std::memory_order_release
    );
}

HBRUSH WINAPI GetSysColorBrush_Hook(int nIndex)
{
    Wh_Log(L"System brush hook triggered");
    if (nIndex == COLOR_HIGHLIGHT || nIndex == COLOR_HIGHLIGHTTEXT)
        UpdateAccentCache();

    if (nIndex == COLOR_HIGHLIGHT)
    {
        Wh_Log(L"Background color is queried");
        
        HBRUSH brush = g_highlightBrush.load(std::memory_order_acquire);

        if (brush)
            return brush;
    }

    if (nIndex == COLOR_HIGHLIGHTTEXT)
    {
        Wh_Log(L"Foreground color is queried");

        HBRUSH brush = g_highlightTextBrush.load(std::memory_order_acquire);
        
        if (brush)
            return brush;
    }

    return GetSysColorBrush_Original(nIndex);
}

// // The mod setting were changed, reload them.
// void Wh_ModSettingsChanged() {
//     Wh_Log(L"SettingsChanged");

//     LoadSettings();
// }
