// ==WindhawkMod==
// @id              extra-dim
// @name            Extra Dim
// @description     Extra dim below the minimum brightness, plus a warm evening tint, in a fluent flyout with a tray icon and global hotkeys.
// @version         1.0.0
// @author          DpakB
// @homepage        https://dpakb.com
// @github          https://github.com/DeepakBalaraman
// @twitter         https://twitter.com/dpakb_
// @donateUrl       https://buymeacoffee.com/dpakb
// @license         MIT
// @include         windhawk.exe
// @compilerOptions -luser32 -lgdi32 -lshell32 -ladvapi32 -lole32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*

# Extra Dim

Dims the screen further than the brightness slider goes, and adds a warm tint
for the evening. A flyout, a tray icon, five hotkeys.

![Extra Dim Mod Sample Screenshot](https://raw.githubusercontent.com/DeepakBalaraman/extra-dim/master/assets/screenshot.png)

Open it with **Ctrl+Alt+Shift+D**, or by clicking the tray icon. The same controls
are in the icon's right-click menu.

## Dimming

- **Extra dim** goes darker than the bottom of the brightness slider. The slider
  stops at the ceiling you set, so the screen can't be dimmed to black by
  accident. Switching the toggle off keeps the level for next time.
- **Overlay** (default) puts a click-through black layer over each display. Works
  with any GPU and driver, doesn't touch HDR, and costs nothing while it's off.
  It can't reach exclusive-fullscreen games, because those skip the desktop.
- **Gamma** lowers the display's own gamma ramp instead, which does reach
  exclusive fullscreen. It depends on the driver: some games and most resolution
  changes reset it, and if the driver refuses, the mod falls back to the overlay
  by itself and says so under the Displays card.

## Warm tint

A second layer, with its own color and strength, on top of dimming or on its
own. It needs no driver support. The panel keeps showing the strength while the
tint is off, so the number you see is the level the switch brings back.

**Ctrl+Alt+T** switches it from anywhere.

## Good to know

- Every hotkey can be changed or cleared.
- By default the dim layer covers the panel too, so the panel shows what the
  screen looks like. Turn on **Keep the panel above the dim layer** to read it at
  full brightness; the controls work either way, because the layer lets clicks
  through.
- Under the overlay the pointer stays bright, because Windows draws it above
  every window. **Dim the mouse pointer too**, under Settings > Dim, swaps the
  system cursors for darkened copies while dimming is on and puts them back
  afterwards. Gamma dims the pointer already. A run that ends without its
  shutdown leaves the swap behind, and the next start undoes it before anything
  else.
- The layer sits with ordinary topmost windows, so what Windows raises above them
  is not covered: the Alt+Tab switcher, shell flyouts that go topmost. Gamma does
  reach those, because it dims the scanout rather than the desktop.
- Gamma is the lighter of the two at the same percentage. Drivers limit how far
  the ramp may go, so the two methods look their most alike at low levels and
  drift apart as the level rises.
- Windows never dims the secure desktop, so the UAC prompt and the sign-in screen
  stay bright. That's the platform, not the mod.
- Levels, toggles, the method and the display choice are remembered unless you
  switch that off. The method and display chosen in the panel are kept beside the
  setting they replaced, so changing that setting in Windhawk wins them back.
- Everything is stored in the mod's own Windhawk storage, one set per user
  account. The mod doesn't read or write Windows' Night light setting, and sends
  nothing anywhere.

## Settings

- **Startup** - start dimmed, the level to start at, remember changes.
- **Appearance** - theme, accent color, panel opacity, panel scale.
- **Placement** - corner, monitor, fine offsets, close on focus loss, tray icon.
- **Dim** - step, maximum, method, all displays, exclude from captures, keep the
  panel bright, smooth changes, dim the pointer.
- **Tint** - color and starting strength.
- **Shortcuts** - one chord per action, written as on a keyboard: `ctrl+alt+d`.
  A chord needs Ctrl, Alt, Shift or Win; a lone key is ignored unless it is an
  F-key, so a stray letter can't be taken from every other program.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- Startup:
  - StartDimmed: false
    $name: Start with extra dim on
    $description: "Apply the extra dim level below as soon as the mod loads."
  - StartLevel: 40
    $name: Extra dim level on start (%)
    $description: "Dimming applied at startup, and the level the slider starts from."
    #! $min: 5
    #! $max: 100
  - RememberValues: true
    $name: Remember changes
    $description: "Keep the levels and toggles you set across restarts and reboots. When on, remembered values win over the startup level."
- Appearance:
  - Theme: auto
    $name: Theme
    $description: "Choose whether the panel matches Windows or uses a fixed theme."
    $options:
      - auto: Follow Windows
      - light: Light
      - dark: Dark
  - AccentColor: accent
    $name: Accent color
    $description: "Color used for the toggles, the sliders and the icons."
    $options:
      - accent: Windows Accent Color
      - custom: Custom (Pick A Color Below)
      - blue: Blue
      - green: Green
      - teal: Teal
      - purple: Purple
      - pink: Pink
      - orange: Orange
      - red: Red
      - gold: Gold
      - mint: Mint
  - CustomAccentColor: "#4285F4"
    $name: Custom accent color
    $description: "Custom color value when Accent color is set to Custom (for example #4285F4)."
    #! $format: colorRgb
    #! $showIf: {AccentColor: custom}
  - Opacity: 96
    $name: Panel opacity (%)
    $description: "Transparency of the panel background. Text and controls stay fully opaque."
    #! $min: 70
    #! $max: 100
  - PanelScale: 100
    $name: Panel scale (%)
    $description: "Overall size of the panel, on top of the monitor's own scaling."
    #! $min: 80
    #! $max: 140
- Placement:
  - Position: right
    $name: Screen position
    $description: "Where the panel pops up on the screen."
    $options:
      - topleft: Top Left
      - top: Top
      - topright: Top Right
      - left: Left
      - center: Center
      - right: Right
      - bottomleft: Bottom Left
      - bottom: Bottom
      - bottomright: Bottom Right
  - Monitor: pointer
    $name: Target monitor
    $description: "Open on the monitor under the mouse cursor or on the primary display."
    $options:
      - pointer: Monitor With The Pointer
      - primary: Primary Monitor
  - HorizontalOffset: 0
    $name: Horizontal offset (pixels)
    $description: "Fine-tune the horizontal position."
    #! $min: -2000
    #! $max: 2000
  - VerticalOffset: 0
    $name: Vertical offset (pixels)
    $description: "Fine-tune the vertical position."
    #! $min: -2000
    #! $max: 2000
  - CloseOnFocusLoss: true
    $name: Close when the panel loses focus
    $description: "Close the panel when you click somewhere else, the way a shell flyout behaves."
  - ShowTrayIcon: true
    $name: Show a tray icon
    $description: "Keep an icon in the notification area while the mod is loaded: a click opens or closes the panel, a right click has the controls."
- Dim:
  - Step: 10
    $name: Step (%)
    $description: "How much one wheel notch, arrow key or hotkey press changes a level."
    #! $min: 1
    #! $max: 25
  - Maximum: 90
    $name: Maximum dim level (%)
    $description: "Ceiling for the slider. At 100% the screen is nearly black, which makes it hard to find the controls again."
    #! $min: 30
    #! $max: 100
  - Method: overlay
    $name: Dimming method
    $description: "Overlay draws a click-through layer over each display. Gamma lowers the display gamma ramp, which also reaches exclusive fullscreen."
    $options:
      - overlay: Overlay (Recommended)
      - gamma: Gamma Ramp
  - AllDisplays: true
    $name: Dim every display
    $description: "Dim all monitors, or only the primary one."
  - ExcludeFromCapture: true
    $name: Leave the layer out of screen captures
    $description: "Keep screenshots, recordings and screen sharing at their normal brightness. Overlay method only."
  - KeepPanelBright: false
    $name: Keep the panel above the dim layer
    $description: "When off, the dim layer covers the panel as well, so the panel shows exactly how dark the screen is. Turn on to keep the panel at full brightness."
  - SmoothTransitions: true
    $name: Smooth level changes
    $description: "Glide between levels instead of jumping."
  - DimPointer: false
    $name: Dim the mouse pointer too
    $description: "Darken the pointer along with the screen. Overlay method only, since Gamma scales the pointer already. The cursors are put back when dimming stops or the mod unloads."
- Tint:
  - Color: "#FFB56B"
    $name: Warm tint color
    $description: "Color of the warm layer, for example #FFB56B for a soft amber."
    #! $format: colorRgb
  - DefaultStrength: 0
    $name: Starting tint strength (%)
    $description: "Strength the Warm tint toggle restores. Zero leaves it off at startup."
    #! $min: 0
    #! $max: 60
- Shortcuts:
  - PanelHotkey: ctrl+alt+shift+d
    $name: Open the panel
    $description: "Chord that opens and closes the panel, for example ctrl+alt+shift+d. Leave empty to disable. A lone key is ignored unless it is an F-key, so a chord always needs Ctrl, Alt, Shift or Win."
  - ToggleDimHotkey: ctrl+alt+d
    $name: Toggle extra dim
    $description: "Chord that switches extra dim on and off, for example ctrl+alt+d. Leave empty to disable."
  - ToggleTintHotkey: ctrl+alt+t
    $name: Toggle warm tint
    $description: "Chord that switches the warm tint on and off, for example ctrl+alt+t. Leave empty to disable."
  - DimUpHotkey: ctrl+alt+shift+up
    $name: Dim more
    $description: "Chord that raises the dim level by the step, for example ctrl+alt+shift+up. Leave empty to disable."
  - DimDownHotkey: ctrl+alt+shift+down
    $name: Dim less
    $description: "Chord that lowers the dim level by the step, for example ctrl+alt+shift+down. Leave empty to disable."
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <windowsx.h>
#include <sddl.h>
#include <shellapi.h>

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <cwchar>
#include <cwctype>
#include <string>
#include <vector>

////////////////////////////////////////////////////////////////////////////////
// Small helpers

inline float Clamp01(float value) {
    return value < 0.0f ? 0.0f : (value > 1.0f ? 1.0f : value);
}

inline int ClampInt(int value, int low, int high) {
    return value < low ? low : (value > high ? high : value);
}

inline double NowMs() {
    static const ULONGLONG start = GetTickCount64();
    return static_cast<double>(GetTickCount64() - start);
}

inline double EaseOutCubic(double t) {
    t = std::clamp(t, 0.0, 1.0);
    const double u = 1.0 - t;
    return 1.0 - u * u * u;
}

template <typename T>
T GetProc(PCWSTR module, const char* name) {
    HMODULE handle = GetModuleHandleW(module);
    if (!handle) {
        handle = LoadLibraryExW(module, nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    }
    if (!handle) {
        return nullptr;
    }
    // Through void* because the prototype the caller wants cannot be expressed
    // by FARPROC, which is what GetProcAddress hands back.
    return reinterpret_cast<T>(
        reinterpret_cast<void*>(GetProcAddress(handle, name)));
}

std::wstring Trim(const std::wstring& text) {
    size_t begin = 0;
    size_t end = text.size();
    while (begin < end && iswspace(text[begin])) {
        begin++;
    }
    while (end > begin && iswspace(text[end - 1])) {
        end--;
    }
    return text.substr(begin, end - begin);
}

std::wstring Lower(std::wstring text) {
    for (wchar_t& c : text) {
        c = static_cast<wchar_t>(towlower(c));
    }
    return text;
}

// Integers are formatted by the standard library, which is locale-independent
// for the numeric conversion.
std::wstring Number(int value) {
    return std::to_wstring(value);
}

std::wstring Percent(int value) {
    return Number(value) + L"%";
}

bool SystemUsesDarkTheme() {
    DWORD value = 1;
    DWORD size = sizeof(value);
    if (RegGetValueW(HKEY_CURRENT_USER,
                     L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
                     L"AppsUseLightTheme", RRF_RT_REG_DWORD, nullptr, &value,
                     &size) != ERROR_SUCCESS) {
        return true;
    }
    return value == 0;
}

////////////////////////////////////////////////////////////////////////////////
// Colors

struct Color {
    float r, g, b, a;
};

constexpr Color Rgb(int r, int g, int b, float a = 1.0f) {
    return {r / 255.0f, g / 255.0f, b / 255.0f, a};
}

inline Color WithAlpha(Color c, float a) {
    c.a = a;
    return c;
}

inline Color Mix(const Color& x, const Color& y, float t) {
    return {x.r + (y.r - x.r) * t, x.g + (y.g - x.g) * t,
            x.b + (y.b - x.b) * t, x.a + (y.a - x.a) * t};
}

inline float Luminance(const Color& c) {
    return c.r * 0.299f + c.g * 0.587f + c.b * 0.114f;
}

constexpr Color kWhite = Rgb(255, 255, 255);
constexpr Color kFallbackAccent = Rgb(76, 194, 255);

inline uint32_t PackPremultiplied(const Color& c, float coverage) {
    const float a = Clamp01(c.a * coverage);
    if (a <= 0.0f) {
        return 0;
    }
    const uint32_t alpha = static_cast<uint32_t>(a * 255.0f + 0.5f);
    auto channel = [&](float v) {
        return std::min(alpha,
                        static_cast<uint32_t>(Clamp01(v) * a * 255.0f + 0.5f));
    };
    return (alpha << 24) | (channel(c.r) << 16) | (channel(c.g) << 8) |
           channel(c.b);
}

inline void BlendOver(uint32_t& dst, uint32_t src) {
    const uint32_t sa = src >> 24;
    if (sa == 0) {
        return;
    }
    if (sa == 255) {
        dst = src;
        return;
    }
    const uint32_t inv = 255 - sa;
    const uint32_t d = dst;
    uint32_t rb = (d & 0x00FF00FF) * inv + 0x00800080;
    rb = ((rb + ((rb >> 8) & 0x00FF00FF)) >> 8) & 0x00FF00FF;
    uint32_t ag = ((d >> 8) & 0x00FF00FF) * inv + 0x00800080;
    ag = (ag + ((ag >> 8) & 0x00FF00FF)) & 0xFF00FF00;
    dst = src + (rb | ag);
}

inline void BlendColor(uint32_t& dst, const Color& c, float coverage) {
    BlendOver(dst, PackPremultiplied(c, coverage));
}

// ClearType reports a coverage value per channel, so text is blended channel by
// channel: the destination is un-premultiplied, mixed and premultiplied again.
inline void BlendText(uint32_t& dst,
                      const Color& c,
                      uint32_t covR,
                      uint32_t covG,
                      uint32_t covB) {
    const float alpha = static_cast<float>(dst >> 24) / 255.0f;
    if (alpha <= 0.0f || (covR | covG | covB) == 0) {
        return;
    }
    auto channel = [&](int shift, float color, uint32_t coverage) {
        const uint32_t value = (dst >> shift) & 0xFF;
        if (coverage == 0) {
            return value;
        }
        const float straight =
            Clamp01(static_cast<float>(value) / 255.0f / alpha);
        const float t = static_cast<float>(coverage) / 255.0f;
        const float out = color * t + straight * (1.0f - t);
        return static_cast<uint32_t>(Clamp01(out * alpha) * 255.0f + 0.5f);
    };
    dst = (dst & 0xFF000000u) | (channel(16, c.r, covR) << 16) |
          (channel(8, c.g, covG) << 8) | channel(0, c.b, covB);
}

////////////////////////////////////////////////////////////////////////////////
// Surfaces

class Surface {
  public:
    Surface() = default;
    ~Surface() { Reset(); }
    Surface(const Surface&) = delete;
    Surface& operator=(const Surface&) = delete;

    bool Resize(int width, int height) {
        width = std::max(width, 1);
        height = std::max(height, 1);
        if (!bits_ || width > stride_ || height > capacityHeight_) {
            Reset();
            const int capacityWidth = (width + 63) & ~63;
            const int capacityHeight = (height + 63) & ~63;

            BITMAPINFO info{};
            info.bmiHeader.biSize = sizeof(info.bmiHeader);
            info.bmiHeader.biWidth = capacityWidth;
            info.bmiHeader.biHeight = -capacityHeight;  // Top-down.
            info.bmiHeader.biPlanes = 1;
            info.bmiHeader.biBitCount = 32;
            info.bmiHeader.biCompression = BI_RGB;

            dc_ = CreateCompatibleDC(nullptr);
            if (!dc_) {
                return false;
            }
            void* bits = nullptr;
            bitmap_ = CreateDIBSection(dc_, &info, DIB_RGB_COLORS, &bits,
                                       nullptr, 0);
            if (!bitmap_ || !bits) {
                Reset();
                return false;
            }
            oldBitmap_ = SelectObject(dc_, bitmap_);
            bits_ = static_cast<uint32_t*>(bits);
            stride_ = capacityWidth;
            capacityHeight_ = capacityHeight;
        }
        width_ = width;
        height_ = height;
        Clear();
        return true;
    }

    void Reset() {
        if (dc_) {
            if (oldBitmap_) {
                SelectObject(dc_, oldBitmap_);
            }
            DeleteDC(dc_);
        }
        if (bitmap_) {
            DeleteObject(bitmap_);
        }
        dc_ = nullptr;
        bitmap_ = nullptr;
        oldBitmap_ = nullptr;
        bits_ = nullptr;
        width_ = height_ = stride_ = capacityHeight_ = 0;
    }

    void Clear() {
        for (int y = 0; y < height_; y++) {
            memset(Row(y), 0, static_cast<size_t>(width_) * 4);
        }
    }

    bool Valid() const { return bits_ != nullptr; }
    int Width() const { return width_; }
    int Height() const { return height_; }
    HDC Dc() const { return dc_; }
    uint32_t* Row(int y) { return bits_ + static_cast<size_t>(y) * stride_; }
    const uint32_t* Row(int y) const {
        return bits_ + static_cast<size_t>(y) * stride_;
    }

  private:
    HDC dc_ = nullptr;
    HBITMAP bitmap_ = nullptr;
    HGDIOBJ oldBitmap_ = nullptr;
    uint32_t* bits_ = nullptr;
    int width_ = 0;
    int height_ = 0;
    int stride_ = 0;
    int capacityHeight_ = 0;
};

////////////////////////////////////////////////////////////////////////////////
// Signed distance shapes

struct Vec2 {
    float x, y;
};

inline float SdRoundRect(float px,
                         float py,
                         float left,
                         float top,
                         float right,
                         float bottom,
                         float radius) {
    const float hw = (right - left) * 0.5f;
    const float hh = (bottom - top) * 0.5f;
    const float r = std::max(0.0f, std::min(radius, std::min(hw, hh)));
    const float qx = fabsf(px - (left + hw)) - hw + r;
    const float qy = fabsf(py - (top + hh)) - hh + r;
    const float ox = std::max(qx, 0.0f);
    const float oy = std::max(qy, 0.0f);
    return sqrtf(ox * ox + oy * oy) + std::min(std::max(qx, qy), 0.0f) - r;
}

inline float SdSegment(Vec2 p, Vec2 a, Vec2 b) {
    const Vec2 pa{p.x - a.x, p.y - a.y};
    const Vec2 ba{b.x - a.x, b.y - a.y};
    const float lengthSq = ba.x * ba.x + ba.y * ba.y;
    const float h =
        lengthSq > 0 ? Clamp01((pa.x * ba.x + pa.y * ba.y) / lengthSq) : 0.0f;
    const float dx = pa.x - ba.x * h;
    const float dy = pa.y - ba.y * h;
    return sqrtf(dx * dx + dy * dy);
}

inline float Erfc(float x) {
    const float z = fabsf(x);
    const float t = 1.0f / (1.0f + 0.3275911f * z);
    const float y =
        t *
        (0.254829592f +
         t * (-0.284496736f +
              t * (1.421413741f + t * (-1.453152027f + t * 1.061405429f)))) *
        expf(-z * z);
    return x >= 0 ? y : 2.0f - y;
}

inline float SoftCoverage(float distance, float sigma) {
    return 0.5f * Erfc(distance / (sigma * 1.41421356f));
}

template <typename DistanceFn, typename ColorFn>
void FillShape(Surface& s,
               float x0,
               float y0,
               float x1,
               float y1,
               DistanceFn distance,
               ColorFn color) {
    const int ix0 = std::max(0, static_cast<int>(floorf(x0)));
    const int iy0 = std::max(0, static_cast<int>(floorf(y0)));
    const int ix1 = std::min(s.Width(), static_cast<int>(ceilf(x1)));
    const int iy1 = std::min(s.Height(), static_cast<int>(ceilf(y1)));
    for (int y = iy0; y < iy1; y++) {
        uint32_t* row = s.Row(y);
        const float py = y + 0.5f;
        for (int x = ix0; x < ix1; x++) {
            const float px = x + 0.5f;
            const float coverage = Clamp01(0.5f - distance(px, py));
            if (coverage > 0.0f) {
                BlendColor(row[x], color(px, py), coverage);
            }
        }
    }
}

void FillRoundRect(Surface& s,
                   float l,
                   float t,
                   float r,
                   float b,
                   float radius,
                   Color c) {
    FillShape(
        s, l - 1, t - 1, r + 1, b + 1,
        [&](float x, float y) { return SdRoundRect(x, y, l, t, r, b, radius); },
        [&](float, float) { return c; });
}

void FillRoundRectGradient(Surface& s,
                           float l,
                           float t,
                           float r,
                           float b,
                           float radius,
                           Color top,
                           Color bottom) {
    const float height = std::max(1.0f, b - t);
    FillShape(
        s, l - 1, t - 1, r + 1, b + 1,
        [&](float x, float y) { return SdRoundRect(x, y, l, t, r, b, radius); },
        [&](float, float y) { return Mix(top, bottom, Clamp01((y - t) / height)); });
}

void StrokeRoundRect(Surface& s,
                     float l,
                     float t,
                     float r,
                     float b,
                     float radius,
                     float width,
                     Color c) {
    FillShape(
        s, l - width, t - width, r + width, b + width,
        [&](float x, float y) {
            return fabsf(SdRoundRect(x, y, l, t, r, b, radius)) - width * 0.5f;
        },
        [&](float, float) { return c; });
}

void FillCircle(Surface& s, float cx, float cy, float radius, Color c) {
    FillShape(
        s, cx - radius - 1, cy - radius - 1, cx + radius + 1, cy + radius + 1,
        [&](float x, float y) {
            return sqrtf((x - cx) * (x - cx) + (y - cy) * (y - cy)) - radius;
        },
        [&](float, float) { return c; });
}

void StrokeCircle(Surface& s,
                  float cx,
                  float cy,
                  float radius,
                  float width,
                  Color c) {
    FillShape(
        s, cx - radius - width, cy - radius - width, cx + radius + width,
        cy + radius + width,
        [&](float x, float y) {
            return fabsf(sqrtf((x - cx) * (x - cx) + (y - cy) * (y - cy)) -
                         radius) -
                   width * 0.5f;
        },
        [&](float, float) { return c; });
}

void StrokeLine(Surface& s, Vec2 a, Vec2 b, float width, Color c) {
    const float half = width * 0.5f;
    FillShape(
        s, std::min(a.x, b.x) - width, std::min(a.y, b.y) - width,
        std::max(a.x, b.x) + width, std::max(a.y, b.y) + width,
        [&](float x, float y) { return SdSegment({x, y}, a, b) - half; },
        [&](float, float) { return c; });
}

void DrawShadow(Surface& s,
                float l,
                float t,
                float r,
                float b,
                float radius,
                float sigma,
                Color c) {
    const float reach = sigma * 3.0f;
    const int ix0 = std::max(0, static_cast<int>(floorf(l - reach)));
    const int iy0 = std::max(0, static_cast<int>(floorf(t - reach)));
    const int ix1 = std::min(s.Width(), static_cast<int>(ceilf(r + reach)));
    const int iy1 = std::min(s.Height(), static_cast<int>(ceilf(b + reach)));
    for (int y = iy0; y < iy1; y++) {
        uint32_t* row = s.Row(y);
        for (int x = ix0; x < ix1; x++) {
            const float d = SdRoundRect(x + 0.5f, y + 0.5f, l, t, r, b, radius);
            const float coverage = SoftCoverage(d, sigma);
            if (coverage > 0.002f) {
                BlendColor(row[x], c, coverage);
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
// Text

class FontCache {
  public:
    void Init() {
        static const PCWSTR kUiFaces[] = {L"Segoe UI Variable Text", L"Segoe UI",
                                          L"Tahoma"};
        face_ = PickFace(kUiFaces, ARRAYSIZE(kUiFaces));
        // The shell's symbol font. Windows 11 ships the Fluent set, Windows 10
        // the older MDL2 one, and both carry the same glyph points.
        static const PCWSTR kIconFaces[] = {L"Segoe Fluent Icons",
                                            L"Segoe MDL2 Assets"};
        iconFace_ = PickFace(kIconFaces, ARRAYSIZE(kIconFaces));
    }

    HFONT Get(int px, int weight) { return Find(entries_, face_, px, weight); }

    // A glyph font for the icons, so a symbol is drawn by the system set rather
    // than by hand. The size is the glyph's em square, roughly its cap height.
    // Symbols use grayscale antialiasing: ClearType's subpixel fringes are there
    // to sharpen letter stems, and on a two-color outline they only smear the
    // edges.
    HFONT GetIcon(int px) {
        return Find(iconEntries_, iconFace_, px, FW_NORMAL,
                    ANTIALIASED_QUALITY);
    }

    void Clear() {
        for (const Entry& entry : entries_) {
            DeleteObject(entry.font);
        }
        entries_.clear();
        for (const Entry& entry : iconEntries_) {
            DeleteObject(entry.font);
        }
        iconEntries_.clear();
    }

  private:
    struct Entry {
        int px;
        int weight;
        HFONT font;
    };

    HFONT Find(std::vector<Entry>& entries,
               const std::wstring& face,
               int px,
               int weight,
               int quality = CLEARTYPE_QUALITY) {
        px = std::max(px, 6);
        for (const Entry& entry : entries) {
            if (entry.px == px && entry.weight == weight) {
                return entry.font;
            }
        }
        HFONT font = CreateFontW(-px, 0, 0, 0, weight, FALSE, FALSE, FALSE,
                                 DEFAULT_CHARSET, OUT_TT_PRECIS,
                                 CLIP_DEFAULT_PRECIS, quality,
                                 DEFAULT_PITCH | FF_DONTCARE, face.c_str());
        if (font) {
            entries.push_back({px, weight, font});
        }
        return font ? font : static_cast<HFONT>(GetStockObject(DEFAULT_GUI_FONT));
    }

    static int CALLBACK EnumFontProc(const LOGFONTW*,
                                     const TEXTMETRICW*,
                                     DWORD,
                                     LPARAM param) {
        *reinterpret_cast<bool*>(param) = true;
        return 0;
    }

    static std::wstring PickFace(const PCWSTR* faces, size_t count) {
        HDC dc = GetDC(nullptr);
        for (size_t i = 0; i < count; i++) {
            LOGFONTW lf{};
            lf.lfCharSet = DEFAULT_CHARSET;
            wcsncpy_s(lf.lfFaceName, faces[i], _TRUNCATE);
            bool found = false;
            EnumFontFamiliesExW(dc, &lf, EnumFontProc,
                                reinterpret_cast<LPARAM>(&found), 0);
            if (found) {
                ReleaseDC(nullptr, dc);
                return faces[i];
            }
        }
        ReleaseDC(nullptr, dc);
        return faces[count - 1];
    }

    std::vector<Entry> entries_;
    std::vector<Entry> iconEntries_;
    std::wstring face_ = L"Segoe UI";
    std::wstring iconFace_ = L"Segoe MDL2 Assets";
};

// ClearType coverage is curved before it is blended, and there are only two
// curves in use, so both tables are built once instead of 256 powf calls on
// every string that is drawn.
struct TextGamma {
    uint32_t v[256];
    explicit TextGamma(float gamma) {
        for (int i = 0; i < 256; i++) {
            v[i] = static_cast<uint32_t>(powf(static_cast<float>(i) / 255.0f,
                                              gamma) *
                                              255.0f + 0.5f);
        }
    }
    static const TextGamma& For(float gamma) {
        static const TextGamma dark{0.8f};
        static const TextGamma light{0.9f};
        return gamma < 0.85f ? dark : light;
    }
};

class TextPainter {
  public:
    SIZE Measure(HFONT font, const std::wstring& text,
                 UINT flags = DT_SINGLELINE) {
        EnsureMeasureDc();
        HGDIOBJ old = SelectObject(measureDc_, font);
        RECT rc{0, 0, 0, 0};
        DrawTextW(measureDc_, text.empty() ? L" " : text.c_str(),
                  text.empty() ? 1 : static_cast<int>(text.size()), &rc,
                  flags | DT_CALCRECT | DT_NOPREFIX);
        SelectObject(measureDc_, old);
        return {rc.right - rc.left, rc.bottom - rc.top};
    }

    int Width(HFONT font, const wchar_t* text, int length) {
        if (length <= 0) {
            return 0;
        }
        EnsureMeasureDc();
        HGDIOBJ old = SelectObject(measureDc_, font);
        SIZE size{};
        GetTextExtentPoint32W(measureDc_, text, length, &size);
        SelectObject(measureDc_, old);
        return size.cx;
    }

    // Shortens the text until it fits, ending it with an ellipsis.
    std::wstring Ellipsize(HFONT font, std::wstring text, int width) {
        if (width <= 0) {
            return std::wstring();
        }
        if (Width(font, text.c_str(), static_cast<int>(text.size())) <= width) {
            return text;
        }
        while (!text.empty()) {
            text.pop_back();
            std::wstring candidate = text + L"\x2026";
            if (Width(font, candidate.c_str(),
                      static_cast<int>(candidate.size())) <= width) {
                return candidate;
            }
        }
        return std::wstring();
    }

    void Draw(Surface& dst,
              HFONT font,
              const std::wstring& text,
              RECT layout,
              RECT clip,
              UINT flags,
              Color color) {
        RECT bounds{0, 0, dst.Width(), dst.Height()};
        if (!IntersectRect(&clip, &clip, &bounds)) {
            return;
        }
        const int w = clip.right - clip.left;
        const int h = clip.bottom - clip.top;
        if (text.empty() || w <= 0 || h <= 0 || !mask_.Resize(w, h)) {
            return;
        }
        HDC dc = mask_.Dc();
        HGDIOBJ old = SelectObject(dc, font);
        SetTextColor(dc, RGB(255, 255, 255));
        SetBkMode(dc, TRANSPARENT);
        OffsetRect(&layout, -clip.left, -clip.top);
        DrawTextW(dc, text.c_str(), static_cast<int>(text.size()), &layout,
                  flags | DT_NOPREFIX);
        SelectObject(dc, old);
        GdiFlush();  // Finish GDI text before reading the mask.

        const float luminance =
            color.r * 0.3f + color.g * 0.59f + color.b * 0.11f;
        const uint32_t* lut = TextGamma::For(luminance < 0.5f ? 0.8f : 0.9f).v;
        for (int y = 0; y < h; y++) {
            const uint32_t* src = mask_.Row(y);
            uint32_t* row = dst.Row(clip.top + y) + clip.left;
            for (int x = 0; x < w; x++) {
                const uint32_t m = src[x];
                if (m & 0x00FFFFFFu) {
                    BlendText(row[x], color, lut[(m >> 16) & 0xFF],
                              lut[(m >> 8) & 0xFF], lut[m & 0xFF]);
                }
            }
        }
    }

    void Draw(Surface& dst,
              HFONT font,
              const std::wstring& text,
              RECT layout,
              UINT flags,
              Color color) {
        Draw(dst, font, text, layout, layout, flags, color);
    }

    ~TextPainter() {
        if (measureDc_) {
            DeleteDC(measureDc_);
        }
    }

  private:
    void EnsureMeasureDc() {
        if (!measureDc_) {
            measureDc_ = CreateCompatibleDC(nullptr);
        }
    }

    Surface mask_;
    HDC measureDc_ = nullptr;
};

FontCache g_fonts;
TextPainter g_text;

////////////////////////////////////////////////////////////////////////////////
// Fluent glyphs

// Every point the panel draws comes from Segoe Fluent Icons, the same set the
// Windows 11 shell uses, so the symbols read as part of the system.
// A crescent, the symbol Windows gives its own night-time display settings.
// It says "darker" at a glance, where a sun only says "brightness".
constexpr PCWSTR kGlyphMoon = L"\uE708";
constexpr PCWSTR kGlyphPalette = L"\uE790";
constexpr PCWSTR kGlyphMonitor = L"\uE7F4";
constexpr PCWSTR kGlyphClose = L"\uE8BB";

// Glyphs are drawn from their centre outwards and at an explicit em size, so
// the symbol lands on the row of text it belongs to instead of overflowing a
// box that also has to hold the pointer target.
void DrawGlyph(Surface& s,
               PCWSTR glyph,
               float centerX,
               float centerY,
               float em,
               Color c) {
    HFONT font = g_fonts.GetIcon(static_cast<int>(em + 0.5f));
    const float half = em * 0.5f;
    RECT layout{static_cast<LONG>(lround(centerX - half)),
                static_cast<LONG>(lround(centerY - half)),
                static_cast<LONG>(lround(centerX + half)),
                static_cast<LONG>(lround(centerY + half))};
    g_text.Draw(s, font, glyph, layout, DT_CENTER | DT_VCENTER | DT_SINGLELINE,
                c);
}

////////////////////////////////////////////////////////////////////////////////
// Window helpers

// The instance every window class and popup is created from.
HINSTANCE g_instance;

class LayeredWindow {
  public:
    bool Create(DWORD exStyle, PCWSTR className, PCWSTR title, void* param) {
        hwnd_ = CreateWindowExW(exStyle | WS_EX_LAYERED | WS_EX_TOPMOST |
                                    WS_EX_TOOLWINDOW,
                                className, title, WS_POPUP, 0, 0, 1, 1, nullptr,
                                nullptr, g_instance, param);
        // A window that has just been made has nothing on it, whatever the last
        // one held.
        contentDirty_ = true;
        return hwnd_ != nullptr;
    }

    void Destroy() {
        if (hwnd_) {
            DestroyWindow(hwnd_);
            hwnd_ = nullptr;
        }
        visible_ = false;
    }

    // Uploading is the expensive half of a frame, so an unchanged picture in an
    // unchanged place is not uploaded again. The panel asks for a present on
    // every animation tick, and a tick that only finished a glide has nothing
    // new to say.
    bool Present(Surface& surface, int x, int y, BYTE alpha) {
        if (!hwnd_ || !surface.Valid()) {
            return false;
        }
        if (!contentDirty_ && x == x_ && y == y_ &&
            surface.Width() == width_ && surface.Height() == height_ &&
            alpha == alpha_) {
            return true;
        }
        POINT dst{x, y};
        SIZE size{surface.Width(), surface.Height()};
        POINT src{0, 0};
        BLENDFUNCTION blend{AC_SRC_OVER, 0, alpha, AC_SRC_ALPHA};
        GdiFlush();  // Finish GDI drawing before uploading the pixels.
        if (!UpdateLayeredWindow(hwnd_, nullptr, &dst, &size, surface.Dc(), &src,
                                 0, &blend, ULW_ALPHA)) {
            return false;
        }
        x_ = x;
        y_ = y;
        width_ = size.cx;
        height_ = size.cy;
        alpha_ = alpha;
        contentDirty_ = false;
        return true;
    }

    // Set by whatever repaints the surface this window shows, so a present knows
    // the pixels it is holding are stale even when the window has not moved.
    void MarkContentDirty() { contentDirty_ = true; }

    void Move(int x, int y) {
        if (hwnd_ && (x != x_ || y != y_)) {
            SetWindowPos(hwnd_, nullptr, x, y, 0, 0,
                         SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE |
                             SWP_NOOWNERZORDER);
            x_ = x;
            y_ = y;
        }
    }

    // Keeps the window at the top of the topmost band without disturbing its
    // position, size or focus.
    void Raise() {
        if (hwnd_ && visible_) {
            SetWindowPos(hwnd_, HWND_TOPMOST, 0, 0, 0, 0,
                         SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
        }
    }

    void Show(bool activate) {
        if (hwnd_ && !visible_) {
            SetWindowPos(hwnd_, HWND_TOPMOST, 0, 0, 0, 0,
                         SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW |
                             (activate ? 0 : SWP_NOACTIVATE));
            visible_ = true;
        }
    }

    void Hide() {
        if (hwnd_ && visible_) {
            ShowWindow(hwnd_, SW_HIDE);
            visible_ = false;
        }
    }

    HWND Hwnd() const { return hwnd_; }
    bool Visible() const { return visible_; }
    int X() const { return x_; }
    int Y() const { return y_; }
    int Width() const { return width_; }
    int Height() const { return height_; }

  private:
    HWND hwnd_ = nullptr;
    bool visible_ = false;
    int x_ = 0;
    int y_ = 0;
    int width_ = 0;
    int height_ = 0;
    BYTE alpha_ = 255;
    bool contentDirty_ = true;
};

HANDLE g_workerThread;
HANDLE g_workerReadyEvent;
HWND g_messageWindow;

constexpr WCHAR kPanelClass[] = L"ExtraDimPanel_" WH_MOD_ID;
constexpr WCHAR kShadowClass[] = L"ExtraDimShadow_" WH_MOD_ID;
constexpr WCHAR kOverlayClass[] = L"ExtraDimOverlay_" WH_MOD_ID;
constexpr WCHAR kMessageClass[] = L"ExtraDimMessages_" WH_MOD_ID;

LayeredWindow g_panel;
LayeredWindow g_shadow;

////////////////////////////////////////////////////////////////////////////////
// Runtime phases

enum Phase {
    kPhaseClosed,
    kPhaseOpening,
    kPhaseOpen,
    kPhaseClosing,
};

Phase g_phase = kPhaseClosed;
float g_openAnim = 0.0f;
double g_animTick = 0.0;

////////////////////////////////////////////////////////////////////////////////
// Settings

enum ThemeMode {
    kThemeAuto,
    kThemeLight,
    kThemeDark,
};

enum DimMethod {
    kMethodOverlay,
    kMethodGamma,
};

std::wstring GetStringSetting(PCWSTR name) {
    PCWSTR value = Wh_GetStringSetting(name);
    std::wstring result = value ? value : L"";
    if (value) {
        Wh_FreeStringSetting(value);
    }
    return Trim(result);
}

// Windhawk cannot tell "the user chose false" from "the setting was never
// written", so the string is asked first: a value that reads back empty is a
// default that has not been touched, and keeps the documented fallback.
bool BoolSetting(PCWSTR name, bool fallback) {
    const std::wstring text = GetStringSetting(name);
    if (text.empty()) {
        return fallback;
    }
    return Wh_GetIntSetting(name) != 0;
}

Color WindowsAccentColor() {
    DWORD value = 0;
    DWORD size = sizeof(value);
    if (RegGetValueW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\DWM",
                     L"AccentColor", RRF_RT_REG_DWORD, nullptr, &value,
                     &size) == ERROR_SUCCESS) {
        // Stored as 0xAABBGGRR.
        return Rgb(value & 0xFF, (value >> 8) & 0xFF, (value >> 16) & 0xFF);
    }
    return kFallbackAccent;
}

Color ParseColorText(std::wstring text, Color fallback) {
    text = Lower(Trim(text));
    if (text.empty()) {
        return fallback;
    }
    if (text[0] == L'#') {
        text.erase(0, 1);
    } else if (text.rfind(L"0x", 0) == 0) {
        text.erase(0, 2);
    }
    const bool isHex =
        (text.size() == 3 || text.size() == 6) &&
        text.find_first_not_of(L"0123456789abcdef") == std::wstring::npos;
    if (isHex) {
        const unsigned long v = wcstoul(text.c_str(), nullptr, 16);
        if (text.size() == 3) {
            return Rgb(static_cast<int>(((v >> 8) & 0xF) * 17),
                       static_cast<int>(((v >> 4) & 0xF) * 17),
                       static_cast<int>((v & 0xF) * 17));
        }
        return Rgb(static_cast<int>((v >> 16) & 0xFF),
                   static_cast<int>((v >> 8) & 0xFF),
                   static_cast<int>(v & 0xFF));
    }

    int values[3] = {0, 0, 0};
    int count = 0;
    for (size_t i = 0; i < text.size() && count < 3;) {
        if (iswdigit(text[i])) {
            values[count++] = static_cast<int>(
                std::min(255L, wcstol(text.c_str() + i, nullptr, 10)));
            while (i < text.size() && iswdigit(text[i])) {
                i++;
            }
        } else {
            i++;
        }
    }
    if (count == 3) {
        return Rgb(values[0], values[1], values[2]);
    }
    return fallback;
}

Color ResolveAccent(const std::wstring& choice, const std::wstring& custom) {
    static const struct {
        PCWSTR name;
        Color color;
    } kPresets[] = {
        {L"blue", Rgb(76, 194, 255)},   {L"red", Rgb(255, 99, 99)},
        {L"green", Rgb(76, 209, 133)},  {L"teal", Rgb(0, 190, 181)},
        {L"purple", Rgb(175, 122, 255)}, {L"pink", Rgb(255, 122, 194)},
        {L"orange", Rgb(255, 150, 61)}, {L"gold", Rgb(240, 190, 80)},
        {L"mint", Rgb(94, 230, 186)},
    };
    const std::wstring key = Lower(Trim(choice));
    if (key == L"custom") {
        return ParseColorText(custom, WindowsAccentColor());
    }
    for (const auto& preset : kPresets) {
        if (key == preset.name) {
            return preset.color;
        }
    }
    return WindowsAccentColor();
}

struct Hotkey {
    UINT mods = 0;
    UINT vk = 0;
    std::wstring label;     // "Ctrl+Alt+Shift+D", for tooltips and readouts
    std::wstring keyLabel;  // "D", so the footer can draw one cap per key

    bool Bound() const { return vk != 0; }
};

bool ParseKeyName(const std::wstring& raw, UINT& vk, std::wstring& label) {
    std::wstring key = Lower(Trim(raw));
    if (key.empty()) {
        return false;
    }
    if (key == L"space") {
        vk = VK_SPACE;
        label = L"Space";
        return true;
    }
    if (key == L"tab") {
        vk = VK_TAB;
        label = L"Tab";
        return true;
    }
    if (key == L"enter" || key == L"return") {
        vk = VK_RETURN;
        label = L"Enter";
        return true;
    }
    if (key == L"esc" || key == L"escape") {
        vk = VK_ESCAPE;
        label = L"Esc";
        return true;
    }
    if (key == L"backspace" || key == L"back") {
        vk = VK_BACK;
        label = L"Backspace";
        return true;
    }
    if (key == L"delete" || key == L"del") {
        vk = VK_DELETE;
        label = L"Delete";
        return true;
    }
    if (key == L"insert" || key == L"ins") {
        vk = VK_INSERT;
        label = L"Insert";
        return true;
    }
    if (key == L"home") {
        vk = VK_HOME;
        label = L"Home";
        return true;
    }
    if (key == L"end") {
        vk = VK_END;
        label = L"End";
        return true;
    }
    if (key == L"pageup" || key == L"pgup") {
        vk = VK_PRIOR;
        label = L"PgUp";
        return true;
    }
    if (key == L"pagedown" || key == L"pgdn") {
        vk = VK_NEXT;
        label = L"PgDn";
        return true;
    }
    if (key == L"left") {
        vk = VK_LEFT;
        label = L"Left";
        return true;
    }
    if (key == L"right") {
        vk = VK_RIGHT;
        label = L"Right";
        return true;
    }
    if (key == L"up") {
        vk = VK_UP;
        label = L"Up";
        return true;
    }
    if (key == L"down") {
        vk = VK_DOWN;
        label = L"Down";
        return true;
    }
    if (key == L"minus" || key == L"-") {
        vk = VK_OEM_MINUS;
        label = L"-";
        return true;
    }
    if (key == L"plus" || key == L"equals" || key == L"=") {
        vk = VK_OEM_PLUS;
        label = L"+";
        return true;
    }
    if (key.size() > 1 && key[0] == L'f') {
        const int number = wcstol(key.c_str() + 1, nullptr, 10);
        if (number >= 1 && number <= 24) {
            vk = static_cast<UINT>(VK_F1 + number - 1);
            label = L"F" + Number(number);
            return true;
        }
        return false;
    }
    if (key.size() == 1) {
        const wchar_t c = static_cast<wchar_t>(towupper(key[0]));
        if ((c >= L'A' && c <= L'Z') || (c >= L'0' && c <= L'9')) {
            vk = static_cast<UINT>(c);
            label.assign(1, c);
            return true;
        }
    }
    return false;
}

// Accepts a chord written the way it is pressed: ctrl+alt+d, win+shift+f5.
// An empty chord is legal and simply leaves the action without a key.
bool ParseHotkeyText(const std::wstring& text, Hotkey& out) {
    out = Hotkey();
    const std::wstring value = Lower(Trim(text));
    if (value.empty()) {
        return true;
    }
    UINT mods = 0;
    std::wstring keyPart;
    size_t start = 0;
    while (start <= value.size()) {
        const size_t plus = value.find(L'+', start);
        const std::wstring token =
            Trim(value.substr(start, plus == std::wstring::npos
                                          ? std::wstring::npos
                                          : plus - start));
        if (!token.empty()) {
            if (token == L"ctrl" || token == L"control") {
                mods |= MOD_CONTROL;
            } else if (token == L"alt") {
                mods |= MOD_ALT;
            } else if (token == L"shift") {
                mods |= MOD_SHIFT;
            } else if (token == L"win" || token == L"windows" ||
                       token == L"meta") {
                mods |= MOD_WIN;
            } else {
                keyPart = token;
            }
        }
        if (plus == std::wstring::npos) {
            break;
        }
        start = plus + 1;
    }
    UINT vk = 0;
    std::wstring label;
    if (keyPart.empty() || !ParseKeyName(keyPart, vk, label)) {
        return false;
    }
    // A global hotkey without a modifier eats that key from every program on
    // the machine, so a lone letter or digit is refused. The function keys are
    // the exception: nothing types with F12.
    if (mods == 0 && !(vk >= VK_F1 && vk <= VK_F24)) {
        return false;
    }
    out.mods = mods;
    out.vk = vk;
    out.label.clear();
    if (mods & MOD_CONTROL) {
        out.label += L"Ctrl+";
    }
    if (mods & MOD_ALT) {
        out.label += L"Alt+";
    }
    if (mods & MOD_SHIFT) {
        out.label += L"Shift+";
    }
    if (mods & MOD_WIN) {
        out.label += L"Win+";
    }
    out.label += label;
    out.keyLabel = label;
    return true;
}

#ifndef MOD_NOREPEAT
#define MOD_NOREPEAT 0x4000
#endif

struct Settings {
    bool startDimmed = false;
    int startLevel = 40;
    bool rememberValues = true;

    int theme = kThemeAuto;
    std::wstring accentChoice = L"accent";
    std::wstring customAccent;
    int opacity = 96;
    int panelScale = 100;

    std::wstring position = L"right";
    bool primaryMonitor = false;
    int offsetX = 0;
    int offsetY = 0;
    bool closeOnFocusLoss = true;
    bool showTrayIcon = true;

    int step = 10;
    int maximum = 90;
    int method = kMethodOverlay;
    bool allDisplays = true;
    // What Windhawk says, before the panel's own chips are taken into account.
    // The chip remembers the setting it replaced, so a later change in Windhawk
    // is noticed and wins again.
    int settingMethod = kMethodOverlay;
    bool settingAllDisplays = true;
    bool excludeCapture = true;
    bool keepPanelBright = false;
    bool smooth = true;
    bool dimPointer = false;

    Color tintColor = Rgb(255, 181, 107);
    int tintDefault = 0;

    Hotkey panelHotkey;
    Hotkey toggleDimHotkey;
    Hotkey toggleTintHotkey;
    Hotkey dimUpHotkey;
    Hotkey dimDownHotkey;
};

Settings g_settings;

Settings LoadSettings() {
    Settings s;

    s.startDimmed = BoolSetting(L"Startup.StartDimmed", false);
    // A settings file written before a group existed reads back as zero, so
    // every numeric falls through to its documented default. A value the user
    // really set to zero is only possible where zero is a legal level.
    const int startLevel = Wh_GetIntSetting(L"Startup.StartLevel");
    s.startLevel = ClampInt(startLevel == 0 ? 40 : startLevel, 5, 100);
    s.rememberValues = BoolSetting(L"Startup.RememberValues", true);

    const std::wstring theme = GetStringSetting(L"Appearance.Theme");
    s.theme = theme == L"light" ? kThemeLight
              : theme == L"dark" ? kThemeDark
                                 : kThemeAuto;
    s.accentChoice = GetStringSetting(L"Appearance.AccentColor");
    if (s.accentChoice.empty()) {
        s.accentChoice = L"accent";
    }
    s.customAccent = GetStringSetting(L"Appearance.CustomAccentColor");
    const int opacity = Wh_GetIntSetting(L"Appearance.Opacity");
    s.opacity = ClampInt(opacity == 0 ? 96 : opacity, 70, 100);
    const int panelScale = Wh_GetIntSetting(L"Appearance.PanelScale");
    s.panelScale = ClampInt(panelScale == 0 ? 100 : panelScale, 80, 140);

    s.position = GetStringSetting(L"Placement.Position");
    if (s.position.empty()) {
        s.position = L"right";
    }
    s.primaryMonitor = GetStringSetting(L"Placement.Monitor") == L"primary";
    s.offsetX =
        ClampInt(Wh_GetIntSetting(L"Placement.HorizontalOffset"), -2000, 2000);
    s.offsetY =
        ClampInt(Wh_GetIntSetting(L"Placement.VerticalOffset"), -2000, 2000);
    s.closeOnFocusLoss = BoolSetting(L"Placement.CloseOnFocusLoss", true);
    s.showTrayIcon = BoolSetting(L"Placement.ShowTrayIcon", true);

    // A settings file written before a group existed reads back as zero, so
    // every numeric falls through to its documented default.
    const int step = Wh_GetIntSetting(L"Dim.Step");
    s.step = ClampInt(step == 0 ? 10 : step, 1, 25);
    const int maximum = Wh_GetIntSetting(L"Dim.Maximum");
    s.maximum = ClampInt(maximum == 0 ? 90 : maximum, 30, 100);
    s.method = GetStringSetting(L"Dim.Method") == L"gamma" ? kMethodGamma
                                                           : kMethodOverlay;
    s.allDisplays = BoolSetting(L"Dim.AllDisplays", true);
    s.excludeCapture = BoolSetting(L"Dim.ExcludeFromCapture", true);
    s.keepPanelBright = BoolSetting(L"Dim.KeepPanelBright", false);
    s.smooth = BoolSetting(L"Dim.SmoothTransitions", true);
    s.dimPointer = BoolSetting(L"Dim.DimPointer", false);

    s.tintColor = ParseColorText(
        GetStringSetting(L"Tint.Color"), Rgb(255, 181, 107));
    // Zero is a legal strength here, so the string form is what tells a value
    // the user set to zero from a group that isn't in the settings file yet.
    const std::wstring tintDefault = GetStringSetting(L"Tint.DefaultStrength");
    s.tintDefault = tintDefault.empty()
                        ? 0
                        : ClampInt(Wh_GetIntSetting(L"Tint.DefaultStrength"),
                                   0, 60);

    if (!ParseHotkeyText(GetStringSetting(L"Shortcuts.PanelHotkey"),
                         s.panelHotkey)) {
        ParseHotkeyText(L"ctrl+alt+shift+d", s.panelHotkey);
    }
    if (!ParseHotkeyText(GetStringSetting(L"Shortcuts.ToggleDimHotkey"),
                         s.toggleDimHotkey)) {
        ParseHotkeyText(L"ctrl+alt+d", s.toggleDimHotkey);
    }
    if (!ParseHotkeyText(GetStringSetting(L"Shortcuts.ToggleTintHotkey"),
                         s.toggleTintHotkey)) {
        ParseHotkeyText(L"ctrl+alt+t", s.toggleTintHotkey);
    }
    if (!ParseHotkeyText(GetStringSetting(L"Shortcuts.DimUpHotkey"),
                         s.dimUpHotkey)) {
        ParseHotkeyText(L"ctrl+alt+shift+up", s.dimUpHotkey);
    }
    if (!ParseHotkeyText(GetStringSetting(L"Shortcuts.DimDownHotkey"),
                         s.dimDownHotkey)) {
        ParseHotkeyText(L"ctrl+alt+shift+down", s.dimDownHotkey);
    }

    if (s.startLevel > s.maximum) {
        s.startLevel = s.maximum;
    }
    return s;
}

////////////////////////////////////////////////////////////////////////////////
// Appearance

struct Palette {
    Color surfaceTop;
    Color surfaceBottom;
    Color glassTint;
    Color card;
    Color cardHover;
    Color border;
    Color borderStrong;
    Color text;
    Color textSecondary;
    Color textTertiary;
    Color track;
    Color knob;
    Color chip;
    Color chipHover;
    Color shadow;
};

Palette g_palette;
Color g_accent = kFallbackAccent;
bool g_darkTheme = true;
float g_uiScale = 1.0f;

int ScaledPx(float value) {
    return static_cast<int>(lround(value * g_uiScale));
}

void ResolvePalette() {
    const int theme = g_settings.theme == kThemeAuto
                          ? (SystemUsesDarkTheme() ? kThemeDark : kThemeLight)
                          : g_settings.theme;
    g_darkTheme = theme == kThemeDark;
    g_accent =
        ResolveAccent(g_settings.accentChoice, g_settings.customAccent);
    const float surfaceAlpha = Clamp01(g_settings.opacity / 100.0f);

    Palette& p = g_palette;
    if (g_darkTheme) {
        p.surfaceTop = Rgb(40, 40, 40, 0.90f * surfaceAlpha);
        p.surfaceBottom = Rgb(26, 26, 26, 0.94f * surfaceAlpha);
        p.glassTint = Rgb(34, 34, 34, 0.86f);
        p.card = Rgb(255, 255, 255, 0.045f);
        p.cardHover = Rgb(255, 255, 255, 0.08f);
        p.border = Rgb(255, 255, 255, 0.075f);
        p.borderStrong = Rgb(255, 255, 255, 0.16f);
        p.text = Rgb(255, 255, 255, 0.97f);
        p.textSecondary = Rgb(255, 255, 255, 0.63f);
        p.textTertiary = Rgb(255, 255, 255, 0.42f);
        p.track = Rgb(255, 255, 255, 0.16f);
        p.knob = Rgb(255, 255, 255);
        p.chip = Rgb(255, 255, 255, 0.06f);
        p.chipHover = Rgb(255, 255, 255, 0.11f);
        p.shadow = Rgb(0, 0, 0, 0.45f);
    } else {
        p.surfaceTop = Rgb(250, 250, 250, 0.92f * surfaceAlpha);
        p.surfaceBottom = Rgb(240, 240, 240, 0.95f * surfaceAlpha);
        p.glassTint = Rgb(246, 246, 246, 0.88f);
        p.card = Rgb(255, 255, 255, 0.62f);
        p.cardHover = Rgb(255, 255, 255, 0.85f);
        p.border = Rgb(0, 0, 0, 0.07f);
        p.borderStrong = Rgb(0, 0, 0, 0.14f);
        p.text = Rgb(24, 24, 24, 0.98f);
        p.textSecondary = Rgb(0, 0, 0, 0.62f);
        p.textTertiary = Rgb(0, 0, 0, 0.42f);
        p.track = Rgb(0, 0, 0, 0.18f);
        p.knob = Rgb(255, 255, 255);
        p.chip = Rgb(0, 0, 0, 0.045f);
        p.chipHover = Rgb(0, 0, 0, 0.08f);
        p.shadow = Rgb(0, 0, 0, 0.22f);
    }
}

////////////////////////////////////////////////////////////////////////////////
// Remembered state
//
// The Windhawk settings say where to start; this storage says where the user
// left the two toggles and the two choices the panel can change on its own. It
// is the mod's own Windhawk storage, so it is removed with the mod instead of
// leaving a key behind in the registry.
//
// A tool mod's storage is shared by every account on the machine, while the
// settings it sits beside are per-user. The user's SID is therefore part of
// every value name, so two people who sign in on the same PC keep their own
// levels rather than inheriting each other's.

std::wstring g_storeSuffix;

void StoreInit() {
    HANDLE token = nullptr;
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &token)) {
        Wh_Log(L"Couldn't open the process token: %u", GetLastError());
        return;
    }
    DWORD size = 0;
    GetTokenInformation(token, TokenUser, nullptr, 0, &size);
    if (size) {
        std::vector<BYTE> buffer(size);
        if (GetTokenInformation(token, TokenUser, buffer.data(), size, &size)) {
            const auto* user = reinterpret_cast<const TOKEN_USER*>(buffer.data());
            LPWSTR sid = nullptr;
            if (ConvertSidToStringSidW(user->User.Sid, &sid)) {
                g_storeSuffix = L"_";
                g_storeSuffix += sid;
                LocalFree(sid);
            }
        }
    }
    CloseHandle(token);
}

std::wstring StoreName(PCWSTR name) {
    return std::wstring(name) + g_storeSuffix;
}

int StoreGetInt(PCWSTR name, int fallback) {
    return Wh_GetIntValue(StoreName(name).c_str(), fallback);
}

void StoreSetInt(PCWSTR name, int value) {
    Wh_SetIntValue(StoreName(name).c_str(), value);
}

void StoreDelete(PCWSTR name) {
    Wh_DeleteValue(StoreName(name).c_str());
}

size_t StoreGetBinary(PCWSTR name, void* buffer, size_t size) {
    return Wh_GetBinaryValue(StoreName(name).c_str(), buffer, size);
}

void StoreSetBinary(PCWSTR name, const void* buffer, size_t size) {
    Wh_SetBinaryValue(StoreName(name).c_str(), buffer, size);
}

////////////////////////////////////////////////////////////////////////////////
// Monitors

struct MonitorInfo {
    RECT rect;
    bool primary;
    std::wstring device;
};

BOOL CALLBACK EnumMonitorProc(HMONITOR monitor, HDC, LPRECT, LPARAM param) {
    auto* list = reinterpret_cast<std::vector<MonitorInfo>*>(param);
    MONITORINFOEXW info{};
    info.cbSize = sizeof(info);
    if (GetMonitorInfoW(monitor, &info)) {
        MonitorInfo entry;
        entry.rect = info.rcMonitor;
        entry.primary = (info.dwFlags & MONITORINFOF_PRIMARY) != 0;
        entry.device = info.szDevice;
        list->push_back(entry);
    }
    return TRUE;
}

std::vector<MonitorInfo> CollectMonitors(bool allDisplays) {
    std::vector<MonitorInfo> monitors;
    EnumDisplayMonitors(nullptr, nullptr, EnumMonitorProc,
                        reinterpret_cast<LPARAM>(&monitors));
    if (!allDisplays) {
        for (const MonitorInfo& monitor : monitors) {
            if (monitor.primary) {
                return {monitor};
            }
        }
        if (!monitors.empty()) {
            return {monitors.front()};
        }
    }
    return monitors;
}

////////////////////////////////////////////////////////////////////////////////
// Dimming
//
// Overlay is the default: one click-through layered window per display, filled
// with a single color and given one alpha by the shell, which costs one small
// window and no per-pixel work. Gamma is the alternative for exclusive
// fullscreen: the display's ramp is scaled instead, which reaches everything the
// GPU outputs but depends on the driver.

bool g_dimOn = false;
int g_dimLevel = 40;
bool g_tintOn = false;
int g_tintLevel = 0;
float g_dimShown = 0.0f;
float g_tintShown = 0.0f;
bool g_stateDirty = false;
bool g_gammaFailed = false;

struct OverlayWindow {
    HWND hwnd = nullptr;
    RECT rect{};
};

std::vector<OverlayWindow> g_overlays;
HBRUSH g_overlayBrush = nullptr;
COLORREF g_overlayColor = RGB(0, 0, 0);
BYTE g_overlayAlpha = 0;
bool g_overlayVisible = false;
bool g_topologyDirty = true;

struct OverlayLook {
    bool visible;
    COLORREF color;
    BYTE alpha;
};

OverlayLook ComposeOverlay(float dim, float tint) {
    OverlayLook look{};
    const float dimAlpha = Clamp01(dim / 100.0f) * 0.96f;
    const float tintAlpha = Clamp01(tint / 60.0f) * 0.55f;
    float alpha = 1.0f - (1.0f - dimAlpha) * (1.0f - tintAlpha);
    if (alpha <= 0.004f) {
        look.visible = false;
        look.color = RGB(0, 0, 0);
        look.alpha = 0;
        return look;
    }
    alpha = std::min(alpha, 0.985f);
    // A blend toward a mid-dark warm tone warms the picture and lowers it a
    // little, which is the closest a single layer can get to a color filter.
    const Color warm = {g_settings.tintColor.r * 0.52f,
                        g_settings.tintColor.g * 0.52f,
                        g_settings.tintColor.b * 0.52f, 1.0f};
    const float warmWeight = tintAlpha * (1.0f - dimAlpha) / alpha;
    auto channel = [&](float v) {
        return static_cast<int>(Clamp01(v * warmWeight) * 255.0f + 0.5f);
    };
    look.visible = true;
    look.color = RGB(channel(warm.r), channel(warm.g), channel(warm.b));
    look.alpha = static_cast<BYTE>(Clamp01(alpha) * 255.0f + 0.5f);
    return look;
}

void PaintOverlay(HDC dc, HWND hwnd) {
    if (!dc) {
        return;
    }
    RECT client{};
    GetClientRect(hwnd, &client);
    if (!g_overlayBrush) {
        g_overlayBrush = CreateSolidBrush(g_overlayColor);
    }
    if (g_overlayBrush) {
        FillRect(dc, &client, g_overlayBrush);
    }
}

LRESULT CALLBACK OverlayWndProc(HWND hwnd,
                                UINT message,
                                WPARAM wParam,
                                LPARAM lParam) {
    switch (message) {
        case WM_ERASEBKGND:
            // The whole client is one color, so this is the fill. WM_PAINT then
            // only validates the region: filling there as well would paint the
            // monitor twice for every color change.
            PaintOverlay(reinterpret_cast<HDC>(wParam), hwnd);
            return 1;
        case WM_PAINT: {
            PAINTSTRUCT paint;
            BeginPaint(hwnd, &paint);
            EndPaint(hwnd, &paint);
            return 0;
        }
        case WM_NCHITTEST:
            return HTTRANSPARENT;  // Clicks belong to the window underneath.
        case WM_MOUSEACTIVATE:
            return MA_NOACTIVATE;
        case WM_DISPLAYCHANGE:
            // The panel window owns the topology refresh; this keeps the layer
            // painting itself while that runs.
            return 0;
        default:
            break;
    }
    return DefWindowProcW(hwnd, message, wParam, lParam);
}

#ifndef WDA_EXCLUDEFROMCAPTURE
#define WDA_EXCLUDEFROMCAPTURE 0x00000011
#endif

void OverlayDestroyAll() {
    for (OverlayWindow& overlay : g_overlays) {
        if (overlay.hwnd) {
            DestroyWindow(overlay.hwnd);
        }
    }
    g_overlays.clear();
    g_overlayVisible = false;
}

// Rebuilds the windows when the display topology or the chosen displays
// changed, otherwise just keeps them in place.
void OverlaySync() {
    const std::vector<MonitorInfo> monitors =
        CollectMonitors(g_settings.allDisplays);

    bool same = monitors.size() == g_overlays.size();
    if (same) {
        for (size_t i = 0; i < monitors.size(); i++) {
            const RECT& a = g_overlays[i].rect;
            const RECT& b = monitors[i].rect;
            if (a.left != b.left || a.top != b.top || a.right != b.right ||
                a.bottom != b.bottom) {
                same = false;
                break;
            }
        }
    }
    if (!same) {
        OverlayDestroyAll();
        for (const MonitorInfo& monitor : monitors) {
            const int width = monitor.rect.right - monitor.rect.left;
            const int height = monitor.rect.bottom - monitor.rect.top;
            if (width <= 0 || height <= 0) {
                continue;
            }
            HWND hwnd = CreateWindowExW(
                WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_NOACTIVATE |
                    WS_EX_TOOLWINDOW | WS_EX_TOPMOST,
                kOverlayClass, L"", WS_POPUP, monitor.rect.left,
                monitor.rect.top, width, height, nullptr, nullptr, g_instance,
                nullptr);
            if (!hwnd) {
                Wh_Log(L"Couldn't create a dim layer: %u", GetLastError());
                continue;
            }
            // The exclusion is a request, not a promise: a driver that does not
            // support it leaves the layer in captures, which is worth a line in
            // the log rather than a silent difference from the setting.
            if (!SetWindowDisplayAffinity(
                    hwnd, g_settings.excludeCapture ? WDA_EXCLUDEFROMCAPTURE
                                                    : WDA_NONE)) {
                Wh_Log(L"The display refused the capture exclusion: %u",
                       GetLastError());
            }
            OverlayWindow overlay;
            overlay.hwnd = hwnd;
            overlay.rect = monitor.rect;
            g_overlays.push_back(overlay);
        }
    }
}

// Puts the dim layers and the panel into the order the settings ask for. The
// layers stay topmost so they still cover the taskbar and any other flyout; the
// panel is then lifted above them only when it is meant to stay bright.
//
// Nothing here is needed per frame: presenting a layered window does not touch
// z-order. This runs when the panel opens, when the layers are rebuilt, and on
// the watchdog, which is enough to hold the arrangement.
void ArrangeZOrder() {
    for (OverlayWindow& overlay : g_overlays) {
        if (!overlay.hwnd || !IsWindowVisible(overlay.hwnd)) {
            continue;
        }
        SetWindowPos(overlay.hwnd, HWND_TOPMOST, 0, 0, 0, 0,
                     SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
    }
    if (g_phase == kPhaseClosed || !g_panel.Hwnd() || !g_shadow.Hwnd()) {
        return;
    }
    if (g_settings.keepPanelBright) {
        g_shadow.Raise();
        g_panel.Raise();
    }
    // The shadow travels with the plate, so it is kept directly beneath it. It
    // is shown after the panel, which would otherwise leave it on top.
    SetWindowPos(g_shadow.Hwnd(), g_panel.Hwnd(), 0, 0, 0, 0,
                 SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
}

// Puts the layers up with one look. The brush only follows the color, and a
// layer whose color did not change is not repainted at all: the shell re-blends
// it for the new alpha without a pixel being redrawn. A dim glide is therefore
// alpha-only work, and no monitor is ever filled twice with the same color.
void OverlayShow(const OverlayLook& look) {
    const bool recolor = !g_overlayBrush || look.color != g_overlayColor;
    // A glide lands on the same byte more often than not, and every layer would
    // otherwise be handed the alpha it already has, once per frame.
    if (g_overlayVisible && !recolor && look.alpha == g_overlayAlpha) {
        return;
    }
    if (recolor) {
        if (g_overlayBrush) {
            DeleteObject(g_overlayBrush);
        }
        g_overlayColor = look.color;
        g_overlayBrush = CreateSolidBrush(g_overlayColor);
    }
    if (!g_overlayBrush) {
        return;
    }

    const bool wasVisible = g_overlayVisible;
    for (OverlayWindow& overlay : g_overlays) {
        if (!overlay.hwnd) {
            continue;
        }
        // The alpha is set before the window is put up, so a layer's first
        // frame is never composited at full strength.
        SetLayeredWindowAttributes(overlay.hwnd, 0,
                                   static_cast<BYTE>(look.alpha), LWA_ALPHA);
        if (!wasVisible) {
            SetWindowPos(overlay.hwnd, HWND_TOPMOST, overlay.rect.left,
                         overlay.rect.top, overlay.rect.right - overlay.rect.left,
                         overlay.rect.bottom - overlay.rect.top,
                         SWP_NOACTIVATE | SWP_SHOWWINDOW);
            // A window that has never been up has nothing painted on it yet.
            InvalidateRect(overlay.hwnd, nullptr, TRUE);
        } else if (recolor) {
            InvalidateRect(overlay.hwnd, nullptr, TRUE);
        }
    }
    g_overlayAlpha = look.alpha;
    g_overlayVisible = !g_overlays.empty();
    if (!wasVisible) {
        ArrangeZOrder();
    }
}

void OverlayHideAll() {
    if (!g_overlayVisible) {
        return;
    }
    for (OverlayWindow& overlay : g_overlays) {
        if (!overlay.hwnd) {
            continue;
        }
        ShowWindow(overlay.hwnd, SW_HIDE);
    }
    g_overlayVisible = false;
}

////////////////////////////////////////////////////////////////////////////////
// Gamma ramp dimming

struct GammaDisplay {
    HDC dc = nullptr;
    std::wstring device;
    WORD original[3][256] = {};
    bool saved = false;
};

std::vector<GammaDisplay> g_gammaDisplays;
int g_gammaKey = -1;  // Guards against re-applying an unchanged ramp.

// A display's name is not a legal value name on its own, so only its letters
// and digits are kept: \\.\DISPLAY1 becomes Gamma__DISPLAY1.
std::wstring GammaValueName(const std::wstring& device) {
    std::wstring name = L"Gamma_";
    for (const wchar_t c : device) {
        if (iswalnum(c)) {
            name += c;
        }
    }
    return name;
}

// Puts a display's own ramp back, and lets go of the copy kept for a run that
// ends without its shutdown, so no later run has anything left to undo.
void GammaRestore(GammaDisplay& display) {
    if (!display.dc || !display.saved) {
        return;
    }
    SetDeviceGammaRamp(display.dc, display.original);
    StoreDelete(GammaValueName(display.device).c_str());
}

// A ramp applied when the process was killed, or when the session ended before
// the worker shut down, is still on the display. It is put back here, before
// anything takes a fresh copy, or the dimmed ramp would be recorded as the
// display's own and each run would stack another on top.
void GammaRecoverStored() {
    for (const MonitorInfo& monitor : CollectMonitors(true)) {
        WORD ramp[3][256];
        const std::wstring name = GammaValueName(monitor.device);
        if (StoreGetBinary(name.c_str(), ramp, sizeof(ramp)) != sizeof(ramp)) {
            continue;
        }
        HDC dc = CreateDCW(L"DISPLAY", monitor.device.c_str(), nullptr, nullptr);
        if (dc) {
            SetDeviceGammaRamp(dc, ramp);
            DeleteDC(dc);
        }
        Wh_Log(L"Put back a gamma ramp a previous run left behind");
        StoreDelete(name.c_str());
    }
}

void GammaRelease() {
    for (GammaDisplay& display : g_gammaDisplays) {
        GammaRestore(display);
        if (display.dc) {
            DeleteDC(display.dc);
        }
    }
    g_gammaDisplays.clear();
    g_gammaKey = -1;
}

// Opens a DC per display once, and remembers the ramp that was there before so
// switching the mod off can put it back exactly.
void GammaSync() {
    const std::vector<MonitorInfo> monitors =
        CollectMonitors(g_settings.allDisplays);
    std::vector<GammaDisplay> kept;
    for (const MonitorInfo& monitor : monitors) {
        for (GammaDisplay& display : g_gammaDisplays) {
            if (display.device == monitor.device) {
                kept.push_back(display);
                display.dc = nullptr;  // Moved, not released.
                break;
            }
        }
    }
    for (GammaDisplay& display : g_gammaDisplays) {
        if (display.dc) {
            GammaRestore(display);
            DeleteDC(display.dc);
        }
    }
    for (const MonitorInfo& monitor : monitors) {
        bool present = false;
        for (const GammaDisplay& display : kept) {
            if (display.device == monitor.device) {
                present = true;
                break;
            }
        }
        if (present) {
            continue;
        }
        GammaDisplay display;
        display.device = monitor.device;
        display.dc = CreateDCW(L"DISPLAY", monitor.device.c_str(), nullptr,
                               nullptr);
        if (!display.dc) {
            continue;
        }
        display.saved = GetDeviceGammaRamp(display.dc, display.original) != FALSE;
        if (!display.saved) {
            DeleteDC(display.dc);
            continue;
        }
        // A copy is left in the mod's storage as well as in memory: a run that
        // dies with a ramp applied would otherwise leave the dimmed ramp as the
        // next run's idea of what the display originally looked like.
        StoreSetBinary(GammaValueName(display.device).c_str(), display.original,
                       sizeof(display.original));
        kept.push_back(display);
    }
    g_gammaDisplays.swap(kept);
    g_gammaKey = -1;
}

void GammaApply(float dim, float tint) {
    if (dim <= 0.01f && tint <= 0.01f) {
        // Nothing to apply. The ramp that was there before is put back rather
        // than replaced with an identity curve, because the identity curve is
        // not the same thing as "the display's own calibration".
        if (g_gammaDisplays.empty()) {
            return;
        }
        for (GammaDisplay& display : g_gammaDisplays) {
            GammaRestore(display);
        }
        g_gammaKey = -1;
        return;
    }
    if (g_gammaDisplays.empty()) {
        GammaSync();
    }
    const float scale = 1.0f - Clamp01(dim / 100.0f) * 0.92f;
    float hue[3] = {1.0f, 1.0f, 1.0f};
    const float peak =
        std::max(g_settings.tintColor.r,
                 std::max(g_settings.tintColor.g, g_settings.tintColor.b));
    if (peak > 0.01f) {
        hue[0] = g_settings.tintColor.r / peak;
        hue[1] = g_settings.tintColor.g / peak;
        hue[2] = g_settings.tintColor.b / peak;
    }
    const float warmth = Clamp01(tint / 60.0f);
    // Whole percentages: a driver call per quarter percent is fourteen trips to
    // the kernel per glide for a difference no one can see.
    const int key = static_cast<int>(lround(dim)) * 1000 +
                    static_cast<int>(lround(tint));
    if (key == g_gammaKey) {
        return;
    }

    int failures = 0;
    for (GammaDisplay& display : g_gammaDisplays) {
        if (!display.dc) {
            continue;
        }
        WORD ramp[3][256];
        for (int channel = 0; channel < 3; channel++) {
            const float gain =
                scale * (1.0f + (hue[channel] - 1.0f) * warmth);
            for (int i = 0; i < 256; i++) {
                const float value =
                    Clamp01(static_cast<float>(i) / 255.0f * gain) * 65535.0f;
                ramp[channel][i] =
                    static_cast<WORD>(value + 0.5f);
            }
        }
        if (!SetDeviceGammaRamp(display.dc, ramp)) {
            failures++;
        }
    }
    g_gammaKey = key;
    if (failures > 0) {
        // One display refusing is enough to give up on the method: a desktop
        // that is dimmed on one monitor and untouched on the next reads as a
        // fault, while the layer dims all of them.
        Wh_Log(L"A display refused the gamma ramp; falling back to overlay");
        g_gammaFailed = true;
        GammaRelease();
    }
}

////////////////////////////////////////////////////////////////////////////////
// Applying the current state

float DimTarget() {
    return g_dimOn ? static_cast<float>(g_dimLevel) : 0.0f;
}

float TintTarget() {
    return g_tintOn ? static_cast<float>(g_tintLevel) : 0.0f;
}

void ApplyDim() {
    const bool useGamma = g_settings.method == kMethodGamma && !g_gammaFailed;

    // Enumerating every display on every animation frame would be wasteful, so
    // the topology is only re-read when it is known to have changed. It has to
    // go to whichever mechanism is in use: a monitor plugged in after the mod
    // started would otherwise never be dimmed in gamma mode, and a resolution
    // change would leave the ramp the driver reset in place.
    if (g_topologyDirty) {
        if (useGamma) {
            GammaSync();
        } else {
            OverlaySync();
        }
        g_topologyDirty = false;
    }

    // Gamma first: where it works it also dims the pointer, which no layer can
    // reach. A driver that refuses the ramp is found out right here, and this
    // frame falls straight through to the layer instead of leaving the screen
    // undimmed until something else happens to trigger a repaint.
    if (useGamma) {
        OverlayHideAll();
        GammaApply(g_dimShown, g_tintShown);
        if (!g_gammaFailed) {
            return;
        }
        // The driver just refused. The layers have not been maintained while
        // gamma was in use, so they are built again before they are shown.
        OverlaySync();
    } else if (!g_gammaDisplays.empty()) {
        GammaRelease();
    }

    const OverlayLook look = ComposeOverlay(g_dimShown, g_tintShown);
    if (!look.visible) {
        OverlayHideAll();
        return;
    }
    if (g_overlays.empty()) {
        OverlaySync();
    }
    OverlayShow(look);
}

////////////////////////////////////////////////////////////////////////////////
// Dimming the pointer
//
// No window can cover the mouse pointer: Windows composites it above every
// window on the desktop. Gamma needs nothing here, because the pointer rides on
// the scanout its ramp is scaling. The overlay does, so for it every system
// cursor is swapped for a darkened copy of itself, and the originals are kept so
// the whole set can be put back.

struct SystemCursor {
    DWORD id;
    HICON original;
};

// The system cursor ids. The SDK calls these OCR_*; the numbers are the ones
// that reach SetSystemCursor, and they have never moved.
constexpr DWORD kSystemCursorIds[] = {
    32512,  // Arrow
    32513,  // I-beam
    32514,  // Busy
    32515,  // Cross
    32516,  // Up arrow
    32642,  // Resize, top left to bottom right
    32643,  // Resize, top right to bottom left
    32644,  // Resize, horizontal
    32645,  // Resize, vertical
    32646,  // Move
    32648,  // Unavailable
    32649,  // Link
    32650,  // Busy with arrow
    32651,  // Help
};

std::vector<SystemCursor> g_cursorSet;
int g_pointerLevel = -1;  // Level the cursor set shows; -1 when it is untouched.

// A cursor is a color bitmap carrying its own alpha, plus the old style mask
// for shapes that predate that. The color is darkened, the alpha is kept, and
// the mask is rewritten to agree with the alpha, so a drawing path that reads
// only the mask still shows the same shape.
HCURSOR DimCursor(HICON source, float factor) {
    ICONINFO sourceInfo{};
    if (!GetIconInfo(source, &sourceInfo)) {
        return nullptr;
    }

    BITMAP bitmap{};
    const bool sized = sourceInfo.hbmColor &&
                       GetObjectW(sourceInfo.hbmColor, sizeof(bitmap), &bitmap) &&
                       bitmap.bmWidth > 0 && bitmap.bmHeight > 0;
    if (!sized) {
        if (sourceInfo.hbmColor) {
            DeleteObject(sourceInfo.hbmColor);
        }
        if (sourceInfo.hbmMask) {
            DeleteObject(sourceInfo.hbmMask);
        }
        return nullptr;
    }

    const int width = bitmap.bmWidth;
    const int height = bitmap.bmHeight;
    const size_t count = static_cast<size_t>(width) * height;
    const size_t maskStride = (static_cast<size_t>(width) + 31) / 32 * 4;

    BITMAPINFO colorInfo{};
    colorInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    colorInfo.bmiHeader.biWidth = width;
    colorInfo.bmiHeader.biHeight = -height;  // Top down, so rows index directly.
    colorInfo.bmiHeader.biPlanes = 1;
    colorInfo.bmiHeader.biBitCount = 32;
    colorInfo.bmiHeader.biCompression = BI_RGB;

    // A one-bit DIB reads two palette entries, and BITMAPINFO only carries one.
    struct MonoInfo {
        BITMAPINFO info;
        RGBQUAD spare;
    } maskInfo{};
    maskInfo.info.bmiHeader = colorInfo.bmiHeader;
    maskInfo.info.bmiHeader.biBitCount = 1;

    std::vector<BYTE> color(count * 4, 0);
    std::vector<BYTE> mask(maskStride * height, 0);
    bool readColor = false;
    bool readMask = false;
    HDC screen = GetDC(nullptr);
    if (screen) {
        readColor =
            GetDIBits(screen, sourceInfo.hbmColor, 0, height, color.data(),
                      &colorInfo, DIB_RGB_COLORS) != 0;
        if (sourceInfo.hbmMask) {
            readMask = GetDIBits(screen, sourceInfo.hbmMask, 0, height,
                                 mask.data(), &maskInfo.info,
                                 DIB_RGB_COLORS) != 0;
        }
        ReleaseDC(nullptr, screen);
    }
    if (sourceInfo.hbmColor) {
        DeleteObject(sourceInfo.hbmColor);
    }
    if (sourceInfo.hbmMask) {
        DeleteObject(sourceInfo.hbmMask);
    }
    if (!readColor) {
        return nullptr;
    }

    // Do the color bits really carry alpha? Windows' own cursors do. An old
    // cursor leaves every alpha byte at zero and shapes itself with the mask,
    // which is only consulted in that case.
    bool alphaChannel = false;
    for (size_t i = 3; i < color.size(); i += 4) {
        if (color[i] != 0) {
            alphaChannel = true;
            break;
        }
    }

    const auto shade = [factor](BYTE value) {
        return static_cast<BYTE>(value * factor + 0.5f);
    };
    for (int y = 0; y < height; y++) {
        const BYTE* maskRow = mask.data() + maskStride * y;
        for (int x = 0; x < width; x++) {
            BYTE* pixel = color.data() + (static_cast<size_t>(y) * width + x) * 4;
            if (!alphaChannel) {
                // In the mask, a set bit is the screen showing through.
                const bool covered =
                    readMask && ((maskRow[x / 8] >> (7 - x % 8)) & 1) != 0;
                pixel[3] = covered ? 0 : 255;
            }
            pixel[0] = shade(pixel[0]);
            pixel[1] = shade(pixel[1]);
            pixel[2] = shade(pixel[2]);
        }
    }

    void* colorBits = nullptr;
    void* maskBits = nullptr;
    HBITMAP colorBitmap = CreateDIBSection(
        nullptr, &colorInfo, DIB_RGB_COLORS, &colorBits, nullptr, 0);
    HBITMAP maskBitmap = CreateDIBSection(
        nullptr, &maskInfo.info, DIB_RGB_COLORS, &maskBits, nullptr, 0);
    HICON result = nullptr;
    if (colorBitmap && colorBits && maskBitmap && maskBits) {
        std::memcpy(colorBits, color.data(), color.size());
        BYTE* maskOut = static_cast<BYTE*>(maskBits);
        std::memset(maskOut, 0, maskStride * height);
        for (int y = 0; y < height; y++) {
            BYTE* row = maskOut + maskStride * y;
            const BYTE* pixels =
                color.data() + static_cast<size_t>(y) * width * 4;
            for (int x = 0; x < width; x++) {
                if (pixels[x * 4 + 3] == 0) {
                    row[x / 8] |= static_cast<BYTE>(0x80 >> (x % 8));
                }
            }
        }
        ICONINFO output{};
        // A cursor, not an icon: the difference is what keeps the hotspot.
        output.fIcon = FALSE;
        output.xHotspot = sourceInfo.xHotspot;
        output.yHotspot = sourceInfo.yHotspot;
        output.hbmMask = maskBitmap;
        output.hbmColor = colorBitmap;
        result = CreateIconIndirect(&output);
    }
    if (colorBitmap) {
        DeleteObject(colorBitmap);
    }
    if (maskBitmap) {
        DeleteObject(maskBitmap);
    }
    return reinterpret_cast<HCURSOR>(result);
}

int PointerTarget() {
    if (!g_settings.dimPointer) {
        return 0;
    }
    // Gamma scales the whole scanout, pointer included, so only the overlay
    // needs the cursor set touched.
    if (g_settings.method == kMethodGamma && !g_gammaFailed) {
        return 0;
    }
    return static_cast<int>(lround(g_dimShown));
}

constexpr PCWSTR kPointerSwappedValue = L"PointerSwapped";

// Reloading the user's own scheme is what undoes a swap the mod never got to
// undo itself. It is also better than copying the saved pictures back: it
// brings back animated and multi-size cursors, and any scheme, size or colour
// change the user made while the dim was on.
void PointerReloadScheme() {
    SystemParametersInfoW(SPI_SETCURSORS, 0, nullptr, 0);
}

// A run that ended without its shutdown left the darkened cursors behind. The
// scheme is reloaded from the record that run left, before anything copies the
// cursors as its idea of the user's own.
void PointerRecoverStored() {
    if (StoreGetInt(kPointerSwappedValue, 0) == 0) {
        return;
    }
    Wh_Log(L"Putting the pointer back after a run that didn't finish");
    PointerReloadScheme();
    StoreDelete(kPointerSwappedValue);
}

void RestorePointer() {
    if (g_pointerLevel >= 0) {
        for (const SystemCursor& entry : g_cursorSet) {
            HICON copy = CopyIcon(entry.original);
            if (copy) {
                // SetSystemCursor owns the handle it is given, so it is given a
                // copy and the original is kept for the next round.
                SetSystemCursor(reinterpret_cast<HCURSOR>(copy), entry.id);
            }
        }
    }
    for (SystemCursor& entry : g_cursorSet) {
        if (entry.original) {
            DestroyIcon(entry.original);
        }
    }
    g_cursorSet.clear();
    const bool swapped = g_pointerLevel >= 0;
    g_pointerLevel = -1;
    if (swapped) {
        PointerReloadScheme();
        StoreDelete(kPointerSwappedValue);
    }
}

void SyncPointer(int level) {
    // A drag on a slider ends with one swap of the whole set. Rebuilding
    // fourteen cursors per mouse move costs more than the frame it rides on and
    // reads as a flicker, so the panel's own capture holds it off until the
    // release.
    if (g_panel.Hwnd() && GetCapture() == g_panel.Hwnd()) {
        return;
    }
    if (level == g_pointerLevel) {
        return;
    }
    if (level <= 0) {
        RestorePointer();
        return;
    }
    if (g_cursorSet.empty()) {
        // Taken while the pointer is still the user's own, so a second level
        // darkens the real cursors rather than the ones already darkened.
        for (const DWORD id : kSystemCursorIds) {
            HICON copy = CopyIcon(LoadCursorW(nullptr, MAKEINTRESOURCEW(id)));
            if (copy) {
                g_cursorSet.push_back({id, copy});
            }
        }
        if (g_cursorSet.empty()) {
            return;
        }
    }
    const float factor = 1.0f - Clamp01(level / 100.0f) * 0.96f;
    int replaced = 0;
    for (const SystemCursor& entry : g_cursorSet) {
        HICON dimmed = DimCursor(entry.original, factor);
        if (!dimmed) {
            continue;
        }
        // On success the system owns the handle; on failure it is still ours.
        if (SetSystemCursor(dimmed, entry.id)) {
            replaced++;
        } else {
            DestroyIcon(dimmed);
        }
    }
    const bool wasSwapped = g_pointerLevel >= 0;
    g_pointerLevel = replaced > 0 ? level : -1;
    if (replaced > 0 && !wasSwapped) {
        // Recorded so a run that never reaches its shutdown is undone by the
        // next one rather than leaving the pointer dark for the session.
        StoreSetInt(kPointerSwappedValue, 1);
    }
    if (replaced == 0) {
        Wh_Log(L"The cursor set refused to change");
    }
}

////////////////////////////////////////////////////////////////////////////////
// Levels, toggles and hotkeys

constexpr UINT_PTR kTimerAnim = 1;
constexpr UINT_PTR kTimerWatchdog = 2;

constexpr UINT kMessageSettings = WM_APP + 1;
constexpr UINT kMessageTray = WM_APP + 2;

constexpr int kHotkeyPanel = 1;
constexpr int kHotkeyDim = 2;
constexpr int kHotkeyTint = 3;
constexpr int kHotkeyDimUp = 4;
constexpr int kHotkeyDimDown = 5;
// One slot per id above, so a chord the system refused can be told apart from
// one it took.
constexpr int kHotkeyCount = 6;
bool g_hotkeyLive[kHotkeyCount] = {};

// Defined with the panel, but reached from the tray and the hotkeys.
void Repaint();
void StartAnimation();
void ToggleFlyout();
void OpenFlyout();
void CloseFlyout();
void SetTintOn(bool on);
void TintLevelChanged(int level);
void TrayRefreshTooltip();

void LoadRememberedLevels() {
    if (g_settings.rememberValues) {
        g_dimOn = StoreGetInt(L"DimOn", g_settings.startDimmed ? 1 : 0) != 0;
        g_dimLevel = StoreGetInt(L"DimLevel", g_settings.startLevel);
        g_tintOn =
            StoreGetInt(L"TintOn", g_settings.tintDefault > 0 ? 1 : 0) != 0;
        g_tintLevel = StoreGetInt(L"TintLevel", g_settings.tintDefault);
    } else {
        g_dimOn = g_settings.startDimmed;
        g_dimLevel = g_settings.startLevel;
        g_tintOn = g_settings.tintDefault > 0;
        g_tintLevel = g_settings.tintDefault;
    }
    g_dimLevel = ClampInt(g_dimLevel, 1, g_settings.maximum);
    g_tintLevel = ClampInt(g_tintLevel, 0, 60);
    g_dimShown = DimTarget();
    g_tintShown = TintTarget();
    g_stateDirty = false;
}

void SaveRememberedLevels() {
    if (!g_settings.rememberValues) {
        g_stateDirty = false;
        return;
    }
    StoreSetInt(L"DimOn", g_dimOn ? 1 : 0);
    StoreSetInt(L"DimLevel", g_dimLevel);
    StoreSetInt(L"TintOn", g_tintOn ? 1 : 0);
    StoreSetInt(L"TintLevel", g_tintLevel);
    g_stateDirty = false;
}

void SetDimOn(bool on) {
    if (g_dimOn == on) {
        return;
    }
    g_dimOn = on;
    if (!on || !g_settings.smooth) {
        // Switching off is immediate on purpose: someone who turns the dim off
        // wants the screen back now, not a gradual brighten. Switching on is
        // the one case the timer is left to, so the layer does not jump.
        g_dimShown = DimTarget();
        ApplyDim();
        SyncPointer(PointerTarget());
    }
    // The knob travels in both directions. Only the screen is allowed to snap,
    // so this must stay outside the branch above, or the knob is left sitting
    // on its old side after the light is switched back on.
    StartAnimation();
    g_stateDirty = true;
    SaveRememberedLevels();
    Repaint();
}

void SetDimLevel(int level, bool animate) {
    level = ClampInt(level, 1, g_settings.maximum);
    const bool wasOff = !g_dimOn;
    if (g_dimLevel == level && !wasOff) {
        return;
    }
    g_dimLevel = level;
    g_dimOn = true;
    if (animate && g_settings.smooth) {
        if (wasOff) {
            g_dimShown = 0.0f;
        }
        StartAnimation();
    } else {
        g_dimShown = DimTarget();
        ApplyDim();
        SyncPointer(PointerTarget());
    }
    g_stateDirty = true;
    Repaint();
}

void NudgeDim(int delta) {
    // A nudge while the dim is off switches it back on at the level it kept,
    // rather than jumping a whole step past a level that was never showing.
    if (!g_dimOn) {
        SetDimLevel(g_dimLevel, true);
        TrayRefreshTooltip();
        return;
    }
    SetDimLevel(g_dimLevel + delta, true);
    // Every path that nudges a level goes through here, so the tooltip is
    // refreshed in one place rather than at each of the callers.
    TrayRefreshTooltip();
}

void ToggleDim() {
    SetDimOn(!g_dimOn);
    TrayRefreshTooltip();
}

void ToggleTint() {
    SetTintOn(!g_tintOn);
    TrayRefreshTooltip();
}

void SetTintOn(bool on) {
    if (g_tintOn == on) {
        return;
    }
    g_tintOn = on;
    if (on && g_tintLevel <= 0) {
        g_tintLevel = g_settings.tintDefault > 0 ? g_settings.tintDefault : 20;
        g_tintLevel = ClampInt(g_tintLevel, 1, 60);
    }
    if (!on || !g_settings.smooth) {
        g_tintShown = TintTarget();
        ApplyDim();
    }
    // As with the dim knob, the knob has to travel whichever way it was thrown.
    StartAnimation();
    g_stateDirty = true;
    SaveRememberedLevels();
    Repaint();
}

void TintLevelChanged(int level) {
    level = ClampInt(level, 0, 60);
    if (g_tintLevel == level && g_tintOn) {
        return;
    }
    g_tintLevel = level;
    g_tintOn = level > 0;
    if (g_settings.smooth) {
        StartAnimation();
    } else {
        g_tintShown = TintTarget();
        ApplyDim();
    }
    g_stateDirty = true;
    Repaint();
}

bool HotkeyLive(int id) {
    return id > 0 && id < kHotkeyCount && g_hotkeyLive[id];
}

void RegisterHotkey(int id, const Hotkey& hotkey) {
    if (!g_messageWindow || !hotkey.Bound()) {
        return;
    }
    if (!RegisterHotKey(g_messageWindow, id, hotkey.mods | MOD_NOREPEAT,
                        hotkey.vk)) {
        Wh_Log(L"Couldn't register %s: %u", hotkey.label.c_str(),
               GetLastError());
        return;
    }
    g_hotkeyLive[id] = true;
}

void ApplyHotkeys() {
    if (!g_messageWindow) {
        return;
    }
    for (int id = 0; id < kHotkeyCount; id++) {
        g_hotkeyLive[id] = false;
    }
    for (int id = kHotkeyPanel; id <= kHotkeyDimDown; id++) {
        UnregisterHotKey(g_messageWindow, id);
    }
    RegisterHotkey(kHotkeyPanel, g_settings.panelHotkey);
    RegisterHotkey(kHotkeyDim, g_settings.toggleDimHotkey);
    RegisterHotkey(kHotkeyTint, g_settings.toggleTintHotkey);
    RegisterHotkey(kHotkeyDimUp, g_settings.dimUpHotkey);
    RegisterHotkey(kHotkeyDimDown, g_settings.dimDownHotkey);
}

////////////////////////////////////////////////////////////////////////////////
// Tray icon
//
// The panel has no taskbar button, so once it is closed the shortcut is the only
// way back. The notification-area icon is the other door: a click opens or
// closes the panel, a right click has every control the panel has.
//
// The icon belongs to the panel window rather than to the message window, which
// is message-only and so never sees the shell's "TaskbarCreated" broadcast - the
// one moment an icon has to be put back by hand, when Explorer restarts.

constexpr UINT kTrayIconId = 1;
constexpr UINT kTrayMenuPanel = 1;
constexpr UINT kTrayMenuDim = 2;
constexpr UINT kTrayMenuDimUp = 4;
constexpr UINT kTrayMenuDimDown = 5;
constexpr UINT kTrayMenuTint = 8;
constexpr UINT kTrayMenuAllOff = 9;

UINT g_taskbarCreated = 0;
bool g_trayAdded = false;
double g_trayIgnoreUntil = 0.0;
bool g_traySkipClick = false;
HICON g_trayIcon = nullptr;
std::wstring g_trayTip;

std::wstring TraySummary() {
    std::wstring text = L"Extra Dim";
    text += L"  \x00B7  ";
    text += g_dimOn ? Percent(g_dimLevel) : std::wstring(L"Off");
    if (g_tintOn) {
        text += L"  \x00B7  Tint ";
        text += Percent(g_tintLevel);
    }
    return text;
}

// Draws the icon rather than loading one, because a mod ships as a single file
// with no assets. It is painted four times too big and averaged down, which is
// what keeps the plate and the crescent smooth at sixteen pixels.
HICON TrayBuildIcon(int size) {
    if (size < 8) {
        size = 8;
    }
    const int scale = 4;
    const int big = size * scale;
    BITMAPINFO info{};
    info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    info.bmiHeader.biWidth = big;
    info.bmiHeader.biHeight = -big;  // Top row first, so y counts downwards.
    info.bmiHeader.biPlanes = 1;
    info.bmiHeader.biBitCount = 32;
    info.bmiHeader.biCompression = BI_RGB;

    HDC dc = CreateCompatibleDC(nullptr);
    if (!dc) {
        return nullptr;
    }
    void* canvasPixels = nullptr;
    HBITMAP canvas =
        CreateDIBSection(dc, &info, DIB_RGB_COLORS, &canvasPixels, nullptr, 0);
    if (!canvas || !canvasPixels) {
        if (canvas) {
            DeleteObject(canvas);
        }
        DeleteDC(dc);
        return nullptr;
    }
    HGDIOBJ oldBitmap = SelectObject(dc, canvas);
    RECT frame{0, 0, big, big};
    FillRect(dc, &frame, static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH)));
    const auto px = [&](double unit) {
        return static_cast<int>(lround(unit * big));
    };
    // The two colors carry the two facts the averaging needs: red says "inside
    // the plate", green says "inside the glyph", and black is outside both.
    // The plate keeps a pixel of air on every side, the way a shell icon does;
    // one that runs to the rim reads as a smudge in a 16 px cell.
    HBRUSH plate = CreateSolidBrush(RGB(255, 0, 0));
    HGDIOBJ oldBrush = SelectObject(dc, plate);
    HGDIOBJ oldPen = SelectObject(dc, GetStockObject(NULL_PEN));
    const double plateRadius = 0.43;
    Ellipse(dc, px(0.5 - plateRadius), px(0.5 - plateRadius),
            px(0.5 + plateRadius), px(0.5 + plateRadius));
    SelectObject(dc, oldBrush);
    DeleteObject(plate);
    SelectObject(dc, oldPen);

    // A crescent, the same mark the flyout carries: one circle, with a second
    // circle biting a slice out of it. It is filled rather than outlined,
    // because a one-pixel stroke is not a shape at sixteen pixels. The region
    // difference is moved onto the plate's centre by its own bounding box, so
    // what ends up centred is the crescent, not the circle it was cut from.
    const double moonRadius = 0.28;
    const double biteRadius = 0.315;
    const double biteShift = 0.245;
    HRGN disc =
        CreateEllipticRgn(px(0.5 - moonRadius), px(0.5 - moonRadius),
                          px(0.5 + moonRadius), px(0.5 + moonRadius));
    HRGN bite = CreateEllipticRgn(px(0.5 + biteShift - biteRadius),
                                  px(0.5 - biteRadius),
                                  px(0.5 + biteShift + biteRadius),
                                  px(0.5 + biteRadius));
    HBRUSH glyphBrush = CreateSolidBrush(RGB(255, 255, 255));
    if (disc && bite) {
        CombineRgn(disc, disc, bite, RGN_DIFF);
        RECT moon{};
        GetRgnBox(disc, &moon);
        const int shiftX = static_cast<int>(
            lround((big - (moon.right - moon.left)) * 0.5));
        const int shiftY = static_cast<int>(
            lround((big - (moon.bottom - moon.top)) * 0.5));
        OffsetRgn(disc, shiftX - moon.left, shiftY - moon.top);
        FillRgn(dc, disc, glyphBrush);
    }
    if (bite) {
        DeleteObject(bite);
    }
    if (disc) {
        DeleteObject(disc);
    }
    DeleteObject(glyphBrush);
    GdiFlush();  // The pixels below are read back, so the drawing has to be done.

    // Averaging the oversized picture down both antialiases the edges and turns
    // the two color channels back into a coverage and a tint.
    const uint8_t* source = static_cast<const uint8_t*>(canvasPixels);
    std::vector<uint32_t> art(static_cast<size_t>(size) * size, 0);
    const int samples = scale * scale;
    for (int y = 0; y < size; y++) {
        for (int x = 0; x < size; x++) {
            int inside = 0;
            int glyph = 0;
            for (int sy = 0; sy < scale; sy++) {
                const uint8_t* row =
                    source + (static_cast<size_t>(y * scale + sy) * big +
                              static_cast<size_t>(x) * scale) *
                                 4;
                for (int sx = 0; sx < scale; sx++) {
                    inside += row[sx * 4 + 2];  // Red: the plate.
                    glyph += row[sx * 4 + 1];   // Green: the glyph on it.
                }
            }
            if (inside == 0) {
                continue;  // Outside the plate, so the pixel stays clear.
            }
            const float alpha = static_cast<float>(inside) / (samples * 255.0f);
            const float white = static_cast<float>(glyph) / inside;
            // The color is left unmultiplied: a 32-bit icon hands the shell
            // its coverage in the alpha channel, and folding that into the
            // color as well would darken the rim of the plate.
            const auto channel = [&](float base) {
                return static_cast<uint32_t>(
                    Clamp01(base + (1.0f - base) * white) * 255.0f + 0.5f);
            };
            art[static_cast<size_t>(y) * size + x] =
                (static_cast<uint32_t>(alpha * 255.0f + 0.5f) << 24) |
                (channel(g_accent.r) << 16) | (channel(g_accent.g) << 8) |
                channel(g_accent.b);
        }
    }
    SelectObject(dc, oldBitmap);
    DeleteObject(canvas);

    // The pixels at the size the shell asks for, plus the mask an icon structure
    // wants even though a 32-bit icon draws from its own alpha.
    info.bmiHeader.biWidth = size;
    info.bmiHeader.biHeight = -size;
    void* iconPixels = nullptr;
    HBITMAP color =
        CreateDIBSection(dc, &info, DIB_RGB_COLORS, &iconPixels, nullptr, 0);
    if (color && iconPixels) {
        std::copy(art.begin(), art.end(), static_cast<uint32_t*>(iconPixels));
    }
    HDC maskDc = CreateCompatibleDC(nullptr);
    HBITMAP mask = CreateBitmap(size, size, 1, 1, nullptr);
    HICON icon = nullptr;
    if (maskDc && mask) {
        HGDIOBJ oldMask = SelectObject(maskDc, mask);
        RECT maskFrame{0, 0, size, size};
        FillRect(maskDc, &maskFrame,
                 static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH)));
        SelectObject(maskDc, oldMask);
        if (color) {
            ICONINFO parts{};
            parts.fIcon = TRUE;
            parts.hbmMask = mask;
            parts.hbmColor = color;
            icon = CreateIconIndirect(&parts);
        }
    }
    if (maskDc) {
        DeleteDC(maskDc);
    }
    if (mask) {
        DeleteObject(mask);
    }
    if (color) {
        DeleteObject(color);
    }
    DeleteDC(dc);
    return icon;
}

// The shell lives in another process, so a tooltip update is a round trip. A
// slider drag would otherwise queue one per mouse move, so the calls are spaced
// out and the last value is remembered until it is allowed through.
constexpr double kTooltipInterval = 120.0;
double g_trayTipStamp = -kTooltipInterval;
bool g_trayTipPending = false;

// Takes the icon away and lets go of the picture it was showing.
void TrayHalt() {
    if (g_panel.Hwnd()) {
        NOTIFYICONDATAW data{};
        data.cbSize = sizeof(data);
        data.hWnd = g_panel.Hwnd();
        data.uID = kTrayIconId;
        Shell_NotifyIconW(NIM_DELETE, &data);
    }
    g_trayAdded = false;
    g_trayTipPending = false;
    if (g_trayIcon) {
        DestroyIcon(g_trayIcon);
        g_trayIcon = nullptr;
    }
}

// Draws the icon and fills in the structure the shell is handed. The picture is
// kept between calls, so a redraw only happens when it was let go of.
bool TrayPrepare(NOTIFYICONDATAW& data) {
    if (!g_taskbarCreated) {
        g_taskbarCreated = RegisterWindowMessageW(L"TaskbarCreated");
        if (g_taskbarCreated) {
            // Explorer and the mod normally run at the same level, so nothing
            // is filtered and this call changes nothing. It matters when the
            // mod runs elevated, as a portable Windhawk started as admin does:
            // Explorer's broadcast would then be dropped before it got here.
            ChangeWindowMessageFilterEx(g_panel.Hwnd(), g_taskbarCreated,
                                        MSGFLT_ALLOW, nullptr);
        }
    }
    if (!g_trayIcon) {
        const int size = static_cast<int>(GetSystemMetrics(SM_CXSMICON));
        g_trayIcon = TrayBuildIcon(size > 0 ? size : 16);
    }
    if (!g_trayIcon) {
        Wh_Log(L"Couldn't draw the tray icon");
        return false;
    }
    data = NOTIFYICONDATAW();
    data.cbSize = sizeof(data);
    data.hWnd = g_panel.Hwnd();
    data.uID = kTrayIconId;
    data.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
    data.uCallbackMessage = kMessageTray;
    data.hIcon = g_trayIcon;
    g_trayTip = TraySummary();
    const size_t length =
        std::min<size_t>(g_trayTip.size(), ARRAYSIZE(data.szTip) - 1);
    std::copy(g_trayTip.begin(), g_trayTip.begin() + length, data.szTip);
    return true;
}

// Hands the icon to the shell: nothing to show takes it away, and an icon that
// is already up is modified rather than put up again, which is what keeps it
// where the user dropped it. False means the shell is not showing this picture.
bool TrayPublish() {
    if (!g_settings.showTrayIcon || !g_panel.Hwnd()) {
        TrayHalt();
        return false;
    }
    NOTIFYICONDATAW data{};
    if (!TrayPrepare(data)) {
        return false;
    }
    if (Shell_NotifyIconW(g_trayAdded ? NIM_MODIFY : NIM_ADD, &data)) {
        g_trayAdded = true;
        return true;
    }
    // A modify fails for the one reason a modify is wrong: the shell no longer
    // has the icon. Explorer restarted and its broadcast was filtered out, so
    // an add is tried once before the icon is given up on for the session.
    if (g_trayAdded && Shell_NotifyIconW(NIM_ADD, &data)) {
        g_trayAdded = true;
        return true;
    }
    // Explorer may not be up yet; its "TaskbarCreated" broadcast adds the icon
    // once it is.
    g_trayAdded = false;
    Wh_Log(L"Couldn't put the tray icon up: %u", GetLastError());
    return false;
}

// Puts the icon up, unless it is already there. Called on the panel thread,
// because that is the thread the icon reports its clicks to.
void TrayAdd() {
    if (g_trayAdded || !g_settings.showTrayIcon || !g_panel.Hwnd()) {
        return;
    }
    TrayPublish();
}

// Follows the settings: the icon appears or goes, and it is redrawn so it wears
// the new accent color.
void TrayRefresh() {
    if (!g_settings.showTrayIcon) {
        TrayHalt();
        return;
    }
    // The shell copies the picture as it takes it, so the one it was showing is
    // released once the new one is in place - and only then. A picture the shell
    // never accepted is destroyed here, because the icon on screen may still be
    // drawing the old one.
    HICON previous = g_trayIcon;
    g_trayIcon = nullptr;
    const bool published = TrayPublish();
    if (published && g_trayIcon) {
        if (previous) {
            DestroyIcon(previous);
        }
        return;
    }
    // The redraw failed, so the picture the shell already has is kept rather
    // than being released and leaving the icon bare. The replacement the shell
    // turned down was never handed over, so it is released here instead.
    if (g_trayIcon) {
        DestroyIcon(g_trayIcon);
    }
    g_trayIcon = previous;
}

// A level changed, so the tooltip is replaced but the icon is not redrawn.
void TrayRefreshTooltip() {
    if (!g_settings.showTrayIcon || !g_panel.Hwnd()) {
        return;
    }
    if (g_trayAdded) {
        const double now = NowMs();
        if (now - g_trayTipStamp < kTooltipInterval) {
            g_trayTipPending = true;
            return;
        }
        g_trayTipStamp = now;
    }
    g_trayTipPending = false;
    NOTIFYICONDATAW data{};
    if (!TrayPrepare(data)) {
        return;
    }
    data.uFlags = NIF_TIP;
    if (!g_trayAdded) {
        TrayPublish();
        return;
    }
    Shell_NotifyIconW(NIM_MODIFY, &data);
}

// Delivers the update the interval held back, if there was one.
void TrayFlushTooltip() {
    if (!g_trayTipPending) {
        return;
    }
    g_trayTipStamp = -kTooltipInterval;
    TrayRefreshTooltip();
}

// The menu behind a right click: the panel itself, then the controls, so every
// level can be reached without opening the panel first.
void TrayShowMenu() {
    HWND owner = g_panel.Hwnd();
    HMENU menu = CreatePopupMenu();
    if (!owner || !menu) {
        if (menu) {
            DestroyMenu(menu);
        }
        return;
    }
    const bool open = g_phase == kPhaseOpen || g_phase == kPhaseOpening;
    AppendMenuW(menu, MF_STRING, kTrayMenuPanel,
                open ? L"Close the controls" : L"Open the controls");
    AppendMenuW(menu, MF_SEPARATOR, 0, nullptr);
    {
        std::wstring label = L"Extra dim";
        label += L"\t";
        label += g_dimOn ? Percent(g_dimLevel) : std::wstring(L"Off");
        AppendMenuW(menu, MF_STRING | (g_dimOn ? MF_CHECKED : 0), kTrayMenuDim,
                    label.c_str());
    }
    {
        std::wstring label = L"Warm tint";
        label += L"\t";
        label += g_tintOn ? Percent(g_tintLevel) : std::wstring(L"Off");
        AppendMenuW(menu, MF_STRING | (g_tintOn ? MF_CHECKED : 0),
                    kTrayMenuTint, label.c_str());
    }
    AppendMenuW(menu, MF_SEPARATOR, 0, nullptr);
    AppendMenuW(menu, MF_STRING, kTrayMenuDimUp, L"Dim more");
    AppendMenuW(menu, MF_STRING, kTrayMenuDimDown, L"Dim less");
    AppendMenuW(menu, MF_SEPARATOR, 0, nullptr);
    AppendMenuW(menu, MF_STRING, kTrayMenuAllOff, L"Turn everything off");

    POINT at{};
    GetCursorPos(&at);
    // A menu opened from the notification area needs its owner in front, or it
    // stays up after the user has clicked somewhere else.
    SetForegroundWindow(owner);
    const int command = TrackPopupMenu(
        menu, TPM_RIGHTBUTTON | TPM_RETURNCMD | TPM_NONOTIFY, at.x, at.y, 0,
        owner, nullptr);
    DestroyMenu(menu);
    PostMessageW(owner, WM_NULL, 0, 0);

    switch (command) {
        case kTrayMenuPanel:
            ToggleFlyout();
            break;
        case kTrayMenuDim:
            ToggleDim();
            break;
        case kTrayMenuTint:
            SetTintOn(!g_tintOn);
            TrayRefreshTooltip();
            break;
        case kTrayMenuDimUp:
            NudgeDim(g_settings.step);
            break;
        case kTrayMenuDimDown:
            NudgeDim(-g_settings.step);
            break;
        case kTrayMenuAllOff:
            SetDimOn(false);
            SetTintOn(false);
            Repaint();
            TrayRefreshTooltip();
            break;
        default:
            break;
    }
}

void TrayOnMessage(LPARAM event) {
    switch (LOWORD(event)) {
        case WM_LBUTTONUP:
            if (g_traySkipClick) {
                g_traySkipClick = false;  // The tail of a double click.
                return;
            }
            if (NowMs() < g_trayIgnoreUntil) {
                // This is the click that took the focus away a moment ago, so
                // it has already done its work on the panel.
                return;
            }
            ToggleFlyout();
            return;
        case WM_LBUTTONDBLCLK:
            g_traySkipClick = true;
            return;
        case WM_RBUTTONUP:
            TrayShowMenu();
            return;
        default:
            break;
    }
}

////////////////////////////////////////////////////////////////////////////////
// Panel layout
//
// Everything below is written in logical units and multiplied by g_uiScale, so
// the panel keeps its proportions on a 100%, 125% or 150% display and still
// grows or shrinks with the scale setting.

enum HitId {
    kHitNone = 0,
    kHitClose,
    kHitDimToggle,
    kHitDimSlider,
    kHitTintToggle,
    kHitTintSlider,
    kHitDisplaysAll,
    kHitDisplaysPrimary,
    kHitMethodOverlay,
    kHitMethodGamma,
};

struct HitResult {
    int id = kHitNone;
    int index = 0;
};

HitResult g_hover;
int g_dragId = 0;
bool g_trackingMouse = false;

struct CardLayout {
    RECT rect{};
    RECT icon{};
    RECT title{};
    RECT toggle{};
    RECT value{};
    RECT slider{};
    RECT status{};
};

struct PanelLayout {
    float width = 400.0f;
    float height = 470.0f;
    RECT icon{};
    RECT title{};
    RECT subtitle{};
    RECT closeButton{};
    CardLayout dim;
    CardLayout tint;
    CardLayout displays;
    RECT rowOneLabel{};
    RECT rowOneChips[2]{};
    RECT rowTwoLabel{};
    RECT rowTwoChips[2]{};
    RECT displaysStatus{};
    RECT footer{};
};

PanelLayout g_layout;
Surface g_surface;
Surface g_shadowSurface;

// The toggle knobs, the panel itself and the dim level all glide on the same
// timer, so a change never half-animates.
float g_toggleAnim[2] = {0.0f, 0.0f};

inline float S(float value) {
    return value * g_uiScale;
}

inline LONG LPx(float value) {
    return static_cast<LONG>(lround(value * g_uiScale));
}

inline RECT RectL(float left, float top, float width, float height) {
    RECT rect{LPx(left), LPx(top), LPx(left + width), LPx(top + height)};
    return rect;
}

inline bool InRect(const RECT& rect, POINT point) {
    return point.x >= rect.left && point.x < rect.right &&
           point.y >= rect.top && point.y < rect.bottom;
}

void BuildCard(CardLayout& card,
               float left,
               float top,
               float width,
               float height,
               bool withStatus) {
    card.rect = RectL(left, top, width, height);
    card.icon = RectL(left + 16, top + 14, 20, 20);
    card.title = RectL(left + 44, top + 12, width - 44 - 16 - 52, 24);
    card.toggle = RectL(left + width - 16 - 44, top + 12, 44, 24);
    // The readout shares the slider's row, the way the volume flyout puts its
    // percentage beside the track, instead of hanging under the toggle.
    const float sliderWidth = width - 32.0f - 54.0f;
    card.slider = RectL(left + 16, top + 51, sliderWidth, 26);
    card.value = RectL(left + 16 + sliderWidth + 10, top + 51, 44, 26);
    // The trailing line is optional: a card is two rows tall without one.
    card.status = withStatus ? RectL(left + 16, top + 88, width - 32, 18)
                             : RECT{};
}

void BuildLayout() {
    PanelLayout& l = g_layout;
    const float width = 400.0f;
    const float pad = 16.0f;
    l.width = width;

    l.icon = RectL(pad, 16, 26, 26);
    l.title = RectL(pad + 32, 8, width - pad * 2 - 32 - 36, 22);
    l.subtitle = RectL(pad + 32, 30, width - pad * 2 - 32 - 36, 18);
    l.closeButton = RectL(width - pad - 30, 13, 30, 30);

    const float cardLeft = 12.0f;
    const float cardWidth = width - 24.0f;
    float top = 60.0f;
    BuildCard(l.dim, cardLeft, top, cardWidth, 92.0f, false);
    top += 92.0f + 8.0f;
    BuildCard(l.tint, cardLeft, top, cardWidth, 112.0f, true);
    top += 112.0f + 8.0f;

    l.displays.rect = RectL(cardLeft, top, cardWidth, 132.0f);
    l.displays.icon = RectL(cardLeft + 16, top + 14, 20, 20);
    l.displays.title = RectL(cardLeft + 44, top + 12, cardWidth - 60, 24);
    l.rowOneLabel = RectL(cardLeft + 16, top + 44, 130, 26);
    l.rowTwoLabel = RectL(cardLeft + 16, top + 76, 130, 26);
    // The trailing line says what the chosen method actually does, and says so
    // plainly when the driver has overruled it.
    l.displaysStatus = RectL(cardLeft + 16, top + 106, cardWidth - 32, 18);
    const float displayChip = 92.0f;
    const float displayGap = 8.0f;
    const float displayRight = cardLeft + cardWidth - 16.0f;
    l.rowOneChips[1] = RectL(displayRight - displayChip, top + 44, displayChip, 26);
    l.rowOneChips[0] =
        RectL(displayRight - displayChip * 2 - displayGap, top + 44,
              displayChip, 26);
    l.rowTwoChips[1] = RectL(displayRight - displayChip, top + 76, displayChip, 26);
    l.rowTwoChips[0] =
        RectL(displayRight - displayChip * 2 - displayGap, top + 76,
              displayChip, 26);

    top += 132.0f;
    // The keyboard hint sits in a band of its own, with the same air above it as
    // the panel leaves below.
    l.footer = RectL(pad, top + 10, width - pad * 2, 20);
    l.height = top + 40.0f;
}

////////////////////////////////////////////////////////////////////////////////
// Painting

void DrawTextBlock(Surface& s,
                   const RECT& rect,
                   const std::wstring& text,
                   int px,
                   int weight,
                   Color color,
                   UINT flags) {
    HFONT font = g_fonts.Get(ScaledPx(static_cast<float>(px)),
                             weight);
    g_text.Draw(s, font, text, rect, flags | DT_SINGLELINE, color);
}

std::wstring Ellipsized(const std::wstring& text, int px, int weight,
                        const RECT& rect) {
    HFONT font = g_fonts.Get(ScaledPx(static_cast<float>(px)), weight);
    return g_text.Ellipsize(font, text, rect.right - rect.left);
}

void DrawCardShell(Surface& s, const RECT& rect, bool highlighted) {
    const float radius = S(8.0f);
    FillRoundRect(s, static_cast<float>(rect.left), static_cast<float>(rect.top),
                  static_cast<float>(rect.right),
                  static_cast<float>(rect.bottom), radius,
                  highlighted ? g_palette.cardHover : g_palette.card);
    StrokeRoundRect(
        s, static_cast<float>(rect.left), static_cast<float>(rect.top),
        static_cast<float>(rect.right), static_cast<float>(rect.bottom), radius,
        std::max(1.0f, S(1.0f)), g_palette.border);
}

// The switch is drawn at the shell's own 40x20 inside a larger pointer target,
// so it is easy to hit and still the size Windows uses. The thumb is grey while
// the switch is off and fades to white as it comes on, which is how the system
// switch reads at a glance.
void DrawToggle(Surface& s, const RECT& rect, float t, bool hovered) {
    const float boxWidth = std::min(static_cast<float>(rect.right - rect.left),
                                    S(40.0f));
    const float height = std::min(static_cast<float>(rect.bottom - rect.top),
                                  S(20.0f));
    const float centerX = (rect.left + rect.right) * 0.5f;
    const float centerY = (rect.top + rect.bottom) * 0.5f;
    const float left = centerX - boxWidth * 0.5f;
    const float right = centerX + boxWidth * 0.5f;
    const float top = centerY - height * 0.5f;
    const float bottom = centerY + height * 0.5f;
    const float radius = height * 0.5f;
    const float knobRadius = radius - S(3.0f);
    t = Clamp01(t);

    const Color offTrack = g_darkTheme ? Rgb(255, 255, 255, 0.15f)
                                      : Rgb(0, 0, 0, 0.12f);
    Color track = Mix(offTrack, WithAlpha(g_accent, 0.94f), t);
    if (hovered) {
        track = Mix(track, WithAlpha(g_accent, 1.0f), 0.18f);
    }
    FillRoundRect(s, left, top, right, bottom, radius, track);
    const Color ring = Mix(g_palette.borderStrong, WithAlpha(g_accent, 0.0f), t);
    if (ring.a > 0.01f) {
        StrokeRoundRect(s, left, top, right, bottom, radius,
                        std::max(1.0f, S(1.0f)), ring);
    }

    const float travel = (right - left) - height;
    const float cx = left + radius + travel * t;
    const float cy = (top + bottom) * 0.5f;
    const Color offKnob = g_darkTheme ? Rgb(198, 198, 198) : Rgb(96, 96, 96);
    FillCircle(s, cx, cy, knobRadius,
               Mix(offKnob, g_palette.knob, t));
}

// A slider that belongs to a switched-off feature keeps its position but drops
// the accent, so the track can never claim a level the readout has just called
// off.
void DrawSlider(Surface& s,
                const RECT& rect,
                float value01,
                bool enabled,
                bool hovered,
                bool active) {
    const float cy = (rect.top + rect.bottom) * 0.5f;
    const float knobRadius = S(8.0f);
    const float left = rect.left + knobRadius;
    const float right = rect.right - knobRadius;
    const float trackHeight = S(hovered || active ? 5.0f : 4.0f);
    const float x = left + (right - left) * Clamp01(value01);
    const Color fill =
        enabled ? WithAlpha(g_accent, 0.95f) : g_palette.borderStrong;

    FillRoundRect(s, left, cy - trackHeight * 0.5f, right,
                  cy + trackHeight * 0.5f, trackHeight * 0.5f, g_palette.track);
    if (x > left + 0.5f) {
        FillRoundRect(s, left, cy - trackHeight * 0.5f, x,
                      cy + trackHeight * 0.5f, trackHeight * 0.5f, fill);
    }
    if (hovered || active) {
        FillCircle(s, x, cy, knobRadius + S(6.0f),
                   WithAlpha(enabled ? g_accent : g_palette.knob, 0.16f));
    }
    // A white thumb on a hairline edge, which is what the shell's own sliders
    // use; an accent ring turns the control into something else.
    FillCircle(s, x, cy, knobRadius, g_palette.knob);
    StrokeCircle(s, x, cy, knobRadius - S(0.5f), std::max(1.0f, S(1.0f)),
                 Rgb(0, 0, 0, g_darkTheme ? 0.28f : 0.20f));
}

void DrawChip(Surface& s,
              const RECT& rect,
              const std::wstring& label,
              bool selected,
              bool hovered) {
    const float left = static_cast<float>(rect.left);
    const float top = static_cast<float>(rect.top);
    const float right = static_cast<float>(rect.right);
    const float bottom = static_cast<float>(rect.bottom);
    const float radius = S(4.0f);
    Color background = selected
                           ? WithAlpha(g_accent, g_darkTheme ? 0.22f : 0.16f)
                           : (hovered ? g_palette.chipHover : g_palette.chip);
    FillRoundRect(s, left, top, right, bottom, radius, background);
    if (selected) {
        StrokeRoundRect(s, left, top, right, bottom, radius,
                        std::max(1.0f, S(1.0f)), WithAlpha(g_accent, 0.6f));
    }
    const Color textColor =
        selected ? (g_darkTheme ? Mix(g_accent, kWhite, 0.35f) : g_accent)
                 : g_palette.textSecondary;
    DrawTextBlock(s, rect, Ellipsized(label, 11, FW_NORMAL, rect), 11,
                  selected ? FW_SEMIBOLD : FW_NORMAL, textColor,
                  DT_CENTER | DT_VCENTER);
}

std::wstring HeaderSubtitle() {
    std::wstring parts;
    if (g_dimOn) {
        parts += L"Dimmed " + Percent(g_dimLevel);
    } else {
        parts += L"Not dimmed";
    }
    if (g_tintOn) {
        parts += L"  \x00B7  Tint ";
        parts += Percent(g_tintLevel);
    }
    return parts;
}

// What the chosen method really does, under the two rows that choose it. When
// the driver has refused the ramp the line says so, because the Gamma chip is
// then describing a method that is not the one dimming the screen.
PCWSTR DisplaysStatusText() {
    if (g_settings.method == kMethodGamma) {
        return g_gammaFailed ? L"Gamma refused. The overlay is dimming instead."
                             : L"Lowers the display ramp. Reaches fullscreen.";
    }
    return L"Drawn over the desktop. Not in fullscreen games.";
}

// The footer is a keyboard hint rather than a sentence: one cap per key, the way
// the shell spells a chord, with the action after it. Groups are dropped from
// the end when the row runs out of room, so a cap is never clipped in half.
void DrawHotkeyFooter(Surface& s, const RECT& area) {
    constexpr int kMaxGroups = 3;
    struct Group {
        std::wstring caps[5];
        int capCount = 0;
        std::wstring action;
    };
    Group groups[kMaxGroups];
    int groupCount = 0;

    // Only chords the system actually took are advertised: one that another
    // program already owns would draw a hint that does nothing.
    const auto add = [&](const Hotkey& hotkey, int id, PCWSTR action) {
        if (!hotkey.Bound() || !HotkeyLive(id) || groupCount >= kMaxGroups) {
            return;
        }
        Group& group = groups[groupCount++];
        if (hotkey.mods & MOD_CONTROL) {
            group.caps[group.capCount++] = L"Ctrl";
        }
        if (hotkey.mods & MOD_ALT) {
            group.caps[group.capCount++] = L"Alt";
        }
        if (hotkey.mods & MOD_SHIFT) {
            group.caps[group.capCount++] = L"Shift";
        }
        if (hotkey.mods & MOD_WIN) {
            group.caps[group.capCount++] = L"Win";
        }
        group.caps[group.capCount++] =
            hotkey.keyLabel.empty() ? hotkey.label : hotkey.keyLabel;
        group.action = action;
    };
    // The chord that opens the panel leads, because it is the one that gets the
    // panel back when the row has to lose something.
    add(g_settings.panelHotkey, kHotkeyPanel, L"panel");
    add(g_settings.toggleDimHotkey, kHotkeyDim, L"dim");
    add(g_settings.toggleTintHotkey, kHotkeyTint, L"tint");
    if (groupCount == 0) {
        return;
    }

    HFONT capFont = g_fonts.Get(ScaledPx(9.0f), FW_SEMIBOLD);
    HFONT actionFont = g_fonts.Get(ScaledPx(10.0f), FW_NORMAL);
    const auto textWidth = [](HFONT font, const std::wstring& text) {
        return static_cast<float>(
            g_text.Width(font, text.c_str(), static_cast<int>(text.size())));
    };
    const float capPad = S(4.0f);
    const float capGap = S(2.0f);
    const float actionGap = S(5.0f);
    const float groupGap = S(12.0f);
    const float capHeight = S(17.0f);

    float groupWidth[kMaxGroups] = {};
    for (int i = 0; i < groupCount; i++) {
        float width = 0.0f;
        for (int c = 0; c < groups[i].capCount; c++) {
            width += textWidth(capFont, groups[i].caps[c]) + capPad * 2.0f +
                     capGap;
        }
        groupWidth[i] =
            width + actionGap + textWidth(actionFont, groups[i].action);
    }

    const float available = static_cast<float>(area.right - area.left);
    float total = 0.0f;
    int shown = 0;
    while (shown < groupCount) {
        const float next =
            total + groupWidth[shown] + (shown > 0 ? groupGap : 0.0f);
        if (next > available) {
            break;
        }
        total = next;
        shown++;
    }
    if (shown == 0) {
        return;
    }

    const float centreY = (area.top + area.bottom) * 0.5f;
    const LONG boxTop = static_cast<LONG>(lround(centreY - capHeight * 0.5f));
    const LONG boxBottom =
        static_cast<LONG>(lround(centreY + capHeight * 0.5f));
    float x = static_cast<float>(area.left) + (available - total) * 0.5f;
    for (int i = 0; i < shown; i++) {
        for (int c = 0; c < groups[i].capCount; c++) {
            const std::wstring& text = groups[i].caps[c];
            const float width = textWidth(capFont, text) + capPad * 2.0f;
            const RECT cap{static_cast<LONG>(lround(x)), boxTop,
                           static_cast<LONG>(lround(x + width)), boxBottom};
            FillRoundRect(s, static_cast<float>(cap.left),
                          static_cast<float>(cap.top),
                          static_cast<float>(cap.right),
                          static_cast<float>(cap.bottom), S(4.0f),
                          g_palette.chip);
            StrokeRoundRect(s, static_cast<float>(cap.left),
                            static_cast<float>(cap.top),
                            static_cast<float>(cap.right),
                            static_cast<float>(cap.bottom), S(4.0f),
                            std::max(1.0f, S(1.0f)), g_palette.border);
            DrawTextBlock(s, cap, text, 9, FW_SEMIBOLD, g_palette.textSecondary,
                          DT_CENTER | DT_VCENTER);
            x += width + capGap;
        }
        x += actionGap - capGap;
        const float actionWidth = textWidth(actionFont, groups[i].action);
        const RECT action{static_cast<LONG>(lround(x)), boxTop,
                          static_cast<LONG>(lround(x + actionWidth + S(2.0f))),
                          boxBottom};
        DrawTextBlock(s, action, groups[i].action, 10, FW_NORMAL,
                      g_palette.textTertiary, DT_LEFT | DT_VCENTER);
        x += actionWidth + groupGap;
    }
}

// A card heading is an accent glyph and a line of semibold text. Windows 11
// never puts a plate behind it, and neither do we: the symbol carries the
// color, the card carries the surface.
void DrawSectionTitle(Surface& s,
                      CardLayout& card,
                      PCWSTR glyph,
                      const std::wstring& title) {
    DrawGlyph(s, glyph, (card.icon.left + card.icon.right) * 0.5f,
              (card.icon.top + card.icon.bottom) * 0.5f, S(16.0f), g_accent);
    DrawTextBlock(s, card.title, title, 13, FW_SEMIBOLD, g_palette.text,
                  DT_LEFT | DT_VCENTER);
}

void Render() {
    BuildLayout();
    const int width = LPx(g_layout.width);
    const int height = LPx(g_layout.height);
    if (!g_surface.Resize(width, height)) {
        return;
    }
    g_panel.MarkContentDirty();
    Surface& s = g_surface;
    const Palette& p = g_palette;
    const float radius = S(8.0f);

    // The surface itself: a translucent plate with a hairline and a slightly
    // lighter top edge, which is what the shell's own flyouts have.
    FillRoundRectGradient(s, 0.5f, 0.5f, width - 0.5f, height - 0.5f, radius,
                          p.surfaceTop, p.surfaceBottom);
    StrokeRoundRect(s, 0.5f, 0.5f, width - 0.5f, height - 0.5f, radius,
                    std::max(1.0f, S(1.0f)), p.border);
    FillRoundRect(s, radius, 0.6f, width - radius, S(1.2f), S(0.6f),
                  Rgb(255, 255, 255, g_darkTheme ? 0.05f : 0.35f));

    // Header. A bare accent glyph next to the title, which is how a Settings
    // page heads itself; no disc, no badge, and nothing extra near the close
    // button. The subtitle already reports the state in words.
    DrawGlyph(s, kGlyphMoon, (g_layout.icon.left + g_layout.icon.right) * 0.5f,
              (g_layout.icon.top + g_layout.icon.bottom) * 0.5f, S(20.0f),
              g_accent);
    DrawTextBlock(s, g_layout.title, L"Extra Dim", 14, FW_SEMIBOLD, p.text,
                  DT_LEFT | DT_VCENTER);
    DrawTextBlock(s, g_layout.subtitle,
                  Ellipsized(HeaderSubtitle(), 11, FW_NORMAL, g_layout.subtitle),
                  11, FW_NORMAL, p.textSecondary, DT_LEFT | DT_VCENTER);

    const bool closeHovered = g_hover.id == kHitClose;
    if (closeHovered) {
        FillRoundRect(s, static_cast<float>(g_layout.closeButton.left),
                      static_cast<float>(g_layout.closeButton.top),
                      static_cast<float>(g_layout.closeButton.right),
                      static_cast<float>(g_layout.closeButton.bottom), S(6.0f),
                      p.chipHover);
    }
    DrawGlyph(s, kGlyphClose,
              (g_layout.closeButton.left + g_layout.closeButton.right) * 0.5f,
              (g_layout.closeButton.top + g_layout.closeButton.bottom) * 0.5f,
              S(11.0f), closeHovered ? p.text : p.textSecondary);

    // Extra dim.
    DrawCardShell(s, g_layout.dim.rect, false);
    DrawSectionTitle(s, g_layout.dim, kGlyphMoon, L"Extra dim");
    DrawToggle(s, g_layout.dim.toggle, g_toggleAnim[0],
               g_hover.id == kHitDimToggle);
    DrawSlider(s, g_layout.dim.slider,
               g_dimLevel / static_cast<float>(std::max(1, g_settings.maximum)),
               g_dimOn, g_hover.id == kHitDimSlider,
               g_dragId == kHitDimSlider || g_hover.id == kHitDimSlider);
    DrawTextBlock(s, g_layout.dim.value,
                  g_dimOn ? Percent(g_dimLevel) : std::wstring(L"Off"), 12,
                  FW_SEMIBOLD, g_dimOn ? p.text : p.textTertiary,
                  DT_RIGHT | DT_VCENTER);

    // Warm tint.
    DrawCardShell(s, g_layout.tint.rect, false);
    DrawSectionTitle(s, g_layout.tint, kGlyphPalette, L"Warm tint");
    DrawToggle(s, g_layout.tint.toggle, g_toggleAnim[1],
               g_hover.id == kHitTintToggle);
    DrawSlider(s, g_layout.tint.slider, g_tintLevel / 60.0f, g_tintOn,
               g_hover.id == kHitTintSlider,
               g_dragId == kHitTintSlider || g_hover.id == kHitTintSlider);
    // The tint readout keeps showing its strength with the switch off: the
    // number is what the slider will apply the moment it is thrown, and "Off"
    // here only repeated what the switch beside the title already says.
    DrawTextBlock(s, g_layout.tint.value, Percent(g_tintLevel), 12,
                  FW_SEMIBOLD, g_tintOn ? p.text : p.textTertiary,
                  DT_RIGHT | DT_VCENTER);
    DrawTextBlock(s, g_layout.tint.status,
                  L"A warm layer for the evening", 11,
                  FW_NORMAL, p.textTertiary, DT_LEFT | DT_VCENTER);

    // Displays.
    DrawCardShell(s, g_layout.displays.rect, false);
    DrawSectionTitle(s, g_layout.displays, kGlyphMonitor, L"Displays");
    DrawTextBlock(s, g_layout.rowOneLabel, L"Dim displays", 11, FW_NORMAL,
                  p.textSecondary, DT_LEFT | DT_VCENTER);
    DrawTextBlock(s, g_layout.rowTwoLabel, L"Method", 11, FW_NORMAL,
                  p.textSecondary, DT_LEFT | DT_VCENTER);
    DrawChip(s, g_layout.rowOneChips[0], L"All", g_settings.allDisplays,
             g_hover.id == kHitDisplaysAll);
    DrawChip(s, g_layout.rowOneChips[1], L"Primary", !g_settings.allDisplays,
             g_hover.id == kHitDisplaysPrimary);
    DrawChip(s, g_layout.rowTwoChips[0], L"Overlay",
             g_settings.method == kMethodOverlay,
             g_hover.id == kHitMethodOverlay);
    DrawChip(s, g_layout.rowTwoChips[1], L"Gamma",
             g_settings.method == kMethodGamma,
             g_hover.id == kHitMethodGamma);
    DrawTextBlock(
        s, g_layout.displaysStatus,
        Ellipsized(DisplaysStatusText(), 11, FW_NORMAL, g_layout.displaysStatus),
        11, FW_NORMAL,
        g_gammaFailed && g_settings.method == kMethodGamma ? p.textSecondary
                                                           : p.textTertiary,
        DT_LEFT | DT_VCENTER);

    // Footer.
    DrawHotkeyFooter(s, g_layout.footer);
}

////////////////////////////////////////////////////////////////////////////////
// Interaction
//
// The panel is a plain layered window, so a hit is resolved against the very
// rectangles the painter used. Nothing is measured twice, and nothing can drift
// out of step with what the pointer is over.

bool g_animRunning = false;
bool g_paintPending = false;
int g_shadowMargin = 0;

int SliderValue(const RECT& rect, int x, int maximum) {
    const float knob = S(8.0f);
    const float left = static_cast<float>(rect.left) + knob;
    const float right = static_cast<float>(rect.right) - knob;
    if (right <= left || maximum <= 0) {
        return 0;
    }
    const float t = Clamp01((static_cast<float>(x) - left) / (right - left));
    return ClampInt(static_cast<int>(lround(t * maximum)), 0, maximum);
}

// The grab area of a slider is taller than the painted track, which is what
// makes a four-pixel rail comfortable with a mouse.
bool HitsSlider(const RECT& rect, POINT point) {
    RECT hit = rect;
    InflateRect(&hit, 0, LPx(6.0f));
    return InRect(hit, point);
}

HitResult HitTest(POINT point) {
    HitResult result;
    if (InRect(g_layout.closeButton, point)) {
        result.id = kHitClose;
        return result;
    }
    if (InRect(g_layout.dim.rect, point)) {
        if (InRect(g_layout.dim.toggle, point)) {
            result.id = kHitDimToggle;
            return result;
        }
        if (HitsSlider(g_layout.dim.slider, point)) {
            result.id = kHitDimSlider;
            return result;
        }
    }
    if (InRect(g_layout.tint.rect, point)) {
        if (InRect(g_layout.tint.toggle, point)) {
            result.id = kHitTintToggle;
            return result;
        }
        if (HitsSlider(g_layout.tint.slider, point)) {
            result.id = kHitTintSlider;
            return result;
        }
    }
    if (InRect(g_layout.displays.rect, point)) {
        if (InRect(g_layout.rowOneChips[0], point)) {
            result.id = kHitDisplaysAll;
            return result;
        }
        if (InRect(g_layout.rowOneChips[1], point)) {
            result.id = kHitDisplaysPrimary;
            return result;
        }
        if (InRect(g_layout.rowTwoChips[0], point)) {
            result.id = kHitMethodOverlay;
            return result;
        }
        if (InRect(g_layout.rowTwoChips[1], point)) {
            result.id = kHitMethodGamma;
            return result;
        }
    }
    return result;
}

// Switching the displays or the method from the panel is a preference of its
// own, so it is stored - beside the Windhawk setting value it replaced, so that
// changing that setting later takes the panel's choice back out of the picture.
void SetAllDisplays(bool all) {
    if (g_settings.allDisplays == all) {
        return;
    }
    if (!g_settings.rememberValues || all == g_settings.settingAllDisplays) {
        // Picking the value the setting already has is not an override, so the
        // record goes instead of pinning the choice the panel was moved away
        // from: the setting answers for it again, here and after a restart.
        StoreDelete(L"AllDisplays");
        StoreDelete(L"AllDisplaysBase");
    } else {
        StoreSetInt(L"AllDisplaysBase", g_settings.settingAllDisplays ? 1 : 0);
        StoreSetInt(L"AllDisplays", all ? 1 : 0);
    }
    g_settings.allDisplays = all;
    g_topologyDirty = true;
    OverlayDestroyAll();
    GammaRelease();
    ApplyDim();
    Repaint();
}

void SetDimMethod(int method) {
    if (g_settings.method == method) {
        return;
    }
    if (!g_settings.rememberValues || method == g_settings.settingMethod) {
        // Back on the setting's own value, so the record would only be a stale
        // override pinning the method the panel was moved away from.
        StoreDelete(L"Method");
        StoreDelete(L"MethodBase");
    } else {
        StoreSetInt(L"MethodBase", g_settings.settingMethod);
        StoreSetInt(L"Method", method);
    }
    g_settings.method = method;
    // A driver that refused a ramp once is asked again now that the user has
    // said, explicitly, that this is the method they want.
    g_gammaFailed = false;
    GammaRelease();
    OverlayHideAll();
    ApplyDim();
    Repaint();
}

void HandlePress(HitResult hit, POINT point) {
    switch (hit.id) {
        case kHitClose:
            CloseFlyout();
            return;
        case kHitDimToggle:
            SetDimOn(!g_dimOn);
            TrayRefreshTooltip();
            return;
        case kHitDimSlider:
            g_dragId = kHitDimSlider;
            SetDimLevel(SliderValue(g_layout.dim.slider, point.x,
                                    g_settings.maximum),
                        false);
            return;
        case kHitTintToggle:
            SetTintOn(!g_tintOn);
            TrayRefreshTooltip();
            return;
        case kHitTintSlider:
            g_dragId = kHitTintSlider;
            TintLevelChanged(SliderValue(g_layout.tint.slider, point.x, 60));
            return;
        case kHitDisplaysAll:
            SetAllDisplays(true);
            return;
        case kHitDisplaysPrimary:
            SetAllDisplays(false);
            return;
        case kHitMethodOverlay:
            SetDimMethod(kMethodOverlay);
            return;
        case kHitMethodGamma:
            SetDimMethod(kMethodGamma);
            return;
        default:
            return;
    }
}

void HandleDrag(POINT point) {
    switch (g_dragId) {
        case kHitDimSlider:
            SetDimLevel(SliderValue(g_layout.dim.slider, point.x,
                                    g_settings.maximum),
                        false);
            TrayRefreshTooltip();
            return;
        case kHitTintSlider:
            TintLevelChanged(SliderValue(g_layout.tint.slider, point.x, 60));
            TrayRefreshTooltip();
            return;
        default:
            return;
    }
}

void OnMouseMove(POINT point, HWND hwnd) {
    if (g_dragId != 0) {
        HandleDrag(point);
        return;
    }
    const HitResult hit = HitTest(point);
    if (hit.id != g_hover.id || hit.index != g_hover.index) {
        g_hover = hit;
        Repaint();
    }
    if (!g_trackingMouse) {
        TRACKMOUSEEVENT track{};
        track.cbSize = sizeof(track);
        track.dwFlags = TME_LEAVE;
        track.hwndTrack = hwnd;
        TrackMouseEvent(&track);
        g_trackingMouse = true;
    }
}

void OnWheel(int delta, HWND hwnd) {
    if (delta == 0) {
        return;
    }
    POINT point{};
    GetCursorPos(&point);
    ScreenToClient(hwnd, &point);
    const int step = delta > 0 ? g_settings.step : -g_settings.step;

    if (InRect(g_layout.tint.rect, point)) {
        TintLevelChanged(g_tintLevel + step);
        TrayRefreshTooltip();
        return;
    }
    // Anywhere else on the panel the wheel moves the dim level, which is the
    // control the panel exists for and the one the hotkeys also drive.
    NudgeDim(step);
}

////////////////////////////////////////////////////////////////////////////////
// Placement
//
// The panel opens on the display under the pointer, so the shortcut works the
// same on a laptop screen and on the monitor beside it. The nine presets keep
// the distance from the work area edge, never from the screen edge, so the
// taskbar is never covered.

RECT PanelWorkArea() {
    POINT cursor{};
    GetCursorPos(&cursor);
    HMONITOR monitor = MonitorFromPoint(cursor, MONITOR_DEFAULTTOPRIMARY);
    if (g_settings.primaryMonitor) {
        POINT origin{0, 0};
        monitor = MonitorFromPoint(origin, MONITOR_DEFAULTTOPRIMARY);
    }
    MONITORINFO info{};
    info.cbSize = sizeof(info);
    if (!monitor || !GetMonitorInfoW(monitor, &info)) {
        RECT fallback{0, 0, GetSystemMetrics(SM_CXSCREEN),
                      GetSystemMetrics(SM_CYSCREEN)};
        return fallback;
    }
    return info.rcWork;
}

UINT DpiForWorkArea() {
    POINT cursor{};
    GetCursorPos(&cursor);
    HMONITOR monitor = MonitorFromPoint(cursor, MONITOR_DEFAULTTOPRIMARY);
    if (g_settings.primaryMonitor) {
        POINT origin{0, 0};
        monitor = MonitorFromPoint(origin, MONITOR_DEFAULTTOPRIMARY);
    }
    using GetDpiForMonitor_t = HRESULT(WINAPI*)(HMONITOR, int, UINT*, UINT*);
    static auto getDpi =
        GetProc<GetDpiForMonitor_t>(L"shcore.dll", "GetDpiForMonitor");
    if (getDpi && monitor) {
        UINT x = 96;
        UINT y = 96;
        if (SUCCEEDED(getDpi(monitor, 0 /* MDT_EFFECTIVE_DPI */, &x, &y)) &&
            x >= 48) {
            return x;
        }
    }
    HDC dc = GetDC(nullptr);
    if (dc) {
        const int dpi = GetDeviceCaps(dc, LOGPIXELSX);
        ReleaseDC(nullptr, dc);
        if (dpi >= 48) {
            return static_cast<UINT>(dpi);
        }
    }
    return 96;
}

void ComputePanelPosition(int width, int height, int& x, int& y) {
    const RECT area = PanelWorkArea();
    const int margin = static_cast<int>(lround(S(14.0f)));
    const int left = area.left + margin;
    const int right = area.right - width - margin;
    const int top = area.top + margin;
    const int bottom = area.bottom - height - margin;
    const int middleX = area.left + ((area.right - area.left) - width) / 2;
    const int middleY = area.top + ((area.bottom - area.top) - height) / 2;

    const std::wstring position = Lower(g_settings.position);
    if (position == L"left" || position == L"centerleft") {
        x = left;
        y = middleY;
    } else if (position == L"center") {
        x = middleX;
        y = middleY;
    } else if (position == L"top") {
        x = middleX;
        y = top;
    } else if (position == L"bottom") {
        x = middleX;
        y = bottom;
    } else if (position == L"topleft") {
        x = left;
        y = top;
    } else if (position == L"topright") {
        x = right;
        y = top;
    } else if (position == L"bottomleft") {
        x = left;
        y = bottom;
    } else if (position == L"bottomright") {
        x = right;
        y = bottom;
    } else {  // "right", the default
        x = right;
        y = middleY;
    }

    x += static_cast<int>(lround(g_settings.offsetX * g_uiScale));
    y += static_cast<int>(lround(g_settings.offsetY * g_uiScale));
    // A stale offset must not park the panel where it cannot be reached.
    x = ClampInt(x, area.left - width / 3, area.right - width * 2 / 3);
    y = ClampInt(y, area.top - height / 3, area.bottom - height * 2 / 3);
}

// Where the plate sits, decided once when the frame is laid out. Reading the
// pointer on every frame would both cost an enumeration per frame and let the
// panel drift if the mouse moved while it was sliding in.
int g_panelX = 0;
int g_panelY = 0;

void AnchorPanel() {
    if (!g_surface.Valid()) {
        return;
    }
    ComputePanelPosition(g_surface.Width(), g_surface.Height(), g_panelX,
                         g_panelY);
}

////////////////////////////////////////////////////////////////////////////////
// Presentation
//
// One surface is painted, uploaded, and that is the whole frame. The shadow is
// its own layered window so the panel can stay a clean, rounded plate instead of
// carrying a transparent ring of blur around itself.

void BuildShadowSurface(int width, int height) {
    const int margin = static_cast<int>(lround(S(24.0f)));
    g_shadowMargin = margin;
    const int w = width + margin * 2;
    const int h = height + margin * 2 + static_cast<int>(lround(S(8.0f)));
    if (!g_shadowSurface.Resize(w, h)) {
        return;
    }
    g_shadow.MarkContentDirty();
    const float left = static_cast<float>(margin);
    const float top = static_cast<float>(margin);
    const float right = left + width;
    const float bottom = top + height;
    const Color shadow = g_palette.shadow;
    // A tight contact shadow and a soft one: the pair reads as depth without
    // the washed-out grey a single wide blur leaves behind.
    DrawShadow(g_shadowSurface, left, top + S(2.0f), right, bottom + S(3.0f),
               S(12.0f), S(4.0f), WithAlpha(shadow, shadow.a * 1.2f));
    DrawShadow(g_shadowSurface, left, top + S(6.0f), right, bottom + S(10.0f),
               S(12.0f), S(12.0f), WithAlpha(shadow, shadow.a * 0.55f));
}

void PresentPanel() {
    if (!g_panel.Hwnd() || !g_surface.Valid()) {
        return;
    }
    const double eased = EaseOutCubic(g_openAnim);
    const int slide = static_cast<int>(lround((1.0 - eased) * S(12.0f)));
    const BYTE alpha =
        static_cast<BYTE>(Clamp01(static_cast<float>(eased)) * 255.0f + 0.5f);

    const int x = g_panelX;
    const int y = g_panelY + slide;

    g_panel.Present(g_surface, x, y, alpha);
    if (g_shadowSurface.Valid()) {
        // The shadow is layered directly under the plate in z-order, which is
        // set once when the panel opens. Uploading a frame does not reorder
        // anything, so there is nothing to put back here.
        g_shadow.Present(g_shadowSurface, x - g_shadowMargin,
                         y - g_shadowMargin, alpha);
    }
}

// Marks the frame stale and lets the animation timer do the painting. A drag
// reports moves far faster than a screen refreshes, so painting each one would
// spend a whole surface render per mouse report; this way a burst costs one
// frame per tick, and the last move is always the one that gets drawn.
void Repaint() {
    if (g_phase == kPhaseClosed) {
        return;
    }
    g_paintPending = true;
    if (!g_panel.Hwnd() || g_animRunning) {
        return;
    }
    g_animTick = NowMs();
    g_animRunning = SetTimer(g_panel.Hwnd(), kTimerAnim, 15, nullptr) != 0;
}

// Paints the surface and puts it up. The timer is the only caller.
void PaintFrame() {
    // The surface holds the frame, so a changed level has to be painted again
    // before it is uploaded. Presenting without this step re-sends the frame
    // that was drawn the last time the pixels changed.
    Render();
    PresentPanel();
}

void PaintPanelSurface() {
    // The surface is rebuilt at the current scale and theme each time, because
    // both can change while the panel is up. The scale is the display's own DPI
    // times the panel scale, so the flyout is the same physical size on a 100%
    // and on a 150% screen.
    g_uiScale = static_cast<float>(DpiForWorkArea()) / 96.0f *
                (static_cast<float>(ClampInt(g_settings.panelScale, 80, 140)) /
                 100.0f);
    Render();
    if (g_surface.Valid()) {
        BuildShadowSurface(g_surface.Width(), g_surface.Height());
        AnchorPanel();
    }
}

// Rebuilds the frame at the current scale, theme and work area, and puts it up.
// This is the one entry point for anything that invalidates the whole surface,
// so the frame is painted once rather than once per step.
void RefreshFrame() {
    // Nothing is on screen, so there is nothing to refresh: the next open paints
    // a fresh frame anyway, and painting one for a hidden panel only costs the
    // theme change its few milliseconds.
    if (g_phase == kPhaseClosed) {
        return;
    }
    PaintPanelSurface();
    PresentPanel();
    // A new scale or a new keep-bright choice can change which side of the layer
    // the plate belongs on.
    ArrangeZOrder();
}

////////////////////////////////////////////////////////////////////////////////
// Animation
//
// Everything that moves is driven by one timer on the panel window, so a toggle
// never lands half way and the panel never closes before its own fade is done.

void StartAnimation() {
    if (!g_panel.Hwnd()) {
        g_dimShown = DimTarget();
        g_tintShown = TintTarget();
        ApplyDim();
        SyncPointer(PointerTarget());
        return;
    }
    g_animTick = NowMs();
    if (g_animRunning) {
        return;
    }
    g_animRunning = true;
    SetTimer(g_panel.Hwnd(), kTimerAnim, 15, nullptr);
}

void FinishClosing() {
    g_panel.Hide();
    g_shadow.Hide();
    g_phase = kPhaseClosed;
    g_paintPending = false;
    g_openAnim = 0.0f;
    g_hover = HitResult();
    g_dragId = 0;
    if (g_stateDirty) {
        SaveRememberedLevels();
    }
}

void UpdateAnimation() {
    const double now = NowMs();
    double dt = (now - g_animTick) / 1000.0;
    g_animTick = now;
    if (dt <= 0.0) {
        dt = 0.016;
    } else if (dt > 0.1) {
        dt = 0.1;
    }

    const float previousDim = g_dimShown;
    const float previousTint = g_tintShown;
    const float previousToggle[2] = {g_toggleAnim[0], g_toggleAnim[1]};
    bool moving = false;
    auto approach = [&](float& value, float target, float rate) {
        const float k = static_cast<float>(1.0 - exp(-rate * dt));
        if (fabsf(target - value) <= 0.0015f) {
            value = target;
            return;
        }
        value += (target - value) * k;
        moving = true;
    };

    approach(g_dimShown, DimTarget(), 13.0f);
    approach(g_tintShown, TintTarget(), 13.0f);
    approach(g_toggleAnim[0], g_dimOn ? 1.0f : 0.0f, 18.0f);
    approach(g_toggleAnim[1], g_tintOn ? 1.0f : 0.0f, 18.0f);

    if (g_phase == kPhaseOpening || g_phase == kPhaseOpen) {
        approach(g_openAnim, 1.0f, 20.0f);
        if (g_phase == kPhaseOpening && g_openAnim >= 0.999f) {
            g_phase = kPhaseOpen;
        }
    } else if (g_phase == kPhaseClosing) {
        approach(g_openAnim, 0.0f, 22.0f);
    }

    // The dim layer is only touched when the level actually moved, so opening
    // the panel does not re-position every display's layer sixty times a second.
    if (g_dimShown != previousDim || g_tintShown != previousTint) {
        ApplyDim();
    }
    if (g_phase != kPhaseClosed) {
        // The toggle knobs live in the pixels, so one that is still gliding
        // needs a fresh surface. The slide and the fade only move the window,
        // which is what PresentPanel does on its own.
        if (g_toggleAnim[0] != previousToggle[0] ||
            g_toggleAnim[1] != previousToggle[1]) {
            g_paintPending = true;
        }
        if (g_paintPending) {
            g_paintPending = false;
            PaintFrame();
        } else {
            PresentPanel();
        }
    }
    if (g_phase == kPhaseClosing && g_openAnim <= 0.0015f) {
        // The panel is gone, but a level may still be gliding: the timer has to
        // outlive the panel or the screen would stop half way to the level the
        // readout was showing. So moving is left alone here, and the check at
        // the end of the tick decides whether there is anything left to do.
        FinishClosing();
    }
    // Every swapped cursor is a fresh bitmap the system has to pick up, so the
    // pointer follows the level once it has settled, not on every frame.
    if (!moving) {
        SyncPointer(PointerTarget());
    }
    if (!moving && g_panel.Hwnd()) {
        KillTimer(g_panel.Hwnd(), kTimerAnim);
        g_animRunning = false;
        // A last frame at rest, so the panel is never left mid-slide.
        if (g_phase == kPhaseOpen) {
            PresentPanel();
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
// Opening and closing

void OpenFlyout() {
    if (!g_panel.Hwnd()) {
        return;
    }
    // The panel is painted before it is shown so the first frame is already
    // correct; a window that appears empty for one frame is what makes a flyout
    // feel unpolished.
    PaintPanelSurface();
    g_phase = kPhaseOpening;
    g_panel.Show(true);
    g_shadow.Show(false);
    // Shown in this order the shadow ends up above the plate, so the band is
    // arranged once, here, rather than on every frame of the slide.
    ArrangeZOrder();
    StartAnimation();
    PresentPanel();
    TrayRefreshTooltip();
}

void CloseFlyout() {
    if (g_phase == kPhaseClosed || g_phase == kPhaseClosing) {
        return;
    }
    if (g_stateDirty) {
        SaveRememberedLevels();
    }
    g_phase = kPhaseClosing;
    StartAnimation();
}

void ToggleFlyout() {
    if (g_phase == kPhaseClosed || g_phase == kPhaseClosing) {
        OpenFlyout();
    } else {
        CloseFlyout();
    }
}

void WatchdogTick() {
    // Nothing in Windows promises that a topmost window stays above another one
    // forever. Re-asserting both is what keeps the dim layer from being
    // swallowed by a fullscreen-ish window, and keeps the panel on whichever
    // side of the layer the settings chose for it.
    if (g_overlayVisible || g_phase != kPhaseClosed) {
        ArrangeZOrder();
    }
    // A hotkey can change a level without the panel ever being opened, so the
    // state is written here too rather than waiting for the mod to unload.
    if (g_stateDirty) {
        SaveRememberedLevels();
    }
    TrayFlushTooltip();
}

void OnHotkey(int id) {
    switch (id) {
        case kHotkeyPanel:
            ToggleFlyout();
            return;
        case kHotkeyDim:
            ToggleDim();
            return;
        case kHotkeyTint:
            ToggleTint();
            return;
        case kHotkeyDimUp:
            NudgeDim(g_settings.step);
            return;
        case kHotkeyDimDown:
            NudgeDim(-g_settings.step);
            return;
        default:
            return;
    }
}

////////////////////////////////////////////////////////////////////////////////
// Window procedures

void ApplySettings();

// The panel is a single window, so the pointer has to come from the hit rather
// than from a control. Anything the pointer can act on gets the hand, which is
// what makes the toggles read as clickable at all.
HCURSOR CursorForHit(int id) {
    // Read every time rather than cached: pointer dimming swaps the system
    // cursors out, and a cached handle would keep showing the old one.
    return LoadCursorW(nullptr, id == kHitNone ? IDC_ARROW : IDC_HAND);
}

LRESULT CALLBACK PanelWndProc(HWND hwnd,
                              UINT message,
                              WPARAM wParam,
                              LPARAM lParam) {
    // Explorer restarted, so it has forgotten every icon in the tray.
    if (g_taskbarCreated && message == g_taskbarCreated) {
        TrayPublish();
        return 0;
    }

    switch (message) {
        case kMessageTray:
            TrayOnMessage(lParam);
            return 0;
        case WM_TIMER:
            switch (wParam) {
                case kTimerAnim:
                    UpdateAnimation();
                    return 0;
                case kTimerWatchdog:
                    WatchdogTick();
                    return 0;
                default:
                    return 0;
            }
        case WM_MOUSEMOVE: {
            POINT point{GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
            OnMouseMove(point, hwnd);
            return 0;
        }
        case WM_MOUSELEAVE:
            g_trackingMouse = false;
            // While a slider is being dragged the pointer may well leave the
            // plate, and clearing the hover then would drop the highlight the
            // drag is drawn with.
            if (g_dragId == 0 && g_hover.id != kHitNone) {
                g_hover = HitResult();
                Repaint();
            }
            return 0;
        case WM_LBUTTONDOWN: {
            POINT point{GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
            const HitResult hit = HitTest(point);
            if (hit.id == kHitNone) {
                return 0;
            }
            SetCapture(hwnd);
            SetFocus(hwnd);
            g_hover = hit;
            HandlePress(hit, point);
            Repaint();
            return 0;
        }
        case WM_LBUTTONUP:
            if (GetCapture() == hwnd) {
                ReleaseCapture();
            }
            if (g_dragId != 0) {
                g_dragId = 0;
                SaveRememberedLevels();
                Repaint();
            }
            // The drag held the cursor set still; it follows the level it was
            // dragged to now that it is done.
            SyncPointer(PointerTarget());
            TrayFlushTooltip();
            return 0;
        case WM_CAPTURECHANGED:
            g_dragId = 0;
            SyncPointer(PointerTarget());
            return 0;
        case WM_MOUSEWHEEL:
            OnWheel(GET_WHEEL_DELTA_WPARAM(wParam), hwnd);
            return 0;
        case WM_KEYDOWN:
            if (wParam == VK_ESCAPE) {
                CloseFlyout();
                return 0;
            }
            if (wParam == VK_UP || wParam == VK_RIGHT) {
                NudgeDim(g_settings.step);
                return 0;
            }
            if (wParam == VK_DOWN || wParam == VK_LEFT) {
                NudgeDim(-g_settings.step);
                return 0;
            }
            break;
        case WM_ACTIVATE:
            if (LOWORD(wParam) == WA_INACTIVE && g_settings.closeOnFocusLoss &&
                g_phase != kPhaseClosed && g_phase != kPhaseClosing) {
                // The click that took the focus away is about to arrive at
                // whatever was clicked, the tray icon included. Remember the
                // moment so that same click does not open the panel again.
                g_trayIgnoreUntil = NowMs() + 300.0;
                CloseFlyout();
            }
            // The default handler is what hands the keyboard to the window that
            // was just activated. Returning here leaves Esc and the arrow keys
            // going to whatever had the focus before the panel opened.
            break;
        case WM_QUERYENDSESSION:
            return TRUE;
        case WM_ENDSESSION:
            // The session is ending and the process is about to be torn down,
            // so the screen, the pointer and the levels are handed back now
            // rather than left to a shutdown that will not run.
            if (wParam) {
                SaveRememberedLevels();
                RestorePointer();
                GammaRelease();
            }
            return 0;
        case WM_DISPLAYCHANGE:
            // The monitors, their resolution or the primary display moved, so
            // the layers are rebuilt and the plate is measured again. In gamma
            // mode the ramp a resolution change resets is taken again and put
            // back, which is why the topology flag is read by both methods.
            g_topologyDirty = true;
            ApplyDim();
            if (g_phase != kPhaseClosed) {
                RefreshFrame();
            }
            return 0;
        case WM_POWERBROADCAST:
            // A display that slept comes back with the ramp the driver boots
            // with, so the saved copy is taken again and the dim is put back.
            if (wParam == PBT_APMRESUMEAUTOMATIC ||
                wParam == PBT_APMRESUMESUSPEND || wParam == PBT_APMRESUMECRITICAL) {
                GammaRelease();
                ApplyDim();
            }
            return TRUE;
        case WM_DPICHANGED:
            RefreshFrame();
            return 0;
        case WM_SETTINGCHANGE:
            // The work area moves when the taskbar does, and the shell's theme
            // changes the palette the panel is painted with.
            if (wParam == SPI_SETWORKAREA) {
                AnchorPanel();
                Repaint();
            } else if (wParam == SPI_SETDESKWALLPAPER || wParam == 0) {
                ResolvePalette();
                // The tray icon is drawn in the accent colour, so it is redrawn
                // too rather than keeping the previous accent until the next
                // settings change.
                TrayRefresh();
                RefreshFrame();
            }
            return 0;
        case WM_ERASEBKGND:
            return 1;
        case WM_PAINT: {
            // The frame lives in the layered surface, so a paint is only ever
            // the shell asking; nothing is drawn here.
            PAINTSTRUCT paint;
            BeginPaint(hwnd, &paint);
            EndPaint(hwnd, &paint);
            return 0;
        }
        case WM_SETCURSOR:
            if (LOWORD(lParam) == HTCLIENT) {
                POINT point{};
                GetCursorPos(&point);
                ScreenToClient(hwnd, &point);
                SetCursor(CursorForHit(HitTest(point).id));
                return TRUE;
            }
            break;
        case WM_NCHITTEST:
            return HTCLIENT;
        case WM_CLOSE:
            CloseFlyout();
            return 0;
        default:
            break;
    }
    return DefWindowProcW(hwnd, message, wParam, lParam);
}

LRESULT CALLBACK ShadowWndProc(HWND hwnd,
                               UINT message,
                               WPARAM wParam,
                               LPARAM lParam) {
    switch (message) {
        case WM_NCHITTEST:
            return HTTRANSPARENT;
        case WM_ERASEBKGND:
            return 1;
        case WM_MOUSEACTIVATE:
            return MA_NOACTIVATE;
        default:
            break;
    }
    return DefWindowProcW(hwnd, message, wParam, lParam);
}

LRESULT CALLBACK MessageWndProc(HWND hwnd,
                                UINT message,
                                WPARAM wParam,
                                LPARAM lParam) {
    switch (message) {
        case kMessageSettings:
            ApplySettings();
            return 0;
        case WM_HOTKEY:
            OnHotkey(static_cast<int>(wParam));
            return 0;
        case WM_CLOSE:
            DestroyWindow(hwnd);
            return 0;
        case WM_DESTROY:
            g_messageWindow = nullptr;
            PostQuitMessage(0);
            return 0;
        default:
            break;
    }
    return DefWindowProcW(hwnd, message, wParam, lParam);
}

////////////////////////////////////////////////////////////////////////////////
// Window lifetime

bool RegisterClassHelper(WNDPROC proc, PCWSTR name) {
    WNDCLASSEXW windowClass{};
    windowClass.cbSize = sizeof(windowClass);
    windowClass.style = 0;
    windowClass.lpfnWndProc = proc;
    windowClass.hInstance = g_instance;
    windowClass.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    windowClass.hbrBackground = nullptr;
    windowClass.lpszClassName = name;
    if (!RegisterClassExW(&windowClass)) {
        // A class that is already registered belongs to another instance in
        // this process; a tool mod runs alone, so it is a failure, not a
        // situation to carry on from.
        Wh_Log(L"RegisterClassExW(%s) failed: %u", name, GetLastError());
        return false;
    }
    return true;
}

bool RegisterWindowClasses() {
    // Every class the mod owns is registered up front: the dim layer class is
    // needed long before the panel is on screen.
    return RegisterClassHelper(PanelWndProc, kPanelClass) &&
           RegisterClassHelper(ShadowWndProc, kShadowClass) &&
           RegisterClassHelper(OverlayWndProc, kOverlayClass) &&
           RegisterClassHelper(MessageWndProc, kMessageClass);
}

void UnregisterWindowClasses() {
    UnregisterClassW(kPanelClass, g_instance);
    UnregisterClassW(kShadowClass, g_instance);
    UnregisterClassW(kOverlayClass, g_instance);
    UnregisterClassW(kMessageClass, g_instance);
}

bool CreateWindows() {
    // The panel accepts the focus: a flyout that cannot take a key press is a
    // flyout the user has to chase with the mouse.
    if (!g_panel.Create(0, kPanelClass, L"Extra Dim", nullptr)) {
        Wh_Log(L"Couldn't create the panel: %u", GetLastError());
        return false;
    }
    if (!g_shadow.Create(WS_EX_TRANSPARENT | WS_EX_NOACTIVATE, kShadowClass,
                         L"", nullptr)) {
        Wh_Log(L"Couldn't create the shadow: %u", GetLastError());
        return false;
    }
    // A message-only window is the one thing that can receive a hotkey or a
    // stop request without ever appearing on screen.
    g_messageWindow =
        CreateWindowExW(0, kMessageClass, L"", 0, 0, 0, 0, 0, HWND_MESSAGE,
                        nullptr, g_instance, nullptr);
    if (!g_messageWindow) {
        Wh_Log(L"Couldn't create the message window: %u", GetLastError());
        return false;
    }
    SetTimer(g_panel.Hwnd(), kTimerWatchdog, 2000, nullptr);
    return true;
}

void DestroyWindows() {
    // The screen and the pointer are handed back first, because the tray icon is
    // a cross-process call that can stall against a busy Explorer, and a
    // shutdown that stalls must not leave a ramp or darkened cursors applied.
    RestorePointer();
    GammaRelease();
    TrayHalt();
    OverlayDestroyAll();
    if (g_overlayBrush) {
        DeleteObject(g_overlayBrush);
        g_overlayBrush = nullptr;
    }
    if (g_panel.Hwnd()) {
        KillTimer(g_panel.Hwnd(), kTimerAnim);
        KillTimer(g_panel.Hwnd(), kTimerWatchdog);
    }
    g_surface.Reset();
    g_shadowSurface.Reset();
    g_panel.Destroy();
    g_shadow.Destroy();
    if (g_messageWindow) {
        DestroyWindow(g_messageWindow);
        g_messageWindow = nullptr;
    }
}

////////////////////////////////////////////////////////////////////////////////
// Settings
//
// The Windhawk settings say what the mod should look like and where it starts.
// Nothing in here is written back: the only two choices the panel changes for
// itself - the displays and the dim method - go to the mod's own storage, so the
// settings file is never rewritten behind Windhawk's back.

void ApplySettings() {
    g_settings = LoadSettings();
    // What Windhawk says is kept apart from what the panel last chose, so a
    // change made in Windhawk can be told from a change made in the panel.
    g_settings.settingAllDisplays = g_settings.allDisplays;
    g_settings.settingMethod = g_settings.method;

    if (g_settings.rememberValues) {
        // A panel choice only answers for the setting value it replaced. Once
        // that setting reads differently, the choice is stale and is dropped
        // rather than winning over the setting for good.
        const int storedAllDisplays =
            StoreGetInt(L"AllDisplaysBase", -1) ==
                    (g_settings.settingAllDisplays ? 1 : 0)
                ? StoreGetInt(L"AllDisplays", -1)
                : -1;
        if (storedAllDisplays >= 0) {
            g_settings.allDisplays = storedAllDisplays != 0;
        }
        const int storedMethod =
            StoreGetInt(L"MethodBase", -1) == g_settings.settingMethod
                ? StoreGetInt(L"Method", -1)
                : -1;
        if (storedMethod == kMethodGamma || storedMethod == kMethodOverlay) {
            g_settings.method = storedMethod;
        }
    } else {
        // Nothing is remembered, so nothing an earlier run wrote is left to be
        // found again if the switch is turned back on later.
        StoreDelete(L"Method");
        StoreDelete(L"MethodBase");
        StoreDelete(L"AllDisplays");
        StoreDelete(L"AllDisplaysBase");
        StoreDelete(L"DimOn");
        StoreDelete(L"DimLevel");
        StoreDelete(L"TintOn");
        StoreDelete(L"TintLevel");
    }

    ResolvePalette();
    g_dimLevel = ClampInt(g_dimLevel, 1, g_settings.maximum);
    g_tintLevel = ClampInt(g_tintLevel, 0, 60);
    // A new ceiling, a new theme or a new scale can all invalidate the surface
    // that is on screen, so the frame is rebuilt rather than merely repainted.
    g_dimShown = DimTarget();
    g_tintShown = TintTarget();
    g_topologyDirty = true;
    g_gammaFailed = false;

    g_toggleAnim[0] = g_dimOn ? 1.0f : 0.0f;
    g_toggleAnim[1] = g_tintOn ? 1.0f : 0.0f;

    ApplyHotkeys();
    // The capture exclusion and the display choice belong to the layer windows
    // themselves, so a settings change builds them again rather than editing
    // them in place.
    OverlayDestroyAll();
    GammaRelease();
    ApplyDim();
    // The pointer's part of a settings change lands at once, rather than when
    // some level happens to settle next.
    SyncPointer(PointerTarget());
    if (g_panel.Hwnd()) {
        TrayRefresh();
        if (g_phase != kPhaseClosed) {
            RefreshFrame();
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
// The worker thread
//
// The mod lives on one thread with its own message loop. Every window, the tray
// icon, the hotkeys, the layered surfaces and the registry writes belong to it,
// so there is nothing to synchronise and nothing to race.

DWORD WINAPI WorkerThreadProc(LPVOID) {
    const HRESULT comInit = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);

    if (!RegisterWindowClasses()) {
        if (SUCCEEDED(comInit)) {
            CoUninitialize();
        }
        if (g_workerReadyEvent) {
            SetEvent(g_workerReadyEvent);
        }
        return 1;
    }

    g_fonts.Init();

    if (!CreateWindows()) {
        DestroyWindows();
        UnregisterWindowClasses();
        if (SUCCEEDED(comInit)) {
            CoUninitialize();
        }
        if (g_workerReadyEvent) {
            SetEvent(g_workerReadyEvent);
        }
        return 1;
    }

    // The storage and the settings come first. The remembered values are read
    // against the Startup group, so taking them before the settings would read
    // them against the defaults and quietly ignore the whole group.
    StoreInit();
    g_settings = LoadSettings();
    // Anything a previous run left on the display or the pointer is undone
    // before either is copied as this run's idea of the original.
    GammaRecoverStored();
    PointerRecoverStored();
    LoadRememberedLevels();
    ApplySettings();
    // The first frame is painted while the window is still hidden; Present is
    // what makes it appear, so nothing flashes.
    PaintPanelSurface();
    TrayAdd();
    if (g_workerReadyEvent) {
        SetEvent(g_workerReadyEvent);
    }

    MSG message;
    while (GetMessageW(&message, nullptr, 0, 0) > 0) {
        TranslateMessage(&message);
        DispatchMessageW(&message);
    }

    SaveRememberedLevels();
    DestroyWindows();
    UnregisterWindowClasses();
    g_fonts.Clear();
    if (SUCCEEDED(comInit)) {
        CoUninitialize();
    }
    return 0;
}

////////////////////////////////////////////////////////////////////////////////
// Mod entry points

BOOL WhTool_ModInit() {
    Wh_Log(L">");
    g_instance = GetModuleHandleW(nullptr);
    g_workerReadyEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_workerReadyEvent) {
        Wh_Log(L"CreateEventW failed: %u", GetLastError());
        return FALSE;
    }
    g_workerThread =
        CreateThread(nullptr, 0, WorkerThreadProc, nullptr, 0, nullptr);
    if (!g_workerThread) {
        Wh_Log(L"CreateThread failed: %u", GetLastError());
        return FALSE;
    }
    HANDLE handles[] = {g_workerReadyEvent, g_workerThread};
    WaitForMultipleObjects(ARRAYSIZE(handles), handles, FALSE, INFINITE);
    return g_messageWindow != nullptr;
}

void WhTool_ModSettingsChanged() {
    Wh_Log(L">");
    if (g_messageWindow) {
        PostMessageW(g_messageWindow, kMessageSettings, 0, 0);
    }
}

void WhTool_ModUninit() {
    Wh_Log(L">");
    if (g_messageWindow) {
        PostMessageW(g_messageWindow, WM_CLOSE, 0, 0);
    }
    if (g_workerThread) {
        WaitForSingleObject(g_workerThread, 5000);
        CloseHandle(g_workerThread);
        g_workerThread = nullptr;
    }
    if (g_workerReadyEvent) {
        CloseHandle(g_workerReadyEvent);
        g_workerReadyEvent = nullptr;
    }
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
        reinterpret_cast<CreateProcessInternalW_t>(reinterpret_cast<void*>(
            GetProcAddress(kernelModule, "CreateProcessInternalW")));
    if (!pCreateProcessInternalW) {
        Wh_Log(L"No CreateProcessInternalW");
        return;
    }

    STARTUPINFO si{};
    si.cb = sizeof(STARTUPINFO);
    si.dwFlags = STARTF_FORCEOFFFEEDBACK;
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
