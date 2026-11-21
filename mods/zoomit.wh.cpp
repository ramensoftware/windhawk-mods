// ==WindhawkMod==
// @id              zoomit
// @name            ZoomIt
// @description     Zoom into the screen and draw on it with a hotkey, like Sysinternals ZoomIt
// @version         1.3.0
// @author          DavidHiFi
// @github          https://github.com/DavidHiFi
// @homepage        https://github.com/DavidHiFi/davids-windhawk-mods
// @donateUrl       https://ko-fi.com/DavidHiFi
// @include         windhawk.exe
// @include         windhawk-mod.exe
// @include         windhawk-mod-uiaccess.exe
// @compilerOptions -ld2d1 -ldwrite -lwindowscodecs -lole32 -luuid -lgdi32 -luser32 -lshell32 -lshlwapi -ladvapi32 -ldwmapi
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# ZoomIt

Zoom into any part of your screen and draw on it, straight from a hotkey. It
works like Sysinternals ZoomIt, without a separate app running in the tray.

![ZoomIt preview](https://raw.githubusercontent.com/DavidHiFi/davids-windhawk-mods/main/media/previews/zoomit.png)

## Features
- **Zoom with Ctrl+1.** The screen under your mouse zooms in smoothly. Scroll
  or use the arrow keys to zoom further, and move the mouse to look around.
- **Draw with Ctrl+2.** Draw on the screen as it is, or click while zoomed to
  draw on the zoomed view.
- **Shapes and highlighters.** Hold Shift for a straight line, Ctrl for a box,
  Tab for a circle and Ctrl+Shift for an arrow. Shift plus a color key gives a
  see-through highlighter.
- **Text and blur.** Press T to type on the screen and X to blur out private
  details.
- **Copy or save.** Ctrl+C copies what you see and Ctrl+S saves it as a
  picture. Add Shift to pick an area first.
- **Works over admin windows.** The hotkeys and drawing keep working while an
  app that runs as administrator is in front.
- **Your settings.** Hotkeys, zoom level and speed, animation, pen color (six
  colors, system, the Windows accent color or your own hex), pen size,
  highlighter and blur strength, text font and size, save folder.

## Keys while zoomed or drawing
| Key | What it does |
|---|---|
| Mouse wheel, Up, Down | Zoom in or out |
| Left click | Start drawing while zoomed |
| Right click | Stop drawing, or leave |
| Esc | Leave |
| R, G, B, Y, O, P | Red, green, blue, yellow, orange or pink pen |
| Shift + a color key | Highlighter in that color |
| X | Blur pen |
| Ctrl + wheel, Ctrl + Up or Down | Pen size, or text size while typing |
| Hold Shift, Ctrl, Tab or Ctrl+Shift while dragging | Line, box, circle or arrow |
| T, Shift+T | Type text, lined up left or right |
| W, K | White or black board |
| E | Erase all drawings |
| Ctrl+Z | Undo the last drawing |
| Space | Move the mouse to the middle of the screen |
| Ctrl+C, Ctrl+Shift+C | Copy the screen, or an area of it |
| Ctrl+S, Ctrl+Shift+S | Save the screen, or an area of it |

## Notes
- Turn off ZoomIt in PowerToys or the ZoomIt app first. Two apps can't use the
  same hotkey.
- Write hotkeys like `Ctrl+1`, `Ctrl+Shift+Z` or `Win+Alt+F2`. Leave one empty
  to turn it off.

## Credits
Works like [ZoomIt](https://learn.microsoft.com/sysinternals/downloads/zoomit)
by Mark Russinovich (Sysinternals). Written from scratch for Windhawk; it shares
no code with ZoomIt. MIT.

To read text bigger under the mouse, try
[Hover Text Magnifier](https://windhawk.net/mods/hover-text-magnifier) by Math
Shamenson.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- zoomHotkey: Ctrl+1
  $name: Zoom hotkey
  $description: Zooms in on the screen under the mouse. Press it again or Esc to leave. Leave empty to turn it off
- drawHotkey: Ctrl+2
  $name: Draw hotkey
  $description: Draw on the screen without zooming. Leave empty to turn it off
- snipHotkey: ""
  $name: Copy an area hotkey
  $description: Drag a box over part of the screen to copy it as a picture, for example Ctrl+6. Empty means off
- snipSaveHotkey: ""
  $name: Save an area hotkey
  $description: Drag a box over part of the screen to save it as a picture, for example Ctrl+Shift+6. Empty means off
- zoomLevel: 200
  $name: Starting zoom (%)
  $description: How far the zoom hotkey zooms in. 200 shows everything at twice the size
- zoomStep: 25
  $name: Zoom step (%)
  $description: How much each scroll notch or arrow press zooms in or out
- maxZoom: 3200
  $name: Largest zoom (%)
- animateZoom: true
  $name: Smooth zoom in
  $description: Glide into the zoom instead of jumping straight to it
- animateZoomOut: true
  $name: Smooth zoom out
  $description: Glide back out when you leave, instead of closing at once
- animationTime: 250
  $name: Zoom animation length (ms)
- smoothImage: true
  $name: Smooth zoomed picture
  $description: Softens pixels when zoomed in. Turn it off to see sharp, blocky pixels
- penColor: red
  $name: Pen color
  $description: The color you start with. The color keys change it while drawing. System follows your text color, Accent is the Windows accent color
  $options:
  - red: Red
  - green: Green
  - blue: Blue (Catppuccin Mocha)
  - yellow: Yellow
  - orange: Orange
  - pink: Pink
  - system: System
  - accent: Accent (Windows)
  - custom: Custom
- penColorCustom: "#ff00ff"
  $name: Custom pen color
  $description: Used when Pen color is Custom. As #RRGGBB, like #FF00FF
- penWidth: 5
  $name: Pen size (pixels)
- highlighterOpacity: 45
  $name: Highlighter strength (%)
  $description: How solid the highlighter looks. Lower lets more of the screen show through
- highlighterWidth: 300
  $name: Highlighter size (% of the pen size)
- blurStrength: 12
  $name: Blur strength
  $description: Higher numbers blur more. 4 is light, 30 is very strong
- blurWidth: 400
  $name: Blur pen size (% of the pen size)
- fontName: Segoe UI
  $name: Text font
- fontSize: 32
  $name: Text size (pixels)
- fontBold: false
  $name: Bold text
- arrowAtStart: false
  $name: Arrow points at the start
  $description: Put the arrow head where you start dragging instead of where you let go
- confineCursor: true
  $name: Keep the mouse on the zoomed screen
- leaveOnFocusLoss: true
  $name: Leave when another window comes to the front
- saveFolder: ""
  $name: Save folder
  $description: Where pictures are saved. Leave empty to choose a place each time
*/
// ==/WindhawkModSettings==

// Windhawk 2 ships the tool mod launcher as a header. Windhawk 1.x does not,
// so it gets the launcher at the end of this file.
#if __has_include(<windhawk_tool_mod.h>)
#include <windhawk_tool_mod.h>
#endif

#include <d2d1_1.h>
#include <dwmapi.h>
#include <dwrite.h>
#include <shlobj.h>
#include <shellapi.h>
#include <shlwapi.h>
#include <shobjidl.h>
#include <wincodec.h>
#include <windowsx.h>
#include <wrl/client.h>

#include <algorithm>
#include <atomic>
#include <cmath>
#include <string>
#include <vector>

using Microsoft::WRL::ComPtr;

namespace {

////////////////////////////////////////////////////////////////////////////////
// Settings

struct Settings {
    std::wstring zoomHotkey, drawHotkey, snipHotkey, snipSaveHotkey;
    float zoomLevel = 2.0f, zoomStep = 1.25f, maxZoom = 32.0f;
    bool animateZoom = true, animateZoomOut = true, smoothImage = true;
    int animationTime = 250;
    int penColor = 0;
    COLORREF penCustom = RGB(255, 0, 255);
    float penWidth = 5;
    float highlighterOpacity = 0.45f, highlighterWidth = 3.0f;
    int blurStrength = 12;
    float blurWidth = 4.0f;
    std::wstring fontName = L"Segoe UI";
    float fontSize = 32;
    bool fontBold = false, arrowAtStart = false;
    bool confineCursor = true, leaveOnFocusLoss = true;
    std::wstring saveFolder;
} g_set;

std::wstring ReadString(PCWSTR name) {
    PCWSTR value = Wh_GetStringSetting(name);
    std::wstring result = value ? value : L"";
    Wh_FreeStringSetting(value);
    return result;
}

float ClampF(float v, float lo, float hi) { return std::max(lo, std::min(hi, v)); }

// "#RGB" or "#RRGGBB", with or without the "#". Anything else falls back to
// magenta, so a typo shows on screen instead of silently drawing nothing.
COLORREF ParseHexColor(const std::wstring& text) {
    std::wstring s = text;
    if (!s.empty() && s[0] == L'#') s.erase(0, 1);
    if (s.size() != 3 && s.size() != 6) return RGB(255, 0, 255);
    for (wchar_t c : s) {
        if (!iswxdigit(c)) return RGB(255, 0, 255);
    }
    auto digit = [](wchar_t c) {
        return (int)(c <= L'9' ? c - L'0' : (towlower(c) - L'a') + 10);
    };
    if (s.size() == 3) {
        return RGB(digit(s[0]) * 17, digit(s[1]) * 17, digit(s[2]) * 17);
    }
    return RGB(digit(s[0]) * 16 + digit(s[1]), digit(s[2]) * 16 + digit(s[3]),
               digit(s[4]) * 16 + digit(s[5]));
}

void LoadSettings() {
    g_set.zoomHotkey = ReadString(L"zoomHotkey");
    g_set.drawHotkey = ReadString(L"drawHotkey");
    g_set.snipHotkey = ReadString(L"snipHotkey");
    g_set.snipSaveHotkey = ReadString(L"snipSaveHotkey");
    g_set.zoomLevel = ClampF(Wh_GetIntSetting(L"zoomLevel") / 100.0f, 1.1f, 32.0f);
    g_set.zoomStep = 1.0f + ClampF(Wh_GetIntSetting(L"zoomStep") / 100.0f, 0.05f, 2.0f);
    g_set.maxZoom = ClampF(Wh_GetIntSetting(L"maxZoom") / 100.0f, 1.5f, 64.0f);
    g_set.zoomLevel = std::min(g_set.zoomLevel, g_set.maxZoom);
    g_set.animateZoom = Wh_GetIntSetting(L"animateZoom");
    g_set.animateZoomOut = Wh_GetIntSetting(L"animateZoomOut");
    g_set.animationTime = std::clamp(Wh_GetIntSetting(L"animationTime"), 50, 2000);
    g_set.smoothImage = Wh_GetIntSetting(L"smoothImage");

    static const PCWSTR colors[] = {L"red",    L"green",  L"blue",   L"yellow",
                                    L"orange", L"pink",   L"system", L"accent",
                                    L"custom"};
    std::wstring color = ReadString(L"penColor");
    g_set.penColor = 0;
    for (int i = 0; i < (int)ARRAYSIZE(colors); i++) {
        if (_wcsicmp(color.c_str(), colors[i]) == 0) g_set.penColor = i;
    }
    g_set.penCustom = ParseHexColor(ReadString(L"penColorCustom"));
    g_set.penWidth = ClampF((float)Wh_GetIntSetting(L"penWidth"), 1, 100);
    g_set.highlighterOpacity =
        ClampF(Wh_GetIntSetting(L"highlighterOpacity") / 100.0f, 0.05f, 1.0f);
    g_set.highlighterWidth =
        ClampF(Wh_GetIntSetting(L"highlighterWidth") / 100.0f, 0.5f, 20.0f);
    g_set.blurStrength = std::clamp(Wh_GetIntSetting(L"blurStrength"), 2, 60);
    g_set.blurWidth = ClampF(Wh_GetIntSetting(L"blurWidth") / 100.0f, 0.5f, 30.0f);
    g_set.fontName = ReadString(L"fontName");
    if (g_set.fontName.empty()) g_set.fontName = L"Segoe UI";
    g_set.fontSize = ClampF((float)Wh_GetIntSetting(L"fontSize"), 6, 400);
    g_set.fontBold = Wh_GetIntSetting(L"fontBold");
    g_set.arrowAtStart = Wh_GetIntSetting(L"arrowAtStart");
    g_set.confineCursor = Wh_GetIntSetting(L"confineCursor");
    g_set.leaveOnFocusLoss = Wh_GetIntSetting(L"leaveOnFocusLoss");
    g_set.saveFolder = ReadString(L"saveFolder");
}

////////////////////////////////////////////////////////////////////////////////
// Hotkey text ("Ctrl+Shift+1") to RegisterHotKey values

std::wstring Lower(std::wstring s) {
    for (auto& c : s) c = (wchar_t)towlower(c);
    return s;
}

std::wstring Trim(const std::wstring& s) {
    size_t a = s.find_first_not_of(L" \t");
    if (a == std::wstring::npos) return L"";
    size_t b = s.find_last_not_of(L" \t");
    return s.substr(a, b - a + 1);
}

UINT KeyNameToVk(const std::wstring& name) {
    std::wstring k = Lower(name);
    if (k.size() == 1) {
        wchar_t c = (wchar_t)towupper(k[0]);
        if ((c >= L'A' && c <= L'Z') || (c >= L'0' && c <= L'9')) return c;
        SHORT scan = VkKeyScanW(k[0]);
        return scan == -1 ? 0 : (scan & 0xFF);
    }
    if (k[0] == L'f' && k.size() <= 3) {
        int n = _wtoi(k.c_str() + 1);
        if (n >= 1 && n <= 24) return VK_F1 + n - 1;
    }
    if (k.rfind(L"numpad", 0) == 0 && k.size() == 7 && iswdigit(k[6])) {
        return VK_NUMPAD0 + (k[6] - L'0');
    }
    static const struct {
        PCWSTR name;
        UINT vk;
    } names[] = {
        {L"space", VK_SPACE},       {L"tab", VK_TAB},
        {L"enter", VK_RETURN},      {L"return", VK_RETURN},
        {L"esc", VK_ESCAPE},        {L"escape", VK_ESCAPE},
        {L"backspace", VK_BACK},    {L"insert", VK_INSERT},
        {L"ins", VK_INSERT},        {L"delete", VK_DELETE},
        {L"del", VK_DELETE},        {L"home", VK_HOME},
        {L"end", VK_END},           {L"pageup", VK_PRIOR},
        {L"pgup", VK_PRIOR},        {L"pagedown", VK_NEXT},
        {L"pgdn", VK_NEXT},         {L"up", VK_UP},
        {L"down", VK_DOWN},         {L"left", VK_LEFT},
        {L"right", VK_RIGHT},       {L"printscreen", VK_SNAPSHOT},
        {L"prtsc", VK_SNAPSHOT},    {L"print", VK_SNAPSHOT},
        {L"pause", VK_PAUSE},       {L"plus", VK_OEM_PLUS},
        {L"minus", VK_OEM_MINUS},   {L"comma", VK_OEM_COMMA},
        {L"period", VK_OEM_PERIOD},
    };
    for (auto& n : names) {
        if (k == n.name) return n.vk;
    }
    return 0;
}

bool ParseHotkey(const std::wstring& text, UINT* mods, UINT* vk) {
    *mods = 0;
    *vk = 0;
    std::wstring s = Trim(text);
    if (s.empty()) return false;
    size_t start = 0;
    while (start <= s.size()) {
        size_t plus = s.find(L'+', start);
        // A trailing "+" is the plus key itself ("Ctrl++").
        if (plus == start && plus + 1 == s.size()) plus = std::wstring::npos;
        std::wstring part = Trim(s.substr(
            start, plus == std::wstring::npos ? std::wstring::npos : plus - start));
        std::wstring k = Lower(part);
        if (k == L"ctrl" || k == L"control") {
            *mods |= MOD_CONTROL;
        } else if (k == L"alt") {
            *mods |= MOD_ALT;
        } else if (k == L"shift") {
            *mods |= MOD_SHIFT;
        } else if (k == L"win" || k == L"windows") {
            *mods |= MOD_WIN;
        } else if (!part.empty()) {
            *vk = KeyNameToVk(part);
            if (!*vk) return false;
        }
        if (plus == std::wstring::npos) break;
        start = plus + 1;
    }
    return *vk != 0;
}

////////////////////////////////////////////////////////////////////////////////
// Drawing model

enum class ShapeKind { Free, Line, Rect, Ellipse, Arrow, Text };
enum class PenKind { Pen, Highlighter, Blur };

struct Shape {
    ShapeKind kind = ShapeKind::Free;
    PenKind pen = PenKind::Pen;
    COLORREF color = RGB(255, 0, 0);
    float width = 5;  // In captured-image pixels.
    std::vector<D2D1_POINT_2F> pts;
    std::wstring text;
    float fontSize = 32;
    bool rightAlign = false;
};

const COLORREF kPalette[] = {RGB(255, 0, 0),   RGB(0, 200, 0),
                             RGB(0x89, 0xB4, 0xFA), RGB(255, 230, 0),
                             RGB(255, 140, 0), RGB(255, 60, 200)};

// Pen color 6: the theme's text color, black or white with dark mode.
COLORREF SystemTextColor() {
    return GetSysColor(COLOR_WINDOWTEXT);
}

// Pen color 7: the Windows accent color, as picked in Personalization.
COLORREF AccentColorRef() {
    HKEY key;
    DWORD value = 0, size = sizeof(value);
    if (RegOpenKeyExW(HKEY_CURRENT_USER,
                      L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer"
                      L"\\Accent",
                      0, KEY_READ, &key) == ERROR_SUCCESS) {
        if (RegQueryValueExW(key, L"AccentColor", nullptr, nullptr, (LPBYTE)&value,
                             &size) == ERROR_SUCCESS &&
            size == sizeof(value)) {
            RegCloseKey(key);
            return RGB(GetRValue(value), GetGValue(value), GetBValue(value));
        }
        RegCloseKey(key);
    }
    BOOL opaque = FALSE;
    DWORD colorization = 0;
    if (SUCCEEDED(DwmGetColorizationColor(&colorization, &opaque))) {
        return RGB((colorization >> 16) & 0xFF, (colorization >> 8) & 0xFF,
                   colorization & 0xFF);
    }
    return RGB(0, 120, 215);
}

// Resolves the current pen color: palette 0-5, then system, accent, custom.
COLORREF PenColorRef(int index) {
    if (index < (int)ARRAYSIZE(kPalette)) return kPalette[index];
    if (index == 6) return SystemTextColor();
    if (index == 7) return AccentColorRef();
    return g_set.penCustom;
}

enum class Mode { None, Zoom, Draw, Crop };
enum class OriginMode { Follow, Anchor, Shrink };
enum class CropAction { Copy, Save };

constexpr UINT kHotkeyZoom = 1, kHotkeyDraw = 2, kHotkeySnip = 3,
               kHotkeySnipSave = 4;
constexpr UINT WM_APP_SETTINGS = WM_APP + 1;
constexpr UINT WM_APP_LEAVE = WM_APP + 2;
constexpr UINT_PTR kTimerAnim = 1, kTimerRetryHotkeys = 2;

struct DeviceRes {
    ComPtr<ID2D1RenderTarget> rt;
    ComPtr<ID2D1DeviceContext> dc;
    ComPtr<ID2D1Bitmap> image, blurImage;
    ComPtr<ID2D1SolidColorBrush> brush;
};

struct State {
    HWND msgWnd = nullptr;
    HWND wnd = nullptr;
    ComPtr<ID2D1Factory1> d2d;
    ComPtr<IDWriteFactory> dwrite;
    ComPtr<IWICImagingFactory> wic;
    ComPtr<ID2D1StrokeStyle> roundStroke, flatStroke;
    ComPtr<ID2D1HwndRenderTarget> hwndRt;
    DeviceRes res;

    RECT mon{};
    int W = 0, H = 0;
    float dpiScale = 1;
    HBITMAP capBmp = nullptr;
    void* capBits = nullptr;
    HBITMAP blurBmp = nullptr;
    void* blurBits = nullptr;
    int blurW = 0, blurH = 0;

    Mode mode = Mode::None;
    bool drawFromZoom = false;
    bool sessionEndsAfterCrop = false;
    Mode cropReturnMode = Mode::None;
    CropAction cropAction = CropAction::Copy;
    bool cropDragging = false;
    POINT cropA{}, cropB{};

    float zoom = 1;
    OriginMode originMode = OriginMode::Follow;
    D2D1_POINT_2F anchorImg{}, anchorWin{}, shrinkOrigin{};
    float shrinkZoom = 1;
    bool animating = false, closing = false;
    float animFrom = 1, animTo = 1;
    double animStart = 0, animLength = 0;

    POINT cursor{};
    int wheelRemainder = 0;
    std::vector<Shape> shapes;
    bool dragging = false;
    bool dragStartedFree = true;
    Shape current;
    bool typing = false;
    Shape textShape;
    int board = 0;  // 0 screen, 1 white, 2 black

    int colorIndex = 0;
    PenKind pen = PenKind::Pen;
    float penWidth = 5;    // Screen pixels.
    float fontSize = 32;   // Screen pixels.
    bool inDialog = false;
    bool hotkeyRetryArmed = false;
} g;

double NowMs() {
    static LARGE_INTEGER freq = [] {
        LARGE_INTEGER f;
        QueryPerformanceFrequency(&f);
        return f;
    }();
    LARGE_INTEGER t;
    QueryPerformanceCounter(&t);
    return t.QuadPart * 1000.0 / freq.QuadPart;
}

D2D1_COLOR_F ToColor(COLORREF c, float a = 1) {
    return D2D1::ColorF(GetRValue(c) / 255.0f, GetGValue(c) / 255.0f,
                        GetBValue(c) / 255.0f, a);
}

////////////////////////////////////////////////////////////////////////////////
// View math: window pixel = (image pixel - origin) * zoom

D2D1_POINT_2F CurrentOrigin() {
    float z = g.zoom, ox = 0, oy = 0;
    switch (g.originMode) {
        case OriginMode::Follow:
            ox = g.cursor.x * (1 - 1 / z);
            oy = g.cursor.y * (1 - 1 / z);
            break;
        case OriginMode::Anchor:
            ox = g.anchorImg.x - g.anchorWin.x / z;
            oy = g.anchorImg.y - g.anchorWin.y / z;
            break;
        case OriginMode::Shrink: {
            float k = g.shrinkZoom > 1.0001f ? (1 - 1 / z) / (1 - 1 / g.shrinkZoom) : 0;
            ox = g.shrinkOrigin.x * k;
            oy = g.shrinkOrigin.y * k;
            break;
        }
    }
    ox = ClampF(ox, 0, std::max(0.0f, g.W - g.W / z));
    oy = ClampF(oy, 0, std::max(0.0f, g.H - g.H / z));
    return D2D1::Point2F(ox, oy);
}

D2D1_POINT_2F WinToImg(POINT p) {
    D2D1_POINT_2F o = CurrentOrigin();
    return D2D1::Point2F(o.x + p.x / g.zoom, o.y + p.y / g.zoom);
}

// Freeze the current view so it no longer follows the mouse.
void FreezeView() {
    g.anchorImg = WinToImg(g.cursor);
    g.anchorWin = D2D1::Point2F((float)g.cursor.x, (float)g.cursor.y);
    g.originMode = OriginMode::Anchor;
}

////////////////////////////////////////////////////////////////////////////////
// Rendering

HRESULT CreateTargetResources(DeviceRes& r) {
    D2D1_BITMAP_PROPERTIES props = D2D1::BitmapProperties(
        D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_IGNORE));
    HRESULT hr = r.rt->CreateBitmap(D2D1::SizeU(g.W, g.H), g.capBits, g.W * 4,
                                    props, &r.image);
    if (FAILED(hr)) return hr;
    if (g.blurBits) {
        r.rt->CreateBitmap(D2D1::SizeU(g.blurW, g.blurH), g.blurBits,
                           g.blurW * 4, props, &r.blurImage);
    }
    hr = r.rt->CreateSolidColorBrush(D2D1::ColorF(1, 0, 0), &r.brush);
    r.rt.As(&r.dc);
    return hr;
}

ComPtr<IDWriteTextLayout> MakeTextLayout(const Shape& s) {
    ComPtr<IDWriteTextFormat> format;
    if (FAILED(g.dwrite->CreateTextFormat(
            g_set.fontName.c_str(), nullptr,
            g_set.fontBold ? DWRITE_FONT_WEIGHT_BOLD : DWRITE_FONT_WEIGHT_NORMAL,
            DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, s.fontSize,
            L"", &format))) {
        return nullptr;
    }
    format->SetTextAlignment(s.rightAlign ? DWRITE_TEXT_ALIGNMENT_TRAILING
                                          : DWRITE_TEXT_ALIGNMENT_LEADING);
    format->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);
    ComPtr<IDWriteTextLayout> layout;
    // A zero-width box keeps alignment anchored at the click point; NO_WRAP
    // lets the text spill out of it.
    g.dwrite->CreateTextLayout(s.text.c_str(), (UINT32)s.text.size(),
                               format.Get(), 0, 0, &layout);
    return layout;
}

ComPtr<ID2D1Geometry> MakeShapeGeometry(const Shape& s, bool filledForBlur) {
    ComPtr<ID2D1Geometry> result;
    if (s.pts.empty()) return result;
    D2D1_POINT_2F a = s.pts.front(), b = s.pts.back();
    D2D1_RECT_F box = D2D1::RectF(std::min(a.x, b.x), std::min(a.y, b.y),
                                  std::max(a.x, b.x), std::max(a.y, b.y));
    if (s.kind == ShapeKind::Rect && filledForBlur) {
        ComPtr<ID2D1RectangleGeometry> rg;
        g.d2d->CreateRectangleGeometry(box, &rg);
        result = rg;
        return result;
    }
    if (s.kind == ShapeKind::Ellipse && filledForBlur) {
        ComPtr<ID2D1EllipseGeometry> eg;
        g.d2d->CreateEllipseGeometry(
            D2D1::Ellipse(D2D1::Point2F((box.left + box.right) / 2,
                                        (box.top + box.bottom) / 2),
                          (box.right - box.left) / 2, (box.bottom - box.top) / 2),
            &eg);
        result = eg;
        return result;
    }
    ComPtr<ID2D1PathGeometry> path;
    g.d2d->CreatePathGeometry(&path);
    ComPtr<ID2D1GeometrySink> sink;
    path->Open(&sink);
    switch (s.kind) {
        case ShapeKind::Free:
            sink->BeginFigure(a, D2D1_FIGURE_BEGIN_HOLLOW);
            if (s.pts.size() == 1) {
                sink->AddLine(D2D1::Point2F(a.x + 0.01f, a.y));
            } else {
                sink->AddLines(s.pts.data() + 1, (UINT32)s.pts.size() - 1);
            }
            sink->EndFigure(D2D1_FIGURE_END_OPEN);
            break;
        case ShapeKind::Line:
        case ShapeKind::Arrow:
            sink->BeginFigure(a, D2D1_FIGURE_BEGIN_HOLLOW);
            sink->AddLine(b);
            sink->EndFigure(D2D1_FIGURE_END_OPEN);
            break;
        case ShapeKind::Rect: {
            D2D1_POINT_2F p[] = {{box.right, box.top},
                                 {box.right, box.bottom},
                                 {box.left, box.bottom}};
            sink->BeginFigure(D2D1::Point2F(box.left, box.top),
                              D2D1_FIGURE_BEGIN_HOLLOW);
            sink->AddLines(p, 3);
            sink->EndFigure(D2D1_FIGURE_END_CLOSED);
            break;
        }
        case ShapeKind::Ellipse: {
            float cx = (box.left + box.right) / 2, cy = (box.top + box.bottom) / 2;
            float rx = (box.right - box.left) / 2, ry = (box.bottom - box.top) / 2;
            sink->BeginFigure(D2D1::Point2F(cx - rx, cy), D2D1_FIGURE_BEGIN_HOLLOW);
            sink->AddArc(D2D1::ArcSegment(D2D1::Point2F(cx + rx, cy),
                                          D2D1::SizeF(rx, ry), 0,
                                          D2D1_SWEEP_DIRECTION_CLOCKWISE,
                                          D2D1_ARC_SIZE_SMALL));
            sink->AddArc(D2D1::ArcSegment(D2D1::Point2F(cx - rx, cy),
                                          D2D1::SizeF(rx, ry), 0,
                                          D2D1_SWEEP_DIRECTION_CLOCKWISE,
                                          D2D1_ARC_SIZE_SMALL));
            sink->EndFigure(D2D1_FIGURE_END_CLOSED);
            break;
        }
        case ShapeKind::Text:
            break;
    }
    sink->Close();
    result = path;
    return result;
}

// Arrow head triangle at `tip`, pointing away from `from`.
ComPtr<ID2D1PathGeometry> MakeArrowHead(D2D1_POINT_2F from, D2D1_POINT_2F tip,
                                        float headLen) {
    ComPtr<ID2D1PathGeometry> path;
    float dx = tip.x - from.x, dy = tip.y - from.y;
    float len = std::sqrt(dx * dx + dy * dy);
    if (len < 0.5f) return path;
    dx /= len;
    dy /= len;
    float headHalf = headLen * 0.55f;
    D2D1_POINT_2F base = D2D1::Point2F(tip.x - dx * headLen, tip.y - dy * headLen);
    D2D1_POINT_2F p1 = D2D1::Point2F(base.x - dy * headHalf, base.y + dx * headHalf);
    D2D1_POINT_2F p2 = D2D1::Point2F(base.x + dy * headHalf, base.y - dx * headHalf);
    g.d2d->CreatePathGeometry(&path);
    ComPtr<ID2D1GeometrySink> sink;
    path->Open(&sink);
    sink->BeginFigure(tip, D2D1_FIGURE_BEGIN_FILLED);
    D2D1_POINT_2F pts[] = {p1, p2};
    sink->AddLines(pts, 2);
    sink->EndFigure(D2D1_FIGURE_END_CLOSED);
    sink->Close();
    return path;
}

// The head scales with the pen but never gets too small to see.
float ArrowHeadLen(float width) { return std::max(width * 4.0f, 12.0f / g.zoom); }

void DrawShape(DeviceRes& r, const Shape& s) {
    ID2D1RenderTarget* rt = r.rt.Get();
    if (s.kind == ShapeKind::Text) {
        if (s.text.empty()) return;
        ComPtr<IDWriteTextLayout> layout = MakeTextLayout(s);
        if (!layout) return;
        r.brush->SetColor(ToColor(s.color));
        rt->DrawTextLayout(s.pts.front(), layout.Get(), r.brush.Get());
        return;
    }
    if (s.pts.empty()) return;

    if (s.pen == PenKind::Blur) {
        if (!r.blurImage) return;
        bool filled = s.kind == ShapeKind::Rect || s.kind == ShapeKind::Ellipse;
        ComPtr<ID2D1Geometry> geom = MakeShapeGeometry(s, filled);
        if (!geom) return;
        ComPtr<ID2D1Geometry> mask = geom;
        if (!filled) {
            ComPtr<ID2D1PathGeometry> widened;
            g.d2d->CreatePathGeometry(&widened);
            ComPtr<ID2D1GeometrySink> sink;
            widened->Open(&sink);
            geom->Widen(s.width, g.roundStroke.Get(), nullptr,
                        D2D1_DEFAULT_FLATTENING_TOLERANCE, sink.Get());
            sink->Close();
            mask = widened;
        }
        ComPtr<ID2D1Layer> layer;
        rt->CreateLayer(&layer);
        rt->PushLayer(D2D1::LayerParameters(D2D1::InfiniteRect(), mask.Get()),
                      layer.Get());
        rt->DrawBitmap(r.blurImage.Get(), D2D1::RectF(0, 0, (float)g.W, (float)g.H),
                       1.0f, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
        rt->PopLayer();
        return;
    }

    bool highlighter = s.pen == PenKind::Highlighter;
    r.brush->SetColor(ToColor(s.color, highlighter ? g_set.highlighterOpacity : 1));
    ID2D1StrokeStyle* stroke =
        highlighter && s.kind == ShapeKind::Free ? g.flatStroke.Get()
                                                 : g.roundStroke.Get();
    if (s.kind == ShapeKind::Arrow) {
        D2D1_POINT_2F a = s.pts.front(), b = s.pts.back();
        if (g_set.arrowAtStart) std::swap(a, b);
        float headLen = ArrowHeadLen(s.width);
        float dx = b.x - a.x, dy = b.y - a.y;
        float len = std::sqrt(dx * dx + dy * dy);
        D2D1_POINT_2F shaftEnd = b;
        if (len > headLen) {
            shaftEnd = D2D1::Point2F(b.x - dx / len * headLen * 0.8f,
                                     b.y - dy / len * headLen * 0.8f);
        }
        rt->DrawLine(a, shaftEnd, r.brush.Get(), s.width, stroke);
        ComPtr<ID2D1PathGeometry> head = MakeArrowHead(a, b, headLen);
        if (head) rt->FillGeometry(head.Get(), r.brush.Get());
        return;
    }
    if (s.kind == ShapeKind::Free && s.pts.size() == 1) {
        float rad = s.width / 2;
        rt->FillEllipse(D2D1::Ellipse(s.pts[0], rad, rad), r.brush.Get());
        return;
    }
    ComPtr<ID2D1Geometry> geom = MakeShapeGeometry(s, false);
    if (geom) rt->DrawGeometry(geom.Get(), r.brush.Get(), s.width, stroke);
}

// Draws the whole scene. `live` adds the pen cursor, the text caret and the
// crop box, which a copied or saved picture leaves out.
void RenderScene(DeviceRes& r, bool live) {
    ID2D1RenderTarget* rt = r.rt.Get();
    rt->BeginDraw();
    rt->SetTransform(D2D1::Matrix3x2F::Identity());
    rt->Clear(D2D1::ColorF(0, 0, 0));

    D2D1_POINT_2F o = CurrentOrigin();
    D2D1::Matrix3x2F view = D2D1::Matrix3x2F::Translation(-o.x, -o.y) *
                            D2D1::Matrix3x2F::Scale(g.zoom, g.zoom);
    rt->SetTransform(view);
    D2D1_RECT_F full = D2D1::RectF(0, 0, (float)g.W, (float)g.H);
    if (g.board == 1 || g.board == 2) {
        float v = g.board == 1 ? 1.0f : 0.0f;
        r.brush->SetColor(D2D1::ColorF(v, v, v));
        rt->FillRectangle(full, r.brush.Get());
    } else if (r.image) {
        bool unscaled = std::fabs(g.zoom - 1) < 0.0005f;
        D2D1_INTERPOLATION_MODE mode =
            unscaled || !g_set.smoothImage
                ? D2D1_INTERPOLATION_MODE_NEAREST_NEIGHBOR
                : r.dc ? D2D1_INTERPOLATION_MODE_HIGH_QUALITY_CUBIC
                       : D2D1_INTERPOLATION_MODE_LINEAR;
        if (r.dc) {
            r.dc->DrawBitmap(r.image.Get(), &full, 1.0f, mode, nullptr, nullptr);
        } else {
            rt->DrawBitmap(
                r.image.Get(), full, 1.0f,
                mode == D2D1_INTERPOLATION_MODE_NEAREST_NEIGHBOR
                    ? D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR
                    : D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
        }
    }

    for (const Shape& s : g.shapes) DrawShape(r, s);
    if (g.dragging) DrawShape(r, g.current);
    if (g.typing) DrawShape(r, g.textShape);

    if (live && g.typing) {
        // Caret at the end of the text.
        D2D1_POINT_2F p = g.textShape.pts.front();
        float caretX = p.x, caretY = p.y, caretH = g.textShape.fontSize * 1.2f;
        // A zero-width space after the text puts the caret on a new line
        // right after Enter.
        Shape probe = g.textShape;
        probe.text += L'\x200B';
        ComPtr<IDWriteTextLayout> layout = MakeTextLayout(probe);
        DWRITE_HIT_TEST_METRICS m;
        FLOAT hx, hy;
        if (layout && SUCCEEDED(layout->HitTestTextPosition(
                          (UINT32)g.textShape.text.size(), FALSE, &hx, &hy, &m))) {
            caretX = p.x + hx;
            caretY = p.y + hy;
            caretH = m.height;
        }
        r.brush->SetColor(ToColor(g.textShape.color));
        rt->DrawLine(D2D1::Point2F(caretX, caretY),
                     D2D1::Point2F(caretX, caretY + caretH), r.brush.Get(),
                     std::max(1.5f / g.zoom, g.textShape.fontSize / 16));
    }

    rt->SetTransform(D2D1::Matrix3x2F::Identity());
    if (live && g.mode == Mode::Draw && !g.typing) {
        D2D1_POINT_2F c = D2D1::Point2F((float)g.cursor.x, (float)g.cursor.y);
        if (g.pen == PenKind::Blur) {
            float rad = g.penWidth * g_set.blurWidth / 2;
            r.brush->SetColor(D2D1::ColorF(0.5f, 0.5f, 0.5f, 0.9f));
            rt->DrawEllipse(D2D1::Ellipse(c, rad, rad), r.brush.Get(), 1.5f);
        } else if (g.pen == PenKind::Highlighter) {
            float half = g.penWidth * g_set.highlighterWidth / 2;
            r.brush->SetColor(
                ToColor(PenColorRef(g.colorIndex), g_set.highlighterOpacity));
            rt->FillRectangle(D2D1::RectF(c.x - half, c.y - half, c.x + half, c.y + half),
                              r.brush.Get());
        } else {
            float rad = std::max(g.penWidth / 2, 1.5f);
            r.brush->SetColor(ToColor(PenColorRef(g.colorIndex)));
            rt->FillEllipse(D2D1::Ellipse(c, rad, rad), r.brush.Get());
            r.brush->SetColor(D2D1::ColorF(1, 1, 1, 0.8f));
            rt->DrawEllipse(D2D1::Ellipse(c, rad + 1, rad + 1), r.brush.Get(), 1);
        }
    }
    if (live && g.mode == Mode::Crop) {
        r.brush->SetColor(D2D1::ColorF(0, 0, 0, 0.45f));
        if (g.cropDragging) {
            D2D1_RECT_F sel = D2D1::RectF(
                (float)std::min(g.cropA.x, g.cropB.x), (float)std::min(g.cropA.y, g.cropB.y),
                (float)std::max(g.cropA.x, g.cropB.x), (float)std::max(g.cropA.y, g.cropB.y));
            rt->FillRectangle(D2D1::RectF(0, 0, (float)g.W, sel.top), r.brush.Get());
            rt->FillRectangle(D2D1::RectF(0, sel.bottom, (float)g.W, (float)g.H), r.brush.Get());
            rt->FillRectangle(D2D1::RectF(0, sel.top, sel.left, sel.bottom), r.brush.Get());
            rt->FillRectangle(D2D1::RectF(sel.right, sel.top, (float)g.W, sel.bottom), r.brush.Get());
            r.brush->SetColor(D2D1::ColorF(1, 1, 1, 0.95f));
            rt->DrawRectangle(sel, r.brush.Get(), 1.5f);
        } else {
            rt->FillRectangle(D2D1::RectF(0, 0, (float)g.W, (float)g.H), r.brush.Get());
        }
    }
    HRESULT hr = rt->EndDraw();
    if (hr == D2DERR_RECREATE_TARGET && r.rt.Get() == g.res.rt.Get()) {
        Wh_Log(L"Render target lost, recreating");
        g.res = DeviceRes{};
        g.hwndRt.Reset();
    }
}

bool EnsureWindowTarget() {
    if (g.hwndRt) return true;
    D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties(
        D2D1_RENDER_TARGET_TYPE_DEFAULT,
        D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_IGNORE), 96,
        96);
    D2D1_HWND_RENDER_TARGET_PROPERTIES hwndProps = D2D1::HwndRenderTargetProperties(
        g.wnd, D2D1::SizeU(g.W, g.H), D2D1_PRESENT_OPTIONS_NONE);
    if (FAILED(g.d2d->CreateHwndRenderTarget(props, hwndProps, &g.hwndRt))) {
        Wh_Log(L"CreateHwndRenderTarget failed");
        return false;
    }
    g.res = DeviceRes{};
    g.res.rt = g.hwndRt;
    if (FAILED(CreateTargetResources(g.res))) {
        Wh_Log(L"Creating the zoom image failed");
        g.hwndRt.Reset();
        g.res = DeviceRes{};
        return false;
    }
    return true;
}

void Redraw() {
    if (!g.wnd || !EnsureWindowTarget()) return;
    RenderScene(g.res, true);
}

////////////////////////////////////////////////////////////////////////////////
// Copy and save

ComPtr<IWICBitmap> RenderToWicBitmap() {
    ComPtr<IWICBitmap> bitmap;
    if (FAILED(g.wic->CreateBitmap(g.W, g.H, GUID_WICPixelFormat32bppPBGRA,
                                   WICBitmapCacheOnLoad, &bitmap))) {
        return nullptr;
    }
    DeviceRes r;
    ComPtr<ID2D1RenderTarget> rt;
    D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties(
        D2D1_RENDER_TARGET_TYPE_DEFAULT,
        D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED),
        96, 96);
    if (FAILED(g.d2d->CreateWicBitmapRenderTarget(bitmap.Get(), props, &rt))) {
        return nullptr;
    }
    r.rt = rt;
    if (FAILED(CreateTargetResources(r))) return nullptr;
    RenderScene(r, false);
    return bitmap;
}

// Copies `rect` (window pixels) of the bitmap out as a bottom-up 32-bit DIB.
bool CopyBitmapToClipboard(IWICBitmap* bitmap, const RECT& rect) {
    int w = rect.right - rect.left, h = rect.bottom - rect.top;
    if (w <= 0 || h <= 0) return false;
    std::vector<BYTE> pixels((size_t)w * h * 4);
    WICRect wr{rect.left, rect.top, w, h};
    if (FAILED(bitmap->CopyPixels(&wr, w * 4, (UINT)pixels.size(), pixels.data()))) {
        return false;
    }
    for (size_t i = 3; i < pixels.size(); i += 4) pixels[i] = 255;
    HGLOBAL mem = GlobalAlloc(GMEM_MOVEABLE, sizeof(BITMAPINFOHEADER) + pixels.size());
    if (!mem) return false;
    auto* bih = (BITMAPINFOHEADER*)GlobalLock(mem);
    *bih = BITMAPINFOHEADER{};
    bih->biSize = sizeof(BITMAPINFOHEADER);
    bih->biWidth = w;
    bih->biHeight = h;
    bih->biPlanes = 1;
    bih->biBitCount = 32;
    bih->biCompression = BI_RGB;
    BYTE* dst = (BYTE*)(bih + 1);
    for (int y = 0; y < h; y++) {
        memcpy(dst + (size_t)(h - 1 - y) * w * 4, pixels.data() + (size_t)y * w * 4,
               (size_t)w * 4);
    }
    GlobalUnlock(mem);
    if (!OpenClipboard(g.wnd)) {
        GlobalFree(mem);
        return false;
    }
    EmptyClipboard();
    if (!SetClipboardData(CF_DIB, mem)) {
        GlobalFree(mem);
        CloseClipboard();
        return false;
    }
    CloseClipboard();
    return true;
}

bool SavePng(IWICBitmap* bitmap, const RECT& rect, const std::wstring& path) {
    ComPtr<IWICBitmapClipper> clipper;
    WICRect wr{rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top};
    if (FAILED(g.wic->CreateBitmapClipper(&clipper)) ||
        FAILED(clipper->Initialize(bitmap, &wr))) {
        return false;
    }
    ComPtr<IWICStream> stream;
    ComPtr<IWICBitmapEncoder> encoder;
    ComPtr<IWICBitmapFrameEncode> frame;
    if (FAILED(g.wic->CreateStream(&stream)) ||
        FAILED(stream->InitializeFromFilename(path.c_str(), GENERIC_WRITE)) ||
        FAILED(g.wic->CreateEncoder(GUID_ContainerFormatPng, nullptr, &encoder)) ||
        FAILED(encoder->Initialize(stream.Get(), WICBitmapEncoderNoCache)) ||
        FAILED(encoder->CreateNewFrame(&frame, nullptr)) ||
        FAILED(frame->Initialize(nullptr)) ||
        FAILED(frame->SetSize(wr.Width, wr.Height))) {
        return false;
    }
    WICPixelFormatGUID format = GUID_WICPixelFormat32bppBGRA;
    frame->SetPixelFormat(&format);
    // Convert premultiplied pixels (all opaque) to the encoder's format.
    ComPtr<IWICFormatConverter> converter;
    g.wic->CreateFormatConverter(&converter);
    converter->Initialize(clipper.Get(), format, WICBitmapDitherTypeNone, nullptr,
                          0, WICBitmapPaletteTypeCustom);
    return SUCCEEDED(frame->WriteSource(converter.Get(), nullptr)) &&
           SUCCEEDED(frame->Commit()) && SUCCEEDED(encoder->Commit());
}

std::wstring DefaultFileName() {
    SYSTEMTIME st;
    GetLocalTime(&st);
    int hour = st.wHour % 12 == 0 ? 12 : st.wHour % 12;
    wchar_t name[96];
    swprintf_s(name, L"ZoomIt %04d-%02d-%02d at %d.%02d.%02d %s.png", st.wYear,
               st.wMonth, st.wDay, hour, st.wMinute, st.wSecond,
               st.wHour < 12 ? L"AM" : L"PM");
    return name;
}

std::wstring AskSavePath() {
    ComPtr<IFileSaveDialog> dialog;
    if (FAILED(CoCreateInstance(CLSID_FileSaveDialog, nullptr, CLSCTX_INPROC_SERVER,
                                IID_PPV_ARGS(&dialog)))) {
        return L"";
    }
    COMDLG_FILTERSPEC filter[] = {{L"PNG picture", L"*.png"}};
    dialog->SetFileTypes(1, filter);
    dialog->SetDefaultExtension(L"png");
    dialog->SetFileName(DefaultFileName().c_str());
    PWSTR pictures = nullptr;
    if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_Pictures, 0, nullptr, &pictures))) {
        ComPtr<IShellItem> folder;
        if (SUCCEEDED(SHCreateItemFromParsingName(pictures, nullptr,
                                                  IID_PPV_ARGS(&folder)))) {
            dialog->SetDefaultFolder(folder.Get());
        }
        CoTaskMemFree(pictures);
    }
    std::wstring result;
    if (SUCCEEDED(dialog->Show(nullptr))) {
        ComPtr<IShellItem> item;
        PWSTR path = nullptr;
        if (SUCCEEDED(dialog->GetResult(&item)) &&
            SUCCEEDED(item->GetDisplayName(SIGDN_FILESYSPATH, &path))) {
            result = path;
            CoTaskMemFree(path);
        }
    }
    return result;
}

