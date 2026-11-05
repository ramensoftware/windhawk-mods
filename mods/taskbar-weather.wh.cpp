// ==WindhawkMod==
// @id taskbar-weather
// @name Taskbar Weather
// @description Current weather on the taskbar with a details card on hover. No Widgets, browser or API key needed
// @version 1.4.0
// @author DavidHiFi
// @github https://github.com/DavidHiFi
// @homepage https://github.com/DavidHiFi/davids-windhawk-mods/tree/main/mods/local/taskbar-weather
// @license MIT
// @include explorer.exe
// @architecture x86-64
// @compilerOptions -lwinhttp -lgdiplus -lgdi32 -luser32 -ld2d1 -ldwrite
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Taskbar Weather

The current weather on the left side of the taskbar, with a details card when
you hover over it. It works on its own, without Windows Widgets.

![Taskbar Weather preview](https://raw.githubusercontent.com/DavidHiFi/davids-windhawk-mods/main/media/previews/taskbar-weather.png)

## Features

- **Weather icon, temperature and conditions** on the taskbar, updated every
  ten minutes by default.
- **Hover card** with the feels-like temperature, today's high and low,
  humidity and wind.
- **Click to refresh** at any time.
- **No Widgets, MSN, Edge or location permission.** Data comes from
  Open-Meteo, with no account or API key.
- **Adjustable** position, width, font and size, in Catppuccin Mocha colors.

## Setup

Open the settings and enter your town's **latitude** and **longitude** (in most
map apps, right-click a place to copy them). Add a **place name** to show it in
the hover card. The coordinates are saved locally and sent to Open-Meteo with each request.
Internet access is required; Open-Meteo also receives your IP address.

Temperatures are in Celsius. If a request fails, the last reading stays and
the mod retries a minute later; readings older than 30 minutes are marked in
the card. Turn off Windows Widgets to avoid a second weather button.

## Credits

