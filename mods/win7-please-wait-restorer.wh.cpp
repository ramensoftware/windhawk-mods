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
// @compilerOptions -lgdi32 -lshcore
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

When the display scaling (DPI) is changed while the mod is active, the mod may
behave slightly differently (e.g. the captured backdrop or the classic box may
look off until the next theme switch). To ensure everything is back to normal,
it is recommended to restart Explorer or log out and log back in after changing
the DPI.
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
// Sfondo in cache: catturato e desaturato UNA sola volta, con la STESSA
// semantica di coordinate dell'originale (origine = ClientToScreen(0,0),
// dimensione = client). Il paint usa sempre questa cache e non ricattura mai
// lo schermo (che conterrebbe il box del frame precedente -> scie a cascata).
// Nessuna conversione e nessun ridimensionamento: nessuno zoom a nessun DPI.
//
// Il percorso in cache e' usato a OGNI DPI, compreso il 100%: la resa a 100%
// e' identica a quella verificata a 125%. La desaturazione e' progressiva,
// come in Windows 7: parte dalla cattura vergine e aumenta nel tempo fino al
// valore scelto (desaturationRampMs ne regola la durata). La cache registra
// il DPI (e la finestra) con cui e' stata costruita: se il DPI cambia durante
// l'esecuzione viene ricostruita subito, senza ambiguita'.
// ---------------------------------------------------------------------------
struct BackdropCache {
    HBITMAP    rawBitmap  = nullptr;   // cattura vergine, mai desaturata
    HDC        rawDc      = nullptr;   // memory DC con rawBitmap selezionata
    HBITMAP    workBitmap = nullptr;   // copia di lavoro per la desaturazione per-frame
    HDC        workDc     = nullptr;   // memory DC con workBitmap selezionata
    void*      workPixels = nullptr;   // pixel di workBitmap (DIB section 32 bpp)
    RECT       size       = { 0, 0, 0, 0 };  // dimensione catturata (= client)
    POINT      origin     = { 0, 0 };  // origine di cattura (ClientToScreen di 0,0)
    UINT       dpi        = 0;         // DPI effettivo del monitor al momento della cattura
    HWND       hwnd       = nullptr;   // finestra a cui appartiene la cattura
    ULONGLONG  startTime  = 0;         // inizio della rampa di desaturazione
    float      lastAmount = -1.0f;     // ultima desaturazione applicata a workBitmap
    bool       valid      = false;
};
static BackdropCache g_backdropCache;
static std::recursive_mutex g_cacheMutex;

static void InvalidateBackdropCache() {
    std::lock_guard<std::recursive_mutex> lock(g_cacheMutex);
    g_backdropCache.valid = false;
}