void EndSession();
void ShowOverlayAgain();

void Export(CropAction action, const RECT& rect) {
    ComPtr<IWICBitmap> bitmap = RenderToWicBitmap();
    if (!bitmap) {
        Wh_Log(L"Rendering the picture failed");
        return;
    }
    if (action == CropAction::Copy) {
        if (!CopyBitmapToClipboard(bitmap.Get(), rect)) Wh_Log(L"Copy failed");
        return;
    }
    std::wstring path;
    if (!g_set.saveFolder.empty()) {
        SHCreateDirectoryExW(nullptr, g_set.saveFolder.c_str(), nullptr);
        path = g_set.saveFolder;
        if (path.back() != L'\\') path += L'\\';
        path += DefaultFileName();
    } else {
        // The dialog can't show above a full screen topmost window, so step
        // aside while it is open.
        g.inDialog = true;
        ClipCursor(nullptr);
        ShowWindow(g.wnd, SW_HIDE);
        path = AskSavePath();
        g.inDialog = false;
        if (g.mode == Mode::None || g.sessionEndsAfterCrop) {
            // Snip session: nothing to return to.
        } else {
            ShowOverlayAgain();
        }
    }
    if (!path.empty() && !SavePng(bitmap.Get(), rect, path)) {
        Wh_Log(L"Saving %s failed", path.c_str());
    }
}

