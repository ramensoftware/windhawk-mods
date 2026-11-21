// ==WindhawkMod==
// @id              custom-dex-clock
// @name            Custom Dex Clock
// @description     Lightweight, fully customizable desktop clock widget 
// @version         5.0
// @author          raj
// @include         windhawk.exe
// @github          https://github.com/raj-customdex
// @compilerOptions -ld2d1 -ldwrite -lgdi32 -luser32 -lshell32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Custom Dex Clock v5.0

A modern desktop clock widget 
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- fontFamily: Quicksand
  $name: Font family
- monitorMode: all
  $name: Show on
  $options:
  - all: All monitors
  - primary: Primary monitor only
  - specific: Specific monitor number
- monitorNumber: 1
  $name: Monitor number (when "Specific" is selected, 1-based)
- lockWidgetPosition: false
  $name: Lock position (prevents dragging)
- syncPositions: true
  $name: Sync positions across monitors
- use24h: true
  $name: 24-hour clock
- showSeconds: false
  $name: Show seconds
- tightVerticalBounds: true
  $name: Tight vertical bounds for hero / overlay / standing text

- heroEnabled: true
  $name: Hero - enabled
- heroTokens: date
  $name: Hero - tokens
- heroCase: none
  $name: Hero - text case
  $options:
  - none: Unchanged
  - upper: UPPERCASE
  - lower: lowercase
- heroCustom: ""
  $name: Hero - custom text (overrides tokens)
- heroSize: 140
  $name: Hero - font size (px)
- heroWeight: 700
  $name: Hero - font weight (100-900)
- heroSpacing: 4
  $name: Hero - letter spacing (px)
- heroColor: "#FFD700"
  $name: Hero - colour
- heroOpacity: 100
  $name: Hero - opacity (%)
- heroGradient: true
  $name: Hero - use vertical gradient
- heroGradTop: "#FFE599"
  $name: Hero - gradient top (brighter golden hue)
- heroGradMid: "#D4AF37"
  $name: Hero - gradient middle
- heroGradBottom: "#B8860B"
  $name: Hero - gradient bottom (deeper brownish-gold)
- heroShadow: true
  $name: Hero - drop shadow
- heroShadowX: 0
  $name: Hero - shadow offset X
- heroShadowY: 4
  $name: Hero - shadow offset Y
- heroShadowColor: "#80000000"
  $name: Hero - shadow colour

- overlayEnabled: true
  $name: Overlay - enabled
- overlayTokens: month
  $name: Overlay - tokens
- overlayCase: upper
  $name: Overlay - text case
  $options:
  - none: Unchanged
  - upper: UPPERCASE
  - lower: lowercase
- overlayCustom: "JULY"
  $name: Overlay - custom text (e.g. JULY overlaid across center of hero digits)
- overlaySize: 28
  $name: Overlay - font size (px)
- overlayWeight: 800
  $name: Overlay - font weight
- overlaySpacing: 6
  $name: Overlay - letter spacing (px)
- overlayColor: "#FFFFFF"
  $name: Overlay - colour
- overlayOpacity: 92
  $name: Overlay - opacity (%)
- overlayShadow: true
  $name: Overlay - drop shadow
- overlayShadowX: 0
  $name: Overlay - shadow offset X
- overlayShadowY: 2
  $name: Overlay - shadow offset Y
- overlayShadowColor: "#80000000"
  $name: Overlay - shadow colour
- overlayOffsetX: 0
  $name: Overlay - position offset X
- overlayOffsetY: 0
  $name: Overlay - position offset Y

- standingEnabled: true
  $name: Standing - enabled
- standingTokens: weekday
  $name: Standing - tokens
- standingCase: upper
  $name: Standing - text case
  $options:
  - none: Unchanged
  - upper: UPPERCASE
  - lower: lowercase
- standingCustom: "TUESDAY"
  $name: Standing - custom text (e.g. TUESDAY vertically aligned along the left zero curve)
- standingSide: left
  $name: Standing - side
  $options:
  - left: Left
  - right: Right
- standingOrientation: vertical
  $name: Standing - orientation
  $options:
  - vertical: Vertical
  - horizontal: Horizontal
- standingCurve: 12
  $name: Standing - curve arc (px, vertical only, 0 = straight)
- standingGap: 14
  $name: Standing - gap to hero (px)
- standingSize: 12
  $name: Standing - font size (px)
- standingWeight: 600
  $name: Standing - font weight
- standingSpacing: 4
  $name: Standing - letter spacing (px)
- standingColor: "#FFFFFF"
  $name: Standing - colour
- standingOpacity: 85
  $name: Standing - opacity (%)
- standingShadow: true
  $name: Standing - drop shadow
- standingShadowX: 0
  $name: Standing - shadow offset X
- standingShadowY: 1
  $name: Standing - shadow offset Y
- standingShadowColor: "#66000000"
  $name: Standing - shadow colour

- row2Enabled: false
  $name: 2nd row - enabled
- row2Tokens: year
  $name: 2nd row - tokens
