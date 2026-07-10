// ==WindhawkMod==
// @id              desktop-draggable-transparent-widgets
// @name            Desktop Transparent Widgets
// @description     Transparent draggable Windows desktop widgets for calendar, clock, music, volume, battery, search, personalization, WiFi, Bluetooth, hotspot, and accessibility.
// @version         1.0
// @author          Jona like it, code it
// @github          https://github.com/SeniorMajor7
// @include         windhawk.exe
// @compilerOptions -ldwmapi -lgdiplus -lgdi32 -lole32 -lshell32
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*

*/
// ==/WindhawkModReadme==

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

constexpr int kDesignW = 400;
constexpr int kDesignH = 680;
constexpr UINT_PTR kTickTimer = 1;
constexpr DWORD kTimerMs = 1000;
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

struct Widget {
    int id;
    RectF rc;
    bool round;
};

Widget g_widgets[WidgetCount] = {
    {WidgetCalendar,        RectF(25, 15, 350, 170), false},
    {WidgetClock,           RectF(15, 215, 170, 170), false},
    {WidgetMusic,           RectF(215, 215, 170, 170), false},
    {WidgetVolume,          RectF(30, 415, 340, 30), false},
    {WidgetBattery,         RectF(30, 475, 340, 30), false},
    {WidgetSearch,          RectF(15, 535, 170, 30), false},
    {WidgetPersonalization, RectF(215, 535, 170, 30), false},
    {WidgetWifi,            RectF(15, 595, 70, 70), true},
    {WidgetHotspot,         RectF(115, 595, 70, 70), true},
    {WidgetBluetooth,       RectF(215, 595, 70, 70), true},
    {WidgetAccessibility,   RectF(315, 595, 70, 70), true},
};

HWND g_hwnd = nullptr;
HANDLE g_uiThread = nullptr;
DWORD g_uiThreadId = 0;
volatile bool g_unloading = false;
ULONG_PTR g_gdiplusToken = 0;
float g_scale = 1.0f;
int g_dragWidget = -1;
PointF g_dragDelta;
PointF g_dragStart;
bool g_dragMoved = false;
bool g_draggingSlider = false;
float g_volumeLevel = 1.0f;
float g_batteryLevel = 1.0f;
bool g_onBatteryCapable = false;

float Clamp(float value, float minValue, float maxValue) {
    return std::max(minValue, std::min(value, maxValue));
}

RectF Deflate(RectF rc, float x, float y) {
    rc.X += x;
    rc.Y += y;
    rc.Width -= x * 2;
    rc.Height -= y * 2;
    return rc;
}

PointF LogicalPointFromLParam(LPARAM lParam) {
    return PointF(GET_X_LPARAM(lParam) / g_scale, GET_Y_LPARAM(lParam) / g_scale);
}

