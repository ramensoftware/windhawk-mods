// ==WindhawkMod==
// @id           win7-please-wait-restorer
// @name         Windows 7/8.1 "Please Wait" Restorer
// @description  This mod replaces the theme-switch overlay with the classic "Please wait" box from Windows 7/8.1.
// @version      1.0.0
// @author       babamohammed 
// @github       https://github.com/babamohammed2022
// @include      rundll32.exe
// @include      explorer.exe
// @include      SystemSettings.exe
// @compilerOptions -lgdi32 -lshcore -lmsimg32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Windows 7/8.1 "Please Wait" Restorer

## About

This mod shows the small classic "Please wait" box from Windows 7/8.1 while the
system changes the theme, instead of the modern full-screen overlay. The
built-in text is localized for several common display languages while unsupported
languages use English.

## Screenshot

![screenshot](https://raw.githubusercontent.com/babamohammed2022/babamohammed2022/main/PleaseWait.PNG)

## Notes

The mod has been tested on Windows 10 1809 and Windows 10 21H2.
Additionally, the mod only attaches when `themeui.dll` is actually loaded by one of the
included processes. If the private themeui symbol is unavailable on a Windows
build, the mod remains inactive in that process.
For specific setups (such as unsupported language), it is recommended to apply the required changes using the mod's settings.

## Known Limitations

On legacy Windows 10 builds (particularly version 1809 and prior), altering the display DPI scaling while the mod is running can result in slight rendering anomalies. For instance, the captured backdrop or classic box may appear misaligned until a subsequent theme change occurs. To ensure full restoration of proper rendering, it is advisable to restart explorer.exe or perform a user logoff/logon cycle following any DPI modification.
## Credits

- Nex — Testing on Windows 10 21H2 and providing feedback
*/
// ==/WindhawkModReadme==


// ==WindhawkModSettings==
/*
- desaturationPercent: 65
  $name: Background desaturation (%)
  $description: This setting changes the amount of color removed from the captured desktop background.
- desaturationRampMs: 500
  $name: Desaturation ramp (ms)
  $description: Duration of the progressive desaturation animation, like in Windows 7. Set to 0 to apply the desaturation instantly.
- boxSizePercent: 105
  $name: Box size (%)
  $description: This setting changes the size of the classic box, its borders, and its decorative details.
- fontSizePercent: 115
  $name: Text size (%)
  $description: This setting changes the size of the bold system message font used for the caption.
- customText: ""
  $name: Custom text
  $description: This setting serves as an optional replacement for the localized caption. Leave it empty to use the Windows display language.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <shellscalingapi.h>
#include <windhawk_utils.h>
#include <algorithm>
#include <atomic>
#include <cmath>
#include <mutex>

#ifdef _WIN64
#define THISCALL _cdecl
#define STHICALL L"__cdecl"
#else
#define THISCALL __thiscall
#define STHICALL L"__thiscall"
#endif

class CDimmedWindow;
using CDimmedWindow_OnPaint_t = void(THISCALL*)(CDimmedWindow*, HDC);
static CDimmedWindow_OnPaint_t g_originalOnPaint = nullptr;

static std::atomic<bool> g_themeUiHandled = false;
static std::mutex g_settingsMutex;

struct Settings {
    std::atomic<int> desaturationPercent;
    std::atomic<int> desaturationRampMs;
    std::atomic<int> boxSizePercent;
    std::atomic<int> fontSizePercent;
    wchar_t customText[256];
    std::atomic<bool> customTextValid;
};

static Settings g_settings = { 65, 500, 105, 115, L"", false };

// ---------------------------------------------------------------------------
// Cached backdrop: captured ONCE with the SAME coordinate semantics as the
// original (origin = ClientToScreen(0,0), size = client). Paint always uses
// this cache and never recaptures the screen (which would contain the previous
// frame's box -> cascading trails). No coordinate space conversion and no
// resizing: rendering is identical at every DPI with no zoom artifacts.
//
// Desaturation is progressive, like in Windows 7: starts from the pristine
// capture and increases over time to the chosen value (desaturationRampMs
// controls the duration). The cache records the DPI and window it was built
// for: if DPI changes during execution it is rebuilt immediately without
// ambiguity.
// ---------------------------------------------------------------------------
struct BackdropCache {
    HBITMAP    rawBitmap   = nullptr;  // raw capture, never desaturated
    HDC        rawDc       = nullptr;  // memory DC with rawBitmap selected
    HBITMAP    grayBitmap  = nullptr;  // fully desaturated copy, built once at capture time
    HDC        grayDc      = nullptr;  // memory DC with grayBitmap selected
    HBITMAP    compBitmap  = nullptr;  // offscreen compositing buffer (backdrop + box)
    HDC        compDc      = nullptr;  // memory DC with compBitmap selected
    RECT       size        = { 0, 0, 0, 0 };  // captured size (= client)
    POINT      origin      = { 0, 0 };  // capture origin (ClientToScreen of 0,0)
    UINT       dpi         = 0;         // effective monitor DPI at capture time
    HWND       hwnd        = nullptr;   // window the capture belongs to
    ULONGLONG  startTime   = 0;         // start of the desaturation ramp
    bool       valid       = false;
};
static BackdropCache g_backdropCache;
static std::recursive_mutex g_cacheMutex;

static void FreeBackdropCache() {
    std::lock_guard<std::recursive_mutex> lock(g_cacheMutex);
    if (g_backdropCache.rawDc) DeleteDC(g_backdropCache.rawDc);
    if (g_backdropCache.rawBitmap) DeleteObject(g_backdropCache.rawBitmap);
    if (g_backdropCache.grayDc) DeleteDC(g_backdropCache.grayDc);
    if (g_backdropCache.grayBitmap) DeleteObject(g_backdropCache.grayBitmap);
    if (g_backdropCache.compDc) DeleteDC(g_backdropCache.compDc);
    if (g_backdropCache.compBitmap) DeleteObject(g_backdropCache.compBitmap);
    g_backdropCache = {};
}


static void LoadSettings() {
    std::lock_guard<std::mutex> lock(g_settingsMutex);
    constexpr int kDefaultDesaturationPercent = 65;
    
    int desaturationPercent = Wh_GetIntSetting(L"desaturationPercent");
    g_settings.desaturationPercent.store(
        desaturationPercent >= 0 && desaturationPercent <= 100
            ? desaturationPercent
            : kDefaultDesaturationPercent
    );
    
    // REMOVED: InvalidateBackdropCache() call - it caused a lock-order inversion
    // with g_cacheMutex and isn't needed because desaturationPercent doesn't
    // affect the cache contents (desaturation is applied per-frame via AlphaBlend).
    
    g_settings.desaturationRampMs.store(std::clamp(Wh_GetIntSetting(L"desaturationRampMs"), 0, 10000));
    g_settings.boxSizePercent.store(std::clamp(Wh_GetIntSetting(L"boxSizePercent"), 50, 200));
    g_settings.fontSizePercent.store(std::clamp(Wh_GetIntSetting(L"fontSizePercent"), 50, 200));

    PCWSTR customText = Wh_GetStringSetting(L"customText");
    lstrcpynW(g_settings.customText, customText, ARRAYSIZE(g_settings.customText));
    g_settings.customTextValid.store(g_settings.customText[0] != L'\0');
    Wh_FreeStringSetting(customText);
}

struct LocalizedText {
    LANGID language;
    const wchar_t* text;
};

static const LocalizedText kPleaseWaitTexts[] = {
    { MAKELANGID(LANG_ENGLISH, SUBLANG_DEFAULT), L"Please wait" },
    { MAKELANGID(LANG_ITALIAN, SUBLANG_DEFAULT), L"Attendere" },
    { MAKELANGID(LANG_GERMAN, SUBLANG_DEFAULT), L"Bitte warten" },
    { MAKELANGID(LANG_FRENCH, SUBLANG_DEFAULT), L"Veuillez patienter" },
    { MAKELANGID(LANG_SPANISH, SUBLANG_DEFAULT), L"Espere" },
    { MAKELANGID(LANG_PORTUGUESE, SUBLANG_DEFAULT), L"Aguarde" },
    { MAKELANGID(LANG_DUTCH, SUBLANG_DEFAULT), L"Een ogenblik geduld" },
    { MAKELANGID(LANG_POLISH, SUBLANG_DEFAULT), L"Prosz\u0119 czeka\u0107" },
    { MAKELANGID(LANG_RUSSIAN, SUBLANG_DEFAULT), L"\u041f\u043e\u0434\u043e\u0436\u0434\u0438\u0442\u0435" },
    { MAKELANGID(LANG_JAPANESE, SUBLANG_DEFAULT), L"\u3057\u3070\u3089\u304a\u5f85\u3061\u304f\u3060\u3055\u3044" },
    { MAKELANGID(LANG_KOREAN, SUBLANG_DEFAULT), L"\uc7a0\uc2dc \uae30\ub2e4\ub824 \uc8fc\uc2ed\uc2dc\uc624" },
    { MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_SIMPLIFIED), L"\u8bf7\u7a0d\u5019" },
    { MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_TRADITIONAL), L"\u8acb\u7a0d\u5019" },
    { MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_HONGKONG), L"\u8acb\u7a0d\u5019" },
};

static void GetPleaseWaitText(wchar_t (&out)[ARRAYSIZE(Settings::customText)]) {
    std::lock_guard<std::mutex> lock(g_settingsMutex);
    
    if (g_settings.customText[0]) {
        lstrcpynW(out, g_settings.customText, ARRAYSIZE(out));
        return;
    }
    
    const LANGID uiLanguage = GetUserDefaultUILanguage();
    for (const auto& entry : kPleaseWaitTexts) {
        if (entry.language == uiLanguage) {
            lstrcpynW(out, entry.text, ARRAYSIZE(out));
            return;
        }
    }
    for (const auto& entry : kPleaseWaitTexts) {
        if (PRIMARYLANGID(entry.language) == PRIMARYLANGID(uiLanguage)) {
            lstrcpynW(out, entry.text, ARRAYSIZE(out));
            return;
        }
    }
    lstrcpynW(out, L"Please wait", ARRAYSIZE(out));
}
class ScopedGdiObject {
public:
    explicit ScopedGdiObject(HGDIOBJ object = nullptr) : object_(object) {}
    ~ScopedGdiObject() { if (object_) DeleteObject(object_); }
    ScopedGdiObject(const ScopedGdiObject&) = delete;
    HGDIOBJ get() const { return object_; }
    explicit operator bool() const { return object_ != nullptr; }
private:
    HGDIOBJ object_;
};

class ScopedSelection {
public:
    ScopedSelection(HDC dc, HGDIOBJ object) : dc_(dc), old_(dc && object ? SelectObject(dc, object) : HGDIOBJ(HGDI_ERROR)) {}
    ~ScopedSelection() { if (dc_ && old_ && old_ != HGDI_ERROR) SelectObject(dc_, old_); }
    bool selected() const { return old_ && old_ != HGDI_ERROR; }
private:
    HDC dc_;
    HGDIOBJ old_;
};

static int ScaleForDpi(int value, UINT dpi) {
    return MulDiv(value, dpi ? dpi : 96, 96);
}

static int ScaleBoxForDpi(int value, UINT dpi) {
    return MulDiv(ScaleForDpi(value, dpi), g_settings.boxSizePercent.load(), 100);
}

static UINT GetPrimaryMonitorDpi() {
    const POINT origin = { 0, 0 };
    HMONITOR monitor = MonitorFromPoint(origin, MONITOR_DEFAULTTOPRIMARY);
    UINT dpiX = 96, dpiY = 96;
    if (monitor) GetDpiForMonitor(monitor, MDT_EFFECTIVE_DPI, &dpiX, &dpiY);
    return dpiX;
}

// Origin of the screen region to capture: ClientToScreen of the client corner,
// exactly as in the original mod code. No coordinate space conversion and no
// resizing: rendering stays identical to the original at every DPI with no
// zoom; the cache eliminates trails entirely.
static bool GetBackdropOrigin(HWND hwnd, POINT* originOut) {
    POINT origin = { 0, 0 };
    if (!ClientToScreen(hwnd, &origin))
        return false;
    if (originOut)
        *originOut = origin;
    return true;
}

// Effective DPI of the monitor containing the window (96 = 100% scale).
// Independent of the process DPI-awareness level: this is the user's true
// scale setting, used to validate the cache.
static UINT GetMonitorDpiForWindow(HWND hwnd) {
    HMONITOR monitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
    UINT dpiX = 96, dpiY = 96;
    if (monitor) GetDpiForMonitor(monitor, MDT_EFFECTIVE_DPI, &dpiX, &dpiY);
    return dpiX;
}

// DPI to use for DRAWING in the window, consistent with the process
// DPI-awareness level: for DPI-unaware processes returns 96 (DC units are
// logical and the system scales the result), for aware processes the real
// DPI of the monitor containing the window.
static UINT GetWindowDpi(HWND hwnd) {
    typedef UINT(WINAPI* GetDpiForWindow_t)(HWND);
    static const GetDpiForWindow_t getDpiForWindow =
        (GetDpiForWindow_t)GetProcAddress(GetModuleHandleW(L"user32.dll"), "GetDpiForWindow");
    if (getDpiForWindow) {
        const UINT dpi = getDpiForWindow(hwnd);
        if (dpi != 0)
            return dpi;
    }
    return GetPrimaryMonitorDpi();
}



// Captures the screen in the client area ONCE, with the SAME semantics as the
// original (origin = ClientToScreen(0,0), size = client): rendering identical
// to the original at every DPI, without zoom. The capture stays PRISTINE:
// progressive desaturation is applied per-frame via AlphaBlend between the raw
// capture and the fully desaturated copy, always starting from this clean base
// (no cascading trails).
static bool BuildBackdropCache(HWND hwnd, const RECT& clientRect, UINT dpi) {
    POINT origin = {};
    if (!GetBackdropOrigin(hwnd, &origin))
        return false;
    // Capture size = client DC size, as in the original: the final blit is
    // always 1:1 (no zoom).
    const int width = clientRect.right - clientRect.left;
    const int height = clientRect.bottom - clientRect.top;
    if (width <= 0 || height <= 0)
        return false;

    std::lock_guard<std::recursive_mutex> lock(g_cacheMutex);

    // Free the old cache.
    if (g_backdropCache.rawDc) DeleteDC(g_backdropCache.rawDc);
    if (g_backdropCache.rawBitmap) DeleteObject(g_backdropCache.rawBitmap);
    if (g_backdropCache.grayDc) DeleteDC(g_backdropCache.grayDc);
    if (g_backdropCache.grayBitmap) DeleteObject(g_backdropCache.grayBitmap);
    if (g_backdropCache.compDc) DeleteDC(g_backdropCache.compDc);
    if (g_backdropCache.compBitmap) DeleteObject(g_backdropCache.compBitmap);
    g_backdropCache = {};

    HDC screenDC = GetDC(nullptr);
    if (!screenDC)
        return false;

    BITMAPINFO bmi = {};
    bmi.bmiHeader.biSize = sizeof(bmi.bmiHeader);
    bmi.bmiHeader.biWidth = width;
    bmi.bmiHeader.biHeight = -height;  // top-down, 32-bit BGR DIB
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    void* rawPixels = nullptr;
    HBITMAP rawDib = CreateDIBSection(screenDC, &bmi, DIB_RGB_COLORS, &rawPixels, nullptr, 0);
    void* grayPixels = nullptr;
    HBITMAP grayDib = CreateDIBSection(screenDC, &bmi, DIB_RGB_COLORS, &grayPixels, nullptr, 0);
    void* compPixels = nullptr;
    HBITMAP compDib = CreateDIBSection(screenDC, &bmi, DIB_RGB_COLORS, &compPixels, nullptr, 0);
    if (!rawDib || !rawPixels || !grayDib || !grayPixels || !compDib || !compPixels) {
        if (rawDib) DeleteObject(rawDib);
        if (grayDib) DeleteObject(grayDib);
        if (compDib) DeleteObject(compDib);
        ReleaseDC(nullptr, screenDC);
        return false;
    }

    HDC rawDc = CreateCompatibleDC(screenDC);
    HDC grayDc = CreateCompatibleDC(screenDC);
    HDC compDc = CreateCompatibleDC(screenDC);
    if (!rawDc || !grayDc || !compDc) {
        if (rawDc) DeleteDC(rawDc);
        if (grayDc) DeleteDC(grayDc);
        if (compDc) DeleteDC(compDc);
        DeleteObject(rawDib);
        DeleteObject(grayDib);
        DeleteObject(compDib);
        ReleaseDC(nullptr, screenDC);
        return false;
    }

    HGDIOBJ oldRaw = SelectObject(rawDc, rawDib);
    HGDIOBJ oldGray = SelectObject(grayDc, grayDib);
    HGDIOBJ oldComp = SelectObject(compDc, compDib);
    if (!oldRaw || oldRaw == HGDI_ERROR || !oldGray || oldGray == HGDI_ERROR || !oldComp || oldComp == HGDI_ERROR) {
        if (oldRaw && oldRaw != HGDI_ERROR) SelectObject(rawDc, oldRaw);
        if (oldGray && oldGray != HGDI_ERROR) SelectObject(grayDc, oldGray);
        if (oldComp && oldComp != HGDI_ERROR) SelectObject(compDc, oldComp);
        DeleteDC(rawDc);
        DeleteDC(grayDc);
        DeleteDC(compDc);
        DeleteObject(rawDib);
        DeleteObject(grayDib);
        DeleteObject(compDib);
        ReleaseDC(nullptr, screenDC);
        return false;
    }

    const bool captured = BitBlt(rawDc, 0, 0, width, height, screenDC,
                                 origin.x, origin.y, SRCCOPY | CAPTUREBLT);
    ReleaseDC(nullptr, screenDC);
    if (!captured) {
        SelectObject(rawDc, oldRaw);
        SelectObject(grayDc, oldGray);
        SelectObject(compDc, oldComp);
        DeleteDC(rawDc);
        DeleteDC(grayDc);
        DeleteDC(compDc);
        DeleteObject(rawDib);
        DeleteObject(grayDib);
        DeleteObject(compDib);
        return false;
    }

    // Copy raw to gray and fully desaturate it once.
    BitBlt(grayDc, 0, 0, width, height, rawDc, 0, 0, SRCCOPY);
    
    // FIX: Force GDI batch flush before reading pixel data
    GdiFlush();
    
    if (grayPixels) {
        auto* pixel = static_cast<BYTE*>(grayPixels);
        const size_t count = (size_t)width * (size_t)height;
        for (size_t i = 0; i < count; ++i) {
            BYTE* color = pixel + i * 4;
            const float luminance = color[2] * 0.299f + color[1] * 0.587f + color[0] * 0.114f;
            color[0] = color[1] = color[2] = static_cast<BYTE>(luminance);
        }
    }

    g_backdropCache.rawBitmap = rawDib;
    g_backdropCache.rawDc = rawDc;
    g_backdropCache.grayBitmap = grayDib;
    g_backdropCache.grayDc = grayDc;
    g_backdropCache.compBitmap = compDib;
    g_backdropCache.compDc = compDc;
    g_backdropCache.size = { 0, 0, width, height };
    g_backdropCache.origin = origin;
    g_backdropCache.dpi = dpi;
    g_backdropCache.hwnd = hwnd;
    g_backdropCache.startTime = GetTickCount64();
    g_backdropCache.valid = true;
    return true;
}


static RECT GetBoxSurfaceArea(const RECT& clientArea) {
    const int width = clientArea.right - clientArea.left;
    const int height = clientArea.bottom - clientArea.top;
    if (width != GetSystemMetrics(SM_CXVIRTUALSCREEN) ||
        height != GetSystemMetrics(SM_CYVIRTUALSCREEN))
        return clientArea;

    MONITORINFO mi = { sizeof(mi) };
    const POINT origin = { 0, 0 };
    HMONITOR primary = MonitorFromPoint(origin, MONITOR_DEFAULTTOPRIMARY);
    if (!primary || !GetMonitorInfoW(primary, &mi))
        return clientArea;
    return {
        clientArea.left + mi.rcMonitor.left - GetSystemMetrics(SM_XVIRTUALSCREEN),
        clientArea.top + mi.rcMonitor.top - GetSystemMetrics(SM_YVIRTUALSCREEN),
        clientArea.left + mi.rcMonitor.right - GetSystemMetrics(SM_XVIRTUALSCREEN),
        clientArea.top + mi.rcMonitor.bottom - GetSystemMetrics(SM_YVIRTUALSCREEN)
    };
}

static bool DrawClassicPleaseWaitBox(HWND hwnd, HDC hdc) {
    RECT clientArea = {};
    if (!hwnd || !GetClientRect(hwnd, &clientArea)) return false;
    if (clientArea.right <= clientArea.left || clientArea.bottom <= clientArea.top) return false;

    // The cached backdrop is already drawn by the hook at EVERY DPI: no
    // recapture here (which would include the previous frame's box -> trails).
    const RECT boxArea = GetBoxSurfaceArea(clientArea);
    const UINT dpi = GetWindowDpi(hwnd);
    const int boxWidth = ScaleBoxForDpi(210, dpi);
    const int boxHeight = ScaleBoxForDpi(74, dpi);
    const int rim = std::max(1, ScaleBoxForDpi(5, dpi));
    const int border = std::max(1, ScaleBoxForDpi(1, dpi));

    RECT outer = {
        boxArea.left + ((boxArea.right - boxArea.left) - boxWidth) / 2,
        boxArea.top + ((boxArea.bottom - boxArea.top) - boxHeight) / 2,
        0, 0
    };
    outer.right = outer.left + boxWidth;
    outer.bottom = outer.top + boxHeight;

    RECT darkBorder = outer;
    InflateRect(&darkBorder, -rim, -rim);
    RECT content = darkBorder;
    InflateRect(&content, -border, -border);
    if (content.right <= content.left || content.bottom <= content.top) return false;

    ScopedGdiObject outerBrush(CreateSolidBrush(RGB(197, 218, 231)));
    ScopedGdiObject borderBrush(CreateSolidBrush(RGB(118, 131, 139)));
    ScopedGdiObject contentBrush(CreateSolidBrush(RGB(255, 255, 255)));
    ScopedGdiObject topStreakBrush(CreateSolidBrush(RGB(231, 241, 247)));
    ScopedGdiObject bottomStreakBrush(CreateSolidBrush(RGB(177, 208, 225)));
    if (!outerBrush || !borderBrush || !contentBrush) return false;

    FillRect(hdc, &outer, (HBRUSH)outerBrush.get());

    const int inset = std::max(1, ScaleBoxForDpi(1, dpi));
    const int streakHeight = std::max(1, ScaleBoxForDpi(1, dpi));
    if (topStreakBrush && bottomStreakBrush && rim > streakHeight) {
        RECT topStreak = { outer.left + inset, outer.top + inset, outer.right - inset, outer.top + inset + streakHeight };
        RECT bottomStreak = { outer.left + inset, outer.bottom - inset - streakHeight, outer.right - inset, outer.bottom - inset };
        FillRect(hdc, &topStreak, (HBRUSH)topStreakBrush.get());
        FillRect(hdc, &bottomStreak, (HBRUSH)bottomStreakBrush.get());
    }

    FillRect(hdc, &darkBorder, (HBRUSH)borderBrush.get());
    FillRect(hdc, &content, (HBRUSH)contentBrush.get());

    NONCLIENTMETRICSW metrics = { sizeof(metrics) };
    LOGFONTW fontDescription = {};
    if (SystemParametersInfoForDpi(SPI_GETNONCLIENTMETRICS, sizeof(metrics), &metrics, 0, dpi)) {
        fontDescription = metrics.lfMessageFont;
    } else {
        fontDescription.lfHeight = -ScaleForDpi(12, dpi);
    }

    fontDescription.lfHeight = MulDiv(fontDescription.lfHeight, g_settings.fontSizePercent.load(), 100);
    fontDescription.lfWeight = FW_BOLD;

    ScopedGdiObject font(CreateFontIndirectW(&fontDescription));
    if (!font) return false;

    int savedDc = SaveDC(hdc);
    ScopedSelection selection(hdc, font.get());
    if (!selection.selected()) {
        RestoreDC(hdc, savedDc);
        return false;
    }

    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, RGB(0, 0, 0));
    
    // FIX: Use local buffer instead of returning a pointer to shared data
    wchar_t textBuffer[ARRAYSIZE(Settings::customText)];
    GetPleaseWaitText(textBuffer);
    DrawTextW(hdc, textBuffer, -1, &content,
              DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS | DT_NOPREFIX);

    RestoreDC(hdc, savedDc);
    return true;
}
static void THISCALL CDimmedWindow_OnPaintHook(CDimmedWindow* window, HDC hdc) {
    if (!hdc) {
        if (g_originalOnPaint) g_originalOnPaint(window, hdc);
        return;
    }

    HWND hwnd = WindowFromDC(hdc);
    RECT clientRect = {};
    if (!hwnd || !GetClientRect(hwnd, &clientRect)) {
        if (g_originalOnPaint) g_originalOnPaint(window, hdc);
        return;
    }
    if (clientRect.right <= clientRect.left || clientRect.bottom <= clientRect.top) {
        if (g_originalOnPaint) g_originalOnPaint(window, hdc);
        return;
    }

    const UINT monitorDpi = GetMonitorDpiForWindow(hwnd);
    POINT origin = {};
    const bool haveOrigin = GetBackdropOrigin(hwnd, &origin);
    const int clientWidth = clientRect.right - clientRect.left;
    const int clientHeight = clientRect.bottom - clientRect.top;

    // Calculate desaturation amount once, before the lock
    const float target = g_settings.desaturationPercent.load() / 100.0f;
    float amount = 0.0f;
    if (target > 0.0f) {
        const int rampMs = g_settings.desaturationRampMs.load();
        const ULONGLONG elapsed = GetTickCount64() - g_backdropCache.startTime;
        if (rampMs <= 0 || elapsed >= (ULONGLONG)rampMs) {
            amount = target;
        } else {
            double t = (double)elapsed / (double)rampMs;
            t = t * t * (3.0 - 2.0 * t);
            amount = target * (float)t;
        }
    }

    {
        std::lock_guard<std::recursive_mutex> lock(g_cacheMutex);

        if (!g_backdropCache.valid ||
            !haveOrigin ||
            g_backdropCache.hwnd != hwnd ||
            g_backdropCache.dpi != monitorDpi ||
            g_backdropCache.origin.x != origin.x ||
            g_backdropCache.origin.y != origin.y ||
            g_backdropCache.size.right != clientWidth ||
            g_backdropCache.size.bottom != clientHeight) {
            if (haveOrigin)
                BuildBackdropCache(hwnd, clientRect, monitorDpi);
        }

        if (!g_backdropCache.valid) {
            if (g_originalOnPaint) g_originalOnPaint(window, hdc);
            return;
        }

        // Compose everything into the offscreen buffer first
        if (g_backdropCache.compDc) {
            // Draw backdrop into compDc
            BitBlt(g_backdropCache.compDc, 0, 0, clientWidth, clientHeight,
                   g_backdropCache.rawDc, 0, 0, SRCCOPY);
            
            // Apply desaturation
            if (amount > 0.0f) {
                BLENDFUNCTION bf = { AC_SRC_OVER, 0, (BYTE)lround(amount * 255.0f), 0 };
                AlphaBlend(g_backdropCache.compDc, 0, 0, clientWidth, clientHeight,
                           g_backdropCache.grayDc, 0, 0, clientWidth, clientHeight, bf);
            }
            
            // Draw the box on top in the composition buffer
            DrawClassicPleaseWaitBox(hwnd, g_backdropCache.compDc);
            
            // Single blit to screen - no flash
            BitBlt(hdc, clientRect.left, clientRect.top, clientWidth, clientHeight,
                   g_backdropCache.compDc, 0, 0, SRCCOPY);
        }
        
        // Keep animation alive
        if (amount < target && g_backdropCache.hwnd)
            InvalidateRect(g_backdropCache.hwnd, nullptr, FALSE);
    }
}

static bool HookThemeUiSymbols(HMODULE themeUi) {
    WindhawkUtils::SYMBOL_HOOK themeuiDllHook = {
        { L"private: void " STHICALL L" CDimmedWindow::OnPaint(struct HDC__ *)" },
        &g_originalOnPaint, CDimmedWindow_OnPaintHook, false
    };
    if (!WindhawkUtils::HookSymbols(themeUi, &themeuiDllHook, 1)) {
        Wh_Log(L"CDimmedWindow::OnPaint was not resolved");
        return false;
    }
    return true;
}

using LoadLibraryExW_t = decltype(&LoadLibraryExW);
static LoadLibraryExW_t LoadLibraryExW_Original = nullptr;

static void HandleLoadedModule(HMODULE /*module*/) {
    if (g_themeUiHandled.load())
        return;
    HMODULE themeUi = GetModuleHandleW(L"themeui.dll");
    if (!themeUi || g_themeUiHandled.exchange(true))
        return;
    if (HookThemeUiSymbols(themeUi))
        Wh_ApplyHookOperations();
}

static HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR fileName, HANDLE file, DWORD flags) {
    HMODULE module = LoadLibraryExW_Original(fileName, file, flags);
    if (module) HandleLoadedModule(module);
    return module;
}

BOOL Wh_ModInit() {
    Wh_Log(L">");
    LoadSettings();
    
    HMODULE themeUi = GetModuleHandleW(L"themeui.dll");
    if (themeUi) {
        g_themeUiHandled = true;
        if (!HookThemeUiSymbols(themeUi))
            return FALSE;
    }

    HMODULE kernelBase = GetModuleHandleW(L"kernelbase.dll");
    auto loadLibraryExW = kernelBase ? (LoadLibraryExW_t)GetProcAddress(kernelBase, "LoadLibraryExW") : nullptr;
    if (!loadLibraryExW) {
        Wh_Log(L"kernelbase!LoadLibraryExW was not found");
        return FALSE;
    }
    if (!WindhawkUtils::SetFunctionHook(loadLibraryExW, LoadLibraryExW_Hook, &LoadLibraryExW_Original)) {
        Wh_Log(L"Failed to hook kernelbase!LoadLibraryExW");
        return FALSE;
    }
    return TRUE;
}
void Wh_ModAfterInit() {
    Wh_Log(L">");
    HMODULE themeUi = GetModuleHandleW(L"themeui.dll");
    if (themeUi && !g_themeUiHandled.exchange(true)) {
        if (!HookThemeUiSymbols(themeUi))
            return;
        Wh_ApplyHookOperations();
    }
}

void Wh_ModSettingsChanged() {
    LoadSettings();
}

void Wh_ModUninit() {
    Wh_Log(L">");
    FreeBackdropCache();
}
