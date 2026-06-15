// ==WindhawkMod==
// @id              mods/winglass-widget.wh.cpp
// @name            mods/winglass-widget.wh.cpp
// @description     Achieve that Premium Glass morph widgets and elevate your Windows Display to look Clean, Professional, High-end and Futuristic.
// @version         3.4
// @author          Jona like It, Code It
// @github          https://github.com/SeniorMajor7
// @include         explorer.exe
// @compilerOptions -lcomctl32 -lgdi32 -luxtheme -ldwmapi -lgdiplus -lole32 -luuid -lshell32 -lshlwapi
// @license         MIT
// @architecture    x86-64
// ==/WindhawkMod==
// ==WindhawkModReadme==
// JonaOS Liquid Glass Widgets
// "Achieve that Premium Glass morph widgets and elevate your Windows Display to look Clean, Professional, High-end and Futuristic".
//"<img width="478" height="376" alt="Image" src="https://github.com/user-attachments/assets/afa6c355-4a19-40d8-b055-a3a5a7d7b2a5" /> "Light mode"
//<img width="479" height="370" alt="Image" src="https://github.com/user-attachments/assets/f0fdf3c9-e95d-4701-92ee-165c981f4eda" /> "Dark mode"
//<img width="475" height="362" alt="Image" src="https://github.com/user-attachments/assets/a90c3cf5-ee5a-4d24-90ab-1e53ffa7ef61" /> "Acrylic/Translucent"
//<img width="475" height="369" alt="Image" src="https://github.com/user-attachments/assets/497b525c-ea6f-4d2e-b113-e97c1bd3c5d8" /> "Mica"
//<img width="484" height="363" alt="Image" src="https://github.com/user-attachments/assets/358af692-960e-4269-91a5-bf1acc3a8e0b" /> "Tabbed Mica"
//<img width="476" height="371" alt="Image" src="https://github.com/user-attachments/assets/38353d6d-321c-4331-b4c5-224b68cf18f7" /> "No DWM Background"
//" Change the Appearance mode. Corner roundness. Switch off any widget"
// "It is better to leave all settings in default since they are designed perfectly except for the appearance mode which the user can change according to their tastes and preferance
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- Side: right
  $name: Automatic desktop side
  $description: Used when a widget X position is set to -1.
  $options:
  - left: Left side
  - right: Right side

- AppearanceMode: Acrylic
  $name: Default appearance mode
  $options:
  - light: Light mode
  - dark: Dark mode
  - acrylic: Acrylic / translucent
  - mica: Mica
  - tabbed: Tabbed Mica
  - none: No DWM backdrop

- LayoutTop: 32
  $name: Automatic layout top
- LayoutGap: 10
  $name: Automatic layout gap
- WidgetOpacity: 255
  $name: Default widget opacity
- WidgetRoundness: 26
  $name: Default widget roundness
- FontName: "Calibri"
  $name: Default font

- CalendarEnabled: true
  $name: Calendar enabled
- CalendarX: -1
  $name: Calendar X position, -1 automatic
- CalendarY: -1
  $name: Calendar Y position, -1 automatic
- CalendarWidth: 360
  $name: Calendar width
- CalendarHeight: 170
  $name: Calendar height
- CalendarAppearanceMode: default
  $name: Calendar appearance mode
  $options:
  - default: Use default
  - light: Light
  - dark: Dark
  - acrylic: Acrylic / translucent
  - mica: Mica
  - tabbed: Tabbed Mica
  - none: No DWM backdrop
- CalendarWidgetColor: ""
  $name: Calendar widget color RGB, blank uses theme
- CalendarLabelColor: ""
  $name: Calendar label color RGB, blank uses theme
- CalendarOpacity: 0
  $name: Calendar opacity, 0 uses default
- CalendarRoundness: 0
  $name: Calendar roundness, 0 uses default

- NowPlayingEnabled: true
  $name: Now Playing enabled
- NowPlayingX: -1
  $name: Now Playing X position, -1 automatic
- NowPlayingY: -1
  $name: Now Playing Y position, -1 automatic
- NowPlayingWidth: 170
  $name: Now Playing width
- NowPlayingHeight: 170
  $name: Now Playing height
- NowPlayingAppearanceMode: default
  $name: Now Playing appearance mode
  $options:
  - default: Use default
  - light: Light
  - dark: Dark
  - acrylic: Acrylic / translucent
  - mica: Mica
  - tabbed: Tabbed Mica
  - none: No DWM backdrop
- NowPlayingWidgetColor: ""
  $name: Now Playing widget color RGB, blank uses theme
- NowPlayingLabelColor: ""
  $name: Now Playing label color RGB, blank uses theme
- NowPlayingOpacity: 0
  $name: Now Playing opacity, 0 uses default
- NowPlayingRoundness: 0
  $name: Now Playing roundness, 0 uses default

- ClockEnabled: true
  $name: Clock enabled
- ClockX: -1
  $name: Clock X position, -1 automatic
- ClockY: -1
  $name: Clock Y position, -1 automatic
- ClockWidth: 170
  $name: Clock width
- ClockHeight: 170
  $name: Clock height
- ClockAppearanceMode: default
  $name: Clock appearance mode
  $options:
  - default: Use default
  - light: Light
  - dark: Dark
  - acrylic: Acrylic / translucent
  - mica: Mica
  - tabbed: Tabbed Mica
  - none: No DWM backdrop
- ClockWidgetColor: ""
  $name: Clock widget color RGB, blank uses theme
- ClockLabelColor: ""
  $name: Clock label color RGB, blank uses theme
- ClockOpacity: 0
  $name: Clock opacity, 0 uses default
- ClockRoundness: 0
  $name: Clock roundness, 0 uses default

- VolumeEnabled: true
  $name: Volume enabled
- VolumeX: -1
  $name: Volume X position, -1 automatic
- VolumeY: -1
  $name: Volume Y position, -1 automatic
- VolumeWidth: 360
  $name: Volume width
- VolumeHeight: 35
  $name: Volume height
- VolumeAppearanceMode: default
  $name: Volume appearance mode
  $options:
  - default: Use default
  - light: Light
  - dark: Dark
  - acrylic: Acrylic / translucent
  - mica: Mica
  - tabbed: Tabbed Mica
  - none: No DWM backdrop
- VolumeWidgetColor: ""
  $name: Volume widget color RGB, blank uses theme
- VolumeLabelColor: ""
  $name: Volume label color RGB, blank uses theme
- VolumeOpacity: 0
  $name: Volume opacity, 0 uses default
- VolumeRoundness: 0
  $name: Volume roundness, 0 uses default

- BatteryEnabled: true
  $name: Battery enabled
- BatteryX: -1
  $name: Battery X position, -1 automatic
- BatteryY: -1
  $name: Battery Y position, -1 automatic
- BatteryWidth: 360
  $name: Battery width
- BatteryHeight: 35
  $name: Battery height
- BatteryAppearanceMode: default
  $name: Battery appearance mode
  $options:
  - default: Use Default
  - light: Light
  - dark: Dark
  - acrylic: Acrylic / translucent
  - mica: Mica
  - tabbed: Tabbed Mica
  - none: No DWM backdrop
- BatteryWidgetColor: ""
  $name: Battery widget color RGB, blank uses theme
- BatteryLabelColor: ""
  $name: Battery label color RGB, blank uses theme
- BatteryOpacity: 0
  $name: Battery opacity, 0 uses default
- BatteryRoundness: 0
  $name: Battery roundness, 0 uses default

- WindowsSearchEnabled: true
  $name: Windows Search enabled
- WindowsSearchX: -1
  $name: Windows Search X position, -1 automatic
- WindowsSearchY: -1
  $name: Windows Search Y position, -1 automatic
- WindowsSearchWidth: 170
  $name: Windows Search width
- WindowsSearchHeight: 35
  $name: Windows Search height
- WindowsSearchAppearanceMode: default
  $name: Windows Search appearance mode
  $options:
  - default: Use Default
  - light: Light
  - dark: Dark
  - acrylic: Acrylic / translucent
  - mica: Mica
  - tabbed: Tabbed Mica
  - none: No DWM backdrop
- WindowsSearchWidgetColor: ""
  $name: Windows Search widget color RGB, blank uses theme
- WindowsSearchLabelColor: ""
  $name: Windows Search label color RGB, blank uses theme
- WindowsSearchOpacity: 0
  $name: Windows Search opacity, 0 uses default
- WindowsSearchRoundness: 0
  $name: Windows Search roundness, 0 uses default

- PersonalizationEnabled: true
  $name: Personalization enabled
- PersonalizationX: -1
  $name: Personalization X position, -1 automatic
- PersonalizationY: -1
  $name: Personalization Y position, -1 automatic
- PersonalizationWidth: 170
  $name: Personalization width
- PersonalizationHeight: 35
  $name: Personalization height
- PersonalizationAppearanceMode: default
  $name: Personalization appearance mode
  $options:
  - default: Use Default
  - light: Light
  - dark: Dark
  - acrylic: Acrylic / translucent
  - mica: Mica
  - tabbed: Tabbed Mica
  - none: No DWM backdrop
- PersonalizationWidgetColor: ""
  $name: Personalization widget color RGB, blank uses theme
- PersonalizationLabelColor: ""
  $name: Personalization label color RGB, blank uses theme
- PersonalizationOpacity: 0
  $name: Personalization opacity, 0 uses default
- PersonalizationRoundness: 0
  $name: Personalization roundness, 0 uses default

- WifiEnabled: true
  $name: WiFi enabled
- WifiX: -1
  $name: WiFi X position, -1 automatic
- WifiY: -1
  $name: WiFi Y position, -1 automatic
- WifiWidth: 70
  $name: WiFi width
- WifiHeight: 70
  $name: WiFi height
- WifiAppearanceMode: default
  $name: WiFi appearance mode
  $options:
  - default: Use Default
  - light: Light
  - dark: Dark
  - acrylic: Acrylic / translucent
  - mica: Mica
  - tabbed: Tabbed Mica
  - none: No DWM backdrop
- WifiWidgetColor: ""
  $name: WiFi widget color RGB, blank uses theme
- WifiLabelColor: ""
  $name: WiFi label color RGB, blank uses theme
- WifiOpacity: 0
  $name: WiFi opacity, 0 uses default
- WifiRoundness: 0
  $name: WiFi roundness, 0 uses default

- HotspotEnabled: true
  $name: Hotspot enabled
- HotspotX: -1
  $name: Hotspot X position, -1 automatic
- HotspotY: -1
  $name: Hotspot Y position, -1 automatic
- HotspotWidth: 70
  $name: Hotspot width
- HotspotHeight: 70
  $name: Hotspot height
- HotspotAppearanceMode: default
  $name: Hotspot appearance mode
  $options:
  - default: Use Default
  - light: Light
  - dark: Dark
  - acrylic: Acrylic / translucent
  - mica: Mica
  - tabbed: Tabbed Mica
  - none: No DWM backdrop
- HotspotWidgetColor: ""
  $name: Hotspot widget color RGB, blank uses theme
- HotspotLabelColor: ""
  $name: Hotspot label color RGB, blank uses theme
