// ==WindhawkMod==
// @id              s66cw-streak-fire-vortex
// @name            STREAK
// @description     Static flame and day counter with 7/30/100-day streak goals, progress bar and saved settings.
// @version         1.7.0
// @author          S66CW ON IG
// @github          https://github.com/sl88la
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lgdiplus -lshell32 -lgdi32 -luser32
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# STREAK - Static Flame & Streak Goals

A COMPLETELY STATIC transparent flame above the taskbar on Windows 11.
No idle animation, no Bloom, no Vortex, no particles, no celebration,
no motion blur, no periodic visual refresh or background shadow.
The number inside the flame changes once when a new day is credited.

LEFT CLICK the flame to open the settings panel:
- Current streak, longest streak, total credited days and today's status.
- Choose a goal: 7 / 30 / 100 consecutive days.
- Static progress bar, percentage and days remaining; shows completed when met.
- Goal selection persists across reboots and does NOT alter streak history.
- Flame size +/- (60%-220%), lock/unlock dragging and stay on top.
- Reset the flame position and close the panel.
- Ctrl+wheel on flame resizes it, even when position locked.
- Unlock position to drag the flame.
- Right click opens a size and position menu.

After approximately 15 seconds of recent keyboard/mouse input during a
calendar day, the day is credited once. This is an activity estimate,
not a continuous-input or exact productivity measurement.
Day data, flame location, scale and panel options are persisted via Windhawk.
This publication uses a different mod ID from older local versions (s66-streak-fire-vortex). Saved streak values and UI settings do not automatically transfer between IDs. Disable the old mod to avoid showing duplicate widgets.

Renders only on launch, actual streak changes, resize or settings change,
to avoid flickering while idle. Settings paint via off-screen buffer.

This mod runs inside explorer.exe; disable it in Windhawk if Explorer
becomes unstable. It does NOT reserve taskbar space.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- autoPopupOnNewDay: false
  $name: Open the static status panel when a day is credited
- activeInputWindowMs: 1300
  $name: Recent input window in milliseconds (activity estimate)
*/
// ==/WindhawkModSettings==

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <shellapi.h>
#include <shlobj.h>
#include <gdiplus.h>
#include <algorithm>
#include <atomic>
#include <cmath>
#include <cstdio>
#include <ctime>
#include <regex>
#include <set>
#include <string>
#include <vector>
#include <cstring>

using namespace Gdiplus;

