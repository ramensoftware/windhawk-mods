// ==WindhawkMod==
// @id taskbar-media-widget
// @name Taskbar Media Widget
// @description A media control widget overlay for the Windows taskbar.
// @version 0.1.0
// @author uMk0
// @github https://github.com/umk0
// @license MIT
// @include explorer.exe
// @architecture x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject -lgdi32 -luser32 -lgdiplus -ldwmapi -ld2d1 -ldwrite -lshell32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Taskbar Media Widget

Windows 10/11 taskbar media widget.

It renders a compact widget over the left side of the taskbar and reads media
data from Windows Global System Media Transport Controls.

Implemented:

- title and artist
- playback status
- smooth progress using LastUpdatedTime
- previous / play-pause / next buttons on hover
- clickable progress bar seeking
- idle launchers for selected music apps
- Fluent/DWM backdrop with Direct2D rendering
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- leftOffset: 8
  $name: Left offset
  $description: Distance from the left edge of the taskbar in physical pixels.
- widgetWidth: 290
  $name: Widget width
- widgetHeight: 42
  $name: Widget height
- innerPadding: 6
  $name: Inner padding
- coverRadius: 7
  $name: Cover radius
- backgroundColor: "#1B2434"
  $name: Background color
- borderColor: "#334157"
  $name: Border color
- accentColor: "#2EA8FF"
  $name: Accent color
- textColor: "#FFFFFF"
  $name: Title text color
- mutedTextColor: "#B9C0CC"
  $name: Artist text color
- followSystemTheme: true
  $name: Follow system theme
  $description: Automatically switch widget colors for Windows light/dark app theme. Disable to use the static colors above.
- themeMode: auto
  $name: Theme mode
  $description: Use light/dark if Windows reports the wrong Fluent theme.
  $options:
    - auto: Auto
    - light: Light
    - dark: Dark
- useArtworkColors: true
  $name: Use artwork colors
  $description: Tint the accent/background from the album art. Disable to use the static colors above.
- useSystemBlur: true
  $name: Use system blur
  $description: Use DWM acrylic/blur for the widget background.
- hideWhenNoMedia: true
  $name: Hide when no media
- idleAppNames: "Яндекс Музыка, Spotify, Apple Music"
  $name: Idle app names
  $description: Comma-separated Start menu shortcut names to show when no media session is active.
*/
// ==/WindhawkModSettings==

#include <atomic>
#include <chrono>
#include <cstring>
#include <cstdint>
#include <cwchar>
#include <cwctype>
#include <memory>
#include <string>
#include <thread>
#include <vector>

#include <windows.h>
#include <windowsx.h>
#include <d2d1.h>
#include <dwmapi.h>
#include <dwrite.h>
#include <gdiplus.h>
#include <shellapi.h>
#include <shlobj.h>

#include <winrt/base.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Media.Control.h>
#include <winrt/Windows.Storage.Streams.h>

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::Media::Control;
using namespace Windows::Storage::Streams;

using Gdiplus::Bitmap;
using Gdiplus::Color;
using Gdiplus::RectF;

struct IBufferByteAccess : ::IUnknown {
    virtual HRESULT STDMETHODCALLTYPE Buffer(BYTE** value) = 0;
};

// Windhawk's MinGW toolchain doesn't always provide importable IID/CLSID and
// FOLDERID symbols. Keep the public Windows identifiers local so the mod links
// consistently without depending on uuid.lib or SDK-specific globals.
const GUID IID_IBufferByteAccess = {
    0x905a0fef,
    0xbc53,
    0x11df,
    {0x8c, 0x49, 0x00, 0x1e, 0x4f, 0xc6, 0x86, 0xda}};

struct IApplicationActivationManagerLocal : ::IUnknown {
    virtual HRESULT STDMETHODCALLTYPE ActivateApplication(
        LPCWSTR appUserModelId,
        LPCWSTR arguments,
        DWORD options,
        DWORD* processId) = 0;
    virtual HRESULT STDMETHODCALLTYPE ActivateForFile(::LPCWSTR,
                                                      ::IUnknown*,
                                                      ::LPCWSTR,
                                                      DWORD*) = 0;
    virtual HRESULT STDMETHODCALLTYPE ActivateForProtocol(::LPCWSTR,
                                                          ::IUnknown*,
                                                          DWORD*) = 0;
};

const GUID CLSID_ApplicationActivationManagerLocal = {
    0x45ba127d,
    0x10a8,
    0x46ea,
    {0x8a, 0xb7, 0x56, 0xea, 0x90, 0x78, 0x94, 0x3c}};

const GUID IID_IApplicationActivationManagerLocal = {
    0x2e941141,
    0x7f97,
    0x4756,
    {0xba, 0x1d, 0x9d, 0xec, 0xde, 0x89, 0x4a, 0x3d}};

const GUID CLSID_ShellLinkLocal = {
    0x00021401,
    0x0000,
    0x0000,
    {0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46}};

const GUID IID_IShellLinkWLocal = {
    0x000214f9,
    0x0000,
    0x0000,
    {0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46}};

const GUID IID_IPersistFileLocal = {
    0x0000010b,
    0x0000,
    0x0000,
    {0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46}};

const GUID IID_IPropertyStoreLocal = {
    0x886d8eeb,
    0x8cf2,
    0x4446,
    {0x8d, 0x02, 0xcd, 0xba, 0x1d, 0xbd, 0xcf, 0x99}};

const GUID IID_IDWriteFactoryLocal = {
    0xb859ee5a,
    0xd838,
    0x4b5b,
    {0xa2, 0xe8, 0x1a, 0xdc, 0x7d, 0x93, 0xdb, 0x48}};

const PROPERTYKEY PKEY_AppUserModel_ID_LOCAL = {
    {0x9f4c2855,
     0x9f79,
     0x4b39,
     {0xa8, 0xd0, 0xe1, 0xd4, 0x2d, 0xe1, 0xd5, 0xf3}},
    5};

const GUID kFolderIdPrograms = {0xa77f5d77,
                                0x2e2b,
                                0x44c3,
                                {0xa6, 0xa2, 0xab, 0xa6, 0x01, 0x05, 0x4a,
                                 0x51}};
const GUID kFolderIdCommonPrograms = {
    0x0139d44e,
    0x6afe,
    0x49f2,
    {0x86, 0x90, 0x3d, 0xaf, 0xca, 0xe6, 0xff, 0xb8}};
const GUID kFolderIdStartMenu = {0x625b53c3,
                                 0xab48,
                                 0x4ec1,
                                 {0xba, 0x1f, 0xa1, 0xef, 0x41, 0x46, 0xfc,
                                  0x19}};
const GUID kFolderIdStartMenuAllPrograms = {
    0xf26305ef,
    0x6948,
    0x40b9,
    {0xb2, 0x55, 0x81, 0x45, 0x3d, 0x09, 0xc7, 0x85}};

constexpr UINT_PTR kTimerPollMedia = 1;
constexpr UINT_PTR kTimerRepaint = 2;
constexpr UINT_PTR kTimerLayout = 3;

#ifndef DWMWA_WINDOW_CORNER_PREFERENCE
#define DWMWA_WINDOW_CORNER_PREFERENCE 33
#endif

#ifndef DWMWA_USE_IMMERSIVE_DARK_MODE
#define DWMWA_USE_IMMERSIVE_DARK_MODE 20
#endif

#ifndef DWMWA_SYSTEMBACKDROP_TYPE
#define DWMWA_SYSTEMBACKDROP_TYPE 38
#endif

enum DWM_WINDOW_CORNER_PREFERENCE_LOCAL {
    DWMWCP_ROUND_LOCAL = 2,
};

enum DWM_SYSTEMBACKDROP_TYPE_LOCAL {
    DWMSBT_NONE_LOCAL = 1,
    DWMSBT_MAINWINDOW_LOCAL = 2,
};

enum AccentState {
    ACCENT_DISABLED = 0,
    ACCENT_ENABLE_HOSTBACKDROP = 5,
};

struct AccentPolicy {
    int accentState;
    int accentFlags;
    int gradientColor;
    int animationId;
};

struct WindowCompositionAttribData {
    int attribute;
    void* data;
    SIZE_T sizeOfData;
};

using SetWindowCompositionAttributeFunc =
    BOOL(WINAPI*)(HWND, WindowCompositionAttribData*);

constexpr int WCA_ACCENT_POLICY = 19;

enum class HotButton {
    None,
    Previous,
    PlayPause,
    Next,
};

enum class ThemeMode {
    Auto,
    Light,
    Dark,
};

struct Settings {
    int leftOffset;
    int widgetWidth;
    int widgetHeight;
    int borderRadius;
    int innerPadding;
    int coverRadius;
    COLORREF backgroundColor;
    COLORREF borderColor;
    COLORREF accentColor;
    COLORREF textColor;
    COLORREF mutedTextColor;
    bool followSystemTheme;
    bool useArtworkColors;
    bool useSystemBlur;
    bool hideWhenNoMedia;
    ThemeMode themeMode;
    std::wstring idleAppNames;
};

struct MediaState {
    bool hasSession = false;
    bool playOnlySession = false;
    hstring sourceAppId;
    hstring title;
    hstring artist;
    GlobalSystemMediaTransportControlsSessionPlaybackStatus status =
        GlobalSystemMediaTransportControlsSessionPlaybackStatus::Closed;
    TimeSpan rawPosition{};
    TimeSpan startTime{};
    TimeSpan endTime{};
    DateTime lastUpdatedTime{};
    bool canPrevious = false;
    bool canNext = false;
    bool canPlay = false;
    bool canPlayPause = false;
    bool canSeek = false;
};

struct IdleApp {
    std::wstring name;
    std::wstring launchPath;
    std::wstring sourceAppId;
    std::wstring activationAppId;
    std::unique_ptr<Bitmap> iconBitmap;
    com_ptr<ID2D1Bitmap> d2dIconBitmap;
};

Settings g_settings;
std::atomic<bool> g_stopThread = false;
std::thread g_widgetThread;
HWND g_widgetWnd = nullptr;
GlobalSystemMediaTransportControlsSession g_mediaSession{nullptr};
MediaState g_mediaState;
bool g_hover = false;
float g_hoverProgress = 0.0f;
float g_blurProgress = 0.0f;
HotButton g_hotButton = HotButton::None;
float g_prevHoverProgress = 0.0f;
float g_playHoverProgress = 0.0f;
float g_nextHoverProgress = 0.0f;
bool g_progressHover = false;
float g_progressHoverProgress = 0.0f;
ULONG_PTR g_gdiplusToken = 0;
std::unique_ptr<Bitmap> g_thumbnailBitmap;
std::vector<IdleApp> g_idleApps;
std::wstring g_thumbnailKey;
COLORREF g_artworkAccentColor = RGB(46, 168, 255);
bool g_hasArtworkColor = false;
bool g_mediaPollCompleted = false;
bool g_isLightTheme = false;
DWORD g_lastMediaSeenTick = 0;
DWORD g_ignoreInputUntilTick = 0;
int g_progressHoverX = 0;
com_ptr<ID2D1Factory> g_d2dFactory;
com_ptr<IDWriteFactory> g_dwriteFactory;
com_ptr<ID2D1DCRenderTarget> g_d2dRenderTarget;
com_ptr<ID2D1Bitmap> g_d2dThumbnailBitmap;
std::wstring g_d2dThumbnailKey;

void RefreshWidget();

int ClampInt(int value, int minValue, int maxValue) {
    if (value < minValue) {
        return minValue;
    }

    if (value > maxValue) {
        return maxValue;
    }

    return value;
}

long long ClampLongLong(long long value, long long minValue, long long maxValue) {
    if (value < minValue) {
        return minValue;
    }

    if (value > maxValue) {
        return maxValue;
    }

    return value;
}

float Approach(float value, float target, float step) {
    if (value < target) {
        value += step;
        if (value > target) {
            value = target;
        }
    } else if (value > target) {
        value -= step;
        if (value < target) {
            value = target;
        }
    }

    return value;
}

COLORREF MixColor(COLORREF a, COLORREF b, float amount) {
    if (amount < 0.0f) {
        amount = 0.0f;
    } else if (amount > 1.0f) {
        amount = 1.0f;
    }

    BYTE r = static_cast<BYTE>(GetRValue(a) +
                               (GetRValue(b) - GetRValue(a)) * amount);
    BYTE g = static_cast<BYTE>(GetGValue(a) +
                               (GetGValue(b) - GetGValue(a)) * amount);
    BYTE blue = static_cast<BYTE>(GetBValue(a) +
                                  (GetBValue(b) - GetBValue(a)) * amount);
    return RGB(r, g, blue);
}

int ColorLuma(COLORREF color) {
    return static_cast<int>(GetRValue(color) * 0.299 +
                            GetGValue(color) * 0.587 +
                            GetBValue(color) * 0.114);
}

COLORREF ReadableAccent(COLORREF color) {
    int luma = ColorLuma(color);
    if (luma < 92) {
        return MixColor(color, RGB(255, 255, 255), 0.42f);
    }

    if (luma > 210) {
        return MixColor(color, RGB(0, 0, 0), 0.28f);
    }

    return color;
}

bool ReadThemeFlag(PCWSTR valueName, bool* outValue) {
    DWORD value = 0;
    DWORD valueSize = sizeof(value);
    LSTATUS status = RegGetValueW(
        HKEY_CURRENT_USER,
        L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
        valueName, RRF_RT_REG_DWORD, nullptr, &value, &valueSize);
    if (status != ERROR_SUCCESS) {
        return false;
    }

    *outValue = value != 0;
    return true;
}

float ColorLuminance(COLORREF color);