bool PtInRectF(const RectF& rc, PointF pt) {
    return pt.X >= rc.X && pt.X <= rc.X + rc.Width && pt.Y >= rc.Y && pt.Y <= rc.Y + rc.Height;
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
    return RectF(rc.X + 90, rc.Y + 7.5f, 240, 15);
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
    SolidBrush text(Color(245, 255, 255, 255));
    SolidBrush muted(Color(205, 255, 255, 255));
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

    DrawTextFit(g, months[st.wMonth - 1], header, RectF(rc.X + 16, rc.Y + 10, 170, 24), text,
                StringAlignmentNear, StringAlignmentCenter);
    DrawTextFit(g, year, header, RectF(rc.X + rc.Width - 96, rc.Y + 10, 80, 24), text,
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
    const float cx = rc.X + rc.Width / 2.0f;
    const float cy = rc.Y + rc.Height / 2.0f;
    const float radius = 70.0f;
    Pen dialPen(Color(230, 255, 255, 255), 2.0f);
    Pen hourTick(Color(255, 255, 210, 38), 3.0f);
    Pen minuteTick(Color(230, 255, 255, 255), 1.2f);
    SolidBrush text(Color(245, 255, 255, 255));
    Font numFont(&family, 13, FontStyleRegular, UnitPixel);

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
    Pen handPen(Color(255, 35, 125, 255), 4.0f);
    handPen.SetStartCap(LineCapRound);
    handPen.SetEndCap(LineCapRound);
    g.DrawLine(&handPen, cx, cy, cx + std::cos(hourAngle) * 35, cy + std::sin(hourAngle) * 35);
    g.DrawLine(&handPen, cx, cy, cx + std::cos(minAngle) * 53, cy + std::sin(minAngle) * 53);
    SolidBrush handHub(Color(255, 35, 125, 255));
    FillEllipseF(g, &handHub, cx - 4.0f, cy - 4.0f, 8.0f, 8.0f);
}

void DrawMusic(Graphics& g, const Widget& widget) {
    RectF rc = widget.rc;
    SolidBrush blue(Color(235, 80, 160, 255));
    SolidBrush cyan(Color(235, 60, 220, 240));
    SolidBrush yellow(Color(255, 255, 210, 38));
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
    SolidBrush playGlyph(Color(255, 35, 35, 35));
    g.FillPolygon(&playGlyph, points, 3);
}

void DrawSlider(Graphics& g, const Widget& widget, const wchar_t* label, float value, Color fillColor, FontFamily& family) {
    RectF rc = widget.rc;
    Font font(&family, 13, FontStyleRegular, UnitPixel);
    SolidBrush text(Color(245, 255, 255, 255));
    DrawTextFit(g, label, font, RectF(rc.X + 14, rc.Y, 70, rc.Height), text, StringAlignmentNear, StringAlignmentCenter);

    RectF slider(rc.X + 90, rc.Y + 7.5f, 240, 15);
    SolidBrush track(Color(105, 255, 255, 255));
    DrawRoundedRectangle(g, slider, 7.5f, track);
    RectF filled = slider;
    filled.Width *= Clamp(value, 0.0f, 1.0f);
    SolidBrush fill(fillColor);
    DrawRoundedRectangle(g, filled, 7.5f, fill);
    SolidBrush thumb(Color(245, 255, 255, 255));
    FillEllipseF(g, &thumb, slider.X + slider.Width * Clamp(value, 0.0f, 1.0f) - 7.0f, slider.Y + 0.5f, 14.0f, 14.0f);
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
    Color yellow(255, 255, 210, 38);
    Font font(&family, 13, FontStyleRegular, UnitPixel);
    SolidBrush text(Color(245, 255, 255, 255));
    RectF iconRc(rc.X + 14, rc.Y + 5, 22, 22);
    if (kind == 0) {
        DrawSearchIcon(g, iconRc, yellow);
    } else {
        DrawBrushIcon(g, iconRc, yellow);
    }
    DrawTextFit(g, label, font, RectF(rc.X + 42, rc.Y, rc.Width - 52, rc.Height), text);
}

void DrawRoundShortcut(Graphics& g, const Widget& widget, const wchar_t* label, int kind, FontFamily& family) {
    RectF rc = widget.rc;
    Color yellow(255, 255, 210, 38);
    float cx = rc.X + rc.Width / 2.0f;
    float cy = rc.Y + 28.0f;
    if (kind == 0) {
        DrawWifiIcon(g, cx, cy - 4, yellow);
    } else if (kind == 1) {
        DrawHotspotIcon(g, cx, cy, yellow);
    } else if (kind == 2) {
        DrawBluetoothIcon(g, cx, cy, yellow);
    } else {
        DrawAccessibilityIcon(g, cx, cy, yellow);
    }

    Font font(&family, wcscmp(label, L"Accessibility") == 0 ? 8.4f : 10.0f, FontStyleRegular, UnitPixel);
    SolidBrush text(Color(245, 255, 255, 255));
    DrawTextFit(g, label, font, RectF(rc.X + 4, rc.Y + 47, rc.Width - 8, 16), text);
}

void Render() {
    if (!g_hwnd) {
        return;
    }

    int w = static_cast<int>(std::ceil(kDesignW * g_scale));
    int h = static_cast<int>(std::ceil(kDesignH * g_scale));
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
        g.ScaleTransform(g_scale, g_scale);

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
    RECT workArea = {};
    SystemParametersInfoW(SPI_GETWORKAREA, 0, &workArea, 0);
    RECT wndRect = {};
    GetWindowRect(g_hwnd, &wndRect);
    POINT pos = {wndRect.left, wndRect.top};
    if (wndRect.right <= wndRect.left) {
        pos.x = workArea.right - w - static_cast<int>(20 * g_scale);
        pos.y = workArea.top + static_cast<int>(20 * g_scale);
    }

    BLENDFUNCTION blend = {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
    UpdateLayeredWindow(g_hwnd, screenDc, &pos, &size, memDc, &src, 0, &blend, ULW_ALPHA);
    SetWindowPos(g_hwnd, HWND_BOTTOM, 0, 0, 0, 0,
                 SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOOWNERZORDER);

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
        case WidgetWifi:
            OpenUri(L"ms-settings:network-wifi");
            break;
        case WidgetHotspot:
            OpenUri(L"ms-settings:network-mobilehotspot");
            break;
        case WidgetBluetooth:
            OpenUri(L"ms-settings:bluetooth");
            break;
        case WidgetAccessibility:
            OpenUri(L"ms-settings:easeofaccess");
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
            Render();
            return 0;

        case WM_LBUTTONDOWN: {
            PointF pt = LogicalPointFromLParam(lParam);
            RectF slider = VolumeSliderRect();
            if (PtInRectF(slider, pt)) {
                g_draggingSlider = true;
                SetCapture(hwnd);
                SetSystemVolume((pt.X - slider.X) / slider.Width);
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
                SetSystemVolume((pt.X - slider.X) / slider.Width);
                Render();
                return 0;
            }

            if (g_dragWidget >= 0 && (wParam & MK_LBUTTON)) {
                Widget& widget = g_widgets[g_dragWidget];
                if (std::fabs(pt.X - g_dragStart.X) > 3.0f || std::fabs(pt.Y - g_dragStart.Y) > 3.0f) {
                    g_dragMoved = true;
                }
                widget.rc.X = Clamp(pt.X - g_dragDelta.X, 0.0f, kDesignW - widget.rc.Width);
                widget.rc.Y = Clamp(pt.Y - g_dragDelta.Y, 0.0f, kDesignH - widget.rc.Height);
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

        case WM_NCHITTEST:
            return HTCLIENT;

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
    HINSTANCE instance = GetModuleHandleW(nullptr);

    WNDCLASSEXW wc = {};
    wc.cbSize = sizeof(wc);
    wc.hInstance = instance;
    wc.lpfnWndProc = WidgetWndProc;
    wc.lpszClassName = kClassName;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    RegisterClassExW(&wc);

    RECT workArea = {};
    SystemParametersInfoW(SPI_GETWORKAREA, 0, &workArea, 0);
    int w = static_cast<int>(std::ceil(kDesignW * g_scale));
    int h = static_cast<int>(std::ceil(kDesignH * g_scale));
    int x = workArea.right - w - static_cast<int>(20 * g_scale);
    int y = workArea.top + static_cast<int>(20 * g_scale);

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

BOOL Wh_ModInit() {
    g_unloading = false;
    g_uiThread = CreateThread(nullptr, 0, UiThreadProc, nullptr, 0, &g_uiThreadId);
    return g_uiThread != nullptr;
}

void Wh_ModUninit() {
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
