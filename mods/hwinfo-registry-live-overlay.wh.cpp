// ==WindhawkMod==
// @id              hwinfo-registry-live-overlay
// @name            HWiNFO Registry Live Overlay
// @description     Displays HWiNFO sensor values from the Registry as a lightweight desktop overlay.
// @version         1.5.0
// @author          SilverAmd
// @github          https://github.com/SilverAmd
// @license         MIT
// @include         windhawk.exe
// @compilerOptions -lgdi32 -lmsimg32 -lshell32 -lole32 -luuid
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# HWiNFO Registry Live Overlay

A lightweight desktop overlay for displaying selected HWiNFO sensor values from the HWiNFO Gadget/VSB Registry output.

The mod can be used to monitor values such as CPU power, GPU power, GPU temperature, VRAM usage, water-cooling pump RPM, coolant temperature, flow rate, PSU power, and other sensor values exposed by HWiNFO.

## Screenshots

![HWiNFO Registry Live Overlay desktop overlay less sensors](https://i.imgur.com/tajpyvb.png)

*Shows the desktop overlay with selected HWiNFO Gadget/VSB sensor values, auto height, background gradient, and optional Water Age.*

![HWiNFO Registry Live Overlay desktop overlay more sensors](https://i.imgur.com/8yz5LV6.png)

*Shows the desktop overlay with more selected HWiNFO Gadget/VSB sensor values, auto height, background gradient, and optional Water Age.*

![HWiNFO Registry Live Overlay export helper](https://i.imgur.com/StZ9Mum.png)

*Shows the HTML registry export helper.*

![HWiNFO64 Sensor Settings Gadget setup](https://i.imgur.com/a2a69Gh.png)

*Shows where to enable HWiNFO Gadget reporting: enable reporting to Gadget, select a sensor, enable “Report value in Gadget”, note the index number, and click OK.*

*Alternatively, use the built-in registry export helper, copy the generated Windhawk Rows text, and paste it into the Rows setting.*

![HWiNFO Registry Live Overlay Rows input field](https://i.imgur.com/D9D4hVT.png)

*Shows the Rows input field. Separate rows with semicolon. Format: Display Name | Registry Value. Use Ctrl+Alt+R to export available HWiNFO Gadget/VSB values as HTML.*



## Features

- Displays selected HWiNFO Gadget/VSB Registry values on the desktop
- Configurable sensor rows using `Label | ValueN`
- Custom font family, size, weight, and style
- Separate label and value colors
- Left/right alignment for label and value columns
- Optional background with opacity
- Optional gradient background
- Optional rounded corners and border
- Optional always-on-top mode
- Toggle hotkey to show or hide the overlay
- Drag hotkey for live positioning
- Optional remembered drag position
- Auto height based on visible rows and Water Age
- Optional Water Age display based on a configured coolant fill date
- HTML export of the current HWiNFO Gadget/VSB Registry sensor list
- Helpful message and HTML page when no HWiNFO Gadget/VSB sensors are found

## HWiNFO setup

This mod reads the HWiNFO Gadget/VSB Registry output.

In HWiNFO:

1. Open the Sensors window.
2. Click Configure Sensors.
3. Open the HWiNFO Gadget tab.
4. Enable reporting to Gadget globally.
5. Select the sensor values you want to use.
6. Enable Report value in Gadget for each selected value.
7. Press OK.
8. Use the registry export hotkey to create an HTML sensor list.

The default registry path is:

```txt
HKCU\SOFTWARE\HWiNFO64\VSB
```

Typical value names are:

```txt
Sensor0 / Label0 / Value0 / ValueRaw0 / Color0
Sensor1 / Label1 / Value1 / ValueRaw1 / Color1
...
```

Rows are configured like this:

```txt
CPU W | Value0; GPU W | Value1; GPU TEMP | Value2
```

## Hotkeys

Default hotkeys:

```txt
Ctrl + Alt + H = Show/hide overlay
Ctrl + Alt + D = Toggle drag mode
Ctrl + Alt + R = Export HWiNFO Gadget/VSB Registry sensor list as HTML
```

The hotkeys can be changed in the mod settings.

## Water Age

The optional Water Age feature calculates the number of days since a configured coolant fill date.

Example:

```txt
Water fill date: 2024-11-18
Water max age: 730 days
```

The Water Age text can change color based on warning and alarm percentage thresholds.

## Notes

This mod does not read all HWiNFO sensors directly. It only reads values that HWiNFO exposes through its Gadget/VSB Registry reporting feature.

This mod does not use HWiNFO Shared Memory.

This is a separate mod because it reads HWiNFO Gadget/VSB Registry values instead of HWiNFO Shared Memory and includes an HTML registry export helper for mapping ValueN entries to Windhawk rows.

HWiNFO must be installed and running, and the desired sensor values must be enabled for Gadget reporting in HWiNFO.

## Credits and thanks

Special thanks to Martin Malik / REALiX, the developer of HWiNFO, for creating HWiNFO and for providing the HWiNFO Gadget/VSB Registry reporting feature.

Martin Malik kindly granted permission to go ahead with this mod.

This mod is not affiliated with HWiNFO / REALiX. All sensor data is provided by HWiNFO. The mod only reads the registry values that the user explicitly enables in HWiNFO Sensor Settings.

Please respect the HWiNFO license terms. If you like HWiNFO and use it regularly, please consider supporting development by purchasing a HWiNFO license, especially for professional or commercial use.

## Development

Created by SilverAmd / Zarko.

Development assistance:

- ChatGPT by OpenAI
- Claude by Anthropic
- Mistral Vibe

## License

MIT
*/
// ==/WindhawkModReadme==


// ==WindhawkModSettings==
/*
- registryRoot: HKCU
  $name: Registry root
  $description: Use HKCU or HKLM.

- registryPath: SOFTWARE\HWiNFO64\VSB
  $name: Registry path

- refreshIntervalMs: 1000
  $name: Refresh interval in milliseconds

- x: 30
  $name: X position

- y: 30
  $name: Y position

- width: 850
  $name: Overlay width

- height: 640
  $name: Overlay height

- autoHeight: true
  $name: Auto height
  $description: "Automatically adjusts the overlay height based on visible rows and water age. Disable this to use Overlay height manually."

- paddingBottom: 24
  $name: Padding bottom

- fontSize: 16
  $name: Font size

- fontFamily: Cascadia Mono
  $name: Font family

- fontWeight: SemiBold
  $name: Font weight
  $description: "Default is SemiBold"
  $options:
  - Normal: Normal
  - Medium: Medium
  - SemiBold: SemiBold
  - Bold: Bold
  - ExtraBold: ExtraBold

- fontStyle: Normal
  $name: Font style
  $description: "Default is Normal"
  $options:
  - Normal: Normal
  - Italic: Italic

- paddingLeft: 24
  $name: Padding left

- labelColumnWidth: 200
  $name: Label column width
  $description: "Width of the label column in pixels. Used when label alignment is Right."

- labelAlignment: Right
  $name: Label alignment
  $description: "Align sensor labels inside the label column."
  $options:
  - Left: Left
  - Right: Right

- paddingTop: 20
  $name: Padding top

- valueColumnX: 330
  $name: Value column X

- valueColumnWidth: 260
  $name: Value column width
  $description: "Width of the value column in pixels. Used when value alignment is Right."

- valueAlignment: Right
  $name: Value alignment
  $description: "Align sensor values inside the value column."
  $options:
  - Left: Left
  - Right: Right

- showColumnSeparator: true
  $name: Show column separator
  $description: "Draws a vertical separator line between label and value columns."

- columnSeparatorX: 285
  $name: Column separator X
  $description: "X position of the vertical separator line."

- columnSeparatorColor: "#FFFFFF"
  $name: Column separator color #RRGGBB
  $description: "Vertical separator color. Alpha is ignored for GDI pens."

- rowSpacing: 2
  $name: Row spacing

- alwaysOnTop: false
  $name: Always on top
  $description: When enabled, the overlay stays above normal windows. When disabled, it behaves like a normal overlay window.

- enableToggleHotkey: true
  $name: Enable toggle hotkey
  $description: "Enables a global hotkey to show or hide the overlay."

- toggleHotkeyKey: H
  $name: Toggle hotkey key
  $description: "Single letter key for the toggle hotkey. Default is H."

- toggleHotkeyCtrl: true
  $name: Toggle hotkey Ctrl

- toggleHotkeyAlt: true
  $name: Toggle hotkey Alt

- toggleHotkeyShift: false
  $name: Toggle hotkey Shift

- enableDragHotkey: true
  $name: Enable drag hotkey
  $description: "Enables a global hotkey to toggle live drag mode."

- dragHotkeyKey: D
  $name: Drag hotkey key
  $description: "Single letter key for the drag hotkey. Default is D."

- dragHotkeyCtrl: true
  $name: Drag hotkey Ctrl

- dragHotkeyAlt: true
  $name: Drag hotkey Alt

- dragHotkeyShift: false
  $name: Drag hotkey Shift

- rememberDraggedPosition: false
  $name: Remember dragged position
  $description: "When enabled, the last dragged position overrides the X/Y position settings. Disable this to position the overlay manually with the X/Y fields."

- enableExportHotkey: true
  $name: Enable registry export hotkey
  $description: "Enables a global hotkey to export the HWiNFO registry sensor list as HTML."

- exportHotkeyKey: R
  $name: Registry export hotkey key
  $description: "Single letter key for the registry export hotkey. Default is R."

- exportHotkeyCtrl: true
  $name: Registry export hotkey Ctrl

- exportHotkeyAlt: true
  $name: Registry export hotkey Alt

- exportHotkeyShift: false
  $name: Registry export hotkey Shift

- openExportHtmlAfterCreate: true
  $name: Open export HTML after create
  $description: "Opens the generated HWiNFO registry export HTML file after creating it."

- showWaterAge: false
  $name: Show water age
  $description: Shows coolant age calculated from the configured fill date.

- waterAgeLabel: WATER AGE
  $name: Water age label

- waterFillDate: "2024-11-18"
  $name: Water fill date
  $description: "Format: YYYY-MM-DD"

- waterMaxAgeDays: 730
  $name: Water max age in days
  $description: "Recommended: 730 days = 2 years." 

- waterAgeWarnPercent: 70
  $name: Water age warning percent
  $description: "Water age turns warning color at this percentage."

- waterAgeAlarmPercent: 90
  $name: Water age alarm percent
  $description: "Water age turns alarm color at this percentage."

- waterAgeWarnColor: "#FFFFAA00"
  $name: Water age warning color #FFFFAA00

- waterAgeAlarmColor: "#FFFF3333"
  $name: Water age alarm color #FFFF3333

- waterAgeTopGap: 8
  $name: Water age top gap
  $description: Extra vertical spacing before the water age row.

- showWaterAgeSeparator: true
  $name: Show water age separator
  $description: "Draws a horizontal separator line above the water age row."

- waterAgeSeparatorColor: "#FFFFFF"
  $name: Water age separator color #RRGGBB
  $description: "Horizontal separator color. Alpha is ignored for GDI pens."

- textColor: "#FFFFFF"
  $name: Fallback text color #RRGGBB
  $description: "Fallback text color. #AARRGGBB is accepted, but alpha is ignored for GDI text."

- labelColor: "#FF0000"
  $name: Label color #RRGGBB
  $description: "Color for sensor labels, for example CPU W, GPU W, FLOW. Alpha is ignored for GDI text."

- valueColor: "#00FF00"
  $name: Value color #RRGGBB
  $description: "Color for sensor values, for example 53.2 W or 330.6 l/h. Alpha is ignored for GDI text."

- backgroundEnabled: true
  $name: Enable background
  $description: "When disabled, only the text is shown without background."

- backgroundColor: "#304050"
  $name: Background color #RRGGBB
  $description: "Background base color. Alpha from #AARRGGBB is ignored because backgroundOpacityPercent controls overlay opacity."

- backgroundOpacityPercent: 90
  $name: Background opacity percent
  $description: "Overall overlay opacity when background is enabled. Overrides the alpha value from backgroundColor. 100 = fully opaque, 50 = half transparent."

- backgroundGradientEnabled: true
  $name: Enable background gradient
  $description: "When enabled, the background is drawn as a gradient between background color and gradient color 2."

- backgroundGradientColor2: "#507080"
  $name: Background gradient color 2 #RRGGBB
  $description: "Second background gradient color. Alpha is ignored; use backgroundOpacityPercent."

- backgroundGradientDirection: DiagonalUp
  $name: Background gradient direction
  $description: "Direction of the background gradient."
  $options:
  - Vertical: Vertical
  - Horizontal: Horizontal
  - DiagonalDown: Diagonal down
  - DiagonalUp: Diagonal up

- backgroundCornerRadius: 18
  $name: Background corner radius
  $description: "Rounded background corners. 0 disables rounded corners."

- backgroundBorderSize: 2
  $name: Background border size
  $description: "Border size in pixels. 0 disables the border."

- backgroundBorderColor: "#FFFFFF"
  $name: Background border color #RRGGBB
  $description: "Border color. Alpha is ignored for GDI pens."

- hideUnavailableRows: false
  $name: Hide unavailable rows
  $description: "Hides rows whose cached value is N/A."

- rows: RAM Used | Value0; CPU Usage | Value1; CPU Package | Value2; GPU Temp | Value3; GPU Power | Value4; GPU Clock | Value5; VRAM Clock | Value6; GPU Usage | Value7; VRAM Used | Value8; GPU Hotspot | Value9
  $name: Rows
  $description: Separate rows with semicolon. Format is Display Name | Registry Value. Use Ctrl+Alt+R to export available HWiNFO Gadget/VSB values as HTML.
*/
// ==/WindhawkModSettings==

// The source code of the mod starts here. This sample was inspired by the great
// article of Kyle Halladay, X64 Function Hooking by Example:
// https://kylehalladay.com/blog/2020/11/13/Hooking-By-Example.html
// If you're new to terms such as code injection and function hooking, the
// article is great to get started.

#include <windows.h>
#include <shellapi.h>
#include <shlobj.h>
#include <string>
#include <vector>
#include <sstream>
#include <climits>

struct RowConfig {
    std::wstring label;
    std::wstring valueName;
    std::wstring cachedValue;
};

struct {
    std::wstring registryRoot;
    std::wstring registryPath;
    int refreshIntervalMs;
    int x;
    int y;
    int width;
    int height;
    bool autoHeight;
    int paddingBottom;
    int fontSize;
    std::wstring fontFamily;
    std::wstring fontWeight;
    std::wstring fontStyle;
    int paddingLeft;
    int labelColumnWidth;
    std::wstring labelAlignment;
    bool labelAlignRight;
    int paddingTop;
    int valueColumnX;
    int valueColumnWidth;
    std::wstring valueAlignment;
    bool valueAlignRight;
    bool showColumnSeparator;
    int columnSeparatorX;
    COLORREF columnSeparatorColor;
    BYTE columnSeparatorAlpha;
    int rowSpacing;
    bool alwaysOnTop;
    bool enableToggleHotkey;
    std::wstring toggleHotkeyKey;
    bool toggleHotkeyCtrl;
    bool toggleHotkeyAlt;
    bool toggleHotkeyShift;
    bool overlayVisible;
    bool enableDragHotkey;
    std::wstring dragHotkeyKey;
    bool dragHotkeyCtrl;
    bool dragHotkeyAlt;
    bool dragHotkeyShift;
    bool rememberDraggedPosition;
    bool dragModeEnabled;
    bool dragging;
    POINT dragStartMouse;
    POINT dragStartWindow;
    bool enableExportHotkey;
    std::wstring exportHotkeyKey;
    bool exportHotkeyCtrl;
    bool exportHotkeyAlt;
    bool exportHotkeyShift;
    bool openExportHtmlAfterCreate;

    bool showWaterAge;
    std::wstring waterAgeLabel;
    std::wstring waterFillDate;
    int waterMaxAgeDays;
    int waterAgeWarnPercent;
    int waterAgeAlarmPercent;
    COLORREF waterAgeWarnColor;
    BYTE waterAgeWarnAlpha;
    COLORREF waterAgeAlarmColor;
    BYTE waterAgeAlarmAlpha;
    int waterAgeTopGap;
    bool showWaterAgeSeparator;
    COLORREF waterAgeSeparatorColor;
    BYTE waterAgeSeparatorAlpha;

    COLORREF textColor;
    BYTE textAlpha;

    COLORREF labelColor;
    BYTE labelAlpha;
    COLORREF valueColor;
    BYTE valueAlpha;

    bool backgroundEnabled;
    COLORREF backgroundColor;
    BYTE backgroundAlpha;
    int backgroundOpacityPercent;

    bool backgroundGradientEnabled;
    COLORREF backgroundGradientColor2;
    BYTE backgroundGradientAlpha2;
    std::wstring backgroundGradientDirection;
    bool backgroundGradientHorizontal;
    bool backgroundGradientDiagonalDown;
    bool backgroundGradientDiagonalUp;

    int backgroundCornerRadius;
    int backgroundBorderSize;
    COLORREF backgroundBorderColor;
    BYTE backgroundBorderAlpha;

    bool hideUnavailableRows;
    std::wstring rowsText;
    std::vector<RowConfig> rows;
} settings;

HWND g_hwnd = nullptr;
HFONT g_font = nullptr;
UINT_PTR g_timerId = 1;
HANDLE g_uiThread = nullptr;
DWORD g_uiThreadId = 0;
constexpr int HOTKEY_TOGGLE_OVERLAY = 1;
constexpr int HOTKEY_DRAG_MODE = 2;
constexpr int HOTKEY_EXPORT_REGISTRY = 3;
constexpr int SAVED_POSITION_NOT_SET = -2147483647;
constexpr COLORREF TRANSPARENT_COLOR_KEY = 0x00FF00FF; // RGB(255, 0, 255)
constexpr UINT WM_APP_SETTINGS_CHANGED = WM_APP + 1;
constexpr const wchar_t* OVERLAY_WINDOW_CLASS_NAME =
    L"HWiNFORegistryLiveOverlayWindow";
std::wstring Trim(const std::wstring& s) {
    size_t start = s.find_first_not_of(L" \t\r\n");
    if (start == std::wstring::npos)
        return L"";

    size_t end = s.find_last_not_of(L" \t\r\n");
    return s.substr(start, end - start + 1);
}

bool ParseHexColor(const std::wstring& text, BYTE* alpha, COLORREF* color) {
    std::wstring s = text;

    if (!s.empty() && s[0] == L'#')
        s.erase(0, 1);

    if (s.length() != 6 && s.length() != 8)
        return false;

    wchar_t* end = nullptr;
    unsigned long value = wcstoul(s.c_str(), &end, 16);

    if (!end || *end != 0)
        return false;

    BYTE a = 0xFF;
    BYTE r = 0;
    BYTE g = 0;
    BYTE b = 0;

    if (s.length() == 8) {
        a = (value >> 24) & 0xFF;
        r = (value >> 16) & 0xFF;
        g = (value >> 8) & 0xFF;
        b = value & 0xFF;
    } else {
        r = (value >> 16) & 0xFF;
        g = (value >> 8) & 0xFF;
        b = value & 0xFF;
    }

    *alpha = a;
    *color = RGB(r, g, b);
    return true;
}

std::vector<RowConfig> ParseRows(const std::wstring& rowsText) {
    std::vector<RowConfig> rows;

    std::wstring normalized = rowsText;

    // Windhawk settings may flatten multiline text into one line.
    // Support semicolon as row separator:
    // RAM Used | Value0; CPU Usage | Value1; GPU Temp | Value3
    for (wchar_t& ch : normalized) {
        if (ch == L';')
            ch = L'\n';
    }

    std::wstringstream ss(normalized);
    std::wstring line;

    while (std::getline(ss, line)) {
        line = Trim(line);

        if (line.empty())
            continue;

        size_t sep = line.find(L'|');
        if (sep == std::wstring::npos)
            continue;

        RowConfig row;
        row.label = Trim(line.substr(0, sep));
        row.valueName = Trim(line.substr(sep + 1));
        row.cachedValue = L"N/A";

        if (!row.label.empty() && !row.valueName.empty())
            rows.push_back(row);
        }

    return rows;
}

HKEY GetConfiguredRegistryRoot() {
    if (_wcsicmp(settings.registryRoot.c_str(), L"HKLM") == 0 ||
        _wcsicmp(settings.registryRoot.c_str(), L"HKEY_LOCAL_MACHINE") == 0) {
        return HKEY_LOCAL_MACHINE;
    }

    return HKEY_CURRENT_USER;
}

std::wstring ReadRegistryStringFromOpenKey(HKEY key, const std::wstring& valueName) {
    if (!key)
        return L"N/A";

    DWORD type = 0;
    DWORD size = 0;

    LONG result = RegQueryValueExW(
        key,
        valueName.c_str(),
        nullptr,
        &type,
        nullptr,
        &size
    );

    if (result != ERROR_SUCCESS)
        return L"N/A";

    if (type == REG_DWORD) {
        if (size != sizeof(DWORD))
            return L"N/A";

        DWORD value = 0;
        DWORD valueSize = sizeof(value);

        result = RegQueryValueExW(
            key,
            valueName.c_str(),
            nullptr,
            &type,
            reinterpret_cast<LPBYTE>(&value),
            &valueSize
        );

        if (result != ERROR_SUCCESS || valueSize != sizeof(value))
            return L"N/A";

        wchar_t numberBuffer[64] = {};
        swprintf_s(numberBuffer, L"%lu", value);
        return numberBuffer;
    }

    if (type != REG_SZ && type != REG_EXPAND_SZ)
        return L"N/A";

    if (size == 0)
        return L"";

    if (size % sizeof(wchar_t) != 0)
        return L"N/A";

    std::vector<wchar_t> buffer((size / sizeof(wchar_t)) + 1, L'\0');

    result = RegQueryValueExW(
        key,
        valueName.c_str(),
        nullptr,
        &type,
        reinterpret_cast<LPBYTE>(buffer.data()),
        &size
    );

    if (result != ERROR_SUCCESS)
        return L"N/A";

    size_t charCount = size / sizeof(wchar_t);

    if (charCount > 0 && buffer[charCount - 1] == L'\0') {
        --charCount;
    }

    return std::wstring(buffer.data(), charCount);
}

void RefreshCachedRegistryValues() {
    HKEY root = GetConfiguredRegistryRoot();

    HKEY key = nullptr;
    LONG result = RegOpenKeyExW(
        root,
        settings.registryPath.c_str(),
        0,
        KEY_READ,
        &key
    );

    if (result != ERROR_SUCCESS) {
        for (RowConfig& row : settings.rows) {
            row.cachedValue = L"N/A";
        }
        return;
    }

    for (RowConfig& row : settings.rows) {
        row.cachedValue = ReadRegistryStringFromOpenKey(key, row.valueName);
    }

    RegCloseKey(key);
}

int GetGdiFontWeight() {
    if (_wcsicmp(settings.fontWeight.c_str(), L"Medium") == 0)
        return FW_MEDIUM;

    if (_wcsicmp(settings.fontWeight.c_str(), L"SemiBold") == 0)
        return FW_SEMIBOLD;

    if (_wcsicmp(settings.fontWeight.c_str(), L"Bold") == 0)
        return FW_BOLD;

    if (_wcsicmp(settings.fontWeight.c_str(), L"ExtraBold") == 0)
        return FW_EXTRABOLD;

    return FW_NORMAL;
}

BOOL GetGdiFontItalic() {
    return _wcsicmp(settings.fontStyle.c_str(), L"Italic") == 0 ? TRUE : FALSE;
}

void RecreateFont() {
    if (g_font) {
        DeleteObject(g_font);
        g_font = nullptr;
    }

    HDC hdc = GetDC(nullptr);
    int height = -MulDiv(settings.fontSize, GetDeviceCaps(hdc, LOGPIXELSY), 72);
    ReleaseDC(nullptr, hdc);

DWORD fontQuality = settings.backgroundEnabled
    ? CLEARTYPE_QUALITY
    : NONANTIALIASED_QUALITY;

    g_font = CreateFontW(
        height,
        0,
        0,
        0,
        GetGdiFontWeight(),
        GetGdiFontItalic(),
        FALSE,
        FALSE,
        DEFAULT_CHARSET,
        OUT_DEFAULT_PRECIS,
        CLIP_DEFAULT_PRECIS,
        fontQuality,
        DEFAULT_PITCH | FF_DONTCARE,
        settings.fontFamily.c_str()
    );
}

void LoadSettings() {
    const wchar_t* registryRoot = Wh_GetStringSetting(L"registryRoot");
    settings.registryRoot = registryRoot ? registryRoot : L"HKCU";
    Wh_FreeStringSetting(registryRoot);

    const wchar_t* registryPath = Wh_GetStringSetting(L"registryPath");
    settings.registryPath = registryPath ? registryPath : L"SOFTWARE\\HWiNFO64\\VSB";
    Wh_FreeStringSetting(registryPath);

    settings.refreshIntervalMs = Wh_GetIntSetting(L"refreshIntervalMs");
    if (settings.refreshIntervalMs < 250)
        settings.refreshIntervalMs = 1000;

    settings.x = Wh_GetIntSetting(L"x");
    settings.y = Wh_GetIntSetting(L"y");
    settings.width = Wh_GetIntSetting(L"width");
    settings.height = Wh_GetIntSetting(L"height");
    settings.autoHeight = Wh_GetIntSetting(L"autoHeight");
    settings.paddingBottom = Wh_GetIntSetting(L"paddingBottom");
    settings.fontSize = Wh_GetIntSetting(L"fontSize");
    settings.paddingLeft = Wh_GetIntSetting(L"paddingLeft");

    settings.labelColumnWidth = Wh_GetIntSetting(L"labelColumnWidth");

    const wchar_t* labelAlignment = Wh_GetStringSetting(L"labelAlignment");
    settings.labelAlignment = labelAlignment ? labelAlignment : L"Left";
    Wh_FreeStringSetting(labelAlignment);
    settings.labelAlignRight =
    _wcsicmp(settings.labelAlignment.c_str(), L"Right") == 0;

    settings.paddingTop = Wh_GetIntSetting(L"paddingTop");
    settings.valueColumnX = Wh_GetIntSetting(L"valueColumnX");
    settings.valueColumnWidth = Wh_GetIntSetting(L"valueColumnWidth");

    const wchar_t* valueAlignment = Wh_GetStringSetting(L"valueAlignment");
    settings.valueAlignment = valueAlignment ? valueAlignment : L"Left";
    Wh_FreeStringSetting(valueAlignment);
    settings.valueAlignRight =
    _wcsicmp(settings.valueAlignment.c_str(), L"Right") == 0;

    settings.showColumnSeparator = Wh_GetIntSetting(L"showColumnSeparator");

    settings.columnSeparatorX = Wh_GetIntSetting(L"columnSeparatorX");
    if (settings.columnSeparatorX < 0)
        settings.columnSeparatorX = 285;
    if (settings.columnSeparatorX > 2000)
        settings.columnSeparatorX = 2000;

    const wchar_t* columnSeparatorColor = Wh_GetStringSetting(L"columnSeparatorColor");
    if (!ParseHexColor(columnSeparatorColor ? columnSeparatorColor : L"#40FFFFFF",
                       &settings.columnSeparatorAlpha,
                       &settings.columnSeparatorColor)) {
        settings.columnSeparatorAlpha = 0x40;
        settings.columnSeparatorColor = RGB(255, 255, 255);
    }
    Wh_FreeStringSetting(columnSeparatorColor);

    settings.rowSpacing = Wh_GetIntSetting(L"rowSpacing");
    settings.alwaysOnTop = Wh_GetIntSetting(L"alwaysOnTop");

    settings.enableToggleHotkey = Wh_GetIntSetting(L"enableToggleHotkey");

    const wchar_t* toggleHotkeyKey = Wh_GetStringSetting(L"toggleHotkeyKey");
    settings.toggleHotkeyKey = toggleHotkeyKey ? toggleHotkeyKey : L"H";
    Wh_FreeStringSetting(toggleHotkeyKey);

    settings.toggleHotkeyCtrl = Wh_GetIntSetting(L"toggleHotkeyCtrl");
    settings.toggleHotkeyAlt = Wh_GetIntSetting(L"toggleHotkeyAlt");
    settings.toggleHotkeyShift = Wh_GetIntSetting(L"toggleHotkeyShift");

    settings.enableDragHotkey = Wh_GetIntSetting(L"enableDragHotkey");

    const wchar_t* dragHotkeyKey = Wh_GetStringSetting(L"dragHotkeyKey");
    settings.dragHotkeyKey = dragHotkeyKey ? dragHotkeyKey : L"D";
    Wh_FreeStringSetting(dragHotkeyKey);

    settings.dragHotkeyCtrl = Wh_GetIntSetting(L"dragHotkeyCtrl");
    settings.dragHotkeyAlt = Wh_GetIntSetting(L"dragHotkeyAlt");
    settings.dragHotkeyShift = Wh_GetIntSetting(L"dragHotkeyShift");
    settings.rememberDraggedPosition = Wh_GetIntSetting(L"rememberDraggedPosition");

    if (settings.rememberDraggedPosition) {
        int savedX = Wh_GetIntValue(L"draggedX", SAVED_POSITION_NOT_SET);
        int savedY = Wh_GetIntValue(L"draggedY", SAVED_POSITION_NOT_SET);

        if (savedX != SAVED_POSITION_NOT_SET &&
            savedY != SAVED_POSITION_NOT_SET) {
            settings.x = savedX;
            settings.y = savedY;
        }
    }
    settings.dragModeEnabled = false;
    settings.dragging = false;

    settings.enableExportHotkey = Wh_GetIntSetting(L"enableExportHotkey");

    const wchar_t* exportHotkeyKey = Wh_GetStringSetting(L"exportHotkeyKey");
    settings.exportHotkeyKey = exportHotkeyKey ? exportHotkeyKey : L"R";
    Wh_FreeStringSetting(exportHotkeyKey);

    settings.exportHotkeyCtrl = Wh_GetIntSetting(L"exportHotkeyCtrl");
    settings.exportHotkeyAlt = Wh_GetIntSetting(L"exportHotkeyAlt");
    settings.exportHotkeyShift = Wh_GetIntSetting(L"exportHotkeyShift");

    settings.openExportHtmlAfterCreate = Wh_GetIntSetting(L"openExportHtmlAfterCreate");

    if (settings.paddingLeft < 0)
        settings.paddingLeft = 24;
    if (settings.labelColumnWidth < 20)
        settings.labelColumnWidth = 200;
    if (settings.labelColumnWidth > 1000)
        settings.labelColumnWidth = 1000;
    if (settings.paddingTop < 0)
        settings.paddingTop = 20;
    if (settings.valueColumnX < 50)
        settings.valueColumnX = 330;
    if (settings.valueColumnWidth < 50)
        settings.valueColumnWidth = 260;
    if (settings.valueColumnWidth > 1000)
        settings.valueColumnWidth = 1000;
    if (settings.rowSpacing < 0)
        settings.rowSpacing = 0;

    if (settings.width < 100)
        settings.width = 420;
    if (settings.height < 50)
        settings.height = 300;
    if (settings.fontSize < 8)
        settings.fontSize = 22;

    if (settings.paddingBottom < 0)
        settings.paddingBottom = 24;

    if (settings.paddingBottom > 500)
        settings.paddingBottom = 500;

    const wchar_t* fontFamily = Wh_GetStringSetting(L"fontFamily");
    settings.fontFamily = fontFamily ? fontFamily : L"Cascadia Mono";
    Wh_FreeStringSetting(fontFamily);

    const wchar_t* fontWeight = Wh_GetStringSetting(L"fontWeight");
    settings.fontWeight = fontWeight ? fontWeight : L"Normal";
    Wh_FreeStringSetting(fontWeight);

    const wchar_t* fontStyle = Wh_GetStringSetting(L"fontStyle");
    settings.fontStyle = fontStyle ? fontStyle : L"Normal";
    Wh_FreeStringSetting(fontStyle);

    const wchar_t* textColor = Wh_GetStringSetting(L"textColor");
    if (!ParseHexColor(textColor ? textColor : L"#CCFFFFFF",
                       &settings.textAlpha,
                       &settings.textColor)) {
        settings.textAlpha = 0xCC;
        settings.textColor = RGB(255, 255, 255);
    }
    Wh_FreeStringSetting(textColor);

    const wchar_t* labelColor = Wh_GetStringSetting(L"labelColor");
    if (!ParseHexColor(labelColor ? labelColor : L"#CCFFFFFF",
                       &settings.labelAlpha,
                       &settings.labelColor)) {
        settings.labelAlpha = settings.textAlpha;
        settings.labelColor = settings.textColor;
    }
    Wh_FreeStringSetting(labelColor);

    const wchar_t* valueColor = Wh_GetStringSetting(L"valueColor");
    if (!ParseHexColor(valueColor ? valueColor : L"#CCFFFFFF",
                    &settings.valueAlpha,
                    &settings.valueColor)) {
        settings.valueAlpha = settings.textAlpha;
        settings.valueColor = settings.textColor;
    }
    Wh_FreeStringSetting(valueColor);

    settings.backgroundEnabled = Wh_GetIntSetting(L"backgroundEnabled");

    const wchar_t* backgroundColor = Wh_GetStringSetting(L"backgroundColor");
    if (!ParseHexColor(backgroundColor ? backgroundColor : L"#80000000",
                       &settings.backgroundAlpha,
                       &settings.backgroundColor)) {
        settings.backgroundAlpha = 0x80;
        settings.backgroundColor = RGB(0, 0, 0);
    }
    Wh_FreeStringSetting(backgroundColor);

    settings.backgroundOpacityPercent = Wh_GetIntSetting(L"backgroundOpacityPercent");

    if (settings.backgroundOpacityPercent < 10)
        settings.backgroundOpacityPercent = 10;

    if (settings.backgroundOpacityPercent > 100)
        settings.backgroundOpacityPercent = 100;

    settings.backgroundAlpha = (BYTE)((settings.backgroundOpacityPercent * 255) / 100);

    settings.backgroundGradientEnabled = Wh_GetIntSetting(L"backgroundGradientEnabled");

    const wchar_t* backgroundGradientColor2 = Wh_GetStringSetting(L"backgroundGradientColor2");
    if (!ParseHexColor(backgroundGradientColor2 ? backgroundGradientColor2 : L"#80507080",
                       &settings.backgroundGradientAlpha2,
                       &settings.backgroundGradientColor2)) {
        settings.backgroundGradientAlpha2 = settings.backgroundAlpha;
        settings.backgroundGradientColor2 = RGB(80, 112, 128);
    }
    Wh_FreeStringSetting(backgroundGradientColor2);

    const wchar_t* backgroundGradientDirection = Wh_GetStringSetting(L"backgroundGradientDirection");
    settings.backgroundGradientDirection = backgroundGradientDirection ? backgroundGradientDirection : L"Vertical";
    Wh_FreeStringSetting(backgroundGradientDirection);
    settings.backgroundGradientHorizontal =
    _wcsicmp(settings.backgroundGradientDirection.c_str(), L"Horizontal") == 0;

    settings.backgroundGradientDiagonalDown =
    _wcsicmp(settings.backgroundGradientDirection.c_str(), L"DiagonalDown") == 0;

    settings.backgroundGradientDiagonalUp =
    _wcsicmp(settings.backgroundGradientDirection.c_str(), L"DiagonalUp") == 0;

    settings.backgroundCornerRadius = Wh_GetIntSetting(L"backgroundCornerRadius");
    if (settings.backgroundCornerRadius < 0)
        settings.backgroundCornerRadius = 0;
    if (settings.backgroundCornerRadius > 100)
        settings.backgroundCornerRadius = 100;

    settings.backgroundBorderSize = Wh_GetIntSetting(L"backgroundBorderSize");
    if (settings.backgroundBorderSize < 0)
        settings.backgroundBorderSize = 0;
    if (settings.backgroundBorderSize > 20)
        settings.backgroundBorderSize = 20;

    const wchar_t* backgroundBorderColor = Wh_GetStringSetting(L"backgroundBorderColor");
    if (!ParseHexColor(backgroundBorderColor ? backgroundBorderColor : L"#50FFFFFF",
                       &settings.backgroundBorderAlpha,
                       &settings.backgroundBorderColor)) {
        settings.backgroundBorderAlpha = 0x50;
        settings.backgroundBorderColor = RGB(255, 255, 255);
    }
    Wh_FreeStringSetting(backgroundBorderColor);

    settings.hideUnavailableRows = Wh_GetIntSetting(L"hideUnavailableRows");

    const wchar_t* rows = Wh_GetStringSetting(L"rows");
    settings.rowsText = rows ? rows : L"RAM Used | Value0";
    Wh_FreeStringSetting(rows);

    settings.rows = ParseRows(settings.rowsText);

    RecreateFont();

    settings.showWaterAge = Wh_GetIntSetting(L"showWaterAge");

    const wchar_t* waterAgeLabel = Wh_GetStringSetting(L"waterAgeLabel");
    settings.waterAgeLabel = waterAgeLabel ? waterAgeLabel : L"WATER AGE";
    Wh_FreeStringSetting(waterAgeLabel);

    const wchar_t* waterFillDate = Wh_GetStringSetting(L"waterFillDate");
    settings.waterFillDate = waterFillDate ? waterFillDate : L"2024-11-18";
    Wh_FreeStringSetting(waterFillDate);

    settings.waterMaxAgeDays = Wh_GetIntSetting(L"waterMaxAgeDays");
    if (settings.waterMaxAgeDays < 1)
        settings.waterMaxAgeDays = 730;

    settings.waterAgeWarnPercent = Wh_GetIntSetting(L"waterAgeWarnPercent");
    if (settings.waterAgeWarnPercent < 0)
        settings.waterAgeWarnPercent = 70;
    if (settings.waterAgeWarnPercent > 100)
        settings.waterAgeWarnPercent = 100;

    settings.waterAgeAlarmPercent = Wh_GetIntSetting(L"waterAgeAlarmPercent");
    if (settings.waterAgeAlarmPercent < 0)
        settings.waterAgeAlarmPercent = 90;
    if (settings.waterAgeAlarmPercent > 100)
        settings.waterAgeAlarmPercent = 100;

    if (settings.waterAgeAlarmPercent < settings.waterAgeWarnPercent)
        settings.waterAgeAlarmPercent = settings.waterAgeWarnPercent;

    const wchar_t* waterAgeWarnColor = Wh_GetStringSetting(L"waterAgeWarnColor");
    if (!ParseHexColor(waterAgeWarnColor ? waterAgeWarnColor : L"#FFFFAA00",
                   &settings.waterAgeWarnAlpha,
                   &settings.waterAgeWarnColor)) {
        settings.waterAgeWarnAlpha = 0xFF;
        settings.waterAgeWarnColor = RGB(255, 170, 0);
    }   
    Wh_FreeStringSetting(waterAgeWarnColor);

    const wchar_t* waterAgeAlarmColor = Wh_GetStringSetting(L"waterAgeAlarmColor");
    if (!ParseHexColor(waterAgeAlarmColor ? waterAgeAlarmColor : L"#FFFF3333",
                   &settings.waterAgeAlarmAlpha,
                   &settings.waterAgeAlarmColor)) {
        settings.waterAgeAlarmAlpha = 0xFF;
        settings.waterAgeAlarmColor = RGB(255, 51, 51);
    }
    Wh_FreeStringSetting(waterAgeAlarmColor);

    settings.waterAgeTopGap = Wh_GetIntSetting(L"waterAgeTopGap");
    if (settings.waterAgeTopGap < 0)
        settings.waterAgeTopGap = 0;

    settings.showWaterAgeSeparator = Wh_GetIntSetting(L"showWaterAgeSeparator");

    const wchar_t* waterAgeSeparatorColor = Wh_GetStringSetting(L"waterAgeSeparatorColor");
    if (!ParseHexColor(waterAgeSeparatorColor ? waterAgeSeparatorColor : L"#30FFFFFF",
                       &settings.waterAgeSeparatorAlpha,
                       &settings.waterAgeSeparatorColor)) {
        settings.waterAgeSeparatorAlpha = 0x30;
        settings.waterAgeSeparatorColor = RGB(255, 255, 255);
    }
    Wh_FreeStringSetting(waterAgeSeparatorColor);
}

bool IsLeapYear(int year) {
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

int DaysInMonth(int year, int month) {
    static const int days[] = {
        31, 28, 31, 30, 31, 30,
        31, 31, 30, 31, 30, 31
    };

    if (month == 2) {
        return IsLeapYear(year) ? 29 : 28;
    }

    return days[month - 1];
}

bool ParseWaterFillDate(int* year, int* month, int* day) {
    if (!year || !month || !day)
        return false;

    int y = 0;
    int m = 0;
    int d = 0;
    int charsRead = 0;

    if (swscanf_s(
            settings.waterFillDate.c_str(),
            L"%d-%d-%d%n",
            &y,
            &m,
            &d,
            &charsRead
        ) != 3) {
        return false;
    }

    if (charsRead <= 0 ||
        (size_t)charsRead != settings.waterFillDate.length()) {
        return false;
    }

    if (y < 1900 || y > 2100)
        return false;

    if (m < 1 || m > 12)
        return false;

    if (d < 1 || d > DaysInMonth(y, m))
        return false;

    *year = y;
    *month = m;
    *day = d;
    return true;
}

int CalculateWaterAgeDays() {
    int year = 0;
    int month = 0;
    int day = 0;

    if (!ParseWaterFillDate(&year, &month, &day))
        return -1;

    SYSTEMTIME fillSt = {};
    fillSt.wYear = (WORD)year;
    fillSt.wMonth = (WORD)month;
    fillSt.wDay = (WORD)day;
    fillSt.wHour = 0;
    fillSt.wMinute = 0;
    fillSt.wSecond = 0;
    fillSt.wMilliseconds = 0;

    SYSTEMTIME nowSt = {};
    GetLocalTime(&nowSt);

    FILETIME fillFt = {};
    FILETIME nowFt = {};

    if (!SystemTimeToFileTime(&fillSt, &fillFt))
        return -1;

    if (!SystemTimeToFileTime(&nowSt, &nowFt))
        return -1;

    ULARGE_INTEGER fill = {};
    fill.LowPart = fillFt.dwLowDateTime;
    fill.HighPart = fillFt.dwHighDateTime;

    ULARGE_INTEGER now = {};
    now.LowPart = nowFt.dwLowDateTime;
    now.HighPart = nowFt.dwHighDateTime;

    if (now.QuadPart < fill.QuadPart)
        return 0;

    const ULONGLONG ticksPerDay = 10000000ULL * 60ULL * 60ULL * 24ULL;
    ULONGLONG days = (now.QuadPart - fill.QuadPart) / ticksPerDay;

    if (days > INT_MAX)
        return INT_MAX;

    return (int)days;
}

std::wstring BuildWaterAgeValue(int ageDays) {
    if (ageDays < 0)
        return L"invalid date";

    wchar_t buffer[128] = {};
    swprintf_s(
        buffer,
        L"%d / %d days",
        ageDays,
        settings.waterMaxAgeDays
    );

    return buffer;
}

COLORREF GetWaterAgeTextColor(int ageDays) {
    if (ageDays < 0 || settings.waterMaxAgeDays <= 0)
        return settings.textColor;

    int percent = (int)(((long long)ageDays * 100) / settings.waterMaxAgeDays);

    if (percent >= settings.waterAgeAlarmPercent)
        return settings.waterAgeAlarmColor;

    if (percent >= settings.waterAgeWarnPercent)
        return settings.waterAgeWarnColor;

    return settings.textColor;
}

HINSTANCE GetCurrentModuleHandle() {
    HINSTANCE hInst = nullptr;

    GetModuleHandleExW(
        GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
            GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
        reinterpret_cast<LPCWSTR>(&GetCurrentModuleHandle),
        &hInst
    );

    return hInst;
}

HWND GetOverlayInsertAfter() {
    return settings.alwaysOnTop ? HWND_TOPMOST : HWND_NOTOPMOST;
}

void UpdateWindowRegion(HWND hwnd) {
    if (!hwnd)
        return;

    if (!settings.backgroundEnabled) {
        SetWindowRgn(hwnd, nullptr, TRUE);
        return;
    }

    if (settings.backgroundCornerRadius <= 0) {
        SetWindowRgn(hwnd, nullptr, TRUE);
        return;
    }

    int radius = settings.backgroundCornerRadius * 2;

    HRGN region = CreateRoundRectRgn(
        0,
        0,
        settings.width + 1,
        settings.height + 1,
        radius,
        radius
    );

    if (region) {
        SetWindowRgn(hwnd, region, TRUE);
        // Do not delete region after SetWindowRgn. Windows owns it now.
    }
}

COLORREF InterpolateColor(COLORREF color1, COLORREF color2, int pos, int maxPos) {
    if (maxPos <= 0)
        return color1;

    int r1 = GetRValue(color1);
    int g1 = GetGValue(color1);
    int b1 = GetBValue(color1);

    int r2 = GetRValue(color2);
    int g2 = GetGValue(color2);
    int b2 = GetBValue(color2);

    int r = r1 + ((r2 - r1) * pos) / maxPos;
    int g = g1 + ((g2 - g1) * pos) / maxPos;
    int b = b1 + ((b2 - b1) * pos) / maxPos;

    return RGB(r, g, b);
}

TRIVERTEX MakeGradientVertex(LONG x, LONG y, COLORREF color) {
    TRIVERTEX vertex = {};
    vertex.x = x;
    vertex.y = y;
    vertex.Red = (COLOR16)(GetRValue(color) << 8);
    vertex.Green = (COLOR16)(GetGValue(color) << 8);
    vertex.Blue = (COLOR16)(GetBValue(color) << 8);
    vertex.Alpha = 0;
    return vertex;
}

void FillGradientBackground(HDC hdc, const RECT& rc) {
    bool horizontal =
        settings.backgroundGradientHorizontal;

    bool diagonalDown =
        settings.backgroundGradientDiagonalDown;

    bool diagonalUp =
        settings.backgroundGradientDiagonalUp;

    if (horizontal || (!diagonalDown && !diagonalUp &&
        _wcsicmp(settings.backgroundGradientDirection.c_str(), L"Vertical") != 0)) {
        TRIVERTEX vertices[2] = {
            MakeGradientVertex(rc.left, rc.top, settings.backgroundColor),
            MakeGradientVertex(rc.right, rc.bottom, settings.backgroundGradientColor2)
        };

        GRADIENT_RECT gradientRect = { 0, 1 };

        GradientFill(
            hdc,
            vertices,
            2,
            &gradientRect,
            1,
            GRADIENT_FILL_RECT_H
        );

        return;
    }

    if (!diagonalDown && !diagonalUp) {
        TRIVERTEX vertices[2] = {
            MakeGradientVertex(rc.left, rc.top, settings.backgroundColor),
            MakeGradientVertex(rc.right, rc.bottom, settings.backgroundGradientColor2)
        };

        GRADIENT_RECT gradientRect = { 0, 1 };

        GradientFill(
            hdc,
            vertices,
            2,
            &gradientRect,
            1,
            GRADIENT_FILL_RECT_V
        );

        return;
    }

    COLORREF middleColor = InterpolateColor(
        settings.backgroundColor,
        settings.backgroundGradientColor2,
        1,
        2
    );

    TRIVERTEX vertices[4] = {};

    if (diagonalDown) {
        // Top-left -> bottom-right
        vertices[0] = MakeGradientVertex(rc.left,  rc.top,    settings.backgroundColor);
        vertices[1] = MakeGradientVertex(rc.right, rc.top,    middleColor);
        vertices[2] = MakeGradientVertex(rc.left,  rc.bottom, middleColor);
        vertices[3] = MakeGradientVertex(rc.right, rc.bottom, settings.backgroundGradientColor2);
    } else {
        // Bottom-left -> top-right
        vertices[0] = MakeGradientVertex(rc.left,  rc.top,    middleColor);
        vertices[1] = MakeGradientVertex(rc.right, rc.top,    settings.backgroundGradientColor2);
        vertices[2] = MakeGradientVertex(rc.left,  rc.bottom, settings.backgroundColor);
        vertices[3] = MakeGradientVertex(rc.right, rc.bottom, middleColor);
    }

    GRADIENT_TRIANGLE triangles[2] = {
        { 0, 1, 2 },
        { 1, 3, 2 }
    };

    GradientFill(
        hdc,
        vertices,
        4,
        triangles,
        2,
        GRADIENT_FILL_TRIANGLE
    );
}

void DrawOverlayBorder(HDC hdc, const RECT& rc) {
    if (settings.backgroundBorderSize <= 0)
        return;

    HPEN borderPen = CreatePen(
        PS_SOLID,
        settings.backgroundBorderSize,
        settings.backgroundBorderColor
    );

    HGDIOBJ oldPen = SelectObject(hdc, borderPen);
    HGDIOBJ oldBrush = SelectObject(hdc, GetStockObject(NULL_BRUSH));

    if (settings.backgroundCornerRadius > 0) {
        int radius = settings.backgroundCornerRadius * 2;
        RoundRect(
            hdc,
            rc.left,
            rc.top,
            rc.right,
            rc.bottom,
            radius,
            radius
        );
    } else {
        Rectangle(
            hdc,
            rc.left,
            rc.top,
            rc.right,
            rc.bottom
        );
    }

    SelectObject(hdc, oldBrush);
    SelectObject(hdc, oldPen);
    DeleteObject(borderPen);
}

void DrawOverlayBackground(HDC hdc, const RECT& rc) {
    if (!settings.backgroundEnabled) {
        HBRUSH clearBrush = CreateSolidBrush(TRANSPARENT_COLOR_KEY);
        FillRect(hdc, &rc, clearBrush);
        DeleteObject(clearBrush);
        return;
    }

    if (settings.backgroundGradientEnabled) {
        HRGN clipRegion = nullptr;

        if (settings.backgroundCornerRadius > 0) {
            int radius = settings.backgroundCornerRadius * 2;
            clipRegion = CreateRoundRectRgn(
                rc.left,
                rc.top,
                rc.right + 1,
                rc.bottom + 1,
                radius,
                radius
            );

            if (clipRegion) {
                SelectClipRgn(hdc, clipRegion);
            }
        }

        FillGradientBackground(hdc, rc);

        if (clipRegion) {
            SelectClipRgn(hdc, nullptr);
            DeleteObject(clipRegion);
        }

        DrawOverlayBorder(hdc, rc);
        return;
    }

    HBRUSH bgBrush = CreateSolidBrush(settings.backgroundColor);

    HPEN borderPen = nullptr;
    if (settings.backgroundBorderSize > 0) {
        borderPen = CreatePen(
            PS_SOLID,
            settings.backgroundBorderSize,
            settings.backgroundBorderColor
        );
    } else {
        borderPen = CreatePen(PS_NULL, 0, RGB(0, 0, 0));
    }

    HGDIOBJ oldBrush = SelectObject(hdc, bgBrush);
    HGDIOBJ oldPen = SelectObject(hdc, borderPen);

    if (settings.backgroundCornerRadius > 0) {
        int radius = settings.backgroundCornerRadius * 2;
        RoundRect(
            hdc,
            rc.left,
            rc.top,
            rc.right,
            rc.bottom,
            radius,
            radius
        );
    } else {
        Rectangle(
            hdc,
            rc.left,
            rc.top,
            rc.right,
            rc.bottom
        );
    }

    SelectObject(hdc, oldPen);
    SelectObject(hdc, oldBrush);

    DeleteObject(borderPen);
    DeleteObject(bgBrush);
}

void UpdateLayeredAttributes(HWND hwnd);
void RegisterToggleHotkey(HWND hwnd);
void RegisterDragHotkey(HWND hwnd);
void ToggleOverlayVisibility(HWND hwnd);
void ToggleDragMode(HWND hwnd);
void UpdateClickThroughState(HWND hwnd);
void SaveDraggedPosition();
void RegisterExportHotkey(HWND hwnd);
void ExportRegistryHtml();
int CalculateOverlayAutoHeight(HWND hwnd);
void UpdateAutoHeight(HWND hwnd);
void ApplyOverlayWindowSize(HWND hwnd);

int GetAlignedLabelX(HDC hdc, const std::wstring& label);

int GetAlignedValueX(HDC hdc, const std::wstring& value) {
    int x = settings.valueColumnX;

    if (settings.valueAlignRight) {
        SIZE textSize = {};
        if (GetTextExtentPoint32W(
                hdc,
                value.c_str(),
                (int)value.length(),
                &textSize)) {
            x = settings.valueColumnX + settings.valueColumnWidth - textSize.cx;
        }
    }

    return x;
}

int GetAlignedLabelX(HDC hdc, const std::wstring& label) {
    int x = settings.paddingLeft;

    if (settings.labelAlignRight) {
        SIZE textSize = {};
        if (GetTextExtentPoint32W(
                hdc,
                label.c_str(),
                (int)label.length(),
                &textSize)) {
            x = settings.paddingLeft + settings.labelColumnWidth - textSize.cx;
        }
    }

    return x;
}

void UpdateLayeredAttributes(HWND hwnd) {
    if (!hwnd)
        return;

    if (settings.backgroundEnabled) {
        SetLayeredWindowAttributes(
            hwnd,
            0,
            settings.backgroundAlpha,
            LWA_ALPHA
        );
    } else {
        SetLayeredWindowAttributes(
            hwnd,
            TRANSPARENT_COLOR_KEY,
            255,
            LWA_COLORKEY
        );
    }
}

int GetVisibleRowCount() {
    int count = 0;

    for (const RowConfig& row : settings.rows) {
        if (settings.hideUnavailableRows && row.cachedValue == L"N/A") {
            continue;
        }

        ++count;
    }

    return count;
}

int CalculateOverlayAutoHeight(HWND hwnd) {
    HDC hdc = hwnd ? GetDC(hwnd) : GetDC(nullptr);

    if (!hdc) {
        return settings.height;
    }

    HFONT oldFont = nullptr;
    if (g_font) {
        oldFont = (HFONT)SelectObject(hdc, g_font);
    }

    TEXTMETRICW tm = {};
    GetTextMetricsW(hdc, &tm);

    if (oldFont) {
        SelectObject(hdc, oldFont);
    }

    ReleaseDC(hwnd ? hwnd : nullptr, hdc);

    int rowHeight = tm.tmHeight + settings.rowSpacing;

    if (rowHeight < 1) {
        rowHeight = settings.fontSize + settings.rowSpacing + 4;
    }

    int newHeight =
        settings.paddingTop +
        (GetVisibleRowCount() * rowHeight) +
        settings.paddingBottom;

    if (settings.showWaterAge) {
        newHeight += settings.waterAgeTopGap;

        if (settings.showWaterAgeSeparator) {
            newHeight += settings.waterAgeTopGap;
        }

        newHeight += rowHeight;
    }

    if (newHeight < 50) {
        newHeight = 50;
    }

    if (newHeight > 4000) {
        newHeight = 4000;
    }

    return newHeight;
}

void UpdateAutoHeight(HWND hwnd) {
    if (!settings.autoHeight) {
        return;
    }

    settings.height = CalculateOverlayAutoHeight(hwnd);
}

void ApplyOverlayWindowSize(HWND hwnd) {
    if (!hwnd) {
        return;
    }

    UpdateAutoHeight(hwnd);

    SetWindowPos(
        hwnd,
        GetOverlayInsertAfter(),
        settings.x,
        settings.y,
        settings.width,
        settings.height,
        SWP_NOACTIVATE
    );

    UpdateWindowRegion(hwnd);
}

void DrawColumnSeparator(HDC hdc, int topY, int bottomY) {
    if (!settings.showColumnSeparator)
        return;

    HPEN pen = CreatePen(
        PS_SOLID,
        1,
        settings.columnSeparatorColor
    );

    HGDIOBJ oldPen = SelectObject(hdc, pen);

    MoveToEx(hdc, settings.columnSeparatorX, topY, nullptr);
    LineTo(hdc, settings.columnSeparatorX, bottomY);

    SelectObject(hdc, oldPen);
    DeleteObject(pen);
}

void DrawWaterAgeSeparator(HDC hdc, int y) {
    if (!settings.showWaterAgeSeparator)
        return;

    int leftX = settings.paddingLeft;
    int rightX = settings.width - settings.paddingLeft;

    if (rightX <= leftX)
        return;

    HPEN pen = CreatePen(
        PS_SOLID,
        1,
        settings.waterAgeSeparatorColor
    );

    HGDIOBJ oldPen = SelectObject(hdc, pen);

    MoveToEx(hdc, leftX, y, nullptr);
    LineTo(hdc, rightX, y);

    SelectObject(hdc, oldPen);
    DeleteObject(pen);
}

LRESULT CALLBACK OverlayWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_TIMER:
        RefreshCachedRegistryValues();

        if (settings.autoHeight) {
            ApplyOverlayWindowSize(hwnd);
        }

        InvalidateRect(hwnd, nullptr, TRUE);
        return 0;

        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);

            RECT rc;
            GetClientRect(hwnd, &rc);

            DrawOverlayBackground(hdc, rc);

            SetBkMode(hdc, TRANSPARENT);
            SetTextColor(hdc, settings.textColor);

            HFONT oldFont = nullptr;
            if (g_font) {
                oldFont = (HFONT)SelectObject(hdc, g_font);
            }

            int y = settings.paddingTop;

            int contentTopY = y;

            TEXTMETRICW tm = {};
            GetTextMetricsW(hdc, &tm);

            int rowHeight = tm.tmHeight + settings.rowSpacing;

            for (const RowConfig& row : settings.rows) {
                const std::wstring& value = row.cachedValue;

                if (settings.hideUnavailableRows && value == L"N/A") {
                    continue;
                }

                SetTextColor(hdc, settings.labelColor);

                int alignedLabelX = GetAlignedLabelX(hdc, row.label);

                TextOutW(
                    hdc,
                    alignedLabelX,
                    y,
                    row.label.c_str(),
                    (int)row.label.length()
                );

                SetTextColor(hdc, settings.valueColor);

                int alignedValueX = GetAlignedValueX(hdc, value);

                TextOutW(
                    hdc,
                    alignedValueX,
                    y,
                    value.c_str(),
                    (int)value.length()
                );

                y += rowHeight;
            }

            int separatorBottomY = y;

                if (settings.showWaterAge) {
                    y += settings.waterAgeTopGap;

                if (settings.showWaterAgeSeparator) {
                    DrawWaterAgeSeparator(hdc, y);
                    y += settings.waterAgeTopGap;
                }

                int ageDays = CalculateWaterAgeDays();
                std::wstring value = BuildWaterAgeValue(ageDays);

                COLORREF oldTextColor = SetTextColor(hdc, GetWaterAgeTextColor(ageDays));

                int alignedWaterAgeLabelX = GetAlignedLabelX(hdc, settings.waterAgeLabel);

                TextOutW(
                    hdc,
                    alignedWaterAgeLabelX,
                    y,
                    settings.waterAgeLabel.c_str(),
                    (int)settings.waterAgeLabel.length()
                );

            int alignedWaterAgeValueX = GetAlignedValueX(hdc, value);

                TextOutW(
                    hdc,
                    alignedWaterAgeValueX,
                    y,
                    value.c_str(),
                    (int)value.length()
                );

                SetTextColor(hdc, oldTextColor);

            y += rowHeight;
}

DrawColumnSeparator(hdc, contentTopY, separatorBottomY);

            if (oldFont) {
                SelectObject(hdc, oldFont);
            }

            EndPaint(hwnd, &ps);
            return 0;
        }

        case WM_HOTKEY:
            if (wParam == HOTKEY_TOGGLE_OVERLAY) {
                ToggleOverlayVisibility(hwnd);
                return 0;
            }

            if (wParam == HOTKEY_DRAG_MODE) {
                ToggleDragMode(hwnd);
                return 0;
            }

            if (wParam == HOTKEY_EXPORT_REGISTRY) {
                ExportRegistryHtml();
                return 0;
            }

            break;

        case WM_LBUTTONDOWN:
            if (settings.dragModeEnabled) {
                settings.dragging = true;

                GetCursorPos(&settings.dragStartMouse);

                RECT windowRect = {};
                GetWindowRect(hwnd, &windowRect);

                settings.dragStartWindow.x = windowRect.left;
                settings.dragStartWindow.y = windowRect.top;

                SetCapture(hwnd);
                return 0;
            }

            break;

        case WM_MOUSEMOVE:
            if (settings.dragModeEnabled && settings.dragging) {
                POINT currentMouse = {};
                GetCursorPos(&currentMouse);

                int dx = currentMouse.x - settings.dragStartMouse.x;
                int dy = currentMouse.y - settings.dragStartMouse.y;

                settings.x = settings.dragStartWindow.x + dx;
                settings.y = settings.dragStartWindow.y + dy;

                SetWindowPos(
                    hwnd,
                    GetOverlayInsertAfter(),
                    settings.x,
                    settings.y,
                    settings.width,
                    settings.height,
                    SWP_NOACTIVATE
                );

                return 0;
            }
            break;

        case WM_LBUTTONUP:
            if (settings.dragging) {
                settings.dragging = false;
                ReleaseCapture();

                SaveDraggedPosition();

                Wh_Log(L"Overlay drag finished: x=%d, y=%d", settings.x, settings.y);
                return 0;
            }
            break;

        case WM_APP_SETTINGS_CHANGED:
            LoadSettings();

            ApplyOverlayWindowSize(hwnd);
            UpdateLayeredAttributes(hwnd);
            RegisterToggleHotkey(hwnd);
            RegisterDragHotkey(hwnd);
            RegisterExportHotkey(hwnd);
            UpdateClickThroughState(hwnd);
            KillTimer(hwnd, g_timerId);
            SetTimer(hwnd, g_timerId, settings.refreshIntervalMs, nullptr);

            RefreshCachedRegistryValues();

            InvalidateRect(hwnd, nullptr, TRUE);
            return 0;

        case WM_DESTROY:
            UnregisterHotKey(hwnd, HOTKEY_TOGGLE_OVERLAY);
            UnregisterHotKey(hwnd, HOTKEY_DRAG_MODE);
            UnregisterHotKey(hwnd, HOTKEY_EXPORT_REGISTRY);
            KillTimer(hwnd, g_timerId);

            if (g_hwnd == hwnd) {
                g_hwnd = nullptr;
            }

            PostQuitMessage(0);
            return 0;
    }

    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

UINT GetToggleHotkeyModifiers() {
    UINT modifiers = MOD_NOREPEAT;

    if (settings.toggleHotkeyCtrl)
        modifiers |= MOD_CONTROL;

    if (settings.toggleHotkeyAlt)
        modifiers |= MOD_ALT;

    if (settings.toggleHotkeyShift)
        modifiers |= MOD_SHIFT;

    return modifiers;
}

UINT GetToggleHotkeyVk() {
    if (settings.toggleHotkeyKey.empty())
        return 'H';

    wchar_t ch = settings.toggleHotkeyKey[0];

    if (ch >= L'a' && ch <= L'z')
        ch = ch - L'a' + L'A';

    if (ch >= L'A' && ch <= L'Z')
        return (UINT)ch;

    if (ch >= L'0' && ch <= L'9')
        return (UINT)ch;

    return 'H';
}

bool HasHotkeyModifier(UINT modifiers) {
    return (modifiers & (MOD_CONTROL | MOD_ALT | MOD_SHIFT)) != 0;
}

void RegisterToggleHotkey(HWND hwnd) {
    UnregisterHotKey(hwnd, HOTKEY_TOGGLE_OVERLAY);

    if (!settings.enableToggleHotkey)
        return;

    UINT modifiers = GetToggleHotkeyModifiers();
    UINT vk = GetToggleHotkeyVk();

    if (!HasHotkeyModifier(modifiers)) {
        Wh_Log(L"Toggle hotkey needs at least one modifier; not registering");
        return;
    }

    if (!RegisterHotKey(hwnd, HOTKEY_TOGGLE_OVERLAY, modifiers, vk)) {
        Wh_Log(L"Failed to register toggle hotkey");
    }
}

void ToggleOverlayVisibility(HWND hwnd) {
    settings.overlayVisible = !settings.overlayVisible;

    ShowWindow(
        hwnd,
        settings.overlayVisible ? SW_SHOWNOACTIVATE : SW_HIDE
    );
}

UINT GetDragHotkeyModifiers() {
    UINT modifiers = MOD_NOREPEAT;

    if (settings.dragHotkeyCtrl)
        modifiers |= MOD_CONTROL;

    if (settings.dragHotkeyAlt)
        modifiers |= MOD_ALT;

    if (settings.dragHotkeyShift)
        modifiers |= MOD_SHIFT;

    return modifiers;
}

UINT GetDragHotkeyVk() {
    if (settings.dragHotkeyKey.empty())
        return 'D';

    wchar_t ch = settings.dragHotkeyKey[0];

    if (ch >= L'a' && ch <= L'z')
        ch = ch - L'a' + L'A';

    if (ch >= L'A' && ch <= L'Z')
        return (UINT)ch;

    if (ch >= L'0' && ch <= L'9')
        return (UINT)ch;

    return 'D';
}

void RegisterDragHotkey(HWND hwnd) {
    UnregisterHotKey(hwnd, HOTKEY_DRAG_MODE);

    if (!settings.enableDragHotkey)
        return;

    UINT modifiers = GetDragHotkeyModifiers();
    UINT vk = GetDragHotkeyVk();

    if (!HasHotkeyModifier(modifiers)) {
        Wh_Log(L"Drag hotkey needs at least one modifier; not registering");
        return;
    }

    if (!RegisterHotKey(hwnd, HOTKEY_DRAG_MODE, modifiers, vk)) {
        Wh_Log(L"Failed to register drag hotkey");
    }
}

UINT GetExportHotkeyModifiers() {
    UINT modifiers = MOD_NOREPEAT;

    if (settings.exportHotkeyCtrl)
        modifiers |= MOD_CONTROL;

    if (settings.exportHotkeyAlt)
        modifiers |= MOD_ALT;

    if (settings.exportHotkeyShift)
        modifiers |= MOD_SHIFT;

    return modifiers;
}

UINT GetExportHotkeyVk() {
    if (settings.exportHotkeyKey.empty())
        return 'R';

    wchar_t ch = settings.exportHotkeyKey[0];

    if (ch >= L'a' && ch <= L'z')
        ch = ch - L'a' + L'A';

    if (ch >= L'A' && ch <= L'Z')
        return (UINT)ch;

    if (ch >= L'0' && ch <= L'9')
        return (UINT)ch;

    return 'R';
}

void RegisterExportHotkey(HWND hwnd) {
    UnregisterHotKey(hwnd, HOTKEY_EXPORT_REGISTRY);

    if (!settings.enableExportHotkey)
        return;

    UINT modifiers = GetExportHotkeyModifiers();
    UINT vk = GetExportHotkeyVk();

    if (!HasHotkeyModifier(modifiers)) {
        Wh_Log(L"Registry export hotkey needs at least one modifier; not registering");
        return;
    }

    if (!RegisterHotKey(hwnd, HOTKEY_EXPORT_REGISTRY, modifiers, vk)) {
        Wh_Log(L"Failed to register registry export hotkey");
    }
}

void UpdateClickThroughState(HWND hwnd) {
    if (!hwnd)
        return;

    LONG_PTR exStyle = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);

    if (settings.dragModeEnabled) {
        exStyle &= ~WS_EX_TRANSPARENT;
    } else {
        exStyle |= WS_EX_TRANSPARENT;
    }

    SetWindowLongPtrW(hwnd, GWL_EXSTYLE, exStyle);

    SetWindowPos(
        hwnd,
        GetOverlayInsertAfter(),
        0,
        0,
        0,
        0,
        SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_FRAMECHANGED
    );
}

void ToggleDragMode(HWND hwnd) {
    settings.dragModeEnabled = !settings.dragModeEnabled;
    settings.dragging = false;

    UpdateClickThroughState(hwnd);

    if (settings.dragModeEnabled) {
        Wh_Log(L"Drag mode enabled");
    } else {
        Wh_Log(L"Drag mode disabled");
    }

    InvalidateRect(hwnd, nullptr, TRUE);
}

void SaveDraggedPosition() {
    if (!settings.rememberDraggedPosition)
        return;

    BOOL savedX = Wh_SetIntValue(L"draggedX", settings.x);
    BOOL savedY = Wh_SetIntValue(L"draggedY", settings.y);

    if (savedX && savedY) {
        Wh_Log(L"Saved dragged position: x=%d, y=%d", settings.x, settings.y);
    } else {
        Wh_Log(L"Failed to save dragged position");
    }
}

struct ExportSensorRow {
    int index;
    std::wstring sensor;
    std::wstring label;
    std::wstring value;
    std::wstring valueRaw;
};

std::wstring EscapeHtml(const std::wstring& text) {
    std::wstring out;

    for (wchar_t ch : text) {
        switch (ch) {
            case L'&':
                out += L"&amp;";
                break;
            case L'<':
                out += L"&lt;";
                break;
            case L'>':
                out += L"&gt;";
                break;
            case L'"':
                out += L"&quot;";
                break;
            case L'\'':
                out += L"&#39;";
                break;
            default:
                out += ch;
                break;
        }
    }

    return out;
}

std::wstring MakeIndexedRegistryName(const wchar_t* prefix, int index) {
    wchar_t buffer[64] = {};
    swprintf_s(buffer, L"%s%d", prefix, index);
    return buffer;
}

std::wstring GetDesktopExportPath() {
    constexpr const wchar_t* fileName = L"HWiNFO_Registry_Output_Windhawk.html";

    PWSTR desktopPath = nullptr;

    if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_Desktop, 0, nullptr, &desktopPath))) {
        std::wstring path = desktopPath;
        CoTaskMemFree(desktopPath);

        if (!path.empty() && path.back() != L'\\') {
            path += L'\\';
        }

        path += fileName;
        return path;
    }

    wchar_t tempPath[MAX_PATH] = {};
    DWORD tempPathLength = GetTempPathW(ARRAYSIZE(tempPath), tempPath);

    if (tempPathLength > 0 && tempPathLength < ARRAYSIZE(tempPath)) {
        std::wstring path = tempPath;

        if (!path.empty() && path.back() != L'\\') {
            path += L'\\';
        }

        path += fileName;
        return path;
    }

    return fileName;
}