////////////////////////////////////////////////////////////////////////////////
// Session

void StartAnimation(float to, double length, bool closing) {
    g.animFrom = g.zoom;
    g.animTo = to;
    g.animStart = NowMs();
    g.animLength = length;
    g.animating = true;
    g.closing = closing;
    SetTimer(g.wnd, kTimerAnim, 8, nullptr);
}

void StepAnimation() {
    double t = g.animLength > 0 ? (NowMs() - g.animStart) / g.animLength : 1;
    if (t >= 1) {
        g.zoom = g.animTo;
        g.animating = false;
        KillTimer(g.wnd, kTimerAnim);
        if (g.closing) {
            EndSession();
            return;
        }
    } else {
        double e = 1 - std::pow(1 - t, 3);
        g.zoom = (float)(g.animFrom * std::pow(g.animTo / g.animFrom, e));
    }
    Redraw();
}

void SetZoom(float target, bool animate) {
    target = ClampF(target, 1.0f, g_set.maxZoom);
    if (g.originMode == OriginMode::Anchor) {
        // Keep the picture under the mouse in place.
        D2D1_POINT_2F p = WinToImg(g.cursor);
        g.anchorImg = p;
        g.anchorWin = D2D1::Point2F((float)g.cursor.x, (float)g.cursor.y);
    }
    if (animate && g_set.animateZoom) {
        StartAnimation(target, std::min(160, g_set.animationTime), false);
    } else {
        g.zoom = target;
        g.animating = false;
        KillTimer(g.wnd, kTimerAnim);
        Redraw();
    }
}