- row2Case: none
  $name: 2nd row - text case
  $options:
  - none: Unchanged
  - upper: UPPERCASE
  - lower: lowercase
- row2Custom: ""
  $name: 2nd row - custom text
- row2Size: 24
  $name: 2nd row - font size (px)
- row2Weight: 500
  $name: 2nd row - font weight
- row2Spacing: 2
  $name: 2nd row - letter spacing (px)
- row2Color: "#FFFFFF"
  $name: 2nd row - colour
- row2Opacity: 90
  $name: 2nd row - opacity (%)
- row2Shadow: true
  $name: 2nd row - drop shadow
- row2ShadowX: 0
  $name: 2nd row - shadow offset X
- row2ShadowY: 2
  $name: 2nd row - shadow offset Y
- row2ShadowColor: "#66000000"
  $name: 2nd row - shadow colour
- row2Gap: 8
  $name: 2nd row - gap above (px)

- row3Enabled: true
  $name: 3rd row - enabled
- row3Tokens: time
  $name: 3rd row - tokens
- row3Case: none
  $name: 3rd row - text case
  $options:
  - none: Unchanged
  - upper: UPPERCASE
  - lower: lowercase
- row3Custom: ""
  $name: 3rd row - custom text
- row3Size: 18
  $name: 3rd row - font size (px)
- row3Weight: 300
  $name: 3rd row - font weight
- row3Spacing: 2
  $name: 3rd row - letter spacing (px)
- row3Color: "#FFFFFF"
  $name: 3rd row - colour
- row3Opacity: 95
  $name: 3rd row - opacity (%)
- row3Shadow: true
  $name: 3rd row - drop shadow
- row3ShadowX: 0
  $name: 3rd row - shadow offset X
- row3ShadowY: 2
  $name: 3rd row - shadow offset Y
- row3ShadowColor: "#66000000"
  $name: 3rd row - shadow colour
- row3Gap: 8
  $name: 3rd row - gap above (px)
*/
// ==/WindhawkModSettings==

//congrats-to-code
#include <windows.h>
#include <shellapi.h>
#include <d2d1.h>
#include <dwrite.h>
#include <dwrite_1.h>
#include <windhawk_api.h>
#include <windhawk_utils.h>

#include <algorithm>
#include <cmath>
#include <memory>
#include <string>
#include <vector>