bool WriteUtf8File(const std::wstring& path, const std::wstring& text) {
    int byteCount = WideCharToMultiByte(
        CP_UTF8,
        0,
        text.c_str(),
        (int)text.size(),
        nullptr,
        0,
        nullptr,
        nullptr
    );

    if (byteCount == 0 && !text.empty()) {
        return false;
    }

    std::string utf8;

    if (byteCount > 0) {
        utf8.resize(byteCount);

        int converted = WideCharToMultiByte(
            CP_UTF8,
            0,
            text.c_str(),
            (int)text.size(),
            utf8.data(),
            byteCount,
            nullptr,
            nullptr
        );

        if (converted != byteCount) {
            return false;
        }
    }

    HANDLE file = CreateFileW(
        path.c_str(),
        GENERIC_WRITE,
        0,
        nullptr,
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        nullptr
    );

    if (file == INVALID_HANDLE_VALUE) {
        return false;
    }

    DWORD written = 0;
    const BYTE bom[] = {0xEF, 0xBB, 0xBF};

    if (!WriteFile(file, bom, sizeof(bom), &written, nullptr) ||
        written != sizeof(bom)) {
        CloseHandle(file);
        return false;
    }

    if (!utf8.empty()) {
        written = 0;

        if (!WriteFile(
                file,
                utf8.data(),
                (DWORD)utf8.size(),
                &written,
                nullptr
            ) ||
            written != utf8.size()) {
            CloseHandle(file);
            return false;
        }
    }

    CloseHandle(file);
    return true;
}

