// ==WindhawkMod==
// @id mond-desktop-clock
// @name Mond Desktop Clock
// @description A universal desktop clock widget with configurable typography, layout, and placement.
// @version 3.5
// @author Elmidin Mahmud
// @github https://github.com/elmidin
// @include windhawk.exe
// @compilerOptions -lgdi32 -luser32 -lshell32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Mond Desktop Clock

A universal desktop clock widget inspired by the [Mond Rainmeter skin by HipHopium](https://www.deviantart.com/hiphopium). It runs in a dedicated tool process, not in Explorer.

The clock shows the weekday, date, and time in three independently styled rows. Drag it to position it; enable position locking to prevent accidental movement.

![Mond Desktop Clock on the desktop](https://i.imgur.com/dj71eKl.png)

![Mond Desktop Clock alternate layout](https://i.imgur.com/ux3bwti.png)

- Drag-and-drop placement with a saved position shared across monitors
- Independent font families, sizes, bold/italic styling, letter spacing, and horizontal alignment for each row
- Weekday, date, or time can be selected as the large heading and moved to the top
- Per-character text and shadow colours for weekday, date, and time
- Anurati lettering with adjustable shadows; Anurati and Quicksand are the defaults
- Adjustable shadows for the weekday, date, and time rows
- 12-hour or 24-hour time and an optional date row

Fonts must be installed in Windows. Missing fonts are silently substituted by Windows.

- [Quicksand](https://fonts.google.com/specimen/Quicksand)
- [Anurati](https://www.dafont.com/anurati.font)

Weekday, date, and time have separate text and shadow colours. Letter colours and shadows are set independently for each row; blank overrides use that row's text or shadow colour. The date displays without a comma. Colours accept #RRGGBB or #AARRGGBB; the alpha component in the latter form is ignored.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- mainHeading: day
  $name: Main heading
  $options:
    - day: Weekday
    - date: Date
    - time: Time

- timeFormat24: false
  $name: Use 24-hour time

- showWeekday: true
  $name: Show weekday

- showDate: true
  $name: Show date

- showTime: true
  $name: Show time

- lockWidgetPosition: false
  $name: Lock position

- dateLeftOffset: 0
  $name: Date horizontal offset
  $description: Positive values shift the date left.

- timeLeftOffset: 0
  $name: Time horizontal offset
  $description: Positive values shift the time left.

- offsetX: 0
  $name: Horizontal position offset
  $description: Shared across monitors. Dragging saves a position until this setting changes.

- offsetY: 0
  $name: Vertical position offset
  $description: Shared across monitors. Dragging saves a position until this setting changes.

- gapDayToDate: 42
  $name: Gap after main heading

- gapDateToTime: 25
  $name: Gap between other rows

- dayFontSize: 72
  $name: Weekday size

- dayFontFamily: Anurati
  $name: Weekday font
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

- dayItalic: false
  $name: Weekday italics

- dayBold: false
  $name: Weekday bold

- dayLetterSpacing: 40
  $name: Weekday letter spacing

- dateFontSize: 18
  $name: Date size

- dateFontFamily: Quicksand
  $name: Date font
  $description: Anurati digits and punctuation are rendered in Quicksand for readability.
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

- dateItalic: false
  $name: Date italics

- dateBold: true
  $name: Date bold

- dateLetterSpacing: 4
  $name: Date letter spacing

- timeFontSize: 18
  $name: Time size

- timeFontFamily: Quicksand
  $name: Time font
  $description: Anurati digits and punctuation are rendered in Quicksand for readability.
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

- timeItalic: false
  $name: Time italics

- timeBold: true
  $name: Time bold

- timeLetterSpacing: 4
  $name: Time letter spacing

- dayTextColor: "#FFFFFFFF"
  $name: Weekday text colour
  $description: Leave blank to use white.
  #! $format: colorRgb

- dateTextColor: "#FFFFFFFF"
  $name: Date text colour
  $description: Leave blank to use white.
  #! $format: colorRgb

- timeTextColor: "#FFFFFFFF"
  $name: Time text colour
  $description: Leave blank to use white.
  #! $format: colorRgb

- dayShadowColor: "#404040"
  $name: Weekday default shadow colour
  #! $format: colorRgb

- dateShadowColor: "#404040"
  $name: Date default shadow colour
  #! $format: colorRgb

- timeShadowColor: "#404040"
  $name: Time default shadow colour
  #! $format: colorRgb

- weekdayLetterColor1: ""
  $name: Weekday letter 1 colour
  #! $format: colorRgb
- weekdayLetterColor2: ""
  $name: Weekday letter 2 colour
  #! $format: colorRgb
- weekdayLetterColor3: ""
  $name: Weekday letter 3 colour
  #! $format: colorRgb
- weekdayLetterColor4: ""
  $name: Weekday letter 4 colour
  #! $format: colorRgb
- weekdayLetterColor5: ""
  $name: Weekday letter 5 colour
  #! $format: colorRgb
- weekdayLetterColor6: ""
  $name: Weekday letter 6 colour
  #! $format: colorRgb
- weekdayLetterColor7: ""
  $name: Weekday letter 7 colour
  #! $format: colorRgb
- weekdayLetterColor8: ""
  $name: Weekday letter 8 colour
  #! $format: colorRgb
- weekdayLetterColor9: ""
  $name: Weekday letter 9 colour
  #! $format: colorRgb

- customLetterStyles:
    - weekday:
        - shadowOffset: "2"
          $name: Weekday letter shadow offset
          $options:
            - "0": Off
            - "1": 1 DIP
            - "2": 2 DIPs
            - "3": 3 DIPs
            - "4": 4 DIPs
            - "5": 5 DIPs
            - "8": 8 DIPs
            - "10": 10 DIPs
            - "16": 16 DIPs
            - "20": 20 DIPs
        - shadow1: ""
          $name: Weekday letter 1 shadow
          #! $format: colorRgb
        - shadow2: ""
          $name: Weekday letter 2 shadow
          #! $format: colorRgb
        - shadow3: ""
          $name: Weekday letter 3 shadow
          #! $format: colorRgb
        - shadow4: ""
          $name: Weekday letter 4 shadow
          #! $format: colorRgb
        - shadow5: ""
          $name: Weekday letter 5 shadow
          #! $format: colorRgb
        - shadow6: ""
          $name: Weekday letter 6 shadow
          #! $format: colorRgb
        - shadow7: ""
          $name: Weekday letter 7 shadow
          #! $format: colorRgb
        - shadow8: ""
          $name: Weekday letter 8 shadow
          #! $format: colorRgb
        - shadow9: ""
          $name: Weekday letter 9 shadow
          #! $format: colorRgb
      $name: Weekday letter shadows
    - date:
        - shadowOffset: "2"
          $name: Date letter shadow offset
          $options:
            - "0": Off
            - "1": 1 DIP
            - "2": 2 DIPs
            - "3": 3 DIPs
            - "4": 4 DIPs
            - "5": 5 DIPs
            - "8": 8 DIPs
            - "10": 10 DIPs
            - "16": 16 DIPs
            - "20": 20 DIPs
        - color1: ""
          $name: Date letter 1 colour
          #! $format: colorRgb
        - shadow1: ""
          $name: Date letter 1 shadow
          #! $format: colorRgb
        - color2: ""
          $name: Date letter 2 colour
          #! $format: colorRgb
        - shadow2: ""
          $name: Date letter 2 shadow
          #! $format: colorRgb
        - color3: ""
          $name: Date letter 3 colour
          #! $format: colorRgb
        - shadow3: ""
          $name: Date letter 3 shadow
          #! $format: colorRgb
        - color4: ""
          $name: Date letter 4 colour
          #! $format: colorRgb
        - shadow4: ""
          $name: Date letter 4 shadow
          #! $format: colorRgb
        - color5: ""
          $name: Date letter 5 colour
          #! $format: colorRgb
        - shadow5: ""
          $name: Date letter 5 shadow
          #! $format: colorRgb
        - color6: ""
          $name: Date letter 6 colour
          #! $format: colorRgb
        - shadow6: ""
          $name: Date letter 6 shadow
          #! $format: colorRgb
        - color7: ""
          $name: Date letter 7 colour
          #! $format: colorRgb
        - shadow7: ""
          $name: Date letter 7 shadow
          #! $format: colorRgb
        - color8: ""
          $name: Date letter 8 colour
          #! $format: colorRgb
        - shadow8: ""
          $name: Date letter 8 shadow
          #! $format: colorRgb
        - color9: ""
          $name: Date letter 9 colour
          #! $format: colorRgb
        - shadow9: ""
          $name: Date letter 9 shadow
          #! $format: colorRgb
        - color10: ""
          $name: Date letter 10 colour
          #! $format: colorRgb
        - shadow10: ""
          $name: Date letter 10 shadow
          #! $format: colorRgb
        - color11: ""
          $name: Date letter 11 colour
          #! $format: colorRgb
        - shadow11: ""
          $name: Date letter 11 shadow
          #! $format: colorRgb
        - color12: ""
          $name: Date letter 12 colour
          #! $format: colorRgb
        - shadow12: ""
          $name: Date letter 12 shadow
          #! $format: colorRgb
        - color13: ""
          $name: Date letter 13 colour
          #! $format: colorRgb
        - shadow13: ""
          $name: Date letter 13 shadow
          #! $format: colorRgb
        - color14: ""
          $name: Date letter 14 colour
          #! $format: colorRgb
        - shadow14: ""
          $name: Date letter 14 shadow
          #! $format: colorRgb
        - color15: ""
          $name: Date letter 15 colour
          #! $format: colorRgb
        - shadow15: ""
          $name: Date letter 15 shadow
          #! $format: colorRgb
        - color16: ""
          $name: Date letter 16 colour
          #! $format: colorRgb
        - shadow16: ""
          $name: Date letter 16 shadow
          #! $format: colorRgb
        - color17: ""
          $name: Date letter 17 colour
          #! $format: colorRgb
        - shadow17: ""
          $name: Date letter 17 shadow
          #! $format: colorRgb
      $name: Date letter colours and shadows
    - time:
        - shadowOffset: "2"
          $name: Time letter shadow offset
          $options:
            - "0": Off
            - "1": 1 DIP
            - "2": 2 DIPs
            - "3": 3 DIPs
            - "4": 4 DIPs
            - "5": 5 DIPs
            - "8": 8 DIPs
            - "10": 10 DIPs
            - "16": 16 DIPs
            - "20": 20 DIPs
        - color1: ""
          $name: Time letter 1 colour
          #! $format: colorRgb
        - shadow1: ""
          $name: Time letter 1 shadow
          #! $format: colorRgb
        - color2: ""
          $name: Time letter 2 colour
          #! $format: colorRgb
        - shadow2: ""
          $name: Time letter 2 shadow
          #! $format: colorRgb
        - color3: ""
          $name: Time letter 3 colour
          #! $format: colorRgb
        - shadow3: ""
          $name: Time letter 3 shadow
          #! $format: colorRgb
        - color4: ""
          $name: Time letter 4 colour
          #! $format: colorRgb
        - shadow4: ""
          $name: Time letter 4 shadow
          #! $format: colorRgb
        - color5: ""
          $name: Time letter 5 colour
          #! $format: colorRgb
        - shadow5: ""
          $name: Time letter 5 shadow
          #! $format: colorRgb
        - color6: ""
          $name: Time letter 6 colour
          #! $format: colorRgb
        - shadow6: ""
          $name: Time letter 6 shadow
          #! $format: colorRgb
        - color7: ""
          $name: Time letter 7 colour
          #! $format: colorRgb
        - shadow7: ""
          $name: Time letter 7 shadow
          #! $format: colorRgb
        - color8: ""
          $name: Time letter 8 colour
          #! $format: colorRgb
        - shadow8: ""
          $name: Time letter 8 shadow
          #! $format: colorRgb
        - color9: ""
          $name: Time letter 9 colour
          #! $format: colorRgb
        - shadow9: ""
          $name: Time letter 9 shadow
          #! $format: colorRgb
      $name: Time letter colours and shadows
  $name: Custom colours per letter
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <shellapi.h>
#include <windhawk_api.h>
#include <string>
#include <vector>
#include <algorithm>
#include <cwchar>
#include <cstdlib>
#include <atomic>
#include <iterator>

static constexpr wchar_t kWindowClass[] = L"WindhawkMondDesktopClock";
static constexpr wchar_t kControllerClass[] = L"WindhawkMondDesktopClockController";
static constexpr UINT kShutdownMessage = WM_APP + 1;
static constexpr UINT kSettingsMessage = WM_APP + 2;
static constexpr UINT_PTR kWatchTimerId = 1;
static constexpr UINT_PTR kRelayoutTimerId = 3;
static constexpr UINT kWatchIntervalMs = 2000;
static constexpr size_t kWeekdayCharacterCount = 9;
static constexpr size_t kDateCharacterCount = 17;
static constexpr size_t kTimeCharacterCount = 9;
static constexpr size_t kMaxHeaderCharacters = kDateCharacterCount;
using GetDpiForMonitor_t = HRESULT(WINAPI*)(HMONITOR, int, UINT*, UINT*);

struct Overlay {
  HWND hwnd = nullptr;
  UINT dpi = 96;
  HFONT dayFont = nullptr;
  HFONT timeFont = nullptr;
  HFONT dateFont = nullptr;
  HFONT timeFallbackFont = nullptr;
  HFONT dateFallbackFont = nullptr;
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
static int g_currentDay = -1;
static int g_currentMinute = -1;
static int g_positionSettingX = 0;
static int g_positionSettingY = 0;
static HMODULE g_shcore = nullptr;
static GetDpiForMonitor_t g_getDpiForMonitor = nullptr;

struct Settings {
  std::wstring dayFontFamily;
  std::wstring timeFontFamily;
  std::wstring dateFontFamily;
  std::wstring mainHeading;
  int dayFontSize;
  int dayLetterSpacing;
  int dayCustomShadowOffset;
  bool dayBold;
  bool dayItalic;
  int timeFontSize;
  int timeLetterSpacing;
  bool timeBold;
  bool timeItalic;
  int timeCustomShadowOffset;
  int dateFontSize;
  int dateLetterSpacing;
  bool dateBold;
  bool dateItalic;
  int dateCustomShadowOffset;
  int gapDayToDate;
  int gapDateToTime;
  int dateLeftOffset;
  int timeLeftOffset;
  bool lockWidgetPosition;
  int offsetX;
  int offsetY;
  bool timeFormat24;
  bool showWeekday;
  bool showDate;
  bool showTime;
  COLORREF dayTextColor;
  COLORREF dayShadowColor;
  COLORREF timeShadowColor;
  COLORREF dateShadowColor;
  COLORREF dateTextColor;
  COLORREF timeTextColor;
  COLORREF weekdayLetterColors[kMaxHeaderCharacters];
  COLORREF weekdayLetterShadowColors[kMaxHeaderCharacters];
  COLORREF dateLetterColors[kMaxHeaderCharacters];
  COLORREF dateLetterShadowColors[kMaxHeaderCharacters];
  COLORREF timeLetterColors[kMaxHeaderCharacters];
  COLORREF timeLetterShadowColors[kMaxHeaderCharacters];
};
static Settings g_settings;

static COLORREF ParseColor(const wchar_t* s) {
    if (!s) return RGB(255, 255, 255);
    if (*s == L'#') ++s;
    const size_t n = wcslen(s);
    const unsigned long v = wcstoul(s, nullptr, 16);
    if (n == 8 || n == 6) {
        return RGB((v >> 16) & 255, (v >> 8) & 255, v & 255);
    }
    return RGB(255, 255, 255);
}

static COLORREF LoadColorSetting(const wchar_t* name, COLORREF fallback) {
  PCWSTR value = Wh_GetStringSetting(name);
  COLORREF color = *value ? ParseColor(value) : fallback;
  Wh_FreeStringSetting(value);
  return color;
}

static int LoadShadowOffset(const wchar_t* name, int fallback) {
  PCWSTR value = Wh_GetStringSetting(name);
  const int result = !*value || _wcsicmp(value, L"default") == 0
      ? fallback : std::clamp(_wtoi(value), 0, 20);
  Wh_FreeStringSetting(value);
  return result;
}

static std::wstring LoadStringSetting(const wchar_t* name,
                                      const wchar_t* fallback) {
  PCWSTR value = Wh_GetStringSetting(name);
  std::wstring result = *value ? value : fallback;
  Wh_FreeStringSetting(value);
  return result;
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

static int DayDisplayFontSize() {
  return g_settings.dayFontSize;
}

static int DateDisplayFontSize() {
  return g_settings.dateFontSize;
}

static int TimeDisplayFontSize() {
  return g_settings.timeFontSize;
}

static void CreateFonts(Overlay* overlay) {
  DeleteFonts(overlay);
  int daySize = MulDiv(DayDisplayFontSize(), overlay->dpi, 96);
  int timeSize = MulDiv(TimeDisplayFontSize(), overlay->dpi, 96);
  int dateSize = MulDiv(DateDisplayFontSize(), overlay->dpi, 96);
  overlay->dayFont = CreateFontW(-daySize, 0, 0, 0,
      g_settings.dayBold ? FW_BOLD : FW_NORMAL, g_settings.dayItalic,
      FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
      CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH,
      g_settings.dayFontFamily.c_str());
  overlay->timeFont = CreateFontW(-timeSize, 0, 0, 0,
      g_settings.timeBold ? FW_BOLD : FW_NORMAL, g_settings.timeItalic,
      FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
      CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH,
      g_settings.timeFontFamily.c_str());
  overlay->dateFont = CreateFontW(-dateSize, 0, 0, 0,
      g_settings.dateBold ? FW_BOLD : FW_NORMAL, g_settings.dateItalic,
      FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
      CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH,
      g_settings.dateFontFamily.c_str());
    overlay->timeFallbackFont = CreateFontW(-timeSize, 0, 0, 0,
      g_settings.timeBold ? FW_BOLD : FW_NORMAL, g_settings.timeItalic,
      FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
      CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH, L"Quicksand");
    overlay->dateFallbackFont = CreateFontW(-dateSize, 0, 0, 0,
      g_settings.dateBold ? FW_BOLD : FW_NORMAL, g_settings.dateItalic,
      FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
      CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH, L"Quicksand");
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

static std::wstring CurrentWeekday() {
    SYSTEMTIME st{};
    GetLocalTime(&st);
    static const wchar_t* names[] = {
        L"SUNDAY", L"MONDAY", L"TUESDAY", L"WEDNESDAY",
        L"THURSDAY", L"FRIDAY", L"SATURDAY"};
    return names[st.wDayOfWeek];
}

static std::wstring CurrentTime() {
    SYSTEMTIME st{};
    GetLocalTime(&st);
    wchar_t b[64]{};
    if (g_settings.timeFormat24) {
      swprintf_s(b, L"%02u:%02u", st.wHour, st.wMinute);
    } else {
        unsigned h = st.wHour % 12;
        if (!h) h = 12;
      swprintf_s(b, L"%u:%02u %s", h, st.wMinute,
                   st.wHour >= 12 ? L"PM" : L"AM");
    }
    return b;
}

static std::wstring CurrentDate() {
    SYSTEMTIME st{};
    GetLocalTime(&st);
    wchar_t b[64]{};
    static const wchar_t* months[] = {
      L"January", L"February", L"March", L"April", L"May", L"June",
      L"July", L"August", L"September", L"October", L"November", L"December"};
    const unsigned month = std::clamp<unsigned>(st.wMonth, 1, 12);
    swprintf_s(b, L"%02u %s %04u", st.wDay, months[month - 1], st.wYear);
    return b;
}

static HFONT FontForCharacter(HFONT font,
                              HFONT fallbackFont,
                              wchar_t character) {
  return fallbackFont && (character < L'A' || character > L'Z')
      ? fallbackFont
      : font;
}

static void DrawCenteredSpaced(HDC dc, const std::wstring& text, int y, HFONT font,
                               int spacing, COLORREF color, int centreX,
                               HFONT fallbackFont = nullptr) {
  HGDIOBJ oldFont = SelectObject(dc, font);
  SetTextColor(dc, color);
  SetBkMode(dc, TRANSPARENT);

  if (fallbackFont) {
    SetTextCharacterExtra(dc, 0);
    int totalWidth = 0;
    for (wchar_t character : text) {
      HGDIOBJ previousFont = SelectObject(
          dc, FontForCharacter(font, fallbackFont, character));
      SIZE size{};
      GetTextExtentPoint32W(dc, &character, 1, &size);
      totalWidth += size.cx;
      SelectObject(dc, previousFont);
    }
    if (!text.empty()) totalWidth += spacing * (static_cast<int>(text.size()) - 1);
    int currentX = centreX - totalWidth / 2;
    for (wchar_t character : text) {
      HGDIOBJ previousFont = SelectObject(
          dc, FontForCharacter(font, fallbackFont, character));
      SIZE size{};
      GetTextExtentPoint32W(dc, &character, 1, &size);
      TextOutW(dc, currentX, y, &character, 1);
      currentX += size.cx + spacing;
      SelectObject(dc, previousFont);
    }
    SelectObject(dc, oldFont);
    return;
  }

  SetTextCharacterExtra(dc, spacing);

  SIZE sz{};
  GetTextExtentPoint32W(dc, text.c_str(), (int)text.size(), &sz);
  if (!text.empty()) sz.cx -= spacing;
  const int x = centreX - sz.cx / 2;
  TextOutW(dc, x, y, text.c_str(), (int)text.size());

  SetTextCharacterExtra(dc, 0);
  SelectObject(dc, oldFont);
}

static void DrawHeaderCharacters(HDC dc, const std::wstring& text, int y,
                                 HFONT font, HFONT fallbackFont, int spacing,
                                 int kerning, const COLORREF* colors,
                                 COLORREF defaultColor, int centreX) {
  HGDIOBJ oldFont = SelectObject(dc, font);
  SetBkMode(dc, TRANSPARENT);

  std::vector<int> charWidths(text.size(), 0);
  int totalWidth = 0;
  for (size_t i = 0; i < text.size(); ++i) {
    HGDIOBJ previousFont = SelectObject(
        dc, FontForCharacter(font, fallbackFont, text[i]));
    SIZE size{};
    GetTextExtentPoint32W(dc, &text[i], 1, &size);
    charWidths[i] = size.cx;
    totalWidth += size.cx;
    SelectObject(dc, previousFont);
  }

  if (!text.empty()) {
    totalWidth += spacing * (static_cast<int>(text.size()) - 1);
    if (text.find(L"DA") != std::wstring::npos) totalWidth -= kerning;
  }

  int currentX = centreX - totalWidth / 2;
  for (size_t i = 0; i < text.size(); ++i) {
    HGDIOBJ previousFont = SelectObject(
        dc, FontForCharacter(font, fallbackFont, text[i]));
    SetTextColor(dc, i < kMaxHeaderCharacters ? colors[i] : defaultColor);
    if (i > 0 && text[i] == L'A' && text[i - 1] == L'D')
      currentX -= kerning;
    TextOutW(dc, currentX, y, &text[i], 1);
    currentX += charWidths[i] + spacing;
    SelectObject(dc, previousFont);
  }

  SelectObject(dc, oldFont);
}

static int MeasureText(HDC dc, const std::wstring& text, HFONT font, int spacing,
                       HFONT fallbackFont = nullptr) {
  HGDIOBJ oldFont = SelectObject(dc, font);
  if (fallbackFont) {
    int width = 0;
    for (wchar_t character : text) {
      HGDIOBJ previousFont = SelectObject(
          dc, FontForCharacter(font, fallbackFont, character));
      SIZE size{};
      GetTextExtentPoint32W(dc, &character, 1, &size);
      width += size.cx;
      SelectObject(dc, previousFont);
    }
    SelectObject(dc, oldFont);
    return width + (text.empty() ? 0 : spacing * (static_cast<int>(text.size()) - 1));
  }
  SIZE size{};
  GetTextExtentPoint32W(dc, text.c_str(), static_cast<int>(text.size()), &size);
  SelectObject(dc, oldFont);
  return size.cx + (text.empty() ? 0 : spacing * (static_cast<int>(text.size()) - 1));
}

static int MeasureWeekday(HDC dc, const std::wstring& text, Overlay* overlay,
              int spacing) {
  HGDIOBJ oldFont = SelectObject(dc, overlay->dayFont);
  int width = 0;
  for (wchar_t ch : text) {
    SIZE size{};
    GetTextExtentPoint32W(dc, &ch, 1, &size);
    width += size.cx;
  }
  SelectObject(dc, oldFont);
  if (!text.empty()) width += spacing * (static_cast<int>(text.size()) - 1);
  if (text.find(L"DA") != std::wstring::npos)
    width -= WeekdayKerning(overlay->dpi);
  return width;
}

static void Paint(Overlay* overlay) {
  HWND hwnd = overlay->hwnd;
  RECT client{};
  if (!GetClientRect(hwnd, &client)) return;
  int width = client.right, height = client.bottom;
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

  const std::wstring day = CurrentWeekday();
  const std::wstring date = CurrentDate();
  const std::wstring time = CurrentTime();
  int xCenter = width / 2;
  int y = Scale(10, overlay->dpi);
  enum class Row { Day, Date, Time };
  std::vector<Row> rows;
  if (g_settings.mainHeading == L"date") {
    rows = {Row::Date, Row::Day, Row::Time};
  } else if (g_settings.mainHeading == L"time") {
    rows = {Row::Time, Row::Day, Row::Date};
  } else {
    rows = {Row::Day, Row::Date, Row::Time};
  }

  std::vector<Row> visibleRows;
  for (Row row : rows) {
    if ((row == Row::Day && g_settings.showWeekday) ||
        (row == Row::Date && g_settings.showDate) ||
        (row == Row::Time && g_settings.showTime)) {
      visibleRows.push_back(row);
    }
  }

  for (size_t index = 0; index < visibleRows.size(); ++index) {
    const Row row = visibleRows[index];
    const std::wstring* text = &day;
    HFONT font = overlay->dayFont;
    HFONT fallbackFont = nullptr;
    int fontSize = DayDisplayFontSize();
    int spacing = Scale(g_settings.dayLetterSpacing, overlay->dpi);
    int leftOffset = 0;
    int shadowOffsetDip = g_settings.dayCustomShadowOffset;
    COLORREF textColor = g_settings.dayTextColor;
    COLORREF shadowColor = g_settings.dayShadowColor;
    const COLORREF* letterColors = g_settings.weekdayLetterColors;
    const COLORREF* letterShadowColors = g_settings.weekdayLetterShadowColors;

    if (row == Row::Date) {
      text = &date;
      font = overlay->dateFont;
      fallbackFont = g_settings.dateFontFamily == L"Anurati"
          ? overlay->dateFallbackFont : nullptr;
      fontSize = DateDisplayFontSize();
      spacing = Scale(g_settings.dateLetterSpacing, overlay->dpi);
      leftOffset = g_settings.dateLeftOffset;
      shadowOffsetDip = g_settings.dateCustomShadowOffset;
      shadowColor = g_settings.dateShadowColor;
      textColor = g_settings.dateTextColor;
      letterColors = g_settings.dateLetterColors;
      letterShadowColors = g_settings.dateLetterShadowColors;
    } else if (row == Row::Time) {
      text = &time;
      font = overlay->timeFont;
      fallbackFont = g_settings.timeFontFamily == L"Anurati"
          ? overlay->timeFallbackFont : nullptr;
      fontSize = TimeDisplayFontSize();
      spacing = Scale(g_settings.timeLetterSpacing, overlay->dpi);
      leftOffset = g_settings.timeLeftOffset;
      shadowOffsetDip = g_settings.timeCustomShadowOffset;
      shadowColor = g_settings.timeShadowColor;
      textColor = g_settings.timeTextColor;
      letterColors = g_settings.timeLetterColors;
      letterShadowColors = g_settings.timeLetterShadowColors;
    }

    const int centerX = xCenter - Scale(leftOffset, overlay->dpi);
    const int shadowOffset = Scale(shadowOffsetDip, overlay->dpi);
    const int kerning = row == Row::Day ? WeekdayKerning(overlay->dpi) : 0;
    if (shadowOffset > 0) {
      DrawHeaderCharacters(memory, *text, y + shadowOffset, font, fallbackFont,
          spacing, kerning, letterShadowColors, shadowColor, centerX + shadowOffset);
    }
    DrawHeaderCharacters(memory, *text, y, font, fallbackFont, spacing,
        kerning, letterColors, textColor, centerX);

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

static void UpdateClock(bool force = false) {
  SYSTEMTIME now{};
  GetLocalTime(&now);
  const int minute = now.wHour * 60 + now.wMinute;
  if (!force && g_currentDay == now.wDay && g_currentMinute == minute) return;
  g_currentDay = now.wDay;
  g_currentMinute = minute;
  for (Overlay* overlay : g_overlays) Paint(overlay);
}

static void DestroyOverlays() {
  for (Overlay* overlay : g_overlays) {
    if (overlay->hwnd) DestroyWindow(overlay->hwnd);
    DeleteFonts(overlay);
    delete overlay;
  }
  g_overlays.clear();
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
      if (g_settings.lockWidgetPosition) return HTTRANSPARENT;
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
        MONITORINFO info{sizeof(info)};
        HMONITOR monitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
        RECT windowRect{};
        if (GetMonitorInfoW(monitor, &info) && GetWindowRect(hwnd, &windowRect)) {
          int centerX = info.rcMonitor.left + (info.rcMonitor.right - info.rcMonitor.left) / 2;
          int centerY = info.rcMonitor.top + (info.rcMonitor.bottom - info.rcMonitor.top) / 2;
          int dpi = static_cast<int>(GetMonitorDpi(monitor, nullptr));
          g_settings.offsetX = MulDiv((windowRect.left + windowRect.right) / 2 - centerX, 96, dpi);
          g_settings.offsetY = MulDiv((windowRect.top + windowRect.bottom) / 2 - centerY, 96, dpi);

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
            int otherCenterX = otherInfo.rcMonitor.left +
              (otherInfo.rcMonitor.right - otherInfo.rcMonitor.left) / 2;
            int otherCenterY = otherInfo.rcMonitor.top +
              (otherInfo.rcMonitor.bottom - otherInfo.rcMonitor.top) / 2;
            int otherX = otherCenterX + Scale(g_settings.offsetX, other->dpi) -
              (otherRect.right - otherRect.left) / 2;
            int otherY = otherCenterY + Scale(g_settings.offsetY, other->dpi) -
              (otherRect.bottom - otherRect.top) / 2;
            SetWindowPos(other->hwnd, nullptr, otherX, otherY, 0, 0,
              SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
          }
            g_syncingPositions = false;
        }
      }
      break;
    }
    case WM_EXITSIZEMOVE:
      g_inMoveLoop = false;
      if (overlay) {
        Wh_SetIntValue(L"offsetX", g_settings.offsetX);
        Wh_SetIntValue(L"offsetY", g_settings.offsetY);
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
    case WM_SETTINGCHANGE:
      if (overlay) Paint(overlay);
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

static BOOL CALLBACK CreateOverlay(HMONITOR monitor, HDC dc, LPRECT, LPARAM) {
  MONITORINFO info{sizeof(info)};
  if (!GetMonitorInfoW(monitor, &info)) return TRUE;
  auto overlay = new Overlay;
  overlay->dpi = GetMonitorDpi(monitor, dc);
  CreateFonts(overlay);
  HDC measureDc = CreateCompatibleDC(dc);
  if (!measureDc) {
    DeleteFonts(overlay);
    delete overlay;
    return TRUE;
  }
  int dayWidth = 0;
  for (const wchar_t* name : {L"SUNDAY", L"MONDAY", L"TUESDAY", L"WEDNESDAY",
                L"THURSDAY", L"FRIDAY", L"SATURDAY"}) {
    dayWidth = std::max(dayWidth, MeasureWeekday(measureDc, name, overlay,
              Scale(g_settings.dayLetterSpacing, overlay->dpi)));
  }
  int dateWidth = MeasureText(measureDc, L"31 SEPTEMBER 8888", overlay->dateFont,
          Scale(g_settings.dateLetterSpacing, overlay->dpi),
          g_settings.dateFontFamily == L"Anurati"
            ? overlay->dateFallbackFont : nullptr);
  int timeWidth = MeasureText(measureDc, g_settings.timeFormat24 ? L"23:59"
                   : L"12:59 AM", overlay->timeFont,
          Scale(g_settings.timeLetterSpacing, overlay->dpi),
          g_settings.timeFontFamily == L"Anurati"
            ? overlay->timeFallbackFont : nullptr);
  DeleteDC(measureDc);
  int dateOffset = std::abs(Scale(g_settings.dateLeftOffset, overlay->dpi));
  int timeOffset = std::abs(Scale(g_settings.timeLeftOffset, overlay->dpi));
  int width = std::max({dayWidth, dateWidth + 2 * dateOffset,
              timeWidth + 2 * timeOffset}) + Scale(48, overlay->dpi);
  const int visibleRows = static_cast<int>(g_settings.showWeekday) +
      static_cast<int>(g_settings.showDate) +
      static_cast<int>(g_settings.showTime);
  int heightDip = 40;
  if (g_settings.showWeekday) heightDip += DayDisplayFontSize();
  if (g_settings.showDate) heightDip += DateDisplayFontSize();
  if (g_settings.showTime) heightDip += TimeDisplayFontSize();
  if (visibleRows > 1) {
    heightDip += g_settings.gapDayToDate;
    if (visibleRows > 2) heightDip += g_settings.gapDateToTime;
  }
  int height = Scale(heightDip, overlay->dpi);
  int centerX = info.rcMonitor.left + (info.rcMonitor.right - info.rcMonitor.left) / 2;
  int centerY = info.rcMonitor.top + (info.rcMonitor.bottom - info.rcMonitor.top) / 2;
  int x = centerX + Scale(g_settings.offsetX, overlay->dpi) - width / 2;
  int y = centerY + Scale(g_settings.offsetY, overlay->dpi) - height / 2;
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
  return TRUE;
}

static void RebuildOverlays() {
  if (g_rebuilding) return;
  g_rebuilding = true;
  DestroyOverlays();
  EnumDisplayMonitors(nullptr, nullptr, CreateOverlay, 0);
  SYSTEMTIME now{};
  GetLocalTime(&now);
  g_currentDay = now.wDay;
  g_currentMinute = now.wHour * 60 + now.wMinute;
  g_rebuilding = false;
}
static void LoadSettings() {
  g_settings.dayFontFamily = LoadStringSetting(L"dayFontFamily", L"Anurati");
  g_settings.timeFontFamily = LoadStringSetting(L"timeFontFamily", L"Quicksand");
  g_settings.dateFontFamily = LoadStringSetting(L"dateFontFamily", L"Quicksand");
  g_settings.mainHeading = LoadStringSetting(L"mainHeading", L"day");
  if (g_settings.mainHeading != L"day" &&
      g_settings.mainHeading != L"date" &&
      g_settings.mainHeading != L"time") {
    g_settings.mainHeading = L"day";
  }
  g_settings.dayBold = Wh_GetIntSetting(L"dayBold") != 0;
  g_settings.dayItalic = Wh_GetIntSetting(L"dayItalic") != 0;
  g_settings.timeBold = Wh_GetIntSetting(L"timeBold") != 0;
  g_settings.timeItalic = Wh_GetIntSetting(L"timeItalic") != 0;
  g_settings.dateBold = Wh_GetIntSetting(L"dateBold") != 0;
  g_settings.dateItalic = Wh_GetIntSetting(L"dateItalic") != 0;
  g_settings.dayFontSize = std::clamp(Wh_GetIntSetting(L"dayFontSize"), 12, 300);
  g_settings.dayLetterSpacing = std::clamp(Wh_GetIntSetting(L"dayLetterSpacing"), 0, 200);
  g_settings.dayCustomShadowOffset = LoadShadowOffset(
      L"customLetterStyles.weekday.shadowOffset", 2);
  g_settings.timeFontSize = std::clamp(Wh_GetIntSetting(L"timeFontSize"), 8, 100);
  g_settings.timeLetterSpacing = std::clamp(Wh_GetIntSetting(L"timeLetterSpacing"), 0, 50);
  g_settings.timeCustomShadowOffset = LoadShadowOffset(
      L"customLetterStyles.time.shadowOffset", 2);
  g_settings.dateFontSize = std::clamp(Wh_GetIntSetting(L"dateFontSize"), 8, 100);
  g_settings.dateLetterSpacing = std::clamp(Wh_GetIntSetting(L"dateLetterSpacing"), 0, 50);
  g_settings.dateCustomShadowOffset = LoadShadowOffset(
      L"customLetterStyles.date.shadowOffset", 2);
  g_settings.gapDayToDate = std::clamp(Wh_GetIntSetting(L"gapDayToDate"), 0, 200);
  g_settings.gapDateToTime = std::clamp(Wh_GetIntSetting(L"gapDateToTime"), 0, 200);
  g_settings.dateLeftOffset = Wh_GetIntSetting(L"dateLeftOffset");
  g_settings.timeLeftOffset = Wh_GetIntSetting(L"timeLeftOffset");
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
  if (configuredOffsetX != savedOffsetXSetting ||
      configuredOffsetY != savedOffsetYSetting) {
    Wh_DeleteValue(L"offsetX");
    Wh_DeleteValue(L"offsetY");
    Wh_SetIntValue(L"offsetXSetting", configuredOffsetX);
    Wh_SetIntValue(L"offsetYSetting", configuredOffsetY);
  }
  g_positionSettingX = configuredOffsetX;
  g_positionSettingY = configuredOffsetY;
  g_settings.offsetX = Wh_GetIntValue(L"offsetX", configuredOffsetX);
  g_settings.offsetY = Wh_GetIntValue(L"offsetY", configuredOffsetY);
  g_settings.timeFormat24 = Wh_GetIntSetting(L"timeFormat24") != 0;
  g_settings.showWeekday = Wh_GetIntSetting(L"showWeekday") != 0;
  g_settings.showDate = Wh_GetIntSetting(L"showDate") != 0;
  g_settings.showTime = Wh_GetIntSetting(L"showTime") != 0;
  g_settings.dayShadowColor = LoadColorSetting(L"dayShadowColor", RGB(64, 64, 64));
  g_settings.timeShadowColor = LoadColorSetting(L"timeShadowColor", RGB(64, 64, 64));
  g_settings.dateShadowColor = LoadColorSetting(L"dateShadowColor", RGB(64, 64, 64));
  g_settings.dayTextColor = LoadColorSetting(L"dayTextColor", RGB(255, 255, 255));
  g_settings.dateTextColor = LoadColorSetting(L"dateTextColor", RGB(255, 255, 255));
  g_settings.timeTextColor = LoadColorSetting(L"timeTextColor", RGB(255, 255, 255));
    std::fill_n(g_settings.weekdayLetterColors, kMaxHeaderCharacters,
      g_settings.dayTextColor);
    std::fill_n(g_settings.weekdayLetterShadowColors, kMaxHeaderCharacters,
      g_settings.dayShadowColor);
    std::fill_n(g_settings.dateLetterColors, kMaxHeaderCharacters,
      g_settings.dateTextColor);
    std::fill_n(g_settings.dateLetterShadowColors, kMaxHeaderCharacters,
      g_settings.dateShadowColor);
    std::fill_n(g_settings.timeLetterColors, kMaxHeaderCharacters,
      g_settings.timeTextColor);
    std::fill_n(g_settings.timeLetterShadowColors, kMaxHeaderCharacters,
      g_settings.timeShadowColor);

    for (size_t i = 0; i < kWeekdayCharacterCount; ++i) {
    wchar_t settingName[64];
    wchar_t legacyName[32];
    swprintf_s(legacyName, L"weekdayLetterColor%u",
      static_cast<unsigned>(i + 1));
    g_settings.weekdayLetterColors[i] =
      LoadColorSetting(legacyName, g_settings.dayTextColor);
    swprintf_s(settingName, L"customLetterStyles.weekday.shadow%u",
      static_cast<unsigned>(i + 1));
    g_settings.weekdayLetterShadowColors[i] =
      LoadColorSetting(settingName, g_settings.dayShadowColor);
    }
    for (size_t i = 0; i < kDateCharacterCount; ++i) {
    wchar_t settingName[64];
    swprintf_s(settingName, L"customLetterStyles.date.color%u",
      static_cast<unsigned>(i + 1));
    g_settings.dateLetterColors[i] =
      LoadColorSetting(settingName, g_settings.dateTextColor);
    swprintf_s(settingName, L"customLetterStyles.date.shadow%u",
      static_cast<unsigned>(i + 1));
    g_settings.dateLetterShadowColors[i] =
      LoadColorSetting(settingName, g_settings.dateShadowColor);
    }
    for (size_t i = 0; i < kTimeCharacterCount; ++i) {
    wchar_t settingName[64];
    swprintf_s(settingName, L"customLetterStyles.time.color%u",
      static_cast<unsigned>(i + 1));
    g_settings.timeLetterColors[i] =
      LoadColorSetting(settingName, g_settings.timeTextColor);
    swprintf_s(settingName, L"customLetterStyles.time.shadow%u",
      static_cast<unsigned>(i + 1));
    g_settings.timeLetterShadowColors[i] =
      LoadColorSetting(settingName, g_settings.timeShadowColor);
  }
}

static bool SameAppearanceSettings(const Settings& first, const Settings& second) {
  return first.dayFontFamily == second.dayFontFamily &&
      first.timeFontFamily == second.timeFontFamily &&
      first.dateFontFamily == second.dateFontFamily &&
      first.mainHeading == second.mainHeading &&
      first.dayFontSize == second.dayFontSize &&
      first.dayLetterSpacing == second.dayLetterSpacing &&
      first.dayCustomShadowOffset == second.dayCustomShadowOffset &&
      first.dayBold == second.dayBold &&
      first.dayItalic == second.dayItalic &&
      first.timeFontSize == second.timeFontSize &&
      first.timeLetterSpacing == second.timeLetterSpacing &&
      first.timeBold == second.timeBold &&
      first.timeItalic == second.timeItalic &&
      first.timeCustomShadowOffset == second.timeCustomShadowOffset &&
      first.dateFontSize == second.dateFontSize &&
      first.dateLetterSpacing == second.dateLetterSpacing &&
      first.dateBold == second.dateBold &&
      first.dateItalic == second.dateItalic &&
      first.dateCustomShadowOffset == second.dateCustomShadowOffset &&
      first.gapDayToDate == second.gapDayToDate &&
      first.gapDateToTime == second.gapDateToTime &&
      first.dateLeftOffset == second.dateLeftOffset &&
      first.timeLeftOffset == second.timeLeftOffset &&
      first.timeFormat24 == second.timeFormat24 &&
      first.showWeekday == second.showWeekday &&
      first.showDate == second.showDate &&
      first.showTime == second.showTime &&
      first.dayTextColor == second.dayTextColor &&
      first.dayShadowColor == second.dayShadowColor &&
      first.timeShadowColor == second.timeShadowColor &&
      first.dateShadowColor == second.dateShadowColor &&
      first.dateTextColor == second.dateTextColor &&
      first.timeTextColor == second.timeTextColor &&
        std::equal(std::begin(first.weekdayLetterColors),
          std::end(first.weekdayLetterColors),
          std::begin(second.weekdayLetterColors)) &&
        std::equal(std::begin(first.weekdayLetterShadowColors),
          std::end(first.weekdayLetterShadowColors),
          std::begin(second.weekdayLetterShadowColors)) &&
        std::equal(std::begin(first.dateLetterColors),
          std::end(first.dateLetterColors),
          std::begin(second.dateLetterColors)) &&
        std::equal(std::begin(first.dateLetterShadowColors),
          std::end(first.dateLetterShadowColors),
          std::begin(second.dateLetterShadowColors)) &&
        std::equal(std::begin(first.timeLetterColors),
          std::end(first.timeLetterColors),
          std::begin(second.timeLetterColors)) &&
        std::equal(std::begin(first.timeLetterShadowColors),
          std::end(first.timeLetterShadowColors),
          std::begin(second.timeLetterShadowColors));
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
      return 0;
    case WM_POWERBROADCAST:
      if (wp == PBT_APMRESUMEAUTOMATIC || wp == PBT_APMRESUMESUSPEND) {
        UpdateClock(true);
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
    if (g_uiStartOk) SetTimer(g_controller.load(), kWatchTimerId,
                  kWatchIntervalMs, nullptr);
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