namespace {

constexpr UINT WM_APP_RELOAD = WM_APP + 1;
constexpr UINT WM_APP_QUIT = WM_APP + 2;
constexpr UINT_PTR TIMER_TICK = 1;
constexpr int kNoPos = 0x7FFFFFFF;
constexpr float kPi = 3.14159265358979f;

const wchar_t kWidgetClass[] = L"WindhawkCustomDexClockWidgetV6";
const wchar_t kCtrlClass[] = L"WindhawkCustomDexClockCtrlV6";

struct Layer {
    bool enabled = false;
    std::wstring tokens;
    std::wstring custom;
    int caseMode = 0;  // 0 none, 1 upper, 2 lower
    float size = 24;
    int weight = 400;
    float spacing = 0;
    D2D1_COLOR_F color = {1, 1, 1, 1};
    bool shadow = false;
    float shX = 0, shY = 0;
    D2D1_COLOR_F shColor = {0, 0, 0, 0.5f};
    bool gradient = false;
    D2D1_COLOR_F gTop = {1, 1, 1, 1}, gMid = {1, 1, 1, 1}, gBot = {1, 1, 1, 1};
    float gap = 0;
    float offX = 0, offY = 0;
};

struct Settings {
    std::wstring font = L"Quicksand";
    int monitorMode = 0;  // 0 all, 1 primary, 2 specific
    int monitorNumber = 1;
    bool lock = false;
    bool sync = true;
    bool use24h = true;
    bool showSeconds = false;
    bool tight = true;
    bool needSeconds = false;
    Layer hero, overlay, standing, row2, row3;
    int standSide = 0;  // 0 left, 1 right
    bool standVertical = true;
    float standCurve = 0;
};

Settings g_s;
ID2D1Factory* g_d2d = nullptr;
IDWriteFactory* g_dw = nullptr;
HWND g_ctrl = nullptr;
HANDLE g_thread = nullptr;
HANDLE g_ready = nullptr;

std::wstring GetStr(const std::wstring& name) {
    PCWSTR s = Wh_GetStringSetting(name.c_str());
    std::wstring r = s ? s : L"";
    Wh_FreeStringSetting(s);
    return r;
}

int GetInt(const std::wstring& name) { return Wh_GetIntSetting(name.c_str()); }

D2D1_COLOR_F ParseColor(const std::wstring& text, int opacityPct) {
    std::wstring h = text;
    if (!h.empty() && h[0] == L'#') h.erase(0, 1);
    unsigned long v = wcstoul(h.c_str(), nullptr, 16);
    float a = 1.0f;
    if (h.size() == 8) a = ((v >> 24) & 0xFF) / 255.0f;
    float r = ((v >> 16) & 0xFF) / 255.0f;
    float g = ((v >> 8) & 0xFF) / 255.0f;
    float b = (v & 0xFF) / 255.0f;
    a *= std::clamp(opacityPct, 0, 100) / 100.0f;
    return D2D1::ColorF(r, g, b, a);
}

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

void LoadLayer(Layer& l, const std::wstring& p, bool hasGradient) {
    l.enabled = GetInt(p + L"Enabled") != 0;
    l.tokens = GetStr(p + L"Tokens");
    std::wstring cs = GetStr(p + L"Case");
    l.caseMode = cs == L"upper" ? 1 : (cs == L"lower" ? 2 : 0);
    l.custom = GetStr(p + L"Custom");
    l.size = (float)std::max(1, GetInt(p + L"Size"));
    l.weight = std::clamp(GetInt(p + L"Weight"), 100, 900);
    l.spacing = (float)GetInt(p + L"Spacing");
    int op = std::clamp(GetInt(p + L"Opacity"), 0, 100);
    l.color = ParseColor(GetStr(p + L"Color"), op);
    l.shadow = GetInt(p + L"Shadow") != 0;
    l.shX = (float)GetInt(p + L"ShadowX");
    l.shY = (float)GetInt(p + L"ShadowY");
    l.shColor = ParseColor(GetStr(p + L"ShadowColor"), 100);
    if (hasGradient) {
        l.gradient = GetInt(p + L"Gradient") != 0;
        l.gTop = ParseColor(GetStr(p + L"GradTop"), op);
        l.gMid = ParseColor(GetStr(p + L"GradMid"), op);
        l.gBot = ParseColor(GetStr(p + L"GradBottom"), op);
    }
}

bool UsesTime(const Layer& l) {
    return l.enabled && l.custom.empty() &&
           Lower(l.tokens).find(L"time") != std::wstring::npos;
}

void LoadSettings() {
    Settings s;
    s.font = GetStr(L"fontFamily");
    if (s.font.empty()) s.font = L"Quicksand";
    std::wstring mm = GetStr(L"monitorMode");
    s.monitorMode = mm == L"primary" ? 1 : (mm == L"specific" ? 2 : 0);
    s.monitorNumber = std::max(1, GetInt(L"monitorNumber"));
    s.lock = GetInt(L"lockWidgetPosition") != 0;
    s.sync = GetInt(L"syncPositions") != 0;
    s.use24h = GetInt(L"use24h") != 0;
    s.showSeconds = GetInt(L"showSeconds") != 0;
    s.tight = GetInt(L"tightVerticalBounds") != 0;

    LoadLayer(s.hero, L"hero", true);
    LoadLayer(s.overlay, L"overlay", false);
    LoadLayer(s.standing, L"standing", false);
    LoadLayer(s.row2, L"row2", false);
    LoadLayer(s.row3, L"row3", false);

    s.overlay.offX = (float)GetInt(L"overlayOffsetX");
    s.overlay.offY = (float)GetInt(L"overlayOffsetY");
    s.standing.gap = (float)GetInt(L"standingGap");
    s.row2.gap = (float)GetInt(L"row2Gap");
    s.row3.gap = (float)GetInt(L"row3Gap");
    s.standSide = GetStr(L"standingSide") == L"right" ? 1 : 0;
    s.standVertical = GetStr(L"standingOrientation") != L"horizontal";
    s.standCurve = (float)std::max(0, GetInt(L"standingCurve"));

    s.needSeconds = s.showSeconds &&
                    (UsesTime(s.hero) || UsesTime(s.overlay) ||
                     UsesTime(s.standing) || UsesTime(s.row2) ||
                     UsesTime(s.row3));
    g_s = s;
}

std::wstring DateFmt(const wchar_t* fmt, const SYSTEMTIME& st) {
    wchar_t buf[128];
    int n = GetDateFormatEx(LOCALE_NAME_USER_DEFAULT, 0, &st, fmt, buf, 128,
                            nullptr);
    return n > 0 ? std::wstring(buf) : std::wstring();
}

std::wstring TimeText(const SYSTEMTIME& st) {
    const wchar_t* fmt;
    if (g_s.use24h)
        fmt = g_s.showSeconds ? L"HH:mm:ss" : L"HH:mm";
    else
        fmt = g_s.showSeconds ? L"hh:mm:ss tt" : L"hh:mm tt";
    wchar_t buf[64];
    int n = GetTimeFormatEx(LOCALE_NAME_USER_DEFAULT, 0, &st, fmt, buf, 64);
    return n > 0 ? std::wstring(buf) : std::wstring();
}

std::wstring TokenText(const std::wstring& t, const SYSTEMTIME& st) {
    if (t == L"date") {
        wchar_t b[8];
        wsprintfW(b, L"%02d", (int)st.wDay);
        return b;
    }
    if (t == L"month") return DateFmt(L"MMMM", st);
    if (t == L"shortmonth") return DateFmt(L"MMM", st);
    if (t == L"year") return DateFmt(L"yyyy", st);
    if (t == L"weekday") return DateFmt(L"dddd", st);
    if (t == L"shortweekday") return DateFmt(L"ddd", st);
    if (t == L"time") return TimeText(st);
    return t;
}

std::wstring ComposeText(const Layer& l, const SYSTEMTIME& st) {
    std::wstring out;
    if (!l.custom.empty()) {
        out = l.custom;
    } else {
        size_t pos = 0;
        const std::wstring& src = l.tokens;
        while (pos <= src.size()) {
            size_t e = src.find(L'+', pos);
            if (e == std::wstring::npos) e = src.size();
            std::wstring tok = Lower(Trim(src.substr(pos, e - pos)));
            std::wstring piece = TokenText(tok, st);
            if (!piece.empty()) {
                if (!out.empty()) out += L' ';
                out += piece;
            }
            pos = e + 1;
        }
    }
    if (!out.empty()) {
        if (l.caseMode == 1)
            CharUpperBuffW(&out[0], (DWORD)out.size());
        else if (l.caseMode == 2)
            CharLowerBuffW(&out[0], (DWORD)out.size());
    }
    return out;
}

struct TextBlock {
    IDWriteTextLayout* layout = nullptr;
    float w = 0, h = 0, topPad = 0;
    void Release() {
        if (layout) {
            layout->Release();
            layout = nullptr;
        }
    }
};

struct Standing {
    std::vector<TextBlock> chars;
    TextBlock whole;
    bool vertical = false;
    float len = 0, thick = 0, extW = 0, extH = 0;
    bool Valid() const { return whole.layout || !chars.empty(); }
    void Release() {
        whole.Release();
        for (auto& c : chars) c.Release();
        chars.clear();
    }
};

bool MakeBlock(TextBlock& b, const std::wstring& text, float size, int weight,
               float spacing, bool tight) {
    b = TextBlock();
    if (text.empty() || !g_dw) return false;

    IDWriteTextFormat* fmt = nullptr;
    if (FAILED(g_dw->CreateTextFormat(
            g_s.font.c_str(), nullptr, (DWRITE_FONT_WEIGHT)weight,
            DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, size,
            L"en-us", &fmt)))
        return false;
    fmt->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);