std::vector<ExportSensorRow> ReadRegistryRowsForExport() {
    std::vector<ExportSensorRow> rows;

    HKEY root = GetConfiguredRegistryRoot();

    HKEY key = nullptr;
    LONG result = RegOpenKeyExW(
        root,
        settings.registryPath.c_str(),
        0,
        KEY_READ,
        &key
    );

    if (result != ERROR_SUCCESS)
        return rows;

    int consecutiveMissing = 0;

    for (int index = 0; index < 1000; ++index) {
        std::wstring sensor = ReadRegistryStringFromOpenKey(
            key,
            MakeIndexedRegistryName(L"Sensor", index)
        );

        std::wstring label = ReadRegistryStringFromOpenKey(
            key,
            MakeIndexedRegistryName(L"Label", index)
        );

        if (sensor == L"N/A" && label == L"N/A") {
            ++consecutiveMissing;

            if (consecutiveMissing >= 25)
                break;

            continue;
        }

        consecutiveMissing = 0;

        ExportSensorRow row = {};
        row.index = index;
        row.sensor = sensor;
        row.label = label;
        row.value = ReadRegistryStringFromOpenKey(
            key,
            MakeIndexedRegistryName(L"Value", index)
        );
        row.valueRaw = ReadRegistryStringFromOpenKey(
            key,
            MakeIndexedRegistryName(L"ValueRaw", index)
        );

        rows.push_back(row);
    }

    RegCloseKey(key);

    return rows;
}

