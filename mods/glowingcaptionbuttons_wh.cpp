// ==WindhawkMod==
// @id              glowing-traffic-light-caption-buttons
// @name            Glowing Traffic Light Caption Buttons (macOS Style)
// @description     Replaces standard Windows titlebar buttons (Minimize, Maximize, Close) with glowing Red, Yellow, and Green traffic-light buttons.
// @version         1.0.0
// @author          Mostafa Ahmed Elshorbagy
// @github          https://github.com/elahorbagy
// @include         *
// @exclude         dwm.exe
// @exclude         csrss.exe
// @exclude         smss.exe
// @exclude         services.exe
// @exclude         lsass.exe
// @exclude         svchost.exe
// @exclude         wininit.exe
// @exclude         winlogon.exe
// @exclude         fontdrvhost.exe
// @exclude         spoolsv.exe
// @exclude         conhost.exe
// @exclude         RuntimeBroker.exe
// @exclude         backgroundTaskHost.exe
// @exclude         SearchIndexer.exe
// @exclude         StartMenuExperienceHost.exe
// @exclude         ShellExperienceHost.exe
// @exclude         SearchHost.exe
// @exclude         TextInputHost.exe
// @exclude         LockApp.exe
// @architecture    x86-64
// @compilerOptions -luser32 -lgdi32 -lmsimg32 -ldwmapi -luxtheme -lcomctl32 -lshcore
// @license         MIT
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Glowing Traffic Light Caption Buttons (v1.0)

Replaces the standard Windows 11/10 titlebar caption buttons (`—`, `□`, `✕`) with **glowing Red, Yellow, and Green traffic-light buttons** (supporting both the glowing rounded-square look and classic macOS circles).

## Key Features
- **Soft Neon Glow + Crisp Core**:
  - Renders a 32-bit premultiplied ARGB outer radial/box glow around each traffic light (Red, Yellow, Green) with a bright inner core, matching the glowing traffic-light aesthetic.
- **Seamless Native Caption Masking**:
  - Automatically samples each window's titlebar color (or uses a frosted dark capsule) to completely hide the original Windows `—`, `□`, `✕` glyphs underneath—working across standard Win32 apps, File Explorer, Notepad, and browsers without needing `dwm.exe` injection.
- **Customizable Button Order & Muscle Memory Support**:
  - **Red · Yellow · Green (Left-to-Right Visual Order, Close on Left)**
  - **Red · Yellow · Green Colors, Windows Action Order** (Keeps the Red-Yellow-Green visual colors from left to right, while making the far-right button Close the window so your Windows muscle memory still works!)
  - **Yellow (Min) · Green (Max) · Red (Close)** (Colors follow Windows actions)
- **Full Visual Customization**:
  - Choose between **Glowing Rounded Squares (Squircle)**, **Glowing Circles**, **Flat Circles**, or **Flat Rounded Squares**.
  - Adjust **Button Size**, **Glow Strength**, **Glow Spread**, **Button Spacing**, **Hover Symbols (`×`, `—`, `□`)**, **Left or Right Titlebar Placement**, and **Custom Hex Colors**.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- buttonStyle: glow_squircle
  $name: Button Shape & Style
  $options:
  - glow_squircle: Glowing Rounded Square (Matches reference image)
  - glow_circle: Glowing macOS Circle
  - flat_circle: Clean Flat macOS Circle (No outer glow)
  - flat_squircle: Clean Flat Rounded Square (No outer glow)
- buttonOrder: ryg_visual
  $name: Button Color & Action Order
  $options:
  - ryg_visual: Red (Close) · Yellow (Minimize) · Green (Maximize)
  - ryg_win_actions: Red · Yellow · Green colors, but Rightmost = Close (Windows muscle memory)
  - ygr_actions: Yellow (Minimize) · Green (Maximize) · Red (Close)
- buttonPosition: right
  $name: Titlebar Position
  $options:
  - right: Top-Right (Replaces Windows caption buttons in place)
  - left: Top-Left (macOS position)
- hoverSymbols: never
  $name: Inner Button Symbols (x, -, +)
  $options:
  - never: No symbols (Pure glowing buttons)
  - hover_only: Show symbols only when hovering
  - always: Always show symbols
- backingMode: auto_sample
  $name: Background Mask Behind Buttons
  $options:
  - auto_sample: Auto-Match Window Titlebar Color (Hides old buttons seamlessly)
  - frosted_capsule: Dark Frosted Capsule (Matches reference image backdrop)
  - custom_color: Custom Solid Hex Color (configured below)
- customBackingColor: "#141821"
  $name: Custom Backing Bar Color (Hex)
  $description: Used when Background Mask is set to Custom Solid Hex Color
- redColor: "#FF4B4B"
  $name: Red Button Color (Hex)
- yellowColor: "#FFD43B"
  $name: Yellow Button Color (Hex)
- greenColor: "#38D952"
  $name: Green Button Color (Hex)
- buttonCoreSize: 13
  $name: Button Core Size (8 to 20 px at 100% DPI)
  $description: Size of the solid inner button square/circle
- glowStrength: 88
  $name: Outer Glow Strength (0 to 100%)
  $description: Intensity of the colored neon halo around each button
- glowRadius: 11
  $name: Outer Glow Spread (2 to 20 px at 100% DPI)
  $description: How far the soft colored glow extends around each button
- buttonSpacing: 32
  $name: Center-to-Center Button Spacing (20 to 48 px at 100% DPI)
  $description: Horizontal distance between the three buttons
- barWidthPx: 138
  $name: Caption Mask Width (100 to 160 px at 100% DPI)
  $description: Total width of the caption button region (138 px covers all 3 standard Windows 11 buttons)
- barHeightPx: 31
  $name: Caption Mask Height (24 to 42 px at 100% DPI)
  $description: Height of the caption button region
- topOffsetPx: 1
  $name: Vertical Offset from Top Edge (0 to 12 px)
- sideOffsetPx: 1
  $name: Horizontal Offset from Window Edge (0 to 20 px)
- dimWhenInactive: true
  $name: Dim Glow When Window Is Inactive
  $description: Softly dims the glow when the window loses focus
*/
// ==/WindhawkModSettings==

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0A00
#endif
#ifndef WINVER
#define WINVER 0x0A00
#endif

#include <windows.h>
#include <windowsx.h>
#include <dwmapi.h>
#include <uxtheme.h>
#include <commctrl.h>
#include <shellscalingapi.h>

#ifdef __has_include
#if __has_include(<windhawk_utils.h>)
#include <windhawk_utils.h>
#define HAS_WINDHAWK_UTILS 1
#endif
#endif