    UINT32 len = (UINT32)text.size();
    HRESULT hr = g_dw->CreateTextLayout(text.c_str(), len, fmt, 16384.0f,
                                        16384.0f, &b.layout);
    fmt->Release();
    if (FAILED(hr)) {
        b.layout = nullptr;
        return false;
    }

    if (spacing != 0) {
        IDWriteTextLayout1* l1 = nullptr;
        if (SUCCEEDED(b.layout->QueryInterface(IID_PPV_ARGS(&l1)))) {
            DWRITE_TEXT_RANGE range = {0, len};
            l1->SetCharacterSpacing(0, spacing, 0, range);
            l1->Release();
        }
    }

    DWRITE_TEXT_METRICS tm = {};
    b.layout->GetMetrics(&tm);
    b.w = tm.widthIncludingTrailingWhitespace - spacing;
    if (b.w < 0) b.w = 0;
    b.h = tm.height;

    if (tight) {
        DWRITE_LINE_METRICS lm = {};
        UINT32 cnt = 0;
        b.layout->GetLineMetrics(&lm, 1, &cnt);
        float capH = size * 0.72f;
        b.topPad = lm.baseline - capH;
        b.h = capH;
    }
    return true;
}

ID2D1Brush* MakeBrush(ID2D1RenderTarget* rt, const Layer& l, float top,
                      float h, bool allowGradient) {
    if (allowGradient && l.gradient) {
        D2D1_GRADIENT_STOP stops[3] = {{0.0f, l.gTop}, {0.5f, l.gMid},
                                       {1.0f, l.gBot}};
        ID2D1GradientStopCollection* coll = nullptr;
        if (SUCCEEDED(rt->CreateGradientStopCollection(stops, 3, &coll))) {
            ID2D1LinearGradientBrush* lb = nullptr;
            HRESULT hr = rt->CreateLinearGradientBrush(
                D2D1::LinearGradientBrushProperties(
                    D2D1::Point2F(0, top), D2D1::Point2F(0, top + h)),
                coll, &lb);
            coll->Release();
            if (SUCCEEDED(hr)) return lb;
        }
    }
    ID2D1SolidColorBrush* sb = nullptr;
    rt->CreateSolidColorBrush(l.color, &sb);
    return sb;
}

void DrawBlock(ID2D1RenderTarget* rt, const Layer& l, const TextBlock& b,
               float cx, float top, float scale, bool allowGradient) {
    if (!b.layout) return;
    float x = cx - b.w / 2;
    float y = top - b.topPad;

    if (l.shadow) {
        ID2D1SolidColorBrush* sh = nullptr;
        if (SUCCEEDED(rt->CreateSolidColorBrush(l.shColor, &sh))) {
            rt->DrawTextLayout(
                D2D1::Point2F(x + l.shX * scale, y + l.shY * scale), b.layout,
                sh);
            sh->Release();
        }
    }
    ID2D1Brush* br = MakeBrush(rt, l, top, b.h, allowGradient);
    if (br) {
        rt->DrawTextLayout(D2D1::Point2F(x, y), b.layout, br);
        br->Release();
    }
}

