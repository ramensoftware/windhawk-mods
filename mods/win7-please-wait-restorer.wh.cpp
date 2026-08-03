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

## Credits

- Nex — Testing on Windows 10 21H2 and providing feedback
*/
// ==/WindhawkModReadme==


// ==WindhawkModSettings==
/*
- desaturationPercent: 20
  $name: Background desaturation (%)
  $description: This setting changes the amount of color removed from the captured desktop background.
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
    std::atomic<int> boxSizePercent;
    std::atomic<int> fontSizePercent;
    wchar_t customText[256];
    std::atomic<bool> customTextValid;
};

static Settings g_settings = { 20, 105, 115, L"", false };

static void LoadSettings() {
    std::lock_guard<std::mutex> lock(g_settingsMutex);
    constexpr int kDefaultDesaturationPercent = 20;
    int desaturationPercent = Wh_GetIntSetting(L"desaturationPercent");
    g_settings.desaturationPercent.store(
        desaturationPercent >= 0 && desaturationPercent <= 100
            ? desaturationPercent
            : kDefaultDesaturationPercent
    );
    g_settings.boxSizePercent.store(std::clamp(Wh_GetIntSetting(L"boxSizePercent"), 50, 200));
    g_settings.fontSizePercent.store(std::clamp(Wh_GetIntSetting(L"fontSizePercent"), 50, 200));

    PCWSTR customText = Wh_GetStringSetting(L"customText");
    size_t customTextLength = 0;
    if (customText) {
        while (customTextLength < 100 && customText[customTextLength])
            ++customTextLength;
    }
    if (customText && customTextLength < 100) {
        lstrcpynW(g_settings.customText, customText, ARRAYSIZE(g_settings.customText));
        g_settings.customTextValid.store(true);
    } else {
        g_settings.customText[0] = L'\0';
        g_settings.customTextValid.store(false);
    }
    if (customText) Wh_FreeStringSetting(customText);
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
    { MAKELANGID(LANG_DUTCH, SUBLANG_DEFAULT), L"Even ogenblik geduld" },
    { MAKELANGID(LANG_POLISH, SUBLANG_DEFAULT), L"Prosz\u0119 czeka\u0107" },
    { MAKELANGID(LANG_RUSSIAN, SUBLANG_DEFAULT), L"\u041f\u043e\u0434\u043e\u0436\u0434\u0430\u043d\u0438\u0442\u0435" },
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

class ScopedDc {
public:
    explicit ScopedDc(HDC dc = nullptr) : dc_(dc) {}
    ~ScopedDc() { if (dc_) DeleteDC(dc_); }
    ScopedDc(const ScopedDc&) = delete;
    HDC get() const { return dc_; }
    explicit operator bool() const { return dc_ != nullptr; }
private:
    HDC dc_;
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

// Uses only documented GDI APIs. A 32-bit DIB section gives direct access to
// the captured pixels, so the desaturation is not dependent on GDI+ behavior.
static bool PaintSnapshotWithGentleDesaturation(HDC hdc, const RECT& area) {
    if (!hdc) return false;
    const int width = area.right - area.left;
    const int height = area.bottom - area.top;
    if (width <= 0 || height <= 0) return false;

    BITMAPINFO bitmapInfo = {};
    bitmapInfo.bmiHeader.biSize = sizeof(bitmapInfo.bmiHeader);
    bitmapInfo.bmiHeader.biWidth = width;
    bitmapInfo.bmiHeader.biHeight = -height;  // top-down, 32-bit BGR DIB
    bitmapInfo.bmiHeader.biPlanes = 1;
    bitmapInfo.bmiHeader.biBitCount = 32;
    bitmapInfo.bmiHeader.biCompression = BI_RGB;
    void* pixels = nullptr;
    ScopedGdiObject bitmap(CreateDIBSection(nullptr, &bitmapInfo, DIB_RGB_COLORS,
                                             &pixels, nullptr, 0));
    if (!bitmap || !pixels) return false;

    HDC screen = GetDC(nullptr);
    if (!screen) return false;
    ScopedDc memoryDc(CreateCompatibleDC(screen));
    if (!memoryDc) { ReleaseDC(nullptr, screen); return false; }
    {
        ScopedSelection selection(memoryDc.get(), bitmap.get());
        if (!selection.selected()) { ReleaseDC(nullptr, screen); return false; }
        const int screenX = GetSystemMetrics(SM_XVIRTUALSCREEN) + area.left;
        const int screenY = GetSystemMetrics(SM_YVIRTUALSCREEN) + area.top;
        if (!BitBlt(memoryDc.get(), 0, 0, width, height, screen, screenX, screenY,
                    SRCCOPY | CAPTUREBLT)) {
            ReleaseDC(nullptr, screen);
            return false;
        }
        ReleaseDC(nullptr, screen);

        // BI_RGB 32-bit DIB pixels are B, G, R, unused/reserved. Blend every
        // channel toward its luminance while leaving the alpha/reserved byte.
        auto* pixel = static_cast<BYTE*>(pixels);
        const float amount = g_settings.desaturationPercent.load() / 100.0f;
        for (size_t i = 0, count = static_cast<size_t>(width) * height; i < count; ++i) {
            BYTE* color = pixel + i * 4;
            const float luminance = color[2] * 0.299f + color[1] * 0.587f + color[0] * 0.114f;
            color[0] = static_cast<BYTE>(color[0] + (luminance - color[0]) * amount);
            color[1] = static_cast<BYTE>(color[1] + (luminance - color[1]) * amount);
            color[2] = static_cast<BYTE>(color[2] + (luminance - color[2]) * amount);
        }
        BitBlt(hdc, area.left, area.top, width, height, memoryDc.get(), 0, 0, SRCCOPY);
    }
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

    if (!PaintSnapshotWithGentleDesaturation(hdc, clientArea)) return false;

    const RECT boxArea = GetBoxSurfaceArea(clientArea);
    const UINT dpi = GetPrimaryMonitorDpi();
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
    if (SystemParametersInfoW(SPI_GETNONCLIENTMETRICS, sizeof(metrics), &metrics, 0)) {
        fontDescription = metrics.lfMessageFont;
    } else {
        fontDescription.lfHeight = -12;
    }

    int baseHeight = fontDescription.lfHeight;
    fontDescription.lfHeight = -ScaleForDpi(baseHeight < 0 ? -baseHeight : 12, dpi);
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
    bool drawn = false;
    if (hdc) {
        HWND hwnd = WindowFromDC(hdc);
        drawn = DrawClassicPleaseWaitBox(hwnd, hdc);
    }
    if (!drawn && g_originalOnPaint) {
        g_originalOnPaint(window, hdc);
    }
}

static bool HookThemeUiSymbols(HMODULE themeUi) {
    WindhawkUtils::SYMBOL_HOOK themeuiDllHook = {
        { { L"private: void " STHICALL L" CDimmedWindow::OnPaint(struct HDC__ *)" } },
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

static void HandleLoadedModule(HMODULE module) {
    if (GetModuleHandleW(L"themeui.dll") != module ||
        g_themeUiHandled.exchange(true))
        return;
    if (HookThemeUiSymbols(module))
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

BOOL Wh_ModAfterInit() {
    Wh_Log(L">");
    HMODULE themeUi = GetModuleHandleW(L"themeui.dll");
    if (themeUi && !g_themeUiHandled.exchange(true)) {
        if (!HookThemeUiSymbols(themeUi))
            return FALSE;
        Wh_ApplyHookOperations();
    }
    return TRUE;
}

void Wh_ModSettingsChanged() {
    LoadSettings();
}

void Wh_ModUninit() {
    Wh_Log(L">");
}