float ZoomTarget() { return g.animating && !g.closing ? g.animTo : g.zoom; }

void FreeScreenCapture() {
    if (g.capBmp) DeleteObject(g.capBmp);
    if (g.blurBmp) DeleteObject(g.blurBmp);
    g.capBmp = g.blurBmp = nullptr;
    g.capBits = g.blurBits = nullptr;
}

bool CaptureMonitor() {
    POINT pt;
    GetCursorPos(&pt);
    HMONITOR monitor = MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi{sizeof(mi)};
    if (!GetMonitorInfo(monitor, &mi)) return false;
    g.mon = mi.rcMonitor;
    g.W = g.mon.right - g.mon.left;
    g.H = g.mon.bottom - g.mon.top;
    UINT dpi = 96;
    using GetDpiForMonitor_t = HRESULT(WINAPI*)(HMONITOR, int, UINT*, UINT*);
    static auto getDpi = (GetDpiForMonitor_t)GetProcAddress(
        LoadLibraryW(L"shcore.dll"), "GetDpiForMonitor");
    UINT dpiY;
    if (getDpi) getDpi(monitor, 0, &dpi, &dpiY);
    g.dpiScale = dpi / 96.0f;

    BITMAPINFO bi{};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = g.W;
    bi.bmiHeader.biHeight = -g.H;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;
    HDC screen = GetDC(nullptr);
    HDC mem = CreateCompatibleDC(screen);
    g.capBmp = CreateDIBSection(screen, &bi, DIB_RGB_COLORS, &g.capBits, nullptr, 0);
    if (!g.capBmp) {
        DeleteDC(mem);
        ReleaseDC(nullptr, screen);
        return false;
    }
    HGDIOBJ old = SelectObject(mem, g.capBmp);
    BitBlt(mem, 0, 0, g.W, g.H, screen, g.mon.left, g.mon.top, SRCCOPY | CAPTUREBLT);

    // A low resolution copy, stretched back up, is the blur pen's picture.
    g.blurW = std::max(1, g.W / g_set.blurStrength);
    g.blurH = std::max(1, g.H / g_set.blurStrength);
    bi.bmiHeader.biWidth = g.blurW;
    bi.bmiHeader.biHeight = -g.blurH;
    HDC small = CreateCompatibleDC(screen);
    g.blurBmp = CreateDIBSection(screen, &bi, DIB_RGB_COLORS, &g.blurBits, nullptr, 0);
    if (g.blurBmp) {
        HGDIOBJ oldSmall = SelectObject(small, g.blurBmp);
        SetStretchBltMode(small, HALFTONE);
        SetBrushOrgEx(small, 0, 0, nullptr);
        StretchBlt(small, 0, 0, g.blurW, g.blurH, mem, 0, 0, g.W, g.H, SRCCOPY);
        SelectObject(small, oldSmall);
    }
    DeleteDC(small);
    SelectObject(mem, old);
    DeleteDC(mem);
    ReleaseDC(nullptr, screen);
    GdiFlush();
    return true;
}