void DrawStandingVertical(ID2D1RenderTarget* rt, const Layer& l,
                          const Standing& sb, bool right, float bx, float by,
                          float scale, float arc) {
    const float L = sb.len;
    const float T = sb.thick;
    const float xc0 = right ? bx + T / 2 : bx + arc + T / 2;

    ID2D1SolidColorBrush* brush = nullptr;
    ID2D1SolidColorBrush* shadow = nullptr;
    rt->CreateSolidColorBrush(l.color, &brush);
    if (l.shadow) rt->CreateSolidColorBrush(l.shColor, &shadow);

    float s = 0;
    for (const auto& c : sb.chars) {
        float sc = s + c.w / 2;
        float t = L > 0 ? (sc - L / 2) / (L / 2) : 0;
        float o = arc * (1 - t * t);
        float dodS = L > 0 ? -4.0f * arc * t / L : 0;

        float px, py, ang;
        if (!right) {
            px = xc0 - o;
            py = by + L - sc;
            ang = atan2f(-1.0f, -dodS);
        } else {
            px = xc0 + o;
            py = by + sc;
            ang = atan2f(1.0f, dodS);
        }
        float deg = ang * 180.0f / kPi;
        D2D1_POINT_2F origin = D2D1::Point2F(-c.w / 2, -T / 2 - c.topPad);

        if (c.layout) {
            if (shadow) {
                rt->SetTransform(
                    D2D1::Matrix3x2F::Rotation(deg, D2D1::Point2F(0, 0)) *
                    D2D1::Matrix3x2F::Translation(px + l.shX * scale,
                                                  py + l.shY * scale));
                rt->DrawTextLayout(origin, c.layout, shadow);
            }
            if (brush) {
                rt->SetTransform(
                    D2D1::Matrix3x2F::Rotation(deg, D2D1::Point2F(0, 0)) *
                    D2D1::Matrix3x2F::Translation(px, py));
                rt->DrawTextLayout(origin, c.layout, brush);
            }
        }
        s += c.w + l.spacing * scale;
    }
    rt->SetTransform(D2D1::Matrix3x2F::Identity());
    if (brush) brush->Release();
    if (shadow) shadow->Release();
}

struct Widget {
    HWND hwnd = nullptr;
    HMONITOR mon = nullptr;
    int index = 0;
    RECT monRect = {};
    HDC memdc = nullptr;
    HBITMAP dib = nullptr;
    int w = 0, h = 0;
    ID2D1DCRenderTarget* rt = nullptr;
    std::wstring lastKey;
    bool placed = false;
};

std::vector<std::unique_ptr<Widget>> g_widgets;

std::wstring PosKey(int index, wchar_t axis) {
    return L"pos" + std::to_wstring(index) + axis;
}

void SavePos(const Widget& w, int ox, int oy) {
    Wh_SetIntValue(PosKey(w.index, L'x').c_str(), ox);
    Wh_SetIntValue(PosKey(w.index, L'y').c_str(), oy);
}

bool EnsureRT(Widget& w) {
    if (w.rt) return true;
    D2D1_RENDER_TARGET_PROPERTIES p = D2D1::RenderTargetProperties(
        D2D1_RENDER_TARGET_TYPE_DEFAULT,
        D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM,
                          D2D1_ALPHA_MODE_PREMULTIPLIED),
        96.0f, 96.0f, D2D1_RENDER_TARGET_USAGE_NONE,
        D2D1_FEATURE_LEVEL_DEFAULT);
    if (FAILED(g_d2d->CreateDCRenderTarget(&p, &w.rt))) return false;
    w.rt->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE);
    return true;
}

bool EnsureSurface(Widget& w, int W, int H) {
    if (w.dib && w.w == W && w.h == H) return true;
    if (!w.memdc) w.memdc = CreateCompatibleDC(nullptr);
    if (!w.memdc) return false;

    BITMAPINFO bi = {};
    bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bi.bmiHeader.biWidth = W;
    bi.bmiHeader.biHeight = -H;
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;

    void* bits = nullptr;
    HBITMAP nb =
        CreateDIBSection(w.memdc, &bi, DIB_RGB_COLORS, &bits, nullptr, 0);
    if (!nb) return false;
    SelectObject(w.memdc, nb);
    if (w.dib) DeleteObject(w.dib);
    w.dib = nb;
    w.w = W;
    w.h = H;
    return true;
}