#ifndef DWMWA_EXTENDED_FRAME_BOUNDS
#define DWMWA_EXTENDED_FRAME_BOUNDS 9
#endif
#ifndef DWMWA_CLOAKED
#define DWMWA_CLOAKED 14
#endif
#ifndef DWMWA_USE_IMMERSIVE_DARK_MODE
#define DWMWA_USE_IMMERSIVE_DARK_MODE 20
#endif
#ifndef DWMWA_CAPTION_BUTTON_BOUNDS
#define DWMWA_CAPTION_BUTTON_BOUNDS 5
#endif
#ifndef ULW_ALPHA
#define ULW_ALPHA 0x00000002
#endif
#ifndef AC_SRC_OVER
#define AC_SRC_OVER 0x00
#endif
#ifndef AC_SRC_ALPHA
#define AC_SRC_ALPHA 0x01
#endif

#include <algorithm>
#include <atomic>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <cwchar>
#include <cwctype>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

namespace {

constexpr wchar_t kOverlayClassName[] = L"WindhawkGlowingCaptionOverlay";
constexpr UINT_PTR kSubclassId        = 0x474C4F57; // 'GLOW'
constexpr UINT_PTR kTimerHoverAnim    = 1;

enum class ButtonStyle : int {
    GlowSquircle = 0,
    GlowCircle   = 1,
    FlatCircle   = 2,
    FlatSquircle = 3
};

enum class ButtonOrder : int {
    RYG_Visual     = 0, // Left=Red(Close), Mid=Yellow(Min), Right=Green(Max)
    RYG_WinActions = 1, // Colors: Red, Yellow, Green; Actions: Left=Min, Mid=Max, Right=Close
    YGR_Actions    = 2  // Left=Yellow(Min), Mid=Green(Max), Right=Red(Close)
};

enum class HoverSymbols : int {
    Never     = 0,
    HoverOnly = 1,
    Always    = 2
};

enum class BackingMode : int {
    AutoSample     = 0,
    FrostedCapsule = 1,
    CustomColor    = 2
};

enum class BtnAction : int {
    Close    = 0,
    Minimize = 1,
    Maximize = 2
};

struct ModConfig {
    ButtonStyle style = ButtonStyle::GlowSquircle;
    ButtonOrder order = ButtonOrder::RYG_Visual;
    bool leftSide = false;
    HoverSymbols symbols = HoverSymbols::Never;
    BackingMode backing = BackingMode::AutoSample;
    COLORREF customBackingColor = RGB(20, 24, 33);
    COLORREF redColor    = RGB(255, 75, 75);
    COLORREF yellowColor = RGB(255, 212, 59);
    COLORREF greenColor  = RGB(56, 217, 82);
    int coreSizePx = 13;
    int glowStrength = 88;
    int glowRadiusPx = 11;
    int spacingPx = 32;
    int barWidthPx = 138;
    int barHeightPx = 31;
    int topOffsetPx = 1;
    int sideOffsetPx = 1;
    bool dimWhenInactive = true;
};

struct OverlayInstance {
    HWND targetHwnd = nullptr;
    HWND overlayHwnd = nullptr;
    UINT dpi = 96;
    int width = 0;
    int height = 0;
    int hoverIdx = -1;
    int pressedIdx = -1;
    float hoverAnim[3] = {0.0f, 0.0f, 0.0f};
    bool trackingMouse = false;
    bool isTargetActive = true;
    COLORREF sampledBg = RGB(24, 28, 38);
    bool hasSampledBg = false;
};

ModConfig g_cfg;
std::mutex g_mutex;
std::unordered_map<HWND, std::unique_ptr<OverlayInstance>> g_byTarget;
std::unordered_map<HWND, OverlayInstance*> g_byOverlay;
HWINEVENTHOOK g_winEventHook = nullptr;
HWINEVENTHOOK g_fgEventHook = nullptr;
std::atomic<bool> g_shuttingDown{false};
ATOM g_overlayAtom = 0;

////////////////////////////////////////////////////////////////////////////////
// Utility & Color Helpers

int ScaleForDpi(int val, UINT dpi) {
    return MulDiv(val, static_cast<int>(dpi ? dpi : 96), 96);
}

std::wstring TrimStr(const std::wstring& s) {
    size_t a = s.find_first_not_of(L" \t\r\n\"");
    if (a == std::wstring::npos) return L"";
    size_t b = s.find_last_not_of(L" \t\r\n\"");
    return s.substr(a, b - a + 1);
}

std::wstring LowerStr(std::wstring s) {
    for (wchar_t& ch : s) ch = static_cast<wchar_t>(std::towlower(ch));
    return s;
}

COLORREF ParseHexColor(std::wstring text, COLORREF fallback) {
    text = TrimStr(text);
    if (!text.empty() && text[0] == L'#') text.erase(0, 1);
    if (text.size() != 6) return fallback;
    for (wchar_t ch : text) {
        if (!std::iswxdigit(ch)) return fallback;
    }
    unsigned long rgb = wcstoul(text.c_str(), nullptr, 16);
    return RGB((rgb >> 16) & 0xFF, (rgb >> 8) & 0xFF, rgb & 0xFF);
}

std::wstring GetSettingStr(PCWSTR name, PCWSTR fallback) {
    PCWSTR raw = Wh_GetStringSetting(name);
    if (!raw) return fallback ? fallback : L"";
    std::wstring res(raw);
    Wh_FreeStringSetting(raw);
    return res;
}

void LoadSettings() {
    ModConfig c;
    std::wstring st = LowerStr(TrimStr(GetSettingStr(L"buttonStyle", L"glow_squircle")));
    if (st == L"glow_circle") c.style = ButtonStyle::GlowCircle;
    else if (st == L"flat_circle") c.style = ButtonStyle::FlatCircle;
    else if (st == L"flat_squircle") c.style = ButtonStyle::FlatSquircle;
    else c.style = ButtonStyle::GlowSquircle;

    std::wstring ord = LowerStr(TrimStr(GetSettingStr(L"buttonOrder", L"ryg_visual")));
    if (ord == L"ryg_win_actions") c.order = ButtonOrder::RYG_WinActions;
    else if (ord == L"ygr_actions") c.order = ButtonOrder::YGR_Actions;
    else c.order = ButtonOrder::RYG_Visual;

    std::wstring pos = LowerStr(TrimStr(GetSettingStr(L"buttonPosition", L"right")));
    c.leftSide = (pos == L"left");

    std::wstring sym = LowerStr(TrimStr(GetSettingStr(L"hoverSymbols", L"never")));
    if (sym == L"hover_only") c.symbols = HoverSymbols::HoverOnly;
    else if (sym == L"always") c.symbols = HoverSymbols::Always;
    else c.symbols = HoverSymbols::Never;

    std::wstring bMode = LowerStr(TrimStr(GetSettingStr(L"backingMode", L"auto_sample")));
    if (bMode == L"frosted_capsule") c.backing = BackingMode::FrostedCapsule;
    else if (bMode == L"custom_color") c.backing = BackingMode::CustomColor;
    else c.backing = BackingMode::AutoSample;

    c.customBackingColor = ParseHexColor(GetSettingStr(L"customBackingColor", L"#141821"), RGB(20, 24, 33));
    c.redColor    = ParseHexColor(GetSettingStr(L"redColor",    L"#FF4B4B"), RGB(255, 75, 75));
    c.yellowColor = ParseHexColor(GetSettingStr(L"yellowColor", L"#FFD43B"), RGB(255, 212, 59));
    c.greenColor  = ParseHexColor(GetSettingStr(L"greenColor",  L"#38D952"), RGB(56, 217, 82));

    int core = Wh_GetIntSetting(L"buttonCoreSize");
    c.coreSizePx = (core <= 0) ? 13 : std::clamp(core, 8, 20);

    int gStr = Wh_GetIntSetting(L"glowStrength");
    c.glowStrength = (gStr < 0) ? 88 : std::clamp(gStr, 0, 100);

    int gRad = Wh_GetIntSetting(L"glowRadius");
    c.glowRadiusPx = (gRad <= 0) ? 11 : std::clamp(gRad, 2, 20);

    int sp = Wh_GetIntSetting(L"buttonSpacing");
    c.spacingPx = (sp <= 0) ? 32 : std::clamp(sp, 20, 48);

    int bw = Wh_GetIntSetting(L"barWidthPx");
    c.barWidthPx = (bw <= 0) ? 138 : std::clamp(bw, 100, 160);

    int bh = Wh_GetIntSetting(L"barHeightPx");
    c.barHeightPx = (bh <= 0) ? 31 : std::clamp(bh, 24, 42);

    c.topOffsetPx = std::clamp(Wh_GetIntSetting(L"topOffsetPx"), 0, 12);
    c.sideOffsetPx = std::clamp(Wh_GetIntSetting(L"sideOffsetPx"), 0, 20);
    c.dimWhenInactive = Wh_GetIntSetting(L"dimWhenInactive") != 0;

    g_cfg = c;
}

void GetSlotColorAndAction(int slotIndex, COLORREF& outColor, BtnAction& outAction) {
    slotIndex = std::clamp(slotIndex, 0, 2);
    switch (g_cfg.order) {
        case ButtonOrder::RYG_WinActions:
            // Visual colors: Red, Yellow, Green from left to right;
            // Actions: Minimize, Maximize, Close (so far-right closes window!)
            if (slotIndex == 0) { outColor = g_cfg.redColor;    outAction = BtnAction::Minimize; }
            if (slotIndex == 1) { outColor = g_cfg.yellowColor; outAction = BtnAction::Maximize; }
            if (slotIndex == 2) { outColor = g_cfg.greenColor;  outAction = BtnAction::Close;    }
            break;
        case ButtonOrder::YGR_Actions:
            if (slotIndex == 0) { outColor = g_cfg.yellowColor; outAction = BtnAction::Minimize; }
            if (slotIndex == 1) { outColor = g_cfg.greenColor;  outAction = BtnAction::Maximize; }
            if (slotIndex == 2) { outColor = g_cfg.redColor;    outAction = BtnAction::Close;    }
            break;
        case ButtonOrder::RYG_Visual:
        default:
            if (slotIndex == 0) { outColor = g_cfg.redColor;    outAction = BtnAction::Close;    }
            if (slotIndex == 1) { outColor = g_cfg.yellowColor; outAction = BtnAction::Minimize; }
            if (slotIndex == 2) { outColor = g_cfg.greenColor;  outAction = BtnAction::Maximize; }
            break;
    }
}

////////////////////////////////////////////////////////////////////////////////
// 32-Bit Premultiplied ARGB Pixel Renderer (Glowing Squircle & Circle Engine)

float SdfRoundedBox(float px, float py, float hx, float hy, float r) {
    float qx = std::fabs(px) - hx + r;
    float qy = std::fabs(py) - hy + r;
    float outDist = std::hypot(std::max(qx, 0.0f), std::max(qy, 0.0f));
    float inDist = std::min(std::max(qx, qy), 0.0f);
    return outDist + inDist - r;
}

void BlendPixelPremul(uint32_t& dst, int r, int g, int b, int a) {
    a = std::clamp(a, 0, 255);
    if (a <= 0) return;
    uint32_t dstA = (dst >> 24) & 0xFF;
    uint32_t dstR = (dst >> 16) & 0xFF;
    uint32_t dstG = (dst >> 8) & 0xFF;
    uint32_t dstB = dst & 0xFF;

    uint32_t srcR = (r * a + 127) / 255;
    uint32_t srcG = (g * a + 127) / 255;
    uint32_t srcB = (b * a + 127) / 255;

    uint32_t invA = 255 - a;
    uint32_t outA = a + (dstA * invA + 127) / 255;
    uint32_t outR = std::min(outA, srcR + (dstR * invA + 127) / 255);
    uint32_t outG = std::min(outA, srcG + (dstG * invA + 127) / 255);
    uint32_t outB = std::min(outA, srcB + (dstB * invA + 127) / 255);

    dst = (outA << 24) | (outR << 16) | (outG << 8) | outB;
}

void DrawAntiAliasedSegment(std::vector<uint32_t>& buf, int w, int h,
                            float x0, float y0, float x1, float y1,
                            float thickness, COLORREF color, int alpha) {
    int minX = std::max(0, static_cast<int>(std::floor(std::min(x0, x1) - thickness - 2.0f)));
    int maxX = std::min(w - 1, static_cast<int>(std::ceil(std::max(x0, x1) + thickness + 2.0f)));
    int minY = std::max(0, static_cast<int>(std::floor(std::min(y0, y1) - thickness - 2.0f)));
    int maxY = std::min(h - 1, static_cast<int>(std::ceil(std::max(y0, y1) + thickness + 2.0f)));

    float dx = x1 - x0;
    float dy = y1 - y0;
    float lenSq = dx * dx + dy * dy;
    float halfT = thickness * 0.5f;

    int r = GetRValue(color);
    int g = GetGValue(color);
    int b = GetBValue(color);

    for (int py = minY; py <= maxY; ++py) {
        for (int px = minX; px <= maxX; ++px) {
            float cx = px + 0.5f;
            float cy = py + 0.5f;
            float t = (lenSq > 0.0001f) ? std::clamp(((cx - x0) * dx + (cy - y0) * dy) / lenSq, 0.0f, 1.0f) : 0.0f;
            float projX = x0 + t * dx;
            float projY = y0 + t * dy;
            float dist = std::hypot(cx - projX, cy - projY);
            float cov = std::clamp(halfT + 0.5f - dist, 0.0f, 1.0f);
            if (cov > 0.0f) {
                BlendPixelPremul(buf[static_cast<size_t>(py) * w + px], r, g, b, static_cast<int>(alpha * cov));
            }
        }
    }
}

void RenderCaptionButtonsBuffer(std::vector<uint32_t>& outPixels, int width, int height,
                                UINT dpi, COLORREF backingColor, bool isWindowActive,
                                int hoverIdx, int pressedIdx, const float hoverAnim[3]) {
    if (width <= 0 || height <= 0) return;
    outPixels.assign(static_cast<size_t>(width) * static_cast<size_t>(height), 0);

    // 1. Render backing mask plate so original Windows (-, [], X) glyphs are 100% hidden
    COLORREF plateCol = backingColor;
    int plateAlpha = 255;
    float plateCornerRad = static_cast<float>(ScaleForDpi(4, dpi));
    if (g_cfg.backing == BackingMode::FrostedCapsule) {
        plateCol = RGB(26, 30, 38);
        plateAlpha = 248;
        plateCornerRad = static_cast<float>(ScaleForDpi(6, dpi));
    } else if (g_cfg.backing == BackingMode::CustomColor) {
        plateCol = g_cfg.customBackingColor;
        plateAlpha = 255;
    }

    int pr = GetRValue(plateCol);
    int pg = GetGValue(plateCol);
    int pb = GetBValue(plateCol);
    float halfW = width * 0.5f;
    float halfH = height * 0.5f;

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            float px = (x + 0.5f) - halfW;
            float py = (y + 0.5f) - halfH;
            float d = SdfRoundedBox(px, py, halfW - 0.5f, halfH - 0.5f, plateCornerRad);
            float cov = std::clamp(0.5f - d, 0.0f, 1.0f);
            if (cov > 0.0f) {
                int a = static_cast<int>(plateAlpha * cov);
                BlendPixelPremul(outPixels[static_cast<size_t>(y) * width + x], pr, pg, pb, a);
                if (g_cfg.backing == BackingMode::FrostedCapsule && d > -1.3f && d <= 0.2f) {
                    BlendPixelPremul(outPixels[static_cast<size_t>(y) * width + x], 90, 98, 112, static_cast<int>(90 * cov));
                }
            }
        }
    }

    // 2. Render each of the 3 traffic light buttons (Outer Neon Glow + Crisp Inner Core + Optional Symbol)
    float spacing = static_cast<float>(ScaleForDpi(g_cfg.spacingPx, dpi));
    float centerY = height * 0.5f;
    float midX    = width * 0.5f;
    float centersX[3] = { midX - spacing, midX, midX + spacing };

    bool useCircle = (g_cfg.style == ButtonStyle::GlowCircle || g_cfg.style == ButtonStyle::FlatCircle);
    bool useGlow   = (g_cfg.style == ButtonStyle::GlowSquircle || g_cfg.style == ButtonStyle::GlowCircle) &&
                     (g_cfg.glowStrength > 0);

    float baseCoreHalf = ScaleForDpi(g_cfg.coreSizePx, dpi) * 0.5f;
    float baseGlowRad  = static_cast<float>(ScaleForDpi(g_cfg.glowRadiusPx, dpi));
    float squircleRad  = std::max(2.0f, baseCoreHalf * 0.42f);

    float activeMult = (!isWindowActive && g_cfg.dimWhenInactive) ? 0.52f : 1.0f;

    for (int i = 0; i < 3; ++i) {
        COLORREF btnColor;
        BtnAction action;
        GetSlotColorAndAction(i, btnColor, action);

        float hAnim = std::clamp(hoverAnim ? hoverAnim[i] : (hoverIdx == i ? 1.0f : 0.0f), 0.0f, 1.0f);
        bool isPressed = (pressedIdx == i);

        float coreHalf = baseCoreHalf * (1.0f + 0.10f * hAnim - (isPressed ? 0.08f : 0.0f));
        float glowRad  = baseGlowRad * (1.0f + 0.22f * hAnim);
        float glowPeakAlpha = (g_cfg.glowStrength / 100.0f) * (215.0f + 40.0f * hAnim) * activeMult;

        int cr = GetRValue(btnColor);
        int cg = GetGValue(btnColor);
        int cb = GetBValue(btnColor);
        if (isPressed) {
            cr = cr * 4 / 5;
            cg = cg * 4 / 5;
            cb = cb * 4 / 5;
        }

        int coreCenterR = std::min(255, cr + 55);
        int coreCenterG = std::min(255, cg + 55);
        int coreCenterB = std::min(255, cb + 55);

        float cx = centersX[i];
        float cy = centerY;
        float maxExtent = coreHalf + (useGlow ? glowRad * 1.6f : 2.0f);

        int x0 = std::max(0, static_cast<int>(std::floor(cx - maxExtent)));
        int x1 = std::min(width - 1, static_cast<int>(std::ceil(cx + maxExtent)));
        int y0 = std::max(0, static_cast<int>(std::floor(cy - maxExtent)));
        int y1 = std::min(height - 1, static_cast<int>(std::ceil(cy + maxExtent)));

        for (int py = y0; py <= y1; ++py) {
            for (int px = x0; px <= x1; ++px) {
                float dx = (px + 0.5f) - cx;
                float dy = (py + 0.5f) - cy;

                float dist = useCircle
                                 ? (std::hypot(dx, dy) - coreHalf)
                                 : SdfRoundedBox(dx, dy, coreHalf, coreHalf, squircleRad);

                if (useGlow && dist > -1.5f) {
                    float normD = std::max(0.0f, dist) / std::max(1.0f, glowRad);
                    if (normD < 1.55f) {
                        float falloff = std::exp(-2.35f * normD * normD);
                        int ga = static_cast<int>(glowPeakAlpha * falloff);
                        if (ga > 1) {
                            BlendPixelPremul(outPixels[static_cast<size_t>(py) * width + px], cr, cg, cb, ga);
                        }
                    }
                }

                float coreCov = std::clamp(0.5f - dist, 0.0f, 1.0f);
                if (coreCov > 0.0f) {
                    float innerFactor = std::clamp(-dist / std::max(1.0f, coreHalf), 0.0f, 1.0f);
                    int rMix = static_cast<int>(cr + (coreCenterR - cr) * innerFactor * 0.65f);
                    int gMix = static_cast<int>(cg + (coreCenterG - cg) * innerFactor * 0.65f);
                    int bMix = static_cast<int>(cb + (coreCenterB - cb) * innerFactor * 0.65f);
                    int coreAlpha = static_cast<int>(255.0f * coreCov * (0.35f + 0.65f * activeMult));
                    BlendPixelPremul(outPixels[static_cast<size_t>(py) * width + px], rMix, gMix, bMix, coreAlpha);

                    if (dist > -1.4f && dist <= 0.0f) {
                        int rimA = static_cast<int>(95.0f * coreCov * activeMult);
                        BlendPixelPremul(outPixels[static_cast<size_t>(py) * width + px],
                                         std::min(255, cr + 85), std::min(255, cg + 85), std::min(255, cb + 85), rimA);
                    }
                }
            }
        }

        bool drawSym = (g_cfg.symbols == HoverSymbols::Always) ||
                       (g_cfg.symbols == HoverSymbols::HoverOnly && (hoverIdx >= 0 || hAnim > 0.05f));
        if (drawSym) {
            float symAlphaF = (g_cfg.symbols == HoverSymbols::Always) ? 0.85f : std::max(hAnim, hoverIdx >= 0 ? 0.85f : 0.0f);
            int symAlpha = static_cast<int>(220.0f * symAlphaF * activeMult);
            float rSym = coreHalf * 0.44f;
            float stroke = std::max(1.25f, ScaleForDpi(1, dpi) * 1.25f);
            COLORREF symCol = RGB(28, 18, 18);

            if (action == BtnAction::Close) {
                DrawAntiAliasedSegment(outPixels, width, height, cx - rSym, cy - rSym, cx + rSym, cy + rSym, stroke, symCol, symAlpha);
                DrawAntiAliasedSegment(outPixels, width, height, cx + rSym, cy - rSym, cx - rSym, cy + rSym, stroke, symCol, symAlpha);
            } else if (action == BtnAction::Minimize) {
                DrawAntiAliasedSegment(outPixels, width, height, cx - rSym, cy, cx + rSym, cy, stroke, symCol, symAlpha);
            } else if (action == BtnAction::Maximize) {
                DrawAntiAliasedSegment(outPixels, width, height, cx - rSym, cy - rSym, cx + rSym, cy - rSym, stroke, symCol, symAlpha);
                DrawAntiAliasedSegment(outPixels, width, height, cx + rSym, cy - rSym, cx + rSym, cy + rSym, stroke, symCol, symAlpha);
                DrawAntiAliasedSegment(outPixels, width, height, cx + rSym, cy + rSym, cx - rSym, cy + rSym, stroke, symCol, symAlpha);
                DrawAntiAliasedSegment(outPixels, width, height, cx - rSym, cy + rSym, cx - rSym, cy - rSym, stroke, symCol, symAlpha);
            }
        }
    }
}

