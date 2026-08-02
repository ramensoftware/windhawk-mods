// ==WindhawkMod==
// @id              win7-please-wait-restorer
// @name            Windows 7/8.1 "Please Wait" Restorer
// @description     This mod replaces the theme-switch overlay with a self-drawn classic "Please wait" box
// @version         1.0.0
// @author          babamohammed
// @github          https://github.com/babamohammed2022
// @include         rundll32.exe
// @include         explorer.exe
// @include         SystemSettings.exe
// @compilerOptions -lgdi32 -lGdiplus
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Windows 7/8.1 "Please Wait" Restorer

## About

When changing the Windows theme, this mod shows the small classic
"Please wait" box from Windows 7/8.1 instead of the modern full-screen effect.
The text is shown in the Windows display language when it is supported.

## Screenshot

![screenshot](https://raw.githubusercontent.com/babamohammed2022/babamohammed2022/main/PleaseWait.PNG)

## Notes

The mod has been tested on Windows 10 1809 (build 17763) and Windows 10 21H2 (build 19044)
Other Windows 10/11 versions may work, but are not guaranteed. If Windows
changes the internal theme component used by this mod, Windhawk disables the
mod safely instead of forcing it to run. Additionally, the mod is a best-effort implementation using
documented Windows functions where appliable to improve the stability of the mod.

## Credits
- Nex - Testing on Windows 10 21H2 and providing feedback
*/
// ==/WindhawkModReadme==

#include <windows.h>
#include <algorithm>
#include <gdiplus.h>
#include <windhawk_utils.h>

using namespace Gdiplus;

#ifdef _WIN64
#define STDCALL __cdecl
#else
#define STDCALL __stdcall
#endif

class CDimmedWindow;
typedef void(STDCALL* CDimmedWindow_OnPaint_t)(CDimmedWindow* window, HDC hdc);
static CDimmedWindow_OnPaint_t g_originalOnPaint = nullptr;
static HMODULE g_themeUi = nullptr;

struct LocalizedText {
    LANGID language;
    const wchar_t* text;
};

// These strings intentionally have no ellipsis, matching the supplied
// Windows 8.1 Italian reference ("Attendere"). Unicode escapes keep this
// source portable even when an editor saves it with a legacy code page.
static const LocalizedText kPleaseWaitTexts[] = {
    { MAKELANGID(LANG_ENGLISH, SUBLANG_DEFAULT), L"Please wait" },
    { MAKELANGID(LANG_ITALIAN, SUBLANG_DEFAULT), L"Attendere" },
    { MAKELANGID(LANG_GERMAN, SUBLANG_DEFAULT), L"Bitte warten" },
    { MAKELANGID(LANG_FRENCH, SUBLANG_DEFAULT), L"Veuillez patienter" },
    { MAKELANGID(LANG_SPANISH, SUBLANG_DEFAULT), L"Espere" },
    { MAKELANGID(LANG_PORTUGUESE, SUBLANG_DEFAULT), L"Aguarde" },
    { MAKELANGID(LANG_DUTCH, SUBLANG_DEFAULT), L"Een ogenblik geduld" },
    { MAKELANGID(LANG_POLISH, SUBLANG_DEFAULT), L"Prosz\u0119 czeka\u0107" },
    { MAKELANGID(LANG_RUSSIAN, SUBLANG_DEFAULT), L"\u041F\u043E\u0434\u043E\u0436\u0434\u0438\u0442\u0435" },
    { MAKELANGID(LANG_UKRAINIAN, SUBLANG_DEFAULT), L"\u0417\u0430\u0447\u0435\u043A\u0430\u0439\u0442\u0435" },
    { MAKELANGID(LANG_TURKISH, SUBLANG_DEFAULT), L"L\u00FCtfen bekleyin" },
    { MAKELANGID(LANG_SWEDISH, SUBLANG_DEFAULT), L"V\u00E4nta" },
    { MAKELANGID(LANG_NORWEGIAN, SUBLANG_DEFAULT), L"Vent litt" },
    { MAKELANGID(LANG_DANISH, SUBLANG_DEFAULT), L"Vent venligst" },
    { MAKELANGID(LANG_FINNISH, SUBLANG_DEFAULT), L"Odota" },
    { MAKELANGID(LANG_CZECH, SUBLANG_DEFAULT), L"\u010Cekejte pros\u00EDm" },
    { MAKELANGID(LANG_SLOVAK, SUBLANG_DEFAULT), L"Pros\u00EDm, \u010Dakajte" },
    { MAKELANGID(LANG_HUNGARIAN, SUBLANG_DEFAULT), L"K\u00E9rem, v\u00E1rjon" },
    { MAKELANGID(LANG_ROMANIAN, SUBLANG_DEFAULT), L"V\u0103 rug\u0103m a\u0219tepta\u021Bi" },
    { MAKELANGID(LANG_GREEK, SUBLANG_DEFAULT), L"\u03A0\u03B1\u03C1\u03B1\u03BA\u03B1\u03BB\u03CE \u03C0\u03B5\u03C1\u03B9\u03BC\u03AD\u03BD\u03B5\u03C4\u03B5" },
    { MAKELANGID(LANG_ARABIC, SUBLANG_DEFAULT), L"\u064A\u0631\u062C\u0649 \u0627\u0644\u0627\u0646\u062A\u0638\u0627\u0631" },
    { MAKELANGID(LANG_HEBREW, SUBLANG_DEFAULT), L"\u05E0\u05D0 \u05D4\u05DE\u05EA\u05DF" },
    { MAKELANGID(LANG_JAPANESE, SUBLANG_DEFAULT), L"\u3057\u3070\u3089\u304F\u304A\u5F85\u3061\u304F\u3060\u3055\u3044" },
    { MAKELANGID(LANG_KOREAN, SUBLANG_DEFAULT), L"\uC7A0\uC2DC \uAE30\uB2E4\uB824 \uC8FC\uC2ED\uC2DC\uC624" },
    { MAKELANGID(LANG_CHINESE_SIMPLIFIED, SUBLANG_DEFAULT), L"\u8BF7\u7A0D\u5019" },
    { MAKELANGID(LANG_CHINESE_TRADITIONAL, SUBLANG_DEFAULT), L"\u8ACB\u7A0D\u5019" },
    { MAKELANGID(LANG_CROATIAN, SUBLANG_DEFAULT), L"Pri\u010Dekajte" },
    { MAKELANGID(LANG_VIETNAMESE, SUBLANG_DEFAULT), L"Vui l\u00F2ng ch\u1EDD" },
    { MAKELANGID(LANG_INDONESIAN, SUBLANG_DEFAULT), L"Harap tunggu" },
    { MAKELANGID(LANG_BULGARIAN, SUBLANG_DEFAULT), L"\u041C\u043E\u043B\u044F, \u0438\u0437\u0447\u0430\u043A\u0430\u0439\u0442\u0435" },
};

static const wchar_t* GetPleaseWaitText()
{
    const WORD primaryLanguage = PRIMARYLANGID(GetUserDefaultUILanguage());
    for (const auto& entry : kPleaseWaitTexts) {
        if (PRIMARYLANGID(entry.language) == primaryLanguage) {
            return entry.text;
        }
    }
    return L"Please wait";
}

// RAII wrappers make all early returns safe: each acquired Win32 handle is
// released at scope exit, including when a C++ exception is caught above it.
class ScopedGdiObject {
public:
    explicit ScopedGdiObject(HGDIOBJ object = nullptr) : object_(object) {}
    ~ScopedGdiObject() { if (object_) DeleteObject(object_); }
    ScopedGdiObject(const ScopedGdiObject&) = delete;
    ScopedGdiObject& operator=(const ScopedGdiObject&) = delete;
    HGDIOBJ get() const { return object_; }
    explicit operator bool() const { return object_ != nullptr; }
private:
    HGDIOBJ object_;
};

class ScopedCompatibleDC {
public:
    explicit ScopedCompatibleDC(HDC referenceDc)
        : dc_(referenceDc ? CreateCompatibleDC(referenceDc) : nullptr) {}
    ~ScopedCompatibleDC() { if (dc_) DeleteDC(dc_); }
    ScopedCompatibleDC(const ScopedCompatibleDC&) = delete;
    ScopedCompatibleDC& operator=(const ScopedCompatibleDC&) = delete;
    HDC get() const { return dc_; }
    explicit operator bool() const { return dc_ != nullptr; }
private:
    HDC dc_;
};

class ScopedScreenDC {
public:
    ScopedScreenDC() : dc_(GetDC(nullptr)) {}
    ~ScopedScreenDC() { if (dc_) ReleaseDC(nullptr, dc_); }
    ScopedScreenDC(const ScopedScreenDC&) = delete;
    ScopedScreenDC& operator=(const ScopedScreenDC&) = delete;
    HDC get() const { return dc_; }
    explicit operator bool() const { return dc_ != nullptr; }
private:
    HDC dc_;
};

class ScopedModule {
public:
    explicit ScopedModule(HMODULE module = nullptr) : module_(module) {}
    ~ScopedModule() { if (module_) FreeLibrary(module_); }
    ScopedModule(const ScopedModule&) = delete;
    ScopedModule& operator=(const ScopedModule&) = delete;
    HMODULE get() const { return module_; }
    HMODULE release() {
        HMODULE result = module_;
        module_ = nullptr;
        return result;
    }
    explicit operator bool() const { return module_ != nullptr; }
private:
    HMODULE module_;
};

class ScopedGdiplus {
public:
    ScopedGdiplus() : token_(0), ready_(false) {}
    ~ScopedGdiplus() { Stop(); }
    ScopedGdiplus(const ScopedGdiplus&) = delete;
    ScopedGdiplus& operator=(const ScopedGdiplus&) = delete;

    bool Start() {
        if (ready_) return true;
        GdiplusStartupInput startupInput;
        ready_ = GdiplusStartup(&token_, &startupInput, nullptr) == Ok;
        return ready_;
    }
    void Stop() {
        if (ready_) {
            GdiplusShutdown(token_);
            token_ = 0;
            ready_ = false;
        }
    }
    bool ready() const { return ready_; }
private:
    ULONG_PTR token_;
    bool ready_;
};

static ScopedGdiplus g_gdiplus;

class ScopedSelection {
public:
    ScopedSelection(HDC dc, HGDIOBJ object) : dc_(dc), old_(HGDI_ERROR) {
        if (dc_ && object) old_ = SelectObject(dc_, object);
    }
    ~ScopedSelection() {
        if (dc_ && old_ && old_ != HGDI_ERROR) SelectObject(dc_, old_);
    }
    ScopedSelection(const ScopedSelection&) = delete;
    ScopedSelection& operator=(const ScopedSelection&) = delete;
    bool selected() const { return old_ && old_ != HGDI_ERROR; }
private:
    HDC dc_;
    HGDIOBJ old_;
};

// Restore the previous wider proportions (216 × 76 px), then enlarge every
// part by 5%, including the font, borders and spacing: 227 × 80 px at 96 DPI.
static constexpr int kReferenceBoxWidth = 216;
static constexpr int kReferenceBoxHeight = 76;
static constexpr int kBoxSizePercent = 105;
// User-selected visible-but-moderate desaturation. Brightness stays nearly
// unchanged while the desktop colours are reduced by 25%.
static constexpr REAL kBackgroundDesaturation = 0.25f;

static int ScaleForDpi(int pixelsAt96Dpi, int dpi)
{
    return MulDiv(pixelsAt96Dpi, dpi > 0 ? dpi : 96, 96);
}

static int ScaleBoxForDpi(int pixelsAt96Dpi, int dpi)
{
    return MulDiv(ScaleForDpi(pixelsAt96Dpi, dpi), kBoxSizePercent, 100);
}

// Windows 7 displayed one message on the primary monitor. On builds where
// themeui paints one virtual-desktop surface, convert the primary monitor's
// screen coordinates to the surface's coordinates. This also keeps the box
// out of the gap between monitors with different positions/resolutions.
static RECT GetClassicBoxArea(const RECT& paintArea)
{
    const int paintWidth = paintArea.right - paintArea.left;
    const int paintHeight = paintArea.bottom - paintArea.top;
    const int virtualWidth = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    const int virtualHeight = GetSystemMetrics(SM_CYVIRTUALSCREEN);

    if (paintWidth != virtualWidth || paintHeight != virtualHeight) {
        // This build supplied a monitor-sized paint surface. It is already
        // the correct coordinate space for the box.
        return paintArea;
    }

    const POINT primaryOrigin = { 0, 0 };
    HMONITOR primaryMonitor = MonitorFromPoint(primaryOrigin, MONITOR_DEFAULTTOPRIMARY);
    MONITORINFO monitorInfo = {};
    monitorInfo.cbSize = sizeof(monitorInfo);
    if (!primaryMonitor || !GetMonitorInfoW(primaryMonitor, &monitorInfo)) {
        return paintArea;
    }

    const int virtualLeft = GetSystemMetrics(SM_XVIRTUALSCREEN);
    const int virtualTop = GetSystemMetrics(SM_YVIRTUALSCREEN);
    return {
        paintArea.left + monitorInfo.rcMonitor.left - virtualLeft,
        paintArea.top + monitorInfo.rcMonitor.top - virtualTop,
        paintArea.left + monitorInfo.rcMonitor.right - virtualLeft,
        paintArea.top + monitorInfo.rcMonitor.bottom - virtualTop
    };
}

// The failed first implementation rendered GDI+ directly to CDimmedWindow's
// paint DC. That DC may be transparent on current builds. This version keeps
// the same reliable off-screen snapshot/composite/BitBlt design used by Aero
// Flip 3D Recreation, but uses a true saturation colour matrix instead of its
// black veil.
static void PaintSnapshotWithGentleDesaturation(HDC hdc, const RECT& area)
{
    const int width = area.right - area.left;
    const int height = area.bottom - area.top;
    if (!g_gdiplus.ready() || !hdc || width <= 0 || height <= 0) return;

    ScopedScreenDC screenDc;
    if (!screenDc) return;

    ScopedCompatibleDC snapshotDc(screenDc.get());
    ScopedCompatibleDC compositeDc(hdc);
    if (!snapshotDc || !compositeDc) return;

    ScopedGdiObject snapshotBitmap(CreateCompatibleBitmap(screenDc.get(), width, height));
    ScopedGdiObject compositeBitmap(CreateCompatibleBitmap(hdc, width, height));
    if (!snapshotBitmap || !compositeBitmap) return;

    // Selection objects are declared after their bitmaps, so they restore the
    // old stock bitmaps before the bitmap destructors run.
    ScopedSelection selectSnapshot(snapshotDc.get(), snapshotBitmap.get());
    ScopedSelection selectComposite(compositeDc.get(), compositeBitmap.get());
    if (!selectSnapshot.selected() || !selectComposite.selected()) return;

    const int screenX = GetSystemMetrics(SM_XVIRTUALSCREEN) + area.left;
    const int screenY = GetSystemMetrics(SM_YVIRTUALSCREEN) + area.top;
    if (!BitBlt(snapshotDc.get(), 0, 0, width, height, screenDc.get(),
                screenX, screenY, SRCCOPY | CAPTUREBLT)) {
        return;
    }

    Bitmap sourceImage(static_cast<HBITMAP>(snapshotBitmap.get()), nullptr);
    if (sourceImage.GetLastStatus() != Ok) return;

    const REAL i = kBackgroundDesaturation;
    const ColorMatrix matrix = {{
        { 1.0f - 0.70f * i, 0.30f * i,        0.30f * i,        0, 0 },
        { 0.59f * i,        1.0f - 0.41f * i, 0.59f * i,        0, 0 },
        { 0.11f * i,        0.11f * i,        1.0f - 0.89f * i, 0, 0 },
        { 0,                 0,                0,                1, 0 },
        { 0,                 0,                0,                0, 1 }
    }};

    ImageAttributes attributes;
    if (attributes.SetColorMatrix(&matrix, ColorMatrixFlagsDefault,
                                  ColorAdjustTypeBitmap) != Ok) {
        return;
    }

    // Crucially, GDI+ now draws into an ordinary memory DC, not into the
    // potentially transparent theme overlay DC.
    Graphics graphics(compositeDc.get());
    if (graphics.GetLastStatus() != Ok ||
        graphics.DrawImage(&sourceImage, Rect(0, 0, width, height),
                           0, 0, width, height, UnitPixel, &attributes) != Ok) {
        return;
    }

    // This opaque copy is reliable even when the target overlay does not
    // support direct GDI+/alpha composition.
    BitBlt(hdc, area.left, area.top, width, height, compositeDc.get(),
           0, 0, SRCCOPY);
}

static void DrawClassicPleaseWaitBox(HDC hdc)
{
    if (!hdc) return;

    // On this paint DC, the clipping rectangle is the complete overlay client
    // area on supported builds. Unlike the old code, no HWND/object-layout
    // offset is dereferenced to obtain it.
    RECT paintArea = {};
    if (GetClipBox(hdc, &paintArea) == ERROR ||
        paintArea.right <= paintArea.left || paintArea.bottom <= paintArea.top) {
        return;
    }

    // This off-screen snapshot/colour-matrix composition is synchronous. If
    // it fails, it safely skips the backdrop and still draws the classic box.
    PaintSnapshotWithGentleDesaturation(hdc, paintArea);

    const RECT boxArea = GetClassicBoxArea(paintArea);
    const int dpi = GetDeviceCaps(hdc, LOGPIXELSX);
    const int outerWidth = ScaleBoxForDpi(kReferenceBoxWidth, dpi);   // 227 px at 96 DPI
    const int outerHeight = ScaleBoxForDpi(kReferenceBoxHeight, dpi); // 80 px at 96 DPI
    const int rim = std::max(1, ScaleBoxForDpi(5, dpi));
    const int border = std::max(1, ScaleBoxForDpi(1, dpi));

    RECT outer = {
        boxArea.left + ((boxArea.right - boxArea.left) - outerWidth) / 2,
        boxArea.top + ((boxArea.bottom - boxArea.top) - outerHeight) / 2,
        0, 0
    };
    outer.right = outer.left + outerWidth;
    outer.bottom = outer.top + outerHeight;

    RECT darkBorder = outer;
    InflateRect(&darkBorder, -rim, -rim);
    RECT content = darkBorder;
    InflateRect(&content, -border, -border);
    if (content.right <= content.left || content.bottom <= content.top) return;

    // Colours sampled/approximated from the supplied Windows 8.1 reference:
    // pale-blue outer rim, slate-grey one-pixel line, white content.
    ScopedGdiObject outerBrush(CreateSolidBrush(RGB(197, 218, 231)));
    ScopedGdiObject borderBrush(CreateSolidBrush(RGB(118, 131, 139)));
    ScopedGdiObject contentBrush(CreateSolidBrush(RGB(255, 255, 255)));
    if (!outerBrush || !borderBrush || !contentBrush) return;
    FillRect(hdc, &outer, static_cast<HBRUSH>(outerBrush.get()));

    // The original frame was not a flat blue fill: it had faint horizontal
    // Aero-like light streaks. Keep them inside the outer rim, so they never
    // reduce the white text area or the dark inner border's contrast.
    ScopedGdiObject topStreakBrush(CreateSolidBrush(RGB(231, 241, 247)));
    ScopedGdiObject bottomStreakBrush(CreateSolidBrush(RGB(177, 208, 225)));
    const int streakHeight = std::max(1, ScaleBoxForDpi(1, dpi));
    if (topStreakBrush && bottomStreakBrush && rim > streakHeight) {
        RECT topStreak = { outer.left + 1, outer.top + 1,
                           outer.right - 1, outer.top + 1 + streakHeight };
        RECT bottomStreak = { outer.left + 1, outer.bottom - 1 - streakHeight,
                              outer.right - 1, outer.bottom - 1 };
        FillRect(hdc, &topStreak, static_cast<HBRUSH>(topStreakBrush.get()));
        FillRect(hdc, &bottomStreak, static_cast<HBRUSH>(bottomStreakBrush.get()));
    }

    FillRect(hdc, &darkBorder, static_cast<HBRUSH>(borderBrush.get()));
    FillRect(hdc, &content, static_cast<HBRUSH>(contentBrush.get()));

    LOGFONTW fontDescription = {};
    fontDescription.lfHeight = -ScaleBoxForDpi(12, dpi);
    fontDescription.lfWeight = FW_BOLD;
    fontDescription.lfQuality = CLEARTYPE_QUALITY;
    fontDescription.lfCharSet = DEFAULT_CHARSET;
    lstrcpynW(fontDescription.lfFaceName, L"Segoe UI", LF_FACESIZE);
    ScopedGdiObject font(CreateFontIndirectW(&fontDescription));
    if (!font) return;

    ScopedSelection selectFont(hdc, font.get());
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, RGB(0, 0, 0));
    DrawTextW(hdc, GetPleaseWaitText(), -1, &content,
              DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
}

void STDCALL CDimmedWindow_OnPaintHook(CDimmedWindow* window, HDC hdc)
{
    // This mod deliberately replaces the original overlay paint. Calling the
    // original here would restore the full-screen Windows 10/11 dimming that
    // the mod is intended to remove.
    (void)window;
    if (!hdc) return;

    try {
        DrawClassicPleaseWaitBox(hdc);
    }
    catch (...) {
        // A rendering failure must never bring down Explorer, rundll32 or
        // Settings. RAII objects created by the drawing path are unwound here.
        Wh_Log(L"Please Wait Restorer: an unexpected C++ exception occurred while drawing; this frame was skipped safely");
    }
}

BOOL Wh_ModInit()
{
    Wh_Log(L"Please Wait Restorer 1.0.0: initializing safe classic-overlay mode");

    try {
        // The box remains available if documented GDI+ colour adjustment is
        // unavailable; only the optional desaturation is skipped.
        if (!g_gdiplus.Start()) {
            Wh_Log(L"Please Wait Restorer: GDI+ unavailable; continuing without desaturation");
        }

        // If initialization returns early or HookSymbols fails, this wrapper
        // releases the reference acquired by LoadLibrary automatically.
        ScopedModule themeUi(LoadLibraryW(L"themeui.dll"));
        if (!themeUi) {
            Wh_Log(L"themeui.dll could not be loaded; disabling the mod safely");
            return FALSE;
        }

        // themeui.dll
        WindhawkUtils::SYMBOL_HOOK themeUiDllHooks[] = {
            {
                { L"private: void __cdecl CDimmedWindow::OnPaint(struct HDC__ *)" },
                &g_originalOnPaint,
                CDimmedWindow_OnPaintHook,
                false
            }
        };

        if (!WindhawkUtils::HookSymbols(themeUi.get(), themeUiDllHooks, ARRAYSIZE(themeUiDllHooks))) {
            Wh_Log(L"CDimmedWindow::OnPaint was not resolved on this Windows build; disabling the mod safely");
            return FALSE;
        }

        // Keep the module loaded for the host process. This avoids unloading
        // code that may still be used by the hooked Windows component.
        g_themeUi = themeUi.release();
        return TRUE;
    }
    catch (...) {
        Wh_Log(L"Please Wait Restorer: an unexpected C++ exception occurred during initialization; disabling safely");
        return FALSE;
    }
}

void Wh_ModUninit()
{
    // Windhawk removes the hook before unloading the mod. Do not free
    // themeui.dll here: it is a Windows module that can still be in use by
    // the host process, and keeping its normal process lifetime is safest.
    g_gdiplus.Stop();
    Wh_Log(L"Please Wait Restorer: unloaded");
}