static void LoadSettings() {
    std::lock_guard<std::mutex> lock(g_settingsMutex);
    constexpr int kDefaultDesaturationPercent = 65;
    const int oldDesaturationPercent = g_settings.desaturationPercent.load();
    int desaturationPercent = Wh_GetIntSetting(L"desaturationPercent");
    g_settings.desaturationPercent.store(
        desaturationPercent >= 0 && desaturationPercent <= 100
            ? desaturationPercent
            : kDefaultDesaturationPercent
    );
    // Se cambia la desaturazione, la cache dello sfondo va ricostruita.
    if (g_settings.desaturationPercent.load() != oldDesaturationPercent)
        InvalidateBackdropCache();
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

static const wchar_t* GetPleaseWaitText() {
    std::lock_guard<std::mutex> lock(g_settingsMutex);
    if (g_settings.customTextValid.load() && g_settings.customText[0])
        return g_settings.customText;
    
    const LANGID uiLanguage = GetUserDefaultUILanguage();
    for (const auto& entry : kPleaseWaitTexts)
        if (entry.language == uiLanguage) return entry.text;
    for (const auto& entry : kPleaseWaitTexts)
        if (PRIMARYLANGID(entry.language) == PRIMARYLANGID(uiLanguage)) return entry.text;
    return L"Please wait";
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

// Origine della regione di schermo da catturare: ClientToScreen dell'angolo
// client, esattamente come nel codice originale del mod. NESSUNA conversione
// di spazio di coordinate e NESSUN ridimensionamento: la resa resta identica
// all'originale a ogni DPI (nessuno zoom); la cache elimina le scie.
static bool GetBackdropOrigin(HWND hwnd, POINT* originOut) {
    POINT origin = { 0, 0 };
    if (!ClientToScreen(hwnd, &origin))
        return false;
    if (originOut)
        *originOut = origin;
    return true;
}

// DPI effettivo del monitor su cui sta la finestra (96 = scala 100%).
// Indipendente dal livello di DPI-awareness del processo: e' la vera
// impostazione di scala dell'utente, usato per convalidare la cache.
static UINT GetMonitorDpiForWindow(HWND hwnd) {
    HMONITOR monitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
    UINT dpiX = 96, dpiY = 96;
    if (monitor) GetDpiForMonitor(monitor, MDT_EFFECTIVE_DPI, &dpiX, &dpiY);
    return dpiX;
}

// DPI da usare per DISEGNARE nella finestra, coerente con il livello di
// DPI-awareness del processo: per processi DPI-unaware restituisce 96 (le
// unita' del DC sono logiche e il sistema scala il risultato), per processi
// aware il DPI reale del monitor su cui sta la finestra.
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



// Cattura lo schermo nell'area client UNA volta sola, con la STESSA semantica
// dell'originale (origine = ClientToScreen(0,0), dimensione = client): resa
// identica all'originale a ogni DPI, senza zoom. La cattura resta VERGINE:
// la desaturazione progressiva viene applicata per-frame sulla copia di
// lavoro, partendo sempre da questa base pulita (niente scie a cascata).
static bool BuildBackdropCache(HWND hwnd, const RECT& clientRect, UINT dpi) {
    POINT origin = {};
    if (!GetBackdropOrigin(hwnd, &origin))
        return false;
    // Dimensione della cattura = dimensione del client DC, come nell'originale:
    // il blit finale e' sempre 1:1 (nessuno zoom).
    const int width = clientRect.right - clientRect.left;
    const int height = clientRect.bottom - clientRect.top;
    if (width <= 0 || height <= 0)
        return false;

    std::lock_guard<std::recursive_mutex> lock(g_cacheMutex);

    // Libera la vecchia cache.
    if (g_backdropCache.rawDc)
        DeleteDC(g_backdropCache.rawDc);
    if (g_backdropCache.rawBitmap)
        DeleteObject(g_backdropCache.rawBitmap);
    if (g_backdropCache.workDc)
        DeleteDC(g_backdropCache.workDc);
    if (g_backdropCache.workBitmap)
        DeleteObject(g_backdropCache.workBitmap);
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
    void* workPixels = nullptr;
    HBITMAP workDib = CreateDIBSection(screenDC, &bmi, DIB_RGB_COLORS, &workPixels, nullptr, 0);
    if (!rawDib || !rawPixels || !workDib || !workPixels) {
        if (rawDib) DeleteObject(rawDib);
        if (workDib) DeleteObject(workDib);
        ReleaseDC(nullptr, screenDC);
        return false;
    }

    HDC rawDc = CreateCompatibleDC(screenDC);
    HDC workDc = CreateCompatibleDC(screenDC);
    if (!rawDc || !workDc) {
        if (rawDc) DeleteDC(rawDc);
        if (workDc) DeleteDC(workDc);
        DeleteObject(rawDib);
        DeleteObject(workDib);
        ReleaseDC(nullptr, screenDC);
        return false;
    }

    HGDIOBJ oldRaw = SelectObject(rawDc, rawDib);
    HGDIOBJ oldWork = SelectObject(workDc, workDib);
    if (!oldRaw || oldRaw == HGDI_ERROR || !oldWork || oldWork == HGDI_ERROR) {
        if (oldRaw && oldRaw != HGDI_ERROR) SelectObject(rawDc, oldRaw);
        if (oldWork && oldWork != HGDI_ERROR) SelectObject(workDc, oldWork);
        DeleteDC(rawDc);
        DeleteDC(workDc);
        DeleteObject(rawDib);
        DeleteObject(workDib);
        ReleaseDC(nullptr, screenDC);
        return false;
    }

    const bool captured = BitBlt(rawDc, 0, 0, width, height, screenDC,
                                 origin.x, origin.y, SRCCOPY | CAPTUREBLT);
    ReleaseDC(nullptr, screenDC);
    if (!captured) {
        SelectObject(rawDc, oldRaw);
        SelectObject(workDc, oldWork);
        DeleteDC(rawDc);
        DeleteDC(workDc);
        DeleteObject(rawDib);
        DeleteObject(workDib);
        return false;
    }

    // Copia iniziale: la copia di lavoro parte dalla cattura vergine.
    BitBlt(workDc, 0, 0, width, height, rawDc, 0, 0, SRCCOPY);

    g_backdropCache.rawBitmap = rawDib;
    g_backdropCache.rawDc = rawDc;
    g_backdropCache.workBitmap = workDib;
    g_backdropCache.workDc = workDc;
    g_backdropCache.workPixels = workPixels;
    g_backdropCache.size = { 0, 0, width, height };
    g_backdropCache.origin = origin;
    g_backdropCache.dpi = dpi;
    g_backdropCache.hwnd = hwnd;
    g_backdropCache.startTime = GetTickCount64();
    g_backdropCache.lastAmount = -1.0f;
    g_backdropCache.valid = true;
    return true;
}

// Disegna lo sfondo sull'HDC partendo SEMPRE dalla cattura vergine, con
// desaturazione progressiva come in Windows 7: all'inizio il colore e'
// pieno, poi sfuma verso il valore scelto nell'arco di desaturationRampMs
// millisecondi (0 = applicazione istantanea). La cache ha le stesse
// dimensioni del client DC, quindi il blit e' SEMPRE 1:1: niente
// StretchBlt, niente zoom.
static bool PaintCachedBackdrop(HDC hdc, const RECT& clientRect) {
    std::lock_guard<std::recursive_mutex> lock(g_cacheMutex);
    if (!g_backdropCache.valid || !g_backdropCache.rawDc || !g_backdropCache.workDc)
        return false;

    const int width = clientRect.right - clientRect.left;
    const int height = clientRect.bottom - clientRect.top;
    if (width <= 0 || height <= 0)
        return false;

    // Quanto desaturare in QUESTO frame, in base al tempo trascorso.
    const float target = g_settings.desaturationPercent.load() / 100.0f;
    float amount = 0.0f;
    if (target > 0.0f) {
        const int rampMs = g_settings.desaturationRampMs.load();
        const ULONGLONG elapsed = GetTickCount64() - g_backdropCache.startTime;
        if (rampMs <= 0 || elapsed >= (ULONGLONG)rampMs) {
            amount = target;  // rampa completata (o disabilitata): valore finale
        } else {
            double t = (double)elapsed / (double)rampMs;
            t = t * t * (3.0 - 2.0 * t);  // smoothstep, come la transizione di Win7
            amount = target * (float)t;
        }
    }

    // Riallaccia l'animazione: finche' la rampa non e' completa, forza un
    // nuovo paint (senza, con pochi paint la progressivita' si perderebbe).
    if (amount < target && g_backdropCache.hwnd)
        InvalidateRect(g_backdropCache.hwnd, nullptr, FALSE);

    // Se la desaturazione non e' cambiata, la copia di lavoro e' gia' pronta:
    // si evita la copia e il passaggio per-pixel inutili.
    if (amount != g_backdropCache.lastAmount) {
        // Riparte SEMPRE dalla cattura vergine: mai dallo schermo (che
        // conterrebbe il box del frame precedente -> scie a cascata).
        BitBlt(g_backdropCache.workDc, 0, 0, width, height,
               g_backdropCache.rawDc, 0, 0, SRCCOPY);

        // Desaturazione in place (BI_RGB 32-bit: byte B, G, R, riservato).
        if (amount > 0.0f && g_backdropCache.workPixels) {
            auto* pixel = static_cast<BYTE*>(g_backdropCache.workPixels);
            const size_t count = (size_t)width * (size_t)height;
            for (size_t i = 0; i < count; ++i) {
                BYTE* color = pixel + i * 4;
                const float luminance = color[2] * 0.299f + color[1] * 0.587f + color[0] * 0.114f;
                color[0] = static_cast<BYTE>(color[0] + (luminance - color[0]) * amount);
                color[1] = static_cast<BYTE>(color[1] + (luminance - color[1]) * amount);
                color[2] = static_cast<BYTE>(color[2] + (luminance - color[2]) * amount);
            }
        }
        g_backdropCache.lastAmount = amount;
    }

    return BitBlt(hdc, clientRect.left, clientRect.top, width, height,
                  g_backdropCache.workDc, 0, 0, SRCCOPY);
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

    // Lo sfondo in cache e' gia' stato disegnato dal hook a OGNI DPI: niente
    // ricattura qui (includerebbe il box del frame precedente -> scie).
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
    DrawTextW(hdc, GetPleaseWaitText(), -1, &content,
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

    // DPI effettivo del monitor (96 = scala 100%, 120 = 125%, ...). A OGNI
    // DPI si usa lo stesso percorso: sfondo in cache con cattura unica per
    // evitare le scie. Il comportamento a 100% e' identico a quello a 125%.
    const UINT monitorDpi = GetMonitorDpiForWindow(hwnd);
    POINT origin = {};
    const bool haveOrigin = GetBackdropOrigin(hwnd, &origin);
    const int clientWidth = clientRect.right - clientRect.left;
    const int clientHeight = clientRect.bottom - clientRect.top;

    {
        std::lock_guard<std::recursive_mutex> lock(g_cacheMutex);

        // Ricrea la cache se non esiste o se cambia finestra, DPI, dimensione
        // o posizione: un cambio DPI a runtime invalida subito la cache.
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

        // Sfondo: SEMPRE dalla cache, mai dallo schermo (che conterrebbe il
        // box del frame precedente -> scie a cascata).
        if (!g_backdropCache.valid || !PaintCachedBackdrop(hdc, clientRect)) {
            if (g_originalOnPaint) g_originalOnPaint(window, hdc);
            return;
        }
    }

    // Box sopra lo sfondo, senza ricattura.
    DrawClassicPleaseWaitBox(hwnd, hdc);
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
}
