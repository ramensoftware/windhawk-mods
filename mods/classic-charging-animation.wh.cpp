// ==WindhawkMod==
// @id              classic-charging-animation
// @name            Classic Charging Animation
// @description     Shows the battery level when you plug in, in the style of the Windows 11 volume and brightness indicators.
// @description:it-IT Mostra il livello della batteria al collegamento del caricatore, nello stile degli indicatori di volume e luminosità di Windows 11.
// @version         1.4.2
// @author          mamt104
// @github          https://github.com/mamt104
// @license         MIT
// @include         windhawk.exe
// @compilerOptions -lgdiplus -lgdi32 -luser32 -ladvapi32 -ldwmapi -lshell32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Classic Charging Animation

Plug in the charger to see the battery percentage in a small bar at the bottom center of the primary display. The bar takes its shape, position and motion from the Windows 11 volume and brightness indicators. It disappears after about two seconds.

![Preview of the charging bar at different battery levels](https://raw.githubusercontent.com/mamt104/windhawk-mods/69c605cce728f97219930ce121b3c3bef5401710/previews/classic-charging-animation-preview.gif)

Below 30% the bar starts amber. Below 15% it starts red, and below 5% an exclamation mark appears. At 95% and above a highlight crosses the bar. A check mark appears at 100%. The indicator follows the Windows light, dark, high contrast and animation settings. If Windows cannot report a percentage, the indicator shows a dash.

Choose **Automatic**, **Pulse** or **Segments** in the settings. With **Preview on save** enabled, saving the settings shows the bar without reconnecting the charger.

[Charging Animation](https://windhawk.net/mods/charging-animation) offers selectable effects and a battery statistics card for power changes. Here, the battery level at plug-in determines the warning or confirmation shown in a single system-style indicator.

### Italiano

Quando colleghi il caricatore, una barra in basso al centro dello schermo principale mostra la percentuale della batteria. Riprende l’aspetto degli indicatori di volume e luminosità di Windows 11 e scompare dopo circa due secondi.

Sotto il 30% la barra parte in ambra, sotto il 15% in rosso. Sotto il 5% compare un punto esclamativo. Dal 95% un riflesso attraversa la barra e al 100% appare un segno di spunta. Puoi scegliere **Automatico**, **Impulso** o **Segmenti** e vedere un’anteprima salvando le impostazioni.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- effect: fluent
  $name: Bar style
  $name:it-IT: Stile della barra
  $options:
  - fluent: Automatic
  - impulso: Pulse
  - scansione: Segments
  $options:it-IT:
  - fluent: Automatico
  - impulso: Impulso
  - scansione: Segmenti
- previewOnSave: true
  $name: Preview on save
  $name:it-IT: Anteprima al salvataggio
*/
// ==/WindhawkModSettings==

#include <windows.h>

#include <dwmapi.h>
#include <gdiplus.h>
#include <shellapi.h>
#include <algorithm>
#include <atomic>
#include <chrono>
#include <cmath>
#include <cwchar>

namespace {
using namespace Gdiplus;

constexpr wchar_t kWindowClass[] = L"Windhawk.ClassicChargingAnimation.1";
constexpr GUID kPowerSource = {
    0x5d3e9a59,
    0xe9d5,
    0x4b00,
    {0xa6, 0xbd, 0xff, 0x34, 0xff, 0x51, 0x65, 0x48}};
HANDLE gStop, gPreview, gThread;

enum class Style { Automatic, Pulse, Segments };
std::atomic<Style> gStyle{Style::Automatic};
using Clock = std::chrono::steady_clock;
constexpr float kWidth = 196, kHeight = 48;
constexpr int kCanvasWidth = 220, kCanvasHeight = 80;
constexpr float kBottomGap = 14.f;
constexpr float kShow = .167f, kFadeTime = .083f, kExitTime = .167f;
constexpr float kExitStart = 2.f, kFillTime = .250f, kSlide = 4.f;
constexpr float kColorFade = .26f;

float Unit(float x) {
    return std::clamp(x, 0.f, 1.f);
}
float Smooth(float x) {
    x = Unit(x);
    return x * x * (3.f - 2.f * x);
}
float FluentEntrance(float x) {
    // For cubic-bezier(0, 0, 0, 1), x = t^3 and y = smoothstep(t).
    return Smooth(std::cbrt(Unit(x)));
}
float Pulse(float t, float start, float length) {
    return std::sin(Unit((t - start) / length) * 3.14159265f);
}

enum class Warning { None, Amber, Red, Critical };

struct ChargeMotion {
    float sweepAt = -1.f;
    float warningHold = 0.f;
    Warning warning = Warning::None;
    bool full = false;
};

ChargeMotion MotionFor(BYTE level) {
    if (level < 5)
        return {.warningHold = .84f, .warning = Warning::Critical};
    if (level < 15)
        return {.warningHold = .62f, .warning = Warning::Red};
    if (level < 30)
        return {.warningHold = .58f, .warning = Warning::Amber};
    if (level >= 95 && level <= 100)
        return {.sweepAt = .50f, .full = level == 100};
    return {};
}

bool HasBattery(const SYSTEM_POWER_STATUS& status) {
    if (status.BatteryFlag == 255)
        return status.BatteryLifePercent <= 100;
    return (status.BatteryFlag & 128) == 0;
}

struct Palette {
    unsigned surface, border, fg, track, accent, highlight;
    float opacity;
    unsigned red, amber;
};

Palette WindowsPalette() {
    HIGHCONTRASTW contrast{sizeof(contrast)};
    if (SystemParametersInfoW(SPI_GETHIGHCONTRAST, sizeof(contrast), &contrast,
                              0) &&
        (contrast.dwFlags & HCF_HIGHCONTRASTON)) {
        const auto rgb = [](int color) {
            const COLORREF c = GetSysColor(color);
            return static_cast<unsigned>(GetRValue(c) << 16 |
                                         GetGValue(c) << 8 | GetBValue(c));
        };
        const unsigned text = rgb(COLOR_WINDOWTEXT);
        return {rgb(COLOR_WINDOW),
                text,
                text,
                rgb(COLOR_GRAYTEXT),
                rgb(COLOR_HIGHLIGHT),
                rgb(COLOR_HIGHLIGHTTEXT),
                1.f,
                text,
                text};
    }
    constexpr wchar_t key[] =
        L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize";
    DWORD light = 1, transparency = 1, bytes = sizeof(DWORD);
    if (RegGetValueW(HKEY_CURRENT_USER, key, L"SystemUsesLightTheme",
                     RRF_RT_REG_DWORD, nullptr, &light,
                     &bytes) != ERROR_SUCCESS)
        light = 1;
    bytes = sizeof(DWORD);
    if (RegGetValueW(HKEY_CURRENT_USER, key, L"EnableTransparency",
                     RRF_RT_REG_DWORD, nullptr, &transparency,
                     &bytes) != ERROR_SUCCESS)
        transparency = 1;
    if (light)
        return {0xf8f8fa,
                0xd9d9db,
                0x202023,
                0xa8a8ad,
                0x167b42,
                0x81e9a7,
                transparency ? .985f : 1.f,
                0xb62b36,
                0x986300};
    return {0x2b2b2c,
            0x222224,
            0xf4f4f5,
            0xa0a0a2,
            0x48db80,
            0xc4ffda,
            transparency ? .985f : 1.f,
            0xff686c,
            0xffcb56};
}

Color Shade(unsigned rgb, float alpha) {
    return Color((static_cast<unsigned>(Unit(alpha) * 255.f) << 24) | rgb);
}

unsigned MixRgb(unsigned from, unsigned to, float fraction) {
    unsigned result = 0;
    for (const unsigned shift : {0u, 8u, 16u}) {
        const auto channel = [shift](unsigned rgb) {
            return static_cast<float>((rgb >> shift) & 255u);
        };
        const auto value = static_cast<unsigned>(
            std::lerp(channel(from), channel(to), fraction) + .5f);
        result |= value << shift;
    }
    return result;
}

void RoundPath(GraphicsPath& path, RectF r, float radius) {
    const float d = std::min({r.Width, r.Height, radius * 2.f});
    path.AddArc(r.X, r.Y, d, d, 180, 90);
    path.AddArc(r.GetRight() - d, r.Y, d, d, 270, 90);
    path.AddArc(r.GetRight() - d, r.GetBottom() - d, d, d, 0, 90);
    path.AddArc(r.X, r.GetBottom() - d, d, d, 90, 90);
    path.CloseFigure();
}

void Rounded(Graphics& g,
             RectF box,
             float radius,
             unsigned rgb,
             float opacity) {
    if (box.Width < .2f || opacity <= 0)
        return;
    GraphicsPath path;
    RoundPath(path, box, radius);
    SolidBrush brush(Shade(rgb, opacity));
    g.FillPath(&brush, &path);
}

void BatteryIcon(Graphics& g,
                 const Palette& p,
                 float alpha,
                 bool colored,
                 bool full,
                 float exclamation,
                 float flash) {
    GraphicsPath body;
    RoundPath(body, {13.5f, 18.5f, 15, 11}, 2.1f);
    const unsigned outline = colored || full ? p.accent : p.fg;
    Pen stroke(Shade(outline, alpha), 1.25f);
    g.DrawPath(&stroke, &body);
    Rounded(g, {29.5f, 22, 1.6f, 4}, .8f, outline, alpha);

    if (full) {
        Pen check(Shade(p.accent, alpha), 1.6f);
        check.SetStartCap(LineCapRound);
        check.SetEndCap(LineCapRound);
        check.SetLineJoin(LineJoinRound);
        const PointF path[] = {{18, 24}, {20.3f, 26.1f}, {24.7f, 21.5f}};
        g.DrawLines(&check, path, 3);
    } else if (exclamation < 1.f) {
        const PointF bolt[] = {{22, 19.8f}, {18.4f, 24.5f}, {20.6f, 24.5f},
                               {20, 28.3f}, {24.1f, 23.1f}, {21.85f, 23.1f}};
        SolidBrush brush(Shade(p.accent, alpha * (1.f - exclamation)));
        g.FillPolygon(&brush, bolt, 6);
    }
    if (exclamation > 0.f) {
        const float symbolAlpha = alpha * exclamation * (.48f + .52f * flash);
        Rounded(g, {20.2f, 19.8f, 1.6f, 5.2f}, .8f, p.accent, symbolAlpha);
        Rounded(g, {20.2f, 26.3f, 1.6f, 1.6f}, .8f, p.accent, symbolAlpha);
    }
}

void ChargeBar(Graphics& g,
               const Palette& p,
               float alpha,
               float fill,
               float signal,
               float sweep,
               Style style) {
    const RectF track{42.5f, 22, 106, 4};
    Rounded(g, track, 2, p.track, alpha);
    const float width = track.Width * fill;

    if (style == Style::Segments) {
        const float cell = track.Width / 10;
        for (int i = 0; i < 10; ++i) {
            const float left = cell * i;
            Rounded(g,
                    {track.X + left, track.Y,
                     std::min(cell - 1.6f, width - left), track.Height},
                    1.3f, p.accent, alpha);
        }
    } else {
        Rounded(g, {track.X, track.Y, width, track.Height}, 2, p.accent, alpha);
    }

    if (signal > 0) {
        // At 0%, highlight the track itself and leave the amount of fill at
        // zero.
        const float pulseWidth = std::clamp(width, 8.f, 42.f);
        Rounded(g, {track.X, track.Y - 2, pulseWidth, 8}, 4, p.accent,
                .32f * signal * alpha);
        if (width >= .2f)
            Rounded(
                g,
                {track.X, track.Y, std::min(width, pulseWidth), track.Height},
                2, p.highlight, .72f * signal * alpha);
    }
    if (sweep >= 0 && sweep <= 1 && width > 1) {
        const float x = track.X + width * sweep;
        const float start = std::max(track.X, x - 12);
        const float end = std::min(track.X + width, x + 12);
        Rounded(g, {start, track.Y, end - start, track.Height}, 2, p.highlight,
                .56f * Pulse(sweep, 0, 1) * alpha);
    }
}

void DrawGlow(Graphics& g,
              float t,
              BYTE level,
              bool motion,
              Style style,
              const Palette& p) {
    const ChargeMotion m = MotionFor(level);
    const float exit = t - kExitStart;
    const float fadeOut =
        1.f - Unit((exit - kExitTime + kFadeTime) / kFadeTime);
    const float alpha = motion ? Unit(t / kFadeTime) * fadeOut : 1.f;
    if (alpha <= 0)
        return;
    const float slide = motion
                            ? kSlide * (2.f - FluentEntrance(t / kShow) -
                                        FluentEntrance(1.f - exit / kExitTime))
                            : 0.f;
    g.TranslateTransform(0.f, slide);

    const RectF box{0, 0, kWidth, kHeight};
    Rounded(g, {-3, 3, kWidth + 6, kHeight + 2}, 10, 0x000000, .09f * alpha);
    Rounded(g, {-1, 2, kWidth + 2, kHeight + 1}, 9, 0x000000, .13f * alpha);
    GraphicsPath body;
    RoundPath(body, box, 8.f);
    SolidBrush surface(Shade(p.surface, p.opacity * alpha));
    Pen border(Shade(p.border, alpha), 1);
    g.FillPath(&surface, &body);
    g.DrawPath(&border, &body);

    const float rise = motion ? FluentEntrance((t - .025f) / kFillTime) : 1.f;
    const bool warned = m.warning != Warning::None;
    const bool critical = m.warning == Warning::Critical;
    float warningStrength = warned ? 1.f : 0.f;
    if (warned && motion)
        warningStrength -= Smooth((t - m.warningHold) / kColorFade);
    Palette colors = p;
    if (warned) {
        const unsigned initial = m.warning == Warning::Amber ? p.amber : p.red;
        colors.accent = MixRgb(p.accent, initial, warningStrength);
        colors.highlight =
            MixRgb(p.highlight, MixRgb(initial, p.fg, .38f), warningStrength);
    }
    const float flash = critical && motion ? std::max(Pulse(t, .10f, .33f),
                                                      Pulse(t, .47f, .33f))
                                           : (critical ? 1.f : 0.f);
    float signal = 0;
    float sweep = -1;
    if (motion && level <= 100) {
        if (critical)
            signal = flash * warningStrength;
        if (style == Style::Pulse && !warned)
            signal = .68f * Pulse(t, .21f, .40f);
        if (m.sweepAt >= 0)
            sweep = (t - m.sweepAt) / .42f;
    }
    BatteryIcon(g, colors, alpha, warned || signal > .08f,
                m.full && (!motion || t >= .55f),
                critical ? warningStrength : 0.f, flash);
    ChargeBar(g, colors, alpha, level <= 100 ? level / 100.f * rise : 0, signal,
              sweep, style);

    wchar_t amount[8] = L"\u2014";
    if (level <= 100)
        std::swprintf(amount, 8, L"%u%%", static_cast<unsigned>(level));
    Font font(L"Segoe UI", level == 100 ? 13.f : 13.5f, FontStyleRegular,
              UnitPixel);
    StringFormat format(StringFormat::GenericTypographic());
    format.SetFormatFlags(format.GetFormatFlags() | StringFormatFlagsNoWrap);
    format.SetAlignment(StringAlignmentFar);
    format.SetLineAlignment(StringAlignmentCenter);
    SolidBrush ink(Shade(p.fg, alpha));
    g.DrawString(amount, -1, &font, RectF(151, 0, 34, kHeight), &format, &ink);
}

class FrameSync {
    using WaitForClock = DWORD(WINAPI*)(UINT, const HANDLE*, DWORD);
    HMODULE dcomp =
        LoadLibraryExW(L"dcomp.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    WaitForClock wait =
        dcomp ? reinterpret_cast<WaitForClock>(
                    GetProcAddress(dcomp, "DCompositionWaitForCompositorClock"))
              : nullptr;

   public:
    ~FrameSync() {
        if (dcomp)
            FreeLibrary(dcomp);
    }

    DWORD Wait() const {
        if (wait) {
            // A sleeping display can report occlusion immediately.
            return wait(0, nullptr, 65) == 0 ? 0 : 50;
        }

        DWM_TIMING_INFO timing{sizeof(timing)};
        DWORD fallback = 17;
        if (SUCCEEDED(DwmGetCompositionTimingInfo(nullptr, &timing)) &&
            timing.rateRefresh.uiNumerator) {
            fallback = std::clamp<DWORD>(
                static_cast<DWORD>(
                    std::ceil(1000.0 * timing.rateRefresh.uiDenominator /
                              timing.rateRefresh.uiNumerator)),
                1, 50);
        }
        const auto begin = Clock::now();
        if (SUCCEEDED(DwmFlush()) &&
            Clock::now() - begin >= std::chrono::microseconds(500))
            return 0;
        return fallback;
    }
};

struct Overlay {
    HWND hwnd{};
    HDC dc{};
    HBITMAP dib{};
    HGDIOBJ oldBitmap{};
    BYTE* pixels{};
    SIZE size{};
    POINT position{};
    Clock::time_point started{};
    bool motion{}, preview{};
    BYTE battery{255}, previousSource{255};
    HPOWERNOTIFY notification{};
    Style style{};
    Palette palette{};

    void Hide() {
        ShowWindow(hwnd, SW_HIDE);
        if (oldBitmap)
            SelectObject(dc, oldBitmap);
        if (dib)
            DeleteObject(dib);
        if (dc)
            DeleteDC(dc);
        oldBitmap = nullptr;
        dib = nullptr;
        dc = nullptr;
        pixels = nullptr;
    }

    void Start(BYTE level, bool isPreview = false) {
        Hide();
        MONITORINFO monitor{sizeof(monitor)};
        if (!GetMonitorInfoW(
                MonitorFromPoint(POINT{}, MONITOR_DEFAULTTOPRIMARY), &monitor))
            return;

        const RECT& bounds = monitor.rcWork;
        const UINT dpi = GetDpiForWindow(hwnd);
        const int scaleDpi = dpi ? static_cast<int>(dpi) : 96;
        size = {
            std::min(bounds.right - bounds.left,
                     static_cast<LONG>(MulDiv(kCanvasWidth, scaleDpi, 96))),
            std::min(bounds.bottom - bounds.top,
                     static_cast<LONG>(MulDiv(kCanvasHeight, scaleDpi, 96)))};
        if (size.cx <= 0 || size.cy <= 0)
            return;
        position = {bounds.left + (bounds.right - bounds.left - size.cx) / 2,
                    bounds.bottom - size.cy};

        BITMAPINFO info{};
        info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        info.bmiHeader.biWidth = size.cx;
        info.bmiHeader.biHeight = -size.cy;
        info.bmiHeader.biPlanes = 1;
        info.bmiHeader.biBitCount = 32;
        info.bmiHeader.biCompression = BI_RGB;
        dc = CreateCompatibleDC(nullptr);
        void* bits = nullptr;
        if (dc)
            dib =
                CreateDIBSection(dc, &info, DIB_RGB_COLORS, &bits, nullptr, 0);
        if (dib)
            oldBitmap = SelectObject(dc, dib);
        if (!oldBitmap || oldBitmap == HGDI_ERROR) {
            oldBitmap = nullptr;
            Hide();
            return;
        }
        pixels = static_cast<BYTE*>(bits);

        battery = level;
        preview = isPreview;
        style = gStyle.load();
        ReadAppearance();
        started = Clock::now();
    }

    void ReadAppearance() {
        palette = WindowsPalette();
        BOOL animation = TRUE;
        SystemParametersInfoW(SPI_GETCLIENTAREAANIMATION, 0, &animation, 0);
        motion = animation != FALSE;
    }

    DWORD Frame() {
        if (!pixels)
            return INFINITE;
        const float elapsed =
            std::chrono::duration<float>(Clock::now() - started).count();
        const ChargeMotion m = MotionFor(battery);
        if (elapsed >= kExitStart + kExitTime) {
            Hide();
            return INFINITE;
        }
        {
            Bitmap surface(size.cx, size.cy, size.cx * 4, PixelFormat32bppPARGB,
                           pixels);
            Graphics graphics(&surface);
            graphics.Clear(Color(0, 0, 0, 0));
            graphics.SetPageUnit(UnitPixel);
            graphics.SetSmoothingMode(SmoothingModeAntiAlias);
            graphics.SetTextRenderingHint(TextRenderingHintAntiAliasGridFit);
            const float scale =
                std::min(size.cx / static_cast<float>(kCanvasWidth),
                         size.cy / static_cast<float>(kCanvasHeight));
            graphics.TranslateTransform(
                (size.cx - kWidth * scale) / 2.f,
                size.cy - (kHeight + kBottomGap) * scale);
            graphics.ScaleTransform(scale, scale);
            DrawGlow(graphics, elapsed, battery, motion, style, palette);
            graphics.Flush(FlushIntentionSync);
        }
        POINT at{};
        BLENDFUNCTION blend{AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
        if (!UpdateLayeredWindow(hwnd, nullptr, &position, &size, dc, &at, 0,
                                 &blend, ULW_ALPHA)) {
            Hide();
            return INFINITE;
        }
        if (!IsWindowVisible(hwnd))
            ShowWindow(hwnd, SW_SHOWNOACTIVATE);

        if (!motion)
            return static_cast<DWORD>(
                std::ceil((kExitStart + kExitTime - elapsed) * 1000.f));

        float settled =
            m.warningHold > 0.f ? m.warningHold + kColorFade : kFadeTime;
        settled = battery <= 100 ? std::max({settled, kShow, .025f + kFillTime,
                                             m.sweepAt + .42f})
                                 : kShow;
        if (style == Style::Pulse && m.warning == Warning::None &&
            battery <= 100)
            settled = std::max(settled, .61f);
        if (elapsed >= settled && elapsed < kExitStart)
            return static_cast<DWORD>(
                std::ceil((kExitStart - elapsed) * 1000.f));
        return 0;
    }

    void Preview() {
        SYSTEM_POWER_STATUS status{};
        const bool hasLevel = GetSystemPowerStatus(&status) &&
                              HasBattery(status) &&
                              status.BatteryLifePercent <= 100;
        Start(hasLevel ? status.BatteryLifePercent : 255, true);
    }

    void PowerChanged(BYTE source) {
        if (source > 1)
            return;
        const bool pluggedIn = previousSource == 0 && source == 1;
        if (previousSource == 1 && source == 0 && pixels && !preview)
            Hide();
        previousSource = source;
        if (!pluggedIn)
            return;
        SYSTEM_POWER_STATUS status{};
        if (GetSystemPowerStatus(&status) && HasBattery(status))
            Start(status.BatteryLifePercent);
    }
};

LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    if (msg == WM_NCCREATE) {
        auto* overlay = static_cast<Overlay*>(
            reinterpret_cast<CREATESTRUCTW*>(lp)->lpCreateParams);
        overlay->hwnd = hwnd;
        SetWindowLongPtrW(hwnd, GWLP_USERDATA,
                          reinterpret_cast<LONG_PTR>(overlay));
    }
    auto* overlay =
        reinterpret_cast<Overlay*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
    if (!overlay)
        return DefWindowProcW(hwnd, msg, wp, lp);

    if (msg == WM_POWERBROADCAST) {
        if (wp == PBT_POWERSETTINGCHANGE && lp) {
            auto* setting = reinterpret_cast<const POWERBROADCAST_SETTING*>(lp);
            if (IsEqualGUID(setting->PowerSetting, kPowerSource) &&
                setting->DataLength == sizeof(DWORD)) {
                if (setting->Data[0] <= 2)
                    overlay->PowerChanged(setting->Data[0] == 0 ? 1 : 0);
            }
        } else if (wp == PBT_APMPOWERSTATUSCHANGE && !overlay->notification) {
            SYSTEM_POWER_STATUS status{};
            if (GetSystemPowerStatus(&status))
                overlay->PowerChanged(status.ACLineStatus);
        }
        return TRUE;
    }
    if (msg == WM_DISPLAYCHANGE)
        overlay->Hide();
    if ((msg == WM_SETTINGCHANGE || msg == WM_THEMECHANGED ||
         msg == WM_SYSCOLORCHANGE) &&
        overlay->pixels)
        overlay->ReadAppearance();
    return DefWindowProcW(hwnd, msg, wp, lp);
}

DWORD WINAPI RunOverlay(void*) {
    SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    GdiplusStartupInput startup;
    ULONG_PTR token{};
    if (GdiplusStartup(&token, &startup, nullptr) != Ok)
        return 0;

    HINSTANCE module{};
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                           GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                       kWindowClass, &module);
    WNDCLASSW windowClass{};
    windowClass.lpfnWndProc = WindowProc;
    windowClass.hInstance = module;
    windowClass.lpszClassName = kWindowClass;
    if (RegisterClassW(&windowClass)) {
        Overlay overlay;
        SYSTEM_POWER_STATUS status{};
        if (GetSystemPowerStatus(&status))
            overlay.previousSource = status.ACLineStatus;
        HWND hwnd =
            CreateWindowExW(WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_TRANSPARENT |
                                WS_EX_NOACTIVATE | WS_EX_TOOLWINDOW,
                            kWindowClass, L"", WS_POPUP, 0, 0, 1, 1, nullptr,
                            nullptr, module, &overlay);
        if (hwnd) {
            overlay.notification = RegisterPowerSettingNotification(
                hwnd, &kPowerSource, DEVICE_NOTIFY_WINDOW_HANDLE);
            const HANDLE events[] = {gStop, gPreview};
            FrameSync frameSync;
            while (WaitForSingleObject(gStop, 0) == WAIT_TIMEOUT) {
                MSG msg;
                while (WaitForSingleObject(gStop, 0) == WAIT_TIMEOUT &&
                       PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
                    DispatchMessageW(&msg);
                }
                if (WaitForSingleObject(gStop, 0) != WAIT_TIMEOUT)
                    break;
                DWORD delay = overlay.Frame();
                if (delay == 0)
                    delay = frameSync.Wait();
                // A successful wait consumes the auto-reset preview event.
                const DWORD wake = MsgWaitForMultipleObjectsEx(
                    ARRAYSIZE(events), events, delay, QS_ALLINPUT,
                    MWMO_INPUTAVAILABLE);
                if (wake == WAIT_OBJECT_0 || wake == WAIT_FAILED)
                    break;
                if (wake == WAIT_OBJECT_0 + 1)
                    overlay.Preview();
            }
            if (overlay.notification)
                UnregisterPowerSettingNotification(overlay.notification);
            overlay.Hide();
            DestroyWindow(hwnd);
        }
        UnregisterClassW(kWindowClass, module);
    }
    GdiplusShutdown(token);
    return 0;
}

void ReadSettings() {
    const wchar_t* value = Wh_GetStringSetting(L"effect");
    Style style = Style::Automatic;
    if (value) {
        if (std::wcscmp(value, L"impulso") == 0)
            style = Style::Pulse;
        if (std::wcscmp(value, L"scansione") == 0)
            style = Style::Segments;
        Wh_FreeStringSetting(value);
    }
    gStyle.store(style);
}
}  // namespace

BOOL WhTool_ModInit() {
    ReadSettings();
    gStop = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!gStop)
        return FALSE;
    gPreview = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    if (gPreview) {
        gThread = CreateThread(nullptr, 0, RunOverlay, nullptr, 0, nullptr);
        if (gThread)
            return TRUE;
        CloseHandle(gPreview);
    }
    CloseHandle(gStop);
    return FALSE;
}

void WhTool_ModSettingsChanged() {
    ReadSettings();
    if (Wh_GetIntSetting(L"previewOnSave"))
        SetEvent(gPreview);
}

void WhTool_ModUninit() {
    SetEvent(gStop);
    WaitForSingleObject(gThread, INFINITE);
    CloseHandle(gThread);
    CloseHandle(gPreview);
    CloseHandle(gStop);
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