ThemeMode ParseThemeMode(PCWSTR value) {
    if (value && _wcsicmp(value, L"light") == 0) {
        return ThemeMode::Light;
    }

    if (value && _wcsicmp(value, L"dark") == 0) {
        return ThemeMode::Dark;
    }

    return ThemeMode::Auto;
}

bool ReadWindowsShellLightTheme() {
    bool systemLight = false;
    bool hasSystem = ReadThemeFlag(L"SystemUsesLightTheme", &systemLight);
    if (hasSystem) {
        return systemLight;
    }

    bool appsLight = false;
    bool hasApps = ReadThemeFlag(L"AppsUseLightTheme", &appsLight);
    if (hasApps) {
        return appsLight;
    }

    return ColorLuminance(GetSysColor(COLOR_WINDOW)) > 0.72f ||
           ColorLuminance(GetSysColor(COLOR_BTNFACE)) > 0.72f;
}

bool EffectiveLightTheme() {
    if (g_settings.themeMode == ThemeMode::Light) {
        return true;
    }

    if (g_settings.themeMode == ThemeMode::Dark) {
        return false;
    }

    return g_isLightTheme;
}

bool UseLightTheme() {
    return EffectiveLightTheme();
}

float ColorLuminance(COLORREF color) {
    return (0.2126f * GetRValue(color) + 0.7152f * GetGValue(color) +
            0.0722f * GetBValue(color)) /
           255.0f;
}

COLORREF ActiveAccentColor() {
    if (g_settings.useArtworkColors && g_hasArtworkColor) {
        return ReadableAccent(g_artworkAccentColor);
    }

    if (g_settings.themeMode != ThemeMode::Auto || g_settings.followSystemTheme ||
        g_settings.useSystemBlur) {
        return UseLightTheme() ? RGB(0, 103, 192) : RGB(46, 168, 255);
    }

    return g_settings.accentColor;
}

COLORREF ActiveBackgroundColor() {
    if (g_settings.useArtworkColors && g_hasArtworkColor) {
        return UseLightTheme()
                   ? MixColor(RGB(246, 248, 252), g_artworkAccentColor, 0.10f)
                   : MixColor(RGB(16, 22, 32), g_artworkAccentColor, 0.24f);
    }

    if (g_settings.themeMode != ThemeMode::Auto || g_settings.followSystemTheme ||
        g_settings.useSystemBlur) {
        return UseLightTheme() ? RGB(246, 248, 252) : RGB(27, 36, 52);
    }

    return g_settings.backgroundColor;
}

COLORREF ActiveBorderColor() {
    if (g_settings.useArtworkColors && g_hasArtworkColor) {
        return UseLightTheme()
                   ? MixColor(RGB(206, 214, 226), g_artworkAccentColor, 0.14f)
                   : MixColor(RGB(70, 82, 104), g_artworkAccentColor, 0.32f);
    }

    if (g_settings.themeMode != ThemeMode::Auto || g_settings.followSystemTheme ||
        g_settings.useSystemBlur) {
        return UseLightTheme() ? RGB(206, 214, 226) : RGB(51, 65, 87);
    }

    return g_settings.borderColor;
}

bool ShouldUseDarkText() {
    if (g_settings.themeMode == ThemeMode::Light) {
        return true;
    }

    if (g_settings.themeMode == ThemeMode::Dark) {
        return false;
    }

    if (g_settings.useSystemBlur || g_settings.followSystemTheme) {
        return EffectiveLightTheme();
    }

    return ColorLuminance(ActiveBackgroundColor()) > 0.62f;
}

COLORREF ActiveTextColor() {
    if (ShouldUseDarkText()) {
        return RGB(18, 24, 33);
    }

    if (g_settings.themeMode != ThemeMode::Auto || g_settings.followSystemTheme ||
        g_settings.useSystemBlur) {
        return RGB(255, 255, 255);
    }

    return g_settings.textColor;
}

COLORREF ActiveMutedTextColor() {
    if (ShouldUseDarkText()) {
        return RGB(78, 88, 104);
    }

    if (g_settings.themeMode != ThemeMode::Auto || g_settings.followSystemTheme ||
        g_settings.useSystemBlur) {
        return RGB(185, 192, 204);
    }

    return g_settings.mutedTextColor;
}

int AccentGradientColor(COLORREF color, BYTE alpha) {
    return (static_cast<int>(alpha) << 24) |
           (static_cast<int>(GetBValue(color)) << 16) |
           (static_cast<int>(GetGValue(color)) << 8) |
           static_cast<int>(GetRValue(color));
}

void ApplySystemBackdrop(HWND hwnd) {
    if (!hwnd) {
        return;
    }

    bool light = EffectiveLightTheme();

    // Let DWM own the window shape. Manual region clipping tends to produce
    // jagged corners when the taskbar is scaled or transparency is enabled.
    DWM_WINDOW_CORNER_PREFERENCE_LOCAL cornerPreference =
        DWMWCP_ROUND_LOCAL;
    DwmSetWindowAttribute(hwnd, DWMWA_WINDOW_CORNER_PREFERENCE,
                          &cornerPreference, sizeof(cornerPreference));

    BOOL darkMode = light ? FALSE : TRUE;
    HRESULT darkResult =
        DwmSetWindowAttribute(hwnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &darkMode,
                              sizeof(darkMode));

    DWM_SYSTEMBACKDROP_TYPE_LOCAL backdropType =
        g_settings.useSystemBlur ? DWMSBT_MAINWINDOW_LOCAL
                                 : DWMSBT_NONE_LOCAL;
    HRESULT backdropResult =
        DwmSetWindowAttribute(hwnd, DWMWA_SYSTEMBACKDROP_TYPE, &backdropType,
                              sizeof(backdropType));

    MARGINS margins = g_settings.useSystemBlur ? MARGINS{-1, -1, -1, -1}
                                               : MARGINS{0, 0, 0, 0};
    HRESULT marginsResult = DwmExtendFrameIntoClientArea(hwnd, &margins);

    HMODULE user32 = GetModuleHandleW(L"user32.dll");
    auto setWindowCompositionAttribute =
        reinterpret_cast<SetWindowCompositionAttributeFunc>(
            GetProcAddress(user32, "SetWindowCompositionAttribute"));

    // Windows 11 uses DWMWA_SYSTEMBACKDROP_TYPE. On Windows 10 the same glassy
    // look is best-effort through SetWindowCompositionAttribute.
    if (setWindowCompositionAttribute) {
        AccentPolicy accent{};
        accent.accentState = g_settings.useSystemBlur && FAILED(backdropResult)
                                 ? ACCENT_ENABLE_HOSTBACKDROP
                                 : ACCENT_DISABLED;
        accent.accentFlags = 2;
        accent.gradientColor =
            AccentGradientColor(light ? RGB(255, 255, 255) : RGB(0, 0, 0),
                                light ? 72 : 96);

        WindowCompositionAttribData data{};
        data.attribute = WCA_ACCENT_POLICY;
        data.data = &accent;
        data.sizeOfData = sizeof(accent);
        setWindowCompositionAttribute(hwnd, &data);
    }

    (void)darkResult;
    (void)marginsResult;
}

bool SyncThemeFromWindows(HWND hwnd, bool force = false) {
    bool oldDetected = g_isLightTheme;
    bool oldEffective = EffectiveLightTheme();
    bool nextLightTheme = ReadWindowsShellLightTheme();
    g_isLightTheme = nextLightTheme;
    bool nextEffective = EffectiveLightTheme();

    if (!force && nextLightTheme == oldDetected &&
        nextEffective == oldEffective) {
        return false;
    }

    ApplySystemBackdrop(hwnd);
    RefreshWidget();
    return true;
}

std::unique_ptr<Bitmap> BitmapFromThumbnail(
    IRandomAccessStreamReference const& thumbnail) {
    if (!thumbnail) {
        return nullptr;
    }

    try {
        auto stream = thumbnail.OpenReadAsync().get();
        uint64_t size64 = stream.Size();
        if (size64 == 0 || size64 > 16 * 1024 * 1024) {
            return nullptr;
        }

        uint32_t size = static_cast<uint32_t>(size64);
        Buffer buffer(size);
        auto readBuffer =
            stream.ReadAsync(buffer, size, InputStreamOptions::None).get();
        uint32_t length = readBuffer.Length();
        if (length == 0) {
            return nullptr;
        }

        com_ptr<IBufferByteAccess> byteAccess;
        check_hresult(reinterpret_cast<::IUnknown*>(get_abi(readBuffer))
                          ->QueryInterface(IID_IBufferByteAccess,
                                           byteAccess.put_void()));
        BYTE* bytes = nullptr;
        if (FAILED(byteAccess->Buffer(&bytes)) || !bytes) {
            return nullptr;
        }

        HGLOBAL global = GlobalAlloc(GMEM_MOVEABLE, length);
        if (!global) {
            return nullptr;
        }

        void* data = GlobalLock(global);
        if (!data) {
            GlobalFree(global);
            return nullptr;
        }

        std::memcpy(data, bytes, length);
        GlobalUnlock(global);

        IStream* imageStream = nullptr;
        if (FAILED(CreateStreamOnHGlobal(global, TRUE, &imageStream))) {
            GlobalFree(global);
            return nullptr;
        }

        std::unique_ptr<Bitmap> loaded(Bitmap::FromStream(imageStream));
        imageStream->Release();

        if (!loaded || loaded->GetLastStatus() != Gdiplus::Ok ||
            loaded->GetWidth() == 0 || loaded->GetHeight() == 0) {
            return nullptr;
        }

        Bitmap* clone = loaded->Clone(0, 0, loaded->GetWidth(),
                                      loaded->GetHeight(),
                                      PixelFormat32bppARGB);
        if (!clone || clone->GetLastStatus() != Gdiplus::Ok) {
            delete clone;
            return nullptr;
        }

        return std::unique_ptr<Bitmap>(clone);
    } catch (hresult_error const& e) {
        Wh_Log(L"Thumbnail error: 0x%08X %s",
               static_cast<uint32_t>(e.code()), e.message().c_str());
    } catch (...) {
        Wh_Log(L"Thumbnail error: unknown exception");
    }

    return nullptr;
}

COLORREF AverageArtworkColor(Bitmap* bitmap) {
    if (!bitmap || bitmap->GetWidth() == 0 || bitmap->GetHeight() == 0) {
        return g_settings.accentColor;
    }

    uint64_t r = 0;
    uint64_t g = 0;
    uint64_t b = 0;
    uint64_t count = 0;

    UINT width = bitmap->GetWidth();
    UINT height = bitmap->GetHeight();
    UINT stepX = width > 24 ? width / 24 : 1;
    UINT stepY = height > 24 ? height / 24 : 1;

    for (UINT y = 0; y < height; y += stepY) {
        for (UINT x = 0; x < width; x += stepX) {
            Color pixel;
            if (bitmap->GetPixel(x, y, &pixel) != Gdiplus::Ok ||
                pixel.GetAlpha() < 180) {
                continue;
            }

            COLORREF color = RGB(pixel.GetRed(), pixel.GetGreen(),
                                 pixel.GetBlue());
            int luma = ColorLuma(color);
            if (luma < 24 || luma > 235) {
                continue;
            }

            r += pixel.GetRed();
            g += pixel.GetGreen();
            b += pixel.GetBlue();
            count++;
        }
    }

    if (count == 0) {
        return g_settings.accentColor;
    }

    return RGB(static_cast<BYTE>(r / count), static_cast<BYTE>(g / count),
               static_cast<BYTE>(b / count));
}

COLORREF ParseColor(PCWSTR value, COLORREF fallback) {
    if (!value || value[0] != L'#') {
        return fallback;
    }

    unsigned int rgb = 0;
    if (swscanf_s(value + 1, L"%x", &rgb) != 1) {
        return fallback;
    }

    return RGB((rgb >> 16) & 0xFF, (rgb >> 8) & 0xFF, rgb & 0xFF);
}

std::wstring ToLower(std::wstring value) {
    for (wchar_t& ch : value) {
        ch = static_cast<wchar_t>(towlower(ch));
    }

    return value;
}

std::wstring Trim(std::wstring value) {
    size_t start = 0;
    while (start < value.size() && iswspace(value[start])) {
        start++;
    }

    size_t end = value.size();
    while (end > start && iswspace(value[end - 1])) {
        end--;
    }

    return value.substr(start, end - start);
}

std::vector<std::wstring> SplitCommaList(std::wstring const& value) {
    std::vector<std::wstring> result;
    size_t start = 0;
    while (start <= value.size()) {
        size_t comma = value.find(L',', start);
        size_t end = comma == std::wstring::npos ? value.size() : comma;
        std::wstring item = Trim(value.substr(start, end - start));
        if (!item.empty()) {
            result.push_back(ToLower(item));
        }

        if (comma == std::wstring::npos) {
            break;
        }

        start = comma + 1;
    }

    return result;
}

std::wstring FileStem(std::wstring path) {
    size_t slash = path.find_last_of(L"\\/");
    size_t start = slash == std::wstring::npos ? 0 : slash + 1;
    size_t dot = path.find_last_of(L'.');
    if (dot == std::wstring::npos || dot < start) {
        dot = path.size();
    }

    return path.substr(start, dot - start);
}