void Render(Widget& wd, bool force) {
    if (!wd.hwnd || !g_d2d || !g_dw) return;

    SYSTEMTIME st;
    GetLocalTime(&st);

    UINT dpi = GetDpiForWindow(wd.hwnd);
    float scale = dpi ? dpi / 96.0f : 1.0f;
    auto S = [&](float v) { return v * scale; };

    std::wstring heroT = g_s.hero.enabled ? ComposeText(g_s.hero, st) : L"";
    std::wstring ovT =
        g_s.overlay.enabled ? ComposeText(g_s.overlay, st) : L"";
    std::wstring stT =
        g_s.standing.enabled ? ComposeText(g_s.standing, st) : L"";
    std::wstring r2T = g_s.row2.enabled ? ComposeText(g_s.row2, st) : L"";
    std::wstring r3T = g_s.row3.enabled ? ComposeText(g_s.row3, st) : L"";

    std::wstring key = heroT + L"\x1f" + ovT + L"\x1f" + stT + L"\x1f" + r2T +
                       L"\x1f" + r3T + L"\x1f" +
                       std::to_wstring((int)(scale * 100));
    if (!force && key == wd.lastKey) return;
    wd.lastKey = key;

    TextBlock hero, overlay, row2, row3;
    Standing sb;
    MakeBlock(hero, heroT, S(g_s.hero.size), g_s.hero.weight,
              S(g_s.hero.spacing), g_s.tight);
    MakeBlock(overlay, ovT, S(g_s.overlay.size), g_s.overlay.weight,
              S(g_s.overlay.spacing), g_s.tight);
    MakeBlock(row2, r2T, S(g_s.row2.size), g_s.row2.weight,
              S(g_s.row2.spacing), false);
    MakeBlock(row3, r3T, S(g_s.row3.size), g_s.row3.weight,
              S(g_s.row3.spacing), false);

    float arc = 0;
    if (!stT.empty()) {
        const Layer& sl = g_s.standing;
        if (g_s.standVertical) {
            sb.vertical = true;
            arc = S(g_s.standCurve);
            sb.chars.resize(stT.size());
            float total = 0;
            for (size_t i = 0; i < stT.size(); ++i)
                MakeBlock(sb.chars[i], std::wstring(1, stT[i]), S(sl.size),
                          sl.weight, 0, true);
            for (auto& c : sb.chars) total += c.w;
            if (stT.size() > 1) total += S(sl.spacing) * (stT.size() - 1);
            sb.len = total;
            sb.thick = S(sl.size) * 0.72f;
            sb.extW = sb.thick + arc;
            sb.extH = total;
        } else {
            MakeBlock(sb.whole, stT, S(sl.size), sl.weight, S(sl.spacing),
                      true);
            sb.extW = sb.whole.w;
            sb.extH = sb.whole.h;
        }
    }

    const float M = S(24.0f);
    float mainW = 0, mainH = 0;
    bool overlayIsMain = false;
    if (hero.layout) {
        mainW = hero.w;
        mainH = hero.h;
    } else if (overlay.layout) {
        mainW = overlay.w;
        mainH = overlay.h;
        overlayIsMain = true;
    }

    bool hasStand = sb.Valid();
    float standW = hasStand ? sb.extW : 0;
    float standH = hasStand ? sb.extH : 0;
    float gapS = S(g_s.standing.gap);
    float leftExtra = (hasStand && g_s.standSide == 0) ? standW + gapS : 0;
    float rightExtra = (hasStand && g_s.standSide == 1) ? standW + gapS : 0;
    float blockW = leftExtra + mainW + rightExtra;
    float blockH = std::max(mainH, standH);

    float contentW = blockW;
    if (overlay.layout) contentW = std::max(contentW, overlay.w);
    if (row2.layout) contentW = std::max(contentW, row2.w);
    if (row3.layout) contentW = std::max(contentW, row3.w);

    float cy = M + blockH;
    float row2Top = 0, row3Top = 0;
    if (row2.layout) {
        if (blockH > 0) cy += S(g_s.row2.gap);
        row2Top = cy;
        cy += row2.h;
    }
    if (row3.layout) {
        if (cy > M) cy += S(g_s.row3.gap);
        row3Top = cy;
        cy += row3.h;
    }

    bool empty = contentW <= 0 && cy <= M;
    int W = empty ? 2 : (int)std::ceil(contentW + 2 * M);
    int H = empty ? 2 : (int)std::ceil(cy + M);

    bool ok = EnsureRT(wd) && EnsureSurface(wd, W, H);
    if (ok) {
        float blockX = M + (contentW - blockW) / 2;
        float mainCx = blockX + leftExtra + mainW / 2;
        float mainTop = M + (blockH - mainH) / 2;
        float standTop = M + (blockH - standH) / 2;

        RECT rc = {0, 0, W, H};
        wd.rt->BindDC(wd.memdc, &rc);
        wd.rt->BeginDraw();
        wd.rt->SetTransform(D2D1::Matrix3x2F::Identity());
        wd.rt->Clear(D2D1::ColorF(0, 0, 0, 0));

        if (hasStand) {
            float bx = g_s.standSide == 0
                           ? blockX
                           : blockX + leftExtra + mainW + gapS;
            if (sb.vertical) {
                DrawStandingVertical(wd.rt, g_s.standing, sb,
                                     g_s.standSide == 1, bx, standTop, scale,
                                     arc);
            } else {
                DrawBlock(wd.rt, g_s.standing, sb.whole, bx + standW / 2,
                          standTop, scale, false);
            }
        }
        if (hero.layout)
            DrawBlock(wd.rt, g_s.hero, hero, mainCx, mainTop, scale, true);
        if (overlay.layout) {
            float ox = mainCx + S(g_s.overlay.offX);
            float oy = mainTop + (mainH - overlay.h) / 2 + S(g_s.overlay.offY);
            if (overlayIsMain) oy = mainTop + S(g_s.overlay.offY);
            DrawBlock(wd.rt, g_s.overlay, overlay, ox, oy, scale, false);
        }
        if (row2.layout)
            DrawBlock(wd.rt, g_s.row2, row2, M + contentW / 2, row2Top, scale,
                      false);
        if (row3.layout)
            DrawBlock(wd.rt, g_s.row3, row3, M + contentW / 2, row3Top, scale,
                      false);

        HRESULT hr = wd.rt->EndDraw();
        if (hr == D2DERR_RECREATE_TARGET) {
            wd.rt->Release();
            wd.rt = nullptr;
            wd.lastKey.clear();
        }

        int x, y;
        if (!wd.placed) {
            int mw = wd.monRect.right - wd.monRect.left;
            int mh = wd.monRect.bottom - wd.monRect.top;
            int ox = Wh_GetIntValue(PosKey(wd.index, L'x').c_str(), kNoPos);
            int oy = Wh_GetIntValue(PosKey(wd.index, L'y').c_str(), kNoPos);
            x = ox == kNoPos ? wd.monRect.left + (mw - W) / 2
                             : wd.monRect.left + ox;
            y = oy == kNoPos ? wd.monRect.top + (mh - H) / 2
                             : wd.monRect.top + oy;
            wd.placed = true;
        } else {
            RECT r;
            GetWindowRect(wd.hwnd, &r);
            x = r.left;
            y = r.top;
        }

        HDC screen = GetDC(nullptr);
        POINT ptDst = {x, y};
        POINT ptSrc = {0, 0};
        SIZE sz = {W, H};
        BLENDFUNCTION bf = {AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
        UpdateLayeredWindow(wd.hwnd, screen, &ptDst, &sz, wd.memdc, &ptSrc, 0,
                            &bf, ULW_ALPHA);
        ReleaseDC(nullptr, screen);

        if (!IsWindowVisible(wd.hwnd)) {
            ShowWindow(wd.hwnd, SW_SHOWNOACTIVATE);
            SetWindowPos(wd.hwnd, HWND_BOTTOM, 0, 0, 0, 0,
                         SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
        }
    }

    hero.Release();
    overlay.Release();
    row2.Release();
    row3.Release();
    sb.Release();
}

void ArmTimer(HWND h) {
    SYSTEMTIME st;
    GetLocalTime(&st);
    UINT ms;
    if (g_s.needSeconds)
        ms = 1000 - st.wMilliseconds + 10;
    else
        ms = (UINT)((60 - st.wSecond) * 1000 - st.wMilliseconds + 20);
    SetTimer(h, TIMER_TICK, ms, nullptr);
}

LRESULT CALLBACK WidgetProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    if (msg == WM_NCCREATE) {
        auto* cs = reinterpret_cast<CREATESTRUCTW*>(lp);
        SetWindowLongPtrW(hwnd, GWLP_USERDATA, (LONG_PTR)cs->lpCreateParams);
        return DefWindowProcW(hwnd, msg, wp, lp);
    }
    Widget* wd = reinterpret_cast<Widget*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));

    switch (msg) {
        case WM_NCHITTEST:
            return g_s.lock ? HTTRANSPARENT : HTCAPTION;

        case WM_MOUSEACTIVATE:
            return MA_NOACTIVATE;

        case WM_WINDOWPOSCHANGING: {
            auto* p = reinterpret_cast<WINDOWPOS*>(lp);
            if (!(p->flags & SWP_NOZORDER)) p->hwndInsertAfter = HWND_BOTTOM;
            return 0;
        }

        case WM_TIMER:
            if (wp == TIMER_TICK && wd) {
                KillTimer(hwnd, TIMER_TICK);
                Render(*wd, false);
                ArmTimer(hwnd);
            }
            return 0;

        case WM_TIMECHANGE:
            if (wd) Render(*wd, true);
            return 0;

        case WM_DPICHANGED:
            if (wd) Render(*wd, true);
            return 0;

        case WM_DISPLAYCHANGE:
            if (g_ctrl) PostMessageW(g_ctrl, WM_APP_RELOAD, 0, 0);
            return 0;

        case WM_EXITSIZEMOVE:
            if (wd) {
                RECT r;
                GetWindowRect(hwnd, &r);
                int ox = r.left - wd->monRect.left;
                int oy = r.top - wd->monRect.top;
                SavePos(*wd, ox, oy);
                if (g_s.sync) {
                    for (auto& o : g_widgets) {
                        if (o.get() == wd || !o->hwnd) continue;
                        SetWindowPos(o->hwnd, HWND_BOTTOM,
                                     o->monRect.left + ox, o->monRect.top + oy,
                                     0, 0, SWP_NOSIZE | SWP_NOACTIVATE);
                        SavePos(*o, ox, oy);
                    }
                }
            }
            return 0;
    }
    return DefWindowProcW(hwnd, msg, wp, lp);
}