- HotspotOpacity: 0
  $name: Hotspot opacity, 0 uses default
- HotspotRoundness: 0
  $name: Hotspot roundness, 0 uses default

- BluetoothEnabled: true
  $name: Bluetooth enabled
- BluetoothX: -1
  $name: Bluetooth X position, -1 automatic
- BluetoothY: -1
  $name: Bluetooth Y position, -1 automatic
- BluetoothWidth: 70
  $name: Bluetooth width
- BluetoothHeight: 70
  $name: Bluetooth height
- BluetoothAppearanceMode: default
  $name: Bluetooth appearance mode
  $options:
  - default: Use Default
  - light: Light
  - dark: Dark
  - acrylic: Acrylic / translucent
  - mica: Mica
  - tabbed: Tabbed Mica
  - none: No DWM backdrop
- BluetoothWidgetColor: ""
  $name: Bluetooth widget color RGB, blank uses theme
- BluetoothLabelColor: ""
  $name: Bluetooth label color RGB, blank uses theme
- BluetoothOpacity: 0
  $name: Bluetooth opacity, 0 uses default
- BluetoothRoundness: 0
  $name: Bluetooth roundness, 0 uses default

- AccessibilityEnabled: true
  $name: Accessibility enabled
- AccessibilityX: -1
  $name: Accessibility X position, -1 automatic
- AccessibilityY: -1
  $name: Accessibility Y position, -1 automatic
- AccessibilityWidth: 70
  $name: Accessibility width
- AccessibilityHeight: 70
  $name: Accessibility height
- AccessibilityAppearanceMode: default
  $name: Accessibility appearance mode
  $options:
  - default: Use Default
  - light: Light
  - dark: Dark
  - acrylic: Acrylic / translucent
  - mica: Mica
  - tabbed: Tabbed Mica
  - none: No DWM backdrop
- AccessibilityWidgetColor: ""
  $name: Accessibility widget color RGB, blank uses theme
- AccessibilityLabelColor: ""
  $name: Accessibility label color RGB, blank uses theme
- AccessibilityOpacity: 0
  $name: Accessibility opacity, 0 uses default
- AccessibilityRoundness: 0
  $name: Accessibility roundness, 0 uses default
*/
// ==/WindhawkModSettings==

#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <wbemidl.h>
#include <comdef.h>
#include <windows.h>
#include <windowsx.h>
#include <dwmapi.h>
#include <gdiplus.h>
#include <uxtheme.h>
#include <commctrl.h>
#include <shellapi.h>
#include <shlobj.h>
#include <shobjidl.h>
#include <shlwapi.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h>
#include <math.h>
#include <stdio.h>
#include <time.h>
#include <algorithm>
#include <memory>

#pragma comment(lib, "shlwapi.lib")

extern "C" {
    const GUID CLSID_MMDeviceEnumerator = {0xbcde0395, 0xe52f, 0x467c, {0x8e, 0x3d, 0xc4, 0x57, 0x92, 0x91, 0x69, 0x2e}};
    const GUID IID_IMMDeviceEnumerator  = {0xa95664d2, 0x9614, 0x4f35, {0xa7, 0x46, 0xde, 0x8d, 0xb6, 0x36, 0x17, 0xe6}};
    const GUID IID_IAudioEndpointVolume = {0x5cdf2c82, 0x841e, 0x4546, {0x97, 0x22, 0x0c, 0xf7, 0x40, 0x78, 0x22, 0x9a}};
}
using namespace Gdiplus;

#ifndef DWMWA_WINDOW_CORNER_PREFERENCE
#define DWMWA_WINDOW_CORNER_PREFERENCE 33
#endif

#ifndef DWMWA_SYSTEMBACKDROP_TYPE
#define DWMWA_SYSTEMBACKDROP_TYPE 38
#endif

#ifndef DWMWCP_ROUND
#define DWMWCP_ROUND 2
#endif

#ifndef DWMSBT_NONE
#define DWMSBT_AUTO 0
#define DWMSBT_NONE 1
#define DWMSBT_MAINWINDOW 2
#define DWMSBT_TRANSIENTWINDOW 3
#define DWMSBT_TABBEDWINDOW 4
#endif

#ifndef DWMSBT_TRANSLUCENTBACKGROUND
#define DWMSBT_TRANSLUCENTBACKGROUND DWMSBT_TRANSIENTWINDOW
#endif

#define CLASS_NAME L"JonaOSLiquidGlassWidgetWindow"
#define TIMER_ID 1001
#define AUTO_MARGIN 32
#define PI_D 3.14159265358979323846

struct ACCENT_POLICY {
    int AccentState;
    int AccentFlags;
    int GradientColor;
    int AnimationId;
};

struct WINDOWCOMPOSITIONATTRIBDATA {
    int Attrib;
    PVOID pvData;
    SIZE_T cbData;
};

enum {
    WCA_ACCENT_POLICY = 19,
    ACCENT_ENABLE_ACRYLICBLURBEHIND = 4
};

typedef BOOL(WINAPI* SetWindowCompositionAttribute_t)(HWND, WINDOWCOMPOSITIONATTRIBDATA*);

enum class WidgetType {
    Calendar,
    NowPlaying,
    Clock,
    Volume,
    Battery,
    WindowsSearch,
    Personalization,
    Wifi,
    Hotspot,
    Bluetooth,
    Accessibility,
};

enum class AppearanceMode {
    Light,
    Dark,
    Acrylic,
    Mica,
    Tabbed,
    None,
};

enum class HitZone {
    None,
    Slider,
    MediaPrev,
    MediaPlayPause,
    MediaNext,
    UtilityButton,
};

struct WidgetConfig {
    bool enabled;
    int x;
    int y;
    int width;
    int height;
    int opacity;
    int cornerRadius;
    AppearanceMode appearance;
    COLORREF widgetColor;
    COLORREF labelColor;
    WCHAR fontName[64];
};

struct WidgetWindow {
    WidgetType type;
    PCWSTR prefix;
    PCWSTR title;
    int defaultOffsetX;
    int defaultOffsetY;
    int defaultWidth;
    int defaultHeight;
    HWND hwnd;
    WidgetConfig cfg;
    // slider state
    RECT sliderRect;
    RECT sliderThumbRect;
    int sliderValue;
    bool sliderTracking;
};

static HINSTANCE g_hinst = nullptr;
static HWND g_parent = nullptr;
static ULONG_PTR g_gdiplusToken = 0;
static bool g_comInitialized = false;
static WCHAR g_side[16] = L"right";
static WCHAR g_defaultFont[64] = L"Calibri";
static AppearanceMode g_defaultAppearance = AppearanceMode::Light;
static int g_layoutTop = 32;
static int g_layoutGap = 10;
static int g_defaultOpacity = 255;
static int g_defaultRoundness = 26;

static const int PILE_WIDTH = 380;
static const int PILE_HEIGHT = 821;

static HBITMAP g_nowPlayingThumb = nullptr;
static WCHAR g_nowPlayingFilePath[MAX_PATH] = {0};

static WidgetWindow g_widgets[] = {
    {WidgetType::Calendar, L"Calendar", L"JonaOS Calendar Widget", 0, 0, 360, 170, nullptr, {},  {}, 0, false},
    {WidgetType::NowPlaying, L"NowPlaying", L"JonaOS Now Playing Widget", 190, 200, 170, 170, nullptr, {}, {}, 0, false},
    {WidgetType::Clock, L"Clock", L"JonaOS Clock Widget", 0, 200, 170, 170, nullptr, {}, {}, 0, false},
    {WidgetType::Volume, L"Volume", L"JonaOS Volume Widget", 0, 400, 360, 35, nullptr, {}, {}, 0, false},
    {WidgetType::Battery, L"Battery", L"JonaOS Battery Widget", 0, 465, 360, 35, nullptr, {}, {}, 0, false},
    {WidgetType::WindowsSearch, L"WindowsSearch", L"JonaOS Windows Search Widget", 0, 530, 170, 35, nullptr, {}, {}, 0, false},
    {WidgetType::Personalization, L"Personalization", L"JonaOS Personalization Widget", 200, 530, 170, 35, nullptr, {}, {}, 0, false},
    {WidgetType::Wifi, L"Wifi", L"JonaOS WiFi Widget", 0, 595, 70, 70, nullptr, {}, {}, 0, false},
    {WidgetType::Hotspot, L"Hotspot", L"JonaOS Hotspot Widget", 96, 595, 70, 70, nullptr, {}, {}, 0, false},
    {WidgetType::Bluetooth, L"Bluetooth", L"JonaOS Bluetooth Widget", 192, 595, 70, 70, nullptr, {}, {}, 0, false},
    {WidgetType::Accessibility, L"Accessibility", L"JonaOS Accessibility Widget", 288, 595, 70, 70, nullptr, {}, {}, 0, false},
};

static int ClampInt(int value, int minValue, int maxValue) {
    return std::max(minValue, std::min(value, maxValue));
}

static bool IsLightTheme(AppearanceMode mode) {
    return mode == AppearanceMode::Light;
}

static COLORREF ParseRgb(PCWSTR text, COLORREF fallback) {
    int r = 0;
    int g = 0;
    int b = 0;

    if (text && *text && swscanf_s(text, L"%d,%d,%d", &r, &g, &b) == 3) {
        return RGB(ClampInt(r, 0, 255), ClampInt(g, 0, 255), ClampInt(b, 0, 255));
    }

    return fallback;
}

static AppearanceMode ParseAppearance(PCWSTR text, AppearanceMode fallback) {
    if (!text || !*text || _wcsicmp(text, L"default") == 0) {
        return fallback;
    }

    if (_wcsicmp(text, L"light") == 0) {
        return AppearanceMode::Light;
    }

    if (_wcsicmp(text, L"dark") == 0) {
        return AppearanceMode::Dark;
    }

    if (_wcsicmp(text, L"mica") == 0) {
        return AppearanceMode::Mica;
    }

    if (_wcsicmp(text, L"tabbed") == 0) {
        return AppearanceMode::Tabbed;
    }

    if (_wcsicmp(text, L"none") == 0) {
        return AppearanceMode::None;
    }

    return AppearanceMode::Acrylic;
}

static void LoadStringSetting(PCWSTR name, PWSTR buffer, size_t cchBuffer, PCWSTR fallback) {
    PCWSTR value = Wh_GetStringSetting(name);

    if (value && *value) {
        wcsncpy_s(buffer, cchBuffer, value, _TRUNCATE);
    } else {
        wcsncpy_s(buffer, cchBuffer, fallback, _TRUNCATE);
    }

    if (value) {
        Wh_FreeStringSetting(value);
    }
}

static COLORREF ThemeWidgetColor(AppearanceMode mode) {
    return IsLightTheme(mode) ? RGB(240, 240, 240) : RGB(30, 30, 30);
}

static COLORREF ThemeLabelColor(AppearanceMode mode) {
    return IsLightTheme(mode) ? RGB(20, 20, 20) : RGB(255, 255, 255);
}