void ConfineCursor() {
    if (g_set.confineCursor && g.wnd && IsWindowVisible(g.wnd)) ClipCursor(&g.mon);
}

void ShowOverlayAgain() {
    ShowWindow(g.wnd, SW_SHOW);
    SetForegroundWindow(g.wnd);
    SetFocus(g.wnd);
    ConfineCursor();
    Redraw();
}

void EndSession() {
    g.mode = Mode::None;
    g.animating = g.closing = g.dragging = g.typing = g.cropDragging = false;
    g.shapes.clear();
    ClipCursor(nullptr);
    if (g.wnd) {
        HWND wnd = g.wnd;
        g.wnd = nullptr;
        g.res = DeviceRes{};
        g.hwndRt.Reset();
        DestroyWindow(wnd);
    }
    FreeScreenCapture();
}

void LeaveSession() {
    if (g.mode == Mode::None) return;
    g.dragging = g.typing = false;
    if (g_set.animateZoomOut && g.zoom > 1.001f && g.mode != Mode::Crop) {
        g.shrinkOrigin = CurrentOrigin();
        g.shrinkZoom = g.zoom;
        g.originMode = OriginMode::Shrink;
        StartAnimation(1.0f, g_set.animationTime, true);
    } else {
        EndSession();
    }
}

