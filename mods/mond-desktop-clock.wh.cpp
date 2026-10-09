// ==WindhawkMod==
// @id mond-desktop-clock
// @name Mond Desktop Clock
// @description A Mond-inspired desktop clock with independent centering, drag-and-drop movement, and custom font kerning. Shoutout to the original Creator of MOND for Rainmeter! 
// @version 3.2
// @author Elmidin Mahmud
// @include explorer.exe
// @architecture x86-64
// @compilerOptions -lgdi32 -luser32 -ldwmapi
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Mond Desktop Clock

A highly customized desktop clock widget overlay inspired by the Mond Rainmeter theme layout.

- Interactive drag-and-drop placement (Left-click and hold text to move anywhere)
- Position locking setting switch parameter
- Large uppercase weekday in Anurati with custom visual kerning overrides
- Mond-style double whitespace tracking alignment layers
- Quicksand thin typeface configuration profiles
- Fully independent horizontal shift parameters for date vs time fields

Fonts must be installed in Windows:
  Anurati / Anurati Regular
  Quicksand
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- dayFontSize: 72
  $name: Day font size
  $description: Anurati weekday size in pixels.

- dayLetterSpacing: 40
  $name: Day letter spacing
  $description: Extra spacing between weekday letters, in pixels. Match original Mond look.

- timeFontSize: 18
  $name: Time font size

- timeLetterSpacing: 4
  $name: Time letter spacing
  $description: Extra spacing between time characters.

- dateFontSize: 18
  $name: Date font size

- dateLetterSpacing: 4
  $name: Date letter spacing
  $description: Extra spacing between date characters.

- gapDayToDate: 42
  $name: Vertical Gap (Day to Date)
  $description: Vertical padding distance between the weekday row and the date row.

- gapDateToTime: 25
  $name: Vertical Gap (Date to Time)
  $description: Vertical padding distance between the date row and the time row.

- dateLeftOffset: -18
  $name: Date Left Shift (pixels)
  $description: Moves the date row left or right to line it up under the day name. Positive shifts left.

- timeLeftOffset: -8
  $name: Time Left Shift (pixels)
  $description: Moves the time row completely independently left or right. Positive shifts left.

- lockWidgetPosition: false
  $name: Lock widget position
  $description: Turn ON to prevent accidental dragging. Turn OFF to reposition the clock freely using the mouse.

- offsetX: 0
  $name: Horizontal position offset (pixels)
  $description: Custom pixel coordinate placement. Updates automatically when dragging.

- offsetY: 0
  $name: Vertical position offset (pixels)
  $description: Custom pixel coordinate placement. Updates automatically when dragging.

- timeFormat24: false
  $name: Time format
  $description: On uses 24-hour time; off uses 12-hour time with AM/PM (Original look uses 12-hour).

- showDate: true
  $name: Show date

- textColor: "#FFFFFFFF"
  $name: Text colour
  $description: Text colour in hex format, for example #FFFFFFFF.
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <windhawk_api.h>
#include <string>
#include <vector>
#include <algorithm>
#include <cwchar>

static constexpr wchar_t kWindowClass[] = L"WindhawkMondDesktopClock";
static constexpr UINT_PTR kTimerId = 1;
static std::vector<HWND> g_overlays;
static HFONT g_dayFont = nullptr;
static HFONT g_timeFont = nullptr;
static HFONT g_dateFont = nullptr;
static bool g_rebuilding = false;