static COLORREF BlendColor(COLORREF a, COLORREF b, int percentB) {
    int percentA = 100 - percentB;
    return RGB(
        (GetRValue(a) * percentA + GetRValue(b) * percentB) / 100,
        (GetGValue(a) * percentA + GetGValue(b) * percentB) / 100,
        (GetBValue(a) * percentA + GetBValue(b) * percentB) / 100);
}

static int GetDwmBackdropValue(AppearanceMode appearance) {
    switch (appearance) {
        case AppearanceMode::Mica:
            return DWMSBT_MAINWINDOW;
        case AppearanceMode::Tabbed:
            return DWMSBT_TABBEDWINDOW;
        case AppearanceMode::Acrylic:
            return DWMSBT_TRANSLUCENTBACKGROUND;
        case AppearanceMode::Light:
        case AppearanceMode::Dark:
        case AppearanceMode::None:
        default:
            return DWMSBT_NONE;
    }
}

static void OpenSettingsUri(PCWSTR uri)
{
    if (!uri || !*uri) {
        return;
    }

    HINSTANCE result = ShellExecuteW(nullptr, L"open", uri, nullptr, nullptr, SW_SHOWNORMAL);
    if ((INT_PTR)result <= 32) {
        Wh_Log(L"OpenSettingsUri: ShellExecuteW failed for URI");
    }
}

static void ApplyPerWidgetDefaultLabelColors(WidgetWindow& widget) {
    bool isLight = (widget.cfg.appearance == AppearanceMode::Light);
    if (isLight &&
        (widget.type == WidgetType::Calendar ||
         widget.type == WidgetType::NowPlaying ||
         widget.type == WidgetType::Clock ||
         widget.type == WidgetType::Volume ||
         widget.type == WidgetType::Battery ||
         widget.type == WidgetType::WindowsSearch ||
         widget.type == WidgetType::Personalization ||
         widget.type == WidgetType::Wifi ||
         widget.type == WidgetType::Hotspot ||
         widget.type == WidgetType::Bluetooth ||
         widget.type == WidgetType::Accessibility)) {
        widget.cfg.labelColor = RGB(0, 0, 0);
    } else {
        widget.cfg.labelColor = ThemeLabelColor(widget.cfg.appearance);
    }
}

static void LoadWidgetConfig(WidgetWindow& widget) {
    WCHAR settingName[128];
    WCHAR modeText[32];

    swprintf_s(settingName, L"%sEnabled", widget.prefix);
    widget.cfg.enabled = Wh_GetIntSetting(settingName) != 0;

    swprintf_s(settingName, L"%sX", widget.prefix);
    widget.cfg.x = Wh_GetIntSetting(settingName);

    swprintf_s(settingName, L"%sY", widget.prefix);
    widget.cfg.y = Wh_GetIntSetting(settingName);

    swprintf_s(settingName, L"%sWidth", widget.prefix);
    int widthSetting = Wh_GetIntSetting(settingName);
    widget.cfg.width = widthSetting <= 0 ? widget.defaultWidth : ClampInt(widthSetting, 40, 1200);

    swprintf_s(settingName, L"%sHeight", widget.prefix);
    int heightSetting = Wh_GetIntSetting(settingName);
    widget.cfg.height = heightSetting <= 0 ? widget.defaultHeight : ClampInt(heightSetting, 20, 1200);

    swprintf_s(settingName, L"%sAppearanceMode", widget.prefix);
    LoadStringSetting(settingName, modeText, _countof(modeText), L"default");
    widget.cfg.appearance = ParseAppearance(modeText, g_defaultAppearance);

    swprintf_s(settingName, L"%sOpacity", widget.prefix);
    int opacitySetting = Wh_GetIntSetting(settingName);
    widget.cfg.opacity = opacitySetting <= 0 ? g_defaultOpacity : ClampInt(opacitySetting, 1, 255);

    swprintf_s(settingName, L"%sRoundness", widget.prefix);
    int roundnessSetting = Wh_GetIntSetting(settingName);
    widget.cfg.cornerRadius = roundnessSetting <= 0 ? g_defaultRoundness : ClampInt(roundnessSetting, 0, 96);

    swprintf_s(settingName, L"%sWidgetColor", widget.prefix);
    PCWSTR widgetColor = Wh_GetStringSetting(settingName);
    widget.cfg.widgetColor = ParseRgb(widgetColor, ThemeWidgetColor(widget.cfg.appearance));
    if (widgetColor) {
        Wh_FreeStringSetting(widgetColor);
    }

    swprintf_s(settingName, L"%sLabelColor", widget.prefix);
    PCWSTR labelColor = Wh_GetStringSetting(settingName);
    widget.cfg.labelColor = ParseRgb(labelColor, ThemeLabelColor(widget.cfg.appearance));
    if (labelColor) {
        Wh_FreeStringSetting(labelColor);
    }

    wcsncpy_s(widget.cfg.fontName, _countof(widget.cfg.fontName), g_defaultFont, _TRUNCATE);

    ApplyPerWidgetDefaultLabelColors(widget);
}

static void LoadSettings() {
    WCHAR modeText[32];

    LoadStringSetting(L"Side", g_side, _countof(g_side), L"right");
    LoadStringSetting(L"AppearanceMode", modeText, _countof(modeText), L"light");
    LoadStringSetting(L"FontName", g_defaultFont, _countof(g_defaultFont), L"Calibri");

    g_defaultAppearance = ParseAppearance(modeText, AppearanceMode::Light);
    g_layoutTop = ClampInt(Wh_GetIntSetting(L"LayoutTop"), 0, 2000);
    if (g_layoutTop == 0) {
        g_layoutTop = 32;
    }

    g_layoutGap = ClampInt(Wh_GetIntSetting(L"LayoutGap"), 0, 80);
    if (g_layoutGap == 0) {
        g_layoutGap = 10;
    }

    g_defaultOpacity = ClampInt(Wh_GetIntSetting(L"WidgetOpacity"), 1, 255);
    if (g_defaultOpacity == 1) {
        g_defaultOpacity = 255;
    }

    g_defaultRoundness = ClampInt(Wh_GetIntSetting(L"WidgetRoundness"), 0, 96);
    if (g_defaultRoundness == 0) {
        g_defaultRoundness = 26;
    }

    g_widgets[0].defaultOffsetX = 0;
    g_widgets[0].defaultOffsetY = 0;

    int calendarH = g_widgets[0].defaultHeight;
    int rowY = calendarH + 30;
    g_widgets[2].defaultOffsetX = 0;
    g_widgets[2].defaultOffsetY = rowY;
    g_widgets[1].defaultOffsetX = g_widgets[2].defaultWidth + 26;
    g_widgets[1].defaultOffsetY = rowY;

    int rowHeight = g_widgets[2].defaultHeight;
    int volumeY = rowY + rowHeight + 30;
    g_widgets[3].defaultOffsetX = 0;
    g_widgets[3].defaultOffsetY = volumeY;

    int batteryY = volumeY + g_widgets[3].defaultHeight + 30;
    g_widgets[4].defaultOffsetX = 0;
    g_widgets[4].defaultOffsetY = batteryY;

    int actionsY = batteryY + g_widgets[4].defaultHeight + 30;
    g_widgets[5].defaultOffsetX = 0;
    g_widgets[5].defaultOffsetY = actionsY;
    g_widgets[6].defaultOffsetX = g_widgets[5].defaultWidth + 30;
    g_widgets[6].defaultOffsetY = actionsY;

    int utilitiesY = actionsY + g_widgets[5].defaultHeight + 30;
    for (int i = 7; i <= 10; i++) {
        g_widgets[i].defaultOffsetX = (i - 7) * (g_widgets[i].defaultWidth + 26);
        g_widgets[i].defaultOffsetY = utilitiesY;
    }

    for (WidgetWindow& widget : g_widgets) {
        LoadWidgetConfig(widget);
    }
}

static BOOL CALLBACK FindWorkerWProc(HWND hwnd, LPARAM lParam) {
    HWND defView = FindWindowExW(hwnd, nullptr, L"SHELLDLL_DefView", nullptr);

    if (defView) {
        HWND* result = (HWND*)lParam;
        HWND worker = FindWindowExW(nullptr, hwnd, L"WorkerW", nullptr);

        if (worker) {
            *result = worker;
            return FALSE;
        }

        *result = hwnd;
        return FALSE;
    }

    return TRUE;
}

static HWND GetDesktopLayerWindow() {
    HWND progman = FindWindowW(L"Progman", nullptr);

    if (progman) {
        DWORD_PTR unused = 0;
        SendMessageTimeoutW(progman, 0x052C, 0, 0, SMTO_NORMAL, 1000, &unused);
    }

    HWND worker = nullptr;
    EnumWindows(FindWorkerWProc, (LPARAM)&worker);

    if (worker && IsWindow(worker)) {
        return worker;
    }

    if (progman && IsWindow(progman)) {
        return progman;
    }

    return GetDesktopWindow();
}

static int ResolveBaseX() {
    int totalWidth = PILE_WIDTH;
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);

    if (_wcsicmp(g_side, L"left") == 0) {
        return AUTO_MARGIN;
    }

    return std::max(AUTO_MARGIN, screenWidth - totalWidth - AUTO_MARGIN);
}

static int ResolveWidgetX(const WidgetWindow& widget) {
    if (widget.cfg.x >= 0) {
        return widget.cfg.x;
    }

    return ResolveBaseX() + widget.defaultOffsetX;
}

static int ResolveWidgetY(const WidgetWindow& widget) {
    if (widget.cfg.y >= 0) {
        return widget.cfg.y;
    }

    return g_layoutTop + widget.defaultOffsetY;
}

static void ApplyLegacyAcrylic(HWND hwnd, COLORREF tintColor) {
    HMODULE user32 = GetModuleHandleW(L"user32.dll");
    auto setWindowCompositionAttribute =
        (SetWindowCompositionAttribute_t)GetProcAddress(user32, "SetWindowCompositionAttribute");

    if (!setWindowCompositionAttribute) {
        return;
    }

    BYTE r = GetRValue(tintColor);
    BYTE g = GetGValue(tintColor);
    BYTE b = GetBValue(tintColor);

    ACCENT_POLICY accent = {};
    accent.AccentState = ACCENT_ENABLE_ACRYLICBLURBEHIND;
    accent.AccentFlags = 2;
    accent.GradientColor = (0x55 << 24) | (b << 16) | (g << 8) | r;

    WINDOWCOMPOSITIONATTRIBDATA data = {};
    data.Attrib = WCA_ACCENT_POLICY;
    data.pvData = &accent;
    data.cbData = sizeof(accent);

    setWindowCompositionAttribute(hwnd, &data);
}