std::wstring WhitelistedSourceForShortcut(std::wstring const& lowerName) {
    if (lowerName.find(L"yandex") != std::wstring::npos ||
        lowerName.find(L"\x044F\x043D\x0434\x0435\x043A\x0441") !=
            std::wstring::npos) {
        return L"ru.yandex.desktop.music";
    }

    if (lowerName.find(L"spotify") != std::wstring::npos) {
        return L"spotify";
    }

    if (lowerName.find(L"apple music") != std::wstring::npos ||
        lowerName.find(L"itunes") != std::wstring::npos) {
        return L"apple";
    }

    if (lowerName.find(L"aimp") != std::wstring::npos) {
        return L"aimp";
    }

    if (lowerName.find(L"foobar") != std::wstring::npos ||
        lowerName.find(L"winamp") != std::wstring::npos ||
        lowerName.find(L"deezer") != std::wstring::npos ||
        lowerName.find(L"tidal") != std::wstring::npos ||
        lowerName.find(L"musicbee") != std::wstring::npos) {
        return lowerName;
    }

    return L"";
}

bool ShortcutAllowedByUserList(std::wstring const& lowerName,
                               std::wstring const& sourceAppId) {
    std::vector<std::wstring> tokens = SplitCommaList(g_settings.idleAppNames);
    if (tokens.empty()) {
        tokens = SplitCommaList(L"\x042F\x043D\x0434\x0435\x043A\x0441 "
                                L"\x041C\x0443\x0437\x044B\x043A\x0430, "
                                L"Spotify, Apple Music");
    }

    for (std::wstring const& token : tokens) {
        if (token.empty()) {
            continue;
        }

        if (lowerName.find(token) != std::wstring::npos) {
            return true;
        }

        if (!sourceAppId.empty() &&
            (ToLower(sourceAppId).find(token) != std::wstring::npos ||
             token.find(ToLower(sourceAppId)) != std::wstring::npos)) {
            return true;
        }

        std::wstring tokenSource = WhitelistedSourceForShortcut(token);
        if (!tokenSource.empty() && !sourceAppId.empty() &&
            ToLower(tokenSource) == ToLower(sourceAppId)) {
            return true;
        }
    }

    return false;
}

std::unique_ptr<Bitmap> BitmapFromIcon(HICON icon) {
    if (!icon) {
        return nullptr;
    }

    std::unique_ptr<Bitmap> bitmap(Bitmap::FromHICON(icon));
    DestroyIcon(icon);

    if (!bitmap || bitmap->GetLastStatus() != Gdiplus::Ok ||
        bitmap->GetWidth() == 0 || bitmap->GetHeight() == 0) {
        return nullptr;
    }

    Bitmap* clone = bitmap->Clone(0, 0, bitmap->GetWidth(),
                                  bitmap->GetHeight(), PixelFormat32bppARGB);
    if (!clone || clone->GetLastStatus() != Gdiplus::Ok) {
        delete clone;
        return nullptr;
    }

    return std::unique_ptr<Bitmap>(clone);
}

bool ResolveShortcutInfo(std::wstring const& path,
                         std::wstring& targetPath,
                         std::wstring& iconPath,
                         int& iconIndex,
                         std::wstring* appUserModelId = nullptr) {
    targetPath.clear();
    iconPath.clear();
    iconIndex = 0;
    if (appUserModelId) {
        appUserModelId->clear();
    }

    IShellLinkW* link = nullptr;
    HRESULT hr = CoCreateInstance(CLSID_ShellLinkLocal, nullptr,
                                  CLSCTX_INPROC_SERVER, IID_IShellLinkWLocal,
                                  reinterpret_cast<void**>(&link));
    if (FAILED(hr) || !link) {
        return false;
    }

    IPersistFile* persistFile = nullptr;
    hr = link->QueryInterface(IID_IPersistFileLocal,
                              reinterpret_cast<void**>(&persistFile));
    if (SUCCEEDED(hr) && persistFile) {
        hr = persistFile->Load(path.c_str(), STGM_READ);
        if (SUCCEEDED(hr)) {
            WCHAR target[MAX_PATH]{};
            if (SUCCEEDED(link->GetPath(target, ARRAYSIZE(target), nullptr,
                                        SLGP_UNCPRIORITY)) &&
                target[0]) {
                targetPath = target;
            }

            WCHAR icon[MAX_PATH]{};
            int index = 0;
            if (SUCCEEDED(link->GetIconLocation(icon, ARRAYSIZE(icon),
                                                &index)) &&
                icon[0]) {
                iconPath = icon;
                iconIndex = index;
            }

            // Store/UWP shortcuts often don't have a useful executable target.
            // The AppUserModelID is the reliable launch handle for those apps.
            if (appUserModelId) {
                IPropertyStore* propertyStore = nullptr;
                if (SUCCEEDED(link->QueryInterface(
                        IID_IPropertyStoreLocal,
                        reinterpret_cast<void**>(&propertyStore))) &&
                    propertyStore) {
                    PROPVARIANT value{};
                    if (SUCCEEDED(propertyStore->GetValue(
                            PKEY_AppUserModel_ID_LOCAL, &value)) &&
                        value.vt == VT_LPWSTR && value.pwszVal &&
                        value.pwszVal[0]) {
                        *appUserModelId = value.pwszVal;
                    }
                    PropVariantClear(&value);
                    propertyStore->Release();
                }
            }
        }

        persistFile->Release();
    }

    link->Release();
    return !targetPath.empty() || !iconPath.empty();
}

std::unique_ptr<Bitmap> LoadIconBitmapFromPath(std::wstring const& path,
                                               int iconIndex = 0) {
    if (path.empty()) {
        return nullptr;
    }

    HICON largeIcon = nullptr;
    if (ExtractIconExW(path.c_str(), iconIndex, &largeIcon, nullptr, 1) > 0 &&
        largeIcon) {
        return BitmapFromIcon(largeIcon);
    }

    SHFILEINFOW fileInfo{};
    DWORD_PTR result = SHGetFileInfoW(path.c_str(), FILE_ATTRIBUTE_NORMAL,
                                      &fileInfo, sizeof(fileInfo),
                                      SHGFI_ICON | SHGFI_LARGEICON);
    if (!result || !fileInfo.hIcon) {
        return nullptr;
    }

    return BitmapFromIcon(fileInfo.hIcon);
}

std::unique_ptr<Bitmap> LoadIconBitmapForShortcut(std::wstring const& path) {
    std::wstring targetPath;
    std::wstring iconPath;
    int iconIndex = 0;
    ResolveShortcutInfo(path, targetPath, iconPath, iconIndex);

    if (!iconPath.empty()) {
        auto icon = LoadIconBitmapFromPath(iconPath, iconIndex);
        if (icon) {
            return icon;
        }
    }

    if (!targetPath.empty()) {
        auto icon = LoadIconBitmapFromPath(targetPath);
        if (icon) {
            return icon;
        }
    }

    return LoadIconBitmapFromPath(path);
}

bool IdleAppAlreadyAdded(std::wstring const& lowerName,
                         std::wstring const& sourceAppId) {
    for (auto const& app : g_idleApps) {
        if (ToLower(app.name) == lowerName ||
            (!sourceAppId.empty() &&
             ToLower(app.sourceAppId) == ToLower(sourceAppId))) {
            return true;
        }
    }

    return false;
}

void AddIdleShortcutIfMatches(std::wstring const& path) {
    if (g_idleApps.size() >= 32) {
        return;
    }

    std::wstring name = FileStem(path);
    std::wstring lowerName = ToLower(name);
    std::wstring sourceAppId = WhitelistedSourceForShortcut(lowerName);
    std::wstring targetPath;
    std::wstring iconPath;
    std::wstring activationAppId;
    int iconIndex = 0;
    ResolveShortcutInfo(path, targetPath, iconPath, iconIndex,
                        &activationAppId);

    // The idle list is intentionally user-controlled. Shortcut names are noisy
    // across Windows installations, so only names from idleAppNames are shown.
    if (!ShortcutAllowedByUserList(lowerName, sourceAppId) ||
        IdleAppAlreadyAdded(lowerName, sourceAppId)) {
        return;
    }

    IdleApp app;
    app.name = name;
    app.launchPath = path;
    app.sourceAppId = !sourceAppId.empty() ? sourceAppId : activationAppId;
    app.activationAppId = activationAppId;
    app.iconBitmap = LoadIconBitmapForShortcut(path);
    g_idleApps.push_back(std::move(app));
}

void ScanIdleShortcutsRecursive(std::wstring const& dir, int depth = 0) {
    if (depth > 6 || g_idleApps.size() >= 32) {
        return;
    }

    std::wstring pattern = dir + L"\\*";
    WIN32_FIND_DATAW findData{};
    HANDLE find = FindFirstFileW(pattern.c_str(), &findData);
    if (find == INVALID_HANDLE_VALUE) {
        return;
    }

    do {
        if (wcscmp(findData.cFileName, L".") == 0 ||
            wcscmp(findData.cFileName, L"..") == 0) {
            continue;
        }

        std::wstring path = dir + L"\\" + findData.cFileName;
        if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            ScanIdleShortcutsRecursive(path, depth + 1);
        } else {
            std::wstring lowerName = ToLower(findData.cFileName);
            if (lowerName.size() >= 4 &&
                lowerName.rfind(L".lnk") == lowerName.size() - 4) {
                AddIdleShortcutIfMatches(path);
            }
        }
    } while (FindNextFileW(find, &findData));

    FindClose(find);
}

void ScanIdleApps() {
    g_idleApps.clear();

    HRESULT coInit = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);

    GUID const folders[] = {kFolderIdPrograms, kFolderIdCommonPrograms,
                            kFolderIdStartMenu,
                            kFolderIdStartMenuAllPrograms};
    for (GUID const& folderId : folders) {
        PWSTR path = nullptr;
        if (SUCCEEDED(SHGetKnownFolderPath(folderId, 0, nullptr, &path)) &&
            path) {
            ScanIdleShortcutsRecursive(path);
            CoTaskMemFree(path);
        }
    }

    if (SUCCEEDED(coInit)) {
        CoUninitialize();
    }

}

void LoadSettings() {
    g_settings.leftOffset = Wh_GetIntSetting(L"leftOffset");
    g_settings.widgetWidth = Wh_GetIntSetting(L"widgetWidth");
    g_settings.widgetHeight = Wh_GetIntSetting(L"widgetHeight");
    g_settings.borderRadius = 10;
    g_settings.innerPadding = Wh_GetIntSetting(L"innerPadding");
    g_settings.coverRadius = Wh_GetIntSetting(L"coverRadius");
    g_settings.followSystemTheme = Wh_GetIntSetting(L"followSystemTheme");
    g_settings.useArtworkColors = Wh_GetIntSetting(L"useArtworkColors");
    g_settings.useSystemBlur = Wh_GetIntSetting(L"useSystemBlur");
    g_settings.hideWhenNoMedia = Wh_GetIntSetting(L"hideWhenNoMedia");

    PCWSTR backgroundColor = Wh_GetStringSetting(L"backgroundColor");
    PCWSTR borderColor = Wh_GetStringSetting(L"borderColor");
    PCWSTR accentColor = Wh_GetStringSetting(L"accentColor");
    PCWSTR textColor = Wh_GetStringSetting(L"textColor");
    PCWSTR mutedTextColor = Wh_GetStringSetting(L"mutedTextColor");
    PCWSTR themeMode = Wh_GetStringSetting(L"themeMode");
    PCWSTR idleAppNames = Wh_GetStringSetting(L"idleAppNames");

    g_settings.backgroundColor = ParseColor(backgroundColor, RGB(27, 36, 52));
    g_settings.borderColor = ParseColor(borderColor, RGB(51, 65, 87));
    g_settings.accentColor = ParseColor(accentColor, RGB(46, 168, 255));
    g_settings.textColor = ParseColor(textColor, RGB(255, 255, 255));
    g_settings.mutedTextColor = ParseColor(mutedTextColor, RGB(185, 192, 204));
    g_settings.themeMode = ParseThemeMode(themeMode);
    g_settings.idleAppNames =
        idleAppNames ? idleAppNames
                     : L"\x042F\x043D\x0434\x0435\x043A\x0441 "
                       L"\x041C\x0443\x0437\x044B\x043A\x0430, Spotify, "
                       L"Apple Music";
    g_isLightTheme = ReadWindowsShellLightTheme();

    Wh_FreeStringSetting(backgroundColor);
    Wh_FreeStringSetting(borderColor);
    Wh_FreeStringSetting(accentColor);
    Wh_FreeStringSetting(textColor);
    Wh_FreeStringSetting(mutedTextColor);
    Wh_FreeStringSetting(themeMode);
    Wh_FreeStringSetting(idleAppNames);

    if (g_settings.widgetWidth < 160) {
        g_settings.widgetWidth = 160;
    }

    if (g_settings.widgetHeight < 38) {
        g_settings.widgetHeight = 38;
    }

    if (g_settings.innerPadding < 4) {
        g_settings.innerPadding = 4;
    }

    if (g_settings.coverRadius < 1) {
        g_settings.coverRadius = 1;
    }
}

BOOL CALLBACK FindTaskbarWindowProc(HWND hwnd, LPARAM lParam) {
    DWORD processId = 0;
    WCHAR className[64];

    if (GetWindowThreadProcessId(hwnd, &processId) &&
        processId == GetCurrentProcessId() &&
        GetClassNameW(hwnd, className, ARRAYSIZE(className)) &&
        _wcsicmp(className, L"Shell_TrayWnd") == 0) {
        *reinterpret_cast<HWND*>(lParam) = hwnd;
        return FALSE;
    }

    return TRUE;
}

HWND FindCurrentProcessTaskbarWnd() {
    HWND taskbarWnd = nullptr;

    EnumWindows(FindTaskbarWindowProc, reinterpret_cast<LPARAM>(&taskbarWnd));

    return taskbarWnd;
}