std::wstring BuildRegistryExportHtml(const std::vector<ExportSensorRow>& rows) {
    std::wstring html;

    html += LR"WHHTML(<!DOCTYPE HTML>
<html lang="en-US">
<head>
<title>HWiNFO Registry Reader - Windhawk Export</title>
<meta http-equiv="content-type" content="text/html;charset=utf-8" />
<style>
body {
    font-family: Segoe UI, Arial, sans-serif;
    font-size: 14px;
    color: #2F2F2F;
    background-color: #E1E3E6;
    margin: 20px;
}

h1 {
    margin: 0 0 12px 0;
}

.info {
    margin-bottom: 14px;
}

.controls {
    background: #FAFAFA;
    border: 1px solid #2F2F2F;
    box-shadow: 0 0 15px 3px #9E9E9E;
    padding: 12px;
    margin-bottom: 18px;
}

textarea {
    width: 100%;
    height: 110px;
    font-family: Consolas, Cascadia Mono, monospace;
    font-size: 13px;
    box-sizing: border-box;
}

input.customName {
    width: 100%;
    box-sizing: border-box;
    font-family: Consolas, Cascadia Mono, monospace;
    font-size: 13px;
    padding: 4px;
}

button {
    margin-top: 8px;
    margin-right: 8px;
    padding: 7px 12px;
    font-weight: 600;
    cursor: pointer;
}

button.moveButton {
    margin: 0 2px;
    padding: 2px 6px;
    font-weight: 600;
}

td.moveCell {
    text-align: center;
    white-space: nowrap;
}

#status {
    margin-top: 8px;
    font-weight: 600;
    color: #0A7A0A;
}