LRESULT CALLBACK OverlayWndProc(HWND, UINT, WPARAM, LPARAM);

bool StartSession(Mode mode, bool snipOnly, CropAction snipAction) {
    if (g.mode != Mode::None) return false;
    if (!CaptureMonitor()) {
        Wh_Log(L"Screen capture failed");
        return false;
    }
    g.wnd = CreateWindowExW(WS_EX_TOPMOST | WS_EX_TOOLWINDOW, L"WindhawkZoomItOverlay",
                            L"ZoomIt", WS_POPUP, g.mon.left, g.mon.top, g.W, g.H,
                            nullptr, nullptr, GetModuleHandleW(nullptr), nullptr);
    if (!g.wnd) {
        FreeScreenCapture();
        return false;
    }
    // Cloak until the first frame is drawn, so the screen never flashes.
    BOOL cloak = TRUE;
    DwmSetWindowAttribute(g.wnd, DWMWA_CLOAK, &cloak, sizeof(cloak));

    GetCursorPos(&g.cursor);
    g.cursor.x -= g.mon.left;
    g.cursor.y -= g.mon.top;
    g.zoom = 1;
    g.originMode = OriginMode::Follow;
    g.shapes.clear();
    g.board = 0;
    g.colorIndex = g_set.penColor;
    g.pen = PenKind::Pen;
    g.penWidth = g_set.penWidth * g.dpiScale;
    g.fontSize = g_set.fontSize * g.dpiScale;
    g.wheelRemainder = 0;
    g.sessionEndsAfterCrop = snipOnly;
    g.mode = mode;
    g.drawFromZoom = false;
    if (snipOnly) {
        g.cropAction = snipAction;
        g.cropReturnMode = Mode::None;
        g.cropDragging = false;
    }
    if (mode == Mode::Draw) FreezeView();

    if (!EnsureWindowTarget()) {
        EndSession();
        return false;
    }
    ShowWindow(g.wnd, SW_SHOW);
    RenderScene(g.res, true);
    cloak = FALSE;
    DwmSetWindowAttribute(g.wnd, DWMWA_CLOAK, &cloak, sizeof(cloak));
    SetForegroundWindow(g.wnd);
    SetFocus(g.wnd);
    ConfineCursor();
    if (mode == Mode::Zoom) SetZoom(g_set.zoomLevel, g_set.animateZoom);
    return true;
}