bool IsFullscreenForegroundWindow(HWND taskbarWnd) {
    HWND foregroundWnd = GetForegroundWindow();
    if (!foregroundWnd || foregroundWnd == g_widgetWnd ||
        foregroundWnd == taskbarWnd || !IsWindowVisible(foregroundWnd)) {
        return false;
    }

    foregroundWnd = GetAncestor(foregroundWnd, GA_ROOT);
    if (!foregroundWnd || foregroundWnd == g_widgetWnd ||
        foregroundWnd == taskbarWnd || !IsWindowVisible(foregroundWnd)) {
        return false;
    }

    WCHAR className[64]{};
    GetClassNameW(foregroundWnd, className, ARRAYSIZE(className));
    if (_wcsicmp(className, L"Shell_TrayWnd") == 0 ||
        _wcsicmp(className, L"Progman") == 0 ||
        _wcsicmp(className, L"WorkerW") == 0) {
        return false;
    }

    HMONITOR monitor = MonitorFromWindow(foregroundWnd, MONITOR_DEFAULTTONULL);
    if (!monitor) {
        return false;
    }

    MONITORINFO monitorInfo{};
    monitorInfo.cbSize = sizeof(monitorInfo);
    if (!GetMonitorInfoW(monitor, &monitorInfo)) {
        return false;
    }

    RECT windowRect{};
    HRESULT hr = DwmGetWindowAttribute(foregroundWnd,
                                       DWMWA_EXTENDED_FRAME_BOUNDS,
                                       &windowRect, sizeof(windowRect));
    if (FAILED(hr)) {
        if (!GetWindowRect(foregroundWnd, &windowRect)) {
            return false;
        }
    }

    RECT monitorRect = monitorInfo.rcMonitor;
    constexpr int tolerance = 2;
    return windowRect.left <= monitorRect.left + tolerance &&
           windowRect.top <= monitorRect.top + tolerance &&
           windowRect.right >= monitorRect.right - tolerance &&
           windowRect.bottom >= monitorRect.bottom - tolerance;
}

std::wstring FormatTimeSpan(TimeSpan timeSpan) {
    auto totalSeconds =
        std::chrono::duration_cast<std::chrono::seconds>(timeSpan).count();
    if (totalSeconds < 0) {
        totalSeconds = 0;
    }

    long long minutes = totalSeconds / 60;
    long long seconds = totalSeconds % 60;

    WCHAR buffer[32];
    swprintf_s(buffer, L"%lld:%02lld", minutes, seconds);
    return buffer;
}

TimeSpan GetEffectivePosition() {
    if (!g_mediaState.hasSession) {
        return TimeSpan::zero();
    }

    TimeSpan position = g_mediaState.rawPosition;

    if (g_mediaState.status ==
            GlobalSystemMediaTransportControlsSessionPlaybackStatus::Playing &&
        g_mediaState.lastUpdatedTime.time_since_epoch() !=
            DateTime::duration::zero()) {
        position += std::chrono::duration_cast<TimeSpan>(
            winrt::clock::now() - g_mediaState.lastUpdatedTime);
    }

    if (position < g_mediaState.startTime) {
        position = g_mediaState.startTime;
    }

    if (g_mediaState.endTime > TimeSpan::zero() &&
        position > g_mediaState.endTime) {
        position = g_mediaState.endTime;
    }

    return position;
}

RECT GetProgressRect(RECT clientRect) {
    int progressHeight =
        3 + static_cast<int>(g_progressHoverProgress * 4.0f);
    RECT rc{
        clientRect.left + g_settings.innerPadding,
        clientRect.bottom - progressHeight,
        clientRect.right - g_settings.innerPadding,
        clientRect.bottom,
    };
    return rc;
}

RECT GetProgressHitRect(RECT clientRect) {
    RECT rc = GetProgressRect(clientRect);
    rc.top -= 8;
    rc.bottom = clientRect.bottom;
    return rc;
}

RECT GetCoverRect(RECT clientRect) {
    float height = static_cast<float>(clientRect.bottom - clientRect.top);
    float padding = static_cast<float>(g_settings.innerPadding);
    float coverSize = height - padding * 2.0f;
    if (coverSize > 30.0f) {
        coverSize = 30.0f;
    }
    if (coverSize < 26.0f) {
        coverSize = 26.0f;
    }

    int left = clientRect.left + g_settings.innerPadding;
    int top = clientRect.top + g_settings.innerPadding;
    int size = static_cast<int>(coverSize + 0.5f);
    return RECT{left, top, left + size, top + size};
}

RECT GetPrevButtonRect(RECT clientRect) {
    int size = 26;
    int centerY = (clientRect.top + clientRect.bottom) / 2 - 2;
    int x = clientRect.right - 182;
    return RECT{x, centerY - size / 2, x + size, centerY + size / 2};
}

RECT GetPlayButtonRect(RECT clientRect) {
    int size = 30;
    int centerY = (clientRect.top + clientRect.bottom) / 2 - 2;
    int x = clientRect.right - 148;
    return RECT{x, centerY - size / 2, x + size, centerY + size / 2};
}

RECT GetNextButtonRect(RECT clientRect) {
    int size = 26;
    int centerY = (clientRect.top + clientRect.bottom) / 2 - 2;
    int x = clientRect.right - 110;
    return RECT{x, centerY - size / 2, x + size, centerY + size / 2};
}

bool IsPlayOnlyModeActive() {
    return g_mediaState.hasSession && g_mediaState.playOnlySession &&
           (g_mediaState.canPlay || g_mediaState.canPlayPause);
}

bool IsPausedHoverModeActive() {
    return g_mediaState.hasSession && !g_mediaState.playOnlySession &&
           g_mediaState.status ==
               GlobalSystemMediaTransportControlsSessionPlaybackStatus::Paused;
}

RECT GetPlayOnlyButtonRect(RECT clientRect) {
    int size = 30;
    int centerX = (clientRect.left + clientRect.right) / 2;
    int centerY = (clientRect.top + clientRect.bottom) / 2;
    return RECT{centerX - size / 2, centerY - size / 2, centerX + size / 2,
                centerY + size / 2};
}

HotButton HitTestButton(RECT clientRect, POINT pt) {
    if (IsPlayOnlyModeActive()) {
        RECT playOnlyRect = GetPlayOnlyButtonRect(clientRect);
        if (pt.x >= playOnlyRect.left && pt.x < playOnlyRect.right &&
            pt.y >= playOnlyRect.top && pt.y < playOnlyRect.bottom) {
            return HotButton::PlayPause;
        }

        return HotButton::None;
    }

    RECT prevRect = GetPrevButtonRect(clientRect);
    RECT playRect = GetPlayButtonRect(clientRect);
    RECT nextRect = GetNextButtonRect(clientRect);

    if (pt.x >= prevRect.left && pt.x < prevRect.right &&
        pt.y >= prevRect.top && pt.y < prevRect.bottom) {
        return HotButton::Previous;
    }

    if (pt.x >= playRect.left && pt.x < playRect.right &&
        pt.y >= playRect.top && pt.y < playRect.bottom) {
        return HotButton::PlayPause;
    }

    if (pt.x >= nextRect.left && pt.x < nextRect.right &&
        pt.y >= nextRect.top && pt.y < nextRect.bottom) {
        return HotButton::Next;
    }

    return HotButton::None;
}

void TickAnimations() {
    bool hoverMode = g_hover || IsPausedHoverModeActive() ||
                     IsPlayOnlyModeActive();
    float hoverTarget = hoverMode ? 1.0f : 0.0f;
    g_hoverProgress = Approach(g_hoverProgress, hoverTarget, 0.12f);
    g_blurProgress = Approach(g_blurProgress, hoverTarget,
                              hoverMode ? 0.34f : 0.20f);

    g_prevHoverProgress =
        Approach(g_prevHoverProgress,
                 g_hover && g_hotButton == HotButton::Previous ? 1.0f : 0.0f,
                 0.18f);
    g_playHoverProgress =
        Approach(g_playHoverProgress,
                 g_hover && g_hotButton == HotButton::PlayPause ? 1.0f : 0.0f,
                 0.18f);
    g_nextHoverProgress =
        Approach(g_nextHoverProgress,
                 g_hover && g_hotButton == HotButton::Next ? 1.0f : 0.0f,
                 0.18f);
    g_progressHoverProgress =
        Approach(g_progressHoverProgress, g_progressHover ? 1.0f : 0.0f,
                 0.22f);
}

int GetIdleIconSizeForHeight(int height) {
    return ClampInt(height - 14, 24, 30);
}

int GetIdleIconGap() {
    return 10;
}

int GetIdleContentWidth(size_t count, int height) {
    if (count == 0) {
        return 0;
    }

    int iconSize = GetIdleIconSizeForHeight(height);
    int gap = GetIdleIconGap();
    return g_settings.innerPadding * 2 + static_cast<int>(count) * iconSize +
           static_cast<int>(count - 1) * gap;
}

size_t GetIdleVisibleCountForWidth(int width, int height) {
    if (g_idleApps.empty()) {
        return 0;
    }

    int iconSize = GetIdleIconSizeForHeight(height);
    int gap = GetIdleIconGap();
    int availableWidth = width - g_settings.innerPadding * 2;
    if (availableWidth < iconSize) {
        return 0;
    }

    size_t capacity =
        static_cast<size_t>((availableWidth + gap) / (iconSize + gap));
    if (capacity > g_idleApps.size()) {
        capacity = g_idleApps.size();
    }

    return capacity;
}

bool IsIdleModeActive() {
    return g_mediaPollCompleted && !g_mediaState.hasSession &&
           !g_idleApps.empty();
}

int GetCurrentWidgetWidth() {
    if (IsPlayOnlyModeActive()) {
        int width = g_settings.innerPadding * 2 + 30;
        return ClampInt(width, g_settings.widgetHeight, g_settings.widgetWidth);
    }

    if (!IsIdleModeActive()) {
        return g_settings.widgetWidth;
    }

    size_t count = GetIdleVisibleCountForWidth(g_settings.widgetWidth,
                                               g_settings.widgetHeight);
    if (count == 0) {
        return g_settings.widgetWidth;
    }

    return ClampInt(GetIdleContentWidth(count, g_settings.widgetHeight),
                    g_settings.widgetHeight, g_settings.widgetWidth);
}

void RepositionWidget() {
    HWND taskbarWnd = FindCurrentProcessTaskbarWnd();
    if (!taskbarWnd || !g_widgetWnd) {
        return;
    }

    RECT taskbarRect{};
    GetWindowRect(taskbarWnd, &taskbarRect);

    int taskbarHeight = taskbarRect.bottom - taskbarRect.top;
    int x = taskbarRect.left + g_settings.leftOffset;
    int y = taskbarRect.top +
            ClampInt((taskbarHeight - g_settings.widgetHeight) / 2, 0,
                     taskbarHeight);

    bool idleMode = IsIdleModeActive();
    bool shouldShow = !g_mediaPollCompleted || g_mediaState.hasSession ||
                      idleMode || !g_settings.hideWhenNoMedia;
    if (IsFullscreenForegroundWindow(taskbarWnd)) {
        shouldShow = false;
    }

    SetWindowPos(g_widgetWnd, HWND_TOPMOST, x, y, GetCurrentWidgetWidth(),
                 g_settings.widgetHeight,
                 SWP_NOACTIVATE | (shouldShow ? SWP_SHOWWINDOW
                                              : SWP_HIDEWINDOW));
}

bool HasMeaningfulMediaProperties(
    hstring const& title,
    hstring const& artist,
    TimeSpan startTime,
    TimeSpan endTime,
    GlobalSystemMediaTransportControlsSessionPlaybackControls const& controls,
    IRandomAccessStreamReference const& thumbnail) {
    std::wstring titleText = Trim(std::wstring(title.c_str()));
    std::wstring artistText = Trim(std::wstring(artist.c_str()));
    if (titleText.empty()) {
        return false;
    }

    bool hasArtist = !artistText.empty();
    bool hasDuration = endTime > startTime &&
                       (endTime - startTime) > std::chrono::seconds(2);
    bool hasMediaControls = controls.IsPreviousEnabled() ||
                            controls.IsNextEnabled() ||
                            controls.IsPlaybackPositionEnabled();
    bool hasThumbnail = static_cast<bool>(thumbnail);

    return hasArtist || hasDuration || hasMediaControls || hasThumbnail;
}

