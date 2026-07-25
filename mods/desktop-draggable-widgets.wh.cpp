// ==WindhawkMod==
// @id              desktop-draggable-widgets
// @name            JonaOS Draggable Desktop Widgets
// @description     Draggable Windows desktop widgets with transparent, light, and dark themes for calendar, clock, music, volume, battery, search, personalization, WiFi, Bluetooth, hotspot, and accessibility.
// @version         1.9
// @author          Jona like it, code it
// @github          https://github.com/Stunning-dev
// @include         windhawk.exe
// @compilerOptions -ldwmapi -lgdiplus -lgdi32 -lole32 -lshell32
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# JonaOS Draggable Transparent Widgets
![Demo](https://i.imgur.com/3PpoXyw.gif)
*Widgets can be dragged and made vertical or horizontal*
![image](https://i.imgur.com/IntMcNV.png)
*White Transparent (Better for Dark Wallpapers)*
![image](https://i.imgur.com/lZCz1Yx.png)
*Smocky*
![image](https://i.imgur.com/zyL5hKj.png)
*PassOver*
![image](https://i.imgur.com/eNTYZt8.png)
*Monochrome*
![image](https://i.imgur.com/Y5GPtQ0.png)
*Honey Dawn*
![image](https://i.imgur.com/y2BR0Hj.png)
*Pink*
![image](https://i.imgur.com/K1vNfOh.png)
*CharcoalCream*
![image](https://i.imgur.com/cwhcLEo.png)
*NavyLinen*
![image](https://i.imgur.com/w8EBiFY.png)
*OliveSand*
![image](https://i.imgur.com/UmBUw5y.png)
*CrimsomBlush*
![image](https://i.imgur.com/JepCRa8.png)
*DeepTeal*
![image](https://i.imgur.com/HWn0dNW.png)
*MerlotPeach*
![image](https://i.imgur.com/EvRLWky.png)
*OceanSand*
![image](https://i.imgur.com/LYWTqNs.png)
*IndigoMist*
![image](https://i.imgur.com/opBfLJA.png)
*SpruceMist*
![image](https://i.imgur.com/E7O6IXT.png)
*PlumCream*
![image](https://i.imgur.com/p1je0M5.png)
*MahoganyGold*
![image](https://i.imgur.com/zbc7nN1.png)
*ForestSage*
![image](https://i.imgur.com/hWodzr2.png)
*PineFrost*
![image](https://i.imgur.com/2IC3zNf.png)
*EspressoKhaki*
![image](https://i.imgur.com/7HJeV1g.png)
*OnyxAmber*
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- appearanceTheme: whiteTransparent
  $name: Appearance theme
  $description: Choose the desktop widget appearance. White transparent keeps the original glass look.
  $options:
  - whiteTransparent: White transparent
  - smoky: Smoky
  - passOver: Pass Over
  - monochrome: Monochrome
  - honeyDawn: Honey Dawn
  - pink: Pink
  - charcoalCream: CharcoalCream,
  - navyLinen: NavyLinen,
  - oliveSand: OliveSand,
  - crimsomBlush: CrimsomBlush,
  - deepTeal: DeepTeal,
  - merlotPeach: MerlotPeach,
  - oceanSand: OceanSand,
  - indigoMist: IndigoMist,
  - spruceMist: SpruceMist,
  - plumCream: PlumCream,
  - mahoganyGold: MahoganyGold,
  - forestSage: ForestSage,
  - pineFrost: PineFrost,
  - espressoKhaki: EspressoKhaki,
  - onyxAmber: OnyxAmber,
- widgetSize: comfortable
  $name: Widget size
  $description: Choose how large the desktop widgets appear. Comfortable is the default size.
  $options:
  - comfortable: Comfortable (100%)
  - compact: Compact (56.25%)
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
    WhiteTransparent,
    Smoky,
    PassOver,
    Monochrome,
    HoneyDawn,
    Pink,
    CharcoalCream,
    NavyLinen,
    OliveSand,
    CrimsomBlush,
    DeepTeal,
    MerlotPeach,
    OceanSand,
    IndigoMist,
    SpruceMist,
    PlumCream,
    MahoganyGold,
    ForestSage,
    PineFrost,
    EspressoKhaki,
    OnyxAmber,
};

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
    {AppearanceTheme::Smoky,      L"smoky",      Color(kSolid, 0x11, 0x12, 0x0D), Color(kSolid, 0xFF, 0xFB, 0xF4), Color(kSolid, 0xD8, 0xCF, 0xBC), Color(kSolid, 0xFF, 0xFB, 0xF4), Color(kSolid, 0x11, 0x12, 0x0D), Color(kSolid, 0xFF, 0xFB, 0xF4)},
    {AppearanceTheme::PassOver,   L"passOver",   Color(kSolid, 0x56, 0x1C, 0x24), Color(kSolid, 0xE8, 0xD8, 0xC4), Color(kSolid, 0xC7, 0xB7, 0xA3), Color(kSolid, 0xE8, 0xD8, 0xC4), Color(kSolid, 0x56, 0x1C, 0x24), Color(kSolid, 0xE8, 0xD8, 0xC4)},
    {AppearanceTheme::Monochrome, L"monochrome", Color(kSolid, 0x04, 0x04, 0x08), Color(kSolid, 0xFE, 0xFE, 0xFE), Color(kSolid, 0xFE, 0xFE, 0xFE), Color(kSolid, 0xAA, 0xAD, 0xB1), Color(kSolid, 0x04, 0x04, 0x08), Color(kSolid, 0x04, 0x04, 0x08)},
    {AppearanceTheme::HoneyDawn,   L"honeyDawn",   Color(kSolid, 0xFA, 0xD7, 0x8B), Color(kSolid, 0xFF, 0xF7, 0xE8), Color(kSolid, 0xD8, 0x96, 0x45), Color(kSolid, 0x4A, 0x2D, 0x0D), Color(kSolid, 0x42, 0x30, 0x1A), Color(kSolid, 0xFF, 0xFB, 0xEF)},
    {AppearanceTheme::Pink,         L"pink",         Color(kSolid, 0xB8, 0x1E, 0x68), Color(kSolid, 0xFF, 0xEB, 0xF5), Color(kSolid, 0xF4, 0x79, 0xB8), Color(kSolid, 0xFF, 0xF7, 0xFC), Color(kSolid, 0x43, 0x12, 0x2B), Color(kSolid, 0xFF, 0xFB, 0xFE)},
    {AppearanceTheme::CharcoalCream, L"charcoalCream", Color(kSolid, 0x11, 0x11, 0x11), Color(kSolid, 0xF5, 0xEB, 0xDD), Color(kSolid, 0xF5, 0xEB, 0xDD), Color(kSolid, 0xF5, 0xEB, 0xDD), Color(kSolid, 0x11, 0x11, 0x11), Color(kSolid, 0x11, 0x11, 0x11)},
    {AppearanceTheme::NavyLinen,     L"navyLinen",     Color(kSolid, 0x1F, 0x2A, 0x44), Color(kSolid, 0xF8, 0xF6, 0xF2), Color(kSolid, 0xF8, 0xF6, 0xF2), Color(kSolid, 0xF8, 0xF6, 0xF2), Color(kSolid, 0x1F, 0x2A, 0x44), Color(kSolid, 0x1F, 0x2A, 0x44)},
    {AppearanceTheme::OliveSand,     L"oliveSand",     Color(kSolid, 0x67, 0x88, 0x31), Color(kSolid, 0xCF, 0xC2, 0xAF), Color(kSolid, 0xCF, 0xC2, 0xAF), Color(kSolid, 0xCF, 0xC2, 0xAF), Color(kSolid, 0x67, 0x88, 0x31), Color(kSolid, 0x67, 0x88, 0x31)},
    {AppearanceTheme::CrimsomBlush,  L"crimsomBlush",  Color(kSolid, 0xFD, 0x18, 0x43), Color(kSolid, 0xFF, 0xF9, 0xFA), Color(kSolid, 0xFF, 0xF9, 0xFA), Color(kSolid, 0xFF, 0xF9, 0xFA), Color(kSolid, 0xFD, 0x18, 0x43), Color(kSolid, 0xFD, 0x18, 0x43)},
    {AppearanceTheme::DeepTeal,      L"deepTeal",      Color(kSolid, 0x00, 0x47, 0x41), Color(kSolid, 0xF0, 0xED, 0xE4), Color(kSolid, 0xF0, 0xED, 0xE4), Color(kSolid, 0xF0, 0xED, 0xE4), Color(kSolid, 0x00, 0x47, 0x41), Color(kSolid, 0x00, 0x47, 0x41)},
    {AppearanceTheme::MerlotPeach,   L"merlotPeach",   Color(kSolid, 0x74, 0x1A, 0x2F), Color(kSolid, 0xFF, 0xC6, 0xA8), Color(kSolid, 0xFF, 0xC6, 0xA8), Color(kSolid, 0xFF, 0xC6, 0xA8), Color(kSolid, 0x74, 0x1A, 0x2F), Color(kSolid, 0x74, 0x1A, 0x2F)},
    {AppearanceTheme::OceanSand,     L"oceanSand",     Color(kSolid, 0x00, 0x41, 0x6A), Color(kSolid, 0xF0, 0xEA, 0xD6), Color(kSolid, 0xF0, 0xEA, 0xD6), Color(kSolid, 0xF0, 0xEA, 0xD6), Color(kSolid, 0x00, 0x41, 0x6A), Color(kSolid, 0x00, 0x41, 0x6A)},
    {AppearanceTheme::IndigoMist,    L"indigoMist",    Color(kSolid, 0x27, 0x18, 0x7E), Color(kSolid, 0xF7, 0xF7, 0xFF), Color(kSolid, 0xF7, 0xF7, 0xFF), Color(kSolid, 0xF7, 0xF7, 0xFF), Color(kSolid, 0x27, 0x18, 0x7E), Color(kSolid, 0x27, 0x18, 0x7E)},
    {AppearanceTheme::SpruceMist,    L"spruceMist",    Color(kSolid, 0x00, 0x46, 0x43), Color(kSolid, 0xF0, 0xED, 0xE5), Color(kSolid, 0xF0, 0xED, 0xE5), Color(kSolid, 0xF0, 0xED, 0xE5), Color(kSolid, 0x00, 0x46, 0x43), Color(kSolid, 0x00, 0x46, 0x43)},
    {AppearanceTheme::PlumCream,     L"plumCream",     Color(kSolid, 0x38, 0x19, 0x32), Color(kSolid, 0xFF, 0xF3, 0xE6), Color(kSolid, 0xFF, 0xF3, 0xE6), Color(kSolid, 0xFF, 0xF3, 0xE6), Color(kSolid, 0x38, 0x19, 0x32), Color(kSolid, 0x38, 0x19, 0x32)},
    {AppearanceTheme::MahoganyGold,  L"mahoganyGold",  Color(kSolid, 0x5B, 0x0E, 0x14), Color(kSolid, 0xF1, 0xE1, 0x94), Color(kSolid, 0xF1, 0xE1, 0x94), Color(kSolid, 0xF1, 0xE1, 0x94), Color(kSolid, 0x5B, 0x0E, 0x14), Color(kSolid, 0x5B, 0x0E, 0x14)},
    {AppearanceTheme::ForestSage,    L"forestSage",    Color(kSolid, 0x1A, 0x25, 0x17), Color(kSolid, 0xAC, 0xC8, 0xA2), Color(kSolid, 0xAC, 0xC8, 0xA2), Color(kSolid, 0xAC, 0xC8, 0xA2), Color(kSolid, 0x1A, 0x25, 0x17), Color(kSolid, 0x1A, 0x25, 0x17)},
    {AppearanceTheme::PineFrost,     L"pineFrost",     Color(kSolid, 0x03, 0x32, 0x2E), Color(kSolid, 0xF4, 0xF4, 0xF4), Color(kSolid, 0xF4, 0xF4, 0xF4), Color(kSolid, 0xF4, 0xF4, 0xF4), Color(kSolid, 0x03, 0x32, 0x2E), Color(kSolid, 0x03, 0x32, 0x2E)},
    {AppearanceTheme::EspressoKhaki, L"espressoKhaki", Color(kSolid, 0x44, 0x30, 0x23), Color(kSolid, 0xBE, 0xA4, 0x75), Color(kSolid, 0xBE, 0xA4, 0x75), Color(kSolid, 0xBE, 0xA4, 0x75), Color(kSolid, 0x44, 0x30, 0x23), Color(kSolid, 0x44, 0x30, 0x23)},
    {AppearanceTheme::OnyxAmber,     L"onyxAmber",     Color(kSolid, 0x19, 0x21, 0x23), Color(kSolid, 0xD9, 0x9E, 0x45), Color(kSolid, 0xD9, 0x9E, 0x45), Color(kSolid, 0xD9, 0x9E, 0x45), Color(kSolid, 0x19, 0x21, 0x23), Color(kSolid, 0x19, 0x21, 0x23)},
};

struct Widget {
    int id;
    RectF rc;
    bool round;
};

Widget g_widgets[WidgetCount] = {
    {WidgetCalendar,        RectF(0, 0, 350, 170), false},
    {WidgetClock,           RectF(0, 190, 165, 170), false},
    {WidgetMusic,           RectF(180, 190, 165, 170), false},
    {WidgetVolume,          RectF(0, 380, 350, 30), false},
    {WidgetBattery,         RectF(0, 430, 350, 30), false},
    {WidgetSearch,          RectF(0, 480, 165, 30), false},
    {WidgetPersonalization, RectF(185, 480, 165, 30), false},
    {WidgetWifi,            RectF(0, 530, 70, 70), true},
    {WidgetHotspot,         RectF(90, 530, 70, 70), true},
    {WidgetBluetooth,       RectF(180, 530, 70, 70), true},
    {WidgetAccessibility,   RectF(270, 530, 70, 70), true},
};

HWND g_hwnd = nullptr;
HANDLE g_uiThread = nullptr;
DWORD g_uiThreadId = 0;
volatile bool g_unloading = false;
ULONG_PTR g_gdiplusToken = 0;
float g_scale = 1.0f;
float g_widgetSizeScale = 0.75f;
int g_dragWidget = -1;
PointF g_dragDelta;
PointF g_dragStart;
bool g_dragMoved = false;
bool g_draggingSlider = false;
float g_volumeLevel = 1.0f;
float g_batteryLevel = 1.0f;
bool g_onBatteryCapable = false;
AppearanceTheme g_appearanceTheme = AppearanceTheme::WhiteTransparent;
bool g_volumeWidgetVertical = false;
bool g_batteryWidgetVertical = false;
bool g_searchWidgetVertical = false;
bool g_personalizationWidgetVertical = false;

float Clamp(float value, float minValue, float maxValue) {
    return std::max(minValue, std::min(value, maxValue));
}

float RenderScale() {
    return g_scale * g_widgetSizeScale;
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
    rc.X += x;
    rc.Y += y;
    rc.Width -= x * 2;
    rc.Height -= y * 2;
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
    return pt.X >= rc.X && pt.X <= rc.X + rc.Width && pt.Y >= rc.Y && pt.Y <= rc.Y + rc.Height;
}

const ThemePalette* CurrentTheme() {
    if (g_appearanceTheme == AppearanceTheme::WhiteTransparent) {
        return nullptr;
    }

    for (const ThemePalette& palette : g_palettes) {
        if (palette.theme == g_appearanceTheme) {
            return &palette;
        }
    }

    return nullptr;
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
        case WidgetVolume:
            return g_volumeWidgetVertical;
        case WidgetBattery:
            return g_batteryWidgetVertical;
        case WidgetSearch:
            return g_searchWidgetVertical;
        case WidgetPersonalization:
            return g_personalizationWidgetVertical;
    }
    return false;
}

void ApplyDefaultWidgetLayout() {
    g_widgets[WidgetCalendar].rc = RectF(0, 0, 350, 170);
    g_widgets[WidgetClock].rc = RectF(0, 190, 165, 170);
    g_widgets[WidgetMusic].rc = RectF(180, 190, 165, 170);

    float y = 380.0f;
    if (!g_volumeWidgetVertical && !g_batteryWidgetVertical) {
        g_widgets[WidgetVolume].rc = RectF(0, y, 350, 30);
        y += 50.0f;
        g_widgets[WidgetBattery].rc = RectF(0, y, 350, 30);
        y += 50.0f;
    } else if (g_volumeWidgetVertical && g_batteryWidgetVertical) {
        g_widgets[WidgetVolume].rc = RectF(0, y, 70, 120);
        g_widgets[WidgetBattery].rc = RectF(90, y, 70, 120);
        y += 140.0f;
    } else if (g_volumeWidgetVertical) {
        g_widgets[WidgetVolume].rc = RectF(0, y, 70, 120);
        g_widgets[WidgetBattery].rc = RectF(90, y, 260, 30);
        y += 140.0f;
    } else {
        g_widgets[WidgetVolume].rc = RectF(0, y, 260, 30);
        g_widgets[WidgetBattery].rc = RectF(280, y, 70, 120);
        y += 140.0f;
    }

    if (!g_searchWidgetVertical && !g_personalizationWidgetVertical) {
        g_widgets[WidgetSearch].rc = RectF(0, y, 165, 30);
        g_widgets[WidgetPersonalization].rc = RectF(185, y, 165, 30);
        y += 50.0f;
    } else if (g_searchWidgetVertical && g_personalizationWidgetVertical) {
        g_widgets[WidgetSearch].rc = RectF(0, y, 80, 110);
        g_widgets[WidgetPersonalization].rc = RectF(100, y, 80, 110);
        y += 130.0f;
    } else if (g_searchWidgetVertical) {
        g_widgets[WidgetSearch].rc = RectF(0, y, 80, 110);
        g_widgets[WidgetPersonalization].rc = RectF(100, y, 250, 30);
        y += 130.0f;
    } else {
        g_widgets[WidgetSearch].rc = RectF(0, y, 220, 30);
        g_widgets[WidgetPersonalization].rc = RectF(240, y, 80, 110);
        y += 130.0f;
    }

    g_widgets[WidgetWifi].rc = RectF(0, y, 70, 70);
    g_widgets[WidgetHotspot].rc = RectF(90, y, 70, 70);
    g_widgets[WidgetBluetooth].rc = RectF(180, y, 70, 70);
    g_widgets[WidgetAccessibility].rc = RectF(270, y, 70, 70);

    RectF desktop = LogicalDesktopBounds();
    const float rightOffset = std::max(0.0f, desktop.Width - static_cast<float>(kDesignW));
    for (Widget& widget : g_widgets) {
        widget.rc.X += rightOffset;
    }
}

RectF RectForOrientation(const RectF& current, bool vertical, float horizontalWidth, float verticalWidth) {
    return vertical
        ? RectF(current.X, current.Y, verticalWidth, 120.0f)
        : RectF(current.X, current.Y, horizontalWidth, 30.0f);
}

void ClampWidgetToDesktop(Widget& widget) {
    RectF desktop = LogicalDesktopBounds();
    const float maxX = std::max(0.0f, desktop.Width - widget.rc.Width);
    const float maxY = std::max(0.0f, desktop.Height - widget.rc.Height);
    widget.rc.X = Clamp(widget.rc.X, 0.0f, maxX);
    widget.rc.Y = Clamp(widget.rc.Y, 0.0f, maxY);
}

void ApplyOrientationResizeIfNeeded(bool oldVolumeVertical,
                                    bool oldBatteryVertical,
                                    bool oldSearchVertical,
                                    bool oldPersonalizationVertical) {
    if (oldVolumeVertical != g_volumeWidgetVertical) {
        g_widgets[WidgetVolume].rc = RectForOrientation(g_widgets[WidgetVolume].rc, g_volumeWidgetVertical, 350.0f, 70.0f);
        ClampWidgetToDesktop(g_widgets[WidgetVolume]);
    }

    if (oldBatteryVertical != g_batteryWidgetVertical) {
        g_widgets[WidgetBattery].rc = RectForOrientation(g_widgets[WidgetBattery].rc, g_batteryWidgetVertical, 350.0f, 70.0f);
        ClampWidgetToDesktop(g_widgets[WidgetBattery]);
    }

    if (oldSearchVertical != g_searchWidgetVertical) {
        g_widgets[WidgetSearch].rc = RectForOrientation(g_widgets[WidgetSearch].rc, g_searchWidgetVertical, 165.0f, 80.0f);
        ClampWidgetToDesktop(g_widgets[WidgetSearch]);
    }

    if (oldPersonalizationVertical != g_personalizationWidgetVertical) {
        g_widgets[WidgetPersonalization].rc = RectForOrientation(g_widgets[WidgetPersonalization].rc, g_personalizationWidgetVertical, 165.0f, 80.0f);
        ClampWidgetToDesktop(g_widgets[WidgetPersonalization]);
    }
}

void LoadSettings() {
    g_appearanceTheme = AppearanceTheme::WhiteTransparent;
    g_widgetSizeScale = 0.75f;
    g_volumeWidgetVertical = false;
    g_batteryWidgetVertical = false;
    g_searchWidgetVertical = false;
    g_personalizationWidgetVertical = false;

    PCWSTR theme = Wh_GetStringSetting(L"appearanceTheme");
    if (theme) {
        for (const ThemePalette& palette : g_palettes) {
            if (wcscmp(theme, palette.settingValue) == 0) {
                g_appearanceTheme = palette.theme;
                break;
            }
        }
        Wh_FreeStringSetting(theme);
    }

    PCWSTR widgetSize = Wh_GetStringSetting(L"widgetSize");
    if (widgetSize) {
        if (wcscmp(widgetSize, L"comfortable") == 0) {
            g_widgetSizeScale = 0.75f;
        } else if (wcscmp(widgetSize, L"compact") == 0) {
            g_widgetSizeScale = 0.5625f;
        }
        Wh_FreeStringSetting(widgetSize);
    }

    g_volumeWidgetVertical = ReadVerticalOrientationSetting(L"volumeWidgetOrientation");
    g_batteryWidgetVertical = ReadVerticalOrientationSetting(L"batteryWidgetOrientation");
    g_searchWidgetVertical = ReadVerticalOrientationSetting(L"searchWidgetOrientation");
    g_personalizationWidgetVertical = ReadVerticalOrientationSetting(L"personalizationWidgetOrientation");
}

bool PtInWidget(const Widget& widget, PointF pt) {
    if (!PtInRectF(widget.rc, pt)) {
        return false;
    }

    if (!widget.round) {
        return true;
    }

    const float cx = widget.rc.X + widget.rc.Width / 2.0f;
    const float cy = widget.rc.Y + widget.rc.Height / 2.0f;
    const float rx = widget.rc.Width / 2.0f;
    const float ry = widget.rc.Height / 2.0f;
    const float dx = (pt.X - cx) / rx;
    const float dy = (pt.Y - cy) / ry;
    return dx * dx + dy * dy <= 1.0f;
}

int HitTestWidget(PointF pt) {
    for (int i = WidgetCount - 1; i >= 0; --i) {
        if (PtInWidget(g_widgets[i], pt)) {
            return i;
        }
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

bool GetEndpointVolumeInterface(IAudioEndpointVolume** endpointVolume) {
    *endpointVolume = nullptr;

    IMMDeviceEnumerator* enumerator = nullptr;
    HRESULT hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL,
                                  __uuidof(IMMDeviceEnumerator), reinterpret_cast<void**>(&enumerator));
    if (FAILED(hr)) {
        return false;
    }

    IMMDevice* device = nullptr;
    hr = enumerator->GetDefaultAudioEndpoint(eRender, eConsole, &device);
    enumerator->Release();
    if (FAILED(hr)) {
        return false;
    }

    hr = device->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_ALL, nullptr,
                          reinterpret_cast<void**>(endpointVolume));
    device->Release();
    return SUCCEEDED(hr);
}

void RefreshVolume() {
    IAudioEndpointVolume* endpointVolume = nullptr;
    if (GetEndpointVolumeInterface(&endpointVolume)) {
        float level = 0.0f;
        if (SUCCEEDED(endpointVolume->GetMasterVolumeLevelScalar(&level))) {
            g_volumeLevel = Clamp(level, 0.0f, 1.0f);
        }
        endpointVolume->Release();
    }
}

void SetSystemVolume(float level) {
    level = Clamp(level, 0.0f, 1.0f);
    IAudioEndpointVolume* endpointVolume = nullptr;
    if (GetEndpointVolumeInterface(&endpointVolume)) {
        endpointVolume->SetMasterVolumeLevelScalar(level, nullptr);
        endpointVolume->Release();
    }
    g_volumeLevel = level;
}

void RefreshBattery() {
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
}

void OpenUri(const wchar_t* uri) {
    ShellExecuteW(nullptr, L"open", uri, nullptr, nullptr, SW_SHOWNORMAL);
}

bool TryOpenUri(const wchar_t* uri) {
    return reinterpret_cast<INT_PTR>(
               ShellExecuteW(nullptr, L"open", uri, nullptr, nullptr, SW_SHOWNORMAL)) > 32;
}

// ── OpenQuickSettingsDirect ───────────────────────────────────────────────────
// Opens the Quick Settings flyout on a specific feature page.
// Strategy:
//   1. Try the modern Windows 11 ms-actioncenter:controlcenter/<page> URI.
//   2. If that fails, try the ms-controlcenter:<page> alternate form used on
//      some older Windows 11 builds.
//   3. If both fail, fall back to the full Settings app URI so the user
//      still reaches the right place (Windows 10 / legacy builds).
// ─────────────────────────────────────────────────────────────────────────────
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
    ShellExecuteW(nullptr, L"open", L"explorer.exe", L"shell:My Music", nullptr, SW_SHOWNORMAL);
}

void OpenWindowsSearch() {
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
    SendInput(ARRAYSIZE(inputs), inputs, sizeof(INPUT));
}

void DrawRoundedRectangle(Graphics& g, const RectF& rc, float radius, const Brush& brush, const Pen* pen = nullptr) {
    GraphicsPath path;
    const float d = radius * 2.0f;
    path.AddArc(rc.X, rc.Y, d, d, 180, 90);
    path.AddArc(rc.X + rc.Width - d, rc.Y, d, d, 270, 90);
    path.AddArc(rc.X + rc.Width - d, rc.Y + rc.Height - d, d, d, 0, 90);
    path.AddArc(rc.X, rc.Y + rc.Height - d, d, d, 90, 90);
    path.CloseFigure();
    g.FillPath(&brush, &path);
    if (pen) {
        g.DrawPath(pen, &path);
    }
}

void FillEllipseF(Graphics& g, Brush* brush, float x, float y, float width, float height) {
    g.FillEllipse(brush, static_cast<REAL>(x), static_cast<REAL>(y),
                  static_cast<REAL>(width), static_cast<REAL>(height));
}

void DrawEllipseF(Graphics& g, Pen* pen, float x, float y, float width, float height) {
    g.DrawEllipse(pen, static_cast<REAL>(x), static_cast<REAL>(y),
                  static_cast<REAL>(width), static_cast<REAL>(height));
}

void DrawArcF(Graphics& g, Pen* pen, float x, float y, float width, float height, float startAngle, float sweepAngle) {
    g.DrawArc(pen, static_cast<REAL>(x), static_cast<REAL>(y),
              static_cast<REAL>(width), static_cast<REAL>(height),
              static_cast<REAL>(startAngle), static_cast<REAL>(sweepAngle));
}

void DrawGlass(Graphics& g, const Widget& widget) {
    const ThemePalette* theme = CurrentTheme();
    if (theme) {
        SolidBrush fill(widget.id == WidgetCalendar ? theme->body : theme->accent);
        if (widget.round) {
            g.FillEllipse(&fill, widget.rc);
        } else {
            DrawRoundedRectangle(g, widget.rc, 18, fill);
        }

        if (widget.id == WidgetCalendar) {
            SolidBrush ribbon(theme->header);
            RectF ribbonRc(widget.rc.X, widget.rc.Y, widget.rc.Width, 40.0f);
            DrawRoundedRectangle(g, ribbonRc, 18, ribbon);
            g.FillRectangle(&ribbon, ribbonRc.X, ribbonRc.Y + 18.0f, ribbonRc.Width, ribbonRc.Height - 18.0f);
        }
        return;
    }

    SolidBrush glass(Color(112, 255, 255, 255));
    Pen edge(Color(165, 255, 255, 255), 1.2f);
    Pen inner(Color(55, 255, 255, 255), 1.0f);

    if (widget.round) {
        g.FillEllipse(&glass, widget.rc);
        g.DrawEllipse(&edge, widget.rc);
        RectF innerRc = Deflate(widget.rc, 3, 3);
        g.DrawEllipse(&inner, innerRc);
    } else {
        DrawRoundedRectangle(g, widget.rc, 18, glass, &edge);
        SolidBrush sheen(Color(20, 255, 255, 255));
        DrawRoundedRectangle(g, Deflate(widget.rc, 3, 3), 15, sheen, &inner);
    }
}

void DrawTextFit(Graphics& g, const wchar_t* text, Font& font, RectF rc, const Brush& brush,
                 StringAlignment horizontal = StringAlignmentCenter,
                 StringAlignment vertical = StringAlignmentCenter) {
    StringFormat format;
    format.SetAlignment(horizontal);
    format.SetLineAlignment(vertical);
    format.SetTrimming(StringTrimmingEllipsisCharacter);
    format.SetFormatFlags(StringFormatFlagsNoWrap);
    g.DrawString(text, -1, &font, rc, &format, &brush);
}

void DrawCalendar(Graphics& g, const Widget& widget, FontFamily& family) {
    RectF rc = widget.rc;
    const ThemePalette* theme = CurrentTheme();
    SolidBrush headerText(theme ? theme->textOnHeader : Color(245, 255, 255, 255));
    SolidBrush text(theme ? theme->textOnBody : Color(245, 255, 255, 255));
    SolidBrush muted(theme ? theme->textOnBody : Color(205, 255, 255, 255));
    SolidBrush red(Color(255, 220, 30, 45));
    Font header(&family, 18, FontStyleRegular, UnitPixel);
    Font dayFont(&family, 14, FontStyleRegular, UnitPixel);
    Font numFont(&family, 15, FontStyleRegular, UnitPixel);

    SYSTEMTIME st;
    GetLocalTime(&st);

    const wchar_t* months[] = {
        L"January", L"February", L"March", L"April", L"May", L"June",
        L"July", L"August", L"September", L"October", L"November", L"December"
    };
    wchar_t year[8];
    swprintf_s(year, L"%u", st.wYear);

    DrawTextFit(g, months[st.wMonth - 1], header, RectF(rc.X + 16, rc.Y + 10, 170, 24), headerText,
                StringAlignmentNear, StringAlignmentCenter);
    DrawTextFit(g, year, header, RectF(rc.X + rc.Width - 96, rc.Y + 10, 80, 24), headerText,
                StringAlignmentFar, StringAlignmentCenter);

    const wchar_t* days[] = {L"S", L"M", L"T", L"W", L"T", L"F", L"S"};
    const float colW = (rc.Width - 32) / 7.0f;
    const float startX = rc.X + 16;
    const float daysY = rc.Y + 45;
    for (int i = 0; i < 7; ++i) {
        DrawTextFit(g, days[i], dayFont, RectF(startX + i * colW, daysY, colW, 18), muted);
    }

    SYSTEMTIME first = st;
    first.wDay = 1;
    FILETIME ft = {};
    SystemTimeToFileTime(&first, &ft);
    FileTimeToSystemTime(&ft, &first);

    int daysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    bool leap = (st.wYear % 4 == 0 && st.wYear % 100 != 0) || (st.wYear % 400 == 0);
    if (leap) {
        daysInMonth[1] = 29;
    }

    const int count = daysInMonth[st.wMonth - 1];
    const int firstDow = first.wDayOfWeek;
    const float rowH = 19.0f;
    const float numsY = rc.Y + 70;
    for (int day = 1; day <= count; ++day) {
        int cell = firstDow + day - 1;
        int row = cell / 7;
        int col = cell % 7;
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
    const float cx = rc.X + rc.Width / 2.0f;
    const float cy = rc.Y + rc.Height / 2.0f;
    const float radius = 70.0f;
    Color dialColor = theme ? theme->header : Color(230, 255, 255, 255);
    Color dialTextColor = theme ? theme->textOnHeader : Color(245, 255, 255, 255);
    Pen dialPen(dialColor, 2.0f);
    Pen hourTick(theme ? dialTextColor : Color(255, 255, 210, 38), 3.0f);
    Pen minuteTick(theme ? dialTextColor : Color(230, 255, 255, 255), 1.2f);
    SolidBrush text(dialTextColor);
    Font numFont(&family, 13, FontStyleRegular, UnitPixel);

    if (theme) {
        SolidBrush dialFill(dialColor);
        FillEllipseF(g, &dialFill, cx - radius, cy - radius, radius * 2.0f, radius * 2.0f);
    }
    DrawEllipseF(g, &dialPen, cx - radius, cy - radius, radius * 2.0f, radius * 2.0f);

    for (int i = 0; i < 60; ++i) {
        float angle = (i * 6.0f - 90.0f) * 3.14159265f / 180.0f;
        bool hour = i % 5 == 0;
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
    float minute = st.wMinute + st.wSecond / 60.0f;
    float hour = (st.wHour % 12) + minute / 60.0f;
    float minAngle = (minute * 6.0f - 90.0f) * 3.14159265f / 180.0f;
    float hourAngle = (hour * 30.0f - 90.0f) * 3.14159265f / 180.0f;
    Color handColor = theme ? dialTextColor : Color(255, 35, 125, 255);
    Pen handPen(handColor, 4.0f);
    handPen.SetStartCap(LineCapRound);
    handPen.SetEndCap(LineCapRound);
    g.DrawLine(&handPen, cx, cy, cx + std::cos(hourAngle) * 35, cy + std::sin(hourAngle) * 35);
    g.DrawLine(&handPen, cx, cy, cx + std::cos(minAngle) * 53, cy + std::sin(minAngle) * 53);
    SolidBrush handHub(handColor);
    FillEllipseF(g, &handHub, cx - 4.0f, cy - 4.0f, 8.0f, 8.0f);
}

void DrawMusic(Graphics& g, const Widget& widget) {
    RectF rc = widget.rc;
    const ThemePalette* theme = CurrentTheme();
    SolidBrush blue(theme ? theme->textOnAccent : Color(235, 80, 160, 255));
    SolidBrush cyan(theme ? theme->textOnAccent : Color(235, 60, 220, 240));
    SolidBrush yellow(theme ? theme->header : Color(255, 255, 210, 38));
    int heights[] = {24, 52, 34, 68, 46, 58, 28};
    const float barW = 9;
    const float gap = 8;
    const float total = 7 * barW + 6 * gap;
    const float x0 = rc.X + (rc.Width - total) / 2.0f;
    const float baseY = rc.Y + 93;
    for (int i = 0; i < 7; ++i) {
        RectF bar(x0 + i * (barW + gap), baseY - heights[i], barW, static_cast<float>(heights[i]));
        DrawRoundedRectangle(g, bar, 4, (i % 2) ? cyan : blue);
    }

    const float px = rc.X + rc.Width / 2.0f;
    const float py = rc.Y + 125;
    FillEllipseF(g, &yellow, px - 18.0f, py - 18.0f, 36.0f, 36.0f);
    PointF points[] = {PointF(px - 5, py - 9), PointF(px - 5, py + 9), PointF(px + 10, py)};
    SolidBrush playGlyph(theme ? theme->textOnHeader : Color(255, 35, 35, 35));
    g.FillPolygon(&playGlyph, points, 3);
}

void DrawSlider(Graphics& g, const Widget& widget, const wchar_t* label, float value, Color fillColor, FontFamily& family) {
    RectF rc = widget.rc;
    const ThemePalette* theme = CurrentTheme();
    Font font(&family, 13, FontStyleRegular, UnitPixel);
    SolidBrush text(theme ? theme->textOnAccent : Color(245, 255, 255, 255));
    const bool vertical = IsWidgetVertical(widget.id);
    if (vertical) {
        DrawTextFit(g, label, font, RectF(rc.X + 4, rc.Y + 8, rc.Width - 8, 20), text);
    } else {
        DrawTextFit(g, label, font, RectF(rc.X + 14, rc.Y, 70, rc.Height), text, StringAlignmentNear, StringAlignmentCenter);
    }

    RectF slider = vertical
        ? RectF(rc.X + rc.Width / 2.0f - 7.5f, rc.Y + 32.0f, 15, rc.Height - 48.0f)
        : RectF(rc.X + 90, rc.Y + 7.5f, std::max(40.0f, rc.Width - 110.0f), 15);
    SolidBrush track(theme ? theme->body : Color(105, 255, 255, 255));
    DrawRoundedRectangle(g, slider, 7.5f, track);
    RectF filled = slider;
    if (vertical) {
        filled.Y += slider.Height * (1.0f - Clamp(value, 0.0f, 1.0f));
        filled.Height *= Clamp(value, 0.0f, 1.0f);
    } else {
        filled.Width *= Clamp(value, 0.0f, 1.0f);
    }
    SolidBrush fill(theme ? theme->header : fillColor);
    DrawRoundedRectangle(g, filled, 7.5f, fill);
    SolidBrush thumb(theme ? theme->textOnAccent : Color(245, 255, 255, 255));
    if (vertical) {
        FillEllipseF(g, &thumb, slider.X + 0.5f, slider.Y + slider.Height * (1.0f - Clamp(value, 0.0f, 1.0f)) - 7.0f, 14.0f, 14.0f);
    } else {
        FillEllipseF(g, &thumb, slider.X + slider.Width * Clamp(value, 0.0f, 1.0f) - 7.0f, slider.Y + 0.5f, 14.0f, 14.0f);
    }
}

void DrawSearchIcon(Graphics& g, const RectF& rc, Color color) {
    Pen pen(color, 3.0f);
    pen.SetStartCap(LineCapRound);
    pen.SetEndCap(LineCapRound);
    DrawEllipseF(g, &pen, rc.X + 2.0f, rc.Y + 1.0f, 12.0f, 12.0f);
    g.DrawLine(&pen, rc.X + 13, rc.Y + 13, rc.X + 20, rc.Y + 20);
}

void DrawBrushIcon(Graphics& g, const RectF& rc, Color color) {
    Pen pen(color, 3.0f);
    pen.SetStartCap(LineCapRound);
    pen.SetEndCap(LineCapRound);
    g.DrawLine(&pen, rc.X + 14, rc.Y + 2, rc.X + 5, rc.Y + 12);
    g.DrawLine(&pen, rc.X + 5, rc.Y + 12, rc.X + 12, rc.Y + 19);
    g.DrawLine(&pen, rc.X + 12, rc.Y + 19, rc.X + 22, rc.Y + 10);
}

void DrawWifiIcon(Graphics& g, float cx, float cy, Color color) {
    Pen pen(color, 3.0f);
    pen.SetStartCap(LineCapRound);
    pen.SetEndCap(LineCapRound);
    DrawArcF(g, &pen, cx - 19.0f, cy - 8.0f, 38.0f, 26.0f, 205.0f, 130.0f);
    DrawArcF(g, &pen, cx - 13.0f, cy - 2.0f, 26.0f, 18.0f, 210.0f, 120.0f);
    DrawArcF(g, &pen, cx - 7.0f, cy + 5.0f, 14.0f, 10.0f, 215.0f, 110.0f);
    SolidBrush dot(color);
    FillEllipseF(g, &dot, cx - 3.0f, cy + 17.0f, 6.0f, 6.0f);
}

void DrawHotspotIcon(Graphics& g, float cx, float cy, Color color) {
    Pen pen(color, 3.0f);
    pen.SetStartCap(LineCapRound);
    DrawEllipseF(g, &pen, cx - 3.0f, cy - 3.0f, 6.0f, 6.0f);
    DrawArcF(g, &pen, cx - 15.0f, cy - 15.0f, 30.0f, 30.0f, 40.0f, 100.0f);
    DrawArcF(g, &pen, cx - 15.0f, cy - 15.0f, 30.0f, 30.0f, 220.0f, 100.0f);
    DrawArcF(g, &pen, cx - 23.0f, cy - 23.0f, 46.0f, 46.0f, 35.0f, 110.0f);
    DrawArcF(g, &pen, cx - 23.0f, cy - 23.0f, 46.0f, 46.0f, 215.0f, 110.0f);
}

void DrawBluetoothIcon(Graphics& g, float cx, float cy, Color color) {
    Pen pen(color, 3.0f);
    pen.SetStartCap(LineCapRound);
    pen.SetEndCap(LineCapRound);
    PointF top(cx, cy - 20), mid(cx, cy), bottom(cx, cy + 20), rightTop(cx + 12, cy - 10), rightBottom(cx + 12, cy + 10);
    g.DrawLine(&pen, top, bottom);
    g.DrawLine(&pen, top, rightTop);
    g.DrawLine(&pen, rightTop, mid);
    g.DrawLine(&pen, mid, rightBottom);
    g.DrawLine(&pen, rightBottom, bottom);
    g.DrawLine(&pen, cx - 12, cy - 10, cx + 12, cy + 10);
    g.DrawLine(&pen, cx - 12, cy + 10, cx + 12, cy - 10);
}

void DrawAccessibilityIcon(Graphics& g, float cx, float cy, Color color) {
    Pen pen(color, 3.0f);
    pen.SetStartCap(LineCapRound);
    pen.SetEndCap(LineCapRound);
    SolidBrush brush(color);
    FillEllipseF(g, &brush, cx - 5.0f, cy - 22.0f, 10.0f, 10.0f);
    g.DrawLine(&pen, cx - 20, cy - 5, cx + 20, cy - 5);
    g.DrawLine(&pen, cx, cy - 5, cx, cy + 12);
    g.DrawLine(&pen, cx, cy + 12, cx - 13, cy + 23);
    g.DrawLine(&pen, cx, cy + 12, cx + 13, cy + 23);
}

void DrawSmallButton(Graphics& g, const Widget& widget, const wchar_t* label, int kind, FontFamily& family) {
    RectF rc = widget.rc;
    const ThemePalette* theme = CurrentTheme();
    Color iconColor = theme ? theme->textOnAccent : Color(255, 255, 210, 38);
    Font font(&family, 13, FontStyleRegular, UnitPixel);
    SolidBrush text(theme ? theme->textOnAccent : Color(245, 255, 255, 255));
    const bool vertical = IsWidgetVertical(widget.id);
    RectF iconRc = vertical
        ? RectF(rc.X + rc.Width / 2.0f - 11.0f, rc.Y + 18.0f, 22, 22)
        : RectF(rc.X + 14, rc.Y + 5, 22, 22);
    if (kind == 0) {
        DrawSearchIcon(g, iconRc, iconColor);
    } else {
        DrawBrushIcon(g, iconRc, iconColor);
    }
    if (vertical) {
        DrawTextFit(g, label, font, RectF(rc.X + 6, rc.Y + 48, rc.Width - 12, rc.Height - 56), text);
    } else {
        DrawTextFit(g, label, font, RectF(rc.X + 42, rc.Y, rc.Width - 52, rc.Height), text);
    }
}

void DrawRoundShortcut(Graphics& g, const Widget& widget, const wchar_t* label, int kind, FontFamily& family) {
    RectF rc = widget.rc;
    const ThemePalette* theme = CurrentTheme();
    Color iconColor = theme ? theme->textOnAccent : Color(255, 255, 210, 38);
    float cx = rc.X + rc.Width / 2.0f;
    float cy = rc.Y + 28.0f;
    if (kind == 0) {
        DrawWifiIcon(g, cx, cy - 4, iconColor);
    } else if (kind == 1) {
        DrawHotspotIcon(g, cx, cy, iconColor);
    } else if (kind == 2) {
        DrawBluetoothIcon(g, cx, cy, iconColor);
    } else {
        DrawAccessibilityIcon(g, cx, cy, iconColor);
    }

    Font font(&family, wcscmp(label, L"Accessibility") == 0 ? 8.4f : 10.0f, FontStyleRegular, UnitPixel);
    SolidBrush text(theme ? theme->textOnAccent : Color(245, 255, 255, 255));
    DrawTextFit(g, label, font, RectF(rc.X + 4, rc.Y + 47, rc.Width - 8, 16), text);
}

void Render() {
    if (!g_hwnd) {
        return;
    }

    const float renderScale = RenderScale();
    RECT workArea = DesktopWorkArea();
    int w = workArea.right - workArea.left;
    int h = workArea.bottom - workArea.top;
    if (w <= 0 || h <= 0) {
        return;
    }

    HDC screenDc = GetDC(nullptr);
    HDC memDc = CreateCompatibleDC(screenDc);

    BITMAPINFO bi = {};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = w;
    bi.bmiHeader.biHeight = -h;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;

    void* bits = nullptr;
    HBITMAP bitmap = CreateDIBSection(screenDc, &bi, DIB_RGB_COLORS, &bits, nullptr, 0);
    HGDIOBJ oldBitmap = SelectObject(memDc, bitmap);

    {
        Graphics g(memDc);
        g.SetSmoothingMode(SmoothingModeAntiAlias);
        g.SetTextRenderingHint(TextRenderingHintClearTypeGridFit);
        g.Clear(Color(0, 0, 0, 0));
        g.ScaleTransform(renderScale, renderScale);

        FontFamily family(L"Calibri");

        for (const Widget& widget : g_widgets) {
            DrawGlass(g, widget);
        }

        DrawCalendar(g, g_widgets[WidgetCalendar], family);
        DrawClock(g, g_widgets[WidgetClock], family);
        DrawMusic(g, g_widgets[WidgetMusic]);
        DrawSlider(g, g_widgets[WidgetVolume], L"Volume", g_volumeLevel, Color(255, 40, 120, 255), family);
        DrawSlider(g, g_widgets[WidgetBattery], L"Battery", g_batteryLevel, Color(255, 45, 205, 92), family);
        DrawSmallButton(g, g_widgets[WidgetSearch], L"Windows Search", 0, family);
        DrawSmallButton(g, g_widgets[WidgetPersonalization], L"Personalization", 1, family);
        DrawRoundShortcut(g, g_widgets[WidgetWifi], L"WiFi", 0, family);
        DrawRoundShortcut(g, g_widgets[WidgetHotspot], L"Hotspot", 1, family);
        DrawRoundShortcut(g, g_widgets[WidgetBluetooth], L"Bluetooth", 2, family);
        DrawRoundShortcut(g, g_widgets[WidgetAccessibility], L"Accessibility", 3, family);
    }

    SIZE size = {w, h};
    POINT src = {0, 0};
    POINT pos = {workArea.left, workArea.top};

    BLENDFUNCTION blend = {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
    UpdateLayeredWindow(g_hwnd, screenDc, &pos, &size, memDc, &src, 0, &blend, ULW_ALPHA);
    SetWindowPos(g_hwnd, HWND_BOTTOM, workArea.left, workArea.top, w, h,
                 SWP_NOACTIVATE | SWP_NOOWNERZORDER);

    SelectObject(memDc, oldBitmap);
    DeleteObject(bitmap);
    DeleteDC(memDc);
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

// ── HandleClick ───────────────────────────────────────────────────────────────
// Dispatches widget clicks to the correct action.
//
// WiFi        → ms-actioncenter:controlcenter/wifi
//               Opens the Wi-Fi toggle panel inside Quick Settings.
//
// Hotspot     → ms-actioncenter:controlcenter/MobileHotspot
//               Opens the Mobile Hotspot toggle panel inside Quick Settings.
//               Falls back to ms-settings:network-mobilehotspot if the URI
//               is not handled (Windows 10 / older builds).
//
// Bluetooth   → ms-actioncenter:controlcenter/bluetooth
//               Opens the Bluetooth toggle panel inside Quick Settings.
//
// Accessibility → ms-actioncenter:controlcenter/accessibility
//               Opens the Accessibility toggle panel inside Quick Settings.
//               Falls back to ms-settings:easeofaccess if not handled.
// ─────────────────────────────────────────────────────────────────────────────
void HandleClick(int widgetId) {
    switch (widgetId) {
        case WidgetMusic:
            OpenMusicFolder();
            break;

        case WidgetSearch:
            OpenWindowsSearch();
            break;

        case WidgetPersonalization:
            OpenUri(L"ms-settings:personalization");
            break;

        // ── WiFi ──────────────────────────────────────────────────────────
        // Primary:   ms-actioncenter:controlcenter/wifi
        //   Opens the Quick Settings Wi-Fi panel directly (Windows 11 22H2+).
        // Alternate: ms-controlcenter:wifi
        //   Older Windows 11 builds that use ms-controlcenter instead.
        // Fallback:  ms-settings:network-wifi
        //   Full Settings page — always works.
        case WidgetWifi:
            OpenQuickSettingsDirect(
                L"ms-actioncenter:controlcenter/wifi",
                L"ms-controlcenter:wifi",
                L"ms-settings:network-wifi");
            break;

        // ── Hotspot ───────────────────────────────────────────────────────
        // Primary:   ms-actioncenter:controlcenter/MobileHotspot
        //   Opens the Quick Settings Mobile Hotspot panel directly.
        //   Windows 11 registers this page under "MobileHotspot" (PascalCase).
        //   "controlcenter/true" previously used here only opened the root
        //   Quick Settings flyout, NOT the hotspot sub-panel — that is why
        //   the hotspot widget was not navigating to the right place.
        // Alternate: ms-controlcenter:MobileHotspot
        //   Alternate scheme for older Windows 11 builds.
        // Fallback:  ms-settings:network-mobilehotspot
        //   Full Settings page — always works.
        case WidgetHotspot:
            OpenQuickSettingsDirect(
                L"ms-actioncenter:controlcenter/MobileHotspot",
                L"ms-controlcenter:MobileHotspot",
                L"ms-settings:network-mobilehotspot");
            break;

        // ── Bluetooth ─────────────────────────────────────────────────────
        // Primary:   ms-actioncenter:controlcenter/bluetooth
        // Alternate: ms-controlcenter:bluetooth
        // Fallback:  ms-settings:bluetooth
        case WidgetBluetooth:
            OpenQuickSettingsDirect(
                L"ms-actioncenter:controlcenter/bluetooth",
                L"ms-controlcenter:bluetooth",
                L"ms-settings:bluetooth");
            break;

        // ── Accessibility ─────────────────────────────────────────────────
        // Primary:   ms-actioncenter:controlcenter/accessibility
        //   Opens the Quick Settings Accessibility panel directly.
        //   Previously this already used the correct slug but the
        //   OpenQuickSettingsUri helper was not always reaching it because
        //   the fallback chain was evaluated incorrectly on some builds.
        //   Using OpenQuickSettingsDirect with explicit nullptr guard fixes
        //   this reliably.
        // Alternate: ms-controlcenter:accessibility
        // Fallback:  ms-settings:easeofaccess
        case WidgetAccessibility:
            OpenQuickSettingsDirect(
                L"ms-actioncenter:controlcenter/accessibility",
                L"ms-controlcenter:accessibility",
                L"ms-settings:easeofaccess");
            break;
    }
}

LRESULT CALLBACK WidgetWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_CREATE: {
            MARGINS margins = {-1};
            DwmExtendFrameIntoClientArea(hwnd, &margins);
            SetTimer(hwnd, kTickTimer, kTimerMs, nullptr);
            RefreshVolume();
            RefreshBattery();
            Render();
            return 0;
        }

        case WM_TIMER:
            RefreshVolume();
            RefreshBattery();
            Render();
            return 0;

        case WM_DISPLAYCHANGE:
        case WM_DPICHANGED:
            UpdateScale();
            ApplyDefaultWidgetLayout();
            Render();
            return 0;

        case kSettingsChangedMessage:
        {
            const bool oldVolumeVertical = g_volumeWidgetVertical;
            const bool oldBatteryVertical = g_batteryWidgetVertical;
            const bool oldSearchVertical = g_searchWidgetVertical;
            const bool oldPersonalizationVertical = g_personalizationWidgetVertical;
            LoadSettings();
            ApplyOrientationResizeIfNeeded(oldVolumeVertical,
                                           oldBatteryVertical,
                                           oldSearchVertical,
                                           oldPersonalizationVertical);
            Render();
            return 0;
        }

        case WM_LBUTTONDOWN: {
            PointF pt = LogicalPointFromLParam(lParam);
            RectF slider = VolumeSliderRect();
            if (PtInRectF(slider, pt)) {
                g_draggingSlider = true;
                SetCapture(hwnd);
                SetSystemVolume(g_volumeWidgetVertical
                    ? 1.0f - (pt.Y - slider.Y) / slider.Height
                    : (pt.X - slider.X) / slider.Width);
                Render();
                return 0;
            }

            int hit = HitTestWidget(pt);
            if (hit >= 0) {
                g_dragWidget = hit;
                g_dragDelta = PointF(pt.X - g_widgets[hit].rc.X, pt.Y - g_widgets[hit].rc.Y);
                g_dragStart = pt;
                g_dragMoved = false;
                SetCapture(hwnd);
                return 0;
            }
            return 0;
        }

        case WM_MOUSEMOVE: {
            PointF pt = LogicalPointFromLParam(lParam);
            if (g_draggingSlider) {
                RectF slider = VolumeSliderRect();
                SetSystemVolume(g_volumeWidgetVertical
                    ? 1.0f - (pt.Y - slider.Y) / slider.Height
                    : (pt.X - slider.X) / slider.Width);
                Render();
                return 0;
            }

            if (g_dragWidget >= 0 && (wParam & MK_LBUTTON)) {
                Widget& widget = g_widgets[g_dragWidget];
                if (std::fabs(pt.X - g_dragStart.X) > 3.0f || std::fabs(pt.Y - g_dragStart.Y) > 3.0f) {
                    g_dragMoved = true;
                }
                RectF desktop = LogicalDesktopBounds();
                const float maxX = std::max(0.0f, desktop.Width - widget.rc.Width);
                const float maxY = std::max(0.0f, desktop.Height - widget.rc.Height);
                widget.rc.X = Clamp(pt.X - g_dragDelta.X, 0.0f, maxX);
                widget.rc.Y = Clamp(pt.Y - g_dragDelta.Y, 0.0f, maxY);
                Render();
                return 0;
            }
            return 0;
        }

        case WM_LBUTTONUP: {
            PointF pt = LogicalPointFromLParam(lParam);
            bool wasSlider = g_draggingSlider;
            int releasedWidget = g_dragWidget;
            bool wasMoved = g_dragMoved;
            g_draggingSlider = false;
            g_dragWidget = -1;
            g_dragMoved = false;
            ReleaseCapture();

            if (!wasSlider && !wasMoved && releasedWidget >= 0 && PtInWidget(g_widgets[releasedWidget], pt)) {
                HandleClick(g_widgets[releasedWidget].id);
            }
            return 0;
        }

        case WM_NCHITTEST: {
            POINT pt = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
            return HitTestWidget(LogicalPointFromScreen(pt)) >= 0 ? HTCLIENT : HTTRANSPARENT;
        }

        case WM_DESTROY:
            KillTimer(hwnd, kTickTimer);
            g_hwnd = nullptr;
            PostQuitMessage(0);
            return 0;
    }

    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

DWORD WINAPI UiThreadProc(LPVOID) {
    CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);

    GdiplusStartupInput gdiplusInput;
    GdiplusStartup(&g_gdiplusToken, &gdiplusInput, nullptr);

    UpdateScale();
    ApplyDefaultWidgetLayout();
    HINSTANCE instance = GetModuleHandleW(nullptr);

    WNDCLASSEXW wc = {};
    wc.cbSize = sizeof(wc);
    wc.hInstance = instance;
    wc.lpfnWndProc = WidgetWndProc;
    wc.lpszClassName = kClassName;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    RegisterClassExW(&wc);

    RECT workArea = DesktopWorkArea();
    int w = workArea.right - workArea.left;
    int h = workArea.bottom - workArea.top;
    int x = workArea.left;
    int y = workArea.top;

    g_hwnd = CreateWindowExW(
        WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
        kClassName,
        L"Windhawk Desktop Glass Widgets",
        WS_POPUP,
        x,
        y,
        w,
        h,
        nullptr,
        nullptr,
        instance,
        nullptr);

    if (g_hwnd) {
        SetWindowPos(g_hwnd, HWND_BOTTOM, x, y, w, h, SWP_NOACTIVATE | SWP_NOOWNERZORDER);
        ShowWindow(g_hwnd, SW_SHOWNOACTIVATE);
        Render();
    }

    MSG msg;
    while (!g_unloading && GetMessageW(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    if (g_hwnd) {
        DestroyWindow(g_hwnd);
    }

    UnregisterClassW(kClassName, instance);
    GdiplusShutdown(g_gdiplusToken);
    CoUninitialize();
    return 0;
}

}  // namespace

BOOL WhTool_ModInit() {
    g_unloading = false;
    LoadSettings();
    g_uiThread = CreateThread(nullptr, 0, UiThreadProc, nullptr, 0, &g_uiThreadId);
    return g_uiThread != nullptr;
}

void WhTool_ModUninit() {
    g_unloading = true;
    if (g_uiThreadId) {
        PostThreadMessageW(g_uiThreadId, WM_QUIT, 0, 0);
    }

    if (g_uiThread) {
        WaitForSingleObject(g_uiThread, 3000);
        CloseHandle(g_uiThread);
        g_uiThread = nullptr;
    }
}


bool g_isToolModProcessLauncher;
HANDLE g_toolModProcessMutex;

void WINAPI EntryPoint_Hook() {
    Wh_Log(L">");
    ExitThread(0);
}

BOOL Wh_ModInit() {
    DWORD sessionId;
    if (ProcessIdToSessionId(GetCurrentProcessId(), &sessionId) &&
        sessionId == 0) {
        return FALSE;
    }

    bool isExcluded = false;
    bool isToolModProcess = false;
    bool isCurrentToolModProcess = false;
    int argc;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLine(), &argc);
    if (!argv) {
        Wh_Log(L"CommandLineToArgvW failed");
        return FALSE;
    }

    for (int i = 1; i < argc; i++) {
        if (wcscmp(argv[i], L"-service") == 0 ||
            wcscmp(argv[i], L"-service-start") == 0 ||
            wcscmp(argv[i], L"-service-stop") == 0) {
            isExcluded = true;
            break;
        }
    }

    for (int i = 1; i < argc - 1; i++) {
        if (wcscmp(argv[i], L"-tool-mod") == 0) {
            isToolModProcess = true;
            if (wcscmp(argv[i + 1], WH_MOD_ID) == 0) {
                isCurrentToolModProcess = true;
            }
            break;
        }
    }

    LocalFree(argv);

    if (isExcluded) {
        return FALSE;
    }

    if (isCurrentToolModProcess) {
        g_toolModProcessMutex =
            CreateMutex(nullptr, TRUE, L"windhawk-tool-mod_" WH_MOD_ID);
        if (!g_toolModProcessMutex) {
            Wh_Log(L"CreateMutex failed");
            ExitProcess(1);
        }

        if (GetLastError() == ERROR_ALREADY_EXISTS) {
            Wh_Log(L"Tool mod already running (%s)", WH_MOD_ID);
            ExitProcess(1);
        }

        if (!WhTool_ModInit()) {
            ExitProcess(1);
        }

        IMAGE_DOS_HEADER* dosHeader =
            (IMAGE_DOS_HEADER*)GetModuleHandle(nullptr);
        IMAGE_NT_HEADERS* ntHeaders =
            (IMAGE_NT_HEADERS*)((BYTE*)dosHeader + dosHeader->e_lfanew);

        DWORD entryPointRVA = ntHeaders->OptionalHeader.AddressOfEntryPoint;
        void* entryPoint = (BYTE*)dosHeader + entryPointRVA;

        Wh_SetFunctionHook(entryPoint, (void*)EntryPoint_Hook, nullptr);
        return TRUE;
    }

    if (isToolModProcess) {
        return FALSE;
    }

    g_isToolModProcessLauncher = true;
    return TRUE;
}

void Wh_ModAfterInit() {
    if (!g_isToolModProcessLauncher) {
        return;
    }

    WCHAR currentProcessPath[MAX_PATH];
    switch (GetModuleFileName(nullptr, currentProcessPath,
                              ARRAYSIZE(currentProcessPath))) {
        case 0:
        case ARRAYSIZE(currentProcessPath):
            Wh_Log(L"GetModuleFileName failed");
            return;
    }

    WCHAR
    commandLine[MAX_PATH + 2 +
                (sizeof(L" -tool-mod \"" WH_MOD_ID "\"") / sizeof(WCHAR)) - 1];
    swprintf_s(commandLine, L"\"%s\" -tool-mod \"%s\"", currentProcessPath,
               WH_MOD_ID);

    HMODULE kernelModule = GetModuleHandle(L"kernelbase.dll");
    if (!kernelModule) {
        kernelModule = GetModuleHandle(L"kernel32.dll");
        if (!kernelModule) {
            Wh_Log(L"No kernelbase.dll/kernel32.dll");
            return;
        }
    }

    using CreateProcessInternalW_t = BOOL(WINAPI*)(
        HANDLE hUserToken, LPCWSTR lpApplicationName, LPWSTR lpCommandLine,
        LPSECURITY_ATTRIBUTES lpProcessAttributes,
        LPSECURITY_ATTRIBUTES lpThreadAttributes, WINBOOL bInheritHandles,
        DWORD dwCreationFlags, LPVOID lpEnvironment, LPCWSTR lpCurrentDirectory,
        LPSTARTUPINFOW lpStartupInfo,
        LPPROCESS_INFORMATION lpProcessInformation,
        PHANDLE hRestrictedUserToken);
    CreateProcessInternalW_t pCreateProcessInternalW =
        (CreateProcessInternalW_t)GetProcAddress(kernelModule,
                                                 "CreateProcessInternalW");
    if (!pCreateProcessInternalW) {
        Wh_Log(L"No CreateProcessInternalW");
        return;
    }

    STARTUPINFO si{
        .cb = sizeof(STARTUPINFO),
        .dwFlags = STARTF_FORCEOFFFEEDBACK,
    };
    PROCESS_INFORMATION pi;
    if (!pCreateProcessInternalW(nullptr, currentProcessPath, commandLine,
                                 nullptr, nullptr, FALSE, NORMAL_PRIORITY_CLASS,
                                 nullptr, nullptr, &si, &pi, nullptr)) {
        Wh_Log(L"CreateProcess failed");
        return;
    }

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}

void Wh_ModSettingsChanged() {
    if (g_isToolModProcessLauncher) {
        return;
    }

    if (g_hwnd) {
        PostMessageW(g_hwnd, kSettingsChangedMessage, 0, 0);
    } else {
        LoadSettings();
    }
}

void Wh_ModUninit() {
    if (g_isToolModProcessLauncher) {
        return;
    }

    WhTool_ModUninit();
    ExitProcess(0);
}