void EnterCrop(CropAction action) {
    if (g.mode == Mode::Crop) return;
    g.dragging = false;
    if (g.typing) {
        g.typing = false;
        if (!g.textShape.text.empty()) g.shapes.push_back(g.textShape);
    }
    if (g.mode == Mode::Zoom) FreezeView();
    g.cropReturnMode = g.mode;
    g.cropAction = action;
    g.cropDragging = false;
    g.mode = Mode::Crop;
    Redraw();
}

void LeaveCrop() {
    g.cropDragging = false;
    if (g.sessionEndsAfterCrop || g.cropReturnMode == Mode::None) {
        EndSession();
        return;
    }
    g.mode = g.cropReturnMode;
    if (g.mode == Mode::Zoom) g.originMode = OriginMode::Follow;
    Redraw();
}

void FinishCrop() {
    RECT r{std::min(g.cropA.x, g.cropB.x), std::min(g.cropA.y, g.cropB.y),
           std::max(g.cropA.x, g.cropB.x) + 1, std::max(g.cropA.y, g.cropB.y) + 1};
    r.left = std::max(0L, r.left);
    r.top = std::max(0L, r.top);
    r.right = std::min((LONG)g.W, r.right);
    r.bottom = std::min((LONG)g.H, r.bottom);
    g.cropDragging = false;
    if (r.right - r.left < 3 || r.bottom - r.top < 3) {
        Redraw();
        return;
    }
    // Render without the crop shading.
    Mode back = g.cropReturnMode == Mode::None ? Mode::Draw : g.cropReturnMode;
    bool endAfter = g.sessionEndsAfterCrop;
    g.mode = back;
    if (endAfter) {
        // Save dialogs come after the overlay is gone.
        ShowWindow(g.wnd, SW_HIDE);
        g.sessionEndsAfterCrop = true;
    }
    Export(g.cropAction, r);
    if (endAfter || !g.wnd) {
        EndSession();
        return;
    }
    if (g.mode == Mode::Zoom) g.originMode = OriginMode::Follow;
    Redraw();
}

void CommitText() {
    if (!g.typing) return;
    g.typing = false;
    if (!g.textShape.text.empty()) g.shapes.push_back(g.textShape);
    Redraw();
}

void EnsureDrawMode() {
    if (g.mode == Mode::Zoom) {
        FreezeView();
        g.mode = Mode::Draw;
        g.drawFromZoom = true;
    }
}

void StartTyping(bool rightAlign) {
    EnsureDrawMode();
    if (g.mode != Mode::Draw) return;
    CommitText();
    g.textShape = Shape{};
    g.textShape.kind = ShapeKind::Text;
    g.textShape.color = PenColorRef(g.colorIndex);
    g.textShape.fontSize = g.fontSize / g.zoom;
    g.textShape.rightAlign = rightAlign;
    g.textShape.pts = {WinToImg(g.cursor)};
    g.typing = true;
    Redraw();
}

ShapeKind KindFromKeys() {
    bool ctrl = GetKeyState(VK_CONTROL) < 0, shift = GetKeyState(VK_SHIFT) < 0;
    if (ctrl && shift) return ShapeKind::Arrow;
    if (ctrl) return ShapeKind::Rect;
    if (shift) return ShapeKind::Line;
    if (GetKeyState(VK_TAB) < 0) return ShapeKind::Ellipse;
    return ShapeKind::Free;
}

void BeginStroke() {
    g.current = Shape{};
    g.current.pen = g.pen;
    g.current.color = PenColorRef(g.colorIndex);
    float w = g.penWidth;
    if (g.pen == PenKind::Highlighter) w *= g_set.highlighterWidth;
    if (g.pen == PenKind::Blur) w *= g_set.blurWidth;
    g.current.width = w / g.zoom;
    g.current.kind = KindFromKeys();
    g.dragStartedFree = g.current.kind == ShapeKind::Free;
    g.current.pts = {WinToImg(g.cursor)};
    g.dragging = true;
}

void ContinueStroke() {
    D2D1_POINT_2F p = WinToImg(g.cursor);
    ShapeKind kind = KindFromKeys();
    if (kind != ShapeKind::Free) {
        // Holding a modifier turns the stroke into that shape from its start.
        g.current.kind = kind;
        g.current.pts = {g.current.pts.front(), p};
        return;
    }
    if (g.current.kind == ShapeKind::Free) {
        D2D1_POINT_2F last = g.current.pts.back();
        float dx = p.x - last.x, dy = p.y - last.y;
        if (dx * dx + dy * dy * g.zoom * g.zoom >= 0.25f) g.current.pts.push_back(p);
    } else {
        g.current.pts = {g.current.pts.front(), p};
    }
}

void EndStroke() {
    if (!g.dragging) return;
    g.dragging = false;
    if (g.current.kind != ShapeKind::Free && g.current.pts.size() < 2) {
        g.current.pts.push_back(g.current.pts.front());
    }
    g.shapes.push_back(std::move(g.current));
    Redraw();
}

void ChangePenOrFont(int steps) {
    if (g.typing) {
        g.fontSize = ClampF(g.fontSize * std::pow(1.1f, (float)steps), 6, 600);
        g.textShape.fontSize = g.fontSize / g.zoom;
    } else {
        float w = g.penWidth;
        float next = steps > 0 ? std::max(w + 1, w * 1.15f) : std::min(w - 1, w / 1.15f);
        g.penWidth = ClampF(next, 1, 300);
    }
    Redraw();
}

void HandleKey(WPARAM vk) {
    bool ctrl = GetKeyState(VK_CONTROL) < 0, shift = GetKeyState(VK_SHIFT) < 0;
    if (g.typing) {
        switch (vk) {
            case VK_ESCAPE: CommitText(); break;
            case VK_UP: ChangePenOrFont(1); break;
            case VK_DOWN: ChangePenOrFont(-1); break;
        }
        return;
    }
    if (g.mode == Mode::Crop) {
        if (vk == VK_ESCAPE) LeaveCrop();
        return;
    }
    switch (vk) {
        case VK_ESCAPE:
            LeaveSession();
            return;
        case VK_UP:
        case VK_DOWN: {
            int dir = vk == VK_UP ? 1 : -1;
            if (ctrl) {
                ChangePenOrFont(dir);
            } else {
                float t = ZoomTarget();
                SetZoom(dir > 0 ? t * g_set.zoomStep : t / g_set.zoomStep, true);
            }
            return;
        }
        case VK_SPACE: {
            SetCursorPos(g.mon.left + g.W / 2, g.mon.top + g.H / 2);
            return;
        }
    }
    if (ctrl) {
        switch (vk) {
            case 'Z':
                if (!g.shapes.empty()) g.shapes.pop_back();
                Redraw();
                return;
            case 'C':
            case 'S': {
                CropAction action = vk == 'C' ? CropAction::Copy : CropAction::Save;
                if (shift) {
                    EnterCrop(action);
                } else {
                    Export(action, RECT{0, 0, g.W, g.H});
                    if (g.wnd) Redraw();
                }
                return;
            }
        }
        return;
    }
    static const struct {
        WPARAM vk;
        int color;
    } colorKeys[] = {{'R', 0}, {'G', 1}, {'B', 2}, {'Y', 3}, {'O', 4}, {'P', 5}};
    for (auto& k : colorKeys) {
        if (vk == k.vk) {
            g.colorIndex = k.color;
            g.pen = shift ? PenKind::Highlighter : PenKind::Pen;
            Redraw();
            return;
        }
    }
    switch (vk) {
        case 'X':
            g.pen = PenKind::Blur;
            Redraw();
            return;
        case 'T': {
            // Drop the "t" that TranslateMessage already queued.
            MSG pending;
            PeekMessageW(&pending, g.wnd, WM_CHAR, WM_CHAR, PM_REMOVE);
            StartTyping(shift);
            return;
        }
        case 'W':
        case 'K': {
            EnsureDrawMode();
            int want = vk == 'W' ? 1 : 2;
            g.board = g.board == want ? 0 : want;
            Redraw();
            return;
        }
        case 'E':
            g.shapes.clear();
            Redraw();
            return;
    }
}