int HitTestButtonSlot(int x, int y, int width, int height, UINT dpi) {
    if (x < 0 || x >= width || y < 0 || y >= height) return -1;
    float spacing = static_cast<float>(ScaleForDpi(g_cfg.spacingPx, dpi));
    float midX    = width * 0.5f;
    float centersX[3] = { midX - spacing, midX, midX + spacing };
    float hitHalfW = spacing * 0.5f;

    for (int i = 0; i < 3; ++i) {
        if (std::fabs(x - centersX[i]) <= hitHalfW) {
            return i;
        }
    }
    return -1;
}

////////////////////////////////////////////////////////////////////////////////
// Target Window Filtering, Positioning & Titlebar Color Sampling

bool IsEligibleTopLevelWindow(HWND hwnd) {
    if (!hwnd || !IsWindow(hwnd) || !IsWindowVisible(hwnd)) return false;
    if (GetAncestor(hwnd, GA_ROOT) != hwnd) return false;

    DWORD pid = 0;
    GetWindowThreadProcessId(hwnd, &pid);
    if (pid != GetCurrentProcessId()) return false;

    LONG_PTR style   = GetWindowLongPtrW(hwnd, GWL_STYLE);
    LONG_PTR exStyle = GetWindowLongPtrW(hwnd, GWL_EXSTYLE);

    if (style & WS_CHILD) return false;
    if (exStyle & (WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE)) return false;
    if ((style & WS_CAPTION) != WS_CAPTION) return false;
    if (!(style & WS_SYSMENU)) return false;

    DWORD cloaked = 0;
    if (SUCCEEDED(DwmGetWindowAttribute(hwnd, DWMWA_CLOAKED, &cloaked, sizeof(cloaked))) && cloaked != 0) {
        return false;
    }

    wchar_t cls[128]{};
    if (GetClassNameW(hwnd, cls, 128)) {
        if (wcscmp(cls, kOverlayClassName) == 0 ||
            wcscmp(cls, L"Shell_TrayWnd") == 0 ||
            wcscmp(cls, L"Shell_SecondaryTrayWnd") == 0 ||
            wcscmp(cls, L"Progman") == 0 ||
            wcscmp(cls, L"WorkerW") == 0 ||
            wcscmp(cls, L"MostafaWin8MetroStartScreen") == 0 ||
            wcscmp(cls, L"WindhawkFullscreenStart") == 0 ||
            wcscmp(cls, L"#32768") == 0) {
            return false;
        }
    }

    RECT wr{};
    if (!GetWindowRect(hwnd, &wr)) return false;
    if (wr.right - wr.left < 180 || wr.bottom - wr.top < 80) return false;

    return true;
}