table {
    table-layout: fixed;
    width: 100%;
    background-color: #FAFAFA;
    box-shadow: 0 0 15px 3px #9E9E9E;
}

table, th, td {
    border-style: solid;
    border-width: 1px;
    border-color: #2F2F2F;
    border-collapse: collapse;
    word-wrap: break-word;
}

th {
    background-color: #F7BC81;
    color: #2F2F2F;
    padding: 10px 4px;
}

td {
    padding: 5px;
}

.small {
    font-size: 12px;
    opacity: 0.8;
}
</style>

<script>
function rebuildRows() {
    const tableRows = document.querySelectorAll('tr.sensorRow');
    const rows = [];

    tableRows.forEach(function(tr) {
        const box = tr.querySelector('.rowCheck');
        const input = tr.querySelector('.customName');
        const cell = tr.querySelector('.windhawkRowCell');

        const index = box.dataset.index;
        let name = input.value.trim();

        if (!name) {
            name = input.dataset.fallback || ('Sensor ' + index);
        }

        const rowText = name + ' | Value' + index;

        cell.textContent = rowText;

        if (box.checked) {
            rows.push(rowText);
        }
    });

    document.getElementById('rowsText').value = rows.join('; ');
    document.getElementById('rowCount').textContent = rows.length;
}