struct Settings {
    int dayFontSize = 72;
    int dayLetterSpacing = 35;
    int timeFontSize = 18;
    int timeLetterSpacing = 6;
    int dateFontSize = 18;
    int dateLetterSpacing = 6;
    int gapDayToDate = 26;
    int gapDateToTime = 18;
    int dateLeftOffset = 12;
    int timeLeftOffset = 24;
    bool lockWidgetPosition = false;
    int offsetX = 0;
    int offsetY = 0;
    bool timeFormat24 = false;
    bool showDate = true;
    COLORREF textColor = RGB(255, 255, 255);
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

static void DeleteFonts() {
    if (g_dayFont) { DeleteObject(g_dayFont); g_dayFont = nullptr; }
    if (g_timeFont) { DeleteObject(g_timeFont); g_timeFont = nullptr; }
    if (g_dateFont) { DeleteObject(g_dateFont); g_dateFont = nullptr; }
}

static void CreateFonts() {
    DeleteFonts();
    g_dayFont = CreateFontW(-g_settings.dayFontSize, 0, 0, 0, FW_NORMAL,
        FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
        CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Anurati");
        
    g_timeFont = CreateFontW(-g_settings.timeFontSize, 0, 0, 0, FW_LIGHT,
        FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
        CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Quicksand");

    g_dateFont = CreateFontW(-g_settings.dateFontSize, 0, 0, 0, FW_LIGHT,
        FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
        CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Quicksand");
}

static HWND FindWallpaperWorkerW() {
    HWND progman = FindWindowW(L"Progman", nullptr);
    if (progman) {
        DWORD_PTR result = 0;
        SendMessageTimeoutW(progman, 0x052C, 0, 0, SMTO_NORMAL, 1000, &result);
    }
    HWND worker = nullptr;
    EnumWindows([](HWND hwnd, LPARAM lParam) -> BOOL {
        HWND shellView = FindWindowExW(hwnd, nullptr, L"SHELLDLL_DefView", nullptr);
        if (shellView) {
            HWND next = FindWindowExW(nullptr, hwnd, L"WorkerW", nullptr);
            if (next) *reinterpret_cast<HWND*>(lParam) = next;
            return FALSE;
        }
        return TRUE;
    }, reinterpret_cast<LPARAM>(&worker));
    if (!worker) worker = FindWindowW(L"Progman", nullptr);
    return worker;
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

static int CalculateSpacedWidth(HDC dc, const std::wstring& text, int spacing) {
    SIZE sz{};
    GetTextExtentPoint32W(dc, text.c_str(), (int)text.size(), &sz);
    if (text.size() > 0) {
        sz.cx += spacing * ((int)text.size() - 1);
    }
    return sz.cx;
}

// Fixed-width alignment metric logic block
static void DrawCenteredSpaced(HDC dc, const std::wstring& text, int y, HFONT font,
                              int spacing, COLORREF color, int centreX) {
    HGDIOBJ oldFont = SelectObject(dc, font);
    SetTextColor(dc, color);
    SetBkMode(dc, TRANSPARENT);
    SetTextCharacterExtra(dc, spacing);
    
    SIZE sz{};
    GetTextExtentPoint32W(dc, text.c_str(), (int)text.size(), &sz);
    
    const int x = centreX - sz.cx / 2;
    TextOutW(dc, x, y, text.c_str(), (int)text.size());
    
    SetTextCharacterExtra(dc, 0);
    SelectObject(dc, oldFont);
}

// Renders the Anurati weekday character-by-character to inject pixel-perfect visual kerning overrides
static void DrawWeekdayWithKerning(HDC dc, const std::wstring& text, int y, HFONT font, int spacing, COLORREF color, int centreX) {
    HGDIOBJ oldFont = SelectObject(dc, font);
    SetTextColor(dc, color);
    SetBkMode(dc, TRANSPARENT);
    
    // First pass: Calculate the width footprint of each individual letter
    std::vector<int> charWidths(text.size(), 0);
    int totalWidth = 0;
    for (size_t i = 0; i < text.size(); ++i) {
        wchar_t ch[2] = { text[i], L'\0' };
        SIZE sz{};
        GetTextExtentPoint32W(dc, ch, 1, &sz);
        charWidths[i] = sz.cx;
        totalWidth += sz.cx;
    }
    
    // Total geometric layout box math including spacing variables
    if (text.size() > 0) {
        totalWidth += spacing * ((int)text.size() - 1);
        
        // Custom visual kerning override: pull the letter 'A' 14 pixels closer to the 'D' to fix the font gap quirk
        if (text.size() >= 6) {
            totalWidth -= 14; 
        }
    }
    
    int currentX = centreX - totalWidth / 2;
    
    // Second pass: Draw the text letters out onto the graphic back buffer natively
    for (size_t i = 0; i < text.size(); ++i) {
        wchar_t ch[2] = { text[i], L'\0' };
        
        // Visual Kerning Adjuster: If rendering the letter 'A' right after 'D', pull it closer to balance whitespace weights
        if (i > 0 && text[i] == L'A' && text[i-1] == L'D') {
            currentX -= 14; 
        }
        
        TextOutW(dc, currentX, y, ch, 1);
        currentX += charWidths[i] + spacing;
    }
    
    SelectObject(dc, oldFont);
}

static void Paint(HWND hwnd) {
    RECT client{};
    if (!GetClientRect(hwnd, &client)) return;
    const int w = client.right - client.left;
    const int h = client.bottom - client.top;
    if (w <= 0 || h <= 0) return;

    HDC dc = GetDC(hwnd);
    HDC mem = CreateCompatibleDC(dc);
    HBITMAP bmp = CreateCompatibleBitmap(dc, w, h);
    HGDIOBJ oldBmp = SelectObject(mem, bmp);
    HBRUSH black = CreateSolidBrush(RGB(0, 0, 0));
FillRect(mem, &client, black);
DeleteObject(black);
const std::wstring day = CurrentWeekday();
const std::wstring date = CurrentDate();
const std::wstring time = CurrentTime();
const int centreX = w / 2;
int y = 10;
// Weekday maps out with optimized individual alignment kerning loops active
DrawWeekdayWithKerning(mem, day, y, g_dayFont, g_settings.dayLetterSpacing, g_settings.textColor, centreX);
y += g_settings.dayFontSize + g_settings.gapDayToDate;
// Split Independent Shift Logic Blocks active for maximum custom alignment control
int adjustedDateCentreX = centreX - g_settings.dateLeftOffset;
int adjustedTimeCentreX = centreX - g_settings.timeLeftOffset;
if (g_settings.showDate) {
DrawCenteredSpaced(mem, date, y, g_dateFont, g_settings.dateLetterSpacing, g_settings.textColor, adjustedDateCentreX);
y += g_settings.dateFontSize + g_settings.gapDateToTime;
}
DrawCenteredSpaced(mem, time, y, g_timeFont, g_settings.timeLetterSpacing, g_settings.textColor, adjustedTimeCentreX);
POINT src{0, 0};
SIZE size{w, h};
POINT pos{};
ClientToScreen(hwnd, &pos);
BLENDFUNCTION blend{AC_SRC_OVER, 0, 255, 0};
UpdateLayeredWindow(hwnd, nullptr, &pos, &size, mem, &src,
RGB(0, 0, 0), &blend, ULW_COLORKEY);
SelectObject(mem, oldBmp);
DeleteObject(bmp);
DeleteDC(mem);
ReleaseDC(hwnd, dc);
}
static void RebuildOverlays();
static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
switch (msg) {
case WM_ERASEBKGND:
return 1;
case WM_TIMER:
if (wp == kTimerId) Paint(hwnd);
return 0;
case WM_NCHITTEST: {
LRESULT hit = DefWindowProcW(hwnd, msg, wp, lp);
if (hit == HTCLIENT && !g_settings.lockWidgetPosition) {
return HTCAPTION;
}
return g_settings.lockWidgetPosition ? HTTRANSPARENT : hit;
}
case WM_WINDOWPOSCHANGED: {
WINDOWPOS* wpStruct = reinterpret_cast<WINDOWPOS*>(lp);
if (wpStruct && !(wpStruct->flags & SWP_NOMOVE) && !g_rebuilding) {
HMONITOR monitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
MONITORINFO mi{sizeof(mi)};
if (GetMonitorInfoW(monitor, &mi)) {
int widgetWidth = wpStruct->cx;
int widgetHeight = wpStruct->cy;
int midX = mi.rcMonitor.left + (mi.rcMonitor.right - mi.rcMonitor.left) / 2;
int midY = mi.rcMonitor.top + (mi.rcMonitor.bottom - mi.rcMonitor.top) / 2;
int currentCentreX = wpStruct->x + widgetWidth / 2;
int currentCentreY = wpStruct->y + widgetHeight / 2;
g_settings.offsetX = currentCentreX - midX;
g_settings.offsetY = currentCentreY - midY;
}
}
break;
}
case WM_SETTINGCHANGE:
Paint(hwnd);
return 0;
case WM_DESTROY:
KillTimer(hwnd, kTimerId);
return 0;
}
return DefWindowProcW(hwnd, msg, wp, lp);
}
static void RebuildOverlays() {
if (g_rebuilding) return;
g_rebuilding = true;
std::vector old = g_overlays;
g_overlays.clear();
for (HWND hwnd : old) if (IsWindow(hwnd)) DestroyWindow(hwnd);
EnumDisplayMonitors(nullptr, nullptr, [](HMONITOR monitor, HDC, LPRECT, LPARAM) -> BOOL {
MONITORINFO mi{sizeof(mi)};
if (!GetMonitorInfoW(monitor, &mi)) return TRUE;
HDC tempDC = GetDC(nullptr);
HFONT tempFont = CreateFontW(-g_settings.dayFontSize, 0, 0, 0, FW_NORMAL,
FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Anurati");
HGDIOBJ oldF = SelectObject(tempDC, tempFont);
std::wstring sampleDay = CurrentWeekday();
int dayWidth = CalculateSpacedWidth(tempDC, sampleDay, g_settings.dayLetterSpacing);
SelectObject(tempDC, oldF);
DeleteObject(tempFont);
ReleaseDC(nullptr, tempDC);
int widgetW = dayWidth + 250;
int widgetH = g_settings.dayFontSize + g_settings.gapDayToDate + g_settings.dateFontSize + g_settings.gapDateToTime + g_settings.timeFontSize + 80;
int midX = mi.rcMonitor.left + (mi.rcMonitor.right - mi.rcMonitor.left) / 2;
int midY = mi.rcMonitor.top + (mi.rcMonitor.bottom - mi.rcMonitor.top) / 2;
int posX = midX + g_settings.offsetX - widgetW / 2;
int posY = midY + g_settings.offsetY - widgetH / 2;
HWND owner = FindWallpaperWorkerW();
HWND hwnd = CreateWindowExW(
WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
kWindowClass, L"Mond Desktop Clock", WS_POPUP,
posX, posY, widgetW, widgetH,
owner, nullptr, GetModuleHandleW(nullptr), nullptr);
if (hwnd) {
g_overlays.push_back(hwnd);
SetTimer(hwnd, kTimerId, 1000, nullptr);
ShowWindow(hwnd, SW_SHOWNOACTIVATE);
Paint(hwnd);
}
return TRUE;
}, 0);
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
g_settings.offsetX = Wh_GetIntSetting(L"offsetX");
g_settings.offsetY = Wh_GetIntSetting(L"offsetY");
g_settings.timeFormat24 = Wh_GetIntSetting(L"timeFormat24") != 0;
g_settings.showDate = Wh_GetIntSetting(L"showDate") != 0;
PCWSTR color = Wh_GetStringSetting(L"textColor");
g_settings.textColor = ParseColor(color);
Wh_FreeStringSetting(color);
}
BOOL Wh_ModInit() {
LoadSettings();
WNDCLASSW wc{};
wc.lpfnWndProc = WndProc;
wc.hInstance = GetModuleHandleW(nullptr);
wc.lpszClassName = kWindowClass;
wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
RegisterClassW(&wc);
CreateFonts();
RebuildOverlays();
return !g_overlays.empty();
}
void Wh_ModUninit() {
std::vector old = g_overlays;
g_overlays.clear();
for (HWND hwnd : old) if (IsWindow(hwnd)) DestroyWindow(hwnd);
DeleteFonts();
}
BOOL Wh_ModSettingsChanged(BOOL* bReload) {
*bReload = FALSE;
LoadSettings();
CreateFonts();
for (HWND hwnd : g_overlays) {
InvalidateRect(hwnd, nullptr, TRUE);
Paint(hwnd);
}
return TRUE;
}