void PollMedia() {
    MediaState nextState;
    GlobalSystemMediaTransportControlsSession nextSession{nullptr};

    try {
        auto manager =
            GlobalSystemMediaTransportControlsSessionManager::RequestAsync()
                .get();

        auto currentSession = manager.GetCurrentSession();
        if (currentSession) {
            nextSession = currentSession;
        } else {
            auto sessions = manager.GetSessions();
            if (sessions.Size() > 0) {
                nextSession = sessions.GetAt(0);
            }
        }

        if (nextSession) {
            auto playbackInfo = nextSession.GetPlaybackInfo();
            auto controls = playbackInfo.Controls();
            auto timeline = nextSession.GetTimelineProperties();
            auto properties = nextSession.TryGetMediaPropertiesAsync().get();
            bool meaningfulMedia = HasMeaningfulMediaProperties(
                properties.Title(), properties.Artist(), timeline.StartTime(),
                timeline.EndTime(), controls, properties.Thumbnail());

            // Some apps publish a session before the first real track arrives.
            // Keep a compact play-only state if Windows says playback can start,
            // otherwise treat the session as absent and fall back to idle mode.
            if (!meaningfulMedia) {
                bool canPlayOnly =
                    controls.IsPlayEnabled() ||
                    controls.IsPlayPauseToggleEnabled();
                if (canPlayOnly) {
                    nextState.hasSession = true;
                    nextState.playOnlySession = true;
                    nextState.sourceAppId = nextSession.SourceAppUserModelId();
                    nextState.status = playbackInfo.PlaybackStatus();
                    nextState.canPlay = controls.IsPlayEnabled();
                    nextState.canPlayPause =
                        controls.IsPlayPauseToggleEnabled();
                } else {
                    nextSession = nullptr;
                }
            }
        }

        if (nextSession && !nextState.playOnlySession) {
            auto playbackInfo = nextSession.GetPlaybackInfo();
            auto controls = playbackInfo.Controls();
            auto timeline = nextSession.GetTimelineProperties();
            auto properties = nextSession.TryGetMediaPropertiesAsync().get();
            std::wstring thumbnailKey =
                std::wstring(nextSession.SourceAppUserModelId().c_str()) +
                L"|" + std::wstring(properties.Title().c_str()) + L"|" +
                std::wstring(properties.Artist().c_str());

            nextState.hasSession = true;
            nextState.sourceAppId = nextSession.SourceAppUserModelId();
            nextState.title = properties.Title();
            nextState.artist = properties.Artist();
            nextState.status = playbackInfo.PlaybackStatus();
            nextState.rawPosition = timeline.Position();
            nextState.startTime = timeline.StartTime();
            nextState.endTime = timeline.EndTime();
            nextState.lastUpdatedTime = timeline.LastUpdatedTime();
            nextState.canPrevious = controls.IsPreviousEnabled();
            nextState.canNext = controls.IsNextEnabled();
            nextState.canPlay = controls.IsPlayEnabled();
            nextState.canPlayPause = controls.IsPlayPauseToggleEnabled();
            nextState.canSeek = controls.IsPlaybackPositionEnabled();

            // Thumbnail reads are comparatively expensive and occasionally lag
            // behind metadata. Cache by source/title/artist and retry on changes.
            if (thumbnailKey != g_thumbnailKey || !g_thumbnailBitmap) {
                auto thumbnailBitmap =
                    BitmapFromThumbnail(properties.Thumbnail());
                if (thumbnailBitmap) {
                    g_thumbnailBitmap = std::move(thumbnailBitmap);
                    g_thumbnailKey = thumbnailKey;
                    g_artworkAccentColor =
                        AverageArtworkColor(g_thumbnailBitmap.get());
                    g_hasArtworkColor = true;
                    if (g_widgetWnd) {
                        ApplySystemBackdrop(g_widgetWnd);
                    }
                } else if (thumbnailKey != g_thumbnailKey) {
                    g_thumbnailBitmap.reset();
                    g_thumbnailKey = thumbnailKey;
                    g_hasArtworkColor = false;
                    if (g_widgetWnd) {
                        ApplySystemBackdrop(g_widgetWnd);
                    }
                }
            }
        } else {
            g_thumbnailBitmap.reset();
            g_thumbnailKey.clear();
            g_hasArtworkColor = false;
            if (g_widgetWnd) {
                ApplySystemBackdrop(g_widgetWnd);
            }
        }
    } catch (hresult_error const& e) {
        Wh_Log(L"PollMedia error: 0x%08X %s",
               static_cast<uint32_t>(e.code()), e.message().c_str());
    } catch (...) {
        Wh_Log(L"PollMedia error: unknown exception");
    }

    g_mediaSession = nextSession;
    g_mediaState = nextState;
    g_mediaPollCompleted = true;
    if (g_mediaState.hasSession) {
        g_lastMediaSeenTick = GetTickCount();
    }

    RepositionWidget();
    if (g_widgetWnd) {
        RefreshWidget();
    }
}

D2D1_COLOR_F ToD2DColor(COLORREF color, float alpha = 1.0f) {
    return D2D1::ColorF(GetRValue(color) / 255.0f, GetGValue(color) / 255.0f,
                        GetBValue(color) / 255.0f, alpha);
}

D2D1_COLOR_F ToD2DColor(Color color) {
    return D2D1::ColorF(color.GetRed() / 255.0f, color.GetGreen() / 255.0f,
                        color.GetBlue() / 255.0f, color.GetAlpha() / 255.0f);
}

D2D1_RECT_F ToD2DRect(RectF rect) {
    return D2D1::RectF(rect.X, rect.Y, rect.X + rect.Width,
                       rect.Y + rect.Height);
}

D2D1_ROUNDED_RECT ToD2DRoundedRect(RectF rect, float radius) {
    return D2D1::RoundedRect(ToD2DRect(rect), radius, radius);
}

bool EnsureD2DResources() {
    if (!g_d2dFactory) {
        if (FAILED(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED,
                                     g_d2dFactory.put()))) {
            return false;
        }
    }

    if (!g_dwriteFactory) {
        ::IUnknown* factory = nullptr;
        if (FAILED(DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED,
                                       IID_IDWriteFactoryLocal, &factory))) {
            return false;
        }

        g_dwriteFactory.attach(reinterpret_cast<IDWriteFactory*>(factory));
    }

    if (!g_d2dRenderTarget) {
        // A DC render target keeps the widget lightweight: explorer owns the
        // window, DWM owns the backdrop, and Direct2D only paints the content.
        D2D1_RENDER_TARGET_PROPERTIES properties =
            D2D1::RenderTargetProperties(
                D2D1_RENDER_TARGET_TYPE_DEFAULT,
                D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM,
                                  D2D1_ALPHA_MODE_PREMULTIPLIED),
                0.0f, 0.0f, D2D1_RENDER_TARGET_USAGE_NONE,
                D2D1_FEATURE_LEVEL_DEFAULT);

        if (FAILED(
                g_d2dFactory->CreateDCRenderTarget(&properties,
                                                   g_d2dRenderTarget.put()))) {
            return false;
        }

        g_d2dRenderTarget->SetTextAntialiasMode(
            D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE);
    }

    return true;
}

void D2DFillRoundedRect(ID2D1RenderTarget* target,
                        RectF rect,
                        float radius,
                        D2D1_COLOR_F color) {
    com_ptr<ID2D1SolidColorBrush> brush;
    if (SUCCEEDED(target->CreateSolidColorBrush(color, brush.put()))) {
        target->FillRoundedRectangle(ToD2DRoundedRect(rect, radius),
                                     brush.get());
    }
}

void D2DDrawRoundedRect(ID2D1RenderTarget* target,
                        RectF rect,
                        float radius,
                        D2D1_COLOR_F color,
                        float strokeWidth = 1.0f) {
    com_ptr<ID2D1SolidColorBrush> brush;
    if (SUCCEEDED(target->CreateSolidColorBrush(color, brush.put()))) {
        target->DrawRoundedRectangle(ToD2DRoundedRect(rect, radius),
                                     brush.get(), strokeWidth);
    }
}

void D2DFillEllipse(ID2D1RenderTarget* target,
                    float x,
                    float y,
                    float radius,
                    D2D1_COLOR_F color) {
    com_ptr<ID2D1SolidColorBrush> brush;
    if (SUCCEEDED(target->CreateSolidColorBrush(color, brush.put()))) {
        target->FillEllipse(D2D1::Ellipse(D2D1::Point2F(x, y), radius, radius),
                            brush.get());
    }
}

void D2DDrawTextLine(ID2D1RenderTarget* target,
                     PCWSTR text,
                     RectF rect,
                     COLORREF color,
                     float alpha,
                     float size,
                     DWRITE_FONT_WEIGHT weight = DWRITE_FONT_WEIGHT_REGULAR,
                     DWRITE_TEXT_ALIGNMENT alignment =
                         DWRITE_TEXT_ALIGNMENT_LEADING,
                     DWRITE_PARAGRAPH_ALIGNMENT paragraphAlignment =
                         DWRITE_PARAGRAPH_ALIGNMENT_CENTER,
                     PCWSTR fontName = L"Segoe UI") {
    if (!g_dwriteFactory) {
        return;
    }

    com_ptr<IDWriteTextFormat> format;
    if (FAILED(g_dwriteFactory->CreateTextFormat(
            fontName, nullptr, weight, DWRITE_FONT_STYLE_NORMAL,
            DWRITE_FONT_STRETCH_NORMAL, size, L"", format.put()))) {
        return;
    }

    format->SetTextAlignment(alignment);
    format->SetParagraphAlignment(paragraphAlignment);
    format->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);

    DWRITE_TRIMMING trimming{};
    trimming.granularity = DWRITE_TRIMMING_GRANULARITY_CHARACTER;
    com_ptr<IDWriteInlineObject> ellipsis;
    if (SUCCEEDED(
            g_dwriteFactory->CreateEllipsisTrimmingSign(format.get(),
                                                       ellipsis.put()))) {
        format->SetTrimming(&trimming, ellipsis.get());
    }

    com_ptr<ID2D1SolidColorBrush> brush;
    if (FAILED(target->CreateSolidColorBrush(ToD2DColor(color, alpha),
                                             brush.put()))) {
        return;
    }

    PCWSTR safeText = text && text[0] ? text : L" ";
    target->DrawText(safeText, static_cast<UINT32>(wcslen(safeText)),
                     format.get(), ToD2DRect(rect), brush.get(),
                     D2D1_DRAW_TEXT_OPTIONS_CLIP);
}

void D2DDrawSystemIcon(ID2D1RenderTarget* target,
                       PCWSTR glyph,
                       RectF rect,
                       COLORREF color,
                       float alpha,
                       float size,
                       bool strong = false) {
    D2DDrawTextLine(target, glyph, rect, color, alpha, size,
                    strong ? DWRITE_FONT_WEIGHT_SEMI_BOLD
                           : DWRITE_FONT_WEIGHT_REGULAR,
                    DWRITE_TEXT_ALIGNMENT_CENTER,
                    DWRITE_PARAGRAPH_ALIGNMENT_CENTER,
                    L"Segoe Fluent Icons");
}

bool EnsureD2DThumbnail(ID2D1RenderTarget* target) {
    if (!g_thumbnailBitmap) {
        g_d2dThumbnailBitmap = nullptr;
        g_d2dThumbnailKey.clear();
        return false;
    }

    if (g_d2dThumbnailBitmap && g_d2dThumbnailKey == g_thumbnailKey) {
        return true;
    }

    g_d2dThumbnailBitmap = nullptr;
    g_d2dThumbnailKey.clear();

    UINT width = g_thumbnailBitmap->GetWidth();
    UINT height = g_thumbnailBitmap->GetHeight();
    if (width == 0 || height == 0) {
        return false;
    }

    Gdiplus::Rect lockRect(0, 0, width, height);
    Gdiplus::BitmapData data{};
    if (g_thumbnailBitmap->LockBits(&lockRect, Gdiplus::ImageLockModeRead,
                                    PixelFormat32bppPARGB,
                                    &data) != Gdiplus::Ok) {
        return false;
    }

    D2D1_BITMAP_PROPERTIES properties =
        D2D1::BitmapProperties(D2D1::PixelFormat(
            DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED));

    HRESULT hr = target->CreateBitmap(D2D1::SizeU(width, height),
                                      data.Scan0, data.Stride, &properties,
                                      g_d2dThumbnailBitmap.put());
    g_thumbnailBitmap->UnlockBits(&data);

    if (FAILED(hr)) {
        g_d2dThumbnailBitmap = nullptr;
        return false;
    }

    g_d2dThumbnailKey = g_thumbnailKey;
    return true;
}

bool CreateD2DBitmapFromGdiBitmap(Bitmap* bitmap,
                                  ID2D1RenderTarget* target,
                                  com_ptr<ID2D1Bitmap>& outBitmap) {
    if (!bitmap || bitmap->GetWidth() == 0 || bitmap->GetHeight() == 0) {
        return false;
    }

    UINT width = bitmap->GetWidth();
    UINT height = bitmap->GetHeight();
    Gdiplus::Rect lockRect(0, 0, width, height);
    Gdiplus::BitmapData data{};
    if (bitmap->LockBits(&lockRect, Gdiplus::ImageLockModeRead,
                         PixelFormat32bppPARGB, &data) != Gdiplus::Ok) {
        return false;
    }

    D2D1_BITMAP_PROPERTIES properties =
        D2D1::BitmapProperties(D2D1::PixelFormat(
            DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED));

    HRESULT hr = target->CreateBitmap(D2D1::SizeU(width, height), data.Scan0,
                                      data.Stride, &properties,
                                      outBitmap.put());
    bitmap->UnlockBits(&data);
    return SUCCEEDED(hr);
}

void ResetIdleD2DIcons() {
    for (auto& app : g_idleApps) {
        app.d2dIconBitmap = nullptr;
    }
}

bool EnsureD2DIdleIcon(size_t index, ID2D1RenderTarget* target) {
    if (index >= g_idleApps.size() || !g_idleApps[index].iconBitmap) {
        return false;
    }

    if (g_idleApps[index].d2dIconBitmap) {
        return true;
    }

    return CreateD2DBitmapFromGdiBitmap(g_idleApps[index].iconBitmap.get(),
                                        target,
                                        g_idleApps[index].d2dIconBitmap);
}

RECT GetIdleAppRect(RECT clientRect, size_t index) {
    int width = clientRect.right - clientRect.left;
    int height = clientRect.bottom - clientRect.top;
    size_t count = GetIdleVisibleCountForWidth(width, height);

    int iconSize = GetIdleIconSizeForHeight(height);
    int gap = GetIdleIconGap();
    int totalWidth = static_cast<int>(count) * iconSize +
                     static_cast<int>(count > 0 ? count - 1 : 0) * gap;
    int startX = clientRect.left +
                 ((clientRect.right - clientRect.left) - totalWidth) / 2;
    int y = clientRect.top +
            ((clientRect.bottom - clientRect.top) - iconSize) / 2;
    int x = startX + static_cast<int>(index) * (iconSize + gap);

    return RECT{x, y, x + iconSize, y + iconSize};
}