struct MonInfo {
    HMONITOR h;
    RECT r;
    bool primary;
};

BOOL CALLBACK EnumMonProc(HMONITOR h, HDC, LPRECT, LPARAM lp) {
    auto* v = reinterpret_cast<std::vector<MonInfo>*>(lp);
    MONITORINFO mi = {};
    mi.cbSize = sizeof(mi);
    if (GetMonitorInfoW(h, &mi))
        v->push_back({h, mi.rcMonitor, (mi.dwFlags & MONITORINFOF_PRIMARY) != 0});
    return TRUE;
}

void DestroyWidgets() {
    for (auto& w : g_widgets) {
        if (w->hwnd) {
            KillTimer(w->hwnd, TIMER_TICK);
            DestroyWindow(w->hwnd);
        }
        if (w->rt) w->rt->Release();
        if (w->memdc) DeleteDC(w->memdc);
        if (w->dib) DeleteObject(w->dib);
    }
    g_widgets.clear();
}

void Rebuild() {
    LoadSettings();
    DestroyWidgets();

    std::vector<MonInfo> mons;
    EnumDisplayMonitors(nullptr, nullptr, EnumMonProc,
                        reinterpret_cast<LPARAM>(&mons));

    HINSTANCE inst = GetModuleHandleW(nullptr);
    for (size_t i = 0; i < mons.size(); ++i) {
        bool want = false;
        if (g_s.monitorMode == 0) want = true;
        else if (g_s.monitorMode == 1) want = mons[i].primary;
        else want = (int)i + 1 == g_s.monitorNumber;
        if (!want) continue;

        auto w = std::make_unique<Widget>();
        w->mon = mons[i].h;
        w->monRect = mons[i].r;
        w->index = (int)i;
        w->hwnd = CreateWindowExW(
            WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE, kWidgetClass,
            L"CustomDexClockV6", WS_POPUP, mons[i].r.left, mons[i].r.top, 1, 1,
            nullptr, nullptr, inst, w.get());
        if (!w->hwnd) continue;
        Render(*w, true);
        ArmTimer(w->hwnd);
        g_widgets.push_back(std::move(w));
    }
}

