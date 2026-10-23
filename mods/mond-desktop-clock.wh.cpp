// ==WindhawkMod==
// @id mond-desktop-clock
// @name Mond Desktop Clock
// @description A universal desktop clock widget with configurable typography, layout, and placement.
// @version 5.1
// @author Elmidin Mahmud
// @github https://github.com/elmidin
// @include windhawk.exe
// @compilerOptions -lgdi32 -luser32 -lshell32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*

A universal desktop clock widget inspired by the [Mond Rainmeter skin by HipHopium](https://www.deviantart.com/hiphopium). It runs in a dedicated tool process, not in Explorer.

---
**Differentiating Feature Set**
While many desk clock widgets offer customizable styles, Mond provides unparalleled granular control over every character. Unlike standard overlays, we allow for **per-letter customization**, letting you set unique colors and shadows for individual characters within the day, date, or time (e.g., coloring only the 'S' in September). This level of detail, combined with multi-row support and precise offset controls, makes Mond a highly flexible styling tool.

---
The clock shows the weekday, date, and time in three independently styled rows. Drag it to position it; enable position locking to prevent accidental movement.

![Mond Desktop Clock on the desktop](https://i.imgur.com/dj71eKl.png)

![Mond Desktop Clock alternate layout](https://i.imgur.com/ux3bwti.png)

**Key Features & Advanced Customization:**
*   **Three Independent Rows:** Display Weekday, Date, and Time in separate, independently configurable rows.
*   **Seconds:** Optionally show seconds. Digits use fixed-width cells so the time doesn't jitter as it ticks.
*   **Left, Centre or Right Alignment:** Align the whole clock, or override the alignment for each row. Aligned edges stay put when the weekday or month name changes length.
*   **Monitor Selection:** Show the clock on all monitors, only the primary monitor, or one specific monitor.
*   **Independent Positions:** Optionally drag and save the clock separately on each monitor.
*   **Granular Letter Control:** Set individual colors and shadow colours for every single character in the day, date, or time.
*   **Advanced Typography:** Full control over font families (Quicksand, Anurati, etc.), size, bold/italic styling, letter spacing, and per-row horizontal offsets.
*   **Layout Flexibility:** Select a "Main Heading" row to automatically reorder the display (e.g., Date on top, Weekday below).

**Settings layout:**
Settings are grouped in this order: Layout, Placement, Weekday, Date, Time, then per-letter colours. Each row's settings (font, size, style, spacing, alignment, colours, shadow) sit together.

**Upgrading from 3.x:** The shadow offset for each row moved next to that row's shadow colour, and the per-letter weekday colours moved into the per-letter group alongside the weekday shadows. Both need to be set again after updating: shadow offsets reset to 2 DIPs, and weekday letter colours reset to the row colour until re-entered in the per-letter group.

**Styling & Fonts:**
Fonts must be installed in Windows. Missing fonts are silently substituted by Windows. We support advanced typography using:
*   [Quicksand](https://fonts.google.com/specimen/Quicksand)
*   [Anurati](https://www.dafont.com/anurati.font)

**Coloring & Formatting:**
*   Separate text and shadow colors are available for the overall row, but individual letter overrides take precedence.
*   Per-letter styles are indexed by 1-based character position from left to right (including spaces). In 12-hour time the hour isn't zero-padded, so time-row letter numbers shift by one between 12:xx and 1:xx; use 24-hour time if you colour individual time characters.
*   The date displays without a comma.
*   Colours accept `#RRGGBB` or `#AARRGGBB`; the alpha component in the latter form is ignored.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- mainHeading: day
  $name: "Layout: Main heading (top row)"
  $options:
    - day: Weekday
    - date: Date
    - time: Time

- alignment: center
  $name: "Layout: Text alignment"
  $description: Aligns all rows to a shared left edge, centre line, or right edge. Each row can override this in its own section.
  $options:
    - left: Left
    - center: Centre
    - right: Right

- showWeekday: true
  $name: "Layout: Show weekday"

- showDate: true
  $name: "Layout: Show date"

- showTime: true
  $name: "Layout: Show time"

- timeFormat24: false
  $name: "Layout: Use 24-hour time"

- showSeconds: false
  $name: "Layout: Show seconds"

- gapDayToDate: 42
  $name: "Layout: Gap below main heading"

- gapDateToTime: 25
  $name: "Layout: Gap between other rows"

- monitorMode: all
  $name: "Placement: Show on"
  $options:
    - all: All monitors
    - primary: Primary monitor only
    - number: A specific monitor

- monitorNumber: 1
  $name: "Placement: Monitor number"
  $description: Used with "A specific monitor". This is Windows' internal display number (\\.\DISPLAYn). It usually matches Settings > System > Display > Identify, but can differ with multiple graphics adapters or after a driver reinstall. Falls back to the primary monitor if that number isn't connected.

- syncPositions: true
  $name: "Placement: Same position on every monitor"
  $description: When off, the clock on each monitor is dragged and saved independently.

- lockWidgetPosition: false
  $name: "Placement: Lock position"

- offsetX: 0
  $name: "Placement: Horizontal offset"
  $description: Distance from the monitor centre in DIPs. Changing this value resets any dragged positions.

- offsetY: 0
  $name: "Placement: Vertical offset"
  $description: Distance from the monitor centre in DIPs. Changing this value resets any dragged positions.

- dayFontFamily: Anurati
  $name: "Weekday: Font"
  $options:
    - Anurati: Anurati
    - Quicksand: Quicksand
    - Segoe UI: Segoe UI
    - Arial: Arial
    - Calibri: Calibri
    - Cambria: Cambria
    - Consolas: Consolas
    - Georgia: Georgia
    - Tahoma: Tahoma
    - Times New Roman: Times New Roman
    - Trebuchet MS: Trebuchet MS
    - Verdana: Verdana
    - custom: Custom...

- dayCustomFontFamily: ""
  $name: "Weekday: Custom font name"
  $description: Used when the font is set to Custom. Enter the exact name of any installed font.
  #! $format: fontFamily

- dayFontSize: 72
  $name: "Weekday: Size"

- dayBold: false
  $name: "Weekday: Bold"

- dayItalic: false
  $name: "Weekday: Italic"

- dayLetterSpacing: 40
  $name: "Weekday: Letter spacing"

- dayAlignment: default
  $name: "Weekday: Alignment"
  $options:
    - default: Same as layout
    - left: Left
    - center: Centre
    - right: Right

- dayTextColor: "#FFFFFF"
  $name: "Weekday: Text colour"
  $description: Leave blank to use white.
  #! $format: colorRgb

- dayShadowColor: "#404040"
  $name: "Weekday: Shadow colour"
  #! $format: colorRgb

- dayShadowOffset: "2"
  $name: "Weekday: Shadow offset"
  $options:
    - "0": "Off"
    - "1": 1 DIP
    - "2": 2 DIPs
    - "3": 3 DIPs
    - "4": 4 DIPs
    - "5": 5 DIPs
    - "8": 8 DIPs
    - "10": 10 DIPs
    - "16": 16 DIPs
    - "20": 20 DIPs

- dateFontFamily: Quicksand
  $name: "Date: Font"
  $description: When Anurati is selected, digits and punctuation are rendered in Quicksand for readability.
  $options:
    - Anurati: Anurati
    - Quicksand: Quicksand
    - Segoe UI: Segoe UI
    - Arial: Arial
    - Calibri: Calibri
    - Cambria: Cambria
    - Consolas: Consolas
    - Georgia: Georgia
    - Tahoma: Tahoma
    - Times New Roman: Times New Roman
    - Trebuchet MS: Trebuchet MS
    - Verdana: Verdana
    - custom: Custom...

- dateCustomFontFamily: ""
  $name: "Date: Custom font name"
  $description: Used when the font is set to Custom. Enter the exact name of any installed font.
  #! $format: fontFamily

- dateFontSize: 18
  $name: "Date: Size"

- dateBold: true
  $name: "Date: Bold"

- dateItalic: false
  $name: "Date: Italic"

- dateLetterSpacing: 4
  $name: "Date: Letter spacing"

- dateAlignment: default
  $name: "Date: Alignment"
  $options:
    - default: Same as layout
    - left: Left
    - center: Centre
    - right: Right

- dateLeftOffset: 0
  $name: "Date: Horizontal nudge"
  $description: Positive values shift the date left, negative values shift it right.

- dateTextColor: "#FFFFFF"
  $name: "Date: Text colour"
  $description: Leave blank to use white.
  #! $format: colorRgb

- dateShadowColor: "#404040"
  $name: "Date: Shadow colour"
  #! $format: colorRgb

- dateShadowOffset: "2"
  $name: "Date: Shadow offset"
  $options:
    - "0": "Off"
    - "1": 1 DIP
    - "2": 2 DIPs
    - "3": 3 DIPs
    - "4": 4 DIPs
    - "5": 5 DIPs
    - "8": 8 DIPs
    - "10": 10 DIPs
    - "16": 16 DIPs
    - "20": 20 DIPs

- timeFontFamily: Quicksand
  $name: "Time: Font"
  $description: When Anurati is selected, digits and punctuation are rendered in Quicksand for readability.
  $options:
    - Anurati: Anurati
    - Quicksand: Quicksand
    - Segoe UI: Segoe UI
    - Arial: Arial
    - Calibri: Calibri
    - Cambria: Cambria
    - Consolas: Consolas
    - Georgia: Georgia
    - Tahoma: Tahoma
    - Times New Roman: Times New Roman
    - Trebuchet MS: Trebuchet MS
    - Verdana: Verdana
    - custom: Custom...

- timeCustomFontFamily: ""
  $name: "Time: Custom font name"
  $description: Used when the font is set to Custom. Enter the exact name of any installed font.
  #! $format: fontFamily

- timeFontSize: 18
  $name: "Time: Size"

- timeBold: true
  $name: "Time: Bold"

- timeItalic: false
  $name: "Time: Italic"

- timeLetterSpacing: 4
  $name: "Time: Letter spacing"

- timeAlignment: default
  $name: "Time: Alignment"
  $options:
    - default: Same as layout
    - left: Left
    - center: Centre
    - right: Right

- timeLeftOffset: 0
  $name: "Time: Horizontal nudge"
  $description: Positive values shift the time left, negative values shift it right.

- timeTextColor: "#FFFFFF"
  $name: "Time: Text colour"
  $description: Leave blank to use white.
  #! $format: colorRgb

- timeShadowColor: "#404040"
  $name: "Time: Shadow colour"
  #! $format: colorRgb

- timeShadowOffset: "2"
  $name: "Time: Shadow offset"
  $options:
    - "0": "Off"
    - "1": 1 DIP
    - "2": 2 DIPs
    - "3": 3 DIPs
    - "4": 4 DIPs
    - "5": 5 DIPs
    - "8": 8 DIPs
    - "10": 10 DIPs
    - "16": 16 DIPs
    - "20": 20 DIPs

- customLetterStyles:
    - weekday:
        - color1: ""
          $name: Weekday letter 1 colour
        - shadow1: ""
          $name: Weekday letter 1 shadow
        - color2: ""
          $name: Weekday letter 2 colour
        - shadow2: ""
          $name: Weekday letter 2 shadow
        - color3: ""
          $name: Weekday letter 3 colour
        - shadow3: ""
          $name: Weekday letter 3 shadow
        - color4: ""
          $name: Weekday letter 4 colour
        - shadow4: ""
          $name: Weekday letter 4 shadow
        - color5: ""
          $name: Weekday letter 5 colour
        - shadow5: ""
          $name: Weekday letter 5 shadow
        - color6: ""
          $name: Weekday letter 6 colour
        - shadow6: ""
          $name: Weekday letter 6 shadow
        - color7: ""
          $name: Weekday letter 7 colour
        - shadow7: ""
          $name: Weekday letter 7 shadow
        - color8: ""
          $name: Weekday letter 8 colour
        - shadow8: ""
          $name: Weekday letter 8 shadow
        - color9: ""
          $name: Weekday letter 9 colour
        - shadow9: ""
          $name: Weekday letter 9 shadow
      $name: Weekday letters
    - date:
        - color1: ""
          $name: Date letter 1 colour
        - shadow1: ""
          $name: Date letter 1 shadow
        - color2: ""
          $name: Date letter 2 colour
        - shadow2: ""
          $name: Date letter 2 shadow
        - color3: ""
          $name: Date letter 3 colour
        - shadow3: ""
          $name: Date letter 3 shadow
        - color4: ""
          $name: Date letter 4 colour
        - shadow4: ""
          $name: Date letter 4 shadow
        - color5: ""
          $name: Date letter 5 colour
        - shadow5: ""
          $name: Date letter 5 shadow
        - color6: ""
          $name: Date letter 6 colour
        - shadow6: ""
          $name: Date letter 6 shadow
        - color7: ""
          $name: Date letter 7 colour
        - shadow7: ""
          $name: Date letter 7 shadow
        - color8: ""
          $name: Date letter 8 colour
        - shadow8: ""
          $name: Date letter 8 shadow
        - color9: ""
          $name: Date letter 9 colour
        - shadow9: ""
          $name: Date letter 9 shadow
        - color10: ""
          $name: Date letter 10 colour
        - shadow10: ""
          $name: Date letter 10 shadow
        - color11: ""
          $name: Date letter 11 colour
        - shadow11: ""
          $name: Date letter 11 shadow
        - color12: ""
          $name: Date letter 12 colour
        - shadow12: ""
          $name: Date letter 12 shadow
        - color13: ""
          $name: Date letter 13 colour
        - shadow13: ""
          $name: Date letter 13 shadow
        - color14: ""
          $name: Date letter 14 colour
        - shadow14: ""
          $name: Date letter 14 shadow
        - color15: ""
          $name: Date letter 15 colour
        - shadow15: ""
          $name: Date letter 15 shadow
        - color16: ""
          $name: Date letter 16 colour
        - shadow16: ""
          $name: Date letter 16 shadow
        - color17: ""
          $name: Date letter 17 colour
        - shadow17: ""
          $name: Date letter 17 shadow
      $name: Date letters
    - time:
        - color1: ""
          $name: Time letter 1 colour
        - shadow1: ""
          $name: Time letter 1 shadow
        - color2: ""
          $name: Time letter 2 colour
        - shadow2: ""
          $name: Time letter 2 shadow
        - color3: ""
          $name: Time letter 3 colour
        - shadow3: ""
          $name: Time letter 3 shadow
        - color4: ""
          $name: Time letter 4 colour
        - shadow4: ""
          $name: Time letter 4 shadow
        - color5: ""
          $name: Time letter 5 colour
        - shadow5: ""
          $name: Time letter 5 shadow
        - color6: ""
          $name: Time letter 6 colour
        - shadow6: ""
          $name: Time letter 6 shadow
        - color7: ""
          $name: Time letter 7 colour
        - shadow7: ""
          $name: Time letter 7 shadow
        - color8: ""
          $name: Time letter 8 colour
        - shadow8: ""
          $name: Time letter 8 shadow
        - color9: ""
          $name: Time letter 9 colour
        - shadow9: ""
          $name: Time letter 9 shadow
        - color10: ""
          $name: Time letter 10 colour
        - shadow10: ""
          $name: Time letter 10 shadow
        - color11: ""
          $name: Time letter 11 colour
        - shadow11: ""
          $name: Time letter 11 shadow
      $name: Time letters
  $name: "Per-letter colours and shadows"
  $description: Letters are numbered from the left starting at 1, counting spaces. Blank entries use the row's colours. In 12-hour time the hour isn't zero-padded, so time letters shift by one between 12:00 and 1:00 (letter 3 is ":" in 1:05 but "0" in 12:05).
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <shellapi.h>
#include <windhawk_api.h>
#include <windhawk_utils.h>
#include <string>
#include <vector>
#include <algorithm>
#include <cwchar>
#include <cwctype>
#include <cstdlib>
#include <atomic>
#include <iterator>

static constexpr wchar_t kWindowClass[] = L"WindhawkMondDesktopClock";
static constexpr wchar_t kControllerClass[] = L"WindhawkMondDesktopClockController";
static constexpr UINT kShutdownMessage = WM_APP + 1;
static constexpr UINT kSettingsMessage = WM_APP + 2;
static constexpr UINT_PTR kWatchTimerId = 1;
static constexpr UINT_PTR kRelayoutTimerId = 3;
static constexpr UINT_PTR kClockTimerId = 4;
static constexpr UINT kWatchIntervalMs = 2000;
static constexpr size_t kWeekdayCharacterCount = 9;
static constexpr size_t kDateCharacterCount = 17;
static constexpr size_t kTimeCharacterCount = 11;  // "12:59:59 PM"
static constexpr size_t kMaxHeaderCharacters = kDateCharacterCount;
static constexpr int kEdgePaddingDip = 24;
using GetDpiForMonitor_t = HRESULT(WINAPI*)(HMONITOR, int, UINT*, UINT*);

enum class Row { Day, Date, Time };
enum class Align { Left, Center, Right };
enum class MonitorMode { All, Primary, Number };

struct Overlay {
  HWND hwnd = nullptr;
  UINT dpi = 96;
  HFONT dayFont = nullptr;
  HFONT timeFont = nullptr;
  HFONT dateFont = nullptr;
  HFONT timeFallbackFont = nullptr;
  HFONT dateFallbackFont = nullptr;
  // Monitor this overlay was created for, and its own position (DIPs from
  // the monitor centre to the text anchor).
  std::wstring monitorKey;
  RECT monitorRect{};
  int offsetX = 0;
  int offsetY = 0;
  // Horizontal padding between the window edge and the aligned text edge.
  int edgePad = 0;
};

static std::vector<Overlay*> g_overlays;
static HINSTANCE g_hinstance = nullptr;
static std::atomic<HWND> g_controller{nullptr};
static HWND g_desktopHost = nullptr;
static UINT g_taskbarCreatedMessage = 0;
static HANDLE g_uiThread = nullptr;
static DWORD g_uiThreadId = 0;
static HANDLE g_readyEvent = nullptr;
static bool g_uiStartOk = false;
static bool g_rebuilding = false;
static bool g_inMoveLoop = false;
static bool g_syncingPositions = false;
static bool g_settingsUpdatePending = false;
static bool g_rebuildPending = false;
static long long g_currentStamp = -1;
static int g_positionSettingX = 0;
static int g_positionSettingY = 0;
static int g_positionGeneration = 0;
static HMODULE g_shcore = nullptr;
static GetDpiForMonitor_t g_getDpiForMonitor = nullptr;

struct Settings {
  // Layout
  std::wstring mainHeading;
  Align alignment;
  bool showWeekday;
  bool showDate;
  bool showTime;
  bool timeFormat24;
  bool showSeconds;
  int gapDayToDate;
  int gapDateToTime;
  // Placement
  MonitorMode monitorMode;
  int monitorNumber;
  bool syncPositions;
  bool lockWidgetPosition;
  int offsetX;
  int offsetY;
  // Weekday
  std::wstring dayFontFamily;
  int dayFontSize;
  bool dayBold;
  bool dayItalic;
  int dayLetterSpacing;
  Align dayAlignment;
  COLORREF dayTextColor;
  COLORREF dayShadowColor;
  int dayShadowOffset;
  // Date
  std::wstring dateFontFamily;
  int dateFontSize;
  bool dateBold;
  bool dateItalic;
  int dateLetterSpacing;
  Align dateAlignment;
  int dateLeftOffset;
  COLORREF dateTextColor;
  COLORREF dateShadowColor;
  int dateShadowOffset;
  // Time
  std::wstring timeFontFamily;
  int timeFontSize;
  bool timeBold;
  bool timeItalic;
  int timeLetterSpacing;
  Align timeAlignment;
  int timeLeftOffset;
  COLORREF timeTextColor;
  COLORREF timeShadowColor;
  int timeShadowOffset;
  // Per-letter
  COLORREF weekdayLetterColors[kMaxHeaderCharacters];
  COLORREF weekdayLetterShadowColors[kMaxHeaderCharacters];
  COLORREF dateLetterColors[kMaxHeaderCharacters];
  COLORREF dateLetterShadowColors[kMaxHeaderCharacters];
  COLORREF timeLetterColors[kMaxHeaderCharacters];
  COLORREF timeLetterShadowColors[kMaxHeaderCharacters];
};
static Settings g_settings;

static COLORREF ParseColor(const wchar_t* s) {
  if (*s == L'#') ++s;
  const size_t n = wcslen(s);
  const unsigned long v = wcstoul(s, nullptr, 16);
  if (n == 8 || n == 6) {
    return RGB((v >> 16) & 255, (v >> 8) & 255, v & 255);
  }
  return RGB(255, 255, 255);
}

static COLORREF LoadColorSetting(const wchar_t* name, COLORREF fallback) {
  auto value = WindhawkUtils::StringSetting::make(name);
  return *value.get() ? ParseColor(value.get()) : fallback;
}

static int LoadShadowOffset(const wchar_t* name, int fallback) {
  auto value = WindhawkUtils::StringSetting::make(name);
  return *value.get() ? std::clamp(_wtoi(value.get()), 0, 20) : fallback;
}

static std::wstring LoadStringSetting(const wchar_t* name,
                                      const wchar_t* fallback) {
  auto value = WindhawkUtils::StringSetting::make(name);
  return *value.get() ? value.get() : fallback;
}

static Align ParseAlign(const std::wstring& value, Align fallback) {
  if (value == L"left") return Align::Left;
  if (value == L"right") return Align::Right;
  if (value == L"center") return Align::Center;
  return fallback;
}

static std::wstring LoadFontFamily(const wchar_t* name,
                                   const wchar_t* customName,
                                   const wchar_t* fallback) {
  std::wstring family = LoadStringSetting(name, fallback);
  if (family == L"custom")
    family = LoadStringSetting(customName, L"Segoe UI");
  return family;
}

static void DeleteFonts(Overlay* overlay) {
  if (overlay->dayFont) DeleteObject(overlay->dayFont);
  if (overlay->timeFont) DeleteObject(overlay->timeFont);
  if (overlay->dateFont) DeleteObject(overlay->dateFont);
  if (overlay->timeFallbackFont) DeleteObject(overlay->timeFallbackFont);
  if (overlay->dateFallbackFont) DeleteObject(overlay->dateFallbackFont);
  overlay->dayFont = overlay->timeFont = overlay->dateFont = nullptr;
  overlay->timeFallbackFont = overlay->dateFallbackFont = nullptr;
}

static HFONT MakeFont(int size, bool bold, bool italic, const wchar_t* family) {
  return CreateFontW(-size, 0, 0, 0, bold ? FW_BOLD : FW_NORMAL, italic,
      FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
      ANTIALIASED_QUALITY, DEFAULT_PITCH, family);
}

static void CreateFonts(Overlay* overlay) {
  DeleteFonts(overlay);
  const int daySize = MulDiv(g_settings.dayFontSize, overlay->dpi, 96);
  const int timeSize = MulDiv(g_settings.timeFontSize, overlay->dpi, 96);
  const int dateSize = MulDiv(g_settings.dateFontSize, overlay->dpi, 96);
  overlay->dayFont = MakeFont(daySize, g_settings.dayBold,
      g_settings.dayItalic, g_settings.dayFontFamily.c_str());
  overlay->timeFont = MakeFont(timeSize, g_settings.timeBold,
      g_settings.timeItalic, g_settings.timeFontFamily.c_str());
  overlay->dateFont = MakeFont(dateSize, g_settings.dateBold,
      g_settings.dateItalic, g_settings.dateFontFamily.c_str());
  overlay->timeFallbackFont = MakeFont(timeSize, g_settings.timeBold,
      g_settings.timeItalic, L"Quicksand");
  overlay->dateFallbackFont = MakeFont(dateSize, g_settings.dateBold,
      g_settings.dateItalic, L"Quicksand");
}

static BOOL CALLBACK FindDesktopHostProc(HWND hwnd, LPARAM lParam) {
  if (FindWindowExW(hwnd, nullptr, L"SHELLDLL_DefView", nullptr)) {
    *reinterpret_cast<HWND*>(lParam) = hwnd;
    return FALSE;
  }
  return TRUE;
}

static HWND FindDesktopHost() {
  HWND progman = FindWindowW(L"Progman", nullptr);
  if (progman && FindWindowExW(progman, nullptr, L"SHELLDLL_DefView", nullptr)) {
    return progman;
  }
  HWND host = nullptr;
  EnumWindows(FindDesktopHostProc, reinterpret_cast<LPARAM>(&host));
  if (host) return host;
  return progman ? progman : GetShellWindow();
}

static HWND DesktopInsertAfter(HWND hwnd) {
  if (!g_desktopHost || !IsWindow(g_desktopHost)) return nullptr;
  DWORD ownPid = GetCurrentProcessId();
  for (HWND window = GetWindow(g_desktopHost, GW_HWNDPREV); window;
     window = GetWindow(window, GW_HWNDPREV)) {
    DWORD processId = 0;
    GetWindowThreadProcessId(window, &processId);
    if (processId == ownPid || !IsWindowVisible(window)) continue;
    return window;
  }
  return HWND_TOP;
}

static UINT GetMonitorDpi(HMONITOR monitor, HDC dc) {
  if (g_getDpiForMonitor) {
    UINT dpiX = 0, dpiY = 0;
    if (SUCCEEDED(g_getDpiForMonitor(monitor, 0, &dpiX, &dpiY)) && dpiX)
      return dpiX;
  }
  int dpi = dc ? GetDeviceCaps(dc, LOGPIXELSX) : 96;
  return dpi > 0 ? static_cast<UINT>(dpi) : 96;
}

static int Scale(int value, UINT dpi) {
  return MulDiv(value, static_cast<int>(dpi), 96);
}

static int WeekdayKerning(UINT dpi) {
  return g_settings.dayFontFamily == L"Anurati" ? Scale(14, dpi) : 0;
}

// ---------------------------------------------------------------------------
// Text

static const wchar_t* const kWeekdays[] = {
    L"SUNDAY", L"MONDAY", L"TUESDAY", L"WEDNESDAY",
    L"THURSDAY", L"FRIDAY", L"SATURDAY"};
static const wchar_t* const kMonths[] = {
    L"January", L"February", L"March", L"April", L"May", L"June",
    L"July", L"August", L"September", L"October", L"November", L"December"};

static std::wstring FormatWeekday(const SYSTEMTIME& st) {
  return kWeekdays[st.wDayOfWeek % 7];
}

static std::wstring FormatTime(const SYSTEMTIME& st) {
  wchar_t b[64]{};
  if (g_settings.timeFormat24) {
    if (g_settings.showSeconds)
      swprintf_s(b, L"%02u:%02u:%02u", st.wHour, st.wMinute, st.wSecond);
    else
      swprintf_s(b, L"%02u:%02u", st.wHour, st.wMinute);
  } else {
    unsigned h = st.wHour % 12;
    if (!h) h = 12;
    const wchar_t* suffix = st.wHour >= 12 ? L"PM" : L"AM";
    if (g_settings.showSeconds)
      swprintf_s(b, L"%u:%02u:%02u %s", h, st.wMinute, st.wSecond, suffix);
    else
      swprintf_s(b, L"%u:%02u %s", h, st.wMinute, suffix);
  }
  return b;
}

static std::wstring FormatDate(const SYSTEMTIME& st) {
  wchar_t b[64]{};
  const unsigned month = std::clamp<unsigned>(st.wMonth, 1, 12);
  swprintf_s(b, L"%02u %s %04u", st.wDay, kMonths[month - 1], st.wYear);
  return b;
}

// ---------------------------------------------------------------------------
// Row layout. Measuring and drawing share one layout routine so the measured
// width always matches what is painted.

struct RowStyle {
  HFONT font = nullptr;
  HFONT fallback = nullptr;
  int spacing = 0;
  int kerning = 0;
  bool tabularDigits = false;
};

struct RowLayout {
  std::vector<int> x;      // left edge of each character's cell
  std::vector<int> cell;   // cell width
  std::vector<int> glyph;  // actual glyph width
  int width = 0;
};

static RowStyle StyleForRow(const Overlay* overlay, Row row) {
  RowStyle style;
  switch (row) {
    case Row::Day:
      style.font = overlay->dayFont;
      style.spacing = Scale(g_settings.dayLetterSpacing, overlay->dpi);
      style.kerning = WeekdayKerning(overlay->dpi);
      break;
    case Row::Date:
      style.font = overlay->dateFont;
      style.fallback = g_settings.dateFontFamily == L"Anurati"
          ? overlay->dateFallbackFont : nullptr;
      style.spacing = Scale(g_settings.dateLetterSpacing, overlay->dpi);
      style.tabularDigits = true;
      break;
    case Row::Time:
      style.font = overlay->timeFont;
      style.fallback = g_settings.timeFontFamily == L"Anurati"
          ? overlay->timeFallbackFont : nullptr;
      style.spacing = Scale(g_settings.timeLetterSpacing, overlay->dpi);
      style.tabularDigits = true;
      break;
  }
  return style;
}

static HFONT FontForCharacter(const RowStyle& style, wchar_t character) {
  return style.fallback && !iswalpha(character) ? style.fallback : style.font;
}

static int DigitCellWidth(HDC dc, const RowStyle& style) {
  HGDIOBJ oldFont = SelectObject(dc, FontForCharacter(style, L'0'));
  int width = 0;
  for (wchar_t digit = L'0'; digit <= L'9'; ++digit) {
    SIZE size{};
    GetTextExtentPoint32W(dc, &digit, 1, &size);
    width = std::max(width, static_cast<int>(size.cx));
  }
  SelectObject(dc, oldFont);
  return width;
}

static RowLayout LayoutRow(HDC dc, const std::wstring& text,
                           const RowStyle& style) {
  RowLayout layout;
  const size_t count = text.size();
  layout.x.resize(count);
  layout.cell.resize(count);
  layout.glyph.resize(count);
  const int digitCell = style.tabularDigits ? DigitCellWidth(dc, style) : 0;
  HGDIOBJ oldFont = SelectObject(dc, style.font);
  int x = 0;
  for (size_t i = 0; i < count; ++i) {
    if (i > 0) {
      x += style.spacing;
      if (text[i] == L'A' && text[i - 1] == L'D') x -= style.kerning;
    }
    SelectObject(dc, FontForCharacter(style, text[i]));
    SIZE size{};
    GetTextExtentPoint32W(dc, &text[i], 1, &size);
    layout.glyph[i] = size.cx;
    layout.cell[i] = style.tabularDigits && iswdigit(text[i])
        ? std::max(digitCell, static_cast<int>(size.cx))
        : static_cast<int>(size.cx);
    layout.x[i] = x;
    x += layout.cell[i];
  }
  SelectObject(dc, oldFont);
  layout.width = x;
  return layout;
}

static void DrawRow(HDC dc, const std::wstring& text, const RowLayout& layout,
                    const RowStyle& style, int startX, int y,
                    const COLORREF* colors, COLORREF defaultColor) {
  HGDIOBJ oldFont = SelectObject(dc, style.font);
  SetBkMode(dc, TRANSPARENT);
  for (size_t i = 0; i < text.size(); ++i) {
    SelectObject(dc, FontForCharacter(style, text[i]));
    SetTextColor(dc, i < kMaxHeaderCharacters ? colors[i] : defaultColor);
    const int x = startX + layout.x[i] + (layout.cell[i] - layout.glyph[i]) / 2;
    TextOutW(dc, x, y, &text[i], 1);
  }
  SelectObject(dc, oldFont);
}

// x coordinate (window client space) of the edge or centre a row is aligned to.
static int AnchorX(const Overlay* overlay, int windowWidth, Align align) {
  switch (align) {
    case Align::Left: return overlay->edgePad;
    case Align::Right: return windowWidth - overlay->edgePad;
    default: return windowWidth / 2;
  }
}

// Point of the window that the position offset refers to. Using the aligned
// edge keeps that edge fixed on screen when the window size changes.
static int ReferenceX(const Overlay* overlay, int windowWidth) {
  return AnchorX(overlay, windowWidth, g_settings.alignment);
}

static int StartXForRow(int anchorX, int width, Align align) {
  switch (align) {
    case Align::Left: return anchorX;
    case Align::Right: return anchorX - width;
    default: return anchorX - width / 2;
  }
}

static std::vector<Row> VisibleRows() {
  std::vector<Row> rows;
  if (g_settings.mainHeading == L"date") {
    rows = {Row::Date, Row::Day, Row::Time};
  } else if (g_settings.mainHeading == L"time") {
    rows = {Row::Time, Row::Day, Row::Date};
  } else {
    rows = {Row::Day, Row::Date, Row::Time};
  }
  std::vector<Row> visible;
  for (Row row : rows) {
    if ((row == Row::Day && g_settings.showWeekday) ||
        (row == Row::Date && g_settings.showDate) ||
        (row == Row::Time && g_settings.showTime)) {
      visible.push_back(row);
    }
  }
  return visible;
}

// ---------------------------------------------------------------------------
// Painting

static void Paint(Overlay* overlay) {
  HWND hwnd = overlay->hwnd;
  RECT client{};
  if (!GetClientRect(hwnd, &client)) return;
  const int width = client.right, height = client.bottom;
  if (width <= 0 || height <= 0) return;
  HDC screen = GetDC(hwnd);
  if (!screen) return;
  HDC memory = CreateCompatibleDC(screen);
  if (!memory) {
    ReleaseDC(hwnd, screen);
    return;
  }
  HBITMAP bitmap = CreateCompatibleBitmap(screen, width, height);
  if (!bitmap) {
    DeleteDC(memory);
    ReleaseDC(hwnd, screen);
    return;
  }
  HGDIOBJ oldBitmap = SelectObject(memory, bitmap);
  const COLORREF key = RGB(1, 2, 3);
  HBRUSH background = CreateSolidBrush(key);
  FillRect(memory, &client, background);
  DeleteObject(background);

  SYSTEMTIME now{};
  GetLocalTime(&now);
  const std::wstring day = FormatWeekday(now);
  const std::wstring date = FormatDate(now);
  const std::wstring time = FormatTime(now);
  int y = Scale(10, overlay->dpi);

  const std::vector<Row> visibleRows = VisibleRows();
  for (size_t index = 0; index < visibleRows.size(); ++index) {
    const Row row = visibleRows[index];
    const std::wstring* text = &day;
    int fontSize = g_settings.dayFontSize;
    int nudge = 0;
    int shadowOffsetDip = g_settings.dayShadowOffset;
    Align align = g_settings.dayAlignment;
    COLORREF textColor = g_settings.dayTextColor;
    COLORREF shadowColor = g_settings.dayShadowColor;
    const COLORREF* letterColors = g_settings.weekdayLetterColors;
    const COLORREF* letterShadowColors = g_settings.weekdayLetterShadowColors;

    if (row == Row::Date) {
      text = &date;
      fontSize = g_settings.dateFontSize;
      nudge = g_settings.dateLeftOffset;
      shadowOffsetDip = g_settings.dateShadowOffset;
      align = g_settings.dateAlignment;
      textColor = g_settings.dateTextColor;
      shadowColor = g_settings.dateShadowColor;
      letterColors = g_settings.dateLetterColors;
      letterShadowColors = g_settings.dateLetterShadowColors;
    } else if (row == Row::Time) {
      text = &time;
      fontSize = g_settings.timeFontSize;
      nudge = g_settings.timeLeftOffset;
      shadowOffsetDip = g_settings.timeShadowOffset;
      align = g_settings.timeAlignment;
      textColor = g_settings.timeTextColor;
      shadowColor = g_settings.timeShadowColor;
      letterColors = g_settings.timeLetterColors;
      letterShadowColors = g_settings.timeLetterShadowColors;
    }

    const RowStyle style = StyleForRow(overlay, row);
    const RowLayout layout = LayoutRow(memory, *text, style);
    const int anchorX = AnchorX(overlay, width, align) -
        Scale(nudge, overlay->dpi);
    const int startX = StartXForRow(anchorX, layout.width, align);
    const int shadowOffset = Scale(shadowOffsetDip, overlay->dpi);
    if (shadowOffset > 0) {
      DrawRow(memory, *text, layout, style, startX + shadowOffset,
          y + shadowOffset, letterShadowColors, shadowColor);
    }
    DrawRow(memory, *text, layout, style, startX, y, letterColors, textColor);

    y += Scale(fontSize, overlay->dpi);
    if (index + 1 < visibleRows.size()) {
      y += Scale(index == 0 ? g_settings.gapDayToDate
                            : g_settings.gapDateToTime,
          overlay->dpi);
    }
  }
  POINT source{0, 0};
  SIZE size{width, height};
  POINT position{};
  ClientToScreen(hwnd, &position);
  BLENDFUNCTION blend{AC_SRC_OVER, 0, 255, 0};
  UpdateLayeredWindow(hwnd, nullptr, &position, &size, memory, &source,
            key, &blend, ULW_COLORKEY);
  SelectObject(memory, oldBitmap);
  DeleteObject(bitmap);
  DeleteDC(memory);
  ReleaseDC(hwnd, screen);
}

static long long ClockStamp(const SYSTEMTIME& st) {
  long long stamp = (static_cast<long long>(st.wYear) * 400 +
      st.wMonth * 32 + st.wDay) * 86400LL + st.wHour * 3600 + st.wMinute * 60;
  if (g_settings.showSeconds) stamp += st.wSecond;
  return stamp;
}

static void UpdateClock(bool force = false) {
  SYSTEMTIME now{};
  GetLocalTime(&now);
  const long long stamp = ClockStamp(now);
  if (!force && stamp == g_currentStamp) return;
  g_currentStamp = stamp;
  for (Overlay* overlay : g_overlays) Paint(overlay);
}

// Arms a one-shot timer for just after the next second (or minute) boundary,
// so the display flips over in step with the system clock.
static void ScheduleClockTick() {
  HWND controller = g_controller.load();
  if (!controller) return;
  SYSTEMTIME now{};
  GetLocalTime(&now);
  UINT delay = 1000 - std::min<UINT>(now.wMilliseconds, 999);
  if (!g_settings.showSeconds) delay += (59 - std::min<UINT>(now.wSecond, 59)) * 1000;
  SetTimer(controller, kClockTimerId, delay + 15, nullptr);
}

// ---------------------------------------------------------------------------
// Positions

static std::wstring PerMonitorValueName(const wchar_t* prefix,
                                        const std::wstring& monitorKey) {
  return std::wstring(prefix) + L"_" + monitorKey;
}

static void SavePerMonitorPosition(const Overlay* overlay) {
  Wh_SetIntValue(PerMonitorValueName(L"offsetX", overlay->monitorKey).c_str(),
      overlay->offsetX);
  Wh_SetIntValue(PerMonitorValueName(L"offsetY", overlay->monitorKey).c_str(),
      overlay->offsetY);
  Wh_SetIntValue(
      PerMonitorValueName(L"positionGeneration", overlay->monitorKey).c_str(),
      g_positionGeneration);
}

static void LoadPositionFor(Overlay* overlay) {
  overlay->offsetX = g_settings.offsetX;
  overlay->offsetY = g_settings.offsetY;
  if (g_settings.syncPositions) return;
  const int generation = Wh_GetIntValue(
      PerMonitorValueName(L"positionGeneration", overlay->monitorKey).c_str(), -1);
  if (generation != g_positionGeneration) return;
  overlay->offsetX = Wh_GetIntValue(
      PerMonitorValueName(L"offsetX", overlay->monitorKey).c_str(),
      g_settings.offsetX);
  overlay->offsetY = Wh_GetIntValue(
      PerMonitorValueName(L"offsetY", overlay->monitorKey).c_str(),
      g_settings.offsetY);
}

static POINT MonitorCenter(const RECT& rect) {
  return {rect.left + (rect.right - rect.left) / 2,
          rect.top + (rect.bottom - rect.top) / 2};
}

static void DestroyOverlays() {
  for (Overlay* overlay : g_overlays) {
    if (overlay->hwnd) DestroyWindow(overlay->hwnd);
    DeleteFonts(overlay);
    delete overlay;
  }
  g_overlays.clear();
}

static void OnOverlayMoved(Overlay* overlay) {
  HWND hwnd = overlay->hwnd;
  RECT windowRect{};
  if (!GetWindowRect(hwnd, &windowRect)) return;
  const int windowWidth = windowRect.right - windowRect.left;
  const int refX = windowRect.left + ReferenceX(overlay, windowWidth);
  const int refY = (windowRect.top + windowRect.bottom) / 2;

  if (!g_settings.syncPositions) {
    // Independent: measure against this overlay's own monitor, even if it
    // has been dragged partly onto another one.
    const POINT center = MonitorCenter(overlay->monitorRect);
    const int dpi = static_cast<int>(overlay->dpi);
    overlay->offsetX = MulDiv(refX - center.x, 96, dpi);
    overlay->offsetY = MulDiv(refY - center.y, 96, dpi);
    return;
  }

  MONITORINFO info{sizeof(info)};
  HMONITOR monitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
  if (!GetMonitorInfoW(monitor, &info)) return;
  const POINT center = MonitorCenter(info.rcMonitor);
  const int dpi = static_cast<int>(GetMonitorDpi(monitor, nullptr));
  g_settings.offsetX = MulDiv(refX - center.x, 96, dpi);
  g_settings.offsetY = MulDiv(refY - center.y, 96, dpi);
  overlay->offsetX = g_settings.offsetX;
  overlay->offsetY = g_settings.offsetY;

  g_syncingPositions = true;
  for (Overlay* other : g_overlays) {
    if (other == overlay || !IsWindow(other->hwnd)) continue;
    MONITORINFO otherInfo{sizeof(otherInfo)};
    RECT otherRect{};
    HMONITOR otherMonitor = MonitorFromWindow(
        other->hwnd, MONITOR_DEFAULTTONEAREST);
    if (!GetMonitorInfoW(otherMonitor, &otherInfo) ||
        !GetWindowRect(other->hwnd, &otherRect)) {
      continue;
    }
    const POINT otherCenter = MonitorCenter(otherInfo.rcMonitor);
    const int otherWidth = otherRect.right - otherRect.left;
    const int otherX = otherCenter.x + Scale(g_settings.offsetX, other->dpi) -
        ReferenceX(other, otherWidth);
    const int otherY = otherCenter.y + Scale(g_settings.offsetY, other->dpi) -
        (otherRect.bottom - otherRect.top) / 2;
    other->offsetX = g_settings.offsetX;
    other->offsetY = g_settings.offsetY;
    SetWindowPos(other->hwnd, nullptr, otherX, otherY, 0, 0,
        SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
  }
  g_syncingPositions = false;
}

static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
  Overlay* overlay = reinterpret_cast<Overlay*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
  if (msg == WM_NCCREATE) {
    auto create = reinterpret_cast<CREATESTRUCTW*>(lp);
    overlay = static_cast<Overlay*>(create->lpCreateParams);
    overlay->hwnd = hwnd;
    SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(overlay));
  }
  switch (msg) {
    case WM_NCHITTEST:
      if (DefWindowProcW(hwnd, msg, wp, lp) == HTCLIENT) return HTCAPTION;
      break;
    case WM_WINDOWPOSCHANGING: {
      auto position = reinterpret_cast<WINDOWPOS*>(lp);
      if (position && !(position->flags & SWP_NOZORDER)) {
        if (g_desktopHost && IsWindow(g_desktopHost)) {
          position->hwndInsertAfter = DesktopInsertAfter(hwnd);
        } else {
          position->flags |= SWP_NOZORDER;
        }
      }
      break;
    }
    case WM_WINDOWPOSCHANGED: {
      auto position = reinterpret_cast<WINDOWPOS*>(lp);
      if (overlay && position && !(position->flags & SWP_NOMOVE) &&
          g_inMoveLoop && !g_rebuilding && !g_syncingPositions) {
        OnOverlayMoved(overlay);
      }
      break;
    }
    case WM_EXITSIZEMOVE:
      g_inMoveLoop = false;
      if (overlay) {
        if (g_settings.syncPositions) {
          Wh_SetIntValue(L"offsetX", g_settings.offsetX);
          Wh_SetIntValue(L"offsetY", g_settings.offsetY);
        } else {
          SavePerMonitorPosition(overlay);
        }
        Wh_SetIntValue(L"offsetXSetting", g_positionSettingX);
        Wh_SetIntValue(L"offsetYSetting", g_positionSettingY);
      }
      return 0;
    case WM_ENTERSIZEMOVE:
      g_inMoveLoop = true;
      return 0;
    case WM_DPICHANGED:
      if (HWND controller = g_controller.load())
        SetTimer(controller, kRelayoutTimerId, 300, nullptr);
      return 0;
    case WM_DESTROY:
      if (g_inMoveLoop) g_inMoveLoop = false;
      return 0;
    case WM_NCDESTROY:
      SetWindowLongPtrW(hwnd, GWLP_USERDATA, 0);
      break;
  }
  return DefWindowProcW(hwnd, msg, wp, lp);
}

// ---------------------------------------------------------------------------
// Monitors and overlay creation

struct MonitorEntry {
  HMONITOR handle = nullptr;
  MONITORINFOEXW info{};
};

static BOOL CALLBACK CollectMonitor(HMONITOR monitor, HDC, LPRECT, LPARAM lp) {
  MonitorEntry entry;
  entry.handle = monitor;
  entry.info.cbSize = sizeof(entry.info);
  if (GetMonitorInfoW(monitor, &entry.info))
    reinterpret_cast<std::vector<MonitorEntry>*>(lp)->push_back(entry);
  return TRUE;
}

// Storage key for a monitor's saved position. DISPLAYn names can be
// renumbered on dock/undock or driver changes, so prefer a hash of the
// monitor's device interface path, which stays with the physical display.
static std::wstring MonitorKey(const MonitorEntry& entry) {
  DISPLAY_DEVICEW device{};
  device.cb = sizeof(device);
  if (EnumDisplayDevicesW(entry.info.szDevice, 0, &device,
                          EDD_GET_DEVICE_INTERFACE_NAME) &&
      device.DeviceID[0]) {
    unsigned long long hash = 1469598103934665603ULL;  // FNV-1a
    for (const wchar_t* c = device.DeviceID; *c; ++c) {
      hash ^= static_cast<unsigned long long>(towlower(*c));
      hash *= 1099511628211ULL;
    }
    wchar_t key[32];
    swprintf_s(key, L"mon%016llx", hash);
    return key;
  }
  std::wstring name = entry.info.szDevice;
  const size_t slash = name.find_last_of(L'\\');
  name = slash == std::wstring::npos ? name : name.substr(slash + 1);
  return name.empty() ? L"DISPLAY" : name;
}

// "\\.\DISPLAY2" -> 2. This is Windows' internal display number; it usually
// matches Settings > Display > Identify, but not always.
static int MonitorNumber(const MonitorEntry& entry) {
  const std::wstring device = entry.info.szDevice;
  size_t end = device.size();
  size_t start = end;
  while (start > 0 && iswdigit(device[start - 1])) --start;
  return start < end ? _wtoi(device.c_str() + start) : 0;
}

static std::vector<MonitorEntry> SelectedMonitors() {
  std::vector<MonitorEntry> all;
  EnumDisplayMonitors(nullptr, nullptr, CollectMonitor,
      reinterpret_cast<LPARAM>(&all));
  if (g_settings.monitorMode == MonitorMode::All || all.empty()) return all;

  std::vector<MonitorEntry> selected;
  for (const MonitorEntry& entry : all) {
    if (g_settings.monitorMode == MonitorMode::Number &&
        MonitorNumber(entry) == g_settings.monitorNumber) {
      selected.push_back(entry);
      break;
    }
  }
  if (selected.empty()) {
    for (const MonitorEntry& entry : all) {
      if (entry.info.dwFlags & MONITORINFOF_PRIMARY) {
        selected.push_back(entry);
        break;
      }
    }
  }
  if (selected.empty()) selected.push_back(all.front());
  return selected;
}

static void CreateOverlay(const MonitorEntry& monitor) {
  auto overlay = new Overlay;
  overlay->dpi = GetMonitorDpi(monitor.handle, nullptr);
  overlay->monitorKey = MonitorKey(monitor);
  overlay->monitorRect = monitor.info.rcMonitor;
  CreateFonts(overlay);
  HDC measureDc = CreateCompatibleDC(nullptr);
  if (!measureDc) {
    DeleteFonts(overlay);
    delete overlay;
    return;
  }

  // Widest possible content for each visible row, so the window never needs
  // to resize as the text changes.
  int contentWidth = 0;
  if (g_settings.showWeekday) {
    const RowStyle style = StyleForRow(overlay, Row::Day);
    for (const wchar_t* name : kWeekdays)
      contentWidth = std::max(contentWidth, LayoutRow(measureDc, name, style).width);
  }
  const int dateNudge = std::abs(Scale(g_settings.dateLeftOffset, overlay->dpi));
  const int timeNudge = std::abs(Scale(g_settings.timeLeftOffset, overlay->dpi));
  int nudge = 0;
  if (g_settings.showDate) {
    const RowStyle style = StyleForRow(overlay, Row::Date);
    for (const wchar_t* month : kMonths) {
      const std::wstring sample = std::wstring(L"00 ") + month + L" 0000";
      contentWidth = std::max(contentWidth,
          LayoutRow(measureDc, sample, style).width);
    }
    nudge = std::max(nudge, dateNudge);
  }
  if (g_settings.showTime) {
    const RowStyle style = StyleForRow(overlay, Row::Time);
    std::vector<std::wstring> samples;
    if (g_settings.timeFormat24) {
      samples = {g_settings.showSeconds ? L"00:00:00" : L"00:00"};
    } else if (g_settings.showSeconds) {
      samples = {L"00:00:00 AM", L"00:00:00 PM"};
    } else {
      samples = {L"00:00 AM", L"00:00 PM"};
    }
    for (const std::wstring& sample : samples) {
      contentWidth = std::max(contentWidth,
          LayoutRow(measureDc, sample, style).width);
    }
    nudge = std::max(nudge, timeNudge);
  }
  DeleteDC(measureDc);

  overlay->edgePad = Scale(kEdgePaddingDip, overlay->dpi) + nudge;
  const int width = std::max(contentWidth, Scale(48, overlay->dpi)) +
      2 * overlay->edgePad;
  const int visibleRows = static_cast<int>(g_settings.showWeekday) +
      static_cast<int>(g_settings.showDate) +
      static_cast<int>(g_settings.showTime);
  int heightDip = 40;
  if (g_settings.showWeekday) heightDip += g_settings.dayFontSize;
  if (g_settings.showDate) heightDip += g_settings.dateFontSize;
  if (g_settings.showTime) heightDip += g_settings.timeFontSize;
  if (visibleRows > 1) {
    heightDip += g_settings.gapDayToDate;
    if (visibleRows > 2) heightDip += g_settings.gapDateToTime;
  }
  const int height = Scale(heightDip, overlay->dpi);

  LoadPositionFor(overlay);
  const POINT center = MonitorCenter(monitor.info.rcMonitor);
  const int x = center.x + Scale(overlay->offsetX, overlay->dpi) -
      ReferenceX(overlay, width);
  const int y = center.y + Scale(overlay->offsetY, overlay->dpi) - height / 2;
  DWORD extendedStyle = WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE;
  if (g_settings.lockWidgetPosition) extendedStyle |= WS_EX_TRANSPARENT;
  overlay->hwnd = CreateWindowExW(extendedStyle, kWindowClass,
    L"Mond Desktop Clock", WS_POPUP, x, y, width, height,
    nullptr, nullptr, g_hinstance, overlay);
  if (overlay->hwnd) {
    g_overlays.push_back(overlay);
    ShowWindow(overlay->hwnd, SW_SHOWNOACTIVATE);
    Paint(overlay);
  } else {
    DeleteFonts(overlay);
    delete overlay;
  }
}

static void RebuildOverlays() {
  if (g_rebuilding) return;
  g_rebuilding = true;
  DestroyOverlays();
  for (const MonitorEntry& monitor : SelectedMonitors()) CreateOverlay(monitor);
  SYSTEMTIME now{};
  GetLocalTime(&now);
  g_currentStamp = ClockStamp(now);
  g_rebuilding = false;
}

// ---------------------------------------------------------------------------
// Settings

static void LoadLetterColors(const wchar_t* group, size_t count,
                             COLORREF textColor, COLORREF shadowColor,
                             COLORREF* colors, COLORREF* shadows) {
  std::fill_n(colors, kMaxHeaderCharacters, textColor);
  std::fill_n(shadows, kMaxHeaderCharacters, shadowColor);
  for (size_t i = 0; i < count; ++i) {
    wchar_t name[96];
    swprintf_s(name, L"customLetterStyles.%s.color%u", group,
               static_cast<unsigned>(i + 1));
    colors[i] = LoadColorSetting(name, textColor);
    swprintf_s(name, L"customLetterStyles.%s.shadow%u", group,
               static_cast<unsigned>(i + 1));
    shadows[i] = LoadColorSetting(name, shadowColor);
  }
}

static void LoadSettings() {
  // Layout
  g_settings.mainHeading = LoadStringSetting(L"mainHeading", L"day");
  if (g_settings.mainHeading != L"day" &&
      g_settings.mainHeading != L"date" &&
      g_settings.mainHeading != L"time") {
    g_settings.mainHeading = L"day";
  }
  g_settings.alignment = ParseAlign(
      LoadStringSetting(L"alignment", L"center"), Align::Center);
  g_settings.showWeekday = Wh_GetIntSetting(L"showWeekday") != 0;
  g_settings.showDate = Wh_GetIntSetting(L"showDate") != 0;
  g_settings.showTime = Wh_GetIntSetting(L"showTime") != 0;
  g_settings.timeFormat24 = Wh_GetIntSetting(L"timeFormat24") != 0;
  g_settings.showSeconds = Wh_GetIntSetting(L"showSeconds") != 0;
  g_settings.gapDayToDate = std::clamp(Wh_GetIntSetting(L"gapDayToDate"), 0, 200);
  g_settings.gapDateToTime = std::clamp(Wh_GetIntSetting(L"gapDateToTime"), 0, 200);

  // Placement
  const std::wstring monitorMode = LoadStringSetting(L"monitorMode", L"all");
  g_settings.monitorMode = monitorMode == L"primary" ? MonitorMode::Primary
      : monitorMode == L"number" ? MonitorMode::Number
      : MonitorMode::All;
  g_settings.monitorNumber = std::max(1, Wh_GetIntSetting(L"monitorNumber"));
  g_settings.syncPositions = Wh_GetIntSetting(L"syncPositions") != 0;
  g_settings.lockWidgetPosition = Wh_GetIntSetting(L"lockWidgetPosition") != 0;

  const int configuredOffsetX = Wh_GetIntSetting(L"offsetX");
  const int configuredOffsetY = Wh_GetIntSetting(L"offsetY");
  int savedOffsetXSetting = Wh_GetIntValue(L"offsetXSetting", configuredOffsetX);
  int savedOffsetYSetting = Wh_GetIntValue(L"offsetYSetting", configuredOffsetY);
  if (Wh_GetIntValue(L"positionSettingsVersion", 0) == 0) {
    Wh_SetIntValue(L"offsetXSetting", configuredOffsetX);
    Wh_SetIntValue(L"offsetYSetting", configuredOffsetY);
    Wh_SetIntValue(L"positionSettingsVersion", 1);
    savedOffsetXSetting = configuredOffsetX;
    savedOffsetYSetting = configuredOffsetY;
  }
  g_positionGeneration = Wh_GetIntValue(L"positionGeneration", 0);
  if (configuredOffsetX != savedOffsetXSetting ||
      configuredOffsetY != savedOffsetYSetting) {
    // The offset setting was edited: discard dragged positions, both the
    // shared one and every per-monitor one.
    Wh_DeleteValue(L"offsetX");
    Wh_DeleteValue(L"offsetY");
    Wh_SetIntValue(L"offsetXSetting", configuredOffsetX);
    Wh_SetIntValue(L"offsetYSetting", configuredOffsetY);
    ++g_positionGeneration;
    Wh_SetIntValue(L"positionGeneration", g_positionGeneration);
  }
  g_positionSettingX = configuredOffsetX;
  g_positionSettingY = configuredOffsetY;
  g_settings.offsetX = Wh_GetIntValue(L"offsetX", configuredOffsetX);
  g_settings.offsetY = Wh_GetIntValue(L"offsetY", configuredOffsetY);

  // Weekday
  g_settings.dayFontFamily = LoadFontFamily(
      L"dayFontFamily", L"dayCustomFontFamily", L"Anurati");
  g_settings.dayFontSize = std::clamp(Wh_GetIntSetting(L"dayFontSize"), 12, 300);
  g_settings.dayBold = Wh_GetIntSetting(L"dayBold") != 0;
  g_settings.dayItalic = Wh_GetIntSetting(L"dayItalic") != 0;
  g_settings.dayLetterSpacing = std::clamp(Wh_GetIntSetting(L"dayLetterSpacing"), 0, 200);
  g_settings.dayAlignment = ParseAlign(
      LoadStringSetting(L"dayAlignment", L"default"), g_settings.alignment);
  g_settings.dayTextColor = LoadColorSetting(L"dayTextColor", RGB(255, 255, 255));
  g_settings.dayShadowColor = LoadColorSetting(L"dayShadowColor", RGB(64, 64, 64));
  g_settings.dayShadowOffset = LoadShadowOffset(L"dayShadowOffset", 2);

  // Date
  g_settings.dateFontFamily = LoadFontFamily(
      L"dateFontFamily", L"dateCustomFontFamily", L"Quicksand");
  g_settings.dateFontSize = std::clamp(Wh_GetIntSetting(L"dateFontSize"), 8, 100);
  g_settings.dateBold = Wh_GetIntSetting(L"dateBold") != 0;
  g_settings.dateItalic = Wh_GetIntSetting(L"dateItalic") != 0;
  g_settings.dateLetterSpacing = std::clamp(Wh_GetIntSetting(L"dateLetterSpacing"), 0, 50);
  g_settings.dateAlignment = ParseAlign(
      LoadStringSetting(L"dateAlignment", L"default"), g_settings.alignment);
  g_settings.dateLeftOffset = Wh_GetIntSetting(L"dateLeftOffset");
  g_settings.dateTextColor = LoadColorSetting(L"dateTextColor", RGB(255, 255, 255));
  g_settings.dateShadowColor = LoadColorSetting(L"dateShadowColor", RGB(64, 64, 64));
  g_settings.dateShadowOffset = LoadShadowOffset(L"dateShadowOffset", 2);

  // Time
  g_settings.timeFontFamily = LoadFontFamily(
      L"timeFontFamily", L"timeCustomFontFamily", L"Quicksand");
  g_settings.timeFontSize = std::clamp(Wh_GetIntSetting(L"timeFontSize"), 8, 100);
  g_settings.timeBold = Wh_GetIntSetting(L"timeBold") != 0;
  g_settings.timeItalic = Wh_GetIntSetting(L"timeItalic") != 0;
  g_settings.timeLetterSpacing = std::clamp(Wh_GetIntSetting(L"timeLetterSpacing"), 0, 50);
  g_settings.timeAlignment = ParseAlign(
      LoadStringSetting(L"timeAlignment", L"default"), g_settings.alignment);
  g_settings.timeLeftOffset = Wh_GetIntSetting(L"timeLeftOffset");
  g_settings.timeTextColor = LoadColorSetting(L"timeTextColor", RGB(255, 255, 255));
  g_settings.timeShadowColor = LoadColorSetting(L"timeShadowColor", RGB(64, 64, 64));
  g_settings.timeShadowOffset = LoadShadowOffset(L"timeShadowOffset", 2);

  // Per-letter
  LoadLetterColors(L"weekday", kWeekdayCharacterCount,
      g_settings.dayTextColor, g_settings.dayShadowColor,
      g_settings.weekdayLetterColors, g_settings.weekdayLetterShadowColors);
  LoadLetterColors(L"date", kDateCharacterCount,
      g_settings.dateTextColor, g_settings.dateShadowColor,
      g_settings.dateLetterColors, g_settings.dateLetterShadowColors);
  LoadLetterColors(L"time", kTimeCharacterCount,
      g_settings.timeTextColor, g_settings.timeShadowColor,
      g_settings.timeLetterColors, g_settings.timeLetterShadowColors);
}

template <size_t N>
static bool SameArray(const COLORREF (&first)[N], const COLORREF (&second)[N]) {
  return std::equal(std::begin(first), std::end(first), std::begin(second));
}

// Everything except the lock flag and the dragged position.
static bool SameAppearanceSettings(const Settings& a, const Settings& b) {
  return a.mainHeading == b.mainHeading &&
      a.alignment == b.alignment &&
      a.showWeekday == b.showWeekday &&
      a.showDate == b.showDate &&
      a.showTime == b.showTime &&
      a.timeFormat24 == b.timeFormat24 &&
      a.showSeconds == b.showSeconds &&
      a.gapDayToDate == b.gapDayToDate &&
      a.gapDateToTime == b.gapDateToTime &&
      a.monitorMode == b.monitorMode &&
      a.monitorNumber == b.monitorNumber &&
      a.syncPositions == b.syncPositions &&
      a.dayFontFamily == b.dayFontFamily &&
      a.dayFontSize == b.dayFontSize &&
      a.dayBold == b.dayBold &&
      a.dayItalic == b.dayItalic &&
      a.dayLetterSpacing == b.dayLetterSpacing &&
      a.dayAlignment == b.dayAlignment &&
      a.dayTextColor == b.dayTextColor &&
      a.dayShadowColor == b.dayShadowColor &&
      a.dayShadowOffset == b.dayShadowOffset &&
      a.dateFontFamily == b.dateFontFamily &&
      a.dateFontSize == b.dateFontSize &&
      a.dateBold == b.dateBold &&
      a.dateItalic == b.dateItalic &&
      a.dateLetterSpacing == b.dateLetterSpacing &&
      a.dateAlignment == b.dateAlignment &&
      a.dateLeftOffset == b.dateLeftOffset &&
      a.dateTextColor == b.dateTextColor &&
      a.dateShadowColor == b.dateShadowColor &&
      a.dateShadowOffset == b.dateShadowOffset &&
      a.timeFontFamily == b.timeFontFamily &&
      a.timeFontSize == b.timeFontSize &&
      a.timeBold == b.timeBold &&
      a.timeItalic == b.timeItalic &&
      a.timeLetterSpacing == b.timeLetterSpacing &&
      a.timeAlignment == b.timeAlignment &&
      a.timeLeftOffset == b.timeLeftOffset &&
      a.timeTextColor == b.timeTextColor &&
      a.timeShadowColor == b.timeShadowColor &&
      a.timeShadowOffset == b.timeShadowOffset &&
      SameArray(a.weekdayLetterColors, b.weekdayLetterColors) &&
      SameArray(a.weekdayLetterShadowColors, b.weekdayLetterShadowColors) &&
      SameArray(a.dateLetterColors, b.dateLetterColors) &&
      SameArray(a.dateLetterShadowColors, b.dateLetterShadowColors) &&
      SameArray(a.timeLetterColors, b.timeLetterColors) &&
      SameArray(a.timeLetterShadowColors, b.timeLetterShadowColors);
}

static void ApplySettings(bool forceRebuild = false) {
  Settings previous = g_settings;
  LoadSettings();
  if (!forceRebuild &&
      previous.lockWidgetPosition != g_settings.lockWidgetPosition &&
      SameAppearanceSettings(previous, g_settings)) {
    g_settings.offsetX = previous.offsetX;
    g_settings.offsetY = previous.offsetY;
    for (Overlay* overlay : g_overlays) {
      LONG_PTR style = GetWindowLongPtrW(overlay->hwnd, GWL_EXSTYLE);
      LONG_PTR updatedStyle = g_settings.lockWidgetPosition
          ? style | WS_EX_TRANSPARENT
          : style & ~static_cast<LONG_PTR>(WS_EX_TRANSPARENT);
      SetWindowLongPtrW(overlay->hwnd, GWL_EXSTYLE, updatedStyle);
      SetWindowPos(overlay->hwnd, nullptr, 0, 0, 0, 0,
          SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE |
              SWP_FRAMECHANGED);
    }
    return;
  }
  RebuildOverlays();
  ScheduleClockTick();
}

static void ScheduleRebuild(HWND hwnd) {
  g_rebuildPending = true;
  SetTimer(hwnd, kRelayoutTimerId, 300, nullptr);
}

static LRESULT CALLBACK ControllerWndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
  if (g_taskbarCreatedMessage && msg == g_taskbarCreatedMessage) {
    g_desktopHost = FindDesktopHost();
    ScheduleRebuild(hwnd);
    return 0;
  }
  switch (msg) {
    case WM_TIMER:
      if (wp == kClockTimerId) {
        UpdateClock();
        ScheduleClockTick();
        return 0;
      }
      if (wp == kWatchTimerId) {
        HWND host = FindDesktopHost();
        bool missingWindow = std::any_of(g_overlays.begin(), g_overlays.end(),
          [](Overlay* overlay) { return !IsWindow(overlay->hwnd); });
        if (host != g_desktopHost || missingWindow) {
          g_desktopHost = host;
          ScheduleRebuild(hwnd);
        }
        UpdateClock();
        return 0;
      }
      if (wp == kRelayoutTimerId) {
        KillTimer(hwnd, kRelayoutTimerId);
        if (g_inMoveLoop) {
          SetTimer(hwnd, kRelayoutTimerId, 300, nullptr);
          return 0;
        }
        if (g_settingsUpdatePending) {
          g_settingsUpdatePending = false;
          const bool rebuild = g_rebuildPending;
          g_rebuildPending = false;
          ApplySettings(rebuild);
          return 0;
        }
        g_rebuildPending = false;
        RebuildOverlays();
        return 0;
      }
      break;
    case WM_DISPLAYCHANGE:
      g_desktopHost = FindDesktopHost();
      ScheduleRebuild(hwnd);
      return 0;
    case WM_DPICHANGED:
      ScheduleRebuild(hwnd);
      return 0;
    case WM_TIMECHANGE:
    case WM_SETTINGCHANGE:
      UpdateClock(true);
      ScheduleClockTick();
      return 0;
    case WM_POWERBROADCAST:
      if (wp == PBT_APMRESUMEAUTOMATIC || wp == PBT_APMRESUMESUSPEND) {
        UpdateClock(true);
        ScheduleClockTick();
        return TRUE;
      }
      return 0;
    case kSettingsMessage:
      if (g_inMoveLoop) {
        g_settingsUpdatePending = true;
        SetTimer(hwnd, kRelayoutTimerId, 300, nullptr);
        return 0;
      }
      g_settingsUpdatePending = false;
      KillTimer(hwnd, kRelayoutTimerId);
      {
        const bool rebuild = g_rebuildPending;
        g_rebuildPending = false;
        ApplySettings(rebuild);
      }
      return 0;
    case kShutdownMessage:
      KillTimer(hwnd, kWatchTimerId);
      KillTimer(hwnd, kClockTimerId);
      DestroyOverlays();
      DestroyWindow(hwnd);
      return 0;
    case WM_DESTROY:
      g_controller.store(nullptr);
      PostQuitMessage(0);
      return 0;
  }
  return DefWindowProcW(hwnd, msg, wp, lp);
}

static DWORD WINAPI UiThreadProc(void*) {
  LoadSettings();
  WNDCLASSW windowClass{};
  windowClass.lpfnWndProc = WndProc;
  windowClass.hInstance = g_hinstance;
  windowClass.lpszClassName = kWindowClass;
  windowClass.hCursor = LoadCursorW(nullptr, IDC_ARROW);
  bool windowClassRegistered = RegisterClassW(&windowClass) != 0;
  WNDCLASSW controllerClass{};
  controllerClass.lpfnWndProc = ControllerWndProc;
  controllerClass.hInstance = g_hinstance;
  controllerClass.lpszClassName = kControllerClass;
  bool controllerClassRegistered = windowClassRegistered &&
                    RegisterClassW(&controllerClass) != 0;
  if (controllerClassRegistered) {
    g_controller.store(CreateWindowExW(WS_EX_TOOLWINDOW, kControllerClass, L"",
      WS_POPUP, 0, 0, 0, 0, nullptr, nullptr, g_hinstance, nullptr));
  }
  if (g_controller.load()) {
    g_shcore = LoadLibraryExW(L"shcore.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (g_shcore) {
      g_getDpiForMonitor = reinterpret_cast<GetDpiForMonitor_t>(
          GetProcAddress(g_shcore, "GetDpiForMonitor"));
    }
    g_taskbarCreatedMessage = RegisterWindowMessageW(L"TaskbarCreated");
    g_desktopHost = FindDesktopHost();
    RebuildOverlays();
    g_uiStartOk = !g_overlays.empty();
    if (g_uiStartOk) {
      SetTimer(g_controller.load(), kWatchTimerId, kWatchIntervalMs, nullptr);
      ScheduleClockTick();
    }
  }
  SetEvent(g_readyEvent);

  if (g_uiStartOk) {
    MSG message{};
    BOOL result;
    while ((result = GetMessageW(&message, nullptr, 0, 0)) > 0) {
      TranslateMessage(&message);
      DispatchMessageW(&message);
    }
  }
  DestroyOverlays();
  if (HWND controller = g_controller.load()) DestroyWindow(controller);
  if (g_shcore) FreeLibrary(g_shcore);
  g_shcore = nullptr;
  g_getDpiForMonitor = nullptr;
  if (controllerClassRegistered) UnregisterClassW(kControllerClass, g_hinstance);
  if (windowClassRegistered) UnregisterClassW(kWindowClass, g_hinstance);
  return 0;
}

BOOL WhTool_ModInit() {
  if (!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
              GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
              reinterpret_cast<LPCWSTR>(&g_hinstance),
              &g_hinstance)) {
    return FALSE;
  }
  g_readyEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
  if (!g_readyEvent) return FALSE;
  g_uiThread = CreateThread(nullptr, 0, UiThreadProc, nullptr, 0, &g_uiThreadId);
  if (!g_uiThread) {
    CloseHandle(g_readyEvent);
    g_readyEvent = nullptr;
    return FALSE;
  }
  WaitForSingleObject(g_readyEvent, INFINITE);
  CloseHandle(g_readyEvent);
  g_readyEvent = nullptr;
  if (!g_uiStartOk) {
    WaitForSingleObject(g_uiThread, INFINITE);
    CloseHandle(g_uiThread);
    g_uiThread = nullptr;
    return FALSE;
  }
  return TRUE;
}

void WhTool_ModUninit() {
  if (g_uiThread) {
    HWND controller = g_controller.load();
    if (!controller || !PostMessageW(controller, kShutdownMessage, 0, 0)) {
      PostThreadMessageW(g_uiThreadId, WM_QUIT, 0, 0);
    }
    WaitForSingleObject(g_uiThread, INFINITE);
    CloseHandle(g_uiThread);
    g_uiThread = nullptr;
  }
}

void WhTool_ModSettingsChanged() {
  if (HWND controller = g_controller.load())
    PostMessageW(controller, kSettingsMessage, 0, 0);
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