bool GetTargetFrameRect(HWND targetHwnd, RECT& outFrame) {
    if (SUCCEEDED(DwmGetWindowAttribute(targetHwnd, DWMWA_EXTENDED_FRAME_BOUNDS, &outFrame, sizeof(outFrame)))) {
        if (outFrame.right > outFrame.left && outFrame.bottom > outFrame.top) {
            return true;
        }
    }
    return GetWindowRect(targetHwnd, &outFrame) != FALSE;
}

COLORREF DetectTitlebarColor(HWND targetHwnd, const RECT& frameRect, int overlayW, int overlayH) {
    BOOL dark = TRUE;
    DwmGetWindowAttribute(targetHwnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &dark, sizeof(dark));
    COLORREF fallback = dark ? RGB(32, 32, 32) : RGB(243, 243, 243);

    int sampleX = g_cfg.leftSide ? (frameRect.left + overlayW + 12) : (frameRect.right - overlayW - 12);
    int sampleY = frameRect.top + overlayH / 2;
    if (sampleX > frameRect.left + 8 && sampleX < frameRect.right - 8) {
        HDC screenDc = GetDC(nullptr);
        if (screenDc) {
            COLORREF c = GetPixel(screenDc, sampleX, sampleY);
            ReleaseDC(nullptr, screenDc);
            if (c != CLR_INVALID) {
                return c;
            }
        }
    }
    return fallback;
}