LRESULT CALLBACK CtrlProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
        case WM_APP_RELOAD:
            Rebuild();
            return 0;
        case WM_APP_QUIT:
            DestroyWidgets();
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProcW(hwnd, msg, wp, lp);
}

DWORD WINAPI ThreadMain(LPVOID) {
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

    HINSTANCE inst = GetModuleHandleW(nullptr);
    bool ok =
        SUCCEEDED(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED,
                                    __uuidof(ID2D1Factory), nullptr,
                                    reinterpret_cast<void**>(&g_d2d))) &&
        SUCCEEDED(DWriteCreateFactory(
            DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory),
            reinterpret_cast<IUnknown**>(&g_dw)));

    WNDCLASSW wc = {};
    wc.lpfnWndProc = WidgetProc;
    wc.hInstance = inst;
    wc.lpszClassName = kWidgetClass;
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    RegisterClassW(&wc);

    WNDCLASSW cc = {};
    cc.lpfnWndProc = CtrlProc;
    cc.hInstance = inst;
    cc.lpszClassName = kCtrlClass;
    RegisterClassW(&cc);

    if (ok) {
        g_ctrl = CreateWindowExW(0, kCtrlClass, L"", 0, 0, 0, 0, 0,
                                 HWND_MESSAGE, nullptr, inst, nullptr);
    }
    SetEvent(g_ready);

    if (ok && g_ctrl) {
        Rebuild();
        MSG msg;
        while (GetMessageW(&msg, nullptr, 0, 0) > 0) {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
        DestroyWidgets();
        DestroyWindow(g_ctrl);
        g_ctrl = nullptr;
    }

    UnregisterClassW(kWidgetClass, inst);
    UnregisterClassW(kCtrlClass, inst);
    if (g_dw) {
        g_dw->Release();
        g_dw = nullptr;
    }
    if (g_d2d) {
        g_d2d->Release();
        g_d2d = nullptr;
    }
    return 0;
}

void StopThread() {
    if (!g_thread) return;
    if (g_ctrl) PostMessageW(g_ctrl, WM_APP_QUIT, 0, 0);
    WaitForSingleObject(g_thread, 5000);
    CloseHandle(g_thread);
    g_thread = nullptr;
    if (g_ready) {
        CloseHandle(g_ready);
        g_ready = nullptr;
    }
}

}  // namespace

BOOL Wh_ModInit() {
    g_ready = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_thread = CreateThread(nullptr, 0, ThreadMain, nullptr, 0, nullptr);
    if (!g_thread) return FALSE;
    WaitForSingleObject(g_ready, 5000);
    return TRUE;
}

void Wh_ModSettingsChanged() {
    if (g_ctrl) PostMessageW(g_ctrl, WM_APP_RELOAD, 0, 0);
}

void Wh_ModBeforeUninit() { StopThread(); }

void Wh_ModUninit() { StopThread(); }