function setAllRows(checked) {
    const boxes = document.querySelectorAll('.rowCheck');

    boxes.forEach(function(box) {
        box.checked = checked;
    });

    rebuildRows();
}

function moveRow(button, direction) {
    const row = button.closest('tr');
    const tbody = row.parentNode;
    const rows = Array.from(tbody.querySelectorAll('tr.sensorRow'));
    const index = rows.indexOf(row);

    if (index < 0) {
        return;
    }

    if (direction < 0 && index > 0) {
        tbody.insertBefore(row, rows[index - 1]);
    }

    if (direction > 0 && index < rows.length - 1) {
        tbody.insertBefore(rows[index + 1], row);
    }

    rebuildRows();
}

function getSuggestedName(label) {
    const map = {
        'CPU Package Power': 'CPU W',
        'GPU Power': 'GPU W',
        'PSU Input Power': 'PSU W',
        'GPU 12VHPWR Power': '12VHPWR W',
        'CPU': 'CPUMP',
        'W_PUMP+': 'WPUMP',
        'AIO Pump': 'APUMP',
        'Water In T Sensor': 'W-IN',
        'Water Out T Sensor': 'W-OUT',
        'Water Flow': 'FLOW',
        'Power Sensor': 'COOL W'
    };

    return map[label] || label;
}

