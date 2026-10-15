// ==WindhawkMod==
// @id              premium-taskbar-net-speed
// @name            Premium Taskbar Net Speed Glass
// @description     A customizable Windows 11 taskbar widget for live download and upload speeds, with compact sizing and saved positioning.
// @version         1.0
// @author          FouadMODS
// @github          https://github.com/modsfouad
// @twitter         https://twitter.com/modsfouad
// @include         windhawk.exe
// @compilerOptions -DWIN32_LEAN_AND_MEAN -liphlpapi -lgdi32 -luser32 -lshell32 -ladvapi32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Premium Taskbar Net Speed Glass

See live download and upload speeds at a glance with a compact Windows 11 taskbar widget. Choose a polished color theme, tune its appearance, and drag it to your preferred position.

## Theme previews

Illustrative examples; displayed speed values and desktop appearance vary by system.

**Signature Glass**

![Signature Glass theme preview](https://i.imgur.com/fYlHDXE.png)

**Arctic Silver**

![Arctic Silver theme preview](https://i.imgur.com/H67mqNt.png)

**Rose Gold**

![Rose Gold theme preview](https://i.imgur.com/MswsNI2.png)

## Features

- Displays live download and upload rates on separate lines.
- Automatically selects the active internet route, or sums active adapters; an optional name filter can target Wi-Fi or Ethernet.
- Switches between automatic byte-per-second and bit-per-second units.
- Fits the widget to the displayed values to keep the layout compact.
- Includes Signature Glass, Midnight Gold, Arctic Silver, Rose Gold, and Emerald Obsidian themes, plus a Custom option.
- Offers Classic, Chevron, and Solid arrow styles, drawn as vector shapes for consistent visibility across fonts.
- Fine-tunes background opacity, border opacity, arrow spacing, font, size, and colors.
- Drag anywhere on the widget to move it. Its position is saved, and a double-click restores automatic tray placement.
- Scales with display DPI and can hide during full-screen presentation or Direct3D applications.

## Getting started

1. Compile and enable the mod in Windhawk.
2. Open **Settings** and choose a look under **Template**.
3. Choose an arrow style, then adjust the sampling interval, smoothing, units, or network adapter as needed.
4. Drag the widget to reposition it. Double-click it to return to its automatic position beside the notification area.

## Appearance

The selected template provides coordinated text, arrow, background, and border colors. Choose **Arrow style** independently to use Classic, Chevron, or Solid symbols. Use **Custom** to configure appearance settings individually. Background opacity, border opacity, and the arrow-to-value gap remain adjustable with any template.

## Notes

This mod draws a floating overlay next to the Windows 11 notification area; it does not reserve taskbar space. If the widget overlaps another taskbar item, adjust **Horizontal offset** or **Gap from system tray** in Settings. Designed primarily for a horizontal taskbar; the automatic tray position uses the primary taskbar.

## Alternatives

This mod is a separate presentation-focused overlay with premium colorways, vector arrow styles, compact sizing, and a saved drag position. For a simpler taskbar-docked readout, see [Taskbar Network Speed Indicator](https://github.com/ramensoftware/windhawk-mods/blob/main/mods/net-speed-taskbar.wh.cpp). For speeds integrated into the clock, see [Taskbar Clock Customization](https://windhawk.net/mods/taskbar-clock-customization).
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- appearanceTemplate: custom
  $name: Template
  $description: Choose a premium colorway and save. Each style has matched text, arrows, tinted glass and fine border; layout and your saved drag position remain adjustable.
  $options:
    - custom: Custom
    - glass: Signature Glass (Aqua & Mint)
    - midnightGold: Midnight Gold
    - arcticSilver: Arctic Silver
    - roseGold: Rose Gold
    - emerald: Emerald Obsidian

- updateIntervalMs: 1000
  $name: Update interval
  $description: Network sampling interval in milliseconds. 500-2000 ms is recommended.

- smoothingPercent: 40
  $name: Smoothing
  $description: 0 = raw speed, 100 = very smooth. 35-50 gives a polished stable readout.

- units: autoBytes
  $name: Units
  $options:
    - autoBytes: Auto B/s, KB/s, MB/s, GB/s
    - autoBits: Auto bps, Kbps, Mbps, Gbps

- adapterMode: activeRoute
  $name: Adapter mode
  $description: Active route normally gives the cleanest internet-speed reading.
  $options:
    - activeRoute: Active internet route
    - all: Sum all active adapters

- adapterName: ""
  $name: Adapter name filter
  $description: Optional partial adapter alias/description, e.g. Wi-Fi, Ethernet, Intel, Realtek. Leave empty for automatic selection.

- width: 108
  $name: Maximum width
  $description: Maximum widget width in logical pixels. The pill automatically shrinks to fit the current speed text.

- height: 38
  $name: Height
  $description: Widget height in logical pixels.

- horizontalOffset: 0
  $name: Horizontal offset
  $description: Fine tune position relative to the left edge of the notification area. Negative moves left, positive moves right.

- verticalOffset: 0
  $name: Vertical offset
  $description: Fine tune vertical position.

- gapFromTray: 6
  $name: Gap from system tray
  $description: Space between the meter and the notification area.

- fontSize: 11
  $name: Font size
  $description: Segoe UI Variable font size in logical pixels.

- fontWeight: semibold
  $name: Font weight
  $description: Used only with the Custom template.
  $options:
    - normal: Normal
    - medium: Medium
    - semibold: Semi-bold
    - bold: Bold

- valueColor: auto
  $name: Speed text color
  $description: Used only with the Custom template.
  $options:
    - auto: Automatic light/dark
    - white: White
    - black: Black
    - cyan: Cyan
    - green: Green

- accentArrows: true
  $name: Accent arrow colors
  $description: Used only with the Custom template. Cyan download arrow and mint upload arrow; disable for monochrome.

- arrowStyle: solid
  $name: Arrow style
  $description: Choose a clear vector arrow shape that renders independently of font glyph support.
  $options:
    - solid: Bold Solid (recommended)
    - classic: Classic (shaft and arrowhead)
    - chevron: Minimal Chevron

- backgroundStyle: glass
  $name: Background
  $description: Used only with the Custom template.
  $options:
    - transparent: Transparent
    - glass: Subtle glass pill
    - solid: Stronger pill

- backgroundOpacity: 100
  $name: Background opacity
  $description: Reduce this to make the tinted glass more see-through. 0 makes its fill fully transparent.

- borderOpacity: 100
  $name: Border opacity
  $description: Adjust or hide the fine outline around premium templates.

- arrowValueGap: 2
  $name: Arrow to value gap
  $description: Space between each direction arrow and its speed value, in logical pixels.

- cornerRadius: 10
  $name: Corner radius
  $description: Used by the glass/solid background. (Used only with the Custom template.)
- hideInFullscreen: true
  $name: Hide in full screen
  $description: Hide the meter during detected Direct3D full-screen or presentation mode.

- fallbackRightOffset: 430
  $name: Fallback right offset
  $description: Used only if Windows doesn't expose the notification-area rectangle.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <netioapi.h>
#include <shellapi.h>
#include <cwchar>
#include <cwctype>
#include <algorithm>
#include <atomic>
#include <cstdint>

namespace {

constexpr UINT_PTR TIMER_SAMPLE = 1;
constexpr UINT_PTR TIMER_POSITION = 2;
constexpr UINT_PTR TIMER_DRAG = 3;
constexpr UINT WM_APP_SETTINGS = WM_APP + 25;

struct Settings {
    UINT updateIntervalMs = 1000;
    int smoothingPercent = 40;
    bool useBits = false;

    enum class AdapterMode {
        ActiveRoute,
        All
    } adapterMode = AdapterMode::ActiveRoute;

    wchar_t adapterName[128] = {};

    int width = 108;
    int height = 38;
    int horizontalOffset = 0;
    int verticalOffset = 0;
    int gapFromTray = 6;
    int fontSize = 11;
    int fontWeight = FW_SEMIBOLD;

    enum class ValueColor {
        Auto,
        White,
        Black,
        Cyan,
        Green
    } valueColor = ValueColor::Auto;

    bool accentArrows = true;

    enum class ArrowStyle { Classic, Chevron, Solid };
    ArrowStyle arrowStyle = ArrowStyle::Solid;

    enum class PremiumPalette { Signature, MidnightGold, ArcticSilver, RoseGold, Emerald };
    PremiumPalette premiumPalette = PremiumPalette::Signature;

    enum class BackgroundStyle {
        Transparent,
        Glass,
        Solid
    } backgroundStyle = BackgroundStyle::Transparent;

    int backgroundOpacity = 100;
    int borderOpacity = 100;
    int arrowValueGap = 2;
    int cornerRadius = 10;
    bool hideInFullscreen = true;
    int fallbackRightOffset = 430;
};

SRWLOCK g_settingsLock = SRWLOCK_INIT;
Settings g_settings;

std::atomic<HWND> g_hwnd{nullptr};
HANDLE g_thread = nullptr;
std::atomic<DWORD> g_threadId{0};
HANDLE g_queueReady = nullptr;
HWINEVENTHOOK g_foregroundHook = nullptr;

ULONGLONG g_lastBytesIn = 0;
ULONGLONG g_lastBytesOut = 0;
ULONGLONG g_lastInterfaceSignature = 0;
ULONGLONG g_lastSampleTick = 0;
bool g_haveBaseline = false;
double g_displayDown = 0.0;
double g_displayUp = 0.0;

UINT g_lastDpi = 96;
int g_lastWidthPx = 0;
int g_lastHeightPx = 0;

// Accessed only by the widget thread. Saved atomically as one storage value.
bool g_isDragging = false;
bool g_dragMoved = false;
bool g_freePosition = false;
POINT g_position = {};
POINT g_dragAnchor = {};
POINT g_dragStart = {};
struct SavedPosition {
    DWORD version;
    LONG x;
    LONG y;
};

static void LoadPosition() {
    SavedPosition saved = {};
    g_freePosition =
        Wh_GetBinaryValue(L"dragPosition", &saved, sizeof(saved)) == sizeof(saved) &&
        saved.version == 1;
    if (g_freePosition) g_position = {saved.x, saved.y};
}

static void SavePosition() {
    if (!g_freePosition) return;
    SavedPosition saved = {1, g_position.x, g_position.y};
    if (!Wh_SetBinaryValue(L"dragPosition", &saved, sizeof(saved))) {
        Wh_Log(L"Couldn't save the dragged widget position");
    }
}

struct RGBA {
    BYTE r;
    BYTE g;
    BYTE b;
    BYTE a;
};

static int ScaleForDpi(int value, UINT dpi) {
    return MulDiv(value, static_cast<int>(dpi), 96);
}

static int GetArrowColumnWidthPx(
    int fontSize, Settings::ArrowStyle style, UINT dpi) {
    int logicalWidth = std::max(7, fontSize * 3 / 4);
    if (style == Settings::ArrowStyle::Solid) {
        logicalWidth += 2;
    }
    return ScaleForDpi(logicalWidth, dpi);
}

static bool EqualsI(PCWSTR a, PCWSTR b) {
    return a && b && _wcsicmp(a, b) == 0;
}

static void CopySettingString(PCWSTR settingName, wchar_t* out, size_t outCount) {
    out[0] = L'\0';
    PCWSTR value = Wh_GetStringSetting(settingName);
    if (value) {
        wcsncpy_s(out, outCount, value, _TRUNCATE);
        Wh_FreeStringSetting(value);
    }
}

static void ApplyAppearanceTemplate(Settings& s, PCWSTR name) {
    // Custom keeps the existing individual controls in charge.
    s.premiumPalette = Settings::PremiumPalette::Signature;
    if (EqualsI(name, L"midnightGold")) {
        s.premiumPalette = Settings::PremiumPalette::MidnightGold;
        s.backgroundStyle = Settings::BackgroundStyle::Solid;
        s.valueColor = Settings::ValueColor::White;
        s.accentArrows = true;
        s.fontWeight = FW_SEMIBOLD;
        s.cornerRadius = 10;
    } else if (EqualsI(name, L"arcticSilver")) {
        s.premiumPalette = Settings::PremiumPalette::ArcticSilver;
        s.backgroundStyle = Settings::BackgroundStyle::Glass;
        s.valueColor = Settings::ValueColor::White;
        s.accentArrows = true;
        s.fontWeight = FW_SEMIBOLD;
        s.cornerRadius = 11;
    } else if (EqualsI(name, L"roseGold")) {
        s.premiumPalette = Settings::PremiumPalette::RoseGold;
        s.backgroundStyle = Settings::BackgroundStyle::Solid;
        s.valueColor = Settings::ValueColor::White;
        s.accentArrows = true;
        s.fontWeight = FW_SEMIBOLD;
        s.cornerRadius = 10;
    } else if (EqualsI(name, L"emerald")) {
        s.premiumPalette = Settings::PremiumPalette::Emerald;
        s.backgroundStyle = Settings::BackgroundStyle::Solid;
        s.valueColor = Settings::ValueColor::White;
        s.accentArrows = true;
        s.fontWeight = FW_SEMIBOLD;
        s.cornerRadius = 10;
    } else if (EqualsI(name, L"glass")) {
        s.premiumPalette = Settings::PremiumPalette::Signature;
        s.backgroundStyle = Settings::BackgroundStyle::Glass;
        s.valueColor = Settings::ValueColor::Auto;
        s.accentArrows = true;
        s.fontWeight = FW_SEMIBOLD;
        s.cornerRadius = 10;
    }
}
static Settings ReadSettings() {
    Settings s;

    int interval = Wh_GetIntSetting(L"updateIntervalMs");
    s.updateIntervalMs = static_cast<UINT>(std::clamp(interval, 250, 10000));

    s.smoothingPercent = std::clamp(
        static_cast<int>(Wh_GetIntSetting(L"smoothingPercent")), 0, 100);

    PCWSTR units = Wh_GetStringSetting(L"units");
    if (units) {
        s.useBits = EqualsI(units, L"autoBits");
        Wh_FreeStringSetting(units);
    }

    PCWSTR adapterMode = Wh_GetStringSetting(L"adapterMode");
    if (adapterMode) {
        s.adapterMode = EqualsI(adapterMode, L"all")
                            ? Settings::AdapterMode::All
                            : Settings::AdapterMode::ActiveRoute;
        Wh_FreeStringSetting(adapterMode);
    }

    CopySettingString(L"adapterName", s.adapterName, ARRAYSIZE(s.adapterName));

    s.width = std::clamp(static_cast<int>(Wh_GetIntSetting(L"width")), 88, 260);
    s.height = std::clamp(static_cast<int>(Wh_GetIntSetting(L"height")), 28, 80);
    s.horizontalOffset = std::clamp(
        static_cast<int>(Wh_GetIntSetting(L"horizontalOffset")), -800, 800);
    s.verticalOffset = std::clamp(
        static_cast<int>(Wh_GetIntSetting(L"verticalOffset")), -100, 100);
    s.gapFromTray = std::clamp(
        static_cast<int>(Wh_GetIntSetting(L"gapFromTray")), 0, 80);
    s.fontSize = std::clamp(
        static_cast<int>(Wh_GetIntSetting(L"fontSize")), 8, 24);

    PCWSTR weight = Wh_GetStringSetting(L"fontWeight");
    if (weight) {
        if (EqualsI(weight, L"normal")) s.fontWeight = FW_NORMAL;
        else if (EqualsI(weight, L"medium")) s.fontWeight = FW_MEDIUM;
        else if (EqualsI(weight, L"bold")) s.fontWeight = FW_BOLD;
        else s.fontWeight = FW_SEMIBOLD;
        Wh_FreeStringSetting(weight);
    }

    PCWSTR valueColor = Wh_GetStringSetting(L"valueColor");
    if (valueColor) {
        if (EqualsI(valueColor, L"white")) s.valueColor = Settings::ValueColor::White;
        else if (EqualsI(valueColor, L"black")) s.valueColor = Settings::ValueColor::Black;
        else if (EqualsI(valueColor, L"cyan")) s.valueColor = Settings::ValueColor::Cyan;
        else if (EqualsI(valueColor, L"green")) s.valueColor = Settings::ValueColor::Green;
        else s.valueColor = Settings::ValueColor::Auto;
        Wh_FreeStringSetting(valueColor);
    }

    s.accentArrows = Wh_GetIntSetting(L"accentArrows") != 0;

    PCWSTR arrowStyle = Wh_GetStringSetting(L"arrowStyle");
    if (arrowStyle) {
        if (EqualsI(arrowStyle, L"chevron")) {
            s.arrowStyle = Settings::ArrowStyle::Chevron;
        } else if (EqualsI(arrowStyle, L"solid")) {
            s.arrowStyle = Settings::ArrowStyle::Solid;
        } else {
            s.arrowStyle = Settings::ArrowStyle::Classic;
        }
        Wh_FreeStringSetting(arrowStyle);
    }

    PCWSTR bg = Wh_GetStringSetting(L"backgroundStyle");
    if (bg) {
        if (EqualsI(bg, L"glass")) s.backgroundStyle = Settings::BackgroundStyle::Glass;
        else if (EqualsI(bg, L"solid")) s.backgroundStyle = Settings::BackgroundStyle::Solid;
        else s.backgroundStyle = Settings::BackgroundStyle::Transparent;
        Wh_FreeStringSetting(bg);
    }

    s.backgroundOpacity = std::clamp(
        static_cast<int>(Wh_GetIntSetting(L"backgroundOpacity")), 0, 100);
    s.borderOpacity = std::clamp(
        static_cast<int>(Wh_GetIntSetting(L"borderOpacity")), 0, 100);
    s.arrowValueGap = std::clamp(
        static_cast<int>(Wh_GetIntSetting(L"arrowValueGap")), 0, 16);
    s.cornerRadius = std::clamp(
        static_cast<int>(Wh_GetIntSetting(L"cornerRadius")), 0, 30);
    s.hideInFullscreen = Wh_GetIntSetting(L"hideInFullscreen") != 0;
    s.fallbackRightOffset = std::clamp(
        static_cast<int>(Wh_GetIntSetting(L"fallbackRightOffset")), 100, 1200);

    PCWSTR appearanceTemplate = Wh_GetStringSetting(L"appearanceTemplate");
    ApplyAppearanceTemplate(s, appearanceTemplate);
    if (appearanceTemplate) Wh_FreeStringSetting(appearanceTemplate);

    return s;
}

static Settings GetSettingsCopy() {
    AcquireSRWLockShared(&g_settingsLock);
    Settings copy = g_settings;
    ReleaseSRWLockShared(&g_settingsLock);
    return copy;
}

static void ReloadSettings() {
    Settings s = ReadSettings();
    AcquireSRWLockExclusive(&g_settingsLock);
    g_settings = s;
    ReleaseSRWLockExclusive(&g_settingsLock);
}

static bool IsSystemLightTheme() {
    DWORD value = 0;
    DWORD size = sizeof(value);
    HKEY key = nullptr;

    if (RegOpenKeyExW(
            HKEY_CURRENT_USER,
            L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
            0, KEY_QUERY_VALUE, &key) == ERROR_SUCCESS) {
        RegQueryValueExW(
            key, L"SystemUsesLightTheme", nullptr, nullptr,
            reinterpret_cast<BYTE*>(&value), &size);
        RegCloseKey(key);
    }

    return value != 0;
}

static RGBA GetValueColor(const Settings& s, bool lightTheme) {
    switch (s.premiumPalette) {
        case Settings::PremiumPalette::MidnightGold: return {255, 244, 220, 255};
        case Settings::PremiumPalette::ArcticSilver: return {226, 247, 255, 255};
        case Settings::PremiumPalette::RoseGold: return {255, 226, 220, 255};
        case Settings::PremiumPalette::Emerald: return {222, 255, 239, 255};
        case Settings::PremiumPalette::Signature: break;
    }
    switch (s.valueColor) {
        case Settings::ValueColor::White: return {245, 245, 245, 255};
        case Settings::ValueColor::Black: return {24, 24, 24, 255};
        case Settings::ValueColor::Cyan:  return {92, 214, 255, 255};
        case Settings::ValueColor::Green: return {113, 230, 164, 255};
        case Settings::ValueColor::Auto:
        default:
            return lightTheme ? RGBA{28, 28, 28, 255}
                              : RGBA{245, 245, 245, 255};
    }
}

static bool ContainsInsensitive(PCWSTR haystack, PCWSTR needle) {
    if (!needle || !*needle) {
        return true;
    }
    if (!haystack) {
        return false;
    }

    for (const wchar_t* h = haystack; *h; ++h) {
        const wchar_t* p = h;
        const wchar_t* n = needle;
        while (*p && *n && towlower(*p) == towlower(*n)) {
            ++p;
            ++n;
        }
        if (!*n) {
            return true;
        }
    }
    return false;
}

static bool IsUsableInterface(const MIB_IF_ROW2& row) {
    if (row.OperStatus != IfOperStatusUp ||
        row.InterfaceAndOperStatusFlags.FilterInterface) {
        return false;
    }

    return row.Type != IF_TYPE_SOFTWARE_LOOPBACK &&
           row.Type != IF_TYPE_TUNNEL;
}

static bool GetNetworkBytesAll(
    const Settings& s,
    ULONGLONG* bytesIn,
    ULONGLONG* bytesOut,
    ULONGLONG* interfaceSignature) {
    *bytesIn = 0;
    *bytesOut = 0;
    *interfaceSignature = 1469598103934665603ULL;

    PMIB_IF_TABLE2 table = nullptr;
    if (GetIfTable2(&table) != NO_ERROR || !table) {
        return false;
    }

    const bool hasFilter = s.adapterName[0] != L'\0';
    bool found = false;
    for (ULONG i = 0; i < table->NumEntries; ++i) {
        const MIB_IF_ROW2& row = table->Table[i];
        if (!IsUsableInterface(row)) {
            continue;
        }

        if (hasFilter) {
            if (!ContainsInsensitive(row.Alias, s.adapterName) &&
                !ContainsInsensitive(row.Description, s.adapterName)) {
                continue;
            }
        } else if (!row.InterfaceAndOperStatusFlags.HardwareInterface) {
            continue;
        }

        *bytesIn += row.InOctets;
        *bytesOut += row.OutOctets;
        *interfaceSignature ^=
            static_cast<ULONGLONG>(row.InterfaceIndex) * 0x9E3779B97F4A7C15ULL;
        found = true;
    }

    FreeMibTable(table);
    return found;
}

static bool GetNetworkBytesActiveRoute(
    const Settings& s,
    ULONGLONG* bytesIn,
    ULONGLONG* bytesOut,
    ULONGLONG* interfaceSignature) {
    *bytesIn = 0;
    *bytesOut = 0;
    *interfaceSignature = 0;

    if (s.adapterName[0]) {
        return GetNetworkBytesAll(s, bytesIn, bytesOut, interfaceSignature);
    }

    DWORD ifIndex = 0;
    if (GetBestInterface(0x08080808, &ifIndex) != NO_ERROR || ifIndex == 0) {
        return false;
    }

    MIB_IF_ROW2 row = {};
    row.InterfaceIndex = ifIndex;
    if (GetIfEntry2(&row) != NO_ERROR || !IsUsableInterface(row)) {
        return false;
    }

    *bytesIn = row.InOctets;
    *bytesOut = row.OutOctets;
    *interfaceSignature = row.InterfaceIndex;
    return true;
}

static bool GetNetworkBytes(
    const Settings& s,
    ULONGLONG* bytesIn,
    ULONGLONG* bytesOut,
    ULONGLONG* interfaceSignature) {
    if (s.adapterMode == Settings::AdapterMode::ActiveRoute &&
        GetNetworkBytesActiveRoute(
            s, bytesIn, bytesOut, interfaceSignature)) {
        return true;
    }

    return GetNetworkBytesAll(s, bytesIn, bytesOut, interfaceSignature);
}

static void FormatSpeed(double bytesPerSec, bool bits, wchar_t* out, size_t outCount) {
    if (bytesPerSec < 0.0) {
        bytesPerSec = 0.0;
    }

    double value = bits ? bytesPerSec * 8.0 : bytesPerSec;
    const double base = bits ? 1000.0 : 1024.0;
    const wchar_t* unitsBytes[] = {L"B/s", L"KB/s", L"MB/s", L"GB/s"};
    const wchar_t* unitsBits[] = {L"bps", L"Kbps", L"Mbps", L"Gbps"};

    int unit = 0;
    while (value >= base && unit < 3) {
        value /= base;
        ++unit;
    }

    const wchar_t* unitText = bits ? unitsBits[unit] : unitsBytes[unit];

    if (unit == 0) {
        swprintf_s(out, outCount, L"%.0f %s", value, unitText);
    } else if (value < 10.0) {
        swprintf_s(out, outCount, L"%.2f %s", value, unitText);
    } else if (value < 100.0) {
        swprintf_s(out, outCount, L"%.1f %s", value, unitText);
    } else {
        swprintf_s(out, outCount, L"%.0f %s", value, unitText);
    }
}

// Fit the glass pill to its widest current speed value plus a small edge margin.
// The configured width acts as a cap, so even the longest unit stays bounded.
static int GetCompactWidgetWidthPx(const Settings& s, UINT dpi) {
    wchar_t downText[40] = {};
    wchar_t upText[40] = {};
    FormatSpeed(g_displayDown, s.useBits, downText, ARRAYSIZE(downText));
    FormatSpeed(g_displayUp, s.useBits, upText, ARRAYSIZE(upText));

    HDC dc = GetDC(nullptr);
    if (!dc) return ScaleForDpi(s.width, dpi);

    int fontPx = ScaleForDpi(s.fontSize, dpi);
    HFONT font = CreateFontW(-fontPx, 0, 0, 0, s.fontWeight,
        FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
        CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY,
        DEFAULT_PITCH | FF_SWISS, L"Segoe UI Variable Text");
    if (!font) font = CreateFontW(-fontPx, 0, 0, 0, s.fontWeight,
        FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
        CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY,
        DEFAULT_PITCH | FF_SWISS, L"Segoe UI");
    if (!font) {
        ReleaseDC(nullptr, dc);
        return ScaleForDpi(s.width, dpi);
    }

    HFONT oldFont = static_cast<HFONT>(SelectObject(dc, font));
    SIZE downSize = {}, upSize = {};
    GetTextExtentPoint32W(dc, downText, lstrlenW(downText), &downSize);
    GetTextExtentPoint32W(dc, upText, lstrlenW(upText), &upSize);
    SelectObject(dc, oldFont);
    DeleteObject(font);
    ReleaseDC(nullptr, dc);

    const int padLeft = ScaleForDpi(5, dpi);
    const int arrowGap = ScaleForDpi(s.arrowValueGap, dpi);
    const int padRight = ScaleForDpi(5, dpi);
    const int arrowWidth = GetArrowColumnWidthPx(s.fontSize, s.arrowStyle, dpi);
    const int textWidth = std::max(downSize.cx, upSize.cx);
    const int contentWidth = padLeft + arrowWidth + arrowGap + textWidth + padRight;
    const int minimumWidth = ScaleForDpi(56, dpi);
    return std::min(ScaleForDpi(s.width, dpi),
                    std::max(minimumWidth, contentWidth));
}
static bool IsFullscreenSuppressed(const Settings& s) {
    if (!s.hideInFullscreen) {
        return false;
    }

    QUERY_USER_NOTIFICATION_STATE state = QUNS_ACCEPTS_NOTIFICATIONS;
    if (FAILED(SHQueryUserNotificationState(&state))) {
        return false;
    }

    return state == QUNS_RUNNING_D3D_FULL_SCREEN ||
           state == QUNS_PRESENTATION_MODE ||
           state == QUNS_BUSY;
}

static bool PointInsideRoundedRect(
    int x, int y, int width, int height, int radius) {
    if (radius <= 0) {
        return x >= 0 && x < width && y >= 0 && y < height;
    }

    radius = std::min(radius, std::min(width, height) / 2);

    if (x >= radius && x < width - radius) return true;
    if (y >= radius && y < height - radius) return true;

    int cx = x < radius ? radius - 1 : width - radius;
    int cy = y < radius ? radius - 1 : height - radius;
    int dx = x - cx;
    int dy = y - cy;

    return dx * dx + dy * dy <= radius * radius;
}

static void AlphaCompositePixel(DWORD* dstPixel, RGBA src, BYTE coverage = 255) {
    BYTE sa = static_cast<BYTE>((static_cast<unsigned>(src.a) * coverage) / 255);
    if (sa == 0) return;

    BYTE sb = static_cast<BYTE>((static_cast<unsigned>(src.b) * sa) / 255);
    BYTE sg = static_cast<BYTE>((static_cast<unsigned>(src.g) * sa) / 255);
    BYTE sr = static_cast<BYTE>((static_cast<unsigned>(src.r) * sa) / 255);

    DWORD d = *dstPixel;
    BYTE db = static_cast<BYTE>(d & 0xFF);
    BYTE dg = static_cast<BYTE>((d >> 8) & 0xFF);
    BYTE dr = static_cast<BYTE>((d >> 16) & 0xFF);
    BYTE da = static_cast<BYTE>((d >> 24) & 0xFF);

    unsigned inv = 255 - sa;

    BYTE ob = static_cast<BYTE>(sb + (db * inv) / 255);
    BYTE og = static_cast<BYTE>(sg + (dg * inv) / 255);
    BYTE or_ = static_cast<BYTE>(sr + (dr * inv) / 255);
    BYTE oa = static_cast<BYTE>(sa + (da * inv) / 255);

    *dstPixel = (static_cast<DWORD>(oa) << 24) |
                (static_cast<DWORD>(or_) << 16) |
                (static_cast<DWORD>(og) << 8) |
                ob;
}

static void DrawRoundedBackground(
    DWORD* pixels,
    int width,
    int height,
    int radius,
    RGBA fill,
    RGBA border) {
    if (!pixels) return;

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            if (!PointInsideRoundedRect(x, y, width, height, radius)) {
                continue;
            }

            bool inner =
                x > 0 && y > 0 && x < width - 1 && y < height - 1 &&
                PointInsideRoundedRect(
                    x - 1, y - 1, width - 2, height - 2,
                    std::max(0, radius - 1));

            AlphaCompositePixel(
                &pixels[y * width + x],
                inner ? fill : border);
        }
    }
}

static void ClearMask(DWORD* pixels, int count) {
    if (pixels) {
        ZeroMemory(pixels, static_cast<SIZE_T>(count) * sizeof(DWORD));
    }
}

static void CompositeMask(
    DWORD* dest,
    const DWORD* mask,
    int width,
    int height,
    RGBA color) {
    if (!dest || !mask) return;

    const int count = width * height;
    for (int i = 0; i < count; ++i) {
        DWORD p = mask[i];

        BYTE b = static_cast<BYTE>(p & 0xFF);
        BYTE g = static_cast<BYTE>((p >> 8) & 0xFF);
        BYTE r = static_cast<BYTE>((p >> 16) & 0xFF);

        BYTE coverage = std::max(r, std::max(g, b));
        if (coverage) {
            AlphaCompositePixel(&dest[i], color, coverage);
        }
    }
}

static void DrawTextToMask(
    HDC dc,
    DWORD* maskPixels,
    int width,
    int height,
    HFONT font,
    const wchar_t* text,
    RECT rect,
    UINT flags) {
    ClearMask(maskPixels, width * height);

    HFONT oldFont = static_cast<HFONT>(SelectObject(dc, font));
    SetBkMode(dc, TRANSPARENT);
    SetTextColor(dc, RGB(255, 255, 255));

    DrawTextW(dc, text, -1, &rect, flags | DT_NOPREFIX);
    GdiFlush();

    SelectObject(dc, oldFont);
}

static void DrawArrowToMask(
    HDC dc,
    DWORD* maskPixels,
    int width,
    int height,
    RECT rect,
    Settings::ArrowStyle style,
    bool pointsDown,
    int fontSize,
    UINT dpi) {
    ClearMask(maskPixels, width * height);

    const int slotHeight = std::max(
        1, static_cast<int>(rect.bottom - rect.top));
    const int cx = (rect.left + rect.right) / 2;
    const int cy = (rect.top + rect.bottom) / 2;
    const int requestedHeight = ScaleForDpi(
        style == Settings::ArrowStyle::Chevron
            ? std::max(6, fontSize * 2 / 3)
            : std::max(8, fontSize),
        dpi);
    const int arrowHeight = std::clamp(
        requestedHeight, 5, std::max(5, slotHeight - ScaleForDpi(2, dpi)));
    const int arrowWidth = std::max(
        ScaleForDpi(6, dpi), GetArrowColumnWidthPx(fontSize, style, dpi));
    const int halfWidth = std::max(
        2, (arrowWidth - ScaleForDpi(2, dpi)) / 2);
    const int top = cy - arrowHeight / 2;
    const int bottom = top + arrowHeight;
    const int tipY = pointsDown ? bottom : top;
    const int shoulderY = pointsDown
        ? top + std::max(2, arrowHeight * 2 / 3)
        : bottom - std::max(2, arrowHeight * 2 / 3);

    if (style == Settings::ArrowStyle::Solid) {
        const int halfStem = std::max(1, halfWidth / 3);
        POINT points[7] = {};
        if (pointsDown) {
            points[0] = {cx - halfStem, top};
            points[1] = {cx + halfStem, top};
            points[2] = {cx + halfStem, shoulderY};
            points[3] = {cx + halfWidth, shoulderY};
            points[4] = {cx, tipY};
            points[5] = {cx - halfWidth, shoulderY};
            points[6] = {cx - halfStem, shoulderY};
        } else {
            points[0] = {cx - halfStem, bottom};
            points[1] = {cx + halfStem, bottom};
            points[2] = {cx + halfStem, shoulderY};
            points[3] = {cx + halfWidth, shoulderY};
            points[4] = {cx, tipY};
            points[5] = {cx - halfWidth, shoulderY};
            points[6] = {cx - halfStem, shoulderY};
        }

        HGDIOBJ oldPen = SelectObject(dc, GetStockObject(WHITE_PEN));
        HGDIOBJ oldBrush = SelectObject(dc, GetStockObject(WHITE_BRUSH));
        Polygon(dc, points, static_cast<int>(ARRAYSIZE(points)));
        GdiFlush();
        SelectObject(dc, oldBrush);
        SelectObject(dc, oldPen);
        return;
    }

    HPEN pen = CreatePen(
        PS_SOLID, std::max(1, ScaleForDpi(style == Settings::ArrowStyle::Chevron ? 1 : 2, dpi)),
        RGB(255, 255, 255));
    if (!pen) return;

    HGDIOBJ oldPen = SelectObject(dc, pen);
    HGDIOBJ oldBrush = SelectObject(dc, GetStockObject(NULL_BRUSH));
    int wingY = pointsDown ? tipY - std::max(2, arrowHeight / 3)
                           : tipY + std::max(2, arrowHeight / 3);

    if (style == Settings::ArrowStyle::Classic) {
        MoveToEx(dc, cx, pointsDown ? top : bottom, nullptr);
        LineTo(dc, cx, wingY);
    }

    MoveToEx(dc, cx - halfWidth, wingY, nullptr);
    LineTo(dc, cx, tipY);
    LineTo(dc, cx + halfWidth, wingY);

    GdiFlush();
    SelectObject(dc, oldBrush);
    SelectObject(dc, oldPen);
    DeleteObject(pen);
}

static void RenderWidget() {
    if (!g_hwnd || !IsWindow(g_hwnd)) {
        return;
    }

    Settings s = GetSettingsCopy();

    RECT wr = {};
    if (!GetWindowRect(g_hwnd, &wr)) {
        return;
    }

    int width = wr.right - wr.left;
    int height = wr.bottom - wr.top;
    if (width <= 0 || height <= 0) {
        return;
    }

    bool lightTheme = IsSystemLightTheme();
    RGBA valueColor = GetValueColor(s, lightTheme);
    RGBA downArrow = s.accentArrows ? RGBA{76, 201, 240, 255} : valueColor;
    RGBA upArrow = s.accentArrows ? RGBA{121, 226, 170, 255} : valueColor;
    switch (s.premiumPalette) {
        case Settings::PremiumPalette::MidnightGold:
            downArrow = {234, 183, 92, 255}; upArrow = {255, 220, 151, 255}; break;
        case Settings::PremiumPalette::ArcticSilver:
            downArrow = {92, 198, 245, 255}; upArrow = {178, 235, 255, 255}; break;
        case Settings::PremiumPalette::RoseGold:
            downArrow = {241, 142, 145, 255}; upArrow = {255, 197, 164, 255}; break;
        case Settings::PremiumPalette::Emerald:
            downArrow = {83, 211, 158, 255}; upArrow = {157, 242, 201, 255}; break;
        case Settings::PremiumPalette::Signature: break;
    }

    HDC screen = GetDC(nullptr);
    if (!screen) return;

    HDC destDc = CreateCompatibleDC(screen);
    HDC maskDc = CreateCompatibleDC(screen);
    if (!destDc || !maskDc) {
        if (destDc) DeleteDC(destDc);
        if (maskDc) DeleteDC(maskDc);
        ReleaseDC(nullptr, screen);
        return;
    }

    BITMAPINFO bmi = {};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = width;
    bmi.bmiHeader.biHeight = -height;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    void* destBits = nullptr;
    void* maskBits = nullptr;

    HBITMAP destBmp =
        CreateDIBSection(destDc, &bmi, DIB_RGB_COLORS, &destBits, nullptr, 0);
    HBITMAP maskBmp =
        CreateDIBSection(maskDc, &bmi, DIB_RGB_COLORS, &maskBits, nullptr, 0);

    if (!destBmp || !maskBmp) {
        if (destBmp) DeleteObject(destBmp);
        if (maskBmp) DeleteObject(maskBmp);
        DeleteDC(destDc);
        DeleteDC(maskDc);
        ReleaseDC(nullptr, screen);
        return;
    }

    HBITMAP oldDestBmp = static_cast<HBITMAP>(SelectObject(destDc, destBmp));
    HBITMAP oldMaskBmp = static_cast<HBITMAP>(SelectObject(maskDc, maskBmp));

    // Zero-alpha pixels pass clicks to the taskbar. Give the entire rectangle
    // a virtually invisible alpha floor so gaps and corners can start a drag.
    std::fill_n(static_cast<DWORD*>(destBits), width * height, 0x01000000u);
    ZeroMemory(maskBits, static_cast<SIZE_T>(width) * height * 4);

    DWORD* destPixels = static_cast<DWORD*>(destBits);
    DWORD* maskPixels = static_cast<DWORD*>(maskBits);

    if (s.backgroundStyle != Settings::BackgroundStyle::Transparent) {
        BYTE fillAlpha =
            s.backgroundStyle == Settings::BackgroundStyle::Glass ? 26 : 118;
        BYTE borderAlpha =
            s.backgroundStyle == Settings::BackgroundStyle::Glass ? 52 : 70;

        RGBA fill = lightTheme
                        ? RGBA{255, 255, 255, fillAlpha}
                        : RGBA{28, 24, 22, fillAlpha};
        RGBA border = lightTheme
                          ? RGBA{70, 70, 70, borderAlpha}
                          : RGBA{255, 255, 255, borderAlpha};

        switch (s.premiumPalette) {
            case Settings::PremiumPalette::MidnightGold:
                fill = {25, 23, 34, 190}; border = {221, 175, 98, 175}; break;
            case Settings::PremiumPalette::ArcticSilver:
                fill = {26, 56, 74, 132}; border = {130, 213, 246, 172}; break;
            case Settings::PremiumPalette::RoseGold:
                fill = {60, 29, 41, 182}; border = {239, 157, 151, 180}; break;
            case Settings::PremiumPalette::Emerald:
                fill = {22, 43, 37, 185}; border = {107, 211, 163, 172}; break;
            case Settings::PremiumPalette::Signature: break;
        }

        fill.a = static_cast<BYTE>(fill.a * s.backgroundOpacity / 100);
        border.a = static_cast<BYTE>(border.a * s.borderOpacity / 100);

        DrawRoundedBackground(
            destPixels, width, height,
            ScaleForDpi(s.cornerRadius, g_lastDpi),
            fill, border);
    }

    int fontPx = ScaleForDpi(s.fontSize, g_lastDpi);
    HFONT font = CreateFontW(
        -fontPx,
        0, 0, 0,
        s.fontWeight,
        FALSE, FALSE, FALSE,
        DEFAULT_CHARSET,
        OUT_DEFAULT_PRECIS,
        CLIP_DEFAULT_PRECIS,
        ANTIALIASED_QUALITY,
        DEFAULT_PITCH | FF_SWISS,
        L"Segoe UI Variable Text");

    if (!font) {
        font = CreateFontW(
            -fontPx, 0, 0, 0, s.fontWeight,
            FALSE, FALSE, FALSE, DEFAULT_CHARSET,
            OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
            ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_SWISS,
            L"Segoe UI");
    }

    wchar_t downText[40] = {};
    wchar_t upText[40] = {};
    FormatSpeed(g_displayDown, s.useBits, downText, ARRAYSIZE(downText));
    FormatSpeed(g_displayUp, s.useBits, upText, ARRAYSIZE(upText));

    const int padX = ScaleForDpi(5, g_lastDpi);
    // Vector arrows keep a stable width even when the selected font has no
    // down/up arrow glyph. Both speed values stay aligned to this column.
    const int arrowWidth = GetArrowColumnWidthPx(s.fontSize, s.arrowStyle, g_lastDpi);
    const int split = height / 2;

    RECT arrowTop = {padX, 0, padX + arrowWidth, split};
    RECT arrowBottom = {padX, split, padX + arrowWidth, height};

    RECT valueTop = {
        padX + arrowWidth + ScaleForDpi(s.arrowValueGap, g_lastDpi),
        0,
        width - ScaleForDpi(3, g_lastDpi),
        split
    };
    RECT valueBottom = {
        padX + arrowWidth + ScaleForDpi(s.arrowValueGap, g_lastDpi),
        split,
        width - ScaleForDpi(3, g_lastDpi),
        height
    };

    DrawArrowToMask(maskDc, maskPixels, width, height, arrowTop,
                    s.arrowStyle, true, s.fontSize, g_lastDpi);
    CompositeMask(destPixels, maskPixels, width, height, downArrow);

    DrawArrowToMask(maskDc, maskPixels, width, height, arrowBottom,
                    s.arrowStyle, false, s.fontSize, g_lastDpi);
    CompositeMask(destPixels, maskPixels, width, height, upArrow);

    DrawTextToMask(
        maskDc, maskPixels, width, height, font,
        downText, valueTop,
        DT_LEFT | DT_VCENTER | DT_SINGLELINE);
    CompositeMask(destPixels, maskPixels, width, height, valueColor);

    DrawTextToMask(
        maskDc, maskPixels, width, height, font,
        upText, valueBottom,
        DT_LEFT | DT_VCENTER | DT_SINGLELINE);
    CompositeMask(destPixels, maskPixels, width, height, valueColor);

    if (font) DeleteObject(font);

    POINT dstPoint = {wr.left, wr.top};
    SIZE size = {width, height};
    POINT srcPoint = {0, 0};

    BLENDFUNCTION blend = {};
    blend.BlendOp = AC_SRC_OVER;
    blend.SourceConstantAlpha = 255;
    blend.AlphaFormat = AC_SRC_ALPHA;

    UpdateLayeredWindow(
        g_hwnd,
        screen,
        &dstPoint,
        &size,
        destDc,
        &srcPoint,
        0,
        &blend,
        ULW_ALPHA);

    SelectObject(destDc, oldDestBmp);
    SelectObject(maskDc, oldMaskBmp);
    DeleteObject(destBmp);
    DeleteObject(maskBmp);
    DeleteDC(destDc);
    DeleteDC(maskDc);
    ReleaseDC(nullptr, screen);
}

static bool RepositionWidget();

static void UpdateNetworkSample() {
    Settings s = GetSettingsCopy();

    ULONGLONG currentIn = 0;
    ULONGLONG currentOut = 0;
    ULONGLONG interfaceSignature = 0;
    if (!GetNetworkBytes(s, &currentIn, &currentOut, &interfaceSignature)) {
        g_haveBaseline = false;
        return;
    }

    ULONGLONG now = GetTickCount64();
    if (!g_haveBaseline ||
        interfaceSignature != g_lastInterfaceSignature ||
        currentIn < g_lastBytesIn || currentOut < g_lastBytesOut) {
        g_lastBytesIn = currentIn;
        g_lastBytesOut = currentOut;
        g_lastInterfaceSignature = interfaceSignature;
        g_lastSampleTick = now;
        g_haveBaseline = true;
        g_displayDown = 0.0;
        g_displayUp = 0.0;
        if (GetCompactWidgetWidthPx(s, g_lastDpi) != g_lastWidthPx) {
            RepositionWidget();
        } else {
            RenderWidget();
        }
        return;
    }

    ULONGLONG elapsedMs = now - g_lastSampleTick;
    if (elapsedMs == 0) {
        return;
    }

    ULONGLONG deltaIn = currentIn - g_lastBytesIn;
    ULONGLONG deltaOut = currentOut - g_lastBytesOut;
    double seconds = elapsedMs / 1000.0;
    double rawDown = static_cast<double>(deltaIn) / seconds;
    double rawUp = static_cast<double>(deltaOut) / seconds;

    if (rawDown < 8.0) rawDown = 0.0;
    if (rawUp < 8.0) rawUp = 0.0;

    double alpha = 1.0 - (s.smoothingPercent / 100.0);
    alpha = std::clamp(alpha, 0.05, 1.0);
    if (g_displayDown == 0.0 && g_displayUp == 0.0) {
        g_displayDown = rawDown;
        g_displayUp = rawUp;
    } else {
        g_displayDown += alpha * (rawDown - g_displayDown);
        g_displayUp += alpha * (rawUp - g_displayUp);
    }

    if (g_displayDown < 1.0) g_displayDown = 0.0;
    if (g_displayUp < 1.0) g_displayUp = 0.0;

    g_lastBytesIn = currentIn;
    g_lastBytesOut = currentOut;
    g_lastInterfaceSignature = interfaceSignature;
    g_lastSampleTick = now;

    if (GetCompactWidgetWidthPx(s, g_lastDpi) != g_lastWidthPx) {
        RepositionWidget();
    } else {
        RenderWidget();
    }
}

static HWND FindPrimaryTaskbar() {
    return FindWindowW(L"Shell_TrayWnd", nullptr);
}

static void CALLBACK ForegroundChanged(
    HWINEVENTHOOK, DWORD event, HWND foreground, LONG, LONG, DWORD, DWORD) {
    if (event != EVENT_SYSTEM_FOREGROUND || !foreground) return;

    HWND taskbar = FindPrimaryTaskbar();
    if (!taskbar || (foreground != taskbar &&
        GetAncestor(foreground, GA_ROOT) != taskbar)) {
        return;
    }

    HWND hwnd = g_hwnd.load(std::memory_order_acquire);
    if (hwnd) PostMessageW(hwnd, WM_APP + 26, 0, 0);
}

static void PlaceWidget(HWND hwnd, int x, int y, int width, int height) {
    RECT current = {};
    const bool hasRect = GetWindowRect(hwnd, &current) != FALSE;
    const bool sameGeometry = hasRect && current.left == x && current.top == y &&
        current.right - current.left == width &&
        current.bottom - current.top == height;
    const bool topmost =
        (GetWindowLongPtrW(hwnd, GWL_EXSTYLE) & WS_EX_TOPMOST) != 0;
    if (sameGeometry && IsWindowVisible(hwnd) && topmost) {
        return;
    }

    SetWindowPos(hwnd, HWND_TOPMOST, x, y, width, height,
                 SWP_NOACTIVATE | SWP_SHOWWINDOW);
}

static bool RepositionWidget() {
    HWND hwnd = g_hwnd.load(std::memory_order_acquire);
    if (!hwnd || !IsWindow(hwnd)) {
        return false;
    }

    if (g_isDragging) return true;
    Settings s = GetSettingsCopy();

    if (g_freePosition) {
        if (IsFullscreenSuppressed(s)) {
            ShowWindow(hwnd, SW_HIDE);
            return true;
        }
        UINT dpi = GetDpiForWindow(hwnd);
        if (!dpi) dpi = 96;
        int widthPx = GetCompactWidgetWidthPx(s, dpi);
        int heightPx = ScaleForDpi(s.height, dpi);
        RECT rect = {g_position.x, g_position.y,
                     g_position.x + widthPx, g_position.y + heightPx};
        MONITORINFO monitor = {};
        monitor.cbSize = sizeof(monitor);
        if (GetMonitorInfoW(MonitorFromRect(&rect, MONITOR_DEFAULTTONEAREST), &monitor)) {
            g_position.x = std::clamp(g_position.x, monitor.rcMonitor.left,
                std::max(monitor.rcMonitor.left, monitor.rcMonitor.right - widthPx));
            g_position.y = std::clamp(g_position.y, monitor.rcMonitor.top,
                std::max(monitor.rcMonitor.top, monitor.rcMonitor.bottom - heightPx));
        }
        bool sizeChanged = widthPx != g_lastWidthPx ||
                           heightPx != g_lastHeightPx || dpi != g_lastDpi;
        g_lastDpi = dpi;
        g_lastWidthPx = widthPx;
        g_lastHeightPx = heightPx;
        PlaceWidget(hwnd, g_position.x, g_position.y, widthPx, heightPx);
        if (sizeChanged) RenderWidget();
        return true;
    }

    HWND taskbar = FindPrimaryTaskbar();
    if (!taskbar || !IsWindow(taskbar) || !IsWindowVisible(taskbar)) {
        ShowWindow(hwnd, SW_HIDE);
        return false;
    }
    if (IsFullscreenSuppressed(s)) {
        ShowWindow(hwnd, SW_HIDE);
        return true;
    }

    RECT taskbarRect = {};
    if (!GetWindowRect(taskbar, &taskbarRect) || IsRectEmpty(&taskbarRect)) {
        ShowWindow(hwnd, SW_HIDE);
        return false;
    }

    int taskbarWidth = taskbarRect.right - taskbarRect.left;
    int taskbarHeight = taskbarRect.bottom - taskbarRect.top;
    if (taskbarHeight > taskbarWidth) {
        ShowWindow(hwnd, SW_HIDE);
        return false;
    }

    UINT dpi = GetDpiForWindow(taskbar);
    if (!dpi) dpi = 96;
    int widthPx = GetCompactWidgetWidthPx(s, dpi);
    int requestedHeightPx = ScaleForDpi(s.height, dpi);
    int maxHeightPx = std::max(16, taskbarHeight - ScaleForDpi(4, dpi));
    int heightPx = std::min(requestedHeightPx, maxHeightPx);

    HWND trayNotify = FindWindowExW(taskbar, nullptr, L"TrayNotifyWnd", nullptr);
    RECT trayRect = {};
    bool haveTrayRect = trayNotify && GetWindowRect(trayNotify, &trayRect) &&
                        !IsRectEmpty(&trayRect);

    int gapPx = ScaleForDpi(s.gapFromTray, dpi);
    int xOffsetPx = ScaleForDpi(s.horizontalOffset, dpi);
    int yOffsetPx = ScaleForDpi(s.verticalOffset, dpi);
    int x = haveTrayRect
        ? trayRect.left - gapPx - widthPx + xOffsetPx
        : taskbarRect.right - ScaleForDpi(s.fallbackRightOffset, dpi) -
              widthPx + xOffsetPx;
    int y = taskbarRect.top + (taskbarHeight - heightPx) / 2 + yOffsetPx;

    const int taskbarLeft = static_cast<int>(taskbarRect.left);
    const int taskbarTop = static_cast<int>(taskbarRect.top);
    const int taskbarRight = static_cast<int>(taskbarRect.right);
    const int taskbarBottom = static_cast<int>(taskbarRect.bottom);
    x = std::clamp(x, taskbarLeft, std::max(taskbarLeft, taskbarRight - widthPx));
    y = std::clamp(y, taskbarTop, std::max(taskbarTop, taskbarBottom - heightPx));

    bool sizeChanged = widthPx != g_lastWidthPx || heightPx != g_lastHeightPx ||
                       dpi != g_lastDpi;
    g_lastDpi = dpi;
    g_lastWidthPx = widthPx;
    g_lastHeightPx = heightPx;
    PlaceWidget(hwnd, x, y, widthPx, heightPx);
    if (sizeChanged) RenderWidget();
    return true;
}

// Move the existing layered surface; do not recreate fonts or repaint every
// mouse move. The anchor preserves the precise point where the user grabbed it.
static void TrackDrag() {
    if (!g_isDragging) return;
    POINT cursor = {};
    if (!GetCursorPos(&cursor)) return;

    if (!g_dragMoved) {
        const int thresholdX = std::max(1, GetSystemMetrics(SM_CXDRAG));
        const int thresholdY = std::max(1, GetSystemMetrics(SM_CYDRAG));
        if (cursor.x - g_dragStart.x < thresholdX &&
            g_dragStart.x - cursor.x < thresholdX &&
            cursor.y - g_dragStart.y < thresholdY &&
            g_dragStart.y - cursor.y < thresholdY) {
            return;
        }

        g_dragMoved = true;
        g_freePosition = true;
    }

    POINT next = {cursor.x - g_dragAnchor.x, cursor.y - g_dragAnchor.y};
    if (next.x == g_position.x && next.y == g_position.y) return;
    g_position = next;
    SetWindowPos(g_hwnd, nullptr, next.x, next.y, 0, 0,
                 SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
}

static void FinishDrag() {
    if (!g_isDragging) return;
    TrackDrag();
    const bool moved = g_dragMoved;
    g_isDragging = false;
    g_dragMoved = false;
    KillTimer(g_hwnd, TIMER_DRAG);
    if (GetCapture() == g_hwnd) ReleaseCapture();
    if (moved) SavePosition();
    RepositionWidget();
    RenderWidget();
}

static LRESULT CALLBACK WidgetWndProc(
    HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_NCHITTEST:
            return HTCLIENT;

        case WM_SETCURSOR:
            SetCursor(LoadCursorW(nullptr, IDC_SIZEALL));
            return TRUE;

        case WM_LBUTTONDOWN: {
            if (g_isDragging) return 0;
            POINT cursor = {};
            RECT rect = {};
            if (!GetCursorPos(&cursor) || !GetWindowRect(hwnd, &rect)) return 0;
            // Create the timer first; if Windows cannot track the drag, leave
            // the original placement intact.
            if (!SetTimer(hwnd, TIMER_DRAG, 16, nullptr)) return 0;
            g_dragAnchor = {cursor.x - rect.left, cursor.y - rect.top};
            g_dragStart = cursor;
            g_position = {rect.left, rect.top};
            g_isDragging = true;
            g_dragMoved = false;
            SetCapture(hwnd);
            SetCursor(LoadCursorW(nullptr, IDC_SIZEALL));
            return 0;
        }

        case WM_MOUSEMOVE:
            if (g_isDragging) TrackDrag();
            return 0;

        case WM_LBUTTONUP:
            FinishDrag();
            return 0;

        case WM_CAPTURECHANGED:
        case WM_CANCELMODE:
            FinishDrag();
            return 0;

        case WM_LBUTTONDBLCLK:
            FinishDrag();
            g_freePosition = false;
            Wh_DeleteValue(L"dragPosition");
            RepositionWidget();
            RenderWidget();
            return 0;

        case WM_MOUSEACTIVATE:
            return MA_NOACTIVATE;

        case WM_TIMER:
            if (wParam == TIMER_DRAG) {
                // No-focus windows can lose mouse messages outside their area.
                // Poll only during a drag so rapid moves still track/release.
                const int button = GetSystemMetrics(SM_SWAPBUTTON) ? VK_RBUTTON : VK_LBUTTON;
                if (!(GetAsyncKeyState(button) & 0x8000)) FinishDrag();
                else TrackDrag();
                return 0;
            }
            if (wParam == TIMER_SAMPLE) {
                if (!g_isDragging) UpdateNetworkSample();
                return 0;
            }

            if (wParam == TIMER_POSITION) {
                RepositionWidget();
                return 0;
            }
            break;

        case WM_DISPLAYCHANGE:
        case WM_DPICHANGED:
        case WM_SETTINGCHANGE:
        case WM_THEMECHANGED:
            RepositionWidget();
            RenderWidget();
            return 0;

        case WM_APP + 26:
            if (IsWindowVisible(hwnd)) {
                SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0,
                    SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
            }
            return 0;

        case WM_APP_SETTINGS: {
            Settings s = GetSettingsCopy();

            KillTimer(hwnd, TIMER_SAMPLE);
            SetTimer(hwnd, TIMER_SAMPLE, s.updateIntervalMs, nullptr);

            g_haveBaseline = false;
            g_displayDown = 0.0;
            g_displayUp = 0.0;

            RepositionWidget();
            RenderWidget();
            return 0;
        }

        case WM_CLOSE:
            DestroyWindow(hwnd);
            return 0;

        case WM_DESTROY:
            g_isDragging = false;
            KillTimer(hwnd, TIMER_DRAG);
            if (GetCapture() == hwnd) ReleaseCapture();
            KillTimer(hwnd, TIMER_SAMPLE);
            KillTimer(hwnd, TIMER_POSITION);
            PostQuitMessage(0);
            return 0;
    }

    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

static DWORD WINAPI WidgetThread(LPVOID) {
    MSG msg = {};
    PeekMessageW(&msg, nullptr, WM_USER, WM_USER, PM_NOREMOVE);
    SetEvent(g_queueReady);

    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

    HMODULE instance = nullptr;
    if (!GetModuleHandleExW(
            GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
            reinterpret_cast<LPCWSTR>(&WidgetThread), &instance)) {
        Wh_Log(L"GetModuleHandleExW failed, error=%u", GetLastError());
        return 1;
    }

    const wchar_t* className = L"WindhawkPremiumNetSpeedWidget";
    WNDCLASSEXW wc = {};
    wc.cbSize = sizeof(wc);
    wc.hInstance = instance;
    wc.style = CS_DBLCLKS;
    wc.lpfnWndProc = WidgetWndProc;
    wc.lpszClassName = className;
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);

    if (!RegisterClassExW(&wc)) {
        Wh_Log(L"RegisterClassExW failed, error=%u", GetLastError());
        return 1;
    }

    g_hwnd = CreateWindowExW(
        WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE | WS_EX_TOPMOST,
        className,
        L"Premium Taskbar Net Speed",
        WS_POPUP,
        0, 0, 10, 10,
        nullptr,
        nullptr,
        instance,
        nullptr);

    if (!g_hwnd.load(std::memory_order_acquire)) {
        Wh_Log(L"CreateWindowExW failed, error=%u", GetLastError());
        UnregisterClassW(className, instance);
        return 1;
    }

    g_foregroundHook = SetWinEventHook(
        EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND, nullptr,
        ForegroundChanged, 0, 0,
        WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS);
    if (!g_foregroundHook) {
        Wh_Log(L"SetWinEventHook failed, error=%u", GetLastError());
    }

    Settings s = GetSettingsCopy();
    LoadPosition();
    RepositionWidget();
    UpdateNetworkSample();
    SetTimer(g_hwnd, TIMER_SAMPLE, s.updateIntervalMs, nullptr);
    SetTimer(g_hwnd, TIMER_POSITION, 1000, nullptr);

    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    if (g_foregroundHook) {
        UnhookWinEvent(g_foregroundHook);
        g_foregroundHook = nullptr;
    }

    HWND hwnd = g_hwnd.exchange(nullptr, std::memory_order_acq_rel);
    if (hwnd && IsWindow(hwnd)) {
        DestroyWindow(hwnd);
    }
    if (!UnregisterClassW(className, instance)) {
        Wh_Log(L"UnregisterClassW failed, error=%u", GetLastError());
    }
    return 0;
}

}  // namespace

BOOL WhTool_ModInit() {
    ReloadSettings();

    g_queueReady = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_queueReady) {
        Wh_Log(L"CreateEventW failed, error=%u", GetLastError());
        return FALSE;
    }

    DWORD threadId = 0;
    g_thread = CreateThread(nullptr, 0, WidgetThread, nullptr, 0, &threadId);
    if (!g_thread) {
        Wh_Log(L"CreateThread failed, error=%u", GetLastError());
        CloseHandle(g_queueReady);
        g_queueReady = nullptr;
        return FALSE;
    }

    g_threadId.store(threadId, std::memory_order_release);
    return TRUE;
}

void WhTool_ModSettingsChanged() {
    ReloadSettings();
    HWND hwnd = g_hwnd.load(std::memory_order_acquire);
    if (hwnd) PostMessageW(hwnd, WM_APP_SETTINGS, 0, 0);
}

void WhTool_ModUninit() {
    HANDLE thread = g_thread;
    if (!thread) return;

    if (g_queueReady) {
        WaitForSingleObject(g_queueReady, INFINITE);
    }
    DWORD threadId = g_threadId.load(std::memory_order_acquire);
    if (threadId) {
        PostThreadMessageW(threadId, WM_QUIT, 0, 0);
    }

    WaitForSingleObject(thread, INFINITE);
    CloseHandle(thread);
    g_thread = nullptr;
    g_threadId.store(0, std::memory_order_release);

    if (g_queueReady) {
        CloseHandle(g_queueReady);
        g_queueReady = nullptr;
    }
}

////////////////////////////////////////////////////////////////////////////////
// Windhawk tool mod implementation for mods which don't need to inject to other
// processes or hook other functions. Context:
// https://github.com/ramensoftware/windhawk/wiki/Mods-as-tools:-Running-mods-in-a-dedicated-process
//
// The mod will load and run in a dedicated windhawk.exe process.
//
// Paste the code below as part of the mod code, and use these callbacks:
// * WhTool_ModInit
// * WhTool_ModSettingsChanged
// * WhTool_ModUninit
//
// Currently, other callbacks are not supported.

bool g_isToolModProcessLauncher;
HANDLE g_toolModProcessMutex;

void WINAPI EntryPoint_Hook() {
    Wh_Log(L">");
    ExitThread(0);
}

BOOL Wh_ModInit() {
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

    WCHAR commandLine[MAX_PATH + 2 +
        (sizeof(L" -tool-mod \"" WH_MOD_ID "\"") / sizeof(WCHAR)) - 1];
    swprintf_s(commandLine, L"\"%s\" -tool-mod \"%s\"",
               currentProcessPath, WH_MOD_ID);

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

    WhTool_ModSettingsChanged();
}

void Wh_ModUninit() {
    if (g_isToolModProcessLauncher) {
        return;
    }

    WhTool_ModUninit();
    ExitProcess(0);
}