int HitTestIdleApp(RECT clientRect, POINT pt) {
    size_t count = GetIdleVisibleCountForWidth(
        clientRect.right - clientRect.left,
        clientRect.bottom - clientRect.top);

    for (size_t i = 0; i < count; ++i) {
        RECT rc = GetIdleAppRect(clientRect, i);
        if (pt.x >= rc.left && pt.x < rc.right && pt.y >= rc.top &&
            pt.y < rc.bottom) {
            return static_cast<int>(i);
        }
    }

    return -1;
}

bool ShellExecutePath(std::wstring const& path);

bool ActivateAppUserModelId(std::wstring const& appUserModelId) {
    if (appUserModelId.empty()) {
        return false;
    }

    try {
        com_ptr<IApplicationActivationManagerLocal> activationManager;
        HRESULT hr = CoCreateInstance(
            CLSID_ApplicationActivationManagerLocal, nullptr,
            CLSCTX_INPROC_SERVER, IID_IApplicationActivationManagerLocal,
            activationManager.put_void());
        if (FAILED(hr)) {
            Wh_Log(L"CoCreateInstance(IApplicationActivationManager) failed: "
                   L"0x%08X",
                   static_cast<uint32_t>(hr));
            return false;
        }

        DWORD processId = 0;
        hr = activationManager->ActivateApplication(
            appUserModelId.c_str(), nullptr, 0, &processId);
        if (SUCCEEDED(hr)) {
            return true;
        }

        Wh_Log(L"ActivateApplication(%s) failed: 0x%08X",
               appUserModelId.c_str(), static_cast<uint32_t>(hr));
    } catch (...) {
        Wh_Log(L"ActivateAppUserModelId failed");
    }

    return false;
}

void LaunchIdleApp(size_t index) {
    if (index >= g_idleApps.size()) {
        return;
    }

    IdleApp const& app = g_idleApps[index];

    // Prefer the AppUserModelID path for Store/UWP apps. ShellExecute on the
    // .lnk is still useful for classic desktop shortcuts and broken metadata.
    if (ActivateAppUserModelId(app.activationAppId)) {
        return;
    }

    if (!app.activationAppId.empty() &&
        ShellExecutePath(L"shell:AppsFolder\\" + app.activationAppId)) {
        return;
    }

    if (app.launchPath.empty()) {
        return;
    }

    HINSTANCE result = ShellExecuteW(nullptr, L"open", app.launchPath.c_str(),
                                     nullptr, nullptr, SW_SHOWNORMAL);
    if (reinterpret_cast<INT_PTR>(result) <= 32) {
        Wh_Log(L"ShellExecuteW(%s) failed: %Id",
               app.launchPath.c_str(), reinterpret_cast<INT_PTR>(result));
    }
}

bool ShellExecutePath(std::wstring const& path) {
    HINSTANCE result = ShellExecuteW(nullptr, L"open", path.c_str(), nullptr,
                                     nullptr, SW_SHOWNORMAL);
    return reinterpret_cast<INT_PTR>(result) > 32;
}

bool IdleAppMatchesSource(IdleApp const& app, std::wstring const& lowerSource) {
    std::wstring lowerName = ToLower(app.name);
    std::wstring lowerAppSource = ToLower(app.sourceAppId);

    if (!lowerAppSource.empty() &&
        (lowerSource.find(lowerAppSource) != std::wstring::npos ||
         lowerAppSource.find(lowerSource) != std::wstring::npos)) {
        return true;
    }

    if (lowerSource.find(L"yandex") != std::wstring::npos &&
        (lowerName.find(L"yandex") != std::wstring::npos ||
         lowerName.find(L"яндекс") != std::wstring::npos)) {
        return true;
    }

    if (lowerSource.find(L"spotify") != std::wstring::npos &&
        lowerName.find(L"spotify") != std::wstring::npos) {
        return true;
    }

    if (lowerSource.find(L"apple") != std::wstring::npos &&
        lowerName.find(L"apple") != std::wstring::npos) {
        return true;
    }

    if (lowerSource.find(L"vlc") != std::wstring::npos &&
        lowerName.find(L"vlc") != std::wstring::npos) {
        return true;
    }

    if (lowerSource.find(L"aimp") != std::wstring::npos &&
        lowerName.find(L"aimp") != std::wstring::npos) {
        return true;
    }

    return false;
}

bool LaunchIdleAppForSource(std::wstring const& sourceAppId) {
    std::wstring lowerSource = ToLower(sourceAppId);
    for (size_t i = 0; i < g_idleApps.size(); ++i) {
        if (IdleAppMatchesSource(g_idleApps[i], lowerSource)) {
            LaunchIdleApp(i);
            return true;
        }
    }

    return false;
}

void D2DDrawCover(ID2D1RenderTarget* target, RectF coverRect) {
    float radius = static_cast<float>(g_settings.coverRadius);
    D2DFillRoundedRect(target, coverRect, radius,
                       D2D1::ColorF(48 / 255.0f, 61 / 255.0f, 84 / 255.0f,
                                    1.0f));

    if (EnsureD2DThumbnail(target)) {
        com_ptr<ID2D1RoundedRectangleGeometry> geometry;
        com_ptr<ID2D1Layer> layer;
        // Direct2D bitmap drawing has no rounded-corner flag; clip with a layer
        // so album art and idle icons keep the same radius as the widget.
        if (SUCCEEDED(g_d2dFactory->CreateRoundedRectangleGeometry(
                ToD2DRoundedRect(coverRect, radius), geometry.put())) &&
            SUCCEEDED(target->CreateLayer(nullptr, layer.put()))) {
            target->PushLayer(D2D1::LayerParameters(ToD2DRect(coverRect),
                                                    geometry.get()),
                              layer.get());

            D2D1_SIZE_F imageSize = g_d2dThumbnailBitmap->GetSize();
            float sourceSize =
                imageSize.width < imageSize.height ? imageSize.width
                                                   : imageSize.height;
            float sourceX = (imageSize.width - sourceSize) / 2.0f;
            float sourceY = (imageSize.height - sourceSize) / 2.0f;

            target->DrawBitmap(
                g_d2dThumbnailBitmap.get(), ToD2DRect(coverRect), 1.0f,
                D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,
                D2D1::RectF(sourceX, sourceY, sourceX + sourceSize,
                            sourceY + sourceSize));
            target->PopLayer();
        }

        D2DDrawRoundedRect(target, coverRect, radius,
                           D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.62f), 1.0f);
    } else {
        D2DDrawSystemIcon(target, L"\xE189", coverRect, ActiveAccentColor(),
                          1.0f, 15.0f, true);
    }
}