function applySuggestedNames() {
    const inputs = document.querySelectorAll('.customName');

    inputs.forEach(function(input) {
        input.value = getSuggestedName(input.dataset.originalLabel || input.value);
    });

    rebuildRows();
}

function applyCustomShortNames() {
    rebuildRows();

    document.getElementById('status').textContent =
        'Applied ' + document.getElementById('rowCount').textContent +
        ' custom short names to Windhawk rows.';
}

async function copyRows() {
    rebuildRows();

    const textarea = document.getElementById('rowsText');
    const text = textarea.value;

    try {
        await navigator.clipboard.writeText(text);
    } catch (e) {
        textarea.focus();
        textarea.select();
        document.execCommand('copy');
    }

    document.getElementById('status').textContent =
        'Copied ' + document.getElementById('rowCount').textContent + ' Windhawk rows to clipboard.';
}

window.addEventListener('DOMContentLoaded', rebuildRows);
</script>
</head>

<body>
<h1>HWiNFO Registry Reader - Windhawk Export</h1>

<div class="info">
    Select the sensors you want and copy the generated Windhawk Rows text.
    <br>
    <span class="small">Format: Display Name | Registry Value</span>
</div>

<div class="controls">
    <b>Windhawk Rows</b>
    <br>
    <textarea id="rowsText" readonly></textarea>
    <br>
    <button onclick="copyRows()">Copy Windhawk Rows</button>
    <button onclick="setAllRows(true)">Select all</button>
    <button onclick="setAllRows(false)">Select none</button>
    <button onclick="applySuggestedNames()">Apply suggested short names</button>
    <button onclick="applyCustomShortNames()">Apply custom short names</button>
    <div id="status"></div>
    <div class="small">Selected rows: <span id="rowCount">0</span></div>