static void ApplyGlass(HWND hwnd, const WidgetConfig& cfg) {
    LONG_PTR exStyle = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);
    exStyle |= WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE;
    SetWindowLongPtrW(hwnd, GWL_EXSTYLE, exStyle);

    if (cfg.appearance != AppearanceMode::Acrylic &&
    cfg.appearance != AppearanceMode::Mica &&
    cfg.appearance != AppearanceMode::Tabbed) {
    SetLayeredWindowAttributes(hwnd, 0, (BYTE)cfg.opacity, LWA_ALPHA);
}

    int cornerPreference = DWMWCP_ROUND;
    DwmSetWindowAttribute(
        hwnd,
        DWMWA_WINDOW_CORNER_PREFERENCE,
        &cornerPreference,
        sizeof(cornerPreference));

    int backdrop = GetDwmBackdropValue(cfg.appearance);
    HRESULT hr = DwmSetWindowAttribute(
        hwnd,
        DWMWA_SYSTEMBACKDROP_TYPE,
        &backdrop,
        sizeof(backdrop));

    if (FAILED(hr) && cfg.appearance == AppearanceMode::Acrylic) {
        ApplyLegacyAcrylic(hwnd, cfg.widgetColor);
    }

    MARGINS margins = {-1, -1, -1, -1};
    DwmExtendFrameIntoClientArea(hwnd, &margins);
}

static HFONT CreateUiFont(const WidgetConfig& cfg, int height, int weight = FW_NORMAL) {
    return CreateFontW(
        -height,
        0,
        0,
        0,
        weight,
        FALSE,
        FALSE,
        FALSE,
        DEFAULT_CHARSET,
        OUT_DEFAULT_PRECIS,
        CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY,
        DEFAULT_PITCH | FF_SWISS,
        cfg.fontName[0] ? cfg.fontName : L"Segoe UI Variable Display");
}

static void DrawTextBlock(HDC hdc, PCWSTR text, RECT rect, HFONT font, COLORREF color, UINT format) {
    HFONT oldFont = (HFONT)SelectObject(hdc, font);
    SetTextColor(hdc, color);
    SetBkMode(hdc, TRANSPARENT);
    DrawTextW(hdc, text, -1, &rect, format);
    SelectObject(hdc, oldFont);
}

static RectF ToRectF(RECT rc) {
    return RectF((REAL)rc.left, (REAL)rc.top, (REAL)(rc.right - rc.left), (REAL)(rc.bottom - rc.top));
}

static std::unique_ptr<GraphicsPath> CreateRoundRectPath(RectF rect, REAL radius) {
    auto path = std::make_unique<GraphicsPath>();
    REAL d = radius * 2.0f;
    d = std::min(d, std::min(rect.Width, rect.Height));

    path->AddArc(rect.X, rect.Y, d, d, 180.0f, 90.0f);
    path->AddArc(rect.X + rect.Width - d, rect.Y, d, d, 270.0f, 90.0f);
    path->AddArc(rect.X + rect.Width - d, rect.Y + rect.Height - d, d, d, 0.0f, 90.0f);
    path->AddArc(rect.X, rect.Y + rect.Height - d, d, d, 90.0f, 90.0f);
    path->CloseFigure();
    return path;
}

static void DrawGlassFrame(HDC hdc, const WidgetConfig& cfg, RECT rc) {
    Graphics graphics(hdc);
    graphics.SetSmoothingMode(SmoothingModeAntiAlias);
    graphics.SetCompositingMode(CompositingModeSourceOver);

    BYTE alpha = (cfg.appearance == AppearanceMode::Acrylic ||
                  cfg.appearance == AppearanceMode::Mica ||
                  cfg.appearance == AppearanceMode::Tabbed)
                     ? 108
                     : 235;
    BYTE r = GetRValue(cfg.widgetColor);
    BYTE g = GetGValue(cfg.widgetColor);
    BYTE b = GetBValue(cfg.widgetColor);

    RectF rect(1.0f, 1.0f, (REAL)(rc.right - rc.left - 2), (REAL)(rc.bottom - rc.top - 2));
    auto path = CreateRoundRectPath(rect, (REAL)cfg.cornerRadius);

    SolidBrush fill(Color(alpha, r, g, b));
    Pen border(Color(IsLightTheme(cfg.appearance) ? 50 : 75, 255, 255, 255), 1.0f);

    graphics.FillPath(&fill, path.get());
    graphics.DrawPath(&border, path.get());
}

static void DrawPill(HDC hdc, RECT rc, COLORREF color, BYTE alpha) {
    Graphics graphics(hdc);
    graphics.SetSmoothingMode(SmoothingModeAntiAlias);
    RectF rect = ToRectF(rc);
    REAL radius = rect.Height / 2.0f;
    auto path = CreateRoundRectPath(rect, radius);
    SolidBrush brush(Color(alpha, GetRValue(color), GetGValue(color), GetBValue(color)));
    graphics.FillPath(&brush, path.get());
}

static void DrawCircle(HDC hdc, int cx, int cy, int radius, COLORREF color, BYTE alpha) {
    Graphics graphics(hdc);
    graphics.SetSmoothingMode(SmoothingModeAntiAlias);
    SolidBrush brush(Color(alpha, GetRValue(color), GetGValue(color), GetBValue(color)));
    graphics.FillEllipse(&brush, cx - radius, cy - radius, radius * 2, radius * 2);
}

static void DrawLineGdip(HDC hdc, REAL x1, REAL y1, REAL x2, REAL y2, COLORREF color, REAL width, LineCap cap = LineCapRound) {
    Graphics graphics(hdc);
    graphics.SetSmoothingMode(SmoothingModeAntiAlias);
    Pen pen(Color(255, GetRValue(color), GetGValue(color), GetBValue(color)), width);
    pen.SetStartCap(cap);
    pen.SetEndCap(cap);
    graphics.DrawLine(&pen, x1, y1, x2, y2);
}

// -------------------- Slider drawing helper --------------------
static void DrawSlider(HDC hdc, RECT trackRc, int value, COLORREF trackColor, COLORREF fillColor, COLORREF thumbColor, int thumbHeight = 18) {
    int trackW = trackRc.right - trackRc.left;
    int trackH = trackRc.bottom - trackRc.top;

    Graphics g(hdc);
    g.SetSmoothingMode(SmoothingModeAntiAlias);

    RectF trackRect((REAL)trackRc.left, (REAL)trackRc.top + (trackH - 18) / 2.0f, (REAL)trackW, 18.0f);
    auto path = CreateRoundRectPath(trackRect, 9.0f);
    SolidBrush trackBrush(Color(200, GetRValue(trackColor), GetGValue(trackColor), GetBValue(trackColor)));
    g.FillPath(&trackBrush, path.get());

    REAL fillW = (REAL)trackW * (value / 100.0f);
    RectF fillRect((REAL)trackRc.left, trackRect.Y, fillW, trackRect.Height);
    SolidBrush fillBrush(Color(255, GetRValue(fillColor), GetGValue(fillColor), GetBValue(fillColor)));
    g.FillRectangle(&fillBrush, fillRect);

    int thumbW = 12;
    int cx = trackRc.left + (int)(fillW);
    RECT thumb = { cx - thumbW/2, trackRc.top + (trackH - thumbHeight)/2, cx + thumbW/2, trackRc.top + (trackH - thumbHeight)/2 + thumbHeight };
    SolidBrush thumbBrush(Color(255, GetRValue(thumbColor), GetGValue(thumbColor), GetBValue(thumbColor)));
    RectF thumbRf((REAL)thumb.left, (REAL)thumb.top, (REAL)(thumb.right - thumb.left), (REAL)(thumb.bottom - thumb.top));
    auto thumbPath = CreateRoundRectPath(thumbRf, (REAL)(thumbHeight/2.0f));
    g.FillPath(&thumbBrush, thumbPath.get());
}

// -------------------- Endpoint volume helper (existing) --------------------
static bool GetEndpointVolume(float* volume, BOOL* muted) {
    if (volume) {
        *volume = 0.0f;
    }
    if (muted) {
        *muted = FALSE;
    }

    IMMDeviceEnumerator* enumerator = nullptr;
    IMMDevice* device = nullptr;
    IAudioEndpointVolume* endpoint = nullptr;

    HRESULT hr = CoCreateInstance(CLSID_MMDeviceEnumerator, nullptr, CLSCTX_ALL,
                                  IID_IMMDeviceEnumerator, (void**)&enumerator);
    if (FAILED(hr)) {
        return false;
    }

    hr = enumerator->GetDefaultAudioEndpoint(eRender, eConsole, &device);
    if (SUCCEEDED(hr)) {
        hr = device->Activate(IID_IAudioEndpointVolume, CLSCTX_ALL, nullptr, (void**)&endpoint);
    }

    if (SUCCEEDED(hr) && endpoint) {
        if (volume) {
            endpoint->GetMasterVolumeLevelScalar(volume);
        }
        if (muted) {
            endpoint->GetMute(muted);
        }
    }

    if (endpoint) {
        endpoint->Release();
    }
    if (device) {
        device->Release();
    }
    if (enumerator) {
        enumerator->Release();
    }

    return TRUE;
}

// -------------------- Media thumbnail & Play behavior helpers --------------------
static void FreeNowPlayingThumbnail() {
    if (g_nowPlayingThumb) {
        DeleteObject(g_nowPlayingThumb);
        g_nowPlayingThumb = nullptr;
    }
    g_nowPlayingFilePath[0] = 0;
}

static bool GetFirstAudioFileInMusicLibrary(WCHAR* outPath, size_t cchOut) {
    PWSTR musicPath = nullptr;
    HRESULT hr = SHGetKnownFolderPath(FOLDERID_Music, KF_FLAG_DEFAULT, nullptr, &musicPath);
    if (FAILED(hr) || !musicPath) {
        return false;
    }

    bool found = false;
    WCHAR searchPath[MAX_PATH];
    PathCombineW(searchPath, musicPath, L"*.*");

    WIN32_FIND_DATAW fd;
    HANDLE hFind = FindFirstFileW(searchPath, &fd);
    if (hFind != INVALID_HANDLE_VALUE) {
        do {
            if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
                LPCWSTR ext = PathFindExtensionW(fd.cFileName);
                if (ext && (*ext)) {
                    if (_wcsicmp(ext, L".mp3") == 0 || _wcsicmp(ext, L".flac") == 0 ||
                        _wcsicmp(ext, L".m4a") == 0 || _wcsicmp(ext, L".wav") == 0 ||
                        _wcsicmp(ext, L".wma") == 0 || _wcsicmp(ext, L".aac") == 0) {
                        PathCombineW(outPath, musicPath, fd.cFileName);
                        found = true;
                        break;
                    }
                }
            }
        } while (FindNextFileW(hFind, &fd));
        FindClose(hFind);
    }

    CoTaskMemFree(musicPath);
    return found;
}

static bool LoadThumbnailForFile(PCWSTR filePath, int thumbSize, HBITMAP* outBitmap) {
    if (!filePath || !*filePath || !outBitmap) {
        return false;
    }

    *outBitmap = nullptr;

    IShellItem* psi = nullptr;
    HRESULT hr = SHCreateItemFromParsingName(filePath, nullptr, IID_PPV_ARGS(&psi));
    if (FAILED(hr) || !psi) {
        return false;
    }

    IShellItemImageFactory* factory = nullptr;
    hr = psi->QueryInterface(IID_PPV_ARGS(&factory));
    if (SUCCEEDED(hr) && factory) {
        SIZE size = { thumbSize, thumbSize };
        HBITMAP hbmp = nullptr;
        hr = factory->GetImage(size, SIIGBF_BIGGERSIZEOK, &hbmp);
        if (SUCCEEDED(hr) && hbmp) {
            *outBitmap = hbmp;
            factory->Release();
            psi->Release();
            return true;
        }
        factory->Release();
    }

    psi->Release();
    return false;
}