LRESULT CALLBACK OverlayWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (hwnd != g.wnd) return DefWindowProcW(hwnd, msg, wParam, lParam);
    switch (msg) {
        case WM_PAINT: {
            PAINTSTRUCT ps;
            BeginPaint(hwnd, &ps);
            EndPaint(hwnd, &ps);
            Redraw();
            return 0;
        }
        case WM_ERASEBKGND:
            return 1;
        case WM_SETCURSOR:
            if (LOWORD(lParam) == HTCLIENT) {
                if (g.mode == Mode::Draw && !g.typing) {
                    SetCursor(nullptr);
                } else if (g.mode == Mode::Crop) {
                    SetCursor(LoadCursor(nullptr, IDC_CROSS));
                } else if (g.typing) {
                    SetCursor(LoadCursor(nullptr, IDC_IBEAM));
                } else {
                    SetCursor(LoadCursor(nullptr, IDC_ARROW));
                }
                return TRUE;
            }
            break;
        case WM_MOUSEMOVE:
            g.cursor = POINT{GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
            if (g.mode == Mode::Crop && g.cropDragging) g.cropB = g.cursor;
            if (g.dragging) ContinueStroke();
            if (!g.animating) Redraw();
            return 0;
        case WM_LBUTTONDOWN:
            g.cursor = POINT{GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
            SetCapture(hwnd);
            if (g.mode == Mode::Crop) {
                g.cropDragging = true;
                g.cropA = g.cropB = g.cursor;
            } else if (g.typing) {
                CommitText();
            } else if (!g.closing) {
                EnsureDrawMode();
                BeginStroke();
            }
            Redraw();
            return 0;
        case WM_LBUTTONUP:
            ReleaseCapture();
            if (g.mode == Mode::Crop && g.cropDragging) {
                FinishCrop();
            } else {
                EndStroke();
            }
            return 0;
        case WM_RBUTTONUP:
            if (g.typing) {
                CommitText();
            } else if (g.mode == Mode::Crop) {
                LeaveCrop();
            } else if (g.mode == Mode::Draw && g.drawFromZoom) {
                g.mode = Mode::Zoom;
                g.drawFromZoom = false;
                g.originMode = OriginMode::Follow;
                Redraw();
            } else {
                LeaveSession();
            }
            return 0;
        case WM_MOUSEWHEEL: {
            g.wheelRemainder += GET_WHEEL_DELTA_WPARAM(wParam);
            int steps = g.wheelRemainder / WHEEL_DELTA;
            g.wheelRemainder -= steps * WHEEL_DELTA;
            if (!steps || g.mode == Mode::Crop || g.closing) return 0;
            if ((GET_KEYSTATE_WPARAM(wParam) & MK_CONTROL) || g.typing) {
                ChangePenOrFont(steps);
            } else {
                float t = ZoomTarget() * std::pow(g_set.zoomStep, (float)steps);
                SetZoom(t, true);
            }
            return 0;
        }
        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
            if (!g.closing) HandleKey(wParam);
            return 0;
        case WM_CHAR:
            if (g.typing) {
                wchar_t c = (wchar_t)wParam;
                if (c == L'\b') {
                    if (!g.textShape.text.empty()) g.textShape.text.pop_back();
                } else if (c == L'\r') {
                    g.textShape.text += L'\n';
                } else if (c >= 0x20 && c != 0x7F) {
                    g.textShape.text += c;
                }
                Redraw();
            }
            return 0;
        case WM_TIMER:
            if (wParam == kTimerAnim) StepAnimation();
            return 0;
        case WM_ACTIVATE:
            if (LOWORD(wParam) == WA_INACTIVE) {
                if (!g.inDialog && g_set.leaveOnFocusLoss && !g.closing) {
                    // Posted: tearing the window down inside its own
                    // deactivation is asking for trouble.
                    PostMessageW(g.msgWnd, WM_APP_LEAVE, 0, 0);
                }
            } else {
                ConfineCursor();
            }
            return 0;
        case WM_DISPLAYCHANGE:
            PostMessageW(g.msgWnd, WM_APP_LEAVE, 1, 0);
            return 0;
        case WM_CLOSE:
            EndSession();
            return 0;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

////////////////////////////////////////////////////////////////////////////////
// Hotkeys and the background thread

void UnregisterHotkeys() {
    for (UINT id = kHotkeyZoom; id <= kHotkeySnipSave; id++) UnregisterHotKey(g.msgWnd, id);
}

void RegisterHotkeys() {
    UnregisterHotkeys();
    const std::pair<UINT, const std::wstring*> keys[] = {
        {kHotkeyZoom, &g_set.zoomHotkey},
        {kHotkeyDraw, &g_set.drawHotkey},
        {kHotkeySnip, &g_set.snipHotkey},
        {kHotkeySnipSave, &g_set.snipSaveHotkey},
    };
    bool anyFailed = false;
    for (auto& [id, text] : keys) {
        UINT mods, vk;
        if (text->empty()) continue;
        if (!ParseHotkey(*text, &mods, &vk)) {
            Wh_Log(L"Can't read hotkey \"%s\"", text->c_str());
            continue;
        }
        if (!RegisterHotKey(g.msgWnd, id, mods | MOD_NOREPEAT, vk)) {
            anyFailed = true;
            if (!g.hotkeyRetryArmed) {
                Wh_Log(L"Hotkey \"%s\" is taken by another app (error %u), retrying",
                       text->c_str(), GetLastError());
            }
        }
    }
    // Another app (PowerToys ZoomIt, for one) may hold a hotkey; keep trying
    // so it works the moment that app lets go.
    g.hotkeyRetryArmed = anyFailed;
    if (anyFailed) {
        SetTimer(g.msgWnd, kTimerRetryHotkeys, 3000, nullptr);
    } else {
        KillTimer(g.msgWnd, kTimerRetryHotkeys);
    }
}

LRESULT CALLBACK MsgWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_HOTKEY:
            if (g.inDialog) return 0;
            switch (wParam) {
                case kHotkeyZoom:
                    if (g.mode == Mode::None) {
                        StartSession(Mode::Zoom, false, CropAction::Copy);
                    } else {
                        LeaveSession();
                    }
                    break;
                case kHotkeyDraw:
                    if (g.mode == Mode::None) {
                        StartSession(Mode::Draw, false, CropAction::Copy);
                    } else if (g.mode == Mode::Zoom && !g.closing) {
                        EnsureDrawMode();
                        Redraw();
                    } else {
                        LeaveSession();
                    }
                    break;
                case kHotkeySnip:
                case kHotkeySnipSave: {
                    CropAction action =
                        wParam == kHotkeySnip ? CropAction::Copy : CropAction::Save;
                    if (g.mode == Mode::None) {
                        StartSession(Mode::Crop, true, action);
                    } else if (!g.closing) {
                        EnterCrop(action);
                    }
                    break;
                }
            }
            return 0;
        case WM_TIMER:
            if (wParam == kTimerRetryHotkeys) RegisterHotkeys();
            return 0;
        case WM_APP_SETTINGS:
            LoadSettings();
            g.hotkeyRetryArmed = false;
            RegisterHotkeys();
            return 0;
        case WM_APP_LEAVE:
            // wParam 1: the screen layout changed, always leave.
            if (g.mode != Mode::None && !g.inDialog && g.wnd &&
                (wParam == 1 || GetForegroundWindow() != g.wnd)) {
                EndSession();
            }
            return 0;
        case WM_DESTROY:
            UnregisterHotkeys();
            EndSession();
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

HANDLE g_thread;
DWORD g_threadId;
HANDLE g_ready;

DWORD WINAPI ThreadProc(void*) {
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);

    D2D1_FACTORY_OPTIONS options{};
    D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, __uuidof(ID2D1Factory1),
                      &options, (void**)g.d2d.GetAddressOf());
    DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory),
                        (IUnknown**)g.dwrite.GetAddressOf());
    CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER,
                     IID_PPV_ARGS(&g.wic));
    if (g.d2d) {
        g.d2d->CreateStrokeStyle(
            D2D1::StrokeStyleProperties(D2D1_CAP_STYLE_ROUND, D2D1_CAP_STYLE_ROUND,
                                        D2D1_CAP_STYLE_ROUND, D2D1_LINE_JOIN_ROUND),
            nullptr, 0, &g.roundStroke);
        g.d2d->CreateStrokeStyle(
            D2D1::StrokeStyleProperties(D2D1_CAP_STYLE_SQUARE, D2D1_CAP_STYLE_SQUARE,
                                        D2D1_CAP_STYLE_SQUARE, D2D1_LINE_JOIN_ROUND),
            nullptr, 0, &g.flatStroke);
    }
    if (!g.d2d || !g.dwrite || !g.wic) {
        Wh_Log(L"Direct2D, DirectWrite or WIC is missing");
        SetEvent(g_ready);
        CoUninitialize();
        return 1;
    }

    HINSTANCE inst = GetModuleHandleW(nullptr);
    WNDCLASSW wc{};
    wc.lpfnWndProc = MsgWndProc;
    wc.hInstance = inst;
    wc.lpszClassName = L"WindhawkZoomItMessages";
    RegisterClassW(&wc);
    WNDCLASSW oc{};
    oc.lpfnWndProc = OverlayWndProc;
    oc.hInstance = inst;
    oc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    oc.lpszClassName = L"WindhawkZoomItOverlay";
    RegisterClassW(&oc);

    g.msgWnd = CreateWindowExW(0, wc.lpszClassName, L"", 0, 0, 0, 0, 0, HWND_MESSAGE,
                               nullptr, inst, nullptr);
    LoadSettings();
    RegisterHotkeys();
    SetEvent(g_ready);

    MSG msg;
    while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    g.res = DeviceRes{};
    g.hwndRt.Reset();
    g.roundStroke.Reset();
    g.flatStroke.Reset();
    g.d2d.Reset();
    g.dwrite.Reset();
    g.wic.Reset();
    UnregisterClassW(oc.lpszClassName, inst);
    UnregisterClassW(wc.lpszClassName, inst);
    CoUninitialize();
    return 0;
}

}  // namespace

BOOL WhTool_ModInit() {
    Wh_Log(L">");
    g_ready = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_thread = CreateThread(nullptr, 0, ThreadProc, nullptr, 0, &g_threadId);
    if (!g_thread) return FALSE;
    WaitForSingleObject(g_ready, 10000);
    return TRUE;
}

void WhTool_ModSettingsChanged() {
    Wh_Log(L">");
    if (g.msgWnd) PostMessageW(g.msgWnd, WM_APP_SETTINGS, 0, 0);
}

void WhTool_ModUninit() {
    Wh_Log(L">");
    if (g.msgWnd) PostMessageW(g.msgWnd, WM_CLOSE, 0, 0);
    if (g_thread) {
        WaitForSingleObject(g_thread, 5000);
        CloseHandle(g_thread);
    }
    ClipCursor(nullptr);
}

#if !__has_include(<windhawk_tool_mod.h>)

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

#endif