</div>

<table>
<tr>
<th style="width: 4%;">Use</th>
<th style="width: 6%;">Move</th>
<th style="width: 5%;">Index</th>
<th style="width: 22%;">Sensor</th>
<th style="width: 19%;">HWiNFO Label</th>
<th style="width: 14%;">Custom Name</th>
<th style="width: 10%;">Value</th>
<th style="width: 9%;">ValueRaw</th>
<th style="width: 11%;">Windhawk Row</th>
</tr>
<tbody id="sensorRows">
)WHHTML";

    for (const ExportSensorRow& row : rows) {
        std::wstring label = row.label;

        if (label.empty() || label == L"N/A") {
            label = L"Sensor " + std::to_wstring(row.index);
        }

        std::wstring windhawkRow =
            label + L" | Value" + std::to_wstring(row.index);

        html += L"<tr class=\"sensorRow\">";

        html += L"<td style=\"text-align:center;\"><input class=\"rowCheck\" type=\"checkbox\" checked onchange=\"rebuildRows()\" data-index=\"";
        html += std::to_wstring(row.index);
        html += L"\"></td>";

        html += L"<td class=\"moveCell\">";
        html += L"<button class=\"moveButton\" onclick=\"moveRow(this, -1)\">Up</button>";
        html += L"<button class=\"moveButton\" onclick=\"moveRow(this, 1)\">Down</button>";
        html += L"</td>";

        html += L"<td>" + std::to_wstring(row.index) + L"</td>";
        html += L"<td>" + EscapeHtml(row.sensor) + L"</td>";
        html += L"<td>" + EscapeHtml(row.label) + L"</td>";

        html += L"<td><input class=\"customName\" type=\"text\" value=\"";
        html += EscapeHtml(label);
        html += L"\" data-fallback=\"";
        html += EscapeHtml(label);
        html += L"\" data-original-label=\"";
        html += EscapeHtml(label);
        html += L"\" oninput=\"rebuildRows()\"></td>";

        html += L"<td>" + EscapeHtml(row.value) + L"</td>";
        html += L"<td>" + EscapeHtml(row.valueRaw) + L"</td>";

        html += L"<td class=\"windhawkRowCell\">";
        html += EscapeHtml(windhawkRow);
        html += L"</td>";

        html += L"</tr>\n";
    }

    html += L"</tbody></table><br></body></html>";

    return html;
}

std::wstring BuildNoRegistryRowsHtml() {
    std::wstring html;

    html += LR"WHHTML(<!DOCTYPE HTML>
<html lang="en-US">
<head>
<title>HWiNFO Registry Reader - No Gadget Rows Found</title>
<meta http-equiv="content-type" content="text/html;charset=utf-8" />
<style>
body {
    font-family: Segoe UI, Arial, sans-serif;
    font-size: 15px;
    color: #2F2F2F;
    background-color: #E1E3E6;
    margin: 24px;
}
.box {
    background: #FFFFFF;
    border: 1px solid #B0B0B0;
    border-radius: 8px;
    padding: 18px 22px;
    max-width: 900px;
}
h1 {
    margin-top: 0;
}
code {
    background: #F2F2F2;
    padding: 2px 5px;
    border-radius: 4px;
}
</style>
</head>
<body>
<div class="box">
<h1>No HWiNFO Gadget/VSB registry rows found</h1>

<p>The Windhawk export hotkey is working, but no HWiNFO Gadget/VSB sensor rows were found in the configured registry path.</p>

<p>Please open HWiNFO and enable at least one sensor for Gadget reporting:</p>

<ol>
<li>Open <b>HWiNFO Sensors</b></li>
<li>Click <b>Configure Sensors</b></li>
<li>Open the <b>HWiNFO Gadget</b> tab</li>
<li>Enable <b>Enable reporting to Gadget</b></li>
<li>Select one or more sensor values and enable <b>Report value in Gadget</b></li>
<li>Press <b>OK</b></li>
<li>Press the Windhawk export hotkey again</li>
</ol>

<p>Expected registry path:</p>
<p><code>)WHHTML";

    html += EscapeHtml(settings.registryRoot);
    html += L"\\";
    html += EscapeHtml(settings.registryPath);

    html += LR"WHHTML(</code></p>

<p>Expected value names:</p>
<p><code>Sensor0 / Label0 / Value0 / ValueRaw0 / Color0</code></p>

</div>
</body>
</html>
)WHHTML";

    return html;
}

void ExportRegistryHtml() {
    std::vector<ExportSensorRow> rows = ReadRegistryRowsForExport();

    std::wstring path = GetDesktopExportPath();
    std::wstring html;

    if (rows.empty()) {
        Wh_Log(L"Registry export: no HWiNFO rows found");

        MessageBoxW(
            nullptr,
            L"No HWiNFO Gadget/VSB registry rows were found.\n\n"
            L"The export hotkey is working, but HWiNFO is currently not reporting any Gadget values.\n\n"
            L"Please enable at least one sensor in:\n"
            L"HWiNFO Sensors -> Configure Sensors -> HWiNFO Gadget -> Report value in Gadget",
            L"HWiNFO Registry Export",
            MB_OK | MB_ICONINFORMATION
        );

        html = BuildNoRegistryRowsHtml();
    } else {
        html = BuildRegistryExportHtml(rows);
    }

    if (!WriteUtf8File(path, html)) {
        Wh_Log(L"Registry export failed: could not write HTML file");

        std::wstring message =
            L"Failed to write the HWiNFO Registry export HTML file:\n\n" +
            path +
            L"\n\nPlease check folder permissions or try again.";

        MessageBoxW(
            nullptr,
            message.c_str(),
            L"HWiNFO Registry Export",
            MB_OK | MB_ICONERROR
        );

        return;
    }

    if (rows.empty()) {
        Wh_Log(L"Registry export help HTML written: no HWiNFO rows found");
    } else {
        Wh_Log(L"Registry export written: %d rows", (int)rows.size());
    }

    if (settings.openExportHtmlAfterCreate) {
        ShellExecuteW(
            nullptr,
            L"open",
            path.c_str(),
            nullptr,
            nullptr,
            SW_SHOWNORMAL
        );
    }
}

bool CreateOverlayWindow() {
    HINSTANCE hInstance = GetCurrentModuleHandle();

    WNDCLASSEXW wc = {};
    wc.cbSize = sizeof(wc);
    wc.lpfnWndProc = OverlayWndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = OVERLAY_WINDOW_CLASS_NAME;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);

    if (!RegisterClassExW(&wc)) {
        Wh_Log(L"RegisterClassExW failed: %u", GetLastError());
        return false;
    }

    DWORD exStyle = WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW;

    if (settings.alwaysOnTop) {
        exStyle |= WS_EX_TOPMOST;
    }

    RefreshCachedRegistryValues();
    UpdateAutoHeight(nullptr);

    g_hwnd = CreateWindowExW(
        exStyle,
        OVERLAY_WINDOW_CLASS_NAME,
        L"HWiNFO Registry Live Overlay",
        WS_POPUP,
        settings.x,
        settings.y,
        settings.width,
        settings.height,
        nullptr,
        nullptr,
        hInstance,
        nullptr
    );

    if (!g_hwnd) {
        Wh_Log(L"CreateWindowExW failed: %u", GetLastError());
        UnregisterClassW(OVERLAY_WINDOW_CLASS_NAME, hInstance);
        return false;
    }

    UpdateWindowRegion(g_hwnd);
    UpdateLayeredAttributes(g_hwnd);

    settings.overlayVisible = true;
    settings.dragModeEnabled = false;
    settings.dragging = false;

    ShowWindow(g_hwnd, SW_SHOWNOACTIVATE);
    RegisterToggleHotkey(g_hwnd);
    RegisterDragHotkey(g_hwnd);
    RegisterExportHotkey(g_hwnd);
    UpdateClickThroughState(g_hwnd);
    UpdateWindow(g_hwnd);

    SetWindowPos(
        g_hwnd,
        GetOverlayInsertAfter(),
        settings.x,
        settings.y,
        settings.width,
        settings.height,
        SWP_NOACTIVATE
    );

    SetTimer(g_hwnd, g_timerId, settings.refreshIntervalMs, nullptr);

    return true;
}

DWORD WINAPI OverlayThreadProc(LPVOID) {
    MSG msg;

    // Ensure a message queue exists before WhTool_ModUninit's
    // PostThreadMessageW can be relied on to reach it.
    PeekMessageW(&msg, nullptr, WM_USER, WM_USER, PM_NOREMOVE);

    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

    if (!CreateOverlayWindow()) {
        Wh_Log(L"Failed to create overlay window in UI thread");
        UnregisterClassW(OVERLAY_WINDOW_CLASS_NAME, GetCurrentModuleHandle());
        return 1;
    }

    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    if (g_hwnd) {
        DestroyWindow(g_hwnd);
        g_hwnd = nullptr;
    }

    UnregisterClassW(OVERLAY_WINDOW_CLASS_NAME, GetCurrentModuleHandle());

    return 0;
}

BOOL WhTool_ModInit() {
    Wh_Log(L"Init");

    LoadSettings();

    g_uiThread = CreateThread(
        nullptr,
        0,
        OverlayThreadProc,
        nullptr,
        0,
        &g_uiThreadId
    );

    if (!g_uiThread) {
        Wh_Log(L"Failed to create UI thread");
        return FALSE;
    }

    return TRUE;
}

void WhTool_ModSettingsChanged() {
    Wh_Log(L"SettingsChanged");

    if (g_hwnd) {
        PostMessageW(g_hwnd, WM_APP_SETTINGS_CHANGED, 0, 0);
    } else {
        LoadSettings();
    }
}

void WhTool_ModUninit() {
    Wh_Log(L"Uninit");

    if (g_uiThreadId) {
        while (!PostThreadMessageW(g_uiThreadId, WM_QUIT, 0, 0) &&
               g_uiThread &&
               WaitForSingleObject(g_uiThread, 10) == WAIT_TIMEOUT) {
        }
    }

    if (g_uiThread) {
        WaitForSingleObject(g_uiThread, INFINITE);
        CloseHandle(g_uiThread);
        g_uiThread = nullptr;
    }

    g_uiThreadId = 0;

    if (g_font) {
        DeleteObject(g_font);
        g_font = nullptr;
    }
}

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

    WhTool_ModSettingsChanged();
}

void Wh_ModUninit() {
    if (g_isToolModProcessLauncher) {
        return;
    }

    WhTool_ModUninit();
    ExitProcess(0);
}
// clang-format on