Weather data by [Open-Meteo.com](https://open-meteo.com/) (CC BY 4.0). MIT.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- latitude: ""
  $name: Latitude
- longitude: ""
  $name: Longitude
- placeName: ""
  $name: Place name
  $description: Shown in the hover card. Leave empty to hide it.
- updateMinutes: 10
  $name: Update interval in minutes
- leftOffset: 16
  $name: Left offset
- width: 220
  $name: Width
- fontFamily: Segoe UI
  $name: Font
- fontSize: 11
  $name: Font size
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <winhttp.h>
#include <gdiplus.h>
#include <d2d1.h>
#include <dwrite.h>
#include <wrl/client.h>
#include <string>
#include <mutex>
#include <atomic>
#include <cmath>
#include <cstdlib>
#include <algorithm>
#include <sstream>
#include <ctime>

using namespace Gdiplus;
using Microsoft::WRL::ComPtr;
namespace {
constexpr wchar_t kClass[] = L"WindhawkIndependentWeather";
constexpr wchar_t kCardClass[] = L"WindhawkIndependentWeatherCard";
constexpr wchar_t kControllerClass[] = L"WindhawkWeatherLifecycle";
constexpr wchar_t kRightProperty[]=L"WindhawkTaskbarWeatherRightDip";
// WM_APP+2 shows the card for eight seconds without moving the mouse, for audits.
constexpr UINT kDataMessage=WM_APP+1, kPreviewMessage=WM_APP+2;
constexpr UINT_PTR kLayoutTimer=1, kPreviewTimer=2, kFadeTimer=3;
constexpr float kCardWidth=320, kShadow=12, kPad=18;
// Taskbar row layout in logical pixels: glyph slot, gap, trailing room.
constexpr float kIconLeft=10, kIconGap=8, kTextRightPad=6;
// Catppuccin Mocha
constexpr ARGB kBase=0xFF1E1E2E, kSurface1=0xFF45475A, kOverlay1=0xFF7F849C, kSubtext0=0xFFA6ADC8,
    kText=0xFFCDD6F4, kBlue=0xFF89B4FA, kYellow=0xFFF9E2AF, kPeach=0xFFFAB387;
HANDLE stopEvent, refreshEvent, uiThread, fetchThread;
std::atomic<HWND> weatherWindow;
std::atomic<HWND> controllerWindow;
HWND cardWindow;
HINSTANCE instance;
ULONG_PTR graphicsToken;
ComPtr<ID2D1Factory> d2dFactory;
ComPtr<IDWriteFactory> writeFactory;
std::mutex dataMutex;
std::wstring latitude, longitude, place, fontFamily;
int interval=10, left=16, width=220, fontSize=12;
bool hover=false, preview=false;
BYTE cardAlpha=0;
struct Reading {
    double temperature=0, feels=0, humidity=0, wind=0, windDirection=0, high=0, low=0;
    int code=0; bool day=true, details=false, daily=false, valid=false, failed=false;
    ULONGLONG received=0; unsigned updates=0; std::wstring time;
} reading;

Color Mocha(ARGB c) { return Color(c); }
Color WithAlpha(ARGB c,BYTE a) { return Color(a,(c>>16)&255,(c>>8)&255,c&255); }
std::wstring Setting(const wchar_t* key) {
    PCWSTR s=Wh_GetStringSetting(key); std::wstring result=s?s:L"";
    Wh_FreeStringSetting(s); return result;
}
bool Coordinate(const std::wstring& s, double bound) {
    if(s.empty()) return false;
    wchar_t* end=nullptr; double v=wcstod(s.c_str(), &end);
    return end && !*end && std::isfinite(v) && std::abs(v)<=bound;
}
bool LocationConfigured() { return Coordinate(latitude,90)&&Coordinate(longitude,180); }
void LoadSettings() {
    latitude=Setting(L"latitude"); longitude=Setting(L"longitude"); place=Setting(L"placeName");
    fontFamily=Setting(L"fontFamily"); if(fontFamily.empty()) fontFamily=L"Segoe UI";
    interval=std::clamp(Wh_GetIntSetting(L"updateMinutes"),1,60);
    left=std::clamp(Wh_GetIntSetting(L"leftOffset"),0,1000);
    width=std::clamp(Wh_GetIntSetting(L"width"),120,400);
    fontSize=std::clamp(Wh_GetIntSetting(L"fontSize"),9,22);
}
// Keep the last successful reading across a mod reload or a short network outage.
// A coordinate match prevents a previous town's cache from appearing after relocation.
void LoadCachedReading() {
    wchar_t buffer[1024]{};if(!Wh_GetStringValue(L"lastWeatherV1",buffer,1024))return;
    std::wistringstream input(buffer);std::wstring lat,lon;long long saved=0;Reading cached;
    if(!(input>>lat>>lon>>saved>>cached.temperature>>cached.feels>>cached.humidity>>cached.wind>>cached.windDirection>>cached.high>>cached.low>>cached.code>>cached.day>>cached.details>>cached.daily>>cached.time))return;
    long long age=static_cast<long long>(std::time(nullptr))-saved;
    if(lat!=latitude||lon!=longitude||age<0||age>86400||!std::isfinite(cached.temperature)||cached.temperature< -100||cached.temperature>70)return;
    cached.valid=true;cached.received=GetTickCount64()-static_cast<ULONGLONG>(age)*1000;
    reading=cached;
}
void SaveCachedReading(const Reading& r) {
    std::wostringstream out;out.precision(15);
    out<<latitude<<L' '<<longitude<<L' '<<std::time(nullptr)<<L' '<<r.temperature<<L' '<<r.feels<<L' '<<r.humidity<<L' '<<r.wind<<L' '<<r.windDirection<<L' '<<r.high<<L' '<<r.low<<L' '<<r.code<<L' '<<r.day<<L' '<<r.details<<L' '<<r.daily<<L' '<<r.time;
    Wh_SetStringValue(L"lastWeatherV1",out.str().c_str());
}
const wchar_t* Condition(int code) {
    switch(code) {
    case 0:return L"Clear"; case 1:return L"Mostly clear";
    case 2:return L"Partly cloudy"; case 3:return L"Cloudy";
    case 45:case 48:return L"Fog";
    case 51:case 53:case 55:case 56:case 57:return L"Drizzle";
    case 61:case 63:case 65:case 66:case 67:return L"Rain";
    case 71:case 73:case 75:case 77:return L"Snow";
    case 80:case 81:case 82:return L"Rain showers";
    case 85:case 86:return L"Snow showers";
    case 95:case 96:case 99:return L"Thunderstorms";
    default:return L"Weather";
    }
}
const wchar_t* Compass(double degrees) {
    static const wchar_t* names[]{L"N",L"NNE",L"NE",L"ENE",L"E",L"ESE",L"SE",L"SSE",L"S",L"SSW",L"SW",L"WSW",L"W",L"WNW",L"NW",L"NNW"};
    return names[(int)std::lround(std::fmod(std::abs(degrees),360.)/22.5)%16];
}
std::wstring Whole(double v) { return std::to_wstring((int)std::lround(v)); }
std::wstring Label(const Reading& r) {
    if(!LocationConfigured())return L"Set weather location";
    return r.valid?Whole(r.temperature)+L"°C  "+Condition(r.code):(r.failed?L"Weather unavailable":L"Loading weather...");
}
// Open-Meteo returns local ISO time such as 2026-09-30T08:45.
std::wstring Clock(const std::wstring& iso) {
    if(iso.size()<16||iso[13]!=L':')return iso;
    int h=_wtoi(iso.substr(11,2).c_str()),m=_wtoi(iso.substr(14,2).c_str());
    return std::to_wstring(h%12?h%12:12)+(m<10?L":0":L":")+std::to_wstring(m)+(h<12?L" am":L" pm");
}
bool Number(const std::string& s,const char* name,double& value) {
    size_t p=s.find(std::string("\"")+name+"\""); if(p==s.npos)return false;
    p=s.find(':',p); if(p==s.npos)return false;
    // Daily values arrive as one-element arrays.
    p=s.find_first_not_of(" [",p+1); if(p==s.npos)return false;
    const char* begin=s.c_str()+p; char* end;
    value=strtod(begin,&end);return end!=begin && std::isfinite(value);
}
bool Fetch(Reading& next) {
    const ULONGLONG deadline=GetTickCount64()+30000;
    if(!Coordinate(latitude,90)||!Coordinate(longitude,180))return false;
    std::wstring path=L"/v1/forecast?latitude="+latitude+L"&longitude="+longitude+
        L"&current=temperature_2m,apparent_temperature,relative_humidity_2m,weather_code,is_day,wind_speed_10m,wind_direction_10m"
        L"&daily=temperature_2m_max,temperature_2m_min&forecast_days=1&timezone=auto";
    HINTERNET session=WinHttpOpen(L"WindhawkTaskbarWeather/1.1",WINHTTP_ACCESS_TYPE_AUTOMATIC_PROXY,nullptr,nullptr,0);
    if(!session)return false;
    WinHttpSetTimeouts(session,5000,5000,5000,5000);
    HINTERNET connection=WinHttpConnect(session,L"api.open-meteo.com",INTERNET_DEFAULT_HTTPS_PORT,0);
    HINTERNET request=connection?WinHttpOpenRequest(connection,L"GET",path.c_str(),nullptr,WINHTTP_NO_REFERER,WINHTTP_DEFAULT_ACCEPT_TYPES,WINHTTP_FLAG_SECURE):nullptr;
    bool ok=request && WinHttpSendRequest(request,WINHTTP_NO_ADDITIONAL_HEADERS,0,WINHTTP_NO_REQUEST_DATA,0,0,0) && WinHttpReceiveResponse(request,nullptr);
    DWORD status=0,len=sizeof(status);
    if(ok)ok=WinHttpQueryHeaders(request,WINHTTP_QUERY_STATUS_CODE|WINHTTP_QUERY_FLAG_NUMBER,nullptr,&status,&len,nullptr)&&status==200;
    std::string body;
    while(ok){if(GetTickCount64()>deadline || WaitForSingleObject(stopEvent,0)==WAIT_OBJECT_0){ok=false;break;}DWORD available=0,read=0; if(!WinHttpQueryDataAvailable(request,&available)){ok=false;break;} if(!available)break;
        if(body.size()+available>65536){ok=false;break;}
        size_t old=body.size();body.resize(old+available);
        if(!WinHttpReadData(request,body.data()+old,available,&read)){ok=false;break;}body.resize(old+read);
    }
    if(request)WinHttpCloseHandle(request);if(connection)WinHttpCloseHandle(connection);WinHttpCloseHandle(session);
    if(!ok || WaitForSingleObject(stopEvent,0)==WAIT_OBJECT_0)return false;
    size_t p=body.find("\"current\":"); if(p==body.npos)return false;
    auto current=body.substr(p); double temp,code,day;
    if(!Number(current,"temperature_2m",temp)||!Number(current,"weather_code",code)||!Number(current,"is_day",day)||temp< -100||temp>70)return false;
    next.temperature=temp;next.code=(int)code;next.day=day!=0;next.valid=true;next.failed=false;next.received=GetTickCount64();
    next.details=Number(current,"apparent_temperature",next.feels)&&Number(current,"relative_humidity_2m",next.humidity)&&
        Number(current,"wind_speed_10m",next.wind)&&Number(current,"wind_direction_10m",next.windDirection);
    p=body.find("\"daily\":");
    if(p!=body.npos){auto daily=body.substr(p);next.daily=Number(daily,"temperature_2m_max",next.high)&&Number(daily,"temperature_2m_min",next.low);}
    p=current.find("\"time\":\"");if(p!=current.npos){p+=8;auto end=current.find('"',p);auto time=current.substr(p,end-p);next.time=std::wstring(time.begin(),time.end());}
    return true;
}
void UpdateStatus() {
    Reading r;{std::lock_guard lock(dataMutex);r=reading;}
    std::wstring status=r.valid?L"Independent weather | "+Whole(r.temperature)+L" C | "+Condition(r.code)+L" | data "+r.time+L" | updates "+std::to_wstring(r.updates):L"Independent weather | waiting for data";
    if(r.details)status+=L" | feels "+Whole(r.feels)+L" | humidity "+Whole(r.humidity)+L" | wind "+Whole(r.wind)+L" "+Compass(r.windDirection);
    if(r.daily)status+=L" | high "+Whole(r.high)+L" | low "+Whole(r.low);
    if(r.failed)status+=L" | request failed";
    SetWindowTextW(weatherWindow,status.c_str());
}
void RoundedRect(GraphicsPath& path,float x,float y,float w,float h,float r) {
    float d=r*2;
    path.AddArc(x,y,d,d,180,90);path.AddArc(x+w-d,y,d,d,270,90);
    path.AddArc(x+w-d,y+h-d,d,d,0,90);path.AddArc(x,y+h-d,d,d,90,90);path.CloseFigure();
}
// Draws the weather glyph in a 24 by 22 logical-pixel box scaled by `scale`.
void DrawIcon(Graphics& g,float x,float y,float scale,int code,bool day) {
    auto state=g.Save();g.TranslateTransform(x,y);g.ScaleTransform(scale,scale);
    SolidBrush sun(Mocha(kYellow)),cloud(Mocha(kText));
    if(code<3){g.FillEllipse(&sun,0.f,0.f,14.f,14.f);if(!day){SolidBrush cut(Mocha(kBase));g.FillEllipse(&cut,5.f,-2.f,12.f,12.f);}}
    if(code!=0){g.FillEllipse(&cloud,3.f,6.f,10.f,10.f);g.FillEllipse(&cloud,9.f,3.f,12.f,12.f);g.FillEllipse(&cloud,16.f,7.f,8.f,9.f);g.FillRectangle(&cloud,7.f,11.f,13.f,5.f);}
    if(code>=51){Pen drops(Mocha(kBlue),1.5f);g.DrawLine(&drops,9.f,18.f,7.f,21.f);g.DrawLine(&drops,18.f,18.f,16.f,21.f);}
    g.Restore(state);
}
// Ink bounds of the glyph inside the 24 by 22 box that DrawIcon fills, so the
// row can centre the glyph itself instead of the box and keep one gap to the text.
float IconInkWidth(int code) { return code==0 ? 14.f : 24.f; }
float IconInkMid(int code) {
    if(code==0) return 7.f;          // sun or moon: 0 to 14
    if(code<3) return 8.f;           // sun behind cloud: 0 to 16
    return code>=51 ? 12.f : 9.5f;   // cloud: 3 to 16, drops reach 21
}
float TextX(int code) { return kIconLeft+IconInkWidth(code)+kIconGap; }
// Row text goes through DirectWrite, like the XAML system info beside it.
// GDI+ DrawString hints small glyphs coarsely on a transparent layered window.
struct RowFont { ComPtr<IDWriteTextFormat> format; float ascent=0, capHeight=0, lineSpacing=0; };
RowFont ResolveRowFont() {
    RowFont result;
    if(!writeFactory&&FAILED(DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED,__uuidof(IDWriteFactory),(IUnknown**)writeFactory.GetAddressOf())))return result;
    ComPtr<IDWriteFontCollection> fonts;if(FAILED(writeFactory->GetSystemFontCollection(&fonts)))return result;
    // Prefer a real semibold face; older font installs expose it as a separate "SemBd" family.
    std::wstring family;DWRITE_FONT_WEIGHT weight=DWRITE_FONT_WEIGHT_SEMI_BOLD;ComPtr<IDWriteFont> font;
    auto find=[&](const std::wstring& name,DWRITE_FONT_WEIGHT want)->bool {
        UINT32 index;BOOL exists=FALSE;ComPtr<IDWriteFontFamily> f;ComPtr<IDWriteFont> match;
        if(FAILED(fonts->FindFamilyName(name.c_str(),&index,&exists))||!exists||FAILED(fonts->GetFontFamily(index,&f)))return false;
        if(FAILED(f->GetFirstMatchingFont(want,DWRITE_FONT_STRETCH_NORMAL,DWRITE_FONT_STYLE_NORMAL,&match)))return false;
        family=name;weight=want;font=match;return true;
    };
    bool semibold=find(fontFamily,DWRITE_FONT_WEIGHT_SEMI_BOLD)&&std::abs((int)font->GetWeight()-600)<=50;
    if(!semibold&&!find(fontFamily+L" SemBd",DWRITE_FONT_WEIGHT_NORMAL)&&!find(fontFamily,DWRITE_FONT_WEIGHT_SEMI_BOLD))find(L"Segoe UI Variable Text",DWRITE_FONT_WEIGHT_SEMI_BOLD);
    if(!font||FAILED(writeFactory->CreateTextFormat(family.c_str(),nullptr,weight,DWRITE_FONT_STYLE_NORMAL,DWRITE_FONT_STRETCH_NORMAL,(FLOAT)fontSize,L"",&result.format)))return RowFont{};
    result.format->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);result.format->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
    ComPtr<IDWriteInlineObject> ellipsis;DWRITE_TRIMMING trimming{DWRITE_TRIMMING_GRANULARITY_CHARACTER,0,0};
    if(SUCCEEDED(writeFactory->CreateEllipsisTrimmingSign(result.format.Get(),&ellipsis)))result.format->SetTrimming(&trimming,ellipsis.Get());
    DWRITE_FONT_METRICS m;font->GetMetrics(&m);float unit=(float)fontSize/m.designUnitsPerEm;
    result.ascent=m.ascent*unit;result.capHeight=m.capHeight*unit;result.lineSpacing=(m.ascent+m.descent+m.lineGap)*unit;
    return result;
}
float TextWidth(const RowFont& font,const std::wstring& s) {
    ComPtr<IDWriteTextLayout> layout;DWRITE_TEXT_METRICS m;
    if(!font.format||FAILED(writeFactory->CreateTextLayout(s.c_str(),(UINT32)s.size(),font.format.Get(),10000.f,100.f,&layout))||FAILED(layout->GetMetrics(&m)))return 0;
    return m.widthIncludingTrailingWhitespace;
}
// Draws the temperature and conditions over the GDI+ icon layer already in `dc`.
void DrawRowText(HDC dc,int w,int h,float scale,const Reading& r) {
    RowFont font=ResolveRowFont();if(!font.format)return;
    if(!d2dFactory&&FAILED(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED,d2dFactory.GetAddressOf())))return;
    auto props=D2D1::RenderTargetProperties(D2D1_RENDER_TARGET_TYPE_DEFAULT,D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM,D2D1_ALPHA_MODE_PREMULTIPLIED),96.f*scale,96.f*scale);
    ComPtr<ID2D1DCRenderTarget> target;RECT bounds{0,0,w,h};
    if(FAILED(d2dFactory->CreateDCRenderTarget(&props,&target))||FAILED(target->BindDC(dc,&bounds)))return;
    // Transparent pixels cannot take ClearType, so use symmetric grayscale smoothing.
    ComPtr<IDWriteRenderingParams> base,params;
    if(SUCCEEDED(writeFactory->CreateRenderingParams(&base))&&SUCCEEDED(writeFactory->CreateCustomRenderingParams(base->GetGamma(),base->GetEnhancedContrast(),0.f,DWRITE_PIXEL_GEOMETRY_FLAT,DWRITE_RENDERING_MODE_NATURAL_SYMMETRIC,&params)))target->SetTextRenderingParams(params.Get());
    target->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE);
    ComPtr<ID2D1SolidColorBrush> brush;
    float height=h/scale,textX=TextX(r.code),textWidth=w/scale-textX-kTextRightPad;
    auto snap=[&](float v){return std::round(v*scale)/scale;};
    auto line=[&](const std::wstring& s,float baseline) {
        ComPtr<IDWriteTextLayout> layout;if(FAILED(writeFactory->CreateTextLayout(s.c_str(),(UINT32)s.size(),font.format.Get(),textWidth,height,&layout)))return;
        DWRITE_LINE_METRICS m;UINT32 count=0;float offset=font.ascent;if(SUCCEEDED(layout->GetLineMetrics(&m,1,&count))&&count)offset=m.baseline;
        target->DrawTextLayout(D2D1::Point2F(textX,baseline-offset),layout.Get(),brush.Get(),D2D1_DRAW_TEXT_OPTIONS_NONE);
    };
    target->BeginDraw();
    if(SUCCEEDED(target->CreateSolidColorBrush(D2D1::ColorF(kText&0xFFFFFF),&brush))) {
        if(r.valid) {
            // Centre the ink, from the first line's cap height to the second baseline, on whole pixels.
            float pitch=std::min(std::ceil(font.lineSpacing*scale)/scale,height/2.f);
            float first=snap(height/2.f+(font.capHeight-pitch)/2.f);
            line(Whole(r.temperature)+L"°C",first);
            line(Condition(r.code),first+pitch);
        } else {
            line(Label(r),snap((height+font.capHeight)/2.f));
        }
    }
    target->EndDraw();
}
void Paint() {
    if(!weatherWindow)return;
    RECT client;GetClientRect(weatherWindow,&client);int w=client.right,h=client.bottom;if(!w||!h)return;
    HDC dc=GetDC(nullptr),mem=CreateCompatibleDC(dc);BITMAPINFO info{};info.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);info.bmiHeader.biWidth=w;info.bmiHeader.biHeight=-h;info.bmiHeader.biPlanes=1;info.bmiHeader.biBitCount=32;info.bmiHeader.biCompression=BI_RGB;
    void* bits;HBITMAP bitmap=CreateDIBSection(dc,&info,DIB_RGB_COLORS,&bits,nullptr,0);auto old=SelectObject(mem,bitmap);memset(bits,0,w*h*4);
    Reading r;{std::lock_guard lock(dataMutex);r=reading;}
    float scale=GetDpiForWindow(weatherWindow)/96.f;
    {
        Bitmap canvas(w,h,w*4,PixelFormat32bppPARGB,(BYTE*)bits);
        Graphics g(&canvas);g.SetSmoothingMode(SmoothingModeAntiAlias);
        g.ScaleTransform(scale,scale);float height=h/scale;
        // Inset the highlight so its antialiased edge stays inside the taskbar pill.
        if(hover||preview){GraphicsPath path;RoundedRect(path,2.f,3.f,w/scale-4,height-6,6.f);SolidBrush bg(WithAlpha(kSurface1,150));g.FillPath(&bg,&path);}
        DrawIcon(g,kIconLeft,height/2-IconInkMid(r.code),1,r.code,r.day);
    }
    GdiFlush();
    DrawRowText(mem,w,h,scale,r);
    POINT dest{},src{};SIZE size{w,h};BLENDFUNCTION blend{AC_SRC_OVER,0,255,AC_SRC_ALPHA};
    UpdateLayeredWindow(weatherWindow,dc,nullptr,&size,mem,&src,0,&blend,ULW_ALPHA);
    SelectObject(mem,old);DeleteObject(bitmap);DeleteDC(mem);ReleaseDC(nullptr,dc);
}
// Lays out the hover card in logical pixels and returns its height. Draws only when `g` is set.
float Card(Graphics* g,const Reading& r) {
    FontFamily family(fontFamily.c_str());const FontFamily* face=family.IsAvailable()?&family:FontFamily::GenericSansSerif();
    Font hero(face,38,FontStyleRegular,UnitPixel),heading(face,16,FontStyleBold,UnitPixel),body(face,14,FontStyleRegular,UnitPixel),small(face,13,FontStyleRegular,UnitPixel);
    SolidBrush text(Mocha(kText)),muted(Mocha(kSubtext0)),faint(Mocha(kOverlay1)),warn(Mocha(kPeach));
    StringFormat format(StringFormat::GenericTypographic());
    format.SetLineAlignment(StringAlignmentCenter);format.SetFormatFlags(StringFormatFlagsNoWrap|StringFormatFlagsMeasureTrailingSpaces);format.SetTrimming(StringTrimmingEllipsisCharacter);
    float y=kPad;
    auto put=[&](const std::wstring& s,const Font& font,Brush& brush,float x,float height)->float {
        if(!g)return 0;
        RectF box(x,y,kCardWidth-kPad-x,height),bounds;
        g->DrawString(s.c_str(),-1,&font,box,&format,&brush);g->MeasureString(s.c_str(),-1,&font,box,&format,&bounds);
        return bounds.Width;
    };
    auto divider=[&]{y+=8;if(g){Pen line(Mocha(kSurface1),1.f);g->DrawLine(&line,kPad,y,kCardWidth-kPad,y);}y+=10;};
    auto pair=[&](const std::wstring& label,const std::wstring& value,float x){
        float labelWidth=put(label,body,muted,x,22);put(value,body,text,x+labelWidth+8,22);
    };
    if(r.valid){
        if(g)DrawIcon(*g,kPad,y+6,1.6f,r.code,r.day);
        float x=kPad+50;
        auto top=y;x+=put(Whole(r.temperature)+L"°",hero,text,x,50)+12;
        put(Condition(r.code),heading,text,x,26);y+=26;
        put(r.details?L"Feels like "+Whole(r.feels)+L"°":L"Celsius",body,muted,x,22);y=top+52;
        if(r.daily||r.details){
            divider();
            if(r.daily){pair(L"High",Whole(r.high)+L"°",kPad);pair(L"Low",Whole(r.low)+L"°",kCardWidth/2+4);y+=26;}
            if(r.details){pair(L"Humidity",Whole(r.humidity)+L"%",kPad);pair(L"Wind",Whole(r.wind)+L" km/h "+Compass(r.windDirection),kCardWidth/2+4);y+=26;}
            y-=4;
        }
    } else {put(!LocationConfigured()?L"Set weather location":(r.failed?L"Weather unavailable":L"Loading weather…"),heading,text,kPad,28);y+=28;}
    divider();
    std::wstring footer=place.empty()?L"":place+L"  ·  ";
    footer+=r.valid?L"Updated "+Clock(r.time):(LocationConfigured()?L"Waiting for Open-Meteo":L"Choose your town in mod Settings");
    put(footer,small,muted,kPad,20);y+=22;
    if(r.valid&&r.failed){put(L"Last update failed. Retrying in a minute.",small,warn,kPad,20);y+=22;}
    if(r.valid&&GetTickCount64()-r.received>30*60*1000){put(L"Stale reading. Connection unavailable.",small,warn,kPad,20);y+=22;}
    put(L"Open-Meteo  ·  Modeled weather",small,muted,kPad,20);y+=22;
    put(LocationConfigured()?L"Click weather to refresh":L"Set latitude and longitude in Settings",small,muted,kPad,20);y+=22;
    return y+kPad-6;
}
void RenderCard() {
    HWND weather=weatherWindow;if(!cardWindow||!weather)return;
    Reading r;{std::lock_guard lock(dataMutex);r=reading;}
    float scale=GetDpiForWindow(weather)/96.f,height=Card(nullptr,r);
    int w=(int)std::ceil((kCardWidth+2*kShadow)*scale),h=(int)std::ceil((height+2*kShadow)*scale);
    RECT anchor,bar;GetWindowRect(weather,&anchor);HWND taskbar=FindWindowW(L"Shell_TrayWnd",nullptr);if(!taskbar||!GetWindowRect(taskbar,&bar))bar=anchor;
    MONITORINFO monitor{sizeof(monitor)};GetMonitorInfoW(MonitorFromWindow(weather,MONITOR_DEFAULTTONEAREST),&monitor);
    int margin=(int)std::lround(kShadow*scale),gap=(int)std::lround(8*scale);
    // Open above a bottom taskbar and below a top one, aligned with the weather's left edge.
    bool bottom=bar.top>(monitor.rcMonitor.top+monitor.rcMonitor.bottom)/2;
    POINT dest{std::clamp(anchor.left-margin,monitor.rcMonitor.left-margin,monitor.rcMonitor.right-w+margin),bottom?bar.top-gap-h+margin:bar.bottom+gap-margin};
    HDC dc=GetDC(nullptr),mem=CreateCompatibleDC(dc);BITMAPINFO info{};info.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);info.bmiHeader.biWidth=w;info.bmiHeader.biHeight=-h;info.bmiHeader.biPlanes=1;info.bmiHeader.biBitCount=32;info.bmiHeader.biCompression=BI_RGB;
    void* bits;HBITMAP bitmap=CreateDIBSection(dc,&info,DIB_RGB_COLORS,&bits,nullptr,0);auto old=SelectObject(mem,bitmap);memset(bits,0,w*h*4);
    {
        Bitmap canvas(w,h,w*4,PixelFormat32bppPARGB,(BYTE*)bits);
        Graphics g(&canvas);g.SetSmoothingMode(SmoothingModeAntiAlias);g.SetTextRenderingHint(TextRenderingHintAntiAliasGridFit);
        g.ScaleTransform(scale,scale);g.TranslateTransform(kShadow,kShadow);
        for(int i=10;i>=1;i--){GraphicsPath shade;RoundedRect(shade,-(float)i,2.f-i,kCardWidth+2*i,height+2*i,8.f+i);SolidBrush brush(Color(6,0,0,0));g.FillPath(&brush,&shade);}
        GraphicsPath fill;RoundedRect(fill,0,0,kCardWidth,height,8);SolidBrush base(WithAlpha(kBase,246));g.FillPath(&base,&fill);
        GraphicsPath edge;RoundedRect(edge,0.5f,0.5f,kCardWidth-1,height-1,7.5f);Pen border(Mocha(kSurface1),1.f);g.DrawPath(&border,&edge);
        Card(&g,r);
    }
    POINT src{};SIZE size{w,h};BLENDFUNCTION blend{AC_SRC_OVER,0,cardAlpha,AC_SRC_ALPHA};
    UpdateLayeredWindow(cardWindow,dc,&dest,&size,mem,&src,0,&blend,ULW_ALPHA);
    SelectObject(mem,old);DeleteObject(bitmap);DeleteDC(mem);ReleaseDC(nullptr,dc);
}
void ShowCard() {
    if(!cardWindow)return;
    bool visible=IsWindowVisible(cardWindow);if(!visible)cardAlpha=0;
    RenderCard();
    if(!visible){ShowWindow(cardWindow,SW_SHOWNOACTIVATE);SetWindowPos(cardWindow,HWND_TOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE|SWP_NOACTIVATE);SetTimer(weatherWindow,kFadeTimer,15,nullptr);}
}
void HideCard() { if(cardWindow){KillTimer(weatherWindow,kFadeTimer);ShowWindow(cardWindow,SW_HIDE);} }
int ContentWidth() {
    Reading r;{std::lock_guard lock(dataMutex);r=reading;}
    RowFont font=ResolveRowFont();
    float textWidth=TextWidth(font,r.valid?Whole(r.temperature)+L"°C":Label(r));
    if(r.valid) textWidth=std::max(textWidth,TextWidth(font,Condition(r.code)));
    return std::clamp((int)std::ceil(textWidth)+(int)TextX(r.code)+(int)kTextRightPad,80,width);
}
void Layout(HWND hwnd) {
    HWND parent=FindWindowW(L"Shell_TrayWnd",nullptr);if(!parent)return;
    RECT r;GetClientRect(parent,&r);int dpi=GetDpiForWindow(parent);int contentWidth=ContentWidth();int x=MulDiv(left,dpi,96),w=MulDiv(contentWidth,dpi,96),padding=MulDiv(4,dpi,96);
    if(GetParent(hwnd)!=parent)SetParent(hwnd,parent);
    SetWindowPos(hwnd,HWND_TOP,x,padding,w,std::max(1L,r.bottom-2*padding),SWP_NOACTIVATE|SWP_SHOWWINDOW);
    SetPropW(parent,kRightProperty,(HANDLE)(INT_PTR)(left+contentWidth));Paint();UpdateStatus();
}
LRESULT CALLBACK WindowProc(HWND hwnd,UINT msg,WPARAM wp,LPARAM lp) {
    switch(msg){
    case WM_MOUSEMOVE:if(!hover){hover=true;TRACKMOUSEEVENT track{sizeof(track),TME_LEAVE|TME_HOVER,hwnd,350};TrackMouseEvent(&track);Paint();}break;
    case WM_MOUSEHOVER:ShowCard();return 0;
    case WM_MOUSELEAVE:hover=false;if(!preview)HideCard();Paint();return 0;
    case WM_LBUTTONUP:SetEvent(refreshEvent);return 0;
    case kDataMessage:UpdateStatus();Paint();if(IsWindowVisible(cardWindow))RenderCard();return 0;
    case kPreviewMessage:preview=true;ShowCard();Paint();SetTimer(hwnd,kPreviewTimer,8000,nullptr);return 0;
    case WM_TIMER:
        if(wp==kFadeTimer){
            cardAlpha=(BYTE)std::min(255,cardAlpha+51);BLENDFUNCTION blend{AC_SRC_OVER,0,cardAlpha,AC_SRC_ALPHA};
            UpdateLayeredWindow(cardWindow,nullptr,nullptr,nullptr,nullptr,nullptr,0,&blend,ULW_ALPHA);
            if(cardAlpha==255)KillTimer(hwnd,kFadeTimer);
        } else if(wp==kPreviewTimer){KillTimer(hwnd,kPreviewTimer);preview=false;if(!hover)HideCard();Paint();}
        else Layout(hwnd);
        return 0;
    case WM_CLOSE:DestroyWindow(hwnd);return 0;
    case WM_DESTROY:RemovePropW(GetParent(hwnd),kRightProperty);HideCard();weatherWindow=nullptr;return 0;
    }
    return DefWindowProcW(hwnd,msg,wp,lp);
}
// The controller owns the message loop. Taskbar child destruction must not end it.
// Its normal layout timer also covers Explorer creating the taskbar after injection.
void EnsureWeatherWindow() {
    if(WaitForSingleObject(stopEvent,0)==WAIT_OBJECT_0)return;
    HWND taskbar=FindWindowW(L"Shell_TrayWnd",nullptr);
    RECT bounds{};
    if(!taskbar || !GetClientRect(taskbar,&bounds) || bounds.bottom<24)return;
    if(!IsWindow(weatherWindow)) {
        weatherWindow=CreateWindowExW(WS_EX_LAYERED|WS_EX_NOACTIVATE,kClass,L"Independent taskbar weather",WS_CHILD|WS_VISIBLE,16,4,220,38,taskbar,nullptr,instance,nullptr);
        if(weatherWindow)Wh_Log(L"Weather taskbar child created");
    }
    if(weatherWindow)Layout(weatherWindow);
}
LRESULT CALLBACK ControllerProc(HWND hwnd,UINT msg,WPARAM wp,LPARAM lp) {
    switch(msg) {
    case WM_TIMER:EnsureWeatherWindow();return 0;
    case WM_POWERBROADCAST:
        if(wp==PBT_APMRESUMEAUTOMATIC || wp==PBT_APMRESUMESUSPEND){SetEvent(refreshEvent);EnsureWeatherWindow();}
        return TRUE;
    case WM_TIMECHANGE:SetEvent(refreshEvent);return 0;
    case WM_CLOSE:DestroyWindow(hwnd);return 0;
    case WM_DESTROY:PostQuitMessage(0);return 0;
    }
    return DefWindowProcW(hwnd,msg,wp,lp);
}
DWORD WINAPI UiWorker(void*) {
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    WNDCLASSW wc{};wc.lpfnWndProc=WindowProc;wc.hInstance=instance;wc.lpszClassName=kClass;wc.hCursor=LoadCursor(nullptr,IDC_ARROW);RegisterClassW(&wc);
    WNDCLASSW card{};card.lpfnWndProc=DefWindowProcW;card.hInstance=instance;card.lpszClassName=kCardClass;RegisterClassW(&card);
    WNDCLASSW controller{};controller.lpfnWndProc=ControllerProc;controller.hInstance=instance;controller.lpszClassName=kControllerClass;RegisterClassW(&controller);
    // Hidden top-level window receives system resume and clock notifications.
    controllerWindow=CreateWindowExW(WS_EX_TOOLWINDOW|WS_EX_NOACTIVATE,kControllerClass,L"Weather lifecycle",WS_POPUP,0,0,0,0,nullptr,nullptr,instance,nullptr);
    if(!controllerWindow){Wh_Log(L"Weather controller creation failed");return 0;}
    // Click-through and never activated, so the card cannot take focus from the user's window.
    cardWindow=CreateWindowExW(WS_EX_LAYERED|WS_EX_TRANSPARENT|WS_EX_TOOLWINDOW|WS_EX_TOPMOST|WS_EX_NOACTIVATE,kCardClass,L"Independent taskbar weather card",WS_POPUP,0,0,1,1,nullptr,nullptr,instance,nullptr);
    SetTimer(controllerWindow,kLayoutTimer,1000,nullptr);EnsureWeatherWindow();
    if(WaitForSingleObject(stopEvent,0)==WAIT_OBJECT_0)PostMessageW(controllerWindow,WM_CLOSE,0,0);
    MSG msg;while(GetMessageW(&msg,nullptr,0,0)>0){TranslateMessage(&msg);DispatchMessageW(&msg);}
    if(weatherWindow)DestroyWindow(weatherWindow);
    if(cardWindow)DestroyWindow(cardWindow);cardWindow=nullptr;weatherWindow=nullptr;controllerWindow=nullptr;
    UnregisterClassW(kClass,instance);UnregisterClassW(kCardClass,instance);UnregisterClassW(kControllerClass,instance);return 0;
}
DWORD WINAPI FetchWorker(void*) {
    HANDLE events[]{stopEvent,refreshEvent};DWORD wait=0;
    while(WaitForMultipleObjects(2,events,FALSE,wait)!=WAIT_OBJECT_0){Reading next;bool ok=Fetch(next);{std::lock_guard lock(dataMutex);if(ok){next.updates=reading.updates+1;reading=next;}else reading.failed=true;}if(ok)SaveCachedReading(next);if(weatherWindow)PostMessageW(weatherWindow,kDataMessage,0,0);if(ok){Wh_Log(L"Weather update succeeded");}else{Wh_Log(L"Weather request failed; retry scheduled");}wait=ok?interval*60000:60000;}
    return 0;
}
}
BOOL Wh_ModInit() {
    LoadSettings();if(!LocationConfigured()){Wh_Log(L"Weather location is not configured");}
    LoadCachedReading();
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,(LPCWSTR)&Wh_ModInit,(HMODULE*)&instance);
    GdiplusStartupInput input;if(GdiplusStartup(&graphicsToken,&input,nullptr)!=Ok)return FALSE;
    stopEvent=CreateEventW(nullptr,TRUE,FALSE,nullptr);refreshEvent=CreateEventW(nullptr,FALSE,FALSE,nullptr);
    if(!stopEvent||!refreshEvent){if(stopEvent)CloseHandle(stopEvent);if(refreshEvent)CloseHandle(refreshEvent);GdiplusShutdown(graphicsToken);return FALSE;}
    uiThread=CreateThread(nullptr,0,UiWorker,nullptr,0,nullptr);
    if(uiThread)fetchThread=CreateThread(nullptr,0,FetchWorker,nullptr,0,nullptr);
    if(!uiThread||!fetchThread){SetEvent(stopEvent);if(controllerWindow)PostMessageW(controllerWindow,WM_CLOSE,0,0);if(uiThread){WaitForSingleObject(uiThread,INFINITE);CloseHandle(uiThread);}CloseHandle(stopEvent);CloseHandle(refreshEvent);GdiplusShutdown(graphicsToken);return FALSE;}
    return TRUE;
}
void Wh_ModUninit() {
    SetEvent(stopEvent);if(controllerWindow)PostMessageW(controllerWindow,WM_CLOSE,0,0);
    if(fetchThread){WaitForSingleObject(fetchThread,INFINITE);CloseHandle(fetchThread);}if(uiThread){WaitForSingleObject(uiThread,INFINITE);CloseHandle(uiThread);}
    CloseHandle(stopEvent);CloseHandle(refreshEvent);d2dFactory.Reset();writeFactory.Reset();GdiplusShutdown(graphicsToken);
}
BOOL Wh_ModSettingsChanged(BOOL* reload) { *reload=TRUE; return TRUE; }