void PaintOverlayWindow(OverlayInstance& inst) {
    if (!inst.overlayHwnd || !IsWindow(inst.overlayHwnd)) return;
    if (inst.width <= 0 || inst.height <= 0) return;

    std::vector<uint32_t> pixels;
    RenderCaptionButtonsBuffer(pixels, inst.width, inst.height, inst.dpi,
                               inst.sampledBg, inst.isTargetActive,
                               inst.hoverIdx, inst.pressedIdx, inst.hoverAnim);

    BITMAPINFO bi{};
    bi.bmiHeader.biSize = sizeof(bi.bmiHeader);
    bi.bmiHeader.biWidth = inst.width;
    bi.bmiHeader.biHeight = -inst.height; // Top-down
    bi.bmiHeader.biPlanes = 1;
    bi.bmiHeader.biBitCount = 32;
    bi.bmiHeader.biCompression = BI_RGB;

    void* bits = nullptr;
    HDC screenDc = GetDC(nullptr);
    HDC memDc = CreateCompatibleDC(screenDc);
    HBITMAP hbmp = CreateDIBSection(memDc, &bi, DIB_RGB_COLORS, &bits, nullptr, 0);
    if (hbmp && bits) {
        memcpy(bits, pixels.data(), pixels.size() * sizeof(uint32_t));
        HGDIOBJ oldBmp = SelectObject(memDc, hbmp);

        RECT wr{};
        GetWindowRect(inst.overlayHwnd, &wr);
        POINT ptDst{wr.left, wr.top};
        POINT ptSrc{0, 0};
        SIZE sz{inst.width, inst.height};
        BLENDFUNCTION bf{AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};

        UpdateLayeredWindow(inst.overlayHwnd, screenDc, &ptDst, &sz, memDc, &ptSrc, 0, &bf, ULW_ALPHA);

        SelectObject(memDc, oldBmp);
        DeleteObject(hbmp);
    }
    DeleteDC(memDc);
    ReleaseDC(nullptr, screenDc);
}