static void TryLoadNowPlayingThumbnailFromMusicLibrary() {
    FreeNowPlayingThumbnail();

    WCHAR filePath[MAX_PATH] = {0};
    if (!GetFirstAudioFileInMusicLibrary(filePath, _countof(filePath))) {
        return;
    }

    HBITMAP hbmp = nullptr;
    if (LoadThumbnailForFile(filePath, 128, &hbmp)) {
        g_nowPlayingThumb = hbmp;
        wcsncpy_s(g_nowPlayingFilePath, _countof(g_nowPlayingFilePath), filePath, _TRUNCATE);
    }
}

static void OpenDefaultMediaPlayerAndPlay() {
    WCHAR filePath[MAX_PATH] = {0};
    if (GetFirstAudioFileInMusicLibrary(filePath, _countof(filePath))) {
        ShellExecuteW(nullptr, L"open", filePath, nullptr, nullptr, SW_SHOWNORMAL);
        FreeNowPlayingThumbnail();
        HBITMAP hbmp = nullptr;
        if (LoadThumbnailForFile(filePath, 128, &hbmp)) {
            g_nowPlayingThumb = hbmp;
            wcsncpy_s(g_nowPlayingFilePath, _countof(g_nowPlayingFilePath), filePath, _TRUNCATE);
        }
    } else {
        ShellExecuteW(nullptr, L"open", L"shell:MusicLibrary", nullptr, nullptr, SW_SHOWNORMAL);
    }

    INPUT input = {};
    input.type = INPUT_KEYBOARD;
    input.ki.wVk = VK_MEDIA_PLAY_PAUSE;
    SendInput(1, &input, sizeof(INPUT));
    input.ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(1, &input, sizeof(INPUT));
}

static void OpenWindowsSearch() {
    INPUT inputs[4] = {};
    inputs[0].type = INPUT_KEYBOARD;
    inputs[0].ki.wVk = VK_LWIN;
    inputs[1].type = INPUT_KEYBOARD;
    inputs[1].ki.wVk = 'S';
    inputs[2].type = INPUT_KEYBOARD;
    inputs[2].ki.wVk = 'S';
    inputs[2].ki.dwFlags = KEYEVENTF_KEYUP;
    inputs[3].type = INPUT_KEYBOARD;
    inputs[3].ki.wVk = VK_LWIN;
    inputs[3].ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(4, inputs, sizeof(INPUT));
}

// -------------------- Forward declarations --------------------
static void DrawCalendar(HWND hwnd, HDC hdc, const WidgetConfig& cfg, RECT rc);
static void DrawAnalogClock(HWND hwnd, HDC hdc, const WidgetConfig& cfg, RECT rc);
static void DrawNowPlaying(HWND hwnd, HDC hdc, const WidgetConfig& cfg, RECT rc);
static void DrawVolume(HWND hwnd, HDC hdc, const WidgetConfig& cfg, RECT rc);
static void DrawBattery(HWND hwnd, HDC hdc, const WidgetConfig& cfg, RECT rc);
static void DrawActionWidget(HWND hwnd, HDC hdc, const WidgetConfig& cfg, RECT rc, PCWSTR label, WidgetType type);
static void DrawUtility(HWND hwnd, HDC hdc, const WidgetConfig& cfg, RECT rc, PCWSTR label, WidgetType type);