bool PaintWidgetD2D(HWND hwnd, HDC hdc, bool fluentContentOnly = false) {
    if (!EnsureD2DResources()) {
        return false;
    }

    RECT clientRect{};
    GetClientRect(hwnd, &clientRect);
    float width = static_cast<float>(clientRect.right - clientRect.left);
    float height = static_cast<float>(clientRect.bottom - clientRect.top);
    if (width <= 1.0f || height <= 1.0f) {
        return false;
    }

    HRESULT bindResult = g_d2dRenderTarget->BindDC(hdc, &clientRect);
    if (FAILED(bindResult)) {
        Wh_Log(L"PaintWidgetD2D: BindDC failed hr=0x%08X",
               static_cast<unsigned>(bindResult));
        return false;
    }

    float padding = static_cast<float>(g_settings.innerPadding);
    float progressHeight = 3.0f + g_progressHoverProgress * 4.0f;
    float progressY = height - progressHeight;
    float coverSize = height - padding * 2.0f;
    if (coverSize > 30.0f) {
        coverSize = 30.0f;
    }
    if (coverSize < 26.0f) {
        coverSize = 26.0f;
    }

    float coverY = padding;
    RectF widgetRect(0.5f, 0.5f, width - 1.0f, height - 1.0f);
    RectF coverRect(padding, coverY, coverSize, coverSize);
    float outerRadius = static_cast<float>(g_settings.borderRadius);
    bool idleMode = IsIdleModeActive();

    auto drawBackground = [&]() {
        if (fluentContentOnly) {
            g_d2dRenderTarget->Clear(D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.0f));
            D2DDrawRoundedRect(g_d2dRenderTarget.get(), widgetRect,
                               outerRadius,
                               ToD2DColor(ActiveBorderColor(),
                                          UseLightTheme() ? 0.56f : 0.42f),
                               1.0f);
            return;
        }

        g_d2dRenderTarget->Clear(ToD2DColor(ActiveBackgroundColor(), 1.0f));

        float backgroundAlpha =
            g_settings.useSystemBlur ? (UseLightTheme() ? 0.72f : 0.34f)
                                     : 0.94f;
        float borderAlpha =
            g_settings.useSystemBlur ? (UseLightTheme() ? 0.70f : 0.56f)
                                     : 0.82f;
        D2DFillRoundedRect(g_d2dRenderTarget.get(), widgetRect, outerRadius,
                           ToD2DColor(ActiveBackgroundColor(),
                                      backgroundAlpha));
        D2DDrawRoundedRect(g_d2dRenderTarget.get(), widgetRect, outerRadius,
                           ToD2DColor(ActiveBorderColor(), borderAlpha), 1.0f);
    };

    auto drawIdleApps = [&]() {
        size_t count = GetIdleVisibleCountForWidth(
            static_cast<int>(width), static_cast<int>(height));

        for (size_t i = 0; i < count; ++i) {
            RECT iconRc = GetIdleAppRect(clientRect, i);
            RectF iconRect(static_cast<float>(iconRc.left),
                           static_cast<float>(iconRc.top),
                           static_cast<float>(iconRc.right - iconRc.left),
                           static_cast<float>(iconRc.bottom - iconRc.top));
            float iconRadius = 7.0f;

            if (EnsureD2DIdleIcon(i, g_d2dRenderTarget.get())) {
                com_ptr<ID2D1RoundedRectangleGeometry> geometry;
                com_ptr<ID2D1Layer> layer;
                if (SUCCEEDED(g_d2dFactory->CreateRoundedRectangleGeometry(
                        ToD2DRoundedRect(iconRect, iconRadius),
                        geometry.put())) &&
                    SUCCEEDED(g_d2dRenderTarget->CreateLayer(nullptr,
                                                             layer.put()))) {
                    g_d2dRenderTarget->PushLayer(
                        D2D1::LayerParameters(ToD2DRect(iconRect),
                                              geometry.get()),
                        layer.get());
                    g_d2dRenderTarget->DrawBitmap(
                        g_idleApps[i].d2dIconBitmap.get(),
                        ToD2DRect(iconRect), 1.0f,
                        D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
                    g_d2dRenderTarget->PopLayer();
                } else {
                    g_d2dRenderTarget->DrawBitmap(
                        g_idleApps[i].d2dIconBitmap.get(),
                        ToD2DRect(iconRect), 1.0f,
                        D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
                }
            } else {
                D2DDrawSystemIcon(g_d2dRenderTarget.get(), L"\xE189",
                                  iconRect, ActiveAccentColor(), 1.0f, 15.0f,
                                  true);
            }
        }
    };

    auto drawPlayOnly = [&]() {
        drawBackground();

        RECT playRect = GetPlayOnlyButtonRect(clientRect);
        RectF playRectF(static_cast<float>(playRect.left),
                        static_cast<float>(playRect.top),
                        static_cast<float>(playRect.right - playRect.left),
                        static_cast<float>(playRect.bottom - playRect.top));

        COLORREF accent = ActiveAccentColor();
        COLORREF playColor =
            MixColor(accent, RGB(255, 255, 255), g_playHoverProgress * 0.20f);
        if (g_playHoverProgress > 0.01f) {
            RectF playGlow(playRectF.X - 2.0f, playRectF.Y - 2.0f,
                           playRectF.Width + 4.0f, playRectF.Height + 4.0f);
            D2DFillRoundedRect(g_d2dRenderTarget.get(), playGlow, 17.0f,
                               D2D1::ColorF(1.0f, 1.0f, 1.0f,
                                            g_playHoverProgress * 0.20f));
        }

        D2DFillRoundedRect(g_d2dRenderTarget.get(), playRectF, 15.0f,
                           ToD2DColor(playColor, 1.0f));
        D2DDrawRoundedRect(g_d2dRenderTarget.get(), playRectF, 15.0f,
                           ToD2DColor(accent, 1.0f), 1.0f);
        D2DDrawSystemIcon(g_d2dRenderTarget.get(), L"\xE768", playRectF,
                          RGB(255, 255, 255), 1.0f, 13.0f, true);
    };

    auto drawBase = [&]() {
        drawBackground();
        D2DDrawCover(g_d2dRenderTarget.get(), coverRect);

        float textX = coverRect.X + coverRect.Width + 9.0f;
        float textRightPadding = padding + 4.0f;
        float textWidth = width - textX - textRightPadding;
        float textTop = padding + 0.5f;
        RectF titleRect(textX, textTop, textWidth, 14.0f);
        RectF artistRect(textX, textTop + 14.0f, textWidth, 12.0f);

        PCWSTR title = g_mediaState.hasSession && !g_mediaState.title.empty()
                           ? g_mediaState.title.c_str()
                           : L"No media";
        PCWSTR artist = g_mediaState.hasSession && !g_mediaState.artist.empty()
                            ? g_mediaState.artist.c_str()
                            : L"";

        float textAlpha = 1.0f - g_hoverProgress;
        if (textAlpha > 0.01f) {
            D2DDrawTextLine(g_d2dRenderTarget.get(), title, titleRect,
                            ActiveTextColor(), textAlpha, 12.0f,
                            DWRITE_FONT_WEIGHT_SEMI_BOLD);
            D2DDrawTextLine(g_d2dRenderTarget.get(), artist, artistRect,
                            ActiveMutedTextColor(), textAlpha * 0.95f, 11.0f);
        }
    };

    auto drawProgress = [&]() {
        RectF progressRect(padding, progressY, width - padding * 2.0f,
                           progressHeight);
        D2DFillRoundedRect(g_d2dRenderTarget.get(), progressRect,
                           progressHeight / 2.0f,
                           D2D1::ColorF(58 / 255.0f, 67 / 255.0f,
                                        84 / 255.0f, 0.58f));

        if (g_mediaState.hasSession &&
            g_mediaState.endTime > TimeSpan::zero()) {
            TimeSpan pos = GetEffectivePosition();
            auto posMs = std::chrono::duration_cast<std::chrono::milliseconds>(
                             pos)
                             .count();
            auto totalMs =
                std::chrono::duration_cast<std::chrono::milliseconds>(
                    g_mediaState.endTime - g_mediaState.startTime)
                    .count();

            if (totalMs > 0) {
                float fillWidth = static_cast<float>(ClampLongLong(
                    static_cast<long long>((posMs * progressRect.Width) /
                                           totalMs),
                    0, static_cast<long long>(progressRect.Width)));
                RectF filled(progressRect.X, progressRect.Y, fillWidth,
                             progressRect.Height);
                D2DFillRoundedRect(g_d2dRenderTarget.get(), filled,
                                   progressHeight / 2.0f,
                                   ToD2DColor(ActiveAccentColor(), 1.0f));
            }

            if (g_progressHoverProgress > 0.01f && g_mediaState.canSeek) {
                float previewX = static_cast<float>(ClampInt(
                    g_progressHoverX, static_cast<int>(progressRect.X),
                    static_cast<int>(progressRect.X + progressRect.Width)));
                float ratio = (previewX - progressRect.X) / progressRect.Width;
                if (ratio < 0.0f) {
                    ratio = 0.0f;
                }
                if (ratio > 1.0f) {
                    ratio = 1.0f;
                }

                TimeSpan duration = g_mediaState.endTime - g_mediaState.startTime;
                TimeSpan previewTime{static_cast<TimeSpan::rep>(
                    duration.count() * static_cast<double>(ratio))};

                float previewAlpha = g_progressHoverProgress;
                RectF previewFill(progressRect.X, progressRect.Y,
                                  previewX - progressRect.X,
                                  progressRect.Height);
                if (previewFill.Width > 0.0f) {
                    D2DFillRoundedRect(
                        g_d2dRenderTarget.get(), previewFill,
                        progressHeight / 2.0f,
                        ToD2DColor(MixColor(ActiveAccentColor(),
                                            RGB(255, 255, 255), 0.36f),
                                   previewAlpha * 0.42f));
                }

                float thumbRadius = progressHeight / 2.0f;
                D2DFillEllipse(g_d2dRenderTarget.get(), previewX,
                               progressRect.Y + progressRect.Height / 2.0f,
                               thumbRadius,
                               D2D1::ColorF(1.0f, 1.0f, 1.0f,
                                            previewAlpha));
                D2DFillEllipse(g_d2dRenderTarget.get(), previewX,
                               progressRect.Y + progressRect.Height / 2.0f,
                               thumbRadius - 1.4f,
                               ToD2DColor(ActiveAccentColor(), previewAlpha));

                std::wstring previewText = FormatTimeSpan(previewTime);
                float bubbleWidth = 42.0f;
                float bubbleHeight = 16.0f;
                float bubbleX = previewX - bubbleWidth / 2.0f;
                if (bubbleX < padding) {
                    bubbleX = padding;
                }
                if (bubbleX + bubbleWidth > width - padding) {
                    bubbleX = width - padding - bubbleWidth;
                }
                RectF bubbleRect(bubbleX, progressRect.Y - bubbleHeight - 4.0f,
                                 bubbleWidth, bubbleHeight);
                D2DFillRoundedRect(
                    g_d2dRenderTarget.get(), bubbleRect, 6.0f,
                    D2D1::ColorF(10 / 255.0f, 14 / 255.0f, 22 / 255.0f,
                                 previewAlpha * 0.74f));
                D2DDrawRoundedRect(
                    g_d2dRenderTarget.get(), bubbleRect, 6.0f,
                    D2D1::ColorF(1.0f, 1.0f, 1.0f, previewAlpha * 0.14f),
                    1.0f);
                D2DDrawTextLine(g_d2dRenderTarget.get(), previewText.c_str(),
                                bubbleRect, RGB(255, 255, 255),
                                previewAlpha, 9.5f,
                                DWRITE_FONT_WEIGHT_MEDIUM,
                                DWRITE_TEXT_ALIGNMENT_CENTER);
            }
        }
    };

    auto drawControls = [&]() {
        if (g_hoverProgress <= 0.01f) {
            return;
        }

        RECT prevRect = GetPrevButtonRect(clientRect);
        RECT playRect = GetPlayButtonRect(clientRect);
        RECT nextRect = GetNextButtonRect(clientRect);
        RectF prevRectF(static_cast<float>(prevRect.left),
                        static_cast<float>(prevRect.top),
                        static_cast<float>(prevRect.right - prevRect.left),
                        static_cast<float>(prevRect.bottom - prevRect.top));
        RectF playRectF(static_cast<float>(playRect.left),
                        static_cast<float>(playRect.top),
                        static_cast<float>(playRect.right - playRect.left),
                        static_cast<float>(playRect.bottom - playRect.top));
        RectF nextRectF(static_cast<float>(nextRect.left),
                        static_cast<float>(nextRect.top),
                        static_cast<float>(nextRect.right - nextRect.left),
                        static_cast<float>(nextRect.bottom - nextRect.top));

        float controlsAlpha = g_hoverProgress;
        if (g_prevHoverProgress > 0.01f) {
            D2DFillRoundedRect(g_d2dRenderTarget.get(), prevRectF, 13.0f,
                               D2D1::ColorF(1.0f, 1.0f, 1.0f,
                                            g_prevHoverProgress * 0.18f));
        }

        D2DDrawSystemIcon(g_d2dRenderTarget.get(), L"\xE892", prevRectF,
                          g_mediaState.canPrevious
                              ? ActiveTextColor()
                              : RGB(100, 106, 116),
                          controlsAlpha, 14.0f);

        COLORREF accent = ActiveAccentColor();
        COLORREF playColor =
            MixColor(accent, RGB(255, 255, 255), g_playHoverProgress * 0.20f);
        if (g_playHoverProgress > 0.01f) {
            RectF playGlow(playRectF.X - 2.0f, playRectF.Y - 2.0f,
                           playRectF.Width + 4.0f, playRectF.Height + 4.0f);
            D2DFillRoundedRect(g_d2dRenderTarget.get(), playGlow, 17.0f,
                               D2D1::ColorF(1.0f, 1.0f, 1.0f,
                                            g_playHoverProgress * 0.20f));
        }

        D2DFillRoundedRect(g_d2dRenderTarget.get(), playRectF, 15.0f,
                           ToD2DColor(playColor, controlsAlpha));
        D2DDrawRoundedRect(g_d2dRenderTarget.get(), playRectF, 15.0f,
                           ToD2DColor(accent, controlsAlpha), 1.0f);

        bool playing =
            g_mediaState.status ==
            GlobalSystemMediaTransportControlsSessionPlaybackStatus::Playing;
        D2DDrawSystemIcon(g_d2dRenderTarget.get(),
                          playing ? L"\xE769" : L"\xE768", playRectF,
                          RGB(255, 255, 255), controlsAlpha, 13.0f, true);

        if (g_nextHoverProgress > 0.01f) {
            D2DFillRoundedRect(g_d2dRenderTarget.get(), nextRectF, 13.0f,
                               D2D1::ColorF(1.0f, 1.0f, 1.0f,
                                            g_nextHoverProgress * 0.18f));
        }

        D2DDrawSystemIcon(g_d2dRenderTarget.get(), L"\xE893", nextRectF,
                          g_mediaState.canNext ? ActiveTextColor()
                                               : RGB(100, 106, 116),
                          controlsAlpha, 14.0f);

        RectF timeRect(width - padding - 62.0f, height - 21.0f, 58.0f, 14.0f);
        std::wstring timeText =
            FormatTimeSpan(GetEffectivePosition()) + L" / " +
            FormatTimeSpan(g_mediaState.endTime - g_mediaState.startTime);
        D2DDrawTextLine(g_d2dRenderTarget.get(), timeText.c_str(), timeRect,
                        ActiveMutedTextColor(), controlsAlpha, 10.0f,
                        DWRITE_FONT_WEIGHT_REGULAR,
                        DWRITE_TEXT_ALIGNMENT_TRAILING);
    };

    auto drawProgressPreview = [&]() {
        if (!g_progressHover || !g_mediaState.canSeek || !g_mediaState.hasSession ||
            g_mediaState.endTime <= g_mediaState.startTime) {
            return;
        }

        RectF progressRect(padding, progressY, width - padding * 2.0f,
                           progressHeight);
        float previewX = static_cast<float>(ClampInt(
            g_progressHoverX, static_cast<int>(progressRect.X),
            static_cast<int>(progressRect.X + progressRect.Width)));
        float ratio = (previewX - progressRect.X) / progressRect.Width;
        if (ratio < 0.0f) {
            ratio = 0.0f;
        }
        if (ratio > 1.0f) {
            ratio = 1.0f;
        }

        TimeSpan duration = g_mediaState.endTime - g_mediaState.startTime;
        TimeSpan previewTime{static_cast<TimeSpan::rep>(
            duration.count() * static_cast<double>(ratio))};

        float previewAlpha = 1.0f;
        RectF previewFill(progressRect.X, progressRect.Y,
                          previewX - progressRect.X, progressRect.Height);
        if (previewFill.Width > 0.0f) {
            D2DFillRoundedRect(
                g_d2dRenderTarget.get(), previewFill, progressHeight / 2.0f,
                ToD2DColor(MixColor(ActiveAccentColor(), RGB(255, 255, 255),
                                    0.36f),
                           previewAlpha * 0.48f));
        }

        float thumbRadius = progressRect.Height / 2.0f;
        float thumbY = progressRect.Y + progressRect.Height / 2.0f;
        D2DFillEllipse(g_d2dRenderTarget.get(), previewX, thumbY, thumbRadius,
                       D2D1::ColorF(1.0f, 1.0f, 1.0f, previewAlpha));
        D2DFillEllipse(g_d2dRenderTarget.get(), previewX, thumbY,
                       thumbRadius - 1.0f,
                       ToD2DColor(ActiveAccentColor(), previewAlpha));

        std::wstring previewText = FormatTimeSpan(previewTime);
        float bubbleWidth = 42.0f;
        float bubbleHeight = 16.0f;
        float bubbleX = previewX - bubbleWidth / 2.0f;
        if (bubbleX < padding) {
            bubbleX = padding;
        }
        if (bubbleX + bubbleWidth > width - padding) {
            bubbleX = width - padding - bubbleWidth;
        }
        RectF bubbleRect(bubbleX, progressRect.Y - bubbleHeight - 4.0f,
                         bubbleWidth, bubbleHeight);
        D2DFillRoundedRect(
            g_d2dRenderTarget.get(), bubbleRect, 6.0f,
            D2D1::ColorF(10 / 255.0f, 14 / 255.0f, 22 / 255.0f,
                         previewAlpha * 0.80f));
        D2DDrawRoundedRect(
            g_d2dRenderTarget.get(), bubbleRect, 6.0f,
            D2D1::ColorF(1.0f, 1.0f, 1.0f, previewAlpha * 0.18f),
            1.0f);
        D2DDrawTextLine(g_d2dRenderTarget.get(), previewText.c_str(),
                        bubbleRect, RGB(255, 255, 255), previewAlpha, 9.5f,
                        DWRITE_FONT_WEIGHT_MEDIUM,
                        DWRITE_TEXT_ALIGNMENT_CENTER);
    };

    g_d2dRenderTarget->BeginDraw();

    if (idleMode) {
        drawBackground();
        drawIdleApps();

        HRESULT idleHr = g_d2dRenderTarget->EndDraw();
        if (FAILED(idleHr)) {
            Wh_Log(L"PaintWidgetD2D: EndDraw idle failed hr=0x%08X",
                   static_cast<unsigned>(idleHr));
        }
        if (idleHr == D2DERR_RECREATE_TARGET) {
            g_d2dRenderTarget = nullptr;
            g_d2dThumbnailBitmap = nullptr;
            ResetIdleD2DIcons();
            return false;
        }

        return SUCCEEDED(idleHr);
    }

    if (IsPlayOnlyModeActive()) {
        drawPlayOnly();

        HRESULT playOnlyHr = g_d2dRenderTarget->EndDraw();
        if (FAILED(playOnlyHr)) {
            Wh_Log(L"PaintWidgetD2D: EndDraw play-only failed hr=0x%08X",
                   static_cast<unsigned>(playOnlyHr));
        }
        if (playOnlyHr == D2DERR_RECREATE_TARGET) {
            g_d2dRenderTarget = nullptr;
            g_d2dThumbnailBitmap = nullptr;
            ResetIdleD2DIcons();
            return false;
        }

        return SUCCEEDED(playOnlyHr);
    }

    drawBase();

    drawProgress();
    drawControls();
    drawProgressPreview();

    HRESULT hr = g_d2dRenderTarget->EndDraw();
    if (FAILED(hr)) {
        Wh_Log(L"PaintWidgetD2D: EndDraw failed hr=0x%08X",
               static_cast<unsigned>(hr));
    }
    if (hr == D2DERR_RECREATE_TARGET) {
        g_d2dRenderTarget = nullptr;
        g_d2dThumbnailBitmap = nullptr;
        ResetIdleD2DIcons();
        return false;
    }

    return SUCCEEDED(hr);
}

void RefreshWidget() {
    if (!g_widgetWnd) {
        return;
    }

    InvalidateRect(g_widgetWnd, nullptr, FALSE);
}

bool PtInRectSafe(RECT rect, POINT pt) {
    return pt.x >= rect.left && pt.x < rect.right && pt.y >= rect.top &&
           pt.y < rect.bottom;
}

void SeekFromPoint(POINT pt) {
    if (!g_mediaSession || !g_mediaState.canSeek ||
        g_mediaState.endTime <= g_mediaState.startTime) {
        return;
    }

    RECT clientRect{};
    GetClientRect(g_widgetWnd, &clientRect);
    RECT progressRect = GetProgressRect(clientRect);

    int width = progressRect.right - progressRect.left;
    if (width <= 0) {
        return;
    }

    int x = ClampInt(pt.x, progressRect.left, progressRect.right);
    double ratio = static_cast<double>(x - progressRect.left) / width;
    auto duration = g_mediaState.endTime - g_mediaState.startTime;
    auto targetTicks =
        static_cast<TimeSpan::rep>(duration.count() * ratio);
    TimeSpan target{targetTicks};

    try {
        g_mediaSession.TryChangePlaybackPositionAsync(target.count()).get();
        PollMedia();
    } catch (hresult_error const& e) {
        Wh_Log(L"Seek error: 0x%08X %s", static_cast<uint32_t>(e.code()),
               e.message().c_str());
    }
}

void ActivateSourceApp() {
    if (!g_mediaState.hasSession || g_mediaState.sourceAppId.empty()) {
        return;
    }

    std::wstring sourceAppId = g_mediaState.sourceAppId.c_str();

    if (ActivateAppUserModelId(sourceAppId)) {
        return;
    }

    std::wstring appsFolderPath = L"shell:AppsFolder\\" + sourceAppId;
    if (ShellExecutePath(appsFolderPath)) {
        return;
    }

    if (LaunchIdleAppForSource(sourceAppId)) {
        return;
    }

    Wh_Log(L"No activation fallback found for %s", sourceAppId.c_str());
}

void TogglePlayPause() {
    if (!g_mediaSession || (!g_mediaState.canPlayPause && !g_mediaState.canPlay)) {
        return;
    }

    try {
        if (g_mediaState.playOnlySession && g_mediaState.canPlay) {
            g_mediaSession.TryPlayAsync().get();
        } else {
            g_mediaSession.TryTogglePlayPauseAsync().get();
        }
        PollMedia();
    } catch (hresult_error const& e) {
        Wh_Log(L"Play/pause error: 0x%08X %s",
               static_cast<uint32_t>(e.code()), e.message().c_str());
    }
}

void PreviousTrack() {
    if (!g_mediaSession || !g_mediaState.canPrevious) {
        return;
    }

    try {
        g_mediaSession.TrySkipPreviousAsync().get();
        PollMedia();
    } catch (hresult_error const& e) {
        Wh_Log(L"Previous error: 0x%08X %s",
               static_cast<uint32_t>(e.code()), e.message().c_str());
    }
}

void NextTrack() {
    if (!g_mediaSession || !g_mediaState.canNext) {
        return;
    }

    try {
        g_mediaSession.TrySkipNextAsync().get();
        PollMedia();
    } catch (hresult_error const& e) {
        Wh_Log(L"Next error: 0x%08X %s", static_cast<uint32_t>(e.code()),
               e.message().c_str());
    }
}

LRESULT CALLBACK WidgetWndProc(HWND hwnd,
                               UINT msg,
                               WPARAM wParam,
                               LPARAM lParam) {
    switch (msg) {
        case WM_CREATE:
            SetTimer(hwnd, kTimerPollMedia, 1000, nullptr);
            SetTimer(hwnd, kTimerRepaint, 33, nullptr);
            SetTimer(hwnd, kTimerLayout, 250, nullptr);
            return 0;

        case WM_TIMER:
            if (wParam == kTimerPollMedia) {
                SyncThemeFromWindows(hwnd);
                PollMedia();
            } else if (wParam == kTimerRepaint) {
                TickAnimations();
                RefreshWidget();
            } else if (wParam == kTimerLayout) {
                RepositionWidget();
            }
            return 0;

        case WM_ERASEBKGND:
            return 1;

        case WM_SETTINGCHANGE:
            SyncThemeFromWindows(hwnd, true);
            return 0;

        case WM_SETCURSOR:
            if (LOWORD(lParam) == HTCLIENT) {
                POINT pt{};
                GetCursorPos(&pt);
                ScreenToClient(hwnd, &pt);
                RECT clientRect{};
                GetClientRect(hwnd, &clientRect);
                HotButton hotButton = g_mediaState.hasSession
                                          ? HitTestButton(clientRect, pt)
                                          : HotButton::None;
                bool coverHover =
                    g_mediaState.hasSession && !g_mediaState.playOnlySession &&
                    PtInRectSafe(GetCoverRect(clientRect), pt);
                bool idleHover = !g_mediaState.hasSession &&
                                 HitTestIdleApp(clientRect, pt) >= 0;

                if (coverHover || idleHover || g_progressHover ||
                    hotButton != HotButton::None) {
                    SetCursor(LoadCursor(nullptr, IDC_HAND));
                } else {
                    SetCursor(LoadCursor(nullptr, IDC_ARROW));
                }
                return TRUE;
            }
            break;

        case WM_MOUSEMOVE: {
            RECT clientRect{};
            GetClientRect(hwnd, &clientRect);
            POINT pt{GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
            HotButton hotButton = g_mediaState.hasSession
                                      ? HitTestButton(clientRect, pt)
                                      : HotButton::None;
            bool progressHover =
                g_mediaState.hasSession && !g_mediaState.playOnlySession &&
                PtInRectSafe(GetProgressHitRect(clientRect), pt);
            bool coverHover =
                g_mediaState.hasSession && !g_mediaState.playOnlySession &&
                PtInRectSafe(GetCoverRect(clientRect), pt);
            bool idleHover = !g_mediaState.hasSession &&
                             HitTestIdleApp(clientRect, pt) >= 0;
            bool needsRefresh = false;

            if (hotButton != g_hotButton) {
                g_hotButton = hotButton;
                needsRefresh = true;
            }

            if (progressHover != g_progressHover) {
                g_progressHover = progressHover;
                if (!progressHover) {
                    g_progressHoverX = 0;
                }
                needsRefresh = true;
            }

            if (progressHover && g_progressHoverX != pt.x) {
                g_progressHoverX = pt.x;
                needsRefresh = true;
            }

            if (coverHover || idleHover || progressHover ||
                hotButton != HotButton::None) {
                SetCursor(LoadCursor(nullptr, IDC_HAND));
            } else {
                SetCursor(LoadCursor(nullptr, IDC_ARROW));
            }

            if (!g_hover) {
                g_hover = true;
                needsRefresh = true;
            }

            if (needsRefresh) {
                RefreshWidget();
            }

            TRACKMOUSEEVENT trackMouseEvent{};
            trackMouseEvent.cbSize = sizeof(TRACKMOUSEEVENT);
            trackMouseEvent.dwFlags = TME_LEAVE;
            trackMouseEvent.hwndTrack = hwnd;
            TrackMouseEvent(&trackMouseEvent);
            return 0;
        }

        case WM_MOUSELEAVE:
            g_hover = false;
            g_hotButton = HotButton::None;
            g_prevHoverProgress = 0.0f;
            g_playHoverProgress = 0.0f;
            g_nextHoverProgress = 0.0f;
            g_progressHover = false;
            g_progressHoverProgress = 0.0f;
            g_progressHoverX = 0;
            RefreshWidget();
            return 0;

        case WM_LBUTTONDOWN: {
            if (GetTickCount() < g_ignoreInputUntilTick) {
                return 0;
            }

            POINT pt{GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
            RECT clientRect{};
            GetClientRect(hwnd, &clientRect);

            int idleApp = !g_mediaState.hasSession
                              ? HitTestIdleApp(clientRect, pt)
                              : -1;
            if (IsPlayOnlyModeActive() &&
                PtInRectSafe(GetPlayOnlyButtonRect(clientRect), pt)) {
                TogglePlayPause();
            } else if (g_mediaState.hasSession &&
                !g_mediaState.playOnlySession &&
                PtInRectSafe(GetProgressHitRect(clientRect), pt)) {
                SeekFromPoint(pt);
            } else if (idleApp >= 0) {
                LaunchIdleApp(static_cast<size_t>(idleApp));
            } else if (g_mediaState.hasSession &&
                !g_mediaState.playOnlySession &&
                PtInRectSafe(GetCoverRect(clientRect), pt)) {
                ActivateSourceApp();
            } else if (PtInRectSafe(GetPrevButtonRect(clientRect), pt)) {
                PreviousTrack();
            } else if (PtInRectSafe(GetPlayButtonRect(clientRect), pt)) {
                TogglePlayPause();
            } else if (PtInRectSafe(GetNextButtonRect(clientRect), pt)) {
                NextTrack();
            }

            return 0;
        }

        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);

            RECT clientRect{};
            GetClientRect(hwnd, &clientRect);
            int width = clientRect.right - clientRect.left;
            int height = clientRect.bottom - clientRect.top;

            if (g_settings.useSystemBlur) {
                PaintWidgetD2D(hwnd, hdc, true);
                EndPaint(hwnd, &ps);
                return 0;
            }

            HDC memDc = CreateCompatibleDC(hdc);
            HBITMAP bitmap = CreateCompatibleBitmap(hdc, width, height);
            HGDIOBJ oldBitmap = SelectObject(memDc, bitmap);

            if (PaintWidgetD2D(hwnd, memDc)) {
                BitBlt(hdc, 0, 0, width, height, memDc, 0, 0, SRCCOPY);
            }

            SelectObject(memDc, oldBitmap);
            DeleteObject(bitmap);
            DeleteDC(memDc);

            EndPaint(hwnd, &ps);
            return 0;
        }

        case WM_CLOSE:
            DestroyWindow(hwnd);
            return 0;

        case WM_DESTROY:
            KillTimer(hwnd, kTimerPollMedia);
            KillTimer(hwnd, kTimerRepaint);
            KillTimer(hwnd, kTimerLayout);
            g_widgetWnd = nullptr;
            PostQuitMessage(0);
            return 0;
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}

void WidgetThread() {
    init_apartment(apartment_type::multi_threaded);

    HWND taskbarWnd = nullptr;
    while (!g_stopThread.load()) {
        taskbarWnd = FindCurrentProcessTaskbarWnd();
        if (taskbarWnd) {
            break;
        }

        Sleep(500);
    }

    if (g_stopThread.load()) {
        return;
    }

    WNDCLASSW wndClass{};
    wndClass.lpfnWndProc = WidgetWndProc;
    wndClass.hInstance = GetModuleHandle(nullptr);
    wndClass.lpszClassName = L"WindhawkTaskbarMediaWidget";
    wndClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wndClass.hbrBackground = nullptr;
    RegisterClassW(&wndClass);

    g_widgetWnd = CreateWindowExW(
        WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE | WS_EX_TOPMOST,
        wndClass.lpszClassName, L"Taskbar Media Widget", WS_POPUP, 0, 0,
        g_settings.widgetWidth, g_settings.widgetHeight, nullptr, nullptr,
        wndClass.hInstance, nullptr);

    if (!g_widgetWnd) {
        Wh_Log(L"CreateWindowEx failed: %u", GetLastError());
        return;
    }

    g_ignoreInputUntilTick = GetTickCount() + 1000;
    ApplySystemBackdrop(g_widgetWnd);
    PollMedia();
    RepositionWidget();
    RefreshWidget();

    MSG msg;
    while (!g_stopThread.load() && GetMessage(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    if (g_widgetWnd) {
        DestroyWindow(g_widgetWnd);
    }
}

BOOL Wh_ModInit() {
    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    Gdiplus::Status status =
        Gdiplus::GdiplusStartup(&g_gdiplusToken, &gdiplusStartupInput, nullptr);
    if (status != Gdiplus::Ok) {
        Wh_Log(L"GdiplusStartup failed: %d", status);
        return FALSE;
    }

    LoadSettings();
    ScanIdleApps();
    g_stopThread = false;
    g_mediaPollCompleted = false;
    g_widgetThread = std::thread(WidgetThread);

    return TRUE;
}

void Wh_ModUninit() {
    g_stopThread = true;
    if (g_widgetWnd) {
        PostMessage(g_widgetWnd, WM_CLOSE, 0, 0);
    }

    if (g_widgetThread.joinable()) {
        g_widgetThread.join();
    }

    g_d2dThumbnailBitmap = nullptr;
    g_d2dRenderTarget = nullptr;
    g_dwriteFactory = nullptr;
    g_d2dFactory = nullptr;
    g_thumbnailBitmap.reset();
    g_idleApps.clear();

    if (g_gdiplusToken) {
        Gdiplus::GdiplusShutdown(g_gdiplusToken);
        g_gdiplusToken = 0;
    }
}

void Wh_ModSettingsChanged() {
    LoadSettings();
    ScanIdleApps();
    if (g_widgetWnd) {
        ApplySystemBackdrop(g_widgetWnd);
    }
    RepositionWidget();
    if (g_widgetWnd) {
        RefreshWidget();
    }
}
