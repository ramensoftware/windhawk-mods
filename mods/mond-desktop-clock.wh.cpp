// ==WindhawkMod==
// @id mond-desktop-clock
// @name Mond Desktop Clock
// @description A desktop clock inspired by the Mond Rainmeter skin, with custom styling and draggable placement.
// @version 3.4
// @author Elmidin Mahmud
// @github https://github.com/elmidin
// @include windhawk.exe
// @compilerOptions -lgdi32 -luser32 -lshell32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Mond Desktop Clock

A desktop clock overlay inspired by the [Mond Rainmeter skin by HipHopium](https://www.deviantart.com/hiphopium). It runs in a dedicated tool process, not in Explorer.

The clock shows the weekday, date, and time in three independently styled rows. Drag it to position it; enable position locking to prevent accidental movement.

![Mond Desktop Clock on the desktop](https://i.imgur.com/dj71eKl.png)

- Drag-and-drop placement with a saved position shared across monitors
- Independent font size, letter spacing, and horizontal alignment for each row
- Anurati weekday lettering with per-letter colour controls
- Quicksand bold date and time text, with separate colour settings
- 12-hour or 24-hour time and an optional date row

Fonts must be installed in Windows. Missing fonts are silently substituted by Windows.

- [Quicksand](https://fonts.google.com/specimen/Quicksand)
- [Anurati](https://www.dafont.com/anurati.font)

Date and time can use separate text colours. Weekday letter colours are set by position (1-9); blank overrides use the Default text colour. Colours accept #RRGGBB or #AARRGGBB; the alpha component in the latter form is ignored.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- dayFontSize: 72
  $name: Day font size
  $description: Anurati weekday size in DIPs (scaled for each monitor's DPI).

- dayLetterSpacing: 40
  $name: Day letter spacing
  $description: Extra spacing between weekday letters in DIPs.

- timeFontSize: 18
  $name: Time font size

- timeLetterSpacing: 4
  $name: Time letter spacing
  $description: Extra spacing between time characters in DIPs.

- dateFontSize: 18
  $name: Date font size

- dateLetterSpacing: 4
  $name: Date letter spacing
  $description: Extra spacing between date characters in DIPs.

- gapDayToDate: 42
  $name: Vertical Gap (Day to Date)
  $description: Vertical padding distance in DIPs between the weekday row and the date row.

- gapDateToTime: 25
  $name: Vertical Gap (Date to Time)
  $description: Vertical padding distance in DIPs between the date row and the time row.

- dateLeftOffset: 0
  $name: Date horizontal offset
  $description: Moves the date row in DIPs. Positive shifts left.

- timeLeftOffset: 0
  $name: Time horizontal offset
  $description: Moves the time row in DIPs, independently of the date. Positive shifts left.

- lockWidgetPosition: false
  $name: Lock widget position
  $description: Turn ON to prevent accidental dragging. Turn OFF to reposition the clock freely using the mouse.

- offsetX: 0
  $name: Horizontal position offset
  $description: Initial horizontal offset from each monitor's centre, in DIPs. Dragged position is saved and shared by all monitors.

- offsetY: 0
  $name: Vertical position offset
  $description: Initial vertical offset from each monitor's centre, in DIPs. Dragged position is saved and shared by all monitors.

- timeFormat24: false
  $name: Use 24-hour time
  $description: Off uses 12-hour time with AM/PM (the original look).

- showDate: true
  $name: Show date

- dateTextColor: ""
  $name: Date text colour
  $description: Hex colour for the date row. Leave blank to use Default text colour.

- timeTextColor: ""
  $name: Time text colour
  $description: Hex colour for the time row. Leave blank to use Default text colour.

- weekdayLetterColor1: ""
  $name: Weekday letter 1 colour
  $description: Hex colour for the first weekday letter. Leave blank to use Default text colour.

- weekdayLetterColor2: ""
  $name: Weekday letter 2 colour
  $description: Hex colour for the second weekday letter. Leave blank to use Default text colour.

- weekdayLetterColor3: ""
  $name: Weekday letter 3 colour
  $description: Hex colour for the third weekday letter. Leave blank to use Default text colour.

- weekdayLetterColor4: ""
  $name: Weekday letter 4 colour
  $description: Hex colour for the fourth weekday letter. Leave blank to use Default text colour.

- weekdayLetterColor5: ""
  $name: Weekday letter 5 colour
  $description: Hex colour for the fifth weekday letter. Leave blank to use Default text colour.

- weekdayLetterColor6: ""
  $name: Weekday letter 6 colour
  $description: Hex colour for the sixth weekday letter. Leave blank to use Default text colour.

- weekdayLetterColor7: ""
  $name: Weekday letter 7 colour
  $description: Hex colour for the seventh weekday letter. Leave blank to use Default text colour.

- weekdayLetterColor8: ""
  $name: Weekday letter 8 colour
  $description: Hex colour for the eighth weekday letter. Leave blank to use Default text colour.

- weekdayLetterColor9: ""
  $name: Weekday letter 9 colour
  $description: Hex colour for the ninth weekday letter. Leave blank to use Default text colour.

- textColor: "#FFFFFFFF"
  $name: Default text colour
  $description: Fallback text colour in hex format, for example #FFFFFFFF.
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
using GetDpiForMonitor_t = HRESULT(WINAPI*)(HMONITOR, int, UINT*, UINT*);

struct Overlay {
  HWND hwnd = nullptr;
  UINT dpi = 96;
  HFONT dayFont = nullptr;
  HFONT timeFont = nullptr;
  HFONT dateFont = nullptr;
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
static HMODULE g_shcore = nullptr;
static GetDpiForMonitor_t g_getDpiForMonitor = nullptr;

struct Settings {
  int dayFontSize;
  int dayLetterSpacing;
  int timeFontSize;
  int timeLetterSpacing;
  int dateFontSize;
  int dateLetterSpacing;
  int gapDayToDate;
  int gapDateToTime;
  int dateLeftOffset;
  int timeLeftOffset;
  bool lockWidgetPosition;
  int offsetX;
  int offsetY;
  bool timeFormat24;
  bool showDate;
  COLORREF textColor;
  COLORREF dateTextColor;
  COLORREF timeTextColor;
  COLORREF weekdayLetterColors[9];
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

static void DeleteFonts(Overlay* overlay) {
  if (overlay->dayFont) DeleteObject(overlay->dayFont);
  if (overlay->timeFont) DeleteObject(overlay->timeFont);
  if (overlay->dateFont) DeleteObject(overlay->dateFont);
  overlay->dayFont = overlay->timeFont = overlay->dateFont = nullptr;
}

static void CreateFonts(Overlay* overlay) {
  DeleteFonts(overlay);
  int daySize = MulDiv(g_settings.dayFontSize, overlay->dpi, 96);
  int timeSize = MulDiv(g_settings.timeFontSize, overlay->dpi, 96);
  int dateSize = MulDiv(g_settings.dateFontSize, overlay->dpi, 96);
  overlay->dayFont = CreateFontW(-daySize, 0, 0, 0, FW_NORMAL,
        FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
    CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH, L"Anurati");
        
  overlay->timeFont = CreateFontW(-timeSize, 0, 0, 0, FW_BOLD,
        FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
    CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH, L"Quicksand");

  overlay->dateFont = CreateFontW(-dateSize, 0, 0, 0, FW_BOLD,
        FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
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
        swprintf_s(b, L"-  %02u:%02u  -", st.wHour, st.wMinute);
    } else {
        unsigned h = st.wHour % 12;
        if (!h) h = 12;
        swprintf_s(b, L"-  %u:%02u %s  -", h, st.wMinute,
                   st.wHour >= 12 ? L"PM" : L"AM");
    }
    return b;
}

static std::wstring CurrentDate() {
    SYSTEMTIME st{};
    GetLocalTime(&st);
    wchar_t b[64]{};
    static const wchar_t* months[] = {
        L"JANUARY", L"FEBRUARY", L"MARCH", L"APRIL", L"MAY", L"JUNE",
        L"JULY", L"AUGUST", L"SEPTEMBER", L"OCTOBER", L"NOVEMBER", L"DECEMBER"};
    const unsigned month = std::clamp<unsigned>(st.wMonth, 1, 12);
    swprintf_s(b, L"%02u  %s,  %04u.", st.wDay, months[month - 1], st.wYear);
    return b;
}

static void DrawCenteredSpaced(HDC dc, const std::wstring& text, int y, HFONT font,
                              int spacing, COLORREF color, int centreX) {
    HGDIOBJ oldFont = SelectObject(dc, font);
    SetTextColor(dc, color);
    SetBkMode(dc, TRANSPARENT);
    SetTextCharacterExtra(dc, spacing);
    
    SIZE sz{};
    GetTextExtentPoint32W(dc, text.c_str(), (int)text.size(), &sz);
    if (!text.empty()) sz.cx -= spacing;
    const int x = centreX - sz.cx / 2;
    TextOutW(dc, x, y, text.c_str(), (int)text.size());
    
    SetTextCharacterExtra(dc, 0);
    SelectObject(dc, oldFont);
}

static void DrawWeekdayWithKerning(HDC dc, const std::wstring& text, int y, HFONT font,
                                   int spacing, int kerning, const COLORREF* colors,
                                   int centreX) {
    HGDIOBJ oldFont = SelectObject(dc, font);
    SetBkMode(dc, TRANSPARENT);
    
    std::vector<int> charWidths(text.size(), 0);
    int totalWidth = 0;
    for (size_t i = 0; i < text.size(); ++i) {
        wchar_t ch[2] = { text[i], L'\0' };
        SIZE sz{};
        GetTextExtentPoint32W(dc, ch, 1, &sz);
        charWidths[i] = sz.cx;
        totalWidth += sz.cx;
    }
    
    if (text.size() > 0) {
        totalWidth += spacing * ((int)text.size() - 1);
        if (text.find(L"DA") != std::wstring::npos) {
            totalWidth -= kerning;
        }
    }
    
    int currentX = centreX - totalWidth / 2;
    
    for (size_t i = 0; i < text.size(); ++i) {
        wchar_t ch[2] = { text[i], L'\0' };
        SetTextColor(dc, colors[i]);
        
        // Visual Kerning Adjuster: If rendering the letter 'A' right after 'D', pull it closer to balance whitespace weights
        if (i > 0 && text[i] == L'A' && text[i-1] == L'D') {
            currentX -= kerning;
        }
        
        TextOutW(dc, currentX, y, ch, 1);
        currentX += charWidths[i] + spacing;
    }
    
    SelectObject(dc, oldFont);
}

static int MeasureText(HDC dc, const std::wstring& text, HFONT font, int spacing) {
  HGDIOBJ oldFont = SelectObject(dc, font);
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
  if (text.find(L"DA") != std::wstring::npos) width -= Scale(14, overlay->dpi);
  return width;
}

static void Paint(Overlay* overlay) {
  HWND hwnd = overlay->hwnd;
  RECT client{};
  if (!GetClientRect(hwnd, &client)) return;
  int width = client.right, height = client.bottom;
  if (width <= 0 || height <= 0) return;
  HDC screen = GetDC(hwnd);
  HDC memory = CreateCompatibleDC(screen);
  HBITMAP bitmap = CreateCompatibleBitmap(screen, width, height);
  if (!screen || !memory || !bitmap) {
    if (bitmap) DeleteObject(bitmap);
    if (memory) DeleteDC(memory);
    if (screen) ReleaseDC(hwnd, screen);
    return;
  }
  HGDIOBJ oldBitmap = SelectObject(memory, bitmap);
  const COLORREF key = RGB(1, 2, 3);
  HBRUSH background = CreateSolidBrush(key);
  FillRect(memory, &client, background);
  DeleteObject(background);

  int daySpacing = Scale(g_settings.dayLetterSpacing, overlay->dpi);
  int dateSpacing = Scale(g_settings.dateLetterSpacing, overlay->dpi);
  int timeSpacing = Scale(g_settings.timeLetterSpacing, overlay->dpi);
  const std::wstring day = CurrentWeekday();
  const std::wstring date = CurrentDate();
  const std::wstring time = CurrentTime();
  int xCenter = width / 2;
  int y = Scale(10, overlay->dpi);
  DrawWeekdayWithKerning(memory, day, y, overlay->dayFont, daySpacing,
               Scale(14, overlay->dpi),
               g_settings.weekdayLetterColors, xCenter);
  y += Scale(g_settings.dayFontSize + g_settings.gapDayToDate, overlay->dpi);
  int dateCenter = xCenter - Scale(g_settings.dateLeftOffset, overlay->dpi);
  int timeCenter = xCenter - Scale(g_settings.timeLeftOffset, overlay->dpi);
  if (g_settings.showDate) {
    DrawCenteredSpaced(memory, date, y, overlay->dateFont, dateSpacing,
               g_settings.dateTextColor, dateCenter);
    y += Scale(g_settings.dateFontSize + g_settings.gapDateToTime,
           overlay->dpi);
  }
  DrawCenteredSpaced(memory, time, y, overlay->timeFont, timeSpacing,
             g_settings.timeTextColor, timeCenter);
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
          !g_rebuilding && !g_syncingPositions) {
        MONITORINFO info{sizeof(info)};
        HMONITOR monitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
        RECT windowRect{};
        if (GetMonitorInfoW(monitor, &info) && GetWindowRect(hwnd, &windowRect)) {
          int centerX = info.rcMonitor.left + (info.rcMonitor.right - info.rcMonitor.left) / 2;
          int centerY = info.rcMonitor.top + (info.rcMonitor.bottom - info.rcMonitor.top) / 2;
          int dpi = static_cast<int>(GetMonitorDpi(monitor, nullptr));
          g_settings.offsetX = MulDiv((windowRect.left + windowRect.right) / 2 - centerX, 96, dpi);
          g_settings.offsetY = MulDiv((windowRect.top + windowRect.bottom) / 2 - centerY, 96, dpi);

          if (g_inMoveLoop) {
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
      }
      break;
    }
    case WM_EXITSIZEMOVE:
      g_inMoveLoop = false;
      if (overlay) {
        Wh_SetIntValue(L"offsetX", g_settings.offsetX);
        Wh_SetIntValue(L"offsetY", g_settings.offsetY);
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
  int dateWidth = MeasureText(measureDc, L"31  SEPTEMBER,  8888.", overlay->dateFont,
                Scale(g_settings.dateLetterSpacing, overlay->dpi));
  int timeWidth = MeasureText(measureDc, g_settings.timeFormat24 ? L"-  23:59  -"
                   : L"-  12:59 PM  -", overlay->timeFont,
                Scale(g_settings.timeLetterSpacing, overlay->dpi));
  DeleteDC(measureDc);
  int dateOffset = std::abs(Scale(g_settings.dateLeftOffset, overlay->dpi));
  int timeOffset = std::abs(Scale(g_settings.timeLeftOffset, overlay->dpi));
  int width = std::max({dayWidth, dateWidth + 2 * dateOffset,
              timeWidth + 2 * timeOffset}) + Scale(48, overlay->dpi);
  int height = Scale(20 + g_settings.dayFontSize + g_settings.gapDayToDate +
             (g_settings.showDate ? g_settings.dateFontSize + g_settings.gapDateToTime : 0) +
             g_settings.timeFontSize + 20, overlay->dpi);
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
  g_settings.dayFontSize = std::clamp(Wh_GetIntSetting(L"dayFontSize"), 12, 300);
  g_settings.dayLetterSpacing = std::clamp(Wh_GetIntSetting(L"dayLetterSpacing"), 0, 200);
  g_settings.timeFontSize = std::clamp(Wh_GetIntSetting(L"timeFontSize"), 8, 100);
  g_settings.timeLetterSpacing = std::clamp(Wh_GetIntSetting(L"timeLetterSpacing"), 0, 50);
  g_settings.dateFontSize = std::clamp(Wh_GetIntSetting(L"dateFontSize"), 8, 100);
  g_settings.dateLetterSpacing = std::clamp(Wh_GetIntSetting(L"dateLetterSpacing"), 0, 50);
  g_settings.gapDayToDate = std::clamp(Wh_GetIntSetting(L"gapDayToDate"), 0, 200);
  g_settings.gapDateToTime = std::clamp(Wh_GetIntSetting(L"gapDateToTime"), 0, 200);
  g_settings.dateLeftOffset = Wh_GetIntSetting(L"dateLeftOffset");
  g_settings.timeLeftOffset = Wh_GetIntSetting(L"timeLeftOffset");
  g_settings.lockWidgetPosition = Wh_GetIntSetting(L"lockWidgetPosition") != 0;
  g_settings.offsetX = Wh_GetIntValue(L"offsetX", Wh_GetIntSetting(L"offsetX"));
  g_settings.offsetY = Wh_GetIntValue(L"offsetY", Wh_GetIntSetting(L"offsetY"));
  g_settings.timeFormat24 = Wh_GetIntSetting(L"timeFormat24") != 0;
  g_settings.showDate = Wh_GetIntSetting(L"showDate") != 0;
  PCWSTR color = Wh_GetStringSetting(L"textColor");
  g_settings.textColor = ParseColor(color);
  Wh_FreeStringSetting(color);
  g_settings.dateTextColor = LoadColorSetting(L"dateTextColor", g_settings.textColor);
  g_settings.timeTextColor = LoadColorSetting(L"timeTextColor", g_settings.textColor);
  for (int i = 0; i < ARRAYSIZE(g_settings.weekdayLetterColors); i++) {
    wchar_t settingName[32];
    swprintf_s(settingName, L"weekdayLetterColor%d", i + 1);
    g_settings.weekdayLetterColors[i] =
        LoadColorSetting(settingName, g_settings.textColor);
  }
}

static bool SameAppearanceSettings(const Settings& first, const Settings& second) {
  return first.dayFontSize == second.dayFontSize &&
      first.dayLetterSpacing == second.dayLetterSpacing &&
      first.timeFontSize == second.timeFontSize &&
      first.timeLetterSpacing == second.timeLetterSpacing &&
      first.dateFontSize == second.dateFontSize &&
      first.dateLetterSpacing == second.dateLetterSpacing &&
      first.gapDayToDate == second.gapDayToDate &&
      first.gapDateToTime == second.gapDateToTime &&
      first.dateLeftOffset == second.dateLeftOffset &&
      first.timeLeftOffset == second.timeLeftOffset &&
      first.timeFormat24 == second.timeFormat24 &&
      first.showDate == second.showDate &&
      first.textColor == second.textColor &&
      first.dateTextColor == second.dateTextColor &&
      first.timeTextColor == second.timeTextColor &&
      std::equal(std::begin(first.weekdayLetterColors),
          std::end(first.weekdayLetterColors),
          std::begin(second.weekdayLetterColors));
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