namespace {
constexpr UINT WM_RELOAD_SETTINGS = WM_APP + 67;
constexpr UINT WM_CLOSE_TOOL = WM_APP + 68;
constexpr UINT TIMER_ID = 1;
constexpr UINT IDM_DETAILS = 902;
constexpr UINT IDM_RESET = 903;
constexpr UINT IDM_SIZE_UP = 904;
constexpr UINT IDM_SIZE_DOWN = 905;
constexpr UINT IDM_SIZE_70 = 910;
constexpr UINT IDM_SIZE_100 = 911;
constexpr UINT IDM_SIZE_130 = 912;
constexpr UINT IDM_SIZE_160 = 913;
constexpr UINT IDM_SIZE_200 = 914;
constexpr int OVERLAY_W = 150;
constexpr int OVERLAY_H = 184;
constexpr int MIN_SIZE_PERCENT = 60;
constexpr int MAX_SIZE_PERCENT = 220;
constexpr int FLYOUT_W = 340;
constexpr int FLYOUT_H = 580;

std::atomic<HWND> g_overlay{nullptr};
HWND g_flyout = nullptr;
HANDLE g_uiThread = nullptr;
HANDLE g_threadReady = nullptr;
ULONG_PTR g_gdiplusToken = 0;
ULONGLONG g_lastTick = 0;
int g_lastCheckedDay = 0;
int g_lastCompletedDay = 0;
int g_currentStreak = 0;
int g_bestStreak = 0;
int g_totalDays = 0;
int g_goalTarget = 7;  // saved independently of streak count
double g_progressSeconds = 0;
bool g_autoPopupOnNewDay = false;
bool g_alwaysOnTop = false;
bool g_lockPosition = true;
int g_activeInputWindowMs = 1300;
int g_sizePercent = 100;

bool IsSupportedGoal(int v) { return v == 7 || v == 30 || v == 100; }

// Returns values independent of UI rendering: no goal side effects.
int GoalDaysDone() { return std::clamp<int>(g_currentStreak, 0, g_goalTarget); }
int GoalDaysLeft() { return std::max(0, g_goalTarget - g_currentStreak); }
int GoalPercent() { return g_goalTarget > 0 ? (100 * GoalDaysDone()) / g_goalTarget : 0; }

int OverlayWidth() { return MulDiv(OVERLAY_W, g_sizePercent, 100); }
int OverlayHeight() { return MulDiv(OVERLAY_H, g_sizePercent, 100); }
bool g_dragging = false;
bool g_dragMoved = false;
POINT g_dragStartCursor{};
POINT g_dragStartWindow{};

int DayKey(const tm& t) {
    return (t.tm_year + 1900) * 10000 + (t.tm_mon + 1) * 100 + t.tm_mday;
}

int LocalDayKey() {
    SYSTEMTIME local{};
    GetLocalTime(&local);
    return static_cast<int>(local.wYear) * 10000 +
           static_cast<int>(local.wMonth) * 100 + local.wDay;
}

int PreviousDay(int dayKey) {
    tm t{};
    t.tm_year = dayKey / 10000 - 1900;
    t.tm_mon = (dayKey / 100) % 100 - 1;
    t.tm_mday = dayKey % 100 - 1;
    t.tm_hour = 12; // noon avoids DST-midnight boundaries
    t.tm_isdst = -1;
    if (mktime(&t) == static_cast<time_t>(-1)) return 0;
    return DayKey(t);
}

bool ParseDay(const std::string& text, int* out) {
    if (text.size() != 10 || text[4] != '-' || text[7] != '-') return false;
    int y = 0, m = 0, d = 0;
    if (sscanf(text.c_str(), "%4d-%2d-%2d", &y, &m, &d) != 3) return false;
    if (y < 2000 || y > 2100 || m < 1 || m > 12 || d < 1 || d > 31) return false;
    tm t{};
    t.tm_year = y - 1900;
    t.tm_mon = m - 1;
    t.tm_mday = d;
    t.tm_hour = 12;
    t.tm_isdst = -1;
    if (mktime(&t) == static_cast<time_t>(-1)) return false;
    if (t.tm_year != y-1900 || t.tm_mon != m-1 || t.tm_mday != d) return false;
    *out = y * 10000 + m * 100 + d;
    return true;
}

void ImportOldElectronHistoryOnce() {
    if (Wh_GetIntValue(L"migrated", 0)) return;
    WCHAR appData[MAX_PATH]{};
    if (SHGetFolderPathW(nullptr, CSIDL_APPDATA, nullptr, SHGFP_TYPE_CURRENT,
                         appData) == S_OK) {
        std::wstring filePath = std::wstring(appData) + L"\\S66Streak\\streak.json";
        FILE* f = _wfopen(filePath.c_str(), L"rb");
        if (f) {
            if (fseek(f, 0, SEEK_END) == 0) {
                long len = ftell(f);
                if (len > 0 && len < 1024 * 1024 && fseek(f, 0, SEEK_SET) == 0) {
                    std::string json(static_cast<size_t>(len), '\0');
                    size_t got = fread(json.data(), 1, json.size(), f);
                    json.resize(got);
                    auto start = json.find("\"days\"");
                    if (start != std::string::npos) {
                        auto open = json.find('[', start);
                        auto close = open == std::string::npos ? std::string::npos
                                                                : json.find(']', open);
                        if (close != std::string::npos) {
                            std::set<int> dates;
                            std::string array = json.substr(open, close - open);
                            std::regex dateRe(R"(\"(\d{4}-\d{2}-\d{2})\")");
                            for (auto it = std::sregex_iterator(array.begin(), array.end(), dateRe);
                                 it != std::sregex_iterator(); ++it) {
                                int key = 0;
                                if (ParseDay((*it)[1].str(), &key) && key <= LocalDayKey())
                                    dates.insert(key);
                            }
                            if (!dates.empty()) {
                                int last = *dates.rbegin();
                                int longest = 0, run = 0, prior = 0;
                                for (int key : dates) {
                                    run = (prior && PreviousDay(key) == prior) ? run + 1 : 1;
                                    longest = std::max(longest, run);
                                    prior = key;
                                }
                                g_lastCompletedDay = last;
                                g_currentStreak = run;
                                g_bestStreak = longest;
                                g_totalDays = static_cast<int>(dates.size());
                                std::regex bestRe(R"(\"best\"\s*:\s*(\d+))");
                                std::smatch bestMatch;
                                if (std::regex_search(json, bestMatch, bestRe)) {
                                    try { g_bestStreak = std::max(g_bestStreak,
                                              std::stoi(bestMatch[1].str())); }
                                    catch (...) {}
                                }
                                Wh_SetIntValue(L"lastDay", g_lastCompletedDay);
                                Wh_SetIntValue(L"current", g_currentStreak);
                                Wh_SetIntValue(L"best", g_bestStreak);
                                Wh_SetIntValue(L"total", g_totalDays);
                            }
                        }
                    }
                }
            }
            fclose(f);
        }
    }
    Wh_SetIntValue(L"migrated", 1);
}

void RefreshCurrentStreakForDate(int today) {
    if (g_lastCompletedDay != today && g_lastCompletedDay != PreviousDay(today))
        g_currentStreak = 0;
}

void LoadState() {
    g_sizePercent = std::clamp<int>(Wh_GetIntValue(L"sizePct", 100),
                                    MIN_SIZE_PERCENT, MAX_SIZE_PERCENT);
    g_lastCompletedDay = Wh_GetIntValue(L"lastDay", 0);
    g_currentStreak = std::max(0, Wh_GetIntValue(L"current", 0));
    g_bestStreak = std::max(0, Wh_GetIntValue(L"best", 0));
    g_totalDays = std::max(0, Wh_GetIntValue(L"total", 0));
    int storedGoal = Wh_GetIntValue(L"goalTarget", 7);
    g_goalTarget = IsSupportedGoal(storedGoal) ? storedGoal : 7;
    ImportOldElectronHistoryOnce();
    g_lastCheckedDay = LocalDayKey();
    const int before = g_currentStreak;
    RefreshCurrentStreakForDate(g_lastCheckedDay);
    // Persist broken streak immediately, without touching best/total history.
    if (before != g_currentStreak) Wh_SetIntValue(L"current", g_currentStreak);
}

void SaveState() {
    Wh_SetIntValue(L"lastDay", g_lastCompletedDay);
    Wh_SetIntValue(L"current", g_currentStreak);
    Wh_SetIntValue(L"best", g_bestStreak);
    Wh_SetIntValue(L"total", g_totalDays);
}

void LoadSettings() {
    g_autoPopupOnNewDay = Wh_GetIntSetting(L"autoPopupOnNewDay") != 0;
    g_alwaysOnTop = Wh_GetIntValue(L"uiOnTop", 0) != 0;
    g_lockPosition = Wh_GetIntValue(L"uiLock", 1) != 0;
    int inputMs = Wh_GetIntSetting(L"activeInputWindowMs");
    g_activeInputWindowMs = std::clamp(inputMs, 250, 5000);
}

void SaveUiSettings() {
    Wh_SetIntValue(L"uiLock", g_lockPosition ? 1 : 0);
    Wh_SetIntValue(L"uiOnTop", g_alwaysOnTop ? 1 : 0);
}

// A fixed, motionless angular flame silhouette in 180x225 logical units.
// This is rendered only when the displayed streak or window geometry changes.
const PointF FLAME_POINTS[] = {
    {87,  6}, {97,  35}, {102, 62}, {120, 41},
    {126, 70}, {120, 95}, {145, 73}, {155,108},
    {169,141}, {166,164}, {152,190}, {130,210},
    {108,220}, {88,224}, {64,219}, {42,209},
    {23,190}, {12,164}, {15,134}, {29,104},
    {31, 74}, {50, 99}, {59,116}, {67, 89},
    {68, 60}, {81, 32}
};
constexpr size_t FLAME_N = sizeof(FLAME_POINTS)/sizeof(FLAME_POINTS[0]);

void DrawFlame(Graphics& gr, int width, int height, int displayedStreak) {
    gr.SetSmoothingMode(SmoothingModeAntiAlias);
    gr.SetTextRenderingHint(TextRenderingHintAntiAliasGridFit);
    const float scale = std::min(width/194.f, height/238.f);
    const float ox = (width-180.f*scale)*.5f;
    const float oy = (height-225.f*scale)*.5f;
    GraphicsState old=gr.Save();
    gr.TranslateTransform(ox, oy, MatrixOrderAppend);
    gr.ScaleTransform(scale,scale,MatrixOrderAppend);

    GraphicsPath shape;
    shape.AddClosedCurve(FLAME_POINTS,static_cast<INT>(FLAME_N),.11f);
    LinearGradientBrush fill(PointF(90.f,5.f),PointF(90.f,224.f),
        Color(255,255,185,83),Color(255,163,28,16));
    gr.FillPath(&fill,&shape);
    PointF inside[]={{87,99},{98,125},{119,162},{110,196},
                     {89,215},{69,203},{56,175},{66,143},{74,120}};
    GraphicsPath core;
    core.AddClosedCurve(inside,9,.19f);
    SolidBrush coreColor(Color(130,118,22,13));
    gr.FillPath(&coreColor,&core);

    SolidBrush ink(Color(255,255,255,255));
    FontFamily impact(L"Impact"), fallback(L"Arial");
    FontFamily* family=impact.IsAvailable()?&impact:&fallback;
    Font numFont(family,65.f,FontStyleRegular,UnitPixel);
    Font dayFont(family,23.f,FontStyleRegular,UnitPixel);
    StringFormat centered;
    centered.SetAlignment(StringAlignmentCenter);
    centered.SetLineAlignment(StringAlignmentCenter);
    std::wstring number=std::to_wstring(std::max(0,displayedStreak));
    gr.DrawString(number.c_str(),-1,&numFont,
                  RectF(18.f,118.f,144.f,65.f),&centered,&ink);
    gr.DrawString(L"day",-1,&dayFont,
                  RectF(38.f,173.f,104.f,29.f),&centered,&ink);
    gr.Restore(old);
}

// ----- Interactive custom GDI+ settings panel (left-click the flame). -----
void RoundRect(Graphics& gr, const RectF& r, float rad,
               const Color& bg, const Color& outline) {
    GraphicsPath path;
    const float d=rad*2.f;
    path.AddArc(r.X, r.Y, d,d,180.f,90.f);
    path.AddArc(r.GetRight()-d,r.Y,d,d,270.f,90.f);
    path.AddArc(r.GetRight()-d,r.GetBottom()-d,d,d,0.f,90.f);
    path.AddArc(r.X,r.GetBottom()-d,d,d,90.f,90.f);
    path.CloseFigure();
    SolidBrush brush(bg);
    gr.FillPath(&brush,&path);
    Pen border(outline,1.f);
    gr.DrawPath(&border,&path);
}

void UiText(Graphics& gr, const std::wstring& value,
            float x,float y,float w,float h,float size, Color color,
            bool bold=false, StringAlignment align=StringAlignmentNear) {
    FontFamily family(L"Segoe UI");
    FontFamily fallback(L"Arial");
    FontFamily* use=family.IsAvailable() ? &family : &fallback;
    Font font(use,size,bold ? FontStyleBold : FontStyleRegular,UnitPixel);
    SolidBrush brush(color);
    StringFormat format;
    format.SetAlignment(align);
    format.SetLineAlignment(StringAlignmentCenter);
    format.SetTrimming(StringTrimmingEllipsisCharacter);
    gr.DrawString(value.c_str(),-1,&font,RectF(x,y,w,h),&format,&brush);
}

void Toggle(Graphics& gr, float x,float y, bool on) {
    RoundRect(gr,RectF(x,y,48.f,26.f),13.f,
              on ? Color(255,223,107,53) : Color(255,66,65,71),
              on ? Color(255,239,137,82) : Color(255,81,80,87));
    SolidBrush circle(Color(255,245,245,246));
    gr.FillEllipse(&circle,x+(on?25.f:3.f),y+3.f,20.f,20.f);
}

void ActionBox(Graphics& gr, float x,float y,float w,float h,
               const std::wstring& value, bool active=false) {
    RoundRect(gr,RectF(x,y,w,h),8.f,
              active ? Color(255,184,68,31) : Color(255,36,35,39),
              active ? Color(255,233,118,66) : Color(255,65,62,66));
    UiText(gr,value,x,y,w,h,12.f,
           active ? Color(255,255,247,240) : Color(255,220,215,212),
           true,StringAlignmentCenter);
}

void DrawPanel(Graphics& gr) {
    gr.SetSmoothingMode(SmoothingModeAntiAlias);
    gr.SetTextRenderingHint(TextRenderingHintAntiAliasGridFit);
    gr.Clear(Color(255,15,15,18));
    RoundRect(gr,RectF(1.f,1.f,338.f,577.f),16.f,
              Color(255,20,20,24),Color(255,66,58,54));
    UiText(gr,L"S66 / STREAK",23,16,190,26,16.f,Color(255,245,243,242),true);
    UiText(gr,L"SETTINGS",215,18,80,22,10.f,Color(255,239,145,92),true,StringAlignmentFar);
    ActionBox(gr,298,15,26,29,L"X");
    UiText(gr,L"STATIC FLAME",23,44,200,17,10.f,Color(255,132,131,138));

    RoundRect(gr,RectF(19.f,70.f,302.f,100.f),12.f,
              Color(255,30,27,28),Color(255,66,45,36));
    UiText(gr,L"CURRENT STREAK",35,79,170,19,10.f,Color(255,239,149,100),true);
    UiText(gr,std::to_wstring(g_currentStreak),32,98,110,57,43.f,Color(255,255,248,242),true);
    UiText(gr,L"day",132,113,55,34,15.f,Color(255,237,152,105),true);
    UiText(gr,L"BEST  "+std::to_wstring(g_bestStreak),209,96,103,22,12.f,Color(255,217,213,211),true);
    UiText(gr,L"TOTAL  "+std::to_wstring(g_totalDays),209,119,103,22,12.f,Color(255,163,161,166));
    UiText(gr,g_lastCompletedDay==LocalDayKey() ? L"Today completed" : L"Today's day not yet credited",
           35,147,260,17,10.f,
           g_lastCompletedDay==LocalDayKey()?Color(255,152,198,135):Color(255,144,143,148));

    UiText(gr,L"STREAK GOALS",22,182,190,23,12.f,Color(255,236,226,218),true);
    UiText(gr,L"Choose your target",188,184,130,19,10.f,Color(255,145,140,142),false,StringAlignmentFar);
    ActionBox(gr,22,211,91,35,L"7 DAYS",g_goalTarget==7);
    ActionBox(gr,124,211,91,35,L"30 DAYS",g_goalTarget==30);
    ActionBox(gr,226,211,91,35,L"100 DAYS",g_goalTarget==100);

    RoundRect(gr,RectF(21.f,258.f,297.f,83.f),10.f,
              Color(255,31,29,31),Color(255,65,53,48));
    UiText(gr,L"PROGRESS",34,268,100,20,10.f,Color(255,238,155,111),true);
    UiText(gr,std::to_wstring(GoalDaysDone()) + L" / " + std::to_wstring(g_goalTarget) + L" days",
           165,267,139,23,12.f,Color(255,244,238,235),true,StringAlignmentFar);
    RoundRect(gr,RectF(34.f,297.f,270.f,11.f),5.f,
              Color(255,64,56,53),Color(255,64,56,53));
    if (GoalDaysDone() > 0) {
        const float progressWidth = std::max(5.f, 270.f * GoalDaysDone() / g_goalTarget);
        RoundRect(gr,RectF(34.f,297.f,progressWidth,11.f),
                  std::min(5.f,progressWidth/2.f),
                  Color(255,237,116,57),Color(255,237,116,57));
    }
    const bool reached = GoalDaysLeft() == 0;
    UiText(gr,reached ? L"GOAL COMPLETED" :
           std::to_wstring(GoalDaysLeft()) + L" days remaining",
           34,313,191,21,11.f,
           reached ? Color(255,150,209,142) : Color(255,165,159,157),true);
    UiText(gr,std::to_wstring(GoalPercent())+L"%",242,313,62,21,11.f,
           Color(255,235,169,127),true,StringAlignmentFar);

    UiText(gr,L"DISPLAY",22,351,120,20,11.f,Color(255,228,221,216),true);
    UiText(gr,L"Static flame - no animations",23,369,289,18,10.f,
           Color(255,150,146,147));
    UiText(gr,L"Flame size",23,399,195,32,12.f,Color(255,220,216,216));
    ActionBox(gr,236,399,36,33,L"-");
    UiText(gr,std::to_wstring(g_sizePercent)+L"%",272,399,32,33,10.f,
           Color(255,242,242,243),true,StringAlignmentCenter);
    ActionBox(gr,305,399,21,33,L"+");

    UiText(gr,L"Lock flame position",23,450,225,28,12.f,Color(255,220,216,216));
    Toggle(gr,273,450,g_lockPosition);
    UiText(gr,L"Stay above other windows",23,489,230,28,12.f,Color(255,220,216,216));
    Toggle(gr,273,489,g_alwaysOnTop);

    RoundRect(gr,RectF(22.f,532.f,296.f,35.f),10.f,
              Color(255,40,36,36),Color(255,83,68,64));
    UiText(gr,L"RESET FLAME POSITION",22,532,296,35,12.f,
           Color(255,226,217,213),true,StringAlignmentCenter);
}

// Render GDI+ to a premultiplied ARGB DIB and deliver it to the shell as a
// genuine transparent window. No drop shadow, black rectangle, or tray icon.
void RenderOverlay() {
    HWND hwnd = g_overlay.load();
    if (!hwnd || !IsWindow(hwnd)) return;
    const int width = OverlayWidth();
    const int height = OverlayHeight();
    Bitmap bitmap(width, height, PixelFormat32bppPARGB);
    if (bitmap.GetLastStatus() != Ok) return;
    {
        Graphics gr(&bitmap);
        gr.Clear(Color(0, 0, 0, 0));
        DrawFlame(gr, width, height, g_currentStreak);
    }
    Rect lockRect(0, 0, width, height);
    BitmapData pixels{};
    if (bitmap.LockBits(&lockRect, ImageLockModeRead,
                        PixelFormat32bppPARGB, &pixels) != Ok) return;
    HDC screen = GetDC(nullptr);
    if (!screen) { bitmap.UnlockBits(&pixels); return; }
    HDC mem = CreateCompatibleDC(screen);
    BITMAPINFO info{};
    info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    info.bmiHeader.biWidth = width;
    info.bmiHeader.biHeight = -height; // top-down premultiplied alpha
    info.bmiHeader.biPlanes = 1;
    info.bmiHeader.biBitCount = 32;
    info.bmiHeader.biCompression = BI_RGB;
    void* bits = nullptr;
    HBITMAP dib = mem ? CreateDIBSection(screen, &info, DIB_RGB_COLORS,
                                         &bits, nullptr, 0) : nullptr;
    if (dib && bits) {
        HGDIOBJ prev = SelectObject(mem, dib);
        for (int y = 0; y < height; ++y) {
            const BYTE* row = static_cast<const BYTE*>(pixels.Scan0) +
                              y * pixels.Stride;
            memcpy(static_cast<BYTE*>(bits) +
                   static_cast<size_t>(y) * width * 4,
                   row, static_cast<size_t>(width) * 4);
        }
        RECT pos{};
        GetWindowRect(hwnd, &pos);
        POINT dst{pos.left, pos.top};
        POINT source{0, 0};
        SIZE size{width, height};
        BLENDFUNCTION blend{AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
        UpdateLayeredWindow(hwnd, screen, &dst, &size, mem, &source,
                            0, &blend, ULW_ALPHA);
        SelectObject(mem, prev);
    }
    if (dib) DeleteObject(dib);
    if (mem) DeleteDC(mem);
    ReleaseDC(nullptr, screen);
    bitmap.UnlockBits(&pixels);
}

RECT WorkAreaFromPoint(POINT p) {
    HMONITOR monitor = MonitorFromPoint(p, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi{sizeof(mi)};
    if (GetMonitorInfoW(monitor, &mi)) return mi.rcWork;
    RECT fallback{};
    SystemParametersInfoW(SPI_GETWORKAREA, 0, &fallback, 0);
    return fallback;
}

void DefaultOverlayPosition(int* x, int* y) {
    POINT cursor{};
    GetCursorPos(&cursor);
    RECT work = WorkAreaFromPoint(cursor);
    *x = work.right - OverlayWidth() - 20;
    *y = work.bottom - OverlayHeight() - 6;
}

void PlaceOverlay(bool useSaved) {
    HWND hwnd = g_overlay.load();
    if (!hwnd) return;
    int x = 0, y = 0;
    DefaultOverlayPosition(&x, &y);
    if (useSaved && Wh_GetIntValue(L"posSaved", 0)) {
        x = Wh_GetIntValue(L"posX", x);
        y = Wh_GetIntValue(L"posY", y);
        POINT midpoint{x + OverlayWidth()/2, y + OverlayHeight()/2};
        RECT work = WorkAreaFromPoint(midpoint);
        x = std::clamp<int>(x, work.left, std::max(work.left, work.right-OverlayWidth()));
        y = std::clamp<int>(y, work.top, std::max(work.top, work.bottom-OverlayHeight()));
    }
    SetWindowPos(hwnd, g_alwaysOnTop ? HWND_TOPMOST : HWND_NOTOPMOST,
                 x, y, OverlayWidth(), OverlayHeight(),
                 SWP_NOACTIVATE | SWP_SHOWWINDOW);
}

void SaveOverlayPosition() {
    HWND hwnd = g_overlay.load();
    if (!hwnd) return;
    RECT rect{};
    GetWindowRect(hwnd, &rect);
    Wh_SetIntValue(L"posX", rect.left);
    Wh_SetIntValue(L"posY", rect.top);
    Wh_SetIntValue(L"posSaved", 1);
}

void PositionFlyout() {
    if (!g_flyout) return;
    RECT anchor{};
    HWND hwnd = g_overlay.load();
    if (!hwnd || !GetWindowRect(hwnd, &anchor)) return;
    POINT p{anchor.left, anchor.top};
    RECT work = WorkAreaFromPoint(p);
    int x = anchor.left - FLYOUT_W - 12;
    int y = anchor.bottom - FLYOUT_H;
    if (x < work.left) x = anchor.right + 10;
    x = std::clamp<int>(x, work.left, std::max(work.left, work.right-FLYOUT_W));
    y = std::clamp<int>(y, work.top, std::max(work.top, work.bottom-FLYOUT_H));
    SetWindowPos(g_flyout, HWND_TOPMOST, x, y, FLYOUT_W, FLYOUT_H,
                 SWP_NOACTIVATE | SWP_NOOWNERZORDER);
}

// Zoom around the flame centre, clamp to the current monitor's work area,
// save size and position, and resize the layered ARGB bitmap to match.
void ResizeOverlay(int requestedPercent) {
    int next = std::clamp<int>(requestedPercent, MIN_SIZE_PERCENT, MAX_SIZE_PERCENT);
    if (next == g_sizePercent) return;
    HWND hwnd = g_overlay.load();
    if (!hwnd || !IsWindow(hwnd)) return;
    RECT before{};
    if (!GetWindowRect(hwnd, &before)) return;
    POINT center{before.left + (before.right - before.left) / 2,
                 before.top + (before.bottom - before.top) / 2};
    RECT work = WorkAreaFromPoint(center);
    g_sizePercent = next;
    const int width = OverlayWidth();
    const int height = OverlayHeight();
    int x = center.x - width / 2;
    int y = center.y - height / 2;
    x = std::clamp<int>(x, work.left,
                        std::max<int>(work.left, work.right - width));
    y = std::clamp<int>(y, work.top,
                        std::max<int>(work.top, work.bottom - height));
    SetWindowPos(hwnd, nullptr, x, y, width, height,
                 SWP_NOZORDER | SWP_NOACTIVATE);
    Wh_SetIntValue(L"sizePct", g_sizePercent);
    Wh_SetIntValue(L"posX", x);
    Wh_SetIntValue(L"posY", y);
    Wh_SetIntValue(L"posSaved", 1);
    RenderOverlay();
    if (g_flyout && IsWindowVisible(g_flyout)) PositionFlyout();
}

void ShowFlyout(bool focus) {
    if (!g_flyout) return;
    PositionFlyout();
    ShowWindow(g_flyout, focus ? SW_SHOWNORMAL : SW_SHOWNOACTIVATE);
    InvalidateRect(g_flyout, nullptr, FALSE);
    if (focus) SetForegroundWindow(g_flyout);
}

void CreditToday() {
    int today = LocalDayKey();
    if (g_lastCompletedDay == today) return;
    g_currentStreak = g_lastCompletedDay == PreviousDay(today) ?
                      g_currentStreak + 1 : 1;
    g_bestStreak = std::max(g_bestStreak, g_currentStreak);
    ++g_totalDays;
    g_lastCompletedDay = today;
    g_progressSeconds = 15;
    SaveState();
    // No celebration or transition: repaint once with the new day count.
    RenderOverlay();
    if (g_autoPopupOnNewDay) ShowFlyout(false);
    if (g_flyout && IsWindowVisible(g_flyout))
        InvalidateRect(g_flyout, nullptr, FALSE);
}

void OnTimer() {
    const ULONGLONG now = GetTickCount64();
    double elapsed = g_lastTick ? std::min(0.25, (now - g_lastTick) / 1000.0) : 0;
    g_lastTick = now;
    const int today = LocalDayKey();
    if (today != g_lastCheckedDay) {
        g_lastCheckedDay = today;
        g_progressSeconds = 0;
        const int oldStreak = g_currentStreak;
        RefreshCurrentStreakForDate(today);
        if (oldStreak != g_currentStreak) {
            Wh_SetIntValue(L"current", g_currentStreak);
            RenderOverlay();
        }
        if (g_flyout && IsWindowVisible(g_flyout))
            InvalidateRect(g_flyout, nullptr, FALSE);
    }
    if (today != g_lastCompletedDay) {
        LASTINPUTINFO li{};
        li.cbSize = sizeof(li);
        if (GetLastInputInfo(&li)) {
            DWORD sinceInput = GetTickCount() - li.dwTime;
            if (sinceInput <= static_cast<DWORD>(g_activeInputWindowMs))
                g_progressSeconds += elapsed;
        }
        if (g_progressSeconds >= 15) CreditToday();
    }


}

void OverlayContextMenu(HWND hwnd) {
    HMENU menu = CreatePopupMenu();
    if (!menu) return;
    AppendMenuW(menu, MF_STRING, IDM_DETAILS, L"Open flame settings");
    AppendMenuW(menu, MF_SEPARATOR, 0, nullptr);
    AppendMenuW(menu, MF_STRING | (g_sizePercent >= MAX_SIZE_PERCENT ? MF_GRAYED : 0),
                IDM_SIZE_UP, L"Make flame bigger (+10%)");
    AppendMenuW(menu, MF_STRING | (g_sizePercent <= MIN_SIZE_PERCENT ? MF_GRAYED : 0),
                IDM_SIZE_DOWN, L"Make flame smaller (-10%)");
    HMENU sizes = CreatePopupMenu();
    if (sizes) {
        const struct { UINT id; int percent; const wchar_t* text; } choices[] = {
            {IDM_SIZE_70, 70, L"70% - Small"},
            {IDM_SIZE_100, 100, L"100% - Normal"},
            {IDM_SIZE_130, 130, L"130% - Medium"},
            {IDM_SIZE_160, 160, L"160% - Large"},
            {IDM_SIZE_200, 200, L"200% - Extra large"},
        };
        for (const auto& item : choices) {
            AppendMenuW(sizes, MF_STRING |
                        (g_sizePercent == item.percent ? MF_CHECKED : 0),
                        item.id, item.text);
        }
        AppendMenuW(menu, MF_POPUP, reinterpret_cast<UINT_PTR>(sizes),
                    L"Set exact size");
    }
    AppendMenuW(menu, MF_SEPARATOR, 0, nullptr);
    AppendMenuW(menu, MF_STRING, IDM_RESET, L"Reset flame position");
    POINT p{};
    GetCursorPos(&p);
    SetForegroundWindow(hwnd);
    UINT cmd = TrackPopupMenu(menu,
                              TPM_RETURNCMD | TPM_NONOTIFY | TPM_RIGHTBUTTON,
                              p.x, p.y, 0, hwnd, nullptr);
    PostMessageW(hwnd, WM_NULL, 0, 0);
    DestroyMenu(menu); // also destroys attached submenus
    if (cmd == IDM_DETAILS) ShowFlyout(true);
    else if (cmd == IDM_SIZE_UP) ResizeOverlay(g_sizePercent + 10);
    else if (cmd == IDM_SIZE_DOWN) ResizeOverlay(g_sizePercent - 10);
    else if (cmd >= IDM_SIZE_70 && cmd <= IDM_SIZE_200) {
        switch (cmd) {
            case IDM_SIZE_70: ResizeOverlay(70); break;
            case IDM_SIZE_100: ResizeOverlay(100); break;
            case IDM_SIZE_130: ResizeOverlay(130); break;
            case IDM_SIZE_160: ResizeOverlay(160); break;
            case IDM_SIZE_200: ResizeOverlay(200); break;
        }
    } else if (cmd == IDM_RESET) {
        Wh_SetIntValue(L"posSaved", 0);
        PlaceOverlay(false);
        RenderOverlay();
    }
}

void ApplyUiClick(HWND hwnd, int x, int y) {
    if (x>=296 && x<=330 && y>=14 && y<=48) {
        ShowWindow(hwnd,SW_HIDE);
        return;
    }
    // Goal buttons: 7 / 30 / 100. Changing the goal never credits days.
    if (y>=211 && y<=246) {
        int chosen = 0;
        if (x>=22 && x<=113) chosen=7;
        else if (x>=124 && x<=215) chosen=30;
        else if (x>=226 && x<=317) chosen=100;
        if (!chosen || chosen == g_goalTarget) return;
        g_goalTarget = chosen;
        Wh_SetIntValue(L"goalTarget", g_goalTarget);
    } else if (y>=392 && y<=440) {
        if (x>=234 && x<=273) ResizeOverlay(g_sizePercent-10);
        else if (x>=304 && x<=328) ResizeOverlay(g_sizePercent+10);
        else return;
    } else if (y>=445 && y<=483) {
        g_lockPosition=!g_lockPosition;
        SaveUiSettings();
    } else if (y>=485 && y<=524) {
        g_alwaysOnTop=!g_alwaysOnTop;
        SaveUiSettings();
        HWND overlay=g_overlay.load();
        if (overlay) SetWindowPos(overlay,
                                 g_alwaysOnTop ? HWND_TOPMOST : HWND_NOTOPMOST,
                                 0,0,0,0,SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
    } else if (y>=528 && y<=570) {
        Wh_SetIntValue(L"posSaved",0);
        PlaceOverlay(false);
        RenderOverlay();
        PositionFlyout();
    } else {
        return;
    }
    InvalidateRect(hwnd,nullptr,FALSE);
}

LRESULT CALLBACK FlyoutProc(HWND hwnd, UINT msg, WPARAM w, LPARAM l) {
    switch (msg) {
        case WM_PAINT: {
            PAINTSTRUCT ps{};
            HDC dc = BeginPaint(hwnd, &ps);
            if (dc) {
                RECT rc{};
                GetClientRect(hwnd, &rc);
                const int width = rc.right - rc.left;
                const int height = rc.bottom - rc.top;
                if (width > 0 && height > 0) {
                    // Paint into an off-screen bitmap, then copy in ONE operation.
                    // Direct GDI+ drawing on the visible HDC caused the popup to
                    // flash on every invalidation in the previous version.
                    HDC bufferDc = CreateCompatibleDC(dc);
                    HBITMAP buffer = bufferDc ? CreateCompatibleBitmap(dc, width, height) : nullptr;
                    if (bufferDc && buffer) {
                        HGDIOBJ original = SelectObject(bufferDc, buffer);
                        {
                            Graphics gr(bufferDc);
                            DrawPanel(gr);
                            gr.Flush(FlushIntentionSync);
                        }
                        BitBlt(dc, 0, 0, width, height, bufferDc, 0, 0, SRCCOPY);
                        SelectObject(bufferDc, original);
                    } else {
                        // Fallback only if the buffer can't be allocated.
                        Graphics gr(dc);
                        DrawPanel(gr);
                    }
                    if (buffer) DeleteObject(buffer);
                    if (bufferDc) DeleteDC(bufferDc);
                }
            }
            EndPaint(hwnd, &ps);
            return 0;
        }
        case WM_ACTIVATE:
            if (LOWORD(w) == WA_INACTIVE)
                ShowWindow(hwnd, SW_HIDE);
            return 0;
        case WM_LBUTTONUP:
            ApplyUiClick(hwnd,static_cast<short>(LOWORD(l)),
                              static_cast<short>(HIWORD(l)));
            return 0;
        case WM_KEYDOWN:
            if (w == VK_ESCAPE) { ShowWindow(hwnd, SW_HIDE); return 0; }
            break;
        case WM_ERASEBKGND: return 1;
    }
    return DefWindowProcW(hwnd, msg, w, l);
}

LRESULT CALLBACK OverlayProc(HWND hwnd, UINT msg, WPARAM w, LPARAM l) {
    switch (msg) {
        case WM_ERASEBKGND:
            return 1;
        case WM_PAINT: {
            PAINTSTRUCT ps{};
            BeginPaint(hwnd, &ps);
            EndPaint(hwnd, &ps);
            return 0;
        }
        case WM_CREATE:
            SetTimer(hwnd, TIMER_ID, 100, nullptr);
            return 0;
        case WM_TIMER:
            if (w == TIMER_ID) OnTimer();
            return 0;
        case WM_LBUTTONDOWN: {
            if (g_lockPosition) { ShowFlyout(true); return 0; }
            g_dragging = true;
            g_dragMoved = false;
            GetCursorPos(&g_dragStartCursor);
            RECT rect{};
            GetWindowRect(hwnd, &rect);
            g_dragStartWindow = {rect.left, rect.top};
            SetCapture(hwnd);
            return 0;
        }
        case WM_MOUSEMOVE:
            if (g_dragging) {
                POINT p{};
                GetCursorPos(&p);
                int dx = p.x - g_dragStartCursor.x;
                int dy = p.y - g_dragStartCursor.y;
                if (std::abs(dx) > 4 || std::abs(dy) > 4) g_dragMoved = true;
                if (g_dragMoved)
                    SetWindowPos(hwnd, nullptr,
                                 g_dragStartWindow.x + dx,
                                 g_dragStartWindow.y + dy, 0, 0,
                                 SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
            }
            return 0;
        case WM_LBUTTONUP:
            if (g_dragging) {
                g_dragging = false;
                ReleaseCapture();
                if (g_dragMoved) SaveOverlayPosition();
                else ShowFlyout(true);
            }
            return 0;
        case WM_CAPTURECHANGED:
            g_dragging = false;
            return 0;
        case WM_MOUSEWHEEL:
            // Hold Ctrl while scrolling above the flame to zoom in and out.
            if (GET_KEYSTATE_WPARAM(w) & MK_CONTROL) {
                int delta = GET_WHEEL_DELTA_WPARAM(w);
                if (delta != 0) ResizeOverlay(g_sizePercent + (delta > 0 ? 10 : -10));
                return 0;
            }
            break;
        case WM_RBUTTONUP:
            OverlayContextMenu(hwnd);
            return 0;
        case WM_RELOAD_SETTINGS:
            LoadSettings();
            if (g_flyout && IsWindowVisible(g_flyout))
                InvalidateRect(g_flyout, nullptr, FALSE);
            RenderOverlay();
            SetWindowPos(hwnd, g_alwaysOnTop ? HWND_TOPMOST : HWND_NOTOPMOST,
                         0, 0, 0, 0,
                         SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
            return 0;
        case WM_CLOSE_TOOL:
            DestroyWindow(hwnd);
            return 0;
        case WM_DESTROY:
            KillTimer(hwnd, TIMER_ID);
            if (g_flyout) { DestroyWindow(g_flyout); g_flyout = nullptr; }
            g_overlay.store(nullptr);
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProcW(hwnd, msg, w, l);
}

DWORD WINAPI UiThreadProc(LPVOID) {
    GdiplusStartupInput gdiplusInput;
    if (GdiplusStartup(&g_gdiplusToken, &gdiplusInput, nullptr) != Ok) {
        if (g_threadReady) SetEvent(g_threadReady);
        return 1;
    }
    LoadState();
    LoadSettings();
    HINSTANCE inst = GetModuleHandleW(nullptr);
    WNDCLASSW cls{};
    cls.hInstance = inst;
    cls.lpszClassName = L"S66StreakFloatWindowV2";
    cls.lpfnWndProc = OverlayProc;
    cls.hCursor = LoadCursorW(nullptr, IDC_HAND);
    if (!RegisterClassW(&cls) && GetLastError() != ERROR_CLASS_ALREADY_EXISTS) {
        if (g_threadReady) SetEvent(g_threadReady);
        GdiplusShutdown(g_gdiplusToken);
        return 2;
    }
    WNDCLASSW detail{};
    detail.hInstance = inst;
    detail.lpszClassName = L"S66StreakDetailsWindowV2";
    detail.lpfnWndProc = FlyoutProc;
    detail.hCursor = LoadCursorW(nullptr, IDC_HAND);
    if (!RegisterClassW(&detail) && GetLastError() != ERROR_CLASS_ALREADY_EXISTS) {
        UnregisterClassW(cls.lpszClassName, inst);
        if (g_threadReady) SetEvent(g_threadReady);
        GdiplusShutdown(g_gdiplusToken);
        return 3;
    }
    HWND hwnd = CreateWindowExW(WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
        cls.lpszClassName, L"S66 STREAK Floating Fire",
        WS_POPUP, 0, 0, OverlayWidth(), OverlayHeight(),
        nullptr, nullptr, inst, nullptr);
    if (!hwnd) {
        UnregisterClassW(detail.lpszClassName, inst);
        UnregisterClassW(cls.lpszClassName, inst);
        if (g_threadReady) SetEvent(g_threadReady);
        GdiplusShutdown(g_gdiplusToken);
        return 4;
    }
    g_overlay.store(hwnd);
    g_flyout = CreateWindowExW(WS_EX_TOOLWINDOW | WS_EX_TOPMOST,
        detail.lpszClassName, L"S66 STREAK Settings", WS_POPUP,
        0, 0, FLYOUT_W, FLYOUT_H, hwnd, nullptr, inst, nullptr);
    if (!g_flyout) {
        DestroyWindow(hwnd);
        UnregisterClassW(detail.lpszClassName, inst);
        UnregisterClassW(cls.lpszClassName, inst);
        if (g_threadReady) SetEvent(g_threadReady);
        GdiplusShutdown(g_gdiplusToken);
        return 5;
    }
    PlaceOverlay(true);
    RenderOverlay();
    ShowWindow(hwnd, SW_SHOWNOACTIVATE);
    if (g_threadReady) SetEvent(g_threadReady);
    MSG msg{};
    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    g_overlay.store(nullptr);
    UnregisterClassW(detail.lpszClassName, inst);
    UnregisterClassW(cls.lpszClassName, inst);
    GdiplusShutdown(g_gdiplusToken);
    return 0;
}

} // namespace

BOOL Wh_ModInit() {
    g_threadReady = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_threadReady) return FALSE;
    g_uiThread = CreateThread(nullptr, 0, UiThreadProc, nullptr, 0, nullptr);
    if (!g_uiThread) {
        CloseHandle(g_threadReady);
        g_threadReady = nullptr;
        return FALSE;
    }
    HANDLE waits[]{g_threadReady, g_uiThread};
    DWORD result = WaitForMultipleObjects(2, waits, FALSE, 4000);
    if (result != WAIT_OBJECT_0 || !g_overlay.load()) {
        WaitForSingleObject(g_uiThread, 2000);
        CloseHandle(g_uiThread);
        g_uiThread = nullptr;
        CloseHandle(g_threadReady);
        g_threadReady = nullptr;
        return FALSE;
    }
    return TRUE;
}

void Wh_ModSettingsChanged() {
    HWND hwnd = g_overlay.load();
    if (hwnd) PostMessageW(hwnd, WM_RELOAD_SETTINGS, 0, 0);
}

void Wh_ModUninit() {
    HWND hwnd = g_overlay.load();
    if (hwnd) PostMessageW(hwnd, WM_CLOSE_TOOL, 0, 0);
    if (g_uiThread) {
        WaitForSingleObject(g_uiThread, 5000);
        CloseHandle(g_uiThread);
        g_uiThread = nullptr;
    }
    if (g_threadReady) {
        CloseHandle(g_threadReady);
        g_threadReady = nullptr;
    }
}