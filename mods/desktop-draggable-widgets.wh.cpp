// ==WindhawkMod==
// @id              desktop-draggable-widgets
// @name            JonaOS Draggable Desktop Widgets
// @description     Draggable widgets with transparent, light, and dark themes. You can add a custom folder path in the mod settings to open your desired music folder.
// @version         7
// @author          Jona like it, code it
// @github          https://github.com/Stunning-dev
// @include         windhawk.exe
// @compilerOptions -ldwmapi -lgdiplus -lgdi32 -lole32 -lshell32
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# JonaOS Draggable Desktop Widgets
View Calendar, Time, Battery, Open your Music easily and fast without need of navigating through folders to reach Music folder, and Control Volume really fast by dragging the slider. Dragging moves them to any position on the Desktop and positions are remembered, the Quick Settings shortcuts need Windows 11 (Windows 10 falls back to full Settings pages), and the panel is primary-monitor only.
![image](https://i.imgur.com/OT1h1u1.png)
*LiquidGlass*
![image](https://i.imgur.com/DjpCK5N.png)
*WhiteTransparent*
![image](https://i.imgur.com/9gl361Y.png)
*AcrylicTranslucent*
![image](https://i.imgur.com/KY8iKNf.png)
*JonaOSLight*
![image](https://i.imgur.com/MOJocT6.png)
*JonaOSDark*
![image](https://i.imgur.com/gXqefEW.png)
*Banana*
![image](https://i.imgur.com/sKjSb9O.png)
*Winblue*
![image](https://i.imgur.com/qFwDZWj.png)
*Monochrome*
![image](https://i.imgur.com/LVPGKke.png)
*Altmonochrome*
![image](https://i.imgur.com/KjSVQMg.png)
*HoneyDawn*
![image](https://i.imgur.com/pW2OSDr.png)
*Pink*
![image](https://i.imgur.com/bqdmOPo.png)
*CharcoalCream*
![image](https://i.imgur.com/YKWtU5i.png)
*CrimsomBlush*
![image](https://i.imgur.com/evaldRJ.png)
*DeepTeal*
![image](https://i.imgur.com/1yBbaNX.png)
*OceanSand*
![image](https://i.imgur.com/GnqSLmN.png)
*IndigoMist*
![image](https://i.imgur.com/77dsN6g.png)
*SpruceMist*
![image](https://i.imgur.com/3ZjPKWy.png)
*PlumCream*
![image](https://i.imgur.com/vH5eTqi.png)
*ForestSage*
![image](https://i.imgur.com/0vqcJfm.png)
*PineFrost*
![image](https://i.imgur.com/nodjpJE.png)
*You can paste your custom folder path eg(E:Music) to open your Music*
![image](https://i.imgur.com/Eyx3ht4.png)
*You can apply a your custom color code eg(#177DDC)*
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- appearanceTheme: liquidGlass
  $name: Appearance theme
  $description: Choose the desktop widget appearance. White transparent keeps the original glass look.
  $options:
  - liquidGlass: Liquid Glass / Tahoe
  - whiteTransparent: White transparent
  - acrylicTranslucent: Acrylic/Translucent
  - jonaOSLight: JonaOS Light mode
  - jonaOSDark: JonaOS Dark mode
  - banana: Banana
  - winblue: Winblue
  - monochrome: Monochrome
  - altmonochrome: Altmonochrome
  - honeyDawn: Honey Dawn
  - pink: Pink
  - charcoalCream: CharcoalCream
  - crimsomBlush: CrimsomBlush
  - deepTeal: DeepTeal
  - oceanSand: OceanSand
  - indigoMist: IndigoMist
  - spruceMist: SpruceMist
  - plumCream: PlumCream
  - forestSage: ForestSage
  - pineFrost: PineFrost

- volumeWidgetOrientation: horizontal
  $name: Volume widget orientation
  $description: Choose whether the Volume widget is horizontal or vertical.
  $options:
  - horizontal: Horizontal
  - vertical: Vertical
- batteryWidgetOrientation: horizontal
  $name: Battery widget orientation
  $description: Choose whether the Battery widget is horizontal or vertical.
  $options:
  - horizontal: Horizontal
  - vertical: Vertical
- searchWidgetOrientation: horizontal
  $name: Windows Search widget orientation
  $description: Choose whether the Windows Search widget is horizontal or vertical.
  $options:
  - horizontal: Horizontal
  - vertical: Vertical
- personalizationWidgetOrientation: horizontal
  $name: Personalization widget orientation
  $description: Choose whether the Personalization widget is horizontal or vertical.
  $options:
  - horizontal: Horizontal
  - vertical: Vertical
- materialCustomColor: ""
  $name: Liquid Glass / Tahoe custom color
  $description: Optional custom color for Liquid Glass / Tahoe accents, icons, sliders, and controls. Use #RRGGBB, RRGGBB, 0xRRGGBB, or #AARRGGBB. This setting applies only to Liquid Glass / Tahoe. Acrylic keeps its built-in colors.
- musicFolderPath: ""
  $name: Music folder path
  $description: Paste a folder path here (e.g. D:\Music) to open that folder when you click the Music widget. Leave empty to open the default Music folder.
*/
// ==/WindhawkModSettings==

#include <windhawk_api.h>

#include <windows.h>
#include <windowsx.h>
#include <dwmapi.h>
#include <gdiplus.h>
#include <endpointvolume.h>
#include <mmdeviceapi.h>
#include <shlobj.h>
#include <shellapi.h>

#include <algorithm>
#include <cmath>
#include <cwchar>

using namespace Gdiplus;

namespace {

constexpr int kDesignW = 390;
constexpr int kDesignH = 620;
constexpr UINT_PTR kTickTimer = 1;
constexpr DWORD kTimerMs = 1000;
constexpr UINT kSettingsChangedMessage = WM_APP + 1;
constexpr UINT kVolumeChangedMessage   = WM_APP + 2;
constexpr UINT kAudioDeviceChangedMessage = WM_APP + 3;
constexpr wchar_t kClassName[] = L"WindhawkDesktopGlassWidgets";

enum WidgetId {
    WidgetCalendar,
    WidgetClock,
    WidgetMusic,
    WidgetVolume,
    WidgetBattery,
    WidgetSearch,
    WidgetPersonalization,
    WidgetWifi,
    WidgetHotspot,
    WidgetBluetooth,
    WidgetAccessibility,
    WidgetCount,
};

enum class AppearanceTheme {
    LiquidGlass,
    WhiteTransparent,
    AcrylicTranslucent,
    JonaOSLight,
    JonaOSDark,
    Banana,
    Winblue,
    Monochrome,
    Altmonochrome,
    HoneyDawn,
    Pink,
    CharcoalCream,
    CrimsomBlush,
    DeepTeal,
    OceanSand,
    IndigoMist,
    SpruceMist,
    PlumCream,
    ForestSage,
    PineFrost,


};
AppearanceTheme g_appearanceTheme = AppearanceTheme::JonaOSLight;

struct ThemePalette {
    AppearanceTheme theme;
    const wchar_t* settingValue;
    Color header;
    Color body;
    Color accent;
    Color textOnHeader;
    Color textOnBody;
    Color textOnAccent;
};

constexpr BYTE kSolid = 255;

ThemePalette g_palettes[] = {
    {AppearanceTheme::AcrylicTranslucent, L"acrylicTranslucent", Color(215, 0x00, 0x78, 0xD7), Color(120, 0x20, 0x22, 0x28), Color(150, 0x1A, 0x1C, 0x22), Color(kSolid, 0xFF, 0xFF, 0xFF), Color(kSolid, 0xF5, 0xF7, 0xFA), Color(kSolid, 0xF0, 0xF4, 0xF8)},
    {AppearanceTheme::Banana,      L"banana",      Color(kSolid, 0x11, 0x12, 0x0D), Color(kSolid, 0xFB, 0xBC, 0x04), Color(kSolid, 0xFB, 0xBC, 0x04), Color(kSolid, 0xFF, 0xFB, 0xF4), Color(kSolid, 0x11, 0x12, 0x0D), Color(kSolid, 0xFF, 0xFB, 0xF4)},
    {AppearanceTheme::Winblue, L"winblue", Color(kSolid, 0xAC, 0xC8, 0xDE), Color(kSolid, 0x18, 0x74, 0xFD), Color(kSolid, 0x18, 0x74, 0xFD), Color(kSolid, 0x18, 0x74, 0xFD), Color(kSolid, 0xFE, 0xFE, 0xFE), Color(kSolid, 0xFE, 0xFE, 0xFE)},
    {AppearanceTheme::Monochrome, L"monochrome", Color(kSolid, 0x04, 0x04, 0x08), Color(kSolid, 0xFE, 0xFE, 0xFE), Color(kSolid, 0xFE, 0xFE, 0xFE), Color(kSolid, 0xAA, 0xAD, 0xB1), Color(kSolid, 0x04, 0x04, 0x08), Color(kSolid, 0x04, 0x04, 0x08)},
    {AppearanceTheme::Altmonochrome, L"altmonochrome", Color(kSolid, 0xFE, 0xFE, 0xFE), Color(kSolid, 0x04, 0x04, 0x08), Color(kSolid, 0x04, 0x04, 0x08), Color(kSolid, 0x04, 0x04, 0x08), Color(kSolid, 0xFE, 0xFE, 0xFE), Color(kSolid, 0xFE, 0xFE, 0xFE)},
    {AppearanceTheme::HoneyDawn,   L"honeyDawn",   Color(kSolid, 0xFA, 0xD7, 0x8B), Color(kSolid, 0xFF, 0xF7, 0xE8), Color(kSolid, 0xD8, 0x96, 0x45), Color(kSolid, 0x4A, 0x2D, 0x0D), Color(kSolid, 0x42, 0x30, 0x1A), Color(kSolid, 0xFF, 0xFB, 0xEF)},
    {AppearanceTheme::Pink,         L"pink",         Color(kSolid, 0xB8, 0x1E, 0x68), Color(kSolid, 0xFF, 0xEB, 0xF5), Color(kSolid, 0xF4, 0x79, 0xB8), Color(kSolid, 0xFF, 0xF7, 0xFC), Color(kSolid, 0x43, 0x12, 0x2B), Color(kSolid, 0xFF, 0xFB, 0xFE)},
    {AppearanceTheme::CharcoalCream, L"charcoalCream", Color(kSolid, 0x11, 0x11, 0x11), Color(kSolid, 0xF5, 0xEB, 0xDD), Color(kSolid, 0xF5, 0xEB, 0xDD), Color(kSolid, 0xF5, 0xEB, 0xDD), Color(kSolid, 0x11, 0x11, 0x11), Color(kSolid, 0x11, 0x11, 0x11)},
    {AppearanceTheme::CrimsomBlush,  L"crimsomBlush",  Color(kSolid, 0xFD, 0x18, 0x43), Color(kSolid, 0xFF, 0xF9, 0xFA), Color(kSolid, 0xFF, 0xF9, 0xFA), Color(kSolid, 0xFF, 0xF9, 0xFA), Color(kSolid, 0xFD, 0x18, 0x43), Color(kSolid, 0xFD, 0x18, 0x43)},
    {AppearanceTheme::DeepTeal,      L"deepTeal",      Color(kSolid, 0x00, 0x47, 0x41), Color(kSolid, 0xF0, 0xED, 0xE4), Color(kSolid, 0xF0, 0xED, 0xE4), Color(kSolid, 0xF0, 0xED, 0xE4), Color(kSolid, 0x00, 0x47, 0x41), Color(kSolid, 0x00, 0x47, 0x41)},
    {AppearanceTheme::OceanSand,     L"oceanSand",     Color(kSolid, 0x00, 0x41, 0x6A), Color(kSolid, 0xF0, 0xEA, 0xD6), Color(kSolid, 0xF0, 0xEA, 0xD6), Color(kSolid, 0xF0, 0xEA, 0xD6), Color(kSolid, 0x00, 0x41, 0x6A), Color(kSolid, 0x00, 0x41, 0x6A)},
    {AppearanceTheme::IndigoMist,    L"indigoMist",    Color(kSolid, 0x27, 0x18, 0x7E), Color(kSolid, 0xF7, 0xF7, 0xFF), Color(kSolid, 0xF7, 0xF7, 0xFF), Color(kSolid, 0xF7, 0xF7, 0xFF), Color(kSolid, 0x27, 0x18, 0x7E), Color(kSolid, 0x27, 0x18, 0x7E)},
    {AppearanceTheme::SpruceMist,    L"spruceMist",    Color(kSolid, 0x00, 0x46, 0x43), Color(kSolid, 0xF0, 0xED, 0xE5), Color(kSolid, 0xF0, 0xED, 0xE5), Color(kSolid, 0xF0, 0xED, 0xE5), Color(kSolid, 0x00, 0x46, 0x43), Color(kSolid, 0x00, 0x46, 0x43)},
    {AppearanceTheme::PlumCream,     L"plumCream",     Color(kSolid, 0x38, 0x19, 0x32), Color(kSolid, 0xFF, 0xF3, 0xE6), Color(kSolid, 0xFF, 0xF3, 0xE6), Color(kSolid, 0xFF, 0xF3, 0xE6), Color(kSolid, 0x38, 0x19, 0x32), Color(kSolid, 0x38, 0x19, 0x32)},
    {AppearanceTheme::ForestSage,    L"forestSage",    Color(kSolid, 0x1A, 0x25, 0x17), Color(kSolid, 0xAC, 0xC8, 0xA2), Color(kSolid, 0xAC, 0xC8, 0xA2), Color(kSolid, 0xAC, 0xC8, 0xA2), Color(kSolid, 0x1A, 0x25, 0x17), Color(kSolid, 0x1A, 0x25, 0x17)},
    {AppearanceTheme::PineFrost,     L"pineFrost",     Color(kSolid, 0x03, 0x32, 0x2E), Color(kSolid, 0xF4, 0xF4, 0xF4), Color(kSolid, 0xF4, 0xF4, 0xF4), Color(kSolid, 0xF4, 0xF4, 0xF4), Color(kSolid, 0x03, 0x32, 0x2E), Color(kSolid, 0x03, 0x32, 0x2E)},
};


// ── JonaOS per-widget colour tables ──────────────────────────────────────────
struct JonaOSPalette {
    // Calendar
    Color calHeader;
    Color calHeaderText;
    Color calBody;
    Color calBodyText;
    // Clock
    Color clockFrame;
    Color clockFace;
    Color clockHands;
    Color clockTicks;
    Color clockNums;
    // Music
    Color musicFrame;
    Color musicBars[7];
    Color musicPlayFrame;
    Color musicPlayGlyph;
    // Volume
    Color volumeFrame;
    Color volumeText;
    Color volumeSlider;
    // Battery
    Color batteryFrame;
    Color batteryText;
    Color batterySlider;
    // Search
    Color searchFrame;
    Color searchText;
    Color searchIcon;
    // Personalization
    Color personalizationFrame;
    Color personalizationText;
    Color personalizationIcon;
    // WiFi
    Color wifiFrame;
    Color wifiText;
    Color wifiIcon;
    // Bluetooth
    Color bluetoothFrame;
    Color bluetoothText;
    Color bluetoothIcon;
    // Hotspot
    Color hotspotFrame;
    Color hotspotText;
    Color hotspotIcon;
    // Accessibility
    Color accessibilityFrame;
    Color accessibilityText;
    Color accessibilityIcon;
};

static const JonaOSPalette g_jonaOSLight = {
    // Calendar
    Color(255,0xFF,0x3B,0x30), // calHeader      — Red
    Color(255,0xE5,0xE5,0xEA), // calHeaderText  — Light grey
    Color(255,0xE5,0xE5,0xEA), // calBody        — Light grey
    Color(255,0x1C,0x1C,0x1E), // calBodyText    — Dark grey
    // Clock
    Color(255,0x1C,0x1C,0x1E), // clockFrame     — Dark grey
    Color(255,0xE5,0xE5,0xEA), // clockFace      — Light grey
    Color(255,0x1C,0x1C,0x1E), // clockHands     — Dark grey
    Color(255,0xE5,0xE5,0xEA), // clockTicks     — Light grey
    Color(255,0x1C,0x1C,0x1E), // clockNums      — Dark grey
    // Music
    Color(255,0xE5,0xE5,0xEA), // musicFrame     — Light grey
    {
        Color(255,0xFF,0x3B,0x30), // bar0 Red
        Color(255,0xFF,0x95,0x00), // bar1 Orange
        Color(255,0xFF,0xCC,0x00), // bar2 Yellow
        Color(255,0x34,0xC7,0x59), // bar3 Green
        Color(255,0x00,0x7A,0xFF), // bar4 Blue
        Color(255,0x4B,0x00,0x82), // bar5 Indigo
        Color(255,0x7F,0x00,0xFF), // bar6 Violet
    },
    Color(255,0xFF,0x3B,0x30), // musicPlayFrame — Red
    Color(255,0xE5,0xE5,0xEA), // musicPlayGlyph — Light grey
    // Volume
    Color(255,0xE5,0xE5,0xEA), // volumeFrame
    Color(255,0x1C,0x1C,0x1E), // volumeText
    Color(255,0x00,0x7A,0xFF), // volumeSlider   — Blue
    // Battery
    Color(255,0xE5,0xE5,0xEA), // batteryFrame
    Color(255,0x1C,0x1C,0x1E), // batteryText
    Color(255,0x34,0xC7,0x59), // batterySlider  — Green
    // Search
    Color(255,0xE5,0xE5,0xEA), // searchFrame
    Color(255,0x1C,0x1C,0x1E), // searchText
    Color(255,0x34,0xC7,0x59), // searchIcon     — Green
    // Personalization
    Color(255,0xE5,0xE5,0xEA), // personalizationFrame
    Color(255,0x1C,0x1C,0x1E), // personalizationText
    Color(255,0x00,0x7A,0xFF), // personalizationIcon — Blue
    // WiFi
    Color(255,0xE5,0xE5,0xEA), // wifiFrame
    Color(255,0xFF,0x3B,0x30), // wifiText       — Red
    Color(255,0xFF,0x3B,0x30), // wifiIcon       — Red
    // Bluetooth
    Color(255,0xE5,0xE5,0xEA), // bluetoothFrame
    Color(255,0xFF,0x95,0x00), // bluetoothText  — Orange
    Color(255,0xFF,0x95,0x00), // bluetoothIcon  — Orange
    // Hotspot
    Color(255,0xE5,0xE5,0xEA), // hotspotFrame
    Color(255,0x34,0xC7,0x59), // hotspotText    — Green
    Color(255,0x34,0xC7,0x59), // hotspotIcon    — Green
    // Accessibility
    Color(255,0xE5,0xE5,0xEA), // accessibilityFrame
    Color(255,0x00,0x7A,0xFF), // accessibilityText  — Blue
    Color(255,0x00,0x7A,0xFF), // accessibilityIcon  — Blue
};

static const JonaOSPalette g_jonaOSDark = {
    // Calendar
    Color(255,0x1C,0x1C,0x1E), // calHeader      — Dark grey
    Color(255,0xFF,0x3B,0x30), // calHeaderText  — Red
    Color(255,0x1C,0x1C,0x1E), // calBody        — Dark grey
    Color(255,0xE5,0xE5,0xEA), // calBodyText    — Light grey
    // Clock
    Color(255,0x1C,0x1C,0x1E), // clockFrame     — Dark grey
    Color(255,0x1C,0x1C,0x1E), // clockFace      — Dark grey
    Color(255,0xE5,0xE5,0xEA), // clockHands     — Light grey
    Color(255,0xE5,0xE5,0xEA), // clockTicks     — Light grey
    Color(255,0xE5,0xE5,0xEA), // clockNums      — Light grey
    // Music
    Color(255,0x1C,0x1C,0x1E), // musicFrame     — Dark grey
    {
        Color(255,0xA8,0x56,0xB2), // bar0 Purple
        Color(255,0xA8,0x56,0xB2), // bar1 Purple
        Color(255,0xA8,0x56,0xB2), // bar2 Purple
        Color(255,0xA8,0x56,0xB2), // bar3 Purple
        Color(255,0xA8,0x56,0xB2), // bar4 Purple
        Color(255,0xA8,0x56,0xB2), // bar5 Purple
        Color(255,0xA8,0x56,0xB2), // bar6 Purple
    },
    Color(255,0x34,0xC7,0x59), // musicPlayFrame — Green
    Color(255,0x1C,0x1C,0x1E), // musicPlayGlyph — Dark grey
    // Volume
    Color(255,0x1C,0x1C,0x1E), // volumeFrame
    Color(255,0xE5,0xE5,0xEA), // volumeText
    Color(255,0x00,0x7A,0xFF), // volumeSlider   — Blue
    // Battery
    Color(255,0x1C,0x1C,0x1E), // batteryFrame
    Color(255,0xE5,0xE5,0xEA), // batteryText
    Color(255,0x34,0xC7,0x59), // batterySlider  — Green
    // Search
    Color(255,0x1C,0x1C,0x1E), // searchFrame
    Color(255,0xE5,0xE5,0xEA), // searchText
    Color(255,0x34,0xC7,0x59), // searchIcon     — Green
    // Personalization
    Color(255,0x1C,0x1C,0x1E), // personalizationFrame
    Color(255,0xE5,0xE5,0xEA), // personalizationText
    Color(255,0x00,0x7A,0xFF), // personalizationIcon — Blue
    // WiFi
    Color(255,0x1C,0x1C,0x1E), // wifiFrame
    Color(255,0xE5,0xE5,0xEA), // wifiText
    Color(255,0xFF,0x3B,0x30), // wifiIcon       — Red
    // Bluetooth
    Color(255,0x1C,0x1C,0x1E), // bluetoothFrame
    Color(255,0xE5,0xE5,0xEA), // bluetoothText
    Color(255,0xFF,0x95,0x00), // bluetoothIcon  — Orange
    // Hotspot
    Color(255,0x1C,0x1C,0x1E), // hotspotFrame
    Color(255,0xE5,0xE5,0xEA), // hotspotText
    Color(255,0x34,0xC7,0x59), // hotspotIcon    — Green
    // Accessibility
    Color(255,0x1C,0x1C,0x1E), // accessibilityFrame
    Color(255,0xE5,0xE5,0xEA), // accessibilityText
    Color(255,0x00,0x7A,0xFF), // accessibilityIcon — Blue
};

const JonaOSPalette* CurrentJonaOSPalette() {
    if (g_appearanceTheme == AppearanceTheme::JonaOSLight) return &g_jonaOSLight;
    if (g_appearanceTheme == AppearanceTheme::JonaOSDark)  return &g_jonaOSDark;
    return nullptr;
}

struct Widget {
    int   id;
    RectF rc;
    bool  round;
};

Widget g_widgets[WidgetCount] = {
    {WidgetCalendar,        RectF(0,   0,   350, 180), false},
    {WidgetClock,           RectF(0,   190, 165, 170), false},
    {WidgetMusic,           RectF(180, 190, 165, 170), false},
    {WidgetVolume,          RectF(0,   380, 350, 30),  false},
    {WidgetBattery,         RectF(0,   430, 350, 30),  false},
    {WidgetSearch,          RectF(0,   480, 165, 34),  false},
    {WidgetPersonalization, RectF(185, 480, 165, 34),  false},
    {WidgetWifi,            RectF(0,   530, 80,  70),  true},
    {WidgetHotspot,         RectF(90,  530, 80,  70),  true},
    {WidgetBluetooth,       RectF(180, 530, 80,  70),  true},
    {WidgetAccessibility,   RectF(270, 530, 80,  70),  true},
};


// ── Global state ─────────────────────────────────────────────────────────────

HWND g_hwnd = nullptr;
HANDLE g_uiThread = nullptr;
DWORD g_uiThreadId = 0;
volatile bool g_unloading = false;
ULONG_PTR g_gdiplusToken = 0;
float g_scale = 1.0f;
constexpr float kWidgetSizeScale = 0.75f;
constexpr float kSmallButtonHeight = 34.0f;
constexpr float kSmallButtonFontSize = 15.0f;
constexpr float kRoundShortcutWidth = 80.0f;
constexpr float kRoundShortcutHeight = 70.0f;
constexpr float kRoundShortcutFontSize = 12.0f;

// Vertical widget dimensions.
// 173.333 logical px * 0.75 = 130 screen px.
// 73.333 logical px * 0.75 = 55 screen px.
constexpr float kVerticalWidgetHeight = 173.333333f;
constexpr float kSearchVerticalWidth = 73.333333f;
int g_dragWidget = -1;
PointF g_dragDelta;
PointF g_dragStart;
bool g_dragMoved = false;
bool g_draggingSlider = false;
float g_volumeLevel = 1.0f;
float g_batteryLevel = 1.0f;
bool g_onBatteryCapable = false;
AppearanceTheme _appearanceTheme = AppearanceTheme::LiquidGlass;
bool g_volumeWidgetVertical = false;
bool g_batteryWidgetVertical = false;
bool g_searchWidgetVertical = false;
bool g_personalizationWidgetVertical = false;
BYTE g_materialAccentR = 0xFE;
BYTE g_materialAccentG = 0xFE;
BYTE g_materialAccentB = 0xFE;
wchar_t g_musicFolderPath[1024] = L"";
int g_lastRenderedMinute = -1;

IMMDeviceEnumerator*          g_audioEnumerator = nullptr;
IMMDevice*                    g_audioDevice = nullptr;
IAudioEndpointVolume*         g_endpointVolume = nullptr;
IAudioEndpointVolumeCallback* g_volumeCallback = nullptr;
IMMNotificationClient*        g_audioNotificationClient = nullptr;

struct RenderSurface {
    HDC      memDc      = nullptr;
    HBITMAP  bitmap     = nullptr;
    HGDIOBJ  oldBitmap  = nullptr;
    int      width      = 0;
    int      height     = 0;
};

RenderSurface g_renderSurface;
RECT g_lastWindowRect = {};
bool g_hasLastWindowRect = false;

// ── Math helpers ─────────────────────────────────────────────────────────────

float Clamp(float value, float minValue, float maxValue) {
    return std::max(minValue, std::min(value, maxValue));
}

float RenderScale() {
    return g_scale * kWidgetSizeScale;
}

int ScaledDesignWidth(float scale) {
    return static_cast<int>(std::ceil(kDesignW * scale));
}

int ScaledDesignHeight(float scale) {
    return static_cast<int>(std::ceil(kDesignH * scale));
}

int ScaledMargin(float scale) {
    return static_cast<int>(20 * scale);
}

RECT DesktopWorkArea() {
    RECT workArea = {};
    SystemParametersInfoW(SPI_GETWORKAREA, 0, &workArea, 0);
    return workArea;
}

SIZE DesktopWorkAreaSize() {
    RECT workArea = DesktopWorkArea();
    return {workArea.right - workArea.left, workArea.bottom - workArea.top};
}

RectF LogicalDesktopBounds() {
    SIZE size = DesktopWorkAreaSize();
    const float renderScale = RenderScale();
    return RectF(0, 0, size.cx / renderScale, size.cy / renderScale);
}

RectF Deflate(RectF rc, float x, float y) {
    rc.X += x; rc.Y += y;
    rc.Width -= x * 2; rc.Height -= y * 2;
    return rc;
}

PointF LogicalPointFromLParam(LPARAM lParam) {
    const float renderScale = RenderScale();
    return PointF(GET_X_LPARAM(lParam) / renderScale, GET_Y_LPARAM(lParam) / renderScale);
}

PointF LogicalPointFromScreen(POINT pt) {
    RECT workArea = DesktopWorkArea();
    const float renderScale = RenderScale();
    return PointF((pt.x - workArea.left) / renderScale, (pt.y - workArea.top) / renderScale);
}

bool PtInRectF(const RectF& rc, PointF pt) {
    return pt.X >= rc.X && pt.X <= rc.X + rc.Width &&
           pt.Y >= rc.Y && pt.Y <= rc.Y + rc.Height;
}

// ── Theme helpers ─────────────────────────────────────────────────────────────

const ThemePalette* CurrentTheme() {
    if (g_appearanceTheme == AppearanceTheme::WhiteTransparent ||
        g_appearanceTheme == AppearanceTheme::LiquidGlass) {
        return nullptr;
    }
    for (const ThemePalette& palette : g_palettes) {
        if (palette.theme == g_appearanceTheme) {
            return &palette;
        }
    }
    return nullptr;
}

bool IsLiquidGlassTheme() {
    return g_appearanceTheme == AppearanceTheme::LiquidGlass;
}

Color LiquidGlassTextColor(BYTE alpha = 252) {
    return Color(alpha, 255, 255, 255);
}

Color MaterialAccentColor(BYTE alpha = 255) {
    return Color(alpha, g_materialAccentR, g_materialAccentG, g_materialAccentB);
}

Color MaterialThemeHeaderColor(const ThemePalette* theme) {
    // Custom material color is intentionally NOT used by Acrylic.
    // Liquid Glass has no ThemePalette, so it falls back to the custom
    // Liquid Glass accent color below.
    return theme ? theme->header : MaterialAccentColor();
}

Color MaterialIconColor(const ThemePalette* theme, bool liquidGlass, Color fallback) {
    // The custom material color is exclusive to Liquid Glass / Tahoe.
    if (liquidGlass) {
        return MaterialAccentColor();
    }
    return theme ? theme->textOnAccent : fallback;
}

// ── Settings helpers ──────────────────────────────────────────────────────────

void CopyTrimmedPathSetting(PCWSTR value, wchar_t* outBuffer, size_t outBufferSize) {
    outBuffer[0] = L'\0';
    if (!value || outBufferSize == 0) return;
    const wchar_t* start = value;
    while (*start == L' ' || *start == L'\t' || *start == L'"' || *start == L'\'') ++start;
    const wchar_t* end = start + wcslen(start);
    while (end > start && (end[-1] == L' ' || end[-1] == L'\t' || end[-1] == L'"' || end[-1] == L'\'')) --end;
    size_t length = static_cast<size_t>(end - start);
    if (length >= outBufferSize) length = outBufferSize - 1;
    wmemcpy(outBuffer, start, length);
    outBuffer[length] = L'\0';
}

int HexDigitValue(wchar_t ch) {
    if (ch >= L'0' && ch <= L'9') return ch - L'0';
    if (ch >= L'a' && ch <= L'f') return ch - L'a' + 10;
    if (ch >= L'A' && ch <= L'F') return ch - L'A' + 10;
    return -1;
}

bool IsSettingTrimChar(wchar_t ch) {
    return ch == L' ' || ch == L'\t' || ch == L'\r' || ch == L'\n' ||
           ch == L'"' || ch == L'\'';
}

bool ParseMaterialAccentColor(PCWSTR value, BYTE* outR, BYTE* outG, BYTE* outB) {
    if (!value) return false;
    const wchar_t* start = value;
    while (IsSettingTrimChar(*start)) ++start;
    if (*start == L'#') ++start;
    else if (start[0] == L'0' && (start[1] == L'x' || start[1] == L'X')) start += 2;
    wchar_t digits[9] = {};
    int digitCount = 0;
    const wchar_t* cursor = start;
    while (*cursor && HexDigitValue(*cursor) >= 0) {
        if (digitCount >= 8) return false;
        digits[digitCount++] = *cursor++;
    }
    while (IsSettingTrimChar(*cursor)) ++cursor;
    if (*cursor != L'\0' || (digitCount != 6 && digitCount != 8)) return false;
    const int rgbOffset = digitCount == 8 ? 2 : 0;
    int r1=HexDigitValue(digits[rgbOffset]),   r2=HexDigitValue(digits[rgbOffset+1]);
    int g1=HexDigitValue(digits[rgbOffset+2]), g2=HexDigitValue(digits[rgbOffset+3]);
    int b1=HexDigitValue(digits[rgbOffset+4]), b2=HexDigitValue(digits[rgbOffset+5]);
    if (r1<0||r2<0||g1<0||g2<0||b1<0||b2<0) return false;
    *outR = static_cast<BYTE>((r1<<4)|r2);
    *outG = static_cast<BYTE>((g1<<4)|g2);
    *outB = static_cast<BYTE>((b1<<4)|b2);
    return true;
}

void ResetMaterialAccentColor() {
    g_materialAccentR = 0xFE;
    g_materialAccentG = 0xFE;
    g_materialAccentB = 0xFE;
}

bool ReadVerticalOrientationSetting(const wchar_t* name) {
    bool vertical = false;
    PCWSTR value = Wh_GetStringSetting(name);
    if (value) {
        vertical = wcscmp(value, L"vertical") == 0;
        Wh_FreeStringSetting(value);
    }
    return vertical;
}

bool IsWidgetVertical(int widgetId) {
    switch (widgetId) {
        case WidgetVolume:           return g_volumeWidgetVertical;
        case WidgetBattery:          return g_batteryWidgetVertical;
        case WidgetSearch:           return g_searchWidgetVertical;
        case WidgetPersonalization:  return g_personalizationWidgetVertical;
    }
    return false;
}

// ── FIX A: Dynamic calendar height ───────────────────────────────────────────
// CalendarRowCount() returns 5 or 6 based on what day-of-week the 1st falls on
// and how many days are in the month.  DynamicCalendarHeight() maps that to a
// pixel height.  UpdateCalendarHeight() is the ONLY calendar-related function
// called from Render() — it adjusts ONLY the Height field and never touches
// any widget's X/Y, so dragged positions are always preserved.

int CalendarRowCount(int year, int month) {
    SYSTEMTIME first = {};
    first.wYear  = static_cast<WORD>(year);
    first.wMonth = static_cast<WORD>(month);
    first.wDay   = 1;
    FILETIME ft = {};
    SystemTimeToFileTime(&first, &ft);
    FileTimeToSystemTime(&ft, &first);
    int firstDow = first.wDayOfWeek;
    int daysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    bool leap = (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
    if (leap) daysInMonth[1] = 29;
    int count = daysInMonth[month - 1];
    int lastCell = firstDow + count - 1;
    return (lastCell / 7) >= 5 ? 6 : 5;
}

float DynamicCalendarHeight() {
    SYSTEMTIME st;
    GetLocalTime(&st);
    return CalendarRowCount(st.wYear, st.wMonth) >= 6 ? 200.0f : 180.0f;
}

// Called every Render() tick — adjusts ONLY Height, never X/Y.
void UpdateCalendarHeight() {
    g_widgets[WidgetCalendar].rc.Height = DynamicCalendarHeight();
}

// ── Layout ────────────────────────────────────────────────────────────────────
// ApplyDefaultWidgetLayout() sets every widget's position from scratch.
// It is called ONLY at WM_CREATE, WM_DISPLAYCHANGE, WM_DPICHANGED, and
// kSettingsChangedMessage — never from Render() — so dragged positions
// survive the one-second timer ticks.

void ApplyDefaultWidgetLayout() {
    const float calH    = DynamicCalendarHeight();
    const float kCalGap = 10.0f;

    g_widgets[WidgetCalendar].rc = RectF(0, 0, 350, calH);

    const float clockY = calH + kCalGap;
    g_widgets[WidgetClock].rc = RectF(0,   clockY, 165, 170);
    g_widgets[WidgetMusic].rc = RectF(180, clockY, 165, 170);

    float y = clockY + 170.0f + 10.0f;

    if (!g_volumeWidgetVertical && !g_batteryWidgetVertical) {
        g_widgets[WidgetVolume].rc  = RectF(0, y, 350, 30);
        y += 50.0f;
        g_widgets[WidgetBattery].rc = RectF(0, y, 350, 30);
        y += 50.0f;
    } else if (g_volumeWidgetVertical && g_batteryWidgetVertical) {
        g_widgets[WidgetVolume].rc  = RectF(0,  y, 70, kVerticalWidgetHeight);
        g_widgets[WidgetBattery].rc = RectF(90, y, 70, kVerticalWidgetHeight);
        y += kVerticalWidgetHeight + 20.0f;
    } else if (g_volumeWidgetVertical) {
        g_widgets[WidgetVolume].rc  = RectF(0,  y, 70,  kVerticalWidgetHeight);
        g_widgets[WidgetBattery].rc = RectF(90, y, 260, 30);
        y += kVerticalWidgetHeight + 20.0f;
    } else {
        g_widgets[WidgetVolume].rc  = RectF(0,   y, 260, 30);
        g_widgets[WidgetBattery].rc = RectF(280, y, 70,  kVerticalWidgetHeight);
        y += kVerticalWidgetHeight + 20.0f;
    }

    if (!g_searchWidgetVertical && !g_personalizationWidgetVertical) {
        g_widgets[WidgetSearch].rc          = RectF(0,   y, 165, kSmallButtonHeight);
        g_widgets[WidgetPersonalization].rc = RectF(185, y, 165, kSmallButtonHeight);
        y += 50.0f;
    } else if (g_searchWidgetVertical && g_personalizationWidgetVertical) {
        g_widgets[WidgetSearch].rc          = RectF(0,   y, kSearchVerticalWidth, kVerticalWidgetHeight);
        g_widgets[WidgetPersonalization].rc = RectF(93.333333f, y, kSearchVerticalWidth, kVerticalWidgetHeight);
        y += kVerticalWidgetHeight + 20.0f;
    } else if (g_searchWidgetVertical) {
        g_widgets[WidgetSearch].rc          = RectF(0,   y, kSearchVerticalWidth, kVerticalWidgetHeight);
        g_widgets[WidgetPersonalization].rc = RectF(93.333333f, y, 256.666667f, kSmallButtonHeight);
        y += kVerticalWidgetHeight + 20.0f;
    } else {
        g_widgets[WidgetSearch].rc          = RectF(0,   y, 256.666667f, kSmallButtonHeight);
        g_widgets[WidgetPersonalization].rc = RectF(276.666667f, y, kSearchVerticalWidth, kVerticalWidgetHeight);
        y += kVerticalWidgetHeight + 20.0f;
    }

    g_widgets[WidgetWifi].rc          = RectF(0,   y, kRoundShortcutWidth, kRoundShortcutHeight);
    g_widgets[WidgetHotspot].rc       = RectF(90,  y, kRoundShortcutWidth, kRoundShortcutHeight);
    g_widgets[WidgetBluetooth].rc     = RectF(180, y, kRoundShortcutWidth, kRoundShortcutHeight);
    g_widgets[WidgetAccessibility].rc = RectF(270, y, kRoundShortcutWidth, kRoundShortcutHeight);

    RectF desktop = LogicalDesktopBounds();
    const float rightOffset = std::max(0.0f, desktop.Width - static_cast<float>(kDesignW));
    for (Widget& widget : g_widgets) {
        widget.rc.X += rightOffset;
    }
}

RectF RectForOrientation(const RectF& current, bool vertical,
                          float horizontalWidth, float verticalWidth,
                          float horizontalHeight = 30.0f) {
    return vertical
        ? RectF(current.X, current.Y, verticalWidth, kVerticalWidgetHeight)
        : RectF(current.X, current.Y, horizontalWidth, horizontalHeight);
}

void ClampWidgetToDesktop(Widget& widget) {
    RectF desktop = LogicalDesktopBounds();
    const float maxX = std::max(0.0f, desktop.Width  - widget.rc.Width);
    const float maxY = std::max(0.0f, desktop.Height - widget.rc.Height);
    widget.rc.X = Clamp(widget.rc.X, 0.0f, maxX);
    widget.rc.Y = Clamp(widget.rc.Y, 0.0f, maxY);
}

void ApplyOrientationResizeIfNeeded(bool oldVolumeVertical,
                                    bool oldBatteryVertical,
                                    bool oldSearchVertical,
                                    bool oldPersonalizationVertical) {
    if (oldVolumeVertical != g_volumeWidgetVertical) {
        g_widgets[WidgetVolume].rc = RectForOrientation(
            g_widgets[WidgetVolume].rc, g_volumeWidgetVertical, 350.0f, 70.0f);
        ClampWidgetToDesktop(g_widgets[WidgetVolume]);
    }
    if (oldBatteryVertical != g_batteryWidgetVertical) {
        g_widgets[WidgetBattery].rc = RectForOrientation(
            g_widgets[WidgetBattery].rc, g_batteryWidgetVertical, 350.0f, 70.0f);
        ClampWidgetToDesktop(g_widgets[WidgetBattery]);
    }
    if (oldSearchVertical != g_searchWidgetVertical) {
        g_widgets[WidgetSearch].rc = RectForOrientation(
            g_widgets[WidgetSearch].rc, g_searchWidgetVertical, 165.0f, kSearchVerticalWidth,
            kSmallButtonHeight);
        ClampWidgetToDesktop(g_widgets[WidgetSearch]);
    }
    if (oldPersonalizationVertical != g_personalizationWidgetVertical) {
        g_widgets[WidgetPersonalization].rc = RectForOrientation(
            g_widgets[WidgetPersonalization].rc, g_personalizationWidgetVertical, 165.0f, kSearchVerticalWidth,
            kSmallButtonHeight);
        ClampWidgetToDesktop(g_widgets[WidgetPersonalization]);
    }
}

// ── FIX B: LoadSettings — WhiteTransparent theme bug fixed ───────────────────
// Previously the "whiteTransparent" setting value was not explicitly handled,
// so it fell through to the g_palettes loop which does not contain it, and
// g_appearanceTheme silently stayed as LiquidGlass.  The else-if branch below
// corrects that.

void LoadSettings() {
    g_appearanceTheme = AppearanceTheme::LiquidGlass;
    g_volumeWidgetVertical = false;
    g_batteryWidgetVertical = false;
    g_searchWidgetVertical = false;
    g_personalizationWidgetVertical = false;
    ResetMaterialAccentColor();
    g_musicFolderPath[0] = L'\0';

    PCWSTR theme = Wh_GetStringSetting(L"appearanceTheme");
    if (theme) {
        if (wcscmp(theme, L"liquidGlass") == 0) {
            g_appearanceTheme = AppearanceTheme::LiquidGlass;
        } else if (wcscmp(theme, L"whiteTransparent") == 0) {
            // FIX B: was missing — caused White Transparent to silently stay
            // as LiquidGlass because it is not in the g_palettes array.
            g_appearanceTheme = AppearanceTheme::WhiteTransparent;
        } else if (wcscmp(theme, L"jonaOSLight") == 0) {
            g_appearanceTheme = AppearanceTheme::JonaOSLight;
        } else if (wcscmp(theme, L"jonaOSDark") == 0) {
            g_appearanceTheme = AppearanceTheme::JonaOSDark;
        } else {
            for (const ThemePalette& palette : g_palettes) {
                if (wcscmp(theme, palette.settingValue) == 0) {
                    g_appearanceTheme = palette.theme;
                    break;
                }
            }
        }
        Wh_FreeStringSetting(theme);
    }

    g_volumeWidgetVertical        = ReadVerticalOrientationSetting(L"volumeWidgetOrientation");
    g_batteryWidgetVertical       = ReadVerticalOrientationSetting(L"batteryWidgetOrientation");
    g_searchWidgetVertical        = ReadVerticalOrientationSetting(L"searchWidgetOrientation");
    g_personalizationWidgetVertical = ReadVerticalOrientationSetting(L"personalizationWidgetOrientation");

    PCWSTR materialCustomColor = Wh_GetStringSetting(L"materialCustomColor");
    if (materialCustomColor) {
        BYTE r = 0, g = 0, b = 0;
        if (ParseMaterialAccentColor(materialCustomColor, &r, &g, &b)) {
            g_materialAccentR = r;
            g_materialAccentG = g;
            g_materialAccentB = b;
        }
        Wh_FreeStringSetting(materialCustomColor);
    }

    PCWSTR musicFolderPath = Wh_GetStringSetting(L"musicFolderPath");
    if (musicFolderPath) {
        CopyTrimmedPathSetting(musicFolderPath, g_musicFolderPath, ARRAYSIZE(g_musicFolderPath));
        Wh_FreeStringSetting(musicFolderPath);
    }
}

void SaveWidgetPositions() {
    for (const Widget& widget : g_widgets) {
        wchar_t key[32];
        swprintf_s(key, L"widgetPosition%d", widget.id);
        wchar_t value[64];
        swprintf_s(value, L"%.2f,%.2f", widget.rc.X, widget.rc.Y);
        Wh_SetStringValue(key, value);
    }
}

void LoadWidgetPositions() {
    for (Widget& widget : g_widgets) {
        wchar_t key[32];
        swprintf_s(key, L"widgetPosition%d", widget.id);
        wchar_t value[64];
        size_t read = Wh_GetStringValue(key, value, ARRAYSIZE(value));
        if (read == 0) continue;
        float x, y;
        if (swscanf_s(value, L"%f,%f", &x, &y) == 2) {
            widget.rc.X = x;
            widget.rc.Y = y;
            ClampWidgetToDesktop(widget);
        }
    }
}

bool PtInWidget(const Widget& widget, PointF pt) {
    if (!PtInRectF(widget.rc, pt)) return false;
    if (!widget.round) return true;
    const float cx = widget.rc.X + widget.rc.Width  / 2.0f;
    const float cy = widget.rc.Y + widget.rc.Height / 2.0f;
    const float rx = widget.rc.Width  / 2.0f;
    const float ry = widget.rc.Height / 2.0f;
    const float dx = (pt.X - cx) / rx;
    const float dy = (pt.Y - cy) / ry;
    return dx * dx + dy * dy <= 1.0f;
}

int HitTestWidget(PointF pt) {
    for (int i = WidgetCount - 1; i >= 0; --i) {
        if (PtInWidget(g_widgets[i], pt)) return i;
    }
    return -1;
}

RectF VolumeSliderRect() {
    RectF rc = g_widgets[WidgetVolume].rc;
    if (g_volumeWidgetVertical) {
        return RectF(rc.X + rc.Width / 2.0f - 7.5f, rc.Y + 32.0f, 15, rc.Height - 48.0f);
    }
    return RectF(rc.X + 90, rc.Y + 7.5f, std::max(40.0f, rc.Width - 110.0f), 15);
}

// ── Audio / battery COM helpers (unchanged from original) ─────────────────────

class VolumeEndpointCallback : public IAudioEndpointVolumeCallback {
public:
    ULONG STDMETHODCALLTYPE AddRef() override { return InterlockedIncrement(&m_refCount); }
    ULONG STDMETHODCALLTYPE Release() override {
        ULONG rc = InterlockedDecrement(&m_refCount);
        if (rc == 0) delete this;
        return rc;
    }
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** object) override {
        if (!object) return E_POINTER;
        if (riid == __uuidof(IUnknown) || riid == __uuidof(IAudioEndpointVolumeCallback)) {
            *object = static_cast<IAudioEndpointVolumeCallback*>(this);
            AddRef(); return S_OK;
        }
        *object = nullptr; return E_NOINTERFACE;
    }
    HRESULT STDMETHODCALLTYPE OnNotify(PAUDIO_VOLUME_NOTIFICATION_DATA notify) override {
        if (notify) {
            float level = Clamp(notify->fMasterVolume, 0.0f, 1.0f);
            if (std::fabs(level - g_volumeLevel) > 0.001f) {
                g_volumeLevel = level;
                if (g_hwnd) PostMessageW(g_hwnd, kVolumeChangedMessage, 0, 0);
            }
        }
        return S_OK;
    }
private:
    volatile LONG m_refCount = 1;
};

class AudioNotificationClient : public IMMNotificationClient {
public:
    ULONG STDMETHODCALLTYPE AddRef() override { return InterlockedIncrement(&m_refCount); }
    ULONG STDMETHODCALLTYPE Release() override {
        ULONG rc = InterlockedDecrement(&m_refCount);
        if (rc == 0) delete this;
        return rc;
    }
    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** object) override {
        if (!object) return E_POINTER;
        if (riid == __uuidof(IUnknown) || riid == __uuidof(IMMNotificationClient)) {
            *object = static_cast<IMMNotificationClient*>(this);
            AddRef(); return S_OK;
        }
        *object = nullptr; return E_NOINTERFACE;
    }
    HRESULT STDMETHODCALLTYPE OnDefaultDeviceChanged(EDataFlow flow, ERole role, LPCWSTR) override {
        if (flow == eRender && role == eConsole && g_hwnd)
            PostMessageW(g_hwnd, kAudioDeviceChangedMessage, 0, 0);
        return S_OK;
    }
    HRESULT STDMETHODCALLTYPE OnDeviceAdded(LPCWSTR) override { return S_OK; }
    HRESULT STDMETHODCALLTYPE OnDeviceRemoved(LPCWSTR) override { return S_OK; }
    HRESULT STDMETHODCALLTYPE OnDeviceStateChanged(LPCWSTR, DWORD) override { return S_OK; }
    HRESULT STDMETHODCALLTYPE OnPropertyValueChanged(LPCWSTR, const PROPERTYKEY) override { return S_OK; }
private:
    volatile LONG m_refCount = 1;
};

void ReleaseAudioEndpoint() {
    if (g_endpointVolume) {
        if (g_volumeCallback)
            g_endpointVolume->UnregisterControlChangeNotify(g_volumeCallback);
        g_endpointVolume->Release();
        g_endpointVolume = nullptr;
    }
    if (g_audioDevice) { g_audioDevice->Release(); g_audioDevice = nullptr; }
}

void ShutdownAudio() {
    ReleaseAudioEndpoint();
    if (g_audioEnumerator) {
        if (g_audioNotificationClient)
            g_audioEnumerator->UnregisterEndpointNotificationCallback(g_audioNotificationClient);
        g_audioEnumerator->Release();
        g_audioEnumerator = nullptr;
    }
    if (g_audioNotificationClient) { g_audioNotificationClient->Release(); g_audioNotificationClient = nullptr; }
    if (g_volumeCallback)          { g_volumeCallback->Release();          g_volumeCallback = nullptr; }
}

bool BindAudioEndpoint() {
    ReleaseAudioEndpoint();
    if (!g_audioEnumerator) {
        HRESULT hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL,
                                      __uuidof(IMMDeviceEnumerator),
                                      reinterpret_cast<void**>(&g_audioEnumerator));
        if (FAILED(hr)) return false;
    }
    HRESULT hr = g_audioEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &g_audioDevice);
    if (FAILED(hr)) return false;
    hr = g_audioDevice->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_ALL, nullptr,
                                 reinterpret_cast<void**>(&g_endpointVolume));
    if (FAILED(hr)) { ReleaseAudioEndpoint(); return false; }
    if (!g_volumeCallback) g_volumeCallback = new VolumeEndpointCallback();
    if (g_volumeCallback)  g_endpointVolume->RegisterControlChangeNotify(g_volumeCallback);
    return true;
}

void InitAudio() {
    if (!g_audioEnumerator) {
        HRESULT hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL,
                                      __uuidof(IMMDeviceEnumerator),
                                      reinterpret_cast<void**>(&g_audioEnumerator));
        if (FAILED(hr)) return;
    }
    if (!g_audioNotificationClient) {
        g_audioNotificationClient = new AudioNotificationClient();
        if (g_audioNotificationClient)
            g_audioEnumerator->RegisterEndpointNotificationCallback(g_audioNotificationClient);
    }
    BindAudioEndpoint();
}

void RefreshVolume() {
    if (!g_endpointVolume) BindAudioEndpoint();
    if (g_endpointVolume) {
        float level = 0.0f;
        if (SUCCEEDED(g_endpointVolume->GetMasterVolumeLevelScalar(&level)))
            g_volumeLevel = Clamp(level, 0.0f, 1.0f);
    }
}

void SetSystemVolume(float level) {
    level = Clamp(level, 0.0f, 1.0f);
    if (!g_endpointVolume) BindAudioEndpoint();
    if (g_endpointVolume)  g_endpointVolume->SetMasterVolumeLevelScalar(level, nullptr);
    g_volumeLevel = level;
}

bool RefreshBattery() {
    float oldBatteryLevel     = g_batteryLevel;
    bool  oldOnBatteryCapable = g_onBatteryCapable;
    SYSTEM_POWER_STATUS status = {};
    if (GetSystemPowerStatus(&status)) {
        if (status.BatteryFlag == 128 || status.BatteryLifePercent == 255) {
            g_onBatteryCapable = false;
            g_batteryLevel = 1.0f;
        } else {
            g_onBatteryCapable = true;
            g_batteryLevel = Clamp(status.BatteryLifePercent / 100.0f, 0.0f, 1.0f);
        }
    }
    return oldOnBatteryCapable != g_onBatteryCapable ||
           std::fabs(oldBatteryLevel - g_batteryLevel) > 0.001f;
}

void OpenUri(const wchar_t* uri) {
    ShellExecuteW(nullptr, L"open", uri, nullptr, nullptr, SW_SHOWNORMAL);
}

bool TryOpenUri(const wchar_t* uri) {
    return reinterpret_cast<INT_PTR>(
               ShellExecuteW(nullptr, L"open", uri, nullptr, nullptr, SW_SHOWNORMAL)) > 32;
}

void OpenQuickSettingsDirect(const wchar_t* primaryUri,
                             const wchar_t* alternateUri,
                             const wchar_t* fallbackUri) {
    if (!TryOpenUri(primaryUri) &&
        (!alternateUri || !TryOpenUri(alternateUri)) &&
        fallbackUri) {
        OpenUri(fallbackUri);
    }
}

void OpenMusicFolder() {
    if (g_musicFolderPath[0] != L'\0' && TryOpenUri(g_musicFolderPath)) return;
    ShellExecuteW(nullptr, L"open", L"explorer.exe", L"shell:My Music", nullptr, SW_SHOWNORMAL);
}

void OpenWindowsSearch() {
    INPUT inputs[4] = {};
    inputs[0].type = INPUT_KEYBOARD; inputs[0].ki.wVk = VK_LWIN;
    inputs[1].type = INPUT_KEYBOARD; inputs[1].ki.wVk = 'S';
    inputs[2].type = INPUT_KEYBOARD; inputs[2].ki.wVk = 'S'; inputs[2].ki.dwFlags = KEYEVENTF_KEYUP;
    inputs[3].type = INPUT_KEYBOARD; inputs[3].ki.wVk = VK_LWIN; inputs[3].ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(ARRAYSIZE(inputs), inputs, sizeof(INPUT));
}


// ── GDI+ drawing helpers ──────────────────────────────────────────────────────

void DrawRoundedRectangle(Graphics& g, const RectF& rc, float radius,
                           const Brush& brush, const Pen* pen = nullptr) {
    GraphicsPath path;
    const float d = radius * 2.0f;
    path.AddArc(rc.X, rc.Y, d, d, 180, 90);
    path.AddArc(rc.X + rc.Width - d, rc.Y, d, d, 270, 90);
    path.AddArc(rc.X + rc.Width - d, rc.Y + rc.Height - d, d, d, 0, 90);
    path.AddArc(rc.X, rc.Y + rc.Height - d, d, d, 90, 90);
    path.CloseFigure();
    g.FillPath(&brush, &path);
    if (pen) g.DrawPath(pen, &path);
}

void FillEllipseF(Graphics& g, Brush* brush, float x, float y, float w, float h) {
    g.FillEllipse(brush, static_cast<REAL>(x), static_cast<REAL>(y),
                  static_cast<REAL>(w), static_cast<REAL>(h));
}

void DrawEllipseF(Graphics& g, Pen* pen, float x, float y, float w, float h) {
    g.DrawEllipse(pen, static_cast<REAL>(x), static_cast<REAL>(y),
                  static_cast<REAL>(w), static_cast<REAL>(h));
}

void DrawArcF(Graphics& g, Pen* pen, float x, float y, float w, float h,
               float startAngle, float sweepAngle) {
    g.DrawArc(pen, static_cast<REAL>(x), static_cast<REAL>(y),
              static_cast<REAL>(w), static_cast<REAL>(h),
              static_cast<REAL>(startAngle), static_cast<REAL>(sweepAngle));
}

// ── Glass / theme backgrounds ─────────────────────────────────────────────────

void DrawLiquidGlass(Graphics& g, const Widget& widget) {
    SolidBrush noBackdrop(Color(1, 255, 255, 255));
    SolidBrush topSheen(Color(18, 255, 255, 255));
    Pen outerEdge(Color(170, 255, 255, 255), 1.25f);
    Pen innerEdge(Color(70,  255, 255, 255), 1.0f);
    Pen coolEdge(MaterialAccentColor(45), 1.0f);
    Pen topGlint(Color(120, 255, 255, 255), 1.0f);

    if (widget.round) {
        g.FillEllipse(&noBackdrop, widget.rc);
        g.DrawEllipse(&outerEdge,  widget.rc);
        RectF innerRc = Deflate(widget.rc, 3, 3);
        g.DrawEllipse(&innerEdge, innerRc);
        RectF glintRc = Deflate(widget.rc, 7, 7);
        DrawArcF(g, &topGlint, glintRc.X, glintRc.Y, glintRc.Width, glintRc.Height, 215.0f, 95.0f);
        DrawArcF(g, &coolEdge, glintRc.X, glintRc.Y, glintRc.Width, glintRc.Height,  35.0f, 105.0f);
        return;
    }
    DrawRoundedRectangle(g, widget.rc, 18, noBackdrop, &outerEdge);
    DrawRoundedRectangle(g, Deflate(widget.rc, 3, 3), 15, noBackdrop, &innerEdge);
    RectF sheenRc = Deflate(widget.rc, 5, 5);
    sheenRc.Height = std::min(28.0f, sheenRc.Height * 0.45f);
    DrawRoundedRectangle(g, sheenRc, 14, topSheen);
    g.DrawLine(&topGlint, widget.rc.X + 20.0f, widget.rc.Y + 2.0f,
               widget.rc.X + widget.rc.Width - 20.0f, widget.rc.Y + 2.0f);
    g.DrawLine(&coolEdge, widget.rc.X + 12.0f, widget.rc.Y + widget.rc.Height - 2.0f,
               widget.rc.X + widget.rc.Width - 12.0f, widget.rc.Y + widget.rc.Height - 2.0f);
}

void DrawGlass(Graphics& g, const Widget& widget) {
    if (IsLiquidGlassTheme()) { DrawLiquidGlass(g, widget); return; }

    // ── JonaOS themes: every widget has its own frame colour ─────────────
    {
        const JonaOSPalette* jp = CurrentJonaOSPalette();
        if (jp) {
            Color frameColor;
            switch (widget.id) {
                case WidgetCalendar:        frameColor = jp->calBody;              break;
                case WidgetClock:           frameColor = jp->clockFrame;           break;
                case WidgetMusic:           frameColor = jp->musicFrame;           break;
                case WidgetVolume:          frameColor = jp->volumeFrame;          break;
                case WidgetBattery:         frameColor = jp->batteryFrame;         break;
                case WidgetSearch:          frameColor = jp->searchFrame;          break;
                case WidgetPersonalization: frameColor = jp->personalizationFrame; break;
                case WidgetWifi:            frameColor = jp->wifiFrame;            break;
                case WidgetHotspot:         frameColor = jp->hotspotFrame;         break;
                case WidgetBluetooth:       frameColor = jp->bluetoothFrame;       break;
                case WidgetAccessibility:   frameColor = jp->accessibilityFrame;   break;
                default:                    frameColor = Color(255,0xE5,0xE5,0xEA); break;
            }
            SolidBrush fill(frameColor);
            if (widget.round) {
                g.FillEllipse(&fill, widget.rc);
            } else {
                DrawRoundedRectangle(g, widget.rc, 18, fill);
            }
            // Calendar also needs a distinct header bar on top
            if (widget.id == WidgetCalendar) {
                SolidBrush ribbon(jp->calHeader);
                RectF ribbonRc(widget.rc.X, widget.rc.Y, widget.rc.Width, 40.0f);
                DrawRoundedRectangle(g, ribbonRc, 18, ribbon);
                g.FillRectangle(&ribbon, ribbonRc.X, ribbonRc.Y + 18.0f,
                                ribbonRc.Width, ribbonRc.Height - 18.0f);
            }
            return;
        }
    }

    const ThemePalette* theme = CurrentTheme();
    if (theme) {
        SolidBrush fill(widget.id == WidgetCalendar ? theme->body : theme->accent);
        if (widget.round) g.FillEllipse(&fill, widget.rc);
        else DrawRoundedRectangle(g, widget.rc, 18, fill);
        if (widget.id == WidgetCalendar) {
            SolidBrush ribbon(MaterialThemeHeaderColor(theme));
            RectF ribbonRc(widget.rc.X, widget.rc.Y, widget.rc.Width, 40.0f);
            DrawRoundedRectangle(g, ribbonRc, 18, ribbon);
            g.FillRectangle(&ribbon, ribbonRc.X, ribbonRc.Y + 18.0f,
                            ribbonRc.Width, ribbonRc.Height - 18.0f);
        }
        return;
    }
    // WhiteTransparent path
    SolidBrush glass(Color(112, 255, 255, 255));
    Pen edge(Color(165, 255, 255, 255), 1.2f);
    Pen inner(Color(55,  255, 255, 255), 1.0f);
    if (widget.round) {
        g.FillEllipse(&glass, widget.rc);
        g.DrawEllipse(&edge,  widget.rc);
        RectF innerRc = Deflate(widget.rc, 3, 3);
        g.DrawEllipse(&inner, innerRc);
    } else {
        DrawRoundedRectangle(g, widget.rc, 18, glass, &edge);
        SolidBrush sheen(Color(20, 255, 255, 255));
        DrawRoundedRectangle(g, Deflate(widget.rc, 3, 3), 15, sheen, &inner);
    }
}

void DrawAcrylicGrain(Graphics& g, const Widget& widget) {
    if (g_appearanceTheme != AppearanceTheme::AcrylicTranslucent) return;
    GraphicsPath clipPath;
    if (widget.round) {
        clipPath.AddEllipse(widget.rc);
    } else {
        const float d = 36.0f;
        RectF rc = widget.rc;
        clipPath.AddArc(rc.X, rc.Y, d, d, 180, 90);
        clipPath.AddArc(rc.X + rc.Width - d, rc.Y, d, d, 270, 90);
        clipPath.AddArc(rc.X + rc.Width - d, rc.Y + rc.Height - d, d, d, 0, 90);
        clipPath.AddArc(rc.X, rc.Y + rc.Height - d, d, d, 90, 90);
        clipPath.CloseFigure();
    }
    Region clipRegion(&clipPath);
    g.SetClip(&clipRegion);
    SolidBrush lightSpeckle(Color(16, 255, 255, 255));
    SolidBrush darkSpeckle(Color(12, 0, 0, 0));
    const int left   = static_cast<int>(widget.rc.X);
    const int top    = static_cast<int>(widget.rc.Y);
    const int right  = static_cast<int>(widget.rc.X + widget.rc.Width);
    const int bottom = static_cast<int>(widget.rc.Y + widget.rc.Height);
    for (int y = top; y < bottom; y += 3) {
        for (int x = left; x < right; x += 3) {
            unsigned int h = static_cast<unsigned int>(x) * 374761393u +
                             static_cast<unsigned int>(y) * 668265263u;
            h = (h ^ (h >> 13)) * 1274126177u;
            h ^= (h >> 16);
            if ((h & 0xFF) < 16) {
                Brush* speckle = ((h >> 8) & 1)
                    ? static_cast<Brush*>(&lightSpeckle)
                    : static_cast<Brush*>(&darkSpeckle);
                g.FillRectangle(speckle, static_cast<float>(x),
                                static_cast<float>(y), 1.0f, 1.0f);
            }
        }
    }
    g.ResetClip();
}

// ── Text helper ───────────────────────────────────────────────────────────────

void DrawTextFit(Graphics& g, const wchar_t* text, Font& font, RectF rc,
                 const Brush& brush,
                 StringAlignment horizontal = StringAlignmentCenter,
                 StringAlignment vertical   = StringAlignmentCenter) {
    StringFormat format;
    format.SetAlignment(horizontal);
    format.SetLineAlignment(vertical);
    format.SetTrimming(StringTrimmingEllipsisCharacter);
    format.SetFormatFlags(StringFormatFlagsNoWrap);
    if (IsLiquidGlassTheme()) {
        SolidBrush shadow(Color(75, 0, 0, 0));
        RectF shadowRc(rc.X + 1.0f, rc.Y + 1.0f, rc.Width, rc.Height);
        g.DrawString(text, -1, &font, shadowRc, &format, &shadow);
    }
    g.DrawString(text, -1, &font, rc, &format, &brush);
}

// ── Widget draw functions ─────────────────────────────────────────────────────

void DrawCalendar(Graphics& g, const Widget& widget, FontFamily& family) {
    RectF rc = widget.rc;
    const ThemePalette* theme = CurrentTheme();
    const bool liquidGlass = IsLiquidGlassTheme();
    const JonaOSPalette* jp = CurrentJonaOSPalette();
    SolidBrush headerText(jp ? jp->calHeaderText : theme ? theme->textOnHeader : liquidGlass ? LiquidGlassTextColor()     : Color(245, 255, 255, 255));
    SolidBrush text      (jp ? jp->calBodyText   : theme ? theme->textOnBody   : liquidGlass ? LiquidGlassTextColor(245)  : Color(245, 255, 255, 255));
    SolidBrush muted     (jp ? jp->calBodyText   : theme ? theme->textOnBody   : liquidGlass ? LiquidGlassTextColor(205)  : Color(205, 255, 255, 255));
    SolidBrush red(Color(255, 220, 30, 45));
    Font header (&family, 18, FontStyleRegular, UnitPixel);
    Font dayFont(&family, 14, FontStyleRegular, UnitPixel);
    Font numFont(&family, 15, FontStyleRegular, UnitPixel);

    SYSTEMTIME st;
    GetLocalTime(&st);

    const wchar_t* months[] = {
        L"January", L"February", L"March",     L"April",   L"May",      L"June",
        L"July",    L"August",   L"September", L"October", L"November", L"December"
    };
    wchar_t year[8];
    swprintf_s(year, L"%u", st.wYear);

    DrawTextFit(g, months[st.wMonth - 1], header,
                RectF(rc.X + 16, rc.Y + 10, 170, 24), headerText,
                StringAlignmentNear, StringAlignmentCenter);
    DrawTextFit(g, year, header,
                RectF(rc.X + rc.Width - 96, rc.Y + 10, 80, 24), headerText,
                StringAlignmentFar, StringAlignmentCenter);

    const wchar_t* days[] = {L"S", L"M", L"T", L"W", L"T", L"F", L"S"};
    const float colW   = (rc.Width - 32) / 7.0f;
    const float startX = rc.X + 16;
    const float daysY  = rc.Y + 45;
    for (int i = 0; i < 7; ++i)
        DrawTextFit(g, days[i], dayFont, RectF(startX + i * colW, daysY, colW, 18), muted);

    SYSTEMTIME first = st;
    first.wDay = 1;
    FILETIME ft = {};
    SystemTimeToFileTime(&first, &ft);
    FileTimeToSystemTime(&ft, &first);

    int daysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    bool leap = (st.wYear % 4 == 0 && st.wYear % 100 != 0) || (st.wYear % 400 == 0);
    if (leap) daysInMonth[1] = 29;

    const int   count    = daysInMonth[st.wMonth - 1];
    const int   firstDow = first.wDayOfWeek;
    const float rowH     = 19.0f;
    const float numsY    = rc.Y + 70;
    for (int day = 1; day <= count; ++day) {
        int cell = firstDow + day - 1;
        int row  = cell / 7;
        int col  = cell % 7;
        RectF cellRc(startX + col * colW, numsY + row * rowH, colW, rowH);
        wchar_t label[4];
        swprintf_s(label, L"%d", day);
        if (day == st.wDay) {
            FillEllipseF(g, &red, cellRc.X + colW / 2 - 9.0f, cellRc.Y + 1.0f, 18.0f, 18.0f);
            SolidBrush todayText(Color(255, 255, 255, 255));
            DrawTextFit(g, label, numFont, cellRc, todayText);
        } else {
            DrawTextFit(g, label, numFont, cellRc, text);
        }
    }
}

void DrawClock(Graphics& g, const Widget& widget, FontFamily& family) {
    RectF rc = widget.rc;
    const ThemePalette* theme = CurrentTheme();
    const bool liquidGlass = IsLiquidGlassTheme();
    const JonaOSPalette* jp = CurrentJonaOSPalette();
    const float cx = rc.X + rc.Width  / 2.0f;
    const float cy = rc.Y + rc.Height / 2.0f;
    const float radius = 70.0f;
    Color dialColor     = jp ? jp->clockFace : theme ? MaterialThemeHeaderColor(theme) : liquidGlass ? LiquidGlassTextColor(150) : Color(230, 255, 255, 255);
    Color dialTextColor = jp ? jp->clockNums : theme ? theme->textOnHeader             : liquidGlass ? LiquidGlassTextColor()    : Color(245, 255, 255, 255);
    Pen dialPen(dialColor, 2.0f);
    Pen hourTick  (jp ? jp->clockTicks : theme ? dialTextColor : liquidGlass ? LiquidGlassTextColor()    : Color(255, 255, 210, 38), 3.0f);
    Pen minuteTick(jp ? jp->clockTicks : theme ? dialTextColor : liquidGlass ? LiquidGlassTextColor(220) : Color(230, 255, 255, 255), 1.2f);
    SolidBrush text(dialTextColor);
    Font numFont(&family, 13, FontStyleRegular, UnitPixel);

    if (theme || jp) {
        SolidBrush dialFill(dialColor);
        FillEllipseF(g, &dialFill, cx - radius, cy - radius, radius * 2.0f, radius * 2.0f);
    }
    DrawEllipseF(g, &dialPen, cx - radius, cy - radius, radius * 2.0f, radius * 2.0f);

    for (int i = 0; i < 60; ++i) {
        float angle = (i * 6.0f - 90.0f) * 3.14159265f / 180.0f;
        bool  hour  = i % 5 == 0;
        float outer = radius - 3;
        float inner = hour ? radius - 17 : radius - 10;
        Pen& pen = hour ? hourTick : minuteTick;
        g.DrawLine(&pen, cx + std::cos(angle) * inner, cy + std::sin(angle) * inner,
                         cx + std::cos(angle) * outer, cy + std::sin(angle) * outer);
    }
    for (int n = 1; n <= 12; ++n) {
        float angle = (n * 30.0f - 90.0f) * 3.14159265f / 180.0f;
        wchar_t label[4];
        swprintf_s(label, L"%d", n);
        DrawTextFit(g, label, numFont,
                    RectF(cx + std::cos(angle) * 49 - 11, cy + std::sin(angle) * 49 - 9, 22, 18), text);
    }

    SYSTEMTIME st;
    GetLocalTime(&st);
    float minute    = static_cast<float>(st.wMinute);
    float hour      = (st.wHour % 12) + minute / 60.0f;
    float minAngle  = (minute * 6.0f  - 90.0f) * 3.14159265f / 180.0f;
    float hourAngle = (hour   * 30.0f - 90.0f) * 3.14159265f / 180.0f;
    Color handColor = jp ? jp->clockHands : theme ? dialTextColor : liquidGlass ? MaterialAccentColor() : Color(255, 35, 125, 255);
    Pen handPen(handColor, 4.0f);
    handPen.SetStartCap(LineCapRound);
    handPen.SetEndCap(LineCapRound);
    g.DrawLine(&handPen, cx, cy, cx + std::cos(hourAngle) * 35, cy + std::sin(hourAngle) * 35);
    g.DrawLine(&handPen, cx, cy, cx + std::cos(minAngle)  * 53, cy + std::sin(minAngle)  * 53);
    SolidBrush handHub(handColor);
    FillEllipseF(g, &handHub, cx - 4.0f, cy - 4.0f, 8.0f, 8.0f);
}

void DrawMusic(Graphics& g, const Widget& widget) {
    RectF rc = widget.rc;
    const ThemePalette* theme = CurrentTheme();
    const bool liquidGlass = IsLiquidGlassTheme();
    const JonaOSPalette* jp = CurrentJonaOSPalette();
    SolidBrush blue  (MaterialIconColor(theme, liquidGlass, Color(235, 80,  160, 255)));
    SolidBrush cyan  (MaterialIconColor(theme, liquidGlass, Color(235, 60,  220, 240)));
    SolidBrush yellow(theme ? MaterialThemeHeaderColor(theme) : liquidGlass ? LiquidGlassTextColor(240) : Color(255, 255, 210, 38));
    int heights[] = {24, 52, 34, 68, 46, 58, 28};
    const float barW  = 9;
    const float gap   = 8;
    const float total = 7 * barW + 6 * gap;
    const float x0    = rc.X + (rc.Width - total) / 2.0f;
    const float baseY = rc.Y + 93;
    for (int i = 0; i < 7; ++i) {
        RectF bar(x0 + i * (barW + gap), baseY - heights[i], barW, static_cast<float>(heights[i]));
        if (jp) {
            SolidBrush jbar(jp->musicBars[i]);
            DrawRoundedRectangle(g, bar, 4, jbar);
        } else {
            DrawRoundedRectangle(g, bar, 4, (i % 2) ? cyan : blue);
        }
    }
    const float px = rc.X + rc.Width  / 2.0f;
    const float py = rc.Y + 125;
    if (jp) {
        SolidBrush jplayFrame(jp->musicPlayFrame);
        FillEllipseF(g, &jplayFrame, px - 18.0f, py - 18.0f, 36.0f, 36.0f);
    } else {
        FillEllipseF(g, &yellow, px - 18.0f, py - 18.0f, 36.0f, 36.0f);
    }
    PointF points[] = {PointF(px - 5, py - 9), PointF(px - 5, py + 9), PointF(px + 10, py)};
    SolidBrush playGlyph(jp ? jp->musicPlayGlyph : theme ? theme->textOnHeader : liquidGlass ? MaterialAccentColor() : Color(255, 35, 35, 35));
    g.FillPolygon(&playGlyph, points, 3);
}

void DrawSlider(Graphics& g, const Widget& widget, const wchar_t* label,
                float value, Color fillColor, FontFamily& family) {
    RectF rc = widget.rc;
    const ThemePalette* theme = CurrentTheme();
    const bool liquidGlass = IsLiquidGlassTheme();
    const JonaOSPalette* jp = CurrentJonaOSPalette();
    Color jonaTextColor = Color(255,0x1C,0x1C,0x1E);
    Color jonaFillColor = fillColor;
    if (jp) {
        if (widget.id == WidgetVolume)  { jonaTextColor = jp->volumeText;  jonaFillColor = jp->volumeSlider;  }
        if (widget.id == WidgetBattery) { jonaTextColor = jp->batteryText; jonaFillColor = jp->batterySlider; }
    }
    Font font(&family, 13, FontStyleRegular, UnitPixel);
    SolidBrush text(jp ? jonaTextColor : theme ? theme->textOnAccent : liquidGlass ? LiquidGlassTextColor() : Color(245, 255, 255, 255));
    const bool vertical = IsWidgetVertical(widget.id);
    if (vertical) {
        DrawTextFit(g, label, font, RectF(rc.X + 4, rc.Y + 8, rc.Width - 8, 20), text);
    } else {
        DrawTextFit(g, label, font, RectF(rc.X + 14, rc.Y, 70, rc.Height), text,
                    StringAlignmentNear, StringAlignmentCenter);
    }
    RectF slider = vertical
        ? RectF(rc.X + rc.Width / 2.0f - 7.5f, rc.Y + 32.0f, 15, rc.Height - 48.0f)
        : RectF(rc.X + 90, rc.Y + 7.5f, std::max(40.0f, rc.Width - 110.0f), 15);
    SolidBrush track(jp ? Color(180,0x88,0x88,0x88) : theme ? theme->body : liquidGlass ? LiquidGlassTextColor(90) : Color(105, 255, 255, 255));
    DrawRoundedRectangle(g, slider, 7.5f, track);
    RectF filled = slider;
    if (vertical) {
        filled.Y      += slider.Height * (1.0f - Clamp(value, 0.0f, 1.0f));
        filled.Height *= Clamp(value, 0.0f, 1.0f);
    } else {
        filled.Width  *= Clamp(value, 0.0f, 1.0f);
    }
    SolidBrush fill(jp ? jonaFillColor : theme ? MaterialThemeHeaderColor(theme) : liquidGlass ? MaterialAccentColor() : fillColor);
    DrawRoundedRectangle(g, filled, 7.5f, fill);
    SolidBrush thumb(jp ? jonaTextColor : theme ? theme->textOnAccent : liquidGlass ? LiquidGlassTextColor() : Color(245, 255, 255, 255));
    if (vertical) {
        FillEllipseF(g, &thumb, slider.X + 0.5f,
                     slider.Y + slider.Height * (1.0f - Clamp(value, 0.0f, 1.0f)) - 7.0f, 14.0f, 14.0f);
    } else {
        FillEllipseF(g, &thumb, slider.X + slider.Width * Clamp(value, 0.0f, 1.0f) - 7.0f,
                     slider.Y + 0.5f, 14.0f, 14.0f);
    }
}

void DrawSearchIcon(Graphics& g, const RectF& rc, Color color) {
    Pen pen(color, 3.0f);
    pen.SetStartCap(LineCapRound); pen.SetEndCap(LineCapRound);
    DrawEllipseF(g, &pen, rc.X + 2.0f, rc.Y + 1.0f, 12.0f, 12.0f);
    g.DrawLine(&pen, rc.X + 13, rc.Y + 13, rc.X + 20, rc.Y + 20);
}

void DrawBrushIcon(Graphics& g, const RectF& rc, Color color) {
    Pen pen(color, 3.0f);
    pen.SetStartCap(LineCapRound); pen.SetEndCap(LineCapRound);
    g.DrawLine(&pen, rc.X + 14, rc.Y +  2, rc.X +  5, rc.Y + 12);
    g.DrawLine(&pen, rc.X +  5, rc.Y + 12, rc.X + 12, rc.Y + 19);
    g.DrawLine(&pen, rc.X + 12, rc.Y + 19, rc.X + 22, rc.Y + 10);
}

void DrawWifiIcon(Graphics& g, float cx, float cy, Color color) {
    Pen pen(color, 3.0f);
    pen.SetStartCap(LineCapRound); pen.SetEndCap(LineCapRound);
    DrawArcF(g, &pen, cx - 19.0f, cy -  8.0f, 38.0f, 26.0f, 205.0f, 130.0f);
    DrawArcF(g, &pen, cx - 13.0f, cy -  2.0f, 26.0f, 18.0f, 210.0f, 120.0f);
    DrawArcF(g, &pen, cx -  7.0f, cy +  5.0f, 14.0f, 10.0f, 215.0f, 110.0f);
    SolidBrush dot(color);
    FillEllipseF(g, &dot, cx - 3.0f, cy + 17.0f, 6.0f, 6.0f);
}

void DrawHotspotIcon(Graphics& g, float cx, float cy, Color color) {
    Pen pen(color, 3.0f);
    pen.SetStartCap(LineCapRound);
    DrawEllipseF(g, &pen, cx -  3.0f, cy -  3.0f,  6.0f,  6.0f);
    DrawArcF(g, &pen, cx - 15.0f, cy - 15.0f, 30.0f, 30.0f,  40.0f, 100.0f);
    DrawArcF(g, &pen, cx - 15.0f, cy - 15.0f, 30.0f, 30.0f, 220.0f, 100.0f);
    DrawArcF(g, &pen, cx - 23.0f, cy - 23.0f, 46.0f, 46.0f,  35.0f, 110.0f);
    DrawArcF(g, &pen, cx - 23.0f, cy - 23.0f, 46.0f, 46.0f, 215.0f, 110.0f);
}

void DrawBluetoothIcon(Graphics& g, float cx, float cy, Color color) {
    Pen pen(color, 3.0f);
    pen.SetStartCap(LineCapRound); pen.SetEndCap(LineCapRound);
    PointF top(cx, cy - 20), mid(cx, cy), bottom(cx, cy + 20),
           rightTop(cx + 12, cy - 10), rightBottom(cx + 12, cy + 10);
    g.DrawLine(&pen, top,      bottom);
    g.DrawLine(&pen, top,      rightTop);
    g.DrawLine(&pen, rightTop, mid);
    g.DrawLine(&pen, mid,      rightBottom);
    g.DrawLine(&pen, rightBottom, bottom);
    g.DrawLine(&pen, cx - 12, cy - 10, cx + 12, cy + 10);
    g.DrawLine(&pen, cx - 12, cy + 10, cx + 12, cy - 10);
}

void DrawAccessibilityIcon(Graphics& g, float cx, float cy, Color color) {
    Pen pen(color, 3.0f);
    pen.SetStartCap(LineCapRound); pen.SetEndCap(LineCapRound);
    SolidBrush brush(color);
    FillEllipseF(g, &brush, cx - 5.0f, cy - 22.0f, 10.0f, 10.0f);
    g.DrawLine(&pen, cx - 20, cy - 5,  cx + 20, cy - 5);
    g.DrawLine(&pen, cx,      cy - 5,  cx,      cy + 12);
    g.DrawLine(&pen, cx,      cy + 12, cx - 13, cy + 23);
    g.DrawLine(&pen, cx,      cy + 12, cx + 13, cy + 23);
}

void DrawSmallButton(Graphics& g, const Widget& widget, const wchar_t* label,
                     int kind, FontFamily& family) {
    RectF rc = widget.rc;
    const ThemePalette* theme = CurrentTheme();
    const bool liquidGlass = IsLiquidGlassTheme();
    const JonaOSPalette* jp = CurrentJonaOSPalette();
    Color jonaTextColor = Color(255,0x1C,0x1C,0x1E);
    Color jonaIconColor = Color(255,0x34,0xC7,0x59);
    if (jp) {
        if (widget.id == WidgetSearch)          { jonaTextColor = jp->searchText;          jonaIconColor = jp->searchIcon;          }
        if (widget.id == WidgetPersonalization) { jonaTextColor = jp->personalizationText; jonaIconColor = jp->personalizationIcon; }
    }
    Color iconColor = jp ? jonaIconColor : MaterialIconColor(theme, liquidGlass, Color(255, 255, 210, 38));
    Font font(&family, kSmallButtonFontSize, FontStyleRegular, UnitPixel);
    SolidBrush text(jp ? jonaTextColor : theme ? theme->textOnAccent : liquidGlass ? LiquidGlassTextColor() : Color(245, 255, 255, 255));
    const bool vertical = IsWidgetVertical(widget.id);
    RectF iconRc = vertical
        ? RectF(rc.X + rc.Width / 2.0f - 11.0f, rc.Y + 18.0f, 22, 22)
        : RectF(rc.X + 14, rc.Y + 5, 22, 22);
    if (kind == 0) DrawSearchIcon(g, iconRc, iconColor);
    else           DrawBrushIcon (g, iconRc, iconColor);
    if (vertical)
        DrawTextFit(g, label, font, RectF(rc.X + 4, rc.Y + 47, rc.Width - 8, rc.Height - 54), text);
    else
        DrawTextFit(g, label, font, RectF(rc.X + 42, rc.Y, rc.Width - 48, rc.Height), text);
}

void DrawRoundShortcut(Graphics& g, const Widget& widget, const wchar_t* label,
                       int kind, FontFamily& family) {
    RectF rc = widget.rc;
    const ThemePalette* theme = CurrentTheme();
    const bool liquidGlass = IsLiquidGlassTheme();
    const JonaOSPalette* jp = CurrentJonaOSPalette();
    Color jonaTextColor = Color(255,0xE5,0xE5,0xEA);
    Color jonaIconColor = Color(255,0xFF,0x3B,0x30);
    if (jp) {
        switch (widget.id) {
            case WidgetWifi:          jonaTextColor = jp->wifiText;          jonaIconColor = jp->wifiIcon;          break;
            case WidgetBluetooth:     jonaTextColor = jp->bluetoothText;     jonaIconColor = jp->bluetoothIcon;     break;
            case WidgetHotspot:       jonaTextColor = jp->hotspotText;       jonaIconColor = jp->hotspotIcon;       break;
            case WidgetAccessibility: jonaTextColor = jp->accessibilityText; jonaIconColor = jp->accessibilityIcon; break;
            default: break;
        }
    }
    Color iconColor = jp ? jonaIconColor : MaterialIconColor(theme, liquidGlass, Color(255, 255, 210, 38));
    float cx = rc.X + rc.Width  / 2.0f;
    float cy = rc.Y + 28.0f;
    if      (kind == 0) DrawWifiIcon        (g, cx, cy - 4, iconColor);
    else if (kind == 1) DrawHotspotIcon     (g, cx, cy,     iconColor);
    else if (kind == 2) DrawBluetoothIcon   (g, cx, cy,     iconColor);
    else                DrawAccessibilityIcon(g, cx, cy,    iconColor);
    Font font(&family, kRoundShortcutFontSize, FontStyleRegular, UnitPixel);
    SolidBrush text(jp ? jonaTextColor : theme ? theme->textOnAccent : liquidGlass ? LiquidGlassTextColor() : Color(245, 255, 255, 255));
    DrawTextFit(g, label, font, RectF(rc.X + 2, rc.Y + 46, rc.Width - 4, 20), text);
}

int CurrentMinuteKey() {
    SYSTEMTIME st;
    GetLocalTime(&st);
    return (st.wDay * 24 * 60) + (st.wHour * 60) + st.wMinute;
}

void DestroyRenderSurface() {
    if (g_renderSurface.memDc) {
        if (g_renderSurface.oldBitmap)
            SelectObject(g_renderSurface.memDc, g_renderSurface.oldBitmap);
        if (g_renderSurface.bitmap)
            DeleteObject(g_renderSurface.bitmap);
        DeleteDC(g_renderSurface.memDc);
    }
    g_renderSurface = {};
}

bool EnsureRenderSurface(HDC screenDc, int width, int height) {
    if (g_renderSurface.memDc && g_renderSurface.bitmap &&
        g_renderSurface.width == width && g_renderSurface.height == height) {
        return true;
    }
    DestroyRenderSurface();
    BITMAPINFO bi = {};
    bi.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth       = width;
    bi.bmiHeader.biHeight      = -height;
    bi.bmiHeader.biPlanes      = 1;
    bi.bmiHeader.biBitCount    = 32;
    bi.bmiHeader.biCompression = BI_RGB;
    void* bits = nullptr;
    g_renderSurface.memDc = CreateCompatibleDC(screenDc);
    if (!g_renderSurface.memDc) return false;
    g_renderSurface.bitmap = CreateDIBSection(screenDc, &bi, DIB_RGB_COLORS, &bits, nullptr, 0);
    if (!g_renderSurface.bitmap) { DestroyRenderSurface(); return false; }
    g_renderSurface.oldBitmap = SelectObject(g_renderSurface.memDc, g_renderSurface.bitmap);
    g_renderSurface.width  = width;
    g_renderSurface.height = height;
    return true;
}

void Render() {
    if (!g_hwnd) return;

    // FIX A: Only update the calendar Height — never X/Y — so dragged
    // positions survive every timer tick.
    UpdateCalendarHeight();

    const float renderScale = RenderScale();
    RECT workArea = DesktopWorkArea();
    int w = workArea.right  - workArea.left;
    int h = workArea.bottom - workArea.top;
    if (w <= 0 || h <= 0) return;

    HDC screenDc = GetDC(nullptr);
    if (!screenDc) return;
    if (!EnsureRenderSurface(screenDc, w, h)) { ReleaseDC(nullptr, screenDc); return; }

    {
        Graphics g(g_renderSurface.memDc);
        g.SetSmoothingMode(SmoothingModeAntiAlias);
        g.SetTextRenderingHint(TextRenderingHintClearTypeGridFit);
        g.Clear(Color(0, 0, 0, 0));
        g.ScaleTransform(renderScale, renderScale);

        FontFamily family(L"Segoe UI");

        for (const Widget& widget : g_widgets) DrawGlass(g, widget);
        for (const Widget& widget : g_widgets) DrawAcrylicGrain(g, widget);

        DrawCalendar   (g, g_widgets[WidgetCalendar],        family);
        DrawClock      (g, g_widgets[WidgetClock],           family);
        DrawMusic      (g, g_widgets[WidgetMusic]);
        DrawSlider     (g, g_widgets[WidgetVolume],  L"Volume",  g_volumeLevel,  Color(255, 40,  120, 255), family);
        DrawSlider     (g, g_widgets[WidgetBattery], L"Battery", g_batteryLevel, Color(255, 45,  205, 92),  family);
        DrawSmallButton(g, g_widgets[WidgetSearch],          L"Windows Search", 0, family);
        DrawSmallButton(g, g_widgets[WidgetPersonalization], L"Personalization", 1, family);
        DrawRoundShortcut(g, g_widgets[WidgetWifi],          L"WiFi",          0, family);
        DrawRoundShortcut(g, g_widgets[WidgetHotspot],       L"Hotspot",       1, family);
        DrawRoundShortcut(g, g_widgets[WidgetBluetooth],     L"Bluetooth",     2, family);
        DrawRoundShortcut(g, g_widgets[WidgetAccessibility], L"Accessibility", 3, family);
    }

    SIZE  size = {w, h};
    POINT src  = {0, 0};
    POINT pos  = {workArea.left, workArea.top};
    BLENDFUNCTION blend = {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
    UpdateLayeredWindow(g_hwnd, screenDc, &pos, &size,
                        g_renderSurface.memDc, &src, 0, &blend, ULW_ALPHA);

    if (!g_hasLastWindowRect ||
        g_lastWindowRect.left   != workArea.left  ||
        g_lastWindowRect.top    != workArea.top   ||
        g_lastWindowRect.right  != workArea.right ||
        g_lastWindowRect.bottom != workArea.bottom) {
        SetWindowPos(g_hwnd, HWND_BOTTOM, workArea.left, workArea.top, w, h,
                     SWP_NOACTIVATE | SWP_NOOWNERZORDER);
        g_lastWindowRect      = workArea;
        g_hasLastWindowRect   = true;
    }

    g_lastRenderedMinute = CurrentMinuteKey();
    ReleaseDC(nullptr, screenDc);
}

void UpdateScale() {
    int sw = GetSystemMetrics(SM_CXSCREEN);
    int sh = GetSystemMetrics(SM_CYSCREEN);
    if (sw >= 1920 && sh >= 1080) {
        g_scale = 1.0f;
    } else {
        g_scale = Clamp(std::min(sw / 1920.0f, sh / 1080.0f), 0.65f, 1.0f);
    }
}

void HandleClick(int widgetId) {
    switch (widgetId) {
        case WidgetMusic:          OpenMusicFolder(); break;
        case WidgetSearch:         OpenWindowsSearch(); break;
        case WidgetPersonalization: OpenUri(L"ms-settings:personalization"); break;
        case WidgetWifi:
            OpenQuickSettingsDirect(L"ms-actioncenter:controlcenter/wifi",
                                    L"ms-controlcenter:wifi",
                                    L"ms-settings:network-wifi"); break;
        case WidgetHotspot:
            OpenQuickSettingsDirect(L"ms-actioncenter:controlcenter/MobileHotspot",
                                    L"ms-controlcenter:MobileHotspot",
                                    L"ms-settings:network-mobilehotspot"); break;
        case WidgetBluetooth:
            OpenQuickSettingsDirect(L"ms-actioncenter:controlcenter/bluetooth",
                                    L"ms-controlcenter:bluetooth",
                                    L"ms-settings:bluetooth"); break;
        case WidgetAccessibility:
            OpenQuickSettingsDirect(L"ms-actioncenter:controlcenter/accessibility",
                                    L"ms-controlcenter:accessibility",
                                    L"ms-settings:easeofaccess"); break;
    }
}

// ── WndProc ───────────────────────────────────────────────────────────────────

LRESULT CALLBACK WidgetWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {

        case WM_CREATE: {
            g_hwnd = hwnd;
            MARGINS margins = {-1};
            DwmExtendFrameIntoClientArea(hwnd, &margins);
            SetTimer(hwnd, kTickTimer, kTimerMs, nullptr);
            UpdateScale();
            ApplyDefaultWidgetLayout();
            LoadWidgetPositions();
            InitAudio();
            RefreshVolume();
            RefreshBattery();
            Render();
            return 0;
        }

        case WM_TIMER: {
            bool batteryChanged = RefreshBattery();
            if (batteryChanged || CurrentMinuteKey() != g_lastRenderedMinute) {
                Render();
            }
            return 0;
        }

        case WM_DISPLAYCHANGE:
        case WM_DPICHANGED:
            UpdateScale();
            ApplyDefaultWidgetLayout();
            LoadWidgetPositions();
            Render();
            return 0;

        case kVolumeChangedMessage:
            Render();
            return 0;

        case kAudioDeviceChangedMessage:
            BindAudioEndpoint();
            RefreshVolume();
            Render();
            return 0;

        case kSettingsChangedMessage: {
            const bool oldVolumeVertical          = g_volumeWidgetVertical;
            const bool oldBatteryVertical         = g_batteryWidgetVertical;
            const bool oldSearchVertical          = g_searchWidgetVertical;
            const bool oldPersonalizationVertical = g_personalizationWidgetVertical;
            LoadSettings();
            ApplyOrientationResizeIfNeeded(oldVolumeVertical, oldBatteryVertical,
                                           oldSearchVertical, oldPersonalizationVertical);
            for (Widget& widget : g_widgets) ClampWidgetToDesktop(widget);
            Render();
            return 0;
        }

        case WM_WINDOWPOSCHANGING: {
            // Show Desktop hides the widget through SWP_HIDEWINDOW; strip it
            // before Windows commits the hide.
            WINDOWPOS* wp = reinterpret_cast<WINDOWPOS*>(lParam);
            if (wp && (wp->flags & SWP_HIDEWINDOW)) {
                wp->flags &= ~SWP_HIDEWINDOW;
            }
            return DefWindowProcW(hwnd, msg, wParam, lParam);
        }

        case WM_LBUTTONDOWN: {
            PointF pt = LogicalPointFromLParam(lParam);
            int hit = HitTestWidget(pt);
            if (hit == WidgetVolume && PtInRectF(VolumeSliderRect(), pt)) {
                RectF slider = VolumeSliderRect();
                g_draggingSlider = true;
                SetCapture(hwnd);
                SetSystemVolume(g_volumeWidgetVertical
                    ? 1.0f - (pt.Y - slider.Y) / slider.Height
                    : (pt.X - slider.X) / slider.Width);
                Render();
                return 0;
            }
            if (hit >= 0) {
                g_dragWidget  = hit;
                g_dragDelta   = PointF(pt.X - g_widgets[hit].rc.X, pt.Y - g_widgets[hit].rc.Y);
                g_dragStart   = pt;
                g_dragMoved   = false;
                SetCapture(hwnd);
            }
            return 0;
        }

        case WM_MOUSEMOVE: {
            PointF pt = LogicalPointFromLParam(lParam);
            if (g_draggingSlider) {
                if (!(wParam & MK_LBUTTON)) {
                    g_draggingSlider = false;
                    ReleaseCapture();
                    return 0;
                }
                RectF slider = VolumeSliderRect();
                SetSystemVolume(g_volumeWidgetVertical
                    ? 1.0f - (pt.Y - slider.Y) / slider.Height
                    : (pt.X - slider.X) / slider.Width);
                Render();
                return 0;
            }
            if (g_dragWidget >= 0 && (wParam & MK_LBUTTON)) {
                Widget& widget = g_widgets[g_dragWidget];
                if (std::fabs(pt.X - g_dragStart.X) > 3.0f ||
                    std::fabs(pt.Y - g_dragStart.Y) > 3.0f) {
                    g_dragMoved = true;
                }
                RectF desktop = LogicalDesktopBounds();
                const float maxX = std::max(0.0f, desktop.Width  - widget.rc.Width);
                const float maxY = std::max(0.0f, desktop.Height - widget.rc.Height);
                widget.rc.X = Clamp(pt.X - g_dragDelta.X, 0.0f, maxX);
                widget.rc.Y = Clamp(pt.Y - g_dragDelta.Y, 0.0f, maxY);
                Render();
            }
            return 0;
        }

        case WM_CAPTURECHANGED:
            if (g_dragMoved && g_dragWidget >= 0) SaveWidgetPositions();
            g_draggingSlider = false;
            g_dragWidget     = -1;
            g_dragMoved      = false;
            return 0;

        case WM_LBUTTONUP: {
            PointF pt          = LogicalPointFromLParam(lParam);
            bool   wasSlider   = g_draggingSlider;
            int    relWidget   = g_dragWidget;
            bool   wasMoved    = g_dragMoved;
            g_draggingSlider   = false;
            g_dragWidget       = -1;
            g_dragMoved        = false;
            ReleaseCapture();
            if (!wasSlider && !wasMoved && relWidget >= 0 && PtInWidget(g_widgets[relWidget], pt))
                HandleClick(g_widgets[relWidget].id);
            if (wasMoved && relWidget >= 0)
                SaveWidgetPositions();
            return 0;
        }

        case WM_NCHITTEST: {
            POINT pt = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
            return HitTestWidget(LogicalPointFromScreen(pt)) >= 0 ? HTCLIENT : HTTRANSPARENT;
        }

        case WM_QUERYENDSESSION:
            SaveWidgetPositions();
            return TRUE;

        case WM_ENDSESSION:
            if (wParam) SaveWidgetPositions();
            break;

        case WM_DESTROY:
            SaveWidgetPositions();
            KillTimer(hwnd, kTickTimer);
            ShutdownAudio();
            DestroyRenderSurface();
            g_hwnd = nullptr;
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

// ── UI thread ─────────────────────────────────────────────────────────────────

DWORD WINAPI UiThreadProc(LPVOID) {
    CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);

    GdiplusStartupInput gdiplusInput;
    GdiplusStartup(&g_gdiplusToken, &gdiplusInput, nullptr);

    HINSTANCE instance = GetModuleHandleW(nullptr);

    WNDCLASSEXW wc = {};
    wc.cbSize        = sizeof(wc);
    wc.hInstance     = instance;
    wc.lpfnWndProc   = WidgetWndProc;
    wc.lpszClassName = kClassName;
    wc.hCursor       = LoadCursor(nullptr, IDC_ARROW);
    RegisterClassExW(&wc);

    RECT workArea = DesktopWorkArea();
    int w = workArea.right  - workArea.left;
    int h = workArea.bottom - workArea.top;
    int x = workArea.left;
    int y = workArea.top;

    g_hwnd = CreateWindowExW(
        WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
        kClassName,
        L"Windhawk Desktop Glass Widgets",
        WS_POPUP,
        x, y, w, h,
        nullptr, nullptr, instance, nullptr);

    if (g_hwnd) {
        SetWindowPos(g_hwnd, HWND_BOTTOM, x, y, w, h,
                     SWP_NOACTIVATE | SWP_NOOWNERZORDER);
        ShowWindow(g_hwnd, SW_SHOWNOACTIVATE);
        Render();
    }

    MSG msg;
    while (!g_unloading && GetMessageW(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    if (g_hwnd) DestroyWindow(g_hwnd);
    UnregisterClassW(kClassName, instance);
    GdiplusShutdown(g_gdiplusToken);
    CoUninitialize();
    return 0;
}

}  // namespace

// ── Windhawk boilerplate ──────────────────────────────────────────────────────

BOOL WhTool_ModInit() {
    g_unloading = false;
    LoadSettings();
    g_uiThread = CreateThread(nullptr, 0, UiThreadProc, nullptr, 0, &g_uiThreadId);
    return g_uiThread != nullptr;
}

void WhTool_ModUninit() {
    g_unloading = true;
    if (g_uiThreadId) PostThreadMessageW(g_uiThreadId, WM_QUIT, 0, 0);
    if (g_uiThread) {
        WaitForSingleObject(g_uiThread, 3000);
        CloseHandle(g_uiThread);
        g_uiThread = nullptr;
    }
}

void WhTool_ModSettingsChanged() {
    if (g_hwnd) PostMessageW(g_hwnd, kSettingsChangedMessage, 0, 0);
    else        LoadSettings();
}

bool g_isToolModProcessLauncher;
HANDLE g_toolModProcessMutex;

void WINAPI EntryPoint_Hook() {
    Wh_Log(L">");
    ExitThread(0);
}

BOOL Wh_ModInit() {
    DWORD sessionId;
    if (ProcessIdToSessionId(GetCurrentProcessId(), &sessionId) && sessionId == 0)
        return FALSE;

    bool isExcluded = false, isToolModProcess = false, isCurrentToolModProcess = false;
    int argc;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLine(), &argc);
    if (!argv) { Wh_Log(L"CommandLineToArgvW failed"); return FALSE; }

    for (int i = 1; i < argc; i++) {
        if (wcscmp(argv[i], L"-service")       == 0 ||
            wcscmp(argv[i], L"-service-start") == 0 ||
            wcscmp(argv[i], L"-service-stop")  == 0) {
            isExcluded = true; break;
        }
    }
    for (int i = 1; i < argc - 1; i++) {
        if (wcscmp(argv[i], L"-tool-mod") == 0) {
            isToolModProcess = true;
            if (wcscmp(argv[i + 1], WH_MOD_ID) == 0) isCurrentToolModProcess = true;
            break;
        }
    }
    LocalFree(argv);
    if (isExcluded) return FALSE;

    if (isCurrentToolModProcess) {
        g_toolModProcessMutex =
            CreateMutex(nullptr, TRUE, L"windhawk-tool-mod_" WH_MOD_ID);
        if (!g_toolModProcessMutex) { Wh_Log(L"CreateMutex failed"); ExitProcess(1); }
        if (GetLastError() == ERROR_ALREADY_EXISTS) {
            Wh_Log(L"Tool mod already running (%s)", WH_MOD_ID); ExitProcess(1);
        }
        if (!WhTool_ModInit()) ExitProcess(1);

        IMAGE_DOS_HEADER* dosHeader  = (IMAGE_DOS_HEADER*)GetModuleHandle(nullptr);
        IMAGE_NT_HEADERS* ntHeaders  =
            (IMAGE_NT_HEADERS*)((BYTE*)dosHeader + dosHeader->e_lfanew);
        DWORD entryPointRVA = ntHeaders->OptionalHeader.AddressOfEntryPoint;
        void* entryPoint    = (BYTE*)dosHeader + entryPointRVA;
        Wh_SetFunctionHook(entryPoint, (void*)EntryPoint_Hook, nullptr);
        return TRUE;
    }

    if (isToolModProcess) return FALSE;
    g_isToolModProcessLauncher = true;
    return TRUE;
}

void Wh_ModAfterInit() {
    if (!g_isToolModProcessLauncher) return;

    WCHAR currentProcessPath[MAX_PATH];
    switch (GetModuleFileName(nullptr, currentProcessPath, ARRAYSIZE(currentProcessPath))) {
        case 0:
        case ARRAYSIZE(currentProcessPath):
            Wh_Log(L"GetModuleFileName failed"); return;
    }

    WCHAR commandLine[MAX_PATH + 2 +
                      (sizeof(L" -tool-mod \"" WH_MOD_ID "\"") / sizeof(WCHAR)) - 1];
    swprintf_s(commandLine, L"\"%s\" -tool-mod \"%s\"", currentProcessPath, WH_MOD_ID);

    HMODULE kernelModule = GetModuleHandle(L"kernelbase.dll");
    if (!kernelModule) {
        kernelModule = GetModuleHandle(L"kernel32.dll");
        if (!kernelModule) { Wh_Log(L"No kernelbase.dll/kernel32.dll"); return; }
    }

    using CreateProcessInternalW_t = BOOL(WINAPI*)(
        HANDLE, LPCWSTR, LPWSTR, LPSECURITY_ATTRIBUTES, LPSECURITY_ATTRIBUTES,
        WINBOOL, DWORD, LPVOID, LPCWSTR, LPSTARTUPINFOW, LPPROCESS_INFORMATION, PHANDLE);
    CreateProcessInternalW_t pCreateProcessInternalW =
        (CreateProcessInternalW_t)GetProcAddress(kernelModule, "CreateProcessInternalW");
    if (!pCreateProcessInternalW) { Wh_Log(L"No CreateProcessInternalW"); return; }

    STARTUPINFO si { .cb = sizeof(STARTUPINFO), .dwFlags = STARTF_FORCEOFFFEEDBACK };
    PROCESS_INFORMATION pi;
    if (!pCreateProcessInternalW(nullptr, currentProcessPath, commandLine,
                                 nullptr, nullptr, FALSE, NORMAL_PRIORITY_CLASS,
                                 nullptr, nullptr, &si, &pi, nullptr)) {
        Wh_Log(L"CreateProcess failed"); return;
    }
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}

void Wh_ModSettingsChanged() {
    if (g_isToolModProcessLauncher) return;
    WhTool_ModSettingsChanged();
}

void Wh_ModUninit() {
    if (g_isToolModProcessLauncher) return;
    WhTool_ModUninit();
    ExitProcess(0);
}