void SyncOverlayPositionAndState(OverlayInstance& inst, bool forceRepaint = false) {
    if (!inst.targetHwnd || !IsWindow(inst.targetHwnd) || !inst.overlayHwnd) return;

    if (!IsWindowVisible(inst.targetHwnd) || IsIconic(inst.targetHwnd)) {
        if (IsWindowVisible(inst.overlayHwnd)) {
            ShowWindow(inst.overlayHwnd, SW_HIDE);
        }
        return;
    }

    DWORD cloaked = 0;
    if (SUCCEEDED(DwmGetWindowAttribute(inst.targetHwnd, DWMWA_CLOAKED, &cloaked, sizeof(cloaked))) && cloaked != 0) {
        if (IsWindowVisible(inst.overlayHwnd)) {
            ShowWindow(inst.overlayHwnd, SW_HIDE);
        }
        return;
    }

    UINT dpi = GetDpiForWindow(inst.targetHwnd);
    if (!dpi) dpi = 96;

    RECT frame{};
    if (!GetTargetFrameRect(inst.targetHwnd, frame)) return;

    int w = ScaleForDpi(g_cfg.barWidthPx, dpi);
    int h = ScaleForDpi(g_cfg.barHeightPx, dpi);

    RECT btnBounds{};
    if (!g_cfg.leftSide &&
        SUCCEEDED(DwmGetWindowAttribute(inst.targetHwnd, DWMWA_CAPTION_BUTTON_BOUNDS, &btnBounds, sizeof(btnBounds))) &&
        btnBounds.right > btnBounds.left && btnBounds.bottom > btnBounds.top) {
        int dwmW = btnBounds.right - btnBounds.left;
        int dwmH = btnBounds.bottom - btnBounds.top;
        if (dwmW >= ScaleForDpi(90, dpi) && dwmW <= ScaleForDpi(180, dpi)) w = std::max(w, dwmW);
        if (dwmH >= ScaleForDpi(22, dpi) && dwmH <= ScaleForDpi(46, dpi)) h = dwmH;
    }

    int sideOff = ScaleForDpi(g_cfg.sideOffsetPx, dpi);
    int topOff  = ScaleForDpi(g_cfg.topOffsetPx, dpi);
    if (IsZoomed(inst.targetHwnd)) {
        topOff = std::max(0, topOff - ScaleForDpi(1, dpi));
    }

    int x = g_cfg.leftSide ? (frame.left + sideOff) : (frame.right - w - sideOff);
    int y = frame.top + topOff;

    bool activeNow = (GetForegroundWindow() == inst.targetHwnd);
    bool sizeOrDpiChanged = (inst.width != w || inst.height != h || inst.dpi != dpi);
    bool activeChanged = (inst.isTargetActive != activeNow);

    inst.dpi = dpi;
    inst.width = w;
    inst.height = h;
    inst.isTargetActive = activeNow;

    if (!inst.hasSampledBg || activeChanged || forceRepaint) {
        inst.sampledBg = DetectTitlebarColor(inst.targetHwnd, frame, w, h);
        inst.hasSampledBg = true;
    }

    SetWindowPos(inst.overlayHwnd, nullptr, x, y, w, h,
                 SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_SHOWWINDOW);

    if (sizeOrDpiChanged || activeChanged || forceRepaint) {
        PaintOverlayWindow(inst);
    }
}

void ExecuteButtonAction(HWND targetHwnd, BtnAction action) {
    if (!targetHwnd || !IsWindow(targetHwnd)) return;
    switch (action) {
        case BtnAction::Close:
            PostMessageW(targetHwnd, WM_SYSCOMMAND, SC_CLOSE, 0);
            break;
        case BtnAction::Minimize:
            PostMessageW(targetHwnd, WM_SYSCOMMAND, SC_MINIMIZE, 0);
            break;
        case BtnAction::Maximize:
            PostMessageW(targetHwnd, WM_SYSCOMMAND, IsZoomed(targetHwnd) ? SC_RESTORE : SC_MAXIMIZE, 0);
            break;
    }
}