// -------------------- Drawing implementations --------------------
static void DrawCalendar(HWND hwnd, HDC hdc, const WidgetConfig& cfg, RECT rc) {
    DrawGlassFrame(hdc, cfg, rc);

    time_t now = time(nullptr);
    tm localTime = {};
    localtime_s(&localTime, &now);

    int year = localTime.tm_year + 1900;
    int month = localTime.tm_mon;
    int today = localTime.tm_mday;

    tm firstDay = {};
    firstDay.tm_year = localTime.tm_year;
    firstDay.tm_mon = month;
    firstDay.tm_mday = 1;
    mktime(&firstDay);

    static const int daysInMonthTable[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    bool leap = (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
    int daysInMonth = month == 1 && leap ? 29 : daysInMonthTable[month];

    int width = rc.right - rc.left;
    int height = rc.bottom - rc.top;
    int left = 12;
    int top = 8;
    int gridTop = 56;
    int cellW = (width - left * 2) / 7;
    int cellH = std::max(18, (height - gridTop - 12) / 6);

    HFONT monthFont = CreateUiFont(cfg, 18, FW_SEMIBOLD);
    HFONT yearFont = CreateUiFont(cfg, 14, FW_MEDIUM);
    HFONT dayFont = CreateUiFont(cfg, 12, FW_MEDIUM);
    HFONT dateFont = CreateUiFont(cfg, 14, FW_SEMIBOLD);

    WCHAR monthName[32], yearStr[16];
    wcsftime(monthName, _countof(monthName), L"%B", &localTime);
    wcsftime(yearStr, _countof(yearStr), L"%Y", &localTime);

    COLORREF monthColor = cfg.labelColor;
    COLORREF yearColor = cfg.labelColor;

    // Place month and year on same line: month left, year right
    RECT monthRc = { left, top, left + (width/2), top + 26 };
    RECT yearRc = { left + (width/2), top, width - left, top + 26 };
    DrawTextBlock(hdc, monthName, monthRc, monthFont, monthColor, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
    DrawTextBlock(hdc, yearStr, yearRc, yearFont, yearColor, DT_RIGHT | DT_VCENTER | DT_SINGLELINE);

    const WCHAR* days[] = {L"S", L"M", L"T", L"W", L"T", L"F", L"S"};
    COLORREF subtle = BlendColor(cfg.labelColor, cfg.widgetColor, 38);

    for (int i = 0; i < 7; i++) {
        RECT dayRc = {left + i * cellW, gridTop - 20, left + (i + 1) * cellW, gridTop - 4};
        DrawTextBlock(hdc, days[i], dayRc, dayFont, subtle, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    }

    int firstWeekday = firstDay.tm_wday;

    for (int day = 1; day <= daysInMonth; day++) {
        int index = firstWeekday + day - 1;
        int row = index / 7;
        int col = index % 7;

        RECT cellRc = {
            left + col * cellW,
            gridTop + row * cellH,
            left + (col + 1) * cellW,
            gridTop + (row + 1) * cellH};

        WCHAR dayText[8];
        swprintf_s(dayText, L"%d", day);

        if (day == today) {
            COLORREF highlight = (cfg.appearance == AppearanceMode::Light) ? RGB(0x01,0x60,0xB9) : RGB(0xFE,0xD3,0x01);
            int cx = (cellRc.left + cellRc.right) / 2;
            int cy = (cellRc.top + cellRc.bottom) / 2;
            DrawCircle(hdc, cx, cy, std::min(cellW, cellH) / 2 - 3, highlight, 230);
            DrawTextBlock(hdc, dayText, cellRc, dateFont, cfg.labelColor, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        } else {
            DrawTextBlock(hdc, dayText, cellRc, dateFont, cfg.labelColor, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        }
    }

    DeleteObject(monthFont);
    DeleteObject(yearFont);
    DeleteObject(dayFont);
    DeleteObject(dateFont);
}

static void DrawAnalogClock(HWND hwnd, HDC hdc, const WidgetConfig& cfg, RECT rc) {
    DrawGlassFrame(hdc, cfg, rc);

    time_t now = time(nullptr);
    tm localTime = {};
    localtime_s(&localTime, &now);

    int width = rc.right - rc.left;
    int height = rc.bottom - rc.top;
    int dialDiameter = 150;
    int cx = width / 2;
    int cy = height / 2 - 6;
    int radius = dialDiameter / 2;
    COLORREF subtle = BlendColor(cfg.labelColor, cfg.widgetColor, 42);

    Graphics graphics(hdc);
    graphics.SetSmoothingMode(SmoothingModeAntiAlias);

    RectF dial((REAL)(cx - radius), (REAL)(cy - radius), (REAL)(radius * 2), (REAL)(radius * 2));
    auto squircle = CreateRoundRectPath(dial, 18.0f);
    SolidBrush dialBrush(Color(34, GetRValue(cfg.widgetColor), GetGValue(cfg.widgetColor), GetBValue(cfg.widgetColor)));
    graphics.FillPath(&dialBrush, squircle.get());

    for (int i = 0; i < 60; i++) {
        double angle = (i * 6.0 - 90.0) * PI_D / 180.0;
        int outer = radius - 6;
        int inner = outer - (i % 5 == 0 ? 10 : 5);
        COLORREF tickColor = (i % 5 == 0) ? ((cfg.appearance == AppearanceMode::Light) ? RGB(0xA6,0x1D,0x24) : RGB(0xFE,0xD3,0x01)) : subtle;
        REAL tickWidth = i % 5 == 0 ? 2.0f : 1.0f;
        DrawLineGdip(hdc,
            (REAL)(cx + cos(angle) * inner),
            (REAL)(cy + sin(angle) * inner),
            (REAL)(cx + cos(angle) * outer),
            (REAL)(cy + sin(angle) * outer),
            tickColor,
            tickWidth);
    }

    HFONT numberFont = CreateUiFont(cfg, 14, FW_SEMIBOLD);
    for (int n = 1; n <= 12; n++) {
        double angle = (n * 30.0 - 90.0) * PI_D / 180.0;
        int tx = (int)(cx + cos(angle) * (radius - 28));
        int ty = (int)(cy + sin(angle) * (radius - 28));
        RECT numRc = {tx - 12, ty - 10, tx + 12, ty + 10};
        WCHAR text[4];
        swprintf_s(text, L"%d", n);
        COLORREF numColor = (cfg.appearance == AppearanceMode::Light) ? RGB(0,0,0) : RGB(0xFE,0xD3,0x01);
        DrawTextBlock(hdc, text, numRc, numberFont, numColor, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    }

    double minuteAngle = ((localTime.tm_min + localTime.tm_sec / 60.0) * 6.0 - 90.0) * PI_D / 180.0;
    double hourAngle = (((localTime.tm_hour % 12) + localTime.tm_min / 60.0) * 30.0 - 90.0) * PI_D / 180.0;

    COLORREF handColor = (cfg.appearance == AppearanceMode::Light) ? RGB(0,0,0) : RGB(0xA6,0x1D,0x24);
    DrawLineGdip(hdc, (REAL)cx, (REAL)cy, (REAL)(cx + cos(hourAngle) * (radius * 0.42)), (REAL)(cy + sin(hourAngle) * (radius * 0.42)), handColor, 9.0f);
    DrawLineGdip(hdc, (REAL)cx, (REAL)cy, (REAL)(cx + cos(minuteAngle) * (radius * 0.63)), (REAL)(cy + sin(minuteAngle) * (radius * 0.63)), handColor, 6.0f);
    DrawCircle(hdc, cx, cy, 6, handColor, 255);

    DeleteObject(numberFont);

    WCHAR dateText[80];
    wcsftime(dateText, _countof(dateText), L"%A, %d %B", &localTime);
    HFONT dateFont = CreateUiFont(cfg, 12, FW_MEDIUM);
    RECT dateRc = {8, height - 22, width - 8, height - 6};
    DrawTextBlock(hdc, dateText, dateRc, dateFont, subtle, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    DeleteObject(dateFont);
}

static RECT MediaButtonRect(HWND hwnd, HitZone zone) {
    RECT rc;
    GetClientRect(hwnd, &rc);
    int width = rc.right - rc.left;
    int height = rc.bottom - rc.top;

    int btnSize = 34;
    int spacing = 18;
    int totalWidth = btnSize * 3 + spacing * 2;
    int startX = (width - totalWidth) / 2;
    int y = height - 46;

    RECT rPrev = { startX, y, startX + btnSize, y + btnSize };
    RECT rPlay = { startX + (btnSize + spacing), y, startX + (btnSize + spacing) + btnSize, y + btnSize };
    RECT rNext = { startX + 2 * (btnSize + spacing), y, startX + 2 * (btnSize + spacing) + btnSize, y + btnSize };

    switch (zone) {
        case HitZone::MediaPrev: return rPrev;
        case HitZone::MediaPlayPause: return rPlay;
        case HitZone::MediaNext: return rNext;
        default: return rPlay;
    }
}

static void DrawMediaIcon(HDC hdc, RECT btn, HitZone zone, COLORREF color) {
    Graphics graphics(hdc);
    graphics.SetSmoothingMode(SmoothingModeAntiAlias);
    SolidBrush brush(Color(255, GetRValue(color), GetGValue(color), GetBValue(color)));

    int cx = (btn.left + btn.right) / 2;
    int cy = (btn.top + btn.bottom) / 2;
    int size = (btn.right - btn.left) / 2 - 4;

    if (zone == HitZone::MediaPlayPause) {
        Point pts[3] = {
            Point(cx - size/2, cy - size),
            Point(cx - size/2, cy + size),
            Point(cx + size, cy)
        };
        graphics.FillPolygon(&brush, pts, 3);
    } else if (zone == HitZone::MediaPrev) {
        Point pts1[3] = {
            Point(cx - size/2 + 6, cy),
            Point(cx + size/2 + 6, cy - size),
            Point(cx + size/2 + 6, cy + size)
        };
        Point pts2[3] = {
            Point(cx - size/2 - 6, cy),
            Point(cx + size/2 - 6, cy - size),
            Point(cx + size/2 - 6, cy + size)
        };
        graphics.FillPolygon(&brush, pts1, 3);
        graphics.FillPolygon(&brush, pts2, 3);
        Pen pen(Color(255, GetRValue(color), GetGValue(color), GetBValue(color)), 4.0f);
        graphics.DrawLine(&pen, (REAL)(btn.right - 10), (REAL)(btn.top + 6), (REAL)(btn.right - 10), (REAL)(btn.bottom - 6));
    } else if (zone == HitZone::MediaNext) {
        Point pts1[3] = {
            Point(cx + size/2 - 6, cy),
            Point(cx - size/2 - 6, cy - size),
            Point(cx - size/2 - 6, cy + size)
        };
        Point pts2[3] = {
            Point(cx + size/2 + 6, cy),
            Point(cx - size/2 + 6, cy - size),
            Point(cx - size/2 + 6, cy + size)
        };
        graphics.FillPolygon(&brush, pts1, 3);
        graphics.FillPolygon(&brush, pts2, 3);
        Pen pen(Color(255, GetRValue(color), GetGValue(color), GetBValue(color)), 4.0f);
        graphics.DrawLine(&pen, (REAL)(btn.left + 10), (REAL)(btn.top + 6), (REAL)(btn.left + 10), (REAL)(btn.bottom - 6));
    }
}

static COLORREF ActionIconColor(const WidgetConfig& cfg) {
    return (cfg.appearance == AppearanceMode::Light) ? RGB(0x01,0x60,0xB9) : RGB(0xFE,0xD3,0x01);
}

static COLORREF UtilityIconColor(const WidgetConfig& cfg) {
    return (cfg.appearance == AppearanceMode::Light) ? RGB(0,0,0) : RGB(255,255,255);
}

static void DrawSearchIcon(HDC hdc, RECT rc, COLORREF color) {
    Graphics graphics(hdc);
    graphics.SetSmoothingMode(SmoothingModeAntiAlias);
    Pen pen(Color(255, GetRValue(color), GetGValue(color), GetBValue(color)), 2.4f);
    int size = std::min(rc.right - rc.left, rc.bottom - rc.top);
    int cx = (rc.left + rc.right) / 2 - 2;
    int cy = (rc.top + rc.bottom) / 2 - 2;
    int r = size / 4;
    graphics.DrawEllipse(&pen, cx - r, cy - r, r * 2, r * 2);
    graphics.DrawLine(&pen, (REAL)(cx + r - 1), (REAL)(cy + r - 1), (REAL)(rc.right - 3), (REAL)(rc.bottom - 3));
}

static void DrawBrushIcon(HDC hdc, RECT rc, COLORREF color) {
    Graphics graphics(hdc);
    graphics.SetSmoothingMode(SmoothingModeAntiAlias);
    Pen pen(Color(255, GetRValue(color), GetGValue(color), GetBValue(color)), 2.8f);
    SolidBrush brush(Color(255, GetRValue(color), GetGValue(color), GetBValue(color)));
    graphics.DrawLine(&pen, (REAL)(rc.left + 6), (REAL)(rc.bottom - 7), (REAL)(rc.right - 6), (REAL)(rc.top + 6));
    Point head[4] = {
        Point(rc.left + 4, rc.bottom - 5),
        Point(rc.left + 10, rc.bottom - 10),
        Point(rc.left + 14, rc.bottom - 6),
        Point(rc.left + 8, rc.bottom - 2)
    };
    graphics.FillPolygon(&brush, head, 4);
}

static void DrawWifiIcon(HDC hdc, RECT rc, COLORREF color) {
    Graphics graphics(hdc);
    graphics.SetSmoothingMode(SmoothingModeAntiAlias);
    Pen pen(Color(255, GetRValue(color), GetGValue(color), GetBValue(color)), 2.4f);
    SolidBrush brush(Color(255, GetRValue(color), GetGValue(color), GetBValue(color)));
    int cx = (rc.left + rc.right) / 2;
    int cy = rc.bottom - 4;
    graphics.DrawArc(&pen, cx - 24, cy - 24, 48, 48, 220, 100);
    graphics.DrawArc(&pen, cx - 16, cy - 16, 32, 32, 220, 100);
    graphics.DrawArc(&pen, cx - 8, cy - 8, 16, 16, 220, 100);
    graphics.FillEllipse(&brush, cx - 3, cy - 3, 6, 6);
}

static void DrawHotspotIcon(HDC hdc, RECT rc, COLORREF color) {
    Graphics graphics(hdc);
    graphics.SetSmoothingMode(SmoothingModeAntiAlias);
    Pen pen(Color(255, GetRValue(color), GetGValue(color), GetBValue(color)), 2.2f);
    SolidBrush brush(Color(255, GetRValue(color), GetGValue(color), GetBValue(color)));
    int cx = (rc.left + rc.right) / 2;
    int cy = (rc.top + rc.bottom) / 2 + 2;
    graphics.FillEllipse(&brush, cx - 4, cy - 4, 8, 8);
    graphics.DrawEllipse(&pen, cx - 12, cy - 12, 24, 24);
    graphics.DrawEllipse(&pen, cx - 21, cy - 21, 42, 42);
}

static void DrawBluetoothIcon(HDC hdc, RECT rc, COLORREF color) {
    Graphics graphics(hdc);
    graphics.SetSmoothingMode(SmoothingModeAntiAlias);
    Pen pen(Color(255, GetRValue(color), GetGValue(color), GetBValue(color)), 2.4f);
    int cx = (rc.left + rc.right) / 2;
    int top = rc.top + 3;
    int bottom = rc.bottom - 3;
    int mid = (top + bottom) / 2;
    Point upper[3] = {Point(cx, top), Point(cx + 11, mid - 8), Point(cx, mid)};
    Point lower[3] = {Point(cx, mid), Point(cx + 11, mid + 8), Point(cx, bottom)};
    graphics.DrawLine(&pen, (REAL)cx, (REAL)top, (REAL)cx, (REAL)bottom);
    graphics.DrawLines(&pen, upper, 3);
    graphics.DrawLines(&pen, lower, 3);
    graphics.DrawLine(&pen, (REAL)(cx - 11), (REAL)(mid - 9), (REAL)(cx + 11), (REAL)(mid + 8));
    graphics.DrawLine(&pen, (REAL)(cx - 11), (REAL)(mid + 9), (REAL)(cx + 11), (REAL)(mid - 8));
}

static void DrawAccessibilityIcon(HDC hdc, RECT rc, COLORREF color) {
    Graphics graphics(hdc);
    graphics.SetSmoothingMode(SmoothingModeAntiAlias);
    Pen pen(Color(255, GetRValue(color), GetGValue(color), GetBValue(color)), 2.6f);
    SolidBrush brush(Color(255, GetRValue(color), GetGValue(color), GetBValue(color)));
    int cx = (rc.left + rc.right) / 2;
    int top = rc.top + 3;
    graphics.FillEllipse(&brush, cx - 4, top, 8, 8);
    graphics.DrawLine(&pen, (REAL)(cx - 18), (REAL)(top + 14), (REAL)(cx + 18), (REAL)(top + 14));
    graphics.DrawLine(&pen, (REAL)cx, (REAL)(top + 14), (REAL)cx, (REAL)(top + 28));
    graphics.DrawLine(&pen, (REAL)cx, (REAL)(top + 28), (REAL)(cx - 12), (REAL)(rc.bottom - 3));
    graphics.DrawLine(&pen, (REAL)cx, (REAL)(top + 28), (REAL)(cx + 12), (REAL)(rc.bottom - 3));
}

static void DrawMusicNoteIcon(HDC hdc, RECT rc, COLORREF color) {
    Graphics graphics(hdc);
    graphics.SetSmoothingMode(SmoothingModeAntiAlias);
    Pen pen(Color(255, GetRValue(color), GetGValue(color), GetBValue(color)), 3.0f);
    SolidBrush brush(Color(255, GetRValue(color), GetGValue(color), GetBValue(color)));
    int stemX = rc.right - 18;
    int stemTop = rc.top + 10;
    int stemBottom = rc.bottom - 18;
    graphics.DrawLine(&pen, (REAL)stemX, (REAL)stemTop, (REAL)stemX, (REAL)stemBottom);
    graphics.DrawLine(&pen, (REAL)(stemX - 1), (REAL)stemTop, (REAL)(rc.left + 18), (REAL)(stemTop + 8));
    graphics.FillEllipse(&brush, rc.left + 10, stemBottom - 2, 18, 12);
}

static void DrawNowPlaying(HWND hwnd, HDC hdc, const WidgetConfig& cfg, RECT rc) {
    DrawGlassFrame(hdc, cfg, rc);

    int width = rc.right - rc.left;
    int height = rc.bottom - rc.top;
    COLORREF subtle = BlendColor(cfg.labelColor, cfg.widgetColor, 42);

    HFONT smallFont = CreateUiFont(cfg, 12, FW_MEDIUM);
    HFONT titleFont = CreateUiFont(cfg, 16, FW_SEMIBOLD);
    HFONT bodyFont = CreateUiFont(cfg, 12, FW_MEDIUM);

    RECT labelRc = {8, 6, width - 8, 28};
    DrawTextBlock(hdc, L"Now Playing", labelRc, smallFont, subtle, DT_LEFT | DT_VCENTER | DT_SINGLELINE);

    RECT titleRc = {8, 36, width - 8, 64};
    DrawTextBlock(hdc, L"Media controls", titleRc, titleFont, cfg.labelColor, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    RECT thumbRc = {12, 72, 72, 142};

    if (g_nowPlayingThumb) {
        Bitmap* bmp = Bitmap::FromHBITMAP(g_nowPlayingThumb, nullptr);
        if (bmp) {
            Graphics graphics(hdc);
            graphics.SetSmoothingMode(SmoothingModeAntiAlias);
            Rect dest(thumbRc.left, thumbRc.top, thumbRc.right - thumbRc.left, thumbRc.bottom - thumbRc.top);
            graphics.DrawImage(bmp, dest);
            delete bmp;
        } else {
            DrawPill(hdc, thumbRc, BlendColor(cfg.widgetColor, cfg.labelColor, 20), 200);
            DrawMusicNoteIcon(hdc, thumbRc, UtilityIconColor(cfg));
        }
    } else {
        DrawPill(hdc, thumbRc, BlendColor(cfg.widgetColor, cfg.labelColor, 20), 200);
        DrawMusicNoteIcon(hdc, thumbRc, UtilityIconColor(cfg));
    }

    RECT metaRc = {84, 72, width - 12, 120};
    DrawTextBlock(hdc, L"Track metadata is not available in this Windhawk context.", metaRc, bodyFont, subtle, DT_LEFT | DT_WORDBREAK);

    HitZone zones[] = {HitZone::MediaPrev, HitZone::MediaPlayPause, HitZone::MediaNext};
    for (HitZone zone : zones) {
        RECT btn = MediaButtonRect(hwnd, zone);
        COLORREF playColor = RGB(0xA6,0x1D,0x24);
        COLORREF prevNextColor = RGB(0x01,0x60,0xB9);
        COLORREF fill = (zone == HitZone::MediaPlayPause) ? playColor : prevNextColor;
        DrawCircle(hdc, (btn.left + btn.right) / 2, (btn.top + btn.bottom) / 2, (btn.right - btn.left) / 2, fill, 230);
        DrawMediaIcon(hdc, btn, zone, (zone == HitZone::MediaPlayPause) ? cfg.widgetColor : cfg.labelColor);
    }

    RECT hintRc = {8, height - 20, width - 8, height - 4};
    DrawTextBlock(hdc, L"Previous  Play/Pause  Next", hintRc, smallFont, subtle, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    DeleteObject(smallFont);
    DeleteObject(titleFont);
    DeleteObject(bodyFont);
}

// -------------------- System helpers for volume/battery --------------------
static bool SetSystemVolumePercent(int percent) {
    float scalar = ClampInt(percent, 0, 100) / 100.0f;
    IMMDeviceEnumerator* enumerator = nullptr;
    IMMDevice* device = nullptr;
    IAudioEndpointVolume* endpoint = nullptr;
    HRESULT hr = CoCreateInstance(CLSID_MMDeviceEnumerator, nullptr, CLSCTX_ALL, IID_IMMDeviceEnumerator, (void**)&enumerator);
    if (FAILED(hr)) return false;
    hr = enumerator->GetDefaultAudioEndpoint(eRender, eConsole, &device);
    if (SUCCEEDED(hr)) hr = device->Activate(IID_IAudioEndpointVolume, CLSCTX_ALL, nullptr, (void**)&endpoint);
    if (SUCCEEDED(hr) && endpoint) {
        hr = endpoint->SetMasterVolumeLevelScalar(scalar, nullptr);
        endpoint->Release();
    }
    if (device) device->Release();
    if (enumerator) enumerator->Release();
    return SUCCEEDED(hr);
}

static int GetSystemVolumePercent() {
    float vol = 0.0f;
    BOOL muted = FALSE;
    if (!GetEndpointVolume(&vol, &muted)) return -1;
    return ClampInt((int)round(vol * 100.0f), 0, 100);
}

static int GetBatteryPercent() {
    SYSTEM_POWER_STATUS sps = {};
    if (GetSystemPowerStatus(&sps)) {
        if (sps.BatteryLifePercent != 255) {
            return ClampInt((int)sps.BatteryLifePercent, 0, 100);
        }
    }
    return -1;
}

// -------------------- Draw volume/battery with sliders --------------------
static void DrawVolume(HWND hwnd, HDC hdc, const WidgetConfig& cfg, RECT rc) {
    DrawGlassFrame(hdc, cfg, rc);

    int padding = 8;
    int width = rc.right - rc.left;
    int height = rc.bottom - rc.top;

    HFONT f = CreateUiFont(cfg, 12, FW_MEDIUM);
    RECT labelRc = { rc.left + padding, rc.top, rc.left + 120, rc.bottom };
    DrawTextBlock(hdc, L"Volume", labelRc, f, cfg.labelColor, DT_LEFT | DT_VCENTER | DT_SINGLELINE);

    int sliderW = 260;
    int sliderH = 18;
    int sliderX = rc.left + 120 + 8;
    int sliderY = rc.top + (height - sliderH) / 2;
    RECT trackRc = { sliderX, sliderY, sliderX + sliderW, sliderY + sliderH };

    RECT pctRc = { trackRc.right + 8, rc.top + (height - 20) / 2, trackRc.right + 8 + 60, rc.top + (height - 20) / 2 + 20 };

    WidgetWindow* widget = (WidgetWindow*)GetWindowLongPtrW(hwnd, GWLP_USERDATA);
    int value = widget ? widget->sliderValue : GetSystemVolumePercent();
    if (value < 0) value = GetSystemVolumePercent();
    if (value < 0) value = 0;

    COLORREF fillColor = (cfg.appearance == AppearanceMode::Light) ? RGB(0xA0,0x6F,0xC1) : RGB(0x56,0xA4,0x02);
    COLORREF trackColor = BlendColor(cfg.widgetColor, cfg.labelColor, 20);
    COLORREF thumbColor = fillColor;

    DrawSlider(hdc, trackRc, value, trackColor, fillColor, thumbColor, sliderH);

    WCHAR pctText[16];
    swprintf_s(pctText, L"%d%%", value);
    HFONT pctFont = CreateUiFont(cfg, 12, FW_SEMIBOLD);
    DrawTextBlock(hdc, pctText, pctRc, pctFont, cfg.labelColor, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    if (widget) {
        widget->sliderRect = trackRc;
        int thumbCenter = trackRc.left + (int)((sliderW * (value/100.0f)));
        widget->sliderThumbRect = { thumbCenter - 6, trackRc.top, thumbCenter + 6, trackRc.bottom };
        widget->sliderValue = value;
    }

    DeleteObject(f);
    DeleteObject(pctFont);
}

static void DrawBattery(HWND hwnd, HDC hdc, const WidgetConfig& cfg, RECT rc) {
    DrawGlassFrame(hdc, cfg, rc);

    int padding = 8;
    int width = rc.right - rc.left;
    int height = rc.bottom - rc.top;

    HFONT f = CreateUiFont(cfg, 12, FW_MEDIUM);
    RECT labelRc = { rc.left + padding, rc.top, rc.left + 120, rc.bottom };
    DrawTextBlock(hdc, L"Battery", labelRc, f, cfg.labelColor, DT_LEFT | DT_VCENTER | DT_SINGLELINE);

    int sliderW = 260;
    int sliderH = 18;
    int sliderX = rc.left + 120 + 8;
    int sliderY = rc.top + (height - sliderH) / 2;
    RECT trackRc = { sliderX, sliderY, sliderX + sliderW, sliderY + sliderH };

    RECT pctRc = { trackRc.right + 8, rc.top + (height - 20) / 2, trackRc.right + 8 + 60, rc.top + (height - 20) / 2 + 20 };

    int value = GetBatteryPercent();
    if (value < 0) value = 0;

    COLORREF fillColor = (cfg.appearance == AppearanceMode::Light) ? RGB(0x56,0xA4,0x02) : RGB(0x01,0x60,0xB9);
    COLORREF trackColor = BlendColor(cfg.widgetColor, cfg.labelColor, 20);
    COLORREF thumbColor = fillColor;

    DrawSlider(hdc, trackRc, value, trackColor, fillColor, thumbColor, sliderH);

    WCHAR pctText[16];
    swprintf_s(pctText, L"%d%%", value);
    HFONT pctFont = CreateUiFont(cfg, 12, FW_SEMIBOLD);
    DrawTextBlock(hdc, pctText, pctRc, pctFont, cfg.labelColor, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

    WidgetWindow* widget = (WidgetWindow*)GetWindowLongPtrW(hwnd, GWLP_USERDATA);
    if (widget) {
        widget->sliderRect = trackRc;
        int thumbCenter = trackRc.left + (int)((sliderW * (value/100.0f)));
        widget->sliderThumbRect = { thumbCenter - 6, trackRc.top, thumbCenter + 6, trackRc.bottom };
        widget->sliderValue = value;
    }

    DeleteObject(f);
    DeleteObject(pctFont);
}

static void DrawActionWidget(HWND hwnd, HDC hdc, const WidgetConfig& cfg, RECT rc, PCWSTR label, WidgetType type) {
    DrawGlassFrame(hdc, cfg, rc);
    HFONT f = CreateUiFont(cfg, 12, FW_MEDIUM);
    RECT iconRc = { rc.left + 12, rc.top + 7, rc.left + 32, rc.top + 27 };
    RECT labelRc = { rc.left + 42, rc.top, rc.right - 8, rc.bottom };
    COLORREF iconColor = ActionIconColor(cfg);

    if (type == WidgetType::WindowsSearch) {
        DrawSearchIcon(hdc, iconRc, iconColor);
    } else {
        DrawBrushIcon(hdc, iconRc, iconColor);
    }

    DrawTextBlock(hdc, label, labelRc, f, cfg.labelColor, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
    DeleteObject(f);
}

static void DrawUtility(HWND hwnd, HDC hdc, const WidgetConfig& cfg, RECT rc, PCWSTR label, WidgetType type) {
    DrawGlassFrame(hdc, cfg, rc);
    int width = rc.right - rc.left;
    HFONT f = CreateUiFont(cfg, 9, FW_MEDIUM);
    RECT iconRc = { rc.left + 14, rc.top + 8, rc.left + width - 14, rc.top + 38 };
    RECT labelRc = { rc.left + 2, rc.top + 43, rc.right - 2, rc.bottom - 4 };
    COLORREF iconColor = UtilityIconColor(cfg);

    switch (type) {
        case WidgetType::Wifi:
            DrawWifiIcon(hdc, iconRc, iconColor);
            break;
        case WidgetType::Hotspot:
            DrawHotspotIcon(hdc, iconRc, iconColor);
            break;
        case WidgetType::Bluetooth:
            DrawBluetoothIcon(hdc, iconRc, iconColor);
            break;
        case WidgetType::Accessibility:
        default:
            DrawAccessibilityIcon(hdc, iconRc, iconColor);
            break;
    }

    DrawTextBlock(hdc, label, labelRc, f, cfg.labelColor, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    DeleteObject(f);
}

// -------------------- Painting and widget lifecycle --------------------
static void PaintWidget(HWND hwnd, WidgetWindow* widget) {
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hwnd, &ps);

    RECT rc;
    GetClientRect(hwnd, &rc);
    int width  = rc.right  - rc.left;
    int height = rc.bottom - rc.top;

    HDC memDC = CreateCompatibleDC(hdc);

    BITMAPINFO bmi = {};
    bmi.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth       = width;
    bmi.bmiHeader.biHeight      = -height;
    bmi.bmiHeader.biPlanes      = 1;
    bmi.bmiHeader.biBitCount    = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    void* pvBits = nullptr;
    HBITMAP memBitmap = CreateDIBSection(hdc, &bmi, DIB_RGB_COLORS, &pvBits, nullptr, 0);
    HBITMAP oldBitmap = (HBITMAP)SelectObject(memDC, memBitmap);

    memset(pvBits, 0, width * height * 4);

    SetBkMode(memDC, TRANSPARENT);

    switch (widget->type) {
        case WidgetType::Calendar:
            DrawCalendar(hwnd, memDC, widget->cfg, rc);
            break;
        case WidgetType::NowPlaying:
            DrawNowPlaying(hwnd, memDC, widget->cfg, rc);
            break;
        case WidgetType::Clock:
            DrawAnalogClock(hwnd, memDC, widget->cfg, rc);
            break;
        case WidgetType::Volume:
            DrawVolume(hwnd, memDC, widget->cfg, rc);
            break;
        case WidgetType::Battery:
            DrawBattery(hwnd, memDC, widget->cfg, rc);
            break;
        case WidgetType::WindowsSearch:
            DrawActionWidget(hwnd, memDC, widget->cfg, rc, L"Windows Search", widget->type);
            break;
        case WidgetType::Personalization:
            DrawActionWidget(hwnd, memDC, widget->cfg, rc, L"Personalization", widget->type);
            break;
        case WidgetType::Wifi:
            DrawUtility(hwnd, memDC, widget->cfg, rc, L"WiFi", widget->type);
            break;
        case WidgetType::Hotspot:
            DrawUtility(hwnd, memDC, widget->cfg, rc, L"Hotspot", widget->type);
            break;
        case WidgetType::Bluetooth:
            DrawUtility(hwnd, memDC, widget->cfg, rc, L"Bluetooth", widget->type);
            break;
        case WidgetType::Accessibility:
            DrawUtility(hwnd, memDC, widget->cfg, rc, L"Accessibility", widget->type);
            break;
    }

    BitBlt(hdc, 0, 0, width, height, memDC, 0, 0, SRCCOPY);

    SelectObject(memDC, oldBitmap);
    DeleteObject(memBitmap);
    DeleteDC(memDC);

    EndPaint(hwnd, &ps);
}

static void CreateWidget(WidgetWindow& widget) {
    if (!widget.cfg.enabled) {
        return;
    }

    int x = ResolveWidgetX(widget);
    int y = ResolveWidgetY(widget);

    HWND hwnd = CreateWindowExW(
        WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
        CLASS_NAME,
        widget.title,
        WS_POPUP,
        x,
        y,
        widget.cfg.width,
        widget.cfg.height,
        nullptr,
        nullptr,
        g_hinst,
        &widget);

    if (!hwnd) {
        Wh_Log(L"Failed to create widget window");
        return;
    }

    widget.hwnd = hwnd;

    if (g_parent && IsWindow(g_parent)) {
        SetParent(hwnd, g_parent);
    }

    ApplyGlass(hwnd, widget.cfg);

    UINT interval = 1000;
    if (widget.type == WidgetType::Battery || widget.type == WidgetType::Volume) {
        interval = 5000;
    }
    SetTimer(hwnd, TIMER_ID, interval, nullptr);

    ShowWindow(hwnd, SW_SHOWNOACTIVATE);
    UpdateWindow(hwnd);
}

static void CreateWidgets() {
    g_parent = GetDesktopLayerWindow();

    for (WidgetWindow& widget : g_widgets) {
        CreateWidget(widget);
    }
}

static void DestroyWidgets() {
    for (WidgetWindow& widget : g_widgets) {
        if (widget.hwnd && IsWindow(widget.hwnd)) {
            DestroyWindow(widget.hwnd);
            widget.hwnd = nullptr;
        }
    }
    FreeNowPlayingThumbnail();
}

// -------------------- Window proc --------------------
static LRESULT CALLBACK WidgetWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    WidgetWindow* widget = (WidgetWindow*)GetWindowLongPtrW(hwnd, GWLP_USERDATA);

    switch (msg) {
        case WM_CREATE: {
            CREATESTRUCTW* cs = (CREATESTRUCTW*)lParam;
            widget = (WidgetWindow*)cs->lpCreateParams;
            SetWindowLongPtrW(hwnd, GWLP_USERDATA, (LONG_PTR)widget);
            break;
        }
        case WM_LBUTTONDOWN: {
            if (!widget) break;
            POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };

            // Slider handling for Volume
            if (widget->type == WidgetType::Volume && PtInRect(&widget->sliderRect, pt)) {
                widget->sliderTracking = true;
                SetCapture(hwnd);
                int sliderW = widget->sliderRect.right - widget->sliderRect.left;
                int relX = pt.x - widget->sliderRect.left;
                int newVal = ClampInt((int)round((relX / (double)sliderW) * 100.0), 0, 100);
                widget->sliderValue = newVal;
                SetSystemVolumePercent(newVal);
                InvalidateRect(hwnd, nullptr, TRUE);
                return 0;
            }

            // NowPlaying media buttons
            if (widget->type == WidgetType::NowPlaying) {
                RECT rPlay = MediaButtonRect(hwnd, HitZone::MediaPlayPause);
                RECT rPrev = MediaButtonRect(hwnd, HitZone::MediaPrev);
                RECT rNext = MediaButtonRect(hwnd, HitZone::MediaNext);
                if (PtInRect(&rPlay, pt)) {
                    OpenDefaultMediaPlayerAndPlay();
                    InvalidateRect(hwnd, nullptr, TRUE);
                    return 0;
                } else if (PtInRect(&rPrev, pt)) {
                    INPUT input = {};
                    input.type = INPUT_KEYBOARD;
                    input.ki.wVk = VK_MEDIA_PREV_TRACK;
                    SendInput(1, &input, sizeof(INPUT));
                    input.ki.dwFlags = KEYEVENTF_KEYUP;
                    SendInput(1, &input, sizeof(INPUT));
                    return 0;
                } else if (PtInRect(&rNext, pt)) {
                    INPUT input = {};
                    input.type = INPUT_KEYBOARD;
                    input.ki.wVk = VK_MEDIA_NEXT_TRACK;
                    SendInput(1, &input, sizeof(INPUT));
                    input.ki.dwFlags = KEYEVENTF_KEYUP;
                    SendInput(1, &input, sizeof(INPUT));
                    return 0;
                }
            }

            if (widget->type == WidgetType::WindowsSearch) {
                OpenWindowsSearch();
                return 0;
            }

            if (widget->type == WidgetType::Personalization) {
                OpenSettingsUri(L"ms-settings:personalization");
                return 0;
            }

            if (widget->type == WidgetType::Wifi) {
                OpenSettingsUri(L"ms-settings:network-wifi");
                return 0;
            }

            if (widget->type == WidgetType::Hotspot) {
                OpenSettingsUri(L"ms-settings:network-mobilehotspot");
                return 0;
            }

            if (widget->type == WidgetType::Bluetooth) {
                OpenSettingsUri(L"ms-settings:bluetooth");
                return 0;
            }

            if (widget->type == WidgetType::Accessibility) {
                OpenSettingsUri(L"ms-settings:easeofaccess-home");
                return 0;
            }

            break;
        }
        case WM_MOUSEMOVE: {
            if (!widget) break;
            if (widget->sliderTracking) {
                POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
                int sliderW = widget->sliderRect.right - widget->sliderRect.left;
                int relX = pt.x - widget->sliderRect.left;
                int newVal = ClampInt((int)round((relX / (double)sliderW) * 100.0), 0, 100);
                widget->sliderValue = newVal;
                SetSystemVolumePercent(newVal);
                InvalidateRect(hwnd, nullptr, TRUE);
                return 0;
            }
            break;
        }
        case WM_LBUTTONUP: {
            if (!widget) break;
            if (widget->sliderTracking) {
                widget->sliderTracking = false;
                ReleaseCapture();
                InvalidateRect(hwnd, nullptr, TRUE);
                return 0;
            }
            break;
        }
        case WM_PAINT:
            if (widget) {
                PaintWidget(hwnd, widget);
            }
            return 0;
        case WM_TIMER:
            if (widget) {
                InvalidateRect(hwnd, nullptr, TRUE);
            }
            return 0;
        case WM_DESTROY:
            if (widget && widget->type == WidgetType::NowPlaying) {
                FreeNowPlayingThumbnail();
            }
            return 0;
    }

    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

static bool RegisterWidgetClass() {
    WNDCLASSEXW wc = {};
    wc.cbSize = sizeof(wc);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WidgetWndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = g_hinst;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = nullptr;
    wc.lpszClassName = CLASS_NAME;

    return RegisterClassExW(&wc) != 0;
}

BOOL Wh_ModInit() {
    g_hinst = GetModuleHandleW(nullptr);

    HRESULT coHr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    g_comInitialized = SUCCEEDED(coHr);

    GdiplusStartupInput gdiplusStartupInput;
    if (GdiplusStartup(&g_gdiplusToken, &gdiplusStartupInput, nullptr) != Ok) {
        Wh_Log(L"Failed to initialize GDI+");
        if (g_comInitialized) {
            CoUninitialize();
            g_comInitialized = false;
        }
        return FALSE;
    }

    LoadSettings();

    if (!RegisterWidgetClass()) {
        Wh_Log(L"Failed to register widget window class");
        GdiplusShutdown(g_gdiplusToken);
        g_gdiplusToken = 0;
        if (g_comInitialized) {
            CoUninitialize();
            g_comInitialized = false;
        }
        return FALSE;
    }

    CreateWidgets();
    return TRUE;
}

void Wh_ModUninit() {
    DestroyWidgets();
    UnregisterClassW(CLASS_NAME, g_hinst);

    if (g_gdiplusToken) {
        GdiplusShutdown(g_gdiplusToken);
        g_gdiplusToken = 0;
    }

    if (g_comInitialized) {
        CoUninitialize();
        g_comInitialized = false;
    }
}

void Wh_ModSettingsChanged() {
    DestroyWidgets();
    LoadSettings();
    CreateWidgets();
}