////////////////////////////////////////////////////////////////////////////////
// Overlay Window Procedure & Target Window Subclass

LRESULT CALLBACK OverlayWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    OverlayInstance* inst = nullptr;
    {
        std::lock_guard<std::mutex> lock(g_mutex);
        auto it = g_byOverlay.find(hwnd);
        if (it != g_byOverlay.end()) inst = it->second;
    }
    if (!inst) return DefWindowProcW(hwnd, msg, wParam, lParam);

    switch (msg) {
        case WM_MOUSEACTIVATE:
            return MA_NOACTIVATE;

        case WM_NCHITTEST: {
            POINT pt{GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
            ScreenToClient(hwnd, &pt);
            int slot = HitTestButtonSlot(pt.x, pt.y, inst->width, inst->height, inst->dpi);
            return (slot >= 0) ? HTCLIENT : HTTRANSPARENT;
        }

        case WM_MOUSEMOVE: {
            if (!inst->trackingMouse) {
                TRACKMOUSEEVENT tme{sizeof(tme), TME_LEAVE, hwnd, 0};
                inst->trackingMouse = TrackMouseEvent(&tme) != FALSE;
            }
            int x = GET_X_LPARAM(lParam);
            int y = GET_Y_LPARAM(lParam);
            int slot = HitTestButtonSlot(x, y, inst->width, inst->height, inst->dpi);
            if (slot != inst->hoverIdx) {
                inst->hoverIdx = slot;
                SetTimer(hwnd, kTimerHoverAnim, 14, nullptr);
                PaintOverlayWindow(*inst);
            }
            return 0;
        }

        case WM_MOUSELEAVE:
            inst->trackingMouse = false;
            inst->hoverIdx = -1;
            inst->pressedIdx = -1;
            SetTimer(hwnd, kTimerHoverAnim, 14, nullptr);
            PaintOverlayWindow(*inst);
            return 0;

        case WM_TIMER:
            if (wParam == kTimerHoverAnim) {
                bool anyMoving = false;
                for (int i = 0; i < 3; ++i) {
                    float target = (inst->hoverIdx == i) ? 1.0f : 0.0f;
                    float diff = target - inst->hoverAnim[i];
                    if (std::fabs(diff) > 0.04f) {
                        inst->hoverAnim[i] += diff * 0.35f;
                        anyMoving = true;
                    } else {
                        inst->hoverAnim[i] = target;
                    }
                }
                PaintOverlayWindow(*inst);
                if (!anyMoving) {
                    KillTimer(hwnd, kTimerHoverAnim);
                }
            }
            return 0;

        case WM_LBUTTONDOWN: {
            int x = GET_X_LPARAM(lParam);
            int y = GET_Y_LPARAM(lParam);
            int slot = HitTestButtonSlot(x, y, inst->width, inst->height, inst->dpi);
            if (slot >= 0) {
                inst->pressedIdx = slot;
                SetCapture(hwnd);
                PaintOverlayWindow(*inst);
            }
            return 0;
        }

        case WM_LBUTTONUP: {
            int x = GET_X_LPARAM(lParam);
            int y = GET_Y_LPARAM(lParam);
            if (GetCapture() == hwnd) ReleaseCapture();
            int slot = HitTestButtonSlot(x, y, inst->width, inst->height, inst->dpi);
            int wasPressed = inst->pressedIdx;
            inst->pressedIdx = -1;
            PaintOverlayWindow(*inst);

            if (wasPressed >= 0 && slot == wasPressed) {
                COLORREF dummyCol;
                BtnAction act;
                GetSlotColorAndAction(slot, dummyCol, act);
                ExecuteButtonAction(inst->targetHwnd, act);
            }
            return 0;
        }

        case WM_RBUTTONUP: {
            if (inst->targetHwnd && IsWindow(inst->targetHwnd)) {
                POINT pt{GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
                ClientToScreen(hwnd, &pt);
                PostMessageW(inst->targetHwnd, 0x0313 /*WM_POPUPSYSTEMMENU*/, 0, MAKELPARAM(pt.x, pt.y));
            }
            return 0;
        }
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

void DestroyOverlayForTarget(HWND targetHwnd) {
    HWND overlayToDestroy = nullptr;
    {
        std::lock_guard<std::mutex> lock(g_mutex);
        auto it = g_byTarget.find(targetHwnd);
        if (it != g_byTarget.end()) {
            overlayToDestroy = it->second->overlayHwnd;
            g_byOverlay.erase(overlayToDestroy);
            g_byTarget.erase(it);
        }
    }
    if (overlayToDestroy && IsWindow(overlayToDestroy)) {
        DestroyWindow(overlayToDestroy);
    }
}

LRESULT CALLBACK TargetSubclassProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam,
                                    UINT_PTR /*uIdSubclass*/, DWORD_PTR /*dwRefData*/) {
    if (g_shuttingDown.load()) {
        return DefSubclassProc(hwnd, uMsg, wParam, lParam);
    }

    switch (uMsg) {
        case WM_NCDESTROY:
            RemoveWindowSubclass(hwnd, TargetSubclassProc, kSubclassId);
            DestroyOverlayForTarget(hwnd);
            break;

        case WM_NCHITTEST: {
            LRESULT hit = DefSubclassProc(hwnd, uMsg, wParam, lParam);
            // Prevent native Windows caption buttons from highlighting underneath our overlay
            if (!g_cfg.leftSide && (hit == HTCLOSE || hit == HTMAXBUTTON || hit == HTMINBUTTON)) {
                return HTCAPTION;
            }
            return hit;
        }

        case WM_WINDOWPOSCHANGED:
        case WM_MOVE:
        case WM_SIZE:
        case WM_SHOWWINDOW:
        case WM_ACTIVATE:
        case WM_NCACTIVATE:
        case WM_STYLECHANGED:
        case WM_DPICHANGED: {
            LRESULT res = DefSubclassProc(hwnd, uMsg, wParam, lParam);
            OverlayInstance* inst = nullptr;
            {
                std::lock_guard<std::mutex> lock(g_mutex);
                auto it = g_byTarget.find(hwnd);
                if (it != g_byTarget.end()) inst = it->second.get();
            }
            if (inst) {
                SyncOverlayPositionAndState(*inst, uMsg == WM_ACTIVATE || uMsg == WM_NCACTIVATE || uMsg == WM_DPICHANGED);
            }
            return res;
        }
    }
    return DefSubclassProc(hwnd, uMsg, wParam, lParam);
}

void AttachOverlayToWindow(HWND targetHwnd) {
    if (g_shuttingDown.load() || !IsEligibleTopLevelWindow(targetHwnd)) return;

    {
        std::lock_guard<std::mutex> lock(g_mutex);
        if (g_byTarget.find(targetHwnd) != g_byTarget.end()) {
            return;
        }
    }

    WTA_OPTIONS opts{};
    opts.dwFlags = WTNCA_NODRAWICON;
    opts.dwMask  = WTNCA_NODRAWICON;
    SetWindowThemeAttribute(targetHwnd, WTA_NONCLIENT, &opts, sizeof(opts));

    HWND overlay = CreateWindowExW(
        WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
        kOverlayClassName,
        L"",
        WS_POPUP,
        0, 0, 138, 31,
        targetHwnd,
        nullptr,
        GetModuleHandleW(nullptr),
        nullptr);
    if (!overlay) return;

    auto inst = std::make_unique<OverlayInstance>();
    inst->targetHwnd = targetHwnd;
    inst->overlayHwnd = overlay;
    inst->dpi = GetDpiForWindow(targetHwnd);
    if (!inst->dpi) inst->dpi = 96;

    OverlayInstance* rawPtr = inst.get();
    {
        std::lock_guard<std::mutex> lock(g_mutex);
        g_byOverlay[overlay] = rawPtr;
        g_byTarget[targetHwnd] = std::move(inst);
    }

    SetWindowSubclass(targetHwnd, TargetSubclassProc, kSubclassId, 0);
    SyncOverlayPositionAndState(*rawPtr, true);
}

void CALLBACK WinEventProc(HWINEVENTHOOK /*hWinEventHook*/, DWORD event, HWND hwnd,
                           LONG idObject, LONG idChild, DWORD /*idEventThread*/, DWORD /*dwmsEventTime*/) {
    if (g_shuttingDown.load() || !hwnd || idObject != OBJID_WINDOW || idChild != CHILDID_SELF) {
        return;
    }

    if (event == EVENT_OBJECT_DESTROY) {
        DestroyOverlayForTarget(hwnd);
        return;
    }

    if (event == EVENT_SYSTEM_FOREGROUND) {
        std::vector<OverlayInstance*> list;
        {
            std::lock_guard<std::mutex> lock(g_mutex);
            for (auto& kv : g_byTarget) list.push_back(kv.second.get());
        }
        for (OverlayInstance* inst : list) {
            SyncOverlayPositionAndState(*inst, false);
        }
        if (IsEligibleTopLevelWindow(hwnd)) {
            AttachOverlayToWindow(hwnd);
        }
        return;
    }

    if (event == EVENT_OBJECT_SHOW || event == EVENT_OBJECT_LOCATIONCHANGE) {
        OverlayInstance* existing = nullptr;
        {
            std::lock_guard<std::mutex> lock(g_mutex);
            auto it = g_byTarget.find(hwnd);
            if (it != g_byTarget.end()) existing = it->second.get();
        }
        if (existing) {
            SyncOverlayPositionAndState(*existing, false);
        } else if (IsEligibleTopLevelWindow(hwnd)) {
            AttachOverlayToWindow(hwnd);
        }
    }
}

void ScanCurrentProcessWindows() {
    DWORD currentPid = GetCurrentProcessId();
    EnumWindows([](HWND hwnd, LPARAM lParam) -> BOOL {
        DWORD pid = 0;
        GetWindowThreadProcessId(hwnd, &pid);
        if (pid == static_cast<DWORD>(lParam) && IsEligibleTopLevelWindow(hwnd)) {
            AttachOverlayToWindow(hwnd);
        }
        return TRUE;
    }, static_cast<LPARAM>(currentPid));
}

void RefreshAllOverlays() {
    std::vector<OverlayInstance*> list;
    {
        std::lock_guard<std::mutex> lock(g_mutex);
        for (auto& kv : g_byTarget) {
            kv.second->hasSampledBg = false;
            list.push_back(kv.second.get());
        }
    }
    for (OverlayInstance* inst : list) {
        SyncOverlayPositionAndState(*inst, true);
    }
}

} // namespace

BOOL Wh_ModInit() {
    g_shuttingDown = false;
    LoadSettings();

    WNDCLASSEXW wc{};
    wc.cbSize = sizeof(wc);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = OverlayWndProc;
    wc.hInstance = GetModuleHandleW(nullptr);
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    wc.lpszClassName = kOverlayClassName;
    g_overlayAtom = RegisterClassExW(&wc);

    g_winEventHook = SetWinEventHook(
        EVENT_OBJECT_CREATE, EVENT_OBJECT_LOCATIONCHANGE,
        nullptr, WinEventProc,
        GetCurrentProcessId(), 0,
        WINEVENT_OUTOFCONTEXT);

    g_fgEventHook = SetWinEventHook(
        EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND,
        nullptr, WinEventProc,
        0, 0,
        WINEVENT_OUTOFCONTEXT);

    ScanCurrentProcessWindows();
    Wh_Log(L"Glowing Traffic Light Caption Buttons v1.0 initialized in PID %lu.", GetCurrentProcessId());
    return TRUE;
}

void Wh_ModUninit() {
    g_shuttingDown = true;
    if (g_winEventHook) {
        UnhookWinEvent(g_winEventHook);
        g_winEventHook = nullptr;
    }
    if (g_fgEventHook) {
        UnhookWinEvent(g_fgEventHook);
        g_fgEventHook = nullptr;
    }

    std::vector<std::pair<HWND, HWND>> pairs;
    {
        std::lock_guard<std::mutex> lock(g_mutex);
        for (auto& kv : g_byTarget) {
            pairs.emplace_back(kv.first, kv.second->overlayHwnd);
        }
        g_byOverlay.clear();
        g_byTarget.clear();
    }

    for (const auto& p : pairs) {
        if (p.first && IsWindow(p.first)) {
            RemoveWindowSubclass(p.first, TargetSubclassProc, kSubclassId);
        }
        if (p.second && IsWindow(p.second)) {
            DestroyWindow(p.second);
        }
    }

    if (g_overlayAtom) {
        UnregisterClassW(kOverlayClassName, GetModuleHandleW(nullptr));
        g_overlayAtom = 0;
    }
    Wh_Log(L"Glowing Traffic Light Caption Buttons v1.0 unloaded.");
}

void Wh_ModSettingsChanged() {
    LoadSettings();
    ScanCurrentProcessWindows();
    RefreshAllOverlays();
}
